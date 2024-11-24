#include "interp.h"

#include <math.h>
#include <time.h>

#include "lib/chars.h"
#include "lib/convert.h"
#include "lib/lexeme.h"
#include "lib/mth.h"

IMPL_VECTOR(vector_insn_t, insn_t, insn, {NULL})

const char* interp_error_to_string(error_t error) {
	switch (error) {
		case ERR_INVARGCNT:
			return "Invalid argument count";
		case ERR_INVARRID:
			return "Invalid array index";
		case ERR_IDXRANGE:
			return "Array index out of range";
		default:
			return NULL;
	}
}

opcode_t opcode_from_string(const char* string) {
	if (!string) return OP_INVALID;

	if (strcmp(string, "load") == 0) return OP_LOAD;
	if (strcmp(string, "save") == 0) return OP_SAVE;
	if (strcmp(string, "rand") == 0) return OP_RAND;
	if (strcmp(string, "concat") == 0) return OP_CONCAT;
	if (strcmp(string, "free") == 0) return OP_FREE;
	if (strcmp(string, "remove") == 0) return OP_REMOVE;
	if (strcmp(string, "copy") == 0) return OP_COPY;
	if (strcmp(string, "sort") == 0) return OP_SORT;
	if (strcmp(string, "shuffle") == 0) return OP_SHUFFLE;
	if (strcmp(string, "stats") == 0) return OP_STATS;
	if (strcmp(string, "print") == 0) return OP_PRINT;

	return OP_INVALID;
}

const char* opcode_to_string(opcode_t op) {
	switch (op) {
		case OP_LOAD:
			return "OP_LOAD";
		case OP_SAVE:
			return "OP_SAVE";
		case OP_RAND:
			return "OP_RAND";
		case OP_CONCAT:
			return "OP_CONCAT";
		case OP_FREE:
			return "OP_FREE";
		case OP_REMOVE:
			return "OP_REMOVE";
		case OP_COPY:
			return "OP_COPY";
		case OP_SORT:
			return "OP_SORT";
		case OP_SHUFFLE:
			return "OP_SHUFFLE";
		case OP_STATS:
			return "OP_STATS";
		case OP_PRINT:
			return "OP_PRINT";
		case OP_INVALID:
			return "OP_INVALID";
	}
}

insn_t insn_create(opcode_t op) {
	return (insn_t){.op = op, .args = vector_str_create()};
}

void insn_destroy(insn_t* insn) {
	if (!insn) return;

	for (size_t i = 0; i != vector_str_size(&insn->args); ++i) {
		string_t* arg = vector_str_get(&insn->args, i);
		string_destroy(arg);
	}

	vector_str_destroy(&insn->args);
}

error_t to_string_fail(error_t errcode, string_t* string) {
	if (string) {
		string_destroy(string);
	}

	return errcode;
}

error_t insn_to_string(const insn_t* insn, string_t* string) {
	if (!insn || !string) return ERR_INVVAL;
	if (!string_create(string)) return ERR_MEM;

	if (!string_append_c_str(string, "insn_t[ op=") ||
	    !string_append_c_str(string, opcode_to_string(insn->op)) ||
	    !string_append_c_str(string, ", args=[ ")) {
		return to_string_fail(ERR_MEM, string);
	}

	for (size_t i = 0; i != vector_str_size(&insn->args); ++i) {
		string_t* arg = vector_str_get(&insn->args, i);

		if (!string_append_char(string, '\'') || !string_append(string, arg) ||
		    !string_append_char(string, '\'') ||
		    (i + 1 < vector_str_size(&insn->args) &&
		     !string_append_c_str(string, ", "))) {
			return to_string_fail(ERR_MEM, string);
		}
	}

	if (!string_append_c_str(string, " ] ]")) {
		return to_string_fail(ERR_MEM, string);
	}

	return 0;
}

const char* insn_arg(const insn_t* insn, size_t idx) {
	if (!insn) return NULL;
	return string_to_c_str(vector_str_get(&insn->args, idx));
}

size_t insn_argc(const insn_t* insn) { return vector_str_size(&insn->args); }

interp_t interp_create(void) {
	interp_t ip;

	for (size_t i = 0; i != sizeof(ip.state) / sizeof(ip.state[0]); ++i) {
		ip.state[i] = vector_i64_create();
	}

	return ip;
}

