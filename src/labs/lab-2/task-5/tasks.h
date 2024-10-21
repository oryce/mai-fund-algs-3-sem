#pragma once

#include "lib/error.h"

#include <stdio.h>

error_t overfprintf(FILE* output, const char* fmt, ...);

error_t oversprintf(char* output, const char* fmt, ...);
