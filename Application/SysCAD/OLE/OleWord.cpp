//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
//OLE 2.0 automation for Excel

#define  __OLEWORD_CPP


#include "stdafx.h"
#include "sc_defs.h"
#include "..\resource.h"
#include "ole_base.h"
#include "oleword.h"
#include "scd_wm.h"
#include "..\..\Com\ScdCOM\ScdCOMIF.h"
#include "sfe_base.h"
#include "project.h"
#include "selctwnd.h"
#include "sp_db.h"

#ifdef NOTDEFINEDXXXXX

const char* OleReportListKey          = "SysCAD_TagList(";
const char* OleReportListOffsetKey    = "SysCAD_TagListOffset(";
const char* OleReportKey              = "SysCAD_Tags(";
const char* OleReportCommonKey        = "SysCAD_Tag";
const char* OleReportOtherKey         = "SysCAD_Reports";
const char* OleReportTrendKey         = "TrendReport";
const char* OleReportTrend2Key        = "SysCAD_TrendReport";
const char* OleSetTagListKey          = "SysCAD_SetTagList(";
const char* OleSetTagListOffsetKey    = "SysCAD_SetTagListOffset(";
const char* OleSetTagsKey             = "SysCAD_SetTags(";
const char* OleReportFeedbackKey      = "Tag Errs";
const char* OleReportTrendFeedbackKey = "Trend Errs";
const char* OleSetTagsFeedbackKey     = "SetTag Errs";
const char* OleGenInfoWorksheetKey    = "Tag Info";

const int MaxCellFeedbackCnt = 30;

//===========================================================================


//===========================================================================

COleExcelBaseAuto::COleExcelBaseAuto(CExecObj* ExecObj, CXM_Route* pRoute, char* FileName)
: COleBaseAuto(ExecObj, pRoute, FileName, true)
  {
  bUseCellName = 1;
  bMakeActive = 0;
  iFBLine = 1;
  pFBSheet = NULL;
  pExcel = NULL;
  bUsingActive = 0;
  bForceNormalView = 0;//1;
  }

//---------------------------------------------------------------------------

void COleExcelBaseAuto::SetOptions(flag Lock, flag UseCellName, flag MakeActive, flag MakeSysCADActive, flag UpdateLinks, flag SaveOnComplete)
  {
  bLock = Lock;
  bUseCellName = UseCellName;
  bMakeActive = MakeActive;
  bActivateSysCAD = MakeSysCADActive;
  bUpdateLinks = UpdateLinks;
  bSaveOnComplete = SaveOnComplete;
  }

//---------------------------------------------------------------------------

void COleExcelBaseAuto::FeedbackActivate()
  {
  LPDISPATCH lpDispatch = pFBSheet->Cells(iFBLine, 1);
  ORange Range;
  Range.AttachDispatch(lpDispatch, TRUE);
  Range.Activate();
  Range.ReleaseDispatch();
  pFBSheet->Activate();
  }

//---------------------------------------------------------------------------

void COleExcelBaseAuto::DoFeedback(char* Txt, COLORREF Color, flag UseColor, flag Bold)
  {
  if (iFBLine>32000)
    return;
  if (iFBLine==32000)
    {
    iFBLine = 31999;
    DoFeedback("Too many errors to report!", RGB(255, 0, 0), True);
    iFBLine=32009;
    return;
    }

  LPDISPATCH lpDispatch = pFBSheet->Cells(iFBLine++, 1);
  ORange Range;
  Range.AttachDispatch(lpDispatch, TRUE);
  Range.SetValue(Txt);
  if (UseColor || Bold)
    {
    lpDispatch = Range.GetFont();
    OFont Font;
    Font.AttachDispatch(lpDispatch, TRUE);
    if (UseColor)
      Font.SetColor(Color);
    if (Bold)
      Font.SetBold(True);
    Font.ReleaseDispatch();
    }
  //if (bVisible)
  //  Range.Activate();
  Range.ReleaseDispatch();
  }

//---------------------------------------------------------------------------

void COleExcelBaseAuto::Feedback(char * fmt, ...)
  {
  char Buff[2048];
  va_list argptr;
  va_start(argptr, fmt);
  vsprintf(Buff, fmt, argptr);
  va_end(argptr);
  DoFeedback(Buff);
  }

//---------------------------------------------------------------------------

void COleExcelBaseAuto::RedFeedback(char * fmt, ...)
  {
  char Buff[2048];
  va_list argptr;
  va_start(argptr, fmt);
  vsprintf(Buff, fmt, argptr);
  va_end(argptr);
  DoFeedback(Buff, RGB(255, 0, 0), True);
  }

//---------------------------------------------------------------------------

void COleExcelBaseAuto::BlueFeedback(char * fmt, ...)
  {
  char Buff[2048];
  va_list argptr;
  va_start(argptr, fmt);
  vsprintf(Buff, fmt, argptr);
  va_end(argptr);
  DoFeedback(Buff, RGB(0, 0, 255), True);
  }

//---------------------------------------------------------------------------

void COleExcelBaseAuto::BoldFeedback(char * fmt, ...)
  {
  char Buff[2048];
  va_list argptr;
  va_start(argptr, fmt);
  vsprintf(Buff, fmt, argptr);
  va_end(argptr);
  DoFeedback(Buff, 0, False, True);
  }

//---------------------------------------------------------------------------

void COleExcelBaseAuto::InitFeedback(OWorksheets* pWkSheets, OWorkbook* pWkBook, flag Exists, char* FeedbackPageName)
  {
  ASSERT(pFBSheet==NULL);
  pFBSheet = new OWorksheet;
  LPDISPATCH lpDispatch;
  if (!Exists)
    {
    pWkSheets->Add();
    lpDispatch = pWkBook->GetActiveSheet();
    pFBSheet->AttachDispatch(lpDispatch, TRUE);
    pFBSheet->SetName(FeedbackPageName);
    }
  else
    {
    lpDispatch = pWkBook->Worksheets(FeedbackPageName);
    pFBSheet->AttachDispatch(lpDispatch, TRUE);
    pFBSheet->Activate();
    }
  lpDispatch = pFBSheet->Columns();
  ORange Range;
  Range.AttachDispatch(lpDispatch, TRUE);
  Range.Clear();
  lpDispatch = pFBSheet->Columns(1);
  Range.AttachDispatch(lpDispatch, TRUE);
  Range.SetColumnWidth(100);
//  if (!bVisible)
//    {
//    lpDispatch = pFBSheet->Cells(2, 1);
//    Range.AttachDispatch(lpDispatch, TRUE);
//    Range.Activate();
//    }
  Range.ReleaseDispatch();
  Strng DtTm;
  BoldFeedback("%s  Project : %s   (%s)", CurDateTime(DtTm), PrjName(), PrjFiles());
  BoldFeedback("%s feedback report generated by SysCAD %s, %s", ShortDesc(), VersionNo(), FullCopyrightNotice());
  }

//---------------------------------------------------------------------------

int COleExcelBaseAuto::StandardStartup()
  {
  if (!FileExists(sFileName()))
    {
    flag FileErr = 1;
    Strng sPath;
    sPath.FnDrivePath(sFileName());
    if (sPath.Len()==0)
      {
      sFileName.FnSearchExpand();
      FileErr = !FileExists(sFileName());
      }
    if (FileErr)
      {
      ReportError(1, "Excel file '%s' does not exist", sFileName());
      return 1;
      }
    }
  sWorkbookName = sFileName();
  if (sWorkbookName.ReverseFind('\\')>=0)
    sWorkbookName = sWorkbookName.Mid(sWorkbookName.ReverseFind('\\')+1, 1024);
  COleInfoMsg* pMsg = new COleInfoMsg("Starting Excel...", (char*)(const char*)sWorkbookName);
  pOleInfoWnd->SendMessage(WMU_OLEINFOUPDATE, 0, (LPARAM)pMsg);
  ASSERT(pExcel==NULL);

  pExcel = new OExcelApplication;
  //try excel 10, then excel 9, then excel 8, then excel 5, then ?, then if all fail report error
  COleException e;
  CLSID clsid;
  UsingExcel8or9 = TRUE;
  if (CLSIDFromProgID(OLESTR("Excel.Application.10"), &clsid) != NOERROR) //Excel XP
    {
    if (CLSIDFromProgID(OLESTR("Excel.Application.9"), &clsid) != NOERROR) //Excel 2000
      {
      if (CLSIDFromProgID(OLESTR("Excel.Application.8"), &clsid) != NOERROR) //Excel 98
        {
        if (CLSIDFromProgID(OLESTR("Excel.Application.5"), &clsid) != NOERROR) //Excel 95
          {
          if (CLSIDFromProgID(OLESTR("Excel.Application"), &clsid) != NOERROR) //finally, perhaps try connect to "most recent" Excel application and assume it is similar to 8 and 9
            {
            ReportError(6, "Unable to find Excel 5, Excel 8, Excel 9 OR Excel 10 Application objects");
            return 6;
            }
          }
        else
          UsingExcel8or9 = FALSE;
        }
      }
    }

  bUsingActive = 0;
  // try to get the active Excel Application before creating a new one
  LPUNKNOWN lpUnk;
  LPDISPATCH lpDispatch;
  HRESULT hr;
  //kga (17/7/97): If there is more than one instance of Excel open, any of them could have the file open.
  //               GetActiveObject always returns a "handle" to the same one, I cannot find a method to
  //               interigate all instances of Excel to see which one has the file open and then attach to that one.
  //               In the mean time, attach to the active object and try our luck!
  if (GetActiveObject(clsid, NULL, &lpUnk) == NOERROR)
    {
    hr = lpUnk->QueryInterface(IID_IDispatch, (LPVOID*)&lpDispatch);
    lpUnk->Release();
    if (hr == NOERROR)
      {
      bUsingActive = 1;
      pExcel->AttachDispatch(lpDispatch, TRUE);
      }
    }

  if (!bUsingActive)
    {
    if (!pExcel->CreateDispatch(clsid, &e))
      {
      ReportError(2, "Unable to create Excel Application object");
      return 2;
      }
    }
  pExcel->m_bAutoRelease = TRUE;
  if (!bUsingActive || bMakeActive)
    pExcel->SetVisible(TRUE);
  return 0;
  }

//---------------------------------------------------------------------------

int COleExcelBaseAuto::Open(OWorkbooks* pWkBooks, OWorkbook* pWkBook, flag OpenAlways)
  {
  flag WorkbookFound = 0;
  LPDISPATCH lpDispatch;
  if (bUsingActive)
    {//Excel allready open, go and find workbook...
    lpDispatch = pExcel->Workbooks();
    if (lpDispatch)
      {
      pWkBooks->AttachDispatch(lpDispatch, TRUE);
      short Count = pWkBooks->GetCount();
      for (short i=1; (i<=Count && WorkbookFound==0); i++)
        {
        pWkBook->AttachDispatch(pExcel->Workbooks(i), TRUE);
        CString WorkbookName = pWkBook->GetName();
        if (_stricmp((const char*)WorkbookName, (const char*)sWorkbookName)==0) //check if workbook names are the same
          {
          CString FullName = pWkBook->GetFullName();
          if (_stricmp(sFileName(), (const char*)FullName)==0) //check if workbook full paths are the same
            WorkbookFound = 1;
          else
            {
            ReportWarning(10, "Saved and closed '%s', since its location is different to '%s'.", (const char*)FullName, sFileName());
            lpDispatch = pExcel->Workbooks((char*)(const char*)sWorkbookName);
            pWkBook->AttachDispatch(lpDispatch, TRUE);
            pWkBook->Activate();
            //pWkBook->SetSaved(TRUE);
            pWkBook->Close(FALSE);
            pWkBook->ReleaseDispatch();
            }
          }
        }
      if (WorkbookFound)
        {
        lpDispatch = pExcel->Workbooks((char*)(const char*)sWorkbookName);
        pWkBook->AttachDispatch(lpDispatch, TRUE);
        if (!OpenAlways && !pWkBook->GetSaved())
          {
          SendInfo("Marked as changed - Cancel");
          ReportError(7, "%s is marked as changed, save the workbook and try again", (const char*)sWorkbookName);
          pExcel->ReleaseDispatch();
          pWkBook->ReleaseDispatch();
          return 7;
          }
        pWkBook->Activate();
        }
      }
    }

  if (!WorkbookFound)
    {
    pWkBooks->AttachDispatch(pExcel->Workbooks(), TRUE); //assign the interface obtained to the new 'WorkBooks' object 
    pWkBooks->Open(sFileName(), TRUE, bUpdateLinks ? 3 : 0); //assumes file exists!
    }
  
  lpDispatch = pExcel->Workbooks((char*)(const char*)sWorkbookName);
  pWkBook->AttachDispatch(lpDispatch, TRUE);
  if (!OpenAlways && pWkBook->GetReadOnly())
    {
    SendInfo("Read Only - Cancel");
    ReportError(3, "%s is Read Only, close all other copies and try again", (const char*)sWorkbookName);
    pWkBook->SetSaved(TRUE);
    if (!bUsingActive)
      pExcel->Quit();
    else
      pWkBook->Close(FALSE);
    pExcel->ReleaseDispatch();
    pWkBook->ReleaseDispatch();
    return 3;
    }

  if (!bUsingActive)
    {//set bUsingActive so that Excel doesn't Quit; ensure Excel is visible.
    bUsingActive = 1;
//    if (!bVisible)
//      {
//      //pExcel->SetVisible(TRUE);
//      //TODO activate SysCAD, bring window to top
//      }
    }
  
  //always ensure excel is visible...
  if (!pExcel->GetVisible() || bMakeActive)
    pExcel->SetVisible(TRUE);
  //TODO activate SysCAD, bring window to top
  
  return 0;
  }

//---------------------------------------------------------------------------

int COleExcelBaseAuto::OpenAndInit(OWorkbooks* pWkBooks, OWorksheets* pWkSheets, OWorkbook* pWkBook, OWorksheet* pWkSheet, char* FeedbackPageName)
  {
  SendInfo("Initialise and open %s", sFileName());

  int RetCode = Open(pWkBooks, pWkBook, false);
  if (RetCode!=0)
    return RetCode;

  short i;
  /*if (bForceNormalView)
    {
    long view = -1;
    LPDISPATCH lpDisTmp;
    lpDisTmp = pExcel->GetActiveWindow();
    if (lpDisTmp)
      {
      ww.AttachDispatch(lpDisTmp, TRUE);
      view = ww.GetView();
      if (view==xlPageBreakPreview)
        ww.SetView(xlNormalView);
      ww.ReleaseDispatch();
      }
    };*/
  /*if (bForceNormalView)
    {
    this does not work because each worksheet needs to be activated to become the active window so that the view can be changed!
    long view = -1;
    LPDISPATCH lpDisTmp;
    OWindows ws;
    lpDisTmp = pWkBook->Windows();
    if (lpDisTmp)
      {
      ws.AttachDispatch(lpDisTmp, TRUE);
      short Cnt = ws.GetCount();
      OWindow ww;
      for (i=1; i<=Cnt; i++)
        {
        ww.AttachDispatch(pWkBook->Windows(i), TRUE);
        view = ww.GetView();
        if (view==xlPageBreakPreview)
          ww.SetView(xlNormalView);
        }
      ws.ReleaseDispatch();
      }
    }*/

  pWkSheets->AttachDispatch(pExcel->Worksheets(), TRUE);
  short Count = pWkSheets->GetCount();
  WkSheetNames.SetSize(Count);
  flag FeedbackFound = 0;
  for (i=1; i<=Count; i++)
    {
    pWkSheet->AttachDispatch(pWkBook->Worksheets(i), TRUE);
    CString SheetName = pWkSheet->GetName();
    WkSheetNames[i-1] = (const char*)SheetName;
    if (_stricmp((const char*)SheetName, (const char*)FeedbackPageName)==0)
      FeedbackFound = 1;
    }

  InitFeedback(pWkSheets, pWkBook, FeedbackFound, FeedbackPageName);
  //need to fetch worksheet names again!
  Count = pWkSheets->GetCount();
  WkSheetNames.SetSize(Count);
  for (i=1; i<=Count; i++)
    {
    pWkSheet->AttachDispatch(pWkBook->Worksheets(i), TRUE);
    CString SheetName = pWkSheet->GetName();
    if (pWkSheet->GetProtectContents())
      {
      RedFeedback("WARNING: Worksheet '%s' is protected", (const char*)SheetName);
      //SheetName += "PROTECTED!";
      }
    WkSheetNames[i-1] = (const char*)SheetName;
    }
  UserWkSheets.SetSize(WkSheetNames.GetSize());
  for (int i=0; i<WkSheetNames.GetSize(); i++)
    {
    UserWkSheets[i] = (_stricmp(WkSheetNames[i](), (char*)OleReportFeedbackKey)!=0) &&
                      (_stricmp(WkSheetNames[i](), (char*)OleReportTrendFeedbackKey)!=0) &&
                      (_stricmp(WkSheetNames[i](), (char*)OleSetTagsFeedbackKey)!=0) &&
                      (_strnicmp(WkSheetNames[i](), (char*)OleGenInfoWorksheetKey, strlen((char*)OleGenInfoWorksheetKey))!=0);
    }
  return 0;
  }

