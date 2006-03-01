#include <stdafx.h>
#ifdef	__BORLANDC__
#include <vcl.h>
#pragma hdrstop
#endif __BORLANDC__

#include <iostream>
//using namespace std;

#include <math.h>

#include <stdlib.h>
#include "ZMat.h"
#include "ZPoly.h"
#include "ZPolePair.h"
//--------------------
// LOCAL FUNCTIONS
//--------------------
//-----------------------------------------------------------------------------
bool CloseEnough(double value1, double value2)
{
   #define	EPSILON	0.00000000001
   if (fabs(value1) < EPSILON)
   	return fabs(value1 - value2) < EPSILON;
   else
   	return fabs((value1 - value2)/value1) < EPSILON;
}
//--------------------
// CONSTRUCTORS
//--------------------
//-----------------------------------------------------------------------------
ZPoly::ZPoly(const ZArrDouble & Arr, char VarCharValue) : ZArrDouble(Arr)
{
	VarChar = VarCharValue;
	prIOMode = omPoly;
   prNormMode = nmLowest;
   if (prSize == 0)
      Append(1);
}

//-----------------------------------------------------------------------------
ZPoly::ZPoly(int Size): ZArrDouble(Size)
{
	VarChar = 's';
	prIOMode = omPoly;
   prNormMode = nmLowest;

   if (prSize == 0)
      DoReset();  // 1
   else
   {
      DoZero();
      prA[0] = 1.0;  // 0s^n + ... + 0s + 1
   }  // ELSE
}

//-----------------------------------------------------------------------------
ZPoly::ZPoly(const ZPoly & Poly): ZArrDouble(Poly)			// Copy Constructor
{
	VarChar = Poly.VarChar;
	prIOMode = Poly.prIOMode;
   prNormMode = Poly.prNormMode;
}
//-----------------------------------------------------------------------------
ZPoly::ZPoly(int arraySize, double InitValue): ZArrDouble(arraySize, InitValue)	// Gives an initial value
{
	VarChar = 's';
	prIOMode = omPoly;
   prNormMode = nmLowest;

   if (prSize == 0)
   {
      DoReset();
      prA[0] = InitValue;
   }
}

//--------------------
// INPUT/OUTPUT FORMAT MODES
//--------------------
//-----------------------------------------------------------------------------
ZPoly::TIOMode	ZPoly::IOMode(void) const
{
	return prIOMode;
}
//-----------------------------------------------------------------------------
void ZPoly::SetIOMode(TIOMode mode) throw(ZException)
{
	switch (mode)
   {
      case omPoly:
      case omPolyFactored:
      case omMatlabCommas:
      case omMatlabSpaces:
      case omReverseMatlabCommas:
      case omReverseMatlabSpaces:
      	prIOMode = mode;
         break;
      default:
      	throw(ZException("ZPoly::SetIOMode: mode is illegal",0));
   }	// SWITCH
}

//-----------------------------------------------------------------------------
ZPoly::TNormaliseMode ZPoly::NormMode(void) const
{
	return prNormMode;
}
//-----------------------------------------------------------------------------
void ZPoly::SetNormMode(TNormaliseMode mode) throw(ZException)
{
	switch (mode)
   {
      case nmHighest:
      case nmLowest:
      case nmNone:
      	prNormMode = mode;
         break;
      default:
      	throw(ZException("ZPoly::SetNormMode: mode is illegal",0));
   }	// SWITCH
}

//--------------------
//	ASSIGNMENT OPERATORS
//--------------------
//-----------------------------------------------------------------------------
ZPoly & ZPoly::operator=(const ZArrDouble & rhs)
{
	this->ZArrDouble::operator=(rhs);

	return *this;
}

//-----------------------------------------------------------------------------
ZPoly & ZPoly::operator=(const ZPoly & rhs)
{
	this->ZArrDouble::operator=(rhs);
	this->VarChar = rhs.VarChar;
	this->prIOMode = rhs.prIOMode;
	this->prNormMode = rhs.prNormMode;

	return *this;
}

