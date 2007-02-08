//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __RodMill_CPP
#include "SysCADSystemHelper.h"
#include "RodMill.h"
//#pragma optimize("", off)

//====================================================================================
//             Rod Mill
//====================================================================================

   CMill_Rod::CMill_Rod()
   {

	// Construct the Persistent System Feed Data
    FeedStream  = RioTintoTS::PFlowStream1( new RioTintoTS::FlowStream1 );
    MatInfo     = RioTintoTS::PStreamInfo1( new RioTintoTS::StreamInfo1 );

    // Initialise parameters
	mi_NParrallelMills =			1;
	mi_DiameterSim =				2.64; //m 
	mi_LengthSim =					3.58; //m
	mi_FracCriticalSpeedSim =		0.65;
	mi_FracLoadSim =				0.3;
	mi_WorkIndexSim =				15.0*3.6; //kWs/kg

	// MODEL PARAMETERS
	mi_DiameterDerived =			2.64; //m
	mi_LengthDerived =				3.58; //m
	mi_FracCriticalSpeedDerived =	0.65;
	mi_FracLoadDerived =			0.3;
	mi_WorkIndexDerived =			15.0*3.6; //kWs/kg
	mi_MillConstantDerived =		1416;
	mi_XC =							0.00743; //m
	mi_IN =							-0.0036; //m
	mi_SL =							0.5;
	mi_F90Derived =					0.0072; //m
	
	for (int i = 0; i < 31; i++)
	{
		mi_A[i] = 0.0;
		mi_C[i] = 0.0;
	}
	
	mi_A[0] = 0.114;	mi_A[1] = 0.191;	mi_A[2] = 0.150;	mi_A[3] = 0.117;	mi_A[4] = 0.0922;	mi_A[5] = 0.0723;	mi_A[6] = 0.0567;	mi_A[7] = 0.0445;	mi_A[8] = 0.0349;	mi_A[9] = 0.0274;	mi_A[10] = 0.0215;	mi_A[11] = 0.0169;	mi_A[12] = 0.0132;	mi_A[13] = 0.0104;	mi_A[14] = 0.0081;	mi_A[15] = 0.0064;	mi_A[16] = 0.0050;	mi_A[17] = 0.0039;	mi_A[18] = 0.0029;	mi_A[19] = 0.0020;	mi_A[20] = 0.0012;	mi_A[21] = 5.00E-04;	mi_A[22] = 0.000;	mi_C[0] = 1.000;	mi_C[1] = 0.500;	mi_C[2] = 0.250;	mi_C[3] = 0.125;	mi_C[4] = 0.0630;	mi_C[5] = 0.0320;	mi_C[6] = 0.0160;	mi_C[7] = 0.0080;	mi_C[8] = 0.0040;	mi_C[9] = 0.0020;	mi_C[10] = 0.0010;	mi_C[11] = 0.0010;	mi_C[12] = 0.0010;	mi_C[13] = 0.0010;

	// MODEL OUTPUTS
	mo_MCSim = 0.0;
	mo_F90Sim = 0.0; // m
	mo_Out_ChangeNStages = 1;
	mo_NBreakageStagesSim = 1;

	}

