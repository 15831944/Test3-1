//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __BallMill_CPP
#include "SysCADSystemHelper.h"
#include "BallMill.h"
//#pragma optimize("", off)

//====================================================================================
//             Ball Mill
//====================================================================================

   CMill_Ball::CMill_Ball()
	{

	// Construct the Persistent System Feed Data
    FeedStream  = RioTintoTS::PFlowStream1( new RioTintoTS::FlowStream1 );
    MatInfo     = RioTintoTS::PStreamInfo1( new RioTintoTS::StreamInfo1 );

    // Initialise parameters
	mi_NParrallelMills =			1;
	mi_DiameterSim =				3; //m 
	mi_LengthSim =					4; //m
	mi_FracCriticalSpeedSim =		0.7;
	mi_FracLoadSim =				0.4;
	mi_WorkIndexSim =				12.0*3.6; //kWs/kg
	mi_BallTopSizeSim =				50.0/1000.0; // m

	// MODEL PARAMETERS
	mi_DiameterDerived =			3; //m

	mi_FracCriticalSpeedDerived =	0.7;
	mi_FracLoadDerived =			0.4;
	mi_WorkIndexDerived =			12.0*3.6; //kWs/kg
	mi_K =							4.4e-4;
	mi_BallTopSizeDerived =			50.0/1000.0; // m

	
	for (int i = 0; i < 31; i++)
	{
		mi_A[i] = 0.0;
		mi_KnotSize[i] = 0.0;
		mi_KnotLnRDStar[i] = 0.0;
		mo_KnotLnRDStar[i]= 0.0;
		mo_RDStar[i] = 0.0;
	}
	mi_NumSplineKnots = 3;
	mi_KnotSize[0] =  0.2/1000.0; //mm
	mi_KnotSize[1] =  1.5/1000.0; //mm
	mi_KnotSize[2] = 15.0/1000.0; //mm
	mi_KnotSize[3] =  0.0; //mm
	mi_KnotLnRDStar[0] = 1.5;
	mi_KnotLnRDStar[1] = 2.5;
	mi_KnotLnRDStar[2] = 3.0;
	mi_KnotLnRDStar[3] = 0.0;

	mi_A[0] =	0.0;	mi_A[1] =	0.193;	mi_A[2] =	0.157;	mi_A[3] =	0.126;	mi_A[4] =	0.101;	mi_A[5] =	0.082;	mi_A[6] =	0.066;	mi_A[7] =   0.053;	mi_A[8] =   0.043;	mi_A[9] =   0.035;	mi_A[10] =  0.028;	mi_A[11] =  0.022;	mi_A[12] =  0.018;	mi_A[13] =  0.015;	mi_A[14] =  0.012;	mi_A[15] =  0.01;	mi_A[16] =  0.008;	mi_A[17] =  0.006;	mi_A[18] =  0.004;	mi_A[19] =  0.003;	mi_A[20] =  0.002;	mi_A[21] =  0.0015;	mi_A[22] =  0.0010;	mi_A[23] =  0.0005;

	}

