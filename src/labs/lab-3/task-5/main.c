#include "lib/convert.h"
#include "lib/io.h"
#include "task.h"

#define INVALID_COMMAND "Unknown or malformed command. Type 'h' for help\n"

void print_student(student_t* student) {
	printf("%lu %s %s %s\n", student->id, string_to_c_str(&student->first_name),
	       string_to_c_str(&student->last_name),
	       string_to_c_str(&student->group));
}

void cmd_find(vector_st_t* students, char* inType, char* inParam) {
	student_filter_t func;
	const void* param;

	// Store temporary values that are parsed inside
	// if blocks here.
	unsigned long idValue;

	if (strcmp(inType, "id") == 0) {
		func = &student_eq_id;

		if (str_to_ulong(inParam, &idValue)) {
			fprintf(stderr, INVALID_COMMAND);
			return;
		}

		param = &idValue;
	} else if (strcmp(inType, "first_name") == 0) {
		func = &student_eq_first_name;
		param = inParam;
	} else if (strcmp(inType, "last_name") == 0) {
		func = &student_eq_last_name;
		param = inParam;
	} else if (strcmp(inType, "group") == 0) {
		func = &student_eq_group;
		param = inParam;
	} else {
		fprintf(stderr, INVALID_COMMAND);
		return;
	}

	student_t* student;
	size_t off = 0;
	bool foundOne = false;

	while ((student = student_find(students, func, param, &off))) {
		print_student(student);
		foundOne = true;
	}

	if (!foundOne) {
		fprintf(stderr, "No students found.\n");
	}
}

void cmd_sort(vector_st_t* students, char* inType) {
	student_comp_t func;

	if (strcmp(inType, "id") == 0) {
		func = &student_comp_id;
	} else if (strcmp(inType, "first_name") == 0) {
		func = &student_comp_first_name;
	} else if (strcmp(inType, "last_name") == 0) {
		func = &student_comp_last_name;
	} else if (strcmp(inType, "group") == 0) {
		func = &student_comp_group;
	} else {
		fprintf(stderr, INVALID_COMMAND);
		return;
	}

	student_sort(students, func);
	printf("OK.\n");
}

void cmd_id_trace(vector_st_t* students, char* inId, FILE* traceFile) {
	unsigned long id;

	if (str_to_ulong(inId, &id)) {
		fprintf(stderr, INVALID_COMMAND);
		return;
	}

	size_t offset = 0;
	student_t* student = student_find(students, student_eq_id, &id, &offset);

	if (!student) {
		fprintf(stderr, "Student not found.\n");
		return;
	}

	double avgGrade = student_avg_grade(student);

	fprintf(traceFile, "%s %s %s %lf\n", string_to_c_str(&student->last_name),
	        string_to_c_str(&student->first_name),
	        string_to_c_str(&student->group), avgGrade);

	if (ferror(traceFile)) {
		fprintf(stderr, "Can't write to the trace file.\n");
	} else {
		printf("OK.\n");
	}
}

void cmd_above_average(vector_st_t* students, FILE* traceFile) {
	double avg = student_avg_grade_all(students);

	student_t* st;
	size_t off = 0;

	while ((st = student_find(students, student_gt_avg_grade, &avg, &off))) {
		fprintf(traceFile, "%s %s\n", string_to_c_str(&st->last_name),
		        string_to_c_str(&st->first_name));
		if (ferror(traceFile)) {
			fprintf(stderr, "Can't write to the trace file.\n");
			return;
		}
	}

	printf("OK.\n");
}

void cmd_loop(vector_st_t* students, FILE* traceFile) {
	printf("Input a command ('h' for help).\n");

	char* command = NULL;
	size_t size;

	while (getline(&command, &size, stdin) > 0) {
		switch (*command) {
			case 'h':
				printf(
				    "Command help:\n"
				    "  f <id/first_name/last_name/group> <param>\n"
				    "      Finds a student and prints it's info.\n"
				    "  s <id/first_name/last_name/group>\n"
				    "      Sorts the students.\n"
				    "  p\n"
				    "      Prints all students.\n"
				    "  i <id>\n"
				    "      Writes the student's info to the trace file.\n"
				    "  a\n"
				    "      Writes students with above-average grades to the\n"
				    "      trace file.\n");
				break;
			case 'f': {
				char type[32];
				char param[128];

				if (sscanf(command, "f %31s %127s", type, param) != 2) {
					fprintf(stderr, INVALID_COMMAND);
					break;
				}

				cmd_find(students, type, param);
				break;
			}
			case 's': {
				char type[32];

				if (sscanf(command, "s %31s", type) != 1) {
					fprintf(stderr, INVALID_COMMAND);
					break;
				}

				cmd_sort(students, type);
				break;
			}
			case 'p': {
				for (size_t i = 0; i != vector_st_size(students); ++i) {
					student_t* student = vector_st_get(students, i);
					print_student(student);
				}

				break;
			}
			case 'i': {
				char id[128];

				if (sscanf(command, "i %127s", id) != 1) {
					fprintf(stderr, INVALID_COMMAND);
					break;
				}

				cmd_id_trace(students, id, traceFile);
				break;
			}
			case 'a': {
				cmd_above_average(students, traceFile);
				break;
			}
			default:
				fprintf(stderr, INVALID_COMMAND);
		}
	}

	free(command);
}

void custom_err_print(error_t error) {
	error_fmt_t fmt[] = {&task_error_to_string};
	error_print_ex(error, fmt, sizeof(fmt) / sizeof(fmt[0]));
}

int main_cleanup(int status, FILE* studentsFile, FILE* traceFile,
                 vector_st_t* students) {
	fclose(studentsFile);
	fclose(traceFile);
	student_destroy_all(students);

	return status;
}

int main(int argc, char** argv) {
	if (argc != 3) {
		fprintf(stderr, "usage: %s <students file> [trace file]", argv[0]);
		return 1;
	}

	FILE* studentsFile = NULL;
	FILE* traceFile = NULL;
	vector_st_t students = {.size = 0};

	if (!(studentsFile = fopen(argv[1], "r"))) {
		fprintf(stderr, "Can't open students file for reading.");
		return main_cleanup(1, studentsFile, traceFile, &students);
	}

	bool same;
	if (paths_same(argv[1], argv[2], &same) || same) {
		fprintf(stderr,
		        "Students file and trace file may not be the same, or one "
		        "doesn't exist.\n");
		return main_cleanup(1, studentsFile, traceFile, &students);
	}
	if (!(traceFile = fopen(argv[2], "w"))) {
		fprintf(stderr, "Can't open trace file for writing.\n");
		return main_cleanup(1, studentsFile, traceFile, &students);
	}

	error_t error = student_read(studentsFile, &students);
	if (error) {
		custom_err_print(error);
		return main_cleanup(1, studentsFile, traceFile, &students);
	}

	cmd_loop(&students, traceFile);
	return main_cleanup(0, studentsFile, traceFile, &students);
}
