#include <stdafx.h>
#ifdef	__BORLANDC__
#include <vcl.h>
#pragma hdrstop
#endif __BORLANDC__

#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <math.h>

#include "ZMat.h"
#include "ZMatrix.hpp"
#include "eispack.h"
#include "DebugMemory.h"
#include "InlineUtils.hpp"

const double SIZE_OF_TIM_AND_MARKS_BRAIN = 1.0e-30;

//=============================================================================
// PROCEDURE:
//    ZMat::ZMat
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Constructor that accepts a real_matrix.
//=============================================================================
ZMat::ZMat(const real_matrix & r_matrix)
{
	ZArrDouble tempA(r_matrix.col_dim);

   display_mode = smOneLine;


	prRows = r_matrix.row_dim;
	prCols = r_matrix.col_dim;

	prM = new ZArrDouble[prRows];	// Calls Default Constructor for each element of
											// ZArrDouble array
#ifdef DEBUG_MEMORY
   DebugMemAllocate((pvoid)prM, "ZMat::ZMat(const real_matrix &)");
#endif

	for (int row = 0; row < prRows; row++)
	{
		for (int col = 0; col < prCols; col++)
			tempA[col] = r_matrix.A[row*prCols + col];

		prM[row] = tempA;
	}
}

//=============================================================================
// PROCEDURE:
//    ZMat::ZMat
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Copy Constructor.
//=============================================================================
ZMat::ZMat(const ZMat & rhs)
{
   display_mode = smOneLine;

	prRows = rhs.prRows;
	prCols = rhs.prCols;

	prM = new ZArrDouble[prRows];	// Calls Default Constructor for each element of
											   // ZArrDouble array
#ifdef DEBUG_MEMORY
      DebugMemAllocate((pvoid)prM, "ZMat::ZMat(const ZMat &)");
#endif

	for (int i = 0; i < prRows; i++)
		prM[i] = rhs.prM[i];		// Calls ZArrDouble Assignment operator method
}

//=============================================================================
// PROCEDURE:
//    ZMat::ZMat
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Constructor that contains size parameters.
//=============================================================================
ZMat::ZMat(int rows, int cols)
{
   display_mode = smOneLine;

	ZArrDouble	tempA(cols);

	prRows = rows;
	prCols = cols;

	prM = new ZArrDouble[rows];	// Calls Default Constructor for each element of
											// array
#ifdef DEBUG_MEMORY
   DebugMemAllocate((pvoid)prM, "ZMat::ZMat(int,int)");
#endif

	for (int i = 0; i < rows; i++)
		prM[i] = tempA;				// Uses Assignment operator
}

//=============================================================================
// PROCEDURE:
//    ZMat::ZMat
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Constructor that contains size parameters and initial value parameter.
//=============================================================================
ZMat::ZMat(int rows, int cols, double InitValue)
{
	ZArrDouble	tempA(cols, InitValue);

   display_mode = smOneLine;

	prRows = rows;
	prCols = cols;

	prM = new ZArrDouble[rows];	// Calls Default Constructor for each element of
											// ZArrDouble array
#ifdef DEBUG_MEMORY
   DebugMemAllocate((pvoid)prM, "ZMat::ZMat(int,int,double)");
#endif

   for (int i = 0; i < rows; i++)
		prM[i] = tempA;				// Uses Assignment operator
}

//=============================================================================
// PROCEDURE:
//    ZMat::ZMat
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Constructor that creates a 1x1 scalar matrix
//=============================================================================
/*
ZMat::ZMat(double InitValue)  // 
{
   display_mode = smOneLine;

	prM = new ZArrDouble[1];	// Calls Default Constructor for each element of
										// ZArrDouble array
#ifdef DEBUG_MEMORY
   DebugMemAllocate((pvoid)prM, "ZMat::ZMat(double)");
#endif
	prM[0][0] = InitValue;
}
*/

//=============================================================================
// PROCEDURE:
//    ZMat::ZMat
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Constructor taking a ZString parameter.
//
//	We expect the Matlab format for a matrix here. eg:
//	   [1 2 3; 4 5 6; 7 8 9];
//	   [1,2,3;4,5,6;7,8,9];
//	Spaces or comma's are mandatory for seperators
//	   [1-2,3;6 7-9];	will try to interprate 7-9 which will fail
//=============================================================================
ZMat::ZMat(const ZString &InitValue) throw (ZException)   // Constructor that creates from formated string
{
	ZMat		tempM;
	ZString	matStr = InitValue;
	ZString	token, rowStr;
	int		start, end;
	int		rowCount, colCount, row, col, i;

   display_mode = smOneLine;

   this->prM = NULL;
   this->prRows = 0;
   this->prCols = 0;

   //	Ensure that there is nothing but white space before the first '['
	matStr.InitToken();
	token = matStr.NextToken(" \t");
	if (token == "")
	{
		return;
	}
	else if (token[0] != '[')
	{
		char	msg[100];

		sprintf(msg, "ZMat::ZMat(ZString&): Expected '[' but found %c", token[0]);
		throw ZException(msg,0);
	}

	//	Look for the text within the square brackets "[...]"
	if ((start = matStr.FirstDelimiter("[")) < 0)
		throw ZException("ZMat::ZMat(ZString&): '[' not found",0);

	if ((end = matStr.FirstDelimiter("]")) < 0)
		throw ZException("ZMat::ZMat(ZString&): ']' not found",0);

	if ((end-start) == 1)	//	Then the NULL matrix
	{
		return;
	}

	//	Grab the string between the square brackers
	matStr = matStr.SubString(start+1,end-start-1);
	matStr.InitToken();

	//	Is the string contents just white space
	token = matStr.NextToken(" \t");
	if (token == "")
	{	// If white space, return the NULL matrix
		return;
	}

	//	Search for the number of rows
	matStr.InitToken();
	rowCount = 0;
	while ((token = matStr.NextToken(";")) != "")
	{
		rowCount++;
	}	//	WHILE;

	//	Determine the number of columns
	matStr.InitToken();
	rowStr = matStr.NextToken(";");
	colCount = 0;
	while (rowStr.NextToken(" ,") != "")
		colCount++;

	if ((colCount == 0) || (rowCount == 0))
	{
		return;
	}

	//	Now we attempt to go through and read in all the value to tempM
	tempM.SetDim(rowCount, colCount);

	matStr.InitToken();
	for (row = 0; row < rowCount; row++)
	{
		rowStr = matStr.NextToken(";");

		rowStr.InitToken();

		col = 0;
		while ((token = rowStr.NextToken(" ,")) != "")
		{
			try
			{
				tempM[row][col] = token.ToDouble();
			}
			catch(...)
			{
				char	msg[100];

				sprintf(msg, "ZMat::ZMat(ZString&): Unable to convert \"%s\" to number", token.c_str());
				throw ZException(msg,0);
			}
			col++;
		}	//	WHILE
		//	Note that if this row is less than full we need to initialise the remaining
		//	terms to 0
		for (i = col; i < colCount; i++)
			tempM[row][i] = 0.0;
	}	//	for row = 0 to rowCount-1

	(*this) = tempM;
}

//=============================================================================
// PROCEDURE:
//    ZMat::~ZMat
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Destructor.
//=============================================================================
ZMat::~ZMat()
{
#ifdef DEBUG_MEMORY
   DebugMemDestroy((pvoid)prM, "ZMat::~ZMat()");
#endif
	delete [] prM;
}


//=================
// OPERATOR OVERLOAD METHODS
//=================

