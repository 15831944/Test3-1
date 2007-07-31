//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
  
#include "stdafx.h"
#include "sc_defs.h"
#include "gpfuncs.h"
#include "ole_base.h"
#include "errorlog.h"
#include "scd_wm.h"

//===========================================================================

UINT wm_SyscadOleCommand = 0;
CWnd* pOleInfoWnd = NULL;
BOOL UsingExcel8or9 = TRUE;

//===========================================================================

COleVariant GetVariant(PkDataUnion* pItem, CCnvIndex iCnv, char* pCnvTxt)
  {
  COleVariant V;
  V.ChangeType(VT_EMPTY);
  switch (pItem->iType)
    {
    case tt_Char   : V.ChangeType(VT_I2); V.iVal = pItem->Char; break;
    case tt_Bool   : 
    case tt_Bit    : V.ChangeType(VT_BOOL); V.boolVal = pItem->Byte; break;
    case tt_Byte   : V.ChangeType(VT_UI1); V.bVal = pItem->Byte; break;
    case tt_Word   : V.ChangeType(VT_I4); V.lVal = pItem->Word; break;
    case tt_DWord  : V.ChangeType(VT_R8); V.dblVal = (double)(pItem->DWord); break;
    case tt_Int    : V.ChangeType(VT_I4); V.lVal = pItem->Int; break;
    case tt_Short  : V.ChangeType(VT_I2); V.iVal = pItem->Short; break;
    case tt_Long   : V.ChangeType(VT_I4); V.lVal = pItem->Long; break;
    case tt_Flt16  : V.ChangeType(VT_R8); V.dblVal = Cnvs[(iCnv)]->Human(pItem->Flt16, pCnvTxt); break;
    case tt_Float  : V.ChangeType(VT_R8); V.dblVal = Cnvs[(iCnv)]->Human(pItem->Float, pCnvTxt); break;
    case tt_Double : V.ChangeType(VT_R8); V.dblVal = ((iCnv)==0 ? pItem->Double : Cnvs[(iCnv)]->Human(pItem->Double, pCnvTxt)); break;
    case tt_pChar  :
    case tt_ppChar :
    case tt_Strng  : V = (const char*)((char*)&(pItem->Len)+sizeof(pItem->Len)); break;
    case tt_NULL : break;
    default : VERIFY(0); break;
    }
  return V;
  }

//===========================================================================

OVariant::OVariant(BOOL b)
  {
  vt = VT_BOOL;
  boolVal = b;
  }

//---------------------------------------------------------------------------

OVariant::OVariant(LPDISPATCH v)
  {
  vt = VT_DISPATCH;
  pdispVal = v;
  }

//---------------------------------------------------------------------------

BOOL OVariant::ForceType(VARTYPE vartype)
  {
  if (vartype==vt)
    return TRUE;

  BOOL OK = TRUE;
  TRY
    {
    ChangeType(vartype);
    }
  // Catch COleException from ChangeType, but not CMemoryException
  CATCH(COleException, e)
    {
    OK = FALSE;
    vt = vartype;
    switch (vt)
      {
      case VT_EMPTY:
      case VT_NULL: break;
      case VT_BOOL: boolVal = 0; break;
      case VT_UI1: bVal = 0; break;
      case VT_I2: iVal = 0; break;
      case VT_I4: lVal = 0; break;
      case VT_CY: cyVal.Hi = 0; cyVal.Lo = 0; break;
      case VT_R4: fltVal = 0.0F; break;
      case VT_R8: dblVal = 0.0; break;
      case VT_DATE: date = 0.0; break;
      case VT_BSTR: bstrVal = NULL; break;
      case VT_ERROR: scode = 0; break;
      default: 
        vt = VT_EMPTY;
        ASSERT(FALSE);  //do not know what to do with these
        break;
      }
    //DELETE_EXCEPTION(e);
    }
  END_CATCH
  return OK;
  }