void interp_destroy(interp_t* ip) {
	if (!ip) return;

	for (size_t i = 0; i != sizeof(ip->state) / sizeof(ip->state[0]); ++i) {
		vector_i64_destroy(&ip->state[i]);
	}
}

error_t get_array_idx_(const char* arg, size_t* idx) {
	if (!arg || !idx) {
		return ERR_INVVAL;
	}
	if ((*arg != '\0' && *(arg + 1) != '\0') || !chars_is_alpha(*arg)) {
		return ERR_INVARRID;
	}

	*idx = chars_lower(*arg) - 'a';
	return 0;
}

error_t load_clean_(error_t errcode, FILE* stream, vector_str_t* lexemes) {
	fclose(stream);

	if (lexemes) {
		lexeme_destroy(lexemes);
	}

	return errcode;
}

error_t interp_load_(interp_t* ip, const insn_t* insn) {
	error_t error;

	if (!ip || !insn || insn->op != OP_LOAD) return ERR_INVVAL;
	if (insn_argc(insn) != 2) return ERR_INVARGCNT;

	size_t idx;
	if ((error = get_array_idx_(insn_arg(insn, 0), &idx))) {
		fprintf(stderr, "Can't parse array index.\n");
		return error;
	}

	FILE* stream = fopen(insn_arg(insn, 1), "r");
	if (!stream) {
		fprintf(stderr, "Can't open the input file for reading.\n");
		return ERR_IO;
	}

	vector_str_t lexemes;
	if ((error = lexeme_read(stream, &lexemes))) {
		fprintf(stderr, "Can't read lexemes from the input file.\n");
		return load_clean_(error, stream, &lexemes);
	}

	if (!vector_i64_clear(&ip->state[idx])) {
		fprintf(stderr, "Can't clear state array.\n");
		return load_clean_(ERR_MEM, stream, &lexemes);
	}

	for (size_t i = 0; i != vector_str_size(&lexemes); ++i) {
		string_t* lexeme = vector_str_get(&lexemes, i);

		long value;
		if ((error = str_to_long(string_to_c_str(lexeme), &value))) {
			fprintf(stderr, "Can't parse %s as long number.\n",
			        string_to_c_str(lexeme));
			return load_clean_(error, stream, &lexemes);
		}

		if (!vector_i64_push_back(&ip->state[idx], value)) {
			fprintf(stderr, "Can't insert number into state array.\n");
			return load_clean_(ERR_MEM, stream, &lexemes);
		}
	}

	return load_clean_(0, stream, &lexemes);
}

error_t print_range_(const vector_i64_t* array, size_t from, size_t to) {
	for (size_t i = from; i != to + 1; ++i) {
		int64_t* item = vector_i64_get(array, i);
		if (!item) {
			return ERR_IDXRANGE;
		}
		printf("[%zu] %lld\n", i, *item);
	}

	return 0;
}

error_t save_clean_(error_t errcode, FILE* stream) {
	fclose(stream);
	return errcode;
}

error_t interp_save_(interp_t* ip, const insn_t* insn) {
	error_t error;

	if (!ip || !insn || insn->op != OP_SAVE) return ERR_INVVAL;
	if (insn_argc(insn) != 2) return ERR_INVARGCNT;

	size_t idx;
	if ((error = get_array_idx_(insn_arg(insn, 0), &idx))) {
		fprintf(stderr, "Can't parse array index.\n");
		return error;
	}

	FILE* stream = fopen(insn_arg(insn, 1), "w");
	if (!stream) {
		fprintf(stderr, "Can't open output file for writing.\n");
		return ERR_IO;
	}

	vector_i64_t* array = &ip->state[idx];
	for (size_t i = 0; i != vector_i64_size(array); ++i) {
		fprintf(stream, "%lld\n", *vector_i64_get(array, i));
		if (ferror(stream)) {
			fprintf(stderr, "Can't write to output file.\n");
			return save_clean_(ERR_IO, stream);
		}
	}

	return save_clean_(0, stream);
}

