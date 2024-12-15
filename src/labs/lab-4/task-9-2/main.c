#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "lib/collections/vector.h"
#include "lib/convert.h"
#include "lib/utils.h"

typedef enum state {
	PROMPT_HEAP_TYPE,
	PROMPT_STORAGE_TYPE,
	PROMPT_START_TIME,
	PROMPT_END_TIME,
	PROMPT_MIN_PROCESS_TIME,
	PROMPT_MAX_PROCESS_TIME,
	PROMPT_DEPARTMENT_COUNT,
	PROMPT_DEPARTMENT,
	PROMPT_DONE
} state_t;

typedef struct dept_info {
	size_t operatorCount;
	double overloadFactor;
} dept_info_t;

DEFINE_VECTOR(vector_dept_info_t, dept_info_t, dept_info)
IMPL_VECTOR(vector_dept_info_t, dept_info_t, dept_info, {NULL})

int cleanup(int exitCode, FILE* settingsFile, char* line,
            vector_dept_info_t* deptInfo) {
	fclose(settingsFile);
	free(line);
	vector_dept_info_destroy(deptInfo);

	return exitCode;
}

int main(int argc, char* argv[]) {
	if (argc != 2) {
		fprintf(stderr, "usage: %s <settings file>", argv[0]);
		return 1;
	}

	FILE* settingsFile = fopen(argv[1], "w");
	if (!settingsFile) {
		fprintf(stderr, "Can't open settings file for writing.\n");
		return 1;
	}

	state_t state = PROMPT_HEAP_TYPE;

	// Count of departments prompted.
	size_t deptCount;
	// Info about departments (operator count & overload factor).
	vector_dept_info_t deptInfo = vector_dept_info_create();

	// Start time for validation.
	time_t startTime;
	// Min processing time for validation.
	unsigned long minProcessTime;

	char* line = NULL;
	size_t capacity;

	while (state != PROMPT_DONE) {
		switch (state) {
			case PROMPT_HEAP_TYPE: {
				printf(
				    "Enter heap type ('binary', 'binomial', 'fibonacci', "
				    "'leftist', 'skew', 'treap'): \n");

				if (getline(&line, &capacity, stdin) <= 0) {
					return cleanup(1, settingsFile, line, &deptInfo);
				}

				char* newline = strrchr(line, '\n');
				if (newline) *newline = '\0';

				if (strcmp("binary", line) == 0)
					fprintf(settingsFile, "HEAP_BINARY\n");
				else if (strcmp("binomial", line) == 0)
					fprintf(settingsFile, "HEAP_BINOMIAL\n");
				else if (strcmp("fibonacci", line) == 0)
					fprintf(settingsFile, "HEAP_FIBONACCI\n");
				else if (strcmp("leftist", line) == 0)
					fprintf(settingsFile, "HEAP_LEFTIST\n");
				else if (strcmp("skew", line) == 0)
					fprintf(settingsFile, "HEAP_SKEW\n");
				else if (strcmp("treap", line) == 0)
					fprintf(settingsFile, "HEAP_TREAP\n");
				else {
					printf("Invalid heap type. Try again.\n");
					break;
				}

				if (ferror(settingsFile)) {
					return cleanup(2, settingsFile, line, &deptInfo);
				}

				state = PROMPT_STORAGE_TYPE;
				break;
			}
			case PROMPT_STORAGE_TYPE: {
				printf(
				    "Enter storage type ('bst', 'dynamic_array', 'hashtable', "
				    "'trie'): \n");

				if (getline(&line, &capacity, stdin) <= 0) {
					return cleanup(1, settingsFile, line, &deptInfo);
				}

				char* newline = strrchr(line, '\n');
				if (newline) *newline = '\0';

				if (strcmp("bst", line) == 0)
					fprintf(settingsFile, "STORAGE_BST\n");
				else if (strcmp("dynamic_array", line) == 0)
					fprintf(settingsFile, "STORAGE_DYNAMIC_ARRAY\n");
				else if (strcmp("hashtable", line) == 0)
					fprintf(settingsFile, "STORAGE_HASHTABLE\n");
				else if (strcmp("trie", line) == 0)
					fprintf(settingsFile, "STORAGE_TRIE\n");
				else {
					printf("Invalid storage type. Try again.\n");
					break;
				}

				if (ferror(settingsFile)) {
					return cleanup(2, settingsFile, line, &deptInfo);
				}

				state = PROMPT_START_TIME;
				break;
			}
			case PROMPT_START_TIME: {
				printf("Enter start time (yyyy-MM-dd HH:mm:ss): \n");

				if (getline(&line, &capacity, stdin) <= 0) {
					return cleanup(1, settingsFile, line, &deptInfo);
				}

				char* newline = strrchr(line, '\n');
				if (newline) *newline = '\0';

				struct tm tm;
				tm.tm_isdst = -1;

				char* pend = strptime(line, "%Y-%m-%d %H:%M:%S", &tm);
				if (!pend || *pend != '\0' || !tm_validate(tm)) {
					printf("Invalid start time. Try again.\n");
					break;
				}

				fprintf(settingsFile, "%s\n", line);

				if (ferror(settingsFile)) {
					return cleanup(2, settingsFile, line, &deptInfo);
				}

				startTime = mktime(&tm);

				state = PROMPT_END_TIME;
				break;
			}
			case PROMPT_END_TIME: {
				printf("Enter end time (yyyy-MM-dd HH:mm:ss): \n");

				if (getline(&line, &capacity, stdin) <= 0) {
					return cleanup(1, settingsFile, line, &deptInfo);
				}

				char* newline = strrchr(line, '\n');
				if (newline) *newline = '\0';

				struct tm tm;
				tm.tm_isdst = -1;

				char* pend = strptime(line, "%Y-%m-%d %H:%M:%S", &tm);
				if (!pend || *pend != '\0' || !tm_validate(tm)) {
					printf("Invalid end time. Try again.\n");
					break;
				}

				time_t endTime = mktime(&tm);
				if (difftime(endTime, startTime) < 0) {
					printf("Invalid end time. Try again.\n");
					break;
				}

				fprintf(settingsFile, "%s\n", line);

				if (ferror(settingsFile)) {
					return cleanup(2, settingsFile, line, &deptInfo);
				}

				state = PROMPT_MIN_PROCESS_TIME;
				break;
			}
			case PROMPT_MIN_PROCESS_TIME: {
				printf("Enter minimum processing time: \n");

				if (getline(&line, &capacity, stdin) <= 0) {
					return cleanup(1, settingsFile, line, &deptInfo);
				}

				char* newline = strrchr(line, '\n');
				if (newline) *newline = '\0';

				if (str_to_ulong(line, &minProcessTime) || minProcessTime < 1) {
					printf("Invalid minimum processing time. Try again.\n");
					break;
				}

				fprintf(settingsFile, "%lu\n", minProcessTime);

				if (ferror(settingsFile)) {
					return cleanup(2, settingsFile, line, &deptInfo);
				}

				state = PROMPT_MAX_PROCESS_TIME;
				break;
			}
			case PROMPT_MAX_PROCESS_TIME: {
				printf("Enter maximum processing time: \n");

				if (getline(&line, &capacity, stdin) <= 0) {
					return cleanup(1, settingsFile, line, &deptInfo);
				}

				char* newline = strrchr(line, '\n');
				if (newline) *newline = '\0';

				unsigned long maxProcessTime;

				if (str_to_ulong(line, &maxProcessTime) || maxProcessTime < 1 ||
				    maxProcessTime < minProcessTime) {
					printf("Invalid maximum processing time. Try again.\n");
					break;
				}

				fprintf(settingsFile, "%lu\n", maxProcessTime);

				if (ferror(settingsFile)) {
					return cleanup(2, settingsFile, line, &deptInfo);
				}

				state = PROMPT_DEPARTMENT_COUNT;
				break;
			}
			case PROMPT_DEPARTMENT_COUNT: {
				printf("Enter department count: \n");

				if (getline(&line, &capacity, stdin) <= 0) {
					return cleanup(1, settingsFile, line, &deptInfo);
				}

				char* newline = strrchr(line, '\n');
				if (newline) *newline = '\0';

				if (str_to_ulong(line, &deptCount) || deptCount < 1 ||
				    deptCount > 100) {
					printf("Invalid department count. Try again.\n");
					break;
				}

				fprintf(settingsFile, "%lu\n", deptCount);

				if (ferror(settingsFile)) {
					return cleanup(2, settingsFile, line, &deptInfo);
				}
				if (!vector_dept_info_ensure_capacity(&deptInfo, deptCount)) {
					return cleanup(3, settingsFile, line, &deptInfo);
				}

				state = PROMPT_DEPARTMENT;
				break;
			}
			case PROMPT_DEPARTMENT: {
				if (vector_dept_info_size(&deptInfo) == deptCount) {
					state = PROMPT_DONE;
					break;
				}

				printf(
				    "Enter department info (<operator count> <overload "
				    "factor>):\n");

				if (getline(&line, &capacity, stdin) <= 0) {
					return cleanup(1, settingsFile, line, &deptInfo);
				}

				char* newline = strrchr(line, '\n');
				if (newline) *newline = '\0';

				dept_info_t info;

				char* tokenizingString = line;
				char* token = strsep(&tokenizingString, " ");

				if (!token || str_to_ulong(token, &info.operatorCount)) {
					printf("Invalid department info. Try again.\n");
					break;
				}

				token = strsep(&tokenizingString, " ");
				if (!token || str_to_double(token, &info.overloadFactor)) {
					printf("Invalid department info. Try again.\n");
					break;
				}

				// Extra text at the end of the string.
				if (tokenizingString) {
					printf("Invalid department info. Try again.\n");
					break;
				}

				if (!vector_dept_info_push_back(&deptInfo, info)) {
					return cleanup(3, settingsFile, line, &deptInfo);
				}

				break;
			}
		}
	}

	size_t size = vector_dept_info_size(&deptInfo);

	for (size_t i = 0; i != size; ++i) {
		dept_info_t* info = vector_dept_info_get(&deptInfo, i);

		fprintf(settingsFile, "%lu", info->operatorCount);

		if (i + 1 < size) {
			fputc(' ', settingsFile);
		}

		if (ferror(settingsFile)) {
			return cleanup(2, settingsFile, line, &deptInfo);
		}
	}

	fputc('\n', settingsFile);

	for (size_t i = 0; i != size; ++i) {
		dept_info_t* info = vector_dept_info_get(&deptInfo, i);

		fprintf(settingsFile, "%lf", info->overloadFactor);

		if (i + 1 < size) {
			fputc(' ', settingsFile);
		}

		if (ferror(settingsFile)) {
			return cleanup(2, settingsFile, line, &deptInfo);
		}
	}

	printf("Settings file created.\n");
	return cleanup(0, settingsFile, line, &deptInfo);
}
