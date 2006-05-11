//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdarg.h>

#define __FLWEQN_CPP

#include "flweqn.h"
#include "executiv.h"
#include "dbgmngr.h"
#include "vlebase.h"
                                                        
//#include "optoff.h"

#if (dbgFlwEqns)
static CDbgMngr dbgFlwEqnData          ("FlwNets", "FlwEqnData");
static CDbgMngr dbgThermals            ("FlwEqns", "Thermals");
static CDbgMngr dbgFlwEqnModes         ("FlwEqns", "FlwEqnModes");
#endif

//==========================================================================
//
//
//
//==========================================================================
//                      { Dynamic, InLineIntegral}
double MinAperture[2] = { 0.02,             0.0};
double MinFracHgt[2]  = { -1.0/*0.01*/,     0.0};
double MaxFracHgt[2]  = { 1.0,              1.0};// {0.98, 1.0};

//==========================================================================
//
//
//
//==========================================================================

LPCTSTR FlwEqnTaskStr(eScdFlwEqnTasks Task)
  {
  switch (Task)
    {
    case FET_GetMode: return "GetMode";
    case FET_SetQm:   return "SetQm  ";
    case FET_CalcDP:  return "CalcDP ";
    default:          return "FET??? ";
    }
  };

LPCTSTR FBModeStr(long Mode)
  {
  switch (Mode)//&~FBMode)
    {
    case FBMode_Full:   return "FBM_Full  ";
    case FBMode_Xfer:   return "FBM_Xfer  ";
    case FBMode_Linear: return "FBM_Linear";
    case FBMode_Simple: return "FBM_Simple";
    default:            return "FBM???    ";
    }
  };

LPCTSTR FlowModeStr(long Mode)
  {
  switch (Mode)///&~LFM_Propagate)
    {
    case 0:                         return "           ";
    case LFM_Full:                  return "LFM_Full   ";
    case LFM_Xfer:                  return "LFM_Xfer   ";
    case LFM_Linear:                return "LFM_Linear ";
    case LFM_Simple:                return "LFM_Simple ";
    case LFM_Full|LFM_Propagate:    return "LFM_Full*  ";
    case LFM_Xfer|LFM_Propagate:    return "LFM_Xfer*  ";
    case LFM_Linear|LFM_Propagate:  return "LFM_Linear*";
    case LFM_Simple|LFM_Propagate:  return "LFM_Simple*";
    default:                        return "LFM???     ";
    }
  };
//==========================================================================
//
//
//
//==========================================================================

long CPressBlk::sm_iNo=0;

CPressBlk::CPressBlk()
  {
  m_iNo=sm_iNo++;
  Init();
  }
void CPressBlk::Init()
  {
  P=Std_P;
  PEst=Std_P;
  PMn=Std_P;

  PGood=Std_P;

  m_PStdyStart=-1;

  m_PMax=fNAN;
  m_PMaxLim=fNAN;
  };

//==========================================================================
//
//
//
//==========================================================================

void CSpPropInfo::SetPropInfoVL(SpModel & Md, double VVolFrac, double LVolFrac, SpMArray * pImage)
  {
  bool Mv=Md.Mass(som_Gas)>UsableMass;
  bool Ml=Md.Mass(som_SL)>UsableMass;
  if (Mv||Ml)
    {
    if (pImage)
      {
      int xxx=0; //INCOMPLETECODE();
      }
    m_dTemp=Md.Temp();
    m_dPress=Md.Press();
    m_dVisc=Md.DynamicViscosity();
    if (Ml)
      m_dRhoL=Md.Rho(som_SL, m_dTemp, m_dPress, pImage);
    if (Mv)
      m_dRhoV=Md.Rho(som_Gas, m_dTemp, m_dPress, pImage);
    m_dVVolFrac=VVolFrac;
    m_dLVolFrac=LVolFrac;
    }
//  dVVolFracBal=0;
  };

//--------------------------------------------------------------------------

void CSpPropInfo::SetPropInfoX(SpConduit & Cd)
  {
  flag Mv=Cd.QMass(som_Gas)>UsableMass;
  flag Ml=Cd.QMass(som_SL)>UsableMass;

  if (Mv||Ml)
    {
    m_dTemp=Cd.Temp();
    m_dPress=Cd.Press();
    m_dVisc=Cd.DynamicViscosity();
    if (Ml)
      m_dRhoL=Cd.Rho(som_SL);
    if (Mv)
      m_dRhoV=Cd.Rho(som_Gas);
    m_dVVolFrac=Cd.VolFrac(som_Gas, m_dTemp, m_dPress);
    m_dLVolFrac=1.0-m_dVVolFrac;
    }
  };

//--------------------------------------------------------------------------

double CSpPropInfo::NettVVolFrac()
  {
//  m_dVVolFracBal=Range(-m_dVVolFrac, m_dVVolFracBal, 1-m_dVVolFrac);
  return m_dVVolFrac;//+m_dVVolFracBal;
  };

//--------------------------------------------------------------------------

double CSpPropInfo::NettLVolFrac()
  {
  m_dLVolFracBal=Range(-m_dLVolFrac, m_dLVolFracBal, 1-m_dLVolFrac);
  return m_dLVolFrac+m_dLVolFracBal;
  };

//--------------------------------------------------------------------------

double CSpPropInfo::AsVMassFrac(double VapVolFrac, double LiqVolFrac, double P)
  {
  double RhV=RhoV(P);
  double RhL=RhoL();
  double MV=RhV*VapVolFrac;
  double ML=RhL*LiqVolFrac;
  return MV/GTZ(ML+MV);
  }

//--------------------------------------------------------------------------

double CSpPropInfo::AsLMassFrac(double VapVolFrac, double LiqVolFrac, double P)
  {
  double RhV=RhoV(P);
  double RhL=RhoL();
  double MV=RhV*VapVolFrac;
  double ML=RhL*LiqVolFrac;
  return ML/GTZ(ML+MV);
//  double RhV=RhoV(P);
//  double RhL=RhoL();
//  return (RhL*LiqVolFrac)/GTZ(RhV*VapVolFrac+RhL*LiqVolFrac);
  }

/*
double AsVMassFrac(double VapVolFrac, double P)
  {
  double RhV=RhoV(P);
  double RhL=RhoL();
  return (RhV*VapVolFrac)/(RhL*(1-VapVolFrac)+RhV*VapVolFrac);
  }
double AsLMassFrac(double LiqVolFrac, double P)
  {
  double RhV=RhoV(P);
  double RhL=RhoL();
  return (RhL*LiqVolFrac)/(RhV*(1-LiqVolFrac)+RhL*LiqVolFrac);
  }
*/

//==========================================================================
//
//
//
//==========================================================================

IMPLEMENT_MEMSTATS(CFlwEqn)

CFlwEqn::CFlwEqn(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  TaggedObject(pClass_, pTag, pAttach, eAttach)
  {
  m_dOverideTauRqd=dNAN;
  m_dOverideRhoLRqd=dNAN;
  m_dOverideRhoVRqd=dNAN;
  m_dOverideTempRqd=dNAN;
  m_dOverideViscRqd=dNAN;

  m_bLinearOn=false;
  m_dLinearDP=1;
  m_dLinearQm=1;
  m_dLinearReg=1;

  m_nEvals=0;

#if WithOEP
  if (OEPOptions())
    m_dOverideTauRqd=2.0;
#endif

  m_pEqnPtr             = NULL;

  m_dViscMeas           = 0.98e-3;
  m_dDensMeas           = 1000;
  m_dVelMeas            = 0;
  m_dVelSonicMeas       = dNAN;
  m_dDP                 = 0;
  //m_dQm                 = 0;

  //m_FlwRqd.m_Options   = 0;
  m_FlwRqd.m_AsMass     = true;
  m_FlwRqd.m_MeasDens   = 1000.0;
  m_FlwRqd.m_ReqdQm     = fNAN;  
  m_FlwRqd.m_ReqdQv     = fNAN;  

  m_FlwRqd.m_RqdQmCap   = fNAN;
  m_FlwRqd.m_MinQmCap   = 0.0;
  m_FlwRqd.m_MaxQmCap   = fNAN;

  m_FlwRqd.m_RqdQvCap   = fNAN;
  m_FlwRqd.m_MinQvCap   = 0.0;
  m_FlwRqd.m_MaxQvCap   = fNAN;
  //m_FlwRqd.m_QvCapRhoLo = 500.0;
  //m_FlwRqd.m_QvCapRhoHi = 2000.0;

  //m_FlwRqd.m_PressOn    = false;
  m_FlwRqd.m_PressDP    = 0.001f;
  m_FlwRqd.m_PressQm    = 1000;
  m_FlwRqd.m_PressQv    = 1;

  };

flag CFlwEqn::DataXchg(DataChangeBlk & DCB)
  {
  return TaggedObject::DataXchg(DCB);
  }

//--------------------------------------------------------------------------

flag CFlwEqn::ValidateData(ValidateDataBlk & VDB)
  {
  if (Valid(m_dOverideTauRqd) && m_dOverideTauRqd<1.0e-6)
    m_dOverideTauRqd=dNAN;
  if (Valid(m_dOverideRhoLRqd) && m_dOverideRhoLRqd<1.0e-3)
    m_dOverideRhoLRqd=dNAN;
  if (Valid(m_dOverideRhoVRqd) && m_dOverideRhoVRqd<1.0e-3)
    m_dOverideRhoVRqd=dNAN;
  if (Valid(m_dOverideTempRqd) && m_dOverideTempRqd<1.0)
    m_dOverideTempRqd=dNAN;
  if (Valid(m_dOverideViscRqd) && fabs(m_dOverideViscRqd)<1e-9)
    m_dOverideViscRqd=dNAN;

  //if (Valid(m_dTRiseRqd) && fabs(m_dTRiseRqd)>1000.0)
  //  m_dTRiseRqd=dNAN;

  //m_dPolyTropicN=Range(1.0,m_dPolyTropicN,1.5);

  return TaggedObject::ValidateData(VDB);
  };

//--------------------------------------------------------------------------

//void CFlwEqn::SetLnkTolerances(double Lnk_Eps_A_Cpy, double Lnk_Eps_R_Cpy)
//  {
//  dLnk_Eps_A_Cpy=Lnk_Eps_A_Cpy;
//  dLnk_Eps_R_Cpy=Lnk_Eps_R_Cpy;
//  }
//
//double CFlwEqn::Lnk_Eps_A() { return dLnk_Eps_A_Cpy; };
//double CFlwEqn::Lnk_Eps_R() { return dLnk_Eps_R_Cpy; };

//--------------------------------------------------------------------------

pchar CFlwEqn::FlwEqnName()
  {
  static Strng S;
  Class()->GetShortDesc(S);
  return S();
  }

//--------------------------------------------------------------------------

void CFlwEqn::SetPhysData(double DZ)
  {
  SetActLength(Max(fabs(DZ), RqdLength()));
  };

//--------------------------------------------------------------------------

//double CFlwEqn::RangeFlow(double Flow, double LoLimit)
//  {
//  ASSERT(TstGTZ(LoLimit));
//  Flow=fabs(Flow);
//  if (Flow>LoLimit)
//    return Flow;
//  /**
//  This fuction will -
//  1 - keep the flow used to measure dPq above 0.25 * LoLimit
//  2 - the resultant curve if smooth
//  3 - the curve is not flat @ 0.0
//  */
//  return Flow+Sqr(LoLimit-Flow)/(3.0*LoLimit);
//  };

//--------------------------------------------------------------------------

void CFlwEqn::BuildDataDefnOveride(DataDefnBlk & DDB, TaggedObject * pThis)
  {
  if (!DDB.ForView() || dbgfile())
    {
    flag DoBlk;

    if (PrjFileVerNo()<22)
      DoBlk=DDB.BeginStruct(this, "Overide", NULL, DDB_NoPage, -1, MarkerClosed);
    else
      DoBlk=DDB.BeginStruct(this, "Override", NULL, DDB_NoPage, -1, MarkerClosed);
    if (DoBlk)
      {
      DDB.Double ("TimeConst"   ,"Tau"  ,DC_Time, "s"      ,&m_dOverideTauRqd  , pThis, isParm|NAN_OK);
      DDB.TagComment(Valid(m_dOverideTauRqd) ? "On":"Off");
      DDB.Double ("DensityL"    ,"RhoL"  ,DC_Rho,  "kg/m^3" ,&m_dOverideRhoLRqd  , pThis, isParm|NAN_OK);
      DDB.Double ("DensityV"    ,"RhoV"  ,DC_Rho,  "kg/m^3" ,&m_dOverideRhoVRqd  , pThis, isParm|NAN_OK);
      DDB.Double ("Temperature" ,"Temp" ,DC_T,    "C"      ,&m_dOverideTempRqd , pThis, isParm|NAN_OK);
      DDB.Double ("Viscosity"   ,"Visc" ,DC_Visc, "cP"     ,&m_dOverideViscRqd , pThis, isParm|NAN_OK);
      //if (PrjFileVerNo()<=33 && DDB.ForFileSnpScn())
      //  DDB.Double ("TempRise"    ,""     ,DC_dT,   "C"    ,&m_dTRiseRqd    , pThis, isParm|NAN_OK);

      }
    DDB.EndStruct();

    if (DDB.BeginStruct(this, "Linear", NULL, DDB_NoPage, -1, MarkerClosed))
      {
      DDB.CheckBoxBtn("On"    ,"" ,DC_,     ""     ,&m_bLinearOn, pThis, isParm);
      DDB.Double("DP"         ,"" ,DC_DP,   "kPa"  ,&m_dLinearDP , pThis, isParm);
      DDB.Double("Qm"         ,"" ,DC_Qm,   "kg/s" ,&m_dLinearQm , pThis, isParm);
      DDB.Double("Regulation" ,"" ,DC_Frac, "%"    ,&m_dLinearReg , pThis, isParm);
      }
    DDB.EndStruct();
    }
  };

//--------------------------------------------------------------------------

//dword CFlwEqn::GetFlags(CFlwBlkBase & FE)
//  {
//  return FE.Flags();
//  };

//--------------------------------------------------------------------------

int CFlwEqn::DoDumpTest=0;
int CFlwEqn::TestLvl=0;

//--------------------------------------------------------------------------

void CFlwEqn::DumpTest(char * Fn, double Lo, double Hi, double Delta,
                      CFlwBlkBase & FE, double Regulation, double P)
  {
  static FILE * f;
  if (TestLvl==0 && Regulation>0.0)
    {
    f=fopen(Fn, "wt");
    if (f)
      {
/*
      double OldQm=FE.Qm;
      double OldmQm=FE.mQm;
      int OldsQm=FE.sQm;
      TestLvl++;
      for (double TstQm=Lo; TstQm<Hi; TstQm+=Delta)
        {
        FE.Qm=TstQm;
        FE.mQm=TstQm;
        FE.sQm=Sign(TstQm);
        FE.P1=P;
        FE.P2=P;
        EvaluateFlwEqn(FE, Regulation, Rho, C, IC);
        fprintf(f, "%g,%g,%g,%g\n",FE.Qm,FE.dPq, FE.dPbdQ, FE.Vel);
        }
      FE.Qm=OldQm;
      FE.mQm=OldmQm;
      FE.sQm=OldsQm;
*/
      TestLvl--;
      fclose(f);
      }
    DoDumpTest=0;
    }
  }

//==========================================================================

void CFlwEqn::EvalProducts(CFlwBlkBase & FE, double Pi, double Po, CFlwThermalBlk & FTB)
  {
  SpConduit &C=FE.Cd();

  double Ti=C.Temp();

  // Heating due to frictional effects
  double PwrLoss=0.981*fabs((Po-Pi)*C.QMass())/GTZ(C.Rho());
  //FTB.AddWorkPower(PwrLoss, Po);

  // PolyTropic Flow
  // n=1 : isothermal
  // n>1 : polytropic
  // n=gamma (Cp/Cv) : adiabatic
  //m_dPolyTropicN=Range(1.0, m_dPolyTropicN, 1.5);
  double GTo=Ti*Pow(Po/Pi,(FTB.PVConst()-1.)/FTB.PVConst());
  double LTo=Ti;
  double GCp=C.totCp(som_Gas);
  double LCp=C.totCp(som_SL);
  double To=(GCp+LCp)>1.0e-10 ? (LTo*LCp+GTo*GCp)/(GCp+LCp) : Ti;
  FTB.AddWorkPower((To-Ti)*C.totCp(), Po);

  // AutoCompression
  double PwrAC=0;
  if (FTB.AutoCompOn())
    {
    //only for flow down ???
    //PwrAC= PhD.Rise()*FE.QmSign()<0.0 ? 0.001*9.81*fabs(PhD.Rise())*C.QMass() : 0.0;
    PwrAC= 0.001*9.81*fabs(m_PhD.Rise()*LScale())*C.QMass();
    #if WithMG
    if (MGOptions())
      PwrAC=0;
    #endif
    FTB.AddACPower(PwrAC, Po);
    }
  }

void CFlwEqn::EvalDiscrete(CFlwBlkBase & FE)
  {
  }

double CFlwEqn::LinearisedDP(double Qm, double Regulation)
  {
  ASSERT(m_bLinearOn);
  return fabs(m_dLinearDP * Qm/m_dLinearQm) * m_dLinearReg/GTZ(Regulation);
  }

//==========================================================================
//
//
//
//==========================================================================

#define DllImportExport
DEFINE_TAGOBJ(VE_None);
class VE_None : public CFlwEqn
  {
  public:

    VE_None(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~VE_None();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);

  };

const LPTSTR NoEqnName  = "None";
const LPTSTR NoEqnGroup = "No Flow Eqn";
CFlwEqnGrp NullFlwGroup(NoEqnGroup, NoEqnName);

IMPLEMENT_FLWEQN(VE_None, NoEqnGroup, NoEqnName, "", TOC_SYSTEM,
                 NoEqnName,
                 "Null CFlwEqn");

VE_None::VE_None(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CFlwEqn(pClass_, pTag, pAttach, eAttach)
  {
  //dbgpln("VE_None::VE_None %#010x", this);
  }

//--------------------------------------------------------------------------

VE_None::~VE_None()
  {
  //dbgpln("VE_None::~VE_None");
  }

//--------------------------------------------------------------------------

void VE_None::BuildDataDefn(DataDefnBlk & DDB)
  {
  }

//--------------------------------------------------------------------------

flag VE_None::ValidateData(ValidateDataBlk & VDB)
  {
  return true;
  }

//--------------------------------------------------------------------------


flag VE_None::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  //dbgpln("VE_None %#010x", this);
  return true;
  }

//==========================================================================
//
//
//
//==========================================================================

DEFINE_TAGOBJ(SA_Xfer);
class SA_Xfer: public CFlwEqn
  {
  public:
    SA_Xfer(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~SA_Xfer();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);

    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);

  public:
  };

