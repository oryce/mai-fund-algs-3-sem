#include <stdio.h>

#include "error.h"
#include "tasks.h"
#include "conv.h"

int main(int argc, char** argv) {
	error_t error;

	if (argc != 3) {
		printf(
		    "Usage: %s <flag> <x>\n"
		    "Flags:\n"
		    "  - /h, -h: prints the first 100 numbers divisible by 'x'\n"
		    "  - /p, -p: determines if 'x' is a prime number\n"
		    "  - /s, -s: converts 'x' to hex\n"
		    "  - /e, -e: raises numbers from lab-task-1 to 10 to powers from 1 to 'x' < 10\n"
		    "  - /a, -a: computes the sum of numbers from 1 to 'x'\n"
		    "  - /f, -f: computes the factorial of 'x'\n",
		    argv[0]);
		return -ERROR_INVALID_PARAMETER;
	}

	char* flag = argv[1];
	if (flag[0] != '-' && flag[0] != '/') {
		fprintf(stderr, "Invalid flag: %s\n", flag);
		return -ERROR_INVALID_PARAMETER;
	}

	long x;
	error = str_to_long(argv[2], &x);
	if (error != ERROR_SUCCESS) {
		fprintf(stderr, "Invalid 'x': %s; malformed number or out of range\n", argv[2]);
		return -ERROR_INVALID_PARAMETER;
	}

	switch (flag[1]) {
		case 'h': {
			error = print_divisible(x);
			break;
		}
		case 'p': {
			error = print_is_prime(x);
			break;
		}
		case 's': {
			error = print_hex(x);
			break;
		}
		case 'e': {
			error = print_powers(x);
			break;
		}
		case 'a': {
			error = print_sums(x);
			break;
		}
		case 'f': {
			error = print_factorial(x);
			break;
		}
		default:
			fprintf(stderr, "Invalid flag: %s\n", flag);
			return -ERROR_INVALID_PARAMETER;
	}

	if (error != ERROR_SUCCESS) {
		error_print(error);
		return -(int)error;
	}
}
