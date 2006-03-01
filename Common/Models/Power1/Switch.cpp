//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#define  __SWITCH_CPP

#include "stdafx.h"
#include "sc_defs.h"
#include "pgm_e.h"
#include "switch.h"

#include "scd_wm.h"

XID xidTSCfg   = PwrXID(1000);
XID xidHtLoad  = PwrXID(1001);

//==========================================================================
//
//
//
//==========================================================================

static IOAreaRec IsolatorIOAreaList[] =
  {{"Input",         "In" , ElecIOId(0), LIO_In0 , nc_ELnk, 0,50, 0, 0.0, {0,0,0,0}, NULL, NULL, "PwrIn"},
   {"Output",        "Out", ElecIOId(1), LIO_Out0, nc_ELnk, 0,50, 0, 0.0, {0,0,0,0}, NULL, NULL, "PwrOut"},
  {NULL}};

//==========================================================================
//
//
//
//==========================================================================

static double Drw_Isolator[] =
       {
       DD_Arc,  0,2,1,
       DD_Arc,  0,-2,1,
       DD_Poly, 0,-2, 2, 2,
       DD_End
       };

IMPLEMENT_MODELUNIT(CIsolator, "E_Isolator", "", Drw_Isolator, "Electrical", "Is", TOC_ALL|TOC_GRP_POWERDIST|TOC_POWERDIST,
                   "Electrical:Isolator",
                   "Isolator.")

CIsolator::CIsolator(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  FlwNode(pClass_, TagIn, pAttach, eAttach),
  m_tsPwrIn(this, "PwrIn", eCT_3Ph4W),
  m_tsPwrOut(this, "PwrOut", eCT_3Ph4W)
  {
  AttachClassInfo(nc_Elec, IsolatorIOAreaList);
  fActiveHoldOK_Dyn=true;
  m_bOn = 0;
  }

//--------------------------------------------------------------------------

CIsolator::~CIsolator()
  {
  }

//--------------------------------------------------------------------------

void CIsolator::ResetData(flag Complete)
  {
  }

//--------------------------------------------------------------------------

void CIsolator::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this, "CIsolator", NULL, DDB_NoPage);
  DDB.Text("");
  DDB.Long       ("TermCfg",    "", DC_,    "",  xidTSCfg, this, isParm, DDBCTTypes);
  DDB.CheckBoxBtn("On",         "", DC_,    "",  &m_bOn,          this, isParm);
  DDB.Double     ("HeatLoad",   "", DC_Pwr, "W",  xidHtLoad,          this, isResult);
  DDB.Text("");
  m_tsPwrIn.BuildDataDefn(DDB, 1, true);
  m_tsPwrOut.BuildDataDefn(DDB, 2, true);

  DDB.EndStruct();
  }

//--------------------------------------------------------------------------

flag CIsolator::DataXchg(DataChangeBlk & DCB)
  {
  if (FlwNode::DataXchg(DCB))
    return 1;
  if (m_tsPwrIn.DataXchg(DCB, 1))
    return 1;
  if (m_tsPwrOut.DataXchg(DCB, 2))
    return 1;
  switch (DCB.lHandle)
    {
    case xidTSCfg:
      if (DCB.rL)
        {
        m_tsPwrIn.SetConfiguration((eTSConfigurations)*DCB.rL);
        m_tsPwrOut.SetConfiguration((eTSConfigurations)*DCB.rL);
        }
      DCB.L=m_tsPwrIn.Configuration();
      return 1;
    case xidHtLoad:
      {
      DCB.D=0;
      for (int w=0; w<m_Electrics.ComponentCount(); w++)
        DCB.D+=m_Electrics.CompData(w)->HeatLoad();
      return 1;
      }
    }
  return False;
  }

//--------------------------------------------------------------------------

flag CIsolator::ValidateData(ValidateDataBlk & VDB)
  {
  //ELEC for (int e=EIO1(); e<EION(); e++)
  //ELEC   EIO_Conduit(e).m_TStrip.SetConfiguration(m_nType, m_dVolts);
  return FlwNode::ValidateData(VDB);
  }

//--------------------------------------------------------------------------

void CIsolator::CollectElectrics(CNodeElectricsArray & TSCA)
  {
  FlwNode::CollectElectrics(TSCA);

  m_Electrics.AddTermStrip(&m_tsPwrIn);
  m_Electrics.AddTermStrip(&m_tsPwrOut);
  }

//--------------------------------------------------------------------------

void CIsolator::ConnectElectrics()
  {
  FlwNode::ConnectElectrics();

  if (m_bOn)
    {
    ASSERT(m_tsPwrIn.TerminalCount()==m_tsPwrOut.TerminalCount());
    for (int w=0; w<m_tsPwrIn.TerminalCount(); w++)
      m_Electrics.SetImpedance(0, m_tsPwrIn[w], m_tsPwrOut[w], 0.001, 0);
    }
  };

//--------------------------------------------------------------------------

flag CIsolator::GetModelAction(Strng & Tag, MdlActionArray & Acts)
  {
  Acts.SetSize(0);
  Acts.SetAtGrow(0, MdlAction(0, MAT_State, !m_bOn, "On", 1));
  Acts.SetAtGrow(1, MdlAction(1, MAT_State, m_bOn, "Off", 0));
  Acts.SetAtGrow(2, MdlAction(2, MAT_Switch));
  return true;
  };

//--------------------------------------------------------------------------

flag CIsolator::SetModelAction(Strng & Tag, MdlAction & Act)
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

