//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#define  __TRANSFORMER_CPP

#include "stdafx.h"
#include "sc_defs.h"
#include "pgm_e.h"
#include "transformer.h"

#include "scd_wm.h"

XID xidTSCfgP   = PwrXID(1000);
XID xidTSCfgS   = PwrXID(1001);
XID xidPriR     = PwrXID(1002);
XID xidPriX     = PwrXID(1003);
XID xidSecR     = PwrXID(1004);
XID xidSecX     = PwrXID(1005);
XID xidHtLoad   = PwrXID(1006);

//==========================================================================
//
//
//
//==========================================================================

static IOAreaRec TwoEIOAreaList[] =
  {{"Input",         "In" , ElecIOId(0), LIO_In0 , nc_ELnk, 0, 50, 0, 0.0, {0,0,0,0}, NULL, NULL, -1, "Pri"},
   {"Input",         "I"  , ElecIOId(0), LIO_In  , nc_ELnk, 0, 50, 0},
   {"Output",        "Out", ElecIOId(1), LIO_Out0, nc_ELnk, 0, 50, 0, 0.0, {0,0,0,0}, NULL, NULL, -1, "Sec"},
   {"Output",        "O"  , ElecIOId(1), LIO_Out , nc_ELnk, 0, 50, 0},
   {NULL}};

static double Drw_Transformer[] =
   {
   DD_Arc, 0,2,3,
   DD_Arc, 0,-2,3,
   DD_End
   };

IMPLEMENT_MODELUNIT(CTransformer, "E_Transformer", "", Drw_Transformer, "Electrical", "Xfm", TOC_ALL|TOC_GRP_POWERDIST|TOC_POWERDIST,
                    "Electrical:Transformer",
                    "Electrical - Transformer.");

CTransformer::CTransformer(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  FlwNode(pClass_, TagIn, pAttach, eAttach),
  m_tsPri(this, "Pri", eCT_3Ph4W),
  m_tsSec(this, "Sec", eCT_3Ph4W),
  m_tsLcl(this, "Lcl", eCT_User),
  m_V1(std::polar(220.0,0.0)),
  m_V2(std::polar(220.0,2*PI/3)),
  m_V3(std::polar(220.0,4*PI/3)),
  m_Z1(1e10,0.0),
  m_Z2(1e10,0.0),
  m_Z3(1e10,0.0),
  m_PriZ(0.01,0.001), 
  m_SecZ(0.01,0.001), 
  m_NoLoadZ(1.0e10,10.0),
  m_dPriV(11000),
  m_dSecV(380),
  m_dNLI(1.0),
  m_dNLA(-acos(0.8))
  {
  m_tsLcl.SetInternal(true);
  m_tsLcl.Add("PR");
  m_tsLcl.Add("PW");
  m_tsLcl.Add("PB");
  m_tsLcl.Add("PN");
  m_tsLcl.Add("SR");
  m_tsLcl.Add("SW");
  m_tsLcl.Add("SB");
  m_tsLcl.Add("SN");
  AttachClassInfo(nc_Elec, TwoEIOAreaList);
  fActiveHoldOK_Dyn=true;
  }

//--------------------------------------------------------------------------

CTransformer::~CTransformer()
  {
  }

//--------------------------------------------------------------------------

void CTransformer::ResetData(flag Complete)
  {
  }

//--------------------------------------------------------------------------

void CTransformer::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this, "CTransformer", NULL, DDB_NoPage);
  DDB.Text("");
  DDB.Long       ("PriCfg",    "", DC_,   "",  xidTSCfgP, this, isParm, DDBCTTypes);
  DDB.Long       ("SecCfg",    "", DC_,   "",  xidTSCfgS, this, isParm, DDBCTTypes);
  DDB.Text("");
  DDB.Double       ("NomPriVolts",   "", DC_V,     "V",     &m_dPriV, this, isParm);
  DDB.Double       ("NomSecVolts",   "", DC_V,     "V",     &m_dSecV, this, isParm);
  DDB.Double       ("PriResist",     "", DC_Ohms,  "Ohms",  xidPriR,  this, isParm);
  DDB.Double       ("PriIndReact",   "", DC_Ohms,  "Ohms",  xidPriX,  this, isParm);
  DDB.Double       ("SecResist",     "", DC_Ohms,  "Ohms",  xidSecR,  this, isParm);
  DDB.Double       ("SecIndReact",   "", DC_Ohms,  "Ohms",  xidSecX,  this, isParm);
  DDB.Double       ("NoLoadCurrent", "", DC_I,     "A",     &m_dNLI,  this, isParm);
  DDB.Double       ("NoLoadPhase",   "", DC_Ang,   "deg",   &m_dNLA,  this, isParm);
  DDB.Double       ("HeatLoad",      "", DC_Pwr,   "W",     xidHtLoad,this, 0);
  DDB.Text("");
  m_tsPri.BuildDataDefn(DDB, 1, true);
  m_tsSec.BuildDataDefn(DDB, 2, true);
  DDB.EndStruct();
  }

