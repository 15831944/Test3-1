//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include "sc_defs.h"
#include "sp_cont.h"
#define  __VLEBASE_CPP
#include "vlebase.h"

#include "dbgmngr.h"

//#include "optoff.h"

#define dbgVLEBlock       WITHDEBUG

#if dbgVLEBlock
static CDbgMngr dbgQVFlash       ("VLEMdl",  "VFlash");
static CDbgMngr dbgQVFlashDbg    ("VLEMdl",  "QVFlashDbg");
static CDbgMngr dbgQVFlashChg    ("VLEMdl",  "QVFlashChg");
static CDbgMngr dbgQVFlashChgDbg ("VLEMdl",  "QVFlashChgDbg");
static CDbgMngr dbgAddQVDerivs   ("VLEMdl",  "AddQVDerivs");
#endif

#define DoDbg  (0 || WITHDEBUG)
#define DoDbg1 (0 || WITHDEBUG)
#define DoDbg2 (0 || WITHDEBUG)

//============================================================================
//
//
//
//============================================================================

static DDBValueLst DDBVLEStates[]=
  {
    {VLES_CfgError,       "Configuration Error"},
    {VLES_OK,             "OK"},
    {VLES_Nothing,        "Nothing to Operate On"},
    {VLES_SubCooled,      "SubCooled"},
    {VLES_BubblePt,       "BubblePt"},
    {VLES_TwoPhase,       "TwoPhase"},
    {VLES_DewPt,          "DewPt"},
    {VLES_SuperHeated,    "SuperHeated"},
    {VLES_NotConverged,   "Not_Converged"},
    {0}
  };

//============================================================================
//
//
//
//============================================================================

const pchar CVLEBlock::GroupName="CVLEBlock";

//IMPLEMENT_TAGOBJ(CVLEBlock, CVLEBlock::GroupName, "CVLEBlock", "VLEOff", "", "Off", "Off");
CVLEBlock::CVLEBlock(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
TaggedObject(pClass_, TagIn, pAttach, eAttach),
Nd(*((FlwNode*)pAttach))//
  {
  VMixFrac.SetSize(SVSpcCount());
  LMixFrac.SetSize(SVSpcCount());
  VMixFrac.SetAll(1.0);
  LMixFrac.SetAll(1.0);
  };

//--------------------------------------------------------------------------

CVLEBlock::~CVLEBlock()
  {
  for (int i=0; i<CICount(); i++)
    ClrCI(i);
  };

//--------------------------------------------------------------------------

void CVLEBlock::SetMixFrac(SpMArray *pVMixFrac, SpMArray *pLMixFrac)
  {
  if (pVMixFrac)
    {
    for (int s=0; s<SVSpcCount(); s++)
      VMixFrac[s]=(*pVMixFrac)[s];
    }
  else
    VMixFrac.SetAll(1.0);
  if (pLMixFrac)
    {
    for (int s=0; s<SVSpcCount(); s++)
      LMixFrac[s]=(*pLMixFrac)[s];
    }
  else
    LMixFrac.SetAll(1.0);
  };

//--------------------------------------------------------------------------

void CVLEBlock::SetMixFrac(double dVMixFrac, double dLMixFrac)
  {
  for (int s=0; s<SVSpcCount(); s++)
    {
    VMixFrac[s]=dVMixFrac;
    LMixFrac[s]=dLMixFrac;
    }
  };

//--------------------------------------------------------------------------

void CVLEBlock::SetMixFrac(int CompNo, double dVMixFrac, double dLMixFrac)
  {
  if (CDB[CompNo].VapPhInx("g", true)>=0)
    VMixFrac[CDB[CompNo].VapPhInx("g", true)]=dVMixFrac;
  if (CDB[CompNo].LiqPhInx("l", true)>=0)
    LMixFrac[CDB[CompNo].LiqPhInx("l", true)]=dLMixFrac;
  };

//============================================================================
//
//
//
//============================================================================

#if WITHVLETIMERS 
CGlblStopWatch CVLEBase::sm_swTPFlash(gs_swFlwSolve,     "VLE", "TPFlash");
CGlblStopWatch CVLEBase::sm_swQPFlash(gs_swFlwSolve,     "VLE", "PFlash");
CGlblStopWatch CVLEBase::sm_swQVFlash(gs_swFlwSolve,     "VLE", "VFlash");
CGlblStopWatch CVLEBase::sm_swQVDerivs(gs_swFlwSolve,    "VLE", "QVDerivs");
CGlblStopWatch CVLEBase::sm_swQMVapFlash(gs_swFlwSolve,  "VLE", "MVapFlash");
#endif

CVLEBase::CVLEBase(TaggedObject * pAttach, dword SelectMask, LPTSTR Name) : 
CBlockEvalBase(BEId_VLE,0,Name) 
  { 
  m_pVLE            = NULL; 
  m_pNd             = pAttach; 
  m_fEnabled        = false;
  m_fFixed          = false; 
  m_dwSelectMask    = SelectMask; 

  m_iFlags          = 0;

  m_TempIn          = Std_T;
  m_TotalPIn        = Std_P;
  m_PartialPIn      = 0.0;
  m_VapQmIn         = 0.0;
  m_VapFracIn       = 0.0;
  m_TotQmIn         = 0.0;
  m_FlshVapQmIn     = 0.0;
  m_FlshVapFracIn   = 0.0;

  m_TempOut         = Std_T;
  m_TotalPOut       = Std_P;
  m_PartialPOut     = 0.0;
  m_VapQmOut        = 0.0;
  m_VapFracOut      = 0.0;
  m_TotQmOut        = 0.0;
  m_FlshVapQmOut    = 0.0;
  m_FlshVapFracOut  = 0.0;

  m_BPE             = 0.0;
  m_sSpModel        = "Unknown";
  m_sSatMethod      = "Unknown";

  };

CVLEBase::~CVLEBase() 
  { 
  delete m_pVLE; 
  };


flag CVLEBase::Open(TagObjClass * pVLEClass, flag Fixed)
  {
  //dbgpln("CVLEBase::Open %s", m_pNd->FullObjTag());

  m_fEnabled=true;

  if (m_pVLE && (pVLEClass==NULL || m_pVLE->Class()==pVLEClass))
    return true;

  delete m_pVLE;
  m_fFixed=Fixed;

  if (pVLEClass)
    m_pVLE=(CVLEBlock*)pVLEClass->Construct(NULL, "VLE", m_pNd, TOA_Embedded);
  else
    m_pVLE=(CVLEBlock*)CVLEVapPressClass.Construct(NULL, "VLE", m_pNd, TOA_Embedded);
  //pVLE=(CVLEBlock*)VLEIdealKClass.Construct(NULL, "VLE", pNd, TOA_Embedded);
  m_pNd->StructureChanged(NULL);
  m_pVLE->m_pVLEBase=this;

  m_fEnabled = (m_pVLE!=NULL);
  return m_fEnabled;
  };

//--------------------------------------------------------------------------

void CVLEBase::Close()
  {
  //dbgpln("CVLEBase::Close %s", pNd->FullObjTag());
  if (m_fFixed)
    return;
  m_fEnabled=false;
  // ?? Why were these two line commented out
  delete m_pVLE;
  m_pVLE=NULL;
  };

//--------------------------------------------------------------------------

void CVLEBase::Add_OnOff(DataDefnBlk &DDB, dword Flags)
  {
  DDB.Byte("VLEquilibrium",      "",       DC_    , "",      xidVLEEnable, m_pNd,  (m_fFixed ? 0 : Flags), GetOnOffValLst());
  }

//--------------------------------------------------------------------------

void CVLEBase::BuildDataDefn(DataDefnBlk &DDB, pchar pTag_, pchar pTagComment, DDBPages PageIs, dword UserInfo)
  {
  if (pTag_==NULL)
    pTag_="VLE";
  DDEF_Flags Old=DDB.GetVisibility();
  DDB.Visibility(NSHM_All, m_fEnabled);

  //if (DDB.BeginObject(m_pNd, pTag_, "VLEBlk", pTagComment, PageIs))
  if (DDB.BeginObject(m_pNd, pTag_, "VLE_Cfg", pTagComment, PageIs))
    {
    DDBValueLstMem DDB0;
    TagObjClass::GetSDescValueLst(CVLEBlock::GroupName, DDB0, NULL, m_dwSelectMask);
    DDB.String  ("Model",      "",       DC_    , "",      xidVLEEqnNm  , m_pNd,m_fFixed ? 0 : isParm|SetOnChange, DDB0());
    if (m_pVLE)
      {
      DDB.SuppressNextBlock();
      m_pVLE->BuildDataDefn(DDB);
      }
    }                                                           
  DDB.Text(" ");
  if ((m_iFlags & VLEF_HideFeedMeasure)==0)
    {
    DDB.Double("Feed.Temp",               "Feed.T", DC_T,      "C",     &m_TempIn,          m_pNd, isResult);
    DDB.Double("Feed.TotalP",                   "", DC_P,      "kPag",  &m_TotalPIn,        m_pNd, isResult);
    DDB.Double("Feed.PartialP",                 "", DC_P,      "kPag",  &m_PartialPIn,      m_pNd, isResult);
    DDB.Double("Feed.TotalMassFlow",     "Feed.Qm", DC_Qm,     "kg/s",  &m_TotQmIn,         m_pNd, isResult);
    DDB.Double("Feed.VapourMassFlow",  "Feed.VQMt", DC_Qm,     "kg/s",  &m_VapQmIn,         m_pNd, isResult);
    DDB.Double("Feed.VapourFrac",        "Feed.Vf", DC_Frac,   "%",     &m_VapFracIn,       m_pNd, isResult);
    DDB.Double("Feed.FlashVapFlow",             "", DC_Qm,     "kg/s",  &m_FlshVapQmIn,     m_pNd, isResult);
    DDB.Double("Feed.FlashVapFrac",             "", DC_Frac,   "%",     &m_FlshVapFracIn,   m_pNd, isResult);
    DDB.Text(" ");
    }

  if ((m_iFlags & VLEF_HideChanges)==0)
    {
    DDB.Double("VapFlowFlashed",          "", DC_Qm,     "kg/s",  xidVLENettVapQm,    m_pNd, isResult);
    }
  
  if (1)
    {
    DDB.Double("BoilingPointElevation",   "", DC_dT,     "C",     &m_BPE,             m_pNd, isResult);
    DDB.String("LiquorModel",             "", DC_,       "",      &m_sSpModel,        m_pNd, isResult);
    DDB.String("SatMethodUsed",           "", DC_,       "",      &m_sSatMethod,      m_pNd, isResult);
    DDB.Text(" ");
    }

  if ((m_iFlags & VLEF_HideProdMeasure)==0)
    {
    DDB.Double("Prod.Temp",               "Prod.T", DC_T,      "C",     &m_TempOut,         m_pNd, isResult);
    DDB.Double("Prod.TotalP",                   "", DC_P,      "kPag",  &m_TotalPOut,       m_pNd, isResult);
    DDB.Double("Prod.PartialP",                 "", DC_P,      "kPag",  &m_PartialPOut,     m_pNd, isResult);
    DDB.Double("Prod.TotalMassFlow",     "Prod.Qm", DC_Qm,     "kg/s",  &m_TotQmOut,        m_pNd, isResult);
    DDB.Double("Prod.VapourMassFlow",  "Prod.VQMt", DC_Qm,     "kg/s",  &m_VapQmOut,        m_pNd, isResult);
    DDB.Double("Prod.VapourFrac",        "Prod.Vf", DC_Frac,   "%",     &m_VapFracOut,      m_pNd, isResult);
    DDB.Double("Prod.FlashVapFlow",             "", DC_Qm,     "kg/s",  &m_FlshVapQmOut,    m_pNd, isResult);
    DDB.Double("Prod.FlashVapFrac",             "", DC_Frac,   "%",     &m_FlshVapFracOut,  m_pNd, isResult);
    DDB.Text(" ");
    }

  DDB.EndObject();

  DDB.SetVisibility(Old);
  }

//--------------------------------------------------------------------------

flag CVLEBase::DataXchg(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
    case xidVLEEnable: 
      if (DCB.rB)
        {
        if (*DCB.rB)
          Open(*DCB.rB);
        else
          Close();

        }
      DCB.B=OpenStatus();// (Enabled()); 
      return 1;
    case xidVLEEqnNm: 
      {
      if (DCB.rpC && !m_fFixed)
        {
        int WasEnabled=m_fEnabled;
        TagObjClass * pC=CVLEIdealKClass.FindGrpShortDesc(DCB.rpC);
        if (pC)
          {
          Open(pC);
          if (!WasEnabled)
            Close();
          }
        else
          {
          Close();
          }
        }
      DCB.pC=m_pVLE ? m_pVLE->ShortDesc() : "Off"; 
      return 1;
      }
    case xidVLENettVapQm:
      DCB.D = m_VapQmOut-m_VapQmIn;
      return 1;
    }

  // Actual Object will be DataXchd via list
  //if (pVLE && m_pVLE->DataXchg(DCB))
  //  return 1;

  return 0;//return pGSM ? pGSM->DataXchg(DCB) : 0; 
  }

//--------------------------------------------------------------------------

flag CVLEBase::ValidateData(ValidateDataBlk & VDB)
  {
  return true;
  };

//--------------------------------------------------------------------------

CSaturationDefn CVLEBase::FlashDefn()         { return Enabled() ? m_pVLE->FlashDefn()     : CSaturationDefn(SMFM_Default, -1); };
int CVLEBase::FlashMethod()                   { return Enabled() ? m_pVLE->FlashMethod()   : SMFM_Default; };
int CVLEBase::FlashCmpIndex()                 { return Enabled() ? m_pVLE->FlashCmpIndex() : -1; };
int CVLEBase::FlashLiqIndex()                 { return Enabled() ? m_pVLE->FlashLiqIndex() : -1; };
int CVLEBase::FlashVapIndex()                 { return Enabled() ? m_pVLE->FlashVapIndex() : -1; };
LPTSTR CVLEBase::FlashDescription()           { return Enabled() ? m_pVLE->FlashDescription() : ""; };
double CVLEBase::FlashVapFrac(SpModel &Mdl)   { return Enabled() ? m_pVLE->FlashVapFrac(Mdl) : 0.0; };

//--------------------------------------------------------------------------

flag CVLEBase::HasTPEnvelope()                { return Enabled() ? m_pVLE->HasTPEnvelope() : false; };

//--------------------------------------------------------------------------

void CVLEBase::SetFlashVapFrac(SpModel &Mdl, double Temp, double Press, double VapFrac, dword Flags)
  {
  if (Enabled()) 
    {
#if DoDbg
    double T0=Mdl.Temp();
    double P0=Mdl.Press();
    double MV0=Mdl.Mass(som_Gas);
    double MSL0=Mdl.Mass(som_SL);
#endif

    m_pVLE->SetFlashVapFrac(Mdl, Temp, Press, VapFrac, Flags);
    Mdl.SetFlashDescription(FlashDescription());
    m_sSatMethod = FlashDescription();

#if DoDbg
    double T1=Mdl.Temp();
    double P1=Mdl.Press();
    double MV1=Mdl.Mass(som_Gas);
    double MSL1=Mdl.Mass(som_SL);
    if (Mdl.pAttachedTo && Mdl.pAttachedTo->DoDbgBrk())
      {
      dbgpln("SetFlashVapFrac:%s",Mdl.FullObjTag());
      dbgpln("  T:%+12.4g %12.6f > %12.6f", T1-T0,T0,T1);
      dbgpln("  P:%+12.4g %12.6f > %12.6f", P1-P0,P0,P1);
      dbgpln("  M:%+12.4g %12.6f > %12.6f %12.6f>%12.6f", (MV1-MV0)+(MSL1-MSL0),MV0,MV1,MSL0,MSL1);
      }
    int xxx=0;
#endif
    }
  };

