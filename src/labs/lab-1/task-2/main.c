#include <float.h>
#include <stdio.h>

#include "lib/convert.h"
#include "lib/error.h"
#include "tasks.h"

typedef double (*compute_function_t)(double);

typedef struct compute_entry {
	char constant[16];
	char method[16];
	compute_function_t function;
} compute_entry_t;

int main(int argc, char** argv) {
	error_t error;

	if (argc != 2) {
		printf(
		    "Usage: %s <epsilon>\n"
		    "Computes various constants using different methods with accuracy of 'epsilon'.\n ",
		    argv[0]);
		return -ERROR_INVALID_PARAMETER;
	}

	double epsilon;
	error = str_to_double(argv[1], &epsilon);
	if (error != ERROR_SUCCESS || epsilon <= 0) {
		fprintf(stderr, "Invalid 'epsilon': malformed number or out of range.\n");
		return -ERROR_INVALID_PARAMETER;
	}

	compute_entry_t entries[] = {
	    {"exp", "limit", &compute_exp_lim},           {"exp", "series", &compute_exp_series},
	    {"exp", "equation", &compute_exp_eq},         {"pi", "limit", &compute_pi_lim},
	    {"pi", "series", &compute_pi_series},         {"pi", "equation", &compute_pi_eq},
	    {"ln_2", "limit", &compute_ln_2_lim},         {"ln_2", "series", &compute_ln_2_series},
	    {"ln_2", "equation", &compute_ln_2_eq},       {"sqrt_2", "limit", &compute_sqrt_2_lim},
	    {"sqrt_2", "series", &compute_sqrt_2_series}, {"sqrt_2", "equation", &compute_sqrt_2_eq},
	    {"gamma", "limit", &compute_gamma_lim},       {"gamma", "series", &compute_gamma_series},
	    {"gamma", "equation", &compute_gamma_eq},
	};

	int nEntries = sizeof(entries) / sizeof(compute_entry_t);
	for (int i = 0; i != nEntries; ++i) {
		compute_entry_t entry = entries[i];

		double value = entry.function(epsilon);
		if (value == DBL_MIN) {
			fprintf(stderr, "%8s | %10s | cannot be computed\n", entry.constant, entry.method);
		} else {
			fprintf(stdout, "%8s | %10s | %lf\n", entry.constant, entry.method, value);
		}
	}

	return 0;
}
