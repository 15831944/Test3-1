//-- OSM_Math.cpp -----------------------------------------------------------
//
// Classes Implemented
//
//  OSM_Vector
//  OSM_Matrix
//
// History:
//
//  31/01/2000  First version.  
//    David T.
//
//  08/02/2000  Added OSM_Vector::load( OSM_Vector& ) to avoid
//    David T.  confusion with default operator=( ), especially
//              in derived class types (eg OSM_Stream).
//

//-- External Definitions ---------------------------------------------------

#include "stdafx.h"
#include <stdlib.h>
#include <math.h>

#include "OSM_math.h"


//---------------------------------------------------------------------------
//
// OSM_Vector IMPLEMENTATION
//
//   Represents a Vector of doubles. Implementation can either
//   use internal storage (allocated with clean-up) or can refer
//   to an array of doubles owned by another object/process.
//
// Members Defined:
//
//   OSM_Vector( int, double* )
//   OSM_Vector( Vector& )
//   ~OSM_Vector( )
//   void deallocate( )
//
//   int dimension( int, double* )
//   OSM_Vector& operator=( double )
//   OSM_Vector& operator=( const OSM_Vector& )
//   OSM_Vector& refersTo( OSM_Vector& )
//
//   OSM_Vector& add( const OSM_Vector& )
//   OSM_Vector& loadProduct( const OSM_Vector&, const OSM_Vector& )
//   OSM_Vector& loadProduct( const OSM_Vector&, const double& )
//   OSM_Vector& loadSum( const OSM_Vector&, const OSM_Vector& v2 )
//   OSM_Vector& loadSumWithCompare( const OSM_Vector&, const OSM_Vector& )
//   OSM_Vector& loadSubtraction( const OSM_Vector&, const OSM_Vector& )
//   double sum( );
//
//   double dot( const OSM_Vector&, const OSMVector& )  (non-member)
//

//-- Default Vector - optionally specify length and buffer ------------------

OSM_Vector::OSM_Vector( int initLength, double* buffer )
{
    length = 0;
    vecBuf = NULL;
    memAlloc = 0;

    if( initLength > 0 )
    {
        dimension( initLength, buffer );
    }
}

//-- Make a reference copy of another Vector ---------------------------------

OSM_Vector::OSM_Vector( OSM_Vector& v1 )
{
    length = v1.length;   // Same length as v1
    vecBuf = v1.vecBuf;   // Same buffer as v1
    memAlloc = 0;         // not responsible for vecBuf memory
}

//-- Free Vector resources ---------------------------------------------------

OSM_Vector::~OSM_Vector( )
{
    deallocate( );
}

//-- Restore Vector to default, empty, unallocated state ---------------------

void OSM_Vector::deallocate( )
{
    if( memAlloc && vecBuf!=NULL )
    {
        delete[] vecBuf;
    }

    length = 0;
    vecBuf = NULL;
    memAlloc = 0;
}

//-- Dimension Vector length, and specify memory to use (optional) -----------

int OSM_Vector::dimension( int initLength, double* buffer )
{
    int dimOk = 1;                         // status of call, assume suceeds

    if( initLength<0 )                     // ensure non-negative dimension
        initLength = 0;

    if( length != initLength )             // different to current size ?
    {
        deallocate( );                     // remove current implementation

        length = initLength;               // construct new implementation
        vecBuf = buffer;
        memAlloc = 0;

        if( vecBuf==NULL && length>0 )     // need to allocate vecBuf[] ?
        {
            vecBuf = new double[length];   // allocate memory

            if( vecBuf != NULL )           // ensure Ok
            {
                memAlloc = 1;              // memory allocated
                operator=( 0.0 );          // initialize contents
            }
            else
            {
                deallocate( );             // cannot implement
                dimOk = 0;                 //   the Vector
            }
        }
    }
    return dimOk;
}

//-- Assign Scalar to all vector elements -----------------------------------

OSM_Vector& OSM_Vector::operator=( double x )
{
    double* ptr1 = vecBuf;
    int  counter = length;

    while( counter-- )
        *ptr1++ = x;

    return (*this);
}

//-- Copy element values from another vector --------------------------------

