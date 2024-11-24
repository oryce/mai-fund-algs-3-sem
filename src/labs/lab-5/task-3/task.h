#pragma once

#include <cstddef>
#include <ostream>

class logical_values_array {
	unsigned value;

   public:
	logical_values_array(unsigned value_ = 0) : value(value_) {}

	logical_values_array lNeg() const;

	logical_values_array lAnd(const logical_values_array& other) const;

	logical_values_array lOr(const logical_values_array& other) const;

	logical_values_array lXor(const logical_values_array& other) const;

	logical_values_array lXnor(const logical_values_array& other) const;

	logical_values_array lImply(const logical_values_array& other) const;

	logical_values_array lConverse(const logical_values_array& other) const;

	logical_values_array lNor(const logical_values_array& other) const;

	logical_values_array lNand(const logical_values_array& other) const;

	unsigned getValue() const;

	bool equals(const logical_values_array& other) const;

	bool getBit(std::size_t idx) const;

	void toString(char* output) const;

	friend std::ostream& operator<<(std::ostream& stream,
	                                const logical_values_array& lva);
};