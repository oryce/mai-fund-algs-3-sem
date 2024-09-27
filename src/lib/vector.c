#include "lib/vector.h"

#include <float.h>
#include <stdlib.h>
#include <string.h>

#include "lib/deque.h"

const size_t VECTOR_MIN_CAPACITY = 4;

const int VECTOR_SORTABLE = 1 << 0;

#define IMPL_VECTOR(VTYPE, TYPE, TNAME, TDEF, FLAGS)                                                           \
	bool vector_##TNAME##_resize(VTYPE* v, size_t capacity) {                                                  \
		TYPE* newBuffer = realloc(v->buffer, capacity * sizeof(TYPE));                                         \
		if (newBuffer == NULL) return false;                                                                   \
                                                                                                               \
		v->buffer = newBuffer;                                                                                 \
		v->capacity = capacity;                                                                                \
		return true;                                                                                           \
	}                                                                                                          \
                                                                                                               \
	bool vector_##TNAME##_shrink_one(VTYPE* v) {                                                               \
		if ((v->size - 1) > VECTOR_MIN_CAPACITY && (v->size - 1) <= v->capacity * 4 / 9) {                     \
			bool resized = vector_##TNAME##_resize(v, v->capacity * 2 / 3);                                    \
			if (!resized) return false;                                                                        \
		}                                                                                                      \
                                                                                                               \
		v->size--;                                                                                             \
		return true;                                                                                           \
	}                                                                                                          \
                                                                                                               \
	VTYPE vector_##TNAME##_create(void) { return vector_##TNAME##_create_with_capacity(VECTOR_MIN_CAPACITY); } \
                                                                                                               \
	VTYPE vector_##TNAME##_create_with_capacity(size_t capacity) {                                             \
		/* allocate later, when a push occurs */                                                               \
		VTYPE v = {.buffer = NULL, .size = 0, .capacity = capacity, .flags = FLAGS};                           \
		return v;                                                                                              \
	}                                                                                                          \
                                                                                                               \
	void vector_##TNAME##_destroy(VTYPE* v) {                                                                  \
		if (v->buffer != NULL) free(v->buffer);                                                                \
                                                                                                               \
		v->buffer = NULL;                                                                                      \
		v->size = 0;                                                                                           \
		v->capacity = 0;                                                                                       \
	}                                                                                                          \
                                                                                                               \
	bool vector_##TNAME##_ensure_capacity(VTYPE* v, size_t capacity) {                                         \
		if (v->capacity >= capacity) return true;                                                              \
		return vector_##TNAME##_resize(v, capacity);                                                           \
	}                                                                                                          \
                                                                                                               \
	bool vector_##TNAME##_push_back(VTYPE* v, TYPE value) {                                                    \
		/* if the vec is not initialized, allocate the buffer */                                               \
		if (v->buffer == NULL) {                                                                               \
			if (v->capacity == 0) return false;                                                                \
                                                                                                               \
			v->buffer = calloc(v->capacity, sizeof(TYPE));                                                     \
			if (v->buffer == NULL) return false; /* allocation failed */                                       \
		}                                                                                                      \
                                                                                                               \
		if (v->size == v->capacity) {                                                                          \
			bool resized = vector_##TNAME##_ensure_capacity(v, v->capacity * 3 / 2);                           \
			if (!resized) return false;                                                                        \
		}                                                                                                      \
                                                                                                               \
		v->buffer[v->size++] = value;                                                                          \
		return true;                                                                                           \
	}                                                                                                          \
                                                                                                               \
	TYPE vector_##TNAME##_pop_back(VTYPE* v) {                                                                 \
		if (v->size == 0) return TDEF;                                                                         \
                                                                                                               \
		TYPE value = v->buffer[v->size - 1];                                                                   \
		if (!vector_##TNAME##_shrink_one(v)) return TDEF;                                                      \
                                                                                                               \
		return value;                                                                                          \
	}                                                                                                          \
                                                                                                               \
	TYPE vector_##TNAME##_get(VTYPE* v, size_t index) {                                                        \
		if (index >= v->size) return TDEF;                                                                     \
		return v->buffer[index];                                                                               \
	}                                                                                                          \
                                                                                                               \
	TYPE vector_##TNAME##_remove(VTYPE* v, size_t index) {                                                     \
		if (index >= v->size) return TDEF;                                                                     \
                                                                                                               \
		TYPE value = v->buffer[index];                                                                         \
		memmove(v->buffer + index, v->buffer + index + 1, (v->size - index - 1) * sizeof(TYPE));               \
                                                                                                               \
		if (!vector_##TNAME##_shrink_one(v)) return TDEF;                                                      \
                                                                                                               \
		return value;                                                                                          \
	}                                                                                                          \
                                                                                                               \
	TYPE vector_##TNAME##_remove_item(VTYPE* v, TYPE value) {                                                  \
		return vector_##TNAME##_remove(v, vector_##TNAME##_index_of(v, value));                                \
	}                                                                                                          \
                                                                                                               \
	bool vector_##TNAME##_clear(VTYPE* v) {                                                                    \
		if (!vector_##TNAME##_resize(v, 4)) return false;                                                      \
                                                                                                               \
		v->size = 0;                                                                                           \
		return true;                                                                                           \
	}                                                                                                          \
                                                                                                               \
	size_t vector_##TNAME##_index_of(VTYPE* v, TYPE value) {                                                   \
		for (size_t i = 0; i != v->size; i++) {                                                                \
			if (v->buffer[i] == value) return (ssize_t)i;                                                      \
		}                                                                                                      \
                                                                                                               \
		return SIZE_MAX;                                                                                       \
	}                                                                                                          \
                                                                                                               \
	bool vector_##TNAME##_swap(vector_##TNAME##_t* v, size_t i1, size_t i2) {                                  \
		if (i1 >= v->size || i2 >= v->size) return false;                                                      \
                                                                                                               \
		TYPE temp = v->buffer[i1];                                                                             \
		v->buffer[i1] = v->buffer[i2];                                                                         \
		v->buffer[i2] = temp;                                                                                  \
                                                                                                               \
		return true;                                                                                           \
	}                                                                                                          \
                                                                                                               \
	bool vector_##TNAME##_sort(vector_##TNAME##_t* v) {                                                        \
		if (!(v->flags & VECTOR_SORTABLE)) {                                                                   \
			return false;                                                                                      \
		}                                                                                                      \
                                                                                                               \
		bool result = true;                                                                                    \
		deque_idx_t lefts = deque_idx_create();  /* Left pair elements */                                      \
		deque_idx_t rights = deque_idx_create(); /* Right pair elements */                                     \
                                                                                                               \
		result = deque_idx_push_front(&lefts, 0);                                                              \
		if (!result) goto cleanup;                                                                             \
		result = deque_idx_push_front(&rights, v->size - 1);                                                   \
		if (!result) goto cleanup;                                                                             \
                                                                                                               \
		do {                                                                                                   \
			size_t l = deque_idx_pop_front(&lefts);                                                            \
			size_t r = deque_idx_pop_front(&rights);                                                           \
                                                                                                               \
			if (l == SIZE_MAX || r == SIZE_MAX) {                                                              \
				result = false;                                                                                \
				goto cleanup;                                                                                  \
			}                                                                                                  \
                                                                                                               \
			do {                                                                                               \
				TYPE pivot = vector_##TNAME##_get(v, (l + r) / 2);                                             \
				size_t i = l;                                                                                  \
				size_t j = r;                                                                                  \
                                                                                                               \
				do {                                                                                           \
					while (vector_##TNAME##_get(v, i) < pivot) i++;                                            \
					while (vector_##TNAME##_get(v, j) > pivot) j--;                                            \
                                                                                                               \
					if (i <= j) {                                                                              \
						vector_##TNAME##_swap(v, i, j);                                                        \
						++i;                                                                                   \
                                                                                                               \
						if (j == 0) break; /* Prevent underflow with size_t */                                 \
						--j;                                                                                   \
					}                                                                                          \
				} while (i <= j);                                                                              \
                                                                                                               \
				if (i < r) {                                                                                   \
					result = deque_idx_push_front(&lefts, i);                                                  \
					if (!result) goto cleanup;                                                                 \
					result = deque_idx_push_front(&rights, r);                                                 \
					if (!result) goto cleanup;                                                                 \
				}                                                                                              \
                                                                                                               \
				r = j;                                                                                         \
			} while (l < r);                                                                                   \
		} while (!deque_idx_is_empty(&lefts) && !deque_idx_is_empty(&rights));                                 \
                                                                                                               \
	cleanup:                                                                                                   \
		deque_idx_destroy(&lefts);                                                                             \
		deque_idx_destroy(&rights);                                                                            \
		return result;                                                                                         \
	}

IMPL_VECTOR(vector_i64_t, int64_t, i64, INT64_MAX, VECTOR_SORTABLE)
IMPL_VECTOR(vector_i32_t, int32_t, i32, INT32_MAX, VECTOR_SORTABLE)
IMPL_VECTOR(vector_i16_t, int16_t, i16, INT16_MAX, VECTOR_SORTABLE)
IMPL_VECTOR(vector_i8_t, int8_t, i8, INT8_MAX, VECTOR_SORTABLE)
IMPL_VECTOR(vector_u64_t, uint64_t, u64, UINT64_MAX, VECTOR_SORTABLE)
IMPL_VECTOR(vector_u32_t, uint32_t, u32, UINT32_MAX, VECTOR_SORTABLE)
IMPL_VECTOR(vector_u16_t, uint16_t, u16, UINT16_MAX, VECTOR_SORTABLE)
IMPL_VECTOR(vector_u8_t, uint8_t, u8, UINT8_MAX, VECTOR_SORTABLE)
IMPL_VECTOR(vector_flt_t, float, flt, FLT_MAX, VECTOR_SORTABLE)
IMPL_VECTOR(vector_dbl_t, double, dbl, DBL_MAX, VECTOR_SORTABLE)
IMPL_VECTOR(vector_ptr_t, void*, ptr, NULL, 0)
