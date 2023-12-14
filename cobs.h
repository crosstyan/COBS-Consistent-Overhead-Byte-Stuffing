/*
 * Copyright (c) 2011 Jacques Fortier
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef COBS_H
#define COBS_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

enum COBS_ERRORS
{
    COBS_ERR_SUCCESS = 0,
    COBS_ERR_OUT_BUFFER_TOO_SMALL = -1,
    COBS_ERR_ZERO_BYTE_IN_INPUT = -2,
    COBS_ERR_OUT_BUFFER_NULL = -4,
    COBS_ERR_ZERO_BEYOND_BUFFER = -5,
};


enum COBS_ERRORS cobs_encode(const uint8_t *input, size_t length, uint8_t *output, size_t *out_length);
enum COBS_ERRORS cobs_decode(const uint8_t *input, size_t length, uint8_t *output, size_t *out_length);

#ifdef __cplusplus
}
#endif

#endif
