#include "task.h"

#include "lib/convert.h"
#include "lib/mth.h"

IMPL_VECTOR(vector_employee_t, employee_t, employee, {NULL})

typedef enum read_state {
	READ_ID,
	READ_FIRST_NAME,
	READ_LAST_NAME,
	READ_SALARY,
	READ_FINISHED
} read_state_t;

static employee_t create_employee(void) {
	return (employee_t){.first_name = {.initialized = false},
	                    .last_name = {.initialized = false}};
}

static error_t read_cleanup(error_t error, char* line,
                            vector_employee_t* employees, employee_t* temp) {
	destroy_employees(employees);

	string_destroy(&temp->first_name);
	string_destroy(&temp->last_name);

	free(line);

	return error;
}

error_t read_employees(vector_employee_t* result, FILE* input) {
	if (!result || !input) return ERROR_INVALID_PARAMETER;

	*result = vector_employee_create();

	char* line = NULL;
	size_t lineCap;

	while (getline(&line, &lineCap, input) > 0) {
		employee_t temp = create_employee();
		read_state_t state = READ_ID;

		char* ptr = line;
		char* token;

		while ((token = strsep(&ptr, " "))) {
			char* newline = strrchr(token, '\n');
			if (newline) *newline = '\0';

			switch (state) {
				case READ_ID:
					if (str_to_ulong(token, &temp.id))
						return read_cleanup(ERROR_UNEXPECTED_TOKEN, line, result, &temp);
					state = READ_FIRST_NAME;
					break;
				case READ_FIRST_NAME:
					if (!string_create(&temp.first_name) ||
					    !string_append_c_str(&temp.first_name, token))
						return read_cleanup(ERROR_OUT_OF_MEMORY, line, result, &temp);
					state = READ_LAST_NAME;
					break;
				case READ_LAST_NAME:
					if (!string_create(&temp.last_name) ||
					    !string_append_c_str(&temp.last_name, token))
						return read_cleanup(ERROR_OUT_OF_MEMORY, line, result, &temp);
					state = READ_SALARY;
					break;
				case READ_SALARY:
					if (str_to_double(token, &temp.salary) || temp.salary < 0)
						return read_cleanup(ERROR_UNEXPECTED_TOKEN, line, result, &temp);
					state = READ_FINISHED;
					break;
			}

			if (state == READ_FINISHED) break;
		}

		if (!vector_employee_push_back(result, temp))
			return read_cleanup(ERROR_OUT_OF_MEMORY, line, result, &temp);
	}

	free(line);

	if (ferror(input)) {
		destroy_employees(result);
		return ERROR_IO;
	}

	return 0;
}

error_t write_employees(vector_employee_t* input, FILE* output) {
	if (!input || !output) return ERROR_INVALID_PARAMETER;

	for (size_t i = 0; i != vector_employee_size(input); ++i) {
		employee_t* employee = vector_employee_get(input, i);

		fprintf(output, "%lu %s %s %lf\n", employee->id,
		        string_to_c_str(&employee->first_name),
		        string_to_c_str(&employee->last_name), employee->salary);
		if (ferror(output)) return ERROR_IO;
	}

	return 0;
}

static int compare_employees_natural(const void* p1, const void* p2) {
	if (!p1 || !p2 || p1 == p2) return 0;

	const employee_t* a = (const employee_t*)p1;
	const employee_t* b = (const employee_t*)p2;

	int order;

	order = mth_sign_double(a->salary - b->salary);
	if (order != 0) return order;
	order = string_compare(&a->last_name, &b->last_name);
	if (order != 0) return order;
	order = string_compare(&a->first_name, &b->first_name);
	if (order != 0) return order;

	return mth_sign_long((long)a->id - (long)b->id);
}

static int compare_employees_reverse(const void* p1, const void* p2) {
	return -compare_employees_natural(p1, p2);
}

error_t sort_employees(vector_employee_t* employees, bool ascending) {
	if (!employees) return ERROR_INVALID_PARAMETER;

	qsort(employees->buffer, employees->size, sizeof(employee_t),
	      ascending ? &compare_employees_natural : &compare_employees_reverse);

	return 0;
}

void destroy_employees(vector_employee_t* vec) {
	for (size_t i = 0; i != vector_employee_size(vec); ++i) {
		employee_t* employee = vector_employee_get(vec, i);
		string_destroy(&employee->first_name);
		string_destroy(&employee->last_name);
	}

	vector_employee_destroy(vec);
}
