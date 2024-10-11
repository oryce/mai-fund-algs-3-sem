#pragma once

#include <stdbool.h>

extern const char* kCharsSepLf;
extern const char* kCharsSepCr;
extern const char* kCharsSepCrlf;
extern const char* kCharsSepNone;

/** Returns whether `ch` is a digit character. */
inline static bool chars_is_digit(char ch) { return ch >= '0' && ch <= '9'; }

/** Returns whether `ch` is a lower-case or upper-case ASCII letter. */
inline static bool chars_is_alpha(char ch) { return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'); }

/** Converts upper-case ASCII letters to lower-case. */
inline static char chars_lower(char ch) { return (char)(ch >= 'A' && ch <= 'Z' ? ch + 32 : ch); }

/** Converts a lower-case ASCII letter to upper-case. */
inline static char chars_upper(char ch) { return (char)(ch >= 'a' && ch <= 'z' ? ch - 32 : ch); }

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
