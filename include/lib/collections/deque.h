#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

extern const int DEQUE_MIN_CAPACITY;

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

#define IMPL_DEQUE(DTYPE, TYPE, TNAME, TDEF)                                                                      \
	DTYPE deque_##TNAME##_create(void) { return deque_##TNAME##_create_with_capacity(DEQUE_MIN_CAPACITY); }       \
                                                                                                                  \
	DTYPE deque_##TNAME##_create_with_capacity(size_t capacity) {                                                 \
		DTYPE v = {.head = 0, .tail = 0, .size = 0, .buffer = NULL, .capacity = capacity};                        \
		return v;                                                                                                 \
	}                                                                                                             \
                                                                                                                  \
	void deque_##TNAME##_destroy(DTYPE* v) {                                                                      \
		if (v->buffer != NULL) free(v->buffer);                                                                   \
                                                                                                                  \
		v->head = 0;                                                                                              \
		v->tail = 0;                                                                                              \
		v->size = 0;                                                                                              \
		v->buffer = NULL;                                                                                         \
		v->capacity = 0;                                                                                          \
	}                                                                                                             \
                                                                                                                  \
	bool deque_##TNAME##_grow(DTYPE* v) {                                                                         \
		size_t newSize = v->capacity * 3 / 2;                                                                     \
                                                                                                                  \
		TYPE* newBuffer = (TYPE*)realloc(v->buffer, newSize * sizeof(TYPE));                                      \
		if (newBuffer == NULL) return false;                                                                      \
                                                                                                                  \
		size_t sizeDelta = newSize - v->capacity;                                                                 \
		memmove(newBuffer + v->head + sizeDelta, newBuffer + v->head, (v->capacity - v->head) * sizeof(TYPE));    \
                                                                                                                  \
		if (v->head <= v->tail) {                                                                                 \
			v->tail += sizeDelta;                                                                                 \
		}                                                                                                         \
		v->head += sizeDelta;                                                                                     \
                                                                                                                  \
		v->buffer = newBuffer;                                                                                    \
		v->capacity = newSize;                                                                                    \
                                                                                                                  \
		return true;                                                                                              \
	}                                                                                                             \
                                                                                                                  \
	bool deque_##TNAME##_shrink(DTYPE* v) {                                                                       \
		size_t newSize = v->capacity * 2 / 3;                                                                     \
                                                                                                                  \
		TYPE* newBuffer = (TYPE*)malloc(newSize * sizeof(TYPE));                                                  \
		if (newBuffer == NULL) return false;                                                                      \
                                                                                                                  \
		if (v->head <= v->tail) {                                                                                 \
			/* If the vector layout is linear (head hasn't wrapped yet), we copy it as is */                      \
			memcpy(newBuffer, v->buffer + v->head, v->size * sizeof(TYPE));                                       \
                                                                                                                  \
			v->head = 0;                                                                                          \
			v->tail = v->size - 1;                                                                                \
		} else {                                                                                                  \
			/* If it had wrapped, we copy the two halves of the vector separately.*/                              \
			size_t sizeDelta = v->capacity - newSize;                                                             \
                                                                                                                  \
			memcpy(newBuffer + v->head - sizeDelta, v->buffer + v->head, (v->capacity - v->head) * sizeof(TYPE)); \
			memcpy(newBuffer, v->buffer, (v->tail + 1) * sizeof(TYPE));                                           \
                                                                                                                  \
			v->head -= sizeDelta;                                                                                 \
		}                                                                                                         \
                                                                                                                  \
		free(v->buffer);                                                                                          \
                                                                                                                  \
		v->buffer = newBuffer;                                                                                    \
		v->capacity = newSize;                                                                                    \
                                                                                                                  \
		return true;                                                                                              \
	}                                                                                                             \
                                                                                                                  \
	inline static bool deque_##TNAME##_needs_shrink(DTYPE* v) {                                                   \
		return v->size > DEQUE_MIN_CAPACITY && v->size <= v->capacity * 4 / 9;                                    \
	}                                                                                                             \
                                                                                                                  \
	inline static bool deque_##TNAME##_init(DTYPE* v) {                                                           \
		if (v->capacity == 0) return false; /* deque is destroyed */                                              \
		if (v->buffer != NULL) return true; /* deque is initialized */                                            \
                                                                                                                  \
		v->buffer = calloc(v->capacity, sizeof(TYPE));                                                            \
		return v->buffer != NULL;                                                                                 \
	}                                                                                                             \
                                                                                                                  \
	bool deque_##TNAME##_push_front(DTYPE* v, TYPE value) {                                                       \
		bool initialized = deque_##TNAME##_init(v);                                                               \
		if (!initialized) return false;                                                                           \
                                                                                                                  \
		if (v->size == v->capacity) {                                                                             \
			bool grown = deque_##TNAME##_grow(v);                                                                 \
			if (!grown) return false;                                                                             \
		}                                                                                                         \
                                                                                                                  \
		v->head = v->head == 0 ? v->capacity - 1 : v->head - 1;                                                   \
		v->buffer[v->head] = value;                                                                               \
		v->size++;                                                                                                \
                                                                                                                  \
		if (v->size == 1) v->tail = v->head;                                                                      \
                                                                                                                  \
		return true;                                                                                              \
	}                                                                                                             \
                                                                                                                  \
	bool deque_##TNAME##_push_back(DTYPE* v, TYPE value) {                                                        \
		bool initialized = deque_##TNAME##_init(v);                                                               \
		if (!initialized) return false;                                                                           \
                                                                                                                  \
		if (v->size == v->capacity) {                                                                             \
			bool grown = deque_##TNAME##_grow(v);                                                                 \
			if (!grown) return false;                                                                             \
		}                                                                                                         \
                                                                                                                  \
		v->tail = (v->tail + 1) % v->capacity;                                                                    \
		v->buffer[v->tail] = value;                                                                               \
		v->size++;                                                                                                \
                                                                                                                  \
		if (v->size == 1) v->head = v->tail;                                                                      \
                                                                                                                  \
		return true;                                                                                              \
	}                                                                                                             \
                                                                                                                  \
	TYPE deque_##TNAME##_pop_front(DTYPE* v) {                                                                    \
		if (v->size == 0) {                                                                                       \
			return TDEF;                                                                                          \
		} else if (deque_##TNAME##_needs_shrink(v)) {                                                             \
			bool shrunk = deque_##TNAME##_shrink(v);                                                              \
			if (!shrunk) return TDEF;                                                                             \
		}                                                                                                         \
                                                                                                                  \
		TYPE value = v->buffer[v->head];                                                                          \
		v->head = (v->head + 1) % v->capacity;                                                                    \
		v->size--;                                                                                                \
                                                                                                                  \
		return value;                                                                                             \
	}                                                                                                             \
                                                                                                                  \
	TYPE deque_##TNAME##_pop_back(DTYPE* v) {                                                                     \
		if (v->size == 0) {                                                                                       \
			return TDEF;                                                                                          \
		} else if (deque_##TNAME##_needs_shrink(v)) {                                                             \
			bool shrunk = deque_##TNAME##_shrink(v);                                                              \
			if (!shrunk) return TDEF;                                                                             \
		}                                                                                                         \
                                                                                                                  \
		TYPE value = v->buffer[v->tail];                                                                          \
		v->tail = v->tail == 0 ? v->capacity - 1 : v->tail - 1;                                                   \
		v->size--;                                                                                                \
                                                                                                                  \
		return value;                                                                                             \
	}                                                                                                             \
                                                                                                                  \
	TYPE deque_##TNAME##_peek_front(DTYPE* v) {                                                                   \
		if (v->size == 0) return TDEF;                                                                            \
		return v->buffer[v->head];                                                                                \
	}                                                                                                             \
                                                                                                                  \
	TYPE deque_##TNAME##_peek_back(DTYPE* v) {                                                                    \
		if (v->size == 0) return TDEF;                                                                            \
		return v->buffer[v->tail];                                                                                \
	}                                                                                                             \
                                                                                                                  \
	TYPE deque_##TNAME##_get(DTYPE* d, size_t index) {                                                            \
		if (index >= d->size) return TDEF;                                                                        \
		return d->buffer[(d->head + index) % d->capacity];                                                        \
	}

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
