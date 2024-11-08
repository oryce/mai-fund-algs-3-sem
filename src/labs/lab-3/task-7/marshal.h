#pragma once

#include <stdio.h>

#include "lib/error.h"
#include "task.h"

#define ERR_INVLN 0x10000001
#define ERR_INVFN 0x10000002
#define ERR_INVMN 0x10000003
#define ERR_INVDOB 0x10000004
#define ERR_INVGEND 0x10000005
#define ERR_INVINC 0x10000006

const char* marshal_error_to_str(error_t error);

error_t liver_read_all(liver_node_t** list, FILE* inFp);

error_t liver_write(liver_t* liver, FILE* outFp);

error_t liver_write_pretty(liver_t* liver, FILE* outFp);

error_t liver_from_str(liver_t* liver, char* str);
