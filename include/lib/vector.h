#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define DEFINE_VECTOR(TYPE, TNAME)                                               \
	typedef struct vector_##TNAME {                                              \
		TYPE* buffer;                                                            \
		size_t size;                                                             \
		size_t capacity;                                                         \
	} vector_##TNAME##_t;                                                        \
                                                                                 \
	vector_##TNAME##_t vector_##TNAME##_create(void);                            \
                                                                                 \
	vector_##TNAME##_t vector_##TNAME##_create_with_capacity(size_t capacity);   \
                                                                                 \
	void vector_##TNAME##_destroy(vector_##TNAME##_t*);                          \
                                                                                 \
	bool vector_##TNAME##_ensure_capacity(vector_##TNAME##_t*, size_t capacity); \
                                                                                 \
	bool vector_##TNAME##_push_back(vector_##TNAME##_t*, TYPE value);            \
                                                                                 \
	TYPE vector_##TNAME##_pop_back(vector_##TNAME##_t*);                         \
                                                                                 \
	TYPE vector_##TNAME##_get(vector_##TNAME##_t*, size_t index);                \
                                                                                 \
	TYPE vector_##TNAME##_remove(vector_##TNAME##_t*, size_t index);             \
                                                                                 \
	TYPE vector_##TNAME##_remove_item(vector_##TNAME##_t*, TYPE value);          \
                                                                                 \
	bool vector_##TNAME##_clear(vector_##TNAME##_t*);                            \
                                                                                 \
	size_t vector_##TNAME##_index_of(vector_##TNAME##_t*, TYPE value);           \
                                                                                 \
	size_t vector_##TNAME##_size(vector_##TNAME##_t*);                           \
                                                                                 \
	bool vector_##TNAME##_is_empty(vector_##TNAME##_t*);                         \
                                                                                 \
	TYPE* vector_##TNAME##_to_array(vector_##TNAME##_t*);                        \
                                                                                 \
	bool vector_##TNAME##_swap(vector_##TNAME##_t*, size_t i1, size_t i2);

DEFINE_VECTOR(int8_t, i8)
DEFINE_VECTOR(int16_t, i16)
DEFINE_VECTOR(int32_t, i32)
DEFINE_VECTOR(int64_t, i64)
DEFINE_VECTOR(uint8_t, u8)
DEFINE_VECTOR(uint16_t, u16)
DEFINE_VECTOR(uint32_t, u32)
DEFINE_VECTOR(uint64_t, u64)
DEFINE_VECTOR(float, flt)
DEFINE_VECTOR(double, dbl)
DEFINE_VECTOR(void*, ptr)