//--------------------------------------------------------------------------

flag CTransformer::DataXchg(DataChangeBlk & DCB)
  {
  if (FlwNode::DataXchg(DCB))
    return 1;
  if (m_tsPri.DataXchg(DCB, 1))
    return 1;
  if (m_tsSec.DataXchg(DCB, 2))
    return 1;
  switch (DCB.lHandle)
    {
    case xidTSCfgP:
      if (DCB.rL)
        m_tsPri.SetConfiguration((eTSConfigurations)*DCB.rL);
      DCB.L=m_tsPri.Configuration();
      return 1;
    case xidTSCfgS:
      if (DCB.rL)
        m_tsPri.SetConfiguration((eTSConfigurations)*DCB.rL);
      DCB.L=m_tsPri.Configuration();
      return 1;
    case xidPriR:
      if (DCB.rD)
        m_PriZ=CDComplex(*DCB.rD, m_PriZ.imag());
      DCB.D=m_PriZ.real();
      return 1;
    case xidPriX:
      if (DCB.rD)
        m_PriZ=CDComplex(m_PriZ.real(), *DCB.rD);
      DCB.D=m_PriZ.imag();
      return 1;
    case xidSecR:
      if (DCB.rD)
        m_SecZ=CDComplex(*DCB.rD, m_SecZ.imag());
      DCB.D=m_SecZ.real();
      return 1;
    case xidSecX:
      if (DCB.rD)
        m_SecZ=CDComplex(m_SecZ.real(), *DCB.rD);
      DCB.D=m_SecZ.imag();
      return 1;
    case xidHtLoad:
      {
      DCB.D=0;
      for (int i=0; i<m_PriConnectors.GetSize(); i++)
        DCB.D+=m_PriConnectors[i]->HeatLoad();
      for (int i=0; i<m_SecConnectors.GetSize(); i++)
        DCB.D+=m_SecConnectors[i]->HeatLoad();
      return 1;
      }
    }
  return False;
  }

//--------------------------------------------------------------------------

flag CTransformer::ValidateData(ValidateDataBlk & VDB)
  {
  m_dNLI=Max(0.001, m_dNLI);
  m_dPriV=Max(1.0, m_dPriV);
  m_dSecV=Max(1.0, m_dSecV);
  CDComplex I(std::polar(m_dNLI, m_dNLA));
  m_NoLoadZ = CDComplex(m_dPriV/Sqrt(3.0), 0.0)/I;
  return FlwNode::ValidateData(VDB);
  }

//--------------------------------------------------------------------------

void CTransformer::CollectElectrics(CNodeElectricsArray & TSCA)
  {
  FlwNode::CollectElectrics(TSCA);

  m_Electrics.AddTermStrip(&m_tsPri);
  m_Electrics.AddTermStrip(&m_tsSec);
  m_Electrics.AddTermStrip(&m_tsLcl);
  }

//--------------------------------------------------------------------------

