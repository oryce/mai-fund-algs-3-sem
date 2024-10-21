#include "tasks.h"

#include <stdarg.h>
#include <string.h>

#include "lib/chars.h"
#include "lib/convert.h"

typedef enum sink_source { SINK_FILE, SINK_STRING } sink_source_t;

typedef struct sink {
	sink_source_t type;
	union {
		FILE* file;
		char* string;
	} source;
} sink_t;

typedef error_t (*printf_specifier_t)(sink_t* sink, va_list* args);

error_t sink_put_string_(sink_t* sink, const char* string) {
	switch (sink->type) {
		case SINK_FILE:
			fputs(string, sink->source.file);
			if (ferror(sink->source.file)) {
				THROW(IOException, "can't write to file sink");
			}
			break;
		case SINK_STRING:
			strcat(sink->source.string, string);
			break;
	}

	return NO_EXCEPTION;
}

error_t sink_put_char_(sink_t* sink, char ch) {
	switch (sink->type) {
		case SINK_FILE:
			fputc(ch, sink->source.file);
			if (ferror(sink->source.file)) {
				THROW(IOException, "can't write to file sink");
			}
			break;
		case SINK_STRING:
			strcat(sink->source.string, (char[]){ch, '\0'});
			break;
	}

	return NO_EXCEPTION;
}

error_t sink_vprintf_(sink_t* sink, const char* fmt, va_list args) {
	switch (sink->type) {
		case SINK_FILE:
			vfprintf(sink->source.file, fmt, args);
			if (ferror(sink->source.file)) {
				THROW(IOException, "can't write to file sink");
			}
			break;
		case SINK_STRING:
			vsprintf(sink->source.string, fmt, args);
			break;
	}

	return NO_EXCEPTION;
}

error_t sink_printf_(sink_t* sink, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);

	error_t status = sink_vprintf_(sink, fmt, args);
	va_end(args);

	return status;
}

error_t roman_specifier_(sink_t* sink, va_list* args) {
	int number = va_arg(*args, int);

	const char* letters[] = {"M", "CM", "D", "CD", "C", "XC", "L", "XL", "X", "IX", "V", "IV", "I"};
	const int values[] = {1000, 900, 500, 400, 100, 90, 50, 40, 10, 9, 5, 4, 1};

	char roman[1024] = {'\0'};

	for (size_t i = 0; number != 0; ++i) {
		while (number >= values[i]) {
			number -= values[i];
			size_t nWritten = strlcat(roman, letters[i], sizeof(roman));
			if (nWritten == sizeof(roman) - 1 && number != 0) {
				THROW(IllegalArgumentException, "number is too large");
			}
		}
	}

	error_t status = sink_put_string_(sink, roman);
	if (FAILED(status)) PASS(status);

	return NO_EXCEPTION;
}

const unsigned int FIB[] = {1,         2,         3,         5,          8,          13,        21,        34,
                            55,        89,        144,       233,        377,        610,       987,       1597,
                            2584,      4181,      6765,      10946,      17711,      28657,     46368,     75025,
                            121393,    196418,    317811,    514229,     832040,     1346269,   2178309,   3524578,
                            5702887,   9227465,   14930352,  24157817,   39088169,   63245986,  102334155, 165580141,
                            267914296, 433494437, 701408733, 1134903170, 1836311903, 2971215073};

error_t zeckendorf_specifier_(sink_t* sink, va_list* args) {
	error_t status;

	unsigned int number = va_arg(*args, unsigned int);
	uint64_t zeckendorf = 0;
	const int nFibs = sizeof(FIB) / sizeof(FIB[0]);

	for (size_t i = nFibs - 1; i--;) {
		if (FIB[i] <= number) {
			zeckendorf |= (1ULL << i);
			number -= FIB[i];
		}
	}

	while (zeckendorf) {
		status = sink_put_char_(sink, zeckendorf & 1 ? '1' : '0');
		if (FAILED(status)) PASS(status);

		zeckendorf >>= 1;
	}

	status = sink_put_char_(sink, '1');
	if (FAILED(status)) PASS(status);

	return NO_EXCEPTION;
}