//--------------------------------------------------------------------------

void CVLEBase::SetFlashVapFrac(SpModel &Mdl, double VapFrac, dword Flags)
  {
  if (Enabled()) 
    {
#if DoDbg
    double T0=Mdl.Temp();
    double P0=Mdl.Press();
    double MV0=Mdl.Mass(som_Gas);
    double MSL0=Mdl.Mass(som_SL);
#endif

    double H=Mdl.totHf();
    m_pVLE->SetFlashVapFrac(Mdl, Mdl.Temp(), Mdl.Press(), VapFrac, Flags);
    Mdl.Set_totHf(H);
    Mdl.SetFlashDescription(FlashDescription());
    m_sSatMethod = FlashDescription();

#if DoDbg
    double T1=Mdl.Temp();
    double P1=Mdl.Press();
    double MV1=Mdl.Mass(som_Gas);
    double MSL1=Mdl.Mass(som_SL);
    if (Mdl.pAttachedTo && Mdl.pAttachedTo->DoDbgBrk())
      {
      dbgpln("SetFlashVapFrac:%s",Mdl.FullObjTag());
      dbgpln("  T:%+12.4g %12.6f > %12.6f", T1-T0,T0,T1);
      dbgpln("  P:%+12.4g %12.6f > %12.6f", P1-P0,P0,P1);
      dbgpln("  M:%+12.4g %12.6f > %12.6f %12.6f>%12.6f", (MV1-MV0)+(MSL1-MSL0),MV0,MV1,MSL0,MSL1);
      }
    int xxx=0;
#endif
    }
  };

//--------------------------------------------------------------------------

void CVLEBase::TPFlash(SpModel &Mdl, double Temp, double Press, dword Flags)
  {
  if (Enabled()) 
    {
    VLETIMERLAP(sm_swTPFlash);
#if DoDbg
    double T0=Mdl.Temp();
    double P0=Mdl.Press();
    double MV0=Mdl.Mass(som_Gas);
    double MSL0=Mdl.Mass(som_SL);
#endif

    //m_TIn=Mdl.Temp();
    //m_PIn=Mdl.Press();
    //m_VapQmIn=Ql.QMass(som_Gas)+Qv.QMass(som_Gas);
    //m_VapFracIn=m_VapQmIn/GTZ(Ql.QMass()+Qv.QMass());

    m_pVLE->TPFlash(Mdl, Temp, Press, Flags);
    Mdl.SetFlashDescription(FlashDescription());
    m_sSatMethod = FlashDescription();

    //m_TOut=Ql.Temp();
    //m_POut=Ql.Press();
    //m_VapQmOut=Ql.QMass(som_Gas)+Qv.QMass(som_Gas);
    //m_VapFracOut=m_VapQmOut/GTZ(Ql.QMass()+Qv.QMass());

#if DoDbg
    double T1=Mdl.Temp();
    double P1=Mdl.Press();
    double MV1=Mdl.Mass(som_Gas);
    double MSL1=Mdl.Mass(som_SL);
    if (Mdl.pAttachedTo && Mdl.pAttachedTo->DoDbgBrk())
      {
      dbgpln("TPFlash:%s",Mdl.FullObjTag());
      dbgpln("  T:%+12.4g %12.6f > %12.6f", T1-T0,T0,T1);
      dbgpln("  P:%+12.4g %12.6f > %12.6f", P1-P0,P0,P1);
      dbgpln("  M:%+12.4g %12.6f > %12.6f %12.6f>%12.6f", (MV1-MV0)+(MSL1-MSL0),MV0,MV1,MSL0,MSL1);
      }
    int xxx=0;
#endif
    }
  };

//--------------------------------------------------------------------------

void CVLEBase::SaveFeedInfo(bool Cvt2Flow, SpModel * Ql, SpModel * Qv)
  {
  m_TempIn          = Ql->Temp();
  m_TotalPIn        = Ql->Press();
  m_TotQmIn         = Ql->Mass();
  m_VapQmIn         = Ql->Mass(som_Gas);
  if (Qv)
    {
    m_TotQmIn       += Qv->Mass();
    m_VapQmIn       += Qv->Mass(som_Gas);
    }
  m_VapFracIn       = m_VapQmIn/GTZ(m_TotQmIn);
  
  if (FlashVapIndex()>=0)
    {
    m_FlshVapQmIn   = Ql->VValue[FlashVapIndex()];
    m_FlshVapFracIn = m_FlshVapQmIn/GTZ(m_TotQmIn);
    if (Qv)
      {
      m_FlshVapQmIn += Qv->VValue[FlashVapIndex()];
      m_PartialPIn  = Qv->PartialPress(FlashVapIndex(), -1, m_TempIn);
      }
    else
      m_PartialPIn  = Ql->PartialPress(FlashVapIndex(), -1, m_TempIn);
    }
  else
    {
    m_PartialPIn    = 0.0;
    m_FlshVapQmIn   = m_VapQmIn;
    m_FlshVapFracIn = m_VapFracIn;
    }
  m_sSatMethod = "?";
  }

void CVLEBase::SaveProdInfo(bool Cvt2Flow, SpModel * Ql, SpModel * Qv)
  {
  m_TempOut           = Ql->Temp();
  m_TotalPOut         = Ql->Press();
  m_TotQmOut          = Ql->Mass();
  m_VapQmOut          = Ql->Mass(som_Gas);
  if (Qv)
    {
    m_TotQmOut        += Qv->Mass();
    m_VapQmOut        += Qv->Mass(som_Gas);
    }
  m_VapFracOut        = m_VapQmOut/GTZ(m_TotQmOut);
  if (FlashVapIndex()>=0)
    {
    m_FlshVapQmOut    = Ql->VValue[FlashVapIndex()];
    m_FlshVapFracOut  = m_FlshVapQmOut/GTZ(m_TotQmOut);
    if (Qv)
      {
      m_PartialPOut   = Qv->PartialPress(FlashVapIndex(), -1, m_TempOut);
      m_FlshVapQmOut  += Qv->VValue[FlashVapIndex()];
      }
    else
      m_PartialPOut   = Ql->PartialPress(FlashVapIndex(), -1, m_TempOut);
    }
  else
    {
    m_FlshVapQmOut    = m_VapQmOut;
    m_FlshVapFracOut  = m_VapFracOut;
    }

  m_BPE               = Ql->BoilingPtElevation(m_TotalPOut);
  m_sSpModel          = Ql->ClassId();
  //m_sSatMethod        = "";//Ql->SaturationMethodUsed();
  }

//--------------------------------------------------------------------------

void CVLEBase::PFlash(SpConduit &Q, double Press, double Duty, dword Flags)
  {
  if (Enabled()) 
    {
    VLETIMERLAP(sm_swQPFlash);
#if DoDbg
    double T0=Q.Temp();
    double P0=Q.Press();
    double H0=Q.totHf();
    double MV0=Q.QMass(som_Gas);
    double MSL0=Q.QMass(som_SL);
#endif

    if ((Flags&VLEF_HoldFeedMeasure)==0)
      SaveFeedInfo(false, Q);

    m_pVLE->PFlash(Q, Press, Duty, Flags);

    if ((Flags&VLEF_HoldProdMeasure)==0)
      SaveProdInfo(false, Q);
    Q.SetFlashDescription(FlashDescription());
    m_sSatMethod = FlashDescription();

#if DoDbg
    double T1=Q.Temp();
    double P1=Q.Press();
    double H1=Q.totHf();
    double MV1=Q.QMass(som_Gas);
    double MSL1=Q.QMass(som_SL);
    if (Q.pAttachedTo && Q.pAttachedTo->DoDbgBrk())
      {
      dbgpln("PFlash:%s",Q.FullObjTag());
      dbgpln("  T:%+14.6g %12.6f > %12.6f", T1-T0,T0,T1);
      dbgpln("  P:%+14.6g %12.6f > %12.6f", P1-P0,P0,P1);
      dbgpln("  M:%+14.6g %12.6f > %12.6f %12.6f>%12.6f", (MV1-MV0)+(MSL1-MSL0),MV0,MV1,MSL0,MSL1);
      dbgpln("  H:%+14.6g %12.6f > %12.6f", H1-H0,H0,H1);
      }
    int xxx=0;
#endif
    }
  };

//--------------------------------------------------------------------------

void CVLEBase::PFlash(SpConduit &Ql, SpConduit &Qv, double Press, double Duty, dword Flags)
  {
  if (Enabled()) 
    {
    VLETIMERLAP(sm_swQPFlash);
#if DoDbg
    double T0=Ql.Temp();
    double P0=Ql.Press();
    double H0=Ql.totHf()+Qv.totHf();
    double MV0=Ql.QMass(som_Gas)+Qv.QMass(som_Gas);
    double MSL0=Ql.QMass(som_SL)+Qv.QMass(som_SL);
    SpMArray XXX(*Ql.Model());
    //    int dbg=Ql.DoDbgBrk();
    int dbg=m_pNd && m_pNd->DoDbgBrk();
    if (dbg)
      {
      dbgpln("PFlash:%s",Ql.FullObjTag());
      Flags |= VLEF_WithDebug;
      }
#endif

    if ((Flags&VLEF_HoldFeedMeasure)==0)
      SaveFeedInfo(false, Ql, Qv);

    m_pVLE->PFlash(Ql, Qv, Press, Duty, Flags);

    if ((Flags&VLEF_HoldProdMeasure)==0)
      SaveProdInfo(false, Ql, Qv);
    Ql.SetFlashDescription(FlashDescription());
    Qv.SetFlashDescription(FlashDescription());
    m_sSatMethod = FlashDescription();

#if DoDbg
    double T1=Ql.Temp();
    double P1=Ql.Press();
    double H1=Ql.totHf()+Qv.totHf();
    double MV1=Ql.QMass(som_Gas)+Qv.QMass(som_Gas);
    double MSL1=Ql.QMass(som_SL)+Qv.QMass(som_SL);
    if (dbg)
      {
      //dbgpln("PFlash:%s",Ql.FullObjTag());
      //      dbgpln("  T:%+12.4g %12.6f > %12.6f", T1-T0,T0,T1);
      //      dbgpln("  P:%+12.4g %12.6f > %12.6f", P1-P0,P0,P1);
      //      dbgpln("  M:%+12.4g %12.6f > %12.6f %12.6f>%12.6f", (MV1-MV0)+(MSL1-MSL0),MV0,MV1,MSL0,MSL1);
      dbgpln("  T : %14.6f > %14.6f [%+14.6g]",  T0, T1, T1-T0);
      dbgpln("  P : %14.6f > %14.6f [%+14.6g]",  P0, P1, P1-P0);
      dbgpln("  H : %14.6g > %14.6g [%+14.6g]",  H0, H1, H1-H0);
      dbgpln("  Mv: %14.6f > %14.6f [%+14.6g]",  MV0, MV1, (MV1-MV0));
      dbgpln("  Ml: %14.6f > %14.6f [%+14.6g]",  MSL0, MSL1, (MSL1-MSL0));
      CSysVector &YYYl=Ql.MArray();
      CSysVector &YYYv=Qv.MArray();
      for (int i=0; i<SVSpcCount(); i++)
        {
        double YYY = YYYl[i]+YYYv[i];
        //if (XXX[i]>1.0e-6 || YYY>1.0e-6)
        if (fabs(YYY-XXX[i])>1e-6)
          dbgpln("  M : %14.6f > %14.6f [%+14.6g] %s", XXX[i], YYY, YYY-XXX[i], SVImg(i).SymOrTag());
        }
      }
    int xxx=0;
#endif
    }
  };

//--------------------------------------------------------------------------

void CVLEBase::PFlash(SpContainer &C, double Press, double Duty, dword Flags)
  {
  if (Enabled()) 
    {
    VLETIMERLAP(sm_swQVFlash);

#if DoDbg
    double T0=C.Temp();
    double P0=C.Press();
    double H0=C.totHf();
    double MV0=C.Mass(som_Gas);
    double MSL0=C.Mass(som_SL);
    SpMArray XXX(*C.Model());
    if (dbgQVFlash() || dbgQVFlashDbg() && C.DoDbgBrk())
      dbgpln("VFlash:%s",C.FullObjTag());
#endif

    if ((Flags&VLEF_HoldFeedMeasure)==0)
      SaveFeedInfo(true, C);

    m_pVLE->PFlash(C, Press, Duty, Flags);

    if ((Flags&VLEF_HoldProdMeasure)==0)
      SaveProdInfo(true, C);
    C.SetFlashDescription(FlashDescription());
    m_sSatMethod = FlashDescription();

#if DoDbg
    double T1=C.Temp();
    double P1=C.Press();
    double H1=C.totHf();
    double MV1=C.Mass(som_Gas);
    double MSL1=C.Mass(som_SL);
    if (dbgQVFlashChg() || dbgQVFlashChgDbg() && C.DoDbgBrk())
      {
      //      dbgpln("VFlash:%s",C.FullObjTag());
      dbgpln("  T : %14.6f > %14.6f [%+14.6g]",  T0, T1, T1-T0);
      dbgpln("  P : %14.6f > %14.6f [%+14.6g]",  P0, P1, P1-P0);
      dbgpln("  H : %14.6g > %14.6g [%+14.6g]",  H0, H1, H1-H0);
      dbgpln("  Mv: %14.6f > %14.6f [%+14.6g]",  MV0, MV1, (MV1-MV0));
      dbgpln("  Ml: %14.6f > %14.6f [%+14.6g]",  MSL0, MSL1, (MSL1-MSL0));
      CSysVector &YYY=C.MArray();
      for (int i=0; i<SVSpcCount(); i++)
        if (XXX[i]>1.0e-6 || YYY[i]>1.0e-6)
          dbgpln("  M : %14.6f > %14.6f [%+14.6g] %s", XXX[i], YYY[i], YYY[i]-XXX[i], SVImg(i).SymOrTag());
      }
    int xxx=0;
#endif
    }
  };

//--------------------------------------------------------------------------

void CVLEBase::VFlash(SpContainer &C, double Duty, dword Flags)
  {
  if (Enabled()) 
    {
    VLETIMERLAP(sm_swQVFlash);

#if DoDbg
    double T0=C.Temp();
    double P0=C.Press();
    double H0=C.totHf();
    double MV0=C.Mass(som_Gas);
    double MSL0=C.Mass(som_SL);
    SpMArray XXX(*C.Model());
    if (dbgQVFlash() || dbgQVFlashDbg() && C.DoDbgBrk())
      dbgpln("VFlash:%s",C.FullObjTag());
#endif

    if ((Flags&VLEF_HoldFeedMeasure)==0)
      SaveFeedInfo(true, C);

    m_pVLE->VFlash(C, Duty, Flags);

    if ((Flags&VLEF_HoldProdMeasure)==0)
      SaveProdInfo(true, C);
    C.SetFlashDescription(FlashDescription());
    m_sSatMethod = FlashDescription();

#if DoDbg
    double T1=C.Temp();
    double P1=C.Press();
    double H1=C.totHf();
    double MV1=C.Mass(som_Gas);
    double MSL1=C.Mass(som_SL);
    if (dbgQVFlashChg() || dbgQVFlashChgDbg() && C.DoDbgBrk())
      {
      //      dbgpln("VFlash:%s",C.FullObjTag());
      dbgpln("  T : %14.6f > %14.6f [%+14.6g]",  T0, T1, T1-T0);
      dbgpln("  P : %14.6f > %14.6f [%+14.6g]",  P0, P1, P1-P0);
      dbgpln("  H : %14.6g > %14.6g [%+14.6g]",  H0, H1, H1-H0);
      dbgpln("  Mv: %14.6f > %14.6f [%+14.6g]",  MV0, MV1, (MV1-MV0));
      dbgpln("  Ml: %14.6f > %14.6f [%+14.6g]",  MSL0, MSL1, (MSL1-MSL0));
      CSysVector &YYY=C.MArray();
      for (int i=0; i<SVSpcCount(); i++)
        if (XXX[i]>1.0e-6 || YYY[i]>1.0e-6)
          dbgpln("  M : %14.6f > %14.6f [%+14.6g] %s", XXX[i], YYY[i], YYY[i]-XXX[i], SVImg(i).SymOrTag());
      }
    int xxx=0;
#endif
    }
  };

