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
static CDbgMngr dbgQVFlash       ("VLEMdl",  "QVFlash");
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

static DDBValueLst DDBVLEStates[]={
  {VLES_Error,          "Error"},
  {VLES_OK,             "OK"},
  {VLES_SubCooled,      "SubCooled"},
  {VLES_BubblePt,       "BubblePt"},
  {VLES_TwoPhase,       "TwoPhase"},
  {VLES_DewPt,          "DewPt"},
  {VLES_SuperHeated,    "SuperHeated"},
  {VLES_NotConverged,   "Not_Converged"},
  {0}};

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
  if (CDB[CompNo].VapPhInx("g")>=0)
    VMixFrac[CDB[CompNo].VapPhInx("g")]=dVMixFrac;
  if (CDB[CompNo].LiqPhInx("l")>=0)
    LMixFrac[CDB[CompNo].LiqPhInx("l")]=dLMixFrac;
  };

//============================================================================
//
//
//
//============================================================================

#if WITHVLETIMERS 
CGlblStopWatch CVLEBase::sm_swTPFlash(gs_swFlwSolve,     "VLE", "TPFlash");
CGlblStopWatch CVLEBase::sm_swQPFlash(gs_swFlwSolve,     "VLE", "QPFlash");
CGlblStopWatch CVLEBase::sm_swQVFlash(gs_swFlwSolve,     "VLE", "QVFlash");
CGlblStopWatch CVLEBase::sm_swQVDerivs(gs_swFlwSolve,    "VLE", "QVDerivs");
CGlblStopWatch CVLEBase::sm_swQMVapFlash(gs_swFlwSolve,  "VLE", "QMVapFlash");
#endif

flag CVLEBase::Open(TagObjClass * pVLEClass, flag Fixed)
  {
  //dbgpln("CVLEBase::Open %s", pNd->FullObjTag());

  fEnabled=true;
  
  if (pVLE && (pVLEClass==NULL || pVLE->Class()==pVLEClass))
    return true;

  delete pVLE;
  fFixed=Fixed;

  if (pVLEClass)
    pVLE=(CVLEBlock*)pVLEClass->Construct(NULL, "VLE", pNd, TOA_Embedded);
  else
    pVLE=(CVLEBlock*)CVLEVapPressClass.Construct(NULL, "VLE", pNd, TOA_Embedded);
    //pVLE=(CVLEBlock*)VLEIdealKClass.Construct(NULL, "VLE", pNd, TOA_Embedded);
  pNd->StructureChanged(NULL);

  fEnabled=pVLE!=NULL;
  return fEnabled;
  };

//--------------------------------------------------------------------------

void CVLEBase::Close()
  {
  //dbgpln("CVLEBase::Close %s", pNd->FullObjTag());
  if (fFixed)
    return;
  fEnabled=false;
// ?? Why were these two line commented out
  delete pVLE;
  pVLE=NULL;
  };

//--------------------------------------------------------------------------

void CVLEBase::Add_OnOff(DataDefnBlk &DDB, dword Flags)
  {
  DDB.Byte("VLEquilibrium",      "",       DC_    , "",      xidVLEEnable, pNd,  (fFixed ? 0 : Flags), GetOnOffValLst());
  }

//--------------------------------------------------------------------------

void CVLEBase::BuildDataDefn(DataDefnBlk &DDB, pchar pTag_, pchar pTagComment, DDBPages PageIs, dword UserInfo)
  {
  if (pTag_==NULL)
    pTag_="VLE";
  DDEF_Flags Old=DDB.GetVisibility();
  DDB.Visibility(NSHM_All, fEnabled);

  //if (DDB.BeginObject(pNd, pTag_, "VLEBlk", pTagComment, PageIs))
  if (DDB.BeginObject(pNd, pTag_, "VLE_Cfg", pTagComment, PageIs))
    {
    DDBValueLstMem DDB0;
    TagObjClass::GetSDescValueLst(CVLEBlock::GroupName, DDB0, NULL, dwSelectMask);
    DDB.String  ("Model",      "",       DC_    , "",      xidVLEEqnNm  , pNd,fFixed ? 0 : isParm|SetOnChange, DDB0());
    if (pVLE)
      {
      DDB.SuppressNextBlock();
      pVLE->BuildDataDefn(DDB);
      }
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
        if (DCB.rpC && !fFixed)
          {
          int WasEnabled=fEnabled;
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
        DCB.pC=pVLE ? pVLE->ShortDesc() : "Off"; 
        return 1;
        }
    }

  // Actual Object will be DataXchd via list
  //if (pVLE && pVLE->DataXchg(DCB))
  //  return 1;
  
  return 0;//return pGSM ? pGSM->DataXchg(DCB) : 0; 
  }

