#pragma once

#include <compare>
#include <cstddef>
#include <iterator>
#include <stdexcept>

class const_iterator {
	const double* begin_;
	const double* end_;
	const double* position_;

	void advance(ptrdiff_t n);

   public:
	using difference_type = ptrdiff_t;
	using value_type = double;
	using pointer = const double*;
	using reference = const double&;
	using iterator_category = std::random_access_iterator_tag;

	explicit const_iterator(const double* begin, const double* end,
	                        const double* position)
	    : begin_(begin), end_(end), position_(position) {
		if (position_ < begin_ || position_ > end_) {
			throw std::invalid_argument(
			    "position must be between 'begin' and 'end'");
		}
	}

	const_iterator(const const_iterator& iterator) = default;

	/* ---------------------------------------------------------------------- */

	const_iterator& operator++();
	const_iterator operator++(int);

	const_iterator& operator--();
	const_iterator operator--(int);

	const_iterator operator+(ptrdiff_t n) const;
	const_iterator& operator+=(ptrdiff_t n);

	const_iterator operator-(ptrdiff_t n) const;
	const_iterator& operator-=(ptrdiff_t n);

	ptrdiff_t operator-(const const_iterator& other) const;

	bool operator==(const const_iterator& other) const = default;
	bool operator!=(const const_iterator& other) const = default;
	std::partial_ordering operator<=>(const const_iterator& other) const;

	const double& operator->() const;
	const double& operator*() const;
	const double& operator[](ptrdiff_t n) const;
};