//--------------------------------------------------------------------------

void CVLEBase::AddQVDerivs(SpContainer &C, double Duty, dword Flags)
  {
  if (Enabled()) 
    {
    VLETIMERLAP(sm_swQVDerivs);
    CSpMdlDerivCtrl &DC=C.pModel->m_DC;

    DC.Save(C.pModel);
    if (C.Closed())
      m_pVLE->VFlash(C, Duty, Flags);
    else
      m_pVLE->PFlash(C, AtmosPress(/*m_pNd->m_Datum*/), Duty, Flags);
    //StkSpContainer
    CSysVector &Mxx=C.pModel->MArray();

    C.pModel->m_DC.Estimate(C.pModel, ICGetDerivEstInc());

#if DoDbg
    double T0=C.Temp();
    double P0=C.Press();
    double H0=C.totHf();
    double MV0=C.Mass(som_Gas);
    double MSL0=C.Mass(som_SL);
    SpMArray XXX(*C.Model());
    if (dbgQVFlash() || dbgQVFlashDbg() && C.DoDbgBrk())
      dbgpln("AddQVFlash:%s",C.FullObjTag());
#endif

#if DoDbg
    //double DH0=C.pModel->m_totH;
    SpMArray AAA;
    AAA=*(C.Model()->m_DC.m_pDM);
#endif

    SpMArray MDeriv=C.pModel->MArray();
    double HDeriv=C.pModel->m_msHf*C.pModel->Mass();//+C.pModel->totHf(som_ALL, K2C(0.0), Std_P, C.pModel->pMArray());

    if (C.Closed())
      m_pVLE->VFlash(C, Duty, Flags);
    else
      m_pVLE->PFlash(C, AtmosPress(/*m_pNd->m_Datum*/), Duty, Flags);

#if DoDbg
    double T1=C.Temp();
    double P1=C.Press();
    double H1=C.totHf();
    double MV1=C.Mass(som_Gas);
    double MSL1=C.Mass(som_SL);
    if (dbgQVFlashChg() || dbgQVFlashChgDbg() && C.DoDbgBrk())
      {
      //      dbgpln("VFlash:%s",C.FullObjTag());
      dbgpln("  T : %14.6f > %14.6f [%+14.6g]",  T0, T1, T1-T0);
      dbgpln("  P : %14.6f > %14.6f [%+14.6g]",  P0, P1, P1-P0);
      dbgpln("  H : %14.6g > %14.6g [%+14.6g]",  H0, H1, H1-H0);
      dbgpln("  Mv: %14.6f > %14.6f [%+14.6g]",  MV0, MV1, (MV1-MV0));
      dbgpln("  Ml: %14.6f > %14.6f [%+14.6g]",  MSL0, MSL1, (MSL1-MSL0));
      CSysVector &YYY=C.MArray();
      for (int i=0; i<SVSpcCount(); i++)
        if (XXX[i]>1.0e-10 || YYY[i]>1.0e-10)
          dbgpln("  M : %14.6f > %14.6f [%+14.6g] %s", XXX[i], YYY[i], YYY[i]-XXX[i], SVImg(i).SymOrTag());
      }
#endif

    for (int s=0; s<SVSpcCount(); s++)
      MDeriv.VValue[s]=(C.pModel->MArray()[s]-MDeriv[s])/ICGetDerivEstInc();
    double HEnd=C.pModel->m_msHf*C.pModel->Mass();//+C.pModel->totHf(som_ALL, K2C(0.0), Std_P, C.pModel->pMArray());
    HDeriv=(HEnd-HDeriv)/ICGetDerivEstInc();

#if DoDbg
    if (dbgAddQVDerivs() && C.DoDbgBrk())
      dbgpln("AddQVDerivs:%s",C.FullObjTag());
    if (dbgAddQVDerivs() && C.DoDbgBrk())
      {
      dbgpln("  DH : %14.6g ",  HDeriv);
      SpMArray BBB;
      BBB=*(C.Model()->m_DC.m_pDM);
      for (int i=0; i<SVSpcCount(); i++)
        if (fabs(MDeriv[i])>1.0e-10)
          dbgpln("  DM : %14.6f %s", MDeriv[i], SVImg(i).SymOrTag());
      }
#endif

    C.AddInternalDerivs(&MDeriv, HDeriv);

    DC.Restore(C.pModel);

    }
  };

//--------------------------------------------------------------------------

void CVLEBase::MVapFlash(SpConduit &Q, double MVap, double Duty, dword Flags)
  {
  if (Enabled()) 
    {
    VLETIMERLAP(sm_swQMVapFlash);
#if DoDbg
    double T0=Q.Temp();
    double P0=Q.Press();
    double MV0=Q.QMass(som_Gas);
    double MSL0=Q.QMass(som_SL);
#endif

    if ((Flags&VLEF_HoldFeedMeasure)==0)
      SaveFeedInfo(false, Q);

    m_pVLE->MVapFlash(Q, MVap, Duty, Flags);

    if ((Flags&VLEF_HoldProdMeasure)==0)
      SaveProdInfo(false, Q);
    Q.SetFlashDescription(FlashDescription());
    m_sSatMethod = FlashDescription();

#if DoDbg
    double T1=Q.Temp();
    double P1=Q.Press();
    double MV1=Q.QMass(som_Gas);
    double MSL1=Q.QMass(som_SL);
    if (Q.pAttachedTo && Q.pAttachedTo->DoDbgBrk())
      {
      dbgpln("MVapFlash:%s",Q.FullObjTag());
      dbgpln("  T:%+12.4g %12.6f > %12.6f", T1-T0,T0,T1);
      dbgpln("  P:%+12.4g %12.6f > %12.6f", P1-P0,P0,P1);
      dbgpln("  M:%+12.4g %12.6f > %12.6f %12.6f>%12.6f", (MV1-MV0)+(MSL1-MSL0),MV0,MV1,MSL0,MSL1);
      }
    int xxx=0;
#endif
    }
  };

//--------------------------------------------------------------------------

void CVLEBase::MVapFlash(SpConduit &Ql, SpConduit &Qv, double MVap, double Duty, dword Flags)
  {
  if (Enabled()) 
    {
    VLETIMERLAP(sm_swQMVapFlash);
#if DoDbg
    double T0=Ql.Temp();
    double P0=Ql.Press();
    double MV0=Ql.QMass(som_Gas);
    double MSL0=Ql.QMass(som_SL);
#endif

    if ((Flags&VLEF_HoldFeedMeasure)==0)
      SaveFeedInfo(false, Ql, Qv);

    m_pVLE->MVapFlash(Ql, Qv, MVap, Duty, Flags);

    if ((Flags&VLEF_HoldProdMeasure)==0)
      SaveProdInfo(false, Ql, Qv);
    Ql.SetFlashDescription(FlashDescription());
    Qv.SetFlashDescription(FlashDescription());
    m_sSatMethod = FlashDescription();

#if DoDbg
    double T1=Ql.Temp();
    double P1=Ql.Press();
    double MV1=Ql.QMass(som_Gas);
    double MSL1=Ql.QMass(som_SL);
    if (Ql.pAttachedTo && Ql.pAttachedTo->DoDbgBrk())
      {
      dbgpln("PFlash:%s",Ql.FullObjTag());
      dbgpln("  T:%+12.4g %12.6f > %12.6f", T1-T0,T0,T1);
      dbgpln("  P:%+12.4g %12.6f > %12.6f", P1-P0,P0,P1);
      dbgpln("  M:%+12.4g %12.6f > %12.6f %12.6f>%12.6f", (MV1-MV0)+(MSL1-MSL0),MV0,MV1,MSL0,MSL1);
      }
    int xxx=0;
#endif
    }
  };

//--------------------------------------------------------------------------

int CVLEBase::GetVLEData(CVLEDataArray & Array)                                  { return Enabled() ? m_pVLE->GetVLEData(Array) : 0;          };
double CVLEBase::CondensableFrac(SpConduit *Ql, SpConduit *Qv)                   { return Enabled() ? m_pVLE->CondensableFrac(Ql, Qv) : dNAN; };
double CVLEBase::CondensableFrac(SpContainer *Cn)                                { return Enabled() ? m_pVLE->CondensableFrac(Cn) : dNAN;     };
double CVLEBase::ApproachTemp()                                                  { return Enabled() && (m_iFlags&VLEF_ApproachTempOn) ? m_pVLE->ApproachTemp() : 0.0; };

//============================================================================
//
//
//
//============================================================================

#if WithFixedPerc

IMPLEMENT_TAGOBJSLCT(CVLEFixdPerc, CVLEBlock::GroupName, "CVLEFixdPerc", "", "", TOC_SYSTEM, "Multi/Fixed %", 
                     "Vapour Liquid Equilibrium Fixed Percentage",
                     VLEF_VfFlash | VLEF_TPFlash | VLEF_QPFlash /*| VLEF_QVFlash*/);
CVLEFixdPerc::CVLEFixdPerc(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
CVLEBlock(pClass_, TagIn, pAttach, eAttach)//,
//Nd(*((FlwNode*)pAttach))
  {
  for (int i=0; i<CDB.No(); i++)
    if (CDB[i].LiqPhInx("l", true)>=0 && CDB[i].VapPhInx("g", true)>=0)
      { 
      CVLEData C(i, 0.5, CDB[i].LiqPhInx("l", true), CDB[i].VapPhInx("g", true));
      Cmp.Add(C);
      }

    M.SetSize(NAvailComps());
    F.SetSize(NAvailComps());
    iState=VLES_Error;
    nComps=0;
  };

//--------------------------------------------------------------------------

CVLEFixdPerc::~CVLEFixdPerc()
  {
  for (int i=0; i<CICount(); i++)
    ClrCI(i);
  };

//--------------------------------------------------------------------------

void CVLEFixdPerc::BuildDataDefn(DataDefnBlk &DDB)//, pchar pTag_, DDBPages PageIs, dword UserInfo)
  {
  if (DDB.BeginStruct(this))
    {
    DDB.Long("", "State",    DC_,     "",    &iState, this, 0, DDBVLEStates);

    if (PrjFileVerNo()<22)
      {
      Strng Sym;
      for (int i=0; i<Cmp.GetSize(); i++)
        {
        ASSERT(CDB[Cmp[i].iCmpNo].Sym());
        //Sym.Set("%s.K", CDB[Cmp[i].iCmpNo].Sym());
        Sym = CDB[Cmp[i].iCmpNo].Sym + ".K";
        DDB.Double("", Sym(), DC_Frac, "%", &Cmp[i].dData, this, isParm);
        }
      }
    else
      {
      if (DDB.BeginObject(this, "K", "VLE_K", NULL, DDB_NoPage))
        {
        for (int i=0; i<Cmp.GetSize(); i++)
          DDB.Double("", CDB[Cmp[i].iCmpNo].Sym(), DC_Frac, "%", &Cmp[i].dData, this, isParm|NAN_OK);
        }
      DDB.EndObject();
      }
    }
  DDB.EndStruct();
  };

//--------------------------------------------------------------------------

flag CVLEFixdPerc::DataXchg(DataChangeBlk & DCB)
  {
  return 0;
  };

//--------------------------------------------------------------------------

flag CVLEFixdPerc::ValidateData(ValidateDataBlk & VDB)
  {
  for (int c=0; c<NAvailComps(); c++)
    Cmp[c].dData=Range(0.0, Cmp[c].dData, 1.0);
  flag OK=1;

  iState=OK ? VLES_OK : VLES_Error;
  return OK;
  };

// -------------------------------------------------------------------------

int CVLEFixdPerc::Setup(SpModel &Mdl)
  { 
  // Initial Calcs
  //TODO What about MixFrac
  Mt=0.0;
  Ht=Mdl.totHf();
  for (int c=0; c<NAvailComps(); c++)
    {
    CVLEData &C=Cmp[c];
    F[c]=Range(0.0, C.dData, 1.0);
    M[c]=Mdl.M[C.li]+Mdl.M[C.vi];
    Mt+=M[c];
    }

  nComps=0;
  for (c=0; c<NAvailComps(); c++)
    if (M[c]/GTZ(Mt)>1.0e-8)
      nComps++;

  m_iState=VLES_Error;
  SetCI(2, nComps<=0);
  if (nComps>0)
    m_iState=VLES_OK;
  SetCI(1, (m_iState==VLES_Error));
  return m_iState;
  }

// -------------------------------------------------------------------------

void CVLEFixdPerc::SetLVRatio(SpModel &Mdl)
  {
  for (int c=0; c<NAvailComps(); c++)
    {
    CVLEData &C=Cmp[c];
    Mdl.SetMass(C.li, M[c]*(1.0-F[c]));
    Mdl.SetMass(C.vi, M[c]*F[c]);
    }

  //Mdl.Set_totHf(Ht+Duty);
  }

// -------------------------------------------------------------------------

double CVLEFixdPerc::FlashVapFrac(SpModel &Mdl)
  { 
  DoBreak();
  return 0.0;
  };

// -------------------------------------------------------------------------

void CVLEFixdPerc::SetFlashVapFrac(SpModel &Mdl, double Temp, double Press, double VapFrac, dword Flags)
  { 
  DoBreak();
  //if (Setup(Mdl)==VLES_OK)
  //  {
  //  SetLVRatio(Mdl);
  //  //Mdl.Set_totHf(Ht+Duty);
  //  Mdl.SetTemp(Temp);
  //  Mdl.SetPress(Press);
  //  }
  };
// -------------------------------------------------------------------------

void CVLEFixdPerc::TPFlash(SpModel &Mdl, double Temp, double Press, dword Flags)
  { 

  if (Setup(Mdl)==VLES_OK)
    {
    SetLVRatio(Mdl);
    //Mdl.Set_totHf(Ht+Duty);
    Mdl.SetTempPress(Temp, Press);
    }

  };

// -------------------------------------------------------------------------

void CVLEFixdPerc::PFlash(SpConduit &Q, double Press, double Duty, dword Flags)
  { 
  if (Setup(*Q.Model())==VLES_OK)
    {
    SetLVRatio(*Q.Model());
    Q.Set_totHf(Ht+Duty);
    }
  };

// -------------------------------------------------------------------------

void CVLEFixdPerc::PFlash(SpConduit &Ql, SpConduit &Qv, double Press, double Duty, dword Flags)
  {
  DoBreak();
  /*
  if (Setup(*Q.Model())==VLES_OK)
  {
  SetLVRatio(*Q.Model());
  Q.Set_totHf(Ht+Duty);
  }
  */
  };

// -------------------------------------------------------------------------

void CVLEFixdPerc::VFlash(SpContainer &C, double Duty, dword Flags)
  { 
  if (Setup(*C.Model())==VLES_OK)
    {
    SetLVRatio(*C.Model());
    C.Set_totHf(Ht+Duty);
    }
  };

// -------------------------------------------------------------------------

void CVLEFixdPerc::MVapFlash(SpConduit &Q, double MVap, double Duty, dword Flags)
  { 
  DoBreak();
  };

// -------------------------------------------------------------------------

void CVLEFixdPerc::MVapFlash(SpConduit &Ql, SpConduit &Qv, double MVap, double Duty, dword Flags)
  { 
  DoBreak();
  };

// -------------------------------------------------------------------------

int CVLEFixdPerc::GetVLEData(CVLEDataArray & Array)
  {
  Array.SetSize(Cmp.GetSize());
  for (int i=0; i<Array.GetSize(); i++)
    Array[i]=Cmp[i];
  return Array.GetSize();
  };

//--------------------------------------------------------------------------

flag CVLEFixdPerc::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case 1: pS="N\tVLE:Error";                      return 1; 
    case 2: pS="N\tVLE:Insufficient components";    return 1;
    case 3: pS="N\tVLE:Too many components";        return 1;
    case 4: pS="N\tVLE:PFlash not Conconverged"; return 1;
    default:                                               
      return CVLEBlock::CIStrng(No, pS);
    }
  };

