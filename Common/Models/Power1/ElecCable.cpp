//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#define  __ELECCABLE_CPP

#include "stdafx.h"
#include "sc_defs.h"
#include "pgm_e.h"
#include "eleccable.h"

#include "scd_wm.h"

XID xidWireCount      = PwrXID(100);
XID xidSrcNd          = PwrXID(101);
XID xidSrcTS          = PwrXID(102);
XID xidSrcTrm         = PwrXID(103);
XID xidDstNd          = PwrXID(104);
XID xidDstTS          = PwrXID(105);
XID xidDstTrm         = PwrXID(106);
XID xidArea           = PwrXID(107);
XID xidCabLength      = PwrXID(108);
XID xidWireLength     = PwrXID(109);

XID xidWireVolts      = PwrXID(110);
XID xidWireVPhase     = PwrXID(111);
XID xidWireAmps       = PwrXID(112);
XID xidWireAPhase     = PwrXID(113);
XID xidLength         = PwrXID(114);
XID xidRAmps          = PwrXID(115);
XID xidXAmps          = PwrXID(116);
XID xidCabHtLoad      = PwrXID(117);

XID xidCabKVA         = PwrXID(120);
XID xidCabKW          = PwrXID(121);
XID xidCabVolts       = PwrXID(122);

//inline double Rad2Deg(double a) { return 180.0/PI*(a<0 ? a+2*PI:a); };
inline double Rad2Deg(double a) { return 180.0/PI*a; };

//==========================================================================
//
//
//
//==========================================================================

//==========================================================================
//
//
//
//==========================================================================

static IOAreaRec TwoEIOAreaList[] =
  {{"Input",         "In" , ElecIOId(0), LIO_In0 , nc_ELnk, 1,50, IOGRP(1)},
   {"Input",         "I"  , ElecIOId(0), LIO_In  , nc_ELnk, 1,50, IOGRP(1)},
   {"Output",        "Out", ElecIOId(1), LIO_Out0, nc_ELnk, 1,50, IOGRP(1)},
   {"Output",        "O"  , ElecIOId(1), LIO_Out , nc_ELnk, 1,50, IOGRP(1)},
   {NULL}};


IMPLEMENT_ELECLINK(CElecCable, "E_Cable", "", "Cab", TOC_ALL|TOC_GRP_POWERDIST|TOC_POWERDIST,
                   "Electrical:Cable",
                   "Electrical:Cable.")

CElecCable::CElecCable(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  FlwNode(pClass_, TagIn, pAttach, eAttach)
  {
  AttachClassInfo(nc_ELnk, TwoEIOAreaList);
  fActiveHoldOK_Dyn=true;
  //m_bPresent = 0;
  //m_dResPerLen= 1;
  m_CabLength = 1;
  m_eCondMat = eCM_Copper;
  }

//--------------------------------------------------------------------------

CElecCable::~CElecCable()
  {
  }

//--------------------------------------------------------------------------

void CElecCable::ResetData(flag Complete)
  {
  }

//--------------------------------------------------------------------------

void CElecCable::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this, "E_Cable", NULL, DDB_NoPage);
  DDB.Text("");
  DDB.String("SrcIO",     "", DC_,        "",     xidSrcIOTag,        this, isResult|noFile|noSnap);
  DDB.String("DstIO",     "", DC_,        "",     xidDstIOTag,        this, isResult|noFile|noSnap);
  DDB.Long  ("CondType",  "", DC_,        "",     (long*)&m_eCondMat, this, isParm, DDBCondMaterial);
  DDB.Double("Length",    "", DC_L,       "m",    xidCabLength,       this, isParm);
  DDB.Double("Power",     "", DC_Pwr,     "kW",   xidCabKW,           this, 0);
  DDB.Double("VA",        "", DC_ElectVA, "kVA",  xidCabKVA,          this, 0);
  DDB.Double("Voltage",   "", DC_V,       "V",    xidCabVolts,        this, 0);
  DDB.Double("HeatLoad",  "", DC_Pwr,     "W",    xidCabHtLoad,       this, 0);

  DDB.Text("");

  DDB.Long("",  "WireCount", DC_,      "",       xidWireCount,  this, isParmConstruct);
  if (m_Wires.GetSize()>0)
    {
    if (DDB.BeginArray(this, "Wire", "E_Connect", m_Wires.GetSize()))
      {
      for (int c=0; c<m_Wires.GetSize(); c++)
        {
        DDB.BeginElement(this, c, NULL, c);
        //if (DDB.ForFiling())
        //  DDB.String("",      "SrcNd",        DC_,      "",       xidSrcNd,  this, 0);
        DDB.String("",      "SrcTermStrip", DC_,      "",       xidSrcTS,  this, isParmConstruct|InitHidden);
        DDB.String("",      "SrcTerminal",  DC_,      "",       xidSrcTrm, this, isParmConstruct|InitHidden);
        //if (DDB.ForFiling())
        //  DDB.String("",      "DstNd",        DC_,      "",       xidDstNd,  this, 0);
        DDB.String("",      "DstTermStrip", DC_,      "",       xidDstTS,  this, isParmConstruct|InitHidden);
        DDB.String("",      "DstTerminal",  DC_,      "",       xidDstTrm, this, isParmConstruct|InitHidden);
        DDB.Double("",      "Length",       DC_L,     "m",      xidLength, this, isParm);
        DDB.Double("",      "XSect",        DC_Area,  "mm^2",   xidArea,   this, isParm);
        long Id=xidConnFirst+c*xidConnCount;
        m_Wires[c].m_CompData.BuildDataDefn(DDB, this, Id, NULL);//S);
        //DDB.Double("",      "Amps",         DC_I,     "A",      xidAmps,   this, 0);
        //DDB.Double("",      "Phase",        DC_Ang,   "°",      xidPhase,  this, 0);
        //DDB.Double("",      "RAmps",        DC_I,     "A",      xidRAmps,  this, isParmConstruct|InitHidden);
        //DDB.Double("",      "XAmps",        DC_I,     "A",      xidXAmps,  this, isParmConstruct|InitHidden);
        }
      }                      
    DDB.EndArray();
    }

  DDB.EndStruct();
  }

