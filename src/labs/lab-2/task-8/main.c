#include <stdio.h>

#include "task.h"

void cleanup_(ubigint_t* sum, string_t* string) {
	ubi_destroy(sum);
	string_destroy(string);
}

int main(void) {
	ubigint_t sum;
	string_t string;

	if (!task_add_bigints(&sum, 16, 2, "aaa", "fffa0000")) {
		cleanup_(&sum, &string);
		return 1;
	}
	if (!ubi_to_string(&sum, &string)) {
		cleanup_(&sum, &string);
		return 1;
	}

	printf("%s\n", string_to_c_str(&string));
	cleanup_(&sum, &string);
}
