//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __Model400Comminution_CPP
#include "SysCADSystemHelper.h"
#include "Model400Comminution.h"
#pragma optimize("", off)

//====================================================================================
//             Model400 Comminution
//====================================================================================

   CComminution_Model400::CComminution_Model400()
	{

	// Construct the Persistent System Feed Data
    FeedStream  = RioTintoTS::PFlowStream1( new RioTintoTS::FlowStream1 );
    MatInfo     = RioTintoTS::PStreamInfo1( new RioTintoTS::StreamInfo1 );

	m_dK1 = 55.0;
	m_dK2 = 55.0;
	m_dK3 = 2.3;
	m_dEcs = 0.6;

    m_dCSS=                  0.02;  
    m_dLLen=                 0;
    m_dET=                   0;
    m_dLHr=                  0;
    m_dMotorPower=           500;
    m_dNoLoadPower=          100;
    m_dA0=                   1; 
    m_dA1=                   0;
    m_dA2=                   0;
    m_dA3=                   0;
    m_dA4=                   0;
    m_dB0=                   1;
    m_dB1=                   0;
    m_dB2=                   0;
    m_dB3=                   0;
    m_dB4=                   0;
    m_dB5=                   30;
    m_dC0=                   2.3; 
    m_dD0=                   0.1; 
    m_dD1=                   0;
    m_dD2=                   0;
    m_dE0=                   0;
    m_dE1=                   0;
    m_dF0=                   0;
    m_dF1=                   0;
    m_dF2=                   0;

	mo_dTPH = 0.0;
	mo_dCapacity = 0.0;
	mo_dUtilisation = 0.0;
	mo_dF80 = 0.0;
	mo_dP80 = 0.0;
	mo_dk1 = 0.0;
	mo_dk2 = 0.0;
	mo_dk3 = 0.0;
	mo_dEcs = 0.0;
	mo_dNetPower = 0.0;
	mo_dGrossPower = 0.0;

	}

//====================================================================================

	void CComminution_Model400::BuildDataFields(MDataDefn &DB)
	
	{ 
    DB.ObjectBegin("TS_Model400", "Model400");


	DB.Page("Model400 Crusher Parameters");

    DB.Double("K1",        "", &m_dK1	       , MF_PARAMETER );
    DB.Double("K2",        "", &m_dK2	       , MF_PARAMETER );
    DB.Double("K3",        "", &m_dK3	       , MF_PARAMETER );
    DB.Double("Ecs",       "", &m_dEcs	       , MF_PARAMETER );


	//DB.Page("Model400 Crusher Outputs");
	//DB.Double("TPH",		"", &mo_dTPH			,MF_RESULT, MC_Qm );
	//DB.Double("Capacity",	"", &mo_dCapacity		,MF_RESULT, MC_None );
	//DB.Double("Utilisation","", &mo_dUtilisation	,MF_RESULT, MC_Frac );
	//DB.Double("F80",		"", &mo_dF80			,MF_RESULT, MC_L );
	//DB.Double("P80",		"", &mo_dP80			,MF_RESULT, MC_L );
	//DB.Double("k1",			"", &mo_dk1				,MF_RESULT, MC_None );
	//DB.Double("k2",			"", &mo_dk2				,MF_RESULT, MC_None );
	//DB.Double("k3",			"", &mo_dk3				,MF_RESULT, MC_None );
	//DB.Double("Ecs",		"", &mo_dEcs			,MF_RESULT, MC_None );
	//DB.Double("NetPower",	"", &mo_dNetPower		,MF_RESULT, MC_Pwr);
	//DB.Double("GrossPower",	"", &mo_dGrossPower		,MF_RESULT, MC_Pwr);

    DB.ObjectEnd();

	}

//====================================================================================

void CComminution_Model400::EvalProducts(MBaseMethod &M,
										 MStream &Feed , MStream &Product ,
		                                 bool bInit )
	
{ 

	// Get info on Size Distribution
    MIPSD & l_PSD=Feed.IF<MIPSD>(false);
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

			// Initialize Model400 Crusher

			m_dCSS=                  m_dK1;  
			m_dLLen=                 0;
			m_dET=                   0;
			m_dLHr=                  0;
			m_dMotorPower=           500;
			m_dNoLoadPower=          100;
			m_dA0=                   0; 
			m_dA1=                   0;
			m_dA2=                   0;
			m_dA3=                   0;
			m_dA4=                   m_dK1;
			m_dB0=                   0;
			m_dB1=                   0;
			m_dB2=                   0;
			m_dB3=                   0;
			m_dB4=                   0;
			m_dB5=                   m_dK2;
			m_dC0=                   m_dK3; 
			m_dD0=                   m_dEcs; 
			m_dD1=                   0;
			m_dD2=                   0;
			m_dE0=                   0;
			m_dE1=                   0;
			m_dF0=                   0;
			m_dF1=                   0;
			m_dF2=                   0;

			m_Params[0] =  m_dCSS*1000;   //m to mm               
			m_Params[1] =  m_dLLen*1000;  //m to mm   
			m_Params[2] =  m_dET*1000;    //m to mm                  
			m_Params[3] =  m_dLHr/3600.0; //sec to hrs                 
			m_Params[4] =  m_dMotorPower;           
			m_Params[5] =  m_dNoLoadPower;          
			m_Params[6] =  m_dA0;                   
			m_Params[7] =  m_dA1;                   
			m_Params[8] =  m_dA2;                   
			m_Params[9] =  m_dA3;                   
			m_Params[10] =  m_dA4;                   
			m_Params[11] =  m_dB0;                   
			m_Params[12] =  m_dB1;                   
			m_Params[13] =  m_dB2;                   
			m_Params[14] =  m_dB3;                   
			m_Params[15] =  m_dB4;                   
			m_Params[16] =  m_dB5;                   
			m_Params[17] =  m_dC0;                   
			m_Params[18] =  m_dD0;                   
			m_Params[19] =  m_dD1;                   
			m_Params[20] =  m_dD2;                   
			m_Params[21] =  m_dE0;                   
			m_Params[22] =  m_dE1;                   
			m_Params[23] =  m_dF0;                   
			m_Params[24] =  m_dF1;                   
			m_Params[25] =  m_dF2;                 
			RioTintoTS::VectorView ParamVec(m_Params,26,1);
			Crusher.Initialize(MatInfo,ParamVec);

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
        Crusher.CalculateModel( FeedStream );

		// Map TS model product to SysCAD product
        SysCADSystemHelper::SystemSolidsToSysCAD(Product,Crusher.Discharge);
        SysCADSystemHelper::SystemLiquidToSysCAD(Product,Crusher.Discharge);

		// Set display outputs
  		mo_dTPH			= Crusher.ModelOutput[0];
		mo_dCapacity		= Crusher.ModelOutput[1];
		mo_dUtilisation	= Crusher.ModelOutput[2];
		mo_dF80			= Crusher.ModelOutput[3];
		mo_dP80			= Crusher.ModelOutput[4];
		mo_dk1			= Crusher.ModelOutput[5];
		mo_dk2			= Crusher.ModelOutput[6];
		mo_dk3			= Crusher.ModelOutput[7];
		mo_dEcs			= Crusher.ModelOutput[8];
		mo_dNetPower		= Crusher.ModelOutput[9];
		mo_dGrossPower	= Crusher.ModelOutput[10];

	  }

}

//====================================================================================