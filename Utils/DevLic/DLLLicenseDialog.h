#if !defined(AFX_DLLLICENSEDIALOG_H__6F1AB9E1_0357_429E_A26D_A33A33679977__INCLUDED_)
#define AFX_DLLLICENSEDIALOG_H__6F1AB9E1_0357_429E_A26D_A33A33679977__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DLLLicenseDialog.h : header file
//

#include "scribble.h"

//===========================================================================

class CDLLLicenseDialog : public CDialog ,  public CScribble
{
public:
	CDLLLicenseDialog(CWnd* pParent = NULL);   // standard constructor
	//{{AFX_DATA(CDLLLicenseDialog)
	enum { IDD = IDD_DIALOG_DLL };
	CString	m_DllName;
	CString	m_DllStatus;
	//}}AFX_DATA
	//{{AFX_VIRTUAL(CDLLLicenseDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
protected:
	//{{AFX_MSG(CDLLLicenseDialog)
	afx_msg void OnButtonDllBrowse();
	afx_msg void OnButtonDllLicense();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//===========================================================================

class CProfINIFile
  {
  public:
    CString sFilename;
    CString sTmpRdStr;

    CProfINIFile(char* pFilename = "");
    CProfINIFile(char* pDir, char* pFilename);
    ~CProfINIFile();
    double RdDouble(LPCTSTR lpszSection, LPCTSTR lpszEntry, double dDefault);
    void WrDouble(LPCTSTR lpszSection, LPCTSTR lpszEntry, double dValue);
    long RdLong(LPCTSTR lpszSection, LPCTSTR lpszEntry, long nDefault);
    void WrLong(LPCTSTR lpszSection, LPCTSTR lpszEntry, long nValue);
    int RdInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault);
    void WrInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue);
    CString &RdStr(LPCTSTR lpszSection, LPCTSTR lpszEntry, char* pDefault="");
    void WrStr(LPCTSTR lpszSection, LPCTSTR lpszEntry, char* pValue);
    DWORD RdSection(LPCTSTR lpszSection, char* pBuff, int BuffLen);
    void WrSection(LPCTSTR lpszSection, char* pBuff);
    DWORD RdSectionNames(char* pBuff, int BuffLen);
    void Flush();
    static void GetProfItem(char* &pBuff, char* &pName, char* &pExp);
    inline void SetProfFilename(char* pFilename) { sFilename = pFilename; };
    inline char* Filename() { return (char*)(const char*)sFilename; }
    inline void WrStr(LPCTSTR lpszSection, LPCTSTR lpszEntry, CString& sValue) { WrStr(lpszSection, lpszEntry, (char*)(const char*)sValue); };
    inline void WrStr(LPCTSTR lpszSection, LPCTSTR lpszEntry, const char* pValue) { WrStr(lpszSection, lpszEntry, (char*)pValue); };
  };

//===========================================================================

class CLicenseLocationDlg : public CDialog
  {
  public:
    CLicenseLocationDlg(CWnd* pParent = NULL);   // standard constructor
    //{{AFX_DATA(CLicenseLocationDlg)
    enum { IDD = IDD_CK_LOCATIONDLG };
    CString m_AppPath;
    int m_LocationOpt;
    //}}AFX_DATA
    //{{AFX_VIRTUAL(CLicenseLocationDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
  protected:
    //{{AFX_MSG(CLicenseLocationDlg)
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    afx_msg void OnBrowse();
    afx_msg void OnLocationOption();
    //}}AFX_MSG
    afx_msg void OnUpdateLocation(CCmdUI* pCmdUi);
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================

extern CProfINIFile AppPF;

void DoInit();

//===========================================================================
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLLLICENSEDIALOG_H__6F1AB9E1_0357_429E_A26D_A33A33679977__INCLUDED_)
