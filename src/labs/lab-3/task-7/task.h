#pragma once

#include <stdbool.h>

#include "lib/collections/vector.h"
#include "lib/error.h"

typedef struct dob {
	unsigned day;
	unsigned month;
	unsigned year;
} dob_t;

typedef struct liver {
	char* last_name;
	char* first_name;
	char* middle_name;
	dob_t dob;
	char gender;
	double income;
} liver_t;

bool validate_name(const char* name);

bool validate_middle_name(const char* middleName);

bool validate_dob(dob_t dob);

bool validate_gender(char gender);

bool validate_income(double income);

void liver_destroy(liver_t* liver);

typedef struct liver_node {
	liver_t value;
	struct liver_node* next;
} liver_node_t;

typedef enum change_type {
	CHANGE_INSERT,
	CHANGE_REMOVE,
	CHANGE_FIRST_NAME,
	CHANGE_LAST_NAME,
	CHANGE_MIDDLE_NAME,
	CHANGE_DOB,
	CHANGE_GENDER,
	CHANGE_INCOME
} change_type_t;

typedef struct change {
	change_type_t type;
	union {
		struct {
			void** loc;
			void* prev;
		} ptr;
		struct {
			// Remove change
			void** c0Loc;
			void* c0Prev;
			// Insert change
			void** c1Loc;
			void* c1Prev;
		} dob;
		struct {
			char* loc;
			char prev;
		} gender;
		struct {
			double* loc;
			double prev;
		} income;
	} variant;
} change_t;

DEFINE_VECTOR(vector_change_t, change_t, change)

error_t ln_insert(liver_node_t** list, liver_t value, vector_change_t* changes);

error_t ln_remove(liver_node_t** node, vector_change_t* changes);

void ln_destroy(liver_node_t* list);

error_t liver_change_last_name(liver_t* liver, const char* lastName,
                               vector_change_t* changes);

error_t liver_change_first_name(liver_t* liver, const char* firstName,
                                vector_change_t* changes);

error_t liver_change_middle_name(liver_t* liver, const char* middleName,
                                 vector_change_t* changes);

error_t liver_change_dob(liver_node_t** list, liver_node_t** node, dob_t dob,
                         vector_change_t* changes);

error_t liver_change_gender(liver_t* liver, char gender,
                            vector_change_t* changes);

error_t liver_change_income(liver_t* liver, double income,
                            vector_change_t* changes);

void change_rollback(change_t* change);

void change_forget(change_t* change);

void ln_rollback(vector_change_t* changes);
