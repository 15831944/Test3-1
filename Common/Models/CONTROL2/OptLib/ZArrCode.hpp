#ifndef Z_ARR_CODE_HPP
#define Z_ARR_CODE_HPP
//-----------------------------------------------------------------------------
#include <stdlib.h>

#include "ZComplex.h"
#include "ZArr.hpp"

//-----------------------------------------------------------------------------
#ifdef DEBUG_MEMORY
#include "DebugMemory.h"
#endif
//-----------------------------------------------------------------------------

//#define DEBUG_MSG

//-----------------------------------------------------------------------------
#ifdef DEBUG_MSG
void print_carr_debug_msg(char *msg)
{
   printf("ZArr::Debug:<%s>\n", msg);
}
#endif DEBUG_MSG

//-----------------------------------------------------------------------------
// CODE IMPLEMENTATION
//-----------------------------------------------------------------------------
template <typename Type>
ZArr<Type>::ZArr(int arraySize)
{
#ifdef DEBUG_MSG
   print_carr_debug_msg("\"ZArr(int arraySize)\" called");
#endif
   prSize = arraySize;
   prA = new Type[arraySize];
#ifdef DEBUG_MEMORY
   DebugMemAllocate((pvoid)prA, "ZArr::ZArr(int)");
#endif

}

//-----------------------------------------------------------------------------
template <typename Type>
ZArr<Type>::ZArr(const ZArr & Arr)			// Copy Constructor
{
#ifdef DEBUG_MSG
   print_carr_debug_msg("Copy constructor called");
#endif
   prSize = Arr.prSize;
   prA = new Type[prSize];
#ifdef DEBUG_MEMORY
   DebugMemAllocate((pvoid)prA, "ZArr::ZArr(ZArr&)");
#endif
   for (int i = 0; i < prSize; i++)
      prA[i] = Arr.prA[i];
}
//-----------------------------------------------------------------------------
template <typename Type>
ZArr<Type>::ZArr(int arraySize, const Type & InitValue)	// Gives an initial value
{
#ifdef DEBUG_MSG
   print_carr_debug_msg("\"ZArr(int arraySize, const Type & InitValue)\" called");
#endif
   int   i;

   prSize = arraySize;
   prA = new Type[arraySize];
#ifdef DEBUG_MEMORY
   DebugMemAllocate((pvoid)prA, "ZArr::ZArr(int,Type&)");
#endif
   for (i = 0; i < arraySize; i++)
      prA[i] = InitValue;
};

//-----------------------------------------------------------------------------
template <typename Type>
ZArr<Type>::~ZArr()
{
#ifdef DEBUG_MSG
   print_carr_debug_msg("Destructor called");
#endif
   if (prA)
   {
#ifdef DEBUG_MEMORY
      DebugMemDestroy((pvoid)prA, "ZArr::~ZArr");
#endif
      delete [] prA;
   }
}
//-----------------------------------------------------------------------------
template <typename Type>
Type & ZArr<Type>::operator[](int index) throw(ZException)
{
#ifdef DEBUG_MSG
   print_carr_debug_msg("\"operator[](int index)\" called");
#endif
   if ((index < 0) || (index >= prSize))
   {
      throw ZException("ZArr::operator[]:Error: index out of range",0);
   }	// ELSE

   return prA[index];
};

//-----------------------------------------------------------------------------
template <typename Type>
const Type & ZArr<Type>::operator[](int index) const throw(ZException)
{
#ifdef DEBUG_MSG
   print_carr_debug_msg("\"const operator[](int index) const\" called");
#endif
   if ((index < 0) || (index >= prSize))
   {
      throw ZException("ZArr::operator[]:Error: index out of range",0);
   }	// ELSE
   return prA[index];
}

//-----------------------------------------------------------------------------
template <typename Type>
ZArr<Type> & ZArr<Type>::operator=(const ZArr<Type> & rhs)
{
	// this = rhs
	// this = this.operator=(rhs)
#ifdef DEBUG_MSG
   print_carr_debug_msg("\"operator=(ZArr<Type> & rhs)\" called");
#endif
   int   i;

   if (rhs.prSize != this->prSize)
   {
      if (prA)
      {
#ifdef DEBUG_MEMORY
         DebugMemDestroy((pvoid)prA, "ZArr::operator=(ZArr&)");
#endif
         delete [] prA;
      }
      prA = new Type[rhs.prSize];
#ifdef DEBUG_MEMORY
      DebugMemAllocate((pvoid)prA, "ZArr::operator=(ZArr&)");
#endif
      prSize = rhs.prSize;
   }  // IF

   for (i = 0; i < rhs.prSize; i++)
   {
      prA[i] = rhs.prA[i];
   }  // FOR

   return (*this);
}