//--------------------------------------------------------------------------

flag CElecCable::DataXchg(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
    case xidWireCount:
      if (DCB.rL)
        {
        m_Wires.SetSize(*DCB.rL);
        for (int c=0; c<m_Wires.GetSize(); c++)
          m_Wires[c].m_Length=Max(m_Wires[c].m_Length, m_CabLength); // to check
        }
      DCB.L=m_Wires.GetSize();
      return 1;
    case xidCabHtLoad:
      {
      DCB.D=0;
      for (int c=0; c<m_Wires.GetSize(); c++)
        DCB.D+=m_Wires[c].m_CompData.HeatLoad();
      return 1;
      }
    case xidSrcNd:
      DCB.pC=(NoEIOs()>=1) ? Nd_Rmt(EIO1())->FullObjTag() : "";
      return 1;
    case xidSrcTS:
      if (DCB.rpC)
        m_Wires[DCB.dwUserInfo].m_sSrcTermStripName=DCB.rpC;
      DCB.pC=(LPTSTR)(LPCTSTR)m_Wires[DCB.dwUserInfo].m_sSrcTermStripName;
      return 1;
    case xidSrcTrm: 
      if (DCB.rpC)
        m_Wires[DCB.dwUserInfo].m_sSrcTerminalName=DCB.rpC;
      DCB.pC=(LPTSTR)(LPCTSTR)m_Wires[DCB.dwUserInfo].m_sSrcTerminalName;
      return 1;
    case xidDstNd:
      DCB.pC=(NoEIOs()>=2) ? Nd_Rmt(EIO1()+1)->FullObjTag() : "";
      return 1;
    case xidDstTS:  
      if (DCB.rpC)
        m_Wires[DCB.dwUserInfo].m_sDstTermStripName=DCB.rpC;
      DCB.pC=(LPTSTR)(LPCTSTR)m_Wires[DCB.dwUserInfo].m_sDstTermStripName;
      return 1;
    case xidDstTrm: 
      if (DCB.rpC)
        m_Wires[DCB.dwUserInfo].m_sDstTerminalName=DCB.rpC;
      DCB.pC=(LPTSTR)(LPCTSTR)m_Wires[DCB.dwUserInfo].m_sDstTerminalName;
      return 1;
    case xidArea:   
      if (DCB.rD)
        m_Wires[DCB.dwUserInfo].m_Area=*DCB.rD;
      DCB.D=m_Wires[DCB.dwUserInfo].m_Area;
      return 1;
    case xidLength:   
      if (DCB.rD)
        m_Wires[DCB.dwUserInfo].m_Length=Max(m_CabLength, *DCB.rD);
      DCB.D=m_Wires[DCB.dwUserInfo].m_Length;
      return 1;
    case xidCabLength:
      if (DCB.rD)
        {
        double OldCabLength=m_CabLength;
        m_CabLength=*DCB.rD;
        for (int c=0; c<m_Wires.GetSize(); c++)
          {
          m_Wires[c].m_Length+=m_CabLength-OldCabLength;
          m_Wires[c].m_Length=Max(m_Wires[c].m_Length, m_CabLength); // to check
          }
        }     
      DCB.D=m_CabLength;
      return 1;
    case xidCabKVA:
      DCB.D=m_kVA;
      return 1;
    case xidCabKW:
      DCB.D=m_kW;
      return 1;
    case xidCabVolts:
      DCB.D=m_CabV;
      return 1;
    //case xidAmps:
    //  //if (m_Wires[DCB.dwUserInfo].m_pConnect)
    //  DCB.D=m_Wires[DCB.dwUserInfo].m_CompData.m_I.Mag();
    //  return 1;
    //case xidPhase:
    //  //if (m_Wires[DCB.dwUserInfo].m_pConnect)
    //  DCB.D=m_Wires[DCB.dwUserInfo].m_CompData.m_I.Ang();
    //  return 1;
    //case xidRAmps:
    //  if (DCB.rD)
    //    m_Wires[DCB.dwUserInfo].m_CompData.m_I._Val[0]=*DCB.rD;
    //  DCB.D=m_Wires[DCB.dwUserInfo].m_CompData.m_I.real();
    //  return 1;
    //case xidXAmps:
    //  if (DCB.rD)
    //    m_Wires[DCB.dwUserInfo].m_CompData.m_I._Val[1]=*DCB.rD;
    //  DCB.D=m_Wires[DCB.dwUserInfo].m_CompData.m_I.imag();
    //  return 1;
    default:
      if (DCB.lHandle>=xidConnFirst && DCB.lHandle<xidConnLast)
        return m_Wires[(DCB.lHandle-xidConnFirst)/xidConnCount].m_CompData.DataXchg(DCB, this, 0);//S);
    //case xidSrcV:
    //  if (m_Wires[DCB.dwUserInfo].m_CompData.m_pSrc)
    //    DCB.D=m_Wires[DCB.dwUserInfo].m_CompData.m_pSrc->m_V.Mag();
    //  return 1;
    //case xidDstV:
    //  if (m_Wires[DCB.dwUserInfo].m_CompData.m_pDst)
    //    DCB.D=m_Wires[DCB.dwUserInfo].m_CompData.m_pDst->m_V.Mag();
    //  return 1;
    }
  
  if (FlwNode::DataXchg(DCB))
    return 1;
