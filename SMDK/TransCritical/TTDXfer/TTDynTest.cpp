//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
//           QAL Classifier Model 2004 - Transcritical Technologies/ QAL 
//   Time-stamp: <2007-05-27 17:33:27 Rod Stephenson Transcritical Pty Ltd>
// Copyright (C) 2005 by Transcritical Technologies Pty Ltd and KWA
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __TTDYNTEST_CPP
#include "ttdyntest.h"


static MInitialiseTest InitTest("DynTest");


static MSpeciePtr  spAlumina       (InitTest, "NaAl[OH]4(l)", false);

enum {idFeed, idUflow, idOflow};
enum {PB_None, PB_PSD, PB_SSA};


static MInOutDefStruct s_IODefs[]=
  {
    { "Feed",      "Feed",   idFeed,  1, 10,  0,  1.0f,   MIO_In |MIO_Material },
    { "Underflow", "UFlow",  idUflow,  1,  1,  0,  1.0f,  MIO_Out|MIO_Material },
    { "Overflow",  "OFlow",  idOflow, 0,  1,  1,  1.0f,   MIO_Out|MIO_Material },
    
    
    { NULL },
  };

double Drw_CDynTestTank[] = 
  { 
    MDrw_Poly, -5,10, -5,-10, 5,-10, 5,10,
    MDrw_End 
  };

//---------------------------------------------------------------------------
DEFINE_TRANSFER_UNIT(CDynTestTank, "DynTestTank", DLL_GroupName)

void CDynTestTank_UnitDef::GetOptions()
{
  SetDefaultTag("DPSD");
  SetDrawing("Tank", Drw_CDynTestTank);
  SetTreeDescription("TT1:DynamicPSD");
  SetDescription("TODO: Dynamic PSD Model");
  SetModelSolveMode(MSolveMode_All);
  SetModelGroup(MGroup_PSD);
  SetModelLicense(MLicense_BlackBox);
};

//---------------------------------------------------------------------------




CDynTestTank::CDynTestTank(MUnitDefBase *pUnitDef, TaggedObject * pNd) :  
  MBaseMethod(pUnitDef, pNd)
{   //default values...

  dDeltaT = 0.0;
}





//---------------------------------------------------------------------------

CDynTestTank::~CDynTestTank()
{


}


bool CDynTestTank::ValidateDataFields()
  {//ensure parameters are within expected ranges
  return true;
  }long lPBType;

//---------------------------------------------------------------------------

void CDynTestTank::Init()
{
  SetIODefinition(s_IODefs);
}

//---------------------------------------------------------------------------




//---------------------------------------------------------------------------










void CDynTestTank::EvalProducts()
  {
  try {
    FlwIOs.AddMixtureIn_Id(Feed, idFeed);
    MStream & UFlow = FlwIOs[FlwIOs.First[idUflow]].Stream; //Reference to the output stream
    MStream & OFlow = FlwIOs[FlwIOs.First[idOflow]].Stream; //Reference to the output stream
    
    //    if (!alreadySolved || UFlow.MassFlow() < 1.0e-3)
    Tank = Feed;
      //else
      //Tank = UFlow;
    UFlow = Tank;
    OFlow = Tank;
    OFlow.SetF(Tank, MP_All, 0.0);
    UFlow.SetF(Tank, MP_All, 1.0);
    dDeltaT = getDeltaTime();
    

    bool streamOK=true;
    
    if (!bOnline) return;

    // Check for existence of Bayer Property... 
    MIBayer & FeedB=Feed.IF<MIBayer>(false);   // Do the checks up front

    Log.SetCondition(IsNothing(FeedB), 1, MMsg_Warning, "Bad Feed Stream - Not BATC Bayer Model");


    // Try PSD
    MIPSD & FeedPSD = Feed.IF<MIPSD>(false);
    if (!IsNothing(FeedPSD)) 
      lPBType = PB_PSD;
    else {   // Try SSA...
     MISSA & FeedSSA = Feed.IF<MISSA>(false);
     if (!IsNothing(FeedSSA))
       lPBType = PB_SSA;
     else
       lPBType = PB_None;
    }


    Log.SetCondition(lPBType==PB_None, 1, MMsg_Warning, "Bad Feed Stream - No PSD or SSA Property");

    if (IsNothing(FeedB) || IsNothing(FeedPSD)) streamOK = false;


    if (bOnline && streamOK) {  // Online and have Bayer and SSA properties...
    

      
      UFlow = Tank;

    } else  {   // Just tidy up and put some sensible stuff in the results...
       Log.Message(MMsg_Warning, "Stream Problem...");
      
    }
  }
  catch (MMdlException &e)
    {
    Log.Message(MMsg_Error, e.Description);
    }
  catch (MFPPException &e)
    {
    e.ClearFPP();
    Log.Message(MMsg_Error, e.Description);
    }
  catch (MSysException &e)
    {
    Log.Message(MMsg_Error, e.Description);
    }
  catch (...)
    {
      Log.Message(MMsg_Error, "Some Unknown Exception occured");
    }
  }

//--------------------------------------------------------------------------





void CDynTestTank::BuildDataFields()
{

  DD.Text  ("");

#ifdef TTDEBUG
  DD.CheckBox("TTDBG", "",  &bTTDebug, MF_PARAMETER);
#endif


  DD.CheckBox("On", "",  &bOnline, MF_PARAMETER|MF_SET_ON_CHANGE);
  //  DD.CheckBox("Int.Cooling", "", &m_bInternalCool, MF_PARAMETER|MF_SET_ON_CHANGE);
  //DD.CheckBox("Ext.Cooling", "", &m_bExternalCool, MF_PARAMETER|MF_SET_ON_CHANGE);
  DD.Text("");
  DD.CheckBox("Growth.On", "",  &bGrowthOn, MF_PARAMETER|MF_SET_ON_CHANGE);
  DD.CheckBox("Agglom.On", "",  &bAgglomOn, MF_PARAMETER|MF_SET_ON_CHANGE);
  DD.CheckBox("Nucleation.On", "",  &bNuclOn, MF_PARAMETER|MF_SET_ON_CHANGE);
  DD.CheckBox("Attrition.On", "",  &bAttritionOn, MF_PARAMETER|MF_SET_ON_CHANGE);





  DD.Show();
  DD.Text("");
  DD.Text  ("Requirements");
  DD.Double("TankVol", "",	        &dTankVol         ,MF_PARAMETER, MC_Vol("m^3"));
  DD.Double("DeltaT", "dT",   &dDeltaT, MF_RESULT, MC_Time("s"));
  DD.Text  ("");

  DD.Text  ("Results Tank");

    
  DD.Text  ("Results");         
  DD.Show();                    
  DD.Double("Vol_FlowIn", "",       &dQvin          ,MF_RESULT, MC_Qv("L/s"));
  DD.Double("Vol_FlowOut", "",      &dQvout         ,MF_RESULT, MC_Qv("L/s"));

  DD.Double("MassFlowIn", "",       &dQmin          ,MF_RESULT, MC_Qm("t/d"));
  DD.Double("MassFlowOut", "",      &dQmout         ,MF_RESULT, MC_Qm("t/d"));
  

  DD.Double("TempIn", "",           &dTin           ,MF_RESULT, MC_T("C"));
  DD.Double("TempOut", "",          &dTout          ,MF_RESULT, MC_T("C"));
  DD.Text("");
  
}
