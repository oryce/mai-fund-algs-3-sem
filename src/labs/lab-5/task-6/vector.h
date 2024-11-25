#pragma once

#include <compare>
#include <cstddef>
#include <initializer_list>

#include "const_iterator.h"
#include "iterator.h"

class vector {
	const size_t DEFAULT_CAPACITY = 4;

	size_t size_;
	size_t capacity_;
	double* data_;

   public:
	explicit vector(size_t size, double initial);

	explicit vector(size_t size) : vector(size, 0) {}

	vector(std::initializer_list<double> l);

	~vector();

	/* ---------------------------------------------------------------------- */

	double at(size_t index) const;

	double& front();
	const double& front() const;

	double& back();
	const double& back() const;

	double* data();
	const double* data() const;

	bool empty() const;
	size_t size() const;

	void reserve(size_t num);

	size_t capacity() const;

	void shrink_to_fit();

	void clear();

	void insert(size_t index, double elem);
	void erase(size_t index);

	void push_back(double elem);
	void pop_back();

	void resize(size_t size, double elem);

	std::partial_ordering operator<=>(const vector& other) const;
	bool operator==(const vector& other) const;

	iterator begin();
	const_iterator begin() const;

	iterator end();
	const_iterator end() const;
};