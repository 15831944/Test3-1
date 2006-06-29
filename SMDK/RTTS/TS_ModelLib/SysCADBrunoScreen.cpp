//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#pragma warning( disable : 4768 )  

#include "stdafx.h"
#define  __SysCADBrunoScreen_CPP
#include "BrunoScreen.h"
#include "SysCADBrunoScreen.h"
#include "SysCADSystemHelper.h"

//====================================================================================
const int idFeed=0;
const int idOversize=1;
const int idUndersize=2;

static MInOutDefStruct s_IODefs[]=
  {
  //  Desc;             Name;          Id; Rqd; Max; CnId, FracHgt;  Options;
    { "Feed",           "Feed",      idFeed,      1,  10,    0,    1.0f,  MIO_In |MIO_Material },
    { "OverSize",       "OverSize",  idOversize,  1,   1,    0,    0.5f,  MIO_Out|MIO_Material },
    { "UnderSize",      "UnderSize", idUndersize, 1,   1,    0,    0.5f,  MIO_Out|MIO_Material },
    { NULL },
  };

long BrunoScreenDraw[] = { NdDrw_Poly,  -4,3,  4,-1,  1,-3,  -4,3,
                           NdDrw_End};

IMPLEMENT_TRANSFERMETHOD(SysCADBrunoScreen, "Bruno Screen", "Size Separation:Bruno Screen", "Data", BrunoScreenDraw, "TS_ModelLib2")

SysCADBrunoScreen::SysCADBrunoScreen()
  {

	// Construct the Persistenet System Feed Data
	FeedStream  = TS_MinProc::PFlowStream1( new TS_MinProc::FlowStream1 );
	SizInfo = TS_MinProc::PSizingInfo1( new TS_MinProc::SizingInfo1 );
	MatInfo =  TS_MinProc::PStreamInfo1( new TS_MinProc::StreamInfo1 );
 
	// Initialisation Flag
	bInit = true;

	// We are using a double array to store our model parameters so they can
	// be overlaid with a VectorView class for passing into the Screen.Initialize routine
	// Hence the type conversions for the longs.

	dParams[0] = 1.0;
	dParams[1] = 1.0;
	dParams[2] = 1.0;
	dParams[3] = 1.0;
	*(long*)(&dParams[4]) = 1;
	*(long*)(&dParams[5]) = 1;
	*(long*)(&dParams[6]) = 1;
	*(long*)(&dParams[7]) = 1;
	*(long*)(&dParams[8]) = 1;
	dParams[9] = 1.0;
  }

//---------------------------------------------------------------------------

void SysCADBrunoScreen::Init()
  {
  SetRunModeOptions(MO_Probal);
  SetIODefinition(s_IODefs);
  }

//---------------------------------------------------------------------------

void SysCADBrunoScreen::BuildDataFields()
  {
  // TO Think about: Should we use a different set of parameters that we copy
  // to the parameter vector to make this cleaner?

  DD.Text("SysCADBrunoScreen Fields...");
  DD.Double("EffectiveArea",     &dParams[0],     MF_PARAMETER, MC_);
  DD.Double("ScreenOpening",     &dParams[1],     MF_PARAMETER, MC_);
  DD.Double("OpenFractionOfEffectiveArea",   &dParams[2],     MF_PARAMETER, MC_);
  DD.Double("FeedBulkDensity",   &dParams[3],     MF_PARAMETER, MC_);
  //DD.Long( "WetScreen (1- true, 0 = false)",   (long*)&dParams[4],    MF_PARAMETER);
  DD.Long( "WetScreen",     (long*)&dParams[4],    MF_PARAMETER);
  DD.Long( "OpeningType",		(long*)&dParams[5],    MF_PARAMETER);
  DD.Long( "ScreenType",		(long*)&dParams[6],    MF_PARAMETER);
  DD.Long( "DeckLocation",	(long*)&dParams[7],    MF_PARAMETER);
  DD.Long( "FeedType",		  (long*)&dParams[8],    MF_PARAMETER);
  DD.Double("CustomerFactor",    &dParams[9],     MF_PARAMETER, MC_);

  // TO DO: Add the Screen.ModelOutput Vector to the DataFields
  }

//---------------------------------------------------------------------------

void SysCADBrunoScreen::EvalProducts()
  {
  try
  {
    if (IsProBal)
    {
      MStream Feed; // SysCAD Feed Stream
      FlwIO.AddMixtureIn_Id(Feed, idFeed); //sum all input streams
      MStream & OS = *FlwIO.Stream[FlwIO.First[idOversize]]; // SysCAD Oversize Output Stream
      MStream & US = *FlwIO.Stream[FlwIO.First[idUndersize]]; // SysCAD Undersize Output Stream

      // Map the SysCAD Parameter Vector to a VectorView
      // TO DO: We should check if the ParamVec changes between calls
      // to EvalProducts and update our initialisation when this occurs
      TS_MinProc::VectorView ParamVec(dParams,1,1);

      MIPSD & PSD=Feed.IF<MIPSD>();

      // Initialise if this is the first time we are executed
      if ( bInit && !IsNothing(PSD))
	    {
	      bInit = false;

        // Copy the Sieve Data Sizing Info
        MPSDDefn &SD = *(PSD.getDefn());
		    SysCADSystemHelper::SysCADSizeDataToSystem(SD,SizInfo);

		    // Copy Material Information from database
		    // For now we just set up n components
		    // The MineralInfo data will come from a database
		    MatInfo->Initialize( SD.getSizeVectorCount() );

		    // Initialize the Bruno Screen
		    Screen.Initialize(SizInfo,MatInfo,ParamVec);

		    // Create the Feed Stream. Use SetConfig after initialisation
		    // if we need to change anything. We need to initialise here
		    // because we get the size information from the Feed stream
		    FeedStream->Create( MatInfo , SizInfo );
      }

      if (!IsNothing(PSD)) // Only do soemthing if our input stream has a SizeData
	    {  

	      // Copy the input size data to the system feed stream solids
	      SysCADSystemHelper::SysCADSolidsToSystem(Feed,FeedStream);

	      // Execute the Model
	      Screen.CalculateModel( FeedStream );

	      // Copy the output size data to the SysCAD output streams
	      SysCADSystemHelper::SystemSolidsToSysCAD(OS,Screen.Oversize);
	      SysCADSystemHelper::SystemSolidsToSysCAD(US,Screen.Undersize);

	    }

    }
    else
    {//Dynamic
    }

  }
  catch (MMdlException &e)
  {
    Log.Message(MMsg_Error, e.Description);
  }
  }

//---------------------------------------------------------------------------

//====================================================================================