CFlwEqnGrp Xfer2AreaGroup("Xfer2Area", "SA_Xfer");

IMPLEMENT_FLWEQN(SA_Xfer, Xfer2AreaGroup.Name(), "SA_Xfer", "", TOC_SYSTEM,
                 "Direct Transfer",
                 "Direct Transfer");
//
SA_Xfer::SA_Xfer(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
CFlwEqn(pClass_, pTag, pAttach, eAttach)
  {
  //dbgpln("SA_Xfer::SA_Xfer %#010x", this);
  }

//--------------------------------------------------------------------------

SA_Xfer::~SA_Xfer()
  {
  //dbgpln("SA_Xfer::~SA_Xfer");
  }

//--------------------------------------------------------------------------

void SA_Xfer::BuildDataDefn(DataDefnBlk & DDB)
  {
  };

//--------------------------------------------------------------------------

flag SA_Xfer::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  //dbgpln("SA_Xfer %#010x", this);
  //FE.SetQmReqd(1000);
  FE.SetXferMode();//(1e10);
  return True;
  };

//==========================================================================
//
//
//
//==========================================================================

byte CFlwBlkData::sm_iFlwPh_Block=FPh_None;
byte CFlwBlkData::sm_iFlwPh_Force=FPh_None;
long CFlwBlkData::sm_iNo=0;

byte   CFlwBlkData::sm_iShowVelLimitAs=FBB_ShowAs_Warn;
double CFlwBlkData::sm_dVelLimitSL=20.0;
double CFlwBlkData::sm_dVelLimitV=100.0;

byte   CFlwBlkData::sm_iShowQmDiffAs=FBB_ShowAs_Off;
CToleranceBlock CFlwBlkData::m_QmMatchTol(TBF_DynSys, "CFlwBlkData:QmMatch", 1.0e-3, 1.0e-1);

CFlwBlkData::CFlwBlkData()
  {
  m_iNo=sm_iNo++;
  m_pNdPB=NULL;
  m_pFlPB=NULL;
  dBstDamping=0.0;
  m_ResTHist.SetSize(0,4);
  m_nResTHist=0;
  m_iResTIndex=0;
  m_iNetLoop=1000;
  
  ResetData(true);//Initialise();
  };

//--------------------------------------------------------------------------

CFlwBlkData::~CFlwBlkData()
  {
  };

//--------------------------------------------------------------------------

void CFlwBlkData::ResetData(flag Complete)
  {
//  iDirn=0;

  m_iDirnRel2Traverse   = 0;
  //m_iDirnRel2Connect    = 0;

  m_sQMeas              = 0;
  m_QmMeas              = 0.0;
  m_MeasType            = 0;
  m_MeasDone            = false;
  //m_QmMeasInit          = 0.0;
  m_MeasRho             = 1.0;
  m_MeasRhoStep         = 1.0;
  m_MeasArea            = 1.0;
  m_MeasLoLimit         = 0.0;
  m_Qm                  = 0.0;

  m_QmEst               = 0.0;
  m_QmEstGood           = 0.0;
  m_QmAvail             = 0.0;
  m_QmSpace             = 0.0;
  m_QmPrvPB             = 0.0;
  m_QmStdyState         = -1.0;

  if (m_pNdPB)
    {
    m_pNdPB->P          = Std_P;
    m_pNdPB->PMax       = fNAN;
    }
  if (m_pFlPB)
    {
    m_pFlPB->P          = Std_P;
    m_pFlPB->PMax       = fNAN;
    }

  m_dPb                 = 0.0;
  m_dPbX                = 0.0;
  m_dPbdQ               = 0.0;
  m_dPa                 = 0.0;
  m_dPadQ               = 0.0;
  m_dPq                 = 0.0;
  m_dPqdQ               = 0.0;
  m_dPz                 = 0.0;
  m_dPzI                = 0.0;
  m_dPzdQ               = 0.0;
  m_dPzX                = 0.0;
  m_dPzXdQ              = 0.0;
  m_dPmB                = 0.0;
  m_dPmQ                = 0.0;
  m_dPmdQ               = 0.0;
  m_ResT                = 0.0;
  m_ResTCalc            = 0.0;
  m_nResTHist           = 0;
  m_iResTIndex          = 0;
  m_iNetLoop            = 1000;
  m_Vel                 = dNAN;
  m_VelSonic            = dNAN;
  m_MomentScl           = 1.0;
  m_Regulation          = 1.0;
  m_pUserValues         = NULL;
  m_nUserValues         = 0;

  m_SimpleRes               = 0.0;

  m_dOverideTau         = dNAN;
  m_dOverideRhoL        = dNAN;
  m_dOverideRhoV        = dNAN;
  m_dOverideTemp        = dNAN;
  m_dOverideVisc        = dNAN;

  fTwoPhase           = false;
  pTwoPhCd            = NULL;
  m_iFlwPh_Rqd          = FPh_Both;
  m_iFlwPh_Act          = FPh_UnKnown;

  if (Complete)
    {
    m_pCd              = NULL;
    //xm_fVFlowAllowed         = false;
    //xm_fLFlowAllowed         = false;
    }

  dVThrottle          = 0.0;
  dLThrottle          = 0.0;

  dSlip               = dNAN;
  dVoidFrac           = dNAN;
  dOnePhPart          = 1.0;
  dTempOut            = Std_T;

  m_fMomentumValid      = false;
  m_fApplyStaticHead    = true;

#if WithOEP
  if (OEPOptions())
    {
    fVFlowAllowed = true;
    fLFlowAllowed = false;
    }
#endif

  m_PropSel.m_iReqd=FBPS_Default;
  m_PropSel.m_iInUse=FBPS_Default;

  m_bClosed=false;
  m_fDerivsBad=false;
  m_fFunctOfPress=false;
  m_fEvalFwdOnly=false;
  m_fEvalRevOnly=false;
  m_fHoldFastConverge=false;

  }

//==========================================================================
//
//
//
//==========================================================================

//--------------------------------------------------------------------------

CFlwEqnPtr::CFlwEqnPtr(CFlwBlkData *pData, long Options, double DPFactor)
  {
  m_FlowMode        = FBMode_Simple;
  m_FMSrc           = NULL;
  m_FMSrcRmt        = false;

  m_pData           = pData;
  m_pParentFEP      = NULL;//this;
  m_pAttachedTo     = NULL;
  m_pEqn            = CreateFlwEqn(NoEqnName);
  m_pEqn->m_pEqnPtr = this;
  m_fDisabled       = false;
  m_dLScl           = 1;
  m_dZScl           = 1;
  m_Options         = 0;
  SetOptions(Options, DPFactor);
  };

//--------------------------------------------------------------------------

CFlwEqnPtr::~CFlwEqnPtr()
  {
  if (m_pParentFEP && m_pParentFEP->m_Children.GetCount())
    {
    POSITION Pos=m_pParentFEP->m_Children.GetHeadPosition();
    while (Pos)
      {
      CFlwEqnPtr *C=m_pParentFEP->m_Children.GetAt(Pos);
      if (C==this)
        break;
      m_pParentFEP->m_Children.GetNext(Pos);
      }
    ASSERT(Pos!=NULL);
    m_pParentFEP->m_Children.RemoveAt(Pos);
    }

  delete m_pEqn;
  };

//--------------------------------------------------------------------------

CFlwEqnPtr & CFlwEqnPtr::operator=(const CFlwEqnPtr & A)
  {
  _asm int 3;
  m_pParentFEP=A.m_pParentFEP;
  m_pEqn=A.m_pEqn;
  m_sGroup=A.m_sGroup;
  m_pAttachedTo=A.m_pAttachedTo;
  m_fDisabled=A.m_fDisabled;
  m_pEqn->m_pEqnPtr=this;

  return *this;
  };

//--------------------------------------------------------------------------

void CFlwEqnPtr::ResetData(flag Complete)
  {
  Eqn()->ResetData(Complete);
  }

void CFlwEqnPtr::SetOptions(long Options, double DPFactor)
  {
  m_Options|=Options;
  FlwRqd().m_PressDP=(float)DPFactor;
  };

// -------------------------------------------------------------------------

void CFlwEqnPtr::SetParentFlwEqn(CFlwEqnPtr *ParentFEP)
  {
  if (m_pParentFEP != ParentFEP)
    {
    Eqn()->StructureChanged(Eqn());
    m_pParentFEP = ParentFEP;
    POSITION Pos=m_pParentFEP->m_Children.GetHeadPosition();
    bool Found=false;
    while (Pos && !Found)
      {
      CFlwEqnPtr *C=m_pParentFEP->m_Children.GetNext(Pos);
      Found=(C==this);
      }
    if (!Found)
      m_pParentFEP->m_Children.AddTail(this);

    Eqn()->StructureChanged(Eqn());
    }
  };

//--------------------------------------------------------------------------

CFlwEqn * CFlwEqnPtr::AssignFlwEqnGroup(CFlwEqnGrp &Group, pchar Rqd, TaggedObject * pAttach)
  {
  CFlwEqnPtr &E=Eqn2Use();
  E.m_pGroup=&Group;
  E.m_sGroup=Group.Name();
  int bNullAllowed=Rqd==NULL || strlen(Rqd)==0;
  E.m_pAttachedTo=pAttach;
  if (E.m_pEqn)
    {
    if (_stricmp(Group.Name(), E.m_pEqn->Group())!=0)
      {
      delete E.m_pEqn;
      E.m_pEqn=NULL;
      }
    }

  if (!E.m_pEqn)
    E.m_pEqn=!bNullAllowed ? CreateFlwEqn(Rqd) : NULL;
  E.m_pEqn->m_pEqnPtr=this;
  return m_pEqn;
  }

//--------------------------------------------------------------------------