//  if (NoEIOs()>=1 && ((DCB.dwUserInfo/1000)==0) && EIO_Conduit(0).DataXchg(DCB, this))
//    return 1;
//  if (NoEIOs()>=2 && ((DCB.dwUserInfo/1000)==1) && EIO_Conduit(1).DataXchg(DCB, this))
//    return 1;
  return False;
  }

//--------------------------------------------------------------------------

flag CElecCable::ValidateData(ValidateDataBlk & VDB)
  {
  for (int c=0; c<m_Wires.GetSize(); c++)
    m_Wires[c].m_Length=Max(m_Wires[c].m_Length, m_CabLength); // to check
  return FlwNode::ValidateData(VDB);
  }

//--------------------------------------------------------------------------

int CElecCable::GetLinkWiring(CLinkWiring &LinkWiring)
  {
  if (NoEIOs()==2)
    {
    for (int Pass=0; Pass<2; Pass++)
      {
      FlwNode * pR= Pass==0 ? (Nd_Rmt(EIO1())) : (Nd_Rmt(EIO1()+1));
      if (Pass==0)
        LinkWiring.m_SrcNdTag=pR->FullObjTag();
      else
        LinkWiring.m_DstNdTag=pR->FullObjTag();

      CNodeElectricsArray TSCA;
      pR->CollectElectrics(TSCA);
      CNodeElectrics &TS=pR->m_Electrics;
      CArray<CLinkWiring::CTerm, CLinkWiring::CTerm&> &Info=(Pass==0?LinkWiring.m_SrcTerminals:LinkWiring.m_DstTerminals); 

      long N=0;
      for (int i=0;i<TS.TermStripCount(); i++)
        {
        if (TS[i]->Internal())
          continue;
        N+=TS[i]->TerminalCount();
        }

      Info.SetSize(N);
      N=0;
      for (i=0;i<TS.TermStripCount(); i++)
        {
        if (TS[i]->Internal())
          continue;
        for (int j=0; j<TS[i]->TerminalCount(); j++)
          {
          CLinkWiring::CTerm &LT=Info[N];
          LT.m_sTermStripName=TS[i]->Name();
          LT.m_sTerminalName=(*TS[i])[j]->Name();
          LT.m_iWire=-1;
          for (int w=0; w<m_Wires.GetSize(); w++)
            {
            CEWire &W=m_Wires[w];
            if (Pass==0)
              {
              if (W.m_sSrcTermStripName==LT.m_sTermStripName && W.m_sSrcTerminalName==LT.m_sTerminalName)
                LT.m_iWire=w;
              }
            else
              {
              if (W.m_sDstTermStripName==LT.m_sTermStripName && W.m_sDstTerminalName==LT.m_sTerminalName)
                LT.m_iWire=w;
              }
            }
          N++;
          }
        }
      }
    }
  return m_Wires.GetSize();
  }

