//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
#define  __HEATX1_CPP
#include "heatx1.h"

//#include "optoff.h"

#define dbgHeatX           WITHDEBUG

#if (1 && dbgHeatX && !defined(_RELEASE))
#define CHECKTEMPS(h, c, x) if (h.To>h.Ti+x || h.To<c.Ti-x || c.To>h.Ti+x || c.To<c.Ti-x) DoBreak();
#else
#define CHECKTEMPS(h, c, x)
#endif
                                  
#if dbgHeatX
#include "dbgmngr.h"
static CDbgMngr  dbgHeater          ("HeatXChg",  "HeatXchg1");
static CDbgMngr  dbgDumpHXCurves    ("HeatXChg",  "DumpHXCurves");
static CDbgMngr  dbgDumpHXConverge  ("HeatXChg",  "DumpHXConverge");
static CDbgMngr  dbgDumpHXConvergeFT("HeatXChg",  "DumpHXConvergeFT");
static CDbgMngr  dbgDerivs          ("HeatXChg",  "Derivs");
static CDbgMngr  dbgODEOperate      ("HeatXChg",  "ODEOperate");
static CDbgMngr  dbgODEOperateSpc   ("HeatXChg",  "ODEOperateSpc");
#endif

#define BOX(x,y,w,h) x,y, x,y+h, x+w,y+h, x+w,y, x,y

#define UseSteelStructure 0
#define dbgF 0


// ==========================================================================
//
//
//
// ==========================================================================

XID xidHXRqdOpMode = ModelXID(1000);
XID xidHXSd_dT     = ModelXID(1001);
XID xidHXPriSg     = ModelXID(1002);
XID xidHXSecSg     = ModelXID(1003);

//==========================================================================
/*#D:#T:Shell & Tube Heat Exchanger(1)
#X:#h<General Description>#n
With all streams connected, calculate the outlet temperature and duty.#n
With liquor streams connected, specify the outlet temperature and steam pressure, assume all steam condensate, and calculate steam usage.
#n#n
#n#h<Inputs and Outputs>#w
Label    Input/Output  Minimum number  Maximum number#n
liquor in    Input          1              20#n
steam        Input          1              20#n
liquor out   Output         1              1#n
condensate   Output         1              1#n
vent         Output         0              1#n
#a#n
#n#h<Variables to be supplied by the user>#n
#i<EnvironHX> : Is there heat transfer with the environment.#n
#i<EHX.Model> : If there is heat transfer with the environment, this determines the model
to be used.#n
#i<QmVentRqd> : #n
#i<HTC> : Heat Transfer Coefficient#n
#i<Area> : #n
#i<LMTDFact> : #n
#n#n
#h<Associated variables>#n
#i<Duty> : #n
#i<LMTD> : #n
#i<m_Pri.Mode> : #n
#i<m_Pri.Ti> : #n
#i<m_Pri.To> : Primary Outlet temperature#n
#i<m_Sec.Mode> : #n
#i<m_Sec.Ti> : #n
#i<m_Sec.To> : Secendary Outlet temperature#n
#n#n
#n#h<Assumptions & Limitations>#n
Given only the liquor streams then assume that all steam is condensing in order to calculate the steam usage.
#n
#n#h<Other>#n
Default model prefix:ST#n
Short name:ShllTube#n
Model type:Unit#n
#G:Units
*/

// This Has define to be replaced with (puresat + bpe) if neccesary
#define BPE 0.0

//===========================================================================
//
//
//
//===========================================================================

static DDBValueLst DDBOpMode[]={
  {HX1_Inoperative      ,   "Inoperative"},
  {HX1_LiquorLiquor     ,   "Liquor/Liquor"},
  {HX1_LiquorGas        ,   "Liquor/Gas"},
  {HX1_GasGas           ,   "Gas/Gas"},
  {HX1_FullyCondensing  ,   "FullyCondensing"},
  //{HX1_PartialCondensing, "PartialCondensing"},
  {0}};

IMPLEMENT_HXB(CHXBlock1, "HXBlock", "1", TOC_ALL|TOC_GRP_ENERGY|TOC_STD_KENWALT, "HeatExchangeBlk", "HeatExchangeBlock");
CHXBlock1::CHXBlock1(pTagObjClass pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach) :
  CHXBlock(pClass_, Tag_, pAttach, eAttach),
  m_VLE(this, VLEF_QPFlash),
  m_FTC(dynamic_cast<FlwNode*>(pAttach))//,
  {
  m_iRqdOpMode=HX1_LiquorLiquor;
  m_iActOpMode=HX1_Inoperative;

  m_dDuty=0.0;
  m_dTheoryDuty=0.0;
  m_dLMTD=0.0;
  m_dLMTDFactor=1.0;
  m_dHTC=0.980;
  m_dArea=1.0;

  m_fPriIsHot=0;

  m_dV2LQ=0.0;
  m_dV2LQZeroDuty=0.0;
  m_dV2LQVFrac=0.0;
  m_dV2LQLFrac=0.0;

  m_dSrgDH=0;
  //for (int s=0; s<SDB.No(); s++)
  //  m_SrgDM[s]=0;
  m_SrgDM.SetSpcScalar(0);

  }

//--------------------------------------------------------------------------

CHXBlock1::~CHXBlock1()
  {
  };

//--------------------------------------------------------------------------

void CHXBlock1::BuildDataDefn(DataDefnBlk & DDB)
  {
  if (DDB.BeginStruct(this, NULL, NULL, DDB_RqdPage))
    {
    DDB.Text("");
    DDB.Visibility();

    DDB.Byte  ("RqdOpMode",     "", DC_,     "",         xidHXRqdOpMode,    this, isParmStopped, DDBOpMode);
    DDB.Byte  ("ActOpMode",     "", DC_,     "",         &m_iActOpMode,     this, isResult, DDBOpMode);

    DDB.CheckBox("On",          "", DC_,     "",         &m_fOn,            this, isParm|SetOnChange);
    DDB.Double("HTC",           "", DC_HTC,  "kW/m^2.K", &m_dHTC,           this, isParm);
    DDB.Double("Area",          "", DC_Area, "m^2",      &m_dArea,          this, isParm);

    DDB.Visibility(NM_Dynamic|SM_All|HM_All, m_bLvlEffOn);
    DDB.Byte("CrossSection",    "", DC_,     "",          &m_iXSect,        this, isParm|AffectsStruct, DDBXSect);
    DDB.Double ("MaxEffLvl",    "", DC_Frac, "%",         &m_dMxEffLvl,     this, isParm);
    DDB.Double ("MinEffLvl",    "", DC_Frac, "%",         &m_dMnEffLvl,     this, isParm);
    DDB.Double ("MaxEff",       "", DC_Frac, "%",         &m_dMxEff,        this, isParm);
    DDB.Double ("MinEff",       "", DC_Frac, "%",         &m_dMnEff,        this, isParm);
    DDB.Double ("Level",        "", DC_Frac, "%",         xidHXLevel,       this, isResult);
    DDB.Double ("NonCondense",  "", DC_Frac, "%",         xidHXNonCondenseFrac, this, isResult);
    DDB.Double ("EffectArea",   "", DC_Area, "m^2",       xidHXEffArea,     this, isResult);

    DDB.Visibility();
    DDB.Double("U*A",           "", DC_UA,   "kW/K",     xidHXEffUA,        this, isResult);
    DDB.Double("LMTD",          "", DC_dT,   "C",        &m_dLMTD,          this, isResult);
    DDB.Visibility(NM_Dynamic|SM_All|HM_All);
    DDB.Double("TheoryDuty",    "", DC_Pwr,  "kW",       &m_dTheoryDuty,    this, isResult);
    DDB.Visibility();
    DDB.Double("Duty",          "", DC_Pwr,  "kW",       &m_dDuty,          this, isResult);
    DDB.Double("LMTDFact",      "", DC_Frac, "%",        &m_dLMTDFactor,    this, (m_iSolveMode==HXM_LMTD) ? isParm : isResult|0);
    DDB.Text("");

    DDB.Visibility(NM_Dynamic|SM_All|HM_All);
    if (DDB.BeginStruct(this, "Structure", NULL, DDB_NoPage))
      {
      #if UseSteelStructure
      DDB.Double("Mass",          "", DC_M,    "kg",       &m_Structure.m_Mass, this, isParm);
      #else
      m_Structure.m_Mass = 0.0; //force to zero!
      DDB.Double("Mass",          "", DC_M,    "kg",       &m_Structure.m_Mass, this, 0);
      #endif
      DDB.Visibility(NM_Dynamic|SM_All|HM_All, m_Structure.m_Mass>1.0e-6);
      DDB.Double("Cp",            "", DC_CpMs,   "kJ/kg.C",  &m_Structure.m_Cp, this, isParm);
      DDB.Double("TempH",         "", DC_T,    "C",        &m_Structure.m_Th, this, isParm);
      DDB.Double("TempC",         "", DC_T,    "C",        &m_Structure.m_Tc, this, isParm);
      DDB.Double("TempHF",        "", DC_T,    "C",        &m_Structure.m_ThF, this, isParmConstruct);
      DDB.Double("TempCF",        "", DC_T,    "C",        &m_Structure.m_TcF, this, isParmConstruct);
      DDB.Double("TempdH",        "", DC_dTdt, "C/s",      &m_Structure.m_dTh, this, isResult);
      DDB.Double("TempdC",        "", DC_dTdt, "C/s",      &m_Structure.m_dTc, this, isResult);
      DDB.Text("");
      DDB.Visibility();
      }
    DDB.EndStruct();
    DDB.Visibility();

    m_FTC.BuildDataDefn(DDB);

    DDB.Double("QmVentRqd",     "", DC_Qm,   "kg/s",     &m_QmVentRqd,      this, isParm/*|NAN_OK*/); //this is effectively steam bypass!?
    DDB.Text("");

    DDB.Page("..", DDB_OptPage);

    m_Pri.BuildDataDefn("Pri", DDB, this, m_Pri.m_pCn==NULL, m_Pri.fConnected, true);
    m_Sec.BuildDataDefn("Sec", DDB, this, m_Sec.m_pCn==NULL, m_Sec.fConnected, false);

    m_VLE.BuildDataDefn(DDB);

    DDB.Text("");
    }
  DDB.EndStruct();
  }


//---------------------------------------------------------------------------

flag CHXBlock1::DataXchg(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
    case xidHXRqdOpMode:
      if (DCB.rB)
        {
        m_iRqdOpMode=*DCB.rB;
        if (m_iRqdOpMode==HX1_FullyCondensing)
          m_VLE.Open();
        else
          m_VLE.Close();
        }
      DCB.B=m_iRqdOpMode;
      return 1;
    default:
      break;
    }

  if (m_VLE.DataXchg(DCB))
    return 1;
  if (CHXBlock::DataXchg(DCB))
    return 1;

  return 0;
  }


//---------------------------------------------------------------------------

flag CHXBlock1::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=True;
  m_Structure.m_Mass=Range(0.0, m_Structure.m_Mass, 1.0e15);
  if (!m_VLE.ValidateData(VDB))
    OK=False;
  if (!CHXBlock::ValidateData(VDB))
    OK=False;
  return OK;
  }

//===========================================================================
//
//  Condensate Finder
//
//===========================================================================

class CondenseFinder : public MRootFinderBase
  {
  protected:
    CHXBlock1 & Blk;
    CHXSide    & HotSd;
    CHXSide    & ColdSd;
    CVLEBase    & m_VLE;
    SpConduit  & QFlsh;
    SpConduit  & QMem;
    static CToleranceBlock s_Tol;
  public:
    CondenseFinder(CHXBlock1* Blk_, CHXSide & HotSd_, CHXSide & ColdSd_, SpConduit &QMemS, SpConduit *pQFlsh);
    ~CondenseFinder();
    LPCTSTR ObjTag() { return (LPCTSTR)Blk.FullObjTag(); };
    //void FlashIt(double Extent, CHXSide & HotSd, CHXSide & ColdSd);
    double Function(double x);
    double TheoryDuty();
  };

CToleranceBlock CondenseFinder::s_Tol(TBF_Both, "HeatExchange:Condense", 0.0, 1.0e-8);

//---------------------------------------------------------------------------

CondenseFinder::CondenseFinder(CHXBlock1* Blk_, CHXSide & HotSd_, CHXSide & ColdSd_, SpConduit &QMemS, SpConduit * pQFlsh) :
  Blk(*Blk_), HotSd(HotSd_), ColdSd(ColdSd_),
  MRootFinderBase("BaseFnd", s_Tol),//1.0e-8),
  QFlsh(pQFlsh ? *pQFlsh : *HotSd.m_pCd),
  QMem(QMemS),
  m_VLE(Blk_->m_VLE)
  {
  QFlsh.SetBadTempOK(True);
  ColdSd.m_pCd->SetBadTempOK(True);
  };

//---------------------------------------------------------------------------

CondenseFinder::~CondenseFinder()
  {
  QFlsh.SetBadTempOK(False);
  ColdSd.m_pCd->SetBadTempOK(False);
  };

//---------------------------------------------------------------------------

double CondenseFinder::TheoryDuty()
  {
  CHECKTEMPS(HotSd, ColdSd, 1000);
  double TDuty=0.0;
  if (Blk.EffUA()>1.0e-6)
    {
    //Blk.m_dLMTD=CHXBlock::LMTD(False, m_Pri.SatT, m_Pri.SatT, m_Sec.Ti, m_Sec.To);
    Blk.m_dLMTD=CHXBlock::LMTDSat(ColdSd.Ti, ColdSd.To, HotSd.SatT-BPE);
    TDuty = Blk.EffUA() * Blk.m_dLMTD * Blk.m_dLMTDFactor;
    }
  else
    {
    Blk.m_dLMTD=0.0;
    TDuty = 0.0;
    }

  return TDuty;
  };

//---------------------------------------------------------------------------

double CondenseFinder::Function(double Amount)
  {
  QFlsh.QSetM(QMem, som_ALL, Amount, QMem.Press());
  HotSd.Ti = QFlsh.Temp();
  HotSd.Ci = QFlsh.totCp();
  HotSd.Cp = QFlsh.msCp();
  HotSd.Qm = QFlsh.QMass();
  HotSd.Hi = QFlsh.totHf();

  double FTemp=Max(ColdSd.Ti, HotSd.SatT-BPE);
  double FPress=HotSd.Po;
  m_VLE.SetFlashVapFrac(*QFlsh.Model(), FTemp, FPress, 0.0, VLEF_Null);
  HotSd.To=FTemp;//QFlsh.Temp();
  Blk.m_dDuty=fabs(QFlsh.totHf()-HotSd.Hi);

  ColdSd.m_pCd->Set_totHf(ColdSd.Hi+ColdSd.Sgn*Blk.m_dDuty+ColdSd.dHEnv+HotSd.dHEnv);
  ColdSd.To=ColdSd.m_pCd->Temp();
  // Temp Returned Could have small errors (due to convergence methods)
  // Check To is on the correvt side of Ti.
  if (ColdSd.Sgn>0) // Heating Up
    ColdSd.To=Max(ColdSd.Ti+1.0e-3, ColdSd.To);
  else
    ColdSd.To=Min(ColdSd.Ti-1.0e-3, ColdSd.To);
  ColdSd.iMode=QPF_Sensible;
  HotSd.iMode=QPF_Condensing;
  ColdSd.Ho=ColdSd.m_pCd->totHf();
  HotSd.Ho=QFlsh.totHf();
  CHECKTEMPS(HotSd, ColdSd,1000);

  double TDuty=GEZ(TheoryDuty());
  double Diff=fabs(Blk.m_dDuty)-TDuty;
  #if (dbgF)
  //if (fabs(Diff)<100.0)
  dbgpln("CF: X:%12.4f  Qp:%10.4f Qs:%10.4f dHp:%14.2f dHs:%14.4f ADty:%14.4f TDty:%14.4f Vf:%8.4f %s",
          Amount, HotSd.Qm, ColdSd.Qm, HotSd.Hi-HotSd.Ho, ColdSd.Hi-ColdSd.Ho, fabs(Blk.m_dDuty), TDuty,
          ColdSd.m_pCd->MassFrac(som_Gas), Blk.FullObjTag());
  #endif

  return Diff;
  };

//===========================================================================
//
//
//
//===========================================================================

