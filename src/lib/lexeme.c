#include "lib/lexeme.h"

#include <stdbool.h>
#include <stdio.h>

#include "lib/collections/string.h"
#include "lib/error.h"

void lexeme_destroy(vector_str_t* lexemes) {
	for (size_t i = 0; i != vector_str_size(lexemes); ++i) {
		string_t* lexeme = vector_str_get(lexemes, i);
		string_destroy(lexeme);
	}

	vector_str_destroy(lexemes);
}

void lexeme_read_cleanup_(vector_str_t* lexemes, string_t* lexeme) {
	vector_str_destroy(lexemes);
	string_destroy(lexeme);
}

error_t lexeme_read(FILE* file, vector_str_t* out) {
	vector_str_t lexemes = vector_str_create();
	string_t lexeme = string_create();

	if (!string_created(&lexeme)) {
		lexeme_read_cleanup_(&lexemes, &lexeme);
		THROW(MemoryError, "can't create lexeme buffer");
	}

	bool in_lexeme = false;

	int ch;
	while ((ch = fgetc(file)) != EOF) {
		if (ferror(file)) {
			lexeme_read_cleanup_(&lexemes, &lexeme);
			THROW(IOException, "can't read from input file");
		}

		if (ch == '\t' || ch == '\n' || ch == '\r' || ch == ' ') {
			if (in_lexeme) {
				if (!vector_str_push_back(&lexemes, lexeme)) {
					lexeme_read_cleanup_(&lexemes, &lexeme);
					THROW(MemoryError, "can't insert lexeme");
				}

				lexeme = string_create();
				if (!string_created(&lexeme)) {
					lexeme_read_cleanup_(&lexemes, &lexeme);
					THROW(MemoryError, "can't create new lexeme");
				}

				in_lexeme = false;
			}

			continue;
		}

		in_lexeme = true;

		if (!string_append_char(&lexeme, (char)ch)) {
			lexeme_read_cleanup_(&lexemes, &lexeme);
			THROW(MemoryError, "can't append char to lexeme");
		}
	}

	if (in_lexeme) {
		if (!vector_str_push_back(&lexemes, lexeme)) {
			lexeme_read_cleanup_(&lexemes, &lexeme);
			THROW(MemoryError, "can't insert lexeme");
		}
	} else {
		string_destroy(&lexeme);
	}

	*out = lexemes;
	return NO_EXCEPTION;
}

error_t lexeme_write(FILE* file, vector_str_t* lexemes, char sep) {
	for (int i = 0; i != vector_str_size(lexemes); ++i) {
		string_t* lexeme = vector_str_get(lexemes, i);

		fprintf(file, "%s%c", string_to_c_str(lexeme), sep);
		if (ferror(file)) THROW(IOException, "can't write lexeme to file");
	}

	return NO_EXCEPTION;
}
