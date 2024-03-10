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

#ifndef PBTOOLS_H
#define PBTOOLS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <string.h>


/* Compile time configuration. */

/* Support for float and double if 1. */
#ifndef PBTOOLS_CONFIG_FLOAT
#    define PBTOOLS_CONFIG_FLOAT                                1
#endif

/* Support for 64-bit numbers if 1, otherwise limit to 32-bit. */
#ifndef PBTOOLS_CONFIG_64BIT
#    define PBTOOLS_CONFIG_64BIT                                1
#endif

/* Errors. */
#define PBTOOLS_BAD_WIRE_TYPE                                   1
#define PBTOOLS_OUT_OF_DATA                                     2
#define PBTOOLS_ENCODE_BUFFER_FULL                              4
#define PBTOOLS_BAD_FIELD_NUMBER                                5
#define PBTOOLS_VARINT_OVERFLOW                                 6
#define PBTOOLS_SEEK_OVERFLOW                                   7
#define PBTOOLS_LENGTH_DELIMITED_OVERFLOW                       8

/* Wire types. */
#define PBTOOLS_WIRE_TYPE_VARINT                                0
#define PBTOOLS_WIRE_TYPE_64_BIT                                1
#define PBTOOLS_WIRE_TYPE_LENGTH_DELIMITED                      2
#define PBTOOLS_WIRE_TYPE_32_BIT                                5

struct pbtools_heap_t {
    char *buf_p;
    int size;
    int pos;
};

struct pbtools_encoder_t {
    uint8_t *buf_p;
    int size;
    int pos;
};

struct pbtools_decoder_t {
    const uint8_t *buf_p;
    int size;
    int pos;
    struct pbtools_heap_t *heap_p;
};

struct pbtools_repeated_info_t {
    struct pbtools_decoder_t decoder;
    int length;
    int tag;
    int wire_type;
};

struct pbtools_message_base_t {
    struct pbtools_heap_t *heap_p;
};

struct pbtools_repeated_message_t {
    int length;
    void *items_p;
};

typedef void (*pbtools_message_init_t)(void *self_p,
                                       struct pbtools_heap_t *heap_p);

typedef void (*pbtools_message_encode_inner_t)(
    struct pbtools_encoder_t *encoder_p,
    void *self_p);

typedef void (*pbtools_message_decode_inner_t)(
    struct pbtools_decoder_t *decoder_p,
    void *self_p);

void pbtools_encoder_write_int32(struct pbtools_encoder_t *self_p,
                                 int field_number,
                                 int32_t value);

bool pbtools_decoder_available(struct pbtools_decoder_t *self_p);

int pbtools_decoder_read_tag(struct pbtools_decoder_t *self_p,
                             int *wire_type_p);

int32_t pbtools_decoder_read_int32(struct pbtools_decoder_t *self_p,
                                   int wire_type);

int64_t pbtools_decoder_read_int64(struct pbtools_decoder_t *self_p,
                                   int wire_type);


void pbtools_decoder_skip_field(struct pbtools_decoder_t *self_p,
                                int wire_type);

void *pbtools_message_new(void *workspace_p,
                          size_t size,
                          size_t message_size,
                          pbtools_message_init_t message_init);

int pbtools_message_encode(struct pbtools_message_base_t *self_p,
                           uint8_t *encoded_p,
                           size_t size,
                           pbtools_message_encode_inner_t message_encode_inner);

int pbtools_message_decode(struct pbtools_message_base_t *self_p,
                           const uint8_t *encoded_p,
                           size_t size,
                           pbtools_message_decode_inner_t message_decode_inner);

int pbtools_alloc_repeated(
    struct pbtools_repeated_message_t *repeated_p,
    int length,
    struct pbtools_heap_t *heap_p,
    size_t item_size,
    pbtools_message_init_t message_init);

void pbtools_encode_repeated_inner(
    struct pbtools_encoder_t *encoder_p,
    int field_number,
    struct pbtools_repeated_message_t *repeated_p,
    size_t item_size,
    pbtools_message_encode_inner_t message_encode_inner);

void pbtools_decode_repeated_inner(
    struct pbtools_decoder_t *decoder_p,
    struct pbtools_repeated_info_t *repeated_info_p,
    struct pbtools_repeated_message_t *repeated_p,
    size_t item_size,
    pbtools_message_init_t message_init,
    pbtools_message_decode_inner_t message_decode_inner);

#ifdef __cplusplus
}
#endif

#endif