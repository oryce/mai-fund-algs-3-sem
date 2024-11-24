#include "task.h"

#include <iostream>
#include <limits>

binary_int binary_int::operator-() const {
	return binary_int(~n) + binary_int(1);
}

binary_int& binary_int::operator++() {
	int mask = 1;

	while (n & mask) {
		n &= ~mask;
		mask <<= 1;
	}

	n |= mask;
	return *this;
}

binary_int& binary_int::operator--() {
	int mask = 1;

	while (!(n & mask)) {
		n |= mask;
		mask <<= 1;
	}

	n &= ~mask;
	return *this;
}

binary_int binary_int::operator++(int) {
	binary_int this0 = *this;

	++(*this);

	return this0;
}

binary_int binary_int::operator--(int) {
	binary_int this0 = *this;

	--(*this);

	return this0;
}

binary_int binary_int::operator+(const binary_int& other) const {
	int a = this->n;
	int b = other.n;

	while (b) {
		int carry = a & b;
		a = a ^ b;
		b = carry << 1;
	}

	return binary_int(a);
}

binary_int binary_int::operator-(const binary_int& other) const {
	return *this + (-other);
}

binary_int& binary_int::operator+=(const binary_int& other) {
	*this = *this + other;
	return *this;
}

binary_int& binary_int::operator-=(const binary_int& other) {
	*this = *this + (-other);
	return *this;
}

binary_int binary_int::operator>>(const binary_int& other) const {
	return binary_int(this->n >> other.n);
}

binary_int binary_int::operator<<(const binary_int& other) const {
	return binary_int(this->n << other.n);
}

binary_int& binary_int::operator>>=(const binary_int& other) {
	*this = *this >> other;
	return *this;
}

binary_int& binary_int::operator<<=(const binary_int& other) {
	*this = *this << other;
	return *this;
}

binary_int binary_int::operator*(const binary_int& other) const {
	if (this->n < 0 && other.n < 0) {
		return -(*this) * -other;
	} else if (other.n < 0) {
		return other * (*this);
	}

	binary_int result(0);
	binary_int times = other;

	while (times.n > 0) {
		result += *this;
		--times;
	}

	return result;
}

binary_int& binary_int::operator*=(const binary_int& other) {
	*this = *this * other;
	return *this;
}

std::ostream& operator<<(std::ostream& stream, const binary_int& bint) {
	stream << std::bitset<sizeof(int) * 8>(bint.n);
	return stream;
}

std::pair<binary_int, binary_int> binary_int::split() const {
	size_t split = std::numeric_limits<int>::digits >> 1;

	unsigned n0 = n;
	int hi = (int)(n0 & (~0U << split));
	int lo = (int)(n0 & (~0U >> split));

	return std::make_pair(binary_int(hi), binary_int(lo));
}