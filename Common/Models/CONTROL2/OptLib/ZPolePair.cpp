#include <stdafx.h>
#include <iostream>
//using namespace std;

#include <math.h>

#include "ZString.h"
#include "ZPolePair.h"
#include "ZPoly.h"
#include "ZException.hpp"
//---------------------------------------------------------------------------
int sign(double value)
{
	if (value > 0)
   	return 1;
   else if (value < 0)
   	return -1;
   else
   	return 0;
}
//-----------------------------------------------------------------------------
ZPolePair::ZPolePair()
{
	prA0 = 0.0;
	prA1 = 0.0;
	prA2 = 0.0;
   VarChar = 's';
   prIOMode = ZPoly::omPoly;
   prNormMode = ZPoly::nmLowest;
}

//-----------------------------------------------------------------------------
ZPolePair::ZPolePair(const ZPolePair &PP)
{
	prA0 = PP.prA0;
	prA1 = PP.prA1;
	prA2 = PP.prA2;
   VarChar = 's';
   prIOMode = ZPoly::omPoly;
   prNormMode = ZPoly::nmLowest;
}

//-----------------------------------------------------------------------------
double ZPolePair::A0(void) const
{
	return prA0;
}
//-----------------------------------------------------------------------------
double ZPolePair::A1(void) const
{
	return prA1;
}
//-----------------------------------------------------------------------------
double ZPolePair::A2(void) const
{
	return prA2;
}

//-----------------------------------------------------------------------------
void ZPolePair::SetA0(double value)
{
	prA0 = value;
}
//-----------------------------------------------------------------------------
void ZPolePair::SetA1(double value)
{
	prA1 = value;
}

//-----------------------------------------------------------------------------
void ZPolePair::SetA2(double value)
{
	prA2 = value;
}

//-----------------------------------------------------------------------------
double & ZPolePair::operator[](int index) throw(ZException)
{
	if ((index < 0) || (index > 2))
	{
		throw ZException("ZPolePair::operator[]: Index out of range",0);
	}	// IF

	switch (index)
	{
		case 0:
			return prA0;
		case 1:
			return prA1;
		default:
			return prA2;
	}
}

//-----------------------------------------------------------------------------
ZPolePair ZPolePair::operator*(const double &rhs) const
{
   ZPolePair   polePair(*this);

   polePair.prA0 *= rhs;
   polePair.prA1 *= rhs;
   polePair.prA2 *= rhs;

   return polePair;
}

//-----------------------------------------------------------------------------
ZPolePair ZPolePair::operator/(const double &rhs) const
{
   ZPolePair   polePair(*this);

   polePair.prA0 /= rhs;
   polePair.prA1 /= rhs;
   polePair.prA2 /= rhs;

   return polePair;
}

//-----------------------------------------------------------------------------
const double & ZPolePair::operator[](int index) const throw(ZException)
{
	if ((index < 0) || (index > 2))
	{
		throw ZException("ZPolePair::operator[]: Index out of range",0);
	}	// IF

	switch (index)
	{
		case 0:
			return prA0;
		case 1:
			return prA1;
		default:
			return prA2;
	}
}

//--------------------
// INPUT/OUTPUT FORMAT MODES
//--------------------
//-----------------------------------------------------------------------------
ZPoly::TIOMode	ZPolePair::IOMode(void) const
{
	return prIOMode;
}
//-----------------------------------------------------------------------------
void ZPolePair::SetIOMode(ZPoly::TIOMode mode) throw(ZException)
{
	switch (mode)
   {
      case ZPoly::omPoly:
      case ZPoly::omPolyFactored:
      case ZPoly::omMatlabCommas:
      case ZPoly::omMatlabSpaces:
      case ZPoly::omReverseMatlabCommas:
      case ZPoly::omReverseMatlabSpaces:
      	prIOMode = mode;
         break;
      default:
      	throw(ZException("ZPolePair::SetIOMode: mode is illegal",0));
   }	// SWITCH
}
//-----------------------------------------------------------------------------
ZPoly::TNormaliseMode	ZPolePair::NormMode(void) const
{
	return prNormMode;
}
//-----------------------------------------------------------------------------
void ZPolePair::SetNormMode(ZPoly::TNormaliseMode mode) throw(ZException)
{
	switch (mode)
   {
      case ZPoly::nmHighest:
      case ZPoly::nmLowest:
      case ZPoly::nmNone:
      	prNormMode = mode;
         break;
      default:
      	throw(ZException("ZPolePair::SetNormMode: mode is illegal",0));
   }	// SWITCH
}

