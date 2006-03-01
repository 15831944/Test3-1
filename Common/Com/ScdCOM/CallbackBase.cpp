w// implementation of the CScdCOMBase class.
//
//////////////////////////////////////////////////////////////////////
 
#include "stdafx.h"

#define __SCDCOMCALLBACK_CPP

#include "ScdCOMIF.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScdCOMBase::CScdCOMBase()
  {
  m_pHook=NULL;
  }

CScdCOMBase::~CScdCOMBase()
  {
  }

void CScdCOMBase::DoEventMsg(UINT e, LPARAM l) 
  { 
  if (m_pHook)
	m_pHook->DoEventMsg(e, l);
  };
HRESULT CScdCOMBase::ReportError(LPCTSTR szDesc, UINT Code) 
  {
  AtlTrace("ScdCOMBase Report Error %i, %s\n", Code, szDesc);
  return m_pHook->ReportError(szDesc, Code); 
  };
void CScdCOMBase::Initialise(CSysCADCmdHookBack* Hook)
  {
  m_pHook=NULL;//Hook;
  }
