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

error_t reverse_string(int argc, char** argv) {
	if (argc != 3 || argv == NULL) {
		return ERROR_INVALID_PARAMETER;
	}

	char* str = argv[2];
	size_t length = my_strlen(str);

	char* reversed = (char*)malloc(length + 1);
	if (reversed == NULL) return ERROR_HEAP_ALLOCATION;

	for (size_t i = 0; i != length; ++i) {
		reversed[i] = str[length - i - 1];
	}
	reversed[length] = '\0';

	printf("Reversed string: %s\n", reversed);

	free(reversed);
	return ERROR_SUCCESS;
}

error_t uppercase_odd_chars(int argc, char** argv) {
	if (argc != 3 || argv == NULL) {
		return ERROR_INVALID_PARAMETER;
	}

	char* str = argv[2];

	string_t modified = string_create();
	if (!string_created(&modified)) return ERROR_HEAP_ALLOCATION;

	for (size_t i = 0; str[i] != '\0'; ++i) {
		char ch = (char)(i % 2 != 0 ? chars_upper(str[i]) : str[i]);
		if (!string_append_char(&modified, ch)) return ERROR_HEAP_ALLOCATION;
	}

	printf("Modified string: %s\n", string_to_c_str(&modified));

	string_destroy(&modified);
	return ERROR_SUCCESS;
}

void rearrange_chars_cleanup(int n, ...) {
	va_list args;
	va_start(args, n);

	for (int i = 0; i != n; ++i) {
		string_t* str = va_arg(args, string_t*);
		string_destroy(str);
	}
}

error_t rearrange_chars(int argc, char** argv) {
	if (argc != 3 || argv == NULL) {
		return ERROR_INVALID_PARAMETER;
	}

	bool error = false;
	char* str = argv[2];

	string_t numbers = string_create();
	string_t letters = string_create();
	string_t other = string_create();
	string_t result = string_create();

	error =
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

	printf("Modified string: %s\n", string_to_c_str(&result));

	rearrange_chars_cleanup(4, numbers, letters, other, result);
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

	int nStrings = argc - 3;
	for (int i = nStrings - 1; i > 0; --i) {
		int j = rand() % (i + 1); // NOLINT(*-msc50-cpp)
		char* temp = argv[i + 3];
		argv[i + 3] = argv[j + 3];
		argv[j + 3] = temp;
	}

	string_t result = string_create();
	if (!string_created(&result)) {
		string_destroy(&result);
		return ERROR_HEAP_ALLOCATION;
	}

	for (int i = 3; i != argc; ++i) {
		bool success = true;
		success &= string_append_c_str(&result, argv[i]);
		success &= string_append_char(&result, ' ');
		if (!success) {
			string_destroy(&result);
			return ERROR_HEAP_ALLOCATION;
		}
	}

	printf("Shuffled strings: %s\n", string_to_c_str(&result));

	string_destroy(&result);
	return ERROR_SUCCESS;
}
