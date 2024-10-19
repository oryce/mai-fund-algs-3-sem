#include <stdio.h>

#include "lib/error.h"
#include "lib/paths.h"
#include "tasks.h"

void cleanup(FILE* inputFile, FILE* outputFile) {
	fclose(inputFile);
	fclose(outputFile);
}

error_t main_(int argc, char** argv) {
	error_t error;
	FILE* inputFile = NULL;
	FILE* outputFile = NULL;

	if (argc != 3) {
		printf(
		    "Usage: %s <input file> <output file>\n"
		    "Determines the minimum base for each number in the input file.\n",
		    argv[0]);
		return NO_EXCEPTION;
	}

	bool samePaths;
	if (FAILED((error = paths_same(argv[1], argv[2], &samePaths)))) {
		fprintf(stderr, "Invalid arguments: paths don't exist or are malformed.\n");
		return NO_EXCEPTION;
	}
	if (samePaths) {
		fprintf(stderr, "Invalid arguments: input and output paths may not be the same.\n");
		return NO_EXCEPTION;
	}

	inputFile = fopen(argv[1], "r");
	if (inputFile == NULL) {
		cleanup(inputFile, outputFile);
		fprintf(stderr, "Can't open input file for reading.\n");
		return NO_EXCEPTION;
	}

	outputFile = fopen(argv[2], "w");
	if (outputFile == NULL) {
		cleanup(inputFile, outputFile);
		fprintf(stderr, "Can't open output file for writing.\n");
		return NO_EXCEPTION;
	}

	return determine_min_number_bases(inputFile, outputFile);
}

int main(int argc, char** argv) {
	error_t error;
	if (FAILED((error = main_(argc, argv)))) {
		error_print(error);
		return (int)error.code;
	}
}
