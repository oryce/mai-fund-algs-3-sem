#include "ubigint.h"

#include "lib/chars.h"

bool ubi_create(const char* n, int radix, ubigint_t* out) {
	if (!n || *n == '\0' || radix < 2 || radix > 36 || !out) return false;

	vector_u8_t digits = vector_u8_create();

	char* begin = (char*)n;
	char* ptr = begin;

	// Only skip if `n` is not a whole zero.
	bool skipZeros = *ptr == '0' && *(ptr + 1) != '\0';

	// Move to the end of the number and offset `begin` to
	// skip leading zeros.
	for (; *ptr != '\0'; ++ptr) {
		if (skipZeros) {
			if (*ptr == '0')
				++begin;
			else
				skipZeros = false;
		}
	}

	for (ptr = ptr - 1; ptr >= begin; --ptr) {
		// Get number ordinal in radix.
		int ord;
		if (*ptr >= 'a' && *ptr <= 'z')
			ord = 10 + (*ptr - 'a');
		else if (*ptr >= '0' && *ptr <= '9')
			ord = *ptr - '0';
		else
			return false;
		if (ord > radix) return false;

		// Append to the little endian number.
		if (!vector_u8_push_back(&digits, ord)) return false;
	}

	*out = (ubigint_t){digits, radix};
	return true;
}

bool ubi_create_zero(int radix, ubigint_t* out) {
	if (radix < 2 || radix > 36 || !out) return false;

	vector_u8_t digits = vector_u8_create();
	if (!vector_u8_push_back(&digits, 0)) return false;

	*out = (ubigint_t){digits, radix};
	return true;
}

void ubi_destroy(ubigint_t* n) { vector_u8_destroy(&n->digits); }

uint8_t add_with_carry_(uint8_t a, uint8_t b, int* carry, int radix) {
	uint8_t result;
	result = (a + b + *carry) % radix;
	*carry = (a + b + *carry) / radix;
	return result;
}

bool ubi_add_mut(ubigint_t* a, const ubigint_t* b) {
	if (!a || !b || a->radix != b->radix || a->radix < 2) return false;
	/* Assume `a` and `b` don't contain leading zeros. */

	vector_u8_t* dA = &a->digits;
	vector_u8_t* dB = &b->digits;
	const size_t sizeA = vector_u8_size(dA);
	const size_t sizeB = vector_u8_size(dB);

	size_t i = 0;
	int carry = 0;

	// Add matching digits in `a` and `b`.
	for (; i != sizeA && i != sizeB; ++i) {
		uint8_t* d0 = vector_u8_get(dA, i);
		uint8_t* d1 = vector_u8_get(dB, i);
		*d0 = add_with_carry_(*d0, *d1, &carry, a->radix);
	}

	if (i != sizeA) {
		// If `b` ran out, add the carry to `a`.
		for (; carry && i != sizeA; ++i) {
			uint8_t* d = vector_u8_get(dA, i);
			*d = add_with_carry_(*d, 0, &carry, a->radix);
		}
	} else if (i != sizeB) {
		// Copy digits from `b` to `a` with the carry.
		for (; i != sizeB; ++i) {
			uint8_t d = add_with_carry_(*vector_u8_get(dB, i), 0, &carry, a->radix);
			if (!vector_u8_push_back(dA, d)) return false;
		}
	}

	if (carry) {
		if (!vector_u8_push_back(dA, carry)) return false;
	}

	return true;
}

bool ubi_to_string(ubigint_t* a, string_t* out) {
	if (!a || !out) return false;
	if (!string_create(out)) return false;

	vector_u8_t* digits = &a->digits;

	for (size_t i = vector_u8_size(digits); i--;) {
		uint8_t ord = *vector_u8_get(digits, i);
		char digit = (char)(ord < 10 ? '0' + ord : 'a' + ord - 10);

		if (!string_append_char(out, digit)) return false;
	}

	return true;
}
