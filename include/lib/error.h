#pragma once

typedef long error_t;

#define ERROR_SUCCESS 0L
#define ERROR_OVERFLOW 1L
#define ERROR_INVALID_PARAMETER 2L
#define ERROR_LIMIT_FAIL 3L
#define ERROR_HEAP_ALLOCATION 4L
#define ERROR_IO 5L

void error_print(error_t error);
