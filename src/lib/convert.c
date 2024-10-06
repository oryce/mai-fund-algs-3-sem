#include "lib/chars.h"
#include "lib/convert.h"

error_t str_to_long(char* in, long* out) {
	int sign = 1;

	if (*in == '-') {
		sign = -1;
		++in;
	}

	long value = 0;

	for (; *in != '\0'; ++in) {
		if (*in >= '0' && *in <= '9') {
			value = value * 10 + (*in - '0');
			if (value < 0) return ERROR_OVERFLOW;
		} else {
			return ERROR_INVALID_PARAMETER;
		}
	}

	*out = value * sign;
	return ERROR_SUCCESS;
}

error_t str_to_double(char* in, double* out) {
	if ((*in == '.' || *in == '-') && *(in + 1) == '\0') {
		return ERROR_INVALID_PARAMETER;
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
				if (frac < 0) return ERROR_OVERFLOW;
				fracDivisor *= 10;
				if (fracDivisor < 0) return ERROR_OVERFLOW;
			} else {
				integer = integer * 10 + digit;
				if (integer < 0) return ERROR_OVERFLOW;
			}
		} else {
			return ERROR_INVALID_PARAMETER;
		}
	}

	if (fracDivisor) {
		*out = sign * (integer + frac / fracDivisor);
	} else {
		*out = sign * integer;
	}

	return ERROR_SUCCESS;
}

error_t long_to_base(long in, int base, char* out, int outSize) {
	if (base > 36 || base < 2) return ERROR_INVALID_PARAMETER;

	bool negative = in < 0;
	if (negative) in = -in;

	int cur = 0;  // Cursor to the current character

	if (in == 0) {
		if (outSize < 2) return ERROR_INVALID_PARAMETER;
		out[cur++] = '0';
	} else {
		// Ensure that the buffer isn't overrun
		while (in != 0 && cur < outSize - 1) {
			int remainder = (int)(in % base);
			if (remainder < 10) {
				out[cur++] = (char)('0' + remainder);
			} else {
				out[cur++] = (char)('a' + remainder - 10);
			}

			in /= base;
		}

		// Buffer too small to hold result.
		if (in != 0) return ERROR_INVALID_PARAMETER;
	}

	if (negative) {
		if (cur >= outSize - 1) return ERROR_INVALID_PARAMETER;
		out[cur++] = '-';
	}

	if (cur >= outSize) return ERROR_INVALID_PARAMETER;
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

	return ERROR_SUCCESS;
}

error_t long_from_base(const char* n, size_t length, int base, long* out) {
	long base10 = 0;
	long multiplier = 1;

	// Skip the first char (minus sign).
	int sign = *n == '-' ? -1 : 1;
	const char* start = sign == -1 ? n + 1 : n;

	// Traverse from the back of the number and assemble the base-10 number.
	for (char* ptr = (char*)(n + length - 1); ptr >= start; --ptr) {
		bool valid = chars_is_digit(*ptr) || chars_is_alpha(*ptr);
		if (!valid) return ERROR_INVALID_PARAMETER;

		int ord = chars_is_digit(*ptr) ? (*ptr - '0') : (10 + chars_lower(*ptr) - 'a');
		if (ord >= base) return ERROR_INVALID_PARAMETER;

		base10 += ord * multiplier;
		if (base10 < 0) return ERROR_OVERFLOW;

		multiplier *= base;
	}

	*out = base10 * sign;
	return ERROR_SUCCESS;
}
