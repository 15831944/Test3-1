//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include "sc_defs.h"
#define  __M_BASE_CPP
#include "m_base.h"
#include "m_area.h"
#include "dbgmngr.h"
#include "scd_wm.h"
//#include "optoff.h"

#define dbgDerivs        (0 || WITHDEBUG)
#define dbgModels        (0 || WITHDEBUG)

#if dbgDerivs
static CDbgMngr dbgDerivsSrg1             ("Derivatives", "SurgeLevel_1");
static CDbgMngr dbgDerivsSrg2             ("Derivatives", "SurgeLevel_2");
static CDbgMngr dbgDerivsSrg3             ("Derivatives", "SurgeLevel_3");
static CDbgMngr dbgDerivsSrgSetOut        ("Derivatives", "SurgeLevel_SetOut");
static CDbgMngr dbgDerivsSrgOvr           ("Derivatives", "SurgeLevel_Ovr");
static CDbgMngr dbgDerivsSrg1DbgBrk       ("Derivatives", "SurgeLevel_1DbgBrk");
static CDbgMngr dbgDerivsSrg2DbgBrk       ("Derivatives", "SurgeLevel_2DbgBrk");
static CDbgMngr dbgDerivsSrg3DbgBrk       ("Derivatives", "SurgeLevel_3DbgBrk");
static CDbgMngr dbgDerivsSrgOvrDbgBrk     ("Derivatives", "SurgeLevel_OvrDbgBrk");
static CDbgMngr dbgDerivsSrgSetOutDbgBrk  ("Derivatives", "SurgeLevel_SetOutDbgBrk");
static CDbgMngr dbgDerivsXfer             ("Derivatives", "Xfer");
static CDbgMngr dbgDerivsFlows            ("Derivatives", "Flows");
#endif
#if dbgModels
static CDbgMngr dbgFlowConverge           ("Models",      "FlowConverge");
static CDbgMngr dbgFlowConvergeDbgBrk     ("Models",      "FlowConvergeDbgBrk");
static CDbgMngr dbgSetZsValue             ("Models",      "SetZsValue");
static CDbgMngr dbgSetZsTrace             ("Models",      "SetZsTrace");
static CDbgMngr dbgSortIOs                ("Models",      "SortIOs");
#endif

const double sc_dTSettleRatio = 1.2;

//============================================================================
//
//
//
//============================================================================

CSurgeControl::CSurgeControl()
  {
  m_bEnabled        = false;
  m_bOn             = true;
  m_iType           = SC_Level;
  m_dLevelSPT       = 0.5;         //kg
  m_dMassSPT        = 0;           //kg
  m_dVolumeSPT      = 0;           //m^3
  m_iStreamIndex    = -1;
  m_dQmCalc         = 0;           //kg/s

  m_dQmMin          = 0;           //kg/s
  m_dQmMax          = fNAN;      //kg/s
  m_dQmMinStep      = 0;           //kg/s
  m_dQmMaxStep      = fNAN;      //kg/s
  m_dQmMaxSlewUp    = fNAN;      //kg/s/s
  m_dQmMaxSlewDn    = fNAN;      //kg/s/s
  //m_dQvMin          = 0;           //m^3/s
  //m_dQvMax          = fNAN;      //m^3/s
  //m_dQvMinStep      = 0;           //m^3/s
  //m_dQvMaxStep      = fNAN;      //m^3/s
  //m_dQvMaxSlewUp    = fNAN;      //m^3/s/s
  //m_dQvMaxSlewDn    = fNAN;      //m^3/s/s
  }

//--------------------------------------------------------------------------

void CSurgeControl::BuildDataDefn(DataDefnBlk & DDB, LPCSTR Tag, FlwNode* pNd)
  {
  DDBValueLstMem DDBSt;
  DDBSt.Add(-1, " ");
  int i;
  for (i=0; i<pNd->NoProcessIOs(); i++)
    {
    //if (pNd->IO_Out(i))
      {
      if (m_iStreamIndex<0 && pNd->IOId_Rmt(i)==0)
        m_iStreamIndex = i; //default to a reasonable stream
      DDBSt.Add(i, pNd->Nd_Rmt(i)->Tag(), pNd->IOId_Rmt(i)==0?0:MDD_Hidden);
      //DDBSt.Add(i, pNd->Nd_Rmt(i)->Tag(), pNd->IO_OutGEZ(i)?0:MDD_Hidden);
      }
    }
  static DDBValueLst DDBType[]=
    {
      {SC_Level,   "Level"},
      {SC_Mass,    "Mass"},
      {SC_Volume,  "Volume"},
      {0},
    };

  DDB.Visibility(m_bEnabled ? SM_DynBoth:0);
  //DDB.Text("Level control...");
  if (DDB.BeginStruct(pNd, (char*)Tag))//.Text("Level control...");
    {
    DDB.CheckBox("On",           "",  DC_,     "",       &m_bOn,            pNd, isParm);
    DDB.Byte    ("",   "Type",        DC_ ,    "",       &m_iType,          pNd, isParm|SetOnChange, DDBType);
    DDB.Visibility(SM_DynBoth|HM_All, m_iType==SC_Level);
    DDB.Double  ("",   "SPT.Level",   DC_Frac, "%",      &m_dLevelSPT,      pNd, isParm);
    DDB.Visibility(SM_DynBoth|HM_All, m_iType==SC_Mass);
    DDB.Double  ("",   "SPT.Mass",    DC_M,    "kg",     &m_dMassSPT,       pNd, isParm);
    DDB.Visibility(SM_DynBoth|HM_All, m_iType==SC_Volume);
    DDB.Double  ("",   "SPT.Volume",  DC_Vol,  "m^3",    &m_dVolumeSPT,     pNd, isParm);
    DDB.Visibility(SM_DynBoth|HM_All);
    //if (iStreamIndex>=0)
    DDB.Short   ("",   "Stream",      DC_ ,       "",         &m_iStreamIndex,   pNd, isParmStopped, DDBSt());
#if WITHSURGECTRLDOUBLE 
    DDB.Double  ("",   "QmMin",       DC_Qm,      "kg/s",     &m_dQmMin,        pNd, isParm);
    DDB.Double  ("",   "QmMax",       DC_Qm,      "kg/s",     &m_dQmMax,        pNd, isParm|NAN_OK);
    DDB.Double  ("",   "QmMinStep",   DC_Qm,      "kg/s",     &m_dQmMinStep,    pNd, isParm|InitHidden);
    DDB.Double  ("",   "QmMaxStep",   DC_Qm,      "kg/s",     &m_dQmMaxStep,    pNd, isParm|InitHidden);
    DDB.Double  ("",   "QmMaxSlewUp", DC_QmSlew,  "kg/s/s",   &m_dQmMaxSlewUp,  pNd, isParm|NAN_OK);
    DDB.Double  ("",   "QmMaxSlewDn", DC_QmSlew,  "kg/s/s",   &m_dQmMaxSlewDn,  pNd, isParm|NAN_OK);
    DDB.Double  ("",   "QmReqd",      DC_Qm,      "kg/s",     &m_dQmCalc,       pNd, isResult);
#else
    DDB.Float   ("",   "QmMin",       DC_Qm,      "kg/s",     &m_dQmMin,        pNd, isParm);
    DDB.Float   ("",   "QmMax",       DC_Qm,      "kg/s",     &m_dQmMax,        pNd, isParm|NAN_OK);
    DDB.Float   ("",   "QmMinStep",   DC_Qm,      "kg/s",     &m_dQmMinStep,    pNd, isParm|InitHidden);
    DDB.Float   ("",   "QmMaxStep",   DC_Qm,      "kg/s",     &m_dQmMaxStep,    pNd, isParm|InitHidden);
    DDB.Float   ("",   "QmMaxSlewUp", DC_QmSlew,  "kg/s/s",   &m_dQmMaxSlewUp,  pNd, isParm|NAN_OK);
    DDB.Float   ("",   "QmMaxSlewDn", DC_QmSlew,  "kg/s/s",   &m_dQmMaxSlewDn,  pNd, isParm|NAN_OK);
    DDB.Float   ("",   "QmReqd",      DC_Qm,      "kg/s",     &m_dQmCalc,       pNd, isResult);
#endif
    }
  DDB.EndStruct();
  if (m_bOn && PrjFileVerNo()<83)
    {//correct for bad data
#if WITHSURGECTRLDOUBLE 
    if (Valid(m_dQmMax) && m_dQmMax>1.0e15)
      m_dQmMax = dNAN;
    if (Valid(m_dQmMaxSlewUp) && m_dQmMaxSlewUp>1.0e15)
      m_dQmMaxSlewUp = dNAN;
    if (Valid(m_dQmMaxSlewDn) && m_dQmMaxSlewDn>1.0e15)
      m_dQmMaxSlewDn = dNAN;
#else
    if (Valid(m_dQmMax) && m_dQmMax>1.0e15)
      m_dQmMax = fNAN;
    if (Valid(m_dQmMaxSlewUp) && m_dQmMaxSlewUp>1.0e15)
      m_dQmMaxSlewUp = fNAN;
    if (Valid(m_dQmMaxSlewDn) && m_dQmMaxSlewDn>1.0e15)
      m_dQmMaxSlewDn = fNAN;
#endif
    }
//  DDB.Text("");
  }

void CSurgeControl::StartStep()
  {
  if (Valid(m_dQmMax))
    m_dQmMax=Max(m_dQmMax, m_dQmMin);         

  m_dQmMinStep=m_dQmMin;         
  if (Valid(m_dQmMaxSlewDn))
#if WITHSURGECTRLDOUBLE 
    m_dQmMinStep=Max(m_dQmMinStep, m_dQmCalc-m_dQmMaxSlewDn*ICGetTimeInc());
#else
    m_dQmMinStep=Max(m_dQmMinStep, float(m_dQmCalc-m_dQmMaxSlewDn*ICGetTimeInc()));
#endif

  m_dQmMaxStep=m_dQmMax;         
  if (Valid(m_dQmMaxSlewUp))
    {
#if WITHSURGECTRLDOUBLE 
    m_dQmMaxStep=m_dQmCalc+m_dQmMaxSlewUp*ICGetTimeInc();
#else
    m_dQmMaxStep=float(m_dQmCalc+m_dQmMaxSlewUp*ICGetTimeInc());
#endif
    if (Valid(m_dQmMax))
      m_dQmMaxStep=Min(m_dQmMax, m_dQmMaxStep);
    }
  };

//--------------------------------------------------------------------------
//
//void CLevelControl::CalcIOOut(FlwNode* pNd)
//  {
//  /*iStreamIndex =-1;
//  if (bOn && pNd->InlineIntegral() && pNd->NoFlwIOs())
//    {
//    for (int i=0; i<pNd->NoFlwIOs(); i++)
//      if (pNd->IO_Out(i))
//        {
//        if (iStreamIndex<0)
//          iStreamIndex = i;
//        }
//    }*/
//  }
//
////--------------------------------------------------------------------------
//
//void CLevelControl::DoEvalCtrl(FlwNode* pNd)
//  {
//  if (bOn && pNd->InlineIntegral() && iStreamIndex>=0)
//    {
//    //TODO: perhaps get QmMin & QmMax from pipe capacity min & max...???
//    double m = pNd->Contents.Mass();
//    double QmIn = 0.0;
//    double PrevQmOut = 0.0;
//    if (pNd->NoFlwIOs())
//      {
//      int HasFlw=0;
//      double TFlw=0;
//      for (int i=0; i<pNd->NoFlwIOs(); i++)
//        {
//        if (pNd->IO_In(i))
//          QmIn += pNd->IOQm_In(i);
//        else if (pNd->IO_Out(i))
//          {
//          PrevQmOut += pNd->IOQm_Out(i);
//          }
//        }
//      double dT=ICGetTimeInc();
//      double DeltaMass = (m+QmIn*dT) - dSetPointMass; //???
//      double QmReqd = DeltaMass / dT;
//      if (QmReqd < 0.0)
//        dQmOutCalc = dQmOutMin;
//      else
//        dQmOutCalc = Range(dQmOutMin, QmReqd, dQmOutMax);
//      //FlwBlk* pFB = pNd->IOFB(iStreamIndex, 0);
//      FlwBlk* pFB = pNd->IOFB_Rmt(iStreamIndex, 0);
//      pFB->SetXferMode();
//      pFB->SetXferCapacity(dQmOutCalc);
//      //TODO: set pipe capacity = dQmOutCalc...
//      /*if (pFB && pFB->InXferNet())
//        {
//        double prev = pFB->dQm_XfCap;
//        pFB->SetXferCapacity(QmOut);
//        }*/
////LogNote(pNd->Tag(), 0, "QmIn:%g  PrevQmOut:%g  Mass:%g   Set Qm=%g", QmIn, PrevQmOut, m, dQmOutCalc);
//
//      //UsableMass)
//      //if (iStreamIndex>=0)
//      //  pNd->IOConduit(iStreamIndex)->Rqd_QmCap += 0.1;
//      }
//    
//    }
//  }

//============================================================================
//
//
//
//============================================================================