//---------------------------------------------------------------------------

CString COleExcelBaseAuto::GetString(short Row, short Column, OWorksheet* pWkSheet, ORange* pRange)
  {
  LPDISPATCH lpDispatch = pWkSheet->Cells(Row, Column);
  pRange->AttachDispatch(lpDispatch, TRUE);
  return pRange->GetStringValue();
  }

//---------------------------------------------------------------------------

char* COleExcelBaseAuto::CellName(int Row, int Col, Strng& sCellName)
  {
  if (bUseCellName)
    {
    if (Row>0 && Col>0)
      {
      sCellName.MakeAlpha(Col);
      sCellName.Append("%d", Row);
      }
    else
      sCellName = "??";
    }
  else
    sCellName.Set("%d,%d", Row, Col);
  return sCellName();
  }

//---------------------------------------------------------------------------

flag COleExcelBaseAuto::CellRowAndCol(char* pCellName, int& Row, int& Col)
  {
  Row = 0;
  Col = 0;
  if (pCellName && pCellName[0]!=0)
    {
    const int len = strlen(pCellName);
    Strng sCol;
    Strng sRow;
    int i=0;
    while (i<len && !isdigit(pCellName[i]))
      sCol += pCellName[i++];
    while (i<len && isdigit(pCellName[i]))
      sRow += pCellName[i++];
    if (sCol.Len()==0 || sRow.Len()==0)
      return false;
    Col = SafeAlphatoI(sCol(), 0, 26);
    Row = atoi(sRow());
    return (Col>0 && Row>0 && Col<256 && Row<65536);
    }
  return false;
  }

//---------------------------------------------------------------------------

void COleExcelBaseAuto::SetCellText(char* Txt, short Row, short Column, OWorksheet* pWkSheet, COLORREF Color/*=0*/, flag UseColor/*=False*/, flag Bold/*=False*/)
  {
  LPDISPATCH lpDispatch = pWkSheet->Cells(Row, Column);
  ORange Range;
  Range.AttachDispatch(lpDispatch, TRUE);
  Range.SetValue(Txt);
  if (UseColor || Bold)
    {
    lpDispatch = Range.GetFont();
    OFont Font;
    Font.AttachDispatch(lpDispatch, TRUE);
    if (UseColor)
      Font.SetColor(Color);
    if (Bold)
      Font.SetBold(True);
    Font.ReleaseDispatch();
    }
  Range.ReleaseDispatch();
  }

//---------------------------------------------------------------------------

void COleExcelBaseAuto::SetCellValue(double Val, short Row, short Column, OWorksheet* pWkSheet, COLORREF Color/*=0*/, flag UseColor/*=False*/, flag Bold/*=False*/)
  {
  LPDISPATCH lpDispatch = pWkSheet->Cells(Row, Column);
  ORange Range;
  Range.AttachDispatch(lpDispatch, TRUE);
  Range.SetValue(Val);
  if (UseColor || Bold)
    {
    lpDispatch = Range.GetFont();
    OFont Font;
    Font.AttachDispatch(lpDispatch, TRUE);
    if (UseColor)
      Font.SetColor(Color);
    if (Bold)
      Font.SetBold(True);
    Font.ReleaseDispatch();
    }
  Range.ReleaseDispatch();
  }

//---------------------------------------------------------------------------
//===========================================================================

class CAutomationHelper
  {
  protected:
    WORD wCmdID;
  public:
    CAutomationHelper(WORD CmdID) { wCmdID = CmdID; };
    ~CAutomationHelper() { ScdMainWnd()->PostMessage(WMU_CMDDONE, wCmdID, 0); }; //let script cmd mngr know cmd is complete
  };

//===========================================================================

CExcelReport::CExcelReport(COleReportMngr* Mngr, OWorkbook* WkBook)
  {
  pMngr = Mngr;
  bResVert = 1;
  bPriVert = 1;
  bSecVert = 0;
  bIsTagList = 0;
  bIsTagOffsetList = 0;
  iTagFoundCnt = 0;
  pWkBook = WkBook;
  }

//---------------------------------------------------------------------------

flag CExcelReport::CheckParms()
  {
  flag OK = True;
  COleReportMngr& M = *pMngr;
  if (M.WkSheetNames.Find(ResLoc.sSheet())<0)
    {
    OK = False;
    M.Feedback("Result worksheet '%s' not found", ResLoc.sSheet());
    }
  if (M.WkSheetNames.Find(PriLoc.sSheet())<0)
    {
    OK = False;
    M.Feedback("Primary tags worksheet '%s' not found", PriLoc.sSheet());
    }
  if (M.WkSheetNames.Find(SecLoc.sSheet())<0)
    {
    OK = False;
    M.Feedback("Secondary tags worksheet '%s' not found", SecLoc.sSheet());
    }
  if (!ResLoc.Check() || !PriLoc.Check() || !SecLoc.Check())
    {
    OK = False;
    M.Feedback("All row and column indexes should be greater than zero");
    }
  if (iPriLen<1 || iPriMaxLen<iPriLen)
    {
    OK = False;
    M.Feedback("Length for primary tags should be greater than 0 and MaxLength should be greater than the length");
    }
  if (iSecLen<1 || iSecMaxLen<iSecLen)
    {
    OK = False;
    M.Feedback("Length for secondary tags should be greater than 0 and MaxLength should be greater than the length");
    }
  return OK;
  }

//---------------------------------------------------------------------------

int CExcelReport::ParseFn(char* Func)
  {
  char Buff[2048];
  sprintf(Buff, "%s", Func);
  int Quote;
  CSVColArray f;
  int nParms = ParseCSVFunction(Buff, f, Quote) - 1;
  if (Quote || nParms<3 || strlen(f[2])==0 || strlen(f[3])==0)
    return -1;
  if (!bIsTagList && (nParms<4 || strlen(f[4])==0))
    return -1;
  sName = f[1];
  sName.Trim("\n\r\t \"\'");
  bPriVert = (!(f[2][0]=='H' || f[2][0]=='h'));
  iSecMult = (!bIsTagList && nParms>=5 && strlen(f[5])>0) ? atoi(f[5]) : 1;
  bResVert = bPriVert;
  bSecVert = (bPriVert ? 0 : 1);
  if (bIsTagList)
    {
    iPriLen = atoi(f[3]);
    iSecLen = 1;
    }
  else
    {
    if (bPriVert)
      {
      iSecLen = atoi(f[3]);
      iPriLen = atoi(f[4]);
      }
    else
      {
      iPriLen = atoi(f[3]);
      iSecLen = atoi(f[4]);
      }
    }
  iPriMaxLen = iPriLen;
  iSecMaxLen = iSecLen;
  sNan = (bIsTagList ? (nParms>3 ? f[4] : "*") : (nParms>4 ? f[5] : "*"));
  return 0;
  }

//---------------------------------------------------------------------------

int CExcelReport::ParseOffsetFn(char* Func, OWorksheet* pWkSheet, int& Row, int& Col)
  {
  ASSERT(bIsTagOffsetList);
  char Buff[1024];
  sprintf(Buff, "%s", Func);
  int Quote;
  CSVColArray f;
  int nParms = ParseCSVFunction(Buff, f, Quote) - 1;
  if (Quote || nParms!=2 || strlen(f[1])==0 || strlen(f[2])==0)
    return -1;

  Strng RepName(f[2]);
  RepName.Trim("\n\r\t \"\'");
  if (RepName.Len()==0)
    return -1;
  sName = RepName;
  Strng Loc(f[1]);
  Loc.Trim("\n\r\t \"\'");
  Loc.Upper();
  COleReportMngr& M = *pMngr;
  if (!M.CellRowAndCol(Loc(), Row, Col))
    return -2;
  ORange R;
  CString OFunc = M.GetString(Row, Col, pWkSheet, &R);
  Strng Fn = (const char*)OFunc;
  Fn.Trim("\n\r\t ");
  if (Fn.Len()<(int)strlen(OleReportListKey) || _strnicmp(OleReportListKey, Fn(), strlen(OleReportListKey))!=0)
    return -3;
  if (ParseFn(Fn())!=0)
    {
    sName = RepName; //must do this again because it is overwritten by ParseFn
    return -4;
    }
  sName = RepName; //must do this again because it is overwritten by ParseFn
  return 0;
  }

//---------------------------------------------------------------------------

long CExcelReport::GetTags(CSVector& Tags, CCellLocation& Loc, short Len, short Wide, short Skip, flag Vert)
  {
  Tags.SetSize(Len*Wide);
  COleReportMngr& M = *pMngr;
  OWorksheet WSheet;
  LPDISPATCH lpDispatch = pWkBook->Worksheets(Loc.sSheet());
  WSheet.AttachDispatch(lpDispatch, TRUE);
  ORange Range;
  int t=0;
  short Col = Loc.iColumn;
  short Row = Loc.iRow;

  for (long j=0; j<Wide; j++)
    {
    if (Vert)
      Row = Loc.iRow;
    else
      Col = Loc.iColumn;
    for (long i=0; i<Len; i++)
      {
      CString s = M.GetString(Row, Col, &WSheet, &Range);
      Tags[t] = (const char*)s;
      Tags[t].Trim("\n\r\t ");
      t++;
      if (Vert)
        Row+=Skip;
      else
        Col+=Skip;
      }

    if (Vert)
      Col+=1;
    else
      Row+=1;
    }
  long NewLen = Len;
  if (Wide==1)
    {
    while (NewLen>0)
      if (Tags[(NewLen-1)].Length()==0)
        NewLen--;
      else
        break;
    Tags.SetSize(NewLen);
    }
  Range.ReleaseDispatch();
  WSheet.ReleaseDispatch();
  return NewLen;
  }

//---------------------------------------------------------------------------

flag CExcelReport::DoReport()
  {
  //CStopWatch sw;
  //sw.Start();
  iTagFoundCnt = 0;
  COleReportMngr& M = *pMngr;
  short PriLen = (short)GetTags(PriTags, PriLoc, iPriLen, 1, iSecMult, bPriVert);
  short SecLen = 1;
  if (!bIsTagList)
    SecLen=(short)GetTags(SecTags, SecLoc, iSecLen, iSecMult, 1, bSecVert);
  flag OK = True;
  if (PriLen==0)
    {
    M.Feedback("No primary tags found");
    OK = False;
    }
  if (!bIsTagList && SecLen==0)
    {
    M.Feedback("No secondary tags found");
    OK = False;
    }
  if (!OK)
    return OK;
  short IncompleteCnt = 0; //number of cells with incomplete tags
  Strng sIncompleteTags;//list of cells with incomplete tags
  OWorksheet WSheet;
  LPDISPATCH lpDispatch = pWkBook->Worksheets(ResLoc.sSheet());
  WSheet.AttachDispatch(lpDispatch, TRUE);
  ORange Range;
  short Row = ResLoc.iRow;
  short Col = ResLoc.iColumn;
  Strng Tag, WrkTag, WrkCnvTxt, FirstTag, LastTag, CellNm;
  for (short i=0; i<PriLen; i++)
    {
    for (short Off=0; Off<iSecMult; Off++)
      {
      if (bResVert)
        Col = ResLoc.iColumn;//+Off;
      else
        Row = ResLoc.iRow;//+Off;
      for (short j=0; j<SecLen; j++)
        {
        M.CellName(Row, Col, CellNm);
        COleVariant V;
        V.ChangeType(VT_EMPTY);
        flag Incomplete = 0;
        flag DoSet = 1;
        if (PriTags[i].Length()>0 && (bIsTagList || SecTags[j+Off*SecLen].Length()>0))
          {
          if (bIsTagList)
            Tag = PriTags[i]();
          else
            Tag.Set("%s.%s", PriTags[i](), SecTags[j+Off*SecLen]());
          TaggedObject::SplitTagCnv(Tag(), WrkTag, WrkCnvTxt);
          if (WrkTag.Length()>0)
            {
            if (M.IsFormula(Row, Col, &WSheet))
              {
              M.Feedback("Formula at cell(%s), tag %s ignored", CellNm(), WrkTag());
              DoSet = 0;
              }
            else
              {
              if (FirstTag.Length()==0)
                FirstTag = WrkTag;
              LastTag = WrkTag;
              flag UseCnv = (WrkCnvTxt.Length()>0);
              CXM_ObjectTag ObjTag(WrkTag(), (UseCnv ? TABOpt_ValCnvsOnce : 0)|TABOpt_StrList);
              CXM_ObjectData ObjData;
              CXM_Route Route;
              if (M.pExecObj->XReadTaggedItem(ObjTag, ObjData, Route))
                {
                iTagFoundCnt++;
                CPkDataItem * pItem = ObjData.FirstItem();
                const byte Typ = pItem->Type();
                if (UseCnv && ((pItem->CnvIndex())==0 || Cnvs[(pItem->CnvIndex())]->Find(WrkCnvTxt())==NULL))
                  {
                  UseCnv = 0;
                  M.Feedback("Warning: Engineering units '%s' used at cell(%s), for tag %s are invalid", WrkCnvTxt(), CellNm(), WrkTag());
                  }
                if (pItem->Value()->IsNAN())
                  V = sNan();
                else if (IsIntData(Typ) && pItem->Contains(PDI_StrList))//IndexedStrList())
                  {
                  Strng_List sStrList;
                  pItem->GetStrList(sStrList);
                  if (sStrList.Length()>0)
                    {
                    long i = pItem->Value()->GetLong();
                    pStrng p = sStrList.AtIndexVal(i);
                    if (p)
                      V = p->Str();
                    else
                      V = GetVariant(pItem->Value());
                    }
                  else
                    V = GetVariant(pItem->Value());
                  }
                else
                  V = (UseCnv ? GetVariant(pItem->Value(), pItem->CnvIndex(), WrkCnvTxt()) : GetVariant(pItem->Value()));
                }
              else
                M.Feedback("%s at cell(%s) not found", WrkTag(), CellNm());
              }
            }
          else
            Incomplete = 1;
          }
        else
          Incomplete = 1;

        if (Incomplete)
          {
          DoSet = 0;
          if (!bIsTagList && iSecMult<2)
            {
            //M.Feedback("Incomplete tag at cell(%s)", CellNm()); //this message drives people crazy...
            if (IncompleteCnt<MaxCellFeedbackCnt)
              {
              if (IncompleteCnt>0)
                sIncompleteTags += ' ';
              sIncompleteTags += CellNm();
              }
            else if (IncompleteCnt==MaxCellFeedbackCnt)
              sIncompleteTags += "...";
            IncompleteCnt++;
            }
          }
        if (DoSet)
          {
          M.Set(Row, Col, V, &WSheet, &Range);
          //dbgpln("%s=%s", CellNm(), "?");
          }
        if (bResVert)
          Col++;
        else
          Row++;
        }
      if (bResVert)
        Row++;
      else
        Col++;
      }
    }
  if (IncompleteCnt>1)
    {
    /*if (sIncompleteTags.Len()>512)
      {
      sIncompleteTags = sIncompleteTags.Left(512);
      sIncompleteTags += "...";
      }*/
    M.Feedback("%d incomplete tags at cells %s", IncompleteCnt, sIncompleteTags());
    }
  else if (IncompleteCnt==1)
    M.Feedback("Incomplete tag at cell %s", sIncompleteTags());
  //M.BlueFeedback("Report generated on worksheet '%s'. %d tags (%s to %s) returned.", ResLoc.sSheet(), iTagFoundCnt, FirstTag(), LastTag());
  M.BlueFeedback("Report generated on worksheet '%s'. %d tags returned.", ResLoc.sSheet(), iTagFoundCnt);
  Range.ReleaseDispatch();
  WSheet.ReleaseDispatch();
  //sw.Stop(); LogNote(OLEServerDesc(), 0, "Report generated:%g", sw.Secs());
  return OK;
  }

//===========================================================================

COleReportMngr::COleReportMngr(CExecObj* ExecObj, CXM_Route* pRoute, char* FileName, char* ReportName, flag SysCAD_Reports)
: COleExcelBaseAuto(ExecObj, pRoute, FileName)
  {
  sReportName = ReportName;
  bSysCAD_Reports = SysCAD_Reports;
  bAll = (sReportName.Length()==0 || sReportName=="*");
  }

//---------------------------------------------------------------------------

void COleReportMngr::Set(short Row, short Column, COleVariant& V, OWorksheet* pWkSheet, ORange* pRange)
  {
  //LPDISPATCH lpDispatch = pWkSheet->Cells(Row, Column);
  //pRange->AttachDispatch(lpDispatch, TRUE);
  pRange->AttachDispatch(pWkSheet->Cells(Row, Column), TRUE);
  pRange->SetValue(V);
  }

//---------------------------------------------------------------------------

short COleReportMngr::GetShort(short Row, short Column, OWorksheet* pWkSheet, ORange* pRange)
  {
  pRange->AttachDispatch(pWkSheet->Cells(Row, Column), TRUE);
  return pRange->GetShortValue();
  }

//---------------------------------------------------------------------------

