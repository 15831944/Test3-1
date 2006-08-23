//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"

#include "sc_defs.h"
#define  __M_SRCSNK_CPP
#include "m_srcsnk.h"
#include "scd_wm.h"

//==========================================================================

int MN_SrcSnk::sm_nCFQuals=-1;
CArray<Strng, Strng&> MN_SrcSnk::sm_CFQualsNm;

MN_SrcSnk::MN_SrcSnk (pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
MdlNode(pClass_, TagIn, pAttach, eAttach),
Contents("Content", this, TOA_Embedded),
Flows("Flows", this, TOA_Embedded),
P_RegB("RL_Basic", this, NULL),
T_RegB("RL_Basic", this, NULL),
m_VLE(this, VLEF_TPFlash),
m_Src(eDIO_Src, this, false, true, "Source"), 
m_Snk(eDIO_Snk, this, false, true, "Sink")
  {
  AttachClassInfo(nc_MSrcSnk,NULL);
  //Initialise();
  fConnectionOn   = true;
  fConnectInitiator = false;
  sConnectTag     = "";
  pConnectedTo    = NULL;
  m_iConnectedIO  = -1;
  fCrossConnected = false;
  dHead=0.0;
  PFeed=Std_P;
  SSMode=SSM_Source|SSM_ConstP;
  dP_Out=1.0;
  dQ_Out=1.0;
  dT_Out=1.0;
  dP_Tau=0.0;
  dQ_Tau=0.0;
  dT_Tau=0.0;
  dP_Std_Dev=dP_Std_Dev_C=0.0;
  dQ_Std_Dev=dQ_Std_Dev_C=0.0;
  dT_Std_Dev=dT_Std_Dev_C=0.0;
  BatchAvail=dNAN;
  dQmOut=0.0;
  nQmOut=0;
  dTime=1.0;
  bMakeUpMode=0;
  bFlashIt=0;
  QmRqdMode=SSQmRqdTot;
  m_bKeepTrace=(PrjFileVerNo()>73 ? 0 : 1);

  iRevFlw=SSQmRev_Null;
  //  fOutFlwIsInFlw=false;

  Contents.SetVLE(&m_VLE);
  Contents.SetSQFlags(SQF_Editable|SQF_Source|SQF_CreateOK|SQF_CreateDsp, true);
  Flows.SetModelSelectable(false);

  SetAllowedModes(false, NM_All|SM_Direct|SM_Inline/*|SM_Buffered*/|HM_All);// "Transfer", "Inline", NULL);

  //  Contents.SetSQCreateOK(true);
  //  //Contents.SetSQRemoveOK(true);
  //  Contents.SetSQDispCreate(true);
  //  Contents.SetSQEditable(true);
  //Contents.SetSQDispRemove(true);
  fCopyFlw2Contents=false;
  fGlblConnectsInPlace=false;

  m_iCFWhen=CFWhenOff;
  m_iCFWhat=CFWhatComp|CFWhatTemp|CFWhatPress;
  m_sCFTag="";
  m_fCFRatio=1.0f;
  if (sm_nCFQuals<0)
    {
    sm_nCFQuals=0;
    for (TagObjClass* pQ=TagObjClass::FindGroupId(SpQuality::GroupName); pQ; pQ=pQ->NextClassInGrp())
      if (pQ->Selectable() && pQ->LicenseOK())
        sm_nCFQuals++;

    sm_CFQualsNm.SetSize(sm_nCFQuals);
    int i=0;
    for (TagObjClass* pQ=TagObjClass::FindGroupId(SpQuality::GroupName); pQ; pQ=pQ->NextClassInGrp())
      if (pQ->Selectable() && pQ->LicenseOK())
        {
        sm_CFQualsNm[i]="CF.What.";
        sm_CFQualsNm[i]+=pQ->ClassId();
        i++;
        }
    }

  m_CFQualsOn.SetSize(sm_nCFQuals);
  for (int i=0; i<sm_nCFQuals; i++)
    m_CFQualsOn[i]=1;
  };

// -------------------------------------------------------------------------

MN_SrcSnk::~MN_SrcSnk ()
  {
  };

// -------------------------------------------------------------------------

void MN_SrcSnk::ResetData(flag Complete)
  {
  for (int i=0; i<NoFlwIOs(); i++)
    for (int j=0; j<NIOFBs(i); j++)
      IOFB(i,j)->ResetData(Complete);
  }

// -------------------------------------------------------------------------

flag MN_SrcSnk::DataXchg(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
    case xidSSMode:
      if (DCB.rB)
        {
        SSMode=*DCB.rB;
        if (SSMode & SSM_ConstP)
          m_iCFWhat &= ~CFWhatFlow;
        }
      DCB.B=SSMode;
      return 1;
    case xidQmRqd:
      if (DCB.rD)
        Contents.SetQmRqdTo(GEZ(*DCB.rD), !DCB.ForFiling(), !DCB.ForFiling());
      DCB.D=Contents.QmRqd();
      return 1;
    case xidQvRqd:
      if (DCB.rD)
        Contents.SetQvRqdTo(GEZ(*DCB.rD), !DCB.ForFiling(), !DCB.ForFiling());// CNM true); recovery of QAL
      DCB.D=Contents.QvRqd();
      return 1;
    case xidNQvRqd:
      if (DCB.rD)
        Contents.SetNQvRqdTo(GEZ(*DCB.rD), !DCB.ForFiling(), !DCB.ForFiling());// CNM true); recovery of QAL
      DCB.D=Contents.NQvRqd();
      return 1;
    case xidTRqd:
      if (DCB.rD && Finite(*DCB.rD))
        Contents.SetTRqdTo(Range(CDB.MinT, *DCB.rD, (iVLEOpt==VLEO_SatVap) ? IF97_MaxSatT : CDB.MaxT));
      DCB.D=Contents.TRqd();
      return 1;
    case xidPRqd:
      if (DCB.rD && Finite(*DCB.rD))
        Contents.SetPRqdTo(Range(1.0, *DCB.rD, (iVLEOpt==VLEO_SatVapP) ? IF97_MaxSatP : 1.0e10));
      DCB.D=Contents.PRqd();
      return 1;
    case xidRho:
      DCB.D=Flows.Rho(som_ALL);
      return 1;
    case xidNRho:
      DCB.D=Flows.NRho(som_ALL);
      return 1;
    case xidLevel:
      DCB.D=dNAN;
      return 1;
    case xidPMean:
      if (DCB.rD)
        Flows.SetPress(Max(1.0, *DCB.rD));
      DCB.D=Flows.Press();
      return 1;
    case xidSolFrac:
      DCB.D=Flows.MassFrac(som_Sol);
      return 1;
    case xidLiqFrac:
      DCB.D=Flows.MassFrac(som_Liq);
      return 1;
    case xidVapFrac:
      DCB.D=Flows.MassFrac(som_Gas);//Frac();
      return 1;
    case xidMoleVapFrac:
      DCB.D=Flows.MoleFrac(som_Gas);
      return 1;
    case xidTemp:
      if (DCB.rD)
        Flows.SetTemp(*DCB.rD);
      DCB.D=Flows.Temp();//som_ALL);
      return 1;
    case xidCFTag:
      if (DCB.rpC)
        {
        m_sCFTag = DCB.rpC;
        m_sCFTag.LRTrim();
        if (m_sCFTag.FindI("Self")>=0)
          m_sCFTag="";
        if (m_sCFTag.Find(' ')>=0)
          m_sCFTag=m_sCFTag.Left(m_sCFTag.Find(' '));
        }
      DCB.pC=m_sCFTag() ? m_sCFTag() : DCB.ForView() ? "(Self.Flows)" : "";
      return 1;
    case xidCFWhen:
      {
      if (DCB.rB)
        m_iCFWhen=*DCB.rB;
      DCB.B=m_iCFWhen;
      return 1;
      }
    case xidCFFetchNow:
      {
      if (DCB.rB && (*DCB.rB!=0))
        DoFetch();
      DCB.B=0;
      return 1;
      }
    case xidConnectTag:
      if (DCB.rpC && !SolutionBusy())
        sConnectTag = DCB.rpC;
      DCB.pC=sConnectTag();
      return 1;
    case xidConnectedTag:
      DCB.pC=sConnectedTag();
      return 1;
    }

  if (MdlNode::DataXchg(DCB))
    return 1;
  if (m_VLE.DataXchg(DCB))
    return 1;
  if (DCB.dwUserInfo==1 && m_Src.DataXchg(DCB))
    return 1;
  if (DCB.dwUserInfo==2 && m_Snk.DataXchg(DCB))
    return 1;

  switch (DCB.dwUserInfo)
    {
    case 1 : if (P_RegB.DataXchg(DCB)) return 1;
    case 2 : if (T_RegB.DataXchg(DCB)) return 1;
    }

  return 0;
  }

