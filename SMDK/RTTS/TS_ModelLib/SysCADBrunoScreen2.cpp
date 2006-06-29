//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __SysCADBrunoScreen_CPP
#include "BrunoScreen.h"
#include "SysCADBrunoScreen2.h"
#include "SysCADSystemHelper.h"


//====================================================================================

struct Parameter TopDeckScreenParameters[NumScreenParams] =
{
    { "Top.ScreenOpening",                  parREAL, "12.1"   },
    { "Top.EffectiveArea",                  parREAL, "17.816" },
    { "Top.OpenFractionOfEffectiveArea",    parREAL, "0.45"   },
    { "Top.FeedBulkDensity",                parREAL, "2.0"    },
    { "Top.Wet_Screen",                     parBOOL, "FALSE"  },
    { "Top.OpeningType",                    parINT , "2"      },
    { "Top.ScreenType",                     parINT,  "4"      },
    { "Top.DeckLocation",                   parINT,  "1"      },
    { "Top.FeedType",                       parINT,  "1"      },
    { "Top.CustomerFactor",                 parINT,  "1"      },
    { "Top.WaterSplitRatio",                parREAL, "0.5"    },
};

struct Parameter BottomDeckScreenParameters[NumScreenParams] =
{
    { "Bottom.ScreenOpening",               parREAL, "12.1"   },
    { "Bottom.EffectiveArea",               parREAL, "17.816" },
    { "Bottom.OpenFractionOfEffectiveArea", parREAL, "0.45"   },
    { "Bottom.FeedBulkDensity",             parREAL, "2.0"    },
    { "Bottom.Wet_Screen",                  parBOOL, "FALSE"  },
    { "Bottom.OpeningType",                 parINT , "2"      },
    { "Bottom.ScreenType",                  parINT,  "4"      },
    { "Bottom.DeckLocation",                parINT,  "1"      },
    { "Bottom.FeedType",                    parINT,  "1"      },
    { "Bottom.CustomerFactor",              parINT,  "1"      },
    { "Bottom.WaterSplitRatio",             parREAL, "0.5"    },
};
const char* OutputDescs[] =
  {
"QF",     //  total tph of feed
"Feed_OS",//  fraction of feed > apperture
"Feed_US",//  fraction of feed < apperture
"Feed_HS",//,  fraction of feed < half-apperture
"QU",     //,  flowrate of undersize stream
"QO",     //,  flowrate of oversize stream
"S",      //,  apperture size
"T",      //  screen efficiency (at S)
"AF",     //  effective screen area
"D50",    //  fitted d50 of separation
"A",      //  basic capacity factor
"B",      //  over-size factor
"C",      //  half-size factor
"D",      //  deck location factor
"E",      //  wet screening factor
"F",      //  feed bulk density factor
"G",      //  load vs efficiency factor
"H",      //  opening shape factor
"J",      //  screen open area factor
"K",      //  rock/gravel factor
"L",      //  humidity factor
"X",      //  user custom factor
"V"       //screen load %
  };

//====================================================================================

/*
const int idFeed=0;
const int idOversize1=1;
const int idOversize2=2;
const int idUndersize=3;

static MInOutDefStruct s_IODefs[]=
  {
  //  Desc;             Name;          Id; Rqd; Max; CnId, FracHgt;  Options;
    { "Feed",           "Feed",      idFeed,      1,  10,    0,    1.0f,  MIO_In |MIO_Material },
    { "OverSize1",      "OverSize1", idOversize1, 1,   1,    0,    0.5f,  MIO_Out|MIO_Material },
    { "OverSize2",      "OverSize2", idOversize2, 0,   1,    0,    0.5f,  MIO_Out|MIO_Material },
    { "UnderSize",      "UnderSize", idUndersize, 1,   1,    0,    0.5f,  MIO_Out|MIO_Material },
    { NULL },
  };
*/

//====================================================================================

const int idFeed      = 0;
const int idOversize  = 1;
const int idMidsize   = 2;
const int idUndersize = 3;

static MInOutDefStruct s_IODefs[]=
  {
  //  Desc;             Name;       Id; Rqd; Max; CnId, FracHgt;  Options;
    { "Feed",           "Feed",      idFeed,      1,  10,    0,    1.0f,  MIO_In |MIO_Material|MIO_Transfer},
    { "OverSize",       "OverSize",  idOversize,  1,   1,    0,    1.0f,  MIO_Out|MIO_Material|MIO_Transfer},
    { "MidSize",        "MidSize",   idMidsize,   0,   1,    0,    1.0f,  MIO_Out|MIO_Material|MIO_Transfer},
    { "UnderSize",      "UnderSize", idUndersize, 1,   1,    0,    1.0f,  MIO_Out|MIO_Material|MIO_Transfer},
    { NULL },
  };

//====================================================================================

double BrunoScreenDraw[] = { MDrw_Poly,  -4,3,  5,-1,  1,-3,  -4,3,
                           MDrw_End};

