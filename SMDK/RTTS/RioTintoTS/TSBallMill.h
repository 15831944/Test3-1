#pragma once
#ifndef BallMill_H_
#define BallMill_H_

#include "TS.h"
#include "ObjectPtr.h"
#include "StreamInfo1.h"
#include "FlowStream1.h"

namespace RioTintoTS
{

// Forward declarations.
class BallMill;

//! Smart pointer typedef for class \c TestStream.
typedef ObjectPtr<BallMill> PBallMill;


/****************************************************************************
*                       
*   class BallMill;
*
*   UnitModel that for the JK style ball mill model.
*
****************************************************************************/
class TS_API BallMill
{

public:

    // Construct default
    BallMill( );

    // Destroy resources
    ~BallMill( );

	// Initialize model with configuration and parameters
	bool Initialize( PStreamInfo1 Config, const VectorView& paramVec );

    // Cause BallMill to calculate itself
    virtual bool CalculateModel( PFlowStream1 FeedStream );

	// Ball Mill streams
    PFlowStream1 Discharge;

	// Model calculated values
	Vector ModelOutput;


private:

    // Implementation

    int nType_;					// Number of size fractions
    int nSize_;					// Number of material types

    Vector C_;					// Classification function for each size
    Vector nomSize_;			// Nominal size of each size fraction

    PStreamInfo1 config_;		// Stream information structure

	//
	// Parameters
	// 

	// NUMBER OF PARALLEL MILLS
	long  mi_NParrallelMills;

	// DIAMETER (INSIDE LINERS, METRES) OF MILL BEING SIMULATED.
	double mi_DiameterSim;

	// LENGTH (INSIDE LINERS, METRES) OF MILL BEING SIMULATED.
	double mi_LengthSim;

	// FRACTION CRITICAL SPEED OF MILL BEING SIMULATED.
	double mi_FracCriticalSpeedSim;

	// LOAD FRACTION (SEE ABOVE) OF MILL BEING SIMULATED.
	double mi_FracLoadSim;

	// WORK INDEX OF ORE FED TO MILL BEING SIMULATED.
	double mi_WorkIndexSim;

	//  Simulated Mill Ball Top Size
	double mi_BallTopSizeSim;

	// MODEL PARAMETERS
	//  ~~~~~~~~~~~~~~~~

	// MILL DIAMETER (INSIDE LINERS, METRES) OF MILL FROM
	//         WHICH MODEL PARAMETERS ARE DERIVED.
	double mi_DiameterDerived;

	// LENGTH OF MILL FROM WHICH MODEL PARAMETERS ARE DERIVED.
	double mi_LengthDerived;

	// FRACTION OF CRITICAL SPEED OF MILL FROM WHICH MODEL
	//                 PARAMETERS ARE DERIVED.
	double mi_FracCriticalSpeedDerived;

	// LOAD FRACTION (I.E. FRACTION OF MILL OCCUPIED BY
	//         STATIONARY LOAD, INCLUDING VOIDS) OF MILL FROM
	//         WHICH MODEL PARAMETERS ARE DERIVED.
	double mi_FracLoadDerived;

	// ORE WORK INDEX OF MILL FROM WHICH MODEL PARAMETERS ARE
	//                 DERIVED.
	double mi_WorkIndexDerived;

	//  Original Mill Ball Top Size
	double mi_BallTopSizeDerived;

	// Max Breakage Rate Factor
	double mi_K;


	// Rate/Discharge Function Input
	long mi_NumSplineKnots;
	Vector mi_KnotSize;
	Vector mi_KnotLnRDStar;
		
	// BALL MILL APPEARANCE FUNCTION Input
	Vector mi_A;

	//  MODEL CALCULATED VALUES
	Vector mo_KnotLnRDStar;
	Vector mo_RDStar;
	double mo_FACTA;
	double mo_FACTB;
	double mo_FACTC;
	double mo_FACTD;
	double mo_FACTEsizeltXm;
	double mo_FACTEsizegeXm;
	double mo_Xm;

};

}

#endif
