#ifndef Z_MATRIX_HPP
#define Z_MATRIX_HPP
//-----------------------------------------------------------------------------
//	This is the matrix template class.
// The template type "Type" can be any sensible class or basic type, but must support
// the following:
//			OPERATORS
//		Type + Type
//		Type += Type
//		Type*Type
//		Type *= Type
//		ostream << Type

//-----------------------------------------------------------------------------
#include "ZArr.hpp"
#include "ZComplex.h"
#include "ZException.hpp"

//-----------------------------------------------------------------------------
template <class Type>
class ZMatrix
{
	protected:
		int	prRows, prCols;
		ZArr<Type> *prM;
	public:
      enum EnumStreamMode{smOneLine=1,smMultiLine=2};
		static EnumStreamMode sm_enumDisplayMode;
			// Constructors
		ZMatrix(int rows=1, int cols=1);								// Default Constructor
		ZMatrix(int rows, int cols, const Type & InitValue);	// Gives an initial value
		ZMatrix(const ZMatrix & Matrix);										// Copy Constructor

			// Destructor
		virtual ~ZMatrix();

			// Operator Overload Methods
		ZArr<Type> & operator[](int rowIndex) throw(ZException);
		const ZArr<Type> & operator[](int rowIndex) const throw(ZException);
		ZMatrix &operator=(ZMatrix & rhs);
		ZMatrix &operator=(int & scalar);
		ZMatrix	operator+(ZMatrix & rhs);
		ZMatrix	operator-(ZMatrix & rhs);
		ZMatrix	operator*(const Type & rhs);
		ZMatrix	operator/(const Type & rhs);
		ZMatrix	operator*(const ZMatrix & rhs);
		ZMatrix	operator/(const ZMatrix & rhs);
		ZMatrix &operator+=(ZMatrix & rhs);
		ZMatrix &operator-=(ZMatrix & rhs);
		ZMatrix &operator*=(const Type & rhs);
		ZMatrix &operator/=(const Type & rhs);
		ZMatrix &operator*=(const ZMatrix & rhs);

			// Misc Uilitiles
		void Dim(int & RowCount, int & ColCount) const;
		void SetDim(int RowCount, int ColCount);

			// Friends
      friend std::ostream & operator<<(std::ostream & os, ZMatrix & rhs);
};

#include "ZMatrixCode.hpp"
//-----------------------------------------------------------------------------
typedef ZMatrix<ZComplex>	ZMatrixComplex;

//-----------------------------------------------------------------------------
#endif Z_MATRIX_HPP