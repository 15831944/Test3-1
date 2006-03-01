#ifndef Z_MATRIX_CODE_HPP
#define Z_MATRIX_CODE_HPP
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
#include <stdlib.h>

#include "ZArr.hpp"
#include "ZComplex.h"
#include "ZException.hpp"
#include "ZMatrix.hpp"

//-----------------------------------------------------------------------------
#ifdef DEBUG_MSG
void print_ZMatrix_debug_msg(char *msg)
{
	printf("ZMatrix::Debug:<%s>\n", msg);
}
#endif DEBUG_MSG

//-----------------------------------------------------------------------------
template <class Type>
typename ZMatrix<Type>::EnumStreamMode ZMatrix<Type>::sm_enumDisplayMode = ZMatrix<Type>::smOneLine;

//-----------------------------------------------------------------------------
template <typename Type>
ZMatrix<Type>::ZMatrix(int rows, int cols)
{
	ZArr<Type>	tempA(cols);

	prRows = rows;
	prCols = cols;

	prM = new ZArr<Type>[rows];	// Calls Default Constructor for each element of
											// array
#ifdef DEBUG_MEMORY
	DebugMemAllocate((pvoid)prM, "ZMatrix::ZMatrix(int,int)");
#endif

	for (int i = 0; i < rows; i++)
		prM[i] = tempA;				// Uses Assignment operator
}

//-----------------------------------------------------------------------------
template <typename Type>
ZMatrix<Type>::ZMatrix(int rows, int cols, const Type & InitValue)
{
	ZArr<Type>	tempA(cols, InitValue);

	prRows = rows;
	prCols = cols;

	prM = new ZArr<Type>[rows];	// Calls Default Constructor for each element of
											// ZArr<Type> array
#ifdef DEBUG_MEMORY
	DebugMemAllocate((pvoid)prM, "ZMatrix::ZMatrix(int,int,Type&)");
#endif

	for (int i = 0; i < rows; i++)
		prM[i] = tempA;				// Uses Assignment operator
}

//-----------------------------------------------------------------------------
template <typename Type>
ZMatrix<Type>::ZMatrix(const ZMatrix<Type> & Matrix)	// Copy Constructor
{
	prRows = Matrix.prRows;
	prCols = Matrix.prCols;

	prM = new ZArr<Type>[prRows];	// Calls Default Constructor for each element of
											// ZArr<Type> array
#ifdef DEBUG_MEMORY
	DebugMemAllocate((pvoid)prM, "ZMatrix::ZMatrix(ZMatrix&)");
#endif

	for (int i = 0; i < prRows; i++)
		prM[i] = Matrix.prM[i];		// Calls ZArr<Type> Assignment operator method
}

//-----------------------------------------------------------------------------
template <typename Type>
ZMatrix<Type>::~ZMatrix()
{
#ifdef DEBUG_MEMORY
	DebugMemDestroy((pvoid)prM, "ZMatrix::~ZMatrix()");
#endif
	delete [] prM;
}

//-----------------------------------------------------------------------------
template <typename Type>
ZArr<Type> & ZMatrix<Type>::operator[](int rowIndex) throw(ZException)
{
	if ((rowIndex < 0) || (rowIndex >= prRows))
	{
		throw ZException("ZMatrix::operator[]:Error: row Index out of range",0);
	}	// ELSE

	return prM[rowIndex];
}

//-----------------------------------------------------------------------------
template <typename Type>
const ZArr<Type> & ZMatrix<Type>::operator[](int rowIndex) const  throw(ZException)
{
	if ((rowIndex < 0) || (rowIndex >= prRows))
	{
		throw ZException("ZMatrix::operator[]:Error: row Index out of range",0);
	}	// ELSE

	return prM[rowIndex];
}

//-----------------------------------------------------------------------------
template <typename Type>
ZMatrix<Type> & ZMatrix<Type>::operator=(ZMatrix<Type> & rhs)
{
	if (!((rhs.prRows == prRows) && (rhs.prCols == prCols)))
	{
#ifdef DEBUG_MEMORY
		DebugMemDestroy((pvoid)prM, "ZMatrix::operator=(ZMatrix&)");
#endif
		delete [] prM;
		prRows = rhs.prRows;
		prCols = rhs.prCols;

		prM = new ZArr<Type>[prRows];	// Calls Default Constructor for each element of
											// ZArr<Type> array
#ifdef DEBUG_MEMORY
		DebugMemAllocate((pvoid)prM, "ZMatrix::operator=(ZMatrix&)");
#endif

	for (int i = 0; i < prRows; i++)
			prM[i] = rhs.prM[i];		// Calls ZArr<Type> Assignment operator method
	}
	else
	{
		for (int row = 0; row < prRows; row++)
			prM[row] = rhs.prM[row];
	}	// ELSE
	return (*this);
}

