//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __OLEEXCEL_H
#define __OLEEXCEL_H

#ifndef __WORDAPPLICATION_H
#include "WordApplication.h"
#endif
#ifndef __VECTORS_H
#include "vectors.h"
#endif

#ifdef NOTDEFINED

//===========================================================================

//===========================================================================
/*#C:Base automation class for Excel Automation*/
class COleExcelBaseAuto : public COleBaseAuto
  {
  public:
    CWordApplication* pExcel;      //Excel
    CString      sWorkbookName;     //name of workbook (filename)
    CSVector     WkSheetNames;      //array of Worksheet names
    CArray<bool, bool> UserWkSheets;//array for Worksheets with true for non-syscad pages
    OWorksheet*  pFBSheet;          //feed back Worksheet;
    short        iFBLine;           //line number for next feedback
    flag         bUseCellName:1,    //report Excel cell locations in a text form. egs C4 or AF32
                 bMakeActive:1,     //make Excel active and bring to front always
                 bUsingActive:1,
                 bUpdateLinks:1,    //update linked files, etc when opening
                 bSaveOnComplete:1, //save workbook on completion
                 bForceNormalView:1;//force all windows to normal view as PageBreakPreview slows report too much

    COleExcelBaseAuto(CExecObj* ExecObj, CXM_Route* pRoute, char* FileName);
    void SetOptions(flag Lock, flag UseCellName, flag MakeActive, flag MakeSysCADActive, flag UpdateLinks, flag SaveOnComplete);

    virtual int AutomationServer() { return OleAutoIsExcel; };
    virtual char* OLEServerDesc() { return "Excel"; };

  protected:
    void FeedbackActivate();
    void DoFeedback(char* Txt, COLORREF Color = 0, flag UseColor = False, flag Bold = False);
    void Feedback(char* fmt, ...);
    void RedFeedback(char* fmt, ...);
    void BlueFeedback(char* fmt, ...);
    void BoldFeedback(char* fmt, ...);
    void FeedbackBlanks(int BlankLines) { iFBLine += BlankLines; };
    void InitFeedback(OWorksheets* pWkSheets, OWorkbook* pWkBook, flag Exists, char* FeedbackPageName);
    int StandardStartup();
    int Open(OWorkbooks* pWkBooks, OWorkbook* pWkBook, flag OpenAlways);
    int OpenAndInit(OWorkbooks* pWkBooks, OWorksheets* pWkSheets, OWorkbook* pWkBook, OWorksheet* pWkSheet, char* FeedbackPageName);
    CString GetString(short Row, short Column, OWorksheet* pWkSheet, ORange* pRange);
    char* CellName(int Row, int Col, Strng& CellName);
    flag CellRowAndCol(char* pCellName, int& Row, int& Col);
    void SetCellText(char* Txt, short Row, short Column, OWorksheet* pWkSheet, COLORREF Color = 0, flag UseColor = False, flag Bold = False);
    void SetCellValue(double Val, short Row, short Column, OWorksheet* pWkSheet, COLORREF Color = 0, flag UseColor = False, flag Bold = False);
    void DoneBeep() { Beep(3000, 25); /*MessageBeep(0xFFFFFFFF);*/ };

  };

//===========================================================================

//_FWDDEF(COleReportMngr)
_FWDDEF(CWordDocs)
class CWordDocs

  {
  public:
    Strng sName;          //report name
    //CCellLocation ResLoc; //result location
    //CCellLocation PriLoc; //primary tags location
    //short iPriLen;        //
    //short iPriMaxLen;     //
    //CCellLocation SecLoc; //secendary tags location
    //short iSecLen;        //
    //short iSecMaxLen;     //
    //short iSecMult;       // Number of column/rows for multi report
    //Strng sNan;           //string to use for NAN
    //flag bResVert:1,      //
    //     bPriVert:1,      //
    //     bSecVert:1,      //
    //     bIsTagList:1,    //busy generating TagList type report
    //     bIsTagOffsetList:1;
    //CSVector PriTags;     //array of primary tags
    //CSVector SecTags;     //array of secondary tags
    CWordApplication * pApp;//
    CWordDocument    * pDoc;   //
    CWordTemplate    * pTmp;   //
    //long iTagFoundCnt;    //

    CWordDocs();//COleReportMngr* Mngr, OWorkbook* WkBook);
    //flag CheckParms();
    //int ParseFn(char* Func);
    //int ParseOffsetFn(char* Func, OWorksheet* pWkSheet, int& Row, int& Col);
    //flag DoReport();
  private:
    //long GetTags(CSVector& Tags, CCellLocation& Loc, short Len, short Wide, short Skip, flag Vert);
  };

