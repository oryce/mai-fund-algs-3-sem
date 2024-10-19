#include "tasks.h"

#include <stdarg.h>
#include <stdlib.h>

#include "lib/chars.h"
#include "lib/collections/string.h"
#include "lib/convert.h"

error_t task_string_length(const char* str, size_t* length) {
	if (str == NULL) THROW(IllegalArgumentException, "`str` may not be null");
	if (length == NULL) THROW(IllegalArgumentException, "`*length` may not be null");

	while (*str != '\0') {
		++str;
		++(*length);
	}

	return NO_EXCEPTION;
}

error_t task_reverse_string(const char* str, char** out) {
	if (str == NULL) THROW(IllegalArgumentException, "`str` may not be null");
	if (out == NULL) THROW(IllegalArgumentException, "`out` may not be null");

	error_t status;

	size_t length;
	if (FAILED((status = task_string_length(str, &length)))) {
		PASS(status);
	}

	char* reversed = (char*)malloc(length + 1);
	if (reversed == NULL) THROW(MemoryError, "can't allocated memory for `reserved`");

	for (size_t i = 0; i != length; ++i) {
		reversed[i] = str[length - i - 1];
	}

	reversed[length] = '\0';
	*out = reversed;
	return NO_EXCEPTION;
}

error_t task_uppercase_odd_chars(const char* str, char** out) {
	if (str == NULL) THROW(IllegalArgumentException, "`str` may not be null");
	if (out == NULL) THROW(IllegalArgumentException, "`out` may not be null");

	error_t status;

	size_t length;
	if (FAILED((status = task_string_length(str, &length)))) {
		PASS(status);
	}

	char* modified = (char*)malloc(length + 1);
	if (modified == NULL) THROW(MemoryError, "can't allocated memory for `reversed`");

	for (size_t i = 0; i != length; ++i) {
		modified[i] = (char)(i % 2 ? chars_upper(str[i]) : str[i]);
	}

	*out = modified;
	return NO_EXCEPTION;
}

void rearrange_chars_cleanup_(int n, ...) {
	va_list args;
	va_start(args, n);

	for (int i = 0; i != n; ++i) {
		string_t* str = va_arg(args, string_t*);
		string_destroy(str);
	}

	va_end(args);
}

error_t task_rearrange_chars(const char* str, char** out) {
	if (str == NULL) THROW(IllegalArgumentException, "`str` may not be null");
	if (out == NULL) THROW(IllegalArgumentException, "`out` may not be null");

	bool error = false;
	string_t numbers = string_create();
	string_t letters = string_create();
	string_t other = string_create();
	string_t result = string_create();

	error |= !string_created(&numbers);
	error |= !string_created(&letters);
	error |= !string_created(&other);
	error |= !string_created(&result);
	if (!error) {
		rearrange_chars_cleanup_(4, numbers, letters, other, result);
		THROW(MemoryError, "can't create string buffers");
	}

	while (*str != '\0') {
		if (chars_is_digit(*str)) {
			error = !string_append_char(&numbers, *str);
		} else if (chars_is_alpha(*str)) {
			error = !string_append_char(&letters, *str);
		} else {
			error = !string_append_char(&other, *str);
		}
		if (!error) {
			rearrange_chars_cleanup_(4, numbers, letters, other, result);
			THROW(MemoryError, "can't append char into string buffer");
		}

		++str;
	}

	error |= !string_append(&result, &numbers);
	error |= !string_append(&result, &letters);
	error |= !string_append(&result, &other);
	if (!error) {
		rearrange_chars_cleanup_(4, numbers, letters, other, result);
		THROW(MemoryError, "can't concat resulting strings");
	}

	*out = (char*)string_to_c_str(&result);
	string_destroy(&numbers);
	string_destroy(&letters);
	string_destroy(&other);
	return NO_EXCEPTION;
}

error_t task_concat_randomly(const char** strings, size_t n, char** out) {
	if (strings == NULL) THROW(IllegalArgumentException, "`strings` may not be null");
	if (out == NULL) THROW(IllegalArgumentException, "`out` may not be null");

	for (size_t i = n - 1; i > 0; --i) {
		size_t j = rand() % (i + 1);  // NOLINT(*-msc50-cpp)

		const char* temp = strings[i];
		strings[i] = strings[j];
		strings[j] = temp;
	}

	string_t result = string_create();
	if (!string_created(&result)) {
		string_destroy(&result);
		THROW(MemoryError, "can't create resulting string");
	}

	for (int i = 0; i != n; ++i) {
		bool success = true;
		success &= string_append_c_str(&result, strings[i]);
		success &= string_append_char(&result, ' ');
		if (!success) {
			string_destroy(&result);
			THROW(MemoryError, "can't append string");
		}
	}

	*out = (char*)string_to_c_str(&result);
	return NO_EXCEPTION;
}

