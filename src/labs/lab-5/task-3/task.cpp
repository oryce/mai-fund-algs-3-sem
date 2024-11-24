#include "task.h"

#include <algorithm>
#include <stdexcept>
#include <string>

unsigned logical_values_array::getValue() const { return value; }

logical_values_array logical_values_array::lNeg() const { return ~value; }

logical_values_array logical_values_array::lAnd(
    const logical_values_array& other) const {
	return value & other.value;
}

logical_values_array logical_values_array::lOr(
    const logical_values_array& other) const {
	return value | other.value;
}

logical_values_array logical_values_array::lXor(
    const logical_values_array& other) const {
	return value ^ other.value;
}

logical_values_array logical_values_array::lXnor(
    const logical_values_array& other) const {
	return lXor(other).lNeg(); /* ~(this ^ other) */
}

logical_values_array logical_values_array::lImply(
    const logical_values_array& other) const {
	return lNeg().lOr(other) /* ~this | other */;
}

logical_values_array logical_values_array::lConverse(
    const logical_values_array& other) const {
	return lOr(other.lNeg()) /* this | ~other */;
}

logical_values_array logical_values_array::lNor(
    const logical_values_array& other) const {
	return lOr(other).lNeg() /* ~(this | other) */;
}

logical_values_array logical_values_array::lNand(
    const logical_values_array& other) const {
	return lAnd(other).lNeg(); /* ~(this & other) */
}

bool logical_values_array::equals(const logical_values_array& other) const {
	return value == other.value;
}

bool logical_values_array::getBit(std::size_t idx) const {
	if (idx > sizeof(value) * 8) {
		throw std::invalid_argument("bit index out of range");
	}

	return (value >> idx) & 1;
}

void logical_values_array::toString(char* output) const {
	if (!output) {
		throw std::invalid_argument("output may not be null");
	}

	std::string string;
	unsigned mask = 1;

	while (mask) {
		string.push_back(value & mask ? '1' : '0');
		mask <<= 1;
	}

	std::reverse(string.begin(), string.end());
	std::strcpy(output, string.c_str());
}

std::ostream& operator<<(std::ostream& stream,
                         const logical_values_array& lva) {
	char buffer[sizeof(lva.value) * 8 + 1];

	lva.toString(buffer);

	return stream << buffer;
}