//---------------------------------------------------------------------------

class COleReportMngr : public COleExcelBaseAuto
  {
  friend class CExcelReport;
  public:
    Strng sReportName;      //name of required report (*=all)
    flag bAll:1,            //fetch all reports regardless of name
         bSysCAD_Reports:1; //search for SysCAD_Reports table

    COleReportMngr(CExecObj* ExecObj, CXM_Route* pRoute, char* FileName, char* ReportName, flag SysCAD_Reports);
    virtual int DoAutomation();
    virtual char* ShortDesc() { return "Tag Values Report"; };
  private:
    void Set(short Row, short Column, COleVariant& V, OWorksheet* pWkSheet, ORange* pRange);
    short GetShort(short Row, short Column, OWorksheet* pWkSheet, ORange* pRange);
    BOOL IsFormula(short Row, short Column, OWorksheet* pWkSheet);
  };

//===========================================================================

//todo : should probably use spares for this for effeciency!
class CRepTrndItem
  {
  public:
    double dTime;
    //CDVector Values;
    CArray <COleVariant,COleVariant> Values;
    CRepTrndItem* pNxt;

    CRepTrndItem(double Time, int nPts);
  };

//---------------------------------------------------------------------------

_FWDDEF(COleReportTrendMngr)
class CExcelReportTrend
  {
  public:
    Strng sName;          //report name
    CCellLocation ResLoc; //result location
    short iTagLen;        //number of tags to be reported
    CSVector Tags;        //array of tags
    CArray <short,short> TagOffsets; //offset of returned data relative to request
    COleReportTrendMngr* pMngr;//
    OWorkbook* pWkBook;   //
    double dStartTime;    //
    double dEndTime;      //
    double dDuration;     //
    Strng sChartName;     //
    long iNoOfPts;        //
    byte iOpt;            //
    byte iTimeOptUnits;   //include numeric time in report in specified units: 0=none; 1=seconds; 2=minutes; 3=hours; 4=days

    WORD bGotName:1,
         bGotLocation:1,
         bGotHeadings:1,
         bGotStart:1,
         bGotEnd:1,
         bGotDuration:1,
         bGotOpt:1,
         bGotTimeOpt:1,
         bGotChart:1,
         bTimeOptFull:1,  //include full time 'string' in report
         bTimeOptVariant:1;  //include full time as VT_DATE variant in report

    CExcelReportTrend(COleReportTrendMngr* Mngr, OWorkbook* WkBook);
    flag CheckParms(bool VertOpts);
    flag ParseFn(char* Func, bool VertOpts);
    flag DoReport();
    flag DoDBReport();
  private:
    void GetTags(CSVector& Tags, CCellLocation& Loc, short Len, flag Vert);
  };

//---------------------------------------------------------------------------

class COleReportTrendMngr : public COleExcelBaseAuto
  {
  friend class CExcelReportTrend;
  public:
    Strng sReportName;      //name of required report (*=all)
    CSVector ChartNames;    //array of chart names
    BYTE bAll:1,            //fetch all reports regardless of name
         bQueryDone:1,      //historian finished returning data
         bQueryTagsDone:1,  //historian has returned list of tags
         bDoneInitCharts:1; //have all the chart names been fetched
    CRepTrndItem* pFirst;   //returned by historian
    Strng sTagList;         //returned by historian
  private:
    CXM_Route HRoute;
    CRITICAL_SECTION LockSection; //used for locks

  public:  
    COleReportTrendMngr(CExecObj* ExecObj, CXM_Route* pRoute, char* FileName, char* ReportName);
    virtual ~COleReportTrendMngr();
    virtual int DoAutomation();
    virtual char* ShortDesc() { return "Trend Report"; };
    inline void Lock() { EnterCriticalSection(&LockSection); };
    inline void Unlock() { LeaveCriticalSection(&LockSection); };
  private:
    void Set(short Row, short Column, COleVariant& V, OWorksheet* pWkSheet, ORange* pRange);
    short GetShort(short Row, short Column, OWorksheet* pWkSheet, ORange* pRange);
  };

//===========================================================================