//--------------------------------------------------------------------------

flag CVLEBase::ValidateData(ValidateDataBlk & VDB)
  {
  return true;
  };

//void CVLEBase::SensibleHeatIn(bool ClearIt, SpModel & Mdl)
//  {
//  if (ClearIt)
//    m_SensHtIn=0;
//  //m_SensHtIn-=Mdl.totHs();
//  m_SensHtIn+=Mdl.totHf(som_ALL, C2K(0), StdP);
//  };
//void CVLEBase::SensibleHeatOut(SpModel & Mdl) 
//  {
//  //m_SensHtIn+=Mdl.totHs();
//  m_SensHtIn-=Mdl.totHf(som_ALL, C2K(0), StdP);
//  };

//--------------------------------------------------------------------------

double CVLEBase::FlashVapFrac(SpModel &Mdl)
  {
  return Enabled() ? pVLE->FlashVapFrac(Mdl) : 0.0;
  };

//--------------------------------------------------------------------------

flag CVLEBase::HasTPEnvelope()
  {
  return Enabled() ? pVLE->HasTPEnvelope() : false;
  };

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

    pVLE->SetFlashVapFrac(Mdl, Temp, Press, VapFrac, Flags);

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
    pVLE->SetFlashVapFrac(Mdl, Mdl.Temp(), Mdl.Press(), VapFrac, Flags);
    Mdl.Set_totHf(H);

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

    pVLE->TPFlash(Mdl, Temp, Press, Flags);

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