//=============================================================================
// PROCEDURE:
//    ZMat::operator[]
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Index operator overloading. This is the version used with a command of
// the form:
//    Mat[4][5] = 34.0;
// i.e. non-const.
//=============================================================================
ZArrDouble & ZMat::operator[](int rowIndex) throw(ZException)
{
	if ((rowIndex < 0) || (rowIndex >= prRows))
	{
		throw ZException("ZMat::operator[]:Error: row Index out of range",0);
	}	// ELSE

	return prM[rowIndex];
}

//=============================================================================
// PROCEDURE:
//    ZMat::operator[]
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Index operator overloading. This is the version used with a command of
// the form:
//    a = Mat[4][5];
// i.e. const.
//=============================================================================
const ZArrDouble & ZMat::operator[](int rowIndex) const throw(ZException)
{
	if ((rowIndex < 0) || (rowIndex >= prRows))
	{
		throw ZException("ZMat::operator[]:Error: row Index out of range",0);
	}	// ELSE

	return prM[rowIndex];
}

//=============================================================================
// PROCEDURE:
//    ZMat::operator=
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Default Assignment Operator overloading.
//=============================================================================
ZMat & ZMat::operator=(const ZMat & rhs)
{
	if (!((rhs.prRows == prRows) && (rhs.prCols == prCols)))
	{
#ifdef DEBUG_MEMORY
      DebugMemDestroy((pvoid)prM, "ZMat::operator=(const ZMat &");
#endif
		delete [] prM;
		prRows = rhs.prRows;
		prCols = rhs.prCols;

		prM = new ZArrDouble[prRows];	// Calls Default Constructor for each element of
											      // ZArrDouble array
#ifdef DEBUG_MEMORY
      DebugMemAllocate((pvoid)prM, "ZMat::ZMat(const ZMat &)");
#endif

	for (int i = 0; i < prRows; i++)
			prM[i] = rhs.prM[i];		// Calls ZArrDouble Assignment operator method
	}
	else
	{
		for (int row = 0; row < prRows; row++)
			prM[row] = rhs.prM[row];
	}	// ELSE
	return (*this);
}

//=============================================================================
// PROCEDURE:
//    ZMat::operator=
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Assignment Operator overloading for int rhs.
//=============================================================================
ZMat & ZMat::operator=(const int & scalar)
{
	ZArrDouble	tempA(1);

	if ((1 != prRows) || (1 != prCols))
	{
#ifdef DEBUG_MEMORY
      DebugMemDestroy((pvoid)prM, "ZMat::operator=(const int&");
#endif
		delete [] prM;
		prRows = 1;
		prCols = 1;

		prM = new ZArrDouble[1];	// Calls Default Constructor for each element of
											// ZArrDouble array
#ifdef DEBUG_MEMORY
      DebugMemAllocate((pvoid)prM, "ZMat::operator=(const int &)");
#endif
	}

	tempA[0] = scalar;
	prM[0] = tempA;

	return (*this);
}

//=============================================================================
// PROCEDURE:
//    ZMat::operator=
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Assignment Operator overloading for real_matrix rhs.
//=============================================================================
ZMat & ZMat::operator=(const real_matrix & rhs)
{
	ZArrDouble	tempA(rhs.col_dim);

	if ((rhs.row_dim != prRows) || (rhs.col_dim != prCols))
	{
#ifdef DEBUG_MEMORY
      DebugMemDestroy((pvoid)prM, "ZMat::operator=(const real_matrix&");
#endif
		delete [] prM;
		prRows = rhs.row_dim;
		prCols = rhs.col_dim;

		prM = new ZArrDouble[prRows];	// Calls Default Constructor for each element of
											// ZArrDouble array
#ifdef DEBUG_MEMORY
      DebugMemAllocate((pvoid)prM, "ZMat::operator=(const real_matrix &)");
#endif
	}

	for (int row = 0; row < prRows; row++)
	{
		for (int col = 0; col < prCols; col++)
			tempA[col] = rhs.A[row*prCols + col];

		prM[row] = tempA;
	}
	return (*this);
}

//=============================================================================
// PROCEDURE:
//    ZMat::operator=
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Assignment Operator overloading for ZString rhs.
//=============================================================================
ZMat & ZMat::operator=(const ZString & rhs) throw (ZException)
{
   ZMat  tempM(rhs);

   (*this) = tempM;

   return (*this);
}
/*ZMat & ZMat::operator=(const ZString & rhs) throw (ZException)
{
	//	We expect the Matlab format for a matrix here
	//	eg.
	//	[1 2 3; 4 5 6; 7 8 9];
	//	[1,2,3;4,5,6;7,8,9];
	//	Spaces or comma's are mandatory for seperators
	//	[1-2,3;6 7-9];	will try to interprate 7-9 which will fail
	ZMat		tempM;
	ZString	matStr = rhs;
	ZString	token, rowStr;
	int		start, end;
	int		rowCount, colCount, row, col, i;

	//	Ensure that there is nothing but white space before the first '['
	matStr.InitToken();
	token = matStr.NextToken(" \t");
	if (token == "")
	{
		this->SetDim(0,0);
		return *this;
	}
	else if (token[0] != '[')
	{
		char	msg[100];

		sprintf(msg, "ZMat::operator=(ZString&): Expected '[' but found %c", token[0]);
		throw ZException(msg,0);
	}

	//	Look for the text within the square brackets "[...]"
	if ((start = matStr.FirstDelimiter("[")) < 0)
		throw ZException("ZMat::operator=(ZString&): '[' not found",0);

	if ((end = matStr.FirstDelimiter("]")) < 0)
		throw ZException("ZMat::operator=(ZString&): ']' not found",0);

	if ((end-start) == 1)	//	Then the NULL matrix
	{
		this->SetDim(0,0);
		return *this;
	}

	//	Grab the string between the square brackers
	matStr = matStr.SubString(start+1,end-start-1);
	matStr.InitToken();

	//	Is the string contents just white space
	token = matStr.NextToken(" \t");
	if (token == "")
	{	// Is white space, return the NULL matrix
		this->SetDim(0,0);
		return *this;
	}

	//	Search for the number of rows
	matStr.InitToken();
	rowCount = 0;
	while ((token = matStr.NextToken(";")) != "")
	{
		rowCount++;
	}	//	WHILE;

	//	Determine the number of columns
	matStr.InitToken();
	rowStr = matStr.NextToken(";");
	colCount = 0;
	while (rowStr.NextToken(" ,") != "")
		colCount++;

	if ((colCount == 0) || (rowCount == 0))
	{
		this->SetDim(0,0);
		return *this;
	}

	//	Now we attempt to go through and read in all the value to tempM
	tempM.SetDim(rowCount, colCount);

	matStr.InitToken();
	for (row = 0; row < rowCount; row++)
	{
		rowStr = matStr.NextToken(";");

		rowStr.InitToken();

		col = 0;
		while ((token = rowStr.NextToken(" ,")) != "")
		{
			try
			{
				tempM[row][col] = token.ToDouble();
			}
			catch(...)
			{
				char	msg[100];
				
				sprintf(msg, "ZMat::operator=(ZString&): Unable to convert \"%s\" to number", token.c_str());
				throw ZException(msg,0);
			}
			col++;
		}	//	WHILE
		//	Note that if this row is less than full we need to initialise the remaining
		//	terms to 0
		for (i = col; i < colCount; i++)
			tempM[row][i] = 0.0;
	}	//	for row = 0 to rowCount-1

	(*this) = tempM;

	return *this;
} */

//-----------------------------------------------------------------------------
ZMat	ZMat::operator+(const ZMat & rhs) throw (ZException)
{
	// A = this + rhs 
	// A = this.operator+(rhs)
	if (!((rhs.prRows == prRows) && (rhs.prCols == prCols)))
	{
		throw ZException("Error in matrix addition. Matrix sizes aren't identical");
	}

	ZMat A(*this);	// Copy constructor

	for (int row = 0; row < prRows; row++)
		A.prM[row] += rhs.prM[row];

	return A;	// Returns a copy of A that is used as the rhs arguament for the
}

