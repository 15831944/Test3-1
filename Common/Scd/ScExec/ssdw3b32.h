// Machine generated IDispatch wrapper class(es) created with ClassWizard
/////////////////////////////////////////////////////////////////////////////
// ISSColumns wrapper class

class ISSColumns : public COleDispatchDriver
{
public:
	ISSColumns() {}		// Calls COleDispatchDriver default constructor
	ISSColumns(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	ISSColumns(const ISSColumns& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:
	short GetCount();
	void SetCount(short);

// Operations
public:
	void Remove(const VARIANT& Index);
	void Add(short Index);
	void RemoveAll();
	LPUNKNOWN _NewEnum();
	LPDISPATCH GetItem(const VARIANT& Index);
	void SetItem(const VARIANT& Index, LPDISPATCH newValue);
};
/////////////////////////////////////////////////////////////////////////////
// ISSColumn wrapper class

class ISSColumn : public COleDispatchDriver
{
public:
	ISSColumn() {}		// Calls COleDispatchDriver default constructor
	ISSColumn(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	ISSColumn(const ISSColumn& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:
	CString GetCaption();
	void SetCaption(LPCTSTR);
	long GetAlignment();
	void SetAlignment(long);
	long GetForeColor();
	void SetForeColor(long);
	long GetBackColor();
	void SetBackColor(long);
	long GetCase();
	void SetCase(long);
	CString GetNumberFormat();
	void SetNumberFormat(LPCTSTR);
	short GetFieldLen();
	void SetFieldLen(short);
	BOOL GetAllowSizing();
	void SetAllowSizing(BOOL);
	long GetHeadForeColor();
	void SetHeadForeColor(long);
	long GetHeadBackColor();
	void SetHeadBackColor(long);
	CString GetDataField();
	void SetDataField(LPCTSTR);
	float GetLeft();
	void SetLeft(float);
	float GetTop();
	void SetTop(float);
	BOOL GetLocked();
	void SetLocked(BOOL);
	CString GetText();
	void SetText(LPCTSTR);
	VARIANT GetValue();
	void SetValue(const VARIANT&);
	BOOL GetVisible();
	void SetVisible(BOOL);
	float GetWidth();
	void SetWidth(float);
	short GetDataType();
	void SetDataType(short);
	BOOL GetSelected();
	void SetSelected(BOOL);
	BOOL GetButtonsAlways();
	void SetButtonsAlways(BOOL);
	short GetLevel();
	void SetLevel(short);
	long GetStyle();
	void SetStyle(long);
	BOOL GetHasHeadForeColor();
	void SetHasHeadForeColor(BOOL);
	BOOL GetHasHeadBackColor();
	void SetHasHeadBackColor(BOOL);
	BOOL GetHasForeColor();
	void SetHasForeColor(BOOL);
	BOOL GetHasBackColor();
	void SetHasBackColor(BOOL);
	BOOL GetVertScrollBar();
	void SetVertScrollBar(BOOL);
	VARIANT GetTagVariant();
	void SetTagVariant(const VARIANT&);
	short GetListCount();
	void SetListCount(short);
	long GetDropDownHwnd();
	void SetDropDownHwnd(long);
	CString GetHeadStyleSet();
	void SetHeadStyleSet(LPCTSTR);
	CString GetStyleSet();
	void SetStyleSet(LPCTSTR);
	short GetPosition();
	void SetPosition(short);
	short GetGroup();
	void SetGroup(short);
	long GetCaptionAlignment();
	void SetCaptionAlignment(long);
	BOOL GetColChanged();
	void SetColChanged(BOOL);
	CString GetName();
	void SetName(LPCTSTR);
	short GetListIndex();
	void SetListIndex(short);
	long GetNullable();
	void SetNullable(long);
	CString GetMask();
	void SetMask(LPCTSTR);
	BOOL GetPromptInclude();
	void SetPromptInclude(BOOL);
	long GetClipMode();
	void SetClipMode(long);
	CString GetPromptChar();
	void SetPromptChar(LPCTSTR);

// Operations
public:
	CString CellText(const VARIANT& Bookmark);
	VARIANT CellValue(const VARIANT& Bookmark);
	void AddItem(LPCTSTR Item, const VARIANT& Index);
	void RemoveItem(short Index);
	void RemoveAll();
	CString GetList(short Index);
	void SetList(short Index, LPCTSTR lpszNewValue);
	void CellStyleSet(LPCTSTR StyleSet, const VARIANT& RowNum);
	BOOL IsCellValid();
	long GetItemData(short Index);
	void SetItemData(short Index, long nNewValue);
};
/////////////////////////////////////////////////////////////////////////////
// ISSGroupColumns wrapper class

class ISSGroupColumns : public COleDispatchDriver
{
public:
	ISSGroupColumns() {}		// Calls COleDispatchDriver default constructor
	ISSGroupColumns(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	ISSGroupColumns(const ISSGroupColumns& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:
	short GetCount();
	void SetCount(short);

// Operations
public:
	void Remove(const VARIANT& Index);
	void Add(short Index);
	void RemoveAll();
	LPUNKNOWN _NewEnum();
	LPDISPATCH GetItem(const VARIANT& Index);
	void SetItem(const VARIANT& Index, LPDISPATCH newValue);
};
/////////////////////////////////////////////////////////////////////////////
// ISSGroups wrapper class

class ISSGroups : public COleDispatchDriver
{
public:
	ISSGroups() {}		// Calls COleDispatchDriver default constructor
	ISSGroups(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	ISSGroups(const ISSGroups& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:
	short GetCount();
	void SetCount(short);

// Operations
public:
	void Remove(const VARIANT& Index);
	void Add(short Index);
	void RemoveAll();
	LPUNKNOWN _NewEnum();
	LPDISPATCH GetItem(const VARIANT& Index);
	void SetItem(const VARIANT& Index, LPDISPATCH newValue);
};
/////////////////////////////////////////////////////////////////////////////
// ISSGroup wrapper class

class ISSGroup : public COleDispatchDriver
{
public:
	ISSGroup() {}		// Calls COleDispatchDriver default constructor
	ISSGroup(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	ISSGroup(const ISSGroup& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:
	CString GetCaption();
	void SetCaption(LPCTSTR);
	long GetCaptionAlignment();
	void SetCaptionAlignment(long);
	BOOL GetAllowSizing();
	void SetAllowSizing(BOOL);
	float GetLeft();
	void SetLeft(float);
	float GetTop();
	void SetTop(float);
	BOOL GetVisible();
	void SetVisible(BOOL);
	float GetWidth();
	void SetWidth(float);
	BOOL GetSelected();
	void SetSelected(BOOL);
	long GetHeadForeColor();
	void SetHeadForeColor(long);
	long GetHeadBackColor();
	void SetHeadBackColor(long);
	BOOL GetHasHeadForeColor();
	void SetHasHeadForeColor(BOOL);
	BOOL GetHasHeadBackColor();
	void SetHasHeadBackColor(BOOL);
	VARIANT GetTagVariant();
	void SetTagVariant(const VARIANT&);
	CString GetHeadStyleSet();
	void SetHeadStyleSet(LPCTSTR);
	CString GetStyleSet();
	void SetStyleSet(LPCTSTR);
	short GetPosition();
	void SetPosition(short);
	LPDISPATCH GetColumns();
	void SetColumns(LPDISPATCH);

// Operations
public:
	short ColPosition(short ColPos);
};
/////////////////////////////////////////////////////////////////////////////
// ISSActiveCell wrapper class

class ISSActiveCell : public COleDispatchDriver
{
public:
	ISSActiveCell() {}		// Calls COleDispatchDriver default constructor
	ISSActiveCell(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	ISSActiveCell(const ISSActiveCell& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:
	long GetTop();
	void SetTop(long);
	long GetLeft();
	void SetLeft(long);
	long GetHeight();
	void SetHeight(long);
	long GetWidth();
	void SetWidth(long);
	CString GetStyleSet();
	void SetStyleSet(LPCTSTR);
	CString GetText();
	void SetText(LPCTSTR);
	VARIANT GetValue();
	void SetValue(const VARIANT&);
	CString GetSelText();
	void SetSelText(LPCTSTR);
	short GetSelLength();
	void SetSelLength(short);
	short GetSelStart();
	void SetSelStart(short);

// Operations
public:
};
/////////////////////////////////////////////////////////////////////////////
// ISSSelBookmarks wrapper class

class ISSSelBookmarks : public COleDispatchDriver
{
public:
	ISSSelBookmarks() {}		// Calls COleDispatchDriver default constructor
	ISSSelBookmarks(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	ISSSelBookmarks(const ISSSelBookmarks& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:
	long GetCount();
	void SetCount(long);

// Operations
public:
	void Add(const VARIANT& Bookmark);
	void Remove(const VARIANT& Index);
	void RemoveAll();
	LPUNKNOWN _NewEnum();
	VARIANT GetItem(const VARIANT& Index);
	void SetItem(const VARIANT& Index, const VARIANT& newValue);
};
/////////////////////////////////////////////////////////////////////////////
// ISSStyleSets wrapper class

class ISSStyleSets : public COleDispatchDriver
{
public:
	ISSStyleSets() {}		// Calls COleDispatchDriver default constructor
	ISSStyleSets(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	ISSStyleSets(const ISSStyleSets& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:
	short GetCount();
	void SetCount(short);

// Operations
public:
	void Add(LPCTSTR Name);
	void Remove(const VARIANT& Index);
	void RemoveAll();
	LPDISPATCH GetItem(const VARIANT& Index);
	void SetItem(const VARIANT& Index, LPDISPATCH newValue);
	LPUNKNOWN _NewEnum();
};
/////////////////////////////////////////////////////////////////////////////
// ISSStyleSet wrapper class

class ISSStyleSet : public COleDispatchDriver
{
public:
	ISSStyleSet() {}		// Calls COleDispatchDriver default constructor
	ISSStyleSet(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	ISSStyleSet(const ISSStyleSet& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:
	float GetPictureMetaHeight();
	void SetPictureMetaHeight(float);
	float GetPictureMetaWidth();
	void SetPictureMetaWidth(float);
	VARIANT GetForeColor();
	void SetForeColor(const VARIANT&);
	VARIANT GetBackColor();
	void SetBackColor(const VARIANT&);
	VARIANT GetPicture();
	void SetPicture(const VARIANT&);
	VARIANT GetAlignmentPicture();
	void SetAlignmentPicture(const VARIANT&);
	VARIANT GetAlignmentText();
	void SetAlignmentText(const VARIANT&);
	VARIANT GetFont();
	void SetFont(const VARIANT&);
	CString GetName();
	void SetName(LPCTSTR);

// Operations
public:
	void Reset();
};
/////////////////////////////////////////////////////////////////////////////
// ISSPrintInfo wrapper class

class ISSPrintInfo : public COleDispatchDriver
{
public:
	ISSPrintInfo() {}		// Calls COleDispatchDriver default constructor
	ISSPrintInfo(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	ISSPrintInfo(const ISSPrintInfo& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:
	CString GetPageHeader();
	void SetPageHeader(LPCTSTR);
	CString GetPageFooter();
	void SetPageFooter(LPCTSTR);
	short GetPageStart();
	void SetPageStart(short);
	short GetPageEnd();
	void SetPageEnd(short);
	short GetCopies();
	void SetCopies(short);
	long GetPrintGridLines();
	void SetPrintGridLines(long);
	float GetMarginTop();
	void SetMarginTop(float);
	float GetMarginBottom();
	void SetMarginBottom(float);
	float GetMarginLeft();
	void SetMarginLeft(float);
	float GetMarginRight();
	void SetMarginRight(float);
	BOOL GetCollate();
	void SetCollate(BOOL);
	BOOL GetPortrait();
	void SetPortrait(BOOL);
	long GetPrintColumnHeaders();
	void SetPrintColumnHeaders(long);
	BOOL GetPrintGroupHeaders();
	void SetPrintGroupHeaders(BOOL);
	long GetPrintHeaders();
	void SetPrintHeaders(long);
	long GetPageBreakOnGroups();
	void SetPageBreakOnGroups(long);
	BOOL GetRowAutoSize();
	void SetRowAutoSize(BOOL);
	short GetMaxLinesPerRow();
	void SetMaxLinesPerRow(short);
	BOOL GetPrintColors();
	void SetPrintColors(BOOL);
	CString GetPrinterDeviceName();
	void SetPrinterDeviceName(LPCTSTR);
	short GetPrinterDriverVer();
	void SetPrinterDriverVer(short);
	long GetDriverOverride();
	void SetDriverOverride(long);
	long GetClippingOverride();
	void SetClippingOverride(long);

// Operations
public:
};
/////////////////////////////////////////////////////////////////////////////
// ISSRowBuffer wrapper class

class ISSRowBuffer : public COleDispatchDriver
{
public:
	ISSRowBuffer() {}		// Calls COleDispatchDriver default constructor
	ISSRowBuffer(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	ISSRowBuffer(const ISSRowBuffer& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	long GetRowCount();
	void SetRowCount(long nNewValue);
	short GetColumnCount();
	void SetColumnCount(short nNewValue);
	VARIANT GetBookmark(long Row);
	void SetBookmark(long Row, const VARIANT& newValue);
	CString GetColumnName(short Col);
	void SetColumnName(short Col, LPCTSTR lpszNewValue);
	VARIANT GetValue(long Row, short Col);
	void SetValue(long Row, short Col, const VARIANT& newValue);
	short GetReadType();
	void SetReadType(short nNewValue);
};
/////////////////////////////////////////////////////////////////////////////
// RowBufferDisp wrapper class

class RowBufferDisp : public COleDispatchDriver
{
public:
	RowBufferDisp() {}		// Calls COleDispatchDriver default constructor
	RowBufferDisp(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	RowBufferDisp(const RowBufferDisp& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:
	long GetRowCount();
	void SetRowCount(long);
	short GetColumnCount();
	void SetColumnCount(short);
	short GetReadType();
	void SetReadType(short);

// Operations
public:
	VARIANT GetBookmark(long Row);
	void SetBookmark(long Row, const VARIANT& newValue);
	CString GetColumnName(short Col);
	void SetColumnName(short Col, LPCTSTR lpszNewValue);
	VARIANT GetValue(long Row, short Col);
	void SetValue(long Row, short Col, const VARIANT& newValue);
};
/////////////////////////////////////////////////////////////////////////////
// _DSSDBGrid wrapper class

class _DSSDBGrid : public COleDispatchDriver
{
public:
	_DSSDBGrid() {}		// Calls COleDispatchDriver default constructor
	_DSSDBGrid(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	_DSSDBGrid(const _DSSDBGrid& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:
	LPUNKNOWN GetDataSource();
	void SetDataSource(LPUNKNOWN);
	OLE_COLOR GetBackColor();
	void SetBackColor(OLE_COLOR);
	CString GetCaption();
	void SetCaption(LPCTSTR);
	BOOL GetEnabled();
	void SetEnabled(BOOL);
	LPFONTDISP GetFont();
	void SetFont(LPFONTDISP);
	OLE_COLOR GetForeColor();
	void SetForeColor(OLE_COLOR);
	OLE_HANDLE GetHWnd();
	void SetHWnd(OLE_HANDLE);
	unsigned long GetBackColorEven();
	void SetBackColorEven(unsigned long);
	unsigned long GetBackColorOdd();
	void SetBackColorOdd(unsigned long);
	unsigned long GetForeColorEven();
	void SetForeColorEven(unsigned long);
	unsigned long GetForeColorOdd();
	void SetForeColorOdd(unsigned long);
	long GetCaptionAlignment();
	void SetCaptionAlignment(long);
	long GetDividerType();
	void SetDividerType(long);
	long GetScrollBars();
	void SetScrollBars(long);
	BOOL GetAllowAddNew();
	void SetAllowAddNew(BOOL);
	BOOL GetAllowDelete();
	void SetAllowDelete(BOOL);
	BOOL GetAllowUpdate();
	void SetAllowUpdate(BOOL);
	BOOL GetAllowRowSizing();
	void SetAllowRowSizing(BOOL);
	BOOL GetAllowGroupSizing();
	void SetAllowGroupSizing(BOOL);
	BOOL GetAllowColumnSizing();
	void SetAllowColumnSizing(BOOL);
	BOOL GetRecordSelectors();
	void SetRecordSelectors(BOOL);
	BOOL GetGroupHeaders();
	void SetGroupHeaders(BOOL);
	BOOL GetColumnHeaders();
	void SetColumnHeaders(BOOL);
	short GetHeadLines();
	void SetHeadLines(short);
	LPDISPATCH GetHeadFont();
	void SetHeadFont(LPDISPATCH);
	long GetHeadFont3D();
	void SetHeadFont3D(long);
	short GetCol();
	void SetCol(short);
	short GetRow();
	void SetRow(short);
	long GetDataMode();
	void SetDataMode(long);
	short GetVisibleCols();
	void SetVisibleCols(short);
	short GetVisibleRows();
	void SetVisibleRows(short);
	short GetLeftCol();
	void SetLeftCol(short);
	short GetCols();
	void SetCols(short);
	long GetRows();
	void SetRows(long);
	CString GetFieldDelimiter();
	void SetFieldDelimiter(LPCTSTR);
	CString GetFieldSeparator();
	void SetFieldSeparator(LPCTSTR);
	long GetFont3D();
	void SetFont3D(long);
	long GetDividerStyle();
	void SetDividerStyle(long);
	BOOL GetUseExactRowCount();
	void SetUseExactRowCount(BOOL);
	unsigned long GetBevelColorFace();
	void SetBevelColorFace(unsigned long);
	unsigned long GetBevelColorFrame();
	void SetBevelColorFrame(unsigned long);
	unsigned long GetBevelColorHighlight();
	void SetBevelColorHighlight(unsigned long);
	unsigned long GetBevelColorShadow();
	void SetBevelColorShadow(unsigned long);
	long GetBevelColorScheme();
	void SetBevelColorScheme(long);
	short GetGroupHeadLines();
	void SetGroupHeadLines(short);
	VARIANT GetBookmark();
	void SetBookmark(const VARIANT&);
	BOOL GetAllowDragDrop();
	void SetAllowDragDrop(BOOL);
	VARIANT GetFirstRow();
	void SetFirstRow(const VARIANT&);
	float GetDefColWidth();
	void SetDefColWidth(float);
	float GetRowHeight();
	void SetRowHeight(float);
	short GetVisibleGrps();
	void SetVisibleGrps(short);
	short GetLeftGrp();
	void SetLeftGrp(short);
	BOOL GetCheckBox3D();
	void SetCheckBox3D(BOOL);
	short GetLevelCount();
	void SetLevelCount(short);
	BOOL GetRedraw();
	void SetRedraw(BOOL);
	BOOL GetComboDroppedDown();
	void SetComboDroppedDown(BOOL);
	BOOL GetAllowGroupMoving();
	void SetAllowGroupMoving(BOOL);
	BOOL GetAllowGroupSwapping();
	void SetAllowGroupSwapping(BOOL);
	long GetAllowColumnMoving();
	void SetAllowColumnMoving(long);
	long GetAllowColumnSwapping();
	void SetAllowColumnSwapping(long);
	float GetResizeHeight();
	void SetResizeHeight(float);
	float GetResizeWidth();
	void SetResizeWidth(float);
	VARIANT GetTagVariant();
	void SetTagVariant(const VARIANT&);
	short GetSplitterPos();
	void SetSplitterPos(short);
	BOOL GetSplitterVisible();
	void SetSplitterVisible(BOOL);
	long GetSelectTypeCol();
	void SetSelectTypeCol(long);
	long GetSelectTypeRow();
	void SetSelectTypeRow(long);
	BOOL GetSelectByCell();
	void SetSelectByCell(BOOL);
	CString GetHeadStyleSet();
	void SetHeadStyleSet(LPCTSTR);
	CString GetStyleSet();
	void SetStyleSet(LPCTSTR);
	BOOL GetMultiLine();
	void SetMultiLine(BOOL);
	CString GetActiveRowStyleSet();
	void SetActiveRowStyleSet(LPCTSTR);
	LPDISPATCH GetPictureButton();
	void SetPictureButton(LPDISPATCH);
	LPDISPATCH GetPictureComboButton();
	void SetPictureComboButton(LPDISPATCH);
	LPDISPATCH GetPictureRecordSelectors();
	void SetPictureRecordSelectors(LPDISPATCH);
	BOOL GetAllowColumnShrinking();
	void SetAllowColumnShrinking(BOOL);
	BOOL GetAllowGroupShrinking();
	void SetAllowGroupShrinking(BOOL);
	BOOL GetDataChanged();
	void SetDataChanged(BOOL);
	BOOL GetRowChanged();
	void SetRowChanged(BOOL);
	BOOL GetBalloonHelp();
	void SetBalloonHelp(BOOL);
	long GetCellNavigation();
	void SetCellNavigation(long);
	long GetRowNavigation();
	void SetRowNavigation(long);
	short GetGrp();
	void SetGrp(short);
	long GetRowSelectionStyle();
	void SetRowSelectionStyle(long);
	long GetMaxSelectedRows();
	void SetMaxSelectedRows(long);
	BOOL GetDroppedDown();
	void SetDroppedDown(BOOL);
	long GetBorderStyle();
	void SetBorderStyle(long);
	long GetHwndEdit();
	void SetHwndEdit(long);
	LPDISPATCH GetColumns();
	void SetColumns(LPDISPATCH);
	LPDISPATCH GetGroups();
	void SetGroups(LPDISPATCH);
	LPDISPATCH GetSelBookmarks();
	void SetSelBookmarks(LPDISPATCH);
	LPDISPATCH GetStyleSets();
	void SetStyleSets(LPDISPATCH);
	BOOL GetUseDefaults();
	void SetUseDefaults(BOOL);
	long GetTabNavigation();
	void SetTabNavigation(long);
	LPDISPATCH GetPageHeaderFont();
	void SetPageHeaderFont(LPDISPATCH);
	LPDISPATCH GetPageFooterFont();
	void SetPageFooterFont(LPDISPATCH);

// Operations
public:
	void DoClick();
	void AddItem(LPCTSTR Item, const VARIANT& Index);
	void RemoveItem(long Index);
	VARIANT RowBookmark(long RowNum);
	short RowContaining(float Y);
	float RowTop(short RowNum);
	void Scroll(short Cols, long Rows);
	void ReBind();
	short ColContaining(float X, const VARIANT& Y);
	short GrpContaining(float X);
	VARIANT GetBookmark(long RowNum);
	void RemoveAll();
	void MoveFirst();
	void MovePrevious();
	void MoveNext();
	void MoveLast();
	void MoveRecords(long Records);
	BOOL IsItemInList();
	long WhereIs(float X, float Y);
	LPDISPATCH ActiveCell();
	short ColPosition(short ColPos);
	short GrpPosition(short GrpPos);
	void Reset();
	BOOL IsAddRow();
	VARIANT AddItemBookmark(long RowIndex);
	long AddItemRowIndex(const VARIANT& Bookmark);
	void Refresh();
	void Update();
	void AboutBox();
	CString DeleteSelected();
	void AddNew();
	void CancelUpdate();
	void Export(long Type, long Flags, LPCTSTR ExportToFile, const VARIANT& HTMLTemplate, const VARIANT& OutputFileField);
	void PrintData(long Flags, BOOL ShowSetupDialog, BOOL ShowPrintDialog, const VARIANT& Reserved);
	void SaveLayout(LPCTSTR FileName, long Flags);
	void LoadLayout(LPCTSTR FileName);
};
/////////////////////////////////////////////////////////////////////////////
// _DSSDBGridEvents wrapper class

class _DSSDBGridEvents : public COleDispatchDriver
{
public:
	_DSSDBGridEvents() {}		// Calls COleDispatchDriver default constructor
	_DSSDBGridEvents(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	_DSSDBGridEvents(const _DSSDBGridEvents& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	void InitColumnProps();
	void KeyDown(short* KeyCode, short Shift);
	void Click();
	void DblClick();
	void KeyPress(short* KeyAscii);
	void KeyUp(short* KeyCode, short Shift);
	void MouseDown(short Button, short Shift, long X, long Y);
	void MouseMove(short Button, short Shift, long X, long Y);
	void MouseUp(short Button, short Shift, long X, long Y);
	void BtnClick();
	void AfterColUpdate(short ColIndex);
	void BeforeColUpdate(short ColIndex, const VARIANT& OldValue, short* Cancel);
	void BeforeInsert(short* Cancel);
	void BeforeUpdate(short* Cancel);
	void ColResize(short ColIndex, short* Cancel);
	void RowResize(short* Cancel);
	void HeadClick(short ColIndex);
	void RowColChange(const VARIANT& LastRow, short LastCol);
	void Scroll(short* Cancel);
	void Change();
	void GrpHeadClick(short GrpIndex);
	void ComboDropDown();
	void SplitterMove(short* Cancel);
	void GrpResize(short GrpIndex, short* Cancel);
	void ComboCloseUp();
	void RowLoaded(const VARIANT& Bookmark);
	void AfterDelete(short* RtnDispErrMsg);
	void BeforeDelete(short* Cancel, short* DispPromptMsg);
	void AfterUpdate(short* RtnDispErrMsg);
	void AfterInsert(short* RtnDispErrMsg);
	void UnboundReadData(LPDISPATCH RowBuf, VARIANT* StartLocation, BOOL ReadPriorRows);
	void UnboundAddData(LPDISPATCH RowBuf, VARIANT* NewRowBookmark);
	void UnboundWriteData(LPDISPATCH RowBuf, VARIANT* WriteLocation);
	void UnboundDeleteRow(VARIANT* Bookmark);
	void UnboundPositionData(VARIANT* StartLocation, long NumberOfRowsToMove, VARIANT* NewLocation);
	void ColMove(short ColIndex, short NewPos, short* Cancel);
	void ColSwap(short ColIndex, short NewPos, short* Cancel);
	void GrpMove(short GrpIndex, short NewPos, short* Cancel);
	void GrpSwap(short GrpIndex, short NewPos, short* Cancel);
	void SelChange(short SelType, short* Cancel, short* DispSelRowOverflow);
	void UpdateError(short ColIndex, BSTR* Text, short* ErrCode, BSTR* ErrString, short* Cancel);
	void BeforeRowColChange(short* Cancel);
	void ScrollAfter();
	void Error(short DataError, short* Response);
	void AfterPosChanged(short WhatChanged, short NewIndex);
	void RowExport(const VARIANT& Bookmark, short* Cancel);
	void RowPrint(const VARIANT& Bookmark, long PageNumber, short* Cancel);
	void PrintBegin(LPDISPATCH ssPrintInfo);
	void PrintError(long PrintError, short* Response);
	void PrintInitialize(LPDISPATCH ssPrintInfo);
	void ValidationError(BSTR* InvalidText, short StartPosition);
};
/////////////////////////////////////////////////////////////////////////////
// _DSSDBCombo wrapper class

class _DSSDBCombo : public COleDispatchDriver
{
public:
	_DSSDBCombo() {}		// Calls COleDispatchDriver default constructor
	_DSSDBCombo(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	_DSSDBCombo(const _DSSDBCombo& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:
	CString GetText();
	void SetText(LPCTSTR);
	CString Get_Text();
	void Set_Text(LPCTSTR);
	OLE_COLOR GetBackColor();
	void SetBackColor(OLE_COLOR);
	BOOL GetEnabled();
	void SetEnabled(BOOL);
	LPFONTDISP GetFont();
	void SetFont(LPFONTDISP);
	OLE_COLOR GetForeColor();
	void SetForeColor(OLE_COLOR);
	OLE_HANDLE GetHWnd();
	void SetHWnd(OLE_HANDLE);
	VARIANT GetValue();
	void SetValue(const VARIANT&);
	LPUNKNOWN GetDataSourceList();
	void SetDataSourceList(LPUNKNOWN);
	unsigned long GetBevelColorFrame();
	void SetBevelColorFrame(unsigned long);
	unsigned long GetBevelColorHighlight();
	void SetBevelColorHighlight(unsigned long);
	unsigned long GetBevelColorShadow();
	void SetBevelColorShadow(unsigned long);
	long GetBevelColorScheme();
	void SetBevelColorScheme(long);
	unsigned long GetBackColorEven();
	void SetBackColorEven(unsigned long);
	unsigned long GetBackColorOdd();
	void SetBackColorOdd(unsigned long);
	VARIANT GetBookmark();
	void SetBookmark(const VARIANT&);
	short GetCols();
	void SetCols(short);
	CString GetDataFieldList();
	void SetDataFieldList(LPCTSTR);
	long GetDataMode();
	void SetDataMode(long);
	long GetDividerStyle();
	void SetDividerStyle(long);
	long GetDividerType();
	void SetDividerType(long);
	BOOL GetDroppedDown();
	void SetDroppedDown(BOOL);
	long GetFont3D();
	void SetFont3D(long);
	unsigned long GetForeColorOdd();
	void SetForeColorOdd(unsigned long);
	BOOL GetColumnHeaders();
	void SetColumnHeaders(BOOL);
	unsigned long GetForeColorEven();
	void SetForeColorEven(unsigned long);
	float GetDefColWidth();
	void SetDefColWidth(float);
	BOOL GetGroupHeaders();
	void SetGroupHeaders(BOOL);
	short GetGroupHeadLines();
	void SetGroupHeadLines(short);
	LPDISPATCH GetHeadFont();
	void SetHeadFont(LPDISPATCH);
	long GetHeadFont3D();
	void SetHeadFont3D(long);
	short GetHeadLines();
	void SetHeadLines(short);
	short GetLeftCol();
	void SetLeftCol(short);
	short GetLeftGrp();
	void SetLeftGrp(short);
	short GetLevelCount();
	void SetLevelCount(short);
	BOOL GetListAutoPosition();
	void SetListAutoPosition(BOOL);
	BOOL GetListAutoValidate();
	void SetListAutoValidate(BOOL);
	BOOL GetListWidthAutoSize();
	void SetListWidthAutoSize(BOOL);
	short GetMaxDropDownItems();
	void SetMaxDropDownItems(short);
	short GetMinDropDownItems();
	void SetMinDropDownItems(short);
	BOOL GetRedraw();
	void SetRedraw(BOOL);
	short GetRow();
	void SetRow(short);
	long GetScrollBars();
	void SetScrollBars(long);
	VARIANT GetTagVariant();
	void SetTagVariant(const VARIANT&);
	short GetVisibleCols();
	void SetVisibleCols(short);
	short GetVisibleRows();
	void SetVisibleRows(short);
	short GetVisibleGrps();
	void SetVisibleGrps(short);
	float GetRowHeight();
	void SetRowHeight(float);
	long GetRows();
	void SetRows(long);
	unsigned long GetBevelColorFace();
	void SetBevelColorFace(unsigned long);
	BOOL GetAllowInput();
	void SetAllowInput(BOOL);
	BOOL GetAllowNull();
	void SetAllowNull(BOOL);
	short GetBevelWidth();
	void SetBevelWidth(short);
	BOOL GetAutoRestore();
	void SetAutoRestore(BOOL);
	LPDISPATCH GetPictureDropDown();
	void SetPictureDropDown(LPDISPATCH);
	long GetMousePointer();
	void SetMousePointer(long);
	LPDISPATCH GetMouseIcon();
	void SetMouseIcon(LPDISPATCH);
	long GetBevelType();
	void SetBevelType(long);
	CString GetFieldDelimiter();
	void SetFieldDelimiter(LPCTSTR);
	CString GetFieldSeparator();
	void SetFieldSeparator(LPCTSTR);
	CString GetTextFormat();
	void SetTextFormat(LPCTSTR);
	BOOL GetUseExactRowCount();
	void SetUseExactRowCount(BOOL);
	float GetListWidth();
	void SetListWidth(float);
	short GetSelStart();
	void SetSelStart(short);
	CString GetSelText();
	void SetSelText(LPCTSTR);
	short GetSelLength();
	void SetSelLength(short);
	BOOL GetCheckBox3D();
	void SetCheckBox3D(BOOL);
	CString GetHeadStyleSet();
	void SetHeadStyleSet(LPCTSTR);
	CString GetStyleSet();
	void SetStyleSet(LPCTSTR);
	BOOL GetMultiLine();
	void SetMultiLine(BOOL);
	VARIANT GetFirstRow();
	void SetFirstRow(const VARIANT&);
	long GetRowSelectionStyle();
	void SetRowSelectionStyle(long);
	long GetBorderStyle();
	void SetBorderStyle(long);
	long GetHwndEdit();
	void SetHwndEdit(long);
	short GetSplitterPos();
	void SetSplitterPos(short);
	CString GetDataFieldToDisplay();
	void SetDataFieldToDisplay(LPCTSTR);
	LPDISPATCH GetColumns();
	void SetColumns(LPDISPATCH);
	LPDISPATCH GetGroups();
	void SetGroups(LPDISPATCH);
	LPDISPATCH GetSelBookmarks();
	void SetSelBookmarks(LPDISPATCH);
	LPDISPATCH GetStyleSets();
	void SetStyleSets(LPDISPATCH);
	CString GetMask();
	void SetMask(LPCTSTR);
	BOOL GetPromptInclude();
	void SetPromptInclude(BOOL);
	long GetClipMode();
	void SetClipMode(long);
	CString GetPromptChar();
	void SetPromptChar(LPCTSTR);

// Operations
public:
	BOOL IsItemInList();
	BOOL IsTextValid(const VARIANT& ErrCode);
	void AddItem(LPCTSTR Item, const VARIANT& Index);
	short ColContaining(float X, const VARIANT& Y);
	void DoClick();
	VARIANT GetBookmark(long RowNum);
	void MoveFirst();
	void MoveLast();
	void MoveNext();
	void MovePrevious();
	void MoveRecords(long Records);
	void ReBind();
	VARIANT RowBookmark(long RowNum);
	short RowContaining(float Y);
	float RowTop(short RowNum);
	void Scroll(short Cols, long Rows);
	short GrpContaining(float X);
	void RemoveAll();
	void RemoveItem(long Index);
	void Reset();
	VARIANT AddItemBookmark(long RowIndex);
	long AddItemRowIndex(const VARIANT& Bookmark);
	void Refresh();
	void SaveLayout(LPCTSTR FileName, long Flags);
	void LoadLayout(LPCTSTR FileName);
	void AboutBox();
};
/////////////////////////////////////////////////////////////////////////////
// _DSSDBComboEvents wrapper class

class _DSSDBComboEvents : public COleDispatchDriver
{
public:
	_DSSDBComboEvents() {}		// Calls COleDispatchDriver default constructor
	_DSSDBComboEvents(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	_DSSDBComboEvents(const _DSSDBComboEvents& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	void InitColumnProps();
	void Click();
	void DropDown();
	void CloseUp();
	void Change();
	void DblClick();
	void KeyDown(short* KeyCode, short Shift);
	void KeyPress(short* KeyAscii);
	void KeyUp(short* KeyCode, short Shift);
	void MouseDown(short Button, short Shift, long X, long Y);
	void MouseMove(short Button, short Shift, long X, long Y);
	void MouseUp(short Button, short Shift, long X, long Y);
	void Scroll(short* Cancel);
	void PositionList(LPCTSTR Text);
	void TextError(long* ErrCode, BSTR* ErrString, BSTR* RestoreString, BSTR* Text, short* RtnDispErrMsg, short* RtnRestore);
	void ValidateList(BSTR* Text, short* RtnPassed);
	void RowLoaded(const VARIANT& Bookmark);
	void UnboundReadData(LPDISPATCH RowBuf, VARIANT* StartLocation, BOOL ReadPriorRows);
	void UnboundPositionData(VARIANT* StartLocation, long NumberOfRowsToMove, VARIANT* NewLocation);
	void ScrollAfter();
	void ValidationError(BSTR* InvalidText, short StartPosition);
};
/////////////////////////////////////////////////////////////////////////////
// _DSSDBDropDown wrapper class

class _DSSDBDropDown : public COleDispatchDriver
{
public:
	_DSSDBDropDown() {}		// Calls COleDispatchDriver default constructor
	_DSSDBDropDown(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	_DSSDBDropDown(const _DSSDBDropDown& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:
	OLE_COLOR GetBackColor();
	void SetBackColor(OLE_COLOR);
	unsigned long GetBackColorEven();
	void SetBackColorEven(unsigned long);
	unsigned long GetBackColorOdd();
	void SetBackColorOdd(unsigned long);
	unsigned long GetBevelColorHighlight();
	void SetBevelColorHighlight(unsigned long);
	unsigned long GetBevelColorFace();
	void SetBevelColorFace(unsigned long);
	unsigned long GetBevelColorFrame();
	void SetBevelColorFrame(unsigned long);
	long GetBevelColorScheme();
	void SetBevelColorScheme(long);
	unsigned long GetBevelColorShadow();
	void SetBevelColorShadow(unsigned long);
	BOOL GetColumnHeaders();
	void SetColumnHeaders(BOOL);
	long GetDataMode();
	void SetDataMode(long);
	LPUNKNOWN GetDataSource();
	void SetDataSource(LPUNKNOWN);
	float GetDefColWidth();
	void SetDefColWidth(float);
	long GetDividerStyle();
	void SetDividerStyle(long);
	long GetDividerType();
	void SetDividerType(long);
	BOOL GetEnabled();
	void SetEnabled(BOOL);
	LPFONTDISP GetFont();
	void SetFont(LPFONTDISP);
	long GetFont3D();
	void SetFont3D(long);
	OLE_COLOR GetForeColor();
	void SetForeColor(OLE_COLOR);
	unsigned long GetForeColorEven();
	void SetForeColorEven(unsigned long);
	unsigned long GetForeColorOdd();
	void SetForeColorOdd(unsigned long);
	long GetHeadFont3D();
	void SetHeadFont3D(long);
	LPDISPATCH GetHeadFont();
	void SetHeadFont(LPDISPATCH);
	short GetHeadLines();
	void SetHeadLines(short);
	OLE_HANDLE GetHWnd();
	void SetHWnd(OLE_HANDLE);
	short GetLeftCol();
	void SetLeftCol(short);
	short GetLeftGrp();
	void SetLeftGrp(short);
	short GetLevelCount();
	void SetLevelCount(short);
	BOOL GetRedraw();
	void SetRedraw(BOOL);
	short GetRow();
	void SetRow(short);
	long GetScrollBars();
	void SetScrollBars(long);
	VARIANT GetTagVariant();
	void SetTagVariant(const VARIANT&);
	float GetRowHeight();
	void SetRowHeight(float);
	short GetCols();
	void SetCols(short);
	BOOL GetGroupHeaders();
	void SetGroupHeaders(BOOL);
	VARIANT GetBookmark();
	void SetBookmark(const VARIANT&);
	short GetGroupHeadLines();
	void SetGroupHeadLines(short);
	long GetRows();
	void SetRows(long);
	short GetVisibleRows();
	void SetVisibleRows(short);
	short GetVisibleCols();
	void SetVisibleCols(short);
	short GetVisibleGrps();
	void SetVisibleGrps(short);
	CString GetDataFieldList();
	void SetDataFieldList(LPCTSTR);
	BOOL GetListAutoPosition();
	void SetListAutoPosition(BOOL);
	BOOL GetListAutoValidate();
	void SetListAutoValidate(BOOL);
	BOOL GetListWidthAutoSize();
	void SetListWidthAutoSize(BOOL);
	short GetMaxDropDownItems();
	void SetMaxDropDownItems(short);
	short GetMinDropDownItems();
	void SetMinDropDownItems(short);
	BOOL GetDroppedDown();
	void SetDroppedDown(BOOL);
	CString GetFieldDelimiter();
	void SetFieldDelimiter(LPCTSTR);
	CString GetFieldSeparator();
	void SetFieldSeparator(LPCTSTR);
	BOOL GetUseExactRowCount();
	void SetUseExactRowCount(BOOL);
	float GetListWidth();
	void SetListWidth(float);
	BOOL GetCheckBox3D();
	void SetCheckBox3D(BOOL);
	CString GetHeadStyleSet();
	void SetHeadStyleSet(LPCTSTR);
	CString GetStyleSet();
	void SetStyleSet(LPCTSTR);
	VARIANT GetFirstRow();
	void SetFirstRow(const VARIANT&);
	long GetRowSelectionStyle();
	void SetRowSelectionStyle(long);
	long GetBorderStyle();
	void SetBorderStyle(long);
	short GetSplitterPos();
	void SetSplitterPos(short);
	CString GetDataFieldToDisplay();
	void SetDataFieldToDisplay(LPCTSTR);
	LPDISPATCH GetColumns();
	void SetColumns(LPDISPATCH);
	LPDISPATCH GetGroups();
	void SetGroups(LPDISPATCH);
	LPDISPATCH GetSelBookmarks();
	void SetSelBookmarks(LPDISPATCH);
	LPDISPATCH GetStyleSets();
	void SetStyleSets(LPDISPATCH);

// Operations
public:
	void RemoveItem(long Index);
	VARIANT RowBookmark(long RowNum);
	float RowTop(short RowNum);
	void ReBind();
	short ColContaining(float X, const VARIANT& Y);
	VARIANT GetBookmark(long RowNum);
	void RemoveAll();
	short GrpContaining(float X);
	void DoClick();
	void AddItem(LPCTSTR Item, const VARIANT& Index);
	short RowContaining(float Y);
	void Scroll(short Cols, long Rows);
	void MoveFirst();
	void MovePrevious();
	void MoveRecords(long Records);
	void MoveNext();
	void MoveLast();
	void Reset();
	VARIANT AddItemBookmark(long RowIndex);
	long AddItemRowIndex(const VARIANT& Bookmark);
	void Refresh();
	void SaveLayout(LPCTSTR FileName, long Flags);
	void LoadLayout(LPCTSTR FileName);
	void AboutBox();
};
/////////////////////////////////////////////////////////////////////////////
// _DSSDBDropDownEvents wrapper class

class _DSSDBDropDownEvents : public COleDispatchDriver
{
public:
	_DSSDBDropDownEvents() {}		// Calls COleDispatchDriver default constructor
	_DSSDBDropDownEvents(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	_DSSDBDropDownEvents(const _DSSDBDropDownEvents& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	void InitColumnProps();
	void Click();
	void Scroll(short* Cancel);
	void CloseUp();
	void DropDown();
	void PositionList(LPCTSTR Text);
	void ValidateList(BSTR* Text, short* RtnPassed);
	void TextError(long* ErrCode, BSTR* ErrString, BSTR* RestoreString, BSTR* Text, short* RtnDispErrMsg, short* RtnRestore);
	void UnboundReadData(LPDISPATCH RowBuf, VARIANT* StartLocation, BOOL ReadPriorRows);
	void RowLoaded(const VARIANT& Bookmark);
	void UnboundPositionData(VARIANT* StartLocation, long NumberOfRowsToMove, VARIANT* NewLocation);
	void ScrollAfter();
};