//-----------------------------------------------------------------------------
double ZPolePair::Damping(void) const throw(ZException)
{
	if (!SecondOrder())
	{
		throw ZException("ZPolePair::Damping:Pole pair not 2nd order",0);
	}	// IF

	if (Integrators())
	{
		throw ZException("ZPolePair::Damping:Pole pair has root(s) at zero",0);
	}	// IF

  	if (sign(prA2) != sign(prA0))
	{
		throw ZException("ZPolePair::Damping:Polynomial Coeffs (a0 & a2) have different signs->non causal",0);
	}	// IF

   return prA1/sqrt(4*prA2*prA0);
}

//-----------------------------------------------------------------------------
double ZPolePair::TimeConstant(void) const throw(ZException)
{
	if (!SecondOrder())
	{
		throw ZException("ZPolePair::TimeConstant:Pole pair not 2nd order",0);
	}	// IF

	if (Integrators())
	{
		throw ZException("ZPolePair::TimeConstant:Pole pair has root(s) at zero",0);
	}	// IF

  	if (sign(prA2) != sign(prA0))
	{
		throw ZException("ZPolePair::Damping:Polynomial Coeffs (a0 & a2) have different signs->non causal",0);
	}	// IF

   return sqrt(prA2/prA0);
}

//-----------------------------------------------------------------------------
double ZPolePair::FreqRadSec(void) const throw(ZException)
{
	if (!SecondOrder())
	{
		throw ZException("ZPolePair::FreqRadSec:Pole pair not 2nd order",0);
	}	// IF

	if (Integrators())
	{
		throw ZException("ZPolePair::FreqRadSec:Pole pair has root(s) at zero",0);
	}	// IF

  	if (sign(prA2) != sign(prA0))
	{
		throw ZException("ZPolePair::Damping:Polynomial Coeffs (a0 & a2) have different signs->non causal",0);
	}	// IF

   return sqrt(prA0/prA2);
}

//-----------------------------------------------------------------------------
bool ZPolePair::DampFreq(double * Damping, double * FreqRadSec) const
{
	double	tc;

   if (!SecondOrder() || Integrators())
		return false;

  	if (sign(prA2) != sign(prA0))
		return false;

   tc = sqrt(prA2/prA0);
   *Damping = prA1/sqrt(4*prA2*prA0);
	*FreqRadSec = 1/tc;

	return true;
}
//-----------------------------------------------------------------------------
bool ZPolePair::DampTimeConst(double * Damping, double * TimeConstantSec) const
{
   if (!SecondOrder() || Integrators())
		return false;

  	if (sign(prA2) != sign(prA0))
		return false;

   *TimeConstantSec = sqrt(prA2/prA0);
   *Damping = prA1/sqrt(4*prA2*prA0);

	return true;
}

//-----------------------------------------------------------------------------
bool ZPolePair::SetDampFreq(double Damping, double FreqRadSec)
{
	if (FreqRadSec < 0)
   	return false;

	prA2 = 1.0;
   prA1 = 2.0*FreqRadSec*Damping;
   prA0 = FreqRadSec*FreqRadSec;

   return true;
}
//-----------------------------------------------------------------------------
bool ZPolePair::SetDampTimeConst(double Damping, double TimeConstantSec)
{
	if (TimeConstantSec <= 0)
   	return false;

	double freq = 1/TimeConstantSec;

	prA2 = 1.0;
   prA1 = 2.0*freq*Damping;
   prA0 = freq*freq;

   return true;
}

