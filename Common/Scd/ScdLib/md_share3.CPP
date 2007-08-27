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

//===========================================================================
//
//
//
//===========================================================================

long MSMFnRanges::sm_Iter=-1;

MSMFnRanges::MSMFnRanges()
  { 
  Clear();
  };
void MSMFnRanges::Clear()
  { 
  m_Flags=0; 
  m_Masks=0; 
  m_Iter=-2;
  };
void MSMFnRanges::SetInRange(eSpFns FnInx, bool OK)
  {
#if WITHSPMDLRANGES
  if (m_Iter!=sm_Iter)
    {
    m_Iter=sm_Iter;
    m_Masks=0;
    }
  long Msk=1<<(int)FnInx;
  if ((m_Masks & Msk)==0)
    m_Flags = m_Flags | Msk;
  if (!OK)
    m_Flags = m_Flags & ~Msk;

  //if (OK)
  //  m_Flags = m_Flags | Msk;
  //else
  //  m_Flags = m_Flags & ~Msk;

  m_Masks = m_Masks | Msk;
#endif
  };
bool MSMFnRanges::InRange(eSpFns FnInx)
  {
#if WITHSPMDLRANGES
  if (m_Iter!=sm_Iter)
    return true;
  long Msk=1<<(int)FnInx;
  return (m_Flags & Msk)!=0 || (m_Masks & Msk)==0;
#else
  return true;
#endif
  };
bool MSMFnRanges::Valid(eSpFns FnInx)
  {
#if WITHSPMDLRANGES
  if (m_Iter!=sm_Iter)
    return true;
  long Msk=1<<(int)FnInx;
  return (m_Masks & Msk) != 0;
#else
  return true;
#endif
  };
bool MSMFnRanges::AllInRange()
  {
#if WITHSPMDLRANGES
  if (m_Iter!=sm_Iter)
    return true;
  return (m_Flags & m_Masks) == m_Masks;
#else
  return true;
#endif
  };
void MSMFnRanges::BumpGlblIter()
  {
  sm_Iter++;
  };

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
