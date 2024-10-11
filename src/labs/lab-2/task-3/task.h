#pragma once

#include "lib/error.h"

error_t find_substring_in_files(const char* needle, int fileCnt, ...);
