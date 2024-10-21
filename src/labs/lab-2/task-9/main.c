#include <stdio.h>

#include "task.h"

void print_results_(const bool* results, size_t n) {
	for (size_t i = 0; i != n; ++i) {
		printf("[%4zu]: %s\n", i, results[i] ? "yes" : "no");
	}
}

int main(void) {
	bool results[1024];

	if (finite_representations(results, 16, 5, 1 / 2., 1 / 4., 35 / 64., 35 / 69., 59 / 512.)) {
		print_results_(results, 5);
	}
}
