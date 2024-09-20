#include "conv.h"

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
			return ERROR_UNEXPECTED_TOKEN;
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
	int fracDivisor = 0;

	for (; *in != '\0'; ++in) {
		if (*in == '.' && !fracDivisor) {
			fracDivisor = 1;
		} else if (*in >= '0' && *in <= '9') {
			int digit = *in - '0';

			if (fracDivisor) {
				frac = frac * 10 + digit;
				fracDivisor *= 10;
			} else {
				integer = integer * 10 + digit;
			}
		} else {
			return ERROR_UNEXPECTED_TOKEN;
		}
	}

	if (fracDivisor) {
		*out = sign * (integer + frac / fracDivisor);
	} else {
		*out = sign * integer;
	}

	return ERROR_SUCCESS;
}
