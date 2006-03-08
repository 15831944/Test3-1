//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "SMDK_FlashTank.h"

#define dbgModels 1

#pragma optimize("", off)

//====================================================================================

const long idFeed   = 0;
const long idSlurry = 1;
const long idSteam  = 2;

static MInOutDefStruct s_IODefs[]=
  {
  //  Desc;             Name;        PortId; Rqd; Max; CnId, FracHgt;  Options;
    { "Feed In",       "Feed",     idFeed,    1,  10,    0,    1.0f,  MIO_In |MIO_Material },
    { "Slurry Out",    "Slurry",   idSlurry,  1,   1,    0,    1.0f,  MIO_Out|MIO_Material },
    { "Steam Out",     "Steam",    idSteam,   1,   1,    0,    1.0f,  MIO_Out|MIO_Material },
    { NULL },
  };

static double Drw_CFlashTank[] = 
  { 
    MDrw_Poly, 0,-8, -5,-4, -5,5, 0,8, 5,5, 5,-4, 0,-8,
    MDrw_Arc3, -5,4, 0,8, 5,4,
    MDrw_End 
  };

//---------------------------------------------------------------------------
DEFINE_TRANSFER_UNIT(CFlashTank, "FlashTank", DLL_GroupName)

void CFlashTank_UnitDef::GetOptions()
  {
  SetDefaultTag("FT", true);
  SetDrawing("Tank", Drw_CFlashTank);
  SetTreeDescription("Demo:FlashTank");
  SetDescription("TODO: A description");
  SetModelSolveMode(MSolveMode_Probal);
  SetModelGroup(MGroup_Energy);
  SetModelLicense(MLicense_HeatExchange);
  };

//---------------------------------------------------------------------------

CFlashTank::CFlashTank(MUnitDefBase * pUnitDef, TaggedObject * pNd) :
MBaseMethod(pUnitDef, pNd),
m_VLE(this, VLEF_QPFlash, "VLE"),
m_FTF(this),
m_PCtrl(this, "PCtrl", PBPC_SatP, StdP, true)
  {
  //m_FlashPRqd=gs_StdPress+50;
  m_FTF.FlashP=StdP;
  m_OnLine=true;
  m_VLE.Enable();
  //SetDevelopementChecksOn();
  }

CFlashTank::~CFlashTank()
  {
  }

//---------------------------------------------------------------------------

void CFlashTank::Init()
  {
  SetIODefinition(s_IODefs);
  //Dbg.PrintLn("%-30s   %s ", m_FTF.TypeInfo()->name(), m_FTF.TypeInfo()->raw_name());
  RegisterFlashTrainNode(idSteam, true, m_FTF.TypeInfo());
  }

//---------------------------------------------------------------------------

const long OM_Simple      = 0;
const long OM_Condensing  = 1;

void CFlashTank::BuildDataFields()
  {
  DD.Text("");
  //DD.Text("Requirements...");
  DD.CheckBox("OnLine", "", &m_OnLine, MF_PARAMETER);
  //DD.Double("FlashP_Rqd",  "",     &m_FlashPRqd,     MF_PARAMETER|MF_NAN_OK, MC_P("kPag"));
  m_VLE.OnOffCheckBox();
  DD.Text("");

  m_PCtrl.BuildDataFields(PBPC_Atmos|PBPC_Reqd|PBPC_SatP|PBPC_ReqdSatT);
  m_FTF.BuildDataFields();

  m_VLE.BuildDataFields();
  }

bool CFlashTank::ExchangeDataFields()
  {
  if (m_FTF.ExchangeDataFields())
    return 1;
  //switch (DX.Handle)
  //  {
  //  case 1: 
  //    if (DX.HasReqdValue)
  //      m_lOpMode=DX.Long;
  //    DX.Long=m_lOpMode;
  //    return true;
  //  }
  return false;
  }

//--------------------------------------------------------------------------

void CFlashTank::EvalMakeUpReqd() 
  { 
  if (m_FTF.Active)
    {
    }
  else
    MakeUpNodeTransferReqd(0); 
  };

//--------------------------------------------------------------------------

void CFlashTank::EvalMakeUpAvail() 
  { 
  if (m_FTF.Active)
    {
    int ioStm=FlwIOs.First[idSteam];
    FlwIOs[ioStm].MakeUpAvail=Max(1.0e-6, m_FTF.VapourFlow); //steam
    }
  else
    MakeUpNodeTransferAvail(0); 
  };

//---------------------------------------------------------------------------

bool CFlashTank::EvalJoinPressures()
  {
  if (IsNetProbal && Joins.Count>0)
    {
    m_FTF.FeedP=Joins[0].GetProbalPIn(&m_PCtrl);

    if (m_FTF.Active)
      {
      switch (m_PCtrl.Method)
        {
        case PBPC_Reqd:       m_FTF.FlashP=m_PCtrl.PRqd;                                break;
        case PBPC_ReqdSatT:   m_FTF.FlashP=m_Feed.SaturationP(m_PCtrl.TRqd);            break;
        case PBPC_Atmos:      m_FTF.FlashP=AtmosPress();                                break;
        default:              m_FTF.FlashP=m_FTF.EstFlashP;                             break;
        }
      }
    else if (m_OnLine)
      {
      switch (m_PCtrl.Method)
        {
        case PBPC_Reqd:       m_FTF.FlashP=m_PCtrl.PRqd;                                break;
        case PBPC_ReqdSatT:   m_FTF.FlashP=m_Feed.SaturationP(m_PCtrl.TRqd);            break;
        default:              m_FTF.FlashP=AtmosPress();                                break;
        }
      }
    else
      m_FTF.FlashP=m_FTF.FeedP;

    m_FTF.FlashP=Max(m_FTF.MinFlshTrainP, m_FTF.FlashP);
    Dbg.PrintLn("FlashP1   %10.2f", m_FTF.FlashP-101.325);
    m_PCtrl.SetP2Set(m_FTF.FlashP, true, true);
    Joins[0].SetProbalP(&m_PCtrl);

    return true;
    }
  return false;
  };

