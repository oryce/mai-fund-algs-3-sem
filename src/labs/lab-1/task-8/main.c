#include <stdio.h>

#include "lib/error.h"
#include "lib/paths.h"
#include "tasks.h"

int main(int argc, char** argv) {
	error_t error = ERROR_SUCCESS;

	FILE* inputFile = NULL;
	FILE* outputFile = NULL;

	if (argc != 3) {
		printf(
		    "Usage: %s <input file> <output file>\n"
		    "Determines the minimum base for each number in the input file.\n",
		    argv[0]);
		return -ERROR_INVALID_PARAMETER;
	}

	bool samePaths;

	error = paths_same(argv[1], argv[2], &samePaths);
	if (error) goto cleanup;

	if (samePaths) {
		fprintf(stderr, "Input and output files may not be the same.\n");

		error = ERROR_INVALID_PARAMETER;
		goto cleanup;
	}

	inputFile = fopen(argv[1], "r");
	if (inputFile == NULL) {
		fprintf(stderr, "Unable to open the input file for reading.\n");

		error = ERROR_IO;
		goto cleanup;
	}

	outputFile = fopen(argv[2], "w");
	if (outputFile == NULL) {
		fprintf(stderr, "Unable to open the output file for writing.\n");

		error = ERROR_IO;
		goto cleanup;
	}

	error = determine_min_number_bases(inputFile, outputFile);

cleanup:
	fclose(inputFile);
	fclose(outputFile);

	if (error) {
		error_print(error);
		return (int) -error;
	}
}