void CVLEBase::QPFlash(SpConduit &Q, double Press, double Duty, dword Flags)
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

    pVLE->QPFlash(Q, Press, Duty, Flags);

    #if DoDbg
    double T1=Q.Temp();
    double P1=Q.Press();
    double H1=Q.totHf();
    double MV1=Q.QMass(som_Gas);
    double MSL1=Q.QMass(som_SL);
    if (Q.pAttachedTo && Q.pAttachedTo->DoDbgBrk())
      {
      dbgpln("QPFlash:%s",Q.FullObjTag());
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

void CVLEBase::QPFlash(SpConduit &Ql, SpConduit &Qv, double Press, double Duty, dword Flags)
  {
  if (Enabled()) 
    {
    VLETIMERLAP(sm_swQPFlash);
    #if DoDbg
    double T0=Ql.Temp();
    double P0=Ql.Press();
    double H0=Ql.totHf();
    double MV0=Ql.QMass(som_Gas);
    double MSL0=Ql.QMass(som_SL);
    SpMArray XXX(*Ql.Model());
//    int dbg=Ql.DoDbgBrk();
    int dbg=pNd && pNd->DoDbgBrk();
    if (dbg)
      {
      dbgpln("QPFlash:%s",Ql.FullObjTag());
      Flags |= VLEF_WithDebug;
      }
    #endif

    pVLE->QPFlash(Ql, Qv, Press, Duty, Flags);

    #if DoDbg
    double T1=Ql.Temp();
    double P1=Ql.Press();
    double H1=Ql.totHf();
    double MV1=Qv.QMass(som_Gas);
    double MSL1=Ql.QMass(som_SL);
    if (dbg)
      {
      //dbgpln("QPFlash:%s",Ql.FullObjTag());
//      dbgpln("  T:%+12.4g %12.6f > %12.6f", T1-T0,T0,T1);
//      dbgpln("  P:%+12.4g %12.6f > %12.6f", P1-P0,P0,P1);
//      dbgpln("  M:%+12.4g %12.6f > %12.6f %12.6f>%12.6f", (MV1-MV0)+(MSL1-MSL0),MV0,MV1,MSL0,MSL1);
      dbgpln("  T : %14.6f > %14.6f [%+14.6g]",  T0, T1, T1-T0);
      dbgpln("  P : %14.6f > %14.6f [%+14.6g]",  P0, P1, P1-P0);
      dbgpln("  H : %14.6g > %14.6g [%+14.6g]",  H0, H1, H1-H0);
      dbgpln("  Mv: %14.6f > %14.6f [%+14.6g]",  MV0, MV1, (MV1-MV0));
      dbgpln("  Ml: %14.6f > %14.6f [%+14.6g]",  MSL0, MSL1, (MSL1-MSL0));
      CSysVector &YYY=Ql.MArray();
      for (int i=0; i<SVSpcCount(); i++)
        if (XXX[i]>1.0e-6 || YYY[i]>1.0e-6)
          dbgpln("  M : %14.6f > %14.6f [%+14.6g] %s", XXX[i], YYY[i], YYY[i]-XXX[i], SVImg(i).SymOrTag());
      }
    int xxx=0;
    #endif
    }
  };

//--------------------------------------------------------------------------

void CVLEBase::QVFlash(SpContainer &C, double Duty, dword Flags)
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
      dbgpln("QVFlash:%s",C.FullObjTag());
    #endif
    
    pVLE->QVFlash(C, Duty, Flags);
    
    #if DoDbg
    double T1=C.Temp();
    double P1=C.Press();
    double H1=C.totHf();
    double MV1=C.Mass(som_Gas);
    double MSL1=C.Mass(som_SL);
    if (dbgQVFlashChg() || dbgQVFlashChgDbg() && C.DoDbgBrk())
      {
//      dbgpln("QVFlash:%s",C.FullObjTag());
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
    if (1) //Must Test
      pVLE->QVFlash(C, Duty, Flags);
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

    pVLE->QVFlash(C, Duty, Flags);

    #if DoDbg
    double T1=C.Temp();
    double P1=C.Press();
    double H1=C.totHf();
    double MV1=C.Mass(som_Gas);
    double MSL1=C.Mass(som_SL);
    if (dbgQVFlashChg() || dbgQVFlashChgDbg() && C.DoDbgBrk())
      {
//      dbgpln("QVFlash:%s",C.FullObjTag());
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

void CVLEBase::QMVapFlash(SpConduit &Q, double MVap, double Duty, dword Flags)
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

    pVLE->QMVapFlash(Q, MVap, Duty, Flags);

    #if DoDbg
    double T1=Q.Temp();
    double P1=Q.Press();
    double MV1=Q.QMass(som_Gas);
    double MSL1=Q.QMass(som_SL);
    if (Q.pAttachedTo && Q.pAttachedTo->DoDbgBrk())
      {
      dbgpln("QMVapFlash:%s",Q.FullObjTag());
      dbgpln("  T:%+12.4g %12.6f > %12.6f", T1-T0,T0,T1);
      dbgpln("  P:%+12.4g %12.6f > %12.6f", P1-P0,P0,P1);
      dbgpln("  M:%+12.4g %12.6f > %12.6f %12.6f>%12.6f", (MV1-MV0)+(MSL1-MSL0),MV0,MV1,MSL0,MSL1);
      }
    int xxx=0;
    #endif
    }
  };

//--------------------------------------------------------------------------

void CVLEBase::QMVapFlash(SpConduit &Ql, SpConduit &Qv, double MVap, double Duty, dword Flags)
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

    pVLE->QMVapFlash(Ql, Qv, MVap, Duty, Flags);

    #if DoDbg
    double T1=Ql.Temp();
    double P1=Ql.Press();
    double MV1=Ql.QMass(som_Gas);
    double MSL1=Ql.QMass(som_SL);
    if (Ql.pAttachedTo && Ql.pAttachedTo->DoDbgBrk())
      {
      dbgpln("QPFlash:%s",Ql.FullObjTag());
      dbgpln("  T:%+12.4g %12.6f > %12.6f", T1-T0,T0,T1);
      dbgpln("  P:%+12.4g %12.6f > %12.6f", P1-P0,P0,P1);
      dbgpln("  M:%+12.4g %12.6f > %12.6f %12.6f>%12.6f", (MV1-MV0)+(MSL1-MSL0),MV0,MV1,MSL0,MSL1);
      }
    int xxx=0;
    #endif
    }
  };

//--------------------------------------------------------------------------

int CVLEBase::GetVLEData(CVLEDataArray & Array)
  {
  if (Enabled()) 
    return pVLE->GetVLEData(Array);
  return 0;
  };

