#include "task.h"

#include <stdarg.h>

bool add_bigints_cleanup_(ubigint_t* bigNum) {
	ubi_destroy(bigNum);
	return false;
}

bool task_add_bigints(ubigint_t* result, int radix, int n, ...) {
	// result = 0
	if (!ubi_create_zero(radix, result)) return false;

	va_list args;
	va_start(args, n);

	while (n--) {
		const char* num = va_arg(args, const char*);
		ubigint_t bigNum;

		if (!ubi_create(num, radix, &bigNum)) {
			va_end(args);
			return add_bigints_cleanup_(&bigNum);
		}

		if (!ubi_add_mut(result, &bigNum)) {
			va_end(args);
			return add_bigints_cleanup_(&bigNum);
		}

		ubi_destroy(&bigNum);
	}

	return true;
}
