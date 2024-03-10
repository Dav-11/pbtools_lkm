/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Erik Moqvist
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <limits.h>
#include <stdalign.h>
#include "pbtools.h"

static uint64_t decoder_read_length_delimited(struct pbtools_decoder_t *self_p, int wire_type);

static void decoder_init_slice(struct pbtools_decoder_t *self_p, struct pbtools_decoder_t *parent_p, int size);

static void decoder_seek(struct pbtools_decoder_t *self_p, int offset);

static struct pbtools_heap_t *heap_new(void *buf_p, size_t size) {
    struct pbtools_heap_t *heap_p;

    if (size >= sizeof(*heap_p)) {
        heap_p = (struct pbtools_heap_t *) buf_p;
        heap_p->buf_p = buf_p;
        heap_p->size = (int) size;
        heap_p->pos = sizeof(*heap_p);
    } else {
        heap_p = NULL;
    }

    return (heap_p);
}


static void *heap_alloc(struct pbtools_heap_t *self_p, size_t size, size_t alignment) {
    uintptr_t addr;
    int left;
    size_t rest;
    size_t pad;

    left = (self_p->size - self_p->pos);

    if ((int) size <= left) {
        addr = (uintptr_t) &self_p->buf_p[self_p->pos];
        rest = (addr & (alignment - 1));

        if (rest == 0) {
            self_p->pos += (int) size;
        } else {
            pad = (alignment - rest);
            size += pad;

            if ((int) size <= left) {
                addr += pad;
                self_p->pos += (int) size;
            } else {
                addr = (uintptr_t) NULL;
            }
        }
    } else {
        addr = (uintptr_t) NULL;
    }

    return ((void *) addr);
}

static void encoder_init(struct pbtools_encoder_t *self_p, uint8_t *buf_p, size_t size) {
    self_p->buf_p = buf_p;
    self_p->size = (int) size;
    self_p->pos = ((int) size - 1);
}

static int encoder_get_result(struct pbtools_encoder_t *self_p) {
    int length;

    if (self_p->pos >= 0) {
        length = (self_p->size - self_p->pos - 1);
        memmove(self_p->buf_p,
                &self_p->buf_p[self_p->pos + 1],
                (size_t) length);
    } else {
        length = self_p->pos;
    }

    return (length);
}

static void encoder_abort(struct pbtools_encoder_t *self_p, int error) {
    if (self_p->size >= 0) {
        self_p->size = -error;
        self_p->pos = -error;
    }
}

static void encoder_write(struct pbtools_encoder_t *self_p, uint8_t *buf_p, int size) {
    if (self_p->pos >= size) {
        self_p->pos -= size;
        memcpy(&self_p->buf_p[self_p->pos + 1], buf_p, (size_t) size);
    } else {
        encoder_abort(self_p, PBTOOLS_ENCODE_BUFFER_FULL);
    }
}

static void encoder_write_varint(struct pbtools_encoder_t *self_p, uint64_t value) {
    uint8_t buf[10];
    int pos;

    pos = 0;

    do {
        buf[pos++] = (uint8_t) (value | 0x80);
        value >>= 7;
    } while (value > 0);

    buf[pos - 1] &= 0x7f;
    encoder_write(self_p, &buf[0], pos);
}

static void encoder_write_tag(struct pbtools_encoder_t *self_p, int field_number, int wire_type) {
    encoder_write_varint(
            self_p,
            (uint64_t) ((uint32_t) field_number << 3) | (uint32_t) wire_type);
}

static void
encoder_write_tagged_varint(struct pbtools_encoder_t *self_p, int field_number, int wire_type, uint64_t value) {
    if (value > 0) {
        encoder_write_varint(self_p, value);
        encoder_write_tag(self_p, field_number, wire_type);
    }
}

static void encoder_write_length_delimited(struct pbtools_encoder_t *self_p, int field_number, uint64_t value) {
    encoder_write_varint(self_p, value);
    encoder_write_tag(self_p, field_number, PBTOOLS_WIRE_TYPE_LENGTH_DELIMITED);
}

/* PUBLIC FUNC */

void pbtools_encoder_write_int32(struct pbtools_encoder_t *self_p, int field_number, int32_t value) {
    encoder_write_tagged_varint(self_p,
                                field_number,
                                PBTOOLS_WIRE_TYPE_VARINT,
                                (uint64_t) (int64_t) value);
}

static void
decoder_init(struct pbtools_decoder_t *self_p, const uint8_t *buf_p, size_t size, struct pbtools_heap_t *heap_p) {
    self_p->buf_p = buf_p;
    self_p->size = (int) size;
    self_p->pos = 0;
    self_p->heap_p = heap_p;
}

static int decoder_get_result(struct pbtools_decoder_t *self_p) {
    return (self_p->pos);
}

static void decoder_abort(struct pbtools_decoder_t *self_p, int error) {
    if (self_p->pos >= 0) {
        self_p->size = -error;
        self_p->pos = -error;
    }
}

bool pbtools_decoder_available(struct pbtools_decoder_t *self_p) {
    return (self_p->pos < self_p->size);
}

