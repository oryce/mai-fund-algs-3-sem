#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "lib/collections/string.h"
#include "lib/convert.h"
#include "lib/error.h"
#include "lib/mth.h"

#define ERROR_NO_NUMBERS_ENTERED (-1L)

error_t finalize_input(string_t* inNumber, int base, long* outNumber) {
	long result;
	error_t error = long_from_base(string_to_c_str(inNumber), string_length(inNumber), base, &result);

	if (error) {
		fprintf(stderr, "Malformed number: %s\n", string_to_c_str(inNumber));
		return error;
	}

	string_destroy(inNumber);
	*outNumber = result;

	return ERROR_SUCCESS;
}

error_t read_numbers_and_find_max(int base, long* out) {
	error_t error = ERROR_SUCCESS;
	long max = LONG_MIN;
	bool anyEntered = false;

	string_t input = string_create();
	if (!input.initialized) {
		error = ERROR_HEAP_ALLOCATION;
		goto cleanup;
	}

	int ch;
	bool stopped = false;

	while ((ch = getchar()) != EOF) {
		// Skip whitespace characters. If any input is captured, attempt to convert it to a number.
		// If it's 'Stop', stop reading numbers.
		if (ch == '\n' || ch == '\t' || ch == ' ') {
			bool inBuffer = string_length(&input) > 0;

			if (inBuffer) {
				stopped = strcmp(string_to_c_str(&input), "Stop") == 0;
				if (stopped) {
					string_destroy(&input);
					break;
				}

				long number;
				error = finalize_input(&input, base, &number);
				if (error) goto cleanup;

				max = mth_long_max(number, max);
				anyEntered = true;

				input = string_create();
				if (!input.initialized) {
					error = ERROR_HEAP_ALLOCATION;
					goto cleanup;
				}
			}

			continue;
		}

		if (!string_append_char(&input, (char)ch)) {
			error = ERROR_HEAP_ALLOCATION;
			goto cleanup;
		}
	}

	// Finalize numbers in the input (if the loop exited with an EOF).
	if (!stopped) {
		long number;
		error = finalize_input(&input, base, &number);
		if (error) goto cleanup;

		max = mth_long_max(number, max);
		anyEntered = true;
	}

	if (!anyEntered) {
		error = ERROR_NO_NUMBERS_ENTERED;
		goto cleanup;
	}

	*out = max;

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

int main(void) {
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
	long max;
	error = read_numbers_and_find_max((int)base, &max);

	if (error == ERROR_NO_NUMBERS_ENTERED) {
		printf("Silly, you didn't input any\n");
	} else if (error != ERROR_SUCCESS) {
		error_print(error);
		return (int)-error;
	} else {
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
}
