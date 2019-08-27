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
 * This file was generated by pbtools version 0.2.0 Tue Aug 27 22:08:14 2019.
 */

#include <string.h>

#include "float.h"

static void float_message_encode_inner(
    struct float_message_t *self_p,
    struct pbtools_encoder_t *encoder_p)
{
    pbtools_encoder_write_float(encoder_p, 1, self_p->value);
}

static void float_message_decode_inner(
    struct float_message_t *self_p,
    struct pbtools_decoder_t *decoder_p)
{
    int wire_type;

    while (pbtools_decoder_available(decoder_p)) {
        switch (pbtools_decoder_read_tag(decoder_p, &wire_type)) {

        case 1:
            self_p->value = pbtools_decoder_read_float(decoder_p, wire_type);
            break;

        default:
            break;
        }
    }
}

struct float_message_t *float_message_new(
    void *workspace_p,
    size_t size)
{
    struct float_message_t *self_p;
    struct pbtools_heap_t *heap_p;

    heap_p = pbtools_heap_new(workspace_p, size);

    if (heap_p == NULL) {
        return (NULL);
    }

    self_p = pbtools_heap_alloc(heap_p, sizeof(*self_p));

    if (self_p != NULL) {
        self_p->heap_p = heap_p;
        self_p->value = 0;
    }

    return (self_p);
}

int float_message_encode(
    struct float_message_t *self_p,
    uint8_t *encoded_p,
    size_t size)
{
    struct pbtools_encoder_t encoder;

    pbtools_encoder_init(&encoder, encoded_p, size);
    float_message_encode_inner(self_p, &encoder);

    return (pbtools_encoder_get_result(&encoder));
}

int float_message_decode(
    struct float_message_t *self_p,
    const uint8_t *encoded_p,
    size_t size)
{
    struct pbtools_decoder_t decoder;

    pbtools_decoder_init(&decoder, encoded_p, size, self_p->heap_p);
    float_message_decode_inner(self_p, &decoder);

    return (pbtools_decoder_get_result(&decoder));
}
