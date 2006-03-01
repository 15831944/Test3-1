//-- OSM_type.cpp -----------------------------------------------------------
//
// Classes Implemented
//
//  OSM_Type
//  OSM_HasType
//
// History:
//
//  31/01/2000  First version. 
//    David T.
//
//  08/02/2000  Class name changes to avoid confusion between typing
//    David T.  of OSM objects (for mutual compatibility) and Stream:
//              - OSM_Type (was StreamType) manages material properties
//                of multiple OSM objects in the current analysis
//              - OSM_HasType (was HasStreamType) brands an object
//                with a reference to its relevant OSM_Type)
//    
//  09/02/2000  - Split OSM_type.cpp from OSM.cpp to separate the
//    David T.    OSM basic type classes from the OSM Stream classes.
//


//-- External Definitions ---------------------------------------------------
#include "stdafx.h"
#include <stdlib.h>
#include <math.h>

#include "OSM.h"


//---------------------------------------------------------------------------
//
// OSM_Type IMPLEMENTATION
//
//   Manages information associated with a type of stream
//   classification. Specifically looks after the number
//   of ore components represented by streams, the number
//   size fractions in the size distribution of each ore
//   component, and the sieve and nominal sizes of each
//   size fraction in those distributions.
//
//   Sizing format is fractional %retained, with the first
//   fraction being the coarsest. It is assumed that the
//   smallest sieve size specified is zero, ie that the 
//   finest size fraction is an undersize fraction.
//
// Members Defined:
//
//   OSM_Type( )
//   ~OSM_Type( )
//   void setSizes( int, double* )
//   void setSizes( OSM_Vector& )
//   void makeNominalSizes( )
//

//-- Uninitialized OSM_Type -------------------------------------------------

OSM_Type::OSM_Type( )
{
    numSize = 0;
    numComp = 0;
}

//-- Deallocate OSM_Type ----------------------------------------------------

OSM_Type::~OSM_Type( )
{
    /* void */
}

//-- Set OSM_Type size distribution, specifying array of sieve numbers ------

void OSM_Type::setSizes( int numSizes, double* buffer )
{
	numSize = numSizes;
    sieveVec.dimension( numSizes, buffer );
    makeNominalSizes( );
}

//-- Set size distribution specifying Vector of sieve sizes -----------------

void OSM_Type::setSizes( OSM_Vector& sieveSizes )
{
	numSize = sieveSizes.count();
    sieveVec = sieveSizes;
    makeNominalSizes( );
}

//-- Create vector of nominal sizes from the current sieve sizes ------------

void OSM_Type::makeNominalSizes( )
{
    int count = sieveVec.count();               // Number of size fractions

    nominalVec.dimension( count );              // Resize nominal size Vector

    for( int size=0; size<count; size++ )       
    {
        if( size==0 )                           
        {                                       // Coarsest size
            nominalVec[size]                    //   assume equal to sieve
              = sieveVec[size] * pow(2.0,0.25);   //   size * 2^(1/4)
        }
        else if( size==count-1 && size>=0 )                  
        {                                       // Finest size
            nominalVec[size]                    //   assume half finest sieve
              = sieveVec[size-1] / 2.0;           
        }
        else
        {                                       // Intermediate sizes
            nominalVec[size] = sqrt(            //   use geometric mean size
                sieveVec[size-1] * sieveVec[size]  
            );               
        }                      
    }
}


//---------------------------------------------------------------------------
//
// OSM_HasType IMPLEMENTATION
//
//   Represents the property of an object needing to know particular
//   OSM_Type properties. e.g. streams and connected models need to
//   agree on the number of components and size fractions represented.
//   Objects which inherit OSM_HasType and specify the same OSM_Type to
//   OSM_HasType() constructor will refer to the OSM_Type object, and
//   will agree on OSM_Type properties.
//
//   OSM_HasType acts as a proxy to the specified remote OSM_Type
//   object by reimplementing much of the OSM_Type interface.
//
// Members Defined:
//
//   OSM_HasType( StreamType& )
//   ~OSM_HasType( )
//

//-- Initialize with StreamType ---------------------------------------------

OSM_HasType::OSM_HasType( OSM_Type& sType )
  : myType( sType )
{
    /* void */
}

//-- Deallocate -------------------------------------------------------------

OSM_HasType::~OSM_HasType( )
{
    /* void */
}


//-- end OSM_type.cpp -------------------------------------------------------

