#include <stdbool.h>
#include <stdio.h>

#include "task.h"

error_t main_(int argc, char** argv) {
	if (argc != 4) {
		fprintf(stderr, "Usage: %s <input file> <-a,/a|-d,/d> <output file>\n",
		        argv[0]);
		return 0;
	}

	bool ascending;
	const char* flag = argv[2];

	if (*flag != '-' && *flag != '/') {
		fprintf(stderr, "Malformed flag.\n");
		return 0;
	}

	switch (flag[1]) {
		case 'a':
			ascending = true;
			break;
		case 'd':
			ascending = false;
			break;
		default:
			fprintf(stderr, "Malformed flag.\n");
			return 0;
	}

	FILE* input = fopen(argv[1], "r");
	if (!input) {
		fprintf(stderr, "Can't open input file for reading.\n");
		return 0;
	}

	FILE* output = fopen(argv[3], "w");
	if (!output) {
		fprintf(stderr, "Can't open output file for writing.\n");
		return 0;
	}

	vector_employee_t employees;

	error_t error = read_employees(&employees, input) ||
	                sort_employees(&employees, ascending) ||
	                write_employees(&employees, output);
	destroy_employees(&employees);

	return error;
}

int main(int argc, char** argv) {
	error_t error = main_(argc, argv);
	if (error) {
		error_print(error);
		return error;
	}
}
