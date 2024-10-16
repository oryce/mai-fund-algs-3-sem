#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

extern const int DEQUE_MIN_CAPACITY;

#define DEFINE_DEQUE(DEQUE_T, TYPE_T, TYPE)                                  \
	typedef struct deque_##TYPE {                                            \
		TYPE_T* buffer;                                                      \
		size_t head;                                                         \
		size_t tail;                                                         \
		size_t capacity;                                                     \
		size_t size;                                                         \
	} deque_##TYPE##_t;                                                      \
                                                                             \
	DEQUE_T deque_##TYPE##_create(void);                                     \
                                                                             \
	DEQUE_T deque_##TYPE##_create_with_capacity(size_t capacity);            \
                                                                             \
	void deque_##TYPE##_destroy(DEQUE_T*);                                   \
                                                                             \
	bool deque_##TYPE##_push_front(DEQUE_T*, TYPE_T value);                  \
                                                                             \
	bool deque_##TYPE##_push_back(DEQUE_T*, TYPE_T value);                   \
                                                                             \
	bool deque_##TYPE##_pop_front(DEQUE_T*, TYPE_T* out);                    \
                                                                             \
	bool deque_##TYPE##_pop_back(DEQUE_T*, TYPE_T* out);                     \
                                                                             \
	const TYPE_T* deque_##TYPE##_peek_front(DEQUE_T*);                       \
                                                                             \
	const TYPE_T* deque_##TYPE##_peek_back(DEQUE_T*);                        \
                                                                             \
	inline static size_t deque_##TYPE##_size(DEQUE_T* d) { return d->size; } \
                                                                             \
	inline static bool deque_##TYPE##_is_empty(DEQUE_T* d) { return d->size == 0; }

