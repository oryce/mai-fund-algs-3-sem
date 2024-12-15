#include "lib/utils.h"

bool tm_validate(struct tm time) {
	int d = time.tm_mday;
	int m = time.tm_mon + 1;
	int y = time.tm_year + 1900;

	// Numbers of days in months (0-11)
	unsigned days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	// If the year is a leap year, add another day to February.
	bool leapYear = y % 4 == 0 && (y % 100 != 0 || y % 400 == 0);
	if (leapYear) days[1] = 29;

	if (d > days[m - 1]) {
		return false;
	}

	return true;
}
