#include "tasks.h"

#include <stdlib.h>
#include <string.h>

#include "lib/chars.h"

typedef bool (*line_count_predicate_t)(char ch);

bool predicate_is_letter(char ch) { return chars_is_alpha(ch); }

bool predicate_is_special(char ch) { return !chars_is_alpha(ch) && !chars_is_digit(ch) && ch != ' '; }

error_t remove_digits(FILE* in, FILE* out) {
	error_t error = ERROR_SUCCESS;

	char *line = NULL, *modified = NULL;
	size_t lineCap, modifiedCap = 0;

	while (getline(&line, &lineCap, in) > 0) {
		if (lineCap == 0) {
			error = error_oops("`getline` returned `lineCap` as zero");
			goto cleanup;
		}

		if (lineCap > modifiedCap) {
			char* newBuffer = realloc(modified, lineCap);

			if (newBuffer == NULL) {
				error = ERROR_HEAP_ALLOCATION;
				goto cleanup;
			}

			modified = newBuffer;
			modifiedCap = lineCap;
		}

		if (modified == NULL) {
			error = error_oops("`modified` buffer is null");
			goto cleanup;
		}

		int i = 0;

		for (char* ptr = line; *ptr != '\0'; ++ptr) {
			if (chars_is_digit(*ptr)) continue;
			modified[i++] = *ptr;
		}

		modified[i] = '\0';
		fprintf(out, "%s", modified);

		if (ferror(out)) {
			error = ERROR_IO;
			goto cleanup;
		}
	}

cleanup:
	free(line);
	free(modified);
	return error;
}

error_t count_lines_predicate(FILE* in, FILE* out, line_count_predicate_t predicate) {
	error_t error = ERROR_SUCCESS;

	char* line = NULL;
	size_t lineCap;

	while (getline(&line, &lineCap, in) > 0) {
		if (ferror(in)) {
			error = ERROR_IO;
			goto cleanup;
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
			error = ERROR_IO;
			goto cleanup;
		}
	}

cleanup:
	free(line);
	return error;
}

error_t count_letters(FILE* in, FILE* out) { return count_lines_predicate(in, out, &predicate_is_letter); }

error_t count_special_characters(FILE* in, FILE* out) { return count_lines_predicate(in, out, &predicate_is_special); }

inline static char to_hex(int n) {
	if (n >= 16) return '\0';
	return (char)(n < 10 ? ('0' + n) : ('a' + n - 10));
}

error_t encode_non_digits(FILE* in, FILE* out) {
	error_t error = ERROR_SUCCESS;

	char *line = NULL, *modified = NULL;
	size_t lineCap, modifiedCap;

	while (getline(&line, &lineCap, in) > 0) {
		if (ferror(in)) {
			error = ERROR_IO;
			goto cleanup;
		}

		// Allocate for the modified line buffer.
		// (4 * lineCap is the worst case, if all chars are encoded)
		if (modified == NULL) {
			modifiedCap = lineCap * 4;
			modified = malloc(modifiedCap);

			if (modified == NULL) {
				error = ERROR_HEAP_ALLOCATION;
				goto cleanup;
			}
		} else if (lineCap * 4 > modifiedCap) {
			modifiedCap = lineCap * 4;
			char* newBuffer = realloc(modified, modifiedCap);

			if (newBuffer == NULL) {
				error = ERROR_HEAP_ALLOCATION;
				goto cleanup;
			}

			modified = newBuffer;
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
				modified[lIdx + 2] = to_hex(hi);
				modified[lIdx + 3] = to_hex(lo);

				lIdx += 4;
			} else {
				modified[lIdx] = line[rIdx];
				++lIdx;
			}
		}

		modified[lIdx] = '\0';
		fprintf(out, "%s", modified);
	}

cleanup:
	free(line);
	free(modified);
	return error;
}
