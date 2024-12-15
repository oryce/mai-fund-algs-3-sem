#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "lib/mth.h"

int main() {
	size_t departmentCount = 2;
	size_t maxPriority = 100;

	const char* isoStartTime = "2024-11-12 18:31:01";
	const char* isoEndTime = "2025-11-12 18:31:01";

	time_t startTime;
	time_t endTime;

	struct tm tm;
	tm.tm_isdst = -1;

	char* pend = strptime(isoStartTime, "%Y-%m-%d %H:%M:%S", &tm);
	if (!pend || *pend != '\0') {
		fprintf(stderr, "Invalid start time.\n");
		return 1;
	}

	startTime = mktime(&tm);

	pend = strptime(isoEndTime, "%Y-%m-%d %H:%M:%S", &tm);
	if (!pend || *pend != '\0') {
		fprintf(stderr, "Invalid end time.\n");
		return 2;
	}

	endTime = mktime(&tm);

	FILE* requestFile = fopen("requests.txt", "w");
	if (!requestFile) {
		fprintf(stderr, "Can't open requests file for writing.\n");
		return 3;
	}

	srand(time(NULL));

	size_t requestCount = /*mth_rand(10, 100)*/ 10000;

	for (size_t i = 0; i != requestCount; ++i) {
		time_t time = mth_rand(startTime, endTime);

		char isoTime[128];
		strftime(isoTime, sizeof(isoTime), "%Y-%m-%d %H:%M:%S",
		         localtime(&time));

		// Write request datetime.
		fprintf(requestFile, "%s ", isoTime);

		// Write priority number.
		unsigned priority = mth_rand(0, maxPriority + 1);
		fprintf(requestFile, "%u ", priority);

		// Write department ID.
		size_t department = mth_rand(0, departmentCount);
		fprintf(requestFile, "D%zu ", department);

		// Write request text.
		fprintf(requestFile, "\"doesn't matter\"\n");

		if (ferror(requestFile)) {
			fprintf(stderr, "Can't write to request file.\n");

			fclose(requestFile);
			return 4;
		}
	}

	fclose(requestFile);
}
