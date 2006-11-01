//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include <math.h>
#include <ctype.h>
#define  __ELECBASE_CPP
#include "sc_defs.h"
#include "sfe_base.h"
#include "elecbase.h"
#include "ndtree.h"
#include "dbgmngr.h"

XID xidVMag  = PwrXID(7000);
XID xidVAng  = PwrXID(7001);

#define dbgElecBase                 (1 && WITHDEBUG)

#if (dbgElecBase)
static CDbgMngr dbgConnectsAlloc    ("ElecBase",  "ConnectsAlloc");
static CDbgMngr dbgConnectsNew      ("ElecBase",  "ConnectsNew");
static CDbgMngr dbgConnectsOld      ("ElecBase",  "ConnectsOld");
static CDbgMngr dbgConfig           ("ElecBase",  "Config");
static CDbgMngr dbgConfigSummary    ("ElecBase",  "ConfigSummary");
static CDbgMngr dbgPathCurrent      ("ElecBase",  "PathCurrent");
static CDbgMngr dbgSolve            ("ElecBase",  "Solve");
static CDbgMngr dbgSolveSummary     ("ElecBase",  "SolveSummary");
static CDbgMngr dbgVoltage          ("ElecBase",  "Voltage");
static CDbgMngr dbgCurrentSummary   ("ElecBase",  "CurrentSummary");
static CDbgMngr dbgPaths            ("ElecBase",  "Paths");
static CDbgMngr dbgPathsT           ("ElecBase",  "PathsT");
static CDbgMngr dbgPathsC           ("ElecBase",  "PathsC");
static CDbgMngr dbgHexAdd           ("ElecBase",  "HexAdd");
#endif
//===========================================================================
//
//
//
//===========================================================================

inline void AddParallel(CDComplex Z1, CDComplex Z2, CDComplex &Z)
  {
  double a=(Z1.real()*Z2.real()-Z1.imag()*Z2.imag());
  double b=(Z1.real()*Z2.imag()+Z1.imag()*Z2.real());
  double c=(Z1.real()+Z2.real());
  double d=(Z1.imag()+Z2.imag());

  Z=CDComplex((a*c+b*d)/(c*c+d*d), (b*c-a*d)/(c*c+d*d));
  }

inline LPCTSTR AddStr(void*p, int DefWide=0)
  {
#if (dbgElecBase)
  static CString S[10];
  static long i=-1;
  i=(i+1)%10;
  if (dbgHexAdd())
    S[i].Format("[%08x]", p);
  else
    S[i].Format("%*.*s", DefWide, DefWide, "");
  return S[i];
#else
  return "";
#endif
  }

inline double Rad2Deg(double a) { return 180.0/PI*(a<0 ? a+2*PI:a); };

//===========================================================================
//
//
//
//===========================================================================

const double gs_CondResistance[]= //ohm.m^2/m
  {
    0.0178e-6,     
    0.029e-6,
  };  
DDBValueLst DDBCondMaterial[]=
  {
    {eCM_Copper,    "Copper"},
    {eCM_Aluminium, "Aluminium"},
    {0}
  };
//===========================================================================
//
//
//
//===========================================================================

long CETerminal::AddComponent(CEComponent * pC)
  {
  m_TrmComponents.Add(pC);
  OnElecComponentChange(eECC_Connect, pC->m_pCompData);
  return m_TrmComponents.GetSize()-1;
  };

long CETerminal::RemoveComponent(CEComponent * pC)
  {
  bool Done=false;
  for (int i=0; i<m_TrmComponents.GetSize(); i++)
    if (m_TrmComponents[i]==pC)
      {
      OnElecComponentChange(eECC_DisConnect, pC->m_pCompData);
      m_TrmComponents.RemoveAt(i);
      Done=true;
      break;
      }
  if (!Done)
    dbgpln("RemoveConnect :Not Found");

  return m_TrmComponents.GetSize()-1;
  };

long CETerminal::RemoveComponent(long i)
  {
  return RemoveComponent(m_TrmComponents[i]);
  };

//long CETerminal::RemoveAllConnects()
//  {
//  for (int c=0; c<m_TrmComponents.GetSize(); c++)
//    RemoveComponent(m_TrmComponents[c]);
//  return 0;
//  };
CEComponent * CETerminal::FindRemote(long Id, CETerminal * pTRmt)
  {
  for (int c=0; c<m_TrmComponents.GetSize(); c++)
    {
    CEComponent &C=*m_TrmComponents[c];
    if (C.m_Id==Id)
      {
      if (C.m_pSrc==this)
        {
        if (C.m_pDst==pTRmt)
          return &C;
        }
      else 
        {
        ASSERT(C.m_pDst==this);
        if (C.m_pSrc==pTRmt)
          return &C;
        }
      }
    }
  return NULL;
  };

CETerminal::~CETerminal() 
  {
  for (int i=0; i<m_TrmComponents.GetSize(); i++)
    delete m_TrmComponents[i];
  #if (dbgElecBase)
  if (dbgConnectsAlloc())
    dbgpln("~CETerminal %s", (LPCTSTR)FullName());
  #endif
  };

void CETerminal::OnElecComponentChange(eElecChanges EC, CECompData *pC)
  {
  m_pTStrip->m_pNd->OnElecComponentChange(EC, pC);
  }

//===========================================================================
//
//
//
//===========================================================================

void CECompData::SetLoad(double Pwr, double PwrFactor)
  {
  if (m_bIsNew)
    {
    CDComplex DV=m_pOwner->m_pDst->m_V-m_pOwner->m_pSrc->m_V;
    double Phi=-acos(Range(0.1,PwrFactor,1.0));
    double Zm=Max(0.001, Sqr(Max(220.0,DV.Mag()))*PwrFactor)/GTZ(Pwr*1000);
    double R=Zm*cos(Phi);
    double X=Zm*sin(Phi);

    CDComplex Z(R,X);
    Z*=Min(1.0, 0.99*OpenCctElecRes/Z.Mag()); // do not let it go open cct
    m_eType       = CECompData::eTypePwr;
    m_bAsLoad     = true;
    m_Z           = Z;

    m_V           = 0;
    m_dPwr        = Pwr;
    m_dPwrFactor  = PwrFactor;

    #if (dbgElecBase)
    if (dbgConfig())
      dbgpln("AddLoad       %-20s:{%4i} %10.2g %4.2f        %s%-25s --> %s%s", m_pOwner?m_pOwner->m_pNd->FullObjTag():"",
             m_pOwner?m_pOwner->m_nSolveCount:-2, Pwr, PwrFactor, 
             m_pOwner?AddStr(m_pOwner->m_pSrc):"", m_pOwner?m_pOwner->m_pSrc->FullName():"", 
             m_pOwner?AddStr(m_pOwner->m_pDst):"", m_pOwner?m_pOwner->m_pDst->FullName():"");
    #endif
    }
  else
    {
    //CDComplex DV=m_pOwner->m_pDst->m_V-m_pOwner->m_pSrc->m_V;
    //double Phi=-acos(Range(0.1,m_pCompData->m_dPwrFactor,1.0));
    //double Zm=Max(0.001, Sqr(Max(220.0,DV.Mag()))*m_pCompData->m_dPwrFactor)/GTZ(m_pCompData->m_dPwr*1000);
    //double R=Zm*cos(Phi);
    //double X=Zm*sin(Phi);

    //CDComplex Z(R,X);
    //Z*=Min(1.0, 0.99*OpenCctElecRes/Z.Mag()); // do not let it go open cct
    //m_Z           = Z*0.5;

    //m_V           = 0;
    m_dPwr        = Pwr;
    m_dPwrFactor  = PwrFactor;

    #if (dbgElecBase)
    if (dbgConfig())
      dbgpln("SetLoad       %-20s:{%4i} %10.2g %4.2f        %s%-25s --> %s%s", m_pOwner?m_pOwner->m_pNd->FullObjTag():"", 
            m_pOwner?m_pOwner->m_nSolveCount:-2, Pwr, PwrFactor, 
            m_pOwner?AddStr(m_pOwner->m_pSrc):"", m_pOwner?m_pOwner->m_pSrc->FullName():"", 
            m_pOwner?AddStr(m_pOwner->m_pDst):"", m_pOwner?m_pOwner->m_pDst->FullName():"");
    #endif
    }
  }

