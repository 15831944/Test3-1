//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include "sc_defs.h"
#define  __SP_MODEL_CPP
#include "sp_model.h"
#include "dbgmngr.h"
#include "executiv.h"
#include "COMStuff.h"
#include "vlebase.h"
//#include "optoff.h"

#define  dbgSoluble   (WITHDEBUG || 1)
#define  dbgAutoVLE   WITHDEBUG
#define  dbgSpModel   WITHDEBUG

_declspec(dllexport) int SpModelExDbgOn = 0;

#if dbgSoluble
static CDbgMngr dbgEquilQuals           ("SpMdlEquil", "EquilQuals");
static CDbgMngr dbgSolubleEquil         ("SpMdlEquil", "Soluble");
static CDbgMngr dbgAutoVLEEquil         ("SpMdlEquil", "AutoVLE");
#endif
#if dbgSpModel
static CDbgMngr dbgAddDeriv             ("SpModel", "AddDeriv");
static CDbgMngr dbgAddDerivDbgBrk       ("SpModel", "AddDerivDbgBrk");
static CDbgMngr dbgAllEditable          ("SpModel", "AllEditable");
static CDbgMngr dbgApplyDeriv           ("SpModel", "ApplyDeriv");
static CDbgMngr dbgApplyDerivDbgBrk     ("SpModel", "ApplyDerivDbgBrk");
static CDbgMngr dbgCheckTempValue       ("SpModel", "CheckTempValue");
static CDbgMngr dbgConfig               ("SpModel", "Config");
static CDbgMngr dbgDerivCtrl            ("SpModel", "DerivCtrl");
static CDbgMngr dbgDerivCtrlDbgBrk      ("SpModel", "DerivCtrlDbgBrk");
static CDbgMngr dbgDerivCtrlTests       ("SpModel", "DerivCtrlTests");
static CDbgMngr dbgHfEvals              ("SpModel", "HfEvals");
static CDbgMngr dbgHfEvalsDbg           ("SpModel", "HfEvalsDbg");
#ifndef _RELEASE
static CDbgMngr dbgHoldTestEstTP        ("SpModel", "HoldTestEstTP");
#endif
static CDbgMngr dbgPVTConverge          ("SpModel", "PVTConverge");
static CDbgMngr dbgPVTConvergeDbg       ("SpModel", "PVTConvergeDbg");
static CDbgMngr dbgValidate             ("SpModel", "Validate");
static CDbgMngr dbgValidateBrk          ("SpModel", "ValidateBrk");
static CDbgMngr dbgValidateSpc          ("SpModel", "ValidateSpc");
static CDbgMngr dbgValidateSpcBrk       ("SpModel", "ValidateSpcBrk");
#endif

#define WithExtra_totHs        0
#define WithExtraEntropy_Sf    0

const double LoPressLift       = 5.0;

const int    AutoVLEDisabled   = -2;
const int    AutoVLEOff        = -1;

// ==========================================================================
//
//                             Extra Debug Stuff
//
// ==========================================================================

//#define DO_RIGOROUS_CHECKS 01

//#if (DO_RIGOROUS_CHECKS && defined(DEBUG))
//  inline flag CheckMassNonNeg(CSysVector * p) { return (p->Mass(som_ALL)>=0.0); };
//  #define ASSERT_MASS_OK(p) { p->CheckIt();/*ASSERT(_CrtCheckMemory());*/ if (!CheckMassNonNeg(p)) DoBreak(); };
//#else
//  #define ASSERT_MASS_OK(p)
//#endif
//
// ==========================================================================
//
//                                 Constants
//
// ==========================================================================

const double MinMolesVap = 1.0e-15;

//const byte EHBusy_Clear   = 0;
const byte EHBusy_RqdTemp = 1;
const byte EHBusy_RqdH    = 2;

// ===========================================================================
//
//                            Specie Quality Info
//
// ===========================================================================

flag SpQualInfo::CopyReqd(SpModel * pMdl)
  {
  if (m_pQ)
    {
    if ((m_iAct==SQA_Remove && pMdl->SQFlagsSet(SQF_RemoveOK)) || m_pQ->m_bRemoveMe)
      return false;
    return !m_pQ->m_bHoldCopy;
    }
  return false;
  };

// ===========================================================================
//
//                              Specie Quality
//
// ===========================================================================

const pchar SpQuality::GroupName="SpQuality";

SpQuality::SpQuality(TagObjClass* pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach) :
TaggedObject(pClass_, Tag_, pAttach, eAttach)
  {
  if (pAttachedTo)
    pModel=(SpModel*)pAttachedTo;
  else
    pModel=new SpModel(&SpModelClass, SpModelDefTag, this, TOA_Embedded);
  m_bRemoveMe=false;
  m_bHoldCopy=false;
  m_iQualIndex=-1;

  //m_pMSpQualBase=NULL;
  };

// --------------------------------------------------------------------------

SpQuality::~SpQuality()
  {
  if (!pAttachedTo)
    delete pModel;
  };

//--------------------------------------------------------------------------

long SpQuality::DefinedPropertyCount()
  {
  return 0;
  }

long SpQuality::DefinedPropertyInfo(long Index, MPropertyInfo & Info)
  {
  //long Inx=Index-CPropertyBase::DefinedPropertyCount();
  //switch(Inx)
  //  {
  //  case  0: Info.SetStructName("SpMdlProps");
  //           Info.Set(ePT_Double,    "",  "T",           DC_T,         0,      0,  MP_Null,     "A Desc"); return Inx;
  //  case  1: Info.Set(ePT_Double,    "",  "totCp",       DC_CpMs,   1e-6,      0,  MP_HardMin,  "A Desc"); return Inx;
  //  }
  return CPropertyBase::DefinedPropertyInfo(Index, Info);
  };

DWORD SpQuality::GetPropertyVisibility(long Index)
  {
  return CPropertyBase::GetPropertyVisibility(Index);
  }

void SpQuality::GetPropertyValue(long Index, PhMask Phase, double T, double P, MPropertyValue & Value)
  {
  //switch (Index-CPropertyBase::DefinedPropertyCount())
  //  {
  //  case   0: Value=Temp();               return;
  //  case   1: Value=totCp(Phase, T, P);   return;
  //  };
  CPropertyBase::GetPropertyValue(Index, Phase, T, P, Value);
  };


void SpQuality::PutPropertyValue(long Index, MPropertyValue & Value)
  {
  //switch (Index)
  //  {
  //  case   0: return Temp();
  //  case   1: return totCp();
  //  };
  CPropertyBase::PutPropertyValue(Index, Value);
  };


// --------------------------------------------------------------------------

flag SpQuality::SQCreator()
  {
  ASSERT(pModel && m_iQualIndex>=0 && m_iQualIndex<pModel->nQuals);
  return pModel->Quals[m_iQualIndex].m_iAct==SQA_Create;
  };

// --------------------------------------------------------------------------

void SpQuality::SetSQCreator(flag On)
  {
  ASSERT(pModel && m_iQualIndex>=0 && m_iQualIndex<pModel->nQuals);
  pModel->Quals[m_iQualIndex].m_iAct=On?SQA_Create:SQA_Xfer;
  };

// ==========================================================================
// ==========================================================================

inline void DoTempChecks(double T)
  {
#if dbgSpModel
  //#ifdef _RELEASE
  if (dbgCheckTempValue())
    //#endif
    {
    if (T<C_2_K(-220))
      {
      static int DoIt=1;
      if (DoIt)
        ASSERT_ALWAYS(FALSE, "Temperature below -220C", __FILE__, __LINE__);
      }
    }
#endif
  }

// ===========================================================================
//
//                           CSpMdlDerivCtrl
//
// ===========================================================================

CSpMdlDerivCtrl::CSpMdlDerivCtrl()
  {
  m_V_TestValue=0.0;
  m_SL_TestValue=0.0;
  m_eIntTests=SPMT_Total;

  m_pFd=NULL;
  m_pPr=NULL;
  m_pCn=NULL;
  m_pDM=NULL;

  m_nFlws=0;
  m_Flws.SetSize(0,8);

  Clear();
  };
CSpMdlDerivCtrl::~CSpMdlDerivCtrl()
  {
  delete m_pFd;
  delete m_pPr;
  delete m_pCn;
  delete m_pDM;
  };
void CSpMdlDerivCtrl::Clear()
  {
  m_fStarted=false;
  m_fMassOK=false;
  if (m_pFd)
    m_pFd->ZeroMass();
  if (m_pPr)
    m_pPr->ZeroMass();
  if (m_pCn)
    m_pCn->ZeroMass();
  if (m_pDM)
    m_pDM->Zero();

  m_nFlws=0;

  m_dDuty=0;
  };

void CSpMdlDerivCtrl::Start(SpModel * pMdl)
  {
  m_fStarted=true;
  m_fMassOK=pMdl->m_bUsableMass;

#if dbgSpModel
  if (dbgDerivCtrl() || dbgDerivCtrlDbgBrk() && pMdl->DoDbgBrk())
    {
    Strng S;
    dbgpln("Start    -- T:%7.2f P:%9.2f Ms:%14.6f msHf:%9.2f %7s %6s %s",
      K_2_C(pMdl->Temp()), pMdl->Press(), pMdl->Mass(), pMdl->m_msHf,
      m_fStarted?"Started":"",m_fMassOK?"MassOK":"", pMdl->FullObjTag());
    }
#endif

  if (m_pFd==NULL)
    m_pFd=(SpModel*)pMdl->Class()->Construct(NULL, "Fd", NULL, TOA_Free);
  if (m_pPr==NULL)
    m_pPr=(SpModel*)pMdl->Class()->Construct(NULL, "Pr", NULL, TOA_Free);
  if (m_pCn==NULL)
    m_pCn=(SpModel*)pMdl->Class()->Construct(NULL, "Cn", NULL, TOA_Free);
  if (m_pDM==NULL)
    m_pDM=new SpVector;

  m_nFlws=0;

  m_pFd->SetPress(pMdl->Press());
  m_pPr->SetPress(pMdl->Press());
  };

void CSpMdlDerivCtrl::Finalise(SpModel * pMdl, double dTime)
  {
  int DoSubs=1;
#if dbgSpModel
  double T0=pMdl->Temp();
  double P0=pMdl->Press();
  double H0=pMdl->totHf();
  double MV0=pMdl->Mass(som_Gas);
  double MSL0=pMdl->Mass(som_SL);
  SpMArray XXX(*pMdl);
  if (dbgDerivCtrl() || dbgDerivCtrlDbgBrk() && pMdl->DoDbgBrk())
    {
    Strng S;
    dbgpln("Finalise -- T:%7.2f P:%9.2f Ms:%14.6f msHf:%9.2f %7s %6s %s",
      K_2_C(pMdl->Temp()), pMdl->Press(), pMdl->Mass(), pMdl->m_msHf,
      m_fStarted?"Started":"",m_fMassOK?"MassOK":"", pMdl->FullObjTag());
    }
#endif
  if (!m_fStarted)
    {
    pMdl->m_msHfDeriv=0;
    return;
    }

  m_pCn->Copy(pMdl);


#if dbgSpModel
  int dbgAdd=dbgAddDeriv() || dbgAddDerivDbgBrk() && pMdl->DoDbgBrk();
#endif

  if (1)
    {
    DMPSYSVEC(1, "Deriv", pMdl->m_dM);
    for (int i=0; i<m_nFlws; i++)
      {
      SpModel * Flw=m_Flws[i].m_pMdl;
      int       Sgn=m_Flws[i].m_iSgn;
      //DMPSYSVEC(0, "Flw", Flw->MArray());
      pMdl->m_dM.AddDeriv(Sgn, dTime, Flw->MArray(), pMdl->MArray(), 0);

      #if dbgSpModel
      if (dbgAdd)
        dbgpln("AddDeriv    T:%7.2f P:%9.2f Qm:%14.6f msHf:%9.2f %s @ %s",
        K_2_C(Flw->Temp()), Flw->Press(), Flw->Mass(), Flw->msHf(), pMdl->FullObjTag(), Flw->FullObjTag());
      #endif

      //if (Sgn>=0)
      //  pMdl->dM += Flw->M;
      //else
      //  pMdl->dM -= Flw->M;
      pMdl->m_Ovr.AddSrc(Flw->m_Ovr);

      for (int q=0; q<Max(pMdl->nQuals, Flw->nQuals); q++)
        {
        if (Flw->Quals[q].Exists())
          pMdl->QualityPtr(q);
        if (pMdl->Quals[q].Exists())
          pMdl->Quals[q].m_pQ->AddDeriv(Flw, Flw->QualityPtr(q, false), Sgn);
        }
      }
    DMPSYSVEC(0, "=", pMdl->m_dM);
    }

  double xmsHFd=m_pFd->msHf();
  double xmsHPr=m_pPr->msHf();
  double xmsH=m_pCn->msHf();
  double xtotH=m_pCn->totHf();

  m_pCn->AddMassF(m_pFd, som_ALL, dTime);
  if (DoSubs)
    m_pCn->SubMassF(m_pPr, som_ALL, dTime);

  double msH=m_pCn->msHf();
  double totH=m_pCn->totHf();

  for (int s=0; s<SVValueCount()/*SVSpcCount()*/; s++)
    m_pCn->m_M.VValue[s] += (*m_pDM)[s]*dTime;
  m_pCn->ClrStatesOK();
  m_pCn->Set_totHf(totH+m_dDuty*dTime);

  if (dTime>1.0e-10)
    {
    double New_msHf = m_pCn->msHf();
    pMdl->m_msHfDeriv = (New_msHf - pMdl->m_msHf)/dTime;
    }
  else
    pMdl->m_msHfDeriv = 0;

#if dbgSpModel
  if (dbgDerivCtrlTests() && pMdl->DoDbgBrk())
    {
    dTime+=100;
    m_pCn->Copy(pMdl);

    double xmsHFd=m_pFd->msHf();
    double xmsHPr=m_pPr->msHf();
    double xmsH=m_pCn->msHf();
    double xtotH=m_pCn->totHf();

    m_pCn->AddMassF(m_pFd, som_ALL, dTime);
    if (DoSubs)
      m_pCn->SubMassF(m_pPr, som_ALL, dTime);

    double msH=m_pCn->msHf();
    double totH=m_pCn->totHf();

    for (int s=0; s<SVValueCount()/*SVSpcCount()*/; s++)
      m_pCn->m_M.VValue[s]+=(*m_pDM)[s]*dTime;
    m_pCn->ClrStatesOK();
    m_pCn->Set_totHf(totH+m_dDuty*dTime);
    //m_pCn->Set_msHf(msH+m_dDuty*dTime/GTZ(pMdl->Mass()));
    //m_pCn->Set_msHf(msH+m_dDuty*dTime/GTZ(m_pCn->Mass()));
    //double dmsHDeriv100=0;
    //if (m_fMassOK && (dTime>1.0e-10))
    //  {
    //  double New_dmsHState = m_pCn->msHf();
    //  dmsHDeriv100 = (New_dmsHState-m_dmsHState)/dTime;
    //  }
    //else
    //  dmsHDeriv100 = 0;

    int xx=0;
    }
#endif

  //if (fabs(m_dmsHDeriv)>1.0e6)
  //  { int xxx=0; }

#if dbgSpModel
  if (dbgDerivCtrl() || dbgDerivCtrlDbgBrk() && pMdl->DoDbgBrk())
    {
    dbgpln("  dHf: %14.6g",  pMdl->m_msHfDeriv);
    for (int i=0; i<SVValueCount()/*SVSpcCount()*/; i++)
      if (fabs(pMdl->m_dM[i])>1.0e-10)
        dbgpln("  dM : %14.6g {F:%+14.6g P:%+14.6g D:%+14.6g} M :%14.6f %5.1f%% %s",
        pMdl->m_dM[i], m_pFd->m_M[i], m_pPr->m_M[i], (*m_pDM)[i],
        pMdl->m_M[i], 100*pMdl->m_dM[i]*ICGetTimeInc()/Max(1.0e-6, pMdl->m_M[i]),
        SVI.Img(i)->FullTag());
    }
#endif
  };

void CSpMdlDerivCtrl::Save(SpModel * pMdl)
  {
  m_pCn->Copy(pMdl);
  };
void CSpMdlDerivCtrl::Restore(SpModel * pMdl)
  {
  pMdl->Copy(m_pCn);
  };
void CSpMdlDerivCtrl::Estimate(SpModel * pMdl, double dTime)
  {
#if dbgSpModel
  double T0=pMdl->Temp();
  double P0=pMdl->Press();
  double H0=pMdl->totHf();
  double MV0=pMdl->Mass(som_Gas);
  double MSL0=pMdl->Mass(som_SL);
  SpMArray XXX(*pMdl);
  if (dbgDerivCtrl() || dbgDerivCtrlDbgBrk() && pMdl->DoDbgBrk())
    dbgpln("Estimate %s %s",m_fStarted?"Started":"",m_fMassOK?"MassOK":"");
#endif
  pMdl->AddMassF(m_pFd, som_ALL, dTime);
  pMdl->SubMassF(m_pPr, som_ALL, dTime);

  double msH=pMdl->msHf();
  double totH=pMdl->totHf();
  for (int s=0; s<SVValueCount()/*SVSpcCount()*/; s++)
    pMdl->m_M.VValue[s]+=(*m_pDM)[s]*dTime;
  pMdl->ClrStatesOK();
  TESTMASSNOTNEGC(pMdl);
  pMdl->Set_msHf(msH+m_dDuty*dTime/GTZ(pMdl->Mass()));
  //pMdl->Set_totHf(totH+m_dDuty*dTime);

#if dbgSpModel
  double T1=pMdl->Temp();
  double P1=pMdl->Press();
  double H1=pMdl->totHf();
  double MV1=pMdl->Mass(som_Gas);
  double MSL1=pMdl->Mass(som_SL);
  if (dbgDerivCtrl() || dbgDerivCtrlDbgBrk() && pMdl->DoDbgBrk())
    {
    //      dbgpln("QVFlash:%s",pMdl->FullObjTag());
    dbgpln("  T : %14.6f > %14.6f [%+14.6g]",  T0, T1, T1-T0);
    dbgpln("  P : %14.6f > %14.6f [%+14.6g]",  P0, P1, P1-P0);
    dbgpln("  H : %14.6g > %14.6g [%+14.6g]",  H0, H1, H1-H0);
    dbgpln("  V : %14.6f > %14.6f [%+14.6g]",  MV0, MV1, (MV1-MV0));
    dbgpln("  L : %14.6f > %14.6f [%+14.6g]",  MSL0, MSL1, (MSL1-MSL0));
    CSysVector &YYY=pMdl->MArray();
    for (int i=0; i<SVValueCount()/*SVSpcCount()*/; i++)
      if (XXX[i]>1.0e-10 || YYY[i]>1.0e-10)
        dbgpln("  M : %14.6f > %14.6f [%+14.6g]{F:%+14.6g P:%+14.6g D:%+14.6g} %s",
        XXX[i], YYY[i], YYY[i]-XXX[i], m_pFd->m_M[i], m_pPr->m_M[i], (*m_pDM)[i], SDB[i].SymOrTag());
    }
#endif

  };

// ==========================================================================
//                               Specie Model
// ==========================================================================

// --------------------------------------------------------------------------

const pchar   SpModel::GroupName="SpModel";
double        SpModel::dUsrDefT=C_2_K(25);
double        SpModel::dUsrDefP=dNAN;
double        SpModel::dTestToleranceMassFrac=0.0001;
bool          SpModel::m_RangeErrorsOn=false;
bool          SpModel::m_RangeExtErrorsOn=true;

flag          SpModel::sm_VarsChange=0;
long          SpModel::sm_VarsAvail=0;
flag          SpModel::sm_SpVarsChange=0;
long          SpModel::sm_SpVarsAvail=0;

#ifndef _RELEASE
bool          SpModel::m_bTestMassNotNeg=true;
#endif

#if WITHSPMDLTIMERS
CGlblStopWatch SpModel::sm_swEvalM(gs_swFlwSolve, "SpMdl",  "EvalM");
CGlblStopWatch SpModel::sm_swEvalT(gs_swFlwSolve, "SpMdl",  "EvalT");
CGlblStopWatch SpModel::sm_swEvalH(gs_swFlwSolve, "SpMdl",  "EvalH");
CGlblStopWatch SpModel::sm_swEvalHC(gs_swFlwSolve, "SpMdl", "EvalHC");
CGlblStopWatch SpModel::sm_swHeat(gs_swFlwSolve, "SpMdl", "Heat");
CGlblStopWatch SpModel::sm_swDens(gs_swFlwSolve, "SpMdl", "Dens");
long SpModel::sm_lEvalMCalls=0;
long SpModel::sm_lEvalMEvals=0;
long SpModel::sm_lEvalHCalls=0;
long SpModel::sm_lEvalHEvals=0;
#endif

//long    SpModel::nCpMeanCnt=5;

SPMEnthStrat SpModel::iEnthalpyStrategy=SPMES_BestTemp;
SPMMixStrat SpModel::iSpMdlMixStrategy=SPMMS_Bld35;

DDBValueLstMem SpModel::sm_DDBFlashable;
CArray<Strng, Strng> SpModel::sm_FlashCmps;
CArray<Strng, Strng> SpModel::sm_FlashCmpsI;
bool    SpModel::sm_FlashInited=false;

IMPLEMENT_SPARES(SpModel, 100);

#if WithSIMPLESPMDL
IMPLEMENT_SPMODEL(SpModel, BaseSpModelName, "", TOC_SYSTEM,
                  BaseSpModelName,
                  "Base specie model");
#else
IMPLEMENT_SPMODEL(SpModel, "Simple", "", TOC_SYSTEM,
                  MassWtMnSpModelName,
                  "Simple Mass-weighted mean specie model");
#endif

IMPLEMENT_BUILDDATADEFNGLOBAL(SpModel);

long SpModel::m_iFidelity=1/*0*/; // currently just a static - must improve
long SpModel::sm_iFixupStrategy=FUS_Largest;//FUS_BottomUp;

eScdGasMixingRule SpModel::sm_iGasMixingRule = eScdGMR_Simple;
eScdEqnOfState    SpModel::sm_iEqnOfState    = eScdEOS_SRK;

SpModel::SpModel(TagObjClass* pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach) ://, pSpMassModel pMM_)
TaggedObject(pClass_, Tag_, pAttach, eAttach)
  {
  if (!sm_FlashInited)
    {
    sm_DDBFlashable.Add(CDB.No(), "<Default>");
    sm_FlashCmps.SetSize(CDB.No());
    sm_FlashCmpsI.SetSize(CDB.No());
    for (int i=0; i<CDB.m_Flashable.GetCount(); i++)
      {
      int c=CDB.m_Flashable[i];
      sm_DDBFlashable.Add(c, CDB[c].SymOrTag());
      sm_FlashCmps[c]=CDB[c].SymOrTag();
      sm_FlashCmpsI[c]="<";
      sm_FlashCmpsI[c]+=CDB[c].SymOrTag();
      sm_FlashCmpsI[c]+=">";
      }

    sm_FlashInited=true;
    };

  Strng S;
  m_dwValidFlags=0;
  m_dwMassVersionNo=0;
  m_iEnable=IE_Disabled;
  //m_bTempKept=0;
  //m_bPresKept=0;
  //m_bFileTempSet=0;
  //m_bFilePresSet=0;
  m_bBadTempOK=0;
  m_iUsage=SPMU_Mass;
  //m_iCfgTemplate=0;
  //m_bSQEditable=1;
  m_fConvergeBusy=0;
  m_pSetCfgImg=NULL;

  //  m_iFidelity = false;
  m_bClosed = 0;
  m_bPressAsSet = 0;
  m_bMixed  = 0;

  m_M.Zero();
  m_dM.Zero();

  m_msHf=0.0;

  m_dMassScale=1.0;
  m_bKeepNormalEnabled=false;
  m_iDataSgn=1;
  m_bNegDataOK=false;

  m_CurrentP=Std_P;
  m_CurrentT=Std_T;
  m_PreviousT=m_CurrentT;

  m_RqdTemp=Std_T;
  m_KeptTemp=dNAN;
  m_FileTemp=dNAN;
  m_FilePres=dNAN;

  //  m_dMinLevel=0.01;
  m_dMaxLevel=0.95;
  m_dMaxPress=Std_P+50000.0;
  //  m_bAutoExpand = false;
  m_bVentExpandOK = false;
  m_iHoldVentExpand = 0;
  m_iHoldEquilibrate = 0;
  m_iEquilibrateBusy = 0;

  m_bUsableMass=true;
  m_bTraceMass=false;

  nQuals=0;
  Quals=new SpQualInfo[MaxSpQualities];

  bSQFlags=SQF_CreateOK;

  for (int i=0; i<MaxPhases; i++)
    m_PhMass[i]=0.0;
  m_bMassInUseOK=false;

  m_iSetView=SVV_NULL;
  m_iViewBasis=SVVB_All;
  m_bSpeciesAdjusted=false;

  m_pGlblView=NULL;
  m_bCfgImgSet=false;
  m_dSetCfgImgT=Std_T;
  m_dSetCfgImgP=Std_P;
  m_sMainTemplate="-";

  m_dPrvSatT=dNAN;//Std_T;

  m_pVLE = NULL;

  m_iSatCmpRqd=-1;
  m_iSatCmp=CDB.DefFlashCmpIndex();

  m_iEquilAct=SQA_Xfer;
  m_bEquilOn=false;

  m_AutoVLE.m_iCmp=-1;
  m_AutoVLE.m_iLiq=-1;
  m_AutoVLE.m_iVap=-1;
  m_AutoVLE.m_iSet=AVLESet_None;
  m_AutoVLE.m_dSpecH=0;
  m_AutoVLE.m_dRelH=0;
  m_AutoVLE.m_dDewPt=C2K(20);

  m_bEquilRqd4HEval=false;
  m_bScan4EquilRqd4HEval=true;
  m_iEHBusy=0;

  m_dGasMixAlpha = 1.0;
  m_LoMassLimit=0.0;

  m_VarsAvail=0;
  if (sm_VarsAvail==0)
    {
    if (PrjFileVerNo()<81)
      sm_VarsAvail = 0xffffffff;
    else
      sm_VarsAvail =  //VAMsk_Overides |
                      VAMsk_Thermo |
                      //VAMsk_Viscosity |
                      //VAMsk_HeatValues |
                      0;
    sm_VarsAvail |= VAMsk_Inited;
    }
  m_SpVarsAvail=0;
  if (sm_SpVarsAvail==0)
    {
    if (PrjFileVerNo()<81)
      sm_SpVarsAvail = 0xffffffff;
    else
      sm_SpVarsAvail =  VAMsk_SpMass |
                        //VAMsk_SpMole |
                        VAMsk_SpVol  |
                        //VAMsk_SpPartialP |
                        //VAMsk_SpNVol |
                        //VAMsk_SpElem |
                        VAMsk_SpComp |
                        0;
    sm_SpVarsAvail |= VAMsk_SpInited;
    }
  };

// --------------------------------------------------------------------------

SpModel::~SpModel()
  {
  if (m_pSetCfgImg)
    {
    CSpMCfgImg * pImg=&m_pSetCfgImg[-SVClcCount()];
    delete pImg;
    }
  for (int q=0; q<nQuals; q++)
    {
    delete Quals[q].m_pQ;
    delete Quals[q].m_pQP;
    }
  delete []Quals;
  };

// --------------------------------------------------------------------------

SpQuality* SpModel::QualityPtr(int No, flag NewIt)
  {
  int q=No;
  VERIFY(q<MaxSpQualities);
  if ( Quals[q].m_pQ==NULL && NewIt)
    {
    TagObjClass* pQ=TagObjClass::FindGroupId(SpQuality::GroupName);
    while (pQ && pQ->GroupIdNo()!=q)
      pQ=pQ->NextClassInGrp();

    ASSERT(pQ);
    Quals[q].m_pQ=(SpQuality*)pQ->Construct(NULL, pQ->DefaultTag(), this, TOA_Embedded);
    Quals[q].m_pQP=GetQualityPersonality(Quals[q].m_pQ);  // ask Spmodel for the personality
    Quals[q].m_pQ->m_iQualIndex=q;
    nQuals=Max( nQuals, (byte)(q+1));
    m_bScan4EquilRqd4HEval=true;
    StructureChanged(this);
    };
  return (SpQuality*)( Quals[q].m_pQ);
  };

//---------------------------------------------------------------------------

void SpModel::QualityRemove(int No)
  {
  int q=No;
  VERIFY(q<MaxSpQualities);
  if (Quals[q].Exists())
    {
    delete Quals[q].m_pQ;
    Quals[q].m_pQ=NULL;
    delete Quals[q].m_pQP;
    Quals[q].m_pQP=NULL;
    nQuals=Max( nQuals, (byte)(q+1));
    m_bScan4EquilRqd4HEval=true;
    StructureChanged(this);
    };
  };

//---------------------------------------------------------------------------

SpQuality* SpModel::QualityPtr(char* RqdQuality, flag NewIt)
  {
  TagObjClass* pQ=TagObjClass::FindGroupId(SpQuality::GroupName);
  pQ=pQ->FindGrpClassId(RqdQuality);
  if (pQ)
    {
    ASSERT(pQ);
    int q=pQ->GroupIdNo();
    ASSERT(q<MaxSpQualities);
    if (!Quals[q].Exists() && NewIt)
      {
      Quals[q].m_pQ=(SpQuality*)pQ->Construct(NULL, pQ->DefaultTag(), this, TOA_Embedded);
      Quals[q].m_pQP=GetQualityPersonality(Quals[q].m_pQ);  // ask Spmodel for the personality;
      nQuals=Max( nQuals, (byte)(q+1));
      m_bScan4EquilRqd4HEval=true;
      StructureChanged(this);
      }
    return (SpQuality*)( Quals[q].m_pQ);
    };
  return NULL;
  };

// --------------------------------------------------------------------------

void SpModel::QualitiesSetState(eScdMdlStateActs RqdState)
  {
  switch (RqdState)
    {
    case MSA_PBInit:
    case MSA_ZeroFlows:
    case MSA_Empty:
      {
      for (int q=0; q<nQuals; q++)
        {
        if (Quals[q].m_iAct!=SQA_Create && Quals[q].Exists())
          {
          QualityRemove(q);
          }
        }
      break;
      }
    /*case MSA_PreSet:
    case MSA_SteadyState:
    case MSA_DynStatsRunInit:*/
    }
  }

// --------------------------------------------------------------------------

void SpModel::Scan4EquilRqd4HEval()
  {
  if (!m_bScan4EquilRqd4HEval)
    return;
  m_bEquilRqd4HEval=false;
  for (int q=0; q<nQuals; q++)
    if (Quals[q].m_pQ && Quals[q].m_pQ->EquilRqd4HEval())
      m_bEquilRqd4HEval=true;
  m_bScan4EquilRqd4HEval=false;
  }

// --------------------------------------------------------------------------

void SpModel::SetQualityPresent(int q, flag Present)
  {
  if ((Quals[q].m_pQ==NULL) && Present)
    {
    QualityPtr(q, true);
    }
  else if (!Present && Quals[q].m_pQ!=NULL)
    {
    QualityRemove(q);
    //Quals[q].pQ->Destroy();
    //Quals[q].pQ=NULL;
    }
  }

// --------------------------------------------------------------------------

void SpModel::CopyQualities(SpModel * SrcModel, int q0, int q1)
  {
  if (m_iSatCmpRqd>=0)
    m_iSatCmp=m_iSatCmpRqd;
  //else if (m_iSatCmpRqd==-2)
  //  m_iSatCmp=CDB.DefFlashCmpIndex();
  else  
    m_iSatCmp=SrcModel->SaturationCmpIndex();

  if (m_iEquilAct==SQA_Create)
    m_bEquilOn=true;
  else if (SrcModel->m_iEquilAct==SQA_Remove)
    m_bEquilOn=false;
  else
    {
    m_bEquilOn=SrcModel->m_bEquilOn;
    m_AutoVLE.m_iCmp=SrcModel->m_AutoVLE.m_iCmp;
    m_AutoVLE.m_iVap=SrcModel->m_AutoVLE.m_iVap;
    m_AutoVLE.m_iLiq=SrcModel->m_AutoVLE.m_iLiq;
    }
  m_Ovr.CopySrc(SrcModel->m_Ovr);

  if (q0<0) q0=0;
  if (q1<0) q1=SrcModel->nQuals-1;
  for (int q=q0; q<=q1; q++)
    {
    if (Quals[q].m_iAct!=SQA_Create)
      {
      SetQualityPresent(q, SrcModel->Quals[q].CopyReqd(this));
      if (Quals[q].Exists())
        Quals[q].m_pQ->Copy(SrcModel, SrcModel->QualityPtr(q, false));
      }
    }

  //CHECK Shouldn't extra qualities in this but not present in SrcModel be deleted!?!??")
  //Scan4EquilRqd4HEval();
  }

// --------------------------------------------------------------------------

void SpModel::SetQualities(SpModel * SrcModel)
  {
  if (m_iSatCmpRqd>=0)
    m_iSatCmp=m_iSatCmpRqd;
  //else if (m_iSatCmpRqd==-2)
  //  m_iSatCmp=CDB.DefFlashCmpIndex();
  else  
    m_iSatCmp=SrcModel->SaturationCmpIndex();

  if (m_iEquilAct==SQA_Create)
    m_bEquilOn=true;
  else if (SrcModel->m_iEquilAct==SQA_Remove)
    m_bEquilOn=false;
  else
    {
    m_bEquilOn=SrcModel->m_bEquilOn;
    m_AutoVLE.m_iCmp=SrcModel->m_AutoVLE.m_iCmp;
    m_AutoVLE.m_iVap=SrcModel->m_AutoVLE.m_iVap;
    m_AutoVLE.m_iLiq=SrcModel->m_AutoVLE.m_iLiq;
    }

  m_Ovr.CopySrc(SrcModel->m_Ovr);

  for (int q=0; q<SrcModel->nQuals; q++)
    {
    if (Quals[q].m_iAct!=SQA_Create)
      {
      SetQualityPresent(q, SrcModel->Quals[q].CopyReqd(this));
      if (Quals[q].Exists())
        Quals[q].m_pQ->SetMassF(MArray(), SrcModel->QualityPtr(q, false));
      }
    }
  //Scan4EquilRqd4HEval();
  }

// --------------------------------------------------------------------------

void SpModel::AddQualities(SpModel * SrcModel, CSysVector & Cm)
  {
  if (SrcModel->Mass(som_ALL)>Mass(som_ALL)) // should test rel masses
    {
    //dbgpln("");
    if (m_iSatCmpRqd>=0)
      m_iSatCmp=m_iSatCmpRqd;
    //else if (m_iSatCmpRqd==-2)
    //  m_iSatCmp=CDB.DefFlashCmpIndex();
    else  
      m_iSatCmp=SrcModel->SaturationCmpIndex();
    }

  if (m_iEquilAct==SQA_Create)
    m_bEquilOn=true;
  else if (SrcModel->m_iEquilAct==SQA_Remove)
    m_bEquilOn=false;
  else
    {
    m_bEquilOn=m_bEquilOn||SrcModel->m_bEquilOn;
    m_AutoVLE.m_iCmp=SrcModel->m_AutoVLE.m_iCmp;
    m_AutoVLE.m_iVap=SrcModel->m_AutoVLE.m_iVap;
    m_AutoVLE.m_iLiq=SrcModel->m_AutoVLE.m_iLiq;
    }

  m_Ovr.AddSrc(SrcModel->m_Ovr);

  for (int q=0; q<Max(nQuals, SrcModel->nQuals); q++)
    {
    if (Quals[q].m_iAct!=SQA_Create)
      {
      if (SrcModel->Quals[q].CopyReqd(this))
        QualityPtr(q);
      //      else //kga 7/4/99
      //        SetQualityPresent(q, false); //kga 7/4/99
      if (Quals[q].Exists())
        Quals[q].m_pQ->AddMassF(MArray(), SrcModel->QualityPtr(q, false), Cm);//Phase, Frac);
      }
    }
  //Scan4EquilRqd4HEval();
  }

// --------------------------------------------------------------------------

CToleranceBlock gs_EquilFinderTol(TBF_BothSys, "SpModel:EquilFinder", 0, 1.0e-9, 100, TBF_UseAbs|TBF_UseRel|TBF_UseMax);

void SpModel::EquilibrateAll(double RqdT, double RqdP)
  {
  if ((m_iEquilibrateBusy==0) && (m_iHoldEquilibrate==0) && ((m_dwValidFlags & SMVF_EquilOK)==0))
    {
#if dbgSoluble || dbgAutoVLE
    if (dbgEquilQuals())
      dbgpln("SpModel::EquilibrateAll RqdT:%15s  RqdP:%15s ----------------------  %s", DbgFltString(RqdT), DbgFltString(RqdP), FullObjTag());
#endif
    m_iEquilibrateBusy++;
    for (int q=0; q<nQuals; q++)
      if (Quals[q].Exists())
        Quals[q].m_pQ->Equilibrate();

    //CHECK Remember Previous as starting values

    if (m_bEquilOn)
      {
#if dbgSoluble || dbgAutoVLE
      if (dbgSolubleEquil() || dbgAutoVLEEquil())
        dbgpln("SpModel::EquilSol/VLE   RqdT:%15s  RqdP:%15s ----------------------  %s", DbgFltString(RqdT), DbgFltString(RqdP), FullObjTag());
#endif
#if dbgSoluble
      double InitdbgT=Valid(RqdT)?RqdT:Temp();
      if (dbgSolubleEquil())
        {
        dbgp("%5s","");
        for (int j=0; j<SDB.m_SolubleSps.GetSize(); j++)
          {
          int iSolvent=SDB.m_SolubleSps[j];
          CSpecie & Solvent=SDB[iSolvent];
          for (int i=0; i<Solvent.SolubilityCount(); i++)
            {
            CSolubility &S=Solvent.Solubility(i);
            dbgp(" %+29s %6s", SDB[S.m_iSoluteS].SymOrTag(),"");
            }
          }
        dbgpln("");
        dbgp("%-5s","Iter");
        for (j=0; j<SDB.m_SolubleSps.GetSize(); j++)
          {
          int iSolvent=SDB.m_SolubleSps[j];
          CSpecie & Solvent=SDB[iSolvent];
          for (int i=0; i<Solvent.SolubilityCount(); i++)
            {
            CSolubility &S=Solvent.Solubility(i);
            dbgp(" %+14s %+14s %+6s", "Act","Rqd", "T");
            }
          }
        dbgpln("");
        }
#endif


      int SolConverged=False;
      int VLEConverged=False;
      for (int Iter=0; Iter<gs_EquilFinderTol.GetMaxIters() && (!SolConverged || !VLEConverged); Iter++)
        {
        SolConverged=true;
        VLEConverged=true;
#if dbgSoluble
        if (dbgSolubleEquil())
          dbgp("%5i",Iter);
#endif

        for (int j=0; j<SDB.m_SolubleSps.GetSize(); j++)
          {
          int iSolvent=SDB.m_SolubleSps[j];
          CSpecie & Solvent=SDB[iSolvent];
          for (int i=0; i<Solvent.SolubilityCount(); i++)
            {
            CSolubility &S=Solvent.Solubility(i);
            double Ht=totHf();
            double TotSolute=m_M[S.m_iSoluteS]+m_M[S.m_iSoluteA];
            double MassSolvent=m_M[iSolvent];
            if (TstGTZ(TotSolute) && TstGTZ(MassSolvent))
              {
              double SolFrac=m_M[S.m_iSoluteS]/TotSolute;
              double MassAq=m_M[S.m_iSoluteA];

              double T=Valid(RqdT)?RqdT:Temp();
              double P=Valid(RqdP)?RqdP:Press();
              //double SolventDens=SDB[iSolvent].Density(m_iFidelity, T, P, &m_Ovr, SVData());
              //double SolventVol=(MassSolvent+MassAq)/SolventDens;
              double ActConc=m_M[S.m_iSoluteA]/MassSolvent;
              //double RqdConc=GEZ(S.pFn1 ? S.pFn1->Value(T) : S.pFn2->Value(T, P));
              double RqdConc=GEZ(S.m_pSolubleFn ? S.m_pSolubleFn->AtT(T) : 1e12);
              double Err=RqdConc-ActConc;
              //Conc[i]=ActConc;
#if dbgSoluble
              if (dbgSolubleEquil())
                dbgp(" %14.6g %14.6g %6.1f",ActConc,RqdConc,T);
#endif
              if (gs_EquilFinderTol.ConvergedDV(Err, RqdConc))
                continue;
              //if (fabs(Err)<1.0e-8*RqdConc+1.0e-10)
              //  continue;
              if (RqdConc>ActConc && SolFrac<0.000000000001)
                continue;
              if (RqdConc<ActConc && SolFrac>0.999999999999)
                continue;
              SolConverged=False;

              double TstdMassAq=SolFrac>0.5 ? +1.0e-3*TotSolute : -1.0e-3*TotSolute;
              double TstSolFrac=Range(0.0, SolFrac-TstdMassAq/TotSolute, 1.0);
              VValue[S.m_iSoluteS]=TotSolute*(TstSolFrac);
              VValue[S.m_iSoluteA]=TotSolute*(1.0-TstSolFrac);
              ClrStatesOK();

              if (Valid(RqdT))
                Ht=totHf(som_ALL, RqdT, RqdP);
              Set_totHf(Ht);

              double T1=Valid(RqdT)?RqdT:Temp();
              //double RqdConc1=GEZ(S.pFn1 ? S.pFn1->Value(T1) : S.pFn2->Value(T1, P));
              double RqdConc1=GEZ(S.m_pSolubleFn ? S.m_pSolubleFn->AtT(T1) : 1e12);
              double Slope=(RqdConc1-RqdConc)/TstdMassAq;

              double dMAq=(MassSolvent*(RqdConc-ActConc))/(1-LEZ(MassSolvent*Slope));
              double NewSolFrac=Range(0.0, SolFrac-dMAq/TotSolute, 1.0);
              VValue[S.m_iSoluteS]=TotSolute*(NewSolFrac);
              VValue[S.m_iSoluteA]=TotSolute*(1.0-NewSolFrac);
              ClrStatesOK();

              if (Valid(RqdT))
                Ht=totHf(som_ALL, RqdT, RqdP);
              Set_totHf(Ht);

              //double SolventDens1=SDB[iSolvent].Density(m_iFidelity, Valid(RqdT)?RqdT:Temp(), P, &m_Ovr, SVData());
              //double SolventVol1=(MassSolvent+MassAq)/SolventDens1;
              double ActConc1=m_M[S.m_iSoluteA]/MassSolvent;

              int xxx=0;
              }
            else
              {
#if dbgSoluble
              if (dbgSolubleEquil())
                dbgp(" %14s %14s %6s","","","");
#endif
              }
            }
          }
#if dbgSoluble
        if (dbgSolubleEquil())
          dbgpln("");
#endif

#if WITHAUTOVLE
        if (m_AutoVLE.m_iCmp>=0)
          {
          switch (m_AutoVLE.m_iSet)
            {
            case AVLESet_None:
              {
              if (Valid(RqdT))
                Set_totHf(totHf(som_ALL, RqdT, RqdP));
              double TotMass=VValue[m_AutoVLE.m_iLiq]+VValue[m_AutoVLE.m_iVap];
              double VapP=SDB[m_AutoVLE.m_iVap].VapourP(m_iFidelity, Valid(RqdT)?RqdT:Temp());
              double PPVap1=PartialPress(m_AutoVLE.m_iVap, -1, Valid(RqdT)?RqdT:Temp());
              double MassGas=Mass(som_Gas);
              if ((MassGas-VValue[m_AutoVLE.m_iVap])<1.0e-6*MassGas)
                {
                dbgp("Pure VLE Gas - No Other Gasses");
                int xxx=0;
                }
              else
                {
#if dbgAutoVLE
                if (dbgAutoVLEEquil())
                  dbgp("I:%3i T:%10.4f PP:%10.4f VapP:%10.4f", Iter ,Valid(RqdT)?RqdT:Temp(), PPVap1, VapP);
#endif
                if (PPVap1<VapP && VValue[m_AutoVLE.m_iLiq]<ZeroLimit)
                  {
#if dbgAutoVLE
                  if (dbgAutoVLEEquil())
                    dbgp("Cannot flash more  T Above DewPt");
#endif
                  }
                else if(!gs_EquilFinderTol.ConvergedVV(PPVap1, VapP))
                  {
#if dbgAutoVLE
                  if (dbgAutoVLEEquil())
                    dbgp("gs_EquilFinderTol NOT Converged");
#endif
                  double DMassVap;
                  if (VValue[m_AutoVLE.m_iVap]>ZeroLimit)
                    DMassVap=(PPVap1>VapP ? -0.01:0.01)*VValue[m_AutoVLE.m_iVap];
                  else
                    DMassVap=0.001*VValue[m_AutoVLE.m_iLiq];
                  if (fabs(DMassVap)>ZeroLimit)
                    {
                    VLEConverged=false;
                    VValue[m_AutoVLE.m_iLiq]-=DMassVap;
                    VValue[m_AutoVLE.m_iVap]+=DMassVap;
                    ClrStatesOK();
                    double PPVap2=PartialPress(m_AutoVLE.m_iVap, -1, Valid(RqdT)?RqdT:Temp());
                    VValue[m_AutoVLE.m_iLiq]-=-DMassVap;
                    VValue[m_AutoVLE.m_iVap]+=-DMassVap;
                    double dPPdM=(PPVap2-PPVap1)/DMassVap;
                    double Mass2Liq=(PPVap1-VapP)/dPPdM;
                    Mass2Liq=Range(-VValue[m_AutoVLE.m_iLiq], Mass2Liq, VValue[m_AutoVLE.m_iVap]);
                    VValue[m_AutoVLE.m_iLiq]+=Mass2Liq;
                    VValue[m_AutoVLE.m_iVap]-=Mass2Liq;
                    ClrStatesOK();

                    if (Valid(RqdT))
                      Set_totHf(totHf(som_ALL, RqdT, RqdP));
#if dbgAutoVLE
                    if (dbgAutoVLEEquil())
                      dbgp("A:%10.4f %10.4f %10.4f %10.4f", Mass2Liq, TotMass, VValue[m_AutoVLE.m_iLiq], VValue[m_AutoVLE.m_iVap]);
#endif
                    }
                  }
                }
              break;
              }
            case AVLESet_RelH:
            case AVLESet_Saturated:
            case AVLESet_SuperSat:
              {
              m_AutoVLE.m_dRelH=Range(0.0, m_AutoVLE.m_dRelH, 1.0);
              bool WithLiq=(m_AutoVLE.m_iSet==AVLESet_SuperSat);
              double PPScl=(m_AutoVLE.m_iSet==AVLESet_RelH)?(m_AutoVLE.m_dRelH):1.0;
              if (PPScl>ZeroLimit)
                {
                //ASSERT(Valid(RqdT));
                if (Valid(RqdT))
                  Set_totHf(totHf(som_ALL, RqdT, RqdP));
                if (!WithLiq)
                  {
                  VValue[m_AutoVLE.m_iLiq]=0;
                  ClrStatesOK();
                  }
                double TotMass=VValue[m_AutoVLE.m_iLiq]+VValue[m_AutoVLE.m_iVap];
                double VapP=SDB[m_AutoVLE.m_iVap].VapourP(m_iFidelity, Valid(RqdT)?RqdT:Temp());
                double PPVap1=PartialPress(m_AutoVLE.m_iVap, -1, Valid(RqdT)?RqdT:Temp());
                double MassGas=Mass(som_Gas);
                if ((MassGas-VValue[m_AutoVLE.m_iVap])<1.0e-6*MassGas)
                  {
                  }
                else
                  {
#if dbgAutoVLE
                  if (dbgAutoVLEEquil())
                    dbgp("A:%3i T:%10.4f PP:%10.4f VapP:%10.4f", Iter ,Valid(RqdT)?RqdT:Temp(), PPVap1, VapP);
#endif
                  if(!gs_EquilFinderTol.ConvergedVV(PPVap1, VapP*PPScl))
                    {
                    double DMassVap;
                    if (VValue[m_AutoVLE.m_iVap]>ZeroLimit)
                      DMassVap=(PPVap1>VapP ? -0.01:0.01)*VValue[m_AutoVLE.m_iVap];
                    else
                      DMassVap=0.001*Mass(som_Gas);
                    if (fabs(DMassVap)>ZeroLimit)
                      {
                      VLEConverged=false;
                      if (WithLiq)
                        VValue[m_AutoVLE.m_iLiq]-=DMassVap;
                      else
                        VValue[m_AutoVLE.m_iLiq]=0;
                      VValue[m_AutoVLE.m_iVap]+=DMassVap;
                      ClrStatesOK();
                      double PPVap2=PartialPress(m_AutoVLE.m_iVap, -1, Valid(RqdT)?RqdT:Temp());
                      if (WithLiq)
                        VValue[m_AutoVLE.m_iLiq]+=DMassVap;
                      VValue[m_AutoVLE.m_iVap]-=DMassVap;
                      double dPPdM=(PPVap2-PPVap1)/DMassVap;
                      double Mass2Vap=-(PPVap1-VapP*PPScl)/dPPdM;
                      Mass2Vap=Max(-VValue[m_AutoVLE.m_iVap], Mass2Vap);
                      if (WithLiq)
                        VValue[m_AutoVLE.m_iLiq]-=Mass2Vap;
                      VValue[m_AutoVLE.m_iVap]+=Mass2Vap;
                      ClrStatesOK();

                      if (Valid(RqdT))
                        Set_totHf(totHf(som_ALL, RqdT, RqdP));

#if dbgAutoVLE
                      if (dbgAutoVLEEquil())
                        dbgp("B:%10.4f %10.4f %10.4f %10.4f", Mass2Vap, TotMass, VValue[m_AutoVLE.m_iLiq], VValue[m_AutoVLE.m_iVap]);
#endif
                      }
                    }
                  }
                }
              else
                {
                VValue[m_AutoVLE.m_iLiq]=0;
                VValue[m_AutoVLE.m_iVap]=0;
                ClrStatesOK();
                if (Valid(RqdT))
                  Set_totHf(totHf(som_ALL, RqdT, RqdP));
                }
              break;
              }
            }
#if dbgAutoVLE
          if (dbgAutoVLEEquil())
            dbgpln(" %s", FullObjTag());
#endif
          }
#endif
        }

      if (!SolConverged)
        LogError(FullObjTag(), 0, "Solubility not Converged");
      if (!VLEConverged)
        LogError(FullObjTag(), 0, "AutoVLE not Converged");

      m_dwValidFlags |= SMVF_EquilOK;

      //dbgpln(" >> %10.2f [Iter:%i]", Valid(RqdT)?RqdT:Temp(), Iter);

      }
    m_iEquilibrateBusy--;
    }
  else
    {
    //dbgpln("SpModel::EquilibrateAll BUSY %s", FullObjTag());
    }
  //dbgpln("--------------------------------------------------------------");
  };

// --------------------------------------------------------------------------

void SpModel::DumpQualities()
  {
  for (int q=0; q<nQuals; q++)
    if (Quals[q].Exists())
      Quals[q].m_pQ->Dump();
  };

// --------------------------------------------------------------------------

static struct {long Inx; long Msk; LPCTSTR Nm; } VAList[] =
  {
    {0, VAMsk_Overides,      "Overides"    },
    {1, VAMsk_Thermo,        "Thermo"      },
    {2, VAMsk_Viscosity,     "Viscosity"   },
    {3, VAMsk_HeatValues,    "HeatValues"  },
    {-1 },
  };
static struct {long Inx; long Msk; LPCTSTR Nm; } VASpList[] =
  {
    //{0,  VAMsk_SpRaw,         "SpRaw"       }, // Always On
    //{1,  VAMsk_SpMass,        "SpMass"      }, // Always On
    {2, VAMsk_SpMole,         "SpMole"      },
    {3, VAMsk_SpVol,          "SpVol"       },
    {4, VAMsk_SpPartialP,     "SpPartialP"  },
    {5, VAMsk_SpNVol,         "SpNVol"      },
    {6, VAMsk_SpConc,         "SpConc"      },
    {7, VAMsk_SpElem,         "SpElem"      },
    {8, VAMsk_SpComp,         "SpComp"      },
    {-1 },
  };
static DDBValueLst DDBShw[] =
  {
    {0,  "Remove All"},
    {1,  "Include All"},
    {2,  "Include Local"},
    {}
  };

void SpModel::BuildDataDefn_AvailVars(DataDefnBlk & DDB)
  {
  if (DDB.BeginObject(this, "VarSelect", "SpModel_VarSelect", NULL, DDB_NoPage, -1, DDEF_TREECLOSED))
    {
    for (int j=0; VAList[j].Inx>=0; j++)
      {
      ASSERT(VAList[j].Msk==(1<<VAList[j].Inx));
      DDB.Byte((LPTSTR)VAList[j].Nm, "",   DC_,    "",      xidSpMdlVarsAvail+VAList[j].Inx,  this, isParmStopped|SetOnChange, DDBShw);
      }
    }
  DDB.EndObject();
  if (DDB.BeginObject(this, "SpVarSelect", "SpModel_SpVarSelect", NULL, DDB_NoPage, -1, DDEF_TREECLOSED))
    {
    for (int j=0; VASpList[j].Inx>=0; j++)
      {
      ASSERT(VASpList[j].Msk==(1<<VASpList[j].Inx));
      DDB.Byte((LPTSTR)VASpList[j].Nm, "",   DC_,    "",      xidSpMdlSpVarsAvail+VASpList[j].Inx,  this, isParmStopped|SetOnChange, DDBShw);
      }
    }
  DDB.EndObject();
  }

// --------------------------------------------------------------------------
flag  SpModel::BuildDataDefnGlobal(TagObjClass *pClass, TaggedObject * pOwner, DataDefnBlk & DDB)
  {
  //DDB.Add_PropertyMap(PropertyMap(), WhichProps, AsParms, xidSpUsrProps, pOwner, pOwner);
  return false;
  };
flag  SpModel::DataXchgGlobal(TagObjClass *pClass, TaggedObject * pOwner, DataChangeBlk & DCB)
  {
  return false;
  };

void SpModel::BuildDataDefn(DataDefnBlk & DDB)
  {
  if (DDB.BeginStruct(this))
    {
    DDB.Text   (" ");

    //should these be visible...???
    DDB.Double  ("Pressure",     "P",     DC_P,    "kPag",   xidSMPress,        this, isParm|InitHidden);
    DDB.Double  ("Temperature",  "T",     DC_T,    "C",      xidSMTemp,         this, isParm);
    DDB.Double  ("",             "H@T",   DC_HMs,  "kJ/kg",  xidSMmsH,          this, isParm|InitHidden);
    DDB.Byte    ("Usage",        "AsWhat",DC_,     "",       xidSMUsage,        this, isParm|InitHidden);
    //DDB.Bool    ("Editable",     "",      DC_,     "",       &m_bSQEditable,    this, isParm|InitHidden);

    for (int i=0; i<SVValueCount(); i++)
      {
      CSysVecItem &I=*SVI.Img(i);
      int MyiCnv = I.CnvIndex();
      LPSTR MysCnv = I.CnvString();
      DDB.Double(I.FullTag(),I.FullSym(), MyiCnv>=0?MyiCnv:DC_M, MyiCnv>=0?MysCnv:"kg", m_M.VPtr[i], this, isParm|noFileAtAll|HideIfZero|DDEF_ISSPECIE|(1/*NANsOK*/?NAN_OK:0));
      //DDB.Double(SVI.Img(i)->FullTag(),SVI.Img(i)->FullSym(), DC_M, "kg", &M[i], this, isParm|noFileAtAll|HideIfZero|DDEF_ISSPECIE);
      }
    }
  DDB.EndStruct();
  }

// --------------------------------------------------------------------------
static DDBFnParms Parms0[] = {{tt_NULL}};
static DDBFnParms Parms00[]= {{tt_Long,   (DC_),  "",    0    },
                              {tt_NULL}};
static DDBFnParms Parms1[] = {{tt_Double, (DC_T), "C",  Std_T},
                              {tt_Long,   (DC_),  "",    0    },
                              {tt_NULL}};
static DDBFnParms Parms2[] = {{tt_Double, (DC_T), "C",  Std_T},
                              {tt_Double, (DC_P), "kPa", Std_P},
                              {tt_Long,   (DC_),  "",    0    },
                              {tt_NULL}};

void SpModel::BuildDataDefn_Vars(DataDefnBlk & DDB)
  {
  DDB.Text("Density");
  DDB.Double  ("Density",       "Rho",      DC_Rho,   "kg/m^3",   xidSMRho,         this, isResult|noFile|noSnap);
  if (m_Ovr.bUse&(SPO_VDens|SPO_SDens|SPO_LDens))
    DDB.TagComment("Ovr");
  DDB.Double  ("SolidDensity",  "SRho",     DC_Rho,   "kg/m^3",   xidSMSRho,        this, isResult|noFile|noSnap|InitHidden);
  if (m_Ovr.bUse&SPO_SDens)
    DDB.TagComment("Ovr");
  DDB.Double  ("LiquidDensity", "LRho",     DC_Rho,   "kg/m^3",   xidSMLRho,        this, isResult|noFile|noSnap/*|InitHidden*/);
  if (m_Ovr.bUse&SPO_LDens)
    DDB.TagComment("Ovr");
  DDB.Double  ("VapourDensity", "VRho",     DC_Rho,   "kg/m^3",   xidSMVRho,        this, isResult|noFile|noSnap|InitHidden);
  if (m_Ovr.bUse&SPO_VDens)
    DDB.TagComment("Ovr");
  DDB.Double  ("SlurryDensity", "SLRho",    DC_Rho,   "kg/m^3",   xidSMSLRho,       this, isResult|noFile|noSnap|InitHidden);
  if (m_Ovr.bUse&(SPO_SDens|SPO_LDens))
    DDB.TagComment("Ovr");
  if (SDB.m_DensCorrSps.GetSize()>0)
    {
    Strng Tg1,Tg2;
    for (int j=0; j<SDB.m_DensCorrSps.GetSize() && xidRhoSoln1+j<=xidRhoSolnLast; j++)
      {
      int iSolvent=SDB.m_DensCorrSps[j];
      CSpecie & Solvent=SDB[iSolvent];
      Tg1.Set("CorrDensity_%s", Solvent.SymOrTag());
      Tg2.Set("CorrRho_%s", Solvent.SymOrTag());
      DDB.Double(Tg1(), Tg2(), DC_Rho, "kg/m^3", xidRhoSoln1+j, this, isResult|noFile|noSnap|InitHidden);
      if (m_Ovr.bUse&SPO_LDens)
        DDB.TagComment("Ovr");
      }
    }

  DDB.Text("Mass Fractions");
  DDB.Double  ("SolidFrac",     "Sf",       DC_Frac,  "%",        xidSMSFrac,       this, isResult|noFile|noSnap);
  DDB.Double  ("LiquidFrac",    "Lf",       DC_Frac,  "%",        xidSMLFrac,       this, isResult|noFile|noSnap);
  DDB.Double  ("VapourFrac",    "Vf",       DC_Frac,  "%",        xidSMVFrac,       this, isResult|noFile|noSnap);

  DDB.Text("");
  //DDB.Text("Molar Fractions");
  DDB.Double  ("MlSolidFrac",   "MlSf",     DC_Frac,  "%",        xidSMMlSFrac,     this, isResult|noFile|noSnap|InitHidden);
  DDB.Double  ("MlLiquidFrac",  "MlLf",     DC_Frac,  "%",        xidSMMlLFrac,     this, isResult|noFile|noSnap|InitHidden);
  DDB.Double  ("MlVapourFrac",  "MlVf",     DC_Frac,  "%",        xidSMMlVFrac,     this, isResult|noFile|noSnap|InitHidden);
  DDB.Double  ("MoleWt",        "",         DC_,      "",         xidSMMoleWt,      this, isResult|noFile|noSnap|InitHidden);

  Strng Tg(DDB.BlockTag());
  Tg+="..";

  DDB.Page(Tg(), DDB_OptPage);
  if (((sm_VarsAvail|m_VarsAvail) & VAMsk_Thermo) && !HeatSkipMethod())
    {
    DDB.Text("Specific Heat");
    DDB.Double  ("",              "Cp@0",     DC_CpMs,    "kJ/kg.C",  xidSMmsCp0,       this, isResult|noFile|noSnap|InitHidden);
    DDB.Double  ("Cp",            "Cp@T",     DC_CpMs,    "kJ/kg.C",  xidSMmsCp,        this, isResult|noFile|noSnap);
    if (m_Ovr.bUse&(SPO_VCp|SPO_SCp|SPO_LCp))
      DDB.TagComment("Ovr");
    //DDB.Double  ("",              "Cp0~T",    DC_CpMs,    "kJ/kg.C",  xidSMmsCp0T,      this, isResult|noFile|noSnap);
    //if (Ovr.bUse&(SPO_VCp|SPO_SCp|SPO_LCp))
    //  DDB.TagComment("Ovr");
    //if (dbgtestfileopen())
    //  {
    DDB.Double  ("",              "UsrDefT",  DC_T,     "C",        &dUsrDefT,        this, isParm|noFile|noSnap|InitHidden);
    DDB.Double  ("",              "UsrDefP",  DC_P,     "kPa",      &dUsrDefP,        this, isParm|NAN_OK|noFile|noSnap|InitHidden);
    if (!Valid(dUsrDefP))
      DDB.TagComment("SaturationP");
    DDB.Double  ("",              "Cp@U",     DC_CpMs,    "kJ/kg.C",  xidSMmsCpUt,      this, isResult|noFile|noSnap|InitHidden);

    DDB.Double  ("",            "totHs@U",    DC_Pwr,   "kJ/s",    xidSMtotHUt,        this, isResult|noFile|noSnap|InitHidden);
    DDB.Double  ("",            "totHz@U",    DC_Pwr,   "kJ/s",    xidSMtotHzUt,       this, isResult|noFile|noSnap);
    DDB.Double  ("",            "totHf@U",    DC_Pwr,   "kJ/s",    xidSMtotHfUt,       this, isResult|noFile|noSnap|InitHidden);

    //DDB.Double  ("",              "CpU~T",    DC_CpMs,    "kJ/kg.C",  xidSMmsCpUtT,     this, isResult|noFile|noSnap|InitHidden);
    //  }
    DDB.Double  ("",              "SmsCp@T",  DC_CpMs,    "kJ/kg.C",  xidSMSmsCp,       this, isResult|noFile|noSnap|InitHidden);
    if (m_Ovr.bUse&(SPO_SCp))
      DDB.TagComment("Ovr");
    DDB.Double  ("",              "LmsCp@T",  DC_CpMs,    "kJ/kg.C",  xidSMLmsCp,       this, isResult|noFile|noSnap|InitHidden);
    if (m_Ovr.bUse&(SPO_LCp))
      DDB.TagComment("Ovr");
    DDB.Double  ("",              "VmsCp@T",  DC_CpMs,    "kJ/kg.C",  xidSMVmsCp,       this, isResult|noFile|noSnap|InitHidden);
    if (m_Ovr.bUse&(SPO_VCp))
      DDB.TagComment("Ovr");
    DDB.Double  ("",              "SLmsCp@T", DC_CpMs,    "kJ/kg.C",  xidSMSLmsCp,      this, isResult|noFile|noSnap|InitHidden);
    if (m_Ovr.bUse&(SPO_SCp|SPO_LCp))
      DDB.TagComment("Ovr");

    DDB.Text("");
    DDB.Double  ("",              "SmsHs@T",  DC_HMs,  "kJ/kg",    xidSMSmsH,        this, isResult|noFile|noSnap|InitHidden);
    DDB.Double  ("",              "LmsHs@T",  DC_HMs,  "kJ/kg",    xidSMLmsH,        this, isResult|noFile|noSnap|InitHidden);
    DDB.Double  ("",              "VmsHs@T",  DC_HMs,  "kJ/kg",    xidSMVmsH,        this, isResult|noFile|noSnap|InitHidden);
    DDB.Double  ("",              "SLmsHs@T", DC_HMs,  "kJ/kg",    xidSMSLmsH,       this, isResult|noFile|noSnap|InitHidden);

    DDB.Double  ("",              "Hs@T",     DC_HMs,  "kJ/kg",    xidSMmsH,         this, isResult|InitHidden);
    DDB.Double  ("",              "Hz@T",     DC_HMs,  "kJ/kg",    xidSMmsHz,        this, isResult);
    DDB.Double  ("",              "Hf@T",     DC_HMs,  "kJ/kg",    xidSMmsHf,        this, isResult|noFile|noSnap|InitHidden);
    DDB.Double  ("",              "Hf@0",     DC_HMs,  "kJ/kg",    xidSMmsHf0,       this, isResult|noFile|noSnap|InitHidden);
  #if WithExtraEntropy_Sf
    DDB.Double  ("",              "Sf@T",     DC_SMs,  "kJ/kg.K",  xidSMmsSf,        this, isResult|noFile|noSnap|InitHidden);
    DDB.Double  ("",              "Sf@0",     DC_SMs,  "kJ/kg.K",  xidSMmsSf0,       this, isResult|noFile|noSnap|InitHidden);
  #endif
    switch (m_iUsage)
      {
      case SPMU_Flow:
  #if WithExtra_totHs
        DDB.Double  ("",            "totHs@AtmP", DC_Pwr, "kJ/s",     xidSMtotHAtmP,    this, isResult|noFile|noSnap|InitHidden);
        DDB.Double  ("",            "totHs@SatP", DC_Pwr, "kJ/s",     xidSMtotHSatP,    this, isResult|noFile|noSnap|InitHidden);
  #endif
        DDB.Double  ("",            "totHs",    DC_Pwr,   "kJ/s",     xidSMtotH,        this, isResult|noFile|noSnap|InitHidden);
        DDB.Double  ("",            "totHz",    DC_Pwr,   "kJ/s",     xidSMtotHz,       this, isResult|noFile|noSnap);
        DDB.Double  ("",            "totHf@T",  DC_Pwr,   "kJ/s",     xidSMtotHf,       this, isResult|noFile|noSnap|InitHidden);
        DDB.Double  ("",            "totHf@0",  DC_Pwr,   "kJ/s",     xidSMtotHf0,      this, isResult|noFile|noSnap|InitHidden);
        break;
      case SPMU_Mass:
  #if WithExtra_totHs
        DDB.Double  ("",            "totHs@AtmP", DC_E,   "kJ",       xidSMtotHAtmP,    this, isResult|noFile|noSnap|InitHidden);
        DDB.Double  ("",            "totHs@SatP", DC_E,   "kJ",       xidSMtotHSatP,    this, isResult|noFile|noSnap|InitHidden);
  #endif
        DDB.Double  ("",            "totHs",    DC_E,     "kJ",       xidSMtotH,        this, isResult|noFile|noSnap|InitHidden);
        DDB.Double  ("",            "totHz",    DC_E,     "kJ",       xidSMtotHz,       this, isResult|noFile|noSnap);
        DDB.Double  ("",            "totHf@T",  DC_E,     "kJ",       xidSMtotHf,       this, isResult|noFile|noSnap|InitHidden);
        DDB.Double  ("",            "totHf@0",  DC_E,     "kJ",       xidSMtotHf0,      this, isResult|noFile|noSnap|InitHidden);
        break;
      }

    if (1)
      {
      DDB.FnDouble  ("",            "totHs.Fn",    DC_Pwr,  "kJ/s",  xidSMtotHsFn,     this, 0, Parms2);
      DDB.FnDouble  ("",            "totHz.Fn",    DC_Pwr,  "kJ/s",  xidSMtotHzFn,     this, 0, Parms2);
      DDB.FnDouble  ("",            "totHf.Fn",    DC_Pwr,  "kJ/s",  xidSMtotHfFn,     this, 0, Parms2);
      }
    DDB.Text("");
    }

  if (!HeatSkipMethod())
    {
    DDB.Double  ("CpCv",          "",         DC_,      "",         xidSMCpCv,        this, isResult|noFileAtAll|InitHidden);
    DDB.String  ("Saturation",    "",         DC_,      "",         xidSMSatComp,     this, isParm|SetOnChange, sm_DDBFlashable());
    DDB.Double  ("WaterSatT@P",   "",         DC_T,     "C",        xidSMPureSatTemp, this, isResult|noFileAtAll|InitHidden);
    DDB.Double  ("SatPP@T",       "",         DC_P,     "kPag",     xidSMSatPPress,   this, isResult|noFileAtAll);
    DDB.Double  ("SatT@PP",       "",         DC_T,     "C",        xidSMSatTempPP,   this, isResult|noFileAtAll|NAN_OK);
    //DDB.Double  ("SatT@P",        "",         DC_T,     "C",        xidSMSatTempP,    this, isResult|noFileAtAll);
    //DDB.Double  ("SatP@T",        "",         DC_P,     "kPag",     xidSMSatPress,    this, isResult|noFileAtAll);
    DDB.Double  ("BPE",           "",         DC_dT,    "C",        xidSMBoilPtEle,   this, isResult|noFileAtAll|InitHidden);
    if (m_Ovr.bUse&(SPO_BPEle))
      DDB.TagComment("Ovr");
    DDB.Double  ("",              "LatHtVap", DC_HMs,   "kJ/kg",    xidSMmsLatHtVap,  this, isResult|noFile|noSnap);
    DDB.Text("");
    }

  if ((sm_VarsAvail|m_VarsAvail)& VAMsk_Viscosity)
    {
    DDB.Page(Tg(), DDB_OptPage);
    DDB.Text("Transport Properties:");
    DDB.Double  ("VViscosity",      "",              DC_Visc,     "cP",     xidVVisc,      this, isResult|noFileAtAll);
    DDB.Double  ("LViscosity",      "",              DC_Visc,     "cP",     xidLVisc,      this, isResult|noFileAtAll);
    DDB.Double  ("VKinematicVisc",  "",              DC_KinVisc,  "m^2/s",  xidVKVisc,     this, isResult|noFileAtAll);
    DDB.Double  ("LKinematicVisc",  "",              DC_KinVisc,  "m^2/s",  xidLKVisc,     this, isResult|noFileAtAll);
    DDB.Double  ("VThermalCond",    "",              DC_HCond,    "kW/m.K", xidVThermCond, this, isResult|noFileAtAll);
    DDB.Double  ("LThermalCond",    "",              DC_HCond,    "kW/m.K", xidLThermCond, this, isResult|noFileAtAll);
    DDB.Double  ("VPrandtlNo",      "",              DC_,         "",       xidVPrandtl,   this, isResult|noFileAtAll);
    DDB.Double  ("LPrandtlNo",      "",              DC_,         "",       xidLPrandtl,   this, isResult|noFileAtAll);
    DDB.Double  ("SurfaceTens",     "LSurfaceTens",  DC_SurfTens, "N/m",    xidLSurfT,     this, isResult|noFileAtAll);
    //DDB.Double  ("LSurfaceTens",  "",  DC_SurfTens, "N/m",    xidSurfT,     this, isResult|noFile|noSnap|InitHidden);
    DDB.Text("");
    }

  if (!DDB.ForFileSnpScn())
    {
    if (((sm_VarsAvail|m_VarsAvail) & VAMsk_HeatValues) && !HeatSkipMethod())
      {
      DDB.Page(Tg(), DDB_OptPage);//DDB.Page("HV");
      DDB.Text("Heating Values:");
      DDB.Double  ("HeatValueHiM",    "HVHiM",    DC_HMs, "MJ/kg",    xidSpHeatValueHiM,    this, isResult|noFileAtAll);
      DDB.Double  ("HeatValueHiMl",   "HVHiMl",   DC_HMl, "MJ/kmol",  xidSpHeatValueHiMl,   this, isResult|noFileAtAll);
      DDB.Double  ("HeatValueHiV",    "HVHiV",    DC_HV,  "MJ/m^3",   xidSpHeatValueHiV,    this, isResult|noFileAtAll);
      DDB.Double  ("HeatValueHiNV",   "HVHiNV",   DC_HNV, "MJ/Nm^3",  xidSpHeatValueHiNV,   this, isResult|noFileAtAll);
      DDB.Double  ("HeatValueLoM",    "HVLoM",    DC_HMs, "MJ/kg",    xidSpHeatValueLoM,    this, isResult|noFileAtAll);
      DDB.Double  ("HeatValueLoMl",   "HVLoMl",   DC_HMl, "MJ/kmol",  xidSpHeatValueLoMl,   this, isResult|noFileAtAll);
      DDB.Double  ("HeatValueLoV",    "HVLoV",    DC_HV,  "MJ/m^3",   xidSpHeatValueLoV,    this, isResult|noFileAtAll);
      DDB.Double  ("HeatValueLoNV",   "HVLoNV",   DC_HNV, "MJ/Nm^3",  xidSpHeatValueLoNV,   this, isResult|noFileAtAll);
      DDB.Double  ("HeatValueHiM25",  "HVHiM25",  DC_HMs, "MJ/kg",    xidSpHeatValueHiM25,  this, isResult|noFileAtAll);
      DDB.Double  ("HeatValueHiMl25", "HVHiMl25", DC_HMl, "MJ/kmol",  xidSpHeatValueHiMl25, this, isResult|noFileAtAll);
      DDB.Double  ("HeatValueHiV25",  "HVHiV25",  DC_HV,  "MJ/m^3",   xidSpHeatValueHiV25,  this, isResult|noFileAtAll);
      DDB.Double  ("HeatValueHiNV25", "HVHiNV25", DC_HNV, "MJ/Nm^3",  xidSpHeatValueHiNV25, this, isResult|noFileAtAll);
      DDB.Double  ("HeatValueLoM25",  "HVLoM25",  DC_HMs, "MJ/kg",    xidSpHeatValueLoM25,  this, isResult|noFileAtAll);
      DDB.Double  ("HeatValueLoMl25", "HVLoMl25", DC_HMl, "MJ/kmol",  xidSpHeatValueLoMl25, this, isResult|noFileAtAll);
      DDB.Double  ("HeatValueLoV25",  "HVLoV25",  DC_HV,  "MJ/m^3",   xidSpHeatValueLoV25,  this, isResult|noFileAtAll);
      DDB.Double  ("HeatValueLoNV25", "HVLoNV25", DC_HNV, "MJ/Nm^3",  xidSpHeatValueLoNV25, this, isResult|noFileAtAll);
      DDB.Text("");
      }

    if (0)  // ToGoBack
      {
      DDB.Text("Eqn Of State:");
      static DDBValueLst DDBEqnOfState[]=
        {
        // {eScdEOS_Null,   "None"}
          {eScdEOS_SRK, "SoaveRedlichKwong"},
          {0},
        };

      static DDBValueLst DDBMixtureRule[]=
        {
        // {eScdGMR_Null,   "None"}
          {eScdGMR_Simple, "Simple"},
          {0},
        };

      DDB.Long    ("EqnOfState",      "",         DC_,    "",         xidSpEqnOfState,      this, isParmStopped, DDBEqnOfState);
      DDB.Long    ("MixtureRule",     "",         DC_,    "",         xidSpMixtureRule,     this, isParmStopped, DDBMixtureRule);
      DDB.Double  ("Alpha",           "",         DC_,    "",         xidSpMixtureAlpha,    this, isParmStopped);

      DDB.Double  ("TCritical",       "Tc",       DC_T,   "C",        xidSpMixtureTc,       this, isResult|noFile|noSnap|InitHidden);
      DDB.Double  ("PCritica",        "Pc",       DC_P,   "MPa",      xidSpMixturePc,       this, isResult|noFile|noSnap|InitHidden);
      DDB.Double  ("VCritica",        "Vc",       DC_Vol, "m^3",      xidSpMixtureVc,       this, isResult|noFile|noSnap|InitHidden);
      DDB.Double  ("AcentricFactor",  "Ac",       DC_,    "",         xidSpMixtureAc,       this, isResult|noFile|noSnap|InitHidden);
      DDB.Double  ("dHIdeal",         "",         DC_HMs, "kJ/kg",    xidSpdHIdeal,         this, isResult|noFile|noSnap|InitHidden);
      DDB.Double  ("dSIdeal",         "",         DC_SMs, "kJ/kg.K",  xidSpdSIdeal,         this, isResult|noFile|noSnap|InitHidden);
      DDB.Double  ("dHResidual",      "",         DC_HMs, "kJ/kg",    xidSpdHResidual,      this, isResult|noFile|noSnap|InitHidden);
      DDB.Double  ("dSResidual",      "",         DC_SMs, "kJ/kg.K",  xidSpdSResidual,      this, isResult|noFile|noSnap|InitHidden);
      DDB.Text("");
      }
    }
#if WITHAUTOVLE
  if (AutoVLEOn())// && !DDB.ForFileSnpScn())
    {
    //DDB.Text("Humidity:");
    if (DDB.BeginStruct(this, "Humidity"))
      {
      if (1)//m_iEquilAct==SQA_Create)
        {
        //if (DDB.BeginStruct(this, "Rqd"))
          {
          static DDBValueLst DDBHSet[]=
            {
              {AVLESet_None,      "None"},
              //{AVLESet_SpecH,     "Specific"},
              {AVLESet_RelH,      "Relative"},
              //{AVLESet_DewPt,     "DewPt"},
              {AVLESet_Saturated, "Saturated"},
              {AVLESet_SuperSat,  "SuperSaturated"},
              {0},
            };

          DDB.Byte  ("Rqd.What",     "", DC_,           "",             &m_AutoVLE.m_iSet,    this, m_iEquilAct==SQA_Create ? isParm|SetOnChange:isResult, DDBHSet);
          if (m_iEquilAct==SQA_Create)
            {
            DDB.Visibility(NSHM_All, m_AutoVLE.m_iSet==AVLESet_SpecH);
            DDB.Double("Rqd.Specific", "", DC_Humidity ,  "kg(l)/kg(v)",  &m_AutoVLE.m_dSpecH,  this, isParm);
            DDB.Visibility(NSHM_All, m_AutoVLE.m_iSet==AVLESet_RelH);
            DDB.Double("Rqd.Relative", "", DC_Frac, "%",                  &m_AutoVLE.m_dRelH,   this, isParm);
            DDB.Visibility(NSHM_All, m_AutoVLE.m_iSet==AVLESet_DewPt);
            DDB.Double("Rqd.DewPoint", "", DC_T,    "C",                  &m_AutoVLE.m_dDewPt,  this, isParm);
            }
          DDB.Visibility(NSHM_All);
          }
        //DDB.EndStruct();
        }
      DDB.Double("Specific", "", DC_,     "",   xidHumiditySpecific,  this, isResult|noFile|noSnap);
      DDB.Double("Relative", "", DC_Frac, "%",  xidHumidityRelative,  this, isResult|noFile|noSnap);
      DDB.Double("DewPoint", "", DC_T,    "C",  xidHumidityDewPt,     this, isResult|noFile|noSnap);
      }
    DDB.EndStruct();
    DDB.Text("");
    }
#endif
  DDB.Byte("", "OverrideActive", DC_, "", xidOvrAnyOn, this, noFile|noSnap, DDBYesNo);//where is a good "visible" place for this?
  };

// --------------------------------------------------------------------------

void SpModel::BuildDataDefn_UsrProps(DataDefnBlk & DDB, ULONG WhichProps/*=MP_UserProp*/, flag AsParms/*=true*/)
  {
  DDB.Add_PropertyMap(PropertyMap(), WhichProps, AsParms, xidSpUsrProps, this, this);
  }

// --------------------------------------------------------------------------

static DDBValueLst DDBX[]=
  {
    {SQA_Xfer, "-"},
    {SQA_Create, "Set"},
    {SQA_Remove, "Clear"},
    {0},
  };

void SpModel::BuildDataDefn_RqdProps(DataDefnBlk & DDB)
  {
  int n=0;

  DWORD Flag=0;
  Flag=isParm;

  if (PrjFileVerNo()<22)
    {
    DDB.Byte("", "VDens_Overide",    DC_, "", xidOvrVDens,     this, Flag, DDBX);
    DDB.Byte("", "LDens_Overide",    DC_, "", xidOvrLDens,     this, Flag, DDBX);
    DDB.Byte("", "SDens_Overide",    DC_, "", xidOvrSDens,     this, Flag, DDBX);
    DDB.Byte("", "VVisc_Overide",    DC_, "", xidOvrVDynVisc,  this, Flag, DDBX);
    DDB.Byte("", "LVisc_Overide",    DC_, "", xidOvrLDynVisc,  this, Flag, DDBX);
    DDB.Byte("", "VThermCd_Overide", DC_, "", xidOvrVThermCd,  this, Flag, DDBX);
    DDB.Byte("", "LThermCd_Overide", DC_, "", xidOvrLThermCd,  this, Flag, DDBX);
    DDB.Byte("", "SurfT_Overide",    DC_, "", xidOvrSurfT,     this, Flag, DDBX);
    DDB.Byte("", "BPE_Overide",      DC_, "", xidOvrBPEle,     this, Flag, DDBX);

    DDB.Float("", "VDens_Rqd",    DC_Rho,       "kg/m^3", &m_Ovr.fVDens, this, Flag);
    DDB.Float("", "LDens_Rqd",    DC_Rho,       "kg/m^3", &m_Ovr.fLDens, this, Flag);
    DDB.Float("", "SDens_Rqd",    DC_Rho,       "kg/m^3", &m_Ovr.fSDens, this, Flag);
    DDB.Float("", "VVisc_Rqd",    DC_Visc,      "cP",     &m_Ovr.fVDynVisc, this, Flag);
    DDB.Float("", "LVisc_Rqd",    DC_Visc,      "cP",     &m_Ovr.fLDynVisc, this, Flag);
    DDB.Float("", "VThermCd_Rqd", DC_HCond,     "kW/m.K", &m_Ovr.fVThermCnd, this, Flag);
    DDB.Float("", "LThermCd_Rqd", DC_HCond,     "kW/m.K", &m_Ovr.fLThermCnd, this, Flag);
    DDB.Float("", "SurfT_Rqd",    DC_SurfTens,  "N/m",    &m_Ovr.fSurfT, this, Flag);
    DDB.Float("", "BPE_Rqd",      DC_dT,        "C",      &m_Ovr.fBPEle, this, Flag);
    }
  else
    {
    if ((sm_VarsAvail|m_VarsAvail)& VAMsk_Overides)
      {
      int DoIt;
      if (PrjFileVerNo()>=45)
        DoIt=DDB.BeginObject(this, "RqdProps", "SpRqdProps", NULL, DDB_NoPage, -1, MarkerClosed);
      else
        DoIt=DDB.BeginStruct(this, "Override", "", DDB_NoPage,-1,MarkerClosed);
      if (DoIt)
        {
        DDB.Byte("", "Active",    DC_, "", xidOvrAnyOn,    this, noFile|noSnap, DDBYesNo);
        DDB.Byte("", "VCp",       DC_, "", xidOvrVCp,      this, Flag, DDBX);
        DDB.Byte("", "LCp",       DC_, "", xidOvrLCp,      this, Flag, DDBX);
        DDB.Byte("", "SCp",       DC_, "", xidOvrSCp,      this, Flag, DDBX);
        DDB.Byte("", "VDens",     DC_, "", xidOvrVDens,    this, Flag, DDBX);
        DDB.Byte("", "LDens",     DC_, "", xidOvrLDens,    this, Flag, DDBX);
        DDB.Byte("", "SDens",     DC_, "", xidOvrSDens,    this, Flag, DDBX);
        DDB.Byte("", "VVisc",     DC_, "", xidOvrVDynVisc, this, Flag, DDBX);
        DDB.Byte("", "LVisc",     DC_, "", xidOvrLDynVisc, this, Flag, DDBX);
        DDB.Byte("", "VThermCd",  DC_, "", xidOvrVThermCd, this, Flag, DDBX);
        DDB.Byte("", "LThermCd",  DC_, "", xidOvrLThermCd, this, Flag, DDBX);
        DDB.Byte("", "SurfT",     DC_, "", xidOvrSurfT,    this, Flag, DDBX);
        DDB.Byte("", "BPE",       DC_, "", xidOvrBPEle,    this, Flag, DDBX);
        //  DDB.Text("");

        if (m_Ovr.bAdd & SPO_VCp)
          DDB.Float("", "VCp_Rqd",      DC_CpMs,       "kJ/kg.C", &m_Ovr.fVCp, this, Flag);
        if (m_Ovr.bAdd & SPO_LCp)
          DDB.Float("", "LCp_Rqd",      DC_CpMs,       "kJ/kg.C", &m_Ovr.fLCp, this, Flag);
        if (m_Ovr.bAdd & SPO_SCp)
          DDB.Float("", "SCp_Rqd",      DC_CpMs,       "kJ/kg.C", &m_Ovr.fSCp, this, Flag);
        if (m_Ovr.bAdd & SPO_VDens)
          DDB.Float("", "VDens_Rqd",    DC_Rho,       "kg/m^3", &m_Ovr.fVDens, this, Flag);
        if (m_Ovr.bAdd & SPO_LDens)
          DDB.Float("", "LDens_Rqd",    DC_Rho,       "kg/m^3", &m_Ovr.fLDens, this, Flag);
        if (m_Ovr.bAdd & SPO_SDens)
          DDB.Float("", "SDens_Rqd",    DC_Rho,       "kg/m^3", &m_Ovr.fSDens, this, Flag);
        if (m_Ovr.bAdd & SPO_VDynVisc)
          DDB.Float("", "VVisc_Rqd",    DC_Visc,      "cP",     &m_Ovr.fVDynVisc, this, Flag);
        if (m_Ovr.bAdd & SPO_LDynVisc)
          DDB.Float("", "LVisc_Rqd",    DC_Visc,      "cP",     &m_Ovr.fLDynVisc, this, Flag);
        if (m_Ovr.bAdd & SPO_VThermCnd)
          DDB.Float("", "VThermCd_Rqd", DC_HCond,     "kW/m.K", &m_Ovr.fVThermCnd, this, Flag);
        if (m_Ovr.bAdd & SPO_LThermCnd)
          DDB.Float("", "LThermCd_Rqd", DC_HCond,     "kW/m.K", &m_Ovr.fLThermCnd, this, Flag);
        if (m_Ovr.bAdd & SPO_SurfT)
          DDB.Float("", "SurfT_Rqd",    DC_SurfTens,  "N/m",    &m_Ovr.fSurfT, this, Flag);
        if (m_Ovr.bAdd & SPO_BPEle)
          DDB.Float("", "BPE_Rqd",      DC_dT,        "C",      &m_Ovr.fBPEle, this, Flag);
        }
      if (PrjFileVerNo()>=45)
        DDB.EndObject();
      else
        DDB.EndStruct();
      }
    }
  if (DDB.ForFiling())
    DDB.Long("", "Use",   DC_,        "",     xidSpUseFlgs, this, Flag);

  }
// --------------------------------------------------------------------------

static PhMask SVV_BasisMask(long Handle2)
  {
  SV_ViewBasis VB=(SV_ViewBasis)SpPhase2(Handle2);
  int          s=SpIndex2(Handle2);
  PhMask  M=SVI.Img(s)->ViewBasisMask(VB);


  return SVI.Img(SpIndex2(Handle2))->ViewBasisMask((SV_ViewBasis)SpPhase2(Handle2));
  };

// --------------------------------------------------------------------------

CSpView & SpModel::WhichView(eSVForce Force)
  {
  if (Force==SVForce_Lcl)
    {
    //dbgpln("Force %i", Vw.m_iView);
    return m_Vw;
    }
  if (m_pGlblView && m_pGlblView->m_bOn && Force==SVForce_Glbl)
    {
    //dbgpln("Global %i", m_pGlblView->m_iView);
    return *m_pGlblView;
    }

  CSpView & V = (m_pGlblView && m_pGlblView->m_bOn) ? *m_pGlblView : m_Vw;

  //dbgpln("Global %i", V.m_iView);

  return V;
  };

// --------------------------------------------------------------------------

void SpModel::SetView(SV_View View, eSVForce Force)
  {
  WhichView(Force).m_iView = View;
  StructureChanged(this);
  };

// --------------------------------------------------------------------------

void SpModel::SetViewBasis(SV_ViewBasis ViewBasis, eSVForce Force)
  {
  WhichView(Force).m_iViewBasis = ViewBasis;
  StructureChanged(this);
  };

// --------------------------------------------------------------------------

void SpModel::AddSpView(DataDefnBlk & DDB, const BDDSpecies_Blk * Blks, int nBlks, DDEF_Flags Flags)
  {
  DDBValueLstMem DDB0;
  DDB0.Empty();
  for (int v=0; v<nBlks; v++)
    {
    if (((sm_SpVarsAvail|m_SpVarsAvail) & gs_ViewInfo[Blks[v].m_iView].m_VAMsk)!=0)
      DDB0.Add(Blks[v].m_iView, gs_ViewInfo[Blks[v].m_iView].FullName);
    }
  if (DDB.ForFileSnpScn())
    {
    DDB.Byte("ViewL", "", DC_,    "",    xidSMViewL, this, isParm|Flags|DupHandlesOk, DDB0());// Allow dup handles
    DDB.Byte("ViewG", "", DC_,    "",    xidSMViewG, this, isParm|Flags|DupHandlesOk, DDB0());// Allow dup handles
    }
  else
    DDB.Byte("View",  "", DC_,    "",    xidSMView, this, isParm|Flags|DupHandlesOk|SetOnChange, DDB0());// Allow dup handles
  }

// --------------------------------------------------------------------------

void SpModel::AddSpViewBasis(DataDefnBlk & DDB, const BDDSpecies_Blk * Blks, int nBlks, SV_View RqdVw, DDEF_Flags Flags)
  {
  //DDBValueLstMem DDB0;
  //DDB0.Empty();
  //for (int v=0; v<nBlks; v++)
  //  DDB0.Add(Blks[v].m_iView, gs_ViewInfo[Blks[v].m_iView].FullName);
  //if (DDB.ForFileSnpScn())
  //  {
  //  DDB.Byte("ViewL", "", DC_,    "",    xidSMViewL, this, isParm|Flags|DupHandlesOk, DDB0());// Allow dup handles
  //  DDB.Byte("ViewG", "", DC_,    "",    xidSMViewG, this, isParm|Flags|DupHandlesOk, DDB0());// Allow dup handles
  //  }
  //else
  //  DDB.Byte("View",  "", DC_,    "",    xidSMView, this, isParm|Flags|DupHandlesOk|SetOnChange, DDB0());// Allow dup handles

  switch (RqdVw)
    {
    case SVV_AsMass:
    case SVV_AsMassFlow:
    case SVV_AsMassFrac:
    case SVV_AsMole:
    case SVV_AsMoleFlow:
    case SVV_AsMoleFrac:
    case SVV_AsVol:
    case SVV_AsVolFlow:
    case SVV_AsVolFrac:
    case SVV_AsNVol:
    case SVV_AsNVolFlow:
    case SVV_AsNVolFrac:
    case SVV_AsSpcConc:
      {
      static DDBValueLst DDBPhVw[] =
        {
          {SVVB_All,   "All"},
          {SVVB_Occ,   "Phase"},
          {SVVB_Phase, "IndividualPhase"},
          {0},
        };
      DDB.Byte("Basis", "",   DC_,    "",    xidSMViewBasisMass, this, isParm|Flags|DupHandlesOk|SetOnChange, DDBPhVw);// Allow dup handles
      break;
      }
    case SVV_AsElem:
    case SVV_AsElemFlow:
    case SVV_AsElemFrac:
    case SVV_AsElemConc:
    case SVV_AsElemConc25:
    case SVV_AsComp:
    case SVV_AsCompFlow:
    case SVV_AsCompFrac:
    case SVV_AsCompConc:
    case SVV_AsCompConc25:
      {
      static DDBValueLst DDB1[]=
        {
          {SVVB_All,    "All"},
          {SVVB_Sol,    "Solids"},
          {SVVB_Liq,    "Liquids"},
          {SVVB_Gas,    "Vapours"},
          {0}
        };
      DDB.Byte("Phase", "",   DC_,    "",    xidSMViewBasisElem, this, isParm|Flags|DupHandlesOk|SetOnChange,DDB1);// Allow dup handles
      break;
      }
    default:;
    }

  if (this->m_pGlblView)
    DDB.CheckBoxBtn("GlobalView", "",    DC_,    "",    xidSMViewGlbl, this, isParm|Flags|DupHandlesOk/*|noFile|noScenario|noSnap*/);// Allow dup handles
  switch (RqdVw)
    {
    case SVV_AsMassFrac:
    case SVV_AsMoleFrac:
    case SVV_AsVolFrac:
    case SVV_AsNVolFrac:
    case SVV_AsSpcConc:
      {
      static DDBValueLst DDBStrat[]=
        {
          {FUS_Largest,       "Largest"},
          {FUS_BottomUp,      "Bottom_Up"},
          //{xFUS_TopDown,       "Top Down"},
          //{xFUS_BelowCurrent,  "Below Current"},
          {0}
        };
      DDB.Long("FixupStrategy", "",   DC_,    "",    &sm_iFixupStrategy, this, isParm|Flags|DupHandlesOk|SetOnChange,DDBStrat);// Allow dup handles
      // FALL THRU
      }
    case SVV_AsMass:
    case SVV_AsMassFlow:
    case SVV_AsMole:
    case SVV_AsMoleFlow:
    case SVV_AsVol:
    case SVV_AsVolFlow:
    case SVV_AsNVol:
    case SVV_AsNVolFlow:
      {
      if (SVI.GetShowGroups())
        {
        DDBValueLstMem DDBTemp;
        DDBTemp.Add(" ");
        POSITION Pos=SVI.m_Templates.GetHeadPosition();
        while (Pos)
          {
          CSysVecTemplateItem *T=SVI.m_Templates.GetNext(Pos);
          DDBTemp.Add((LPSTR)(LPCSTR)T->m_Name);
          }
        DDB.String("Template.Main",   "", DC_,  "", &m_sMainTemplate,     this, isParm|Flags|DupHandlesOk|SetOnChange,DDBTemp());// Allow dup handles
        DDB.Button("Template.Apply",  "", DC_,  "", xidApplyCfgTemplate,  this, isParm|Flags|DupHandlesOk|SetOnChange);// Allow dup handles
        }
      };
    default:;
    }
  }

// --------------------------------------------------------------------------

void SpModel::BuildDataDefn_Species(DataDefnBlk & DDB, const BDDSpecies_Blk * Blks, size_t BlkSize)
  {
  DDEF_Flags OldVis=DDB.GetVisibility();
  int nBlks=BlkSize/sizeof(BDDSpecies_Blk);

  //return;

  #if dbgSpModel
  bool EditAll=(dbgAllEditable()!=0);
  #else
  bool EditAll=false;
  #endif

  int           iDefltBlk   = 0;
  int           iViewBlk    = -1;
  //int           iFileBlk    = -1;
  int           iBlk0       = 0;
  int           iBlkN       = nBlks;

  if (DDB.ForFileSnpScn())
    {
    if (nBlks>1)
      AddSpView(DDB, Blks, nBlks, (true/*Blks[iViewBlk].m_bAsParm*/ ? isParm : 0) | OldVis);
    }

  // Get Current View Reqd
  SV_View       iView       = GetView();
  SV_ViewBasis  iViewBasis  = GetViewBasis();
  if (DDB.ForFileSnpScn())
    {
    // switch view to file equivalent
    for (int iBlk=0; iBlk<nBlks; iBlk++)
      if (Blks[iBlk].m_iView==iView)
        {
        iView=Blks[iBlk].m_iFileView;
        break;
        }
    }

  // Check that iView is Valid
  for (int iBlk=0; iBlk<nBlks; iBlk++)
    {
    if (Blks[iBlk].m_iView==iView)
      iViewBlk=iBlk;
    if (Blks[iBlk].m_bIsRoot)
      iDefltBlk=iBlk;
    }
  if (iViewBlk<0)//iBlk==nBlks) // Invalid VIew
    {
    iViewBlk=iDefltBlk;
    iView=Blks[iDefltBlk].m_iView;
    SetView(iView);
    }

  if (DDB.ForView())// || DDB.ForFileSnpScn())
    {
    //if (DDB.ForView())
    DDB.Page(gs_ViewInfo[iView].Nm, DDB_RqdPage);//DDB_OptPage);
    if (nBlks>1)
      AddSpView(DDB, Blks, nBlks, (Blks[iViewBlk].m_bAsParm? isParm : 0) | OldVis);
    }
  if (DDB.ForView() || DDB.ForFileSnpScn())
    {
    iBlk0=iViewBlk;
    iBlkN=iViewBlk+1;
    }

  for (iBlk=iBlk0; iBlk<iBlkN; iBlk++)
    {
    SV_View  RqdVw=Blks[iBlk].m_iView;
    flag     AsParms=Blks[iBlk].m_bAsParm;
    flag     IsRoot=Blks[iBlk].m_bIsRoot;
    flag     ForView=(iView==RqdVw);
    const CSV_ViewInfo & ViewInfo=gs_ViewInfo[RqdVw];
    SV_ViewBasis iBasis0=SVVB_All;
    SV_ViewBasis iBasisN=SVVB_All;

    if (((sm_SpVarsAvail|m_SpVarsAvail) & Blks[iBlk].m_VAMsk)==0)
      continue;

    if (DDB.ForFileSnpScn())
      {
      //if (!ForFile)
      //  continue;
      }
    else
      {
      while (ViewInfo.m_Basis[iBasisN+1].m_Basis<SVVB_End)
       iBasisN++;
      }
    if (DDB.ForView())
      {
      if (RqdVw!=iView)
        continue;
      for (SV_ViewBasis ii=iBasis0; ii<=iBasisN; ii++)
        if (ViewInfo.m_Basis[ii].m_Basis==iViewBasis)
          {
          iBasis0=ii;
          iBasisN=ii;
          }
        if (iBasis0!=iBasisN)
          {
          iBasis0=SVVB_All;
          iBasisN=SVVB_All;
          }
      }

    DDEF_Flags NewVis=OldVis;
    DDEF_Flags Flags=  (AsParms ? isParm : isParmConstruct) | NewVis;
    DDEF_Flags FlagsV= (AsParms ? isParm : 0) | NewVis;
    DDB.SetVisibility(NewVis);
    switch (RqdVw)
      {
      case SVV_AsRawFrac:
      case SVV_AsRawMass:
      case SVV_AsRawMassFlow:
        {
        const CSV_ViewInfo & ViewInfo=gs_ViewInfo[RqdVw];
        const CSV_ViewCnv * Cnv=ViewInfo.Cnvs[0];
        DDB.SetVisibility(NewVis);
        //DDB.Page(ViewInfo.Nm, DDB_RqdPage);//DDB_OptPage);
        if (nBlks>1)
          {
          //AddSpView(DDB, Blks, nBlks, FlagsV);
          AddSpViewBasis(DDB, Blks, nBlks, RqdVw, FlagsV);
          }

        flag BlkReqd=(!IsRoot);// || RqdVwBasis!=SVVB_All);
        if (PrjFileVerNo()>=22)
          BlkReqd=BlkReqd || DDB.ForFileSnpScn();

        flag DoBlock=true;
        if (BlkReqd)
          {
          if (PrjFileVerNo()<22)
            DoBlock=DDB.BeginStruct(this, ViewInfo.Nm, NULL, DDB_NoPage);
          else
            {
            Strng Class("Sp_");
            Class+=ViewInfo.Nm;
            DoBlock=DDB.BeginObject(this, ViewInfo.Nm, Class(), NULL, DDB_NoPage);
            }
          }
        if (DoBlock)
          {
          //          if ((som_Which==som_ALL) && ForView)
          if (ForView)
            {
            for (int i=0; i<SVCfgCount(); i++)
              {
              CSysVecItem &I=*SVI.Cfg(i);
              //S=I.FullTag();
              int iSp=I.SVIndex();
              bool AsPerRqdVw=I.CnvIndex()<0;
              int MyiCnv = AsPerRqdVw ? *Cnv->m_pInx : I.CnvIndex();
              LPSTR MysCnv = AsPerRqdVw ? (LPSTR)(LPCSTR)Cnv->m_Txt: I.CnvString();
              switch (I.Type())
                {
                case SVT_Specie:
                  DDB.Double(SDB[iSp].Tag(), SDB.Sym(iSp), MyiCnv, MysCnv, ViewInfo.xid/*+iSp*/, SpHandle2(0, iSp), this, isResult|Flags|HideIfZero/*|(NANsOK?NAN_OK:0)*/);
                  if (m_bMassInUseOK && !MassInUse(iSp))
                    DDB.TagComment("*");

                  break;

                case SVT_Annotation:
                  if (ForView)
                    {
                    switch (I.Annotation())
                      {
                      case SVA_Text:    DDB.Text(I.FullTag());              break;
                      case SVA_Marker:  DDB.Text(I.FullTag(), Marker);      break;
                      case SVA_OptPage: DDB.Page(I.FullTag(), DDB_OptPage); break;
                      case SVA_RqdPage: DDB.Page(I.FullTag(), DDB_RqdPage); break;
                      }
                    }
                  break;
                }
              }
            }
          else
            {
            for (int i=0; i<SVSpcCount(); i++)
              {
              DDB.Double(SDB[i].Tag(), SDB.Sym(i),
                *Cnv->m_pInx, (LPSTR)(LPCSTR)Cnv->m_Txt, ViewInfo.xid, SpHandle2(0, i), this, isResult|Flags/*|(NANsOK?NAN_OK:0)*/);
              }
            }
          }
        if (BlkReqd)
          {
          if (PrjFileVerNo()<22)
            DDB.EndStruct();
          else
            DDB.EndObject();
          }
        break;
        }
      //case SVV_AsRawMass:
      //case SVV_AsRawMassFlow:

      //  break;
      case SVV_AsMass:
      case SVV_AsMassFlow:
      case SVV_AsMassFrac:
      case SVV_AsMole:
      case SVV_AsMoleFlow:
      case SVV_AsMoleFrac:
      case SVV_AsVol:
      case SVV_AsVolFlow:
      case SVV_AsVolFrac:
      case SVV_AsPartialP:
      case SVV_AsPartialPFrac:
      case SVV_AsNVol:
      case SVV_AsNVolFlow:
      case SVV_AsNVolFrac:
      case SVV_AsSpcConc:
        {
        const CSV_ViewInfo & ViewInfo=gs_ViewInfo[RqdVw];

        for (SV_ViewBasis ii=iBasis0; ii<=iBasisN; ii++)
          {
          SV_ViewBasis RqdVwBasis=ViewInfo.m_Basis[ii].m_Basis;
          //const CSV_ViewCnv * Cnv=SpViewCnv(ii, RqdVwBasis, );
          byte LPh=GetViewBasis(DDB.ForFiling()?SVForce_Lcl:SVForce_None);
          Strng Ttl(ViewInfo.Nm);
          Ttl+=ViewInfo.m_Basis[ii].m_Txt;
          DDB.SetVisibility(NewVis);

          //DDB.Page(ViewInfo.Nm, DDB_RqdPage);//DDB_OptPage);
          if (nBlks>1)
            {
            //AddSpView(DDB, Blks, nBlks, FlagsV);
            AddSpViewBasis(DDB, Blks, nBlks, RqdVw, FlagsV);
            }

          flag BlkReqd=(!IsRoot || RqdVwBasis!=SVVB_All);
          if (PrjFileVerNo()>=22)
            BlkReqd=BlkReqd || DDB.ForFileSnpScn();


          int BlkPass0=0;
          int BlkPassN=0;
          if (DDB.ForFileSnpScn()&&PrjFileVerNo()>=75)
            {
            BlkPass0=1;
            BlkPassN=2;
            }
          for (int BlkPass=BlkPass0; BlkPass<=BlkPassN; BlkPass++)
            {
            int i0=0;
            int iN=SVCfgCount();
            flag DoBlock=true;
            if (BlkReqd)
              {
              if (PrjFileVerNo()<22)
                DoBlock=DDB.BeginStruct(this, Ttl(), NULL, DDB_NoPage);
              else if (PrjFileVerNo()<75)
                {
                Strng Class("Sp_");
                Class+=ViewInfo.Nm;
                DoBlock=DDB.BeginObject(this, Ttl(), Class(), NULL, DDB_NoPage);
                }
              else
                {
                if (BlkPass<2)
                  {
                  Strng Class("Sp_");
                  Class+=ViewInfo.Nm;
                  DoBlock=DDB.BeginObject(this, Ttl(), Class(), NULL, DDB_NoPage);
                  iN=(BlkPass==0 ? SVCfgCount() : SVSpcCount());
                  }
                else
                  {
                  Strng Class("Sp_Attribute");
                  DoBlock=DDB.BeginObject(this, /*Ttl()*/"Att", Class(), NULL, DDB_NoPage);
                  iN=SVAttCount();
                  }
                }
              }
            if (DoBlock)
              {
              //bool FirstSpecie=true;
              if (m_bMassInUseOK)
                {
                DDB.Text("(*) Not applied , *+ Apply None, *- Apply All");
                }
              for (int ii=i0; ii<iN; ii++)
                {
                int i;
                switch (BlkPass)
                  {
                  case 0: i=SVI.Cfg(ii)->CfgIndex(); break;
                  case 1: i=SVI.Spc(ii)->CfgIndex(); break;
                  case 2: i=SVI.Att(ii)->CfgIndex(); break;
                  }
                CSysVecItem &SVCfg=*SVI.Cfg(i);

                bool AsPerRqdVw=SVCfg.CnvIndex()<0;

                switch (SVCfg.Type())
                  {
                  case SVT_Specie:
                    {
                    int iSp=SVCfg.SVIndex();
                    const CSV_ViewCnv * Cnv=SVI.Img(iSp)->ViewBasisCnv(RqdVw, RqdVwBasis);
                    int MyiCnv = AsPerRqdVw ? *Cnv->m_pInx : SVCfg.CnvIndex();
                    LPSTR MysCnv = AsPerRqdVw ? (LPSTR)(LPCSTR)Cnv->m_Txt: SVCfg.CnvString();
                    DDEF_Flags SFlags= FixSpEditFlags(Flags, iSp, (AsParms && (SVCfg.ViewBasisEditable(RqdVw, RqdVwBasis)||EditAll) || !DDB.ForView()))|DDEF_ISSPECIE;
                    DDB.Double(SVCfg.FullTag(), SVCfg.FullSym(), MyiCnv, MysCnv,
                      ViewInfo.xid, SpHandle2(RqdVwBasis, iSp), this, SFlags|HideIfZero/*|(NANsOK?NAN_OK:0)*/);
                    if (m_bMassInUseOK && !MassInUse(iSp))
                      DDB.TagComment("*");
                    break;
                    }
                  case SVT_Attribute:
                    {
                    if (SVCfg.ViewBasisUsed(RqdVwBasis))
                      {
                      int iAt=SVCfg.SVIndex();
                      int Handle, Handle2;
                      if (AsPerRqdVw)
                        {
                        Handle=xidAttrViews+(ViewInfo.xid-xidSpFirst);
                        Handle2=SpHandle2(RqdVwBasis, iAt);
                        }
                      else
                        {
                        Handle=xidAttrSimple;//-iCal;
                        Handle2=SpHandle2(0, iAt);
                        }
                      DDEF_Flags AFlags = ((Flags&!(isParm|isResult)) | AsParms ? isParm : isResult)|
                                          (SVCfg.ChildCount()>0?Marker:0)| DDEF_ISSPECIEATT;
                      DDB.Double(SVCfg.FullTag(), SVCfg.FullSym(), SVCfg.CnvIndex(), SVCfg.CnvString(),
                        Handle, Handle2, this, AFlags|HideIfZero/*|(NANsOK?NAN_OK:0)*/);
                      DDB.TagComment("[a]");
                      }
                    break;
                    }
                  case SVT_Calculation:
                    {
                    if (SVCfg.ViewBasisUsed(RqdVwBasis))
                      {
                      int iCal=SVCfg.SVIndex();             //NBNB Index - Negetive
                      int Handle, Handle2;
                      if (AsPerRqdVw)
                        {
                        Handle=xidCalcViews+(ViewInfo.xid-xidSpFirst);
                        Handle2=SpHandle2(RqdVwBasis, iCal);
                        }
                      else
                        {
                        Handle=xidCalcSimple;//-iCal;
                        Handle2=SpHandle2(0, iCal);
                        }

                      int MyiCnv;
                      LPSTR MysCnv;
                      if (AsPerRqdVw)
                        {
                        const CSV_ViewCnv * Cnv=SVI.Img(iCal)->ViewBasisCnv(RqdVw, RqdVwBasis);
                        MyiCnv = *Cnv->m_pInx;
                        MysCnv = (LPSTR)(LPCSTR)Cnv->m_Txt;
                        }
                      else
                        {
                        MyiCnv = SVCfg.CnvIndex();
                        MysCnv = SVCfg.CnvString();
                        }

                      DDEF_Flags ParmResFlag=isParm;
                      if (DDB.ForView())
                        ParmResFlag=((Flags&!(isParm|isResult)) | (AsParms && (SVCfg.ViewBasisEditable(RqdVw, RqdVwBasis)||EditAll) ? SVCfg.FnIsSpcsSum() : false) ? isParm : isResult);
                      DDEF_Flags CFlags=noFileAtAll|DDEF_ISSPECIECALC;
                      if (SVI.Img(iCal)->InitHidden())
                        CFlags|=InitHidden;
                      DDB.Double(SVCfg.FullTag(), SVCfg.FullSym(), MyiCnv, MysCnv, Handle, Handle2, this, CFlags|ParmResFlag|HideIfZero/*|(NANsOK?NAN_OK:0)*/);
                      DDB.TagComment("[c]");
                      }
                    break;
                    }
                  case SVT_Annotation:
                    {
                    if (SVCfg.ViewBasisUsed(RqdVwBasis))
                      {
                      DDEF_Flags ViewFlag=0;
                      switch (SVCfg.Annotation())
                        {
                        case SVA_Text:    DDB.Text(SVCfg.FullTag(), ViewFlag);              break;
                        case SVA_Marker:  DDB.Text(SVCfg.FullTag(), ViewFlag|Marker);       break;
                        case SVA_OptPage: DDB.Page(SVCfg.FullTag(), DDB_OptPage, ViewFlag); break;
                        case SVA_RqdPage: DDB.Page(SVCfg.FullTag(), DDB_RqdPage, ViewFlag); break;
                        }
                      }
                    break;
                    }
                  }
                }
              }
            if (BlkReqd)
              {
              if (PrjFileVerNo()<22)
                DDB.EndStruct();
              else
                DDB.EndObject();
              }
            }
          DDB.SetVisibility(OldVis);
          }
        break;
        }
      case SVV_AsElem:
      case SVV_AsElemFlow:
      case SVV_AsElemFrac:
      case SVV_AsElemConc:
      case SVV_AsElemConc25:
        {
        const CSV_ViewInfo & ViewInfo=gs_ViewInfo[RqdVw];
        for (int ii=iBasis0; ii<=iBasisN; ii++)
          {
          int RqdVwBasis=ViewInfo.m_Basis[ii].m_Basis;
          const CSV_ViewCnv * Cnv=ViewInfo.Cnvs[0];
          DDEF_Flags FlagsE= FlagsV&~isParm;
          Strng Ttl(ViewInfo.Nm);
          //Ttl+=SVV_Name[RqdVwBasis];
          Ttl+=ViewInfo.m_Basis[ii].m_Txt;
          //DDB.Page(ViewInfo.Nm, DDB_RqdPage);//DDB_OptPage);
          if (nBlks>1)
            {
            //AddSpView(DDB, Blks, nBlks, FlagsE);
            AddSpViewBasis(DDB, Blks, nBlks, RqdVw, FlagsE);
            }

          flag BlkReqd=(!IsRoot || RqdVwBasis!=SVVB_All);
          if (PrjFileVerNo()>=22)
            BlkReqd=BlkReqd || DDB.ForFileSnpScn();

          flag DoBlock=true;
          if (BlkReqd)
            {
            if (PrjFileVerNo()<22)
              DoBlock=DDB.BeginStruct(this, Ttl(), NULL, DDB_NoPage);
            else
              {
              Strng Class("El_");
              Class+=ViewInfo.Nm;
              DoBlock=DDB.BeginObject(this, Ttl(), Class(), NULL, DDB_NoPage);
              }
            }
          if (DoBlock)
            {
            for (int i=0; i<EDB.Count(); i++)
              if (EDB[i].SpComp->GetSize()>0)
                DDB.Double(EDB[i].Name,"",
                *Cnv->m_pInx, (LPSTR)(LPCSTR)Cnv->m_Txt,
                ViewInfo.xid, SpHandle2(RqdVwBasis, i), this,
                isResult|FlagsE|HideIfZero);
            }
          if (BlkReqd)
            {
            if (PrjFileVerNo()<22)
              DDB.EndStruct();
            else
              DDB.EndObject();
            }
          }
        break;
        }
      case SVV_AsComp:
      case SVV_AsCompFlow:
      case SVV_AsCompFrac:
      case SVV_AsCompConc:
      case SVV_AsCompConc25:
        {
        const CSV_ViewInfo & ViewInfo=gs_ViewInfo[RqdVw];
        for (int ii=iBasis0; ii<=iBasisN; ii++)
          {
          int RqdVwBasis=ViewInfo.m_Basis[ii].m_Basis;
          const CSV_ViewCnv * Cnv=ViewInfo.Cnvs[0];
          DDEF_Flags FlagsC= Flags&~isParm;
          Strng Ttl(ViewInfo.Nm);
          Ttl+=ViewInfo.m_Basis[ii].m_Txt;
          //DDB.Page(ViewInfo.Nm, DDB_RqdPage);
          if (nBlks>1)
            {
            //AddSpView(DDB, Blks, nBlks, FlagsV);
            AddSpViewBasis(DDB, Blks, nBlks, RqdVw, FlagsV);
            }

          flag BlkReqd=(!IsRoot || RqdVwBasis!=SVVB_All);
          if (PrjFileVerNo()>=22)
            BlkReqd=BlkReqd || DDB.ForFileSnpScn();

          flag DoBlock=true;
          if (BlkReqd)
            {
            if (PrjFileVerNo()<22)
              DoBlock=DDB.BeginStruct(this, Ttl(), NULL, DDB_NoPage);
            else
              {
              Strng Class("Cm_");
              Class+=ViewInfo.Nm;
              DoBlock=DDB.BeginObject(this, Ttl(), Class(), NULL, DDB_NoPage);
              }
            }
          if (DoBlock)
            for (int i=0; i<CDB.No(); i++)
              DDB.Double(CDB[i].Sym(), "",
                          *Cnv->m_pInx, (LPSTR)(LPCSTR)Cnv->m_Txt,
                          ViewInfo.xid, SpHandle2(RqdVwBasis, i), this,
                          isResult|FlagsC|HideIfZero);
          if (BlkReqd)
            {
            if (PrjFileVerNo()<22)
              DDB.EndStruct();
            else
              DDB.EndObject();
            }
          }
        break;
        }
      default:
        DoBreak();
        break;
      }
    }

  if (m_bMassInUseOK && !DDB.ForView() && (m_MassInUse.GetSize()>0 || DDB.DoingPutData()))
    {
    SV_View  RqdVw=Blks[iViewBlk].m_iView;
    const CSV_ViewInfo & ViewInfo=gs_ViewInfo[RqdVw];

    Strng Class("SpUsage");
    Strng Tag("Apply");
    if (DDB.BeginObject(this, Tag(), Class(), NULL, DDB_NoPage))
      {
      for (int i=0; i<SVSpcCount(); i++)
        {
        CSysVecItem &I=*SVI.Spc(i);
        DDB.Bool(SDB[i].Tag(), SDB.Sym(i), 0, "", xidSpInUse, i, this, isParm);
        }
      }
    DDB.EndObject();
    }

  DDB.SetVisibility(OldVis);
  };

// --------------------------------------------------------------------------

flag SpModel::DataXchg_Species(DataChangeBlk & DCB)
  {
  ASSERT(MaxCalculations<MaxSpeciesEx);
  ASSERT(xidSpcEndOfList<xidSpUsrProps);

  switch (DCB.lHandle)
    {
    case xidSMViewGlbl:
      if (m_pGlblView)
        {
        if (1)//!DCB.ForFiling())
          {
          if (DCB.rB)
            m_pGlblView->m_bOn=*DCB.rB;
          DCB.B=m_pGlblView->m_bOn;
          }
        else
          DCB.B=false;
        }
      return 1;
    case xidSMView:
      if (DCB.rB)
        SetView(SV_View(*DCB.rB));
      DCB.B=GetView(DCB.ForFiling()?SVForce_Lcl:SVForce_None);
      return 1;
    case xidSMViewL:
      if (DCB.rB)
        SetView(SV_View(*DCB.rB));
      DCB.B=GetView(SVForce_Lcl);
      return 1;
    case xidSMViewG:
      if (DCB.rB)
        SetView(SV_View(*DCB.rB));
      DCB.B=GetView(SVForce_Glbl);
      return 1;
    case xidSMViewBasisMass:
      if (DCB.rB)
        SetViewBasis(SV_ViewBasis(*DCB.rB));
      DCB.B=GetViewBasis(DCB.ForFiling()?SVForce_Lcl:SVForce_None);
      return 1;
    case xidSMViewBasisElem:
      if (DCB.rB)
        SetViewBasis(SV_ViewBasis(*DCB.rB));
      DCB.B=GetViewBasis(DCB.ForFiling()?SVForce_Lcl:SVForce_None);
      return 1;

    case xidApplyCfgTemplate:
      {
      if (DCB.rB && *DCB.rB)
        {
        CSysVecTemplateItem *pT=SVI.m_Templates.FindTemplate(m_sMainTemplate.StrC());
        if (pT)
          {
          double SumM=0;
          double SumT=0;
          for (int iImg=0; iImg<SVValueCount(); iImg++)
            {
            CSysVecItem &I=*SVI.Img(iImg);
            if (Finite(pT->m_Values[iImg]))
              {
              if (I.Type()==SVT_Specie)
                {
                SumM+=m_M[iImg];
                SumT+=pT->m_Values[iImg];
                }
              }
            }
          SumT=GTZ(SumT);
          if (SumM<1e-10)
            SumM=1.0;
          for (int iImg=0; iImg<SVValueCount(); iImg++)
            {
            CSysVecItem &I=*SVI.Img(iImg);
            if (Finite(pT->m_Values[iImg]))
              {
              switch (I.Type())
                {
                case SVT_Specie:
                  SetCfgImage(SVV_AsMassFrac, SVVB_All, iImg, pT->m_Values[iImg]*SumM/SumT);
                  break;
                case SVT_Attribute:
                  SetCfgImage(SVV_AsMassFrac, SVVB_All, iImg, pT->m_Values[iImg]);
                  break;
                }
              }
            }
          }
        }
      DCB.B=0;
      return 1;
      }
    case xidSaveCfgTemplate:
      {
      if (DCB.rB && *DCB.rB)
        {
        CString MyTemplate=FullObjTag();
        CSysVecTemplateItem *pT=SVI.m_Templates.FindTemplate(MyTemplate, true);
        (*pT)=*this;
        int xxx=0;
        }
      DCB.B=0;
      return 1;
      }
    default:
      {
      if (DCB.lHandle>=xidSpRawFrac && DCB.lHandle<xidCalcSimple)
        {
        bool IsSpc=(DCB.lHandle<xidCalcViews);
        if (!IsSpc)
          DCB.lHandle-=xidViewCount;
        switch (DCB.lHandle)
          {
          case xidSpRawFrac:
            {
            int s=SpIndex2(DCB.lHandle2);
            if (IsSpc && DCB.rD)
              {
              if (0)
                {
                Strng Tg(FullObjTag());
                Tg+=".";
                Tg+=SVImg(s).SymOrTag();
                dbgpln("<<RF %-60s %12.4f", Tg(), *DCB.rD);
                }
              if (m_bMassInUseOK)
                {
                if(DCB.m_bWasNAN)
                  {
                  if (HasNANFlag(DCB.m_dNANValue, NF_Plus) || HasNANFlag(DCB.m_dNANValue, NF_Minus))
                    {
                    bool On=HasNANFlag(DCB.m_dNANValue, NF_Plus);
                    for (int i=0; i<SVSpcCount(); i++)
                      SetMassInUse(i, !On);
                    }
                  else
                    SetMassInUse(s, false);
                  }
                else
                  {
                  if (m_MassInUse.GetSize()>0)
                    SetMassInUse(s, true);
                  m_M.VValue[s]=*DCB.rD;
                  ClrStatesOK();
                  }
                }
              }
            if (IsSpc)
              DCB.D=m_M[s];
            else
              DCB.D=m_M.VValue[s];
            return 1;
            }
          case xidSpRawMass:
          case xidSpRawMassFlow:
            {
            int s=SpIndex2(DCB.lHandle2);
            if (IsSpc && DCB.rD)
              {
              #if dbgSpModel
              if (0)
                {
                Strng Tg(FullObjTag());
                Tg+=".";
                Tg+=SVImg(s).SymOrTag();
                //dbgpln("<<RM %-60s %s %12.4f", Tg(), DCB.m_bWasNAN?"NAN":"   ", *DCB.rD);
                dbgpln("<<RM %14s %14s %s", DCB.m_bWasNAN ? "NAN":DbgFltString(*DCB.rD), DbgFltString(m_dMassScale), Tg());
                }
              #endif
              if (m_bMassInUseOK)
                {
                if(DCB.m_bWasNAN)
                  {
                  if (HasNANFlag(DCB.m_dNANValue, NF_Plus) || HasNANFlag(DCB.m_dNANValue, NF_Minus))
                    {
                    bool On=HasNANFlag(DCB.m_dNANValue, NF_Plus);
                    for (int i=0; i<SVSpcCount(); i++)
                      SetMassInUse(i, !On);
                    }
                  else
                    SetMassInUse(s, false);
                  }
                else
                  {
                  if (m_MassInUse.GetSize()>0)
                    SetMassInUse(s, true);
                  m_M.VValue[s]=*DCB.rD;
                  ClrStatesOK();
                  }
                }
              }

            if (IsSpc)
              DCB.D=m_M[s];
            else
              DCB.D=m_M.VValue[s];
            return 1;
            }
          case xidSpMass:
          case xidSpMassFlow:
          case xidSpMole:
          case xidSpMoleFlow:
          case xidSpVol:
          case xidSpVolFlow:
          case xidSpNVol:
          case xidSpNVolFlow:
            {
            int Op = SVOperatorsXID(DCB.lHandle);
            int s = SpIndex2(DCB.lHandle2);
            PhMask PhM = SVV_BasisMask(DCB.lHandle2);
            if (DCB.rD)
              {
              #if dbgSpModel
              if (0)
                {
                Strng Tg(FullObjTag());
                Tg+=".";
                Tg+=SVImg(s).SymOrTag();
                dbgpln("<<M  %14s %14s %s", DbgFltString(*DCB.rD), DbgFltString(m_dMassScale), Tg());
                }
              #endif
              if(DCB.m_bWasNAN)
                {
                if (HasNANFlag(DCB.m_dNANValue, NF_Plus) || HasNANFlag(DCB.m_dNANValue, NF_Minus))
                  {
                  bool On=HasNANFlag(DCB.m_dNANValue, NF_Plus);
                  for (int i=0; i<SVSpcCount(); i++)
                    SetMassInUse(i, !On);
                  }
                else
                  SetMassInUse(s, false);
                }
              else
                {
                SetMassInUse(s, true);

                double M = Valid(*DCB.rD) ? *DCB.rD : 0.0;
                if (NegDataOK() && fabs(M)>1.0e-20)
                  SetDataSign((int)Sign(M));
                else
                  M=GEZ(M);
                ASSERT(s<MaxSpecies);
                SV_View       RqdVw = (SV_View)(DCB.lHandle-xidSpFirst);
                SV_ViewBasis  RqdBasis = (SV_ViewBasis)SpPhase2(DCB.lHandle2);
                SetCfgImage(RqdVw, RqdBasis, s, fabs(M) / (m_dMassScale>0.0?m_dMassScale:1.0));
                }
              }
            DCB.D=((m_bUsableMass || !DCB.ForView()) && ((SVI.Img(s)->OccMask()&PhM)!=0)) ? m_iDataSgn * m_dMassScale * m_M.ValueOp(s, Op, Fidelity(), Temp(), Press()) : 0.0;
            return 1;
            }
          case xidSpMassFrac:
          case xidSpMoleFrac:
          case xidSpVolFrac:
          case xidSpNVolFrac:
          case xidSpConc:
            {
            int Op=SVOperatorsXID(DCB.lHandle);
            int s=SpIndex2(DCB.lHandle2);
            PhMask PhM=SVV_BasisMask(DCB.lHandle2);
            double PhTotal=0;
            switch (DCB.lHandle)
              {
              case xidSpMassFrac: PhTotal=Mass(PhM); break;
              case xidSpMoleFrac: PhTotal=Moles(PhM); break;
              case xidSpVolFrac:  PhTotal=Volume(PhM); break;
              case xidSpNVolFrac: PhTotal=NVolume(PhM); break;
              case xidSpConc:     PhTotal=Volume(PhM); break;
              }
            if (DCB.rD)
              {
              if (0)
                {
                Strng Tg(FullObjTag());
                Tg+=".";
                Tg+=SVImg(s).SymOrTag();
                dbgpln("<<MF %-60s %12.4f %12.4f", Tg(), *DCB.rD, m_dMassScale);
                }
              if(DCB.m_bWasNAN)
                {
                if (HasNANFlag(DCB.m_dNANValue, NF_Plus) || HasNANFlag(DCB.m_dNANValue, NF_Minus))
                  {
                  bool On=HasNANFlag(DCB.m_dNANValue, NF_Plus);
                  for (int i=0; i<SVSpcCount(); i++)
                    SetMassInUse(i, !On);
                  }
                else
                  SetMassInUse(s, false);
                }
              else
                {
                SetMassInUse(s, true);

                double M = Valid(*DCB.rD) ? *DCB.rD : 0.0;
                if (NegDataOK() && fabs(M)>1.0e-20)
                  SetDataSign((int)Sign(M));
                else
                  M=GEZ(M);
                SV_View       RqdVw = (SV_View)(DCB.lHandle-xidSpFirst);
                SV_ViewBasis  RqdBasis = (SV_ViewBasis)SpPhase2(DCB.lHandle2);
                SetCfgImage(RqdVw, RqdBasis, s, M);
                }
              }
            DCB.D=((SVI.Img(s)->OccMask()&PhM)!=0 ? m_M.ValueOp(s, Op, Fidelity(), Temp(), Press()) : 0.0) / GTZ(PhTotal);
            return 1;
            }
          case xidSpPartialP:
          case xidSpPartialPFrac:
            {
            int s=SpIndex2(DCB.lHandle2);
            PhMask PhM=SVV_BasisMask(DCB.lHandle2);
            double T=Temp();
            double P=Press();
            if (IsSpc)
              {
              if (SDB[s].IsGas())
                DCB.D=(DCB.lHandle==xidSpPartialP) ? PartialPress(s, -1, T) : PartialPressFrac(s, -1, T);
              else
                DCB.D=dNAN;
              }
            else
              DCB.D=dNAN;
            return 1;
            }
            
          case xidSpElem:
          case xidSpElemFrac:
          case xidSpElemConc:
          case xidSpElemConc25:
            {
            int e=SpIndex2(DCB.lHandle2);
            SV_ViewBasis VB=(SV_ViewBasis)SpPhase2(DCB.lHandle2);
            PhMask PhM=som_ALL;
            switch (VB)
              {
              case SVVB_All: PhM = CSysVecInfo::SpVwPhMask(SpVwPhMaskIndex_All);    break;
              case SVVB_Sol: PhM = CSysVecInfo::SpVwPhMask(SpVwPhMaskIndex_Solid);  break;
              case SVVB_Liq: PhM = CSysVecInfo::SpVwPhMask(SpVwPhMaskIndex_Liquid); break;
              case SVVB_Gas: PhM = CSysVecInfo::SpVwPhMask(SpVwPhMaskIndex_Gas);    break;
              default:
                DoBreak();
              }

            CompFracArray &CF=*(EDB[e].SpComp);
            int Ns=CF.GetSize();
            DCB.D=0.0;
            if (m_bUsableMass || !DCB.ForView())
              {
              for (int i=0; i<Ns; i++)
                {
                int s=CF[i].iSpNo;
                if (SDB[s].PhaseIs(PhM))
                  DCB.D+=m_M[s]*CF[i].dMassFrac;
                }
              switch (DCB.lHandle)
                {
                case xidSpElem:       DCB.D *= m_iDataSgn;                          return 1;
                case xidSpElemFrac:   DCB.D /= GTZ(Mass(PhM));                      return 1;
                case xidSpElemConc:   DCB.D /= GTZ(Volume(PhM));                    return 1;
                case xidSpElemConc25: DCB.D /= GTZ(Volume(PhM, C2K(25), Press()));  return 1;
                }
              }
            return 1;
            }
          case xidSpComp:
          case xidSpCompFrac:
          case xidSpCompConc:
          case xidSpCompConc25:
            {
            int c=SpIndex2(DCB.lHandle2);
            SV_ViewBasis VB=(SV_ViewBasis)SpPhase2(DCB.lHandle2);
            PhMask PhM=som_ALL;
            switch (VB)
              {
              case SVVB_All: PhM = CSysVecInfo::SpVwPhMask(SpVwPhMaskIndex_All);    break;
              case SVVB_Sol: PhM = CSysVecInfo::SpVwPhMask(SpVwPhMaskIndex_Solid);  break;
              case SVVB_Liq: PhM = CSysVecInfo::SpVwPhMask(SpVwPhMaskIndex_Liquid); break;
              case SVVB_Gas: PhM = CSysVecInfo::SpVwPhMask(SpVwPhMaskIndex_Gas);    break;
              default:
                DoBreak();
              }
            //PhMask PhM=SVV_BasisMask(DCB.lHandle2);
            CComponent &C=CDB[c];
            DCB.D=0.0;
            for (int i=0; i<C.NSpecies(); i++)
              if (C.pSpecie(i)->PhaseIs(PhM))
                DCB.D+=m_M[C.iSpecie(i)];
            switch (DCB.lHandle)
              {
              case xidSpComp:       DCB.D *= m_iDataSgn;                        return 1;
              case xidSpCompFrac:   DCB.D /= GTZ(Mass(PhM));                      return 1;
              case xidSpCompConc:   DCB.D /= GTZ(Volume(PhM));                    return 1;
              case xidSpCompConc25: DCB.D /= GTZ(Volume(PhM, C2K(25), Press()));  return 1;
              }

            return 1;
            }
          default:
            DoBreak();
            return 1;
          }
        }
      else if (DCB.lHandle>=xidCalcSimple && DCB.lHandle<xidSpInUse)
        {
        int c=SpIndex2(DCB.lHandle2);
        if (DCB.rD)
          m_M.VValue[c]=*DCB.rD;
        DCB.D=m_M.VValue[c];
        return 1;
        }
      else if (DCB.lHandle>=xidSpInUse && DCB.lHandle<xidAttrViews)
        {
        if (m_bMassInUseOK)
          {
          int s=DCB.lHandle2;
          if (DCB.rB)
            SetMassInUse(s, *DCB.rB!=0);
          DCB.B=MassInUse(s);
          }
        else
          DCB.B=false;
        return 1;
        }
      else if (DCB.lHandle>=xidAttrViews && DCB.lHandle<xidLastAttribute)
        {
        if (DCB.lHandle>=xidAttrSimple)
          DCB.lHandle-=xidViewCount;
        int a=SpIndex2(DCB.lHandle2);
        if (DCB.rD)
          {
          if(DCB.m_bWasNAN)
            {
            }
          else
            {
            double A = Valid(*DCB.rD) ? *DCB.rD : 0.0;
            SV_View       RqdVw = (SV_View)(DCB.lHandle-xidAttrViews);
            SV_ViewBasis  RqdBasis = (SV_ViewBasis)SpPhase2(DCB.lHandle2);
            SetCfgImage(RqdVw, RqdBasis, a, A);
            }
          }
        DCB.D=m_M[a];
        return 1;
        }
      }
    }
  return 0;
  }

// --------------------------------------------------------------------------

void SpModel::SetCfgImage(SV_View SetView, SV_ViewBasis ViewBasis, int Index, double Value)
  {
  if (!m_bCfgImgSet)//m_iSetView!=SetView)
    {
    m_bCfgImgSet=true;
    if (SetView!=SVV_NULL)
      m_iSetView=SetView;
    //m_iPhMask=PhM;
    m_iViewBasis=ViewBasis;
    if (!m_pSetCfgImg)
      {
      CSpMCfgImg * pImg=new CSpMCfgImg[SVClcCount()+SVValueCount()];
      m_pSetCfgImg = &pImg[SVClcCount()];
      }
    for (int s=-SVClcCount(); s<SVValueCount(); s++)
      {
      m_pSetCfgImg[s].m_Value=dNAN;
      //m_pSetCfgImg[s].m_PhMask=0;
      }
    m_dSetCfgImgT=Temp();
    m_dSetCfgImgP=Press();
    //dbgpln("SetImage %i %i -----------------------", SetView, ViewBasis);
    }
  //dbgpln("SetImage %i %14s", Index, DbgFltString(Value));
  m_pSetCfgImg[Index].m_Value=Value;
  //m_pSetCfgImg[Index].m_PhMask=PhMsk;
  };

// --------------------------------------------------------------------------
#define DoAdjDbg 0

static void AdjustFracsL(double *Frac, double *Reqd, int N, bool H2OOption)
  {
  ASSERT(N<MaxSpecies);
  bool RqdIsValid[MaxSpecies];
  for (int s=0; s<N; s++)
    {
    RqdIsValid[s]=Finite(Reqd[s]);
    if (RqdIsValid[s])
      Frac[s]=Range(0.0, Reqd[s], 1.0);
    }

  double TotalFrac;
  for (;;)
    {
    TotalFrac=0.0;
    int sMax=-1;
    double MaxFrac=0.0;
    for (int ss=0; ss<N; ss++)
      {
      TotalFrac+=Frac[ss];
      if (!RqdIsValid[ss] && (Frac[ss]>=MaxFrac))
        {
        sMax=ss;
        MaxFrac=Frac[ss];
        }
      }
    if (sMax<0)
      {
      if (H2OOption)
        {
        int iSn=SDB.Find("H2O(l)");
        if (iSn>=0 && iSn<N)
          sMax=iSn;
        else
          sMax=0;
        }
      else
        break;
      }

    if (sMax>=0)
      {
      double Error=(TotalFrac-1.0);
      if (TotalFrac<0.999999999)
        Frac[sMax]=Frac[sMax]-Error;
      else if ((TotalFrac>1.000000001) && (Frac[sMax]>0.0))
        {
        double OldFrac=Frac[sMax];
        Frac[sMax]=GEZ(Frac[sMax]-Error);
        TotalFrac+=(Frac[sMax]-OldFrac);
        }
      else
        break;
      }
    else
      break;
    }
  //else
  if (fabs(TotalFrac-1)>1.0e-8)
    {
    double TotalFrac=0.0;
    for (int ss=0; ss<N; ss++)
      TotalFrac+=Frac[ss];
    TotalFrac=GTZ(TotalFrac);
    for (int ss=0; ss<N; ss++)
      Frac[ss] /= TotalFrac;
    }
  }

// --------------------------------------------------------------------------

static void AdjustFracsLPhase(CSysVector & Frac, CSpMCfgImg * Reqd, PhMask Ph, bool H2OOption)
  {
  SpecieIter I(Ph);

  SpMArray DFrac, DReqd;
  int N=0;
  for (int ss=-1; I.Loop(ss); )
    {
    DFrac.VValue[N]=Frac[ss];
    DReqd.VValue[N]=Reqd[ss].m_Value;
    N++;
    }

  AdjustFracsL(DFrac, DReqd, N, H2OOption);

  // Done - Transfer back
  int i=0;
  for (int ss=-1; I.Loop(ss); )
    Frac.VValue[ss]=DFrac[i++];
  }

// --------------------------------------------------------------------------

static void AdjustFracsBU(double *Frac, double *Reqd, int N)
  {
  SpMArray New;
  double OldTotal=0;
  bool RqdIsValid[MaxSpecies];

  // Transfer Reqd in to New
  int iInxLast=-1;
  for (int ss=0; ss<N; ss++)
    {
    RqdIsValid[ss]=Finite(Reqd[ss]);
    if (RqdIsValid[ss])
      {
      New.VValue[ss]=Range(0.0, Reqd[ss], 1.0);
      iInxLast=ss;
      }
    else
      New.VValue[ss]=0;
    OldTotal+=Frac[ss];
    }

  const int ReqdTotal=1;

  for (int ss=0; ss<N; ss++)
    {
    if (!RqdIsValid[ss])
      New.VValue[ss]=Frac[ss];
    }

  double NewTotal=0;
  for (int ss=0; ss<N; ss++)
    NewTotal+=New[ss];

  // If NewTotal is less than Total copy from top and adjust
  if (NewTotal<ReqdTotal)
    {
    for (int ss=N-1; ss>=0; ss--)
      {
      if (!RqdIsValid[ss])
        {
        if (ReqdTotal>NewTotal)
          {
          New.VValue[ss]+=ReqdTotal-NewTotal;
          NewTotal+=ReqdTotal-NewTotal;
          }
        }
      }
    if (NewTotal<ReqdTotal)
      {
      int ss = N-1;
      if (ss>=0)
        {
        New.VValue[ss]+=ReqdTotal-NewTotal;
        NewTotal=ReqdTotal;
        }
      }
    }

  // If NewTotal is greater than ReqdTotal adjust the all species from iInxLast upwards
  if (NewTotal>ReqdTotal)
    {
    for (int ss=N-1; ss>=0; ss--)
      {
      double Error=NewTotal-ReqdTotal;
      if (!RqdIsValid[ss])
        {
        if (Error>New[ss])
          {
          NewTotal-=New[ss];
          New.VValue[ss]=0;
          }
        else
          {
          New.VValue[ss]-=Error;
          NewTotal-=Error;
          break; // no need to continue
          }
        }
      }
    }

  // Done - Transfer back
  for (int ss=0; ss<N; ss++)
    Frac[ss]=New[ss];
  }

// --------------------------------------------------------------------------

static void AdjustFracsBUPhase(CSysVector & Frac, CSpMCfgImg * Reqd, PhMask Ph)
  {
  SpecieIter I(Ph);

  SpMArray DFrac, DReqd;
  int N=0;
  for (int ss=-1; I.Loop(ss); )
    {
    DFrac.VValue[N]=Frac[ss];
    DReqd.VValue[N]=Reqd[ss].m_Value;
    N++;
    }

  AdjustFracsBU(DFrac, DReqd, N);

  // Done - Transfer back
  int i=0;
  for (int ss=-1; I.Loop(ss); )
    Frac.VValue[ss]=DFrac[i++];
  }

// --------------------------------------------------------------------------
#ifdef NOTUSED
static void AdjustFracsTD(CSysVector & Frac, CSpMCfgImg * Reqd, PhMask Ph, double ReqdTotal)
  {
  SpecieIter I(Ph);

  SpMArray New;
  double OldTotal=0;
  double NewTotal=0;

  if (DoAdjDbg)
    dbgpln("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++");

  // Transfer Reqd in to New
  int iInxLast=-1;
  for (int ss=-1; I.Loop(ss); )
    {
    OldTotal+=Frac[ss];
    if (Finite(Reqd[ss].m_Value))
      {
      New[ss]=Reqd[ss].m_Value;
      NewTotal+=New[ss];
      iInxLast=ss;
      if (DoAdjDbg)
        dbgpln(" Rqd     Frac %3i %-20.20s %10.3f  %10.3f  %10.3f", ss, SVI.Img(ss)->FullTag(), New[ss], NewTotal, ReqdTotal);
      }
    else
      New[ss]=0;
    }

  if (!Valid(ReqdTotal))
    ReqdTotal=OldTotal;

  for (int ss=-1; I.Loop(ss); )
    {
    if (!Finite(Reqd[ss].m_Value))
      {
      New[ss]=Frac[ss];
      NewTotal+=New[ss];
      if (DoAdjDbg && (New[ss]>0))
        dbgpln(" Cpy1    Frac %3i %-20.20s %10.3f  %10.3f  %10.3f", ss, SVI.Img(ss)->FullTag(), New[ss], NewTotal, ReqdTotal);
      }
    }

  // If NewTotal is less than Total copy from top and adjust
  if (NewTotal<ReqdTotal)
    {
    for (ss=-1; I.Loop(ss); )
      {
      if (!Finite(Reqd[ss].m_Value))
        {
        if (ReqdTotal>NewTotal)
          {
          New[ss]+=ReqdTotal-NewTotal;
          NewTotal+=ReqdTotal-NewTotal;
          if (DoAdjDbg)
            dbgpln(" Inc     Frac %3i %-20.20s %10.3f  %10.3f  %10.3f", ss, SVI.Img(ss)->FullTag(), New[ss], NewTotal, ReqdTotal);
          }
        }
      }
    if (NewTotal<ReqdTotal)
      {
      int ss = -1;//iInxLast;
      if (I.Loop(ss))
        {
        New[ss]+=ReqdTotal-NewTotal;
        NewTotal=ReqdTotal;
        if (DoAdjDbg)
          dbgpln(" Adjust  Frac %3i %-20.20s %10.3f  %10.3f  %10.3f", ss, SVI.Img(ss)->FullTag(), New[ss], NewTotal, ReqdTotal);
        }
      }
    }

  // If NewTotal is greater than ReqdTotal adjust the all species from iInxLast upwards
  if (NewTotal>ReqdTotal)
    {
    for (ss=-1; I.Loop(ss); )
      {
      double Error=NewTotal-ReqdTotal;
      if (Error>New[ss])
        {
        NewTotal-=New[ss];
        New[ss]=0;
        if (DoAdjDbg)
          dbgpln(" Cut     Frac %3i %-20.20s %10.3f  %10.3f  %10.3f", ss, SVI.Img(ss)->FullTag(), New[ss], NewTotal, ReqdTotal);
        }
      else
        {
        New[ss]-=Error;
        NewTotal-=Error;
        if (DoAdjDbg)
          dbgpln(" Cut BRK Frac %3i %-20.20s %10.3f  %10.3f  %10.3f", ss, SVI.Img(ss)->FullTag(), New[ss], NewTotal, ReqdTotal);
        break; // no need to continue
        }
      }
    }

  // Done - Transfer back
  for (int ss=-1; I.Loop(ss); )
    {
    Frac[ss]=New[ss];
    if (DoAdjDbg && (New[ss]!=0))
      dbgpln(" ==      Frac %3i %-20.20s %10.3f ", ss, SVI.Img(ss)->FullTag(), New[ss]);
    }
  }

// --------------------------------------------------------------------------

static void AdjustFracsBC(CSysVector & Frac, CSpMCfgImg * Reqd, PhMask Ph, double ReqdTotal)
  {
  SpecieIter I(Ph);

  SpMArray New;
  double OldTotal=0;
  double NewTotal=0;

  if (DoAdjDbg)
    dbgpln("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++");

  // Transfer Reqd in to New
  int iInxLast=-1;
  for (int ss=-1; I.Loop(ss); )
    {
    OldTotal+=Frac[ss];
    if (Finite(Reqd[ss].m_Value))
      {
      New[ss]=Reqd[ss].m_Value;
      NewTotal+=New[ss];
      iInxLast=ss;
      if (DoAdjDbg)
        dbgpln(" Rqd     Frac %3i %-20.20s %10.3f  %10.3f  %10.3f", ss, SVI.Img(ss)->FullTag(), New[ss], NewTotal, ReqdTotal);
      }
    else
      New[ss]=0;
    }

  if (!Valid(ReqdTotal))
    ReqdTotal=OldTotal;

  for (int ss=-1; I.Loop(ss); )
    {
    if (!Finite(Reqd[ss].m_Value))
      {
      New[ss]=Frac[ss];
      NewTotal+=New[ss];
      if (DoAdjDbg && (New[ss]>0))
        dbgpln(" Cpy1    Frac %3i %-20.20s %10.3f  %10.3f  %10.3f", ss, SVI.Img(ss)->FullTag(), New[ss], NewTotal, ReqdTotal);
      }
    }

  // If NewTotal is less than Total copy from below last and adjust
  if (NewTotal<ReqdTotal)
    {
    for (ss=iInxLast; I.Loop(ss); )
      {
      if (!Finite(Reqd[ss].m_Value))
        {
        if (ReqdTotal>NewTotal)
          {
          New[ss]+=ReqdTotal-NewTotal;
          NewTotal+=ReqdTotal-NewTotal;
          if (DoAdjDbg)
            dbgpln(" Inc     Frac %3i %-20.20s %10.3f  %10.3f  %10.3f", ss, SVI.Img(ss)->FullTag(), New[ss], NewTotal, ReqdTotal);
          }
        }
      }
    if (NewTotal<ReqdTotal)
      {
      int ss = iInxLast;
      if (I.Loop(ss))
        {
        New[ss]+=ReqdTotal-NewTotal;
        NewTotal=ReqdTotal;
        if (DoAdjDbg)
          dbgpln(" Adjust  Frac %3i %-20.20s %10.3f  %10.3f  %10.3f", ss, SVI.Img(ss)->FullTag(), New[ss], NewTotal, ReqdTotal);
        }
      }
    }

  // If NewTotal is greater than ReqdTotal adjust the all species from iInxLast upwards
  if (NewTotal>ReqdTotal)
    {
    for (ss=iInxLast; I.Loop(ss); )
      {
      double Error=NewTotal-ReqdTotal;
      if (Error>New[ss])
        {
        NewTotal-=New[ss];
        New[ss]=0;
        if (DoAdjDbg)
          dbgpln(" Cut     Frac %3i %-20.20s %10.3f  %10.3f  %10.3f", ss, SVI.Img(ss)->FullTag(), New[ss], NewTotal, ReqdTotal);
        }
      else
        {
        New[ss]-=Error;
        NewTotal-=Error;
        if (DoAdjDbg)
          dbgpln(" Cut BRK Frac %3i %-20.20s %10.3f  %10.3f  %10.3f", ss, SVI.Img(ss)->FullTag(), New[ss], NewTotal, ReqdTotal);
        break; // no need to continue
        }
      }
    }

  // If NewTotal is still greater than ReqdTotal adjust the all species from iInxLast downwards
  if (NewTotal>ReqdTotal)
    {
    for (ss=iInxLast; I.LoopRevSpc(ss); )
      {
      double Error=NewTotal-ReqdTotal;
      if (Error>New[ss])
        {
        NewTotal-=New[ss];
        New[ss]=0;
        if (DoAdjDbg)
          dbgpln(" Cut     Frac %3i %-20.20s %10.3f  %10.3f  %10.3f", ss, SVI.Img(ss)->FullTag(), New[ss], NewTotal, ReqdTotal);
        }
      else
        {
        New[ss]-=Error;
        NewTotal-=Error;
        if (DoAdjDbg)
          dbgpln(" Cut BRK Frac %3i %-20.20s %10.3f  %10.3f  %10.3f", ss, SVI.Img(ss)->FullTag(), New[ss], NewTotal, ReqdTotal);
        break; // no need to continue
        }
      }
    }

  // Done - Transfer back
  for (int ss=-1; I.Loop(ss); )
    {
    Frac[ss]=New[ss];
    if (DoAdjDbg && (New[ss]!=0))
      dbgpln(" ==      Frac %3i %-20.20s %10.3f ", ss, SVI.Img(ss)->FullTag(), New[ss]);
    }
  };
#endif

// ==========================================================================

void SpModel::VDS_UpdateAttribures()
  {
  for (int s=SVSpc1(); s<SVSpcN(); s++)
    {
    }
  for (int a=SVAtt1(); a<SVAttN(); a++)
    {
    CSysVecItem &I=SVImg(a);
    switch (I.AttrType())
      {
      case SVAT_Amnt:
        INCOMPLETECODE(__FILE__, __LINE__);
        //WS.m_MTmp.VValue[a]=m_M[a]*m_dMassScale;
        break;
      case SVAT_Qual:
        if (Finite(m_pSetCfgImg[a].m_Value))
          m_M.VValue[a]=m_pSetCfgImg[a].m_Value;
        if (Valid(I.m_dMinValue))
          m_M.VValue[a]=Max(I.m_dMinValue, m_M.VValue[a]);
        if (Valid(I.m_dMaxValue))
          m_M.VValue[a]=Min(I.m_dMaxValue, m_M.VValue[a]);
        break;
      }
    }
  };

// --------------------------------------------------------------------------

void SpModel::VDS_FindOptions(CVDS_WorkSpace & WS, bool doDbg)
  {
  WS.m_bKeepNormal=false;
  WS.m_bAdjustScale=false;

  WS.m_Op=0;
  for (int i=SVSpc1(); i<SVSpcN(); i++)
    {
    WS.m_IsAmount[i]=true;
    WS.m_MTmp.VValue[i]=m_M[i]*m_dMassScale;
    }
  for (int i=SVAtt1(); i<SVAttN(); i++)
    {
    CSysVecItem &I=SVImg(i);
    switch (I.AttrType())
      {
      case SVAT_Amnt:
        WS.m_IsAmount[i]=true;
        WS.m_MTmp.VValue[i]=m_M[i]*m_dMassScale;
        break;
      case SVAT_Qual:
        WS.m_IsAmount[i]=false;
        WS.m_MTmp.VValue[i]=m_M[i];
        //if (Valid(I.m_dMinValue))
        //  WS.m_MTmp.VValue[i]=Max(I.m_dMinValue, WS.m_MTmp.VValue[i]);
        //if (Valid(I.m_dMaxValue))
        //  WS.m_MTmp.VValue[i]=Min(I.m_dMaxValue, WS.m_MTmp.VValue[i]);
        break;
      }
    }

  for (int i=0; i<SVValueCount(); i++)
    WS.m_SclX.VValue[i]=1;

  switch (m_iSetView)
    {
    case SVV_AsRawMass:
    case SVV_AsRawMassFlow:
    case SVV_AsRawFrac:
      WS.m_Op=SVOP_Mass;
      break;
    case SVV_AsMass:
    case SVV_AsMassFlow:
      WS.m_bAdjustScale=true; // and fall thru
    case SVV_AsMassFrac:
      WS.m_bKeepNormal=m_bKeepNormalEnabled;
      if (WS.m_bKeepNormal)
        WS.m_MTmp.Normalise();
      WS.m_Op=SVOP_Mass;
      break;
    case SVV_AsMole:
    case SVV_AsMoleFlow:
      WS.m_bAdjustScale=true; // and fall thru
    case SVV_AsMoleFrac:
      WS.m_bKeepNormal=m_bKeepNormalEnabled; // and fall thru
      if (WS.m_bKeepNormal)
        WS.m_MTmp.Normalise();
      WS.m_Op=SVOP_Mole;
      for (int i=0; i<SVSpcCount(); i++)
        WS.m_SclX.VValue[i]=1/SDB[i].MoleWt();
      break;
    case SVV_AsVol:
    case SVV_AsVolFlow:
      WS.m_bAdjustScale=true; // and fall thru
    case SVV_AsVolFrac:
      WS.m_bKeepNormal=m_bKeepNormalEnabled; // and fall thru
      if (WS.m_bKeepNormal)
        WS.m_MTmp.Normalise();
      WS.m_Op=SVOP_Vol;
      for (int i=0; i<SVValueCount(); i++)
        {
        if (SDB[i].DensityDirect())
          WS.m_SclX.VValue[i]=SDB[i].msVolume(m_iFidelity, m_dSetCfgImgT, m_dSetCfgImgP, &m_Ovr, SVData());
        else
          WS.m_SclX.VValue[i]=0;
        }
      break;
    case SVV_AsNVol:
    case SVV_AsNVolFlow:
      WS.m_bAdjustScale=true; // and fall thru
    case SVV_AsNVolFrac:
      {
      WS.m_bKeepNormal=m_bKeepNormalEnabled; // and fall thru
      if (WS.m_bKeepNormal)
        WS.m_MTmp.Normalise();
      WS.m_Op=SVOP_NVol;
      for (int i=0; i<SVValueCount(); i++)
        {
        if (SDB[i].DensityDirect())
          WS.m_SclX.VValue[i]=SDB[i].msVolume(m_iFidelity, Norm_T, Norm_P, &m_Ovr, SVData());
        else
          WS.m_SclX.VValue[i]=0;
        }
      }
    break;
    }

  WS.m_MTmpPh0=0;
  WS.m_MTmpPhN=-1;
  WS.m_CalcPh0=0;
  WS.m_CalcPhN=-1;
  switch (m_iViewBasis)
    {
    case SVVB_All:
      WS.m_MTmpPh0=SpVwPhMaskIndex_All;
      WS.m_MTmpPhN=SpVwPhMaskIndex_All;
      WS.m_CalcPh0=SpVwPhMaskIndex_Solid;
      WS.m_CalcPhN=SpVwPhMaskIndex_Gas;
      break;
    case SVVB_Occ:
      WS.m_MTmpPh0=SpVwPhMaskIndex_Solid;
      WS.m_MTmpPhN=SpVwPhMaskIndex_Gas;
      WS.m_CalcPh0=SpVwPhMaskIndex_Solid;
      WS.m_CalcPhN=SpVwPhMaskIndex_Gas;
      break;
    case SVVB_Phase:
      WS.m_MTmpPh0=SpVwPhMaskIndex_Phase0;
      WS.m_MTmpPhN=SpVwPhMaskIndex_Phase0+CDB.PhaseCount()-1;
      WS.m_CalcPh0=SpVwPhMaskIndex_Phase0;
      WS.m_CalcPhN=SpVwPhMaskIndex_Phase0+CDB.PhaseCount()-1;
      break;
    }
#if dbgSpModel
  if (doDbg)
    {
    dbgpln("At Start =======================================================");
    dbgpln("Tag         : %s", FullObjTag());
    dbgpln("View        : %s", SV_ViewStr[m_iSetView]);
    dbgpln("Basis       : %s", SV_ViewBasisStr[m_iViewBasis]);
    dbgpln("KeepNormalEn: %s", m_bKeepNormalEnabled? "TRUE":"False");
    dbgpln("KeepNormal  : %s", WS.m_bKeepNormal ? "TRUE":"False");
    dbgpln("MassScale   : %14s", DbgFltString(m_dMassScale));
    dbgpln("MTmpPh0     : %3i", WS.m_MTmpPh0);
    dbgpln("MTmpPhN     : %3i", WS.m_MTmpPhN);
    dbgpln("CalcPh0     : %3i", WS.m_CalcPh0);
    dbgpln("CalcPhN     : %3i", WS.m_CalcPhN);
    dbgpln(" %14s %14s %14s %14s %14s", "Mass", "Scale", "Image", "M*MScl", "V*MScl");
    for (int s=-SVClcCount(); s<SVValueCount(); s++)
      {
      if (s>=0)
        {
        double &V=m_pSetCfgImg[s].m_Value;
        dbgp(" %14s %14s %14s %14s %14s",
          DbgFltString(m_M[s]), DbgFltString((WS.m_IsAmount[s]?WS.m_SclX[s]:1)), DbgFltString(V),
          DbgFltString(m_M[s]*(WS.m_IsAmount[s]?m_dMassScale:1)),
          DbgFltString(Valid(V)?V*(WS.m_IsAmount[s]?m_dMassScale:1):V));
        }
      else
        dbgp(" %14s %14s %14s %14s %14s", "", "", "", "", "");
      dbgpln(" %4i %#010x %4i %#010x %s",
        SVI.Img(s)->OccInx(),
        SVI.Img(s)->OccMask(),
        SVI.Img(s)->PhaseInx(),
        SVI.Img(s)->PhaseMask(),
        SVI.Img(s)->FullTag());
      }
    }
#endif

  };

//--------------------------------------------------------------------------

void SpModel::VDS_GetMTmpForAmount(CVDS_WorkSpace & WS, PhMask PhM, bool doDbg)
  {
  SpecieIter IT(PhM);
  // Check Calcs
  for (int c=IT.ClcInx1(); IT.LoopClc(c); )
    {
    if (Finite(m_pSetCfgImg[c].m_Value))
      {
#if dbgSpModel
      if (doDbg)
        dbgpln("   Calc[%3i] %s", c, DbgFltString(m_pSetCfgImg[c].m_Value), SVI.Img(c)->FullTag());
#endif
      CIArray &Inx=SVI.Img(c)->FnIndicesOfSumSpcs();
      double New= m_pSetCfgImg[c].m_Value;
      double Old=m_M.ValueOp(c, WS.m_Op, Fidelity(), WS.m_TheTemp, WS.m_ThePress);
      if (fabs(New)>1.0e-20)
        SetDataSign((int)Sign(New));
      if (Old>1e-20)
        {
        double Scl=(New)/GTZ(Old);
        for (int i=0; i<Inx.GetSize(); i++)
          {
          int s=Inx[i];
          m_pSetCfgImg[s].m_Value=m_M[s]*Scl*CSysVector::Mass2OtherFactor(s, WS.m_Op, Fidelity(), WS.m_TheTemp, WS.m_ThePress, SVData());
#if dbgSpModel
          if (doDbg)
            dbgpln("   A Img[%3i]=%14s %s", s, DbgFltString(m_pSetCfgImg[s].m_Value), SVI.Img(s)->FullTag());
#endif
          }
        }
      else
        {
        CSysVecTemplateItem *pTemplate=SVI.m_Templates.FindTemplate(m_sMainTemplate.StrC());
        if (pTemplate)
          {
          double Tmp=0;
          for (int i=0; i<Inx.GetSize(); i++)
            {
            int s=Inx[i];
            if (Valid(pTemplate->m_Values[s]))
              Tmp+=pTemplate->m_Values[s]*CSysVector::Mass2OtherFactor(s, WS.m_Op, Fidelity(), WS.m_TheTemp, WS.m_ThePress, SVData());
            };
          if (Tmp>1e-30)
            {
            double Scl=New/Tmp;
            for (int i=0; i<Inx.GetSize(); i++)
              {
              int s=Inx[i];
              if (Valid(pTemplate->m_Values[s]))
                {
                m_pSetCfgImg[s].m_Value=pTemplate->m_Values[s]*Scl*CSysVector::Mass2OtherFactor(s, WS.m_Op, Fidelity(), WS.m_TheTemp, WS.m_ThePress, SVData());
#if dbgSpModel
                if (doDbg)
                  dbgpln("   B Img[%3i]=%14s %s", s, DbgFltString(m_pSetCfgImg[s].m_Value), SVI.Img(s)->FullTag());
#endif
                }
              }
            }
          else
            pTemplate=NULL;
          }

        if (!pTemplate)
          {
          int s=Inx[0];
          m_pSetCfgImg[s].m_Value=fabs(New);
#if dbgSpModel
          if (doDbg)
            dbgpln("   C Img[%3i]=%14s %s", s, DbgFltString(m_pSetCfgImg[s].m_Value), SVI.Img(s)->FullTag());
#endif
          }
        }
      }
    }

  for (int s=IT.SpcInx1(); IT.LoopAll(s); )
    {
    if (Finite(m_pSetCfgImg[s].m_Value))
      {
      if (WS.m_IsAmount[s])
        WS.m_MTmp.VValue[s]=m_pSetCfgImg[s].m_Value/WS.m_SclX[s];
      else
        WS.m_MTmp.VValue[s]=m_pSetCfgImg[s].m_Value;
      }
    }
  }

// --------------------------------------------------------------------------

void SpModel::VDS_GetMTmpForFrac(CVDS_WorkSpace & WS, PhMask PhM, bool doDbg)
  {
  SpecieIter IT(PhM);
  double Total=0;
  double PhValue=0;
  switch (m_iSetView)
    {
    case SVV_AsMass:
    case SVV_AsMassFlow:
    case SVV_AsMassFrac:
      Total=Mass();
      PhValue=Mass(PhM);
      break;
    case SVV_AsSpcConc:
      Total=Volume();
      PhValue=Mass(PhM);
      break;
    case SVV_AsMole:
    case SVV_AsMoleFlow:
    case SVV_AsMoleFrac:
      Total=Moles();
      PhValue=Moles(PhM);
      break;
    case SVV_AsVol:
    case SVV_AsVolFlow:
    case SVV_AsVolFrac:
      Total=Volume();
      PhValue=Volume(PhM);
      break;
    case SVV_AsNVol:
    case SVV_AsNVolFlow:
    case SVV_AsNVolFrac:
      Total=NVolume();
      PhValue=NVolume(PhM);
      break;
    }

  double PhFrac=PhValue/GTZ(Total);

  bool HasRqd=false;
  double TotRqdImg=0.0;
  for (int ss=IT.SpcInx1(); IT.Loop(ss); )// && !HasRqd; )
    {
    if (Finite(m_pSetCfgImg[ss].m_Value))
      {
      HasRqd=true;
      TotRqdImg+=m_pSetCfgImg[ss].m_Value;
      }
    }

  if (HasRqd)
    {
    //dbgpln("HasRqd Total    : %14.4g", Total);
    //dbgpln("       PhValue  : %14.4g", PhValue);
    //dbgpln("       PhFrac   : %14.4g", PhFrac);
    //dbgpln("       TotRqdImg: %14.4g", TotRqdImg);

    if (Total<SmallMassFrac)
      {
      Total=1;
      PhFrac=1;
      }
    else if (PhFrac<SmallMassFrac)
      {
      PhFrac=Total*SmallMassFrac;
      }
    //double PhFracDivisor = (PhFrac>=1e-20) ? PhFrac : 1e-20;

    SpMArray Frac;
    SpMArray Reqd;
    double TotRqd=0.0;
    int N=0;
    for (int ss=IT.SpcInx1(); IT.LoopAll(ss); )
      {
      if (WS.m_IsAmount[ss])
        {
        Frac.VValue[N]=m_M[ss]*WS.m_SclX[ss]/PhFrac;//Divisor;
        if (Valid(m_pSetCfgImg[ss].m_Value))
          {
          Reqd.VValue[N]=m_pSetCfgImg[ss].m_Value;//*WS.m_SclX[ss]/GTZ(PhFrac*NrmScl);
          TotRqd+=Reqd.VValue[N];
          }
        else
          Reqd.VValue[N]=m_pSetCfgImg[ss].m_Value;
        N++;
        }
      }
#if dbgSpModel
    if (doDbg)
      {
      dbgpln("Before Adjust [MassScale=%14s]------------------------", DbgFltString(m_dMassScale));
      dbgpln(" %14s %14s %14s %14s", "Mass", "Scale", "Reqd", "Frac");
      int i=0;
      for (int ss=IT.SpcInx1(); IT.LoopAll(ss); i++)
        dbgpln(" %14s %14s %14s %14s  %s", DbgFltString(m_M[ss]), DbgFltString(WS.m_SclX[ss]), DbgFltString(Reqd[i]), DbgFltString(Frac[i]), SVI.Img(ss)->FullTag());
      }
#endif

    switch (sm_iFixupStrategy)
      {
      case FUS_BottomUp:
        AdjustFracsBU(Frac, Reqd, N);
        break;
      case FUS_Largest:
      default:
        AdjustFracsL(Frac, Reqd, N, true);
        break;
      }

    N=0;
    for (int ss=IT.SpcInx1(); IT.LoopAll(ss); )
      {
      if (WS.m_IsAmount[ss])
        WS.m_MTmp.VValue[ss]=PhFrac*Frac[N++]/GTZ(WS.m_SclX[ss]);
      else if (Valid(m_pSetCfgImg[ss].m_Value))
        WS.m_MTmp.VValue[ss]=m_pSetCfgImg[ss].m_Value;
      else
        WS.m_MTmp.VValue[ss]=m_M[ss];
      }

#if dbgSpModel
    if (doDbg)
      {
      dbgpln("After  Adjust [MassScale=%14s]------------------------", DbgFltString(m_dMassScale));
      dbgpln(" %14s %14s %14s %14s %14s", "Mass", "MTmp", "Scale", "Reqd", "Frac");
      int i=0;
      for (int ss=IT.SpcInx1(); IT.LoopAll(ss); i++)
        dbgpln(" %14s %14s %14s %14s %14s  %s",
        DbgFltString(m_M[ss]),
        DbgFltString(WS.m_MTmp[ss]),
        DbgFltString(WS.m_SclX[ss]),
        DbgFltString(Reqd[i]), DbgFltString(Frac[i]), SVI.Img(ss)->FullTag());
      }
#endif
    }
  else
    {
    for (int ss=IT.SpcInx1(); IT.LoopAll(ss); )
      WS.m_MTmp.VValue[ss]=m_M[ss];
    }
  };

// --------------------------------------------------------------------------

void SpModel::VDS_UpdateFractions(CVDS_WorkSpace &WS, bool doDbg)
  {
  // Check Calcs
  double TotalValue = 0;
  double PhValue[SpVwPhMaskIndex_Count];
  double PhFrac[SpVwPhMaskIndex_Count];
  double RqdPhFrac[SpVwPhMaskIndex_Count];
  double OldPhFrac[SpVwPhMaskIndex_Count];

  for (int p=WS.m_CalcPh0; p<=WS.m_CalcPhN; p++)
    {
    PhMask PhM = CSysVecInfo::SpVwPhMask(p);
    switch (m_iSetView)
      {
      case SVV_AsMassFrac: PhValue[p]=Mass(PhM);  break;
      case SVV_AsMoleFrac: PhValue[p]=Moles(PhM);  break;
      case SVV_AsVolFrac:  PhValue[p]=Volume(PhM);  break;
      case SVV_AsNVolFrac: PhValue[p]=NVolume(PhM);  break;
      }
    TotalValue += PhValue[p];
    }
  TotalValue = GTZ(TotalValue);
  for (int p=WS.m_CalcPh0; p<=WS.m_CalcPhN; p++)
    {
    PhFrac[p]    = PhValue[p]/TotalValue;
    OldPhFrac[p] = PhFrac[p];
    RqdPhFrac[p] = dNAN;
    }

  for (int c=SVClc1(); c<SVClcN(); c++)
    {
    if (Finite(m_pSetCfgImg[c].m_Value))
      {
      switch (SVI.Img(c)->CalcType())
        {
        case SVCT_Sum:
        case SVCT_PhSum:
        case SVCT_IPhSum:
          {
          double RqdFrac = m_pSetCfgImg[c].m_Value;
          double ActFrac = 0;
          for (int p=WS.m_CalcPh0; p<=WS.m_CalcPhN; p++)
            {
            PhMask PhM = CSysVecInfo::SpVwPhMask(p);
            if (PhM & SVI.Img(c)->PhaseMask())
              ActFrac += PhFrac[p];
            }
          ActFrac = GTZ(ActFrac);

          // Apportion RqfFrac across all species
          int pLast=-1;
          double RqdFracTotal=0.0;
          for (int p=WS.m_CalcPh0; p<=WS.m_CalcPhN; p++)
            {
            PhMask PhM = CSysVecInfo::SpVwPhMask(p);
            if (PhM & SVI.Img(c)->PhaseMask())
              {
              RqdPhFrac[p]=RqdFrac/ActFrac*PhFrac[p];
              RqdFracTotal+=RqdPhFrac[p];
              pLast=p;
              }
            }
          if (RqdFracTotal<1e-20 && pLast>=0) // Mass in Phase is too small - just set the last last specie
            RqdPhFrac[pLast]=RqdFrac;
          }
        }
      }
    }
#if dbgSpModel
  if (doDbg)
    {
    dbgpln("PhValue %2s %10s %14s %14s %14s", "Ph", "Mask", "OldPhFrac", "PhFrac", "RqdPhFrac");
    for (int p=WS.m_CalcPh0; p<=WS.m_CalcPhN; p++)
      dbgpln("PhValue %2i %#010x %14.4g %14.4g %14s  ", p, CSysVecInfo::SpVwPhMask(p), OldPhFrac[p], PhFrac[p], DbgFltString(RqdPhFrac[p]));
    }
#endif

  switch (sm_iFixupStrategy)
    {
    case FUS_BottomUp:
      AdjustFracsBU(&PhFrac[WS.m_CalcPh0], &RqdPhFrac[WS.m_CalcPh0], WS.m_CalcPhN-WS.m_CalcPh0+1);
      break;
      //case xFUS_TopDown:
      //  AdjustFracsTD(Frac, m_pSetCfgImg, PhM, 1.0);
      //  break;
      //case xFUS_BelowCurrent:
      //  AdjustFracsBC(Frac, m_pSetCfgImg, PhM, 1.0);
      //  break;
    case FUS_Largest:
    default:
      AdjustFracsL(&PhFrac[WS.m_CalcPh0], &RqdPhFrac[WS.m_CalcPh0], WS.m_CalcPhN-WS.m_CalcPh0+1, false);
      break;
    }

#if dbgSpModel
  if (doDbg)
    dbgpln("Rqd        %10s %14s ", "Mask", "RqdFrac");
#endif
  for (int p=WS.m_CalcPh0; p<=WS.m_CalcPhN; p++)
    {
    PhMask PhM = CSysVecInfo::SpVwPhMask(p);
#if dbgSpModel
    if (doDbg)
      dbgp("Rqd     %2i %#010x %14s   ", p, PhM, DbgFltString(RqdPhFrac[p]));
#endif
    if (PhValue[p]>1e-30) // Sufficient Mass to Scale up/down
      {
      double Multiplier=PhFrac[p]/GTZ(OldPhFrac[p]);
      if (PhFrac[p]<1e-30 && OldPhFrac[p]>=1.0e-30)
        Multiplier=1.00001e-30/GTZ(OldPhFrac[p]);

      m_M.Mult(PhM, Multiplier);
#if dbgSpModel
      if (doDbg)
        dbgpln("Mult * %14s %14s %14s %14s ", DbgFltString(Multiplier), DbgFltString(PhFrac[p]), DbgFltString(OldPhFrac[p]), DbgFltString(PhFrac[p]/GTZ(OldPhFrac[p])));
#endif
      }
    else // Mass too small - set the last specie in Phase
      {
      SpecieIter IT(PhM);
      int iLast=-1;
      for (int i=IT.SpcInx1(); IT.Loop(i); )
        iLast=i;

      if (iLast>=0)
        m_M.VValue[iLast]=PhFrac[p]*TotalValue/CSysVector::Mass2OtherFactor(iLast, WS.m_Op, Fidelity(), WS.m_TheTemp, WS.m_ThePress, SVData());
#if dbgSpModel
      if (doDbg)
        dbgpln("SET M[%3i] %14s", iLast, DbgFltString(PhFrac[p]*TotalValue));
#endif
      }
    }
  ClrStatesOK();
  TESTMASSNOTNEG();
  SetTempPress(WS.m_TheTemp, WS.m_ThePress);
  };

// --------------------------------------------------------------------------

flag SpModel::ValidateData_Species(ValidateDataBlk & VDB)
  {
  #if dbgSpModel
  const bool doDbg=0||dbgValidateSpc() || dbgValidateSpcBrk() && DoDbgBrk();
  if (doDbg)
    {
    dbgpln(">>> ######################################################################################");
    dbgpln("SpModel::ValidateDataSpc %s", FullObjTag());
    dbgpln("CfgImgSet                %s", m_bCfgImgSet?"TRUE":"");
    dbgpln("View                     %s", SV_ViewStr[m_iSetView]);
    for (int s=-SVClcCount(); s<SVValueCount(); s++)
      dbgpln("%3i) %14s %14s  %s",s, s>=0?DbgFltString(m_M[s]):"", m_bCfgImgSet ? DbgFltString(m_pSetCfgImg[s].m_Value):"", SVImg(s).SymOrTag());
    }
  #else
  const bool doDbg=0;
  #endif
  double TheTemp=Temp();
  double ThePress=Press();
  if (m_bCfgImgSet)//m_iSetView!=SVV_NULL)
    {
    if (m_iSetView==SVV_NULL)
      {
      VDS_UpdateAttribures();
      }
    else
      {
      CVDS_WorkSpace WS;
      WS.m_TheTemp=TheTemp;
      WS.m_ThePress=ThePress;
      VDS_FindOptions(WS, doDbg);

      for (int p=WS.m_MTmpPh0; p<=WS.m_MTmpPhN; p++)
        {
        PhMask PhM = CSysVecInfo::SpVwPhMask(p);

        #if dbgSpModel
        if (doDbg)
          {
          dbgpln("%2i) Phase %#010x --------------------------------------------------------------------------------------", p, PhM);
          dbgindent(+2);
          }
        #endif

        switch (m_iSetView)
          {
          case SVV_AsMass:
          case SVV_AsMassFlow:
          case SVV_AsMole:
          case SVV_AsMoleFlow:
          case SVV_AsVol:
          case SVV_AsVolFlow:
          case SVV_AsNVol:
          case SVV_AsNVolFlow:
            {
            VDS_GetMTmpForAmount(WS, PhM, doDbg);
            break;
            }
          case SVV_AsMassFrac:
          case SVV_AsMoleFrac:
          case SVV_AsVolFrac:
          case SVV_AsNVolFrac:
            {
            VDS_GetMTmpForFrac(WS, PhM, doDbg);
            break;
            }
          }
        #if dbgSpModel
        if (doDbg)
          {
          //dbgpln("%2i) Phase %#010x --------------------------------------------------------------------------------------", p, PhM);
          dbgindent(-2);
          }
        #endif
        }

      if (WS.m_bKeepNormal)
        {
        double NewTot=WS.m_MTmp.Mass();
        WS.m_MTmp.Normalise();
        if (WS.m_bAdjustScale)
          SetMassScale((m_dMassScale>0.0) ? m_dMassScale*=NewTot : NewTot, "Adjust");
        #if dbgSpModel
        if (0 || doDbg)
          dbgpln("WS.m_MTmp.Normalise()");
        #endif
        }

      for (int i=0; i<SVValueCount(); i++)
        {
        m_M.VValue[i]=WS.m_MTmp[i];
        #if dbgSpModel
        if (doDbg)
          dbgpln("M[%3i] = %14s  %s", i, DbgFltString(m_M[i]), SVI.Img(i)->FullTag());
        #endif
        }

      ClrStatesOK();
      TestMStatesOK();
      TESTMASSNOTNEG();
      SetTempPress(TheTemp, ThePress);

      // Now Calcs for Fraction Part
      switch (m_iSetView)
        {
        case SVV_AsMassFrac:
        case SVV_AsMoleFrac:
        case SVV_AsVolFrac:
        case SVV_AsNVolFrac:
          {
          VDS_UpdateFractions(WS, doDbg);
          break;
          }
        }

      #if dbgSpModel
      if (0|| doDbg)
        {
        dbgpln("Final ---------------------------------------------------------- Scl:%10.3e", m_dMassScale);
        dbgpln(" %14s %14s %14s %14s %14s", "Mass", "Scale", "Image", "M*MScl", "V*MScl");
        for (int s=0; s<SVSpcCount(); s++)
          dbgpln(" %14s %14s %14s %14s %14s %s",
            DbgFltString(m_M[s]), DbgFltString(WS.m_SclX[s]), DbgFltString(m_pSetCfgImg[s].m_Value),
            DbgFltString(m_M[s]*m_dMassScale),
            m_bCfgImgSet?DbgFltString(Valid(m_pSetCfgImg[s].m_Value)?m_pSetCfgImg[s].m_Value*m_dMassScale:m_pSetCfgImg[s].m_Value):"",
            SVI.Img(s)->FullTag());
        dbgpln("================================================================");
        }
      #endif
      }

    ClrStatesOK();
    TestMStatesOK();
    TESTMASSNOTNEG();
    SetTempPress(TheTemp, ThePress);
    m_iSetView=SVV_NULL;
    m_bCfgImgSet=false;
    m_bSpeciesAdjusted=true;
    }
  #if dbgSpModel
  if (doDbg)
    {
    for (int s=-SVClcCount(); s<SVValueCount(); s++)
      dbgpln("%3i) %14s %14s  %s",s, s>=0?DbgFltString(m_M[s]):"", m_bCfgImgSet ? DbgFltString(m_pSetCfgImg[s].m_Value):"", SVImg(s).SymOrTag());
    dbgpln("<<< ######################################################################################");
    }
  #endif
  return 1;
  };

// --------------------------------------------------------------------------

void SpModel::BuildDataDefn_QualsSlct(DataDefnBlk & DDB, flag AsParms)
  {
  DWORD Flag=SetOnChange;
  if (SQFlagsSet(SQF_CreateDsp|SQF_RemoveDsp))
    Flag|=isParm;

  DDB.Text("Qualities", Marker/*|MarkerClosed*/);
  DDB.Byte("", "EquilFndOn", DC_, "", &m_bEquilOn, this, InitHidden|isParmConstruct, DDBYesNo);
  DDBValueLstMem DDBX;
  if (SQFlagsSet(SQF_TearSet))
    {
    DDBX.Add(SQA_Create, "Set in Tear");
    DDBX.Add(SQA_Xfer, "-");
    }
  else if (SQFlagsSet(SQF_Source))
    {
    DDBX.Add(SQA_Create, "Create");
    DDBX.Add(SQA_Xfer, "-");
    }
  else
    {
    DDBX.Add(SQA_Xfer, "-");
    if (SQFlagsSet(SQF_CreateDsp))
      DDBX.Add(SQA_Create, "Create");
    if (SQFlagsSet(SQF_RemoveDsp))
      DDBX.Add(SQA_Remove, "Remove");
    }

  DDB.Byte("", "EquilFnd", DC_, "", xidEquilSwitch, this, Flag, &DDBX);
  if (m_bEquilOn)
    DDB.TagComment("On");
#if WITHAUTOVLE
  if (AutoVLEOK())// && m_bEquilOn)
    {

    //DDB.CheckBoxBtn("", "AutoVLE", DC_, "", &m_AutoVLE.m_bOn, this, Flag);
    DDBValueLstMem DDB1;
    if (m_bEquilOn)
      {
      DDB1.Add(AutoVLEOff, "Off");
      //DDB1.Add(CDB.No(), "MultiComponent");
      for (int i=0; i<CDB.m_Flashable.GetCount(); i++)
        {
        int j=CDB.m_Flashable[i];
        DDB1.Add(j, CDB[j].SymOrTag());
        }
      }
    else
      DDB1.Add(AutoVLEDisabled, "Disabled");

    // RESTORE
    //DDB.Long  ("AutoVLE",    "",  DC_,     "",    xidAutoVLEComp,    this, m_bEquilOn ? isParm:0, DDB1());
    }
#endif

  Strng SDesc;
  for (TagObjClass* pQ=TagObjClass::FindGroupId(SpQuality::GroupName); pQ; pQ=pQ->NextClassInGrp())
    {
    if (pQ->Selectable() && pQ->LicenseOK())
      {
      int QNo=pQ->GroupIdNo();
      SDesc=pQ->ShortDesc();
      // remove spaces from desc
      for (int i=-1; ((i=SDesc.Find(" "))>=0); )
        SDesc[i]='_';

      if (PrjFileVerNo() < 19)
        {
        DDBValueLstMem DDB0;
        DDB0.Add(0, "-");
        DDB0.Add(0, "Present");
        if (SQFlagsSet(SQF_CreateDsp))
          DDB0.Add(0, QualityPtr(QNo, false) ? "Create {*}" : "Create {-}");
        if (SQFlagsSet(SQF_RemoveDsp))
          DDB0.Add(0, QualityPtr(QNo, false) ? "Remove {*}" : "Remove {-}");
        DDB.String(pQ->ClassId(),  SDesc(),    DC_,    "",    xidQualSwitch+pQ->GroupIdNo(),     this, Flag, DDB0());
        }
      else
        {
        Strng Nmm(pQ->ClassId());
        Strng NmmA(pQ->ClassId());
        Strng NmmD(SDesc());
        Nmm+="On";
        NmmA+="Act";
        NmmD+="Act";
        DDB.Byte("", Nmm(), DC_, "", xidQualSwitch1+pQ->GroupIdNo(), this,InitHidden|isParmConstruct|SetOnChange, DDBYesNo);
        DDBValueLstMem DDBX;
        if (SQFlagsSet(SQF_TearSet))
          {
          DDBX.Add(SQA_Create, "Set in Tear");
          DDBX.Add(SQA_Xfer, "-");
          }
        else if (SQFlagsSet(SQF_Source))
          {
          DDBX.Add(SQA_Create, "Create");
          DDBX.Add(SQA_Xfer, "-");
          }
        else
          {
          DDBX.Add(SQA_Xfer, "-");
          if (SQFlagsSet(SQF_CreateDsp))
            DDBX.Add(SQA_Create, "Create");
          if (SQFlagsSet(SQF_RemoveDsp))
            DDBX.Add(SQA_Remove, "Remove");
          }

        if (PrjFileVerNo() < 21)
          DDB.Byte(SDesc(), pQ->ClassId(), DC_, "", xidQualSwitch2+pQ->GroupIdNo(), this,Flag, &DDBX);
        else
          DDB.Byte(NmmD(), NmmA(), DC_, "", xidQualSwitch2+pQ->GroupIdNo(), this,Flag|SetOnChange, &DDBX);
        if (QualityPtr(QNo, false))
          DDB.TagComment("On");
        }
      //Does Nothing: if (Quals[pQ->GroupIdNo()].Exists())
      //Does Nothing:   Quals[pQ->GroupIdNo()].m_pQ->BuildDataDefn_Lcl(DDB);
      }
    }
  }

// --------------------------------------------------------------------------

flag SpModel::Has_Quals()
  {
  TagObjClass * pQ;
  for (pQ=TagObjClass::FindGroupId(SpQuality::GroupName); pQ; pQ=pQ->NextClassInGrp())
    if (Quals[pQ->GroupIdNo()].Exists())
      return true;
  return false;
  }

// --------------------------------------------------------------------------

void SpModel::BuildDataDefn_Quals(DataDefnBlk & DDB, flag AsParms)
  {
  TagObjClass* pQ;
  for (pQ=TagObjClass::FindGroupId(SpQuality::GroupName); pQ; pQ=pQ->NextClassInGrp())
    if (Quals[pQ->GroupIdNo()].Exists())
      {
      Quals[pQ->GroupIdNo()].m_pQ->BuildDataDefn(DDB);
      if (Quals[pQ->GroupIdNo()].m_pQP)
        Quals[pQ->GroupIdNo()].m_pQP->BuildDataDefn(DDB);
      }
  }

// --------------------------------------------------------------------------

#define PARM(N) DCB.pDataInfo->m_dParms[N]

flag SpModel::DataXchg(DataChangeBlk & DCB)
  {
  ASSERT(MaxCalculations<MaxSpeciesEx);
  ASSERT(xidSpcEndOfList<xidSpUsrProps);

  if (DataXchg_Species(DCB))
    return 1;

  switch (DCB.lHandle)
    {
    case xidSMUsage:
      if (DCB.rB)
        SetUsage(SpMdlUsage(*DCB.rB));
      DCB.B=m_iUsage;
      return 1;
    case xidSMPress:
      if (DCB.rD!=NULL)
        {
        if (DCB.ForFiling())
          SetFilePress(*DCB.rD);
        else
          SetPress(*DCB.rD);
        }
      DCB.D = Press();
      return 1;
    case xidSMTemp:
      if (DCB.rD!=NULL)
        {
        double m_RqdTemp=Range(0.0, *DCB.rD, 10000.0);
        if (DCB.ForFiling())
          SetFileTemp(m_RqdTemp);
        else
          SetTemp(m_RqdTemp);
        }
      DCB.D=Temp();
      return 1;
    case xidSMViewGlbl:
      if (m_pGlblView)
        {
        if (1)//!DCB.ForFiling())
          {
          if (DCB.rB)
            m_pGlblView->m_bOn=*DCB.rB;
          DCB.B=m_pGlblView->m_bOn;
          }
        else
          DCB.B=false;
        }
      return 1;
    case xidSMView:
      if (DCB.rB)
        {
        SetView(SV_View(*DCB.rB));
        //byte b=GetViewBasis(DCB.ForFiling()?SVForce_Lcl:SVForce_None);

        }
      DCB.B=GetView(DCB.ForFiling()?SVForce_Lcl:SVForce_None);
      return 1;
    case xidSMViewL:
      if (DCB.rB)
        {
        SetView(SV_View(*DCB.rB));
        //byte b=GetViewBasis(DCB.ForFiling());
        }
      DCB.B=GetView(SVForce_Lcl);
      return 1;
    case xidSMViewG:
      if (DCB.rB)
        {
        SetView(SV_View(*DCB.rB));
        //byte b=GetViewBasis(DCB.ForFiling());
        }
      DCB.B=GetView(SVForce_Glbl);
      return 1;
    case xidSMViewBasisMass:
      if (DCB.rB)
        SetViewBasis(SV_ViewBasis(*DCB.rB));// ? SVVB_Phase : SVVB_All);
      DCB.B=GetViewBasis(DCB.ForFiling()?SVForce_Lcl:SVForce_None);//==SVVB_Phase;
      return 1;
    case xidSMViewBasisElem:
      if (DCB.rB)
        SetViewBasis(SV_ViewBasis(*DCB.rB));
      DCB.B=GetViewBasis(DCB.ForFiling()?SVForce_Lcl:SVForce_None);
      return 1;

    case xidSMTMass       : DCB.D = m_bUsableMass || !DCB.ForView() ? m_iDataSgn * m_dMassScale * Mass(som_ALL) : 0.0; return 1;
    case xidSMSMass       : DCB.D = m_bUsableMass || !DCB.ForView() ? m_iDataSgn * m_dMassScale * Mass(som_Sol) : 0.0;              return 1;
    case xidSMLMass       : DCB.D = m_bUsableMass || !DCB.ForView() ? m_iDataSgn * m_dMassScale * Mass(som_Liq) : 0.0;              return 1;
    case xidSMVMass       : DCB.D = m_bUsableMass || !DCB.ForView() ? m_iDataSgn * m_dMassScale * Mass(som_Gas) : 0.0;              return 1;
    case xidSMSLMass      : DCB.D = m_bUsableMass || !DCB.ForView() ? m_iDataSgn * m_dMassScale * Mass(som_SL) : 0.0;               return 1;

    case xidSMTVol        : DCB.D = m_bUsableMass || !DCB.ForView() ? m_iDataSgn * m_dMassScale * Volume(som_ALL) : 0.0;            return 1;
    case xidSMSVol        : DCB.D = m_bUsableMass || !DCB.ForView() ? m_iDataSgn * m_dMassScale * Volume(som_Sol) : 0.0;            return 1;
    case xidSMLVol        : DCB.D = m_bUsableMass || !DCB.ForView() ? m_iDataSgn * m_dMassScale * Volume(som_Liq) : 0.0;            return 1;
    case xidSMVVol        : DCB.D = m_bUsableMass || !DCB.ForView() ? m_iDataSgn * m_dMassScale * Volume(som_Gas) : 0.0;            return 1;
    case xidSMSLVol       : DCB.D = m_bUsableMass || !DCB.ForView() ? m_iDataSgn * m_dMassScale * Volume(som_SL) : 0.0;             return 1;

    case xidSMTNVol       : DCB.D = m_bUsableMass || !DCB.ForView() ? m_iDataSgn * m_dMassScale * NVolume(som_ALL) : 0.0;            return 1;
    case xidSMSNVol       : DCB.D = m_bUsableMass || !DCB.ForView() ? m_iDataSgn * m_dMassScale * NVolume(som_Sol) : 0.0;            return 1;
    case xidSMLNVol       : DCB.D = m_bUsableMass || !DCB.ForView() ? m_iDataSgn * m_dMassScale * NVolume(som_Liq) : 0.0;            return 1;
    case xidSMVNVol       : DCB.D = m_bUsableMass || !DCB.ForView() ? m_iDataSgn * m_dMassScale * NVolume(som_Gas) : 0.0;            return 1;
    case xidSMSLNVol      : DCB.D = m_bUsableMass || !DCB.ForView() ? m_iDataSgn * m_dMassScale * NVolume(som_SL) : 0.0;             return 1;

    case xidSMRho         : DCB.D = Rho(som_ALL, Temp(), Press());  return 1;
    case xidSMSRho        : DCB.D = Rho(som_Sol, Temp(), Press());  return 1;
    case xidSMLRho        : DCB.D = Rho(som_Liq, Temp(), Press());  return 1;
    case xidSMVRho        : DCB.D = Rho(som_Gas, Temp(), Press());  return 1;
    case xidSMSLRho       : DCB.D = Rho(som_SL , Temp(), Press());  return 1;

    case xidSMMoleWt      : DCB.D = MoleWt(som_ALL);                return 1;
    case xidSMCpCv        : DCB.D = CpCv(som_ALL);                  return 1;

    case xidSMSFrac       : DCB.D = MassFrac(som_Sol);              return 1;
    case xidSMLFrac       : DCB.D = MassFrac(som_Liq);              return 1;
    case xidSMVFrac       : DCB.D = MassFrac(som_Gas);              return 1;

    case xidSMMlSFrac     : DCB.D = MoleFrac(som_Sol);              return 1;
    case xidSMMlLFrac     : DCB.D = MoleFrac(som_Liq);              return 1;
    case xidSMMlVFrac     : DCB.D = MoleFrac(som_Gas);              return 1;

    case xidSMPureSatTemp : DCB.D = PureSaturationT(Press());       return 1;
    case xidSMSatComp     : 
      {
      if (DCB.rpC)
        {
        if (stricmp(DCB.rpC, "<Default>")==0)
          {
          m_iSatCmp=CDB.DefFlashCmpIndex();
          m_iSatCmpRqd=-1;
          }
        else
          {
          int i=sm_DDBFlashable.Find(DCB.rpC);
          if (i>=0)  
            {
            i=sm_DDBFlashable[i].m_lVal;
            m_iSatCmp=i;
            m_iSatCmpRqd=i;
            }
          }
        }
      if (m_iSatCmpRqd>=0)
        DCB.pC = sm_FlashCmps[m_iSatCmpRqd]();
      else if (m_iSatCmp>=0)
        DCB.pC = sm_FlashCmpsI[m_iSatCmp]();
      else
        DCB.pC = sm_FlashCmpsI[CDB.DefFlashCmpIndex()]();
      return 1;
      }
    case xidSMSatTempP    : DCB.D = SaturationT(Press(), NULL, SaturationCmpIndex());   return 1;
    case xidSMSatTempPP   : DCB.D = Mass(som_Gas)>SmallMassFrac ? SaturationT(PartialPressC(SaturationCmpIndex(), -1, Temp()), NULL, SaturationCmpIndex()) : dNAN;   return 1;
    case xidSMSatPress    : DCB.D = SaturationP(Temp(), NULL, SaturationCmpIndex());    return 1;
    case xidSMSatPPress   : DCB.D = PartialPressC(SaturationCmpIndex(), -1, Temp());     return 1;
    case xidSMBoilPtEle   : DCB.D = BoilingPtElevation(Press());    return 1;

    case xidSMtotHsFn     : DCB.D=totHs(som_ALL, PARM(0), PARM(1), NULL); return true;
    case xidSMtotHzFn     : DCB.D=totHz(som_ALL, PARM(0), PARM(1), NULL); return true;
    case xidSMtotHfFn     : DCB.D=totHf(som_ALL, PARM(0), PARM(1), NULL); return true;

    case xidSMmsLatHtVap  : DCB.D = msLatentHeatVap(Temp(), Press()); return 1;
    case xidSMSmsH        : DCB.D = msHz(som_Sol, Temp(), Press());   return 1;
    case xidSMLmsH        : DCB.D = msHz(som_Liq, Temp(), Press());   return 1;
    case xidSMVmsH        : DCB.D = msHz(som_Gas, Temp(), Press());   return 1;
    case xidSMSLmsH       : DCB.D = msHz(som_SL,  Temp(), Press());   return 1;
    case xidSMSmsCp       : DCB.D = msCp(som_Sol, Temp(), Press());   return 1;
    case xidSMLmsCp       : DCB.D = msCp(som_Liq, Temp(), Press());   return 1;
    case xidSMVmsCp       : DCB.D = msCp(som_Gas, Temp(), Press());   return 1;
    case xidSMSLmsCp      : DCB.D = msCp(som_SL,  Temp(), Press());   return 1;

    case xidSMmsH         : DCB.D = msHs();                           return 1;
    case xidSMmsHz        : DCB.D = msHz();                           return 1;
    case xidSMmsHf        : DCB.D = msHf();                           return 1;
    case xidSMmsHf0       : DCB.D = msHf(som_ALL, ZeroCinK, SaturationP(ZeroCinK)); return 1;
#if WithExtraEntropy_Sf
    case xidSMmsSf        : DCB.D = msSf();                           return 1;
    case xidSMmsSf0       : DCB.D = msSf(som_ALL, ZeroCinK, SaturationP(ZeroCinK)); return 1;
#endif
    case xidSMmsCp        : DCB.D = msCp();                           return 1;
    case xidSMmsCp0       : DCB.D = msCp(som_ALL, ZeroCinK, SaturationP(ZeroCinK)); return 1;
    case xidSMmsCpUt      : DCB.D = msCp(som_ALL, dUsrDefT, Valid(dUsrDefP) ? dUsrDefP : SaturationP(dUsrDefT)); return 1;
#if WithExtra_totHs
    case xidSMtotHAtmP    : DCB.D = m_bUsableMass || !DCB.ForView() ? m_dMassScale * totHs(som_ALL, Temp(), Std_P) : 0.0; return 1;
    case xidSMtotHSatP    : DCB.D = m_bUsableMass || !DCB.ForView() ? m_dMassScale * totHs(som_ALL, Temp(), SaturationP(Temp())) : 0.0; return 1;
#endif
    case xidSMtotH        : DCB.D = m_bUsableMass || !DCB.ForView() ? m_dMassScale * totHs() : 0.0; return 1;
    case xidSMtotHz       : DCB.D = m_bUsableMass || !DCB.ForView() ? m_dMassScale * totHz() : 0.0; return 1;
    case xidSMtotHf       : DCB.D = m_bUsableMass || !DCB.ForView() ? m_dMassScale * totHf() : 0.0; return 1;
    case xidSMtotHf0      : DCB.D = m_bUsableMass || !DCB.ForView() ? m_dMassScale * totHf(som_ALL, ZeroCinK, SaturationP(ZeroCinK)) : 0.0; return 1;
    case xidSMtotSf       : DCB.D = m_bUsableMass || !DCB.ForView() ? m_dMassScale * totSf() : 0.0; return 1;
    case xidSMtotSf0      : DCB.D = m_bUsableMass || !DCB.ForView() ? m_dMassScale * totSf(som_ALL, ZeroCinK, SaturationP(ZeroCinK)) : 0.0; return 1;
    case xidSMtotHUt      : DCB.D = m_bUsableMass || !DCB.ForView() ? m_dMassScale * totHs(som_ALL, dUsrDefT, Valid(dUsrDefP) ? dUsrDefP : SaturationP(dUsrDefT)) : 0.0; return 1;
    case xidSMtotHzUt     : DCB.D = m_bUsableMass || !DCB.ForView() ? m_dMassScale * totHz(som_ALL, dUsrDefT, Valid(dUsrDefP) ? dUsrDefP : SaturationP(dUsrDefT)) : 0.0; return 1;
    case xidSMtotHfUt     : DCB.D = m_bUsableMass || !DCB.ForView() ? m_dMassScale * totHf(som_ALL, dUsrDefT, Valid(dUsrDefP) ? dUsrDefP : SaturationP(dUsrDefT)) : 0.0; return 1;

    case xidVVisc         : DCB.D = DynamicViscosity(som_Gas);           return 1;
    case xidLVisc         : DCB.D = DynamicViscosity(som_SL);            return 1;
    case xidDVisc         : DCB.D = DynamicViscosity(som_ALL);           return 1;
    case xidVKVisc        : DCB.D = KinematicViscosity(som_Gas);         return 1;
    case xidLKVisc        : DCB.D = KinematicViscosity(som_SL);          return 1;
    case xidVThermCond    : DCB.D = ThermalConductivity(som_Gas);        return 1;
    case xidLThermCond    : DCB.D = ThermalConductivity(som_SL);         return 1;
    case xidVPrandtl      : DCB.D = PrandtlNo(som_Gas);                  return 1;
    case xidLPrandtl      : DCB.D = PrandtlNo(som_SL);                   return 1;
    case xidLSurfT        : DCB.D = SurfaceTension(som_Liq);             return 1;

    case xidSpHeatValueHiM : DCB.D = HeatOfCombustionHi()/GTZ(Mass());     return 1;
    case xidSpHeatValueHiMl: DCB.D = HeatOfCombustionHi()/GTZ(Moles());    return 1;
    case xidSpHeatValueHiV : DCB.D = HeatOfCombustionHi()/GTZ(Volume());   return 1;
    case xidSpHeatValueHiNV: DCB.D = HeatOfCombustionHi()/GTZ(NVolume());  return 1;
    case xidSpHeatValueLoM : DCB.D = HeatOfCombustionLo()/GTZ(Mass());     return 1;
    case xidSpHeatValueLoMl: DCB.D = HeatOfCombustionLo()/GTZ(Moles());    return 1;
    case xidSpHeatValueLoV : DCB.D = HeatOfCombustionLo()/GTZ(Volume());   return 1;
    case xidSpHeatValueLoNV: DCB.D = HeatOfCombustionLo()/GTZ(NVolume());  return 1;
    case xidSpHeatValueHiM25 : DCB.D = HeatOfCombustionHi(som_ALL, Std_T, Std_P)/GTZ(Mass());     return 1;
    case xidSpHeatValueHiMl25: DCB.D = HeatOfCombustionHi(som_ALL, Std_T, Std_P)/GTZ(Moles());    return 1;
    case xidSpHeatValueHiV25 : DCB.D = HeatOfCombustionHi(som_ALL, Std_T, Std_P)/GTZ(Volume());   return 1;
    case xidSpHeatValueHiNV25: DCB.D = HeatOfCombustionHi(som_ALL, Std_T, Std_P)/GTZ(NVolume());  return 1;
    case xidSpHeatValueLoM25 : DCB.D = HeatOfCombustionLo(som_ALL, Std_T, Std_P)/GTZ(Mass());     return 1;
    case xidSpHeatValueLoMl25: DCB.D = HeatOfCombustionLo(som_ALL, Std_T, Std_P)/GTZ(Moles());    return 1;
    case xidSpHeatValueLoV25 : DCB.D = HeatOfCombustionLo(som_ALL, Std_T, Std_P)/GTZ(Volume());   return 1;
    case xidSpHeatValueLoNV25: DCB.D = HeatOfCombustionLo(som_ALL, Std_T, Std_P)/GTZ(NVolume());  return 1;

    case xidSpEqnOfState    : DCB.L = SpModel::sm_iEqnOfState;                  return 1;
    case xidSpMixtureRule   : DCB.L = SpModel::sm_iGasMixingRule;               return 1;
    case xidSpMixtureAlpha  : if (DCB.rD) m_dGasMixAlpha=Range(0.1, *DCB.rD, 10.0); DCB.D = m_dGasMixAlpha;   return 1;
    case xidSpMixtureTc     : DCB.D = TCritical(som_Gas);                       return 1;
    case xidSpMixturePc     : DCB.D = PCritical(som_Gas);                       return 1;
    case xidSpMixtureVc     : DCB.D = VCritical(som_Gas);                       return 1;
    case xidSpMixtureAc     : DCB.D = AccentricFactor(som_Gas);                 return 1;
    case xidSpdHIdeal       : DCB.D = msdHIdeal(som_Gas, Std_T, Temp());        return 1;
    case xidSpdSIdeal       : DCB.D = msdSIdeal(som_Gas, Std_T, Temp());        return 1;
    case xidSpdHResidual    : DCB.D = msdHResidual(som_Gas, Temp(), Press());   return 1;
    case xidSpdSResidual    : DCB.D = msdSResidual(som_Gas, Temp(), Press());   return 1;

    case xidAutoVLEComp      : 
      if (DCB.rL) 
        { 
        m_AutoVLE.m_iCmp  = *DCB.rL;
        if (AutoVLEOn())
          {
          m_AutoVLE.m_iVap = CDB[m_AutoVLE.m_iCmp].VapPhInx("XXXX", true); 
          m_AutoVLE.m_iLiq = CDB[m_AutoVLE.m_iCmp].LiqPhInx("XXXX", true); 
          }
        else
          {
          m_AutoVLE.m_iVap = -1;
          m_AutoVLE.m_iLiq = -1;
          }
        m_dwValidFlags &= ~(SMVF_EquilOK); 
        }; 
      DCB.L = m_bEquilOn ? m_AutoVLE.m_iCmp : AutoVLEDisabled;
      return 1;
    case xidAutoVLESet      : if (DCB.rB) { m_AutoVLE.m_iSet   = *DCB.rB; m_dwValidFlags &= ~(SMVF_EquilOK); }; DCB.B = m_AutoVLE.m_iSet;   return 1;
    case xidAutoVLESpecH    : if (DCB.rD) { m_AutoVLE.m_dSpecH = *DCB.rD; m_dwValidFlags &= ~(SMVF_EquilOK); }; DCB.D = m_AutoVLE.m_dSpecH;   return 1;
    case xidAutoVLERelH     : if (DCB.rD) { m_AutoVLE.m_dRelH  = *DCB.rD; m_dwValidFlags &= ~(SMVF_EquilOK); }; DCB.D = m_AutoVLE.m_dRelH;   return 1;
    case xidAutoVLEDewPt    : if (DCB.rD) { m_AutoVLE.m_dDewPt = *DCB.rD; m_dwValidFlags &= ~(SMVF_EquilOK); }; DCB.D = m_AutoVLE.m_dDewPt;   return 1;

    case xidHumiditySpecific: DCB.D = SpecificHumidity();                       return 1;
    case xidHumidityRelative: DCB.D = RelativeHumidity();                       return 1;
    case xidHumidityDewPt   : DCB.D = HumidityDewPt();                          return 1;

    case xidEquilSwitch:
      {
      if (DCB.rB)
        {
        m_iEquilAct=*DCB.rB;
        switch (m_iEquilAct)
          {
          case SQA_Xfer:
            if (SQFlagsSet(SQF_Source|SQF_TearSet))
              m_bEquilOn=false;
            break;
          case SQA_Create:
            m_bEquilOn=true;
            break;
          case SQA_Remove:
            m_bEquilOn=false;
            break;
          }
        }
      DCB.B=m_iEquilAct;
      return 1;
      }
    case xidSpUseFlgs:
      {
      if (DCB.rL)
        m_Ovr.bUse=(word)(*DCB.rL);
      DCB.L=m_Ovr.bUse;
      return 1;
      }
    case xidOvrAnyOn:
      DCB.B=(m_Ovr.bUse!=0);
      return 1;
    case xidOvrVDens:
    case xidOvrLDens:
    case xidOvrSDens:
    case xidOvrVCp  :
    case xidOvrLCp  :
    case xidOvrSCp  :
    case xidOvrVDynVisc:
    case xidOvrLDynVisc:
    case xidOvrVThermCd:
    case xidOvrLThermCd:
    case xidOvrSurfT:
    case xidOvrBPEle:
      {
      static SPOType Masks[] =
        {
        SPO_VDens, SPO_LDens, SPO_SDens,
        SPO_VCp, SPO_LCp, SPO_SCp,
        SPO_VDynVisc, SPO_LDynVisc,
        SPO_VThermCnd, SPO_LThermCnd,
        SPO_SurfT,
        SPO_BPEle
        };
      SPOType Mask=Masks[DCB.lHandle-xidOvrVDens];
      int Act=(m_Ovr.bAdd & Mask) ? SQA_Create : (m_Ovr.bRem & Mask) ? SQA_Remove : SQA_Xfer;
      if (DCB.rB)
        {
        Act=*DCB.rB;
        switch (Act)
          {
          case SQA_Xfer:
            m_Ovr.bAdd &= ~Mask;
            m_Ovr.bRem &= ~Mask;
            m_Ovr.bUse &= ~Mask;
            break;
          case SQA_Create:
            m_Ovr.bAdd |= Mask;
            m_Ovr.bRem &= ~Mask;
            break;
          case SQA_Remove:
            m_Ovr.bAdd &= ~Mask;
            m_Ovr.bRem |= Mask;
            break;
          }
        }
      m_Ovr.bUse = m_Ovr.bAdd | (m_Ovr.bUse & ~m_Ovr.bRem);
      DCB.B=Act;
      return 1;
      }

    default:
      {
      if (DCB.lHandle>=xidSpUsrProps && DCB.lHandle<xidSpUsrPropsLast)
        {
        long i=DCB.lHandle-xidSpUsrProps;

        CPropertyMap &Map=PropertyMap();
        MPropertyInfo *I=Map[i];
        if (I && (I->m_dwFlags&MP_NoAccess)==0)
          {
          switch (I->m_iType)
            {
            case ePT_Bool:
              {
              if (I->Settable() && DCB.rB)
                {
                m_WorkPropValue=*DCB.rB;
                PutPropertyValue(i, m_WorkPropValue);
                }
              GetPropertyValue(i, som_ALL, Temp(), Press(), m_WorkPropValue);
              DCB.B=long(m_WorkPropValue)!=0;
              return 1;
              }
            case ePT_Long:
              {
              if (I->Settable() && DCB.rL)
                {
                m_WorkPropValue=*DCB.rL;
                PutPropertyValue(i, m_WorkPropValue);
                }
              GetPropertyValue(i, som_ALL, Temp(), Press(), m_WorkPropValue);
              DCB.L=m_WorkPropValue;
              return 1;
              }
            //case ePT_ULong:
            //  {
            //  if (I->Settable() && DCB.rDW)
            //    {
            //    m_WorkPropValue=*DCB.rDW;
            //    PutPropertyValue(i, m_WorkPropValue);
            //    }
            //  GetPropertyValue(i, som_ALL, Temp(), Press(), m_WorkPropValue);
            //  DCB.DW=m_WorkPropValue;
            //  return 1;
            //  }
            case ePT_Double:
              {
              if (I->Settable() && DCB.rD)
                {
                m_WorkPropValue=*DCB.rD;
                PutPropertyValue(i, m_WorkPropValue);
                }
              GetPropertyValue(i, som_ALL, Temp(), Press(), m_WorkPropValue);
              DCB.D=m_WorkPropValue;
              return 1;
              }
            case ePT_String:
              {
              if (I->Settable() && DCB.rpC)
                {
                m_WorkPropValue=(LPCTSTR)*DCB.rpC;
                PutPropertyValue(i, m_WorkPropValue);
                }
              GetPropertyValue(i, som_ALL, Temp(), Press(), m_WorkPropValue);
              DCB.pC=m_WorkPropValue;
              return 1;
              }
            }
          }
        }
      else if (DCB.lHandle>=xidQualSwitch && DCB.lHandle<xidQualSwitch+MaxSpQualities)
        {
        int q=DCB.lHandle-xidQualSwitch;
        if (DCB.rpC)
          {
          if (strcmp(DCB.rpC, "-")==0)
            {
            SetQualityPresent(q, false);
            Quals[q].m_iAct=SQA_Xfer;
            }
          else if (strcmp(DCB.rpC, "Present")==0)
            {
            SetQualityPresent(q, true);
            if (SQFlagsSet(SQF_Source|SQF_TearSet))
              Quals[q].m_iAct=SQA_Create;
            else
              Quals[q].m_iAct=SQA_Xfer;
            }
          else if (strncmp(DCB.rpC, "Create", 6)==0)
            {
            SetQualityPresent(q, true);
            Quals[q].m_iAct=SQFlagsSet(SQF_CreateDsp) ? SQA_Create : SQA_Xfer;
            }
          else if (strncmp(DCB.rpC, "Remove", 6)==0)
            {
            SetQualityPresent(q, false);
            Quals[q].m_iAct=SQFlagsSet(SQF_RemoveDsp) ? SQA_Remove : SQA_Xfer;
            }
          }
        switch (Quals[q].m_iAct)
          {
          case SQA_Create : DCB.pC=Quals[q].Exists() ? "Create {*}" : "Create {-}"; break;
          case SQA_Remove : DCB.pC=Quals[q].Exists() ? "Remove {*}" : "Remove {-}"; break;
          case SQA_Xfer   : DCB.pC=Quals[q].Exists() ? "Present" : "-"; break;
          }
        return 1;
        }
      else if (DCB.lHandle>=xidQualSwitch1 && DCB.lHandle<xidQualSwitch1+MaxSpQualities)
        {
        int q=DCB.lHandle-xidQualSwitch1;
        if (DCB.rB)
          SetQualityPresent(q, (*DCB.rB!=0));
        DCB.B=(QualityPtr(q, false)!=NULL);
        return 1;
        }
      else if (DCB.lHandle>=xidQualSwitch2 && DCB.lHandle<xidQualSwitch2+MaxSpQualities)
        {
        int q=DCB.lHandle-xidQualSwitch2;
        if (DCB.rB)
          {
          Quals[q].m_iAct=*DCB.rB;
          switch (Quals[q].m_iAct)
            {
            case SQA_Xfer:
              if (SQFlagsSet(SQF_Source|SQF_TearSet))
                SetQualityPresent(q, false);
              break;
            case SQA_Create:
              SetQualityPresent(q, true);
              break;
            case SQA_Remove:
              SetQualityPresent(q, false);
              break;
            }
          }
        DCB.B=Quals[q].m_iAct;
        return 1;
        }
      else if (DCB.lHandle>=xidRhoSoln1 && DCB.lHandle<=xidRhoSolnLast)
        {
        const int iSolvent=SDB.m_DensCorrSps[DCB.lHandle-xidRhoSoln1];
        //CSpecie & Solvent=SDB[iSolvent];
        ASSERT(SDB[iSolvent].DensityDirect() && SDB[iSolvent].HasDensCorrs());
        DCB.D=SDB[iSolvent].Density(m_iFidelity, Temp(), Press(), &m_Ovr, SVData());
        return 1;
        }
      else if (DCB.lHandle>=xidSpMdlVarsAvail && DCB.lHandle<xidSpMdlVarsAvailEnd)
        {
        int i=DCB.lHandle-xidSpMdlVarsAvail;
        long Msk=1<<i;
        if (DCB.rB)
          {
          if (!DCB.ForFiling() || PrjFileVerNo()>80)
            {
            sm_VarsAvail = (*DCB.rB&1) ? (sm_VarsAvail |Msk ): (sm_VarsAvail &~Msk);
            m_VarsAvail = (*DCB.rB&2) ? (m_VarsAvail |Msk ): (m_VarsAvail &~Msk);
            }
          }
        DCB.B =  (sm_VarsAvail&Msk)!=0 ? 1 : 0;
        DCB.B |= (m_VarsAvail&Msk)!=0 ? 2 : 0;
        return 1;
        }
      else if (DCB.lHandle>=xidSpMdlSpVarsAvail && DCB.lHandle<xidSpMdlSpVarsAvailEnd)
        {
        int i=DCB.lHandle-xidSpMdlSpVarsAvail;
        long Msk=1<<i;
        if (DCB.rB)
          {
          if (!DCB.ForFiling() || PrjFileVerNo()>80)
            {
            sm_SpVarsAvail = (*DCB.rB&1) ? (sm_SpVarsAvail |Msk ): (sm_SpVarsAvail &~Msk);
            m_SpVarsAvail = (*DCB.rB&2) ? (m_SpVarsAvail |Msk ): (m_SpVarsAvail &~Msk);
            }
          }
        DCB.B = (sm_SpVarsAvail&Msk)!=0 ? 1 : 0;
        DCB.B |= (m_SpVarsAvail&Msk)!=0 ? 2 : 0;
        return 1;
        }
      else
        {
        // Is it in the qualities
        for (TagObjClass* pQ=TagObjClass::FindGroupId(SpQuality::GroupName); pQ; pQ=pQ->NextClassInGrp())
          {
          if (Quals[pQ->GroupIdNo()].Exists())
            {
            if (Quals[pQ->GroupIdNo()].m_pQ->DataXchg(DCB))
              return 1;
            if (Quals[pQ->GroupIdNo()].m_pQP)
              if (Quals[pQ->GroupIdNo()].m_pQP->DataXchg(DCB))
                return 1;
            }
          }
        }
      }
    }
  return 0;
  }

// --------------------------------------------------------------------------

flag SpModel::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=ValidateData_Species(VDB);

  #if dbgSpModel
  const bool doDbg=0||dbgValidate() || dbgValidateBrk() && DoDbgBrk();
  double OldT=Temp();
  double OldP=Press();
  if (doDbg)
    {
    dbgpln(">>> ######################################################################################");
    dbgpln("SpModel::ValidateData %s", FullObjTag());
    dbgpln("Mass                  %14.4f", Mass());
    dbgpln("FileTemp              %s", DbgFltString(m_FileTemp));
    dbgpln("FilePress             %s", DbgFltString(m_FilePres));
    }
  #endif

  if (Finite(m_FileTemp) && Finite(m_FilePres))
    SetTempPress(m_FileTemp, m_FilePres);
  else if (Finite(m_FileTemp))
    SetTemp(m_FileTemp);
  else if (Finite(m_FilePres))
    SetPress(m_FilePres);
  m_FileTemp=dNAN;
  m_FilePres=dNAN;

  #if dbgSpModel
  if (doDbg)
    {
    dbgpln("Temp                  %14.4f > %14.4f", OldT, Temp());
    dbgpln("Press                 %14.4f > %14.4f", OldP, Press());
    dbgpln("<<< ######################################################################################");
    }
  #endif

  EquilibrateAll();

  for (TagObjClass* pQ=TagObjClass::FindGroupId(SpQuality::GroupName); pQ; pQ=pQ->NextClassInGrp())
    if (Quals[pQ->GroupIdNo()].Exists())
      OK = OK && Quals[pQ->GroupIdNo()].m_pQ->ValidateData(VDB);

  return OK;
  }

// --------------------------------------------------------------------------

long SpModel::CheckDataRanges()
  {
  long NBadRanges=0;
  if (HeatSkipMethod())
    {
    ClrCI(5);
    }
  else
    {
    const int WithSpcs=1;
    double T=Temp();
    double Mt=GTZ(Mass());
    Strng Spcs;
    flag AsWarn=false;
    if (Mt>1.0e-9)
      {
      Strng S1;
      int NBad=0;
      for (int s=0; s<SDB.Count(); s++)
        {
        if (m_M[s]/Mt > dTestToleranceMassFrac)
          {
          CSpecie & D=SDB[s];
          flag IsBad=false;
          if (SpModel::m_RangeExtErrorsOn && (T<D.TestLoT(m_iFidelity) || T>D.TestHiT(m_iFidelity)))
            {
            AsWarn=true;
            IsBad=true;
            }
          else if (SpModel::m_RangeErrorsOn && (T<D.LoT(m_iFidelity) || T>D.HiT(m_iFidelity)))
            IsBad=true;

          if (IsBad)
            {
            NBadRanges++;
            if (WithSpcs)
              {
              const int MaxBad=5;
              if (NBad<MaxBad)
                {
                if (Spcs.Length()>0)
                  Spcs+=",";
                if (0)
                  {
                  S1.Set("%s[%5.1f%%]", D.SymOrTag(), 100*m_M[s]/Mt);
                  Spcs+=S1();
                  }
                else
                  Spcs+=D.SymOrTag();
                }
              else if (NBad==MaxBad)
                Spcs+=" ...";
              NBad++;
              }
            }
          }
        }
      }
    if (NBadRanges)
      {
      if (WithSpcs)
        SetCI(5, "%s\tTemperature Range [%7.2f C] %s", AsWarn?"W":"N", K_2_C(T), Spcs());
      else
        SetCI(5, "%s\tTemperature [%7.2f C] out of Valid Data Range", AsWarn?"W":"N", K_2_C(T));
      }
    else
      ClrCI(5);
    }
  return NBadRanges;
  };

// --------------------------------------------------------------------------

const byte SpOD_ID    = 0x21;
const byte SpOD_More  = 0x22;
const byte SpOD_Qual  = 0x23;
const byte SpOD_End   = 0x24;
const byte SpOD_MoreA = 0x25;
const byte SpOD_ID1   = 0x26;
const byte SpOD_ID2   = 0x27;
const byte SpOD_MoreB = 0x28;

flag SpModel::GetOtherData(FilingControlBlock &FCB)
  {
  DWORD nBytes, Len=0xffffffff;
  DWORD StartPos=FCB.SetFilePointer(0, FILE_CURRENT);

  FCB.WriteFile(&SpOD_ID2, sizeof(SpOD_ID2), &nBytes);
  DWORD LenPos=FCB.SetFilePointer(0, FILE_CURRENT);
  FCB.WriteFile(&Len, sizeof(Len), &nBytes);
  FCB.WriteFile(&m_msHf, sizeof(m_msHf), &nBytes);

  short nMasses=0;
  for (int i=0; i<SVValueCount(); i++)
    if (m_M[i]> 1.0e-10)
      nMasses++;

  FCB.WriteFile(&nMasses, sizeof(nMasses), &nBytes);
  for (int i=0; i<SVValueCount(); i++)
    {
    if (m_M[i]> 1.0e-10)
      {
      int l=strlen(SDB[i].SymOrTag())+1;
      FCB.WriteFile(&i, sizeof(i), &nBytes);
      FCB.WriteFile(&l, sizeof(l), &nBytes);
      FCB.WriteFile(SDB[i].SymOrTag(), l, &nBytes);
      FCB.WriteFile(m_M.VPtr[i], sizeof(m_M[i]), &nBytes);
      }
    }
  for (int q=0; q<nQuals; q++)
    {
    if (Quals[q].Exists())
      {
      FCB.WriteFile(&SpOD_Qual, sizeof(SpOD_Qual), &nBytes);
      int l=strlen(Quals[q].m_pQ->ClassId())+1;
      FCB.WriteFile(&l, sizeof(l), &nBytes);
      FCB.WriteFile(Quals[q].m_pQ->ClassId(), l, &nBytes);
      Quals[q].m_pQ->GetOtherData(FCB);
      }
    }

  FCB.WriteFile(&SpOD_More,               sizeof(SpOD_More),                &nBytes);

  FCB.WriteFile(&m_msHf,                  sizeof(m_msHf),                   &nBytes);
  FCB.WriteFile(&m_msHfDeriv,             sizeof(m_msHfDeriv),              &nBytes);
  FCB.WriteFile(&m_CurrentP,              sizeof(m_CurrentP),               &nBytes);
  FCB.WriteFile(&m_CurrentT,              sizeof(m_CurrentT),               &nBytes);
  FCB.WriteFile(&m_RqdTemp,               sizeof(m_RqdTemp),                &nBytes);
  FCB.WriteFile(&m_KeptTemp,              sizeof(m_KeptTemp),               &nBytes);
  FCB.WriteFile(&m_bMixed,                sizeof(m_bMixed),                 &nBytes);
  FCB.WriteFile(&m_bClosed,               sizeof(m_bClosed),                &nBytes);
  FCB.WriteFile(&m_bPressAsSet,           sizeof(m_bPressAsSet),            &nBytes);
  FCB.WriteFile(&m_bUsableMass,           sizeof(m_bUsableMass),            &nBytes);
  FCB.WriteFile(&m_bTraceMass,            sizeof(m_bTraceMass),             &nBytes);
  FCB.WriteFile(&m_bVentExpandOK,         sizeof(m_bVentExpandOK),          &nBytes);
  FCB.WriteFile(&m_bScan4EquilRqd4HEval,  sizeof(m_bScan4EquilRqd4HEval),   &nBytes);
  FCB.WriteFile(&m_bEquilRqd4HEval,       sizeof(m_bEquilRqd4HEval),        &nBytes);

  FCB.WriteFile(&m_iUsage,                sizeof(m_iUsage),                 &nBytes);
  FCB.WriteFile(&m_bBadTempOK,            sizeof(m_bBadTempOK),             &nBytes);
  FCB.WriteFile(&m_iHoldVentExpand,       sizeof(m_iHoldVentExpand),        &nBytes);
  FCB.WriteFile(&m_iHoldEquilibrate,      sizeof(m_iHoldEquilibrate),       &nBytes);
  FCB.WriteFile(&m_iEHBusy,               sizeof(m_iEHBusy),                &nBytes);
  FCB.WriteFile(&m_dMaxLevel,             sizeof(m_dMaxLevel),              &nBytes);
  FCB.WriteFile(&m_dMaxPress,             sizeof(m_dMaxPress),              &nBytes);
  FCB.WriteFile(&m_dMassScale,            sizeof(m_dMassScale),             &nBytes);
  FCB.WriteFile(&m_bKeepNormalEnabled,    sizeof(m_bKeepNormalEnabled),     &nBytes);
  FCB.WriteFile(&m_iDataSgn,              sizeof(m_iDataSgn),               &nBytes);
  FCB.WriteFile(&m_bNegDataOK,            sizeof(m_bNegDataOK),             &nBytes);

  int l=Range(1,m_sMainTemplate.GetLength()+1,1024);
  FCB.WriteFile(&l,                       sizeof(l),                        &nBytes);
  FCB.WriteFile(m_sMainTemplate(),        l,                                &nBytes);

  FCB.WriteFile(&m_iSetView,              sizeof(m_iSetView),               &nBytes);
  FCB.WriteFile(&m_iViewBasis,            sizeof(m_iViewBasis),             &nBytes);

  FCB.WriteFile(&m_dGasMixAlpha,          sizeof(m_dGasMixAlpha),           &nBytes);
  FCB.WriteFile(&m_Vw.m_bOn,              sizeof(m_Vw.m_bOn),               &nBytes);
  FCB.WriteFile(&m_Vw.m_iView,            sizeof(m_Vw.m_iView),             &nBytes);
  FCB.WriteFile(&m_Vw.m_iViewBasis,       sizeof(m_Vw.m_iViewBasis),        &nBytes);
  FCB.WriteFile(&m_pGlblView,             sizeof(m_pGlblView ),             &nBytes);
  FCB.WriteFile(&m_dPrvSatT,              sizeof(m_dPrvSatT),               &nBytes);

  FCB.WriteFile(&m_Ovr.bAdd,              sizeof(m_Ovr.bAdd),               &nBytes);
  FCB.WriteFile(&m_Ovr.bRem,              sizeof(m_Ovr.bRem),               &nBytes);
  FCB.WriteFile(&m_Ovr.bUse,              sizeof(m_Ovr.bUse),               &nBytes);
  FCB.WriteFile(&m_Ovr.fVDens,            sizeof(m_Ovr.fVDens),             &nBytes);
  FCB.WriteFile(&m_Ovr.fLDens,            sizeof(m_Ovr.fLDens),             &nBytes);
  FCB.WriteFile(&m_Ovr.fSDens,            sizeof(m_Ovr.fSDens),             &nBytes);
  FCB.WriteFile(&m_Ovr.fVCp,              sizeof(m_Ovr.fVCp),               &nBytes);
  FCB.WriteFile(&m_Ovr.fLCp,              sizeof(m_Ovr.fLCp),               &nBytes);
  FCB.WriteFile(&m_Ovr.fSCp,              sizeof(m_Ovr.fSCp),               &nBytes);
  FCB.WriteFile(&m_Ovr.fVDynVisc,         sizeof(m_Ovr.fVDynVisc),          &nBytes);
  FCB.WriteFile(&m_Ovr.fLDynVisc,         sizeof(m_Ovr.fLDynVisc),          &nBytes);
  FCB.WriteFile(&m_Ovr.fVThermCnd,        sizeof(m_Ovr.fVThermCnd),         &nBytes);
  FCB.WriteFile(&m_Ovr.fLThermCnd,        sizeof(m_Ovr.fLThermCnd),         &nBytes);
  FCB.WriteFile(&m_Ovr.fSurfT,            sizeof(m_Ovr.fSurfT),             &nBytes);
  FCB.WriteFile(&m_Ovr.fBPEle,            sizeof(m_Ovr.fBPEle),             &nBytes);

  FCB.WriteFile(&SpOD_MoreB,              sizeof(SpOD_MoreB),               &nBytes);

  short nMassInUse=0;
  for (int i=0; i<m_MassInUse.GetSize(); i++)
    if (m_MassInUse[i])
      nMassInUse++;

  FCB.WriteFile(&m_bMassInUseOK,          sizeof(m_bMassInUseOK),           &nBytes);
  FCB.WriteFile(&nMassInUse,              sizeof(nMassInUse),               &nBytes);
  for (int i=0; i<m_MassInUse.GetSize(); i++)
    {
    if (m_MassInUse[i])
      {
      byte l=strlen(SDB[i].SymOrTag())+1;
      FCB.WriteFile(&i, sizeof(i),                            &nBytes);
      FCB.WriteFile(&l, sizeof(l),                            &nBytes);
      FCB.WriteFile(SDB[i].SymOrTag(), l,                     &nBytes);
      byte b=m_MassInUse[i]?1:0;
      FCB.WriteFile(&b, sizeof(b),  &nBytes);
      }
    }


  FCB.WriteFile(&m_DC.m_V_TestValue,      sizeof(m_DC.m_V_TestValue),  &nBytes);
  FCB.WriteFile(&m_DC.m_SL_TestValue,     sizeof(m_DC.m_SL_TestValue), &nBytes);

  FCB.WriteFile(&SpOD_End, sizeof(SpOD_End), &nBytes);

  DWORD EndPos=FCB.SetFilePointer(0, FILE_CURRENT);
  FCB.SetFilePointer(LenPos, FILE_BEGIN);
  Len=EndPos-StartPos;
  FCB.WriteFile(&Len, sizeof(Len), &nBytes);
  FCB.SetFilePointer(EndPos, FILE_BEGIN);

  return true;
  };

// --------------------------------------------------------------------------

void SpModel::CopyPutOtherData(SpModel & Src)
  {
  if (0)
    {
    Strng S;
    dbgpln("CopyPutData %s", FullObjTag(S, TOA_All));
    dbgpln("m_CurrentP       %14.4f > %14.4f", m_CurrentP,        Src.m_CurrentP);
    dbgpln("m_CurrentT       %14.4f > %14.4f", m_CurrentT,        Src.m_CurrentT);
    dbgpln("m_RqdTemp        %14.4f > %14.4f", m_RqdTemp,         Src.m_RqdTemp);
    dbgpln("m_iEnable        %14i > %14i", m_iEnable,         Src.m_iEnable);
    dbgpln("m_bMixed         %14i > %14i", m_bMixed,          Src.m_bMixed );
    dbgpln("m_bClosed        %14i > %14i", m_bClosed,         Src.m_bClosed);
    //    dbgpln("m_bAutoExpand    %14i > %14i", m_bAutoExpand,     Src.m_bAutoExpand);
    dbgpln("m_bBadTempOK     %14i > %14i", m_bBadTempOK,      Src.m_bBadTempOK);
    dbgpln("m_dMaxLevel      %14.4f > %14.4f", m_dMaxLevel,       Src.m_dMaxLevel);
    dbgpln("m_dMaxPress      %14.4f > %14.4f", m_dMaxPress,       Src.m_dMaxPress);
    dbgpln("m_dMassScale     %14.4f > %14.4f", m_dMassScale,      Src.m_dMassScale);
    dbgpln("Vw.iView         %14i > %14i", m_Vw.m_iView,          Src.m_Vw.m_iView);
    dbgpln("Vw.iViewBasis    %14i > %14i", m_Vw.m_iViewBasis,     Src.m_Vw.m_iViewBasis);
    }

  //m_CurrentP      = Src.m_CurrentP;
  //m_CurrentT      = Src.m_CurrentT;
  //m_RqdTemp       = Src.m_RqdTemp;
  //m_iEnable       = Src.m_iEnable;
  //m_bMixed        = Src.m_bMixed ;
  //m_bClosed       = Src.m_bClosed;
  //m_bBadTempOK    = Src.m_bBadTempOK;
  //m_dMaxLevel     = Src.m_dMaxLevel;
  //m_dMaxPress     = Src.m_dMaxPress;
  //m_dMassScale    = Src.m_dMassScale;
  //m_Vw.m_iView      = Src.m_Vw.m_iView;
  //m_Vw.m_iViewBasis = Src.m_Vw.m_iViewBasis;

  m_msHf                 = Src.m_msHf;
  m_msHfDeriv            = Src.m_msHfDeriv;
  m_CurrentP             = Src.m_CurrentP;
  m_CurrentT             = Src.m_CurrentT;
  m_RqdTemp              = Src.m_RqdTemp;
  m_KeptTemp             = Src.m_KeptTemp;
  m_bMixed               = Src.m_bMixed;
  m_bClosed              = Src.m_bClosed;
  m_bPressAsSet          = Src.m_bPressAsSet;
  m_bUsableMass          = Src.m_bUsableMass;
  m_bTraceMass           = Src.m_bTraceMass;
  m_bVentExpandOK        = Src.m_bVentExpandOK;
  m_bScan4EquilRqd4HEval = Src.m_bScan4EquilRqd4HEval;
  m_bEquilRqd4HEval      = Src.m_bEquilRqd4HEval;

  m_iUsage               = Src.m_iUsage;
  m_bBadTempOK           = Src.m_bBadTempOK;
  m_iHoldVentExpand      = Src.m_iHoldVentExpand;
  m_iHoldEquilibrate     = Src.m_iHoldEquilibrate;
  m_iEHBusy              = Src.m_iEHBusy;
  m_dMaxLevel            = Src.m_dMaxLevel;
  m_dMaxPress            = Src.m_dMaxPress;
  m_dMassScale           = Src.m_dMassScale;
  m_bKeepNormalEnabled   = Src.m_bKeepNormalEnabled;
  m_iDataSgn             = Src.m_iDataSgn;
  m_bNegDataOK            = Src.m_bNegDataOK;
  m_sMainTemplate         = Src.m_sMainTemplate;
  m_iSetView              = Src.m_iSetView;
  m_iViewBasis            = Src.m_iViewBasis;

  m_dGasMixAlpha          = Src.m_dGasMixAlpha;
  m_Vw.m_bOn              = Src.m_Vw.m_bOn;
  m_Vw.m_iView            = Src.m_Vw.m_iView;
  m_Vw.m_iViewBasis       = Src.m_Vw.m_iViewBasis;
  m_pGlblView             = Src.m_pGlblView;
  m_dPrvSatT              = Src.m_dPrvSatT;

  m_Ovr.bAdd              = Src.m_Ovr.bAdd;
  m_Ovr.bRem              = Src.m_Ovr.bRem;
  m_Ovr.bUse              = Src.m_Ovr.bUse;
  m_Ovr.fVDens            = Src.m_Ovr.fVDens;
  m_Ovr.fLDens            = Src.m_Ovr.fLDens;
  m_Ovr.fSDens            = Src.m_Ovr.fSDens;
  m_Ovr.fVCp              = Src.m_Ovr.fVCp;
  m_Ovr.fLCp              = Src.m_Ovr.fLCp;
  m_Ovr.fSCp              = Src.m_Ovr.fSCp;
  m_Ovr.fVDynVisc         = Src.m_Ovr.fVDynVisc;
  m_Ovr.fLDynVisc         = Src.m_Ovr.fLDynVisc;
  m_Ovr.fVThermCnd        = Src.m_Ovr.fVThermCnd;
  m_Ovr.fLThermCnd        = Src.m_Ovr.fLThermCnd;
  m_Ovr.fSurfT            = Src.m_Ovr.fSurfT;
  m_Ovr.fBPEle            = Src.m_Ovr.fBPEle;

  m_bMassInUseOK          = Src.m_bMassInUseOK;

  m_MassInUse.SetSize(Src.m_MassInUse.GetSize());
  for (int i=0; i<m_MassInUse.GetSize(); i++)
    m_MassInUse[i] = Src.m_MassInUse[i];

  m_DC.m_V_TestValue      = Src.m_DC.m_V_TestValue;
  m_DC.m_SL_TestValue     = Src.m_DC.m_SL_TestValue;
  }

// --------------------------------------------------------------------------

flag SpModel::PutOtherData(FilingControlBlock &FCB)
  {
  DWORD StartPos=FCB.SetFilePointer(0, FILE_CURRENT);
  DWORD nBytes;
  byte What;
  FCB.ReadFile(&What, sizeof(What), &nBytes);
  DWORD Len;
  FCB.ReadFile(&Len, sizeof(Len), &nBytes);

  double totHf=0;
  bool SettotHReqd=false;

  switch (What)
    {
    case SpOD_ID:
    case SpOD_ID1:
      {
      for (byte i=0; i<SVValueCount(); i++)
        m_M.VValue[i]=0.0;

      if (What==SpOD_ID)
        {
        SettotHReqd=true;
        FCB.ReadFile(&totHf, sizeof(totHf), &nBytes);
        }
      else
        FCB.ReadFile(&m_msHf, sizeof(m_msHf), &nBytes);
      //FCB.ReadFile(&msH, sizeof(msH), &nBytes);
      short nMasses;
      FCB.ReadFile(&nMasses, sizeof(nMasses), &nBytes);
      for (i=0; i<nMasses; i++)
        {
        byte s,l;
        char Name[256];
        double m;
        FCB.ReadFile(&s, sizeof(s), &nBytes);
        FCB.ReadFile(&l, sizeof(l), &nBytes);
        FCB.ReadFile(Name, l, &nBytes);
        FCB.ReadFile(&m, sizeof(m), &nBytes);

        if (s>=0 && s<SVValueCount() && strcmp(SVImg(s).SymOrTag(), Name)==0)
          m_M.VValue[s]=m;
        else
          {
          int i=SDB.Find(Name);
          if (i>=0)
            m_M.VValue[i]=m;
          }
        }

      FCB.ReadFile(&What, sizeof(What), &nBytes);
      while (What==SpOD_Qual)
        {
        byte l;
        char QualName[256];
        FCB.ReadFile(&l, sizeof(l), &nBytes);
        FCB.ReadFile(QualName, l, &nBytes);

        SpQuality* pQ=QualityPtr(QualName, true);
        if (pQ)
          pQ->PutOtherData(FCB);

        FCB.ReadFile(&What, sizeof(What), &nBytes);
        }

      if (What==SpOD_More || What==SpOD_MoreA)
        {
        bool DoneOld=false;
        DWORD RewindPos=FCB.SetFilePointer(0, FILE_CURRENT);
        flag Temp;
        static SpModel*TempMdl=NULL;
        if (TempMdl==NULL)
          TempMdl=new SpModel(&SpModelClass, "SpModel::PutOtherData", NULL, TOA_Free);

        FCB.ReadFile(&TempMdl->m_CurrentP,      sizeof(m_CurrentP),      &nBytes);
        FCB.ReadFile(&TempMdl->m_CurrentT,      sizeof(m_CurrentT),      &nBytes);
        if (What!=SpOD_MoreA)
          {
          double xm_CurrentSpace;
          FCB.ReadFile(&xm_CurrentSpace,  sizeof(xm_CurrentSpace),  &nBytes);
          }
        FCB.ReadFile(&TempMdl->m_RqdTemp,       sizeof(m_RqdTemp),       &nBytes);
        double OldRqdPres=0;
        FCB.ReadFile(&OldRqdPres,               sizeof(OldRqdPres),       &nBytes);
        if (PrjFileVerNo()<47 || FCB.PrjFileVerNo()<47)
          {
          char iEn=0;
          FCB.ReadFile(&iEn, sizeof(iEn), &nBytes);
          TempMdl->m_iEnable=(IE_Enables)iEn;
          DoneOld=true;
          }
        else
          FCB.ReadFile(&TempMdl->m_iEnable,     sizeof(m_iEnable ),    &nBytes);
        FCB.ReadFile(&TempMdl->m_bMixed,        sizeof(m_bMixed    ),    &nBytes);
        FCB.ReadFile(&TempMdl->m_bClosed,       sizeof(m_bClosed   ),    &nBytes);
        if (What!=SpOD_MoreA)
          {
          flag m_bAutoExpand;
          FCB.ReadFile(&m_bAutoExpand,   sizeof(m_bAutoExpand),   &nBytes);
          }
        FCB.ReadFile(&Temp,          sizeof(Temp),          &nBytes);
        flag Tmp;
        FCB.ReadFile(&Tmp/*TempMdl->m_bTempKept*/,     sizeof(Tmp/*m_bTempKept*/ ),    &nBytes);
        FCB.ReadFile(&Tmp/*TempMdl->m_bPresKept*/,     sizeof(Tmp/*m_bPresKept*/ ),    &nBytes);
        FCB.ReadFile(&TempMdl->m_bBadTempOK,    sizeof(m_bBadTempOK),    &nBytes);
        FCB.ReadFile(&TempMdl->m_dMaxLevel,     sizeof(m_dMaxLevel ),    &nBytes);
        FCB.ReadFile(&TempMdl->m_dMaxPress,     sizeof(m_dMaxPress ),    &nBytes);
        FCB.ReadFile(&TempMdl->m_dMassScale,    sizeof(m_dMassScale),    &nBytes);
        FCB.ReadFile(&TempMdl->m_Vw.m_iView,       sizeof(m_Vw.m_iView     ),    &nBytes);
        FCB.ReadFile(&TempMdl->m_Vw.m_iViewBasis,  sizeof(m_Vw.m_iViewBasis),    &nBytes);

        if (1)
          {
          byte           iLFlash=0;
          byte           iVFlash=0;
          short          iFlashComp=0;
          FCB.ReadFile(&iFlashComp,    sizeof(iFlashComp),    &nBytes);
          FCB.ReadFile(&iLFlash,       sizeof(iLFlash   ),    &nBytes);
          FCB.ReadFile(&iVFlash,       sizeof(iVFlash   ),    &nBytes);
          }

        FCB.ReadFile(&What, sizeof(What), &nBytes);

        DWORD EndPos=FCB.SetFilePointer(0, FILE_CURRENT);
        if (Len==(EndPos-StartPos))
          CopyPutOtherData(*TempMdl);
        }
      else
        m_RqdTemp=dNAN;
      break;
      }
    case SpOD_ID2:
      {
      for (int i=0; i<SVValueCount(); i++)
        m_M.VValue[i]=0.0;

      FCB.ReadFile(&m_msHf, sizeof(m_msHf), &nBytes);
      short nMasses;
      FCB.ReadFile(&nMasses, sizeof(nMasses), &nBytes);
      for (int i=0; i<nMasses; i++)
        {
        int s,l;
        char Name[256];
        double m;
        FCB.ReadFile(&s, sizeof(s), &nBytes);
        FCB.ReadFile(&l, sizeof(l), &nBytes);
        FCB.ReadFile(Name, l, &nBytes);
        FCB.ReadFile(&m, sizeof(m), &nBytes);

        if (s>=0 && s<SVValueCount() && strcmp(SVImg(s).SymOrTag(), Name)==0)
          m_M.VValue[s]=m;
        else
          {
          int i=SDB.Find(Name);
          if (i>=0)
            m_M.VValue[i]=m;
          }
        }

      FCB.ReadFile(&What, sizeof(What), &nBytes);
      while (What==SpOD_Qual)
        {
        int l;
        char QualName[256];
        FCB.ReadFile(&l, sizeof(l), &nBytes);
        FCB.ReadFile(QualName, l, &nBytes);

        SpQuality* pQ=QualityPtr(QualName, true);
        if (pQ)
          pQ->PutOtherData(FCB);

        FCB.ReadFile(&What, sizeof(What), &nBytes);
        }

      if (What==SpOD_More)
        {
        static SpModel*TempMdl=NULL;
        if (TempMdl==NULL)
          TempMdl=new SpModel(&SpModelClass, "SpModel::PutOtherData1", NULL, TOA_Free);

        DWORD RewindPos=FCB.SetFilePointer(0, FILE_CURRENT);
        FCB.ReadFile(&TempMdl->m_msHf,                  sizeof(m_msHf),                 &nBytes);
        FCB.ReadFile(&TempMdl->m_msHfDeriv,             sizeof(m_msHfDeriv),            &nBytes);
        FCB.ReadFile(&TempMdl->m_CurrentP,              sizeof(m_CurrentP),             &nBytes);
        FCB.ReadFile(&TempMdl->m_CurrentT,              sizeof(m_CurrentT),             &nBytes);
        FCB.ReadFile(&TempMdl->m_RqdTemp,               sizeof(m_RqdTemp),              &nBytes);
        FCB.ReadFile(&TempMdl->m_KeptTemp,              sizeof(m_KeptTemp),             &nBytes);
        FCB.ReadFile(&TempMdl->m_bMixed,                sizeof(m_bMixed),               &nBytes);
        FCB.ReadFile(&TempMdl->m_bClosed,               sizeof(m_bClosed),              &nBytes);
        FCB.ReadFile(&TempMdl->m_bPressAsSet,           sizeof(m_bPressAsSet),          &nBytes);
        FCB.ReadFile(&TempMdl->m_bUsableMass,           sizeof(m_bUsableMass),          &nBytes);
        FCB.ReadFile(&TempMdl->m_bTraceMass,            sizeof(m_bTraceMass),           &nBytes);
        FCB.ReadFile(&TempMdl->m_bVentExpandOK,         sizeof(m_bVentExpandOK),        &nBytes);
        FCB.ReadFile(&TempMdl->m_bScan4EquilRqd4HEval,  sizeof(m_bScan4EquilRqd4HEval), &nBytes);
        FCB.ReadFile(&TempMdl->m_bEquilRqd4HEval,       sizeof(m_bEquilRqd4HEval),      &nBytes);

        FCB.ReadFile(&TempMdl->m_iUsage,                sizeof(m_iUsage),               &nBytes);
        FCB.ReadFile(&TempMdl->m_bBadTempOK,            sizeof(m_bBadTempOK),           &nBytes);
        FCB.ReadFile(&TempMdl->m_iHoldVentExpand,       sizeof(m_iHoldVentExpand),      &nBytes);
        FCB.ReadFile(&TempMdl->m_iHoldEquilibrate,      sizeof(m_iHoldEquilibrate),     &nBytes);
        FCB.ReadFile(&TempMdl->m_iEHBusy,               sizeof(m_iEHBusy),              &nBytes);
        FCB.ReadFile(&TempMdl->m_dMaxLevel,             sizeof(m_dMaxLevel),            &nBytes);
        FCB.ReadFile(&TempMdl->m_dMaxPress,             sizeof(m_dMaxPress),            &nBytes);
        FCB.ReadFile(&TempMdl->m_dMassScale,            sizeof(m_dMassScale),           &nBytes);
        FCB.ReadFile(&TempMdl->m_bKeepNormalEnabled,    sizeof(m_bKeepNormalEnabled),   &nBytes);
        FCB.ReadFile(&TempMdl->m_iDataSgn,              sizeof(m_iDataSgn),             &nBytes);
        FCB.ReadFile(&TempMdl->m_bNegDataOK,            sizeof(m_bNegDataOK),           &nBytes);

        int l=0;
        char MainTemplateBuff[1024];
        FCB.ReadFile(&l,                                sizeof(l),                        &nBytes);
        FCB.ReadFile(MainTemplateBuff,                  Min(l, (int)sizeof(MainTemplateBuff)), &nBytes);
        m_sMainTemplate=MainTemplateBuff;

        FCB.ReadFile(&TempMdl->m_iSetView,              sizeof(m_iSetView),             &nBytes);
        FCB.ReadFile(&TempMdl->m_iViewBasis,            sizeof(m_iViewBasis),           &nBytes);

        FCB.ReadFile(&TempMdl->m_dGasMixAlpha,          sizeof(m_dGasMixAlpha),         &nBytes);
        FCB.ReadFile(&TempMdl->m_Vw.m_bOn,              sizeof(m_Vw.m_bOn),             &nBytes);
        FCB.ReadFile(&TempMdl->m_Vw.m_iView,            sizeof(m_Vw.m_iView),           &nBytes);
        FCB.ReadFile(&TempMdl->m_Vw.m_iViewBasis,       sizeof(m_Vw.m_iViewBasis),      &nBytes);
        FCB.ReadFile(&TempMdl->m_pGlblView,             sizeof(m_pGlblView ),           &nBytes);
        FCB.ReadFile(&TempMdl->m_dPrvSatT,              sizeof(m_dPrvSatT),             &nBytes);

        FCB.ReadFile(&TempMdl->m_Ovr.bAdd,              sizeof(m_Ovr.bAdd),             &nBytes);
        FCB.ReadFile(&TempMdl->m_Ovr.bRem,              sizeof(m_Ovr.bRem),             &nBytes);
        FCB.ReadFile(&TempMdl->m_Ovr.bUse,              sizeof(m_Ovr.bUse),             &nBytes);
        FCB.ReadFile(&TempMdl->m_Ovr.fVDens,            sizeof(m_Ovr.fVDens),           &nBytes);
        FCB.ReadFile(&TempMdl->m_Ovr.fLDens,            sizeof(m_Ovr.fLDens),           &nBytes);
        FCB.ReadFile(&TempMdl->m_Ovr.fSDens,            sizeof(m_Ovr.fSDens),           &nBytes);
        FCB.ReadFile(&TempMdl->m_Ovr.fVCp,              sizeof(m_Ovr.fVCp),             &nBytes);
        FCB.ReadFile(&TempMdl->m_Ovr.fLCp,              sizeof(m_Ovr.fLCp),             &nBytes);
        FCB.ReadFile(&TempMdl->m_Ovr.fSCp,              sizeof(m_Ovr.fSCp),             &nBytes);
        FCB.ReadFile(&TempMdl->m_Ovr.fVDynVisc,         sizeof(m_Ovr.fVDynVisc),        &nBytes);
        FCB.ReadFile(&TempMdl->m_Ovr.fLDynVisc,         sizeof(m_Ovr.fLDynVisc),        &nBytes);
        FCB.ReadFile(&TempMdl->m_Ovr.fVThermCnd,        sizeof(m_Ovr.fVThermCnd),       &nBytes);
        FCB.ReadFile(&TempMdl->m_Ovr.fLThermCnd,        sizeof(m_Ovr.fLThermCnd),       &nBytes);
        FCB.ReadFile(&TempMdl->m_Ovr.fSurfT,            sizeof(m_Ovr.fSurfT),           &nBytes);
        FCB.ReadFile(&TempMdl->m_Ovr.fBPEle,            sizeof(m_Ovr.fBPEle),           &nBytes);

        FCB.ReadFile(&What,                             sizeof(What),                   &nBytes);
        if (What==SpOD_MoreB)
          {
          short nMassInUse=0;
          FCB.ReadFile(&TempMdl->m_bMassInUseOK,          sizeof(m_bMassInUseOK),         &nBytes);
          FCB.ReadFile(&nMassInUse,                       sizeof(nMassInUse),             &nBytes);
          if (nMassInUse>0)
            {
            TempMdl->SetMassInUse(false);
            for (int i=0; i<nMassInUse; i++)
              {
              int s,l;
              char Name[256];
              byte b;
              FCB.ReadFile(&s, sizeof(s),   &nBytes);
              FCB.ReadFile(&l, sizeof(l),   &nBytes);
              FCB.ReadFile(Name, l,         &nBytes);
              FCB.ReadFile(&b, sizeof(b),   &nBytes);
              if (s>=0 && s<m_MassInUse.GetSize() && strcmp(SVImg(s).SymOrTag(), Name)==0)
                TempMdl->m_MassInUse[i]=b!=0;
              else
                {
                int i=SDB.Find(Name);
                if (i>=0)
                  TempMdl->m_MassInUse[i]=b!=0;
                }
              }
            }
          FCB.ReadFile(&TempMdl->m_DC.m_V_TestValue,      sizeof(m_DC.m_V_TestValue),     &nBytes);
          FCB.ReadFile(&TempMdl->m_DC.m_SL_TestValue,     sizeof(m_DC.m_SL_TestValue),    &nBytes);

          FCB.ReadFile(&What, sizeof(What), &nBytes); // SpOD_End
          }

        DWORD EndPos=FCB.SetFilePointer(0, FILE_CURRENT);
        if (Len==(EndPos-StartPos) && What==SpOD_End)
          CopyPutOtherData(*TempMdl);
        }
      //else
      //  ??? m_RqdTemp=dNAN;

      break;
      }
    default:
      FCB.SetFilePointer(StartPos, FILE_BEGIN);
      return false;
    };

  ClrStatesOK();
  TESTMASSNOTNEG();

  DWORD EndPos=FCB.SetFilePointer(0, FILE_CURRENT);
  if (Len!=(EndPos-StartPos))
    {
    LogError(FullObjTag(), 0, "SpModel:PutOtherData - Bad Length");
    FCB.SetFilePointer(StartPos+Len, FILE_BEGIN);
    }

  if (SettotHReqd)
    Set_totHf(totHf);

  return true;
  };

// --------------------------------------------------------------------------

static int ModelFixup(void *pFixupData)
  {
  SpModel * pM=(SpModel*)pFixupData;
  if (pM)
    {
    }
  return false;
  };

// --------------------------------------------------------------------------
#define  WITHINTEGRATORS 01
void SpModel::SetStateAction(IE_Enables E)
  {
  m_iEnable=E;
  m_States.RemoveAll();
  switch (E)
    {
    case IE_Integrate:
      {
      for (int i=0; i < SVSpcCount()/*SDB.No()*/; i++)
        {
        double *pTst;
        if (m_DC.m_eIntTests==SPMT_Individual)
          pTst=m_M.VPtr[i];
        else if (SDB[i].IsGas())
          pTst=&m_DC.m_V_TestValue;
        else
          pTst=&m_DC.m_SL_TestValue;
        m_States.Add(m_M.VPtr[i], m_dM.VPtr[i], pTst);
        }
      for (  ;i < SVValueCount()/*SDB.No()*/; i++)
        m_States.Add(m_M.VPtr[i], m_dM.VPtr[i], m_M.VPtr[i]);
      m_States.Add(&m_msHf, &m_msHfDeriv, &m_msHf);
      }
    break;
    case IE_SaveState:
      {
      for (int i=0; i < SVValueCount()/*SDB.No()*/; i++)
        m_States.Add(m_M.VPtr[i]);
      m_States.Add(&m_msHf);
      }
    break;
    default:
      break;
    }
  };

// --------------------------------------------------------------------------

void SpModel::SetIntegrationTests(SPMTests T)
  {
  if (m_States.NInts()==SVValueCount()/*SDB.No()*/+1)
    {
    for (int i=0; i < SVSpcCount()/*SDB.No()*/; i++)
      {
      double *pTst;
      if (T==SPMT_Individual)
        pTst=m_M.VPtr[i];
      else if (SDB[i].IsGas())
        pTst=&m_DC.m_V_TestValue;
      else
        pTst=&m_DC.m_SL_TestValue;
      m_States.Int(i).SetTestValue(pTst);
      }
    for ( ;i < SVValueCount()/*SDB.No()*/; i++)
      m_States.Add(m_M.VPtr[i], m_dM.VPtr[i], m_M.VPtr[i]);
    }
  else if (m_States.NInts()>0)
    {
    ASSERT_ALWAYS(FALSE, "Bad No of Ints", __FILE__, __LINE__);
    }
  }

// --------------------------------------------------------------------------

void SpModel::ExchangeSpecies(SpModel * Other)
  {
  //LQExchangeSpecies(Other);

  for (int q=0; q<Max(nQuals, Other->nQuals); q++)
    {
    TagObjAttachment eAtt=Quals[q].m_pQ ? Quals[q].m_pQ->eAttachment : NULL;
    TagObjAttachment eAttO=Other->Quals[q].m_pQ ? Other->Quals[q].m_pQ->eAttachment : NULL;
    ASSERT(!Quals[q].m_pQ || Quals[q].m_pQ->pAttachedTo==this);
    ASSERT(!Other->Quals[q].m_pQ || Other->Quals[q].m_pQ->pAttachedTo==Other);
    if (Quals[q].m_pQ) Quals[q].m_pQ->DetachMeFromOwner();
    if (Other->Quals[q].m_pQ) Other->Quals[q].m_pQ->DetachMeFromOwner();
    Exchange(Quals[q].m_pQ, Other->Quals[q].m_pQ);
    Exchange(Quals[q].m_iAct, Other->Quals[q].m_iAct);
    if (Quals[q].m_pQ && Other->Quals[q].m_pQ)
      {
      Exchange(Quals[q].m_pQ->m_bRemoveMe, Other->Quals[q].m_pQ->m_bRemoveMe);
      Exchange(Quals[q].m_pQ->m_bHoldCopy, Other->Quals[q].m_pQ->m_bHoldCopy);
      }
    if (Quals[q].m_pQ)
      {
      Quals[q].m_pQ->pModel=this;
      Quals[q].m_pQ->AttachMeToObj(this, eAtt);
      }
    if (Other->Quals[q].m_pQ)
      {
      Other->Quals[q].m_pQ->pModel=Other;
      Other->Quals[q].m_pQ->AttachMeToObj(Other, eAttO);
      }
    }
  Exchange(nQuals, Other->nQuals);

  IE_Enables WasEnabled=m_iEnable;
  IE_Enables OWasEnabled=Other->m_iEnable;
  SetStateAction(IE_Disabled);
  Other->SetStateAction(IE_Disabled);

  ClrStatesOK();
  Other->ClrStatesOK();
  Exchange(m_msHf,                  Other->m_msHf);
  Exchange(m_msHfDeriv,             Other->m_msHfDeriv);
  Exchange(m_CurrentP,              Other->m_CurrentP);
  Exchange(m_CurrentT,              Other->m_CurrentT);
  Exchange(m_RqdTemp,               Other->m_RqdTemp);
  Exchange(m_KeptTemp,              Other->m_KeptTemp);
  for (int i=0; i < SVValueCount(); i++)
    {
#define EXCHANGE(t,a,b) {t c=a; a=b; b=c;}
    EXCHANGE(double, m_M.VValue[i], Other->m_M.VValue[i]);
    EXCHANGE(double, m_dM.VValue[i], Other->m_dM.VValue[i]);
#undef EXCHANGE
    }
  Exchange(m_bMixed,                Other->m_bMixed);
  Exchange(m_bClosed,               Other->m_bClosed);
  Exchange(m_bPressAsSet,           Other->m_bPressAsSet);
  Exchange(m_bUsableMass,           Other->m_bUsableMass);
  Exchange(m_bTraceMass,            Other->m_bTraceMass);
  Exchange(m_bVentExpandOK,         Other->m_bVentExpandOK);
  Exchange(m_bScan4EquilRqd4HEval,  Other->m_bScan4EquilRqd4HEval);
  Exchange(m_bEquilRqd4HEval,       Other->m_bEquilRqd4HEval);
  //Exchange(m_bSQEditable,           Other->m_bKeepNormalEnabled);

  Exchange(m_iUsage,                Other->m_iUsage);
  Exchange(m_bBadTempOK,            Other->m_bBadTempOK);
  Exchange(m_iHoldVentExpand,       Other->m_iHoldVentExpand);
  Exchange(m_iHoldEquilibrate,      Other->m_iHoldEquilibrate);
  Exchange(m_iEHBusy,               Other->m_iEHBusy);
  Exchange(m_dMaxLevel,             Other->m_dMaxLevel);
  Exchange(m_dMaxPress,             Other->m_dMaxPress);
  Exchange(m_dMassScale,            Other->m_dMassScale);
  Exchange(m_bKeepNormalEnabled,    Other->m_bKeepNormalEnabled);
  Exchange(m_iDataSgn,              Other->m_iDataSgn);
  Exchange(m_bNegDataOK,            Other->m_bNegDataOK);
  Exchange(m_sMainTemplate,         Other->m_sMainTemplate);
  Exchange(m_iSetView,              Other->m_iSetView);
  Exchange(m_iViewBasis,            Other->m_iViewBasis);

  Exchange(m_dGasMixAlpha,          Other->m_dGasMixAlpha);
  Exchange(m_Vw,                    Other->m_Vw);
  Exchange(m_pGlblView ,            Other->m_pGlblView );
  Exchange(m_dPrvSatT,              Other->m_dPrvSatT);

  Exchange(m_Ovr,                   Other->m_Ovr);

  Exchange(m_pVLE,                  Other->m_pVLE);

  if (m_MassInUse.GetSize()>0 && Other->m_MassInUse.GetSize()>0)
    {
    for (int i=0; i<m_MassInUse.GetSize(); i++)
      Exchange(m_MassInUse[i], Other->m_MassInUse[i]);
    }
  else if (m_MassInUse.GetSize()>0)
    {
    Other->m_MassInUse.SetSize(m_MassInUse.GetSize());
    for (int i=0; i<m_MassInUse.GetSize(); i++)
      Other->m_MassInUse[i]=m_MassInUse[i];
    m_MassInUse.SetSize(0);
    }
  else if (Other->m_MassInUse.GetSize()>0)
    {
    m_MassInUse.SetSize(Other->m_MassInUse.GetSize());
    for (int i=0; i<Other->m_MassInUse.GetSize(); i++)
      m_MassInUse[i]=Other->m_MassInUse[i];
    Other->m_MassInUse.SetSize(0);
    }
  Exchange(m_bMassInUseOK,          Other->m_bMassInUseOK);

  Exchange(m_DC.m_V_TestValue,      Other->m_DC.m_V_TestValue);
  Exchange(m_DC.m_SL_TestValue,     Other->m_DC.m_SL_TestValue);

  SetStateAction(OWasEnabled);
  Other->SetStateAction(WasEnabled);

  // Taken Out for QAL : I Believe this is correct for all models (CNM)
  //SetTemp(Other->Temp());
  }

// --------------------------------------------------------------------------

void SpModel::ZeroMass()
  {
  m_RqdTemp=dNAN;//Valid(m_CurrentT) ? m_CurrentT : m_PreviousT;
  m_KeptTemp=dNAN;
  m_msHf=0.0;
  m_DC.Clear();
  for (int i=0; i < SVValueCount()/*SDB.No()*/; i++)
    m_M.VValue[i]=0.0;

  ClrStatesOK();

  m_Ovr.bUse=0;

  m_bEquilOn=false;

  for (int q=0; q<nQuals; q++)
    {
    if (Quals[q].m_iAct!=SQA_Create && Quals[q].m_pQ)
      {
      Quals[q].m_pQ->ZeroMass(); //Destroy();
      SetQualityPresent(q, false);
      }
    }

  DoTempChecks(m_PreviousT);
  ClrStatesOK();
  };

// --------------------------------------------------------------------------

void SpModel::ZeroDeriv()
  {
  m_DC.m_V_TestValue=0.0;
  m_DC.m_SL_TestValue=0.0;
  m_DC.m_fStarted=false;
  m_DC.m_fMassOK=false;
  m_msHfDeriv=0.0;

#if dbgSpModel
  if (dbgAddDeriv() || dbgAddDerivDbgBrk() && DoDbgBrk())
    dbgpln("ZeroDeriv    %s", FullObjTag());
#endif

  for (int i=0; i < SVSpcCount()/*SDB.No()*/; i++)
    {
    m_dM.VValue[i]=0.;
    if (SDB[i].IsGas())
      m_DC.m_V_TestValue+=m_M[i];
    else
      m_DC.m_SL_TestValue+=m_M[i];
    }
  for (  ; i < SVValueCount()/*SDB.No()*/; i++)
    m_dM.VValue[i]=0.;

  //LQZeroDeriv();

  for (int q=0; q<nQuals; q++)
    if (Quals[q].m_pQ)
      Quals[q].m_pQ->ZeroDeriv();

  if (m_DC.m_pFd)
    m_DC.m_pFd->ZeroMass();
  if (m_DC.m_pPr)
    m_DC.m_pPr->ZeroMass();
  if (m_DC.m_pCn)
    m_DC.m_pCn->ZeroMass();
  if (m_DC.m_pDM)
    m_DC.m_pDM->Zero();
  m_DC.m_dDuty=0.0;

  if (!m_DC.m_fStarted)
    m_DC.Start(this);
  };

// --------------------------------------------------------------------------

void SpModel::ScaleMass(PhMask Phase, double Mult)
  {
  KeepTemp();

  m_M.Mult(Phase, Mult);

  for (int q=0; q<nQuals; q++)
    if (Quals[q].m_pQ)
      Quals[q].m_pQ->ScaleMass(Phase, Mult);
  ClrStatesOK();
  TESTMASSNOTNEG();
  };

// --------------------------------------------------------------------------

void SpModel::ScaleMass(CIArray & SpIds, double Mult)
  {
  KeepTemp();

  m_M.Mult(SpIds, Mult);

  for (int q=0; q<nQuals; q++)
    if (Quals[q].m_pQ)
      Quals[q].m_pQ->ScaleMass(SpIds, Mult);
  ClrStatesOK();
  TESTMASSNOTNEG();
  };

// --------------------------------------------------------------------------

void SpModel::AdjustMassTo(PhMask Phase, double RqdMass)
  {
  ScaleMass(Phase, RqdMass/GTZ(Mass(Phase)));
  };

// --------------------------------------------------------------------------

void SpModel::AdjustMassTo(CIArray & SpIds, double RqdMass)
  {
  ScaleMass(SpIds, RqdMass/GTZ(Mass(SpIds)));
  };

// --------------------------------------------------------------------------

void SpModel::SetTraceMass()
  {
  double M=Mass(som_ALL);
  if (M >= 4.0*TraceMass)
    ScaleMass(som_ALL, 2.0*TraceMass/M);
  else if (M <= TraceMass)
    {
    ScaleMass(som_ALL, 0.0);
    SetTemp(m_PreviousT);
    }
  };

// --------------------------------------------------------------------------

void SpModel::SaveMass(SpVector & V)
  {
  V=m_M;
  };

// --------------------------------------------------------------------------

void SpModel::SaveMass(CSysVector & A)
  {
  A=m_M;
  };

// --------------------------------------------------------------------------

void SpModel::RestoreMass(SpVector & V)
  {
  KeepTemp();
  m_M=V;

  ClrStatesOK();
  TESTMASSNOTNEG();
  };

// --------------------------------------------------------------------------

void SpModel::RestoreMass(CSysVector & A)
  {
  KeepTemp();
  m_M=A;

  ClrStatesOK();
  TESTMASSNOTNEG();
  };

// --------------------------------------------------------------------------

void SpModel::SetMassM(SpModel * C, PhMask Phase, double Mass__, double Press)
  {
  double Mt=C->Mass(Phase);
  double Frac=Mt>1.0e-12 ? Mass__/Mt : 0.0;
  SetMassF(C, Phase, Frac, Press);
  }

// --------------------------------------------------------------------------

void SpModel::SetMassF(SpModel * C, PhMask Phase, double Frac, double Press)
  {
  TestHStatesOK();
  C->TestHStatesOK();
  MArray().Set(C->MArray(), Phase, Frac, m_LoMassLimit);

  m_msHf=msHf(som_ALL, C->Temp(), C->Press(), pMArray());
  ASSERT(Finite(m_msHf) && (m_msHf>-1.0e30) && (m_msHf < 1.0e30));
  m_RqdTemp=dNAN;
  m_KeptTemp=dNAN;     
  m_CurrentT=C->Temp();
  m_PreviousT=C->Temp();
  m_CurrentP=Max(0.001, Press);  
  ClrStatesOK();
  TESTMASSNOTNEG();
  //SetPress(Press);

  SetQualities(C);

  ////  if (fabs(RefTemp()-C->RefTemp())<0.001)
  //  if (pClass==C->pClass)
  //    {
  //    m_totH=totEnthalpy(C->Temp(), C->Press(), pMArray());
  //    m_RqdTemp=dNAN;
  //    ASSERT(Finite(m_totH) && (m_totH>-1.0e30) && (m_totH < 1.0e30));
  //    }
  //  else
  //    {
  //    // Translate Enthalpy to account for different references
  //    // NB Pressures must be correct
  //    m_RqdTemp=C->Temp();
  //    }


  EquilibrateAll(C->Temp(), C->Press());

  };

// --------------------------------------------------------------------------

void SpModel::SetMassM(SpModel * C, eSetMass How, double SQ, double LQ, double VQ, double Press)
  {
  double SF=0;
  double LF=0;
  double VF=0;

  ASSERT_ALWAYS(0, "SetMassM SQ,LQ,VQ Incomplete", __FILE__, __LINE__);
  SetMassF(C, How, SF, LF, VF, Press);
  }

// --------------------------------------------------------------------------

void SpModel::SetMassF(SpModel * C, eSetMass How, double SF, double LF, double VF, double Press)
  {
  TestHStatesOK();
  C->TestHStatesOK();

  double T=C->Temp();
  if (How==SetMass_Frac)
    {
    MArray().Set(C->MArray(), SF, LF, VF);
    }
  else // SetMass_MassFlow
    {
    double SumS=C->Mass(som_Sol);
    double SumL=C->Mass(som_Liq);
    double SumV=C->Mass(som_Gas);
    MArray().Set(C->MArray(), SF/GTZ(SumS), LF/GTZ(SumL), VF/GTZ(SumV));
    }

  m_RqdTemp=T;
  m_KeptTemp=dNAN;
  m_CurrentP=Max(0.001, Press);

  ClrStatesOK();
  TESTMASSNOTNEG();

  SetQualities(C);

  EquilibrateAll(m_RqdTemp, Press);
  }

// --------------------------------------------------------------------------

void SpModel::SetMassM(SpModel * C, CSysVector &Filter, double Mass__, double Press)
  {
  double Mt=0.0;
  for (int i=0; i < SVValueCount()/*SDB.No()*/; i++)
    Mt+=Filter[i]*C->m_M[i];
  double Frac=Mass__/GTZ(Mt);

  m_M.Set(C->m_M, som_ALL, Frac, Filter.SVData());
  //for (i=0; i < SVSpcCount(); i++)
  //  M[i]=Frac*Filter[i]*C->M[i];
  //for (i=0; i < SVAttCount(); i++)
  //  M.Att[i]=C->M.Att[i];
  //ASSERT_MASS_OK(pMArray());

  m_RqdTemp=C->Temp();
  m_KeptTemp=dNAN;
  m_CurrentP=Max(0.001, Press);
  ClrStatesOK();
  TESTMASSNOTNEG();

  SetQualities(C);

  EquilibrateAll(m_RqdTemp, m_CurrentP);
  }

// --------------------------------------------------------------------------

void SpModel::SetMassF(SpModel * C, CSysVector &Filter, double Frac, double Press)
  {
  double Ft=0.0;
  for (int i=0; i < SVSpcCount(); i++)
    Ft+=Filter[i];

  m_M.Set(C->m_M, som_ALL, Frac/Ft, Filter.SVData());
  //for (i=0; i < SVSpcCount(); i++)
  //  M[i]=Frac*Filter[i]/Ft*C->M[i];
  //for (  ; i < SVAttCount(); i++)
  //  M[i]=C->M[i];
  ASSERT_MASS_OK(pMArray());

  m_RqdTemp=C->Temp();
  m_KeptTemp=dNAN;
  m_CurrentP=Max(0.001, Press);
  ClrStatesOK();
  TESTMASSNOTNEG();

  SetQualities(C);

  EquilibrateAll(m_RqdTemp, m_CurrentP);
  };

// --------------------------------------------------------------------------

void SpModel::AddMassM(SpModel * C, PhMask Phase, double Mass__)
  {
  double Mt=C->Mass(Phase);
  double Frac=Mt>1.0e-12 ? Mass__/Mt : 0.0;
  AddMassF(C, Phase, Frac);
  }

// --------------------------------------------------------------------------

void SpModel::AddMassF(SpModel * C, PhMask Phase, double Frac)
  {
  if (DoDbgBrk() || C->DoDbgBrk())
    BreakPoint();
#define dbgAdd 0
#define dbgAdd1 0
  double Scl=(Frac>=0.0) ? 1 : -1;
  Frac=fabs(Frac);
  ASSERT(Frac>=0);
  TestHStatesOK();
  C->TestHStatesOK();
#if (dbgAdd)
  double T0=Temp();
  double T1=C->Temp();
#endif


  SpMArray Cm(C->MArray(), Phase, Frac);

  AddQualities(C, Cm);

#if (dbgAdd1)
  double Ha0=msHf(som_ALL, 273, 50, &MArray());
  double Hb0=msHf(som_ALL, 323, 50, &MArray());
  double Hc0=msHf(som_ALL, 373, 50, &MArray());
  double Ha1=msHf(som_ALL, 273, 50, &Cm);
  double Hb1=msHf(som_ALL, 323, 50, &Cm);
  double Hc1=msHf(som_ALL, 373, 50, &Cm);
  double Za0=msHz(som_ALL, 273, 50, &MArray());
  double Zb0=msHz(som_ALL, 323, 50, &MArray());
  double Zc0=msHz(som_ALL, 373, 50, &MArray());
  double Za1=msHz(som_ALL, 273, 50, &Cm);
  double Zb1=msHz(som_ALL, 323, 50, &Cm);
  double Zc1=msHz(som_ALL, 373, 50, &Cm);
#endif

  double M0;//=Mass();
  double M1;//=Cm.Mass();
  double H0=msHf(som_ALL, Temp(), Press(), NULL, &M0);
  double H1;
  if (SpModel::iEnthalpyStrategy==SPMES_BestTemp ||
    (int)(C->RefTemp()-RefTemp())) // RefTemp Diff > 1.0
    H1=msHf(som_ALL, C->Temp(), C->Press(), &Cm, &M1);
  else
    H1=C->msHf(som_ALL, C->Temp(), C->Press(), &Cm, &M1);
  m_msHf=(H0*M0+H1*Scl*M1)/GTZ(M0+M1*Scl);
  ASSERT(Finite(m_msHf) && (m_msHf>-1.0e30) && (m_msHf < 1.0e30));
  m_RqdTemp=dNAN;
  m_KeptTemp=dNAN;

  if (Scl<0)
    MArray() -= Cm;
  else
    MArray() += Cm;

  ClrStatesOK();
  TESTMASSNOTNEG();
  EquilibrateAll();


#if (dbgAdd)
  if (DoDbgBrk() || C->DoDbgBrk())
    {
    double TR=Temp();
    dbgpln("-----------------");
    dbgpln("M0,H0,T0    %14.6g  %14.6g  %14.6g %s", M0,H0,T0, ClassId());
    dbgpln("M1,H1,T1    %14.6g  %14.6g  %14.6g %s", M1,H1,T1, C->ClassId());
    dbgpln("MR,Hr,TR    %14.6g  %14.6g  %14.6g", Mass(),m_msHf,TR);

#if (dbgAdd1)

    double Ha2=msHf(som_ALL, 273, 50, &MArray());
    double Hb2=msHf(som_ALL, 323, 50, &MArray());
    double Hc2=msHf(som_ALL, 373, 50, &MArray());
    double Za2=msHz(som_ALL, 273, 50, &MArray());
    double Zb2=msHz(som_ALL, 323, 50, &MArray());
    double Zc2=msHz(som_ALL, 373, 50, &MArray());

    dbgpln("Ha0,Ha1,Ha2 %14.6g  %14.6g  %14.6g", Ha0,Ha1,Ha2);
    dbgpln("Hb0,Hb1,Hb2 %14.6g  %14.6g  %14.6g", Hb0,Hb1,Hb2);
    dbgpln("Hc0,Hc1,Hc2 %14.6g  %14.6g  %14.6g", Hc0,Hc1,Hc2);
    dbgpln("Za0,Za1,Za2 %14.6g  %14.6g  %14.6g", Za0,Za1,Za2);
    dbgpln("Zb0,Zb1,Zb2 %14.6g  %14.6g  %14.6g", Zb0,Zb1,Zb2);
    dbgpln("Zc0,Zc1,Zc2 %14.6g  %14.6g  %14.6g", Zc0,Zc1,Zc2);
#endif
    }
#endif
  };

// --------------------------------------------------------------------------

void SpModel::AddMassM(SpModel * C, eSetMass How, double SQ, double LQ, double VQ)
  {
  LogError(FullObjTag(), 0, "SpModel::AddMassF - SQ/LQ/LQ Not tested");

  double SF=0;
  double LF=0;
  double VF=0;

  AddMassF(C, How, SF, LF, VF);

  //  EquilibrateAll();
  }

// --------------------------------------------------------------------------

void SpModel::AddMassF(SpModel * C, eSetMass How, double SF, double LF, double VF)
  {
  ASSERT(SF>=0.0);
  ASSERT(LF>=0.0);
  ASSERT(VF>=0.0);
  TestHStatesOK();
  C->TestHStatesOK();

  double T=C->Temp();
  SpMArray Cm;
  if (How==SetMass_Frac)
    {
    Cm.Set(C->MArray(), SF, LF, VF);
    }
  else // SetMass_MassFlow
    {
    double SumS=C->Mass(som_Sol);
    double SumL=C->Mass(som_Liq);
    double SumV=C->Mass(som_Gas);
    Cm.Set(C->MArray(), SF/GTZ(SumS), LF/GTZ(SumL), VF/GTZ(SumV));
    }

  AddQualities(C, Cm);

  double M0;//=Mass();
  double M1;//=Cm.Mass();
  double H0=msHf(som_ALL, Temp(), Press(), NULL, &M0);
  double H1;
  if (SpModel::iEnthalpyStrategy==SPMES_BestTemp ||
    (int)(C->RefTemp()-RefTemp())) // RefTemp Diff > 1.0
    H1=msHf(som_ALL, C->Temp(), C->Press(), &Cm, &M1);
  else
    H1=C->msHf(som_ALL, C->Temp(), C->Press(), &Cm, &M1);
  m_msHf=(H0*M0+H1*M1)/GTZ(M0+M1);
  ASSERT(Finite(m_msHf) && (m_msHf>-1.0e30) && (m_msHf < 1.0e30));
  m_RqdTemp=dNAN;
  m_KeptTemp=dNAN;

  MArray() += Cm;

  ClrStatesOK();
  TESTMASSNOTNEG();

  EquilibrateAll();
  }

// --------------------------------------------------------------------------

void SpModel::AddMassM(SpModel * C, CSysVector &Filter, double Mass__)
  {
  DoBreak();

  double Mt=0.0;
  for (int i=0; i < SVSpcCount(); i++)
    Mt+=Filter[i]*C->m_M[i];
  double Frac=Mass__/GTZ(Mt);

  SpMArray Cm;
  for (i=0; i < SVSpcCount(); i++)
    Cm.SetVMass(i, C->m_M, Frac*Filter[i]*C->m_M.VMass[i]);
  for (i=0; i < SVAttCount(); i++)
    Cm.VAttr[i]=C->m_M.VAttr[i];

  AddQualities(C, Cm);

  double M0;//=Mass();
  double M1;//=Cm.Mass();
  double H0=msHf(som_ALL, Temp(), Press(), NULL, &M0);
  double H1;
  if (SpModel::iEnthalpyStrategy==SPMES_BestTemp ||
    (int)(C->RefTemp()-RefTemp())) // RefTemp Diff > 1.0
    H1=msHf(som_ALL, C->Temp(), C->Press(), &Cm, &M1);
  else
    H1=C->msHf(som_ALL, C->Temp(), C->Press(), &Cm, &M1);
  m_msHf=(H0*M0+H1*M1)/GTZ(M0+M1);
  ASSERT(Finite(m_msHf) && (m_msHf>-1.0e30) && (m_msHf < 1.0e30));

  m_RqdTemp=dNAN;
  m_KeptTemp=dNAN;

  MArray() += Cm;

  ClrStatesOK();
  TESTMASSNOTNEG();

  EquilibrateAll();
  }

// --------------------------------------------------------------------------

void SpModel::AddMassF(SpModel * C, CSysVector &Filter, double Frac)
  {
  double Scl=(Frac>=0.0) ? 1 : -1;
  Frac=fabs(Frac);
  //  ASSERT(Frac>=0.0);
  //  ASSERT(Frac>=0.0);
  TestHStatesOK();
  C->TestHStatesOK();

  SpMArray Cm;

  DoBreak();

  double Ft=0.0;
  for (int i=0; i < SVSpcCount(); i++)
    Ft+=Filter[i];
  Ft=GTZ(Ft);

  Cm.Set(C->m_M, som_ALL, Frac/Ft, Filter.SVData());
  //for (i=0; i < SDB.No(); i++)
  //  Cm[i]=Frac*Filter[i]/Ft*C->M[i];

  AddQualities(C, Cm);

  double M0;//=Mass();
  double M1;//=Cm.Mass();
  double H0=msHf(som_ALL, Temp(), Press(), NULL, &M0);
  double H1;
  if (SpModel::iEnthalpyStrategy==SPMES_BestTemp ||
    (int)(C->RefTemp()-RefTemp())) // RefTemp Diff > 1.0
    H1=msHf(som_ALL, C->Temp(), C->Press(), &Cm, &M1);
  else
    H1=C->msHf(som_ALL, C->Temp(), C->Press(), &Cm, &M1);
  m_msHf=(H0*M0+H1*Scl*M1)/GTZ(M0+M1*Scl);
  ASSERT(Finite(m_msHf) && (m_msHf>-1.0e30) && (m_msHf < 1.0e30));

  m_RqdTemp=dNAN;
  m_KeptTemp=dNAN;

  if (Scl<0)
    MArray() -= Cm;
  else
    MArray() += Cm;

  ClrStatesOK();
  TESTMASSNOTNEG();

  EquilibrateAll();
  };

// --------------------------------------------------------------------------

void SpModel::SubMassM(SpModel * C, PhMask Phase, double Mass__)
  {
  double Mt=C->Mass(Phase);
  double Frac=Mt>1.0e-12 ? Mass__/Mt : 0.0;
  SubMassF(C, Phase, Frac);
  }

// --------------------------------------------------------------------------

void SpModel::SubMassF(SpModel * C, PhMask Phase, double Frac)
  {
  ASSERT(Frac>=0.0);
  double T1=Temp();
  double P1=Press();
  C->TestHStatesOK();

  SpMArray Cm(C->MArray(), Phase, Frac);

  MArray() -= Cm;
  SetTempPress(T1,P1);

  ClrStatesOK();
  TESTMASSNOTNEG();
  };

// --------------------------------------------------------------------------

void SpModel::SubMassM(SpModel * C, eSetMass How, double SQ, double LQ, double VQ)
  {
  LogError(FullObjTag(), 0, "SpModel::SubMassF - SQ/LQ/LQ Not tested");

  double SF=0;
  double LF=0;
  double VF=0;

  SubMassF(C, How, SF, LF, VF);
  }

// --------------------------------------------------------------------------

void SpModel::SubMassF(SpModel * C, eSetMass How, double SF, double LF, double VF)
  {
  ASSERT(SF>=0.0);
  ASSERT(LF>=0.0);
  ASSERT(VF>=0.0);
  double T1=Temp();
  double P1=Press();
  C->TestHStatesOK();

  double T=C->Temp();
  SpMArray Cm;
  if (How==SetMass_Frac)
    {
    Cm.Set(C->MArray(), SF, LF, VF);
    }
  else // SetMass_MassFlow
    {
    double SumS=C->Mass(som_Sol);
    double SumL=C->Mass(som_Liq);
    double SumV=C->Mass(som_Gas);
    Cm.Set(C->MArray(), SF/GTZ(SumS), LF/GTZ(SumL), VF/GTZ(SumV));
    }

  MArray() -= Cm;
  SetTempPress(T1, P1);

  ClrStatesOK();
  TESTMASSNOTNEG();
  }

// --------------------------------------------------------------------------

void SpModel::SubMassM(SpModel * C, CSysVector &Filter, double Mass__)
  {
  LogError(FullObjTag(), 0, "SpModel::SubMassF - Filter Not tested");
  }

// --------------------------------------------------------------------------

void SpModel::SubMassF(SpModel * C, CSysVector &Filter, double Frac)
  {
  LogError(FullObjTag(), 0, "SpModel::SubMassF - Filter Not tested");
  double T1=Temp();
  };

// --------------------------------------------------------------------------

void SpModel::Copy(SpModel * C)
  {
  if (m_pClass!=C->m_pClass)
    LogError(FullObjTag(), 0, "SpModel::Copy : Different Classes ");

  m_RqdTemp=C->m_RqdTemp;
  m_KeptTemp=C->m_KeptTemp;
  m_msHf=C->m_msHf;
  ASSERT(Finite(m_msHf) && (m_msHf>-1.0e30) && (m_msHf < 1.0e30));
  //for (int i=0; i < SVValueCount(); i++)
  //  M.VValue[i]=C->M[i];
  m_M=C->m_M;
  //m_dMassScale=C->m_dMassScale;
  ASSERT_MASS_OK(pMArray());

  m_CurrentP=C->m_CurrentP;
  ClrStatesOK();
  TESTMASSNOTNEG();

  CopyQualities(C);

  EquilibrateAll();
  };

// --------------------------------------------------------------------------

void SpModel::AddDeriv(SpModel * Flw, double Sgn_)
  {
  TestHStatesOK();
  Flw->TestHStatesOK();

  if (m_pClass!=Flw->m_pClass)
    {
    //LogNote(FullObjTag(), 0, "SpModel::AddDeriv - Different SpModels not catered for yet! ");
    }

#if dbgSpModel
  double x=m_msHfDeriv;
#endif

  if (Sgn_>0.0)
    {
    m_DC.m_pFd->AddMassF(Flw, som_ALL, 1.0);
    m_DC.m_Flws.SetAtGrow(m_DC.m_nFlws++, CSpMdlDerivCtrlFlw(Flw,1));
    }
  else
    {
    m_DC.m_pPr->AddMassF(Flw, som_ALL, 1.0);
    m_DC.m_Flws.SetAtGrow(m_DC.m_nFlws++, CSpMdlDerivCtrlFlw(Flw,-1));
    }
#if dbgSpModel
  int dbg=dbgAddDeriv() || dbgAddDerivDbgBrk() && DoDbgBrk();
  if (dbg)
    dbgpln("AddDeriv    T:%7.2f P:%9.2f Qm:%14.6f msHf:%9.2f %s @ %s",
    K_2_C(Flw->Temp()), Flw->Press(), Flw->Mass(), Flw->msHf(),
    FullObjTag(), Flw->FullObjTag());
#endif
  /**
  for (int i=0; i < SDB.No(); i++)
  {
  #if dbgSpModel
  double x=dM[i];
  #endif
  dM[i]  +=  Sgn_*Flw->M[i];
  #if dbgSpModel
  if (dbg && (fabs(x)>1e-10 || fabs(dM[i])>1e-10))
  dbgpln("  dM : %14.6f > %14.6f [%14.6f] M :%14.6f %5.1f%% %s", x, dM[i], dM[i]-x, M[i], 100*dM[i]*ICGetTimeInc()/Max(1.0e-6, M[i]), SDB[i].SymOrTag());
  #endif
  }

  #if dbgSpModel
  if (dbg)
  dbgpln("  dHf: %14.6f > %14.6f [%14.6f] Hf:%14.3f %5.1f%%", x, m_DC.m_dmsHDeriv, m_DC.m_dmsHDeriv-x, m_DC.m_dmsHState, 100*m_DC.m_dmsHDeriv*ICGetTimeInc()/NZ(m_DC.m_dmsHState));
  #endif

  Ovr.AddSrc(Flw->Ovr);
  //LQAddDeriv(Flw, Sgn_);

  for (int q=0; q<Max(nQuals, Flw->nQuals); q++)
  {
  if (Flw->Quals[q].m_pQ)
  QualityPtr(q);
  if (Quals[q].m_pQ)
  Quals[q].m_pQ->AddDeriv(Flw, Flw->QualityPtr(q, false), Sgn_);
  }
  **/
  };

// --------------------------------------------------------------------------

void SpModel::AddInternalDerivs(CSysVector * pdMdt, double dHdt)
  {
  TestHStatesOK();

  if (!m_DC.m_fStarted)
    m_DC.Start(this);

#if dbgSpModel
  double x=m_msHfDeriv;
#endif

  if (pdMdt)
    {
    for (int i=0; i < SVValueCount(); i++)
      m_DC.m_pDM->VValue[i]+=(*pdMdt)[i];
    }
  m_DC.m_dDuty+=dHdt;

#if dbgSpModel
  int dbg=dbgAddDeriv() || dbgAddDerivDbgBrk() && DoDbgBrk();
  if (dbg)
    dbgpln("AddIntDeriv %s", FullObjTag());
#endif
  if (pdMdt)
    {
    for (int i=0; i < SVValueCount(); i++)
      {
#if dbgSpModel
      double x=m_dM[i];
#endif
      m_dM.VValue[i]+=(*pdMdt)[i];
#if dbgSpModel
      if (dbg && (fabs(x)>1e-10 || fabs(m_dM[i])>1e-10))
        dbgpln("  dM : %14.6f M :%14.6f %5.1f%% %s", (*pdMdt)[i], m_M[i], 100*m_dM[i]*ICGetTimeInc()/Max(1.0e-6, m_M[i]), SDB[i].SymOrTag());
#endif
      }
    }

#if dbgSpModel
  if (dbg)
    dbgpln("  dHf: %14.6f Hf:%14.3f %5.1f%%", dHdt, totHf(), 100*dHdt*ICGetTimeInc()/Max(1.0e-6, fabs(totHf())));
#endif
  }

// --------------------------------------------------------------------------

void SpModel::AddDiscrete(SpModel * Flw, double Sgn_)
  {
  TestHStatesOK();
  Flw->TestHStatesOK();

  m_Ovr.AddSrc(Flw->m_Ovr);
  //LQAddDiscrete(Flw, Sgn_);
  for (int q=0; q<Max(nQuals, Flw->nQuals); q++)
    {
    if (Flw->Quals[q].m_pQ)
      QualityPtr(q);
    if (Quals[q].m_pQ)
      Quals[q].m_pQ->AddDiscrete(Flw, Flw->QualityPtr(q, false), Sgn_);
    }

  ClrStatesOK();
  TESTMASSNOTNEG();

  EquilibrateAll();
  };

// --------------------------------------------------------------------------

void SpModel::ApplyDerivs(double dTime, flag DoDbg)
  {
  flag MGoesNeg=false;

#if dbgSpModel
  int dbgder=(dbgApplyDeriv() || dbgApplyDerivDbgBrk() && DoDbgBrk());
  bool HdDone=false;
  double msHf0=m_msHf;
#else
  int dbgder=0;
  bool HdDone=false;
#endif

  double M0=m_M.Mass();
  double T0=Temp();
  double Hf0=msHf();
  double Hf1=m_msHf+m_msHfDeriv*dTime;

  SpMArray Mn;//, dMErr;
  for (int s=0; s<SVValueCount(); s++)
    {
    Mn.VValue[s] = m_M[s] + m_dM[s]*dTime;
    int WentNeg=(Mn[s]<-1.0e-10*m_M[s]); //0.0);

    if ((dbgder || WentNeg) && (fabs(m_M[s])>1e-10 || fabs(Mn[s])>1e-10))
      {
      LPCSTR HdSep="--------------------------------------------------------------------";
      if (!HdDone)
        dbgpln("%s ApplyDeriv %s", HdSep, FullObjTag());
      HdDone=true;
      dbgpln("M: %19.11f > %19.11f [%19.11f] dm:%19.11f %-20s %s", m_M[s], Mn[s], Mn[s]-m_M[s], m_dM[s]*dTime, WentNeg?"NEG MASS OCCURRED":"", SVImg(s).FullTag());
      }

    if (WentNeg)
      Mn.VValue[s]=0.0;
    MGoesNeg=MGoesNeg||WentNeg;
    }

  for (s=0; s<SVValueCount(); s++)
    m_M.VValue[s]=Mn[s];
  ClrStatesOK();
  TESTMASSNOTNEG();

  double M1=m_M.Mass(); // 14 digits of significance
  if (M1<1e-12*M0 || M1<1e-10) // 14 digits of significance
    {
    m_M.Zero();
    Hf1 = 0;
    }

  Set_msHf(Hf1);

#if dbgSpModel
  if (dbgder)
    {
    //ASSERT_ALWAYS(FALSE, "INCOMPLETE", __FILE__, __LINE__);
    dbgpln("H: %19.6f > %19.6f [%19.6f]    %19s %20s %s", msHf0, m_msHf, m_msHf-msHf0, "","", "Enthalpy");
    dbgpln("T: %19.3f > %19.3f [%19s]    %19s %20s %s", K2C(T0), K2C(Temp()),"","" ,"", "Temperature");
    int xxx=0;
    }
#endif
  }

// --------------------------------------------------------------------------

void SpModel::ODEOperate(CODEDataBlock & ODB)
  {
  switch (ODB.m_Cmd)
    {
    case eStateAdvance:
      {
      if (m_DC.m_fStarted)
        {
        m_DC.Finalise(this, ICGetTimeInc());
        //double x=msHz();
        //dTState=Temp();
        ODB.m_pFn(ODB, m_States);

        // Do Mass Checks
        for (int s=0; s<SVSpcCount(); s++)
          m_M.VValue[s]=GEZ(m_M[s]);

        ClrStatesOK();

        if (m_DC.m_fMassOK)
          Set_msHf(m_msHf);
        else
          Set_msHf(m_DC.m_pFd->msHf());

        //LQODEOperate(ODB);
        for (int q=0; q<nQuals; q++)
          if (Quals[q].m_pQ)
            Quals[q].m_pQ->ODEOperate(ODB);

        EquilibrateAll();
        }
      else
        {
        LogStop(FullObjTag(), 0, "Incomplete Code in SpModel::ODEOperate (Indicates a Order of Eval Bug)");
        }
      break;
      }
    case eStateConverge:
      {
      ODB.m_pFn(ODB, m_States);

      //LQODEOperate(ODB);
      for (int q=0; q<nQuals; q++)
        if (Quals[q].m_pQ)
          Quals[q].m_pQ->ODEOperate(ODB);

      EquilibrateAll();
      break;
      }
    case eStateLoad:
    case eStateDiskLoad:
      {
      ODB.m_pFn(ODB, m_States);
      ClrStatesOK();
//TODO FIX
      //      Set_msHf(m_DC.m_dmsHState);
      //      SetTemp(dTState);
      break;
      }
    case eStateFixDV:
    case eStateTest:
    case eStateSave:
    case eStateDiskSave:
      {
      ODB.m_pFn(ODB, m_States);
      break;
      }
    }
  };

// --------------------------------------------------------------------------

//void SpModel::SetMinLevel(double MinLvl)
//  {
//  m_dMinLevel=MinLvl;
//  };

// --------------------------------------------------------------------------

void SpModel::SetMaxLevel(double MaxLvl)
  {
  m_dMaxLevel=MaxLvl;
  };

// --------------------------------------------------------------------------

void SpModel::SetMaxPressure(double Press)
  {
  m_dMaxPress=Press;
  };

// --------------------------------------------------------------------------

void SpModel::SetMassScale(double Scl, LPCTSTR DbgStr)
  {
  //dbgpln("====== SetMassScale %14s %s @ %s", DbgFltString(Scl), FullObjTag(), DbgStr);
  m_dMassScale=/*GTZ*/(Scl);
  };

// --------------------------------------------------------------------------

void SpModel::SetDataSign(int Sgn)
  {
  m_iDataSgn = (Sgn!=0 && m_bNegDataOK) ? (signed char)Sgn : 1;
  };

// --------------------------------------------------------------------------

inline SpShape* Shape(SpModel* This)
  {
  SpModelOwner *pOwner=(SpModelOwner *)This->pAttachedTo;
  ASSERT(dynamic_cast<SpModelOwner *>(This->pAttachedTo));
  return pOwner ? pOwner->Shape() : NULL;
  }

// --------------------------------------------------------------------------

void SpModel::EvalMStates()
  {
  if (!(m_dwValidFlags & SMVF_M_OK))
    {
    //CStopWatch SW;
    //SW.Start();
    SPMDLTIMESTUFF(sm_lEvalMEvals++);
    SPMDLTIMERLAP(sm_swEvalM);

    for (int soi=0; soi<ph_ALL;soi++)
      {
      m_PhMass[soi]=0.0;
      }

    double mt=0.0;
    for (int i=0; i < SVSpcCount(); i++)
      {
      double m     = m_M[i];
      if (Valid(m))
        {
        m_PhMass[SDB[i].m_PhInx]  += m;
        mt+=m;
        }
      }
    ASSERT(mt>=0.0);
    m_bUsableMass=mt>=UsableMass;
    m_bTraceMass=mt>=TraceMass;
    m_dwValidFlags |= SMVF_M_OK;
    ClrHStatesOK();
    //ClrVStatesOK();


    if ((m_iHoldVentExpand==0) && m_bVentExpandOK)
      {
      double CurrentSpace=Shape(this)->ActualVolume();
      if (m_bClosed && !m_bPressAsSet && (CurrentSpace>1.0e-6))
        {
        if (0)//m_bAutoExpand)
          {
          //double Vl=Volume(som_SL)/CurrentSpace;
          //if (Vl>m_dMaxLevel)
          //  {
          //  //SpModelOwner *pOwner=(SpModelOwner *)pAttachedTo;
          //  if (Shape(this))
          //    {
          //    //double V=pOwner->MCB_GetVolume();
          //    double V=Shape(this)->ApparentVolume();
          //    V*=1.5*Vl/m_dMaxLevel;
          //    //pOwner->MCB_SetVolume(V);
          //    Shape(this)->SetApparentVolume(V);
          //    LogNote(FullObjTag(), 0, "Volume Expanded");
          //    }
          //  else
          //    LogWarning(FullObjTag(), 0, "Volume NOT Expanded - No Shape");
          //  }
          //double P=Press();
          //if (P>m_dMaxPress)
          //  {
          //  //SpModelOwner *pOwner=(SpModelOwner *)pAttachedTo;
          //  if (Shape(this))//pOwner)
          //    {
          //    //double V=pOwner->MCB_GetVolume();
          //    double V=Shape(this)->ApparentVolume();
          //    V*=1.5*P/m_dMaxPress;
          //    //pOwner->MCB_SetVolume(V);
          //    Shape(this)->SetApparentVolume(V);
          //    LogNote(FullObjTag(), 0, "Volume Expanded");
          //    }
          //  else
          //    LogWarning(FullObjTag(), 0, "Volume NOT Expanded - No Shape");
          //  }
          }
        else if (0)
          {
          double Vl=Volume(som_SL)/CurrentSpace;
          if (Vl>m_dMaxLevel*1.0001)
            {
            double P=Press();
            Vl=Min(0.99, Vl);
            double LScl=m_dMaxLevel/Vl;
            double VFrac=(1.0-m_dMaxLevel);
            double VScl=VFrac/(1.0-Vl);
            double T=Temp();
            ScaleMass(som_SL, LScl);
            ScaleMass(som_Gas, VScl);
            SetTemp(T);

            double P1=Press();
            int xxx=0;
            //LogNote(FullObjTag(), 0, "Slurry Vented");
            }
          SetCI(1, (Vl>=m_dMaxLevel*0.9999));

          double P=Press();
          if (P>m_dMaxPress*1.0001)
            {
            double PScl=m_dMaxPress/P*(1.0-1e-12);
            double T=Temp();
            ScaleMass(som_Gas, PScl);
            //LogNote(FullObjTag(), 0, "Gas Vented");
            SetTemp(T);
            }
          SetCI(2, (P>m_dMaxPress*0.9999));
          }
        }
      }
//dbgpln("SpModel::EvalMStates()     %10.2f",SW.LapTime()*1e6);
    }
  };

// --------------------------------------------------------------------------

class CTempFnd : public MRootFinderBase
  {
  public:
    SpModel * pMdl;
    CSysVector * pMA;
    double P, H;

    static CToleranceBlock s_Tol;
    CTempFnd(SpModel * pMdl_, CSysVector * pMA_, double P_, double H_) :
    MRootFinderBase("TempFnd", s_Tol)
      {
      pMdl=pMdl_; pMA=pMA_; P=P_; H=H_;
      SetErrorHandling(true, &pMdl->m_fConvergeBusy);
      };
    LPCTSTR ObjTag() { return (LPCTSTR)pMdl->FullObjTag(); };
    double Function(double x)
      {
#if dbgSpModel
      double he=pMdl->msHf(som_ALL, x, P, pMA)-H;
      if (dbgHfEvals() || dbgHfEvalsDbg() && pMdl->DoDbgBrk())
        dbgpln("  FnHErr T:%12.5f P:%12.5f = %16.8f  %s", K2C(x),P,he, pMdl->FullObjTag());
      return he;
#else
      return pMdl->msHf(som_ALL, x, P, pMA)-H;
#endif
      };
  };

CToleranceBlock CTempFnd::s_Tol(TBF_BothSys, "SpModel:TempFind", 0, 1.0e-9, 100, TBF_Fixed|TBF_UseAbs|TBF_UseRel|TBF_UseMax);

// --------------------------------------------------------------------------
//static CStopWatch *ftSW=NULL;

void SpModel::FindTFromHAtP(double ThePress)
  {
  TestMStatesOK();
  SPMDLTIMERLAP(sm_swEvalT);
#ifndef _RELEASE
  int TstTP=CSpecieDataBase::gs_nDoTestEstTP=!dbgHoldTestEstTP();
  if (dbgHoldTestEstTP())
    CSpecieDataBase::gs_nDoTestEstTP=0;
#endif

  if (fDoDbgBrk)
    { int xxx=0; };
#if WITHDEBUGDUMPENTHALPY
  bool DumpOn=false;
#endif

  //double RqdTmp=Valid(m_KeptTemp) ? m_KeptTemp : m_RqdTemp;
  double RqdTmp=Valid(m_RqdTemp) ? m_RqdTemp : m_KeptTemp;
  m_KeptTemp = dNAN;
  m_RqdTemp = dNAN;
  if (!m_bTraceMass)
    {
    m_CurrentT = Valid(RqdTmp) ? RqdTmp : m_PreviousT;
    }
  else
    {
    if (!Valid(m_CurrentT))
      m_CurrentT=Std_T;

    //m_bTempKept=0;
    //m_bPresKept=0;
    if (Valid(RqdTmp))
      {
      double RqdTempMem=RqdTmp;
      m_msHf=msHf(som_ALL, RqdTmp, ThePress, pMArray());
      ASSERT(Finite(m_msHf));
//dbgpln("FindTFromHAtP                             %10.2f",ftSW->LapTime()*1e6);
      m_CurrentT=RqdTmp;
      m_dwValidFlags |= SMVF_H_OK;
      m_PreviousT=m_CurrentT;

      if (m_bScan4EquilRqd4HEval)
        Scan4EquilRqd4HEval();
      if ((m_bEquilRqd4HEval || m_bEquilOn) && !m_iEHBusy)
        {
        m_iEHBusy=EHBusy_RqdTemp;
        while (1)
          {
          EquilibrateAll(RqdTempMem, ThePress);
          if (fabs(m_CurrentT-RqdTempMem)<=0.001)
            break;
          m_RqdTemp=RqdTempMem;
          ClrHStatesOK();
          EvalHStates();
          }
        m_iEHBusy=0;
        }
      goto Finished;
      }

    double PosT=1.0e6, PosF=1.0e10;
    double NegT=0.0, NegF=-1.0e10;
    flag PosOk=0, NegOk=0, DoBrent=0;

    // Range for Safety
    double EstT=Range(10.0, m_CurrentT, 10000.0);
    double EstH=msHf(som_ALL, EstT, ThePress, pMArray());
    double Err=EstH-m_msHf;
#if dbgSpModel
    if (dbgHfEvals() || dbgHfEvalsDbg() && DoDbgBrk())
      dbgpln("T@P HErr T:%12.5f P:%12.5f = %16.8f  EstH(%16.6f) %s", K2C(EstT),ThePress,Err,EstH, FullObjTag());
#endif

//dbgpln("FindTFromHAtP                             %10.2f",ftSW->LapTime()*1e6);

    double t0=EstT;
    double h0=EstH;

    if (Err>0.0)
      {
      PosF=Err;
      PosT=EstT;
      PosOk=1;
      EstT-=1.0; // EstT too High
      }
    else
      {
      NegF=Err;
      NegT=EstT;
      NegOk=1;
      EstT+=1.0; // EstT too Low
      }

    bool dbgLT1Done=false;
    const int MaxIt=80;
    double Damp=0.0;
    for (int It=0; It<MaxIt; It++)
      {
      if (EstT>gs_CDB.MaxT+1000)
        EstT=gs_CDB.MaxT+1000;

      EstH=msHf(som_ALL, EstT, ThePress, pMArray());
      double Err=EstH-m_msHf;
#if dbgSpModel
      if (dbgHfEvals() || dbgHfEvalsDbg() && DoDbgBrk())
        dbgpln("    HErr T:%12.5f P:%12.5f = %16.8f  EstH(%16.6f) %s", K2C(EstT),ThePress,Err,EstH,FullObjTag());
#endif

      //perhaps do something else for this loop with "excesive" EstT / ThePress / EstH ???

      if (fabs(EstT-t0)<0.00001)
        goto Done;

      if (Err>0.0)
        {
        PosF=Min(PosF, Err);
        PosT=Min(PosT, EstT);
        PosOk=1;
        }
      else
        {
        NegF=Max(NegF, Err);
        NegT=Max(NegT, EstT);
        NegOk=1;
        }
      if (PosOk && NegOk)
        goto Brent;

      double t=t0+(m_msHf-h0)/NZ(EstH-h0)*(EstT-t0);
      t=Range(-1.0e60, t, 1.0e60); //kga (13/3/2001) - stop crash???
      if (t<1.0)
        {
#if dbgSpModel
        dbgLT1Done=true;
        if (!m_bBadTempOK)
          dbgpln("SpModel Temp < 1.0 ========== %s", FullObjTag());
#endif
        if (msHf(som_ALL, 1.0, ThePress, pMArray())>m_msHf)
          {
          EstT=10.0;
          m_msHf=msHf(som_ALL, EstT, ThePress, pMArray());
          ASSERT(Finite(m_msHf));
          goto Done;
          }
        else
          t=1.0;
        }
      if (It==40)
        {
        Damp=0.5;
#if WITHDEBUGDUMPENTHALPY
        DumpOn=true;
        CSpecieDataBase::DebugHDumpOn++;
#endif
        }
      if (It>=MaxIt-MaxIt/10)
        { int xxx=0; }
      t0=EstT;
      h0=EstH;
      EstT=EstT*Damp+t*(1.0-Damp);
#ifndef _RELEASE
      if (CSpecieDataBase::gs_nDoTestEstTP && (EstT<0.001 || EstT>10001.0))
        {
        static int s_TestCount=0;
        if (s_TestCount++<3)
          {
        //  ASSERT_ALWAYS(FALSE, "Temperature Range in FindTFromHAtP!", __FILE__, __LINE__);
          }
        }
#endif
      //EstT=Range(0.001, EstT, 10000.0);//kga: 23/5/2002 Range again for Safety !?!?!
      }

    // Allows large exploratory Duty without getting error messages
    if (!m_bBadTempOK && fabs(m_msHf)<1.0e10)
      LogWarning(FullObjTag(), 0, "SpModel::Temp() - Not Converged %g != %g",EstH, m_msHf);
    goto Done;

Brent:
      { // Allow Initialisation of Local Variables
      CTempFnd TF(this, pMArray(), ThePress, m_msHf);
      TF.SetTarget(0.0);
      int iErr=TF.Start(NegT, PosT, NegF, PosF);
      if (iErr==RF_OK)
        {
        if (TF.Solve_Brent()!=RF_OK)
          {
          if (!m_bBadTempOK)
            LogWarning(FullObjTag(), 0, "SpModel::Temp() - Brent Not Converged %g != %g",EstH, m_msHf);
          }
        else
          EstT=TF.Result();
        }
      else if (!m_bBadTempOK)
        LogWarning(FullObjTag(), 0, "SpModel::Temp() - Brent Not Started");
      }
Done:
#if dbgSpModel
    if (dbgLT1Done && !m_bBadTempOK)
      dbgpln("  %14.3fK   Temp was < 1.0 ========== %s", EstT, FullObjTag());
#endif

    ASSERT(Finite(EstT));
    m_CurrentT=EstT;
    }

  m_dwValidFlags |= SMVF_H_OK;
  m_PreviousT=m_CurrentT;
  DoTempChecks(m_CurrentT);

  if (m_bScan4EquilRqd4HEval)
    Scan4EquilRqd4HEval();
  if ((m_bEquilRqd4HEval || m_bEquilOn) && !m_iEHBusy)
    {
    m_iEHBusy=EHBusy_RqdH;
    EquilibrateAll();
    m_iEHBusy=0;
    }

Finished:;
#ifndef _RELEASE
  CSpecieDataBase::gs_nDoTestEstTP=TstTP;
#endif

#if WITHDEBUGDUMPENTHALPY
  if (DumpOn)
    CSpecieDataBase::DebugHDumpOn--;
#endif
//dbgpln("FindTFromHAtP Done                        %10.2f",ftSW->LapTime()*1e6);
  };

// --------------------------------------------------------------------------

CToleranceBlock s_PVTTol(TBF_DynSys, "SpModel:PVTConverge", 0, 1.0e-10, 100, /*TBF_Fixed|TBF_UseAbs|*/TBF_UseRel|TBF_UseMax);

void SpModel::EvalHStates()
  {
  if (!(m_dwValidFlags&SMVF_H_OK))
    {
//dbgpln("SpModel::EvalHStates() vvvv");
//    CStopWatch SW;
//    ftSW=&SW;
//    SW.Start();
    SPMDLTIMESTUFF(sm_lEvalHEvals++);
    SPMDLTIMERLAP(sm_swEvalH);
    m_iHoldVentExpand++;

    if (HeatSkipMethod())
      {
      m_CurrentT=Std_T;
      }
    else
      {
      static bool DoQuick=0;
      bool DoClosedStuff=(m_bClosed && !m_bPressAsSet);
      if (!DoQuick || !DoClosedStuff)
        FindTFromHAtP(m_CurrentP);
//dbgpln("SpModel::EvalHStates()     %10.2f",SW.LapTime()*1e6);
      if (DoClosedStuff)
        {
        double HReqd=m_msHf*Mass();//+totHf(som_ALL, ZeroCinK, Std_P);
        SPMDLTIMERLAP(sm_swEvalHC);
        double VolStdP = Volume(som_Gas, m_CurrentT, m_CurrentP, pMArray());
        double VolumeSL = Volume(som_SL, m_CurrentT, m_CurrentP, pMArray());
        double CurrentSpace=Shape(this)->ActualVolume();

#if dbgSpModel
        if (dbgPVTConverge() || dbgPVTConvergeDbg() && DoDbgBrk())
          {
          dbgpln("%4.4s     T:%12.5f P:%12.5f V:%12.5f Vsl:%12.5f Msl:%12.5f Mv:%12.5f Vol:%12.5f %s",
            "PVT ",
            K2C(m_CurrentT), m_CurrentP, VolStdP, VolumeSL,
            Mass(som_SL), Mass(som_Gas), CurrentSpace,
            FullObjTag());
          }
#endif

        if (DoDbgBrk())
          { int xxx=0; }
        // Assume Linear Relationship - Ideal Gas;


        if (VolStdP>1.0e-6)
          {
          double VGas=Max(CurrentSpace * 0.001, CurrentSpace-VolumeSL);
          //double PEst=m_CurrentP*VolStdP/GTZ(VGas);

          const double MaxPr=50000.0;
          //m_CurrentP=Min(MaxPr, m_CurrentP*VolStdP/GTZ(VGas));
          double VolActP = Volume(som_Gas, m_CurrentT, m_CurrentP, pMArray());

          if (VGas > 1.0e-6)
            {
            CurrentSpace=Shape(this)->ActualVolume();
            VGas=Max(CurrentSpace * 0.001, CurrentSpace-VolumeSL);

            int dbgStarted=0;
            int MaxLoops=s_PVTTol.GetMaxIters();
            int Loop=0;
            bool DoDampOsc=false;
            double DVHist[3]={1,1,1};
            while (((Loop++)<MaxLoops) && !s_PVTTol.ConvergedVV(VolActP, VGas))
              {
              double DT=Min(0.01, 0.5* m_CurrentT);
              double DP=Min(0.01, 0.5* m_CurrentP);

              //FindTFromHAtP(m_CurrentP);

              VolumeSL = Volume(som_SL, m_CurrentT, m_CurrentP, pMArray());
              CurrentSpace=Shape(this)->ActualVolume();
              VGas=Max(CurrentSpace * 0.001, CurrentSpace-VolumeSL);

              VolActP = Volume(som_Gas, m_CurrentT, m_CurrentP, pMArray());
              double VT1= Volume(som_Gas, m_CurrentT+DT, m_CurrentP, pMArray());
              double VP1= Volume(som_Gas, m_CurrentT, m_CurrentP+DP, pMArray());
              double VT2= Volume(som_Gas, m_CurrentT-DT, m_CurrentP, pMArray());
              double VP2= Volume(som_Gas, m_CurrentT, m_CurrentP-DP, pMArray());
              double dVdT=0.5*(VT1-VT2)/(2*DT);
              double dVdP=0.5*(VP1-VP2)/(2*DP);

              double H0= totHf(som_ALL, m_CurrentT, m_CurrentP, pMArray());
              double HT1= totHf(som_ALL, m_CurrentT+DT, m_CurrentP, pMArray());
              double HP1= totHf(som_ALL, m_CurrentT, m_CurrentP+DP, pMArray());
              double HT2= totHf(som_ALL, m_CurrentT-DT, m_CurrentP, pMArray());
              double HP2= totHf(som_ALL, m_CurrentT, m_CurrentP-DP, pMArray());
              double dHdT=0.5*(HT1-HT2)/(2*DT);
              double dHdP=0.5*(HP1-HP2)/(2*DP);

              double Scl=1 || DoDampOsc ? 0.5 : 1.0;

              double ReqdPChg, ReqdTChg, AVT1, AVP1, AVT2, AVP2, dAVdT, dAVdP;

              double VErr=VolActP-VGas;
              double HErr=H0-HReqd;

              switch (Shape(this)->VolumeType())
                {
                case VVT_FixedV:
                  //ReqdPChg=-Scl*(VolActP-VGas)/NZ(dVdP-dHdP*dVdT/dHdT);
                  //ReqdTChg=-ReqdPChg*dHdP/dHdT;
                  ReqdPChg=-Scl*(VErr/NZ(dVdP-dHdP*dVdT/NZ(dHdT))+HErr/NZ(dHdP-dVdP*dHdT/NZ(dVdT)));
                  ReqdTChg=-Scl*(VErr/NZ(dVdT-dHdT*dVdP/NZ(dHdP))+HErr/NZ(dHdT-dVdT*dHdP/NZ(dVdP)));

                  AVP1=0;
                  break;
                case VVT_FunctP:
                  AVT1=Shape(this)->ActualVolume(m_CurrentT+DT, m_CurrentP);
                  AVP1=Shape(this)->ActualVolume(m_CurrentT, m_CurrentP+DP);
                  AVT2=Shape(this)->ActualVolume(m_CurrentT-DT, m_CurrentP);
                  AVP2=Shape(this)->ActualVolume(m_CurrentT, m_CurrentP-DP);
                  dAVdT=0.5*(AVT1-AVT2)/(2*DT);
                  dAVdP=0.5*(AVP1-AVP2)/(2*DP);

                  ReqdPChg=-Scl*(VolActP-VGas)/NZ((dVdP-dAVdP)-dHdP*(dVdT-dAVdT)/dHdT);
                  ReqdTChg=-ReqdPChg*dHdP/dHdT;
                  break;
                case VVT_FixedP:
                  DoBreak();
                  ReqdPChg=-Scl*(VolActP-VGas)/NZ(dVdP-dHdP*dVdT/dHdT);
                  ReqdTChg=-ReqdPChg*dHdP/dHdT;
                  break;
                default:
                  DoBreak();
                }

              ReqdPChg=Range(-m_CurrentP*0.2, ReqdPChg, m_CurrentP*0.2);
              ReqdTChg=Range(-m_CurrentT*0.2, ReqdTChg, m_CurrentT*0.2);
              if (!DoDampOsc)
                {
                DVHist[2]=DVHist[1];
                DVHist[1]=DVHist[0];
                DVHist[0]=ReqdPChg;
                DoDampOsc=(DVHist[2]*DVHist[1]<0 && DVHist[1]*DVHist[0]<0);
                }

#if dbgSpModel
              if (Loop>MaxLoops-10 || dbgPVTConverge() || (dbgPVTConvergeDbg() && DoDbgBrk()))
                {
                dbgp("%4.4s %3i T:%12.5f P:%12.5f dV:%12.5f dH:%12.5f "
                  //"Vg:%12.5f V:%12.5f "
                  "[%s P:+%10.6f T:+%10.6f]",
                  !dbgStarted ? "PVT " : "", Loop, K2C(m_CurrentT), m_CurrentP,
                  VErr, HErr,
                  //VGas, VolActP,
                  DoDampOsc?"#":" ", ReqdPChg, ReqdTChg);
                if (0)
                  dbgp(" dVdT:%8.4f dVdP:%8.4f dHdT:%8.4f dHdP:%8.4f",dVdT,dVdP,dHdT,dHdP);
                if (Shape(this)->VolumeType()==VVT_FunctP)
                  dbgp(" dAVdT:%8.4f dAVdP:%8.4f",dAVdT,dAVdP);
                dbgpln(" %s",FullObjTag());
                dbgStarted=1;
                if (0 && Loop==50)
                  {
                  double P= m_CurrentP-20;
                  double H1= SDB.totHf(m_iFidelity, som_ALL, m_CurrentT, P, &m_Ovr, SVData());

                  for (int i=1; i<50; i++)
                    {
                    P+=40/50.0;
                    double H2= SDB.totHf(m_iFidelity, som_ALL, m_CurrentT, P, &m_Ovr, SVData());
                    dbgpln("P:%12.5f dH:%10.5f ",P,(H2-H1));
                    H1=H2;
                    }
                  }
                if (Loop==MaxLoops)
                  LogError(FullObjTag(), 0, "PVT Not converged T:%gC,P:%g",K2C(m_CurrentT),m_CurrentP);
                }
#endif

              m_CurrentP+=ReqdPChg;
              m_CurrentT+=ReqdTChg;
              //SetTempPress(m_CurrentT, m_CurrentP);
              }
            if (Loop<0)
              LogWarning(FullObjTag(), 0, "Pressure not converged");
            static int Count=0;
            if ((Count++<100) && (CurrentSpace>1.0e-6) && (fabs(VolActP-VGas)>1.0e-4*Max(VGas,VolActP)))
              dbgpln("Non Ideal Gas - Bad Pressure");
            }

          }
        else
          m_CurrentP=0.0;
        if (m_CurrentP<LoPressLift)
          {
          //Limit Lowest Pressure up by 1 kPa at 0 kPa prop to press below 20.0 kPa
          m_CurrentP+=1.0*(LoPressLift-m_CurrentP)/LoPressLift;
          }
        }
      }
    m_iHoldVentExpand--;
    m_dwValidFlags|=SMVF_H_OK;
//dbgpln("SpModel::EvalHStates() ----%10.2f",SW.LapTime()*1e6);
//ftSW=NULL;
    }
  };

// --------------------------------------------------------------------------

double SpModel::Mass(PhMask Phase)
  {
  TestMStatesOK();

  double m=0.0;
  dword Msk=1;
  for (int phi=ph_SolS; phi<=ph_GasE; phi++)
    {
    if (Phase & Msk)
      m+=m_PhMass[phi];
    Msk <<= 1;
    }
  return m;
  };

// --------------------------------------------------------------------------

double SpModel::msCp(PhMask Phase, double T_, double P_, CSysVector * pMA, double *pTotalM)
  {
  SPMDLTIMERLAP(sm_swHeat);
  if (HeatSkipMethod())
    {
    if (pTotalM)
      *pTotalM=Mass(Phase);
    return 1;
    }
  return SDB.msCp(m_iFidelity, Phase, T_, P_, &m_Ovr, pMA ? pMA->SVData() : m_M.SVData(), pTotalM);
  };
double SpModel::msHs(PhMask Phase, double T_, double P_, CSysVector * pMA, double *pTotalM)
  {
  SPMDLTIMERLAP(sm_swHeat);
  if (HeatSkipMethod())
    {
    if (pTotalM)
      *pTotalM=Mass(Phase);
    return T_;
    }
  return SDB.msHs(m_iFidelity, Phase, T_, P_, &m_Ovr, pMA ? pMA->SVData() : m_M.SVData(), pTotalM);
  };
double SpModel::msHz(PhMask Phase, double T_, double P_, CSysVector * pMA, double *pTotalM)
  {
  SPMDLTIMERLAP(sm_swHeat);
  if (HeatSkipMethod())
    {
    if (pTotalM)
      *pTotalM=Mass(Phase);
    return T_;
    }
  if (0 && SpModelExDbgOn)
    {
    dbgpln("SpModel::msHz %2i %04x %7.2f %7.2f %12.3f",
      m_iFidelity, Phase, T_, P_,
      SDB.msHz(m_iFidelity, Phase, T_, P_, &m_Ovr, pMA ? pMA->SVData() : m_M.SVData(), pTotalM));
    }
  return SDB.msHz(m_iFidelity, Phase, T_, P_, &m_Ovr, pMA ? pMA->SVData() : m_M.SVData(), pTotalM);
  };
double SpModel::msHf(PhMask Phase, double T_, double P_, CSysVector * pMA, double *pTotalM)
  {
  if (HeatSkipMethod())
    {
    if (pTotalM)
      *pTotalM=Mass(Phase);
    return T_;
    }
  SPMDLTIMERLAP(sm_swHeat);
  return SDB.msHf(m_iFidelity, Phase, T_, P_, &m_Ovr, pMA ? pMA->SVData() : m_M.SVData(), pTotalM);
  };
double SpModel::msSf(PhMask Phase, double T_, double P_, CSysVector * pMA, double *pTotalM)
  {
  if (HeatSkipMethod())
    {
    if (pTotalM)
      *pTotalM=Mass(Phase);
    return 1;
    }
  SPMDLTIMERLAP(sm_swHeat);
  return SDB.msSf(m_iFidelity, Phase, T_, P_, &m_Ovr, pMA ? pMA->SVData() : m_M.SVData(), pTotalM);
  };
double SpModel::totCp(PhMask Phase, double T_, double P_, CSysVector * pMA)
  {
  SPMDLTIMERLAP(sm_swHeat);
  if (HeatSkipMethod())
    return (pMA ? pMA->Mass(Phase) : Mass(Phase));
  return SDB.totCp(m_iFidelity, Phase, T_, P_, &m_Ovr, pMA ? pMA->SVData() : m_M.SVData());
  };
double SpModel::totHs(PhMask Phase, double T_, double P_, CSysVector * pMA)
  {
  SPMDLTIMERLAP(sm_swHeat);
  if (HeatSkipMethod())
    return (pMA ? pMA->Mass(Phase) : Mass(Phase))*T_;
  return SDB.totHs(m_iFidelity, Phase, T_, P_, &m_Ovr, pMA ? pMA->SVData() : m_M.SVData());
  };
double SpModel::totHz(PhMask Phase, double T_, double P_, CSysVector * pMA)
  {
  SPMDLTIMERLAP(sm_swHeat);
  if (HeatSkipMethod())
    return (pMA ? pMA->Mass(Phase) : Mass(Phase))*T_;
  return SDB.totHz(m_iFidelity, Phase, T_, P_, &m_Ovr, pMA ? pMA->SVData() : m_M.SVData());
  };
double SpModel::totHf(PhMask Phase, double T_, double P_, CSysVector * pMA)
  {
  SPMDLTIMERLAP(sm_swHeat);
  if (HeatSkipMethod())
    return (pMA ? pMA->Mass(Phase) : Mass(Phase))*T_;
  return SDB.totHf(m_iFidelity, Phase, T_, P_, &m_Ovr, pMA ? pMA->SVData() : m_M.SVData());
  };
double SpModel::totSf(PhMask Phase, double T_, double P_, CSysVector * pMA)
  {
  SPMDLTIMERLAP(sm_swHeat);
  if (HeatSkipMethod())
    return (pMA ? pMA->Mass(Phase) : Mass(Phase));
  return SDB.totSf(m_iFidelity, Phase, T_, P_, &m_Ovr, pMA ? pMA->SVData() : m_M.SVData());
  };

// --------------------------------------------------------------------------

double SpModel::Volume(PhMask Phase, double T_, double P_, CSysVector * pMA)
  {
  double m=pMA ? pMA->Mass(Phase) : Mass(Phase);
  double r=(m >= 1.0e-100) ? Rho(Phase, T_, P_, pMA) : DefaultRho;
  return m/GTZ(r);
  };

// --------------------------------------------------------------------------

double SpModel::Volume(CIArray & SpIDs, double T_, double P_, CSysVector * pMA)
  {
  double m=pMA ? pMA->Mass(SpIDs) : Mass(SpIDs);
  double r=(m >= 1.0e-100) ? Rho(SpIDs, T_, P_, pMA) : DefaultRho;
  return m/GTZ(r);
  };

// --------------------------------------------------------------------------

double SpModel::Volume(CSysVector &Scale, double T_, double P_, CSysVector * pMA)
  {
  double m=pMA ? pMA->Mass(Scale) : Mass(Scale);
  double r=(m >= 1.0e-100) ? Rho(Scale, T_, P_, pMA) : DefaultRho;
  return m/GTZ(r);
  };

// --------------------------------------------------------------------------

double SpModel::DynamicViscosity(PhMask Phase)   { return DynamicViscosity(Phase, Temp(), Press()); };
double SpModel::DynamicViscosity(PhMask Phase, double T_, double P_, CSysVector * pMA)
  {
  //TODO This should go to the databaseto collect data -
  // The question is how to combine the data for mixtures

  double mv=0.0;
  double ml=0.0;
  double VVisc=0.0;
  double LVisc=0.0;
  double Tc = K_2_C(T_);
  if (Phase & som_Gas)
    {
    mv=pMA ? pMA->Mass(som_Gas) : Mass(som_Gas);
    if (mv>UsableMass)
      {
      // ********** Vapour viscosity in mPa.s = cP
      if (m_Ovr.bUse & SPO_VDynVisc)
        VVisc=m_Ovr.fVDynVisc/0.001; // will multiply by 0.001 later
      else if (Tc < 250)
        VVisc= 0.00003 * Tc + 0.0089;
      else if (Tc >= 250 && Tc < 310)
        VVisc= 0.00004 * Tc + 0.0062;
      else //if (Tc >= 310)
        VVisc= 0.000003 * Sqr(Tc) - 0.0017 * Tc + 0.2823;
      if (VVisc<1.0e-6)
        {
        VVisc=1.0e-6;
        LogError(FullObjTag(), 0, "Low Vapour Viscosity T=%10.4fC", Tc);
        }
      else if (VVisc>1e12)
        {
        int xxx=0; 
        }
      }
    }
  if (Phase & som_SL)
    {
    ml=pMA ? pMA->Mass(som_SL) : Mass(som_SL);
    if (ml>UsableMass)
      {
      if (m_Ovr.bUse & SPO_LDynVisc)
        LVisc=m_Ovr.fLDynVisc/0.001; // will multiply by 0.001 later
      // ********** Water viscosity in mPa.s = cP
      else if (Tc > 0.01 && Tc < 100)
        LVisc = 1.4851 * Exps(-0.018 * Tc);
      else if (Tc >= 100 && Tc < 200)
        LVisc = 0.5629 * Exps(-0.0073 * Tc);
      else if (Tc >= 200 && Tc < 370)
        LVisc = -0.0004 * Tc + 0.2188;
      else
        LVisc = 1.0; // Hard Wired;
      if (LVisc<1.0e-20)
        {
        Strng S;
        S.Set("Silly Liquid Viscosity T=%10.4fC", Tc);
        ASSERT_ALWAYS(FALSE, S(), __FILE__, __LINE__);
        }
      }
    }

  if (mv+ml>UsableMass)
    {
    //TODO This is not really correct
    double VFrac=mv/(mv+ml);
    return (VVisc*VFrac+LVisc*(1-VFrac))*0.001;
    }
  return 0.0;
  };

// --------------------------------------------------------------------------

double SpModel::KinematicViscosity(PhMask Phase)   { return KinematicViscosity(Phase, Temp(), Press()); };
double SpModel::KinematicViscosity(PhMask Phase, double Temp, double Pres, CSysVector * pMA)
  {
  return DynamicViscosity(Phase, Temp, Pres, pMA)/GTZ(Rho(Phase, Temp, Pres, pMA));
  };

// --------------------------------------------------------------------------

double SpModel::ThermalConductivity(PhMask Phase)   { return ThermalConductivity(Phase, Temp(), Press()); };
double SpModel::ThermalConductivity(PhMask Phase, double Temp, double Pres, CSysVector * pMA)
  {
  //TODO This should go to the databaseto collect data -
  // The question is how to combine the data for mixtures

  double mv=0.0;
  double ml=0.0;
  double VCond=0.0;
  double LCond=0.0;
  if (Phase & som_Gas)
    {
    mv=pMA ? pMA->Mass(som_Gas) : Mass(som_Gas);
    if (mv>UsableMass)
      {
      if (m_Ovr.bUse & SPO_VThermCnd)
        VCond=m_Ovr.fVThermCnd;
      else
        VCond= 0.000016; // Hardwired for Steam
      }
    }
  if (Phase & som_SL)
    {
    ml=pMA ? pMA->Mass(som_SL) : Mass(som_SL);
    if (ml>UsableMass)
      {
      if (m_Ovr.bUse & SPO_LThermCnd)
        LCond=m_Ovr.fLThermCnd;
      else
        LCond = 0.00058; // Hardwired for Water
      }
    }

  if (mv+ml>UsableMass)
    {
    //TODO This is not really correct
    double VFrac=mv/(mv+ml);
    return (VCond*VFrac+LCond*(1-VFrac));
    }
  return 0.0;
  };

// --------------------------------------------------------------------------

double SpModel::PrandtlNo(PhMask Phase)   { return PrandtlNo(Phase, Temp(), Press()); };
double SpModel::PrandtlNo(PhMask Phase, double Temp, double Pres, CSysVector * pMA)
  {
  return DynamicViscosity(Phase, Temp, Pres, pMA)*msCp(Phase, Temp, Pres, pMA)/GTZ(ThermalConductivity(Phase, Temp, Pres, pMA));
  };

// --------------------------------------------------------------------------

double SpModel::SurfaceTension(PhMask Phase)
  {
  return SurfaceTension(Phase, Temp(), Press());
  }

double SpModel::SurfaceTension(PhMask Phase, double T_, double P_, CSysVector * pMA)
  {
  if (m_Ovr.bUse & SPO_SurfT)
    return m_Ovr.fSurfT;

  // ********** SurfaceTension tension of water in N/m
  //KELV = Tc + 273.15
  const double Tx = GEZ(647.15 - T_) / 647.15;
  const double C10 = 0.2358 * Pow(Tx, 1.256);
  const double C11 = 1 - 0.625 * Tx;
  return C10 * C11;
  }

// --------------------------------------------------------------------------

double SpModel::HeatOfCombustionHi(PhMask Phase)
  {
  if (m_bUsableMass)
    return SDB.HeatOfCombustionHi(m_iFidelity, Phase, Temp(), Press(), SVData());
  return 0.0;
  }

double SpModel::HeatOfCombustionHi(PhMask Phase, double T_, double P_, CSysVector * pMA)
  {
  if (m_bUsableMass)
    return SDB.HeatOfCombustionHi(m_iFidelity, Phase, T_, P_, pMA ? pMA->SVData() : SVData());
  return 0.0;
  };


// --------------------------------------------------------------------------

double SpModel::HeatOfCombustionLo(PhMask Phase)
  {
  if (m_bUsableMass)
    return SDB.HeatOfCombustionLo(m_iFidelity, Phase, Temp(), Press(), SVData());
  return 0.0;
  }
double SpModel::HeatOfCombustionLo(PhMask Phase, double T_, double P_, CSysVector * pMA)
  {
  if (m_bUsableMass)
    return SDB.HeatOfCombustionLo(m_iFidelity, Phase, T_, P_, pMA ? pMA->SVData() : SVData());
  return 0.0;
  };

// --------------------------------------------------------------------------

double SpModel::TCritical(PhMask Phase, eScdGasMixingRule Rule)
  {
  SpMArray Ml;
  double Mt=0, Tc=0, Xs=0;
  SpecieIter I(Phase);
  for (int i=-1; I.Loop(i); )
    {
    Ml.VValue[i] = m_M[i]*SDB[i].MoleWt();
    Mt+=Ml[i];
    }
  Mt=GTZ(Mt);

  switch (Rule!=eScdGMR_Default ? Rule : sm_iGasMixingRule)
    {
    case eScdGMR_Simple:
      {
      for (int i=-1; I.Loop(i); )
        {
        double x=Ml[i]/Mt;
        double xx=Pow(x, m_dGasMixAlpha);
        Tc+=xx*SDB[i].TCrit();
        Xs+=xx;
        }
      return Tc/GTZ(Xs);
      };
    }

  return 1;
  };
double SpModel::PCritical(PhMask Phase, eScdGasMixingRule Rule)
  {
  SpMArray Ml;
  double Mt=0, Pc=0, Xs=0;
  SpecieIter I(Phase);
  for (int i=-1; I.Loop(i); )
    {
    Ml.VValue[i] = m_M[i]*SDB[i].MoleWt();
    Mt+=Ml[i];
    }
  Mt=GTZ(Mt);

  switch (Rule!=eScdGMR_Default ? Rule : sm_iGasMixingRule)
    {
    case eScdGMR_Simple:
      {
      for (int i=-1; I.Loop(i); )
        {
        double x=Ml[i]/Mt;
        double xx=Pow(x, m_dGasMixAlpha);
        Pc+=xx*SDB[i].PCrit();
        Xs+=xx;
        }
      return Pc/GTZ(Xs);
      };
    }
  return 1;
  };
double SpModel::VCritical(PhMask Phase, eScdGasMixingRule Rule)
  {
  SpMArray Ml;
  double Mt=0, Vc=0, Xs=0;
  SpecieIter I(Phase);
  for (int i=-1; I.Loop(i); )
    {
    Ml.VValue[i] = m_M[i]*SDB[i].MoleWt();
    Mt+=Ml[i];
    }
  Mt=GTZ(Mt);

  switch (Rule!=eScdGMR_Default ? Rule : sm_iGasMixingRule)
    {
    case eScdGMR_Simple:
      {
      for (int i=-1; I.Loop(i); )
        {
        double x=Ml[i]/Mt;
        double xx=Pow(x, m_dGasMixAlpha);
        Vc+=xx*SDB[i].VCrit();
        Xs+=xx;
        }
      return Vc/GTZ(Xs);
      };
    }
  return 1;
  };
double SpModel::AccentricFactor(PhMask Phase, eScdGasMixingRule Rule)
  {
  SpMArray Ml;
  double Mt=0, Ac=0, Xs=0;
  SpecieIter I(Phase);
  for (int i=-1; I.Loop(i); )
    {
    Ml.VValue[i] = m_M[i]*SDB[i].MoleWt();
    Mt+=Ml[i];
    }
  Mt=GTZ(Mt);

  switch (Rule!=eScdGMR_Default ? Rule : sm_iGasMixingRule)
    {
    case eScdGMR_Simple:
      {
      for (int i=-1; I.Loop(i); )
        {
        double x=Ml[i]/Mt;
        double xx=Pow(x, m_dGasMixAlpha);
        Ac+=xx*SDB[i].ACent();
        Xs+=xx;
        }
      return Ac/GTZ(Xs);
      };
    }
  return 1;
  };

double SpModel::msdHIdeal(PhMask Phase, double T1, double T2)
  {
  // Mass Weight sum ....
  double Mt=0, dH=0;
  SpecieIter I(Phase);
  for (int i=-1; I.Loop(i); )
    {
    dH += m_M[i]*SDB[i].msdHIdeal(m_iFidelity, T1, T2);
    Mt += m_M[i];
    }
  return dH/GTZ(Mt);
  };
double SpModel::msdSIdeal(PhMask Phase, double T1, double T2)
  {
  // Mass Weight sum ....
  double Mt=0, dS=0;
  SpecieIter I(Phase);
  for (int i=-1; I.Loop(i); )
    {
    dS += m_M[i]*SDB[i].msdSIdeal(m_iFidelity, T1, T2);
    Mt += m_M[i];
    }
  return dS/GTZ(Mt);
  };

double SpModel::msdHResidual(PhMask Phase, double T, double P, eScdGasMixingRule Rule, eScdEqnOfState Eqn)
  {
  //long    SpModel::sm_iGasMixingRule = eScdGMR_Simple;

  if ((Phase & som_Gas) && Mass(som_Gas)>ZeroLimit)
    {
    //Residual enthalpy"""
    double TRed = T / TCritical(Phase, Rule);   // Reduced variables
    double PRed = P / PCritical(Phase, Rule);
    double wac = AccentricFactor(Phase, Rule);
    double mwt=MoleWt(Phase);
    static const double R=8.31433;
    switch (Eqn!=eScdEOS_Default ? Eqn : sm_iEqnOfState)
      {
      case eScdEOS_SRK:
        {
        double m = 0.48 + wac * (1.574 - 0.176 * wac);
        double alpha = 1 + m * (1 - sqrt(TRed));
        double A = 0.42747 * alpha*alpha * PRed / TRed / TRed;
        double B = 0.08664 * PRed / TRed;
        double ZZ = ::FindLargestCubeRoot(-1., A - B * (1 + B), -A * B);
        double dHRT=1. - ZZ + 0.42747/0.08664/TRed*(1+m)*alpha*log(1+B/ZZ);
        return dHRT * R / mwt * T;
        }
      }
    }
  return 0;
  };

double SpModel::msdSResidual(PhMask Phase, double T, double P, eScdGasMixingRule Rule, eScdEqnOfState Eqn)
  {
  if ((Phase & som_Gas) && Mass(som_Gas)>ZeroLimit)
    {
    //Residual entropy"""
    double TRed = T / TCritical(Phase, Rule);   // Reduced variables
    double PRed = P / PCritical(Phase, Rule);
    double wac = AccentricFactor(Phase, Rule);
    double mwt=MoleWt(Phase);
    static const double R=8.31433;
    switch (Eqn!=eScdEOS_Default ? Eqn : sm_iEqnOfState)
      {
      case eScdEOS_SRK:
        {
        double m = 0.48 + wac * (1.574 - 0.176 * wac);
        double alpha = 1 + m * (1 - sqrt(TRed));
        double A = 0.42747 * alpha*alpha * PRed / TRed / TRed;
        double B = 0.08664 * PRed / TRed;
        double ZZ = ::FindLargestCubeRoot(-1., A - B * (1 + B), -A * B);
        double dHRT=1. - ZZ + 0.42747/0.08664/TRed*(1+m)*alpha*log(1+B/ZZ);
        double lnPhi = ZZ -1 -log(ZZ-B) -A/B*log(1+B/ZZ);
        return R * (dHRT+lnPhi)/mwt;
        }
      }
    }
  return 0;
  };

// --------------------------------------------------------------------------

double SpModel::BoilingPtElevation(double P_, CSysVector * pMA)
  {
  if (m_Ovr.bUse & SPO_BPEle)
    return m_Ovr.fBPEle;

  if (0)
    {
    double A=SaturationT(P_, pMA);
    double B=PureSaturationT(P_, pMA);
    dbgpln("SpModel::BoilingPtElevation %10.3f %10.3f %10.3f %10.3f", P_, A-B, A, B);
    }
  return SaturationT(P_, pMA) - PureSaturationT(P_, pMA);
  };

// --------------------------------------------------------------------------

double SpModel::SpecieConc(double T_, int iSpNo, PhMask Phase)
  {
  SpMArray Ma(MArray(), Phase);
  return Ma[iSpNo] /GTZ(Ma.Mass()/GTZ(Rho(som_ALL, T_, Press(), &Ma)));
  }

// --------------------------------------------------------------------------

double SpModel::PhaseConc(double T_, PhMask Ph, PhMask Phase)
  {
  SpMArray Ma(MArray(), Phase);
  return Mass(Ph)/GTZ(Ma.Mass()/GTZ(Rho(som_ALL, T_, Press(), &Ma)));
  }

// --------------------------------------------------------------------------

void SpModel::KeepTemp()
  {
  if (!Finite(m_KeptTemp))
    m_KeptTemp=Temp();
  //if (!Finite(m_RqdTemp))
  //  {
  //  if (m_bTempKept)
  //    { int xxx=0; }
  //  }
  };

// --------------------------------------------------------------------------
void SpModel::SetFileTemp(double T)
  {
  if (!Finite(m_FileTemp))
    m_FileTemp=T;
  };
// --------------------------------------------------------------------------
void SpModel::SetFilePress(double P)
  {
  if (!Finite(m_FilePres))
    m_FilePres=P;
  };
// --------------------------------------------------------------------------

double SpModel::Temp()
  {
  TestHStatesOK();

  return m_CurrentT;
  };

// --------------------------------------------------------------------------

double SpModel::dTemp()
  {
  return dNAN;
  };

// --------------------------------------------------------------------------

double SpModel::Press()
  {
  TestHStatesOK();

  return m_CurrentP;
  }

// --------------------------------------------------------------------------

double SpModel::PartialPress(SpecieIter &Ids, double T)
  {
  TestHStatesOK();

  double PPart=0;
  if (1)//m_bClosed && !m_bPressAsSet)
    {
    double P=Press();
    double Vol1=0;
    double VolA = SDB.Volume(m_iFidelity, som_Gas, T, P, &m_Ovr, SVData());

    for (int i=-1; Ids.Loop(i); )
      if (SDB[i].DensityDirect())
        Vol1 += m_M[i]*SDB[i].msVolume(m_iFidelity, T, P, &m_Ovr, SVData());
    // Assume Linear Relationship - Ideal Gas;
    PPart=P*Vol1/GTZ(VolA);
    //if (PPart<20.0)
    //  {
    //  //Limit Lowest Pressure up by 1 kPa at 0 kPa prop to press below 20.0 kPa
    //  PPart+=1.0*(20.0-PPart)/20.0;
    //  }
    }

  return PPart;
  }

// --------------------------------------------------------------------------

double SpModel::PartialPress(int IncIds, int ExcIds, double T)
  {
  TestHStatesOK();

  double P=Press();
  double PPart=0.0;

  if (0)
    dbgpln("SpModel::PartialPress T:%12.4f P:%12.4f", T, P);

  if (1)//m_bClosed && !m_bPressAsSet)
    {
    if (IncIds>=0 )
      {
      double Vol1 = m_M[IncIds]*SDB[IncIds].msVolume(m_iFidelity, T, P, &m_Ovr, SVData());
      double VolA = SDB.Volume(m_iFidelity, som_Gas, T, P, &m_Ovr, SVData());
      // Assume Linear Relationship - Ideal Gas;
      PPart=P*Vol1/GTZ(VolA);
      }
    else
      {
      SpecieIter I(som_Gas);
      for (int i=-1; I.Loop(i); )
        {
        if (i!=ExcIds)
          {
          double Vol1 = m_M[i]*SDB[i].msVolume(m_iFidelity, T, P, &m_Ovr, SVData());
          double VolA = SDB.Volume(m_iFidelity, som_Gas, T, P, &m_Ovr, SVData());
          // Assume Linear Relationship - Ideal Gas;
          PPart+=P*Vol1/GTZ(VolA);
          if (0)
            dbgpln("   %3i) V1:%12.4f Va:%12.4f PP:%12.4f (PPart:%12.4f) %s", 
             i, Vol1, VolA, P*Vol1/GTZ(VolA), PPart, SDB[i].SymOrTag());
          }
        }
      }
    }

  return PPart;
  }

// --------------------------------------------------------------------------

double SpModel::PartialPressC(int CmpIncIds, int CmpExcIds, double T)
  {
  int SpIncIds=-1;
  int SpExcIds=-1;
  if (CmpIncIds>=0)
    SpIncIds=CDB[CmpIncIds].VapPhInx();
  if (CmpExcIds>=0)
    SpExcIds=CDB[CmpExcIds].VapPhInx();
  return PartialPress(SpIncIds, SpExcIds, T);
  }

// --------------------------------------------------------------------------

double SpModel::PartialPressFrac(SpecieIter &Ids, double T)
  {
  TestHStatesOK();

  double Pf=0;
  if (1)//m_bClosed && !m_bPressAsSet)
    {
    double P=Press();
    double Vol1=0;
    double VolA= SDB.Volume(m_iFidelity, som_Gas, T, P, &m_Ovr, SVData());

    for (int i=-1; Ids.Loop(i); )
      if (SDB[i].DensityDirect())
        Vol1 += m_M[i]*SDB[i].msVolume(m_iFidelity, T, P, &m_Ovr, SVData());

    // Assume Linear Relationship - Ideal Gas;
    Pf=Vol1/GTZ(VolA);
    }

  return Pf;
  }

// --------------------------------------------------------------------------

double SpModel::PartialPressFrac(int IncIds, int ExcIds, double T)
  {
  TestHStatesOK();

  double P=Press();
  double Pf=0;
  if (1)//m_bClosed && !m_bPressAsSet)
    {
    if (IncIds>=0 )
      {           
      double VolA = SDB.Volume(m_iFidelity, som_Gas, T, P, &m_Ovr, SVData());
      double Vol1 = m_M[IncIds]*SDB[IncIds].msVolume(m_iFidelity, T, P, &m_Ovr, SVData());

      // Assume Linear Relationship - Ideal Gas;
      Pf=Vol1/GTZ(VolA);
      }
    else
      {
      double Vol1 = 0;
      double VolA = SDB.Volume(m_iFidelity, som_Gas, T, P, &m_Ovr, SVData());
      SpecieIter I(som_Gas);
      for (int i=-1; I.Loop(i); )
        {
        if (i!=ExcIds)
          Vol1 += m_M[IncIds]*SDB[IncIds].msVolume(m_iFidelity, T, P, &m_Ovr, SVData());
        }
      Pf=Vol1/GTZ(VolA);
      }
    }
  return Pf;
  }

// --------------------------------------------------------------------------

double SpModel::PartialPressFracC(int CmpIncIds, int CmpExcIds, double T)
  {
  int SpIncIds=-1;
  int SpExcIds=-1;
  if (CmpIncIds>=0)
    SpIncIds=CDB[CmpIncIds].VapPhInx();
  if (CmpExcIds>=0)
    SpExcIds=CDB[CmpExcIds].VapPhInx();
  return PartialPress(SpIncIds, SpExcIds, T);
  }

// --------------------------------------------------------------------------

void SpModel::SetTempPress(double T_, double P_)
  {
  TestMStatesOK();
  m_CurrentP=Max(0.001, P_);
  if (!m_bTraceMass)// || m_bTempKept)
    {
    m_RqdTemp=T_;
    m_CurrentT=T_;
    ClrHStatesOK();
    m_KeptTemp=dNAN;
    }
  else
    {
    EquilibrateAll(T_, P_);

    m_RqdTemp=dNAN;
    m_msHf=msHf(som_ALL, T_, P_, pMArray());
    ASSERT(Finite(m_msHf));
    m_CurrentT=T_;
    m_dwValidFlags |= SMVF_H_OK;
    m_PreviousT=m_CurrentT;
    m_KeptTemp=dNAN;

    DoTempChecks(m_CurrentT);
    }
  };

// --------------------------------------------------------------------------

void SpModel::SetPress(double P_)
  {
  P_=Max(0.001, P_);
  m_msHf=msHf(som_ALL, Temp(), P_, pMArray());
  m_CurrentP=P_;
  ClrHStatesOK();

  m_RqdTemp=dNAN;
  m_KeptTemp=dNAN;
  };

// --------------------------------------------------------------------------

void SpModel::SetTemp(double T_)
  {
  if (!m_bTraceMass)// || Finite(m_KeptTemp))
    {
    m_RqdTemp=T_;
    ClrHStatesOK();
    m_KeptTemp=dNAN;
    }
  else
    {
    TestMStatesOK();
    m_RqdTemp=dNAN;

    m_msHf=msHf(som_ALL, T_, m_CurrentP, pMArray());
    ASSERT(Finite(m_msHf));

    m_CurrentT=T_;
    m_dwValidFlags |= SMVF_H_OK;
    m_PreviousT=m_CurrentT;
    DoTempChecks(m_CurrentT);
    }
  };

// --------------------------------------------------------------------------

//void SpModel::putVal(int SpNo, double M_)
//  {
//  KeepTemp();
//  M.Val[SpNo]=M_;
//#ifndef _RELEASE
//  if (m_bTestMassNotNeg && M_<0)
//    DoBreak();
//#endif
//  ClrStatesOK();
//  };

// --------------------------------------------------------------------------

//void SpModel::putMss(int SpNo, double M_)
//  {
//  KeepTemp();
//  M.VMass[SpNo]=M_;
//#ifndef _RELEASE
//  if (m_bTestMassNotNeg && M_<0)
//    DoBreak();
//#endif
//  ClrStatesOK();
//  };

// --------------------------------------------------------------------------
#ifndef _RELEASE
void SpModel::DoTestMassNotNeg()
  {
  for (int s=0; s<SVSpcCount(); s++)
    if (m_M[s]<0)
      {
      LogError(FullObjTag(), 0, "Negative Specie %10.3g %s", m_M[s], SDB[s].SymOrTag());
      m_M.VValue[s]=0;
      }
  };
#endif
// --------------------------------------------------------------------------

void SpModel::Set_msHs(double H_, double P, SpModel * pRefMdl)
  {
  if (Valid(P))
    m_CurrentP=Max(0.001, P);
  ASSERT_ALWAYS(FALSE, "INCOMPLETE", __FILE__, __LINE__);
    //m_totH=H_*Mass()+totdHf(som_ALL, C_2_K(0));
  ASSERT(Valid(m_msHf));
  ClrHStatesOK();
  m_KeptTemp=dNAN;
  m_RqdTemp=dNAN;
  };
void SpModel::Set_msHz(double H_, double P, SpModel * pRefMdl)
  {
  if (Valid(P))
    m_CurrentP=Max(0.001, P);
  //ASSERT_ALWAYS(FALSE, "INCOMPLETE", __FILE__, __LINE__);
  if (pRefMdl)
    m_msHf=H_+pRefMdl->msHf(som_ALL, ZeroCinK, Std_P, &MArray());
  else
    m_msHf=H_+msHf(som_ALL, ZeroCinK, Std_P);
  ASSERT(Valid(m_msHf));
  //dbgpln("Set_msHz  @ %#010x = %14.4f (%14.4f)", this, m_msHz, H_);
  ClrHStatesOK();
  m_KeptTemp=dNAN;
  m_RqdTemp=dNAN;
  };
void SpModel::Set_msHf(double H_, double P, SpModel * pRefMdl)
  {
  if (Valid(P))
    m_CurrentP=Max(0.001, P);
  m_msHf=H_;//-msHf(som_ALL, ZeroCinK, Std_P);
  ASSERT(Valid(m_msHf));
  ASSERT(pRefMdl==NULL);
  //dbgpln("Set_msHf  @ %#010x = %14.4f (%14.4f)", this, m_msHz, H_);
  ClrHStatesOK();
  m_KeptTemp=dNAN;
  m_RqdTemp=dNAN;
  };

void SpModel::Set_totHs(double H_, double P, SpModel * pRefMdl)
  {
  if (Valid(P))
    m_CurrentP=Max(0.001, P);
  if (pRefMdl)
    m_msHf=(H_+pRefMdl->totHf(som_ALL, C2K(0), StdP, &MArray()))/GTZ(Mass());
  else
    m_msHf=(H_+totHf(som_ALL, C2K(0), StdP))/GTZ(Mass());
  ASSERT(Valid(m_msHf));
  //dbgpln("Set_totHz @ %#010x = %14.4f (%14.4f)", this, m_msHz, H_);
  ClrStatesOK();
  m_KeptTemp=dNAN;
  m_RqdTemp=dNAN;
  };
void SpModel::Set_totHz(double H_, double P, SpModel * pRefMdl)
  {
  if (Valid(P))
    m_CurrentP=Max(0.001, P);
  if (pRefMdl)
    m_msHf=(H_+pRefMdl->totHfb0(som_ALL, &MArray()))/GTZ(Mass());
  else
    m_msHf=(H_+totHfb0(som_ALL))/GTZ(Mass());
  ASSERT(Valid(m_msHf));
  //dbgpln("Set_totHz @ %#010x = %14.4f (%14.4f)", this, m_msHz, H_);
  ClrStatesOK();
  m_KeptTemp=dNAN;
  m_RqdTemp=dNAN;
  };
void SpModel::Set_totHf(double H_, double P, SpModel * pRefMdl)
  {
  if (Valid(P))
    m_CurrentP=Max(0.001, P);
  m_msHf=(H_)/GTZ(Mass());
  ASSERT(Valid(m_msHf));
  ASSERT(pRefMdl==NULL);
  //dbgpln("Set_totHf @ %#010x = %14.4f (%14.4f)", this, m_msHz, H_);
  ClrStatesOK();
  m_KeptTemp=dNAN;
  m_RqdTemp=dNAN;
  };

// --------------------------------------------------------------------------

void SpModel::SetDebug(flag DbgOn_)
  {
  //  if (iEnable!=IE_Null)
  //    {
  //    if (iM)
  //      {
  //      iTState->SetDbgOn(DbgOn_);
  //      for (int i=0; i < SDB.No(); i++)
  //        if (iM[i])
  //          iM[i]->SetDbgOn(DbgOn_);
  //      }
  //    }
  for (int q=0; q<nQuals; q++)
    if (Quals[q].m_pQ)
      Quals[q].m_pQ->SetDebug(DbgOn_);
  };

//===========================================================================

double SpModel::msLatentHeatVap(double T, double P, CSysVector * pMA)
  {
  //  if (SatPressCompV()>=0 && SatPressCompL()>=0)
  if (CDB.DefFlashCmpIndex()>=0)
    {
    CSysVector &MA = (pMA==NULL ? MArray() : *pMA);
    double H0=CDB.DefFlashVapDesc().msHf(m_iFidelity, T, P, &m_Ovr, MA.SVData());
    double H1=CDB.DefFlashLiqDesc().msHf(m_iFidelity, T, P, &m_Ovr, MA.SVData());
    return H0-H1;
    }
  return 0.0;
  };

//===========================================================================

int SpModel::FlashCmpIndex()
  {
  if (m_pVLE && m_pVLE->Enabled())
    return m_pVLE->FlashCmpIndex();
  if (AutoVLEOn())
    return m_AutoVLE.m_iCmp;
  return CDB.DefFlashCmpIndex();
  };

int SpModel::FlashLiqIndex()
  {
  if (m_pVLE && m_pVLE->Enabled())
    return m_pVLE->FlashLiqIndex();
  if (AutoVLEOn())
    return m_AutoVLE.m_iLiq;
  if (CDB.DefFlashCmpIndex()>=0)
    return CDB.DefFlashLiqIndex();
  return -1;
  };

int SpModel::FlashVapIndex()
  {
  if (m_pVLE && m_pVLE->Enabled())
    return m_pVLE->FlashVapIndex();
  if (AutoVLEOn())
    return m_AutoVLE.m_iVap;
  if (CDB.DefFlashCmpIndex()>=0)
    return CDB.DefFlashVapIndex();
  return -1;
  };

LPTSTR SpModel::FlashDescription()
  {
  static CString X;
  X="None";
  if (m_pVLE && m_pVLE->Enabled())
    {
    X="VLE:";
    X+=m_pVLE->FlashDescription();
    //X+=SDB[m_pVLE->.m_iLiq].SymOrTag();
    //X+="/";
    //X+=SDB[m_AutoVLE.m_iVap].SymOrTag();
    }
  else if (AutoVLEOn())
    {
    X="Auto:Vp:";
    X+=SDB[m_AutoVLE.m_iLiq].SymOrTag();
    X+="/";
    X+=SDB[m_AutoVLE.m_iVap].SymOrTag();
    }
  else if (CDB.DefFlashCmpIndex()>=0)
    {
    X="Def:Vp:";
    X+=CDB.DefFlashLiqDesc().SymOrTag();
    X+="/";
    X+=CDB.DefFlashVapDesc().SymOrTag();
    }
  return (LPTSTR)(LPCTSTR)X;
  };

LPTSTR SpModel::SaturationDescription()    
  { 
  static CString X;
  if (m_iSatCmpRqd<0)
    {
    X="(";
    X+=CDB[SaturationCmpIndex()].SymOrTag();
    X+=")";
    }
  else
    X=CDB[SaturationCmpIndex()].SymOrTag();
  return (LPTSTR)(LPCTSTR)X;
  };
int SpModel::SaturationCmpIndex()
  { 
 if (m_iSatCmpRqd>=0)
    return m_iSatCmpRqd;
  if (m_iSatCmp>=0)
    return m_iSatCmp;
  return CDB.DefFlashCmpIndex();
  };

double SpModel::SaturationP(double T_, CSysVector * pMA, int iSatComp)
  {
  if (iSatComp>=0)
    {
    //dbgpln("SpModel::SaturationP %-20s T:%12.4f = %12.f %s", "SatComp", T_, CDB[iSatComp].VapourP(m_iFidelity, T_), FullObjTag());
    return CDB[iSatComp].VapourP(m_iFidelity, T_);
    }

  if (m_pVLE && m_pVLE->Enabled())
    {
    double P=m_pVLE->SaturationP(T_, pMA);
    if (Valid(P))
      {
      //dbgpln("SpModel::SaturationP %-20s T:%12.4f = %12.f %s", "VLE", T_, P, FullObjTag());
      return P;
      }
    }

  if (m_AutoVLE.m_iCmp>=0)
    {
    //dbgpln("SpModel::SaturationP %-20s T:%12.4f = %12.f %s", "AutoVLE", T_, CDB[m_AutoVLE.m_iCmp].VapourP(m_iFidelity, T_), FullObjTag());
    return CDB[m_AutoVLE.m_iCmp].VapourP(m_iFidelity, T_);
    }

  if (CDB.DefFlashCmpIndex()>=0)
    {
    //dbgpln("SpModel::SaturationP %-20s T:%12.4f = %12.f %s", "DefFlash", T_, CDB.DefFlashCmpDesc().VapourP(m_iFidelity, T_), FullObjTag());
    return CDB.DefFlashCmpDesc().VapourP(m_iFidelity, T_);
    }

  dbgpln("SpModel::SaturationP = No Calculation");
  return Std_P;
  };

double SpModel::SaturationTotalP(double T_, CSysVector * pMA, int iSatComp)
  {
  if (iSatComp>=0)
    {
    double PP=PartialPress(-1, iSatComp, T_);  
    return CDB[iSatComp].VapourP(m_iFidelity, T_)+PP;
    }

  if (m_pVLE && m_pVLE->Enabled())
    {
    double P=m_pVLE->SaturationTotalP(T_, pMA ? pMA : pMArray(), this);
    if (Valid(P))
      return P;
    }

  if (m_AutoVLE.m_iCmp>=0)
    {
    double PP=PartialPress(-1, m_AutoVLE.m_iCmp, T_);  
    return CDB[m_AutoVLE.m_iCmp].VapourP(m_iFidelity, T_)+PP;
    }

  if (CDB.DefFlashCmpIndex()>=0)
    {
    double PP=PartialPress(-1, CDB.DefFlashCmpIndex(), T_);  
    return CDB.DefFlashCmpDesc().VapourP(m_iFidelity, T_)+PP;
    }
  dbgpln("SpModel::SaturationTotalP = No Calculation");
  return Std_P;
  };

//---------------------------------------------------------------------------


// -------------------------------------------------------------------------

class SatTFnd : public MRootFinderBase
  {
  public:
    CSysVector & MA;
    SpModel *Mdl;
    int m_iSatComp;
    static CToleranceBlock s_Tol;
    SatTFnd(CSysVector *pMA, SpModel * pMdl_, int iSatComp) : MA(*pMA), MRootFinderBase("SatTFnd", s_Tol)
      {
      Mdl=pMdl_;
      m_iSatComp=iSatComp;
      SetErrorHandling(true, &Mdl->m_fConvergeBusy);
      };
    LPCTSTR ObjTag() { return (LPCTSTR)Mdl->FullObjTag(); };
    double Function(double x) { return Mdl->SpModel::SaturationP(x, NULL, m_iSatComp); };
  };

CToleranceBlock SatTFnd::s_Tol(TBF_BothSys, "SpModel:SatTFnd", 0, 1.0e-9, 100, TBF_Fixed|TBF_UseAbs|TBF_UseRel|TBF_UseMax);

// -------------------------------------------------------------------------


double SpModel::SaturationT(double P_, CSysVector * pMA, int iSatComp)
  {
  CSysVector & MA = pMA==NULL ? MArray() : *pMA;
  double TRet=Std_T;
  bool Set4=false;
  bool Set6=false;
  bool Set7=false;
  if (MA.Mass()>1.0e-6)
    {
    SatTFnd SatT(&MA, this, iSatComp);
    SatT.SetTarget(P_);
    double LoLim=0.1;//Std_T-100.0;
    double HiLim=5000.0;//IF97_MaxSatT;//647.096;//CDB.MaxT;
    SatT.SetLimits(LoLim, HiLim);

    if (Valid(m_dPrvSatT) && SatT.SolveFromEst(m_dPrvSatT, 1.0, 50.0)==RF_OK)
      {
      m_dPrvSatT=SatT.Result();
      TRet=m_dPrvSatT;
      }
    else 
      {
      switch (SatT.Start(LoLim, HiLim))
        {
        case RF_OK:
          if (SatT.Solve_Brent()==RF_OK)
            {
            m_dPrvSatT=SatT.Result();
            TRet = m_dPrvSatT;
            }
          else
            Set4=true;
          break;
        case RF_LoLimit:
          Set6=true;
          m_dPrvSatT=SatT.Result();
          TRet = m_dPrvSatT;
          break;
        case RF_HiLimit:
          Set7=true;
          m_dPrvSatT=SatT.Result();
          TRet = m_dPrvSatT;
          break;
        default:
          if (!m_fConvergeBusy)
            {
            if (CDB.DefFlashCmpIndex()>=0) 
              {
              if (MA[CDB.DefFlashLiqIndex()] + MA[CDB.DefFlashVapIndex()]>1e-12)
                Set4=true; // a FLash Component (the default) exists
              }
            }
        }
      }
    }
  
  SetCI(4, Set4);
  SetCI(6, Set6);
  SetCI(7, Set7);

  return TRet;
  };

//===========================================================================

flag SpModel::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case 1: pS="W\tSlurry Venting";                     return 1;
    case 2: pS="W\tGas Venting";                        return 1;
    case 3: pS="W\tVolume Expanded";                    return 1;
    case 4: pS="W\tSaturationT not converged";          return 1;
    case 5: pS="W\tTemp/Press Out of Range for ????";   return 1;
    case 6: pS="N\tSaturationT Low Limit";              return 1;
    case 7: pS="N\tSaturationT High Limit";             return 1;
    default:
      return TaggedObject::CIStrng(No, pS);
    }
  };

//--------------------------------------------------------------------------

double SpModel::msHfb0(PhMask Phase, CSysVector * pMA)
  {
  //double H=msHz(Phase, T_, P_, pMA);

  CSysVector &MA = ((pMA==NULL) ? MArray() : *pMA);
  double Ht=0, Mt=0;
  SpecieIter I(Phase);
  for (int i=-1; I.Loop(i); )
    if ((MA[i] > MeasTolerance) && SDB[i].CpDirect())
      {
      Ht+=MA[i]*SDB[i].msHfBase0(m_iFidelity);
      Mt+=MA[i];
      }
    return Ht/GTZ(Mt);
  };

//--------------------------------------------------------------------------

double SpModel::msH_DHfb0(PhMask Phase, CSysVector * pMA)
  {
  //double H=msHz(Phase, T_, P_, pMA);

  CSysVector &MA = ((pMA==NULL) ? MArray() : *pMA);
  double Ht=0, Mt=0;
  SpecieIter I(Phase);
  for (int i=-1; I.Loop(i); )
    if ((MA[i] > MeasTolerance) && SDB[i].CpDirect())
      {
      Ht+=MA[i]*(SDB[i].msHfBase0(m_iFidelity)-SDB[i].msHf0(m_iFidelity));
      Mt+=MA[i];
      }
    return Ht/GTZ(Mt);
  };

//--------------------------------------------------------------------------

double SpModel::totHfb0(PhMask Phase, CSysVector * pMA)
  {
  CSysVector &MA = ((pMA==NULL) ? MArray() : *pMA);
  return msHfb0(Phase, &MA)*MA.Sum(Phase);
  };

//--------------------------------------------------------------------------

double SpModel::totH_DHfb0(PhMask Phase, CSysVector * pMA)
  {
  CSysVector &MA = ((pMA==NULL) ? MArray() : *pMA);
  return msH_DHfb0(Phase, &MA)*MA.Sum(Phase);
  }

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

#define WithSpMdlProps 0

long SpModel::DefinedPropertyCount()
  {
#if WithSpMdlProps
  return 2;
#else
  return 0;
#endif
  }

//--------------------------------------------------------------------------

long SpModel::DefinedPropertyInfo(long Index, MPropertyInfo & Info)
  {
#if WithSpMdlProps
  long Inx=Index-CPropertyBase::DefinedPropertyCount();
  switch(Inx)
    {
    case  0: Info.SetStructName("SpMdlProps");
             Info.Set(ePT_Double,    "",  "T",           DC_T,    "C",    0,      0,  MP_Null,     "A Desc"); return Inx;
    case  1: Info.Set(ePT_Double,    "",  "totCp",       DC_CpMs,  "", 1e-6,      0,  MP_HardMin,  "A Desc"); return Inx;
    }
#endif
  return CPropertyBase::DefinedPropertyInfo(Index, Info);
  }

//--------------------------------------------------------------------------

DWORD SpModel::GetPropertyVisibility(long Index)
  {
  return CPropertyBase::GetPropertyVisibility(Index);
  }

//--------------------------------------------------------------------------

void SpModel::GetPropertyValue(long Index, PhMask Phase, double T, double P, MPropertyValue & Value)
  {
#if WithSpMdlProps
  switch (Index-CPropertyBase::DefinedPropertyCount())
    {
    case   0: Value=Temp();               return;
    case   1: Value=totCp(Phase, T, P);   return;
    }
#endif
  CPropertyBase::GetPropertyValue(Index, Phase, T, P, Value);
  }

//--------------------------------------------------------------------------

void SpModel::PutPropertyValue(long Index, MPropertyValue & Value)
  {
#if WithSpMdlProps
  //switch (Index)
  //  {
  //  case   0: return Temp();
  //  case   1: return totCp();
  //  };
#endif
  CPropertyBase::PutPropertyValue(Index, Value);
  }

//--------------------------------------------------------------------------

bool SpModel::MassInUseOK()           { return m_bMassInUseOK; }
void SpModel::SetMassInUseOK(bool On) { m_bMassInUseOK=On; }

//--------------------------------------------------------------------------

bool SpModel::MassInUse(int i)
  {
  if (!m_bMassInUseOK || i<0)
     return true;
  return m_MassInUse.GetSize()==0 || m_MassInUse[i];
  };

//--------------------------------------------------------------------------

void SpModel::SetMassInUse(int i, bool InUse)
  {
  if (!m_bMassInUseOK || i<0)
     return;
  if (m_MassInUse.GetSize()==0)
    {
    if (InUse) // size == 0 implies All In Use
      return;
    m_MassInUse.SetSize(SVSpcCount());
    for (int j=0; j<SVSpcCount(); j++)
      m_MassInUse[j]=true;
    };
  m_MassInUse[i]=InUse;
  };

//--------------------------------------------------------------------------

void SpModel::SetMassInUse(bool InUse)
  {
  if (!m_bMassInUseOK)
     return;
  if (m_MassInUse.GetSize()==0)
    {
    if (InUse) // size == 0 implies All In Use
      return;
    m_MassInUse.SetSize(SVSpcCount());
    }
  for (int j=0; j<SVSpcCount(); j++)
    m_MassInUse[j]=InUse;
  };

//--------------------------------------------------------------------------

void SpModel::ClrMassInUse()
  {
  if (!m_bMassInUseOK)
   return;
  m_MassInUse.SetSize(0);
  };

// --------------------------------------------------------------------------

void SpModel::InitialiseHumidity()
  {
  //m_AutoVLE.m_iVap=SDB.Find("H2O(g)");
  //m_AutoVLE.m_iLiq=SDB.Find("H2O(l)");
  //if (m_AutoVLE.m_iVap<0 || m_AutoVLE.m_iLiq<0)
  //  {
  //  m_AutoVLE.m_iVap=-1;
  //  m_AutoVLE.m_iLiq=-1;
  //  }
  };

// --------------------------------------------------------------------------

double SpModel::SpecificHumidity()
  {
  if (AutoVLEOn())
    {
    double P=Press();
    double Pw=PartialPress(m_AutoVLE.m_iVap, -1, Temp());
    double MWw=SDB[m_AutoVLE.m_iVap].MoleWt();
    double MSw=VValue[m_AutoVLE.m_iVap];
    double MWo=0;
    double MSo=0;
    SpecieIter I(som_Gas);
    for (int i=-1; I.Loop(i); )
      {
      if (i!=m_AutoVLE.m_iVap)
        {
        MSo+=VValue[i];
        MWo+=VValue[i]*SDB[i].MoleWt();
        }
      }
    MWo=MWo/GTZ(MSo);
    return MWw*Pw/GTZ(MWo*(P-Pw));
    }
  return dNAN;
  }

// --------------------------------------------------------------------------

double SpModel::RelativeHumidity()
  {
  if (AutoVLEOn())
    {
    double Pw=PartialPress(m_AutoVLE.m_iVap, -1, Temp());
    double PSat=SDB[m_AutoVLE.m_iVap].VapourP(m_iFidelity, Temp());
    return Pw/GTZ(PSat);
    }
  return dNAN;
  }

// --------------------------------------------------------------------------

double SpModel::HumidityDewPt()
  {
  if (AutoVLEOn())
    {
    double Pw=PartialPress(m_AutoVLE.m_iVap, -1, Temp());
    return SDB[m_AutoVLE.m_iVap].VapourT(m_iFidelity, Pw);//ress());
    }
  return dNAN;
  }

// ==========================================================================
//
//                              SpModelEx
//
// ==========================================================================

SpModelEx::SpModelEx(TagObjClass* pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach) :
SpModel(pClass_, Tag_, pAttach, eAttach)
  {
  };

//===========================================================================

SpModelEx::~SpModelEx()
  {
  };

double SpModelEx::msHs(PhMask Phase, double T_, double P_, CSysVector * pMA, double *pTotalM)
  {
  double H1=msHm(Phase, T_, P_, pMA);
  double H2=msHm(Phase, ZeroCinK, Std_P, pMA, pTotalM);

  //dbgpln("Ex::msHs %10.3f %10.3f %15.6f %15.6f %15.6f", T_, P_, H1-H2, H1, H2);
  return H1-H2;
  };
double SpModelEx::msHz(PhMask Phase, double T_, double P_, CSysVector * pMA, double *pTotalM)
  {
  double H=msHm(Phase, T_, P_, pMA, pTotalM);

  //dbgpln("Ex::msHz %10.3f %10.3f %15.6f", T_, P_, H);
  return H;
  }

double SpModelEx::msHf(PhMask Phase, double T_, double P_, CSysVector * pMA, double *pTotalM)
  {
  if (SpModelExDbgOn)
    dbgindent(2);

  CSysVector &MA = ((pMA==NULL) ? MArray() : *pMA);

  double H2=msHm(Phase, T_, P_, &MA, pTotalM);
  double H1=0;//msHm(Phase, ZeroCinK, StdP, &MA, pTotalM);

  double Ht=0, Mt=0;
  if (1)
    {
    SpecieIter I(Phase);
    for (int i=-1; I.Loop(i); )
      {
      if ((MA[i] > MeasTolerance) && SDB[i].CpDirect())
        {
        Ht+=MA[i]*SDB[i].msHfBase0(m_iFidelity);
        Mt+=MA[i];
        if (0 && SpModelExDbgOn)
          {
          double h=MA[i]*SDB[i].msHfBase0(m_iFidelity);
          dbgpln("              %i %3i M:%7.2f %7.2f H:%12.3f %12.3f HfBase0:%12.3f %s",
            m_iFidelity, i, MA[i], Mt, h, Ht, SDB[i].msHfBase0(m_iFidelity),SDB[i].SymOrTag());
          }
        }
      }
    }

  if (SpModelExDbgOn)
    {
    dbgindent(-2);
    dbgpln("SpModelEx::msHf %7.2f %7.2f M:%12.3f H:%12.3f Ht:%12.3f Mt:%12.3f =%12.3f",
            T_, P_, MA.Mass(), H2-H1, Ht, Mt, H2-H1+Ht/GTZ(Mt));
    }
  return H2-H1+Ht/GTZ(Mt);
  };
double SpModelEx::msSf(PhMask Phase, double T_, double P_, CSysVector * pMA, double *pTotalM)
  {
  return SpModel::msSf(Phase, T_, P_, pMA, pTotalM);
  };
double SpModelEx::totCp(PhMask Phase, double T_, double P_, CSysVector * pMA)
  {
  CSysVector &MA = ((pMA==NULL) ? MArray() : *pMA);
  return msCp(Phase, T_, P_, pMA) * MA.Sum(Phase);
  }
double SpModelEx::totHs(PhMask Phase, double T_, double P_, CSysVector * pMA)
  {
  CSysVector &MA = ((pMA==NULL) ? MArray() : *pMA);
  double Ret=msHs(Phase, T_, P_, pMA) * MA.Sum(Phase);
  //dbgpln("Ex::totHs     @ %8.2f %8.2f = %15.6f", T_, P_, Ret);
  return Ret;
  };
double SpModelEx::totHz(PhMask Phase, double T_, double P_, CSysVector * pMA)
  {
  CSysVector &MA = ((pMA==NULL) ? MArray() : *pMA);
  double Ret=msHz(Phase, T_, P_, pMA) * MA.Sum(Phase);
  //dbgpln("Ex::totHz     @ %8.2f %8.2f = %15.6f", T_, P_, Ret);
  return Ret;
  };
double SpModelEx::totHf(PhMask Phase, double T_, double P_, CSysVector * pMA)
  {
  CSysVector &MA = ((pMA==NULL) ? MArray() : *pMA);
  double Ret=msHf(Phase, T_, P_, pMA) * MA.Sum(Phase);
  //dbgpln("Ex::totHf     @ %8.2f %8.2f = %15.6f  %8.3f%% %8.3f%% %8.3f%%", T_, P_, Ret, MA.MassFrac(som_Sol)*100, MA.MassFrac(som_Liq)*100, MA.MassFrac(som_Gas)*100);
  return Ret;
  };
double SpModelEx::totSf(PhMask Phase, double T_, double P_, CSysVector * pMA)
  {
  return SpModel::totSf(Phase, T_, P_, pMA);
  };

// ==========================================================================
//
//                              SpModelOwner
//
// ==========================================================================

Strng SpModelOwner::DefaultShape="VertCyl";
Strng SpModelOwner::DefaultModel=BaseSpModelName;

SpModelOwner::SpModelOwner(TagObjClass* pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach, flag HasShape) :
TaggedObject(pClass_, Tag_, pAttach, eAttach)
  {
  fPermModel=false;
  m_fModelSelectable=true;
  pModel=NULL;
  }

//---------------------------------------------------------------------------

SpModelOwner::~SpModelOwner()
  {
  }

// --------------------------------------------------------------------------

void SpModelOwner::AddShapeName(DataDefnBlk &DDB)
  {
  DDBValueLstMem DDB0;
  TagObjClass::GetSDescValueLst(SpShape::GroupName, DDB0);
  DDB.String("Shape",      "",        DC_,    "",      xidSpShape,  this, DDB.ForDisplayOnly() ? 0 : isParmStopped|SetOnChange, DDB0());
  }

// --------------------------------------------------------------------------

#define INHERITSTR "Inherit"

void SpModelOwner::AddModelName(DataDefnBlk &DDB)
  {
  DDBValueLstMem DDB1;
  TagObjClass::GetSDescValueLst(SpModel::GroupName, DDB1, INHERITSTR);
  //gs_SpModelCOMClasses.AppendProgIDs(DDB1);
  DDB.String("Model",         "",        DC_,    "",      xidSpModel, this, 
         !m_fModelSelectable || fPermModel ? 0 : (isParm|SetOnChange), DDB1());
  DDB.TagComment(sReqdModel() ? "Reqd" : "Heritage");
  DDB.String("Class",         "",        DC_,    "",      xidSpModelClass, this, InitHidden);
  }

// --------------------------------------------------------------------------

void SpModelOwner::AddFidelity(DataDefnBlk &DDB, bool Editable)
  {
  static DDBValueLst DDB1[]=
    {
      {0,  "Low"},
      {1,  "High"},
      {0}
    };
  DDB.Long  ("Fidelity",        "",   DC_,    "",      xidSpMdlFidelity, this, Editable ? (isParmStopped|SetOnChange):0, DDB1);
  }

// --------------------------------------------------------------------------

void SpModelOwner::AddFnRanges(DataDefnBlk &DDB)
  {
#if WITHSPMDLRANGES
  if (DDB.BeginStruct(this, "FnRange", 0, DDB_NoPage))
    {
    DDB.String("Errors",   "",   DC_,    "",      xidSMFnRanges,    this, isResult);
    DDB.Long  ("ErrorID",  "",   DC_,    "",      xidSMFnRangeID,   this, isResult);
    }
  DDB.EndStruct();
#endif
  }

// --------------------------------------------------------------------------

flag SpModelOwner::DataXchg(DataChangeBlk & DCB)
  {
  if (TaggedObject::DataXchg(DCB))
    return 1;
  switch (DCB.lHandle)
    {
    case xidSpModel:
      {
      if (DCB.rpC)
        {
        if (DCB.ForFiling() && (PrjFileVerNo()<27))
          {
          sReqdModel="";
#if WithSIMPLESPMDL
          Strng Mdl=DCB.rpC;
          CheckSpMdlName(Mdl);
          ChangeModel(Mdl());
#else
          ChangeModel(DCB.rpC);
#endif
          }
        else
          {
          Strng Mdl;
          if (m_fModelSelectable)
            Mdl=DCB.rpC;
          if (Mdl[0]=='(')
            {
            sReqdModel="";
            Mdl.LRTrim("()");
#if WithSIMPLESPMDL
            CheckSpMdlName(Mdl);
#endif
            }
          else if (Mdl.XStrICmp(INHERITSTR)==0)
            {
            sReqdModel="";
            Mdl="";
            }
          else 
            {
#if WithSIMPLESPMDL
            CheckSpMdlName(Mdl);
            sReqdModel=Mdl;
#else
            sReqdModel=DCB.rpC;
#endif
            }
          ChangeModel(Mdl());
          }
        }

      static Strng ActMdl;
      ActMdl.Set(sReqdModel() ? "%s" : "(%s)", pModel->ShortDesc());
      DCB.pC=ActMdl();
      return 1;
      }
    case xidSpModelClass:
      {
      DCB.pC=pModel->ClassId();
      return 1;
      }
    case xidSpMdlFidelity:
      if (DCB.rL)
        SpModel::SetFidelity(*DCB.rL);
      DCB.L=SpModel::Fidelity();
      return 1;
    case xidSMFnRanges:
      {
      m_sSMFnRanges="";
#if WITHSPMDLRANGES
      MSMFnRanges * p=pModel->SMFnRanges();
      if (p)
        {
        long Bad=(~p->m_Flags) & p->m_Masks;
        for (int i=0; i<(long)eFn_Last; i++)
          {
          if ((Bad & 0x00000001)!=0)
            {
            if (m_sSMFnRanges.GetLength()>0)
              m_sSMFnRanges+=":";
            m_sSMFnRanges+=eSpFnsNames[i];
            }
          Bad = Bad >>1;
          }
        }
      //if (m_sSMFnRanges.GetLength()==0)
      //  m_sSMFnRanges="OK";
#endif
      DCB.pC=m_sSMFnRanges();
      return 1;
      }
    case xidSMFnRangeID:
      {
#if WITHSPMDLRANGES
      MSMFnRanges * p=pModel->SMFnRanges();
      DCB.L=p ? ((~p->m_Flags) & p->m_Masks) : 0;
#else
      DCB.L=0;
#endif
      return 1;
      }
    }
  return 0;
  };

// --------------------------------------------------------------------------

void SpModelOwner::SelectModel(SpModel * &pModel, SpModel * pOther, flag ForceIt)
  {
  if (ForceIt)
    {
    ChangeModel(pModel, pOther->Class());
    }
  else
    {
    SpModel * pChgTo=NULL;
    SpModel * pPref=pModel->PreferredModel(pOther);
    if (pModel->IsBaseClassOf(pOther))
      pChgTo=pOther;
    else if (pPref)
      pChgTo=pPref;

    if (pChgTo)
      ChangeModel(pModel, pChgTo->Class());

    }
  };

// --------------------------------------------------------------------------

void CSpMdlSlct::Add(SpModel * pMdl, double Qm)
  {
  Qm=fabs(Qm);
  iQMax=-1;
  if (N>0)
    {
    if (SpModel::iSpMdlMixStrategy==SPMMS_PreBld35)
      {
      for (int i=0; i<N; i++)
        {
        SpModel * pPref=Mdls[i]->PreferredModel(pMdl);
        if (Mdls[i]->IsBaseClassOf(pMdl))
          Mdls[i]=pMdl;
        else if (pPref)
          Mdls[i]=pPref;
        else
          continue;
        Qms[i]+=Qm;
        break;
        }
      if (i>=N)
        {
        Mdls[N]=pMdl;
        Qms[N]=Qm;
        N++;
        }
      }
    else
      {
      for (int i=0; i<N; i++)
        {
        SpModel * pPref=Mdls[i]->PreferredModel(pMdl);
        if (pMdl->IsBaseClassOf(Mdls[i]))
          Mdls[i]=Mdls[i];
        else if (Mdls[i]->IsBaseClassOf(pMdl))
          Mdls[i]=pMdl;
        else if (pPref)
          Mdls[i]=pPref;
        else
          continue;
        Qms[i]+=Qm;
        break;
        }
      if (i>=N)
        {
        Mdls[N]=pMdl;
        Qms[N]=Qm;
        N++;
        }
      }
    }
  else
    {
    Mdls[N]=pMdl;
    Qms[N]=Qm;
    N++;
    }
  };

// --------------------------------------------------------------------------

TagObjClass* CSpMdlSlct::RequiredMdlClass()
  {
  RequiredMdl();
  if (iQMax>=0)
    return Mdls[iQMax]->Class();
  return NULL;
  }

// --------------------------------------------------------------------------

SpModel * CSpMdlSlct::RequiredMdl()
  {
  if (iQMax<0)
    {
    double QMax=-1.0;
    for (int i=0; i<N; i++)
      if (Qms[i]>QMax)
        {
        QMax=Qms[i];
        iQMax=i;
        }
    }
  if (iQMax>=0)
    return Mdls[iQMax];
  return NULL;
  }

// --------------------------------------------------------------------------

void SpModelOwner::SelectModel(SpModel * &pModel, CSpMdlSlct & Slct)
  {
  TagObjClass* pC=Slct.RequiredMdlClass();
  if (pC)
    ChangeModel(pModel, pC);
  }

// --------------------------------------------------------------------------

void SpModelOwner::ChangeShape(pSpShape &pShape, pchar ReqdShapeDesc)
  {
  TagObjClass* pOldClass=pShape ? pShape->Class() : NULL;
  pchar ReqdShape=NULL;
  if (ReqdShapeDesc && _stricmp(ReqdShapeDesc, pShape->ShortDesc())!=0)
    {
    Strng S;
    for (int i=0; ;i++)
      {
      TagObjClass* pC=TagObjClass::FindClassIndexed(SpShape::GroupName, i);
      if (pC==NULL)
        break;
      else
        {
        pC->GetShortDesc(S);
        if (S.XStrICmp(ReqdShapeDesc)==0)
          {
          ReqdShape=pC->ClassId();
          break;
          }
        }
      }
    if (ReqdShape)
      {
      pSpShape pNewShape=(pSpShape)SpShapeClass.ConstructGroup(ReqdShape, NULL, pShape->Tag(), this, TOA_Embedded);
      if (pNewShape)
        {
        pNewShape->CopyOnChange(pShape);
        //pNewShape->SetActualVolume(pShape->ActualVolume());
        //pNewShape->SetVapVolScale(pShape->VapVolScale());
        //pNewShape->SetHeight(pShape->Height());
        pShape->Destroy();
        pShape=pNewShape;
        StructureChanged(this);
        pShape->StructureChanged(this);
        }
      }
    else
      LogError("SpModel", 0, "Unknown Container Shape %s", ReqdShapeDesc);
    }

  //return pOldClass;
  };

// --------------------------------------------------------------------------

void SpModelOwner::ChangeShape(pSpShape &pShape, TagObjClass* pRqdShapeClass_)
  {

  TagObjClass* pOldClass=pShape ? pShape->Class() : NULL;
  if (pRqdShapeClass_ && pRqdShapeClass_!=pShape->Class())
    {
    pSpShape pNewShape=(pSpShape)pRqdShapeClass_->Construct(NULL, pShape->Tag(), this, TOA_Embedded);
    if (pNewShape)
      {
      pNewShape->CopyOnChange(pShape);
      //pNewShape->SetActualVolume(pShape->ActualVolume());
      //pNewShape->SetVapVolScale(pShape->VapVolScale());
      //pNewShape->SetHeight(pShape->Height());
      pShape->Destroy();
      pShape=pNewShape;
      StructureChanged(this);
      pShape->StructureChanged(this);
      }
    }

  //return pOldClass;
  };

// --------------------------------------------------------------------------

void SpModelOwner::ChangeModel(SpModel * &pModel, pchar ReqdModelDesc, bool UseAsClassid)
  {
  if (sReqdModel())
    ReqdModelDesc=sReqdModel();

  TagObjClass* pOldClass=pModel ? pModel->Class() : NULL;
  pchar ReqdModel=NULL;
  if (fPermModel && pOldClass!=NULL)
    return ;//pOldClass;
  if (ReqdModelDesc && _stricmp(ReqdModelDesc, UseAsClassid?pModel->ClassId():pModel->ShortDesc())!=0)
    {
    //    gs_SpModelCOMClasses.FindClass(


    Strng S;
    for (int i=0; ;i++)
      {
      TagObjClass* pC=TagObjClass::FindClassIndexed(SpModel::GroupName, i);
      if (pC==NULL)
        break;
      else
        {
        if (UseAsClassid)
          S=pC->ClassId();
        else
          pC->GetShortDesc(S);
        if (S.XStrICmp(ReqdModelDesc)==0)
          {
          ReqdModel=pC->ClassId();
          break;
          }
        }
      }

    SpModel * pNewModel=NULL;
    if (ReqdModel)
      {
      pNewModel=(SpModel *)SpModelClass.ConstructGroup(ReqdModel, NULL, pModel->Tag(), this, TOA_Embedded);
      }
    //    else
    //      {
    //      pNewModel=(SpModel *)gs_SpModelCOMClasses.Construct(ReqdModelDesc, pModel->Tag(), this, TOA_Embedded);
    //      }
    if (pNewModel)
      {
      double CurTemp=pModel->Temp();
      double CurPress=pModel->Press();
      pNewModel->SetMixed(pModel->Mixed());
      pNewModel->SetClosed(pModel->Closed());
      pNewModel->SetUsage(pModel->Usage());
      //        pNewModel->SetSQEditable(pModel->SQEditable());
      pNewModel->bSQFlags=pModel->bSQFlags;
      pNewModel->SetView(pModel->GetView());
      pNewModel->m_pGlblView=pModel->m_pGlblView;
      pNewModel->SetDataSign(pModel->DataSign());
      pNewModel->SetNegDataOK(pModel->NegDataOK());

      pNewModel->ExchangeSpecies(pModel);
      pNewModel->SetTempPress(CurTemp, CurPress);

      pModel->Destroy();
      pModel=pNewModel;
      StructureChanged(this);
      pModel->StructureChanged(this);
      pModel->ClrStatesOK();
      }
    else
      LogError("SpModel", 0, "Unknown Specie Model '%s'", ReqdModelDesc);
    }

  //return pOldClass;//pModel;
  };

// --------------------------------------------------------------------------

void SpModelOwner::ChangeModel(SpModel * &pModel, TagObjClass* pRqdModelClass_)//TagObjClass* pRqdModelClass_)
  {
  if (sReqdModel())
    {
    ChangeModel(pModel, (char*)NULL);
    //DoBreak();
    //ReqdModelDesc=sReqdModel();
    return;
    }
  TagObjClass* pOldClass=pModel ? pModel->Class() : NULL;
  if (fPermModel && pOldClass!=NULL)
    return ;//pOldClass;
  if (pRqdModelClass_ && pRqdModelClass_!=pModel->Class())
    {
    SpModel * pNewModel=(SpModel *)pRqdModelClass_->Construct(NULL, pModel->Tag(), this, TOA_Embedded);
    if (pNewModel)
      {
      double CurTemp=pModel->Temp();
      double CurPress=pModel->Press();

      pNewModel->SetMixed(pModel->Mixed());
      pNewModel->SetClosed(pModel->Closed());
      pNewModel->SetUsage(pModel->Usage());
      pNewModel->bSQFlags=pModel->bSQFlags;
      //pNewModel->SetSQEditable(pModel->SQEditable());
      pNewModel->SetView(pModel->GetView());
      pNewModel->m_pGlblView=pModel->m_pGlblView;
      pNewModel->SetDataSign(pModel->DataSign());
      pNewModel->SetNegDataOK(pModel->NegDataOK());

      pNewModel->ExchangeSpecies(pModel);
      pNewModel->SetTempPress(CurTemp, CurPress);

      pModel->Destroy();
      pModel=pNewModel;
      StructureChanged(this);
      pModel->StructureChanged(this);
      pModel->ClrStatesOK();
      }
    else
      LogError("SpModel", 0, "Construction Failed '%s'",pRqdModelClass_->ClassId());
    }
  //return pOldClass;
  };

// --------------------------------------------------------------------------

void SpModelOwner::CopyQualities(SpModel * pModel, SpModel * SrcModel, int q0, int q1)
  {
  pModel->CopyQualities(SrcModel, q0, q1);
  };

// --------------------------------------------------------------------------
#ifndef _RELEASE
static int DoSpModelSanityCheck = 1;
void SpModelOwner::SanityCheck()
  {
  if (DoSpModelSanityCheck)
    {
    if (pModel)
      {
      for (int sn=0; sn<SVSpcCount(); sn++)
        {
        double mm=MArray()[sn];
        if (mm<0.0)
          {
          Strng ss = SVImg(sn).SymOrTag();
          ASSERT_ALWAYS(FALSE, "Unexpected -ve !", __FILE__, __LINE__);
          }
        }
      }
    //Perhaps put some other checks here?
    //ASSERT_ALWAYS(???, "??? !", __FILE__, __LINE__);
    }
  }
#endif

//===========================================================================
//
//
//
//===========================================================================