//--------------------------------------------------------------------------

int CElecCable::SetLinkWiring(CLinkWiring &LinkWiring)
  {
  if (NoEIOs()==2)
    {
    for (int i=0; i<m_Wires.GetSize(); i++)
      m_Electrics.RemoveCompData(&m_Wires[i].m_CompData);

    if (LinkWiring.m_bAutoWire)
      {
      CNodeWiring NW0, NW1;
      FlwNode * p0=Nd_Rmt(EIO1());
      FlwNode * p1=Nd_Rmt(EIO1()+1);
      p0->AutoWiring(AW_Prepare);
      p1->AutoWiring(AW_Prepare);
      p0->GetNodeWiring(NW0);
      p1->GetNodeWiring(NW1);

      CNodeWiring::CTermStripArray &SrcTSs = NW0.m_TermStrips;
      for (int iSrcTS=0; iSrcTS<SrcTSs.GetCount(); iSrcTS++)
        if (LinkWiring.m_SrcTS.CompareNoCase(SrcTSs[iSrcTS].m_sName)==0)
          break;
      CNodeWiring::CTermStripArray &DstTSs = NW1.m_TermStrips;
      for (int iDstTS=0; iDstTS<DstTSs.GetCount(); iDstTS++)
        if (LinkWiring.m_DstTS.CompareNoCase(DstTSs[iDstTS].m_sName)==0)
          break;

      if (iSrcTS<SrcTSs.GetCount() && iDstTS<DstTSs.GetCount())
        {
        CNodeWiring::CTerminalArray &SrcTrms = NW0.m_Terminals;
        CNodeWiring::CTerminalArray &DstTrms = NW1.m_Terminals;
 
        // Match Terminal Names
        bool Good=true;
        CLArray iSrcTerms;
        CLArray iDstTerms;
        int nWires=NW0.TSTerminals(iSrcTS, iSrcTerms);
        if (nWires>0)
          {
          iDstTerms.SetSize(nWires);
          for (int t0=0; t0<nWires; t0++)
            {
            int iDstTerm=NW1.FindTerminal(iDstTS, SrcTrms[iSrcTerms[t0]].m_sName); 
            if (iDstTerm>=0)
              iDstTerms[t0]=iDstTerm;
            else 
              Good=false;
            }
          }
        else 
          Good=false;
        
        if (Good)
          {
          m_Wires.SetSize(nWires);
          for (long iWire=0; iWire<nWires; iWire++)
            {
            CEWire & W=m_Wires[iWire];
            W.m_sSrcTermStripName = LinkWiring.m_SrcTS;
            W.m_sSrcTerminalName  = NW0.m_Terminals[iSrcTerms[iWire]].m_sName;
            W.m_sDstTermStripName = LinkWiring.m_DstTS;
            W.m_sDstTerminalName  = NW1.m_Terminals[iDstTerms[iWire]].m_sName;
            }
          return nWires;
          }
        }
      return -1;
      }
    else
      {
      long nWires=0;
      for (i=0; i<LinkWiring.m_SrcTerminals.GetSize(); i++)
        if (LinkWiring.m_SrcTerminals[i].m_iWire>=0)
          nWires++;
                      
      m_Wires.SetSize(nWires);
      long iWire=0;
      for (i=0; i<LinkWiring.m_SrcTerminals.GetSize(); i++)
        {
        if (LinkWiring.m_SrcTerminals[i].m_iWire>=0)
          {
          CLinkWiring::CTerm & SI=LinkWiring.m_SrcTerminals[i];
          for (int j=0; j<LinkWiring.m_DstTerminals.GetSize(); j++)
            if (LinkWiring.m_DstTerminals[j].m_iWire==SI.m_iWire)
              break;
          ASSERT(j<LinkWiring.m_DstTerminals.GetSize());
          CLinkWiring::CTerm & DI=LinkWiring.m_DstTerminals[j];
          CEWire & W=m_Wires[iWire];
          W.m_sSrcTermStripName = SI.m_sTermStripName;
          W.m_sSrcTerminalName  = SI.m_sTerminalName;
          W.m_sDstTermStripName = DI.m_sTermStripName;
          W.m_sDstTerminalName  = DI.m_sTerminalName;
          iWire++;
          }
        }
      }
    }
  return m_Wires.GetSize();
  };

