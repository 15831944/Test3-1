#include <stdafx.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

#include "ZArrDouble.h"
#include "DebugMemory.h"
#include "InlineUtils.hpp"

//-----------------------------------------------------------------------------
// CODE IMPLEMENTATION
//-----------------------------------------------------------------------------

//=============================================================================
// PROCEDURE:
//    ZArrDouble::ZArrDouble
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Default Constructor (arraySize defaults to 1).
//=============================================================================
ZArrDouble::ZArrDouble(const int arraySize)
{
	prSize = arraySize;
	prA = new double[arraySize];
#ifdef DEBUG_MEMORY
   DebugMemAllocate((pvoid)prA, "ZArrDouble::ZArrDouble(int)");
#endif

}

//-----------------------------------------------------------------------------
ZArrDouble::ZArrDouble(const ZArrDouble & Arr)			// Copy Constructor
{
	prSize = Arr.prSize;
	prA = new double[prSize];
#ifdef DEBUG_MEMORY
   DebugMemAllocate((pvoid)prA, "ZArrDouble::ZArrDouble(const ZArrDouble &)");
#endif

   for (int i = 0; i < prSize; i++)
		prA[i] = Arr.prA[i];
}

//-----------------------------------------------------------------------------
ZArrDouble::ZArrDouble(const int arraySize, const double & InitValue) // Gives an initial value
{
	int	i;

	prSize = arraySize;
	prA = new double[arraySize];
#ifdef DEBUG_MEMORY
   DebugMemAllocate((pvoid)prA, "ZArrDouble::ZArrDouble(int,const double&)");
#endif

   for (i = 0; i < arraySize; i++)
		prA[i] = InitValue;
}

//-----------------------------------------------------------------------------
ZArrDouble::ZArrDouble(const double & ScalarAsArr)
{
   ZArrDouble  tempA(1, ScalarAsArr);
   (*this) = tempA;
}

//-----------------------------------------------------------------------------
ZArrDouble::~ZArrDouble()
{
	if (prA)
	{
#ifdef DEBUG_MEMORY
      DebugMemDestroy((pvoid)prA, "ZArrDouble::~ZArrDouble()");
#endif
		delete [] prA;
	}
}

//-----------------------------------------------------------------------------
double & ZArrDouble::operator[](int index) throw(ZException)
{
	if ((index < 0) || (index >= prSize))
	{
		throw ZException("ZArrDouble::operator[]:Error: index out of range",0);
	}	// ELSE

	return prA[index];
}

//-----------------------------------------------------------------------------
const double & ZArrDouble::operator[](int index) const throw(ZException)
{
	if ((index < 0) || (index >= prSize))
	{
		throw ZException("ZArrDouble::operator[]:Error: index out of range",0);
	}	// ELSE
	return prA[index];
}

//-----------------------------------------------------------------------------
ZArrDouble & ZArrDouble::operator=(const ZArrDouble & rhs)
{
	// this = rhs
	// this = this.operator=(rhs)
	int	i;

	if (rhs.prSize != this->prSize)
	{
		if (prA)
		{
#ifdef DEBUG_MEMORY
         DebugMemDestroy((pvoid)prA, "ZArrDouble::operator=(const ZArrDouble&)");
#endif
			delete [] prA;
		}
		prA = new double[rhs.prSize];
#ifdef DEBUG_MEMORY
      DebugMemAllocate((pvoid)prA, "ZArrDouble::operator=(const ZArrDouble&)");
#endif
		prSize = rhs.prSize;
	}	// IF

	for (i = 0; i < rhs.prSize; i++)
	{
		prA[i] = rhs.prA[i];
	}	// FOR

	return (*this);
}

