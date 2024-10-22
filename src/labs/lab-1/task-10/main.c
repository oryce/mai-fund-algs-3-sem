#include <limits.h>
#include <stdbool.h>
#include <stdio.h>

#include "lib/collections/string.h"
#include "lib/convert.h"
#include "lib/error.h"

#define ERR_NONUM 0x10000001

static const char* custom_error_to_string(error_t error) {
	return error == ERR_NONUM ? "No numbers were entered" : NULL;
}

error_t read_and_find_max(int base, long* out) {
	error_t status;

	long max = LONG_MIN;
	bool anyEntered = false;
	bool stopped = false;

	string_t input = string_create();
	if (!string_created(&input)) {
		string_destroy(&input);
		return ERR_MEM;
	}

	int ch;
	while ((ch = getchar()) != EOF) {
		// Skip whitespace characters. If any input is captured, attempt to
		// convert it to a number. If it's 'Stop', stop reading numbers.
		if (ch == '\n' || ch == '\t' || ch == ' ') {
			bool inBuffer = string_length(&input) > 0;
			if (!inBuffer) continue;

			const char* buffer = string_to_c_str(&input);
			size_t length = string_length(&input);

			// Exit condition.
			stopped = strcmp(buffer, "Stop") == 0;
			if (stopped) {
				string_destroy(&input);
				break;
			}

			long number;

			status = long_from_base(buffer, length, base, &number);
			if (status) {
				string_destroy(&input);
				return status;
			}

			max = number > max ? number : max;
			anyEntered = true;

			// Prepare to read the next number.
			string_destroy(&input);
			input = string_create();

			if (!string_created(&input)) {
				string_destroy(&input);
				return ERR_MEM;
			}

			continue;
		}

		if (!string_append_char(&input, (char)ch)) {
			string_destroy(&input);
			return ERR_MEM;
		}
	}

	if (!stopped) {
		string_destroy(&input);
		return ERR_CHECK;
	}
	if (!anyEntered) {
		string_destroy(&input);
		return ERR_NONUM;
	}

	*out = max;
	return 0;
}

error_t print_in_base(long number, int base) {
	char buffer[65];  // Big enough to hold a long number in base-2.

	error_t error = long_to_base(number, base, buffer, sizeof(buffer));
	if (error) return error;

	printf("%s", buffer);
	return 0;
}

error_t main_(void) {
	printf("Enter a base (2-36): \n");

	char baseStr[33];
	scanf("%32s", baseStr);

	long base;
	if (str_to_long(baseStr, &base)) {
		fprintf(stderr, "Invalid base: malformed number\n");
		return 0;
	}
	if (base < 2 || base > 36) {
		fprintf(stderr, "Invalid base: must be 2 <= base <= 36\n");
		return 0;
	}

	printf("Input some numbers ('Stop' to continue):\n");

	long max;

	error_t error = read_and_find_max((int)base, &max);
	if (error) return error;

	printf("Max number: %ld", max);

	for (int i = 9; i <= 36; i += 9) {
		printf("\n  in base %d: ", i);
		if ((error = print_in_base(max, i))) return error;
	}

	printf("\n");
	return 0;
}

int main(void) {
	error_t error = main_();

	if (error) {
		error_fmt_t fmt[] = {&custom_error_to_string};
		error_print_ex(error, fmt, sizeof(fmt) / sizeof(fmt[0]));

		return error;
	}
}