error_t to_base_specifier_(sink_t* sink, va_list* args, bool uppercase) {
	error_t status;

	int n = va_arg(*args, int);
	int base = va_arg(*args, int);
	if (base < 2 || base > 36) base = 10;

	char nInBase[65];
	if (FAILED((status = long_to_base((long)n, base, nInBase, sizeof(nInBase))))) {
		PASS(status);
	}

	if (uppercase) {
		for (size_t i = 0; nInBase[i] != '\0'; ++i) {
			status = sink_put_char_(sink, chars_upper(nInBase[i]));
			if (FAILED(status)) PASS(status);
		}
	} else {
		status = sink_put_string_(sink, nInBase);
		if (FAILED(status)) PASS(status);
	}
	return NO_EXCEPTION;
}

error_t to_base_lowercase_specifier_(sink_t* sink, va_list* args) { return to_base_specifier_(sink, args, false); }

error_t to_base_uppercase_specifier_(sink_t* sink, va_list* args) { return to_base_specifier_(sink, args, true); }

error_t from_base_specifier_(sink_t* sink, va_list* args, bool uppercase) {
	error_t status;

	const char* number = va_arg(*args, const char*);
	int base = va_arg(*args, int);
	if (base < 2 || base > 36) base = 10;

	size_t length = 0;
	if (uppercase) {
		// Validate that all the letters are in uppercase and also count the length.
		// (not required by long_from_base, but required by the task)
		for (size_t i = 0; number[i] != '\0'; ++i) {
			if (number[i] >= 'a' && number[i] <= 'z') {
				THROW(IllegalArgumentException, "unexpected lowercase letter")
			}
			++length;
		}
	} else {
		length = strlen(number);
	}

	long base10;
	if (FAILED((status = long_from_base(number, length, base, &base10)))) {
		PASS(status);
	}

	status = sink_printf_(sink, "%ld", base10);
	if (FAILED(status)) PASS(status);

	return NO_EXCEPTION;
}

error_t from_base_lowercase_specifier_(sink_t* sink, va_list* args) { return from_base_specifier_(sink, args, false); }

error_t from_base_uppercase_specifier_(sink_t* sink, va_list* args) { return from_base_specifier_(sink, args, true); }

error_t repr_32_(sink_t* sink, uint32_t value) {
	const int size = sizeof(value) * 8;
	char repr[size + 1];

	for (size_t i = 0; i != size; ++i) {
		repr[size - i - 1] = value & 1 ? '1' : '0';
		value >>= 1;
	}

	repr[size] = '\0';

	error_t status = sink_put_string_(sink, repr);
	if (FAILED(status)) PASS(status);

	return NO_EXCEPTION;
}

error_t repr_64_(sink_t* sink, uint64_t value) {
	const int size = sizeof(value) * 8;
	char repr[size + 1];

	for (size_t i = 0; i != size; ++i) {
		repr[size - i - 1] = value & 1 ? '1' : '0';
		value >>= 1;
	}

	repr[size] = '\0';

	error_t status = sink_put_string_(sink, repr);
	if (FAILED(status)) PASS(status);

	return NO_EXCEPTION;
}

error_t memory_int_specifier_(sink_t* sink, va_list* args) {
	int32_t number = va_arg(*args, int32_t);
	return repr_32_(sink, *(uint32_t*)(&number));
}

error_t memory_uint_specifier(sink_t* sink, va_list* args) {
	uint32_t number = va_arg(*args, uint32_t);
	return repr_32_(sink, number);
}

error_t memory_double_specifier(sink_t* sink, va_list* args) {
	double number = va_arg(*args, double);
	return repr_64_(sink, *(uint64_t*)(&number));
}

error_t memory_float_specifier(sink_t* sink, va_list* args) {
	float number = va_arg(*args, double);  // Use `double` because of type promotion
	return repr_32_(sink, *(uint32_t*)(&number));
}