void CHXBlock1::FindLimitingDuties(byte OpMode, CHXSide& HotSd, CHXSide& ColdSd, const double ApproachDT)//, CHXSdInfo &HotI, CHXSdInfo &ColdI)
  {
  CHECKTEMPS(HotSd, ColdSd,1);

  double dMaxDuty1=1e30;
  if (OpMode==HX1_FullyCondensing)
    {
    // All Hot Condensing .. No Change in Cold
    //dMaxDuty1=EffUA()*LMTDSat(ColdSd.Ti, ColdSd.Ti, HotSd.SatT-BPE);
    double HotSdToEst=Min(HotSd.Ti, HotSd.SatT-BPE);
    HotSd.m_pCd->SetTemp(ColdSd.Ti);
    HotSd.MaxNfDuty=HotSd.Hi-HotSd.m_pCd->totHf();
    m_VLE.SetFlashVapFrac(*HotSd.m_pCd->Model(), HotSdToEst, HotSd.Po, 0.0, VLEF_Null);
    HotSd.MaxFlDuty=HotSd.Hi-HotSd.m_pCd->totHf();
    //dbgpln("FC : ToE:%10.3f To:%10.3f D:%14.5g %s", HotSdToEst, HotSd.m_pCd->Temp(), m_dDuty, pAttachedTo->FullObjTag());
    }
  else
    {
    HotSd.m_pCd->SetTemp(Min(ColdSd.Ti+ApproachDT, HotSd.Ti));
    HotSd.MaxNfDuty=HotSd.Hi-HotSd.m_pCd->totHf();
    HotSd.MaxFlDuty=HotSd.MaxNfDuty;
    }

  double dMaxDuty2=1e30;
  if (OpMode==HX1_FullyCondensing)
    {
    // All Cold To Max Temp .. No Hot Condensing
    //dMaxDuty2=EffUA()*LMTD(False, ColdSd.Ti, HotSd.SatT-BPE, HotSd.Ti, HotSd.SatT-BPE);
    //ColdSd.m_pCd->SetTemp(Min(HotSd.Ti, HotSd.SatT-BPE));
    ColdSd.m_pCd->SetTemp(HotSd.Ti);
    ColdSd.MaxNfDuty=ColdSd.m_pCd->totHf()-ColdSd.Hi;
    ColdSd.MaxFlDuty=ColdSd.MaxNfDuty;
    }
  else
    {
    // All Cold To Max Temp .. No Hot Change
    //dMaxDuty2=EffUA()*LMTD(False, ColdSd.Ti, HotSd.Ti, HotSd.Ti, HotSd.Ti);
    ColdSd.m_pCd->SetTemp(Max(HotSd.Ti-ApproachDT, ColdSd.Ti));
    ColdSd.MaxNfDuty=ColdSd.m_pCd->totHf()-ColdSd.Hi;
    ColdSd.MaxFlDuty=ColdSd.MaxNfDuty;
    }

  m_dMaxDuty=EffUA()*fabs(ColdSd.Ti-HotSd.Ti);//Min(dMaxDuty1, dMaxDuty2);
  //==============================================

  HotSd.MaxNfDuty=Min(m_dMaxDuty, HotSd.MaxNfDuty);
  ColdSd.MaxNfDuty=Min(m_dMaxDuty, ColdSd.MaxNfDuty);
  HotSd.MaxFlDuty=Min(m_dMaxDuty, HotSd.MaxFlDuty);
  ColdSd.MaxFlDuty=Min(m_dMaxDuty, ColdSd.MaxFlDuty);

  m_dMaxNfDuty=Min(ColdSd.MaxNfDuty, HotSd.MaxNfDuty);
  m_dMaxFlDuty=Min(ColdSd.MaxFlDuty, HotSd.MaxFlDuty);
  }

//===========================================================================

double CHXBlock1::CondenseAll(flag AllowVent, flag MixVentBack, CHXSide& HotSd, CHXSide& ColdSd)//, CHXSdInfo &HotI, CHXSdInfo &ColdI)
  {
  flag fCoCurrent=False;
  double VentFlow=0.0;

  if (AllowVent)
    {
//   m_VLE.QPFlash(*HotSd.m_pCd, HotSd.Po, -m_dMaxFlDuty, VLEF_Null);
////    m_VLE.SetFlashVapFrac(*HotSd.m_pCd, HotSd.SatT-BPE, HotSd.Po, 0.0, VLEF_Null);
//    HotSd.To=HotSd.m_pCd->Temp();
//    m_dDuty=fabs(HotSd.m_pCd->totHf()-HotSd.Hi);
//
//    VentFlow=m_VLE.FlashVapFrac(*HotSd.m_pCd);
//    HotSd.AddGasToVent(VentFlow);
//    m_dDuty=m_dMaxFlDuty;

    double HotSdToEst=Min(HotSd.Ti, HotSd.SatT-BPE);

    double OrigVapFrac=m_VLE.FlashVapFrac(*HotSd.m_pCd->Model());
    m_VLE.SetFlashVapFrac(*HotSd.m_pCd->Model(), HotSdToEst, HotSd.Po, 0.0, VLEF_Null);
    HotSd.To=HotSd.m_pCd->Temp();
    m_dDuty=fabs(HotSd.m_pCd->totHf()-HotSd.Hi);

//dbgpln("CA : ToE:%10.3f To:%10.3f D:%14.5g %s", HotSdToEst, HotSd.m_pCd->Temp(), m_dDuty, pAttachedTo->FullObjTag());

    if (m_dDuty>m_dMaxFlDuty)
      {
      double VentFrac=(m_dDuty-m_dMaxFlDuty)/GTZ(m_dDuty);
      // Adjust for what existed
      VentFrac = VentFrac*(1.0-OrigVapFrac);
      m_VLE.SetFlashVapFrac(*HotSd.m_pCd->Model(), HotSdToEst, HotSd.Po, VentFrac, VLEF_Null);
      VentFlow=HotSd.m_pCd->QMass(som_Gas);
      //Move Gases to Vent
      HotSd.AddGasToVent(VentFlow, MixVentBack!=0);
//      // Remove from HotSd Product
//      HotSd.QScaleMass(som_Gas, 0.0);
      m_dDuty=m_dMaxFlDuty;
      }
    m_dDuty=Min(m_dDuty, HotSd.MaxFlDuty);
    }
  else
    {
    double HotSdToEst=Min(HotSd.Ti, HotSd.SatT-BPE);
    m_VLE.SetFlashVapFrac(*HotSd.m_pCd->Model(), HotSdToEst, HotSd.Po, 0.0, VLEF_Null);
    HotSd.To=HotSd.m_pCd->Temp();
    m_dDuty=fabs(HotSd.m_pCd->totHf()-HotSd.Hi);
    m_dDuty=Min(m_dDuty, HotSd.MaxFlDuty);
    }

  HotSd.iMode=QPF_Condensing;
  HotSd.Ho=HotSd.m_pCd->totHf();

  ColdSd.m_pCd->SetPress(ColdSd.Po);
  ColdSd.m_pCd->Set_totHf(ColdSd.Hi+ColdSd.Sgn*m_dDuty+ColdSd.dHEnv+HotSd.dHEnv);
  ColdSd.To=ColdSd.m_pCd->Temp();
  ColdSd.iMode=QPF_Sensible;
  ColdSd.Ho=ColdSd.m_pCd->totHf();

  CHECKTEMPS(HotSd, ColdSd, 1000);

//  m_dLMTD=LMTDSat(ColdSd.Ti, ColdSd.To, HotSd.SatT-BPE);

  return VentFlow;
  }

//===========================================================================

void CHXBlock1::SolveForDutyBrent(CHXSide &HotSd, CHXSide &ColdSd, double ApproachDT)
  {
  // Find Mode Of Operation
  FindLimitingDuties(m_iActOpMode, HotSd, ColdSd, ApproachDT);//, HotSdI, ColdSdI);

  CHXDutyFinder DutyFnd(this, HXF_CounterCurrent, HXM_LMTD);
  DutyFnd.SetTarget(0.0);

  double MaxHXExtent=1.0;
  switch (DutyFnd.Start(0.0, MaxHXExtent))
    {
    case RF_OK:
      if (DutyFnd.Solve_Brent()==RF_OK)
        break;
      //SetCondition(2);
      break;
    case RF_LoLimit:
      DutyFnd.Function(0.0);
      break;
    case RF_HiLimit:
      // Flash the output
      DutyFnd.Function(MaxHXExtent);
      break;
    }
  }

//===========================================================================

#define SET_ENTH_AND_TO(Sd, H) \
  if (Sd.m_pCd)\
    {\
    Sd.m_pCd->Set_totHf(H);\
    Sd.Ho=H;\
    Sd.To=Sd.m_pCd->Temp();\
    }\
  else\
    {\
    Sd.Ho=H;\
    Sd.To=Sd.Ti+(Sd.Ho-Sd.Hi)/GTZ(Sd.Ci);\
    };

//===========================================================================

double CHXBlock1::SolveForDutySubsFn(double UA, CHXSide &SdA, CHXSide &SdB, double DutyEst, double DutyB)
  {
  double Sgn=(SdA.Ti>SdB.Ti) ? 1 : -1;
  SdA.Duty=-Sgn*DutyEst + SdA.dHEnv;
  SdB.Duty=Sgn*DutyEst + DutyB + SdB.dHEnv;
  if (SdA.m_pCd)
    SdA.m_pCd->QRestoreMass(SdA.MassImg);
  if (SdB.m_pCd)
    SdB.m_pCd->QRestoreMass(SdB.MassImg);
  SET_ENTH_AND_TO(SdA, SdA.Hi+SdA.Duty);
  SET_ENTH_AND_TO(SdB, SdB.Hi+SdB.Duty);

  if (SdA.m_pCd)
    SdA.m_pCd->SetPress(SdA.Po);
  if (SdB.m_pCd)
    SdB.m_pCd->SetPress(SdB.Po);
  SdA.iMode=QPF_Sensible;
  SdB.iMode=QPF_Sensible;

  m_dLMTD=LMTD(False, SdB.Ti, SdB.To, SdA.Ti, SdA.To);
  double CalcDuty=UA * m_dLMTD;
  return Min(CalcDuty, m_dMaxNfDuty);
  }

//===========================================================================

void CHXBlock1::SolveForDutySubs(CHXSide &HotSd, CHXSide &ColdSd, const double ApproachDT)
  {
  // Find Mode Of Operation
  FindLimitingDuties(m_iActOpMode, HotSd, ColdSd, ApproachDT);//, HotSdI, ColdSdI);

  double UA=EffUA() * m_dLMTDFactor;
  double Est=fabs(m_dDuty);
  double A=0.0;
  double B=m_dMaxNfDuty;
  double ErrA=SolveForDutySubsFn(UA, HotSd, ColdSd, A)-A;
  double ErrB=SolveForDutySubsFn(UA, HotSd, ColdSd, B)-B;

  // Sensible Heat Xfer Only
  for (int i=0; i<100; i++)
    {
    double Err=SolveForDutySubsFn(UA, HotSd, ColdSd, Est)-Est;
    double E1;
    if (ConvergedDV(Err, Est, 1.0e-8, 1.0e-8, E1))
//    if (fabs(Err)<1.0e-8*Est)
      goto Done;

    if (Err>=0.0)
      {
      A=Est;
      ErrA=Err;
      }
    else
      {
      B=Est;
      ErrB=Err;
      }
    Est=A+(B-A)*(0.0-ErrA)/(ErrB-ErrA);
    }
  LogNote(FullObjTag(), 0, "Duty Not Converged");

Done:
  m_dDuty=Est;

  }

//===========================================================================

CStructure::CStructure()
  {
  m_Ci=0.0;
  m_Cp=0.12;
  m_dTh=0.0;
  m_dTc=0.0;
  m_Th=293.16;
  m_Tc=293.16;
  m_Hi=0.0;
  m_Ho=0.0;
  m_Duty=0.0;
  m_Mass=0.0;//500.0;
  m_ThF=293.16;
  m_TcF=293.16;
  };

//===========================================================================

double CHXBlock1::FindFinalTSFn(CHXSide &HotSd, CHXSide &ColdSd, double TSif, double TSof, double LMTDRqdH, double LMTDRqdC,
                                double *pErrH0, double *pErrC0)
  {
  double dLMTDH0=LMTD1(true, HotSd.Ti, HotSd.To, TSif, TSof);
  double dLMTDC0=LMTD1(true, ColdSd.Ti, ColdSd.To, TSof, TSif);
  double ErrH0=dLMTDH0-LMTDRqdH;
  double ErrC0=dLMTDC0-LMTDRqdC;
  if (pErrH0)
    *pErrH0=ErrH0;
  if (pErrC0)
    *pErrC0=ErrC0;
  return ErrH0+ErrC0;
  }

//===========================================================================

void CHXBlock1::FindFinalTS(double UAH, double UAC, CHXSide &HotSd, CStructure &Struct, CHXSide &ColdSd)
  {
  if (!TstNZ(m_Pri.Ci))
    {
//    Struct.m_Th = ColdSd.Ti;
//    Struct.m_Tc = ColdSd.Ti;
    Struct.m_ThF = ColdSd.Ti;
    Struct.m_TcF = ColdSd.Ti;
    Struct.m_dTh = 0;
    Struct.m_dTc = 0;
    }
  else if (!TstNZ(m_Sec.Ci))
    {
//    Struct.Th = HotSd.Ti;
//    Struct.Tc = HotSd.Ti;
    Struct.m_ThF = HotSd.Ti;
    Struct.m_TcF = HotSd.Ti;
    Struct.m_dTh = 0;
    Struct.m_dTc = 0;
    }
  else
    {
    SolveForDutyBrent(HotSd, ColdSd, 1.0e-9);
//    SolveForDutySubs(HotSd, ColdSd);

    m_dLMTD=LMTD1(False, ColdSd.Ti, ColdSd.To, HotSd.Ti, HotSd.To);

    double X=HotSd.Ci/(ColdSd.Ci+HotSd.Ci);
    if (Struct.m_ThF>=HotSd.Ti || Struct.m_ThF<=ColdSd.To ||
        Struct.m_TcF>=HotSd.To || Struct.m_TcF<=ColdSd.Ti)
      {
      Struct.m_ThF = X*(HotSd.Ti-ColdSd.To)+ColdSd.To;
      Struct.m_TcF = X*(HotSd.To-ColdSd.Ti)+ColdSd.Ti;
      }
    double LMTDRqdH=m_dLMTD*UAC/GTZ(UAH+UAC);
    double LMTDRqdC=m_dLMTD*UAH/GTZ(UAH+UAC);

    int dbgCurves=0;
    if (dbgCurves)
      {
      dbgpln("Iter\tStruct.m_ThF\tTSof\tErrH\tErrC");
      Struct.m_ThF=ColdSd.To;
      Struct.m_TcF=ColdSd.Ti;
      }

  #if dbgHeatX
  if (dbgDumpHXConvergeFT())
    dbgpln("---------------------------------------------");
  #endif

    double EH, EC;
    double ErrH0, ErrC0;
    double ErrHIx, ErrCIx, ErrHIy, ErrCIy;
    double ErrHOx, ErrCOx, ErrHOy, ErrCOy;

    double DTH=0.0001*(HotSd.Ti-ColdSd.To);
    double DTC=0.0001*(HotSd.To-ColdSd.Ti);
    for (int i=0; i<250; i++)
      {
      double ErrSum=FindFinalTSFn(HotSd, ColdSd, Struct.m_ThF, Struct.m_TcF, LMTDRqdH, LMTDRqdC, &ErrH0, &ErrC0);

      double TSif1=Struct.m_ThF+DTH;
      double ErrSumI1=FindFinalTSFn(HotSd, ColdSd, TSif1, Struct.m_TcF, LMTDRqdH, LMTDRqdC, &ErrHIx, &ErrCIx);
      double TSif2=Struct.m_ThF-DTH;
      double ErrSumI2=FindFinalTSFn(HotSd, ColdSd, TSif2, Struct.m_TcF, LMTDRqdH, LMTDRqdC, &ErrHIy, &ErrCIy);

      double ChgHx=ErrHIx-ErrH0;
      double ChgHy=ErrH0-ErrHIy;

      double TSof1=Struct.m_TcF+DTC;
      double ErrSumO1=FindFinalTSFn(HotSd, ColdSd, Struct.m_ThF, TSof1, LMTDRqdH, LMTDRqdC, &ErrHOx, &ErrCOx);
      double TSof2=Struct.m_TcF-DTC;
      double ErrSumO2=FindFinalTSFn(HotSd, ColdSd, Struct.m_ThF, TSof2, LMTDRqdH, LMTDRqdC, &ErrHOy, &ErrCOy);

      double ChgHI=0.5*(ErrHIx-ErrHIy)/DTH;
      double ChgHO=0.5*(ErrHOx-ErrHOy)/DTC;
      double ChgCI=0.5*(ErrCIx-ErrCIy)/DTH;
      double ChgCO=0.5*(ErrCOx-ErrCOy)/DTC;

      if (dbgCurves)
        {
        dbgpln("%i\t%g\t%g\t%g\t%g",i,Struct.m_ThF,Struct.m_TcF,ErrH0,ErrC0);
        if (i%25==0)
          {
          Struct.m_ThF=ColdSd.To;
          Struct.m_TcF+=(HotSd.To-ColdSd.Ti)/11;
          }
        else
          Struct.m_ThF+=(HotSd.Ti-ColdSd.To)/26;
        continue;
        }


      if (ConvergedDV(ErrH0, LMTDRqdH, 1.0e-6, 1.0e-6, EH) && ConvergedDV(ErrC0, LMTDRqdC, 1.0e-6, 1.0e-6, EC))
        goto Done;

      double ReqdChgErrH=-ErrH0;
      double ReqdChgErrC=-ErrC0;

      double ChgTI=(ChgCO*ReqdChgErrH-ChgHO*ReqdChgErrC)/NZ(ChgCO*ChgHI-ChgHO*ChgCI);
      double ChgTO=(ReqdChgErrC-ChgTI*ChgCI)/NZ(ChgCO);

if(1)
  {
  ChgTI*=0.5;
  ChgTO*=0.5;
  }

//      ChgTI=Range(-0.25*GEZ(Struct.m_ThF-ColdSd.To), ChgTI, 0.25*GEZ(HotSd.Ti-Struct.m_ThF));
//      ChgTO=Range(-0.25*GEZ(Struct.m_TcF-ColdSd.Ti), ChgTO, 0.25*GEZ(HotSd.To-Struct.m_TcF));
      ChgTI=Range(-0.5*GEZ(Struct.m_ThF-ColdSd.To), ChgTI, 0.5*GEZ(HotSd.Ti-Struct.m_ThF));
      ChgTO=Range(-0.5*GEZ(Struct.m_TcF-ColdSd.Ti), ChgTO, 0.5*GEZ(HotSd.To-Struct.m_TcF));

      #if dbgHeatX
      if (i>240 || dbgDumpHXConvergeFT())
        dbgpln("%i  %14.8g %14.8g %14.8g %14.8g %14.8g %14.8g", i, Struct.m_ThF, Struct.m_TcF, ErrH0, ErrC0, ChgTI, ChgTO);
      #endif

      Struct.m_ThF+=ChgTI;
      Struct.m_TcF+=ChgTO;
      }
    // NBNB Leave as the best estimate
    //Struct.m_ThF = X*(HotSd.Ti-ColdSd.To)+ColdSd.To;
    //Struct.m_TcF = X*(HotSd.To-ColdSd.Ti)+ColdSd.Ti;
    LogNote(FullObjTag(), 0, "Steel Final Temps Not Converged");

Done:;

    }
  };

