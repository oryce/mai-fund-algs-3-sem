#include "task.h"

#include "lib/chars.h"
#include "lib/convert.h"
#include "lib/mth.h"

const char* task_error_to_string(error_t error) {
	switch (error) {
		case ERR_INVID:
			return "Invalid ID";
		case ERR_INVFN:
			return "Invalid first name";
		case ERR_INVLN:
			return "Invalid last name";
		case ERR_INVGRP:
			return "Invalid group";
		case ERR_INVGRD:
			return "Invalid grade";
		case ERR_INVGRDCNT:
			return "Invalid grade count";
		case ERR_TOOFEWTOK:
			return "Too few tokens";
		default:
			return NULL;
	}
}

IMPL_VECTOR(vector_st_t, student_t, st, {NULL})

student_t student_create(void) {
	return (student_t){.first_name = {.initialized = false},
	                   .last_name = {.initialized = false},
	                   .group = {.initialized = false},
	                   .grades = vector_u8_create()};
}

void student_destroy(student_t* out) {
	if (!out) return;

	vector_u8_destroy(&out->grades);
	string_destroy(&out->first_name);
	string_destroy(&out->last_name);
	string_destroy(&out->group);
}

void student_destroy_all(vector_st_t* students) {
	if (!students) return;

	for (size_t i = 0; i != vector_st_size(students); ++i) {
		student_t* st = vector_st_get(students, i);
		student_destroy(st);
	}

	vector_st_destroy(students);
}

typedef enum read_step {
	READ_ID,
	READ_FIRST_NAME,
	READ_LAST_NAME,
	READ_GROUP,
	READ_GRADES,
	READ_VALID
} read_step_t;

typedef struct read_state {
	read_step_t step;
	student_t student;
} read_state_t;

bool valid_name(string_t* name) {
	if (!name) return false;

	char* str = (char*)string_to_c_str(name);
	bool nonEmpty = false;

	for (; *str != '\0'; ++str) {
		if (!chars_is_alpha(*str) && *str != ' ') return false;
		if (*str != ' ') nonEmpty = true;
	}

	return nonEmpty;
}

bool valid_group(string_t* name) {
	if (!name) return false;

	char* str = (char*)string_to_c_str(name);
	bool nonBlank = false;

	for (; *str != '\0'; ++str) {
		if (!chars_is_space(*str)) nonBlank = true;
	}

	return nonBlank;
}

int grade_value(const char grade) {
	if (grade < '1' || grade > '5') return -1;
	return grade - '1' + 1;
}

bool valid_grade(const char grade) { return grade_value(grade) != -1; }

error_t read_fail(error_t error, vector_st_t* students, student_t* student,
                  char* line) {
	if (students) student_destroy_all(students);
	if (student) student_destroy(student);
	free(line);

	return error;
}

error_t student_read(FILE* fp, vector_st_t* students) {
	if (!students) return ERR_INVVAL;

	const int reqGrades = 5;

	*students = vector_st_create();
	read_state_t st;

	char* line = NULL;
	size_t size;

	while (getline(&line, &size, fp) > 0) {
		st.step = READ_ID;
		st.student = student_create();

		char* newline = strrchr(line, '\n');
		if (newline) *newline = '\0';

		char* linePtr = line;
		char* token;

		while ((token = strsep(&linePtr, ","))) {
			switch (st.step) {
				case READ_ID: {
					if (str_to_ulong(token, &st.student.id))
						return read_fail(ERR_INVID, students, &st.student,
						                 line);

					st.step = READ_FIRST_NAME;
					break;
				}
				case READ_FIRST_NAME: {
					if (!string_from_c_str(&st.student.first_name, token) ||
					    !valid_name(&st.student.first_name)) {
						return read_fail(ERR_INVFN, students, &st.student,
						                 line);
					}

					st.step = READ_LAST_NAME;
					break;
				}
				case READ_LAST_NAME: {
					if (!string_from_c_str(&st.student.last_name, token) ||
					    !valid_name(&st.student.last_name)) {
						return read_fail(ERR_INVLN, students, &st.student,
						                 line);
					}

					st.step = READ_GROUP;
					break;
				}
				case READ_GROUP: {
					if (!string_from_c_str(&st.student.group, token) ||
					    !valid_group(&st.student.group)) {
						return read_fail(ERR_INVGRP, students, &st.student,
						                 line);
					}

					st.step = READ_GRADES;
					break;
				}
				case READ_GRADES: {
					char* grPtr = token;
					char* grTok;

					while ((grTok = strsep(&grPtr, ";"))) {
						if (strlen(grTok) != 1 || !valid_grade(*grTok))
							return read_fail(ERR_INVGRD, students, &st.student,
							                 line);
						if (!vector_u8_push_back(&st.student.grades, *grTok))
							return read_fail(ERR_MEM, students, &st.student,
							                 line);
					}

					st.step = READ_VALID;
					break;
				}
				case READ_VALID:
					// Unexpected token.
					return read_fail(ERR_UNEXPTOK, students, &st.student, line);
			}
		}

		if (st.step != READ_VALID)
			return read_fail(ERR_TOOFEWTOK, students, &st.student, line);
		if (vector_u8_size(&st.student.grades) != reqGrades)
			return read_fail(ERR_INVGRDCNT, students, &st.student, line);
		if (!vector_st_push_back(students, st.student))
			return read_fail(ERR_MEM, students, &st.student, line);
	}

	free(line);

	if (ferror(fp)) {
		return read_fail(ERR_MEM, students, &st.student, NULL);
	}

	return 0;
}