BOOL COleReportMngr::IsFormula(short Row, short Column, OWorksheet* pWkSheet)
  {
  ORange Range;
  Range.AttachDispatch(pWkSheet->Cells(Row, Column), TRUE);
  OVariant V = Range.GetFormula();
  if (V.vt!=VT_BSTR)
    return FALSE;
  CString s = V.bstrVal;
  return (s.GetLength()>0 && s[0]=='=');
  }

//---------------------------------------------------------------------------

int COleReportMngr::DoAutomation()
  {
  CAutomationHelper AutoHelper(ComCmd_Report_GetValues); //destructor does work
  BOOL DidSetScrUpdating = FALSE;
  long PrevCalc = 0;
  long TtlTagFoundCnt = 0;
  flag Failed = 0;
  Strng ErrMsg, s, StatusBarTxt;
  CString cs;
  try
    {
    CStopWatch sw;
    sw.Start();
    // Execute some code that might throw an exception.
    int RetCode = StandardStartup();
    if (RetCode!=0)
      {
      if (pExcel)
        {
        delete pExcel;
        pExcel = NULL;
        }
      return RetCode;
      }

    pExcel->SetStatusBar("SysCAD: Busy ...");
    OWorkbooks WkBooks;
    OWorksheets WkSheets;
    OWorkbook WkBook;
    OWorksheet WkSheet;
    RetCode = OpenAndInit(&WkBooks, &WkSheets, &WkBook, &WkSheet, (char*)OleReportFeedbackKey);
    if (RetCode!=0)
      {
      delete pExcel;
      pExcel = NULL;
      return RetCode;
      }

    const short Count = (short)WkSheetNames.GetLen();
    LPDISPATCH lpDispatch;
    ORange R1,R2;

    if (bLock)
      {
      PrevCalc = pExcel->GetCalculation();
      pExcel->SetCalculation(xlCalcManual);
      RedFeedback("  ----- Please WAIT  -----  (Excel is 'locked' and 'calculations off' until SysCAD is finished) -----");
      FeedbackActivate();
      iFBLine--;
      pExcel->SetScreenUpdating(FALSE);
      Feedback(" "); //clear message
      iFBLine--;
      DidSetScrUpdating = TRUE;
      }

    //search for all 'SysCAD_Reports'...
    if (bSysCAD_Reports)
      {
      SendInfo("Search...");
      for (short i=1; i<=Count; i++)
      if (UserWkSheets[i-1])
        {
        lpDispatch = WkBook.Worksheets(i);
        WkSheet.AttachDispatch(lpDispatch, TRUE);
        lpDispatch = WkSheet.Columns();
        R1.AttachDispatch(lpDispatch, TRUE);
        LPDISPATCH lpDis = R1.Find((char*)OleReportOtherKey);
        if (lpDis)
          {
          R1.AttachDispatch(lpDis, TRUE);
          short Row = R1.GetRow();
          short Col = R1.GetColumn();
          short RowCnt = Row+3;
          CString Name = GetString(RowCnt, Col, &WkSheet, &R2);
          while (Name.GetLength()>0)
            {
            Strng TempName = (const char*)Name;
            TempName.Trim("\n\r\t \"\'");
            if (bAll || (TempName.Length()==sReportName.Length() && _stricmp(TempName(), sReportName())==0))
              {
              CExcelReport R(this, &WkBook);
              R.sName = TempName;
              CString ss = GetString(RowCnt, Col+1, &WkSheet, &R2);
              short ii = GetShort(RowCnt, Col+2, &WkSheet, &R2);
              short jj = GetShort(RowCnt, Col+3, &WkSheet, &R2);
              R.ResLoc.Set((char*)(const char*)ss, ii, jj);
              StatusBarTxt.Set("SysCAD: Report %s (%s) ...", R.sName(), R.ResLoc.sSheet());
              pExcel->SetStatusBar(StatusBarTxt());
              ss = GetString(RowCnt, Col+4, &WkSheet, &R2);
              if (ss.GetLength()==0 || _strnicmp((const char*)ss, "h", 1)==0)
                R.bResVert = 0;
              ss = GetString(RowCnt, Col+5, &WkSheet, &R2);
              ii = GetShort(RowCnt, Col+6, &WkSheet, &R2);
              jj = GetShort(RowCnt, Col+7, &WkSheet, &R2);
              R.PriLoc.Set((char*)(const char*)ss, ii, jj);
              ss = GetString(RowCnt, Col+8, &WkSheet, &R2);
              if (ss.GetLength()==0 || _strnicmp((const char*)ss, "h", 1)==0)
                R.bPriVert = 0;
              R.iPriLen = GetShort(RowCnt, Col+9, &WkSheet, &R2);
              R.iPriMaxLen = GetShort(RowCnt, Col+10, &WkSheet, &R2);
              ss = GetString(RowCnt, Col+11, &WkSheet, &R2);
              ii = GetShort(RowCnt, Col+12, &WkSheet, &R2);
              jj = GetShort(RowCnt, Col+13, &WkSheet, &R2);
              R.SecLoc.Set((char*)(const char*)ss, ii, jj);
              ss = GetString(RowCnt, Col+14, &WkSheet, &R2);
              if (ss.GetLength()==0 || _strnicmp((const char*)ss, "v", 1)==0)
                R.bSecVert = 1;
              R.iSecLen = GetShort(RowCnt, Col+15, &WkSheet, &R2);
              R.iSecMaxLen = GetShort(RowCnt, Col+16, &WkSheet, &R2);
              R.sNan = GetString(RowCnt, Col+17, &WkSheet, &R2);
              SendInfo("Generate report %s...", R.sName());
              FeedbackBlanks(1);
              BoldFeedback("Generate report %s...", R.sName());
              flag OK = R.CheckParms();
              if (OK)
                {
                OK = R.DoReport();
                TtlTagFoundCnt += R.iTagFoundCnt;
                }
              else
                Feedback("Please correct the report specification errors shown above");
              if (!OK)
                RedFeedback("%s report NOT generated", R.sName());
              SendInfo("Search...");
              }
            RowCnt++;
            lpDis = WkSheet.Cells(RowCnt, Col);
            R2.AttachDispatch(lpDis, TRUE);
            Name = R2.GetStringValue();
            }
          }
        }
      }
  
    const char* OleKeys[3] = { OleReportKey, OleReportListKey, OleReportListOffsetKey};
    Strng Examples[3];
    Examples[0].Set("%s\"R1\",H,5,32)", OleKeys[0]);
    Examples[1].Set("%s\"R1\",V,8)", OleKeys[1]);
    Examples[2].Set("%sB3,\"R2\")", OleKeys[2]);
    SendInfo("Search...");
    Strng CellNm;
    for (short i=1; i<=Count; i++)
    if (UserWkSheets[i-1])
      {
      lpDispatch = WkBook.Worksheets(i);
      WkSheet.AttachDispatch(lpDispatch, TRUE);
      lpDispatch = WkSheet.Columns();
      R1.AttachDispatch(lpDispatch, TRUE);
      LPDISPATCH lpDis = R1.Find((char*)OleReportCommonKey);
      short FirstRow = 0;
      short FirstCol = 0;
      while (lpDis)
        {
        CExcelReport R(this, &WkBook);
        R2.AttachDispatch(lpDis, TRUE);
        short Row = R2.GetRow();
        short Col = R2.GetColumn();
        short RMult = 1;
        short CMult = 1;
        if (Row==FirstRow && Col==FirstCol)
          break;
        if (FirstRow<1)
          {
          FirstRow = Row;
          FirstCol = Col;
          }
        CString Func = GetString(Row, Col, &WkSheet, &R2);
        Strng Fn = (const char*)Func;
        Fn.Trim("\n\r\t ");
        const int FnLen = Fn.Len();
        flag OK = true;
        flag SearchStrOK = true;
        int SearchTypes = -1;
        if (FnLen>(int)strlen(OleReportKey) && _strnicmp(Fn(), OleReportKey, strlen(OleReportKey))==0)
          SearchTypes = 0;
        else if (FnLen>(int)strlen(OleReportListKey) && _strnicmp(Fn(), OleReportListKey, strlen(OleReportListKey))==0)
          SearchTypes = 1;
        else if (FnLen>(int)strlen(OleReportListOffsetKey) && _strnicmp(Fn(), OleReportListOffsetKey, strlen(OleReportListOffsetKey))==0)
          SearchTypes = 2;
        else
          {
          SearchStrOK = false;
          OK = false;
          }

        R.bIsTagList = (SearchTypes!=0);
        R.bIsTagOffsetList = (SearchTypes==2);
        int RRow,RCol;
        int OffsetFnErr = 0;
        int ReportFnErr = 0;
        if (OK)
          {
          if (SearchTypes==2)
            {
            OffsetFnErr = R.ParseOffsetFn(Fn(), &WkSheet, RRow, RCol);
            OK = (OffsetFnErr==0);
            }
          else
            {
            ReportFnErr = R.ParseFn(Fn());
            OK = (ReportFnErr==0);
            RRow = Row;
            RCol = Col;
            RMult= R.bPriVert ? R.iSecMult : 1;
            CMult= R.bPriVert ? 1 : R.iSecMult;
            }
          }
        if (OK)
          {
          if (bAll || (R.sName.Length()==sReportName.Length() && _stricmp(R.sName(), sReportName())==0))
            {
            s.Set("Generate report %s at cell(%s)...", R.sName(), CellName(Row, Col, CellNm));
            SendInfo(s());
            FeedbackBlanks(1);
            BoldFeedback(s());
            if (SearchTypes==2)
              {
              if (R.bPriVert)
                R.ResLoc.Set(WkSheetNames[i-1](), Row+1, Col);
              else
                R.ResLoc.Set(WkSheetNames[i-1](), Row, Col+1);
              }
            else
              R.ResLoc.Set(WkSheetNames[i-1](), Row+RMult, Col+CMult);
            StatusBarTxt.Set("SysCAD: Report %s (%s) ...", R.sName(), R.ResLoc.sSheet());
            pExcel->SetStatusBar(StatusBarTxt());
            if (R.bPriVert)
              {
              R.PriLoc.Set(WkSheetNames[i-1](), RRow+RMult, RCol);
              R.SecLoc.Set(WkSheetNames[i-1](), RRow, RCol+1);
              }
            else
              {
              R.PriLoc.Set(WkSheetNames[i-1](), RRow, RCol+CMult);
              R.SecLoc.Set(WkSheetNames[i-1](), Row+1, Col);
              }
            OK = R.CheckParms();
            if (OK)
              {
              OK = R.DoReport();
              TtlTagFoundCnt += R.iTagFoundCnt;
              }
            else
              {
              Feedback("Please correct the report specification errors shown above");
              Feedback("Check syntax %s....)  eg %s", OleKeys[SearchTypes], Examples[SearchTypes]());
              }
            if (!OK)
              RedFeedback("%s report NOT generated", R.sName());
            SendInfo("Search...");
            }
          }
        else if (SearchStrOK)
          {
          flag DoMsg = bAll;
          if (!bAll)
            {
            if (OffsetFnErr==0 || OffsetFnErr==-1)
              DoMsg = true;
            if (OffsetFnErr<-1 && R.sName.Length()==sReportName.Length() && _stricmp(R.sName(), sReportName())==0)
              DoMsg = true;
            if (ReportFnErr==0 || ReportFnErr==-1)
              DoMsg = true;
            }
          if (DoMsg)
            {
            FeedbackBlanks(1);
            Feedback("Worksheet '%s' cell(%s) %s", WkSheetNames[i-1](), CellName(Row, Col, CellNm), Fn());
            switch (OffsetFnErr)
              {
              case -2: Feedback("Illegal cell reference for parameter 1"); break;
              case -3: Feedback("Cell referenced does not have valid %s...)", OleReportListKey); break;
              case -4: Feedback("Invalid syntax for %s...) at referenced cell", OleReportListKey); break;
              }
            Feedback("Illegal syntax for %s....)  eg %s", OleKeys[SearchTypes], Examples[SearchTypes]());
            RedFeedback("Report NOT generated");
            }
          }
        lpDis = WkSheet.Cells(Row, Col);
        lpDis = R1.FindNext(lpDis);
        }
      }

    if (DidSetScrUpdating)
      {
      pExcel->SetCalculation(PrevCalc);
      pExcel->SetScreenUpdating(TRUE);
      DidSetScrUpdating = FALSE;
      }
    SendInfo("Done");
    FeedbackBlanks(1);
    Feedback("Total of %d tags returned. (Time:%s)", TtlTagFoundCnt, sw.MinSecDesc(cs));
    //Feedback("Total of %d tags returned. (Time:%s)", TtlTagFoundCnt, sw.MinSecHunDesc(cs));
    FeedbackBlanks(1);
    if (pExcel->GetCalculation()!=xlCalcAutomatic)
      BoldFeedback("NOTE : Excel Calculation mode is currently NOT set to Automatic!");
    Feedback("Do not enter any information in this worksheet, it is cleared each time reports are generated.");
    FeedbackActivate();
    if (bSaveOnComplete)
      WkBook.Save();
    StatusBarTxt.Set("SysCAD: Done (Time:%s)", sw.MinSecDesc(cs));
    pExcel->SetStatusBar(StatusBarTxt());
    R1.ReleaseDispatch();
    R2.ReleaseDispatch();
    WkSheet.ReleaseDispatch();
    WkSheets.ReleaseDispatch();
    WkBook.ReleaseDispatch();
    WkBooks.ReleaseDispatch();
    pFBSheet->ReleaseDispatch();
    pExcel->RestoreStatusBar();
    if (!bUsingActive)
      pExcel->Quit();
    pExcel->ReleaseDispatch();
    delete pFBSheet;
    pFBSheet = NULL;
    delete pExcel;
    pExcel = NULL;
    sw.Stop(); 
    LogNote(OLEServerDesc(), 0, "Finished generating tag values report %s (Time:%s)", sFileName(), sw.MinSecDesc(cs));
    DoneBeep();
    }
  catch( COleDispatchException* e )
    {
    // Handle the OleDispatch exception here.
    ErrMsg.Set("Ole Dispatch Exception : %s [%s]", (const char*)e->m_strDescription, (const char*)e->m_strSource);
    e->Delete();
    Failed = 1;
    }
  catch( COleException * e )
    {
    // Handle the Ole exceptions here.
    char buffer[256];
    e->GetErrorMessage(buffer, sizeof(buffer));
    ErrMsg.Set("Ole Exception : %s", buffer);
    e->Delete();
    Failed = 1;
    }
  delete m_pComCmd;
  gs_Events.Send(ComCmd_Report_GetValues, /*m_pComCmd,*/ Failed ? 1:0);
  if (Failed)
    {
    if (pFBSheet)
      {
      delete pFBSheet;
      pFBSheet = NULL;
      }
    if (pExcel)
      {
      SendInfo("Error, Quit...");
      try
        {//try recover elegently...
        if (DidSetScrUpdating)
          {
          pExcel->SetCalculation(PrevCalc);
          pExcel->SetScreenUpdating(TRUE);
          DidSetScrUpdating = FALSE;
          }
        if (!bUsingActive)
          pExcel->Quit();
        pExcel->ReleaseDispatch();
        }
      catch( COleDispatchException* e )
        {
        e->Delete();
        }
      catch( COleException * e )
        {
        e->Delete();
        }
      delete pExcel;
      pExcel = NULL;
      }
    ReportError(5, ErrMsg());
    return 5;
    }
  return 0;
  }

//===========================================================================

CRepTrndItem::CRepTrndItem(double Time, int nPts)// : Values(nPts)
  {
  Values.SetSize(nPts);
  dTime = Time;
  pNxt = NULL;
  }

//===========================================================================

CExcelReportTrend::CExcelReportTrend(COleReportTrendMngr* Mngr, OWorkbook* WkBook)
  {
  pMngr = Mngr;
  pWkBook = WkBook;
  sName = "";
  bGotName = 0;
  bGotLocation = 0;
  bGotHeadings = 0;
  bGotStart = 0;
  bGotEnd = 0;
  bGotDuration = 0;
  bGotOpt = 0;
  bGotTimeOpt = 0;
  bGotChart = 0;
  dStartTime = 0.0;
  dEndTime = 0.0;
  dDuration = 3600.0;
  iOpt = 1;//AveEqualySpaced
  bTimeOptFull = 0;
  bTimeOptVariant = 0;
  iTimeOptUnits = 0; //0=none; 1=seconds; 2=minutes; 3=hours; 4=days
  iNoOfPts = 61;
  sChartName = "";
//enum QueryDestination { Trend, File, DDE, OLE };
//enum QueryOption { All, AveEqualySpaced };
  }

//---------------------------------------------------------------------------

