#pragma once

#include "vector.h"

typedef struct string {
	vector_i8_t buffer;
	bool initialized;
} string_t;

DEFINE_VECTOR(vector_str_t, string_t, str)

/** Creates a new string. */
bool string_create(string_t* string);

/** Creates a new string from a null-terminated string. */
bool string_from_c_str(string_t* string, const char* cStr);

/** Destroys the string, freeing allocated memory. */
void string_destroy(string_t* string);

/** Returns whether the string was initialized
 * (null-terminated, can be used safely). */
inline static bool string_created(string_t* string) {
	return string->initialized;
}

/**
 * Appends a character to the |input| string.
 *
 * @param string input string
 * @param c      character
 */
bool string_append_char(string_t* string, char c);

/**
 * Appends a null-terminated string to the |input| string.
 *
 * @param string input string
 * @param cStr   null-terminated string
 */
bool string_append_c_str(string_t* string, const char* cStr);

/**
 * Appends |other| string to the |input| string.
 *
 * @param string input string
 * @param other  another string
 */
bool string_append(string_t* string, string_t* other);

/**
 * Copies the |src| to |dst|, overwriting the contents.
 *
 * If the |dst| src wasn't created prior to calling |string_copy|,
 * it's initialized with an empty src.
 *
 * @param src source src
 * @param dst dest src
 */
bool string_copy(string_t* src, string_t* dst);

/**
 * Returns the length of the string.
 *
 * @param s input string
 * @return length of the string
 */
size_t string_length(string_t* s);

/**
 * Returns a pointer to the string buffer.
 *
 * @param s input string
 * @return string buffer
 */
const char* string_to_c_str(string_t* s);

/**
 * Returns a character in |s| at position |index|.
 *
 * @param s     input string
 * @param index character index
 * @return character at position |index|;
 *         INT8_MAX if the index is invalid.
 */
char string_char_at(string_t* s, size_t index);

/**
 * Reverses the string in-place.
 *
 * @param string input string
 */
void string_reverse(string_t* string);

/**
 * Compares two strings.
 *
 * @param str1 first string
 * @param str2 second string
 *
 * @return -1 if str1 < str2;
 * 			0 if str1 == str2;
 * 			1 if str1 > str2.
 */
int string_compare(const string_t* str1, const string_t* str2);
