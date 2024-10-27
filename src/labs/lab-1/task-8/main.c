#include <stdio.h>

#include "lib/error.h"
#include "lib/io.h"
#include "tasks.h"

void cleanup(FILE* inputFile, FILE* outputFile) {
	fclose(inputFile);
	fclose(outputFile);
}

error_t main_(int argc, char** argv) {
	FILE* inputFile = NULL;
	FILE* outputFile = NULL;

	if (argc != 3) {
		printf(
		    "Usage: %s <input file> <output file>\n"
		    "Determines the minimum base for each number in the input file.\n",
		    argv[0]);
		return 0;
	}

	bool samePaths;
	if (paths_same(argv[1], argv[2], &samePaths) || samePaths) {
		fprintf(stderr,
		        "Invalid arguments: paths are malformed or are the same.\n");
		return 0;
	}

	inputFile = fopen(argv[1], "r");
	if (inputFile == NULL) {
		cleanup(inputFile, outputFile);
		fprintf(stderr, "Can't open input file for reading.\n");
		return 0;
	}

	outputFile = fopen(argv[2], "w");
	if (outputFile == NULL) {
		cleanup(inputFile, outputFile);
		fprintf(stderr, "Can't open output file for writing.\n");
		return 0;
	}

	return determine_min_number_bases(inputFile, outputFile);
}

int main(int argc, char** argv) {
	error_t error = main_(argc, argv);
	if (error) {
		error_print(error);
		return error;
	}
}
