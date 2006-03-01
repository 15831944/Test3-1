// SlotConnect.cpp: implementation of the CSlotConnect class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SysCADMarshal.h"
#include "Slot.h"
#include "SlotConnect.h"
#include "slotmngr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// =======================================================================
//
//
//
// =======================================================================

CSlotConnPrf::CSlotConnPrf()
  {
  m_Mode=eNULL;
  };

// -----------------------------------------------------------------------

long CSlotConnPrf::Parse(LPCSTR File)
  {
  if (!FileExists((LPTSTR)File))
    return 1;
  
  //m_sTag.FnName((LPTSTR)File);
  m_bYReversed=false;

  long RetCode=0;
  Strng Ext;
  Ext.FnExt((LPTSTR)File);
  if (Ext.XStrICmp(".txt")==0 || Ext.XStrICmp(".csv")==0)
    {
    FILE *h=fopen(File, "rt");
    if (h)
      {
      char Buff[4096];
      CSVColArray c;
      int Quote;
      int nFlds = 0;
      Buff[0]=0;
      while (strlen(Buff)==0 && fgets(Buff, sizeof(Buff), h))
        XStrLTrim(Buff, " \t\n");

      nFlds = ParseCSVTokens(Buff, c, Quote);

      if (nFlds>0)
        {
        if (stricmp(Buff, "ABS")==0)
          m_Mode=eABS;
        else if (stricmp(Buff, "SCL")==0)
          m_Mode=eSCL;
        else if (stricmp(Buff, "SCL%")==0)
          m_Mode=eSCLPERC;
        else if (stricmp(Buff, "CONTRONIC")==0)
          m_Mode=eCONTRONIC;
        else
          {
          return 4;
          goto Leave;
          }

        if (m_Mode==eCONTRONIC)
          {
          while (fgets(Buff, sizeof(Buff), h))
            {
            int nFlds = ParseTokenList(Buff, c, "=:");
            if (nFlds>=3)
              {
              CSlotConnPrfPt Pt;
              Pt.X=(float)SafeAtoF(c[1]);
              Pt.Y=(float)SafeAtoF(c[2]);
              m_Points.Add(Pt);
              }
            else if (m_Points.GetSize()>0)
              break;
            }
          }
        else
          {
          while (fgets(Buff, sizeof(Buff), h))
            {
            int nFlds = ParseCSVTokens(Buff, c, Quote);
            if (nFlds>=2)
              {
              CSlotConnPrfPt Pt;
              Pt.X=(float)SafeAtoF(c[0]);
              Pt.Y=(float)SafeAtoF(c[1]);
              m_Points.Add(Pt);
              }
            else if (m_Points.GetSize()>0)
              break;
            }
          }
        if (m_Points.GetSize()<2)
          {
          RetCode=5;
          goto Leave;
          }
        m_bYReversed=m_Points[0].Y> m_Points[m_Points.GetUpperBound()].Y;
        fclose(h);
        }
      else
        RetCode=4;
      }
    else
      RetCode=2;
Leave:
    if (h)
      fclose(h);
    return RetCode;
    }
  else
    return 3;
  };

// CONTRONIC FORMAT
//NO. 1=    .0 %:        .0 I/P VALUE  :
//NO. 2=   6.2 %:     123.6 I/P VALUE  :
//NO. 3=  12.5 %:     221.1 I/P VALUE  :
//NO. 4=  18.7 %:     308.1 I/P VALUE  :
// -----------------------------------------------------------------------

LPCTSTR CSlotConnPrf::ErrorString(long RetCode)
  {
  switch (RetCode)
    {
    case 0: return "No Error";
    case 1: return "File missing";
    case 2: return "File not opened";
    case 3: return "Must be txt or csv";
    case 4: return "First Line must contain ABS,SCL SCL% or CONTRONIC";
    case 5: return "Must have 2 or more points";
    default:
      return "Unknown error";
    }
  };

// -----------------------------------------------------------------------

