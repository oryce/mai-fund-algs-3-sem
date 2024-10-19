#include "lib/paths.h"

#include <string.h>

#ifdef _WIN32
#include <errhandlingapi.h>
#include <fileapi.h>
#else
#include <stdlib.h>
#endif

error_t paths_same(const char* path1, const char* path2, bool* result) {
	if (path1 == NULL) THROW(IllegalArgumentException, "`path1` may not be null");
	if (path2 == NULL) THROW(IllegalArgumentException, "`path2` may not be null");

	const int pathBufSize = 4096;
	char abs1[pathBufSize];
	char abs2[pathBufSize];

#ifdef _WIN32
	if (!GetFullPathNameA(path1, pathBufSize, abs1, NULL)) {
		THROW(IOException, "`GetFullPathNameA` failed on path (1): 0x%08x", GetLastError());
	}
	if (!GetFullPathNameA(path2, pathBufSize, abs2, NULL)) {
		THROW(IOException, "`GetFullPathNameA` failed on path (2): 0x%08x", GetLastError());
	}
#else
	if (realpath(path1, abs1) == NULL) {
		THROW(IOException, "`realpath` failed on path (1)");
	}
	if (realpath(path2, abs2) == NULL) {
		THROW(IOException, "`realpath` failed on path (2)");
	}
#endif

	*result = strncmp(abs1, abs2, pathBufSize) == 0;
	return NO_EXCEPTION;
}