//---------------------------------------------------------------------------

void CECompData::SetImpedance(CDComplex Z, bool AsLoad)
  {
  if (m_bIsNew)
    {
    //pC->m_pCompData->SetImpedance(Z, AsLoad);
    m_Z       = Z;
    m_V       = 0;
    m_bAsLoad = AsLoad;
    #if (dbgElecBase)
    if (dbgConfig())
      dbgpln("AddImpedance  %-20s:{%4i} %10.2g %10.2gi %s%-25s --> %s%s", m_pOwner?m_pOwner->m_pNd->FullObjTag():"",
              m_pOwner?m_pOwner->m_nSolveCount:-2, Z.real(), Z.imag(), 
              m_pOwner?AddStr(m_pOwner->m_pSrc):"", m_pOwner?m_pOwner->m_pSrc->FullName():"", 
              m_pOwner?AddStr(m_pOwner->m_pDst):"", m_pOwner?m_pOwner->m_pDst->FullName():"");
    #endif
    }
  else
    {
    m_Z = Z;
    m_V = 0;
  
    #if (dbgElecBase)
    if (dbgConfig())
      dbgpln("SetImpedance  %-20s:{%4i} %10.2g %10.2gi %s%-25s --> %s%s", m_pOwner?m_pOwner->m_pNd->FullObjTag():"",
              m_pOwner?m_pOwner->m_nSolveCount:-2, Z.real(), Z.imag(), 
              m_pOwner?AddStr(m_pOwner->m_pSrc):"", m_pOwner?m_pOwner->m_pSrc->FullName():"", 
              m_pOwner?AddStr(m_pOwner->m_pDst):"", m_pOwner?m_pOwner->m_pDst->FullName():"");
    #endif
    }
  };

//---------------------------------------------------------------------------

void CECompData::SetSource(CDComplex V, CDComplex Z)
  {
  if (m_bIsNew)
    {
    m_Z       = Z;
    m_V       = V;
    //      pC->m_V       = CDComplex(std::polar(V,Phi));
    m_bAsLoad = false;

    #if (dbgElecBase)
    if (dbgConfig())
      dbgpln("AddSource     %-20s:{%4i} %10.2g %10.2gi %s%-25s --> %s%s", m_pOwner?m_pOwner->m_pNd->FullObjTag():"",
      m_pOwner?m_pOwner->m_nSolveCount:-2, Z.real(), Z.imag(), 
      m_pOwner?AddStr(m_pOwner->m_pSrc):"", m_pOwner?m_pOwner->m_pSrc->FullName():"", 
      m_pOwner?AddStr(m_pOwner->m_pDst):"", m_pOwner?m_pOwner->m_pDst->FullName():"");
    #endif
    }
  else
    {
    m_Z       = Z;
    m_V       = V;

    #if (dbgElecBase)
    if (dbgConfig())
      dbgpln("SetSource     %-20s:{%4i} %10.2g %10.2gi %s%-25s --> %s%s", m_pOwner?m_pOwner->m_pNd->FullObjTag():"",
      m_pOwner?m_pOwner->m_nSolveCount:-2, Z.real(), Z.imag(), 
      m_pOwner?AddStr(m_pOwner->m_pSrc):"", m_pOwner?m_pOwner->m_pSrc->FullName():"", 
      m_pOwner?AddStr(m_pOwner->m_pDst):"", m_pOwner?m_pOwner->m_pDst->FullName():"");
    #endif
    }

  };

//===========================================================================
//
//
//
//===========================================================================

CEComponent::~CEComponent() 
  { 
  #if (dbgElecBase)
  if (dbgConnectsAlloc())
    {
    dbgpln("~CEComponent %s --> %s", m_pSrc?(LPCTSTR)m_pSrc->FullName():"***", m_pDst?(LPCTSTR)m_pDst->FullName():"***");
    dbgindent(10);
    }
  #endif

  if (m_pNd)
    m_pNd->OnElecComponentChange(eECC_Delete,this->m_pCompData);
  if (m_pSrc)
    m_pSrc->RemoveComponent(this);
  m_pSrc=NULL;
  if (m_pDst)
    m_pDst->RemoveComponent(this);
  m_pDst=NULL;

  m_pCompData->m_pOwner=NULL;
  if (m_bOwnsCompData)
    delete m_pCompData;

  #if (dbgElecBase)
  if (dbgConnectsAlloc())
    dbgindent(-10);
  #endif
  }
    
LPCTSTR CEComponent::SrcName() 
  { 
  if (!m_pSrc)
    return "";
  static CString S[5];
  static long i=-1;
  i=(i+1)%5;
  S[i].Format("%s.%s.%s", SrcNdName(), SrcTSName(), SrcTName());
  return S[i];
  }
LPCTSTR CEComponent::DstName() 
  { 
  if (!m_pDst)
    return "";
  static CString S[5];
  static long i=-1;
  i=(i+1)%5;
  S[i].Format("%s.%s.%s", DstNdName(), DstTSName(), DstTName());
  return S[i];
  }

//===========================================================================
//
//
//
//===========================================================================

void CECompData::BuildDataDefn(DataDefnBlk & DDB, TaggedObject * pTagObj, long Id, LPCTSTR Tag)
  {
  if (Tag)
    DDB.BeginElement(pTagObj, Tag, NULL, Id);
  DDB.String   ("", "Src",      DC_,      "",     Id+xidConnSrc,    pTagObj, 0);
  DDB.String   ("", "Dst",      DC_,      "",     Id+xidConnDst,    pTagObj, 0);

  DDB.Double   ("", "Volts",    DC_V,     "V",    Id+xidConnVolts,  pTagObj, 0);
  DDB.Double   ("", "V.Phase",  DC_Ang,   "deg",  Id+xidConnVAng,   pTagObj, 0);
  DDB.Double   ("", "Amps",     DC_I,     "A",    Id+xidConnAmps,   pTagObj, 0);
  DDB.Double   ("", "I.Phase",  DC_Ang,   "deg",  Id+xidConnIAng,   pTagObj, 0);

  DDB.Double   ("", "VoltDrop", DC_V,     "V",    Id+xidConnVDrop,  pTagObj, 0);
  DDB.Double   ("", "HeatLoad", DC_Pwr,   "W",    Id+xidConnHtLoad, pTagObj, 0);
  DDB.Double   ("", "RAmps",    DC_I,     "A",    Id+xidConnRAmps,  pTagObj, isParmConstruct|InitHidden);
  DDB.Double   ("", "XAmps",    DC_I,     "A",    Id+xidConnXAmps,  pTagObj, isParmConstruct|InitHidden);
  DDB.Double   ("", "R",        DC_Ohms,  "Ohms", Id+xidConnR,      pTagObj, isParmConstruct|InitHidden);
  DDB.Double   ("", "X",        DC_Ohms,  "Ohms", Id+xidConnX,      pTagObj, isParmConstruct|InitHidden);
  DDB.Double   ("", "VSrc",     DC_V,     "V",    Id+xidConnV,      pTagObj, isParmConstruct|InitHidden);
  DDB.Double   ("", "VSrcPh",   DC_Ang,   "deg",  Id+xidConnPh,     pTagObj, isParmConstruct|InitHidden);
  };

