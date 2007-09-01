//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#define  __MD_SHARE3_CPP
#include "sc_defs.h"
#include "md_share3.h"
#include "msglog.h"
#include "tagobj.h"
#include "datacnvs.h"

//===========================================================================
//
//
//
//===========================================================================

#if WITHMDLRANGECHECKS

template<> UINT AFXAPI HashKey (MRngChkKey *Key)
  { 
  UINT nHash = 0;
	LPCSTR pC;
  pC = Key->m_sCntxt;
  while (*pC)
		nHash = (nHash<<5) + nHash + toupper(*pC++);
	pC = Key->m_sName;
  while (*pC)
		nHash = (nHash<<5) + nHash + toupper(*pC++);
	return nHash;
  };

//-----------------------------------------------------------------------

template<> BOOL AFXAPI CompareElements (MRngChkKey* const * E1, MRngChkKey* const * E2)
  { 
  return ((*E1)->m_sCntxt==(*E2)->m_sCntxt && (*E1)->m_sName==(*E2)->m_sName);
  };

//=============================================================================
//
//=============================================================================

MRngChkItem::MRngChkItem(const MRngChkKey & Key) : \
m_Key(Key)
  {
  m_Iter      = -1;
  m_iCnv      = -1;
  m_LTMin     = false;
  m_GTMax     = false;
  m_MinValue  = 0.0;
  m_MaxValue  = 0.0;
  m_Value     = 0.0;
  };

//-----------------------------------------------------------------------

LPCSTR MRngChkItem::FormatDisplay(int FormatNo)
  {
  if (!m_IsMsg)
    {
    if (m_iCnv<0)
      {
      m_sCnvTxt=m_Key.m_sName.TrimRight();
      int i;
      while ((i=m_sCnvTxt.Find(" "))>=0)
        m_sCnvTxt.Delete(0, i+1);
      if (m_sCnvTxt.GetLength()>0)
        {
        m_sNameTrim=m_Key.m_sName.Left(m_Key.m_sName.GetLength()-m_sCnvTxt.GetLength()).TrimRight();
        if ((i=m_sCnvTxt.Find("("))>=0)
          {
          CString S=m_sCnvTxt.Left(i);
          m_iCnv=gs_CnvsMngr.FindCnv(S.GetBuffer()); 
          m_sCnvTxt.Delete(0, i+1);
          m_sCnvTxt=m_sCnvTxt.TrimRight(")");
          }
        }
      }
    if (m_iCnv>=0)
      {
      if (m_LTMin)  
        m_sDisplay.Format("%s: Min %s (%s) = %.4g < %.4g", m_Key.m_sCntxt, m_sNameTrim, m_sCnvTxt, Cnvs[m_iCnv]->Human(m_Value, m_sCnvTxt.GetBuffer()), Cnvs[m_iCnv]->Human(m_MinValue, m_sCnvTxt.GetBuffer()));
      else if (m_GTMax)
        m_sDisplay.Format("%s: Max %s (%s) = %.4g > %.4g", m_Key.m_sCntxt, m_sNameTrim, m_sCnvTxt, Cnvs[m_iCnv]->Human(m_Value, m_sCnvTxt.GetBuffer()), Cnvs[m_iCnv]->Human(m_MaxValue, m_sCnvTxt.GetBuffer()));
      }
    else
      {
      if (m_LTMin)  
        m_sDisplay.Format("%s: Min %s = %.4g < %.4g", m_Key.m_sCntxt, m_Key.m_sName, m_Value, m_MinValue);
      else if (m_GTMax)
        m_sDisplay.Format("%s: Max %s = %.4g > %.4g", m_Key.m_sCntxt, m_Key.m_sName, m_Value, m_MaxValue);
      }
    }
  else
    m_sDisplay.Format("%s: %s %s", m_Key.m_sCntxt, m_Key.m_sName, m_sMsg);

  return m_sDisplay;
  };

//=============================================================================
//
//=============================================================================

byte MRngChkCntxt::sm_iCndRngChks=MRngChkCntxt::CRC_Basic;

MRngChkCntxt::MRngChkCntxt(TaggedObject * pObject, LPCSTR Context) : \
m_pObject(pObject),
m_sContext(Context)
  {
  while (m_pObject->pAttachedTo)
    m_pObject=m_pObject->pAttachedTo;

  m_Position   = gs_RngChecks.m_List.AddTail(this);
  m_Iter       = -1;
  m_ValidCount = 0;
  m_Map.InitHashTable(23);
  };

