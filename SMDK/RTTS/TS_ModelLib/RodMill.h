//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __RodMill_H
#define  __RodMill_H

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

#include "TSRodMill.h"
#include "Mill.h"

//---------------------------------------------------------------------------

class CMill_Rod : public CMill
{
public:
    // Constructor
	CMill_Rod();

    // Data Entry Field Method
	void BuildDataFields(MDataDefn &DB);

    // Seperator Method Execution
	void EvalProducts(MBaseMethod &M,
		              MStream &Feed ,
	                  MStream &Product,
					  bool bInit);

protected:

	//
	// Parameters
	// 
	//MILL OPERATING VARIABLES
	//C  ~~~~~~~~~~~~~~~~~~~~~~~~
	//
	//PARAMETER  1 = NUMBER OF PARALLEL MILLS
	long  mi_NParrallelMills;

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
	double mi_A[31];

	//  PARAMETERS 48-78 = ROD MILL CLASSIFICATION FUNCTION
	// How will we specify this or will it be calculated???
	// Doco talks about default classification values being same as sieves??
	double mi_C[31];

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
	double mo_S[31];

	//  PARAMETERS 114-144 = Sizes at which above function data occurred

	// The TS Rod Crusher
	RioTintoTS::RodMill Mill;
	// System persistent feed data
	RioTintoTS::PFlowStream1   FeedStream;
	RioTintoTS::PStreamInfo1   MatInfo;

	// Mapping area for SysCAD to TS params
 	double m_Params[78];

};

#endif