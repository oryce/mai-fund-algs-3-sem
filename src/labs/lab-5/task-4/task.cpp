#include "task.h"

#include <stdexcept>

complex complex::add(const complex& other) const {
	return {this->re + other.re, this->im + other.im};
}

complex complex::sub(const complex& other) const {
	return {this->re - other.re, this->im - other.im};
}

complex complex::mul(const complex& other) const {
	return {this->re * other.re - this->im * other.im,
	        this->re * other.im + this->im * other.re};
}

complex complex::div(const complex& other) const {
	if (other.re == 0 && other.im == 0) {
		throw std::domain_error("division by zero");
	}

	double d = other.re * other.re + other.im * other.im;

	return {(this->re * other.re + this->im * other.im) / d,
	        (this->im * other.re - this->re * other.im) / d};
}

double complex::abs() const {
	return std::sqrt(this->re * this->re + this->im * this->im);
}

double complex::arg() const { return std::atan2(this->im, this->re); }

std::ostream& operator<<(std::ostream& stream, const complex& complex) {
	stream << complex.re;
	if (complex.im < 0) {
		stream << "-" << std::abs(complex.im) << "i";
	} else if (complex.im > 0) {
		stream << "+" << std::abs(complex.im) << "i";
	}
	return stream;
}
