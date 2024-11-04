#pragma once

#include <stdbool.h>

extern const char* kCharsSepLf;
extern const char* kCharsSepCr;
extern const char* kCharsSepCrlf;
extern const char* kCharsSepNone;

/** Returns whether `ch` is a digit character. */
bool chars_is_digit(char ch);

/** Returns whether `ch` is a lower-case or upper-case ASCII letter. */
bool chars_is_alpha(char ch);

/** Returns whether `ch` is a whitespace character. */
bool chars_is_space(char ch);

/** Converts upper-case ASCII letters to lower-case. */
char chars_lower(char ch);

/** Converts a lower-case ASCII letter to upper-case. */
char chars_upper(char ch);

/**
 * Checks if a string at |ptr| is a line separator.
 *
 * Supports:
 * 	- LF (kCharsSepLf)
 * 	- CR (kCharsSepCr)
 * 	- CRLF (kCharsSepCrlf)
 *
 * @param ptr string to be checked
 *
 * @return line separator; '\0' (kCharsSepNone) if it's not a separator
 */
const char* chars_line_separator(const char* ptr);
