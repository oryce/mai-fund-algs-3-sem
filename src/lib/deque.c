#include "lib/deque.h"

#include <float.h>
#include <stdlib.h>
#include <string.h>

const int DEQUE_MIN_CAPACITY = 4;

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