//--------------------------------------------------------------------------

void CElecCable::ClearWires()
  {
  for (int i=0; i<m_Wires.GetSize(); i++)
    m_Electrics.RemoveCompData(&m_Wires[i].m_CompData);
  m_Wires.SetSize(0);
  }; 

//--------------------------------------------------------------------------

bool CElecCable::SetWire(int iWire, LPCTSTR sTag,
                         LPCTSTR sSrcTermStripName, LPCTSTR sSrcTerminalName,
                         LPCTSTR sDstTermStripName, LPCTSTR sDstTerminalName) 
  { 
  m_Wires.SetSize(Max(iWire+1, m_Wires.GetSize()));
  CEWire & W=m_Wires[iWire];
  W.m_sTag=sTag;
  W.m_sSrcTermStripName = sSrcTermStripName;
  W.m_sSrcTerminalName  = sSrcTerminalName;
  W.m_sDstTermStripName = sDstTermStripName;
  W.m_sDstTerminalName  = sDstTerminalName;
  //W.m_Area              = Area;
  //W.m_Length            = Length;
  return true;
  }

//--------------------------------------------------------------------------

void CElecCable::PostConnect(int IONo)
  {
  if (NoEIOs()==2)
    {
    //CETermStripCollectionArray TSCA;
    //ConnectElectrics(TSCA);
    }
  };

//--------------------------------------------------------------------------

void CElecCable::PreDisConnect(int IONo)
  {
  m_Electrics.RemoveAllComponents();
  //for (int i=0; i<m_Wires.GetSize(); i++)
  //  {
  //  m_Electrics.RemoveConnect(m_Wires[i].m_pConnect);
  //  delete m_Wires[i].m_pConnect; // this will disconnect connects
  //  }
  m_Wires.SetSize(0); 
  };

//--------------------------------------------------------------------------

// CTNode Overide
void CElecCable::OnElecComponentChange(eElecChanges EC, CECompData *pC)
  {
  for (int i=0; i<m_Wires.GetSize(); i++)
    {
    CEWire &W=m_Wires[i];
    if (&W.m_CompData==pC)
      {
      if (EC==eECC_Delete)
        {
        // the CEConnect is being deleted elsewhere
        //m_Wires.RemoveAt(i);
        }
      }
    }
  }; 

//--------------------------------------------------------------------------

void CElecCable::CollectElectrics(CNodeElectricsArray & TSCA)
  {
  FlwNode::CollectElectrics(TSCA);
  }

//--------------------------------------------------------------------------

void CElecCable::ConnectElectrics()
  {
  FlwNode::ConnectElectrics();

  if (NoEIOs()==2)
    {

    FlwNode * pSrcNd=Nd_Rmt(EIO1());
    FlwNode * pDstNd=Nd_Rmt(EIO1()+1);

    CNodeElectrics &SrcElectics=pSrcNd->m_Electrics;
    CNodeElectrics &DstElectics=pDstNd->m_Electrics;
    
    for (int i=0; i<m_Wires.GetSize(); i++)
      {
      CEWire &W=m_Wires[i];
      W.m_pSrcTerm=SrcElectics.Terminal(W.m_sSrcTermStripName, W.m_sSrcTerminalName);
      W.m_pDstTerm=DstElectics.Terminal(W.m_sDstTermStripName, W.m_sDstTerminalName);

      W.m_CompData.m_Z=CDComplex(gs_CondResistance[m_eCondMat]*Max(m_CabLength, W.m_Length)/GTZ(W.m_Area), 0);

      if (W.m_pSrcTerm && W.m_pDstTerm)
        {
        m_Electrics.SetCompData(this, 0, W.m_pSrcTerm, W.m_pDstTerm, &W.m_CompData);
        }
      else
        LogError(FullObjTag(), 0, "Wire %s -> %s not connected", (LPCTSTR)W.FullSrcName(), (LPCTSTR)W.FullDstName());
      }
    }
  };

