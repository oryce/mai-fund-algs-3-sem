#include "task.h"

error_t main_(void) {
	const char* substring = "hello world";
	return find_substring_in_files(substring, 2, "file.txt", "file2.txt");
}

int main(void) {
	error_t error = main_();
	if (FAILED(error)) {
		error_print(error);
		return (int)error.code;
	}
}
