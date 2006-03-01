// Machine generated IDispatch wrapper class(es) created with ClassWizard

#include "stdafx.h"
#include "ssdw3b32.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// ISSColumns properties

short ISSColumns::GetCount()
{
	short result;
	GetProperty(0x1, VT_I2, (void*)&result);
	return result;
}

void ISSColumns::SetCount(short propVal)
{
	SetProperty(0x1, VT_I2, propVal);
}

/////////////////////////////////////////////////////////////////////////////
// ISSColumns operations

void ISSColumns::Remove(const VARIANT& Index)
{
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x3, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 &Index);
}

void ISSColumns::Add(short Index)
{
	static BYTE parms[] =
		VTS_I2;
	InvokeHelper(0x4, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Index);
}

void ISSColumns::RemoveAll()
{
	InvokeHelper(0x5, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

LPUNKNOWN ISSColumns::_NewEnum()
{
	LPUNKNOWN result;
	InvokeHelper(0xfffffffc, DISPATCH_METHOD, VT_UNKNOWN, (void*)&result, NULL);
	return result;
}

LPDISPATCH ISSColumns::GetItem(const VARIANT& Index)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x0, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, parms,
		&Index);
	return result;
}

void ISSColumns::SetItem(const VARIANT& Index, LPDISPATCH newValue)
{
	static BYTE parms[] =
		VTS_VARIANT VTS_DISPATCH;
	InvokeHelper(0x0, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 &Index, newValue);
}


/////////////////////////////////////////////////////////////////////////////
// ISSColumn properties

CString ISSColumn::GetCaption()
{
	CString result;
	GetProperty(0x1, VT_BSTR, (void*)&result);
	return result;
}

void ISSColumn::SetCaption(LPCTSTR propVal)
{
	SetProperty(0x1, VT_BSTR, propVal);
}

long ISSColumn::GetAlignment()
{
	long result;
	GetProperty(0x2, VT_I4, (void*)&result);
	return result;
}

void ISSColumn::SetAlignment(long propVal)
{
	SetProperty(0x2, VT_I4, propVal);
}

long ISSColumn::GetForeColor()
{
	long result;
	GetProperty(0x3, VT_I4, (void*)&result);
	return result;
}

void ISSColumn::SetForeColor(long propVal)
{
	SetProperty(0x3, VT_I4, propVal);
}

long ISSColumn::GetBackColor()
{
	long result;
	GetProperty(0x4, VT_I4, (void*)&result);
	return result;
}

void ISSColumn::SetBackColor(long propVal)
{
	SetProperty(0x4, VT_I4, propVal);
}

long ISSColumn::GetCase()
{
	long result;
	GetProperty(0x5, VT_I4, (void*)&result);
	return result;
}

void ISSColumn::SetCase(long propVal)
{
	SetProperty(0x5, VT_I4, propVal);
}

CString ISSColumn::GetNumberFormat()
{
	CString result;
	GetProperty(0x6, VT_BSTR, (void*)&result);
	return result;
}

void ISSColumn::SetNumberFormat(LPCTSTR propVal)
{
	SetProperty(0x6, VT_BSTR, propVal);
}

short ISSColumn::GetFieldLen()
{
	short result;
	GetProperty(0x7, VT_I2, (void*)&result);
	return result;
}

void ISSColumn::SetFieldLen(short propVal)
{
	SetProperty(0x7, VT_I2, propVal);
}

BOOL ISSColumn::GetAllowSizing()
{
	BOOL result;
	GetProperty(0x8, VT_BOOL, (void*)&result);
	return result;
}

void ISSColumn::SetAllowSizing(BOOL propVal)
{
	SetProperty(0x8, VT_BOOL, propVal);
}

long ISSColumn::GetHeadForeColor()
{
	long result;
	GetProperty(0x9, VT_I4, (void*)&result);
	return result;
}

void ISSColumn::SetHeadForeColor(long propVal)
{
	SetProperty(0x9, VT_I4, propVal);
}

long ISSColumn::GetHeadBackColor()
{
	long result;
	GetProperty(0xa, VT_I4, (void*)&result);
	return result;
}

void ISSColumn::SetHeadBackColor(long propVal)
{
	SetProperty(0xa, VT_I4, propVal);
}

CString ISSColumn::GetDataField()
{
	CString result;
	GetProperty(0xb, VT_BSTR, (void*)&result);
	return result;
}

void ISSColumn::SetDataField(LPCTSTR propVal)
{
	SetProperty(0xb, VT_BSTR, propVal);
}

float ISSColumn::GetLeft()
{
	float result;
	GetProperty(0xc, VT_R4, (void*)&result);
	return result;
}

void ISSColumn::SetLeft(float propVal)
{
	SetProperty(0xc, VT_R4, propVal);
}

float ISSColumn::GetTop()
{
	float result;
	GetProperty(0xd, VT_R4, (void*)&result);
	return result;
}

void ISSColumn::SetTop(float propVal)
{
	SetProperty(0xd, VT_R4, propVal);
}

BOOL ISSColumn::GetLocked()
{
	BOOL result;
	GetProperty(0xe, VT_BOOL, (void*)&result);
	return result;
}

void ISSColumn::SetLocked(BOOL propVal)
{
	SetProperty(0xe, VT_BOOL, propVal);
}

CString ISSColumn::GetText()
{
	CString result;
	GetProperty(0xf, VT_BSTR, (void*)&result);
	return result;
}

void ISSColumn::SetText(LPCTSTR propVal)
{
	SetProperty(0xf, VT_BSTR, propVal);
}

VARIANT ISSColumn::GetValue()
{
	VARIANT result;
	GetProperty(0x10, VT_VARIANT, (void*)&result);
	return result;
}

void ISSColumn::SetValue(const VARIANT& propVal)
{
	SetProperty(0x10, VT_VARIANT, &propVal);
}

BOOL ISSColumn::GetVisible()
{
	BOOL result;
	GetProperty(0x11, VT_BOOL, (void*)&result);
	return result;
}

void ISSColumn::SetVisible(BOOL propVal)
{
	SetProperty(0x11, VT_BOOL, propVal);
}

float ISSColumn::GetWidth()
{
	float result;
	GetProperty(0x12, VT_R4, (void*)&result);
	return result;
}

void ISSColumn::SetWidth(float propVal)
{
	SetProperty(0x12, VT_R4, propVal);
}

short ISSColumn::GetDataType()
{
	short result;
	GetProperty(0x13, VT_I2, (void*)&result);
	return result;
}

void ISSColumn::SetDataType(short propVal)
{
	SetProperty(0x13, VT_I2, propVal);
}

BOOL ISSColumn::GetSelected()
{
	BOOL result;
	GetProperty(0x14, VT_BOOL, (void*)&result);
	return result;
}

void ISSColumn::SetSelected(BOOL propVal)
{
	SetProperty(0x14, VT_BOOL, propVal);
}

BOOL ISSColumn::GetButtonsAlways()
{
	BOOL result;
	GetProperty(0x15, VT_BOOL, (void*)&result);
	return result;
}

void ISSColumn::SetButtonsAlways(BOOL propVal)
{
	SetProperty(0x15, VT_BOOL, propVal);
}

short ISSColumn::GetLevel()
{
	short result;
	GetProperty(0x16, VT_I2, (void*)&result);
	return result;
}

void ISSColumn::SetLevel(short propVal)
{
	SetProperty(0x16, VT_I2, propVal);
}

long ISSColumn::GetStyle()
{
	long result;
	GetProperty(0x17, VT_I4, (void*)&result);
	return result;
}

void ISSColumn::SetStyle(long propVal)
{
	SetProperty(0x17, VT_I4, propVal);
}

BOOL ISSColumn::GetHasHeadForeColor()
{
	BOOL result;
	GetProperty(0x18, VT_BOOL, (void*)&result);
	return result;
}

void ISSColumn::SetHasHeadForeColor(BOOL propVal)
{
	SetProperty(0x18, VT_BOOL, propVal);
}

BOOL ISSColumn::GetHasHeadBackColor()
{
	BOOL result;
	GetProperty(0x19, VT_BOOL, (void*)&result);
	return result;
}

void ISSColumn::SetHasHeadBackColor(BOOL propVal)
{
	SetProperty(0x19, VT_BOOL, propVal);
}

BOOL ISSColumn::GetHasForeColor()
{
	BOOL result;
	GetProperty(0x1a, VT_BOOL, (void*)&result);
	return result;
}

void ISSColumn::SetHasForeColor(BOOL propVal)
{
	SetProperty(0x1a, VT_BOOL, propVal);
}

BOOL ISSColumn::GetHasBackColor()
{
	BOOL result;
	GetProperty(0x1b, VT_BOOL, (void*)&result);
	return result;
}

void ISSColumn::SetHasBackColor(BOOL propVal)
{
	SetProperty(0x1b, VT_BOOL, propVal);
}

BOOL ISSColumn::GetVertScrollBar()
{
	BOOL result;
	GetProperty(0x1c, VT_BOOL, (void*)&result);
	return result;
}

void ISSColumn::SetVertScrollBar(BOOL propVal)
{
	SetProperty(0x1c, VT_BOOL, propVal);
}

VARIANT ISSColumn::GetTagVariant()
{
	VARIANT result;
	GetProperty(0x1d, VT_VARIANT, (void*)&result);
	return result;
}

void ISSColumn::SetTagVariant(const VARIANT& propVal)
{
	SetProperty(0x1d, VT_VARIANT, &propVal);
}

short ISSColumn::GetListCount()
{
	short result;
	GetProperty(0x1e, VT_I2, (void*)&result);
	return result;
}

void ISSColumn::SetListCount(short propVal)
{
	SetProperty(0x1e, VT_I2, propVal);
}

long ISSColumn::GetDropDownHwnd()
{
	long result;
	GetProperty(0x1f, VT_I4, (void*)&result);
	return result;
}

void ISSColumn::SetDropDownHwnd(long propVal)
{
	SetProperty(0x1f, VT_I4, propVal);
}

CString ISSColumn::GetHeadStyleSet()
{
	CString result;
	GetProperty(0x21, VT_BSTR, (void*)&result);
	return result;
}

void ISSColumn::SetHeadStyleSet(LPCTSTR propVal)
{
	SetProperty(0x21, VT_BSTR, propVal);
}

CString ISSColumn::GetStyleSet()
{
	CString result;
	GetProperty(0x22, VT_BSTR, (void*)&result);
	return result;
}

void ISSColumn::SetStyleSet(LPCTSTR propVal)
{
	SetProperty(0x22, VT_BSTR, propVal);
}

short ISSColumn::GetPosition()
{
	short result;
	GetProperty(0x23, VT_I2, (void*)&result);
	return result;
}

void ISSColumn::SetPosition(short propVal)
{
	SetProperty(0x23, VT_I2, propVal);
}

short ISSColumn::GetGroup()
{
	short result;
	GetProperty(0x24, VT_I2, (void*)&result);
	return result;
}

void ISSColumn::SetGroup(short propVal)
{
	SetProperty(0x24, VT_I2, propVal);
}

long ISSColumn::GetCaptionAlignment()
{
	long result;
	GetProperty(0x25, VT_I4, (void*)&result);
	return result;
}

void ISSColumn::SetCaptionAlignment(long propVal)
{
	SetProperty(0x25, VT_I4, propVal);
}

BOOL ISSColumn::GetColChanged()
{
	BOOL result;
	GetProperty(0x26, VT_BOOL, (void*)&result);
	return result;
}

void ISSColumn::SetColChanged(BOOL propVal)
{
	SetProperty(0x26, VT_BOOL, propVal);
}

CString ISSColumn::GetName()
{
	CString result;
	GetProperty(0x4b, VT_BSTR, (void*)&result);
	return result;
}

void ISSColumn::SetName(LPCTSTR propVal)
{
	SetProperty(0x4b, VT_BSTR, propVal);
}

short ISSColumn::GetListIndex()
{
	short result;
	GetProperty(0x4c, VT_I2, (void*)&result);
	return result;
}

void ISSColumn::SetListIndex(short propVal)
{
	SetProperty(0x4c, VT_I2, propVal);
}

long ISSColumn::GetNullable()
{
	long result;
	GetProperty(0x4d, VT_I4, (void*)&result);
	return result;
}

void ISSColumn::SetNullable(long propVal)
{
	SetProperty(0x4d, VT_I4, propVal);
}

CString ISSColumn::GetMask()
{
	CString result;
	GetProperty(0x4e, VT_BSTR, (void*)&result);
	return result;
}

void ISSColumn::SetMask(LPCTSTR propVal)
{
	SetProperty(0x4e, VT_BSTR, propVal);
}

BOOL ISSColumn::GetPromptInclude()
{
	BOOL result;
	GetProperty(0x4f, VT_BOOL, (void*)&result);
	return result;
}

void ISSColumn::SetPromptInclude(BOOL propVal)
{
	SetProperty(0x4f, VT_BOOL, propVal);
}

long ISSColumn::GetClipMode()
{
	long result;
	GetProperty(0x50, VT_I4, (void*)&result);
	return result;
}

void ISSColumn::SetClipMode(long propVal)
{
	SetProperty(0x50, VT_I4, propVal);
}

CString ISSColumn::GetPromptChar()
{
	CString result;
	GetProperty(0x51, VT_BSTR, (void*)&result);
	return result;
}

void ISSColumn::SetPromptChar(LPCTSTR propVal)
{
	SetProperty(0x51, VT_BSTR, propVal);
}

/////////////////////////////////////////////////////////////////////////////
// ISSColumn operations

CString ISSColumn::CellText(const VARIANT& Bookmark)
{
	CString result;
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x27, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms,
		&Bookmark);
	return result;
}

VARIANT ISSColumn::CellValue(const VARIANT& Bookmark)
{
	VARIANT result;
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x28, DISPATCH_METHOD, VT_VARIANT, (void*)&result, parms,
		&Bookmark);
	return result;
}

void ISSColumn::AddItem(LPCTSTR Item, const VARIANT& Index)
{
	static BYTE parms[] =
		VTS_BSTR VTS_VARIANT;
	InvokeHelper(0x29, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Item, &Index);
}

void ISSColumn::RemoveItem(short Index)
{
	static BYTE parms[] =
		VTS_I2;
	InvokeHelper(0x2a, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Index);
}

void ISSColumn::RemoveAll()
{
	InvokeHelper(0x2b, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

CString ISSColumn::GetList(short Index)
{
	CString result;
	static BYTE parms[] =
		VTS_I2;
	InvokeHelper(0x2d, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, parms,
		Index);
	return result;
}

void ISSColumn::SetList(short Index, LPCTSTR lpszNewValue)
{
	static BYTE parms[] =
		VTS_I2 VTS_BSTR;
	InvokeHelper(0x2d, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 Index, lpszNewValue);
}

void ISSColumn::CellStyleSet(LPCTSTR StyleSet, const VARIANT& RowNum)
{
	static BYTE parms[] =
		VTS_BSTR VTS_VARIANT;
	InvokeHelper(0x2c, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 StyleSet, &RowNum);
}

BOOL ISSColumn::IsCellValid()
{
	BOOL result;
	InvokeHelper(0x2e, DISPATCH_METHOD, VT_BOOL, (void*)&result, NULL);
	return result;
}

long ISSColumn::GetItemData(short Index)
{
	long result;
	static BYTE parms[] =
		VTS_I2;
	InvokeHelper(0x2f, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, parms,
		Index);
	return result;
}

void ISSColumn::SetItemData(short Index, long nNewValue)
{
	static BYTE parms[] =
		VTS_I2 VTS_I4;
	InvokeHelper(0x2f, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 Index, nNewValue);
}


/////////////////////////////////////////////////////////////////////////////
// ISSGroupColumns properties

short ISSGroupColumns::GetCount()
{
	short result;
	GetProperty(0x1, VT_I2, (void*)&result);
	return result;
}

void ISSGroupColumns::SetCount(short propVal)
{
	SetProperty(0x1, VT_I2, propVal);
}

/////////////////////////////////////////////////////////////////////////////
// ISSGroupColumns operations

void ISSGroupColumns::Remove(const VARIANT& Index)
{
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x3, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 &Index);
}

void ISSGroupColumns::Add(short Index)
{
	static BYTE parms[] =
		VTS_I2;
	InvokeHelper(0x4, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Index);
}

void ISSGroupColumns::RemoveAll()
{
	InvokeHelper(0x5, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

LPUNKNOWN ISSGroupColumns::_NewEnum()
{
	LPUNKNOWN result;
	InvokeHelper(0xfffffffc, DISPATCH_METHOD, VT_UNKNOWN, (void*)&result, NULL);
	return result;
}

LPDISPATCH ISSGroupColumns::GetItem(const VARIANT& Index)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x0, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, parms,
		&Index);
	return result;
}

void ISSGroupColumns::SetItem(const VARIANT& Index, LPDISPATCH newValue)
{
	static BYTE parms[] =
		VTS_VARIANT VTS_DISPATCH;
	InvokeHelper(0x0, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 &Index, newValue);
}


/////////////////////////////////////////////////////////////////////////////
// ISSGroups properties

short ISSGroups::GetCount()
{
	short result;
	GetProperty(0x1, VT_I2, (void*)&result);
	return result;
}

void ISSGroups::SetCount(short propVal)
{
	SetProperty(0x1, VT_I2, propVal);
}

/////////////////////////////////////////////////////////////////////////////
// ISSGroups operations

void ISSGroups::Remove(const VARIANT& Index)
{
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x3, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 &Index);
}

void ISSGroups::Add(short Index)
{
	static BYTE parms[] =
		VTS_I2;
	InvokeHelper(0x4, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Index);
}

void ISSGroups::RemoveAll()
{
	InvokeHelper(0x5, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

LPUNKNOWN ISSGroups::_NewEnum()
{
	LPUNKNOWN result;
	InvokeHelper(0xfffffffc, DISPATCH_METHOD, VT_UNKNOWN, (void*)&result, NULL);
	return result;
}

LPDISPATCH ISSGroups::GetItem(const VARIANT& Index)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x0, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, parms,
		&Index);
	return result;
}

void ISSGroups::SetItem(const VARIANT& Index, LPDISPATCH newValue)
{
	static BYTE parms[] =
		VTS_VARIANT VTS_DISPATCH;
	InvokeHelper(0x0, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 &Index, newValue);
}