flag CECompData::DataXchg(DataChangeBlk & DCB, TaggedObject * pTagObj, long Id)
  {
  if (DCB.lHandle>=xidConnFirst && DCB.lHandle<xidConnLast)
    {
    switch ((DCB.lHandle-xidConnFirst)%xidConnCount)
      {
      case xidConnSrc: 
        DCB.pC=m_pOwner ? (LPTSTR)m_pOwner->SrcName() : "";
        return 1;
      case xidConnDst: 
        DCB.pC=m_pOwner ? (LPTSTR)m_pOwner->DstName() : "";
        return 1;
      case xidConnVolts: 
        DCB.D=m_SrcV.Mag() > m_DstV.Mag() ? m_SrcV.Mag() : m_DstV.Mag();
        return 1;
      case xidConnVAng: 
        DCB.D=m_SrcV.Mag() > m_DstV.Mag() ? m_SrcV.Ang() : m_DstV.Ang();
        return 1;
      case xidConnAmps: 
        DCB.D=m_I.Mag();
        return 1;
      case xidConnIAng: 
        DCB.D=m_I.Ang()-m_SrcV.Ang();
        return 1;
      case xidConnVDrop: 
        DCB.D=/*(m_pDst&&m_pSrc) ?*/ AppliedVoltage().Mag()/* : 0.0*/;
        return 1;
      case xidConnHtLoad: 
        DCB.D=HeatLoad();
        return 1;
      case xidConnRAmps:
        DCB.D=m_I.real();
        return 1;
      case xidConnXAmps:
        DCB.D=m_I.imag();
        return 1;
      case xidConnR:
        DCB.D=m_Z.real();
        return 1;
      case xidConnX:
        DCB.D=m_Z.imag();
        return 1;
      case xidConnV:
        DCB.D=m_V.Mag();
        return 1;
      case xidConnPh:
        DCB.D=m_V.Ang();
        return 1;
      }
    }
  return 0;
  };

//===========================================================================
//
//
//
//===========================================================================

CETermStrip::~CETermStrip() 
  {
  for (int i=0; i<m_Terms.GetSize(); i++)
    delete m_Terms[i];
  #if (dbgElecBase)
  if (dbgConnectsAlloc())
    dbgpln("~CETermStrip %s", (LPCTSTR)FullName());
  #endif
  };

//long CETermStrip::RemoveAllConnects()
//  {
//  for (int t=0; t<m_Terms.GetSize(); t++)
//    m_Terms[t]->RemoveAllConnects();
//  return 0;
//  };
LPCTSTR   CETermStrip::NdName()   { return m_pNd->Tag(); };
CString   CETermStrip::FullName() { return m_sName+":"+m_pNd->Tag(); };

void CETermStrip::SetDoInit(bool Init)
  {
  for (int t=0; t<m_Terms.GetSize(); t++)
    m_Terms[t]->m_bDoInit=Init;
  };

void CETermStrip::SetConfiguration(eTSConfigurations Cfg)//, double NominalVolts);
  {
  //m_dNomV=NominalVolts;

  m_eCfg=Cfg;

  #if (dbgElecBase)
  if (dbgConnectsAlloc())
    dbgpln("CETermStrip::SetConfiguration %i", Cfg);
  #endif
  switch (m_eCfg)
    {
    case eCT_1Ph2W:
      {
      SetTerminalCount(2);
      //m_Terms.SetSize(2);
      m_Terms[0]->m_sName="L";
      m_Terms[1]->m_sName="N";
      return;
      }
    case eCT_3Ph3W:
      {
      SetTerminalCount(3);
      // only enforce the name - retain all other settings
      m_Terms[0]->m_sName="R";
      m_Terms[1]->m_sName="W";
      m_Terms[2]->m_sName="B";
      return;
      }
    case eCT_3Ph4W:
      {
      SetTerminalCount(4);
      // only enforce the name - retain all other settings
      m_Terms[0]->m_sName="R";
      m_Terms[1]->m_sName="W";
      m_Terms[2]->m_sName="B";
      m_Terms[3]->m_sName="N";
      return;
      }
    }
  };

//---------------------------------------------------------------------------

void CETermStrip::SetWireNames(CETermStripDefn & Terms)
  {
  long N=Terms.TerminalCount();
  SetTerminalCount(N);
  for (int i=0; i<N; i++)
    m_Terms[i]->m_sName=Terms[i].m_sName;
  };

//---------------------------------------------------------------------------

void CETermStrip::SetLoad(double Power, double PwrFactor)
  {
  ASSERT(m_pTSC);
  switch (m_eCfg)
    {
    case eCT_1Ph2W:
      {
      CETerminal *pL=Find("L");
      CETerminal *pN=Find("N");
      if (pN && pL)
        {
        m_pTSC->SetLoad(0, pN, pL, Power, PwrFactor);
        }
      return;
      }
    case eCT_3Ph3W:
      {
      CETerminal *pR=Find("R");
      CETerminal *pW=Find("W");
      CETerminal *pB=Find("B");
      double P=Power/3;
      if (pR && pW && pB)
        {
        m_pTSC->SetLoad(0, pW, pR, P, PwrFactor);
        m_pTSC->SetLoad(0, pB, pW, P, PwrFactor);
        m_pTSC->SetLoad(0, pR, pB, P, PwrFactor);
        }
      return;
      }
    case eCT_3Ph4W:
      {
      CETerminal *pR=Find("R");
      CETerminal *pW=Find("W");
      CETerminal *pB=Find("B");
      CETerminal *pN=Find("N");
      double P=Power/3;
      if (pN && pR && pW && pB)
        {
        m_pTSC->SetLoad(0, pN, pR, P, PwrFactor);
        m_pTSC->SetLoad(0, pN, pW, P, PwrFactor);
        m_pTSC->SetLoad(0, pN, pB, P, PwrFactor);
        }
      return;
      }
    }
  };

//---------------------------------------------------------------------------

void CETermStrip::SetSource(double LineVolts, double Phase, CDComplex Z)
  {
  ASSERT(m_pTSC);
  switch (m_eCfg)
    {
    case eCT_1Ph2W:
      {
      CETerminal *pL=Find("L");
      CETerminal *pN=Find("N");
      if (pN && pL)
        {
        m_pTSC->SetSource(0, pN, pL, LineVolts, Phase, Z.real(), Z.imag());
        m_pTSC->SetReferenceVoltage(pN, 0.0, 0.0);
        }
      return;
      }
    case eCT_3Ph3W:
      {
      CETerminal *pR=Find("R");
      CETerminal *pW=Find("W");
      CETerminal *pB=Find("B");
      if (pR && pW && pB)
        {
        m_pTSC->SetSource(0, pW, pR, LineVolts, Phase,        Z.real(), Z.imag());
        m_pTSC->SetSource(0, pB, pW, LineVolts, Phase+2*PI/3.0, Z.real(), Z.imag());
        m_pTSC->SetSource(0, pR, pB, LineVolts, Phase+4*PI/3.0, Z.real(), Z.imag());
        }
      return;
      }
    case eCT_3Ph4W:
      {
      CETerminal *pR=Find("R");
      CETerminal *pW=Find("W");
      CETerminal *pB=Find("B");
      CETerminal *pN=Find("N");
      if (pN && pR && pW && pB)
        {
        m_pTSC->SetSource(0, pN, pR, LineVolts/Sqrt(3.0), Phase,          Z.real(), Z.imag());
        m_pTSC->SetSource(0, pN, pW, LineVolts/Sqrt(3.0), Phase+2*PI/3.0, Z.real(), Z.imag());
        m_pTSC->SetSource(0, pN, pB, LineVolts/Sqrt(3.0), Phase+4*PI/3.0, Z.real(), Z.imag());
        m_pTSC->SetReferenceVoltage(pN, 0.0, 0.0);
        }
      return;
      }
    }
  };

//---------------------------------------------------------------------------

bool CETermStrip::IsPowerStd()
  {
  switch (m_eCfg)
    {
    case eCT_1Ph2W:
    case eCT_3Ph3W:
    case eCT_3Ph4W:
      return true;
    }
  return false;
  };

//---------------------------------------------------------------------------

bool CETermStrip::PowerPresent()
  {
  switch (m_eCfg)
    {
    case eCT_1Ph2W:
      {
      CETerminal *pL=Find("L");
      CETerminal *pN=Find("N");
      return (pN && pL) ? (CDComplex(pL->m_V-pN->m_V).Mag()>1) : false;
      }
    case eCT_3Ph3W:
      {
      CETerminal *pR=Find("R");
      CETerminal *pW=Find("W");
      return (pR && pW) ? (CDComplex(pR->m_V-pW->m_V).Mag()>1) : false;
      }
    case eCT_3Ph4W:
      {
      CETerminal *pR=Find("R");
      CETerminal *pW=Find("W");
      return (pR && pW) ? (CDComplex(pR->m_V-pW->m_V).Mag()>1) : false;
      }
    }
  return false;
  };

//---------------------------------------------------------------------------

