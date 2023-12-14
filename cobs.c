/*
 * Copyright (c) 2011 Jacques Fortier
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdint.h>
#include <stddef.h>
#include "cobs.h"

#define COBS_ISV COBS_INPLACE_SENTINEL_VALUE
typedef unsigned char cobs_byte_t;

/**
 * The COBS protocol requires an extra byte at the beginning and end of the payload.
 * If encoding and decoding in-place, it becomes your responsibility to reserve these extra bytes.
 * It's easy to mess this up and just put your own data at byte 0,
 * but your data must start at byte 1.
 *
 * @note For safety and sanity, cobs_encode_inplace will error with COBS_RET_ERR_BAD_PAYLOAD
 * if the first and last bytes aren't explicitly set to the sentinel value.
 * You have to put them there.
 */
enum COBS_ERRORS cobs_encode_inplace(uint8_t *buf, unsigned len) {
    if (!buf) {
        return COBS_ERR_OUT_BUFFER_NULL;
    }

    if (len < 2) {
        return COBS_ERR_BAD_OUTPUT_LENGTH;
    }

    cobs_byte_t *const src = (cobs_byte_t *) buf;
    if ((src[0] != COBS_ISV) || (src[len - 1] != COBS_ISV)) {
        return COBS_ERR_BAD_PAYLOAD;
    }

    unsigned patch = 0, cur = 1;
    while (cur < len - 1) {
        if (src[cur] == COBS_FRAME_DELIMITER) {
            unsigned const ofs = cur - patch;
            if (ofs > 255) { return COBS_ERR_BAD_PAYLOAD; }
            src[patch] = (cobs_byte_t) ofs;
            patch = cur;
        }
        ++cur;
    }
    unsigned const ofs = cur - patch;
    if (ofs > 255) { return COBS_ERR_BAD_PAYLOAD; }
    src[patch] = (cobs_byte_t) ofs;
    src[cur] = 0;
    return COBS_ERR_SUCCESS;
}

/**
 * @brief encode the input buffer to output buffer
 * @param [in]input input buffer
 * @param [in]length input buffer length
 * @param [out]output output buffer
 * @param [in,out]out_length output buffer length
 * @return error code, see enum COBS_ERRORS
 */
enum COBS_ERRORS cobs_encode(const uint8_t *input, size_t length, uint8_t *output, size_t *out_length) {
    size_t read_index = 0;
    size_t write_index = 1;
    size_t code_index = 0;
    uint8_t code = 1;

    if (out_length == NULL) {
        return COBS_ERR_OUT_BUFFER_NULL;
    }

    // at most one byte for every 254 non-zero bytes
    // plus one byte for the end-of-frame marker.
    if (*out_length < length + (length / 254) + 2) {
        return COBS_ERR_BAD_OUTPUT_LENGTH;
    }

    while (read_index < length) {
        if (input[read_index] == 0) {
            output[code_index] = code;
            code = 1;
            code_index = write_index++;
            read_index++;
        } else {
            output[write_index++] = input[read_index++];
            code++;
            if (code == 0xFF) {
                output[code_index] = code;
                code = 1;
                code_index = write_index++;
            }
        }
    }

    output[code_index] = code;

    *out_length = write_index;
    return COBS_ERR_SUCCESS;
}

/**
 * @brief decode the input buffer to output buffer
 * @param [in]input input buffer
 * @param [in]length input buffer length
 * @param [out]output output buffer
 * @param [in,out]out_length output buffer length
 * @return error code, see enum COBS_ERRORS
 *
 * @note could decode in place i.e. input == output
 */
enum COBS_ERRORS cobs_decode(const uint8_t *input, size_t length, uint8_t *output, size_t *out_length) {
    size_t read_index = 0;
    size_t write_index = 0;
    uint8_t code;
    uint8_t i;

    if (out_length == NULL) {
        return COBS_ERR_OUT_BUFFER_NULL;
    }

    if (*out_length < (length - 1)) {
        return COBS_ERR_BAD_OUTPUT_LENGTH;
    }

    while (read_index < length) {
        code = input[read_index];

        if (code == COBS_FRAME_DELIMITER) {
            *out_length = write_index - 1;
            return COBS_ERR_EOF_IN_INPUT;
        }

        if (read_index + code > length && code != 1) {
            return COBS_ERR_ZERO_BEYOND_BUFFER;
        }

        read_index++;

        for (i = 1; i < code; i++) {
            output[write_index++] = input[read_index++];
        }
        if (code != 0xFF && read_index != length) {
            output[write_index++] = 0x00;
        }
    }

    *out_length = write_index;
    return COBS_ERR_SUCCESS;
}
