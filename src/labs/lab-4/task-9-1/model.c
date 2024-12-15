#include "model.h"

#include <float.h>
#include <stdarg.h>
#include <stdbool.h>
#include <time.h>

#include "lib/convert.h"
#include "lib/mth.h"
#include "lib/utils.h"

typedef enum read_state {
	READ_REQUEST_HEAP_TYPE,
	READ_DEPT_STORAGE_TYPE,
	READ_START_TIME,
	READ_END_TIME,
	READ_MIN_PROCESS_TIME,
	READ_MAX_PROCESS_TIME,
	READ_DEPARTMENT_COUNT,
	READ_OPERATOR_COUNT,
	READ_OVERLOAD_FACTOR,
	READ_VALID
} read_state_t;

error_t heap_type_from_string(const char* string, heap_type_t* outType) {
	if (!string || !outType) return ERROR_INVALID_PARAMETER;

	if (strcmp(string, "HEAP_BINARY") == 0)
		*outType = HEAP_BINARY;
	else if (strcmp(string, "HEAP_BINOMIAL") == 0)
		*outType = HEAP_BINOMIAL;
	else if (strcmp(string, "HEAP_FIBONACCI") == 0)
		*outType = HEAP_FIBONACCI;
	else if (strcmp(string, "HEAP_LEFTIST") == 0)
		*outType = HEAP_LEFTIST;
	else if (strcmp(string, "HEAP_SKEW") == 0)
		*outType = HEAP_SKEW;
	else if (strcmp(string, "HEAP_TREAP") == 0)
		*outType = HEAP_TREAP;
	else {
		return ERROR_MODEL_UNKNOWN_HEAP_TYPE;
	}

	return 0;
}

error_t storage_type_from_string(const char* string, storage_type_t* outType) {
	if (!string || !outType) return ERROR_INVALID_PARAMETER;

	if (strcmp(string, "STORAGE_BST") == 0)
		*outType = STORAGE_BST;
	else if (strcmp(string, "STORAGE_DYNAMIC_ARRAY") == 0)
		*outType = STORAGE_DYNAMIC_ARRAY;
	else if (strcmp(string, "STORAGE_HASHTABLE") == 0)
		*outType = STORAGE_HASHTABLE;
	else if (strcmp(string, "STORAGE_TRIE") == 0)
		*outType = STORAGE_TRIE;
	else {
		return ERROR_MODEL_UNKNOWN_STORAGE_TYPE;
	}

	return 0;
}

model_t model_create() {
	return (model_t){.departments = NULL, .departmentMap = NULL};
}

void model_destroy(model_t* model) {
	if (!model) return;

	if (model->departments) {
		for (size_t i = 0; i != model->departmentCount; ++i) {
			department_destroy(&model->departments[i]);
		}
		free(model->departments);
		model->departments = NULL;
	}

	storage_destroy(model->departmentMap);
	model->departmentMap = NULL;
}

error_t model_read_fail(error_t retval, char* line) {
	free(line);
	return retval;
}

