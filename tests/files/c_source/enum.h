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
 * This file was generated by pbtools version 0.1.0 Thu Jan 24 08:14:06 2019.
 */

#ifndef ENUM_H
#define ENUM_H

#include <stdint.h>
#include <stdbool.h>

struct enum_heap_t {
    char *buf_p;
    int size;
    int pos;
};

/**
 * Message Message in package enum.
 */
enum enum_message_enum_e {
                          enum_message_enum_a_e = 0,
                          enum_message_enum_b_e = 1
};

struct enum_message_t {
    struct enum_heap_t *heap_p;
    enum enum_message_enum_e value;
};

/**
 * Create a new message Message in given workspace.
 *
 * @param[in] src_p Data to decode.
 * @param[in] size Size of src_p.
 *
 * @return Initialized address book, or NULL on failure.
 */
struct enum_message_t *enum_message_new(
    void *workspace_p,
    size_t size);

/**
 * Encode message Message defined in package enum.
 *
 * @param[out] dst_p Buffer to encode into.
 * @param[in] size Size of dst_p.
 * @param[in] src_p Data to encode.
 *
 * @return Encoded data length or negative error code.
 */
int enum_message_encode(
    struct enum_message_t *self_p,
    uint8_t *encoded_p,
    size_t size);

/**
 * Decode message Message defined in package enum.
 *
 * @param[out] dst_p Decoded data.
 * @param[in] src_p Data to decode.
 * @param[in] size Size of src_p.
 *
 * @return Number of bytes decoded or negative error code.
 */
int enum_message_decode(
    struct enum_message_t *self_p,
    const uint8_t *encoded_p,
    size_t size);

#endif
