#include <stdio.h>
#include <time.h>

#include "lib/convert.h"
#include "marshal.h"
#include "task.h"

#define INVALID_COMMAND "Unknown or invalid command. Type 'h' for help.\n"

void err_print(error_t error) {
	error_fmt_t fmt[] = {&marshal_error_to_str};
	error_print_ex(error, fmt, sizeof(fmt) / sizeof(fmt[0]));
}

liver_node_t** find_by_idx(liver_node_t** livers, size_t idx) {
	if (!livers) return NULL;

	// Find a liver "by index": traverse the list until
	// the index runs out.
	liver_node_t** ln = livers;

	while (*ln && idx--) {
		ln = &(*ln)->next;
	}

	if (!(*ln)) {
		fprintf(stderr, "List index out of range.\n");
		return NULL;
	}

	return ln;
}

void cmd_print(liver_node_t** livers) {
	if (!livers) return;

	size_t i = 0;

	for (liver_node_t* ln = *livers; ln; ln = ln->next, ++i) {
		printf("[%zu] ", i);
		liver_write_pretty(&ln->value, stdout);
	}
}

void cmd_find(liver_node_t** livers, const char* lastName,
              const char* firstName, const char* middleName) {
	if (!livers || !lastName || !firstName || !middleName) return;

	for (liver_node_t* ln = *livers; ln; ln = ln->next) {
		liver_t* liver = &ln->value;
		if (strcmp(liver->last_name, lastName) == 0 &&
		    strcmp(liver->first_name, firstName) == 0 &&
		    strcmp(liver->middle_name, middleName) == 0) {
			liver_write_pretty(liver, stdout);
		}
	}
}

void cmd_add(liver_node_t** livers, vector_change_t* changes) {
	if (!livers || !changes) return;

	// Read a serialized liver from stdin and parse it.
	printf(
	    "Enter the liver's info according to the format:\n"
	    "last_name,first_name,middle_name,dd.MM.yyyy,M/F,income\n");

	char* line = NULL;
	size_t size;

	if (getline(&line, &size, stdin) <= 0) {
		fprintf(stderr, "Can't read from stdin.");

		free(line);
		return;
	}

	char* newline = strrchr(line, '\n');
	if (newline) *newline = '\0';

	liver_t liver;
	char* ptr = line;

	error_t error =
	    liver_from_str(&liver, ptr) || ln_insert(livers, liver, changes);
	if (error) {
		err_print(error);

		free(line);
		return;
	}

	printf("OK.\n");
	free(line);
}

void cmd_remove(liver_node_t** livers, size_t idx, vector_change_t* changes) {
	if (!livers || !changes) return;

	liver_node_t** ln = find_by_idx(livers, idx);
	if (!ln) return;

	// Remove the found node.
	error_t error = ln_remove(ln, changes);

	if (error) {
		err_print(error);
		return;
	}

	printf("OK.\n");
}

void cmd_change(liver_node_t** livers, size_t idx, const char* field,
                const char* value, vector_change_t* changes) {
	if (!livers || !field || !value || !changes) return;

	liver_node_t** ln = find_by_idx(livers, idx);
	if (!ln) return;

	liver_t* liver = &(*ln)->value;
	error_t error = 0;

	if (strcmp(field, "first_name") == 0) {
		error = liver_change_first_name(liver, value, changes);
	} else if (strcmp(field, "last_name") == 0) {
		error = liver_change_last_name(liver, value, changes);
	} else if (strcmp(field, "middle_name") == 0) {
		error = liver_change_middle_name(liver, value, changes);
	} else if (strcmp(field, "dob") == 0) {
		struct tm tm;
		if (!strptime(value, "%d.%m.%Y", &tm)) error = ERROR_INVALID_PARAMETER;
		if (!error) {
			dob_t dob = {.day = tm.tm_mday,
			             .month = tm.tm_mon + 1,
			             .year = tm.tm_year + 1900};
			error = liver_change_dob(livers, ln, dob, changes);
		}
	} else if (strcmp(field, "gender") == 0) {
		if (*value != '\0' && *(value + 1) != '\0') error = ERROR_INVALID_PARAMETER;
		if (!error) error = liver_change_gender(liver, *value, changes);
	} else if (strcmp(field, "income") == 0) {
		double income;
		error = str_to_double((char*)value, &income) ||
		        liver_change_income(liver, income, changes);
	} else {
		fprintf(stderr, "Unrecognized field: %s\n", field);
	}

	if (error) {
		error_print(error);
	} else {
		printf("OK.\n");
	}
}

void cmd_rollback(vector_change_t* changes) {
	if (!changes) return;

	ln_rollback(changes);
	printf("OK.\n");
}