// -------------------------------------------------------------------------

flag MN_SrcSnk::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=MdlNode::ValidateData(VDB);

  if (sConnectTag.Length()==0)
    fConnectionOn=true;

  dHead=GEZ(dHead);

  if (fGlblConnectsInPlace && !fCrossConnected)
    {
    if (NoProcLnkIOs()>0 && fGlblConnectsInPlace)
      if (IOId_Self(0)==SSIO_Snk)
        SSMode = SSMode &~SSM_Source | SSM_Sink;
      else
        SSMode = SSMode &~SSM_Sink | SSM_Source;

    if (SSMode==SSM_Source || SSMode==SSM_Sink)
      SSMode|=SSM_ConstP;
    }

  if (NetProbalMethod())
    {
    if ((SSMode & (SSM_ConstQm|SSM_ConstQv|SSM_ConstNQv))==0)
      SSMode = (SSMode & ~SSM_ConstP) | SSM_ConstQm;
    if (NoProcLnkIOs()==1)
      {
      if (IOId_Self(0)==SSIO_Snk)
        SSMode = (SSMode & ~SSM_Source) | SSM_Sink; //forced to be a sink
      else if (IOId_Self(0)==SSIO_Src)
        SSMode = (SSMode & ~SSM_Sink) | SSM_Source; //forced to be a source
      }
    }
  else 
    {
    if (NoProcLnkIOs()==1)
      {
      if (IOId_Self(0)==SSIO_Snk)
        {
        SSMode = (SSMode & ~SSM_Source) | SSM_Sink; //forced to be a sink
        SSMode = (SSMode & ~(SSM_ConstQm|SSM_ConstQv|SSM_ConstNQv)) | SSM_ConstP;
        }
      else if (IOId_Self(0)==SSIO_Src)
        {
        SSMode = (SSMode & ~SSM_Sink) | SSM_Source; //forced to be a source
        }
      }
    }

  switch (SSMode)
    {
    case SSM_Source:
    case SSM_Sink:
      LogError(FullObjTag(), 0, "Bad Feed Mode");
      return false;
    case SSM_Source | SSM_ConstQm:
      Contents.SetQMode(SPI_QModeQm);
      if (!Valid(Contents.QmRqd()))
        {
        LogError(FullObjTag(), 0, "Invalid Mass Flow Rate");
        Contents.SetQmRqdTo(0.0, true, true);
        }
      break;
    case SSM_Source | SSM_ConstQv:
      Contents.SetQMode(SPI_QModeQv);
      if (!Valid(Contents.QvRqd()))
        {
        LogError(FullObjTag(), 0, "Invalid Volumetric Flow Rate");
        Contents.SetQvRqdTo(0.0, true, true);
        }
      break;
    case SSM_Source | SSM_ConstNQv:
      Contents.SetQMode(SPI_QModeNQv);
      if (!Valid(Contents.NQvRqd()))
        {
        LogError(FullObjTag(), 0, "Invalid Normal Volumetric Flow Rate");
        Contents.SetNQvRqdTo(0.0, true, true);
        }
      break;
    case SSM_Source | SSM_ConstP:
      if (!Valid(Contents.PRqd()))
        {
        LogError(FullObjTag(), 0, "Invalid Pressure");
        Contents.SetPRqdTo(Std_P);
        }
      break;
    }

  EvalState();

  if (fGlblConnectsInPlace)
    {

    SpModel & CMdl=*Contents.Model();
    // For compatiblity
    if (bFlashIt)
      {
      double Duty=0.0;
      m_VLE.TPFlash(CMdl, TRequired(), PRequired(), VLEF_Null);//NULL, NULL, NULL);
      }
    else
      {
      //VLE.TPFlash(CMdl, TRequired(), PRequired(), VLEF_Null);//NULL, NULL, NULL);
      //double TSat=Contents.SaturationT(TRequired());
      //double PSat=Contents.SaturationP(TRequired());
      double PSat,TSat;
      switch (iVLEOpt)
        {
        case VLEO_SatLiq:
          PSat=Contents.SaturationP(TRequired());
          Contents.SetPRqdTo(PSat);
          m_VLE.SetFlashVapFrac(CMdl, TRequired(), PSat, 0.0, 0);
          break;
        case VLEO_SatVap:
          PSat=Contents.SaturationP(TRequired());
          Contents.SetPRqdTo(PSat);
          m_VLE.SetFlashVapFrac(CMdl, TRequired(), PSat, 1.0, 0);
          break;
        case VLEO_SatLiqP:
          TSat=Contents.SaturationT(PRequired());
          Contents.SetTRqdTo(TSat);
          m_VLE.SetFlashVapFrac(CMdl, TSat, PRequired(), 0.0, 0);
          break;
        case VLEO_SatVapP:
          TSat=Contents.SaturationT(PRequired());
          Contents.SetTRqdTo(TSat);
          m_VLE.SetFlashVapFrac(CMdl, TSat, PRequired(), 1.0, 0);
          break;
        case VLEO_SupVap:
          m_VLE.SetFlashVapFrac(CMdl, TRequired(), PRequired(), 1.0, 0);
          PSat=Contents.SaturationP(TRequired());
          if (PRequired()>PSat)
            {
            Contents.SetPRqdTo(PSat);
            m_VLE.SetFlashVapFrac(CMdl, TRequired(), PRequired(), 1.0, 0);
            }
          break;
        case VLEO_SubLiq:
          m_VLE.SetFlashVapFrac(CMdl, TRequired(), PRequired(), 0.0, 0);
          PSat=Contents.SaturationP(TRequired());
          if (PRequired()<PSat)
            {
            Contents.SetPRqdTo(PSat);
            m_VLE.SetFlashVapFrac(CMdl, TRequired(), PRequired(), 0.0, 0);
            }
          break;
        }

      int xxx=0;
      }
    //else
    //  {
    //  Contents.SetTemp(TRequired());
    //  Contents.SetPress(PRequired());
    //  }

    Contents.SetTempPress(TRequired(), PRequired());

    Contents.ValidateData(VDB);

    PFeed=Contents.Press();

    if ((SSMode & SSM_Source) && !fCrossConnected)
      {
      double Qo=QRequired(Contents.Rho(som_ALL));
      Flows.SetLoMassLimit(SmallMassFrac);
      Flows.QSetM(Contents, som_ALL, Qo, Contents.Press());
      Flows.SetTempPress(TRequired(), PRequired());
      Flows.SetLoMassLimit();
      }
    }


  TaggedObject::ValidateTag(sConnectTag);
  if (!OK)
    { int xxx=0; }
  return OK;
  };

