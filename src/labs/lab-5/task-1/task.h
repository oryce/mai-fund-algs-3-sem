#pragma once

#include <vector>

class binary_int {
   private:
	int n;

   public:
	binary_int(int n_) : n(n_) {}

	/** Overload for -bint. */
	binary_int operator-() const;
	/** Overload for ++bint. */
	binary_int& operator++();
	/** Overload for --bint. */
	binary_int& operator--();
	/** Overload for bint++. */
	binary_int operator++(int);
	/** Overload for bint--. */
	binary_int operator--(int);
	/** Overload for a + b. */
	binary_int operator+(const binary_int&) const;
	/** Overload for a - b. */
	binary_int operator-(const binary_int&) const;
	/** Overload for a += b. */
	binary_int& operator+=(const binary_int&);
	/** Overload for a -= b. */
	binary_int& operator-=(const binary_int&);
	/** Overload for a >> b. */
	binary_int operator>>(const binary_int&) const;
	/** Overload for a << b. */
	binary_int operator<<(const binary_int&) const;
	/** Overload for a >>= b. */
	binary_int& operator>>=(const binary_int&);
	/** Overload for a <<= b. */
	binary_int& operator<<=(const binary_int&);
	/** Overload for a * b. */
	binary_int operator*(const binary_int&) const;
	/** Overload for a *= b. */
	binary_int& operator*=(const binary_int&);

	friend std::ostream& operator<<(std::ostream&, const binary_int&);

	std::pair<binary_int, binary_int> split() const;
};