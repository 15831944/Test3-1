#ifndef Z_ARR_DOUBLE_H
#define Z_ARR_DOUBLE_H
//-----------------------------------------------------------------------------
#include <stdlib.h>
#include <iostream>

#include "ZException.hpp"

//#ifdef OPTIMISATIONLIB_EXPORTS
#define DllImportExport __declspec( dllexport ) 
//#else
//#define DllImportExport __declspec( dllimport )
//#endif

//-----------------------------------------------------------------------------
// This next line disables the compiler warning "C++ Exception Specification ignored"
// This relates to the use of throw to indicate the throw status of a member function.
#pragma warning( disable : 4290 )

//-----------------------------------------------------------------------------
class DllImportExport ZArrDouble
{
	protected:
		int prSize;
		double *prA;
	public:
			// Constructors
		ZArrDouble(const int arraySize=1);		// Default Constructor
		ZArrDouble(const ZArrDouble & Arr);		// Copy Constructor
		ZArrDouble(const int arraySize, const double & InitValue);	
                                             // Gives an initial value
      ZArrDouble(const double & ScalarAsArr);
		~ZArrDouble();						// Destructor

			// Operator Overload Methods
		double & operator[](int index) throw(ZException);
		const double &operator[](int index) const throw(ZException);
		ZArrDouble &operator=(const ZArrDouble & rhs);
		ZArrDouble  operator+(const ZArrDouble & rhs) const throw (ZException);
		ZArrDouble  operator-(const ZArrDouble & rhs) const throw (ZException);
		ZArrDouble  operator*(const double & rhs) const;
		ZArrDouble  operator*(const ZArrDouble & rhs) const;
		ZArrDouble  operator/(const double & rhs) const;
		ZArrDouble &operator+=(const ZArrDouble & rhs) throw (ZException);
		ZArrDouble &operator-=(const ZArrDouble & rhs);
		ZArrDouble &operator*=(const double & rhs);
		ZArrDouble &operator*=(const ZArrDouble & rhs);
		ZArrDouble &operator/=(const double & rhs);

		ZArrDouble &Set(double arr[], int len);
		ZArrDouble &Append(double arr[], int len);
		ZArrDouble &Append(double element);
      ZArrDouble &Delete(int index) throw(ZException);
		const double* Get(int *len) const;
		ZArrDouble	SubArr(int index, int count) throw (ZException);
		ZArrDouble	SubArr(int index);	// to end of poly
		const int	Length(void) const;
		const int	Count(void) const;
		void			SetLength(int size);
		ZArrDouble &DoReset();
      double      Minimum();
      double      Maximum();

			// Friends
      friend std::ostream & operator<<(std::ostream & os, const ZArrDouble & rhs);
};

#undef DllImportExport

#endif Z_ARR_DOUBLE_H
