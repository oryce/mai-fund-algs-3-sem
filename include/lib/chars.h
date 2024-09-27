#pragma once

#include <stdbool.h>

extern const char* kCharsSepLf;
extern const char* kCharsSepCr;
extern const char* kCharsSepCrlf;
extern const char* kCharsSepNone;

inline static bool chars_is_digit(int ch) { return ch >= '0' && ch <= '9'; }

inline static bool chars_is_alpha(int ch) { return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'); }

inline static int chars_lower(int ch) { return ch >= 'A' && ch <= 'Z' ? ch + 32 : ch; }

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
