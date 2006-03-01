#ifndef Z_POLY_HPP
#define Z_POLY_HPP
//-----------------------------------------------------------------------------
#include <iostream>

#include "ZArr.hpp"
#include "ZArrCode.hpp"
#include "ZArrDouble.h"
//-----------------------------------------------------------------------------
#ifndef Z_POLE_PAIR_H
class ZPolePair;
#endif
//-----------------------------------------------------------------------------
class ZPoly : public ZArrDouble
{
	public:
		enum TIOMode
		{
			omPoly,
			omPolyFactored,
			omMatlabCommas,
			omMatlabSpaces,
			omReverseMatlabCommas,
			omReverseMatlabSpaces
		};
      enum TNormaliseMode
      {
         nmHighest,
         nmLowest,
         nmNone
      };
   private:
		TIOMode	      prIOMode;
      TNormaliseMode prNormMode;
	public:
      //-------------------------
      //	PUBLIC MEMBER VARIABLES
      //-------------------------
      char	VarChar;

      //-------------------------
      //	CONSTRUCTORS
      //-------------------------
		ZPoly(const ZArrDouble & Arr, char VarCharValue='s');
		ZPoly(int Size=1);
		ZPoly(const ZPoly & Poly);
		ZPoly(int arraySize, double InitValue);

      //-------------------------
      //	ASSIGNMENT OPERATORS
      //-------------------------
		ZPoly & operator=(const ZArrDouble & rhs);
		ZPoly & operator=(const ZPoly & rhs);
      ZPoly & operator=(const ZPolePair & rhs);
      ZPoly & operator=(const ZString & rhs);

      //-------------------------
      //	TYPECASTING OPERATORS
      //-------------------------
      operator ZString();

      //-------------------------
      //	UTILITY MEMBER FUNCTIONS
      //-------------------------

      //---------
      //	IOMode:	The string input and output interface mode
      TIOMode	IOMode(void) const;
      void SetIOMode(TIOMode Mode) throw(ZException);

      //---------
      //	NormMode:	The string input and output normalise mode
      TNormaliseMode	NormMode(void) const;
      void SetNormMode(TNormaliseMode Mode) throw(ZException);


      //---------
      //	Order: Returns the Order of the polynomial. Does not pack
		int Order(void) const;

      //---------
      // Roots:	Returns the complex roots of the polynomial
		ZArr<ZComplex>	Roots(void) const;

      //---------
      // Factorise:	Factors out the polynomial into its various root
      //       components.
      //          - Roots at zero - these are the number zero roots (s = 0)
      //          - Real roots - an array of real roots (s = real number)
      //          - Complex roots - the number of complex pair roots (s = a +- jb)
		int		Factorise(	int & RootsAtZero,
      							ZArrDouble & RealRoots,
                           ZArr<ZPolePair> & ComplexRoots) const;

      //---------
      //	Zero: Zero's all terms. Length() remains unchanged
		ZPoly &	DoZero(void);		//	Zeros this
		ZPoly 	Zero(void) const;	//	Returns a zero'd copy of this

      //---------
      //	Reset: Reset's the polynomial to a zero'th order polynomial with a constant coeff of 1
		ZPoly &	DoReset(void);		   //	Reset this
		ZPoly 	Reset(void) const;	//	Returns a reset poly. Kind of useless

      //---------
      //	Normalise: Normalises the polynomial either so that the high order
      //		term is 1 (if NormMode == false) or the low order term is 1.
      //    DoNormalise works on this. Normalise returns a normalised copy of this
      ZPoly &	DoNormalise(TNormaliseMode NormMode=nmHighest) throw(ZException); 	// Normalises this
      ZPoly 	Normalise(TNormaliseMode NormMode=nmHighest) const throw(ZException);// returns a Normalised copy of this

      //---------
      //	Pack: Gets rid of high order zero terms
      ZPoly &	DoPack(void);     // packs this
      ZPoly 	Pack(void) const;	// returns a packed copy of this

      //---------
      //	ReverseOrder:	Self explanatory
      ZPoly   	ReverseOrder(void) const;
      ZPoly &  DoReverseOrder(void);

      //---------
      // Overridden friend streaming
      friend std::ostream & operator<<(std::ostream & os, ZPoly & rhs);
		friend ZString & operator<<(ZString & str, ZPoly & rhs);
};

//-----------------------------------------------------------------------------
#endif Z_POLY_HPP