DEFINE_TRANSFER_UNIT(SysCADBrunoScreen, "TSBrunoScreen", DLL_GroupName)
void SysCADBrunoScreen_UnitDef::GetOptions()
  {
  SetDefaultTag("SC");
  SetDrawing(BrunoScreenDraw);
  SetTreeDescription("Size Separation:RioTinto Bruno Screen");
  SetDescription("RioTinto TS Screen");
  SetModelSolveMode(MSolveMode_Probal|MSolveMode_DynamicFlow);
  SetModelGroup(MGroup_PSD);
  SetModelLicense(MLicense_PSD);
  };

//====================================================================================

SysCADBrunoScreen::SysCADBrunoScreen(TaggedObject * pNd) : MBaseMethod(pNd)
  {
    // Construct the Persistent System Feed Data
    FeedStream  = TS_MinProc::PFlowStream1( new TS_MinProc::FlowStream1 );
    SizInfo = TS_MinProc::PSizingInfo1( new TS_MinProc::SizingInfo1 );
    MatInfo =  TS_MinProc::PStreamInfo1( new TS_MinProc::StreamInfo1 );

    // Initialisation Flag
    bInit = true;

    bTwoDecks = true;

    // Need to initialise the Screen Parameters	SysCADParams::InitParams(ScreenParameters,NumParams);

    SysCADParams::InitParams(TopDeckScreenParameters,NumScreenParams,TopDeckDataDefnParams);
    SysCADParams::InitParams(BottomDeckScreenParameters,NumScreenParams,BottomDeckDataDefnParams);

  }

//---------------------------------------------------------------------------

void SysCADBrunoScreen::Init()
{
  SetIODefinition(s_IODefs);
}

//---------------------------------------------------------------------------

void SysCADBrunoScreen::BuildDataFields()
{

    // TO Think about: Should we use a different set of parameters that we copy
    // to the parameter vector to make this cleaner?

    // SysCAD Tag Name convention ==> no spaces between words
    // Use MF_VISIBLE to make parameters invisible

    // Bruno Screen Parameters
    DD.Page("Configuration");

    DD.Bool("TwoDecks", "", &bTwoDecks , MF_RESULT);//MF_PARAMETER | MF_SET_ON_CHANGE | MF_PARAM_STOPPED);
    DD.Text("Top Deck Fields...");
    SysCADParams::BuildDataFields(TopDeckScreenParameters,NumScreenParams,this,TopDeckDataDefnParams);

    if ( bTwoDecks )
    {
        //DD.Page("Bottom Deck");
        DD.Text("");
        DD.Text("Bottom Deck Fields...");
        SysCADParams::BuildDataFields(BottomDeckScreenParameters,NumScreenParams,this,BottomDeckDataDefnParams);
    }

    // Model Output Data
    // We need to make this interface nicer as well
    // so we can automatically assign the model outputs a name
    DD.Page("TopDeck Output");
    DD.Text("Top Deck Model Output Data...");
    int l = TopDeckScreen.ModelOutput.size();
    DD.StructBegin("Top", "Top");
    for ( int i = 0 ; i < l ; i++ )
    {
        DD.Double(OutputDescs[i], "", &TopDeckScreen.ModelOutput[i], MF_RESULT, MC_None );
    }
    DD.StructEnd();

    if ( bTwoDecks )
    {
        DD.Page("BottomDeck Output");
        DD.Text("Bottom Deck Model Output Data...");
        int l = BottomDeckScreen.ModelOutput.size();
        for ( int i = 0 ; i < l ; i++ )
        {
            DD.Double(OutputDescs[i], "", &BottomDeckScreen.ModelOutput[i], MF_RESULT, MC_None );
        }
    }
}

//---------------------------------------------------------------------------

