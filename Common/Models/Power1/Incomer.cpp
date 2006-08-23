//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#define  __INCOMER_CPP

#include "stdafx.h"
#include "sc_defs.h"
#include "pgm_e.h"
#include "incomer.h"

#include "scd_wm.h"


//==========================================================================
//
//
//
//==========================================================================

//XID xxidConnectTag    = MdlBsXID(4000);
//XID xxidConnectedTag  = MdlBsXID(4001);
XID xidTSCfg   = PwrXID(1000);
XID xidOn      = PwrXID(1001);

//==========================================================================
//
//
//
//==========================================================================

static IOAreaRec IncomerIOAreaList[] =
  {{"Input",         "In" , ElecIOId(0), LIO_In0 , nc_ELnk, 0,1, 0, 0.0, {0,0,0,0}, NULL, NULL, "PwrIn"},
   {"Output",        "Out", ElecIOId(1), LIO_Out0, nc_ELnk, 0,1, 0, 0.0, {0,0,0,0}, NULL, NULL, "PwrOut"},
   {NULL}};

static double Drw_Incomer[] =
       {
       DD_Poly, -2, 3, 0, 1, 2, 3,
       DD_Poly, -2, 1, 0,-1, 2, 1,
       DD_Poly, -2,-1, 0,-3, 2,-1,
       DD_End
       };

IMPLEMENT_MODELUNIT(CIncomer, "E_Incomer", "", Drw_Incomer, "Electrical", "In", TOC_ALL|TOC_GRP_POWERDIST|TOC_POWERDIST,
                   "Electrical:Incomer",
                   "Incomer.")

CIncomer::CIncomer(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  FlwNode(pClass_, TagIn, pAttach, eAttach),
  m_tsPwrIn(this, "PwrIn", eCT_3Ph4W),
  m_tsPwrOut(this, "PwrOut", eCT_3Ph4W)
  {

  AttachClassInfo(nc_Elec,NULL);
  //Initialise();
  fConnectionOn     = true;
  fConnectInitiator = false;
  sConnectTag       = "";
  pConnectedTo      = NULL;
  iConnectedIO      = 255;
  fCrossConnected   = false;

  m_dXfrPower       = 0;
  m_dXfrPwrFct      = 0;

  fGlblConnectsInPlace=false;

  AttachClassInfo(nc_Elec, IncomerIOAreaList);
  m_bOn = 1;
  m_bPresent = 0;
  //m_nType=eCT_3Ph4W;
  m_dVolts=380;

 }

//--------------------------------------------------------------------------

CIncomer::~CIncomer()
  {
  }

//--------------------------------------------------------------------------

void CIncomer::ResetData(flag Complete)
  {
  }

//--------------------------------------------------------------------------

static DDBValueLst DDB2[]={
  {False, "-"},
  {True,  "Yes"},
  {0}};

void CIncomer::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this, "CIncomer", NULL, DDB_NoPage);
  DDB.Text("");
  DDB.Long       ("TermCfg",    "", DC_,   "",  xidTSCfg,   this, isParm, DDBCTTypes);
  DDB.Double     ("NomVolts",   "", DC_V,  "V", &m_dVolts,  this, isParm);
  DDB.CheckBoxBtn("On",         "", DC_,   "",  xidOn,      this, isParm);

  DDB.Text  ("");
  DDB.CheckBoxBtn("Present",     "", DC_,   "", &m_bPresent,      this, isResult|InitHidden);

  DDB.Visibility();
  DDB.Text  ("");
  DDB.Text   ("Cross Page Connection");
  DDB.CheckBoxBtn("",       "ConnectionOn", DC_, "", &fConnectionOn,   this, sConnectTag.Length()>0 ? isParmStopped : isParmConstruct);
  DDB.String ("Connection", "Conn_To",      DC_, "", xidConnectTag,    this, 
    (fConnectInitiator && pConnectedTo || !pConnectedTo) && fConnectionOn ? isParmStopped|isTag : isTag);
  DDB.String ("",           "Connected_To", DC_, "", xidConnectedTag,  this, DDEF_WRITEPROTECT|isTag);
  DDB.Bool   ("Connected",  "",             DC_, "", &fCrossConnected, this, DDEF_WRITEPROTECT, DDB2);
  DDB.Text("");

  DDB.Double("Power",     "", DC_Pwr,     "kW",   &m_dXfrPower   ,          this, isParm);
  DDB.Double("PwrFct",    "", DC_,        "",     &m_dXfrPwrFct  ,          this, isParm);
  DDB.Text("");

  m_tsPwrIn.BuildDataDefn(DDB, 1, true);
  m_tsPwrOut.BuildDataDefn(DDB, 2, true);

  DDB.EndStruct();
  }