_FWDDEF(COleSetTagsMngr)
_FWDDEF(CExcelSetTags)
class CExcelSetTags
  {
  public:
    Strng sName;          //tag set name
    CCellLocation ResLoc; //tag values location
    CCellLocation PriLoc; //primary tags location
    short iPriLen;        //
    short iPriMaxLen;     //
    CCellLocation SecLoc; //secendary tags location
    short iSecLen;        //
    short iSecMaxLen;     //
    //Strng sNan;           //string to use for NAN
    flag bResVert:1,      //
         bPriVert:1,      //
         bSecVert:1,      //
         bIsTagList:1,    //busy generating TagList type report
         bIsTagOffsetList:1;
    CSVector PriTags;     //array of primary tags
    CSVector SecTags;     //array of secondary tags
    COleSetTagsMngr* pMngr;//
    OWorkbook* pWkBook;   //
    long iTagFoundCnt;    //

    CExcelSetTags(COleSetTagsMngr* Mngr, OWorkbook* WkBook);
    flag CheckParms();
    flag ParseFn(char* Func);
    int ParseOffsetFn(char* Func, OWorksheet* pWkSheet, int& Row, int& Col);
    flag DoSetTags(OWorksheet& WkSheet);
  private:
    void GetTags(CSVector& Tags, CCellLocation& Loc, short Len, flag Vert);
  };

//---------------------------------------------------------------------------

class COleSetTagsMngr : public COleExcelBaseAuto
  {
  friend class CExcelSetTags;
  public:
    Strng sTagSetName;      //name of required tag set (*=all)
    flag bAll:1;            //fetch/set all regardless of name

    COleSetTagsMngr(CExecObj* ExecObj, CXM_Route* pRoute, char* FileName, char* TagSetName);
    virtual int DoAutomation();
    virtual char* ShortDesc() { return "Set Tag Values"; };
  private:
    //void Set(short Row, short Column, COleVariant& V, OWorksheet* pWkSheet, ORange* pRange);
    //short GetShort(short Row, short Column, OWorksheet* pWkSheet, ORange* pRange);
    double GetDouble(short Row, short Column, OWorksheet* pWkSheet, ORange* pRange);
    long GetLong(short Row, short Column, OWorksheet* pWkSheet, ORange* pRange);
    //BOOL IsFormula(short Row, short Column, OWorksheet* pWkSheet);
  };

//---------------------------------------------------------------------------

class COleStartExcel : public COleExcelBaseAuto
  {
  public:
    COleStartExcel(CExecObj* ExecObj, CXM_Route* pRoute, char* FileName);
    virtual int DoAutomation();
    virtual char* ShortDesc() { return "Open Excel"; };
  private:
    //double GetDouble(short Row, short Column, OWorksheet* pWkSheet, ORange* pRange);
    //long GetLong(short Row, short Column, OWorksheet* pWkSheet, ORange* pRange);
  };

//---------------------------------------------------------------------------

class COleSaveCloseExcel : public COleExcelBaseAuto
  {
  public:
    COleSaveCloseExcel(CExecObj* ExecObj, CXM_Route* pRoute, char* FileName);
    virtual int DoAutomation();
    virtual char* ShortDesc() { return "Save and Close Excel"; };
  private:
    //double GetDouble(short Row, short Column, OWorksheet* pWkSheet, ORange* pRange);
    //long GetLong(short Row, short Column, OWorksheet* pWkSheet, ORange* pRange);
  };

//---------------------------------------------------------------------------

class COleExcelMacro : public COleExcelBaseAuto
  {
  public:
    Strng sMacroName;      //name of Excel Module Macro

    COleExcelMacro(CExecObj* ExecObj, CXM_Route* pRoute, char* FileName, char* MacroName);
    virtual int DoAutomation();
    virtual char* ShortDesc() { return "Excel Macro"; };
  private:
    //double GetDouble(short Row, short Column, OWorksheet* pWkSheet, ORange* pRange);
    //long GetLong(short Row, short Column, OWorksheet* pWkSheet, ORange* pRange);
  };

//---------------------------------------------------------------------------

const int MaxGenInfoSheets = 2;
class COleInfoReportMngr : public COleExcelBaseAuto
  {
  public:
    OWorksheet* pGISheet[MaxGenInfoSheets];  //General Information worksheets for the reports

    COleInfoReportMngr(CExecObj* ExecObj, CXM_Route* pRoute, char* FileName);
    virtual int DoAutomation();
    virtual char* ShortDesc() { return "Flowsheet Information Report"; };
  private:
    void ReportTagLists0(OWorksheet* pSheet);
    void ReportTagLists1(OWorksheet* pSheet);
    void ReportTagLists(int Pg);
  };

//===========================================================================

#endif

#endif

