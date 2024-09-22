#pragma once

#include <stdbool.h>

inline static bool chars_is_digit(int ch) { return ch >= '0' && ch <= '9'; }

inline static bool chars_is_alpha(int ch) { return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'); }

inline static int chars_lower(int ch) { return ch >= 'A' && ch <= 'Z' ? ch + 32 : ch; }
