#pragma once

#include "lib/error.h"

error_t task_string_length(const char* str, size_t* length);

error_t task_reverse_string(const char* str, char** out);

error_t task_uppercase_odd_chars(const char* str, char** out);

error_t task_rearrange_chars(const char* str, char** out);

error_t task_concat_randomly(const char** strings, size_t n, char** out);
