#include "lib/collections/string.h"

#include <string.h>

#include "lib/mth.h"
#include "lib/utils.h"

/** Comparison function for |vector_str_t|, which takes multiple void
 * pointers. */
inline static int string_vector_compare(const void* p1, const void* p2) {
	return string_compare((const string_t*)p1, (const string_t*)p2);
}

IMPL_VECTOR(vector_str_t, string_t, str, {&string_vector_compare})

bool string_enlarge(string_t* s, size_t size) {
	if (!s->initialized) return false;
	return vector_i8_ensure_capacity(&s->buffer,
	                                 vector_i8_size(&s->buffer) + size);
}

bool string_create(string_t* string) {
	if (!string) return false;

	*string = (string_t){.buffer = vector_i8_create(), .initialized = false};

	// Null-terminate the string by default.
	if (vector_i8_push_back(&string->buffer, '\0')) {
		string->initialized = true;
		return true;
	} else {
		string_destroy(string);
		return false;
	}
}

bool string_from_c_str(string_t* string, const char* cStr) {
	return string_create(string) && string_append_c_str(string, cStr);
}

void string_destroy(string_t* string) {
	vector_i8_destroy(&string->buffer);
	string->initialized = false;
}

bool string_append_char(string_t* string, char c) {
	if (!string->initialized) return false;

	int8_t t;
	return (vector_i8_pop_back(&string->buffer, &t) && t == '\0') &&
	       vector_i8_push_back(&string->buffer, c) &&
	       vector_i8_push_back(&string->buffer, '\0');
}

bool string_append_c_str(string_t* string, const char* cStr) {
	if (!string->initialized || !cStr) return false;

	// 1) Remove null-terminator.
	int8_t t;
	if (!vector_i8_pop_back(&string->buffer, &t) || t != '\0') {
		return false;
	}

	// 2) Copy characters from |cStr| to |string| (including the null-term.)
	char* ptr = (char*)cStr;

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
	int8_t t;
	if (!string_enlarge(string, string_length(other)) ||
	    !vector_i8_pop_back(&string->buffer, &t) || t != '\0')
		return false;

	// 3) Copy characters from |other| to |string|.
	for (size_t i = 0; i != string_length(other); i++) {
		char ch = string_char_at(other, i);
		if (!vector_i8_push_back(&string->buffer, ch)) return false;
	}

	// 4) Add the null-terminator.
	if (!vector_i8_push_back(&string->buffer, '\0')) return false;

	return true;
}

bool string_copy(string_t* src, string_t* dst) {
	// Initialize |dst| if it's not initialized.
	if (!string_created(dst)) {
		if (!string_create(dst)) return false;
	}

	// Resize the dest buffer to fit the source string.
	if (!vector_i8_ensure_capacity(&dst->buffer, vector_i8_size(&src->buffer)))
		return false;

	// Copy contents of |dst| into |src|.
	memcpy((char*)vector_i8_to_array(&dst->buffer),
	       (char*)vector_i8_to_array(&src->buffer),
	       vector_i8_size(&src->buffer));

	dst->buffer.size = src->buffer.size;
	return true;
}

size_t string_length(string_t* s) {
	// Account for the null-term.
	return s->initialized ? vector_i8_size(&s->buffer) - 1 : 0;
}

const char* string_to_c_str(string_t* s) {
	return s->initialized ? (const char*)vector_i8_to_array(&s->buffer) : NULL;
}

char string_char_at(string_t* s, size_t index) {
	if (!s->initialized) return INT8_MAX;

	char* ch = (char*)vector_i8_get(&s->buffer, index);
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

	int order;
	order = mth_sign_long((long)string_length((string_t*)str1) -
	                      (long)string_length((string_t*)str2));
	if (order) return order;

	const char* buf1 = string_to_c_str((string_t*)str1);
	const char* buf2 = string_to_c_str((string_t*)str2);
	if (buf1 == buf2) return 0;

	return mth_sign_long(strcmp(buf1, buf2));
}

bool string_clear(string_t* string) {
	if (!string || !string->initialized) return false;
	return vector_i8_clear(&string->buffer) &&
	       vector_i8_push_back(&string->buffer, '\0');
}

bool char_in_charset(char ch, const char* chars) {
	for (char* p = (char*)chars; *p; ++p) {
		if (*p == ch) return true;
	}

	return false;
}

bool string_strip0(string_t* string, const char* chars, bool leading,
                   bool trailing) {
	if (!string || !string->initialized) return false;
	if (!string_length(string)) return true;

	// Index of the first non-whitespace char
	size_t first = 0;
	// Index of the last non-whitespace char
	size_t last = string_length(string) - 1;

	if (trailing) {
		for (; last > 0; --last) {
			if (!char_in_charset(string_char_at(string, last), chars)) break;
		}

		*vector_i8_get(&string->buffer, last + 1) = '\0';
		string->buffer.size = last + 2;  // Account for the '\0'
	}

	if (leading) {
		for (; first < string_length(string); ++first) {
			if (!char_in_charset(string_char_at(string, first), chars)) break;
		}

		if (first) {
			int8_t* buffer = (int8_t*)vector_i8_to_array(&string->buffer);
			memmove(buffer, buffer + first,
			        (vector_i8_size(&string->buffer) - first) * sizeof(int8_t));
			string->buffer.size -= first;
		}
	}

	return true;
}

bool string_strip(string_t* string, const char* chars) {
	return string_strip0(string, chars, true, true);
}

bool string_lstrip(string_t* string, const char* chars) {
	return string_strip0(string, chars, true, false);
}

bool string_rstrip(string_t* string, const char* chars) {
	return string_strip0(string, chars, false, true);
}