//-----------------------------------------------------------------------------
template <typename Type>
ZArr<Type> ZArr<Type>::operator+(const ZArr<Type> & rhs)
{
   // A = this + rhs 
   // A = this.operator+(rhs)
   if (rhs.prSize != prSize)
   {
      cout << "Error in ZArr<Type>::operator+(ZArr<Type> & rhs). Different array sizes\n";
      return this;
   }

   ZArr<Type> A(this.prSize);

   for (int i = 0; i < prSize; i++)
      A.prA[i] = prA[i] + rhs.prA[i];

   return A;	// Returns a copy of A that is used as the rhs arguament for the
               // assignment operator method
}

//-----------------------------------------------------------------------------
template <typename Type>
ZArr<Type> ZArr<Type>::operator-(const ZArr<Type> & rhs)
{
	// A = this - rhs
	// A = this.operator-(rhs)
	if (rhs.prSize != prSize)
	{
		cout << "Error in ZArr<Type>::operator-(ZArr<Type> & rhs). Different array sizes\n";
		return this;
	}

	ZArr<Type> A(this.prSize);

	for (int i = 0; i < prSize; i++)
		A.prA[i] = prA[i] - rhs.prA[i];

	return A;	// Returns a copy of A that is used as the rhs arguament for the
					// assignment operator method
}

//-----------------------------------------------------------------------------
template <typename Type>
ZArr<Type> ZArr<Type>::operator*(const Type & rhs)
{
	//	A = this*rhs
	// A = this.operator*(rhs)
	ZArr<Type>	A(*this);

	for (int i = 0; i < prSize; i++)
		A.prA[i] *= rhs;

	return A;
}

//-----------------------------------------------------------------------------
template <typename Type>
ZArr<Type> ZArr<Type>::operator*(const ZArr<Type> & rhs)
{
   int			size = this->prSize + rhs.prSize - 1;
   ZArr<Type>	tempArr(size);
   int			i, left, right;

   for (i = 0; i < size; i++)
   	tempArr[i] = 0.0;

   for (left = 0; left < this->prSize; left++)
   	for (right = 0; right < rhs.prSize; right++)
      	tempArr[left+right] += this->operator[](left)*rhs[right];

	return tempArr;
}

//-----------------------------------------------------------------------------
template <typename Type>
ZArr<Type> ZArr<Type>::operator/(const Type & rhs)
{
	//	A = this/rhs	// rhs == scalar
	// A = this.operator/(rhs)
	ZArr<Type>	A(*this);

	for (int i = 0; i < prSize; i++)
		A.prA[i] /= rhs;

	return A;
}

//-----------------------------------------------------------------------------
template <typename Type>
ZArr<Type> & ZArr<Type>::operator+=(const ZArr<Type> & rhs)
{
	// this += rhs 
	// this.operator+=(rhs)
	if (rhs.prSize != prSize)
	{
		cout << "Error in ZArr<Type>::operator+=(ZArr<Type> & rhs). Different array sizes\n";
		return *this;
	}

	for (int i = 0; i < prSize; i++)
		prA[i] += rhs.prA[i];

	return *this;	
}

//-----------------------------------------------------------------------------
template <typename Type>
ZArr<Type> & ZArr<Type>::operator-=(const ZArr<Type> & rhs)
{
	// this -= rhs 
	// this.operator-=(rhs)
	if (rhs.prSize != prSize)
	{
		cout << "Error in ZArr<Type>::operator-=(ZArr<Type> & rhs). Different array sizes\n";
		return *this;
	}

	for (int i = 0; i < prSize; i++)
		prA[i] -= rhs.prA[i];

	return *this;
}

//-----------------------------------------------------------------------------
template <typename Type>
ZArr<Type> & ZArr<Type>::operator*=(const Type & rhs)
{
	//	this *= rhs	// rhs is a scalar
	// this.operator*=(rhs)
	for (int i = 0; i < prSize; i++)
		prA[i] *= rhs;

	return *this;
}

