#include "task.h"

int main(void) {
	const char* substring = "hello world";
	error_t error = find_substring_in_files(substring, 2, "file.txt", "file2.txt");

	if (error) {
		error_print(error);
		return (int)-error;
	}
}