#endif
//============================================================================
//
//
//
//============================================================================

IMPLEMENT_TAGOBJSLCT(CVLEVapPress, CVLEBlock::GroupName, "CVLEVapPress", "", "", "", TOC_SYSTEM, "Single/Vapour P", 
                     "Vapour Liquid Equilibrium Vapour Press",
                     VLEF_VfFlash | VLEF_TPFlash | VLEF_QPFlash | VLEF_QVFlash | VLEF_QMVapFlash );
CVLEVapPress::CVLEVapPress(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
CVLEBlock(pClass_, TagIn, pAttach, eAttach)//,
//Nd(*((FlwNode*)pAttach))
  {
  // Initialise
  pMdlL=NULL;
  pMdlV=NULL;

  m_iCmpNoReqd=-1;
  m_iCmpNo=CDB.DefFlashCmpIndex(); //Set to the default 
  if (m_iCmpNo<0 && CDB.m_Flashable.GetCount()>=0)
    m_iCmpNo=CDB.m_Flashable[0];    //First Flashable

  m_iLiqNo=-1;
  m_iVapNo=-1;
  m_iState=VLES_CfgError;
  Mt=0.0;
  Ht=0.0;
  m_dPrvVapFrac=0.1;
  m_dPrvVapPress=Std_P;
  m_fModelSatP=true;
  m_dApproachTemp=0.0;
  m_iFlashMethod=SMFM_Default;
  };

//--------------------------------------------------------------------------

CVLEVapPress::~CVLEVapPress()
  {
  for (int i=0; i<CICount(); i++)
    ClrCI(i);
  };

//--------------------------------------------------------------------------

void CVLEVapPress::BuildDataDefn(DataDefnBlk &DDB)//, pchar pTag_, DDBPages PageIs, dword UserInfo)
  {
  if (DDB.BeginStruct(this))
    {

    DDB.Long    ("", "State",      DC_, "", &m_iState,     this, 0, DDBVLEStates);
    DDB.CheckBox("", "ModelSatP",  DC_, "", &m_fModelSatP, this, isParm, DDBOnOff);

    //Strng S;
    //DDBValueLstMem DDBCmp;
    //DDBCmp.Add(-1, " ", MDD_Hidden);
    //DDBCmp.Add(-1, "<Default>");
    //for (int i=0; i<CDB.No(); i++)
    //  if (CDB[i].VpOK() && (CDB[i].LiqPhInx("l", true)>=0) && (CDB[i].VapPhInx("g", true)>=0))
    //    DDBCmp.Add(i, CDB[i].SymOrTag());


//DDB.String  ("Method",        "",         DC_,      "",         xidSMSatMethod,   this, isParm|SetOnChange, CDB.m_DDBFlashMethods());
//DDB.String  ("Component",     "",         DC_,      "",         xidSMSatComp,     this, isParm|SetOnChange, CDB.m_DDBFlashable());

    DDB.String("Method",       "",  DC_,     "",    xidVLEFlashMethod, this, isParm|SetOnChange, CDB.m_DDBFlashMethods());
    DDB.String("Component",    "",  DC_,     "",    xidVLEFlashComp,   this, isParm|SetOnChange, CDB.m_DDBFlashable());
    DDB.Double("PrvVapFrac",   "",  DC_Frac, "%",   &m_dPrvVapFrac,    this, isParm|InitHidden);
    DDB.Double("PrvVapPress",  "",  DC_P,    "kPa", &m_dPrvVapPress,   this, isParm|InitHidden);
    if (m_pVLEBase->ApproachTempOn())
      DDB.Double("ApproachTemp", "",  DC_dT,   "C",   &m_dApproachTemp,  this, isParm);
    }
  DDB.EndStruct();
  };                                      

//--------------------------------------------------------------------------

flag CVLEVapPress::DataXchg(DataChangeBlk & DCB)
  {   
  switch (DCB.lHandle)
    {
    case xidVLEFlashMethod:
      {
      //m_iFlashMethod
      if (DCB.rpC)
        {
        if (stricmp(DCB.rpC, "<Default>")==0)
          {
          m_iFlashMethod=SMFM_Default; 
          }
        else
          {
          int i=CDB.m_DDBFlashMethods.Find(DCB.rpC);
          if (i>=0)  
            {
            i=CDB.m_DDBFlashMethods[i].m_lVal;
            m_iFlashMethod=i;
            }
          }
        }
      if (m_iFlashMethod>=0)
        DCB.pC = CDB.m_FlashMethods[m_iFlashMethod]();
      else
        DCB.pC = CDB.m_FlashMethodsI[CDB.DefFlashMethod()]();
      return 1;
      }
    case xidVLEFlashComp:
      {
      if (DCB.rpC)
        {
        if (stricmp(DCB.rpC, "<Default>")==0)
          {
          m_iCmpNoReqd=-1;
          m_iCmpNo=CDB.DefFlashCmpIndex();
          }
        else
          {
          int i=CDB.m_DDBFlashable.Find(DCB.rpC);
          if (i>=0)  
            {
            i=CDB.m_DDBFlashable[i].m_lVal;
            m_iCmpNoReqd=i;
            m_iCmpNo=i;
            }
          }
        }
      if (m_iCmpNoReqd>=0)
        DCB.pC = CDB.m_FlashCmps[m_iCmpNoReqd]();
      else if (m_iCmpNo>=0)
        DCB.pC = CDB.m_FlashCmpsI[m_iCmpNo]();
      else
        DCB.pC = CDB.m_FlashCmpsI[CDB.DefFlashCmpIndex()]();
      return 1;
      }
    default:
      break;
    }

  return 0;
  };

//--------------------------------------------------------------------------

flag CVLEVapPress::ValidateData(ValidateDataBlk & VDB)
  {
  //dApproachTemp = ValidateRange(VDB, "ApproachTemp", 0.0, dApproachTemp,    20.0);

  flag OK=true;
  if (m_iCmpNo>=0)
    {
    m_iLiqNo=CDB[m_iCmpNo].LiqPhInx("l", false);
    if (m_iLiqNo<0)
      m_iLiqNo=CDB[m_iCmpNo].LiqPhInx("aq", true);
    m_iVapNo=CDB[m_iCmpNo].VapPhInx("g", true);
    OK=(m_iLiqNo>=0 && m_iVapNo>=0);

    bool Lst[MaxSpecies];
    for (int i=0; i<SVSpcCount(); i++)
      Lst[i]=(i!=m_iVapNo && SVImg(i).PhaseIs(som_Gas)!=0);

    OtherVapSp.CreateFrom(Lst, false);
    }
  else
    {
    m_iLiqNo=-1;
    m_iVapNo=-1;
    OK=false;
    }
  m_iState=OK ? VLES_OK : VLES_CfgError;
  return OK;
  };

// -------------------------------------------------------------------------

int CVLEVapPress::Setup(SpModel *pMdlL_, SpModel *pMdlV_)
  { 
  pMdlL=pMdlL_;
  pMdlV=pMdlV_;
  // Initial Calcs
  m_iState=VLES_CfgError;
  if (m_iLiqNo>=0 && m_iVapNo>=0)
    {
    ClrCI(1);
    //Mt=0.0;
    Ht=pMdlL->totHf();
    Mt=pMdlL->m_M[m_iLiqNo]+pMdlL->m_M[m_iVapNo];
    if (pMdlV)
      {
      // Vapours ito Liq
      Ht+=pMdlL->totHf(som_ALL, pMdlV->Temp(), pMdlV->Press(), pMdlV->pMArray());
      //Ht+=pMdlV->totHf();
      Mt+=pMdlV->m_M[m_iLiqNo]+pMdlV->m_M[m_iVapNo];
      }

    if (Mt>=UsableMass)
      //if (Mt>1.0e-8)
      {
      m_iState=VLES_OK;
      Vf=pMdlL->m_M[m_iVapNo];
      if (pMdlV)
        Vf+=pMdlV->m_M[m_iVapNo];
      Vf/=Mt;
      ClrCI(2);
      m_iState=VLES_OK;
      }
    else
      {
      m_iState=VLES_Nothing;
      //SetCI(2);
      Vf=0.0;
      }
    }
  else
    {
    SetCI(1);
    ClrCI(2);
    }
  return m_iState;
  }

// -------------------------------------------------------------------------

void CVLEVapPress::SetVapFrac(double VapFrac)//, double RqdH)
  {
#ifndef _RELEASE
  if (VapFrac<0.0 || VapFrac>1.0)
    DoBreak(); //Unexpected Vapour Fraction!
#endif
  if (pMdlV)
    {
    pMdlL->VValue[m_iLiqNo] = Mt*(1.0-VapFrac);
    pMdlL->VValue[m_iVapNo] = 0.0;
    pMdlV->VValue[m_iLiqNo] = 0.0;
    pMdlV->VValue[m_iVapNo] = Mt*VapFrac;
    }
  else
    {
    pMdlL->VValue[m_iLiqNo] = Mt*(1.0-VapFrac);
    pMdlL->VValue[m_iVapNo] = Mt*VapFrac;
    }
  }

// -------------------------------------------------------------------------

LPTSTR CVLEVapPress::FlashDescription()
  {
  m_sFlashDesc="Vp:";
  m_sFlashDesc+= (FlashMethod()==SMFM_PartialP) ? "PartialP":"TotalP";
  m_sFlashDesc+=":";
  if (m_iCmpNo>=0)
    m_sFlashDesc+=CDB[m_iCmpNo].SymOrTag();
  else 
    m_sFlashDesc+="???";
  return (LPTSTR)(LPCTSTR)m_sFlashDesc();
  };

int    CVLEVapPress::FlashCmpIndex()                    { return m_iCmpNo;      };
int    CVLEVapPress::FlashLiqIndex()                    { return m_iLiqNo;      };
int    CVLEVapPress::FlashVapIndex()                    { return m_iVapNo;      };
int    CVLEVapPress::FlashMethod()                      { return m_iFlashMethod>=0?m_iFlashMethod:CDB.DefFlashMethod(); };
double CVLEVapPress::ApproachTemp()                     { return m_dApproachTemp; };

// -------------------------------------------------------------------------

double CVLEVapPress::FlashVapFrac(SpModel &Mdl)
  { 
  if (Setup(&Mdl, NULL)==VLES_OK)
    return Vf;
  return 0.0;
  };

// -------------------------------------------------------------------------

void CVLEVapPress::SetFlashVapFrac(SpModel &Mdl, double Temp, double Press, double VapFrac, dword Flags)
  { 
  if (Setup(&Mdl, NULL)==VLES_OK)
    {
    SetVapFrac(VapFrac);
    Mdl.SetTempPress(Temp, Press);
    }
  }

// -------------------------------------------------------------------------

void CVLEVapPress::TPFlash(SpModel &Mdl, double Temp, double Press, dword Flags)
  { 
  if (Setup(&Mdl, NULL)==VLES_OK)
    {
    Mdl.SetHoldVentExpand();  

    //    DoBreak(); // Should Use Partial Press
    //double PPressO=Mdl.

    CSaturationDefn &SD=FlashDefn();
    double OtherPP;
    if (SD.Method()==SMFM_PartialP)
      OtherPP=pMdlL->PartialPress(OtherVapSp, Temp);
    else
      OtherPP=0.0;
    double TSat;
    if (m_fModelSatP)
      TSat=Mdl.SaturationT(Max(1.0, Press-OtherPP));
    else
      TSat=CDB[m_iCmpNo].VapourT(Mdl.Fidelity(), Max(1.0, Press-OtherPP));

    double VF=(Temp > TSat) ?  1.0 : 0.0;

    SetFlashVapFrac(Mdl, Temp, Press, VF, Flags);

    Mdl.ClrHoldVentExpand();  
    }

  };

// -------------------------------------------------------------------------

extern int SpModelExDbgOn;

class QPRootFnd : public MRootFinderBase
  {
  public:
    SpModel * pMdlL;
    SpModel * pMdlV;
    CVLEVapPress &VLE;
    double m_TMn;
    double m_TMx;
    double m_P;
    double m_ApproachT;
    bool m_DbgOn;
    bool m_PassOne;
    static CToleranceBlock s_Tol;

    QPRootFnd(pSpModel pMdlL_, pSpModel pMdlV_, CVLEVapPress &VLE_, double p, double ApproachT, bool DbgOn) :
    VLE(VLE_),
      MRootFinderBase("QPRootFnd", s_Tol)//1.0e-12)
      {
      m_PassOne   = true;
      m_DbgOn     = DbgOn;
      pMdlL       = pMdlL_;
      pMdlV       = pMdlV_;
      m_P         = p;
      m_ApproachT = ApproachT;
      m_TMn       = CDB.MinT;
      m_TMx       = CDB.MaxT;
      SetErrorHandling(true, &pMdlL->m_fConvergeBusy);
      };

    LPCTSTR ObjTag() { return (LPCTSTR)pMdlL->FullObjTag(); };

    double Function(double x)
      {
      if (m_DbgOn && m_PassOne)
        { int xxx=0; }
      m_ApproachT=GEZ(m_ApproachT);

      VLE.SetVapFrac(Range(0.0, x, 1.0));

      SpModel * pWrk=(pMdlV ? pMdlV : pMdlL);
      CSaturationDefn &SD=VLE.FlashDefn();
      double OtherPFrac;
      if (SD.Method()==SMFM_PartialP)
        OtherPFrac=pWrk->PartialPressFrac(VLE.OtherVapSp, pMdlL->Temp(), m_P);
      else
        OtherPFrac=0.0;

      double Ts = Min(m_TMx, pMdlL->SaturationT(Max(1.0, (1.0-OtherPFrac)*m_P)-m_ApproachT, NULL, &SD/*.m_iCmpNo*/));
      double h, t;
      if (x<0.0)
        t=m_TMn+(Ts-m_TMn)*(x+1.0); // Map -1<=x<=0 on to TMn to Ts
      else if (x>1.0)
        t=Ts+(m_TMx-Ts)*(x-1.0);   // Map 1<=x<=2 on to Ts to TMx
      else
        t=Ts;                   // Map 1<=x<=2 on to Ts to TMx

      pMdlL->SetTempPress(t, m_P);
      if (pMdlV)
        {
        pMdlV->SetTempPress(t, m_P);
        }
      h = pMdlL->totHf();
      if (pMdlV)
        h+=pMdlL->totHf(som_ALL, pMdlV->Temp(), pMdlV->Press(), pMdlV->pMArray()); // Vapours in Liq

      if (m_DbgOn)
        {
        SpModelExDbgOn++;

        if (m_PassOne)
          dbgpln("  QPRoot\t%14s\t%14s\t%14s\t%14s\t%14s\t%14s\t%14s\t%14s\t%14s\t%14s\t%14s\t%14s\t%14s\t%14s", 
          "VFrac","Ts","t","TMx","M","m","P", "PP","Htrg","H-Htrg","Hl", "Hv", "Ml", "Mv");
        dbgpln("  QPRoot\t%14.10f\t%14.6f\t%14.6f\t%14.6f\t%14.6f\t%14.6f\t%14.6f\t%14.6f\t%14.6g\t%14.6f\t%14.6f\t%14.6f\t%14.6f", 
          x, K_2_C(Ts),K_2_C(t),K_2_C(m_TMx),VLE.Mt,pMdlL->Mass(),m_P,m_P*(1.0-OtherPFrac),dTarget,h-dTarget, 
          pMdlL->totHf(), pMdlV?pMdlL->totHf(som_ALL, pMdlV->Temp(), pMdlV->Press(), pMdlV->pMArray()):0.0,
          pMdlL->Mass(), pMdlV?pMdlV->Mass():0.0);

        //double xx0=pMdlL->totHf();
        //double xx1=pMdlV?pMdlL->totHf(som_ALL, pMdlV->Temp(), pMdlV->Press(), pMdlV->pMArray()):0;
        //int xxx=0;
        SpModelExDbgOn--;
        }
      m_PassOne=false;
      return h;
      };
  };

CToleranceBlock QPRootFnd::s_Tol(TBF_BothSys, "VLE:QPRootFind", 0, 1.0e-12);

// --------------------------------------------------------------------------

static int QPFlashBrentErr=0;

//Adjust VapFraction to give result at specified pressure. Output Hf = Input Hf + Duty.
void CVLEVapPress::PFlash(double Press, double Duty, dword Flags)
  {
  pMdlL->SetHoldVentExpand();  
  if (pMdlV)
    pMdlV->SetHoldVentExpand();  

  double P0 = pMdlL->Press();
  double T0 = pMdlL->Temp();
  double HL = pMdlL->totHf();
  double HV = 0.0;
  if (pMdlV)
    HV += pMdlL->totHf(som_ALL, pMdlV->Temp(), pMdlV->Press(), pMdlV->pMArray()); // Vapours in Liq
  double HTrg = HL+HV+Duty;
  double VFInit=Vf;
  double VFrac0=m_dPrvVapFrac;//Vf;

  if (pMdlV)
    {
    //move all 'other gasses' to pMdlV 
    SpecieIter I(som_Gas);
    for (int i=-1; I.Loop(i); )
      {
      if (i!=m_iVapNo)
        {
        pMdlV->VValue[i]+=pMdlL->VValue[i];  
        pMdlL->VValue[i]=0.0;  
        }
      }
    }

  QPRootFnd QPF(pMdlL, pMdlV, *this, Press, m_dApproachTemp, (Flags&VLEF_WithDebug)!=0);
  QPF.SetTarget(HTrg);

  double LoX = -1;
  double HiX = 2;

  flag fBad=false;
  QPF.SetLimits(LoX, HiX);

  if (QPF.SolveFromEst(VFrac0, 1.0, 0.1)==RF_OK)
    {
    }
  else if (QPF.Start(LoX, HiX)==RF_OK)
    {
    switch (QPF.Solve_Brent())
      {
      case RF_OK:
        break;

      case RF_HiLimit:
        break;

      case RF_LoLimit:
        break;

      default:
        fBad=true;
        break;
      }
    }
  else
    fBad=true;

  if (fBad)
    {
    SetVapFrac(VFInit);
    pMdlL->Set_totHf(HL);
    pMdlL->SetPress(P0);
    if (pMdlV)
      {
      pMdlV->Set_totHf(HV);
      pMdlV->SetPress(P0);
      }
    }
  if (fBad)
    {
    if (QPFlashBrentErr++<5)
      dbgpln("CVLEBase - QPF.Solve_Brent FAILED");
    }
  else
    {
    double dVFrac=QPF.Result();
    m_dPrvVapFrac=dVFrac;
    }

  pMdlL->ClrHoldVentExpand();  
  if (pMdlV)
    pMdlV->ClrHoldVentExpand();  

  };

// -------------------------------------------------------------------------

void CVLEVapPress::PFlash(SpConduit &Q, double Press, double Duty, dword Flags)
  { 
  if (Setup(Q.Model(), NULL)==VLES_OK)
    {
    PFlash(Press, Duty, Flags);
    }
  };

// -------------------------------------------------------------------------

void CVLEVapPress::PFlash(SpConduit &Ql, SpConduit &Qv, double Press, double Duty, dword Flags)
  { 
  if (Setup(Ql.Model(), Qv.Model())==VLES_OK)
    {
    PFlash(Press, Duty, Flags);
    }
  };

// -------------------------------------------------------------------------

void CVLEVapPress::PFlash(SpContainer &C, double Press, double Duty, dword Flags)
  { 
  if (Setup(C.Model(), NULL)==VLES_OK)
    {
    PFlash(Press, Duty, Flags);
    }
  };

//============================================================================

#define NEWQVFLASH 01

#if !NEWQVFLASH 

class VPQVFnd : public MRootFinderBase
  {
  public:
    SpContainer & C;
    CVLEVapPress & VLE;
    double P, Ht;
    static CToleranceBlock s_Tol;
    VPQVFnd(SpContainer & C_, CVLEVapPress & VLE_, double Ht_) : 
    MRootFinderBase("VPQVFnd", s_Tol),//1.0e-9),
      C(C_),
      VLE(VLE_)
      { 
      Ht=Ht_;
      //dbgpln("VLE:VPQFFnd --------");
      };
    char * ObjTag() { return C.FullObjTag(); };
    double Function(double Vf) 
      { 
      VLE.SetVapFrac(Vf);
      C.Set_totHf(Ht);
      //      double P1=C.Press();
      double P1=C.PartialPress(VLE.vi);
#ifdef _RELEASE
      double T1=Min(10000.0, C.Temp()-VLE.dApproachTemp); //range to prevent crash?!?
#else
      double T1=C.Temp()-VLE.dApproachTemp;
      ASSERT_ALWAYS(T1<10000.0, "HIGH temperature in VFlash!", __FILE__, __LINE__);
      T1=Min(10000.0, T1);
#endif

      double PSat;
      if (VLE.fModelSatP)
        PSat=C.SaturationP(T1);
      else
        PSat=CDB[VLE.iCmpNo].VapourP(T1); 
      //dbgpln("VLE:VPQFFnd %8.3f %8.3f %8.3f",P1,PSat,P1-PSat);
      return P1-PSat; 
      };
  };

CToleranceBlock VPQVFnd::s_Tol(TBF_BothSys, "VLE:VPQFFnd", 0, 1.0e-9);
#endif

// -------------------------------------------------------------------------

#if NEWQVFLASH 
CToleranceBlock s_QVFlashTol(TBF_DynSys, "CVLEVapPress:QVFlashTol", 0, 1.0e-10, 100, /*TBF_Fixed|TBF_UseAbs|*/TBF_UseRel|TBF_UseMax);
#endif

void CVLEVapPress::VFlash(SpContainer &C, double Duty, dword Flags)
  { 
  if (Setup(C.Model(), NULL)==VLES_OK)
    {
    SpModel & Mdl=*C.Model();
    C.SetHoldVentExpand();

#if NEWQVFLASH
    // dApproachTemp : neg diff - subcooled

    double HReqd=C.totHf();

    SPMDLTIMERLAP(sm_swEvalHC);
    double CurrentT=C.Temp();
    double ApparentT=CurrentT-m_dApproachTemp;
    double CurrentP=C.Press();
    double SaturationP=Mdl.SaturationP(ApparentT);
    double VolumeSL = C.Volume(som_SL, CurrentT, CurrentP);
    double CurrentSpace=C.Shape()->ActualVolume();
    double VGas=Max(CurrentSpace * 0.001, CurrentSpace-VolumeSL);

#if dbgVLEBlock
    if (dbgQVFlash() || dbgQVFlashDbg() && DoDbgBrk())
      dbgpln("%s     T:%12.5f Ta:%12.5f P:%12.5f Vp:%12.5f Vol:%12.5f %s",
      "Flash", K2C(CurrentT), K2C(ApparentT), CurrentP, Mdl.Press(), CurrentSpace,  
      Mdl.FullObjTag());
#endif


    const double MaxPr=50000.0;

    if (VGas > 1.0e-6)
      {
      CurrentSpace=C.Shape()->ActualVolume();
      VGas=Max(CurrentSpace * 0.001, CurrentSpace-VolumeSL);
      double XFrac=FlashVapFrac(Mdl);
      long nSuperHt=0;
      CurrentP=SaturationP;

      double V0=C.Volume(som_Gas, CurrentT, CurrentP);
      double H0=Mdl.totHf(som_ALL, CurrentT, CurrentP, C.pMArray());

      int dbgStarted=0;
      int MaxLoops=s_QVFlashTol.GetMaxIters();
      int Loop=0;
      bool DoDampOsc=false;
      double DXHist[3]={1,1,1};
      while ((Loop++)<MaxLoops)
        {
        if (s_QVFlashTol.ConvergedVV(V0, VGas))
          break;

        if (nSuperHt>3)
          {
          // SuperHeated - fall out and allow SpModel PVT Converge to 
          Mdl.ClrHStatesOK();
          Mdl.EvalHStates();
          break;
          }

        double DT=Min(0.01, 0.5* CurrentT);
        double DX=0.001;

        double VolumeSL = C.Volume(som_SL, CurrentT, CurrentP);
        CurrentSpace=C.Shape()->ActualVolume();
        VGas=Max(CurrentSpace * 0.001, CurrentSpace-VolumeSL);

        V0=Mdl.Volume(som_Gas, CurrentT, CurrentP, C.pMArray());
        H0=Mdl.totHf(som_ALL, CurrentT, CurrentP, C.pMArray());


        double VErr=V0-VGas;
        double HErr=H0-HReqd;

        double Scl= 1||DoDampOsc ? 0.5 : 1.0;
        double ReqdTChg, ReqdXChg;
        double dHdT, dVdT, dHdX, dVdX;


        double P1=Mdl.SaturationP(ApparentT+DT);
        double P2=Mdl.SaturationP(ApparentT-DT);
        double VT1= Mdl.Volume(som_Gas, CurrentT+DT, P1, C.pMArray());
        double HT1= Mdl.totHf(som_ALL, CurrentT+DT, P1, C.pMArray());
        dHdT=0.5*(HT1-H0)/(DT);
        dVdT=0.5*(VT1-V0)/(DT);
        if (XFrac<0.5)
          {
          SetVapFrac(XFrac+DX);
          double PX1=Mdl.SaturationP(ApparentT);
          double VX1= Mdl.Volume(som_Gas, CurrentT, PX1, C.pMArray());
          double HX1= Mdl.totHf(som_ALL, CurrentT, PX1, C.pMArray());
          dHdX=0.5*(HX1-H0)/(DX);
          dVdX=0.5*(VX1-V0)/(DX);
          }
        else
          {
          SetVapFrac(XFrac-DX);
          double PX2=Mdl.SaturationP(ApparentT);
          double VX2= Mdl.Volume(som_Gas, CurrentT, PX2, C.pMArray());
          double HX2= Mdl.totHf(som_ALL, CurrentT, PX2, C.pMArray());
          dHdX=0.5*(H0-HX2)/(DX);
          dVdX=0.5*(V0-VX2)/(DX);
          }

        switch (C.Shape()->VolumeType())
          {
          case VVT_FixedV:
            {
            double A=1/NZ(dVdX-dHdX*dVdT/NZ(dHdT));
            double B=1/NZ(dHdX-dVdX*dHdT/NZ(dVdT));
            double C=1/NZ(dVdT-dHdT*dVdX/NZ(dHdX));
            double D=1/NZ(dHdT-dVdT*dHdX/NZ(dVdX));

            ReqdTChg=-Scl*(VErr*C+HErr*D);
            ReqdXChg=-Scl*(VErr*A+HErr*B);

            ReqdTChg=Range(-CurrentT*0.2, ReqdTChg, CurrentT*0.2);
            ReqdXChg=Range(-0.1, ReqdXChg, 0.1);

            double XErr=Range(-XFrac, ReqdXChg, 1.0-XFrac)-ReqdXChg;
            if (fabs(XErr)>1.0e-30)
              {
              ReqdTChg=-Scl*(XErr-HErr*B)*C/A+HErr*D;
              ReqdXChg+=XErr;
              int xxx=0;
              }
            //ReqdXChg=-Scl*(VErr/NZ(dVdX-dHdX*dVdT/NZ(dHdT))+HErr/NZ(dHdX-dVdX*dHdT/NZ(dVdT)));
            //ReqdTChg=-Scl*(VErr/NZ(dVdT-dHdT*dVdX/NZ(dHdX))+HErr/NZ(dHdT-dVdT*dHdX/NZ(dVdX)));
            //ReqdXChg=Range(Max(-0.1, -XFrac), ReqdXChg, Min(0.1, 1.0-XFrac));
            //ReqdTChg=Range(-CurrentT*0.2, ReqdTChg, CurrentT*0.2);
            }
            break;
            //case VVT_FunctP:
            //  AVT1=C.Shape()->ActualVolume(CurrentT+DT, CurrentP);
            //  AVP1=C.Shape()->ActualVolume(CurrentT, CurrentP+DP);
            //  AVT2=C.Shape()->ActualVolume(CurrentT-DT, CurrentP);
            //  AVP2=C.Shape()->ActualVolume(CurrentT, CurrentP-DP);
            //  dAVdT=0.5*(AVT1-AVT2)/(2*DT);
            //  dAVdP=0.5*(AVP1-AVP2)/(2*DP);
            //
            //  ReqdPChg=-Scl*(V0-VGas)/NZ((dVdP-dAVdP)-dHdP*(dVdT-dAVdT)/dHdT);
            //  ReqdTChg=-ReqdPChg*dHdP/dHdT;
            //  break;
            //case VVT_FixedP:
            //  DoBreak();
            //  ReqdPChg=-Scl*(V0-VGas)/NZ(dVdP-dHdP*dVdT/dHdT);
            //  ReqdTChg=-ReqdPChg*dHdP/dHdT;
            //  break;
          default:
            DoBreak();
          }


        if (!DoDampOsc)
          {
          DXHist[2]=DXHist[1];
          DXHist[1]=DXHist[0];
          DXHist[0]=ReqdXChg;
          DoDampOsc=(DXHist[2]*DXHist[1]<0 && DXHist[1]*DXHist[0]<0);
          }

#if dbgVLEBlock
        if (Loop>MaxLoops-10 || dbgQVFlash() || dbgQVFlashDbg() && DoDbgBrk())
          {
          int iH2O=SDB.Find("H2O(g)");
          //if (iH2O>=0)
          //  H2OFrac=M[iH2O]/GTZ(M.Sum());
          dbgp("%s %3i T:%12.5f P:%12.5f Vp:%12.5f dV:%12.5f dH:%12.5f "
            //"Vg:%12.5f V:%12.5f "
            "Stm:%12.9f "
            "[%s X:%+10g T:%+10g]",
            nSuperHt>0? "SuperHt": "Sat    ", Loop, K2C(CurrentT), CurrentP, Mdl.SaturationP(CurrentT),  
            VErr, HErr, 
            //VGas, V0, 
            XFrac, DoDampOsc?"#":" ", ReqdXChg, ReqdTChg);
          //if (0)
          //  dbgp(" dVdT:%8.4f dVdP:%8.4f dHdT:%8.4f dHdP:%8.4f",dVdT,dVdP,dHdT,dHdP);
          //if (C.Shape()->VolumeType()==VVT_FunctP) 
          //  dbgp(" dAVdT:%8.4f dAVdP:%8.4f",dAVdT,dAVdP);
          dbgpln(" %s",FullObjTag());
          dbgStarted=1;
          if (0 && Loop==50)
            {
            double P= CurrentP-20;
            double H1= Mdl.totHf(som_ALL, CurrentT, P, C.pMArray());

            for (int i=1; i<50; i++)
              {
              P+=40/50.0;
              double H2= Mdl.totHf(som_ALL, CurrentT, P, C.pMArray());
              dbgpln("P:%10.2f dH:%10.5f ",P,(H2-H1));
              H1=H2;
              }
            }
          if (Loop==MaxLoops)
            LogError(FullObjTag(), 0, "VFlash Not converged T:%g,P:%g",K2C(CurrentT),CurrentP-101.325);
          }
#endif

        XFrac+=ReqdXChg;
        SetVapFrac(Range(0.0,XFrac,1.0));

        CurrentT+=ReqdTChg;

        ApparentT=CurrentT-m_dApproachTemp;
        SaturationP=Mdl.SaturationP(ApparentT);

        if (XFrac>=1.0)
          nSuperHt++;
        else
          nSuperHt=0;
        CurrentP=SaturationP;
        }

      if (nSuperHt==0)
        C.SetTempPress(CurrentT, CurrentP);

      if (Loop<0)
        LogWarning(FullObjTag(), 0, "Pressure not converged");

      static int Count=0;
      if ((Count++<100) && (CurrentSpace>1.0e-6) && (fabs(V0-VGas)>1.0e-4*Max(VGas,V0)))
        dbgpln("Non Ideal Gas - Bad Pressure");
      }


#else
    int OK=true;

    double P0 = C.Press();

    double ChgMax = 0.01;
    VPQVFnd Fnd(C, *this, Ht+Duty);
    Fnd.SetTarget(0.0);

    if (0)
      {
      ////TODO "Experiment with minimum level..."
      //double MnLvl=C.MinLevel();
      //double L=GTZ(C.Level());
      //double HoldMass=Min(MnLvl, L)/L*GTZ(C.Mass(som_SL));
      //double MaxVf=1-HoldMass/GTZ(C.Mass(som_ALL));
      //Fnd.SetLimits(0.0, MaxVf);
      }
    else if (0)
      Fnd.SetLimits(0.0, 1.0);
    else
      {
      //TODO Implement intelligent limits for Brent for QVLSolve
      //todo: implement improved limits and ChgMax based on conditions, P, T, prev results, etc....
      //ChgMax = ??;
      if (P0>100.0)
        Fnd.SetLimits(0.0001, 0.99999);
      else
        Fnd.SetLimits(0.000001, 0.999999);
      }

    dPrvVapFrac=FlashVapFrac(*C.Model());

    ChgMax=Min(0.01, 0.45*fabs(dPrvVapFrac));
    switch (Fnd.SolveFromEst(dPrvVapFrac, 1.0, ChgMax))
      {
      case RF_OK:
        m_iState=VLES_TwoPhase;
        dPrvVapFrac=Fnd.Result();
        break;

      case RF_HiLimit:
        m_iState=VLES_SubCooled;
        dPrvVapFrac=Fnd.Result();
        break;

      case RF_LoLimit:
        m_iState=VLES_SuperHeated;
        dPrvVapFrac=Fnd.Result();
        break;

      default:
        m_iState=VLES_NotConverged;
        SetVapFrac(dPrvVapFrac);
        C.Set_totHf(Ht);

        // LogError(FullObjTag(), 0, "VFlash Not Converged");
        break;
      }
    SetCI(4, m_iState==VLES_NotConverged);
#endif  

    C.ClrHoldVentExpand(); 
    }

  };

// -------------------------------------------------------------------------

class QTsRootFnd : public MRootFinderBase
  {
  public:
    pSpModel pMdlL, pMdlV;
    CVLEVapPress &VLE;
    double TMn, TMx;//, P;
    static CToleranceBlock s_Tol;

    QTsRootFnd (pSpModel pMdlL_, pSpModel pMdlV_, CVLEVapPress &VLE_) :
    VLE(VLE_),
      MRootFinderBase("QTsRootFnd ", s_Tol)//1.0e-12)
      {
      pMdlL=pMdlL_;
      pMdlV=pMdlV_;
      //P=p;
      TMn=CDB.MinT;
      TMx=CDB.MaxT;
      SetErrorHandling(true, &pMdlL->m_fConvergeBusy);
      };

    LPCTSTR ObjTag() { return (LPCTSTR)pMdlL->FullObjTag(); };

    double Function(double P)
      {
      //??
      double Ts = pMdlL->SaturationT(P);
      pMdlL->SetTempPress(Ts, P);
      if (pMdlV)
        pMdlV->SetTempPress(Ts, P);
      double h = pMdlL->totHf();
      if (pMdlV)
        // Vapours ito Liq
        h+=pMdlL->totHf(som_ALL, pMdlV->Temp(), pMdlV->Press(), pMdlV->pMArray());
      static int xxx=0;
      if (xxx)
        {
        dbgpln("  QTsRoot\t%14.10f\t%14.6f\t%14.6f\t%14.6f\t%14.6g\t%14.6f", 
          K_2_C(Ts),VLE.Mt,P,dTarget,h-dTarget, pMdlL->totHf(som_ALL, Ts, pMdlL->SaturationP(Ts, 0, &VLE.FlashDefn()/*.FlashCmpIndex()*/), pMdlL->pMArray()));
        }
      return h;
      };
  };

CToleranceBlock QTsRootFnd::s_Tol(TBF_BothSys, "VLE:QTsRootFind", 0, 1.0e-12);

// --------------------------------------------------------------------------

static int QMVapFlashBrentErr=0;

void CVLEVapPress::MVapFlash(double MVap, double Duty, dword Flags)
  {
  pMdlL->SetHoldVentExpand();  
  if (pMdlV)
    pMdlV->SetHoldVentExpand();  


  double P0 = pMdlL->Press();
  double T0 = pMdlL->Temp();
  double HL = pMdlL->totHf(), HV=0;
  if (pMdlV)
    // Vapours ito Liq
    HV+=pMdlL->totHf(som_ALL, pMdlV->Temp(), pMdlV->Press(), pMdlV->pMArray());
  double HTrg = HL+HV+Duty;
  double VFInit=Vf;

  SetVapFrac(1.0);
  double Qv=pMdlL->Mass(som_Gas);
  if (pMdlV)
    Qv+=pMdlV->Mass(som_Gas);
  SetVapFrac(Range(0.0, MVap/GTZ(Qv), 1.0));

  QTsRootFnd QTsF(pMdlL, pMdlV, *this);
  QTsF.SetTarget(HTrg);

  double LoX = -1;
  double HiX = 2;

  double Press0=Range(1.0+10.0, m_dPrvVapPress, 10000.0-10.0);//Vf;
  //if (Press0<1.1) 
  //  Press0=10.0;
  QTsF.SetLimits(1.0, 10000.0);
  switch (QTsF.SolveFromEst(Press0, 1.0, 10.0))
    {
    case RF_OK: 
      m_dPrvVapPress=QTsF.Result(); 
      break;
    default:
      SetVapFrac(VFInit);
      pMdlL->Set_totHf(HL);
      pMdlL->SetPress(P0);
      if (pMdlV)
        {
        pMdlV->Set_totHf(HV);
        pMdlV->SetPress(P0);
        }
      if (QMVapFlashBrentErr++<5)
        dbgpln("CVLEBase - QTsF.Solve_Brent FAILED");
    }

  pMdlL->ClrHoldVentExpand();  
  if (pMdlV)
    pMdlV->ClrHoldVentExpand();  

  };

// -------------------------------------------------------------------------

void CVLEVapPress::MVapFlash(SpConduit &Q, double MVap, double Duty, dword Flags)
  { 
  if (Setup(Q.Model(), NULL)==VLES_OK)
    {
    MVapFlash(MVap, Duty, Flags);
    }
  };

// -------------------------------------------------------------------------

void CVLEVapPress::MVapFlash(SpConduit &Ql, SpConduit &Qv, double MVap, double Duty, dword Flags)
  { 
  if (Setup(Ql.Model(), Qv.Model())==VLES_OK)
    {
    MVapFlash(MVap, Duty, Flags);
    }
  };

// -------------------------------------------------------------------------

int CVLEVapPress::GetVLEData(CVLEDataArray & Array)
  {
  Array.SetSize(1);
  Array[0].iCmpNo=m_iCmpNo;
  Array[0].li=m_iLiqNo;
  Array[0].vi=m_iVapNo;
  return Array.GetSize();
  };

//--------------------------------------------------------------------------

double CVLEVapPress::CondensableFrac()
  { 
  if (m_iLiqNo>=0 && m_iVapNo>=0)
    {
    double Mv=pMdlL->m_M[m_iVapNo];
    if (pMdlV)
      Mv+=pMdlV->m_M[m_iVapNo];

    if (Mv>=UsableMass)
      {
      double Mvt=pMdlL->Mass(som_Gas);
      if (pMdlV)
        Mvt+=pMdlV->Mass(som_Gas);
      return Mv/GTZ(Mvt);
      }
    }
  return dNAN;
  };

//--------------------------------------------------------------------------

double CVLEVapPress::CondensableFrac(SpConduit *Ql, SpConduit *Qv) 
  { 
  if (Setup(Ql->Model(), Qv ? Qv->Model() : NULL)==VLES_OK)
    return CondensableFrac();
  return dNAN;
  };

//--------------------------------------------------------------------------

double CVLEVapPress::CondensableFrac(SpContainer*Cn) 
  { 
  if (Setup(Cn->Model(), NULL)==VLES_OK)
    return CondensableFrac();
  return dNAN;
  };

//--------------------------------------------------------------------------

double CVLEVapPress::SaturationP(SpModel *pMdl, double Temp, CSysVector * pMA)
  {
  if (pMdl==NULL)
    pMdl = pMdlL;
  if (m_iLiqNo>=0)
    return SDB[m_iLiqNo].VapourP(pMdl->Fidelity(), Temp);
  return dNAN;
  };

//--------------------------------------------------------------------------

double CVLEVapPress::SaturationTotalP(SpModel *pMdl, double Temp, CSysVector * pMA)
  {
  if (pMdl==NULL)
    pMdl = pMdlL;
  if (m_iLiqNo>=0)
    return SDB[m_iLiqNo].VapourP(pMdl->Fidelity(), Temp) + pMdl->PartialPress(-1, m_iVapNo, Temp);
  return dNAN;
  };

//--------------------------------------------------------------------------

flag CVLEVapPress::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case 1: pS="N\tVLE:Error";                    return 1; 
    case 2: pS="N\tVLE:Insufficient material";    return 1;
    case 4: pS="N\tVLE:Not Conconverged";         return 1;
    default:                                               
      return CVLEBlock::CIStrng(No, pS);
    }
  };