//--------------------------------------------------------------------------

flag CIncomer::DataXchg(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
    case xidConnectTag:
      if (DCB.rpC && !SolutionBusy())
        sConnectTag = DCB.rpC;
      DCB.pC=sConnectTag();
      return 1;
    case xidConnectedTag:
      DCB.pC=sConnectedTag();
      return 1;
    case xidTSCfg:
      if (DCB.rL)
        {
        m_tsPwrIn.SetConfiguration((eTSConfigurations)*DCB.rL);
        m_tsPwrOut.SetConfiguration((eTSConfigurations)*DCB.rL);
        }
      DCB.L=m_tsPwrIn.Configuration();
      return 1;
    case xidOn:
      if (DCB.rB)
        m_bOn=*DCB.rB;
      DCB.B=m_bOn;
      return 1;
    }

  if (FlwNode::DataXchg(DCB))
    return 1;
  if (m_tsPwrIn.DataXchg(DCB, 1))
    return 1;
  if (m_tsPwrOut.DataXchg(DCB, 2))
    return 1;
  return False;
  }

//--------------------------------------------------------------------------

flag CIncomer::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=FlwNode::ValidateData(VDB);

  if (sConnectTag.Length()==0)
    fConnectionOn=true;

  if (fGlblConnectsInPlace && !fCrossConnected)
    {
    }

  EvalState();

  TaggedObject::ValidateTag(sConnectTag);
  if (!OK)
	  { int xxx=0; }
  return OK;
  }

//--------------------------------------------------------------------------

int CIncomer::ChangeTag(pchar pOldTag, pchar pNewTag)
  {
  if (sConnectTag.XStrICmp(pOldTag)==0)
    sConnectTag=pNewTag;
  const int len = strlen(pOldTag);
  //if (m_sCFTag.Len()>=len && _strnicmp(m_sCFTag(), pOldTag, len)==0 && (m_sCFTag.Len()==len || m_sCFTag[len]=='.'))
  //  {
  //  Strng Right = m_sCFTag.Mid(len, 256);
  //  m_sCFTag = pNewTag;
  //  m_sCFTag += Right();
  //  }
  return EOCT_DONE;
  };

//--------------------------------------------------------------------------

long CIncomer::NodeFlwTask(NodeFlwTasks Task)
  {
  if (Task==NFT_PBQueryRemove)
    {
    if (fCrossConnected || NoEIOs()>1)
      return 0;
    if (NoEIOs()==0 || EIO_In(0))
      return 0;
    
    long nIn=0;
    long nOut=0;
    for (int i=0; i<NoEIOs(); i++)
      {
      if (EIO_In(i))
        nIn++;
      else if (EIO_Out(i))
        nOut++;
      }
        
    if (nIn>0 || nOut>0)
      return 0;
    else
      return 1;
    }
  else
    return FlwNode::NodeFlwTask(Task);
  }

//--------------------------------------------------------------------------