double CSlotConnPrf::X2Y(double X)
  {
  double Y=0;
  long i0, i1, inc;
  i0=0;
  i1=m_Points.GetUpperBound();
  inc=1;

  for (int i=i0; i0!=i1; i+=inc)
    if (X<m_Points[i].X)
      break;

  CSlotConnPrfPt & Pt0=m_Points[i];
  CSlotConnPrfPt & Pt1=m_Points[i+1];

  Y=Pt0.Y+(Pt1.Y-Pt0.Y)*(X-Pt0.X)/(Pt1.X-Pt0.X);

  return Y;
  };

// -----------------------------------------------------------------------

double CSlotConnPrf::Y2X(double Y)
  {
  double X=0;
  long i0, i1, inc;
  if (m_bYReversed)
    {
    i0=m_Points.GetUpperBound();
    i1=0+1;
    inc=-1;
    }
  else
    {
    i0=0;
    i1=m_Points.GetUpperBound()-1;
    inc=1;
    }
  for (int i=i0; i!=i1; i+=inc)
    if (Y<m_Points[i].Y)
      break;

  CSlotConnPrfPt & Pt0=m_Points[i];
  CSlotConnPrfPt & Pt1=m_Points[i+inc];

  X=Pt0.X+(Pt1.X-Pt0.X)*(Y-Pt0.Y)/NZ(Pt1.Y-Pt0.Y);

  return X;
  };

// =======================================================================
//
//
//
// =======================================================================

CSlotConnect::CSlotConnect(LPCSTR pTag, /*LPCSTR pCnvTxt,*/ bool IsSet, bool Inv)
  {
  m_sTag = pTag;
  m_bIsSet = IsSet;
  m_bIsGet = !IsSet;
  m_bInvert = Inv;

  m_bValid=false;
  m_eSrc=eCSD_Null;
  m_eDst=eCSD_Null;
  m_lSrcIndex=-1;
  m_lDstIndex=-1;

  m_pCdBlk = NULL;
  m_pCdBlkVar = NULL;
  m_lCdBlk=-1;
  m_pCdBlk=NULL;
  m_pCdBlkVar=NULL;
  m_bCdBlkVarFlt=false;
  }

CSlotConnect::~CSlotConnect()
  {
  for (int i=0; i<m_Ops.GetSize(); i++)
    delete m_Ops[i];
  };

// --------------------------------------------------------------------------

void CSlotConnect::SetDelayTimes(DWORD DelayTime1, DWORD DelayTime2, bool UseDelay2)
  {
  m_Delay.m_dwTime1 = DelayTime1;
  m_Delay.m_dwTime2 = DelayTime2;
  m_Delay.m_bUseTime2 = UseDelay2;
  m_Delay.m_bEdge = (m_Delay.m_dwTime1==InfiniteDelay ||
                     (m_Delay.m_bUseTime2 && m_Delay.m_dwTime2==InfiniteDelay));
  }

// --------------------------------------------------------------------------

void CSlotConnect::Initialise()
  {
  m_bValid=false;
  m_eSrc=eCSD_Null;
  m_eDst=eCSD_Null;
  m_lSrcIndex=-1;
  m_lDstIndex=-1;

  m_pCdBlk = NULL;
  m_pCdBlkVar = NULL;
  m_lCdBlk=-1;
  m_pCdBlk=NULL;
  m_pCdBlkVar=NULL;
  m_bCdBlkVarFlt=false;
  }

// -----------------------------------------------------------------------

