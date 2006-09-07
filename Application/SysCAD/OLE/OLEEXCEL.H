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
      bMakeActive,     //make Excel active and bring to front always
      bUsingActive,
      bUpdateLinks,    //update linked files, etc when opening
      bSaveOnComplete, //save workbook on completion
      bForceNormalView;//force all windows to normal view as PageBreakPreview slows report too much

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
class CExcelReport; // _FWDDEF(CExcelReport)
class CXLRptFn;

//--------------------------------------------------------------------------

class CXLRptFnValue
  {
  public:
    CXLRptFnValue()           { m_cType = tt_NULL;   m_lValue=0; m_dValue=0; m_sValue="";   }
    CXLRptFnValue(long   L)   { m_cType = tt_Long;   m_lValue=L; m_dValue=0; m_sValue="";   }
    CXLRptFnValue(double D)   { m_cType = tt_Double; m_lValue=0; m_dValue=D; m_sValue="";   }
    CXLRptFnValue(LPTSTR S)   { m_cType = tt_Strng;  m_lValue=0; m_dValue=0; m_sValue=S;    }
    CXLRptFnValue(const CXLRptFnValue& Other)   
      { 
      m_cType   = Other.m_cType; 
      m_lValue  = Other.m_lValue;
      m_dValue  = Other.m_dValue;
      m_sValue  = Other.m_sValue;
      };
    CXLRptFnValue & operator=(const CXLRptFnValue& Other)   
      { 
      m_cType   = Other.m_cType; 
      m_lValue  = Other.m_lValue;
      m_dValue  = Other.m_dValue;
      m_sValue  = Other.m_sValue;
      return *this;
      };
    void Clear()
      {
      m_cType   = tt_NULL;
      m_lValue  = 0;
      m_dValue  = 0;
      m_sValue  = "";
      }
    operator bool () const   
      { 
      switch (m_cType)
        {
        case tt_Long:   return m_lValue!=0;
        case tt_Double: return m_dValue!=0;
        case tt_Strng:  return m_sValue.GetLength()>0; break;
        }
      return false;
      };
    operator long () const
      { 
      switch (m_cType)
        {
        case tt_Long:   return m_lValue;
        case tt_Double: return (long)Range((double)INT_MIN, m_dValue, (double)INT_MAX);
        case tt_Strng:  return SafeAtoL(m_sValue()); 
        }
      return 0;
      };
    operator double () const   
      { 
      switch (m_cType)
        {
        case tt_Long:   return m_lValue;
        case tt_Double: return m_dValue;
        case tt_Strng:  return SafeAtoF(m_sValue()); 
        }
      return 0;
      };
    operator Strng() const   
      { 
      Strng S;
      switch (m_cType)
        {
        case tt_Long:   S.Set("%i", m_lValue); break;
        case tt_Double: S.Set("%.20g", m_dValue); break;
        case tt_Strng:  S=m_sValue; 
        }
      return S;
      };
    CString String() const   
      { 
      CString S;
      switch (m_cType)
        {
        case tt_Long:   S.Format("%i", m_lValue); break;
        case tt_Double: S.Format("%.20g", m_dValue); break;
        case tt_Strng:  S=m_sValue(); 
        }
      return S;
      };
    CXLRptFnValue & operator=(const long L)   
      { 
      m_cType   = tt_Long; 
      m_lValue  = L;
      return *this;
      };
    CXLRptFnValue & operator=(const int L)   
      { 
      m_cType   = tt_Long; 
      m_lValue  = L;
      return *this;
      };
    CXLRptFnValue & operator=(const double D)   
      { 
      m_cType   = tt_Double; 
      m_dValue  = D;
      return *this;
      };
    CXLRptFnValue & operator=(const LPCTSTR S)   
      { 
      m_cType   = tt_Strng; 
      m_sValue  = S;
      return *this;
      };
    CXLRptFnValue operator + (const CXLRptFnValue & Other)   
      { 
      CXLRptFnValue X(*this);
      switch (m_cType)
        {
        case tt_Long:   X.m_lValue  += Other.m_lValue; break;
        case tt_Double: X.m_dValue  += Other.m_dValue; break;
        case tt_Strng:  X.m_sValue  += Other.m_sValue; break;
        }
      return X;
      };
    CXLRptFnValue & operator += (const CXLRptFnValue & Other)   
      { 
      switch (m_cType)
        {
        case tt_Long:   m_lValue  += Other.m_lValue; break;
        case tt_Double: m_dValue  += Other.m_dValue; break;
        case tt_Strng:  m_sValue  += Other.m_sValue; break;
        }
      return *this;
      };
    CXLRptFnValue operator - (const CXLRptFnValue & Other)   
      { 
      CXLRptFnValue X(*this);
      switch (m_cType)
        {
        case tt_Long:   X.m_lValue  -= Other.m_lValue; break;
        case tt_Double: X.m_dValue  -= Other.m_dValue; break;
        //case tt_Strng:  X.m_sValue  -= Other.m_sValue; break;
        }
      return X;
      };
    CXLRptFnValue & operator -= (const CXLRptFnValue & Other)   
      { 
      switch (m_cType)
        {
        case tt_Long:   m_lValue  -= Other.m_lValue; break;
        case tt_Double: m_dValue  -= Other.m_dValue; break;
        //case tt_Strng:  m_sValue  -= Other.m_sValue; break;
        }
      return *this;
      };
    CXLRptFnValue operator * (const CXLRptFnValue & Other)   
      { 
      CXLRptFnValue X(*this);
      switch (m_cType)
        {
        case tt_Long:   X.m_lValue  *= Other.m_lValue; break;
        case tt_Double: X.m_dValue  *= Other.m_dValue; break;
        //case tt_Strng:  X.m_sValue  *= Other.m_sValue; break;
        }
      return X;
      };
    CXLRptFnValue & operator *= (const CXLRptFnValue & Other)   
      { 
      switch (m_cType)
        {
        case tt_Long:   m_lValue  *= Other.m_lValue; break;
        case tt_Double: m_dValue  *= Other.m_dValue; break;
        //case tt_Strng:  m_sValue  *= Other.m_sValue; break;
        }
      return *this;
      };
    CXLRptFnValue operator / (const CXLRptFnValue & Other)   
      { 
      CXLRptFnValue X(*this);
      switch (m_cType)
        {
        case tt_Long:   X.m_lValue  /= NZ(Other.m_lValue); break;
        case tt_Double: X.m_dValue  /= NZ(Other.m_dValue); break;
        //case tt_Strng:  X.m_sValue  ="//"/= Other.m_sValue; break;
        }
      return X;
      };
    CXLRptFnValue & operator /= (const CXLRptFnValue & Other)   
      { 
      switch (m_cType)
        {
        case tt_Long:   m_lValue  /= NZ(Other.m_lValue); break;
        case tt_Double: m_dValue  /= NZ(Other.m_dValue); break;
        //case tt_Strng:  m_sValue  /= Other.m_sValue; break;
        }
      return *this;
      };
    bool operator < (const CXLRptFnValue & B)   
      { 
      switch (m_cType)
        {
        case tt_Long:   return m_lValue < (long)B;  
        case tt_Double: { double O=(double)B; return Valid(m_dValue) && Valid(O) ? m_dValue < O : false; }
        case tt_Strng:  return stricmp(m_sValue(), ((Strng)B)())<0;
        }
      return false;
      };
    bool operator <= (const CXLRptFnValue & B)   
      { 
      switch (m_cType)
        {
        case tt_Long:   return m_lValue <= (long)B; 
        case tt_Double: { double O=(double)B; return Valid(m_dValue) && Valid(O) ? m_dValue <= O : false; }
        case tt_Strng:  return stricmp(m_sValue(), ((Strng)B)())<=0;
        }
      return false;
      };
    bool operator == (const CXLRptFnValue & B)   
      { 
      switch (m_cType)
        {
        case tt_Long:   return m_lValue == (long)B;  
        case tt_Double: { double O=(double)B; return Valid(m_dValue) && Valid(O) ? m_dValue == O : Valid(m_dValue) == Valid(O); }
        case tt_Strng:  return stricmp(m_sValue(), ((Strng)B)())==0;
        }
      return false;
      };
    bool operator != (const CXLRptFnValue & B)   
      { 
      switch (m_cType)
        {
        case tt_Long:   return m_lValue != (long)B;  
        case tt_Double: { double O=(double)B; return Valid(m_dValue) && Valid(O) ? m_dValue != O : Valid(m_dValue) != Valid(O); }
        case tt_Strng:  return stricmp(m_sValue(), ((Strng)B)())!=0;
        }
      return false;
      };
    bool operator >= (const CXLRptFnValue & B)   
      { 
      switch (m_cType)
        {
        case tt_Long:   return m_lValue >= (long)B; 
        case tt_Double: { double O=(double)B; return Valid(m_dValue) && Valid(O) ? m_dValue >= O : false; }
        case tt_Strng:  return stricmp(m_sValue(), ((Strng)B)())>=0;
        }
      return false;
      };
    bool operator > (const CXLRptFnValue & B)   
      { 
      switch (m_cType)
        {
        case tt_Long:   return m_lValue > (long)B;  
        case tt_Double: { double O=(double)B; return Valid(m_dValue) && Valid(O) ? m_dValue > O : false; }
        case tt_Strng:  return stricmp(m_sValue(), ((Strng)B)())>0;
        }
      return false;
      };
    bool operator && (const CXLRptFnValue & B)   
      { 
      switch (m_cType)
        {
        case tt_Long:   return m_lValue && (long)B; 
        case tt_Double: return m_dValue!=0 && ((double)B)!=0; 
        //case tt_Strng:  return stricmp(m_sValue(), ((Strng)B)())>0;
        }
      return false;
      };
    bool operator || (const CXLRptFnValue & B)   
      { 
      switch (m_cType)
        {
        case tt_Long:   return m_lValue || (long)B; 
        case tt_Double: return m_dValue!=0 || ((double)B)!=0; 
        //case tt_Strng:  return stricmp(m_sValue(), ((Strng)B)())>0;
        }
      return false;
      };

  public:
    byte      m_cType;
    long      m_lValue;
    double    m_dValue;
    Strng     m_sValue;
  };

