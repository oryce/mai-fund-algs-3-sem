#include "tasks.h"

#include <stdio.h>

#include "lib/chars.h"
#include "lib/lexeme.h"
#include "lib/mth.h"

error_t process_lexeme(const char* lexeme, FILE* out) {
	int base = 2;
	int sign = *lexeme == '-' ? -1 : 1;

	// Skip the first char (minus sign).
	const char* start = sign == -1 ? lexeme + 1 : lexeme;
	char* ptr;

	for (ptr = (char*)start; *ptr != '\0'; ++ptr) {
		if (!chars_is_alpha(*ptr) && !chars_is_digit(*ptr)) {
			THROWF(UnexpectedTokenException, "unexpected char in lexeme: %c", *ptr);
		}

		int ord = chars_is_alpha(*ptr) ? 10 + (chars_lower(*ptr) - 'a') : *ptr - '0';
		base = mth_int_max(base, ord + 1);
	}

	// Convert the number from |base| to base 10.
	long base10 = 0;
	long multiplier = 1;

	// Traverse backwards (|ptr| is '\0' when the first loop finishes).
	for (ptr = ptr - 1; ptr >= start; --ptr) {
		int ord = chars_is_alpha(*ptr) ? 10 + (chars_lower(*ptr) - 'a') : *ptr - '0';

		base10 += ord * multiplier;
		if (base10 < 0) {
			THROW(OverflowException, "input number is too large");
		}

		multiplier *= base;
	}

	base10 *= sign;

	// Skip leading zeros in the original number
	// (|ptr| is back at the beginning of the number).
	do {
		++ptr;
	} while (*ptr == '0');

	if (sign == -1) {
		fprintf(out, "-%s %d %ld\n", ptr, base, base10);
	} else {
		fprintf(out, "%s %d %ld\n", ptr, base, base10);
	}
	if (ferror(out)) {
		THROW(IOException, "can't write to output file");
	}

	return NO_EXCEPTION;
}

void cleanup_(vector_str_t* lexemes) {
	if (vector_str_size(lexemes) != -1) {
		lexeme_destroy(lexemes);
	}
}

error_t determine_min_number_bases(FILE* in, FILE* out) {
	error_t error;
	vector_str_t lexemes = {.size = -1};

	if (FAILED((error = lexeme_read(in, &lexemes)))) {
		cleanup_(&lexemes);
		PASS(error);
	}

	for (int k = 0; k != vector_str_size(&lexemes); ++k) {
		string_t lexeme = vector_str_get(&lexemes, k);
		if (FAILED((error = process_lexeme(string_to_c_str(&lexeme), out)))) {
			cleanup_(&lexemes);
			PASS(error);
		}
	}

	return NO_EXCEPTION;
}
