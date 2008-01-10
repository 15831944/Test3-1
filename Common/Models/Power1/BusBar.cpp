//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================


#include "stdafx.h"
#define  __BUSBAR_CPP
#include "sc_defs.h"
#include "pgm_e.h"
#include "busbar.h"

#include "scd_wm.h"

XID xidNoSegs     = PwrXID(1000);
XID xidTSCfg      = PwrXID(1001); 
XID xidHtLoad     = PwrXID(1002); 

//==========================================================================
//
//
//
//==========================================================================

static IOAreaRec TwoEIOAreaList[] =
  {{"",         "IO" , ElecIOId(0), LIO_InOut, nc_ELnk, 0, 50, 0},
   {NULL}};

//==========================================================================
//
//
//
//==========================================================================

static double Drw_BusBar[] =
       {
       DD_Poly, -20, 1, 20, 1, 20, -1, -20, -1, -20, 1,
       DD_End
       };

IMPLEMENT_MODELUNIT(CBusBar, "E_BusBar", "", Drw_BusBar, "Electrical", "Bb", TOC_ALL|TOC_GRP_POWERDIST|TOC_POWERDIST,
                   "Electrical:BusBar",
                   "BusBar.")

CBusBar::CBusBar(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  FlwNode(pClass_, TagIn, pAttach, eAttach)
  {
  AttachClassInfo(nc_Elec, TwoEIOAreaList);
  fActiveHoldOK_Dyn=true;
  //m_bPresent = 0;
  m_dArea=0.0005;
  m_eCondMat = eCM_Copper;

  m_Drop.SetSize(1);
  m_Drop[0]=new CETermStrip(this, "Ts0", eCT_3Ph4W);
  SetSegCount(1);
  }

//--------------------------------------------------------------------------

CBusBar::~CBusBar()
  {
  }

//--------------------------------------------------------------------------

LPTSTR CBusBar::GetPrimaryCfg() 
  { 
  m_sNStr.Format("%i", m_Seg.GetSize()); 
  return (LPTSTR)(LPCTSTR)m_sNStr;
  };

//--------------------------------------------------------------------------

flag CBusBar::PutPrimaryCfg(LPCTSTR Cfg) 
  { 
  m_sNStr=Cfg;
  SetSegCount(SafeAtoL(Cfg,1));
  return true;
  };

//--------------------------------------------------------------------------

long CBusBar::GetSegCount()  { return m_Seg.GetSize(); }
long CBusBar::GetDropCount()   { return m_Drop.GetSize(); }

//--------------------------------------------------------------------------

void CBusBar::SetSegCount(long NSeg)
  {
  CString No;
  NSeg=Range(0L, NSeg, 50L);

  long NDrop=NSeg+1;
  for (long i=NDrop; i<m_Drop.GetSize(); i++)
    delete m_Drop[i];

  long NDrop0=m_Drop.GetSize();
  long NSeg0=m_Seg.GetSize();

  m_Drop.SetSize(NDrop);
  for (i=NDrop0; i<NDrop; i++)
    {
    No.Format("Ts%i", i);
    m_Drop[i]=new CETermStrip(this, No, m_Drop[0]->Configuration());
    }

  m_Seg.SetSize(NSeg);
  for (i=NSeg0; i<NSeg; i++)
    m_Seg[i].m_Connectors.SetSize(m_Drop[0]->TerminalCount());
  }

//--------------------------------------------------------------------------

void CBusBar::ResetData(flag Complete)
  {
  }

//--------------------------------------------------------------------------

void CBusBar::BuildDataDefn(DataDefnBlk & DDB)
  {
  CString S;
  DDB.BeginStruct(this, "CBusBar", NULL, DDB_NoPage);
  DDB.Long   ("TermCfg",      "",       DC_,    "",     xidTSCfg,           this, isParm, DDBCTTypes);
  DDB.Long   ("CondType",     "",       DC_,    "",     (long*)&m_eCondMat, this, isParm, DDBCondMaterial);
  DDB.Double ("CrossSection", "XSect",  DC_Area,"mm^2", &m_dArea,           this, isParm);
  DDB.Double  ("HeatLoad",    "",       DC_Pwr, "W",    xidHtLoad,  this, isResult);
  DDB.Text("");
  DDB.Long   ("NoOfSegs",     "",       DC_,    "",     xidNoSegs,          this, isParmStopped);
  long TCnt=m_Drop[0]->TerminalCount();
  if (DDB.BeginArray(this, "Seg", "E_Connect", GetSegCount(), 0, DDB_OptPage))
    {
    for (int i=0; i<GetSegCount(); i++)
      {
      S.Format("%i", i);
      DDB.BeginElement(this, S, NULL, i, DDB_OptPage);
      DDB.Double   ("Length",  "L", DC_L,   "", &m_Seg[i].m_dLen, this, isParm);
      for (int t=0; t<TCnt; t++)
        {
        S.Format("%i%s", i, m_Drop[i]->Terminal(t)->Name());
        long Id=xidConnFirst+(i*TCnt+t)*xidConnCount;
        m_Seg[i].m_Connectors[t]->BuildDataDefn(DDB, this, Id, S);
        }
      }
    }
  DDB.EndArray();

  DDB.Text("");
  if (DDB.BeginArray(this, "Drop", "E_Terminal", GetDropCount(), 0, DDB_OptPage))
    {
    for (int i=0; i<GetDropCount(); i++)
      m_Drop[i]->BuildDataDefn(DDB, i, false);
    }
  DDB.EndArray();
  DDB.EndStruct();
  }

//--------------------------------------------------------------------------

flag CBusBar::DataXchg(DataChangeBlk & DCB)
  {
  if (FlwNode::DataXchg(DCB))
    return 1;
  for (int i=0; i<GetDropCount(); i++)
    if (m_Drop[i]->DataXchg(DCB, i))
      return 1;
  
  switch (DCB.lHandle)
    {
    case xidNoSegs:
      if (DCB.rL)
        SetSegCount(*DCB.rL);
      DCB.L=GetSegCount();
      return 1;
    case xidTSCfg:
      if (DCB.rL)
        {
        for (long i=0; i<m_Drop.GetSize(); i++)
          m_Drop[i]->SetConfiguration((eTSConfigurations)*DCB.rL);
        }
      DCB.L=m_Drop[0]->Configuration();
      return 1;
    case xidHtLoad:
      {
      DCB.D=0;
      for (int i=0; i<GetSegCount(); i++)
        for (int t=0; t<m_Seg[i].m_Connectors.GetSize(); t++)
          DCB.D+=m_Seg[i].m_Connectors[t]->HeatLoad();
      }
      return 1;
    default:
      if (DCB.lHandle>=xidConnFirst && DCB.lHandle<xidConnLast)
        {
        long TCnt=m_Drop[0]->TerminalCount();
        long l=(DCB.lHandle-xidConnFirst)/xidConnCount;
        long i=l/TCnt;
        long t=l%TCnt;
        return m_Seg[i].m_Connectors[t]->DataXchg(DCB, this, 0);
        return 0;
        }
    }
  return False;
  }

//--------------------------------------------------------------------------

flag CBusBar::ValidateData(ValidateDataBlk & VDB)
  {
  m_dArea=Max(m_dArea, 1.0e-6);
  for (int i=0; i<GetSegCount(); i++)
    m_Seg[i].m_dLen=Max(m_Seg[i].m_dLen, 0.0);
  return FlwNode::ValidateData(VDB);
  }

//--------------------------------------------------------------------------
// CTNode Overide
void CBusBar::OnElecComponentChange(eElecChanges EC, CECompData *pC)
  {
  for (int i=0; i<GetSegCount(); i++)
    {
    for (int c=0; c<m_Seg[i].m_Connectors.GetSize(); c++)
      {
//  for (int i=0; i<m_Wires.GetSize(); i++)
//    {
//    CElecWire &W=m_Wires[i];
//    if (&W.m_Connect==pC)
//      {
//      if (EC==eECC_Delete)
//        {
//        // the CEConnect is being deleted elsewhere
//        //m_Wires.RemoveAt(i);
//        }
      }
    }
  }; 

//--------------------------------------------------------------------------

void CBusBar::CollectElectrics(CNodeElectricsArray & TSCA)
  {
  FlwNode::CollectElectrics(TSCA);

  for (int i=0; i<GetDropCount(); i++)
    m_Electrics.AddTermStrip(m_Drop[i]);
  }

//--------------------------------------------------------------------------

void CBusBar::ConnectElectrics()
  {
  FlwNode::ConnectElectrics();

  for (int i=0; i<GetSegCount(); i++)
    {
    ASSERT(m_Drop[i+1]->TerminalCount()==m_Drop[0]->TerminalCount());
    for (int w=0; w<m_Drop[i+1]->TerminalCount(); w++)
      {
      double R=gs_CondResistance[m_eCondMat]*m_Seg[i].m_dLen/GTZ(m_dArea);
      m_Seg[i].m_Connectors[w]->SetImpedance(CDComplex(R, 0.0));

      m_Electrics.SetCompData(this, 0, m_Drop[i]->Terminal(w), m_Drop[i+1]->Terminal(w), m_Seg[i].m_Connectors[w]);
      //m_Electrics.SetImpedance(0, m_Drop[i]->Terminal(w), m_Drop[i+1]->Terminal(w), CDComplex(R, 0.0));
      }
    }
  };

//--------------------------------------------------------------------------

flag CBusBar::GetModelAction(CMdlActionArray & Acts)
  {
  //CMdlAction M0 = {0, MAT_State, 0/*!PSB.On()*/, "On", 1};
  //Acts.SetSize(0);
  //Acts.SetAtGrow(0, M0);
  return true;
  };

//--------------------------------------------------------------------------

flag CBusBar::SetModelAction(CMdlAction & Act)
  {
  switch (Act.iIndex)
    {
    case 0:
      //PSB.SetOn(Act.iValue);
      break;
    }
  return true;
  };

//--------------------------------------------------------------------------

dword CBusBar::ModelStatus()
  {
  dword Status=FlwNode::ModelStatus();
  Status|=0;//FNS_IsElec;//|(m_bOn ? FNS_On : FNS_Off);
  //Status|=(m_bPresent ? FNS_On : FNS_Off);
  return Status;
  }

//--------------------------------------------------------------------------

flag CBusBar::CIStrng(int No, pchar & pS)
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