//-----------------------------------------------------------------------------
template <typename Type>
ZMatrix<Type> & ZMatrix<Type>::operator=(int & scalar)
{
	ZArr<Type>	tempA(1);

	if ((1 != prRows) || (1 != prCols))
	{
#ifdef DEBUG_MEMORY
		DebugMemDestroy((pvoid)prM, "ZMatrix::operator=(int&)");
#endif
		delete [] prM;
		prRows = 1;
		prCols = 1;

		prM = new ZArr<Type>[1];	// Calls Default Constructor for each element of
											// ZArr<Type> array
#ifdef DEBUG_MEMORY
		DebugMemAllocate((pvoid)prM, "ZMatrix::operator=(int&)");
#endif
	}

	tempA[0] = scalar;
	prM[0] = tempA;

	return (*this);
}


//-----------------------------------------------------------------------------
template <typename Type>
ZMatrix<Type> ZMatrix<Type>::operator+(ZMatrix<Type> & rhs)
{
	// A = this + rhs 
	// A = this.operator+(rhs)
	if (!((rhs.prRows == prRows) && (rhs.prCols == prCols)))
	{
		cout << "Error in matrix addition. Matrix sizes aren't identical\n";
		return (*this);
	}

	ZMatrix<Type> A(*this);	// Copy constructor

	for (int row = 0; row < prRows; row++)
		A.prM[row] += rhs.prM[row];

	return A;	// Returns a copy of A that is used as the rhs arguament for the
}

//-----------------------------------------------------------------------------
template <typename Type>
ZMatrix<Type> ZMatrix<Type>::operator-(ZMatrix<Type> & rhs)
{
	// A = this - rhs 
	// A = this.operator-(rhs)
	if (!((rhs.prRows == prRows) && (rhs.prCols == prCols)))
	{
		cout << "Error in matrix subtraction. Matrix sizes aren't identical\n";
		return (*this);
	}

	ZMatrix<Type> A(*this);	// Copy constructor

	for (int row = 0; row < prRows; row++)
		A.prM[row] -= rhs.prM[row];

	return A;	// Returns a copy of A that is used as the rhs arguament for the
}

//-----------------------------------------------------------------------------
template <typename Type>
ZMatrix<Type> ZMatrix<Type>::operator*(const Type & rhs)
{
	//	A = this*rhs;	// rhs is scalar
	// this.operator*(rhs)
	ZMatrix<Type>	A(*this);

	for (int i = 0; i < prRows; i++)
		A.prM[i] *= rhs;

	return A;
}

//-----------------------------------------------------------------------------
template <typename Type>
ZMatrix<Type> ZMatrix<Type>::operator/(const Type & rhs)
{
	//	A = this*rhs;	// rhs is scalar
	// this.operator*(rhs)
	ZMatrix<Type>	A(*this);

	for (int i = 0; i < prRows; i++)
		A.prM[i] /= rhs;

	return A;
}

//-----------------------------------------------------------------------------
template <typename Type>
ZMatrix<Type> ZMatrix<Type>::operator*(const ZMatrix<Type> & rhs)
{
	//	A = this*rhs;	// rhs is a matrix
	// this.operator*(rhs)

	if (prCols != rhs.prRows)
	{
		cout << "Error in matrix multiplication. Left Matrix column count != Right Matrix Row Count\n";
		return (*this);
	}
	ZMatrix<Type>	A(prRows, rhs.prCols);

	for (int rows = 0; rows < prRows; rows++)
		for (int cols = 0; cols < rhs.prCols; cols++)
		{
			A.prM[rows][cols] = prM[rows][0]*rhs.prM[0][cols];
			for (int i = 1; i < prCols; i++)
				A.prM[rows][cols] += prM[rows][i]*rhs.prM[i][cols];
		}	// FOR

	return A;
}