void CETermStrip::BuildDataDefn(DataDefnBlk & DDB, long TSIndex, bool WithArray)
  {
  if (WithArray)
    DDB.BeginArray(m_pNd, (LPTSTR)(LPCTSTR)Name(), "E_Terminal", TerminalCount(), 0, DDB_OptPage);
  for (int j=0; j<TerminalCount(); j++)
    {
    CETerminal & T=*Terminal(j);
    DDB.BeginElement(m_pNd, WithArray ? T.Name():T.FullName(), NULL, TSIndex*10000+j);
    DDB.Double("", "Volts", DC_V,   "V", xidVMag, m_pNd, 0);
    DDB.Double("", "Phase", DC_Ang, "deg", xidVAng, m_pNd, 0);
    }
  if (WithArray)
    DDB.EndArray();
  };

//---------------------------------------------------------------------------

flag CETermStrip::DataXchg(DataChangeBlk & DCB, long TSIndex)
  {
  if (DCB.dwUserInfo/10000!=TSIndex)
    return 0;
  switch (DCB.lHandle)
    {
    case xidVMag:
      DCB.D=Terminal(DCB.dwUserInfo%10000)->m_V.Mag();
      return 1;
    case xidVAng:
      DCB.D=Terminal(DCB.dwUserInfo%10000)->m_V.Ang();
      return 1;
    }
  return 0;
  };

//===========================================================================
//
//
//
//===========================================================================

CNodeElectrics::CNodeElectrics(CTNode * pNd) : m_pNd(pNd) 
  {
  #if (dbgElecBase)
  if (dbgConnectsAlloc())
    dbgpln("CNodeElectrics");
  #endif
  m_pSolveCount=NULL;
  m_lTermStripCount=0; 
  m_lTopoCount=0;
  }

CNodeElectrics::~CNodeElectrics()
  {
  //#if (dbgElecBase)
  //if (dbgComponentsAlloc())
  //  dbgpln("~CNodeElectrics");
  //#endif
  for (int i=0; i<m_Components.GetSize(); i++)
    //if (!m_Components[i]->m_bExternal)
    delete m_Components[i];
  }

//---------------------------------------------------------------------------

//void CNodeElectrics::BuildDataDefn(DataDefnBlk & DDB)
//  {
//  DDB.BeginArray(m_pNd, "TStrips", "TermStrip", TermStripCount());
//  for (int i=0; i<TermStripCount(); i++)
//    {
//    CETermStrip & TS=*TermStrip(i);
//    DDB.BeginArray(m_pNd, (LPTSTR)(LPCTSTR)TS.Name(), "Terminal", TS.TerminalCount(), 0, DDB_OptPage);
//    for (int j=0; j<TS.TerminalCount(); j++)
//      {
//      CETerminal & T=*TS.Terminal(j);
//      DDB.BeginElement(m_pNd, T.Name(), NULL, i*10000+j);
//      DDB.Double("Volts", "V",  DC_V,   "V", xidVMag, m_pNd, 0);
//      DDB.Double("Phase", "ø",  DC_Ang, "deg", xidVAng, m_pNd, 0);
//      }
//    DDB.EndArray();
//    }
//  DDB.EndArray();
//  };
//
////---------------------------------------------------------------------------
//
//flag CNodeElectrics::DataXchg(DataChangeBlk & DCB)
//  {
//  long i,j;
//  switch (DCB.lHandle)
//    {
//    case xidVMag:
//      i=DCB.dwUserInfo/10000;
//      j=DCB.dwUserInfo%10000;
//      DCB.D=TermStrip(i)->Terminal(j)->m_V.Mag();
//      return 1;
//    case xidVAng:
//      i=DCB.dwUserInfo/10000;
//      j=DCB.dwUserInfo%10000;
//      DCB.D=TermStrip(i)->Terminal(j)->m_V.Ang();
//      return 1;
//    }
//  return 0;
//  };

//---------------------------------------------------------------------------

LPCTSTR CNodeElectrics::NodeName() { return m_pNd->FullObjTag(); };

void CNodeElectrics::AddComponent(CTNode * pNd, CETerminal * Src, CETerminal * Dst, CEComponent * pComponent)//, bool External)
  {
  pComponent->m_pNd=pNd;
  pComponent->m_pSrc=Src;
  pComponent->m_pDst=Dst;
  //pConnect->m_bExternal=External;

  m_Components.Add(pComponent);

  pNd->OnElecComponentChange(eECC_Add, pComponent->m_pCompData);

  Src->AddComponent(pComponent);
  Dst->AddComponent(pComponent);

  #if (dbgElecBase)
  if (dbgConnectsNew())
    dbgpln("AddConnection %-20s:{%4i}  %s             %s%-25s --> %s%s", NodeName(), pComponent->m_nSolveCount,
            AddStr(pComponent,10), AddStr(Src), Src->FullName(), AddStr(Dst), Dst->FullName());
  #endif
  
  //return pConnect;
  };

//---------------------------------------------------------------------------

void CNodeElectrics::TouchComponent(CEComponent * pConnect)
  {
  pConnect->m_nSolveCount=*m_pSolveCount;
  }

//---------------------------------------------------------------------------

void CNodeElectrics::RemoveComponent(CEComponent * pConnect)
  {
  if (pConnect==NULL)
    {
    dbgpln("RemoveConnect : Connect has no Owner");
    return;
    }
  CETerminal * Src=pConnect->m_pSrc;
  CETerminal * Dst=pConnect->m_pDst;

  //#if (dbgElecBase)
  //if (dbgConnectsNew())
  //  dbgpln("RemoveConnect :%s             %s%-25s --> %s%s", AddStr(pConnect,10), AddStr(Src), Src?Src->FullName():"", AddStr(Dst), Dst?Dst->FullName():"");
  //#endif

  if (Src)
    Src->RemoveComponent(pConnect);
  if (Dst)
    Dst->RemoveComponent(pConnect);
  pConnect->m_pSrc=NULL;
  pConnect->m_pDst=NULL;

  bool Done=false;
  for (int i=0; i<m_Components.GetSize(); i++)
    if (m_Components[i]==pConnect)
      {
      m_Components.RemoveAt(i);
      //if (!pConnect->m_bExternal)
      delete pConnect; 
      Done=true;
      break;
      }
  if (!Done)
    dbgpln("RemoveConnect :Not Found");
  };

//---------------------------------------------------------------------------

void CNodeElectrics::RemoveAllComponents()
  {
  //for (int i=m_Components.GetSize()-1; i>=0; i--)
  //  RemoveComponent(m_Components[i]);
  while (m_Components.GetSize())
    RemoveComponent(m_Components[0]);
  };

//---------------------------------------------------------------------------

CEComponent * CNodeElectrics::GetComponent(long Id, CETerminal *Src, CETerminal *Dst, bool AllowNew, bool * IsNew)
  {
  CEComponent *pSD=Src->FindRemote(Id, Dst); 
  CEComponent *pDS=Dst->FindRemote(Id, Src); 

  if (IsNew)
    *IsNew=false;
  if (pSD && pDS)
    {
    pSD->m_pCompData->m_bIsNew=false;
    return pSD;
    }
  else if (AllowNew && pSD==NULL && pDS==NULL)
    {
    if (IsNew)
      *IsNew=true;
    CEComponent *pC=new CEComponent(Id);
    //pC->SetId(Id)Id, 
    AddComponent(m_pNd, Src, Dst, pC);//, false);
    pC->m_pCompData->m_bIsNew=true;
    return pC;
    }
  return NULL;
  }

//---------------------------------------------------------------------------

void CNodeElectrics::SetCompData(CTNode * pNd, long Id, CETerminal * Src, CETerminal * Dst, CECompData * pConnect)
  {
  bool IsNew;
  CEComponent *pC=GetComponent(Id, Src, Dst, true, &IsNew);
  if (pC->m_bOwnsCompData)
    delete pC->m_pCompData;
  pC->m_pCompData=pConnect;
  pC->m_bOwnsCompData=false;
  pC->m_pNd=pNd;
  pConnect->m_pOwner=pC;
  pC->m_nSolveCount=*m_pSolveCount;
  };

//---------------------------------------------------------------------------