// -------------------------------------------------------------------------

flag MN_SrcSnk::PreStartCheck()
  {
  flag Ok=true;
  if (Contents.Model())
    Ok=Contents.Model()->Class()->LicenseOK();
  if (!Ok)
    LogError(FullObjTag(), 0, "Specie model '%s' not enabled by licensing", Contents.Model()->Class()->ShortDesc());
  return MdlNode::PreStartCheck();
  };

//--------------------------------------------------------------------------

int MN_SrcSnk::ChangeTag(pchar pOldTag, pchar pNewTag)
  {
  if (sConnectTag.XStrICmp(pOldTag)==0)
    sConnectTag=pNewTag;
  const int len = strlen(pOldTag);
  if (m_sCFTag.Len()>=len && _strnicmp(m_sCFTag(), pOldTag, len)==0 && (m_sCFTag.Len()==len || m_sCFTag[len]=='.'))
    {
    Strng Right = m_sCFTag.Mid(len, 256);
    m_sCFTag = pNewTag;
    m_sCFTag += Right();
    }
  return EOCT_DONE;
  };

//--------------------------------------------------------------------------

flag MN_SrcSnk::GetModelAction(MdlActionArray & Acts)
  {
  //MdlAction {MAT_NULL, MAT_State, MAT_Value};

  MdlAction M0(0, MAT_Value, SSMode&SSM_ConstP && fabs(Contents.PRqd()-Std_P)>0.1,
    "Pressure (kPa)", false,
    (Contents.PRqd()-Std_P)*P_RegB.Output(), 0.0, Contents.PRqd()-Std_P);

  MdlAction M1(1, MAT_Value, SSMode&SSM_ConstQm && fabs(Contents.QmRqd())>1.0e-3,
    "Flow (kg/s)", false,
    (Contents.QmRqd())*P_RegB.Output(), 0.0, Contents.QmRqd());

  MdlAction M2(2, MAT_Value, 1,
    "Temperature (%)", false,
    K_2_C(Contents.TRqd())*T_RegB.Output(), 0.0, K_2_C(Contents.TRqd()));

  MdlAction M3(3, MAT_Follow, 1,
    "Find Connected"/*sConnectedTag()*/, false,
    0.0, 0.0, 0.0);

  Acts.SetSize(0);
  Acts.SetAtGrow(0, M0);
  Acts.SetAtGrow(1, M1);
  Acts.SetAtGrow(2, M2);
  if (sConnectedTag.Len()>0)
    Acts.SetAtGrow(3, M3);
  return true;
  };

//--------------------------------------------------------------------------

flag MN_SrcSnk::SetModelAction(MdlAction & Act)
  {
  switch (Act.iIndex)
    {
    case 0:
      P_RegB.SetOutput(Act.dValue/GTZ(Contents.PRqd()-Std_P));
      break;
    case 1:
      P_RegB.SetOutput(Act.dValue/GTZ(Contents.QmRqd()));
      break;
    case 2:
      T_RegB.SetOutput(Act.dValue/GTZ(K_2_C(Contents.TRqd())));
      break;
    case 3:
      if (sConnectedTag.Len()>0)
        {
        //SUB_TAGACTION_FINDANDACCESS_NOERRDLG
        //SUB_TAGACTION_FINDANDACCESS
        //SUB_TAGACTION_FIND
        char* pTxt = new char[sConnectedTag.Len()+1];
        strcpy(pTxt, sConnectedTag());
        ScdMainWnd()->PostMessage(WMU_TAGACTION, SUB_TAGACTION_FINDANDACCESS, (LPARAM)pTxt);
        }
      break;
    }
  return true;
  };

//--------------------------------------------------------------------------

long MN_SrcSnk::NodeFlwTask(NodeFlwTasks Task)
  {
  if (Task==NFT_PBQueryRemove)
    {
    if (fCrossConnected || NoProcLnkIOs()>1)
      return 0;
    if (NoProcLnkIOs()==0 || IO_In(0))
      return 0;

    long nIn=0;
    long nOut=0;
    for (int i=0; i<NoProcessIOs(); i++)
      {
      if (IO_In(i))
        nIn++;
      else if (IO_Out(i))
        nOut++;
      }

    if (nIn>0 || nOut>0)
      return 0;
    else
      return 1;
    }
  else
    return MdlNode::NodeFlwTask(Task);
  }

//--------------------------------------------------------------------------

