#pragma once

#include <stdio.h>

typedef long error_t;

#define ERROR_SUCCESS 0L
#define ERROR_OVERFLOW 1L
#define ERROR_INVALID_PARAMETER 2L
#define ERROR_LIMIT_FAIL 3L
#define ERROR_HEAP_ALLOCATION 4L
#define ERROR_IO 5L
#define ERROR_DIVERGING 6L
#define ERROR_INTEGRAL_FAIL 7L
#define ERROR_OOPS 8L
#define ERROR_UNDERFLOW 9L

/** Prints the error message to `stderr`. */
void error_print(error_t error);

/** Creates an "oops" message -- an unexpected condition -- and prints it to `stderr`. */
inline static error_t error_oops(const char* message) {
	fprintf(stderr, "oops: %s", message);
	return ERROR_OOPS;
}
