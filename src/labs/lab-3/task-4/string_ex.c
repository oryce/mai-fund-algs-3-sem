#include "string_ex.h"

string_t string_create_empty(void) { return (string_t){.initialized = false}; }

string_t* string_dup(string_t* string) {
	string_t* new = (string_t*)calloc(1, sizeof(string_t));
	return new && string_copy(string, new) ? new : NULL;
}
