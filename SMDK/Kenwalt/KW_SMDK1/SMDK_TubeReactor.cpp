//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "SMDK_TubeReactor.h"

#define dbgModels 1

//====================================================================================

static double LMTD(double TbTi, double TbTo, double ShTi, double ShTo)
  {
  const double gttd = ShTo - TbTi;
  const double lttd = ShTi - TbTo;
  //Rather test for if gttd and/or lttd are zero; or if gttd and lttd are opposite signs!
  //return (gttd==lttd) ? gttd : (gttd-lttd)/log(gttd/lttd);
  return (gttd==lttd) ? gttd : (gttd-lttd)/log(GTZ(gttd)/(GTZ(lttd)));
  }

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
const long idShellI = 2;
const long idShellO = 3;

static MInOutDefStruct s_IODefs[]=
  {
  //  Desc;             Name;       PortId; Rqd; Max; CnId, FracHgt;  Options;
    { "Tube In",       "TubeI",     idTubeI,   1,  10,    0,    1.0f,  MIO_In |MIO_Material },
    { "Tube Out",      "TubeO",     idTubeO,   1,   1,    0,    1.0f,  MIO_Out|MIO_Material },
    { "Shell In",      "ShellI",    idShellI,  1,  10,    1,    1.0f,  MIO_In |MIO_Material },
    { "Shell Out",     "ShellO",    idShellO,  1,   1,    1,    1.0f,  MIO_Out|MIO_Material },
    { NULL },
  };

static double Drw_CTubeReactor[] = 
  { 
  MDrw_Poly, -10,1, 10,1,
  MDrw_Poly, -10,-1, 10,-1,

  MDrw_Poly, -9,3, -9,1,
  MDrw_Poly, -9,-1, -9,-2, 8,-2, 8,-3,

  MDrw_Poly, 9,-3, 9,-1,
  MDrw_Poly, 9,1, 9,2, -8,2, -8,3,

  MDrw_End 
  };

//---------------------------------------------------------------------------
DEFINE_TRANSFER_UNIT(CTubeReactor, "TubeReactor", DLL_GroupName)

void CTubeReactor_UnitDef::GetOptions()
  {
  SetDefaultTag("TR", true);
  SetDrawing("HeatExchange", Drw_CTubeReactor);
  SetTreeDescription("Demo:Tube Reactor");
  SetDescription("TODO: A description");
  SetModelSolveMode(MSolveMode_Probal);
  SetModelGroup(MGroup_Energy);
  SetModelLicense(MLicense_HeatExchange);
  };

//---------------------------------------------------------------------------

CTubeReactor::CTubeReactor(MUnitDefBase * pUnitDef, TaggedObject * pNd) : 
MBaseMethod(pUnitDef, pNd),
m_RB(this, false, "RB"),
m_VLE(this, VLEF_QPFlash, "VLE"),
m_FTC(this)
  {
  //default values...
  m_lOpMode     = 0;
  m_dHTC        = 0.98;
  m_dArea       = 1;
  m_dUA         = 0;
  m_dLMTD       = 0;
  m_dDuty       = 0;
  m_dLMTDFactor = 1;
  }

//---------------------------------------------------------------------------

CTubeReactor::~CTubeReactor()
  {
  }

//---------------------------------------------------------------------------

void CTubeReactor::Init()
  {
  SetIODefinition(s_IODefs);
  //Dbg.PrintLn("%-30s   %s ", m_FTC.TypeInfo()->name(), m_FTC.TypeInfo()->raw_name());
  RegisterFlashTrainNode(idShellI, true, m_FTC.TypeInfo());
  }

//---------------------------------------------------------------------------

const long OM_Simple      = 0;
const long OM_Condensing  = 1;

void CTubeReactor::BuildDataFields()
  {
  static MDDValueLst DDOpMode[]=
    {
      {OM_Simple,      "Simple"},
      {OM_Condensing,  "Condensing"},
      {0}
    };

  // DD.CheckBox("On",          "", DC_,     "",         &m_fOn,            this, isParm|SetOnChange);
  m_RB.OnOffCheckBox();
  DD.Long  ("OpMode",    "",     1,                 MF_PARAMETER|MF_SET_ON_CHANGE, DDOpMode);
  DD.Text("");
  DD.Double("HTC",       "",     &m_dHTC,           MF_PARAMETER, MC_HTC("kW/m^2.K"));
  DD.Double("Area",      "",     &m_dArea,          MF_PARAMETER, MC_Area("m^2"));

  DD.Double("U*A",       "",     &m_dUA,            MF_RESULT, MC_UA("kW/K"));
  DD.Double("LMTD",      "",     &m_dLMTD,          MF_RESULT, MC_dT("C"));
  DD.Double("Duty",      "",     &m_dDuty,          MF_RESULT, MC_Pwr("kW"));
  DD.Double("LMTDFact",  "",     &m_dLMTDFactor,    MF_RESULT, MC_Frac("%"));
  DD.Text("");

  m_FTC.BuildDataFields();

  m_RB.BuildDataFields();
  }

