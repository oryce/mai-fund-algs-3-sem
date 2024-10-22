#include "lib/error.h"

#include <stdio.h>

const char* error_builtin_fmt(error_t code) {
	switch (code) {
		case ERR_INVVAL:
			return "Invalid parameter";
		case ERR_MEM:
			return "Memory error";
		case ERR_OVERFLOW:
			return "Integer overflow";
		case ERR_UNDERFLOW:
			return "Integer underflow";
		case ERR_IO:
			return "I/O error";
		case ERR_CHECK:
			return "Check failed";
		case ERR_UNEXPTOK:
			return "Unexpected token";
		case ERR_UNRECOPT:
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