printf_specifier_t get_custom_specifier_(const char* spec) {
	if (strcmp(spec, "%Ro") == 0) return &roman_specifier_;
	if (strcmp(spec, "%Zr") == 0) return &zeckendorf_specifier_;
	if (strcmp(spec, "%Cv") == 0) return &to_base_lowercase_specifier_;
	if (strcmp(spec, "%CV") == 0) return &to_base_uppercase_specifier_;
	if (strcmp(spec, "%to") == 0) return &from_base_lowercase_specifier_;
	if (strcmp(spec, "%TO") == 0) return &from_base_uppercase_specifier_;
	if (strcmp(spec, "%mi") == 0) return &memory_int_specifier_;
	if (strcmp(spec, "%mu") == 0) return &memory_uint_specifier;
	if (strcmp(spec, "%md") == 0) return &memory_double_specifier;
	if (strcmp(spec, "%mf") == 0) return &memory_float_specifier;

	return NULL;
}

bool is_builtin_specifier_(char last) {
	// https://man7.org/linux/man-pages/man3/printf.3.html
	// Compilers often optimise this switch into a few instructions.
	switch (last) {
		case 'd':
		case 'i':
		case 'o':
		case 'u':
		case 'x':
		case 'X':
		case 'e':
		case 'E':
		case 'f':
		case 'F':
		case 'g':
		case 'G':
		case 'a':
		case 'A':
		case 'c':
		case 's':
		case 'p':
		case 'n':
			return true;
		default:
			return false;
	}
}

error_t sink_overfprintf_(sink_t* sink, const char* fmt, va_list args) {
	if (sink == NULL) THROW(IllegalArgumentException, "`sink` may not be null");
	if (fmt == NULL) THROW(IllegalArgumentException, "`fmt` may not be null");

	error_t status;

	char spec[16];  // Should be enough for printf() specifiers.
	                // Our specifiers are 2 chars long each.
	ssize_t specCursor = 0;

	while (*fmt != '\0') {
		if (specCursor) {
			if (*fmt == '%') {  // Escaped spec
				if (FAILED((status = sink_put_char_(sink, '%')))) {
					PASS(status);
				}
				specCursor = 0;
			} else {
				const size_t maxLength = sizeof(spec) / sizeof(spec[0]) - 1;

				if (specCursor == maxLength - 1) {  // Format spec is too long.
					THROW(IllegalArgumentException, "invalid format spec");
				}

				spec[specCursor++] = *fmt;
				spec[specCursor] = '\0';

				// Try to resolve a custom spec. If it fails, check if it's a
				// printf spec and delegate there.
				printf_specifier_t customSpecifier = get_custom_specifier_(spec);

				if (customSpecifier) {
					if (FAILED((status = customSpecifier(sink, &args)))) {
						RETHROW(status, "can't execute format spec");
					}
					specCursor = 0;
				} else if (is_builtin_specifier_(*fmt)) {
					if (FAILED((status = sink_vprintf_(sink, spec, args)))) {
						PASS(status);
					}
					va_arg(args, void*);  // Advance `args` because we provide an argument to vprintf
					specCursor = 0;
				}
			}
		} else if (*fmt == '%') {
			// Start reading the format specifier.
			spec[specCursor++] = '%';
		} else {
			if (FAILED((status = sink_put_char_(sink, *fmt)))) {
				PASS(status);
			}
		}

		++fmt;
	}

	return NO_EXCEPTION;
}

error_t overfprintf(FILE* output, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);

	sink_t sink = {.type = SINK_FILE, .source = {.file = output}};
	error_t status = sink_overfprintf_(&sink, fmt, args);

	va_end(args);
	if (FAILED(status)) PASS(status);

	return NO_EXCEPTION;
}

error_t oversprintf(char* output, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);

	// Null-terminate `output`. Required for `strcat`.
	output[0] = '\0';

	sink_t sink = {.type = SINK_STRING, .source = {.string = output}};
	error_t status = sink_overfprintf_(&sink, fmt, args);

	va_end(args);
	if (FAILED(status)) PASS(status);

	return NO_EXCEPTION;
}