void CTransformer::ConnectElectrics()
  {
  FlwNode::ConnectElectrics();

  CECompData *pC1, *pC2, *pC3;
  long nConns=0;
  switch (m_tsPri.Configuration())
    {
    case eCT_1Ph2W:
      {
      return;
      }
    case eCT_3Ph3W:
    case eCT_3Ph4W:
      {
      CETerminal *pPR=m_tsPri.Find("R");
      CETerminal *pPW=m_tsPri.Find("W");
      CETerminal *pPB=m_tsPri.Find("B");
      CETerminal *pPN=m_tsPri.Configuration()==eCT_3Ph4W ? m_tsPri.Find("N"):m_tsPri.Find("PN");
      CETerminal *plPR=m_tsLcl.Find("PR");
      CETerminal *plPW=m_tsLcl.Find("PW");
      CETerminal *plPB=m_tsLcl.Find("PB");


      if (pPN && pPR && pPW && pPB)
        {
        CDComplex ZNl=m_NoLoadZ+m_PriZ;

        m_PriConnectors.SetSize(9);
        m_PriConnectors[0]->SetImpedance(m_PriZ, false);
        m_PriConnectors[1]->SetImpedance(m_PriZ, false);
        m_PriConnectors[2]->SetImpedance(m_PriZ, false);

        m_PriConnectors[3]->SetImpedance(m_NoLoadZ, true);
        m_PriConnectors[4]->SetImpedance(m_NoLoadZ, true);
        m_PriConnectors[5]->SetImpedance(m_NoLoadZ, true);

        double Ratio=m_dPriV/m_dSecV;

        m_PriConnectors[6]->SetImpedance(m_Z1, true);
        m_PriConnectors[7]->SetImpedance(m_Z2, true);
        m_PriConnectors[8]->SetImpedance(m_Z3, true);

        m_Electrics.SetCompData(this, 0, plPR, pPR, m_PriConnectors[0]);
        m_Electrics.SetCompData(this, 0, plPW, pPW, m_PriConnectors[1]);
        m_Electrics.SetCompData(this, 0, plPB, pPB, m_PriConnectors[2]);
        m_Electrics.SetCompData(this, 0, pPN, plPR, m_PriConnectors[3]);
        m_Electrics.SetCompData(this, 0, pPN, plPW, m_PriConnectors[4]);
        m_Electrics.SetCompData(this, 0, pPN, plPB, m_PriConnectors[5]);
        m_Electrics.SetCompData(this, 1, pPN, plPR, m_PriConnectors[6]);
        m_Electrics.SetCompData(this, 1, pPN, plPW, m_PriConnectors[7]);
        m_Electrics.SetCompData(this, 1, pPN, plPB, m_PriConnectors[8]);

        m_V1=m_PriConnectors[6]->AppliedVoltage()/Ratio;
        m_V2=m_PriConnectors[7]->AppliedVoltage()/Ratio;
        m_V3=m_PriConnectors[8]->AppliedVoltage()/Ratio;

        /*
        m_Electrics.SetImpedance(0, plPR, pPR, m_PriZ, false);
        m_Electrics.SetImpedance(0, plPW, pPW, m_PriZ, false);
        m_Electrics.SetImpedance(0, plPB, pPB, m_PriZ, false);

        m_Electrics.SetImpedance(0, pPN, plPR, m_NoLoadZ, true);
        m_Electrics.SetImpedance(0, pPN, plPW, m_NoLoadZ, true);
        m_Electrics.SetImpedance(0, pPN, plPB, m_NoLoadZ, true);

        double Ratio=m_dPriV/m_dSecV;

        pC1=m_Electrics.SetImpedance(1, pPN, plPR, m_Z1, true);
        pC2=m_Electrics.SetImpedance(1, pPN, plPW, m_Z2, true);
        pC3=m_Electrics.SetImpedance(1, pPN, plPB, m_Z3, true);
        m_V1=pC1->AppliedVoltage()/Ratio;
        m_V2=pC2->AppliedVoltage()/Ratio;
        m_V3=pC3->AppliedVoltage()/Ratio;
        */
        }
      break;
      }
    }

  switch (m_tsSec.Configuration())
    {
    case eCT_1Ph2W:
      {
      return;
      }
    case eCT_3Ph3W:
    case eCT_3Ph4W:
      {
      CETerminal *pSR=m_tsSec.Find("R");
      CETerminal *pSW=m_tsSec.Find("W");
      CETerminal *pSB=m_tsSec.Find("B");
      CETerminal *pSN=m_tsSec.Configuration()==eCT_3Ph4W ? m_tsSec.Find("N"):m_tsSec.Find("SN");
      if (pSN && pSR && pSW && pSB)
        {
        double Ratio=m_dPriV/m_dSecV;
        pC1=m_Electrics.SetSource(0, pSN, pSR, m_V1, m_SecZ);
        pC2=m_Electrics.SetSource(0, pSN, pSW, m_V2, m_SecZ);
        pC3=m_Electrics.SetSource(0, pSN, pSB, m_V3, m_SecZ);
        m_Z1=pC1->LoadImpedance()*Ratio*Ratio;
        m_Z2=pC2->LoadImpedance()*Ratio*Ratio;
        m_Z3=pC3->LoadImpedance()*Ratio*Ratio;
        m_Electrics.SetReferenceVoltage(pSN, CDComplex(0.0,0.0));
        }
      break;
      }
    }
  };

//--------------------------------------------------------------------------

flag CTransformer::GetModelAction(CMdlActionArray & Acts)
  {
  //Acts.SetSize(0);
  //Acts.SetAtGrow(0, CMdlAction(0, MAT_State, 0/*!PSB.On()*/, "On", 1));
  return true;
  };

//--------------------------------------------------------------------------

flag CTransformer::SetModelAction(CMdlAction & Act)
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

dword CTransformer::ModelStatus()
  {
  dword Status=FlwNode::ModelStatus();
  //Status|=(m_bPresent ? FNS_On : FNS_Off);
  return Status;
  }

//--------------------------------------------------------------------------

flag CTransformer::CIStrng(int No, pchar & pS)
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
