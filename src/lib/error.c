#include "lib/error.h"

#include <stdio.h>

void error_print(error_t error) {
	switch (error) {
		case ERROR_SUCCESS:
			fprintf(stderr, "No error\n");
			break;
		case ERROR_OVERFLOW:
			fprintf(stderr, "Error: Number overflow\n");
			break;
		case ERROR_INVALID_PARAMETER:
			fprintf(stderr, "Error: Invalid parameter\n");
			break;
		case ERROR_LIMIT_FAIL:
			fprintf(stderr, "Error: Limit cannot be computed\n");
			break;
		case ERROR_HEAP_ALLOCATION:
			fprintf(stderr, "Error: Cannot allocate heap memory\n");
			break;
		case ERROR_IO:
			fprintf(stderr, "Error: I/O error. File read/write failed\n");
			break;
		case ERROR_DIVERGING:
			fprintf(stderr, "Error: Diverging sequence\n");
			break;
		case ERROR_INTEGRAL_FAIL:
			fprintf(stderr, "Error: Integral cannot be computed\n");
			break;
		case ERROR_OOPS:
			fprintf(stderr, "Error: Unexpected condition\n");
			break;
		case ERROR_UNDERFLOW:
			fprintf(stderr, "Error: Number underflow.\n");
			break;
		default:
			fprintf(stderr, "Unknown error\n");
	}
}
