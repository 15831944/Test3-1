#ifndef Z_COMPLEX_H
#define Z_COMPLEX_H

#include <iostream>

#include "ZException.hpp"
#include "ZString.h"
//-----------------------------------------------------------------------------
extern const double PI;

//-----------------------------------------------------------------------------
// This next line disables the compiler warning "C++ Exception Specification ignored"
// This relates to the use of throw to indicate the throw status of a member function.
#pragma warning( disable : 4290 )

//-----------------------------------------------------------------------------
class ZComplex
{
   private:
      double	prRe;
      double	prIm;
   public:

      ZComplex(double Re=0.0, double Im=0.0);
      ZComplex(const ZComplex & No);
      ~ZComplex();

      const double Re(void) const;
      void Re(double Value);

      const double Im(void) const;
      void Im(double Value);

      const double Mag(void) const;
      void Mag(double mag);

      const double ArgDeg(void) const;
      void ArgDeg(double deg);

      const double ArgRad(void) const;
      void ArgRad(double rad);

      ZComplex	Conjugate(void) const;

      bool	IsComplex() const;
      bool	IsNull() const;
      bool	IsReal() const;
      bool	IsImag() const;

      // Operator overloading
      bool     operator==(const ZComplex &rhs);

      ZComplex &operator=(const ZComplex & rhs);
      ZComplex &operator=(const double & scalar);

      ZComplex operator+(const double & rhs) const;
      ZComplex operator-(const double & rhs) const;
      ZComplex	operator+(const ZComplex & rhs) const;
      ZComplex	operator-(const ZComplex & rhs) const;

      ZComplex	operator-(void) const;	// unary operator

      ZComplex	operator*(const double & rhs) const;
      ZComplex	operator/(const double & rhs) const throw(ZException);
      ZComplex	operator*(const ZComplex & rhs) const;
      ZComplex	operator/(const ZComplex & rhs) const throw(ZException);
      ZComplex &operator+=(const ZComplex & rhs);
      ZComplex &operator-=(const ZComplex & rhs);
      ZComplex &operator*=(const double & rhs);
      ZComplex &operator/=(const double & rhs) throw(ZException);
      ZComplex &operator*=(const ZComplex & rhs);
      ZComplex &operator/=(const ZComplex & rhs) throw(ZException);

      // Conversion Functions
      //operator double() const;
      operator ZString() const;

      // Streaming
      ZComplex &operator<<(char * rhs) throw(ZException);

      // Friends
      friend std::ostream & operator<<(std::ostream & os, const ZComplex & rhs);

      friend ZComplex operator+(const double lhs, const ZComplex & rhs);
      friend ZComplex operator-(const double lhs, const ZComplex & rhs);
      friend ZComplex operator*(const double lhs, const ZComplex & rhs);
      friend ZComplex operator/(const double lhs, const ZComplex & rhs);

      // Global static library functions
      static ZComplex ZComplex::Sqrt(const double & number);
};

//-----------------------------------------------------------------------------

#endif Z_COMPLEX_H
