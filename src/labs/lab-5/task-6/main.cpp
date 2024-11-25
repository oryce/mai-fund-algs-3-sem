#include <iostream>

#include "vector.h"

int main() {
	vector v1 = {1, 2, 3, 4};

	v1.push_back(6);
	v1.push_back(7);
	v1.push_back(8);
	v1.push_back(9);
	v1.push_back(10);
	v1.push_back(11);
	v1.push_back(12);

	for (auto item : v1) {
		std::cout << item << " ";
	}
	std::cout << std::endl;

	v1.erase(0);
	v1.erase(2);
	v1.erase(4);

	for (auto item : v1) {
		std::cout << item << " ";
	}
	std::cout << std::endl;

	v1.insert(0, -10);
	v1.insert(v1.size(), -20);

	for (auto item : v1) {
		std::cout << item << " ";
	}
	std::cout << std::endl;

	while (v1.size() != 2) {
		v1.pop_back();
	}

	for (auto item : v1) {
		std::cout << item << " ";
	}
	std::cout << std::endl;

	v1.clear();

	vector v2 = {4, 5, 6, 7};
	vector v3 = {4, 5, 6, 7, 8};
	vector v4 = {4, 5, 6, 5};

	std::cout << "v2 == v2: " << (v2 == v2) << std::endl;
	std::cout << "v2 != v3: " << (v2 != v3) << std::endl;
	std::cout << "v2 == v2: " << (v2 < v3) << std::endl;
	std::cout << "v2 > v4 : " << (v2 > v4) << std::endl;
	std::cout << "v3 > v4 : " << (v3 > v4) << std::endl;

	const vector v5 = {1, 2, 3, 4};

	for (const_iterator iter = v5.begin(); iter != v5.end(); ++iter) {
		std::cout << *iter << " ";
	}
	std::cout << std::endl;

	std::cout << "begin + 2 : " << *(v5.begin() + 2) << std::endl;
	std::cout << "begin[2]  : " << v5.begin()[2] << std::endl;
	std::cout << "end [-2]  : " << v5.end()[-2] << std::endl;
}