//--------------------------------------------------------------------------

double CVLEBase::CondensableFrac(SpConduit *Ql, SpConduit *Qv)
  {
  if (Enabled()) 
    return pVLE->CondensableFrac(Ql, Qv);
  return dNAN;
  };

//--------------------------------------------------------------------------

double CVLEBase::CondensableFrac(SpContainer *Cn)
  {
  if (Enabled()) 
    return pVLE->CondensableFrac(Cn);
  return dNAN;
  };

//--------------------------------------------------------------------------

double CVLEBase::SaturationP(double Temp, CSysVector * pMA)
  {
  if (Enabled()) 
    return pVLE->SaturationP(Temp, pMA);
  return dNAN;
  };

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
    if (CDB[i].LiqPhInx("l")>=0 && CDB[i].VapPhInx("g")>=0)
      { 
      CVLEData C(i, 0.5, CDB[i].LiqPhInx("l"), CDB[i].VapPhInx("g"));
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

  iState=VLES_Error;
  SetCI(2, nComps<=0);
  if (nComps>0)
    iState=VLES_OK;
  SetCI(1, (iState==VLES_Error));
  return iState;
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

void CVLEFixdPerc::QPFlash(SpConduit &Q, double Press, double Duty, dword Flags)
  { 
  if (Setup(*Q.Model())==VLES_OK)
    {
    SetLVRatio(*Q.Model());
    Q.Set_totHf(Ht+Duty);
    }
  };

// -------------------------------------------------------------------------

void CVLEFixdPerc::QPFlash(SpConduit &Ql, SpConduit &Qv, double Press, double Duty, dword Flags)
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

void CVLEFixdPerc::QVFlash(SpContainer &C, double Duty, dword Flags)
  { 
  if (Setup(*C.Model())==VLES_OK)
    {
    SetLVRatio(*C.Model());
    C.Set_totHf(Ht+Duty);
    }
  };

// -------------------------------------------------------------------------

void CVLEFixdPerc::QMVapFlash(SpConduit &Q, double MVap, double Duty, dword Flags)
  { 
  DoBreak();
  };

// -------------------------------------------------------------------------

void CVLEFixdPerc::QMVapFlash(SpConduit &Ql, SpConduit &Qv, double MVap, double Duty, dword Flags)
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
    case 4: pS="N\tVLE:QPFlash not Conconverged"; return 1;
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

  m_iCmpNo=-1; //Shouldn't default always be H2O ???
  for (int i=0; i<CDB.No(); i++)
    if (CDB[i].VpOK() && (CDB[i].LiqPhInx("l")>=0) && (CDB[i].VapPhInx("g")>=0))
      {
      m_iCmpNo=i;
      break;
      }
  m_li=-1;
  m_vi=-1;
  iState=VLES_Error;
  Mt=0.0;
  Ht=0.0;
  dPrvVapFrac=0.1;
  dPrvVapPress=Std_P;
  fModelSatP=true;
  dApproachTemp=0.0;
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

    DDB.Long    ("", "State",      DC_, "", &iState,     this, 0, DDBVLEStates);
    DDB.CheckBox("", "ModelSatP",  DC_, "", &fModelSatP, this, isParm, DDBOnOff);
  
    //Strng S;
    DDBValueLstMem DDB1;
    DDB1.Add(-1, " ");
    for (int i=0; i<CDB.No(); i++)
      if (CDB[i].VpOK() && (CDB[i].LiqPhInx("l")>=0) && (CDB[i].VapPhInx("g")>=0))
        DDB1.Add(i, CDB[i].SymOrTag());
  //  if (DDB1.Length()==0)
  //    DDB1.Add(-1, " ");
    DDB.Long  ("Component",    "",  DC_,     "",    &m_iCmpNo,        this, isParm, DDB1());
    DDB.Double("PrvVapFrac",   "",  DC_Frac, "%",   &dPrvVapFrac,   this, isParm|InitHidden);
    DDB.Double("PrvVapPress",  "",  DC_P,    "kPa", &dPrvVapPress,  this, isParm|InitHidden);
    DDB.Double("ApproachTemp", "",  DC_dT,   "C",   &dApproachTemp, this, isParm);
    }
  DDB.EndStruct();
  };

