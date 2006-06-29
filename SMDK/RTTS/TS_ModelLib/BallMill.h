//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __BallMill_H
#define  __BallMill_H

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

#include "TSBallMill.h"
#include "Mill.h"

//---------------------------------------------------------------------------

class CMill_Ball : public CMill
{
public:
    // Constructor
	CMill_Ball();

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
	double mi_KnotSize[31];
	double mi_KnotLnRDStar[31];
		
	// BALL MILL APPEARANCE FUNCTION Input
	double mi_A[31];

	//  MODEL CALCULATED VALUES
	//  ~~~~~~~~~~~~~~~~~~~~~~~

	double mo_KnotLnRDStar[31];
	double mo_RDStar[31];

	// Model calculated scaling factors
	double mo_FACTA;
	double mo_FACTB;
	double mo_FACTC;
	double mo_FACTD;
	double mo_FACTEsizeltXm;
	double mo_FACTEsizegeXm;
	double mo_Xm;

	// The TS Ball Crusher
	RioTintoTS::BallMill Mill;
	// System persistent feed data
	RioTintoTS::PFlowStream1   FeedStream;
	RioTintoTS::PStreamInfo1   MatInfo;

	// Mapping area for SysCAD to TS params
 	double m_Params[200];

};

#endif