#include <stdafx.h>
#ifdef	__BORLANDC__
//-----------------------------------------------------------------------------
#include <iostream>

#include <vcl.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#pragma hdrstop
//-----------------------------------------------------------------------------
#else
//-----------------------------------------------------------------------------
#include <iostream>

#include <stdlib.h>
#include <string.h>
#include <math.h>
//-----------------------------------------------------------------------------
#endif __BORLANDC__

//-----------------------------------------------------------------------------
#include "ZException.hpp"
#include "ZComplex.h"

const double PI = 3.141592654;

//-----------------------------------------------------------------------------
ZComplex::ZComplex(double Re, double Im)
{
	prRe = Re;
	prIm = Im;
}

//-----------------------------------------------------------------------------
ZComplex::ZComplex(const ZComplex & No)
{
	prRe = No.prRe;
	prIm = No.prIm;
}

//-----------------------------------------------------------------------------
ZComplex::~ZComplex()
{
}

//-----------------------------------------------------------------------------
const double ZComplex::Re(void) const
{
	return prRe;
}
//-----------------------------------------------------------------------------
void ZComplex::Re(double Value)
{
	prRe = Value;
}

//-----------------------------------------------------------------------------
const double ZComplex::Im(void) const
{
	return prIm;
}

//-----------------------------------------------------------------------------
void ZComplex::Im(double Value)
{
	prIm = Value;
}

//-----------------------------------------------------------------------------
const double ZComplex::Mag(void) const
{
	return sqrt(prRe*prRe + prIm*prIm);
}
//-----------------------------------------------------------------------------
void ZComplex::Mag(double mag)
{
	double	ratio;
	double	oldMag;

	oldMag = sqrt(prRe*prRe + prIm*prIm);
	if (oldMag == 0)
	{
		prRe = mag;
		return;
	}
	ratio = mag/oldMag;

	prRe *= ratio;
	prIm *= ratio;
}

//-----------------------------------------------------------------------------
const double ZComplex::ArgDeg(void) const
{
	if (prRe == 0.0)
	{
		if (prIm < 0)
			return (-90.0);
		else
			return 90.0;
	}

	return (180.0/PI)*atan(prIm/prRe);
}
//-----------------------------------------------------------------------------
void ZComplex::ArgDeg(double deg)
{
	double	argRad;
	double	mag;

	argRad = deg*PI/180.0;
	mag = Mag();

	prRe = mag*cos(argRad);
	prIm = mag*sin(argRad);
}

//-----------------------------------------------------------------------------
const double ZComplex::ArgRad(void) const
{
	if (prRe == 0.0)
	{
		if (prIm < 0)
			return (-PI/2.0);
		else
			return PI/2.0;
	}

	return atan(prIm/prRe);
}
//-----------------------------------------------------------------------------
void ZComplex::ArgRad(double rad)
{
	double	mag;

	mag = Mag();

	prRe = mag*cos(rad);
	prIm = mag*sin(rad);
}

//-----------------------------------------------------------------------------
ZComplex	ZComplex::Conjugate(void) const
{
	ZComplex	tempC;

   tempC = (*this);
   tempC.Im(-tempC.Im());

   return tempC;
}
//-----------------------------------------------------------------------------
bool ZComplex::IsComplex() const
{
	if (prIm != 0.0)
		return true;
	return false;
}

//-----------------------------------------------------------------------------
bool ZComplex::IsNull(void) const
{
	return (prRe == 0.0) && (prIm == 0.0);
}

//-----------------------------------------------------------------------------
bool ZComplex::IsReal() const
{
	return (prIm == 0.0);
}

//-----------------------------------------------------------------------------
bool ZComplex::IsImag() const
{
	return (prIm != 0.0) && (prRe == 0.0);
}


//======================
// Operator overloading
//======================
//-----------------------------------------------------------------------------
bool ZComplex::operator==(const ZComplex &rhs)
{
   return ((prRe == rhs.prRe) && (prIm == rhs.prIm));
}

//-----------------------------------------------------------------------------
ZComplex &ZComplex::operator=(const ZComplex & rhs)
{
	prRe = rhs.prRe;
	prIm = rhs.prIm;

	return *this;
}

