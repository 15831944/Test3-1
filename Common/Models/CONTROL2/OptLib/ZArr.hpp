#ifndef Z_ARR_HPP
#define Z_ARR_HPP
//-----------------------------------------------------------------------------
#include <stdlib.h>

#include "ZComplex.h"

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template <typename Type>
class ZArr
{
	protected:
		int prSize;
		Type *prA;
	public:
      //----------
      // Constructors/Destructors
      //----------
      ZArr(int arraySize=0);		// Default Constructor
      ZArr(const ZArr & Arr);			// Copy Constructor
      ZArr(int arraySize, const Type & InitValue);	// Gives an initial value
      ~ZArr();						// Destructor

      //----------
      // Operator Overload Methods
      //----------
      Type & operator[](int index) throw(ZException);             // Read/Write
      const Type & operator[](int index) const throw(ZException); // Read only

      ZArr & operator=(const ZArr & rhs);
      ZArr operator+(const ZArr & rhs);
      ZArr operator-(const ZArr & rhs);
      ZArr operator*(const Type & rhs);
      ZArr operator*(const ZArr & rhs);
      ZArr operator/(const Type & rhs);
      ZArr & operator+=(const ZArr & rhs);
      ZArr & operator-=(const ZArr & rhs);
      ZArr & operator*=(const Type & rhs);
      ZArr & operator*=(const ZArr & rhs);
      ZArr & operator/=(const Type & rhs);

      ZArr &		Set(Type arr[], int len);
      ZArr &		Append(Type arr[], int len);
      ZArr &		Append(Type element);
      ZArr &		Delete(int index) throw(ZException);
      ZArr &		DoReset();

      const Type* Get(int *len) const;
      Type *      GetArrPointer(void) const;

      ZArr			SubArr(int index, int count);
      ZArr			SubArr(int index);	// to end of poly
      const int	Length(void) const;
      const int	Count(void) const;
      void			SetLength(int size);

      //----------
      // Friends
      //----------
      friend std::ostream & operator<<(std::ostream & os, const ZArr<Type> & rhs);
};

//-----------------------------------------------------------------------------
//	ZARR SPECIALIZATIONS
//-----------------------------------------------------------------------------
typedef ZArr<ZComplex>	ZArrComplex;


#endif Z_ARR_HPP
