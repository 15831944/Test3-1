//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __AGSAGMill_H
#define  __AGSAGMill_H

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

//#include "TSAGSAGMill.h"
#include "Model430A.h"
#include "Mill.h"

//---------------------------------------------------------------------------

class CMill_AGSAG : public CMill
{
public:
    // Constructor
	CMill_AGSAG();

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

	// Recycles
	/*
	double m_iReference_F80sim;
	double m_iNew_Feedsim;		
	double m_iNew_FeedFracsim;
	double m_iNew_FeedRecyclesim;
	double m_oRecyclesim;	
	double m_oRecycleratiosim;
	bool   m_iUseRecycles;	

	double m_iReference_F80org;
	double m_iNew_Feedorg;		
	double m_oRecycleorg;	
	double m_oRecycleratioorg;
    */

	// Dimensions

	double m_iDiametersim;
	double m_iBellyLengthsim;
	double m_iFeedTrunionDiasim;		
	double m_iFeedConeAnglesim;		
	double m_iDischargeConeAnglesim;	
	double m_ixgsim;					
	double m_ixmsim;					
	double m_iGrateOpenAreasim;		
	double m_iPebblePortOpenAreasim;
	double m_iPebblePortSizesim;		
	double m_iMeanRelRadialPossim;	

	/*
	double m_iDiameterorg;			
	double m_iBellyLengthorg;			
	double m_iFeedTrunionDiaorg;		
	double m_iFeedConeAngleorg;		
	double m_iDischargeConeAngleorg;	
	double m_ixgorg;					
	double m_ixmorg;					
	double m_iGrateOpenAreaorg;		
	double m_iPebblePortOpenAreaorg;	
	double m_iPebblePortSizeorg;		
	double m_iMeanRelRadialPosorg;	
    */

	// Ball Load
	
	double m_iBallLoadsim;
	/*
	double m_oBallLoadsim;		
	*/
	double m_iBallTopSizesim;

	/*
	double m_iBallLoadorg;	
	double m_oBallLoadorg;		
	double m_iBallTopSizeorg;
    */

	double m_iBallSize1sim;
	double m_iBallSize2sim;	
	double m_iBallSize3sim;	
	double m_iBallSize4sim;	

	//double m_iBallSize1org;	
	//double m_iBallSize2org;	
	//double m_iBallSize3org;	
	//double m_iBallSize4org;	

	/*
	double m_oOreSize1;	
	double m_oOreSize2;		
	double m_oOreSize3;		
	double m_oOreSize4;		
	double m_oCalcSubsize;
    */

	//
	// Breakage Rates
	//
	double m_iKnot1;	
	double m_iKnot2;	
	double m_iKnot3;
	double m_iKnot4;
	double m_iKnot5;	

	double m_iRate1sim;
	double m_iRate2sim;
	double m_iRate3sim;
	double m_iRate4sim;
	double m_iRate5sim;

	/*
	double m_oRate1org;
	double m_oRate2org;
	double m_oRate3org;
	double m_oRate4org;
	double m_oRate5org;
    */

	//
	// Breakage Function
	//
	/*
	double m_iAsim;
	double m_iBsim;	
	double m_itasim;	
	double m_iAorg;
	double m_iBorg;	
	double m_itaorg;	
	double m_oEcs;	
    */

	//
	// Mass Transfer
	//
	double m_iDischargeCoeffsim;
	double m_im1sim;
	double m_im2sim;			
	double m_iCoarseFactorsim;
	//double m_oMaxMillCapacitysim;
	//double m_iUserTotLoadsim;	
	//double m_oMaxDesignFlowsim;

	//double m_iDischargeCoefforg;	
	//double m_om1org;
	//double m_om2org;			
	//double m_oCoarseFactororg;

	//
	// Power
	//
	double m_iFracCriticalSpeedsim;
	double m_iBallSGsim;				
	double m_iPorositysim;			
	double m_iPowerAdjustsim;		
	//double m_oChargeDensitysim;		
	//double m_oVoidFractionsim;		
	double m_iNoLoadPower;			
	//double m_oGrossPower;			
	//double m_iMeasuredPower;			
	//double m_iExpVolTotalLoad;		
	//double m_oCalVolTotalLoad;		
	//double m_iSDVolTotalLoad;		
	//double m_iErrVolTotalLoad;		

	//double m_iFracCriticalSpeedorg;
	//double m_iBallSGorg;			
	//double m_iPorosityorg;		
	//double m_iPowerAdjustorg;		

	//
	// Calculate Parameters
	//

	//
	// Ore Load
	//
	double m_oAverageSizeTop20;
	double m_oMillPulpLoad;

#if WithFortranModel430
	// The TS AGSAG Crusher
	RioTintoTS::Model430A Mill;
#endif

	// System persistent feed data
	RioTintoTS::PFlowStream1   FeedStream;
	RioTintoTS::PStreamInfo1   MatInfo;

	// Mapping area for SysCAD to TS params
 	double m_Params[78];

};

#endif