flag CExcelReportTrend::CheckParms(bool VertOpts)
  {
  flag OK = True;
  COleReportTrendMngr& M = *pMngr;
  if (bGotLocation)
    {
    if (M.WkSheetNames.Find(ResLoc.sSheet())<0)
      {
      OK = False;
      M.Feedback("Result worksheet '%s' not found", ResLoc.sSheet());
      }
    if (!ResLoc.Check())
      {
      OK = False;
      M.Feedback("All row and column indexes should be greater than zero");
      }
    }
  else
    {
    OK = False;
    M.Feedback("A result location MUST be specified. eg Location(Sheet2,1,1)");
    }
  if (iTagLen==0)
    {
    OK = False;
    if (VertOpts)
      M.Feedback("One or more valid tags must be listed vertically below %s options and empty cell", OleReportTrend2Key);
    else
      M.Feedback("One or more valid tags must be listed horizontally in the row below %s", OleReportTrendKey);
    }
  if (bGotOpt)
    {
    if (iOpt==1 && iNoOfPts<1)
      {
      M.Feedback("AvePoints(%d): Number of points must be greater than zero", iNoOfPts);
      OK = False;
      }
    if (iOpt==2 && iNoOfPts<1)
      {
      M.Feedback("EndPoints(%d): Number of points must be greater than zero", iNoOfPts);
      OK = False;
      }
    }
  else
    M.Feedback("Query type not specified. Assume AvePoints(%d)", iNoOfPts);
  flag ValidTimeOpt = 1;
  if (bGotDuration && !bGotStart && !bGotEnd)
    {
    dEndTime = gs_Exec.TimeStopped(); //current time
    dStartTime = dEndTime - dDuration;
    }
  else if (bGotDuration && bGotStart && !bGotEnd)
    dEndTime = dStartTime + dDuration;
  else if (bGotDuration && !bGotStart && bGotEnd)
    dStartTime = dEndTime - dDuration;
  else if (!bGotDuration && bGotStart && bGotEnd)
    dDuration = dEndTime - dStartTime;
  else
    {
    M.Feedback("Query times not correctly specified, use a valid combination of Start(), End() and Duration()");
    OK = False;
    ValidTimeOpt = 0;
    }
  if (ValidTimeOpt && dStartTime>=dEndTime)
    {
    M.Feedback("Start time must be less then the end time");
    OK = False;
    }
  if (ValidTimeOpt && dEndTime<0.0)
    {
    char buff[256];
    M.Feedback("Start and End time cannot be less than '%s'", SecstoHMSDate(0.0, buff, true));
    OK = False;
    }
  if (ValidTimeOpt && dStartTime<0.0)
    {
    char buff[256];
    if (iOpt==0)
      {//All
      dStartTime=0.0;
      M.Feedback("Warning: Start time too low, changed to '%s'", SecstoHMSDate(dStartTime, buff, true));
      }
    else
      {//AveEqualySpaced or LastEqualySpaced
      const double Interval = dDuration/Max(1L, iNoOfPts-1);
      iNoOfPts = Max(1L, iNoOfPts - (long)floor(fabs(dStartTime)/Interval) - 1L);
      dStartTime = dEndTime - ((iNoOfPts-1)*Interval);
      M.Feedback("Warning: Start time too low, changed to '%s'", SecstoHMSDate(dStartTime, buff, true));
      M.Feedback("Warning: Ave number of points changed to %d", iNoOfPts);
      }
    }
  if (bGotChart && iTimeOptUnits==0)
    {
    M.Feedback("Warning: Chart '%s' will not be generated unless the time option with units is specified", sChartName);
    bGotChart = 0;
    }
  return OK;
  }

//---------------------------------------------------------------------------

flag CExcelReportTrend::ParseFn(char* Func, bool VertOpts)
  {
  char Buff[2048];
  sprintf(Buff, "%s", Func);
  int Quote;
  CSVColArray f;
  int nParms = ParseCSVFunction(Buff, f, Quote) - 1;
  flag OK = 1;
  if (Quote)// || nParms==0)
    OK = 0;
  else if ((_stricmp(f[0], OleReportTrendKey)==0 && !VertOpts) || (_stricmp(f[0], OleReportTrend2Key)==0 && VertOpts))
    {
    if (nParms==1)
      {
      sName = f[1];
      sName.Trim("\n\r\t \"\'");
      bGotName = 1;
      }
    }
  else if (_stricmp(f[0], "Location")==0)
    {
    if (nParms!=3 || strlen(f[1])==0 || strlen(f[2])==0 || strlen(f[3])==0)
      OK = 0;
    else
      {
      bGotLocation = 1;
      Strng s = f[1];
      s.Trim("\n\r\t \"\'");
      ResLoc.Set(s(), atoi(f[2]), atoi(f[3]));
      }
    }
  else if (_stricmp(f[0], "Start")==0)
    {
    if (nParms!=1 || !HMSDatetoSecs(f[1], dStartTime))
      OK = 0;
    else
      bGotStart = 1;
    }
  else if (_stricmp(f[0], "End")==0)
    {
    if (nParms!=1 || !HMSDatetoSecs(f[1], dEndTime))
      OK = 0;
    else
      bGotEnd = 1;
    }
  else if (_stricmp(f[0], "Duration")==0)
    {
    if (nParms!=1 || !HMStoSecs(f[1], dDuration))
      OK = 0;
    else
      bGotDuration = 1;
    }
  else if (_stricmp(f[0], "Headings")==0)
    {
    bGotHeadings = 1;
    }
  else if (_stricmp(f[0], "AllPoints")==0)
    {
    iOpt = 0;//All
    bGotOpt = 1;
    }
  else if (_stricmp(f[0], "AvePoints")==0)
    {
    if (nParms!=1 || strlen(f[1])<1)
      OK = 0;
    else
      {
      iNoOfPts = atoi(f[1]);
      if (iNoOfPts<1)
        OK = 0;
      else
        {
        iOpt = 1;//AveEqualySpaced
        bGotOpt = 1;
        }
      }
    }
  else if (_stricmp(f[0], "EndPoints")==0)
    {
    if (nParms!=1 || strlen(f[1])<1)
      OK = 0;
    else
      {
      iNoOfPts = atoi(f[1]);
      if (iNoOfPts<1)
        OK = 0;
      else
        {
        iOpt = 2;//LastEqualySpaced
        bGotOpt = 1;
        }
      }
    }
  else if (_stricmp(f[0], "Time")==0)
    {
    if (nParms!=1 || strlen(f[1])<1)
      OK = 0;
    else
      {
      bGotTimeOpt = 1;
      Strng s = f[1];
      s.Lower();
      switch (s[0])
        {
        case 's': iTimeOptUnits = 1; break; //seconds
        case 'm': iTimeOptUnits = 2; break; //minutes
        case 'h': iTimeOptUnits = 3; break; //hours
        case 'd': iTimeOptUnits = 4; break; //days
        case 'f': bTimeOptFull = 1; break; //full
        case 't': bTimeOptVariant = 1; break; //time-date format
        case 'b': iTimeOptUnits = 1; bTimeOptFull=1; break; //both (for backward compatability)
        }
      }
    }
  else if (_stricmp(f[0], "TimeDate")==0)
    {
    bTimeOptVariant = 1;
    }
  else if (_stricmp(f[0], "TimeFull")==0)
    {
    bTimeOptFull = 1;
    }
  else if (_stricmp(f[0], "DoChart")==0)
    {
    if (nParms!=1 || strlen(f[1])<1)
      OK = 0;
    else
      {
      bGotChart = 1;
      sChartName = f[1];
      sChartName.Trim("\n\r\t \"\'");
      }
    }
  else
    OK = 0;
  return OK;
  }
  
//---------------------------------------------------------------------------

flag CExcelReportTrend::DoReport()
  {
  //CStopWatch sw;
  //sw.Start();
  COleReportTrendMngr& M = *pMngr;
  flag OK = True;
  M.bQueryDone = 0;
  M.bQueryTagsDone = 0;
  CXMsgLst XM;
  CXM_QueryHistoryOther *xb=new CXM_QueryHistoryOther (dStartTime, dEndTime, (long)pMngr, iOpt, iTimeOptUnits, bTimeOptFull, 0, iNoOfPts, dNAN, 3/*OLE*/, NULL, NULL, 0);
  for (int i=0; i<iTagLen; i++)
    {
    TagOffsets.Add(0L);
    xb->xAddTag(Tags[i]());
    }
  XM.PackMsg(xb);
  XM.PackMsg(new CXM_Route(M.HRoute));
  M.pExecObj->XSendMessage(XM, M.HRoute);
  Sleep(50); //gives historian time to return data
  OWorksheet WSheet;
  LPDISPATCH lpDispatch = pWkBook->Worksheets(ResLoc.sSheet());
  WSheet.AttachDispatch(lpDispatch, TRUE);
  ORange Range;
  short Row = ResLoc.iRow;
  short Col = ResLoc.iColumn;
  short RowCnt = 0;
  short SecsTimeCol = ResLoc.iColumn;
  short ColStart = ResLoc.iColumn;
  if (bTimeOptFull)
    ColStart++;
  if (bTimeOptVariant)
    ColStart++;
  if (iTimeOptUnits>0)
    {
    SecsTimeCol++;
    ColStart++;
    }
  while (!M.bQueryDone && !M.bQueryTagsDone)
    {
    Sleep(200); //gives historian time to return data
    }
  if (M.bQueryTagsDone)
    {
#ifndef _RELEASE
    for (i=0; i<iTagLen; i++)
      TagOffsets[i] = -1;
#endif
    int Cnt = 0;
    Strng s1;
    Strng s = M.sTagList;
    int Pos = s.Find(":\"");
    while (Pos>=0)
      {
      s = s.Mid(Pos+2, 16384);
      int Pos2 = s.Find('\"');
      if (Pos2>=0)
        s1 = s.Left(Pos2);
      else 
        s1 = s;
      const int tl = s1.Len();
      for (i=0; i<iTagLen; i++)
        {
        if (Tags[i].Len()>=tl && _strnicmp(Tags[i](), s1(), tl)==0 && (Tags[i].Len()==tl || Tags[i]()[tl]==' '))
          {
          TagOffsets[Cnt] = i;
          i = iTagLen;
          }
        }
      Pos = s.Find(":\"");
      Cnt++;
      }
#ifndef _RELEASE
    ASSERT_RDB(iTagLen==Cnt, "Should be equal!?!?");
    for (i=0; i<iTagLen; i++)
      ASSERT_RDB(TagOffsets[i]!=-1, "Why!!!");
#endif
    }

  char Buff[256];
  Buff[0] = '\'';
  COleVariant V;
  if (bGotHeadings)
    {
    if (bTimeOptFull)
      {
      V = "Time";
      M.Set(Row, Col++, V, &WSheet, &Range);
      }
    if (bTimeOptVariant)
      {
      V = "Time";
      M.Set(Row, Col++, V, &WSheet, &Range);
      }
    if (iTimeOptUnits>0)
      {
      switch (iTimeOptUnits)
        {
        case 1: V = "Seconds"; break;
        case 2: V = "Minutes"; break;
        case 3: V = "Hours"; break;
        case 4: V = "Days"; break;
        }
      M.Set(Row, Col++, V, &WSheet, &Range);
      }
    for (int i=0; i<iTagLen; i++)
      {
      V = Tags[i]();
      M.Set(Row, Col, V, &WSheet, &Range);
      Col++;
      }
    Row++;
    }
  short RowStart = Row;
  while (!M.bQueryDone || M.pFirst)
    {
    while (M.pFirst)
      {
      //do stuff...
      Col = ResLoc.iColumn;
      if (bTimeOptFull)
        {
        SecstoHMSDate(M.pFirst->dTime, &Buff[1]);
        V = Buff;
        M.Set(Row, Col++, V, &WSheet, &Range);
        }
      if (bTimeOptVariant)
        {
        V.ChangeType(VT_EMPTY);
        V.ChangeType(VT_DATE);
        SecsToDate1900Var(M.pFirst->dTime, ((LPVARIANT)V));
        V.ChangeType(VT_R8);
        M.Set(Row, Col++, V, &WSheet, &Range); //in-explicably, this sometimes throws an exception if variant is of type VT_DATE
        }
      if (iTimeOptUnits>0)
        {
        switch (iTimeOptUnits)
          {
          case 1: V = (M.pFirst->dTime - dStartTime); break;
          case 2: V = (M.pFirst->dTime - dStartTime)/60.0; break;
          case 3: V = (M.pFirst->dTime - dStartTime)/3600.0; break;
          case 4: V = (M.pFirst->dTime - dStartTime)/86400.0; break;
          }
        M.Set(Row, Col++, V, &WSheet, &Range);
        }
      for (int i=0; i<iTagLen; i++)
        {
        //V = M.pFirst->Values[i];
        M.Set(Row, Col+TagOffsets[i], M.pFirst->Values[i], &WSheet, &Range);
        }
      Row++;
        
      CRepTrndItem* DelItem = M.pFirst;
      M.Lock();
      M.pFirst = M.pFirst->pNxt;
      M.Unlock();
      delete DelItem;
      RowCnt++;
      }
    if (!M.bQueryDone)
      Sleep(200); //gives historian time to return data
    }
  
  if (bTimeOptFull)
    {
    //Range(Cells(RowStart,ResLoc.iColumn),Cells(RowStart+RowCnt-1,ResLoc.iColumn)).NumberFormat = "@"
    Range.AttachDispatch(WSheet.Range(RowStart, ResLoc.iColumn, RowStart+RowCnt-1, ResLoc.iColumn), TRUE);
    Range.SetNumberFormat("@");
    }
  
  if (RowCnt>0)
    {//place blank line at bottom...
    V.ChangeType(VT_EMPTY);
    Col = ResLoc.iColumn;
    if (bTimeOptFull)
      M.Set(Row, Col++, V, &WSheet, &Range);
    if (bTimeOptVariant)
      M.Set(Row, Col++, V, &WSheet, &Range);
    if (iTimeOptUnits>0)
      M.Set(Row, Col++, V, &WSheet, &Range);
    for (int i=0; i<iTagLen; i++)
      M.Set(Row, Col++, V, &WSheet, &Range);
    }
  M.BlueFeedback("Trend report generated on worksheet '%s' (%d rows).", ResLoc.sSheet(), RowCnt);

  if (bGotChart && sChartName.Length()>0)
    {//generate chart...
    M.SendInfo("Generate chart %s", sChartName());
    OCharts Charts;
    OChart Chart;
    Charts.AttachDispatch(pWkBook->Charts(), TRUE);
    if (!M.bDoneInitCharts)
      {
      M.bDoneInitCharts = 1;
      short Count = Charts.GetCount();
      M.ChartNames.SetSize(Count);
      for (short i=1; i<=Count; i++)
        {
        Chart.AttachDispatch(pWkBook->Charts(i), TRUE);
        CString ChartName = Chart.GetName();
        M.ChartNames[i-1] = (const char*)ChartName;
        }
      }
    short Count = (short)M.ChartNames.GetLen();
    short index = (short)M.ChartNames.Find(sChartName());
    if (index<0)
      {
      M.bDoneInitCharts = 0;
      Charts.Add();
      Chart.AttachDispatch(pWkBook->GetActiveChart(), TRUE);
      Chart.SetName(sChartName());
      M.pFBSheet->Activate(); //ensure the feedback sheet is active and has focus
      }
    else
      {
      Chart.AttachDispatch(pWkBook->Charts(index+1), TRUE);
      //Chart.Activate();
      }
    Chart.SetType(-4169); //xlXYScatter
    OSeries Series;
    OSeriesCollection SeriesCollection;
    lpDispatch = Chart.SeriesCollection();
    SeriesCollection.AttachDispatch(lpDispatch, TRUE);
    short SeriesCount = SeriesCollection.GetCount();
    if (SeriesCount<iTagLen)
      {
      for (i=0; i<iTagLen-SeriesCount; i++)
        {
        //Range(Cells(RowStart,ColStart),Cells(RowStart+RowCnt-1,ColStart))
        LPDISPATCH lpDis = WSheet.Range(RowStart, ColStart, RowStart+RowCnt-1, ColStart);
        SeriesCollection.Add(lpDis);
        }
      }
    else if (iTagLen<SeriesCount)
      {
      for (i=SeriesCount-1; i>=iTagLen; i--)
        {
        LPDISPATCH lpDis = Chart.SeriesCollection(i+1);
        Series.AttachDispatch(lpDis, TRUE);
        try
          {
          Series.Delete(); //this may fail (eg when series has no points)
          M.Feedback("Series %d on chart '%s' deleted", i+1, sChartName());
          }
        catch( COleDispatchException* e )
          {
          e->Delete();
          M.Feedback("Series %d on chart '%s' not required", i+1, sChartName());
          }
        catch( COleException * e )
          {
          e->Delete();
          M.Feedback("Series %d on chart '%s' not required", i+1, sChartName());
          }
        }
      }
    for (i=0; i<iTagLen; i++)
      {
      Series.AttachDispatch(Chart.SeriesCollection(i+1), TRUE);
      lpDispatch = WSheet.Range(RowStart, ColStart+i, RowStart+RowCnt-1, ColStart+i);
      Series.SetValues(lpDispatch);
      lpDispatch = WSheet.Range(RowStart, SecsTimeCol, RowStart+RowCnt-1, SecsTimeCol);
      Series.SetXValues(lpDispatch);
      }
    
    Series.ReleaseDispatch();
    SeriesCollection.ReleaseDispatch();
    Chart.ReleaseDispatch();
    Charts.ReleaseDispatch();
    M.BlueFeedback("Chart '%s' generated", sChartName());
    }

  Range.ReleaseDispatch();
  WSheet.ReleaseDispatch();
  //sw.Stop(); LogNote(OLEServerDesc(), 0, "Report generated:%g", sw.Secs());
  return OK;
  }

