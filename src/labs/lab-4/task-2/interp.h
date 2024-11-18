#pragma once

#include "lib/collections/string.h"
#include "lib/collections/vector.h"
#include "lib/error.h"

#define ERR_INVARGCNT 0x20000001
#define ERR_INVARRID 0x20000002
#define ERR_IDXRANGE 0x20000003
#define ERR_INVOP 0x20000004

#if defined(_WIN32)
#include <Windows.h>
#define stricmp _stricmp
#elif defined(__unix__) || defined(__APPLE__)
#include <strings.h>
#define stricmp strcasecmp
#endif

const char* interp_error_to_string(error_t error);

typedef enum opcode {
	OP_LOAD,
	OP_SAVE,
	OP_RAND,
	OP_CONCAT,
	OP_FREE,
	OP_REMOVE,
	OP_COPY,
	OP_SORT,
	OP_SHUFFLE,
	OP_STATS,
	OP_PRINT,
	OP_INVALID
} opcode_t;

opcode_t opcode_from_string(const char* string);

const char* opcode_to_string(opcode_t op);

typedef struct insn {
	opcode_t op;
	vector_str_t args;
} insn_t;

DEFINE_VECTOR(vector_insn_t, insn_t, insn)

insn_t insn_create(opcode_t op);

void insn_destroy(insn_t* insn);

error_t insn_to_string(const insn_t* insn, string_t* string);

const char* insn_arg(const insn_t* insn, size_t idx);

size_t insn_argc(const insn_t* insn);

typedef struct interp {
	vector_i64_t state[26];
} interp_t;

interp_t interp_create(void);

void interp_destroy(interp_t* ip);

error_t interp_run(interp_t* ip, const vector_insn_t* insns);
