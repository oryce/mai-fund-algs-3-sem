#include "tasks.h"

#include <stdarg.h>
#include <stdlib.h>

#include "lib/chars.h"
#include "lib/collections/string.h"
#include "lib/convert.h"

size_t my_strlen(const char* str) {
	if (str == NULL) return 0;
	size_t length = 0;

	while (*str != '\0') {
		++str;
		++length;
	}

	return length;
}

error_t print_length(int argc, char** argv) {
	if (argc != 3 || argv == NULL) {
		return ERROR_INVALID_PARAMETER;
	}

	printf("String length: %zu\n", my_strlen(argv[2]));
	return ERROR_SUCCESS;
}

error_t reverse_string_s(const char* str, char** out) {
	if (str == NULL || out == NULL) {
		return ERROR_INVALID_PARAMETER;
	}
	size_t length = my_strlen(str);

	char* reversed = (char*)malloc(length + 1);
	if (reversed == NULL) return ERROR_HEAP_ALLOCATION;

	for (size_t i = 0; i != length; ++i) {
		reversed[i] = str[length - i - 1];
	}

	reversed[length] = '\0';
	*out = reversed;
	return ERROR_SUCCESS;
}

error_t reverse_string(int argc, char** argv) {
	if (argc != 3 || argv == NULL) {
		return ERROR_INVALID_PARAMETER;
	}

	char* reversed;
	error_t error = reverse_string_s(argv[2], &reversed);
	if (error) {
		return error;
	}

	printf("Reversed string: %s\n", reversed);
	free(reversed);
	return ERROR_SUCCESS;
}

error_t uppercase_odd_chars_s(const char* str, string_t* out) {
	if (str == NULL || out == NULL) {
		return ERROR_INVALID_PARAMETER;
	}

	string_t modified = string_create();
	if (!string_created(&modified)) {
		string_destroy(&modified);
		return ERROR_HEAP_ALLOCATION;
	}

	for (size_t i = 0; str[i] != '\0'; ++i) {
		char ch = (char)(i % 2 != 0 ? chars_upper(str[i]) : str[i]);
		if (!string_append_char(&modified, ch)) {
			string_destroy(&modified);
			return ERROR_HEAP_ALLOCATION;
		}
	}

	*out = modified;
	return ERROR_SUCCESS;
}

error_t uppercase_odd_chars(int argc, char** argv) {
	if (argc != 3 || argv == NULL) {
		return ERROR_INVALID_PARAMETER;
	}

	string_t result;
	error_t error = uppercase_odd_chars_s(argv[2], &result);
	if (error) {
		return error;
	}

	printf("Modified string: %s\n", string_to_c_str(&result));
	string_destroy(&result);
	return ERROR_SUCCESS;
}

void rearrange_chars_cleanup(int n, ...) {
	va_list args;
	va_start(args, n);

	for (int i = 0; i != n; ++i) {
		string_t* str = va_arg(args, string_t*);
		string_destroy(str);
	}

	va_end(args);
}

error_t rearrange_chars_str(const char* str, string_t* out) {
	if (str == NULL || out == NULL) {
		return ERROR_INVALID_PARAMETER;
	}

	string_t numbers = string_create();
	string_t letters = string_create();
	string_t other = string_create();
	string_t result = string_create();

	bool error =
	    !string_created(&numbers) || !string_created(&letters) || !string_created(&other) || !string_created(&result);
	if (error) {
		rearrange_chars_cleanup(4, numbers, letters, other, result);
		return ERROR_HEAP_ALLOCATION;
	}

	while (*str != '\0') {
		if (chars_is_digit(*str)) {
			error = !string_append_char(&numbers, *str);
		} else if (chars_is_alpha(*str)) {
			error = !string_append_char(&letters, *str);
		} else {
			error = !string_append_char(&other, *str);
		}
		if (error) {
			rearrange_chars_cleanup(4, numbers, letters, other, result);
			return ERROR_HEAP_ALLOCATION;
		}

		++str;
	}

	error |= !string_append(&result, &numbers);
	error |= !string_append(&result, &letters);
	error |= !string_append(&result, &other);
	if (error) {
		rearrange_chars_cleanup(4, numbers, letters, other, result);
		return ERROR_HEAP_ALLOCATION;
	}

	*out = result;
	string_destroy(&numbers);
	string_destroy(&letters);
	string_destroy(&other);
	return ERROR_SUCCESS;
}

error_t rearrange_chars(int argc, char** argv) {
	if (argc != 3 || argv == NULL) {
		return ERROR_INVALID_PARAMETER;
	}

	string_t result;
	error_t error = rearrange_chars_str(argv[2], &result);
	if (error) {
		return error;
	}

	printf("Modified string: %s\n", string_to_c_str(&result));
	string_destroy(&result);
	return ERROR_SUCCESS;
}

error_t concat_randomly_s(const char** strings, size_t nStrings, string_t* out) {
	if (strings == NULL || out == NULL) {
		return ERROR_INVALID_PARAMETER;
	}

	for (size_t i = nStrings - 1; i > 0; --i) {
		size_t j = rand() % (i + 1);  // NOLINT(*-msc50-cpp)

		const char* temp = strings[i];
		strings[i] = strings[j];
		strings[j] = temp;
	}

	string_t result = string_create();
	if (!string_created(&result)) {
		string_destroy(&result);
		return ERROR_HEAP_ALLOCATION;
	}

	for (int i = 0; i != nStrings; ++i) {
		bool success = true;
		success &= string_append_c_str(&result, strings[i]);
		success &= string_append_char(&result, ' ');
		if (!success) {
			string_destroy(&result);
			return ERROR_HEAP_ALLOCATION;
		}
	}

	*out = result;
	return ERROR_SUCCESS;
}

error_t concat_randomly(int argc, char** argv) {
	if (argc < 4 || argv == NULL) {
		return ERROR_INVALID_PARAMETER;
	}

	long seed;
	if (str_to_long(argv[2], &seed) != ERROR_SUCCESS) {
		return ERROR_INVALID_PARAMETER;
	}
	srand(seed);

	const char* strings[argc - 3];
	memcpy(strings, &argv[3], (argc - 3) * sizeof(char*));

	string_t result;
	error_t error = concat_randomly_s(strings, argc - 3, &result);
	if (error) {
		return error;
	}

	printf("Shuffled strings: %s\n", string_to_c_str(&result));
	string_destroy(&result);
	return ERROR_SUCCESS;
}
