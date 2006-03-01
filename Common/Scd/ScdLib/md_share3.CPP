//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#define  __MD_SHARE3_CPP
#include "sc_defs.h"
#include "md_share3.h"

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