//-----------------------------------------------------------------------------
ZPoly & ZPoly::operator=(const ZPolePair & rhs)
{
	SetLength(3);
   prA[0] = rhs[0];
   prA[1] = rhs[1];
   prA[2] = rhs[2];
   
	this->VarChar = rhs.VarChar;

   return *this;
}

//-----------------------------------------------------------------------------
ZPoly & ZPoly::operator=(const ZString & rhs)
{
	ZString	   token;
   ZString	   str;
   ZArrDouble  inputArr;

   switch (prIOMode)
   {
   	case omPoly:
		case omPolyFactored:
operator_eq_poly_mode:
			throw ZException("ZPoly::operator=(const ZString):Not supported yet in omPoly modes",0);

      case omMatlabCommas:
      case omMatlabSpaces:
      case omReverseMatlabCommas:
      case omReverseMatlabSpaces:
      	str = rhs;
      	str.InitToken();

         // We need to start with an empty array
         this->SetLength(0);

         // Then we append each coefficient we find
         while ((token = str.NextToken("[, ]")) != "")
         	Append(token.ToDouble());

         // We can't leave the array empty
         if (prSize == 0)
         {
            this->SetLength(1);
            prA[0] = 1.0;
         }

         if ((prIOMode == omMatlabCommas) || (prIOMode == omMatlabSpaces))
         	DoReverseOrder();

      	break;
      default:
         prIOMode = omPoly;
         goto operator_eq_poly_mode;
	};

   if (prSize == 0)
      DoReset();

   return *this;
}

