/*
 * Copyright (c) 2011 Jacques Fortier
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdint.h>
#include <stddef.h>
#include "cobs.h"

#define COBS_DELIMITER 0x00

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
        return COBS_ERR_OUT_BUFFER_TOO_SMALL;
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
    output[code_index + 1] = COBS_DELIMITER;

    *out_length = write_index;
    return COBS_ERR_SUCCESS;
}

/* Unstuffs "length" bytes of data at the location pointed to by
 * "input", writing the output * to the location pointed to by
 * "output". Returns the number of bytes written to "output" if
 * "input" was successfully unstuffed, and 0 if there was an
 * error unstuffing "input".
 *
 * Remove the "restrict" qualifiers if compiling with a
 * pre-C99 C dialect.
 */

/**
 * @brief decode the input buffer to output buffer
 * @param [in]input input buffer
 * @param [in]length input buffer length
 * @param [out]output output buffer
 * @param [in,out]out_length output buffer length
 * @return error code, see enum COBS_ERRORS
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
        return COBS_ERR_OUT_BUFFER_TOO_SMALL;
    }

    while (read_index < length) {
        code = input[read_index];

        if (code == COBS_DELIMITER) {
            return COBS_ERR_ZERO_BYTE_IN_INPUT;
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
