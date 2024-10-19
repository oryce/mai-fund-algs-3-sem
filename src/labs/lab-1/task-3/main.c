#include <stdio.h>
#include <string.h>

#include "cmd.h"
#include "lib/error.h"

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
	error_t error;

	opt_t opts[] = {{"q", "<eps> <a> <b> <c>", "solves a quadratic equation", &cmd_quad_eq},
	                {"m", "<a> <b>", "checks whether a is divisible by b", &cmd_divisible},
	                {"t", "<eps> <a> <b> <c>", "checks whether a triangle on points A, B, C is a right triangle",
	                 &cmd_right_triangle}};
	int nOpts = sizeof(opts) / sizeof(opt_t);

	if (argc == 1) {
		printf("Usage: %s <flag> <...>\nFlags:\n", argv[0]);
		print_opts(opts, nOpts);
		return NO_EXCEPTION;
	}

	opt_t opt;
	if (FAILED((error = parse_opt(argv[1], opts, nOpts, &opt)))) {
		fprintf(stderr, "Invalid arguments: %s.", error.message);
		return NO_EXCEPTION;
	}

	return opt.handler(argc, argv);
}

int main(int argc, char** argv) {
	error_t status;
	if (FAILED((status = main_(argc, argv)))) {
		error_print(status);
		return (int)status.code;
	}
}