CFlwEqn * CFlwEqnPtr::CreateFlwEqn(pchar Name_)
  {
  CFlwEqn * pEq=NULL;
  if (Name_ && strlen(Name_)>0 && _stricmp(Name_,NoEqnName)!=0)
    pEq=(CFlwEqn*)TagObjClass::Construct(GroupName(), Name_, NULL, "CFlwEqn", AttachedTo(), AttachedTo() ? TOA_Embedded : TOA_Free);
  else
    pEq=(CFlwEqn*)TagObjClass::Construct(NoEqnGroup, NoEqnName, NULL, "CFlwEqn", AttachedTo(), AttachedTo() ? TOA_Embedded : TOA_Free);
  pEq->m_pEqnPtr=this;
  ASSERT_ALWAYS(pEq!=NULL, "Eqn Expected");
  return pEq;
  };

// -------------------------------------------------------------------------

CFlwEqn * CFlwEqnPtr::ChangeFlwEqn(pTagObjClass pRqdEqnClass_)
  {
  ASSERT_ALWAYS(pRqdEqnClass_!=NULL, "EqnClass Expected");
  Strng OldName, OldEDesc, OldEMemo, OldEIdStr, OldELocation;
  CFBPhysData OldPhD;
  CFlwEqnFlwRqd OldFR;
  if (Eqn2Use().m_pEqn && (pRqdEqnClass_==NULL || (pRqdEqnClass_ && pRqdEqnClass_!=Eqn2Use().m_pEqn->Class())))
    {
    Eqn()->StructureChanged(Eqn());
    OldPhD =Eqn()->m_PhD;
    OldName=Eqn()->Tag();
    OldEDesc=Eqn()->EqpDesc();
    OldEMemo=Eqn()->EqpMemo();
    OldEIdStr=Eqn()->EqpIdStr();
    OldELocation=Eqn()->EqpLocation();
    OldFR = Eqn()->m_FlwRqd;

    delete Eqn2Use().m_pEqn;
    Eqn2Use().m_pEqn=NULL;
    }
  if (Eqn2Use().m_pEqn==NULL && pRqdEqnClass_)
    {
    Eqn2Use().m_pEqn=(CFlwEqn*)pRqdEqnClass_->Construct(NULL, OldName(), Eqn2Use().m_pAttachedTo, Eqn2Use().m_pAttachedTo ? TOA_Embedded : TOA_Free);
    if (Eqn2Use().m_pEqn)
      {
      Eqn()->SetEqpDesc(OldEDesc());
      Eqn()->SetEqpMemo(OldEMemo());
      Eqn()->SetEqpIdStr(OldEIdStr());
      Eqn()->SetEqpLocation(OldELocation());
      Eqn()->m_PhD=OldPhD;
      Eqn()->m_FlwRqd=OldFR;
      Eqn()->m_pEqnPtr=this;
      }
    }
  ASSERT_ALWAYS(Eqn2Use().m_pEqn!=NULL, "Eqn Expected");
  return Eqn2Use().m_pEqn;
  };

// -------------------------------------------------------------------------

CFlwEqn * CFlwEqnPtr::ChangeFlwEqn(pchar Name_, flag ChgDefault)
  {
  Strng OldName, OldEDesc, OldEMemo, OldEIdStr, OldELocation;
  CFBPhysData OldPhD;
  CFlwEqnFlwRqd OldFR;
  //if (Name_ && (strlen(Name_)==0 || _stricmp(Name_,NoEqnName)==0))
  //  Name_=NULL;

  if (Eqn2Use().m_pEqn && (Name_==NULL || (Name_ && _stricmp(Name_, Eqn2Use().m_pEqn->FlwEqnName())!=0)))
    {
    Eqn()->StructureChanged(Eqn());
    OldPhD =Eqn()->m_PhD;
    OldName=Eqn()->Tag();
    OldEDesc=Eqn()->EqpDesc();
    OldEMemo=Eqn()->EqpMemo();
    OldEIdStr=Eqn()->EqpIdStr();
    OldELocation=Eqn()->EqpLocation();
    OldFR = Eqn()->m_FlwRqd;
    delete Eqn2Use().m_pEqn;
    Eqn2Use().m_pEqn=NULL;
    }
  if (Eqn2Use().m_pEqn==NULL && Name_)
    {
    Eqn2Use().m_pEqn=CreateFlwEqn(Name_);
    Eqn()->SetEqpDesc(OldEDesc());
    Eqn()->SetEqpMemo(OldEMemo());
    Eqn()->SetEqpIdStr(OldEIdStr());
    Eqn()->SetEqpLocation(OldELocation());
    Eqn()->m_PhD=OldPhD;
    Eqn()->m_FlwRqd=OldFR;
    Eqn()->m_pEqnPtr=this;
  
    ////TODO Find Default
    }
  ASSERT_ALWAYS(m_pEqn!=NULL, "Eqn Expected");
  return m_pEqn;
  };

//--------------------------------------------------------------------------

void CFlwEqnPtr::SetFlowModeSrc(FlwNode * p, bool IsRmt)
  { 
  if (p)
    {
    if (IsRmt)
      {
      if (m_FMSrc==NULL || m_FMSrcRmt)
        {
        m_FMSrc=p;
        m_FMSrcRmt=IsRmt;
        }
      }
    else
      {
      m_FMSrc = p;
      m_FMSrcRmt=IsRmt;
      }
    }
  else
    {
    m_FMSrc=NULL;
    m_FMSrcRmt=false;
    }
  };

//--------------------------------------------------------------------------

long CFlwEqnPtr::FlowMode()//bool MustHaveSrc)    
  { 
  if (m_FMSrc)
    return m_FMSrc->FlowMode();
  //ASSERT_ALWAYS(!MustHaveSrc, "m_FMSrc should be Set");
  return m_FlowMode;
  };
//long m=AttachedTo()->FlowMode(); return m ? m : AttachedTo()->FlowModeReqd() ; };//m_FlowMode; };

//--------------------------------------------------------------------------

void CFlwEqnPtr::SetFlowMode(long Md)
  { 
  m_FlowMode=Md; 
  };

//--------------------------------------------------------------------------

void CFlwEqnPtr::BuildDataDefn(long DDOptions, DataDefnBlk & DDB, pTaggedObject pOwnObj, char * FBTag, DWORD UserInfo, DDBPages PgOpt)
  {
  if (!FBTag)
    FBTag = "Eqn";

  long LclOptions=Options()|DDOptions;

  flag SkipIt=false;
  flag DoIt;
  if (PrjFileVerNo()<22)
    DoIt=DDB.BeginStruct(pOwnObj, FBTag, NULL, PgOpt, UserInfo);
  else if ((LclOptions & FBDDOpt_HideEqnStruct)==0)
    {
    SkipIt=DDB.ForFileSnpScn() && (PrjFileVerNo()>=95);
    DoIt=!SkipIt && DDB.BeginObject(pOwnObj, FBTag, "FB_Eqn", NULL, PgOpt, UserInfo);
    }
  else
    DoIt=true;
  if (!SkipIt)
    {
    if (DoIt)
      {
      if (LclOptions & FBDDOpt_WithEqn)//Group)
        DDB.String  ("Group", "", DC_ , "", xidFlwEqnGrp, pOwnObj, isParmConstruct|/*InitHidden|*//*noFile|*/noSnap|noScenario);//, DDB0());
      if (GroupOK())
        {
        if ((PrjFileVerNo()>=23) && (LclOptions & FBDDOpt_WithXtraEqn))
          {
          if (DDB.BeginStruct(pOwnObj, "FB", NULL, DDB_NoPage, -1, MarkerClosed))
            {
            DDB.String ("Desc",     "",       DC_,      "",       xidFlwEqnDesc,      pOwnObj, isParm);
            DDB.String ("IdStr",    "",       DC_,      "",       xidFlwEqnIdStr,     pOwnObj, isParm|InitHidden);
            DDB.String ("Location", "",       DC_,      "",       xidFlwEqnLocation,  pOwnObj, isParm|InitHidden);
            DDB.Double ("RqdLen",   "",       DC_L,     "m",      xidFlwEqnRqdLen,    pOwnObj, isParmStopped);
            DDB.Double ("ActLen",   "",       DC_L,     "m",      xidFlwEqnActLen,    pOwnObj, isParmStopped);
            DDB.Double ("Diam",     "",       DC_L,     "mm",     xidFlwEqnDiam,      pOwnObj, isParm);
            DDB.Double ("",         "DZ",     DC_L,     "mm",     xidFlwEqnDZ,        pOwnObj, isParm);
            DDB.Double ("DatumFlng",  "",     DC_L,     "m",      xidFlwEqnDatumFlng, pOwnObj, isParm);
            DDB.Double ("DatumNode",  "",     DC_L,     "m",      xidFlwEqnDatumNode, pOwnObj, isParm);
            }
          DDB.EndStruct();
          }

        long Md = FlowMode();//
        bool ForNDB=(Md==0);// (DDB.Options()|DDEF_NDBIMPORT)!=0;

        if (((Md==FBMode_Full) || ForNDB) && (LclOptions & FBDDOpt_WithEqn) || DDB.ForFileSnpScn())
        //if (pOwnObj->SolveBufferedMethod())
          {
          DDBValueLstMem DDB0;
          TagObjClass::GetSDescValueLst(GroupName(), DDB0, NoEqnName);
          DDB.String  ("Name",        "", DC_, "", xidFlwEqnNm,   pOwnObj, ((DDB0.Length()>1?isParm:0)|SetOnChange), DDB0());
          DDB.Long    ("Evaluations", "", DC_, "", &Eqn()->m_nEvals,   pOwnObj, isResult|InitHidden);
          //Eqn()->BuildDataDefn(DDB);
          }

        //if ((Md==FBMode_Simple || Md==FBMode_Linear || Md==FBMode_Full) && (LclOptions & (FBDDOpt_WithQmSpec|FBDDOpt_WithDPSpec)))
        if (LclOptions & (FBDDOpt_WithQmSpec|FBDDOpt_WithDPSpec))
          {
          DDB.Text("");
          DDB.CheckBoxBtn("",                "AsMass",        DC_,      "",     xidFlwEqnQmAsMass,          pOwnObj, isParm|SetOnChange);
          }

        bool AsMass = FlwRqd().m_AsMass;
        if (LclOptions & FBDDOpt_WithQmSpec)
          {
          if (Md!=FBMode_Xfer || ForNDB)
            {
            DDB.Double ("",                "Qm.Rqd",        DC_Qm,   "kg/s",  xidFlwEqnQmReqd,        pOwnObj, (AsMass?isParm:0)|NAN_OK, DDBNAN_NotSpecd);
            DDB.Double ("",                "Qv.Rqd",        DC_Qv,   "L/s",   xidFlwEqnQvReqd,        pOwnObj, (!AsMass?isParm:0)|NAN_OK, DDBNAN_NotSpecd);
            }
          if (Md==FBMode_Xfer || ForNDB)
            {
            DDB.Double ("Qm.RqdCapacity",  "Qm.Capacity",  DC_Qm,   "kg/s",  xidFlwEqnQmRqdCapacity, pOwnObj, (AsMass?isParm:0)|NAN_OK, DDBNAN_NotSpecd);
            DDB.Double ("Qm.MinCapacity",  "Qm.MinCap",    DC_Qm,   "kg/s",  xidFlwEqnQmMinCapacity, pOwnObj, (AsMass?isParm:0));
            DDB.Double ("Qm.MaxCapacity",  "Qm.MaxCap",    DC_Qm,   "kg/s",  xidFlwEqnQmMaxCapacity, pOwnObj, (AsMass?isParm:0)|NAN_OK, DDBNAN_NotSpecd);
            DDB.Double ("Qv.RqdCapacity",  "Qv.Capacity",  DC_Qv,   "L/s",   xidFlwEqnQvRqdCapacity, pOwnObj, (!AsMass?isParm:0)|NAN_OK, DDBNAN_NotSpecd);
            DDB.Double ("Qv.MinCapacity",  "Qv.MinCap",    DC_Qv,   "L/s",   xidFlwEqnQvMinCapacity, pOwnObj, (!AsMass?isParm:0));
            DDB.Double ("Qv.MaxCapacity",  "Qv.MaxCap",    DC_Qv,   "L/s",   xidFlwEqnQvMaxCapacity, pOwnObj, (!AsMass?isParm:0)|NAN_OK, DDBNAN_NotSpecd);
            }
          DDB.Text("");
          }

        if (((Md==FBMode_Simple) || (Md==FBMode_Linear) || ForNDB) && (LclOptions & FBDDOpt_WithDPSpec))
          {
          if ((LclOptions & FBDDOpt_WithDPSpec))
            {
            //DDB.Text("");

            if (DDB.BeginStruct(pOwnObj, "Nominal"))
              {
              DDB.Double ("",      "DP",    DC_DP,      "kPa",   xidFlwEqnNomDP,       pOwnObj, isParm);
              DDB.Double ("",      "Qm",    DC_Qm,      "kg/s",  xidFlwEqnNomQm,       pOwnObj, (AsMass?isParm:0));
              DDB.Double ("",      "Qv",    DC_Qv,      "L/s",   xidFlwEqnNomQv,       pOwnObj, (!AsMass?isParm:0));
              }
            DDB.EndStruct();
            }

          DDB.Text("");
          }
        if ((Md==FBMode_Full || ForNDB) && (LclOptions & FBDDOpt_WithEqn) || DDB.ForFileSnpScn())
        //if (pOwnObj->SolveBufferedMethod())
          {
          //DDBValueLstMem DDB0;
          //TagObjClass::GetSDescValueLst(Group(), DDB0, NoEqnName);
          //DDB.String  ("Name",        "", DC_, "", xidFlwEqnNm,   pOwnObj, ((DDB0.Length()>1?isParm:0)|SetOnChange), DDB0());
          //DDB.Long    ("Evaluations", "", DC_, "", &Eqn()->m_nEvals,   pOwnObj, isResult|InitHidden);
          Eqn()->BuildDataDefn(DDB);
          }
        }
      }
    if (PrjFileVerNo()<22)
      DDB.EndStruct();
    else if ((LclOptions & FBDDOpt_HideEqnStruct)==0)
      DDB.EndObject();
    else
      {};
    }
  else
    {
    int xxx=0;
    }
  };

//--------------------------------------------------------------------------

