#pragma once

typedef long error_t;

#define ERROR_SUCCESS 0L
#define ERROR_OVERFLOW 1L
#define ERROR_INVALID_PARAMETER 2L

void error_print(error_t error);