error_t model_from_file(FILE* stream, model_t* out) {
	if (!out) return ERROR_INVALID_PARAMETER;

	error_t error;

	*out = model_create();

	// Parsing state.
	read_state_t state = READ_REQUEST_HEAP_TYPE;
	// Current line being read.
	char* line = NULL;
	// Capacity of the line buffer.
	size_t cap;

	while (getline(&line, &cap, stream) > 0) {
		char* newline = strrchr(line, '\n');
		if (newline) *newline = '\0';

		switch (state) {
			case READ_REQUEST_HEAP_TYPE: {
				error = heap_type_from_string(line, &out->requestHeapType);
				if (error) return model_read_fail(error, line);

				state = READ_DEPT_STORAGE_TYPE;
				break;
			}
			case READ_DEPT_STORAGE_TYPE: {
				error = storage_type_from_string(line, &out->deptStorageType);
				if (error) return model_read_fail(error, line);

				state = READ_START_TIME;
				break;
			}
			case READ_START_TIME: {
				struct tm tm;
				tm.tm_isdst = -1;

				char* pend = strptime(line, "%Y-%m-%d %H:%M:%S", &tm);
				if (!pend || *pend != '\0' || !tm_validate(tm)) {
					return model_read_fail(ERROR_MODEL_INVALID_START_TIME,
					                       line);
				}

				out->startTime = mktime(&tm);

				state = READ_END_TIME;
				break;
			}
			case READ_END_TIME: {
				struct tm tm;
				tm.tm_isdst = -1;

				char* pend = strptime(line, "%Y-%m-%d %H:%M:%S", &tm);
				if (!pend || *pend != '\0' || !tm_validate(tm)) {
					return model_read_fail(ERROR_MODEL_INVALID_END_TIME, line);
				}

				out->endTime = mktime(&tm);

				if (difftime(out->startTime, out->endTime) >= 0) {
					// |endTime| must be greater than |startTime|
					return model_read_fail(ERROR_MODEL_INVALID_END_TIME, line);
				}

				state = READ_MIN_PROCESS_TIME;
				break;
			}
			case READ_MIN_PROCESS_TIME: {
				unsigned long minProcessTime;

				error = str_to_ulong(line, &minProcessTime);
				if (error || minProcessTime < 1) {
					return model_read_fail(ERROR_MODEL_INVALID_MIN_PROCESS_TIME,
					                       line);
				}

				out->minProcessTime = minProcessTime;

				state = READ_MAX_PROCESS_TIME;
				break;
			}
			case READ_MAX_PROCESS_TIME: {
				unsigned long maxProcessTime;

				error = str_to_ulong(line, &maxProcessTime);
				if (error || maxProcessTime < 1) {
					return model_read_fail(ERROR_MODEL_INVALID_MAX_PROCESS_TIME,
					                       line);
				}

				out->maxProcessTime = maxProcessTime;

				if (out->minProcessTime >= out->maxProcessTime) {
					// |maxProcessTime| must be greater than |minProcessTime|
					return model_read_fail(ERROR_MODEL_INVALID_MAX_PROCESS_TIME,
					                       line);
				}

				state = READ_DEPARTMENT_COUNT;
				break;
			}
			case READ_DEPARTMENT_COUNT: {
				size_t departmentCount;

				error = str_to_ulong(line, &departmentCount);
				if (error || departmentCount < 1 ||
				    departmentCount > MODEL_MAX_DEPARTMENTS) {
					return model_read_fail(ERROR_MODEL_INVALID_DEPARTMENT_COUNT,
					                       line);
				}

				out->departmentCount = departmentCount;

				state = READ_OPERATOR_COUNT;
				break;
			}
			case READ_OPERATOR_COUNT: {
				char* tokenizingString = line;
				char* token;
				size_t i = 0;

				while (i < out->departmentCount &&
				       (token = strsep(&tokenizingString, " "))) {
					size_t operatorCount;

					error = str_to_ulong(token, &operatorCount);
					if (error || operatorCount < 1 ||
					    operatorCount > MODEL_MAX_OPERATORS) {
						return model_read_fail(
						    ERROR_MODEL_INVALID_OPERATOR_COUNT, line);
					}

					out->operatorCount[i] = operatorCount;

					++i;
				}

				if (i != out->departmentCount || tokenizingString) {
					// Not enough or too many counts.
					return model_read_fail(ERROR_MODEL_INVALID_OPERATOR_COUNT,
					                       line);
				}

				state = READ_OVERLOAD_FACTOR;
				break;
			}
			case READ_OVERLOAD_FACTOR: {
				char* tokenizingString = line;
				char* token;
				size_t i = 0;

				while (i < out->departmentCount &&
				       (token = strsep(&tokenizingString, " "))) {
					double overloadFactor;

					error = str_to_double(token, &overloadFactor);
					if (error || overloadFactor < 1.0) {
						return model_read_fail(
						    ERROR_MODEL_INVALID_OVERLOAD_FACTOR, line);
					}

					out->overloadFactor[i] = (float)overloadFactor;

					++i;
				}

				if (i != out->departmentCount || tokenizingString) {
					// Not enough or too many counts.
					return model_read_fail(ERROR_MODEL_INVALID_OVERLOAD_FACTOR,
					                       line);
				}

				state = READ_VALID;
				break;
			}
			case READ_VALID: {
				return model_read_fail(ERROR_UNEXPECTED_TOKEN, line);
			}
		}
	}

	if (ferror(stream)) {
		return model_read_fail(ERROR_IO, line);
	}
	if (state != READ_VALID) {
		return model_read_fail(ERROR_UNEXPECTED_TOKEN, line);
	}

	free(line);
	return 0;
}

