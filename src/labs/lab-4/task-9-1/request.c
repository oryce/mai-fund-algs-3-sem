#include "request.h"

#include "lib/convert.h"
#include "lib/mth.h"
#include "lib/utils.h"

typedef enum read_state {
	READ_PRIORITY,
	READ_DEPARTMENT_ID,
	READ_TEXT,
} read_state_t;

int request_time_cmp(const void* p1, const void* p2) {
	if (!p1 || !p2 || p1 == p2) return 0;

	const request_t* a = (const request_t*)p1;
	const request_t* b = (const request_t*)p2;

	return mth_sign_double(difftime(a->time, b->time));
}

IMPL_DEQUE(deque_request_t, request_t, request)
IMPL_VECTOR(vector_request_t, request_t, request, {&request_time_cmp})

void request_destroy(request_t* request) {
	if (!request) return;

	free(request->departmentId);
	request->departmentId = NULL;
	free(request->text);
	request->text = NULL;
}

error_t request_read_fail(error_t error, char* line, request_t* out) {
	free(line);

	request_destroy(out);

	return error;
}

error_t request_from_string(const char* string, request_t* out,
                            unsigned maxPriority) {
	if (!out) return ERROR_INVALID_PARAMETER;

	error_t error;

	out->text = NULL;
	out->departmentId = NULL;

	char* line = strdup(string);
	if (!line) return ERROR_OUT_OF_MEMORY;

	// Find a second space to terminate the datetime.
	char* firstSpace = NULL;
	char* secondSpace = NULL;

	for (char* p = line; *p; ++p) {
		if (*p == ' ') {
			if (!firstSpace) {
				firstSpace = p;
			} else {
				secondSpace = p;
				break;
			}
		}
	}

	if (!secondSpace) {
		return request_read_fail(ERROR_REQUEST_INVALID_TIME, line, out);
	}

	// Read the datetime.
	*secondSpace = '\0';

	struct tm tm;
	tm.tm_isdst = -1;

	char* pend = strptime(line, "%Y-%m-%d %H:%M:%S", &tm);
	if (!pend || *pend != '\0' || !tm_validate(tm)) {
		return request_read_fail(ERROR_REQUEST_INVALID_TIME, line, out);
	}

	out->time = mktime(&tm);

	// Read the rest of the data from here, tokenizing the string.
	char* readPtr = secondSpace + 1;
	char* token;
	read_state_t state = READ_PRIORITY;

	while ((token = strsep(&readPtr, " "))) {
		switch (state) {
			case READ_PRIORITY: {
				unsigned long priority;

				error = str_to_ulong(token, &priority);
				if (error || priority > maxPriority) {
					return request_read_fail(ERROR_REQUEST_INVALID_PRIORITY,
					                         line, out);
				}

				out->priority = priority;

				state = READ_DEPARTMENT_ID;
				break;
			}
			case READ_DEPARTMENT_ID: {
				out->departmentId = strdup(token);
				if (!out->departmentId) {
					return request_read_fail(ERROR_OUT_OF_MEMORY, line, out);
				}

				state = READ_TEXT;
				break;
			}
		}

		if (state == READ_TEXT) {
			break;
		}
	}

	if (state != READ_TEXT || !readPtr || *readPtr != '"') {
		return request_read_fail(ERROR_UNEXPECTED_TOKEN, line, out);
	}

	// Skip the first quote.
	++readPtr;

	// Find the last quote, check that the input string ends after it, and
	// terminate the token.
	char* textEnd = strrchr(readPtr, '"');
	if (!textEnd || *(textEnd + 1) != '\0') {
		return request_read_fail(ERROR_UNEXPECTED_TOKEN, line, out);
	}

	*textEnd = '\0';

	// Copy text to request.
	out->text = strdup(readPtr);
	if (!out->text) {
		return request_read_fail(ERROR_OUT_OF_MEMORY, line, out);
	}

	free(line);
	return 0;
}

error_t request_files_cleanup(error_t error, vector_request_t* temp,
                              char* line) {
	for (size_t i = 0; i != vector_request_size(temp); ++i) {
		request_t* request = vector_request_get(temp, i);
		request_destroy(request);
	}

	vector_request_destroy(temp);

	free(line);

	return error;
}

error_t request_from_files(FILE* files[], size_t nFiles, deque_request_t* out,
                           unsigned maxPriority) {
	if (!files || !nFiles || !out) return ERROR_INVALID_PARAMETER;

	*out = deque_request_create();

	// Temporary vector to contain unsorted requests.
	vector_request_t temp = vector_request_create();
	unsigned long idSequence = 0;

	for (size_t i = 0; i != nFiles; ++i) {
		char* line = NULL;
		size_t cap;

		while (getline(&line, &cap, files[i]) > 0) {
			char* newline = strrchr(line, '\n');
			if (newline) *newline = '\0';

			request_t request;

			// Read line from files[i].
			error_t error = request_from_string(line, &request, maxPriority);
			if (error) return request_files_cleanup(error, &temp, line);

			// Assign a sequential ID.
			request.id = idSequence;
			++idSequence;

			// Parse the line into a request.
			if (!vector_request_push_back(&temp, request)) {
				request_destroy(&request);
				return request_files_cleanup(ERROR_OUT_OF_MEMORY, &temp, line);
			}
		}

		if (ferror(files[i])) {
			return request_files_cleanup(ERROR_IO, &temp, line);
		}

		free(line);
	}

	vector_request_sort(&temp);

	for (size_t i = 0; i != vector_request_size(&temp); ++i) {
		request_t* request = vector_request_get(&temp, i);

		if (!deque_request_push_back(out, *request)) {
			deque_request_destroy(out);
			return request_files_cleanup(ERROR_OUT_OF_MEMORY, &temp, NULL);
		}
	}

	vector_request_destroy(&temp);
	return 0;
}

int request_priority_cmp(const request_t* a, const request_t* b) {
	if (!a || !b) return 0;

	int order = mth_sign_long((long)a->priority - (long)b->priority);
	if (order) return order;

	order = -mth_sign_double(difftime(a->time, b->time));
	if (order) return order;

	return 0;
}

const char* request_error_to_string(error_t error) {
	switch (error) {
		case ERROR_REQUEST_INVALID_TIME:
			return "Invalid request: invalid time";
		case ERROR_REQUEST_INVALID_PRIORITY:
			return "Invalid request: invalid priority";
		case ERROR_REQUEST_INVALID_TEXT:
			return "Invalid request: invalid text";
		default:
			return NULL;
	}
}