//-------------------------
//	TYPECASTING OPERATORS
//-------------------------
//-----------------------------------------------------------------------------
ZPoly::operator ZString()
{
	int               i;
   int               RootsAtZero;
   ZArrDouble        RealRoots;
   ZArr<ZPolePair>   ComplexRoots;
   ZPoly             inPoly;
   ZString           str;

   inPoly = (*this);

   inPoly.DoNormalise(prNormMode);

   DoPack();

   if (prSize == 0)
   	return ZString("");

	switch (inPoly.prIOMode)
	{
		case ZPoly::omReverseMatlabSpaces:
		case ZPoly::omReverseMatlabCommas:
			//-----------------------------------------------------------------------
			//	reverse MATLAB form
			//	eg. [9,1,5,10,12] (commas) or [ 9 1 5 10 12 ] (spaces)
			//	9 is the lowest order term

			if (inPoly.prSize == 0)
			{
				str += "[]";
				return str;
			}

			if (inPoly.prIOMode == ZPoly::omMatlabCommas)
				str += "[";
			else
				str += "[ ";
			str += inPoly.prA[0];
			for ( i = 1; i < inPoly.prSize; i++)
			{
				if (inPoly.prIOMode == ZPoly::omMatlabCommas)
					str += ",";
				else
					str += " ";
				str += inPoly.prA[i];
			} // FOR
			if (inPoly.prIOMode == ZPoly::omMatlabCommas)
				str += "]";
			else
				str += " ]";

			return str;

		case ZPoly::omMatlabSpaces:
		case ZPoly::omMatlabCommas:
			//-----------------------------------------------------------------------
			//	MATLAB form
			//	eg. [12,10,5,1,9] (commas) or [12 10 5 1 9] (spaces)
			//	9 is the lowest order term

			if (inPoly.prSize == 0)
			{
				str += "[]";
				return str;
			}

			if (inPoly.prIOMode == ZPoly::omReverseMatlabCommas)
				str += "[";
			else
				str += "[ ";
			str += inPoly.prA[inPoly.prSize-1];
			for ( i = inPoly.prSize-2; i >= 0; i--)
			{
				if (inPoly.prIOMode == ZPoly::omReverseMatlabCommas)
					str += ",";
				else
					str += " ";
				str += inPoly.prA[i];
			} // FOR
			if (inPoly.prIOMode == ZPoly::omReverseMatlabCommas)
				str += "]";
			else
				str += " ]";

			return str;

		case ZPoly::omPolyFactored:
			//-----------------------------------------------------------------------
			//	POLYNOMIAL form
			//	eg. (2s+1)(9s^2 + 3s + 1)s
			if (inPoly.prSize == 0)
			{
				str += "0";
				return str;
			}
         else if (inPoly.prSize == 1)
         {
         	str += inPoly[0];
            return str;
         }

			inPoly.Factorise(RootsAtZero, RealRoots, ComplexRoots);

         //	Real Roots
         for (i = 0; i < RealRoots.Count(); i++)
         {
            switch (prNormMode)
            {
               case nmHighest:
                  str += ZString("(s ");
                  if (RealRoots[i] > 0)
                     str += "-";
                  else
                     str += "+";

                  if (CloseEnough(fabs(RealRoots[i]), 1.0))
                     str += " 1";
                  else
                     str += fabs(RealRoots[i]);

                  str += ZString(")");
                  break;

               case nmLowest:
               default:
                  str += ZString("(");
                  if (CloseEnough(fabs(RealRoots[i]), 1.0))
                  {
                     if (RealRoots[i] > 0)
                        str += "-";
                  }
                  else
                     str += 1/fabs(RealRoots[i]);
                  str += ZString(inPoly.VarChar);
                  str += ZString(" + 1)");
                  break;
            }  // SWITCH
         }

         // Resonant Roots
         for (i = 0; i < ComplexRoots.Count(); i++)
         {
            ZPolePair   polePair = ComplexRoots[i];

            polePair.SetIOMode(ZPoly::omPolyFactored);
            polePair.SetNormMode(inPoly.NormMode());
            polePair.VarChar = VarChar;

            str += polePair.String();
         }

         // Roots at zero
         if (RootsAtZero > 0)
         {
         	if (RootsAtZero == 1)
            	str += ZString(inPoly.VarChar);
            else
            {
            	str += ZString(inPoly.VarChar);
            	str += "^";
               str += RootsAtZero;
            }
         }

			return str;

		case ZPoly::omPoly:
		default:
			//-----------------------------------------------------------------------
			//	POLYNOMIAL form
			//	eg. 12s^4 + 10s^3 + 5s^2 + s
			if (inPoly.prSize == 0)
			{
				str += "0";
				return str;
			}

			//Highest order element
         i = inPoly.Order();
			if (i > 0)
			{
				if (inPoly.prA[i] == 1.0)
				{
				}
				else if (inPoly.prA[i] == -1.0)
				{
					str += "-";
				}
				else
				{
					str += inPoly.prA[i];
				}
				if (i > 0)
					str += inPoly.VarChar;
				if (i > 1)
				{
					str += "^";
					str += i;
				}
			}
			else
			{
				str += inPoly.prA[0];
				return str;
			}
			i--;

			// Other elements
			while (i >= 0)
			{
				if (inPoly.prA[i] != 0.0)
				{
					if (inPoly.prA[i] < 0.0)
						str += " - ";
					else
						str += " + ";
					if ((fabs(inPoly.prA[i]) != 1.0) || (i == 0))
					{
						str += fabs(inPoly.prA[i]);
					}
					if (i > 0)
						str += inPoly.VarChar;
					if (i > 1)
					{
						str += "^";
						str += i;
					}
				}	// IF
				i--;
			}	//	WHILE

			return str;
	}	// SWITCH
}


//--------------------
//	UTILITY MEMBER FUNCTIONS
//--------------------
//-----------------------------------------------------------------------------
int ZPoly::Order(void) const
{
	int	i;

	i = prSize-1;
	while ((prA[i] == 0.0) && (i > 0))
		i--;

	return i;
}

//-----------------------------------------------------------------------------
ZArrComplex	ZPoly::Roots(void) const
{
	ZArrComplex	roots;

	int	i;
	ZMat	Companion, EigVectors;
	ZPoly	reduced_poly; // Do not modify original
	int	zeros;

   // % ROOTS finds the eigenvalues of the associated companion matrix.

   if (prSize < 2)
   {
		return 0;
   }

   // Assume that leading zeros have been removed already.

   // % Strip trailing zeros, but remember them as roots at zero.
	zeros = 0;
   reduced_poly = *this;
	while (prA[zeros] == 0.0)
   {
		zeros++;
      reduced_poly.Delete(0);
   }

   // Check to make sure we aren't trying to find the roots of a constant
   if (reduced_poly.Order() != 0)
   {

      //  Polynomial roots via a companion matrix
	   Companion.SetCompanion(reduced_poly);

	   //	Calculate the eigen values and vectors
	   Companion.Eigen(roots, EigVectors);
   }

	ZComplex	zeroC;
	for (i = 0; i < zeros; i++)
		roots.Append(zeroC);

	return roots;
}

