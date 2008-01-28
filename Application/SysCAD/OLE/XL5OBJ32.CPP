//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* KGA: 11/6/96: This code is based on the machine generated IDispatch driver 
   classes created with ClassWizard. The required objects with the required 
   methods were copied from xl5en32.h and xl5en32.cpp. These two files can be
   regenerated using class wizard OLE automation functionality by selecting 
   all objects. These are based on Excel version 5 32 bit edition. The 
   interface, naming etc have been changed to hide more of the OLE stuff and
   to make the interface more simple and direct. For further references see 
   vba_xl.hlp which is shipped with Excel 5. Also see autodemo sample project
   found on Microsoft Developer Network CD October 95. */

/* KGA: 12/6/97: This code has been extended to use Excel version 8. As before,
   the class wizard was used to generate excel8.h and excel8.cpp. A global flag
   is set determining which version is being used, the commands have been extended
   to call the correct OLE dispatch etc based on the Excel version. ie One set of 
   classes calls the required Excel version. */

/* KGA: 5/7/99: Above process repeated to use Excel version 9 (part of Office2000).
   Excel 8 and Excel 9 appear to be identical.*/
//===========================================================================

//#pragma warning (disable : 4237) // keyword reserved for future use : eg flag

#include "stdafx.h"
#include "sc_defs.h"
#include "xl5obj32.h"
#include "ole_base.h"
//#include "optoff.h"

//===========================================================================

LPDISPATCH OExcelApplication::GetActiveSheet()
  {
  LPDISPATCH lpDispatch;
  if (UsingExcel8or9)
    InvokeHelper(0x133, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&lpDispatch, NULL);
  else
    GetProperty(0x133, VT_DISPATCH, (void*)&lpDispatch);
  return lpDispatch;
  }

LPDISPATCH OExcelApplication::GetActiveWindow()
  {
  LPDISPATCH lpDispatch;
  if (UsingExcel8or9)
    InvokeHelper(0x2f7, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&lpDispatch, NULL);
  else
    GetProperty(0x2f7, VT_DISPATCH, (void*)&lpDispatch);
  return lpDispatch;
  }

void OExcelApplication::SetDefaultFilePath(LPCTSTR lpszNewValue)
  {
  static BYTE parms[] = VTS_BSTR;
  InvokeHelper(0x40e, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, lpszNewValue);
  }

void OExcelApplication::SetScreenUpdating(BOOL bUpdating)
  {
  if (UsingExcel8or9)
    {
    static BYTE parms[] = VTS_BOOL;
    InvokeHelper(0x17e, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, bUpdating);
    }
  else
    {
    OVariant propVal(bUpdating);
    SetProperty(0x17e, VT_VARIANT, &propVal);
    }
  }

void OExcelApplication::SetStatusBar(LPCTSTR pStatusBarTxt)
  {
  static BYTE parms[] = VTS_BSTR;
  InvokeHelper(0x182, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, pStatusBarTxt);
  }

void OExcelApplication::RestoreStatusBar()
  {
  static BYTE parms[] = VTS_BOOL;
  InvokeHelper(0x182, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, FALSE);
  }

