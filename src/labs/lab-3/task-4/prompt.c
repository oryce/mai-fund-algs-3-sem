#include "prompt.h"

#include <limits.h>
#include <stdio.h>

#include "datetime.h"
#include "lib/convert.h"
#include "string_ex.h"

static bool read_string_cleanup(char* line, string_t* output) {
	fprintf(stderr, CATASTROPHIC_ERROR);

	free(line);
	string_destroy(output);

	return false;
}

bool prompt_string(string_t* output) {
	if (string_created(output)) {
		string_destroy(output);
	}

	char* line = NULL;
	size_t lineCap;

	bool failed = getline(&line, &lineCap, stdin) <= 0;
	if (failed) return read_string_cleanup(line, output);

	char* newline = strrchr(line, '\n');
	if (newline) *newline = '\0';

	failed = !string_from_c_str(output, line);
	if (failed) return read_string_cleanup(line, output);

	free(line);
	return true;
}

static bool prompt_while(bool condition(char* line, void* output),
                         void* output) {
	char* line = NULL;
	size_t lineCap;

	while (getline(&line, &lineCap, stdin) > 0) {
		char* newline = strrchr(line, '\n');
		if (newline) *newline = '\0';

		if (condition(line, output)) {
			fprintf(stderr,
			        "Looks like you made a mistake. Please try again.\n");
		} else {
			free(line);
			return true;
		}
	}

	// getline returned EOF or failed to allocate something.
	fprintf(stderr, CATASTROPHIC_ERROR);

	free(line);
	return false;
}

static bool prompt_uint_condition(char* line, void* output) {
	unsigned long* outLong = (unsigned long*)output;
	return str_to_ulong(line, outLong) || *outLong > UINT_MAX;
}

static bool prompt_double_condition(char* line, void* output) {
	double* outDouble = (double*)output;
	return str_to_double(line, outDouble) || *outDouble < 0;
}

bool prompt_uint(unsigned* output) {
	unsigned long outLong;

	if (!prompt_while(&prompt_uint_condition, &outLong)) {
		return false;
	}

	*output = (unsigned)outLong;
	return true;
}

bool prompt_double(double* output) {
	return prompt_while(&prompt_double_condition, output);
}

static bool read_datetime_condition(char* line, void* output) {
	parsed_dt_t* outDt = (parsed_dt_t*)output;

	if (!datetime_parse(line, &outDt->datetime)) {
		return true;
	}
	// if this fails, we're fucked, so we end the loop and check for this
	// state later.
	if (!string_from_c_str(&outDt->source, line)) {
		string_destroy(&outDt->source);
	}

	return false;
}

bool prompt_datetime(parsed_dt_t* output) {
	return prompt_while(&read_datetime_condition, output) &&
	       string_created(&output->source);
}

bool prompt_address(address_t* output) {
	printf("City: ");
	if (!prompt_string(&output->city)) return false;
	printf("Street: ");
	if (!prompt_string(&output->street)) return false;
	printf("House number: ");
	if (!prompt_uint(&output->house)) return false;
	printf("Housing: ");
	if (!prompt_string(&output->housing)) return false;
	printf("Apartment: ");
	if (!prompt_uint(&output->apartment)) return false;

	// prompt continuously until the input is correct.
	do {
		printf("Zip code (6 chars): ");
		if (!prompt_string(&output->zip_code)) return false;
	} while (string_length(&output->zip_code) != 6);

	return true;
}

bool prompt_mail_id(string_t* output) {
	do {
		printf("ID (14 chars): ");
		if (!prompt_string(output)) return false;
	} while (string_length(output) != 14);

	return true;
}

bool prompt_mail(mail_t* output) {
	printf("Please fill in the recipient's address.\n");
	if (!prompt_address(&output->address)) return false;
	printf("Weight: ");
	if (!prompt_double(&output->weight)) return false;

	if (!prompt_mail_id(&output->id)) return false;

	printf("Creation datetime: ");
	if (!prompt_datetime(&output->creation_date)) return false;
	printf("Delivery datetime: ");
	if (!prompt_datetime(&output->delivery_date)) return false;

	return true;
}

bool prompt_command(char** line) {
	printf("Please input a command ('h' for help).\n");

	size_t lineCap;
	return getline(line, &lineCap, stdin) > 0;
}
