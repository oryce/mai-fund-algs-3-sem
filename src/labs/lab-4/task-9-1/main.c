#include <stdio.h>

#include "heap.h"
#include "lib/convert.h"
#include "model.h"
#include "request.h"

void app_error_print(error_t error) {
	error_fmt_t fmt[] = {&heap_error_to_string, &model_error_to_string,
	                     &request_error_to_string};
	error_print_ex(error, fmt, sizeof(fmt) / sizeof(fmt[0]));
}

int cleanup(int exitCode, model_t* model, deque_request_t* requests,
            FILE* logFile) {
	while (!deque_request_is_empty(requests)) {
		request_t request;
		if (deque_request_pop_back(requests, &request)) {
			request_destroy(&request);
		}
	}

	model_destroy(model);
	deque_request_destroy(requests);
	fclose(logFile);

	return exitCode;
}

int main(int argc, char* argv[]) {
	error_t error;

	model_t model = {.departments = NULL, .departmentMap = NULL};
	deque_request_t requests = {.size = 0, .buffer = NULL};

	unsigned long maxPriority;

	FILE* logFile = NULL;

	if (argc < 4) {
		fprintf(stderr,
		        "usage: %s <settings file> <max priority> <request files...>",
		        argv[0]);
		return 1;
	}

	logFile = fopen("log.txt", "w");
	if (!logFile) {
		fprintf(stderr, "Can't open log.txt for writing.\n");
		return 10;
	}

	FILE* settingsFile = fopen(argv[1], "r");
	if (!settingsFile) {
		fprintf(stderr, "Can't open settings file for reading.\n");
		return 2;
	}

	error = model_from_file(settingsFile, &model);
	fclose(settingsFile);

	if (error) {
		app_error_print(error);
		return cleanup(3, &model, &requests, logFile);
	}

	error = model_init(&model);
	if (error) {
		app_error_print(error);
		return cleanup(4, &model, &requests, logFile);
	}

	error = str_to_ulong(argv[2], &maxPriority);
	if (error) {
		app_error_print(error);
		return cleanup(7, &model, &requests, logFile);
	}
	if (maxPriority > UINT32_MAX) {
		fprintf(stderr, "Max priority out of range.\n");
		return cleanup(8, &model, &requests, logFile);
	}

	size_t nRequestFiles = argc - 3;
	FILE** requestFiles = (FILE**)calloc(nRequestFiles, sizeof(FILE*));

	if (!requestFiles) {
		return cleanup(5, &model, &requests, logFile);
	}

	for (size_t i = 0; i != nRequestFiles; ++i) {
		requestFiles[i] = fopen(argv[i + 3], "r");

		if (!requestFiles[i]) {
			fprintf(stderr, "Can't open file %s for reading.\n", argv[i + 3]);

			for (size_t j = 0; j != nRequestFiles; ++j) {
				fclose(requestFiles[j]);
			}
			free(requestFiles);

			return cleanup(6, &model, &requests, logFile);
		}
	}

	error =
	    request_from_files(requestFiles, nRequestFiles, &requests, maxPriority);

	for (size_t j = 0; j != nRequestFiles; ++j) {
		fclose(requestFiles[j]);
	}
	free(requestFiles);

	if (error) {
		app_error_print(error);
		return cleanup(9, &model, &requests, logFile);
	}

	printf("Initialized!!!\n");

	error = model_run(&model, &requests, logFile);
	if (error) {
		app_error_print(error);
		return cleanup(11, &model, &requests, logFile);
	}

	return cleanup(0, &model, &requests, logFile);
}