//-----------------------------------------------------------------------------
ZComplex &ZComplex::operator=(const double & scalar)
{
	prRe = scalar;
	prIm = 0.0;

	return *this;
}

//-----------------------------------------------------------------------------
ZComplex ZComplex::operator+(const double & rhs) const
{
	ZComplex	tempC;

	tempC = (*this);

	tempC.prRe += rhs;
	return tempC;
}

//-----------------------------------------------------------------------------
ZComplex ZComplex::operator-(const double & rhs) const
{
	ZComplex	tempC;
	
	tempC = (*this);

	tempC.prRe -= rhs;
	return tempC;
}

//-----------------------------------------------------------------------------
ZComplex	ZComplex::operator+(const ZComplex & rhs) const
{
	ZComplex	resultC;

	resultC.prRe = prRe + rhs.prRe;
	resultC.prIm = prIm + rhs.prIm;

	return resultC;
}
//-----------------------------------------------------------------------------
ZComplex	ZComplex::operator-(const ZComplex & rhs) const
{
	ZComplex	resultC;

	resultC.prRe = prRe - rhs.prRe;
	resultC.prIm = prIm - rhs.prIm;

	return resultC;
}

//-----------------------------------------------------------------------------
ZComplex	ZComplex::operator-(void) const
{
	ZComplex	resultC;

	resultC.prRe = - prRe;
	resultC.prIm = - prIm;

	return resultC;
}

//-----------------------------------------------------------------------------
ZComplex	ZComplex::operator*(const double & rhs) const
{
	ZComplex	resultC;

	resultC.prRe = prRe*rhs;
	resultC.prIm = prIm*rhs;

	return resultC;
}

//-----------------------------------------------------------------------------
ZComplex	ZComplex::operator/(const double & rhs) const throw(ZException)
{
	if (rhs == 0)
	{
		throw ZException("ZComplex <operator/(const double &)>: Divide by Zero",0);
	}

	ZComplex	resultC;

	resultC.prRe = prRe/rhs;
	resultC.prIm = prIm/rhs;
	
	return resultC;
}

//-----------------------------------------------------------------------------
ZComplex	ZComplex::operator*(const ZComplex & rhs) const
{
	ZComplex	resultC;

	resultC.prRe = prRe*rhs.prRe - prIm*rhs.prIm;
	resultC.prIm = prRe*rhs.prIm + prIm*rhs.prRe;

	return resultC;
}

//-----------------------------------------------------------------------------
ZComplex	ZComplex::operator/(const ZComplex & rhs) const throw(ZException)
{
	ZComplex	rhsConj, resultC;


	if (rhs.Mag() == 0)
	{
		throw ZException("ZComplex <operator/(const ZComplex &)>: Divide by Zero",0);
	}
	
	rhsConj.prRe = rhs.prRe;
	rhsConj.prIm = -rhs.prIm;

	resultC = (*this)*rhsConj/(rhs.prRe*rhs.prRe + rhs.prIm*rhs.prIm);

	return resultC;
}
//-----------------------------------------------------------------------------
ZComplex &ZComplex::operator+=(const ZComplex & rhs)
{
	prRe += rhs.prRe;
	prIm += rhs.prIm;

	return *this;
}

//-----------------------------------------------------------------------------
ZComplex &ZComplex::operator-=(const ZComplex & rhs)
{
	prRe -= rhs.prRe;
	prIm -= rhs.prIm;

	return *this;
}

//-----------------------------------------------------------------------------
ZComplex &ZComplex::operator*=(const double & rhs)
{
	prRe *= rhs;
	prIm *= rhs;

	return *this;
}
//-----------------------------------------------------------------------------
ZComplex &ZComplex::operator/=(const double & rhs) throw(ZException)
{
	if (rhs == 0)
	{
		throw ZException("ZComplex <operator/=(const double &)>: Divide by Zero",0);
	}

	prRe /= rhs;
	prIm /= rhs;

	return *this;
}
//-----------------------------------------------------------------------------
ZComplex &ZComplex::operator*=(const ZComplex & rhs)
{

	prRe = prRe*rhs.prRe - prIm*rhs.prIm;
	prIm = prRe*rhs.prIm + prIm*rhs.prRe;

	return *this;
}
//-----------------------------------------------------------------------------
ZComplex &ZComplex::operator/=(const ZComplex & rhs) throw(ZException)
{
	if (rhs.Mag() == 0)
	{
		throw ZException("ZComplex <operator/(const ZComplex &)>: Divide by Zero",0);
	}

	ZComplex	rhsConj, tempC;
	
	rhsConj.prRe = rhs.prRe;
	rhsConj.prIm = -rhs.prIm;

	(*this) = (*this)*rhsConj/(rhs.prRe*rhs.prRe + rhs.prIm*rhs.prIm);

	return *this;
}

