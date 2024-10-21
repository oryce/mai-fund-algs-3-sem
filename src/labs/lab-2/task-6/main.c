#include <assert.h>

#include "tasks.h"

int main(void) {
	const char* buffer = "helloworldVI1234XIV4567";

	int roman0, decimal0, roman1, decimal1;
	oversscanf((char*)buffer, "hello world %Ro %d %Ro %d", &roman0, &decimal0, &roman1, &decimal1);

	assert(roman0 == 6);
	assert(decimal0 == 1234);
	assert(roman1 == 14);
	assert(decimal1 == 4567);
}
