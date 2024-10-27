#pragma once

#define SWAP(p1, p2, T) \
	do {                \
		T temp = *p1;   \
		*p1 = *p2;      \
		*p2 = temp;     \
	} while (0);