//-----------------------------------------------------------------------------

MRngChkCntxt::~MRngChkCntxt()
  {
  gs_RngChecks.m_List.RemoveAt(m_Position);
  };

//-----------------------------------------------------------------------------

MRngChkItem * MRngChkCntxt::FindItem(LPCSTR Name, LPCSTR Tag)
  {
  CString Cntxt(Tag);
  LPCSTR pObjTag=m_pObject->FullObjTag();
 
  int Len=strlen(pObjTag);
  if (Len<Cntxt.GetLength() && strncmp(Tag, pObjTag, Len)==0)
    {
    if (Cntxt[Len]=='.')
      Cntxt.Delete(0, Len+1);
    else
      Cntxt.Delete(0, Len);
    }

  MRngChkKey Key(Cntxt, Name);
  MRngChkItem *pItem;
  if (m_Map.Lookup(&Key, pItem))
    {
    m_List.RemoveAt(pItem->m_Position);
    pItem->m_Position=m_List.AddHead(pItem);
    }
  else
    {
    pItem=new MRngChkItem(Key);
    pItem->m_Position=m_List.AddHead(pItem);
    m_Map.SetAt(&pItem->m_Key, pItem);
    }

  return pItem;
  };

//-----------------------------------------------------------------------------

void MRngChkCntxt::AddRngChk(LPCSTR Name, TaggedObject * pSrc, bool LTMin, bool GTMax, double MinValue, double MaxValue, double Value)
  {
  MRngChkItem * pItem = FindItem(Name, pSrc->FullObjTag());

  bool IsNew = pItem->m_Iter != gs_RngChecks.m_Iter;
  if (IsNew)
    {

    pItem->m_iCnv     = -1;
    pItem->m_IsMsg    = false;
    pItem->m_LTMin    = LTMin;
    pItem->m_GTMax    = GTMax;
    pItem->m_MinValue = MinValue;
    pItem->m_MaxValue = MaxValue;
    pItem->m_Value    = Value;
    pItem->m_Iter     = gs_RngChecks.m_Iter;

    if (m_Iter!=gs_RngChecks.m_Iter)
      {
      if (m_pObject)
        {
        for (int i=0; i<m_ValidCount; i++)
          m_pObject->ClrCI(FirstRngChkCI+i);
        }
      m_ValidCount = 1;
      }
    else
      m_ValidCount++;
    m_Iter=gs_RngChecks.m_Iter;

    if (m_pObject)
      {
      if (sm_iCndRngChks>=CRC_Basic)
        {
        m_pObject->SetCI(1);
        if (sm_iCndRngChks>=CRC_All)
          {
          if (m_ValidCount<=MaxRngChksPerNode)
            {
            pItem->FormatDisplay(0);
            Strng S;
            S.Set("Wr\t%s", pItem->m_sDisplay);
            m_pObject->SetCI(FirstRngChkCI+m_ValidCount-1, "%s", S);
            }
          }
        }
      }

    dbgpln("MRngChkMngr::AddRngChk() %4i %08x %-30s %-20s %-20s %12.4f %12.4f %12.4f", 
      m_Iter, this, pItem->m_Key.m_sCntxt, pItem->m_Key.m_sName, Name, MinValue, MaxValue, Value);
    }
  };

//-----------------------------------------------------------------------------

