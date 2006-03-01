#pragma once


// CSqSzDMeasPPg dialog

class CSqSzDMeasPPg : public CDHtmlDialog
{
	DECLARE_DYNCREATE(CSqSzDMeasPPg)

public:
	CSqSzDMeasPPg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSqSzDMeasPPg();
// Overrides
	HRESULT OnButtonOK(IHTMLElement *pElement);
	HRESULT OnButtonCancel(IHTMLElement *pElement);

// Dialog Data
	enum { IDD = IDD_PP_MDLCFG_MEAS, IDH = IDR_HTML_SQSZDMEASPPG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()
};