void MN_SrcSnk::GlobalConnect()
  {
  fGlblConnectsInPlace=true;

  bool AllOK=false;
  Contents.SetSQFlags(SQF_Source, true);
  if (sConnectTag.Length() > 0 && fConnectionOn)
    {
    if (fCrossConnected)
      {
      if (pConnectedTo)
        LogError(FullObjTag(), 0/*LF_Exclamation*/, "Already connected to %s", pConnectedTo->Tag());
      else
        LogError(sConnectTag(), 0/*LF_Exclamation*/, "Incorrectly connected to %s", FullObjTag());
      goto Done;
      }

    if (NoProcLnkIOs() > 1)
      {
      Strng S;
      S.Set("More than one Connection : %s", Nd_Rmt(0)->FullObjTag());
      for (int i=1; i<NoProcLnkIOs(); i++)
        {
        S+=", ";
        S+=Nd_Rmt(i)->FullObjTag();
        }

      LogError(FullObjTag(), 0/*LF_Exclamation*/, S());
      goto Done;
      }

    MdlNode *Root = (MdlNode*)InsertRoot(NULL);
    MN_SrcSnk *pOther  = dynamic_cast<MN_SrcSnk*>(Root->FindObj(sConnectTag()));

    if (pOther==NULL)
      {
      LogError(FullObjTag(), 0/*LF_Exclamation*/, "Connect tag not found %s", sConnectTag());
      goto Done;
      }
    if (pOther==this)
      {
      LogError(FullObjTag(), 0/*LF_Exclamation*/, "Cannot Connect to Self");
      goto Done;
      }
    //KGA 1/12/95 need these lines because the sequence in which XPage connecters are checked does
    // not garentee that pOther has allready been checked...

    if (pOther->NoProcLnkIOs() > 1)
      {
      LogError(FullObjTag(), 0, "%s Too many Connections: ", pOther->Tag());
      goto Done;
      }

    flag WithLnks=(NoProcLnkIOs()==1 && pOther->NoProcLnkIOs()==1);
    if (WithLnks && (pOther->IOId_Self(0)==IOId_Self(0)))
      {//kga both cross page connectors have pipes that are connected as sinks or sources !!!
      LogError(FullObjTag(), 0/*LF_Exclamation*/, "%s Cannot Connect: check pipes(links) IO's, both are src or snk", sConnectTag());
      goto Done;
      }
    if (pOther->ClassId() != ClassId())
      {//Checked that the remote is also a MN_SrcSnk
      LogError(FullObjTag(), 0/*LF_Exclamation*/, "%s Cannot Connect: different models", sConnectTag());
      goto Done;
      }
    if (pOther->fCrossConnected)
      {
      LogError(FullObjTag(), 0/*LF_Exclamation*/, "%s Already connected to %s", pOther->Tag(), pOther->pConnectedTo->Tag());
      goto Done;
      }

    if (!GetActive() || !pOther->GetActive())
      goto Done;

    for (int iO=0; (pOther->IOAreas[iO].Dirn & LIO_Out)==0 ; iO++) {} ;
    for (int iI=0; (IOAreas[iI].Dirn & LIO_In)==0 ; iI++) {} ;
    ASSERT(pOther->IOAreas[iO].pDesc!=NULL);
    ASSERT(IOAreas[iI].pDesc!=NULL);


    flag IAmOut=false;
    if (NoProcLnkIOs()>0 && (IODesc_Self(0)->Dirn & LIO_Out)!=0)
      IAmOut=true;
    else if (pOther->NoProcLnkIOs()>0 && (pOther->IODesc_Self(0)->Dirn & LIO_In)!=0)
      IAmOut=true;
    IOInfoBlk Info;
    if (IAmOut)
      {
      ConnectIOTag(pOther, pOther->IOAreas[iO].pName, this, IOAreas[iI].pName, &Info);
      m_iConnectedIO = Info[1].IOIndex;
      pOther->m_iConnectedIO = Info[0].IOIndex;
      SetIOAutoConnect(m_iConnectedIO, true);
      pOther->SetIOAutoConnect(pOther->m_iConnectedIO, true);
      }
    else
      {
      ConnectIOTag(pOther, pOther->IOAreas[iI].pName, this, IOAreas[iO].pName, &Info);
      m_iConnectedIO = Info[1].IOIndex;
      pOther->m_iConnectedIO = Info[0].IOIndex;
      SetIOAutoConnect(m_iConnectedIO, true);
      pOther->SetIOAutoConnect(pOther->m_iConnectedIO, true);
      }
    m_Datum = pOther->m_Datum;

    fConnectInitiator=true;

    fCrossConnected=1;
    pConnectedTo = pOther;
    sConnectedTag = pOther->FullObjTag();

    pConnectedTo->fCrossConnected=true;
    pConnectedTo->pConnectedTo = this;
    pConnectedTo->sConnectedTag = FullObjTag();
    // Restore Actual Flows - Needed in case of tear
    if (WithLnks)
      IOConduit(m_iConnectedIO)->QCopy(*IOConduit(OtherEnd(m_iConnectedIO)));

    Contents.SetSQFlags(SQF_Source, false);
    AllOK=true;
    }

Done:
  //m_Src.Enabled=true; 
  //m_Snk.Enabled=true; 
  ////if (!fCrossConnected && (NoProcessIOs()>0))
  ////  {
  ////  if (IOId_Self(0)==SSIO_Src)
  ////    m_Src.Enabled=true; 
  ////  if (IOId_Self(0)==SSIO_Snk)
  ////    m_Snk.Enabled=true; 
  ////  }
  MdlNode::GlobalConnect();
  };

// -------------------------------------------------------------------------

void MN_SrcSnk::GlobalDisConnect()
  {
  if (fConnectInitiator)
    {
    DisConnect(m_iConnectedIO);

    fCrossConnected=false;
    pConnectedTo->fCrossConnected=false;
    pConnectedTo->sConnectedTag="";
    pConnectedTo->pConnectedTo=NULL;
    pConnectedTo = NULL;
    sConnectedTag = "";

    m_iConnectedIO = -1;
    fConnectInitiator=false;
    Contents.SetSQFlags(SQF_Source, true);
    }
  fGlblConnectsInPlace=false;
  MdlNode::GlobalDisConnect();
  };

//--------------------------------------------------------------------------

CFlange * MN_SrcSnk::GetFlange(int IOId)
  {
  //switch (IOId)
  //  {
  //  case IOId_Src2Area: return &m_Src.Flange;
  //  case IOId_Snk2Area: return &m_Snk.Flange;
  //  }
  return NULL;
  };

//---------------------------------------------------------------------------

void MN_SrcSnk::PostConnect(int IONo)
  {
  if (!PostConnectDirect(IONo))
    {
    MdlNode::PostConnect(IONo);
    //IOFB(IONo,0)->AssignFlwEqnGroup(NullFlwGroup, NullFlwGroup.Default(), this);
    }
  };

//---------------------------------------------------------------------------

void MN_SrcSnk::PreDisConnect(int IONo)
  {
  if (!PreDisConnectDirect(IONo))
    MdlNode::PreDisConnect(IONo);
  //switch (IOId_Self(IONo))
  //  {
  //  case IOId_Src2Area: m_Src.Enabled=false; break;
  //  case IOId_Snk2Area: m_Snk.Enabled=false; break;
  //  }
  };

//--------------------------------------------------------------------------

void MN_SrcSnk::SetDatums(int Pass, CFlwNodeIndexList & List, int IOIn)
  {
  SetDatums_Node(Pass, List, IOIn, NULL);
  };

//--------------------------------------------------------------------------

flag MN_SrcSnk::Set_Sizes()
  {
  return true;
  };

//--------------------------------------------------------------------------

void MN_SrcSnk::StartSolution()
  {
  }

//--------------------------------------------------------------------------

void MN_SrcSnk::StartStep()
  {
  dTime=ICGetTimeInc();
  }

//---------------------------------------------------------------------------

bool MN_SrcSnk::PropagateNetInfo(CPropagateNetInfoCtrl & Ctrl, long IONo)
  {
  if (!FlwNode::DoPropagateNetInfo(Ctrl, IONo, false))
    return false;

  for (int i=0; i<NoFlwIOs(); i++)
    {
    if (i!=IONo)
      Nd_Rmt(i)->PropagateNetInfo(Ctrl, IOIONo_Rmt(i));
    }

  return true;
  };

//---------------------------------------------------------------------------

void MN_SrcSnk::ConfigureJoins()
  {
  if (fCrossConnected && (NoProcessIOs()==2))
    {
    for (int i=0; i<NoProcessIOs(); i++)
      SetIO_Join(i, 0);
    }
  else
    {
    for (int i=0; i<NoProcessIOs(); i++)
      SetIO_Open(i, 0, false, ESS_Denied);
    
    for (int ii=NoProcessIOs(), jj=0+1; ii<NoFlwIOs(); ii++, jj++)
      SetIO_Direct(ii, jj);
    }
  };

//--------------------------------------------------------------------------

