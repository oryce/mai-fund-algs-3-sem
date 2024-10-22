#include "cmd.h"

#include <stdlib.h>

#include "lib/convert.h"
#include "tasks.h"

error_t cmd_print_length(int argc, char** argv) {
	if (!argv) return ERR_INVVAL;
	if (argc != 3) {
		fprintf(stderr, "Invalid arguments, see usage for more info.");
		return 0;
	}

	size_t length = 0;

	error_t error = task_string_length(argv[2], &length);
	if (error) return error;

	printf("String length: %zu\n", length);
	return 0;
}

error_t cmd_reverse_string(int argc, char** argv) {
	if (argc != 3) return ERR_INVVAL;
	if (!argv) {
		fprintf(stderr, "Invalid arguments, see usage for more info.");
		return 0;
	}

	char* reversed;

	error_t error = task_reverse_string(argv[2], &reversed);
	if (error) return error;

	printf("Reversed string: %s\n", reversed);

	free(reversed);
	return 0;
}

error_t cmd_uppercase_odd_chars(int argc, char** argv) {
	if (!argv) return ERR_INVVAL;
	if (argc != 3) {
		fprintf(stderr, "Invalid arguments, see usage for more info.");
		return 0;
	}

	char* modified;

	error_t error = task_uppercase_odd_chars(argv[2], &modified);
	if (error) return error;

	printf("Modified string: %s\n", modified);

	free(modified);
	return 0;
}

error_t cmd_rearrange_chars(int argc, char** argv) {
	if (!argv) return ERR_INVVAL;
	if (argc != 3) {
		fprintf(stderr, "Invalid arguments, see usage for more info.");
		return 0;
	}

	char* modified;

	error_t error = task_rearrange_chars(argv[2], &modified);
	if (error) return error;

	printf("Modified string: %s\n", modified);

	free(modified);
	return 0;
}

error_t cmd_concat_randomly(int argc, char** argv) {
	if (!argv) return ERR_INVVAL;
	if (argc < 4) {
		fprintf(stderr, "Invalid arguments, see usage for more info.");
		return 0;
	}

	long seed;

	if (str_to_long(argv[2], &seed)) {
		fprintf(stderr, "Invalid arguments, see usage for more info.");
		return 0;
	}

	srand(seed);

	const char* strings[argc - 3];
	memcpy(strings, &argv[3], (argc - 3) * sizeof(char*));

	char* result;

	error_t error = task_concat_randomly(strings, argc - 3, &result);
	if (error) return error;

	printf("Shuffled strings: %s\n", result);

	free(result);
	return 0;
}