BOOL OExcelApplication::GetVisible()
  {
  if (UsingExcel8or9)
    {
    BOOL result;
    InvokeHelper(0x22e, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
    return result;
    }
  COleVariant result;
  GetProperty(0x22e, VT_VARIANT, (void*)&result);
  return result.boolVal;
  }

void OExcelApplication::SetVisible(BOOL bVisible)
  {
  if (UsingExcel8or9)
    {
    static BYTE parms[] = VTS_BOOL;
    InvokeHelper(0x22e, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, bVisible);
    }
  else
    {
    OVariant propVal(bVisible);
    SetProperty(0x22e, VT_VARIANT, &propVal);
    }
  }

long OExcelApplication::GetCalculation()
  {
  long result;
  InvokeHelper(0x13c, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
  return result;
  }

void OExcelApplication::SetCalculation(long nNewValue)
  {
  static BYTE parms[] = VTS_I4;
  InvokeHelper(0x13c, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, nNewValue);
  }

void OExcelApplication::Quit()
  {
  if (UsingExcel8or9)
    InvokeHelper(0x12e, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
  else
    {
    VARIANT result;
    InvokeHelper(0x12e, DISPATCH_METHOD, VT_VARIANT, (void*)&result, NULL);
    //return result;
    }
  }

LPDISPATCH OExcelApplication::Workbooks(short iIndex)
  {
  COleVariant Index(iIndex);
  LPDISPATCH lpDispatch;
  static BYTE BASED_CODE parms[] = VTS_VARIANT;
  if (UsingExcel8or9)
    InvokeHelper(0x23c, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&lpDispatch, parms, &Index);
  else
    InvokeHelper(0x23c, DISPATCH_METHOD, VT_DISPATCH, (void*)&lpDispatch, parms, &Index);
  return lpDispatch;
  }

LPDISPATCH OExcelApplication::Workbooks(char* pName)
  {
  COleVariant Name(pName);
  LPDISPATCH lpDispatch;
  static BYTE BASED_CODE parms[] = VTS_VARIANT;
  if (UsingExcel8or9)
    InvokeHelper(0x23c, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&lpDispatch, parms, &Name);
  else
    InvokeHelper(0x23c, DISPATCH_METHOD, VT_DISPATCH, (void*)&lpDispatch, parms, &Name);
  return lpDispatch;
  }

LPDISPATCH OExcelApplication::Workbooks()
  {
  LPDISPATCH lpDispatch;
  if (UsingExcel8or9)
    InvokeHelper(0x23c, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&lpDispatch, NULL);
  else
    InvokeHelper(0x23c, DISPATCH_METHOD, VT_DISPATCH, (void*)&lpDispatch, NULL);
  return lpDispatch;
  }

LPDISPATCH OExcelApplication::Worksheets(char* pName)
  {
  COleVariant Name(pName);
  LPDISPATCH lpDispatch;
  static BYTE BASED_CODE parms[] = VTS_VARIANT;
  if (UsingExcel8or9)
    InvokeHelper(0x1ee, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&lpDispatch, parms, &Name);
  else
    InvokeHelper(0x1ee, DISPATCH_METHOD, VT_DISPATCH, (void*)&lpDispatch, parms, &Name);
  return lpDispatch;
  }

LPDISPATCH OExcelApplication::Worksheets(short iIndex)
  {
  COleVariant Index(iIndex);
  LPDISPATCH lpDispatch;
  static BYTE BASED_CODE parms[] = VTS_VARIANT;
  if (UsingExcel8or9)
    InvokeHelper(0x1ee, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&lpDispatch, parms, &Index);
  else
    InvokeHelper(0x1ee, DISPATCH_METHOD, VT_DISPATCH, (void*)&lpDispatch, parms, &Index);
  return lpDispatch;
  }

LPDISPATCH OExcelApplication::Worksheets()
  {
  LPDISPATCH lpDispatch;
  if (UsingExcel8or9)
    InvokeHelper(0x1ee, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&lpDispatch, NULL);
  else
    InvokeHelper(0x1ee, DISPATCH_METHOD, VT_DISPATCH, (void*)&lpDispatch, NULL);
  return lpDispatch;
  }

VARIANT OExcelApplication::Run(const VARIANT& Macro)//, const VARIANT& Arg1, const VARIANT& Arg2, const VARIANT& Arg3, const VARIANT& Arg4, const VARIANT& Arg5, const VARIANT& Arg6, const VARIANT& Arg7, const VARIANT& Arg8, const VARIANT& Arg9, 
		//const VARIANT& Arg10, const VARIANT& Arg11, const VARIANT& Arg12, const VARIANT& Arg13, const VARIANT& Arg14, const VARIANT& Arg15, const VARIANT& Arg16, const VARIANT& Arg17, const VARIANT& Arg18, const VARIANT& Arg19, 
		//const VARIANT& Arg20, const VARIANT& Arg21, const VARIANT& Arg22, const VARIANT& Arg23, const VARIANT& Arg24, const VARIANT& Arg25, const VARIANT& Arg26, const VARIANT& Arg27, const VARIANT& Arg28, const VARIANT& Arg29, 
		//const VARIANT& Arg30)
  {
  VARIANT result;
  static BYTE parms[] =
    VTS_VARIANT;// VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT 
    //VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT;
  InvokeHelper(0x103, DISPATCH_METHOD, VT_VARIANT, (void*)&result, parms,
    &Macro);//, &Arg1, &Arg2, &Arg3, &Arg4, &Arg5, &Arg6, &Arg7, &Arg8, &Arg9, &Arg10, &Arg11, &Arg12, &Arg13, &Arg14, &Arg15, &Arg16, &Arg17, &Arg18, &Arg19, &Arg20, &Arg21, &Arg22, &Arg23, &Arg24, &Arg25, &Arg26, &Arg27, &Arg28, &Arg29, &Arg30);
  return result;
  }

//===========================================================================

short ORange::GetColumn()
  {
  COleVariant result;
  GetProperty(0xf0, VT_VARIANT, (void*)&result);
  return result.iVal;
  }

void ORange::SetColumnWidth(short Width)
  {
  COleVariant propVal(Width);
  SetProperty(0xf2, VT_VARIANT, &propVal);
  }

LPDISPATCH ORange::GetFont()
  {
  LPDISPATCH lpDispatch;
  GetProperty(0x92, VT_DISPATCH, (void*)&lpDispatch);
  return lpDispatch;
  }

VARIANT ORange::GetFormula()
  {
  VARIANT result;
  GetProperty(0x105, VT_VARIANT, (void*)&result);
  return result;
  }

void ORange::SetNumberFormat(char* pFormat)
  {
  COleVariant propVal(pFormat);
  SetProperty(0xc1, VT_VARIANT, &propVal);
  }

short ORange::GetRow()
  {
  COleVariant result;
  GetProperty(0x101, VT_VARIANT, (void*)&result);
  return result.iVal;
  }

VARIANT ORange::GetValue()
  {
  VARIANT result;
  GetProperty(0x6, VT_VARIANT, (void*)&result);
  return result;
  }

short ORange::GetShortValue()
  {
  OVariant V = GetValue();
  V.ForceType(VT_I2);
  return V.iVal;
  }

long ORange::GetLongValue()
  {
  OVariant V = GetValue();
  V.ForceType(VT_I4);
  return V.lVal;
  }

double ORange::GetDoubleValue()
  {
  OVariant V = GetValue();
  V.ForceType(VT_R8);
  return V.dblVal;
  }

CString ORange::GetStringValue()
  {
  OVariant V = GetValue();
  V.ForceType(VT_BSTR);
  CString s = V.bstrVal;
  return s;
  }

void ORange::SetValue(char* pTxt)
  {
  COleVariant propVal(pTxt);
  SetProperty(0x6, VT_VARIANT, &propVal);
  }

void ORange::SetValue(double Val)
  {
  COleVariant propVal(Val);
  SetProperty(0x6, VT_VARIANT, &propVal);
  }

void ORange::SetValue(const VARIANT& propVal)
  {
  SetProperty(0x6, VT_VARIANT, &propVal);
  }

VARIANT ORange::Activate()
  {
  VARIANT result;
  InvokeHelper(0x130, DISPATCH_METHOD, VT_VARIANT, (void*)&result, NULL);
  return result;
  }

VARIANT ORange::Clear()
  {
  VARIANT result;
  InvokeHelper(0x6f, DISPATCH_METHOD, VT_VARIANT, (void*)&result, NULL);
  return result;
  }

LPDISPATCH ORange::Find(char* pWhat)
  {
  LPDISPATCH lpDispatch;
  COleVariant What(pWhat);
  if (1)
    {//KGA Jan2008: changed the find function to use XlPart and MatchCase=false because state of previous Excel find may be wrong
    VARIANT argNULL;
    VariantInit(&argNULL); 
    V_VT(&argNULL) = VT_ERROR; 
    V_ERROR(&argNULL) = DISP_E_PARAMNOTFOUND;
    OVariant LookAt((unsigned char)2); //xlWhole==1; xlPart==2
    BOOL bMatchCase = false;
    OVariant MatchCase(bMatchCase);
    static BYTE parms[] = VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT;
    InvokeHelper(0x18e, DISPATCH_METHOD, VT_DISPATCH, (void*)&lpDispatch, parms, &What, &argNULL, &argNULL, &LookAt, &argNULL, &argNULL, &MatchCase);
    //Cells.Find(What:="SysCAD_", After:=ActiveCell, LookIn:=xlFormulas, LookAt:=xlPart, SearchOrder:=xlByRows, SearchDirection:=xlNext, MatchCase:=False, SearchFormat:=False).Activate
    }
  else
    {
    static BYTE parms[] = VTS_VARIANT;
    InvokeHelper(0x18e, DISPATCH_METHOD, VT_DISPATCH, (void*)&lpDispatch, parms, &What);
    //, &After, &LookIn, &LookAt, &SearchOrder, &SearchDirection, &MatchCase);
    }
  return lpDispatch;
  }

LPDISPATCH ORange::FindNext(LPDISPATCH lpDispatchAfter)
  {
  LPDISPATCH lpDispatch;
  OVariant After(lpDispatchAfter);
  //After.ChangeType(VT_DISPATCH);
  //After.vt = VT_DISPATCH;
  //After.pdispVal = lpDispatchAfter;
  //After.Set(lpDispatchAfter);
  static BYTE parms[] = VTS_VARIANT;//VTS_DISPATCH;
  InvokeHelper(0x18f, DISPATCH_METHOD, VT_DISPATCH, (void*)&lpDispatch, parms, &After);
  return lpDispatch;
  }

LPDISPATCH ORange::FindNext(const VARIANT& After)
  {
  LPDISPATCH lpDispatch;
  static BYTE parms[] = VTS_VARIANT;
  InvokeHelper(0x18f, DISPATCH_METHOD, VT_DISPATCH, (void*)&lpDispatch, parms, &After);
  return lpDispatch;
  }

//===========================================================================

LPDISPATCH OWorkbook::GetActiveChart()
  {
  LPDISPATCH lpDispatch;
  GetProperty(0xb7, VT_DISPATCH, (void*)&lpDispatch);
  return lpDispatch;
  }

LPDISPATCH OWorkbook::GetActiveSheet()
  {
  LPDISPATCH lpDispatch;
  GetProperty(0x133, VT_DISPATCH, (void*)&lpDispatch);
  return lpDispatch;
  }

CString OWorkbook::GetFullName()
  {
  VARIANT result;
  GetProperty(0x121, VT_VARIANT, (void*)&result);
  CString s = result.bstrVal;
  return s;
  }

CString OWorkbook::GetName()
  {
  VARIANT result;
  GetProperty(0x6e, VT_VARIANT, (void*)&result);
  CString s = result.bstrVal;
  return s;
  }

BOOL OWorkbook::GetReadOnly()
  {
  VARIANT result;
  GetProperty(0x128, VT_VARIANT, (void*)&result);
  return result.boolVal;
  }

BOOL OWorkbook::GetSaved()
  {
  BOOL result;
  InvokeHelper(0x12a, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
  return result;
  }

void OWorkbook::SetSaved(BOOL bSaved)
  {
  OVariant propVal(bSaved);
  SetProperty(0x12a, VT_VARIANT, &propVal);
  }

VARIANT OWorkbook::Activate()
  {
  VARIANT result;
  InvokeHelper(0x130, DISPATCH_METHOD, VT_VARIANT, (void*)&result, NULL);
  return result;
  }

LPDISPATCH OWorkbook::Charts(short iIndex)
  {
  COleVariant Index(iIndex);
  Index.ChangeType(VT_I2);
  LPDISPATCH lpDispatch;
  static BYTE BASED_CODE parms[] = VTS_VARIANT;
  if (UsingExcel8or9)
    InvokeHelper(0x79, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&lpDispatch, parms, &Index);
  else
    InvokeHelper(0x79, DISPATCH_METHOD, VT_DISPATCH, (void*)&lpDispatch, parms, &Index);
  return lpDispatch;
  }

LPDISPATCH OWorkbook::Charts()
  {
  LPDISPATCH lpDispatch;
  if (UsingExcel8or9)
    InvokeHelper(0x79, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&lpDispatch, NULL);
  else
    InvokeHelper(0x79, DISPATCH_METHOD, VT_DISPATCH, (void*)&lpDispatch, NULL);
  return lpDispatch;
  }

void OWorkbook::Close(BOOL bSaveChanges)
  {
  OVariant SaveChanges(bSaveChanges);
  static BYTE parms[] = VTS_VARIANT;// VTS_VARIANT VTS_VARIANT;
  InvokeHelper(0x115, DISPATCH_METHOD, VT_EMPTY, NULL, parms, &SaveChanges);//, &Filename, &RouteWorkbook);
  }

VARIANT OWorkbook::Save()
  {
  VARIANT result;
  InvokeHelper(0x11b, DISPATCH_METHOD, VT_VARIANT, (void*)&result, NULL);
  return result;
  }

LPDISPATCH OWorkbook::Windows(short iIndex)
  {
  COleVariant Index(iIndex);
  Index.ChangeType(VT_I2);
  LPDISPATCH lpDispatch;
  static BYTE BASED_CODE parms[] = VTS_VARIANT;
  if (UsingExcel8or9)
    InvokeHelper(0x1ae, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&lpDispatch, parms, &Index);
  else
    InvokeHelper(0x1ae, DISPATCH_METHOD, VT_DISPATCH, (void*)&lpDispatch, parms, &Index);
  return lpDispatch;
  }

LPDISPATCH OWorkbook::Windows()
  {
  LPDISPATCH lpDispatch;
  if (UsingExcel8or9)
    InvokeHelper(0x1ae, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&lpDispatch, NULL);
  else
    InvokeHelper(0x1ae, DISPATCH_METHOD, VT_DISPATCH, (void*)&lpDispatch, NULL);
  return lpDispatch;
  }

LPDISPATCH OWorkbook::Worksheets(char* pName)
  {
  COleVariant Name(pName);
  LPDISPATCH lpDispatch;
  static BYTE BASED_CODE parms[] = VTS_VARIANT;
  if (UsingExcel8or9)
    InvokeHelper(0x1ee, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&lpDispatch, parms, &Name);
  else
    InvokeHelper(0x1ee, DISPATCH_METHOD, VT_DISPATCH, (void*)&lpDispatch, parms, &Name);
  return lpDispatch;
  }

LPDISPATCH OWorkbook::Worksheets(short iIndex)
  {
  COleVariant Index(iIndex);
  Index.ChangeType(VT_I2);
  LPDISPATCH lpDispatch;
  static BYTE BASED_CODE parms[] = VTS_VARIANT;
  if (UsingExcel8or9)
    InvokeHelper(0x1ee, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&lpDispatch, parms, &Index);
  else
    InvokeHelper(0x1ee, DISPATCH_METHOD, VT_DISPATCH, (void*)&lpDispatch, parms, &Index);
  return lpDispatch;
  }

LPDISPATCH OWorkbook::Worksheets()
  {
  LPDISPATCH lpDispatch;
  if (UsingExcel8or9)
    InvokeHelper(0x1ee, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&lpDispatch, NULL);
  else
    InvokeHelper(0x1ee, DISPATCH_METHOD, VT_DISPATCH, (void*)&lpDispatch, NULL);
  return lpDispatch;
  }

//===========================================================================

short OWorkbooks::GetCount()
  {
  COleVariant result;
  GetProperty(0x76, VT_VARIANT, (void*)&result);
  return result.iVal;
  }

/*From Excel5:
VARIANT OpenText(const VARIANT& Filename, const VARIANT& Origin, const VARIANT& StartRow, 
                   const VARIANT& DataType, const VARIANT& TextQualifier, const VARIANT& ConsecutiveDelimiter,
                   const VARIANT& Tab, const VARIANT& Semicolon, const VARIANT& Comma,
                   const VARIANT& Space, const VARIANT& Other, const VARIANT& OtherChar,
                   const VARIANT& FieldInfo);
From Excel8:
LPDISPATCH Open(LPCTSTR Filename, const VARIANT& UpdateLinks, const VARIANT& ReadOnly,
                 const VARIANT& Format, const VARIANT& Password, const VARIANT& WriteResPassword,
                 const VARIANT& IgnoreReadOnlyRecommended, const VARIANT& Origin, const VARIANT& Delimiter,
                 const VARIANT& Editable, const VARIANT& Notify, const VARIANT& Converter,
                 const VARIANT& AddToMru);*/
LPDISPATCH OWorkbooks::Open(char* pFilename, BOOL bIgnoreReadOnlyRecommended, int iUpdateLinks/*=0*/)
  {
  // NOTE: in order to specify the correct syntax of the "Open" method, we 
  //     have to pass NULL arguments for the unused parameters. To pass a 
  //     NULL argument we use a VARIANT indicating VT_ERROR and initialize
  //     it's "scode" member to "DISP_E_PARAMNOTFOUND"
  /*VARIANT argNULL;
  VariantInit(&argNULL); 
  V_VT(&argNULL) = VT_ERROR; 
  V_ERROR(&argNULL) = DISP_E_PARAMNOTFOUND;
  static BYTE parms[] =
    VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT;
  COleVariant sFilename(pFilename);
  VARIANT result;
  InvokeHelper(0x2aa, DISPATCH_METHOD, VT_VARIANT, (void*)&result, parms,	&sFilename,
    &argNULL, &argNULL, &argNULL, &argNULL, &argNULL, &argNULL, &argNULL, &argNULL, &argNULL, &argNULL, &argNULL);*/

  //UpdateLinks : 0	Doesn't update any references
  //              1	Updates external references but not remote references
  //              2	Updates remote references but not external references
  //              3	Updates both remote and external references

  //NB: Excel5 and Excel8 have different parameters for the open method!!!
  COleVariant sFilename(pFilename);
  OVariant IgnoreReadOnlyRecommended(bIgnoreReadOnlyRecommended);
  OVariant UpdateLinks(iUpdateLinks);

  LPDISPATCH result;
  static BYTE parms5[] = VTS_VARIANT;
  static BYTE parms8[] = VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT;
  if (UsingExcel8or9)
    {
    //static BYTE parms[] = VTS_BSTR VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT;
    //InvokeHelper(0x2aa, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms, Filename, &UpdateLinks, &ReadOnly, &Format, &Password, &WriteResPassword, &IgnoreReadOnlyRecommended, &Origin, &Delimiter, &Editable, &Notify, &Converter, &AddToMru);
    VARIANT argNULL;
    VariantInit(&argNULL); 
    V_VT(&argNULL) = VT_ERROR; 
    V_ERROR(&argNULL) = DISP_E_PARAMNOTFOUND;
    InvokeHelper(0x2aa, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms8, &sFilename, &UpdateLinks, &argNULL, &argNULL, &argNULL, &argNULL, &IgnoreReadOnlyRecommended);
    }
  else
    {
    InvokeHelper(0x2aa, DISPATCH_METHOD, VT_VARIANT, (void*)&result, parms5, &sFilename);
    }
  return result;
  }

//===========================================================================

BOOL OWorksheet::GetProtectContents()
  {
  COleVariant result;
  GetProperty(0x124, VT_VARIANT, (void*)&result);
  return result.boolVal;
  }

CString OWorksheet::GetName()
  {
  VARIANT result;
  GetProperty(0x6e, VT_VARIANT, (void*)&result);
  CString s = result.bstrVal;
  return s;
  }

void OWorksheet::SetName(char* pName)
  {
  COleVariant propVal(pName);
  SetProperty(0x6e, VT_VARIANT, &propVal);
  }

VARIANT OWorksheet::Activate()
  {
  VARIANT result;
  InvokeHelper(0x130, DISPATCH_METHOD, VT_VARIANT, (void*)&result, NULL);
  return result;
  }

LPDISPATCH OWorksheet::Cells(short iRowIndex, short iColumnIndex)
  {
  LPDISPATCH lpDispatch;
  static BYTE parms[] = VTS_VARIANT VTS_VARIANT;
  COleVariant RowIndex(iRowIndex);
  COleVariant ColumnIndex(iColumnIndex);
  if (UsingExcel8or9)
    InvokeHelper(0xee, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&lpDispatch, parms, &RowIndex, &ColumnIndex);
  else
    InvokeHelper(0xee, DISPATCH_METHOD, VT_DISPATCH, (void*)&lpDispatch, parms, &RowIndex, &ColumnIndex);
  return lpDispatch;
  }

LPDISPATCH OWorksheet::Columns()
  {
  LPDISPATCH lpDispatch;
  if (UsingExcel8or9)
    InvokeHelper(0xf1, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&lpDispatch, NULL);
  else
    InvokeHelper(0xf1, DISPATCH_METHOD, VT_DISPATCH, (void*)&lpDispatch, NULL);
  return lpDispatch;
  }

LPDISPATCH OWorksheet::Columns(short iColumnIndex)
  {
  LPDISPATCH lpDispatch;
  static BYTE parms[] = VTS_VARIANT;
  COleVariant ColumnIndex(iColumnIndex);
  if (UsingExcel8or9)
    InvokeHelper(0xf1, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&lpDispatch, parms, &ColumnIndex);
  else
    InvokeHelper(0xf1, DISPATCH_METHOD, VT_DISPATCH, (void*)&lpDispatch, parms, &ColumnIndex);
  return lpDispatch;
  }

LPDISPATCH OWorksheet::Range(short iRowIndex1, short iColumnIndex1, short iRowIndex2, short iColumnIndex2)
  {
  LPDISPATCH lpDis1 = Cells(iRowIndex1, iColumnIndex1);
  OVariant Cell1(lpDis1);
  LPDISPATCH lpDis2 = Cells(iRowIndex2, iColumnIndex2);
  OVariant Cell2(lpDis2);
  LPDISPATCH lpDispatch;
  static BYTE parms[] = VTS_VARIANT VTS_VARIANT;//VTS_DISPATCH VTS_DISPATCH;
  if (UsingExcel8or9)
    InvokeHelper(0xc5, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&lpDispatch, parms, &Cell1, &Cell2);
  else
    InvokeHelper(0xc5, DISPATCH_METHOD, VT_DISPATCH, (void*)&lpDispatch, parms, &Cell1, &Cell2);
  return lpDispatch;
  }

//CNM
LPDISPATCH OWorksheet::Range(LPCTSTR Name)
  {
  //LPDISPATCH lpDis1 = Cells(iRowIndex1, iColumnIndex1);
  OVariant Cell1(Name);
  //LPDISPATCH lpDis2 = Cells(iRowIndex2, iColumnIndex2);
  //OVariant Cell2(lpDis2);
  LPDISPATCH lpDispatch;
  static BYTE parms[] = VTS_VARIANT ;//VTS_DISPATCH VTS_DISPATCH;
  if (UsingExcel8or9)
    InvokeHelper(0xc5, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&lpDispatch, parms, &Cell1);
  else
    InvokeHelper(0xc5, DISPATCH_METHOD, VT_DISPATCH, (void*)&lpDispatch, parms, &Cell1);
  return lpDispatch;
  }
//===========================================================================

short OWorksheets::GetCount()
  {
  COleVariant result;
  GetProperty(0x76, VT_VARIANT, (void*)&result);
  return result.iVal;
  }

VARIANT OWorksheets::Add()
  {
  VARIANT result;
  InvokeHelper(0xb5, DISPATCH_METHOD, VT_VARIANT, (void*)&result, NULL);
  return result;
  }

//===========================================================================

void OFont::SetBold(BOOL bBold)
  {
  OVariant propVal(bBold);
  SetProperty(0x60, VT_VARIANT, &propVal);
  }

void OFont::SetColor(COLORREF Color)
  {
  COleVariant propVal((long)Color);
  SetProperty(0x63, VT_VARIANT, &propVal);
  }

//===========================================================================

CString OChart::GetName()
  {
  VARIANT result;
  GetProperty(0x6e, VT_VARIANT, (void*)&result);
  CString s = result.bstrVal;
  return s;
  }

void OChart::SetName(char* pName)
  {
  COleVariant propVal(pName);
  SetProperty(0x6e, VT_VARIANT, &propVal);
  }

void OChart::SetType(short Type)
  {
  COleVariant propVal(Type);
  SetProperty(0x6c, VT_VARIANT, &propVal);
  }

LPDISPATCH OChart::SeriesCollection(short iIndex)
  {
  COleVariant Index(iIndex);
  Index.ChangeType(VT_I2);
  LPDISPATCH lpDispatch;
  static BYTE BASED_CODE parms[] = VTS_VARIANT;
  InvokeHelper(0x44, DISPATCH_METHOD, VT_DISPATCH, (void*)&lpDispatch, parms, &Index);
  return lpDispatch;
  }

LPDISPATCH OChart::SeriesCollection()
  {
  LPDISPATCH lpDispatch;
  InvokeHelper(0x44, DISPATCH_METHOD, VT_DISPATCH, (void*)&lpDispatch, NULL);
  return lpDispatch;
  }

//===========================================================================

short OCharts::GetCount()
  {
  COleVariant result;
  GetProperty(0x76, VT_VARIANT, (void*)&result);
  return result.iVal;
  }

VARIANT OCharts::Add()
  {
  VARIANT result;
  InvokeHelper(0xb5, DISPATCH_METHOD, VT_VARIANT, (void*)&result, NULL);
  return result;
  }

//===========================================================================

void OSeries::SetValues(LPDISPATCH pDispatch)
  {
  OVariant propVal(pDispatch);
  SetProperty(0xa4, VT_VARIANT, &propVal);
  }

void OSeries::SetXValues(LPDISPATCH pDispatch)
  {
  OVariant propVal(pDispatch);
  SetProperty(0x457, VT_VARIANT, &propVal);
  }

VARIANT OSeries::Delete()
  {
  VARIANT result;
  InvokeHelper(0x75, DISPATCH_METHOD, VT_VARIANT, (void*)&result, NULL);
  return result;
  }

LPDISPATCH OSeries::Points()
  {
  LPDISPATCH lpDispatch;
  InvokeHelper(0x46, DISPATCH_METHOD, VT_DISPATCH, (void*)&lpDispatch, NULL);
  return lpDispatch;
  }

//===========================================================================

short OSeriesCollection::GetCount()
  {
  COleVariant result;
  GetProperty(0x76, VT_VARIANT, (void*)&result);
  return result.iVal;
  }

VARIANT OSeriesCollection::Add(LPDISPATCH pDispatch)
  {
  VARIANT result;
  static BYTE parms[] = VTS_VARIANT;// VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT;
  OVariant Source(pDispatch);
  InvokeHelper(0xb5, DISPATCH_METHOD, VT_VARIANT, (void*)&result, parms, &Source);
  return result;
  }

//===========================================================================

long OWindow::GetView()
  {
  long result;
  InvokeHelper(0x4aa, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
  return result;
  }

void OWindow::SetView(long nNewValue)
  {
  static BYTE parms[] = VTS_I4;
  InvokeHelper(0x4aa, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, nNewValue);
  }

//===========================================================================

short OWindows::GetCount()
  {
  COleVariant result;
  GetProperty(0x76, VT_VARIANT, (void*)&result);
  return result.iVal;
  }

//===========================================================================