/////////////////////////////////////////////////////////////////////////////
// ISSGroup properties

CString ISSGroup::GetCaption()
{
	CString result;
	GetProperty(0x1, VT_BSTR, (void*)&result);
	return result;
}

void ISSGroup::SetCaption(LPCTSTR propVal)
{
	SetProperty(0x1, VT_BSTR, propVal);
}

long ISSGroup::GetCaptionAlignment()
{
	long result;
	GetProperty(0x2, VT_I4, (void*)&result);
	return result;
}

void ISSGroup::SetCaptionAlignment(long propVal)
{
	SetProperty(0x2, VT_I4, propVal);
}

BOOL ISSGroup::GetAllowSizing()
{
	BOOL result;
	GetProperty(0x3, VT_BOOL, (void*)&result);
	return result;
}

void ISSGroup::SetAllowSizing(BOOL propVal)
{
	SetProperty(0x3, VT_BOOL, propVal);
}

float ISSGroup::GetLeft()
{
	float result;
	GetProperty(0x4, VT_R4, (void*)&result);
	return result;
}

void ISSGroup::SetLeft(float propVal)
{
	SetProperty(0x4, VT_R4, propVal);
}

float ISSGroup::GetTop()
{
	float result;
	GetProperty(0x5, VT_R4, (void*)&result);
	return result;
}

void ISSGroup::SetTop(float propVal)
{
	SetProperty(0x5, VT_R4, propVal);
}

BOOL ISSGroup::GetVisible()
{
	BOOL result;
	GetProperty(0x6, VT_BOOL, (void*)&result);
	return result;
}

void ISSGroup::SetVisible(BOOL propVal)
{
	SetProperty(0x6, VT_BOOL, propVal);
}

float ISSGroup::GetWidth()
{
	float result;
	GetProperty(0x7, VT_R4, (void*)&result);
	return result;
}

void ISSGroup::SetWidth(float propVal)
{
	SetProperty(0x7, VT_R4, propVal);
}

BOOL ISSGroup::GetSelected()
{
	BOOL result;
	GetProperty(0x8, VT_BOOL, (void*)&result);
	return result;
}

void ISSGroup::SetSelected(BOOL propVal)
{
	SetProperty(0x8, VT_BOOL, propVal);
}

long ISSGroup::GetHeadForeColor()
{
	long result;
	GetProperty(0x9, VT_I4, (void*)&result);
	return result;
}

void ISSGroup::SetHeadForeColor(long propVal)
{
	SetProperty(0x9, VT_I4, propVal);
}

long ISSGroup::GetHeadBackColor()
{
	long result;
	GetProperty(0xa, VT_I4, (void*)&result);
	return result;
}

void ISSGroup::SetHeadBackColor(long propVal)
{
	SetProperty(0xa, VT_I4, propVal);
}

BOOL ISSGroup::GetHasHeadForeColor()
{
	BOOL result;
	GetProperty(0xb, VT_BOOL, (void*)&result);
	return result;
}

void ISSGroup::SetHasHeadForeColor(BOOL propVal)
{
	SetProperty(0xb, VT_BOOL, propVal);
}

BOOL ISSGroup::GetHasHeadBackColor()
{
	BOOL result;
	GetProperty(0xc, VT_BOOL, (void*)&result);
	return result;
}

void ISSGroup::SetHasHeadBackColor(BOOL propVal)
{
	SetProperty(0xc, VT_BOOL, propVal);
}

VARIANT ISSGroup::GetTagVariant()
{
	VARIANT result;
	GetProperty(0xd, VT_VARIANT, (void*)&result);
	return result;
}

void ISSGroup::SetTagVariant(const VARIANT& propVal)
{
	SetProperty(0xd, VT_VARIANT, &propVal);
}

CString ISSGroup::GetHeadStyleSet()
{
	CString result;
	GetProperty(0xe, VT_BSTR, (void*)&result);
	return result;
}

void ISSGroup::SetHeadStyleSet(LPCTSTR propVal)
{
	SetProperty(0xe, VT_BSTR, propVal);
}

CString ISSGroup::GetStyleSet()
{
	CString result;
	GetProperty(0xf, VT_BSTR, (void*)&result);
	return result;
}

void ISSGroup::SetStyleSet(LPCTSTR propVal)
{
	SetProperty(0xf, VT_BSTR, propVal);
}

short ISSGroup::GetPosition()
{
	short result;
	GetProperty(0x10, VT_I2, (void*)&result);
	return result;
}

void ISSGroup::SetPosition(short propVal)
{
	SetProperty(0x10, VT_I2, propVal);
}

LPDISPATCH ISSGroup::GetColumns()
{
	LPDISPATCH result;
	GetProperty(0x3e9, VT_DISPATCH, (void*)&result);
	return result;
}

void ISSGroup::SetColumns(LPDISPATCH propVal)
{
	SetProperty(0x3e9, VT_DISPATCH, propVal);
}

/////////////////////////////////////////////////////////////////////////////
// ISSGroup operations

short ISSGroup::ColPosition(short ColPos)
{
	short result;
	static BYTE parms[] =
		VTS_I2;
	InvokeHelper(0x12, DISPATCH_METHOD, VT_I2, (void*)&result, parms,
		ColPos);
	return result;
}


/////////////////////////////////////////////////////////////////////////////
// ISSActiveCell properties

long ISSActiveCell::GetTop()
{
	long result;
	GetProperty(0x1, VT_I4, (void*)&result);
	return result;
}

void ISSActiveCell::SetTop(long propVal)
{
	SetProperty(0x1, VT_I4, propVal);
}

long ISSActiveCell::GetLeft()
{
	long result;
	GetProperty(0x2, VT_I4, (void*)&result);
	return result;
}

void ISSActiveCell::SetLeft(long propVal)
{
	SetProperty(0x2, VT_I4, propVal);
}

long ISSActiveCell::GetHeight()
{
	long result;
	GetProperty(0x3, VT_I4, (void*)&result);
	return result;
}

void ISSActiveCell::SetHeight(long propVal)
{
	SetProperty(0x3, VT_I4, propVal);
}

long ISSActiveCell::GetWidth()
{
	long result;
	GetProperty(0x4, VT_I4, (void*)&result);
	return result;
}

void ISSActiveCell::SetWidth(long propVal)
{
	SetProperty(0x4, VT_I4, propVal);
}

CString ISSActiveCell::GetStyleSet()
{
	CString result;
	GetProperty(0x5, VT_BSTR, (void*)&result);
	return result;
}

void ISSActiveCell::SetStyleSet(LPCTSTR propVal)
{
	SetProperty(0x5, VT_BSTR, propVal);
}

CString ISSActiveCell::GetText()
{
	CString result;
	GetProperty(0x6, VT_BSTR, (void*)&result);
	return result;
}

void ISSActiveCell::SetText(LPCTSTR propVal)
{
	SetProperty(0x6, VT_BSTR, propVal);
}

VARIANT ISSActiveCell::GetValue()
{
	VARIANT result;
	GetProperty(0x7, VT_VARIANT, (void*)&result);
	return result;
}

void ISSActiveCell::SetValue(const VARIANT& propVal)
{
	SetProperty(0x7, VT_VARIANT, &propVal);
}

CString ISSActiveCell::GetSelText()
{
	CString result;
	GetProperty(0x8, VT_BSTR, (void*)&result);
	return result;
}

void ISSActiveCell::SetSelText(LPCTSTR propVal)
{
	SetProperty(0x8, VT_BSTR, propVal);
}

short ISSActiveCell::GetSelLength()
{
	short result;
	GetProperty(0x9, VT_I2, (void*)&result);
	return result;
}

void ISSActiveCell::SetSelLength(short propVal)
{
	SetProperty(0x9, VT_I2, propVal);
}

short ISSActiveCell::GetSelStart()
{
	short result;
	GetProperty(0xa, VT_I2, (void*)&result);
	return result;
}

void ISSActiveCell::SetSelStart(short propVal)
{
	SetProperty(0xa, VT_I2, propVal);
}

/////////////////////////////////////////////////////////////////////////////
// ISSActiveCell operations


/////////////////////////////////////////////////////////////////////////////
// ISSSelBookmarks properties

long ISSSelBookmarks::GetCount()
{
	long result;
	GetProperty(0x1, VT_I4, (void*)&result);
	return result;
}

void ISSSelBookmarks::SetCount(long propVal)
{
	SetProperty(0x1, VT_I4, propVal);
}

/////////////////////////////////////////////////////////////////////////////
// ISSSelBookmarks operations

void ISSSelBookmarks::Add(const VARIANT& Bookmark)
{
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x3, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 &Bookmark);
}

void ISSSelBookmarks::Remove(const VARIANT& Index)
{
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x4, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 &Index);
}

void ISSSelBookmarks::RemoveAll()
{
	InvokeHelper(0x5, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

LPUNKNOWN ISSSelBookmarks::_NewEnum()
{
	LPUNKNOWN result;
	InvokeHelper(0xfffffffc, DISPATCH_METHOD, VT_UNKNOWN, (void*)&result, NULL);
	return result;
}

VARIANT ISSSelBookmarks::GetItem(const VARIANT& Index)
{
	VARIANT result;
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x0, DISPATCH_PROPERTYGET, VT_VARIANT, (void*)&result, parms,
		&Index);
	return result;
}

void ISSSelBookmarks::SetItem(const VARIANT& Index, const VARIANT& newValue)
{
	static BYTE parms[] =
		VTS_VARIANT VTS_VARIANT;
	InvokeHelper(0x0, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 &Index, &newValue);
}


/////////////////////////////////////////////////////////////////////////////
// ISSStyleSets properties

short ISSStyleSets::GetCount()
{
	short result;
	GetProperty(0x1, VT_I2, (void*)&result);
	return result;
}

void ISSStyleSets::SetCount(short propVal)
{
	SetProperty(0x1, VT_I2, propVal);
}

/////////////////////////////////////////////////////////////////////////////
// ISSStyleSets operations

void ISSStyleSets::Add(LPCTSTR Name)
{
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x2, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Name);
}

void ISSStyleSets::Remove(const VARIANT& Index)
{
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x4, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 &Index);
}

void ISSStyleSets::RemoveAll()
{
	InvokeHelper(0x5, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

LPDISPATCH ISSStyleSets::GetItem(const VARIANT& Index)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x0, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, parms,
		&Index);
	return result;
}

void ISSStyleSets::SetItem(const VARIANT& Index, LPDISPATCH newValue)
{
	static BYTE parms[] =
		VTS_VARIANT VTS_DISPATCH;
	InvokeHelper(0x0, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 &Index, newValue);
}

LPUNKNOWN ISSStyleSets::_NewEnum()
{
	LPUNKNOWN result;
	InvokeHelper(0xfffffffc, DISPATCH_METHOD, VT_UNKNOWN, (void*)&result, NULL);
	return result;
}


/////////////////////////////////////////////////////////////////////////////
// ISSStyleSet properties

float ISSStyleSet::GetPictureMetaHeight()
{
	float result;
	GetProperty(0x1, VT_R4, (void*)&result);
	return result;
}

void ISSStyleSet::SetPictureMetaHeight(float propVal)
{
	SetProperty(0x1, VT_R4, propVal);
}

float ISSStyleSet::GetPictureMetaWidth()
{
	float result;
	GetProperty(0x2, VT_R4, (void*)&result);
	return result;
}

void ISSStyleSet::SetPictureMetaWidth(float propVal)
{
	SetProperty(0x2, VT_R4, propVal);
}

VARIANT ISSStyleSet::GetForeColor()
{
	VARIANT result;
	GetProperty(0x3, VT_VARIANT, (void*)&result);
	return result;
}

void ISSStyleSet::SetForeColor(const VARIANT& propVal)
{
	SetProperty(0x3, VT_VARIANT, &propVal);
}

VARIANT ISSStyleSet::GetBackColor()
{
	VARIANT result;
	GetProperty(0x4, VT_VARIANT, (void*)&result);
	return result;
}

void ISSStyleSet::SetBackColor(const VARIANT& propVal)
{
	SetProperty(0x4, VT_VARIANT, &propVal);
}

VARIANT ISSStyleSet::GetPicture()
{
	VARIANT result;
	GetProperty(0x6, VT_VARIANT, (void*)&result);
	return result;
}

void ISSStyleSet::SetPicture(const VARIANT& propVal)
{
	SetProperty(0x6, VT_VARIANT, &propVal);
}

VARIANT ISSStyleSet::GetAlignmentPicture()
{
	VARIANT result;
	GetProperty(0x7, VT_VARIANT, (void*)&result);
	return result;
}

void ISSStyleSet::SetAlignmentPicture(const VARIANT& propVal)
{
	SetProperty(0x7, VT_VARIANT, &propVal);
}

VARIANT ISSStyleSet::GetAlignmentText()
{
	VARIANT result;
	GetProperty(0x8, VT_VARIANT, (void*)&result);
	return result;
}

void ISSStyleSet::SetAlignmentText(const VARIANT& propVal)
{
	SetProperty(0x8, VT_VARIANT, &propVal);
}

VARIANT ISSStyleSet::GetFont()
{
	VARIANT result;
	GetProperty(0x5, VT_VARIANT, (void*)&result);
	return result;
}

void ISSStyleSet::SetFont(const VARIANT& propVal)
{
	SetProperty(0x5, VT_VARIANT, &propVal);
}

CString ISSStyleSet::GetName()
{
	CString result;
	GetProperty(0x65, VT_BSTR, (void*)&result);
	return result;
}

void ISSStyleSet::SetName(LPCTSTR propVal)
{
	SetProperty(0x65, VT_BSTR, propVal);
}

/////////////////////////////////////////////////////////////////////////////
// ISSStyleSet operations

