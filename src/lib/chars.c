#include "lib/chars.h"

#include <string.h>

const char* kCharsSepLf = "\n";
const char* kCharsSepCr = "\r";
const char* kCharsSepCrlf = "\r\n";
const char* kCharsSepNone = "\0";

const char* chars_line_separator(const char* ptr) {
	const char* separators[] = {kCharsSepLf, kCharsSepCr, kCharsSepCrlf};
	int nSeparators = sizeof(separators) / sizeof(const char*);

	for (int i = 0; i != nSeparators; ++i) {
		if (strcmp(separators[i], ptr) == 0) return separators[i];
	}

	return kCharsSepNone;
}