#define IMPL_DEQUE(DEQUE_T, TYPE_T, TYPE)                                                                           \
	DEQUE_T deque_##TYPE##_create(void) { return deque_##TYPE##_create_with_capacity(DEQUE_MIN_CAPACITY); }         \
                                                                                                                    \
	DEQUE_T deque_##TYPE##_create_with_capacity(size_t capacity) {                                                  \
		DEQUE_T v = {.head = 0, .tail = 0, .size = 0, .buffer = NULL, .capacity = capacity};                        \
		return v;                                                                                                   \
	}                                                                                                               \
                                                                                                                    \
	void deque_##TYPE##_destroy(DEQUE_T* v) {                                                                       \
		if (v->buffer != NULL) free(v->buffer);                                                                     \
                                                                                                                    \
		v->head = 0;                                                                                                \
		v->tail = 0;                                                                                                \
		v->size = 0;                                                                                                \
		v->buffer = NULL;                                                                                           \
		v->capacity = 0;                                                                                            \
	}                                                                                                               \
                                                                                                                    \
	bool deque_##TYPE##_grow(DEQUE_T* v) {                                                                          \
		size_t newSize = v->capacity * 3 / 2;                                                                       \
                                                                                                                    \
		TYPE_T* newBuffer = (TYPE_T*)realloc(v->buffer, newSize * sizeof(TYPE_T));                                  \
		if (newBuffer == NULL) return false;                                                                        \
                                                                                                                    \
		size_t sizeDelta = newSize - v->capacity;                                                                   \
		memmove(newBuffer + v->head + sizeDelta, newBuffer + v->head, (v->capacity - v->head) * sizeof(TYPE_T));    \
                                                                                                                    \
		if (v->head <= v->tail) {                                                                                   \
			v->tail += sizeDelta;                                                                                   \
		}                                                                                                           \
		v->head += sizeDelta;                                                                                       \
                                                                                                                    \
		v->buffer = newBuffer;                                                                                      \
		v->capacity = newSize;                                                                                      \
                                                                                                                    \
		return true;                                                                                                \
	}                                                                                                               \
                                                                                                                    \
	bool deque_##TYPE##_shrink(DEQUE_T* v) {                                                                        \
		size_t newSize = v->capacity * 2 / 3;                                                                       \
                                                                                                                    \
		TYPE_T* newBuffer = (TYPE_T*)malloc(newSize * sizeof(TYPE_T));                                              \
		if (newBuffer == NULL) return false;                                                                        \
                                                                                                                    \
		if (v->head <= v->tail) {                                                                                   \
			/* If the vector layout is linear (head hasn't wrapped yet), we copy it as is */                        \
			memcpy(newBuffer, v->buffer + v->head, v->size * sizeof(TYPE_T));                                       \
                                                                                                                    \
			v->head = 0;                                                                                            \
			v->tail = v->size - 1;                                                                                  \
		} else {                                                                                                    \
			/* If it had wrapped, we copy the two halves of the vector separately.*/                                \
			size_t sizeDelta = v->capacity - newSize;                                                               \
                                                                                                                    \
			memcpy(newBuffer + v->head - sizeDelta, v->buffer + v->head, (v->capacity - v->head) * sizeof(TYPE_T)); \
			memcpy(newBuffer, v->buffer, (v->tail + 1) * sizeof(TYPE_T));                                           \
                                                                                                                    \
			v->head -= sizeDelta;                                                                                   \
		}                                                                                                           \
                                                                                                                    \
		free(v->buffer);                                                                                            \
                                                                                                                    \
		v->buffer = newBuffer;                                                                                      \
		v->capacity = newSize;                                                                                      \
                                                                                                                    \
		return true;                                                                                                \
	}                                                                                                               \
                                                                                                                    \
	inline static bool deque_##TYPE##_needs_shrink(DEQUE_T* v) {                                                    \
		return v->size > DEQUE_MIN_CAPACITY && v->size <= v->capacity * 4 / 9;                                      \
	}                                                                                                               \
                                                                                                                    \
	inline static bool deque_##TYPE##_init(DEQUE_T* v) {                                                            \
		if (v->capacity == 0) return false; /* deque is destroyed */                                                \
		if (v->buffer != NULL) return true; /* deque is initialized */                                              \
                                                                                                                    \
		v->buffer = calloc(v->capacity, sizeof(TYPE_T));                                                            \
		return v->buffer != NULL;                                                                                   \
	}                                                                                                               \
                                                                                                                    \
	bool deque_##TYPE##_push_front(DEQUE_T* v, TYPE_T value) {                                                      \
		bool initialized = deque_##TYPE##_init(v);                                                                  \
		if (!initialized) return false;                                                                             \
                                                                                                                    \
		if (v->size == v->capacity) {                                                                               \
			bool grown = deque_##TYPE##_grow(v);                                                                    \
			if (!grown) return false;                                                                               \
		}                                                                                                           \
                                                                                                                    \
		v->head = v->head == 0 ? v->capacity - 1 : v->head - 1;                                                     \
		v->buffer[v->head] = value;                                                                                 \
		v->size++;                                                                                                  \
                                                                                                                    \
		if (v->size == 1) v->tail = v->head;                                                                        \
                                                                                                                    \
		return true;                                                                                                \
	}                                                                                                               \
                                                                                                                    \
	bool deque_##TYPE##_push_back(DEQUE_T* v, TYPE_T value) {                                                       \
		bool initialized = deque_##TYPE##_init(v);                                                                  \
		if (!initialized) return false;                                                                             \
                                                                                                                    \
		if (v->size == v->capacity) {                                                                               \
			bool grown = deque_##TYPE##_grow(v);                                                                    \
			if (!grown) return false;                                                                               \
		}                                                                                                           \
                                                                                                                    \
		v->tail = (v->tail + 1) % v->capacity;                                                                      \
		v->buffer[v->tail] = value;                                                                                 \
		v->size++;                                                                                                  \
                                                                                                                    \
		if (v->size == 1) v->head = v->tail;                                                                        \
                                                                                                                    \
		return true;                                                                                                \
	}                                                                                                               \
                                                                                                                    \
	bool deque_##TYPE##_pop_front(DEQUE_T* v, TYPE_T* out) {                                                        \
		if (v->size == 0) {                                                                                         \
			return false;                                                                                           \
		} else if (deque_##TYPE##_needs_shrink(v)) {                                                                \
			bool shrunk = deque_##TYPE##_shrink(v);                                                                 \
			if (!shrunk) return false;                                                                              \
		}                                                                                                           \
                                                                                                                    \
		TYPE_T value = v->buffer[v->head];                                                                          \
		v->head = (v->head + 1) % v->capacity;                                                                      \
		v->size--;                                                                                                  \
                                                                                                                    \
		*out = value;                                                                                               \
		return true;                                                                                                \
	}                                                                                                               \
                                                                                                                    \
	bool deque_##TYPE##_pop_back(DEQUE_T* v, TYPE_T* out) {                                                         \
		if (out == NULL || v->size == 0) {                                                                          \
			return false;                                                                                           \
		} else if (deque_##TYPE##_needs_shrink(v)) {                                                                \
			bool shrunk = deque_##TYPE##_shrink(v);                                                                 \
			if (!shrunk) return false;                                                                              \
		}                                                                                                           \
                                                                                                                    \
		TYPE_T value = v->buffer[v->tail];                                                                          \
		v->tail = v->tail == 0 ? v->capacity - 1 : v->tail - 1;                                                     \
		v->size--;                                                                                                  \
                                                                                                                    \
		*out = value;                                                                                               \
		return true;                                                                                                \
	}                                                                                                               \
                                                                                                                    \
	const TYPE_T* deque_##TYPE##_peek_front(DEQUE_T* v) {                                                           \
		if (v->size == 0) return (const TYPE_T*)NULL;                                                               \
		return (const TYPE_T*)&v->buffer[v->head];                                                                  \
	}                                                                                                               \
                                                                                                                    \
	const TYPE_T* deque_##TYPE##_peek_back(DEQUE_T* v) {                                                            \
		if (v->size == 0) return (const TYPE_T*)NULL;                                                               \
		return (const TYPE_T*)&v->buffer[v->tail];                                                                  \
	}

DEFINE_DEQUE(deque_i64_t, int64_t, i64)
DEFINE_DEQUE(deque_i32_t, int32_t, i32)
DEFINE_DEQUE(deque_i16_t, int16_t, i16)
DEFINE_DEQUE(deque_i8_t, int8_t, i8)
DEFINE_DEQUE(deque_u64_t, uint64_t, u64)
DEFINE_DEQUE(deque_u32_t, uint32_t, u32)
DEFINE_DEQUE(deque_u16_t, uint16_t, u16)
DEFINE_DEQUE(deque_u8_t, uint8_t, u8)
DEFINE_DEQUE(deque_idx_t, size_t, idx)
DEFINE_DEQUE(deque_flt_t, float, flt)
DEFINE_DEQUE(deque_dbl_t, double, dbl)
DEFINE_DEQUE(deque_ptr_t, void*, ptr)