bool CSlotConnect::Connect(CSlot * pSlot)
  {
  long iDot;
  long iSpc;
  bool MustBeLink=false;
  Strng RootTag,WrkCnvTxt;

  m_sTag.TrimLeft(" ");
  m_sTag.TrimRight(" ");
  if ((iSpc=m_sTag.Find(' '))>0)
    {
    TaggedObject::SplitTagCnv((LPSTR)(LPCSTR)m_sTag, RootTag, WrkCnvTxt);
    TaggedObject::ValidateTag(RootTag);
    if (RootTag.Length()>0)
      {
      MustBeLink=true;
      }
    }
  else
    {
    // may be a link or something else 
    RootTag=m_sTag;
    }

  //m_lLink=-1;
  m_lCdBlk=-1;
  m_eSrc=eCSD_Slot;
  m_lSrcIndex=pSlot->m_lSlot;
  m_lDstIndex=-1;
  m_bValid=false;
  m_pCdBlk=NULL;
  m_pCdBlkVar=NULL;

  if (!MustBeLink)
    {
    long iSlot=gs_SlotMngr.FindSlot(m_sTag);
    if (iSlot>=0)
      {
      m_eDst=eCSD_Slot;
      m_lDstIndex=iSlot;
      if (m_bIsGet)
        {
        Exchange(m_eSrc, m_eDst);
        Exchange(m_lSrcIndex, m_lDstIndex);
        gs_SlotMngr.m_Slots[m_lSrcIndex]->m_ReflectedGets.Add(this);
        }
      m_bValid=true;
      goto Done;
      }
  
    //Split Tag & Variable
    iDot=m_sTag.Find('.');
    if (iDot>0)
      {
      CString CdTag=m_sTag.Left(iDot);
      CString Variable=m_sTag.Mid(iDot+1);
      if ((m_lCdBlk=gs_SlotMngr.FindCdBlk(CdTag))>=0)
        {
        //CString CdTag=Tag;
        //CString Variable=Var;
        m_eDst=eCSD_CdBlk;
        m_pCdBlk=gs_SlotMngr.m_CdBlks[m_lCdBlk];
        m_pCdBlkVar=m_pCdBlk->m_Code.m_pVarList->FindTag((LPSTR)(LPCSTR)Variable);
        if (!m_pCdBlkVar)
          {
          pSlot->SetError(SErr_CdBlkTagMissing, "CodeBlock Tag '%s' not Found", Variable);
          return (m_bValid=false);
          goto Done;
          }

        switch (m_pCdBlkVar->WhatAmI())
          {
          case VarDouble:
            m_bCdBlkVarFlt=true;
            break;
          case VarLong  :
          case VarByte  :
          case VarBit   :
            m_bCdBlkVarFlt=false;
            break;
          default :
            pSlot->SetError(SErr_CdBlkTagBadType, "CodeBlock Tag '%s' Bad Type", Variable);
            m_bValid=false;
            goto Done;
            break;
          }
        if (m_bIsGet)
          {
          Exchange(m_eSrc, m_eDst);
          Exchange(m_lSrcIndex, m_lDstIndex);
          gs_SlotMngr.m_CdBlks[m_lCdBlk]->m_ReflectedGets.Add(this);
          }
        m_bValid=true;
        goto Done;
        }
      }
    }

  m_bValid=true;
  m_eDst=eCSD_Link;     
  
  if (1)
    {
    CString LinkTag;
    if (0)
      {
      LinkTag=RootTag();
      long i;
      while ((i=LinkTag.FindOneOf("."))>=0)
        LinkTag.SetAt(i, '_');               
      }
    else
      LinkTag = pSlot->m_sTag;
    m_lDstIndex=gs_SlotMngr.AddLink(pSlot, LinkTag, RootTag(), WrkCnvTxt(), pSlot->Type(), m_bIsGet, m_bIsSet);
    if (m_lDstIndex>=0)
      {
      if (m_bIsGet)
        {
        Exchange(m_eSrc, m_eDst);
        Exchange(m_lSrcIndex, m_lDstIndex);

        //EXCHANGE(eConnSrcDst, m_eSrc, m_eDst);
        //EXCHANGE(long, m_lSrcIndex, m_lDstIndex);
        gs_SlotMngr.m_Links[m_lSrcIndex]->m_ReflectedGets.Add(this);

        if (m_eSrc==eCSD_Link)
          {
          CLink &L=*gs_SlotMngr.m_Links[m_lSrcIndex];
          CSlotConnArray &RGs=L.m_ReflectedGets;
          for (int rg=0; rg<RGs.GetSize(); rg++)
            {
            CSlotConnOpArray &Ops=RGs[rg]->m_Ops;
            for (int op=0; op<Ops.GetSize(); op++)
              {
              if (Ops[op]->IsConditioning())
                {
                L.m_CondBlk.m_On                = true;
                L.m_CondBlk.m_BaseValueValid    = false;
                }
              }
            }
          }
        }
      else
        {
        }
      }
    }
Done:
  //Dump("  ");
  return m_bValid;
  }

