//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __AGSAGMill_CPP
#include "SysCADSystemHelper.h"
#include "AGSAGMill.h"
//#pragma optimize("", off)

//====================================================================================
//             AGSAG Mill
//====================================================================================

   CMill_AGSAG::CMill_AGSAG()
   {

	// Construct the Persistent System Feed Data
    FeedStream  = RioTintoTS::PFlowStream1( new RioTintoTS::FlowStream1 );
    MatInfo     = RioTintoTS::PStreamInfo1( new RioTintoTS::StreamInfo1 );
	//
	// Recycles
	//
		// Sim
//	m_iReference_F80sim		= 77.95/3.6;
//	m_iNew_Feedsim			= 464.9/3.6;
//	m_iNew_FeedFracsim		= 0.0;
//	m_iNew_FeedRecyclesim	= 0.0;
//	m_oRecyclesim			= 0.0;
//	m_oRecycleratiosim		= 0.0;
//	m_iUseRecycles			= false;

		// Org
//	m_iReference_F80org		= 77.95/3.6;
//	m_iNew_Feedorg			= 464.9/3.6;
//	m_oRecycleorg			= 0.0;
//	m_oRecycleratioorg	    = 0.0;

	//
	// Dimensions
	//
			// Sim
	m_iDiametersim				= 3.9;
	m_iBellyLengthsim			= 7.2;
	m_iFeedTrunionDiasim		= 2.0;
	m_iFeedConeAnglesim			= 0.0;
	m_iDischargeConeAnglesim	= 0.0;
	m_ixgsim					= 125.0/1000.0;
	m_ixmsim					= 0.7/1000.0;
	m_iGrateOpenAreasim			= 0.5;
	m_iPebblePortOpenAreasim	= 0.0;
	m_iPebblePortSizesim		= 0.0;
	m_iMeanRelRadialPossim		= 0.25;

		// Org
	/*
	m_iDiameterorg				= 3.9;
	m_iBellyLengthorg			= 7.2;
	m_iFeedTrunionDiaorg		= 2.0;
	m_iFeedConeAngleorg			= 0.0;
	m_iDischargeConeAngleorg	= 0.0;
	m_ixgorg					= 125.0/1000.0;
	m_ixmorg					= 0.7/1000.0;
	m_iGrateOpenAreaorg			= 0.5;
	m_iPebblePortOpenAreaorg	= 0.0;
	m_iPebblePortSizeorg		= 0.0;
	m_iMeanRelRadialPosorg		= 0.25;
    */

	//
	// Ball Load
	//
	m_iBallLoadsim				= 0.0;
	//m_oBallLoadsim				= 0.0;
	m_iBallTopSizesim			= 100.0/1000;

	//m_iBallLoadorg				= 0.0;
	//m_oBallLoadorg				= 0.0;
	//m_iBallTopSizeorg			= 100.0/1000;

	m_iBallSize1sim				= 54.0/100.0;
	m_iBallSize2sim				= 27.0/100.0;
	m_iBallSize3sim				= 13.0/100.0;
	m_iBallSize4sim				= 6.0/100.0;

	//m_iBallSize1org				= 54.0/100.0;
	//m_iBallSize2org				= 27.0/100.0;
	//m_iBallSize3org				= 13.0/100.0;
	//m_iBallSize4org				= 6.0/100.0;

	//m_oOreSize1					= 0.0;
	//m_oOreSize2					= 0.0;
	//m_oOreSize3					= 0.0;
	//m_oOreSize4					= 0.0;
	//m_oCalcSubsize				= 0.0;

	//
	// Breakage Rates
	//
	m_iKnot1					= 0.25/1000.0;
	m_iKnot2					= 4.0/1000.0;
	m_iKnot3					= 16.0/1000.0;
	m_iKnot4					= 44.8/1000.0;
	m_iKnot5					= 128.0/1000.0;

	m_iRate1sim					= 0.23;
	m_iRate2sim					= 2.86;
	m_iRate3sim					= 1.48;
	m_iRate4sim					= 3.59;
	m_iRate5sim					= 0.0;

	//m_oRate1org					= 0.0;
	//m_oRate2org					= 0.0;
	//m_oRate3org					= 0.0;
	//m_oRate4org					= 0.0;
	//m_oRate5org					= 0.0;

	//
	// Breakage Function
	//
	//m_iAsim						= 70.2;
	//m_iBsim						= 3.28;
	//m_itasim					= 2.19;

	//m_iAorg						= 70.2;
	//m_iBorg						= 3.28;
	//m_itaorg					= 2.19;

	//m_oEcs						= 0.0;

	//
	// Mass XFER
	//
	m_iDischargeCoeffsim		= 26000;
	m_im1sim				    = 0.502;
	m_im2sim					= 0.5;
	m_iCoarseFactorsim			= 1.2;
	//m_oMaxMillCapacitysim		= 102.6;
	//m_iUserTotLoadsim			= 18.81;
	//m_oMaxDesignFlowsim			= 126.4;

	//m_iDischargeCoefforg		= 26000;
	//m_om1org					= 0.502;
	//m_om2org					= 0.500;
	//m_oCoarseFactororg			= 1.200;

	//
	// Power
	//

	m_iFracCriticalSpeedsim		= 0.65;
	m_iBallSGsim				= 7.8*1000.0;
	m_iPorositysim				= 0.4;
	m_iPowerAdjustsim			= 1.26;
	//m_oChargeDensitysim			= 2.165*1000;
	//m_oVoidFractionsim			= 1.0;
	m_iNoLoadPower				= 96.96;
	//m_iMeasuredPower			= 407.0;
	//m_iExpVolTotalLoad			= 0.0;
	//m_oCalVolTotalLoad			= 17.25;
	//m_iExpVolTotalLoad			= 1.00;
	//m_iErrVolTotalLoad			= 17.25;


	//m_iFracCriticalSpeedorg		= 0.65;
	//m_iBallSGorg				= 7.8;
	//m_iPorosityorg				= 0.4;
	//m_iPowerAdjustorg			= 1.26;

	//
	// Ore Load
	//
	m_oGrossPower				 = 0.0;
	m_oAverageSizeTop20  = 0.0;
	m_oMillPulpLoad			 = 0.0;

	}