//===========================================================================

double CHXBlock1::SolveForDutySteelFn(double UA, CHXSide &Flow, CStructure &Struct, CHXSide &Other,
                                      double DutyEst, double OtherDuty,
                                      double TSi, double TSo,
                                      double TSif, double TSof,
                                      double &dTSi, double &dTSo)
  {
  // DutyEst Direction is from Side (Liquor) to m_Structure
  // Stream
  if (FlowExists(Flow, Struct))
    {
    Flow.Duty=-DutyEst + Flow.dHEnv;
    if (Flow.m_pCd)
      Flow.m_pCd->QRestoreMass(Flow.MassImg);
    SET_ENTH_AND_TO(Flow, Flow.Hi+Flow.Duty);
    if (Flow.m_pCd)
      Flow.m_pCd->SetPress(Flow.Po);
    Flow.iMode=QPF_Sensible;
    }
  else
    {
    Flow.Ti=TSi;
    Flow.To=TSo;
    }

  if (FlowExists(Other, Struct))
    {
    Other.Duty=-OtherDuty + Other.dHEnv;
    if (Other.m_pCd)
      Other.m_pCd->QRestoreMass(Other.MassImg);
    SET_ENTH_AND_TO(Other, Other.Hi+Other.Duty);
    if (Other.m_pCd)
      Other.m_pCd->SetPress(Other.Po);
    Other.iMode=QPF_Sensible;
    }
  else
    {
    Other.Ti=TSo;
    Other.To=TSi;
    }

  // Steel
  Struct.m_Duty=DutyEst+OtherDuty;
  Struct.m_Ho=Struct.m_Hi+Struct.m_Duty;

//  double TSm=(TSi+TSo)/2;

  double dTSm=(Struct.m_Ho-Struct.m_Hi)/Struct.m_Ci;
//  double xdTSi=UA*((Flow.Ti-TSif)+(Other.To-TSif))/(Struct.m_Ci);
//  double xdTSo=UA*((Flow.To-TSof)+(Other.Ti-TSof))/(Struct.m_Ci);
  double xdTSi=UA*(TSif-TSi)/(Struct.m_Ci);
  double xdTSo=UA*(TSof-TSo)/(Struct.m_Ci);
  double xdTSm=(xdTSi+xdTSo)/2;

  double TSLimit=0.5;
  xdTSi=Range(TSLimit*LEZ(TSif-TSi), xdTSi, TSLimit*GEZ(TSif-TSi));
  xdTSo=Range(TSLimit*LEZ(TSof-TSo), xdTSo, TSLimit*GEZ(TSof-TSo));

  double xdTErr=0.0;//dTSm-xdTSm;

  dTSi=xdTSi+xdTErr;
  dTSo=xdTSo+xdTErr;

//  if (Flow.Ti>Other.Ti)
//    {
//    dTSi=Range(0.25*LEZ(Other.To-TSi), dTSi, 0.25*GEZ(Flow.Ti-TSi));
//    dTSo=Range(0.25*LEZ(Other.Ti-TSo), dTSo, 0.25*GEZ(Flow.To-TSo));
//    }
//  else
//    {
//    dTSi=Range(0.25*LEZ(Flow.Ti-TSi), dTSi, 0.25*GEZ(Other.To-TSi));
//    dTSo=Range(0.25*LEZ(Flow.To-TSo), dTSo, 0.25*GEZ(Other.Ti-TSo));
//    }


  m_dLMTD=LMTD1(true, TSi+dTSi, TSo+dTSo, Flow.Ti, Flow.To);

  double CalcDuty=UA * m_dLMTD *(Flow.Ti>TSi ? 1 : -1);

  if (pAttachedTo->fDoDbgBrk)
    { int xxx=0; }

//  return Min(CalcDuty, m_dMaxNfDuty);
  return CalcDuty;
  }

//===========================================================================

void CHXBlock1::SolveForDutySteel(CHXSide &HotSd, CStructure &Struct, CHXSide &ColdSd)
  {
  //UnUsed;
  m_dMaxNfDuty=1e30;
  m_dMaxFlDuty=1e30;
  // Adjust Transfer COeff up by factor of 2
//  double EffUA=2.0 * EffUA() * m_dLMTDFactor;

  double X=Range(0.001, HotSd.Ci/GTZ(ColdSd.Ci), 1000.0);
  double EffUAO=EffUA() * m_dLMTDFactor;
  double EffUAC, EffUAH;
if (0)
  {
  EffUAC=(1+X)*EffUAO/X;
  EffUAH=(1+X)*EffUAO;
  }
else
  {
  EffUAC=2*EffUAO;
  EffUAH=2*EffUAO;
  }

  // Reconstruct Th & Tc
  double StMn=0.5*(Struct.m_Th+Struct.m_Tc);
//  double StMn=0.5*(Struct.m_Th+Struct.m_Tc);

  double ErrH_Lo, EstH_Lo;
  double ErrC_Lo, EstC_Lo;
  double ErrH_Hi, EstH_Hi;
  double ErrC_Hi, EstC_Hi;
  double dStTmH, dStTmC;//, dStTmC;
  if (!FlowExists(HotSd, Struct))
    {
    HotSd.Ti=Struct.m_Th;
    HotSd.To=Struct.m_Tc;
    }
  if (!FlowExists(ColdSd, Struct))
    {
    ColdSd.Ti=Struct.m_Tc;
    ColdSd.To=Struct.m_Th;
    }

  FindFinalTS(EffUAH, EffUAC, HotSd, Struct, ColdSd);


  // Assume UA=Large
  double TFinal=ColdSd.Ti+(HotSd.Ti-ColdSd.Ti)*HotSd.Ci/GTZ(ColdSd.Ci+HotSd.Ci);
  // If all ends up at Final Temp
  double EstH_Max=(TFinal-ColdSd.Ti)*ColdSd.Ci+(TFinal-Min(TFinal, StMn))*Struct.m_Ci;
  // If all ends up at Hot Steel
  double EstH_Min=(HotSd.Ti-Max(HotSd.Ti, StMn))*Struct.m_Ci;


  // If all ends up at Hot Temp
  double EstC_Min=(TFinal-HotSd.Ti)*HotSd.Ci+(Max(TFinal, StMn)-TFinal)*Struct.m_Ci;
  // If all ends up at Cold Steel
  double EstC_Max=(ColdSd.Ti-Min(ColdSd.Ti, StMn))*Struct.m_Ci;

  if (EstH_Min>EstH_Max)
    Exchange(EstH_Min, EstH_Max);
  if (EstC_Min>EstC_Max)
    Exchange(EstC_Min, EstC_Max);

  #if dbgHeatX
  if (dbgDumpHXCurves())
    {
    dbgpln("Iter\tEstH\tErrH\tEstC\tErrC");
    double EstH=EstH_Min;
    double EstC=EstC_Min;
    double dStTmH, dStTmC;//, dStTmC;
    const int NPts=200;
    for (int i=0;i<NPts; i++)
      {
      double ErrH=SolveForDutySteelFn(EffUAH, HotSd, Struct, ColdSd, EstH, 0.0, Struct.m_Th, Struct.m_Tc, Struct.m_ThF, Struct.m_TcF, dStTmH, dStTmC)-EstH;
      double ErrC=SolveForDutySteelFn(EffUAC, ColdSd, Struct, HotSd, EstC, 0.0, Struct.m_Tc, Struct.m_Th, Struct.m_TcF, Struct.m_ThF, dStTmC, dStTmH)-EstC;

      dbgpln("%i\t%g\t%g\t%g\t%g",i, EstH, ErrH, EstC, ErrC);

      EstH+=(EstH_Max-EstH_Min)/(NPts-1);
      EstC+=(EstC_Max-EstC_Min)/(NPts-1);
      }
    }
  #endif

  EstH_Lo=EstH_Min;
  EstH_Hi=EstH_Max;
  EstC_Lo=EstC_Min;
  EstC_Hi=EstC_Max;

  double EstH, ErrH;
  double EstC, ErrC;

  #if dbgHeatX
  if (dbgDumpHXConverge())
    dbgpln("---------------------------------------------");
  #endif

  flag HErrsOK=false;
  flag CErrsOK=false;
  for (int i=0;i<250; i++)
    {
    // Bisection
    EstH=0.5*(EstH_Lo+EstH_Hi);
    EstC=0.5*(EstC_Lo+EstC_Hi);

    ErrH=SolveForDutySteelFn(EffUAH, HotSd, Struct, ColdSd, EstH, EstC, Struct.m_Th, Struct.m_Tc, Struct.m_ThF, Struct.m_TcF, dStTmH, dStTmC)-EstH;
    ErrC=SolveForDutySteelFn(EffUAC, ColdSd, Struct, HotSd, EstC, EstH, Struct.m_Tc, Struct.m_Th, Struct.m_TcF, Struct.m_ThF, dStTmC, dStTmH)-EstC;

    #if dbgHeatX
    if (i>240 || dbgDumpHXConverge())
      dbgpln("%i  H:%14.4g<%14.4g<%14.4g [%14.4g]  C:%14.4g<%14.4g<%14.4g [%14.4g]",i, EstH_Lo, EstH, EstH_Hi, ErrH, EstC_Lo, EstC, EstC_Hi, ErrC);
    #endif

    flag HOk=ConvergedDV(ErrH, EstH, 1.0e-6, 1.0e-6) || ConvergedVV(EstH_Lo, EstH_Hi, 1.0e-6, 1.0e-6);
    flag COk=ConvergedDV(ErrC, EstC, 1.0e-6, 1.0e-6) || ConvergedVV(EstC_Lo, EstC_Hi, 1.0e-6, 1.0e-6);
    if (HOk && COk)
      goto Done;

    flag AdjustH = !HOk && fabs(ErrH)>fabs(ErrC) || COk;
    if (AdjustH)
      {
      CErrsOK=false;
      if (!HErrsOK)
        {
        HErrsOK=true;
        ErrH_Lo=SolveForDutySteelFn(EffUAH, HotSd, Struct, ColdSd, EstH_Lo, EstC, Struct.m_Th, Struct.m_Tc, Struct.m_ThF, Struct.m_TcF, dStTmH, dStTmC)-EstH_Lo;
        ErrH_Hi=SolveForDutySteelFn(EffUAH, HotSd, Struct, ColdSd, EstH_Hi, EstC, Struct.m_Th, Struct.m_Tc, Struct.m_ThF, Struct.m_TcF, dStTmH, dStTmC)-EstH_Hi;
        // Check that Lo..Hi straddle Zero;
        while (ErrH_Lo*ErrH_Hi>0.0)
          {
          if (fabs(ErrH_Lo)<fabs(ErrH_Hi))
            {
            // Move Lo
            EstH_Lo-=Max(1.0e-6, EstH_Hi-EstH_Lo);
            ErrH_Lo=SolveForDutySteelFn(EffUAH, HotSd, Struct, ColdSd, EstH_Lo, EstC, Struct.m_Th, Struct.m_Tc, Struct.m_ThF, Struct.m_TcF, dStTmH, dStTmC)-EstH_Lo;
            #if dbgHeatX
            if (dbgDumpHXConverge()) dbgpln(" H_Lo -- %14.4g [%14.4g]", EstH_Lo, ErrH_Lo);
            #endif
            }
          else
            {
            // Move Hi
            EstH_Hi+=Max(1.0e-6, EstH_Hi-EstH_Lo);
            ErrH_Hi=SolveForDutySteelFn(EffUAH, HotSd, Struct, ColdSd, EstH_Hi, EstC, Struct.m_Th, Struct.m_Tc, Struct.m_ThF, Struct.m_TcF, dStTmH, dStTmC)-EstH_Hi;
            #if dbgHeatX
            if (dbgDumpHXConverge()) dbgpln(" H_Hi ++ %14.4g [%14.4g]", EstH_Hi, ErrH_Hi);
            #endif
            }
          }
        }
      if (ErrH*ErrH_Hi>0) // Same Side as Hi
        {
        ErrH_Hi=ErrH;
        EstH_Hi=EstH;
        }
      else
        {
        ErrH_Lo=ErrH;
        EstH_Lo=EstH;
        }
      }
    else
      {
      HErrsOK=false;
      if (!CErrsOK)
        {
        CErrsOK=true;
        ErrC_Lo=SolveForDutySteelFn(EffUAH, ColdSd, Struct, HotSd, EstC_Lo, EstH, Struct.m_Tc, Struct.m_Th, Struct.m_TcF, Struct.m_ThF, dStTmC, dStTmH)-EstC_Lo;
        ErrC_Hi=SolveForDutySteelFn(EffUAH, ColdSd, Struct, HotSd, EstC_Hi, EstH, Struct.m_Tc, Struct.m_Th, Struct.m_TcF, Struct.m_ThF, dStTmC, dStTmH)-EstC_Hi;
        while (ErrC_Lo*ErrC_Hi>0.0)
          {
          if (fabs(ErrC_Lo)<fabs(ErrC_Hi))
            {
            // Move Lo
            EstC_Lo-=Max(1.0e-6, EstC_Hi-EstC_Lo);
            ErrC_Lo=SolveForDutySteelFn(EffUAH, ColdSd, Struct, HotSd, EstC_Lo, EstH, Struct.m_Tc, Struct.m_Th, Struct.m_TcF, Struct.m_ThF, dStTmC, dStTmH)-EstC_Lo;
            #if dbgHeatX
            if (dbgDumpHXConverge()) dbgpln(" C_Lo -- %14.4g [%14.4g]", EstC_Lo, ErrC_Lo);
            #endif
            }
          else
            {
            // Move Hi
            EstC_Hi+=Max(1.0e-6, EstC_Hi-EstC_Lo);
            ErrC_Hi=SolveForDutySteelFn(EffUAH, ColdSd, Struct, HotSd, EstC_Hi, EstH, Struct.m_Tc, Struct.m_Th, Struct.m_TcF, Struct.m_ThF, dStTmC, dStTmH)-EstC_Hi;
            #if dbgHeatX
            if (dbgDumpHXConverge()) dbgpln(" C_Hi ++ %14.4g [%14.4g]", EstC_Hi, ErrC_Hi);
            #endif
            }
          }
        }
      if (ErrC*ErrC_Hi>0) // Same Side as Hi
        {
        ErrC_Hi=ErrC;
        EstC_Hi=EstC;
        }
      else
        {
        ErrC_Lo=ErrC;
        EstC_Lo=EstC;
        }
      int xxx=0;
      }
    }

  Struct.m_dTh=0;
  Struct.m_dTc=0;
  LogNote(FullObjTag(), 0, "Steel Duty Not Converged");
  return;

Done:;
//  Struct.m_dT=(dStTmH+dStTmC)/ICGetTimeInc();
  Struct.m_dTh=dStTmH/ICGetTimeInc();
  Struct.m_dTc=dStTmC/ICGetTimeInc();

if (1)
  {
  if (pAttachedTo->fDoDbgBrk)
    { int xxx=0; }
  ErrH=SolveForDutySteelFn(EffUAH, HotSd, Struct, ColdSd, EstH, EstC, Struct.m_Th, Struct.m_Tc, Struct.m_ThF, Struct.m_TcF, dStTmH, dStTmC)-EstH;
  ErrC=SolveForDutySteelFn(EffUAC, ColdSd, Struct, HotSd, EstC, EstH, Struct.m_Tc, Struct.m_Th, Struct.m_TcF, Struct.m_ThF, dStTmC, dStTmH)-EstC;
  }

  #if dbgHeatX
  if (dbgDumpHXConverge())
    {
    dbgpln(" H_Lo %14.4g [%14.4g]", EstH_Lo, ErrH_Lo);
    dbgpln(" H    %14.4g [%14.4g]", EstH   , ErrH   );
    dbgpln(" H_Hi %14.4g [%14.4g]", EstH_Hi, ErrH_Hi);
    dbgpln(" C_Lo %14.4g [%14.4g]", EstC_Lo, ErrC_Lo);
    dbgpln(" C    %14.4g [%14.4g]", EstC   , ErrC   );
    dbgpln(" C_Hi %14.4g [%14.4g]", EstC_Hi, ErrC_Hi);
    }
  #endif

//#pragma chNOTE(Temporary)
//  Struct.m_dT=0;

  int xxx=0;
  }

//===========================================================================
//
//  Condensate Finder
//
//===========================================================================

