#include <iostream>

#include "task.h"

int main() {
	binary_int a(5);
	binary_int b(6);
	std::cout << a + b << std::endl;
	std::cout << a - b << std::endl;
	std::cout << a * b << std::endl;

	binary_int x(10);
	std::cout << ++x << std::endl;
	std::cout << x++ << std::endl;
	std::cout << --x << std::endl;
	std::cout << x-- << std::endl;

	binary_int y(4);
	std::cout << (y << 1) << std::endl;
	std::cout << (y >> 1) << std::endl;

	std::cout << binary_int(-15) << std::endl;

	auto split = binary_int(-15).split();
	std::cout << split.first << " " << split.second << std::endl;
}