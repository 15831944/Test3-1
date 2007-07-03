//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "SMDK_CondShellTube.h"
#pragma optimize("", off)

//====================================================================================

static double LMTDSaturation(double Ti, double To, double Ts)
  {
  if (Ti>Ts)
    {
    Ts=Min(Ts, Ti-0.01);
    To=Range(Ts+1.0e-6, To, Ti-1.0e-6);
    }
  else
    {
    Ts=Max(Ts, Ti+0.01);
    To=Range(Ti+1.0e-6, To, Ts-1.0e-6);
    }
  return (To-Ti)/NZ(Ln((Ts-Ti)/(Ts-To)));
  };

//====================================================================================

const long idTubeI  = 0;
const long idTubeO  = 1;
const long idShellI = 2; //steam in
const long idShellO = 3; //cond. out

static MInOutDefStruct s_IODefs[]=
  {
  //NOTE: Different CnId for tube side and shell side flows (material does not mix!)
  //  Desc;             Name;       PortId; Rqd; Max; CnId, FracHgt;  Options;
    { "Tube In",       "TubeI",     idTubeI,   1,  10,    0,    1.0f,  MIO_In |MIO_Material }, 
    { "Tube Out",      "TubeO",     idTubeO,   1,   1,    0,    1.0f,  MIO_Out|MIO_Material },
    { "Shell Steam In", "ShellI",   idShellI,  1,   1,    1,    1.0f,  MIO_In |MIO_Material },
    { "Shell Cond Out", "ShellO",   idShellO,  1,   1,    1,    1.0f,  MIO_Out|MIO_Material },
    { NULL },
  };

static double Drw_CCondShellAndTube[] = 
  { 
  MDrw_Arc, 0, 0, 5,
  MDrw_Poly, -5,0, -3,0, -3,-2, 3,2, 3,0, 5,0,
  MDrw_Poly, -5,1, -6,1, -6,0, -5,0, -5,1,
  MDrw_Poly, 5,-1, 6,-1, 6,0, 5,0, 5,-1,
  MDrw_End
  };

//---------------------------------------------------------------------------
DEFINE_TRANSFER_UNIT(CCondShellAndTube, "CondShellAndTube", DLL_GroupName)

void CCondShellAndTube_UnitDef::GetOptions()
  {
  SetDefaultTag("CST");
  SetDrawing("HeatExchange", Drw_CCondShellAndTube);
  SetTreeDescription("Demo:Condensing Shell and Tube Heat Exchanger");
  SetDescription("TODO: A description");
  SetModelSolveMode(MSolveMode_Probal);
  SetModelGroup(MGroup_Energy);
  SetModelLicense(MLicense_HeatExchange);
  };

//---------------------------------------------------------------------------

CCondShellAndTube::CCondShellAndTube(MUnitDefBase * pUnitDef, TaggedObject * pNd) : 
MBaseMethod(pUnitDef, pNd),
m_VLE(this, VLEF_QPFlash, "VLE"),
m_FTC(this)
  {
  //default values...
  m_dHTC        = 1.0;
  m_dArea       = 10.0;
  m_dLMTD       = 0.0;
  m_dDuty       = 0.0;
  }

//---------------------------------------------------------------------------

CCondShellAndTube::~CCondShellAndTube()
  {
  }

//---------------------------------------------------------------------------

void CCondShellAndTube::Init()
  {
  SetIODefinition(s_IODefs);
  RegisterFlashTrainNode(idShellI, true, m_FTC.TypeInfo()); //register as potential part of flashtrain and which connection
  }

//---------------------------------------------------------------------------
const int idDX_UA = 1;

