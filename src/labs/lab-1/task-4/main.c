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
		return ERROR_INVALID_PARAMETER;
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

			return ERROR_SUCCESS;
		}
	}

	return ERROR_INVALID_PARAMETER;
}

void print_opts(opt_t opts[], int nOpts) {
	for (int i = 0; i != nOpts; ++i) {
		opt_t opt = opts[i];
		fprintf(stdout, "  /%s, -%s: %s\n", opt.name, opt.name, opt.desc);
	}
}

int main(int argc, char** argv) {
	error_t error = ERROR_SUCCESS;

	opt_t opts[] = {
	    {"d", "removes digit characters from the input file", &remove_digits},
	    {"i", "appends the letter count to each line", &count_letters},
	    {"s", "appends the special character count to each line", &count_special_characters},
	    {"a", "encodes non-digit characters in hex ASCII", &encode_non_digits},
	};
	int nOpts = sizeof(opts) / sizeof(opt_t);

	if (argc != 3 && argc != 4) {
		printf(
		    "Usage: %s <flag> <input file> [output file]\n"
		    "Append 'n' before the flag ('/nd') to specify the output file.\n"
		    "Flags:\n",
		    argv[0]);
		print_opts(opts, nOpts);
		return -ERROR_INVALID_PARAMETER;
	}

	opt_t opt;
	error = parse_opt(argv[1], opts, nOpts, &opt);
	if (error != ERROR_SUCCESS) {
		fprintf(stderr, "Invalid option. See usage for more details.\n");
		return -ERROR_INVALID_PARAMETER;
	}

	const char* inPath = argv[2];
	FILE* inFile = fopen(inPath, "r");
	if (inFile == NULL) {
		fprintf(stderr, "Error: Cannot open the input file.\n");
		return -ERROR_IO;
	}

	FILE* outFile = NULL;
	char* outPath = NULL;
	if (opt.specify_output) {
		if (argc != 4) {
			fprintf(stderr, "Error: Specify the output file.\n");

			error = ERROR_INVALID_PARAMETER;
			goto cleanup;
		}

		outFile = fopen(argv[3], "w");

		bool samePaths;

		error = paths_same(argv[2], argv[3], &samePaths);
		if (error) goto cleanup;

		if (samePaths) {
			fprintf(stderr, "Error: Output path must be different from the input path.\n");

			error = ERROR_INVALID_PARAMETER;
			goto cleanup;
		}

		if (outFile == NULL) {
			fprintf(stderr, "Error: Cannot open the output file.\n");

			error = ERROR_IO;
			goto cleanup;
		}
	} else {
		const char* prefix = "out_";
		outPath = (char*)malloc(strlen(inPath) + strlen(prefix) + 1);

		if (outPath == NULL) {
			fprintf(stderr, "Error: Cannot allocate for the output file name.\n");

			error = ERROR_HEAP_ALLOCATION;
			goto cleanup;
		}

		char delimiter;
#ifdef _WIN32
		delimiter = '\\';
#else
		delimiter = '/';
#endif
		const char* lastSep = strrchr(inPath, delimiter);
		if (lastSep == NULL) {
			strcat(outPath, prefix);
			strcat(outPath, inPath);
		} else {
			// Copy everything before the filename, including the separator
			strncat(outPath, inPath, lastSep - inPath + 1);
			strcat(outPath, prefix);
			// Append the filename
			strcat(outPath, lastSep + 1);
		}

		outFile = fopen(outPath, "w");
		if (outFile == NULL) {
			fprintf(stderr, "Error: Cannot open output file.\n");

			error = ERROR_IO;
			goto cleanup;
		}
	}

	error = opt.handler(inFile, outFile);
	if (error != ERROR_SUCCESS) {
		error_print(error);
	}

cleanup:
	fclose(inFile);
	fclose(outFile);
	free(outPath);

	return (int)(-error);
}