class DutyFinder1 : public MRootFinderBase
  {
  protected:
    CHXBlock1 & Blk;
    CHXSide    & HotSd;
    CHXSide    & ColdSd;
    CVLEBase    & m_VLE;
    SpConduit  & QFlsh;
    SpConduit  & QMem;
    static CToleranceBlock s_Tol;

  public:
    DutyFinder1(CHXBlock1* Blk_, CHXSide & HotSd_, CHXSide & ColdSd_, SpConduit &QMemS, SpConduit *pQFlsh);
    ~DutyFinder1();
    LPCTSTR ObjTag() { return (LPCTSTR)Blk.FullObjTag(); };
    //void FlashIt(double Extent, CHXSide & HotSd, CHXSide & ColdSd);
    double Function(double x);
    double TheoryDuty();
  };

CToleranceBlock DutyFinder1::s_Tol(TBF_Both, "HeatExchange:DutyFinder1", 0.0, 1.0e-8);

//---------------------------------------------------------------------------

DutyFinder1::DutyFinder1(CHXBlock1* Blk_, CHXSide & HotSd_, CHXSide & ColdSd_, SpConduit &QMemS, SpConduit * pQFlsh) :
  Blk(*Blk_), HotSd(HotSd_), ColdSd(ColdSd_),
  MRootFinderBase("BaseFnd", s_Tol), //1.0e-8),
  QFlsh(pQFlsh ? *pQFlsh : *HotSd.m_pCd),
  QMem(QMemS),
  m_VLE(Blk_->m_VLE)
  {
  QFlsh.SetBadTempOK(True);
  ColdSd.m_pCd->SetBadTempOK(True);
  };

//---------------------------------------------------------------------------

DutyFinder1::~DutyFinder1()
  {
  QFlsh.SetBadTempOK(False);
  ColdSd.m_pCd->SetBadTempOK(False);
  };

//---------------------------------------------------------------------------

//inline bool CrossOver(CHXSide & h, CHXSide & c, double x)
//  { return (h.To>h.Ti+x || h.To<c.Ti-x || c.To>h.Ti+x || c.To<c.Ti-x); };

double DutyFinder1::TheoryDuty()
  {
  //CHECKTEMPS(HotSd, ColdSd, 1000);
  double TDuty=0.0;
//  if (Blk.EffUA()>1.0e-6)
//    {
//    if (CrossOver(HotSd, ColdSd, 0.001))
//      {
//      Blk.m_dLMTD=(HotSd.SatT-BPE)-ColdSd.To;
//      TDuty = Blk.EffUA() * Blk.m_dLMTD * Blk.m_dLMTDFactor;
//      }
//    else
//      {
      //Blk.m_dLMTD=CHXBlock::LMTD(False, m_Pri.SatT, m_Pri.SatT, m_Sec.Ti, m_Sec.To);
  Blk.m_dLMTD=CHXBlock::LMTDSat1(ColdSd.Ti, ColdSd.To, HotSd.SatT-BPE);
  TDuty = Blk.EffUA() * Blk.m_dLMTD * Blk.m_dLMTDFactor;
//      }
//    }
//  else
//    {
//    Blk.m_dLMTD=0.0;
//    TDuty = 0.0;
//    }

  return TDuty;
  };

//---------------------------------------------------------------------------

//Frac = 2  : duty = o
//Frac = 1  : Hi-Hs
//Frac < 0  : Hi-Hs +++

double DutyFinder1::Function(double Frac)
  {
  QFlsh.QCopy(QMem, QMem.Press());
  HotSd.Ti = QFlsh.Temp();
  HotSd.Ci = QFlsh.totCp();
  HotSd.Cp = QFlsh.msCp();
  HotSd.Qm = QFlsh.QMass();
  HotSd.Hi = QFlsh.totHf();
  HotSd.Po = QMem.Press();
  HotSd.SatT = QFlsh.SaturationT(HotSd.Po);
  if (Frac>1)
    {
    double FTemp=Max(ColdSd.Ti, HotSd.SatT-BPE);
    double FPress=HotSd.Po;
    m_VLE.SetFlashVapFrac(*QFlsh.Model(), FTemp, FPress, 1.0, VLEF_Null);


   // m_VLE.SetFlashVapFrac(*QFlsh.Model(), FTemp, FPress, Frac, VLEF_Null);

    double Ts = QFlsh.SaturationT(HotSd.Po);
 //   double Ps = QFlsh.SaturationP(HotSd.Ti);

    double Hs= QFlsh.totHf(som_ALL, Ts, HotSd.Po);

    Blk.m_dDuty=(HotSd.Hi-Hs)*(2-Frac);
    HotSd.To=HotSd.Ti-(HotSd.Ti-Ts)*(2-Frac);

    int xxx=0;
    }
  else
    {

    double FTemp=Max(ColdSd.Ti, HotSd.SatT-BPE);
    double FPress=HotSd.Po;
    m_VLE.SetFlashVapFrac(*QFlsh.Model(), FTemp, FPress, Frac, VLEF_Null);
    HotSd.To=FTemp;//QFlsh.Temp();
    Blk.m_dDuty=fabs(QFlsh.totHf()-HotSd.Hi);
    }

  #ifndef _RELEASE
  int OldTET=SpModel::SetDoTestEstTP(0);
  #endif
  ColdSd.m_pCd->Set_totHf(ColdSd.Hi+ColdSd.Sgn*Blk.m_dDuty+ColdSd.dHEnv+HotSd.dHEnv);
  ColdSd.To=ColdSd.m_pCd->Temp();
  // Temp Returned Could have small errors (due to convergence methods)
  // Check To is on the correct side of Ti.
  if (ColdSd.Sgn>0) // Heating Up
    ColdSd.To=Max(ColdSd.Ti+1.0e-3, ColdSd.To);
  else
    ColdSd.To=Min(ColdSd.Ti-1.0e-3, ColdSd.To);
  ColdSd.iMode=QPF_Sensible;
  HotSd.iMode=QPF_Condensing;
  ColdSd.Ho=ColdSd.m_pCd->totHf();
  HotSd.Ho=QFlsh.totHf();

  double Diff, TDuty;
  TDuty=/*GEZ*/(TheoryDuty());
  Diff=fabs(Blk.m_dDuty)-TDuty;
  #ifndef _RELEASE
  SpModel::SetDoTestEstTP(OldTET);
  #endif

  #if (dbgF)
  //if (fabs(Diff)<100.0)
  dbgpln("CF: X:%12.4f  Qp:%10.4f Qs:%10.4f dHp:%14.2f dHs:%14.4f ADty:%14.4f TDty:%14.4f Vf:%8.4f@%6.2fC Vf:%8.4f@%6.2fC %s",
          Frac, HotSd.Qm, ColdSd.Qm, HotSd.Hi-HotSd.Ho, ColdSd.Hi-ColdSd.Ho, fabs(Blk.m_dDuty), TDuty,
          ColdSd.m_pCd->MassFrac(som_Gas), K2C(ColdSd.m_pCd->Temp()), HotSd.m_pCd->MassFrac(som_Gas), K2C(HotSd.m_pCd->Temp()), Blk.FullObjTag());
  #endif
//  dbgpln("Frac: X:%12.4f  LMTD:%10.4f  Duty:%10.4f TDuty:%10.4f Diff:%10.4f %s",
//          Frac, Blk.m_dLMTD, Blk.m_dDuty, TDuty, Diff, Blk.FullObjTag());

  return Diff;
  };

//===========================================================================

void CHXBlock1::EvalProductsDynFCSurge(FlwNode* pNd, bool PriIsHot)
  {
  m_fPriIsHot = PriIsHot;//(m_Pri.Ti>m_Sec.Ti);
  m_Pri.Sgn=(m_fPriIsHot ? -1 : 1);
  m_Sec.Sgn=-m_Pri.Sgn;

  CHXSide &HotSd = (m_fPriIsHot ? m_Pri : m_Sec);
  CHXSide &ColdSd = (m_fPriIsHot ? m_Sec : m_Pri);

  m_iActOpMode=m_iRqdOpMode;

  // Do Reactions Etc
  ColdSd.EvalProducts(ColdSd.To);
  HotSd.EvalProducts(HotSd.To);

  if (ColdSd.m_pCd && HotSd.m_pCd)
    {
    StkSpConduit QResult("QResult", chLINEID(), pNd);
    SpConduit & QInitial=(*HotSd.m_pCd);

    ColdSd.dHEnv=0;// ??? CNM 9/sep/2001 ColdSd.m_pCd ? ColdSd.m_pCd->totHf()-ColdSd.Hi : 0.0;
    HotSd.dHEnv=0;// ??? CNM 9/sep/2001 HotSd.m_pCd->totHf()-HotSd.Hi;


    QResult().QCopy(*HotSd.m_pCd, HotSd.Pi);

    // update data
    HotSd.SetInput(HotSd.m_pCdIn, QResult(), HotSd.Pi, HotSd.m_pEHX, HotSd.m_pRB, HotSd.dFinalTEst, HotSd.fPreReact);

    CSysVector & AR=QResult().MArray();
    CSysVector & AI=QInitial.MArray();

    double qv1=QResult().QMass(som_Gas);
    double ql1=QResult().QMass(som_SL);


    double HotSdToEst=Min(HotSd.Ti, HotSd.SatT-BPE);
    HotSd.m_pCd->SetTemp(ColdSd.Ti);
    HotSd.MaxNfDuty=HotSd.Hi-HotSd.m_pCd->totHf();
    m_VLE.SetFlashVapFrac(*HotSd.m_pCd->Model(), HotSdToEst, HotSd.Po, 0.0, VLEF_Null);
    HotSd.MaxFlDuty=HotSd.Hi-HotSd.m_pCd->totHf();


    double P1=QResult().Press();
    double T1=QResult().Temp();
    double T2=QResult().SaturationT(P1);
    double H1=QResult().totHf();
    double H2=QResult().totHf(som_ALL, T2, P1);


    QResult().QCopy(QInitial, HotSd.Pi);


    m_Structure.m_Ci=m_Structure.m_Cp*m_Structure.m_Mass/ICGetTimeInc();
    m_Structure.m_Hi=0.5*(m_Structure.m_Th+m_Structure.m_Tc)*m_Structure.m_Ci;
    flag HasSteel=(m_Structure.m_Mass>1.0e-6) && (m_Structure.m_Ci>Max(m_Pri.Ci, m_Sec.Ci)*1.0e-6);
    if (!HasSteel)
      {
      m_Structure.m_Th=0.5*(HotSd.Ti+ColdSd.To);
      m_Structure.m_Tc=0.5*(HotSd.To+ColdSd.Ti);
      m_Structure.m_dTh=0;
      m_Structure.m_dTc=0;
      }
    if (HasSteel)
      SolveForDutySteel(HotSd, m_Structure, ColdSd);

    DutyFinder1 CondFnd(this, HotSd, ColdSd, QInitial, &QResult());


    CondFnd.SetTarget(0.0);

    double MaxVf=QResult().MassFrac(som_Gas);
    int iStrtErr=CondFnd.Start(0.0, 2.0);//Max(2.0e-6, 1.0e-6*MaxFlQm), MaxFlQm);
    switch (iStrtErr)
      {
      case RF_OK:
        {
        int iRet=CondFnd.Solve_Brent();
        switch (iRet)
          {
          case RF_OK:
            CondFnd.Result();
            break;
          default:    LogError(FullObjTag(), 0, "Condensate Finder not converged"); break;
          }
        break;
        }
      case RF_LoLimit:
        CondFnd.Function(0);
        break;
      case RF_HiLimit:
        // Flash the output
        CondFnd.Function(MaxVf);
        break;
      }

    double qv2=QResult().QMass(som_Gas);
    double ql2=QResult().QMass(som_SL);
    double qv3=QInitial.QMass(som_Gas);
    double ql3=QInitial.QMass(som_SL);
    HotSd.MeasureHXDataCd(NULL);

    ColdSd.SatP = ColdSd.m_pCd->SaturationP(ColdSd.To);
    HotSd.SatP = HotSd.m_pCd->SaturationP(HotSd.To);

    m_dLMTD=LMTDSat1(ColdSd.Ti, ColdSd.To, HotSd.SatT);
    // Temperature Crossover Error
    CHXBlock::SetCI(3, m_dLMTD<0.0);

    // remember temp change
    m_dInlineDT=(ColdSd.To-ColdSd.Ti);
    m_bInlineIsPri= !m_fPriIsHot;
    //now The Derivatives
    ColdSd.Duty=m_dDuty;
    HotSd.Duty=-m_dDuty;
    m_dSrgDH=-m_dDuty;
    for (int s=0; s<SDB.Count(); s++)
      m_SrgDM.VValue[s]=(AR[s]-AI[s]);
    }
  else
    {
    m_dSrgDH=0;
    m_SrgDM.SetSpcScalar(0);
    //for (int s=0; s<SDB.No(); s++)
    //  m_SrgDM[s]=0;
    }
  }

//===========================================================================

void CHXBlock1::EvalProductsDynFCInline(FlwNode* pNd)
  {
  SpConduit *pQ=m_bInlineIsPri ? m_Pri.m_pCd : m_Sec.m_pCd;
  if (pQ)
    pQ->SetTempPress(pQ->Temp()+m_dInlineDT, pQ->Press());
  if (m_bInlineIsPri)
    m_Pri.To=m_Pri.Ti+m_dInlineDT;
  else
    m_Sec.To=m_Sec.Ti+m_dInlineDT;
  }

//===========================================================================