//--------------------------------------------------------------------------

flag CVLEVapPress::DataXchg(DataChangeBlk & DCB)
  {

//  if (DCB.lHandle>=xidGSMSpcSplts &&  DCB.lHandle<xidGSMSpcSplts+SDB.No()*IOs.GetSize())
//    {
//    return 1;
//    }
//
  return 0;
  };

//--------------------------------------------------------------------------

flag CVLEVapPress::ValidateData(ValidateDataBlk & VDB)
  {
  //dApproachTemp = ValidateRange(VDB, "ApproachTemp", 0.0, dApproachTemp,    20.0);

  flag OK=true;
  if (m_iCmpNo>=0)
    {
    m_li=CDB[m_iCmpNo].LiqPhInx("l");
    m_vi=CDB[m_iCmpNo].VapPhInx("g");
    OK=(m_li>=0 && m_vi>=0);
    
    bool Lst[MaxSpecies];
    for (int i=0; i<SVSpcCount(); i++)
      Lst[i]=(i!=m_vi && SVImg(i).PhaseIs(som_Gas)!=0);

    OtherVapSp.CreateFrom(Lst, false);
    }
  else
    {
    m_li=-1;
    m_vi=-1;
    OK=false;
    }
  iState=OK ? VLES_OK : VLES_Error;
  return OK;
  };

// -------------------------------------------------------------------------