// -----------------------------------------------------------------------

void CSlotConnect::CorrectConnects(CLongArray & NewNos)
  {
  if (m_eSrc==eCSD_Link)
    m_lSrcIndex=NewNos[m_lSrcIndex];
  if (m_eDst==eCSD_Link)
    m_lDstIndex=NewNos[m_lDstIndex];
  };

// -----------------------------------------------------------------------

void CSlotConnect::Dump(LPCSTR Hd)
  {
  dbgpln("%sConn %s %s %s %10s %4i %10s %4i Cd:%4i %08x %08x %s", 
    Hd,
    m_bValid?"OK":"  ", 
    m_bIsGet?"Get":"   ", 
    m_bIsSet?"Set":"   ", 
    SrcDstString(m_eSrc),
    m_lSrcIndex,
    SrcDstString(m_eDst),
    m_lDstIndex,
    m_lCdBlk,
    m_pCdBlk,
    m_pCdBlkVar,
    m_sTag);
  }

// -----------------------------------------------------------------------

void CSlotConnect::ApplyRangeLink2Slot(CSlot & S, COleVariant &V)
  {
  // Apply Range COMING into Link
  if (S.m_Range.m_bValid)
    {
    // Apply Range before going to Link
    if (IsFloatDataVT(V.vt))
      {
      HRESULT hr=VariantChangeType(&V, &V, 0, VT_R8);
      if (FAILED(hr))
        S.SetError(SErr_ApplyRangeLink2Slot, hr, "");
      V.dblVal=(V.dblVal-S.m_Range.m_dMin)/(S.m_Range.m_dMax-S.m_Range.m_dMin);
      }
    else if (IsSignedDataVT(V.vt))
      {
      HRESULT hr=VariantChangeType(&V, &V, 0, VT_I4);
      if (FAILED(hr))
        S.SetError(SErr_ApplyRangeLink2Slot, hr, "");
      V.lVal=(V.lVal-(long)S.m_Range.m_dMin)/(long)(S.m_Range.m_dMax-S.m_Range.m_dMin);
      }
    else if (IsUnsignedDataVT(V.vt))
      {
      HRESULT hr=VariantChangeType(&V, &V, 0, VT_UI4);
      if (FAILED(hr))
        S.SetError(SErr_ApplyRangeLink2Slot, hr, "");
      V.ulVal=(V.ulVal-(unsigned long)S.m_Range.m_dMin)/(unsigned long)(S.m_Range.m_dMax-S.m_Range.m_dMin);
      }
    }
  }

// -----------------------------------------------------------------------

void CSlotConnect::ApplyRangeSlot2Link(CSlot & S, COleVariant &V)
  {
  if (S.m_Range.m_bValid)
    {
    // Apply Range GOING to Link
    if (IsFloatDataVT(S.Type()))
      {
      V.ChangeType(VT_R8);
      V.dblVal=S.m_Range.m_dMin+V.dblVal*(S.m_Range.m_dMax-S.m_Range.m_dMin);
      }
    else if (IsSignedDataVT(S.Type()))
      {
      V.ChangeType(VT_I4);
      V.lVal=(long)S.m_Range.m_dMin+V.lVal*(long)(S.m_Range.m_dMax-S.m_Range.m_dMin);
      }
    else if (IsUnsignedDataVT(S.Type()))
      {
      V.ChangeType(VT_UI4);
      V.ulVal=(unsigned long)S.m_Range.m_dMin+V.ulVal*(unsigned long)(S.m_Range.m_dMax-S.m_Range.m_dMin);
      }
    }
  }

// -----------------------------------------------------------------------