void CHXBlock1::EvalProducts(FlwNode* pNd, flag MixVentBack)
  {
  m_iActOpMode=m_iRqdOpMode;
  m_dMaxDuty=0.0;
  flag SecCondensing=0;
  flag CoCurrent=false;

  m_Structure.m_Ci=m_Structure.m_Cp*m_Structure.m_Mass/ICGetTimeInc();
  m_Structure.m_Hi=0.5*(m_Structure.m_Th+m_Structure.m_Tc)*m_Structure.m_Ci;
  flag HasSteel=(m_Structure.m_Mass>1.0e-6) && (m_Structure.m_Ci>Max(m_Pri.Ci, m_Sec.Ci)*1.0e-6);
  if (!HasSteel)
    {
    m_Structure.m_dTh=0;
    m_Structure.m_dTc=0;
    }

  if (m_iActOpMode!=HX1_Inoperative && (HasSteel || (TstNZ(m_Pri.Ci) && TstNZ(m_Sec.Ci))) && m_fOn)
    {
    if (!SolveBufferedMethod())
      {
      if (m_FTC.Active() && m_iActOpMode!=HX1_FullyCondensing)
        {
        m_iActOpMode=HX1_FullyCondensing;
        CHXBlock::SetCI(4);
        }
      else if (m_Pri.m_pCn || m_Sec.m_pCn)
        {
        CHXBlock::ClrCI(4);
        m_iActOpMode=HX1_LiquorLiquor;
        }
      else
        CHXBlock::ClrCI(4);
      }
    else
      CHXBlock::ClrCI(4);

    m_fPriIsHot = (m_Pri.Ti>m_Sec.Ti);
    //if (NetProbalMethod())
    //  m_fPriIsHot = false;// CNM Hard Wired
    m_Pri.Sgn=(m_fPriIsHot ? -1 : 1);
    m_Sec.Sgn=-m_Pri.Sgn;

    CHXSide &HotSd = (m_fPriIsHot ? m_Pri : m_Sec);
    CHXSide &ColdSd = (m_fPriIsHot ? m_Sec : m_Pri);

    switch (m_iActOpMode)
      {
      case HX1_LiquorLiquor:
      case HX1_LiquorGas:
      case HX1_GasGas:
        {
        // Vents & Bypasses
        HotSd.SendGasToVent(false);

        // Do Reactions Etc

        double CVf=0.0;
        if (ColdSd.m_pCd)
          {
          CVf=ColdSd.m_pCd->MassFrac(som_Gas);
          ColdSd.EvalProducts(ColdSd.To);
          ColdSd.dHEnv=ColdSd.m_pCd->totHf()-ColdSd.Hi;
          }

        double HVf=0.0;
        if (HotSd.m_pCd)
          {
          HVf=HotSd.m_pCd->MassFrac(som_Gas);
          HotSd.EvalProducts(HotSd.To);
          HotSd.dHEnv=HotSd.m_pCd->totHf()-HotSd.Hi;
          }

        if (CVf>0.5 && HVf>0.5)
          m_iActOpMode=HX1_GasGas;
        else if (CVf<=0.5 && HVf<=0.5)
          m_iActOpMode=HX1_LiquorLiquor;
        else
          m_iActOpMode=HX1_LiquorGas;

        if (HasSteel)
          SolveForDutySteel(HotSd, m_Structure, ColdSd);
        else
          SolveForDutyBrent(HotSd, ColdSd);

        m_dLMTD=LMTD(false, ColdSd.Ti, ColdSd.To, HotSd.Ti, HotSd.To);

        break;
        }
      case HX1_FullyCondensing:
        {
        // Vents & Bypasses
        HotSd.KeepByPassLiquor(HotSd.Pi);
        HotSd.SendGasToVent(true);

        // Do Reactions Etc
        StkSpConduit QMemS("QMemS", chLINEID(), pNd);
        QMemS().QSetF(*HotSd.m_pCd, som_ALL, 1.0, HotSd.Pi);

        double PVf=ColdSd.m_pCd->MassFrac(som_Gas);
        double SVf=HotSd.m_pCd->MassFrac(som_Gas);

        double ColdHi=ColdSd.m_pCd->totHf();
        double HotHi=HotSd.m_pCd->totHf();

        ColdSd.EvalProducts(ColdSd.To);
        HotSd.EvalProducts(HotSd.To);

        if (1) // this needs to go back <<<<<<<<<<<,
          {
          ColdSd.dHEnv=ColdSd.m_pCd->totHf()-ColdHi;
          HotSd.dHEnv=HotSd.m_pCd->totHf()-HotHi;
          }

        // Find Mode Of Operation
        FindLimitingDuties(m_iActOpMode, HotSd, ColdSd);//, HotSdI, ColdSdI);

        // Estimate the required amount able to Condense
        if (m_FTC.Active())
          {
          m_FTC.m_dQCond = HotSd.Qm;
          m_FTC.m_dQCool = ColdSd.Qm;
          m_VLE.SetFlashVapFrac(*HotSd.m_pCd->Model(), Max(HotSd.SatT-BPE, CoCurrent ? ColdSd.To : ColdSd.Ti), HotSd.Po, 0.0, VLEF_Null);
          double OldQRqd=m_FTC.m_dQRqd;
          double MaxFlQm=ColdSd.MaxFlDuty/GTZ(HotSd.Hi/NZ(HotSd.Qm)-HotSd.m_pCd->msHf());
          //MaxFlQm=Min(MaxFlQm, HotSd.Qm);

          CondenseFinder CondFnd(this, HotSd, ColdSd, QMemS(), NULL);
          CondFnd.SetTarget(0.0);
          // NBNB 2.0e-6 must be fixed properly
          // ... it should be 2*MinSpMass - to go into SpModel.H

          int iStrtErr=CondFnd.Start(Max(2.0e-6, 1.0e-6*MaxFlQm), MaxFlQm);
          switch (iStrtErr)
            {
            case RF_OK:
              {
              int iRet=CondFnd.Solve_Brent();
              switch (iRet)
                {
                case RF_OK:
                  m_FTC.m_dQRqd=CondFnd.Result();
                  break;
                default: LogError(FullObjTag(), 0, "Condensate Finder not converged"); break;
                }
              break;
              }
            case RF_LoLimit:
              CondFnd.Function(1.0e-6*MaxFlQm);
              m_FTC.m_dQRqd=1.0e-6*MaxFlQm;
              break;
            case RF_HiLimit:
              // Flash the output
              CondFnd.Function(MaxFlQm);
              m_FTC.m_dQRqd=MaxFlQm;
              break;
            }
          #if dbgF
          for (double i=0; i<20; i+=i<3.0 ? 0.02 : 0.5)
            {
            CondFnd.Function(i); // Cool & Condense
            }
          #endif

          // Prevent oscillations
          m_FTC.m_dQRqd=Max(Max(1.0e-6,0.5*OldQRqd), m_FTC.m_dQRqd);

          //flag CondCnvBad=!ConvergedVV(m_FTC.dQRqd, m_FTC.dQCond, EqnCB().Cfg.dEps_Abs, EqnCB().Cfg.dEps_Rel*2.0);
          //CHXBlock::SetCI(1, CondCnvBad);
          //relax tolerance for message display...
          double d1 = EqnCB().Cfg.dEps_Abs*5.0;
          double d2 = EqnCB().Cfg.dEps_Rel*2.0*5.0;
          const flag CondCnvBad=!ConvergedVV(m_FTC.m_dQRqd, m_FTC.m_dQCond, d1, d2);
          if (CondCnvBad)
            CHXBlock::SetCI(1, "W\tCondensate Not Converged (%7.4f kg/s)", fabs(m_FTC.m_dQRqd-m_FTC.m_dQCond));
          else
            CHXBlock::ClrCI(1);

          HotSd.m_pCd->QCopy(QMemS());
          HotSd.MeasureHXDataCd(NULL);

          double VentFlow=CondenseAll(False, false, HotSd, ColdSd);//, HotSdI, ColdSdI, False);
          // No Vent Flow
          }
        else
          {
          // Add to Vent Gas
          double VentFlow=CondenseAll(True, MixVentBack, HotSd, ColdSd);//, HotSdI, ColdSdI, True);
          }

        ColdSd.Duty=m_dDuty;
        HotSd.Duty=-m_dDuty;
        ColdSd.SatP = ColdSd.m_pCd->SaturationP(ColdSd.To);
        HotSd.SatP = HotSd.m_pCd->SaturationP(HotSd.To);

        m_dLMTD=LMTDSat(ColdSd.Ti, ColdSd.To, HotSd.SatT);
        // Temperature Crossover Error
        CHXBlock::SetCI(3, m_dLMTD<0.0);

        if (m_dLMTD<0.0)
          m_FTC.m_dQRqd=1.0e-20;

        m_FTC.m_dMinSatPress=HotSd.m_pCd->SaturationP(CoCurrent ? HotSd.Ti : ColdSd.Ti);

        HotSd.AddByPassLiquor();
        break;
        }
      default:
        break;
      }
    m_dTheoryDuty=EffUA()*m_dLMTD;
    }
  else
    {
    m_iActOpMode=HX1_Inoperative;
    m_Pri.To=m_Pri.Ti;
    m_Pri.Ho=m_Pri.Hi;
    m_Sec.To=m_Sec.Ti;
    m_Sec.Ho=m_Sec.Hi;
    m_dLMTD=0.0;
    m_dDuty=0.0;
    m_dTheoryDuty=EffUA()*m_dLMTD;
    m_Pri.iMode=QPF_Sensible;
    m_Sec.iMode=QPF_Sensible;
    CHXBlock::ClrCI(3);
    m_FTC.m_dMinSatPress=10000.0;
    if (m_pVent)
      m_pVent->QZero();
    }

  #if dbgHeatX
  if (dbgHeater())
    {
    dbgpln("HX:         Q dHp:%14.2f      Ti     To      %s", fabs(m_dDuty), FullObjTag());
    dbgpln("   %10.4f     %14.2f   %6.2f  %6.2f(%6.2f)",m_Pri.Qm, m_Pri.Hi-m_Pri.m_pCd->totHf(), K_2_C(m_Pri.Ti), K_2_C(m_Pri.To), K_2_C(m_Pri.m_pCd->Temp()));
    dbgpln("   %10.4f     %14.2f   %6.2f  %6.2f(%6.2f)",m_Sec.Qm, m_Sec.Hi-m_Sec.m_pCd->totHf(), K_2_C(m_Sec.Ti), K_2_C(m_Sec.To), K_2_C(m_Sec.m_pCd->Temp()));
    }
  #endif

  //m_Pri.fConnected=0;
  //m_Sec.fConnected=0;
  m_QmVent=m_pVent ? m_pVent->QMass() : 0.0;
  CHXBlock::SetCI(7, NetProbalMethod() && m_pVent && Valid(m_QmVentRqd) && (m_QmVent-m_QmVentRqd)<1.0e-9);
  //CHXBlock::SetCI(7, NetProbalMethod() && m_pVent && Valid(m_QmVentRqd) && fabs(m_QmVent-m_QmVentRqd)>1.0e-9);
  }

//===========================================================================

//void CHXBlock1::TransferMakeup(FlwNode* pNd, int IoId, pSpConduit pVent)
//  {
//  if (m_FTC.Active())
//    {
//    int nF=0;
//    pFlange pF[MaxIOList];
//    double R=m_FTC.dQRqd+(pVent ? pVent->QMass() : 0.0);
//    double A=0;
//    for (int i=0; i<pNd->NoFlwIOs(); i++)
//      if (pNd->IOId_Self(i)==IoId)
//        if (pNd->IO_In(i) && pNd->IOFlange(i)->IsMakeUpAvail())
//          {
//          pF[nF]=pNd->IOFlange(i);
//          A+=pF[nF]->MakeUpAvail();
//          nF++;
//          }
//    A=GTZ(A);
//    // Distribute according to MakeUpAvail
//    for (i=0; i<nF; i++)
//      pF[i]->SetMakeUpReqd(R*Max(1.0e-9, pF[i]->MakeUpAvail()/A));
//    }
//  }
//
//===========================================================================

void CHXBlock1::EvalPressureSens(FlwNode* pNd, SpContainer & Contents, int iJoin, IDMask idMask)
  {
  // Total Energy Transfer
  double Duty=-m_dDuty*ICGetTimeInc();
  // Initial Conditions
  CSysVector &ContentsMA=Contents.MArray();
  SpMArray MassStartA(ContentsMA);
  double totH0=Contents.totHf();
//  double dtotH0=Contents.dtotEnthalpy();
  double PresStart=Contents.Press();
  double MassStart=Contents.Mass();
  double MassStartV=Contents.Mass(som_Gas);
  double MassStartL=Contents.Mass(som_SL);

  Contents.SetHoldVentExpand();

  // Vapour Sensitivity
  double QMassV=1.0;
  double DeltaFrac=Range(1.0e-6, 0.01*Max(1.0, MassStartV)/NZ(fabs(QMassV)), 1.0);
  Contents.AddM(Contents, som_Gas, DeltaFrac);
  double PSensV=(Contents.Press()-PresStart)/DeltaFrac;
  RestoreContents(Contents, MassStartA, totH0);

  // Liquid Sensitivity
  double QMassL=1.0;
  DeltaFrac=Range(1.0e-6, 0.01*Max(1.0, MassStartL)/NZ(fabs(QMassL)), 1.0);
  Contents.AddM(Contents, som_SL, DeltaFrac);
  double PSensL=(Contents.Press()-PresStart)/DeltaFrac;
  RestoreContents(Contents, MassStartA, totH0);
  // Set them
  ////TODO Replace with SetVLSensitivities(...)
  //  pNd->Set_JoinP_SensVL(iJoin, PSensV, PSensL);
  for (int i=0; i<pNd->NoFlwIOs(); i++)
    if (pNd->IOIdMask_Self(i)&idMask)
      pNd->Set_IO_PSensVL(i, PSensV, PSensL);

  // Flash to Equilibrium
  if (m_VLE.Enabled())
    {
    // Transfer due to movement toward equil
    double MV0,MV1,DeltaMass;
    MV0=Contents.Mass(som_Gas);

    m_VLE.QVFlash(Contents, Duty, VLEF_Null);
    MV1=Contents.Mass(som_Gas);
    m_dV2LQ=(MV0-MV1)/ICGetTimeInc();

    pNd->SetJoinV2LFlow(iJoin, m_dV2LQ);//, V2LQVFrac, V2LQLFrac);
    dbgpln("HXSens V:%14.6g L:%14.6g V2L:%14.6g %s", PSensV, PSensL, m_dV2LQ, FullObjTag());

    SpMArray MassEquilA(ContentsMA);
    double totHA=Contents.totHf();
//    double dtotH0=Contents.dtotEnthalpy();
    double PresEquil=Contents.Press();
    double MassEquil=Contents.Mass();
    double MassEquilV=Contents.Mass(som_Gas);
    double MassEquilL=Contents.Mass(som_SL);

    CJoinRec & J=pNd->Joins[iJoin];
    for (int c=0; c<J.NConns(); c++)
      {
      int i=J.IONo(c);
      SpConduit &Cd=*pNd->IOConduit(i);

      RestoreContents(Contents, MassEquilA, totHA);
      // Transfer to retain equil due to movement of vapour
      DeltaMass=Max(1.0e-6, 0.01*MassEquilV);
      Contents.AddM(Cd, som_Gas, DeltaMass);
      MV0=Contents.Mass(som_Gas);
      m_VLE.QVFlash(Contents, 0.0, VLEF_Null);
      MV1=Contents.Mass(som_Gas);
      m_dV2LQVFrac=(MV0-MV1)/DeltaMass;
//      RestoreContents(Contents, MassEquilA, totHA);

      // Transfer to retain equil due to movement of liquor
      RestoreContents(Contents, MassEquilA, totHA);
      DeltaMass=Max(1.0e-6, 0.01*MassEquilL);
      Contents.AddM(Cd, som_SL, DeltaMass);
      MV0=Contents.Mass(som_Gas);
      m_VLE.QVFlash(Contents, 0.0, VLEF_Null);
      MV1=Contents.Mass(som_Gas);
      m_dV2LQLFrac=(MV0-MV1)/DeltaMass;
//    RestoreContents(MassEquilA, totH0);

      pNd->Set_IO_V2LQmFact(i, m_dV2LQVFrac, m_dV2LQLFrac);

      dbgpln("                                                         "
             "V2LV:%14.6g V2LL:%14.6g ", m_dV2LQVFrac, m_dV2LQLFrac);
      }
    // if flwnode dbgEvalPressSens...
//    dbgpln("HXSens V:%14.6g L:%14.6g V2L:%14.6g V2LV:%14.6g V2LL:%14.6g %s", PSensV, PSensL, m_dV2LQ, m_dV2LQVFrac, m_dV2LQLFrac, FullObjTag());
//    pNd->SetJoinV2LFlow(iJoin, m_dV2LQ);//, m_dV2LQVFrac, m_dV2LQLFrac);
    RestoreContents(Contents, MassStartA, totH0);
    }
  else
    {
    dbgpln("HXSens V:%14.6g L:%14.6g     %14s      %14s      %14s %s", PSensV, PSensL, "", "", "", FullObjTag());
    pNd->SetJoinV2LFlow(iJoin, 0.0);// 0.0, 0.0);
    }

  Contents.ClrHoldVentExpand();
  }

//===========================================================================

void CHXBlock1::RestoreContents(SpContainer &Contents, SpMArray & M, double H0)
  {
  Contents.MArray().Set(M, som_ALL, 1.0);
  Contents.Set_totHf(H0);
  };

//===========================================================================
//
//
//
//===========================================================================

const byte ioidPIn   = 0;
const byte ioidPOut  = 1;
const byte ioidPVent = 2;
const byte ioidSIn   = 3;
const byte ioidSOut  = 4;
const byte ioidSVent = 5;

const IDMask PriIOMask=Id_2_Mask(ioidPIn)|
                      Id_2_Mask(ioidPOut)|
                      Id_2_Mask(ioidPVent);

const IDMask SecIOMask=Id_2_Mask(ioidSIn)|
                      Id_2_Mask(ioidSOut)|
                      Id_2_Mask(ioidSVent);

static IOAreaRec HeaterIOAreaList[] =
  {{"",              "PriIn" , ioidPIn ,  LIO_In0 ,  nc_MLnk, 1, 20, IOGRP(1)|IOPipeEntry|IOApertureHoriz, 1.0f, {0,0,0,0}, NULL, &HeatXDPGroup},//, "Side[iP].I"},
   {"",              "IP"    , ioidPIn ,  LIO_In  ,  nc_MLnk, 1, 20, IOGRP(1)|IOPipeEntry|IOApertureHoriz, 1.0f, {0,0,0,0}, NULL, &HeatXDPGroup},//, "Side[iP].I"},
   {"",              "I"     , ioidPIn ,  LIO_In  ,  nc_MLnk, 1, 20, IOGRP(1)|IOPipeEntry|IOApertureHoriz, 1.0f, {0,0,0,0}, NULL, &HeatXDPGroup},//, "Side[iP].I"},
   {"",              "PriOut", ioidPOut,  LIO_Out0,  nc_MLnk, 1,  5, IOGRP(1)|IOPipeEntry|IOApertureHoriz, 0.0f, {0,0,0,0}, NULL, &HeatXDPGroup},//, "Side[iP].O"},
   {"",              "OP"    , ioidPOut,  LIO_Out ,  nc_MLnk, 1,  5, IOGRP(1)|IOPipeEntry|IOApertureHoriz, 0.0f, {0,0,0,0}, NULL, &HeatXDPGroup},//, "Side[iP].O"},
   {"",              "O"     , ioidPOut,  LIO_Out ,  nc_MLnk, 1,  5, IOGRP(1)|IOPipeEntry|IOApertureHoriz, 0.0f, {0,0,0,0}, NULL, &HeatXDPGroup},//, "Side[iP].O"},
   {"",              "PVent" , ioidPVent, LIO_Out ,  nc_MLnk, 0,  1, IOGRP(1)|IOPipeEntry|IOApertureHoriz, 1.0f, {0,0,0,0}, NULL, &HeatXDPGroup},//, "Side[iP].O"},
   {"",              "SecIn" , ioidSIn ,  LIO_In1 ,  nc_MLnk, 1, 20, IOGRP(2)|IOPipeEntry|IOApertureHoriz, 1.0f, {0,0,0,0}, NULL, &HeatXDPGroup},//, "Side[iS].I"},
   {"",              "IS"    , ioidSIn ,  LIO_In  ,  nc_MLnk, 1, 20, IOGRP(2)|IOPipeEntry|IOApertureHoriz, 1.0f, {0,0,0,0}, NULL, &HeatXDPGroup},//, "Side[iS].I"},
   {"",              "SecOut", ioidSOut,  LIO_Out1,  nc_MLnk, 1,  5, IOGRP(2)|IOPipeEntry|IOApertureHoriz, 0.0f, {0,0,0,0}, NULL, &HeatXDPGroup},//, "Side[iS].O"},
   {"",              "OS"    , ioidSOut,  LIO_Out ,  nc_MLnk, 1,  5, IOGRP(2)|IOPipeEntry|IOApertureHoriz, 0.0f, {0,0,0,0}, NULL, &HeatXDPGroup},//, "Side[iS].O"},
   {"",              "SVent" , ioidSVent, LIO_Out ,  nc_MLnk, 0,  1, IOGRP(2)|IOPipeEntry|IOApertureHoriz, 0.0f, {0,0,0,0}, NULL, &HeatXDPGroup},//, "Side[iS].O"},
   {NULL}};


static double Drw_Heater[] = {DD_Poly, -6,-6, -6,6, 6,6, 6,-6, -6,-6,
                            DD_Poly, -5,-6, -5,-3, -4,3, -3,-3, -2,3, -1,-3, 0,3,
                                     1,-3, 2,3, 3,-3, 4,3, 5,-3, 5,-6,
                            DD_End };

