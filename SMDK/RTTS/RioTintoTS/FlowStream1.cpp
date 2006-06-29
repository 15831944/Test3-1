
// externals

#include "FlowStream1.h"

#include <cmath>
#include <vector>
#include <sstream>

#pragma optimize("", off)



//-- Externals ------------------------------------------------------------------------

using namespace RioTintoTS;


//-- FlowStream1( ) -------------------------------------------------------------------
//
//	Empty, unconfigured FlowStream1
//
FlowStream1::FlowStream1( void ) 
{
	/* void */
}


//-- ~FlowStream1( ) ------------------------------------------------------------------
//
FlowStream1::~FlowStream1( void )
{
	/* void */
}


/****************************************************************************
*
*   FlowStream1::Initialize( )
*
*   Apply array of variant arguments to initialize the values
*   of a FlowStream1 object
*
****************************************************************************/
bool FlowStream1::Initialize
(
	PStreamInfo1 Config,
	Matrix		 solids,
	double       liquid
)
{
    // Attempt to build stream structure to match these
    if( !SetConfig( Config ) )
        goto initFailed;

    // Test that dimensions are sensible
    if( nSize_<2 || nType_<1 )
        goto initFailed;

    // test that supplied matrix is correct size
    if( solids.rowCount() == nSize() 
        && solids.columnCount() == nType() )
    {
        // use the provided solids data
        solids_ = solids;
    }

    // use supplied liquid flow
    liquid_ = liquid;

    // refresh calculated values
    Refresh( );

    return true;

initFailed:

    return false;

}




/****************************************************************************
*
*   FlowStream1::SetConfig( )
*
*   Configure a stream to match the materials and dimensions
*   specified by supplied material and sizing information objects.
*
****************************************************************************/
bool FlowStream1::SetConfig( PStreamInfo1 Config )
{
    // locals
    int i = 0;

    // Test that arguments are reasonable (size)
    if( Config->nType()<1 || Config->nSize()<2 )
        goto configFail;

    // Load members with information arguments
    config_   = Config;
    nType_ = Config->nType();
    nSize_    = Config->nSize();
    
    // Construct empty Solids matrix and liquid flow
    solids_.resize( nSize_, nType_ );
    solids_.clear( 0.0 );
    liquid_ = 0;

    // Build P80, SG and combinedSolids vectors
    massInSize_.resize( nSize_ );
    volumeInSize_.resize( nSize_ );
    massInType_.resize( nType_ );
    volumeInType_.resize( nType_ );

    P80_.resize( nType_ );
    SG_.resize( nType_ );
    InvSG_.resize( nType_ );

    // Set late calculated values to 0.0
    massInSize_.clear( 0.0 );
    volumeInSize_.clear( 0.0 );
    massInType_.clear( 0.0 );
    volumeInType_.clear( 0.0 );
    P80_.clear( 0.0 );

    // Set the SG and InvSG_ vector from the mineral types
    for( i=0; i<nType_; i++ )
    {
        double SG = config_->GetMineral(i)->SG();

        SG_[i] = SG;

        if( fabs(SG) < EPSILON )
            InvSG_[i] = 0.0;
        else
            InvSG_[i] = 1.0 / SG;
    }

    // FlowStream1 successfully configured
    //  and can be used
    return true;

configFail:

    // Configuration failure. FlowStream object
    //  should be destroyed or re-configured
    return false;

}




/****************************************************************************
*
*   FlowStream1::RockType( )
*
*   Obtain the material properties of one of the solid materials
*   represented by a FlowStream1 object
*
****************************************************************************/
PMineralInfo1 FlowStream1::GetMineral( int iType )
{
    if( config_ && iType < nType_ )
    {
        return config_->GetMineral(iType);
    }
    else
    {
        return PMineralInfo1();
    }
}




/****************************************************************************
*
*   FlowStream1::Clear( )
*
*   Set all components of a stream to 0
*
****************************************************************************/
void FlowStream1::Clear( )
{
    liquid_ = 0.0;

    if( nSize_>0 && nType_>0 )
        solids_.clear();
}




/****************************************************************************
*
*   FlowStream1::SetStream( )
*
*   Set the values contained in this stream to be the same as another
*   stream. Can only succeed if both streams have the same number of
*   size fractions and the same number of mineral types.
*
****************************************************************************/
void FlowStream1::SetStream( PFlowStream1 Stream2 )
{
    if( nSize_ == Stream2->nSize() && nType_ == Stream2->nType() )
    {
        liquid_ = Stream2->GetLiquidMass();
        solids_ = Stream2->AccessSolidsMatrix();
    }
}




/****************************************************************************
*
*   FlowStream1::AddStream( )
*
*   Add to the values contained in this stream with the values
*   of another stream. Can only succeed if both streams have the 
*   same number of size fractions and the same number of types.
*
****************************************************************************/
void FlowStream1::AddStream( PFlowStream1 Stream2 )
{
    int i=0;
    int j=0;

    if( nSize_ == Stream2->nSize() && nType_ == Stream2->nType() )
    {
        liquid_ += Stream2->GetLiquidMass();

        MatrixView AddSolids = Stream2->AccessSolidsMatrix();

        for( i=0; i<nSize_; i++ )
            for( j=0; j<nType_; j++ )
                solids_[i][j] += AddSolids[i][j];
    }
}




