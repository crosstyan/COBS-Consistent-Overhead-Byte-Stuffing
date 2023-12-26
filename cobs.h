/*
 * Copyright (c) 2011 Jacques Fortier
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef COBS_H
#define COBS_H

#ifdef __cplusplus
extern "C" {
#endif

enum COBS_ERRORS {
    COBS_ERR_SUCCESS           = 0,
    COBS_ERR_BAD_OUTPUT_LENGTH = -1,
    /**
     * technically, this is not an error.
     * Just an indication that the decoding
     * ended with a zero byte.
     */
    COBS_ERR_EOF_IN_INPUT       = -2,
    COBS_ERR_OUT_BUFFER_NULL    = -4,
    COBS_ERR_ZERO_BEYOND_BUFFER = -5,
    COBS_ERR_BAD_PAYLOAD        = -6,
};

enum {
    // All COBS frames end with this value. If you're scanning a data source
    // for frame delimiters, the presence of this zero byte indicates the
    // completion of a frame.
    COBS_FRAME_DELIMITER = 0x00,

    // In-place encoding mandatory placeholder byte values.
    COBS_INPLACE_SENTINEL_VALUE = 0x5A,

    // In-place encodings that fit in a buffer of this size will always succeed.
    COBS_INPLACE_SAFE_BUFFER_SIZE = 256
};

enum COBS_ERRORS cobs_encode_inplace(uint8_t *buf, unsigned len);
enum COBS_ERRORS cobs_encode(const uint8_t *input, size_t length, uint8_t *output, size_t *out_length);
enum COBS_ERRORS cobs_decode(const uint8_t *input, size_t length, uint8_t *output, size_t *out_length);

#ifdef __cplusplus
}
#endif

#endif