flag MN_SrcSnk::MoveTearFlange(int iTearIO, pFlwNode &pNewTearNd, int &iNewTearIO)
  {
  if (!fCrossConnected)
    return false;
  if (iTearIO>=2)
    return false;
  if (IO_Out(iTearIO))
    {
    // Move to Beginning
    pNewTearNd = this;
    iNewTearIO = OtherEnd(iTearIO);

    // Try Move Further
    pFlwNode pRmtNd=pNewTearNd->Nd_Rmt(iNewTearIO);
    int iRmtIO=pNewTearNd->IOIONo_Rmt(iNewTearIO);
    pFlwNode pRmtTearNd;
    int iRmtTearIO;
    if (pRmtNd->MoveTearFlange(iRmtIO, pRmtTearNd, iRmtTearIO))
      {
      pNewTearNd=pRmtTearNd;
      iNewTearIO=iRmtTearIO;
      }
    return true;
    }
  return false;
  }

//--------------------------------------------------------------------------

void MN_SrcSnk::EvalJoinPressures(long JoinMask)
  {
  if (fCrossConnected && (NoProcLnkIOs()==2))
    {
    int In  = (IO_In(0) ? 0 : OtherEnd(0));
    int Out = OtherEnd(In);
    double Po = IOP_Flng(In);

    Clr_RhoH_Self(In);
    Clr_RhoH_Self(Out);
    Set_IOP_Self(Out, Po);
    Set_IOP_Flng(Out, Po);
    }
  else
    {
    const int nfios=NoProcLnkIOs();
    if (NetProbalMethod())
      {
      if ((SSMode&SSM_Sink)!=0)
        {//probal sink
        if (nfios==1)
          Contents.SetPress(IOP_Flng(0));
        else if (nfios>1)
          {
          const int In  = (IO_In(0) ? 0 : OtherEnd(0));
          Contents.SetPress(IOP_Flng(In));
          }
        }
      else
        {//probal source
        if (Valid(Contents.PRqd()) && ((SSMode&(SSM_ConstP|SSM_ConstQm|SSM_ConstQv|SSM_ConstNQv))!=0))
          Contents.SetPress(PRequired());
        }
      }
    else
      {
      if (Valid(Contents.PRqd()) && ((SSMode&(SSM_ConstP|SSM_ConstQm|SSM_ConstQv|SSM_ConstNQv))!=0))
        Contents.SetPress(PRequired());
      }

    PFeed=Contents.Press();
    for (int i = 0; i<nfios; i++)
      {
      Set_IOP_Self(i,PFeed);
      if (NetProbalMethod())
        {
        Set_IOP_Flng(i,PFeed);
        Clr_RhoH_Self(i);
        }
      else
        Set_IOP_RhoH_Self(i, PFeed, Contents.Rho(som_SL), dHead);
      }
    }
  };

//--------------------------------------------------------------------------

flag MN_SrcSnk::EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo)
  {
  if (m_Src.EvalFlowEquations(Task, pProps, IONo, FE, LnkNo))
    return true;
  if (m_Snk.EvalFlowEquations(Task, pProps, IONo, FE, LnkNo))
    return true;

  FlwBlk *pFB = IOFB(IONo,0);

  if (Task==FET_GetMode)
    {
    if (fCrossConnected)
      {
      if (NoProcLnkIOs()>0)
        {
        pFB->SetQmFree();
        }
      }
    else if (((Valid(Contents.QmRqd()) && ((SSMode&SSM_ConstQm)!=0)) ||
      (Valid(Contents.QvRqd()) && ((SSMode&SSM_ConstQv)!=0)) ||
      (Valid(Contents.NQvRqd()) && ((SSMode&SSM_ConstNQv)!=0))))
      {
      Contents.SetPRqdTo(IOP_Flng(IONo));
      double PFeed=Contents.Press();
      pFB->SetQmReqd((((SSMode&SSM_Source)!=0) ? -1 : 1) * QRequired(Contents.Rho(som_ALL)), true, (float)PFeed);
      }
    else
      pFB->SetQmFree();
    return true;
    }
  else
    {
    flag EqnOK=pFB->EvaluateFlwEqn(Task, pProps, true, false, 1.0, NULL, NULL);
    return EqnOK || pFB->m_fFixed;
    }
  };

//--------------------------------------------------------------------------

void MN_SrcSnk::EvalState()
  {
  };

//--------------------------------------------------------------------------

void MN_SrcSnk::ClosureInfo()
  {
  bool InRange=true;
  for (int i=0; i<NoProcessIOs(); i++)
    InRange = InRange && IOConduit(i)->SMFnsInRange();
  if ((SSMode & SSM_Source) && !fCrossConnected)
    InRange = InRange && Contents.SMFnsInRange();
  InRange = InRange && Flows.SMFnsInRange();
  FlwNode::SetCI(29, !InRange);

  if (fCrossConnected)
    return;

  if (m_Closure.DoFlows())
    {
    CClosureInfo &CI=m_Closure[0];
    if (!fCrossConnected)
      {
      for (int i=0;i<NoProcessIOs(); i++)
        {
        SpConduit &C=*IOConduit(i);
        if (IO_In(i))
          {
          CI.m_HfLoss   += C.totHf();
          CI.m_HsLoss   += C.totHs();
          CI.m_MassLoss += C.QMass();
          }
        else if (IO_Out(i))
          {
          CI.m_HfGain   += C.totHf();
          CI.m_HsGain   += C.totHs();
          CI.m_MassGain += C.QMass();
          }
        }
      }
    }
  };

//--------------------------------------------------------------------------