error_t interp_rand_(interp_t* ip, const insn_t* insn) {
	error_t error;

	if (!ip || !insn || insn->op != OP_RAND) return ERR_INVVAL;
	if (insn_argc(insn) != 4) return ERR_INVARGCNT;

	size_t idx;
	if ((error = get_array_idx_(insn_arg(insn, 0), &idx))) {
		fprintf(stderr, "Can't parse array index.\n");
		return error;
	}

	size_t count;
	if ((error = str_to_ulong(insn_arg(insn, 1), &count))) {
		fprintf(stderr, "Can't parse item count.\n");
		return error;
	}
	long lb;
	if ((error = str_to_long(insn_arg(insn, 2), &lb))) {
		fprintf(stderr, "Can't parse lower bound.\n");
		return error;
	}
	long ub;
	if ((error = str_to_long(insn_arg(insn, 3), &ub))) {
		fprintf(stderr, "Can't parse upper bound.\n");
		return error;
	}

	srand(time(NULL));  // NOLINT(*-msc51-cpp)

	while (count--) {
		long value = mth_rand(lb, ub + 1);
		if (!vector_i64_push_back(&ip->state[idx], value)) {
			fprintf(stderr, "Can't push value into array.\n");
			return ERR_MEM;
		}
	}

	return 0;
}

error_t interp_concat_(interp_t* ip, const insn_t* insn) {
	error_t error;

	if (!ip || !insn || insn->op != OP_CONCAT) return ERR_INVVAL;
	if (insn_argc(insn) != 2) return ERR_INVARGCNT;

	size_t dstIdx;
	if ((error = get_array_idx_(insn_arg(insn, 0), &dstIdx))) {
		fprintf(stderr, "Can't parse destination array index.\n");
		return error;
	}
	size_t srcIdx;
	if ((error = get_array_idx_(insn_arg(insn, 1), &srcIdx))) {
		fprintf(stderr, "Can't parse source array index.\n");
		return error;
	}

	vector_i64_t* src = &ip->state[srcIdx];
	vector_i64_t* dst = &ip->state[dstIdx];

	for (size_t i = 0; i != vector_i64_size(src); ++i) {
		int64_t* value = vector_i64_get(src, i);
		if (!vector_i64_push_back(dst, *value)) {
			fprintf(stderr, "Can't push into destination array.\n");
			return ERR_MEM;
		}
	}

	return 0;
}

error_t interp_free_(interp_t* ip, const insn_t* insn) {
	error_t error;

	if (!ip || !insn || insn->op != OP_FREE) return ERR_INVVAL;
	if (insn_argc(insn) != 1) return ERR_INVARGCNT;

	size_t idx;
	if ((error = get_array_idx_(insn_arg(insn, 0), &idx))) {
		fprintf(stderr, "Can't parse array index.\n");
		return error;
	}

	vector_i64_t* array = &ip->state[idx];
	if (!vector_i64_clear(array)) {
		fprintf(stderr, "Can't clean state array.\n");
		return ERR_MEM;
	}

	return 0;
}

error_t interp_remove_(interp_t* ip, const insn_t* insn) {
	error_t error;

	if (!ip || !insn || insn->op != OP_REMOVE) return ERR_INVVAL;
	if (insn_argc(insn) != 3) return ERR_INVARGCNT;

	size_t idx;
	if ((error = get_array_idx_(insn_arg(insn, 0), &idx))) {
		fprintf(stderr, "Can't parse array index.\n");
		return error;
	}
	size_t from, count;
	if ((error = str_to_ulong(insn_arg(insn, 1), &from)) ||
	    (error = str_to_ulong(insn_arg(insn, 2), &count))) {
		fprintf(stderr, "Invalid range.\n");
		return error;
	}

	vector_i64_t* array = &ip->state[idx];

	while (count--) {
		if (!vector_i64_remove(array, from, NULL)) {
			fprintf(stderr,
			        "Can't remove item from vector. Index is out of range or \n"
			        "the vector couldn't be shrunk.\n");
			return ERR_CHECK;
		}
	}

	return 0;
}

