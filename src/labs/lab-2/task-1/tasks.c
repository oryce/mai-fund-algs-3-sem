#include "tasks.h"

#include <stdarg.h>
#include <stdlib.h>

#include "lib/chars.h"
#include "lib/collections/string.h"
#include "lib/convert.h"

error_t task_string_length(const char* str, size_t* length) {
	if (!str || !length) return ERROR_INVALID_PARAMETER;

	while (*str != '\0') {
		++str;
		++(*length);
	}

	return 0;
}

error_t task_reverse_string(const char* str, char** out) {
	if (!str || !out) return ERROR_INVALID_PARAMETER;

	size_t length = 0;

	error_t error = task_string_length(str, &length);
	if (error) return error;

	char* reversed = (char*)malloc(length + 1);
	if (!reversed) return ERROR_OUT_OF_MEMORY;

	for (size_t i = 0; i != length; ++i) {
		reversed[i] = str[length - i - 1];
	}

	reversed[length] = '\0';
	*out = reversed;
	return 0;
}

error_t task_uppercase_odd_chars(const char* str, char** out) {
	if (!str || !out) return ERROR_INVALID_PARAMETER;

	size_t length = 0;

	error_t error = task_string_length(str, &length);
	if (error) return error;

	char* modified = (char*)malloc(length + 1);
	if (!modified) return ERROR_OUT_OF_MEMORY;

	for (size_t i = 0; i != length; ++i) {
		modified[i] = (char)(i % 2 ? chars_upper(str[i]) : str[i]);
	}

	*out = modified;
	return 0;
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
	if (str == NULL) return ERROR_INVALID_PARAMETER;
	if (out == NULL) return ERROR_INVALID_PARAMETER;

	bool error = false;
	string_t numbers, letters, other, result;

	if (!string_create(&numbers) || !string_create(&letters) ||
	    !string_create(&other) || !string_create(&result)) {
		return ERROR_OUT_OF_MEMORY;
	}

	while (*str != '\0') {
		if (chars_is_digit(*str))
			error = !string_append_char(&numbers, *str);
		else if (chars_is_alpha(*str))
			error = !string_append_char(&letters, *str);
		else
			error = !string_append_char(&other, *str);
		if (!error) {
			rearrange_chars_cleanup_(4, numbers, letters, other, result);
			return ERROR_OUT_OF_MEMORY;
		}

		++str;
	}

	error |= !string_append(&result, &numbers);
	error |= !string_append(&result, &letters);
	error |= !string_append(&result, &other);
	if (!error) {
		rearrange_chars_cleanup_(4, numbers, letters, other, result);
		return ERROR_OUT_OF_MEMORY;
	}

	*out = (char*)string_to_c_str(&result);
	string_destroy(&numbers);
	string_destroy(&letters);
	string_destroy(&other);
	return 0;
}

error_t task_concat_randomly(const char** strings, size_t n, char** out) {
	if (!strings || !out) return ERROR_INVALID_PARAMETER;

	for (size_t i = n - 1; i > 0; --i) {
		size_t j = rand() % (i + 1);  // NOLINT(*-msc50-cpp)

		const char* temp = strings[i];
		strings[i] = strings[j];
		strings[j] = temp;
	}

	string_t result;
	if (!string_create(&result)) return ERROR_OUT_OF_MEMORY;

	for (int i = 0; i != n; ++i) {
		if (!string_append_c_str(&result, strings[i]) ||
		    !string_append_char(&result, ' ')) {
			string_destroy(&result);
			return ERROR_OUT_OF_MEMORY;
		}
	}

	*out = (char*)string_to_c_str(&result);
	return 0;
}
