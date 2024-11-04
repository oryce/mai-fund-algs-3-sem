#include "lib/chars.h"

#include <string.h>

const char* kCharsSepLf = "\n";
const char* kCharsSepCr = "\r";
const char* kCharsSepCrlf = "\r\n";
const char* kCharsSepNone = "\0";

bool chars_is_digit(char ch) { return ch >= '0' && ch <= '9'; }

bool chars_is_alpha(char ch) {
	return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

bool chars_is_space(char ch) {
	return ch == ' ' || ch == '\t' || ch == '\r';
}

char chars_lower(char ch) {
	return (char)(ch >= 'A' && ch <= 'Z' ? ch + 32 : ch);
}

char chars_upper(char ch) {
	return (char)(ch >= 'a' && ch <= 'z' ? ch - 32 : ch);
}

const char* chars_line_separator(const char* ptr) {
	const char* separators[] = {kCharsSepLf, kCharsSepCr, kCharsSepCrlf};
	int nSeparators = sizeof(separators) / sizeof(const char*);

	for (int i = 0; i != nSeparators; ++i) {
		if (strcmp(separators[i], ptr) == 0) return separators[i];
	}

	return kCharsSepNone;
}