IMPLEMENT_MODELUNIT(HeatXchg1, "HeatXchg", "1", Drw_Heater, "HeatExchange", "HXCH", TOC_ALL|TOC_GRP_ENERGY|TOC_HEATBAL,
//                    "HX - Heat Exchanger(1)",
                    "Heat Transfer:Heat Exchanger(1)",
                    "")

HeatXchg1::HeatXchg1(pTagObjClass pClass_, pchar TagIn, TaggedObject* pAttach, TagObjAttachment eAttach) :
  MdlNode(pClass_, TagIn, pAttach, eAttach),
  m_HX(&CHXBlock1Class, "HX", this, TOA_Embedded),
  //m_HXJ(this, ioidPIn, ioidPVent, ioidSIn, ioidSVent) //KGA why PVent and SVent !!!?!?!?!?!?!!
  m_HXJ(this, ioidPIn, ioidPOut, ioidSIn, ioidSOut)
  {
  AttachClassInfo(nc_Process, HeaterIOAreaList, &HeatXDPGroup);

  m_fUsePriDP=PrjFileVerNo()>=36;
  m_fUseSecDP=PrjFileVerNo()>=36;

  m_iPriSgMd=HXSM_NoSurge;
  m_iSecSgMd=HXSM_NoSurge;

  m_pPriSg=NULL;
  m_pSecSg=NULL;
  m_pPriVent=NULL;
  m_pSecVent=NULL;
  m_pPriSgPS=NULL;
  m_pSecSgPS=NULL;
  //m_fDamped=false;

  m_MyPrjFileVerNo=-1;

  //EHX.Open(&CEHX_LossPerQmClass);

  RegisterMacroMdlNode(CMMFlashTrain::MMIOs, &typeid(HeatXchg1), ioidSIn, mmio_MODEL, &typeid(CFT_Condenser));
  }

//--------------------------------------------------------------------------

HeatXchg1::~HeatXchg1()
  {
  delete m_pPriSg;
  delete m_pSecSg;
  delete m_pPriVent;
  delete m_pSecVent;
  delete m_pPriSgPS;
  delete m_pSecSgPS;
  }

//--------------------------------------------------------------------------

static DDBValueLst SurgeMdLst[] =
  {
    {HXSM_NoSurge, "NoSurge"                },
    {HXSM_Tubes,   "ConstMass", MDD_Hidden},
    {HXSM_Tubes,   "Tubes"                  },
    {HXSM_Shell,   "VarMass",   MDD_Hidden},
    {HXSM_Shell,   "Shell"                  },
    {0}
  };

static /*const*/ char * PSrgTags[] = {"PNS", "PTb", "PSh"};
static /*const*/ char * SSrgTags[] = {"SNS", "STb", "SSh"};

void HeatXchg1::BuildDataDefn(DataDefnBlk & DDB)
  {
  if (m_MyPrjFileVerNo<46 && PrjFileVerNo()>=46)
    {
    if (m_pPriSg)
      m_pPriSg->Tag(PSrgTags[m_iPriSgMd]);
    if (m_pSecSg)
      m_pSecSg->Tag(SSrgTags[m_iSecSgMd]);
    }
  m_MyPrjFileVerNo=PrjFileVerNo();

  DDB.BeginStruct(this, NULL, NULL, DDB_NoPage);
  DDB.Text(" ");

  DDB.Byte   ("",       "PriSgMd",   DC_,    "",     xidHXPriSg,  this, isParmStopped|SetOnChange, SurgeMdLst);
  DDB.Byte   ("",       "SecSgMd",   DC_,    "",     xidHXSecSg,  this, isParmStopped|SetOnChange, SurgeMdLst);

  LPTSTR pPTg=(PrjFileVerNo()<22?"PriDP":"PriDPOn");
  LPTSTR pSTg=(PrjFileVerNo()<22?"SecDP":"SecDPOn");
  DDB.CheckBoxBtn("", pPTg,     DC_,    "",     &m_fUsePriDP,  this, isParmStopped|SetOnChange);
  DDB.CheckBoxBtn("", pSTg,     DC_,    "",     &m_fUseSecDP,  this, isParmStopped|SetOnChange);

  EHX.Add_OnOff(DDB);

  DDB.Visibility(NM_Dynamic|SM_All|HM_All);
  BuildDataDefnElevation(DDB);
  //DDB.CheckBoxBtn("Networked", "", DC_     , "",    &m_fDamped,     this, isParmStopped , DDBYesNo);
  DDB.Visibility();


  DDEF_Flags OldVis=DDB.GetVisibility();

  DDB.SetVisibility(OldVis);

  DDB.Text(" ");
  BuildDataDefnShowIOs(DDB);
  BuildDataDefnIOOpts(DDB);

  DDB.Text(" ");

  m_HX.BuildDataDefn(DDB);

  if (m_pPriSg)
    DDB.Object(m_pPriSg, this, NULL, NULL, DDB_RqdPage);
  if (m_pSecSg)
    DDB.Object(m_pSecSg, this, NULL, NULL, DDB_RqdPage);

  if (m_pPriVent)
    DDB.Object(m_pPriVent, this, NULL, NULL, DDB_RqdPage);
  if (m_pSecVent)
    DDB.Object(m_pSecVent, this, NULL, NULL, DDB_RqdPage);

  EHX.BuildDataDefn(DDB, NULL, NULL, DDB_RqdPage, 0);

  DDB.EndStruct();
  }

//---------------------------------------------------------------------------

void HeatXchg1::SetContProps(int SgMd, SpContainer *pSg, IDMask IOMask)
  {
  switch (SgMd)
    {
    case HXSM_Shell:
      m_HX.SetLevel(pSg->RLevel());
      m_HX.SetCondensableFrac(m_HX.m_VLE.CondensableFrac(pSg));
      pSg->SetClosed(true);
      pSg->SetPressAsSet(false);
      pSg->SetMixed(false);
      pSg->SetVentExpandOK();
      break;
    case HXSM_Tubes:
      pSg->SetClosed(true);
      pSg->SetPressAsSet(true);
      pSg->ChangeShape(&SpShPlugFlwClass);
      pSg->SetMixed(true);
      pSg->ClrVentExpandOK();
      pSg->dwDisplay=SPDF_ShapeNameDisplay|SPDF_ShapeData|SPDF_DynMode;
      pSg->m_bKeepLPTOnVChange=true;

      for (int i=0; i<NoFlwIOs(); i++)
        if (Id_2_Mask(IOId_Self(i)) & IOMask)
          SetIORelFracHgt_Flng(i, 0.0);
      break;
    }
  }

//---------------------------------------------------------------------------

flag HeatXchg1::DataXchg(DataChangeBlk & DCB)
  {
  if (MdlNode::DataXchg(DCB))
    return 1;

  if (m_HX.DataXchg(DCB))
    return 1;
  switch (DCB.lHandle)
    {

    case xidHXPriSg:
      if (DCB.rB)
        {
        if (m_iPriSgMd!=*DCB.rB)
          {
          delete m_pPriSg;
          delete m_pPriSgPS;
          delete m_pPriVent;
          m_pPriSg=NULL;
          m_pPriSgPS=NULL;
          m_pPriVent=NULL;
          };
        m_iPriSgMd=*DCB.rB;
        if (m_iPriSgMd!=HXSM_NoSurge)
          {
          if (!m_pPriSg)
            {
            m_pPriSg=new SpContainer(PrjFileVerNo()>= 46 ? PSrgTags[m_iPriSgMd] : "PriSg", this, TOA_Embedded);
            if (!(NetDynamicMethod() && (m_HX.m_iRqdOpMode==HX1_FullyCondensing)))
              m_pPriVent=new SpConduit("PriVent", this, TOA_Embedded);
            m_pPriSgPS=new CSpContPreset;//VLE(&m_HX.m_VLE);
            m_pPriSg->SetPreset(m_pPriSgPS, NULL);
            m_pPriSg->SetVLE(&m_HX.m_VLE);
            }
          SetContProps(m_iPriSgMd, m_pPriSg, PriIOMask);
          if (DCB.ForView() && (m_iPriSgMd==HXSM_Tubes))
            {
            SigmaQOut(m_HXJ.P.Qo(), som_ALL, Id_2_Mask(ioidPOut));
            CPlugFlowInfo PFI(this, VFFX_PostMix, m_pPriSg, NULL, false);
            PFI.InitialisePlugFlow(m_HXJ.P.Qo(),/* *m_pPriSg,*/ 1.0);
            }
          }
        }
      DCB.B=m_iPriSgMd;
      return 1;
    case xidHXSecSg:
      if (DCB.rB)
        {
        if (m_iSecSgMd!=*DCB.rB)
          {
          delete m_pSecSg;
          delete m_pSecSgPS;
          delete m_pSecVent;
          m_pSecSg=NULL;
          m_pSecSgPS=NULL;
          m_pSecVent=NULL;
          };
        m_iSecSgMd=*DCB.rB;
        if (m_iSecSgMd!=HXSM_NoSurge)
          {
          if (!m_pSecSg)
            {
            m_pSecSg=new SpContainer(PrjFileVerNo()>= 46 ? SSrgTags[m_iSecSgMd] : "SecSg", this, TOA_Embedded);
            if (!(NetDynamicMethod() && (m_HX.m_iRqdOpMode==HX1_FullyCondensing)))
              m_pSecVent=new SpConduit("SecVent", this, TOA_Embedded);
            m_pSecSgPS=new CSpContPreset;//VLE(&m_HX.m_VLE);
            m_pSecSg->SetPreset(m_pSecSgPS, NULL);
            m_pSecSg->SetVLE(&m_HX.m_VLE);
            }
          SetContProps(m_iSecSgMd, m_pSecSg, SecIOMask);
          if (DCB.ForView() && (m_iSecSgMd==HXSM_Tubes))
            {
            SigmaQOut(m_HXJ.S.Qo(), som_ALL, Id_2_Mask(ioidSOut));
            CPlugFlowInfo PFI(this, VFFX_PostMix, m_pSecSg, NULL, false);
            PFI.InitialisePlugFlow(m_HXJ.S.Qo(),/* *m_pSecSg,*/ 1.0);
            };
          }
        }
      DCB.B=m_iSecSgMd;
      return 1;
    }
//  switch (DCB.dwUserInfo)
//    {
//    case 1 : FBP.DataXchg(DCB); return 1;
//    case 2 : FBS.DataXchg(DCB); return 1;
//    }

  return 0;
  }

//---------------------------------------------------------------------------

flag HeatXchg1::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=MdlNode::ValidateData(VDB);

  if (m_iPriSgMd==HXSM_Shell && m_iSecSgMd==HXSM_Shell)
    {
    OK=false;
    LogNote(Tag(), 0, "Two Shells are not permitted");
    }

//  Contents.
  //bool TAdj=!Shell.Closed() || !m_VLE.Enabled();
  m_HX.m_bLvlEffOn=(m_iPriSgMd==HXSM_Shell || m_iSecSgMd==HXSM_Shell);

  SetContProps(m_iPriSgMd, m_pPriSg, PriIOMask);
  if (m_iPriSgMd!=HXSM_NoSurge && !(NetDynamicMethod() && (m_HX.m_iRqdOpMode==HX1_FullyCondensing)))
    {
    if (!m_pPriVent)
      m_pPriVent=new SpConduit("PriVent", this, TOA_Embedded);
    }
  else
    {
    delete m_pPriVent;
    m_pPriVent=NULL;
    }

  SetContProps(m_iSecSgMd, m_pSecSg, SecIOMask);

  if (m_iSecSgMd!=HXSM_NoSurge && !(NetDynamicMethod() && (m_HX.m_iRqdOpMode==HX1_FullyCondensing)))
    {
    if (!m_pSecVent)
      m_pSecVent=new SpConduit("SecVent", this, TOA_Embedded);
    }
  else
    {
    delete m_pSecVent;
    m_pSecVent=NULL;
    }

  for (int i=0; i<NoFlwIOs(); i++)
    {
    flag On=(IOId_Self(i)>=ioidPIn && IOId_Self(i)<=ioidPVent) ? m_fUsePriDP : m_fUseSecDP;
    CFlwEqnGrp * pGrp= (On ? &HeatXDPGroup : &NullFlwGroup);
    IOFB(i,0)->AssignFlwEqnGroup(*pGrp, pGrp->Default(), this);
    }

  return OK;
  }

//--------------------------------------------------------------------------

flag HeatXchg1::InitialiseSolution()
  {
  flag OK=MdlNode::InitialiseSolution();

  switch (SolveMethod())
    {
    case SM_Direct:
      if (m_pPriSg)
        m_pPriSg->SetStateAction(IE_Disabled);
      if (m_pSecSg)
        m_pSecSg->SetStateAction(IE_Disabled);
      break;
      if (m_pPriSg)
        m_pPriSg->SetStateAction(IE_SaveState);
      if (m_pSecSg)
        m_pSecSg->SetStateAction(IE_SaveState);
      break;
    case SM_Inline:
    case SM_Buffered:
      if (m_pPriSg)
        m_pPriSg->SetStateAction(m_iPriSgMd!=HXSM_NoSurge ? IE_Integrate : IE_SaveState);
      if (m_pSecSg)
        m_pSecSg->SetStateAction(m_iSecSgMd!=HXSM_NoSurge ? IE_Integrate : IE_SaveState);
      break;
    }


  flag BadPrimaries=False;
  flag BadSecondaries=False;
  switch (SolveMethod())
    {
    case SM_Direct:
      {
      if (NIOsWithId_Self(ioidPOut)>1)
        BadPrimaries=True;
      if (NIOsWithId_Self(ioidSOut)>1)
        BadSecondaries=True;
      break;
      }
    case SM_Inline:
    case SM_Buffered:
      {
      if (m_iPriSgMd!=HXSM_Shell)
        if (NIOsWithId_Self(ioidPOut)>1)
          BadPrimaries=True;
      if (m_iSecSgMd!=HXSM_Shell)
        if (NIOsWithId_Self(ioidSOut)>1)
          BadSecondaries=True;
      break;
      }
    }
  if (BadPrimaries)
    LogError(FullObjTag(), 0, "Too many connections to Primary side");
  if (BadSecondaries)
    LogError(FullObjTag(), 0, "Too many connections to Secondary side");

  return OK && !BadPrimaries && !BadSecondaries;
  }

//---------------------------------------------------------------------------

void HeatXchg1::StartSolution()
  {
//  FBP.StartSolution();
//  FBS.StartSolution();
  if (m_pPriVent)
    m_pPriVent->QSetTraceMass(); // QZero();
  if (m_pSecVent)
    m_pSecVent->QSetTraceMass(); // QZero();
  }

//--------------------------------------------------------------------------

static const IDMask PriMask=Id_2_Mask(ioidPIn)|Id_2_Mask(ioidPOut)|Id_2_Mask(ioidPVent);
static const IDMask SecMask=Id_2_Mask(ioidSIn)|Id_2_Mask(ioidSOut)|Id_2_Mask(ioidSVent);

void HeatXchg1::SetDatums(int Pass, CFlwNodeIndexList & List, int IOIn)
  {
  CSetDatumsData SDD[]=
    {
      {PriMask, m_pPriSg},
      {SecMask, m_pSecSg},
      {0}
    };

  SetDatums_Node(Pass, List, IOIn, SDD);
  };

//--------------------------------------------------------------------------

void HeatXchg1::SetDatumsDone()
  {
  MdlNode::SetDatumsDone();

  SortSurgeIOData SDD[]=
    {
      {PriMask, m_pPriSg, &m_PriHgtOrd},
      {SecMask, m_pSecSg, &m_SecHgtOrd},
      {0 },
    };
  SortSurgeIO(SDD);
  };

//--------------------------------------------------------------------------

flag HeatXchg1::Set_Sizes()
  {
  return True;
  };

//--------------------------------------------------------------------------

void   HeatXchg1::SetState(eScdMdlStateActs RqdState)
  {
  MdlNode::SetState(RqdState);
  int s;
  switch (RqdState)
    {
    case MSA_PBInit:
      m_HX.m_FTC.SetState(RqdState);
      break;
    case MSA_Empty:
      for (s=0; s<2; s++)
        {
        SpContainer * p=(s==0) ? m_pPriSg : m_pSecSg;
        if (p)
          p->SetState(RqdState);
        }
      break;
    case MSA_PreSet:
      for (s=0; s<2; s++)
        {
        SpContainer * p=(s==0) ? m_pPriSg : m_pSecSg;
        if (p)
          p->SetState(RqdState);
        }
      break;
    case MSA_SteadyState:
      LogNote(FullObjTag(), 0, "SteadyState Undefined");
      break;
    }
  };

//---------------------------------------------------------------------------

bool HeatXchg1::PropagateNetInfo(CPropagateNetInfoCtrl & Ctrl, long IONo)
  {
  if (!MdlNode::DoPropagateNetInfo(Ctrl, IONo, false))
    return false;

  IDMask IOInMsk=IONo>=0 ? Id_2_Mask(IOId_Self(IONo)) : First64IOIds;
  for (int i=0; i<NoFlwIOs(); i++)
    {
    if ((PriMask & IOInMsk & Id_2_Mask(IOId_Self(i)))!=0 ||
        (SecMask & IOInMsk & Id_2_Mask(IOId_Self(i)))!=0)
      Nd_Rmt(i)->PropagateNetInfo(Ctrl, IOIONo_Rmt(i));
    }

  return true;
  };

//--------------------------------------------------------------------------