//-----------------------------------------------------------------------------
ZMat	ZMat::operator-(const ZMat & rhs) throw (ZException)
{
	// A = this - rhs 
	// A = this.operator-(rhs)
	if (!((rhs.prRows == prRows) && (rhs.prCols == prCols)))
	{
		throw ZException("Error in matrix subtraction. Matrix sizes aren't identical");
	}

	ZMat A(*this);	// Copy constructor

	for (int row = 0; row < prRows; row++)
		A.prM[row] -= rhs.prM[row];

	return A;	// Returns a copy of A that is used as the rhs arguament for the
}

//-----------------------------------------------------------------------------
ZMat ZMat::operator*(const double & rhs)
{
	//	A = this*rhs;	// rhs is scalar
	// this.operator*(rhs)
	ZMat	A(*this);

	for (int i = 0; i < prRows; i++)
		A.prM[i] *= rhs;

	return A;
}

//-----------------------------------------------------------------------------
ZMat ZMat::operator/(const double & rhs)
{
	//	A = this*rhs;	// rhs is scalar
	// this.operator*(rhs)
	ZMat	A(*this);

	for (int i = 0; i < prRows; i++)
		A.prM[i] /= rhs;

	return A;
}

//-----------------------------------------------------------------------------
ZMat ZMat::operator*(const ZMat & rhs) const throw (ZException)
{
	//	A = this*rhs;	// rhs is a matrix
	// this.operator*(rhs)

	if (prCols != rhs.prRows)
	{
		throw ZException("Error in matrix multiplication. Left Matrix column count != Right Matrix Row Count");
	}
	ZMat	A(prRows, rhs.prCols);

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
ZMat & ZMat::operator+=(const ZMat & rhs) throw (ZException)
{
	// this += rhs 
	// this.operator+=(rhs)
	if (!((rhs.prRows == prRows) && (rhs.prCols == prCols)))
	{
		throw ZException("Error in matrix addition. Matrix sizes aren't identical");
	}

	for (int row = 0; row < prRows; row++)
		prM[row] += rhs.prM[row];

	return *this;
}

//-----------------------------------------------------------------------------
ZMat & ZMat::operator-=(const ZMat & rhs) throw (ZException)
{
	// this -= rhs 
	// this.operator+=(rhs)
	if (!((rhs.prRows == prRows) && (rhs.prCols == prCols)))
	{
		throw ZException("Error in matrix subtraction. Matrix sizes aren't identical");
	}

	for (int row = 0; row < prRows; row++)
		prM[row] -= rhs.prM[row];

	return *this;
}
//-----------------------------------------------------------------------------
ZMat & ZMat::operator*=(const double & rhs)
{
	// this *= rhs // rhs is scalar
	// this.operator*=(rhs)

	for (int row = 0; row < prRows; row++)
		prM[row] *= rhs;

	return *this;
}

//-----------------------------------------------------------------------------
ZMat & ZMat::operator/=(const double & rhs)
{
	// this /= rhs // rhs is scalar
	// this.operator/=(rhs)

	for (int row = 0; row < prRows; row++)
		prM[row] /= rhs;

	return *this;
}

//-----------------------------------------------------------------------------
ZMat & ZMat::operator*=(const ZMat & rhs) throw (ZException)
{	// If the two matrices aren't square, but satisfy the condition that
	// this.prCols == rhs.prRows, then the new this matrix will have changed
	// size.
	//
	//	this *= rhs;	// rhs is a matrix
	// this.operator*=(rhs)

	if (prCols != rhs.prRows)
	{
		throw ZException("Error in matrix multiplication. Left Matrix column count != Right Matrix Row Count");
	}

	ZMat	A(prRows, rhs.prCols);

	for (int rows = 0; rows < prRows; rows++)
		for (int cols = 0; cols < rhs.prCols; cols++)
		{
			A.prM[rows][cols] = prM[rows][0]*rhs.prM[0][cols];
			for (int i = 1; i < prCols; i++)
				A.prM[rows][cols] += prM[rows][i]*rhs.prM[i][cols];
		}	// FOR

#ifdef DEBUG_MEMORY
   DebugMemDestroy((pvoid)prM, "ZMat::operator*=(const ZMat&");
#endif
	delete [] prM;

	prM = new ZArrDouble[prRows];
#ifdef DEBUG_MEMORY
   DebugMemAllocate((pvoid)prM, "ZMat::operator*=(const ZMat &)");
#endif
	prCols = rhs.prCols;

	for (int i = 0; i < prRows; i++)
		prM[i] = A.prM[i];		// Calls ZArrDouble Assignment operator method

	return *this;
}

//====================
// TYPE CONVERSION OPERATORS
//====================

//-----------------------------------------------------------------------------
/*ZMat::operator double() const throw (ZException)
{
   if ((prRows != 1) || (prCols != 1))
      throw ZException("ZMat::operator double():Matrix is not 1x1");

   return prM[0][0];
}*/

//=============================================================================
// PROCEDURE:
//    ZMat::operator real_matrix
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Converts a ZMat to a real_matrix. Note that the internal memory is
// allocated using malloc so should be free'd using free. This makes it 
// compatible with the unac function destroy_matrix.
//=============================================================================
ZMat::operator real_matrix() const
{
   real_matrix rm;

	if (!prRows || !prCols)
	{
		rm.A = NULL;
		rm.row_dim = 0;
		rm.col_dim = 0;
		return rm;
	}	// if

   rm.A = (double*)malloc(sizeof(UNAC_DOUBLE)*prRows*prCols);
   if (!rm.A)
      throw (ZException("ZMat::operator real_matrix: Out of memory"));

 	for (int row = 0; row < prRows; row++)
		for (int col = 0; col < prCols; col++)
			rm.A[row*prCols + col] = double(prM[row][col]);

	return rm;
}

//=============================================================================
// PROCEDURE:
//    ZMat::operator ZString
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Returns a string version of this matrix.
//=============================================================================
ZMat::operator ZString() const
{
   ZString  returnStr;
	int	   row;
   int      col;
   int      i;

	if ((prRows == 0) || (prCols == 0))
	{
      returnStr = "[]";
		return returnStr;
	}

	if (display_mode == ZMat::smOneLine)
	{
		returnStr = "[";
		for (row = 0; row < prRows; row++)
		{
			returnStr += (*this)[row][0];
			for (col = 1; col < prCols; col++)
			{
				returnStr += ",";
            returnStr += (*this)[row][col];
			}
			if (row < (prRows-1))
				returnStr += ";";
		}	// for
		returnStr += "]";
	}
	else
	{
		int	         *maxColumnStringSizes;
		int	         maxStringSize = 0;
		ZMatrix<char*> strArr(prRows, prCols);
		char	         tempStr[100];
      char           fmtStr[10];

      maxColumnStringSizes = new int[prCols];
#ifdef DEBUG_MEMORY
      DebugMemAllocate((pvoid)maxColumnStringSizes, "operator ZString()");
#endif
		//memset(maxColumnStringSizes, sizeof(maxColumnStringSizes), 0);
		memset(maxColumnStringSizes, 0, sizeof(maxColumnStringSizes));


		int strWidth;

		for (row = 0; row < prRows; row++)
		{
			for (col = 0; col < prCols; col++)
			{
				strWidth =  sprintf(tempStr, "%g", prM[row][col]);
				strArr[row][col] = new char[strWidth+1];
#ifdef DEBUG_MEMORY
            DebugMemAllocate((pvoid)strArr[row][col], "operator ZString()");
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
		returnStr += "\n _";
		for (col = 0; col < prCols; col++)
			for (i = 0; i < maxStringSize; i++)
				returnStr += " ";
		for (col = 0; col < (prCols-1); col++)
			returnStr += " ";

		returnStr += "_\n";
		for (row = 0; row < prRows; row++)
		{
         char  *strLine;

         strLine = new char[(maxStringSize+2)*prCols + 5];

			if (row != (prRows-1))
				sprintf(strLine, "| ");
			else
				sprintf(strLine, "|_");

         int   lineIndex = 2;
			for (col = 0; col < prCols; col++)
			{
				sprintf(&(strLine[lineIndex]), fmtStr, strArr[row][col]);
            lineIndex += maxStringSize;

				if (col < (prCols-1))
            {
					sprintf(&(strLine[lineIndex]), " ");
               lineIndex++;
            }
			}

			if (row == (prRows-1))
				sprintf(&(strLine[lineIndex]), "_|\n");
			else
				sprintf(&(strLine[lineIndex]), " |\n");

         returnStr += strLine;

         delete [] strLine;
		}

		for (row = 0; row < prRows; row++)
		{
			for (col = 0; col < prCols; col++)
			{
#ifdef DEBUG_MEMORY
            DebugMemDestroy((pvoid)strArr[row][col], "operator<<(ostream&,const ZMat&)");
#endif
				delete strArr[row][col];
			}
		}	//	FOR

#ifdef DEBUG_MEMORY
      DebugMemDestroy((pvoid)maxColumnStringSizes, "operator<<(ostream&,const ZMat&)");
#endif
		delete [] maxColumnStringSizes;
	}

	return returnStr;
}

//=============================================================================
// PROCEDURE:
//    ZMat::operator double
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Returns the element in the first row and column.
//=============================================================================
/*
ZMat::operator double() const
{
   return prM[0][0];
}
*/

//---------------
// Misc Utilities
//-----------------------------------------------------------------------------
void ZMat::Dim(int & RowCount, int & ColCount) const
{
	RowCount = prRows;
	ColCount = prCols;
}
//-----------------------------------------------------------------------------
void ZMat::SetDim(int RowCount, int ColCount)
{
	ZArrDouble	*pTempMat;
	ZArrDouble	tempArr(ColCount);

	if ((prRows == RowCount) && (prCols == ColCount))
		return;


	pTempMat = new ZArrDouble[RowCount];
#ifdef DEBUG_MEMORY
   DebugMemAllocate((pvoid)pTempMat, "ZZMat::SetDim(int,int)");
#endif

	for (int i = 0; i < RowCount; i++)
		pTempMat[i] = tempArr;				// Uses Assignment operator to set size

	for (int row = 0; row < Min(RowCount, prRows); row++)
		for (int col = 0; col < Min(ColCount, prCols); col++)
			pTempMat[row][col] = prM[row][col];

#ifdef DEBUG_MEMORY
   DebugMemDestroy((pvoid)prM, "ZMat::SetDim(int,int");
#endif
	delete [] prM;
	prM = pTempMat;

	prRows = RowCount;
	prCols = ColCount;
}

//-----------------------------------------------------------------------------
int ZMat::RowCount(void) const
{
   return prRows;
}

//-----------------------------------------------------------------------------
int ZMat::ColCount(void) const
{
   return prCols;
}

//-----------------------------------------------------------------------------
void ZMat::ToRealMatrix(real_matrix *pReal_matrix) const
{

   if (  (pReal_matrix->row_dim != prRows) ||
         (pReal_matrix->col_dim != prCols)   )
   {
      char  msg[100];

      sprintf(msg, "ZMat::ToRealMatrix(real_matrix*): Expected %dx%d, got %dx%d",
                     prRows, prCols, pReal_matrix->row_dim, pReal_matrix->col_dim);
		throw ZException(msg, 0);
   }

 	for (int row = 0; row < prRows; row++)
		for (int col = 0; col < prCols; col++)
			pReal_matrix->A[row*prCols + col] = (*this)[row][col];
}

//=============================================================================
// PROCEDURE:
//    ZMat::Square   
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Is this matrix square?
//=============================================================================
bool ZMat::Square(void) const
{
   return ((prRows == prCols) && (prRows != 0));
}

//=============================================================================
// PROCEDURE:
//    ZMat::SubMat   
//-----------------------------------------------------------------------------
// PARAMETERS
//    scDim
//       The matlab style dimensioning string.
//    bOneIndexing
//       If false then indexing is zero-based as in C/C++.
//       If true then indexing is 1-based as in matlab.
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Uses Matlab style indexing to specify and return a submatrix of this
// matrix. If this matrix is a 4 x 6 matrix, and bOneIndexing == true, the
// following are valid strings:
//    "1:3,:"  - (3x6): all columns of rows 1 to 3
//    ":,:"    - (4x6): the entire matrix.
//    "4,5"    - (1x1): the 1x1 matrix at row 4, column 5
//    "2"      - (1x6): the entire 2nd row
//    ":,4"    - (4x1): the entire 4th column
//    ",4"    - (4x1): the entire 4th column
//=============================================================================
ZMat ZMat::SubMat(const ZString & scDim, bool bOneIndexing) const
{
   ZStringVect vectIndices;
   int         nRowStart;
   int         nRowEnd;
   int         nColStart;
   int         nColEnd;
   
   ZString     scSeparator = ",";
   scDim.Split(scSeparator, vectIndices);

   switch (vectIndices.size())
   {
   case 0:
      return (*this);

   case 1:
      if (!ParseRange(vectIndices[0].Trim(), nRowStart, nRowEnd, true, bOneIndexing))
         throw ZException("Illegal string syntax in ZMat::SubMat");

      return SubMat(nRowStart, nRowEnd, 0, this->ColCount()-1);

   case 2:
      if (!ParseRange(vectIndices[0].Trim(), nRowStart, nRowEnd, true, bOneIndexing))
         throw ZException("Illegal string syntax in ZMat::SubMat");

      if (!ParseRange(vectIndices[1].Trim(), nColStart, nColEnd, false, bOneIndexing))
         throw ZException("Illegal string syntax in ZMat::SubMat");
      return SubMat(nRowStart, nRowEnd, nColStart, nColEnd);

   default:
      throw ZException("Illegal string syntax in ZMat::SubMat");
   }  // SWITCH
   
   return (*this);
}

//=============================================================================
// PROCEDURE:
//    ZMat::ParseRowColRange   
//-----------------------------------------------------------------------------
// PARAMETERS
//
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    
//=============================================================================
bool ZMat::ParseRange(  const ZString & scRange, 
                        int & nStartIndex, 
                        int & nEndIndex, 
                        bool bRow,
                        bool bOneIndexing) const
{
   // Look for a colon. Should be zero or one.
   int   nColonIndex = -1;

   for (int i = 0; i < scRange.Length(); i++)
   {
      if (scRange[i] == ':')
      {
         if (nColonIndex > 0)
            return false;
//            throw ZException("ZMat::ParseRowColRange: 2 or more colon's (':') found");
         nColonIndex = i;
      }
   }  // FOR

   if (nColonIndex < 0)
   {
      // No colon found.

      // Is the string in fact zero length
      if (scRange.Length() == 0)
      {
         nStartIndex = 0;
         if (bRow)
            nEndIndex = this->RowCount()-1;
         else
            nEndIndex = this->ColCount()-1;
      }
      else
      {
         // If no colon, we're only expecting an integer
         int   nTemp;
         try
         {
            nTemp = scRange.ToInt();
            if (bOneIndexing)
            {
               nStartIndex = nTemp-1;
               nEndIndex = nTemp-1;
            }
            else
            {
               nStartIndex = nTemp;
               nEndIndex = nTemp;
            }
            return true;
         }
         catch(...)
         {
            return false;
         }
      }  // E:SE
   }  // No colon found
   else
   {
      ZString  scBefore = scRange.SubString(0, nColonIndex);
      ZString  scAfter = scRange.SubString(nColonIndex+1);

      if (scBefore.Trim().Length() == 0)
         nStartIndex = 0;
      else
      {
         if (bOneIndexing)
            nStartIndex = scBefore.ToInt()-1;
         else
            nStartIndex = scBefore.ToInt();
      }  // ELSE

      if (scAfter.Trim().Length() == 0)
      {
         if (bRow)
            nEndIndex = this->RowCount()-1;
         else
            nEndIndex = this->ColCount()-1;
      }
      else
      {
         if (bOneIndexing)
            nEndIndex = scAfter.ToInt()-1;
         else
            nEndIndex = scAfter.ToInt();
      }  // ELSE
   }

   int   nMaxIndex;
   if (bRow)
      nMaxIndex = this->RowCount()-1;
   else
      nMaxIndex = this->ColCount();

   if ((nStartIndex > nEndIndex) || (nStartIndex < 0) || (nEndIndex > nMaxIndex))
      return false;
//      throw ZException("ZMat::ParseRange: Index out of range in call to SubMat");
        
   return true;
}


//=============================================================================
// PROCEDURE:
//    ZMat::SubMat   
//-----------------------------------------------------------------------------
// PARAMETERS
//    Specify rows nRowStart to nRowEnd and nColStart to nColEnd
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Allows when to select a sub matrix of this matrix.
//=============================================================================
ZMat ZMat::SubMat(int nRowStart, int nRowEnd, int nColStart, int nColEnd) const
{
   int   nRowIn;
   int   nRowOut;
   int   nColIn;
   int   nColOut;
   int   nRowCount = nRowEnd - nRowStart + 1;
   int   nColCount = nColEnd - nColStart + 1;

   if ((nRowCount < 1) || (nColCount < 1))
      throw ZException("Illegal index in ZMat::SubMat");

   if ((nRowCount > this->RowCount()) || (nColCount > this->ColCount()))
      throw ZException("Index out of range in ZMat::SubMat");

   ZMat  mReturnMat(nRowCount, nColCount, 0.0);

   for (nRowIn = nRowStart, nRowOut = 0; nRowIn <= nRowEnd; nRowIn++, nRowOut++)
      for (nColIn = nColStart, nColOut = 0; nColIn <= nColEnd; nColIn++, nColOut++)
         mReturnMat[nRowOut][nColOut] = (*this)[nRowIn][nColIn];

   return mReturnMat;
}


//================
// Math Utilities.
//================

//=============================================================================
// PROCEDURE:
//    ZMat::Eigen   
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Finds the eigen values and vectors of this matrix
//=============================================================================
void ZMat::Eigen(ZArr<ZComplex> & EigenValues, ZMat & EigenVectors) throw (ZException)
{
	long			nm, n, matz, ierr;
 	real_matrix	B;
	integer		*ivlA;
	double		*eigValReals, *eigValImags, *eigVects, *fv1A;
	int			row, col;
	int			i;

  	B.A = NULL;
	B   = real_matrix(*this);

	ivlA = new integer[prRows*100];
	fv1A = new double[prCols*100];
#ifdef DEBUG_MEMORY
   DebugMemAllocate((pvoid)ivlA, "ZMat::Eigen(ZArr<ZComplex>&,ZMat&)");
   DebugMemAllocate((pvoid)fv1A, "ZMat::Eigen(ZArr<ZComplex>&,ZMat&)");
#endif

	eigValReals = new double[prCols];
	eigValImags = new double[prCols];
	eigVects		= new double[prRows*100];
#ifdef DEBUG_MEMORY
   DebugMemAllocate((pvoid)eigValReals, "ZMat::Eigen(ZArr<ZComplex>&,ZMat&)");
   DebugMemAllocate((pvoid)eigValImags, "ZMat::Eigen(ZArr<ZComplex>&,ZMat&)");
   DebugMemAllocate((pvoid)eigVects, "ZMat::Eigen(ZArr<ZComplex>&,ZMat&)");
#endif

	n = nm = prCols;
	matz = 1;

   rg_( &nm, &n, B.A, eigValReals, eigValImags, &matz, eigVects, ivlA, fv1A, &ierr);

#ifdef DEBUG_MEMORY
   DebugMemDestroy((pvoid)ivlA, "ZMat::Eigen(ZArr<ZComplex>&,ZMat&)");
   DebugMemDestroy((pvoid)fv1A, "ZMat::Eigen(ZArr<ZComplex>&,ZMat&)");
//   DebugMemDestroy((pvoid)B.A, "ZMat::Eigen(ZArr<ZComplex>&,ZMat&)");
#endif
	delete [] ivlA;
	delete [] fv1A;
//	delete [] B.A;

	if ( ierr != 0 )
	{
      char  msg[100];
		sprintf(msg, "ZMatrix<double>::Eigen: ierr = %d\n",(int)ierr);

#ifdef DEBUG_MEMORY
      DebugMemDestroy((pvoid)eigVects, "ZMat::Eigen(ZArr<ZComplex>&,ZMat&)");
      DebugMemDestroy((pvoid)eigValImags, "ZMat::Eigen(ZArr<ZComplex>&,ZMat&)");
      DebugMemDestroy((pvoid)eigValReals, "ZMat::Eigen(ZArr<ZComplex>&,ZMat&)");
#endif
		delete [] eigVects;
		delete [] eigValImags;
		delete [] eigValReals;
      throw ZException(msg);
	}

	EigenValues.SetLength(n);
	for (i = 0; i < n; i++)
	{
		EigenValues[i].Re(eigValReals[i]);
		EigenValues[i].Im(eigValImags[i]);
	}

	int	rows = nm;
	int	cols = n;
	EigenVectors.SetDim(rows, cols);
	for (row = 0; row < rows; row++)
		for (col = 0; col < cols; col++)
			EigenVectors[row][col] = eigVects[row*cols + col];

#ifdef DEBUG_MEMORY
   DebugMemDestroy((pvoid)eigVects, "ZMat::Eigen(ZArr<ZComplex>&,ZMat&)");
   DebugMemDestroy((pvoid)eigValImags, "ZMat::Eigen(ZArr<ZComplex>&,ZMat&)");
   DebugMemDestroy((pvoid)eigValReals, "ZMat::Eigen(ZArr<ZComplex>&,ZMat&)");
#endif
	delete [] eigVects;
	delete [] eigValImags;
	delete [] eigValReals;
}

//=============================================================================
// PROCEDURE:
//    ZMat::SetCompanion   
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Polynomial in ascending order.
//    e.g. Polynomial[0] is the constant coefficient
//=============================================================================
ZMat & ZMat::SetCompanion(ZPoly & Polynomial) throw(ZException)
{
	ZPoly		poly = Polynomial;
	int		n = Polynomial.Order();
	int		row, col;

	if (n < 1)
	{
   	throw ZException("ZMat::SetCompanion: Error - poly dimension too small");
	}

	//	Normalise the polynomial
   poly.DoNormalise(ZPoly::nmHighest);	// Normalises to higher order coeff == 1

	// Initialise the matrix
	SetDim(n,n);

	//	The first row is the low order coefficients of the polynomial
	for (col = 0; col < n; col++)
		prM[0][col] = -poly[n-col-1];

	// The rest of the matrix is zero accept the lower off diagonal terms
	// which are 1
	for (row = 1; row < n; row++)
   {
		for (col = 0; col < n; col++)
		{
			if (row == (col+1))
				prM[row][col] = 1.0;
			else
				prM[row][col] = 0.0;
		}
   }  // FOR
	return *this;
}

//=============================================================================
// Procedure:
//    ZMat::Transpose   
//-----------------------------------------------------------------------------
// Description:
//    Returns a transposed copy of this.
//=============================================================================
ZMat ZMat::Transpose(void) const
{
   ZMat  tempM(prCols,prRows);
   for (int row = 0; row < prRows; row++)
      for (int col = 0; col < prCols; col++)
         tempM[col][row] = (*this)[row][col];

   return tempM;
}

//=============================================================================
// Procedure:
//    ZMat::DoTranspose   
//-----------------------------------------------------------------------------
// Description:
//    Transposes this.
//=============================================================================
ZMat & ZMat::DoTranspose()
{
   ZMat  tempM(prCols,prRows);
   for (int row = 0; row < prRows; row++)
      for (int col = 0; col < prCols; col++)
         tempM[col][row] = (*this)[row][col];

   (*this) = tempM;
   return (*this);
}

//=============================================================================
// Procedure:
//    ZMat::Trace   
//-----------------------------------------------------------------------------
// Description:
//    Returns the trace of this.
//=============================================================================
double   ZMat::Trace(void) const
{
   double   trace = 0.0;
   int      i;

   if (!Square())
      throw ZException("ZMat::Trace: Matrix not square");

   for (i = 0; i < prRows; i++)
      trace += (*this)[i][i];

   return trace;
}

//=============================================================================
// Procedure:
//    ZMat::Identity   
//-----------------------------------------------------------------------------
// Description:
//    Returns an Identity matrix of the given dimension. This method could be
// static but I couldn't be bothered figuring out how to implement it.
//=============================================================================
ZMat ZMat::Identity(int Dim)
{
   ZMat  m(Dim, Dim, 0.0);
   int   i;

   for (i = 0; i < Dim; i++)
      m[i][i] = 1.0;

   return m;
}

//=============================================================================
// Procedure:
//    ZMat::DoIdentity   
//-----------------------------------------------------------------------------
// Description:
//    Converts this matrix to a square identity matrix of the given dimension.
//=============================================================================
ZMat & ZMat::DoIdentity(int Dim)
{
   ZMat  m(Dim, Dim, 0.0);
   int   i;

   for (i = 0; i < Dim; i++)
      m[i][i] = 1.0;

   (*this) = m;

   return (*this);
}

//---------------
// Friends
//-----------------------------------------------------------------------------

std::ostream & operator<<(std::ostream & os, const ZMat & rhs)
{
	int	row, col, i;

	if ((rhs.prRows == 0) || (rhs.prCols == 0))
	{
			os << "[]";
		return os;
	}

	if (rhs.display_mode == ZMat::smOneLine)
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
		int	         *maxColumnStringSizes;
		int	         maxStringSize = 0;
		ZMatrix<char*> strArr(rhs.prRows, rhs.prCols);
		char	         tempStr[100];
      char           fmtStr[10];

      maxColumnStringSizes = new int[rhs.prCols];
#ifdef DEBUG_MEMORY
      DebugMemAllocate((pvoid)maxColumnStringSizes, "operator<<(ostream&,const ZMat&)");
#endif
		//memset(maxColumnStringSizes, sizeof(maxColumnStringSizes),0);
		memset(maxColumnStringSizes, 0, sizeof(maxColumnStringSizes));


		int strWidth;

		for (row = 0; row < rhs.prRows; row++)
		{
			for (col = 0; col < rhs.prCols; col++)
			{
				strWidth =  sprintf(tempStr, "%g", rhs.prM[row][col]);
				strArr[row][col] = new char[strWidth+1];
#ifdef DEBUG_MEMORY
            DebugMemAllocate((pvoid)strArr[row][col], "operator<<(ostream&,const ZMat&)");
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
            DebugMemDestroy((pvoid)strArr[row][col], "operator<<(ostream&,const ZMat&)");
#endif
				delete strArr[row][col];
			}
		}	//	FOR

#ifdef DEBUG_MEMORY
      DebugMemDestroy((pvoid)maxColumnStringSizes, "operator<<(ostream&,const ZMat&)");
#endif
		delete [] maxColumnStringSizes;
	}

	return os;
}


//-----------------------------------------------------------------------------
real_matrix & operator<<(real_matrix & rm, const ZMat & rhs)
{
   if (rm.row_dim == -1)	// then uninitialised
      rm.A = NULL;
   else if (   (rm.row_dim == rhs.prRows) && 
               (rm.col_dim == rhs.prCols)   )
   {
      for (int row = 0; row < rhs.prRows; row++)
         for (int col = 0; col < rhs.prCols; col++)
            rm.A[row*rhs.prCols + col] = double(rhs.prM[row][col]);
      return rm;
   }
   else if (rm.A)
   {
      free(rm.A);
      rm.A = NULL;
   }

   rm = real_matrix(rhs);

   return rm;
/*	if (!rhs.prRows || !rhs.prCols)
	{
		if (rm.A)
		{
#ifdef DEBUG_MEMORY
         DebugMemDestroy((pvoid)rm.A, "operator<<(real_matrix&,const ZMat&)");
#endif
			delete rm.A;
		}
		rm.A = NULL;
		rm.row_dim = 0;
		rm.col_dim = 0;
		return rm;
	}	// if

	if (rm.row_dim == -1)	// then uninitialised
		rm.A = NULL;

	if (	(rm.row_dim != rhs.prRows) || 
			(rm.col_dim != rhs.prCols)	||
			!rm.A									)
	{
		if (rm.A)
		{
#ifdef DEBUG_MEMORY
         DebugMemDestroy((pvoid)rm.A, "operator<<(real_matrix&,const ZMat&)");
#endif
			delete rm.A;
		}
      rm.A = new double[rhs.prRows*rhs.prCols];
//		rm.A = (double*)malloc(sizeof(UNAC_DOUBLE)*rhs.prRows*rhs.prCols);
		if (rm.A == NULL)
		{
			rm.row_dim = 0;
			rm.col_dim = 0;
			return rm;
		}
		rm.row_dim = rhs.prRows;
		rm.col_dim = rhs.prCols;
	}	// IF

	for (int row = 0; row < rhs.prRows; row++)
		for (int col = 0; col < rhs.prCols; col++)
			rm.A[row*rhs.prCols + col] = double(rhs.prM[row][col]);

	return rm;*/

}

//=============================================================================
// PROCEDURE:
//    friend operator*
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Friend operator overload for the multiplication operation where the left-
// hand side is of type double.
//=============================================================================
ZMat operator*(double dLhs, const ZMat & mRhs)
{
   ZMat  A(mRhs);

   A *= dLhs;

   return A;
}


//=============================================================================
// PROCEDURE:
//    ZMat::Inv
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Returns an inverted copy of *this
//=============================================================================
ZMat ZMat::Inv(void) const
{
   ZMat  returnM;

   returnM = *this;

   returnM.DoInv();

   return returnM;
}

//=============================================================================
// PROCEDURE:
//    ZMat::DoInv
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This procedure finds the inverse of this matrix.
// The technique is to decompose the input matrix and then solve it using the 
// identity matrix as the right hand side. 
// 
// The user should check to see that the matrix is invertible with a check
// on its' determinant before calling this routine.
//=============================================================================
//void INVERT(real_matrix *x1, real_matrix *inverse);
ZMat & ZMat::DoInv(void)
{
   ZArr<int>   indexArr;
   int         row;
   int         col;
   double      row_interchanges = 0.0;
   ZArrDouble  rhs;
   ZMat        tempM;


   tempM = *this;
   tempM.DoLUDecomposition(indexArr, row_interchanges);
   
/*#ifdef LINPACK_LU
   // LINPACK IS COLUMN MAJOR
   
   UNAC_DOUBLE rcond;
   UNAC_DOUBLE *A;
   UNAC_DOUBLE *Atest;
   UNAC_DOUBLE *z;
   UNAC_LONG_INT n;
   UNAC_LONG_INT lda;
   UNAC_INT i,j;
   
   n = x1->row_dim;
   lda = n;
   A = (UNAC_DOUBLE*)malloc( n*n*sizeof(UNAC_DOUBLE) );
   z = (UNAC_DOUBLE*)malloc( n*sizeof(UNAC_DOUBLE) );
	 	
   for ( row=0; row < x1->row_dim; row++)
   {
      for ( col=0; col < x1->row_dim; col++)
      {
         element(A,row,col,n) = GET_ELEMENT(x1,row,col);
      }
   }
   
   print_matrix( A , n );
   
   dgeco_(A, &lda, &n , IndexArr, &rcond,  z );
   
   printf("rcond = %5.20g\n",rcond);
   print_matrix( A , n );
   
   free(A);
   free(z);
   
   for ( row  = 0 ;  row < x1->row_dim; row++)
   {
      for ( col  = 0 ;  col < x1->row_dim; col++)
      {
         SET_ELEMENT(&answer,row,col,element(A,row,col,n));
      }
   }
   
   // L*PACK RECORDS rows and cols from 1
   for ( row  = 0 ;  row < x1->row_dim; row++)
   {
      IndexArr[row]--;
   }
   
#endif
   */

   rhs.SetLength(prRows);   

   for (col = 0; col < prCols; col++)
   {
      //-----------
      // Set up corresponding column of identity matrix.
      //-----------
      for (row = 0; row < prRows; row++)
         rhs[row] = 0.0;
      
      rhs[col] = 1.0;
      
      //--------------
      // Solve for that column using 'answer' from LU_DECOMPOSITION.
      //--------------
      tempM.LUBackSubstitution(rhs, indexArr);

      //--------------
      // Add that column to inverse matrix.
      //--------------
      for (row = 0; row < prRows; row++)
         (*this)[row][col] = rhs[row];
      
   }

   return *this;
   
}

//=============================================================================
// PROCEDURE:
//    ZMat::LUDecomposition
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Returns a LU Decomposed copy of this. Just calls DoLUDecomposition on
// a local copy of this, so see DoLUDecomposition for a fuller explanation.
//=============================================================================
ZMat ZMat::LUDecomposition(ZArr<int> &IndexArr, double & RowInterchanges) const
{
   ZMat  returnMat;

   returnMat = *this;

   returnMat.DoLUDecomposition(IndexArr, RowInterchanges);

   return returnMat;
}

//=============================================================================
// PROCEDURE:
//    ZMat::DoLUDecomposition
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This procedure follows the technique used in Numerical Recipes in C.
// Given this matrix is n x n, this routine returns its LU decomposition of 
// a row wise permutation.
// 
// The resultant matrix is in LU form such that L.U = A. 
// IndexArr[1..n] is an output vector which records the row permutation
// affected by the partial pivoting. 
// 'row_interchanges' is output as +/- 1 depending on whether the number of row
// interchanges was even or odd, respectively. 
//     
// This routine can be used in conjunction with LU_BACK_SUBSTITUTION to 
// solve linear equations or invert a matrix.
// 
// For more information consult Numerical Recipes in C pp39-45.
// 
// Throws an exception if:
//  - not square 
//  - is a Singular Matrix
//=============================================================================
ZMat & ZMat::DoLUDecomposition(ZArr<int> &IndexArr, double & RowInterchanges)
{
   int         i;
   ZMat        answer;
   ZArrDouble  implicit_scaling; // Vector storing implicit scaling information
                                 // for each row of the matrix answer.
   int         row;
   int         col;
   int         row_with_max_element = 0;
   double      largest_element;
   double      dummy_element;
   double      sum;
   double      temp_double;

   if ((prRows != prCols) || (prRows <= 0))
   {
      throw ZException("ZMat::DoLUDecomposition: Matrix must be square and finite size");
   }

   implicit_scaling.SetLength(prRows);
   IndexArr.SetLength(prRows);
   
   //---------------
   // Initialise IndexArr.
   //---------------
   for (i = 0; i < prRows; i++)
      IndexArr[i] = i;
   
   answer = *this;
   
   RowInterchanges = 1.0;   // No interchanges yet */
   
   //-------------
   // Scan all the rows to get implicit scaling information.
   //-------------
   for( row = 0; row < answer.prRows; row++ )
   {
      largest_element = 0.0;
      
      //-------------
      // Find the largest element in this row.
      //-------------
      for (col = 0; col < answer.prCols; col++)
      {
         temp_double = fabs(answer[row][col]);
         if (temp_double > largest_element)
         {
            largest_element = temp_double;
         }
      }
      
      if (largest_element == 0.0)
      {
         //----------
         // Singular matrix
         //----------
         throw ZException("ZMat::DoLUDecomposition: Matrix is Singular");
      }
      
      implicit_scaling[row] = 1.0 / largest_element;
   }
   
   for (col = 0; col < answer.prCols; col++)
   {	  
      //-----------
      // Loop over columns of Crout's method.
      //-----------
      
      //-----------
      // EQUATION 2.3.12 except for  i = j
      //-----------
      for (row = 0; row < col; row++)
      {
         sum = answer[row][col];
         
         for (i = 0; i < row; i++)
         {
            sum -= answer[row][i]*answer[i][col];
         }

         answer[row][col] = sum;
      }  // FOR
      
      //------------
      // Now find the largest pivot element.
      //------------
      
      largest_element = 0.0;
      
      //----------
      // i = j of 2.3.12 and 2.3.13
      //----------
      for (row = col; row < answer.prRows; row++)
      {
         sum = answer[row][col ];
         
         for (i = 0; i < col; i++)
         {
            sum -= answer[row][i]* answer[i][col];
         }
         answer[row][col] = sum;
         
         //--------------
         // Is the figure of merit for the pivot better than the best so far?
         //--------------
         temp_double = implicit_scaling[row] * fabs(sum);
         if ( temp_double >= largest_element)
         {
            largest_element = temp_double;
            row_with_max_element = row;
         }
      }
      
      //------------
      // Is it necessary to interchange rows?
      //------------
      if (col != row_with_max_element)
      {	
         //--------------
         // Interchange necessary.
         //--------------
         for (i = 0; i < answer.prCols; i++)
         {
            dummy_element = answer[row_with_max_element][i];
            answer[row_with_max_element][i] = answer[col][i];
            answer[col][i] = dummy_element;
         }
         
         //--------------
         // Change the parity of RowInterchanges since we have interchanged
         // a row.
         //--------------
         RowInterchanges = -RowInterchanges;
         
         //--------------
         // Interchange the scale factor.
         //--------------
         dummy_element = implicit_scaling[row_with_max_element];
         implicit_scaling[row_with_max_element] = implicit_scaling[col];
         implicit_scaling[col] = dummy_element; // Adam: Added this line. Not
         // sure if it is supposed to be in. Its not in "unac_math.c".
      }
      
      IndexArr[col] = row_with_max_element;
      
      //--------------
      // Prevent divide by zero.
      //--------------
      
      if (answer[col][col] == 0.0)
      {
         answer[col][col] = SIZE_OF_TIM_AND_MARKS_BRAIN; // Maybe too small??
      }
      
      //-------------
      // Divide by pivot.
      //-------------
      if (col != (answer.prCols - 1))
      {
         dummy_element = 1.0 / answer[col][col];
         for (row = (col + 1); row < answer.prCols; row++)
         {
            answer[row][col] = answer[row][col]*dummy_element;
         }
      }  // IF
   }  // FOR

   *this = answer;
   
   return *this;
}

//=============================================================================
// PROCEDURE:
//    ZMat::DoLUBackSubstitution
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This procedure solves the linear set of equations A.X = B, using
// LU decomposition where this is the A matrix. Once the matrix is decomposed,
// the solution set is obtained by forward and back substitution.
//
// To solve a linear set of equations with multiple right hand sides the
// decomposition of this matrix (using LUDecomposition) only has to be
// performed once. This method is then invoked for each right hand side
// column. The right hand side column is passed in as the array 'answer', and 
// the solution set returned in 'answer'.
//
// Index is a vector which contains the row permutation information
// caused by the partial pivoting which occurs in LU_DECOMPOSITION.
//	For more information consult Numerical Recipes in C pp44-45.
//=============================================================================
void ZMat::LUBackSubstitution(ZArrDouble & Answer, const ZArr<int> & Index) const
{
   int     row;
   int     col;
   int     permutation;
   int     ii = -1;
   double  sum;
   
   //---------------
   // Do the forward substitution taking account to unscramble
   // the permutation of the decomposed matrix as given by Index.
   //---------------
   for (row = 0; row < prRows; row++ )
   {
      permutation         = Index[row];
      sum                 = Answer[permutation];
      Answer[permutation] = Answer[row];
      
      //-------------
      // Don't start multiplying until we get to the first 
      // non zero element.
      //-------------
      if (ii != -1)
      {
         for (col = ii; col <= (row - 1); col++)
            sum -= (*this)[row][col]*Answer[col];
      }
      else if (sum != 0.0 )
         ii = row;

      Answer[row] = sum;
   }
   
   //-----------
   // Now back substitute.
   //-----------
   for (row = prRows - 1; row >= 0; row--)
   {
      sum = Answer[row];

      for (col = row + 1; col < prRows; col++)
      {
         sum -= (*this)[row][col]*Answer[col];
      }
      Answer[row] = sum / (*this)[row][row];
   }
}

//=============================================================================
// PROCEDURE:
//    ZMat::Det
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This procedure returns the determinant of the passed matrix. The technique
// is to decompose the input matrix to LU form, then the determinate is 
// simply the product of all the diagonal elements.
//
//    Will throw an exception if the matrix is not square.
//=============================================================================
double ZMat::Det(void) const
{
   double      determinant = 0.0;
   ZMat        LU_form;
   ZArr<int>   indexs;
   int         row;
   
   LU_form = *this;

   if (!Square())
      throw ZException("ZMat::Det: Matrix not square");
   
   //------------
   // Decompose the input matrix. Will throw an exception if the matrix is
   // singular.
   //------------
   try
   {
      LU_form.DoLUDecomposition(indexs, determinant);
   }
   catch(ZException &)
   {
      return 0;
   }

   //-----------
   // Form the determinant.
   //-----------
   for (row = 0; row < prRows; row++)
      determinant *= LU_form[row][row];
         
   return determinant;
}

//=============================================================================
// PROCEDURE:
//    ZMat::Exp
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Returns a matrix exponential copy *this.
//
// Uses a Numerical integration technique outlined in Middleton & Goodwin p38.
// The Nth order Runge-Kutter algorithm is used which is:
//    exp(A) = I + A + A^2/2! + A^3/3! + A^4/4! + ... + A^n/n!
//=============================================================================
ZMat ZMat::Exp(int RungeOrder) const
{
   ZMat  tempM = *this;

   tempM.DoExp(RungeOrder);

   return tempM;
}

//=============================================================================
// PROCEDURE:
//    ZMat::DoExp
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Converts *this to exp(*this).
//
// Uses a Numerical integration technique outlined in Middleton & Goodwin p38.
// The Nth order Runge-Kutter algorithm is used which is:
//    exp(A) = I + A + A^2/2! + A^3/3! + A^4/4! + ... + A^n/n!
//=============================================================================
ZMat & ZMat::DoExp(int RungeOrder)
{
   ZMat        eye;
   ZMat        tempM;
   ZMat        powMat;
   ZMat        A_on_n;
   double      factorial;


   if (!Square())
      throw ZException("ZMat::DoExp: Matrix not square");

   eye = ZMat::Identity(prRows);  // = I

   tempM = eye;
   powMat = eye;
   factorial = 1;

   for (int i = 1; i <= RungeOrder; i++)
   {
      factorial *= double(i);    // i=1: fact. = 1       i=2: fact. = 2;
      powMat *= (*this);         // i=1: powMat = A      i=2: powMat = A^2
      tempM += powMat/factorial; // i=1: tempM = I + A   i=2: tempM = I + A + A^2/2
   }

   (*this) = tempM;

   return (*this);
}

//=============================================================================
// PROCEDURE:
//    ZMat::Pow
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Returns a copy of *this raised to the power Power
//=============================================================================
ZMat ZMat::Pow(int Power) const
{
   ZMat  tempM = *this;

   tempM.DoPow(Power);

   return tempM;
}

//=============================================================================
// PROCEDURE:
//    ZMat::DoPow
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Raises *this to the power Power
//=============================================================================
ZMat & ZMat::DoPow(int Power)
{
   int   i;
   bool  invert = false;
   ZMat  oldThis;

   if (!Square())
      throw ZException("ZMat::DoPow: Matrix not square");

   if (Power < 0)
   {
      invert = true;
   }
   else if (Power == 0)
   {
      *this = ZMat::Identity(prRows);
   }

   oldThis = *this;

   i = Power;
   while (i > 1)
   {
      *this *= oldThis;
      i--;
   }

   if (invert)
      this->DoInv();

   return *this;
}

//=============================================================================
// PROCEDURE
//    ZMat::CopyTo(real_matrix  &rmDest, bool bReallocate)
//-----------------------------------------------------------------------------
// DESCRIPTION
//    Copies the contents of the real matrix passed in. If bReallocate is
// true and there is difference in the two dimensions, the memory is reallocated
// using free and malloc. If bReallocated is false and there is a difference,
// this procedure throws an exception. The default is false.
//    Needless to say, rmDest needs to be initialised.
//=============================================================================
void ZMat::CopyTo(real_matrix  &rmDest, bool bReallocate)
{
   if (  (rmDest.row_dim != this->prRows) ||
         (rmDest.col_dim != this->prCols)    )
   {
      if (bReallocate)
      {
         free(rmDest.A);
         rmDest.A = (UNAC_DOUBLE*) malloc(sizeof(UNAC_DOUBLE)*this->prRows*this->prCols);

         rmDest.row_dim = this->prRows;
         rmDest.col_dim = this->prCols;
      }
      else
         throw ZException("ZMat::CopyTo(real_matrix&,false): real_matrix dimensions are incompatible",0);
   }  // IF

   for (int row = 0; row < prRows; row++)
      for (int col = 0; col < prCols; col++)
         rmDest.A[row*prCols + col] = (*this)[row][col];
}

//=============================================================================
// PROCEDURE
//    ZMat::Sum(const ZMat & mIn)
//-----------------------------------------------------------------------------
// DESCRIPTION
//    Returns the sum of the elements of the matrix.
//=============================================================================
double ZMat::Sum(const ZMat & mIn)
{
   double   dResult = 0;
   int   nRowCount = mIn.RowCount();
   int   nColCount = mIn.ColCount();
   for (int row = 0; row < nRowCount; row++)
      for (int col = 0; col < nColCount; col++)
         dResult += mIn[row][col];
   return dResult;
}