//-----------------------------------------------------------------------------
template <typename Type>
ZArr<Type> & ZArr<Type>::operator*=(const ZArr<Type> & rhs)
{
   int			size = this->prSize + rhs.prSize - 1;
   ZArr<Type>	tempArr(size);
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
template <typename Type>
ZArr<Type> & ZArr<Type>::operator/=(const Type & rhs)
{
	//	this /= rhs	// rhs is a scalar
	// this.operator/=(rhs)
	for (int i = 0; i < prSize; i++)
		prA[i] /= rhs;

	return *this;
}

//-----------------------------------------------------------------------------
template <typename Type>
ZArr<Type> & ZArr<Type>::Set(Type arr[], int len)
{
	if (len <= 0)
		return *this;

	if (prSize != len)
	{
		if (prA)
		{
#ifdef DEBUG_MEMORY
			DebugMemDestroy((pvoid)prA, "ZArr::Set(Type[],int)");
#endif
			delete [] prA;
		}
		prA = new Type[len];
#ifdef DEBUG_MEMORY
		DebugMemAllocate((pvoid)prA, "ZArr::Set(Type[],int)");
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
template <typename Type>
ZArr<Type> & ZArr<Type>::Append(Type arr[], int len)
{
	Type	*pType;
	int	i;

	if (len <= 0)
		return *this;

	pType = new Type[len + prSize];
#ifdef DEBUG_MEMORY
	DebugMemAllocate((pvoid)pType, "ZArr::Append(Type,int)");
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
		DebugMemDestroy((pvoid)prA, "ZArr::Append(Type[],int)");
#endif
		delete [] prA;
	}
	prA = pType;

	return *this;
}

//-----------------------------------------------------------------------------
template <typename Type>
ZArr<Type> & ZArr<Type>::Append(Type Element)
{
	Type	*pType;
	int	i;

	pType = new Type[prSize+1];
#ifdef DEBUG_MEMORY
	DebugMemAllocate((pvoid)pType, "ZArr::Append(Type)");
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
		DebugMemDestroy((pvoid)prA, "ZArr::Append(Type)");
#endif
		delete [] prA;
	}
	prA = pType;

	return *this;
}
//-----------------------------------------------------------------------------
template <typename Type>
ZArr<Type> & ZArr<Type>::Delete(int index) throw(ZException)
{
	if ((index >= prSize) || (index < 0))
   {
      throw ZException("ZArr<Type>::Delete: Index out of range",0);
   }

   if (prSize == 1)
   {
#ifdef DEBUG_MEMORY
		DebugMemDestroy((pvoid)prA, "ZArr::Delete(int)");
#endif
   	delete [] prA;
      prSize = 0;
      return *this;
   }

	Type	*pType;
	int	i;

	pType = new Type[prSize-1];
#ifdef DEBUG_MEMORY
	DebugMemAllocate((pvoid)pType, "ZArr::Delete(int)");
#endif
	if (!pType)
   {
      throw ZException("ZArr<Type>::Delete: Out of Memory",0);
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
		DebugMemDestroy((pvoid)prA, "ZArr::Delete(int)");
#endif
		delete [] prA;
	}
	prA = pType;

	return *this;
}

//-----------------------------------------------------------------------------
template <typename Type>
const Type * ZArr<Type>::Get(int * len) const
{
	(*len) = prSize;
	return prA;
}

//-----------------------------------------------------------------------------
template <typename Type>
Type* ZArr<Type>::GetArrPointer(void) const
{
   return prA;
}

//-----------------------------------------------------------------------------
template <typename Type>
ZArr<Type> ZArr<Type>::SubArr(int index, int count)
{
	ZArr	tempArr(0);

	if ((index < 0) || ((index + count) > prSize))
	{
		printf("Index/count out of range");
		return tempArr;
	}

	if (count == 0)
		return tempArr;

	if (count < 0)
	{
		printf("count < 0");
		return tempArr;
	}

	tempArr.SetLength(count);
	for (int i = index; i < index+count; i++)
		tempArr[i - index] = prA[i];

	return tempArr;
}

//-----------------------------------------------------------------------------
template <typename Type>
ZArr<Type> ZArr<Type>::SubArr(int index)
{
	int count = prSize - index;

	return SubArr(index,count);
}

//-----------------------------------------------------------------------------
template <typename Type>
const int ZArr<Type>::Length(void) const
{
	return prSize;
}

//-----------------------------------------------------------------------------
template <typename Type>
const int ZArr<Type>::Count(void) const
{
	return prSize;
}

//-----------------------------------------------------------------------------
template <typename Type>
void ZArr<Type>::SetLength(int Size)
{
	Type	*pType;
   int   n;

	if (prSize != Size)
	{
		pType = new Type[Size];
#ifdef DEBUG_MEMORY
		DebugMemAllocate((pvoid)pType, "ZArr::SetLength(int)");
#endif
      n = prSize;
      if (n > Size)
         n = Size;
		for (int i = 0; i < n; i++)
			pType[i] = prA[i];
		if (prA)
		{
#ifdef DEBUG_MEMORY
			DebugMemDestroy((pvoid)prA, "ZArr::SetLength(int)");
#endif
			delete [] prA;
		}

		prA = pType;
		prSize = Size;
	}
}

//-----------------------------------------------------------------------------
template <typename Type>
ZArr<Type> & ZArr<Type>::DoReset()
{
	if (prA)
	{
#ifdef DEBUG_MEMORY
		DebugMemDestroy((pvoid)prA, "ZArr::DoReset()");
#endif
		delete [] prA;
	}
	prA = NULL;
	prSize = 0;

	return *this;
}

//-----------------------------------------------------------------------------
// friend class
template <typename Type>
std::ostream & operator<<(std::ostream & os, const ZArr<Type> & rhs)
{
#ifdef DEBUG_MSG
	print_carr_debug_msg("\"operator<<(ostream & os, ZArr<Type> & rhs)\" called");
#endif
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
			os << endl;
	} // FOR
	os << "]";

	return os;
}
//-----------------------------------------------------------------------------
#endif Z_ARR_CODE_HPP