void CNodeElectrics::TouchCompData(CECompData * pConnect)
  {
  pConnect->m_pOwner->m_nSolveCount=*m_pSolveCount;
  };

//---------------------------------------------------------------------------

void CNodeElectrics::RemoveCompData(CECompData * pConnect)
  {
  RemoveComponent(pConnect->m_pOwner);
  };

//---------------------------------------------------------------------------

CECompData * CNodeElectrics::GetCompData(long Id, CETerminal *Src, CETerminal *Dst, bool AllowNew, bool * IsNew)
  {
  CEComponent *pC=GetComponent(Id, Src, Dst, AllowNew, IsNew);
  return pC ? pC->m_pCompData : NULL;
  }

//---------------------------------------------------------------------------

CECompData * CNodeElectrics::SetImpedance(long Id, CETerminal *Src, CETerminal *Dst, CDComplex Z, bool AsLoad/*=false*/)
  {
  bool IsNew;
  CEComponent *pC=GetComponent(Id, Src, Dst, true, &IsNew);
  if (pC)
    {
    pC->m_nSolveCount=*m_pSolveCount;
    pC->m_pCompData->SetImpedance(Z, AsLoad);
    }
  else
    {
    INCOMPLETECODE();
    }
  return pC ? pC->m_pCompData : NULL;
  };

//---------------------------------------------------------------------------


CECompData * CNodeElectrics::SetLoad(long Id, CETerminal *Src, CETerminal *Dst, double Pwr, double PwrFactor)
  {
  bool IsNew;
  //CDComplex cPwr(Pwr, Pwr*atan(Phi));

  CEComponent *pC=GetComponent(Id, Src, Dst, true, &IsNew);
  if (pC)
    {
    pC->m_nSolveCount=*m_pSolveCount;
    pC->m_pCompData->SetLoad(Pwr, PwrFactor);
    }
  else
    {
    INCOMPLETECODE();
    }

  return pC ? pC->m_pCompData : NULL;
  };

//---------------------------------------------------------------------------

CECompData * CNodeElectrics::SetSource(long Id, CETerminal *Src, CETerminal *Dst, CDComplex V, CDComplex Z)
  {
  bool IsNew;
  CEComponent *pC=GetComponent(Id, Src, Dst, true, &IsNew);
  if (pC)
    {
    pC->m_nSolveCount=*m_pSolveCount;
    pC->m_pCompData->SetSource(V, Z);
    }
  else
    {
    INCOMPLETECODE();
    }
  return pC ? pC->m_pCompData : NULL;
  };

//---------------------------------------------------------------------------
//
//void CNodeElectrics::SetTransformer(CETerminal * PriSrc, CETerminal * PriDst, 
//                                           CETerminal * SecSrc, CETerminal * SecDst, 
//                                           double TurnsRatio, 
//                                           double PriR, double PriX, double SecR, double SecX)
//  {
//  bool IsNewP, IsNewS;
//  CEComponent *pPriC=GetComponent(PriSrc, PriDst, true, &IsNewP);
//  CEComponent *pSecC=GetComponent(SecSrc, SecDst, true, &IsNewS);
//  if (pPriC)
//    {
//    pPriC->m_nSolveCount=*m_pSolveCount;
//    if (IsNewP)
//      {
//      pPriC->m_Z       = CDComplex(1e10,0); // Initial Guess
//      pPriC->m_ZInt    = CDComplex(PriR,PriX);
//      pPriC->m_V       = 0;
//      pPriC->m_bAsLoad = true;
//      pPriC->m_eType   = CEComponent::eTypeReflectZ;
//      pPriC->m_pReflectConn=pSecC;
//      
//      #if (dbgElecBase)
//      if (dbgConfig())
//        dbgpln("AddXfrPri     %-20s:{%4i} %10.2g %10.2gi %s%-25s --> %s%s", NodeName(),
//                pPriC->m_nSolveCount, PriR, PriX, AddStr(PriSrc), PriSrc->FullName(), AddStr(PriDst), PriDst->FullName());
//      #endif
//      }
//    else
//      {
//      pPriC->m_ZInt       = CDComplex(PriR,PriX);
//      pPriC->m_V          = 0;
//    
//      #if (dbgElecBase)
//      if (dbgConfig())
//        dbgpln("SetXfrPri     %-20s:{%4i} %10.2g %10.2gi %s%-25s --> %s%s", NodeName(),
//                pPriC->m_nSolveCount, PriR, PriX, AddStr(PriSrc), PriSrc->FullName(), AddStr(PriDst), PriDst->FullName());
//      #endif
//      }
//    }
//  else
//    {
//    _asm int 3;
//    }
//  if (pSecC)
//    {
//    pSecC->m_nSolveCount=*m_pSolveCount;
//    if (IsNewP)
//      {
//      pSecC->m_Z       = CDComplex(SecR,SecX);
//      pSecC->m_V       = CDComplex(1.0,0.0); // Initial Guess
//      pSecC->m_bAsLoad = false;
//      pSecC->m_eType   = CEComponent::eTypeReflectV;
//      pSecC->m_pReflectConn=pPriC;
//      
//      #if (dbgElecBase)
//      if (dbgConfig())
//        dbgpln("AddXfrSec     %-20s:{%4i} %10.2g %10.2gi %s%-25s --> %s%s", NodeName(),
//                pSecC->m_nSolveCount, SecR, SecX, AddStr(SecSrc), SecSrc->FullName(), AddStr(SecDst), SecDst->FullName());
//      #endif
//      }
//    else
//      {
//      pSecC->m_Z       = CDComplex(SecR,SecX);
//    
//      #if (dbgElecBase)
//      if (dbgConfig())
//        dbgpln("SetXfrSec     %-20s:{%4i} %10.2g %10.2gi %s%-25s --> %s%s", NodeName(),
//                pSecC->m_nSolveCount, SecR, SecX, AddStr(SecSrc), SecSrc->FullName(), AddStr(SecDst), SecDst->FullName());
//      #endif
//      }
//    }
//  else
//    {
//    _asm int 3;
//    }
//  };

//---------------------------------------------------------------------------

void CNodeElectrics::SetReferenceVoltage(CETerminal *Term, CDComplex V)
  {
  //Terminal &Trm=Terminal(TermId); 
  Term->SetRefVoltage(V);
  };

//---------------------------------------------------------------------------

bool CEComponent::CalcImpedance()
  {
  if (m_pCompData->m_eType != CECompData::eTypePwr)
    return true;
  CECompData &CD=*m_pCompData;
  //CD.m_SrcV=m_pSrc->m_V;
  //CD.m_DstV=m_pDst->m_V;

  double Phi=acos(Range(0.1,m_pCompData->m_dPwrFactor,1.0));
  double Zm=Max(0.001, Sqr(CD.AppliedVoltage().Mag())*CD.m_dPwrFactor)/GTZ(CD.m_dPwr*1000);
  double R=Zm*cos(Phi);
  double X=Zm*sin(Phi);

  CDComplex Z(R,X);
  Z*=Min(1.0, 0.99*OpenCctElecRes/Z.Mag()); // do not let it go open cct

  CDComplex dZ=m_pCompData->m_Z-Z;
  bool OK=dZ.Mag()<0.001;
  if (0)
    {
    if (Z.Mag()<m_pCompData->m_Z.Mag()) // decreasing - go slow
      m_pCompData->m_Z=m_pCompData->m_Z*0.9+Z*0.1;
    else
      m_pCompData->m_Z=Z;
    }
  else
    m_pCompData->m_Z=Z;
  return OK;
  }

//---------------------------------------------------------------------------

