#include "tasks.h"

#include <stdio.h>

#include "lib/chars.h"
#include "lib/conv.h"
#include "lib/lexeme.h"
#include "lib/paths.h"

error_t merge_lexemes(int argc, char** argv) {
	error_t error = ERROR_SUCCESS;

	vector_str_t firstLexemes = {.size = -1};
	vector_str_t secondLexemes = {.size = -1};
	FILE* inputFile1 = NULL;
	FILE* inputFile2 = NULL;
	FILE* outputFile = NULL;

	if (argc != 5) {
		error = ERROR_INVALID_PARAMETER;
		goto cleanup;
	}

	bool samePaths = false;
	bool samePathsResult;

	error = paths_same(argv[2], argv[4], &samePathsResult);
	if (error) goto cleanup;
	samePaths |= samePathsResult;

	error = paths_same(argv[3], argv[4], &samePathsResult);
	if (error) goto cleanup;
	samePaths |= samePathsResult;

	if (samePaths) {
		fprintf(stderr, "Input and output files may not be the same.\n");
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
	size_t n = vector_str_size(&firstLexemes);
	size_t m = vector_str_size(&secondLexemes);

	for (; i < n && j < m; ++i, ++j) {
		string_t first = vector_str_get(&firstLexemes, i);
		string_t second = vector_str_get(&secondLexemes, j);

		fprintf(outputFile, "%s %s ", string_to_c_str(&first), string_to_c_str(&second));

		if (ferror(outputFile)) {
			error = ERROR_IO;
			goto cleanup;
		}
	}

	size_t k = 0;
	vector_str_t remainingVec;

	if (i == n) {
		k = j;
		remainingVec = secondLexemes;
	} else {
		k = i;
		remainingVec = firstLexemes;
	}

	for (; k < vector_str_size(&remainingVec); ++k) {
		string_t lexeme = vector_str_get(&remainingVec, k);
		fprintf(outputFile, "%s ", string_to_c_str(&lexeme));

		if (ferror(outputFile)) {
			error = ERROR_IO;
			goto cleanup;
		}
	}

cleanup:
	fclose(inputFile1);
	fclose(inputFile2);
	fclose(outputFile);

	if (vector_str_size(&firstLexemes) != -1) {
		lexeme_destroy(&firstLexemes);
	}
	if (vector_str_size(&secondLexemes) != -1) {
		lexeme_destroy(&secondLexemes);
	}

	return error;
}

error_t process_lexemes(int argc, char** argv) {
	error_t error = ERROR_SUCCESS;

	vector_str_t lexemes = {.size = -1};
	FILE* inputFile = NULL;
	FILE* outputFile = NULL;

	if (argc != 4) {
		error = ERROR_INVALID_PARAMETER;
		goto cleanup;
	}

	bool samePaths;

	error = paths_same(argv[2], argv[3], &samePaths);
	if (error) goto cleanup;

	if (samePaths) {
		fprintf(stderr, "Input and output paths may not be the same.\n");
		error = ERROR_IO;
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

	for (int k = 0; k != vector_str_size(&lexemes); ++k) {
		string_t lexeme = vector_str_get(&lexemes, k);
		const char* value = string_to_c_str(&lexeme);

		int n = k + 1;

		if (n % 10 == 0) {
			for (; *value != '\0'; ++value) {
				error = conv_to_arb_base(chars_lower(*value), 4, numberToBase, 65);
				if (error) goto cleanup;

				fprintf(outputFile, "%s", numberToBase);
			}

			fputc(' ', outputFile);
		} else if (n % 2 == 0) {
			for (; *value != '\0'; ++value) {
				fputc(chars_lower(*value), outputFile);
			}

			fputc(' ', outputFile);
		} else if (n % 5 == 0) {
			for (; *value != '\0'; ++value) {
				error = conv_to_arb_base(chars_lower(*value), 8, numberToBase, 65);
				if (error) goto cleanup;

				fprintf(outputFile, "%s", numberToBase);
			}

			fputc(' ', outputFile);
		} else {
			fprintf(outputFile, "%s ", value);
		}

		if (ferror(outputFile)) {
			error = ERROR_IO;
			goto cleanup;
		}
	}

cleanup:
	fclose(inputFile);
	fclose(outputFile);

	if (vector_str_size(&lexemes) != -1) {
		lexeme_destroy(&lexemes);
	}

	return error;
}