//---------------------------------------------------------------------------
//===========================================================================

COleReportTrendMngr::COleReportTrendMngr(CExecObj* ExecObj, CXM_Route* pRoute, char* FileName, char* ReportName)
: COleExcelBaseAuto(ExecObj, pRoute, FileName)
  {
  InitializeCriticalSection(&LockSection);
  sReportName = ReportName;
  bAll = (sReportName.Length()==0 || sReportName=="*");
  bDoneInitCharts = 0;
  bQueryDone = 0;
  bQueryTagsDone = 0;
  pFirst = NULL;
  }

//---------------------------------------------------------------------------

COleReportTrendMngr::~COleReportTrendMngr()
  {
  ASSERT(bQueryDone); //should be finished...
  DeleteCriticalSection(&LockSection);
  }

//---------------------------------------------------------------------------

void COleReportTrendMngr::Set(short Row, short Column, COleVariant& V, OWorksheet* pWkSheet, ORange* pRange)
  {
  LPDISPATCH lpDispatch = pWkSheet->Cells(Row, Column);
  pRange->AttachDispatch(lpDispatch, TRUE);
  pRange->SetValue(V);
  }

//---------------------------------------------------------------------------

short COleReportTrendMngr::GetShort(short Row, short Column, OWorksheet* pWkSheet, ORange* pRange)
  {
  LPDISPATCH lpDispatch = pWkSheet->Cells(Row, Column);
  pRange->AttachDispatch(lpDispatch, TRUE);
  return pRange->GetShortValue();
  }

//---------------------------------------------------------------------------

int COleReportTrendMngr::DoAutomation()
  {
  CAutomationHelper AutoHelper(ComCmd_Report_Trend); //destructor does work
  flag GotHistorian = pExecObj->XFindObject(pExecName_Historian, HRoute);
  if (!GotHistorian)
    {
    ReportError(4, "No historian configured, cannot generate report");
    return 4;
    }

  BOOL DidSetScrUpdating = FALSE;
  long PrevCalc = 0;
  flag Failed = 0;
  Strng ErrMsg;
  try
    {
    CStopWatch sw;
    sw.Start();
    int RetCode = StandardStartup();
    if (RetCode!=0)
      {
      if (pExcel)
        {
        delete pExcel;
        pExcel = NULL;
        }
      return RetCode;
      }

    OWorkbooks WkBooks;
    OWorksheets WkSheets;
    OWorkbook WkBook;
    OWorksheet WkSheet;
    RetCode = OpenAndInit(&WkBooks, &WkSheets, &WkBook, &WkSheet, (char*)OleReportTrendFeedbackKey);
    if (RetCode!=0)
      {
      delete pExcel;
      pExcel = NULL;
      return RetCode;
      }

    Strng CellNm;
    short Count = (short)WkSheetNames.GetLen();
    LPDISPATCH lpDispatch;
    ORange R1,R2;

    if (bLock)
      {
      PrevCalc = pExcel->GetCalculation();
      pExcel->SetCalculation(xlCalcManual);
      RedFeedback("  ----- Please WAIT  -----  (Excel is 'locked' and 'calculations off' until SysCAD is finished) -----");
      FeedbackActivate();
      iFBLine--;
      pExcel->SetScreenUpdating(FALSE);
      Feedback(" "); //clear message
      iFBLine--;
      DidSetScrUpdating = TRUE;
      }

    SendInfo("Search...");
    for (short KeyName=0; KeyName<2; KeyName++)
      {
      bool VertOpts = (KeyName==1);
      for (short i=1; i<=Count; i++)
      if (UserWkSheets[i-1])
        {
        lpDispatch = WkBook.Worksheets(i);
        WkSheet.AttachDispatch(lpDispatch, TRUE);
        lpDispatch = WkSheet.Columns();
        R1.AttachDispatch(lpDispatch, TRUE);
        LPDISPATCH lpDis = R1.Find((char*)(KeyName==0 ? OleReportTrendKey : OleReportTrend2Key));
        short FirstRow = 0;
        short FirstCol = 0;
        while (lpDis)
          {
          CExcelReportTrend R(this, &WkBook);
          R2.AttachDispatch(lpDis, TRUE);
          short Row = R2.GetRow();
          short Col = R2.GetColumn();
          if (Row==FirstRow && Col==FirstCol)
            break;
          if (FirstRow<1)
            {
            FirstRow = Row;
            FirstCol = Col;
            }
          CString Func = GetString(Row, Col, &WkSheet, &R2);
          Strng Fn = (const char*)Func;
          Fn.Trim("\n\r\t ");
          flag OK = (Fn.Length()>0);
          flag OK_Key = false;
          if (OK)
            {
            if (VertOpts)
              OK_Key = _strnicmp(Fn(), OleReportTrend2Key, strlen(OleReportTrend2Key))==0;
            else
              OK_Key = _strnicmp(Fn(), OleReportTrendKey, strlen(OleReportTrendKey))==0;
            OK = OK_Key;
            if (OK)
              OK = R.ParseFn(Fn(), VertOpts);
            }
          if (OK)
            {
            if (bAll || (R.sName.Length()==sReportName.Length() && _stricmp(R.sName(), sReportName())==0))
              {
              SendInfo("Generate report %s...", R.sName());
              FeedbackBlanks(1);
              BoldFeedback("Generate report %s...", R.sName());

              //parse all functions to setup trend report...
              short ColCnt = Col;
              short RowCnt = Row;
              if (VertOpts)
                RowCnt++;
              else
                ColCnt++;
              flag FoundBlank = 0;
              while (!FoundBlank)
                {
                Func = GetString(RowCnt, ColCnt, &WkSheet, &R2);
                Fn = (const char*)Func;
                Fn.Trim("\n\r\t ");
                FoundBlank = (Fn.Length()==0);
                if (!FoundBlank)
                  {
                  if (!R.ParseFn(Fn(), VertOpts))
                    Feedback("Error at cell(%s) '%s'", CellName(RowCnt, ColCnt, CellNm), Fn());
                  }
                if (VertOpts)
                  RowCnt++;
                else
                  ColCnt++;
                }

              //get all tags...
              R.iTagLen = 0;
              if (VertOpts)
                {
                //allow more than one blank cell
                FoundBlank = 1;
                int BlankCnt = 0;
                while (FoundBlank && BlankCnt<5)
                  {
                  Func = GetString(RowCnt, ColCnt, &WkSheet, &R2);
                  Fn = (const char*)Func;
                  Fn.Trim("\n\r\t ");
                  FoundBlank = (Fn.Length()==0);
                  if (FoundBlank)
                    {
                    BlankCnt++;
                    RowCnt++;
                    }
                  }
                }
              else
                {
                RowCnt = Row+1;
                ColCnt = Col;
                }
              FoundBlank = 0;
              while (!FoundBlank)
                {
                Func = GetString(RowCnt, ColCnt, &WkSheet, &R2);
                Fn = (const char*)Func;
                Fn.Trim("\n\r\t ");
                FoundBlank = (Fn.Length()==0);
                if (!FoundBlank)
                  {
                  BOOL DupErr = false;
                  Strng WrkTag,WrkCnvTxt;
                  TaggedObject::SplitTagCnv(Fn(), WrkTag, WrkCnvTxt);
                  const int tl = WrkTag.Len();
                  for (int j=0; j<R.iTagLen; j++)
                    if (R.Tags[j].Len()>=tl && _strnicmp(R.Tags[j](), WrkTag(), tl)==0 && (R.Tags[j].Len()==tl || R.Tags[j]()[tl]==' '))
                      DupErr = true;
                  if (DupErr)
                    {
                    Feedback("Tag '%s' requested twice", WrkTag());
                    }
                  else
                    {
                    CXM_HistoryExists *xb=new CXM_HistoryExists (0, WrkTag());
                    CXMsgLst XM;
                    XM.PackMsg(xb);
                    DWORD RetCode = pExecObj->XSendMessage(XM, HRoute);
                    if ((RetCode & RC_SM_HistRecOn)!=0)
                      {
                      if (R.iTagLen>=R.Tags.GetLen())
                        R.Tags.SetSize(R.iTagLen + 16);
                      R.Tags[R.iTagLen++] = Fn();
                      }
                    else
                      Feedback("Tag '%s' not recorded in historian", WrkTag());
                    }
                  }
                if (VertOpts)
                  RowCnt++;
                else
                  ColCnt++;
                }
              R.Tags.SetSize(R.iTagLen);

              OK = R.CheckParms(VertOpts);
              if (OK)
                OK = R.DoReport();
              else
                {
                Feedback("Please correct the report specification errors shown above");
                //Feedback("Example: ????");
                }
              if (!OK)
                RedFeedback("%s report NOT generated", R.sName());
              SendInfo("Search...");
              }
            }
          else
            {
            if (OK_Key)
              {
              FeedbackBlanks(1);
              Feedback("Worksheet '%s' cell(%s) %s", WkSheetNames[i-1](), CellName(Row, Col, CellNm), Fn());
              RedFeedback("Trend report NOT generated");
              }
            }
          lpDis = WkSheet.Cells(Row, Col);
          lpDis = R1.FindNext(lpDis);
          }
        }
      }

    if (DidSetScrUpdating)
      {
      pExcel->SetCalculation(PrevCalc);
      pExcel->SetScreenUpdating(TRUE);
      DidSetScrUpdating = FALSE;
      }
    SendInfo("Done");
    FeedbackBlanks(1);
    CString cs;
    Feedback("Trend report time:%s", sw.MinSecDesc(cs));
    FeedbackBlanks(1);
    if (pExcel->GetCalculation()!=xlCalcAutomatic)
      BoldFeedback("NOTE : Excel Calculation mode is currently NOT set to Automatic!");
    Feedback("Do not enter any information in this worksheet, it is cleared each time this report is generated.");
    FeedbackActivate();
    if (bSaveOnComplete)
      WkBook.Save();

    WkSheet.ReleaseDispatch();
    WkSheets.ReleaseDispatch();
    WkBook.ReleaseDispatch();
    WkBooks.ReleaseDispatch();
    pFBSheet->ReleaseDispatch();
    if (!bUsingActive)
      pExcel->Quit();
    pExcel->ReleaseDispatch();
    delete pFBSheet;
    pFBSheet = NULL;
    delete pExcel;
    pExcel = NULL;
    sw.Stop();
    LogNote(OLEServerDesc(), 0, "Finished generating trend report %s (Time:%s)", sFileName(), sw.MinSecDesc(cs));
    DoneBeep();
    }
  catch( COleDispatchException* e )
    {
    // Handle the OleDispatch exception here.
    ErrMsg.Set("Ole Dispatch Exception : %s [%s]", (const char*)e->m_strDescription, (const char*)e->m_strSource);
    e->Delete();
    Failed = 1;
    }
  catch( COleException * e )
    {
    // Handle the Ole exceptions here.
    char buffer[256];
    e->GetErrorMessage(buffer, sizeof(buffer));
    ErrMsg.Set("Ole Exception : %s", buffer);
    e->Delete();
    Failed = 1;
    }
  gs_Events.Send(ComCmd_Report_Trend, /*m_pComCmd,*/ Failed ? 1:0);
  if (Failed)
    {
    if (pFBSheet)
      {
      delete pFBSheet;
      pFBSheet = NULL;
      }
    if (pExcel)
      {
      try
        {//try recover elegently...
        if (DidSetScrUpdating)
          {
          pExcel->SetCalculation(PrevCalc);
          pExcel->SetScreenUpdating(TRUE);
          DidSetScrUpdating = FALSE;
          }
        if (!bUsingActive)
          pExcel->Quit();
        pExcel->ReleaseDispatch();
        }
      catch( COleDispatchException* e )
        {
        e->Delete();
        }
      catch( COleException * e )
        {
        e->Delete();
        }
      delete pExcel;
      pExcel = NULL;
      }
    ReportError(5, ErrMsg());
    return 5;
    }
  return 0;
  }

//===========================================================================

CExcelSetTags::CExcelSetTags(COleSetTagsMngr* Mngr, OWorkbook* WkBook)
  {
  pMngr = Mngr;
  bResVert = 1;
  bPriVert = 1;
  bSecVert = 0;
  bIsTagList = 0;
  bIsTagOffsetList = 0;
  iTagFoundCnt = 0;
  pWkBook = WkBook;
  }

//---------------------------------------------------------------------------

flag CExcelSetTags::CheckParms()
  {
  flag OK = True;
  COleSetTagsMngr& M = *pMngr;
  if (M.WkSheetNames.Find(ResLoc.sSheet())<0)
    {
    OK = False;
    M.Feedback("Result worksheet '%s' not found", ResLoc.sSheet());
    }
  if (M.WkSheetNames.Find(PriLoc.sSheet())<0)
    {
    OK = False;
    M.Feedback("Primary tags worksheet '%s' not found", PriLoc.sSheet());
    }
  if (M.WkSheetNames.Find(SecLoc.sSheet())<0)
    {
    OK = False;
    M.Feedback("Secondary tags worksheet '%s' not found", SecLoc.sSheet());
    }
  if (!ResLoc.Check() || !PriLoc.Check() || !SecLoc.Check())
    {
    OK = False;
    M.Feedback("All row and column indexes should be greater than zero");
    }
  if (iPriLen<1 || iPriMaxLen<iPriLen)
    {
    OK = False;
    M.Feedback("Length for primary tags should be greater than 0 and MaxLength should be greater than the length");
    }
  if (iSecLen<1 || iSecMaxLen<iSecLen)
    {
    OK = False;
    M.Feedback("Length for secondary tags should be greater than 0 and MaxLength should be greater than the length");
    }
  return OK;
  }

//---------------------------------------------------------------------------

flag CExcelSetTags::ParseFn(char* Func)
  {
  char Buff[2048];
  sprintf(Buff, "%s", Func);
  int Quote;
  CSVColArray f;
  int nParms = ParseCSVFunction(Buff, f, Quote) - 1;
  if (bIsTagList)
    {
    if (Quote || nParms!=3 || strlen(f[1])==0 || strlen(f[2])==0 || strlen(f[3])==0)
      return False;
    }
  else
    {
    if (Quote || nParms!=4 || strlen(f[2])==0 || strlen(f[3])==0 || strlen(f[4])==0)
      return False;
    }

  sName = f[1];
  sName.Trim("\n\r\t \"\'");
  if (bIsTagList && sName.Len()==0)
    return False;
  bPriVert = (!(f[2][0]=='H' || f[2][0]=='h'));
  bResVert = bPriVert;
  bSecVert = (bPriVert ? 0 : 1);
  if (bIsTagList)
    {
    iPriLen = atoi(f[3]);
    iSecLen = 1;
    }
  else
    {
    if (bPriVert)
      {
      iSecLen = atoi(f[3]);
      iPriLen = atoi(f[4]);
      }
    else
      {
      iPriLen = atoi(f[3]);
      iSecLen = atoi(f[4]);
      }
    }
  iPriMaxLen = iPriLen;
  iSecMaxLen = iSecLen;
  return True;
  }

//---------------------------------------------------------------------------

int CExcelSetTags::ParseOffsetFn(char* Func, OWorksheet* pWkSheet, int& Row, int& Col)
  {
  ASSERT(bIsTagOffsetList);
  char Buff[1024];
  sprintf(Buff, "%s", Func);
  int Quote;
  CSVColArray f;
  int nParms = ParseCSVFunction(Buff, f, Quote) - 1;
  if (Quote || nParms!=2 || strlen(f[1])==0 || strlen(f[2])==0)
    return -1;

  Strng RepName(f[2]);
  RepName.Trim("\n\r\t \"\'");
  if (RepName.Len()==0)
    return -1;
  sName = RepName;
  Strng Loc(f[1]);
  Loc.Trim("\n\r\t \"\'");
  Loc.Upper();
  COleSetTagsMngr& M = *pMngr;
  if (!M.CellRowAndCol(Loc(), Row, Col))
    return -2;
  ORange R;
  CString OFunc = M.GetString(Row, Col, pWkSheet, &R);
  Strng Fn = (const char*)OFunc;
  Fn.Trim("\n\r\t ");
  if (Fn.Len()<(int)strlen(OleSetTagListKey) || _strnicmp(OleSetTagListKey, Fn(), strlen(OleSetTagListKey))!=0)
    return -3;
  if (!ParseFn(Fn()))
    {
    sName = RepName; //must do this again because it is overwritten by ParseFn
    return -4;
    }
  sName = RepName; //must do this again because it is overwritten by ParseFn
  return 0;
  }

//---------------------------------------------------------------------------

