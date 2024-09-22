#include "tasks.h"

#include <stdio.h>

#include "lexeme.h"
#include "lib/chars.h"
#include "lib/conv.h"

error_t merge_lexemes(int argc, char** argv) {
	error_t error = ERROR_SUCCESS;

	vector_ptr_t firstLexemes = {.size = 0};
	vector_ptr_t secondLexemes = {.size = 0};
	FILE* inputFile1 = NULL;
	FILE* inputFile2 = NULL;
	FILE* outputFile = NULL;

	if (argc != 5) {
		error = ERROR_INVALID_PARAMETER;
		goto cleanup;
	}

	inputFile1 = fopen(argv[2], "r");
	if (inputFile1 == NULL) {
		fprintf(stderr, "Unable to open file 1 for reading.\n");
		error = ERROR_IO;
		goto cleanup;
	}

	inputFile2 = fopen(argv[3], "r");
	if (inputFile2 == NULL) {
		fprintf(stderr, "Unable to open file 2 for reading.\n");
		error = ERROR_IO;
		goto cleanup;
	}

	outputFile = fopen(argv[4], "w");
	if (outputFile == NULL) {
		fprintf(stderr, "Unable to open output file for writing.\n");
		error = ERROR_IO;
		goto cleanup;
	}

	error = lexeme_read(inputFile1, &firstLexemes);
	if (error) {
		fprintf(stderr, "Unable to read lexemes from the first file.\n");
		goto cleanup;
	}

	error = lexeme_read(inputFile2, &secondLexemes);
	if (error) {
		fprintf(stderr, "Unable to read lexemes from the second file.\n");
		goto cleanup;
	}

	size_t i = 0, j = 0;
	size_t n = vector_ptr_size(&firstLexemes);
	size_t m = vector_ptr_size(&secondLexemes);

	for (; i < n && j < m; ++i, ++j) {
		lexeme_t* first = vector_ptr_get(&firstLexemes, i);
		lexeme_t* second = vector_ptr_get(&secondLexemes, j);

		fprintf(outputFile, "%s %s ", first->value, second->value);
		if (ferror(outputFile)) {
			error = ERROR_IO;
			goto cleanup;
		}
	}

	size_t k = 0;
	vector_ptr_t remainingVec;

	if (i == n) {
		k = j;
		remainingVec = secondLexemes;
	} else {
		k = i;
		remainingVec = firstLexemes;
	}

	for (; k < vector_ptr_size(&remainingVec); ++k) {
		lexeme_t* lexeme = vector_ptr_get(&remainingVec, k);

		fprintf(outputFile, "%s ", lexeme->value);
		if (ferror(outputFile)) {
			error = ERROR_IO;
			goto cleanup;
		}
	}

cleanup:
	fclose(inputFile1);
	fclose(inputFile2);
	fclose(outputFile);

	if (!vector_ptr_is_empty(&firstLexemes)) {
		lexeme_destroy(&firstLexemes);
	}
	if (!vector_ptr_is_empty(&firstLexemes)) {
		lexeme_destroy(&secondLexemes);
	}

	return error;
}

error_t process_lexemes(int argc, char** argv) {
	error_t error = ERROR_SUCCESS;

	vector_ptr_t lexemes = {.size = 0};
	FILE* inputFile = NULL;
	FILE* outputFile = NULL;

	if (argc != 4) {
		error = ERROR_INVALID_PARAMETER;
		goto cleanup;
	}

	inputFile = fopen(argv[2], "r");
	if (inputFile == NULL) {
		fprintf(stderr, "Unable to open input file for reading.\n");
		error = ERROR_IO;
		goto cleanup;
	}

	outputFile = fopen(argv[3], "w");
	if (outputFile == NULL) {
		fprintf(stderr, "Unable to open output file for writing.\n");
		error = ERROR_IO;
		goto cleanup;
	}

	error = lexeme_read(inputFile, &lexemes);
	if (error) {
		fprintf(stderr, "Unable to read lexemes from the input file.\n");
		goto cleanup;
	}

	char numberToBase[65];  // Holds the current number being converted to a base. Big enough to hold a 64-bit number
	                        // (+null-byte)

	for (int k = 0; k != vector_ptr_size(&lexemes); ++k) {
		char* lexeme = (char*)((lexeme_t*)vector_ptr_get(&lexemes, k))->value;

		int n = k + 1;

		if (n % 10 == 0) {
			for (; *lexeme != '\0'; ++lexeme) {
				error = long_to_base(chars_lower(*lexeme), 4, numberToBase, 65);
				if (error) goto cleanup;

				fprintf(outputFile, "%s", numberToBase);
			}

			fputc(' ', outputFile);
		} else if (n % 2 == 0) {
			for (; *lexeme != '\0'; ++lexeme) {
				fputc(chars_lower(*lexeme), outputFile);
			}

			fputc(' ', outputFile);
		} else if (n % 5 == 0) {
			for (; *lexeme != '\0'; ++lexeme) {
				error = long_to_base(chars_lower(*lexeme), 8, numberToBase, 65);
				if (error) goto cleanup;

				fprintf(outputFile, "%s", numberToBase);
			}

			fputc(' ', outputFile);
		} else {
			fprintf(outputFile, "%s ", lexeme);
		}

		if (ferror(outputFile)) {
			error = ERROR_IO;
			goto cleanup;
		}
	}

cleanup:
	fclose(inputFile);
	fclose(outputFile);

	if (!vector_ptr_is_empty(&lexemes)) {
		lexeme_destroy(&lexemes);
	}

	return error;
}
