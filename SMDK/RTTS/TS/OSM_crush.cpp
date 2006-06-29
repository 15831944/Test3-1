//-- OSM_Crush.cpp ----------------------------------------------------------
//
// Classes Implemented:
//
//  OSM_Crusher
//  OSM_Model400
//
// History:
//
//  31/01/2000  First version.  
//    David T.
//


//-- External Definitions ---------------------------------------------------

#include "stdafx.h"
#include <stdlib.h>
#include <math.h>

#include "OSM.h"


//---------------------------------------------------------------------------
//
// OSM_Crusher IMPLEMENTATION
//
//   Represents a generic crusher model for OSM_Stream Streamss.
//   Manages connection to a user supplied feed stream, and ownership
//   of a product stream with the OSM_Crusher.
//
// Members Defined:
//
//   OSM_Crusher( OSM_Type& )
//   ~OSM_Crusher( )
//
//   int setFeed( OSM_Stream& )

//-- Create according to a OSM_Type template --------------------------------

OSM_Crusher::OSM_Crusher( OSM_Type& sType )

    : OSM_HasType(sType),       // dimension this model
      productStream(sType)      // dimension our product stream
{
    // Not connected to a feed
    feedStream = NULL;
}

//-- Deallocate -------------------------------------------------------------

OSM_Crusher::~OSM_Crusher( )
{
    /* void */
}

//-- Connect to a user supplied feed Stream ---------------------------------

int OSM_Crusher::setFeed( OSM_Stream& modelFeed )
{
    int setOk = 0;                // Default fail

    if( isSameType(modelFeed) )   // Check StreamTypes
    {
        feedStream = &modelFeed;  // Save pointer to modelFeed
        setOk = 1;                // Succeed
    }
    return setOk;
}


//---------------------------------------------------------------------------
//
// OSM_Model400 IMPLEMENTATION
//
//   Crusher model similar to JKSimmet Model 400.
//
// Members Defined:
//
//   OSM_Model400( StreamType& )
//   ~OSM_Model400( )
//   int calculate( )
//   void calculateC( )
//   void calculateA( int )
//   void makeDaughters( OSM_Vector&, OSM_Vector& )
//

//-- Build default JKSimmet Model400 ----------------------------------------

OSM_Model400::OSM_Model400( OSM_Type& sType ) : OSM_Crusher( sType )
{
    // Set defaults

    m0 =  1.4936;                 // Appearance function
    m1 =  0.9543;                 //   regression parameters
    m2 = -0.3856;
    m3 = 23.7200;

    k1 = 0;                       // Machine
    k2 = 50;                      //   parameters
    k3 = 2.3;

    T10 = 10;                     // Applied energy - rock hardness relation

    tolerance = 1e-9;             // Convergence tolerance
    maxIteration = 100;           // Iteration limit

    // Dimension classification Vector
    C.dimension( nSize() );

    // Dimension appearance Matrix
    A.dimension( nSize(), nSize() );    
}

//-- Deallocate Model400 ----------------------------------------------------

OSM_Model400::~OSM_Model400( )
{
    /* void */
}

//-- Calculate Model400 -----------------------------------------------------

int OSM_Model400::calculate( )
{
    // Return value, assume successful
    int retVal = 0;

    // Vectors for internal component streams
    OSM_Vector x( nSize() );       // net crusher contents
    OSM_Vector y( nSize() );       // material classified for breakage
    OSM_Vector z( nSize() );       // breakage products of y

    // Calculate classification function: C[]
    calculateC( );

    // Create contents matrix if required
    if( contents.rows()!=nComp() || contents.columns()!=nSize() )
    {
        contents.dimension( nComp(), nSize() );
    }

    //-- Main Loop - loop over components in the feed -----------------------

    for( int comp=0; comp<nComp(); comp++ )
    {
        // Aliases to component vectors
        OSM_Vector& feedC = feed()[comp];
        OSM_Vector& prodC = product()[comp];
        OSM_Vector& x     = contents[comp];

        if( feedC.sum() > 0 )                   // Component present ?
        {
            calculateA( comp );                 // Make appearance function
            double misconvergence = 1e20;       // Force misconvergence
            long iteration = 0;                 // No iterations yet

            //-- Iteration Loop - until converged or maximum iterations -----

            while( misconvergence>tolerance && iteration<maxIteration )
            {
                // classify material for breakage y = C(x)
                y.loadProduct( C, x );

                // obtain breakage products z = A(y)
                makeDaughters( y, z );

                // obtain next iteration of recycle: x = feed + z
                misconvergence = x.loadSumWithCompare( feedC, z );

                // a successful iteration (hopefully)
                iteration++;
            }
            // product component by balance
            prodC.loadSubtraction( x, y );
        }
        else
        {
            // product component is 0
            prodC = 0;
        }
    }
    // indicate success
    return retVal;
}

//-- Calculate classification function --------------------------------------

void OSM_Model400::calculateC( )
{
    // Calculate C[] for each size
    for( int size=0; size<nSize(); size++ )
    {
        double nomSize = nominal()[size];
        if( size==nSize()-1 ) // Ugly patch
            nomSize = 0;

        if( nomSize < k1 )
        {
            C[size] = 0.0;
        }
        else if( nomSize < k2 )
        {
            C[size] = 1.0 - pow( (k2-nomSize)/(k2-k1), k3 );
        }
        else
        {
            C[size] = 1.0;
        }
    }
}

//-- Calculate appearance function ------------------------------------------

void OSM_Model400::calculateA( int comp )
{
    int i = 0;
    int j = 0;

    // Set A[][] to identity matrix
    A = 0;
    for( i=0; i<nSize(); i++ )
        A[i][i] = 1;

    double logT10 = log( T10[comp] );
    double t1Size = 0;
    double tSize  = 0;
    double yPass  = 0;

    // 1. Generate square upper triangular matrix of appearance
    //    functions. ie. the complete appearance function table
    //    any size to any other size

    for( i=0; i<nSize()-1; i++ )
    {
        // t1 marker size basis for this row
        t1Size = nominal()[i];

        // from 2nd coarsest size (i+1) to 2nd finest (nSize-1)
        for( j=(i+1); j<(nSize()-1); j++ )
        {
            tSize = t1Size / nominal()[j];

            yPass = 0.01 * exp(  m0 + m1 * logT10
                                    - m2 * log(tSize)
                                    + m3 / (tSize*(T10[comp]+10)) ) / tSize;
            if( yPass > 1.0 )
                yPass = 1.0;

            if( tSize < 1.0001 )
                yPass = 1.0;

            A[i][j] = yPass;
        }
        A[i][nSize()-1] = 0;
    }

    // 2. Convert to a FRACTION retained basis

    for( i=0; i<nSize(); i++ )
    {
        // ultrafine sink size a special case
        A[i][nSize()-1] = A[i][nSize()-2];

        for( j=nSize()-2; j>i; j-- )
        {
            // retained by difference
            A[i][j] = A[i][j-1] - A[i][j];
        }
        // diagonal element is zero
        A[i][i] = 0;
    }
}

//-- Create daughter products of breakage -----------------------------------

void OSM_Model400::makeDaughters( OSM_Vector& feed, OSM_Vector& broken )
{
    int size = 0;
    double tonnes = 0;

    // No material in broken yet
    broken = 0;

    // Contribute breakage of each non-zero size fraction to broken
    for( size=0; size<nSize(); size++ )
        if( (tonnes=feed[size]) != 0.0 )
            broken.addProduct( A[size], tonnes );
}


//-- end OSM_Crush.cpp ------------------------------------------------------