//============================================================================
//
//
//
//============================================================================

//IMPLEMENT_TAGOBJ(CVLERootK, CVLEBlock::GroupName, "CVLERootK", "", "", "Multi/Ideal K's", "Vapour Liquid Equilibrium Ideal K's");
CVLERootK::CVLERootK(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) : \
CVLEBlock(pClass_, TagIn, pAttach, eAttach),
//  Nd(*((FlwNode*)pAttach)),
n(Z)
  {
  pMdlL=NULL;
  pMdlV=NULL;

  for (int i=0; i<CDB.No(); i++)
    if (CDB[i].LiqPhInx("l", true)>=0 && CDB[i].VapPhInx("g", true)>=0)
      { 
      CVLEData C(i, 1.0, CDB[i].LiqPhInx("l", true), CDB[i].VapPhInx("g", true));
      Cmp.Add(C);
      }

    X.SetSize(NAvailComps());
    Y.SetSize(NAvailComps());
    Z.SetSize(NAvailComps());
    M.SetSize(NAvailComps());
    L.SetSize(NAvailComps());
    V.SetSize(NAvailComps());
    C.SetSize(NAvailComps());
    K.SetSize(NAvailComps());
    MvX.SetSize(NAvailComps());
    MlX.SetSize(NAvailComps());
    CData.SetSize(NAvailComps());
    CDefn.SetSize(NAvailComps());
    iState=VLES_CfgError;
    nComps=0;
    dPrvTemp=dNAN;//Std_T;
    dPrvPress=dNAN;//Std_P;
    dPrvVMoleFrac=0.1;
    Vf=0.0;
  };