error_t interp_copy_(interp_t* ip, const insn_t* insn) {
	error_t error;

	if (!ip || !insn || insn->op != OP_COPY) return ERR_INVVAL;
	if (insn_argc(insn) != 4) return ERR_INVARGCNT;

	size_t srcIdx;
	if ((error = get_array_idx_(insn_arg(insn, 0), &srcIdx))) {
		fprintf(stderr, "Can't parse source array index.\n");
		return error;
	}
	size_t from, to;
	if ((error = str_to_ulong(insn_arg(insn, 1), &from)) ||
	    (error = str_to_ulong(insn_arg(insn, 2), &to))) {
		fprintf(stderr, "Invalid range.\n");
		return error;
	}
	size_t dstIdx;
	if ((error = get_array_idx_(insn_arg(insn, 3), &dstIdx))) {
		fprintf(stderr, "Can't parse destination array index.\n");
		return error;
	}

	vector_i64_t* src = &ip->state[srcIdx];
	vector_i64_t* dst = &ip->state[dstIdx];

	if (!vector_i64_clear(dst)) {
		return ERR_MEM;
	}

	for (size_t i = from; i != to + 1; ++i) {
		int64_t* value = vector_i64_get(src, i);
		if (!value) {
			return ERR_IDXRANGE;
		}
		if (!vector_i64_push_back(dst, *value)) {
			fprintf(stderr, "Can't push value into destination array.\n");
			return ERR_MEM;
		}
	}

	return 0;
}

int cmp_long_ascending_(const int64_t* a, const int64_t* b) {
	if (!a || !b || a == b) return 0;
	return mth_sign_long(*a - *b);
}

int cmp_long_descending_(const int64_t* a, const int64_t* b) {
	if (!a || !b || a == b) return 0;
	return mth_sign_long(*b - *a);
}

int cmp_long_random(const int64_t* a, const int64_t* b) {
	if (!a || !b) return 0;
	return (int)mth_rand(-1, 2);  // Random value in [-1; 1]
}

error_t interp_sort_(interp_t* ip, const insn_t* insn) {
	if (!ip || !insn || insn->op != OP_SORT) return ERR_INVVAL;
	if (insn_argc(insn) != 1) return ERR_INVARGCNT;

	const char* arg = insn_arg(insn, 0);

	vector_i64_t* array;
	bool sortAscending;

	if (chars_is_alpha(*arg) && (*(arg + 1) == '+' || *(arg + 1) == '-')) {
		array = &ip->state[chars_lower(*arg) - 'a'];
		sortAscending = *(arg + 1) == '+';
	} else {
		fprintf(stderr, "Invalid array index or sort order.\n");
		return ERR_INVVAL;
	}

	qsort(array->buffer, array->size, sizeof(int64_t),
	      (int (*)(const void*, const void*))(
	          sortAscending ? &cmp_long_ascending_ : &cmp_long_descending_));

	return 0;
}

error_t interp_shuffle_(interp_t* ip, const insn_t* insn) {
	error_t error;

	if (!ip || !insn || insn->op != OP_SHUFFLE) return ERR_INVVAL;
	if (insn_argc(insn) != 1) return ERR_INVARGCNT;

	size_t idx;
	if ((error = get_array_idx_(insn_arg(insn, 0), &idx))) {
		return error;
	}

	srand(time(NULL));  // NOLINT(*-msc51-cpp)

	vector_i64_t* array = &ip->state[idx];
	qsort(array->buffer, array->size, sizeof(int64_t),
	      (int (*)(const void*, const void*)) & cmp_long_random);

	return 0;
}

error_t stats_clean_(error_t errcode, vector_i64_t* clone) {
	vector_i64_destroy(clone);
	return errcode;
}