void HeatXchg1::PostConnect(int IONo)
  {
  MdlNode::PostConnect(IONo);
  };

//---------------------------------------------------------------------------

void HeatXchg1::ConfigureJoins()
  {
  //m_HXJ.P.JoinId=-1;
  //m_HXJ.S.JoinId=-1;
  switch (NetMethod())
    {
    case NM_Probal:
      {
      //flag fP=m_HXJ.PriIsJoin();
      Set_NoJoins(2);
      m_HXJ.SetJoinIds(0, True, True);
      //flag fP=m_HXJ.P.IsConnected();
      //flag fS=m_HXJ.S.IsConnected();
      for (int i=0; i<NoProcessIOs(); i++)
        if (IOId_Self(i)>=ioidPIn && IOId_Self(i)<=ioidPOut)
          if (m_HXJ.PriIsJoined())
            SetIO_Join(i, m_HXJ.P.JoinId());
          else
            SetIO_Closed(i, m_HXJ.P.JoinId());

      for (i=0; i<NoProcessIOs(); i++)
        if (IOId_Self(i)>=ioidSIn && IOId_Self(i)<=ioidSOut)
          {
          // if Fully Condensing then try to prevent Tears
          IOFlange(i)->SetTearPriority((1 && (IOId_Self(i)==ioidSIn)) ? TP_Last : TP_Normal);
          if (m_HXJ.SecIsJoined())
            SetIO_Join(i, m_HXJ.S.JoinId());
          else
            SetIO_Closed(i, m_HXJ.S.JoinId());
          }
      break;
      }

    case NM_Dynamic:
      {
      if (fDoDbgBrk)
        { int xxx=0; };
      Set_NoJoins(2);
      m_HXJ.SetJoinIds(0, m_iPriSgMd!=HXSM_Shell, m_iSecSgMd!=HXSM_Tubes);

      if (m_iPriSgMd==HXSM_Shell)
        {
        for (int i=0; i<NoProcessIOs(); i++)
          if (m_HXJ.PriIO(i))
            SetIO_Open(i, m_HXJ.PriJoinId(), /*m_fDamped*/m_pPriSg->NetPressDamp(),
                          SolveInlineMethod() ? ESS_Denied : ESS_Allowed);
        }
      else
        {
        for (int i=0; i<NoProcessIOs(); i++)
          if (m_HXJ.PriIO(i))
            if (m_HXJ.PriIsJoined())
              SetIO_Join(i, m_HXJ.P.JoinId(),
                         m_iPriSgMd==HXSM_Tubes ? ESS_Allowed : ESS_Denied);
            else
              SetIO_Closed(i, m_HXJ.P.JoinId());
        }

      if (m_iSecSgMd==HXSM_Shell)
        {
        for (int i=0; i<NoProcessIOs(); i++)
          if (m_HXJ.SecIO(i))
            SetIO_Open(i, m_HXJ.SecJoinId(), /*m_fDamped*/m_pSecSg->NetPressDamp(),
                       SolveInlineMethod() ? ESS_Denied : ESS_Allowed);
        }
      else
        {
        for (int i=0; i<NoProcessIOs(); i++)
          if (m_HXJ.SecIO(i))
            if (m_HXJ.SecIsJoined())
              SetIO_Join(i, m_HXJ.S.JoinId(),
                         m_iSecSgMd==HXSM_Tubes ? ESS_Allowed : ESS_Denied);
            else
              SetIO_Closed(i, m_HXJ.S.JoinId());
        }
      break;
      }
    }
  }

//--------------------------------------------------------------------------

void HeatXchg1::StartStep()
  {
  if (m_pPriSg)
    m_PriStart.AtStart(*m_pPriSg);
  if (m_pSecSg)
    m_SecStart.AtStart(*m_pSecSg);
  }

//--------------------------------------------------------------------------

void HeatXchg1::EvalJoinPressures(long JoinMask)
  {
  switch (NetMethod())
    {
    case NM_Probal:
      {
      for (int j=0; j<NoProcessJoins(); j++)
        {
        double Pj=GetPBInputPressure(j);

        if (j==1 && m_HX.m_FTC.Active())
          m_HX.m_FTC.SetSuctionP(Pj);

        SetPBJoinPressure(j, Pj, true, true);
        #if dbgFlwNode
        if (dbgEvalJoinPress(sTag()))
          dbgpln("EJP: %12.2f  %s[%i]", Pm, sTag(), j);
        #endif
        }
      }
      break;
    default:;
      {
      #if dbgEvalPres
      dbgpln("EvalJoinPressures:%s",FullObjTag());
      #endif
      if (fDoDbgBrk)
        { int xxx=0; }
      if (m_iPriSgMd==HXSM_Shell)
        {
        IOP_RhoH_Info RhoHInfo(*m_pPriSg);
        double Pm=ContainerMeanPress(RhoHInfo, 0.0);
        Set_JoinP(m_HXJ.PriJoinId(), Pm);
        for (int i=0; i<NoFlwIOs(); i++)
          if (IOId_Self(i)>=ioidPIn && IOId_Self(i)<=ioidPVent)
            Set_IOP_RhoH_Self(i,Pm,RhoHInfo);
        }
      else
        {
        double Pm=JoinP(m_HXJ.PriJoinId());
        for (int i=0; i<NoFlwIOs(); i++)
          if (IOId_Self(i)>=ioidPIn && IOId_Self(i)<=ioidPVent)
            Set_IOP_RhoH_Self(i,Pm,1.0,0.0);
        }

      if (m_iSecSgMd==HXSM_Shell)
        {
        IOP_RhoH_Info RhoHInfo(*m_pSecSg);
        double Pm=ContainerMeanPress(RhoHInfo, 0.0);
        Set_JoinP(m_HXJ.SecJoinId(), Pm);
        for (int i=0; i<NoFlwIOs(); i++)
          if (IOId_Self(i)>=ioidSIn && IOId_Self(i)<=ioidSVent)
            Set_IOP_RhoH_Self(i,Pm,RhoHInfo);
        }
      else
        {
        double Pm=JoinP(m_HXJ.SecJoinId());
        for (int i=0; i<NoFlwIOs(); i++)
          if (IOId_Self(i)>=ioidSIn && IOId_Self(i)<=ioidSVent)
            Set_IOP_RhoH_Self(i,Pm,1.0,0.0);
        }

      break;
      }
    }
  };

//--------------------------------------------------------------------------

CSpPropInfo* HeatXchg1::IOGetNetProps(int i, double Qm)
  {
  DoIOGetNetProps_Surge(i, Qm);
  return IONetProps(i);
  }

//--------------------------------------------------------------------------

flag HeatXchg1::EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo)
  {
  return IOFB(IONo,FE)->EvaluateFlwEqn(Task, pProps, true, false, 1.0, &IOFBFlng_Rmt(IONo)->PhD(), NULL);
  };

//--------------------------------------------------------------------------

void HeatXchg1::EvalProductsSurge(CNodeEvalIndex & NEI)
  {
  if (SolveInlineMethod())
    return;

  HeatXchg1::EvalProductsDyn(true, false);
  }

//--------------------------------------------------------------------------

void HeatXchg1::EvalProducts(CNodeEvalIndex & NEI)
  {
  switch (SolveMethod())
    {
    case SM_Direct:
      {
      if (m_HXJ.PriIsJoined() && IOWithId_Self(ioidPVent)>=0 && m_pPriVent==NULL)
        m_pPriVent=new SpConduit("PriVent", this, TOA_Embedded);
      if (m_HXJ.SecIsJoined() && IOWithId_Self(ioidSVent)>=0 && m_pSecVent==NULL)
        m_pSecVent=new SpConduit("SecVent", this, TOA_Embedded);
      m_HX.SetCI(6, m_pPriVent && m_pSecVent); //m_HX DOES NOT SUPPORT TWO VENTS!!!

      m_HX.ClrVent();
      double PriPIn=0.0;
      double SecPIn=0.0;
      if (m_HXJ.PriIsJoined() && m_HXJ.PriConnected())
        {
        SpConduit & CdOut=*IOConduit(m_HXJ.P.IOFlwOut());
        PriPIn=SigmaQInPMin(CdOut, som_ALL, ioidPIn, ioidPIn);//Out);
        m_HX.SetPrimary(NULL, CdOut, PriPIn);
        }
      else
        m_HX.ClrPrimary();
      if (m_pPriVent)
        m_HX.SetVent(*m_pPriVent, PriPIn);

      if (m_HXJ.SecIsJoined() && m_HXJ.SecConnected())
        {
        SpConduit & CdOut=*IOConduit(m_HXJ.S.IOFlwOut());
        SecPIn=SigmaQInPMin(CdOut, som_ALL, ioidSIn, ioidSIn);//som_Gas);
        m_HX.SetSecondary(NULL, CdOut, SecPIn, EHX());
        }
      else
        m_HX.ClrSecondary();
      if (m_pSecVent)
        m_HX.SetVent(*m_pSecVent, SecPIn);

      m_HX.EvalProducts(this, false);

      if (m_HXJ.PriIsJoined() && m_pPriVent)
        {
        // Copy VentConduit to Output if present
        const int idVent=IOWithId_Self(ioidPVent);
        if (idVent>=0)
          IOConduit(idVent)->QSetF(*m_pPriVent, som_ALL, 1.0, PriPIn);
        }
      if (m_HXJ.SecIsJoined() && m_pSecVent)
        {
        // Copy VentConduit to Output if present
        const int idVent=IOWithId_Self(ioidSVent);
        if (idVent>=0)
          IOConduit(idVent)->QSetF(*m_pSecVent, som_ALL, 1.0, SecPIn);
        }
      }
      break;
    case SM_Inline:
    case SM_Buffered:
      {
      HeatXchg1::EvalProductsDyn(SolveInlineMethod()!=0, true);
      }
      break;
    }
  }

//--------------------------------------------------------------------------

void HeatXchg1::EvalProductsDyn(bool DoSurge, bool DoXferOut)
  {
  m_HX.m_iActOpMode=HX1_FullyCondensing;

  if (fDoDbgBrk)
    { int xxx=0; };

  double PressInP=Std_P;
  double PressInS=Std_P;
  if (m_iPriSgMd==HXSM_Shell)
    {
    if (DoSurge)
      {
      m_HX.SetLevel(m_pPriSg->RLevel());
      m_HX.SetCondensableFrac(m_HX.m_VLE.CondensableFrac(m_pPriSg));
      EvalProducts_SurgeLevel(m_PriHgtOrd, NULL);//m_pPriSg);
      }
    else
      SigmaQInPMin(m_HXJ.P.Qi(), som_ALL, PriIOMask);
    }
  else if (m_HXJ.PriIsJoined())
    {
    //if (m_iPriSgMd==HXSM_Tubes)
    if (m_iPriSgMd==HXSM_Tubes && m_HXJ.PriConnected()) //kga:31/10/01 added PriConnected to try prevent crash because m_HXJ.P.IOFlwOut() could be -1 !
      {
      if (DoSurge)
        {
        double P=JoinP(m_HXJ.PriJoinId());
        double Q=IOQmEst_Out(m_HXJ.P.IOFlwOut());
        CPlugFlowInfo PFI(this, VFFX_PostMix, m_pPriSg, NULL, false);
        PFI.SetPlugFlowOut(*IOConduit(m_HXJ.P.IOFlwOut()), /**m_pPriSg,*/ GEZ(Q), P);
        }
      else
        {
        SigmaQInPMin(m_HXJ.P.Qi(), som_ALL, PriIOMask);
        //DoPlugFlowIn(SpConduit & Fi, SpContainer & Cn, double QmIn);
        //DoPlugFlowIn(
        m_HXJ.P.Qo().QCopy(m_HXJ.P.Qi());
        }
      }
    else
      {
//#if NewXferEval
      PressInP=Joins[m_HXJ.P.JoinId()].Pressure();
      Xfer_EvalProductsIn_(m_HXJ.P.JoinId(), PressInP, &m_HXJ.P.Qi(), &m_HXJ.P.Qo(), NULL);
      Xfer_EvalProductsWork_(&m_HXJ.P.Qi(), &m_HXJ.P.Qo(), NULL, NULL);
//#else
//      PressInP=Xfer_EvalProductsIn(m_HXJ.P.JoinId(), Joins[m_HXJ.P.JoinId()].Pressure(),
//                                   &m_HXJ.P.Qi(), &m_HXJ.P.Qo(), NULL, NULL, NULL);
//#endif
      }
    }
  if (m_iSecSgMd==HXSM_Shell)
    {
    if (DoSurge)
      {
      m_HX.SetLevel(m_pSecSg->RLevel());
      m_HX.SetCondensableFrac(m_HX.m_VLE.CondensableFrac(m_pSecSg));
      EvalProducts_SurgeLevel(m_SecHgtOrd, NULL);//m_pSecSg);
      }
    else
      SigmaQInPMin(m_HXJ.S.Qi(), som_ALL, SecIOMask);
    }
  else if (m_HXJ.SecIsJoined())
    {
    if (m_iSecSgMd==HXSM_Tubes)
      {
      if (DoSurge)
        {
        double P=JoinP(m_HXJ.SecJoinId());
        if (m_HXJ.S.IOFlwOut()>=0)
          {
          double Q=IOQmEst_Out(m_HXJ.S.IOFlwOut());
          CPlugFlowInfo PFI(this, VFFX_PostMix, m_pSecSg, NULL, false);
          PFI.SetPlugFlowOut(*IOConduit(m_HXJ.S.IOFlwOut()),/* *m_pSecSg,*/ Q, P);
          }
        else
          {
          //TODO What goes here - new bug due to changes to IOs -> CArray.
          }
        }
      else
        {
        SigmaQInPMin(m_HXJ.S.Qi(), som_ALL, SecIOMask);
        m_HXJ.S.Qo().QCopy(m_HXJ.S.Qi());
        }
      }
    else
      {
//#if NewXferEval
      PressInS=Joins[m_HXJ.S.JoinId()].Pressure();
      Xfer_EvalProductsIn_(m_HXJ.S.JoinId(), PressInS, &m_HXJ.S.Qi(), &m_HXJ.S.Qo(), NULL);
      Xfer_EvalProductsWork_(&m_HXJ.S.Qi(), &m_HXJ.S.Qo(), NULL, NULL);
//#else
//      PressInS=Xfer_EvalProductsIn(m_HXJ.S.JoinId(), Joins[m_HXJ.S.JoinId()].Pressure(),
//                                   &m_HXJ.S.Qi(), &m_HXJ.S.Qo(), NULL, NULL, NULL);
//#endif
      }
    }

  if (m_iPriSgMd==HXSM_Shell)
    m_HX.SetPrimary(&m_HXJ.P.Qi(), *m_pPriSg, som_Gas, 1.0, m_pPriSg->Press(), &m_HXJ.P.Qi());
  else if (m_HXJ.PriIsJoined() && m_HXJ.PriConnected())
    m_HX.SetPrimary(&m_HXJ.P.Qi(), m_HXJ.P.Qo(), PressInP);
  else
    m_HX.ClrPrimary();

  if (m_iSecSgMd==HXSM_Shell)
    m_HX.SetSecondary(&m_HXJ.S.Qi(), *m_pSecSg, som_Gas, 1.0, m_pSecSg->Press(), &m_HXJ.S.Qi());
  else if (m_HXJ.SecIsJoined() && m_HXJ.SecConnected())
    m_HX.SetSecondary(&m_HXJ.S.Qi(), m_HXJ.S.Qo(), PressInS);
  else
    m_HX.ClrSecondary();

  if (m_iPriSgMd>HXSM_NoSurge && 
      m_iSecSgMd>HXSM_NoSurge &&
      m_HX.m_iRqdOpMode==HX1_FullyCondensing)
    {
    if (DoSurge)
      m_HX.EvalProductsDynFCSurge(this, m_iPriSgMd==HXSM_Shell);
    else
      m_HX.EvalProductsDynFCInline(this);
    }
  else
    {
    m_HX.EvalProducts(this, (m_iPriSgMd==HXSM_NoSurge && m_iSecSgMd==HXSM_NoSurge));
    }

  if(DoXferOut)
    {
    if (m_iPriSgMd==HXSM_Shell)
      {
      }
    else if (m_HXJ.PriIsJoined())
      {
      if (m_iPriSgMd==HXSM_Tubes)
        {
        int xxx=0;
        }
      else
        {
//#if NewXferEval
        Xfer_EvalProductsOut_(m_HXJ.P.JoinId(), &m_HXJ.P.Qi(), &m_HXJ.P.Qo(), NULL, NULL);
//#else
//        Xfer_EvalProductsOut(m_HXJ.P.JoinId(), Joins[m_HXJ.P.JoinId()].Pressure(),
//                             NULL/*&m_HXJ.P.Qi()*/, &m_HXJ.P.Qo(), NULL, NULL, NULL);
//#endif
        }
      }

   if (m_iSecSgMd==HXSM_Shell)
      {
      }
    else if (m_HXJ.SecIsJoined())
      {
      if (m_iSecSgMd==HXSM_Tubes)
        {
        int xxx=0;
        }
      else
        {
//#if NewXferEval
        Xfer_EvalProductsOut_(m_HXJ.S.JoinId(), &m_HXJ.S.Qi(), &m_HXJ.S.Qo(), NULL, NULL);
//#else
//        Xfer_EvalProductsOut(m_HXJ.S.JoinId(), Joins[m_HXJ.S.JoinId()].Pressure(),
//                             NULL/*&m_HXJ.S.Qi()*/, &m_HXJ.S.Qo(), NULL, NULL, NULL);
//#endif
        }
      }
    }
  }