error_t model_init_fail(error_t error, model_t* model) {
	model_destroy(model);
	return error;
}

error_t model_init(model_t* model) {
	if (!model) return ERROR_INVALID_PARAMETER;
	if (model->departments) return ERROR_MODEL_ALREADY_INITIALIZED;

	model->departments = NULL;
	model->departmentMap = NULL;

	model->departments = calloc(model->departmentCount, sizeof(department_t));
	if (!model->departments) {
		return model_init_fail(ERROR_OUT_OF_MEMORY, model);
	}

	model->departmentMap = storage_create(model->deptStorageType);
	if (!model->departmentMap) {
		return model_init_fail(ERROR_OUT_OF_MEMORY, model);
	}

	for (size_t i = 0; i != model->departmentCount; ++i) {
		char id[128];
		snprintf(id, sizeof(id), "D%zu", i);

		department_t* dept = &model->departments[i];

		dept->id = strdup(id);
		if (!dept->id) {
			return model_init_fail(ERROR_OUT_OF_MEMORY, model);
		}

		dept->requestQueue = heap_create(model->requestHeapType);
		if (!dept->requestQueue) {
			return model_init_fail(ERROR_OUT_OF_MEMORY, model);
		}

		dept->overloadFactor = model->overloadFactor[i];
		dept->requestsInQueue = 0;

		dept->operators = vector_oper_create();

		for (size_t j = 0; j != model->operatorCount[i]; ++j) {
			size_t length = 16;

			// For whatever reason, without this `rand()` always produces the
			// same values for me.
			srand(time(NULL) + i + j);

			// Generate a random 16-character "name"
			char* name = (char*)malloc(sizeof(char) * (length + 1));
			if (!name) {
				return model_init_fail(ERROR_OUT_OF_MEMORY, model);
			}

			for (size_t k = 0; k != length; ++k) {
				name[k] = (char)(rand() % 2 ? mth_rand('a', 'z')
				                            : mth_rand('A', 'Z'));
			}

			name[length] = '\0';

			oper_t oper = {.name = name, .request = NULL, .remainingTime = 0};
			if (!vector_oper_push_back(&dept->operators, oper)) {
				free(name);
				return model_init_fail(ERROR_OUT_OF_MEMORY, model);
			}
		}

		if (storage_put(model->departmentMap, id, dept)) {
			return model_init_fail(ERROR_OUT_OF_MEMORY, model);
		}
	}

	return 0;
}

void model_log(FILE* logFile, time_t modelTime, event_t eventId,
               const char* fmt, ...) {
	// Format message.
	va_list args;
	va_start(args, fmt);

	char msg[256];
	vsnprintf(msg, sizeof(msg), fmt, args);

	va_end(args);

	// Format local time.
	struct tm* tm = localtime(&modelTime);
	if (tm == NULL) return;

	tm->tm_isdst = -1;

	char timeIso8601[128];
	strftime(timeIso8601, sizeof(timeIso8601), "%Y-%m-%d %H:%M:%S", tm);

	const char* event;

	switch (eventId) {
		case NEW_REQUEST:
			event = "NEW_REQUEST";
			break;
		case REQUEST_HANDLING_STARTED:
			event = "REQUEST_HANDLING_STARTED";
			break;
		case REQUEST_HANDLING_FINISHED:
			event = "REQUEST_HANDLING_FINISHED";
			break;
		case DEPARTMENT_OVERLOADED:
			event = "DEPARTMENT_OVERLOADED";
			break;
	}

	fprintf(logFile, "[%s] [%s]: %s\n", timeIso8601, event, msg);
	fflush(logFile);

	if (ferror(logFile)) {
		fprintf(stderr, "[WARN] model_log: can't write log message\n");
	}
}

