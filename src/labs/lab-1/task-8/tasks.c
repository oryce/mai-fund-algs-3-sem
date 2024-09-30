#include "tasks.h"

#include <stdio.h>

#include "lib/chars.h"
#include "lib/lexeme.h"
#include "lib/mth.h"

error_t process_lexeme(const char* lexeme, FILE* out) {
	int base = 2;
	int sign = *lexeme == '-' ? -1 : 1;

	// Skip the first char (minus sign).
	const char* startPtr = sign == -1 ? lexeme + 1 : lexeme;

	// Determine the minimum base to represent the number.
	char* ptr;

	for (ptr = (char*)startPtr; *ptr != '\0'; ++ptr) {
		if (!chars_is_alpha(*ptr) && !chars_is_digit(*ptr)) {
			fprintf(stderr, "Malformed lexeme: %s\n", lexeme);
			return ERROR_INVALID_PARAMETER;
		}

		int ord = chars_is_alpha(*ptr) ? 10 + (chars_lower(*ptr) - 'a') : *ptr - '0';
		base = mth_int_max(base, ord + 1);
	}

	// Convert the number from |base| to base 10.
	long base10 = 0;
	long multiplier = 1;

	// Traverse backwards (|ptr| is '\0' when the first loop finishes).
	for (ptr = ptr - 1; ptr >= startPtr; --ptr) {
		int ord = chars_is_alpha(*ptr) ? 10 + (chars_lower(*ptr) - 'a') : *ptr - '0';

		base10 += ord * multiplier;
		if (base10 < 0) return ERROR_OVERFLOW;

		multiplier *= base;
	}

	base10 *= sign;

	// Skip leading zeros in the original number.
	++ptr;
	while (*ptr == '0') ++ptr;

	if (sign == -1) {
		fprintf(out, "-%s %d %ld\n", ptr, base, base10);
	} else {
		fprintf(out, "%s %d %ld\n", ptr, base, base10);
	}
	if (ferror(out)) return ERROR_IO;

	return ERROR_SUCCESS;
}

error_t determine_min_number_bases(FILE* in, FILE* out) {
	error_t error = ERROR_SUCCESS;
	vector_ptr_t lexemes = {.size = -1};

	error = lexeme_read(in, &lexemes);
	if (error) goto cleanup;

	for (int k = 0; k != vector_ptr_size(&lexemes); ++k) {
		lexeme_t* lexeme = vector_ptr_get(&lexemes, k);

		error = process_lexeme(lexeme->value, out);
		if (error) goto cleanup;
	}

cleanup:
	if (vector_ptr_size(&lexemes) != -1) {
		lexeme_destroy(&lexemes);
	}
	return error;
}
