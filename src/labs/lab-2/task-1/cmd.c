#include "cmd.h"

#include <stdlib.h>

#include "lib/convert.h"
#include "tasks.h"

error_t cmd_print_length(int argc, char** argv) {
	if (argv == NULL) THROW(IllegalArgumentException, "`argv` may not be null");
	if (argc != 3) {
		fprintf(stderr, "Invalid arguments, see usage for more info.");
		return NO_EXCEPTION;
	}

	error_t status;

	size_t length;
	if (FAILED((status = task_string_length(argv[2], &length)))) {
		PASS(status);
	}

	printf("String length: %zu\n", length);
	return NO_EXCEPTION;
}

error_t cmd_reverse_string(int argc, char** argv) {
	if (argc != 3) THROW(IllegalArgumentException, "bad argument count");
	if (argv == NULL) {
		fprintf(stderr, "Invalid arguments, see usage for more info.");
		return NO_EXCEPTION;
	}

	error_t status;

	char* reversed;
	if (FAILED((status = task_reverse_string(argv[2], &reversed)))) {
		PASS(status);
	}

	printf("Reversed string: %s\n", reversed);
	free(reversed);
	return NO_EXCEPTION;
}

error_t cmd_uppercase_odd_chars(int argc, char** argv) {
	if (argv == NULL) THROW(IllegalArgumentException, "`argv` may not be null");
	if (argc != 3) {
		fprintf(stderr, "Invalid arguments, see usage for more info.");
		return NO_EXCEPTION;
	}

	error_t status;

	char* modified;
	if (FAILED((status = task_uppercase_odd_chars(argv[2], &modified)))) {
		PASS(status);
	}

	printf("Modified string: %s\n", modified);
	free(modified);
	return NO_EXCEPTION;
}

error_t cmd_rearrange_chars(int argc, char** argv) {
	if (argv == NULL) THROW(IllegalArgumentException, "`argv` may not be null");
	if (argc != 3) {
		fprintf(stderr, "Invalid arguments, see usage for more info.");
		return NO_EXCEPTION;
	}

	error_t status;

	char* modified;
	if (FAILED((status = task_rearrange_chars(argv[2], &modified)))) {
		PASS(status);
	}

	printf("Modified string: %s\n", modified);
	free(modified);
	return NO_EXCEPTION;
}

error_t cmd_concat_randomly(int argc, char** argv) {
	if (argv == NULL) THROW(IllegalArgumentException, "`argv` may not be null");
	if (argc < 4) {
		fprintf(stderr, "Invalid arguments, see usage for more info.");
		return NO_EXCEPTION;
	}

	error_t status;

	long seed;
	if (FAILED((status = str_to_long(argv[2], &seed)))) {
		fprintf(stderr, "Invalid arguments, see usage for more info.");
		return NO_EXCEPTION;
	}

	srand(seed);

	const char* strings[argc - 3];
	memcpy(strings, &argv[3], (argc - 3) * sizeof(char*));

	char* result;
	if (FAILED((status = task_concat_randomly(strings, argc - 3, &result)))) {
		PASS(status);
	}

	printf("Shuffled strings: %s\n", result);
	free(result);
	return NO_EXCEPTION;
}
