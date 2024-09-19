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
		default:
			fprintf(stderr, "Unknown error\n");
	}
}
