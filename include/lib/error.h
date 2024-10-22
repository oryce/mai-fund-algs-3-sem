#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef int32_t error_t;

typedef const char* (*error_fmt_t)(error_t);

#define ERR_INVVAL 0x00000001
#define ERR_MEM 0x00000002
#define ERR_OVERFLOW 0x00000003
#define ERR_UNDERFLOW 0x00000004
#define ERR_IO 0x00000005
#define ERR_CHECK 0x00000006
#define ERR_UNEXPTOK 0x00000007
#define ERR_UNRECOPT 0x00000008

#define FILENAME_ (strrchr("/" __FILE__, '/') + 1)

#define CHECK(condition, msg)                                                  \
	do {                                                                       \
		if ((condition)) {                                                     \
			fprintf(stderr, "check failed (%s:%d): %s\n", FILENAME_, __LINE__, \
			        msg);                                                      \
			return ERR_CHECK;                                                  \
		}                                                                      \
	} while (0);

/** Prints the error message to `stderr`. */
void error_print(error_t error);

/**
 * Prints the error message, but allows for several functions to determine the
 * exception's name. Useful for custom exceptions.
 */
void error_print_ex(error_t error, error_fmt_t fmt[], size_t nFmt);
