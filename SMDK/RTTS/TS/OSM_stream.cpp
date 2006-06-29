//-- OSM_stream.cpp ---------------------------------------------------------
//
// Classes Implemented
//
//   OSM_Stream
//   OSM_cSizeDist
//
// History:
//
//   31/01/2000  First version.  
//    David T.
//
//   08/02/2000  Class name changes to avoid confusion between typing
//    David T.   of OSM objects (for mutual compatibility) and Stream:
//               - OSM_Type (was StreamType) manages material properties
//                 of multiple OSM objects in the current analysis
//               - OSM_HasType (was HasStreamType) brands an object
//                 with a reference to its relevant OSM_Type)
//    
//   09/02/2000  - Split OSM_stream.cpp from OSM.cpp to separate the
//    David T.     OSM Stream classes from the OSM basic type classes.
//               - Changed class CumulativeSizeDist to OSM_cSizeDist
//                 for naming consistency with other OSM classes.


//-- External Definitions ---------------------------------------------------

#include "stdafx.h"
//#include <iostream.h>
//#include <iomanip.h>
#include <stdlib.h>
#include <math.h>

#include "OSM.h"


//---------------------------------------------------------------------------
//
// OSM_Stream IMPLEMENTATION
//
//   Holds a Matrix of component tph flows representing a process
//   flow stream. Rows of the Matrix are size distributions of
//   separate ore-components. Uses an inherited HasStreamType to
//   specify the number of size fractions and components to be
//   defined in the OSM_Stream. The data area of the OSM_Matrix 
//   can be linked to a remote buffer area containing a matrix 
//   in row major form.
//
// Members Defined:
//
//    OSM_Stream( )
//    ~OSM_Stream( )
//
//    void clear( )
//    OSM_Stream& operator=( OSM_Vector& V )
//
//    ostream& operator<<( ostream& out, OSM_Stream& S )  (non-member)
//

//-- Create OSM_Stream, specifying its type and its component matrix --------

OSM_Stream::OSM_Stream( OSM_Type& sType, double* compMatrix)
  : OSM_HasType( sType )
{
	dimension( nComp(), nSize(), compMatrix );
}

//-- Free OSM_Stream resources ----------------------------------------------

OSM_Stream::~OSM_Stream( )
{
    /* void */
}

//-- Fill OSM_Stream component matrix with 0 elements -----------------------

void OSM_Stream::clear( )
{
    // Fill matrix elements with 0.0
    OSM_Matrix::operator=( 0.0 );
}

//-- Assign stream fractional flows from a Vector ---------------------------

OSM_Stream& OSM_Stream::operator=( OSM_Vector& V1 )
{
    // Use Vector assignment
    OSM_Vector::operator=( V1 );

    return (*this);
}

//-- Output text representation of an OSM_Stream to ostream& ----------------

ostream& operator<<( ostream& out, OSM_Stream& S )
{
#ifdef NEVER
	out.setf( ios::fixed, ios::floatfield );
    out.setf( ios::showpoint );

    OSM_cSizeDist cDist( S );

    for( int i=0; i<S.nSize(); i++ )
    {
        // output size index #
        out << setw(3) << i+1;

        // output size sieve size
        out << setw(10) << setprecision(3) << S.sieve()[i];

        // output tph in fraction by ore component
        for( int j=0; j<S.nComp(); j++ )
            out << setw(10) << setprecision(3) << S[j][i];

        // output cumulative size distribution
        out << setw(10) << setprecision(3) << cDist[i];
        out << endl;
    }

    // output total tph in each component
    out << "    TPH TOTAL";
    for( int j=0; j<S.nComp(); j++ )
        out << setw(10) << setprecision(3) << S[j].sum();

    // output total tph in the Stream
    out << setw(10) << setprecision(3) << S.sum();
    out << endl;
#endif
    return out;
}


//---------------------------------------------------------------------------
//
// OSM_cSizeDIst IMPLEMENTATION
//
//   Manages a cumulative %passing size distribution
//   calculated from a specified OSM_Stream. The separate
//   ore-components of the Stream are added together
//   to make a single size distribution. The finest
//   point on the distribution is redundant to match
//   the sieve sizes used in StreamType, ie zero percent
//   passes zero size.
//
//   Provides interpolation methods between points on
//   the distribution using linear Rossin-Rammler 
//   interpolation.
// 
// Members Defined:
//
//   OSM_cSizeDist( OSM_Stream& )
//   ~OSM_cSizeDist( )
//
//   double sizePassing( double )
//   double percentPassing( double )
//
//   void setRR( double, double, double, double )
//   double percentRR( double )
//   double sizeRR( double )
//   void filterS( double& )
//   void filterP( double& )
//

