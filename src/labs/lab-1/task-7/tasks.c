#include "tasks.h"

#include <stdio.h>

#include "lib/chars.h"
#include "lib/convert.h"
#include "lib/io.h"
#include "lib/lexeme.h"

void task_merge_cleanup_(vector_str_t* lexemes1, vector_str_t* lexemes2) {
	if (vector_str_size(lexemes1) != -1) lexeme_destroy(lexemes1);
	if (vector_str_size(lexemes2) != -1) lexeme_destroy(lexemes2);
}

error_t task_merge_lexemes(FILE* input1, FILE* input2, FILE* output) {
	error_t status;
	vector_str_t lexemes1 = {.size = -1};
	vector_str_t lexemes2 = {.size = -1};

	if ((status = lexeme_read(input1, &lexemes1)) ||
	    (status = lexeme_read(input2, &lexemes2))) {
		task_merge_cleanup_(&lexemes1, &lexemes2);
		return status;
	}

	size_t i = 0, j = 0;
	size_t n = vector_str_size(&lexemes1);
	size_t m = vector_str_size(&lexemes2);

	for (; i < n && j < m; ++i, ++j) {
		string_t* first = vector_str_get(&lexemes1, i);
		string_t* second = vector_str_get(&lexemes2, j);

		fprintf(output, "%s %s ", string_to_c_str(first),
		        string_to_c_str(second));
		if (ferror(output)) {
			task_merge_cleanup_(&lexemes1, &lexemes2);
			return ERROR_IO;
		}
	}

	size_t k = 0;
	vector_str_t remainingVec;

	if (i == n) {
		k = j;
		remainingVec = lexemes2;
	} else {
		k = i;
		remainingVec = lexemes1;
	}

	for (; k < vector_str_size(&remainingVec); ++k) {
		string_t* lexeme = vector_str_get(&remainingVec, k);
		fprintf(output, "%s ", string_to_c_str(lexeme));
		if (ferror(output)) {
			task_merge_cleanup_(&lexemes1, &lexemes2);
			return ERROR_IO;
		}
	}

	task_merge_cleanup_(&lexemes1, &lexemes2);
	return 0;
}

void task_process_cleanup_(vector_str_t* lexemes) {
	if (vector_str_size(lexemes) != -1) lexeme_destroy(lexemes);
}

error_t task_process_lexemes(FILE* input, FILE* output) {
	error_t status;
	vector_str_t lexemes = {.size = -1};

	if ((status = lexeme_read(input, &lexemes))) {
		task_process_cleanup_(&lexemes);
		return status;
	}

	char numberToBase[65];  // Holds the current number being converted to a
	                        // base. Big enough to hold a 64-bit number.

	for (int k = 0; k != vector_str_size(&lexemes); ++k) {
		string_t* lexeme = vector_str_get(&lexemes, k);
		const char* value = string_to_c_str(lexeme);

		int n = k + 1;

		if (n % 10 == 0) {
			for (; *value != '\0'; ++value) {
				status = long_to_base(chars_lower(*value), 4, numberToBase,
				                      sizeof(numberToBase));
				if (status) {
					task_process_cleanup_(&lexemes);
					return status;
				}

				fprintf(output, "%s", numberToBase);
			}

			fputc(' ', output);
		} else if (n % 2 == 0) {
			for (; *value != '\0'; ++value) {
				fputc(chars_lower(*value), output);
			}

			fputc(' ', output);
		} else if (n % 5 == 0) {
			for (; *value != '\0'; ++value) {
				status = long_to_base(chars_lower(*value), 8, numberToBase,
				                      sizeof(numberToBase));
				if (status) {
					task_process_cleanup_(&lexemes);
					return status;
				}

				fprintf(output, "%s", numberToBase);
			}

			fputc(' ', output);
		} else {
			fprintf(output, "%s ", value);
		}

		if (ferror(output)) {
			task_process_cleanup_(&lexemes);
			return ERROR_IO;
		}
	}

	task_process_cleanup_(&lexemes);
	return 0;
}