OSM_Vector& OSM_Vector::operator=( const OSM_Vector& v1 )
{
    if( sameSize(v1) )
    {
        double* ptr1 = vecBuf;
        double* ptr2 = v1.vecBuf;
        int  counter = length;

        while( counter-- )
            *ptr1++ = *ptr2++;
    }
    return (*this);
}

//-- Refer to the implementation of another Vector --------------------------

OSM_Vector& OSM_Vector::refersTo( OSM_Vector& v1 )
{
    deallocate( );              // remove current implementation
    
    length = v1.length;         // use dimension of source vector
    vecBuf = v1.vecBuf;         // use buffer of source vector
    memAlloc = 0;               // not responsible for vecBuf memory

    return (*this);
}

//-- Add another Vector to this one -----------------------------------------

OSM_Vector& OSM_Vector::add( const OSM_Vector& v1 )
{
    if( sameSize(v1) )
    {
        double* ptr1 = vecBuf;
        double* ptr2 = v1.vecBuf;
        int  counter = length;

        while( counter-- )
            *ptr1++ += *ptr2++;
    }
    return (*this);
}

//-- Obtain result of member-wise multiplication of two vectors --------------

OSM_Vector&
  OSM_Vector::loadProduct( const OSM_Vector& v1, const OSM_Vector& v2 )
{
    if( sameSize(v1,v2) )
    {
        double* ptr1 = vecBuf;
        double* ptr2 = v1.vecBuf;
        double* ptr3 = v2.vecBuf;
        int  counter = length;

        while( counter-- )
            *ptr1++ = *ptr2++ * *ptr3++;
    }
    return (*this);
}

//-- Obtain result of multiplying a vector by a scalar -----------------------

OSM_Vector& OSM_Vector::loadProduct( const OSM_Vector& v1, const double& x )
{
    if( sameSize(v1) )
    {
        double* ptr1 = vecBuf;
        double* ptr2 = v1.vecBuf;
        int counter = count();

        while( counter-- )
            *ptr1++ = *ptr2++ * x;
    }
    return (*this);
}

//-- Add the result of multiplying a vector by a scalar ----------------------

OSM_Vector& OSM_Vector::addProduct( const OSM_Vector& v1, const double& x )
{
    if( sameSize(v1) )
    {
        double* ptr1 = vecBuf;
        double* ptr2 = v1.vecBuf;
        int counter = count();

        while( counter-- )
            *ptr1++ += *ptr2++ * x;
    }
    return (*this);
}

//-- Obtain result of adding two vectors -------------------------------------

OSM_Vector& OSM_Vector::loadSum( const OSM_Vector& v1, const OSM_Vector& v2 )
{
    if( sameSize(v1,v2) )
    {
        double* ptr1 = vecBuf;
        double* ptr2 = v1.vecBuf;
        double* ptr3 = v2.vecBuf;
        int  counter = count();

        while( counter-- )
            *ptr1++ = *ptr2++ + *ptr3++;
    }
    return (*this);
}

//-- Obtain result of adding two vectors, compare with existing value --------

double OSM_Vector::loadSumWithCompare(
    const OSM_Vector& v1, const OSM_Vector& v2
    )
{
    double maxDiff = 0;         // Maximum difference on compare

    if( sameSize(v1,v2) )
    {
        double temp = 0;
        double diff = 0;
        double* ptr1 = vecBuf;
        double* ptr2 = v1.vecBuf;
        double* ptr3 = v2.vecBuf;
        int  counter = count();

        while( counter-- )
        {
            temp = *ptr2++ + *ptr3++;
            diff = fabs(temp-*ptr1);
            *ptr1++ = temp;

            if( diff > maxDiff )
                maxDiff = diff;
        }
    }
    return maxDiff;
}

//-- Obtain result of subtracting two vectors --------------------------------

OSM_Vector& OSM_Vector::loadSubtraction(
    const OSM_Vector& v1, const OSM_Vector& v2
    )
{
    if( sameSize(v1,v2) )
    {
        double* ptr1 = vecBuf;
        double* ptr2 = v1.vecBuf;
        double* ptr3 = v2.vecBuf;
        int  counter = count();

        while( counter-- )
            *ptr1++ = *ptr2++ - *ptr3++;
    }
    return (*this);
}

//-- Obtain the sum of Vector elements ---------------------------------------

