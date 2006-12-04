//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "smdk_pump.h"

//====================================================================================
//#pragma optimize("", off)

//static MInitialiseTest InitTest("SMDK_Precip");

//static MSpeciePtr   spWater            (InitTest, "H2O(l)", false);

//====================================================================================

enum PVPM_PumpValvePressMethod { PVPM_None, PVPM_Fixed, PVPM_Boost, PVPM_Drop, PVPM_Atmos, PVPM_SatP };
enum PVM_PumpValveModel { PVM_Pump, PVM_Valve };

const long idFeed  = 0;
const long idProd  = 1;

static MInOutDefStruct s_IODefs[]=
  {
  //  Desc;             Name;        PortId; Rqd; Max; CnId, FracHgt;  Options;
    { "Feed",          "Feed",      idFeed,    1,  10,    0,    1.0f,  MIO_In |MIO_Material },
    { "Product",       "Prod",      idProd,    1,   1,    0,    1.0f,  MIO_Out|MIO_Material },
    { NULL },
  };

static double Drw_CPumpValve[] = 
  { 
  MDrw_Poly, -5,10, -5,-10, 5,-10, 5,10,
  MDrw_End 
  };

//---------------------------------------------------------------------------
DEFINE_TRANSFER_UNIT(CPumpValve, "Pump", DLL_GroupName)

void CPumpValve_UnitDef::GetOptions()
  {
  SetDefaultTag("PMP", true);
  SetDrawing("Pump", Drw_CPumpValve);
  SetTreeDescription("Demo:Pump");
  SetDescription("TODO: A description");
  SetModelSolveMode(MSolveMode_Probal);
  SetModelGroup(MGroup_General);
  //SetModelLicense(MLicense_HeatExchange|MLicense_Alumina);
  };

//---------------------------------------------------------------------------

CPumpValve::CPumpValve(MUnitDefBase * pUnitDef, TaggedObject * pNd) : MBaseMethod(pUnitDef, pNd)
  {
  //default values...
  iModel = PVM_Pump;
  if (iModel==PVM_Pump)
    {
    bIsentropic = 1;
    iPressMethod = PVPM_Boost;
    dPressBoost_Rqd=10.0;
    }
  else
    {
    bIsentropic = 0;
    iPressMethod = PVPM_Drop;
    dPressBoost_Rqd=-10.0;
    }
  dPout_Rqd=dNAN;
  dTout_Rqd=dNAN;
  dDuty_Rqd=0.0;

  dTout=StdTemp(); //actual
  dTin=StdTemp(); //actual
  dPout=StdPress(); //actual
  dPin=StdPress(); //actual
  dDuty=0.0; //actual
  }

//---------------------------------------------------------------------------

CPumpValve::~CPumpValve()
  {
  }

//---------------------------------------------------------------------------

void CPumpValve::Init()
  {
  SetIODefinition(s_IODefs);
  }

//---------------------------------------------------------------------------
const long xidModel         =  8;
const long xidPDropRqd      =  9;
const long xidPBoostRqd     = 10;
const long xidPoutRqd       = 11;
const long xidDuty          = 12;
const long xidQm            = 13;
const long xidTempIn        = 14;
const long xidTempOut       = 15;
const long xidPIn           = 16;
const long xiddP            = 17;
const long xidPOut          = 18;
const long xidVapFracIn     = 19;
const long xidVapFracOut    = 20;

