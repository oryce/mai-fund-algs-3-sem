#include "lib/collections/deque.h"

#include <float.h>

const int DEQUE_MIN_CAPACITY = 4;

IMPL_DEQUE(deque_i64_t, int64_t, i64)
IMPL_DEQUE(deque_i32_t, int32_t, i32)
IMPL_DEQUE(deque_i16_t, int16_t, i16)
IMPL_DEQUE(deque_i8_t, int8_t, i8)
IMPL_DEQUE(deque_u64_t, uint64_t, u64)
IMPL_DEQUE(deque_u32_t, uint32_t, u32)
IMPL_DEQUE(deque_u16_t, uint16_t, u16)
IMPL_DEQUE(deque_u8_t, uint8_t, u8)
IMPL_DEQUE(deque_idx_t, size_t, idx)
IMPL_DEQUE(deque_flt_t, float, flt)
IMPL_DEQUE(deque_dbl_t, double, dbl)
IMPL_DEQUE(deque_ptr_t, void*, ptr)
