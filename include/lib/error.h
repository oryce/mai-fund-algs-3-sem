#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef int32_t error_t;

typedef const char* (*error_fmt_t)(error_t);

#define ERROR_INVALID_PARAMETER 0x00000001
#define ERROR_OUT_OF_MEMORY 0x00000002
#define ERROR_OVERFLOW 0x00000003
#define ERROR_UNDERFLOW 0x00000004
#define ERROR_IO 0x00000005
#define ERROR_ASSERT 0x00000006
#define ERROR_UNEXPECTED_TOKEN 0x00000007
#define ERROR_UNRECOGNIZED_OPTION 0x00000008

#define FILENAME_ (strrchr("/" __FILE__, '/') + 1)

#define ASSERT(condition)                                                \
	do {                                                                 \
		if ((condition)) {                                               \
			fprintf(stderr, "assertion failed (%s:%d): %s\n", FILENAME_, \
			        __LINE__, #condition);                               \
			return ERROR_ASSERT;                                         \
		}                                                                \
	} while (0);

/** Prints the error message to `stderr`. */
void error_print(error_t error);

/**
 * Prints the error message, but allows for several functions to determine the
 * exception's name. Useful for custom exceptions.
 */
void error_print_ex(error_t error, error_fmt_t fmt[], size_t nFmt);