void MRngChkCntxt::AddRngChkMsg(LPCSTR Name, TaggedObject * pSrc, LPCSTR Msg)
  {
  MRngChkItem * pItem = FindItem(Name, pSrc->FullObjTag());

  bool IsNew = pItem->m_Iter != gs_RngChecks.m_Iter;
  if (IsNew)
    {
    pItem->m_iCnv     = 0;
    pItem->m_IsMsg    = true;
    pItem->m_LTMin    = false;
    pItem->m_GTMax    = false;

    pItem->m_Iter     = gs_RngChecks.m_Iter;

    pItem->m_sMsg     = Msg;

    if (m_Iter!=gs_RngChecks.m_Iter)
      {
      if (m_pObject)
        {
        for (int i=0; i<m_ValidCount; i++)
          m_pObject->ClrCI(FirstRngChkCI+i);
        }
      m_ValidCount = 1;
      }
    else
      m_ValidCount++;
    m_Iter=gs_RngChecks.m_Iter;

    if (m_pObject)
      {
      if (sm_iCndRngChks>=CRC_Basic)
        {
        m_pObject->SetCI(1);
        if (sm_iCndRngChks>=CRC_All)
          {
          if (IsNew && m_ValidCount<=MaxRngChksPerNode)
            {
            pItem->FormatDisplay(0);
            Strng S;
            S.Set("Wr\t %s", pItem->m_sDisplay);
            m_pObject->SetCI(FirstRngChkCI+m_ValidCount-1, "%s", S);
            }

          }
        }
      }

    dbgpln("MRngChkMngr::AddRngChkMsg() %4i %08x %-30s %-20s %-20s %s", 
      m_Iter, this, pItem->m_Key.m_sCntxt, pItem->m_Key.m_sName, Name, Msg);
    }
  };

//-----------------------------------------------------------------------------

void MRngChkCntxt::CheckRangeList(CRangeListItem Checks[], TaggedObject * pSrc)
  {
  CRangeListItem *pItem=&Checks[0];
  while (pItem->m_Name)
    {
    if (gs_RngChecks.Selected(pItem->m_IdMask))
      {
      bool LTMin = pItem->m_Value<pItem->m_MinValue;
        bool GTMax = pItem->m_Value>pItem->m_MaxValue;
        if (LTMin || GTMax)
          AddRngChk(pItem->m_Name, pSrc, LTMin, GTMax, pItem->m_MinValue, pItem->m_MaxValue, pItem->m_Value);
      }
    pItem++;
    }
  };

//-----------------------------------------------------------------------------

long MRngChkCntxt::RngChkCount()
  {
  int N=0;
  POSITION Pos=m_List.GetHeadPosition();
  while (Pos)
    {
    MRngChkItem &I = *m_List.GetNext(Pos);
    if (I.m_Iter==gs_RngChecks.m_Iter)
      N++;
    else
      break;
    }
  return N;
  };

//-----------------------------------------------------------------------------

LPCSTR MRngChkCntxt::RngChkDisplay(long Index)
  {
  int N=0;
  POSITION Pos=m_List.GetHeadPosition();
  while (Pos)
    {
    MRngChkItem &I = *m_List.GetNext(Pos);
    if (N<Index)
      N++;
    else
      return I.FormatDisplay(0);
    }
  return "";
  };

//=============================================================================
//
//=============================================================================

MRngChks::MRngChks(TaggedObject * pSrc, LPCSTR Context)
  {
  m_pSrc =  pSrc;
  };

//-----------------------------------------------------------------------------

MRngChks::~MRngChks()
  {
  };

//-----------------------------------------------------------------------------

MRngChkCntxt  * MRngChks::Context()
  {
  TaggedObject * p=m_pSrc;
  while (p)
    {
    MRngChkCntxt *pC = dynamic_cast<MRngChkCntxt*>(p);
    if (pC)
      return pC;
    p=p->pAttachedTo;
    }
  return NULL;
  };

//=============================================================================
//
//=============================================================================

MRngChkMngr::MRngChkMngr()
  {
  m_Iter=0;
  m_SelectMask = /*RngChk_T |*/ RngChk_TExtn | RngChk_P | RngChk_Rho | RngChk_Comp | RngChk_Other ;
  };

//-----------------------------------------------------------------------------

void MRngChkMngr::BumpGlblIter()
  {
  m_Iter++;
  
  dbgpln("MRngChkMngr::BumpGlblIter() %i", m_Iter);
  };

//-----------------------------------------------------------------------------

void MRngChkMngr::Dump()
  {
  POSITION Pos=m_List.GetHeadPosition();
  while (Pos)
    {
    MRngChkCntxt &C=*m_List.GetNext(Pos);
    //dbgpln("MRngChkMngr::Dump %4i %08x %i  %s", m_Iter, this, C.m_Iter, C.RngChkTag());
    if (C.m_Iter==m_Iter)
      {
      //dbgpln("RngChk %s:%s", C.RngChkTag());
      POSITION PosI=C.m_List.GetHeadPosition();
      while (PosI)
        {
        MRngChkItem &I=*C.m_List.GetNext(PosI);
        dbgp("  %-20s %08x", I.m_Key.m_sCntxt, &I);
        if (I.m_GTMax)
          dbgp("  %-20s %.4f > %.4f", I.m_Key.m_sName, I.m_Value, I.m_MaxValue);
        if (I.m_LTMin)
          dbgp("  %-20s %.4f < %.4f", I.m_Key.m_sName, I.m_Value, I.m_MinValue);
        dbgpln("");
        
        }
      }
    }
  };