MdlNode::MdlNode(TagObjClass * pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  FlwNode(pClass_, TagIn, pAttach, eAttach),
  GSM(this),
  EHX(this)
  {
  FlowIn = 0.0;
//  bFlashIt=0;
//  Rqd_Flash_Press=dNAN;
  bNodeMakeUpReqd=0;
//  bPMUCfgOn=false;

  m_dwAuditOptions=MNAudit_Qm|MNAudit_Hz;
  };

//--------------------------------------------------------------------------

MdlNode::~MdlNode()
  {
  //delete pGSM;
  }

//--------------------------------------------------------------------------

void MdlNode::FillIOIndexList(int JoinId, int *I)
  {
  if (JoinId<0)
    {
    for (int i=0; i<NoProcessIOs(); i++)
      I[i]=i;
    I[i]=-1;
    }
  else
    {
    ASSERT(JoinId<Joins.GetSize());
    CJoinRec &J=Joins[JoinId];
    for (int i=0; i<J.NConns(); i++)
      I[i]=J.IONo(i);
    I[J.NConns()]=-1;
    }
  }

//--------------------------------------------------------------------------

#define OnlyCopyValidMass 1
void MdlNode::Xfer_EvalProductsIn_(int JoinId, double & Press, SpConduit *pQi, SpConduit *pQo, CPBPressCtrl *pPC)
  {
  int I[MaxIOList+1];
  FillIOIndexList(JoinId, I);

  //dbgpln("======================================================= %s", FullObjTag());

  #ifndef _RELEASE
  flag dbg=0;//fDoDbgBrk;
  #endif
  if (SolveDirectMethod())
    {
    Press=GetPBInputPressure(JoinId, pPC);
    //dbgpln("PBPress     = %14.3f  %14.3f", Press,pPC->TRqd());
    }

  #if OnlyCopyValidMass //kga 28/10/02 only want to copy from input streams with "VALID FEED"!
  const double TraceMassTest = TraceMass*10.0;
  #endif
  double TotQmIn=0.0;
  //double PMin=100000.0;
  CSpMdlSlct Slct;
  for (int i, ic = 0; (i=I[ic])>=0; ic++)
    {
    if (i<NoProcessIOs() && IO_In(i))
      {
      const double QmIn = IOQm_In(i);
      TotQmIn += QmIn;
      #if OnlyCopyValidMass
      Slct.Add(IOConduit(i)->Model(), QmIn>TraceMassTest ? QmIn : 0.0); //copy qualities, etc but not Qm
      #else
      Slct.Add(IOConduit(i)->Model(), QmIn);
      #endif
      }
    }

  SpConduit & Qi=pQi ? *pQi : *pQo;

  Qi.QZero();
  Qi.SetPress(Press);
  Qi.SelectModel(Slct);

  //dbgpln("Ini T,P,Qm  = %14.3f %14.3f %14.3f ",Qi.Temp(),Qi.Press(), Qi.QMass());

  if (0 && fDoDbgBrk)
    {
    double T=Qi.Temp();
    double M=Qi.QMass();
    double H=Qi.totHf();
    int xxx=0;
    }

  /*const double LoQm=QmIn*(pPC ? (pPC->IgnoreLoQm() ? pPC->LowQmFrac():0.0):pPC->LowQmFracNC());*/

  for (ic = 0; (i=I[ic])>=0; ic++)
    if (i<NoProcessIOs() && IO_In(i)/* && IOConduit(i)->QMass()>=LoQm*/)
      {
      //dbgpln("Add >>>>>>>>>>>");
      #if OnlyCopyValidMass
      if (TotQmIn<TraceMass)
        Qi.QAddF(*IOConduit(i), som_ALL, 1.0);
      else if (IOQm_In(i)>TraceMassTest)
        Qi.QAddF(*IOConduit(i), som_ALL, 1.0);
      #else
      Qi.QAddF(*IOConduit(i), som_ALL, 1.0);
      #endif

      //dbgpln("Add T1,T2   = %14.3f %14.3f %s",Qi.Temp(),IOConduit(i)->Temp(),Nd_Rmt(i)->Tag());

      if (0 && fDoDbgBrk)
        {
        double T1=Qi.Temp();
        double M1=Qi.QMass();
        double H1=Qi.totHf();
        double T2=IOConduit(0)->Temp();
        double M2=IOConduit(0)->QMass();
        double H2=IOConduit(0)->totHf();
        int xxx=0;
        }
      }

  #ifndef _RELEASE
  if (dbg)
    {
    for (ic = 0; (i=I[ic])>=0; ic++)
      if (i<NoProcessIOs() && IO_In(i))
        {
        char t[128];
        sprintf(t, "%s.%s", FullObjTag(), IODesc_Self(i)->pName);
        dbgpln("i-Xfr %-12.12s : %14.6g, %14.6g | %14.6g (%14.6g)",t,
               IOConduit(i)->QMass(som_SL), IOConduit(i)->QMass(som_Gas), K_2_C(IOConduit(i)->Temp()), IOConduit(i)->totHz());
        }
    }
  #endif

  SpConduit & Qo=pQo ? *pQo : *pQi;
  if (&Qi != &Qo)
    Qo.QCopy(Qi);

  #ifndef _RELEASE
  if (dbg)
    {
    double Ti=Qi.Temp();
    double To=Qo.Temp();
    dbgpln("Ti/To = %14.6g %14.6g",Ti,To);
    dbgpln("o-Xfr %-12.12s : %14.6g, %14.6g | %14.6g (%14.6g)","", Qo.QMass(som_SL), Qo.QMass(som_Gas), K_2_C(Qo.Temp()), Qo.totHz());
    }
  #endif

  //dbgpln("=======================================================");
  }

//--------------------------------------------------------------------------

void MdlNode::Xfer_EvalProductsWork_(SpConduit *pQi, SpConduit *pQo, CReactionBlock *pRB, CEnvironHX * pEHX)
  {
  SpConduit & Qo=pQo ? *pQo : *pQi;
  if (pRB)
    pRB->EvalProducts(Qo);
  if (pEHX)
    pEHX->EvalProducts(Qo);

  if (fDoDbgBrk)
    {
    double T=Qo.Temp();
    int xxx=0;
    }
  }

//--------------------------------------------------------------------------

void MdlNode::Xfer_EvalProductsOut_(int JoinId, SpConduit *pQi, SpConduit *pQo, GSModel * pGSM, CPBPressCtrl *pPC)
  {
  // What Comes in must go out
  SpConduit & Qo=pQo ? *pQo : *pQi;
  int I[MaxIOList+1];
  FillIOIndexList(JoinId, I);

  if (pGSM)
    {
    pGSM->EvalProductsOut(SolveMethod(), Qo.Press(), Qo);
    }
  else if (bNodeMakeUpReqd)
    {
    MakeUpNodeEvalProducts(0, Qo);
    }
  else
    {
    double Qot=0.0;
    for (int i,ic = 0; (i=I[ic])>=0; ic++)
      if (i<NoProcessIOs() && IO_Out(i))
        Qot += IOQmEst_Out(i);

    double QinA=Qo.QMass(som_ALL);
    double TIn=Qo.Temp();
    // What Comes in must go out
    double Scl=Range(0.0, QinA/GTZ(Qot), 1000.0);
    for (ic = 0; (i=I[ic])>=0; ic++)
      if (i<NoProcessIOs())
        {
        if (IO_Out(i))
          IOConduit(i)->QSetM(Qo, som_ALL, IOQmEst_Out(i)*Scl, Qo.Press());
        else if (!IO_In(i))
          IOConduit(i)->QSetM(Qo, som_ALL, 0, Qo.Press());
        }
    }
  }

//--------------------------------------------------------------------------

void MdlNode::Xfer_EvalProducts(int JoinId, double Press, SpConduit *pQi, SpConduit *pQo, CReactionBlock *pRB, GSModel * pGSM, CEnvironHX * pEHX, CPBPressCtrl *pPC/*=NULL*/)
  {
  Press=Max(Press, LowPressure);
  SpConduit *Sd = (pQo!=NULL) ? pQo : GetWrkConduit("Xfer0", chLINEID());
  Xfer_EvalProductsIn_(JoinId, Press, pQi, Sd, pPC);
  Xfer_EvalProductsWork_(pQi, Sd, pRB, pEHX);
  Xfer_EvalProductsOut_(JoinId, pQi, Sd, pGSM, pPC);
  }

//--------------------------------------------------------------------------

void MdlNode::Xfer_EvalProducts(Xfer_EvalProducts_Info & S1,
                                Xfer_EvalProducts_Info & S2,
                                CHXBase * pHX)
  {
  SpConduit *Sd1 = (S1.pQo!=NULL) ? S1.pQo : GetWrkConduit("Xfer1", chLINEID());
  SpConduit *Sd2 = (S2.pQo!=NULL) ? S2.pQo : GetWrkConduit("Xfer2", chLINEID());
  Xfer_EvalProductsIn_(S1.iJoinId, S1.dPress, S1.pQo, Sd1, S1.pPBP);
  Xfer_EvalProductsIn_(S2.iJoinId, S2.dPress, S2.pQo, Sd2, S2.pPBP);

  flag HX_PreReact = (pHX && SolveDirectMethod() && pHX->PreReact()); //should this HX sequencing option be enabled for dynamic?
  if (HX_PreReact)
    {
    pHX->SetPrimary(NULL, *Sd1, S1.dPress);
    pHX->SetSecondary(NULL, *Sd2, S2.dPress);
    pHX->EvalProducts();
    }

  Xfer_EvalProductsWork_(S1.pQo, Sd1, S1.pRB, S1.pEHX);
  Xfer_EvalProductsWork_(S2.pQo, Sd2, S2.pRB, S2.pEHX);

  if (pHX && !HX_PreReact)
    {
    pHX->SetPrimary(NULL, *Sd1, S1.dPress);
    pHX->SetSecondary(NULL, *Sd2, S2.dPress);
    pHX->EvalProducts();
    }

  Xfer_EvalProductsOut_(S1.iJoinId, S1.pQo, Sd1, S1.pGSM, S1.pPBP);
  Xfer_EvalProductsOut_(S2.iJoinId, S2.pQo, Sd2, S2.pGSM, S2.pPBP);
  }

// -------------------------------------------------------------------------

void MdlNode::PostConnect(int IONo)
  {
  FlwNode::PostConnect(IONo);
  GSM.PostConnect(IONo);
  };

// -------------------------------------------------------------------------

void MdlNode::PreDisConnect(int IONo)
  {
  GSM.PreDisConnect(IONo);
  FlwNode::PreDisConnect(IONo);
  };

//--------------------------------------------------------------------------

void MdlNode::SetProdMakeup(PMU_DWFLAGS dwFlags, int IOInx, SpMArray &S, double Temperature, double Pressure, SpModel * pQualMdl)
  {
  if (dwFlags & PMU_IONo)
    {
    ASSERT(IOInx >=0 && IOInx <NoProcessIOs());
    if (IO_Out(IOInx))
      {
      if (dwFlags & (PMU_Image|PMU_AvlImage|PMU_Passing|PMU_Blocking))
        IO_Self(IOInx).SetPMUOptions(dwFlags);
      //if (dwFlags & PMU_Image)
      //  IO_Self(IOInx).SetImage();
      //else if (dwFlags & PMU_AvlImage)
      //  IO_Self(IOInx).SetAvlImage();
      //else if (dwFlags & PMU_Passing)
      //  IO_Self(IOInx).SetPassing();                        
      //else if (dwFlags & PMU_Blocking)
      //  IO_Self(IOInx).SetBlocking();
      else
        VERIFY(0);
      SpConduit & Q = *IOConduit(IOInx);
      Q.CopyQualities(pQualMdl);
      for (int k = 0; k < SVSpcCount(); k++)
        Q.SetVMass(k, Q, S[k]);
      Q.SetTempPress(Temperature, Pressure);
      }
    }
  else if (dwFlags & PMU_IOId)
    {
    PMU_DWFLAGS Flags = (dwFlags & ~PMU_IOId) | PMU_IONo;
    for (int i = 0; i < NoProcessIOs(); i++)
      if (IOId_Self(i) == IOInx)
        SetProdMakeup(Flags, i, S, Temperature, Pressure, pQualMdl);
    }
  else
    VERIFY(0); // Must Specify IOInx  or IOId
}

//--------------------------------------------------------------------------

void MdlNode::SetProdMakeup(PMU_DWFLAGS dwFlags, int IOInx, PMUImgArray &S, double Temperature, double Pressure, SpModel * pQualMdl)
  {
  if (dwFlags & PMU_IONo)
    {
    ASSERT(IOInx >=0 && IOInx <NoProcessIOs());
    if (IO_Out(IOInx))
      {
      if (dwFlags & (PMU_Image|PMU_AvlImage|PMU_Passing|PMU_Blocking))
        IO_Self(IOInx).SetPMUOptions(dwFlags);
      //if (dwFlags & PMU_Image)
      //  IO_Self(IOInx).SetImage();
      //else if (dwFlags & PMU_AvlImage)
      //  IO_Self(IOInx).SetAvlImage();
      //else if (dwFlags & PMU_Passing)
      //  IO_Self(IOInx).SetPassing();
      //else if (dwFlags & PMU_Blocking)
      //  IO_Self(IOInx).SetBlocking();
      else
        VERIFY(0);
      SpConduit & Q = *IOConduit(IOInx);
      Q.CopyQualities(pQualMdl);
      for (int k = 0; k < S.GetSize(); k++)
        Q.SetVMass(k, Q, S[k]);
      Q.SetTempPress(Temperature, Pressure);
      }
    }
  else if (dwFlags & PMU_IOId)
    {
    PMU_DWFLAGS Flags = (dwFlags & ~PMU_IOId) | PMU_IONo;
    for (int i = 0; i < NoProcessIOs(); i++)
      if (IOId_Self(i) == IOInx)
        SetProdMakeup(Flags, i, S, Temperature, Pressure, pQualMdl);
    }
  else
    VERIFY(0); // Must Specify IOInx  or IOId
}

//--------------------------------------------------------------------------

void MdlNode::SetProdMakeup(PMU_DWFLAGS dwFlags, int IOInx, SpContainer &Contents, PhMask PhaseM)
  {
  if (dwFlags & PMU_IONo)
    {
    ASSERT(IOInx >=0 && IOInx <NoProcessIOs());
    if (IO_Out(IOInx))
      {
      ASSERT(0);
      }
    }
  else if (dwFlags & PMU_IOId)
    {
    PMU_DWFLAGS Flags = (dwFlags & ~PMU_IOId) | PMU_IONo;
    for (int i = 0; i < NoProcessIOs(); i++)
      if (IOId_Self(i) == IOInx)
        SetProdMakeup(Flags, i, Contents, PhaseM);
    }
  else
    VERIFY(0); // Must Specify IOInx  or IOId
  };

//--------------------------------------------------------------------------

void MdlNode::SetProdMakeup(PMU_DWFLAGS dwFlags, int IOInx, SpContainer &Contents, double SolFrac, double LiqFrac, double VapFrac)
  {
  if (dwFlags & PMU_IONo)
    {
    ASSERT(IOInx >=0 && IOInx <NoProcessIOs());
    if (IO_Out(IOInx))
      {
      SpConduit & Q = *IOConduit(IOInx);
      //if (dwFlags & PMU_Image)
      //  IO_Self(IOInx).SetImage();
      //else if (dwFlags & PMU_AvlImage)
      //  IO_Self(IOInx).SetAvlImage();
      //else if (dwFlags & PMU_Passing)
      //  IO_Self(IOInx).SetPassing();
      //else if (dwFlags & PMU_Blocking)
      //  IO_Self(IOInx).SetBlocking();
      if (dwFlags & (PMU_Image|PMU_AvlImage|PMU_Passing|PMU_Blocking))
        IO_Self(IOInx).SetPMUOptions(dwFlags);
      else
        VERIFY(0);
      Q.QSetM(Contents, som_Sol, SolFrac, IOP_Self(IOInx));
      Q.QAddM(Contents, som_Liq, LiqFrac);
      Q.QAddM(Contents, som_Gas, VapFrac);
      }
    }
  else if (dwFlags & PMU_IOId)
    {
    PMU_DWFLAGS Flags = (dwFlags & ~PMU_IOId) | PMU_IONo;
    for (int i = 0; i < NoProcessIOs(); i++)
      if (IOId_Self(i) == IOInx)
        SetProdMakeup(Flags, i, Contents, SolFrac, LiqFrac, VapFrac);
    }
  else
    VERIFY(0); // Must Specify IOInx  or IOId
  };

void MdlNode::SetProdMakeup(PMU_DWFLAGS dwFlags, int IOInx, SpContainer &Contents, double ReqdValue)
  {
  if (dwFlags & PMU_IONo)
    {
    ASSERT(IOInx >=0 && IOInx <NoProcessIOs());
    if (IO_Out(IOInx))
      {
      if (dwFlags & PMU_SLRatio)
        {
        double SLRatio=Max(0.0, ReqdValue);
        IO_Self(IOInx).SetPMUImage();
        SpConduit & Q = *IOConduit(IOInx);
        Q.QSetM(Contents, som_Sol, SLRatio, IOP_Self(IOInx));
        Q.QAddM(Contents, som_Liq, 1.0);
        }
      else if (dwFlags & PMU_SLRatio_Rho)
        {
        double LRho=Contents.Rho(som_Liq);
        double SRho=Contents.Rho(som_Sol);

        TRACE("CHECK THIS FOR MATERIAL AVAILABILITY\n");
        if (LRho > 1.0e-6 && SRho >= 1.0e-6)
          {
          double ReqdRho=Range(Min(LRho, SRho), ReqdValue, Max(LRho, SRho)-1.0e-6);
          double SLRatio=(ReqdRho*SRho/LRho-SRho)/(SRho-ReqdRho);
      //      rFlange F = *IOFlange(IOInx);
          IO_Self(IOInx).SetPMUImage();
          SpConduit & Q = *IOConduit(IOInx);
          Q.QSetM(Contents, som_Sol, SLRatio, IOP_Self(IOInx));
          Q.QAddM(Contents, som_Liq, 1.0);
          }
        }
      else
        VERIFY(0); // Must Specify Valid Option
      }
    }
  else if (dwFlags & PMU_IOId)
    {
    PMU_DWFLAGS Flags = (dwFlags & ~PMU_IOId) | PMU_IONo;
    for (int i = 0; i < NoProcessIOs(); i++)
      if (IOId_Self(i) == IOInx)
        SetProdMakeup(Flags, i, Contents, ReqdValue);
    }
  else
    VERIFY(0); // Must Specify IOInx  or IOId
  };

//--------------------------------------------------------------------------

void MdlNode::SetProdMakeup(PMU_DWFLAGS dwFlags, int IOInx, SpConduit & C, PhMask PhaseM)
  {
  if (dwFlags & PMU_IONo)
    {
    ASSERT(IOInx >=0 && IOInx <NoProcessIOs());
    if (IO_Out(IOInx))
      {
      ASSERT(0);
      }
    }
  else if (dwFlags & PMU_IOId)
    {
    PMU_DWFLAGS Flags = (dwFlags & ~PMU_IOId) | PMU_IONo;
    for (int i = 0; i < NoProcessIOs(); i++)
      if (IOId_Self(i) == IOInx)
        SetProdMakeup(Flags, i, C, PhaseM);
    }
  else
    VERIFY(0); // Must Specify IOInx  or IOId
  };

//--------------------------------------------------------------------------

void MdlNode::SetProdMakeup(PMU_DWFLAGS dwFlags, int IOInx, SpConduit & C, double SolFrac, double LiqFrac, double VapFrac)
  {
  if (dwFlags & PMU_IONo)
    {
    ASSERT(IOInx >=0 && IOInx <NoProcessIOs());
    if (IO_Out(IOInx))
      {
      ASSERT_ALWAYS(0, "SetProdMakeup Sol Liq Vap NOT Implemented" );
      }
    }
  else if (dwFlags & PMU_IOId)
    {
    PMU_DWFLAGS Flags = (dwFlags & ~PMU_IOId) | PMU_IONo;
    for (int i = 0; i < NoProcessIOs(); i++)
      if (IOId_Self(i) == IOInx)
        SetProdMakeup(Flags, i, C, SolFrac, LiqFrac, VapFrac);
    }
  else
    VERIFY(0); // Must Specify IOInx  or IOId
  };

//--------------------------------------------------------------------------
/*#F:
This ??
*/

void MdlNode::SetProdMakeup(PMU_DWFLAGS dwFlags, int IOInx, SpConduit & C, double ReqdValue)
  {
  if (dwFlags & PMU_IONo)
    {
    ASSERT(IOInx >=0 && IOInx <NoProcessIOs());
    if (IO_Out(IOInx))
      {
      if (dwFlags & PMU_SLRatio)
        {
        IO_Self(IOInx).SetPMUImage();
        SpConduit & Q = *IOConduit(IOInx);
        double SLRatio=Max(0.0, ReqdValue);
        Q.QSetM(C, som_Sol, SLRatio, IOP_Self(IOInx));
        Q.QAddM(C, som_Liq, 1.0);
        }
      else if (dwFlags & PMU_SLRatio_Rho)
        {
        double LRho=C.Rho(som_Liq);
        double SRho=C.Rho(som_Sol);

        IO_Self(IOInx).SetPMUImage();
        SpConduit & Q = *IOConduit(IOInx);

        //TRACE("CHECK THIS FOR MATERIAL AVAILABILITY\n");
        //dbgpln("CHECK SetProdMakeup(SLRatio) FOR MATERIAL AVAILABILITY\n");
        if (LRho > 1.0e-6 && SRho >= 1.0e-6)
          {
          double ReqdRho=Range(Min(LRho, SRho), ReqdValue, Max(LRho, SRho)-1.0e-6);
          double SLRatio=Max(0.0, (ReqdRho*SRho/LRho-SRho)/(SRho-ReqdRho));
      //      rFlange F = *IOFlange(IOInx);
          Q.QSetM(C, som_Sol, SLRatio, IOP_Self(IOInx));
          Q.QAddM(C, som_Liq, 1.0);
          }
        else
          Q.QSetM(C, som_ALL, 1.0, IOP_Self(IOInx));

        }
      else
        VERIFY(0); // Must Specify Valid Option
      }
    }
  else if (dwFlags & PMU_IOId)
    {
    PMU_DWFLAGS Flags = (dwFlags & ~PMU_IOId) | PMU_IONo;
    for (int i = 0; i < NoProcessIOs(); i++)
      if (IOId_Self(i) == IOInx)
        SetProdMakeup(Flags, i, C, ReqdValue);
    }
  else
    VERIFY(0); // Must Specify IOInx  or IOId
  };

//--------------------------------------------------------------------------
/*#F:
*/

double MdlNode::SigmaQInPMax(SpConduit & Qi, PhMask PhaseM, IDMask IOIds, TagObjClass * RqdMdl)
  {
  int    iQMax=-1;
  double QMax=0.0;
  double  PMax=0.01;
  CSpMdlSlct Slct;
  for (int i=0; i<NoProcessIOs(); i++)
    if (IO_In(i) && ((IOIdMask_Self(i) & IOIds)!=0))
      {
      PMax=Max(PMax, IOP_Self(i));
      Slct.Add(IOConduit(i)->Model(), IOQm_In(i));
      //if (IOQm_In(i)>QMax)
      //  {
      //  QMax=IOQm_In(i);
      //  iQMax=i;
      //  }
      }

  Qi.QZero();
  Qi.SetPress(PMax);
  if (RqdMdl)
    Qi.ChangeModel(RqdMdl);
  else //if (iQMax>=0)
    Qi.SelectModel(Slct);//IOConduit(iQMax), true);
  for (i=0; i<NoProcessIOs(); i++)
    if (IO_In(i) && ((IOIdMask_Self(i) & IOIds)!=0))
      Qi.QAddF(*IOConduit(i), PhaseM, 1.0);
//  Qi.QZero();
//  Qi.SetPress(PMax);
//  if (RqdMdl)
//    Qi.ChangeModel(RqdMdl);
//  else if (iQMax>=0)
//    Qi.SelectModel(IOConduit(iQMax), true);
//  for (i=0; i<NoProcessIOs(); i++)
//    if ((IOIdMask_Self(i) & IOIds)!=0)
//      if (IO_In(i))
//        Qi.QAddF(*IOConduit(i), PhaseM, 1.0);
  return PMax;
  }

//--------------------------------------------------------------------------
/*#F:
*/

double MdlNode::SigmaQInPMin(SpConduit & Qi, PhMask PhaseM, IDMask IOIds, TagObjClass * RqdMdl)
  {
  int    iQMax=-1;
  double QMax=0.0;
  double PMin=100000.0;
  CSpMdlSlct Slct;
  for (int i=0; i<NoProcessIOs(); i++)
    if (IO_In(i) && ((IOIdMask_Self(i) & IOIds)!=0))
      {
      PMin=Min(PMin, IOP_Self(i));
      Slct.Add(IOConduit(i)->Model(), IOQm_In(i));
      }

  Qi.QZero();
  Qi.SetPress(PMin);
  if (RqdMdl)
    Qi.ChangeModel(RqdMdl);
  else //if (iQMax>=0)
    Qi.SelectModel(Slct);//IOConduit(iQMax), true);
  for (i=0; i<NoProcessIOs(); i++)
    {
    if (IO_In(i) && ((IOIdMask_Self(i) & IOIds)!=0))
      {
      if (fDoDbgBrk)
        dbgpln("Qi %3i %14.6f", i, IOConduit(i)->QMass(PhaseM));

      Qi.QAddF(*IOConduit(i), PhaseM, 1.0);
      }
    }

//  Qi.QZero();
//  Qi.SetPress(PMin);
//  int NFeeds=0;
//  for (i=0; i<NoProcessIOs(); i++)
//    if (IO_In(i) && ((IOIdMask_Self(i) & IOIds)!=0))
//      {
//      if (RqdMdl)
//        Qi.ChangeModel(RqdMdl);
//      else
//        Qi.SelectModel(IOConduit(i), NFeeds==0);
//      Qi.QAddF(*IOConduit(i), som_ALL, 1.0);
//      NFeeds++;
//      }

  return PMin;
  }

//--------------------------------------------------------------------------
/*#F:
*/

double MdlNode::SigmaQInPMax(SpConduit & Qi, PhMask PhaseM, int IOId1, int IOIdN, TagObjClass * RqdMdl)
  {
  int    iQMax=-1;
  double QMax=0.0;
  double PMax=0.01;
  CSpMdlSlct Slct;
  for (int i=0; i<NoProcessIOs(); i++)
    if (IO_In(i) && (IOId_Self(i) >= IOId1) && (IOId_Self(i) <= IOIdN))
      {
      PMax=Max(PMax, IOP_Self(i));
      Slct.Add(IOConduit(i)->Model(), IOQm_In(i));
      //if (IOQm_In(i)>QMax)
      //  {
      //  QMax=IOQm_In(i);
      //  iQMax=i;
      //  }
      }

  Qi.QZero();
  Qi.SetPress(PMax);
  if (RqdMdl)
    Qi.ChangeModel(RqdMdl);
  else //if (iQMax>=0)
    Qi.SelectModel(Slct);//IOConduit(iQMax), true);
  for (i=0; i<NoProcessIOs(); i++)
    if (IO_In(i) && (IOId_Self(i) >= IOId1) && (IOId_Self(i) <= IOIdN))
      //if (IO_In(i) && ((IOIdMask_Self(i) & IOIds)!=0))
      Qi.QAddF(*IOConduit(i), PhaseM, 1.0);
//  Qi.QZero();
//  Qi.SetPress(PMax);
//  if (RqdMdl)
//    Qi.ChangeModel(RqdMdl);
//  else if (iQMax>=0)
//    Qi.SelectModel(IOConduit(iQMax), true);
//  for (i=0; i<NoProcessIOs(); i++)
//    if (IO_In(i) && (IOId_Self(i) >= IOId1) && (IOId_Self(i) <= IOIdN))
//      Qi.QAddF(*IOConduit(i), PhaseM, 1.0);
  return PMax;
  }

//--------------------------------------------------------------------------
/*#F:
*/

double MdlNode::SigmaQInPMin(SpConduit & Qi, PhMask PhaseM, int IOId1, int IOIdN, TagObjClass * RqdMdl)
  {
  int    iQMax=-1;
  double QMax=0.0;
  double PMin=100000.0;
  CSpMdlSlct Slct;
  for (int i=0; i<NoProcessIOs(); i++)
    if (IO_In(i) && (IOId_Self(i) >= IOId1) && (IOId_Self(i) <= IOIdN))
      {
      PMin=Min(PMin, IOP_Self(i));
      Slct.Add(IOConduit(i)->Model(), IOQm_In(i));
      //if (IOQm_In(i)>QMax)
      //  {
      //  QMax=IOQm_In(i);
      //  iQMax=i;
      //  }
      }

  Qi.QZero();
  Qi.SetPress(PMin);
  if (RqdMdl)
    Qi.ChangeModel(RqdMdl);
  else //if (iQMax>=0)
    Qi.SelectModel(Slct);//IOConduit(iQMax), true);
  for (i=0; i<NoProcessIOs(); i++)
    if (IO_In(i) && (IOId_Self(i) >= IOId1) && (IOId_Self(i) <= IOIdN))
      //if (IO_In(i) && ((IOIdMask_Self(i) & IOIds)!=0))
      Qi.QAddF(*IOConduit(i), PhaseM, 1.0);
//  Qi.QZero();
//  Qi.SetPress(PMin);
//  if (RqdMdl)
//    Qi.ChangeModel(RqdMdl);
//  else if (iQMax>=0)
//    Qi.SelectModel(IOConduit(iQMax), true);
//  for (i=0; i<NoProcessIOs(); i++)
//    if (IO_In(i) && (IOId_Self(i) >= IOId1) && (IOId_Self(i) <= IOIdN))
//      Qi.QAddF(*IOConduit(i), PhaseM, 1.0);
  return PMin;
  }

//--------------------------------------------------------------------------
/*#F:
*/

void MdlNode::SigmaQOut(SpConduit & Qo, PhMask PhaseM, IDMask IOIds)
  {
  int    iQMax=-1;
  double QMax=0.0;
  double  PMax=0.0;
  for (int i=0; i<NoProcessIOs(); i++)
    if (IO_Out(i) && ((IOIdMask_Self(i) & IOIds)!=0))
      {
      PMax=Max(PMax, IOP_Self(i));
      if (IOQm_Out(i)>QMax)
        {
        QMax=IOQm_Out(i);
        iQMax=i;
        }
      }

  Qo.QZero();
  Qo.SetPress(PMax);
  //if (RqdMdl)
  //  Qo.ChangeModel(RqdMdl);
  //else
  if (iQMax>=0)
    Qo.ChangeModel(IOConduit(iQMax));//ue);
//  int NFeeds=0;
  for (i=0; i<NoProcessIOs(); i++)
    if (IO_Out(i) && ((IOIdMask_Self(i) & IOIds)!=0))
      Qo.QAddF(*IOConduit(i), som_ALL, 1.0);
  }

//--------------------------------------------------------------------------
/*#F:
*/

double MdlNode::SigmaQv(PhMask PhaseM, IDMask IOIds)
  {
  double Qv=0.0;
  for (int i=0; i<NoProcessIOs(); i++)
    if ((IOIdMask_Self(i) & IOIds)!=0)
      if (IO_Out(i))
        Qv -= IOConduit(i)->QVolume(PhaseM);
      else if (IO_In(i))
        Qv += IOConduit(i)->QVolume(PhaseM);
  return Qv;
  }

//--------------------------------------------------------------------------
/*#F:
*/

double MdlNode::SigmaQvIn(PhMask PhaseM, IDMask IOIds)
  {
  double Qv=0.0;
  for (int i=0; i<NoProcessIOs(); i++)
    if ((IOIdMask_Self(i) & IOIds)!=0)
      if (IO_In(i))
        Qv += IOConduit(i)->QVolume(PhaseM);
  return Qv;
  }

//--------------------------------------------------------------------------

double MdlNode::SigmaQvOut(PhMask PhaseM, IDMask IOIds)
  {
  double Qv=0.0;
  for (int i=0; i<NoProcessIOs(); i++)
    if ((IOIdMask_Self(i) & IOIds)!=0)
      if (IO_Out(i))
        Qv += IOConduit(i)->QVolume(PhaseM);
  return Qv;
  }

//--------------------------------------------------------------------------

void MdlNode::SetOutputFlows(CHgtOrdInfo & HO, int IONo, double LVolFrac, double VVolFrac)
  {
  SpContainer & Contents=HO.Container();
  SpConduit   & ContentFlw=HO.ContentFlw();
  SpModel     & Cn = *(SolveInlineMethod() ? ContentFlw.Model() : Contents.Model());

  
  SpConduit & Fo = *IOConduit(IONo);
  double Vf=VVolFrac;
  double Lf=LVolFrac;


  if (0) // correct for entrainment
    {
    //double L=Contents.Level(); // Includes Entrained
    //double S=Contents.Volume(som_SL);
    //double Ls=Contents.Shape()->Level(0,S);
    //// Frac of Apparent Slurry that is Vapour
    //double SLV=

    double dVf=Contents.Level()-Contents.Level(false);
    Vf+=dVf;
    Lf-=dVf;
    }

  CSpPropInfo & NP =*IONetProps(IONo);

  // Convert VolFrac to MassFracs
  double Qv, Qs;

  if (IO_Self(IONo).IsPMUImage() || IO_Self(IONo).IsPMUAvlImage())
    {
    //dbgpln("  Out %-12.12s : Image", Nd_Rmt(IONo)->FullObjTag());
    IO_Self(IONo).SetPMUImage(False);
    IO_Self(IONo).SetPMUAvlImage(False);
    //TRACE ("MdlNode::SetOutputFlows Take Account of QInEst\n");
    SpConduit & Img=Contents.QOutEst();
    //Contents.QOutEst().ChangeModel(Contents.pModel->ClassId());
    //CNM bug in MG Glycol Cont : Contents.QOutEst().QCopy(Fo);

    // Fetch the image than change model to
    // same model as in container
    Img.QCopy(Fo);
    Img.CopyModel(&Cn);
    Img.SetTempPress(Cn.Temp(), Cn.Press());

    // When would this be needed ???????
    double DensCorr=1;//Img.Rho(som_SL)/GTZ(Contents.Rho(som_SL));
    LVolFrac*=DensCorr;
    double VMassFrac=VVolFrac*NP.AsVMassFrac(VVolFrac, LVolFrac, Cn.Press());
    double LMassFrac=LVolFrac*NP.AsLMassFrac(VVolFrac, LVolFrac, Cn.Press());
    Qv=VMassFrac*IOQmEst_Out(IONo);
    Qs=LMassFrac*IOQmEst_Out(IONo);

    if (Cn.Mixed())
      {
      // Level is ignored
      Fo.QSetM(Img, som_ALL, Qs+Qv, IOP_Self(IONo));
      }
    else
      {
      Fo.QSetM(Img, som_Gas, Qv, IOP_Self(IONo));
      Fo.QAddM(Img, som_SL,  Qs);
      }
    //Fo.SetTemp(Contents.Temp());
    //Fo.CopyModel(Contents.Model());
    }
  else if (IO_Self(IONo).IsPMUPassing() || IO_Self(IONo).IsPMUBlocking())
    {
    //dbgpln("  Out %-12.12s : Pass/Block", Nd_Rmt(IONo)->FullObjTag());
    SpMArray FiltSL(*(IOConduit(IONo)->Model()));
    FiltSL.SetToOnesAndZeros(IO_Self(IONo).IsPMUBlocking());
    SpMArray FiltVap(FiltSL);
    FiltSL.ZeroPhase(som_Gas);
    FiltVap.ZeroPhase(som_SL);
    //TRACE ("MdlNode::SetOutputFlows Take Account of QInEst\n");

    double DensCorr=FiltSL.Rho(IOConduit(IONo)->Model()->Fidelity(), som_SL, Cn.Temp(), Cn.Press(), NULL)/GTZ(Cn.Rho(som_SL));
    LVolFrac*=DensCorr;
    double VMassFrac=VVolFrac*NP.AsVMassFrac(VVolFrac, LVolFrac, Cn.Press());
    double LMassFrac=LVolFrac*NP.AsLMassFrac(VVolFrac, LVolFrac, Cn.Press());
    Qv=VMassFrac*IOQmEst_Out(IONo);
    Qs=LMassFrac*IOQmEst_Out(IONo);

    //Qv=VFrac*Info.QEst[k];//Out[k];
    //Qs=SFrac*Info.QOut[k];
    Fo.QSetM(Cn, FiltVap, Qv, IOP_Self(IONo));
    Fo.QAddM(Cn, FiltSL, Qs);
    Fo.SetTempPress(Cn.Temp(), Cn.Press());
    IO_Self(IONo).SetPMUPassing(False);
    IO_Self(IONo).SetPMUBlocking(False);
    }
  else
    {
    double VMassFrac=VVolFrac*NP.AsVMassFrac(VVolFrac, LVolFrac, Cn.Press());
    double LMassFrac=LVolFrac*NP.AsLMassFrac(VVolFrac, LVolFrac, Cn.Press());
    Qv=VMassFrac*IOQmEst_Out(IONo);
    Qs=LMassFrac*IOQmEst_Out(IONo);

    //dbgpln("  Out %-12.12s : NetProps : %14.6gl, %14.6gv", Nd_Rmt(IONo)->FullObjTag(), NP.m_dLVolFrac, NP.m_dVVolFrac);

    if (Contents.Closed())
      Fo.QSetM(Cn, som_Gas, Qv, IOP_Self(IONo));
    else
      {
      Fo.QRestoreMass(gs_Environment);
      Fo.SetTempPress(AmbientTemp(), AtmosPress());
      Fo.QAdjustQmTo(som_ALL, Qv);
      }

    Fo.QAddM(Cn, som_SL, Qs);
    double Qv_=Fo.QMass(som_Gas);
    double Qs_=Fo.QMass(som_SL);
    //if (Qv_<0.99*Qv)
    //  Fo.QAddM(QEstIn, som_Gas, Qv-Qv_);
    //if (Qs_<0.99*Qs)
    //  Fo.QAddM(QEstIn, som_SL, Qs-Qs_);
    Fo.SetTempPress(Cn.Temp(), Cn.Press());
    }

  #if dbgDerivs
  if (dbgDerivsSrg2() || fDoDbgBrk && dbgDerivsSrg2DbgBrk())
    {
    dbgpln("  <O< %-12.12s : %14.6gl, %14.6gv", Nd_Rmt(IONo)->FullObjTag(),Qs,Qv);
    dbgpln("      %-12.12s : %14.6gl, %14.6gv", Nd_Rmt(IONo)->FullObjTag(),Fo.QMass(som_SL),Fo.QMass(som_Gas));
    dbgpln("   Cn %-12.12s : %14.6gl, %14.6gv", Nd_Rmt(IONo)->FullObjTag(),Cn.Mass(som_SL),Cn.Mass(som_Gas));
  //  dbgSpFlows("QMTtl", FullObjTag(), Fo,0);
    }
  #endif

  }

//--------------------------------------------------------------------------

void MdlNode::EvalProductsInit_Source(EvalProductsInitTasks Task, int IoNo, double SLQm, double VQm)
  {
  switch (Task)
    {
    case EPI_SurgeEval:
      {
      if (IO_Out(IoNo))
        {
        SetIOQmAvail_Self(IoNo, SLQm);
        }
      else if (IO_In(IoNo))
        {
        SetIOQmAvail_Self(IoNo, 0.0);
        }
      }
      break;
    case EPI_FwdEval:
      break;
    case EPI_RevEval:
      break;
    }
  }

//--------------------------------------------------------------------------

void MdlNode::EvalProductsInit_SurgeLevel(EvalProductsInitTasks Task, SpContainer &Contents, IDMask IOIds)
  {
  switch (Task)
    {
    case EPI_SurgeEval:
      {
      double L=Contents.Level();
      double M0=Contents.Mass(som_SL);
      for (int i=0; i<NoProcessIOs(); i++)
        {
        if (IOIdMask_Self(i) & IOIds)
          {
          if (IO_Out(i))
            {
            double A=M0*GEZ(L-IOAbsFracHgt_Term(i, Contents))/(GTZ(L)*2.0*Max(1.0, ICGetTimeIncMx()));
            SetIOQmAvail_Self(i, A);
            }
          else if (IO_In(i))
            {
            SetIOQmAvail_Self(i, 0.0);
            }
          }
        }
      }
      break;
    case EPI_FwdEval:
      break;
    case EPI_RevEval:
      break;
    }
  }

//--------------------------------------------------------------------------

void MdlNode::EvalProductsInit_Node(EvalProductsInitTasks Task, IDMask IOIds)
  {
  switch (Task)
    {
    case EPI_SurgeEval:
      break;
    case EPI_FwdEval:
      {
      double QmOTot=0.0;
      double QmAvTot=0.0;
      int iOut[MaxIOList];
      int nOut=0;
      for (int i=0; i<NoProcessIOs(); i++)
        {
        if (IOIdMask_Self(i) & IOIds)
          {
          if (IO_Out(i))
            {
            iOut[nOut++]=i;
            QmOTot+=IOQm_Out(i);
            }
          else if (IO_In(i))
            {
            QmAvTot+=IOQmAvail_Rmt(i);
            }
          }
        }

      QmOTot=GTZ(QmOTot);
      for (int j=0; j<nOut; j++)
        {
        i=iOut[j];
        SetIOQmAvail_Self(i, QmAvTot*IOQm_Out(i)/QmOTot);
        }

      int xxx=0;
      }
      break;
    case EPI_RevEval:
      {
      double QmITot=0.0;
      double QmOTot=0.0;
      double QmSpTot=0.0;
      double QmAvTot=0.0;
      int iIn[MaxIOList];
      int nIn=0;
      for (int i=0; i<NoProcessIOs(); i++)
        {
        if (IOIdMask_Self(i) & IOIds)
          {
          if (IO_Out(i))
            {
            QmOTot+=IOQm_Out(i);
            QmSpTot+=IOQmSpace_Rmt(i);
            }
          else if (IO_In(i))
            {
            iIn[nIn++]=i;
            QmITot+=IOQm_In(i);
            QmAvTot+=IOQmAvail_Rmt(i);
            }
          }
        }

      QmAvTot=GTZ(QmAvTot);
      for (int j=0; j<nIn; j++)
        {
        i=iIn[j];
        SetIOQmSpace_Self(i, QmSpTot*IOQmAvail_Rmt(i)/QmAvTot);
        }

      }
      break;
    }
  };

//--------------------------------------------------------------------------

void MdlNode::EvalProductsInit_Link(EvalProductsInitTasks Task)
  {
  if (NoProcessIOs()>=2)
    {
    double q;
    int iIn=IO_In(0) ? 0 : 1;
    int iOut=(iIn+1)%2;
    switch (Task)
      {
      case EPI_FwdEval:
        q=IOQmAvail_Rmt(iIn);
        SetIOQmAvail_Self(iIn, q);
        SetIOQmAvail_Self(iOut, q);
        break;
      case EPI_RevEval:
        q=IOQmSpace_Rmt(iOut);
        SetIOQmSpace_Self(iOut, q);
        SetIOQmSpace_Self(iIn, q);
        break;
      }
    }
  };

//--------------------------------------------------------------------------

inline  double Smooth(double x) { return (x> 0.5) ?  1.0-(2.0*Sqr(1.0-x)) : 2.0*Sqr(x); };

//--------------------------------------------------------------------------

void MdlNode::VentVapours(SpContainer &Contents)
  {
  // Vent Vapours
  flag GasVentNeeded=0;
  //if (Contents.VentExists())
    Contents.Vent.Cd.QZero();

  for (int i=0; i<SDB.Count();i++)
    if (SDB[i].IsGas() && Contents.VMass[i]>1.0e-30)
      {
      GasVentNeeded=1;
      double RqdMass=GEZ(Contents.VMass[i]/Max(Max(1.0, ICGetTimeInc()) * 1.5, Contents.VapourEscapeTau()));
      Contents.Vent.Cd.SetVMass(i, Contents, RqdMass);
      }

  if (GasVentNeeded)
    Contents.Vent.Cd.SetTempPress(Contents.Temp(), Contents.Press());
  else
    Contents.Vent.Cd.QSetTraceMass();
  }

//--------------------------------------------------------------------------

void MdlNode::DoOverflowLinks(CHgtOrdInfo & HO)
  {
  SpContainer &Contents=HO.Container();
  // Setup Overflow product
  if (Contents.Spill.Flowing())
    {
    if (HO.NOvr()>0)
      {
      double QOvr=GTZ(Contents.Spill.Cd.QMass(som_SL));
      double QOut=0.0;
      for (int i=0; i<HO.NOvr(); i++)
        QOut+=IOQmEst_Out(HO.Ovr[i]);

      double Scl=Min(1.0, QOvr/GTZ(QOut));
      for (i=0; i<HO.NOvr(); i++)
        IOConduit(HO.Ovr[i])->QSetM(Contents.Spill.Cd, som_SL, Scl*IOQmEst_Out(HO.Ovr[i]), Std_P);
      Contents.Spill.Cd.QScaleMass(som_ALL, GEZ(1.0-QOut/QOvr));
      }

    //if (Contents.Spill.Discard())
    //  {
    //  Contents.ClrCI(2);
    //  Contents.ClrCI(3);
    //  }
    //else if (Contents.Spill.Target().Valid())
    //  {
    //  SpContainer &C=Contents.Spill.Target().Container();
    //  if (&C!=&Contents)
    //    {
    //    Contents.ClrCI(2);
    //    //TODO Spills
    //    _asm int 3;
    //    //SpDirectCd &I=C.InFlow();
    //    //if (I.ZeroReqd())
    //    //  {
    //    //          
    //    //  I.ClrZeroReqd();
    //    //  I.QSetF(Contents.Spill, som_ALL, 1.0, Std_P);
    //    //  dbgpln("Target SET %14.3f %14.3f %s", Contents.Spill.QMass(), I.QMass(), I.Target().Container().FullObjTag());
    //    //  }
    //    //else
    //    //  {
    //    //  I.QAddF(Contents.Spill, som_ALL, 1.0);
    //    //  dbgpln("Target ADD %14.3f %14.3f %s", Contents.Spill.QMass(), I.QMass(), I.Target().Container().FullObjTag());
    //    //  }
    //    }
    //  else
    //    Contents.SetCI(2);
    //  Contents.ClrCI(3);
    //  }
    //else
    //  {
    //  Contents.ClrCI(2);
    //  char *pS=Contents.Spill.Target().Tag();
    //  Contents.SetCI(3, "E\tOverflow Target '%s' not Found", pS ? pS : "");
    //  }
    }
  else
    {
    Contents.ClrCI(2);
    Contents.ClrCI(3);
    for (int i=0; i<HO.NOvr(); i++)
      IOConduit(HO.Ovr[i])->QZero();
    }
  }

//--------------------------------------------------------------------------

void MdlNode::DoGasVentLinks(CHgtOrdInfo & HO)
  {
  SpContainer &Contents=HO.Container();
  // Setup Gasvent product
  if (Contents.Vent.Flowing())
    {
    if (HO.NGas()>0)
      {
      double QGas=GTZ(Contents.Vent.Cd.QMass(som_Gas));
      double QOut=0.0;
      for (int i=0; i<HO.NGas(); i++)
        if (IO_Out(HO.Gas[i]))
          QOut+=IOQmEst_Out(HO.Gas[i]);

      double Scl=Min(1.0, QGas/GTZ(QOut));
      for (i=0; i<HO.NGas(); i++)
        if (IO_Out(HO.Gas[i]))
          IOConduit(HO.Gas[i])->QSetM(Contents.Vent.Cd, som_Gas, Scl*IOQmEst_Out(HO.Gas[i]), Std_P);
      Contents.Vent.Cd.QScaleMass(som_ALL, GEZ(1.0-QOut/QGas));
      }

    //if (Contents.Vent.Discard())
    //  {
    //  Contents.ClrCI(5);
    //  Contents.ClrCI(6);
    //  }
    //else if (Contents.Vent.Target().Valid())
    //  {
    //  SpContainer &C=Contents.Vent.Target().Container();
    //  if (&C!=&Contents)
    //    {
    //    Contents.ClrCI(5);
    //    _asm int 3;
    //    //SpDirectCd &I=C.InFlow();
    //    //if (I.ZeroReqd())
    //    //  {
    //    //  I.ClrZeroReqd();
    //    //  I.QSetF(Contents.Vent(), som_ALL, 1.0, Std_P);
    //    //  }
    //    //else
    //    //  I.QAddF(Contents.Vent(), som_ALL, 1.0);
    //    }
    //  else
    //    Contents.SetCI(5);
    //  Contents.ClrCI(6);
    //  }
    //else
    //  {
    //  Contents.ClrCI(5);
    //  char *pS=Contents.Vent.Target().Tag();
    //  Contents.SetCI(6, "E\tGasVent Target '%s' not Found", pS ? pS : "");
    //  }
    }
  else
    {
    Contents.ClrCI(5);
    Contents.ClrCI(6);
    for (int i=0; i<HO.NGas(); i++)
      {
      const int ii = HO.Gas[i];
      //if (ii>=0 && ii<NoFlwIOs())//if (IOConduit(ii))
        IOConduit(ii)->QZero();
      }
    }
  }

//--------------------------------------------------------------------------

void MdlNode::EvalProducts_SurgeLevel(CHgtOrdInfo & HO, SpConduit * pQFeed, CSurgeControl *pControl)
  {
  if (SolveInlineMethod())
    {
    ASSERT_ALWAYS(pQFeed!=NULL, "Inline integral - Feed not supplied")
    EvalProducts_SurgeLevelInline(HO, pQFeed, pControl);
    }
  else
    EvalProducts_SurgeLevelDeriv(HO, pQFeed, pControl);
  };

//--------------------------------------------------------------------------

void MdlNode::EvalProducts_SurgeLevelDeriv(CHgtOrdInfo & HO, SpConduit * pQFeed, CSurgeControl *pControl)
  {
  #if dbgDerivs
  long dbg1=dbgDerivsSrg1(FullObjTag())       || fDoDbgBrk && dbgDerivsSrg1DbgBrk();
  long dbg2=dbgDerivsSrg2(FullObjTag())       || fDoDbgBrk && dbgDerivsSrg2DbgBrk();
  long dbg3=dbgDerivsSrg3(FullObjTag())       || fDoDbgBrk && dbgDerivsSrg3DbgBrk();
  long dbgo=dbgDerivsSrgOvr(FullObjTag())     || fDoDbgBrk && dbgDerivsSrgOvrDbgBrk();
  long dbgso=dbgDerivsSrgSetOut(FullObjTag()) || fDoDbgBrk && dbgDerivsSrgSetOutDbgBrk();
  #endif

  SpContainer & Contents=HO.Container();
  //SpConduit   & ContentFlw=HO.ContentFlw();
  //ContentFlw.QSetF(Contents, som_ALL, 1.0/ICGetTimeInc());
  //ContentFlw.QAddF(QFeed, som_ALL, 1.0);

  for (int i = 0; i < NoProcessIOs(); i++)
    {
    if ((IOIdMask_Self(i) & HO.m_dwIOIdMask)!=0)
      {
      if ((IOExitRule(i)&IOER_Lvl)==0)// && IOFlange(i)->PMUOptions()==0)
        {
        byte ER=IOExitRule(i);

        double EntVapFrac=Range(0.0, Contents.VapourEntrained()/GTZ(Contents.Mass(som_Gas)),1.0);

        double SScl=(ER&IOER_Sol) ? 1.0 : 0.0;
        double LScl=(ER&IOER_Liq) ? 1.0 : 0.0;
        double VScl=(ER&IOER_Vap) ? 1.0-LScl*EntVapFrac : LScl*EntVapFrac;

        SpMArray Img;
        Img.Add(Contents, SScl, LScl, VScl);

        if (ER&IOER_Img)
          {
          SpVectorObj *IOImg = IOExitImage(i);
          double ImgTotal=0, IOImgTotal=0; 
          for (int s=0; s<SDB.Count(); s++)
            {
            if (Valid(IOImg->M[s]))
              {
              ImgTotal+=Img[s]; 
              IOImgTotal+=IOImg->M[s]; 
              Img.SetVMass(s, IOImg->M, IOImg->M[s]);
              }
            else if (HasNANFlag(IOImg->M[s], NF_Block))
              {
              ImgTotal+=Img[s]; 
              IOImgTotal+=0; 
              Img.SetVMass(s, IOImg->M, 0);
              }
            }
          double Scl=ImgTotal/GTZ(IOImgTotal);
          for (int s=0; s<SDB.Count(); s++)
            {
            if (Valid(IOImg->M[s]))
              Img.SclVMass(s, Scl);
            else if (HasNANFlag(IOImg->M[s], NF_Block))
              Img.SclVMass(s, 0);
            }
          }

        SetProdMakeup(PMU_IONo|PMU_Image, i, Img, Contents.Temp(), Contents.Press(), Contents.Model());
        }
      }
    }

  //double QmActual_SL=0.0;
  //double QvActual_SL=0.0;

  #if dbgDerivs
  if (dbg1)
    {
    dbgpln("------- %s.%s", Tag(),Contents.Tag());
    dbgpln(" ALvl %7.3f%%",Contents.Level()*100) ;
    dbgpln(" Cntnt%-12.12s : %14.6gl, %14.6gv", "",Contents.Mass(som_SL),Contents.Mass(som_Gas));
    dbgpln(" QFeed%-12.12s : %14.6gl, %14.6gv", "",pQFeed->QMass(som_SL),pQFeed->QMass(som_Gas));
    //dbgpln(" Est  %-12.12s : %14.6gl, %14.6gv", "",QInEst.QMass(som_SL),QInEst.QMass(som_Gas));
    }
  #endif

  for (int j=0; j<HO.NOrd(); j++)
    {
    int iIONo=HO.Ord[j];
    if (!IO_In(iIONo))
      {
      double LVolFrac=IONetProps(iIONo)->LVolFrac();
      double VVolFrac;
      if (Contents.Closed())
        VVolFrac=1.0-LVolFrac;
      //else if (gs_Environment.StateOK())
      //  VFrac=1.0-SFrac;
      else
        VVolFrac=0.0;

      SetOutputFlows(HO, iIONo, LVolFrac, VVolFrac);
      }
    }

  //==========================================================
  // still gas & overflows

  if (Contents.Closed()) // GetRid of Overflow Conduit
    {
    if (Contents.SpillExists())
      Contents.Spill.Cd.QSetTraceMass();
    if (Contents.VentExists())
      Contents.Vent.Cd.QSetTraceMass();
    }
  else if (!Contents.Mixed() && !Contents.Closed())
    {
    //    if (Contents.Level()>1.0)
//      {
//      ASSERT_ALWAYS(FALSE, "INCOMPLETE")
//      }

    if (DoDbgBrk())
      { int xxx=0; };

    double SettleTm=Max(Max(ICGetTimeIncMx()*sc_dTSettleRatio, ICGetSettleTime()), Contents.dOverFlowSettle);
    double QmSLRqd=Max(1.0e-6, (Contents.Level()-1.0)*Contents.Mass(som_SL)/SettleTm);
    double QvSLRqd=QmSLRqd/Min_FE_Rho(Contents.SLEVDensity());

    double QmSLFeed=0;      // total Slurry feed into this Vessel
    double QvSLFeed=0;      // total Slurry feed into this Vessel
    double QvSLOutBelow=0;  // Slurry leaving affecting this IO

    GetCnFlows(false, HO, -1, QmSLFeed, QvSLFeed, QvSLOutBelow);

    double Aperture=0.01;
    double FHgt=1.0-Aperture;

    //SpMArray Image;
    double SVolFrac=GetSVolFrac(Contents, -1, Aperture, FHgt, QvSLRqd, QvSLFeed, QvSLOutBelow, NULL);

    if (SVolFrac>1.0e-12) // Got FLow
      {
      SVolFrac=Range(0.0, SVolFrac, 1.0);
      double Qs=SVolFrac*QmSLRqd;
      ASSERT_RDB(Qs>=0.0, "Why is Qs negative !!!"); //kga 10/12/02 ERROR!!! (Set to zero for now)
      Contents.Spill.Cd.QSetM(Contents, som_SL, Max(0.0, Qs), Contents.Press());
      //Contents.Spill.QSetM(Contents, som_SL, Qs, Contents.Press());
      //QvActual_SL+=Qs;
      }
    else
      {
      if ((Contents.Level()<0.90) && Contents.Closed()) // GetRid of Overflow Conduit
        Contents.Spill.Cd.QSetTraceMass();
      else //if (Contents.SpillExists())
        Contents.Spill.Cd.QZero();
      }

    if (Contents.Closed())
      {
      Contents.Spill.Cd.QSetTraceMass();
      Contents.Vent.Cd.QSetTraceMass();
      }
    else
      {
      VentVapours(Contents);
      DoOverflowLinks(HO);
      DoGasVentLinks(HO);
      }
    }
  }

//--------------------------------------------------------------------------

void MdlNode::EvalProducts_SurgeLevelInline(CHgtOrdInfo & HO, SpConduit * pQFeed, CSurgeControl *pControl)
  {
  #if dbgDerivs
  long dbg1=dbgDerivsSrg1(FullObjTag()) || fDoDbgBrk && dbgDerivsSrg1DbgBrk(FullObjTag());
  long dbg2=dbgDerivsSrg2(FullObjTag()) || fDoDbgBrk && dbgDerivsSrg2DbgBrk(FullObjTag());
  long dbg3=dbgDerivsSrg3(FullObjTag()) || fDoDbgBrk && dbgDerivsSrg3DbgBrk(FullObjTag());
  long dbgo=dbgDerivsSrgOvr()           || fDoDbgBrk && dbgDerivsSrgOvrDbgBrk();
  long dbgso=dbgDerivsSrgSetOut(FullObjTag()) || fDoDbgBrk && dbgDerivsSrgSetOutDbgBrk();
  #endif

  SpContainer & Contents=HO.Container();
  SpConduit   & ContentFlw=HO.ContentFlw();
  ContentFlw.QSetF(Contents, som_ALL, 1.0/ICGetTimeInc());
  ContentFlw.QAddF(*pQFeed, som_ALL, 1.0);

  // DoReaction 
//TODO EvalProducts_SurgeLevelInline - DoReaction
  if (fDoDbgBrk)
    { int xxx=0; };

  for (int i = 0; i < NoProcessIOs(); i++)
    {
    if ((IOIdMask_Self(i) & HO.m_dwIOIdMask)!=0)
      {
      if ((IOExitRule(i)&IOER_Lvl)==0)// && IOFlange(i)->PMUOptions()==0)
        {
        byte ER=IOExitRule(i);

        double EntVapFrac=Range(0.0, Contents.VapourEntrained()/GTZ(Contents.Mass(som_Gas)),1.0);

        double SScl=(ER&IOER_Sol) ? 1.0 : 0.0;
        double LScl=(ER&IOER_Liq) ? 1.0 : 0.0;
        double VScl=(ER&IOER_Vap) ? 1.0-LScl*EntVapFrac : LScl*EntVapFrac;

        SpMArray Img;
        Img.Add(ContentFlw, SScl, LScl, VScl);

        if (ER&IOER_Img)
          {
          SpVectorObj *IOImg = IOExitImage(i);
          double ImgTotal=0, IOImgTotal=0; 
          for (int s=0; s<SDB.Count(); s++)
            {
            if (Valid(IOImg->M[s]))
              {
              ImgTotal+=Img[s]; 
              IOImgTotal+=IOImg->M[s]; 
              Img.SetVMass(s, IOImg->M, IOImg->M[s]);
              }
            else if (HasNANFlag(IOImg->M[s], NF_Block))
              {
              ImgTotal+=Img[s]; 
              IOImgTotal+=0; 
              Img.ClrVMass(s);
              }
            }
          double Scl=ImgTotal/GTZ(IOImgTotal);
          for (int s=0; s<SDB.Count(); s++)
            {
            if (Valid(IOImg->M[s]))
              Img.SclVMass(s, Scl);
            else if (HasNANFlag(IOImg->M[s], NF_Block))
              Img.ClrVMass(s);
            }
          }

        SetProdMakeup(PMU_IONo|PMU_Image, i, Img, ContentFlw.Temp(), ContentFlw.Press(), ContentFlw.Model());
        }
      }
    }

  #if dbgDerivs
  if (dbg1)
    {
    dbgpln("------- %s.%s", Tag(),Contents.Tag());
    dbgpln(" Cntnt%-12.12s : %14.6gl, %14.6gv", "",Contents.Mass(som_SL),Contents.Mass(som_Gas));
    dbgpln(" QFeed%-12.12s : %14.6gl, %14.6gv", "",pQFeed->QMass(som_SL),pQFeed->QMass(som_Gas));
    dbgpln(" Qm   %-12.12s : %14.6gl, %14.6gv", "",ContentFlw.QMass(som_SL),ContentFlw.QMass(som_Gas));
    dbgpln(" Qv   %-12.12s : %14.6gl, %14.6gv", "",ContentFlw.QVolume(som_SL),ContentFlw.QVolume(som_Gas));
    }
  #endif

  int iLvlCtrl=pControl ? pControl->StreamIndex() : -1;
  Contents.SetCI(7, pControl && pControl->m_iStreamIndex<0 && pControl->m_bOn && pControl->m_bEnabled);

  bool DoneIt[MaxIOList];
  int HOSeq=-1;
  double PrevFHgt=0;
  for (int j=0; j<HO.NOrd(); )//j++)
    {
    int iIONo=HO.Ord[j];
    DoneIt[iIONo]=false;
    if (!IO_In(iIONo) && HO.Seq[j]>HOSeq)
      {
          
      #if dbgDerivs
      if (dbgso)
        dbgpln("  IOOut %3i Calc %s %s",iIONo, FullObjTag(), Nd_Rmt(iIONo)->FullObjTag());
      #endif
      HOSeq=HO.Seq[j];

      int j1;
      double FlwDmd=0;
      double CtrlDmd=0;
      bool CtrlNow=false;
      for (j1=j; j1<HO.NOrd() && HO.Seq[j1]==HOSeq; j1++)
        {
        if (!IO_In(HO.Ord[j1]))
          {
          if (iLvlCtrl==HO.Ord[j1])
            {
            CtrlDmd=IOQmEst_Out(HO.Ord[j1]);
            CtrlNow=true;
            }
          else
            FlwDmd+=IOQmEst_Out(HO.Ord[j1]);
          }
        }

      double Flw=ContentFlw.QMass(som_SL);
      double FlwRetBelow=ContentFlw.Rho(som_SL)*Contents.ShapeVolume()*(IORelFracHgt_Flng(iIONo)-PrevFHgt)/ICGetTimeInc();
      double FlwRetCtrl=0;
#if WITHSURGECTRLDOUBLE 
      double CtrlFlw=0;
#else
      float CtrlFlw=0;
#endif
      if (iLvlCtrl>=0 && CtrlNow)
        {
        switch (pControl->m_iType)
          {
          case SC_Level:
            FlwRetCtrl=ContentFlw.Rho(som_SL)*Contents.ShapeVolume()*GEZ(pControl->m_dLevelSPT-IORelFracHgt_Flng(iLvlCtrl))/ICGetTimeInc();
            break;
          case SC_Mass:
            FlwRetCtrl=GEZ(pControl->m_dMassSPT-ContentFlw.Rho(som_SL)*Contents.ShapeVolume()*IORelFracHgt_Flng(iIONo))/ICGetTimeInc();
            break;
          case SC_Volume:
            FlwRetCtrl=ContentFlw.Rho(som_SL)*GEZ(pControl->m_dVolumeSPT-Contents.ShapeVolume()*IORelFracHgt_Flng(iIONo))/ICGetTimeInc();
            break;
          }

#if WITHSURGECTRLDOUBLE 
        CtrlFlw=GEZ(Flw-(FlwRetBelow+FlwRetCtrl)-FlwDmd);
#else
        CtrlFlw=float(GEZ(Flw-(FlwRetBelow+FlwRetCtrl)-FlwDmd));
#endif
        CtrlFlw=Max(CtrlFlw, pControl->m_dQmMinStep);
        if (Valid(pControl->m_dQmMaxStep))
          CtrlFlw=Min(CtrlFlw, pControl->m_dQmMaxStep);
        pControl->m_dQmCalc=CtrlFlw;
        FlwDmd+=CtrlFlw;
        }

      ContentFlw.QScaleMass(som_SL, GEZ(Flw-FlwRetBelow)/GTZ(Flw));
      Flw=ContentFlw.QMass(som_SL);
      
      double FlwScl=Min(1.0, Flw/GTZ(FlwDmd));
      Flw=GTZ(Flw);

      for (j1=j; j1<HO.NOrd() && HO.Seq[j1]==HOSeq; j1++)
        if (!IO_In(HO.Ord[j1]))  
          {
          if (iLvlCtrl==HO.Ord[j1])
            {
            SetOutputFlows(HO, HO.Ord[j1], FlwScl*CtrlFlw/GTZ(CtrlDmd), 0.0);
            DoneIt[HO.Ord[j1]]=true;
            #if dbgDerivs
            if (dbgso)
              dbgpln("    %3i = %18.3f %s",HO.Ord[j1],IOQmEst_Out(HO.Ord[j1])*FlwScl, Nd_Rmt(HO.Ord[j1])->FullObjTag());
            #endif
            }
          else
            {
            SetOutputFlows(HO, HO.Ord[j1], FlwScl, 0.0);
            DoneIt[HO.Ord[j1]]=true;
            #if dbgDerivs
            if (dbgso)
              dbgpln("    %3i = %18.3f %s",HO.Ord[j1],IOQmEst_Out(HO.Ord[j1])*FlwScl, Nd_Rmt(HO.Ord[j1])->FullObjTag());
            #endif
            }
          }
      #if dbgDerivs
      double CF0=ContentFlw.QMass();
      #endif
      ContentFlw.QScaleMass(som_SL, GEZ(Flw-FlwScl*FlwDmd)/GTZ(Flw));
      #if dbgDerivs
      if (dbgso)
        dbgpln("    ContentFlw %18.6f >> %18.6f", CF0, ContentFlw.QMass());
      #endif
      PrevFHgt=IORelFracHgt_Flng(iIONo);
      j=j1; // 
      }
    else
      j++;
    }

  for (int j=0; j<HO.NOrd(); j++)
    {
    int iIONo=HO.Ord[j];
    if (!DoneIt[iIONo])
      {
      if (!IO_In(iIONo))
        {
        #if dbgDerivs
        if (dbgso)
          dbgpln("  IOOut %3i ZERO %s %s",iIONo, FullObjTag(), Nd_Rmt(iIONo)->FullObjTag());
        #endif
        SetOutputFlows(HO, iIONo, 0.0, 0.0/*VVolFrac*/);
        }
      else
        {
        #if dbgDerivs
        if (dbgso)
          dbgpln("  IOOut %3i Skip %s %s",iIONo, FullObjTag(), Nd_Rmt(iIONo)->FullObjTag());
        #endif
        }
      }
    }

  //==========================================================
  // still gas & overflows

  if (Contents.Closed()) // GetRid of Overflow Conduit
    {
    Contents.Spill.Cd.QSetTraceMass();
    Contents.Vent.Cd.QSetTraceMass();
    }
  else if (!Contents.Mixed() && !Contents.Closed())
    {
    if (DoDbgBrk())
      { int xxx=0; };

    double OvFHgt=1.0;
    double FlwRetBelow=ContentFlw.Rho(som_SL)*Contents.ShapeVolume()*(OvFHgt-PrevFHgt)/ICGetTimeInc();
    double Flw=ContentFlw.QMass(som_SL);
    ContentFlw.QScaleMass(som_SL, GEZ(Flw-FlwRetBelow)/GTZ(Flw));
    Flw=ContentFlw.QMass(som_SL);
    #if dbgDerivs
    if (dbgo)
      dbgpln("  Overflow Ret=%18.6f Ovr=%18.6f",FlwRetBelow, Flw);
    #endif
    if (Flw>1.0e-12)
      {
      Contents.Spill.Cd.QSetM(ContentFlw, som_SL, Flw, Contents.Press());
      }
    else
      {
      if (/*(Contents.Level()<0.90) &&*/ Contents.Closed()) // GetRid of Overflow Conduit
        Contents.Spill.Cd.QSetTraceMass();
      else //if (Contents.SpillExists())
        Contents.Spill.Cd.QZero();
      }

    if (Contents.Closed())
      {
      Contents.Spill.Cd.QSetTraceMass();
      Contents.Vent.Cd.QSetTraceMass();
      }
    else
      {
      VentVapours(Contents);
      DoOverflowLinks(HO);
      DoGasVentLinks(HO);
      }
    }
  }

//--------------------------------------------------------------------------

void MdlNode::EvalDerivs_SetQs_Xfer(SpContainer &Contents, IDMask IOIds)
  {
  #if dbgDerivs
  if (dbgDerivsXfer(/*sTag()*/))
    dbgpln("-NSrg-- SetQs -- %s.%s", FullObjTag(), Contents.Tag());
  #endif
  SpConduit qi("qi", this, TOA_Free);
  qi.QZero();
  double qot=0.0;

  for (int i = 0; i < NoProcessIOs(); i++)
    if ((IOIdMask_Self(i) & IOIds)!=0)
      {
      SpConduit & F = *IOConduit(i);
      if (IO_In(i))
        {
        qi.QAddF(F, som_ALL, 1.0);
        }
      else
        {
        qot += IOQmEst_Out(i);
        }
      }

  for (i = 0; i < NoProcessIOs(); i++)
    if (((IOIdMask_Self(i) & IOIds)!=0) && !IO_In(i))
      {
      SpConduit & F = *IOConduit(i);
      F.QSetM(qi, som_ALL, IOQmEst_Out(i), IOP_Self(i));
      }
  };

//--------------------------------------------------------------------------

static char IOTagTmp[256];

//const dword UIIOFBFlag =  0x00100000;

flag MdlNode::DataXchg(DataChangeBlk & DCB)
  {
  //if (DCB.dwUserInfo & UIIOFBFlag)
  //  {
  //  DWORD UI=DCB.dwUserInfo&(~UIIOFBFlag);
  //  DCB.dwUserInfo=0;
  //  if (IOFB(UI&0xff, UI>>8)->DataXchg(DCB))
  //    return 1;
  //  DCB.dwUserInfo=UI;
  //  }

  if (FlwNode::DataXchg(DCB))
    return true;
  if (GSM.DataXchg(DCB))
    return true;
  if (EHX.DataXchg(DCB))
    return true;

  if (DCB.lHandle>=xidFEFirst && DCB.lHandle<xidFELast)
    {
    XID Handle=((DCB.lHandle-xidFEFirst)/NxidFEIOs)*NxidFEIOs+xidFEFirst;
    int IONo=(DCB.lHandle-xidFEFirst)%NxidFEIOs;
    if (IONo>=NoFlwIOs())
      {
      DCB.D=dNAN;
      return true;
      }
    switch (Handle)
      {
      case xidFEArea       :  DCB.D=IOFB(IONo,0)->Area();           return true;
      case xidFELength     :  DCB.D=IOFB(IONo,0)->ActLength();      return true;
      case xidFEDZ         :  DCB.D=IOFB(IONo,0)->Rise();           return true;
      case xidFEVelocity   :  DCB.D=IOFB(IONo,0)->Velocity();       return true;
      case xidFEDPb        :  DCB.D=IOFB(IONo,0)->DPb();            return true;
      case xidFEDPbX       :  DCB.D=IOFB(IONo,0)->DPbX();           return true;
      case xidFEDPbdQ      :  DCB.D=IOFB(IONo,0)->DPbdQ();          return true;
      case xidFEDPq        :  DCB.D=IOFB(IONo,0)->DPq();            return true;
      case xidFEDPqdQ      :  DCB.D=IOFB(IONo,0)->DPqdQ();          return true;
      case xidFEDPz        :  DCB.D=IOFB(IONo,0)->DPz();            return true;
      case xidFEDPzdQ      :  DCB.D=IOFB(IONo,0)->DPzdQ();          return true;
      case xidFEDPmB       :  DCB.D=IOFB(IONo,0)->DPmB();           return true;
      case xidFEDPmQ       :  DCB.D=IOFB(IONo,0)->DPmQ();           return true;
      case xidFEDPmdQ      :  DCB.D=IOFB(IONo,0)->DPmdQ();          return true;
      case xidFEQm         :  DCB.D=IOFB(IONo,0)->GetQm();          return true;
      case xidFEQmEst      :  DCB.D=IOFB(IONo,0)->GetQmEst();       return true;
      case xidFEQmPrvPB    :  DCB.D=IOFB(IONo,0)->GetQmPrvPB();     return true;
      case xidFEQmAvail    :  DCB.D=IOFB(IONo,0)->GetQmAvail();     return true;
      case xidFEQmSpace    :  DCB.D=IOFB(IONo,0)->GetQmSpace();     return true;
      //case xidFEQmMemPB    :  DCB.D=IOFB(IONo,0)->GetQmMemPB();     return true;
      case xidFEBstDamping :  DCB.D=IOFB(IONo,0)->BstDamping();     return true;
      }
    }

  switch (DCB.lHandle)
    {
    case xidInlineIntegralOn: // Only for PrjFileVersion < 94
      if (DCB.rB)
        SetRunModes((*DCB.rB!=0) ? SM_Inline:SM_Buffered, SM_All);// InlineIntegral(*DCB.rB!=0);
      DCB.B=SolveInlineMethod();
      return true;
    case xidRho:
      if (NoProcessIOs() == 2)
        {
        int i= (IO_In(0) ? 0 : 1);
        DCB.D=IOConduit(i)->Rho(som_ALL);
        }
      else
        DCB.D=dNAN;
      return true;
    case xidNRho:
      if (NoProcessIOs() == 2)
        {
        int i= (IO_In(0) ? 0 : 1);
        DCB.D=IOConduit(i)->NRho(som_ALL);
        }
      else
        DCB.D=dNAN;
      return true;
    case xidQmEstType:
    case xidQmEstSrc:
      DCB.pC="";
      return true;
    case xidQmEst:
      DCB.D=dNAN;
      return true;
    case xidQm:
      if (NoProcessIOs() == 1)
        DCB.D=IOConduit(0)->QMass();
      else if (NoProcessIOs() >= 2)
        {
        int i= (!IO_Out(0) ? 0 : 1);
        DCB.D=IOConduit(i)->QMass();
        }
      else
        DCB.D=dNAN;
      return true;
    case xidQv:
      if (NoProcessIOs() == 1)
        DCB.D=IOConduit(0)->QVolume();
      else if (NoProcessIOs() >= 2)
        {
        int i= (!IO_Out(0) ? 0 : 1);
        DCB.D=IOConduit(i)->QVolume();
        }
      else
        DCB.D=dNAN;
      return true;
    case xidNQv:
      if (NoProcessIOs() == 1)
        DCB.D=IOConduit(0)->QNVolume();
      else if (NoProcessIOs() >= 2)
        {
        int i= (!IO_Out(0) ? 0 : 1);
        DCB.D=IOConduit(i)->QNVolume();
        }
      else
        DCB.D=dNAN;
      return true;
    case xidLevel:
      DCB.D=dNAN;//pContents->Level();
      return true;
    case xidMomentum:
      if (NoProcessIOs()>0)
        {
        DCB.D=0.0;
        for (int j=0; j<NIOFBs(0); j++)
          DCB.D=IOFB(0,j)->DPmB()+IOFB(0,j)->DPmQ();
        }
      else
        DCB.D=dNAN;
      return true;
    case xidNettBoost:
      if (NoProcessIOs()>0)
        {
        DCB.D=0.0;
        for (int j=0; j<NIOFBs(0); j++)
          DCB.D=IOFB(0,j)->DPb()+IOFB(0,j)->DPz();
        }
      else
        DCB.D=dNAN;
      return true;
    case xiddP:
      if (NoProcessIOs() >= 2)
        DCB.D=IOP_Flng(1)-IOP_Flng(0);
      else
        DCB.D=0.0;
      return true;
    case xidPIn:
      if (NoProcessIOs() >= 1)
        {
        if (DCB.rD)
          Set_IOP_Flng(0, *DCB.rD);
        DCB.D=IOP_Flng(0);
        }
      else
        DCB.D=AtmosPress();
      return true;
    case xidPOut:
      if (NoProcessIOs() >= 2)
        {
        if (DCB.rD)
          Set_IOP_Flng(1, *DCB.rD);
        DCB.D=IOP_Flng(1);
        }
      else
        DCB.D=AtmosPress();
      return true;
    case xidPInEst:
      if (NoProcessIOs() >= 1)
        {
        if (DCB.rD)
          Set_IOP_Est_Flng(0, *DCB.rD);
        DCB.D=IOP_Est_Flng(0);
        }
      else
        DCB.D=AtmosPress();
      return true;
    case xidPOutEst:
      if (NoProcessIOs() >= 2)
        {
        if (DCB.rD)
          Set_IOP_Est_Flng(1, *DCB.rD);
        DCB.D=IOP_Est_Flng(1);
        }
      else
        DCB.D=AtmosPress();
      return true;
    case xidPInRmt:
      DCB.D=(NoProcessIOs() >= 1) ? IOP_Term_Rmt(0) : AtmosPress();
      return true;
    case xidPOutRmt:
      DCB.D=(NoProcessIOs() >= 2) ? IOP_Term_Rmt(1) : AtmosPress();
      return true;
    case xidPMean:
      if (DCB.rD)
        Set_JoinP(0, *DCB.rD);
      DCB.D=JoinP(0);
      return true;
    case xidPEstMean:
      if (DCB.rD)
        Set_JoinP_Est(0, *DCB.rD);
      DCB.D=JoinP_Est(0);
      return true;
    case xidDPInRmt:
      DCB.D=(NoProcessIOs() >= 1) ? IOP_Flng(0)-IOP_Term_Rmt(0) : 0;
      return true;
    case xidDPOutRmt:
      DCB.D=(NoProcessIOs() >= 2) ? IOP_Term_Rmt(1) - IOP_Flng(1): 0;
      return true;
    case xidDPRmt:
      DCB.D=(NoProcessIOs() >= 2) ? IOP_Flng(1) : AtmosPress();
      DCB.D-=(NoProcessIOs() >= 1) ? IOP_Flng(0) : AtmosPress();
      return true;

    case xidHzIn:
      DCB.D=(NoProcessIOs() >= 1) ? IOConduit(0)->msHz(som_ALL, IOConduit(0)->Temp(), IOP_Flng(0)) : 0;
      return true;
    case xidHzOut:
      DCB.D=(NoProcessIOs() >= 2) ? IOConduit(1)->msHz(som_ALL, IOConduit(1)->Temp(), IOP_Flng(1)) : 0;
      return true;
    case xidHzInRmt:
      DCB.D=(NoProcessIOs() >= 1) ? IOConduit(0)->msHz(som_ALL, IOConduit(0)->Temp(), IOP_Term_Rmt(0)) : 0;
      return true;
    case xidHzOutRmt:
      DCB.D=(NoProcessIOs() >= 2) ? IOConduit(1)->msHz(som_ALL, IOConduit(1)->Temp(), IOP_Term_Rmt(1)) : 0;
      return true;
    case xidtotHzIn:
      DCB.D=(NoProcessIOs() >= 1) ? IOConduit(0)->totHz(som_ALL, IOConduit(0)->Temp(), IOP_Flng(0)) : 0;
      return true;
    case xidtotHzOut:
      DCB.D=(NoProcessIOs() >= 2) ? IOConduit(1)->totHz(som_ALL, IOConduit(1)->Temp(), IOP_Flng(1)) : 0;
      return true;
    case xidtotHzInRmt:
      DCB.D=(NoProcessIOs() >= 1) ? IOConduit(0)->totHz(som_ALL, IOConduit(0)->Temp(), IOP_Term_Rmt(0)) : 0;
      return true;
    case xidtotHzOutRmt:
      DCB.D=(NoProcessIOs() >= 2) ? IOConduit(1)->totHz(som_ALL, IOConduit(1)->Temp(), IOP_Term_Rmt(1)) : 0;
      return true;

    case xidSolFrac:
      if (NoProcessIOs() >= 1)
        {
        pSpConduit Q=IOConduit(0);
        DCB.D=(Q ? Q->QMass(som_Sol)/GTZ(Q->QMass(som_ALL)) : dNAN);
        }
      else
        DCB.D=dNAN;
      return true;
    case xidLiqFracIn:
    case xidLiqFrac:
      if (NoProcessIOs() >= 1)
        {
        pSpConduit Q=IOConduit(0);
        DCB.D=(Q ? Q->QMass(som_Liq)/GTZ(Q->QMass(som_ALL)) : dNAN);
        }
      else
        DCB.D=dNAN;
      return true;
    case xidLiqFracOut:
      if (NoProcessIOs() >= 2)
        {
        pSpConduit Q=IOConduit(1);
        DCB.D=(Q ? Q->QMass(som_Liq)/GTZ(Q->QMass(som_ALL)) : dNAN);
        }
      else
        DCB.D=dNAN;
      return true;
    case xidVapFracIn:
    case xidVapFrac:
      if (NoProcessIOs() >= 1)
        {
        pSpConduit Q=IOConduit(0);
        DCB.D=(Q ? Q->QMass(som_Gas)/GTZ(Q->QMass(som_ALL)) : dNAN);
        }
      else
        DCB.D=dNAN;
      return true;
    case xidVapFracOut:
      if (NoProcessIOs() >= 2)
        {
        pSpConduit Q=IOConduit(1);
        DCB.D=(Q ? Q->QMass(som_Gas)/GTZ(Q->QMass(som_ALL)) : dNAN);
        }
      else
        DCB.D=dNAN;
      return true;
    case xidMoleVapFrac:
      if (NoProcessIOs() >= 1)
        {
        pSpConduit Q=IOConduit(0);
        DCB.D=(Q ? Q->QMole(som_Gas)/GTZ(Q->QMole(som_ALL)) : dNAN);
        }
      else
        DCB.D=dNAN;
      return true;
    case xidTemp:
    case xidTempIn:
      if (NoProcessIOs() >= 1)
        {
        pSpConduit Q=IOConduit(0);
        DCB.D=(Q ? Q->Temp() : dNAN);
        }
      else
        DCB.D=dNAN;
      return true;
    case xidTempOut:
      if (NoProcessIOs() >= 2)
        {
        pSpConduit Q=IOConduit(1);
        DCB.D=(Q ? Q->Temp() : dNAN);
        }
      else
        DCB.D=dNAN;
      return true;

    case xidIO_QmTotalIn:
      {
      double Sig=0.0;
      for (int i=0; i<NoProcessIOs(); i++)
        if (IO_In(i))
          Sig+=IOConduit(i)->QMass();
      DCB.D=Sig;
      return true;
      }
    case xidIO_QmTotalOut:
      {
      double Sig=0.0;
      for (int i=0; i<NoProcessIOs(); i++)
        if (IO_Out(i))
          Sig-=IOConduit(i)->QMass();
      DCB.D=Sig;
      return true;
      }
    case xidIO_QmNett:
      {
      double Sig=0.0;
      for (int i=0; i<NoProcessIOs(); i++)
        Sig+=(IO_In(i) ? 1 : IO_Out(i) ? -1 : 0) * IOAreaSum(i)->FlowMass;
      DCB.D=Sig;
      return true;
      }
    case xidIO_QmBalance:
      {
      double Sig=0.0;
      for (int i=0; i<NoProcessIOs(); i++)
        Sig+=(IO_In(i) ? 1 : IO_Out(i) ? -1 : 0) * IOAreaSum(i)->FlowMass;
      for (int d=FirstAdtDIO(); d<NoDirectIOs(); d++)
        {
        CDirectFlwIO &D=*m_DirectIOs[d];
        Sig+=D.NdSignImplied*D.Sum.FlowMass;
        }
      DCB.D=Sig;
      return true;
      }
    case xidIO_QmBalanceRel:
      {
      double Sig=0.0;
      double SigPos=0.0;
      double SigNeg=0.0;
      for (int i=0; i<NoProcessIOs(); i++)
        {
        Sig+=(IO_In(i) ? 1 : IO_Out(i) ? -1 : 0) * IOAreaSum(i)->FlowMass;
        if (IO_In(i))
          SigPos+=IOAreaSum(i)->FlowMass;
        else if (IO_Out(i))
          SigNeg+=IOAreaSum(i)->FlowMass;
        }
      for (int d=FirstAdtDIO(); d<NoDirectIOs(); d++)
        {
        CDirectFlwIO &D=*m_DirectIOs[d];
        Sig+=D.NdSignImplied*D.Sum.FlowMass;
        if (D.NdSignImplied>0)
          SigPos+=D.Sum.FlowMass;
        else if (D.NdSignImplied<0)
          SigNeg+=D.Sum.FlowMass;
        }
      DCB.D=Sig/GTZ(Max(SigPos, SigNeg));
      return true;
      }
    case xidIO_QmNettTotal:
      {
      double Sig=0.0;
      for (int i=0; i<NoProcessIOs(); i++)
        {
        //dbgpln("%3i %3i %3i %3i %10.6f %10.6f %10.6f %10.6f %10.6f", i, IOId_Self(i), IO_In(i), IO_Out(i), IOQm_In(i), IOQm_Out(i), IOQmEst_In(i), IOQmEst_Out(i), IOAreaSum(i)->TotalMass);
        Sig+=(IO_In(i) ? 1 : IO_Out(i) ? -1 : 0) * IOAreaSum(i)->TotalMass;
        }
      DCB.D=Sig;
      return true;
      }
    case xidIO_QmBalanceTotal:
      {
      double Sig=0.0;
      for (int i=0; i<NoProcessIOs(); i++)
        Sig+=(IO_In(i) ? 1 : IO_Out(i) ? -1 : 0) * IOAreaSum(i)->TotalMass;
      for (int d=FirstAdtDIO(); d<NoDirectIOs(); d++)
        {
        CDirectFlwIO &D=*m_DirectIOs[d];
        Sig+=D.NdSignImplied*D.Sum.TotalMass;
        }
      DCB.D=Sig;
      return true;
      }
    case xidIO_QmBalanceTotalRel:
      {
      double Sig=0.0;
      double SigPos=0.0;
      double SigNeg=0.0;
      for (int i=0; i<NoFlwIOs(); i++)
        {
        Sig+=(IO_In(i) ? 1 : IO_Out(i) ? -1 : 0) * IOAreaSum(i)->TotalMass;
        if (IO_In(i))
          SigPos+=IOAreaSum(i)->TotalMass;
        else if (IO_Out(i))
          SigNeg+=IOAreaSum(i)->TotalMass;
        }
      for (int d=FirstAdtDIO(); d<NoDirectIOs(); d++)
        {
        CDirectFlwIO &D=*m_DirectIOs[d];
        Sig+=D.NdSignImplied*D.Sum.TotalMass;
        if (D.NdSignImplied>0)
          SigPos+=D.Sum.TotalMass;
        else if (D.NdSignImplied<0)
          SigNeg+=D.Sum.TotalMass;
        }
      DCB.D=Sig/GTZ(Max(SigPos, SigNeg));
      return true;
      }

    case xidIO_HzNett:
      {
      double Sig=0.0;
      for (int i=0; i<NoFlwIOs(); i++)
        Sig+=(IO_In(i) ? 1 : IO_Out(i) ? -1 : 0) * IOAreaSum(i)->FlowHz;
      DCB.D=Sig;
      return true;
      }
    case xidIO_HzBalance:
      {
      double Sig=0.0;
      for (int i=0; i<NoFlwIOs(); i++)
        Sig+=(IO_In(i) ? 1 : IO_Out(i) ? -1 : 0) * IOAreaSum(i)->FlowHz;
      for (int d=FirstAdtDIO(); d<NoDirectIOs(); d++)
        {
        CDirectFlwIO &D=*m_DirectIOs[d];
        Sig+=D.NdSignImplied*D.Sum.FlowHz;
        }
      DCB.D=Sig;
      return true;
      }
    case xidIO_HzBalanceRel:
      {
      double Sig=0.0;
      double SigPos=0.0;
      double SigNeg=0.0;
      for (int i=0; i<NoFlwIOs(); i++)
        {
        Sig+=(IO_In(i) ? 1 : IO_Out(i) ? -1 : 0) * IOAreaSum(i)->FlowHz;
        if (IO_In(i))
          SigPos+=IOAreaSum(i)->FlowHz;
        else if (IO_Out(i))
          SigNeg+=IOAreaSum(i)->FlowHz;
        }
      for (int d=FirstAdtDIO(); d<NoDirectIOs(); d++)
        {
        CDirectFlwIO &D=*m_DirectIOs[d];
        Sig+=D.NdSignImplied*D.Sum.FlowHz;
        if (D.NdSignImplied>0)
          SigPos+=D.Sum.FlowHz;
        else if (D.NdSignImplied<0)
          SigNeg+=D.Sum.FlowHz;
        }
      DCB.D=Sig/GTZ(Max(SigPos,SigNeg));
      return true;
      }
    case xidIO_HzNettTotal:
      {
      double Sig=0.0;
      for (int i=0; i<NoFlwIOs(); i++)
        Sig+=(IO_In(i) ? 1 : IO_Out(i) ? -1 : 0) * IOAreaSum(i)->TotalHz;
      DCB.D=Sig;
      return true;
      }
    case xidIO_HzBalanceTotal:
      {
      double Sig=0.0;
      for (int i=0; i<NoFlwIOs(); i++)
        Sig+=(IO_In(i) ? 1 : IO_Out(i) ? -1 : 0) * IOAreaSum(i)->TotalHz;
      for (int d=FirstAdtDIO(); d<NoDirectIOs(); d++)
        {
        CDirectFlwIO &D=*m_DirectIOs[d];
        Sig+=D.NdSignImplied*D.Sum.TotalHz;
        }
      DCB.D=Sig;
      return true;
      }
    case xidIO_HzBalanceTotalRel:
      {
      double Sig=0.0;
      double SigPos=0.0;
      double SigNeg=0.0;
      for (int i=0; i<NoFlwIOs(); i++)
        {
        Sig+=(IO_In(i) ? 1 : IO_Out(i) ? -1 : 0) * IOAreaSum(i)->TotalHz;
        if (IO_In(i))
          SigPos+=IOAreaSum(i)->TotalHz;
        else if (IO_Out(i))
          SigNeg+=IOAreaSum(i)->TotalHz;
        }
      for (int d=FirstAdtDIO(); d<NoDirectIOs(); d++)
        {
        CDirectFlwIO &D=*m_DirectIOs[d];
        Sig+=D.NdSignImplied*D.Sum.TotalHz;
        if (D.NdSignImplied>0)
          SigPos+=D.Sum.TotalHz;
        else if (D.NdSignImplied<0)
          SigNeg+=D.Sum.TotalHz;
        }
      DCB.D=Sig/GTZ(Max(SigPos, SigNeg));
      return true;
      }


    case xidIO_LnkCount:
      DCB.L=NoProcessIOs();
      return true;
    case xidIO_DirCount:
      DCB.L=NoDirDIOs();
      return true;
    case xidIO_AdtCount:
      DCB.L=NoAdtDIOs();
      return true;


#if WITHIOSENERGY
    case xidIO_QvTotalIn:
      {
      double Sig=0.0;
      for (int i=0; i<NoFlwIOs(); i++)
        if (IO_In(i))
          Sig+=IOConduit(i)->QVolume(som_ALL);
      DCB.D=Sig;
      return true;
      }
    case xidIO_QvTotalOut:
      {
      double Sig=0.0;
      for (int i=0; i<NoFlwIOs(); i++)
        if (IO_Out(i))
          Sig+=IOConduit(i)->QVolume(som_ALL);
      DCB.D=Sig;
      return true;
      }
    case xidIO_HsTotalIn:
      {
      double Sig=0.0;
      for (int i=0; i<NoFlwIOs(); i++)
        if (IO_In(i))
          Sig+=IOConduit(i)->totHs();
      DCB.D=Sig;
      return true;
      }
    case xidIO_HsTotalOut:
      {
      double Sig=0.0;
      for (int i=0; i<NoFlwIOs(); i++)
        if (IO_Out(i))
          Sig+=IOConduit(i)->totHs();
      DCB.D=Sig;
      return true;
      }
    case xidIO_HsSigma:
      {
      double Sig=0.0;
      for (int i=0; i<NoFlwIOs(); i++)
        if (IO_In(i))
          Sig+=IOConduit(i)->totHs();
        else
          Sig-=IOConduit(i)->totHs();
      DCB.D=Sig;
      return true;
      }
    case xidIO_HzTotalIn:
      {
      double Sig=0.0;
      for (int i=0; i<NoFlwIOs(); i++)
        if (IO_In(i))
          Sig+=IOConduit(i)->totHz();
      DCB.D=Sig;
      return true;
      }
    case xidIO_HzTotalOut:
      {
      double Sig=0.0;
      for (int i=0; i<NoFlwIOs(); i++)
        if (IO_Out(i))
          Sig+=IOConduit(i)->totHz();
      DCB.D=Sig;
      return true;
      }
    case xidIO_HzSigma:
      {
      double Sig=0.0;
      for (int i=0; i<NoFlwIOs(); i++)
        if (IO_In(i))
          Sig+=IOConduit(i)->totHz();
        else
          Sig-=IOConduit(i)->totHz();
      DCB.D=Sig;
      return true;
      }
    case xidIO_HfTotalIn:
      {
      double Sig=0.0;
      for (int i=0; i<NoFlwIOs(); i++)
        if (IO_In(i))
          Sig+=IOConduit(i)->totHf();
      DCB.D=Sig;
      return true;
      }
    case xidIO_HfTotalOut:
      {
      double Sig=0.0;
      for (int i=0; i<NoFlwIOs(); i++)
        if (IO_Out(i))
          Sig+=IOConduit(i)->totHf();
      DCB.D=Sig;
      return true;
      }
    case xidIO_HfSigma:
      {
      double Sig=0.0;
      for (int i=0; i<NoFlwIOs(); i++)
        if (IO_In(i))
          Sig+=IOConduit(i)->totHf();
        else
          Sig-=IOConduit(i)->totHf();
      DCB.D=Sig;
      return true;
      }
#endif
    //case xidIO_MkUpAvail0:
    //case xidIO_MkUpAvail1:
    //  {
    //  int ioId=DCB.lHandle-xidIO_MkUpAvail0;
    //  if (DCB.rD && NoFlwIOs()>1)
    //    {
    //    if (Valid(*DCB.rD))
    //      IOFlange(ioId)->SetMakeUpAvail(*DCB.rD);
    //    else
    //      IOFlange(ioId)->ClrMakeUpAvail();
    //    }
    //  if (NoFlwIOs()>1 && IOFlange(ioId)->IsMakeUpAvail())
    //    DCB.D=IOFlange(ioId)->MakeUpAvail();
    //  else
    //    DCB.D=dNAN;
    //  return true;
    //  }
    //case xidIO_MkUpReqd0:
    //case xidIO_MkUpReqd1:
    //  {
    //  int ioId=DCB.lHandle-xidIO_MkUpReqd0;
    //  if (DCB.rD && NoFlwIOs()>1)
    //    {
    //    if (Valid(*DCB.rD))
    //      IOFlange(ioId)->SetMakeUpReqd(*DCB.rD);
    //    else
    //      IOFlange(ioId)->ClrMakeUpReqd();
    //    }
    //  if (NoFlwIOs()>1 && IOFlange(ioId)->IsMakeUpReqd())
    //    DCB.D=IOFlange(ioId)->MakeUpReqd();
    //  else
    //    DCB.D=dNAN;
    //  return true;
    //  }
    case xidIO_QmAvail:
      {
      DCB.D=(NoFlwIOs()>1) ? IOQmAvail_Self(IO_In(0) ? 0 : 1) : 0.0;
      return true;
      }
    case xidIO_QmSpace:
      {
      DCB.D=(NoFlwIOs()>1) ? IOQmSpace_Self(IO_In(0) ? 0 : 1) : 0.0;
      return true;
      }
//    case xidChgErr:
//      DCB.D=ChgStats[DCB.dwUserInfo].RelErr();
//      return 1;
//    case xidChgChg:
//      DCB.D=ChgStats[DCB.dwUserInfo].RelChg();
//      return 1;
//    case xidChgOsc:
//      DCB.D=ChgStats[DCB.dwUserInfo].RelOsc();
//      return 1;
//    case xidIO_GasCanEscape:
//      {
//      DCB.B=(NoFlwIOs()>1) ? IOGasCanEscape(IO_In(0) ? 0 : 1) : 0;
//      return true;
//      }
//    case xidIO_FullLiqFlow:
//      {
//      DCB.D=(NoFlwIOs()>1) ? IOFullLiqFlow(IO_In(0) ? 0 : 1) : 0.0;
//      return true;
//      }
//    case xidIO_LiqAtBase:
//      {
//      DCB.B=(NoFlwIOs()>1) ? IOLiqAtBase(IO_In(0) ? 0 : 1) : 0;
//      return true;
//      }

    case xidIO_JoinMassFlwIn        : DCB.D=GetBalanceValue(eBV_JoinMassFlwIn,        DCB.dwUserInfo); return true;
    case xidIO_JoinMassGain         : DCB.D=GetBalanceValue(eBV_JoinMassGain,         DCB.dwUserInfo); return true;
    case xidIO_JoinMassFlwOut       : DCB.D=GetBalanceValue(eBV_JoinMassFlwOut,       DCB.dwUserInfo); return true;
    case xidIO_JoinFeedCpT          : DCB.D=GetBalanceValue(eBV_JoinFeedCpT,          DCB.dwUserInfo); return true;
    case xidIO_JoinProdCpT          : DCB.D=GetBalanceValue(eBV_JoinProdCpT,          DCB.dwUserInfo); return true;
    case xidIO_JoinHfIn             : DCB.D=GetBalanceValue(eBV_JoinHfIn,             DCB.dwUserInfo); return true;
    case xidIO_JoinHfGain           : DCB.D=GetBalanceValue(eBV_JoinHfGain,           DCB.dwUserInfo); return true;
    case xidIO_JoinHfOut            : DCB.D=GetBalanceValue(eBV_JoinHfOut,            DCB.dwUserInfo); return true;
    case xidIO_JoinHfChg            : DCB.D=GetBalanceValue(eBV_JoinHfChg,            DCB.dwUserInfo); return true;
    case xidIO_JoinHsIn             : DCB.D=GetBalanceValue(eBV_JoinHsIn,             DCB.dwUserInfo); return true;
    case xidIO_JoinHsGain           : DCB.D=GetBalanceValue(eBV_JoinHsGain,           DCB.dwUserInfo); return true;
    case xidIO_JoinHsOut            : DCB.D=GetBalanceValue(eBV_JoinHsOut,            DCB.dwUserInfo); return true;
    case xidIO_JoinHsChg            : DCB.D=GetBalanceValue(eBV_JoinHsChg,            DCB.dwUserInfo); return true;
    case xidIO_JoinPowerIn          : DCB.D=GetBalanceValue(eBV_JoinPowerIn,          DCB.dwUserInfo); return true;
    case xidIO_RCTMassGain          : DCB.D=GetBalanceValue(eBV_RCTMassGain,          DCB.dwUserInfo); return true;
    case xidIO_RCTHfGain            : DCB.D=GetBalanceValue(eBV_RCTHfGain,            DCB.dwUserInfo); return true;
    case xidIO_RCTHsGain            : DCB.D=GetBalanceValue(eBV_RCTHsGain,            DCB.dwUserInfo); return true;
    case xidIO_RCTPowerIn           : DCB.D=GetBalanceValue(eBV_RCTPowerIn,           DCB.dwUserInfo); return true;
    case xidIO_RCTHOR_Std0          : DCB.D=GetBalanceValue(eBV_RCTHOR_Std0,          DCB.dwUserInfo); return true;
    case xidIO_RCTHOR_Mdl0          : DCB.D=GetBalanceValue(eBV_RCTHOR_Mdl0,          DCB.dwUserInfo); return true;
    case xidIO_RCTHOR_Used0         : DCB.D=GetBalanceValue(eBV_RCTHOR_Used0,         DCB.dwUserInfo); return true;
    case xidIO_RCTHOR_Diff0         : DCB.D=GetBalanceValue(eBV_RCTHOR_Diff0,         DCB.dwUserInfo); return true;
    //case xidIO_RCTHOR_EstTChg       : DCB.D=GetBalanceValue(eBV_RCTHOR_EstTChg,       DCB.dwUserInfo); return true;
    case xidIO_VLEHsGain            : DCB.D=GetBalanceValue(eBV_VLEHsGain,            DCB.dwUserInfo); return true;
    case xidIO_EHXPowerIn           : DCB.D=GetBalanceValue(eBV_EHXPowerIn,           DCB.dwUserInfo); return true;
    //case xidIO_EHXEstTChg           : DCB.D=GetBalanceValue(eBV_EHXEstTChg,           DCB.dwUserInfo); return true;

    case xidIO_CopyClsCompact          :
    case xidIO_CopyClsFull             :
      {
      if (DCB.ForView() && DCB.rB)
        {
        char* pTag = new char[strlen(FullObjTag())+1];
        strcpy(pTag, FullObjTag());
        AfxGetMainWnd()->PostMessage(WMU_NODEAUDIT, SUB_NA_CLS_SUMMARY | ((DCB.lHandle==xidIO_CopyClsCompact) ? SUB_NA_CLS_COMPACT : 0), (LPARAM)pTag);
        }
      DCB.B=0;
      return 1;
      }
   
    case xidIO_Count:
      {
      DCB.S=(short)NoProcessIOs();
      return 1;
      }
    case xidIO_InCnt:
      {
      DCB.S=0;
      for (int i=0; i<NoProcessIOs(); i++)
        if (LnkFlowDefinedIn(i))
          DCB.S++;
      return 1;
      }
    case xidIO_OutCnt:
      {
      DCB.S=0;
      for (int i=0; i<NoProcessIOs(); i++)
        if (LnkFlowDefinedOut(i))
          DCB.S++;
      return 1;
      }

    default :
      if (DCB.lHandle>=xidIO_IoQm0 && DCB.lHandle<xidIO_IoQm0+NoFlwIOs())
        {
        int i=DCB.lHandle-xidIO_IoQm0;
        DCB.D=IOConduit(i)->QMass()*Sign(IOQm_In(i));
        return true;
        }
      else if (DCB.lHandle>=xidIO_DrQm0 && DCB.lHandle<xidIO_DrQm0+m_DirectIOs.GetCount())
        {
        int i=DCB.lHandle-xidIO_DrQm0;
        if (i<NoDirectIOs())
          {
          CDirectFlwIO &D=*m_DirectIOs[i];
          DCB.D=D.NdSignImplied*D.Sum.FlowMass;
          }
        else
          DCB.D=dNAN;
        return true;
        }
      else if (DCB.lHandle>=xidIO_IoHz0 && DCB.lHandle<xidIO_IoHz0+NoFlwIOs())
        {
        int i=DCB.lHandle-xidIO_IoHz0;
        DCB.D=IOConduit(i)->totHz()*Sign(IOQm_In(i));
        return true;
        }
      else if (DCB.lHandle>=xidIO_DrHz0 && DCB.lHandle<xidIO_DrHz0+NoDirectIOs())
        {
        int i=DCB.lHandle-xidIO_DrHz0;
        if (i<NoDirectIOs())
          {
          CDirectFlwIO &D=*m_DirectIOs[i];
          DCB.D=D.NdSignImplied*D.Sum.FlowHz;
          }
        else
          DCB.D=dNAN;
        return true;
        }
      else if (DCB.lHandle>=xidIO_IoQmTotal0 && DCB.lHandle<xidIO_IoQmTotal0+NoFlwIOs())
        {
        int i=DCB.lHandle-xidIO_IoQmTotal0;
        DCB.D=IOFlange(i)->Sum.TotalMass;
        return true;
        }
      else if (DCB.lHandle>=xidIO_DrQmTotal0 && DCB.lHandle<xidIO_DrQmTotal0+NoDirectIOs())
        {
        int i=DCB.lHandle-xidIO_DrQmTotal0;
        if (i<NoDirectIOs())
          {
          CDirectFlwIO &D=*m_DirectIOs[i];
          DCB.D=D.NdSignImplied*D.Sum.TotalMass;
          }
        else
          DCB.D=dNAN;
        return true;
        }
      else if (DCB.lHandle>=xidIO_IoHzTotal0 && DCB.lHandle<xidIO_IoHzTotal0+NoFlwIOs())
        {
        int i=DCB.lHandle-xidIO_IoHzTotal0;
        DCB.D=IOFlange(i)->Sum.TotalHz;
        return true;
        }
      else if (DCB.lHandle>=xidIO_DrHzTotal0 && DCB.lHandle<xidIO_DrHzTotal0+m_DirectIOs.GetCount())
        {
        int i=DCB.lHandle-xidIO_DrHzTotal0;
        if (i<NoDirectIOs())
          {
          CDirectFlwIO &D=*m_DirectIOs[i];
          DCB.D=D.NdSignImplied*D.Sum.TotalHz;
          }
        else
          DCB.D=dNAN;
        return true;
        }
      else if (DCB.lHandle>=xidIO_StreamTag0 && DCB.lHandle<xidIO_StreamTag0+NoFlwIOs())
        {
        DCB.pC=Nd_Rmt(char(DCB.lHandle-xidIO_StreamTag0))->FullObjTag();
        return true;
        }
      else if (DCB.lHandle>=xidIO_Hgt0 && DCB.lHandle<xidIO_Hgt0+NoFlwIOs())
        {
        DCB.D= IODatum_Flng(char(DCB.lHandle-xidIO_Hgt0));
        return true;
        }
      else if (DCB.lHandle>=xidIO_HFrc0 && DCB.lHandle<xidIO_HFrc0+NoFlwIOs())
        {
        int i=DCB.lHandle-xidIO_HFrc0;
        if (DCB.rD)
          SetIORelFracHgt_Flng(i, *DCB.rD);
        DCB.D= IORelFracHgt_Flng(i);
        return true;
        }
      else if (DCB.lHandle>=xidIO_Aperture0 && DCB.lHandle<xidIO_Aperture0+NoFlwIOs())
        {
        int i=DCB.lHandle-xidIO_Aperture0;
        if (DCB.rD)
          SetIOAperture(i, *DCB.rD);
        DCB.D= IOAperture(i, !DCB.ForFileSnpScn());
        return true;
        }
      else if (DCB.lHandle>=xidIO_ApertureAng0 && DCB.lHandle<xidIO_ApertureAng0+NoFlwIOs())
        {
        int i=DCB.lHandle-xidIO_ApertureAng0;
        if (DCB.rD)
          SetIOApertureAngle(i, *DCB.rD);
        DCB.D= IOApertureAngle(i);
        return true;
        }
      else if (DCB.lHandle>=xidIO_EntrainRateA0 && DCB.lHandle<xidIO_EntrainRateA0+NoFlwIOs())
        {
        int i=DCB.lHandle-xidIO_EntrainRateA0;
        if (DCB.rD)
          SetIOEntrainRateA(i, *DCB.rD);
        DCB.D= IOEntrainRateA(i);
        return true;
        }
      else if (DCB.lHandle>=xidIO_EntrainRateB0 && DCB.lHandle<xidIO_EntrainRateB0+NoFlwIOs())
        {
        int i=DCB.lHandle-xidIO_EntrainRateB0;
        if (DCB.rD)
          SetIOEntrainRateB(i, *DCB.rD);
        DCB.D= IOEntrainRateB(i);
        return true;
        }
      else if (DCB.lHandle>=xidIO_ExitRule0 && DCB.lHandle<xidIO_ExitRule0+NoFlwIOs())
        {
        int i=DCB.lHandle-xidIO_ExitRule0;
        if (DCB.rB)
          SetIOExitRule(i, *DCB.rB);
        DCB.B= IOExitRule(i);
        return true;
        }
      else if (DCB.lHandle>=xidIO_EstV2L0 && DCB.lHandle<xidIO_EstV2L0+NoFlwIOs())
        {
        int i=DCB.lHandle-xidIO_EstV2L0;
        if (DCB.rD)
          SetIOEstV2LOnEntry(i, *DCB.rD);
        DCB.D= IOEstV2LOnEntry(i);
        return true;
        }
      else if (DCB.lHandle>=xidIO_EstL2V0 && DCB.lHandle<xidIO_EstL2V0+NoFlwIOs())
        {
        int i=DCB.lHandle-xidIO_EstL2V0;
        if (DCB.rD)
          SetIOEstL2VOnEntry(i, *DCB.rD);
        DCB.D=IOEstL2VOnEntry(i);
        return true;
        }

    }
  return False;
  }

//--------------------------------------------------------------------------

void MdlNode::ClosureInfo()
  {
  if (m_Closure.DoFlows())
    {
    CClosureInfo &CI=m_Closure[0];
    if (EHX.Enabled())
      CI.m_EHXPowerIn+=EHX.HeatFlow();
    }
  };

//==========================================================================

long MdlNode::CheckDataRanges(bool CheckIO)
  {
  long N=FlwNode::CheckDataRanges(CheckIO);
  if (CheckIO)
    {
    for (int i=0; i<NoFlwIOs(); i++)
      N+=IOConduit(i)->CheckDataRanges();
    }
  return N;
  };

//==========================================================================

inline double DeltaZ(MdlNode * pNd, int IONo, CSetDatumsData * Data)
  {
  IDMask IOMsk=pNd->IOIdMask_Self(IONo);
  static CSetDatumsData DataIfNull[]=
    {
      {First64IOIds, NULL},
      {0}
    };
  CSetDatumsData * pData=Data ? Data:DataIfNull;
  while (pData->m_IOIdMask && ((pData->m_IOIdMask&IOMsk)==0))
    pData++;
  if (pData->m_IOIdMask)
    {
    double D=pData->m_dShpBaseFromDatum;
    if (pData->m_pCn)
      {
      SpContainer &Cn=*pData->m_pCn;
      pNd->SetIORelFracHgt_Flng(IONo, Max(Cn.LowestIOPt(), pNd->IORelFracHgt_Flng(IONo)));
      D += pNd->IOAbsFracHgt_Flng(IONo, *(pData->m_pCn)) * Cn.ShapeHeight() - Cn.ShapeDatumPt();
      }
    else
      {
      pNd->SetIORelFracHgt_Flng(IONo, Max(pData->m_dLowestIOPt, pNd->IORelFracHgt_Flng(IONo)));
      D += pNd->IOAbsFracHgt_Flng(IONo, pData->m_dShpHeight, pData->m_dShpDatumPt) * pData->m_dShpHeight - pData->m_dShpDatumPt;
      }
    return D;
    }
  return 0;
  }

//==========================================================================
#if dbgModels
class CKwikIndent
  {
  public:
    CKwikIndent()
      {
      if (dbgSetZsValue() || dbgSetZsTrace())
        dbgindent(2); 
      }
    ~CKwikIndent()
      {
      if (dbgSetZsValue() || dbgSetZsTrace())
        dbgindent(-2); 
      }
  };

#define KWIKINDENT CKwikIndent KwikIndent;
#else
#define KWIKINDENT
#endif

LPCTSTR Z_StateStr(Z_States State)
  {
  switch (State)
    {
    case Z_Unknown:           return "Z_Unknown      ";
    case Z_Guessed:           return "Z_Guessed      ";
    case Z_Known:             return "Z_Known        ";
    case Z_OverSpecified:     return "Z_OverSpecified";    
    }
  return "???";
  }

LPCTSTR IndentMatch()
  {
  static CString S;
  S.Format("%*.*s", 40-dbgindentcnt(), 40-dbgindentcnt(), "");
  return S;
  }

Z_States MdlNode::SetDatums_Node(int Pass, CSetDatumsData * Data, int IOIn, double Zi, Z_States Z_State_Src /*,double &Z_Ret, Z_States &Z_State_Ret*/)
  {
  //static CSetDatumsData DataIfNull[1]={{-1,0,0}};
  //CSetDatumsData * pData=Data ? Data:DataIfNull;
  //while (pData->m_IOId>=0 && pData->m_IOId!=IOId_Self(IOIn))
  //  pData++;

  KWIKINDENT

  if (!Done && !Busy)
    {
    switch (Pass)
      {
      case 0:
        {
        if (Valid(m_dDatumRqd))
          {
          Z_State=Z_Known;
          m_dDatum = m_dDatumRqd;
          }
        else
          {
          Z_State=Z_Unknown;
          m_dDatum = 0.0;
          }
        #if dbgModels
        if (dbgSetZsValue())
          dbgpln("-       %s    %-15s      %8.2f %s ", IndentMatch(), Z_StateStr(Z_State), m_dDatum, FullObjTag());
        #endif
        for (int i = 0; (i < NoProcessIOs()); i++)
          {
          IOFlange(i)->m_dDatum = m_dDatum + DeltaZ(this, i, Data);
          #if dbgModels
          if (dbgSetZsValue())
            dbgpln(">> FA   %s    %-15s      %8.2f %s > %s", IndentMatch(), "", IOFlange(i)->m_dDatum, FullObjTag(), Nd_Rmt(i)->FullObjTag());
          #endif
          }
        return Z_State;
        }
      case 1:
        {
        switch (Z_State)
          {
          case Z_Unknown :
            if (Z_State_Src>=Z_Known)
              {
              IOFlange(IOIn)->m_dDatum = Zi;
              m_dDatum= Zi - DeltaZ(this, IOIn, Data);
              Z_State=Z_State_Src;
              }
            else
              return Z_State;
            break;
          case Z_Guessed:
            ASSERT(0);
            break;
          case Z_Known:
            if (Z_State_Src>=Z_Known)
              {
              FlwNode::SetCI(12);
              return Z_State;
              }
            break;
          case Z_OverSpecified:
            ASSERT(0);
            break;
          }
        break;
        }
      case 2:
        {
        switch (Z_State)
          {
          case Z_Unknown :
            if (Z_State_Src>=Z_Guessed)
              {
              IOFlange(IOIn)->m_dDatum = Zi;
              m_dDatum = Zi - DeltaZ(this, IOIn, Data);
              Z_State=Z_State_Src;
              }
            else
              {
              m_dDatum=0.0;
              Z_State=Z_Guessed;
              }
            break;
          case Z_Guessed:
            if (Z_State_Src>=Z_Known)
              {
              IOFlange(IOIn)->m_dDatum = Zi;
              m_dDatum = Zi - DeltaZ(this, IOIn, Data);
              Z_State=Z_State_Src;
              }
            break;
          case Z_Known:
            break;
          case Z_OverSpecified:
            dbgpln("SET Datum's : SHOULD NEVER GET HERE !");
            break;
          }
        break;
        }
      }

    Busy = 1;

    #if dbgModels
    if (dbgSetZsTrace())
       dbgpln(">> Nd   %s    %-15s      %8.2f %s", IndentMatch(), Z_StateStr(Z_State_Src), Zi, FullObjTag());
    #endif

    CFlange *pC;
    for (int i = 0; (i < NoProcessIOs()); i++)
      {
      pC = IOFlange(i);
      if (i != IOIn)
        {
        pC->m_dDatum = m_dDatum + DeltaZ(this, i, Data);
        Nd_Rmt(i)->SetDatums(Pass, IOIONo_Rmt(i), pC->m_dDatum, Z_State);
        #if dbgModels
        if (dbgSetZsValue())
          dbgpln(">> FB   %s    %-15s      %8.2f %s > %s", IndentMatch(), "", pC->m_dDatum, FullObjTag(), Nd_Rmt(i)->FullObjTag());
        #endif
        }
      }

    Busy = 0;
    #if dbgModels
    if (dbgSetZsTrace())
      dbgpln("<< Nd ");
    #endif
    }
  return Z_State;
  };

//--------------------------------------------------------------------------

Z_States MdlNode::SetDatums_Link(int Pass, double dZ_Rqd, int IOIn, double Zi, Z_States Z_State_Src /*double &Z_Ret, Z_States &Z_State_Ret*/)
  {
  KWIKINDENT

  if (!Done && !Busy && (NoProcessIOs() == 2))
    {
    int IOOut = (IOIn+1)%2;
    double Zo;
    switch (Pass)
      {
      case 0:
        return Z_Unknown;
      case 1:
        if (Valid(dZ_Rqd))
          Zo = Zi + dZ_Rqd * Direction(IOIn, IOOut);
        else
          return Z_Unknown;
        break;
      case 2:
        Zo = Zi;
        break;
      }

    #if dbgModels
    if (dbgSetZsTrace())
      {
      CString X;
      X.Format(Valid(dZ_Rqd)?"%+4.1f":"   *", dZ_Rqd);
      dbgpln("-> Link %s    %-15s %s %8.2f %s", IndentMatch(), Z_StateStr(Z_State_Src), X, Zi, FullObjTag());
      }
    #endif

    Busy = 1;
    Z_States Z_State_Ret=Nd_Rmt(IOOut) ? Nd_Rmt(IOOut)->SetDatums(Pass, IOIONo_Rmt(IOOut), Zo, Z_State_Src) : Z_Unknown;
    Done = 1;
    Busy = 0;

    #if dbgModels
    if (dbgSetZsTrace())
      dbgpln("<- Link %s    %-15s               %s", IndentMatch(), Z_StateStr(Z_State_Ret),FullObjTag());
    #endif

    return Z_State_Src;
    }
  return Z_Unknown;
  };

//--------------------------------------------------------------------------

Z_States MdlNode::SetDatums(int Pass, int IOIn, double Zi, Z_States Z_State_Src)
  {
  return SetDatums_Node(Pass, NULL, IOIn, Zi, Z_State_Src);
  };

//--------------------------------------------------------------------------

void MdlNode::SortSurgeIO(SortSurgeIOData * Data)
  {
  #if dbgModels
  flag dbg=(flag)dbgSortIOs();
  #endif


  for ( ;Data->m_IOIdMask; Data++)
    {
    double  FHgt[MaxIOList];

    SortSurgeIOData &D=*Data;
    CHgtOrdInfo     &HO=*D.m_pOrd;
    HO.m_dwIOIdMask = D.m_IOIdMask;

    // Connect Container to Hgt Order
    HO.m_pCn=D.m_pCn;

    if (D.m_pCn==NULL)
      continue;

    SpContainer     &Cn=*D.m_pCn;

    #if dbgModels
    if (dbg)
      dbgpln("SortSurgeIOs --------------- %s", Cn.FullObjTag());
    #endif

    for (int i = 0; i < NoFlwIOs(); i++)
      {
      if (IOIdMask_Self(i)&D.m_IOIdMask)
        {
        double App=0;
        if (!SolveInlineMethod())
          {
          App=IOFBJoin(i)->Diam()/Max(0.01, Cn.ShapeHeight());
          if (OrigPrjFileVerNo()<72 && !SolveInlineMethod())
            {
            App=Max(0.0, 0.01+App*sin(IOApertureAngle(i)));
            SetIOAperture(i, Range(0.02, App, 0.5));
            }
          else
            {
            App=Max(0.0, App*sin(IOApertureAngle(i)));
            SetIOAperture(i, Range(0.0, App, 0.5));
            }
          }
        FHgt[i]=IOAbsFracHgt_Term(i, Cn);
        // Connect Hgt Order to CIORec
        IOSurgeInfo(i)->m_pHgtInfo=D.m_pOrd;
        }
      }

    long    IHgt[MaxIOList];
    ASSERT(NoFlwIOs()+2<MaxIOList);
    //memset(IHgt, -1, NoFlwIOs()+2); kga 14/10/02 : WRONG memset for long doesn't work!!!
    for (i=0; i<NoFlwIOs()+2; i++)
      IHgt[i]=-1;
    IHgt[NoFlwIOs()]=-2; // As A Marker - temp
    int Oi=0;
    int nOvr=0;
    int nGas=0;

    byte DoSort;
    do
      {
      DoSort=0;
      Oi=0;
      for (i = 0; i < NoFlwIOs() && !DoSort; i++)
        if ((IOIdMask_Self(i) & D.m_IOIdMask)!=0)
          if (/*IO_Out(i) &&*/ (FHgt[i]<=1.01))
            {
            IOAreaRec * pA=IODesc_Self(i);
            if ((IsSpillIOId(pA->m_Id) || IsVentIOId(pA->m_Id)) && !Cn.Closed())
              {
              if (IsSpillIOId(pA->m_Id))
                HO.Ovr.SetAtGrow(nOvr++, i);
              else // IOGasVent
                HO.Gas.SetAtGrow(nGas++, i);
              }
            else
              {
              if (IOHgtOrd(i)>=0 && IOHgtOrd(i)<NoFlwIOs())
                {
                IHgt[IOHgtOrd(i)]=i;
                ++Oi;
                }
              else
                DoSort=1;
              }
            }

      for (i = 0; i < Oi && !DoSort; i++)
        if (IHgt[i]<0)
          DoSort=1;
        else if (i>0 && (FHgt[IHgt[i-1]]> FHgt[IHgt[i]]))
          DoSort=1;

      if (DoSort)
        {
        Oi=0;
        for (i = 0; i < NoFlwIOs(); i++)
          if ((IOIdMask_Self(i) & D.m_IOIdMask)!=0)
            if (/*IO_Out(i) &&*/ (FHgt[i]<=1.01))
              IHgt[Oi++]=i;
            else
              IOHgtOrd(i)=-1;

        for (i = 1; i < Oi; i++)
          {
          short j=i;
          while ((j > 0) && (FHgt[IHgt[j-1]] > FHgt[IHgt[j]]))
            {
            Exchange(IHgt[j-1], IHgt[j]);
            --j;
            }
          }
        for (i = 0; i < Oi; i++)
          {
          IOHgtOrd((byte)IHgt[i])=i;
          SetIOSameHgt((byte)IHgt[i], (i>0) && ((FHgt[IHgt[i]] - FHgt[IHgt[i-1]]) < 1.0e-3));
          //#if dbgModels
          //if (dbg)
          //  dbgpln(" %2i %2i %2i %6.2f ",i,IHgt[i],IOSameHgt(IHgt[i]), FHgt[IHgt[i]]);
          //#endif
          }
        }
      }
    while (DoSort);

    HO.Ord.SetSize(Oi);
    HO.Seq.SetSize(Oi);
    int HSeq=0;
    for (i = 0; i < Oi; i++)
      {
      HO.Ord[i]=IHgt[i];
      if (i>0 && ((FHgt[IHgt[i]] - FHgt[IHgt[i-1]]) >= 1.0e-3))
        HSeq++;
      HO.Seq[i]=HSeq;
      }

    #if dbgModels
    if (dbg)
      {
      dbgpln("Ord:   i Inx Seq  SH  Height Apertur");
      for (i = 0; i < Oi; i++)
        dbgpln("Ord: %3i %3i %3i %3i %6.2f%% %6.2f%% %s",
          i,HO.Ord[i], HO.Seq[i], IOSameHgt((byte)IHgt[i]), FHgt[IHgt[i]]*100, IOAperture(IHgt[i])*100, Nd_Rmt(HO.Ord[i])->FullObjTag());
      }
    #endif
    }
  }


//--------------------------------------------------------------------------
                     
void MdlNode::GetCnFlows(bool GetActualQm, CHgtOrdInfo &HO, int IONo, double &QmSLFeed, double &QvSLFeed, double &QvSLOutBelow)
  {
  SpContainer &Cn=HO.Container();

  QmSLFeed=0;      // total Slurry feed into this Vessel
  QvSLFeed=0;      // total Slurry feed into this Vessel
  QvSLOutBelow=0;  // Slurry leaving affecting this IO

  int MySeq=MaxIOList;
  for (int j=0; j<HO.NOrd(); j++)
    {
    int ii=HO.Ord[j];
    if (ii==IONo)
      MySeq=HO.Seq[j];
    }

  for (int j=0; j<HO.NOrd(); j++)
    {
    int ii=HO.Ord[j];
    // if IONo<0 then doing Overflow
    if (ii==IONo)
      {
      //MySeq=HO.Seq[j];
      }
    else if (IO_In(ii))
      {
      CSpPropInfo &Fp=*IOFlwProps(ii,0);
      double QmIn;
      if (GetActualQm)
        QmIn=IOConduit(ii)->QMass(som_SL);
      else
        QmIn=IOFB(ii,0)->m_fInXferNet ? 0.0 : IOQmEst_In(ii);
      QmSLFeed+=QmIn;
      QvSLFeed+=QmIn*(Fp.msVolL()*(1-Fp.VVolFrac())*(1-IOEstL2VOnEntry(ii))+
                      Fp.msVolL()*(Fp.VVolFrac())*IOEstV2LOnEntry(ii));
      }
    else if ((ii!=IONo) && IO_Out(ii) && (HO.Seq[j]</*=*/MySeq))
      {
      CSpPropInfo &Fp=*IOFlwProps(ii,0);
      double Qm=GetActualQm ? IOQm_Out(ii) : IOQmEst_Out(ii);
      QvSLOutBelow+=Qm*Fp.msVolL()*(Fp.NettLVolFrac());
      }
    }
  }
                      
//--------------------------------------------------------------------------

double MdlNode::GetSVolFrac(SpContainer & Cn,
                            long IONo, double Aperture, double FHgt, double QvSLRqd,
                            double QvSLFeed, double QvSLOutBelow,
                            SpMArray *pImage)
  {

  double OutT=FHgt+1.0*Aperture; // Frac height of top of Outlet
  double OutB=FHgt-0.0*Aperture; // Frac height of bottom of Outlet
  double AvlSFracEquil=(QvSLFeed-QvSLOutBelow)/GTZ(QvSLRqd);
  double AvlLevelEquil=OutB+AvlSFracEquil*Aperture;

  double Lvl=Cn.Level();
  double dT2Target=Max(Max(ICGetTimeIncMx()*sc_dTSettleRatio, ICGetSettleTime()), Cn.dLevelSettle);
  double ShVol=GTZ(Cn.ShapeVolume());
  double AvgVol=ShVol*Lvl+1.0*dT2Target*(QvSLFeed-QvSLOutBelow);
  double SVolFrac;
  if (0)
    {
    double VolLocked=ShVol*OutB;
    double AvlVol=GEZ(AvgVol-VolLocked);
    double RqdVol=dT2Target*QvSLRqd;

    double SVolFracCalc=AvlVol/GTZ(RqdVol);
    SVolFrac=Range(0.0, SVolFracCalc, 1.0);
    }
  if (1)
    {
    double RqdVol=dT2Target*QvSLRqd;

    double SVolFracCalc=(AvgVol-ShVol*OutB)/GTZ(RqdVol+ShVol*Aperture);
    SVolFrac=Range(0.0, SVolFracCalc, 1.0);
    }

  if (pImage)
    Cn.m_MixSet.GetImage(FHgt, *pImage);

  return SVolFrac;
  }
//--------------------------------------------------------------------------

void MdlNode::DoIOGetNetProps_Surge(int IONo, double Qm)
  {
  if (IOSurgeInfo(IONo)==NULL || IOHgtOrder(IONo)==NULL)
    return;

  CSrgFlwInfo &SI=*IOSurgeInfo(IONo);
  CHgtOrdInfo &HO=*IOHgtOrder(IONo);
  SpContainer &Cn=HO.Container();

  // Make + for the purpose of this arithmeitc
  Qm=fabs(Qm);

  if (fDoDbgBrk)
    { int xxx=0; };

  if (Cn.Mixed())
    {
    // VapourFrac is that which exists in the tank
    double VVolFrac=Cn.VolFrac(som_Gas, Cn.Temp(), Cn.Press());
    double SVolFrac=1.0-VVolFrac;
    IONetProps(IONo)->SetPropInfoVL(Cn, VVolFrac, SVolFrac, NULL);
    }
  else
    {
    double QmSLFeed=0;      // total Massflw Slurry feed into this Vessel
    double QvSLFeed=0;      // total Volflw Slurry feed into this Vessel
    double QvSLOutBelow=0;  // Slurry leaving affecting this IO

    GetCnFlows(false, HO, IONo, QmSLFeed, QvSLFeed, QvSLOutBelow);

    double Aperture=IOAperture(IONo, SolveInlineMethod());
    double FHgt=Range(0.00/*1*/, IOAbsFracHgt_Term(IONo, Cn), 1.0-Aperture);
    // mean Density leaving tank
    double MassTnk=Cn.Mass(som_SL);
    double MassIn=QmSLFeed*ICGetTimeInc();
    double DensIn=QmSLFeed/GTZ(QvSLFeed);
    double Dens=(MassTnk*Cn.SLEVDensity()+MassIn*DensIn)/GTZ(MassTnk+MassIn);

    double QvSLRqd=Qm/Min_FE_Rho(Dens);

    double SVolFrac;
    if (Cn.m_MixSet.On())
      {
      SpMArray Image;
      SVolFrac=GetSVolFrac(Cn, IONo, Aperture, FHgt, QvSLRqd, QvSLFeed, QvSLOutBelow, &Image);
      IONetProps(IONo)->SetPropInfoVL(Cn, 1-SVolFrac, SVolFrac, &Image);
      SetProdMakeup(PMU_IONo|PMU_Image, IONo, Image, Cn.Temp(), Cn.Press(), Cn.Model());
      }
    else
      {
      SVolFrac=GetSVolFrac(Cn, IONo, Aperture, FHgt, QvSLRqd, QvSLFeed, QvSLOutBelow, NULL);

      // Correct for Entrained Vapours
      //SFrac*= SLEVMSLfrac;
      IONetProps(IONo)->SetPropInfoVL(Cn, 1-SVolFrac, SVolFrac, NULL);
      }

    #if dbgModels
    if (dbgFlowConverge() || dbgFlowConvergeDbgBrk() && fDoDbgBrk)
      {
      dbglock();
      dbgp("FHgt%:%9.4f",FHgt*100);
      dbgp(" Lvl%%:%9.4f", Cn.Level()*100);
      dbgp(" SLvFd:%13.6g SLvOut:%13.6g SLvRqd:%13.6g Qm:%13.6g", QvSLFeed,QvSLOutBelow,QvSLRqd,Qm);
      // dbgpln(" Sf:%13.6g : %s", SFrac, DoingIO ? Nd_Rmt(Info.Hgt[io0])->FullObjTag() : "Overflow");
      dbgpln(" Svf%%:%13.6g : %s", SVolFrac*100, Nd_Rmt(IONo)->FullObjTag());
      dbgunlock();
      }
    #endif

    //#if dbgModels
    //if (dbgFlowConverge())
    //  dbgpln("SrgIO: %2i Qm:%12.5f  SLVFd:%12.5f  SLVOut:%12.5f %s -> %s",IONo,Qm,QvSLFeed,QvSLOutBelow, FullObjTag(), Nd_Rmt(IONo)->FullObjTag());
    //#endif
    }
  };

//--------------------------------------------------------------------------

flag MdlNode::Set_Sizes()
  {
  return true;
  };

//--------------------------------------------------------------------------

//DDBValueLst DDBFlwModes[]={
//  {1,  "AtLevel"},
//  {2,  "BlockSolids"},
//  {3,  "BlockLiquids"},
//  {4,  "BlockVapours"},
//  {5,  "PassSolids"},
//  {6,  "PassLiquids"},
//  {7,  "PassVapours"},
//  {8,  "PassImage"},
//  {0}};

void MdlNode::BuildDataDefnIOOptions(DataDefnBlk & DDB)
  {
  }

//--------------------------------------------------------------------------

void MdlNode::BuildDataDefnShowIOs(DataDefnBlk & DDB, flag bWithSigma)
  {
  Strng Nm;
  DDB.Visibility();

  }

//--------------------------------------------------------------------------

DDBValueLst DDBExitRules[]={
  {IOER_Lvl,      "At-Level",       MDD_Default},
  {IOER_All,      "Pass-All"        },
  {IOER_Vap,      "Pass-Vapours"    },
  {IOER_Liq,      "Pass-Liquids"    },
  {IOER_Sol,      "Pass-Solids"     },
  {IOER_NoVap,    "Block-Vapours"   },
  {IOER_NoLiq,    "Block-Liquids"   },
  {IOER_NoSol,    "Block-Solids"    },
  {IOER_ImgVap,   "Image-Vapours"   },
  {IOER_ImgLiq,   "Image-Liquids"   },
  {IOER_ImgSol,   "Image-Solids"    },
  {IOER_ImgNoVap, "Image-NoVapours" },
  {IOER_ImgNoLiq, "Image-NoLiquids" },
  {IOER_ImgNoSol, "Image-NoSolids"  },
  {IOER_ImgAll,   "Image-All"       },
  {0}};

void MdlNode::BuildDataDefnIOOpts(DataDefnBlk & DDB)
  {
  if (SolveDynamicMethod())
    {
    int N=0;
    for (int i=0; i<NoFlwIOs(); i++)
      {
      if (Nd_Rmt(i) && (IODesc_Self(i)->dwOther&IOOptsHide)==0)
        N++;
      }
    if (N)
      {
      if (DDB.BeginArray(this, "IOOpts", "NdOpts", NoFlwIOs(), 0, DDB_RqdPage))
        {
        char*pERTg=PrjFileVerNo()<42?"EntryRule":"ExitRule";
        Strng Tg;
        for (int i=0; i<NoFlwIOs(); i++)
          {
          if (Nd_Rmt(i) && (IODesc_Self(i)->dwOther&IOOptsHide)==0)
            {
            MAKEIOTG4DDB(Tg, i, 0);
            IOAreaRec & D=*IODesc_Self(i);
            if (DDB.BeginElement(this, Tg(), D.pName, 0, DDB_OptPage, 0))
              {
              DDB.Byte   (pERTg,           "",     DC_,         "",       xidIO_ExitRule0+i,    this, isParm|SetOnChange, DDBExitRules);
              DDB.Double ("HeightFrac",    "HFrc", DC_Frac,     "%",      xidIO_HFrc0+i,        this, ((D.dwOther&IOShwFracHgt)?noView:0)|isParmStopped);
              DDB.Double ("ApertureAngle", "",     DC_Ang,      "deg",    xidIO_ApertureAng0+i, this, ((D.dwOther&IOShwAperture || SolveInlineMethod())?noView:0)|isParmStopped);
              DDB.Double ("Datum",         "Z",    DC_L,        "m",      xidIO_Hgt0+i,         this, isResult|noFile|noSnap);
              DDB.Double ("Aperture",      "",     DC_Frac,     "%",      xidIO_Aperture0+i,    this, ((D.dwOther&IOShwAperture || SolveInlineMethod())?noView:0)|isResult|noFile|noSnap);
              DDB.Double ("EntrainRateV",  "",     DC_EntRate,  "%.s/kg", xidIO_EntrainRateB0+i, this, isParm);
              DDB.Double ("EntrainRateSL", "",     DC_EntRate,  "%.s/kg", xidIO_EntrainRateA0+i, this, isParm);
              DDB.Double ("Vap2LiqEst",    "",     DC_Frac,     "%",      xidIO_EstV2L0+i,       this, isParm);
              DDB.Double ("Liq2VapEst",    "",     DC_Frac,     "%",      xidIO_EstL2V0+i,       this, isParm);
              if (IOExitRule(i)&IOER_Img)
                DDB.Object(IOExitImage(i), this, "Image", NULL, DDB_RqdPage, -1, isParm);
              }
            }
          }
        }
      DDB.EndArray();
      }
    if (SolveBufferedMethod() && !DDB.ForFileSnpScn())
      {
      int N=0;
      for (int i=0; i<NoFlwIOs(); i++)
        {
        if (Nd_Rmt(i) && (IODesc_Self(i)->dwOther&IOEqnsHide)==0)
          N++;
        }
      if (N)
        {
        if (DDB.BeginArray(this, "IOEqns", "NdEqns", NoFlwIOs(), 0, DDB_RqdPage))
          {
          for (int i=0; i<NoFlwIOs(); i++)
            {
            if (Nd_Rmt(i) && (IODesc_Self(i)->dwOther&IOEqnsHide)==0)
              {
              Strng Tg;
              for (int fe=0; fe<NIOFBs(i); fe++)
                {
                MAKEIOTG4DDB(Tg, i, fe);
                DWORD UI=UIIOFBFlag+i+(fe<<8);
                if (DDB.BeginElement(this, Tg(),NULL,UI,DDB_OptPage,0/*MarkerClosed*/))
                  {
                  IOFB(i,fe)->BuildDataDefn(FBDDOpt_WithEqn/*|FBDDOpt_HideEqnStruct*/, DDB, this, NULL/*Nd_Rmt(i)->Tag()*/, UI, DDB_NoPage);
                  }
                }
              }
            }
          }
        DDB.EndArray();
        }
      }
    }
  }

//---------------------------------------------------------------------------

void MdlNode::BuildCommonEndDefnB481(DataDefnBlk &DDB)
  {
  DDB.Page("..", DDB_RqdPage);
  Strng Sym, Tag;

  if (m_dwAuditOptions&MNAudit_Qm)
    {
    DDB.Text("Mass Flows");
    int Cnt = 0;
    for (int i=0; i<NoProcessIOs(); i++)
      {
      Sym.Set("IO.%s.Qm", Nd_Rmt(i)->FullObjTag());
      Tag.Set("IO.[%i].Qm", i);
      DDB.Double(Tag(), Sym(),  DC_Qm , "kg/s",    xidIO_IoQm0+i,       this, isResult|isTag|noFile|noSnap);
      }
    for (int d=0; d<NoDirDIOs(); d++) 
      {
      CDirectFlwIO &D=*m_DirectIOs[d];
      Sym.Set("IO.%s.Qm", D.Tag);
      Tag.Set("IO.[%i].Qm", i);
      DDB.Double(Tag(), Sym(),  DC_Qm , "kg/s",    xidIO_IoQm0+i,       this, isResult|noFile|noSnap);
      i++;
      }

    DDB.Double("Qm_TotalIn",    "QmIn",   DC_Qm , "kg/s",    xidIO_QmTotalIn,     this, isResult|noFile|noSnap|InitHidden);
    DDB.Double("Qm_TotalOut",   "QmOut",  DC_Qm , "kg/s",    xidIO_QmTotalOut,    this, isResult|noFile|noSnap|InitHidden);
    DDB.Double("Nett_Qm",       "QmNett", DC_Qm , "kg/s",    xidIO_QmNett,       this, isResult|noFile|noSnap);

    if (NoDirectIOs()>0)
      {
      int iAdt=0;
      DDB.Text(" ");
      DDB.Text("Mass Audit");
      for (int d=FirstAdtDIO(); d<NoDirectIOs(); d++) 
        {
        CDirectFlwIO &D=*m_DirectIOs[d];
        Sym.Set("AI.%s.Qm", D.Tag);
        Tag.Set("AI.[%i].Qm", iAdt++);
        DDB.Double(Tag(), Sym(),  DC_Qm , "kg/s",    xidIO_DrQm0+d,       this, isResult|isTag|noFile|noSnap);
        }
      }
    DDB.Double("QmBalance",  "", DC_Qm , "kg/s",    xidIO_QmBalance,       this, isResult|noFile|noSnap);
    }

  if (m_dwAuditOptions && !DDB.ForFileSnpScn())
    {
    DDB.Text(" ");
    DDB.Text("Heat Flows");
    //DDB.Short ("IOCount",  "",  DC_   , "",        xidIO_Count,         this, isResult|noFileAtAll|InitHidden);
    int Cnt = 0;
    for (int i=0; i<NoProcessIOs(); i++)
      {
      Sym.Set("IO.%s.Hz", Nd_Rmt(i)->FullObjTag());
      Tag.Set("IO.[%i].Hz", i);
      DDB.Double(Tag(), Sym(),  DC_Pwr , "kJ/s",    xidIO_IoHz0+i,       this, isResult|isTag|noFile|noSnap);
      }
    for (int d=0; d<NoDirDIOs(); d++) 
      {
      CDirectFlwIO &D=*m_DirectIOs[d];
      Sym.Set("IO.%s.Hz", D.Tag);
      Tag.Set("IO.[%i].Hz", i);
      DDB.Double(Tag(), Sym(),  DC_Pwr , "kJ/s",    xidIO_IoHz0+i,       this, isResult|noFile|noSnap);
      i++;
      }

    DDB.Double("Nett_Hz",       "HzNett", DC_Pwr, "kJ/s",    xidIO_HzNett,       this, isResult|noFile|noSnap);

    if (NoDirectIOs()>0)
      {
      int iAdt=0;
      DDB.Text(" ");
      DDB.Text("Heat Audit");
      for (int d=FirstAdtDIO(); d<NoDirectIOs(); d++) 
        {
        CDirectFlwIO &D=*m_DirectIOs[d];
        Sym.Set("AI.%s.Hz", D.Tag);
        Tag.Set("AI.[%i].Hz", iAdt++);
        DDB.Double(Tag(), Sym(),  DC_Pwr , "kJ/s",    xidIO_DrHz0+d,       this, isResult|isTag|noFile|noSnap);
        }
      }
    DDB.Double("HzBalance",  "", DC_Pwr , "kJ/s",    xidIO_HzBalance,       this, isResult|noFile|noSnap);
    }
  }
//---------------------------------------------------------------------------

void MdlNode::BuildCommonEndDefn(DataDefnBlk &DDB)
  {
  //  if (CollectChgStats() && SolveDirectMethod() && (ChgStats.GetSize()>0))
  //    ChngTrcBuildDataDefn(DDB);

  Strng Sym, Tag;

  if (m_dwAuditOptions!=0 && !DDB.ForFileSnpScn())
    {
    if (DDB.BeginStruct(this, "Audit", NULL, DDB_RqdPage))
      {
      if (DDB.BeginStruct(this, "Flows", NULL, DDB_NoPage))
        {
        if (DDB.BeginStruct(this, "Mass", NULL, DDB_NoPage))
          {
          int iIO=0; 
          for (int i=0; i<NoProcessIOs(); i++)
            {
            Sym.Set("Lnk.%s", Nd_Rmt(iIO)->FullObjTag());
            Tag.Set("Lnk%i", iIO);
            DDB.Double(Tag(), Sym(),  DC_Qm , "kg/s",    xidIO_IoQm0+iIO,       this, isResult|/*isTag|*/noFile|noSnap);
            iIO++;
            }

          DDB.Double("Nett",       "", DC_Qm ,  "kg/s",    xidIO_QmNett,       this, isResult|noFile|noSnap);

          int iDir=0;
          for (int d=0; d<NoDirDIOs(); d++) 
            {
            CDirectFlwIO &D=*m_DirectIOs[d];
            Sym.Set("Dir.%s", D.Tag);
            Tag.Set("Dir%i", iDir);
            DDB.Double(Tag(), Sym(),  DC_Qm , "kg/s",    xidIO_IoQm0+iIO,       this, isResult|noFile|noSnap);
            iIO++;
            iDir++;
            }

          if (NoDirectIOs()>0)
            {
            int iAdt=0;
            for (int d=FirstAdtDIO(); d<NoDirectIOs(); d++) 
              {
              CDirectFlwIO &D=*m_DirectIOs[d];
              Sym.Set("Adt.%s", D.Tag);
              Tag.Set("Adt%i", iAdt++);
              DDB.Double(Tag(), Sym(),  DC_Qm , "kg/s",    xidIO_DrQm0+iDir,       this, isResult|/*isTag|*/noFile|noSnap);
              iDir++;
              }
            }
          DDB.Double("Balance",     "", DC_Qm ,   "kg/s",  xidIO_QmBalance,        this, isResult|noFile|noSnap);
          DDB.Double("Balance.Rel", "", DC_Frac,  "%",     xidIO_QmBalanceRel,     this, isResult|noFile|noSnap);
          }
        DDB.EndStruct();

        DDB.Text(" ");
        if (DDB.BeginStruct(this, "Heat", NULL, DDB_NoPage))
          {
          int iIO=0; 
          for (int i=0; i<NoProcessIOs(); i++)
            {
            Sym.Set("Lnk.%s", Nd_Rmt(iIO)->FullObjTag());
            Tag.Set("Lnk%i", iIO);
            DDB.Double(Tag(), Sym(),  DC_Pwr , "kJ/s",      xidIO_IoHz0+iIO,       this, isResult|/*isTag|*/noFile|noSnap);
            iIO++;
            }

          DDB.Double("Nett",       "", DC_Pwr, "kJ/s",      xidIO_HzNett,        this, isResult|noFile|noSnap);

          int iDir=0;
          for (int d=0; d<NoDirDIOs(); d++) 
            {
            CDirectFlwIO &D=*m_DirectIOs[d];
            Sym.Set("Dir.%s", D.Tag);
            Tag.Set("Dir%i", iDir);
            DDB.Double(Tag(), Sym(),  DC_Pwr , "kJ/s",    xidIO_IoHz0+iIO,       this, isResult|noFile|noSnap);
            iIO++;
            iDir++;
            }

          if (NoDirectIOs()>0)
            {
            int iAdt=0;
            for (int d=FirstAdtDIO(); d<NoDirectIOs(); d++) 
              {
              CDirectFlwIO &D=*m_DirectIOs[d];
              Sym.Set("Adt.%s", D.Tag);
              Tag.Set("Adt%i", iAdt++);
              DDB.Double(Tag(), Sym(),  DC_Pwr , "kJ/s",  xidIO_DrHz0+iDir,       this, isResult|/*isTag|*/noFile|noSnap);
              iDir++;
              }
            }
          DDB.Double("Balance",     "", DC_Pwr , "kJ/s",    xidIO_HzBalance,       this, isResult|noFile|noSnap);
          DDB.Double("Balance.Rel", "", DC_Frac,  "%",      xidIO_HzBalanceRel,    this, isResult|noFile|noSnap);
          }
        DDB.EndStruct();
        }
      DDB.EndStruct();

      DDB.Text(" ");
      DDB.Page("..", DDB_OptPage);
      if (DDB.BeginStruct(this, "Totals", NULL, DDB_NoPage))
        {
        if (DDB.BeginStruct(this, "Mass", NULL, DDB_NoPage))
          {
          int iIO=0; 
          for (int i=0; i<NoProcessIOs(); i++)
            {
            Sym.Set("Lnk.%s", Nd_Rmt(iIO)->FullObjTag());
            Tag.Set("Lnk.[%i]", iIO);
            DDB.Double(Tag(), Sym(),  DC_M , "kg",       xidIO_IoQmTotal0+iIO,       this, isResult|/*isTag|*/noFile|noSnap);
            iIO++;
            }

          DDB.Double("Nett",       "", DC_M ,  "kg",     xidIO_QmNettTotal,       this, isResult|noFile|noSnap);

          int iDir=0;
          for (int d=0; d<NoDirDIOs(); d++) 
            {
            CDirectFlwIO &D=*m_DirectIOs[d];
            Sym.Set("Dir.%s", D.Tag);
            Tag.Set("Dir.[%i]", iDir);
            DDB.Double(Tag(), Sym(),  DC_M , "kg",     xidIO_IoQmTotal0+iIO,       this, isResult|noFile|noSnap);
            iIO++;
            iDir++;
            }

          if (NoDirectIOs()>0)
            {
            int iAdt=0;
            for (int d=FirstAdtDIO(); d<NoDirectIOs(); d++) 
              {
              CDirectFlwIO &D=*m_DirectIOs[d];
              Sym.Set("Adt.%s", D.Tag);
              Tag.Set("Adt.[%i]", iAdt++);
              DDB.Double(Tag(), Sym(),  DC_M , "kg",   xidIO_DrQmTotal0+iDir,       this, isResult|/*isTag|*/noFile|noSnap);
              iDir++;
              }
            }
          DDB.Double("Balance",       "", DC_M , "kg",   xidIO_QmBalanceTotal,       this, isResult|noFile|noSnap);
          DDB.Double("Balance.Rel",   "", DC_Frac, "%",  xidIO_QmBalanceTotalRel,   this, isResult|noFile|noSnap);
          }
        DDB.EndStruct();

        DDB.Text(" ");
        if (DDB.BeginStruct(this, "Heat", NULL, DDB_NoPage))
          {
          int iIO=0; 
          for (int i=0; i<NoProcessIOs(); i++)
            {
            Sym.Set("Lnk.%s", Nd_Rmt(iIO)->FullObjTag());
            Tag.Set("Lnk.[%i]", iIO);
            DDB.Double(Tag(), Sym(),  DC_E , "kJ",      xidIO_IoHzTotal0+iIO,         this, isResult|/*isTag|*/noFile|noSnap);
            iIO++;
            }

          DDB.Double("Nett",       "", DC_E, "kJ",      xidIO_HzNettTotal,          this, isResult|noFile|noSnap);

          int iDir=0;
          for (int d=0; d<NoDirDIOs(); d++) 
            {
            CDirectFlwIO &D=*m_DirectIOs[d];
            Sym.Set("Dir.%s", D.Tag);
            Tag.Set("Dir.[%i]", iDir);
            DDB.Double(Tag(), Sym(),  DC_E , "kJ",    xidIO_IoHzTotal0+iIO,         this, isResult|noFile|noSnap);
            iIO++;
            iDir++;
            }

          if (NoDirectIOs()>0)
            {
            int iAdt=0;
            for (int d=FirstAdtDIO(); d<NoDirectIOs(); d++) 
              {
              CDirectFlwIO &D=*m_DirectIOs[d];
              Sym.Set("Adt.%s", D.Tag);
              Tag.Set("Adt.[%i]", iAdt++);
              DDB.Double(Tag(), Sym(),  DC_E , "kJ",  xidIO_DrHzTotal0+iDir,        this, isResult|/*isTag|*/noFile|noSnap);
              iDir++;
              }
            }
          DDB.Double("Balance",     "", DC_E , "kJ",    xidIO_HzBalanceTotal,       this, isResult|noFile|noSnap);
          DDB.Double("Balance.Rel", "", DC_Frac,  "%",  xidIO_HzBalanceTotalRel,    this, isResult|noFile|noSnap);
          }
        DDB.EndStruct();
        }
      DDB.EndStruct();

      DDB.Text(" ");
      DDB.Page("..", DDB_OptPage);
      if (DDB.BeginStruct(this, "Counts", NULL, DDB_NoPage))
        {
        DDB.Long("Lnk", "",  DC_ , "",       xidIO_LnkCount,       this, isResult|/*isTag|*/noFile|noSnap);
        DDB.Long("Dir", "",  DC_ , "",       xidIO_DirCount,       this, isResult|/*isTag|*/noFile|noSnap);
        DDB.Long("Adt", "",  DC_ , "",       xidIO_AdtCount,       this, isResult|/*isTag|*/noFile|noSnap);
        }
      DDB.EndStruct();
      }

    DDB.EndStruct();
    }

#if WITHIOSENERGY
  DDB.Double("Qv_TotalIn",    "QvIn",   DC_Qv , "L/s",     xidIO_QvTotalIn,     this, isResult|noFile|noSnap|InitHidden);
  DDB.Double("Qv_TotalOut",   "QvOut",  DC_Qv , "L/s",     xidIO_QvTotalOut,    this, isResult|noFile|noSnap|InitHidden);
  //DDB.Double("Nett_Qv",       "QvNett", DC_Qv , "L/s",     xidIO_QvSigma,       this, isResult|noFile|noSnap);
  DDB.Double("Hs_TotalIn",    "HsIn",   DC_Pwr, "kJ/s",    xidIO_HsTotalIn,     this, isResult|noFile|noSnap|InitHidden);
  DDB.Double("Hs_TotalOut",   "HsOut",  DC_Pwr, "kJ/s",    xidIO_HsTotalOut,    this, isResult|noFile|noSnap|InitHidden);
  DDB.Double("Nett_Hs",       "HsNett", DC_Pwr, "kJ/s",    xidIO_HsSigma,       this, isResult|noFile|noSnap);
  DDB.Double("Hf_TotalIn",    "HfIn",   DC_Pwr, "kJ/s",    xidIO_HfTotalIn,     this, isResult|noFile|noSnap|InitHidden);
  DDB.Double("Hf_TotalOut",   "HfOut",  DC_Pwr, "kJ/s",    xidIO_HfTotalOut,    this, isResult|noFile|noSnap|InitHidden);
  DDB.Double("Nett_Hf",       "HfNett", DC_Pwr, "kJ/s",    xidIO_HfSigma,       this, isResult|noFile|noSnap);
  DDB.Double("Hz_TotalIn",    "HzIn",   DC_Pwr, "kJ/s",    xidIO_HzTotalIn,     this, isResult|noFile|noSnap|InitHidden);
  DDB.Double("Hz_TotalOut",   "HzOut",  DC_Pwr, "kJ/s",    xidIO_HzTotalOut,    this, isResult|noFile|noSnap|InitHidden);
  DDB.Double("Nett_Hz",       "HzNett", DC_Pwr, "kJ/s",    xidIO_HzSigma,       this, isResult|noFile|noSnap);
#endif

  if (m_dwAuditOptions && !DDB.ForFileSnpScn())
    {
    if (1)//OrigPrjFileVerNo()<81)
      {
      Strng Nm;
      DDB.Text("IO Tags");
      DDB.Short ("IOCount",  "",  DC_   , "",        xidIO_Count,         this, isResult|noFileAtAll|InitHidden);
      DDB.Short ("IOInputCount",  "InCnt",  DC_   , "",        xidIO_InCnt,         this, isResult|noFileAtAll|InitHidden);
      int Cnt = 0;
      for (int i=0; i<NoFlwIOs(); i++)
        {
        if (LnkFlowDefinedIn(i))
          {
          Nm.Set("IO.In%d", Cnt++);
          DDB.String("",   Nm(),  DC_,    "", xidIO_StreamTag0+i,       this, isResult|isTag|noFileAtAll|InitHidden);
          }
        }

      DDB.Short ("IOOutputCount", "OutCnt", DC_   , "",        xidIO_OutCnt,        this, isResult|noFileAtAll|InitHidden);
      Cnt = 0;
      for (i=0; i<NoFlwIOs(); i++)
        {
        if (LnkFlowDefinedOut(i))
          {
          Nm.Set("IO.Out%d", Cnt++);
          DDB.String("",   Nm(),  DC_,    "", xidIO_StreamTag0+i,       this, isResult|isTag|noFileAtAll|InitHidden);
          }
        }
      }
    }
  else
    {
    DDB.Page("Audit", DDB_RqdPage);
    }

  if (OrigPrjFileVerNo()<81 && !DDB.ForFileSnpScn())
    {
    BuildCommonEndDefnB481(DDB);
    }

  DDB.Text("");
  if (m_Closure.GetSize())
    {
    if (DDB.BeginArray(this, "NdAudit", "Audit", m_Closure.GetSize(), 0, DDB_NoPage))
      {
      for (int i=0; i<m_Closure.GetSize(); i++)
        {
        if (Nd_Rmt(i))
          {
          if (DDB.BeginElement(this, i, NULL, i, DDB_OptPage, 0))
            {
            DDB.Double("MassIn",              "",  DC_Qm,  "kg/s",    xidIO_JoinMassFlwIn,        this, isResult);
            DDB.Double("MassGain",            "",  DC_Qm,  "kg/s",    xidIO_JoinMassGain,         this, isResult);
            DDB.Double("MassGain.RCT",        "",  DC_Qm,  "kg/s",    xidIO_RCTMassGain,          this, isResult);
            DDB.Double("MassOut",             "",  DC_Qm,  "kg/s",    xidIO_JoinMassFlwOut,       this, isResult);
            DDB.Double("FeedCp@T",            "",  DC_CpMs,"kJ/kg.C", xidIO_JoinFeedCpT,          this, isResult);
            DDB.Double("ProdCp@T",            "",  DC_CpMs,"kJ/kg.C", xidIO_JoinProdCpT,          this, isResult);
            DDB.Double("TotalHeatIn",         "",  DC_Pwr, "kJ/s",    xidIO_JoinHfIn,             this, isResult);
            DDB.Double("TotalHeatGain",       "",  DC_Pwr, "kJ/s",    xidIO_JoinHfGain,           this, isResult);
            DDB.Double("TotalHeatGain.RCT",   "",  DC_Pwr, "kJ/s",    xidIO_RCTHfGain,            this, isResult);
            DDB.Double("TotalHeatOut",        "",  DC_Pwr, "kJ/s",    xidIO_JoinHfOut,            this, isResult);
            DDB.Double("PowerIn",             "",  DC_Pwr, "kJ/s",    xidIO_JoinPowerIn,          this, isResult);
            DDB.Double("PowerIn.RCT",         "",  DC_Pwr, "kJ/s",    xidIO_RCTPowerIn,           this, isResult);
            DDB.Double("PowerIn.EHX",         "",  DC_Pwr, "kJ/s",    xidIO_EHXPowerIn,           this, isResult);
            DDB.Double("SensibleHeatIn",      "",  DC_Pwr, "kJ/s",    xidIO_JoinHsIn,             this, isResult);
            DDB.Double("SensibleHeatGain",    "",  DC_Pwr, "kJ/s",    xidIO_JoinHsGain,           this, isResult);
            DDB.Double("SensibleHeatGain.RCT","",  DC_Pwr, "kJ/s",    xidIO_RCTHsGain,            this, isResult);
            DDB.Double("SensibleHeatOut",     "",  DC_Pwr, "kJ/s",    xidIO_JoinHsOut,            this, isResult);
            DDB.Double("VLE.HfChange@0",      "",  DC_Pwr, "kJ/s",    xidIO_VLEHsGain,            this, isResult);
            DDB.Double("HOR.Std@0",           "",  DC_Pwr, "kJ/s",    xidIO_RCTHOR_Std0,          this, isResult);
            DDB.Double("HOR.Mdl@0",           "",  DC_Pwr, "kJ/s",    xidIO_RCTHOR_Mdl0,          this, isResult);
            DDB.Double("HOR.Used@0",          "",  DC_Pwr, "kJ/s",    xidIO_RCTHOR_Used0,         this, isResult);
            DDB.Double("SensibleHeatChg",     "",  DC_Pwr, "kJ/s",    xidIO_JoinHsChg,            this, isResult);
            DDB.Double("HOR.Diff@0",          "",  DC_Pwr, "kJ/s",    xidIO_RCTHOR_Diff0,         this, isResult);
            DDB.Double("TotalHeatChg",        "",  DC_Pwr, "kJ/s",    xidIO_JoinHfChg,            this, isResult);
            }
          }
        }
      }
    DDB.EndArray();

    DDB.Text("Closure Clipboard:");
    DDB.Button("Copy_Compact", "", DC_, "", xidIO_CopyClsCompact, this, isParm);
    DDB.Button("Copy_Full", "", DC_, "", xidIO_CopyClsFull, this, isParm);
    }
  else
    DDB.Text("No Closure Information");
  FlwNode::BuildCommonEndDefn(DDB);
  };

//---------------------------------------------------------------------------

const double MinAvailFrac=1.0e-9;

void MdlNode::MakeUpNodeTransferAvail(int JoinId)
  {
  if (JoinId>=NJoins())
    return;

  double A=0.0;
  double O=0.0;

  int I[MaxIOList+1];
  FillIOIndexList(JoinId, I);

  int dbg=0;//fDoDbgBrk;
  if (dbg)
    dbgpln("NdAvail %s", Tag());

  flag OK=0;
  for (int i, ic = 0; (i=I[ic])>=0; ic++)
    {
    if (i<NoFlwIOs())
      {
      CFlange &F=*IOFlange(i);
      if (IO_In(i) && F.IsMakeUpAvail())
        {
        A+=F.MakeUpAvail();
        OK=1;
        if (dbg)
          dbgpln("  I:%12.5f %s", F.MakeUpAvail(), Nd_Rmt(i)->Tag());
        }
      else if (IO_Out(i))
        if (F.IsMakeUpReqd())
          O+=F.MakeUpReqd();
        else
          O+=1.0e-6;
      }
    }
  if (OK)
    {
    O=GTZ(O);
    for (int i, ic = 0; (i=I[ic])>=0; ic++)
      if (IO_Out(i))
        {
        CFlange &F=*IOFlange(i);
        if (F.IsMakeUpReqd())
          F.SetMakeUpAvail(A*Max(MinAvailFrac, F.MakeUpReqd()/O));
        else
          F.SetMakeUpAvail(A*MinAvailFrac);
        if (dbg)
          dbgpln("  O:%12.5f %s", F.MakeUpAvail(), Nd_Rmt(i)->Tag());
        }
    }
  };

//--------------------------------------------------------------------------

void MdlNode::MakeUpNodeTransferReqd(int JoinId)
  {
  if (JoinId>=NJoins())
    return;

  double R=0.0, Q=0.0;//, S=0.0;

  int I[MaxIOList+1];
  FillIOIndexList(JoinId, I);

  int dbg=fDoDbgBrk;
  if (dbg)
    dbgpln("NdReqd %s", Tag());

  //if (stricmp(sTag(), "A723H01")==0)
  //  { int xx=0; }

  bNodeMakeUpReqd=0;
  int NIns=0;
  for (int i, ic = 0; (i=I[ic])>=0; ic++)
    if (i<NoFlwIOs())
      {
      if (IO_Out(i))
        {
        if (IOFlange(i)->IsMakeUpReqd())
          {
          R+=IOFlange(i)->MakeUpReqd();
          bNodeMakeUpReqd=1;
          if (dbg)
            dbgpln("  O:a%12.5f r%12.5f %s", IOFlange(i)->MakeUpAvail(), IOFlange(i)->MakeUpReqd(), Nd_Rmt(i)->Tag());
          }
        }
      else if (IO_In(i))
        {
        NIns++;
        if (!IOFlange(i)->IsMakeUpAvail())
          Q+=IOQmPrvPB_In(i);
        }
      }

  if (bNodeMakeUpReqd)
    {
    double A=0;
    for (int i, ic = 0; (i=I[ic])>=0; ic++)
      {
      if (IO_In(i) && IOFlange(i)->IsMakeUpAvail())
        A+=IOFlange(i)->MakeUpAvail();
      }
    R=Max(0.0, R-Q);
    A=GTZ(A);
    for (ic = 0; (i=I[ic])>=0; ic++)
      if (IO_In(i) && IOFlange(i)->IsMakeUpAvail())
        {
        // Distibute according to MakeUpAvail
        IOFlange(i)->SetMakeUpReqd(R*Max(MinAvailFrac, IOFlange(i)->MakeUpAvail()/A));
        if (dbg)
          dbgpln("  I:a%12.5f r%12.5f %s", IOFlange(i)->MakeUpAvail(), IOFlange(i)->MakeUpReqd(), Nd_Rmt(i)->Tag());
        }
    }
  };

//--------------------------------------------------------------------------

flag MdlNode::MakeUpNodeEvalProducts(int JoinId, SpConduit &Sd)
  {
  if (JoinId>=NJoins())
    return False;
  if (!bNodeMakeUpReqd)
    return False;

  flag Done[MaxIOList+1];
  flag Rqd[MaxIOList+1];
  double OutRqd[MaxIOList+1];
  int I[MaxIOList+1];
  FillIOIndexList(JoinId, I);

  double  Qot=0.0;
  for (int i, ic = 0; (i=I[ic])>=0; ic++)
    {
    Rqd[i]=0;
    Done[i]=0;
    OutRqd[i]=0.;
    if (IO_Out(i) && !IOFlange(i)->IsMakeUpReqd())
      Qot += IOQmEst_Out(i);
    }

  double Press=Sd.Press();

  double TotOutRqd=0.0;
  for (ic = 0; (i=I[ic])>=0; ic++)
    if (IO_Out(i) && IOFlange(i)->IsMakeUpReqd())
      {
      Rqd[i]=1;
      OutRqd[i]=Max(OutRqd[i], IOFlange(i)->MakeUpReqd());
      TotOutRqd+=OutRqd[i];
      }

  // First Supply Makeup Feeds
  double Qin=Max(0.0, Sd.QMass(som_ALL));
  double Scl=(TotOutRqd>Qin) ? Qin/GTZ(TotOutRqd) : 1.0;
  for (ic = 0; (i=I[ic])>=0; ic++)
    if (IO_Out(i) && Rqd[i])
      IOConduit(i)->QSetM(Sd, som_ALL, OutRqd[i]*Scl, Press);

  // Now Supply Others
  Qin=GEZ(Qin-TotOutRqd);
  Scl=(Qot>Qin) ? Qin/GTZ(Qot) : 1.0;
  for (ic = 0; (i=I[ic])>=0; ic++)
    if (IO_Out(i) && !Rqd[i])
      IOConduit(i)->QSetM(Sd, som_ALL, IOQmEst_Out(i)*Scl, Press);

  return true;
  };

//--------------------------------------------------------------------------

flag MdlNode::MakeUpNodeEvalProducts(int JoinId)
  {
  if (JoinId>=NJoins())
    return False;
  if (!bNodeMakeUpReqd)
    return False;

//  flag Done[MaxIOList+1];
//  flag Rqd[MaxIOList+1];
//  double OutRqd[MaxIOList+1];
  int I[MaxIOList+1];
  FillIOIndexList(JoinId, I);

  SpConduit &Sd=*GetWrkConduit("MkUpEVP", chLINEID());
  CSpMdlSlct Slct;
  for (int i, ic = 0; (i=I[ic])>=0; ic++)
    if (IO_In(i))
      Slct.Add(IOConduit(i)->Model(), IOQm_In(i));

  double Press=Std_P;
  Sd.QZero();
  Sd.SetPress(Press);
  Sd.SelectModel(Slct);
  for (ic = 0; (i=I[ic])>=0; ic++)
    if (IO_In(i))
      Sd.QAddF(*IOConduit(i), som_ALL, 1.0);

  return MakeUpNodeEvalProducts(JoinId, Sd);
  };

//--------------------------------------------------------------------------

void MdlNode::MakeUpLinkTransferAvail()
  {
  if (NoFlwIOs()<2)
    return;
  int iIn=IO_In(0) ? 0 : 1;
  int iOut=(iIn+1)%2;
  IOFlange(iOut)->SetMakeUpAvail(IOFlange(iIn)->MakeUpAvail());
  };

//--------------------------------------------------------------------------

void MdlNode::MakeUpLinkTransferReqd()
  {
  if (NoFlwIOs()<2)
    return;
  int iIn=IO_In(0) ? 0 : 1;
  int iOut=(iIn+1)%2;
  IOFlange(iIn)->SetMakeUpReqd(IOFlange(iOut)->MakeUpReqd());
  };

//--------------------------------------------------------------------------

void MdlNode::SigmaIOFlows(CSigmaIOFlows & Sigma)
  {
  Sigma.Clear();
  for (int i=0; i<NoFlwIOs(); i++)
    {
    Sigma.Mass+=(IO_In(i) ? 1 : -1) * IOConduit(i)->QMass();
    Sigma.MaxMass=Max(Sigma.MaxMass, (IO_In(i) ? 1 : -1) * IOConduit(i)->QMass());
    Sigma.Heat+=(IO_In(i) ? 1 : -1) * IOConduit(i)->totHf();
    Sigma.MaxHeat=Max(Sigma.MaxHeat, fabs((IO_In(i) ? 1 : -1) * IOConduit(i)->totHf()));
    }
  }

CSigmaIOFlows::TestResult CSigmaIOFlows::Test()
  {
  _asm int 3;
  return Balanced;
  };

//--------------------------------------------------------------------------

flag MdlNode::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case 1: pS="N\tOverFlowing";                    return 1;
    case 2: pS="N\tOverflow Targeted to Self";      return 1;
    case 3: pS="N\tOverflow Target not Found";      return 1;
    case 4: pS="N\tGas Venting";                    return 1;
    case 5: pS="N\tGasVent Targeted to Self";       return 1;
    case 6: pS="N\tGasVent Target not Found";       return 1;
    case 7: pS="E\tMass Imbalance";                 return 1;
    case 8: pS="E\tHeat Imbalance";                 return 1;
    case 9: pS="E\tMass and Heat Imbalance";        return 1;
    default:
      return FlwNode::CIStrng(No, pS);
    }
  };