void MN_SrcSnk::DoEvalProducts(flag Validating, double dTime)
  {
  if (Valid(Contents.TRqd()))
    Contents.SetTempPress(TRequired(), PRequired());

  int i, nOut=0, nIn=0;
  switch (NetMethod())
    {
    case NM_Dynamic:
      {
      for (i=0;i<NoProcLnkIOs(); i++)
        {
        // Output
        if (IO_Out(i) || IO_Zero(i))
          {
          SpConduit & Q = *IOConduit(i);
          if (IOId_Self(i)==SSIO_Src)
            {// Output
            double Qo = IOQm_Out(i);
            if (((SSMode & (SSM_ConstQm|SSM_ConstQv|SSM_ConstNQv))!=0))
              if (Valid(Contents.QmRqd()) || Valid(Contents.QvRqd()) || Valid(Contents.NQvRqd()))
                Qo = Min(Qo, QRequired(Contents.Rho(som_ALL, TRequired(), IOP_Flng(i))));
            if (!Validating && Valid(BatchAvail))
              Qo=Min(Qo, BatchAvail/dTime);
            if (TstGTZ(Qo))
              {
              Q.SetLoMassLimit(SmallMassFrac);
              Q.QSetM(Contents, som_ALL, Qo, IOP_Flng(i));
              Q.SetTempPress(TRequired(), IOP_Flng(i));
              Q.SetLoMassLimit();
              if (nOut==0)
                Flows.QSetF(Q, som_ALL, 1.0, this->IOP_Flng(i));
              else
                Flows.QAddF(Q, som_ALL, 1.0);
              nOut++;
              if (!Validating)
                {
                dQmOut+=Q.QMass();
                nQmOut++;
                }
              }
            else
              {
              if (m_bKeepTrace)
                {
                Q.QSetTraceMass();
                Flows.QSetTraceMass();
                }
              else
                {
                Q.QZero();
                Flows.QZero();
                }
              }
            }
          else
            { // Flow out of input end
            switch (iRevFlw)
              {
              case SSQmRev_InFlw:
                {
                double Qo = IOQm_Out(i);
                if (TstGTZ(Qo))
                  Q.QSetM(Flows, som_ALL, Qo, IOP_Flng(i));
                else
                  {
                  if (m_bKeepTrace)
                    {
                    Q.QSetTraceMass();
                    Flows.QSetTraceMass();
                    }
                  else
                    {
                    Q.QZero();
                    Flows.QZero();
                    }
                  }
                break;
                }
              case SSQmRev_Image:
                {
                double Qo = IOQm_Out(i);
                if (TstGTZ(Qo))
                  {
                  Q.SetLoMassLimit(SmallMassFrac);
                  Q.QSetM(Contents, som_ALL, Qo, IOP_Flng(i));
                  Q.SetLoMassLimit();
                  }
                else
                  {
                  if (m_bKeepTrace)
                    {
                    Q.QSetTraceMass();
                    Flows.QSetTraceMass();
                    }
                  else
                    {
                    Q.QZero();
                    Flows.QZero();
                    }
                  }
                break;
                }
              case SSQmRev_Null:
              default:
                if (m_bKeepTrace)
                  {
                  Q.QSetTraceMass();
                  Flows.QSetTraceMass();
                  }
                else
                  {
                  Q.QZero();
                  Flows.QZero();
                  }
                break;
              }
            }
          }
        else if (IO_In(i)) // Defined Input
          {
          if (nIn==0)
            Flows.QSetF(*IOConduit(i), som_ALL, 1.0, IOP_Flng(i));
          else
            Flows.QAddF(*IOConduit(i), som_ALL, 1.0);
          nIn++;
          }
        }

      break;
      }
    case NM_Probal:
      {
      for (i=0;i<NoProcLnkIOs(); i++)
        {
        if (IO_Out(i) || IO_Zero(i))
          {
          SpConduit & Q = *IOConduit(i);
          double Qo=QRequired(Contents.Rho(som_ALL));
          if (bMakeUpMode && IOFlange(i)->IsMakeUpReqd())
            Qo=Max(Qo, IOFlange(i)->MakeUpReqd());

          Qo=GEZ(Qo);
          if (TstGTZ(Qo))
            {
            const bool UseOldCode = true;
            if (UseOldCode)
              {
              Q.QZero(); //kga 14/1/2003 : first set to zero to ensure "old" qualities are deleted!
              }
            else
              {
              //Q.QZero(); //kga 14/1/2003 : first set to zero to ensure "old" qualities are deleted!
              //kga: 16/7/04 Above statement is wrong we only "sometimes" want to delete qualities depending on what is happening (ie cro
              if (fCrossConnected)
                {//delete any qualities that may have been created when acting as feeder!!!
                Q.QZero(); //set to zero to ensure "old" qualities are deleted!
                /*for (int q=0; q<Q.nQuals; q++)
                if (Q.Quals[q].m_pQ)
                {
                Q.SetQualityPresent(q, false);
                }*/
                }
              }

            Q.SetLoMassLimit(SmallMassFrac);
            Q.QSetM(Contents, som_ALL, Qo, IOP_Self(i));
            Q.SetTempPress(TRequired(), IOP_Self(i));
            Q.SetLoMassLimit();
            if (nOut==0)
              Flows.QSetF(Q, som_ALL, 1.0, IOP_Self(i));
            else
              Flows.QAddF(Q, som_ALL, 1.0);
            nOut++;
            if (!Validating)
              {
              dQmOut+=Q.QMass();
              nQmOut++;
              }
            }
          else
            {
            Q.QZero();
            Flows.QZero();
            }
          }
        else if (IO_In(i)) // Defined Input
          {
          if (bMakeUpMode)
            {
            rSpConduit Q = *IOConduit(i);
            double Qo=QRequired(Contents.Rho(som_ALL));
            IOFlange(i)->SetMakeUpReqd(Qo);
            }
          if (nIn==0)
            Flows.QSetF(*IOConduit(i), som_ALL, 1.0, IOP_Self(i));
          else
            Flows.QAddF(*IOConduit(i), som_ALL, 1.0);
          nIn++;
          }
        }
      break;
      }
    };

  if (1)//NoDirectIOs()>0)
    {
    int IOOut=IOWithId_Self(SSIO_Src);
    if (IOOut>=0)
      m_Src.SetF(*IOConduit(IOOut));

    int IOIn=IOWithId_Self(SSIO_Snk);
    if (IOIn>=0)
      m_Snk.SetF(*IOConduit(IOIn));
    }

  // This piece of code will overwrite the makeup image if acting as a sink
  if (0 && (nIn>0) && (SSMode & SSM_Sink))
    {
    // A Single line wont compile in Release mode
    double Q=Flows.QMass();
    double P=IOP_Self(0);
    Contents.SetM(Flows, som_ALL, Q, P);
    }
  }

// --------------------------------------------------------------------------

void MN_SrcSnk::EvalProductsInit(EvalProductsInitTasks Task)
  {
  if (NoProcLnkIOs()>=2 && (fCrossConnected || (IO_In(0)==IO_Out(1))))
    {
    }
  else
    {
    DoEvalProducts(false, ICGetTimeInc());
    for (int i=0;i<NoProcLnkIOs(); i++)
      if (IO_Out(i) || IO_Zero(i))
        EvalProductsInit_Source(Task, i, IOConduit(i)->QMass(som_SL), IOConduit(i)->QMass(som_Gas));
    }
  }

//--------------------------------------------------------------------------

void MN_SrcSnk::EvalProducts(CNodeEvalIndex & NEI)
  {
  if (NoProcLnkIOs()>=2 && (fCrossConnected || (IO_In(0)==IO_Out(1))))
    {
    // if flowest is zero default to forward 0->1
    int In  = TwoIOInIndex();
    int Out = OtherEnd(In);
    rSpConduit Fi=*IOConduit(In);
    rSpConduit Fo=*IOConduit(Out);

    Fo.QCopy(Fi);
    Flows.QCopy(Fi);

    double QmIn=Fi.QMass();
    double QmOut=Fo.QMass();

    SetIOQm_In(In, Fi.QMass());
    SetIOQm_Out(Out, Fo.QMass());

    if (fCopyFlw2Contents)
      {
      Contents.SetF(Flows, som_ALL, 1.0, IOP_Flng(0));
      Contents.SetTempPress(Flows.Temp(), IOP_Self(0));

      Contents.SetQmRqdTo(Flows.QMass(), true, true);
      Contents.SetTRqdTo(Flows.Temp());
      Contents.SetPRqdTo(Flows.Press());

      //    fCopyFlw2Contents;
      }

#if dbgDumpSpcInfo
    //if (Forcedbg)
    //  dbgSpFlows("XPG ", Tag(), IOFlange(Out)->Q, 1);
#endif

    }
  else
    {
    if (ICStepStart() || ICStepReStart())
      {
      dQmOut=0.0;
      nQmOut=0;
      }

    DoEvalProducts(false, ICGetTimeInc());
    }
  }

//--------------------------------------------------------------------------