MRngChkMngr gs_RngChecks;

#endif
//===========================================================================
//
//
//
//===========================================================================

void MLog::Message(MMessageType Type, LPCSTR pFmt, ...)
  {
  char Buff[2048];
  va_list argptr;
  va_start(argptr, pFmt);
  vsprintf(Buff, pFmt, argptr);
  va_end(argptr);

  if (Buff[0])//strlen(Buff)>0)
    {
    static DWORD What[] = {LogFlag_Note, LogFlag_Error, LogFlag_Warning, LogFlag_Note };
    LogLn(LogFlag_FromBlackBox|What[Type], m_pObj ? m_pObj->FullObjTag() : "SysCAD", 0/*BoxFlags*/, Buff);
    }
  }

//---------------------------------------------------------------------------

void MLog::SetCondition(bool On, long index, MMessageType Type, LPCSTR pFmt, ...)
  {
  if (index>=0 && index<MaxUserCI)
    {
    if (On)
      {
      char Buff[2048];
      va_list argptr;
      va_start(argptr, pFmt);
      vsprintf(Buff, pFmt, argptr);
      va_end(argptr);

      if (Buff[0])//strlen(Buff)>0)
        {
        static LPCSTR What[/*MMessageType*/] = {"?", "E", "W", "N" };
        m_pObj->SetCIVirt(FirstUserCI+index+m_pObj->CBContextVirt(), "%s\t%s", What[Type], Buff);
        }
      }
    else
      {
      m_pObj->ClrCIVirt(FirstUserCI+index+m_pObj->CBContextVirt());
      }
    }
  }

//---------------------------------------------------------------------------

void MLog::SetCondition(long index, MMessageType Type, LPCSTR pFmt, ...)
  {
  if (index>=0 && index<MaxUserCI)
    {
    char Buff[2048];
    va_list argptr;
    va_start(argptr, pFmt);
    vsprintf(Buff, pFmt, argptr);
    va_end(argptr);

    if (Buff[0])//strlen(Buff)>0)
      {
      static LPCSTR What[/*MMessageType*/] = {"?", "E", "W", "N" };
      m_pObj->SetCIVirt(FirstUserCI+index+m_pObj->CBContextVirt(), "%s\t%s", What[Type], Buff);
      }
    }
  }

//---------------------------------------------------------------------------

void MLog::ClearCondition(long index)
  {
  if (index>=0 && index<MaxUserCI)
    {
    m_pObj->ClrCIVirt(FirstUserCI+index+m_pObj->CBContextVirt());
    }
  }

MLog gs_Log;

//===========================================================================
//
//
//
//===========================================================================

MDebug::MDebug()                        { m_pObj=NULL; };
void MDebug::Init(TaggedObject * pObj)  { m_pObj=pObj; };
bool MDebug::Marked()                   { return m_pObj ? m_pObj->DoDbgBrk()!=NULL : false; };
void MDebug::Indent(int iIndent)        { dbgindent(iIndent); };
void MDebug::Lock()                     { dbglock();          };
void MDebug::UnLock()                   { dbgunlock();        };

//---------------------------------------------------------------------------

void MDebug::Print(LPCSTR pFmt, ...)
  {
  char Buff[2048];
  va_list argptr;
  va_start(argptr, pFmt);
  vsprintf(Buff, pFmt, argptr);
  va_end(argptr);
  dbgp("%s", Buff);
  };

//---------------------------------------------------------------------------

void MDebug::PrintLn(LPCSTR pFmt, ...)
  {
  char Buff[2048];
  va_list argptr;
  va_start(argptr, pFmt);
  vsprintf(Buff, pFmt, argptr);
  va_end(argptr);
  dbgpln("%s", Buff);
  };

//===========================================================================

MDebug gs_Dbg;

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