//====================================================================================

	void CMill_Ball::BuildDataFields(MDataDefn &DB)
	
	{ 
    DB.ObjectBegin("TS_Ball", "Ball");
	DB.Page("Ball Mill Parameters");
	DB.Text("Simulated Mill");
	// NUMBER OF PARALLEL MILLS
    DB.Long("NMills",        "", &mi_NParrallelMills , MF_PARAMETER );	

	//DIAMETER (INSIDE LINERS, METRES) OF MILL BEING SIMULATED.
    DB.Double("Dsim",        "", &mi_DiameterSim	, MF_PARAMETER ,  MC_L);

	// LENGTH (INSIDE LINERS, METRES) OF MILL BEING SIMULATED.
    DB.Double("Lsim",        "", &mi_LengthSim	, MF_PARAMETER ,  MC_L);

	// FRACTION CRITICAL SPEED OF MILL BEING SIMULATED.
    DB.Double("CSsim",        "", &mi_FracCriticalSpeedSim	, MF_PARAMETER ,  MC_Frac);

	// LOAD FRACTION (SEE ABOVE) OF MILL BEING SIMULATED.
    DB.Double("LFsim",        "", &mi_FracLoadSim	, MF_PARAMETER ,  MC_Frac);

	// WORK INDEX OF ORE FED TO MILL BEING SIMULATED.
    DB.Double("WIsim",        "", & mi_WorkIndexSim	, MF_PARAMETER , MC_WI ); // Units??

	//  Simulated Mill Ball Top Size
    DB.Double("BallSizesim",  "", & mi_BallTopSizeSim	, MF_PARAMETER ,  MC_L );


	// MODEL PARAMETERS
	//  ~~~~~~~~~~~~~~~~
	DB.Text("Original Mill");
	// MILL DIAMETER (INSIDE LINERS, METRES) OF MILL FROM
	//         WHICH MODEL PARAMETERS ARE DERIVED.
    DB.Double("Dfit",        "", &mi_DiameterDerived	, MF_PARAMETER ,  MC_L);

	// FRACTION OF CRITICAL SPEED OF MILL FROM WHICH MODEL
	//                 PARAMETERS ARE DERIVED.
    DB.Double("CSfit",        "", &mi_FracCriticalSpeedDerived	, MF_PARAMETER ,  MC_Frac);

	// LOAD FRACTION (I.E. FRACTION OF MILL OCCUPIED BY
	//         STATIONARY LOAD, INCLUDING VOIDS) OF MILL FROM
	//         WHICH MODEL PARAMETERS ARE DERIVED.
    DB.Double("LFfit",        "", &mi_FracLoadDerived	, MF_PARAMETER ,  MC_Frac);

	//  ORE WORK INDEX OF MILL FROM WHICH MODEL PARAMETERS ARE
	//                 DERIVED.
    DB.Double("WIfit",        "", & mi_WorkIndexDerived	, MF_PARAMETER , MC_WI); // Units??

    DB.Double("BreakageRateFactor",        "", & mi_K	, MF_PARAMETER );


	//  Mill Ball Top Size
    DB.Double("BallSizefit",  "", & mi_BallTopSizeDerived	, MF_PARAMETER ,  MC_L );

	DB.Page("Rate/Discharge Function");
    DB.Long("NSplineKnots",  "", &mi_NumSplineKnots , MF_PARAMETER );	
	if (mi_NumSplineKnots<3) mi_NumSplineKnots = 3;
	if (mi_NumSplineKnots > 30) mi_NumSplineKnots = 30;
	DB.Text("Size - Enter in units of um");
	DB.Text("Size");
	for (int i = 0; i < (mi_NumSplineKnots+1); i++)
	{
		char buff[20];
		sprintf(buff,"K%d_Size",i);
		DB.Double( buff ,  "", &mi_KnotSize[i]	, MF_PARAMETER ,  MC_L );
	}
	DB.Text("Ln R/D* (meas)");
	for (int i = 0; i < (mi_NumSplineKnots+1); i++)
	{
		char buff[20];
		sprintf(buff,"K%d_LnRDm",i);
		DB.Double( buff ,  "", &mi_KnotLnRDStar[i]	, MF_PARAMETER );

	}
	DB.Text("Ln R/D* (calc)");
	for (int i = 0; i < (mi_NumSplineKnots+1); i++)
	{
		char buff[20];
		sprintf(buff,"K%d_LnRDc",i);
		DB.Double( buff ,  "", &mo_KnotLnRDStar[i]	, MF_RESULT );
	}

	DB.Page("Appearance Data");
	for (int i = 0; i < 31; i++)
	{
		char buff[20];
		sprintf(buff,"A%d",i);
		DB.Double(buff,        "", &mi_A[i]	, MF_PARAMETER );
	}

	DB.Page("R/D*");
	for (int i = 0; i < 31; i++)
	{
		char buff[20];
		sprintf(buff,"RDS%d",i);
		DB.Double(buff,        "", &mo_RDStar[i]	, MF_RESULT );
	}

	DB.Page("Scale");
	DB.Double("FACTA","", &mo_FACTA , MF_RESULT );
	DB.Double("FACTB","", &mo_FACTB , MF_RESULT );
	DB.Double("FACTC","", &mo_FACTC , MF_RESULT );
	DB.Double("FACTD","", &mo_FACTD , MF_RESULT );
	DB.Double("FACTE<Xm","", &mo_FACTEsizeltXm , MF_RESULT );
	DB.Double("FACTE>Xm","", &mo_FACTEsizegeXm , MF_RESULT );
	DB.Double("Xm","", &mo_Xm , MF_RESULT );

    DB.ObjectEnd();

	}

//====================================================================================

void CMill_Ball::EvalProducts(MBaseMethod &M,
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

			// Initialize Ball Mill
			m_Params[0] = mi_NParrallelMills;
			m_Params[1] = mi_DiameterSim;
	        m_Params[2] = mi_LengthSim;
	        m_Params[3] = mi_FracCriticalSpeedSim;
	        m_Params[4] = mi_FracLoadSim;
	        m_Params[5] = mi_WorkIndexSim;
	        m_Params[6] = mi_BallTopSizeSim;
	        m_Params[7] = mi_DiameterDerived;
	        m_Params[8] = mi_LengthDerived;
	        m_Params[9] = mi_FracCriticalSpeedDerived;
	        m_Params[10] = mi_FracLoadDerived;
	        m_Params[11] = mi_WorkIndexDerived;
	        m_Params[12] = mi_BallTopSizeDerived;
	        m_Params[13] = mi_K;
			m_Params[14] = mi_NumSplineKnots;
			for ( int i = 0 ; i < 31 ; i++ )
				m_Params[15+i] = mi_KnotSize[i];
			for ( int i = 0 ; i < 31 ; i++ )
				m_Params[46+i] = mi_KnotLnRDStar[i];
			for ( int i = 0 ; i < 31 ; i++ )
				m_Params[77+i] = mi_A[i];

			RioTintoTS::VectorView ParamVec(m_Params,108,1);
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
		for ( int i = 0 ; i < 31 ; i++ )
			mo_RDStar[i] = Mill.ModelOutput[i];
		for ( int i = 0 ; i < 31 ; i++ )
			mo_KnotLnRDStar[i] = Mill.ModelOutput[31+i];
		mo_FACTA		=Mill.ModelOutput[62];
		mo_FACTB		=Mill.ModelOutput[63];
		mo_FACTC		=Mill.ModelOutput[64];
		mo_FACTD		=Mill.ModelOutput[65];
		mo_FACTEsizeltXm=Mill.ModelOutput[66];
		mo_FACTEsizegeXm=Mill.ModelOutput[67];
		mo_Xm=Mill.ModelOutput[68];
	  }

}

//====================================================================================