#include "vector.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

vector::vector(size_t size, double initial)
    : size_(size), capacity_(size), data_(new double[size]) {
	std::fill(data_, data_ + size_, initial);
}

vector::vector(std::initializer_list<double> l)
    : size_(l.size()), capacity_(l.size()), data_(new double[l.size()]) {
	std::copy(l.begin(), l.end(), data_);
}

vector::~vector() { delete[] data_; }

double vector::at(size_t index) const {
	if (index >= size_) {
		throw std::range_error("index out of bounds");
	}
	return data_[index];
}

double& vector::front() {
	if (empty()) {
		throw std::logic_error("vector is empty");
	}
	return data_[0];
}

const double& vector::front() const {
	return const_cast<vector*>(this)->front();
}

double& vector::back() {
	if (empty()) {
		throw std::logic_error("vector is empty");
	}
	return data_[size_ - 1];
}

const double& vector::back() const { return const_cast<vector*>(this)->back(); }

double* vector::data() { return data_; }

const double* vector::data() const { return data_; }

bool vector::empty() const { return size_ == 0; }

size_t vector::size() const { return size_; }

void vector::reserve(size_t num) {
	if (capacity_ >= num) {
		return;
	}

	auto newData = new double[num];
	std::copy(data_, data_ + size_, newData);

	delete[] data_;

	data_ = newData;
	capacity_ = num;
}

size_t vector::capacity() const { return capacity_; }

void vector::shrink_to_fit() {
	if (capacity_ == size_) {
		return;
	}
	resize(size_, 0);
}

void vector::clear() {
	size_ = 0;
	shrink_to_fit();
}

void vector::insert(size_t index, double elem) {
	if (index > size_) {
		throw std::range_error("index out of bounds");
	}

	if (capacity_ == 0) {
		reserve(DEFAULT_CAPACITY);
	} else if (size_ == capacity_) {
		reserve(capacity_ * 2);
	}

	// Shift all elements by one to the right to fit the inserted value:
	// [ ... a_i, a_i+1, ..., a_n ] --> [ ... [] a_i, a_i+1, ..., a_n ]
	std::move(data_ + index, data_ + size_, data_ + index + 1);

	// Insert the value
	data_[index] = elem;

	size_++;
}

void vector::erase(size_t index) {
	if (index >= size_) {
		throw std::range_error("index out of bounds");
	}

	// Shift all elements by one to the left to remove a_i:
	// [ ... a_i, a_i+1, ..., a_n ] --> [ ... a_i+1, ..., a_n ]
	std::move(data_ + index + 1, data_ + size_, data_ + index);

	// Shrink the buffer if needed
	size_--;
	if (size_ >= DEFAULT_CAPACITY && size_ <= capacity_ / 2) {
		resize(capacity_ / 2, 0);
	}
}

void vector::push_back(double elem) {
	if (capacity_ == 0) {
		reserve(DEFAULT_CAPACITY);
	} else if (size_ == capacity_) {
		reserve(capacity_ * 2);
	}
	data_[size_++] = elem;
}

void vector::pop_back() {
	if (size_ == 0) {
		throw std::logic_error("vector is empty");
	}

	// Shrink the buffer if needed
	size_--;
	if (size_ >= DEFAULT_CAPACITY && size_ <= capacity_ / 2) {
		resize(capacity_ / 2, 0);
	}
}

void vector::resize(size_t size, double elem) {
	bool expand = size > size_;

	auto newData = new double[size];
	std::copy(data_, data_ + (expand ? size_ : size), newData);

	if (expand) {
		for (size_t i = size_; i != size; ++i) {
			newData[i] = elem;
		}
	}

	delete[] data_;

	data_ = newData;
	size_ = size;
	capacity_ = size;
}

std::partial_ordering vector::operator<=>(const vector& other) const {
	size_t i = 0, j = 0;

	while (i < size() && j < other.size()) {
		double a = at(i), b = other.at(j);

		if (std::isnan(a) || std::isnan(b)) {
			// NaNs can't be compared
			return std::partial_ordering::unordered;
		}
		if (a != b) {
			return a < b ? std::partial_ordering::less
			             : std::partial_ordering::greater;
		}

		++i;
		++j;
	}

	if (i == size() && j == other.size()) {
		// Both vectors have the same amount of items,
		// and they're equal
		return std::partial_ordering::equivalent;
	} else if (i == size()) {
		// |other| has more items
		return std::partial_ordering::less;
	} else {
		// |this| has more items
		return std::partial_ordering::greater;
	}
}

bool vector::operator==(const vector& other) const {
	size_t i = 0, j = 0;

	while (i < size() && j < other.size()) {
		if (at(i) != other.at(j)) {
			return false;
		}
		++i;
		++j;
	}

	return i == size() && j == other.size();
}

iterator vector::begin() {
	return iterator(data_, data_ + size_, /*position=*/data_);
}

const_iterator vector::begin() const {
	return const_iterator(data_, data_ + size_, /*position=*/data_);
}

iterator vector::end() {
	return iterator(data_, data_ + size_, /*position=*/data_ + size_);
}

const_iterator vector::end() const {
	return const_iterator(data_, data_ + size_, /*position=*/data_ + size_);
}