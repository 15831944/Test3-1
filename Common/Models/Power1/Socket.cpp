//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#define  __SOCKET_CPP

#include "stdafx.h"
#include "sc_defs.h"
#include "pgm_e.h"
#include "Socket.h"

#include "scd_wm.h"

XID xidTSCfg   = PwrXID(1000);
XID xidOn      = PwrXID(1001);

//==========================================================================
//
//
//
//==========================================================================

static IOAreaRec SocketIOAreaList[] =
  {
    {"Input",         "In" , ElecIOId(0), LIO_In0, nc_ELnk, 0, 50, 0, 0.0, {0,0,0,0}, NULL, NULL, "PwrIn"},
    {"Output",        "Out", ElecIOId(1), LIO_Out, nc_ELnk, 0, 50, 0, 0.0, {0,0,0,0}, NULL, NULL, "PwrOut"},
    {NULL}
  };

static double Drw_Socket[] =
  {
  DD_Poly, 2,4, 2,0, -2,0, -2,4, 2,4,
  DD_Arc,  0,-2,2,
  DD_End
  };

IMPLEMENT_MODELUNIT(CSocket, "E_Socket", "", Drw_Socket, "Electrical", "Sk", TOC_ALL|TOC_GRP_POWERDIST|TOC_POWERDIST,
                   "Electrical:Socket",
                   "Socket.")

CSocket::CSocket(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  FlwNode(pClass_, TagIn, pAttach, eAttach),
  m_Socket("Socket", this, TOA_Embedded),
  m_tsPwrIn(this, "PwrIn", eCT_3Ph4W),
  m_tsPwrOut(this, "PwrOut", eCT_3Ph4W)
  {
  AttachClassInfo(nc_Elec, SocketIOAreaList);
  fActiveHoldOK_Dyn=true;
  //m_bOn = 1;
  m_bPresent = 0;
  //m_dValue = 0;
  //m_nType=eCT_3Ph4W;
  //m_dVolts=220;
  m_dRqdPower=0;
  m_dRqdPwrFct=1;
  m_Socket.SetSupply(&m_Supply);
  m_Supply.m_pTermStrip=&m_tsPwrOut;
  }

//--------------------------------------------------------------------------

CSocket::~CSocket()
  {
  }

//--------------------------------------------------------------------------

void CSocket::ResetData(flag Complete)
  {
  }

//--------------------------------------------------------------------------

void CSocket::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this, "CSocket", NULL, DDB_NoPage);
  DDB.Text("");
  DDB.Long       ("TermCfg",    "", DC_,   "",  xidTSCfg, this, isParm, DDBCTTypes);
  //DDB.Double     ("NomPhVolts", "", DC_V,  "V", &m_dVolts,       this, isParm);
  DDB.CheckBoxBtn("On",     "", DC_,   "",xidOn,        this, isParm);

  DDB.Text("");
  DDB.CheckBoxBtn("Present",     "", DC_,   "", &m_bPresent,      this, isResult);
  m_Supply.BuildDataDefn(DDB, this, "Supply", false);//, false);
  DDB.Text("");
  //DDB.String     ("Target",  "", DC_,   "", xidTargetTag,   this, isParmStopped);

  m_Socket.BuildDataDefn(DDB);
  DDB.Text("");
  m_tsPwrIn.BuildDataDefn(DDB, 1, true);
  m_tsPwrOut.BuildDataDefn(DDB, 2, true);

  DDB.EndStruct();
  }

//--------------------------------------------------------------------------

flag CSocket::DataXchg(DataChangeBlk & DCB)
  {
  if (FlwNode::DataXchg(DCB))
    return 1;
  if (m_Supply.DataXchg(DCB, this))
    return 1;
  if (m_Socket.DataXchg(DCB))
    return 1;
  if (m_tsPwrIn.DataXchg(DCB, 1))
    return 1;
  if (m_tsPwrOut.DataXchg(DCB, 2))
    return 1;
  switch (DCB.lHandle)
    {
    case xidOn:
      if (DCB.rB)
        m_Socket.SetOn(*DCB.rB!=0);
      DCB.B=m_Socket.On();
      return 1;
    case xidTSCfg:
      if (DCB.rL)
        {
        m_tsPwrIn.SetConfiguration((eTSConfigurations)*DCB.rL);
        m_tsPwrOut.SetConfiguration((eTSConfigurations)*DCB.rL);
        }
      DCB.L=m_tsPwrIn.Configuration();
      return 1;
    }

  return 0;
  };