//--------------------------------------------------------------------------

CVLERootK::~CVLERootK()
  {
  for (int i=0; i<CICount(); i++)
    ClrCI(i);
  };

//--------------------------------------------------------------------------

//void CVLERootK::BuildDataDefn(DataDefnBlk &DDB)
//  {
//  DDB.BeginStruct(this, "VLE", NULL, DDB_NoPage);
//
//
//  DDBValueLstMem DDB0;
//  DDB0.Empty();
//  DDB0.Add(VLES_Error,          "Error");
//  DDB0.Add(VLES_OK,             "OK");
//  DDB0.Add(VLES_SubCooled,      "Subcooled");
//  DDB0.Add(VLES_BubblePt,       "BubblePt");
//  DDB0.Add(VLES_TwoPhase,       "TwoPhase");
//  DDB0.Add(VLES_DewPt,          "DewPt");
//  DDB0.Add(VLES_SuperHeated,    "SuperHeated");
//  DDB.Int("", "State",    DC_,     "",    (int*)&m_iState, this, 0, DDB0());
//
//  DDB.BeginArray(this, "CMP", "VLEARRAY", Cmp.GetSize());
//  for (int i=0; i<Cmp.GetSize(); i++)
//    {
//    DDB.BeginElement(this, CDB[Cmp[i].iCmpNo].Name());
//    DDB.Double("K",     "",     DC_,    "",   &Cmp[i].dData, this, isParm);
//    }
//  DDB.EndArray();
//  DDB.EndStruct();
//  };

//--------------------------------------------------------------------------

flag CVLERootK::DataXchg(DataChangeBlk & DCB)
  {

  //  if (DCB.lHandle>=xidGSMSpcSplts &&  DCB.lHandle<xidGSMSpcSplts+SDB.No()*IOs.GetSize())
  //    {
  //    return 1;
  //    }
  //
  return 0;
  };

//--------------------------------------------------------------------------

flag CVLERootK::ValidateData(ValidateDataBlk & VDB)
  {
  for (int c=0; c<NAvailComps(); c++)
    if (Valid(Cmp[c].dData))
      Cmp[c].dData=Range(1.0e-8, Cmp[c].dData, 1.0e8);
  flag OK=1;

  iState=OK ? VLES_OK : VLES_CfgError;
  return OK;
  };

// -------------------------------------------------------------------------

