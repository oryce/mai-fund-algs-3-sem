#pragma once

#include "vector.h"

typedef struct string {
	vector_i8_t buffer;
	bool initialized;
} string_t;

DEFINE_VECTOR(vector_str_t, string_t, str)

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
 * Compares multiple strings.
 *
 * @param str1 first string
 * @param str2 second string
 *
 * @return -1 if str1 < str2;
 * 			0 if str1 == str2;
 * 			1 if str1 > str2.
 */
int string_compare(const string_t* str1, const string_t* str2);
