#include <stdarg.h>
#include <stdio.h>

#include "lib/chars.h"
#include "lib/collections/deque.h"
#include "lib/error.h"

typedef enum match_result { MATCH_FULL, MATCH_PARTIAL, MATCH_NONE } match_result_t;

typedef struct match {
	int line;
	int column;
	ptrdiff_t needleOffset;
} match_t;

DEFINE_DEQUE(deque_match_t, match_t, match)
IMPL_DEQUE(deque_match_t, match_t, match)

match_result_t match_substring(const char* cursor, const char* needle, match_t match, ptrdiff_t* outOffset) {
	if (cursor == NULL || needle == NULL) {
		return MATCH_NONE;
	}

	char* haystackCur = (char*)cursor;
	char* needleCur = (char*)needle + match.needleOffset;

	while (*haystackCur != '\0' && *needleCur != '\0') {
		if (*haystackCur != *needleCur) {
			return MATCH_NONE;
		}
		++haystackCur;
		++needleCur;
	}

	if (*haystackCur == '\0' && *needleCur != '\0') {
		*outOffset = needleCur - needle;
		return MATCH_PARTIAL;
	}

	return MATCH_FULL;
}

inline static void cleanup_(FILE* file, deque_match_t* full, deque_match_t* partial) {
	fclose(file);
	deque_match_destroy(full);
	deque_match_destroy(partial);
}

error_t find_substring_in_file(const char* needle, const char* filePath, deque_match_t* matches) {
	FILE* file = fopen(filePath, "r");
	if (file == NULL) {
		THROW(IOException, "can't open input file");
	}

	deque_match_t fullMatches = deque_match_create();
	deque_match_t partialMatches = deque_match_create();

	const int bufSize = 16;
	char buffer[bufSize];
	int line = 1;
	int column = 0;

	while (fgets(buffer, bufSize, file) != NULL) {
		size_t incompleteCnt = deque_match_size(&partialMatches);

		// Check matches that weren't checked fully due to the previous buffer running out.
		for (size_t i = 0; i != incompleteCnt; ++i) {
			match_t partial;
			if (!deque_match_pop_front(&partialMatches, &partial)) {
				cleanup_(file, &fullMatches, &partialMatches);
				THROW(MemoryError, "can't get partial match");
			}

			ptrdiff_t newOffset;
			match_result_t result = match_substring(buffer, needle, partial, &newOffset);

			if (result == MATCH_FULL) {
				if (!deque_match_push_back(&fullMatches, partial)) {
					cleanup_(file, &fullMatches, &partialMatches);
					THROW(MemoryError, "can't insert full match");
				}
			} else if (result == MATCH_PARTIAL) {
				partial.needleOffset = newOffset;
				if (!deque_match_push_back(&partialMatches, partial)) {
					cleanup_(file, &fullMatches, &partialMatches);
					THROW(MemoryError, "can't insert partial match");
				}
			}
		}

		// Continue checking from the start of the substring.
		for (char* cursor = buffer; *cursor != '\0'; ++cursor) {
			// Check for line separator.
			if (chars_line_separator(cursor) != kCharsSepNone) {
				++line;
				column = 0;
			}

			match_t match = {line, column, 0};

			ptrdiff_t newOffset;
			match_result_t result = match_substring(cursor, needle, match, &newOffset);

			if (result == MATCH_FULL) {
				if (!deque_match_push_back(&fullMatches, match)) {
					cleanup_(file, &fullMatches, &partialMatches);
					THROW(MemoryError, "can't insert full match");
				}
			} else if (result == MATCH_PARTIAL) {
				// If the haystack buffer ran out, but the needle didn't, "schedule" it for checking.
				match.needleOffset = newOffset;
				if (!deque_match_push_back(&partialMatches, match)) {
					cleanup_(file, &fullMatches, &partialMatches);
					THROW(MemoryError, "can't insert partial match into queue");
				}
			}

			++column;
		}
	}

	if (ferror(file)) {
		cleanup_(file, &fullMatches, &partialMatches);
		THROW(MemoryError, "can't read from input file");
	}

	*matches = fullMatches;
	fclose(file);
	deque_match_destroy(&partialMatches);

	return NO_EXCEPTION;
}

error_t find_substring_in_files(const char* needle, int fileCnt, ...) {
	error_t status;

	va_list files;
	va_start(files, fileCnt);

	for (int i = 0; i != fileCnt; ++i) {
		const char* path = va_arg(files, const char*);
		deque_match_t matches;

		if (FAILED((status = find_substring_in_file(needle, path, &matches)))) {
			PASS(status);
		}

		if (deque_match_is_empty(&matches)) {
			printf("No matches in %s\n", path);
		} else {
			printf("%zu match(es) in %s:\n", deque_match_size(&matches), path);
			while (!deque_match_is_empty(&matches)) {
				match_t match;
				if (!deque_match_pop_front(&matches, &match)) {
					deque_match_destroy(&matches);
					THROW(MemoryError, "can't read match");
				}
				printf("  at %d:%d\n", match.line, match.column);
			}
		}

		deque_match_destroy(&matches);
	}

	va_end(files);
	return NO_EXCEPTION;
}