static uint8_t decoder_get(struct pbtools_decoder_t *self_p) {
    uint8_t value;

    if (pbtools_decoder_available(self_p)) {
        value = self_p->buf_p[self_p->pos];
        self_p->pos++;
    } else {
        decoder_abort(self_p, PBTOOLS_OUT_OF_DATA);
        value = 0;
    }

    return (value);
}

static uint64_t decoder_read_varint(struct pbtools_decoder_t *self_p) {
    uint64_t value;
    uint8_t byte;
    int offset;

    value = 0;
    offset = 0;

    do {
        byte = decoder_get(self_p);
        value |= (((uint64_t) byte & 0x7f) << offset);
        offset += 7;
    } while ((offset < 64) && (byte & 0x80));

    if (byte & 0x80) {
        decoder_abort(self_p, PBTOOLS_VARINT_OVERFLOW);
        value = 0;
    }

    return (value);
}

static uint64_t read_varint_check_wire_type(
        struct pbtools_decoder_t *self_p,
        int wire_type,
        int expected_wire_type) {
    if (wire_type != expected_wire_type) {
        decoder_abort(self_p, PBTOOLS_BAD_WIRE_TYPE);

        return (0);
    }

    return (decoder_read_varint(self_p));
}

static uint64_t read_varint_check_wire_type_varint(
        struct pbtools_decoder_t *self_p,
        int wire_type) {
    return (read_varint_check_wire_type(
            self_p,
            wire_type,
            PBTOOLS_WIRE_TYPE_VARINT));
}

static uint64_t decoder_read_length_delimited(struct pbtools_decoder_t *self_p,
                                              int wire_type) {
    uint64_t length;

    length = read_varint_check_wire_type(
            self_p,
            wire_type,
            PBTOOLS_WIRE_TYPE_LENGTH_DELIMITED);

    if (length >= INT_MAX) {
        decoder_abort(self_p, PBTOOLS_LENGTH_DELIMITED_OVERFLOW);
        length = 0;
    }

    return (length);
}

int pbtools_decoder_read_tag(struct pbtools_decoder_t *self_p,
                             int *wire_type_p) {
    uint32_t value;
    int field_number;

    value = (uint32_t) decoder_read_varint(self_p);
    field_number = (int) (value >> 3);
    *wire_type_p = (value & 0x7);

    if (field_number == 0) {
        decoder_abort(self_p, PBTOOLS_BAD_FIELD_NUMBER);
    }

    return (field_number);
}

int32_t pbtools_decoder_read_int32(struct pbtools_decoder_t *self_p,
                                   int wire_type) {
    return ((int32_t) pbtools_decoder_read_int64(self_p, wire_type));
}

int64_t pbtools_decoder_read_int64(struct pbtools_decoder_t *self_p,
                                   int wire_type) {
    return ((int64_t) read_varint_check_wire_type_varint(
            self_p,
            wire_type));
}

static bool are_more_items_to_decode(struct pbtools_repeated_info_t *repeated_info_p,
                                     int pos) {
    return (pbtools_decoder_available(&repeated_info_p->decoder)
            && (pos < repeated_info_p->length));
}

static void decoder_init_slice(struct pbtools_decoder_t *self_p,
                               struct pbtools_decoder_t *parent_p,
                               int size) {
    self_p->buf_p = &parent_p->buf_p[parent_p->pos];

    if (size <= (parent_p->size - parent_p->pos)) {
        self_p->size = size;
        self_p->pos = 0;
    } else {
        self_p->size = -PBTOOLS_OUT_OF_DATA;
        self_p->pos = -PBTOOLS_OUT_OF_DATA;
    }

    self_p->heap_p = parent_p->heap_p;
}

static void decoder_seek(struct pbtools_decoder_t *self_p,
                         int offset) {
    if (self_p->pos < 0) {
        return;
    }

    if (offset < 0) {
        decoder_abort(self_p, -offset);
    } else if (((unsigned int) self_p->pos + (unsigned int) offset) > INT_MAX) {
        decoder_abort(self_p, PBTOOLS_SEEK_OVERFLOW);
    } else {
        self_p->pos += offset;

        if (self_p->pos > self_p->size) {
            decoder_abort(self_p, PBTOOLS_OUT_OF_DATA);
        }
    }
}

void pbtools_decoder_skip_field(struct pbtools_decoder_t *self_p,
                                int wire_type) {
    uint64_t value;

    switch (wire_type) {

        case PBTOOLS_WIRE_TYPE_VARINT:
            (void) decoder_read_varint(self_p);
            break;

        case PBTOOLS_WIRE_TYPE_64_BIT:
            decoder_seek(self_p, 8);
            break;

        case PBTOOLS_WIRE_TYPE_LENGTH_DELIMITED:
            value = decoder_read_length_delimited(self_p, wire_type);
            decoder_seek(self_p, (int) (int64_t) value);
            break;

        case PBTOOLS_WIRE_TYPE_32_BIT:
            decoder_seek(self_p, 4);
            break;

        default:
            decoder_abort(self_p, PBTOOLS_BAD_WIRE_TYPE);
            break;
    }
}

