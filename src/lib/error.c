#include "lib/error.h"

#include <stdarg.h>
#include <stdio.h>

void error_print_(error_t error, const char* exceptionName) {
	if (exceptionName == NULL) {
		fprintf(stderr, "Exception 0x%08x: %s\n", error.code, error.message);
	} else {
		fprintf(stderr, "%s (0x%08x): %s\n", exceptionName, error.code, error.message);
	}

	for (size_t i = 0; i != error.stack_trace_length; ++i) {
		error_ste_t ste = error.stack_trace[i];
		fprintf(stderr, "  at %s:%d\n", ste.function, ste.line);
	}

	if (error.stack_trace_extra_frames != 0) {
		fprintf(stderr, "  ... %d more", error.stack_trace_extra_frames);
	}
}

const char* error_to_string_(error_code_t code) {
	switch (code) {
		case OverflowException:
			return "OverflowException";
		case IllegalArgumentException:
			return "IllegalArgumentException";
		case MemoryError:
			return "MemoryError";
		case IOException:
			return "IOException";
		case AssertionError:
			return "AssertionError";
		case UnderflowException:
			return "UnderflowException";
		case UnexpectedTokenException:
			return "UnexpectedTokenException";
		default:
			return NULL;
	}
}

void error_print(error_t error) { error_print_(error, error_to_string_(error.code)); }

void error_print_ex(error_t error, error_fmt_t fmt[], size_t nFmt) {
	for (size_t i = 0; i != nFmt; ++i) {
		const char* message = fmt[i](error.code);

		if (message != NULL) {
			error_print_(error, message);
			return;
		}
	}

	error_print(error);  // Fall back to default exception message
}

void append_ste_(error_t* error, const char* function, int line) {
	const size_t maxFrames = sizeof(error->stack_trace) / sizeof(error->stack_trace[0]);

	if (error->stack_trace_length == maxFrames) {
		++error->stack_trace_extra_frames;
	} else {
		error_ste_t* ste = &error->stack_trace[error->stack_trace_length];

		strncpy(ste->function, function, sizeof(ste->function));
		ste->line = line;

		++error->stack_trace_length;
	}
}

void copy_ste_(error_t* srcError, error_t* dstError) {
	memcpy(dstError->stack_trace, srcError->stack_trace, sizeof(dstError->stack_trace));
	dstError->stack_trace_length = srcError->stack_trace_length;
	dstError->stack_trace_extra_frames = srcError->stack_trace_extra_frames;
}

error_t error_throw(error_code_t code, const char* message, const char* function, int line) {
	error_t error = {.code = code};

	strncpy(error.message, message, sizeof(error.message));
	append_ste_(&error, function, line);

	return error;
}

error_t error_throwf(error_code_t code, const char* message, const char* function, int line, ...) {
	va_list args;
	va_start(args, line);

	char newMessage[256];
	vsnprintf(newMessage, sizeof(newMessage), message, args);

	va_end(args);
	return error_throw(code, newMessage, function, line);
}

error_t error_rethrow(const char* message, const char* function, int line, error_t error) {
	char newMessage[sizeof(error.message)];
	snprintf(newMessage, sizeof(newMessage), "%s: %s", message, error.message);

	error_t newError = {.code = error.code};

	strcpy(newError.message, newMessage);
	copy_ste_(&error, &newError);
	append_ste_(&newError, function, line);

	return newError;
}

error_t error_pass(error_t error, const char* function, int line) {
	error_t newError = {.code = error.code};

	strcpy(newError.message, error.message);
	copy_ste_(&error, &newError);
	append_ste_(&newError, function, line);

	return newError;
}
