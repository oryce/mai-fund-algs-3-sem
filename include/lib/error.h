#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef int32_t error_code_t;

typedef const char* (*error_fmt_t)(error_code_t code);

typedef struct error_ste {
	char function[64];
	uint16_t line;
} error_ste_t;

typedef struct error {
	/** Exception type. */
	error_code_t code;
	/** Exception message. */
	char message[256];
	/** Stack frames. */
	error_ste_t stack_trace[8];
	/** Amount of frames in stack_trace. */
	size_t stack_trace_length;
	/** Amount of frames that couldn't fit into stack_trace. */
	uint16_t stack_trace_extra_frames;
} error_t;

/** Throws an exception. */
#define THROW(code, message)                                   \
	do {                                                       \
		return error_throw(code, message, __func__, __LINE__); \
	} while (0);

/** Throws a formatted exception. */
#define THROWF(code, message, ...)                                           \
	do {                                                                     \
		return error_throwf(code, message, __func__, __LINE__, __VA_ARGS__); \
	} while (0);

/** Re-throws an exception, adding a message before the original. */
#define RETHROW(error, message)                                   \
	do {                                                          \
		return error_rethrow(message, __func__, __LINE__, error); \
	} while (0);

/** Passes the exception to the caller. */
#define PASS(error)                                   \
	do {                                              \
		return error_pass(error, __func__, __LINE__); \
	} while (0);

#define FAILED(error) error.code != 0
#define SUCCEEDED(error) error.code == 0

#define OverflowException 0x00000001
#define IllegalArgumentException 0x00000002
#define MemoryError 0x00000003
#define IOException 0x00000004
#define AssertionError 0x00000005
#define UnderflowException 0x00000006
#define UnexpectedTokenException 0x00000007

#define NO_EXCEPTION \
	(error_t) { .code = 0x0000000 }

/** Prints the error message to `stderr`. */
void error_print(error_t error);

/**
 * Prints the error message, but allows for several functions to determine the exception's name.
 * Useful for custom exceptions.
 */
void error_print_ex(error_t error, error_fmt_t fmt[], size_t nFmt);

error_t error_throw(error_code_t code, const char* message, const char* function, int line);

error_t error_throwf(error_code_t code, const char* message, const char* function, int line, ...);

error_t error_rethrow(const char* message, const char* function, int line, error_t error);

error_t error_pass(error_t error, const char* function, int line);