//====================================================================================

	void CMill_Rod::BuildDataFields(MDataDefn &DB)
	
	{ 
    DB.ObjectBegin("TS_Rod", "Rod");
	DB.Page("Rod Mill Parameters");

	DB.Text("Simulated Mill");
	//PARAMETER  1 = NUMBER OF PARALLEL MILLS
    DB.Long("NMills",        "", &mi_NParrallelMills , MF_PARAMETER );	

	//PARAMETER  2 = DIAMETER (INSIDE LINERS, METRES) OF MILL BEING SIMULATED.
    DB.Double("Dsim",        "", &mi_DiameterSim	, MF_PARAMETER ,  MC_L);

	//PARAMETER  3 = LENGTH (INSIDE LINERS, METRES) OF MILL BEING SIMULATED.
    DB.Double("Lsim",        "", &mi_LengthSim	, MF_PARAMETER ,  MC_L);

	//PARAMETER  4 = FRACTION CRITICAL SPEED OF MILL BEING SIMULATED.
    DB.Double("CSsim",        "", &mi_FracCriticalSpeedSim	, MF_PARAMETER ,  MC_Frac);

	//PARAMETER  5 = LOAD FRACTION (SEE ABOVE) OF MILL BEING SIMULATED.
    DB.Double("LFsim",        "", &mi_FracLoadSim	, MF_PARAMETER ,  MC_Frac);

	//PARAMETER  6 = WORK INDEX OF ORE FED TO MILL BEING SIMULATED.
    DB.Double("WIsim",        "", & mi_WorkIndexSim	, MF_PARAMETER , MC_WI); // Units??

	// MODEL PARAMETERS
	//  ~~~~~~~~~~~~~~~~
	DB.Text("Fitted Mill");
	//PARAMETER  7 = MILL DIAMETER (INSIDE LINERS, METRES) OF MILL FROM
	//         WHICH MODEL PARAMETERS ARE DERIVED.
    DB.Double("Dfit",        "", &mi_DiameterDerived	, MF_PARAMETER ,  MC_L);

	//PARAMETER  8 = LENGTH OF MILL FROM WHICH MODEL PARAMETERS ARE DERIVED.
    DB.Double("Lfit",        "", &mi_LengthDerived	, MF_PARAMETER ,  MC_L);

	//PARAMETER  9 = FRACTION OF CRITICAL SPEED OF MILL FROM WHICH MODEL
	//                 PARAMETERS ARE DERIVED.
    DB.Double("CSfit",        "", &mi_FracCriticalSpeedDerived	, MF_PARAMETER ,  MC_Frac);

	//PARAMETER 10 = LOAD FRACTION (I.E. FRACTION OF MILL OCCUPIED BY
	//         STATIONARY LOAD, INCLUDING VOIDS) OF MILL FROM
	//         WHICH MODEL PARAMETERS ARE DERIVED.
    DB.Double("LFfit",        "", &mi_FracLoadDerived	, MF_PARAMETER ,  MC_Frac);

	//  PARAMETER 11 = ORE WORK INDEX OF MILL FROM WHICH MODEL PARAMETERS ARE
	//                 DERIVED.
    DB.Double("WIfit",        "", & mi_WorkIndexDerived	, MF_PARAMETER , MC_WI ); // Units??

	//  PARAMETER 12 = MILL CONSTANT OF MILL FROM WHICH MODEL PARAMETERS ARE DERIVED
    DB.Double("MCfit",        "", & mi_MillConstantDerived	, MF_PARAMETER ); // Units??

	//  PARAMETER 16 = 90% PASSING SIZE OF FEED TO ROD MILL FROM WHICH MILL
	//          PARAMETERS WERE DERIVED.
    DB.Double("F90fit",        "", &mi_F90Derived	, MF_PARAMETER ,  MC_L );

	//  PARAMETER 13 = PARAMETER XC, THE PARTICLE SIZE BELOW WHICH THE
	//         SELECTION FUNCTION IS CONSTANT.
	DB.Text("Selection Function");
    DB.Double("XC",        "", &mi_XC	, MF_PARAMETER ,  MC_L);

	//  PARAMETER 14 = INTERCEPT OF SELECTION FUNCTION AT ZERO SIZE (IN).
    DB.Double("IN",        "", &mi_IN	, MF_PARAMETER ,  MC_L);

	//  PARAMETER 15 = SLOPE OF SELECTION FUNCTION WITH PARTICLE SIZE (SL).
    DB.Double("SL",        "", &mi_SL	, MF_PARAMETER );

	//  PARAMETERS 17-47 = ROD MILL APPEARANCE FUNCTION
	// How are we going to enter this or will it be calculated based on ore type??
	DB.Page("Appearance Data");
	for (int i = 0; i < 31; i++)
	{
		char buff[20];
		sprintf(buff,"A%d",i);
		DB.Double(buff,        "", &mi_A[i]	, MF_PARAMETER );
	}

	//  PARAMETERS 48-78 = ROD MILL CLASSIFICATION FUNCTION
	// How will we specify this or will it be calculated???
	// Doco talks about default classification values being same as sieves??
	DB.Page("Classification Data");
	for (int i = 0; i < 31; i++)
	{
		char buff[20];
		sprintf(buff,"C%d",i);
		DB.Double(buff,        "", &mi_C[i]	, MF_PARAMETER );
	}

	//  MODEL CALCULATED VALUES
	//  ~~~~~~~~~~~~~~~~~~~~~~~
	DB.Page("Rod Mill Outputs");

	//  PARAMETER 79 = MILL CONSTANT OF MILL BEING SIMULATED.
    DB.Double("MCsim",        "", &mo_MCSim	, MF_RESULT);

	//  PARAMETER 80 = 90% PASSING SIZE OF FEED TO MILL BEING SIMULATED
    DB.Double("F90sim",        "", &mo_F90Sim	, MF_RESULT ,  MC_Frac);

	//  PARAMETER 81 = CHANGE IN NO. OF BREAKAGE STAGES WITH FEED COARSENESS
    DB.Double("NChange",  "", &mo_Out_ChangeNStages , MF_RESULT);

	//  PARAMETER 82 = NO. OF BREAKAGE STAGES IN MILL BEING SIMULATED
    DB.Double("NBreakStages",  "", &mo_NBreakageStagesSim , MF_RESULT);

	//  PARAMETERS 83-113 = ROD MILL SELECTION FUNCTION
	DB.Page("Selection Function");
	for (int i = 0; i < 31; i++)
	{
		char buff[20];
		sprintf(buff,"S%d",i);
		DB.Double(buff,        "", &mo_S[i]	, MF_RESULT );
	}

	//  PARAMETERS 114-144 = Sizes at which above function data occurred

    DB.ObjectEnd();

	}