void cmd_write(liver_node_t** livers, const char* filename) {
	if (!livers || !filename) return;

	FILE* fp = fopen(filename, "w");
	if (!fp) {
		fprintf(stderr, "Can't open %s for writing.\n", filename);
		return;
	}

	for (liver_node_t* ln = *livers; ln; ln = ln->next) {
		error_t error = liver_write(&ln->value, fp);
		if (error) {
			error_print(error);
			return;
		}
	}

	if (fflush(fp) != 0) {
		fprintf(stderr, "Can't flush output buffer.\n");
		return;
	}

	printf("OK.\n");
}

void cmd_loop(liver_node_t** livers) {
	if (!livers) return;

	vector_change_t changes = vector_change_create();
	printf("Input a command ('h' for help).\n");

	char* cmd = NULL;
	size_t size;

	while (getline(&cmd, &size, stdin) > 0) {
		char* newline = strrchr(cmd, '\n');
		if (newline) *newline = '\0';

		switch (cmd[0]) {
			case 'h':
				printf(
				    "Command help:\n"
				    "  p\n"
				    "     Prints all livers.\n"
				    "  f <last_name>,<first_name>,<middle_name>\n"
				    "     Finds livers by name.\n"
				    "  a\n"
				    "     Adds a new liver, prompting for info.\n"
				    "  r <idx>\n"
				    "     Removes a liver, given an index in 'p'.\n"
				    "  c <idx> <first_name/last_name/middle_name/dob/gender\n"
				    "    /income> '<value>'\n"
				    "     Modifies a liver, given an index in 'p'."
				    "  u\n"
				    "     Rollbacks last N / 2 operations.\n"
				    "  w <file>\n"
				    "     Saves livers to file.\n");
				break;
			case 'p':
				cmd_print(livers);
				break;
			case 'f': {
				char inLastName[128];
				char inFirstName[128];
				char inMiddleName[128] = {'\0'};

				if (sscanf(cmd, "f %127[^,],%127[^,],%127[^,]", inLastName,
				           inFirstName, inMiddleName) < 2) {
					fprintf(stderr, INVALID_COMMAND);
					break;
				}

				cmd_find(livers, inLastName, inFirstName, inMiddleName);
				break;
			}
			case 'a':
				cmd_add(livers, &changes);
				break;
			case 'r': {
				char inIdx[128];
				size_t idx;

				if (sscanf(cmd, "r %127s", inIdx) != 1 ||
				    str_to_ulong(inIdx, &idx)) {
					fprintf(stderr, INVALID_COMMAND);
					break;
				}

				cmd_remove(livers, idx, &changes);
				break;
			}
			case 'c': {
				char inIdx[128];
				char inField[128];
				char inValue[256];
				size_t idx;

				if (sscanf(cmd, "c %127s %127s '%255[^']'", inIdx, inField,
				           inValue) != 3 ||
				    str_to_ulong(inIdx, &idx)) {
					fprintf(stderr, INVALID_COMMAND);
					break;
				}

				cmd_change(livers, idx, inField, inValue, &changes);
				break;
			}
			case 'w': {
				char inFilename[128];

				if (sscanf(cmd, "w %127s", inFilename) != 1) {
					fprintf(stderr, INVALID_COMMAND);
					break;
				}

				cmd_write(livers, inFilename);
				break;
			}
			case 'u':
				cmd_rollback(&changes);
				break;
			default:
				fprintf(stderr, INVALID_COMMAND);
		}
	}

	free(cmd);

	// Rollback all changes, so we can clean up livers later without having to
	// worry about double-frees.
	for (size_t i = vector_change_size(&changes); i-- > 0;) {
		change_t* change = vector_change_get(&changes, i);
		change_rollback(change);
	}

	vector_change_destroy(&changes);
}

int main_cleanup(int ret, FILE* inputFile, liver_node_t* livers) {
	fclose(inputFile);
	ln_destroy(livers);

	return ret;
}

int main(int argc, char** argv) {
	if (argc != 2) {
		fprintf(stderr, "usage: %s <input file>", argv[0]);
		return 1;
	}

	FILE* inputFile = NULL;
	liver_node_t* livers = NULL;

	inputFile = fopen(argv[1], "r");
	if (!inputFile) {
		fprintf(stderr, "Can't open input file for reading.\n");
		return main_cleanup(2, inputFile, livers);
	}

	error_t error = liver_read_all(&livers, inputFile);
	if (error) {
		err_print(error);
		return main_cleanup(3, inputFile, livers);
	}

	cmd_loop(&livers);
	return main_cleanup(0, inputFile, livers);
}