bool CEComponent::CalcPathCurrent(CEPathHelper &PH)
  {
  bool Converged=true;
  CDComplex Zt=0;
  CEComponent *pPwrC=NULL;
  long       nPwrC=0;
  for (int i=0; i<PH.m_lTPathLen; i++)
    {
    CEComponent &C=*PH.m_CPath[i];
    if (C.m_pCompData->m_eType == CECompData::eTypePwr)
      {
      nPwrC++;
      pPwrC=PH.m_CPath[i];
      }
    else
      Zt+=C.m_pCompData->m_Z;

    #if (dbgElecBase)
    if (dbgPathCurrent())
      {
      if (i==0)
        dbgp(" %4i) V:(%10.2f<%3i)",i, C.m_pCompData->m_V.Mag(), (long)Rad2Deg(C.m_pCompData->m_V.Ang()));
      else
        dbgp(" %4i) %*s",i, 5+10+3,"");
      dbgp(C.m_pCompData->m_Z.Mag()>1e6?" Z:(%10.2g,%10.2gi)":" Z:(%10.2f,%10.2fi)", C.m_pCompData->m_Z.real(), C.m_pCompData->m_Z.imag());
      dbgp(Zt.Mag()>1e6?" Zt:(%10.2g,%10.2gi)":" Zt:(%10.2f,%10.2fi)", Zt.real(), Zt.imag());
      if (C.m_pDst==PH.m_TPath[i])
        dbgp(" > %-25s > %s", C.SrcName(), C.DstName());
      else
        dbgp(" > %-25s > %s", C.DstName(), C.SrcName());
      dbgpln("");
      }
    #endif
    }

  if (pPwrC && nPwrC>0)
    {
    pPwrC->CalcImpedance();
    
    if (pPwrC->m_pCompData->m_Z.Mag()<Zt.Mag()) // Max Power Point
      {
      pPwrC->m_pCompData->m_Z=Zt;
      }
    Zt+=pPwrC->m_pCompData->m_Z;
    }

  m_pCompData->m_Zt=Zt;
  CDComplex I=PH.m_CPath[0]->m_pCompData->m_V/Zt;
  if (PH.m_CPath[0]->m_pDst!=PH.m_TPath[0])
    I*=-1;

  #if (dbgElecBase)
  if (dbgPathCurrent())
    dbgpln(" Calc) I:(%10.4f<%3i) <<<<<<<<<<<", I.Mag(), (long)Rad2Deg(I.Ang()));
  #endif

  for (i=0; i<PH.m_lTPathLen; i++)
    {
    CEComponent &C=*PH.m_CPath[i];
    if (C.m_pDst==PH.m_TPath[i])
      C.m_pCompData->m_I+=I;
    else
      C.m_pCompData->m_I-=I;
    C.m_lPathCount++;
    
    #if (dbgElecBase)
    if (dbgPathCurrent())
      {
      dbgp(" %4i) I:(%10.4f<%3i)",i, C.m_pCompData->m_I.Mag(), (long)Rad2Deg(C.m_pCompData->m_I.Ang()));
      CETerminal &T=*PH.m_TPath[i];
      CETermStrip &TS=*T.m_pTStrip;    
      if (C.m_pDst==PH.m_TPath[i])
        dbgp(" %-25s > %s", C.SrcName(), C.DstName());
      else
        dbgp(" %-25s > %s", C.DstName(), C.SrcName());

      dbgpln("");
      }
    #endif
    }
  return Converged;
  }

//---------------------------------------------------------------------------

bool CETerminal::FindPaths(CEComponent *pC, CEPathHelper &PH, long Lvl)
  {
//  m_bDone=false; // in preparation for CalcVolts
  #if (dbgElecBase)
  if (dbgPathsT())
    dbgpln("Trm [%3i]: %s Ld:%2i %s.%s", Lvl, m_bBusy?"Busy":"    ", PH.m_nLoadCount, m_pTStrip->m_pNd->Tag(), FullName());
  #endif
  if (m_bBusy)
    return false;
  PH.m_TPath.SetAtGrow(PH.m_lTPathLen++, this);
  for (int i=0; i<m_TrmComponents.GetSize(); i++)
    {
    CEComponent &C=*m_TrmComponents[i];
    if (&C!=pC)
      {
      m_bBusy=true;
      C.FindPaths(this, PH, Lvl+1);
      m_bBusy=false;
      }
    }
  PH.m_lTPathLen--;
  return true;
  }

//---------------------------------------------------------------------------

bool CEComponent::FindPaths(CETerminal *pT, CEPathHelper &PH, long Lvl)
  {
  #if (dbgElecBase)
  if (dbgPathsC())
    dbgpln("Conn[%3i]: %s       %s > %s", Lvl, "    ", SrcName(), DstName());
  #endif

  if (PH.m_pVoltSrc==this)
    {
    #if (dbgElecBase)
    if (dbgPaths())
      dbgpln("Path %i ----------",PH.m_iCurrentPathNo);
    #endif
    //ASSERT(FALSE);
    CalcPathCurrent(PH);
    PH.m_iCurrentPathNo++;
    return false;
    }

  if (m_pCompData->m_bAsLoad)
    {
    if (PH.m_nLoadCount>=1)
      return false;
    PH.m_nLoadCount++;
    }


  if (m_pCompData->m_Z.Mag()<OpenCctElecRes)
    {
    PH.m_CPath.SetAtGrow(PH.m_lCPathLen++, this);
    if (m_pSrc!=pT)
      m_pSrc->FindPaths(this, PH, Lvl+1);
    else
      m_pDst->FindPaths(this, PH, Lvl+1);
    PH.m_lCPathLen--;
    }

  if (m_pCompData->m_bAsLoad)
    PH.m_nLoadCount--;

  return true;
  }

//---------------------------------------------------------------------------

bool CETerminal::CalcVolts(CEComponent *pC, CEPathHelper &PH)
  {
  bool Solved=true;
  PH.m_TPath.SetAtGrow(PH.m_lTPathLen++, this);
  for (int i=0; i<m_TrmComponents.GetSize(); i++)
    {
    CEComponent &C=*m_TrmComponents[i];
    if (&C!=pC)
      Solved=C.CalcVolts(this, PH) && Solved;
    }
  PH.m_lTPathLen--;
  return Solved;
  }

bool CEComponent::CalcVolts(CETerminal *pT, CEPathHelper &PH)
  {
  bool Solved=true;
  if (m_pCompData->m_Z.Mag()>=OpenCctElecRes)
    return Solved;

  PH.m_CPath.SetAtGrow(PH.m_lCPathLen++, this);
  if (m_pSrc!=pT)
    {
    //dbgpln(" CVS@:%s %s    ", m_pSrc->m_bDone?"Done":"    ", m_pSrc->FullName());
    if (!m_pSrc->m_bDone)
      {
      m_pSrc->m_bDone=true;
      CDComplex dV(m_pSrc->m_V);
      m_pSrc->m_V=m_pDst->m_V-(m_pCompData->m_V-(m_pCompData->m_I*m_pCompData->m_Z));
      dV-=m_pSrc->m_V;

      #if (dbgElecBase)
      if (dbgVoltage())
        {
        dbgp("CalcV: ");
        dbgp(" Vd:%10.2f<%3i ", m_pDst->m_V.Mag(), (long)Rad2Deg(m_pDst->m_V.Ang()));
        dbgp(" V:%10.2f<%3i ", m_pCompData->m_V.Mag(), (long)Rad2Deg(m_pCompData->m_V.Ang()));
        dbgp(" I:%10.2f<%3i ", m_pCompData->m_I.Mag(), (long)Rad2Deg(m_pCompData->m_I.Ang()));
        dbgp(m_pCompData->m_Z.Mag()>1e6?" Z:(%10.2g,%10.2g)":" Z:(%10.2f,%10.2f)", m_pCompData->m_Z.real(),m_pCompData->m_Z.imag());
        dbgp(" Vs:%10.2f<%3i ", m_pSrc->m_V.Mag(), (long)Rad2Deg(m_pSrc->m_V.Ang()));
        dbgp(" dV %10.2f ", dV.Mag());
        dbgp(" D:%-25s >> ", DstName());
        dbgp(" S:%s    ", SrcName());
        dbgpln("");  
        }
      #endif
      
      if (!m_pSrc->CalcVolts(this, PH) || (dV.Mag()>0.001))
        Solved=false;
      }
    }
  else
    {
    //dbgpln(" CVD@:%s %s    ", m_pDst->m_bDone?"Done":"    ", m_pDst->FullName());
    if (!m_pDst->m_bDone)
      {
      m_pDst->m_bDone=true;
      CDComplex dV(m_pDst->m_V);
      m_pDst->m_V=m_pSrc->m_V+(m_pCompData->m_V-(m_pCompData->m_I*m_pCompData->m_Z));
      dV-=m_pDst->m_V;

      #if (dbgElecBase)
      if (dbgVoltage())
        {
        dbgp("CalcV: ");
        dbgp(" Vs:%10.2f<%3i ", m_pSrc->m_V.Mag(), (long)Rad2Deg(m_pSrc->m_V.Ang()));
        dbgp(" V:%10.2f<%3i ", m_pCompData->m_V.Mag(), (long)Rad2Deg(m_pCompData->m_V.Ang()));
        dbgp(" I:%10.2f<%3i ", m_pCompData->m_I.Mag(), (long)Rad2Deg(m_pCompData->m_I.Ang()));
        dbgp(m_pCompData->m_Z.Mag()>1e6?" Z:(%10.2g,%10.2g)":" Z:(%10.2f,%10.2f)", m_pCompData->m_Z.real(),m_pCompData->m_Z.imag());
        dbgp(" Vd:%10.2f<%3i ", m_pDst->m_V.Mag(), (long)Rad2Deg(m_pDst->m_V.Ang()));
        dbgp(" dV %10.2f ", dV.Mag());
        dbgp(" S:%-25s >> ", SrcName());
        dbgp(" D:%s    ", DstName());
        dbgpln("");  
        }
      #endif

      if (!m_pDst->CalcVolts(this, PH) ||(dV.Mag()>0.001))
        Solved=false;
      }
    }

  m_pCompData->m_SrcV=m_pSrc->m_V;
  m_pCompData->m_DstV=m_pDst->m_V;
  
  PH.m_lCPathLen--;
  return Solved;
  }

