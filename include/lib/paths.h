#pragma once

#include <stdbool.h>

#include "lib/error.h"

/**
 * Checks whether two file paths `p1` and `p2` are pointing to the same file.
 *
 * @return `ERROR_IO` if the paths hadn't been resolved.
 */
error_t paths_same(const char* path1, const char* path2, bool* result);
