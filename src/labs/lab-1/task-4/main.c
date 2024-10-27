#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib/error.h"
#include "lib/io.h"
#include "tasks.h"

typedef error_t (*opt_handler_t)(FILE* in, FILE* out);

typedef struct opt {
	char name[16];
	char desc[256];
	opt_handler_t handler;
	bool specify_output;
} opt_t;

error_t parse_opt(const char* flag, opt_t opts[], int nOpts, opt_t* outOpt) {
	if (*flag != '-' && *flag != '/') return ERR_INVVAL;
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

			return 0;
		}
	}

	return ERR_UNRECOPT;
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
	if (!(*out)) return ERR_MEM;

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

	return 0;
}

void cleanup(FILE* input, FILE* output, char* outputPath) {
	fclose(input);
	fclose(output);
	free(outputPath);
}

error_t main_(int argc, char** argv) {
	opt_t opts[] = {
	    {"d", "removes digit characters from the input file",
	     &task_remove_digits},
	    {"i", "appends the letter count to each line", &task_count_letters},
	    {"s", "appends the special character count to each line",
	     &task_count_special_characters},
	    {"a", "encodes non-digit characters in hex ASCII",
	     &task_encode_non_digits},
	};
	int nOpts = sizeof(opts) / sizeof(opt_t);

	if (argc != 3 && argc != 4) {
		printf(
		    "Usage: %s <flag> <input file> [output file]\n"
		    "Append 'n' before the flag ('/nd') to specify the output file.\n"
		    "Flags:\n",
		    argv[0]);
		print_opts(opts, nOpts);
		return 0;
	}

	opt_t opt;

	error_t error = parse_opt(argv[1], opts, nOpts, &opt);
	if (error) return error;

	const char* inPath = argv[2];
	char* outPath = NULL;
	FILE* input = fopen(inPath, "r");
	FILE* output = NULL;

	if (input == NULL) {
		fprintf(stderr, "Can't open the input file.\n");
		return 0;
	}

	if (opt.specify_output) {
		if (argc != 4) {
			cleanup(input, output, outPath);
			fprintf(stderr, "Specify the output file.\n");
			return 0;
		}

		bool samePaths;
		if (paths_same(argv[2], argv[3], &samePaths) || samePaths) {
			cleanup(input, output, outPath);
			fprintf(stderr,
			        "Path is malformed or is the same as the input path.\n");
			return 0;
		}

		output = fopen(argv[3], "w");
		if (output == NULL) {
			cleanup(input, output, outPath);
			fprintf(stderr, "Can't open the output file.\n");
			return 0;
		}
	} else {
		if ((error = add_prefix(inPath, &outPath))) {
			cleanup(input, output, outPath);
			return error;
		}

		output = fopen(outPath, "w");
		if (output == NULL) {
			cleanup(input, output, outPath);
			fprintf(stderr, "Can't open the output file.\n");
			return 0;
		}
	}

	error = opt.handler(input, output);
	cleanup(input, output, outPath);

	return error;
}

int main(int argc, char** argv) {
	error_t error = main_(argc, argv);
	if (error) {
		error_print(error);
		return error;
	}
}
