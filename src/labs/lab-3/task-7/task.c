#include "task.h"

#include <stdlib.h>

#include "lib/chars.h"

IMPL_VECTOR(vector_change_t, change_t, change, {NULL})

bool validate_name(const char* name) {
	if (!name) return false;

	for (char* p = (char*)name; *p; ++p)
		if (!chars_is_alpha(*p)) return false;

	return *name != '\0';
}

bool validate_middle_name(const char* middleName) {
	if (!middleName) return false;

	for (char* p = (char*)middleName; *p; ++p)
		if (!chars_is_alpha(*p)) return false;

	return true;  // can be empty
}

bool validate_dob(dob_t dob) {
	if (dob.month < 1 || dob.month > 12) return false;

	unsigned days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	bool leap =
	    dob.year % 4 == 0 && (dob.year % 100 != 0 || dob.year % 400 == 0);
	if (leap) days[1] = 29;

	if (dob.day > days[dob.month - 1]) return false;

	return true;
}

bool validate_gender(char gender) { return gender == 'M' || gender == 'W'; }

bool validate_income(double income) { return income > 0; }

void liver_destroy(liver_t* liver) {
	if (!liver) return;

	free(liver->last_name);
	free(liver->first_name);
	free(liver->middle_name);
}

int liver_cmp(const liver_t* a, const liver_t* b) {
	if (!a || !b || a == b) return 0;

	int order;

	order = (int)a->dob.year - (int)b->dob.year;
	if (order) return order;
	order = (int)a->dob.month - (int)b->dob.month;
	if (order) return order;
	order = (int)a->dob.day - (int)b->dob.day;
	if (order) return order;

	return 0;
}

error_t ln_insert(liver_node_t** list, liver_t value,
                  vector_change_t* changes) {
	if (!list) return ERR_INVVAL;

	liver_node_t** node = list;
	while (*node && liver_cmp(&(*node)->value, &value) > 0) {
		node = &(*node)->next;
	}

	liver_node_t* newNode = (liver_node_t*)malloc(sizeof(liver_node_t));
	if (!newNode) return ERR_MEM;

	newNode->value = value;
	newNode->next = *node;

	bool registered = vector_change_push_back(
	    changes,
	    (change_t){.type = CHANGE_INSERT,
	               .variant = {.ptr = {.loc = (void**)node, .prev = *node}}});
	if (changes && !registered) {
		free(newNode);
		return ERR_MEM;
	}

	*node = newNode;
	return 0;
}

error_t ln_remove(liver_node_t** node, vector_change_t* changes) {
	if (!node) return ERR_INVVAL;

	liver_node_t* node0 = *node;

	bool registered = vector_change_push_back(
	    changes,
	    (change_t){.type = CHANGE_REMOVE,
	               .variant = {.ptr = {.loc = (void**)node, .prev = node0}}});
	if (changes && !registered) return ERR_MEM;

	*node = (*node)->next;
	return 0;
}

void ln_destroy(liver_node_t* list) {
	if (!list) return;

	liver_destroy(&list->value);
	if (list->next) ln_destroy(list->next);

	free(list);
}

error_t change_name(change_type_t type, char** pName, const char* name,
                    vector_change_t* changes) {
	if (!pName || !name) return ERR_INVVAL;

	// Copy the provided name to prevent a possible double-free on rollback.
	char* nameCopy = strdup(name);
	if (!nameCopy) return ERR_MEM;

	bool registered = vector_change_push_back(
	    changes,
	    (change_t){.type = type,
	               .variant = {.ptr = {.loc = (void**)pName, .prev = *pName}}});
	if (changes && !registered) return ERR_MEM;

	*pName = nameCopy;
	return 0;
}

error_t liver_change_last_name(liver_t* liver, const char* lastName,
                               vector_change_t* changes) {
	if (!liver || !validate_name(lastName)) return ERR_INVVAL;
	return change_name(CHANGE_LAST_NAME, &liver->last_name, lastName, changes);
}

error_t liver_change_first_name(liver_t* liver, const char* firstName,
                                vector_change_t* changes) {
	if (!liver || !validate_name(firstName)) return ERR_INVVAL;
	return change_name(CHANGE_FIRST_NAME, &liver->first_name, firstName,
	                   changes);
}

error_t liver_change_middle_name(liver_t* liver, const char* middleName,
                                 vector_change_t* changes) {
	if (!liver || !validate_middle_name(middleName)) return ERR_INVVAL;
	return change_name(CHANGE_MIDDLE_NAME, &liver->middle_name, middleName,
	                   changes);
}

