#include <assert.h>

#include "tasks.h"

int main(void) {
	int written;

	int roman0, decimal0, roman1, decimal1;
	written = oversscanf("VI1234XVII-1257", "%Ro%d%Ro%d", &roman0, &decimal0, &roman1, &decimal1);

	assert(written == 4);
	assert(roman0 == 6);
	assert(decimal0 == 1234);
	assert(roman1 == 17);
	assert(decimal1 == -1257);

	written = oversscanf("IX  0011  123", "%Ro %Zr %d", &roman0, &decimal0, &decimal1);
	assert(written == 3);
	assert(roman0 == 9);
	assert(decimal0 == 3);
	assert(decimal1 == 123);

	// %Zr didn't remove the last digit on EOF
	written = oversscanf("0011", "%Zr", &decimal0);
	assert(written == 1);
	assert(decimal0 == 3);

	// NULL tests
	written = oversscanf("1234", "%d", NULL);
	assert(written == 0);
	written = oversscanf("IX", "%Ro", NULL);
	assert(written == 0);

	return 0;
}