//--------------------------------------------------------------------------

flag CSocket::ValidateData(ValidateDataBlk & VDB)
  {
  //m_tsPwrIn.SetConfiguration(m_nType);
  //for (int e=EIO1(); e<EION(); e++)
  //  EIO_Conduit(e).m_TStrip.SetStdCable(m_nType, m_dVolts);
  return FlwNode::ValidateData(VDB);
  }

// -------------------------------------------------------------------------

void CSocket::PostConnect(int IONo)
  {
  FlwNode::PostConnect(IONo);
//  if (EIOId_Self(IONo)==0)
//    m_Socket.SetConduit(&EIO_Conduit(IONo));
  };

//--------------------------------------------------------------------------

void CSocket::PreDisConnect(int IONo)
  {
//  if (EIOId_Self(IONo)==0)
//    m_Socket.SetConduit(NULL);
  FlwNode::PreDisConnect(IONo);
  };

//--------------------------------------------------------------------------

void CSocket::CollectElectrics(CNodeElectricsArray & TSCA)
  {
  FlwNode::CollectElectrics(TSCA);

  m_Electrics.AddTermStrip(&m_tsPwrIn);
  m_Electrics.AddTermStrip(&m_tsPwrOut);
  }

//--------------------------------------------------------------------------

void CSocket::ConnectElectrics()
  {
  FlwNode::ConnectElectrics();
  
  if (m_Socket.On())
    {
    for (int i=0; i<m_tsPwrIn.TerminalCount(); i++)
      m_Electrics.SetImpedance(0, m_tsPwrIn.Terminal(i), m_tsPwrOut.Terminal(i), 0, 0);
    if (NoEIOs()==1 && EIO_In(0))
      m_tsPwrIn.SetLoad(m_dRqdPower, m_dRqdPwrFct);
    }
  };

//--------------------------------------------------------------------------

void CSocket::EvalPowerRequired()
  {
  m_bPresent=false;
  m_Socket.EvalPowerRequired();
  if (m_Socket.Load())
    {
    m_dRqdPower=m_Socket.Load()->ShaftPower();
    CElecLoad * p=dynamic_cast<CElecLoad*>(m_Socket.Load());
    if (p)
      {
      m_dRqdPwrFct=cos(p->m_dPhaseAngle);
      }
    }
  }

//--------------------------------------------------------------------------

flag CSocket::GetModelAction(Strng & Tag, MdlActionArray & Acts)
  {
  Acts.SetSize(0);
  Acts.SetAtGrow(0, MdlAction(0, MAT_State, !m_Socket.On(), "On", 1));
  Acts.SetAtGrow(1, MdlAction(1, MAT_State, m_Socket.On(), "Off", 0));
  Acts.SetAtGrow(2, MdlAction(2, MAT_Switch));
  return true;
  };

//--------------------------------------------------------------------------

flag CSocket::SetModelAction(Strng & Tag, MdlAction & Act)
  {
  switch (Act.iIndex)
    {
    case 0:
    case 1:
      m_Socket.SetOn(Act.iValue!=0);
      break;
    case 2:
      m_Socket.SetOn(!m_Socket.On());
      break;
    }
  return true;
  };

//--------------------------------------------------------------------------

dword CSocket::ModelStatus()
  {
  dword Status=FlwNode::ModelStatus();
  Status|=/*FNS_IsElec|*/(m_Socket.On()? FNS_On : FNS_Off);
  return Status;
  }

//--------------------------------------------------------------------------

flag CSocket::CIStrng(int No, pchar & pS)
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
