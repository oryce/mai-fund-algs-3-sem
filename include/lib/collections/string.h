#pragma once

#include "vector.h"

typedef struct string {
	vector_i8_t buffer;
	bool initialized;
} string_t;

/** Default value of |string_t| for |vector_string_t|. */
extern const string_t STRING_DEF;

DEFINE_VECTOR(string_t, str)

/** Creates a new string. */
string_t string_create(void);

/** Destroys the string, freeing allocated memory. */
void string_destroy(string_t* string);

/** Returns whether the string was initialized successfully. */
inline static bool string_created(string_t* string) {
	return string->initialized;
}

/**
 * Appends a character to the input string.
 *
 * @param string input string
 * @param c character
 * @return |true| if the append was successful, |false| otherwise.
 */
bool string_append_char(string_t* string, char c);

/**
 * Appends a null-terminated string to the input string.
 *
 * @param string input string
 * @param const_str null-terminated string
 * @return |true| if the append was successful, |false| otherwise.
 */
bool string_append_c_str(string_t* string, const char* const_str);

/**
 * Appends another |string_t| to the input string.
 *
 * @param string input string
 * @param other another string
 * @return |true| if the append was successful, |false| otherwise.
 */
bool string_append(string_t* string, string_t* other);

/**
 * Returns the length of the string.
 *
 * @param string input string
 * @return length of the string
 */
size_t string_length(string_t* string);

/**
 * Returns a pointer to the string buffer.
 *
 * @param string input string
 * @return string buffer
 */
const char* string_to_c_str(string_t* string);

/**
 * Returns a character in |string| at position |index|.
 *
 * @param string input string
 * @param index character index
 * @return character at position |index|; INT8_MAX if the index is invalid.
 */
char string_char_at(string_t* string, size_t index);

/**
 * Reverses the string in-place.
 *
 * @param string input string
 */
void string_reverse(string_t* string);

/**
 * Compares two strings.
 *
 * @param s1 first string
 * @param s2 second string
 *
 * @return -1 if s1 < s2;
 * 			0 if s1 == s2;
 * 			1 if s1 > s2.
 */
int string_compare(const string_t* s1, const string_t* s2);