//-----------------------------------------------------------------------------
ZArrDouble ZArrDouble::operator+(const ZArrDouble & rhs) const throw (ZException)
{
	// A = this + rhs 
	// A = this.operator+(rhs)
	if (rhs.prSize != prSize)
	{
		throw ZException("Error in ZArrDouble::operator+(ZArrDouble & rhs). Different array sizes");
	}

	ZArrDouble A(prSize);

	for (int i = 0; i < prSize; i++)
		A.prA[i] = prA[i] + rhs.prA[i];

	return A;	// Returns a copy of A that is used as the rhs arguament for the
					// assignment operator method
}

//-----------------------------------------------------------------------------
ZArrDouble ZArrDouble::operator-(const ZArrDouble & rhs) const throw (ZException)
{
	// A = this - rhs
	// A = this.operator-(rhs)
	if (rhs.prSize != prSize)
	{
		throw ZException("Error in ZArrDouble::operator-(ZArrDouble & rhs). Different array sizes");
	}

	ZArrDouble A(prSize);

	for (int i = 0; i < prSize; i++)
		A.prA[i] = prA[i] - rhs.prA[i];

	return A;	// Returns a copy of A that is used as the rhs arguament for the
					// assignment operator method
}

//-----------------------------------------------------------------------------
ZArrDouble ZArrDouble::operator*(const double & rhs) const
{
	//	A = this*rhs
	// A = this.operator*(rhs)
	ZArrDouble	A(*this);

	for (int i = 0; i < prSize; i++)
		A.prA[i] *= rhs;

	return A;
}

//-----------------------------------------------------------------------------
ZArrDouble ZArrDouble::operator*(const ZArrDouble & rhs) const
{
   int			size;
   ZArrDouble	tempArr(*this);
   int			i, left, right;

   size = this->prSize + rhs.prSize - 1;
   tempArr.SetLength(size);

   for (i = 0; i < size; i++)
   	tempArr[i] = 0.0;

   for (left = 0; left < this->prSize; left++)
   	for (right = 0; right < rhs.prSize; right++)
      	tempArr[left+right] += this->operator[](left)*rhs[right];

	return tempArr;
}

//-----------------------------------------------------------------------------
ZArrDouble ZArrDouble::operator/(const double & rhs) const
{
	//	A = this/rhs	// rhs == scalar
	// A = this.operator/(rhs)
	ZArrDouble	A(*this);

	for (int i = 0; i < prSize; i++)
		A.prA[i] /= rhs;

	return A;
}

//-----------------------------------------------------------------------------
ZArrDouble & ZArrDouble::operator+=(const ZArrDouble & rhs) throw (ZException)
{
	// this += rhs 
	// this.operator+=(rhs)
	if (rhs.prSize != prSize)
	{
		throw ZException("Error in ZArrDouble::operator+=(ZArrDouble & rhs). Different array sizes");
	}

	for (int i = 0; i < prSize; i++)
		prA[i] += rhs.prA[i];

	return *this;	
}

//-----------------------------------------------------------------------------
ZArrDouble & ZArrDouble::operator-=(const ZArrDouble & rhs)
{
	// this -= rhs 
	// this.operator-=(rhs)
	if (rhs.prSize != prSize)
	{
		throw ZException("Error in ZArrDouble::operator-=(ZArrDouble & rhs). Different array sizes");
	}

	for (int i = 0; i < prSize; i++)
		prA[i] -= rhs.prA[i];

	return *this;
}

//-----------------------------------------------------------------------------
ZArrDouble & ZArrDouble::operator*=(const double & rhs)
{
	//	this *= rhs	// rhs is a scalar
	// this.operator*=(rhs)
	for (int i = 0; i < prSize; i++)
		prA[i] *= rhs;

	return *this;
}

//-----------------------------------------------------------------------------
ZArrDouble & ZArrDouble::operator*=(const ZArrDouble & rhs)
{
   int			size = this->prSize + rhs.prSize - 1;
   ZArrDouble	tempArr(size);
   int			i, left, right;

   for (i = 0; i < size; i++)
   	tempArr[i] = 0.0;

   for (left = 0; left < this->prSize; left++)
   	for (right = 0; right < rhs.prSize; right++)
      	tempArr[left+right] += this->operator[](left)*rhs[right];

   (*this) = tempArr;

	return *this;
}

