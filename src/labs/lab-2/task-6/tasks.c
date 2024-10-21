#include "tasks.h"

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

#include "lib/convert.h"

#define SEOF (-1)

typedef enum source_type { SOURCE_FILE, SOURCE_STRING } source_type_t;

typedef struct source {
	source_type_t type;
	union {
		FILE* file;
		char* string;
	} source;
} source_t;

typedef int (*scanf_specifier_t)(source_t* src, va_list* args, size_t* nRead);

int source_get_char_(source_t* src) {
	switch (src->type) {
		case SOURCE_FILE: {
			int ch = fgetc(src->source.file);
			return ch == EOF ? SEOF : ch;
		}
		case SOURCE_STRING: {
			return *src->source.string == '\0' ? SEOF : *(src->source.string++);
		}
	}
}

int source_scanf_(source_t* src, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);

	int assigned = 0;

	switch (src->type) {
		case SOURCE_FILE:
			assigned = vfscanf(src->source.file, fmt, args);
			break;
		case SOURCE_STRING:
			// NB: Caller should track the amount of chars read and
			//     advance the stream.
			assigned = vsscanf(src->source.string, fmt, args);
			break;
	}

	va_end(args);
	return assigned;
}

void source_rewind_(source_t* src, int ch) {
	switch (src->type) {
		case SOURCE_FILE:
			ungetc(ch, src->source.file);
			break;
		case SOURCE_STRING:
			--src->source.string;
			break;
	}
}

inline static int roman_value_(int ch) {
	switch (ch) {
		case 'I':
			return 1;
		case 'V':
			return 5;
		case 'X':
			return 10;
		case 'L':
			return 50;
		case 'C':
			return 100;
		case 'D':
			return 500;
		case 'M':
			return 1000;
		default:
			return -1;
	}
}

int roman_specifier_(source_t* src, va_list* args, size_t* nRead) {
	// https://stackoverflow.com/a/45064346
	int* out = va_arg(*args, int*);
	int result = 0;

	bool iv_ix = false;
	bool xl_xc = false;
	bool cd_cm = false;
	int ch;
	int ch0 = 0;

	while ((ch = source_get_char_(src)) != SEOF) {
		++(*nRead);

		int value = roman_value_(ch);
		if (value == -1) {
			if (ch0 == -1) return 0;
			source_rewind_(src, ch);
			break;
		}

		result += value;

		if (ch0 == 'I' && (ch == 'V' || ch == 'X')) iv_ix = true;
		if (ch0 == 'X' && (ch == 'L' || ch == 'C')) xl_xc = true;
		if (ch0 == 'C' && (ch == 'D' || ch == 'M')) cd_cm = true;

		ch0 = ch;
	}

	if (ch0 == -1) return 0;

	if (iv_ix) result -= 2;
	if (xl_xc) result -= 20;
	if (cd_cm) result -= 200;

	*out = result;
	return 1;
}

const unsigned int FIB[] = {1,         2,         3,         5,          8,          13,        21,        34,
                            55,        89,        144,       233,        377,        610,       987,       1597,
                            2584,      4181,      6765,      10946,      17711,      28657,     46368,     75025,
                            121393,    196418,    317811,    514229,     832040,     1346269,   2178309,   3524578,
                            5702887,   9227465,   14930352,  24157817,   39088169,   63245986,  102334155, 165580141,
                            267914296, 433494437, 701408733, 1134903170, 1836311903, 2971215073};

int zeckendorf_specifier_(source_t* src, va_list* args, size_t* nRead) {
	unsigned int* out = va_arg(*args, unsigned int*);
	unsigned int result = 0;

	int ch;
	int ch0 = 0;

	for (size_t i = 0; (ch = source_get_char_(src)) != SEOF; ++i, ++(*nRead)) {
		if (ch != '1' && ch != '0') {
			if (ch0 == '1') {
				// Terminating condition. Remove the extra number added before.
				result -= FIB[i - 1];
			}
			source_rewind_(src, ch);
			break;
		}

		const int fibSize = sizeof(FIB) / sizeof(FIB[0]);
		if (i == fibSize) {
			if (ch == '1') {
				break;  // Terminating condition, all good.
			} else {
				return 0;  // Too many numbers, bail out.
			}
		}

		if (ch == '1') {
			result += FIB[i];
		}

		ch0 = ch;
	}

	*out = result;
	return 1;
}

bool valid_for_base_(int ch, int base, bool uppercase) {
	if (base < 2 || base > 36) return false;

	int ord = -1;
	if (ch >= '0' && ch <= '9')
		ord = ch - '0';
	else if (uppercase && ch >= 'A' && ch <= 'Z')
		ord = ch - 'A' + 10;
	else if (!uppercase && ch >= 'a' && ch <= 'z')
		ord = ch - 'a' + 10;
	return ord != -1 && ord < base;
}

int from_base_specifier_(source_t* src, va_list* args, bool uppercase, size_t* nRead) {
	int* out = va_arg(*args, int*);
	int base = va_arg(*args, int);

	char buffer[65];
	size_t length = 0;

	for (int ch; (ch = source_get_char_(src)) != SEOF; ++(*nRead)) {
		if (!valid_for_base_(ch, base, uppercase) || length == sizeof(buffer) - 2) {
			source_rewind_(src, ch);
			break;
		}
		buffer[length++] = (char)ch;
	}

	buffer[length] = '\0';

	long result;
	if (FAILED(long_from_base(buffer, length, base, &result))) return 0;
	if (result > INT32_MAX || result < INT32_MIN) return 0;

	*out = (int)result;
	return 1;
}

