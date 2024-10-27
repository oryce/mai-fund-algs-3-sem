#include "lib/collections/string.h"
#include "lib/collections/vector.h"
#include "lib/error.h"

typedef struct employee {
	unsigned long id;
	string_t first_name;
	string_t last_name;
	double salary;
} employee_t;

DEFINE_VECTOR(vector_employee_t, employee_t, employee)

error_t read_employees(vector_employee_t* result, FILE* input);

error_t write_employees(vector_employee_t* input, FILE* output);

error_t sort_employees(vector_employee_t* employees, bool ascending);

void destroy_employees(vector_employee_t* vec);
