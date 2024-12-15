#pragma once

#include <stdbool.h>
#include <time.h>

#define SWAP(p1, p2, T) \
	do {                \
		T temp = p1;    \
		p1 = p2;        \
		p2 = temp;      \
	} while (0);

bool tm_validate(struct tm time);