//-----------------------------------------------------------------------------
int ZPolePair::Poles(ZComplex & P1Secs, ZComplex & P2Secs) const
{
   ZComplex pRadSec;

	if (prA2 == 0)
	{
		if (prA1 == 0)
			return 0;
		else if (prA0 == 0)
		{
			P1Secs = 0;
			return 1;	// one pole at zero
		}

		P1Secs = -prA1/prA0;	// one real non-zero pole
		return 1;
	}	// IF

	// At this point we know that prA2 != 0
	if (prA0 == 0)
	{
		// then atleast one integrator
		if (prA1 == 0)
		{
			// then 2 integrators
			P1Secs = 0;
			P2Secs = 0;
			return 2;
		}
		else
		{	// one integrator and 1 real pole
			P1Secs = 0;
			P2Secs = -prA2/prA1;
			return 2;
		}
	}

   //-------------
	// At this point we know that both A0 and A2 are non-zero. Hence
	// we use the traditional means for finding the roots of a 2nd
	// order function.
   //-------------
	pRadSec = (-prA1 + ZComplex::Sqrt(prA1*prA1 - 4.0*prA0*prA2))/(2*prA2);
   P1Secs = 1/pRadSec;
	if (P1Secs.IsComplex())
	{
		P2Secs = P1Secs;
		P2Secs.Im(-P2Secs.Im());
	}
	else
   {
		pRadSec = (-prA1 - ZComplex::Sqrt(prA1*prA1 - 4.0*prA0*prA2))/(2*prA2);
      P2Secs = 1/pRadSec;
   }

	return 2;
}

//-----------------------------------------------------------------------------
void ZPolePair::SetPoles(const ZComplex &P1Secs, const ZComplex &P2Secs)
{
   ZComplex p2Secs = P2Secs;

	if (P1Secs.IsComplex() || P2Secs.IsComplex())
	{	// then we'll ignore P2 and make it the conjugate of P1
		p2Secs = P1Secs.Conjugate();
	}	// IF
	prA0 = 1.0;
	prA1 = -(P1Secs + p2Secs).Mag();
	prA2 = (P1Secs*p2Secs).Mag();
}

//-----------------------------------------------------------------------------
void ZPolePair::SetPoles(double P1Secs, double P2Secs)
{
	prA0 = 1.0;
	prA1 = -(P1Secs + P2Secs);
	prA2 = P1Secs*P2Secs;
}

//-----------------------------------------------------------------------------
int	ZPolePair::Integrators(void) const
{
	if (prA0 != 0)
		return 0;

	if (prA1 != 0)
		return 1;

	if (prA2 != 0)
		return 2;

	return 0;
}
//-----------------------------------------------------------------------------
bool	ZPolePair::SecondOrder(void) const
{
	return (prA2 != 0.0);
}

//-----------------------------------------------------------------------------
bool	ZPolePair::Real(void) const
{
   return (prA1*prA1 >= 4.0*prA0*prA2);
}

//-----------------------------------------------------------------------------
bool	ZPolePair::Stable(void) const
{
   if (prA2 == 0)
	{
		if (sign(prA1) == -sign(prA0))
			return false;
		return true;
	}
	else if (prA1/(2*prA2) < 0)
		return false;

	return true;
}