void MN_SrcSnk::EvalDerivs(CNodeEvalIndex & NEI)
  {
  };

//--------------------------------------------------------------------------

void MN_SrcSnk::EvalDiscrete()
  {
  if (1)
    {
    //kga 14/1/2003 :
    //if Contents in feed has quality SzData present, and the feeder is now NOT connected and NOT using Fetch,
    //then the presence of the "old" SzData is NOT deleted. So as a crude fix, lets delete it here now!
    if (!fCrossConnected && NoProcLnkIOs()>0)// && IO_Out(0))
      {
      SpModel * pMdl = Contents.Model();
      for (int q=0; q<pMdl->nQuals; q++)
        {
        if (pMdl->Quals[q].m_iAct!=SQA_Create &&
          pMdl->Quals[q].Exists()/* &&
                                 strcmp(pMdl->Quals[q].pQ->Class()->ClassName(), "SQSzDist1")==0*/)
          {
          pMdl->QualityRemove(q);
          }
        }
      }
    }

  switch (m_iCFWhen)
    {
    case CFWhenOnceOff:
      DoFetch();
      m_iCFWhen=CFWhenOff;
      break;
    case CFWhenContinuous:
      DoFetch();
      break;
    }

  P_RegB.EvalDiscrete(this);
  T_RegB.EvalDiscrete(this);

  dP_Out += OutputChg(P_RegB.Output(), dP_Out, dP_Tau, dTime);
  dQ_Out += OutputChg(P_RegB.Output(), dQ_Out, dQ_Tau, dTime);
  dT_Out += OutputChg(T_RegB.Output(), dT_Out, dT_Tau, dTime);

  dP_Std_Dev_C = TstNZ(dP_Std_Dev) ? Range(-1.0,dP_Std_Dev*P_Noise.GetVal(), 1.0) : 0.0;
  dQ_Std_Dev_C = TstNZ(dQ_Std_Dev) ? Range(-1.0,dQ_Std_Dev*Q_Noise.GetVal(), 1.0) : 0.0;
  dT_Std_Dev_C = TstNZ(dT_Std_Dev) ? Range(-1.0,dT_Std_Dev*T_Noise.GetVal(), 1.0) : 0.0;

  if (Valid(BatchAvail) && (nQmOut>0))
    BatchAvail = Max(0.0, BatchAvail-(ICGetTimeInc() * dQmOut/nQmOut));

  if (!fCrossConnected && (Valid(Contents.QmRqd()) && (SSMode&SSM_ConstQm) ||
    Valid(Contents.QvRqd()) && (SSMode&SSM_ConstQv) ||
    Valid(Contents.NQvRqd()) && (SSMode&SSM_ConstNQv)))
    {
    if (NoProcLnkIOs()>0)// && IO_Out(0))
      {
      if (!NetProbalMethod())
        {
        const double P_ = IOP_Flng(0);
        Contents.SetPRqdTo(P_);
        Contents.SetTempPress(Contents.Temp(), P_);
        }
      }
    }

  const double P = Contents.Press();
  if (fCrossConnected || InRange(CDB.MinP, P, CDB.MaxP))
    ClrCI(2);
  else if (InRange(CDB.MinP*0.95, P, CDB.MaxP*1.3))
    SetCI(2, "N\tRequired pressure %.1fkPa out of project range (%g to %g)", P, CDB.MinP, CDB.MaxP);
  else
    SetCI(2, "W\tRequired pressure %.1fkPa out of project range (%g to %g)", P, CDB.MinP, CDB.MaxP);
  }

// --------------------------------------------------------------------------

//void MN_SrcSnk::EvalIOPAverage(int AveIter)
//  {
//  // Same as Set
//  for (int i=0; i<NoProcLnkIOs(); i++)
//    Set_IOP_Ave_Self(i, IOP_Self(i));
//  };

//---------------------------------------------------------------------------

//flag MN_SrcSnk::EvalPressureSens()
//  {
//  return true;
//  };

//--------------------------------------------------------------------------

flag MN_SrcSnk::InitialiseSolution()
  {
  MdlNode::InitialiseSolution();
  return 1;
  };

//--------------------------------------------------------------------------

flag MN_SrcSnk::TerminateSolution()
  {

  return 1;
  };

//--------------------------------------------------------------------------

void MN_SrcSnk::DoFetch()
  {
  SpContainer * pCn=NULL;
  SpConduit * pCd=NULL;
  if (m_sCFTag())
    {
    MdlNode *pRoot = (MdlNode*)InsertRoot(NULL);
    //pMdl = dynamic_cast<SpModel*>(pRoot->FindObjTag(m_sCFTag(), true, ));
    char TheTag[1024];
    strcpy(TheTag, m_sCFTag());
    int ObjTagLen=0;
    TaggedObject * p=pRoot->FindObjTag(TheTag, true, ObjTagLen, 0);
    if (p)
      {
      pCn=dynamic_cast<SpContainer*>(p);
      pCd=dynamic_cast<SpConduit*>(p);
      if (pCd==NULL && pCd==NULL)
        {
        pTaggedObject p1=p->pAttachments;
        while (p1 && (_stricmp(p1->Tag(), &TheTag[ObjTagLen+1])!=0))
          {
          p1=p1->pNxtAttachment;
          }
        if (p1)
          p=p1;
        pCn=dynamic_cast<SpContainer*>(p);
        pCd=dynamic_cast<SpConduit*>(p);
        }
      }
    }
  else
    pCd=&Flows;

  if (pCd)
    {
    if (m_iCFWhat&CFWhatComp)
      {
      for (int i=0; i<sm_nCFQuals; i++)
        pCd->SetQualityHoldCopy(i, !m_CFQualsOn[i]);
      Contents.Copy(*pCd);
      for (int i=0; i<sm_nCFQuals; i++)
        pCd->SetQualityHoldCopy(i, false);
      Contents.MArray().Normalise();
      Contents.Model()->SetMassScale(1);
      }
    if (m_iCFWhat&CFWhatTemp)
      Contents.m_T_Rqd=pCd->Temp();
    if (m_iCFWhat&CFWhatPress)
      Contents.SetPRqdTo(pCd->Press());
    if (m_iCFWhat&CFWhatFlow)
      {
      switch (Contents.QMode())
        {
        case SPI_QModeQm:  Contents.SetQmRqdTo(pCd->QMass()*m_fCFRatio, true, false); break;
        case SPI_QModeQv:  Contents.SetQvRqdTo(pCd->QVolume()*m_fCFRatio, true, false); break;
        case SPI_QModeNQv: Contents.SetNQvRqdTo(pCd->QNVolume()*m_fCFRatio, true, false); break;
        }
      }
    ClrCI(1);
    }
  else if (pCn)
    {
    if (m_iCFWhat&CFWhatComp)
      {
      for (int i=0; i<sm_nCFQuals; i++)
        pCd->SetQualityHoldCopy(i, !m_CFQualsOn[i]);
      Contents.Copy(*pCn, pCn->Press());
      for (int i=0; i<sm_nCFQuals; i++)
        pCd->SetQualityHoldCopy(i, false);
      }
    if (m_iCFWhat&CFWhatTemp)
      Contents.m_T_Rqd=pCn->Temp();
    if (m_iCFWhat&CFWhatPress)
      Contents.SetPRqdTo(pCn->Press());
    ClrCI(1);
    }
  else
    SetCI(1);
  }

