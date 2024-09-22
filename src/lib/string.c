#include "lib/string.h"

#include <string.h>

bool string_enlarge(string_t* string, size_t size) {
	return vector_i8_ensure_capacity(&string->buffer, vector_i8_size(&string->buffer) + size);
}

string_t string_create(void) {
	string_t string = {.buffer = vector_i8_create()};
	// Null-terminate the string by default.
	vector_i8_push_back(&string.buffer, '\0');

	return string;
}

void string_destroy(string_t* string) { vector_i8_destroy(&string->buffer); }

bool string_append_char(string_t* string, char c) {
	return vector_i8_pop_back(&string->buffer) == '\0' && vector_i8_push_back(&string->buffer, c) &&
	       vector_i8_push_back(&string->buffer, '\0');
}

bool string_append_c_str(string_t* string, const char* c) {
	// 1) Enlarge the string to fit the added string ahead of time.
	// 2) Remove null-terminator.
	if (!string_enlarge(string, strlen(c)) || vector_i8_pop_back(&string->buffer) != '\0') {
		return false;
	}

	// 3) Copy characters from |c| to |string|.
	for (size_t i = 0; i != strlen(c); i++)
		if (!vector_i8_push_back(&string->buffer, c[i])) {
			return false;
		}

	// 4) Add null-terminator.
	if (!vector_i8_push_back(&string->buffer, '\0')) {
		return false;
	}

	return true;
}

// Same as |string_append_c_str|, but with |string_length| instead of |strlen|.
bool string_append(string_t* string, string_t* other) {
	if (!string_enlarge(string, string_length(other)) || vector_i8_pop_back(&string->buffer) != '\0') {
		return false;
	}

	for (size_t i = 0; i != string_length(other); i++) {
		if (!vector_i8_push_back(&string->buffer, string_char_at(other, i))) {
			return false;
		}
	}

	if (!vector_i8_push_back(&string->buffer, '\0')) {
		return false;
	}

	return true;
}

size_t string_length(string_t* string) {
	// Account for the null-byte.
	return vector_i8_size(&string->buffer) - 1;
}

const char* string_to_c_str(string_t* string) { return (const char*)vector_i8_to_array(&string->buffer); }

char string_char_at(string_t* string, size_t index) { return vector_i8_get(&string->buffer, index); }

void string_reverse(string_t* string) {
	size_t i = string_length(string) - 1;
	size_t j = 0;

	while (i > j) {
		vector_i8_swap(&string->buffer, i, j);
		i--;
		j++;
	}
}