//--------------------------------------------------------------------------

//void CElecCable::EvalCtrlActions()
//  {
//  }
//
////--------------------------------------------------------------------------
//
//void CElecCable::EvalCtrlStrategy()
//  {
//  }
//
//--------------------------------------------------------------------------

void CElecCable::EvalDiscrete()
  {
  const int DoDbg=0;
  if (NoEIOs()==2)
    {
    FlwNode * pSrcNd=Nd_Rmt(EIO1());
    FlwNode * pDstNd=Nd_Rmt(EIO1()+1);

    CNodeElectrics &SrcElectics=pSrcNd->m_Electrics;
    CNodeElectrics &DstElectics=pDstNd->m_Electrics;
    m_kVA=0;
    m_kW=0;
    m_CabV=0;

    if (DoDbg)
      dbgpln("----------------------------------------------------------------------------------------------- %s", m_sTag());
    for (int i=0; i<m_Wires.GetSize(); i++)
      {
      CEWire &W=m_Wires[i];
      W.m_pSrcTerm=SrcElectics.Terminal(W.m_sSrcTermStripName, W.m_sSrcTerminalName);
      W.m_pDstTerm=DstElectics.Terminal(W.m_sDstTermStripName, W.m_sDstTerminalName);

      m_kVA+=0.001*W.m_pSrcTerm->m_V.Mag()*W.m_CompData.m_I.Mag();
      m_kW+=0.001*W.m_pSrcTerm->m_V.Mag()*W.m_CompData.m_I.Mag()*cos(W.m_pSrcTerm->m_V.Ang()-W.m_CompData.m_I.Ang());

      if (DoDbg)
        dbgpln("%2i V:%10.2f<%5.1f I:%10.2f<%5.1f [%7.2f %7.4f] VA:%10.2f W:%10.2f", 
          i,
          W.m_pSrcTerm->m_V.Mag(), W.m_pSrcTerm->m_V.Ang(), 
          W.m_CompData.m_I.Mag(), W.m_CompData.m_I.Ang(),
          Rad2Deg(W.m_pSrcTerm->m_V.Ang()-W.m_CompData.m_I.Ang()),
          cos(W.m_pSrcTerm->m_V.Ang()-W.m_CompData.m_I.Ang()),
          m_kVA*1000, m_kW*1000);
 
      for (int j=0; j<m_Wires.GetSize(); j++)
        {
        if (j!=i)
          {
          CEWire &Wj=m_Wires[j];
          Wj.m_pSrcTerm=SrcElectics.Terminal(Wj.m_sSrcTermStripName, Wj.m_sSrcTerminalName);
          CDComplex DV=Wj.m_pSrcTerm->m_V-W.m_pSrcTerm->m_V;
          m_CabV=Max(m_CabV, DV.Mag());
          };
        }
      }
    }
  }

//--------------------------------------------------------------------------

flag CElecCable::GetModelAction(Strng & Tag, MdlActionArray & Acts)
  {
  //MdlAction M0 = {0, MAT_State, 0/*!PSB.On()*/, "On", 1};
  //Acts.SetSize(0);
  //Acts.SetAtGrow(0, M0);
  return true;
  };

//--------------------------------------------------------------------------

flag CElecCable::SetModelAction(Strng & Tag, MdlAction & Act)
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

dword CElecCable::ModelStatus()
  {
  dword Status=FlwNode::ModelStatus();
  bool VoltsPresent=false;
  for (int i=0; !VoltsPresent&& i<m_Wires.GetSize(); i++)
    VoltsPresent=m_Wires[i].m_CompData.VoltagePresent();
    //if (m_wires[i].m_connect.m_psrc)
    //  voltspresent=(m_wires[i].m_connect.m_psrc->m_v.mag()>=0.001);
  Status|=/*FNS_IsELnk|*/(VoltsPresent? FNS_On : FNS_Off);
  return Status;
  }

//--------------------------------------------------------------------------

flag CElecCable::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case  1: pS="E\tBad external tag references"; return 1;
    case  2: pS="E\tErrors in evaluating functions"; return 1;
    case  3: pS="E\tEngineering units invalid"; return 1;
    case  4: pS="E\tMismatched Terminals"; return 1;
    default:
      return FlwNode::CIStrng(No, pS);
    }
  }

//==========================================================================
