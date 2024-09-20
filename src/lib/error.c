#include "error.h"

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
		default:
			fprintf(stderr, "Unknown error\n");
	}
}
