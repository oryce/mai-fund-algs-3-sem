#include "tasks.h"

int main(void) {
	// epic tests by Pavel Domkin
	int nWritten = overfprintf(stdout, "Number is: %Ro %lf %Ro\n", 5124, 123.0, 1);
	printf("overfprintf wrote %d chars\n", nWritten);  // 35

	overfprintf(stdout, "%*d %Ro\n", 5, 10, 123);
	overfprintf(stdout, "this is a test %Ro %n\n", 2024);
	overfprintf(stdout, "Number is: %Zr\n", (unsigned int)10);
	overfprintf(stdout, "Number is: %Cv\n", 1234, 16);
	overfprintf(stdout, "Numbers: %to %TO %to %TO\n", "abcdef", 16, "123", 12, "0001", 2, "-25", 17);
	overfprintf(stdout, "%mi\n%mu\n%md\n%mf\n", (int)10, (unsigned int)12, (double)2.2, (double)5.6);
	overfprintf(stdout, "Number is: %Ro\n", (unsigned int)111);
	overfprintf(stdout, "%Ro %CV %Ro %CV %CV\n", 15, 10, 16, 15, 22, 2, 17, 2);

	char buffer[1024];

	oversprintf(buffer, "Number is: %Ro", 123);
	printf("%s\n", buffer);
	oversprintf(buffer, "Number is: %Cv, %CV", 0010, 5, 1234, 16);
	printf("%s\n", buffer);
	oversprintf(buffer, "Numbers: %to %TO %d%d", "abcdef", 16, "10", 10, 12, 12);
	printf("%s\n", buffer);
	oversprintf(buffer, "%mi\n%mu\n%md\n%mf\n", (int)10, (unsigned int)12, (double)2.2, (double)5.6);
	printf("%s\n", buffer);
	oversprintf(buffer, "Number is: %Zr\n", (unsigned int)10);
	printf("%s\n", buffer);
}