flag CFlwEqnPtr::DataXchg(DataChangeBlk & DCB)
  {
  if (DCB.lHandle==xidFlwEqnGrp)
    {
// not changeable from file or access ...
//    if (DCB.rpC)
//      sGroup=DCB.rpC;
    DCB.pC=m_sGroup();
    return true;
    }

  if (!GroupOK())
    return false;

  switch (DCB.lHandle)
    {
    case xidFlwEqnNm:
      {
      if (DCB.rpC)
        {
        int IsNone=(_stricmp(DCB.rpC, NoEqnName)==0);
        if (PrjFileVerNo()<39 && !IsNone)
          IsNone=(_stricmp(DCB.rpC, "Null CFlwEqn")==0);

        if (IsNone)
          {
          ChangeFlwEqn(NoEqnName, !DCB.ForFiling());
          }
        else
          {
          Strng S;
          for (int i=0; ;i++)
            {
            pTagObjClass pC=TagObjClass::FindClassIndexed(GroupName(), i);
            if (pC==NULL)
              break;
            else
              {
              pC->GetShortDesc(S);
              if (S.XStrICmp(DCB.rpC)==0)
                {
                ChangeFlwEqn(pC->ClassId(), !DCB.ForFiling());
                break;
                }
              }
            }
          }
        }
      DCB.pC=FlwEqnName();
//      DCB.pC=Eqn() ? FlwEqnName() : NoEqnName;
      return true;
      }
    }

  switch (DCB.lHandle)
    {
    case xidFlwEqnDesc:
      if (DCB.rpC)
        Eqn()->SetEqpDesc(DCB.rpC);
      DCB.pC=Eqn()->EqpDesc();
      return true;
    case xidFlwEqnIdStr:
      if (DCB.rpC)
        Eqn()->SetEqpIdStr(DCB.rpC);
      DCB.pC=Eqn()->EqpIdStr();
      return true;
    case xidFlwEqnLocation:
      if (DCB.rpC)
        Eqn()->SetEqpLocation(DCB.rpC);
      DCB.pC=Eqn()->EqpLocation();
      return true;
    }

  if (PrjFileVerNo()>=36 && KFact().DataXchg(DCB, xidFlwEqnKFact, DCB.pDataInfo->pThis))
    return true;

  switch (DCB.lHandle)
    {
    case xidFlwEqnDiam:
      if (DCB.rD)
        SetDiam(*DCB.rD);
      DCB.D=Diam();
      return true;
    case xidFlwEqnArea:
      if (DCB.rD)
        SetArea(*DCB.rD);
      DCB.D=Area();
      return true;
    case xidFlwEqnRqdLen:
      if (DCB.rD)
        SetRqdLength(*DCB.rD);
      DCB.D=RqdLength();
      return true;
    case xidFlwEqnActLen:
      if (DCB.rD)
        SetActLength(*DCB.rD);
      DCB.D=ActLength();
      return true;
    case xidFlwEqnFitLen:
      if (DCB.rD)
        SetFitLength(*DCB.rD);
      DCB.D=FitLength();
      return true;
    case xidFlwEqnDZ:
      if (DCB.rD)
        SetRise(*DCB.rD);
      DCB.D=Rise();
      return true;
    case xidFlwEqnKFact:
      if (DCB.rD)
        KFact().SetVal(*DCB.rD, Eqn());
      DCB.D=KFact().Val();
      return true;
    case xidFlwEqnKFactOvr:
      if (DCB.rD)
        KFact().SetOvr(*DCB.rD, Eqn());
      DCB.D=KFact().Ovr();
      return true;
    case xidFlwEqnDatumFlng:
      if (DCB.rD)
        SetDatumFlng(*DCB.rD);
      DCB.D=DatumFlng();
      return true;
    case xidFlwEqnDatumNode:
      if (DCB.rD)
        SetDatumNode(*DCB.rD);
      DCB.D=DatumNode();
      return true;
    case xidFlwEqnQmRqdCapacity:
      if (DCB.rD)
        {
        Eqn()->m_FlwRqd.m_RqdQmCap=*DCB.rD;
        Eqn()->m_FlwRqd.m_RqdQvCap=Valid(*DCB.rD) ? *DCB.rD/Eqn()->m_FlwRqd.m_MeasDens : dNAN;
        }
      DCB.D=Eqn()->m_FlwRqd.m_RqdQmCap;
      return 1;
    case xidFlwEqnQmMinCapacity:
      if (DCB.rD)
        {
        Eqn()->m_FlwRqd.m_MinQmCap=*DCB.rD;
        Eqn()->m_FlwRqd.m_MinQvCap=Valid(*DCB.rD) ? *DCB.rD/Eqn()->m_FlwRqd.m_MeasDens : dNAN;
        }
      DCB.D=Eqn()->m_FlwRqd.m_MinQmCap;
      return 1;
    case xidFlwEqnQmMaxCapacity:
      if (DCB.rD)
        {
        Eqn()->m_FlwRqd.m_MaxQmCap=Valid(*DCB.rD) ? Max(Eqn()->m_FlwRqd.m_MinQmCap, *DCB.rD) : dNAN;
        Eqn()->m_FlwRqd.m_MaxQvCap=Valid(*DCB.rD) ? Max(Eqn()->m_FlwRqd.m_MinQmCap, *DCB.rD)/Eqn()->m_FlwRqd.m_MeasDens : dNAN;
        }
      DCB.D=Eqn()->m_FlwRqd.m_MaxQmCap;
      return 1;
    case xidFlwEqnQmAsMass:       
      if (DCB.rB)
        Eqn()->m_FlwRqd.m_AsMass=*DCB.rB!=0;
      DCB.B=Eqn()->m_FlwRqd.m_AsMass?1:0;
      return 1;
    case xidFlwEqnQmReqd:       
      if (DCB.rD)
        {
        CFlwEqnFlwRqd &FR=Eqn()->m_FlwRqd;
        FR.m_ReqdQm=*DCB.rD;
        FR.m_ReqdQv=Valid(*DCB.rD) ? *DCB.rD/FR.m_MeasDens : *DCB.rD;
        }
      DCB.D=Eqn()->m_FlwRqd.m_ReqdQm;
      return 1;
    case xidFlwEqnQvReqd:       
      if (DCB.rD)
        {
        CFlwEqnFlwRqd &FR=Eqn()->m_FlwRqd;
        FR.m_ReqdQv=*DCB.rD;
        FR.m_ReqdQm=*DCB.rD*FR.m_MeasDens;
        }
      DCB.D=Eqn()->m_FlwRqd.m_ReqdQv;
      return 1;
    case xidFlwEqnQvRqdCapacity:
      if (DCB.rD)
        {
        Eqn()->m_FlwRqd.m_RqdQvCap=*DCB.rD;
        if (OrigPrjFileVerNo()>=90)
          Eqn()->m_FlwRqd.m_RqdQmCap=*DCB.rD*Eqn()->m_FlwRqd.m_MeasDens;
        }
      DCB.D=Eqn()->m_FlwRqd.m_RqdQvCap;
      return 1;
    case xidFlwEqnQvMinCapacity:
      if (DCB.rD)
        {
        Eqn()->m_FlwRqd.m_MinQvCap=*DCB.rD;
        if (OrigPrjFileVerNo()>=90)
          Eqn()->m_FlwRqd.m_MinQmCap=*DCB.rD*Eqn()->m_FlwRqd.m_MeasDens;
        }
      DCB.D=Eqn()->m_FlwRqd.m_MinQvCap;
      return 1;
    case xidFlwEqnQvMaxCapacity:
      if (DCB.rD)
        {
        Eqn()->m_FlwRqd.m_MaxQvCap=Valid(*DCB.rD) ? Max(Eqn()->m_FlwRqd.m_MinQvCap, *DCB.rD) : dNAN;
        if (OrigPrjFileVerNo()>=90)
          Eqn()->m_FlwRqd.m_MaxQmCap=Valid(*DCB.rD) ? Max(Eqn()->m_FlwRqd.m_MinQvCap, *DCB.rD)*Eqn()->m_FlwRqd.m_MeasDens : dNAN;
        }
      DCB.D=Eqn()->m_FlwRqd.m_MaxQvCap;
      return 1;
    //case xidFlwEqnPressOn:
    //  {
    //  CFlwEqnFlwRqd & FR=FlwRqd();
    //  if (DCB.rB)
    //    { 
    //    FR.m_PressOn= (*DCB.rB!=0);
    //    if (DCB.ForView())
    //      {
    //      FlwNode * pNd=dynamic_cast<FlwNode*>(m_pAttachedTo);
    //      if (pNd)
    //        {
    //        CPropagateNetInfo Info;
    //        Info.m_Task=CPropagateNetInfo::eSetPressOn;
    //        Info.m_PressOn=FR.m_PressOn;
    //        pNd->PropagateNetInfo(Info, true);
    //        }
    //      }
    //    }
    //  DCB.B=(FR.m_PressOn!=0);
    //  return 1;
    //  }
    case xidFlwEqnNomDP  :
      if (DCB.rD)
        Eqn()->m_FlwRqd.m_PressDP=Range(0.001, *DCB.rD, 1000.0);
      DCB.D=Eqn()->m_FlwRqd.m_PressDP;
      return 1;
    case xidFlwEqnNomQm  :
      if (DCB.rD)
        {
        Eqn()->m_FlwRqd.m_PressQm=Range(0.001, *DCB.rD, 1000.0);
        Eqn()->m_FlwRqd.m_PressQv=Eqn()->m_FlwRqd.m_PressQm/Eqn()->m_FlwRqd.m_MeasDens;
        }
      DCB.D=Eqn()->m_FlwRqd.m_PressQm;
      return 1;
    case xidFlwEqnNomQv  :
      if (DCB.rD)
        {
        Eqn()->m_FlwRqd.m_PressQv=Range(0.001, *DCB.rD, 1000.0);
        Eqn()->m_FlwRqd.m_PressQm=Eqn()->m_FlwRqd.m_PressQv*Eqn()->m_FlwRqd.m_MeasDens;
        }
      DCB.D=Eqn()->m_FlwRqd.m_PressQv;
      return 1;

    }

  if (Eqn())
    return Eqn()->DataXchg(DCB);

  return false;
  };

//--------------------------------------------------------------------------

flag CFlwEqnPtr::ValidateData(ValidateDataBlk & VDB)
  {
  ASSERT_ALWAYS(m_pEqn!=NULL, "Eqn Expected");
  if (!GroupOK())
    return true;
  flag OK=true;
  //if (Eqn())
  //  {
  //  CFlwEqn *E=Eqn();
  //  //E->SetPhD(this);
    OK=Eqn()->ValidateData(VDB);
  //  //E->SetPhD(this);
  //  }
  return OK;
  }

double CFlwEqnPtr::FR_Resistance(double Density, double Regulation)
  { 
  CFlwEqnFlwRqd &FR=FlwRqd();
  //double CFlwEqnFlwRqd::Resistance(double Density)
//  {
//  //if (!PressOn())
//  //  return 1e-6;
//_asm int 3;
  if (FlowMode()!=FBMode_Xfer)
    {
    if (FR.m_AsMass)
      {                    
      FR.m_PressQv=FR.m_PressQm/FR.m_MeasDens;
      return FR.m_PressDP/GTZ(FR.m_PressQm*Regulation);
      }
    else
      {
      FR.m_PressQm=FR.m_PressQv*FR.m_MeasDens;
      return FR.m_PressDP/GTZ(FR.m_PressQv*Density*Regulation);
      }
    }
  return 1e-6;
  };


double CFlwEqnPtr::ShaftPower()
  {
  POSITION Pos=m_Children.GetHeadPosition();
  if (Pos) // implies Children
    {
    double Tot=0;
    while (Pos)
      Tot+=m_Children.GetNext(Pos)->ShaftPower();
    return Tot;
    }
  return Eqn()->ShaftPower();
  };

//==========================================================================
//
//
//
//==========================================================================

byte CFlwBlkBase::fMomentumOn=0;
double CFlwBlkBase::dHasFlowLimit=1.0e-6;

CFlwBlkBase::CFlwBlkBase() : CFlwEqnPtr(this, 0, 0.0)
  {
  m_iFwdFlwProps=TheFwdProps;
  m_iRevFlwProps=TheRevProps;
  CFlwBlkBase::ResetData(true);
  };

//--------------------------------------------------------------------------

CFlwBlkBase::~CFlwBlkBase()
  {
  };

//--------------------------------------------------------------------------

