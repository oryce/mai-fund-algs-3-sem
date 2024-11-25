#include "const_iterator.h"

#include <stdexcept>

void const_iterator::advance(ptrdiff_t n) {
	if (position_ + n < begin_ || position_ + n > end_) {
		throw std::out_of_range("iterator index out of range");
	}
	position_ += n;
}

const_iterator& const_iterator::operator++() { return *this += 1; }

const_iterator const_iterator::operator++(int) {
	const_iterator previous = *this;

	++(*this);

	return previous;
}

const_iterator& const_iterator::operator--() { return *this -= 1; }

const_iterator const_iterator::operator--(int) {
	const_iterator previous = *this;

	--(*this);

	return previous;
}

const_iterator const_iterator::operator+(ptrdiff_t n) const {
	const_iterator iter = *this;

	iter.advance(n);

	return iter;
}

const_iterator& const_iterator::operator+=(ptrdiff_t n) {
	advance(n);
	return *this;
}

const_iterator const_iterator::operator-(ptrdiff_t n) const {
	return *this + (-n);
}

const_iterator& const_iterator::operator-=(ptrdiff_t n) {
	return *this += (-n);
}

std::partial_ordering const_iterator::operator<=>(
    const const_iterator& other) const {
	if (begin_ != other.begin_ || end_ != other.end_) {
		return std::partial_ordering::unordered;
	}
	if (position_ == other.position_) {
		return std::partial_ordering::equivalent;
	} else if (position_ < other.position_) {
		return std::partial_ordering::less;
	} else {
		return std::partial_ordering::greater;
	}
}

const double& const_iterator::operator->() const {
	if (position_ == end_) {
		throw std::logic_error("can't deref iterator on it's end");
	}
	return *position_;
}

const double& const_iterator::operator*() const {
	if (position_ == end_) {
		throw std::logic_error("can't deref iterator on it's end");
	}
	return *position_;
}

const double& const_iterator::operator[](ptrdiff_t n) const {
	return *(*this + n);
}