void CExcelSetTags::GetTags(CSVector& Tags, CCellLocation& Loc, short Len, flag Vert)
  {
  Tags.SetSize(Len);
  COleSetTagsMngr& M = *pMngr;
  OWorksheet WSheet;
  LPDISPATCH lpDispatch = pWkBook->Worksheets(Loc.sSheet());
  WSheet.AttachDispatch(lpDispatch, TRUE);
  ORange Range;
  short Row = Loc.iRow;
  short Col = Loc.iColumn;
  for (long i=0; i<Len; i++)
    {
    CString s = M.GetString(Row, Col, &WSheet, &Range);
    Tags[i] = (const char*)s;
    Tags[i].Trim("\n\r\t ");
    if (Vert)
      Row++;
    else
      Col++;
    }
  if (Tags[Len-1].Length()==0)
    {
    long NewLen = Len-1;
    while (NewLen>0 && Tags[NewLen-1].Length()==0)
      NewLen--;
    Tags.SetSize(NewLen);
    }
  Range.ReleaseDispatch();
  WSheet.ReleaseDispatch();
  }

//---------------------------------------------------------------------------

flag CExcelSetTags::DoSetTags(OWorksheet& WkSheet)
  {
  //CStopWatch sw;
  //sw.Start();
  iTagFoundCnt = 0;
  COleSetTagsMngr& M = *pMngr;
  GetTags(PriTags, PriLoc, iPriLen, bPriVert);
  if (!bIsTagList)
    GetTags(SecTags, SecLoc, iSecLen, bSecVert);
  short PriLen = (short)PriTags.GetLen();
  short SecLen = bIsTagList ? 1 : (short)SecTags.GetLen();
  flag OK = True;
  if (PriLen==0)
    {
    M.Feedback("No primary tags found");
    OK = False;
    }
  if (SecLen==0)
    {
    M.Feedback("No secondary tags found");
    OK = False;
    }
  if (!OK)
    return OK;
  
  //TaggedObject::SetXWritesBusy(true);
  gs_Exec.SetHoldValidateData(true, true, true);

  //TaggedObject::SetHoldValidateData(true);
  short IncompleteCnt = 0; //number of cells with incomplete tags
  short EmptyCnt = 0; //number of cells with empty contents
  Strng sIncompleteTags;//list of cells with incomplete tags
  Strng sEmptyCells;//list of cells with empty contents
  OWorksheet WSheet;
  LPDISPATCH lpDispatch = pWkBook->Worksheets(ResLoc.sSheet());
  WSheet.AttachDispatch(lpDispatch, TRUE);
  ORange Range,R1;
  short Row = ResLoc.iRow;
  short Col = ResLoc.iColumn;
  Strng Tag, WrkTag, WrkCnvTxt, FirstTag, LastTag, CellNm;
  for (short i=0; i<PriLen; i++)
    {
    if (bResVert)
      Col = ResLoc.iColumn;
    else
      Row = ResLoc.iRow;
    for (short j=0; j<SecLen; j++)
      {
      flag Incomplete = 0;
      M.CellName(Row, Col, CellNm);
      if (PriTags[i].Length()>0 && (bIsTagList || SecTags[j].Length()>0))
        {
        if (bIsTagList)
          Tag = PriTags[i]();
        else
          Tag.Set("%s.%s", PriTags[i](), SecTags[j]());
        TaggedObject::SplitTagCnv(Tag(), WrkTag, WrkCnvTxt);
        if (WrkTag.Len()>0)
          {
          CString sValue = M.GetString(Row, Col, &WSheet, &R1);
          Strng ss = (const char*)sValue;
          ss.Trim("\n\r\t ");
          if (FirstTag.Length()==0)
            FirstTag = WrkTag;
          LastTag = WrkTag;
          flag isNAN = (ss.Len()==1 && _stricmp(ss(), "*")==0);
          flag UseCnv = ((!isNAN) && (WrkCnvTxt.Len()>0));
          CXM_ObjectTag ObjTag(WrkTag(), (UseCnv ? TABOpt_ValCnvsOnce : 0)|TABOpt_StrList);
          CXM_ObjectData ObjData;
          CXM_Route Route;
          if (M.pExecObj->XReadTaggedItem(ObjTag, ObjData, Route))
            {
            CPkDataItem * pItem = ObjData.FirstItem();
            //flag Editable = ((pItem->Flags() & isParm)!=0);
            if (UseCnv && ((pItem->CnvIndex())==0 || Cnvs[(pItem->CnvIndex())]->Find(WrkCnvTxt())==NULL))
              {
              UseCnv = 0;
              M.Feedback("Warning: Engineering units '%s' used at cell(%s), for tag %s are invalid", WrkCnvTxt(), CellNm(), WrkTag());
              }
            PkDataUnion DU;
            const byte Typ = pItem->Type();
            flag DoSet = 1;
            if (IsStrng(Typ))
              {
              if (UseCnv)
                M.Feedback("Warning: Engineering units cannot be used for string data types at cell(%s), for tag %s", CellNm(), WrkTag());
              DU.SetTypeString(Typ, (char*)(const char*)sValue);
              }
            else
              {
              if (ss.Len()==0)
                {
                //M.Feedback("Empty cell(%s) for tag %s ignored", CellNm(), WrkTag());
                if (EmptyCnt<MaxCellFeedbackCnt)
                  {
                  if (EmptyCnt>0)
                    sEmptyCells += ' ';
                  sEmptyCells += CellNm();
                  }
                else if (EmptyCnt==MaxCellFeedbackCnt)
                  sEmptyCells += "...";
                EmptyCnt++;
                DoSet = 0;
                }
              else
                {
                if (IsFloatData(Typ))
                  {
                  if (isNAN)
                    DU.SetTypeDouble(Typ, dNAN);
                  else
                    {
                    double dValue = M.GetDouble(Row, Col, &WkSheet, &R1);
                    if (UseCnv)
                      DU.SetTypeDouble(Typ, dValue, pItem->CnvIndex(), WrkCnvTxt());
                    else
                      DU.SetTypeDouble(Typ, dValue);
                    }
                  }
                else if (IsIntData(Typ))
                  {
                  if (isNAN)
                    DU.SetTypeDouble(Typ, dNAN);
                  else
                    {
                    const flag StrList = pItem->IndexedStrList();
                    if (StrList)
                      {
                      DU.SetTypeString(tt_Generic, (char*)(const char*)sValue);
                      }
                    else
                      {
                      long lValue = M.GetLong(Row, Col, &WkSheet, &R1);
                      if (UseCnv)
                        DU.SetTypeLong(Typ, lValue, pItem->CnvIndex(), WrkCnvTxt());
                      else
                        DU.SetTypeLong(Typ, lValue);
                      }
                    }
                  }
                else
                  {
                  M.Feedback("Invalid tag (valid type?) expected at cell(%s), for tag %s", CellNm(), WrkTag());
                  DoSet = 0;
                  }
                }
              }
            if (DoSet)
              {
              CXM_ObjectData ObjData(0, 0, WrkTag(), 0, DU);
              if (M.pExecObj->XWriteTaggedItem(ObjData, Route)==TOData_OK)
                iTagFoundCnt++;
              else
                M.Feedback("SetTag at cell(%s) for %s failed (possibly invalid data)", CellNm(), WrkTag());
              }
            }
          else
            M.Feedback("%s at cell(%s) not found", WrkTag(), CellNm());
          }
        else
          Incomplete = 1;
        }
      else
        Incomplete = 1;

      if (Incomplete && !bIsTagList)
        {
        //M.Feedback("Incomplete tag at cell(%s)", CellNm()); //this message drives people crazy...
        if (IncompleteCnt<MaxCellFeedbackCnt)
          {
          if (IncompleteCnt>0)
            sIncompleteTags += ' ';
          sIncompleteTags += CellNm();
          }
        else if (IncompleteCnt==MaxCellFeedbackCnt)
          sIncompleteTags += "...";
        IncompleteCnt++;
        }
      if (bResVert)
        Col++;
      else
        Row++;
      }
    if (bResVert)
      Row++;
    else
      Col++;
    }

  //feedback at end of report...
  if (EmptyCnt>1)
    {
    /*if (sEmptyCells.Len()>512)
      {
      sEmptyCells = sEmptyCells.Left(512);
      sEmptyCells += "...";
      }*/
    M.Feedback("%d empty cells for tags at cells %s", EmptyCnt, sEmptyCells());
    }
  else if (EmptyCnt==1)
    M.Feedback("Empty cell for tag at cell %s", sEmptyCells());
  if (IncompleteCnt>1)
    {
    /*if (sIncompleteTags.Len()>512)
      {
      sIncompleteTags = sIncompleteTags.Left(512);
      sIncompleteTags += "...";
      }*/
    M.Feedback("%d incomplete tags at cells %s", IncompleteCnt, sIncompleteTags());
    }
  else if (IncompleteCnt==1)
    M.Feedback("Incomplete tag at cell %s", sIncompleteTags());
  //M.BlueFeedback("Tag set on worksheet '%s' processed %d tags (%s to %s).", ResLoc.sSheet(), iTagFoundCnt, FirstTag(), LastTag());
  M.BlueFeedback("Tag set on worksheet '%s' processed %d tags.", ResLoc.sSheet(), iTagFoundCnt);
  Range.ReleaseDispatch();
  WSheet.ReleaseDispatch();
  //sw.Stop(); LogNote(OLEServerDesc(), 0, "Tag set processed:%g", sw.Secs());
//  TaggedObject::SetXWritesBusy(false);
  gs_Exec.SetHoldValidateData(false, true, true);

  //TaggedObject::SetHoldValidateData(false);
  return OK;
  }

//===========================================================================

COleSetTagsMngr::COleSetTagsMngr(CExecObj* ExecObj, CXM_Route* pRoute, char* FileName, char* TagSetName)
: COleExcelBaseAuto(ExecObj, pRoute, FileName)
  {
  sTagSetName = TagSetName;
  bAll = (sTagSetName.Length()==0 || sTagSetName=="*");
  }

//---------------------------------------------------------------------------

double COleSetTagsMngr::GetDouble(short Row, short Column, OWorksheet* pWkSheet, ORange* pRange)
  {
  LPDISPATCH lpDispatch = pWkSheet->Cells(Row, Column);
  pRange->AttachDispatch(lpDispatch, TRUE);
  return pRange->GetDoubleValue();
  }

//---------------------------------------------------------------------------

long COleSetTagsMngr::GetLong(short Row, short Column, OWorksheet* pWkSheet, ORange* pRange)
  {
  LPDISPATCH lpDispatch = pWkSheet->Cells(Row, Column);
  pRange->AttachDispatch(lpDispatch, TRUE);
  return pRange->GetLongValue();
  }

//---------------------------------------------------------------------------

int COleSetTagsMngr::DoAutomation()
  {
  CAutomationHelper AutoHelper(ComCmd_Report_SetValues); //destructor does work
  if (gs_Exec.Busy())// CNM SolvingPB())
    {
    LogWarning(OLEServerDesc(), 0, "Unable to set tags while solving.");
    return 6;
    }
  BOOL DidSetScrUpdating = FALSE;
  long PrevCalc = 0;
  long TtlTagFoundCnt = 0;
  flag Failed = 0;
  Strng ErrMsg;
  try
    {
    CStopWatch sw;
    sw.Start();
    // Execute some code that might throw an exception.
    int RetCode = StandardStartup();
    if (RetCode!=0)
      {
      if (pExcel)
        {
        delete pExcel;
        pExcel = NULL;
        }
      return RetCode;
      }

    pExcel->SetStatusBar("SysCAD: Busy ...");
    OWorkbooks WkBooks;
    OWorksheets WkSheets;
    OWorkbook WkBook;
    OWorksheet WkSheet;
    RetCode = OpenAndInit(&WkBooks, &WkSheets, &WkBook, &WkSheet, (char*)OleSetTagsFeedbackKey);
    if (RetCode!=0)
      {
      delete pExcel;
      pExcel = NULL;
      return RetCode;
      }

    if (bLock)
      {
      PrevCalc = pExcel->GetCalculation();
      pExcel->SetCalculation(xlCalcManual);
      RedFeedback("  ----- Please WAIT  -----  (Excel is 'locked' and 'calculations off' until SysCAD is finished) -----");
      FeedbackActivate();
      iFBLine--;
      pExcel->SetScreenUpdating(FALSE);
      Feedback(" "); //clear message
      iFBLine--;
      DidSetScrUpdating = TRUE;
      }

    Strng CellNm, s, StatusBarTxt;
    CString cs;
    const short Count = (short)WkSheetNames.GetLen();
    LPDISPATCH lpDispatch;
    ORange R1,R2;

    for (int SearchTypes=0; SearchTypes<3; SearchTypes++)
      {
      SendInfo("Search...");
      const char* OleKey = (SearchTypes==0 ? OleSetTagsKey : (SearchTypes==1 ? OleSetTagListKey : OleSetTagListOffsetKey));
      Strng Example;
      if (SearchTypes==0)
        Example.Set("%s\"Inputs1\",H,5,32)", OleKey);
      else if (SearchTypes==1)
        Example.Set("%s\"Case1\",V,8)", OleKey);
      else
        Example.Set("%sB3,\"Case2\")", OleKey);
      for (short i=1; i<=Count; i++)
      if (UserWkSheets[i-1])
        {
        lpDispatch = WkBook.Worksheets(i);
        WkSheet.AttachDispatch(lpDispatch, TRUE);
        lpDispatch = WkSheet.Columns();
        R1.AttachDispatch(lpDispatch, TRUE);
        LPDISPATCH lpDis = R1.Find((char*)OleKey);
        short FirstRow = 0;
        short FirstCol = 0;
        while (lpDis)
          {
          CExcelSetTags R(this, &WkBook);
          R.bIsTagList = (SearchTypes!=0);
          R.bIsTagOffsetList = (SearchTypes==2);
          R2.AttachDispatch(lpDis, TRUE);
          short Row = R2.GetRow();
          short Col = R2.GetColumn();
          if (Row==FirstRow && Col==FirstCol)
            break;
          if (FirstRow<1)
            {
            FirstRow = Row;
            FirstCol = Col;
            }
          CString Func = GetString(Row, Col, &WkSheet, &R2);
          Strng Fn = (const char*)Func;
          Fn.Trim("\n\r\t ");
          int RRow,RCol;
          int OffsetFnErr = 0;
          flag OK = (Fn.Len()>0);
          if (OK)
            {
            if (SearchTypes==2)
              {
              OffsetFnErr = R.ParseOffsetFn(Fn(), &WkSheet, RRow, RCol);
              OK = (OffsetFnErr==0);
              }
            else
              {
              R.ParseFn(Fn());
              RRow = Row;
              RCol = Col;
              }
            }
          if (OK)
            {
            if (bAll || (R.sName.Length()==sTagSetName.Length() && _stricmp(R.sName(), sTagSetName())==0))
              {
              s.Set("Process tag set %s at cell(%s)...", R.sName(), CellName(Row, Col, CellNm));
              SendInfo(s());
              FeedbackBlanks(1);
              BoldFeedback(s());
              if (SearchTypes==2)
                {
                if (R.bPriVert)
                  R.ResLoc.Set(WkSheetNames[i-1](), Row+1, Col);
                else
                  R.ResLoc.Set(WkSheetNames[i-1](), Row, Col+1);
                }
              else
                R.ResLoc.Set(WkSheetNames[i-1](), Row+1, Col+1);
              StatusBarTxt.Set("SysCAD: Tag set %s (%s) ...", R.sName(), R.ResLoc.sSheet());
              pExcel->SetStatusBar(StatusBarTxt());
              if (R.bPriVert)
                {
                R.PriLoc.Set(WkSheetNames[i-1](), RRow+1, RCol);
                R.SecLoc.Set(WkSheetNames[i-1](), RRow, RCol+1);
                }
              else
                {
                R.PriLoc.Set(WkSheetNames[i-1](), RRow, RCol+1);
                R.SecLoc.Set(WkSheetNames[i-1](), Row+1, Col);
                }
              OK = R.CheckParms();
              if (OK)
                {
                OK = R.DoSetTags(WkSheet);
                TtlTagFoundCnt += R.iTagFoundCnt;
                }
              else
                {
                Feedback("Please correct the errors shown above");
                Feedback("Check syntax %s....)  eg %s", OleKey, Example());
                }
              if (!OK)
                RedFeedback("Tag set %s NOT processed", R.sName());
              SendInfo("Search...");
              }
            }
          else
            {
            flag DoMsg = bAll;
            if (!bAll && (OffsetFnErr==0 || OffsetFnErr==-1))
              DoMsg = true;
            if (!bAll && OffsetFnErr<-1 && R.sName.Length()==sTagSetName.Length() && _stricmp(R.sName(), sTagSetName())==0)
              DoMsg = true;
            if (DoMsg)
              {
              FeedbackBlanks(1);
              Feedback("Worksheet '%s' cell(%s) %s", WkSheetNames[i-1](), CellName(Row, Col, CellNm), Fn());
              switch (OffsetFnErr)
                {
                case -2: Feedback("Illegal cell reference for parameter 1"); break;
                case -3: Feedback("Cell referenced does not have valid %s...)", OleSetTagListKey); break;
                case -4: Feedback("Invalid syntax for %s...) at referenced cell", OleSetTagListKey); break;
                }
              Feedback("Illegal syntax for %s....)  eg %s", OleKey, Example());
              RedFeedback("Tag set NOT processed");
              }
            }
          lpDis = WkSheet.Cells(Row, Col);
          lpDis = R1.FindNext(lpDis);
          }
        }
      }

    if (DidSetScrUpdating)
      {
      pExcel->SetCalculation(PrevCalc);
      pExcel->SetScreenUpdating(TRUE);
      DidSetScrUpdating = FALSE;
      }
    SendInfo("Done");
    FeedbackBlanks(1);
    Feedback("Total of %d tags processed. (Time:%s)", TtlTagFoundCnt, sw.MinSecDesc(cs));
    FeedbackBlanks(1);
    if (pExcel->GetCalculation()!=xlCalcAutomatic)
      BoldFeedback("NOTE : Excel Calculation mode is currently NOT set to Automatic!");
    Feedback("Do not enter any information in this worksheet, it is cleared each time this report is generated.");
    FeedbackActivate();
    if (bSaveOnComplete)
      WkBook.Save();
    StatusBarTxt.Set("SysCAD: Done (Time:%s)", sw.MinSecDesc(cs));
    pExcel->SetStatusBar(StatusBarTxt());
    R1.ReleaseDispatch();
    R2.ReleaseDispatch();
    WkSheet.ReleaseDispatch();
    WkSheets.ReleaseDispatch();
    WkBook.ReleaseDispatch();
    WkBooks.ReleaseDispatch();
    pFBSheet->ReleaseDispatch();
    if (!bUsingActive)
      pExcel->Quit();
    pExcel->ReleaseDispatch();
    delete pFBSheet;
    pFBSheet = NULL;
    delete pExcel;
    pExcel = NULL;
    sw.Stop(); 
    LogNote(OLEServerDesc(), 0, "Finished processing tag sets in %s (Time:%s)", sFileName(), sw.MinSecDesc(cs));
    DoneBeep();
    }
  catch( COleDispatchException* e )
    {
    // Handle the OleDispatch exception here.
    ErrMsg.Set("Ole Dispatch Exception : %s [%s]", (const char*)e->m_strDescription, (const char*)e->m_strSource);
    e->Delete();
    Failed = 1;
    }
  catch( COleException * e )
    {
    // Handle the Ole exceptions here.
    char buffer[256];
    e->GetErrorMessage(buffer, sizeof(buffer));
    ErrMsg.Set("Ole Exception : %s", buffer);
    e->Delete();
    Failed = 1;
    }

  gs_Events.Send(ComCmd_Report_SetValues, /*m_pComCmd,*/ Failed ? 1:0);

  if (Failed)
    {
    if (pFBSheet)
      {
      delete pFBSheet;
      pFBSheet = NULL;
      }
    if (pExcel)
      {
      SendInfo("Error, Quit...");
      try
        {//try recover elegently...
        if (DidSetScrUpdating)
          {
          pExcel->SetCalculation(PrevCalc);
          pExcel->SetScreenUpdating(TRUE);
          DidSetScrUpdating = FALSE;
          }
        if (!bUsingActive)
          pExcel->Quit();
        pExcel->ReleaseDispatch();
        }
      catch( COleDispatchException* e )
        {
        e->Delete();
        }
      catch( COleException * e )
        {
        e->Delete();
        }
      delete pExcel;
      pExcel = NULL;
      }
    //TaggedObject::SetXWritesBusy(false); //ensure this flag does NOT get left on
    gs_Exec.SetHoldValidateData(false, true, true);
    ReportError(5, ErrMsg());
    return 5;
    }
  return 0;
  }

