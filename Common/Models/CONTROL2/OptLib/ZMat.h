#ifndef	Z_MAT_H
#define	Z_MAT_H

#include <iostream>

#include "ZArrDouble.h"
#include "ZComplex.h"
#include "ZPoly.h"
//-----------------------------------------------------------------------------
#ifndef UNAC_MATH
#define	UNAC_INT		int
#define	UNAC_DOUBLE	double
typedef struct
{
	UNAC_INT		row_dim,col_dim;
	UNAC_DOUBLE	*A;
} real_matrix;
#endif UNAC_MATH


//#ifdef OPTIMISATIONLIB_EXPORTS
#define DllImportExport __declspec( dllexport ) 
//#else
//#define DllImportExport __declspec( dllimport )
//#endif

//-----------------------------------------------------------------------------
class DllImportExport ZMat
{
   protected:
      int	prRows, prCols;
      ZArrDouble *prM;

   protected:
      bool     ParseRange( const ZString & scRange, 
                           int & nStartIndex, 
                           int & nEndIndex, 
                           bool bRow,
                           bool bOneIndexing) const;

   public:
      enum enumStreamMode{smOneLine=1,smMultiLine=2};
      enumStreamMode display_mode;

      //--------------------
      // Constructors
      //--------------------
      ZMat(const real_matrix & r_matrix);
      ZMat(const ZMat & rhs);
      ZMat(int rows=1, int cols=1);
      ZMat(int rows, int cols, double InitValue);
//      ZMat(double InitValue);  // Constructor that creates a 1x1 scalar matrix
      ZMat(const ZString &InitValue) throw (ZException);   // Constructor that creates from formated string

      //--------------------
      // Destructor
      //--------------------
      virtual ~ZMat();

      //--------------------
      // Operator Overload Methods
      //--------------------
   	ZArrDouble & operator[](int rowIndex) throw(ZException);
      const ZArrDouble & operator[](int rowIndex) const throw(ZException);

      ZMat &operator=(const ZMat & rhs);
      ZMat &operator=(const int & scalar);
      ZMat &operator=(const real_matrix & rhs);
      ZMat &operator=(const ZString & rhs) throw (ZException);

      ZMat	operator+(const ZMat & rhs) throw (ZException);
      ZMat	operator-(const ZMat & rhs) throw (ZException);
      ZMat	operator*(const double & rhs);
      ZMat	operator*(const ZMat & rhs) const throw (ZException);
      ZMat	operator/(const double & rhs);
      ZMat	operator/(const ZMat & rhs);

      ZMat &operator+=(const ZMat & rhs) throw (ZException);
      ZMat &operator-=(const ZMat & rhs) throw (ZException);
      ZMat &operator*=(const double & rhs);
      ZMat &operator*=(const ZMat & rhs) throw (ZException);
      ZMat &operator/=(const double & rhs);

      //--------------------
      // Type conversion
      //--------------------
//      operator double() const throw (ZException);
      operator real_matrix() const;
      operator ZString() const;
//      operator double() const;

      //--------------------
      // Misc Utilities
      //--------------------
      void     Dim(int & RowCount, int & ColCount) const;
      void     SetDim(int RowCount, int ColCount);
      int      RowCount(void) const;
      int      ColCount(void) const;
      void     ToRealMatrix(real_matrix *pReal_matrix) const;
      bool     Square(void) const;

      ZMat     SubMat(const ZString & scDim, bool bOneIndexing = false) const;
      ZMat     SubMat(int nRowStart, int nRowEnd, int nColStart, int nColEnd) const;

      //--------------------
      // Math Utilities.
      //--------------------
      void Eigen(ZArr<ZComplex> & EigenValues, ZMat & EigenVectors) throw (ZException);
      ZMat & SetCompanion(ZPoly & Polynomial) throw(ZException);
      // "Polynomial" is in ascending order.
      // e.g. Polynomial[0] is the constant coefficient
      static ZMat Identity(int Dim);
      ZMat &   DoIdentity(int Dim);

      ZMat     Transpose() const;
      ZMat &   DoTranspose();

      ZMat     Inv(void) const;
      ZMat &   DoInv(void);

      ZMat     LUDecomposition(ZArr<int> &IndexArr, double & RowInterchanges) const;
      ZMat &   DoLUDecomposition(ZArr<int> &IndexArr, double & RowInterchanges);

      void     LUBackSubstitution(ZArrDouble & Answer, const ZArr<int> & Index) const;

      ZMat     Exp(int RungeOrder = 20) const;
      ZMat &   DoExp(int RungeOrder = 20);

      ZMat     Pow(int Power) const;
      ZMat &   DoPow(int Power);


      double   Det(void) const;
      double   Trace() const;

      void     CopyTo(real_matrix &rmDest, bool bReallocate = false);

      //--------------------
      // Static
      //--------------------
      static double Sum(const ZMat & mIn);

      //--------------------
      // Friends
      //--------------------
      friend std::ostream & operator<<(std::ostream & os, const ZMat & rhs);
      friend real_matrix & operator<<(real_matrix & rm, const ZMat & rhs);	// Only makes sense for double
      friend ZMat operator*(double, const ZMat &);
};

#undef DllImportExport

//-----------------------------------------------------------------------------
#endif	Z_MAT_H
