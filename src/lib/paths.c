#include "lib/paths.h"

#include <string.h>

#ifdef _WIN32
#include <fileapi.h>
#else
#include <stdlib.h>
#endif

error_t paths_same(const char* path1, const char* path2, bool* result) {
	if (path1 == NULL || path2 == NULL) {
		return ERROR_INVALID_PARAMETER;
	}

	const int pathBufSize = 4096;
	char abs1[pathBufSize];
	char abs2[pathBufSize];

#ifdef _WIN32
	if (!GetFullPathNameA(path1, pathBufSize, abs1, NULL)) {
		return ERROR_IO;
	}
	if (!GetFullPathNameA(path2, pathBufSize, abs2, NULL)) {
		return ERROR_IO;
	}
#else
	if (realpath(path1, abs1) == NULL) return ERROR_IO;
	if (realpath(path2, abs2) == NULL) return ERROR_IO;
#endif

	*result = strncmp(abs1, abs2, pathBufSize) == 0;
	return ERROR_SUCCESS;
}
