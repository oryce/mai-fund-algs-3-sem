#pragma once

#include <compare>
#include <cstddef>
#include <iterator>
#include <stdexcept>

class iterator {
	const double* begin_;
	const double* end_;
	double* position_;

	void advance(ptrdiff_t n);

   public:
	using difference_type = ptrdiff_t;
	using value_type = double;
	using pointer = double*;
	using reference = double&;
	using iterator_category = std::random_access_iterator_tag;

	explicit iterator(const double* begin, const double* end, double* position)
	    : begin_(begin), end_(end), position_(position) {
		if (position_ < begin_ || position_ > end_) {
			throw std::invalid_argument(
			    "position must be between 'begin' and 'end'");
		}
	}

	iterator(const iterator& iterator) = default;

	/* ---------------------------------------------------------------------- */

	iterator& operator++();
	iterator operator++(int);

	iterator& operator--();
	iterator operator--(int);

	iterator operator+(ptrdiff_t n) const;
	iterator& operator+=(ptrdiff_t n);

	iterator operator-(ptrdiff_t n) const;
	iterator& operator-=(ptrdiff_t n);

	ptrdiff_t operator-(const iterator& other) const;

	bool operator==(const iterator& other) const = default;
	bool operator!=(const iterator& other) const = default;
	std::partial_ordering operator<=>(const iterator& other) const;

	double& operator->() const;
	double& operator*() const;
	double& operator[](ptrdiff_t n) const;
};