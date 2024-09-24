#include <stdio.h>
#include <string.h>

#include "lib/conv.h"
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
		return ERROR_INVALID_PARAMETER;
	}

	++flag;

	for (int i = 0; i != nOpts; ++i) {
		if (strncmp(opts[i].name, flag, 16) == 0) {
			*outOpt = opts[i];
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
		return -ERROR_INVALID_PARAMETER;
	}

	opt_t opt;
	error = parse_opt(argv[1], opts, nOpts, &opt);
	if (error != ERROR_SUCCESS) {
		fprintf(stderr, "Invalid option. See usage for more details.\n");
		return -ERROR_INVALID_PARAMETER;
	}

	long x;
	error = str_to_long(argv[2], &x);
	if (error != ERROR_SUCCESS) {
		fprintf(stderr, "Invalid 'x': %s; malformed number or out of range\n", argv[2]);
		return -ERROR_INVALID_PARAMETER;
	}

	error = opt.handler(x);
	if (error != ERROR_SUCCESS) {
		error_print(error);
		return -(int)error;
	}
}