void CPumpValve::BuildDataFields()
  {
  static MDDValueLst DDB1[]={
    {PVM_Pump, "Pump"},
    {PVM_Valve, "Valve"},
    {0}};
    static MDDValueLst DDB2[]={
      {PVPM_None, "None"},
      {PVPM_Boost, "Boost"},
      {PVPM_Fixed, "Fixed"},
      {0}};
      static MDDValueLst DDB3[]={
        {PVPM_None, "None"},
        {PVPM_Drop, "Drop"},
        {PVPM_Fixed, "Fixed"},
        {PVPM_Atmos, "Atmos"},
        {0}};
        DD.Text    ("");
        DD.Text    ("Requirements");
        DD.Long    ("Model", "", xidModel, MF_PARAMETER|MF_SET_ON_CHANGE, DDB1);
        DD.Long    ("PressMethod", "", &iPressMethod, MF_PARAMETER|MF_SET_ON_CHANGE, (iModel==PVM_Pump ? DDB2 : DDB3));
        if (iModel==PVM_Pump)
          {
          DD.Show    (iPressMethod==PVPM_Boost);
          DD.Double  ("PressBoostRqd", "PBoost", &dPressBoost_Rqd, MF_PARAMETER, MC_DP("kPa"));
          }
        else
          {
          DD.Show    (iPressMethod==PVPM_Drop);
          DD.Double  ("PressDropRqd", "PDrop", xidPDropRqd, MF_PARAMETER, MC_DP("kPa"));
          }
        DD.Show    (iPressMethod==PVPM_Fixed);
        DD.Double  ("OutletPressRqd", "PRqd", &dPout_Rqd, MF_PARAMETER|MF_NAN_OK, MC_P("kPag"));
        DD.Show    ();
        DD.Text    ("");
        DD.Text    ("Results");
        DD.Double  ("",                "Work", &dDuty,          MF_RESULT,                  MC_Pwr("kW"));
        DD.Double  ("Mass_Flow",       "Qm",   xidQm,           MF_RESULT|MF_NO_FILING,     MC_Qm("kg/s"));
        DD.Double  ("TemperatureIn",   "Ti",   &dTin,           MF_RESULT|MF_NO_FILING,     MC_T("C"));
        DD.Double  ("TemperatureOut",  "To",   &dTout,          MF_RESULT|MF_NO_FILING,     MC_T("C"));
        DD.Double  ("PressIn",         "Pi",   &dPin,           MF_RESULT,                  MC_P("kPag"));//isParmConstruct);
        //DD.Double  ("Press_Change",    "dP",   xiddP,           MF_RESULT|MF_NO_FILING|MF_INIT_HIDDEN,   MC_DP("kPa"));
        DD.Double  ("PressOut",        "Po",   &dPout,          MF_RESULT,                  MC_P("kPag"));//isParmConstruct);
        //DD.Double  ("VapourFracIn",    "Vfi",  xidVapFracIn,    MF_RESULT|MF_NO_FILING|MF_INIT_HIDDEN, MC_Frac("%"));
        //DD.Double  ("VapourFracOut",   "Vfo",  xidVapFracOut,   MF_RESULT|MF_NO_FILING|MF_INIT_HIDDEN, MC_Frac("%"));
        //DD.Double  ("PrevPOut",        "",     &dPout,          MF_PARAMETER|MF_NO_VIEW,    MC_P("kPag"));
        //DD.Double  ("PrevDuty",        "",     &dDuty,          MF_PARAMETER|MF_NO_VIEW,    MC_Pwr("kW"));
        DD.Text("");
  }

bool CPumpValve::ExchangeDataFields()
  {
  switch (DX.Handle)
    {
    case xidModel:
      if (DX.HasReqdValue)
        {
        iModel=DX.Long;
        if (iModel==PVM_Pump)
          {
          bIsentropic = 1;
          }
        else
          {
          bIsentropic = 0;
          }
        }
      DX.Long=iModel;
      return 1;
    case xidPDropRqd:
      if (DX.HasReqdValue)
        dPressBoost_Rqd=(DX.Double*-1.0);
      DX.Double=(dPressBoost_Rqd*-1.0);
      return 1;
      /*case xidPBoostRqd:
      if (DX.HasReqdValue)
      dPressBoost_Rqd=DX.Double;
      DX.Double=dPressBoost_Rqd;
      return 1;
      case xidDuty:
      if (DX.HasReqdValue)
      dDuty=DX.Double;
      DX.Double=dDuty;
      return 1;*/
    }
  return false;
  }
//---------------------------------------------------------------------------

bool CPumpValve::ValidateDataFields()
  {//ensure parameters are within expected ranges
  return true;
  }

//---------------------------------------------------------------------------

bool CPumpValve::EvalJoinPressures()
  {
  if (IsNetProbal)
    {//set pressures at each join (pipes connected to unit)
    for (int j=0; j<Joins.Count; j++)
      {
      Joins[j].SetProbalP(dPout, false, true); //only set pressure at outlet
      }
    return true;
    }
  else
    {
    //INCOMPLETECODE(__FILE__, __LINE__);
    return MBaseMethod::EvalJoinPressures();
    }
  return false;
  }