double OSM_Vector::sum( ) const
{
    double result = 0;
    double*  ptr1 = vecBuf;
    int   counter = count();

    while( counter-- )
        result += *ptr1++;

    return result;
}

//-- Obtain the dot product of two vectors -----------------------------------

double dot( const OSM_Vector& v1, const OSM_Vector& v2 )
{
    double result = 0;

    if( v1.count() == v2.count() )
    {
        double* ptr1 = v1.vecBuf;
        double* ptr2 = v2.vecBuf;
        int  counter = v1.count();

        while( counter-- )
            result += *ptr1++ * *ptr2++;
    }
    return result;
}


//---------------------------------------------------------------------------
//
// OSM_Matrix IMPLEMENTATION
//
//   Represents a 2D array of doubles. Implemented as OSM_Vector
//   of double by inheritance, with an array of OSM_Vector to
//   to access individual rows of the inherited vector.
//
// Members Defined:
//
//   OSM_Matrix( )
//   OSM_Matrix( int, int, double* )
//   ~OSM_Matrix( )
//
//   void deallocate( )
//   int dimension( int, int double* )
//
//   OSM_Matrix& operator=( const double& x )
//   OSM_Matrix& operator=( const OSM_Vector V1 )
//   OSM_Matrix& refersTo( OSM_Matrix& M1 )
//

//-- Default empty Matrix ---------------------------------------------------

OSM_Matrix::OSM_Matrix( )
{
    // build default implementation

    numRows = 0;
    numCols = 0;
    rowVec  = NULL;
}

//-- Matrix of specified size and buffer ------------------------------------

OSM_Matrix::OSM_Matrix( int initRows, int initCols, double* buffer )
{
    // build default implementation

    numRows = 0;
    numCols = 0;
    rowVec  = NULL;

    // dimension to specified size

    dimension( initRows, initCols, buffer );
}

//-- Release Matrix ---------------------------------------------------------

OSM_Matrix::~OSM_Matrix( )
{
    deallocate( );
}

//-- Restore default Matrix -------------------------------------------------

void OSM_Matrix::deallocate( )
{
    if( rowVec != NULL )
    {
        delete[] rowVec;               // release array of row vectors
        rowVec = NULL;                 // flag rowVec[] as unallocated
    }
    OSM_Vector::deallocate();          // release any Vector resources
}

//-- Dimension matrix, optionally specify data location ---------------------

int OSM_Matrix::dimension( int nRows, int nCols, double* buffer )
{
    if( nRows<0 ) nRows = 0;           // ensure non-negative
    if( nCols<0 ) nCols = 0;           //   dimensions

    int elemCount = nRows * nCols;     // elements stored in Matrix

    deallocate( );                     // release Matrix implemenetation

    if( elemCount > 0 )
    {
        // create Vector:: storage for matrix elements
        OSM_Vector::dimension( elemCount, buffer );

        // set buffer to address of first element
        buffer = (double*)(*this);

        numRows = nRows;
        numCols = nCols;

         // need to create row Vectors ?
        if( nRows > 0 )
        {
            rowVec = new OSM_Vector[nRows];
            if( rowVec != NULL )
            {
                for( int row=0; row<nRows; row++ )
                {
                    // dimension each row Vector
                    rowVec[row].dimension( nCols, buffer );

                    // locate start of next row
                    buffer += nCols;
                }
            }
            else
            {
                // fail, zero implementation
                deallocate( );
            }
        }
    }

    // indicate success of dimension
    return( rows()==nRows && columns()==nCols );
}

//-- Assign scalar to matrix elements ---------------------------------------

OSM_Matrix& OSM_Matrix::operator=( const double& x )
{
    OSM_Vector::operator=( x );
    return (*this);
}

//-- Assign OSM_Matrix values from a OSM_Vector -----------------------------

OSM_Matrix& OSM_Matrix::operator=( const OSM_Vector V1 )
{
    OSM_Vector::operator=( V1 );
    return (*this);
}

//-- Use the implementation of another Matrix -------------------------------

OSM_Matrix& OSM_Matrix::refersTo( OSM_Matrix& M1 )
{
    deallocate( );
    dimension( M1.rows(), M1.columns(), (double*) M1 );
    return (*this);
}


//-- end OSM_Math.cpp -------------------------------------------------------

