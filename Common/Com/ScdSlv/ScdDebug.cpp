// ScdDebug.cpp : Implementation of CScdDebug
#include "stdafx.h"
#include "ScdSlv.h"
#include "ScdDebug.h"
#include "..\..\..\SMDK\Include\DebugLib.h"

/////////////////////////////////////////////////////////////////////////////
// CScdDebug


STDMETHODIMP CScdDebug::FileClose()
  {
  dllSCD_COMENTRY(long)
    {
    dbgfileclose();
    }		
	SCD_COMEXIT
  }

STDMETHODIMP CScdDebug::FileOpen(BSTR FileName)
  {
  USES_CONVERSION;
  dllSCD_COMENTRY(long)
    {
    dbgfileopen(OLE2T(FileName));
    }		
	SCD_COMEXIT
  }