//===========================================================================

COleStartExcel::COleStartExcel(CExecObj* ExecObj, CXM_Route* pRoute, char* FileName)
: COleExcelBaseAuto(ExecObj, pRoute, FileName)
  {
  }

//---------------------------------------------------------------------------

int COleStartExcel::DoAutomation()
  {
  bMakeActive = 1;
  bActivateSysCAD = 0;
  //CAutomationHelper AutoHelper(Cmd_???); //destructor does work
  Strng ErrMsg;
  try
    {// Execute some code that might throw an exception.
    int RetCode = StandardStartup(); //open's Excel with correct file
    if (RetCode!=0)
      {
      if (pExcel)
        {
        delete pExcel;
        pExcel = NULL;
        }
      return RetCode;
      }

    OWorkbooks WkBooks;
    OWorkbook WkBook;
    RetCode = Open(&WkBooks, &WkBook, true);
    if (RetCode!=0)
      {
      delete pExcel;
      pExcel = NULL;
      return RetCode;
      }
    
    pExcel->ReleaseDispatch();
    delete pExcel;
    pExcel = NULL;
    }
  catch( COleDispatchException* e )
    {
    // Handle the OleDispatch exception here.
    ErrMsg.Set("Ole Dispatch Exception : %s [%s]", (const char*)e->m_strDescription, (const char*)e->m_strSource);
    e->Delete();
    }
  catch( COleException * e )
    {
    // Handle the Ole exceptions here.
    char buffer[256];
    e->GetErrorMessage(buffer, sizeof(buffer));
    ErrMsg.Set("Ole Exception : %s", buffer);
    e->Delete();
    }
  return 0;
  }

//===========================================================================

COleSaveCloseExcel::COleSaveCloseExcel(CExecObj* ExecObj, CXM_Route* pRoute, char* FileName)
: COleExcelBaseAuto(ExecObj, pRoute, FileName)
  {
  }

//---------------------------------------------------------------------------

int COleSaveCloseExcel::DoAutomation()
  {
  CAutomationHelper AutoHelper(ComCmd_Report_SaveAndClose); //destructor does work
  Strng ErrMsg;
  try
    {// Execute some code that might throw an exception.
    int RetCode = StandardStartup(); //open's Excel with correct file
    if (RetCode!=0)
      {
      if (pExcel)
        {
        delete pExcel;
        pExcel = NULL;
        }
      return RetCode;
      }

    OWorkbooks WkBooks;
    OWorkbook WkBook;
    flag WorkbookFound = 0;
    LPDISPATCH lpDispatch;
    if (bUsingActive)
      {//Excel allready open, go and find workbook...
      lpDispatch = pExcel->Workbooks();
      if (lpDispatch)
        {
        WkBooks.AttachDispatch(lpDispatch, TRUE);
        short Count = WkBooks.GetCount();
        for (short i=1; (i<=Count && WorkbookFound==0); i++)
          {
          WkBook.AttachDispatch(pExcel->Workbooks(i), TRUE);
          CString WorkbookName = WkBook.GetName();
          if (_stricmp((const char*)WorkbookName, (const char*)sWorkbookName)==0) //check if workbook names are the same
            {
            WorkbookFound = 1;
            SendInfo("Save and close %s", (const char*)sWorkbookName);
            lpDispatch = pExcel->Workbooks((char*)(const char*)sWorkbookName);
            WkBook.AttachDispatch(lpDispatch, TRUE);
            WkBook.Activate();
            //WkBook.SetSaved(TRUE);
            WkBook.Save();
            WkBook.Close(FALSE);
            WkBook.ReleaseDispatch();
            }
          }
        }
      }
   
    pExcel->ReleaseDispatch();
    delete pExcel;
    pExcel = NULL;
    delete m_pComCmd;
    gs_Events.Send(ComCmd_Report_SaveAndClose, /*m_pComCmd,*/ 0);
    }
  catch( COleDispatchException* e )
    {
    // Handle the OleDispatch exception here.
    ErrMsg.Set("Ole Dispatch Exception : %s [%s]", (const char*)e->m_strDescription, (const char*)e->m_strSource);
    e->Delete();
    delete m_pComCmd;
    gs_Events.Send(ComCmd_Report_SaveAndClose, /*m_pComCmd,*/ 1);
    }
  catch( COleException * e )
    {
    // Handle the Ole exceptions here.
    char buffer[256];
    e->GetErrorMessage(buffer, sizeof(buffer));
    ErrMsg.Set("Ole Exception : %s", buffer);
    e->Delete();
    delete m_pComCmd;
    gs_Events.Send(ComCmd_Report_SaveAndClose, /*m_pComCmd,*/ 2);
    }
  return 0;
  }

//===========================================================================

COleExcelMacro::COleExcelMacro(CExecObj* ExecObj, CXM_Route* pRoute, char* FileName, char* MacroName)
: COleExcelBaseAuto(ExecObj, pRoute, FileName)
  {
  sMacroName = MacroName;
  }

//---------------------------------------------------------------------------

int COleExcelMacro::DoAutomation()
  {
  CAutomationHelper AutoHelper(ComCmd_Report_ExecuteMacro); //destructor does work
  flag Failed = 0;
  Strng ErrMsg;
  try
    {// Execute some code that might throw an exception.
    int RetCode = StandardStartup(); //open's Excel with correct file
    if (RetCode!=0)
      {
      if (pExcel)
        {
        delete pExcel;
        pExcel = NULL;
        }
      return RetCode;
      }

    OWorkbooks WkBooks;
    OWorkbook WkBook;
    RetCode = Open(&WkBooks, &WkBook, true);
    if (RetCode!=0)
      {
      delete pExcel;
      pExcel = NULL;
      return RetCode;
      }
    
    /*
    char fstr[1024];
    //Run("Abc2(2)")
    //sprintf(fstr, "CCC.abc2(5)");
    sprintf(fstr, "Module1.abc()");
    sprintf(fstr, "Module1.Sub_abc1()");
    sprintf(fstr, "Module1.Sub_abc2(3)");
    sprintf(fstr, "Module1.abc()");
    COleVariant VV(fstr);
    COleVariant VVV;
    VVV = pExcel->Run(VV);
    */

    //TODO: Check that Excel Macro exists!
    SendInfo("Execute Macro '%s'", sMacroName());
    COleVariant VV(sMacroName());
    COleVariant VVV;
    VVV = pExcel->Run(VV);

    pExcel->ReleaseDispatch();
    delete pExcel;
    pExcel = NULL;
    }
  catch( COleDispatchException* e )
    {
    // Handle the OleDispatch exception here.
    ErrMsg.Set("Ole Dispatch Exception : %s [%s]", (const char*)e->m_strDescription, (const char*)e->m_strSource);
    e->Delete();
    Failed = 1;
    }
  catch( COleException * e )
    {
    // Handle the Ole exceptions here.
    char buffer[256];
    e->GetErrorMessage(buffer, sizeof(buffer));
    ErrMsg.Set("Ole Exception : %s", buffer);
    e->Delete();
    Failed = 1;
    }
  if (Failed)
    {
    ReportError(5, ErrMsg());
    return 5;
    }
  return 0;
  }

//===========================================================================

COleInfoReportMngr::COleInfoReportMngr(CExecObj* ExecObj, CXM_Route* pRoute, char* FileName)
: COleExcelBaseAuto(ExecObj, pRoute, FileName)
  {
  for (int j=0; j<MaxGenInfoSheets; j++)
    pGISheet[j] = NULL;
  }

//---------------------------------------------------------------------------

void COleInfoReportMngr::ReportTagLists0(OWorksheet* pSheet)
  {
  LPDISPATCH lpDispatch;
  ORange Range;
  int iLine,i,t,ColRef;
  Strng ClassId;
  CEvalOrderArray Info;
  char Buff[1024];

  const short StartLine = 4;

  bool Format1 = true;
  bool Format2 = true;
  bool Format3 = true;
  bool Format4 = false;
  
  int MaxLineCnt = StartLine+3;

  const short Fmt1Col = 2;
  const short Fmt2Col = Fmt1Col+4;
  const short Fmt3Col = Fmt2Col+6;
  const short Fmt4Col = Fmt3Col+6;
  const short End_Col = Fmt4Col+5;
  
  CTagTrees TagTrees;
  if (Format1 || Format2)
    TagTrees.Rebuild(RQ_Tags);//RQ_TagsConns);

  if (Format1)
    {
    iLine = StartLine;
    ColRef = Fmt1Col;

    bool DoneOne = false;
    lpDispatch = pSheet->Columns(ColRef-1);
    Range.AttachDispatch(lpDispatch, TRUE);
    Range.SetColumnWidth(1);
    SetCellText("Units (not ordered)", iLine, ColRef, pSheet);
    iLine+=3;
    SetCellText("Unit", iLine, ColRef, pSheet, 0, false, true);
    SetCellText(/*"Type"*/"ClassId", iLine, ColRef+1, pSheet, 0, false, true);
    iLine++;
    CModelTypeListArray& List = TagTrees.GetList();
    for (int j=0; j<List.GetSize(); j++)
      {
      CModelTypeList* pTagList = List[j];
      if (pTagList->bUnit)
        {
        for (int k=0; k<pTagList->GetSize(); k++)
          {
          DoneOne = true;
          lpDispatch = pSheet->Cells(iLine, ColRef);
          Range.AttachDispatch(lpDispatch, TRUE);
          Range.SetValue(pTagList->GetTagAt(k));
          lpDispatch = pSheet->Cells(iLine, ColRef+1);
          Range.AttachDispatch(lpDispatch, TRUE);
          Range.SetValue(pTagList->ModelName());
          iLine++;
          }
        }
      }
    if (DoneOne)
      {
      sprintf(Buff, "=COUNTA(R[3]C:R[%d]C)", iLine-StartLine-2);
      SetCellText(Buff, StartLine+1, ColRef, pSheet);
      SetCellText("Unit Count", StartLine+1, ColRef+1, pSheet);
      }
    MaxLineCnt = Max(MaxLineCnt, iLine+2);
    }

  if (Format2)
    {
    iLine = StartLine;
    ColRef = Fmt2Col;

    bool DoneOne = false;
    lpDispatch = pSheet->Columns(ColRef-2);
    Range.AttachDispatch(lpDispatch, TRUE);
    Range.SetColumnWidth(1);//spare
    lpDispatch = pSheet->Columns(ColRef-1);
    Range.AttachDispatch(lpDispatch, TRUE);
    Range.SetColumnWidth(1);
    SetCellText("All Links (not ordered)", iLine, ColRef, pSheet);
    iLine+=3;
    SetCellText(/*"Stream"*/"Link", iLine, ColRef, pSheet, 0, false, true);
    SetCellText(/*"Type"*/"ClassId", iLine, ColRef+1, pSheet, 0, false, true);
    SetCellText("Source", iLine, ColRef+2, pSheet, 0, false, true);
    SetCellText("Destination", iLine, ColRef+3, pSheet, 0, false, true);
    iLine++;
    RequestModelInfoRec MInfo;
    CModelTypeListArray& List = TagTrees.GetList();
    for (int j=0; j<List.GetSize(); j++)
      {
      CModelTypeList* pTagList = List[j];
      if (!(pTagList->bUnit))
        {
        for (int k=0; k<pTagList->GetSize(); k++)
          {
          DoneOne = true;
          lpDispatch = pSheet->Cells(iLine, ColRef);
          Range.AttachDispatch(lpDispatch, TRUE);
          Range.SetValue(pTagList->GetTagAt(k));
          lpDispatch = pSheet->Cells(iLine, ColRef+1);
          Range.AttachDispatch(lpDispatch, TRUE);
          Range.SetValue(pTagList->ModelName());
          RequestModelIOConnRec ConnInfo;
          int jj = 0;
          while (gs_pPrj->pFlwLib->RequestModelIOConn(pTagList->GetTagAt(k), jj++, ConnInfo))
            {
            if (jj<3)
              {
              lpDispatch = pSheet->Cells(iLine, ColRef+1+jj);
              Range.AttachDispatch(lpDispatch, TRUE);
              Range.SetValue(ConnInfo.Tag());
              }
            }
          iLine++;
          }
        }
      }
    if (DoneOne)
      {
      sprintf(Buff, "=COUNTA(R[3]C:R[%d]C)", iLine-StartLine-2);
      SetCellText(Buff, StartLine+1, ColRef, pSheet);
      SetCellText("Link Count", StartLine+1, ColRef+1, pSheet);
      }
    MaxLineCnt = Max(MaxLineCnt, iLine+2);
    }

  if (Format3)
    {
    iLine = StartLine;
    ColRef = Fmt3Col;

    bool DoneOne = false;
    lpDispatch = pSheet->Columns(ColRef-2);
    Range.AttachDispatch(lpDispatch, TRUE);
    Range.SetColumnWidth(1);//spare
    lpDispatch = pSheet->Columns(ColRef-1);
    Range.AttachDispatch(lpDispatch, TRUE);
    Range.SetColumnWidth(1);
    SetCellText("All Links (evaluation order)", iLine, ColRef, pSheet);
    iLine+=3;
    SetCellText("Stream"/*"Link"*/, iLine, ColRef, pSheet, 0, false, true);
    SetCellText(/*"Type"*/"ClassId", iLine, ColRef+1, pSheet, 0, false, true);
    SetCellText("Source", iLine, ColRef+2, pSheet, 0, false, true);
    SetCellText("Destination", iLine, ColRef+3, pSheet, 0, false, true);
    iLine++;
    long iRet=gs_pPrj->pFlwLib->FE_GetEvalOrder(0, 1, 1, TV_AbsChg, Info);
    //long iRet=gs_pPrj->pFlwLib->FE_GetEvalOrder(0, 1, Info);
    for (t=0; t<Info.GetSize(); t++)
      {
      CEvalOrderItem &I=Info[t];
      flag b = gs_pPrj->RequestModelClassId(I.m_sTag(), ClassId);
      if (ClassId=="Pipe-1")//todo or any other link!
        {
        DoneOne = true;
        lpDispatch = pSheet->Cells(iLine, ColRef);
        Range.AttachDispatch(lpDispatch, TRUE);
        Range.SetValue(I.m_sTag());
        RequestModelIOConnRec ConnInfo;
        int jj = 0;
        while (gs_pPrj->pFlwLib->RequestModelIOConn(I.m_sTag(), jj++, ConnInfo))
          {
          if (jj<3)
            {
            lpDispatch = pSheet->Cells(iLine, ColRef+1+jj);
            Range.AttachDispatch(lpDispatch, TRUE);
            Range.SetValue(ConnInfo.Tag());
            }
          }
        iLine++;
        }
      }
    if (DoneOne)
      {
      sprintf(Buff, "=COUNTA(R[3]C:R[%d]C)", iLine-StartLine-2);
      SetCellText(Buff, StartLine+1, ColRef, pSheet);
      SetCellText(/*"Link*/"Stream Count", StartLine+1, ColRef+1, pSheet);
      }
    MaxLineCnt = Max(MaxLineCnt, iLine+2);
    }

  if (Format4)
    {
    iLine = StartLine;
    ColRef = Fmt4Col;

    bool DoneOne = false;
    lpDispatch = pSheet->Columns(ColRef-2);
    Range.AttachDispatch(lpDispatch, TRUE);
    Range.SetColumnWidth(1);//spare
    lpDispatch = pSheet->Columns(ColRef-1);
    Range.AttachDispatch(lpDispatch, TRUE);
    Range.SetColumnWidth(1);
    SetCellText("All unit and links (evaluation order)", iLine, ColRef, pSheet);
    iLine+=3;
    SetCellText("Tag", iLine, ColRef, pSheet, 0, false, true);
    SetCellText("Conn", iLine, ColRef+1, pSheet, 0, false, true);
    SetCellText("ClassID", iLine, ColRef+2, pSheet, 0, false, true);
    iLine++;
    long iRet=gs_pPrj->pFlwLib->FE_GetEvalOrder(0, 1, 1, TV_AbsChg, Info);
    //long iRet=gs_pPrj->pFlwLib->FE_GetEvalOrder(0, 1, Info);
    for (t=0; t<Info.GetSize(); t++)
      {
      CEvalOrderItem &I=Info[t];
      flag b = gs_pPrj->RequestModelClassId(I.m_sTag(), ClassId);
      long NoFIOs=I.m_FIOs.GetSize();
      for (i=0; i<NoFIOs; i++)
        {
        DoneOne = true;
        CEvalOrderIOItem &F=Info[t].m_FIOs[i];
        //LogNote("EvalOrd", 0, "%2d:%s : %s  (%s)", i, I.m_sTag(), F.m_sRmt(), ClassId());

        lpDispatch = pSheet->Cells(iLine, ColRef+0);
        Range.AttachDispatch(lpDispatch, TRUE);
        Range.SetValue(I.m_sTag());
        lpDispatch = pSheet->Cells(iLine, ColRef+1);
        Range.AttachDispatch(lpDispatch, TRUE);
        Range.SetValue(F.m_sRmtDesc());
        lpDispatch = pSheet->Cells(iLine, ColRef+2);
        Range.AttachDispatch(lpDispatch, TRUE);
        Range.SetValue(ClassId());
        iLine++;
        }
      }
    if (DoneOne)
      {
      sprintf(Buff, "=COUNTA(R[3]C:R[%d]C)", iLine-StartLine-2);
      SetCellText(Buff, StartLine+1, ColRef, pSheet);
      SetCellText("Count", StartLine+1, ColRef+1, pSheet);
      }
    MaxLineCnt = Max(MaxLineCnt, iLine+4);
    }

  lpDispatch = pSheet->Columns(End_Col-1);
  Range.AttachDispatch(lpDispatch, TRUE);
  Range.SetColumnWidth(1);
  SetCellText("Do not enter any information in this worksheet, it is cleared each time this report is generated.", MaxLineCnt, 1, pSheet);
  
  Range.ReleaseDispatch();
  }