//--------------------------------------------------------------------------

void HeatXchg1::EvalDerivs(CNodeEvalIndex & NEI)
  {
  if (IntegralDone())
    {
    if (m_pPriSg)
      m_pPriSg->ZeroDeriv();
    if (m_pSecSg)
      m_pSecSg->ZeroDeriv();
    }
  else
    {
    int PriIsShell = (m_iPriSgMd==HXSM_Shell);
    int SecIsShell = (m_iSecSgMd==HXSM_Shell);
    int CondOn=(m_HX.m_iRqdOpMode==HX1_FullyCondensing);

    //SetIntegralDone(false);
    for (int s=0; s<2; s++)
      {
      SpContainer * pA[]={NULL, (s==0) ?  m_pPriSg : m_pSecSg, (s==0) ?  m_pPriSg : m_pSecSg};
      SpContainer * m_pCn=pA[(s==0) ? m_iPriSgMd : m_iSecSgMd];
      int ShellStuff=(((s==0) ? m_iPriSgMd : m_iSecSgMd)==HXSM_Shell);
      if (m_pCn)
        {
        m_pCn->ZeroDeriv();
        for (int i = 0; i < NoFlwIOs(); i++)
          if (IOIdMask_Self(i) & ((s==0)?PriIOMask:SecIOMask))
            {

            SpConduit & Cd=*IOConduit(i);
            double FracAbove=(IOAbsFracHgt_Term(i, *m_pCn)-m_pCn->Level())/IOAperture(i);
            FracAbove=Range(0.0, FracAbove, 1.0);
            double EntR=FracAbove*IOEntrainRateA(i)*Cd.QMass(som_SL)+
                        (1.0-FracAbove)*IOEntrainRateB(i)*Cd.QMass(som_Gas);
            if (CondOn && (IOSign(i)>0))
              m_HX.m_VLE.QPFlash(Cd, m_pCn->Press(), 0.0, VLEF_Null);

            m_pCn->AddDeriv(Cd, IOSign(i), EntR);
            }
        //m_pCn->AddInFlowDeriv();

        if (m_pCn->SpillExists())
          m_pCn->AddDeriv(m_pCn->Spill.Cd, -1.0);
        if (m_pCn->VentExists())
          m_pCn->AddDeriv(m_pCn->Vent.Cd, -1.0);
        }

      }

    //if (m_pPriSg && m_pSecSg)
    if (m_pPriSg && m_pSecSg && m_HX.m_Pri.m_pCdIn) //kga 31/10/01 trying to fix crash
      {
      if (fDoDbgBrk)
        { int xxx=0; };

      CHXSide & P=m_HX.m_Pri;
      CHXSide & S=m_HX.m_Sec;

      P.Ti=P.m_pCdIn->Temp();
      P.To=m_pPriSg->Temp();
      S.Ti=S.m_pCdIn->Temp();
      S.To=m_pSecSg->Temp();

      int DutySgn=1;
      if (PriIsShell)
        {
        //P.Ti=P.To;
        m_HX.m_dLMTD=CHXBlock::LMTDSat1(S.Ti, S.To, P.To);
        DutySgn=(S.Ti<P.To) ? -1 : 1;
        }
      else if (SecIsShell)
        {
        //S.Ti=S.To;
        m_HX.m_dLMTD=CHXBlock::LMTDSat1(P.Ti, P.To, S.To);
        DutySgn=(P.Ti<S.To) ? 1 : -1;
        }
      else
        {
        m_HX.m_dLMTD=CHXBlock::LMTD(false, P.Ti, P.To, S.Ti, S.To);
        DutySgn=(S.Ti<P.Ti) ? -1 : 1;
        }

      m_HX.m_dDuty=DutySgn*m_HX.EffUA() * m_HX.m_dLMTD * m_HX.m_dLMTDFactor;

      #if dbgHeatX
      if (dbgDerivs() && fDoDbgBrk)
        {
        dbgpln("HXDerivs:Pri  Ti:%13.6f To:%13.6f M:%13.6f %s", P.Ti, P.To, m_pPriSg->Mass(), FullObjTag());
        dbgpln("         Sec  Ti:%13.6f To:%13.6f M:%13.6f   ", S.Ti, S.To, m_pSecSg->Mass());
        dbgpln("         Duty DT:%13.6f A :%13.6f = %13.6f   ", m_HX.m_dLMTD, m_HX.EffUA(), m_HX.m_dDuty);
        }
      #endif


      P.Duty=m_HX.m_dDuty;
      S.Duty=-m_HX.m_dDuty;

      m_pPriSg->SetDuty(P.Duty);
      m_pSecSg->SetDuty(S.Duty);
      }
    else
      {
      //m_HX.m_dDuty=DutySgn*m_HX.EffUA() * m_HX.m_dLMTD * m_HX.m_dLMTDFactor;
      //m_HX.m_Pri.Duty=m_HX.m_dDuty;
      //m_HX.m_Sec.Duty=-m_HX.m_dDuty;

      if (m_pPriSg)
        m_pPriSg->SetDuty(m_HX.m_Pri.Duty);
      if (m_pSecSg)
        m_pSecSg->SetDuty(m_HX.m_Sec.Duty);
      }
    }
  if (GetActiveHold())
    {
    if (m_pPriSg)
      m_pPriSg->ZeroDeriv();
    if (m_pSecSg)
      m_pSecSg->ZeroDeriv();
    }
  };

//--------------------------------------------------------------------------
/**
void HeatXchg1::EvalIntegral(CNodeEvalIndex & NEI)
  {

  int PriIsShell = (m_iPriSgMd==HXSM_Shell);
  int SecIsShell = (m_iSecSgMd==HXSM_Shell);

  if (NetDynamicMethod() &&
      (PriIsShell || SecIsShell) &&
      (m_HX.m_iRqdOpMode==HX1_FullyCondensing))
    {

    // All IOs are now calculated
    SpContainer &Shell = PriIsShell ? *m_pPriSg:*m_pSecSg;
    dword ShellIO = PriIsShell ? PriIOMask:SecIOMask;

    Shell.ZeroDeriv();
    for (int i = 0; i < NoFlwIOs(); i++)
      if (IOIdMask_Self(i) & ShellIO)
      {
      SpConduit & Cd=*IOConduit(i);
      double FracAbove=(IOAbsFracHgt_Term(i, Shell)-Shell.Level())/IOAperture(i);
      FracAbove=Range(0.0, FracAbove, 1.0);
      double EntR=FracAbove*IOEntrainRateA(i)*Cd.QMass(som_SL)+
                  (1.0-FracAbove)*IOEntrainRateB(i)*Cd.QMass(som_Gas);
      Shell.AddDeriv(Cd, IOSign(i), EntR);
      }
    Shell.AddInFlowDeriv();

    if (Shell.SpillExists())
      Shell.AddDeriv(Shell.OverFlow(), -1.0);
    if (Shell.VentExists())
      Shell.AddDeriv(Shell.GasVent(), -1.0);

    Shell.AddInternalDerivs(m_HX.m_SrgDM.pMArray(), m_HX.m_dSrgDH);

    Shell.ApplyDerivs(ICGetTimeInc(), true);

    m_HX.m_VLE.QVFlash(Shell, 0.0, VLEF_Null);

    SetIntegralDone(true);
    }
  else
    SetIntegralDone(false);
  }
**/
//--------------------------------------------------------------------------

void HeatXchg1::ODEOperate(CODEDataBlock & ODB)
  {
  switch (ODB.m_Cmd)
    {
    case eStateAdvance:
    case eStateConverge:
    case eStateLoad:
    case eStateDiskLoad:
      {
      //if (m_pPriSg)
      //  m_pPriSg->ODEOperate(ODB);
      //if (m_pSecSg)
      //  m_pSecSg->ODEOperate(ODB);
      //
      //ConvergeStates();

      if (m_pPriSg)
        {
        #if dbgHeatX
        if (dbgODEOperate() && fDoDbgBrk)
          dbgpln("ODE:---  : T:%+13.6f = %13.6f P:%+13.6f = %13.6f M:%+13.6f = %13.6f L:%+13.6f = %13.6f %s.Pri",
            m_PriStart.DTemp(*m_pPriSg), m_pPriSg->Temp(),
            m_PriStart.DPress(*m_pPriSg), m_pPriSg->Press(),
            m_PriStart.DMass(*m_pPriSg), m_pPriSg->Mass(),
            m_PriStart.DLevel(*m_pPriSg)*100, m_pPriSg->Level()*100, FullObjTag());
        #endif

        m_pPriSg->ODEOperate(ODB);

        #if dbgHeatX
        if (dbgODEOperate() && fDoDbgBrk)
          dbgpln("ODE:Int  : T:%+13.6f = %13.6f P:%+13.6f = %13.6f M:%+13.6f = %13.6f L:%+13.6f = %13.6f %s.Pri",
            m_PriStart.DTemp(*m_pPriSg), m_pPriSg->Temp(),
            m_PriStart.DPress(*m_pPriSg), m_pPriSg->Press(),
            m_PriStart.DMass(*m_pPriSg), m_pPriSg->Mass(),
            m_PriStart.DLevel(*m_pPriSg)*100, m_pPriSg->Level()*100, FullObjTag());
        #endif
        }
      if (m_pSecSg)
        {
        #if dbgHeatX
        if (dbgODEOperate() && fDoDbgBrk)
          dbgpln("ODE:---  : T:%+13.6f = %13.6f P:%+13.6f = %13.6f M:%+13.6f = %13.6f L:%+13.6f = %13.6f %s.Sec",
            m_SecStart.DTemp(*m_pSecSg), m_pSecSg->Temp(),
            m_SecStart.DPress(*m_pSecSg), m_pSecSg->Press(),
            m_SecStart.DMass(*m_pSecSg), m_pSecSg->Mass(),
            m_SecStart.DLevel(*m_pSecSg)*100, m_pSecSg->Level()*100, FullObjTag());
        #endif

        m_pSecSg->ODEOperate(ODB);

        #if dbgHeatX
        if (dbgODEOperate() && fDoDbgBrk)
          dbgpln("ODE:Int  : T:%+13.6f = %13.6f P:%+13.6f = %13.6f M:%+13.6f = %13.6f L:%+13.6f = %13.6f %s.Sec",
            m_SecStart.DTemp(*m_pSecSg), m_pSecSg->Temp(),
            m_SecStart.DPress(*m_pSecSg), m_pSecSg->Press(),
            m_SecStart.DMass(*m_pSecSg), m_pSecSg->Mass(),
            m_SecStart.DLevel(*m_pSecSg)*100, m_pSecSg->Level()*100, FullObjTag());
        #endif
        }

      ConvergeStates(CConvergeStateBlk(ODB));
      #if dbgHeatX
      if (m_pPriSg)
        {
        if (dbgODEOperate() && fDoDbgBrk)
          dbgpln("ODE:Cvg  : T:%+13.6f = %13.6f P:%+13.6f = %13.6f M:%+13.6f = %13.6f L:%+13.6f = %13.6f %s.Pri",
            m_PriStart.DTemp(*m_pPriSg), m_pPriSg->Temp(),
            m_PriStart.DPress(*m_pPriSg), m_pPriSg->Press(),
            m_PriStart.DMass(*m_pPriSg), m_pPriSg->Mass(),
            m_PriStart.DLevel(*m_pPriSg)*100, m_pPriSg->Level()*100, FullObjTag());
        }
      if (m_pSecSg)
        {
        if (dbgODEOperate() && fDoDbgBrk)
          dbgpln("ODE:Cvg  : T:%+13.6f = %13.6f P:%+13.6f = %13.6f M:%+13.6f = %13.6f L:%+13.6f = %13.6f %s.Sec",
            m_SecStart.DTemp(*m_pSecSg), m_pSecSg->Temp(),
            m_SecStart.DPress(*m_pSecSg), m_pSecSg->Press(),
            m_SecStart.DMass(*m_pSecSg), m_pSecSg->Mass(),
            m_SecStart.DLevel(*m_pSecSg)*100, m_pSecSg->Level()*100, FullObjTag());
        }
      #endif
      break;
      }
    case eStateFixDV:
    case eStateTest:
    case eStateSave:
    case eStateDiskSave:
      {
      if (m_pPriSg)
        m_pPriSg->ODEOperate(ODB);
      if (m_pSecSg)
        m_pSecSg->ODEOperate(ODB);
      break;
      }
    }
  };

//--------------------------------------------------------------------------

void HeatXchg1::ConvergeStates(CConvergeStateBlk &CSB)
  {
  MdlNode::ConvergeStates(CSB);
  ASSERT(NetDynamicMethod());

  if (m_HX.m_iRqdOpMode==HX1_FullyCondensing && !IntegralDone())
    {
    if (m_iPriSgMd==HXSM_Shell)
      m_HX.m_VLE.QVFlash(*m_pPriSg, 0.0, VLEF_Null);//, NULL, NULL, NULL);
    if (m_iSecSgMd==HXSM_Shell)
      m_HX.m_VLE.QVFlash(*m_pSecSg, 0.0, VLEF_Null);//, NULL, NULL, NULL);
    }
  }

//--------------------------------------------------------------------------

/*#F:This is called once, after the iteration process and allows the discreet
manipulation of the contents contained by the surge unit.*/

void HeatXchg1::EvalDiscrete()
  {
  for (int s=0; s<2; s++)
    {
    SpContainer * pA[]={NULL, NULL, (s==0) ?  m_pPriSg : m_pSecSg};
    SpContainer * p=pA[(s==0) ? m_iPriSgMd : m_iSecSgMd];
    if (p)
      {
      //if (p->SpillExists())
      //  {
      //  SpDirectCd & O=p->OverFlow();
      //  O.SetTotal(O.Total()+O.QMass()*ICGetTimeInc());
      //  }
      //if (p->VentExists())
      //  {
      //  SpDirectCd & O=p->GasVent();
      //  O.SetTotal(O.Total()+O.QMass()*ICGetTimeInc());
      //  }

      // Qualities
      for (int i = 0; i < NoFlwIOs(); i++)
        if (IOIdMask_Self(i) & ((s==0)?PriIOMask:SecIOMask))
          p->AddDiscrete(*IOConduit(i), IOSign(i));
      }
    }
  }

//---------------------------------------------------------------------------

flag HeatXchg1::EvalPressureSens()
  {
  double BigDiff=0.0;
  m_HX.m_dDutySens=m_HX.m_dDuty;
  if (m_pPriSg && m_iPriSgMd==HXSM_Shell)
    {
    // Must be closed and have contents
    if (m_pPriSg->Closed() && m_pPriSg->Mass()> 1.0e-6)
      m_HX.EvalPressureSens(this, *m_pPriSg, m_HXJ.P.JoinId(), m_HXJ.P.IdMask());
    else
      ZeroVLSensitivities(m_HXJ.P.JoinId());
//      Set_JoinP_SensVL(m_HXJ.P.JoinId(), 0.0, 0.0);
    }

  if (m_pSecSg && m_iSecSgMd==HXSM_Shell)
    {
    // Must be closed and have contents
    if (m_pSecSg->Closed() && m_pSecSg->Mass()> 1.0e-6)
      m_HX.EvalPressureSens(this, *m_pSecSg, m_HXJ.S.JoinId(), m_HXJ.S.IdMask());
    else
      ZeroVLSensitivities(m_HXJ.P.JoinId());
      //Set_JoinP_SensVL(m_HXJ.P.JoinId(), 0.0, 0.0);
    }

  return true;//False;
  };

//===========================================================================

void HeatXchg1::ClosureInfo()
  {
  if (NoFlwIOs()>0)
    {
    MdlNode::ClosureInfo();
    if (m_Closure.DoFlows())
      {
      if (m_Closure.GetSize()>0)
        {
        CClosureInfo &CI0=m_Closure[0];
        CI0.m_PowerIn+=m_HX.m_dDuty;
        if (m_pPriVent)
          {
          const int id=IOWithId_Self(ioidPVent);
          if (id<0)
            {
            CI0.m_HfLoss    += m_pPriVent->totHf();
            CI0.m_HsLoss    += m_pPriVent->totHs();
            CI0.m_MassLoss  += m_pPriVent->QMass();
            }
          }
        if (m_Closure.GetSize()>1)
          {
          CClosureInfo &CI1=m_Closure[1];
          CI1.m_PowerIn+=-m_HX.m_dDuty;
          if (m_pSecVent)
            {
            const int id=IOWithId_Self(ioidSVent);
            if (id<0)
              {
              CI1.m_HfLoss    += m_pSecVent->totHf();
              CI1.m_HsLoss    += m_pSecVent->totHs();
              CI1.m_MassLoss  += m_pSecVent->QMass();
              }
            }
          }
        }
      }
    }
  };

//--------------------------------------------------------------------------

dword HeatXchg1::ModelStatus()
  {
  dword Status=MdlNode::ModelStatus();

  if (NoFlwIOs())
    {
    const int ioP=IOWithId_Self(ioidPIn);
    const int ioS=IOWithId_Self(ioidSIn);

    double FlwP=ioP>=0 ? IOQm_In(ioP) : 0.0;
    double FlwS=ioS>=0 ? IOQm_In(ioS) : 0.0;
    //int HasFlw=(fabs(FlwP)>UsableMass && fabs(FlwS)>UsableMass);
    int HasFlw=(fabs(FlwP)>UsableMass || fabs(FlwS)>UsableMass);
    Status |= (HasFlw ? FNS_UFlw : FNS_UNoFlw);
    }
  return Status;
  };

//===========================================================================




