#pragma once

#include "error.h"

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

/**
 * Converts a number from an arbitrary base to a base-10 number.
 *
 * @param n a number in base |base|
 * @param length length of the number
 * @param base base of the number
 * @param out pointer to the output base-10 number
 *
 * @return status code
 */
error_t long_from_base(const char* n, size_t length, int base, long* out);