class CXLOrdValue 
  {
  public:
    CXLOrdValue()
      {
      m_bAscend = true;
      }
    CXLOrdValue(const CXLOrdValue & Other)
      {
      m_Value   = Other.m_Value;
      m_bAscend = Other.m_bAscend;
      }
    CXLOrdValue & operator=(const CXLOrdValue & Other)
      {
      m_Value   = Other.m_Value;
      m_bAscend = Other.m_bAscend;
      return * this;
      }

  public:
    bool    m_bAscend;
    CXLRptFnValue m_Value;
  };

class CXLOrdTerm
  {
  public:
    CXLOrdTerm() { };
    CXLOrdTerm(LPTSTR Field, LPTSTR Fn)
      {
      m_sField = Field;
      m_sFn    = Fn;
      } 
    Strng   m_sField; 
    Strng   m_sFn; 
  };

class CXLRqdTag 
  {
  public:
    CXLRqdTag(LPTSTR Tag) : m_sTag(Tag) {} 

    BOOL AddOrderValue(LPTSTR Field, LPTSTR Fn, CExcelReport &Rpt, COleReportMngr & Mngr);

  public:
    Strng   m_sTag; 
    CArray<CXLOrdValue,CXLOrdValue&> m_OrdValues;
  };

class CXLTagGraphics
  {
  public:
    CXLTagGraphics(LPCTSTR Tag) : m_sTag(Tag) {} 

    void AddGraphic(LPCTSTR GrfTitle) { m_GrfTitles.Add(CString(GrfTitle)); };

  public:
    CString m_sTag; 
    CArray<CString, CString&> m_GrfTitles;
  };

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

  protected:
    long        GetTags(CSVector& Tags, CCellLocation& Loc, short Len, short Wide, short Skip, flag Vert);
    void        ClearSecondary(OWorksheet* pSheet, ORange & Range, int Row1, int Col1);
    void        GetTagPages(CModelTypeListArray& List);

  public:
    Strng               sName;                  //report name
    CCellLocation       ResLoc;                 //result location
    CCellLocation       PriLoc;                 //primary tags location
    short               iPriLen;                //
    short               iPriMaxLen;             //
    CCellLocation       SecLoc;                 //secendary tags location
    short               iSecLen;                //
    short               iSecMaxLen;             //
    short               iSecMult;               // Number of column/rows for multi report
    Strng               sNan;                   //string to use for NAN
    flag                bResVert,               //
                        bPriVert,               //
                        bSecVert,               //
                        bIsTagList,             //busy generating TagList type report
                        bIsTagOffsetList,       //
                        bIsAutoTags,
                        m_bIgnoreMissingField,
                        m_bIgnoreMissingSelect,
                        m_bIgnoreMissingOrder;     

    CXLRptFn          & m_Select;
    Strng               m_sOrder;
    CArray <CXLOrdTerm, CXLOrdTerm&> m_OrderTerms;

    CArray<CXLTagGraphics*, CXLTagGraphics*> m_TGs;
    CMap<LPCTSTR, LPCTSTR, CXLTagGraphics*, CXLTagGraphics*> m_TGMap;

    CSVector            PriTags;                //array of primary tags
    CSVector            SecTags;                //array of secondary tags
    COleReportMngr    * pMngr;
    OWorkbook         * pWkBook;
    long                iTagFoundCnt;

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