void CFlwBlkBase::BuildDataDefn(long DDOptions, DataDefnBlk & DDB, pTaggedObject pOwnObj, char * FBTag, DWORD UserInfo, DDBPages PgOpt)
  {
  if (PrjFileVerNo()<22 && Eqn())
    {
    DDB.String ("Desc",     "",       DC_,      "",       xidFlwEqnDesc,    pOwnObj, isParm);
    DDB.String ("IdStr",    "",       DC_,      "",       xidFlwEqnIdStr,   pOwnObj, isParm|InitHidden);
    DDB.String ("Location", "",       DC_,      "",       xidFlwEqnLocation,pOwnObj, isParm|InitHidden);
    DDB.Double ("RqdLen",   "",       DC_L,     "m",      xidFlwEqnRqdLen,  pOwnObj, isParmStopped);
    DDB.Double ("ActLen",   "",       DC_L,     "m",      xidFlwEqnActLen,  pOwnObj, isParmStopped);
    DDB.Double ("Diam",     "",       DC_L,     "mm",     xidFlwEqnDiam,    pOwnObj, isParm);
    DDB.Double ("",         "DZ",     DC_L,     "mm",     xidFlwEqnDZ,      pOwnObj, isParm);
    }

  long LclOptions = Options()|DDOptions;
  if (LclOptions & FBDDOpt_WithEqn)
    {
    CFlwEqnPtr::BuildDataDefn(LclOptions, DDB, pOwnObj, FBTag, UserInfo, PgOpt);
    PgOpt=DDB_NoPage;
    }

  if ((LclOptions & FBDDOpt_WithData) && (!DDB.ForView() || dbgfile()))
    {
    Strng Tg(PrjFileVerNo()<35?"Data":"FB.");
    if (FBTag)
      Tg+=FBTag;
    flag DoIt;
    if (PrjFileVerNo()<22)
      DoIt=DDB.BeginStruct(pOwnObj, Tg(), NULL, PgOpt, -1, MarkerClosed);
    else
      DoIt=DDB.BeginObject(pOwnObj, Tg(), "FB_Data", NULL, PgOpt, -1, MarkerClosed);
    if (DoIt)
      {
      if (PrjFileVerNo()==22 && Eqn())
        {
        DDB.String ("Desc",       "",         DC_,      "",       xidFlwEqnDesc,    pOwnObj, isParm);
        DDB.String ("IdStr",      "",         DC_,      "",       xidFlwEqnIdStr,   pOwnObj, isParm|InitHidden);
        DDB.String ("Location",   "",         DC_,      "",       xidFlwEqnLocation,pOwnObj, isParm|InitHidden);
        DDB.Double ("RqdLen",     "",         DC_L,     "m",      xidFlwEqnRqdLen,  pOwnObj, isParmStopped);
        DDB.Double ("ActLen",     "",         DC_L,     "m",      xidFlwEqnActLen,  pOwnObj, isParmStopped);
        DDB.Double ("Diam",       "",         DC_L,     "mm",     xidFlwEqnDiam,    pOwnObj, isParm);
        DDB.Double ("",           "DZ",       DC_L,     "mm",     xidFlwEqnDZ,      pOwnObj, isParm);
        }
      DDB.Double ("",             "Qm",       DC_Qm,    "kg/s",   &m_Qm,              pOwnObj, isResult|InitHidden);
      DDB.Double ("",             "QmEst",    DC_Qm,    "kg/s",   &m_QmEst,           pOwnObj, isResult|InitHidden);
      DDB.Double ("",             "dPq",      DC_DP,    "kPa",    &m_dPq,             pOwnObj, isResult);
      DDB.Double ("",             "dPa",      DC_DP,    "kPa",    &m_dPa,             pOwnObj, isResult);
      DDB.Double ("",             "dPz",      DC_DP,    "kPa",    &m_dPz,             pOwnObj, isResult);
      DDB.Double ("",             "dPzX",     DC_DP,    "kPa",    &m_dPzX,            pOwnObj, isResult);
      DDB.Double ("",             "dPb",      DC_DP,    "kPa",    &m_dPb,             pOwnObj, isResult);

      DDB.Double ("",             "Vel",      DC_Ldt,   "m/s",    &m_Vel,             pOwnObj, isResult);
      DDB.Double ("",             "VelSonic", DC_Ldt,   "m/s",    &m_VelSonic,        pOwnObj, isResult);

      DDB.Double ("SlipRatio",    "",       DC_,      "",       &dSlip,           pOwnObj, isResult|NAN_OK|InitHidden);
      DDB.Double ("VoidFraction", "",       DC_Frac,  "%",      &dVoidFrac,       pOwnObj, isResult|NAN_OK|InitHidden);
      DDB.Double ("OnePhPart",    "",       DC_Frac,  "%",      &dOnePhPart,      pOwnObj, isResult|NAN_OK|InitHidden);
      DDB.Double ("TempOut",      "",       DC_T,     "C",      &dTempOut,        pOwnObj, isResult|NAN_OK|InitHidden);
      }

    if (PrjFileVerNo()<22)
      DDB.EndStruct();
    else
      DDB.EndObject();
    }

  };

//--------------------------------------------------------------------------

flag CFlwBlkBase::DataXchg(DataChangeBlk & DCB)
  {
  return CFlwEqnPtr::DataXchg(DCB);
  };

//--------------------------------------------------------------------------

void CFlwBlkBase::ResetData(flag Complete)
  {
//  m_pNP=NULL;
  m_TopoChgRqd=true;
  m_FlwProps[TheFwdProps].m_bValid=false;
  m_FlwProps[TheRevProps].m_bValid=false;
  m_iFwdFlwProps=TheFwdProps;
  m_iRevFlwProps=TheRevProps;

  CFlwBlkData::ResetData(Complete);
  CFlwEqnPtr::ResetData(Complete);
  }

//--------------------------------------------------------------------------

double CFlwBlkBase::RangeFlow(double Flow, double LoLimit)
  {
  ASSERT(TstGTZ(LoLimit));
  Flow=fabs(Flow);
  if (Flow>LoLimit)
    return Flow;
  /**
  This fuction will -
  1 - keep the flow used to measure dPq above 0.25 * LoLimit
  2 - the resultant curve if smooth
  3 - the curve is not flat @ 0.0
  */
  return Flow+Sqr(LoLimit-Flow)/(3.0*LoLimit);
  };

//--------------------------------------------------------------------------

double CFlwBlkBase::SetQmMeasRange(double Rho, double LoQm)
  {
  ASSERT(m_MeasLoLimit==0.0);
  m_MeasType    = 1;
  m_MeasRho     = Rho;
  m_MeasArea    = 1.0;
  m_MeasLoLimit = LoQm; 
  m_QmMeas      = Max(m_QmMeas, 1.0e-6*LoQm);
  m_QmMeas      = RangeFlow(m_QmMeas, LoQm);
  return m_QmMeas;
  };

//--------------------------------------------------------------------------

double CFlwBlkBase::SetQvMeasRange(double Rho, double LoVol)
  {
  ASSERT(m_MeasLoLimit==0.0);
  m_MeasType    = 2;
  m_MeasRho     = Rho;
  m_MeasArea    = 1.0;
  m_MeasLoLimit = LoVol; 
  m_QmMeas      = Max(m_QmMeas, 1.0e-6*LoVol*MeasRhoA());
  m_QmMeas      = RangeFlow(m_QmMeas/MeasRhoA(), LoVol)*MeasRhoA();
  return m_QmMeas;
  };

//--------------------------------------------------------------------------

double CFlwBlkBase::SetVelMeasRange(double Rho, double Area, double LoVel)
  {
  ASSERT(m_MeasLoLimit==0.0);
  m_MeasType    = 3;
  m_MeasRho     = Rho;
  m_MeasArea    = Area;
  m_MeasLoLimit = LoVel; 
  m_QmMeas      = Max(m_QmMeas, 1.0e-6*LoVel*MeasRhoA());
  m_QmMeas      = RangeFlow(m_QmMeas/MeasRhoA(), LoVel)*MeasRhoA();
  return m_QmMeas;
  };

//--------------------------------------------------------------------------

double CFlwBlkBase::QmMeas(double Scl)  
  { 
  if (m_MeasType==0)
    return m_QmMeas;

  ASSERT_ALWAYS(m_MeasType>0, "CFlwBlkBase::QmMeasBad");
  return RangeFlow(m_QmMeas*Scl, m_MeasLoLimit); 
  };
double CFlwBlkBase::QvMeas(double Scl) 
  { 
  ASSERT_ALWAYS(m_MeasType==2, "CFlwBlkBase::VolMeasBad");
  return RangeFlow(m_QmMeas*Scl, m_MeasLoLimit)/MeasRhoA(); 
  };
double CFlwBlkBase::VelMeas(double Scl) 
  { 
  ASSERT_ALWAYS(m_MeasType==3, "CFlwBlkBase::VelMeasBad");
  return RangeFlow(m_QmMeas*Scl, m_MeasLoLimit)/MeasRhoA();
  };
double CFlwBlkBase::DQmMeas(double Scl) 
  { 
  return NZ(QmMeas(Scl)-QmMeas(1.0)); 
  };

//--------------------------------------------------------------------------

//void CFlwBlkBase::SetFill(double F)
//  {
////  FillC.dFill=F;
//  dFill=F;
//#if WithOEP
//  if (OEPOptions())
//    dFill=0.0;
////    FillC.dFill=0.0;
//#endif
//  };

//--------------------------------------------------------------------------
// Called at beginning of solution
void CFlwBlkBase::InitFlowInfo(SpConduit * pCd)
  {
  SetCd(pCd);
  Eqn()->InitFlowInfo(*this);
  }

//--------------------------------------------------------------------------
// Called at beginning of each step
void CFlwBlkBase::StepFlowInfo()
  {
  m_QmEstGood=m_QmEst;

  m_MeasDone=false;

  m_dPb=0.0;
  m_dPbX=0.0;
  m_dPbdQ=0.0;
  m_dPa=0.0;
  m_dPadQ=0.0;
  m_dPq=0.0;
  m_dPqdQ=0.0;
  m_dPz=0.0;
  m_dPzI=0.0;
  m_dPzdQ=0.0;
  m_dPzX=0.0;
  m_dPzXdQ=0.0;
  m_dPmB=0.0;
  m_dPmQ=0.0;
  m_dPmdQ=0.0;

  int TauOK=true;
  double Tau,A;
  if (Valid(Eqn()->m_dOverideTauRqd))
    Tau=Eqn()->m_dOverideTauRqd;
  else if (Valid(m_dOverideTau))
    Tau=m_dOverideTau;
  else
    TauOK=false;

  if (TauOK)
    {
    Tau=Max(ICGetTimeInc(), Tau);
    A=1.0-exp(-Range(0.0, ICGetTimeInc()/Max(1.0e-3, Tau), 87.0));
    FlngPB().PMn=A*FlngPB().P + (1.0-A)*FlngPB().PMn;
    NodePB().PMn=A*NodePB().P + (1.0-A)*NodePB().PMn;
    }
  else
    {
    FlngPB().PMn=FlngPB().P;
    NodePB().PMn=NodePB().P;
    }

  m_dOverideRhoL=Eqn()->m_dOverideRhoLRqd;
  m_dOverideRhoV=Eqn()->m_dOverideRhoVRqd;
  m_dOverideTemp=Eqn()->m_dOverideTempRqd;
  m_dOverideVisc=Eqn()->m_dOverideViscRqd;


//  if (Valid(Eqn()->OverideTempRqd()) && Eqn()->OverideTempRqd()>1.0)
//    dOverideTemp=Eqn()->OverideTempRqd();
//  else if (TauOK)
//    {
//    double MeasTemp=MeanTemp(false);
//    if (MeasTemp>1.0)
//      {
//      if (Valid(dOverideTemp))
//        dOverideTemp=A*MeasTemp + (1.0-A)*dOverideTemp;
//      else
//        dOverideTemp=MeasTemp;
//      }
//    }
//  else
//    dOverideTemp=dNAN;
//
//  if (Valid(Eqn()->OverideRhoRqd()) && Eqn()->OverideRhoRqd()>1.0e-3)
//    dOverideRho=Eqn()->OverideRhoRqd();
//  else if (TauOK)
//    {
//    double MeasRho=MeanRho(MeanPress(), false);
//    if (MeasRho>1.0e-3)
//      {
//      if (Valid(dOverideRho))
//        dOverideRho=A*MeasRho + (1.0-A)*dOverideRho;
//      else
//        dOverideRho=MeasRho;
//      }
//    }
//  else
//    dOverideRho=dNAN;
//
//  if (Valid(Eqn()->OverideViscRqd()) && Eqn()->OverideViscRqd()>1.0e-9)
//    dOverideVisc=Eqn()->OverideViscRqd();
//  else if (TauOK)
//    {
//    double MeasVisc=MeanViscosity(false);
//    if (MeasVisc>1.0)
//      {
//      if (Valid(dOverideVisc))
//        dOverideVisc=A*MeasVisc + (1.0-A)*dOverideVisc;
//      else
//        dOverideVisc=MeasVisc;
//      }
//    }
//  else
//    dOverideVisc=dNAN;

  //if (EqnOK())
    Eqn()->StepFlowInfo(*this);
    Eqn()->m_nEvals=0;
  };

//--------------------------------------------------------------------------
// Called at start of each deriv eval
flag CFlwBlkBase::EvalFlowInfoStart()
  {
  if (m_TopoChgRqd)
    {
    m_TopoChgRqd=false;
    return true;
    }
  return false;;
//#endif
  };

//--------------------------------------------------------------------------
// Called at end of each deriv eval
void CFlwBlkBase::EvalFlowInfoEnd()
  {
  byte OldFPA=m_iFlwPh_Rqd;

  //flag oldVFlowAllowed=xVFlowAllowed();
  //flag oldLFlowAllowed=xLFlowAllowed();

  double PhysVol=ActLength()*Area();
  double FlowVol=fabs(Cd().QVolume()*ICGetTimeInc());

#pragma chNOTE(Adjust to Freeze the density in FBs)
  flag VolAveraging = 0;
  flag FreezeRho = 0;
  double VolScl=FreezeRho  ? 0.0 : (VolAveraging ? FlowVol/GTZ(FlowVol+PhysVol) : 1.0);
  double InvVolScl=(1.0-VolScl);

  if (m_Qm>0)
    {
    m_FlwProps[TheFwdProps].SetPropInfoX(Cd());
    m_FlwProps[TheFwdProps].m_bValid=true;
    }
  else if (m_Qm<0)
    {
    m_FlwProps[TheRevProps].SetPropInfoX(Cd());
    m_FlwProps[TheRevProps].m_bValid=true;
    }
  if (m_FlwProps[TheFwdProps].m_bValid && m_FlwProps[TheRevProps].m_bValid)
    {
    m_iFwdFlwProps=TheFwdProps;
    m_iRevFlwProps=TheRevProps;
    }
  else if (m_FlwProps[TheFwdProps].m_bValid && !m_FlwProps[TheRevProps].m_bValid)
    {
    m_iFwdFlwProps=TheFwdProps;
    m_iRevFlwProps=TheFwdProps;
    }
  else if (!m_FlwProps[TheFwdProps].m_bValid && m_FlwProps[TheRevProps].m_bValid)
    {
    m_iFwdFlwProps=TheRevProps;
    m_iRevFlwProps=TheRevProps;
    }

  m_TopoChgRqd=(OldFPA!=m_iFlwPh_Rqd);
  //m_TopoChgRqd=(oldVFlowAllowed!=xVFlowAllowed() || oldLFlowAllowed!=xLFlowAllowed());
  /*
  flag oldVFlowAllowed=VFlowAllowed();
  flag oldLFlowAllowed=LFlowAllowed();
//
//  double MV=Cd().QMass(som_Gas);
//  double ML=Cd().QMass(som_SL);
//  double VV=Cd().QVolume(som_Gas);
//  double VL=Cd().QVolume(som_SL);
//
//  if ((MV+ML)>1.0e-6)
//    {
//    double AppFill=Range(0.0, ML/GTZ(fabs(MV+ML)), 1.0);
//    double AppVF=1.0-AppFill;
//
//    static double Filt=0.0;
////    SetFill((1.0-Filt)*AppFill+Filt*Fill());
//    }
//
//#if WithOEP
//  if (OEPOptions())
//    SetArea(Max(Area(), 1.0));
//#endif
//
  double PhysVol=ActLength()*Area();
  double FlowVol=fabs(Cd().QVolume()*ICGetTimeInc());

#pragma chNOTE(Adjust to Freeze the density in FBs)
  flag VolAveraging = 0;
  flag FreezeRho = 0;
  double VolScl=FreezeRho  ? 0.0 : (VolAveraging ? FlowVol/GTZ(FlowVol+PhysVol) : 1.0);
  double InvVolScl=(1.0-VolScl);

  m_FlwProps.SetPropInfo(Cd(), VolScl);


// Vap Frac is Volume based ??
//  SetVFrac((1.0-Filt)*AppVF+Filt*VFrac());//Cd().MassFrac(som_Gas));

//  double QmLRatio=1.0+0.8*(Range(0.0, ML/GTZ(fabs(QmEst)), 2.0)-1.0);

//  if (ML>1.0e-6)
//    {
//    double RL=Cd().Rho(som_SL);
//    if (RL>1.0e-3)
//      SetRhoL(VolScl*Cd().Rho(som_SL)+InvVolScl*FillC.dRhoL);
//    }
//  if (MV>1.0e-6)
//    {
//    double RV=Cd().Rho(som_Gas);
//    if (RV>1.0e-3)
//      SetRhoV(VolScl*RV+InvVolScl*FillC.dRhoV, Cd().Temp(), Cd().Press());
//    }
//  double T=Cd().Temp();
//  if (T>1.0)
//    SetTemp(VolScl*T+InvVolScl*FillC.dTemp);
//
//  double V=Cd().Viscosity();
//  if (V>1e-9)
//    SetVisc(VolScl*V+InvVolScl*FillC.dVisc);
//
//  SetVLFracs(Cd().MassFrac(som_Gas), Cd().MassFrac(som_Liq));
//
//  if (FillC.dTemp<1.0 || FillC.dRhoL<1.e-3 || FillC.dRhoV<1.e-3)
//    DoBreak();

  return oldVFlowAllowed!=VFlowAllowed() || oldLFlowAllowed!=LFlowAllowed();
//#endif
*/




  };