int CVLEVapPress::Setup(SpModel *pMdlL_, SpModel *pMdlV_)
  { 
  pMdlL=pMdlL_;
  pMdlV=pMdlV_;
  // Initial Calcs
  iState=VLES_Error;
  if (m_li>=0 && m_vi>=0)
    {
    ClrCI(1);
    //Mt=0.0;
    Ht=pMdlL->totHf();
    Mt=pMdlL->m_M[m_li]+pMdlL->m_M[m_vi];
    if (pMdlV)
      {
      // Vapours ito Liq
      Ht+=pMdlL->totHf(som_ALL, pMdlV->Temp(), pMdlV->Press(), pMdlV->pMArray());
      //Ht+=pMdlV->totHf();
      Mt+=pMdlV->m_M[m_li]+pMdlV->m_M[m_vi];
      }

    if (Mt>=UsableMass)
    //if (Mt>1.0e-8)
      {
      iState=VLES_OK;
      Vf=pMdlL->m_M[m_vi];
      if (pMdlV)
        Vf+=pMdlV->m_M[m_vi];
      Vf/=Mt;
      ClrCI(2);
      iState=VLES_OK;
      }
    else
      {
      //SetCI(2);
      Vf=0.0;
      }
    }
  else
    {
    SetCI(1);
    ClrCI(2);
    }
  return iState;
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
    pMdlL->VValue[m_li] = Mt*(1.0-VapFrac);
    pMdlL->VValue[m_vi] = 0.0;
    pMdlV->VValue[m_li] = 0.0;
    pMdlV->VValue[m_vi] = Mt*VapFrac;
    }
  else
    {
    pMdlL->VValue[m_li] = Mt*(1.0-VapFrac);
    pMdlL->VValue[m_vi] = Mt*VapFrac;
    }
  }

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
    double OtherPP=0.0;//pMdlL->PartialPress(OtherVapSp, Temp);
    double TSat;
    if (fModelSatP)
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
    double TMn, TMx, P;
    bool m_DbgOn;
    bool m_PassOne;
    static CToleranceBlock s_Tol;

    QPRootFnd(pSpModel pMdlL_, pSpModel pMdlV_, CVLEVapPress &VLE_, double p, bool DbgOn) :
      VLE(VLE_),
      MRootFinderBase("QPRootFnd", s_Tol)//1.0e-12)
      {
      m_PassOne=true;
      m_DbgOn=DbgOn;
      pMdlL=pMdlL_;
      pMdlV=pMdlV_;
      P=p;
      TMn=CDB.MinT;
      TMx=CDB.MaxT;
      SetErrorHandling(true, &pMdlL->m_fConvergeBusy);
      };
    
    LPCTSTR ObjTag() { return (LPCTSTR)pMdlL->FullObjTag(); };

    double Function(double x)
      {
      if (m_DbgOn && m_PassOne)
        { int xxx=0; }
      
      double OtherPP=pMdlL->PartialPress(VLE.OtherVapSp, pMdlL->Temp());

      VLE.SetVapFrac(Range(0.0, x, 1.0));
      double Ts = Min(TMx, pMdlL->SaturationT(Max(1.0, P-OtherPP)));
      double h, t;
      if (x<0.0)
        t=TMn+(Ts-TMn)*(x+1.0); // Map -1<=x<=0 on to TMn to T
      else if (x>1.0)
        t=Ts+(TMx-Ts)*(x-1.0);   // Map 1<=x<=2 on to T to TMx
      else
        t=Ts;                   // Map 1<=x<=2 on to T to TMx

      pMdlL->SetTempPress(t, P);
      if (pMdlV)
        {
        pMdlV->SetTempPress(t, P);
        }
      h = pMdlL->totHf();
      if (pMdlV)
        h+=pMdlL->totHf(som_ALL, pMdlV->Temp(), pMdlV->Press(), pMdlV->pMArray()); // Vapours in Liq

      if (m_DbgOn)
        {
        SpModelExDbgOn++;

        if (m_PassOne)
          dbgpln("  QPRoot\t%14s\t%14s\t%14s\t%14s\t%14s\t%14s\t%14s\t%14s\t%14s\t%14s\t%14s", 
          "VFrac","Ts","t","M","P","Htrg","H-Htrg","Hl", "Hv", "Ml", "Mv");
        dbgpln("  QPRoot\t%14.10f\t%14.6f\t%14.6f\t%14.6f\t%14.6f\t%14.6f\t%14.6g\t%14.6f\t%14.6f\t%14.6f\t%14.6f", 
          x, K_2_C(Ts),K_2_C(t),VLE.Mt,P,dTarget,h-dTarget, 
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
void CVLEVapPress::QPFlash(double Press, double Duty, dword Flags)
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
  double VFrac0=dPrvVapFrac;//Vf;

  QPRootFnd QPF(pMdlL, pMdlV, *this, Press, (Flags&VLEF_WithDebug)!=0);
  QPF.SetTarget(HTrg);


//static int xxx=0;
//if (xxx)
//    {
//    SpConduit &W=*GetWrkConduit("QPFW");
//    SpConduit &L=*GetWrkConduit("QPFL");
//    SpConduit &S=*GetWrkConduit("QPFS");
//    W.QSetF(*pMdlL, som_ALL, 1.0, pMdlL->Press());
//    W.ChangeModel("Steam/Water");
//    L.QZero();
//    L.ChangeModel("Steam/Water");
//    L.QSet(2, 1.0);
//    S.QZero();
//    S.ChangeModel("Steam/Water");
//    S.QSet(0, 1.0);
//    for (double P=Std_P; P<2500; P+=20)
//      {
//      double Ts = pMdlL->SaturationT(P);
//      double Tsw = W.SaturationT(P);
//      double h1 = pMdlL->msHf(som_ALL, Ts, P, pMdlL->pMArray());
//      double h2 = W.msHf(som_ALL, Ts, P, pMdlL->pMArray());
//      double h3 = L.msHf(som_ALL, Ts, P, L.pMArray());
//      double h4 = S.msHf(som_ALL, Ts, P, S.pMArray());
//
//      dbgpln("XXX\t%16.6f\t%16.6f\t%16.6f\t%16.6f\t%16.6f\t%16.6f\t%16.6f",
//        P,K_2_C(Ts),K_2_C(Tsw),h1,h2,h3,h4);
//      }
//    }
  
  double LoX = -1;
  double HiX = 2;

  flag fBad=false;
  flag DoCnvg=true;
  flag DoRestore=false;
  double Hf0, Hf1;

  Hf0 = QPF.Function(0.0);
  if (Hf0 >= HTrg)
    {// Sub Cooled (ie 0% vapour)
    HiX=0.0;
    //HiF=Hf0;
    if (!pMdlV)
      {
      DoCnvg =false;
      pMdlL->Set_totHf(HTrg);
      }
    goto Continue;
    }

  Hf1 = QPF.Function(1.0);

  if (Hf1 <= HTrg)
    {// SuperHeated (ie 100% vapour)
    LoX=1.0;
    //LoF=Hf1;
    if (!pMdlV)
      {
      DoCnvg =false;
      pMdlL->Set_totHf(HTrg);
      }
    goto Continue;
    }

  LoX=0.0;
  HiX=1.0;
  
Continue:
  if (DoCnvg)
    {  
    QPF.SetLimits(LoX, HiX);

    /*switch (QPF.SolveFromEst(VFrac0, 1.0, 0.1))
      {
      case RF_OK:
        break;

      case RF_HiLimit:
        //DoRestore=(Flags & VLEF_SaturationLimit);
        break;

      case RF_LoLimit:
        //DoRestore=(Flags & VLEF_SaturationLimit);
        break;

      default:
        fBad=true;
        break;
      }*/
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
          //DoRestore=(Flags & VLEF_SaturationLimit);
          break;

        case RF_LoLimit:
          //DoRestore=(Flags & VLEF_SaturationLimit);
          break;

        default:
          fBad=true;
          break;
        }
      }
    else
      fBad=true;

    if (fBad || DoRestore)
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
      dPrvVapFrac=dVFrac;
      }
    }

  pMdlL->ClrHoldVentExpand();  
  if (pMdlV)
    pMdlV->ClrHoldVentExpand();  

  };