/****************************************************************************
*
*   FlowStream1::AbsDifference( )    * STATIC FREE FUNCTION *
*
*   Determine the greatest absolute numerical difference between
*   the values in one stream and the corresponding values in
*   another stream.  Can only succeed if both streams have the
*   same number of size fractions and the same number of mineral
*   types.
*
****************************************************************************/
double FlowStream1::AbsDifference( PFlowStream1 Stream1, PFlowStream1 Stream2 )
{
    int i=0;
    int j=0;

    double maxError = 0.0;
    double delta    = 0.0;

    int nSize = Stream1->nSize();
    int nType = Stream1->nType();

    if( nSize == Stream2->nSize() && nType == Stream2->nType() )
    {
        delta = fabs( Stream1->GetLiquidMass() - Stream2->GetLiquidMass() );
        if( delta > maxError ) maxError = delta;

        MatrixView& Solids1 = Stream1->AccessSolidsMatrix();
        MatrixView& Solids2 = Stream2->AccessSolidsMatrix();

        for( i=0; i<nSize; i++ )
        {
            for( j=0; j<nType; j++ )
            {
                delta = fabs( Solids1[i][j] - Solids2[i][j] );
                if( delta > maxError ) maxError = delta;
            }
        }
    }
    return maxError;
}




/****************************************************************************
*
*   FlowStream1::CalcWaterAddition( )
*
*   Calculates the ammount of water required to have the solids
*   desnity by mass of a flow stream to be equal to the 
*   supplied %solids value. Does not modify the flow stream.
*
*   Returns the quantity of water required (m3/h).
*
****************************************************************************/
double FlowStream1::CalcWaterAddition( const double& percentSolids )
{
    double WaterRequired = 0.0;
    double WaterAddition = 0.0;

    // Determine the mass of solids
    double SolidsMass = solids_.sum();

    // Determine the mass of water required
    if( percentSolids < EPSILON )
    {
        WaterAddition = 0;
    }
    else if( percentSolids >= 100.0 )
    {
        WaterAddition = 0;
    }
    else if( fabs(SolidsMass) < EPSILON )
    {
        WaterAddition = 0;
    }
    else
    {
        WaterRequired = (SolidsMass/percentSolids)*(100-percentSolids);
        WaterAddition = WaterRequired - liquid_;
    }
    return WaterAddition;
}





/****************************************************************************
*
*   FlowStream1::SetSolidsDensity( )
*
*   Adds the ammount of water required to have the solids
*   density by mass of a flow stream to be equal to the 
*   suppled %solids value. Returns the ammount of water 
*   required to perform this adjustment.
*
****************************************************************************/
double FlowStream1::SetSolidsDensity( const double& percentSolids )
{
    double WaterAddition = CalcWaterAddition( percentSolids );

    liquid_ += WaterAddition;

    return WaterAddition;
}




/****************************************************************************
*
*   FlowStream1::Create( )
*
*   Create a new FlowStream1 that matches the supplied 
*   materials information and size distribution series.
*
****************************************************************************/
PFlowStream1 FlowStream1::Create( PStreamInfo1 Config )
{
    // Empty flowstream to build
    PFlowStream1 NewStream;

    // Test that arguments are valid
    if( Config )
    {
        // Create new FlowStream1
        NewStream = PFlowStream1( new FlowStream1 );

        // Test configuration with arguments
        if( false == NewStream->SetConfig(Config) )
        {
            // Failed - remove the newly created stream
            NewStream = PFlowStream1( );
        }
    }
    // Send newly created/configured stream to caller
    return NewStream;
}




/****************************************************************************
*
*   FlowStream1::Refresh( )
*
*   Create a new FlowStream1 that matches the supplied 
*   materials information and size distribution series.
*
****************************************************************************/
void FlowStream1::Refresh( )
{
    int iSize = 0;
    int iType = 0;

    // start with the first row of the solids matrix
    TableVector RowVec = solids_.row( 0 );

    // Start solids totals
    solidsMass_  = 0.0;
    solidsVolume_ = 0.0;

    // loop over sizes (rows of the solids matrix)
    for( iSize=0; iSize<nSize_; iSize++ )
    {
        // calculate mass/volume in this size fraction
        double mass = RowVec.sum();
        double volume = dotProduct( RowVec, InvSG_ );
        
        // store mass in fraction
        massInSize_[iSize] = mass;
        volumeInSize_[iSize] = volume;

        // accumulate totals
        solidsMass_ += mass;
        solidsVolume_ += volume;

        // go to the next row of the solids matrix
        RowVec.next();
    }

    // start with the first column of the solid matrix
    TableVector ColVec = solids_.column( 0 );

    // loop over types (columns of the solids matrix)
    for( iType=0; iType<nType_; iType++ )
    {
        // calculate the total material in this type
        double mass = ColVec.sum();

        massInType_[iType]   = mass;
        volumeInType_[iType] = mass * InvSG_[iType];

        // go to the next column of the solids matrix
        ColVec.next();
    }
}



