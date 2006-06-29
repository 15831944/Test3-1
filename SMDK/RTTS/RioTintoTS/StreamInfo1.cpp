//-- StreamInfo1.cpp --------------------------------------------------------

#include <memory.h>
#include "CubicSpline.h"
#include "StreamInfo1.h"

using namespace RioTintoTS;
#pragma optimize("", off)



namespace
{
	void TotallyUselessFunction_NeverGetsCalled( )
	{
		PStreamInfo1 dummy( new StreamInfo1() );
	}
}



/****************************************************************************
*
*   StreamInfo1::Configure( )
*
*   Set configuration of stream information.
*
****************************************************************************/
bool StreamInfo1::Configure
(
    VectorView& sieves,                     // vector of sieve sizes
    std::vector<PMineralInfo1>& minerals,   // collection of minerals
    double liquidSG                         // density of liquid phase
)
{
    // ensure reasonable number of sieves
    if( sieves.size() <= 2 )
        goto initFailed;

    // ensure reasonable number of minerals
    if( minerals.size() < 1 )
        goto initFailed;

    // set implementation to supplied size distribution
    nSize_ = sieves.size();
    sizes_.resize( nSize_ );
    sizes_ = sieves;

    // set implementation to supplied mineral collection
    nType_ = static_cast<long>( minerals.size() );
    minerals_.clear( );
    minerals_.assign( minerals.begin(), minerals.end() );

    // should probably check for NULL
    //  pointer entries in minerals_

    // succeeded
    return true;

initFailed:

    // Initialization failed - object should not be used
    return false;
}




//-- StreamInfo1.cpp --------------------------------------------------------