//-----------------------------------------------------------------------------
ZArrDouble & ZArrDouble::operator/=(const double & rhs)
{
	//	this /= rhs	// rhs is a scalar
	// this.operator/=(rhs)
	for (int i = 0; i < prSize; i++)
		prA[i] /= rhs;

	return *this;
}

//-----------------------------------------------------------------------------
ZArrDouble & ZArrDouble::Set(double arr[], int len)
{
	if (len <= 0)
		return *this;

	if (prSize != len)
	{
		if (prA)
		{
#ifdef DEBUG_MEMORY
         DebugMemDestroy((pvoid)prA, "ZArrDouble::Set(double*,int)");
#endif
			delete [] prA;
		}
		prA = new double[len];
#ifdef DEBUG_MEMORY
      DebugMemAllocate((pvoid)prA, "ZArrDouble::Set(double*,int)");
#endif
		if (prA)
			prSize = len;
		else
			prSize = 0;
	}
	if (prSize)
		memcpy(prA, arr, sizeof(prA));
	return *this;
}

//-----------------------------------------------------------------------------
ZArrDouble & ZArrDouble::Append(double arr[], int len)
{
	double	*pType;
	int	i;

	if (len <= 0)
		return *this;

	pType = new double[len + prSize];
#ifdef DEBUG_MEMORY
   DebugMemAllocate((pvoid)pType, "ZArrDouble::Append(double*,int)");
#endif
	if (!pType)
		return *this;

	// copy old array into first part of new array
	for (i = 0; i < prSize; i++)
		pType[i] = prA[i];

	// copy arr onto last part of new array
	for (i = 0; i < len; i++)
		pType[i+prSize] = arr[i];

	prSize = prSize + len;

	if (prA)
	{
#ifdef DEBUG_MEMORY
      DebugMemDestroy((pvoid)prA, "ZArrDouble::Append(double*,int)");
#endif
		delete [] prA;
	}
	prA = pType;

	return *this;
}

//-----------------------------------------------------------------------------
ZArrDouble & ZArrDouble::Append(double Element)
{
	double	*pType;
	int	i;

	pType = new double[prSize+1];
#ifdef DEBUG_MEMORY
   DebugMemAllocate((pvoid)pType, "ZArrDouble::Append(double)");
#endif
	if (!pType)
		return *this;

	// copy old array into first part of new array
	for (i = 0; i < prSize; i++)
		pType[i] = prA[i];

	// copy arr onto last part of new array
	pType[prSize] = Element;

	prSize++;

	if (prA)
	{
#ifdef DEBUG_MEMORY
      DebugMemDestroy((pvoid)prA, "ZArrDouble::Append(double)");
#endif
		delete [] prA;
	}
	prA = pType;

	return *this;
}
//-----------------------------------------------------------------------------
ZArrDouble & ZArrDouble::Delete(int index) throw(ZException)
{
	if ((index >= prSize) || (index < 0))
   {
      throw ZException("ZArrDouble::Delete: Index out of range",0);
   }

   if (prSize == 1)
   {
#ifdef DEBUG_MEMORY
      DebugMemDestroy((pvoid)prA, "ZArrDouble::Delete(int)");
#endif
   	delete [] prA;
      prSize = 0;
      return *this;
   }

	double	*pType;
	int	i;

	pType = new double[prSize-1];
#ifdef DEBUG_MEMORY
   DebugMemAllocate((pvoid)pType, "ZArrDouble::Delete(int)");
#endif
	if (!pType)
   {
      throw ZException("ZArrDouble::Delete: Out of Memory",0);
   }

	// copy old array upto but not including index into first part of new array
	for (i = 0; i < index; i++)
		pType[i] = prA[i];

	// copy rest of old array after index into rest of new array
	for (i = index+1; i < prSize; i++)
		pType[i-1] = prA[i];

	prSize--;

	if (prA)
	{
#ifdef DEBUG_MEMORY
      DebugMemDestroy((pvoid)prA, "ZArrDouble::Delete(int)");
#endif
		delete [] prA;
	}
	prA = pType;

	return *this;
}

