#include "tasks.h"

#include <stdlib.h>
#include <string.h>

#include "lib/chars.h"

typedef bool (*line_count_predicate_t)(char ch);

bool predicate_is_letter_(char ch) { return chars_is_alpha(ch); }

bool predicate_is_special_(char ch) { return !chars_is_alpha(ch) && !chars_is_digit(ch) && ch != ' '; }

void remove_digits_cleanup_(char* line, char* modified) {
	free(line);
	free(modified);
}

error_t task_remove_digits(FILE* in, FILE* out) {
	char* line = NULL;
	char* modified = NULL;
	size_t lineCap;
	size_t modifiedCap = 0;

	while (getline(&line, &lineCap, in) > 0) {
		if (lineCap == 0) {
			remove_digits_cleanup_(line, modified);
			return ERROR_ASSERT;
		}

		if (lineCap > modifiedCap) {
			char* newBuffer = (char*)realloc(modified, lineCap);
			if (!newBuffer) {
				remove_digits_cleanup_(line, modified);
				return ERROR_OUT_OF_MEMORY;
			}
			modified = newBuffer;
			modifiedCap = lineCap;
		}

		if (!modified) {
			remove_digits_cleanup_(line, modified);
			return ERROR_ASSERT;
		}

		int i = 0;

		for (char* ptr = line; *ptr != '\0'; ++ptr) {
			if (chars_is_digit(*ptr)) continue;
			modified[i++] = *ptr;
		}

		modified[i] = '\0';
		fprintf(out, "%s", modified);

		if (ferror(out)) {
			remove_digits_cleanup_(line, modified);
			return ERROR_IO;
		}
	}

	remove_digits_cleanup_(line, modified);
	return 0;
}

error_t count_lines_predicate(FILE* in, FILE* out, line_count_predicate_t predicate) {
	char* line = NULL;
	size_t lineCap;

	while (getline(&line, &lineCap, in) > 0) {
		if (ferror(in)) {
			free(line);
			return ERROR_IO;
		}

		int count = 0;
		const char* separator;

		for (char* ptr = line; *ptr != '\0'; ++ptr) {
			separator = chars_line_separator(ptr);
			if (separator != kCharsSepNone) break;  // Line end

			bool counted = predicate(*ptr);
			if (counted) ++count;
		}

		line[strcspn(line, separator)] = '\0';
		fprintf(out, "%s %d%s", line, count, separator);

		if (ferror(out)) {
			free(line);
			return ERROR_IO;
		}
	}

	free(line);
	return 0;
}

error_t task_count_letters(FILE* in, FILE* out) { return count_lines_predicate(in, out, &predicate_is_letter_); }

error_t task_count_special_characters(FILE* in, FILE* out) {
	return count_lines_predicate(in, out, &predicate_is_special_);
}

inline static char to_hex_(int n) {
	if (n >= 16) return '\0';
	return (char)(n < 10 ? ('0' + n) : ('a' + n - 10));
}

void encode_non_digits_cleanup_(char* line, char* modified) {
	free(line);
	free(modified);
}

error_t task_encode_non_digits(FILE* in, FILE* out) {
	char* line = NULL;
	char* modified = NULL;
	size_t lineCap;
	size_t modifiedCap;

	while (getline(&line, &lineCap, in) > 0) {
		if (ferror(in)) {
			encode_non_digits_cleanup_(line, modified);
			return ERROR_IO;
		}

		// Allocate for the modified line buffer.
		// (4 * lineCap is the worst case, if all chars are encoded)
		if (!modified) {
			modifiedCap = lineCap * 4;
			modified = malloc(modifiedCap);
			if (!modified) {
				encode_non_digits_cleanup_(line, modified);
				return ERROR_OUT_OF_MEMORY;
			}
		} else if (lineCap * 4 > modifiedCap) {
			char* newBuffer = realloc(modified, lineCap * 4);
			if (!newBuffer) {
				encode_non_digits_cleanup_(line, modified);
				return ERROR_OUT_OF_MEMORY;
			}
			modified = newBuffer;
			modifiedCap = lineCap * 4;
		}

		int lIdx, rIdx;

		for (lIdx = 0, rIdx = 0; line[rIdx] != '\0'; ++rIdx) {
			bool separator = (line[rIdx] == '\r' && line[rIdx + 1] == '\n') || line[rIdx] == '\n';
			bool encode = !chars_is_digit(line[rIdx]) && !separator;

			if (encode) {
				unsigned char ch = (unsigned char)line[rIdx];
				int lo = (ch & 0x0F) >> 0;
				int hi = (ch & 0xF0) >> 4;

				modified[lIdx + 0] = '0';
				modified[lIdx + 1] = 'x';
				modified[lIdx + 2] = to_hex_(hi);
				modified[lIdx + 3] = to_hex_(lo);

				lIdx += 4;
			} else {
				modified[lIdx] = line[rIdx];
				++lIdx;
			}
		}

		modified[lIdx] = '\0';
		fprintf(out, "%s", modified);

		if (ferror(out)) {
			encode_non_digits_cleanup_(line, modified);
			return ERROR_IO;
		}
	}

	encode_non_digits_cleanup_(line, modified);
	return 0;
}