//-----------------------------------------------------------------------------
// Conversion Functions
//-----------------------------------------------------------------------------
//ZComplex::operator double() const
//{
//	return sqrt(prRe*prRe + prIm*prIm);
//}


//-----------------------------------------------------------------------------
ZComplex::operator ZString() const
{
   ZString  str;

   if (prRe != 0.0)
      str = ZString(prRe);
   else
      str = "0";

   if (prIm != 0.0)
   {
      if (prIm > 0.0)
      {
         str += "+j"; 
         if (prIm != 1.0)
            str += ZString(prIm);
      }
      else
      {
         str += "-j";
         if (prIm != -1.0)
            str += ZString(prIm*(-1.0));
      } // ELSE
   }

   return str;
}

//-----------------------------------------------------------------------------
ZComplex &ZComplex::operator<<(char * rhs) throw(ZException)
{
	int	i;                                                   
	ZString	token;
	double	sign = 1.0;
	bool		imaj = false;
	double	re = 0.0;
	double	im = 0.0;

	i = 0;
	while (i < (int)strlen(rhs))
	{
		switch (rhs[i])
		{
			case '-':
				sign = -1.0;
				i++;
				break;
			case '+':
				sign = 1.0;
				i++;
				break;
			case 'i':
			case 'j':
				imaj = true;
				im = sign;
				i++;
				break;
			case ' ':
				i++;
				break;
			default:
				token = "";
				while (	((rhs[i] >= '0') && (rhs[i] <= '9')) ||
							(rhs[i] == '.')								)
				{
					token += rhs[i];
					i++;
				}	// while
				if ((rhs[i] == 'i') || (rhs[i] == 'j'))
				{
					i++;
					imaj = true;
				}

				if (token.Length() > 0)
				{
					if (imaj)
						im = sign*token.ToDouble();
					else
						re = sign*token.ToDouble();

					sign = 1.0;
					imaj = false;
				}
				break;
		}	// SWITCH
	}	// WHILE

	prRe = re;
	prIm = im;

	return *this;
}

//-----------------------------------------------------------------------------
// Friends
//-----------------------------------------------------------------------------
std::ostream & operator<<(std::ostream & os, const ZComplex & rhs)
{
	if (rhs.prRe != 0.0)
		os << rhs.prRe;
	else
		os << "0";

	if (rhs.prIm != 0.0)
	{
		if (rhs.prIm > 0.0)
		{
			os << "+j"; 
			if (rhs.prIm != 1.0)
				os << rhs.prIm;
		}
		else
		{
			os << "-j";
			if (rhs.prIm != -1.0)
				os << rhs.prIm*(-1.0);
		}	// ELSE
	}  // IF

	return os;
}


//-----------------------------------------------------------------------------
ZComplex operator+(const double lhs, const ZComplex & rhs)
{
	return rhs + lhs;
}

//-----------------------------------------------------------------------------
ZComplex operator-(const double lhs, const ZComplex & rhs)
{
	return (-rhs) + lhs;
}
//-----------------------------------------------------------------------------
ZComplex operator*(const double lhs, const ZComplex & rhs)
{
	return rhs*lhs;
}
//-----------------------------------------------------------------------------
ZComplex operator/(const double lhs, const ZComplex & rhs)
{
	return ZComplex(lhs)/rhs;
}
//-----------------------------------------------------------------------------
ZComplex ZComplex::Sqrt(const double & number)
{
	ZComplex tempC;

	if (number < 0)
		tempC.Im(sqrt(-number));
	else
		tempC.Re(sqrt(number));

	return tempC;
}