//-----------------------------------------------------------------------------
template <typename Type>
ZMatrix<Type> & ZMatrix<Type>::operator+=(ZMatrix<Type> & rhs)
{
	// this += rhs 
	// this.operator+=(rhs)
	if (!((rhs.prRows == prRows) && (rhs.prCols == prCols)))
	{
		cout << "Error in matrix addition. Matrix sizes aren't identical\n" << endl;
		return (*this);
	}

	for (int row = 0; row < prRows; row++)
		prM[row] += rhs.prM[row];

	return *this;
}

//-----------------------------------------------------------------------------
template <typename Type>
ZMatrix<Type> & ZMatrix<Type>::operator-=(ZMatrix<Type> & rhs)
{
	// this -= rhs 
	// this.operator+=(rhs)
	if (!((rhs.prRows == prRows) && (rhs.prCols == prCols)))
	{
		cout << "Error in matrix subtraction. Matrix sizes aren't identical\n" << endl;
		return (*this);
	}

	for (int row = 0; row < prRows; row++)
		prM[row] -= rhs.prM[row];

	return *this;
}

//-----------------------------------------------------------------------------
template <typename Type>
ZMatrix<Type> & ZMatrix<Type>::operator*=(const Type & rhs)
{
	// this *= rhs // rhs is scalar
	// this.operator*=(rhs)

	for (int row = 0; row < prRows; row++)
		prM[row] *= rhs;

	return *this;
}

//-----------------------------------------------------------------------------
template <typename Type>
ZMatrix<Type> & ZMatrix<Type>::operator/=(const Type & rhs)
{
	// this /= rhs // rhs is scalar
	// this.operator/=(rhs)

	for (int row = 0; row < prRows; row++)
		prM[row] /= rhs;

	return *this;
}

//-----------------------------------------------------------------------------
template <typename Type>
ZMatrix<Type> & ZMatrix<Type>::operator*=(const ZMatrix<Type> & rhs)
{	// If the two matrices aren't square, but satisfy the condition that
	// this.prCols == rhs.prRows, then the new this matrix will have changed
	// size.
	//
	//	this *= rhs;	// rhs is a matrix
	// this.operator*=(rhs)

	if (prCols != rhs.prRows)
	{
		cout << "Error in matrix multiplication. Left Matrix column count != Right Matrix Row Count\n";
		return (*this);
	}

	ZMatrix<Type>	A(prRows, rhs.prCols);

	for (int rows = 0; rows < prRows; rows++)
		for (int cols = 0; cols < rhs.prCols; cols++)
		{
			A.prM[rows][cols] = prM[rows][0]*rhs.prM[0][cols];
			for (int i = 1; i < prCols; i++)
				A.prM[rows][cols] += prM[rows][i]*rhs.prM[i][cols];
		}	// FOR

#ifdef DEBUG_MEMORY
	DebugMemDestroy((pvoid)prM, "ZMatrix::operator*=(ZMatrix&)");
#endif
	delete [] prM;

	prM = new ZArr<Type>[prRows];
#ifdef DEBUG_MEMORY
	DebugMemAllocate((pvoid)prM, "ZMatrix::operator*=(ZMatrix&)");
#endif
	prCols = rhs.prCols;

	for (int i = 0; i < prRows; i++)
		prM[i] = A.prM[i];		// Calls ZArr<Type> Assignment operator method

	return *this;
}

//-----------------------------------------------------------------------------
//	MISC UTILITIES
//-----------------------------------------------------------------------------
template <typename Type>
void ZMatrix<Type>::Dim(int & RowCount, int & ColCount) const
{
	RowCount = prRows;
	ColCount = prCols;
}

//-----------------------------------------------------------------------------
template <typename Type>
void ZMatrix<Type>::SetDim(int RowCount, int ColCount)
{
	ZArr<Type>	*pTempMat;
	ZArr<Type>	tempArr(ColCount);

	if ((prRows == RowCount) && (prCols == ColCount))
		return;


	pTempMat = new ZArr<Type>[RowCount];
#ifdef DEBUG_MEMORY
	DebugMemAllocate((pvoid)pTempMat, "ZMatrix::SetDim(int,int)");
#endif

	for (int i = 0; i < RowCount; i++)
		pTempMat[i] = tempArr;				// Uses Assignment operator to set size

	for (int row = 0; row < Min(RowCount, prRows); row++)
		for (int col = 0; col < Min(ColCount, prCols); col++)
			pTempMat[row][col] = prM[row][col];

#ifdef DEBUG_MEMORY
	DebugMemDestroy((pvoid)prM, "ZMatrix::SetDim(int,int)");
#endif
	delete [] prM;
	prM = pTempMat;

	prRows = RowCount;
	prCols = ColCount;
}

