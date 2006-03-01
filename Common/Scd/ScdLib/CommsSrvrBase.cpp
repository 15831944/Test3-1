//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
#define  __COMMSSRVRBASE_CPP
#include "CommsSrvrBase.h"

//===========================================================================

CCommsServerManagerBase::CCommsServerManagerBase()
  {
  }

//===========================================================================

flag bOPCServerAllowed = 0;

flag OPCServerAllowed()
  {
  return bOPCServerAllowed;
  }

void SetOPCServerAllowed(flag Allowed)
  {
  bOPCServerAllowed = Allowed;
  }

//===========================================================================