void SysCADBrunoScreen::EvalProducts()
{
  try
  {
      MStream Feed; // SysCAD Feed Stream
      FlwIOs.AddMixtureIn_Id(Feed, idFeed); //sum all input streams

      //
      // Get References to our Output Streams using the FlwIOs helper class and IO ids
      //

      MStream & OS = FlwIOs[FlwIOs.First[idOversize]].Stream; // SysCAD Oversize Output Stream
      MStream & US = FlwIOs[FlwIOs.First[idUndersize]].Stream; // SysCAD Undersize Output Stream

      OS=Feed;
      US=Feed;

      bTwoDecks = (FlwIOs.First[idMidsize]>=0);

      MIPSD & PSD=Feed.IF<MIPSD>();

      //
      // Initialise if this is the first time we are executed
      // and the feed stream has size data
      //
      if ( bInit && !IsNothing(PSD))
	    {
	      bInit = false;

        // Copy the Sieve Data Sizing Info
        //MPSDDefn &SD = (PSD.getDefn());
        //SysCADSystemHelper::SysCADSizeDataToSystem(SD, SizInfo);
        SysCADSystemHelper::SysCADSizeDataToSystem(PSD, SizInfo);

        // Copy Material Information from database
        // For now we just set up n components
        // The MineralInfo data will come from a database
        //MatInfo->Initialize( SD.getSizeVectorCount() );
        MatInfo->Initialize( PSD.PSDVectorCount );

        // Initialize Top Deck
        SysCADParams::ConvertParamsToDoubles(TopDeckScreenParameters,NumScreenParams,dParams,TopDeckDataDefnParams);
        TS_MinProc::VectorView ParamVec(dParams,11,1);
        TopDeckScreen.Initialize(SizInfo,MatInfo,ParamVec);

        if ( bTwoDecks )
        {
            // Initialise Bottom Deck
            SysCADParams::ConvertParamsToDoubles(BottomDeckScreenParameters,NumScreenParams,dParams,BottomDeckDataDefnParams);
            TS_MinProc::VectorView ParamVec(dParams,11,1);
            BottomDeckScreen.Initialize(SizInfo,MatInfo,ParamVec);
        }

        // Create the Feed Stream. Use SetConfig after initialisation
        // if we need to change anything. We need to initialise here
        // because we get the size information from the Feed stream
        FeedStream = TS_MinProc::FlowStream1::Create( MatInfo , SizInfo );
      }


      if (!IsNothing(PSD)) // Only do soemthing if our input stream has a SizeData
      {  
        // Copy the input size data to the system feed stream solids
        SysCADSystemHelper::SysCADSolidsToSystem(Feed,FeedStream);
        SysCADSystemHelper::SysCADLiquidToSystem(Feed,FeedStream);
        // Execute the Top Deck Model
        TopDeckScreen.CalculateModel( FeedStream );

        if ( bTwoDecks )
        {
            // Execute the Top Deck Model
            BottomDeckScreen.CalculateModel( TopDeckScreen.Undersize );
        }
        // Copy the output size data to the SysCAD output streams
        if (!bTwoDecks)
        {
            SysCADSystemHelper::SystemSolidsToSysCAD(OS,TopDeckScreen.Oversize);
            SysCADSystemHelper::SystemSolidsToSysCAD(US,TopDeckScreen.Undersize);
            SysCADSystemHelper::SystemLiquidToSysCAD(OS,TopDeckScreen.Oversize);
            SysCADSystemHelper::SystemLiquidToSysCAD(US,TopDeckScreen.Undersize);
        }
        else
        {
            MStream & MS = FlwIOs[FlwIOs.First[idMidsize]].Stream; // SysCAD Midsize Output Stream
            MS=Feed;
            SysCADSystemHelper::SystemSolidsToSysCAD(OS,TopDeckScreen.Oversize);
            SysCADSystemHelper::SystemSolidsToSysCAD(MS,BottomDeckScreen.Oversize);
            SysCADSystemHelper::SystemSolidsToSysCAD(US,BottomDeckScreen.Undersize);
            SysCADSystemHelper::SystemLiquidToSysCAD(OS,TopDeckScreen.Oversize);
            SysCADSystemHelper::SystemLiquidToSysCAD(MS,BottomDeckScreen.Oversize);
            SysCADSystemHelper::SystemLiquidToSysCAD(US,BottomDeckScreen.Undersize);
        }

        // IMPORTANT CODE TO HANDLE COMPONENTS WITHOUT SIZE DATA
        // We also need to handle data that does not have size data
        // so that mass balance is achieved in SysCAD. Water H20(l) is
        // already handled above so we do not worry about it here

        // D.T wants to handle water differently later
#ifdef WantThis
        const int WaterIndex = gs_MVDefn.Lookup("H2O(l)");
        for (int i=0; i<gs_MVDefn.Count(); i++)
        {
            if ( i != WaterIndex )
            {
            /*PSD.getDefn()->HasSizeData(i)
                if (!gs_MVDefn.HasSizeData(i))
                {
                    OS.putM(i, 0.0);
                    // Transfer all solids without size data and liquids apart from water to UnderSize
                    MS.putM(i, 0.0);
                    US.putM(i, Feed.getM(i));
                }*/
            }
        }
#endif
      }

  }

  //
  // Catch SysCAD Model Specific Exceptions
  //
  catch (MMdlException &e)
    {
    Log.Message(MMsg_Error, e.Description);
    }

  //
  // Catch Floating Point Exceptions
  //
  catch (MFPPException &e)
    {
    e.ClearFPP();
    Log.Message(MMsg_Error, e.Description);
    }

  //
  // Catch System Exceptions
  //
  catch (MSysException &e)
    {
    Log.Message(MMsg_Error, e.Description);
    }

  //
  // Catch Unkown Exceptions
  //
  catch (...)
    {
    Log.Message(MMsg_Error, "Some Unknown Exception occured");
    }
}



//====================================================================================
