#pragma once

#include <stdio.h>
#include <time.h>

#include "heap.h"
#include "lib/error.h"
#include "storage.h"

#define ERROR_MODEL_UNKNOWN_HEAP_TYPE 0x30000001
#define ERROR_MODEL_UNKNOWN_STORAGE_TYPE 0x30000002
#define ERROR_MODEL_INVALID_START_TIME 0x30000003
#define ERROR_MODEL_INVALID_END_TIME 0x30000004
#define ERROR_MODEL_INVALID_MIN_PROCESS_TIME 0x30000005
#define ERROR_MODEL_INVALID_MAX_PROCESS_TIME 0x30000006
#define ERROR_MODEL_INVALID_DEPARTMENT_COUNT 0x30000007
#define ERROR_MODEL_INVALID_OPERATOR_COUNT 0x30000008
#define ERROR_MODEL_INVALID_OVERLOAD_FACTOR 0x30000009
#define ERROR_MODEL_ALREADY_INITIALIZED 0x30000010
#define ERROR_MODEL_UNKNOWN_DEPARTMENT 0x30000011

#define MODEL_MAX_DEPARTMENTS 100
#define MODEL_MAX_OPERATORS 50

typedef struct model {
	heap_type_t requestHeapType;
	storage_type_t deptStorageType;
	time_t startTime;
	time_t endTime;
	unsigned minProcessTime;
	unsigned maxProcessTime;
	size_t departmentCount;
	size_t operatorCount[MODEL_MAX_DEPARTMENTS];
	double overloadFactor[MODEL_MAX_DEPARTMENTS];

	time_t time;
	department_t* departments;
	storage_t* departmentMap;
} model_t;

typedef enum event {
	NEW_REQUEST,
	REQUEST_HANDLING_STARTED,
	REQUEST_HANDLING_FINISHED,
	DEPARTMENT_OVERLOADED
} event_t;

model_t model_create();

void model_destroy(model_t* model);

error_t model_from_file(FILE* stream, model_t* out);

error_t model_init(model_t* model);

error_t model_run(model_t* model, deque_request_t* requests, FILE* logFile);

const char* model_error_to_string(error_t error);
