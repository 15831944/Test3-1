#pragma once
#ifndef RodMill_H_
#define RodMill_H_

#include "TS.h"
#include "ObjectPtr.h"
#include "StreamInfo1.h"
#include "FlowStream1.h"

namespace RioTintoTS
{

// Forward declarations.
class RodMill;

//! Smart pointer typedef for class \c TestStream.
typedef ObjectPtr<RodMill> PRodMill;




/****************************************************************************
*                       
*   class RodMill;
*
*   UnitModel that for the JK style rod mill model.
*
****************************************************************************/
class TS_API RodMill
{

public:

    // Construct default
    RodMill( );

    // Destroy resources
    ~RodMill( );

	// Initialize model with configuration and parameters
	bool Initialize( PStreamInfo1 Config, const VectorView& paramVec );

    // Cause RodMill to calculate itself
    virtual bool CalculateModel( PFlowStream1 FeedStream );

	// Product streams
    PFlowStream1 Discharge;

	// Model calculated values
	Vector ModelOutput;
	Vector T10;

private:

    // Implementation


    int nType_;					// Number of size fractions
    int nSize_;					// Number of material types

    Vector C_;					// Classification function for each size
    Vector nomSize_;			// Nominal size of each size fraction

    PStreamInfo1 config_;		// Stream information structure
    Vector Content_;			// Machine contents
    Vector BreakFeed_;			// Material selected for breakage
    Vector BreakProd_;			// Breakage fragments of BreakFeed_;


	double mo_TPH;					// solids feed to crusher
	double mo_F80;					// 80% passing size of feed
	double mo_P80;					// 80% passing size of product

	//MILL OPERATING VARIABLES
	//C  ~~~~~~~~~~~~~~~~~~~~~~~~
	//
	//PARAMETER  1 = NUMBER OF PARALLEL MILLS
	int  mi_NParrallelMills;

	//PARAMETER  2 = DIAMETER (INSIDE LINERS, METRES) OF MILL BEING SIMULATED.
	double mi_DiameterSim;

	//PARAMETER  3 = LENGTH (INSIDE LINERS, METRES) OF MILL BEING SIMULATED.
	double mi_LengthSim;

	//PARAMETER  4 = FRACTION CRITICAL SPEED OF MILL BEING SIMULATED.
	double mi_FracCriticalSpeedSim;

	//PARAMETER  5 = LOAD FRACTION (SEE ABOVE) OF MILL BEING SIMULATED.
	double mi_FracLoadSim;

	//PARAMETER  6 = WORK INDEX OF ORE FED TO MILL BEING SIMULATED.
	double mi_WorkIndexSim;

	// MODEL PARAMETERS
	//  ~~~~~~~~~~~~~~~~

	//PARAMETER  7 = MILL DIAMETER (INSIDE LINERS, METRES) OF MILL FROM
	//         WHICH MODEL PARAMETERS ARE DERIVED.
	double mi_DiameterDerived;

	//PARAMETER  8 = LENGTH OF MILL FROM WHICH MODEL PARAMETERS ARE DERIVED.
	double mi_LengthDerived;

	//PARAMETER  9 = FRACTION OF CRITICAL SPEED OF MILL FROM WHICH MODEL
	//                 PARAMETERS ARE DERIVED.
	double mi_FracCriticalSpeedDerived;

	//PARAMETER 10 = LOAD FRACTION (I.E. FRACTION OF MILL OCCUPIED BY
	//         STATIONARY LOAD, INCLUDING VOIDS) OF MILL FROM
	//         WHICH MODEL PARAMETERS ARE DERIVED.
	double mi_FracLoadDerived;

	//  PARAMETER 11 = ORE WORK INDEX OF MILL FROM WHICH MODEL PARAMETERS ARE
	//                 DERIVED.
	double mi_WorkIndexDerived;

	//  PARAMETER 12 = MILL CONSTANT OF MILL FROM WHICH MODEL PARAMETERS ARE DERIVED
	double mi_MillConstantDerived;

	//  PARAMETER 13 = PARAMETER XC, THE PARTICLE SIZE BELOW WHICH THE
	//         SELECTION FUNCTION IS CONSTANT.
	double mi_XC;

	//  PARAMETER 14 = INTERCEPT OF SELECTION FUNCTION AT ZERO SIZE (IN).
	double mi_IN;

	//  PARAMETER 15 = SLOPE OF SELECTION FUNCTION WITH PARTICLE SIZE (SL).
	double mi_SL;

	//  PARAMETER 16 = 90% PASSING SIZE OF FEED TO ROD MILL FROM WHICH MILL
	//          PARAMETERS WERE DERIVED.
	double mi_F90Derived;

	//  PARAMETERS 17-47 = ROD MILL APPEARANCE FUNCTION
	// How are we going to enter this or will it be calculated based on ore type??
	Vector mi_A;

	//  PARAMETERS 48-78 = ROD MILL CLASSIFICATION FUNCTION
	// How will we specify this or will it be calculated???
	// Doco talks about default classification values being same as sieves??
	Vector mi_C;


	//  MODEL CALCULATED VALUES
	//  ~~~~~~~~~~~~~~~~~~~~~~~

	//  PARAMETER 79 = MILL CONSTANT OF MILL BEING SIMULATED.
	double mo_MCSim;

	//  PARAMETER 80 = 90% PASSING SIZE OF FEED TO MILL BEING SIMULATED
	double mo_F90Sim;

	//  PARAMETER 81 = CHANGE IN NO. OF BREAKAGE STAGES WITH FEED COARSENESS
	double mo_Out_ChangeNStages;

	//  PARAMETER 82 = NO. OF BREAKAGE STAGES IN MILL BEING SIMULATED
	double mo_NBreakageStagesSim;

	//  PARAMETERS 83-113 = ROD MILL SELECTION FUNCTION
	Vector mo_S;

	//  PARAMETERS 114-144 = Sizes at which above function data occurred


};

}

#endif