//---------------------------------------------------------------------------

void OVariant::Set(BOOL v)
  {
  Clear();
  vt = VT_BOOL;
  boolVal = v;
  }

//---------------------------------------------------------------------------

void OVariant::Set(short v)
  {
  Clear();
  vt = VT_I2;
  iVal = v;
  }

//---------------------------------------------------------------------------

void OVariant::Set(long v)
  {
  Clear();
  vt = VT_I4;
  lVal = v;
  }

//---------------------------------------------------------------------------

void OVariant::Set(LPDISPATCH v)
  {
  Clear();
  vt = VT_DISPATCH;
  pdispVal = v;
  }

//===========================================================================

COleBaseAuto::COleBaseAuto(CExecObj* ExecObj, CXM_Route* pRoute, char* FileName, flag Lock)
  {
  pExecObj = ExecObj;
  xRoute.ReverseRoute(*pRoute);
  sFileName = FileName;
  sFileName.FnSearchExpand(NULL, AF_All|AF_BackupFiles);
  bPrompt = 0;
  bLock = Lock;
  bActivateSysCAD = 1;
  m_pComCmd = NULL;
  }

//---------------------------------------------------------------------------

COleBaseAuto::~COleBaseAuto()
  {
  }

//---------------------------------------------------------------------------

void COleBaseAuto::ActivateSysCAD()
  {
  if (bActivateSysCAD)
    {
    ScdMainWnd()->PostMessage(WMU_UPDATEMAINWND, SUB_UPDMAIN_ACTIVATEAPP, 0); //get main window thread to top AND activate SysCAD
    }
  }

//---------------------------------------------------------------------------

void COleBaseAuto::ReportError(long ErrNo, char * fmt, ...)
  {
  char Buff[1024];
  va_list argptr;
  va_start(argptr, fmt);
  vsprintf(Buff, fmt, argptr);
  va_end(argptr);
  ReportLogMsg(ErrNo, Buff, TRUE);
  }

//---------------------------------------------------------------------------

void COleBaseAuto::ReportWarning(long ErrNo, char * fmt, ...)
  {
  char Buff[1024];
  va_list argptr;
  va_start(argptr, fmt);
  vsprintf(Buff, fmt, argptr);
  va_end(argptr);
  ReportLogMsg(ErrNo, Buff, FALSE);
  }

//---------------------------------------------------------------------------

void COleBaseAuto::ReportLogMsg(long ErrNo, char* pMsg, BOOL Err)
  {
  if (ErrNo)
    {
    if (Err)
      LogError(OLEServerDesc(), 0, "%s: %s", ShortDesc(), pMsg);
    else
      LogWarning(OLEServerDesc(), 0, "%s: %s", ShortDesc(), pMsg);
    }
  //do we still need this code? ...
  CXM_OleErrorCode *xb=new CXM_OleErrorCode (ErrNo, pMsg);
  CXMsgLst XMr;
  XMr.PackMsg(xb);
  pExecObj->XSendMessage(XMr, xRoute);
  }

//---------------------------------------------------------------------------

void COleBaseAuto::Prompt(char * fmt, ...)
  {
  char Buff[1024];
  va_list argptr;
  va_start(argptr, fmt);
  vsprintf(Buff, fmt, argptr);
  va_end(argptr);
  if (bPrompt)
    AfxMessageBox(Buff);
  }

//---------------------------------------------------------------------------

void COleBaseAuto::SendInfo(char * fmt, ...)
  {
  char Buff[1024];
  va_list argptr;
  va_start(argptr, fmt);
  vsprintf(Buff, fmt, argptr);
  va_end(argptr);
  COleInfoMsg* pMsg = new COleInfoMsg(Buff);
  pOleInfoWnd->SendMessage(WMU_OLEINFOUPDATE, 0, (LPARAM)pMsg);
  }

//---------------------------------------------------------------------------
//===========================================================================