//-----------------------------------------------------------------------------
bool ZPolePair::Null(void) const
{
	return ((prA0 == 0) && (prA1 == 0) && (prA2 == 0));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//		Normalise, DoNormalise
//			Normalises the polynomial either so that the high order term (2) is 1
//			(if A0_is_1 == false) or the low order term is 1.	If parameter
//			A0_is_1 is true, it tries to set the zeroth order coefficient to 1.0.
//			Hence, for this case, the zeroth order coefficient cannot be zero.
//
//			ZPolePair &	DoNormalise(bool A0_is_1=false) throw(ZException); 	// Normalises this
//
//			ZPolePair 	Normalise(bool A0_is_1=false) const throw(ZException);// returns a Normalised copy of this
//-----------------------------------------------------------------------------
ZPolePair &	ZPolePair::DoNormalise(bool A0_is_1) throw(ZException)
{
	if (A0_is_1)
   {
      if (prA0 == 0.0)
      	throw(ZException(	"ZPolePair::DoNormalise: zero order coeff == 0.0",
         						Z_POLE_PAIR_NORMALISE_FAIL_A0_EQ_0));

      prA1 /= prA0;
      prA2 /= prA0;
      prA0 = 1.0;
   }
   else
   {
      if (prA2 == 0.0)
      	throw(ZException(	"ZPolePair::DoNormalise: 2nd order coeff == 0.0",
         						Z_POLE_PAIR_NORMALISE_FAIL_A2_EQ_0));

      prA1 /= prA2;
      prA0 /= prA2;
      prA2 = 1.0;
   }

   return *this;
}
//-----------------------------------------------------------------------------
ZPolePair ZPolePair::Normalise(bool A0_is_1) const throw(ZException)
{
   ZPolePair	tempPoly;

   tempPoly = *this;
   try
   {
   	tempPoly.DoNormalise(A0_is_1);
   }
   catch(ZException & exception)
   {
   	switch(exception.ErrorCode)
      {
      	case Z_POLE_PAIR_NORMALISE_FAIL_A0_EQ_0:
         	throw ZException(	"ZPolePair::Normalise: zero order coeff == 0.0",
         							Z_POLE_PAIR_NORMALISE_FAIL_A0_EQ_0);
         default:
         	throw ZException(	"ZPolePair::Normalise: 2nd order coeff == 0.0",
         							Z_POLE_PAIR_NORMALISE_FAIL_A2_EQ_0);
      }	// SWITCH
   }	// CATCH

   return tempPoly;
}

//-----------------------------------------------------------------------------
ZString ZPolePair::String(void) throw(ZException)
{
   ZString	str;
   double   a2;
   double   a1;
   double   a0;

   switch (prIOMode)
   {
      case ZPoly::omPolyFactored:
      case ZPoly::omPoly:
         // First we get the normalised coefficients
         switch (prNormMode)
         {
            case ZPoly::nmHighest:
               if (prA2 != 0)
               {
                  a2 = 1;
                  a1 = prA1/prA2;
                  a0 = prA0/prA2;
               }
               else if (prA1 != 0)
               {
                  a2 = 0;
                  a1 = 1;
                  a0 = prA0/prA1;
               }
               else if (prA0 != 0)
               {
                  a2 = a1 = 0;
                  a0 = 1;
               }
               else
               {
                  a2 = a1 = a0 = 0;
               }
               break;
            case ZPoly::nmNone:
               a2 = prA2;
               a1 = prA1;
               a0 = prA0;
               break;
            case ZPoly::nmLowest:
            default:
               if (prA0 != 0)
               {
                  a0 = 1;
                  a1 = prA1/prA0;
                  a2 = prA2/prA0;
               }
               else if (prA1 != 0)
               {
                  a0 = 0;
                  a1 = 1;
                  a2 = prA2/prA1;
               }
               else if (prA2 != 0)
               {
                  a0 = a1 = 0;
                  a2 = 1;
               }
               else
               {
                  a2 = a1 = a0 = 0;
               }
               break;
         }  // SWITCH (prNormMode)

         // Then we construct the string
         if (a2 != 0)
         {
            if (fabs(a2) != 1)
               str += a2;
            else if (a2 < 0)
               str += "-";
            str += "s^2";
            if ((a1 != 0) || (a0 != 0))
               str += " ";
         }

         if (a1 != 0)
         {
            if (a1 > 0)
               str += "+";
            else
               str += "-";

            if (str.Length() > 1)
               str += " ";

            if (fabs(a1) != 1)
               str += fabs(a1);
            str += "s";

            if (a0 != 0)
               str += " ";
         }

         if (a0 != 0)
         {
            if (a0 > 0)
               str += "+";
            else
               str += "-";

            if (str.Length() > 1)
               str += " ";

            str += fabs(a0);
         }

         if (prIOMode == ZPoly::omPolyFactored)
         {
            str = ZString("(") + str + ZString(")");
         }

         return str;

      default:
         ZPoly poly;
         poly = *this;

         poly.SetIOMode(ZPoly::TIOMode(prIOMode));
         poly.SetNormMode(ZPoly::TNormaliseMode(prNormMode));

         str = ZString(poly);
         return str;
   }  // SWITCH

}

//---------------------------------------------------------------------------
//--------------------
//	STREAMING FRIEND FUNCTIONS
//--------------------
//-----------------------------------------------------------------------------
// Override the ZArr class << operator
// friend class
std::ostream & operator<<(std::ostream & os, ZPolePair & rhs) throw(ZException)
{
	ZString	str;

   str = rhs.String();

	os << str.c_str();
	return os;
}

//-----------------------------------------------------------------------------
//	Generates a string representing the polynomial. The string format
// depends on the prIOMode selected.
//	The comments are based on the following polynomial
//	(2s+1)(9s^2 + 3s + 1)s = 12s^4 + 10s^3 + 5s^2 + s
ZString & operator<<(ZString & str, ZPolePair & rhs) throw(ZException)
{
   str = rhs.String();

	return str;
}