//-- Build cumulative size distribution from an OSM_Stream ------------------

OSM_cSizeDist::OSM_cSizeDist( OSM_Stream& S )

  : OSM_HasType( S )        // same OSM type as Stream
{
    int size, comp;

    // Initialize cumulative passing Vector
    cDist.dimension( nSize() );

    // Calculate tons per hour in size distribution
    tph = S.sum();

    // Add all component size distributions of S into cDist[]
    cDist = 0;
    for( comp=0; comp<nComp(); comp++ )
        cDist.loadSum( cDist, S[comp] );

    // Convert to cumulative passing distribution (tph basis)

    for( size=0; size<nSize()-1; size++ )
        cDist[size] = cDist[size+1];

    cDist[nSize()-1] = 0;
    for( size=nSize()-2; size>=0; size-- )
        cDist[size] += cDist[size+1];

    // Scale cumulative distribution by tph (percent basis)
    if( tph > 0 )
    {
        cDist.loadProduct( cDist, (100/tph) );
    }
    else
    {
        cDist = 0;
    }
}

//-- Deallocate CumulativeSizeDist ------------------------------------------

OSM_cSizeDist::~OSM_cSizeDist( )
{
    /* void */
}

//-- Determine the size at which a given percentage passes ------------------

double OSM_cSizeDist::sizePassing( double percent )
{
    double retVal = 0.0;
    int i, N = nSize();
    OSM_Vector& sVec = sieve();

    for( i=0; i<(N-1); i++ )
    {
        if( cDist[i] == percent )
        {
            retVal = sVec[i];
            break;
        }
        else if( cDist[i]>percent && cDist[i+1]<percent )
        {
            setRR( sVec[i], sVec[i+1], cDist[i], cDist[i+1] );
            retVal = sizeRR( percent );
            break;
        }
    }
    return retVal;
}

//-- Determine the percentage passing a given size --------------------------

double OSM_cSizeDist::percentPassing( double size )
{
    double retVal = 0.0;
    int i, N = nSize();
    OSM_Vector& sVec = sieve();

    if (N>0 && size>=sVec[0])
      retVal = cDist[0]; //assume data for largest size

    for( i=0; i<(N-1); i++ )
    {
        if( sVec[i] == size )
        {
            retVal = cDist[i];
            break;
        }
        else if( sVec[i] > size && sVec[i+1] < size )
        {
            setRR( sVec[i], sVec[i+1], cDist[i], cDist[i+1] );
            retVal = percentRR( size );
            break;
        }
    }
    return retVal;
}

//-- Set line-segment in RR-space -------------------------------------------

void OSM_cSizeDist::setRR( double s1, double s2, double p1, double p2 )
{
    filterS( s1 );
    filterS( s2 );
    filterP( p1 );
    filterP( p2 );

    s0 = s1;
    x0 = log(s1);
    dx = log(s2) - x0;

    p0 = p1;
    y0 = log( -log(p1/100) );
    dy = log( -log(p2/100) ) - y0;
}

//-- Lookup percent passing a specified size on RR line segment -------------

double OSM_cSizeDist::percentRR( double size )
{
    double x, y, retVal = 0;

    if( dx==0 )
    {
        retVal = p0;
    }
    else
    {
        filterS( size );
        x = log( size );
        y = y0 + dy * (x-x0)/dx;
        retVal = 100 * exp( -exp(y) );
    }
    return retVal;
}

//-- Lookup size passing a specifed percentage on RR line segment -----------

double OSM_cSizeDist::sizeRR( double percent )
{
    double x, y, retVal = 0;

    if( dy==0 )
    {
        retVal = s0;
    }
    else
    {
        filterP( percent );
        y = log( -log(percent/100) );
        x = x0 + dx * (y-y0)/dy;
        retVal = exp(x);
    }
    return retVal;
}

//-- Filter a size value on open interval (0,inf) ---------------------------

void OSM_cSizeDist::filterS( double& S )
{
    const double MIN_SIZE = 1e-8;
    if( S <= MIN_SIZE ) S = MIN_SIZE;
}

//-- Filter a percent value on open interval (0,100) ------------------------

void OSM_cSizeDist::filterP( double& P )
{
    const double MIN_PERCENT = 1e-8;
    const double MAX_PERCENT = 100 - MIN_PERCENT;

    if( P <= MIN_PERCENT ) P = MIN_PERCENT;
    if( P >= MAX_PERCENT ) P = MAX_PERCENT;
}


//-- end OSM_stream.cpp -----------------------------------------------------

