#pragma once

#include "lib/collections/string.h"

typedef struct datetime {
	int year;
	int month;
	int day;
	int hours;
	int minutes;
	int seconds;
} datetime_t;

typedef struct parsed_dt_t {
	/** Исходная строка в формате "dd:MM:yyyy hh:mm:ss". */
	string_t source;
	/** Распарсенная строка. who said we can't have additional fields? */
	datetime_t datetime;
} parsed_dt_t;

bool datetime_parse(const char* input, datetime_t* output);

datetime_t datetime_current(void);

int datetime_compare(const void* p1, const void* p2);