//==========================================================================
//
//
//
//==========================================================================

CEquipmentThermals::CEquipmentThermals(TaggedObject * pObj, SpContainer * pCn)
  {
  m_pObj=pObj;
  m_pCn=pCn;
  m_dMass=100;
  m_dTemp=AmbientTemp();
  m_dCp=1.0;
  m_dHTC1=10.0;
  m_dHTC2=1.0;
  m_bActive=false;//true; kga changed default from true to false 20/5/2003
  m_bConfigured=false;
  };
CEquipmentThermals::~CEquipmentThermals()
  {
  };

void CEquipmentThermals::BuildDataDefn(DataDefnBlk & DDB, char* pTag_, char* pTagComment, DDBPages PageIs, dword UserInfo)
  {
  if (DDB.BeginStruct(m_pObj, pTag_ ? pTag_ : "Equipment", pTagComment, PageIs, UserInfo))
    {
    DDB.CheckBoxBtn("Enable", "",          DC_,       "",         &m_bActive,      m_pObj, isParm);
    DDB.Double("Mass",        "M",         DC_M,      "kg",       &m_dMass,        m_pObj, isParm);
    DDB.Double("",            "Cp",        DC_CpMs,     "kJ/kg.C",  &m_dCp,          m_pObj, isParm);
    DDB.Double("Temp",        "T",         DC_T,      "C",        &m_dTemp,        m_pObj, isParm);
    m_dAmbient.BuildDataDefn(DDB, "", "AmbientT", DC_T, "C", xidEqThermAmbT, m_pObj, "Atmos", "LclAmb");
//    DDB.Double("",            "AmbientT",  DC_T,      "C",        &m_dAmbient,     m_pObj, isParm|NAN_OK);
    DDB.Double("HTC_Cn2Eq",   "HTCEq",     DC_UA,     "kW/K",     &m_dHTC1,        m_pObj, isParm);
    DDB.Double("HTC_Eq2Am",   "HTCAm",     DC_UA,     "kW/K",     &m_dHTC2,        m_pObj, isParm);
    DDB.Double("TimeConstEq", "TauEq",     DC_Time,   "s",        xidEqThermTau1,  m_pObj, isResult);
    DDB.Double("TimeConstAm", "TauAm",     DC_Time,   "h",        xidEqThermTau2,  m_pObj, isResult);
    DDB.Double("PowerEq",     "PwrEq",     DC_Pwr,    "kW",       &m_dPwr1,        m_pObj, isResult);
    DDB.Double("PowerAm",     "PwrAm",     DC_Pwr,    "kW",       &m_dPwr2,        m_pObj, isResult);
    }
  DDB.EndStruct();
  };

