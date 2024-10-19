#pragma once

#include "lib/error.h"

error_t task_merge_lexemes(FILE* input1, FILE* input2, FILE* output);

error_t task_process_lexemes(FILE* input, FILE* output);
