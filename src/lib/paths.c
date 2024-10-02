#include "lib/paths.h"

#include <stdlib.h>
#include <limits.h>
#include <string.h>

error_t paths_same(const char* p1, const char* p2, bool* result) {
	char abs1[PATH_MAX];
	char abs2[PATH_MAX];

	if (realpath(p1, abs1) == NULL) return ERROR_IO;
	if (realpath(p2, abs2) == NULL) return ERROR_IO;

	*result = strncmp(abs1, abs2, PATH_MAX) == 0;
	return ERROR_SUCCESS;
}
