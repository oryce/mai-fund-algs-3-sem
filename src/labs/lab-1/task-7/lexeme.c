#include "lexeme.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "lib/error.h"
#include "lib/string.h"
#include "lib/vector.h"

void lexeme_destroy(vector_ptr_t* lexemes) {
	for (size_t i = 0; i != vector_ptr_size(lexemes); ++i) {
		lexeme_t* lexeme = (lexeme_t*)vector_ptr_get(lexemes, i);
		free((void*)lexeme->value);
		free(lexeme);
	}

	vector_ptr_destroy(lexemes);
}

error_t lexeme_finalize(vector_ptr_t* lexemes, string_t* lexeme_value) {
	lexeme_t* lexeme = malloc(sizeof(lexeme_t));
	if (lexeme == NULL) return ERROR_HEAP_ALLOCATION;

	lexeme->value = string_to_c_str(lexeme_value);

	if (!vector_ptr_push_back(lexemes, lexeme)) {
		free(lexeme);
		return ERROR_HEAP_ALLOCATION;
	}

	return ERROR_SUCCESS;
}

error_t lexeme_read(FILE* file, vector_ptr_t* out) {
	vector_ptr_t lexemes = vector_ptr_create();
	error_t error;

	string_t lexeme_value = string_create();
	bool in_lexeme = false;

	int ch;
	while ((ch = fgetc(file)) != EOF) {
		if (ferror(file)) {
			error = ERROR_IO;
			goto cleanup;
		}

		if (ch == '\t' || ch == '\n' || ch == '\r' || ch == ' ') {
			if (in_lexeme) {
				error = lexeme_finalize(&lexemes, &lexeme_value);
				if (error) goto cleanup;

				lexeme_value = string_create();
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
		error = lexeme_finalize(&lexemes, &lexeme_value);
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

error_t lexeme_write(FILE* file, vector_ptr_t* lexemes, char sep) {
	for (int i = 0; i != vector_ptr_size(lexemes); ++i) {
		lexeme_t* lexeme = (lexeme_t*)vector_ptr_get(lexemes, i);

		fprintf(file, "%s%c", lexeme->value, sep);
		if (ferror(file)) return ERROR_IO;
	}

	return ERROR_SUCCESS;
}