//====================================================================================

void CMill_AGSAG::BuildDataFields(MDataDefn &DB)
  { 
  DB.ObjectBegin("TS_AGSAG", "SAG");

  #ifdef Recycles
  DB.Page("Recycles");
  DB.ObjectBegin("AGSAG_Recycles" , "Recycles" ); // Table Name, TagName
  DB.StructBegin("Sim");
  DB.Double("F80 Ref"					,"",&m_iReference_F80sim	,MF_PARAMETER			);
  DB.Double("New Feed"				,"",&m_iNew_Feedsim			,MF_PARAMETER	,MC_Qm	);
  DB.Double("New Feed % (-20+4mm)"	,"",&m_iNew_FeedFracsim		,MF_PARAMETER	,MC_Frac);
  DB.Double("Fixed Recycle (-20+4mm)" ,"",&m_iNew_FeedRecyclesim	,MF_PARAMETER	,MC_Qm	);
  DB.Double("Recycle (-20+4mm)"		,"",&m_oRecyclesim			,MF_RESULT		,MC_Qm	);
  DB.Double("Recycle ratio (-20+4mm)"	,"",&m_oRecycleratiosim		,MF_RESULT		,MC_Frac);
  DB.CheckBox("Use Recycles"			,"",&m_iUseRecycles			,MF_PARAMETER			);
  DB.StructEnd();
  DB.StructBegin("Org");
  DB.Double("F80 Ref"					,"",&m_iReference_F80org	,MF_PARAMETER			);
  DB.Double("New Feed"				,"",&m_iNew_Feedorg			,MF_PARAMETER	,MC_Qm	);
  DB.Double("Recycle (-20+4mm)"		,"",&m_oRecycleorg			,MF_RESULT		,MC_Qm	);
  DB.Double("Recycle ratio (-20+4mm)"	,"",&m_oRecycleratioorg		,MF_RESULT		,MC_Frac);
  DB.StructEnd();
  DB.ObjectEnd(); // AGSAG_Recycles
  #endif

  DB.Page("Dimensions");
  DB.ObjectBegin("AGSAG_Dimensions","Dimensions"); // Table Name, TagName
  DB.StructBegin("Sim");
  DB.Double("Diameter (inside liners)"     ,"Diameter"			,&m_iDiametersim			,MF_PARAMETER	,MC_L	);
  DB.Double("Belly Length (inside liners)" ,"Belly_Length"		,&m_iBellyLengthsim			,MF_PARAMETER	,MC_L	);
  DB.Double("Feed Trunion Diameter"        ,"Trunion_Diameter"	,&m_iFeedTrunionDiasim		,MF_PARAMETER	,MC_L	);
  DB.Double("Feed end Cone Angle"          ,"Feed_Cone_Angle"		,&m_iFeedConeAnglesim		,MF_PARAMETER	,MC_Ang	);
  DB.Double("Discharge end Cone Angle"     ,"Discharge_Cone_Angle",&m_iDischargeConeAnglesim	,MF_PARAMETER	,MC_Ang	);
  DB.Double("Grate Size"					 ,"XG"					,&m_ixgsim					,MF_PARAMETER	,MC_L	);
  DB.Double("Fine Size"					 ,"XF"					,&m_ixmsim					,MF_PARAMETER	,MC_L	);
  DB.Double("Grate Open Area Frac"		 ,"Grate_OA"			,&m_iGrateOpenAreasim		,MF_PARAMETER	,MC_Frac);
  DB.Double("Pebble Port Open Area Frac"	 ,"Pebble_Port_OA"		,&m_iPebblePortOpenAreasim	,MF_PARAMETER	,MC_Frac);
  DB.Double("Pebble Port Size"			 ,"Pebble_Port_Size"	,&m_iPebblePortSizesim		,MF_PARAMETER	,MC_L);
  DB.Double("Mean rel radial pos grate app","Mean_Radial_Pos"		,&m_iMeanRelRadialPossim	,MF_PARAMETER	);
  DB.StructEnd();
  /*
  DB.StructBegin("Org");
  DB.Double("Diameter (inside liners)"     ,"",&m_iDiameterorg			,MF_PARAMETER	,MC_L	);
  DB.Double("Belly Length (inside liners)" ,"",&m_iBellyLengthorg			,MF_PARAMETER	,MC_L	);
  DB.Double("Feed Trunion Diameter"        ,"",&m_iFeedTrunionDiaorg		,MF_PARAMETER	,MC_L	);
  DB.Double("Feed end Cone Angle"          ,"",&m_iFeedConeAngleorg		,MF_PARAMETER	,MC_Ang	);
  DB.Double("Discharge end Cone Angle"     ,"",&m_iDischargeConeAngleorg	,MF_PARAMETER	,MC_Ang	);
  DB.Double("Grate Size"					 ,"",&m_ixgorg					,MF_PARAMETER	,MC_L	);
  DB.Double("Fine Size"					 ,"",&m_ixmorg					,MF_PARAMETER	,MC_L	);
  DB.Double("Grate Open Area Frac"		 ,"",&m_iGrateOpenAreaorg		,MF_PARAMETER	,MC_Frac);
  DB.Double("Pebble Port Open Area Frac"	 ,"",&m_iPebblePortOpenAreaorg	,MF_PARAMETER	,MC_Frac);
  DB.Double("Pebble Port Size"			 ,"",&m_iPebblePortSizeorg		,MF_PARAMETER	,MC_L);
  DB.Double("Mean rel radial pos grate app","",&m_iMeanRelRadialPosorg	,MF_PARAMETER	);
  DB.StructEnd();
  */
  DB.ObjectEnd(); // AGSAG_Dimensions


  DB.Page("Ball Load");
  DB.ObjectBegin("AGSAG_BallLoad","BallLoad"); // Table Name, TagName
  DB.StructBegin("Sim");
  DB.Double("Ball Load"					,"Ball_Load"					,&m_iBallLoadsim				,MF_PARAMETER	,MC_Frac	);
  //DB.Double("Calculated Load"				,"",&m_oBallLoadsim				,MF_RESULT		,MC_M	);
  DB.Double("Ball Top Size"				,"Top_Size"						,&m_iBallTopSizesim			,MF_PARAMETER	,MC_L	);
  DB.StructEnd();
  /*
  DB.StructBegin("Org");
  DB.Double("Ball Load"					,"",&m_iBallLoadorg				,MF_PARAMETER	,MC_Frac	);
  DB.Double("Calculated Load"				,"",&m_oBallLoadorg				,MF_PARAMETER	,MC_M	);
  DB.Double("Ball Top Size"				,"",&m_iBallTopSizeorg			,MF_PARAMETER	,MC_L	);
  DB.StructEnd();
  */

  DB.Text("Ball Charge Sizing");
  DB.StructBegin("Sim");
  DB.Double("Ts/sqrt(2)"					,"TS_1"							,&m_iBallSize1sim			,MF_PARAMETER	,MC_Frac);
  DB.Double("Ts/2"					    ,"TS_2"							,&m_iBallSize2sim			,MF_PARAMETER	,MC_Frac);
  DB.Double("Ts/2*sqrt(2)"				,"TS_3"							,&m_iBallSize3sim			,MF_PARAMETER	,MC_Frac);
  DB.Double("Ts/4"						,"TS_4"							,&m_iBallSize4sim			,MF_PARAMETER	,MC_Frac);
  DB.StructEnd();
  /*
  DB.StructBegin("Org");
  DB.Double("Ts/sqrt(2)"					,"",&m_iBallSize1org			,MF_PARAMETER	,MC_Frac);
  DB.Double("Ts/2"					    ,"",&m_iBallSize2org			,MF_PARAMETER	,MC_Frac);
  DB.Double("Ts/2*sqrt(2)"				,"",&m_iBallSize3org			,MF_PARAMETER	,MC_Frac);
  DB.Double("Ts/4"						,"",&m_iBallSize4org			,MF_PARAMETER	,MC_Frac);
  DB.StructEnd();
  */
  /*
  DB.Text("Equivalent Ore Size");
  DB.Double("Ts/sqrt(2)"					,"",&m_oOreSize1				,MF_RESULT		,MC_L);
  DB.Double("Ts/2"						,"",&m_oOreSize2				,MF_RESULT		,MC_L);
  DB.Double("Ts/2*sqrt(2)"				,"",&m_oOreSize3				,MF_RESULT		,MC_L);
  DB.Double("Ts/4"						,"",&m_oOreSize4				,MF_RESULT		,MC_L);
  DB.Double("Calc Sub Size"				,"",&m_oCalcSubsize				,MF_RESULT		,MC_L);
  */
  DB.ObjectEnd(); // AGSAG_BallLoad

  DB.Page("Breakage Rates");
  DB.ObjectBegin("AGSAG_BreakageRates","BreakageRates"); // Table Name, TagName
  DB.StructBegin("Knot");
  DB.Double("K1"							,"K1"					,&m_iKnot1					,MF_PARAMETER	,MC_L);
  DB.Double("K2"							,"K2"					,&m_iKnot2					,MF_PARAMETER	,MC_L);
  DB.Double("K3"							,"K3"					,&m_iKnot3					,MF_PARAMETER	,MC_L);
  DB.Double("K4"							,"K4"					,&m_iKnot4					,MF_PARAMETER	,MC_L);
  DB.Double("K5"							,"K5"					,&m_iKnot5					,MF_PARAMETER	,MC_L);
  DB.StructEnd();

  DB.StructBegin("Sim");
  DB.Double("R1"							,"R1"				,&m_iRate1sim					,MF_PARAMETER		);
  DB.Double("R2"							,"R2"				,&m_iRate2sim					,MF_PARAMETER		);
  DB.Double("R3"							,"R3"				,&m_iRate3sim					,MF_PARAMETER		);
  DB.Double("R4"							,"R4"				,&m_iRate4sim					,MF_PARAMETER		);
  DB.Double("R5"							,"R5"				,&m_iRate5sim					,MF_PARAMETER		); 
  DB.StructEnd();
  /*
  DB.StructBegin("Org");
  DB.Double("R1"				,"",&m_oRate1org					,MF_RESULT		);
  DB.Double("R2"				,"",&m_oRate2org					,MF_RESULT		);
  DB.Double("R3"				,"",&m_oRate3org					,MF_RESULT		);
  DB.Double("R4"				,"",&m_oRate4org					,MF_RESULT		);
  DB.Double("R5"				,"",&m_oRate5org					,MF_RESULT		); 
  DB.StructEnd();
  */
  DB.ObjectEnd(); // AGSAG_BreakageRates

  /*
  DB.Page("Breakage Function");
  DB.ObjectBegin("AGSAG_BreakageFunction","BreakageFunction"); // Table Name, TagName
  DB.StructBegin("Sim");
  DB.Double("A"						,"",&m_iAsim						,MF_PARAMETER		);
  DB.Double("B"						,"",&m_iBsim						,MF_PARAMETER		);
  DB.Double("Abrasion ta"				,"",&m_itasim						,MF_PARAMETER		);
  DB.StructEnd();
  DB.StructBegin("Org");
  DB.Double("A"						,"",&m_iAorg						,MF_PARAMETER		);
  DB.Double("B"						,"",&m_iBorg						,MF_PARAMETER		);
  DB.Double("Abrasion ta"				,"",&m_itaorg						,MF_PARAMETER		);
  DB.StructEnd();
  DB.Double("Calculated Ecs"			,"",&m_oEcs							,MF_RESULT		);
  DB.ObjectEnd(); // AGSAG_BreakageFunction
  */
  DB.Page("Mass Transfer");
  DB.ObjectBegin("AGSAG_MassTransfer","MassTransfer"); // Table Name, TagName
  DB.StructBegin("Sim");
  //DB.Double("Discharge Coeff"			,"",&m_iDischargeCoeffsim			,MF_PARAMETER			);
  DB.Double("m1"						,"m1",					&m_im1sim						,MF_PARAMETER  			);
  DB.Double("m2"						,"m2",					&m_im2sim						,MF_PARAMETER			);
  //DB.Double("Coarse Factor"			,"",&m_iCoarseFactorsim				,MF_RESULT  			);
  //DB.Double("Max Mill Capacity"		,"",&m_oMaxMillCapacitysim			,MF_RESULT		,MC_Vol );
  //DB.Double("User design tot load"	,"",&m_iUserTotLoadsim				,MF_PARAMETER	,MC_Frac);
  //DB.Double("Max design flow"			,"",&m_oMaxDesignFlowsim			,MF_RESULT		,MC_Vol );
  DB.StructEnd();
  /*
  DB.StructBegin("Org");
  DB.Double("Discharge Coeff"			,"",&m_iDischargeCoefforg			,MF_PARAMETER			);
  DB.Double("m1"						,"",&m_om1org						,MF_RESULT  			);
  DB.Double("m2"						,"",&m_om2org						,MF_RESULT				);
  DB.Double("Coarse Factor"			,"",&m_oCoarseFactororg				,MF_RESULT  			);
  DB.StructEnd();
  */
  DB.ObjectEnd(); // AGSAG_MassTransfer

  DB.Page("Power");
  DB.ObjectBegin("AGSAG_Power","Power"); // Table Name, TagName
  DB.StructBegin("Sim");
  DB.Double("Frac Critical Speed"			  ,"Frac_Critical_Speed", &m_iFracCriticalSpeedsim			,MF_PARAMETER , MC_Frac);
  DB.Double("Ball Specific Gravity"		  ,"Ball_SG", &m_iBallSGsim						,MF_PARAMETER , MC_Rho);
  //DB.Double("Ball and rock charge porosity" ,"",&m_iPorositysim					,MF_PARAMETER , MC_Frac);
  DB.Double("Net power adjustment factor"	  ,"NPadj", &m_iPowerAdjustsim				,MF_PARAMETER );
  //DB.Double("Total Charge Density"		  ,"",&m_oChargeDensitysim				,MF_RESULT	  , MC_Rho);
  //DB.Double("Void fill fraction"			  ,"",&m_oVoidFractionsim				,MF_RESULT	  , MC_Frac);
  DB.Double("NoLoadPower", "NLP", &m_iNoLoadPower					,MF_PARAMETER ,MC_Pwr);
  //DB.Double("Measured Power"				  ,"",&m_iMeasuredPower					,MF_PARAMETER ,MC_Pwr);
  //DB.Double("Exp Vol total load"			  ,"",&m_iExpVolTotalLoad			    ,MF_PARAMETER ,MC_Frac);
  //DB.Double("Cal Vol total load"			  ,"",&m_oCalVolTotalLoad			    ,MF_RESULT    ,MC_Frac);
  //DB.Double("SD Vol total load"			  ,"",&m_iSDVolTotalLoad			    ,MF_PARAMETER ,MC_Frac);
  //DB.Double("Err Vol total load (exp - cal)","",&m_iErrVolTotalLoad			    ,MF_RESULT    ,MC_Frac);
  DB.StructEnd();
  /*
  DB.StructBegin("Org");
  DB.Double("Frac Critical Speed"			  ,"",&m_iFracCriticalSpeedorg			,MF_PARAMETER , MC_Frac);
  DB.Double("Ball specific gravity"		  ,"",&m_iBallSGorg						,MF_PARAMETER , MC_Rho);
  DB.Double("Ball and rock charge porosity" ,"",&m_iPorosityorg					,MF_PARAMETER , MC_Frac);
  DB.Double("Net power adjustment factor"	  ,"",&m_iPowerAdjustorg				,MF_PARAMETER );
  DB.StructEnd();
  */
  DB.ObjectEnd(); // AGSAG_Power

	//  MODEL CALCULATED VALUES
	//  ~~~~~~~~~~~~~~~~~~~~~~~
	//DB.Page("Calculated Parameters");
  //DB.ObjectBegin("AGSAG_CalculatedParameters","CalculatedParameters"); // Table Name, TagName
  //DB.ObjectEnd(); // AGSAG_CalculatedParameters

  DB.Page("Ore Load");
  DB.ObjectBegin("AGSAG_OreLoad","OreLoad"); // Table Name, TagName
  DB.Double("AverageSize_of_Top_20%", "", &m_oAverageSizeTop20, MF_RESULT, MC_L);
  DB.Double("MillPulpLoad", "", &m_oMillPulpLoad, MF_RESULT, MC_Pwr);//MC_Frac);
  DB.Double("GrossPower", "", &m_oGrossPower, MF_RESULT, MC_Pwr);
  DB.ObjectEnd();

  DB.ObjectEnd();
	}