int student_comp_id(const student_t* a, const student_t* b) {
	if (!a || !b || a == b) return 0;
	return mth_sign_long((long)a->id - (long)b->id);
}

int student_comp_first_name(const student_t* a, const student_t* b) {
	if (!a || !b || a == b) return 0;
	return string_compare(&a->first_name, &b->first_name);
}

int student_comp_last_name(const student_t* a, const student_t* b) {
	if (!a || !b || a == b) return 0;
	return string_compare(&a->last_name, &b->last_name);
}

int student_comp_group(const student_t* a, const student_t* b) {
	if (!a || !b || a == b) return 0;
	return string_compare(&a->group, &b->group);
}

bool student_eq_id(const student_t* student, const void* param) {
	if (!student || !param) return false;
	return student->id == *(unsigned long*)param;
}

bool student_eq_first_name(const student_t* student, const void* param) {
	if (!student || !param) return false;
	return strcmp(string_to_c_str((string_t*)&student->first_name),
	              (const char*)param) == 0;
}

bool student_eq_last_name(const student_t* student, const void* param) {
	if (!student || !param) return false;
	return strcmp(string_to_c_str((string_t*)&student->last_name),
	              (const char*)param) == 0;
}

bool student_eq_group(const student_t* student, const void* param) {
	if (!student || !param) return false;
	return strcmp(string_to_c_str((string_t*)&student->group),
	              (const char*)param) == 0;
}

bool student_gt_avg_grade(const student_t* student, const void* param) {
	if (!student || !param) return false;
	return student_avg_grade((student_t*)student) > *(double*)param;
}

student_t* student_find(vector_st_t* students, student_filter_t eq,
                        const void* param, size_t* off) {
	if (!students || !eq || !param || !off) return NULL;

	for (; *off < vector_st_size(students); ++(*off)) {
		student_t* other = vector_st_get(students, (*off));

		if (eq(other, param)) {
			++(*off);
			return other;
		}
	}

	return NULL;
}

void student_sort(vector_st_t* students, student_comp_t comp) {
	if (!students || !comp) return;
	qsort(students->buffer, students->size, sizeof(student_t),
	      (int (*)(const void*, const void*))comp);
}

double student_avg_grade(student_t* student) {
	if (!student) return 0.0;

	double sum = 0;
	double amount = (double)vector_u8_size(&student->grades);

	for (size_t i = 0; i != vector_u8_size(&student->grades); ++i) {
		uint8_t* grade = vector_u8_get(&student->grades, i);
		sum += grade_value((char)*grade);
	}

	return sum / amount;
}

double student_avg_grade_all(vector_st_t* students) {
	if (!students) return 0.0;

	double sum = 0;
	double amount = (double)vector_st_size(students);

	for (size_t i = 0; i != vector_st_size(students); ++i) {
		student_t* student = vector_st_get(students, i);
		sum += student_avg_grade(student);
	}

	return sum / amount;
}