//---------------------------------------------------------------------------

void COleInfoReportMngr::ReportTagLists1(OWorksheet* pSheet)
  {
  LPDISPATCH lpDispatch;
  ORange Range;
  int iLine,ColRef;
  CEvalOrderArray Info;
  char Buff[1024];

  const short StartLine = 4;

  bool Format1 = true;
  bool Format2 = true;
  
  int MaxLineCnt = StartLine+3;

  const short Fmt1Col = 2;
  const short Fmt2Col = Fmt1Col+4;
  const short End_Col = Fmt2Col+5;
  
  if (Format1)
    {
    iLine = StartLine;
    ColRef = Fmt1Col;

    bool DoneOne = false;
    lpDispatch = pSheet->Columns(ColRef-1);
    Range.AttachDispatch(lpDispatch, TRUE);
    Range.SetColumnWidth(1);
    SetCellText("Specie list", iLine, ColRef, pSheet);
    iLine+=3;
    SetCellText("Specie", iLine, ColRef, pSheet, 0, false, true);
    SetCellText("Phase", iLine, ColRef+1, pSheet, 0, false, true);
    iLine++;

    for (int i=0; i<SVCfgCount(); i++)
      {
      CSysVecItem &I=*SVI.Cfg(i);
      switch (I.Type())
        {
        case SVT_Specie:
          {
          int iSp=I.SDBIndex();
          DoneOne = true;
          lpDispatch = pSheet->Cells(iLine, ColRef);
          Range.AttachDispatch(lpDispatch, TRUE);
          Range.SetValue(SDB[iSp].SymOrTag());
          lpDispatch = pSheet->Cells(iLine, ColRef+1);
          Range.AttachDispatch(lpDispatch, TRUE);
          if (SDB[iSp].PhaseIs(som_Sol))
            Range.SetValue("Solid");
          else if (SDB[iSp].PhaseIs(som_Liq))
            Range.SetValue("Liquid");
          else if (SDB[iSp].PhaseIs(som_Gas))
            Range.SetValue("Vapour");
          else
            Range.SetValue("Other");
          iLine++;
          break;
          }
        case SVT_Attribute:
          {
          int iAt=I.CfgIndex();
          DoneOne = true;
          lpDispatch = pSheet->Cells(iLine, ColRef);
          Range.AttachDispatch(lpDispatch, TRUE);
          Range.SetValue(SVI.Cfg(iAt)->FullTag());
          lpDispatch = pSheet->Cells(iLine, ColRef+1);
          Range.AttachDispatch(lpDispatch, TRUE);
          Range.SetValue("Attribute");
          iLine++;
          break;
          }
        case SVT_Calculation:
          {
          int iCal=I.CfgIndex();             //NBNB Index - Negetive
          DoneOne = true;
          lpDispatch = pSheet->Cells(iLine, ColRef);
          Range.AttachDispatch(lpDispatch, TRUE);
          Range.SetValue(SVI.Cfg(iCal)->FullTag());
          lpDispatch = pSheet->Cells(iLine, ColRef+1);
          Range.AttachDispatch(lpDispatch, TRUE);
          Range.SetValue("Calculation");
          iLine++;
          break;
          }
        }
      }

    if (DoneOne)
      {
      sprintf(Buff, "=COUNTA(R[3]C:R[%d]C)", iLine-StartLine-2);
      SetCellText(Buff, StartLine+1, ColRef, pSheet);
      SetCellText("Specie Count", StartLine+1, ColRef+1, pSheet);
      iLine++;
      sprintf(Buff, "=COUNTIF(R[%d]C[1]:R[-2]C[1],\"Solid\")", StartLine-iLine+4);
      SetCellText(Buff, iLine+0, ColRef, pSheet);
      SetCellText("Solid Count", iLine+0, ColRef+1, pSheet);
      sprintf(Buff, "=COUNTIF(R[%d]C[1]:R[-3]C[1],\"Liquid\")", StartLine-iLine+3);
      SetCellText(Buff, iLine+1, ColRef, pSheet);
      SetCellText("Liquid Count", iLine+1, ColRef+1, pSheet);
      sprintf(Buff, "=COUNTIF(R[%d]C[1]:R[-4]C[1],\"Vapour\")", StartLine-iLine+2);
      SetCellText(Buff, iLine+2, ColRef, pSheet);
      SetCellText("Vapour Count", iLine+2, ColRef+1, pSheet);
      }
    MaxLineCnt = Max(MaxLineCnt, iLine+4);
    }

  if (Format2)
    {
    CMdlGeneralInfo* pMGI = CMdlGeneralInfo::First();
    ColRef = Fmt2Col;
    if (pMGI)
      {
      pMGI->Init();
      bool b = pMGI->GetInfo(); //get headings, etc
      if (b)
        {
        lpDispatch = pSheet->Columns(ColRef-2);
        Range.AttachDispatch(lpDispatch, TRUE);
        Range.SetColumnWidth(1);//spare
        lpDispatch = pSheet->Columns(ColRef-1);
        Range.AttachDispatch(lpDispatch, TRUE);
        Range.SetColumnWidth(1);
        iLine = StartLine;
        for (int i=0; i<pMGI->Headings.GetSize(); i++)
          SetCellText(pMGI->Headings[i](), iLine, ColRef+i, pSheet);
        iLine++;
        if (pMGI->bStringData)
          SetCellText(pMGI->sItem(), iLine+pMGI->iItemIndex, ColRef+pMGI->iGroupIndex, pSheet);
        else
          SetCellValue(pMGI->dItem, iLine+pMGI->iItemIndex, ColRef+pMGI->iGroupIndex, pSheet);
        while (pMGI->GetInfo())
          {
          if (pMGI->bStringData)
            SetCellText(pMGI->sItem(), iLine+pMGI->iItemIndex, ColRef+pMGI->iGroupIndex, pSheet);
          else
            SetCellValue(pMGI->dItem, iLine+pMGI->iItemIndex, ColRef+pMGI->iGroupIndex, pSheet);
          }
        MaxLineCnt = Max(MaxLineCnt, iLine+pMGI->iItemIndex);
        }
      ColRef += (2 + pMGI->Headings.GetSize());
      lpDispatch = pSheet->Columns(ColRef-2);
      Range.AttachDispatch(lpDispatch, TRUE);
      Range.SetColumnWidth(1);
      lpDispatch = pSheet->Columns(ColRef-1);
      Range.AttachDispatch(lpDispatch, TRUE);
      Range.SetColumnWidth(1);
      pMGI = pMGI->Next();
      }
    }

  /*lpDispatch = pSheet->Columns(End_Col-1);
  Range.AttachDispatch(lpDispatch, TRUE);
  Range.SetColumnWidth(1);*/
  SetCellText("Do not enter any information in this worksheet, it is cleared each time this report is generated.", MaxLineCnt, 1, pSheet);
  
  Range.ReleaseDispatch();
  }

//---------------------------------------------------------------------------

void COleInfoReportMngr::ReportTagLists(int Pg)
  {
  if (gs_pPrj==NULL || gs_pPrj->pFlwLib==NULL)
    return;
  if (Pg==0)
    ReportTagLists0(pGISheet[0]);
  else if (Pg==1)
    ReportTagLists1(pGISheet[1]);
  }

//---------------------------------------------------------------------------

int COleInfoReportMngr::DoAutomation()
  {
  //CAutomationHelper AutoHelper(ComCmd_InfoReport); //destructor does work
  BOOL DidSetScrUpdating = FALSE;
  long PrevCalc = 0;
  flag Failed = 0;
  Strng ErrMsg;
  try
    {// Execute some code that might throw an exception.
    int RetCode = StandardStartup(); //open's Excel with correct file
    if (RetCode!=0)
      {
      if (pExcel)
        {
        delete pExcel;
        pExcel = NULL;
        }
      return RetCode;
      }

    OWorkbooks WkBooks;
    OWorkbook WkBook;
    RetCode = Open(&WkBooks, &WkBook, true);
    if (RetCode!=0)
      {
      delete pExcel;
      pExcel = NULL;
      return RetCode;
      }
    
    pExcel->SetStatusBar("SysCAD: Busy ...");
    char Buff00[1024];
    char Buff01[1024];
    Strng DtTm;
    sprintf(Buff00, "%s  Project : %s   (%s)", CurDateTime(DtTm), PrjName(), PrjFiles());
    sprintf(Buff01, "General Tag Information Report generated by SysCAD %s, %s", VersionNo(), FullCopyrightNotice());
    OWorksheets WkSheets;
    OWorksheet WkSheet;
    WkSheets.AttachDispatch(pExcel->Worksheets(), TRUE);
    for (int j=0; j<MaxGenInfoSheets; j++)
      {
      char WorksheetNameBuff[32];
      sprintf(WorksheetNameBuff, "%s%d", OleGenInfoWorksheetKey, j);
      SendInfo("Generate General Tag Information report (%s)", WorksheetNameBuff);

      short Count = WkSheets.GetCount();
      WkSheetNames.SetSize(Count);
      bool RqdWorksheetFound = 0;
      short i;
      for (i=1; i<=Count; i++)
        {
        WkSheet.AttachDispatch(WkBook.Worksheets(i), TRUE);
        CString SheetName = WkSheet.GetName();
        WkSheetNames[i-1] = (const char*)SheetName;
        if (_stricmp((const char*)SheetName, WorksheetNameBuff)==0)
          RqdWorksheetFound = 1;
        }

      LPDISPATCH lpDispatch;
      pGISheet[j] = new OWorksheet;
      if (!RqdWorksheetFound)
        {
        WkSheets.Add();
        lpDispatch = WkBook.GetActiveSheet();
        pGISheet[j]->AttachDispatch(lpDispatch, TRUE);
        pGISheet[j]->SetName(WorksheetNameBuff);
        }
      else
        {
        lpDispatch = WkBook.Worksheets(WorksheetNameBuff);
        pGISheet[j]->AttachDispatch(lpDispatch, TRUE);
        pGISheet[j]->Activate();
        }
      lpDispatch = pGISheet[j]->Columns();
      ORange Range;
      Range.AttachDispatch(lpDispatch, TRUE);
      Range.Clear();
      Range.ReleaseDispatch();

      if (bLock)
        {
        PrevCalc = pExcel->GetCalculation();
        pExcel->SetCalculation(xlCalcManual);
        SetCellText("  ----- Please WAIT  -----  (Excel is 'locked' and 'calculations off' until SysCAD is finished) -----", 1, 1, pGISheet[j]);
        pExcel->SetScreenUpdating(FALSE);
        DidSetScrUpdating = TRUE;
        }

      SetCellText(Buff00, 1, 1, pGISheet[j]);
      SetCellText(Buff01, 2, 1, pGISheet[j]);
  
      ReportTagLists(j); //do the work

      if (DidSetScrUpdating)
        {
        pExcel->SetCalculation(PrevCalc);
        pExcel->SetScreenUpdating(TRUE);
        DidSetScrUpdating = FALSE;
        }
      pGISheet[j]->ReleaseDispatch();
      }

    pExcel->SetStatusBar("SysCAD: Done...");
    SendInfo("Done");
    if (bSaveOnComplete)
      WkBook.Save();
    pExcel->RestoreStatusBar();
    pExcel->ReleaseDispatch();
    for (j=0; j<MaxGenInfoSheets; j++)
      {
      delete pGISheet[j];
      pGISheet[j] = NULL;
      }
    delete pExcel;
    pExcel = NULL;
    }
  catch( COleDispatchException* e )
    {
    // Handle the OleDispatch exception here.
    ErrMsg.Set("Ole Dispatch Exception : %s [%s]", (const char*)e->m_strDescription, (const char*)e->m_strSource);
    e->Delete();
    Failed = 1;
    }
  catch( COleException * e )
    {
    // Handle the Ole exceptions here.
    char buffer[256];
    e->GetErrorMessage(buffer, sizeof(buffer));
    ErrMsg.Set("Ole Exception : %s", buffer);
    e->Delete();
    Failed = 1;
    }
  if (Failed)
    {
    for (int j=0; j<MaxGenInfoSheets; j++)
      {
      if (pGISheet[j])
        {
        delete pGISheet[j];
        pGISheet[j] = NULL;
        }
      }
    if (pExcel)
      {
      SendInfo("Error, Quit...");
      try
        {//try recover elegently...
        if (DidSetScrUpdating)
          {
          pExcel->SetCalculation(PrevCalc);
          pExcel->SetScreenUpdating(TRUE);
          DidSetScrUpdating = FALSE;
          }
        if (!bUsingActive)
          pExcel->Quit();
        pExcel->ReleaseDispatch();
        }
      catch( COleDispatchException* e )
        {
        e->Delete();
        }
      catch( COleException * e )
        {
        e->Delete();
        }
      delete pExcel;
      pExcel = NULL;
      }
    ReportError(5, ErrMsg());
    return 5;
    }
  return 0;
  }

//===========================================================================

#endif


