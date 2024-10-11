#include "lib/collections/string.h"

#include <string.h>

const string_t STRING_DEF = {.initialized = false};

/** Comparison function for |vector_string_t|, which takes two void pointers. */
inline static int string_vector_compare(const void* p1, const void* p2) {
	return string_compare((const string_t*)p1, (const string_t*)p2);
}

IMPL_VECTOR(vector_str_t, string_t, str, STRING_DEF, {&string_vector_compare})

bool string_enlarge(string_t* string, size_t size) {
	if (!string->initialized) return false;
	return vector_i8_ensure_capacity(&string->buffer, vector_i8_size(&string->buffer) + size);
}

string_t string_create(void) {
	string_t string = {.buffer = vector_i8_create(), .initialized = false};

	// Null-terminate the string by default. If the push succeeds, mark
	// the string as available for use ("initialized"). This is to prevent
	// non-null-terminated strings.
	if (vector_i8_push_back(&string.buffer, '\0')) {
		string.initialized = true;
	}

	return string;
}

void string_destroy(string_t* string) {
	vector_i8_destroy(&string->buffer);
	string->initialized = false;
}

bool string_append_char(string_t* string, char c) {
	if (!string->initialized) return false;

	bool success = true;
	success &= vector_i8_pop_back(&string->buffer) == '\0';
	success &= vector_i8_push_back(&string->buffer, c);
	success &= vector_i8_push_back(&string->buffer, '\0');
	return success;
}

bool string_append_c_str(string_t* string, const char* c) {
	if (!string->initialized || c == NULL) return false;

	// 1) Remove null-terminator.
	if (vector_i8_pop_back(&string->buffer) != '\0') {
		return false;
	}

	// 2) Copy characters from |c| to |string| (including the null-term.)
	char* ptr = (char*)c;

	do {
		if (!vector_i8_push_back(&string->buffer, *ptr)) {
			return false;
		}
	} while (*(ptr++) != '\0');

	return true;
}

bool string_append(string_t* string, string_t* other) {
	if (!string->initialized || !other->initialized) return false;

	// 1) Enlarge the string ahead of time.
	// 2) Remove the null-terminator.
	if (!string_enlarge(string, string_length(other)) || vector_i8_pop_back(&string->buffer) != '\0') {
		return false;
	}

	// 3) Copy characters from |other| to |string|.
	for (size_t i = 0; i != string_length(other); i++) {
		char ch = string_char_at(other, i);
		if (!vector_i8_push_back(&string->buffer, ch)) {
			return false;
		}
	}

	// 4) Add the null-terminator.
	if (!vector_i8_push_back(&string->buffer, '\0')) {
		return false;
	}

	return true;
}

size_t string_length(string_t* string) {
	if (!string->initialized) return 0;
	return vector_i8_size(&string->buffer) - 1;  // Account for the null-term.
}

const char* string_to_c_str(string_t* string) {
	if (!string->initialized) return NULL;
	return (const char*)vector_i8_to_array(&string->buffer);
}

char string_char_at(string_t* string, size_t index) {
	if (!string->initialized) return INT8_MAX;
	return vector_i8_get(&string->buffer, index);
}

void string_reverse(string_t* string) {
	if (!string->initialized) return;

	size_t i = string_length(string) - 1;
	size_t j = 0;

	while (i > j) {
		vector_i8_swap(&string->buffer, i, j);
		i--;
		j++;
	}
}

int string_compare(const string_t* s1, const string_t* s2) {
	if (s1 == NULL || s2 == NULL || s1 == s2) return 0;

	const char* b1 = string_to_c_str((string_t*)s1);
	const char* b2 = string_to_c_str((string_t*)s2);
	if (b1 == b2) return 0;

	int result = strcmp(b1, b2);

	if (result < 0)
		return -1;
	else if (result == 0)
		return 0;
	else
		return 1;
}