error_t interp_stats_(interp_t* ip, const insn_t* insn) {
	error_t error;

	if (!ip || !insn || insn->op != OP_STATS) return ERR_INVVAL;
	if (insn_argc(insn) != 1) return ERR_INVARGCNT;

	size_t idx;
	if ((error = get_array_idx_(insn_arg(insn, 0), &idx))) {
		return error;
	}

	vector_i64_t* array = &ip->state[idx];
	size_t n = vector_i64_size(array);

	if (vector_i64_is_empty(array)) {
		fprintf(stderr, "The array is empty.\n");
		return 0;
	}

	size_t minIdx = 0, maxIdx = 0;
	double mean = 0, stddev = 0;

	for (size_t i = 0; i != n; ++i) {
		int64_t* value = vector_i64_get(array, i);

		if (*value < *vector_i64_get(array, minIdx)) minIdx = i;
		if (*value > *vector_i64_get(array, maxIdx)) maxIdx = i;

		mean += (double)*value;
	}

	mean /= (double)n;

	for (size_t i = 0; i != n; ++i) {
		double value = (double)*vector_i64_get(array, i);
		stddev += (value - mean) * (value - mean);
	}

	stddev /= (double)n;
	stddev = sqrt(stddev);

	vector_i64_t clone;
	if (!vector_i64_dup(array, &clone) || !vector_i64_sort(&clone)) {
		return stats_clean_(ERR_MEM, &clone);
	}

	size_t maxCount = 1, count = 1;
	int64_t mode = *vector_i64_get(&clone, 0);

	for (size_t i = 1; i != n; ++i) {
		int64_t value = *vector_i64_get(&clone, i);
		int64_t value0 = *vector_i64_get(&clone, i - 1);

		if (value == value0) {
			++count;
		} else {
			count = 1;
		}
		if (count > maxCount) {
			maxCount = count;
			mode = value;
		} else if (count == maxCount && value > mode) {
			mode = value;
		}
	}

	printf("Min: %lld at %zu, max: %lld at %zu\n",
	       *vector_i64_get(array, minIdx), minIdx,
	       *vector_i64_get(array, maxIdx), maxIdx);
	printf("Mean: %lf, stddev: %lf, mode: %lld\n", mean, stddev, mode);

	return stats_clean_(0, &clone);
}

error_t interp_print_(interp_t* ip, const insn_t* insn) {
	error_t error;

	if (!ip || !insn || insn->op != OP_PRINT) return ERR_INVVAL;
	if (insn_argc(insn) != 2 && insn_argc(insn) != 3) return ERR_INVARGCNT;

	size_t idx;
	if ((error = get_array_idx_(insn_arg(insn, 0), &idx))) {
		return error;
	}

	vector_i64_t* array = &ip->state[idx];

	if (insn_argc(insn) == 2) {
		const char* arg = insn_arg(insn, 1);
		if (stricmp(arg, "all") == 0) {
			// Special case, because (0 - 1) in an ulong is UB.
			if (vector_i64_is_empty(array)) {
				printf("<empty array>");
			} else {
				error = print_range_(array, 0, vector_i64_size(array) - 1);
			}
		} else {
			size_t i;
			if ((error = str_to_ulong(arg, &i))) {
				fprintf(stderr, "Invalid `from` index.\n");
				return error;
			}
			// Only print this element.
			error = print_range_(array, i, i);
		}
	} else /* argc == 3 */ {
		size_t from, to;
		if ((error = str_to_ulong(insn_arg(insn, 1), &from)) ||
		    (error = str_to_ulong(insn_arg(insn, 2), &to))) {
			fprintf(stderr, "Invalid range.\n");
			return error;
		}
		error = print_range_(array, from, to);
	}

	return error;
}

error_t interp_run(interp_t* ip, const vector_insn_t* insns) {
	if (!ip || !insns) return ERR_INVVAL;

	for (size_t i = 0; i != vector_insn_size(insns); ++i) {
		insn_t* insn = vector_insn_get(insns, i);
		error_t error;

		switch (insn->op) {
			case OP_LOAD:
				error = interp_load_(ip, insn);
				break;
			case OP_SAVE:
				error = interp_save_(ip, insn);
				break;
			case OP_RAND:
				error = interp_rand_(ip, insn);
				break;
			case OP_CONCAT:
				error = interp_concat_(ip, insn);
				break;
			case OP_FREE:
				error = interp_free_(ip, insn);
				break;
			case OP_REMOVE:
				error = interp_remove_(ip, insn);
				break;
			case OP_COPY:
				error = interp_copy_(ip, insn);
				break;
			case OP_SORT:
				error = interp_sort_(ip, insn);
				break;
			case OP_SHUFFLE:
				error = interp_shuffle_(ip, insn);
				break;
			case OP_STATS:
				error = interp_stats_(ip, insn);
				break;
			case OP_PRINT:
				error = interp_print_(ip, insn);
				break;
			default:
				error = ERR_INVOP;
				break;
		}

		if (error) {
			fprintf(stderr, "----------------------------------------------\n");
			fprintf(stderr, "While executing instruction %s (bci = %zu):\n",
			        opcode_to_string(insn->op), i);

			error_fmt_t fmt[] = {&interp_error_to_string};
			error_print_ex(error, fmt, sizeof(fmt) / sizeof(fmt[0]));

			return error;
		}
	}

	return 0;
}
