#include "string_ex.h"

bool string_from_c_str(string_t* string, const char* cStr) {
	return string_create(string) && string_append_c_str(string, cStr);
}

string_t* string_dup(string_t* string) {
	string_t* new = (string_t*)calloc(1, sizeof(string_t));
	return new && string_copy(string, new) ? new : NULL;
}
