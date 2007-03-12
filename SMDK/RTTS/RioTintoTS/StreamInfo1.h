//-- StreamInfo1.h: Declarations for StreamInfo1 class ----------------------

#if !defined(STREAMINFO1_H_44F28FA0_40A3_422D_A809_C776D589F509)
#define      STREAMINFO1_H_44F28FA0_40A3_422D_A809_C776D589F509

#pragma once

#include "ObjectPtr.h"
#include "Vector.h"
#include "Matrix.h"
#include "MineralInfo1.h"

#pragma warning(disable : 4251) 

namespace RioTintoTS
{

// smart pointer for StreamInfo1

class StreamInfo1;
typedef ObjectPtr<StreamInfo1> PStreamInfo1;


/****************************************************************************
*                       
*   class StreamInfo1
*
*   Class defines the configuration of a FlowStream1 stream
*
****************************************************************************/
class StreamInfo1
{

public:    

    // Empty StreamInfo
    StreamInfo1( ) { }

    // Configure StreamInfo
    bool Configure
    (                                                      
        RioTintoTS::VectorView& sieves,         // vector of sieve sizes
        std::vector<PMineralInfo1>& MineralVec, // collection of minerals
        double liquidSG = 1.0                   // density of liquid phase
    );
    
    // Get the number of size fractions
    long nSize( ) const { return nSize_; }

	// Get the number of ore types
    long nType( ) const { return nType_; }

    // Access the vector of size fraction sieves
    RioTintoTS::VectorView& GetSizes() { return sizes_; }

    // Access particular mineral information
    PMineralInfo1& GetMineral( int iType ) { return minerals_[iType]; }

    // Retrieve the liquid density
    double GetLiquidSG( ) { return liquidSG_; }

	// Set the sieve series
	bool SetSizes( const VectorView& newSieves )
	{
		nSize_ = newSieves.size();
		sizes_ = newSieves;
		return true;
	}

	// Set the number of minerals
	bool SetNumberOfMinerals( int nType )
	{
		nType_ = nType;
		minerals_.resize( nType, PMineralInfo1() );
		// fill vector[] wih uninitialized MineralInfo()
		for( int i=0; i<nType; i++ )
		{
			minerals_[i] = PMineralInfo1( new MineralInfo1() );
			minerals_[i]->SetDefaultAppearance( );
		}
		return(true);
	}

private:

    long nSize_;            // Number of size fractions
    long nType_;         // Number of mineral types
    double liquidSG_;       // Density of liquid phase

    // Array of mineral information
    std::vector<PMineralInfo1> minerals_;

    // Vector of size fraction sieve values
    RioTintoTS::Vector sizes_;
	
};
            


}       // RioTintoTS namespace

#endif  // STREAMINFO1_H_44F28FA0_40A3_422D_A809_C776D589F509