inline double ThermTimeConst(double Cp1, double Cp2, double T1, double T2, double HTC)
  {
  double Tf=(Cp1*T1+Cp2*T2)/GTZ(Cp1+Cp2);
  double Tau1,Tau2;
  //if (Cp1<Cp2)
    Tau1=(fabs(T1-Tf)*Cp1)/GTZ(fabs(T1-Tf)*HTC);
  //else
    Tau2=(fabs(T2-Tf)*Cp2)/GTZ(fabs(T2-Tf)*HTC);
  return Tau1;
  };

flag CEquipmentThermals::DataXchg(DataChangeBlk & DCB)
  {
  m_bConfigured=true;
  if (DCB.lHandle==xidEqThermAmbT)
    {
    m_dAmbient.SetVal(AmbientTemp(), m_pObj);
    if (m_dAmbient.DataXchg(DCB,xidEqThermAmbT , m_pObj))
      return 1;
    }

  switch (DCB.lHandle)
    {
    case xidEqThermTau1:
      {
      DCB.D=ThermTimeConst(m_pCn->totCp(), m_dMass*m_dCp, m_pCn->Temp(), m_dTemp, m_dHTC1);
      return 1;
      }
    case xidEqThermTau2:
      {
      DCB.D=ThermTimeConst(m_dMass*m_dCp, 1e12, m_dTemp, m_dAmbient(), m_dHTC2);
      return 1;
      }
    }

  return 0;
  };

