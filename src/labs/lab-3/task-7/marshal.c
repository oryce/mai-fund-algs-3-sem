#include "marshal.h"

#include <time.h>

#include "lib/convert.h"

const char* marshal_error_to_str(error_t error) {
	switch (error) {
		case ERR_INVLN:
			return "Invalid liver: invalid last name";
		case ERR_INVFN:
			return "Invalid liver: invalid first name";
		case ERR_INVMN:
			return "Invalid liver: invalid middle name";
		case ERR_INVDOB:
			return "Invalid liver: invalid date of birth";
		case ERR_INVGEND:
			return "Invalid liver: invalid gender";
		case ERR_INVINC:
			return "Invalid liver: invalid income";
		default:
			return NULL;
	}
}

error_t read_end(error_t ret, liver_node_t** list, char* line) {
	if (list) ln_destroy(*list);

	free(line);

	return ret;
}

error_t liver_read_all(liver_node_t** list, FILE* inFp) {
	if (!list) return ERROR_INVALID_PARAMETER;

	// "Initialize" a new linked list.
	*list = NULL;

	char* line = NULL;
	size_t size;

	while (getline(&line, &size, inFp) > 0) {
		char* newline = strrchr(line, '\n');
		if (newline) *newline = '\0';

		liver_t liver;

		error_t error = liver_from_str(&liver, line);
		if (error) return read_end(error, list, line);

		error = ln_insert(list, liver, NULL);
		if (error) return read_end(error, list, line);
	}

	free(line);

	if (ferror(inFp)) {
		return read_end(ERROR_IO, list, NULL);
	}

	return 0;
}

error_t liver_write(liver_t* liver, FILE* outFp) {
	if (!liver) return ERROR_INVALID_PARAMETER;

	fprintf(outFp, "%s,%s,%s,%02d.%02d.%02d,%c,%lf\n", liver->last_name,
	        liver->first_name, liver->middle_name, liver->dob.day,
	        liver->dob.month, liver->dob.year, liver->gender, liver->income);
	if (ferror(outFp)) return ERROR_IO;

	return 0;
}

error_t liver_write_pretty(liver_t* liver, FILE* outFp) {
	if (!liver) return ERROR_INVALID_PARAMETER;

	fprintf(outFp, "%s %s %s (%02d.%02d.%02d) | %c | %lf\n", liver->last_name,
	        liver->first_name, liver->middle_name, liver->dob.day,
	        liver->dob.month, liver->dob.year, liver->gender, liver->income);
	if (ferror(outFp)) return ERROR_IO;

	return 0;
}

typedef enum parse_st {
	PARSE_LAST_NAME,
	PARSE_FIRST_NAME,
	PARSE_MIDDLE_NAME,
	PARSE_DOB,
	PARSE_GENDER,
	PARSE_INCOME,
	PARSE_VALID
} parse_st_t;

error_t parse_end(error_t ret, liver_t* liver) {
	free(liver->last_name);
	free(liver->first_name);
	free(liver->middle_name);

	return ret;
}

error_t liver_from_str(liver_t* liver, char* str) {
	if (!liver || !str) return ERROR_INVALID_PARAMETER;

	// set these to NULL so we can freely free them
	liver->last_name = NULL;
	liver->first_name = NULL;
	liver->middle_name = NULL;

	parse_st_t st = PARSE_LAST_NAME;
	char* token;

	while ((token = strsep(&str, ","))) {
		switch (st) {
			case PARSE_LAST_NAME:
				if (!validate_name(token)) {
					return parse_end(ERR_INVLN, liver);
				}

				liver->last_name = strdup(token);
				if (!liver->last_name) {
					return parse_end(ERROR_OUT_OF_MEMORY, liver);
				}

				st = PARSE_FIRST_NAME;
				break;
			case PARSE_FIRST_NAME:
				if (!validate_name(token)) {
					return parse_end(ERR_INVFN, liver);
				}

				liver->first_name = strdup(token);
				if (!liver->first_name) {
					return parse_end(ERROR_OUT_OF_MEMORY, liver);
				}

				st = PARSE_MIDDLE_NAME;
				break;
			case PARSE_MIDDLE_NAME:
				if (!validate_middle_name(token)) {
					return parse_end(ERR_INVMN, liver);
				}

				liver->middle_name = strdup(token);
				if (!liver->middle_name) {
					return parse_end(ERROR_OUT_OF_MEMORY, liver);
				}

				st = PARSE_DOB;
				break;
			case PARSE_DOB: {
				struct tm tm;
				if (!strptime(token, "%d.%m.%Y", &tm)) {
					return parse_end(ERR_INVDOB, liver);
				}

				liver->dob.year = tm.tm_year + 1900;
				liver->dob.month = tm.tm_mon + 1;
				liver->dob.day = tm.tm_mday;
				if (!validate_dob(liver->dob)) {
					return parse_end(ERR_INVDOB, liver);
				}

				st = PARSE_GENDER;
				break;
			}
			case PARSE_GENDER: {
				bool tooLong = *token != '\0' && *(token + 1) != '\0';
				if (tooLong) return parse_end(ERR_INVGEND, liver);

				liver->gender = *token;
				if (!validate_gender(liver->gender)) {
					return parse_end(ERR_INVGEND, liver);
				}

				st = PARSE_INCOME;
				break;
			}
			case PARSE_INCOME: {
				if (str_to_double(token, &liver->income) ||
				    !validate_income(liver->income)) {
					return parse_end(ERR_INVINC, liver);
				}

				st = PARSE_VALID;
				break;
			}
			case PARSE_VALID:
				return parse_end(ERROR_UNEXPECTED_TOKEN, liver);
		}
	}

	return 0;
}
