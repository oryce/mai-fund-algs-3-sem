#include "lib/convert.h"

#include "lib/chars.h"

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
			if (value < 0) {
				THROW(OverflowException, "long overflow; number is too large");
			}
		} else {
			THROWF(UnexpectedTokenException, "%c", *in);
		}
	}

	*out = value * sign;
	return NO_EXCEPTION;
}

error_t str_to_double(char* in, double* out) {
	if ((*in == '.' || *in == '-') && *(in + 1) == '\0') {
		THROW(IllegalArgumentException, "invalid number");
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
				if (frac < 0) {
					THROW(OverflowException, "double overflow in fractional part");
				}
				fracDivisor *= 10;
				if (fracDivisor < 0) {
					THROW(OverflowException, "double overflow in fractional divisor");
				}
			} else {
				integer = integer * 10 + digit;
				if (integer < 0) {
					THROW(OverflowException, "double overflow in integer part");
				}
			}
		} else {
			THROWF(UnexpectedTokenException, "unexpected token: %c", *in);
		}
	}

	if (fracDivisor) {
		*out = sign * (integer + frac / fracDivisor);
	} else {
		*out = sign * integer;
	}

	return NO_EXCEPTION;
}

error_t long_to_base(long in, int base, char* out, int outSize) {
	if (base > 36 || base < 2) {
		THROWF(IllegalArgumentException, "invalid base: %d", base);
	}

	bool negative = in < 0;
	if (negative) in = -in;

	int cur = 0;  // Cursor to the current character

	if (in == 0) {
		if (outSize < 2) {
			THROW(IllegalArgumentException, "buffer is too small");
		}
		out[cur++] = '0';
	} else {
		while (in != 0 && cur < outSize - 1) {
			int remainder = (int)(in % base);
			if (remainder < 10) {
				out[cur++] = (char)('0' + remainder);
			} else {
				out[cur++] = (char)('a' + remainder - 10);
			}
			in /= base;
		}
		if (in != 0) {
			THROW(IllegalArgumentException, "buffer is too small");
		}
	}

	if (negative) {
		if (cur >= outSize - 1) {
			THROW(IllegalArgumentException, "buffer is too small");
		}
		out[cur++] = '-';
	}

	if (cur >= outSize) {
		THROW(IllegalArgumentException, "buffer is too small");
	}
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

	return NO_EXCEPTION;
}

error_t long_from_base(const char* n, size_t length, int base, long* out) {
	if (n == NULL) THROW(IllegalArgumentException, "`n` may not be null");
	if (out == NULL) THROW(IllegalArgumentException, "`out` may not be null");

	long base10 = 0;
	long multiplier = 1;

	// Skip the first char (minus sign).
	int sign = *n == '-' ? -1 : 1;
	const char* start = sign == -1 ? n + 1 : n;

//	if (*start == '\0') {
//		THROW(IllegalArgumentException, "input number may not be empty");
//	}

	// Traverse from the back of the number and assemble the base-10 number.
	for (char* ptr = (char*)(n + length - 1); ptr >= start; --ptr) {
		bool valid = chars_is_digit(*ptr) || chars_is_alpha(*ptr);
		if (!valid) {
			THROWF(UnexpectedTokenException, "invalid character: %c", *ptr);
		}

		int ord = chars_is_digit(*ptr) ? (*ptr - '0') : (10 + chars_lower(*ptr) - 'a');
		if (ord >= base || ord < 0) {
			THROWF(UnexpectedTokenException, "invalid character %c for base %d", *ptr, base);
		}

		base10 += ord * multiplier;
		if (base10 < 0) {
			THROW(OverflowException, "input number is too large");
		}

		multiplier *= base;
	}

	*out = base10 * sign;
	return NO_EXCEPTION;
}