//===========================================================================
//
//
//
//===========================================================================

CETermStripArray &CETermStripArray::operator=(const CETermStripDefnArray & D)
  {
  // NBNB Connects ??
  // Remove those which are no longer needed
  for (int i=0; i<GetSize(); i++)
    {
    CETermStrip &TS=*ElementAt(i);
    for (int j=0; j<D.GetSize(); j++)
      if (_stricmp(TS.Name(), D[j].Name())==0)
        break;
    if (j>=D.GetSize())
      {// this element is not needed
      delete ElementAt(i);
      RemoveAt(i);
      i--; // this will be reincremented
      }
    }

  // Add the new ones
  for (i=0; i<D.GetSize(); i++)
    {
    const CETermStripDefn &TSD=D[i];
    for (int j=0; j<GetSize(); j++)
      if (_stricmp(TSD.Name(), ElementAt(j)->Name())==0)
        break;
    if (j>=GetSize())
      {// this element is needed
      Add(TSD);
      CETermStrip * pTS=ElementAt(GetUpperBound());
      }
    }


  return *this;
  };

//===========================================================================
//
//
//
//===========================================================================

//===========================================================================
//
//
//
//===========================================================================

void CNodeElectricsArray::InitialiseCollection()
  { 
  #if (dbgElecBase)
  if (dbgSolve())
  dbgpln("CNodeElectricsArray::InitialiseCollection");
  #endif
  m_lTermStripCollectCount=0; 
  m_lNodeCount=0; 
  }; 
void CNodeElectricsArray::InitialiseSolution()
  { 
  #if (dbgElecBase)
  if (dbgSolve())
    dbgpln("CNodeElectricsArray::InitialiseSolution");
  #endif
  m_nSolveCount++;
  if (m_nSolveCount<0)
    m_nSolveCount=0;
  }; 
void CNodeElectricsArray::Add(CTNode * pNd, CNodeElectrics * pTS) 
  { 
  #if (dbgElecBase)
  if (dbgSolve())
    dbgpln("CNodeElectricsArray::Add %3i %3i ", m_lNodeCount, m_lTermStripCollectCount);
  #endif
  pTS->m_pSolveCount=&m_nSolveCount;
  m_Nodes.SetAtGrow(m_lNodeCount++, pNd); 
  m_TermStripCollects.SetAtGrow(m_lTermStripCollectCount++, pTS); 
  }; 

long CNodeElectricsArray::StartSolution()
  {
  #if (dbgElecBase)
  if (dbgSolve())
    dbgpln("Electrics:Start");
  #endif
  
  long NChg=0;
  for (int tsc=0; tsc<m_lTermStripCollectCount/*m_TermStripCollects.GetSize()*/; tsc++)
    {
    CNodeElectrics &TSC=*m_TermStripCollects[tsc];
    for (int c=0; c<TSC.ComponentCount(); c++)
      {
      CEComponent &C=*TSC.Component(c);
      if (C.m_nSolveCount==m_nSolveCount)
        {      
        C.m_pCompData->m_I=0;
        C.m_lPathCount=0;
        if (C.m_nSolveCountPrv!=m_nSolveCount-1)
          NChg++;
        C.m_nSolveCountPrv=m_nSolveCount;
        }
      else
        {
        // must remove - it is no longer referenced
        #if (dbgElecBase)
        if (dbgConnectsNew())
          dbgpln("Remove Unused Connect %-25s --> %s", C.SrcName(), C.DstName());
        #endif                                           
        TSC.RemoveComponent(TSC.Component(c));
        NChg++;
        }
      }

    for (int ts=0; ts<TSC.TermStripCount(); ts++)
      {
      CETermStrip &TS=*TSC.TermStrip(ts);
      for (int t=0; t<TS.TerminalCount(); t++)
        {
        CETerminal *T=TS[t];
        //T.m_V=0;
        //dbgpln(" ClearDone @: %s    ", T->FullName());
        T->m_bDone=false;
        T->m_bBusy=false;
        T->m_pTStrip=&TS;
        }
      }

#if (dbgElecBase)
    if (dbgSolveSummary())
      {
      if (TSC.TermStripCount() || TSC.ComponentCount())
        {
        dbgpln("CTNode %s ---------------------", TSC.NodeName());
        for (int i=0; i<TSC.TermStripCount(); i++)
          {
          CETermStrip &TS=(*TSC[i]);
          dbgp("TermStrip %2i:%s.%s {", i,TS.NdName(), TS.Name());
          for (int j=0; j<TS.TerminalCount(); j++)
            dbgp("%s%s%s%s", j>0?",":"", AddStr(TS.Terminal(j)), TS.Terminal(j)->Name(), TS.Terminal(j)->m_bRefV?"[REF]":"");
          dbgpln("}");
          if (0)
            {
            for (int j=0; j<TS.TerminalCount(); j++)
              for (int k=0; k<TS.Terminal(j)->m_TrmComponents.GetSize(); k++)
                dbgpln("      %s  %s --> %s {%4i %4i}", 
                AddStr(TS.Terminal(j)->m_TrmComponents[k]),
                AddStr(TS.Terminal(j)->m_TrmComponents[k]->m_pSrc), AddStr(TS.Terminal(j)->m_TrmComponents[k]->m_pDst), 
                TS.Terminal(j)->m_TrmComponents[k]->m_nSolveCount, TS.Terminal(j)->m_TrmComponents[k]->m_nSolveCountPrv); 
            }
          }
        for (int j=0; j<TSC.ComponentCount(); j++)
          {
          CEComponent &C=*TSC.Component(j);
          if (C.m_pCompData->m_eType == CECompData::eTypePwr)
            {
            CDComplex dV(C.m_pDst->m_V-C.m_pSrc->m_V);
            dbgpln("Connect   %2i%s: Pwr:(%6.1f,Va%6.1f) V%10.4f R(%10.2g %10.2gi) %s%-25s -- > %s%s", 
              j, AddStr(&C), C.m_pCompData->m_dPwr, dV.Mag(), C.m_pCompData->m_V.Mag(), C.m_pCompData->m_Z.real(), C.m_pCompData->m_Z.imag(), AddStr(C.m_pSrc), C.SrcName(), AddStr(C.m_pDst), C.DstName());
            }
          else
            {
            dbgpln("Connect   %2i%s: %*s V%10.4f R(%10.2g %10.2gi) %s%-25s -- > %s%s", 
              j, AddStr(&C), 12+9,"", C.m_pCompData->m_V.Mag(), C.m_pCompData->m_Z.real(), C.m_pCompData->m_Z.imag(), AddStr(C.m_pSrc), C.SrcName(), AddStr(C.m_pDst), C.DstName());
            }
          //dbgpln("Connect   %2i%s: %s V%10.4f R(%10.2g %10.2gi) %s%-25s -- > %s%s", 
          //  j, AddStr(&C), C.m_bUsePwr?"Pwr":"   ", C.m_V.Mag(), C.m_Z.real(), C.m_Z.imag(), AddStr(C.m_pSrc), C.SrcName(), AddStr(C.m_pDst), C.DstName());
          }
        }
      }
#endif
    };
  return NChg;
  };