//---------------------------------------------------------------------------

bool CTubeReactor::ExchangeDataFields()
  {
  if (m_FTC.ExchangeDataFields())
    return 1;
  switch (DX.Handle)
    {
    case 1: 
      if (DX.HasReqdValue)
        m_lOpMode=DX.Long;
      DX.Long=m_lOpMode;
      return true;
    }
  return false;
  }

//---------------------------------------------------------------------------

// NOTE: if the tears in a flash train are moved to the slurry lines connecting the heaters the resulting convergence
// is much improved. This code snippet will set this up automatically
bool CTubeReactor::ConfigureJoins()
  { 
  if (MBaseMethod::ConfigureJoins())
    {
    // If a tear is needed then tear the slurry lines first
    for (int i=0; i<FlwIOs.Count; i++)
      {
      MFlow F=FlwIOs[i];
      if (F.Id>=idTubeI && F.Id<=idTubeO)
        F.TearPriority=MTP_First;
      }
  
    return true;
    }
  return false;
  }

//---------------------------------------------------------------------------

bool CTubeReactor::EvalJoinPressures()
  {
  if (IsNetProbal)
    {//set pressures at each join (pipes connected to unit)
    for (int j=0; j<Joins.Count; j++)
      {
      double Pj=Joins[j].GetProbalPIn();

      if (j==1 && m_FTC.Active)
        m_FTC.SuctionP=Pj;

      Joins[j].SetProbalP(Pj, true, true);
      
      #if dbgFlwNode
      Dbg.PrintLn("EJP: %12.2f  %s[%i]", Pm, Tag, j );
      #endif
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
//
//
//
//---------------------------------------------------------------------------

class CSimpleSolverFn : public MRootFinder
  {
  public:
    CSimpleSolverFn(CTubeReactor * pTH, MStream &TubeI, MStream &ShellI, MStream &TubeO, MStream &ShellO) : \
      MRootFinder("A Desc" ,s_Tol),
      m_TR(*pTH), m_TubeI(TubeI), m_ShellI(ShellI), m_TubeO(TubeO), m_ShellO(ShellO) 
      {
      }
    double Function(double TubeTOut) 
      {           
      //m_TR.m_RB.EvalProducts(m_TubeO);    
      //double dHf=m_TR.m_RB.HfSumRct();

      double q = m_TubeI.totHf(MP_All, TubeTOut)-m_TubeI.totHf(MP_All, m_TubeI.T);
      m_TubeO.Set_totHf(m_TubeI.totHf()+q);
      m_ShellO.Set_totHf(m_ShellI.totHf()-q);
      
      double lmtd=LMTD(m_TubeI.T, TubeTOut, m_ShellI.T , m_ShellO.T);
      double qp = m_TR.m_dUA*lmtd;

      m_TR.m_dDuty=qp;
      m_TR.m_dLMTD=lmtd;

      //m_TR.Dbg.PrintLn("%10.3f %10.4f %10.4f %10.4f %10.4f %10.4f", 
      //  K2C(TubeTOut), q+qp, K2C(m_TubeI.T), K2C(m_TubeO.T), K2C(m_ShellI.T), K2C(m_ShellO.T));
      return q-qp;
      };

  public:
    MStream &m_TubeI;
    MStream &m_ShellI;
    MStream &m_TubeO;
    MStream &m_ShellO;
    //  double Duty;

    CTubeReactor &m_TR;
    static MToleranceBlock s_Tol;
  };

MToleranceBlock CSimpleSolverFn::s_Tol(TBF_Both, "TubeReactor:Simple", 0.00005, 0, 100);

//---------------------------------------------------------------------------

void CTubeReactor::DoSimpleHeater(MStream & ShellI, MStream & TubeI, MStream & ShellO, MStream & TubeO)
  {
  TubeO = TubeI;
  ShellO = ShellI;

  if (TubeI.MassFlow()>0 && ShellI.MassFlow()>0)
    {
    double TTi=TubeI.T;
    double STi=ShellI.T;
    long   nSegs=10;

    m_dUA=m_dArea*m_dHTC;

    m_RB.EvalProducts(TubeO);    
    double dHf=m_RB.HfSumRct();

    //for (int i=0; i<nSegs; i++)
      {

      CSimpleSolverFn Fn(this, TubeI, ShellI, TubeO, ShellO);

      double TubeOutT;
      double MnTbOutT=TubeI.T;// No Transfer
      double MxTbOutT=ShellI.T-0.001;
      // simplistic protection against crossover
      // assume all vapour condensed
      double qShell=ShellI.totHf()-ShellO.totHf(MP_All, TubeI.T, ShellI.P);
      double qTube=TubeI.totHf(MP_All, ShellI.T, TubeI.P)-TubeI.totHf();
      
      if (qShell<qTube) // Limited By Shell - Tube TOut Limited
        MxTbOutT=MnTbOutT+(qShell)/GTZ(qTube)*(MxTbOutT-MnTbOutT);

      switch (Fn.FindRoot(0, MnTbOutT, MxTbOutT))
        {
        case RF_OK:       TubeOutT = Fn.Result();   break;
        case RF_LoLimit:  TubeOutT = MnTbOutT;      break;
        case RF_HiLimit:  TubeOutT = MxTbOutT;      break;
        default: 
          Log.Message(MMsg_Error, "TubeOutT not found - RootFinder:%s", Fn.ResultString(Fn.Error())); 
          TubeOutT=Fn.Result();
          break;
        }
      }
    }

  };

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------

class CCondensingSolverFn : public MRootFinder
  {
  public:
    MStream & m_TubeI;
    MStream & m_ShellI;
    MStream & m_TubeO;
    MStream & m_ShellO;

    double    m_SatT;
    double    m_SatP;
    double    m_qDeSupHt;
    double    m_FdVapFrac;

    CTubeReactor &m_TR;
    MVLEBlk      &m_VLE;
    static MToleranceBlock s_Tol;

    double Function(double TubeTOut) 
      {           
      //m_TR.m_RB.EvalProducts(m_TubeO);    
      //double dHf=m_TR.m_RB.HfSumRct();

      double q = m_TubeI.totHf(MP_All, TubeTOut)-m_TubeI.totHf(MP_All, m_TubeI.T);
      m_TubeO.Set_totHf(m_TubeI.totHf()+q);

      if (q<m_qDeSupHt)
        {
        m_TR.m_VLE.SetFlashVapFrac(m_ShellO, m_FdVapFrac, 0);
        m_ShellO.P=m_SatP;
        m_ShellO.Set_totHf(m_ShellI.totHf()-q);
        }
      else
        {
        m_ShellO.Set_totHf(m_ShellI.totHf()-q);
        m_TR.m_VLE.PFlash(m_ShellO, m_SatP, 0, 0);
        }

      if (m_ShellO.T<m_ShellI.T)
        {
        // CrossOver
        }

      //bool CondensingLMTD = true; // condensing version of LMTD 
      //double gttd = m_ShellO.T - m_TubeI.T;
      //double lttd = (CondensingLMTD ? m_ShellO.T:m_ShellI.T)-m_TubeO.T;
      //double lmtd = (gttd=lttd) ? lmtd = gttd : (gttd-lttd)/log(GTZ(gttd)/(GTZ(lttd)));
      double lmtd=LMTDSaturation(m_TubeI.T, m_TubeO.T, m_ShellO.T);
      double qp = m_TR.m_dUA*lmtd;

      m_TR.m_dDuty=qp;
      m_TR.m_dLMTD=lmtd;

      m_TR.Dbg.PrintLn("TO:%10.3f q:%10.4f qp:%10.4f TbHf:%10.4f ShHf:%10.4f(%10.4f) Tube:%10.4f %10.4f Shell:%10.4f %10.4f %10.4f", 
        TubeTOut, q, qp, m_TubeO.totHf(), m_ShellO.totHf(), m_ShellO.MassFrac(MP_Gas),
        m_TubeI.T, m_TubeO.T, 
        m_ShellI.T, m_ShellO.T, m_ShellO.P);
      return q-qp;
      };

    CCondensingSolverFn(CTubeReactor * pTH, MStream &TubeI, MStream &ShellI, MStream &TubeO, MStream &ShellO) : 
      MRootFinder("A Desc" ,s_Tol),
      m_TR(*pTH), 
      m_TubeI(TubeI),
      m_ShellI(ShellI),
      m_TubeO(TubeO),
      m_ShellO(ShellO),
      m_VLE(pTH->m_VLE)
      {
      m_FdVapFrac=m_VLE.FlashVapFrac(ShellI);
      m_SatP=ShellI.P;
      m_SatT=ShellI.SaturationT(ShellI.P);
      m_qDeSupHt=0;
      if (m_SatT<ShellI.T)
        m_qDeSupHt=ShellI.totHf()-ShellI.totHf(MP_All, m_SatT, m_SatP);

      m_VLE.SetFlashVapFrac(ShellO,m_FdVapFrac, 0);
      ShellO.SetTP(m_SatT, m_SatP);
      }
  };

MToleranceBlock CCondensingSolverFn::s_Tol(TBF_Both, "TubeReactor:Condensing", 0.00005, 0, 100);

//===========================================================================
//
//  Condensate Finder
//
//===========================================================================

class CCondensateFinder : public MRootFinder
  {
  public:
    CCondensateFinder(CTubeReactor * pTH, MStream &TubeI, MStream &ShellI, MStream &TubeO, MStream &ShellO);
    ~CCondensateFinder();
    double Function(double x);

  protected:
    MStream & m_TubeI;
    MStream & m_ShellI;
    MStream & m_TubeO;
    MStream & m_ShellO;

    CTubeReactor &m_TR;
    MVLEBlk      &m_VLE;

    static MToleranceBlock s_Tol;
  };

MToleranceBlock CCondensateFinder::s_Tol(TBF_Both, "CTubeReactor:CondensFinder", 0.0, 1.0e-12);

//---------------------------------------------------------------------------

CCondensateFinder::CCondensateFinder(CTubeReactor * pTH, MStream &TubeI, MStream &ShellI, MStream &TubeO, MStream &ShellO) : 
MRootFinder("A Desc" ,s_Tol),
m_TR(*pTH), 
m_TubeI(TubeI),
m_ShellI(ShellI),
m_TubeO(TubeO),
m_ShellO(ShellO),
m_VLE(pTH->m_VLE)
  {
  m_TubeO.SuppressErrors=true;
  m_ShellO.SuppressErrors=true;
  };

//---------------------------------------------------------------------------

CCondensateFinder::~CCondensateFinder()
  {
  m_TubeO.SuppressErrors=false;
  m_ShellO.SuppressErrors=false;
  };

//---------------------------------------------------------------------------

double CCondensateFinder::Function(double Amount)
  {
  m_ShellO.SetM(m_ShellI, MP_All, Amount, m_ShellI.P);
  double ShtotHf=m_ShellO.totHf();

  //double FTemp=Max(m_TubeO.T, m_ShellI.SaturationT());
  double FTemp=m_ShellI.SaturationT();
  double FPress=m_ShellI.SaturationP();
  m_VLE.SetFlashVapFrac(m_ShellO, FTemp, FPress, 0.0, VLEF_Null);
  m_ShellO.SetTP(FTemp, FPress);//m_ShellO.m_pCd->Temp();
  m_TR.m_dDuty=-(m_ShellO.totHf()-ShtotHf);

  m_TubeO.Set_totHf(m_TubeI.totHf()+/*m_TubeO.Sgn**/m_TR.m_dDuty/*+m_TubeO.dHEnv+m_ShellO.dHEnv*/);

  double TDuty;
  if (m_TR.m_dUA>1.0e-6)
    {
    //bool CondensingLMTD = true; // condensing version of LMTD 
    //double gttd  = m_ShellO.T - m_TubeI.T;
    //double lttd  = (CondensingLMTD ? m_ShellO.T:m_ShellI.T)-m_TubeO.T;
    //m_TR.m_dLMTD = (gttd=lttd) ? gttd : (gttd-lttd)/log(GTZ(gttd)/(GTZ(lttd)));
    m_TR.m_dLMTD = LMTDSaturation(m_TubeI.T, m_TubeO.T, m_ShellO.T);
    TDuty = m_TR.m_dUA * m_TR.m_dLMTD * m_TR.m_dLMTDFactor;
    }
  else
    {
    m_TR.m_dLMTD=0.0;
    TDuty = 0.0;
    }

  double Diff=m_TR.m_dDuty-TDuty;

  #if dbgModels
  m_TR.Dbg.PrintLn("CF: X:%12.4f Diff:%14.4f lmtd:%7.2f D1:%10.2f D2:%10.2f sTi:%7.2f sTo:%7.2f tTi:%7.2f tTo:%7.2f sdH:%14.2f tdH:%14.4f Vf:%8.4f ",//%s",
    Amount, 
    Diff,
    m_TR.m_dLMTD,
    m_TR.m_dDuty, TDuty, 
    m_ShellI.T, m_ShellO.T, m_TubeI.T, m_TubeO.T, 
    ShtotHf-m_ShellO.totHf(), m_TubeI.totHf()-m_TubeO.totHf(), 
    m_ShellI.MassFrac(MP_Gas));//, Blk.FullObjTag());
  #endif

  return Diff;
  };

//---------------------------------------------------------------------------

void CTubeReactor::DoCondensingHeater(MStream & ShellI, MStream & TubeI, MStream & ShellO, MStream & TubeO)
  {
  TubeO = TubeI;
  ShellO = ShellI;

  if (TubeI.MassFlow()>0 && ShellI.MassFlow()>0)
    {
    double TTi=TubeI.T;
    double STi=ShellI.T;

    m_dUA=m_dArea*m_dHTC;

    m_RB.EvalProducts(TubeO);    
    double dHf=m_RB.HfSumRct();

    m_FTC.VapourFlow=ShellI.MassFlow();

    if (m_FTC.Active)
      {//operating as part of a flash train macro model, must calculate steam demand...
      double MaxVapFlow=1;

      m_FTC.VapourFlow = ShellI.MassFlow();

      //CCondensingSolverFn Fn(this, TubeI, ShellI, TubeO, ShellO);
      CCondensateFinder Fn(this, TubeI, ShellI, TubeO, ShellO);
      // Now Solve
      switch (Fn.FindRoot(0, 0, MaxVapFlow))//MnTbOutT, MxTbOutT))
        {
        case RF_OK:      m_FTC.VapourFlowReqd = Fn.Result();       break;
        case RF_LoLimit: m_FTC.VapourFlowReqd = MaxVapFlow*1e-6;   break;
        case RF_HiLimit: m_FTC.VapourFlowReqd = MaxVapFlow;        break;
        default: 
          Log.Message(MMsg_Error, "Condesate Flw not found - RootFinder:%s", Fn.ResultString(Fn.Error())); 
          break;
        }
      }
    else
      {//operating as a stand alone heater (ie NOT in flash train)
      // Temperature Range Limits
      double TubeOutT;
      double MnTbOutT=TubeI.T;// No Transfer
      double MxTbOutT=ShellI.T-0.001;//C2K(2000);

      // simplistic protection against crossover
      // assume all vapour condensed
      double qCond=ShellI.totHf();
      m_VLE.SetFlashVapFrac(ShellO, 0, 0);
      ShellO.SetTP(ShellI.T, ShellI.P);
      qCond=qCond-ShellO.totHf(MP_All);//, ShellI.SaturationT(ShellI.P), ShellI.P);

      double qHeat=TubeI.totHf(MP_All, MxTbOutT)-TubeI.totHf();

      if (qHeat>qCond)
        {
        TubeO.Set_totHf(TubeI.totHf()+qCond);
        MxTbOutT=TubeO.T;
        }

      CCondensingSolverFn Fn(this, TubeI, ShellI, TubeO, ShellO);
      // Now Solve
      switch (Fn.FindRoot(0, MnTbOutT, MxTbOutT))
        {
        case RF_OK:       TubeOutT = Fn.Result();   break;
        case RF_LoLimit:  TubeOutT = MnTbOutT;      break;
        case RF_HiLimit:  TubeOutT = MxTbOutT;      break;
        default: 
          Log.Message(MMsg_Error, "TubeOutT not found - RootFinder:%s", Fn.ResultString(Fn.Error())); 
          TubeOutT=Fn.Result();
          break;
        }

      double s0=ShellI.totHf();
      double s1=ShellO.totHf();
      double t0=TubeI.totHf();
      double t1=TubeO.totHf();
      }

    if (m_FTC.Active)
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
  };

//---------------------------------------------------------------------------

void CTubeReactor::EvalProducts()
  {
  try           
    {
    MStreamI TubeI;
    MStreamI ShellI;
    MStream & TubeO = FlwIOs[FlwIOs.First[idTubeO]].Stream; //get reference to the actual output stream
    MStream & ShellO = FlwIOs[FlwIOs.First[idShellO]].Stream; //get reference to the actual output stream

    FlwIOs.AddMixtureIn_Id(TubeI, idTubeI);
    FlwIOs.AddMixtureIn_Id(ShellI, idShellI);

    TubeO = TubeI;
    ShellO = ShellI;

    switch (m_lOpMode)
      {
      case OM_Simple:       DoSimpleHeater(ShellI, TubeI, ShellO, TubeO); break;
      case OM_Condensing:   DoCondensingHeater(ShellI, TubeI, ShellO, TubeO); break;
      }

    double tt=TubeO.T;
    double st=ShellO.T;

    double ShellPi=ShellI.P;
    double ShellPo=ShellO.P;
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

void CTubeReactor::ClosureInfo(MClosureInfo & CI)
  {
  if (CI.DoFlows())
    {
    //  HtIn += dActualDuty; //ensure heat balance
    }
  }

//====================================================================================
