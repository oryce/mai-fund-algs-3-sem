#include "datetime.h"

#include <time.h>

#include "lib/convert.h"
#include "lib/mth.h"

typedef enum datetime_state {
	DT_DAYS,
	DT_MONTHS,
	DT_YEARS,
	DT_HOURS,
	DT_MINUTES,
	DT_SECONDS,
	DT_VALID
} datetime_state_t;

static bool dt_fail(char* ptr) {
	free(ptr);
	return false;
}

bool datetime_parse(const char* input, datetime_t* output) {
	datetime_t dt;
	datetime_state_t st = DT_DAYS;

	char* copy = strdup(input);
	if (!copy) return false;

	char* copyPtr = copy;
	char* datePart;

	// Split "dd:MM:yyyy hh:mm:ss" into two parts (datePart).

	while ((datePart = strsep(&copyPtr, " "))) {
		if (st != DT_DAYS && st != DT_HOURS) return dt_fail(copy);

		char* datePartPtr = datePart;
		char* segment;

		// Split the part into segments.

		while ((segment = strsep(&datePartPtr, ":"))) {
			size_t length = strlen(segment);

			unsigned long value;
			if (str_to_ulong(segment, &value)) return dt_fail(copy);

			switch (st) {
				case DT_DAYS:
					if (length != 2) return dt_fail(copy);

					dt.day = (int)value;
					if (dt.day == 0 || dt.day > 31) return dt_fail(copy);

					st = DT_MONTHS;
					break;
				case DT_MONTHS:
					if (length != 2) return dt_fail(copy);

					dt.month = (int)value;
					if (dt.month == 0 || dt.month > 12) return dt_fail(copy);

					switch (dt.month) {
						case 1:   // January
						case 3:   // March
						case 5:   // May
						case 7:   // July
						case 8:   // August
						case 10:  // October
						case 12:  // December
							if (dt.day > 30) return dt_fail(copy);
							break;
						case 2:  // February
							if (dt.day > 29) return dt_fail(copy);
							break;
					}

					st = DT_YEARS;
					break;
				case DT_YEARS:
					if (length != 4) return dt_fail(copy);

					dt.year = (int)value;

					bool leap_year = dt.year % 4 == 0 &&
					                 (dt.year % 100 != 0 || dt.year % 400 == 0);
					if (!leap_year && dt.month == 2 && dt.day > 28)
						return dt_fail(copy);

					st = DT_HOURS;
					break;
				case DT_HOURS:
					if (length != 2) return dt_fail(copy);

					dt.hours = (int)value;
					if (dt.hours > 23) return dt_fail(copy);

					st = DT_MINUTES;
					break;
				case DT_MINUTES:
					if (length != 2) return dt_fail(copy);

					dt.minutes = (int)value;
					if (dt.minutes > 59) return dt_fail(copy);

					st = DT_SECONDS;
					break;
				case DT_SECONDS:
					if (length != 2) return dt_fail(copy);

					dt.seconds = (int)value;
					if (dt.seconds > 59) return dt_fail(copy);

					st = DT_VALID;
					break;
				case DT_VALID:
					// unexpected part
					return dt_fail(copy);
			}
		}
	}

	if (st == DT_VALID && output) {
		*output = dt;
	}

	free(copy);
	return st == DT_VALID;
}

datetime_t datetime_current(void) {
	time_t rawTime;
	time(&rawTime);

	struct tm* timeInfo;
	timeInfo = localtime(&rawTime);

	return (datetime_t){.day = timeInfo->tm_mday,
	                    .month = timeInfo->tm_mon + 1,
	                    .year = timeInfo->tm_year + 1900,
	                    .hours = timeInfo->tm_hour,
	                    .minutes = timeInfo->tm_min,
	                    .seconds = timeInfo->tm_sec};
}

int datetime_compare(const void* p1, const void* p2) {
	if (!p1 || !p2 || p1 == p2) return 0;

	const datetime_t* a = (const datetime_t*)p1;
	const datetime_t* b = (const datetime_t*)p2;

	int order;

	order = mth_sign_long(a->year - b->year);
	if (order) return order;
	order = mth_sign_long(a->month - b->month);
	if (order) return order;
	order = mth_sign_long(a->day - b->day);
	if (order) return order;
	order = mth_sign_long(a->hours - b->hours);
	if (order) return order;
	order = mth_sign_long(a->minutes - b->minutes);
	if (order) return order;
	order = mth_sign_long(a->seconds - b->seconds);
	if (order) return order;

	return 0;
}
