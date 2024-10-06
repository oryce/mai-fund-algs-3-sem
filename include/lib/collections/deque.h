#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define DEFINE_DEQUE(TYPE, TNAME)                                                              \
	typedef struct deque_##TNAME {                                                             \
		TYPE* buffer;                                                                          \
		size_t head;                                                                           \
		size_t tail;                                                                           \
		size_t capacity;                                                                       \
		size_t size;                                                                           \
	} deque_##TNAME##_t;                                                                       \
                                                                                               \
	deque_##TNAME##_t deque_##TNAME##_create(void);                                            \
                                                                                               \
	deque_##TNAME##_t deque_##TNAME##_create_with_capacity(size_t capacity);                   \
                                                                                               \
	void deque_##TNAME##_destroy(deque_##TNAME##_t*);                                          \
                                                                                               \
	bool deque_##TNAME##_push_front(deque_##TNAME##_t*, TYPE value);                           \
                                                                                               \
	bool deque_##TNAME##_push_back(deque_##TNAME##_t*, TYPE value);                            \
                                                                                               \
	TYPE deque_##TNAME##_pop_front(deque_##TNAME##_t*);                                        \
                                                                                               \
	TYPE deque_##TNAME##_pop_back(deque_##TNAME##_t*);                                         \
                                                                                               \
	TYPE deque_##TNAME##_peek_front(deque_##TNAME##_t*);                                       \
                                                                                               \
	TYPE deque_##TNAME##_peek_back(deque_##TNAME##_t*);                                        \
                                                                                               \
	inline static size_t deque_##TNAME##_size(deque_##TNAME##_t* d) { return d->size; }        \
                                                                                               \
	inline static bool deque_##TNAME##_is_empty(deque_##TNAME##_t* d) { return d->size == 0; } \
                                                                                               \
	TYPE deque_##TNAME##_get(deque_##TNAME##_t*, size_t index);

DEFINE_DEQUE(int8_t, i8)
DEFINE_DEQUE(int16_t, i16)
DEFINE_DEQUE(int32_t, i32)
DEFINE_DEQUE(int64_t, i64)
DEFINE_DEQUE(uint8_t, u8)
DEFINE_DEQUE(uint16_t, u16)
DEFINE_DEQUE(uint32_t, u32)
DEFINE_DEQUE(uint64_t, u64)
DEFINE_DEQUE(size_t, idx)
DEFINE_DEQUE(float, flt)
DEFINE_DEQUE(double, dbl)
DEFINE_DEQUE(void*, ptr)
