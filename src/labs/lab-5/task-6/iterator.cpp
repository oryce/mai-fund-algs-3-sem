#include "iterator.h"

#include <stdexcept>

void iterator::advance(ptrdiff_t n) {
	if (position_ + n < begin_ || position_ + n > end_) {
		throw std::out_of_range("iterator index out of range");
	}
	position_ += n;
}

iterator& iterator::operator++() { return *this += 1; }

iterator iterator::operator++(int) {
	iterator previous = *this;

	++(*this);

	return previous;
}

iterator& iterator::operator--() { return *this -= 1; }

iterator iterator::operator--(int) {
	iterator previous = *this;

	--(*this);

	return previous;
}

iterator iterator::operator+(ptrdiff_t n) const {
	iterator iter = *this;

	iter.advance(n);

	return iter;
}

iterator& iterator::operator+=(ptrdiff_t n) {
	advance(n);
	return *this;
}

iterator iterator::operator-(ptrdiff_t n) const { return *this + (-n); }

iterator& iterator::operator-=(ptrdiff_t n) { return *this += (-n); }

std::partial_ordering iterator::operator<=>(const iterator& other) const {
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

double& iterator::operator->() const {
	if (position_ == end_) {
		throw std::logic_error("can't deref iterator on it's end");
	}
	return *position_;
}

double& iterator::operator*() const {
	if (position_ == end_) {
		throw std::logic_error("can't deref iterator on it's end");
	}
	return *position_;
}

double& iterator::operator[](ptrdiff_t n) const { return *(*this + n); }