dword CIsolator::ModelStatus()
  {
  dword Status=FlwNode::ModelStatus();
  Status|=/*FNS_IsElec|*/(m_bOn ? FNS_On : FNS_Off);
  return Status;
  }

//--------------------------------------------------------------------------

flag CIsolator::CIStrng(int No, pchar & pS)
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
//
//
//
//==========================================================================

static IOAreaRec CctBreakerIOAreaList[] =
  {{"Input",         "In" , ElecIOId(0), LIO_In0 , nc_ELnk, 0,50, 0, 0.0, {0,0,0,0}, NULL, NULL, "PwrIn"},
   {"Output",        "Out", ElecIOId(1), LIO_Out0, nc_ELnk, 0,50, 0, 0.0, {0,0,0,0}, NULL, NULL, "PwrOut"},
   {NULL}};

static double Drw_CctBreaker[] =
       {
       DD_Arc,  0,2,1,
       DD_Arc,  0,-2,1,
       DD_Poly, 0,-2, 2, 2,
       DD_End
       };

IMPLEMENT_MODELUNIT(CCctBreaker, "E_CctBrk", "", Drw_CctBreaker, "Electrical", "Ccb", TOC_ALL|TOC_GRP_POWERDIST|TOC_POWERDIST,
                   "Electrical:CctBreaker",
                   "Circuit Breaker.")

CCctBreaker::CCctBreaker(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  FlwNode(pClass_, TagIn, pAttach, eAttach),
  m_tsPwrIn(this, "PwrIn", eCT_3Ph4W),
  m_tsPwrOut(this, "PwrOut", eCT_3Ph4W)
  {
  AttachClassInfo(nc_Elec, CctBreakerIOAreaList);
  fActiveHoldOK_Dyn=true;
  m_bOn = 0;
  }

//--------------------------------------------------------------------------

CCctBreaker::~CCctBreaker()
  {
  }

//--------------------------------------------------------------------------

void CCctBreaker::ResetData(flag Complete)
  {
  }

//--------------------------------------------------------------------------

void CCctBreaker::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this, "CCctBreaker", NULL, DDB_NoPage);
  DDB.Text("");
  DDB.Long       ("TermCfg",    "", DC_,   "",  xidTSCfg, this, isParm, DDBCTTypes);
  DDB.CheckBoxBtn("On",     "", DC_,   "", &m_bOn,        this, isParm);
  DDB.Double     ("HeatLoad",   "", DC_Pwr, "W",  xidHtLoad,          this, isResult);
  DDB.Text("");
  m_tsPwrIn.BuildDataDefn(DDB, 1, true);
  m_tsPwrOut.BuildDataDefn(DDB, 2, true);

  DDB.EndStruct();
  }

//--------------------------------------------------------------------------

flag CCctBreaker::DataXchg(DataChangeBlk & DCB)
  {
  if (FlwNode::DataXchg(DCB))
    return 1;
  if (m_tsPwrIn.DataXchg(DCB, 1))
    return 1;
  if (m_tsPwrOut.DataXchg(DCB, 2))
    return 1;
  switch (DCB.lHandle)
    {
    case xidTSCfg:
      if (DCB.rL)
        {
        m_tsPwrIn.SetConfiguration((eTSConfigurations)*DCB.rL);
        m_tsPwrOut.SetConfiguration((eTSConfigurations)*DCB.rL);
        }
      DCB.L=m_tsPwrIn.Configuration();
      return 1;
    case xidHtLoad:
      {
      DCB.D=0;
      for (int w=0; w<m_Electrics.ComponentCount(); w++)
        DCB.D+=m_Electrics.CompData(w)->HeatLoad();
      return 1;
      }
    }
  return False;
  }

//--------------------------------------------------------------------------

flag CCctBreaker::ValidateData(ValidateDataBlk & VDB)
  {
  return FlwNode::ValidateData(VDB);
  }

//--------------------------------------------------------------------------

void CCctBreaker::CollectElectrics(CNodeElectricsArray & TSCA)
  {
  FlwNode::CollectElectrics(TSCA);

  m_Electrics.AddTermStrip(&m_tsPwrIn);
  m_Electrics.AddTermStrip(&m_tsPwrOut);
  }

//--------------------------------------------------------------------------

void CCctBreaker::ConnectElectrics()
  {
  FlwNode::ConnectElectrics();
  
  if (m_bOn)
    {
    ASSERT(m_tsPwrIn.TerminalCount()==m_tsPwrOut.TerminalCount());
    for (int w=0; w<m_tsPwrIn.TerminalCount(); w++)
      m_Electrics.SetImpedance(0, m_tsPwrIn[w], m_tsPwrOut[w], 0.001, 0);
    }
  };

//--------------------------------------------------------------------------

flag CCctBreaker::GetModelAction(Strng & Tag, MdlActionArray & Acts)
  {
  Acts.SetSize(0);
  Acts.SetAtGrow(0, MdlAction(0, MAT_State, !m_bOn, "On", 1));
  Acts.SetAtGrow(1, MdlAction(1, MAT_State, m_bOn, "Off", 0));
  Acts.SetAtGrow(2, MdlAction(2, MAT_Switch));
  return true;
  };

//--------------------------------------------------------------------------

flag CCctBreaker::SetModelAction(Strng & Tag, MdlAction & Act)
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

dword CCctBreaker::ModelStatus()
  {
  dword Status=FlwNode::ModelStatus();
  Status|=/*FNS_IsElec|*/(m_bOn? FNS_On : FNS_Off);
  return Status;
  }

//--------------------------------------------------------------------------

flag CCctBreaker::CIStrng(int No, pchar & pS)
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
//==========================================================================
