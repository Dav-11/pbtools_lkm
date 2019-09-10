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

/**
 * This file was generated by pbtools.
 */

#include <limits.h>
#include "scalar_value_types.h"

#if CHAR_BIT != 8
#    error "Number of bits in a char must be 8."
#endif

void scalar_value_types_message_init(
    struct scalar_value_types_message_t *self_p,
    struct pbtools_heap_t *heap_p,
    struct scalar_value_types_message_t *next_p);

void scalar_value_types_message_encode_inner(
    struct pbtools_encoder_t *encoder_p,
    struct scalar_value_types_message_t *self_p);

void scalar_value_types_message_decode_inner(
    struct pbtools_decoder_t *decoder_p,
    struct scalar_value_types_message_t *self_p);

void scalar_value_types_message_init(
    struct scalar_value_types_message_t *self_p,
    struct pbtools_heap_t *heap_p,
    struct scalar_value_types_message_t *next_p)
{
    self_p->base.heap_p = heap_p;
    self_p->base.next_p = &next_p->base;
    self_p->v1 = 0;
    pbtools_bytes_init(&self_p->v2);
    self_p->v3 = 0;
    self_p->v4 = 0;
    self_p->v5 = 0;
    self_p->v6 = 0;
    self_p->v7 = 0;
    self_p->v8 = 0;
    self_p->v9 = 0;
    self_p->v10 = 0;
    self_p->v11 = 0;
    self_p->v12 = 0;
    self_p->v13_p = "";
    self_p->v14 = 0;
    self_p->v15 = 0;
}

void scalar_value_types_message_encode_inner(
    struct pbtools_encoder_t *encoder_p,
    struct scalar_value_types_message_t *self_p)
{
    pbtools_encoder_write_uint64(encoder_p, 15, self_p->v15);
    pbtools_encoder_write_uint32(encoder_p, 14, self_p->v14);
    pbtools_encoder_write_string(encoder_p, 13, self_p->v13_p);
    pbtools_encoder_write_sint64(encoder_p, 12, self_p->v12);
    pbtools_encoder_write_sint32(encoder_p, 11, self_p->v11);
    pbtools_encoder_write_sfixed64(encoder_p, 10, self_p->v10);
    pbtools_encoder_write_sfixed32(encoder_p, 9, self_p->v9);
    pbtools_encoder_write_int64(encoder_p, 8, self_p->v8);
    pbtools_encoder_write_int32(encoder_p, 7, self_p->v7);
    pbtools_encoder_write_float(encoder_p, 6, self_p->v6);
    pbtools_encoder_write_fixed64(encoder_p, 5, self_p->v5);
    pbtools_encoder_write_fixed32(encoder_p, 4, self_p->v4);
    pbtools_encoder_write_double(encoder_p, 3, self_p->v3);
    pbtools_encoder_write_bytes(encoder_p, 2, &self_p->v2);
    pbtools_encoder_write_bool(encoder_p, 1, self_p->v1);
}

void scalar_value_types_message_decode_inner(
    struct pbtools_decoder_t *decoder_p,
    struct scalar_value_types_message_t *self_p)
{
    int wire_type;

    while (pbtools_decoder_available(decoder_p)) {
        switch (pbtools_decoder_read_tag(decoder_p, &wire_type)) {

        case 1:
            self_p->v1 = pbtools_decoder_read_bool(decoder_p, wire_type);
            break;

        case 2:
            pbtools_decoder_read_bytes(decoder_p, wire_type, &self_p->v2);
            break;

        case 3:
            self_p->v3 = pbtools_decoder_read_double(decoder_p, wire_type);
            break;

        case 4:
            self_p->v4 = pbtools_decoder_read_fixed32(decoder_p, wire_type);
            break;

        case 5:
            self_p->v5 = pbtools_decoder_read_fixed64(decoder_p, wire_type);
            break;

        case 6:
            self_p->v6 = pbtools_decoder_read_float(decoder_p, wire_type);
            break;

        case 7:
            self_p->v7 = pbtools_decoder_read_int32(decoder_p, wire_type);
            break;

        case 8:
            self_p->v8 = pbtools_decoder_read_int64(decoder_p, wire_type);
            break;

        case 9:
            self_p->v9 = pbtools_decoder_read_sfixed32(decoder_p, wire_type);
            break;

        case 10:
            self_p->v10 = pbtools_decoder_read_sfixed64(decoder_p, wire_type);
            break;

        case 11:
            self_p->v11 = pbtools_decoder_read_sint32(decoder_p, wire_type);
            break;

        case 12:
            self_p->v12 = pbtools_decoder_read_sint64(decoder_p, wire_type);
            break;

        case 13:
            pbtools_decoder_read_string(decoder_p, wire_type, &self_p->v13_p);
            break;

        case 14:
            self_p->v14 = pbtools_decoder_read_uint32(decoder_p, wire_type);
            break;

        case 15:
            self_p->v15 = pbtools_decoder_read_uint64(decoder_p, wire_type);
            break;

        default:
            pbtools_decoder_skip_field(decoder_p, wire_type);
            break;
        }
    }
}

struct scalar_value_types_message_t *
scalar_value_types_message_new(
    void *workspace_p,
    size_t size)
{
    return (pbtools_message_new(
                workspace_p,
                size,
                sizeof(struct scalar_value_types_message_t),
                (pbtools_message_init_t)scalar_value_types_message_init));
}

int scalar_value_types_message_encode(
    struct scalar_value_types_message_t *self_p,
    uint8_t *encoded_p,
    size_t size)
{
    return (pbtools_message_encode(
                &self_p->base,
                encoded_p,
                size,
                (pbtools_message_encode_inner_t)scalar_value_types_message_encode_inner));
}

int scalar_value_types_message_decode(
    struct scalar_value_types_message_t *self_p,
    const uint8_t *encoded_p,
    size_t size)
{
    return (pbtools_message_decode(
                &self_p->base,
                encoded_p,
                size,
                (pbtools_message_decode_inner_t)scalar_value_types_message_decode_inner));
}
