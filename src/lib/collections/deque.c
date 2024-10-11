#include "lib/collections/deque.h"

#include <float.h>

const int DEQUE_MIN_CAPACITY = 4;

IMPL_DEQUE(deque_i64_t, int64_t, i64, INT64_MAX)
IMPL_DEQUE(deque_i32_t, int32_t, i32, INT32_MAX)
IMPL_DEQUE(deque_i16_t, int16_t, i16, INT16_MAX)
IMPL_DEQUE(deque_i8_t, int8_t, i8, INT8_MAX)
IMPL_DEQUE(deque_u64_t, uint64_t, u64, UINT64_MAX)
IMPL_DEQUE(deque_u32_t, uint32_t, u32, UINT32_MAX)
IMPL_DEQUE(deque_u16_t, uint16_t, u16, UINT16_MAX)
IMPL_DEQUE(deque_u8_t, uint8_t, u8, UINT8_MAX)
IMPL_DEQUE(deque_idx_t, size_t, idx, SIZE_MAX)
IMPL_DEQUE(deque_flt_t, float, flt, FLT_MAX)
IMPL_DEQUE(deque_dbl_t, double, dbl, DBL_MAX)
IMPL_DEQUE(deque_ptr_t, void*, ptr, NULL)