//--------------------------------------------------------------------------

double MN_SrcSnk::TRequired()
  {
  if (Valid(Contents.TRqd()))
    {
    double dO = OutputChg(T_RegB.Output(), dT_Out, dT_Tau, dTime);
    double T = NetProbalMethod() ? Contents.TRqd() : (Contents.TRqd()-ZeroCinK) * (1.0+dT_Std_Dev_C) * (dT_Out+dO) + ZeroCinK;
    Contents.SetTemp(T);
    return T;
    }
  else
    return Contents.Temp();
  };

//--------------------------------------------------------------------------

double MN_SrcSnk::PRequired()
  {
  if (Valid(Contents.PRqd()))
    {
    double dO = OutputChg(P_RegB.Output(), dP_Out, dP_Tau, dTime);
    double P = NetProbalMethod() ? Contents.PRqd() : (Contents.PRqd()-Std_P) * (1.0+dP_Std_Dev_C) * (dP_Out+dO) + Std_P;
    Contents.SetPress(P);
    return P;
    }
  else
    return Contents.Press();
  }

//--------------------------------------------------------------------------

double MN_SrcSnk::QRequired(double Rho)
  {
  double Qr= ((SSMode & SSM_ConstQm)!=0) ? Contents.QmRqd() : Contents.QvRqd() * Rho;
  if (!Valid(Qr))
    return 0;
  if (!NetProbalMethod())
    {
    double dO = OutputChg(P_RegB.Output(), dQ_Out, dQ_Tau, dTime);
    Qr = (Qr) * (1.0+dQ_Std_Dev_C) * (dQ_Out+dO);
    }
  switch (QmRqdMode)
    {
    case SSQmRqdTot: break;
    case SSQmRqdSol: Qr /= Max(0.01, Contents.MassFrac(som_Sol)); break;
    case SSQmRqdLiq: Qr /= Max(0.01, Contents.MassFrac(som_Liq)); break;
    case SSQmRqdVap: Qr /= Max(0.01, Contents.MassFrac(som_Gas)); break;
    }
  return Qr;
  };

//--------------------------------------------------------------------------

void MN_SrcSnk::EvalPBMakeUpReqd(long JoinMask)
  {
  if (fCrossConnected && NoProcLnkIOs()==2)
    MakeUpLinkTransferReqd();
  else if (NoProcLnkIOs()==1)
    {
    //double A=dNAN;
    double R=dNAN;
    if (bMakeUpMode && IO_In(0))
      {
      R=QRequired(Contents.Rho(som_ALL));
      if (Valid(R))
        IOFlange(0)->SetMakeUpReqd(R);
      }
    }
  };

//--------------------------------------------------------------------------

void MN_SrcSnk::EvalPBMakeUpAvail(long JoinMask)
  {
  if (fCrossConnected && NoProcLnkIOs()==2)
    MakeUpLinkTransferAvail();
  else if (NoProcLnkIOs()==1)
    {
    double A=dNAN;
    //double R=dNAN;
    if (bMakeUpMode)
      if (IO_Out(0) || IO_Zero(0))
        A=1.0;
    if (Valid(A))
      IOFlange(0)->SetMakeUpAvail(A);
    else
      IOFlange(0)->ClrMakeUpAvail();
    //if (Valid(R))
    //  IOFlange(0)->SetMakeUpReqd(R);
    //else
    //  IOFlange(0)->ClrMakeUpReqd();
    }
  };

//--------------------------------------------------------------------------

int MN_SrcSnk::GetConnectionTags(Strng_List & TagLst)
  {
  //TagLst.Clear();
  if (fCrossConnected)
    {
    Strng *p=new Strng(sConnectedTag());
    p->SetIndex(1);
    TagLst.Append(p);
    for (int i=0; i<NoProcLnkIOs(); i++)
      {
      char * pTag=Nd_Rmt(i)->FullObjTag();
      if (sConnectedTag.XStrICmp(pTag)!=0)
        {
        Strng *p=new Strng(Nd_Rmt(i)->FullObjTag());
        p->SetIndex(0);
        TagLst.Append(p);
        }
      }
    return TagLst.Length();
    }
  return FlwNode::GetConnectionTags(TagLst);
  };

//--------------------------------------------------------------------------

#if DOPBSMOOTHING
void MN_SrcSnk::EvalMsAGDatas()
  {
  if (NoProcLnkIOs()==1 && !fCrossConnected)
    {
    MsJoins.SetSize(1);
    if (IO_In(0))
      {
      MsJoins[0].m_ConstBG.SetAll(0);
      MsJoins[0].m_ConstAG.Copy(IOConduit(0)->MArray(), -1);
      }
    else if (IO_Out(0))
      {
      MsJoins[0].m_ConstBG.Copy(IOConduit(0)->MArray(), 1);
      MsJoins[0].m_ConstAG.SetAll(0);
      }

    //    SpMArray * AMI[1]={IO_Out(0) ? &M : NULL};
    //    SpMArray * AMO[1]={IO_In(0) ? &M : NULL};
    MdlNode::EvalMsAGDatas();
    }
  else
    MdlNode::EvalMsAGDatas();
  };
#endif

//--------------------------------------------------------------------------

dword  MN_SrcSnk::ModelStatus()
  {
  dword Status=MdlNode::ModelStatus();
  if (NoProcLnkIOs()>0)
    {
    int HasFlw=0;
    double TFlw=0.0;
    for (int i=0; i<NoProcLnkIOs(); i++)
      {
      TFlw+=IOQm_In(i);
      if (IOConduit(i)->QMass()>UsableMass)
        HasFlw=1;
      }
    if (GetStatusAsStopped())
      {
      if (!fCrossConnected && (NoProcLnkIOs()==1))
        {
        if (IOId_Self(0)==0)
          Status |= FNS_IsSnk;
        else 
          Status |= FNS_IsSrc;
        }
      else
        {
        Status |= FNS_UNoFlw;
        //Status |= (HasFlw ? FNS_UFlw : FNS_UNoFlw);
        }
      }
    else
      {
      Status |= (IOConduit(0)->QMass()>UsableMass ? FNS_UFlw : FNS_UNoFlw);

      int HasFlw=0;
      double TFlw=0.0;
      for (int i=0; i<NoProcLnkIOs(); i++)
        {
        TFlw+=IOQm_In(i);
        if (IOConduit(i)->QMass()>UsableMass)
          HasFlw=1;
        }
      Status |= (HasFlw ? FNS_UFlw : FNS_UNoFlw);
      if (NoProcLnkIOs()==1)
        {
        if (TFlw>1.0e-6)
          Status |= FNS_IsSnk;
        else if (TFlw<-1.0e-6)
          Status |= FNS_IsSrc;
        }
      }
    }

  return Status;
  };

//--------------------------------------------------------------------------

flag MN_SrcSnk::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case 1: pS="E\tFetch tag not found (valid examples: P_1.Qi or X_1.QFeed)"; return 1;
    case 2: pS="W\tRequired pressure out of project range"; return 1;
    default:
      return MdlNode::CIStrng(No, pS);
    }
  };

//==========================================================================