error_t model_assign_requests(model_t* model, FILE* logFile) {
	if (!model || !logFile) return ERROR_INVALID_PARAMETER;

	error_t error;

	for (size_t i = 0; i != model->departmentCount; ++i) {
		department_t* dept = &model->departments[i];

		if (dept->requestsInQueue == 0) continue;

		// Find an operator which isn't currently working and assign the most
		// top-priority request to it.
		oper_t* assignTo = NULL;

		for (size_t j = 0; j != vector_oper_size(&dept->operators); ++j) {
			oper_t* oper = vector_oper_get(&dept->operators, j);

			if (!oper->request) {
				assignTo = oper;
				break;
			}
		}

		if (!assignTo) continue;

		request_t* request;

		error = heap_pop_max(dept->requestQueue, &request);
		if (error) return error;

		--dept->requestsInQueue;

		assignTo->request = request;
		assignTo->remainingTime = request->requiredTime;

		model_log(logFile, model->time, REQUEST_HANDLING_STARTED,
		          "request id=%lu, assigned to %s at %s", request->id,
		          assignTo->name, dept->id);
	}

	return 0;
}

error_t model_move_requests(model_t* model, request_t* causingRequest,
                            department_t* overloadedDept, FILE* logFile) {
	if (!model || !overloadedDept || !logFile) return ERROR_INVALID_PARAMETER;

	department_t* moveTo = NULL;
	double minLoad = INFINITY;

	for (size_t i = 0; i != model->departmentCount; ++i) {
		department_t* dept 	= &model->departments[i];

		if (dept == overloadedDept || department_is_overloaded(dept)) {
			continue;
		}

		double load = department_load(dept);

		if (!moveTo || load < minLoad) {
			moveTo = dept;
			minLoad = load;
		}
	}

	if (moveTo) {
		error_t error =
		    heap_meld(overloadedDept->requestQueue, moveTo->requestQueue);
		if (error) return error;

		moveTo->requestsInQueue += overloadedDept->requestsInQueue;
		overloadedDept->requestsInQueue = 0;

		model_log(logFile, model->time, DEPARTMENT_OVERLOADED,
		          "request id=%lu, moved to department %s", causingRequest->id,
		          moveTo->id);
	} else {
		model_log(logFile, model->time, DEPARTMENT_OVERLOADED,
		          "request id=%lu, cannot move, all departments are overloaded",
		          causingRequest->id);
	}

	return 0;
}

error_t model_tick_departments(model_t* model, FILE* logFile) {
	for (size_t i = 0; i != model->departmentCount; ++i) {
		department_t* dept = &model->departments[i];

		for (size_t j = 0; j != vector_oper_size(&dept->operators); ++j) {
			oper_t* oper = vector_oper_get(&dept->operators, j);

			if (!oper->request) continue;

			--oper->remainingTime;

			if (oper->remainingTime == 0) {
				request_t* req = oper->request;

				model_log(logFile, model->time, REQUEST_HANDLING_FINISHED,
				          "request id=%lu, completed in %u mins by operator %s",
				          req->id, req->requiredTime, oper->name);

				oper->request = NULL;
			}
		}
	}

	return 0;
}

error_t model_run_clean(error_t error, vector_request_t* offloadedRequests) {
	for (size_t i = 0; i != vector_request_size(offloadedRequests); ++i) {
		request_t* request = vector_request_get(offloadedRequests, i);
		request_destroy(request);
	}
	vector_request_destroy(offloadedRequests);

	return error;
}