void CSlotConnect::Process(eConnSrcDst eSrc, long SrcI, CFullValue & SrcValue)
  {
  // Always Executed as a SET - Hence the use of the ReflectedGets

  if (!m_bValid)
    return;

  switch (m_eDst)
    {
    case eCSD_Slot:
      {
      CFullValue V=SrcValue;
      ProcessOps(V);
      if (m_eSrc==eCSD_Link)
        ApplyRangeLink2Slot(*Slots[m_lDstIndex], V);
      gs_SlotMngr.AppendChange(eSrc, SrcI, eCSD_Slot, m_lDstIndex, -1, V, &m_Delay);
      break;
      }
    case eCSD_CdBlk:
      {
      //if (1)
      //  break;

      CFullValue V=SrcValue;
      ProcessOps(V);
      if (m_bCdBlkVarFlt)
        {
        V.ChangeType(VT_R8);
        m_pCdBlkVar->set(V.m_vValue.dblVal);
        }
      else
        {
        V.ChangeType(VT_I4);
        m_pCdBlkVar->set(V.m_vValue.lVal);
        }
      // Execute pgm
      CGExecContext ECtx(NULL);
      m_pCdBlk->m_Code.Execute(ECtx);

      // Update Destination Tags
      // Update Destination Tags
      for (int i=0; i<m_pCdBlk->m_ReflectedGets.GetSize(); i++)
        {
        CSlotConnect &C=(*m_pCdBlk->m_ReflectedGets[i]);
        if (C.m_bCdBlkVarFlt)
          {
          CFullValue D=V;
          D.m_vValue=C.m_pCdBlkVar->getD();
          C.ProcessOps(D);
          gs_SlotMngr.AppendChange(eCSD_CdBlk, C.m_lSrcIndex, C.m_eDst, C.m_lDstIndex, -1, D, &C.m_Delay);
          }
        else
          {
          CFullValue L=V;
          L.m_vValue=C.m_pCdBlkVar->getL();
          C.ProcessOps(L);
          gs_SlotMngr.AppendChange(eCSD_CdBlk, C.m_lSrcIndex, C.m_eDst, C.m_lDstIndex, -1, L, &C.m_Delay);
          }
        }
      break;
      }
    case eCSD_Link:
      CFullValue V=SrcValue;
      if (m_eSrc==eCSD_Slot)
        ApplyRangeSlot2Link(*Slots[m_lSrcIndex], V);
      ProcessOps(V);
      gs_SlotMngr.AppendChange(eSrc, m_lSrcIndex, m_eDst, m_lDstIndex, -1, V, &m_Delay);
      break;
    }
  }

// -----------------------------------------------------------------------

void CSlotConnect::ProcessOps(VARIANT &V)
  {
  if (m_Ops.GetSize() || m_bInvert)
    {
    COleVariant T;
    T.ChangeType(VT_R8, &V);
    if (m_bInvert && m_bIsGet)
      T.dblVal = (T.dblVal!=0? 0.0 : 1.0);
    for (int i=0; i<m_Ops.GetSize(); i++)
      T.dblVal=m_Ops[i]->Exec(T.dblVal, 0);
    if (m_bInvert && m_bIsSet)
      T.dblVal = (T.dblVal!=0? 0.0 : 1.0);
    VariantChangeType(&V, &T, 0, V.vt);
    }
  };

// =======================================================================
//
//
//
// =======================================================================

long        CSlotConnect::getSlotCount() const               { return gs_SlotMngr.m_Slots.GetSize(); };
CSlot     * CSlotConnect::getSlot(long Index) const          { return gs_SlotMngr.m_Slots[Index]; };
void        CSlotConnect::putSlot(long Index, CSlot * pSlot) { gs_SlotMngr.m_Slots[Index] = pSlot; };
long        CSlotConnect::getLinkCount() const               { return gs_SlotMngr.m_Links.GetSize(); };
CLink     * CSlotConnect::getLink(long Index) const          { return gs_SlotMngr.m_Links[Index]; };
void        CSlotConnect::putLink(long Index, CLink * pLink) { gs_SlotMngr.m_Links[Index] = pLink; };
long        CSlotConnect::getDeviceCount() const                   { return gs_SlotMngr.m_Devices.GetSize(); };
CDevice   * CSlotConnect::getDevice(long Index) const              { return gs_SlotMngr.m_Devices[Index]; };
void        CSlotConnect::putDevice(long Index, CDevice * pDevice) { gs_SlotMngr.m_Devices[Index] = pDevice; };

// =======================================================================
//
//
//
// =======================================================================

