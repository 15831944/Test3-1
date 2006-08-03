// Link.cpp: implementation of the CLink class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SysCADMarshal.h"
#include "Link.h"
#include "slotmngr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLinkValue::CLinkValue(WORD VType)
  {
  m_lLink=-1;
  ChangeType(VType);
  m_lValueChanges=0;
  m_lValuesIgnored=0;
  m_dwTransactionID=0;
  m_lCfgSequence=-1;
  m_iLastChgDirn=0;
  };

CLinkValue::CLinkValue(const CLinkValue & V) :
  CFullValue(V)
  {
  m_lLink           = V.m_lLink;
  m_dwTransactionID = V.m_dwTransactionID;
  m_lValueChanges   = V.m_lValueChanges;
  m_lValuesIgnored  = V.m_lValuesIgnored;
  m_lCfgSequence    = V.m_lCfgSequence;
  m_iLastChgDirn    = V.m_iLastChgDirn;
  m_State           = V.m_State;
  }

CLinkValue & CLinkValue::operator =(const CLinkValue & V)
  {
  *((CFullValue*)this)=*((CFullValue*)&V);
  m_lLink           = V.m_lLink;
  m_dwTransactionID = V.m_dwTransactionID;
  m_lValueChanges   = V.m_lValueChanges;
  m_lValuesIgnored  = V.m_lValuesIgnored;
  m_lCfgSequence    = V.m_lCfgSequence;
  m_iLastChgDirn    = V.m_iLastChgDirn;
  m_State           = V.m_State;
  return *this;
  }

// =======================================================================
//
//
//
// =======================================================================

CLinkCondBlk::CLinkCondBlk()
  {
  m_On                  = false;
  m_BaseValueValid      = false;
  m_ChgCount            = 0;
  m_BaseValue           = 0;
  }

// =======================================================================
//
//
//
// =======================================================================

CLinkCfg::CLinkCfg(LPCSTR Tag, LPCSTR SimTag, LPCSTR CnvTxt, LPCSTR Values, LPCSTR Desc, WORD VType, bool Get, bool Set):
  CLinkValue(VType),
  m_sTag(Tag),
  m_sSimTag(SimTag),
  m_sCnvTxt(CnvTxt),
  m_sDesc(Desc)
  {
  SetValueSet(Values);
  m_bGet          = Get;
  m_bSet          = Set;
  m_bInUse        = false;
  m_bInFilter     = true;
  m_pSlot         = NULL;
  //m_bSimIsParm          = false;
  //m_bSimTagValid          = false;
  };

CLinkCfg::~CLinkCfg()
  {
  };

CLinkCfg::CLinkCfg(const CLinkCfg & V):
  CLinkValue(V),
  CSlotValueSet(V)
  {
  m_sTag        = V.m_sTag;
  m_sSimTag     = V.m_sSimTag;
  m_sCnvTxt     = V.m_sCnvTxt;    
  m_bGet        = V.m_bGet;
  m_bSet        = V.m_bSet;        
  m_bInUse      = V.m_bInUse; 
  m_bInFilter   = V.m_bInFilter; 
  m_sDesc       = V.m_sDesc;
  m_pSlot       = V.m_pSlot;
  }

CLinkCfg & CLinkCfg::operator =(const CLinkCfg & V)
  {
  *((CLinkValue*)this)=*((CLinkValue*)&V);
  *((CSlotValueSet*)this)=*((CSlotValueSet*)&V);
  m_sTag        = V.m_sTag;
  m_sSimTag     = V.m_sSimTag;
  m_sCnvTxt     = V.m_sCnvTxt;    
  m_bGet        = V.m_bGet;
  m_bSet        = V.m_bSet;        
  m_bInUse      = V.m_bInUse; 
  m_bInFilter   = V.m_bInFilter; 
  m_sDesc       = V.m_sDesc;
  m_pSlot       = V.m_pSlot;
  //m_bSimIsParm  = V.m_bSimIsParm;
  return *this;
  }

void CLinkCfg::SetError(USHORT Error, bool LogIt)
  {
  m_State.m_nError=Error;
  if (Error && LogIt)
    ReportError(m_sTag, 0, "%s", SErr_String(Error));
  };