void *pbtools_message_new(
        void *workspace_p,
        size_t size,
        size_t message_size,
        pbtools_message_init_t message_init) {
    struct pbtools_message_base_t *self_p;
    struct pbtools_heap_t *heap_p;

    heap_p = heap_new(workspace_p, size);

    if (heap_p == NULL) {
        return (NULL);
    }

    self_p = heap_alloc(heap_p,
                        message_size,
                        alignof(struct pbtools_message_base_t));

    if (self_p != NULL) {
        message_init(self_p, heap_p);
    }

    return (self_p);
}

int pbtools_message_encode(
        struct pbtools_message_base_t *self_p,
        uint8_t *encoded_p,
        size_t size,
        pbtools_message_encode_inner_t message_encode_inner) {
    struct pbtools_encoder_t encoder;

    encoder_init(&encoder, encoded_p, size);
    message_encode_inner(&encoder, self_p);

    return (encoder_get_result(&encoder));
}

int pbtools_message_decode(
        struct pbtools_message_base_t *self_p,
        const uint8_t *encoded_p,
        size_t size,
        pbtools_message_decode_inner_t message_decode_inner) {
    struct pbtools_decoder_t decoder;

    decoder_init(&decoder, encoded_p, size, self_p->heap_p);
    message_decode_inner(&decoder, self_p);

    return (decoder_get_result(&decoder));
}

int pbtools_alloc_repeated(
        struct pbtools_repeated_message_t *repeated_p,
        int length,
        struct pbtools_heap_t *heap_p,
        size_t item_size,
        pbtools_message_init_t message_init) {
    int i;
    char *item_p;

    repeated_p->items_p = heap_alloc(heap_p,
                                     item_size * (size_t) length,
                                     alignof(uint64_t));

    if (repeated_p->items_p == NULL) {
        return (-1);
    }

    item_p = repeated_p->items_p;

    for (i = 0; i < length; i++) {
        message_init(item_p, heap_p);
        item_p += item_size;
    }

    repeated_p->length = length;

    return (0);
}

void pbtools_encode_repeated_inner(
        struct pbtools_encoder_t *encoder_p,
        int field_number,
        struct pbtools_repeated_message_t *repeated_p,
        size_t item_size,
        pbtools_message_encode_inner_t message_encode_inner) {
    int i;
    int pos;
    char *item_p;

    item_p = repeated_p->items_p;
    item_p += ((size_t) repeated_p->length * item_size);

    for (i = 0; i < repeated_p->length; i++) {
        item_p -= item_size;
        pos = encoder_p->pos;
        message_encode_inner(encoder_p, item_p);
        encoder_write_length_delimited(encoder_p,
                                       field_number,
                                       (uint64_t) (pos - encoder_p->pos));
    }
}

static void decode_repeated_inner_item(
        struct pbtools_repeated_info_t *repeated_info_p,
        size_t item_size,
        pbtools_message_decode_inner_t message_decode_inner,
        char **item_pp,
        int *length_p) {
    int size;
    struct pbtools_decoder_t decoder;

    size = (int) decoder_read_length_delimited(&repeated_info_p->decoder,
                                               PBTOOLS_WIRE_TYPE_LENGTH_DELIMITED);
    decoder_init_slice(&decoder, &repeated_info_p->decoder, size);
    message_decode_inner(&decoder, *item_pp);
    decoder_seek(&repeated_info_p->decoder, decoder_get_result(&decoder));
    *item_pp += item_size;
    (*length_p)++;
}

void pbtools_decode_repeated_inner(
        struct pbtools_decoder_t *decoder_p,
        struct pbtools_repeated_info_t *repeated_info_p,
        struct pbtools_repeated_message_t *repeated_p,
        size_t item_size,
        pbtools_message_init_t message_init,
        pbtools_message_decode_inner_t message_decode_inner) {
    int res;
    int wire_type;
    int tag;
    int length;
    char *item_p;

    if (repeated_info_p->length == 0) {
        return;
    }

    res = pbtools_alloc_repeated(repeated_p,
                                 repeated_info_p->length,
                                 repeated_info_p->decoder.heap_p,
                                 item_size,
                                 message_init);

    if (res != 0) {
        return;
    }

    length = 0;
    item_p = repeated_p->items_p;
    decode_repeated_inner_item(repeated_info_p,
                               item_size,
                               message_decode_inner,
                               &item_p,
                               &length);

    while (are_more_items_to_decode(repeated_info_p, length)) {
        tag = pbtools_decoder_read_tag(&repeated_info_p->decoder, &wire_type);

        if (tag == repeated_info_p->tag) {
            decode_repeated_inner_item(repeated_info_p,
                                       item_size,
                                       message_decode_inner,
                                       &item_p,
                                       &length);
        } else {
            pbtools_decoder_skip_field(&repeated_info_p->decoder, wire_type);
        }
    }

    res = decoder_get_result(&repeated_info_p->decoder);

    if (res < 0) {
        decoder_abort(decoder_p, -res);
    }
}


