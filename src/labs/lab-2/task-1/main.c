#include <stdio.h>
#include <string.h>

#include "cmd.h"

typedef error_t (*opt_handler_t)(int argc, char** argv);

typedef struct opt {
	char name[16];
	char args[64];
	char desc[256];
	opt_handler_t handler;
} opt_t;

error_t parse_opt(const char* flag, opt_t opts[], int nOpts, opt_t* outOpt) {
	if (flag[0] != '-' && flag[0] != '/') {
		THROW(IllegalArgumentException, "flag starts with an invalid character");
	}

	++flag;

	for (int i = 0; i != nOpts; ++i) {
		if (strncmp(opts[i].name, flag, 16) == 0) {
			*outOpt = opts[i];
			return NO_EXCEPTION;
		}
	}

	THROWF(IllegalArgumentException, "unrecognized option: %c", flag[0]);
}

void print_opts(opt_t opts[], int nOpts) {
	for (int i = 0; i != nOpts; ++i) {
		opt_t opt = opts[i];
		fprintf(stdout, "  /%s, -%s %s: %s\n", opt.name, opt.name, opt.args, opt.desc);
	}
}

error_t main_(int argc, char** argv) {
	error_t status;

	opt_t opts[] = {{"l", "<string>", "counts the string's length", &cmd_print_length},
	                {"r", "<string>", "reverses the string", &cmd_reverse_string},
	                {"u", "<string>", "uppercases chars on odd indices in the string", &cmd_uppercase_odd_chars},
	                {"n", "<string>", "rearranges characters in the string", &cmd_rearrange_chars},
	                {"c", "<seed> <strings...>", "concatenates strings randomly", &cmd_concat_randomly}};
	int nOpts = sizeof(opts) / sizeof(opt_t);

	if (argc == 1) {
		printf("Usage: %s <flag> <options...>\nFlags:\n", argv[0]);
		print_opts(opts, nOpts);
		return NO_EXCEPTION;
	}

	opt_t opt;
	if (FAILED((status = parse_opt(argv[1], opts, nOpts, &opt)))) {
		fprintf(stderr, "Invalid arguments: %s.", status.message);
		return NO_EXCEPTION;
	}
	if (FAILED((status = opt.handler(argc, argv)))) {
		PASS(status);
	}

	return NO_EXCEPTION;
}

int main(int argc, char** argv) {
	error_t status = main_(argc, argv);
	if (FAILED(status)) {
		error_print(status);
		return (int)status.code;
	}
}
