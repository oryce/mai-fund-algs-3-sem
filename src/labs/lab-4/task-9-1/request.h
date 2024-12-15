#pragma once

#include <time.h>

#include "lib/collections/deque.h"
#include "lib/collections/vector.h"
#include "lib/error.h"

#define ERROR_REQUEST_INVALID_TIME 0x40000001
#define ERROR_REQUEST_INVALID_PRIORITY 0x40000002
#define ERROR_REQUEST_INVALID_TEXT 0x40000003

typedef struct request {
	unsigned long id;
	time_t time;
	unsigned priority;
	const char* departmentId;
	const char* text;
	unsigned requiredTime;
} request_t;

DEFINE_DEQUE(deque_request_t, request_t, request)
DEFINE_VECTOR(vector_request_t, request_t, request)

void request_destroy(request_t* request);

error_t request_from_string(const char* string, request_t* out,
                            unsigned maxPriority);

error_t request_from_files(FILE* files[], size_t nFiles, deque_request_t* out,
                           unsigned maxPriority);

int request_priority_cmp(const request_t* a, const request_t* b);

const char* request_error_to_string(error_t error);
