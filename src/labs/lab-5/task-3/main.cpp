#include <iostream>

#include "task.h"

int main() {
	logical_values_array a = 0b1100;
	logical_values_array b = 0b1010;

	std::cout << "not a = " << a.lNeg() << std::endl;
	std::cout << "a and b = " << a.lAnd(b) << std::endl;
	std::cout << "a or b = " << a.lOr(b) << std::endl;
	std::cout << "a xor b = " << a.lXor(b) << std::endl;
	std::cout << "a xnor b = " << a.lXnor(b) << std::endl;
	std::cout << "a imply b = " << a.lImply(b) << std::endl;
	std::cout << "a converse b = " << a.lConverse(b) << std::endl;
	std::cout << "a nor b = " << a.lNor(b) << std::endl;
	std::cout << "a nand b = " << a.lNand(b) << std::endl;

	std::cout << "a == a = " << a.equals(a) << std::endl;
	std::cout << "a == b = " << a.equals(b) << std::endl;

	std::cout << "a[0] = " << a.getBit(0) << std::endl;
	std::cout << "a[2] = " << a.getBit(2) << std::endl;
}