//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __OLEEXCEL_H
#define __OLEEXCEL_H

#include "xl5obj32.h"
#include "vectors.h"
#include "selctwnd.h"

//===========================================================================

_FWDDEF(CCellLocation)
class CCellLocation
  {
  public:
    Strng sSheet;      //worksheet name
    short iRow;        //cell row
    short iColumn;     //cell column

    CCellLocation() { sSheet = ""; iRow = 1; iColumn = 1; };
    CCellLocation(char* Sheet, short Row, short Column) { sSheet = Sheet; iRow = Row; iColumn = Column; };
    CCellLocation(short Row, short Column) { sSheet = ""; iRow = Row; iColumn = Column; };
    void Set(char* Sheet, short Row, short Column) { sSheet = Sheet; iRow = Row; iColumn = Column; };
    void Set(short Row, short Column) { iRow = Row; iColumn = Column; };
    flag Check() { return (iRow>0 && iColumn>0); };
  };

//===========================================================================
/*#C:Base automation class for Excel Automation*/
class COleExcelBaseAuto : public COleBaseAuto
  {
  public:
    OExcelApplication* pExcel;      //Excel
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

  public:
    void Feedback(char* fmt, ...);
  protected:
    void FeedbackActivate();
    void DoFeedback(char* Txt, COLORREF Color = 0, flag UseColor = False, flag Bold = False);
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

_FWDDEF(COleReportMngr)
_FWDDEF(CExcelReport)

//class CExcelTagValue
//  {
//  public:
//    Strng m_sTag;
//    Strng m_sFun;
//    Strng m_sValue;
//  };
class CExcelReport
  {
  public:
    CExcelReport(COleReportMngr* Mngr, OWorkbook* WkBook);
    ~CExcelReport();
    flag CheckParms();
    int ParseFn(char* Func);
    int ParseOffsetFn(char* Func, OWorksheet* pWkSheet, int& Row, int& Col);
    flag DoReport();
    BOOL GetAutoTags(OWorksheet* pSheet, int Row1, int Col1);

    class COrdValue
      {
      public:
        COrdValue()
          {
          m_cType   = tt_NULL;
          m_bAscend = true;
          m_lValue  = 0;
          m_dValue  = 0;
          }
        COrdValue(const COrdValue & Other)
          {
          m_cType   = Other.m_cType  ;
          m_bAscend = Other.m_bAscend;
          m_lValue  = Other.m_lValue ;
          m_dValue  = Other.m_dValue ;
          m_sValue  = Other.m_sValue ;
          }
        COrdValue & operator=(const COrdValue & Other)
          {
          m_cType   = Other.m_cType  ;
          m_bAscend = Other.m_bAscend;
          m_lValue  = Other.m_lValue ;
          m_dValue  = Other.m_dValue ;
          m_sValue  = Other.m_sValue ;
          return * this;
          }

      public:
        byte    m_cType;
        bool    m_bAscend;
        long    m_lValue;
        double  m_dValue;
        Strng   m_sValue;
      };

    class COrdTerm
      {
      public:
        COrdTerm() { };
        COrdTerm(LPTSTR Field, LPTSTR Fn)
          {
          m_sField = Field;
          m_sFn    = Fn;
          } 
        Strng   m_sField; 
        Strng   m_sFn; 
      };

    class CRqdTag 
      {
      public:
        CRqdTag(LPTSTR Tag) : m_sTag(Tag) {} 
        
        BOOL AddOrderValue(LPTSTR Field, LPTSTR Fn, CExcelReport &Rpt, COleReportMngr & Mngr);

      public:
        Strng   m_sTag; 
        CArray<COrdValue,COrdValue&> m_OrdValues;
      };

    class CTagGraphics
      {
      public:
        CTagGraphics(LPCTSTR Tag) : m_sTag(Tag) {} 
        
        void AddGraphic(LPCTSTR GrfTitle) { m_GrfTitles.Add(CString(GrfTitle)); };

      public:
        CString m_sTag; 
        CArray<CString, CString&> m_GrfTitles;
      };

  protected:
    long        GetTags(CSVector& Tags, CCellLocation& Loc, short Len, short Wide, short Skip, flag Vert);
    BOOL        TagCmpOK(LPTSTR Tag, LPTSTR Field, LPTSTR Cmp, LPTSTR Value);
    void        ClearSecondary(OWorksheet* pSheet, ORange & Range, int Row1, int Col1);
    void        GetTagPages(CModelTypeListArray& List);

  public:
    Strng sName;          //report name
    CCellLocation ResLoc; //result location
    CCellLocation PriLoc; //primary tags location
    short iPriLen;        //
    short iPriMaxLen;     //
    CCellLocation SecLoc; //secendary tags location
    short iSecLen;        //
    short iSecMaxLen;     //
    short iSecMult;       // Number of column/rows for multi report
    Strng sNan;           //string to use for NAN
    flag bResVert:1,      //
         bPriVert:1,      //
         bSecVert:1,      //
         bIsTagList:1,    //busy generating TagList type report
         bIsTagOffsetList:1, //
         bIsAutoTags:1;      //
                    
    //CArray<CExcelTagValue CExcelTagValue&> m_Select;
    //CArray<CExcelTagValue CExcelTagValue&> m_Order;

    Strng       m_sSelect;
    Strng       m_sOrder;
    CArray <COrdTerm, COrdTerm&> m_OrderTerms;

    CArray<CTagGraphics*, CTagGraphics*> m_TGs;
    CMap<LPCTSTR, LPCTSTR, CTagGraphics*, CTagGraphics*> m_TGMap;

    CSVector PriTags;     //array of primary tags
    CSVector SecTags;     //array of secondary tags
    COleReportMngr* pMngr;//
    OWorkbook* pWkBook;   //
    long iTagFoundCnt;    //

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
    CTimeValue m_dTime;
    //CDVector Values;
    CArray <COleVariant,COleVariant> Values;
    CRepTrndItem* pNxt;

    CRepTrndItem(CTimeValue Time, int nPts);
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
    CTimeValue dStartTime;    //
    CTimeValue dEndTime;      //
    CTimeValue dDuration;     //
    CTimeValue dReportTimeOffset;
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