// -------------------------------------------------------------------------

void CVLEVapPress::QPFlash(SpConduit &Q, double Press, double Duty, dword Flags)
  { 
  if (Setup(Q.Model(), NULL)==VLES_OK)
    {
    QPFlash(Press, Duty, Flags);
    }
  };

// -------------------------------------------------------------------------

void CVLEVapPress::QPFlash(SpConduit &Ql, SpConduit &Qv, double Press, double Duty, dword Flags)
  { 
  if (Setup(Ql.Model(), Qv.Model())==VLES_OK)
    {
    QPFlash(Press, Duty, Flags);
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
      ASSERT_ALWAYS(T1<10000.0, "HIGH temperature in QVFlash!", __FILE__, __LINE__);
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

void CVLEVapPress::QVFlash(SpContainer &C, double Duty, dword Flags)
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
    double ApparentT=CurrentT-dApproachTemp;
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
            LogError(FullObjTag(), 0, "QVFlash Not converged T:%g,P:%g",K2C(CurrentT),CurrentP-101.325);
          }
        #endif

        XFrac+=ReqdXChg;
        SetVapFrac(Range(0.0,XFrac,1.0));

        CurrentT+=ReqdTChg;
        
        ApparentT=CurrentT-dApproachTemp;
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
        iState=VLES_TwoPhase;
        dPrvVapFrac=Fnd.Result();
        break;

      case RF_HiLimit:
        iState=VLES_SubCooled;
        dPrvVapFrac=Fnd.Result();
        break;

      case RF_LoLimit:
        iState=VLES_SuperHeated;
        dPrvVapFrac=Fnd.Result();
        break;

      default:
        iState=VLES_NotConverged;
        SetVapFrac(dPrvVapFrac);
        C.Set_totHf(Ht);

        // LogError(FullObjTag(), 0, "QVFlash Not Converged");
        break;
      }
    SetCI(4, iState==VLES_NotConverged);
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
         K_2_C(Ts),VLE.Mt,P,dTarget,h-dTarget, pMdlL->totHf(som_ALL, Ts, pMdlL->SaturationP(Ts), pMdlL->pMArray()));
  }
      return h;
      };
  };

CToleranceBlock QTsRootFnd::s_Tol(TBF_BothSys, "VLE:QTsRootFind", 0, 1.0e-12);

// --------------------------------------------------------------------------

static int QMVapFlashBrentErr=0;

