// CodeBlocks.cpp: implementation of the CCdBlks class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SysCADMarshal.h"
#include "CodeBlock.h"
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

CCdBlkCfg::CCdBlkCfg()
  {
  }

CCdBlkCfg::~CCdBlkCfg()
  {
  }

CCdBlkCfg::CCdBlkCfg(const CCdBlkCfg & V)
  {
  *this=V;
  }

CCdBlkCfg & CCdBlkCfg::operator =(const CCdBlkCfg & V)
  {
  m_lCdBlk        = V.m_lCdBlk;
  m_sTag          = V.m_sTag;
  m_sSchedule     = V.m_sSchedule;
  m_sCdBlk        = V.m_sCdBlk;
  m_sDescription  = V.m_sDescription;
  m_lCfgSequence  = V.m_lCfgSequence;
  m_State         = V.m_State;

  return *this;
  }

void CCdBlkCfg::SetError(USHORT Error)
  {
  m_State.m_nError=Error;
  if (Error)
    ReportError(m_sTag, 0, "%s", SErr_String(Error));
  };
void CCdBlkCfg::SetError(USHORT Error, LPCSTR Fmt, ...)
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
void CCdBlkCfg::SetError(USHORT Error, HRESULT hr, LPCSTR Fmt, ...)
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


// =======================================================================
//
//
//
// =======================================================================

CCdBlk::CCdBlk() : m_Code("pgm")
  {                                
  m_bValid=false;
  }

// -----------------------------------------------------------------------

CCdBlk::~CCdBlk()
  {

  };

// -----------------------------------------------------------------------

bool CCdBlk::Parse(CAddCdBlk * pAdd, LPCSTR CdFile)
  {
  m_bValid=true;

  m_Code.m_bLogAllBuildErrs=true;
  if (m_Code.Load(AF_All|AF_BackupFiles, (LPSTR)CdFile, &gs_SlotMngr, DefGenConSet))
    {
    m_sTag=pAdd->m_sTag;
    m_sCdBlk=pAdd->m_sCdBlk;
    m_sDescription=pAdd->m_sDescription;

    //m_CodeBlkMap.SetAt(p->m_sTag(), UsedCodeBlks);
    //UsedCodeBlks=m_CodeBlks.Add(p)+1;
    }
  else
    {
    ReportError(pAdd->m_sTag, 0, "CdBlk %s not loaded", CdFile);
    m_bValid=false;
    }


  return m_bValid;
  };

// -----------------------------------------------------------------------

void CCdBlk::RemoveConnects()
  {
  m_ReflectedGets.SetSize(0);
  }

// -----------------------------------------------------------------------

void CCdBlk::InitialiseConnects()
  {
  m_ReflectedGets.SetSize(0);
  }

// =======================================================================
//
//
//
// =======================================================================
