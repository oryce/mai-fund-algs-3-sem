#include "task.h"

#include <iostream>

int main() {
	complex z1(1, 5);
	complex z2(-1, -8);

	std::cout << z1.add(z2) << std::endl;
	std::cout << z1.sub(z2) << std::endl;
	std::cout << z1.mul(z2) << std::endl;
	std::cout << z1.div(z2) << std::endl;
	std::cout << z1.abs() << std::endl;
	std::cout << z1.arg() << std::endl;
}