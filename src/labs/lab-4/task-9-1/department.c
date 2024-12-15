#include "department.h"

IMPL_VECTOR(vector_oper_t, oper_t, oper, {NULL})

void oper_destroy(oper_t* oper) {
	if (!oper) return;

	free(oper->name);
	oper->name = NULL;

	oper->request = NULL;
	oper->remainingTime = 0;
}

void department_destroy(department_t* dept) {
	if (!dept) return;

	free(dept->id);
	dept->id = NULL;

	heap_destroy(dept->requestQueue);
	dept->requestQueue = NULL;

	for (size_t i = 0; i != vector_oper_size(&dept->operators); ++i) {
		oper_t* oper = vector_oper_get(&dept->operators, i);
		oper_destroy(oper);
	}

	vector_oper_destroy(&dept->operators);
}

double department_load(const department_t* dept) {
	if (!dept) return 0.0;

	double inQueue = (double)dept->requestsInQueue;
	double atMost =
	    (double)vector_oper_size(&dept->operators) * dept->overloadFactor;

	return inQueue / atMost;
}

bool department_is_overloaded(const department_t* dept) {
	return department_load(dept) > 1.0;
}
