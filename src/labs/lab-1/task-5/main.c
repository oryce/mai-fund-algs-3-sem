#include <stdio.h>

#include "lib/convert.h"
#include "lib/error.h"
#include "lib/mth.h"
#include "tasks.h"

typedef error_t (*integral_func_t)(double eps, double x, double* out);

typedef struct series {
	char name[8];
	integral_func_t func;
} series_t;

error_t main_(int argc, char** argv) {
	if (argc != 3) {
		printf(
		    "Usage: %s <eps> <x>\n"
		    "Computes various series values given an error margin.\n",
		    argv[0]);
		return 0;
	}

	double eps;
	if (str_to_double(argv[1], &eps) || eps <= 0) {
		fprintf(stderr, "Invalid 'eps': malformed number or out of range.\n");
		return 0;
	}

	double x;
	if (str_to_double(argv[2], &x)) {
		fprintf(stderr, "Invalid 'x': malformed number or out of range.\n");
		return 0;
	}

	series_t series[] = {
	    {"a)", &compute_series_a},
	    {"b)", &compute_series_b},
	    {"c)", &compute_series_c},
	    {"d)", &compute_series_d},
	};
	int nSeries = sizeof(series) / sizeof(series_t);

	for (int i = 0; i != nSeries; ++i) {
		double value;
		error_t error = series[i].func(eps, x, &value);

		if (!error)
			printf("%8s | cannot be computed\n", series[i].name);
		else if (error == ERR_M_DIVERGING)
			printf("%8s | %f\n", series[i].name, value);
		else
			return error;
	}

	return 0;
}

int main(int argc, char** argv) {
	error_t error = main_(argc, argv);

	if (error) {
		error_fmt_t fmt[] = {&mth_error_to_string};
		error_print_ex(error, fmt, sizeof(fmt) / sizeof(fmt[0]));

		return error;
	}
}