//--------------------------------------------------------------------------

flag CFlwBlkBase::TestFlowInfo()
  {
  flag OK;
  OK=Eqn()->TestFlowInfo(*this);
  return OK;
  };

//--------------------------------------------------------------------------

CSpPropInfo *  CFlwBlkBase::Properties(CSpPropInfo * pProps)
  {
  switch (m_PropSel.m_iInUse)
    {
    case FBPS_Default:
      if (pProps)
        return pProps;
      if (m_Qm>0.0)
        return &m_FlwProps[m_iFwdFlwProps];
      return &m_FlwProps[m_iRevFlwProps];

    case FBPS_FwdOnly:
      return &m_FlwProps[m_iFwdFlwProps];

    case FBPS_RevOnly:
      return &m_FlwProps[m_iRevFlwProps];
    }

  return NULL; //pProps?pProps:&m_FlwProps;
  };

//--------------------------------------------------------------------------

double CFlwBlkBase::MeanTemp(CSpPropInfo *pProps, flag AllowOveride)
  {
  CSpPropInfo *p=Properties(pProps);//pProps?pProps:&m_FlwProps;
  return p->m_dTemp;

  if (AllowOveride && Valid(m_dOverideTemp))
    return m_dOverideTemp;
  //return dTemp;
  return p->m_dTemp;
  };

//--------------------------------------------------------------------------

double CFlwBlkBase::MeanRho(CSpPropInfo *pProps, double P, flag AllowOveride)
  {

  CSpPropInfo *p=Properties(pProps);//pProps?pProps:&m_FlwProps;
  if (1)
    {
    double Rv=AllowOveride && Valid(m_dOverideRhoV) ? m_dOverideRhoV : (P/p->m_dPress)/**(Norm_T/GTZ(p->m_dTemp))*/*p->m_dRhoV;
    double Rl=AllowOveride && Valid(m_dOverideRhoL) ? m_dOverideRhoL : p->m_dRhoL;
    double Ml=Rl*(p->NettLVolFrac());
    double Mv=Rv*(p->NettVVolFrac());
    return Min_FE_Rho((Mv+Ml)/GTZ(Ml/Rl+Mv/Rv));
    }

  };

//--------------------------------------------------------------------------

double CFlwBlkBase::MeanRhoL(CSpPropInfo *pProps, flag AllowOveride)
  {
  if (AllowOveride && Valid(m_dOverideRhoL))
    return m_dOverideRhoL;

  CSpPropInfo *p=Properties(pProps);//pProps?pProps:&m_FlwProps;
  double Rl=p->m_dRhoL;
  return Min_FE_Rho(Rl);
  }

//--------------------------------------------------------------------------

double CFlwBlkBase::MeanRhoV(CSpPropInfo *pProps, double P, flag AllowOveride)
  {
  if (AllowOveride && Valid(m_dOverideRhoV))
    return m_dOverideRhoV;

  CSpPropInfo *p=Properties(pProps);//pProps?pProps:&m_FlwProps;
  if (1)
    {
    P=Max(1.0,P);
    double Rv=(P/p->m_dPress)/**(Norm_T/GTZ(p->m_dTemp))*/*p->m_dRhoV;
    return Min_FE_Rho(Rv);
    }
  };

//--------------------------------------------------------------------------

double CFlwBlkBase::MeanViscosity(CSpPropInfo *pProps, flag AllowOveride)
  {
  if (AllowOveride && Valid(m_dOverideVisc))
    return m_dOverideVisc;

  CSpPropInfo *p=Properties(pProps);//pProps?pProps:&m_FlwProps;

  return p->m_dVisc;
  };

double CFlwBlkBase::NettVVolFrac(CSpPropInfo *pProps)
  {
  return Properties(pProps)->NettVVolFrac();
  }

double CFlwBlkBase::NettLVolFrac(CSpPropInfo *pProps)
  {
  return Properties(pProps)->NettLVolFrac();
  }

void CFlwBlkBase::SetVLFracs(CSpPropInfo * pProps, double V, double L)
  {
  pProps=Properties(pProps);
  pProps->m_dLVolFrac=L;
  pProps->m_dVVolFrac=V;
  //pProps->dVVolFracBal=0.0;
  // this isneeded for MFBs and Two Ph Flow
  //FillC.dVFrac=V; FillC.dLFrac=L;
  };

//--------------------------------------------------------------------------

double CFlwBlkBase::StaticHead(CSpPropInfo *pProps)
  {
  switch (FlowMode())
    {
    case LFM_Full:
    case LFM_Linear:
      {
      CSpPropInfo *p=Properties(pProps);
    //  double X=/*Fill();//*/(1-pProps.dVVolFrac);//.??Fill();//(1.0-dFill);
      const double X=(p->NettLVolFrac());
      // This Equation is really only for Air
      const double dRise=Rise();
      const double dMeanT=MeanTemp(pProps);
      double RhoHV=Sign(dRise)*(1.0-X)*MeanPress()*(exp(9.81*fabs(dRise)/(287.0*dMeanT))-1.0);
      double RhoHL=m_fApplyStaticHead ? dRise*X*9.81*p->m_dRhoL/1000.0 : 0.0;
      return -(RhoHV+RhoHL);
      }
    default:
      return 0.0;
    }
  }

//--------------------------------------------------------------------------
/*
double CFlwBlkBase::StaticHead(flag WithLiquid, double Fill, double RhoLFill, double NRhoVFill)
  {
  // This Equation is really only for Air
  double dRise=Rise();
  double RhoHV=Sign(dRise)*(1.0-Fill)*MeanFBPress()*(exp(9.81*fabs(dRise)/(287.0*FillC.dTemp))-1.0);
RhoHV=0.0;
  double RhoHL=WithLiquid ? dRise*Fill*9.81*FillC.dRhoL/1000.0 : 0.0;

  if (pAttachedTo && pAttachedTo->fDoDbgBrk)
    {int xxx=0;}
//  dbgpln("RhoH   :[%2i] %10.3f Fill:%8.6f s:%8.6f d:%8.6f %s",FD, -(RhoHV+RhoHL), dFill, dFillStatic, dFillDynamic, pAttachedTo ? pAttachedTo->Tag() : "???");
  return -(RhoHV+RhoHL);
  }

//--------------------------------------------------------------------------

double CFlwBlkBase::GetFill_StaticHead(double RhoH)
  {
  double A=MeanFBPress()*(exp(9.81*fabs(Rise())/(287.0*FillC.dTemp))-1.0);
A=0.0;
  double B=fabs(Rise())*9.81*FillC.dRhoL/1000.0;
  double X=(RhoH-A)/NZ(B-A);
  double RqdFill=X;///GTZ(1-??VapFrac(FD));//(1.0-Fill());
  return Range(0.0, RqdFill, 1.0);//(1.0-Fill());
  }
*/

//--------------------------------------------------------------------------

void CFlwBlkBase::EvalGetModeRqd(long FlowMode, CSpPropInfo *pProps, bool On, bool IsDegFreedom, double Regulation)
  {
  CFlwEqnFlwRqd &FR=FlwRqd();
  //_asm int 3;
  //;// .EvaluateFlwEqn(Task, pProps, cioOn()?1.0:0.0, NULL, NULL);//Fill);
      //if (IONo==0 && FE==0)
      //  {
  bool DoXfer=(FlowMode==FBMode_Xfer);

  FR.m_MeasDens=(float)Max(0.001, MeanRho(pProps));

  if (InXferNet() || DoXfer)
    {
    if (DoXfer)
      SetXferMode();
    else
      SetQmFree();
    if (On)
      {
      if (FR.m_AsMass)
        {
        double Cap = FR.m_RqdQmCap;
        if (Valid(Cap))
          {
          Cap=Max(Cap, FR.m_MinQmCap);
          if (Valid(FR.m_MaxQmCap))
            Cap=Min(Cap, FR.m_MaxQmCap*Regulation);
          }
        else if (Valid(FR.m_MaxQmCap))
          Cap=FR.m_MaxQmCap*Regulation;
        else if (Regulation<1.0)
          Cap=MaxXfCap*Regulation;
        if (Valid(Cap))
          SetXferCapacity(Cap);
        }
      else
        {
        double Cap = FR.m_RqdQvCap;
        if (Valid(Cap))
          {
          Cap=Max(Cap, FR.m_MinQvCap);
          if (Valid(FR.m_MaxQvCap))
            Cap=Min(Cap, FR.m_MaxQvCap*Regulation);
          }
        else if (Valid(FR.m_MaxQvCap))
          Cap=FR.m_MaxQvCap*Regulation;
        if (Valid(Cap))
          SetXferCapacity(Cap*MeanRho(pProps));
        }

      //CFlwEqnFlwRqd &FR=FlwRqd();
      //double Cap = Valid(FR.xMax_QmCap) ? xMax_QmCap : MaxXfCap;
      //if (Valid(xRqd_QmCap))
      //  Cap = Range(xMin_QmCap, xRqd_QmCap, Cap);
      //Cap *= (m_iLF_Direction==LFD_Reverse ? -1.0 : 1.0);
      //SetXferCapacity(Cap);
      }
    else
      {
      if (DoXfer)
        SetXferCapacity(0.0);
      else
        SetQmReqd(0.0);
      }
    //return True;
    }
  else if (!On)
    SetQmReqd(0.0);
  else if (Valid(FR.m_ReqdQm))
    SetQmReqd(FR.m_ReqdQm, IsDegFreedom);
  else
    SetQmFree();
  }
  //    else
  //      {
  //      if (cioOn())
  //        FB.SetQmFree();
  //      else
  //        FB.SetQmReqd(0.0);
  //      }
  //}

//--------------------------------------------------------------------------

flag CFlwBlkBase::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, bool On, bool IsDegFreedom, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  double Q=NZ(m_Qm);

  m_sQMeas      = FlwSign(Q);
  m_QmMeas      = fabs(Q);
  m_MeasLoLimit = 0.0;
  m_MeasRho     = 1.0;
  m_MeasArea    = 1.0;
  if (Eqn()->DoDbgBrk())
    { int xxx=0; };

  flag b=false;
  CFlwEqnFlwRqd &FR=FlwRqd();

  FR.m_MeasDens=(float)Max(0.001, MeanRho(pProps));

  if (1)
    {
    TaggedObject *p=m_pAttachedTo;
    FlwNode *pNd=dynamic_cast<FlwNode*>(p);
    while (pNd==NULL && p)
      {
      p=p->pAttachedTo;
      pNd=dynamic_cast<FlwNode*>(p);
      }
    #if (dbgFlwEqns)
    if (dbgFlwEqnModes())
      dbgpln("Eval %s %s %s", FlwEqnTaskStr(Task), FBModeStr(FlowMode()), pNd?pNd->FullObjTag():"");
    #endif
    }

  if (!On)
    Regulation=0;

  if (Task==FET_GetMode)
    {
    EvalGetModeRqd(FlowMode(), pProps, On, IsDegFreedom, Regulation);
    ClrFunctOfPress();
    b=true;
    }

  //if (0 && (FlowMode()==FBMode_Xfer))
  //  {
  //  SetXferMode();

  //  if (Valid(FR.m_RqdQmCap) || Valid(FR.m_RqdQvCap))
  //    { int xxx=0; }

  //  if (FR.m_AsMass)
  //    {
  //    double Cap = FR.m_RqdQmCap;
  //    if (Valid(Cap))
  //      {
  //      Cap=Max(Cap, FR.m_MinQmCap);
  //      if (Valid(FR.m_MaxQmCap))
  //        Cap=Min(Cap, FR.m_MaxQmCap*Regulation);
  //      }
  //    else if (Valid(FR.m_MaxQmCap))
  //      Cap=FR.m_MaxQmCap*Regulation;
  //    if (Valid(Cap))
  //      SetXferCapacity(Cap);
  //    }
  //  else
  //    {
  //    double Cap = FR.m_RqdQvCap;
  //    if (Valid(Cap))
  //      {
  //      Cap=Max(Cap, FR.m_MinQvCap);
  //      if (Valid(FR.m_MaxQvCap))
  //        Cap=Min(Cap, FR.m_MaxQvCap*Regulation);
  //      }
  //    else if (Valid(FR.m_MaxQvCap))
  //      Cap=FR.m_MaxQvCap*Regulation;
  //    if (Valid(Cap))
  //      SetXferCapacity(Cap*MeanRho(pProps));
  //    }
  //  b=true;

  else if (Disabled())
    {
    if (ICStepStart())
      {
      return true;
      }
    }
  else
    {
    ClrDerivsBad();
    double QmMem=m_Qm;
    //if (Task==FET_GetMode)
    //  ClrFunctOfPress();
    m_fMomentumValid=false;

    #if (dbgFlwEqns)
    if (dbgFlwEqnData())
      dbgpln("FE: %-20s %-10s %10.4f %10.4f %10.4f",
      Eqn()->pAttachedTo ? Eqn()->pAttachedTo->Tag():"NA",
         Eqn()->ClassId(),
         Eqn()->Diam(), Eqn()->TotLength(), Eqn()->Rise());
    #endif

    switch (FlowMode())
      {
      case FBMode_Full:
        {
        b=Eqn()->EvaluateFlwEqn(Task, pProps, *this, Regulation, pPhD0, pPhD1);
        Eqn()->m_nEvals++;
        break;
        }
      case FBMode_Xfer:
        break;
      case FBMode_Linear:
      case FBMode_Simple:
        {
        if (Options() & FBOpt_HasSimpleRes)
          m_SimpleRes=Range(1e-6, FR_Resistance(MeanRho(pProps), Regulation)/GTZ(Regulation), 1e6);
        else
          m_SimpleRes=0;//1e-6;

        if (Regulation<1.0e-6)
          {
          SetQmReqd(0);
          b=true;
          }

        if (!b && Task==FET_CalcDP)
          {
          //b=Eqn()->EvaluateFlwEqn(Task, pProps, *this, Regulation, pPhD0, pPhD1);
          //Eqn()->m_nEvals++;
          SetDPb(0);
          //SetDPbX(0);
          SetDPz(StaticHead(pProps));
          b=true;
          }
        break;
        }
      }
    m_Qm=QmMem;
    // Overide QmRqd Modes etc if closed
    if (m_bClosed)
      {
      SetQmReqd(0.0);
      b=true;
      };

    return b;
    }

  return false;
  };

