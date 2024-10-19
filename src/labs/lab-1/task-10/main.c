#include <limits.h>
#include <stdbool.h>
#include <stdio.h>

#include "lib/collections/string.h"
#include "lib/convert.h"
#include "lib/error.h"

#define NoNumbersException 0x10000001

const char* custom_error_to_string(error_code_t error) {
	switch (error) {
		case NoNumbersException:
			return "NoNumbersException";
		default:
			return NULL;
	}
}

error_t read_and_find_max(int base, long* out) {
	error_t status;

	long max = LONG_MIN;
	bool anyEntered = false;
	bool stopped = false;

	string_t input = string_create();
	if (!string_created(&input)) {
		string_destroy(&input);
		THROW(MemoryError, "can't create string buffer");
	}

	int ch;
	while ((ch = getchar()) != EOF) {
		// Skip whitespace characters. If any input is captured,
		// attempt to convert it to a number. If it's 'Stop', stop reading numbers.
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
			if (FAILED((status = long_from_base(buffer, length, base, &number)))) {
				string_destroy(&input);
				RETHROW(status, "can't parse input string");
			}

			max = number > max ? number : max;
			anyEntered = true;

			// Prepare to read the next number.
			string_destroy(&input);
			input = string_create();
			if (!string_created(&input)) {
				string_destroy(&input);
				THROW(MemoryError, "can't create new string buffer");
			}

			continue;
		}

		if (!string_append_char(&input, (char)ch)) {
			string_destroy(&input);
			THROW(MemoryError, "can't append char to string buffer");
		}
	}

	if (!stopped) {
		string_destroy(&input);
		THROW(AssertionError, "unexpected EOF")
	}
	if (!anyEntered) {
		string_destroy(&input);
		THROW(NoNumbersException, "no numbers were entered")
	}

	*out = max;
	return NO_EXCEPTION;
}

error_t print_in_base(long number, int base) {
	error_t error;

	char buffer[65];  // Big enough to hold a long number in base-2 (+null-term.)
	if (FAILED((error = long_to_base(number, base, buffer, sizeof(buffer))))) {
		PASS(error);
	}

	printf("%s", buffer);
	return NO_EXCEPTION;
}

error_t main_(void) {
	error_t status;
	printf("Enter a base (2-36): \n");

	char baseStr[33];
	scanf("%32s", baseStr);

	long base;
	if (FAILED((status = str_to_long(baseStr, &base)))) {
		fprintf(stderr, "Invalid base: malformed number\n");
		return NO_EXCEPTION;
	}
	if (base < 2 || base > 36) {
		fprintf(stderr, "Invalid base: must be 2 <= base <= 36\n");
		return NO_EXCEPTION;
	}

	printf("Input some numbers ('Stop' to continue):\n");

	long max;
	if (FAILED((status = read_and_find_max((int)base, &max)))) {
		PASS(status);
	}

	printf("Max number: %ld", max);

	for (int i = 9; i <= 36; i += 9) {
		printf("\n  in base %d: ", i);
		if (FAILED((status = print_in_base(max, i)))) {
			PASS(status);
		}
	}
	printf("\n");
	return NO_EXCEPTION;
}

int main(void) {
	error_t status;
	if (FAILED((status = main_()))) {
		error_fmt_t fmt[] = {&custom_error_to_string};
		error_print_ex(status, fmt, sizeof(fmt) / sizeof(fmt[0]));
		return (int)status.code;
	}
}