void CVLEVapPress::QMVapFlash(double MVap, double Duty, dword Flags)
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
 
  double Press0=Range(1.0+10.0, dPrvVapPress, 10000.0-10.0);//Vf;
  //if (Press0<1.1) 
  //  Press0=10.0;
  QTsF.SetLimits(1.0, 10000.0);
  switch (QTsF.SolveFromEst(Press0, 1.0, 10.0))
    {
    case RF_OK: 
      dPrvVapPress=QTsF.Result(); 
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

void CVLEVapPress::QMVapFlash(SpConduit &Q, double MVap, double Duty, dword Flags)
  { 
  if (Setup(Q.Model(), NULL)==VLES_OK)
    {
    QMVapFlash(MVap, Duty, Flags);
    }
  };

// -------------------------------------------------------------------------

void CVLEVapPress::QMVapFlash(SpConduit &Ql, SpConduit &Qv, double MVap, double Duty, dword Flags)
  { 
  if (Setup(Ql.Model(), Qv.Model())==VLES_OK)
    {
    QMVapFlash(MVap, Duty, Flags);
    }
  };

// -------------------------------------------------------------------------

int CVLEVapPress::GetVLEData(CVLEDataArray & Array)
  {
  Array.SetSize(1);
  Array[0].iCmpNo=m_iCmpNo;
  Array[0].li=m_li;
  Array[0].vi=m_vi;
  return Array.GetSize();
  };

//--------------------------------------------------------------------------

double CVLEVapPress::CondensableFrac()
  { 
  if (m_li>=0 && m_vi>=0)
    {
    double Mv=pMdlL->m_M[m_vi];
    if (pMdlV)
      Mv+=pMdlV->m_M[m_vi];

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

double CVLEVapPress::SaturationP(double Temp, CSysVector * pMA)
  {
  if (m_li>=0)
    return SDB[m_li].VapourP(pMdlL->Fidelity(), Temp);
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
CVLERootK::CVLERootK(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CVLEBlock(pClass_, TagIn, pAttach, eAttach),
//  Nd(*((FlwNode*)pAttach)),
  n(Z)
  {
  pMdlL=NULL;
  pMdlV=NULL;

  for (int i=0; i<CDB.No(); i++)
    if (CDB[i].LiqPhInx("l")>=0 && CDB[i].VapPhInx("g")>=0)
      { 
      CVLEData C(i, 1.0, CDB[i].LiqPhInx("l"), CDB[i].VapPhInx("g"));
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
  iState=VLES_Error;
  nComps=0;
  dPrvTemp=dNAN;//Std_T;
  dPrvPress=dNAN;//Std_P;
  dPrvVMoleFrac=0.1;
  Vf=0.0;
  //m_SensHeatGain=0;  
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
//  DDB.Int("", "State",    DC_,     "",    (int*)&iState, this, 0, DDB0());
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

  iState=OK ? VLES_OK : VLES_Error;
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

  iState=VLES_Error;
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

void CVLERootK::QPFlash(SpConduit &Q, double Press, double Duty, dword Flags)
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
      //LogError(FullObjTag(), 0, "QPFlash Not Converged");
      };
    }
  };

// -------------------------------------------------------------------------

void CVLERootK::QPFlash(SpConduit &Ql, SpConduit &Qv, double Press, double Duty, dword Flags)
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
      //LogError(FullObjTag(), 0, "QPFlash Not Converged");

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

void CVLERootK::QVFlash(SpContainer &C, double Duty, dword Flags)
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
        dbgpln("QVFlash %14.4e", Ht+Duty);
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
      //  LogError(FullObjTag(), 0, "QVFlash Not Converged");
      SetCI(4, !OK);
      #if DoDbg1
      if (DoDbgBrk())
        dbgpln(" ");
      #endif
      }
    }
  };
**/

void CVLERootK::QVFlash(SpContainer &C, double Duty, dword Flags)
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
//      //  LogError(FullObjTag(), 0, "QVFlash Not Converged");
      SetCI(4, !OK);
      #if DoDbg1
      if (DoDbgBrk())
        dbgpln(" ");
      #endif
      }
    }
  };

// -------------------------------------------------------------------------

void CVLERootK::QMVapFlash(SpConduit &Q, double MVap, double Duty, dword Flags)
  { 
  DoBreak();
  };

// -------------------------------------------------------------------------

void CVLERootK::QMVapFlash(SpConduit &Ql, SpConduit &Qv, double MVap, double Duty, dword Flags)
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
//  bTAdj=!bDoVLE || !Cn.Closed() || !pVLE->Enabled() || pVLE->HasTPEnvelope();
//  return CSpContPreset::ValidateData(VDB, Cn);
//  }
//
//flag SpContPresetVLE::AdjustConditions(SpContainer & Cn)
//  {
//  if (bDoVLE)
//    {
//    pVLE->QVFlash(Cn, 0.0, VLEF_Null);
//    return pVLE->Enabled();
//    }
//  return false;
//  }
//
//============================================================================
//
//
//
//============================================================================