//--------------------------------------------------------------------------

flag CFlwBlkBase::SolveFlwEqn(CSpPropInfo *pProps, double Regulation, double P1_, double P2_, double & EstFlw)
  {
  ClrDerivsBad();
  ClrFunctOfPress();
  ClrEvalFwdOnly();
  ClrEvalRevOnly();
  _asm int 3; // next line must be be restored
  //CFlwBlkData Mem=*this;

  m_Qm=NZ(EstFlw);
  ASSERT(fabs(m_Qm)>1.0e-200);
  m_fMomentumValid=false;
  double m_P_Applied = P1_-P2_;
  flag b=false;
  const double MaxdQMult         = 4.0;
  const double SignificanceRatio = 1.0e-12; // 12 digits
  const int    MaxLoop           = 50;
  for (int Loop=0; Loop<MaxLoop; Loop++)
    {
    m_sQMeas=FlwSign(m_Qm);
    m_QmMeas=fabs(m_Qm);
    m_MeasLoLimit=0.0;
    m_MeasType=0;
    m_MeasRho=1.0;
    m_MeasArea=1.0;
    b=Eqn()->EvaluateFlwEqn(FET_GetMode, pProps, *this, Regulation, NULL, NULL);
    b=Eqn()->EvaluateFlwEqn(FET_CalcDP, pProps, *this, Regulation, NULL, NULL);
    Eqn()->m_nEvals++;
    double R1=fabs(m_dPq)/m_QmMeas;
    double R2=fabs(m_dPmQ)/GTZ(fabs(m_Qm));
    // Ensure that some resistance exists
    R1=Max(1.0e-20, R1);
    m_ResT=R1+R2;
    // Reconstruct dPq & dPmQ
    m_dPq=-R1*m_Qm;
    m_dPmQ=-R2*m_Qm;


    double Sigma_dP   = -(m_dPq+m_dPmQ) - (m_dPa+m_dPb+m_dPbX+m_dPz+m_dPzI+m_dPzX+m_dPmB);
    double Sigma_dPdQ = fabs(m_dPqdQ) + fabs(m_dPmdQ) - (m_dPadQ+m_dPbdQ + m_dPzdQ + m_dPzXdQ + m_dPmdQ);
    double dQUnLim=(fabs(Sigma_dPdQ) > SmallPosFlow ? - (Sigma_dP - m_P_Applied) / Sigma_dPdQ : 0.0);

    double dQMax=Max(SmallPosFlow*0.1, m_QmMeas * MaxdQMult);
    double dQLim=Sign(dQUnLim)*Min(fabs(dQUnLim), dQMax);
    double dQ=(FlwExists(m_QmMeas) ? dQLim : dQUnLim);

    double QmMem=m_Qm;
    m_Qm+=dQ;//*m_CnvFilter;
    //Qm=Range(-QMax, Qm, QMax);

    if (Loop>0 && fabs(dQ)/GTZ(m_QmMeas) < SignificanceRatio) // CNM
      break;
    }
  EstFlw=m_Qm;
  _asm int 3; // next line must be be restored
  //*(static_cast<CFlwBlkData*>(this))=Mem;

  return b;
  };

//--------------------------------------------------------------------------

void CFlwBlkBase::EvalProducts(double Pi, double Po, SpConduit & C, CFlwThermalBlk & FTB)
  {
  SpConduit * pFlwCd=m_pCd;
  m_pCd=&C;
  Eqn()->EvalProducts(*this, Pi, Po, FTB);
  m_pCd=pFlwCd;
  };

//--------------------------------------------------------------------------

void CFlwBlkBase::EvalDiscrete()
  {
  Eqn()->EvalDiscrete(*this);
  }

//--------------------------------------------------------------------------

void CFlwBlkBase::SetMomentum(CSpPropInfo *pProps, double P)
  {
  CSpPropInfo *p=Properties(pProps);

  const double Scl=1.0e-3;///9.81;
  if (fMomentumOn && (Area()>1.0e-12) && 0)
    {
    double dPmB_=0.0;
    double dPmQ_=0.0;
    double dPmdQ_=0.0;
    double Density=MeanRho(pProps, P);
    double RhoA=GTZ(Density*Area());
    double VelStart=m_QmEstGood/RhoA;
    double Vel0=m_Qm/RhoA;
    m_Vel=Vel0;
    double dQm=Sign(m_Qm)*Max(fabs(m_Qm)*0.001, 1.0e-6);
    double Vel1=(m_Qm+dQm)/RhoA;
    double Mass=RhoA*ActLength();
    double dTA=ICGetTimeInc()*Area();

    m_dPmB=Mass*(VelStart)*Scl/dTA;
    m_dPmQ=-Mass*(Vel0)*Scl/dTA;
    m_dPmdQ=-Mass*(Vel1-Vel0)*Scl/dTA/dQm;
    m_fMomentumValid=true;
    //dbgpln("dT:%8.4f Ms:%8.4f Vs:%14.6g  V0:%14.6g  dPm0:%14.6g  dPm1:%14.6g  dPmdQ1:%14.6g",
    //        Max(ICGetTimeInc()*0.01, IC.GetTimeIncFromStartOfStep()), Mass,
    //        VelGood, Vel0, dPm0_, dPm1_, dPmdQ1_);
    }
  else
    {
    m_dPmB=0.0;
    m_dPmQ=0.0;
    m_dPmdQ=0.0;
    m_fMomentumValid=false;
    };
  }

void CFlwBlkBase::AddMomentum(CSpPropInfo *pProps, double P)
  {
  double _dPmB;
  double _dPmQ;
  double _dPmdQ;
  if (m_fMomentumValid)
    {
    _dPmB  = m_dPmB;
    _dPmQ  = m_dPmQ;
    _dPmdQ = m_dPmdQ;
    }
  else
    {
    _dPmB  = 0;
    _dPmQ  = 0;
    _dPmdQ = 0;
    }

  SetMomentum(pProps, P);

  m_dPmB  += _dPmB;
  m_dPmQ  += _dPmQ;
  m_dPmdQ += _dPmdQ;
  }

void CFlwBlkBase::ClrMomentum()
  {
  m_dPmB=0.0;
  m_dPmQ=0.0;
  m_dPmdQ=0.0;
  m_fMomentumValid=false;
  };

//--------------------------------------------------------------------------

double CFlwBlkBase::PAtFromEntry(double Frac)
  {
  return (m_sQMeas>0) ? FlngPB().P+Frac*DPtot() : NodePB().P-Frac*DPtot();
  };

//==========================================================================
//
//
//
//==========================================================================

double CFrictionFactor::Value(double Diam, double Roughness, double Vel, double Dens, double LVisc)
  {
  double Re = Reynolds(Diam, Vel, Dens, LVisc);

  if (m_Cnt==1 && fabs(Re-m_ReLo)<1.0)
    return m_FacLo;

  if (m_Cnt==2 && Re>=m_ReLo && Re<=m_ReHi)
    return m_FacLo+(Re-m_ReLo)*(m_FacHi-m_FacLo)/NZ(m_ReHi-m_ReLo);

  double A = Pow((-2.457 * log(Pow(7.0/Re,0.9) + 0.27 * Roughness/Diam)), 16);
  double B = Pow((37530.0/Re), 16);
  // Crane Eqn 1-4
  double F = 8 * Pow((Pow((8/Re), 12) + 1.0/Pow((A + B), 1.5)),0.0833);

  double Tol=0.01*Re;
  switch (m_Cnt)
    {
    case 0:
      m_FacLo=F;
      m_ReLo=Re;
      m_Cnt=1;
      break;
    case 1:
      {
      double ReX=fabs(Re-m_ReLo);
      if (ReX<Tol && ReX>1.0)
        {
        m_FacHi=F;
        m_ReHi=Re;
        if (m_ReLo>=m_ReHi)
          {
          Swap(m_ReLo, m_ReHi);
          Swap(m_FacLo, m_FacHi);
          }
        m_Cnt=2;
        }
      else
        {
        m_FacLo=F;
        m_ReLo=Re;
        }
      }
      break;
    case 2:
      {
      //double ReMid=0.5*(m_ReLo+m_ReHi);
      if (Re>m_ReHi)
        {
        if (Re-m_ReLo<Tol)
          {
          m_FacHi=F;
          m_ReHi=Re;
          }
        else
          {
          m_FacLo=m_FacHi;
          m_ReLo=m_ReHi;
          m_FacHi=F;
          m_ReHi=Re;
          }
        }
      else // Re<m_ReLo
        {
        if (m_ReHi-Re<Tol)
          {
          m_FacLo=F;
          m_ReLo=Re;
          }
        else
          {
          m_FacHi=m_FacLo;
          m_ReHi=m_ReLo;
          m_FacLo=F;
          m_ReLo=Re;
          }
        }
      }
      break;
    }
  return F;
  };

//==========================================================================
//
//
//
//==========================================================================

double PressDropKFactMult=1.0;

//==========================================================================
//
//
//
//==========================================================================

CFlwThermalBlk::CFlwThermalBlk(FlwNode * Nd, CFlwEqnPtr * FEP)//, CVLEBase * VLE, CEnvironHXBase * EHX, CReactionBase * RB)
  {
  m_pNd             = Nd;
  m_pFEP            = FEP;
  m_pCd             = NULL;
  
  m_TRiseSpec       = TR_None;
  m_TRiseRqd        = 0;
  m_TFinalRqd       = Std_T;
  m_AutoCompOn      = false;
  m_PVConst         = 1.0;

  m_TIn             = Std_T;           
  m_TOut            = m_TIn;          

  m_TRiseMech       = 0.0;     
  m_TRiseHeat       = 0.0;     
  m_TRiseVLE        = 0.0;     
  m_TRiseEHX        = 0.0;     
  m_TRiseRB         = 0.0;      
  m_TRiseEvap       = 0.0;      
  m_TRiseAC         = 0.0;      
  m_TRiseTot        = 0.0;
  m_TRiseCorr       = 0.0;

  m_PowerMech       = 0.0;     
  m_PowerHeat       = 0.0;     
  m_PowerEHX        = 0.0;     
  m_PowerRB         = 0.0;      
  m_PowerEvap       = 0.0;      
  m_PowerAC         = 0.0;      
  m_PowerTot        = 0.0;
  m_PowerCorr       = 0.0;
  };

//--------------------------------------------------------------------------

void CFlwThermalBlk::BuildDataDefn(DataDefnBlk & DDB, char* Tag, char* TagComment, DDBPages PageIs, byte Flags)
  {
  if ((Flags & FEOF_WithAutoComp)==0)
    m_AutoCompOn=false;
  if ((Flags & (FEOF_WithPVConst|FEOF_ShowPVConst))==0)
    m_PVConst=1.0;

  if (DDB.BeginStruct(m_pNd, Tag, TagComment, PageIs, -1))//, DDEF_TREECLOSED))
    {
    static DDBValueLst DDBTRiseSpec[] = 
      {
        { TR_None,      "None"},
        { TR_TChange,   "TRise"},
        { TR_TFinal ,   "TFinal"},
      };
    DDB.Byte   ("TOut.Spec",         "",  DC_,    "",  &m_TRiseSpec,    m_pNd, isParm|SetOnChange, DDBTRiseSpec);
    DDB.Visibility(SHM_All, m_TRiseSpec==TR_TChange);
    DDB.Double ("TOut.Rise",          "",  DC_dT,  "C", &m_TRiseRqd,     m_pNd, isParm);
    DDB.Visibility(SHM_All, m_TRiseSpec==TR_TFinal);
    DDB.Double ("TOut.Final",         "",  DC_T,   "C", &m_TFinalRqd,    m_pNd, isParm);
    DDB.Visibility();
    DDB.Text("");
    if (Flags & FEOF_WithAutoComp)
      DDB.CheckBox("AutoCompression", "",  DC_,    "",  &m_AutoCompOn,   m_pNd, isParm);
    if (Flags & (FEOF_WithPVConst|FEOF_ShowPVConst))
      {
      DDB.Double ("PVConst",          "",  DC_,    "",  &m_PVConst,  m_pNd, (Flags & FEOF_WithPVConst)?isParm:0);
      DDB.TagComment((fabs(m_PVConst-1.0)<1e-8) ? "IsoThermal" : "PolyTropic");
      }
    DDB.Text("");
    DDB.Double ("Temperature.In",     "",  DC_T,   "C", &m_TIn,          m_pNd, isResult|noFileAtAll);
    DDB.Double ("Temperature.TOut",   "",  DC_T,   "C", &m_TOut,         m_pNd, isResult|noFileAtAll);

    DDB.Text("");
    DDB.Double ("TRise.Work",         "",  DC_dT,  "C", &m_TRiseMech,    m_pNd, isResult|noFileAtAll);
    DDB.Double ("TRise.Heat",         "",  DC_dT,  "C", &m_TRiseHeat,    m_pNd, isResult|noFileAtAll);
    if (Flags & FEOF_WithAutoComp)  
      DDB.Double ("TRise.AutoComp",   "",  DC_dT,  "C", &m_TRiseAC,      m_pNd, isResult|noFileAtAll);
    DDB.Double ("TRise.VLE",          "",  DC_dT,  "C", &m_TRiseVLE,     m_pNd, isResult|noFileAtAll);
    DDB.Double ("TRise.Reactions",    "",  DC_dT,  "C", &m_TRiseRB,      m_pNd, isResult|noFileAtAll);
    DDB.Double ("TRise.Evaporation",  "",  DC_dT,  "C", &m_TRiseEvap,    m_pNd, isResult|noFileAtAll);
    DDB.Double ("TRise.Environment",  "",  DC_dT,  "C", &m_TRiseEHX,     m_pNd, isResult|noFileAtAll);
    DDB.Double ("TRise.Correction",   "",  DC_dT,  "C", &m_TRiseCorr,    m_pNd, isResult|noFileAtAll);
    DDB.Double ("TRise.Total",        "",  DC_dT,  "C", &m_TRiseTot,     m_pNd, isResult|noFileAtAll);
    //if (Valid(m_TRiseRqd))
    //  DDB.TagComment("Enforced");

    DDB.Text("");
    DDB.Double ("Power.Work",         "",  DC_Pwr,  "kW",   &m_PowerMech,    m_pNd, isResult|noFileAtAll);
    DDB.Double ("Power.Heat",         "",  DC_Pwr,  "kW",   &m_PowerHeat,    m_pNd, isResult|noFileAtAll);
    if (Flags & FEOF_WithAutoComp)
      DDB.Double ("Power.AutoComp",   "",  DC_Pwr,  "kW",   &m_PowerAC,      m_pNd, isResult|noFileAtAll);
    DDB.Double ("Power.Reactions",    "",  DC_Pwr,  "kW",   &m_PowerRB,      m_pNd, isResult|noFileAtAll);
    DDB.Double ("Power.Evaporation",  "",  DC_Pwr,  "kW",   &m_PowerRB,      m_pNd, isResult|noFileAtAll);
    DDB.Double ("Power.Environment",  "",  DC_Pwr,  "kW",   &m_PowerEHX,     m_pNd, isResult|noFileAtAll);
    DDB.Double ("Power.Correction",   "",  DC_Pwr,  "kW",   &m_PowerCorr,    m_pNd, isResult|noFileAtAll);
    DDB.Double ("Power.Total",        "",  DC_Pwr,  "kW",   &m_PowerTot,     m_pNd, isResult|noFileAtAll);
    }
  DDB.EndStruct();
  };

