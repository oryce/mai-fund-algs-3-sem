#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "lib/conv.h"
#include "lib/error.h"
#include "lib/string.h"
#include "lib/vector.h"

error_t finalize_number(string_t* input, int base, vector_i64_t* numbers) {
	long numberBase10;

	error_t error = num_to_base_10(string_to_c_str(input), string_length(input), base, &numberBase10);
	if (error) {
		fprintf(stderr, "Malformed number: %s\n", string_to_c_str(input));
		return error;
	}

	string_destroy(input);

	if (!vector_i64_push_back(numbers, numberBase10)) {
		return ERROR_HEAP_ALLOCATION;
	}

	return ERROR_SUCCESS;
}

error_t read_numbers(vector_i64_t* numbers, int base) {
	error_t error = ERROR_SUCCESS;
	string_t input = string_create();

	int ch;
	bool stopped = false;

	while ((ch = getchar()) != EOF) {
		if (ch == '\n' || ch == '\t' || ch == ' ') {
			bool inBuffer = string_length(&input) > 0;

			if (inBuffer) {
				if (strcmp(string_to_c_str(&input), "Stop") == 0) {
					string_destroy(&input);
					stopped = true;

					break;
				}

				error = finalize_number(&input, (int)base, numbers);
				if (error) goto cleanup;

				input = string_create();
			}

			continue;
		}

		if (!string_append_char(&input, (char)ch)) {
			error = ERROR_HEAP_ALLOCATION;
			goto cleanup;
		}
	}

	// Check if the string has been destroyed already
	if (!stopped) {
		error = finalize_number(&input, (int)base, numbers);
		if (error) goto cleanup;
	}

cleanup:
	string_destroy(&input);
	return error;
}

void print_in_base(long number, int base) {
	char buffer[65];  // Big enough to hold a long number in base-2 (+null-term.)
	error_t error = long_to_base(number, base, buffer, 65);

	if (error) {
		fprintf(stderr, "Catastrophic failure");
	} else {
		printf("%s", buffer);
	}
}

int main(int argc, char** argv) {
	printf("Enter a base (2-36): \n");

	char baseStr[33];
	scanf("%32s", baseStr);

	long base;
	error_t error = str_to_long(baseStr, &base);
	if (error || base < 2 || base > 36) {
		fprintf(stderr, "Invalid base.\n");
		return -ERROR_INVALID_PARAMETER;
	}

	printf("Input some numbers ('Stop' to continue):\n");

	vector_i64_t numbers = vector_i64_create();

	error = read_numbers(&numbers, (int)base);
	if (error) goto cleanup;

	if (vector_i64_is_empty(&numbers)) {
		printf("Silly, you didn't input any\n");
	} else {
		long max = -1;

		for (int i = 0; i != vector_i64_size(&numbers); ++i) {
			long n = vector_i64_get(&numbers, i);
			if (max == -1 || n > max) max = n;
		}

		printf("Max number: %ld\n", max);
		printf("  in base 9: ");
		print_in_base(max, 9);
		printf("\n  in base 18: ");
		print_in_base(max, 18);
		printf("\n  in base 27: ");
		print_in_base(max, 27);
		printf("\n  in base 36: ");
		print_in_base(max, 36);
		printf("\n");
	}

cleanup:
	vector_i64_destroy(&numbers);

	if (error) {
		error_print(error);
		return (int)-error;
	}
}
