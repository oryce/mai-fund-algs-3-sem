#pragma once

#include "error.h"

#include "lib/string.h"

error_t str_to_long(char* in, long* out);

error_t str_to_double(char* in, double* out);

/**
 * Converts a long base 10 number to a number in a specified base.
 * Stores the result in a buffer with limited size, specified as outSize.
 *
 * @param in input number
 * @param base target base
 * @param out output number
 * @param outSize output buffer size
 *
 * @return status code
 */
error_t long_to_base(long in, int base, char* out, int outSize);