VLEStates CVLERootK::Setup(SpModel *pMdlL_, SpModel *pMdlV_)
  { 
  pMdlL=pMdlL_;
  pMdlV=pMdlV_;
  // Initial Calcs
  Mt=0.0;
  Ht=pMdlL->totHf();
  if (pMdlV)
    // Vapours ito Liq
    Ht+=pMdlL->totHf(som_ALL, pMdlV->Temp(), pMdlV->Press(), pMdlV->pMArray());
  //Ht+=pMdlV->totHf();

  for (int c=0; c<NAvailComps(); c++)
    {
    CVLEData &C=Cmp[c];
    MlX[c]=(1.0-LMixFrac[C.li])*pMdlL->m_M[C.li];
    MvX[c]=(1.0-VMixFrac[C.vi])*pMdlL->m_M[C.vi];
    L[c]=LMixFrac[C.li]*pMdlL->m_M[C.li]/SDB[C.li].MoleWt();
    V[c]=VMixFrac[C.vi]*pMdlL->m_M[C.vi]/SDB[C.vi].MoleWt();
    if (pMdlV)
      {
      MlX[c]+=(1.0-LMixFrac[C.li])*pMdlL->m_M[C.li];
      MvX[c]+=(1.0-VMixFrac[C.vi])*pMdlL->m_M[C.vi];
      L[c]+=LMixFrac[C.li]*pMdlV->m_M[C.li]/SDB[C.li].MoleWt();
      V[c]+=VMixFrac[C.vi]*pMdlV->m_M[C.vi]/SDB[C.vi].MoleWt();
      }

    M[c]=L[c]+V[c];
    Mt+=M[c];
    }

  // Remove the Components which dont exist
  double MT=Mt; // Remember total;
  Lt=0.0;
  Vt=0.0;
  Mt=0.0;
  nComps=0;
  for (c=0; c<NAvailComps(); c++)
    {
    CVLEData &C=Cmp[c];
    if (M[c]/GTZ(MT)>1.0e-8)
      {
      K[nComps]=C.dData;
      L[nComps]=L[c];
      V[nComps]=V[c];
      M[nComps]=L[nComps]+V[nComps];
      Lt+=L[nComps];
      Vt+=V[nComps];
      Mt+=M[nComps];
      CData[nComps]=&Cmp[c];
      CDefn[nComps]=&CDB[C.iCmpNo];
      VSpIds.SetSize(Max(nComps+2, VSpIds.GetSize()));
      VSpIds[nComps]=C.vi;
      VSpIds[nComps+1]=-1;
      C.iIndex=nComps++;
      }
    else
      {
      C.iIndex=-1;
      C.dData=dNAN;
      }
    }

  iState=VLES_CfgError;
  if (nComps>1)
    {
    iState=VLES_OK;
    for (c=0; c<nComps; c++)
      {
      X[c]=L[c]/GTZ(Lt);
      Y[c]=V[c]/GTZ(Vt);
      n[c]=M[c]/Mt;
      }
    }
  Vf=Vt/GTZ(Mt);
  SetCI(2, nComps<=1);

  return iState;
  }

// -------------------------------------------------------------------------

//const double LnExpMax  = 690.0;
//
//void CVLERootK::IdealGasKRatio(double T, double P)
//  {
//  double ZC,TR,PSat;
//  P=Max(1.0, P);
//
////return ;
//
//  for (int c=0; c<nComps; c++)
////  for (i = 0; i < ZLen; i++) 
//    {
//    CVLEData &C=Cmp[c];
//    ZC = 0.29 - 0.065 * CDefn[c]->ACent();
//    TR = T/CDefn[c]->TCrit();
//
//    if (T > CDefn[c]->TCrit()) 
//      {
//      double ReducedLnPSat = (16.26-73.85*ZC+90.0*ZC*ZC) * (1.0-1.0/TR)-exp(log(10.0)*(-8.68*(TR-1.8+6.2*ZC)*(TR-1.8+6.2*ZC)));
//      ReducedLnPSat = Range(-LnExpMax, ReducedLnPSat, LnExpMax);
//      PSat = exp(ReducedLnPSat) * CDefn[c]->PCrit();
//      }
//    else   
//      {
//      PSat = exp((4.92 * CDefn[c]->ACent() + 5.81) * log(TR) - 0.0838 * (4.92 * CDefn[c]->ACent() + 2.06) * (36.0 / 
//              TR - 35.0 - (TR * TR * TR * TR * TR * TR) + 42.0 * 
//              log(TR))) * CDefn[c]->PCrit();
//      }
//    K[c] = PSat / P;
//    CData[c]->dData=K[c];
//    }
//  }

// -------------------------------------------------------------------------

VLEStates CVLERootK::TestState()
  {
  double f1=0.0;
  double f2=0.0;
  VLEStates iState=VLES_OK;
  for (int c=0; c<nComps; c++)
    {
    f1+=Z[c]*K[c];
    f2+=Z[c]/K[c];
    }
  if (f1<=1.0)
    iState=f1<(1.0-1.0e-12) ? VLES_SubCooled : VLES_BubblePt;
  else if (f2<1.0)
    iState=f2<(1.0-1.0e-12) ? VLES_SuperHeated : VLES_DewPt;
  else
    iState=VLES_TwoPhase;
  return iState;
  }

// -------------------------------------------------------------------------

double CVLERootK::BubblePt()
  { 
  double f=0.0;
  for (int c=0; c<nComps; c++)
    f+=X[c]*K[c];
  return f;
  }

// -------------------------------------------------------------------------

double CVLERootK::DewPt()
  { 
  double f=0.0;
  for (int c=0; c<nComps; c++)
    f+=Y[c]/K[c];
  return f;
  }

// -------------------------------------------------------------------------

double CVLERootK::SolveForLVRatio()
  {
  double Ri=Lt/GTZ(Vt);
  double R=Ri;
#if DoDbg2
  if (DoDbgBrk())
    dbgpln("SolveForLVRatio: L/V %18.9f/%18.9f  R:%18.9f", Lt, Vt, R);
#endif
  // Calculate C's
  double Damp=1.0;
  int WasNeg=false;
  double dR=0.0;
  for (int Iter=100;Iter;Iter--)
    {
    double E=0.0;
    double F=0.0;
    for (int c=0; c<nComps; c++)
      {
      C[c]=(M[c]*(K[c]-1.0)*(R+1.0))/(K[c]+R);
      E+=C[c];
      F+=(C[c]*C[c])/M[c];
      }


    if (fabs(E)<1.0e-8)
      return R;

    double Rp=(F*R-E*(R+1.0))/(F+E*(R+1.0));

#if DoDbg2
    double RTmp=R;
#endif

    if (dR*(R-Rp)<0.0)
      Damp*=0.8;
    dR=(R-Rp);

    if (Rp<0.0)
      {
      R=0.5*R;
      if (!WasNeg)
        Damp=Max(Damp*0.2, 0.1);
      WasNeg=true;
      }
    else
      {
      WasNeg=false;
      Damp=Max(Damp, 0.01);
      R=Damp*Rp+(1-Damp)*R;
      }
#if DoDbg2
    if (DoDbgBrk())
      dbgpln("R:  %18.9f > %18.9f", RTmp, R);
#endif
    }
  if (Iter==0)
    LogError(FullObjTag(),0,"VLE not Converged");
  //    }
  //SetCI(4, Iter==0);
  return R;
  }

// -------------------------------------------------------------------------

void CVLERootK::SetLVRatio(double R)
  {
  Lt=0.0;
  Vt=0.0;
  for (int c=0; c<nComps; c++)
    {
    //    L[c]=M[c]*Range(0.0, R/(K[c]+R), 1.0);
    L[c]=M[c]*Range(1.0e-10, R/(K[c]+R), 1.0-1.0e-10);
    V[c]=M[c]-L[c];
    Lt+=L[c];
    Vt+=V[c];
    }
  }

// -------------------------------------------------------------------------

void CVLERootK::SetAllVapour()//SpModel &Mdl)
  {
  for (int c=0; c<nComps; c++)
    {
    L[c]=0.0;
    V[c]=M[c];
    }
  }

// -------------------------------------------------------------------------

void CVLERootK::SetAllLiquid()//SpModel &Mdl)
  {
  for (int c=0; c<nComps; c++)
    {
    L[c]=M[c];
    V[c]=0.0;
    }
  }

// -------------------------------------------------------------------------

void CVLERootK::SetMass()///SpModel &Mdl)
  {
  for (int c=0; c<NAvailComps(); c++)
    {
    CVLEData &C=Cmp[c];
    if (C.iIndex>=0)
      {
      pMdlL->VValue[C.li] = MlX[c]+L[C.iIndex]*SDB[C.li].MoleWt();
      if (pMdlV)
        {
        pMdlL->VValue[C.vi] = 0.0;
        pMdlV->VValue[C.li] = 0.0;
        pMdlV->VValue[C.vi] = MvX[c]+V[C.iIndex]*SDB[C.vi].MoleWt();
        }
      else
        pMdlL->VValue[C.vi] = MvX[c]+V[C.iIndex]*SDB[C.vi].MoleWt();
      }
    }
  }

// -------------------------------------------------------------------------

LPTSTR CVLERootK::FlashDescription()
  {
  m_sFlashDesc="VLERootK";
  return (LPTSTR)(LPCTSTR)m_sFlashDesc();
  };

// -------------------------------------------------------------------------

double CVLERootK::FlashVapFrac(SpModel &Mdl)
  { 
  if (Setup(&Mdl, NULL)==VLES_OK)
    return Vf;
  return 0.0;
  };

// -------------------------------------------------------------------------

void CVLERootK::SetFlashVapFrac(SpModel &Mdl, double Temp, double Press, double VapFrac, dword Flags)
  { 
  // DoBreak();
  //SetKRatios(Temp, Press);
  if (Setup(&Mdl, NULL)==VLES_OK)
    {
    SetKRatios(Temp, Press);
    //switch (iState=TestState())
    //  {
    //  case VLES_SubCooled:
    //    SetAllLiquid();
    //    break;
    //  case VLES_SuperHeated:
    //    SetAllVapour();
    //    break;
    //  case VLES_TwoPhase:
    if (VapFrac>0.999999)
      SetAllVapour();
    else if (VapFrac<0.000001)
      SetAllLiquid();
    else
      SetLVRatio(1.0/VapFrac-1.0);
    //    break;
    //  }
    SetMass();
    pMdlL->SetTempPress(Temp, Press);
    }
  };

// -------------------------------------------------------------------------

void CVLERootK::TPFlash(SpModel &Mdl, double Temp, double Press, dword Flags)
  { 
  SetKRatios(Temp, Press);
  if (Setup(&Mdl, NULL)==VLES_OK)
    {
    SetKRatios(Temp, Press);
    switch (iState=TestState())
      {
      case VLES_SubCooled:   SetAllLiquid(); break;
      case VLES_SuperHeated: SetAllVapour(); break;
      case VLES_TwoPhase:    SetLVRatio(SolveForLVRatio()); break;
      }
    SetMass();
    Mdl.SetTempPress(Temp, Press);
    }
  };

// -------------------------------------------------------------------------
//
//
//
// -------------------------------------------------------------------------

double CVLERootK::QPFlashFun(double T, double P)
  {
  SetKRatios(T, P);
  switch (iState=TestState())
    {
    case VLES_SubCooled:   SetAllLiquid(); break;
    case VLES_SuperHeated: SetAllVapour(); break;
    case VLES_TwoPhase:    SetLVRatio(SolveForLVRatio()); break;
    }

  SetMass();
  pMdlL->SetTempPress(T, P);
  if (pMdlV)
    pMdlV->SetTempPress(T, P);
  double H=pMdlL->totHf(som_ALL, T, P, pMdlL->pMArray());
  if (pMdlV)
    // Vapours ito Liq
    H+=pMdlL->totHf(som_ALL, pMdlV->Temp(), pMdlV->Press(), pMdlV->pMArray());
  //H+=pMdlV->totHf(som_ALL, T, P, pMdlV->pMArray());
  return H;
  }

// -------------------------------------------------------------------------

class QPFnd : public MRootFinderBase
  {
  public:
    //    SpModel &Mdl;
    CVLERootK & VLE;
    double P, Ht;
    static CToleranceBlock s_Tol;
    QPFnd(CVLERootK * pVLE, double P_, double Ht_) : 
    MRootFinderBase("QPFnd", s_Tol),//1.0e-9),
      //Mdl(*pMdl_),
      VLE(*pVLE)
      { 
      P=P_; Ht=Ht_;
      };
    LPCTSTR ObjTag() { return (LPCTSTR)VLE.pMdlL->FullObjTag(); };
    double Function(double x) 
      { 
      return VLE.QPFlashFun(x, P)-Ht; 
      };
  };

CToleranceBlock QPFnd::s_Tol(TBF_BothSys, "VLE:QPFind", 0, 1.0e-9);

// -------------------------------------------------------------------------

void CVLERootK::PFlash(SpConduit &Q, double Press, double Duty, dword Flags)
  { 
  if (Setup(Q.Model(), NULL)==VLES_OK)
    {
    SpModel &Mdl=*Q.Model();
    double Temp=Valid(dPrvTemp) ? dPrvTemp : Q.Temp();

    QPFnd QP(this, Press, Ht+Duty);
    QP.SetTarget(0.0);
    QP.SetLimits(Std_T-100.0, 1000.0);
    if (QP.SolveFromEst(Temp, 1.0, 50.0)==RF_OK)
      {
      dPrvTemp=QP.Result();
      ClrCI(4);
      }
    else
      {
      SetCI(4);
      //LogError(FullObjTag(), 0, "PFlash Not Converged");
      };
    }
  };

// -------------------------------------------------------------------------

void CVLERootK::PFlash(SpConduit &Ql, SpConduit &Qv, double Press, double Duty, dword Flags)
  { 
  //  DoBreak();

  if (Setup(Ql.Model(), Qv.Model())==VLES_OK)
    {
    //SpModel &MdlL=*Ql.Model();
    //SpModel &MdlV=*Qv.Model();
    double Temp=Valid(dPrvTemp) ? dPrvTemp : Ql.Temp();
    QPFnd QP(this, Press, Ht+Duty);
    QP.SetTarget(0.0);
    QP.SetLimits(Std_T-100.0, 1000.0);
    if (QP.SolveFromEst(Temp, 1.0, 50.0)==RF_OK)
      {
      dPrvTemp=QP.Result();
      ClrCI(4);
      }
    else
      SetCI(4);
    //LogError(FullObjTag(), 0, "PFlash Not Converged");

    }

  };

// -------------------------------------------------------------------------

void CVLERootK::PFlash(SpContainer &C, double Press, double Duty, dword Flags)
  { 
  if (Setup(C.Model(), NULL)==VLES_OK)
    {
    SpModel &Mdl=*C.Model();
    double Temp=Valid(dPrvTemp) ? dPrvTemp : C.Temp();

    QPFnd QP(this, Press, Ht+Duty);
    QP.SetTarget(0.0);
    QP.SetLimits(Std_T-100.0, 1000.0);
    if (QP.SolveFromEst(Temp, 1.0, 50.0)==RF_OK)
      {
      dPrvTemp=QP.Result();
      ClrCI(4);
      }
    else
      {
      SetCI(4);
      //LogError(FullObjTag(), 0, "PFlash Not Converged");
      };
    }
  };

