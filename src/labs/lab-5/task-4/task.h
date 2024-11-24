#pragma once

#include <ostream>

class complex {
   private:
	double re;
	double im;

   public:
	complex(double re_ = 0, double im_ = 0) : re(re_), im(im_) {}

	complex add(const complex&) const;

	complex sub(const complex&) const;

	complex mul(const complex&) const;

	complex div(const complex&) const;

	double abs() const;

	double arg() const;

	friend std::ostream& operator<<(std::ostream&, const complex&);
};