#ifndef Z_POLE_PAIR_H
#define	Z_POLE_PAIR_H

//-----------------------------------------------------------------------------
// This class prototype is required in order to allow the declaration
// of the ZPoly class in the ZPoly.h header file.
class ZPolePair;
//-----------------------------------------------------------------------------
#include <iostream>

#include "ZComplex.h"
#include "ZPoly.h"
//-----------------------------------------------------------------------------
class ZPolePair
{
	public:
	private:
		double	               prA0;
      double                  prA1;
      double                  prA2;
		ZPoly::TIOMode	         prIOMode;
      ZPoly::TNormaliseMode   prNormMode;
	public:
   	char	VarChar;

		ZPolePair();
		ZPolePair(const ZPolePair &);
		double A0(void) const;
		double A1(void) const;
		double A2(void) const;

		void	SetA0(double value);
		void	SetA1(double value);
		void	SetA2(double value);

		double & operator[](int index) throw(ZException);
		const double & operator[](int index) const throw(ZException);

      ZPolePair   operator*(const double &rhs) const;
      ZPolePair   operator/(const double &rhs) const;

      //	IOMode:	The string input and output interface mode
      ZPoly::TIOMode	IOMode(void) const;
      void SetIOMode(ZPoly::TIOMode mode) throw(ZException);

      //	NormMode:	String output mode display mode
      ZPoly::TNormaliseMode	NormMode(void) const;
      void SetNormMode(ZPoly::TNormaliseMode Mode) throw(ZException);

		double	Damping(void) const throw(ZException);
		double	TimeConstant(void) const throw(ZException);
		double	FreqRadSec(void) const throw(ZException);
		bool	   DampFreq(double * Damping, double * FreqRadSec) const;
		bool	   DampTimeConst(double * Damping, double * TimeConstantSec) const;

		bool	   SetDampFreq(double Damping, double FreqRadSec);
		bool	   SetDampTimeConst(double Damping, double TimeConstantSec);

		int	   Poles(ZComplex & P1Secs, ZComplex & P2Secs) const;
		void	   SetPoles(const ZComplex &P1Secs, const ZComplex &P2Secs);
		void	   SetPoles(double P1Secs, double P2Secs);

		int	   Integrators(void) const;
		bool	   SecondOrder(void) const;
		bool	   Real(void) const;
		bool	   Stable(void) const;
      bool	   Null(void) const;

      //	Normalise: Normalises the polynomial either so that A2 is 1
      //	(if A0_is_1 == false) or A0 is 1
		ZPolePair &	DoNormalise(bool A0_is_1=false) throw(ZException); 	// Normalises this
		ZPolePair 	Normalise(bool A0_is_1=false) const throw(ZException);// returns a Normalised copy of this

		ZString String(void)  throw(ZException);

      			// Overridden friend streaming
      friend std::ostream & operator<<(std::ostream & os, ZPolePair & rhs) throw(ZException);
		friend ZString & operator<<(ZString & str, ZPolePair & rhs) throw(ZException);

};

//-----------------------------------------------------------------------------
//	EXCEPTION CODES
#define	Z_POLE_PAIR_NORMALISE_FAIL_A0_EQ_0	1
#define	Z_POLE_PAIR_NORMALISE_FAIL_A2_EQ_0	2
//-----------------------------------------------------------------------------
#endif Z_POLE_PAIR_H