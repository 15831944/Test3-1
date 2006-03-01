// SqSzDMeasPPg.cpp : implementation file
//

#include "stdafx.h"
#include "SizeDst1.h"
#include "SqSzDMeasPPg.h"


// CSqSzDMeasPPg dialog

IMPLEMENT_DYNCREATE(CSqSzDMeasPPg, CDHtmlDialog)

CSqSzDMeasPPg::CSqSzDMeasPPg(CWnd* pParent /*=NULL*/)
	: CDHtmlDialog(CSqSzDMeasPPg::IDD, CSqSzDMeasPPg::IDH, pParent)
{
}

CSqSzDMeasPPg::~CSqSzDMeasPPg()
{
}

void CSqSzDMeasPPg::DoDataExchange(CDataExchange* pDX)
{
	CDHtmlDialog::DoDataExchange(pDX);
}

BOOL CSqSzDMeasPPg::OnInitDialog()
{
	CDHtmlDialog::OnInitDialog();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(CSqSzDMeasPPg, CDHtmlDialog)
END_MESSAGE_MAP()

BEGIN_DHTML_EVENT_MAP(CSqSzDMeasPPg)
	DHTML_EVENT_ONCLICK(_T("ButtonOK"), OnButtonOK)
	DHTML_EVENT_ONCLICK(_T("ButtonCancel"), OnButtonCancel)
END_DHTML_EVENT_MAP()



// CSqSzDMeasPPg message handlers

HRESULT CSqSzDMeasPPg::OnButtonOK(IHTMLElement* /*pElement*/)
{
	OnOK();
	return S_OK;  // return TRUE  unless you set the focus to a control
}

HRESULT CSqSzDMeasPPg::OnButtonCancel(IHTMLElement* /*pElement*/)
{
	OnCancel();
	return S_OK;  // return TRUE  unless you set the focus to a control
}
