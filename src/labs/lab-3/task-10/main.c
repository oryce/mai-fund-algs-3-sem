#include <stdio.h>

#include "lib/io.h"
#include "task.h"

int main_cleanup(int retval, FILE* inputFile, FILE* outputFile) {
	fclose(inputFile);
	fclose(outputFile);

	return retval;
}

int main(int argc, char** argv) {
	if (argc != 3) {
		fprintf(stderr, "usage: %s <input file> <output file>", argv[0]);
		return 1;
	}

	FILE* inputFile = NULL;
	FILE* outputFile = NULL;

	if (!(inputFile = fopen(argv[1], "r"))) {
		fprintf(stderr, "Can't open input file for reading.\n");
		return main_cleanup(2, inputFile, outputFile);
	}
	if (!(outputFile = fopen(argv[2], "w"))) {
		fprintf(stderr, "Can't open output file for writing.\n");
		return main_cleanup(3, inputFile, outputFile);
	}

	char* line;
	size_t size;

	while (getline(&line, &size, inputFile) > 0) {
		char* newline = strrchr(line, '\n');
		if (newline) *newline = '\0';

		node_t* tree = tree_from_expr(line);
		if (!tree) {
			fprintf(stderr, "Can't parse expression %s\n", line);
			return main_cleanup(4, inputFile, outputFile);
		}

		node_print(outputFile, tree);
		fprintf(outputFile, "\n");

		node_destroy(tree);

		if (ferror(outputFile)) {
			fprintf(stderr, "Can't write to the output file.\n");
			return main_cleanup(5, inputFile, outputFile);
		}
	}

	if (ferror(inputFile)) {
		fprintf(stderr, "Can't read from the input file.\n");
		return main_cleanup(6, inputFile, outputFile);
	}

	return main_cleanup(0, inputFile, outputFile);
}
