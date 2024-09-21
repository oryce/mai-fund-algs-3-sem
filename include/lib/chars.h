#include <stdbool.h>

inline bool chars_is_digit(int ch) { return ch >= '0' && ch <= '9'; }

inline bool chars_is_alpha(int ch) { return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'); }
