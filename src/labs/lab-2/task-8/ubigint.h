#pragma once

#include "lib/collections/vector.h"
#include "lib/collections/string.h"

typedef struct {
	/** Big integer's digits. Stored as ordinals in in the
	 * radix in little endian (LSD-first). */
	vector_u8_t digits;
	/** Radix of the digits. */
	int radix;
} ubigint_t;

bool ubi_create(const char* n, int radix, ubigint_t* out);

bool ubi_create_zero(int radix, ubigint_t* out);

void ubi_destroy(ubigint_t* n);

bool ubi_add_mut(ubigint_t* a, const ubigint_t* b);

bool ubi_to_string(ubigint_t* a, string_t* out);