void ISSStyleSet::Reset()
{
	InvokeHelper(0x9, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// ISSPrintInfo properties

CString ISSPrintInfo::GetPageHeader()
{
	CString result;
	GetProperty(0x1, VT_BSTR, (void*)&result);
	return result;
}

void ISSPrintInfo::SetPageHeader(LPCTSTR propVal)
{
	SetProperty(0x1, VT_BSTR, propVal);
}

CString ISSPrintInfo::GetPageFooter()
{
	CString result;
	GetProperty(0x2, VT_BSTR, (void*)&result);
	return result;
}

void ISSPrintInfo::SetPageFooter(LPCTSTR propVal)
{
	SetProperty(0x2, VT_BSTR, propVal);
}

short ISSPrintInfo::GetPageStart()
{
	short result;
	GetProperty(0x3, VT_I2, (void*)&result);
	return result;
}

void ISSPrintInfo::SetPageStart(short propVal)
{
	SetProperty(0x3, VT_I2, propVal);
}

short ISSPrintInfo::GetPageEnd()
{
	short result;
	GetProperty(0x4, VT_I2, (void*)&result);
	return result;
}

void ISSPrintInfo::SetPageEnd(short propVal)
{
	SetProperty(0x4, VT_I2, propVal);
}

short ISSPrintInfo::GetCopies()
{
	short result;
	GetProperty(0x5, VT_I2, (void*)&result);
	return result;
}

void ISSPrintInfo::SetCopies(short propVal)
{
	SetProperty(0x5, VT_I2, propVal);
}

long ISSPrintInfo::GetPrintGridLines()
{
	long result;
	GetProperty(0x6, VT_I4, (void*)&result);
	return result;
}

void ISSPrintInfo::SetPrintGridLines(long propVal)
{
	SetProperty(0x6, VT_I4, propVal);
}

float ISSPrintInfo::GetMarginTop()
{
	float result;
	GetProperty(0x7, VT_R4, (void*)&result);
	return result;
}

void ISSPrintInfo::SetMarginTop(float propVal)
{
	SetProperty(0x7, VT_R4, propVal);
}

float ISSPrintInfo::GetMarginBottom()
{
	float result;
	GetProperty(0x8, VT_R4, (void*)&result);
	return result;
}

void ISSPrintInfo::SetMarginBottom(float propVal)
{
	SetProperty(0x8, VT_R4, propVal);
}

float ISSPrintInfo::GetMarginLeft()
{
	float result;
	GetProperty(0x9, VT_R4, (void*)&result);
	return result;
}

void ISSPrintInfo::SetMarginLeft(float propVal)
{
	SetProperty(0x9, VT_R4, propVal);
}

float ISSPrintInfo::GetMarginRight()
{
	float result;
	GetProperty(0xa, VT_R4, (void*)&result);
	return result;
}

void ISSPrintInfo::SetMarginRight(float propVal)
{
	SetProperty(0xa, VT_R4, propVal);
}

BOOL ISSPrintInfo::GetCollate()
{
	BOOL result;
	GetProperty(0xb, VT_BOOL, (void*)&result);
	return result;
}

void ISSPrintInfo::SetCollate(BOOL propVal)
{
	SetProperty(0xb, VT_BOOL, propVal);
}

BOOL ISSPrintInfo::GetPortrait()
{
	BOOL result;
	GetProperty(0xc, VT_BOOL, (void*)&result);
	return result;
}

void ISSPrintInfo::SetPortrait(BOOL propVal)
{
	SetProperty(0xc, VT_BOOL, propVal);
}

long ISSPrintInfo::GetPrintColumnHeaders()
{
	long result;
	GetProperty(0xd, VT_I4, (void*)&result);
	return result;
}

void ISSPrintInfo::SetPrintColumnHeaders(long propVal)
{
	SetProperty(0xd, VT_I4, propVal);
}

BOOL ISSPrintInfo::GetPrintGroupHeaders()
{
	BOOL result;
	GetProperty(0xe, VT_BOOL, (void*)&result);
	return result;
}

void ISSPrintInfo::SetPrintGroupHeaders(BOOL propVal)
{
	SetProperty(0xe, VT_BOOL, propVal);
}

long ISSPrintInfo::GetPrintHeaders()
{
	long result;
	GetProperty(0xf, VT_I4, (void*)&result);
	return result;
}

void ISSPrintInfo::SetPrintHeaders(long propVal)
{
	SetProperty(0xf, VT_I4, propVal);
}

long ISSPrintInfo::GetPageBreakOnGroups()
{
	long result;
	GetProperty(0x10, VT_I4, (void*)&result);
	return result;
}

void ISSPrintInfo::SetPageBreakOnGroups(long propVal)
{
	SetProperty(0x10, VT_I4, propVal);
}

BOOL ISSPrintInfo::GetRowAutoSize()
{
	BOOL result;
	GetProperty(0x11, VT_BOOL, (void*)&result);
	return result;
}

void ISSPrintInfo::SetRowAutoSize(BOOL propVal)
{
	SetProperty(0x11, VT_BOOL, propVal);
}

short ISSPrintInfo::GetMaxLinesPerRow()
{
	short result;
	GetProperty(0x12, VT_I2, (void*)&result);
	return result;
}

void ISSPrintInfo::SetMaxLinesPerRow(short propVal)
{
	SetProperty(0x12, VT_I2, propVal);
}

BOOL ISSPrintInfo::GetPrintColors()
{
	BOOL result;
	GetProperty(0x13, VT_BOOL, (void*)&result);
	return result;
}

void ISSPrintInfo::SetPrintColors(BOOL propVal)
{
	SetProperty(0x13, VT_BOOL, propVal);
}

CString ISSPrintInfo::GetPrinterDeviceName()
{
	CString result;
	GetProperty(0x14, VT_BSTR, (void*)&result);
	return result;
}

void ISSPrintInfo::SetPrinterDeviceName(LPCTSTR propVal)
{
	SetProperty(0x14, VT_BSTR, propVal);
}

short ISSPrintInfo::GetPrinterDriverVer()
{
	short result;
	GetProperty(0x15, VT_I2, (void*)&result);
	return result;
}

void ISSPrintInfo::SetPrinterDriverVer(short propVal)
{
	SetProperty(0x15, VT_I2, propVal);
}

long ISSPrintInfo::GetDriverOverride()
{
	long result;
	GetProperty(0x16, VT_I4, (void*)&result);
	return result;
}

void ISSPrintInfo::SetDriverOverride(long propVal)
{
	SetProperty(0x16, VT_I4, propVal);
}

long ISSPrintInfo::GetClippingOverride()
{
	long result;
	GetProperty(0x17, VT_I4, (void*)&result);
	return result;
}

void ISSPrintInfo::SetClippingOverride(long propVal)
{
	SetProperty(0x17, VT_I4, propVal);
}

/////////////////////////////////////////////////////////////////////////////
// ISSPrintInfo operations


/////////////////////////////////////////////////////////////////////////////
// ISSRowBuffer properties

/////////////////////////////////////////////////////////////////////////////
// ISSRowBuffer operations

long ISSRowBuffer::GetRowCount()
{
	long result;
	InvokeHelper(0x60020000, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

void ISSRowBuffer::SetRowCount(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x60020000, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

short ISSRowBuffer::GetColumnCount()
{
	short result;
	InvokeHelper(0x60020002, DISPATCH_PROPERTYGET, VT_I2, (void*)&result, NULL);
	return result;
}

void ISSRowBuffer::SetColumnCount(short nNewValue)
{
	static BYTE parms[] =
		VTS_I2;
	InvokeHelper(0x60020002, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

VARIANT ISSRowBuffer::GetBookmark(long Row)
{
	VARIANT result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x60020004, DISPATCH_PROPERTYGET, VT_VARIANT, (void*)&result, parms,
		Row);
	return result;
}

void ISSRowBuffer::SetBookmark(long Row, const VARIANT& newValue)
{
	static BYTE parms[] =
		VTS_I4 VTS_VARIANT;
	InvokeHelper(0x60020004, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 Row, &newValue);
}

CString ISSRowBuffer::GetColumnName(short Col)
{
	CString result;
	static BYTE parms[] =
		VTS_I2;
	InvokeHelper(0x60020006, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, parms,
		Col);
	return result;
}

void ISSRowBuffer::SetColumnName(short Col, LPCTSTR lpszNewValue)
{
	static BYTE parms[] =
		VTS_I2 VTS_BSTR;
	InvokeHelper(0x60020006, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 Col, lpszNewValue);
}

VARIANT ISSRowBuffer::GetValue(long Row, short Col)
{
	VARIANT result;
	static BYTE parms[] =
		VTS_I4 VTS_I2;
	InvokeHelper(0x60020008, DISPATCH_PROPERTYGET, VT_VARIANT, (void*)&result, parms,
		Row, Col);
	return result;
}

void ISSRowBuffer::SetValue(long Row, short Col, const VARIANT& newValue)
{
	static BYTE parms[] =
		VTS_I4 VTS_I2 VTS_VARIANT;
	InvokeHelper(0x60020008, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 Row, Col, &newValue);
}

short ISSRowBuffer::GetReadType()
{
	short result;
	InvokeHelper(0x6002000a, DISPATCH_PROPERTYGET, VT_I2, (void*)&result, NULL);
	return result;
}

void ISSRowBuffer::SetReadType(short nNewValue)
{
	static BYTE parms[] =
		VTS_I2;
	InvokeHelper(0x6002000a, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}


/////////////////////////////////////////////////////////////////////////////
// RowBufferDisp properties

long RowBufferDisp::GetRowCount()
{
	long result;
	GetProperty(0x1, VT_I4, (void*)&result);
	return result;
}

void RowBufferDisp::SetRowCount(long propVal)
{
	SetProperty(0x1, VT_I4, propVal);
}

short RowBufferDisp::GetColumnCount()
{
	short result;
	GetProperty(0x2, VT_I2, (void*)&result);
	return result;
}

void RowBufferDisp::SetColumnCount(short propVal)
{
	SetProperty(0x2, VT_I2, propVal);
}

short RowBufferDisp::GetReadType()
{
	short result;
	GetProperty(0x6, VT_I2, (void*)&result);
	return result;
}

void RowBufferDisp::SetReadType(short propVal)
{
	SetProperty(0x6, VT_I2, propVal);
}

/////////////////////////////////////////////////////////////////////////////
// RowBufferDisp operations

VARIANT RowBufferDisp::GetBookmark(long Row)
{
	VARIANT result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x3, DISPATCH_PROPERTYGET, VT_VARIANT, (void*)&result, parms,
		Row);
	return result;
}

void RowBufferDisp::SetBookmark(long Row, const VARIANT& newValue)
{
	static BYTE parms[] =
		VTS_I4 VTS_VARIANT;
	InvokeHelper(0x3, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 Row, &newValue);
}

CString RowBufferDisp::GetColumnName(short Col)
{
	CString result;
	static BYTE parms[] =
		VTS_I2;
	InvokeHelper(0x4, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, parms,
		Col);
	return result;
}

void RowBufferDisp::SetColumnName(short Col, LPCTSTR lpszNewValue)
{
	static BYTE parms[] =
		VTS_I2 VTS_BSTR;
	InvokeHelper(0x4, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 Col, lpszNewValue);
}

VARIANT RowBufferDisp::GetValue(long Row, short Col)
{
	VARIANT result;
	static BYTE parms[] =
		VTS_I4 VTS_I2;
	InvokeHelper(0x5, DISPATCH_PROPERTYGET, VT_VARIANT, (void*)&result, parms,
		Row, Col);
	return result;
}

void RowBufferDisp::SetValue(long Row, short Col, const VARIANT& newValue)
{
	static BYTE parms[] =
		VTS_I4 VTS_I2 VTS_VARIANT;
	InvokeHelper(0x5, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 Row, Col, &newValue);
}


/////////////////////////////////////////////////////////////////////////////
// _DSSDBGrid properties

LPUNKNOWN _DSSDBGrid::GetDataSource()
{
	LPUNKNOWN result;
	GetProperty(0x1, VT_UNKNOWN, (void*)&result);
	return result;
}

void _DSSDBGrid::SetDataSource(LPUNKNOWN propVal)
{
	SetProperty(0x1, VT_UNKNOWN, propVal);
}

OLE_COLOR _DSSDBGrid::GetBackColor()
{
	OLE_COLOR result;
	GetProperty(DISPID_BACKCOLOR, VT_I4, (void*)&result);
	return result;
}

void _DSSDBGrid::SetBackColor(OLE_COLOR propVal)
{
	SetProperty(DISPID_BACKCOLOR, VT_I4, propVal);
}

CString _DSSDBGrid::GetCaption()
{
	CString result;
	GetProperty(DISPID_CAPTION, VT_BSTR, (void*)&result);
	return result;
}

void _DSSDBGrid::SetCaption(LPCTSTR propVal)
{
	SetProperty(DISPID_CAPTION, VT_BSTR, propVal);
}

BOOL _DSSDBGrid::GetEnabled()
{
	BOOL result;
	GetProperty(DISPID_ENABLED, VT_BOOL, (void*)&result);
	return result;
}

void _DSSDBGrid::SetEnabled(BOOL propVal)
{
	SetProperty(DISPID_ENABLED, VT_BOOL, propVal);
}

LPFONTDISP _DSSDBGrid::GetFont()
{
	LPFONTDISP result;
	GetProperty(DISPID_FONT, VT_DISPATCH, (void*)&result);
	return result;
}

void _DSSDBGrid::SetFont(LPFONTDISP propVal)
{
	SetProperty(DISPID_FONT, VT_DISPATCH, propVal);
}

OLE_COLOR _DSSDBGrid::GetForeColor()
{
	OLE_COLOR result;
	GetProperty(DISPID_FORECOLOR, VT_I4, (void*)&result);
	return result;
}

void _DSSDBGrid::SetForeColor(OLE_COLOR propVal)
{
	SetProperty(DISPID_FORECOLOR, VT_I4, propVal);
}

OLE_HANDLE _DSSDBGrid::GetHWnd()
{
	OLE_HANDLE result;
	GetProperty(DISPID_HWND, VT_I4, (void*)&result);
	return result;
}

void _DSSDBGrid::SetHWnd(OLE_HANDLE propVal)
{
	SetProperty(DISPID_HWND, VT_I4, propVal);
}

unsigned long _DSSDBGrid::GetBackColorEven()
{
	unsigned long result;
	GetProperty(0x2, VT_I4, (void*)&result);
	return result;
}

void _DSSDBGrid::SetBackColorEven(unsigned long propVal)
{
	SetProperty(0x2, VT_I4, propVal);
}

unsigned long _DSSDBGrid::GetBackColorOdd()
{
	unsigned long result;
	GetProperty(0x3, VT_I4, (void*)&result);
	return result;
}

void _DSSDBGrid::SetBackColorOdd(unsigned long propVal)
{
	SetProperty(0x3, VT_I4, propVal);
}

unsigned long _DSSDBGrid::GetForeColorEven()
{
	unsigned long result;
	GetProperty(0x4, VT_I4, (void*)&result);
	return result;
}

void _DSSDBGrid::SetForeColorEven(unsigned long propVal)
{
	SetProperty(0x4, VT_I4, propVal);
}

unsigned long _DSSDBGrid::GetForeColorOdd()
{
	unsigned long result;
	GetProperty(0x5, VT_I4, (void*)&result);
	return result;
}

void _DSSDBGrid::SetForeColorOdd(unsigned long propVal)
{
	SetProperty(0x5, VT_I4, propVal);
}

long _DSSDBGrid::GetCaptionAlignment()
{
	long result;
	GetProperty(0x6, VT_I4, (void*)&result);
	return result;
}

void _DSSDBGrid::SetCaptionAlignment(long propVal)
{
	SetProperty(0x6, VT_I4, propVal);
}

long _DSSDBGrid::GetDividerType()
{
	long result;
	GetProperty(0x7, VT_I4, (void*)&result);
	return result;
}

void _DSSDBGrid::SetDividerType(long propVal)
{
	SetProperty(0x7, VT_I4, propVal);
}

long _DSSDBGrid::GetScrollBars()
{
	long result;
	GetProperty(0x8, VT_I4, (void*)&result);
	return result;
}

void _DSSDBGrid::SetScrollBars(long propVal)
{
	SetProperty(0x8, VT_I4, propVal);
}

BOOL _DSSDBGrid::GetAllowAddNew()
{
	BOOL result;
	GetProperty(0x9, VT_BOOL, (void*)&result);
	return result;
}

void _DSSDBGrid::SetAllowAddNew(BOOL propVal)
{
	SetProperty(0x9, VT_BOOL, propVal);
}

BOOL _DSSDBGrid::GetAllowDelete()
{
	BOOL result;
	GetProperty(0xa, VT_BOOL, (void*)&result);
	return result;
}

void _DSSDBGrid::SetAllowDelete(BOOL propVal)
{
	SetProperty(0xa, VT_BOOL, propVal);
}

BOOL _DSSDBGrid::GetAllowUpdate()
{
	BOOL result;
	GetProperty(0xb, VT_BOOL, (void*)&result);
	return result;
}

void _DSSDBGrid::SetAllowUpdate(BOOL propVal)
{
	SetProperty(0xb, VT_BOOL, propVal);
}

BOOL _DSSDBGrid::GetAllowRowSizing()
{
	BOOL result;
	GetProperty(0xc, VT_BOOL, (void*)&result);
	return result;
}

void _DSSDBGrid::SetAllowRowSizing(BOOL propVal)
{
	SetProperty(0xc, VT_BOOL, propVal);
}

BOOL _DSSDBGrid::GetAllowGroupSizing()
{
	BOOL result;
	GetProperty(0xd, VT_BOOL, (void*)&result);
	return result;
}

void _DSSDBGrid::SetAllowGroupSizing(BOOL propVal)
{
	SetProperty(0xd, VT_BOOL, propVal);
}

BOOL _DSSDBGrid::GetAllowColumnSizing()
{
	BOOL result;
	GetProperty(0xe, VT_BOOL, (void*)&result);
	return result;
}

void _DSSDBGrid::SetAllowColumnSizing(BOOL propVal)
{
	SetProperty(0xe, VT_BOOL, propVal);
}

BOOL _DSSDBGrid::GetRecordSelectors()
{
	BOOL result;
	GetProperty(0xf, VT_BOOL, (void*)&result);
	return result;
}

void _DSSDBGrid::SetRecordSelectors(BOOL propVal)
{
	SetProperty(0xf, VT_BOOL, propVal);
}

BOOL _DSSDBGrid::GetGroupHeaders()
{
	BOOL result;
	GetProperty(0x10, VT_BOOL, (void*)&result);
	return result;
}

void _DSSDBGrid::SetGroupHeaders(BOOL propVal)
{
	SetProperty(0x10, VT_BOOL, propVal);
}

BOOL _DSSDBGrid::GetColumnHeaders()
{
	BOOL result;
	GetProperty(0x11, VT_BOOL, (void*)&result);
	return result;
}

void _DSSDBGrid::SetColumnHeaders(BOOL propVal)
{
	SetProperty(0x11, VT_BOOL, propVal);
}

short _DSSDBGrid::GetHeadLines()
{
	short result;
	GetProperty(0x12, VT_I2, (void*)&result);
	return result;
}

void _DSSDBGrid::SetHeadLines(short propVal)
{
	SetProperty(0x12, VT_I2, propVal);
}

LPDISPATCH _DSSDBGrid::GetHeadFont()
{
	LPDISPATCH result;
	GetProperty(0x13, VT_DISPATCH, (void*)&result);
	return result;
}

void _DSSDBGrid::SetHeadFont(LPDISPATCH propVal)
{
	SetProperty(0x13, VT_DISPATCH, propVal);
}

long _DSSDBGrid::GetHeadFont3D()
{
	long result;
	GetProperty(0x14, VT_I4, (void*)&result);
	return result;
}

void _DSSDBGrid::SetHeadFont3D(long propVal)
{
	SetProperty(0x14, VT_I4, propVal);
}

short _DSSDBGrid::GetCol()
{
	short result;
	GetProperty(0x15, VT_I2, (void*)&result);
	return result;
}

void _DSSDBGrid::SetCol(short propVal)
{
	SetProperty(0x15, VT_I2, propVal);
}

short _DSSDBGrid::GetRow()
{
	short result;
	GetProperty(0x16, VT_I2, (void*)&result);
	return result;
}

void _DSSDBGrid::SetRow(short propVal)
{
	SetProperty(0x16, VT_I2, propVal);
}

long _DSSDBGrid::GetDataMode()
{
	long result;
	GetProperty(0x17, VT_I4, (void*)&result);
	return result;
}

void _DSSDBGrid::SetDataMode(long propVal)
{
	SetProperty(0x17, VT_I4, propVal);
}

short _DSSDBGrid::GetVisibleCols()
{
	short result;
	GetProperty(0x18, VT_I2, (void*)&result);
	return result;
}

void _DSSDBGrid::SetVisibleCols(short propVal)
{
	SetProperty(0x18, VT_I2, propVal);
}

short _DSSDBGrid::GetVisibleRows()
{
	short result;
	GetProperty(0x19, VT_I2, (void*)&result);
	return result;
}

void _DSSDBGrid::SetVisibleRows(short propVal)
{
	SetProperty(0x19, VT_I2, propVal);
}

short _DSSDBGrid::GetLeftCol()
{
	short result;
	GetProperty(0x1a, VT_I2, (void*)&result);
	return result;
}

void _DSSDBGrid::SetLeftCol(short propVal)
{
	SetProperty(0x1a, VT_I2, propVal);
}

short _DSSDBGrid::GetCols()
{
	short result;
	GetProperty(0x1b, VT_I2, (void*)&result);
	return result;
}

void _DSSDBGrid::SetCols(short propVal)
{
	SetProperty(0x1b, VT_I2, propVal);
}

long _DSSDBGrid::GetRows()
{
	long result;
	GetProperty(0x1c, VT_I4, (void*)&result);
	return result;
}

void _DSSDBGrid::SetRows(long propVal)
{
	SetProperty(0x1c, VT_I4, propVal);
}

CString _DSSDBGrid::GetFieldDelimiter()
{
	CString result;
	GetProperty(0x1d, VT_BSTR, (void*)&result);
	return result;
}

void _DSSDBGrid::SetFieldDelimiter(LPCTSTR propVal)
{
	SetProperty(0x1d, VT_BSTR, propVal);
}

CString _DSSDBGrid::GetFieldSeparator()
{
	CString result;
	GetProperty(0x1e, VT_BSTR, (void*)&result);
	return result;
}

void _DSSDBGrid::SetFieldSeparator(LPCTSTR propVal)
{
	SetProperty(0x1e, VT_BSTR, propVal);
}

long _DSSDBGrid::GetFont3D()
{
	long result;
	GetProperty(0x1f, VT_I4, (void*)&result);
	return result;
}

void _DSSDBGrid::SetFont3D(long propVal)
{
	SetProperty(0x1f, VT_I4, propVal);
}

long _DSSDBGrid::GetDividerStyle()
{
	long result;
	GetProperty(0x20, VT_I4, (void*)&result);
	return result;
}

void _DSSDBGrid::SetDividerStyle(long propVal)
{
	SetProperty(0x20, VT_I4, propVal);
}

BOOL _DSSDBGrid::GetUseExactRowCount()
{
	BOOL result;
	GetProperty(0x21, VT_BOOL, (void*)&result);
	return result;
}

void _DSSDBGrid::SetUseExactRowCount(BOOL propVal)
{
	SetProperty(0x21, VT_BOOL, propVal);
}

unsigned long _DSSDBGrid::GetBevelColorFace()
{
	unsigned long result;
	GetProperty(0x22, VT_I4, (void*)&result);
	return result;
}

void _DSSDBGrid::SetBevelColorFace(unsigned long propVal)
{
	SetProperty(0x22, VT_I4, propVal);
}

unsigned long _DSSDBGrid::GetBevelColorFrame()
{
	unsigned long result;
	GetProperty(0x23, VT_I4, (void*)&result);
	return result;
}

void _DSSDBGrid::SetBevelColorFrame(unsigned long propVal)
{
	SetProperty(0x23, VT_I4, propVal);
}

unsigned long _DSSDBGrid::GetBevelColorHighlight()
{
	unsigned long result;
	GetProperty(0x24, VT_I4, (void*)&result);
	return result;
}

void _DSSDBGrid::SetBevelColorHighlight(unsigned long propVal)
{
	SetProperty(0x24, VT_I4, propVal);
}

unsigned long _DSSDBGrid::GetBevelColorShadow()
{
	unsigned long result;
	GetProperty(0x25, VT_I4, (void*)&result);
	return result;
}

void _DSSDBGrid::SetBevelColorShadow(unsigned long propVal)
{
	SetProperty(0x25, VT_I4, propVal);
}

long _DSSDBGrid::GetBevelColorScheme()
{
	long result;
	GetProperty(0x26, VT_I4, (void*)&result);
	return result;
}

void _DSSDBGrid::SetBevelColorScheme(long propVal)
{
	SetProperty(0x26, VT_I4, propVal);
}

short _DSSDBGrid::GetGroupHeadLines()
{
	short result;
	GetProperty(0x27, VT_I2, (void*)&result);
	return result;
}

void _DSSDBGrid::SetGroupHeadLines(short propVal)
{
	SetProperty(0x27, VT_I2, propVal);
}

VARIANT _DSSDBGrid::GetBookmark()
{
	VARIANT result;
	GetProperty(0x28, VT_VARIANT, (void*)&result);
	return result;
}

void _DSSDBGrid::SetBookmark(const VARIANT& propVal)
{
	SetProperty(0x28, VT_VARIANT, &propVal);
}

BOOL _DSSDBGrid::GetAllowDragDrop()
{
	BOOL result;
	GetProperty(0x29, VT_BOOL, (void*)&result);
	return result;
}

void _DSSDBGrid::SetAllowDragDrop(BOOL propVal)
{
	SetProperty(0x29, VT_BOOL, propVal);
}

VARIANT _DSSDBGrid::GetFirstRow()
{
	VARIANT result;
	GetProperty(0x2a, VT_VARIANT, (void*)&result);
	return result;
}

void _DSSDBGrid::SetFirstRow(const VARIANT& propVal)
{
	SetProperty(0x2a, VT_VARIANT, &propVal);
}

float _DSSDBGrid::GetDefColWidth()
{
	float result;
	GetProperty(0x2b, VT_R4, (void*)&result);
	return result;
}

void _DSSDBGrid::SetDefColWidth(float propVal)
{
	SetProperty(0x2b, VT_R4, propVal);
}

float _DSSDBGrid::GetRowHeight()
{
	float result;
	GetProperty(0x2c, VT_R4, (void*)&result);
	return result;
}

void _DSSDBGrid::SetRowHeight(float propVal)
{
	SetProperty(0x2c, VT_R4, propVal);
}

short _DSSDBGrid::GetVisibleGrps()
{
	short result;
	GetProperty(0x2d, VT_I2, (void*)&result);
	return result;
}

void _DSSDBGrid::SetVisibleGrps(short propVal)
{
	SetProperty(0x2d, VT_I2, propVal);
}

short _DSSDBGrid::GetLeftGrp()
{
	short result;
	GetProperty(0x2e, VT_I2, (void*)&result);
	return result;
}

void _DSSDBGrid::SetLeftGrp(short propVal)
{
	SetProperty(0x2e, VT_I2, propVal);
}

BOOL _DSSDBGrid::GetCheckBox3D()
{
	BOOL result;
	GetProperty(0x2f, VT_BOOL, (void*)&result);
	return result;
}

void _DSSDBGrid::SetCheckBox3D(BOOL propVal)
{
	SetProperty(0x2f, VT_BOOL, propVal);
}

short _DSSDBGrid::GetLevelCount()
{
	short result;
	GetProperty(0x30, VT_I2, (void*)&result);
	return result;
}

void _DSSDBGrid::SetLevelCount(short propVal)
{
	SetProperty(0x30, VT_I2, propVal);
}

BOOL _DSSDBGrid::GetRedraw()
{
	BOOL result;
	GetProperty(0x31, VT_BOOL, (void*)&result);
	return result;
}

void _DSSDBGrid::SetRedraw(BOOL propVal)
{
	SetProperty(0x31, VT_BOOL, propVal);
}

BOOL _DSSDBGrid::GetComboDroppedDown()
{
	BOOL result;
	GetProperty(0x32, VT_BOOL, (void*)&result);
	return result;
}

void _DSSDBGrid::SetComboDroppedDown(BOOL propVal)
{
	SetProperty(0x32, VT_BOOL, propVal);
}

BOOL _DSSDBGrid::GetAllowGroupMoving()
{
	BOOL result;
	GetProperty(0x33, VT_BOOL, (void*)&result);
	return result;
}

void _DSSDBGrid::SetAllowGroupMoving(BOOL propVal)
{
	SetProperty(0x33, VT_BOOL, propVal);
}

BOOL _DSSDBGrid::GetAllowGroupSwapping()
{
	BOOL result;
	GetProperty(0x34, VT_BOOL, (void*)&result);
	return result;
}

void _DSSDBGrid::SetAllowGroupSwapping(BOOL propVal)
{
	SetProperty(0x34, VT_BOOL, propVal);
}

long _DSSDBGrid::GetAllowColumnMoving()
{
	long result;
	GetProperty(0x35, VT_I4, (void*)&result);
	return result;
}

void _DSSDBGrid::SetAllowColumnMoving(long propVal)
{
	SetProperty(0x35, VT_I4, propVal);
}

long _DSSDBGrid::GetAllowColumnSwapping()
{
	long result;
	GetProperty(0x36, VT_I4, (void*)&result);
	return result;
}

void _DSSDBGrid::SetAllowColumnSwapping(long propVal)
{
	SetProperty(0x36, VT_I4, propVal);
}

float _DSSDBGrid::GetResizeHeight()
{
	float result;
	GetProperty(0x37, VT_R4, (void*)&result);
	return result;
}

void _DSSDBGrid::SetResizeHeight(float propVal)
{
	SetProperty(0x37, VT_R4, propVal);
}

float _DSSDBGrid::GetResizeWidth()
{
	float result;
	GetProperty(0x38, VT_R4, (void*)&result);
	return result;
}

void _DSSDBGrid::SetResizeWidth(float propVal)
{
	SetProperty(0x38, VT_R4, propVal);
}

VARIANT _DSSDBGrid::GetTagVariant()
{
	VARIANT result;
	GetProperty(0x39, VT_VARIANT, (void*)&result);
	return result;
}

void _DSSDBGrid::SetTagVariant(const VARIANT& propVal)
{
	SetProperty(0x39, VT_VARIANT, &propVal);
}

short _DSSDBGrid::GetSplitterPos()
{
	short result;
	GetProperty(0x3a, VT_I2, (void*)&result);
	return result;
}

void _DSSDBGrid::SetSplitterPos(short propVal)
{
	SetProperty(0x3a, VT_I2, propVal);
}

BOOL _DSSDBGrid::GetSplitterVisible()
{
	BOOL result;
	GetProperty(0x3b, VT_BOOL, (void*)&result);
	return result;
}

void _DSSDBGrid::SetSplitterVisible(BOOL propVal)
{
	SetProperty(0x3b, VT_BOOL, propVal);
}

long _DSSDBGrid::GetSelectTypeCol()
{
	long result;
	GetProperty(0x3c, VT_I4, (void*)&result);
	return result;
}

void _DSSDBGrid::SetSelectTypeCol(long propVal)
{
	SetProperty(0x3c, VT_I4, propVal);
}

long _DSSDBGrid::GetSelectTypeRow()
{
	long result;
	GetProperty(0x3d, VT_I4, (void*)&result);
	return result;
}

void _DSSDBGrid::SetSelectTypeRow(long propVal)
{
	SetProperty(0x3d, VT_I4, propVal);
}

BOOL _DSSDBGrid::GetSelectByCell()
{
	BOOL result;
	GetProperty(0x3e, VT_BOOL, (void*)&result);
	return result;
}

void _DSSDBGrid::SetSelectByCell(BOOL propVal)
{
	SetProperty(0x3e, VT_BOOL, propVal);
}

CString _DSSDBGrid::GetHeadStyleSet()
{
	CString result;
	GetProperty(0x3f, VT_BSTR, (void*)&result);
	return result;
}

void _DSSDBGrid::SetHeadStyleSet(LPCTSTR propVal)
{
	SetProperty(0x3f, VT_BSTR, propVal);
}

CString _DSSDBGrid::GetStyleSet()
{
	CString result;
	GetProperty(0x40, VT_BSTR, (void*)&result);
	return result;
}

void _DSSDBGrid::SetStyleSet(LPCTSTR propVal)
{
	SetProperty(0x40, VT_BSTR, propVal);
}

BOOL _DSSDBGrid::GetMultiLine()
{
	BOOL result;
	GetProperty(0x41, VT_BOOL, (void*)&result);
	return result;
}

void _DSSDBGrid::SetMultiLine(BOOL propVal)
{
	SetProperty(0x41, VT_BOOL, propVal);
}

CString _DSSDBGrid::GetActiveRowStyleSet()
{
	CString result;
	GetProperty(0x42, VT_BSTR, (void*)&result);
	return result;
}

void _DSSDBGrid::SetActiveRowStyleSet(LPCTSTR propVal)
{
	SetProperty(0x42, VT_BSTR, propVal);
}

LPDISPATCH _DSSDBGrid::GetPictureButton()
{
	LPDISPATCH result;
	GetProperty(0x43, VT_DISPATCH, (void*)&result);
	return result;
}

void _DSSDBGrid::SetPictureButton(LPDISPATCH propVal)
{
	SetProperty(0x43, VT_DISPATCH, propVal);
}

LPDISPATCH _DSSDBGrid::GetPictureComboButton()
{
	LPDISPATCH result;
	GetProperty(0x44, VT_DISPATCH, (void*)&result);
	return result;
}

void _DSSDBGrid::SetPictureComboButton(LPDISPATCH propVal)
{
	SetProperty(0x44, VT_DISPATCH, propVal);
}

LPDISPATCH _DSSDBGrid::GetPictureRecordSelectors()
{
	LPDISPATCH result;
	GetProperty(0x45, VT_DISPATCH, (void*)&result);
	return result;
}

void _DSSDBGrid::SetPictureRecordSelectors(LPDISPATCH propVal)
{
	SetProperty(0x45, VT_DISPATCH, propVal);
}

BOOL _DSSDBGrid::GetAllowColumnShrinking()
{
	BOOL result;
	GetProperty(0x46, VT_BOOL, (void*)&result);
	return result;
}

void _DSSDBGrid::SetAllowColumnShrinking(BOOL propVal)
{
	SetProperty(0x46, VT_BOOL, propVal);
}

BOOL _DSSDBGrid::GetAllowGroupShrinking()
{
	BOOL result;
	GetProperty(0x47, VT_BOOL, (void*)&result);
	return result;
}

void _DSSDBGrid::SetAllowGroupShrinking(BOOL propVal)
{
	SetProperty(0x47, VT_BOOL, propVal);
}

BOOL _DSSDBGrid::GetDataChanged()
{
	BOOL result;
	GetProperty(0x48, VT_BOOL, (void*)&result);
	return result;
}

void _DSSDBGrid::SetDataChanged(BOOL propVal)
{
	SetProperty(0x48, VT_BOOL, propVal);
}

BOOL _DSSDBGrid::GetRowChanged()
{
	BOOL result;
	GetProperty(0x49, VT_BOOL, (void*)&result);
	return result;
}

void _DSSDBGrid::SetRowChanged(BOOL propVal)
{
	SetProperty(0x49, VT_BOOL, propVal);
}

BOOL _DSSDBGrid::GetBalloonHelp()
{
	BOOL result;
	GetProperty(0x4a, VT_BOOL, (void*)&result);
	return result;
}

void _DSSDBGrid::SetBalloonHelp(BOOL propVal)
{
	SetProperty(0x4a, VT_BOOL, propVal);
}

long _DSSDBGrid::GetCellNavigation()
{
	long result;
	GetProperty(0x4b, VT_I4, (void*)&result);
	return result;
}

void _DSSDBGrid::SetCellNavigation(long propVal)
{
	SetProperty(0x4b, VT_I4, propVal);
}

long _DSSDBGrid::GetRowNavigation()
{
	long result;
	GetProperty(0x4c, VT_I4, (void*)&result);
	return result;
}

void _DSSDBGrid::SetRowNavigation(long propVal)
{
	SetProperty(0x4c, VT_I4, propVal);
}

short _DSSDBGrid::GetGrp()
{
	short result;
	GetProperty(0x4d, VT_I2, (void*)&result);
	return result;
}

void _DSSDBGrid::SetGrp(short propVal)
{
	SetProperty(0x4d, VT_I2, propVal);
}

long _DSSDBGrid::GetRowSelectionStyle()
{
	long result;
	GetProperty(0x4e, VT_I4, (void*)&result);
	return result;
}

void _DSSDBGrid::SetRowSelectionStyle(long propVal)
{
	SetProperty(0x4e, VT_I4, propVal);
}

long _DSSDBGrid::GetMaxSelectedRows()
{
	long result;
	GetProperty(0x4f, VT_I4, (void*)&result);
	return result;
}

void _DSSDBGrid::SetMaxSelectedRows(long propVal)
{
	SetProperty(0x4f, VT_I4, propVal);
}

BOOL _DSSDBGrid::GetDroppedDown()
{
	BOOL result;
	GetProperty(0x96, VT_BOOL, (void*)&result);
	return result;
}

void _DSSDBGrid::SetDroppedDown(BOOL propVal)
{
	SetProperty(0x96, VT_BOOL, propVal);
}

long _DSSDBGrid::GetBorderStyle()
{
	long result;
	GetProperty(0x12c, VT_I4, (void*)&result);
	return result;
}

void _DSSDBGrid::SetBorderStyle(long propVal)
{
	SetProperty(0x12c, VT_I4, propVal);
}

long _DSSDBGrid::GetHwndEdit()
{
	long result;
	GetProperty(0x12d, VT_I4, (void*)&result);
	return result;
}

void _DSSDBGrid::SetHwndEdit(long propVal)
{
	SetProperty(0x12d, VT_I4, propVal);
}

LPDISPATCH _DSSDBGrid::GetColumns()
{
	LPDISPATCH result;
	GetProperty(0x3e9, VT_DISPATCH, (void*)&result);
	return result;
}

void _DSSDBGrid::SetColumns(LPDISPATCH propVal)
{
	SetProperty(0x3e9, VT_DISPATCH, propVal);
}

LPDISPATCH _DSSDBGrid::GetGroups()
{
	LPDISPATCH result;
	GetProperty(0x3ea, VT_DISPATCH, (void*)&result);
	return result;
}

void _DSSDBGrid::SetGroups(LPDISPATCH propVal)
{
	SetProperty(0x3ea, VT_DISPATCH, propVal);
}

LPDISPATCH _DSSDBGrid::GetSelBookmarks()
{
	LPDISPATCH result;
	GetProperty(0x3eb, VT_DISPATCH, (void*)&result);
	return result;
}

void _DSSDBGrid::SetSelBookmarks(LPDISPATCH propVal)
{
	SetProperty(0x3eb, VT_DISPATCH, propVal);
}

LPDISPATCH _DSSDBGrid::GetStyleSets()
{
	LPDISPATCH result;
	GetProperty(0x3ec, VT_DISPATCH, (void*)&result);
	return result;
}

void _DSSDBGrid::SetStyleSets(LPDISPATCH propVal)
{
	SetProperty(0x3ec, VT_DISPATCH, propVal);
}

BOOL _DSSDBGrid::GetUseDefaults()
{
	BOOL result;
	GetProperty(0x3ed, VT_BOOL, (void*)&result);
	return result;
}

void _DSSDBGrid::SetUseDefaults(BOOL propVal)
{
	SetProperty(0x3ed, VT_BOOL, propVal);
}

long _DSSDBGrid::GetTabNavigation()
{
	long result;
	GetProperty(0x3ee, VT_I4, (void*)&result);
	return result;
}

void _DSSDBGrid::SetTabNavigation(long propVal)
{
	SetProperty(0x3ee, VT_I4, propVal);
}

LPDISPATCH _DSSDBGrid::GetPageHeaderFont()
{
	LPDISPATCH result;
	GetProperty(0x3ef, VT_DISPATCH, (void*)&result);
	return result;
}

void _DSSDBGrid::SetPageHeaderFont(LPDISPATCH propVal)
{
	SetProperty(0x3ef, VT_DISPATCH, propVal);
}

LPDISPATCH _DSSDBGrid::GetPageFooterFont()
{
	LPDISPATCH result;
	GetProperty(0x3f0, VT_DISPATCH, (void*)&result);
	return result;
}

void _DSSDBGrid::SetPageFooterFont(LPDISPATCH propVal)
{
	SetProperty(0x3f0, VT_DISPATCH, propVal);
}

/////////////////////////////////////////////////////////////////////////////
// _DSSDBGrid operations

void _DSSDBGrid::DoClick()
{
	InvokeHelper(DISPID_DOCLICK, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void _DSSDBGrid::AddItem(LPCTSTR Item, const VARIANT& Index)
{
	static BYTE parms[] =
		VTS_BSTR VTS_VARIANT;
	InvokeHelper(0x52, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Item, &Index);
}

void _DSSDBGrid::RemoveItem(long Index)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x53, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Index);
}

VARIANT _DSSDBGrid::RowBookmark(long RowNum)
{
	VARIANT result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x54, DISPATCH_METHOD, VT_VARIANT, (void*)&result, parms,
		RowNum);
	return result;
}

short _DSSDBGrid::RowContaining(float Y)
{
	short result;
	static BYTE parms[] =
		VTS_R4;
	InvokeHelper(0x55, DISPATCH_METHOD, VT_I2, (void*)&result, parms,
		Y);
	return result;
}

float _DSSDBGrid::RowTop(short RowNum)
{
	float result;
	static BYTE parms[] =
		VTS_I2;
	InvokeHelper(0x56, DISPATCH_METHOD, VT_R4, (void*)&result, parms,
		RowNum);
	return result;
}

void _DSSDBGrid::Scroll(short Cols, long Rows)
{
	static BYTE parms[] =
		VTS_I2 VTS_I4;
	InvokeHelper(0x57, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Cols, Rows);
}

void _DSSDBGrid::ReBind()
{
	InvokeHelper(0x58, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

short _DSSDBGrid::ColContaining(float X, const VARIANT& Y)
{
	short result;
	static BYTE parms[] =
		VTS_R4 VTS_VARIANT;
	InvokeHelper(0x59, DISPATCH_METHOD, VT_I2, (void*)&result, parms,
		X, &Y);
	return result;
}

short _DSSDBGrid::GrpContaining(float X)
{
	short result;
	static BYTE parms[] =
		VTS_R4;
	InvokeHelper(0x5a, DISPATCH_METHOD, VT_I2, (void*)&result, parms,
		X);
	return result;
}

VARIANT _DSSDBGrid::GetBookmark(long RowNum)
{
	VARIANT result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x5b, DISPATCH_METHOD, VT_VARIANT, (void*)&result, parms,
		RowNum);
	return result;
}

void _DSSDBGrid::RemoveAll()
{
	InvokeHelper(0x5d, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void _DSSDBGrid::MoveFirst()
{
	InvokeHelper(0x5e, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void _DSSDBGrid::MovePrevious()
{
	InvokeHelper(0x5f, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void _DSSDBGrid::MoveNext()
{
	InvokeHelper(0x60, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void _DSSDBGrid::MoveLast()
{
	InvokeHelper(0x61, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void _DSSDBGrid::MoveRecords(long Records)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x62, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Records);
}

BOOL _DSSDBGrid::IsItemInList()
{
	BOOL result;
	InvokeHelper(0x68, DISPATCH_METHOD, VT_BOOL, (void*)&result, NULL);
	return result;
}

long _DSSDBGrid::WhereIs(float X, float Y)
{
	long result;
	static BYTE parms[] =
		VTS_R4 VTS_R4;
	InvokeHelper(0x6a, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		X, Y);
	return result;
}

LPDISPATCH _DSSDBGrid::ActiveCell()
{
	LPDISPATCH result;
	InvokeHelper(0x6b, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, NULL);
	return result;
}

short _DSSDBGrid::ColPosition(short ColPos)
{
	short result;
	static BYTE parms[] =
		VTS_I2;
	InvokeHelper(0x6c, DISPATCH_METHOD, VT_I2, (void*)&result, parms,
		ColPos);
	return result;
}

short _DSSDBGrid::GrpPosition(short GrpPos)
{
	short result;
	static BYTE parms[] =
		VTS_I2;
	InvokeHelper(0x6d, DISPATCH_METHOD, VT_I2, (void*)&result, parms,
		GrpPos);
	return result;
}

void _DSSDBGrid::Reset()
{
	InvokeHelper(0x6e, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

BOOL _DSSDBGrid::IsAddRow()
{
	BOOL result;
	InvokeHelper(0x6f, DISPATCH_METHOD, VT_BOOL, (void*)&result, NULL);
	return result;
}

VARIANT _DSSDBGrid::AddItemBookmark(long RowIndex)
{
	VARIANT result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x70, DISPATCH_METHOD, VT_VARIANT, (void*)&result, parms,
		RowIndex);
	return result;
}

long _DSSDBGrid::AddItemRowIndex(const VARIANT& Bookmark)
{
	long result;
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x71, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		&Bookmark);
	return result;
}

void _DSSDBGrid::Refresh()
{
	InvokeHelper(DISPID_REFRESH, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void _DSSDBGrid::Update()
{
	InvokeHelper(0x72, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void _DSSDBGrid::AboutBox()
{
	InvokeHelper(0xfffffdd8, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

CString _DSSDBGrid::DeleteSelected()
{
	CString result;
	InvokeHelper(0x12e, DISPATCH_METHOD, VT_BSTR, (void*)&result, NULL);
	return result;
}

void _DSSDBGrid::AddNew()
{
	InvokeHelper(0x12f, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void _DSSDBGrid::CancelUpdate()
{
	InvokeHelper(0x130, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void _DSSDBGrid::Export(long Type, long Flags, LPCTSTR ExportToFile, const VARIANT& HTMLTemplate, const VARIANT& OutputFileField)
{
	static BYTE parms[] =
		VTS_I4 VTS_I4 VTS_BSTR VTS_VARIANT VTS_VARIANT;
	InvokeHelper(0x131, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Type, Flags, ExportToFile, &HTMLTemplate, &OutputFileField);
}

void _DSSDBGrid::PrintData(long Flags, BOOL ShowSetupDialog, BOOL ShowPrintDialog, const VARIANT& Reserved)
{
	static BYTE parms[] =
		VTS_I4 VTS_BOOL VTS_BOOL VTS_VARIANT;
	InvokeHelper(0x132, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Flags, ShowSetupDialog, ShowPrintDialog, &Reserved);
}

void _DSSDBGrid::SaveLayout(LPCTSTR FileName, long Flags)
{
	static BYTE parms[] =
		VTS_BSTR VTS_I4;
	InvokeHelper(0x44c, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 FileName, Flags);
}

void _DSSDBGrid::LoadLayout(LPCTSTR FileName)
{
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x44d, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 FileName);
}


/////////////////////////////////////////////////////////////////////////////
// _DSSDBGridEvents properties

/////////////////////////////////////////////////////////////////////////////
// _DSSDBGridEvents operations

void _DSSDBGridEvents::InitColumnProps()
{
	InvokeHelper(0x1, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void _DSSDBGridEvents::KeyDown(short* KeyCode, short Shift)
{
	static BYTE parms[] =
		VTS_PI2 VTS_I2;
	InvokeHelper(DISPID_KEYDOWN, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 KeyCode, Shift);
}

void _DSSDBGridEvents::Click()
{
	InvokeHelper(DISPID_CLICK, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void _DSSDBGridEvents::DblClick()
{
	InvokeHelper(DISPID_DBLCLICK, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void _DSSDBGridEvents::KeyPress(short* KeyAscii)
{
	static BYTE parms[] =
		VTS_PI2;
	InvokeHelper(DISPID_KEYPRESS, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 KeyAscii);
}

void _DSSDBGridEvents::KeyUp(short* KeyCode, short Shift)
{
	static BYTE parms[] =
		VTS_PI2 VTS_I2;
	InvokeHelper(DISPID_KEYUP, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 KeyCode, Shift);
}

void _DSSDBGridEvents::MouseDown(short Button, short Shift, long X, long Y)
{
	static BYTE parms[] =
		VTS_I2 VTS_I2 VTS_I4 VTS_I4;
	InvokeHelper(DISPID_MOUSEDOWN, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Button, Shift, X, Y);
}

void _DSSDBGridEvents::MouseMove(short Button, short Shift, long X, long Y)
{
	static BYTE parms[] =
		VTS_I2 VTS_I2 VTS_I4 VTS_I4;
	InvokeHelper(DISPID_MOUSEMOVE, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Button, Shift, X, Y);
}

void _DSSDBGridEvents::MouseUp(short Button, short Shift, long X, long Y)
{
	static BYTE parms[] =
		VTS_I2 VTS_I2 VTS_I4 VTS_I4;
	InvokeHelper(DISPID_MOUSEUP, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Button, Shift, X, Y);
}

void _DSSDBGridEvents::BtnClick()
{
	InvokeHelper(0x2, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void _DSSDBGridEvents::AfterColUpdate(short ColIndex)
{
	static BYTE parms[] =
		VTS_I2;
	InvokeHelper(0x3, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 ColIndex);
}

void _DSSDBGridEvents::BeforeColUpdate(short ColIndex, const VARIANT& OldValue, short* Cancel)
{
	static BYTE parms[] =
		VTS_I2 VTS_VARIANT VTS_PI2;
	InvokeHelper(0x4, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 ColIndex, &OldValue, Cancel);
}

void _DSSDBGridEvents::BeforeInsert(short* Cancel)
{
	static BYTE parms[] =
		VTS_PI2;
	InvokeHelper(0x5, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Cancel);
}

void _DSSDBGridEvents::BeforeUpdate(short* Cancel)
{
	static BYTE parms[] =
		VTS_PI2;
	InvokeHelper(0x6, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Cancel);
}

void _DSSDBGridEvents::ColResize(short ColIndex, short* Cancel)
{
	static BYTE parms[] =
		VTS_I2 VTS_PI2;
	InvokeHelper(0x7, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 ColIndex, Cancel);
}

void _DSSDBGridEvents::RowResize(short* Cancel)
{
	static BYTE parms[] =
		VTS_PI2;
	InvokeHelper(0x8, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Cancel);
}

void _DSSDBGridEvents::HeadClick(short ColIndex)
{
	static BYTE parms[] =
		VTS_I2;
	InvokeHelper(0x9, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 ColIndex);
}

void _DSSDBGridEvents::RowColChange(const VARIANT& LastRow, short LastCol)
{
	static BYTE parms[] =
		VTS_VARIANT VTS_I2;
	InvokeHelper(0xa, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 &LastRow, LastCol);
}

void _DSSDBGridEvents::Scroll(short* Cancel)
{
	static BYTE parms[] =
		VTS_PI2;
	InvokeHelper(0xb, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Cancel);
}

void _DSSDBGridEvents::Change()
{
	InvokeHelper(0xc, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void _DSSDBGridEvents::GrpHeadClick(short GrpIndex)
{
	static BYTE parms[] =
		VTS_I2;
	InvokeHelper(0xd, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 GrpIndex);
}

void _DSSDBGridEvents::ComboDropDown()
{
	InvokeHelper(0xe, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void _DSSDBGridEvents::SplitterMove(short* Cancel)
{
	static BYTE parms[] =
		VTS_PI2;
	InvokeHelper(0xf, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Cancel);
}

void _DSSDBGridEvents::GrpResize(short GrpIndex, short* Cancel)
{
	static BYTE parms[] =
		VTS_I2 VTS_PI2;
	InvokeHelper(0x10, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 GrpIndex, Cancel);
}

void _DSSDBGridEvents::ComboCloseUp()
{
	InvokeHelper(0x11, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void _DSSDBGridEvents::RowLoaded(const VARIANT& Bookmark)
{
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x12, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 &Bookmark);
}

void _DSSDBGridEvents::AfterDelete(short* RtnDispErrMsg)
{
	static BYTE parms[] =
		VTS_PI2;
	InvokeHelper(0x13, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 RtnDispErrMsg);
}

void _DSSDBGridEvents::BeforeDelete(short* Cancel, short* DispPromptMsg)
{
	static BYTE parms[] =
		VTS_PI2 VTS_PI2;
	InvokeHelper(0x14, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Cancel, DispPromptMsg);
}

void _DSSDBGridEvents::AfterUpdate(short* RtnDispErrMsg)
{
	static BYTE parms[] =
		VTS_PI2;
	InvokeHelper(0x15, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 RtnDispErrMsg);
}

void _DSSDBGridEvents::AfterInsert(short* RtnDispErrMsg)
{
	static BYTE parms[] =
		VTS_PI2;
	InvokeHelper(0x16, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 RtnDispErrMsg);
}

void _DSSDBGridEvents::UnboundReadData(LPDISPATCH RowBuf, VARIANT* StartLocation, BOOL ReadPriorRows)
{
	static BYTE parms[] =
		VTS_DISPATCH VTS_PVARIANT VTS_BOOL;
	InvokeHelper(0x17, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 RowBuf, StartLocation, ReadPriorRows);
}

void _DSSDBGridEvents::UnboundAddData(LPDISPATCH RowBuf, VARIANT* NewRowBookmark)
{
	static BYTE parms[] =
		VTS_DISPATCH VTS_PVARIANT;
	InvokeHelper(0x18, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 RowBuf, NewRowBookmark);
}

void _DSSDBGridEvents::UnboundWriteData(LPDISPATCH RowBuf, VARIANT* WriteLocation)
{
	static BYTE parms[] =
		VTS_DISPATCH VTS_PVARIANT;
	InvokeHelper(0x19, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 RowBuf, WriteLocation);
}

void _DSSDBGridEvents::UnboundDeleteRow(VARIANT* Bookmark)
{
	static BYTE parms[] =
		VTS_PVARIANT;
	InvokeHelper(0x1a, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Bookmark);
}

void _DSSDBGridEvents::UnboundPositionData(VARIANT* StartLocation, long NumberOfRowsToMove, VARIANT* NewLocation)
{
	static BYTE parms[] =
		VTS_PVARIANT VTS_I4 VTS_PVARIANT;
	InvokeHelper(0x1b, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 StartLocation, NumberOfRowsToMove, NewLocation);
}

void _DSSDBGridEvents::ColMove(short ColIndex, short NewPos, short* Cancel)
{
	static BYTE parms[] =
		VTS_I2 VTS_I2 VTS_PI2;
	InvokeHelper(0x1c, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 ColIndex, NewPos, Cancel);
}

void _DSSDBGridEvents::ColSwap(short ColIndex, short NewPos, short* Cancel)
{
	static BYTE parms[] =
		VTS_I2 VTS_I2 VTS_PI2;
	InvokeHelper(0x1d, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 ColIndex, NewPos, Cancel);
}

void _DSSDBGridEvents::GrpMove(short GrpIndex, short NewPos, short* Cancel)
{
	static BYTE parms[] =
		VTS_I2 VTS_I2 VTS_PI2;
	InvokeHelper(0x1e, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 GrpIndex, NewPos, Cancel);
}

void _DSSDBGridEvents::GrpSwap(short GrpIndex, short NewPos, short* Cancel)
{
	static BYTE parms[] =
		VTS_I2 VTS_I2 VTS_PI2;
	InvokeHelper(0x1f, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 GrpIndex, NewPos, Cancel);
}

void _DSSDBGridEvents::SelChange(short SelType, short* Cancel, short* DispSelRowOverflow)
{
	static BYTE parms[] =
		VTS_I2 VTS_PI2 VTS_PI2;
	InvokeHelper(0x20, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 SelType, Cancel, DispSelRowOverflow);
}

void _DSSDBGridEvents::UpdateError(short ColIndex, BSTR* Text, short* ErrCode, BSTR* ErrString, short* Cancel)
{
	static BYTE parms[] =
		VTS_I2 VTS_PBSTR VTS_PI2 VTS_PBSTR VTS_PI2;
	InvokeHelper(0x21, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 ColIndex, Text, ErrCode, ErrString, Cancel);
}

void _DSSDBGridEvents::BeforeRowColChange(short* Cancel)
{
	static BYTE parms[] =
		VTS_PI2;
	InvokeHelper(0x22, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Cancel);
}

void _DSSDBGridEvents::ScrollAfter()
{
	InvokeHelper(0x23, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void _DSSDBGridEvents::Error(short DataError, short* Response)
{
	static BYTE parms[] =
		VTS_I2 VTS_PI2;
	InvokeHelper(0x24, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 DataError, Response);
}

void _DSSDBGridEvents::AfterPosChanged(short WhatChanged, short NewIndex)
{
	static BYTE parms[] =
		VTS_I2 VTS_I2;
	InvokeHelper(0x25, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 WhatChanged, NewIndex);
}

void _DSSDBGridEvents::RowExport(const VARIANT& Bookmark, short* Cancel)
{
	static BYTE parms[] =
		VTS_VARIANT VTS_PI2;
	InvokeHelper(0x26, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 &Bookmark, Cancel);
}

void _DSSDBGridEvents::RowPrint(const VARIANT& Bookmark, long PageNumber, short* Cancel)
{
	static BYTE parms[] =
		VTS_VARIANT VTS_I4 VTS_PI2;
	InvokeHelper(0x27, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 &Bookmark, PageNumber, Cancel);
}

void _DSSDBGridEvents::PrintBegin(LPDISPATCH ssPrintInfo)
{
	static BYTE parms[] =
		VTS_DISPATCH;
	InvokeHelper(0x28, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 ssPrintInfo);
}

void _DSSDBGridEvents::PrintError(long PrintError, short* Response)
{
	static BYTE parms[] =
		VTS_I4 VTS_PI2;
	InvokeHelper(0x29, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 PrintError, Response);
}

void _DSSDBGridEvents::PrintInitialize(LPDISPATCH ssPrintInfo)
{
	static BYTE parms[] =
		VTS_DISPATCH;
	InvokeHelper(0x2a, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 ssPrintInfo);
}

void _DSSDBGridEvents::ValidationError(BSTR* InvalidText, short StartPosition)
{
	static BYTE parms[] =
		VTS_PBSTR VTS_I2;
	InvokeHelper(0x3e8, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 InvalidText, StartPosition);
}


/////////////////////////////////////////////////////////////////////////////
// _DSSDBCombo properties

CString _DSSDBCombo::GetText()
{
	CString result;
	GetProperty(DISPID_TEXT, VT_BSTR, (void*)&result);
	return result;
}

void _DSSDBCombo::SetText(LPCTSTR propVal)
{
	SetProperty(DISPID_TEXT, VT_BSTR, propVal);
}

CString _DSSDBCombo::Get_Text()
{
	CString result;
	GetProperty(0x0, VT_BSTR, (void*)&result);
	return result;
}

void _DSSDBCombo::Set_Text(LPCTSTR propVal)
{
	SetProperty(0x0, VT_BSTR, propVal);
}

OLE_COLOR _DSSDBCombo::GetBackColor()
{
	OLE_COLOR result;
	GetProperty(DISPID_BACKCOLOR, VT_I4, (void*)&result);
	return result;
}

void _DSSDBCombo::SetBackColor(OLE_COLOR propVal)
{
	SetProperty(DISPID_BACKCOLOR, VT_I4, propVal);
}

BOOL _DSSDBCombo::GetEnabled()
{
	BOOL result;
	GetProperty(DISPID_ENABLED, VT_BOOL, (void*)&result);
	return result;
}

void _DSSDBCombo::SetEnabled(BOOL propVal)
{
	SetProperty(DISPID_ENABLED, VT_BOOL, propVal);
}

LPFONTDISP _DSSDBCombo::GetFont()
{
	LPFONTDISP result;
	GetProperty(DISPID_FONT, VT_DISPATCH, (void*)&result);
	return result;
}

void _DSSDBCombo::SetFont(LPFONTDISP propVal)
{
	SetProperty(DISPID_FONT, VT_DISPATCH, propVal);
}

OLE_COLOR _DSSDBCombo::GetForeColor()
{
	OLE_COLOR result;
	GetProperty(DISPID_FORECOLOR, VT_I4, (void*)&result);
	return result;
}

void _DSSDBCombo::SetForeColor(OLE_COLOR propVal)
{
	SetProperty(DISPID_FORECOLOR, VT_I4, propVal);
}

OLE_HANDLE _DSSDBCombo::GetHWnd()
{
	OLE_HANDLE result;
	GetProperty(DISPID_HWND, VT_I4, (void*)&result);
	return result;
}

void _DSSDBCombo::SetHWnd(OLE_HANDLE propVal)
{
	SetProperty(DISPID_HWND, VT_I4, propVal);
}

VARIANT _DSSDBCombo::GetValue()
{
	VARIANT result;
	GetProperty(0x2e, VT_VARIANT, (void*)&result);
	return result;
}

void _DSSDBCombo::SetValue(const VARIANT& propVal)
{
	SetProperty(0x2e, VT_VARIANT, &propVal);
}

LPUNKNOWN _DSSDBCombo::GetDataSourceList()
{
	LPUNKNOWN result;
	GetProperty(0x2b, VT_UNKNOWN, (void*)&result);
	return result;
}

void _DSSDBCombo::SetDataSourceList(LPUNKNOWN propVal)
{
	SetProperty(0x2b, VT_UNKNOWN, propVal);
}

unsigned long _DSSDBCombo::GetBevelColorFrame()
{
	unsigned long result;
	GetProperty(0x3, VT_I4, (void*)&result);
	return result;
}

void _DSSDBCombo::SetBevelColorFrame(unsigned long propVal)
{
	SetProperty(0x3, VT_I4, propVal);
}

unsigned long _DSSDBCombo::GetBevelColorHighlight()
{
	unsigned long result;
	GetProperty(0x4, VT_I4, (void*)&result);
	return result;
}

void _DSSDBCombo::SetBevelColorHighlight(unsigned long propVal)
{
	SetProperty(0x4, VT_I4, propVal);
}

unsigned long _DSSDBCombo::GetBevelColorShadow()
{
	unsigned long result;
	GetProperty(0x5, VT_I4, (void*)&result);
	return result;
}

void _DSSDBCombo::SetBevelColorShadow(unsigned long propVal)
{
	SetProperty(0x5, VT_I4, propVal);
}

long _DSSDBCombo::GetBevelColorScheme()
{
	long result;
	GetProperty(0x6, VT_I4, (void*)&result);
	return result;
}

void _DSSDBCombo::SetBevelColorScheme(long propVal)
{
	SetProperty(0x6, VT_I4, propVal);
}

unsigned long _DSSDBCombo::GetBackColorEven()
{
	unsigned long result;
	GetProperty(0x7, VT_I4, (void*)&result);
	return result;
}

void _DSSDBCombo::SetBackColorEven(unsigned long propVal)
{
	SetProperty(0x7, VT_I4, propVal);
}

unsigned long _DSSDBCombo::GetBackColorOdd()
{
	unsigned long result;
	GetProperty(0x8, VT_I4, (void*)&result);
	return result;
}

void _DSSDBCombo::SetBackColorOdd(unsigned long propVal)
{
	SetProperty(0x8, VT_I4, propVal);
}

VARIANT _DSSDBCombo::GetBookmark()
{
	VARIANT result;
	GetProperty(0x9, VT_VARIANT, (void*)&result);
	return result;
}

void _DSSDBCombo::SetBookmark(const VARIANT& propVal)
{
	SetProperty(0x9, VT_VARIANT, &propVal);
}

short _DSSDBCombo::GetCols()
{
	short result;
	GetProperty(0xa, VT_I2, (void*)&result);
	return result;
}

void _DSSDBCombo::SetCols(short propVal)
{
	SetProperty(0xa, VT_I2, propVal);
}

CString _DSSDBCombo::GetDataFieldList()
{
	CString result;
	GetProperty(0xb, VT_BSTR, (void*)&result);
	return result;
}

void _DSSDBCombo::SetDataFieldList(LPCTSTR propVal)
{
	SetProperty(0xb, VT_BSTR, propVal);
}

long _DSSDBCombo::GetDataMode()
{
	long result;
	GetProperty(0xc, VT_I4, (void*)&result);
	return result;
}

void _DSSDBCombo::SetDataMode(long propVal)
{
	SetProperty(0xc, VT_I4, propVal);
}

long _DSSDBCombo::GetDividerStyle()
{
	long result;
	GetProperty(0xd, VT_I4, (void*)&result);
	return result;
}

void _DSSDBCombo::SetDividerStyle(long propVal)
{
	SetProperty(0xd, VT_I4, propVal);
}

long _DSSDBCombo::GetDividerType()
{
	long result;
	GetProperty(0xe, VT_I4, (void*)&result);
	return result;
}

void _DSSDBCombo::SetDividerType(long propVal)
{
	SetProperty(0xe, VT_I4, propVal);
}

BOOL _DSSDBCombo::GetDroppedDown()
{
	BOOL result;
	GetProperty(0xf, VT_BOOL, (void*)&result);
	return result;
}

void _DSSDBCombo::SetDroppedDown(BOOL propVal)
{
	SetProperty(0xf, VT_BOOL, propVal);
}

long _DSSDBCombo::GetFont3D()
{
	long result;
	GetProperty(0x10, VT_I4, (void*)&result);
	return result;
}

void _DSSDBCombo::SetFont3D(long propVal)
{
	SetProperty(0x10, VT_I4, propVal);
}

unsigned long _DSSDBCombo::GetForeColorOdd()
{
	unsigned long result;
	GetProperty(0x11, VT_I4, (void*)&result);
	return result;
}

void _DSSDBCombo::SetForeColorOdd(unsigned long propVal)
{
	SetProperty(0x11, VT_I4, propVal);
}

BOOL _DSSDBCombo::GetColumnHeaders()
{
	BOOL result;
	GetProperty(0x12, VT_BOOL, (void*)&result);
	return result;
}

void _DSSDBCombo::SetColumnHeaders(BOOL propVal)
{
	SetProperty(0x12, VT_BOOL, propVal);
}

unsigned long _DSSDBCombo::GetForeColorEven()
{
	unsigned long result;
	GetProperty(0x13, VT_I4, (void*)&result);
	return result;
}

void _DSSDBCombo::SetForeColorEven(unsigned long propVal)
{
	SetProperty(0x13, VT_I4, propVal);
}

float _DSSDBCombo::GetDefColWidth()
{
	float result;
	GetProperty(0x14, VT_R4, (void*)&result);
	return result;
}

void _DSSDBCombo::SetDefColWidth(float propVal)
{
	SetProperty(0x14, VT_R4, propVal);
}

BOOL _DSSDBCombo::GetGroupHeaders()
{
	BOOL result;
	GetProperty(0x15, VT_BOOL, (void*)&result);
	return result;
}

void _DSSDBCombo::SetGroupHeaders(BOOL propVal)
{
	SetProperty(0x15, VT_BOOL, propVal);
}

short _DSSDBCombo::GetGroupHeadLines()
{
	short result;
	GetProperty(0x16, VT_I2, (void*)&result);
	return result;
}

void _DSSDBCombo::SetGroupHeadLines(short propVal)
{
	SetProperty(0x16, VT_I2, propVal);
}

LPDISPATCH _DSSDBCombo::GetHeadFont()
{
	LPDISPATCH result;
	GetProperty(0x17, VT_DISPATCH, (void*)&result);
	return result;
}

void _DSSDBCombo::SetHeadFont(LPDISPATCH propVal)
{
	SetProperty(0x17, VT_DISPATCH, propVal);
}

long _DSSDBCombo::GetHeadFont3D()
{
	long result;
	GetProperty(0x18, VT_I4, (void*)&result);
	return result;
}

void _DSSDBCombo::SetHeadFont3D(long propVal)
{
	SetProperty(0x18, VT_I4, propVal);
}

short _DSSDBCombo::GetHeadLines()
{
	short result;
	GetProperty(0x19, VT_I2, (void*)&result);
	return result;
}

void _DSSDBCombo::SetHeadLines(short propVal)
{
	SetProperty(0x19, VT_I2, propVal);
}

short _DSSDBCombo::GetLeftCol()
{
	short result;
	GetProperty(0x1a, VT_I2, (void*)&result);
	return result;
}

void _DSSDBCombo::SetLeftCol(short propVal)
{
	SetProperty(0x1a, VT_I2, propVal);
}

short _DSSDBCombo::GetLeftGrp()
{
	short result;
	GetProperty(0x1b, VT_I2, (void*)&result);
	return result;
}

void _DSSDBCombo::SetLeftGrp(short propVal)
{
	SetProperty(0x1b, VT_I2, propVal);
}

short _DSSDBCombo::GetLevelCount()
{
	short result;
	GetProperty(0x1c, VT_I2, (void*)&result);
	return result;
}

void _DSSDBCombo::SetLevelCount(short propVal)
{
	SetProperty(0x1c, VT_I2, propVal);
}

BOOL _DSSDBCombo::GetListAutoPosition()
{
	BOOL result;
	GetProperty(0x1d, VT_BOOL, (void*)&result);
	return result;
}

void _DSSDBCombo::SetListAutoPosition(BOOL propVal)
{
	SetProperty(0x1d, VT_BOOL, propVal);
}

BOOL _DSSDBCombo::GetListAutoValidate()
{
	BOOL result;
	GetProperty(0x1e, VT_BOOL, (void*)&result);
	return result;
}

void _DSSDBCombo::SetListAutoValidate(BOOL propVal)
{
	SetProperty(0x1e, VT_BOOL, propVal);
}

BOOL _DSSDBCombo::GetListWidthAutoSize()
{
	BOOL result;
	GetProperty(0x1f, VT_BOOL, (void*)&result);
	return result;
}

void _DSSDBCombo::SetListWidthAutoSize(BOOL propVal)
{
	SetProperty(0x1f, VT_BOOL, propVal);
}

short _DSSDBCombo::GetMaxDropDownItems()
{
	short result;
	GetProperty(0x20, VT_I2, (void*)&result);
	return result;
}

void _DSSDBCombo::SetMaxDropDownItems(short propVal)
{
	SetProperty(0x20, VT_I2, propVal);
}

short _DSSDBCombo::GetMinDropDownItems()
{
	short result;
	GetProperty(0x21, VT_I2, (void*)&result);
	return result;
}

void _DSSDBCombo::SetMinDropDownItems(short propVal)
{
	SetProperty(0x21, VT_I2, propVal);
}

BOOL _DSSDBCombo::GetRedraw()
{
	BOOL result;
	GetProperty(0x22, VT_BOOL, (void*)&result);
	return result;
}

void _DSSDBCombo::SetRedraw(BOOL propVal)
{
	SetProperty(0x22, VT_BOOL, propVal);
}

short _DSSDBCombo::GetRow()
{
	short result;
	GetProperty(0x23, VT_I2, (void*)&result);
	return result;
}

void _DSSDBCombo::SetRow(short propVal)
{
	SetProperty(0x23, VT_I2, propVal);
}

long _DSSDBCombo::GetScrollBars()
{
	long result;
	GetProperty(0x24, VT_I4, (void*)&result);
	return result;
}

void _DSSDBCombo::SetScrollBars(long propVal)
{
	SetProperty(0x24, VT_I4, propVal);
}

VARIANT _DSSDBCombo::GetTagVariant()
{
	VARIANT result;
	GetProperty(0x25, VT_VARIANT, (void*)&result);
	return result;
}

void _DSSDBCombo::SetTagVariant(const VARIANT& propVal)
{
	SetProperty(0x25, VT_VARIANT, &propVal);
}

short _DSSDBCombo::GetVisibleCols()
{
	short result;
	GetProperty(0x26, VT_I2, (void*)&result);
	return result;
}

void _DSSDBCombo::SetVisibleCols(short propVal)
{
	SetProperty(0x26, VT_I2, propVal);
}

short _DSSDBCombo::GetVisibleRows()
{
	short result;
	GetProperty(0x27, VT_I2, (void*)&result);
	return result;
}

void _DSSDBCombo::SetVisibleRows(short propVal)
{
	SetProperty(0x27, VT_I2, propVal);
}

short _DSSDBCombo::GetVisibleGrps()
{
	short result;
	GetProperty(0x28, VT_I2, (void*)&result);
	return result;
}

void _DSSDBCombo::SetVisibleGrps(short propVal)
{
	SetProperty(0x28, VT_I2, propVal);
}

float _DSSDBCombo::GetRowHeight()
{
	float result;
	GetProperty(0x29, VT_R4, (void*)&result);
	return result;
}

void _DSSDBCombo::SetRowHeight(float propVal)
{
	SetProperty(0x29, VT_R4, propVal);
}

long _DSSDBCombo::GetRows()
{
	long result;
	GetProperty(0x2a, VT_I4, (void*)&result);
	return result;
}

void _DSSDBCombo::SetRows(long propVal)
{
	SetProperty(0x2a, VT_I4, propVal);
}

unsigned long _DSSDBCombo::GetBevelColorFace()
{
	unsigned long result;
	GetProperty(0x2, VT_I4, (void*)&result);
	return result;
}

void _DSSDBCombo::SetBevelColorFace(unsigned long propVal)
{
	SetProperty(0x2, VT_I4, propVal);
}

BOOL _DSSDBCombo::GetAllowInput()
{
	BOOL result;
	GetProperty(0x2c, VT_BOOL, (void*)&result);
	return result;
}

void _DSSDBCombo::SetAllowInput(BOOL propVal)
{
	SetProperty(0x2c, VT_BOOL, propVal);
}

BOOL _DSSDBCombo::GetAllowNull()
{
	BOOL result;
	GetProperty(0x2d, VT_BOOL, (void*)&result);
	return result;
}

void _DSSDBCombo::SetAllowNull(BOOL propVal)
{
	SetProperty(0x2d, VT_BOOL, propVal);
}

short _DSSDBCombo::GetBevelWidth()
{
	short result;
	GetProperty(0x1, VT_I2, (void*)&result);
	return result;
}

void _DSSDBCombo::SetBevelWidth(short propVal)
{
	SetProperty(0x1, VT_I2, propVal);
}

BOOL _DSSDBCombo::GetAutoRestore()
{
	BOOL result;
	GetProperty(0x2f, VT_BOOL, (void*)&result);
	return result;
}

void _DSSDBCombo::SetAutoRestore(BOOL propVal)
{
	SetProperty(0x2f, VT_BOOL, propVal);
}

LPDISPATCH _DSSDBCombo::GetPictureDropDown()
{
	LPDISPATCH result;
	GetProperty(0x30, VT_DISPATCH, (void*)&result);
	return result;
}

void _DSSDBCombo::SetPictureDropDown(LPDISPATCH propVal)
{
	SetProperty(0x30, VT_DISPATCH, propVal);
}

long _DSSDBCombo::GetMousePointer()
{
	long result;
	GetProperty(0x31, VT_I4, (void*)&result);
	return result;
}

void _DSSDBCombo::SetMousePointer(long propVal)
{
	SetProperty(0x31, VT_I4, propVal);
}

LPDISPATCH _DSSDBCombo::GetMouseIcon()
{
	LPDISPATCH result;
	GetProperty(0x32, VT_DISPATCH, (void*)&result);
	return result;
}

void _DSSDBCombo::SetMouseIcon(LPDISPATCH propVal)
{
	SetProperty(0x32, VT_DISPATCH, propVal);
}

long _DSSDBCombo::GetBevelType()
{
	long result;
	GetProperty(0x33, VT_I4, (void*)&result);
	return result;
}

void _DSSDBCombo::SetBevelType(long propVal)
{
	SetProperty(0x33, VT_I4, propVal);
}

CString _DSSDBCombo::GetFieldDelimiter()
{
	CString result;
	GetProperty(0x34, VT_BSTR, (void*)&result);
	return result;
}

void _DSSDBCombo::SetFieldDelimiter(LPCTSTR propVal)
{
	SetProperty(0x34, VT_BSTR, propVal);
}

CString _DSSDBCombo::GetFieldSeparator()
{
	CString result;
	GetProperty(0x35, VT_BSTR, (void*)&result);
	return result;
}

void _DSSDBCombo::SetFieldSeparator(LPCTSTR propVal)
{
	SetProperty(0x35, VT_BSTR, propVal);
}

CString _DSSDBCombo::GetTextFormat()
{
	CString result;
	GetProperty(0x36, VT_BSTR, (void*)&result);
	return result;
}

void _DSSDBCombo::SetTextFormat(LPCTSTR propVal)
{
	SetProperty(0x36, VT_BSTR, propVal);
}

BOOL _DSSDBCombo::GetUseExactRowCount()
{
	BOOL result;
	GetProperty(0x37, VT_BOOL, (void*)&result);
	return result;
}

void _DSSDBCombo::SetUseExactRowCount(BOOL propVal)
{
	SetProperty(0x37, VT_BOOL, propVal);
}

float _DSSDBCombo::GetListWidth()
{
	float result;
	GetProperty(0x38, VT_R4, (void*)&result);
	return result;
}

void _DSSDBCombo::SetListWidth(float propVal)
{
	SetProperty(0x38, VT_R4, propVal);
}

short _DSSDBCombo::GetSelStart()
{
	short result;
	GetProperty(0x39, VT_I2, (void*)&result);
	return result;
}

void _DSSDBCombo::SetSelStart(short propVal)
{
	SetProperty(0x39, VT_I2, propVal);
}

CString _DSSDBCombo::GetSelText()
{
	CString result;
	GetProperty(0x3a, VT_BSTR, (void*)&result);
	return result;
}

void _DSSDBCombo::SetSelText(LPCTSTR propVal)
{
	SetProperty(0x3a, VT_BSTR, propVal);
}

short _DSSDBCombo::GetSelLength()
{
	short result;
	GetProperty(0x3b, VT_I2, (void*)&result);
	return result;
}

void _DSSDBCombo::SetSelLength(short propVal)
{
	SetProperty(0x3b, VT_I2, propVal);
}

BOOL _DSSDBCombo::GetCheckBox3D()
{
	BOOL result;
	GetProperty(0x3c, VT_BOOL, (void*)&result);
	return result;
}

void _DSSDBCombo::SetCheckBox3D(BOOL propVal)
{
	SetProperty(0x3c, VT_BOOL, propVal);
}

CString _DSSDBCombo::GetHeadStyleSet()
{
	CString result;
	GetProperty(0x3d, VT_BSTR, (void*)&result);
	return result;
}

void _DSSDBCombo::SetHeadStyleSet(LPCTSTR propVal)
{
	SetProperty(0x3d, VT_BSTR, propVal);
}

CString _DSSDBCombo::GetStyleSet()
{
	CString result;
	GetProperty(0x3e, VT_BSTR, (void*)&result);
	return result;
}

void _DSSDBCombo::SetStyleSet(LPCTSTR propVal)
{
	SetProperty(0x3e, VT_BSTR, propVal);
}

BOOL _DSSDBCombo::GetMultiLine()
{
	BOOL result;
	GetProperty(0x3f, VT_BOOL, (void*)&result);
	return result;
}

void _DSSDBCombo::SetMultiLine(BOOL propVal)
{
	SetProperty(0x3f, VT_BOOL, propVal);
}

VARIANT _DSSDBCombo::GetFirstRow()
{
	VARIANT result;
	GetProperty(0x40, VT_VARIANT, (void*)&result);
	return result;
}

void _DSSDBCombo::SetFirstRow(const VARIANT& propVal)
{
	SetProperty(0x40, VT_VARIANT, &propVal);
}

long _DSSDBCombo::GetRowSelectionStyle()
{
	long result;
	GetProperty(0x41, VT_I4, (void*)&result);
	return result;
}

void _DSSDBCombo::SetRowSelectionStyle(long propVal)
{
	SetProperty(0x41, VT_I4, propVal);
}

long _DSSDBCombo::GetBorderStyle()
{
	long result;
	GetProperty(0x12c, VT_I4, (void*)&result);
	return result;
}

void _DSSDBCombo::SetBorderStyle(long propVal)
{
	SetProperty(0x12c, VT_I4, propVal);
}

long _DSSDBCombo::GetHwndEdit()
{
	long result;
	GetProperty(0x12d, VT_I4, (void*)&result);
	return result;
}

void _DSSDBCombo::SetHwndEdit(long propVal)
{
	SetProperty(0x12d, VT_I4, propVal);
}

short _DSSDBCombo::GetSplitterPos()
{
	short result;
	GetProperty(0x12e, VT_I2, (void*)&result);
	return result;
}

void _DSSDBCombo::SetSplitterPos(short propVal)
{
	SetProperty(0x12e, VT_I2, propVal);
}

CString _DSSDBCombo::GetDataFieldToDisplay()
{
	CString result;
	GetProperty(0x12f, VT_BSTR, (void*)&result);
	return result;
}

void _DSSDBCombo::SetDataFieldToDisplay(LPCTSTR propVal)
{
	SetProperty(0x12f, VT_BSTR, propVal);
}

LPDISPATCH _DSSDBCombo::GetColumns()
{
	LPDISPATCH result;
	GetProperty(0x3e9, VT_DISPATCH, (void*)&result);
	return result;
}

void _DSSDBCombo::SetColumns(LPDISPATCH propVal)
{
	SetProperty(0x3e9, VT_DISPATCH, propVal);
}

LPDISPATCH _DSSDBCombo::GetGroups()
{
	LPDISPATCH result;
	GetProperty(0x3ea, VT_DISPATCH, (void*)&result);
	return result;
}

void _DSSDBCombo::SetGroups(LPDISPATCH propVal)
{
	SetProperty(0x3ea, VT_DISPATCH, propVal);
}

LPDISPATCH _DSSDBCombo::GetSelBookmarks()
{
	LPDISPATCH result;
	GetProperty(0x3eb, VT_DISPATCH, (void*)&result);
	return result;
}

void _DSSDBCombo::SetSelBookmarks(LPDISPATCH propVal)
{
	SetProperty(0x3eb, VT_DISPATCH, propVal);
}

LPDISPATCH _DSSDBCombo::GetStyleSets()
{
	LPDISPATCH result;
	GetProperty(0x3ec, VT_DISPATCH, (void*)&result);
	return result;
}

void _DSSDBCombo::SetStyleSets(LPDISPATCH propVal)
{
	SetProperty(0x3ec, VT_DISPATCH, propVal);
}

CString _DSSDBCombo::GetMask()
{
	CString result;
	GetProperty(0x3ed, VT_BSTR, (void*)&result);
	return result;
}

void _DSSDBCombo::SetMask(LPCTSTR propVal)
{
	SetProperty(0x3ed, VT_BSTR, propVal);
}

BOOL _DSSDBCombo::GetPromptInclude()
{
	BOOL result;
	GetProperty(0x3ee, VT_BOOL, (void*)&result);
	return result;
}

void _DSSDBCombo::SetPromptInclude(BOOL propVal)
{
	SetProperty(0x3ee, VT_BOOL, propVal);
}

long _DSSDBCombo::GetClipMode()
{
	long result;
	GetProperty(0x3ef, VT_I4, (void*)&result);
	return result;
}

void _DSSDBCombo::SetClipMode(long propVal)
{
	SetProperty(0x3ef, VT_I4, propVal);
}

CString _DSSDBCombo::GetPromptChar()
{
	CString result;
	GetProperty(0x3f0, VT_BSTR, (void*)&result);
	return result;
}

void _DSSDBCombo::SetPromptChar(LPCTSTR propVal)
{
	SetProperty(0x3f0, VT_BSTR, propVal);
}

/////////////////////////////////////////////////////////////////////////////
// _DSSDBCombo operations

BOOL _DSSDBCombo::IsItemInList()
{
	BOOL result;
	InvokeHelper(0x42, DISPATCH_METHOD, VT_BOOL, (void*)&result, NULL);
	return result;
}

BOOL _DSSDBCombo::IsTextValid(const VARIANT& ErrCode)
{
	BOOL result;
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x43, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		&ErrCode);
	return result;
}

void _DSSDBCombo::AddItem(LPCTSTR Item, const VARIANT& Index)
{
	static BYTE parms[] =
		VTS_BSTR VTS_VARIANT;
	InvokeHelper(0x44, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Item, &Index);
}

short _DSSDBCombo::ColContaining(float X, const VARIANT& Y)
{
	short result;
	static BYTE parms[] =
		VTS_R4 VTS_VARIANT;
	InvokeHelper(0x45, DISPATCH_METHOD, VT_I2, (void*)&result, parms,
		X, &Y);
	return result;
}

void _DSSDBCombo::DoClick()
{
	InvokeHelper(DISPID_DOCLICK, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

VARIANT _DSSDBCombo::GetBookmark(long RowNum)
{
	VARIANT result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x47, DISPATCH_METHOD, VT_VARIANT, (void*)&result, parms,
		RowNum);
	return result;
}

void _DSSDBCombo::MoveFirst()
{
	InvokeHelper(0x49, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void _DSSDBCombo::MoveLast()
{
	InvokeHelper(0x4a, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void _DSSDBCombo::MoveNext()
{
	InvokeHelper(0x4b, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void _DSSDBCombo::MovePrevious()
{
	InvokeHelper(0x4c, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void _DSSDBCombo::MoveRecords(long Records)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x4d, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Records);
}

void _DSSDBCombo::ReBind()
{
	InvokeHelper(0x4e, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

VARIANT _DSSDBCombo::RowBookmark(long RowNum)
{
	VARIANT result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x4f, DISPATCH_METHOD, VT_VARIANT, (void*)&result, parms,
		RowNum);
	return result;
}

short _DSSDBCombo::RowContaining(float Y)
{
	short result;
	static BYTE parms[] =
		VTS_R4;
	InvokeHelper(0x50, DISPATCH_METHOD, VT_I2, (void*)&result, parms,
		Y);
	return result;
}

float _DSSDBCombo::RowTop(short RowNum)
{
	float result;
	static BYTE parms[] =
		VTS_I2;
	InvokeHelper(0x51, DISPATCH_METHOD, VT_R4, (void*)&result, parms,
		RowNum);
	return result;
}

void _DSSDBCombo::Scroll(short Cols, long Rows)
{
	static BYTE parms[] =
		VTS_I2 VTS_I4;
	InvokeHelper(0x52, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Cols, Rows);
}

short _DSSDBCombo::GrpContaining(float X)
{
	short result;
	static BYTE parms[] =
		VTS_R4;
	InvokeHelper(0x53, DISPATCH_METHOD, VT_I2, (void*)&result, parms,
		X);
	return result;
}

void _DSSDBCombo::RemoveAll()
{
	InvokeHelper(0x55, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void _DSSDBCombo::RemoveItem(long Index)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x56, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Index);
}

void _DSSDBCombo::Reset()
{
	InvokeHelper(0x5d, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

VARIANT _DSSDBCombo::AddItemBookmark(long RowIndex)
{
	VARIANT result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x5e, DISPATCH_METHOD, VT_VARIANT, (void*)&result, parms,
		RowIndex);
	return result;
}

long _DSSDBCombo::AddItemRowIndex(const VARIANT& Bookmark)
{
	long result;
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x5f, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		&Bookmark);
	return result;
}

void _DSSDBCombo::Refresh()
{
	InvokeHelper(DISPID_REFRESH, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void _DSSDBCombo::SaveLayout(LPCTSTR FileName, long Flags)
{
	static BYTE parms[] =
		VTS_BSTR VTS_I4;
	InvokeHelper(0x130, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 FileName, Flags);
}

void _DSSDBCombo::LoadLayout(LPCTSTR FileName)
{
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x131, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 FileName);
}

void _DSSDBCombo::AboutBox()
{
	InvokeHelper(0xfffffdd8, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// _DSSDBComboEvents properties

/////////////////////////////////////////////////////////////////////////////
// _DSSDBComboEvents operations

void _DSSDBComboEvents::InitColumnProps()
{
	InvokeHelper(0x4, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void _DSSDBComboEvents::Click()
{
	InvokeHelper(DISPID_CLICK, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void _DSSDBComboEvents::DropDown()
{
	InvokeHelper(0x1, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void _DSSDBComboEvents::CloseUp()
{
	InvokeHelper(0x2, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void _DSSDBComboEvents::Change()
{
	InvokeHelper(0x3, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void _DSSDBComboEvents::DblClick()
{
	InvokeHelper(DISPID_DBLCLICK, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void _DSSDBComboEvents::KeyDown(short* KeyCode, short Shift)
{
	static BYTE parms[] =
		VTS_PI2 VTS_I2;
	InvokeHelper(DISPID_KEYDOWN, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 KeyCode, Shift);
}

void _DSSDBComboEvents::KeyPress(short* KeyAscii)
{
	static BYTE parms[] =
		VTS_PI2;
	InvokeHelper(DISPID_KEYPRESS, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 KeyAscii);
}

void _DSSDBComboEvents::KeyUp(short* KeyCode, short Shift)
{
	static BYTE parms[] =
		VTS_PI2 VTS_I2;
	InvokeHelper(DISPID_KEYUP, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 KeyCode, Shift);
}

void _DSSDBComboEvents::MouseDown(short Button, short Shift, long X, long Y)
{
	static BYTE parms[] =
		VTS_I2 VTS_I2 VTS_I4 VTS_I4;
	InvokeHelper(DISPID_MOUSEDOWN, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Button, Shift, X, Y);
}

void _DSSDBComboEvents::MouseMove(short Button, short Shift, long X, long Y)
{
	static BYTE parms[] =
		VTS_I2 VTS_I2 VTS_I4 VTS_I4;
	InvokeHelper(DISPID_MOUSEMOVE, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Button, Shift, X, Y);
}

void _DSSDBComboEvents::MouseUp(short Button, short Shift, long X, long Y)
{
	static BYTE parms[] =
		VTS_I2 VTS_I2 VTS_I4 VTS_I4;
	InvokeHelper(DISPID_MOUSEUP, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Button, Shift, X, Y);
}

void _DSSDBComboEvents::Scroll(short* Cancel)
{
	static BYTE parms[] =
		VTS_PI2;
	InvokeHelper(0x5, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Cancel);
}

void _DSSDBComboEvents::PositionList(LPCTSTR Text)
{
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x6, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Text);
}

void _DSSDBComboEvents::TextError(long* ErrCode, BSTR* ErrString, BSTR* RestoreString, BSTR* Text, short* RtnDispErrMsg, short* RtnRestore)
{
	static BYTE parms[] =
		VTS_PI4 VTS_PBSTR VTS_PBSTR VTS_PBSTR VTS_PI2 VTS_PI2;
	InvokeHelper(0x7, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 ErrCode, ErrString, RestoreString, Text, RtnDispErrMsg, RtnRestore);
}

void _DSSDBComboEvents::ValidateList(BSTR* Text, short* RtnPassed)
{
	static BYTE parms[] =
		VTS_PBSTR VTS_PI2;
	InvokeHelper(0x8, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Text, RtnPassed);
}

void _DSSDBComboEvents::RowLoaded(const VARIANT& Bookmark)
{
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x9, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 &Bookmark);
}

void _DSSDBComboEvents::UnboundReadData(LPDISPATCH RowBuf, VARIANT* StartLocation, BOOL ReadPriorRows)
{
	static BYTE parms[] =
		VTS_DISPATCH VTS_PVARIANT VTS_BOOL;
	InvokeHelper(0xa, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 RowBuf, StartLocation, ReadPriorRows);
}

void _DSSDBComboEvents::UnboundPositionData(VARIANT* StartLocation, long NumberOfRowsToMove, VARIANT* NewLocation)
{
	static BYTE parms[] =
		VTS_PVARIANT VTS_I4 VTS_PVARIANT;
	InvokeHelper(0xb, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 StartLocation, NumberOfRowsToMove, NewLocation);
}

void _DSSDBComboEvents::ScrollAfter()
{
	InvokeHelper(0xc, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void _DSSDBComboEvents::ValidationError(BSTR* InvalidText, short StartPosition)
{
	static BYTE parms[] =
		VTS_PBSTR VTS_I2;
	InvokeHelper(0x3e8, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 InvalidText, StartPosition);
}


/////////////////////////////////////////////////////////////////////////////
// _DSSDBDropDown properties

OLE_COLOR _DSSDBDropDown::GetBackColor()
{
	OLE_COLOR result;
	GetProperty(DISPID_BACKCOLOR, VT_I4, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetBackColor(OLE_COLOR propVal)
{
	SetProperty(DISPID_BACKCOLOR, VT_I4, propVal);
}

unsigned long _DSSDBDropDown::GetBackColorEven()
{
	unsigned long result;
	GetProperty(0x1, VT_I4, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetBackColorEven(unsigned long propVal)
{
	SetProperty(0x1, VT_I4, propVal);
}

unsigned long _DSSDBDropDown::GetBackColorOdd()
{
	unsigned long result;
	GetProperty(0x2, VT_I4, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetBackColorOdd(unsigned long propVal)
{
	SetProperty(0x2, VT_I4, propVal);
}

unsigned long _DSSDBDropDown::GetBevelColorHighlight()
{
	unsigned long result;
	GetProperty(0x3, VT_I4, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetBevelColorHighlight(unsigned long propVal)
{
	SetProperty(0x3, VT_I4, propVal);
}

unsigned long _DSSDBDropDown::GetBevelColorFace()
{
	unsigned long result;
	GetProperty(0x4, VT_I4, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetBevelColorFace(unsigned long propVal)
{
	SetProperty(0x4, VT_I4, propVal);
}

unsigned long _DSSDBDropDown::GetBevelColorFrame()
{
	unsigned long result;
	GetProperty(0x5, VT_I4, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetBevelColorFrame(unsigned long propVal)
{
	SetProperty(0x5, VT_I4, propVal);
}

long _DSSDBDropDown::GetBevelColorScheme()
{
	long result;
	GetProperty(0x6, VT_I4, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetBevelColorScheme(long propVal)
{
	SetProperty(0x6, VT_I4, propVal);
}

unsigned long _DSSDBDropDown::GetBevelColorShadow()
{
	unsigned long result;
	GetProperty(0x7, VT_I4, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetBevelColorShadow(unsigned long propVal)
{
	SetProperty(0x7, VT_I4, propVal);
}

BOOL _DSSDBDropDown::GetColumnHeaders()
{
	BOOL result;
	GetProperty(0x8, VT_BOOL, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetColumnHeaders(BOOL propVal)
{
	SetProperty(0x8, VT_BOOL, propVal);
}

long _DSSDBDropDown::GetDataMode()
{
	long result;
	GetProperty(0x9, VT_I4, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetDataMode(long propVal)
{
	SetProperty(0x9, VT_I4, propVal);
}

LPUNKNOWN _DSSDBDropDown::GetDataSource()
{
	LPUNKNOWN result;
	GetProperty(0xa, VT_UNKNOWN, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetDataSource(LPUNKNOWN propVal)
{
	SetProperty(0xa, VT_UNKNOWN, propVal);
}

float _DSSDBDropDown::GetDefColWidth()
{
	float result;
	GetProperty(0xb, VT_R4, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetDefColWidth(float propVal)
{
	SetProperty(0xb, VT_R4, propVal);
}

long _DSSDBDropDown::GetDividerStyle()
{
	long result;
	GetProperty(0xc, VT_I4, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetDividerStyle(long propVal)
{
	SetProperty(0xc, VT_I4, propVal);
}

long _DSSDBDropDown::GetDividerType()
{
	long result;
	GetProperty(0xd, VT_I4, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetDividerType(long propVal)
{
	SetProperty(0xd, VT_I4, propVal);
}

BOOL _DSSDBDropDown::GetEnabled()
{
	BOOL result;
	GetProperty(DISPID_ENABLED, VT_BOOL, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetEnabled(BOOL propVal)
{
	SetProperty(DISPID_ENABLED, VT_BOOL, propVal);
}

LPFONTDISP _DSSDBDropDown::GetFont()
{
	LPFONTDISP result;
	GetProperty(DISPID_FONT, VT_DISPATCH, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetFont(LPFONTDISP propVal)
{
	SetProperty(DISPID_FONT, VT_DISPATCH, propVal);
}

long _DSSDBDropDown::GetFont3D()
{
	long result;
	GetProperty(0xe, VT_I4, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetFont3D(long propVal)
{
	SetProperty(0xe, VT_I4, propVal);
}

OLE_COLOR _DSSDBDropDown::GetForeColor()
{
	OLE_COLOR result;
	GetProperty(DISPID_FORECOLOR, VT_I4, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetForeColor(OLE_COLOR propVal)
{
	SetProperty(DISPID_FORECOLOR, VT_I4, propVal);
}

unsigned long _DSSDBDropDown::GetForeColorEven()
{
	unsigned long result;
	GetProperty(0xf, VT_I4, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetForeColorEven(unsigned long propVal)
{
	SetProperty(0xf, VT_I4, propVal);
}

unsigned long _DSSDBDropDown::GetForeColorOdd()
{
	unsigned long result;
	GetProperty(0x10, VT_I4, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetForeColorOdd(unsigned long propVal)
{
	SetProperty(0x10, VT_I4, propVal);
}

long _DSSDBDropDown::GetHeadFont3D()
{
	long result;
	GetProperty(0x11, VT_I4, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetHeadFont3D(long propVal)
{
	SetProperty(0x11, VT_I4, propVal);
}

LPDISPATCH _DSSDBDropDown::GetHeadFont()
{
	LPDISPATCH result;
	GetProperty(0x12, VT_DISPATCH, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetHeadFont(LPDISPATCH propVal)
{
	SetProperty(0x12, VT_DISPATCH, propVal);
}

short _DSSDBDropDown::GetHeadLines()
{
	short result;
	GetProperty(0x13, VT_I2, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetHeadLines(short propVal)
{
	SetProperty(0x13, VT_I2, propVal);
}

OLE_HANDLE _DSSDBDropDown::GetHWnd()
{
	OLE_HANDLE result;
	GetProperty(DISPID_HWND, VT_I4, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetHWnd(OLE_HANDLE propVal)
{
	SetProperty(DISPID_HWND, VT_I4, propVal);
}

short _DSSDBDropDown::GetLeftCol()
{
	short result;
	GetProperty(0x14, VT_I2, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetLeftCol(short propVal)
{
	SetProperty(0x14, VT_I2, propVal);
}

short _DSSDBDropDown::GetLeftGrp()
{
	short result;
	GetProperty(0x15, VT_I2, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetLeftGrp(short propVal)
{
	SetProperty(0x15, VT_I2, propVal);
}

short _DSSDBDropDown::GetLevelCount()
{
	short result;
	GetProperty(0x16, VT_I2, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetLevelCount(short propVal)
{
	SetProperty(0x16, VT_I2, propVal);
}

BOOL _DSSDBDropDown::GetRedraw()
{
	BOOL result;
	GetProperty(0x17, VT_BOOL, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetRedraw(BOOL propVal)
{
	SetProperty(0x17, VT_BOOL, propVal);
}

short _DSSDBDropDown::GetRow()
{
	short result;
	GetProperty(0x18, VT_I2, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetRow(short propVal)
{
	SetProperty(0x18, VT_I2, propVal);
}

long _DSSDBDropDown::GetScrollBars()
{
	long result;
	GetProperty(0x19, VT_I4, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetScrollBars(long propVal)
{
	SetProperty(0x19, VT_I4, propVal);
}

VARIANT _DSSDBDropDown::GetTagVariant()
{
	VARIANT result;
	GetProperty(0x1a, VT_VARIANT, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetTagVariant(const VARIANT& propVal)
{
	SetProperty(0x1a, VT_VARIANT, &propVal);
}

float _DSSDBDropDown::GetRowHeight()
{
	float result;
	GetProperty(0x1b, VT_R4, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetRowHeight(float propVal)
{
	SetProperty(0x1b, VT_R4, propVal);
}

short _DSSDBDropDown::GetCols()
{
	short result;
	GetProperty(0x1c, VT_I2, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetCols(short propVal)
{
	SetProperty(0x1c, VT_I2, propVal);
}

BOOL _DSSDBDropDown::GetGroupHeaders()
{
	BOOL result;
	GetProperty(0x1d, VT_BOOL, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetGroupHeaders(BOOL propVal)
{
	SetProperty(0x1d, VT_BOOL, propVal);
}

VARIANT _DSSDBDropDown::GetBookmark()
{
	VARIANT result;
	GetProperty(0x1e, VT_VARIANT, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetBookmark(const VARIANT& propVal)
{
	SetProperty(0x1e, VT_VARIANT, &propVal);
}

short _DSSDBDropDown::GetGroupHeadLines()
{
	short result;
	GetProperty(0x1f, VT_I2, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetGroupHeadLines(short propVal)
{
	SetProperty(0x1f, VT_I2, propVal);
}

long _DSSDBDropDown::GetRows()
{
	long result;
	GetProperty(0x20, VT_I4, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetRows(long propVal)
{
	SetProperty(0x20, VT_I4, propVal);
}

short _DSSDBDropDown::GetVisibleRows()
{
	short result;
	GetProperty(0x21, VT_I2, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetVisibleRows(short propVal)
{
	SetProperty(0x21, VT_I2, propVal);
}

short _DSSDBDropDown::GetVisibleCols()
{
	short result;
	GetProperty(0x22, VT_I2, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetVisibleCols(short propVal)
{
	SetProperty(0x22, VT_I2, propVal);
}

short _DSSDBDropDown::GetVisibleGrps()
{
	short result;
	GetProperty(0x23, VT_I2, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetVisibleGrps(short propVal)
{
	SetProperty(0x23, VT_I2, propVal);
}

CString _DSSDBDropDown::GetDataFieldList()
{
	CString result;
	GetProperty(0x24, VT_BSTR, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetDataFieldList(LPCTSTR propVal)
{
	SetProperty(0x24, VT_BSTR, propVal);
}

BOOL _DSSDBDropDown::GetListAutoPosition()
{
	BOOL result;
	GetProperty(0x25, VT_BOOL, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetListAutoPosition(BOOL propVal)
{
	SetProperty(0x25, VT_BOOL, propVal);
}

BOOL _DSSDBDropDown::GetListAutoValidate()
{
	BOOL result;
	GetProperty(0x26, VT_BOOL, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetListAutoValidate(BOOL propVal)
{
	SetProperty(0x26, VT_BOOL, propVal);
}

BOOL _DSSDBDropDown::GetListWidthAutoSize()
{
	BOOL result;
	GetProperty(0x27, VT_BOOL, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetListWidthAutoSize(BOOL propVal)
{
	SetProperty(0x27, VT_BOOL, propVal);
}

short _DSSDBDropDown::GetMaxDropDownItems()
{
	short result;
	GetProperty(0x28, VT_I2, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetMaxDropDownItems(short propVal)
{
	SetProperty(0x28, VT_I2, propVal);
}

short _DSSDBDropDown::GetMinDropDownItems()
{
	short result;
	GetProperty(0x29, VT_I2, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetMinDropDownItems(short propVal)
{
	SetProperty(0x29, VT_I2, propVal);
}

BOOL _DSSDBDropDown::GetDroppedDown()
{
	BOOL result;
	GetProperty(0x2a, VT_BOOL, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetDroppedDown(BOOL propVal)
{
	SetProperty(0x2a, VT_BOOL, propVal);
}

CString _DSSDBDropDown::GetFieldDelimiter()
{
	CString result;
	GetProperty(0x2b, VT_BSTR, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetFieldDelimiter(LPCTSTR propVal)
{
	SetProperty(0x2b, VT_BSTR, propVal);
}

CString _DSSDBDropDown::GetFieldSeparator()
{
	CString result;
	GetProperty(0x2c, VT_BSTR, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetFieldSeparator(LPCTSTR propVal)
{
	SetProperty(0x2c, VT_BSTR, propVal);
}

BOOL _DSSDBDropDown::GetUseExactRowCount()
{
	BOOL result;
	GetProperty(0x2d, VT_BOOL, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetUseExactRowCount(BOOL propVal)
{
	SetProperty(0x2d, VT_BOOL, propVal);
}

float _DSSDBDropDown::GetListWidth()
{
	float result;
	GetProperty(0x2e, VT_R4, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetListWidth(float propVal)
{
	SetProperty(0x2e, VT_R4, propVal);
}

BOOL _DSSDBDropDown::GetCheckBox3D()
{
	BOOL result;
	GetProperty(0x2f, VT_BOOL, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetCheckBox3D(BOOL propVal)
{
	SetProperty(0x2f, VT_BOOL, propVal);
}

CString _DSSDBDropDown::GetHeadStyleSet()
{
	CString result;
	GetProperty(0x30, VT_BSTR, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetHeadStyleSet(LPCTSTR propVal)
{
	SetProperty(0x30, VT_BSTR, propVal);
}

CString _DSSDBDropDown::GetStyleSet()
{
	CString result;
	GetProperty(0x31, VT_BSTR, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetStyleSet(LPCTSTR propVal)
{
	SetProperty(0x31, VT_BSTR, propVal);
}

VARIANT _DSSDBDropDown::GetFirstRow()
{
	VARIANT result;
	GetProperty(0x32, VT_VARIANT, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetFirstRow(const VARIANT& propVal)
{
	SetProperty(0x32, VT_VARIANT, &propVal);
}

long _DSSDBDropDown::GetRowSelectionStyle()
{
	long result;
	GetProperty(0x33, VT_I4, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetRowSelectionStyle(long propVal)
{
	SetProperty(0x33, VT_I4, propVal);
}

long _DSSDBDropDown::GetBorderStyle()
{
	long result;
	GetProperty(0x12c, VT_I4, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetBorderStyle(long propVal)
{
	SetProperty(0x12c, VT_I4, propVal);
}

short _DSSDBDropDown::GetSplitterPos()
{
	short result;
	GetProperty(0x12d, VT_I2, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetSplitterPos(short propVal)
{
	SetProperty(0x12d, VT_I2, propVal);
}

CString _DSSDBDropDown::GetDataFieldToDisplay()
{
	CString result;
	GetProperty(0x12e, VT_BSTR, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetDataFieldToDisplay(LPCTSTR propVal)
{
	SetProperty(0x12e, VT_BSTR, propVal);
}

LPDISPATCH _DSSDBDropDown::GetColumns()
{
	LPDISPATCH result;
	GetProperty(0x3e9, VT_DISPATCH, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetColumns(LPDISPATCH propVal)
{
	SetProperty(0x3e9, VT_DISPATCH, propVal);
}

LPDISPATCH _DSSDBDropDown::GetGroups()
{
	LPDISPATCH result;
	GetProperty(0x3ea, VT_DISPATCH, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetGroups(LPDISPATCH propVal)
{
	SetProperty(0x3ea, VT_DISPATCH, propVal);
}

LPDISPATCH _DSSDBDropDown::GetSelBookmarks()
{
	LPDISPATCH result;
	GetProperty(0x3eb, VT_DISPATCH, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetSelBookmarks(LPDISPATCH propVal)
{
	SetProperty(0x3eb, VT_DISPATCH, propVal);
}

LPDISPATCH _DSSDBDropDown::GetStyleSets()
{
	LPDISPATCH result;
	GetProperty(0x3ec, VT_DISPATCH, (void*)&result);
	return result;
}

void _DSSDBDropDown::SetStyleSets(LPDISPATCH propVal)
{
	SetProperty(0x3ec, VT_DISPATCH, propVal);
}

/////////////////////////////////////////////////////////////////////////////
// _DSSDBDropDown operations

void _DSSDBDropDown::RemoveItem(long Index)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x36, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Index);
}

VARIANT _DSSDBDropDown::RowBookmark(long RowNum)
{
	VARIANT result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x37, DISPATCH_METHOD, VT_VARIANT, (void*)&result, parms,
		RowNum);
	return result;
}

float _DSSDBDropDown::RowTop(short RowNum)
{
	float result;
	static BYTE parms[] =
		VTS_I2;
	InvokeHelper(0x38, DISPATCH_METHOD, VT_R4, (void*)&result, parms,
		RowNum);
	return result;
}

void _DSSDBDropDown::ReBind()
{
	InvokeHelper(0x39, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

short _DSSDBDropDown::ColContaining(float X, const VARIANT& Y)
{
	short result;
	static BYTE parms[] =
		VTS_R4 VTS_VARIANT;
	InvokeHelper(0x3a, DISPATCH_METHOD, VT_I2, (void*)&result, parms,
		X, &Y);
	return result;
}

VARIANT _DSSDBDropDown::GetBookmark(long RowNum)
{
	VARIANT result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x3b, DISPATCH_METHOD, VT_VARIANT, (void*)&result, parms,
		RowNum);
	return result;
}

void _DSSDBDropDown::RemoveAll()
{
	InvokeHelper(0x3c, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

short _DSSDBDropDown::GrpContaining(float X)
{
	short result;
	static BYTE parms[] =
		VTS_R4;
	InvokeHelper(0x3d, DISPATCH_METHOD, VT_I2, (void*)&result, parms,
		X);
	return result;
}

void _DSSDBDropDown::DoClick()
{
	InvokeHelper(DISPID_DOCLICK, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void _DSSDBDropDown::AddItem(LPCTSTR Item, const VARIANT& Index)
{
	static BYTE parms[] =
		VTS_BSTR VTS_VARIANT;
	InvokeHelper(0x3f, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Item, &Index);
}

short _DSSDBDropDown::RowContaining(float Y)
{
	short result;
	static BYTE parms[] =
		VTS_R4;
	InvokeHelper(0x40, DISPATCH_METHOD, VT_I2, (void*)&result, parms,
		Y);
	return result;
}

void _DSSDBDropDown::Scroll(short Cols, long Rows)
{
	static BYTE parms[] =
		VTS_I2 VTS_I4;
	InvokeHelper(0x41, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Cols, Rows);
}

void _DSSDBDropDown::MoveFirst()
{
	InvokeHelper(0x42, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void _DSSDBDropDown::MovePrevious()
{
	InvokeHelper(0x43, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void _DSSDBDropDown::MoveRecords(long Records)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x44, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Records);
}

void _DSSDBDropDown::MoveNext()
{
	InvokeHelper(0x45, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void _DSSDBDropDown::MoveLast()
{
	InvokeHelper(0x46, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void _DSSDBDropDown::Reset()
{
	InvokeHelper(0x4d, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

VARIANT _DSSDBDropDown::AddItemBookmark(long RowIndex)
{
	VARIANT result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x4e, DISPATCH_METHOD, VT_VARIANT, (void*)&result, parms,
		RowIndex);
	return result;
}

long _DSSDBDropDown::AddItemRowIndex(const VARIANT& Bookmark)
{
	long result;
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x4f, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		&Bookmark);
	return result;
}

void _DSSDBDropDown::Refresh()
{
	InvokeHelper(DISPID_REFRESH, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void _DSSDBDropDown::SaveLayout(LPCTSTR FileName, long Flags)
{
	static BYTE parms[] =
		VTS_BSTR VTS_I4;
	InvokeHelper(0x12f, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 FileName, Flags);
}

void _DSSDBDropDown::LoadLayout(LPCTSTR FileName)
{
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x130, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 FileName);
}

void _DSSDBDropDown::AboutBox()
{
	InvokeHelper(0xfffffdd8, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// _DSSDBDropDownEvents properties

/////////////////////////////////////////////////////////////////////////////
// _DSSDBDropDownEvents operations

void _DSSDBDropDownEvents::InitColumnProps()
{
	InvokeHelper(0x1, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void _DSSDBDropDownEvents::Click()
{
	InvokeHelper(DISPID_CLICK, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void _DSSDBDropDownEvents::Scroll(short* Cancel)
{
	static BYTE parms[] =
		VTS_PI2;
	InvokeHelper(0x2, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Cancel);
}

void _DSSDBDropDownEvents::CloseUp()
{
	InvokeHelper(0x3, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void _DSSDBDropDownEvents::DropDown()
{
	InvokeHelper(0x4, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void _DSSDBDropDownEvents::PositionList(LPCTSTR Text)
{
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x5, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Text);
}

void _DSSDBDropDownEvents::ValidateList(BSTR* Text, short* RtnPassed)
{
	static BYTE parms[] =
		VTS_PBSTR VTS_PI2;
	InvokeHelper(0x6, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Text, RtnPassed);
}

void _DSSDBDropDownEvents::TextError(long* ErrCode, BSTR* ErrString, BSTR* RestoreString, BSTR* Text, short* RtnDispErrMsg, short* RtnRestore)
{
	static BYTE parms[] =
		VTS_PI4 VTS_PBSTR VTS_PBSTR VTS_PBSTR VTS_PI2 VTS_PI2;
	InvokeHelper(0x7, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 ErrCode, ErrString, RestoreString, Text, RtnDispErrMsg, RtnRestore);
}

void _DSSDBDropDownEvents::UnboundReadData(LPDISPATCH RowBuf, VARIANT* StartLocation, BOOL ReadPriorRows)
{
	static BYTE parms[] =
		VTS_DISPATCH VTS_PVARIANT VTS_BOOL;
	InvokeHelper(0x9, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 RowBuf, StartLocation, ReadPriorRows);
}

void _DSSDBDropDownEvents::RowLoaded(const VARIANT& Bookmark)
{
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x8, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 &Bookmark);
}

void _DSSDBDropDownEvents::UnboundPositionData(VARIANT* StartLocation, long NumberOfRowsToMove, VARIANT* NewLocation)
{
	static BYTE parms[] =
		VTS_PVARIANT VTS_I4 VTS_PVARIANT;
	InvokeHelper(0xa, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 StartLocation, NumberOfRowsToMove, NewLocation);
}

void _DSSDBDropDownEvents::ScrollAfter()
{
	InvokeHelper(0xb, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}