//-----------------------------------------------------------------------------
const double * ZArrDouble::Get(int * len) const
{
	(*len) = prSize;
	return prA;
}

//-----------------------------------------------------------------------------
ZArrDouble ZArrDouble::SubArr(int index, int count) throw (ZException)
{
	ZArrDouble	tempArr(0);

	if ((index < 0) || ((index + count) > prSize))
	{
		throw ZException("ZArrDouble::SubArr:Index/count out of range");
	}

	if (count == 0)
		return tempArr;

	if (count < 0)
	{
		throw ZException("ZArrDouble::SubArr:count < 0");
	}

	tempArr.SetLength(count);
	for (int i = index; i < index+count; i++)
		tempArr[i - index] = prA[i];

	return tempArr;
}

//-----------------------------------------------------------------------------
ZArrDouble ZArrDouble::SubArr(int index)
{
	int count = prSize - index;

	return SubArr(index,count);
}

//-----------------------------------------------------------------------------
const int ZArrDouble::Length(void) const
{
	return prSize;
}

//-----------------------------------------------------------------------------
const int ZArrDouble::Count(void) const
{
	return prSize;
}

//-----------------------------------------------------------------------------
void ZArrDouble::SetLength(int Size)
{
	double	*pType;

	if (prSize != Size)
	{
		pType = new double[Size];
#ifdef DEBUG_MEMORY
      DebugMemAllocate((pvoid)pType, "ZArrDouble::SetLength(int)");
#endif
		for (int i = 0; i < Min(prSize, Size); i++)
			pType[i] = prA[i];
		if (prA)
		{
#ifdef DEBUG_MEMORY
         DebugMemDestroy((pvoid)prA, "ZArrDouble::SetLength(int)");
#endif
			delete [] prA;
		}

		prA = pType;
		prSize = Size;
	}
}

//=============================================================================
// Procedure:
//    ZArrDouble::DoReset
//-----------------------------------------------------------------------------
// Description:
//    Resets the polynomial
//=============================================================================
ZArrDouble & ZArrDouble::DoReset()
{
	if (prA)
	{
#ifdef DEBUG_MEMORY
      DebugMemDestroy((pvoid)prA, "ZArrDouble::DoReset()");
#endif
		delete [] prA;
	}
	prA = NULL;
	prSize = 0;

	return *this;
}

//=============================================================================
// Procedure:
//    ZArrDouble::Minimum
//-----------------------------------------------------------------------------
// Description:
//    Returns the minimum value in the polynomial.
//=============================================================================
double ZArrDouble::Minimum()
{
   if (prSize)
   {
      double   min = prA[0];

      for (int i = 1; i < prSize; i++)
         if (prA[i] < min)
            min = prA[i];

      return min;
   }
   else
      return 0;
}

//=============================================================================
// Procedure:
//    ZArrDouble::Maximum
//-----------------------------------------------------------------------------
// Description:
//    Returns the maximum value in the polynomial.
//=============================================================================
double ZArrDouble::Maximum()
{
   if (prSize)
   {
      double   max = prA[0];

      for (int i = 1; i < prSize; i++)
         if (prA[i] > max)
            max = prA[i];

      return max;
   }
   else
      return 0;
}

//-----------------------------------------------------------------------------
// friend class
std::ostream & operator<<(std::ostream & os, const ZArrDouble & rhs)
{
	int	i;

	if (rhs.prSize == 0)
	{
		os << "[]";
		return os;
	}

	os << "[" << rhs.prA[0];
	for ( i = 1; i < rhs.prSize; i++)
	{
		os << "," << rhs.prA[i];
		if ((i % 5 == 4) && (i != (rhs.prSize-1)))
         os << std::endl;
	} // FOR
	os << "]";

	return os;
}