bool CNodeElectricsArray::Solve()
  {
  #if (dbgElecBase)
  if (dbgSolve())
    dbgpln("Electrics:Solve");
  #endif


  CEPathHelper PH;
  PH.m_lStartPathNo=0; // must match that set in StartSolution for m_lRecentPathNo

  for (int tsc=0; tsc<m_lTermStripCollectCount/*m_TermStripCollects.GetSize()*/; tsc++)
    {
    CNodeElectrics &TSC=*m_TermStripCollects[tsc];
    for (int c=0; c<TSC.ComponentCount(); c++)
      {
      CEComponent &C=*TSC.Component(c);
      if (C.m_pCompData->HasVoltageSrc())
        {
        #if (dbgElecBase)
        if (dbgPathCurrent())
          dbgpln("Voltage Source : %-25s > %s", C.SrcName(), C.DstName());
        #endif
        
        PH.Clear(&C);
        PH.m_CPath.SetAtGrow(PH.m_lCPathLen++, &C);
        C.m_pDst->FindPaths(&C, PH, 0);
        }
      }
    }

  bool Solved=true;
  for (tsc=0; tsc<m_lTermStripCollectCount/*m_TermStripCollects.GetSize()*/; tsc++)
    {
    CNodeElectrics &TSC=*m_TermStripCollects[tsc];
    for (int ts=0; ts<TSC.TermStripCount(); ts++)
      {
      CETermStrip &TS=*TSC.TermStrip(ts);
      for (int t=0; t<TS.TerminalCount(); t++)
        {
        CETerminal *T=TS[t];
        if (T->HasRefVoltage())
          {
          T->m_bDone=true;
          T->m_V=T->m_RefV;
          for (int c=0; c<T->m_TrmComponents.GetSize(); c++)
            {
            #if (dbgElecBase)
            if (dbgVoltage())
              dbgpln("*****************************");
            #endif
            
            CEComponent *C=T->m_TrmComponents[c];
            PH.Clear(C);
            PH.m_CPath.SetAtGrow(PH.m_lCPathLen++, C);
            if (!C->CalcVolts(T, PH))
              Solved=false;
            }
          }
        }
      }
    }

  for (tsc=0; tsc<m_lTermStripCollectCount/*m_TermStripCollects.GetSize()*/; tsc++)
    {
    CNodeElectrics &TSC=*m_TermStripCollects[tsc];
    for (int ts=0; ts<TSC.TermStripCount(); ts++)
      {
      CETermStrip &TS=*TSC.TermStrip(ts);
      for (int t=0; t<TS.TerminalCount(); t++)
        {
        CETerminal *T=TS[t];
        if (!T->m_bDone)
          {
          T->m_bDone=true;
          T->m_V=0;
          for (int c=0; c<T->m_TrmComponents.GetSize(); c++)
            {
            #if (dbgElecBase)
            if (dbgVoltage())
              dbgpln("***************************** DisConn");
            #endif
            
            CEComponent *C=T->m_TrmComponents[c];
            PH.Clear(C);
            PH.m_CPath.SetAtGrow(PH.m_lCPathLen++, C);
            if (!C->CalcVolts(T, PH))
              Solved=false;
            }
          }
        }
      }
    }
  return Solved;
  };

void CNodeElectricsArray::FinaliseSolution()
  {
  #if (dbgElecBase)
  if (dbgSolve())
    dbgpln("Electrics:Final");
  #endif
  for (int tsc=0; tsc<m_lTermStripCollectCount/*m_TermStripCollects.GetSize()*/; tsc++)
    {
    CNodeElectrics &TSC=*m_TermStripCollects[tsc];
    for (int ts=0; ts<TSC.TermStripCount(); ts++)
      {
      CETermStrip &TS=*TSC.TermStrip(ts);
      for (int t=0; t<TS.TerminalCount(); t++)
        {
        CETerminal *T=TS[t];
        //T->m_I=0;
        for (int c=0; c<T->m_TrmComponents.GetSize(); c++)
          {
          CEComponent *C=T->m_TrmComponents[c];
          CDComplex cI=C->m_pCompData->m_I;
          if (C->m_pSrc==T)
            cI*=-1;
          //if (cI.Ang()>=0 && cI.Ang()<2*PI/3)
          //  T->m_I+=cI;
          //else
          //  T->m_I-=C.m_I;
          int xxx=0;
          }
        }
      }
    }

  #if (dbgElecBase)
  if (dbgCurrentSummary())
    {
    for (int tsc=0; tsc<m_lTermStripCollectCount/*m_TermStripCollects.GetSize()*/; tsc++)
      {
      CNodeElectrics &TSC=*m_TermStripCollects[tsc];
      for (int c=0; c<TSC.ComponentCount(); c++)
        {
        CEComponent &C=*TSC.Component(c);
        dbgp("CI: %s (%10.4f<%3i)", C.m_bOwnsCompData?"Ext":"   ", C.m_pCompData->m_I.Mag(), (long)Rad2Deg(C.m_pCompData->m_I.Ang()));
        dbgp(" %-25s --> %s", C.SrcName(), C.DstName());
        dbgpln("");
        };
      }
    }
  #endif
  };          

void CNodeElectricsArray::SolveElectrics(bool ConvergeIt)
  {
  bool Solved=false;
  long NChgs;

  CStopWatch SW;
  SW.Start();

  #if (dbgElecBase)
  if (dbgSolve())
    dbgpln("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< Start           %10.6f", SW.Lap());
  #endif
  for (long Loop=0; !Solved && Loop<100; Loop++)
    {
    InitialiseSolution();
    #if (dbgElecBase)
    if (dbgSolve())
      dbgpln("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< Initialise [%3i]%10.6f", Loop, SW.Lap());
    #endif

    for (int n=0; n<m_lNodeCount; n++)
      m_Nodes[n]->ConnectElectrics();
    #if (dbgElecBase)
    if (dbgSolve())
      dbgpln("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< Connect    [%3i]%10.6f", Loop, SW.Lap());
    #endif
  
    NChgs=StartSolution(); // return Number of topology changes
    #if (dbgElecBase)
    if (dbgSolve())
      dbgpln("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< StartSoln  [%3i]%10.6f", Loop, SW.Lap());
    #endif

    //if (Loop==0 || NChgs==0)
    Solved=Solve();
    #if (dbgElecBase)
    if (dbgSolve())
      dbgpln("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< Solve      [%3i]%10.6f", Loop, SW.Lap());
    #endif
    };
  if (!Solved)
    LogError("Electrics", 0, "Not Solved");

  FinaliseSolution();
  #if (dbgElecBase)
  if (dbgSolve())
    dbgpln("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< Final      [%3i]%10.6f", Loop, SW.Lap());
  #endif
  }

void CNodeElectricsArray::TerminateCollection()
  {
//TODO Shutdown / save fails if this is in place
//  return ;



  for (int tsc=0; tsc<m_lTermStripCollectCount/*m_TermStripCollects.GetSize()*/; tsc++)
    {
    CNodeElectrics &TSC=*m_TermStripCollects[tsc];
    //for (int c=0; c<TSC.ConnectCount(); c++)
    #if (dbgElecBase)
    if (dbgConnectsNew())
      dbgpln("Remove Connects @ %s", TSC.m_pNd->Tag());
    #endif                                           
    while (TSC.ComponentCount())
      {
      CEComponent &C=*TSC.Component(0);  // must remove 
      #if (dbgElecBase)
      if (dbgConnectsNew())
        dbgpln("Remove Connect %-25s --> %s", C.SrcName(), C.DstName());
      #endif                                           
      TSC.RemoveComponent(TSC.Component(0));
      }
    }
  m_lTermStripCollectCount=0;
  }

//===========================================================================
//
//
//
//===========================================================================
