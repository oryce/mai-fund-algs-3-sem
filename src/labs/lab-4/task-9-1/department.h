#pragma once

#include <stdbool.h>

#include "heap.h"
#include "lib/collections/vector.h"
#include "request.h"

typedef struct oper {
	/** Name of the operator. */
	const char* name;

	/** Current request being handled.*/
	request_t* request;
	/** Remaining time at work. */
	unsigned remainingTime;
} oper_t;

void oper_destroy(oper_t* oper);

DEFINE_VECTOR(vector_oper_t, oper_t, oper)

typedef struct department {
	/** ID of the department. */
	const char* id;

	/** Priority queue of requests in department. */
	heap_t* requestQueue;
	/** Requests currently in queue. */
	size_t requestsInQueue;

	double overloadFactor;

	/** List of operators in the department. */
	vector_oper_t operators;
} department_t;

void department_destroy(department_t* dept);

double department_load(const department_t* dept);

bool department_is_overloaded(const department_t* dept);
