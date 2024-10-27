#include "lib/collections/string.h"

#include <string.h>

#include "lib/mth.h"
#include "lib/utils.h"

/** Comparison function for |vector_string_t|, which takes multiple void
 * pointers. */
inline static int string_vector_compare(const void* p1, const void* p2) {
	return string_compare((const string_t*)p1, (const string_t*)p2);
}

IMPL_VECTOR(vector_str_t, string_t, str, {&string_vector_compare})

bool string_enlarge(string_t* string, size_t size) {
	if (!string->initialized) return false;
	return vector_i8_ensure_capacity(&string->buffer,
	                                 vector_i8_size(&string->buffer) + size);
}

bool string_create(string_t* string) {
	if (!string) return false;

	*string = (string_t){.buffer = vector_i8_create(), .initialized = false};

	// Null-terminate the string by default. If the push succeeds, mark
	// the string as available for use ("initialized"). This is to prevent
	// non-null-terminated strings.
	if (vector_i8_push_back(&string->buffer, '\0')) {
		string->initialized = true;
		return true;
	} else {
		string_destroy(string);
		return false;
	}
}

void string_destroy(string_t* string) {
	vector_i8_destroy(&string->buffer);
	string->initialized = false;
}

bool string_append_char(string_t* string, char c) {
	if (!string->initialized) return false;
	int8_t terminator;

	bool success = true;
	success &=
	    vector_i8_pop_back(&string->buffer, &terminator) && terminator == '\0';
	success &= vector_i8_push_back(&string->buffer, c);
	success &= vector_i8_push_back(&string->buffer, '\0');
	return success;
}

bool string_append_c_str(string_t* string, const char* c) {
	if (!string->initialized || c == NULL) return false;

	// 1) Remove null-terminator.
	int8_t terminator;
	if (!vector_i8_pop_back(&string->buffer, &terminator)) return false;
	if (terminator != '\0') return false;

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
	int8_t terminator;
	if (!string_enlarge(string, string_length(other))) return false;
	if (!vector_i8_pop_back(&string->buffer, &terminator)) return false;
	if (terminator != '\0') return false;

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

	char* ch = (char*)vector_i8_get(&string->buffer, index);
	return (char)(ch == NULL ? INT8_MAX : *ch);
}

void string_reverse(string_t* string) {
	if (!string->initialized) return;

	vector_i8_t* buffer = &string->buffer;
	int8_t* l = vector_i8_get(buffer, 0);
	int8_t* r = vector_i8_get(buffer, string_length(string) - 1);

	for (; l < r; ++l, --r) {
		SWAP(*l, *r, int8_t);
	}
}

int string_compare(const string_t* str1, const string_t* str2) {
	if (!str1 || !str2 || str1 == str2) return 0;

	const char* buf1 = string_to_c_str((string_t*)str1);
	const char* buf2 = string_to_c_str((string_t*)str2);
	if (buf1 == buf2) return 0;

	return mth_sign_long(strcmp(buf1, buf2));
}
