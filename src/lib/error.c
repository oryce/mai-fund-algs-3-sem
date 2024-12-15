#include "lib/error.h"

#include <stdio.h>

const char* error_builtin_fmt(error_t code) {
	switch (code) {
		case ERROR_INVALID_PARAMETER:
			return "The parameter is incorrect";
		case ERROR_OUT_OF_MEMORY:
			return "Out of memory";
		case ERROR_OVERFLOW:
			return "Integer overflow";
		case ERROR_UNDERFLOW:
			return "Integer underflow";
		case ERROR_IO:
			return "I/O error";
		case ERROR_ASSERT:
			return "Assertion failed";
		case ERROR_UNEXPECTED_TOKEN:
			return "Unexpected token";
		case ERROR_UNRECOGNIZED_OPTION:
			return "Unrecognized option";
		default:
			return NULL;
	}
}

void error_print_(const char* msg) { fprintf(stderr, "Error: %s\n", msg); }

void error_print(error_t error) { error_print_(error_builtin_fmt(error)); }

void error_print_ex(error_t error, error_fmt_t fmt[], size_t nFmt) {
	for (size_t i = 0; i != nFmt; ++i) {
		const char* message = fmt[i](error);

		if (message != NULL) {
			error_print_(message);
			return;
		}
	}

	error_print(error);  // Fall back to default exception message
}
