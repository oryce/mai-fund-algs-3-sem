#include "lib/error.h"

typedef struct vector {
	double* coords;
	double norm;
} vector_t;

typedef error_t (*norm_t)(double* res, const vector_t* vec, unsigned n,
                          const void* param);

/* see tasks.pdf */

error_t norm_1(double* res, const vector_t* vec, unsigned n, const void* param);

error_t norm_2(double* res, const vector_t* vec, unsigned n, const void* param);

error_t norm_3(double* res, const vector_t* vec, unsigned n, const void* param);

/**
 * Finds vectors with the largest norms, given vectors and norm functions.
 *
 * For each norm function a list of vectors with the largest norms is stored in
 * |outVecs[i]| with the size |outLens[i]|.
 *
 * |outVecs| & |outLens| must be allocated with the size of |nNorms| prior to
 * calling the function. The function will allocate buffers of |nVecs| in
 * |outVecs| which must be freed by the caller.
 *
 * The function must be called the following way:
 *
 * ```
 * longest_vecs(
 *   outVecs, outLens, n, nNorms, nVecs,
 *   <norm 1>, <norm param 1>, ..., <norm nNorms>, <norm param nNorms>,
 *   <vec 1>, ..., <vec nVecs>
 * )
 * ```
 *
 * Each norm expects a _norm parameter_, which is a non-NULL void*. See the
 * respective norm's docs for more info on the parameter.
 *
 * @param outVecs list of results per norm. |result[i]| is an array of size
 *                |outLens[i]|, which contains vectors with the largest norms.
 * @param outLens sizes of resulting arrays
 * @param n       vectors dimension
 * @param nNorms  amount of norms passed to the function
 * @param nVecs   amount of vectors passed to the function
 */
error_t longest_vecs(vector_t** outVecs, size_t* outLens, unsigned n,
                     unsigned nNorms, unsigned nVecs, ...);
