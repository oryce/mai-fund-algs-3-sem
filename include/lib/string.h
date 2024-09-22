#pragma once

#include "lib/vector.h"

typedef struct string {
	vector_i8_t buffer;
} string_t;

string_t string_create(void);

void string_destroy(string_t* string);

bool string_append_char(string_t* string, char c);

bool string_append_c_str(string_t* string, const char* const_str);

bool string_append(string_t* string, string_t* other);

size_t string_length(string_t* string);

const char* string_to_c_str(string_t* string);

char string_char_at(string_t* string, size_t index);

void string_reverse(string_t* string);
