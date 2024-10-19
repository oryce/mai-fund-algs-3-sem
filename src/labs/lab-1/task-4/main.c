#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib/error.h"
#include "lib/paths.h"
#include "tasks.h"

typedef error_t (*opt_handler_t)(FILE* in, FILE* out);

typedef struct opt {
	char name[16];
	char desc[256];
	opt_handler_t handler;
	bool specify_output;
} opt_t;

error_t parse_opt(const char* flag, opt_t opts[], int nOpts, opt_t* outOpt) {
	if (flag[0] != '-' && flag[0] != '/') {
		THROW(IllegalArgumentException, "flag starts with an invalid character");
	}

	++flag;

	bool specify_output = false;
	if (*flag == 'n') {
		specify_output = true;
		++flag;
	}

	for (int i = 0; i != nOpts; ++i) {
		if (strncmp(opts[i].name, flag, 16) == 0) {
			*outOpt = opts[i];
			outOpt->specify_output = specify_output;

			return NO_EXCEPTION;
		}
	}

	THROWF(IllegalArgumentException, "unrecognized option: %c", flag[0]);
}

void print_opts(opt_t opts[], int nOpts) {
	for (int i = 0; i != nOpts; ++i) {
		opt_t opt = opts[i];
		fprintf(stdout, "  /%s, -%s: %s\n", opt.name, opt.name, opt.desc);
	}
}

error_t add_prefix(const char* path, char** out) {
	const char* prefix = "out_";

	*out = (char*)malloc(strlen(path) + strlen(prefix) + 1);
	if (*out == NULL) THROW(MemoryError, "can't allocate for modified path");

	char delimiter;
#ifdef _WIN32
	delimiter = '\\';
#else
	delimiter = '/';
#endif

	const char* lastSeparator = strrchr(path, delimiter);

	if (lastSeparator == NULL) {
		strcat(*out, prefix);
		strcat(*out, path);
	} else {
		strncat(*out, path, lastSeparator - path + 1);
		strcat(*out, prefix);
		strcat(*out, lastSeparator + 1);
	}

	return NO_EXCEPTION;
}

void cleanup(FILE* input, FILE* output, char* outputPath) {
	fclose(input);
	fclose(output);
	free(outputPath);
}

error_t main_(int argc, char** argv) {
	error_t status = NO_EXCEPTION;

	opt_t opts[] = {
	    {"d", "removes digit characters from the input file", &task_remove_digits},
	    {"i", "appends the letter count to each line", &task_count_letters},
	    {"s", "appends the special character count to each line", &task_count_special_characters},
	    {"a", "encodes non-digit characters in hex ASCII", &task_encode_non_digits},
	};
	int nOpts = sizeof(opts) / sizeof(opt_t);

	if (argc != 3 && argc != 4) {
		printf(
		    "Usage: %s <flag> <input file> [output file]\n"
		    "Append 'n' before the flag ('/nd') to specify the output file.\n"
		    "Flags:\n",
		    argv[0]);
		print_opts(opts, nOpts);
		return NO_EXCEPTION;
	}

	opt_t opt;
	if (FAILED((status = parse_opt(argv[1], opts, nOpts, &opt)))) {
		fprintf(stderr, "Invalid arguments: %s.", status.message);
		return NO_EXCEPTION;
	}

	const char* inPath = argv[2];
	char* outPath = NULL;
	FILE* input = fopen(inPath, "r");
	FILE* output = NULL;

	if (input == NULL) {
		fprintf(stderr, "Can't open the input file.\n");
		return NO_EXCEPTION;
	}

	if (opt.specify_output) {
		if (argc != 4) {
			cleanup(input, output, outPath);
			fprintf(stderr, "Specify the output file.\n");
			return NO_EXCEPTION;
		}

		bool samePaths;
		if (FAILED((status = paths_same(argv[2], argv[3], &samePaths)))) {
			cleanup(input, output, outPath);
			fprintf(stderr, "Path is malformed.\n");
			return NO_EXCEPTION;
		}
		if (samePaths) {
			cleanup(input, output, outPath);
			fprintf(stderr, "Output path must be different from the input path.\n");
			return NO_EXCEPTION;
		}

		output = fopen(argv[3], "w");
		if (output == NULL) {
			cleanup(input, output, outPath);
			fprintf(stderr, "Can't open the output file.\n");
			return NO_EXCEPTION;
		}
	} else {
		if (FAILED((status = add_prefix(inPath, &outPath)))) {
			cleanup(input, output, outPath);
			PASS(status);
		}

		output = fopen(outPath, "w");
		if (output == NULL) {
			cleanup(input, output, outPath);
			fprintf(stderr, "Can't open the output file.\n");
			return NO_EXCEPTION;
		}
	}

	status = opt.handler(input, output);
	cleanup(input, output, outPath);

	if (FAILED(status)) {
		PASS(status);
	} else {
		return NO_EXCEPTION;
	}
}

int main(int argc, char** argv) {
	error_t error = main_(argc, argv);
	if (FAILED(error)) {
		error_print(error);
		return (int)error.code;
	}
}