//-----------------------------------------------------------------------------
//	MATH UTILITIES
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//	FRIEND MEMBER FUNCTIONS
//-----------------------------------------------------------------------------
template <typename Type>
std::ostream & operator<<(std::ostream & os, ZMatrix<Type> & rhs)
{
	int	row, col, i;

	if ((rhs.prRows == 0) || (rhs.prCols == 0))
	{
			os << "[]";
		return os;
	}

	if (rhs.ms_enumDisplayMode == ZMatrix<Type>::smOneLine)
	{
		os << "[";
		for (row = 0; row < rhs.prRows; row++)
		{
			os << rhs[row][0];
			for (col = 1; col < rhs.prCols; col++)
			{
				os << "," << rhs[row][col];
			}
			if (row < (rhs.prRows-1))
				os << ";";
		}	// for
		os << "]";
	}
	else
	{
		int	*maxColumnStringSizes = new int[rhs.prCols];
#ifdef DEBUG_MEMORY
		DebugMemAllocate((pvoid)maxColumnStringSizes, "ZMatrix friend::operator<<(ostream&,ZMatrix&)");
#endif
		int	maxStringSize = 0;
//		ZMatrix<char*>	strArr(rhs.prRows, rhs.prCols);
		ZMatrix<char*>	strArr(rhs.prRows, rhs.prCols);
		char	tempStr[100], fmtStr[10];
		memset(maxColumnStringSizes, sizeof(maxColumnStringSizes),0);

		int strWidth;

		for (row = 0; row < rhs.prRows; row++)
		{
			for (col = 0; col < rhs.prCols; col++)
			{
				strWidth =  sprintf(tempStr, "%g", rhs.prM[row][col]);
				strArr[row][col] = new char[strWidth+1];
#ifdef DEBUG_MEMORY
				DebugMemAllocate((pvoid)strArr[row][col], "ZMatrix friend::operator<<(ostream&,ZMatrix&)");
#endif
				strcpy(strArr[row][col], tempStr);
				if (strWidth > maxColumnStringSizes[col])
					maxColumnStringSizes[col] = strWidth;
				if (strWidth > maxStringSize)
					maxStringSize = strWidth;
			}	// FOR
		}	// FOR

		sprintf(fmtStr, "%c%ds", '%', maxStringSize);
		// We aim to get something like
		//	 _               _
		//	| 34.67  5.89 2.1 |
		// |   8.6 67.90 2.2 |
		// |_    8  11.2   0_|
		//  
		// Print the top line
		//	 _               _
		printf("\n _");
		for (col = 0; col < rhs.prCols; col++)
//			for (i = 0; i <= maxColumnStringSizes[col]; i++)
			for (i = 0; i < maxStringSize; i++)
				printf(" ");
		for (col = 0; col < (rhs.prCols-1); col++)
			printf(" ");

		printf("_\n");
		for (row = 0; row < rhs.prRows; row++)
		{
			if (row != (rhs.prRows-1))
				printf("| ");
			else
				printf("|_");
			for (col = 0; col < rhs.prCols; col++)
			{
//				sprintf(fmtStr, "%c%ds", '%', maxColumnStringSizes[col]);
				printf(fmtStr, strArr[row][col]);
				if (col < (rhs.prCols-1))
					printf(" ");
			}
			if (row == (rhs.prRows-1))
				printf("_|\n");
			else
				printf(" |\n");
		}

		for (row = 0; row < rhs.prRows; row++)
		{
			for (col = 0; col < rhs.prCols; col++)
			{
#ifdef DEBUG_MEMORY
				DebugMemDestroy((pvoid)strArr[row][col], "ZMatrix friend operator<<(ostream&,ZMatrix&)");
#endif
				delete strArr[row][col];
			}
		}	//	FOR

#ifdef DEBUG_MEMORY
		DebugMemDestroy((pvoid)maxColumnStringSizes, "ZMatrix friend operator<<(ostream&,ZMatrix&)");
#endif
		delete [] maxColumnStringSizes;
	}

	return os;
} 

//-----------------------------------------------------------------------------
#endif Z_MATRIX_CODE_HPP