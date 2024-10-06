#include "lib/lexeme.h"

#include <stdbool.h>
#include <stdio.h>

#include "lib/collections/string.h"
#include "lib/error.h"

void lexeme_destroy(vector_str_t* lexemes) {
	for (size_t i = 0; i != vector_str_size(lexemes); ++i) {
		string_t lexeme = vector_str_get(lexemes, i);
		string_destroy(&lexeme);
	}

	vector_str_destroy(lexemes);
}

error_t lexeme_finalize(vector_str_t* lexemes, string_t lexeme_value) {
	return vector_str_push_back(lexemes, lexeme_value) ? ERROR_SUCCESS : ERROR_HEAP_ALLOCATION;
}

error_t lexeme_read(FILE* file, vector_str_t* out) {
	error_t error;
	vector_str_t lexemes = vector_str_create();

	string_t lexeme_value = string_create();
	if (!lexeme_value.initialized) {
		error = ERROR_HEAP_ALLOCATION;
		goto cleanup;
	}

	bool in_lexeme = false;

	int ch;
	while ((ch = fgetc(file)) != EOF) {
		if (ferror(file)) {
			error = ERROR_IO;
			goto cleanup;
		}

		if (ch == '\t' || ch == '\n' || ch == '\r' || ch == ' ') {
			if (in_lexeme) {
				error = lexeme_finalize(&lexemes, lexeme_value);
				if (error) goto cleanup;

				lexeme_value = string_create();
				if (!lexeme_value.initialized) {
					error = ERROR_HEAP_ALLOCATION;
					goto cleanup;
				}

				in_lexeme = false;
			}

			continue;
		}

		in_lexeme = true;

		if (!string_append_char(&lexeme_value, (char)ch)) {
			error = ERROR_HEAP_ALLOCATION;
			goto cleanup;
		}
	}

	if (in_lexeme) {
		error = lexeme_finalize(&lexemes, lexeme_value);
		if (error) goto cleanup;
	} else {
		string_destroy(&lexeme_value);
	}

	*out = lexemes;
	return ERROR_SUCCESS;

cleanup:
	string_destroy(&lexeme_value);
	lexeme_destroy(&lexemes);
	return error;
}

error_t lexeme_write(FILE* file, vector_str_t* lexemes, char sep) {
	for (int i = 0; i != vector_str_size(lexemes); ++i) {
		string_t lexeme = vector_str_get(lexemes, i);
		fprintf(file, "%s%c", string_to_c_str(&lexeme), sep);

		if (ferror(file)) return ERROR_IO;
	}

	return ERROR_SUCCESS;
}
