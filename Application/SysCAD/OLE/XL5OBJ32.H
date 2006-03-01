//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __XL5OBJ32_H
#define __XL5OBJ32_H

const long xlNormalView = 1;
const long xlPageBreakPreview = 2;
const long xlCalcAutomatic = -4105;
const long xlCalcManual = -4135;

//===========================================================================
// Excel Application wrapper class
class OExcelApplication : public COleDispatchDriver
  {
  // Attributes
  public:
    void SetDefaultFilePath(LPCTSTR lpszNewValue);
    void SetScreenUpdating(BOOL bUpdating);
    void SetStatusBar(LPCTSTR pStatusBarTxt);
    void RestoreStatusBar();
    BOOL GetVisible();
    void SetVisible(BOOL bVisible = TRUE);
  // Operations
  public:
    LPDISPATCH GetActiveSheet();
  	LPDISPATCH GetActiveWindow();
	  long GetCalculation();
	  void SetCalculation(long nNewValue);
    void Quit();
    LPDISPATCH Workbooks(short iIndex);  // syntax 1 (see "VBA_XL.HLP" for more info)
    LPDISPATCH Workbooks(char* pName);   // syntax 1 (see "VBA_XL.HLP" for more info)
    LPDISPATCH Workbooks();              // syntax 2 (see "VBA_XL.HLP" for more info)
    LPDISPATCH Worksheets(char* pName);  // syntax 1 (see "VBA_XL.HLP" for more info)
    LPDISPATCH Worksheets(short iIndex); // syntax 1 (see "VBA_XL.HLP" for more info)
    LPDISPATCH Worksheets();             // syntax 2 (see "VBA_XL.HLP" for more info)
    VARIANT Run(const VARIANT& Macro);//, const VARIANT& Arg1, const VARIANT& Arg2, const VARIANT& Arg3, const VARIANT& Arg4, const VARIANT& Arg5, const VARIANT& Arg6, const VARIANT& Arg7, const VARIANT& Arg8, const VARIANT& Arg9, 
    //const VARIANT& Arg10, const VARIANT& Arg11, const VARIANT& Arg12, const VARIANT& Arg13, const VARIANT& Arg14, const VARIANT& Arg15, const VARIANT& Arg16, const VARIANT& Arg17, const VARIANT& Arg18, const VARIANT& Arg19, 
    //const VARIANT& Arg20, const VARIANT& Arg21, const VARIANT& Arg22, const VARIANT& Arg23, const VARIANT& Arg24, const VARIANT& Arg25, const VARIANT& Arg26, const VARIANT& Arg27, const VARIANT& Arg28, const VARIANT& Arg29, 
    //const VARIANT& Arg30);
  };

