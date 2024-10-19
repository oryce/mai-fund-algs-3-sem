#include <stdio.h>
#include <string.h>

#include "lib/convert.h"
#include "lib/error.h"
#include "tasks.h"

typedef error_t (*opt_handler_t)(long x);

typedef struct opt {
	char name[16];
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
		fprintf(stdout, "  /%s, -%s: %s\n", opt.name, opt.name, opt.desc);
	}
}

error_t main_(int argc, char** argv) {
	error_t error;

	opt_t opts[] = {{"h", "prints the first 100 numbers divisible by 'x'", &print_divisible},
	                {"p", "determines if 'x' is a prime number", &print_is_prime},
	                {"s", "converts 'x' to hex", &print_hex},
	                {"e", "raises numbers from 1 to 10 to powers from 1 to 'x' < 10", &print_powers},
	                {"a", "computes the sum of numbers from 1 to 'x'", &print_sums},
	                {"f", "computes the factorial of 'x'", &print_factorial}};
	int nOpts = sizeof(opts) / sizeof(opt_t);

	if (argc != 3) {
		printf("Usage: %s <flag> <x>\nFlags:\n", argv[0]);
		print_opts(opts, nOpts);
		return NO_EXCEPTION;
	}

	opt_t opt;
	if (FAILED((error = parse_opt(argv[1], opts, nOpts, &opt)))) {
		fprintf(stderr, "Invalid arguments: %s.", error.message);
		return NO_EXCEPTION;
	}

	long x;
	if (FAILED((error = str_to_long(argv[2], &x)))) {
		fprintf(stderr, "Invalid 'x': %s.\n", error.message);
		return NO_EXCEPTION;
	}

	return opt.handler(x);
}

int main(int argc, char** argv) {
	error_t error;
	if (FAILED((error = main_(argc, argv)))) {
		error_print(error);
		return (int)error.code;
	}
}
