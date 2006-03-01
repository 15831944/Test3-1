#ifndef INLINE_UTILS_HPP
#define INLINE_UTILS_HPP
//=============================================================================
#include <math.h>
#include <vector>
#include <list>
#include "ZException.hpp"

#ifdef __BORLANDC__
#pragma warn -8027
#endif

//=============================================================================
// PROCEDURE:
//    ToUpper    
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Converts character to Uppercase.
//=============================================================================
inline char ToUpper(char a)
{
   return (((a >= 'a') && (a <= 'z')) ? a-('a'-'A') : a );
}

//=============================================================================
// PROCEDURE:
//    ToLower    
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Converts character to Lowercase.
//=============================================================================
inline char ToLower(char a)
{
   return ((a >= 'A' && a <= 'Z') ? a+('a'-'A') : a );
}

//=============================================================================
// PROCEDURE:
//    Abs    
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Returns absolute value of parameter x.
//=============================================================================
template<typename Type>
inline Type Abs(Type x)
{ 
   return ((x) >= 0 ? (x) : -(x));
}


//=============================================================================
// PROCEDURE:
//    Min    
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Returns minimum of two parameters.
//=============================================================================
template<typename Type>
inline Type Min(Type a, Type b)
{
   return (a <= b ? a : b);
}

template<typename Type>
inline Type Min(Type a, Type b, Type c)
{
   return Min(Min(a,b),c);
}

template<typename Type>
inline Type Min(Type a, Type b, Type c, Type d)
{
   return Min(Min(Min(a,b),c),d);
}

template<typename Type>
inline Type Min(const std::vector<Type> & vectType)
{
   Type  tMin;
   if (vectType.size()==0)
      throw ZException("InlineUtils::Min(vector<Type>): Vector size is 0");

   tMin = vectType[0];
   for (int __i = 1; __i < vectType.size(); __i++)
   {
      if (vectType[i] < tMin)
         tMin = vectType[i];
   }
   return tMin;
}

template<typename Type>
inline Type Min(const std::list<Type> & listType)
{
   Type  tMin;
   if (listType.size()==0)
      throw ZException("InlineUtils::Min(list<Type>): Vector size is 0");

   std::list<Type>::iterator itList = listType.begin();

   tMin = *itList;
   itList++;
   while (itList != listType.end())
   {
      if ((*itList) < tMin)
         tMin = (*itList);
   }
   return tMin;
}

//=============================================================================
// PROCEDURE:
//    Max
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Returns maximum of two parameters.
//=============================================================================
template<typename Type>
inline Type Max(Type a, Type b)
{
   return (a >= b ? a : b);
}

template<typename Type>
inline Type Max(Type a, Type b, Type c)
{
   return Max(Max(a,b),c);
}

template<typename Type>
inline Type Max(Type a, Type b, Type c, Type d)
{
   return Max(Max(Max(a,b),c),d);
}

template<typename Type>
inline Type Max(const std::vector<Type> & vectType)
{
   Type  tMax;
   if (vectType.size()==0)
      throw ZException("InlineUtils::Max(vector<Type>): Vector size is 0");

   tMax = vectType[0];
   for (int __i = 1; __i < vectType.size(); __i++)
   {
      if (vectType[i] > tMax)
         tMax = vectType[i];
   }
   return tMax;
}

template<typename Type>
inline Type Max(const std::list<Type> & listType)
{
   Type  tMax;
   if (listType.size()==0)
      throw ZException("InlineUtils::Max(list<Type>): Vector size is 0");

   std::list<Type>::iterator itList = listType.begin();

   tMax = *itList;
   itList++;
   while (itList != listType.end())
   {
      if ((*itList) > tMax)
         tMax = (*itList);
   }
   return tMax;
}

//=============================================================================
// PROCEDURE:
//    MinMax
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Returns inVal saturated to within the limits of minVal and maxVal.
//=============================================================================
template<typename Type>
inline Type MinMax(Type inVal, Type minVal, Type maxVal)
{
   if (minVal > maxVal)
      return Max(Min(inVal, minVal), maxVal);
   else
      return Min(Max(inVal, minVal), maxVal);
}

#define Saturate MinMax

//=============================================================================
// PROCEDURE:
//    To_bool
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Converts the value to boolean type. Interprets true or false in the c
// sense of non-zero = true.
//=============================================================================
template<typename Type>
inline bool To_bool(Type Value)
{
   if (Value)
      return true;
   else
      return false;
}

//=============================================================================
// PROCEDURE:
//    Round
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Rounds the floating point number to the nearest integer number.
//=============================================================================
template<typename FloatType>
inline int Round(FloatType Number)
{
	if ((Number - (FloatType)floor(double(Number))) < 0.5)
	  return int(floor(double(Number)));
	else
	  return int(ceil(double(Number)));
}

//=============================================================================
// PROCEDURE:
//    Round2
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Rounds the floating point number to the nearest integer type number.
//=============================================================================
template<typename FloatType, typename IntType>
inline IntType Round2(FloatType Number)
{
	if ((Number - (FloatType)floor(double(Number))) < 0.5)
	  return IntType(floor(double(Number)));
	else
	  return IntType(ceil(double(Number)));
}