error_t liver_change_dob(liver_node_t** list, liver_node_t** node, dob_t dob,
                         vector_change_t* changes) {
	if (!list || !node || !validate_dob(dob)) return ERR_INVVAL;

	change_t change;
	change_t newChange = {CHANGE_DOB};

	liver_t liver = (*node)->value;

	error_t error = ln_remove(node, changes);
	if (error) return error;

	if (changes) {
		error = vector_change_pop_back(changes, &change) ? 0 : ERR_MEM;
		if (error) return error;

		newChange.variant.dob.c0Loc = change.variant.ptr.loc;
		newChange.variant.dob.c0Prev = change.variant.ptr.prev;
	}

	liver.dob = dob;

	error = ln_insert(list, liver, changes);
	if (error) return error;

	if (changes) {
		error = vector_change_pop_back(changes, &change) ? 0 : ERR_MEM;
		if (error) return error;

		newChange.variant.dob.c1Loc = change.variant.ptr.loc;
		newChange.variant.dob.c1Prev = change.variant.ptr.prev;

		if (!vector_change_push_back(changes, newChange)) return ERR_MEM;
	}

	return 0;
}

error_t liver_change_gender(liver_t* liver, char gender,
                            vector_change_t* changes) {
	if (!liver || !validate_gender(gender)) return ERR_INVVAL;

	bool registered = vector_change_push_back(
	    changes, (change_t){.type = CHANGE_GENDER,
	                        .variant = {.gender = {.loc = &liver->gender,
	                                               .prev = liver->gender}}});
	if (changes && !registered) return ERR_MEM;

	liver->gender = gender;
	return 0;
}

error_t liver_change_income(liver_t* liver, double income,
                            vector_change_t* changes) {
	if (!liver || !validate_income(income)) return ERR_INVVAL;

	bool registered = vector_change_push_back(
	    changes, (change_t){.type = CHANGE_INCOME,
	                        .variant = {.income = {.loc = &liver->income,
	                                               .prev = liver->income}}});
	if (changes && !registered) return ERR_MEM;

	liver->income = income;
	return 0;
}

void change_rollback(change_t* change) {
	if (!change) return;

	switch (change->type) {
		case CHANGE_INSERT: {
			liver_node_t* node = (liver_node_t*)*change->variant.ptr.loc;
			liver_destroy(&node->value);

			free(node);
			*change->variant.ptr.loc = change->variant.ptr.prev;

			break;
		}
		case CHANGE_LAST_NAME:
		case CHANGE_FIRST_NAME:
		case CHANGE_MIDDLE_NAME:
			free(*change->variant.ptr.loc);
			*change->variant.ptr.loc = change->variant.ptr.prev;

			break;
		case CHANGE_REMOVE:
			*change->variant.ptr.loc = change->variant.ptr.prev;
			break;
		case CHANGE_DOB:
			free(*change->variant.dob.c1Loc);
			*change->variant.dob.c1Loc = change->variant.dob.c1Prev;
			*change->variant.dob.c0Loc = change->variant.dob.c0Prev;

			break;
		case CHANGE_GENDER:
			*change->variant.gender.loc = change->variant.gender.prev;
			break;
		case CHANGE_INCOME:
			*change->variant.income.loc = change->variant.income.prev;
			break;
	}
}

void change_forget(change_t* change) {
	if (!change) return;

	switch (change->type) {
		case CHANGE_REMOVE: {
			liver_node_t* node = (liver_node_t*)change->variant.ptr.prev;
			liver_destroy(&node->value);

			free(node);
			break;
		}
		case CHANGE_LAST_NAME:
		case CHANGE_FIRST_NAME:
		case CHANGE_MIDDLE_NAME:
			free(change->variant.ptr.prev);
			break;
		case CHANGE_DOB:
			free(change->variant.dob.c0Prev);
			break;
	}
}

void ln_rollback(vector_change_t* changes) {
	size_t n = vector_change_size(changes);

	for (size_t i = n; i-- > n / 2;) {
		printf("[ln_rollback] rollback %zu/%zu\n", i + 1, n);

		change_t* change = vector_change_get(changes, i);
		change_rollback(change);
	}

	for (size_t i = n / 2; i-- > 0;) {
		printf("[ln_rollback] forget %zu/%zu\n", i + 1, n);

		change_t* change = vector_change_get(changes, i);
		change_forget(change);
	}

	vector_change_clear(changes);
}