void CCondShellAndTube::BuildDataFields()
  {
  DD.Text("");
  DD.Text("Configuration");
  //DD.CheckBox("On",          "", DC_,     "",         &m_fOn,            this, isParm|SetOnChange);
  DD.Double("HTC",       "",     &m_dHTC,           MF_PARAMETER, MC_HTC("kW/m^2.K"));
  DD.Double("Area",      "",     &m_dArea,          MF_PARAMETER, MC_Area("m^2"));

  DD.Text("");
  DD.Text("Results..");
  DD.Double("U*A",       "",     idDX_UA,           MF_RESULT|MF_NO_FILING, MC_UA("kW/K"));
  DD.Double("LMTD",      "",     &m_dLMTD,          MF_RESULT, MC_dT("C"));
  DD.Double("Duty",      "",     &m_dDuty,          MF_RESULT, MC_Pwr("kW"));
  DD.Text("");

  m_FTC.BuildDataFields();
  }

//---------------------------------------------------------------------------

bool CCondShellAndTube::ExchangeDataFields()
  {
  switch (DX.Handle)
    {
    case idDX_UA: 
      //do not need to set anything this is simply a calculated result
      DX.Double=m_dHTC*m_dArea;
      return true;
    }

  if (m_FTC.ExchangeDataFields())
    return 1;
  return false;
  }

//---------------------------------------------------------------------------

bool CCondShellAndTube::EvalJoinPressures()
  {
  if (1)//IsProbal)
    {//set pressures at each join (pipes connected to unit)
    for (int j=0; j<Joins.Count; j++)
      {
      double Pj=Joins[j].GetProbalPIn();

      if (j==1 && m_FTC.Active)
        m_FTC.SuctionP=Pj; //shell side and flashtrain is active

      Joins[j].SetProbalP(Pj, true, true);
      }
    return true;
    }
  else
    {
    //INCOMPLETECODE(__FILE__, __LINE__) 
    }
  return false;
  }

//---------------------------------------------------------------------------
//===========================================================================
//---------------------------------------------------------------------------

class CST_CondensateFinder : public MRootFinder
  {
  public:
    CST_CondensateFinder(CCondShellAndTube * pUnit, MStream &TubeI, MStream &ShellI, MStream &TubeO, MStream &ShellO);
    ~CST_CondensateFinder();
    double Function(double x);

  protected:
    MStream & m_TubeI;
    MStream & m_ShellI;
    MStream & m_TubeO;
    MStream & m_ShellO;

    CCondShellAndTube &m_Unit;
    MVLEBlk      &m_VLE;

    static MToleranceBlock s_Tol;
  };

MToleranceBlock CST_CondensateFinder::s_Tol(TBF_Both, "DemoShellTube:CondensFinder", 0.0, 1.0e-12);

//---------------------------------------------------------------------------

CST_CondensateFinder::CST_CondensateFinder(CCondShellAndTube * pUnit, MStream &TubeI, MStream &ShellI, MStream &TubeO, MStream &ShellO) : 
MRootFinder("A Desc" ,s_Tol),
m_Unit(*pUnit), 
m_TubeI(TubeI),
m_ShellI(ShellI),
m_TubeO(TubeO),
m_ShellO(ShellO),
m_VLE(pUnit->m_VLE)
  {
  m_TubeO.SuppressErrors=true;
  m_ShellO.SuppressErrors=true;
  };

//---------------------------------------------------------------------------

CST_CondensateFinder::~CST_CondensateFinder()
  {
  m_TubeO.SuppressErrors=false;
  m_ShellO.SuppressErrors=false;
  };

//---------------------------------------------------------------------------

double CST_CondensateFinder::Function(double Amount)
  {
  m_ShellO.SetM(m_ShellI, MP_All, Amount, m_ShellI.P);
  double ShtotHf=m_ShellO.totHf();

  double FTemp=m_ShellI.SaturationT();
  double FPress=m_ShellI.SaturationP();
  m_VLE.SetFlashVapFrac(m_ShellO, FTemp, FPress, 0.0, VLEF_Null);
  m_ShellO.SetTP(FTemp, FPress);
  m_Unit.m_dDuty=-(m_ShellO.totHf()-ShtotHf);

  m_TubeO.Set_totHf(m_TubeI.totHf()+m_Unit.m_dDuty);

  double TDuty;
  const double UA = m_Unit.m_dHTC*m_Unit.m_dArea;
  if (UA>1.0e-6)
    {
    m_Unit.m_dLMTD = LMTDSaturation(m_TubeI.T, m_TubeO.T, m_ShellO.T);
    TDuty = UA * m_Unit.m_dLMTD;
    }
  else
    {
    m_Unit.m_dLMTD = 0.0;
    TDuty = 0.0;
    }

  double Diff = m_Unit.m_dDuty-TDuty;
  return Diff;
  };

