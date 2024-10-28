#pragma once

#include "lib/collections/string.h"

bool string_from_c_str(string_t* string, const char* cStr);

string_t* string_dup(string_t* string);