int from_base_lowercase_specifier_(source_t* source, va_list* args, size_t* nRead) {
	return from_base_specifier_(source, args, false, nRead);
}

int from_base_uppercase_specifier_(source_t* source, va_list* args, size_t* nRead) {
	return from_base_specifier_(source, args, true, nRead);
}

scanf_specifier_t get_custom_specifier_(const char* spec) {
	if (strcmp(spec, "%Ro") == 0) return &roman_specifier_;
	if (strcmp(spec, "%Zr") == 0) return &zeckendorf_specifier_;
	if (strcmp(spec, "%Cv") == 0) return &from_base_lowercase_specifier_;
	if (strcmp(spec, "%CV") == 0) return &from_base_uppercase_specifier_;

	return NULL;
}

bool is_builtin_specifier_(char ch) {
	// See https://man7.org/linux/man-pages/man3/sscanf.3.html for more info.
	switch (ch) {
		case 'd':
		case 'i':
		case 'o':
		case 'u':
		case 'x':
		case 'X':
		case 'f':
		case 'e':
		case 'g':
		case 'E':
		case 'a':
		case 's':
		case 'c':
		case 'p':
			return true;
		default:
			return false;
	}
}

int overfscanf_(source_t* src, const char* fmt, va_list args) {
	if (src == NULL || fmt == NULL) return -1;

	int assigned = 0;  // Amount of variables assigned.
	size_t nRead = 0;  // Amount of chars read.

	char spec[64];                // Current format specifier being read.
	size_t length = 0;            // Length of the specifier.
	bool suppressAssign = false;  // Optional assignment suppressor as per the `scanf` spec.

	while (*fmt != '\0') {
		if (length && *fmt != '%') {  // Format specifier parsing.
			                          // Ignore additional '%' (escape sequence).
			const size_t maxLength = sizeof(spec) / sizeof(char) - 1;
			// Provided specifier is too large.
			if (length == maxLength - 1) return assigned;

			spec[length++] = *fmt;
			spec[length] = '\0';

			int assigned_ = -1;  // "-1" -- no specifier handler executed.

			// Try resolving a custom specifier. If it fails, call original `scanf`.
			scanf_specifier_t customSpecifier = get_custom_specifier_(spec);

			if (customSpecifier) {
				assigned_ = customSpecifier(src, &args, &nRead);
				if (assigned_ == 0) return assigned;
			}
			// '%n' assigns the amount of characters read to the next pointer.
			else if (*fmt == 'n') {
				// NB: not adding all the others, it's ridiculous.
				if (strcmp(spec, "%n") == 0) {
					*(va_arg(args, int*)) = (int)nRead;
				} else if (strcmp(spec, "%zn") == 0) {
					*(va_arg(args, size_t*)) = (size_t)nRead;
				}
				assigned_ = 0;  // Shouldn't increase the count.
			}
			// (1) Check for a character that ends the builtin (`scanf`) specifier.
			// (2) Check that the ']' closes the pattern. Per the manpage, it doesn't
			//     count if it's after a '^'.
			else if (is_builtin_specifier_(*fmt) || (*fmt == ']' && *(fmt - 1) != '^')) {
				// Add '%n' to count the chars `scanf` consumed. This is needed for our '%n'.
				strlcat(spec, "%zn", sizeof(spec));
				ssize_t read = -1;

				if (suppressAssign) {
					assigned_ = source_scanf_(src, spec, &read);
					// If `read` wasn't changed, the matching failed, and we bail out.
					if (read == -1) return assigned;
				} else {
					assigned_ = source_scanf_(src, spec, va_arg(args, void*), &read);
					if (assigned_ == 0) return assigned;
				}

				if (src->type == SOURCE_STRING) {
					// Advance the stream manually.
					src->source.string += read;
				}

				nRead += read;
			}

			if (assigned_ != -1) {
				assigned += assigned_;
				length = 0;
			}
		} else if (!length && *fmt == '%') {  // Format specifier start
			spec[length++] = *fmt;
			// `scanf` specs specifies an optional assignment suppressor (*). It's either
			// directly after the '%', or after a quote (') after '%'. We don't check for
			// the quote.
			suppressAssign = *(fmt + 1) != '\0' && (*(fmt + 1) == '*' || *(fmt + 2) == '*');
		} else {  // Other characters in the pattern
			int ch;
			if (*fmt == ' ') {
				// Skip an arbitrary amount of spaces (can be none).
				while (isspace(ch = source_get_char_(src))) ++nRead;
				source_rewind_(src, ch);
			} else {
				// No full match with the pattern -- exit.
				if (source_get_char_(src) != *fmt) return assigned;
				++nRead;
			}
			// If there are two consecutive '%%' (escape sequence), we end up here in an
			// unfinished parsing state. Reset it.
			length = 0;
		}

		++fmt;
	}

	return assigned;
}

int overfscanf(FILE* input, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);

	source_t src = {.type = SOURCE_FILE, .source = {.file = input}};
	int assigned = overfscanf_(&src, fmt, args);

	va_end(args);
	return assigned;
}

int oversscanf(char* input, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);

	source_t src = {.type = SOURCE_STRING, .source = {.string = input}};
	int assigned = overfscanf_(&src, fmt, args);

	va_end(args);
	return assigned;
}