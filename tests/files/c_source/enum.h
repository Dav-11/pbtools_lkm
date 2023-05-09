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

#ifndef ENUM_H
#define ENUM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "pbtools.h"

/**
 * Enum enum.Enum.
 */
enum enum_enum_e {
    enum_c_e = 0,
    enum_d_e = 1
};

/**
 * Enum enum.Message.Enum.
 */
enum enum_message_enum_e {
    enum_message_a_e = 0,
    enum_message_b_e = 1
};

/**
 * Message enum.Message.
 */
struct enum_message_repeated_t {
    int length;
    struct enum_message_t *items_p;
};

struct enum_message_t {
    struct pbtools_message_base_t base;
    enum enum_message_enum_e value;
};

/**
 * Enum enum.Message2.InnerEnum.
 */
enum enum_message2_inner_enum_e {
    enum_message2_e_e = 0,
    enum_message2_f_e = 1
};

/**
 * Message enum.Message2.
 */
struct enum_message2_repeated_t {
    int length;
    struct enum_message2_t *items_p;
};

struct enum_message2_t {
    struct pbtools_message_base_t base;
    enum enum_enum_e outer;
    enum enum_message2_inner_enum_e inner;
};

/**
 * Enum enum.Limits.Enum.
 */
enum enum_limits_enum_e {
    enum_limits_g_e = 0,
    enum_limits_h_e = -2147483648,
    enum_limits_i_e = 2147483647
};

/**
 * Message enum.Limits.
 */
struct enum_limits_repeated_t {
    int length;
    struct enum_limits_t *items_p;
};

struct enum_limits_t {
    struct pbtools_message_base_t base;
    enum enum_limits_enum_e value;
};

/**
 * Enum enum.AllowAlias.Enum.
 */
enum enum_allow_alias_enum_e {
    enum_allow_alias_a_e = 0,
    enum_allow_alias_b_e = 1,
    enum_allow_alias_c_e = 1
};

/**
 * Message enum.AllowAlias.
 */
struct enum_allow_alias_repeated_t {
    int length;
    struct enum_allow_alias_t *items_p;
};

struct enum_allow_alias_t {
    struct pbtools_message_base_t base;
    enum enum_allow_alias_enum_e value;
};

/**
 * Encoding and decoding of enum.Message.
 */
struct enum_message_t *
enum_message_new(
    void *workspace_p,
    size_t size);

int enum_message_encode(
    struct enum_message_t *self_p,
    uint8_t *encoded_p,
    size_t size);

int enum_message_decode(
    struct enum_message_t *self_p,
    const uint8_t *encoded_p,
    size_t size);

/**
 * Encoding and decoding of enum.Message2.
 */
struct enum_message2_t *
enum_message2_new(
    void *workspace_p,
    size_t size);

int enum_message2_encode(
    struct enum_message2_t *self_p,
    uint8_t *encoded_p,
    size_t size);

int enum_message2_decode(
    struct enum_message2_t *self_p,
    const uint8_t *encoded_p,
    size_t size);

/**
 * Encoding and decoding of enum.Limits.
 */
struct enum_limits_t *
enum_limits_new(
    void *workspace_p,
    size_t size);

int enum_limits_encode(
    struct enum_limits_t *self_p,
    uint8_t *encoded_p,
    size_t size);

int enum_limits_decode(
    struct enum_limits_t *self_p,
    const uint8_t *encoded_p,
    size_t size);

/**
 * Encoding and decoding of enum.AllowAlias.
 */
struct enum_allow_alias_t *
enum_allow_alias_new(
    void *workspace_p,
    size_t size);

int enum_allow_alias_encode(
    struct enum_allow_alias_t *self_p,
    uint8_t *encoded_p,
    size_t size);

int enum_allow_alias_decode(
    struct enum_allow_alias_t *self_p,
    const uint8_t *encoded_p,
    size_t size);

/* Internal functions. Do not use! */

void enum_message_init(
    struct enum_message_t *self_p,
    struct pbtools_heap_t *heap_p);

void enum_message_encode_inner(
    struct pbtools_encoder_t *encoder_p,
    struct enum_message_t *self_p);

void enum_message_decode_inner(
    struct pbtools_decoder_t *decoder_p,
    struct enum_message_t *self_p);

void enum_message_encode_repeated_inner(
    struct pbtools_encoder_t *encoder_p,
    int field_number,
    struct enum_message_repeated_t *repeated_p);

void enum_message_decode_repeated_inner(
    struct pbtools_decoder_t *decoder_p,
    struct pbtools_repeated_info_t *repeated_info_p,
    struct enum_message_repeated_t *repeated_p);

void enum_message2_init(
    struct enum_message2_t *self_p,
    struct pbtools_heap_t *heap_p);

void enum_message2_encode_inner(
    struct pbtools_encoder_t *encoder_p,
    struct enum_message2_t *self_p);

void enum_message2_decode_inner(
    struct pbtools_decoder_t *decoder_p,
    struct enum_message2_t *self_p);

void enum_message2_encode_repeated_inner(
    struct pbtools_encoder_t *encoder_p,
    int field_number,
    struct enum_message2_repeated_t *repeated_p);

void enum_message2_decode_repeated_inner(
    struct pbtools_decoder_t *decoder_p,
    struct pbtools_repeated_info_t *repeated_info_p,
    struct enum_message2_repeated_t *repeated_p);

void enum_limits_init(
    struct enum_limits_t *self_p,
    struct pbtools_heap_t *heap_p);

void enum_limits_encode_inner(
    struct pbtools_encoder_t *encoder_p,
    struct enum_limits_t *self_p);

void enum_limits_decode_inner(
    struct pbtools_decoder_t *decoder_p,
    struct enum_limits_t *self_p);

void enum_limits_encode_repeated_inner(
    struct pbtools_encoder_t *encoder_p,
    int field_number,
    struct enum_limits_repeated_t *repeated_p);

void enum_limits_decode_repeated_inner(
    struct pbtools_decoder_t *decoder_p,
    struct pbtools_repeated_info_t *repeated_info_p,
    struct enum_limits_repeated_t *repeated_p);

void enum_allow_alias_init(
    struct enum_allow_alias_t *self_p,
    struct pbtools_heap_t *heap_p);

void enum_allow_alias_encode_inner(
    struct pbtools_encoder_t *encoder_p,
    struct enum_allow_alias_t *self_p);

void enum_allow_alias_decode_inner(
    struct pbtools_decoder_t *decoder_p,
    struct enum_allow_alias_t *self_p);

void enum_allow_alias_encode_repeated_inner(
    struct pbtools_encoder_t *encoder_p,
    int field_number,
    struct enum_allow_alias_repeated_t *repeated_p);

void enum_allow_alias_decode_repeated_inner(
    struct pbtools_decoder_t *decoder_p,
    struct pbtools_repeated_info_t *repeated_info_p,
    struct enum_allow_alias_repeated_t *repeated_p);

#ifdef __cplusplus
}
#endif

#endif