//====================================================================================

void CMill_AGSAG::EvalProducts(MBaseMethod &M, MStream &Feed, MStream &Product, bool bInit)
{ 
#if WithFortranModel430
  // Get info on Size Distribution
  MIPSD & l_PSD=*Feed.FindIF<MIPSD>();
  if (IsNothing(l_PSD))
    {
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
    SysCADSystemHelper::PopulateMaterialInfo(M, MatInfo);

		// Initialize Rod Mill           
		
		/*Config->SAG_NumMills*/				m_Params[0]		= 1;       
		/*Config->SAG_Diameter*/				m_Params[1]		= m_iDiametersim;         
		/*Config->SAG_Length*/					m_Params[ 2]	= m_iBellyLengthsim;
		/*Config->SAG_Trunion_D*/				m_Params[ 3]	= m_iFeedTrunionDiasim;
		/*Config->SAG_Feed_Cone_Angle*/			m_Params[ 4]	= m_iFeedConeAnglesim;
		/*Config->SAG_Disch_Cone_Angle*/		m_Params[ 5]	= m_iDischargeConeAnglesim;
		/*Config->SAG_Grate_Size*/				m_Params[ 6]	= m_ixgsim*1000.0;//mm
		/*Config->SAG_Fine_Size*/				m_Params[ 7]	= m_ixmsim*1000.0;//mm
		/*Config->SAG_Grate_OAF*/				m_Params[ 8]	= m_iGrateOpenAreasim;
		/*Config->SAG_PP_OAF*/					m_Params[ 9]	= m_iPebblePortOpenAreasim;
		/*Config->SAG_PP_Size*/					m_Params[ 10]	= m_iPebblePortSizesim*1000.0;//mm
		/*Config->SAG_Mean_Radial_Position*/	m_Params[ 11]	= m_iMeanRelRadialPossim;
		/*Config->SAG_Fr_CriticalSpeed*/		m_Params[ 12]	= m_iFracCriticalSpeedsim;
		/*Config->SAG_Ball_Load_by_Volume*/		m_Params[ 13]	= m_iBallLoadsim;
		/*Config->SAG_No_Load_Power*/			m_Params[ 14]   = m_iNoLoadPower;
		/*Config->SAG_Feed_Solids_Density*/		m_Params[ 15]   = 0.0; // Need to add parameter for this D T option

		/*Parameters->SAG_No_of_Knots*/			m_Params[ 16]   = 5;        

 		/*Parameters->SAG_Spline_Size_Knots.resize(Parameters->SAG_No_of_Knots);*/
 		/*Parameters->SAG_Breakage_Rates.resize(Parameters->SAG_No_of_Knots);*/
		/*Parameters->SAG_Spline_Size_Knots[0]*/	m_Params[ 17] = m_iKnot1*1000.0; //mm
		/*Parameters->SAG_Spline_Size_Knots[1]*/	m_Params[ 18] = m_iKnot2*1000.0; //mm
		/*Parameters->SAG_Spline_Size_Knots[2]*/	m_Params[ 19] = m_iKnot3*1000.0; //mm
		/*Parameters->SAG_Spline_Size_Knots[3]*/	m_Params[ 20] = m_iKnot4*1000.0; //mm
		/*Parameters->SAG_Spline_Size_Knots[4]*/	m_Params[ 21] = m_iKnot5*1000.0; //mm

		/*Parameters->SAG_Breakage_Rates[0]*/		m_Params[ 22] = m_iRate1sim; 
		/*Parameters->SAG_Breakage_Rates[1]*/		m_Params[ 23] = m_iRate2sim; 
		/*Parameters->SAG_Breakage_Rates[2]*/		m_Params[ 24] = m_iRate3sim; 
		/*Parameters->SAG_Breakage_Rates[3]*/		m_Params[ 25] = m_iRate4sim; 
		/*Parameters->SAG_Breakage_Rates[4]*/		m_Params[ 26] = m_iRate5sim; 

		/*Parameters->SAG_Disch_Coeff*/				m_Params[ 27] = m_iDischargeCoeffsim;
		/*Parameters->SAG_Param_M1*/				  m_Params[ 28] = m_im1sim;
		/*Parameters->SAG_Param_M2*/				  m_Params[ 29] = m_im2sim;
		/*Parameters->SAG_Coarse_Factor*/			m_Params[ 30] = m_iCoarseFactorsim;
		/* design load ??? */
		/*Parameters->SAG_Charge_Porosity*/			m_Params[ 31] = m_iPorositysim;
		/*Parameters->SAG_Net_Power_Factor*/		m_Params[ 32] = m_iPowerAdjustsim;

		/*BallCharge->Ball_SG*/						  m_Params[ 33] = m_iBallSGsim/1000.0;// t/m3
		/*BallCharge->Ball_TopSize*/				m_Params[ 34] = m_iBallTopSizesim*1000.0; // mm
		/*BallCharge->Ball_Size1*/					m_Params[ 35] = m_iBallSize1sim*100.0; //%
		/*BallCharge->Ball_Size2*/					m_Params[ 36] = m_iBallSize2sim*100.0; //%
		/*BallCharge->Ball_Size3*/					m_Params[ 37] = m_iBallSize3sim*100.0; //%
		/*BallCharge->Ball_Size4*/					m_Params[ 38] = m_iBallSize4sim*100.0; //%


		RioTintoTS::VectorView ParamVec(m_Params,39,1);
		Mill.Initialize(MatInfo, ParamVec);

    if (MatInfo->nSize()>MaxOriginalSizes)
      {
      M.Log.Message(MMsg_Error, "Too many size intervals for Mill model (max allowed %d)", MaxOriginalSizes);
      }
    if (MatInfo->nType()>MaxFortranMatType)
      {
      M.Log.Message(MMsg_Error, "Too many material types with PSD for Mill model (max allowed %d)", MaxFortranMatType);
      }

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
    m_oAverageSizeTop20 = Mill.out_ModelOutput[6];
    m_oMillPulpLoad = Mill.out_ModelOutput[17];
    m_oGrossPower = Mill.out_ModelOutput[18];

    #ifdef NEVER
    mo_MCSim				= Mill.ModelOutput[0];
    mo_F90Sim				= Mill.ModelOutput[1];
    mo_Out_ChangeNStages	= Mill.ModelOutput[2];
    mo_NBreakageStagesSim	= Mill.ModelOutput[3];
    for (int i = 0; i < 31 ; i++ )
	    mo_S[i] = Mill.ModelOutput[4+i];
    #endif

	  }
#endif
}

//====================================================================================