#include "cmd.h"

#include "lib/error.h"
#include "lib/paths.h"
#include "tasks.h"

void cmd_merge_cleanup_(FILE* input1, FILE* input2, FILE* output) {
	fclose(input1);
	fclose(input2);
	fclose(output);
}

error_t cmd_merge_lexemes(int argc, char** argv) {
	error_t status;
	FILE* input1 = NULL;
	FILE* input2 = NULL;
	FILE* output = NULL;

	if (argc != 5) {
		fprintf(stderr, "Invalid arguments, see usage for more info.");
		return NO_EXCEPTION;
	}

	bool same_2_4, same_3_4;
	if (FAILED((status = paths_same(argv[2], argv[4], &same_2_4))) ||
	    FAILED((status = paths_same(argv[3], argv[4], &same_3_4)))) {
		cmd_merge_cleanup_(input1, input2, output);
		fprintf(stderr, "Invalid arguments: paths don't exist or are malformed.\n");
		return NO_EXCEPTION;
	}
	if (same_2_4 || same_3_4) {
		cmd_merge_cleanup_(input1, input2, output);
		fprintf(stderr, "Invalid arguments: input path may not match the output path.\n");
		return NO_EXCEPTION;
	}

	input1 = fopen(argv[2], "r");
	if (input1 == NULL) {
		cmd_merge_cleanup_(input1, input2, output);
		fprintf(stderr, "Unable to open file 1 for reading.\n");
		return NO_EXCEPTION;
	}

	input2 = fopen(argv[3], "r");
	if (input2 == NULL) {
		cmd_merge_cleanup_(input1, input2, output);
		fprintf(stderr, "Unable to open file 2 for reading.\n");
		return NO_EXCEPTION;
	}

	output = fopen(argv[4], "w");
	if (output == NULL) {
		cmd_merge_cleanup_(input1, input2, output);
		fprintf(stderr, "Unable to open output file for writing.\n");
		return NO_EXCEPTION;
	}

	status = task_merge_lexemes(input1, input2, output);
	cmd_merge_cleanup_(input1, input2, output);

	return status;
}

void cmd_process_cleanup_(FILE* input, FILE* output) {
	fclose(input);
	fclose(output);
}

error_t cmd_process_lexemes(int argc, char** argv) {
	error_t status;
	FILE* input = NULL;
	FILE* output = NULL;

	if (argc != 4) {
		fprintf(stderr, "Invalid arguments, see usage for more info.");
		return NO_EXCEPTION;
	}

	bool samePaths;
	if (FAILED((status = paths_same(argv[2], argv[3], &samePaths)))) {
		cmd_process_cleanup_(input, output);
		fprintf(stderr, "Invalid arguments: paths don't exist or are malformed.\n");
		return NO_EXCEPTION;
	}
	if (samePaths) {
		cmd_process_cleanup_(input, output);
		fprintf(stderr, "Invalid arguments: input path may not match the output path.\n");
		return NO_EXCEPTION;
	}

	input = fopen(argv[2], "r");
	if (input == NULL) {
		cmd_process_cleanup_(input, output);
		fprintf(stderr, "Unable to open input file for reading.\n");
		return NO_EXCEPTION;
	}

	output = fopen(argv[3], "w");
	if (output == NULL) {
		cmd_process_cleanup_(input, output);
		fprintf(stderr, "Unable to open output file for writing.\n");
		return NO_EXCEPTION;
	}

	status = task_process_lexemes(input, output);
	cmd_process_cleanup_(input, output);

	return status;
}
