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
	} DEQUE_T;                                                               \
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
	const TYPE_T* deque_##TYPE##_get(DEQUE_T* d, size_t idx);                \
                                                                             \
	inline static size_t deque_##TYPE##_size(DEQUE_T* d) { return d->size; } \
                                                                             \
	inline static bool deque_##TYPE##_is_empty(DEQUE_T* d) { return d->size == 0; }

#define IMPL_DEQUE(DEQUE_T, TYPE_T, TYPE)                                                                           \
	DEQUE_T deque_##TYPE##_create(void) { return deque_##TYPE##_create_with_capacity(DEQUE_MIN_CAPACITY); }         \
                                                                                                                    \
	DEQUE_T deque_##TYPE##_create_with_capacity(size_t capacity) {                                                  \
		DEQUE_T d = {.head = 0, .tail = 0, .size = 0, .buffer = NULL, .capacity = capacity};                        \
		return d;                                                                                                   \
	}                                                                                                               \
                                                                                                                    \
	void deque_##TYPE##_destroy(DEQUE_T* d) {                                                                       \
		if (d->buffer != NULL) free(d->buffer);                                                                     \
                                                                                                                    \
		d->head = 0;                                                                                                \
		d->tail = 0;                                                                                                \
		d->size = 0;                                                                                                \
		d->buffer = NULL;                                                                                           \
		d->capacity = 0;                                                                                            \
	}                                                                                                               \
                                                                                                                    \
	bool deque_##TYPE##_grow(DEQUE_T* d) {                                                                          \
		size_t newSize = d->capacity * 3 / 2;                                                                       \
                                                                                                                    \
		TYPE_T* newBuffer = (TYPE_T*)realloc(d->buffer, newSize * sizeof(TYPE_T));                                  \
		if (newBuffer == NULL) return false;                                                                        \
                                                                                                                    \
		size_t sizeDelta = newSize - d->capacity;                                                                   \
		memmove(newBuffer + d->head + sizeDelta, newBuffer + d->head, (d->capacity - d->head) * sizeof(TYPE_T));    \
                                                                                                                    \
		if (d->head <= d->tail) {                                                                                   \
			d->tail += sizeDelta;                                                                                   \
		}                                                                                                           \
		d->head += sizeDelta;                                                                                       \
                                                                                                                    \
		d->buffer = newBuffer;                                                                                      \
		d->capacity = newSize;                                                                                      \
                                                                                                                    \
		return true;                                                                                                \
	}                                                                                                               \
                                                                                                                    \
	bool deque_##TYPE##_shrink(DEQUE_T* d) {                                                                        \
		size_t newSize = d->capacity * 2 / 3;                                                                       \
                                                                                                                    \
		TYPE_T* newBuffer = (TYPE_T*)calloc(newSize, sizeof(TYPE_T));                                               \
		if (newBuffer == NULL) return false;                                                                        \
                                                                                                                    \
		if (d->head <= d->tail) {                                                                                   \
			/* If the deque layout is linear (head hasn't wrapped yet), we copy it as is */                         \
			memcpy(newBuffer, d->buffer + d->head, d->size * sizeof(TYPE_T));                                       \
			d->head = 0;                                                                                            \
			d->tail = d->size - 1;                                                                                  \
		} else {                                                                                                    \
			/* If it had wrapped, we copy the multiple halves of the deque separately.*/                            \
			size_t sizeDelta = d->capacity - newSize;                                                               \
                                                                                                                    \
			memcpy(newBuffer + d->head - sizeDelta, d->buffer + d->head, (d->capacity - d->head) * sizeof(TYPE_T)); \
			memcpy(newBuffer, d->buffer, (d->tail + 1) * sizeof(TYPE_T));                                           \
                                                                                                                    \
			d->head -= sizeDelta;                                                                                   \
		}                                                                                                           \
                                                                                                                    \
		free(d->buffer);                                                                                            \
		d->buffer = newBuffer;                                                                                      \
		d->capacity = newSize;                                                                                      \
                                                                                                                    \
		return true;                                                                                                \
	}                                                                                                               \
                                                                                                                    \
	inline static bool deque_##TYPE##_needs_shrink(DEQUE_T* d) {                                                    \
		return d->size > DEQUE_MIN_CAPACITY && d->size <= d->capacity * 4 / 9;                                      \
	}                                                                                                               \
                                                                                                                    \
	inline static bool deque_##TYPE##_init(DEQUE_T* d) {                                                            \
		if (d->capacity == 0) return false; /* deque is destroyed */                                                \
		if (d->buffer != NULL) return true; /* deque is initialized */                                              \
                                                                                                                    \
		d->buffer = calloc(d->capacity, sizeof(TYPE_T));                                                            \
		return d->buffer != NULL;                                                                                   \
	}                                                                                                               \
                                                                                                                    \
	bool deque_##TYPE##_push_front(DEQUE_T* d, TYPE_T value) {                                                      \
		bool initialized = deque_##TYPE##_init(d);                                                                  \
		if (!initialized) return false;                                                                             \
                                                                                                                    \
		if (d->size == d->capacity) {                                                                               \
			bool grown = deque_##TYPE##_grow(d);                                                                    \
			if (!grown) return false;                                                                               \
		}                                                                                                           \
                                                                                                                    \
		d->head = d->head == 0 ? d->capacity - 1 : d->head - 1;                                                     \
		d->buffer[d->head] = value;                                                                                 \
		d->size++;                                                                                                  \
                                                                                                                    \
		if (d->size == 1) d->tail = d->head;                                                                        \
                                                                                                                    \
		return true;                                                                                                \
	}                                                                                                               \
                                                                                                                    \
	bool deque_##TYPE##_push_back(DEQUE_T* d, TYPE_T value) {                                                       \
		bool initialized = deque_##TYPE##_init(d);                                                                  \
		if (!initialized) return false;                                                                             \
                                                                                                                    \
		if (d->size == d->capacity) {                                                                               \
			bool grown = deque_##TYPE##_grow(d);                                                                    \
			if (!grown) return false;                                                                               \
		}                                                                                                           \
                                                                                                                    \
		d->tail = (d->tail + 1) % d->capacity;                                                                      \
		d->buffer[d->tail] = value;                                                                                 \
		d->size++;                                                                                                  \
                                                                                                                    \
		if (d->size == 1) d->head = d->tail;                                                                        \
                                                                                                                    \
		return true;                                                                                                \
	}                                                                                                               \
                                                                                                                    \
	bool deque_##TYPE##_pop_front(DEQUE_T* d, TYPE_T* out) {                                                        \
		if (d->size == 0) {                                                                                         \
			return false;                                                                                           \
		} else if (deque_##TYPE##_needs_shrink(d)) {                                                                \
			bool shrunk = deque_##TYPE##_shrink(d);                                                                 \
			if (!shrunk) return false;                                                                              \
		}                                                                                                           \
                                                                                                                    \
		TYPE_T value = d->buffer[d->head];                                                                          \
		d->head = (d->head + 1) % d->capacity;                                                                      \
		d->size--;                                                                                                  \
                                                                                                                    \
		*out = value;                                                                                               \
		return true;                                                                                                \
	}                                                                                                               \
                                                                                                                    \
	bool deque_##TYPE##_pop_back(DEQUE_T* d, TYPE_T* out) {                                                         \
		if (out == NULL || d->size == 0) {                                                                          \
			return false;                                                                                           \
		} else if (deque_##TYPE##_needs_shrink(d)) {                                                                \
			bool shrunk = deque_##TYPE##_shrink(d);                                                                 \
			if (!shrunk) return false;                                                                              \
		}                                                                                                           \
                                                                                                                    \
		TYPE_T value = d->buffer[d->tail];                                                                          \
		d->tail = d->tail == 0 ? d->capacity - 1 : d->tail - 1;                                                     \
		d->size--;                                                                                                  \
                                                                                                                    \
		*out = value;                                                                                               \
		return true;                                                                                                \
	}                                                                                                               \
                                                                                                                    \
	const TYPE_T* deque_##TYPE##_peek_front(DEQUE_T* d) {                                                           \
		if (d->size == 0) return (const TYPE_T*)NULL;                                                               \
		return (const TYPE_T*)&d->buffer[d->head];                                                                  \
	}                                                                                                               \
                                                                                                                    \
	const TYPE_T* deque_##TYPE##_peek_back(DEQUE_T* d) {                                                            \
		if (d->size == 0) return (const TYPE_T*)NULL;                                                               \
		return (const TYPE_T*)&d->buffer[d->tail];                                                                  \
	}                                                                                                               \
                                                                                                                    \
	const TYPE_T* deque_##TYPE##_get(DEQUE_T* d, size_t idx) {                                                      \
		if (idx >= d->size) return (const TYPE_T*)NULL;                                                             \
		return (const TYPE_T*)&d->buffer[(d->head + idx) % d->capacity];                                            \
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
