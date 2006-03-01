//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  OPCServerDlg.h
//  $Author    :  Jim Hansen
//
//  Description:  Dialog to select OPC server to connect.
//
//**************************************************************************

class OPCServerInfo
{
public:
    OPCServerInfo( LPOLESTR ProgID, LPOLESTR Description, CLSID& clsid )
        : m_ProgID(ProgID), m_Description(Description), m_clsid(clsid) {}
    CString     m_ProgID;
    CString     m_Description;
    CLSID       m_clsid;
};

typedef CTypedPtrList<CPtrList, OPCServerInfo*> OPCServerList;

class OPCServerDlg : public CDialog
{
// Construction
public:
   OPCServerDlg(CWnd* pParent = NULL);
   ~OPCServerDlg();

// Dialog Data
   //{{AFX_DATA(OPCServerDlg)
    enum { IDD = IDD_OPC_SERVER };
    CString m_Node;
    CString m_Server;
    CString m_currentServer;
    CLSID   m_clsid;
    //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(OPCServerDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   BOOL             EnumerateNodes(LPNETRESOURCE lpnr);
   BOOL             GetOPCServers(LPTSTR node = NULL);
   BOOL             m_Enumerated;
   OPCServerList    m_Servers;
   CLSID            m_catid;

   // Generated message map functions
   //{{AFX_MSG(OPCServerDlg)
   virtual BOOL OnInitDialog();
   afx_msg void OnDblclkServerList();
   afx_msg void OnRefresh();
   afx_msg void OnSelchangeNode();
   afx_msg void OnSelchangeServerList();
   afx_msg void OnDropdownNode();
	virtual void OnOK();
	afx_msg void OnServers10();
	afx_msg void OnServers20();
	//}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
