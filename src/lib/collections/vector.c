#include "lib/collections/vector.h"

#include <stdlib.h>

const size_t VECTOR_MIN_CAPACITY = 4;

#define COMPARATOR(TYPE)                              \
	int comp_##TYPE(const void* p1, const void* p2) { \
		if (!p1 || !p2 || p1 == p2) return 0;         \
                                                      \
		TYPE t1 = *(TYPE*)p1;                         \
		TYPE t2 = *(TYPE*)p2;                         \
                                                      \
		if (t1 < t2)                                  \
			return -1;                                \
		else if (t1 > t2)                             \
			return 1;                                 \
		else                                          \
			return 0;                                 \
	}

COMPARATOR(int64_t)
COMPARATOR(int32_t)
COMPARATOR(int16_t)
COMPARATOR(int8_t)
COMPARATOR(uint64_t)
COMPARATOR(uint32_t)
COMPARATOR(uint16_t)
COMPARATOR(uint8_t)
COMPARATOR(float)
COMPARATOR(double)

IMPL_VECTOR(vector_i64_t, int64_t, i64, {&comp_int64_t})
IMPL_VECTOR(vector_i32_t, int32_t, i32, {&comp_int32_t})
IMPL_VECTOR(vector_i16_t, int16_t, i16, {&comp_int16_t})
IMPL_VECTOR(vector_i8_t, int8_t, i8, {&comp_int8_t})
IMPL_VECTOR(vector_u64_t, uint64_t, u64, {&comp_uint64_t})
IMPL_VECTOR(vector_u32_t, uint32_t, u32, {&comp_uint32_t})
IMPL_VECTOR(vector_u16_t, uint16_t, u16, {&comp_uint16_t})
IMPL_VECTOR(vector_u8_t, uint8_t, u8, {&comp_uint8_t})
IMPL_VECTOR(vector_flt_t, float, flt, {&comp_float})
IMPL_VECTOR(vector_dbl_t, double, dbl, {&comp_double})
IMPL_VECTOR(vector_ptr_t, void*, ptr, {NULL})
