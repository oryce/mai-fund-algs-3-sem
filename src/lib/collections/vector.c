#include "lib/collections/vector.h"

#include <stdlib.h>

const size_t VECTOR_MIN_CAPACITY = 4;

#define SIMPLE_COMPARATOR(TYPE)                                           \
	inline static int comparator_##TYPE(const void* p1, const void* p2) { \
		if (p1 == NULL || p2 == NULL) return 0;                           \
                                                                          \
		TYPE t1 = *(TYPE*)p1;                                             \
		TYPE t2 = *(TYPE*)p2;                                             \
                                                                          \
		if (t1 < t2)                                                      \
			return -1;                                                    \
		else if (t1 > t2)                                                 \
			return 1;                                                     \
		else                                                              \
			return 0;                                                     \
	}

SIMPLE_COMPARATOR(int64_t)
SIMPLE_COMPARATOR(int32_t)
SIMPLE_COMPARATOR(int16_t)
SIMPLE_COMPARATOR(int8_t)
SIMPLE_COMPARATOR(uint64_t)
SIMPLE_COMPARATOR(uint32_t)
SIMPLE_COMPARATOR(uint16_t)
SIMPLE_COMPARATOR(uint8_t)
SIMPLE_COMPARATOR(float)
SIMPLE_COMPARATOR(double)

IMPL_VECTOR(vector_i64_t, int64_t, i64, {&comparator_int64_t})
IMPL_VECTOR(vector_i32_t, int32_t, i32, {&comparator_int32_t})
IMPL_VECTOR(vector_i16_t, int16_t, i16, {&comparator_int16_t})
IMPL_VECTOR(vector_i8_t, int8_t, i8, {&comparator_int8_t})
IMPL_VECTOR(vector_u64_t, uint64_t, u64, {&comparator_uint64_t})
IMPL_VECTOR(vector_u32_t, uint32_t, u32, {&comparator_uint32_t})
IMPL_VECTOR(vector_u16_t, uint16_t, u16, {&comparator_uint16_t})
IMPL_VECTOR(vector_u8_t, uint8_t, u8, {&comparator_uint8_t})
IMPL_VECTOR(vector_flt_t, float, flt, {&comparator_float})
IMPL_VECTOR(vector_dbl_t, double, dbl, {&comparator_double})
IMPL_VECTOR(vector_ptr_t, void*, ptr, {NULL})