void CLinkCfg::SetError(USHORT Error, LPCSTR Fmt, ...)
  {
  m_State.m_nError=Error;
  if (Error)
    {
    char    Buff[2048];
    va_list argptr;
    va_start(argptr, Fmt);
    vsprintf(Buff, Fmt, argptr);
    va_end(argptr);

    ReportError(m_sTag, 0, "%s: %s", SErr_String(Error), Buff);
    }
  };
void CLinkCfg::SetError(USHORT Error, HRESULT hr, LPCSTR Fmt, ...)
  {
  m_State.m_nError=Error;
  m_State.m_nError=Error;
  if (Error)
    {
    char    Buff[2048];
    va_list argptr;
    va_start(argptr, Fmt);
    vsprintf(Buff, Fmt, argptr);
    va_end(argptr);

    ReportError(m_sTag, hr, "%s: %s", SErr_String(Error), Buff);
    }
  };

bool CLinkCfg::InFilter(long DevSelect, CSelection &Select)
  {
  if (!((DevSelect<0) || (DevSelect==0 && m_bGet) || (DevSelect==1 && m_bSet)))
    return false; 
  return Select.InFilter(m_sTag, m_sSimTag, m_pSlot->m_sOPCTag, NULL, m_State.m_nError!=0, m_State.m_bHold);
  }

// =======================================================================
//
//
//
// =======================================================================

CLink::CLink(CSlot * pSlot, LPCSTR Tag, LPCSTR ConnectTag, LPCSTR CnvTxt, LPCSTR Values, LPCSTR Desc, WORD VType, bool Get, bool Set):
  CLinkCfg(Tag, ConnectTag, CnvTxt, Values, Desc, VType, Get, Set)
  {
  m_bVisible          = false;
  m_dwLastWriteTick   = GetTickCount(); 
  m_dLastAppliedValue = 0;
  //m_dFixedRange     = 0;
  m_dMinValue         = 0;
  m_dMaxValue         = 0;
  m_bMustSend         = true;
  m_bRangeInited      = false;
  m_ChgFlags          = IOMChgFlag_All;
  m_dwFlushChangeCalls = 0;
  m_dwChangesInCall    = 0;

  m_pSlot             = pSlot;
  if (m_pSlot)
    m_pSlot->m_pLink=this;
  };

// -----------------------------------------------------------------------

CLink::~CLink()
  {
  };

// -----------------------------------------------------------------------

void CLink::RemoveConnects()
  {
  m_ReflectedGets.SetSize(0);
  }

// -----------------------------------------------------------------------

void CLink::InitialiseConnects()
  {
  m_ReflectedGets.SetSize(0);
  }

// -----------------------------------------------------------------------

long CLink::ProcessConnects()
  {
  long n=0;
  for (int c=0; c<m_ReflectedGets.GetSize(); c++)
    m_ReflectedGets[c]->Process(eCSD_Link, m_lLink, eCSD_Default, -1, *this, 0);
  return n;
  };

// -----------------------------------------------------------------------

bool CLink::SetValue(CChangeItem * pRqst)
  {
  ASSERT(pRqst && pRqst->m_lDstInx==m_lLink);


  if (m_State.m_bHold && !pRqst->m_bOverrideHold)
    {
    m_ChgFlags|=IOMChgFlag_Ignored;
    m_lValuesIgnored++;
    return false;
    }

  VARTYPE SlotType=Type();
  m_iLastChgDirn=FullValue().ChangeDirection(*pRqst);
  FullValue()=*pRqst;
  if (pRqst->m_eSrc==eCSD_Simulator)
    {
    COleVariant &V=FullValue().m_vValue;
    CSlotRange & Rng=m_pSlot->m_Range;
    //CSlotSpan & Spn=m_pSlot->m_Span;
    if (IsFloatDataVT(SlotType))
      {
      HRESULT hr=VariantChangeType(&V, &V, 0, VT_R8);
      if (FAILED(hr))
        SetError(SErr_SetValue, hr, "");

      if (!m_bMustSend)
        {
        DWORD TC=GetTickCount();
        double TimeDiff=fabs((double)TC-m_dwLastWriteTick);
        double Tol=0;
        if (Rng.m_bValid)
          {
          Tol=gs_SlotMngr.CalcFloatTolerance(Rng.Range(), TimeDiff);
          }
        else if (!m_bRangeInited)
          {
          m_bRangeInited=true;
          m_dMinValue=V.dblVal;
          m_dMaxValue=V.dblVal;
          Tol=0;
          }
        else 
          {
          m_dMinValue=Min(m_dMinValue, V.dblVal);
          m_dMaxValue=Max(m_dMaxValue, V.dblVal);
          Tol=gs_SlotMngr.CalcFloatTolerance(Max(1.0e-6, m_dMaxValue-m_dMinValue), TimeDiff);
          }
    
        if (fabs(V.dblVal-m_dLastAppliedValue)<Tol)
          {
          m_ChgFlags|=IOMChgFlag_Ignored;
          m_lValuesIgnored++;
          SendDisplayValue();
          return false;
          }
        m_dwLastWriteTick=TC;
        m_dLastAppliedValue=V.dblVal;
        }
      }
    
    HRESULT hr=VariantChangeType(&V, &V, 0, SlotType);
    if (FAILED(hr))
      SetError(SErr_SetValue, hr, "");

    m_bMustSend = false;
    }

//
//  if (VariantsEqual(&m_vValue, &pRqst->m_vValue)
//      && m_wQuality==pRqst->m_wQuality
//      //&& m_ftTimeStamp==pRqst->m_ftTimeStamp
//      )
//    {
//    m_lValuesIgnored++;
//    return false;
//    }

  m_ChgFlags|=IOMChgFlag_Changes|IOMChgFlag_Value|IOMChgFlag_HoldValue;
  m_lValueChanges++;

  m_dwTransactionID=pRqst->m_dwTransactionID;

  ProcessConnects();

  SendDisplayValue();

  return true;
  };