//---------------------------------------------------------------------------

void CFlashTank::EvalProducts()
  {
  try           
    {
    //MStream Feed;
    //MStream ShellI;
    MStream & Slurry = FlwIOs[FlwIOs.First[idSlurry]].Stream; //get reference to the actual Slurry stream
    MStream & Steam  = FlwIOs[FlwIOs.First[idSteam]].Stream;  //get reference to the actual Steam stream

    FlwIOs.AddMixtureIn_Id(m_Feed, idFeed);
    double PIn=m_Feed.P;

    //Steam.SetModel();
    Steam.ZeroMass();//   = Feed;
    Slurry  = m_Feed;

    bool FlashOK=false;
    if (m_OnLine)
      {
      // DoReactions etc on Slurry
      
      m_WrkStrm.Copy(Slurry, PIn);
      double LMass=Slurry.Mass();
      if (LMass>UsableMass)
        {//TODO : CHECK! ... Does all this VLE stuff work if (1) steam is present in feed mixture? (2) other vapours are present in feed mixture?
        FlashOK=true;
        m_FTF.FeedSaturationP=Slurry.SaturationP(Slurry.T);
        double PMargin=0.0001;

        m_VLE.QPFlash(Slurry, Steam, m_FTF.SuctionP, 0.0, VLEF_SaturationLimit);

        int Iter=4;  // goes to 1 Kpa;
        do 
          {
          PMargin *= 10.0;
          bool DoQP=true;
          if (m_FTF.Active)
            {
            m_FTF.FlashP=Min(m_FTF.FlashP, m_FTF.FeedSaturationP-PMargin); // Force a small amount of Vapour
            Dbg.PrintLn("FlashP2   %10.2f", m_FTF.FlashP-101.325);
            m_FTF.EstFlashP=m_FTF.FlashP;
            }
          if (DoQP)
            m_VLE.QPFlash(Slurry, Steam, m_FTF.FlashP, 0.0, VLEF_SaturationLimit);

          m_WrkStrm.Copy(Slurry, PIn);
          m_WrkStrm.AddF(Steam, MP_All, 1.0);
          m_FTF.FlashT=Slurry.T;
          m_FTF.VapourFlow=Steam.Mass(MP_All); //assumes steam!!!

          if (DbgBrk)
            { int xxx=0; };
          }
        while ((--Iter>0) && (m_FTF.VapourFlow<1.0e-6*LMass)); 
        }

      }

    if (!FlashOK)
      {
      m_FTF.FlashT=Slurry.T;
      m_FTF.FlashP=PIn;
      Dbg.PrintLn("FlashP3   %10.2f", m_FTF.FlashP-101.325);
      Steam.SetF(Slurry, MP_All, 0.0, PIn);
      m_FTF.VapourFlow=0.0;
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

void CFlashTank::ClosureInfo(MClosureInfo & CI)
  {
  if (CI.DoFlows())
    {
    //  HtIn += dActualDuty; //ensure heat balance
    }
  }

//====================================================================================

void CFlashTank::MacroMdlEvaluate(eScdMacroMdlEvals Eval)
  { 
  if (m_FTF.Active)
    {
    switch (Eval)
      {
      case MME_Flash2P:
        {
        double P0=m_FTF.PEval;
        m_VLE.QPFlash(m_WrkStrm, P0, 0.0, VLEF_Null);
        double Q0=m_WrkStrm.Mass(MP_Gas);
        double P1=P0-0.1; // CNM
        m_VLE.QPFlash(m_WrkStrm, P1, 0.0, VLEF_Null);
        double Q1=m_WrkStrm.Mass(MP_Gas);
        m_FTF.VapourFlowSensEval=(Q0-Q1)/(P0-P1); // should be negative
        m_FTF.VapourFlowSensGood=(m_WrkStrm.Mass()>1.0e-10);
        #if dbgModels   
        Dbg.PrintLn("FlashMM2P  %12.6f @ %12.6f %s", m_FTF.VapourFlowSensEval, m_FTF.PEval, Tag);
        #endif
        }
        break;
      case MME_Flash2Qv:
        {
        double P0=m_FTF.PEval;
        m_VLE.QPFlash(m_WrkStrm, P0, 0.0, VLEF_Null);
        m_FTF.VapourFlowEval=m_WrkStrm.Mass(MP_Gas);
        #if dbgModels   
        Dbg.PrintLn("FlashMM2Qv %12.6f @ %12.6f %s", m_FTF.VapourFlowEval, m_FTF.PEval, Tag);
        #endif
        }
        break;
      case MME_Flash2QvSens:
        {
        //double P0=m_FTF.dPEval;
        m_VLE.QMVapFlash(m_WrkStrm, m_FTF.VapourFlowEval, 0.0, VLEF_Null);
        m_FTF.PEval=m_WrkStrm.P;
        #if dbgModels   
        Dbg.PrintLn("FlashMM2QS %12.6f @ %12.6f %s", m_FTF.VapourFlowEval, m_FTF.PEval, Tag);
        #endif
        }
        break;
      }
    }
  }

//====================================================================================