//===========================================================================
// Range wrapper class
class ORange : public COleDispatchDriver
  {
  public:
    ORange() {} // Calls COleDispatchDriver default constructor
    ORange(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
    ORange(const ORange& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}
  // Attributes
  public:
    short GetColumn();
    void SetColumnWidth(short Width);
    LPDISPATCH GetFont();
    VARIANT GetFormula();
    void SetNumberFormat(char* pFormat);
    short GetRow();
    VARIANT GetValue();
    short GetShortValue();
    long GetLongValue();
    double GetDoubleValue();
    CString GetStringValue();
    void SetValue(char* pTxt);
    void SetValue(double Val);
    void SetValue(const VARIANT&);
  // Operations
  public:
    VARIANT Activate();
    VARIANT Clear();
    LPDISPATCH Find(char* pWhat);
    //VARIANT Find(const VARIANT& What, const VARIANT& After, const VARIANT& LookIn, const VARIANT& LookAt, const VARIANT& SearchOrder, const VARIANT& SearchDirection, const VARIANT& MatchCase);
    LPDISPATCH FindNext(LPDISPATCH lpDispatchAfter);
    LPDISPATCH FindNext(const VARIANT& After);
  };

//===========================================================================
// Workbook wrapper class
class OWorkbook : public COleDispatchDriver
  {
  public:
    OWorkbook() {} // Calls COleDispatchDriver default constructor
    OWorkbook(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
    OWorkbook(const OWorkbook& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

  // Attributes
  public:
    LPDISPATCH GetActiveChart();
    LPDISPATCH GetActiveSheet();
    CString GetFullName();
    CString GetName();
    BOOL GetReadOnly();
    BOOL GetSaved();
    void SetSaved(BOOL bSaved);
  // Operations
  public:
    VARIANT Activate();
    LPDISPATCH Charts(short iIndex);
    LPDISPATCH Charts();
    void Close(BOOL bSaveChanges);
    VARIANT Save();
    LPDISPATCH Windows(short iIndex);
    LPDISPATCH Windows();
    LPDISPATCH Worksheets(char* pName);
    LPDISPATCH Worksheets(short iIndex);
    LPDISPATCH Worksheets();
  };

//===========================================================================
// Workbooks wrapper class
class OWorkbooks : public COleDispatchDriver
  {
  public:
    OWorkbooks() {}
    OWorkbooks(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
    OWorkbooks(const OWorkbooks& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}
  // Attributes
  public:
    short GetCount();
  // Operations
  public:
    LPDISPATCH Open(char* pFilename, BOOL bIgnoreReadOnlyRecommended, int iUpdateLinks = 0);
  };

//===========================================================================
// Worksheet wrapper class

class OWorksheet : public COleDispatchDriver
  {
  public:
    OWorksheet() {} // Calls COleDispatchDriver default constructor
    OWorksheet(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
    OWorksheet(const OWorksheet& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

  // Attributes
  public:
    CString GetName();
    void SetName(char* pName);
    BOOL GetProtectContents();
  // Operations
  public:
    VARIANT Activate();
    LPDISPATCH Cells(short iRowIndex, short iColumnIndex);
    LPDISPATCH Columns();
    LPDISPATCH Columns(short iColumnIndex);
    LPDISPATCH Range(short iRowIndex1, short iColumnIndex1, short iRowIndex2, short iColumnIndex2);
  };

//===========================================================================
// Worksheets wrapper class
class OWorksheets : public COleDispatchDriver
  {
  public:
    OWorksheets() {} // Calls COleDispatchDriver default constructor
    OWorksheets(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
    OWorksheets(const OWorksheets& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}
  // Attributes
  public:
    short GetCount();
  // Operations
  public:
    VARIANT Add();
  };

//===========================================================================
// Font wrapper class

class OFont : public COleDispatchDriver
  {
  public:
    OFont() {} // Calls COleDispatchDriver default constructor
    OFont(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
    OFont(const OFont& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

  // Attributes
  public:
    void SetBold(BOOL bBold);
    void SetColor(COLORREF Color);
  // Operations
  public:
    //VARIANT Application();
  };

//===========================================================================
// Chart wrapper class

class OChart : public COleDispatchDriver
  {
  public:
  OChart() {} // Calls COleDispatchDriver default constructor
  OChart(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
  OChart(const OChart& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

  // Attributes
  public:
    CString GetName();
    void SetName(char* pName);
    void SetType(short Type);

  // Operations
  public:
    LPDISPATCH SeriesCollection(short iIndex);
    LPDISPATCH SeriesCollection();
  };

//===========================================================================
// Charts wrapper class

class OCharts : public COleDispatchDriver
  {
  public:
    OCharts() {} // Calls COleDispatchDriver default constructor
    OCharts(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
    OCharts(const OCharts& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

  // Attributes
  public:
    short GetCount();

  // Operations
  public:
    VARIANT Add();
  };

//===========================================================================
// Series wrapper class

class OSeries : public COleDispatchDriver
  {
  public:
    OSeries() {} // Calls COleDispatchDriver default constructor
    OSeries(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
    OSeries(const OSeries& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

  // Attributes
  public:
    void SetValues(LPDISPATCH pDispatch);
    void SetXValues(LPDISPATCH pDispatch);

  // Operations
  public:
    VARIANT Delete();
    LPDISPATCH Points();
  };

//===========================================================================
// SeriesCollection wrapper class

class OSeriesCollection : public COleDispatchDriver
  {
  public:
  OSeriesCollection() {} // Calls COleDispatchDriver default constructor
  OSeriesCollection(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
  OSeriesCollection(const OSeriesCollection& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

  // Attributes
  public:
    short GetCount();

  // Operations
  public:
    VARIANT Add(LPDISPATCH pDispatch);
  };

//===========================================================================
// Window wrapper class

class OWindow : public COleDispatchDriver
{
  public:
    OWindow() {}		// Calls COleDispatchDriver default constructor
    OWindow(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
    OWindow(const OWindow& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

  // Attributes
  public:

  // Operations
  public:
    long GetView();
    void SetView(long nNewValue);
  };

//===========================================================================
// Windows wrapper class

class OWindows : public COleDispatchDriver
  {
  public:
    OWindows() {}		// Calls COleDispatchDriver default constructor
    OWindows(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
    OWindows(const OWindows& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

  // Attributes
  public:

  // Operations
  public:
    short GetCount();
  };
//===========================================================================
#endif