//-----------------------------------------------------------------------------
int ZPoly::Factorise(int & RootsAtZero, ZArrDouble & RealRoots, ZArr<ZPolePair> & ComplexRoots) const
{
	ZArr<ZComplex>	roots;
	ZPolePair		conjRoots;
	int				i;


	RootsAtZero = 0;
	RealRoots.DoReset();
	ComplexRoots.DoReset();

	roots = Roots();

	i = 0;
	while (i < roots.Length())
	{
		if (roots[i].IsNull())
		{
			RootsAtZero++;
		}
		else if (roots[i].IsComplex())
		{
			conjRoots.SetPoles(1/roots[i], 1/roots[i+1]);
			ComplexRoots.Append(conjRoots);
			i++;
		}
		else
		{
			RealRoots.Append(roots[i].Re());
		}
		i++;
	}

	return roots.Length();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//		Reset, DoReset
//			Resets to a length 1 transfer function (zeroth order) with constant
//       coeff == 1.
//
//		ZPoly &	DoReset(void);		// Reset's this
//
//		ZPoly 	Reset(void) const;// returns a Reset'd copy of this. Pretty
//                               // useless but we keep for consistency.
//-----------------------------------------------------------------------------
ZPoly & ZPoly::DoReset(void)
{
   if (prA)
   {
#ifdef DEBUG_MEMORY
      DebugMemDestroy((pvoid)prA, "ZPoly::DoReset()");
#endif
      delete [] prA;
   }

   prA = new double[1];
#ifdef DEBUG_MEMORY
   DebugMemAllocate((pvoid)prA, "ZPoly::DoReset: new double[1]");
#endif
   prA[0] = 1.0;

   prSize = 1;

	return *this;
}
//-----------------------------------------------------------------------------
ZPoly ZPoly::Reset(void) const
{
   return ZPoly(1);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//		Zero, DoZero
//			Zeros all coefficients. Length unchanged
//
//		ZPoly &	DoZero(void);		// Zero's this
//
//		ZPoly 	Zero(void) const;	// returns a zero'd copy of this
//-----------------------------------------------------------------------------
ZPoly & ZPoly::DoZero(void)
{
	for (int i = 0; i < prSize; i++)
		prA[i] = 0.0;

	return *this;
}
//-----------------------------------------------------------------------------
ZPoly ZPoly::Zero(void) const
{
   ZPoly	tempPoly;

   tempPoly.SetLength(prSize);

   return tempPoly;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//		Normalise, DoNormalise
//			Normalises the polynomial either so that the high order term is 1
//			(if NormMode == nmHighest), the low order term is 1 (NormMode ==
//       nmLowest) or the constant term is one (NormMode == nmA0). If the last
//       option can't be satisfied because the constant coefficient is 0, then
//       an exception will be thrown. This is the difference between nmLowest
//       and nmA0. nmLowest will find the lowest non-zero term.
//
//			ZPoly &	DoNormalise(TNormaliseMode NormMode=nmHighest) throw(ZException); 	// Normalises this
//
//			ZPoly 	Normalise(TNormaliseMode NormMode=nmHighest) const throw(ZException);// returns a Normalised copy of this
//-----------------------------------------------------------------------------
ZPoly & ZPoly::DoNormalise(TNormaliseMode NormMode) throw(ZException)
{
	int	order, i;
   int   lowestNonZeroIndex;

	if (prSize == 0)
	{
		throw ZException("ZPoly::Normalise: Cannot normalise a Zero size Polynomial",0);
	}	// IF

   switch (NormMode)
   {
      case nmHighest:
         order = this->Order();
         if ((order == 0) && (prA[0] == 0.0))
         {
            throw ZException("ZPoly::Normalise: Cannot normalise Null polynomial",0);
         }	// IF

         //------------------
         // Normalise.
         //------------------
         for (i = order-1; i >= 0; i--)
            prA[i] = prA[i]/prA[order];
         prA[order] = 1.0;
         break;

      case nmLowest:
         lowestNonZeroIndex = 0;

         //------------------
         // Get the lowest non-zero coefficient. Throw an exception if all zero.
         //------------------
         while (prA[lowestNonZeroIndex] == 0)
         {
            if (lowestNonZeroIndex == this->Order())
            {
               throw ZException("ZPoly::Normalise: Cannot normalise Null polynomial",0);
            }	// IF
            lowestNonZeroIndex++;
         }  // WHILE

         //------------------
         // Normalise.
         //------------------
         for (i = prSize-1; i > lowestNonZeroIndex; i--)
            prA[i] = prA[i]/prA[lowestNonZeroIndex];
         prA[lowestNonZeroIndex] = 1.0;
         break;

      case nmNone:
      default:
         break;
	}  // SWITCH

	return *this;
}
//-----------------------------------------------------------------------------
ZPoly ZPoly::Normalise(TNormaliseMode NormMode) const throw(ZException)
{
   ZPoly	tempPoly;

   tempPoly = *this;
   tempPoly.DoNormalise(NormMode);

   return tempPoly;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//		Pack, DoPack
//			Gets rid of high order zero terms
//
//		ZPoly &	DoPack(void);		// packs this
//
//		ZPoly 	Pack(void) const;	// returns a packed copy of this
//-----------------------------------------------------------------------------
ZPoly & ZPoly::DoPack(void)
{
   int  i;

   if (prSize < 2)
      return *this;
      
   i = prSize-1;
   while ((prA[i] == 0.0) && (i > 0))
      i--;

   SetLength(i+1);   // Set to highest non-zero coefficient index + 1.

   return *this;
}

//-----------------------------------------------------------------------------
ZPoly ZPoly::Pack(void) const
{
   ZPoly	tempPoly;

   tempPoly = *this;
   tempPoly.DoPack();

   return tempPoly;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//		ReverseOrder, DoReverseOrder
//			Reverses the order of the coefficients. Note that no pack is performed
//			before hand.
//
//		ZPoly &	DoReverseOrder(void);		// reverses this
//
//		ZPoly 	ReverseOrder(void) const;	// returns a reversed copy of this
//-----------------------------------------------------------------------------
ZPoly &  ZPoly::DoReverseOrder(void)
{
	double	*pArr;

   if (prSize > 0)
   {
      pArr = new double[prSize];
#ifdef DEBUG_MEMORY
		DebugMemAllocate((pvoid)pArr, "ZPoly::DoReverseOrder()");
#endif DEBUG_MEMORY
      for (int i = 0; i < prSize; i++)
      	pArr[i] = prA[prSize - i - 1];

#ifdef DEBUG_MEMORY
		DebugMemDestroy((pvoid)prA, "ZPoly::DoReverseOrder()");
#endif DEBUG_MEMORY
      delete [] prA;
      prA = pArr;
   }

   return *this;
}
//-----------------------------------------------------------------------------
ZPoly ZPoly::ReverseOrder(void) const
{
   ZPoly	tempPoly;

   tempPoly = *this;
   tempPoly.DoReverseOrder();

   return tempPoly;
}


//--------------------
//	STREAMING FRIEND FUNCTIONS
//--------------------
//-----------------------------------------------------------------------------
// Override the ZArr class << operator
// friend class
std::ostream & operator<<(std::ostream & os, ZPoly & rhs)
{
	ZString	str;

	str = ZString(rhs);
	os << str.c_str();
	return os;
}

//-----------------------------------------------------------------------------
//	Generates a string representing the polynomial. The string format
// depends on the prIOMode selected.
//	The comments are based on the following polynomial
//	(2s+1)(9s^2 + 3s + 1)s = 12s^4 + 10s^3 + 5s^2 + s
ZString & operator<<(ZString & str, ZPoly & rhs)
{
   str += rhs.operator ZString();

   return str;
}