//--------------------------------------------------------------------------

flag CFlwThermalBlk::DataXchg(DataChangeBlk & DCB)
  {
  return 0;
  };

//--------------------------------------------------------------------------

flag CFlwThermalBlk::ValidateData(ValidateDataBlk & VDB)
  {
  switch (m_TRiseSpec)
    {
    case TR_TChange: m_TRiseRqd=Range(-5000.0,Valid(m_TRiseRqd)?m_TRiseRqd:0.0, 5000.0);                break;
    case TR_TFinal:  m_TFinalRqd=Range(gs_CDB.MinT, Valid(m_TFinalRqd)?m_TFinalRqd:Std_T, gs_CDB.MaxT); break;
    };
    
  m_PVConst=Range(1.0,m_PVConst,1.5);
  return 1;
  };    

//--------------------------------------------------------------------------

void CFlwThermalBlk::BeginEvalProducts(SpConduit & Cd) 
  {
  #if (dbgFlwEqns)
  if (dbgThermals())
    dbgpln("BeginThermals -------- %s", m_pNd->FullObjTag()); 
  #endif
  m_pCd         = &Cd;
  m_HfIn        = m_pCd->totHf();           
  m_HfOut       = m_HfIn;          

  m_TIn         = m_pCd->Temp();           
  m_TOut        = m_TIn;          
  
  m_TRiseMech   = 0.0;     
  m_TRiseHeat   = 0.0;     
  m_TRiseVLE    = 0.0;     
  m_TRiseEHX    = 0.0;     
  m_TRiseRB     = 0.0;      
  m_TRiseEvap   = 0.0;      
  m_TRiseAC     = 0.0;      
  m_TRiseCorr   = 0.0;
  m_TRiseTot    = 0.0;

  m_PowerMech    = 0.0;     
  m_PowerHeat    = 0.0;     
  m_PowerEHX     = 0.0;     
  m_PowerRB      = 0.0;      
  m_PowerEvap    = 0.0;      
  m_PowerAC      = 0.0;      
  m_PowerTot     = 0.0;
  m_PowerCorr    = 0.0;
  };

//--------------------------------------------------------------------------

void CFlwThermalBlk::AddWorkPower(double Power, double Press) 
  {
  #if (dbgFlwEqns)
  if (dbgThermals())
    dbgpln("AddWorkPower %10.2f %10.2f %10.2f %10.2f", Power, m_pCd->totHz(), K2C(m_pCd->Temp()), m_pCd->Press()); 
  m_TRiseMechTmp=m_TRiseMech;      
  #endif
  m_PowerMech+=Power;
  m_TRiseMech-=m_pCd->Temp();      
  //m_pCd->SetHoldEquilibrate(); 
  //double Hf=m_pCd->totHf()+Power; 

  const int MaxDT=1000;
  double DT=Power/NZ(m_pCd->totCp());
  if (fabs(DT)>1000)
    Power*=1000/DT;

  m_pCd->Set_totHf(m_pCd->totHf()+Power, Press); 
  m_TRiseMech+=m_pCd->Temp();      
  #if (dbgFlwEqns)
  if (dbgThermals())
    dbgpln("                       %10.2f %10.2f %10.2f Vel:%+10.5f %10.5f", m_pCd->totHz(), K2C(m_pCd->Temp()), m_pCd->Press(), m_TRiseMech-m_TRiseMechTmp, m_TRiseMech); 
  #endif
  };

//--------------------------------------------------------------------------

void CFlwThermalBlk::AddHeatPower(double Power, double Press) 
  {
  #if (dbgFlwEqns)
  if (dbgThermals())
    dbgpln("AddHeatPower %10.2f %10.2f %10.2f %10.2f", Power, m_pCd->totHz(), K2C(m_pCd->Temp()), m_pCd->Press()); 
  m_TRiseHeatTmp=m_TRiseHeat;      
  #endif
  m_PowerHeat+=Power;
  m_TRiseHeat-=m_pCd->Temp();      
  //m_pCd->SetHoldEquilibrate(); 
  //double Hf=m_pCd->totHf()+Power; 

  const int MaxDT=1000;
  double DT=Power/NZ(m_pCd->totCp());
  if (fabs(DT)>1000)
    Power*=1000/DT;

  m_pCd->Set_totHf(m_pCd->totHf()+Power, Press); 
  m_TRiseHeat+=m_pCd->Temp();      
  #if (dbgFlwEqns)
  if (dbgThermals())
    dbgpln("                       %10.2f %10.2f %10.2f Heat:%+10.5f %10.5f", m_pCd->totHz(), K2C(m_pCd->Temp()), m_pCd->Press(), m_TRiseHeat-m_TRiseHeatTmp, m_TRiseHeat); 
  #endif
  };

//--------------------------------------------------------------------------

void CFlwThermalBlk::AddVLEBegin() 
  {
  #if (dbgFlwEqns)
  if (dbgThermals())
    dbgpln("AddVLEBegin            %10.2f %10.2f %10.2f", m_pCd->totHz(), K2C(m_pCd->Temp()), m_pCd->Press()); 
  m_TRiseVLETmp=m_TRiseVLE;      
  #endif
  m_TRiseVLE-=m_pCd->Temp();      
  };
void CFlwThermalBlk::AddVLEEnd() 
  {
  m_TRiseVLE+=m_pCd->Temp();      
  #if (dbgFlwEqns)
  if (dbgThermals())
    dbgpln("                       %10.2f %10.2f %10.2f VLE:%+10.5f %10.5f", m_pCd->totHz(), K2C(m_pCd->Temp()), m_pCd->Press(), m_TRiseVLE-m_TRiseVLETmp, m_TRiseVLE); 
  #endif
  };

//--------------------------------------------------------------------------

void CFlwThermalBlk::AddEHXBegin() 
  {
  #if (dbgFlwEqns)
  if (dbgThermals())
    dbgpln("AddEHXBegin            %10.2f %10.2f %10.2f", m_pCd->totHz(), K2C(m_pCd->Temp()), m_pCd->Press()); 
  m_TRiseEHXTmp=m_TRiseEHX;      
  #endif
  m_PowerEHX-=m_pCd->totHf();
  m_TRiseEHX-=m_pCd->Temp();      
  };
void CFlwThermalBlk::AddEHXEnd() 
  {
  m_PowerEHX+=m_pCd->totHf();
  m_TRiseEHX+=m_pCd->Temp();      
  #if (dbgFlwEqns)
  if (dbgThermals())
    dbgpln("                      %10.2f %10.2f %10.2f  EHX:%+10.5f %10.5f", m_pCd->totHz(), K2C(m_pCd->Temp()), m_pCd->Press(), m_TRiseEHX-m_TRiseEHXTmp, m_TRiseEHX); 
  #endif
  };

//--------------------------------------------------------------------------

void CFlwThermalBlk::AddRBBegin() 
  {
  #if (dbgFlwEqns)
  if (dbgThermals())
    dbgpln("AddRBBegin             %10.2f %10.2f %10.2f", m_pCd->totHz(), K2C(m_pCd->Temp()), m_pCd->Press()); 
  m_TRiseRBTmp=m_TRiseRB;      
  #endif
  m_PowerRB-=m_pCd->totHf();
  m_TRiseRB-=m_pCd->Temp();      
  };
void CFlwThermalBlk::AddRBEnd() 
  {
  m_PowerRB+=m_pCd->totHf();
  m_TRiseRB+=m_pCd->Temp();      
  #if (dbgFlwEqns)
  if (dbgThermals())
    dbgpln("                       %10.2f %10.2f %10.2f  RB:%+10.5f %10.5f", m_pCd->totHz(), K2C(m_pCd->Temp()), m_pCd->Press(), m_TRiseRB-m_TRiseRBTmp, m_TRiseRB); 
  #endif
  };

//--------------------------------------------------------------------------

void CFlwThermalBlk::AddEvapBegin() 
  {
  #if (dbgFlwEqns)
  if (dbgThermals())
    dbgpln("AddEvapBegin           %10.2f %10.2f %10.2f", m_pCd->totHz(), K2C(m_pCd->Temp()), m_pCd->Press()); 
  m_TRiseEvapTmp=m_TRiseEvap;      
  #endif
  m_PowerEvap-=m_pCd->totHf();
  m_TRiseEvap-=m_pCd->Temp();      
  };
void CFlwThermalBlk::AddEvapEnd() 
  {
  m_PowerEvap+=m_pCd->totHf();
  m_TRiseEvap+=m_pCd->Temp();      
  #if (dbgFlwEqns)
  if (dbgThermals())
    dbgpln("                       %10.2f %10.2f %10.2f  Evap:%+10.5f %10.5f", m_pCd->totHz(), K2C(m_pCd->Temp()), m_pCd->Press(), m_TRiseEvap-m_TRiseEvapTmp, m_TRiseEvap); 
  #endif
  };

//--------------------------------------------------------------------------

void CFlwThermalBlk::AddACPower(double Power, double Press) 
  {
  #if (dbgFlwEqns)
  if (dbgThermals())
    dbgpln("AddACPower  %10.2f %10.2f %10.2f %10.2f", Power, m_pCd->totHz(), K2C(m_pCd->Temp()), m_pCd->Press()); 
  m_TRiseACTmp=m_TRiseAC;      
  #endif
  m_PowerAC+=Power;
  m_TRiseAC-=m_pCd->Temp();      
  m_pCd->Set_totHf(m_pCd->totHf()+Power, Press); 
  m_TRiseAC+=m_pCd->Temp();      
  #if (dbgFlwEqns)
  if (dbgThermals())
    dbgpln("                       %10.2f %10.2f %10.2f  AC:%+10.5f %10.5f", m_pCd->totHz(), K2C(m_pCd->Temp()), m_pCd->Press(), m_TRiseAC-m_TRiseACTmp, m_TRiseAC); 
  #endif
  };

//--------------------------------------------------------------------------

void CFlwThermalBlk::EndEvalProducts() 
  {
  m_HfOut    = m_pCd->totHf();           

#if UseNewThermalBlock
  if (m_TRiseSpec!=TR_None)
    {
    double TR;
    switch (m_TRiseSpec)
      {
      case TR_TChange: TR=m_TRiseRqd;         break;
      case TR_TFinal:  TR=m_TFinalRqd-m_TIn;  break;
      };

    //if (Valid(m_TRiseRqd))
    //  TR=m_TRiseRqd;
    //else if (Valid(m_TFinalRqd))
    //  TR=m_TFinalRqd-m_TIn;

    m_TRiseCorr = TR-(m_pCd->Temp()-m_TIn);
    m_pCd->SetTempPress(m_TIn+TR, m_pCd->Press());
    m_PowerCorr = m_pCd->totHf()-m_HfOut;
    m_HfOut    = m_pCd->totHf();           
    #if (dbgFlwEqns)
    if (dbgThermals())
      dbgpln("PowerRqd    %10.2f %10.2f %10.2f %10.2f", m_PowerCorr, m_pCd->totHz(), K2C(m_pCd->Temp()), m_pCd->Press()); 
    #endif
    }
#endif

  m_TOut     = m_pCd->Temp();
  m_TRiseTot = m_TOut-m_TIn; 
  m_PowerTot = m_HfOut-m_HfIn;

  m_pCd=NULL;
  #if (dbgFlwEqns)
  if (dbgThermals())
    dbgpln("EndThermals ---------- "); 
  #endif
  };

//--------------------------------------------------------------------------

void CFlwThermalBlk::GetClosureInfo(CClosureInfo &CI, bool DoRB, bool DoEHX)
  {
  CI.m_PowerIn += PowerAC()+PowerMech();
  if (DoRB)
    CI.m_RCTPowerIn += PowerRB();
  if (DoEHX)
    CI.m_EHXPowerIn += PowerEHX();
  if (m_TRiseSpec!=TR_None)
    {
    CI.m_PowerIn += m_PowerCorr;
    }
  };

// ==========================================================================
//
//
//
// ==========================================================================