void CIncomer::GlobalConnect()
  {
  FlwNode::GlobalConnect();

  fGlblConnectsInPlace=true;

  //Contents.SetSQFlags(SQF_Source, true);
  if (sConnectTag.Length() > 0 && fConnectionOn)
    {
//    _asm int 3;

    if (fCrossConnected)
      {
      if (pConnectedTo)
        LogError(FullObjTag(), 0, "Already connected to %s", pConnectedTo->Tag());
      else
        LogError(sConnectTag(), 0, "Incorrectly connected to %s", FullObjTag());
      return;
      }

    if (NoEIOs() > 1)
      {
      Strng S;
      S.Set("More than one Connection : %s", Nd_Rmt(0)->FullObjTag());
      for (int i=EIO1(); i<EION(); i++)
        {
        S+=", ";
        S+=Nd_Rmt(i)->FullObjTag();
        }

      LogError(FullObjTag(), 0, S());
      return;
      }

    MdlNode *Root = (MdlNode*)InsertRoot(NULL);
    CTNode *pTmp     = Root->FindObj(sConnectTag());

    if (pTmp==NULL)
      {
      LogError(FullObjTag(), 0, "Connect tag not found %s", sConnectTag());
      return;
      }
    pCIncomer pOther  = dynamic_cast<CIncomer*>(pTmp);

    if (pOther==NULL)
      {
      LogError(FullObjTag(), 0, "Connect tag not an Incomer %s", sConnectTag());
      return;
      }
    if (pOther==this)
      {
      LogError(FullObjTag(), 0, "Cannot Connect to Self");
      return;
      }
    //KGA 1/12/95 need these lines because the sequence in which XPage connecters are checked does
    // not garentee that pOther has allready been checked...

    if (pOther->NoEIOs() > 1)
      {
      LogError(FullObjTag(), 0, "%s Too many Connections: ", pOther->Tag());
      return;
      }

    flag WithLnks=(NoEIOs()==1 && pOther->NoEIOs()==1);
    if (WithLnks && (pOther->EIOId_Self(0)==EIOId_Self(0)))
      {//kga both cross page connectors have pipes that are connected as sinks or sources !!!
      LogError(FullObjTag(), 0, "%s Cannot Connect: check pipes(links) IO's, both are src or snk", sConnectTag());
      return;
      }
    if (pOther->ClassId() != ClassId())
      {//Checked that the remote is also a CIncomer
      LogError(FullObjTag(), 0, "%s Cannot Connect: different models", sConnectTag());
      return;
      }
    if (pOther->fCrossConnected)
      {
      LogError(FullObjTag(), 0, "%s Already connected to %s", pOther->Tag(), pOther->pConnectedTo->Tag());
      return;
      }

    if (!GetActive() || !pOther->GetActive())
      return;

    for (int iO=0; (pOther->IOAreas[iO].Dirn & LIO_Out)==0 ; iO++) {} ;
    for (int iI=0; (IOAreas[iI].Dirn & LIO_In)==0 ; iI++) {} ;
    ASSERT(pOther->IOAreas[iO].pDesc!=NULL);
    ASSERT(IOAreas[iI].pDesc!=NULL);


    m_bIAmOut=false;
    if (NoEIOs()>0 && (IODesc_Self(0)->Dirn & LIO_Out)!=0)
      m_bIAmOut=true;
    else if (pOther->NoFlwIOs()>0 && (pOther->IODesc_Self(0)->Dirn & LIO_In)!=0)
      m_bIAmOut=true;

    IOInfoBlk Info;
    if (m_bIAmOut)
      {
      ConnectIOTag(pOther, pOther->IOAreas[iO].pName, this, IOAreas[iI].pName, &Info);
      iConnectedIO = Info[1].IOIndex;
      pOther->iConnectedIO = Info[0].IOIndex;
      SetIOAutoConnect(iConnectedIO, true);
      pOther->SetIOAutoConnect(pOther->iConnectedIO, true);
      }
    else
      {
      ConnectIOTag(pOther, pOther->IOAreas[iI].pName, this, IOAreas[iO].pName, &Info);
      iConnectedIO = Info[1].IOIndex;
      pOther->iConnectedIO = Info[0].IOIndex;
      SetIOAutoConnect(iConnectedIO, true);
      pOther->SetIOAutoConnect(pOther->iConnectedIO, true);
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
    //if (WithLnks)
    //  EIO_Conduit(iConnectedIO).SetVolts(EIO_Conduit(OtherEnd(iConnectedIO)));
    }
  };

// -------------------------------------------------------------------------

void CIncomer::GlobalDisConnect()
  {
  if (fConnectInitiator)
    {
    m_Electrics.RemoveAllComponents();
    //m_tsPwrIn.RemoveAllComponents();
    //m_tsPwrOut.RemoveAllComponents();

    DisConnect(iConnectedIO);

    fCrossConnected=false;
    pConnectedTo->fCrossConnected=false;
    pConnectedTo->sConnectedTag="";
    pConnectedTo->pConnectedTo=NULL;
    pConnectedTo = NULL;
    sConnectedTag = "";

    iConnectedIO = 255;
    fConnectInitiator=false;
    //Contents.SetSQFlags(SQF_Source, true);
    }
  fGlblConnectsInPlace=false;

  FlwNode::GlobalDisConnect();
  };

//---------------------------------------------------------------------------

void CIncomer::PostConnect(int IONo)
  {
  FlwNode::PostConnect(IONo);
  };

//---------------------------------------------------------------------------

void CIncomer::PreDisConnect(int IONo)
  {
  FlwNode::PreDisConnect(IONo);
  };

//--------------------------------------------------------------------------

void CIncomer::CollectElectrics(CNodeElectricsArray & TSCA)
  { 
  FlwNode::CollectElectrics(TSCA);

  m_Electrics.AddTermStrip(&m_tsPwrIn);
  m_Electrics.AddTermStrip(&m_tsPwrOut);

  }

//--------------------------------------------------------------------------

void CIncomer::ConnectElectrics()
  { 
  FlwNode::ConnectElectrics();

  if (m_bOn)
    {
    if (fCrossConnected)
      {
      for (int i=0; i<m_tsPwrIn.TerminalCount(); i++)
        m_Electrics.SetImpedance(0, m_tsPwrIn.Terminal(i), m_tsPwrOut.Terminal(i), 0, 0);

      if (fConnectInitiator)
        {         
        // will always be 2 connects - set up in GlobalConnect 
        if (m_bIAmOut)
          {
          for (int i=0; i<m_tsPwrIn.TerminalCount(); i++)
            m_Electrics.SetImpedance(0, pConnectedTo->m_tsPwrOut.Terminal(i), m_tsPwrIn.Terminal(i), 0, 0);
          }
        else
          {
          for (int i=0; i<m_tsPwrIn.TerminalCount(); i++)
            m_Electrics.SetImpedance(0, m_tsPwrOut.Terminal(i), pConnectedTo->m_tsPwrIn.Terminal(i), 0, 0);
          }
        }
      }
    else
      {
      if (NoEIOs()>=2)
        {
        for (int i=0; i<m_tsPwrIn.TerminalCount(); i++)
          m_Electrics.SetImpedance(0, m_tsPwrOut.Terminal(i), m_tsPwrIn.Terminal(i), 0, 0);
        }
      else if (NoEIOs()==1 && EIO_In(0))
        {
        //for (int i=0; i<m_tsPwrIn.TerminalCount(); i++)
        //  m_Electrics.SetImpedance(0, m_tsPwrIn.Terminal(i), m_tsPwrOut.Terminal(i), 0, 0);
        m_tsPwrIn.SetLoad(m_dXfrPower, m_dXfrPwrFct);
        }
      else
        {
        m_tsPwrOut.SetSource(m_dVolts, 0, CDComplex(0.01,0));
        }
      }
    }
  };

//--------------------------------------------------------------------------

void CIncomer::EvalDiscrete()
  {
  m_bPresent=false;

  if (m_bOn && fCrossConnected)
    {
    double kVA=0;
    double kW=0;

    for (int i=0; i<m_tsPwrIn.TerminalCount(); i++)
      {
      CETerminal &T=*m_tsPwrIn.Terminal(i);

      CDComplex V=T.Volts();
      CDComplex I=T.Current(0, m_tsPwrOut.Terminal(i));
      kVA+=0.001*V.Mag()*I.Mag();
      kW+=0.001*V.Mag()*I.Mag()*cos(V.Ang()-I.Ang());

      }
    m_dXfrPower=kW;
    m_dXfrPwrFct=Min(kW/GTZ(kVA), 1.0);
    }
  }

//--------------------------------------------------------------------------

flag CIncomer::GetModelAction(MdlActionArray & Acts)
  {
  Acts.SetSize(0);
  Acts.SetAtGrow(0, MdlAction(0, MAT_State, !m_bOn, "On", 1));
  Acts.SetAtGrow(1, MdlAction(1, MAT_State, m_bOn, "Off", 0));
  Acts.SetAtGrow(2, MdlAction(2, MAT_Switch));
  return true;
  };

//--------------------------------------------------------------------------

flag CIncomer::SetModelAction(MdlAction & Act)
  {
  switch (Act.iIndex)
    {
    case 0:
    case 1:
      m_bOn=Act.iValue!=0;
      break;
    case 2:
      m_bOn=!m_bOn;
      break;
    }
  return true;
  };

//--------------------------------------------------------------------------

dword CIncomer::ModelStatus()
  {
  dword Status=FlwNode::ModelStatus();
  Status|=/*FNS_IsElec|*/(m_bPresent ? FNS_On : FNS_Off);
  return Status;
  }

//--------------------------------------------------------------------------

flag CIncomer::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case  1: pS="E\tBad external tag references"; return 1;
    case  2: pS="E\tErrors in evaluating functions"; return 1;
    case  3: pS="E\tEngineering units invalid"; return 1;
    default:
      return FlwNode::CIStrng(No, pS);
    }
  }

//==========================================================================