//---------------------------------------------------------------------------

void CCondShellAndTube::DoCondensingHeater(MStream & ShellI, MStream & TubeI, MStream & ShellO, MStream & TubeO)
  {
  if (TubeI.MassFlow()>0 && ShellI.MassFlow()>0)
    {
    m_FTC.VapourFlow=ShellI.MassFlow();
    double MaxVapFlow=150.0;//???
    CST_CondensateFinder Fn(this, TubeI, ShellI, TubeO, ShellO);
    // Now Solve and set the flash train demand
    int RetCode = Fn.FindRoot(0, 0, MaxVapFlow);
    switch (RetCode)
      {
      case RF_OK:      m_FTC.VapourFlowReqd = Fn.Result();       break;
      case RF_LoLimit: m_FTC.VapourFlowReqd = MaxVapFlow*1e-6;   break;
      case RF_HiLimit: m_FTC.VapourFlowReqd = MaxVapFlow;        break;
      default: 
        Log.Message(MMsg_Error, "Condesate Flow not found - RootFinder:%s", Fn.ResultString(Fn.Error())); 
        break;
      }

    if (m_FTC.Active)
      {//operating as part of a flash train macro model, must set "makeup" required...
      if (1)
        {
        //only allow one steam feed connection
        for (int i=0; i<FlwIOs.Count; i++)
          {
          MFlow F=FlwIOs[i];
          if (F.Id==idShellI && F.IsFlwIn && F.IsMakeUpAvail)
            F.MakeUpReqd=m_FTC.VapourFlowReqd;
          }
        }
      else
        {
        //loop through all the connections and sum the "available makeup". There may be more than one steam source directly connected!
        double Reqd=m_FTC.VapourFlowReqd;
        double Avail=0.0;
        for (int i=0; i<FlwIOs.Count; i++)
          {
          MFlow F=FlwIOs[i];
          if (F.Id==idShellI && F.IsFlwIn && F.IsMakeUpAvail)
            Avail+=F.MakeUpAvail;
          }
        Avail=GTZ(Avail);

        // Distribute according to MakeUpAvail
        for (i=0; i<FlwIOs.Count; i++)
          {
          MFlow F=FlwIOs[i];
          if (F.Id==idShellI && F.IsFlwIn && F.IsMakeUpAvail)
            F.MakeUpReqd=Reqd*Max(1.0e-9, F.MakeUpAvail/Avail);
          }
        }
      }
    }
  };

//---------------------------------------------------------------------------

void CCondShellAndTube::EvalProducts()
  {
  try           
    {
    MStreamI TubeI;
    MStreamI ShellI;
    MStream & TubeO = FlwIOs[FlwIOs.First[idTubeO]].Stream; //get reference to the actual output stream
    MStream & ShellO = FlwIOs[FlwIOs.First[idShellO]].Stream; //get reference to the actual output stream

    FlwIOs.AddMixtureIn_Id(TubeI, idTubeI);
    FlwIOs.AddMixtureIn_Id(ShellI, idShellI);

    TubeO = TubeI; //copy tube input to output
    ShellO = ShellI; //copy shell input to output

    DoCondensingHeater(ShellI, TubeI, ShellO, TubeO);

    //double tt=TubeO.T;
    //double st=ShellO.T;

    //double ShellPi=ShellI.P;
    //double ShellPo=ShellO.P;
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

void CCondShellAndTube::ClosureInfo(MClosureInfo & CI)
  {
  if (CI.DoFlows())
    {
    //  HtIn += dActualDuty; //ensure heat balance
    }
  }

//====================================================================================