flag CEquipmentThermals::ValidateData(ValidateDataBlk & VDB)
  {
  m_dAmbient.SetVal(AmbientTemp(), m_pObj);
  return true;//Enabled() ? pPMU->ValidateData(VDB) : 0;
  };

void CEquipmentThermals::EvalDerivs()
  {

  m_dAmbient.SetVal(AmbientTemp(), m_pObj);

  // HeatFlow inwards = +
  // Limit rates of vchange to keep stable
  if (!m_bActive || !m_bConfigured)
    {
    m_dPwr1=0;
    m_dPwr2=0;
    m_dPwr1=0;
    m_dTemp=m_pCn->Temp();
    m_dTempDT=0;
    return;
    }

  double DT1=m_dTemp-m_pCn->Temp();
  double DT2=m_dAmbient()-m_dTemp;
  m_dPwr1=DT1*m_dHTC1;
  m_dPwr2=DT2*m_dHTC2;

  double DDT1=m_dPwr1/GTZ(m_pCn->totCp())*ICGetTimeInc();

  const double LimitFrac1=0.5;
  if (DDT1*DT1 > 0.0 && fabs(DDT1)>fabs(LimitFrac1*DT1))
    m_dPwr1 *= LimitFrac1*DT1/DDT1;

  double PwrDiff=m_dPwr2-m_dPwr1;
  m_dTempDT=PwrDiff/GTZ(m_dMass*m_dCp);

  double TBodyFinal=m_pCn->Temp()+(m_dAmbient()-m_pCn->Temp())*m_dHTC2/GTZ(m_dHTC1+m_dHTC2);

  const double LimitFrac2=0.9;
  double MaxDT=LimitFrac2*(TBodyFinal-m_dTemp);
  double DDT2=m_dTempDT*ICGetTimeInc();
  if (DDT2*MaxDT > 0.0 && fabs(DDT2)>fabs(MaxDT))
    {
    if (MaxDT>0)
      m_dTempDT=Min(m_dTempDT, MaxDT);
    else
      m_dTempDT=Max(m_dTempDT, MaxDT);
    }

  m_pCn->AddInternalDerivs(NULL, m_dPwr1);
  };

// --------------------------------------------------------------------------

void CEquipmentThermals::ConvergeStates(CConvergeStateBlk &CSB)
  {
  if (!m_bActive || !m_bConfigured)
    {
    return;
    }
  };

// --------------------------------------------------------------------------

void CEquipmentThermals::SetStateAction(IE_Enables E)
  {
  m_States.RemoveAll();
  switch (E)
    {
    case IE_Integrate:
      m_States.Add(&m_dTemp, &m_dTempDT, &m_dTemp);
      break;
    case IE_SaveState:
      m_States.Add(&m_dTemp);
      break;
    default:
      break;
    }
  };
// --------------------------------------------------------------------------

void CEquipmentThermals::ODEOperate(CODEDataBlock & ODB)
  {
  if (!m_bActive || !m_bConfigured)
    return;

  switch (ODB.m_Cmd)
    {
    case eStateAdvance:
      {
      ODB.m_pFn(ODB, m_States);

        // Do Mass Checks
//        for (int s=0; s<SDB.No(); s++)
//          M[s]=GEZ(M[s]);

      break;
      }
    case eStateConverge:
    case eStateLoad:
    case eStateDiskLoad:
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

// ==========================================================================
//
//
//
// ==========================================================================