error_t model_run(model_t* model, deque_request_t* requests, FILE* logFile) {
	if (!model || !requests || !logFile) return ERROR_INVALID_PARAMETER;

	error_t error;
	model->time = model->startTime;

	// When popping a request from the queue, it is returned as a value. It is
	// then inserted into the respective department's queue by reference.
	// Because its scope ends right after, a use-after-scope occurs, which is
	// why this temporary storage exists.
	vector_request_t offloadedRequests = vector_request_create();

	// Don't want to accidentally resize it.
	if (!vector_request_ensure_capacity(&offloadedRequests,
	                                    deque_request_size(requests))) {
		return model_run_clean(ERROR_OUT_OF_MEMORY, &offloadedRequests);
	}

	while (difftime(model->time, model->endTime) <= 0) {
		const request_t* head = deque_request_peek_front(requests);

		// Popped request from the queue.
		request_t* request;
		// Department this request was moved to.
		department_t* dept = NULL;

		if (head && difftime(model->time, head->time) >= 0) {
			// Insert request into department.
			request_t temp;
			deque_request_pop_front(requests, &temp);

			if (!vector_request_push_back(&offloadedRequests, temp)) {
				return model_run_clean(ERROR_OUT_OF_MEMORY, &offloadedRequests);
			}

			request =
			    vector_request_get(&offloadedRequests,
			                       vector_request_size(&offloadedRequests) - 1);

			request->requiredTime =
			    mth_rand(model->minProcessTime, model->maxProcessTime);

			dept = storage_get(model->departmentMap, request->departmentId);
			if (!dept) {
				return model_run_clean(ERROR_MODEL_UNKNOWN_DEPARTMENT,
				                       &offloadedRequests);
			}

			error = heap_insert(dept->requestQueue, request);
			if (error) {
				return model_run_clean(error, &offloadedRequests);
			}

			++dept->requestsInQueue;

			model_log(logFile, model->time, NEW_REQUEST,
			          "request id=%lu, department=%s", request->id,
			          request->departmentId);
		}

		// Distribute requests in queue to available operators.
		error = model_assign_requests(model, logFile);
		if (error) {
			return error;
		}

		// If this request caused the department to become overloaded, move all
		// requests from this department to another that is not overloaded.
		if (dept && department_is_overloaded(dept)) {
			error = model_move_requests(model, request, dept, logFile);
			if (error) {
				return error;
			}
		}

		// Update request progress on all departments.
		error = model_tick_departments(model, logFile);
		if (error) return error;

		// Advance time by 1 minute.
		struct tm* tm = localtime(&model->time);
		tm->tm_min += 1;
		model->time = mktime(tm);
	}

	return model_run_clean(0, &offloadedRequests);
}

const char* model_error_to_string(error_t error) {
	switch (error) {
		case ERROR_MODEL_UNKNOWN_HEAP_TYPE:
			return "Invalid model settings: unknown heap type";
		case ERROR_MODEL_UNKNOWN_STORAGE_TYPE:
			return "Invalid model settings: unknown storage type";
		case ERROR_MODEL_INVALID_START_TIME:
			return "Invalid model settings: malformed start time";
		case ERROR_MODEL_INVALID_END_TIME:
			return "Invalid model settings: malformed end time";
		case ERROR_MODEL_INVALID_MIN_PROCESS_TIME:
			return "Invalid model settings: malformed min process time";
		case ERROR_MODEL_INVALID_MAX_PROCESS_TIME:
			return "Invalid model settings: malformed max process time";
		case ERROR_MODEL_INVALID_DEPARTMENT_COUNT:
			return "Invalid model settings: malformed department count";
		case ERROR_MODEL_INVALID_OPERATOR_COUNT:
			return "Invalid model settings: malformed department operator "
			       "count";
		case ERROR_MODEL_INVALID_OVERLOAD_FACTOR:
			return "Invalid model settings: malformed department overload "
			       "factor";
		case ERROR_MODEL_ALREADY_INITIALIZED:
			return "Model can't be initialized twice";
		case ERROR_MODEL_UNKNOWN_DEPARTMENT:
			return "Invalid request: unknown department";
		default:
			return NULL;
	}
}
