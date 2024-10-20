#include "tasks.h"

error_t main_(void) {
	error_t status;

	// epic tests by Pavel Domkin
	status = overfprintf(stdout, "Number is: %Ro %lf %Ro\n", 5124, 123.0, 1);
	if (FAILED(status)) PASS(status);
	status = overfprintf(stdout, "Number is: %Zr\n", (unsigned int)10);
	if (FAILED(status)) PASS(status);
	status = overfprintf(stdout, "Number is: %Cv\n", 1234, 16);
	if (FAILED(status)) PASS(status);
	status = overfprintf(stdout, "Numbers: %to %TO %to %TO\n", "abcdef", 16, "123", 12, "0001", 2, "-25", 17);
	if (FAILED(status)) PASS(status);
	status = overfprintf(stdout, "%mi\n%mu\n%md\n%mf\n", (int)10, (unsigned int)12, (double)2.2, (double)5.6);
	if (FAILED(status)) PASS(status);
	status = overfprintf(stdout, "Number is: %Ro\n", (unsigned int)111);
	if (FAILED(status)) PASS(status);
	status = overfprintf(stdout, "%Ro %CV %Ro %CV %CV\n", 15, 10, 16, 15, 22, 2, 17, 2);
	if (FAILED(status)) PASS(status);

	char buffer[1024];

	status = oversprintf(buffer, "Number is: %Ro", 123);
	if (FAILED(status)) PASS(status);
	printf("%s\n", buffer);

	status = oversprintf(buffer, "Number is: %Cv, %CV", 0010, 5, 1234, 16);
	if (FAILED(status)) PASS(status);
	printf("%s\n", buffer);

	status = oversprintf(buffer, "Numbers: %to %TO %d%d", "abcdef", 16, "10", 10, 12, 12);
	if (FAILED(status)) PASS(status);
	printf("%s\n", buffer);

	status = oversprintf(buffer, "%mi\n%mu\n%md\n%mf\n", (int)10, (unsigned int)12, (double)2.2, (double)5.6);
	if (FAILED(status)) PASS(status);
	printf("%s\n", buffer);

	status = oversprintf(buffer, "Number is: %Zr\n", (unsigned int)10);
	if (FAILED(status)) PASS(status);
	printf("%s\n", buffer);

	return NO_EXCEPTION;
}

int main(void) {
	error_t status = main_();
	if (FAILED(status)) {
		error_print(status);
		return (int)status.code;
	}
}
