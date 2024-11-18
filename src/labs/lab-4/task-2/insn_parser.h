#pragma once

#include <stdio.h>

#include "interp.h"
#include "lib/error.h"

#define ERR_INVOP 0x10000001

error_t insn_parse_stream(vector_insn_t* insns, FILE* stream);