//====================================================================================

void CMill_Rod::EvalProducts(MBaseMethod &M,
							 MStream &Feed , MStream &Product ,
		                     bool bInit )
	
{ 

	// Get info on Size Distribution
    MIPSD & l_PSD=*Feed.FindIF<MIPSD>();
    if (IsNothing(l_PSD))
      {
      int xx=0;
      return; //Why do we get here!!!
      }
    long    l_SizeCount      = l_PSD.getSizeCount();
    long    l_PSDVectorCount = l_PSD.getPSDVectorCount();


      if ( bInit && !IsNothing(l_PSD))
	    {
	      bInit = false;

			// Copy the Sieve Data Sizing Info
            SysCADSystemHelper::SysCADSizeDataToSystem(l_PSD, MatInfo);

			// Copy Material Information from database
			// For now we just set up n components
			// The MineralInfo data will come from a database
			MatInfo->SetNumberOfMinerals( l_PSD.PSDVectorCount );
			SysCADSystemHelper::PopulateMaterialInfo(M,MatInfo);

			// Initialize Rod Mill           
			m_Params[0] = mi_NParrallelMills;			
			m_Params[1] = mi_DiameterSim;				
			m_Params[2] = mi_LengthSim;					
			m_Params[3] = mi_FracCriticalSpeedSim;		
			m_Params[4] = mi_FracLoadSim;				
			m_Params[5] = mi_WorkIndexSim;				

			// MODEL PARAMETERS
			m_Params[6] = mi_DiameterDerived;			
			m_Params[7] = mi_LengthDerived;				
			m_Params[8] = mi_FracCriticalSpeedDerived;	
			m_Params[9] = mi_FracLoadDerived;			
			m_Params[10] = mi_WorkIndexDerived;			
			m_Params[11] = mi_MillConstantDerived;	
			m_Params[12] = mi_XC;							
			m_Params[13] = mi_IN;							
			m_Params[14] = mi_SL;							
			m_Params[15] = mi_F90Derived;	
			
			for (int i = 0; i < 31; i++)
			{
				 m_Params[16+i] = mi_A[i];
			}
			for (int i = 0; i < 31; i++)
			{
				 m_Params[47+i] = mi_C[i];
			}
			
			RioTintoTS::VectorView ParamVec(m_Params,78,1);
			Mill.Initialize(MatInfo,ParamVec);

			// Create the Feed Stream. Use SetConfig after initialisation
			// if we need to change anything. We need to initialise here
			// because we get the size information from the Feed stream
			FeedStream = RioTintoTS::FlowStream1::Create( MatInfo );
      }

      if (!IsNothing(l_PSD))
	  {

        // Copy the input size data to the system feed stream solids
        SysCADSystemHelper::SysCADSolidsToSystem(Feed,FeedStream);
        SysCADSystemHelper::SysCADLiquidToSystem(Feed,FeedStream);

        // Execute the Top Deck Model
        Mill.CalculateModel( FeedStream );

		// Map TS model product to SysCAD product
        SysCADSystemHelper::SystemSolidsToSysCAD(Product,Mill.Discharge);
        SysCADSystemHelper::SystemLiquidToSysCAD(Product,Mill.Discharge);

		// Set display outputs
		mo_MCSim				= Mill.ModelOutput[0];
		mo_F90Sim				= Mill.ModelOutput[1];
		mo_Out_ChangeNStages	= Mill.ModelOutput[2];
		mo_NBreakageStagesSim	= Mill.ModelOutput[3];
		for (int i = 0; i < 31 ; i++ )
			mo_S[i] = Mill.ModelOutput[4+i];

	  }
}

//====================================================================================