#pragma once

#include <stdbool.h>

#include "lib/error.h"

error_t paths_same(const char* p1, const char* p2, bool* result);
