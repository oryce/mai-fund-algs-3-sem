#pragma once

#include <stdio.h>

#include "lib/collections/string.h"
#include "lib/collections/vector.h"
#include "lib/error.h"

#define ERR_INVID 0x10000001
#define ERR_INVFN 0x10000002
#define ERR_INVLN 0x10000003
#define ERR_INVGRP 0x10000004
#define ERR_INVGRD 0x10000005
#define ERR_INVGRDCNT 0x10000006
#define ERR_TOOFEWTOK 0x10000007

const char* task_error_to_string(error_t error);

typedef struct student {
	unsigned long id;
	string_t first_name;
	string_t last_name;
	string_t group;
	vector_u8_t grades;
} student_t;

DEFINE_VECTOR(vector_st_t, student_t, st)

student_t student_create(void);

void student_destroy(student_t* out);

void student_destroy_all(vector_st_t* students);

error_t student_read(FILE* fp, vector_st_t* students);

typedef int (*student_comp_t)(const student_t* a, const student_t* b);

typedef bool (*student_filter_t)(const student_t* student, const void* param);

int student_comp_id(const student_t* a, const student_t* b);

int student_comp_first_name(const student_t* a, const student_t* b);

int student_comp_last_name(const student_t* a, const student_t* b);

int student_comp_group(const student_t* a, const student_t* b);

bool student_eq_id(const student_t* student, const void* param);

bool student_eq_first_name(const student_t* student, const void* param);

bool student_eq_last_name(const student_t* student, const void* param);

bool student_eq_group(const student_t* student, const void* param);

bool student_gt_avg_grade(const student_t* student, const void* param);

student_t* student_find(vector_st_t* students, student_filter_t eq,
                        const void* param, size_t* off);

void student_sort(vector_st_t* students, student_comp_t comp);

double student_avg_grade(student_t* student);

double student_avg_grade_all(vector_st_t* students);