// -----------------------------------------------------------------------

void CLink::SendDisplayValue()
  {
  //dbgpln("CLink::SendDisplayValue %4i %s", m_lLink, m_bVisible?"Visible":"Hidden");

  if (m_bVisible && gs_SlotMngr.m_bMonitorOn)
    {
    BOOL OK=theApp.PostThreadMessage(WMU_UPDATEONELINK, m_lLink, (long)new CLinkValue(*this)); 
    if (!OK)
      dbgpln("SendDisplayValue Failed %4i", m_lLink);
    }

  }

// -----------------------------------------------------------------------

void CLink::SendStateValue()
  {
  //dbgpln("SendDisplayValue %4i %s", m_lSlot, m_bVisible?"Visible":"Hidden");
  //if (m_bVisible && gs_SlotMngr.m_bMonitorOn)
    {
    BOOL OK=theApp.PostThreadMessage(WMU_UPDATEONELINKSTATE, m_lLink, StateLong()); 
    if (!OK)
      dbgpln("SendDisplayValue Failed %4i", m_lLink);
    }
  }

// -----------------------------------------------------------------------

void CLink::Save(CProfINIFile & SavePF)
  {
  if (m_bInUse)
    {
    CString Value, V;
    VariantToString(*this, V, false);
    Value.Format("%i,%i,%s ", m_lLink, m_State.m_bHold?1:0, V);
    SavePF.WrStr("Links", m_sTag, Value);
    }
  };

// -----------------------------------------------------------------------

void CLink::Load(CProfINIFile & SavePF)
  {
  if (1)
    return;
  
  CString Line, V;
  Line=SavePF.RdStr("Links", m_sTag, "");
  if (Line.GetLength()>0)
    {
    int iStart=0;
    int lLink = SafeAtoL(Line.Tokenize(",", iStart));
    //int lDbg  = SafeAtoL(Line.Tokenize(",", iStart));
    int lHold = SafeAtoL(Line.Tokenize(",", iStart));
    V=Line.Tokenize("'", iStart);
    
    //m_bSltDbgOn=lDbg!=0;
    m_State.m_bHold=lHold!=0;

    CFullValue FV(OPC_QUALITY_GOOD);
    StringToVariant(Type(), V, FV, false);
    gs_SlotMngr.AppendChange(eCSD_File, -1, eCSD_Link, m_lLink, -1/*gs_SlotMngr.GetTransactionID()*/, FV, NULL, true);
    }
  };

// -----------------------------------------------------------------------

//void CLink::SetRqdValue(LPCTSTR ValueID)
//  {
//
//  };

// -----------------------------------------------------------------------

IMPLEMENT_SPARES(CLink, 0);
IMPLEMENT_SPARES(CLinkCfg, 0);
//IMPLEMENT_SPARES(CDspLink, 0);
IMPLEMENT_SPARES(CLinkValue, 10000);

// =======================================================================
//
//
//
// =======================================================================
