#pragma once
#ifndef FLOWSTREAM1_H_F2C90C01_FC3D_4243_BD83_07CA807DC8BD
#define FLOWSTREAM1_H_F2C90C01_FC3D_4243_BD83_07CA807DC8BD
#include "TS.h"
#include "StreamInfo1.h"

namespace RioTintoTS
{

// Forward declarations.
class FlowStream1;

//! Smart pointer typedef for class \c TestStream.
typedef ObjectPtr<FlowStream1> PFlowStream1;

//! A simple stream that consists of a single floating point value.
class TS_API FlowStream1
{
public:
  //! Construct an unconfigured stream.
	FlowStream1( void );

	//! Destructor.
	~FlowStream1( void );

	// Initialize the stream with configuration and data
	bool Initialize
	(
		PStreamInfo1 Config,
		Matrix solids = Matrix(),
		double liquid = 0
	);

	// Configure this stream
	bool SetConfig( PStreamInfo1 other );

	// Get the number of size fractions
	long nSize( )				{ return nSize_; }

    // Get the number of material types
    long nType()             { return nType_; }

	// Refer to a mineral in the configuration
	PMineralInfo1 GetMineral( int iType );
	
	// Obtain a reference to the solids matrix
    MatrixView& AccessSolidsMatrix( ) { return solids_; }

    // Get the liquid phase quantity
    double GetLiquidMass( )          { return liquid_; }

	// Set the liquid phase quantity
	double SetLiquidMass( const double& liquid ) { return liquid_ = liquid; }

    // Get a particular mineral type's information
	PMineralInfo1 AccessMaterialInfo( int iType )
	{
		return config_->GetMineral(iType);
	}

    // Get the vector of Sieve sizes;
    const VectorView& GetSizes( )  { return config_->GetSizes(); }

    // Clear the values in a FlowStream
    void Clear( );

    // Set the values from another FlowStream
    void SetStream( PFlowStream1 Stream2 );

    // Add our values to the values in another FlowStream
    void AddStream( PFlowStream1 Stream2 );
	
	// Set the solids density of a Flowstream by adding water
    double SetSolidsDensity( const double& percentSolids );

    // Calculate additional water required to set a specified solids density 
    double CalcWaterAddition( const double& percentSolids );

	double SolidsVolume( void ) { return solidsVolume_; };

    // Calcualte extra stream values (totals etc)
    void Refresh( );

	// Get combined P80
	double CombinedP80( ) { return combinedP80_; }


    // *STATIC* Create a FlowStream1 object from material info / sizing
    static PFlowStream1 Create( PStreamInfo1 Config );

    // *STATIC* Compare two streams - greatest absolute difference
    static double AbsDifference
    (
        PFlowStream1 Stream1,
        PFlowStream1 Stream2
    );


private:

	PStreamInfo1 config_;		// Stream configuration information
	int nSize_;					// number of size fractions
	int nType_;				// number of mineral types
    
    double liquid_;				// Quantity of liquid (mass basis)
    Matrix solids_;				// Quantity of solids in size x type classes (mass basis)

    //-- Values writen when Refresh() is called -----------------------------

    Vector P80_;                     // P80 of each type
    Vector SG_;                      // SG of each type
    Vector InvSG_;                   // Inverse of SG_

    double solidsMass_;              // mass total of all solids
    double solidsVolume_;            // volume total of all solids
    double percentSolids_;           // solids density (mass-based)

    Vector massInSize_;              // sum of mass in size class
    Vector volumeInSize_;            // sum of mass in size class

    Vector massInType_;              // sum of mass in type class
    Vector volumeInType_;            // sum of volume in type class

    double combinedP80_;             // P80 of the combined sizing
};

}		// namespace RioTintoTS

#endif  // FLOWSTREAM1_H_F2C90C01_FC3D_4243_BD83_07CA807DC8BD