//=============================================================================
// PROCEDURE:
//    Exponent
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Inline function that returns the exponent (power of 10) of a number
// as an integer.
//=============================================================================
template<typename FloatType>
inline int Exponent(FloatType Number)
{
   return int(floor(log10(double(Abs(Number)))));
}

//=============================================================================
// PROCEDURE:
//    Mantissa
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Inline function that returns the mantissa (component between 1 & 10) of a
// number.
// Note: 1 <= mantissa < 10.
// Note: The value is converted to double for the purposes of the operation
//=============================================================================
template<typename FloatType>
inline FloatType Mantissa(FloatType Number)
{
   return Number/FloatType( pow(10.0, Exponent(Abs(double(Number)))) );
}

//=============================================================================
// PROCEDURE:
//    Sign
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Returns -1 if the number is negative or 1 if the number is positive or 0.
//=============================================================================
template<typename Type>
inline Type Sign(Type Number)
{
	if (Number < 0)
		return -1;
	else
		return 1;
}

//=============================================================================
// PROCEDURE:
//    Sum
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Calculates the sum of the array passed in.
//=============================================================================
template <typename Type>
inline Type Sum(Type *Arr, int Size)
{
   Type  sum = 0;

   for (int i = 0; i < Size; i++)
   {
      sum += Arr[i];
   }
   return sum;
}

template<typename Type>
inline Type Sum(const std::vector<Type> & vectType)
{
   Type  tSum;

   for (int __i = 0; __i < vectType.size(); __i++)
      tSum = tSum + vectType[i];

   return tSum;
}

template<typename Type>
inline Type Sum(const std::list<Type> & listType)
{
   Type                       tSum;
   std::list<Type>::iterator  itList;

   for (itList = listType.begin(); itList == listType.end(); itList++)
      tSum = tSum + (*itList);

   return tSum;
}


//=============================================================================
// PROCEDURE:
//    Sort
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Sorts the array ascending. Just uses the bubble sort algorithm.
//=============================================================================
template <typename Type>
inline void Sort(Type *Arr, int Size, bool Ascending=true)
{
   Type  tempSwap;

   if (Ascending)
   {
      for (int i = 0; i < (Size-1); i++)
         for (int j = i+1; j < Size; j++)
            if (Arr[i] > Arr[j])
            {
               tempSwap = Arr[i];
               Arr[i] = Arr[j];
               Arr[j] = tempSwap;
            }
   }
   else
   {
      for (int i = 0; i < (Size-1); i++)
         for (int j = i+1; j < Size; j++)
            if (Arr[i] < Arr[j])
            {
               tempSwap = Arr[i];
               Arr[i] = Arr[j];
               Arr[j] = tempSwap;
            }
   }
}

//=============================================================================
// PROCEDURE:
//    Modulus
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Returns the remainder after the first arguament is divided by the second.
// The Type cannot be a floating point type.
//=============================================================================
template <typename Type>
inline int Modulus(const Type &d1, const Type &d2)
{
   return d1 % d2;
}

//=============================================================================
// PROCEDURE:
//    Odd
//-----------------------------------------------------------------------------
// PARAMETER LIST:
//    const Type &a
//       The value to perform the test on.
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Returns whether or not the integer value is Odd.
//    The Type cannot be a floating point type.
//=============================================================================
template <typename Type>
inline bool Odd(const Type &a)
{
   return (Modulus(a,Type(2))==Type(1));
}

//=============================================================================
// PROCEDURE:
//    Even
//-----------------------------------------------------------------------------
// PARAMETER LIST:
//    const Type &a
//       The value to perform the test on.
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Returns whether or not the integer value is Even.
//    The Type cannot be a floating point type.
//=============================================================================
template <typename Type>
inline bool Even(const Type &a)
{
   return !Odd(a);
}

//=============================================================================
// CONST VARIABLE:
//    MAX_ULONG
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Used by DWTimeDiff. There is a value ULONG_MAX defined in a windows
// header file but I had too many problems using it. Besides the use of const's
// is better.
//=============================================================================
const unsigned long  MAX_ULONG = 0xffffffff;

//=============================================================================
// PROCEDURE:
//    DWTimeDiff
//-----------------------------------------------------------------------------
// PARAMETERS:
//    dwStartTimeMS
//       The start time.
//    dwEndTimeMS
//       The end time.
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Finds the time difference between the two unsigned times. This method
// is designed to deal with the wrap-around that occurs with all integers.
// It is particularly designed for timeGetTime() which returns a DWORD in
// milliseconds since the time Windows started, and hence wraps around every
// 50 days.
//=============================================================================
inline unsigned TimeDiff(unsigned dwStartTime, unsigned dwEndTime)
{
   if (dwEndTime > dwStartTime)
      return (dwEndTime - dwStartTime);
   else
      return (dwEndTime + (MAX_ULONG - dwStartTime) + 1);
}

//=============================================================================
#endif INLINE_UTILS_HPP