//---------------------------------------------------------------------------

class CPumpTempSolverFn : public MRootFinder
  {
  public:
    CPumpTempSolverFn(MStream &Stream, double RqdP) : MRootFinder("A Desc" ,s_Tol), m_Stream(Stream)
      {
      m_RqdP = RqdP;
      }
    //vary temperature at rqd pressure to maintain entropy...
    double Function(double x)
      {
      m_Stream.SetTP(x, m_RqdP);
      double s = m_Stream.totSf();
      return s;
      }
  protected:
    MStream &m_Stream;
    double m_RqdP;
    static MToleranceBlock s_Tol;
  };

MToleranceBlock CPumpTempSolverFn::s_Tol(TBF_Both, "PumpTemperatureFn", 1.0e-6, 0, 100);

//---------------------------------------------------------------------------

void CPumpValve::EvalProducts()
  {
  try
    {
    MStream Feed;
    FlwIOs.AddMixtureIn_Id(Feed, idFeed);
    MStream & Prod = FlwIOs[FlwIOs.First[idProd]].Stream; //get reference to the actual output stream

    Prod = Feed;
    dPin = Feed.P;
    dTin = Feed.T;

    const double dQm = Feed.MassFlow();
    if (iPressMethod==PVPM_None || fabs(dQm)<1.0e-12)
      {
      dDuty = 0.0;
      dPout = Prod.P;
      dTout = Prod.T;
      }
    else
      {
      dPout = Prod.P;
      switch (iPressMethod)
        {
        case PVPM_None : break;
        case PVPM_Fixed: dPout = Range(1.0, dPout_Rqd, 1.0e15); break;
        case PVPM_Boost: 
        case PVPM_Drop : dPout = Range(1.0, dPout+dPressBoost_Rqd, 1.0e15); break;
        case PVPM_Atmos: dPout = AtmosPress(); break;
        case PVPM_SatP : /*dPout = todo*/ break; 
        }

      if (bIsentropic)
        {//for pump maintain entropy...
        const double TOut = Prod.T;
        const double h0 = Prod.totHf();
        const double s0 = Prod.totSf();
        const double MnT = max(dTout-100.0, ZeroCinK); //WHAT LIMITS SHOULD BE USED???!!!
        const double MxT = dTout+200.0; //WHAT LIMITS SHOULD BE USED???!!!
        CPumpTempSolverFn Fn(Prod, dPout);
        double t;
        switch (Fn.FindRoot(s0, MnT, MxT))
          {
          case RF_OK:       t = Fn.Result();   break;
          case RF_LoLimit:  t = MnT;      break;
          case RF_HiLimit:  t = MxT;      break;
          default: 
            Log.Message(MMsg_Error, "Pump temperature outlet T not found - RootFinder:%s", Fn.ResultString(Fn.Error())); 
            t = Fn.Result();
            break;
          }
        dDuty = Prod.totHf() - h0;
        const double s1 = Prod.totSf();
        }
      else
        {
        if (Valid(dTout_Rqd))
          {
          const double h0 = Prod.totHf();
          Prod.SetTP(dTout_Rqd, dPout);
          dDuty = Prod.totHf() - h0;
          }
        else if (fabs(dDuty_Rqd)>1.0e-12)
          {
          const double h0 = Prod.totHf();
          Prod.Set_totHf(h0+dDuty_Rqd);
          dDuty = dDuty_Rqd;
          }
        else
          dDuty = 0.0;
        const double h1 = Prod.totHf();
        //Prod.SetPress(dPout);
        Prod.P = dPout;
        Prod.Set_totHf(h1); //restore enthalpy
        }
      //VLE.QPFlash(Fo, IOP_Flng(Out), 0.0, VLEF_Null);
      dTout = Prod.T;
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

void CPumpValve::ClosureInfo(MClosureInfo & CI)
  {//ensure heat balance
  if (CI.DoFlows())
    {
    //CI.m_PowerIn+=dDuty;
    }
  }

//====================================================================================