// -------------------------------------------------------------------------
//
//
//
// -------------------------------------------------------------------------
/***
double CVLERootK::QVFlashFun1(double T, double P)
{
SetKRatios(T, P);
//dbgp("",T,P);
switch (iState=TestState())
{
case VLES_SubCooled:   
case VLES_BubblePt:
SetAllLiquid(); 
#if DoDbg1
if (DoDbgBrk())
dbgp("  QVF1 T:%14.4f P:%14.4f %3i %-18.18s",T,P, iState, "Liquid");
#endif
break;
case VLES_SuperHeated: 
case VLES_DewPt:
SetAllVapour(); 
#if DoDbg1
if (DoDbgBrk())
dbgp("  QVF1 T:%14.4f P:%14.4f %3i %-18.18s",T,P, iState, "Vapour");
#endif
break;
case VLES_TwoPhase:    
{
double R=SolveForLVRatio(); 
SetLVRatio(R); 
#if DoDbg1
if (DoDbgBrk())
dbgp("  QVF1 T:%14.4f P:%14.4f %3i %18.9f",T,P, iState, R);
#endif
break;
}
}

SetMass();
pMdlL->SetTemp(T);
double H=pMdlL->totHf(T, P, pMdlL->pMArray());
if (pMdlV)
{
pMdlV->SetTemp(T);
H+=pMdlV->totHf(T, P, pMdlV->pMArray());
}
#if DoDbg1
if (DoDbgBrk())
dbgpln(" H:%14.4e", H);
#endif
return H;
}

// -------------------------------------------------------------------------

class QFnd1 : public MRootFinderBase
{
public:
SpModel &Mdl;
CVLERootK & VLE;
double P, Ht;
QFnd1(pSpModel pMdl_, CVLERootK * pVLE, double P_, double Ht_) : 
MRootFinderBase("QFnd1", 1.0e-9),
Mdl(*pMdl_),
VLE(*pVLE)
{ 
P=P_; Ht=Ht_;
};
char * ObjTag() { return Mdl.FullObjTag(); };  
double Function(double x) 
{ 
return VLE.QVFlashFun1(x, P)-Ht; 
};
};

// -------------------------------------------------------------------------

class QFnd2 : public MRootFinderBase
{
public:
SpModel &Mdl;
CVLERootK & VLE;
double Ht;
QFnd2(pSpModel pMdl_, CVLERootK * pVLE, double Ht_) : 
MRootFinderBase("QFnd2", 1.0e-7),
Mdl(*pMdl_),
VLE(*pVLE)
{ 
Ht=Ht_;
};
char * ObjTag() { return Mdl.FullObjTag(); };
double Function(double PEst) 
{ 
double Temp=Valid(VLE.dPrvTemp) ? VLE.dPrvTemp : Mdl.Temp();

//#pragma chMSG(Temporary Temp Offset)
//Temp += 10;

QFnd1 QP(&Mdl, &VLE, PEst, Ht);
QP.SetTarget(0.0);
QP.SetLimits(Std_T-200.0, 10000.0);
if (QP.SolveFromEst(Temp, 1.0, 5.0)==RF_OK)
{
VLE.dPrvTemp=Mdl.Temp();
Mdl.ClrVStatesOK();
return Mdl.Press()-PEst;
}
else
{
DoBreak();
return 0.0;
}
};
};

// -------------------------------------------------------------------------

void CVLERootK::VFlash(SpContainer &C, double Duty, dword Flags)
{ 
if (Setup(C.Model(), NULL)==VLES_OK)
{
SpModel &Mdl=*C.Model();
double TMoles=0.0;
for (int c=0; c<nComps; c++)
TMoles+=M[c];
if (TMoles>1.0e-10)
{
int OK=false;
double PEst=Valid(dPrvPress) ? dPrvPress : C.Press();

//#pragma chMSG(Temporary Press Offset)
//PEst-=100.0;

#if DoDbg1
if (DoDbgBrk())
dbgpln("VFlash %14.4e", Ht+Duty);
#endif

QFnd2 QP(&Mdl, this, Ht+Duty);
QP.SetTarget(0.0);
QP.SetLimits(1.0, 100000.0);
if (QP.SolveFromEst(PEst, 1.0,  50.0)==RF_OK)
{
dPrvPress=QP.Result();
OK = true;
}
//if (!OK)
//  LogError(FullObjTag(), 0, "VFlash Not Converged");
SetCI(4, !OK);
#if DoDbg1
if (DoDbgBrk())
dbgpln(" ");
#endif
}
}
};
**/

void CVLERootK::VFlash(SpContainer &C, double Duty, dword Flags)
  { 
  if (Setup(C.Model(), NULL)==VLES_OK)
    {
    SpModel &Mdl=*C.Model();
    double TMoles=0.0;
    for (int c=0; c<nComps; c++)
      TMoles+=M[c];
    if (TMoles>1.0e-10)
      {
      flag OK=false;
      flag VLRatOK=true;
      double PEst=Valid(dPrvPress) ? dPrvPress : C.Press();

      //#pragma chMSG(Temporary Press Offset)
      //PEst-=100.0;

#if DoDbg1
      if (DoDbgBrk())
        dbgpln("    H: %14.4e", Ht+Duty);
#endif

      double TEst=C.Temp();
      double LVEst=Lt/Max(1.0e-100, Vt);
      SetKRatios(TEst, PEst);
      int MaxI=1000;
      LVEst=Range(1e-20,LVEst,1e20);
      for (int i=0; i<MaxI; i++)
        {
        SetLVRatio(LVEst); 
        SetMass();
        C.Set_totHf(Ht+Duty);

        double PMeas=C.Press();
        double TMeas=C.Temp();

        SetKRatios(TMeas, PMeas);
        double LVNew=Lt/Max(1.0e-100, Vt);
        if (!InRange(1e-20,LVNew,1e20))
          {
          VLRatOK=false;
          break;
          }
        if (i>=2 && ConvergedVV(LVEst, LVNew, 1e-10, 1e-10))
          break;
        LVEst=LVNew;
#if DoDbg1
        if (DoDbgBrk())
          {
          dbgp("    T:%14.8f P:%14.8f LV:%20.10g H:%14.4e",TMeas,PMeas,LVEst, Ht+Duty);
          for (int c=0; c<nComps; c++)
            dbgp(" Ki:%14.8f",K[c]);
          dbgpln("");
          }
#endif
        int xxx=0;
        if (i>990)
          {
          int yyy=0;
          }
        }
      SetCI(5, !VLRatOK);
      OK=i<MaxI;
      /*
      for (int i=100; i; i--)
      {
      SetKRatios(TEst, PEst);
      switch (iState=TestState())
      {
      case VLES_SubCooled:   SetAllLiquid(); break;
      case VLES_SuperHeated: SetAllVapour(); break;
      case VLES_TwoPhase:    SetLVRatio(SolveForLVRatio()); break;
      }
      SetMass();
      double PMeas=C.Press();
      double HMeas=Mdl.totHf(TEst, PEst, Mdl.pMArray());
      double HErr=HMeas-(Ht+Duty);
      int xxx=0;
      }
      */
      //      Mdl.SetTemp(Temp);
      //      Mdl.SetPress(Press);

      //      QFnd2 QP(&Mdl, this, Ht+Duty);
      //      QP.SetTarget(0.0);
      //      QP.SetLimits(1.0, 100000.0);
      //      if (QP.SolveFromEst(PEst, 1.0,  50.0)==RF_OK)
      //        {
      //        dPrvPress=QP.Result();
      //        OK = true;
      //        }
      //      //if (!OK)
      //      //  LogError(FullObjTag(), 0, "VFlash Not Converged");
      SetCI(4, !OK);
#if DoDbg1
      if (DoDbgBrk())
        dbgpln(" ");
#endif
      }
    }
  };

// -------------------------------------------------------------------------

void CVLERootK::MVapFlash(SpConduit &Q, double MVap, double Duty, dword Flags)
  { 
  DoBreak();
  };

// -------------------------------------------------------------------------

void CVLERootK::MVapFlash(SpConduit &Ql, SpConduit &Qv, double MVap, double Duty, dword Flags)
  { 
  DoBreak();
  };

// -------------------------------------------------------------------------

int CVLERootK::GetVLEData(CVLEDataArray & Array)
  {
  Array.SetSize(Cmp.GetSize());
  for (int i=0; i<Array.GetSize(); i++)
    Array[i]=Cmp[i];
  return Array.GetSize();
  };

// -------------------------------------------------------------------------
//
//
//
//--------------------------------------------------------------------------

flag CVLERootK::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case 1: pS="N\tVLE:Error";                      return 1;
    case 2: pS="N\tVLE:Insufficient components";    return 1;
    case 3: pS="N\tVLE:Too many components";        return 1;
    case 4: pS="N\tVLE:Not Converged";              return 1;
    case 5: pS="N\tVLE:LVRatio Limited";            return 1;
    case 6: pS="E\tVLE:Missing Data";               return 1;
    default:                                               
      return CVLEBlock::CIStrng(No, pS);
    }
  };

//============================================================================
//
//
//
//============================================================================
//
// Based on the paper 'Equilibrium flash calcs...' by A.K.Coker Oil & Gas Journal Jan 14 1991

/*#D:#T:Multi/Fixed K
#X:#h<General Description>#n??? Vapour Liquid Equilibrium Fixed K ???
#n#n
#n#h<Refrences>#n
Based on the paper 'Equilibrium flash calcs...' by A.K.Coker Oil & Gas Journal Jan 14 1991
#n#n
#n#h<Variables to be supplied by the user>#n
#i<State> : #n
#i<???> : #n
#n
#n#h<Other>#n
Short name:CVLEFixdK#n
Model type:Vapour Liquid Equilibrium Logic#n
#G:Vapour Liquid Equilibrium Logic
*/

IMPLEMENT_TAGOBJSLCT(CVLEFixdK, CVLEBlock::GroupName, "CVLEFixdK", "", "", "", TOC_SYSTEM, "Multi/Fixed K", 
                     "Vapour Liquid Equilibrium Fixed K",
                     VLEF_VfFlash |VLEF_TPFlash | VLEF_QPFlash /*| VLEF_QVFlash*/);
CVLEFixdK::CVLEFixdK(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
CVLERootK(pClass_, TagIn, pAttach, eAttach)
  {
  };

//--------------------------------------------------------------------------

CVLEFixdK::~CVLEFixdK()
  {
  };

//--------------------------------------------------------------------------

void CVLEFixdK::BuildDataDefn(DataDefnBlk &DDB)
  {
  if (DDB.BeginStruct(this))
    {
    DDB.Long("", "State",    DC_,     "",    (long*)&iState, this, 0, DDBVLEStates);

    if (PrjFileVerNo()<22)
      {
      Strng Sym;
      for (int i=0; i<Cmp.GetSize(); i++)
        {
        ASSERT(CDB[Cmp[i].iCmpNo].Sym());
        //Sym.Set("%s.K", CDB[Cmp[i].iCmpNo].Sym());
        Sym=CDB[Cmp[i].iCmpNo].Sym + ".K";
        DDB.Double("", Sym(), DC_Frac, "%", &Cmp[i].dData, this, isParm);
        }
      }
    else
      {
      if (DDB.BeginObject(this, "K", "VLE_K", NULL, DDB_NoPage))
        {
        for (int i=0; i<Cmp.GetSize(); i++)
          //DDB.Double("", CDB[Cmp[i].iCmpNo].Sym(), DC_Frac, "%", &Cmp[i].dData, this, isParm|NAN_OK);
          DDB.Double("", CDB[Cmp[i].iCmpNo].Sym(), DC_, "", &Cmp[i].dData, this, isParm|NAN_OK);
        }
      DDB.EndObject();
      }
    }
  DDB.EndStruct();
  };

//--------------------------------------------------------------------------

void CVLEFixdK::SetKRatios(double T, double P)
  {
  // Do Nothing K's fixed
  };

//============================================================================
//
//
//
//============================================================================

/*#D:#T:Multi/Ideal K's
#X:#h<General Description>#n??? Vapour Liquid Equilibrium Ideal K's ???
#n#n
#n#h<Refrences>#n
Based on the paper 'Equilibrium flash calcs...' by A.K.Coker Oil & Gas Journal Jan 14 1991
#n#n
#n#h<Variables to be supplied by the user>#n
#i<State> : #n
#i<???> : #n
#n
#n#h<Other>#n
Short name:CVLEIdealK#n
Model type:Vapour Liquid Equilibrium Logic#n
#G:Vapour Liquid Equilibrium Logic
*/

IMPLEMENT_TAGOBJSLCT(CVLEIdealK, CVLEBlock::GroupName, "CVLEIdealK", "", "", "", TOC_SYSTEM, "Multi/Ideal K's", 
                     "Vapour Liquid Equilibrium Ideal K's",
                     VLEF_VfFlash | VLEF_TPFlash | VLEF_QPFlash | VLEF_QVFlash);
CVLEIdealK::CVLEIdealK(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
CVLERootK(pClass_, TagIn, pAttach, eAttach)
  {
  };

//--------------------------------------------------------------------------

CVLEIdealK::~CVLEIdealK()
  {
  };

//--------------------------------------------------------------------------

void CVLEIdealK::BuildDataDefn(DataDefnBlk &DDB)
  {
  if (DDB.BeginStruct(this))
    {
    DDB.Long("", "State",    DC_,     "",    (long*)&iState, this, 0, DDBVLEStates);

    if (PrjFileVerNo()<22)
      {
      Strng Sym;
      for (int i=0; i<Cmp.GetSize(); i++)
        {
        ASSERT(CDB[Cmp[i].iCmpNo].Sym());
        //Sym.Set("%s.K", CDB[Cmp[i].iCmpNo].Sym());
        Sym= CDB[Cmp[i].iCmpNo].Sym + ".K";
        DDB.Double("", Sym(), DC_Frac, "%", &Cmp[i].dData, this, isParm);
        }
      }
    else
      {
      if (DDB.BeginObject(this, "K", "VLE_K", NULL, DDB_NoPage))
        {
        for (int i=0; i<Cmp.GetSize(); i++)
          //DDB.Double("", CDB[Cmp[i].iCmpNo].Sym(), DC_Frac, "%", &Cmp[i].dData, this, isParm|NAN_OK);
          DDB.Double("", CDB[Cmp[i].iCmpNo].Sym(), DC_, "", &Cmp[i].dData, this, 0);
        }
      DDB.EndObject();
      }
    }
  DDB.EndStruct();
  };

//--------------------------------------------------------------------------

void CVLEIdealK::SetKRatios(double T, double P)
  {
  // Ideal K's
  const double LnExpMax  = 690.0;

  double ZC,TR,PSat;
  P=Max(1.0, P);

  flag BadData=false;
  for (int c=0; c<nComps; c++)
    {
    CVLEData &C=Cmp[c];
    CComponent &Cd=*CDefn[c];
    ZC = 0.29 - 0.065 * Cd.ACent();
    TR = T/Cd.TCrit();

    if (Cd.AcOK() && Cd.PcOK() && Cd.TcOK())
      {
      if (T > Cd.TCrit()) 
        {
        double ReducedLnPSat = (16.26-73.85*ZC+90.0*ZC*ZC) * (1.0-1.0/TR)-exp(log(10.0)*(-8.68*(TR-1.8+6.2*ZC)*(TR-1.8+6.2*ZC)));
        ReducedLnPSat = Range(-LnExpMax, ReducedLnPSat, LnExpMax);
        PSat = exp(ReducedLnPSat) * Cd.PCrit();
        }
      else   
        {
        PSat = exp((4.92 * Cd.ACent() + 5.81) * log(TR) - 0.0838 * (4.92 * Cd.ACent() + 2.06) * (36.0 / 
          TR - 35.0 - (TR * TR * TR * TR * TR * TR) + 42.0 * 
          log(TR))) * Cd.PCrit();
        }
      K[c] = PSat / P;
      }
    else
      {
      K[c]=1.0e6;
      BadData=true;
      }

    CData[c]->dData=K[c];
    }
  SetCI(6, BadData);
  }

//============================================================================
//
//
//
//============================================================================
//
//flag SpContPresetVLE::ValidateData(ValidateDataBlk & VDB, SpContainer & Cn)
//  {
//  bTAdj=!bDoVLE || !Cn.Closed() || !m_pVLE->Enabled() || m_pVLE->HasTPEnvelope();
//  return CSpContPreset::ValidateData(VDB, Cn);
//  }
//
//flag SpContPresetVLE::AdjustConditions(SpContainer & Cn)
//  {
//  if (bDoVLE)
//    {
//    m_pVLE->VFlash(Cn, 0.0, VLEF_Null);
//    return m_pVLE->Enabled();
//    }
//  return false;
//  }
//
//============================================================================
//
//
//
//============================================================================
