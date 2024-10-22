#include "lib/convert.h"

#include "lib/chars.h"
#include "lib/jtckdint.h"

/**
 * Parses a long from a string.
 *
 * @return `ERR_OVERFLOW` if the number is too large.
 *         `ERR_UNEXPTOK` if the number contains invalid chars.
 */
error_t str_to_long(char* in, long* out) {
	int sign = 1;

	if (*in == '-') {
		sign = -1;
		++in;
	}

	long value = 0;

	for (; *in != '\0'; ++in) {
		if (*in >= '0' && *in <= '9') {
			if (ckd_mul(&value, value, 10)) return ERR_OVERFLOW;
			if (ckd_add(&value, value, *in - '0')) return ERR_OVERFLOW;
		} else {
			return ERR_UNEXPTOK;
		}
	}

	*out = value * sign;
	return 0;
}

/**
 * Parses a double from a string.
 *
 * @return `ERR_INVVAL`   if the number is invalid, has only `-` or `.`
 *         `ERR_UNEXPTOK` if the number has an invalid char.
 *         `ERR_OVERFLOW` if the number is too large.
 */
error_t str_to_double(char* in, double* out) {
	if ((*in == '.' || *in == '-') && *(in + 1) == '\0') {
		return ERR_INVVAL;
	}

	int sign = 1;

	if (*in == '-') {
		sign = -1;
		++in;
	}

	double integer = 0, frac = 0;
	double fracDivisor = 0;

	for (; *in != '\0'; ++in) {
		if (*in == '.' && !fracDivisor) {
			fracDivisor = 1;
		} else if (*in >= '0' && *in <= '9') {
			int digit = *in - '0';

			if (fracDivisor) {
				frac = frac * 10 + digit;
				if (frac < 0) return ERR_OVERFLOW;
				fracDivisor *= 10;
				if (fracDivisor < 0) return ERR_OVERFLOW;
			} else {
				integer = integer * 10 + digit;
				if (integer < 0) return ERR_OVERFLOW;
			}
		} else {
			return ERR_UNEXPTOK;
		}
	}

	if (fracDivisor)
		*out = sign * (integer + frac / fracDivisor);
	else
		*out = sign * integer;
	return 0;
}

/**
 * Converts a long number to a representation in a specified base.
 *
 * @param out buffer for base representation
 * @param outSize buffer size
 *
 * @return `ERR_INVVAL` if the base is invalid (must be in [2; 36])
 *                      or the buffer is too small to hold the result.
 */
error_t long_to_base(long in, int base, char* out, int outSize) {
	if (base > 36 || base < 2) return ERR_INVVAL;

	bool negative = in < 0;
	if (negative) in = -in;

	int cur = 0;  // Cursor to the current character

	if (in == 0) {
		if (outSize < 2) return ERR_INVVAL;
		out[cur++] = '0';
	} else {
		while (in != 0 && cur < outSize - 1) {
			int remainder = (int)(in % base);
			if (remainder < 10)
				out[cur++] = (char)('0' + remainder);
			else
				out[cur++] = (char)('a' + remainder - 10);
			in /= base;
		}
		if (in != 0) return ERR_INVVAL;
	}

	if (negative) {
		if (cur >= outSize - 1) return ERR_INVVAL;
		out[cur++] = '-';
	}

	if (cur >= outSize) return ERR_INVVAL;
	out[cur] = '\0';

	// Reverse the string in-place.
	int i = 0;
	int j = cur - 1;

	while (i < j) {
		char temp = out[i];
		out[i] = out[j];
		out[j] = temp;

		i++;
		j--;
	}

	return 0;
}

/**
 * Converts a number from an arbitrary base to a base-10 number.
 *
 * @param length length of the number.
 * @param out pointer to the output number.
 *
 * @return `ERR_INVVAL`   if any of the pointers is null or the string is empty.
 *         `ERR_UNEXPTOK` if the number has invalid digits for the base.
 *         `ERR_OVERFLOW` if the number is too large.
 */
error_t long_from_base(const char* n, size_t length, int base, long* out) {
	if (n == NULL || out == NULL) return ERR_INVVAL;

	long base10 = 0;
	long multiplier = 1;

	// Skip the first char (minus sign).
	int sign = *n == '-' ? -1 : 1;
	const char* start = sign == -1 ? n + 1 : n;

	if (*start == '\0') {
		return ERR_INVVAL;
	}

	// Traverse from the back of the number and assemble the base-10 number.
	for (char* ptr = (char*)(n + length - 1); ptr >= start; --ptr) {
		bool valid = chars_is_digit(*ptr) || chars_is_alpha(*ptr);
		if (!valid) return ERR_UNEXPTOK;

		int ord = chars_is_digit(*ptr) ? (*ptr - '0')
		                               : (10 + chars_lower(*ptr) - 'a');
		if (ord >= base || ord < 0) return ERR_UNEXPTOK;

		if (ckd_add(&base10, base10, ord * multiplier)) return ERR_OVERFLOW;
		if (ckd_mul(&multiplier, multiplier, base)) return ERR_OVERFLOW;
	}

	*out = base10 * sign;
	return 0;
}
