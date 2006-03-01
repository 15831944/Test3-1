// TestMarshalDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TestMarshal.h"
#include "TestMarshalDlg.h"
#include "DlgProxy.h"
//#include <atlbase.h>
#if WITHSCD 
#include "..\..\SysCADMarshal\SysCADMarshal_i.c"
#else
#include "D:\Temp\XXYY1\XXYY1_i.c"
#include "D:\Temp\XXYY1\XXYY1CP.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

int LogError(LPCTSTR Where, UINT BoxFlags, LPCTSTR fmt, ...)
  {
  _asm int 3;
  return 0;
  };
int LogWarning(LPCTSTR Where, UINT BoxFlags, LPCTSTR fmt, ...)
  {
  _asm int 3;
  return 0;
  };
int LogNote(LPCTSTR Where, UINT BoxFlags, LPCTSTR fmt, ...)
  {
  _asm int 3;
  return 0;
  };

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestMarshalDlg dialog

IMPLEMENT_DYNAMIC(CTestMarshalDlg, CDialog);

CTestMarshalDlg::CTestMarshalDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestMarshalDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTestMarshalDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pAutoProxy = NULL;
}

CTestMarshalDlg::~CTestMarshalDlg()
{
	// If there is an automation proxy for this dialog, set
	//  its back pointer to this dialog to NULL, so it knows
	//  the dialog has been deleted.
	if (m_pAutoProxy != NULL)
		m_pAutoProxy->m_pDialog = NULL;
}

void CTestMarshalDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestMarshalDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTestMarshalDlg, CDialog)
	//{{AFX_MSG_MAP(CTestMarshalDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_CONNECT, OnConnect)
	ON_BN_CLICKED(IDC_DISCONNECT, OnDisconnect)
	ON_BN_CLICKED(IDC_QUERYTIME, OnQuerytime)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestMarshalDlg message handlers

BOOL CTestMarshalDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTestMarshalDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTestMarshalDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTestMarshalDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

// Automation servers should not exit when a user closes the UI
//  if a controller still holds on to one of its objects.  These
//  message handlers make sure that if the proxy is still in use,
//  then the UI is hidden but the dialog remains around if it
//  is dismissed.

void CTestMarshalDlg::OnClose() 
{
	if (CanExit())
		CDialog::OnClose();
}

void CTestMarshalDlg::OnOK() 
{
	if (CanExit())
		CDialog::OnOK();
}

void CTestMarshalDlg::OnCancel() 
{
	if (CanExit())
		CDialog::OnCancel();
}

BOOL CTestMarshalDlg::CanExit()
{
	// If the proxy object is still around, then the automation
	//  controller is still holding on to this application.  Leave
	//  the dialog around, but hide its UI.
	if (m_pAutoProxy != NULL)
	{
		ShowWindow(SW_HIDE);
		return FALSE;
	}

	return TRUE;
}

void CTestMarshalDlg::OnConnect() 
{
USES_CONVERSION;

#if WITHSCD 
  if (m_Marshal.IsOk())
    return;
#else
  if (m_XXXYYY.IsOk())
    return;
#endif

  HRESULT hr = S_OK;

  // Create a running object from the class ID
  LPUNKNOWN pUnkn = NULL;

  DWORD ClsCtx=CLSCTX_LOCAL_SERVER | CLSCTX_REMOTE_SERVER;
  LPCSTR Node=NULL;
  if(0)//!Node || strlen(Node)==0)
    {
#if WITHSCD 
    hr = CoCreateInstance(CLSID_ScdMarshal, NULL, ClsCtx/*CLSCTX_ALL*/, IID_IScdMarshal, (LPVOID *)&pUnkn);
#else
    hr = CoCreateInstance(CLSID_XXXYYY1, NULL, ClsCtx/*CLSCTX_ALL*/, IID_IUnknown, (LPVOID *)&pUnkn);
#endif
    if( FAILED(hr) || pUnkn == NULL)
      {
      LogError("Marshal", 0, "Error connecting to Marshal server %08x ", hr);
      return;
      }
    }
  else  // use the node name
    {
    COSERVERINFO si={0};
    MULTI_QI  qi;

    si.dwReserved1 = 0;
    si.pwszName = NULL;
    if (Node && strlen(Node)>0)
      si.pwszName = T2OLE(Node);
    si.pAuthInfo = NULL;
    si.dwReserved2 = 0;

#if WITHSCD 
    qi.pIID = &IID_IScdMarshal;
#else
    qi.pIID = &IID_IXXXYYY1;
#endif
    qi.pItf = NULL;
    qi.hr = 0;

#if WITHSCD 
    hr = CoCreateInstanceEx(CLSID_ScdMarshal, NULL, ClsCtx, &si, 1, &qi);
#else
    hr = CoCreateInstanceEx(CLSID_XXXYYY1, NULL, ClsCtx, &si, 1, &qi);
#endif
    if (FAILED(hr))
      {
      LogError("Marshal", 0, "Error connecting to OPC server '%s'");
      }
    if (FAILED(qi.hr))
      {
      LogError("Marshal", 0, "???");//ReportError(m_sName, qi.hr, _T("MultiQI: "));
      }
    pUnkn = qi.pItf;
    }


#if WITHSCD
  hr=m_Marshal.Attach(pUnkn );
#else
  hr=m_XXXYYY.Attach(pUnkn );
#endif
  // Get the IOPCServer interface.
//  hr = m_Marshal.Attach( pUnkn );
  pUnkn->Release();  // Don't need this anymore.
  pUnkn = NULL;
  if( FAILED(hr) )
    {
    CString appName((LPCSTR)AFX_IDS_APP_TITLE);
    LogError("Marshal", 0, _T("You may not have registered the OPC Proxy dll!\n"), appName, MB_OK);
    return ;
    }

//  hr = m_XObject.Attach(m_Marshal);
//  if( FAILED(hr) )
//    {
//    CString appName((LPCSTR)AFX_IDS_APP_TITLE);
//    LogError("Marshal", 0, _T("You may not have registered the OPC Proxy dll!\n"), appName, MB_OK);
//    return ;
//    }

  IConnectionPointContainer *pCPC = 0;
#if WITHSCD
  hr = m_Marshal.QueryInterface(IID_IConnectionPointContainer, (void**)&pCPC);
#else
  hr = m_ExecObject.QueryInterface(IID_IConnectionPointContainer, (void**)&pCPC);
#endif
  if( SUCCEEDED(hr) )  // This server supports 2.0
    {
    IConnectionPoint *pCallbackCP = 0;
#if WITHSCD
    hr = pCPC->FindConnectionPoint(IID__IScdMarshalEvents, &pCallbackCP);
#else
    hr = pCPC->FindConnectionPoint(IID__IXXXYYY1Events, &pCallbackCP);
#endif
    pCPC->Release();
    if( SUCCEEDED(hr) )
      {
      int xxx=0;
      DWORD dwXX;
//      CProxy_IScdExecObjectEvents *pCP=NULL;

      CScdMarshalEventsObject * pMarshalEvents=new CScdMarshalEventsObject;

//      m_pCallbackCP = new OPCCallbackObject;   // create the ConnectionPoint for notifications
//  m_pCallbackCP->AddRef();

      hr = pCallbackCP->Advise(pMarshalEvents, &dwXX);
      pCallbackCP->Release();
      if( FAILED(hr) )
        {
        //portError("XXXX", 0 _T("Advise: IID_IOPCShutdown") );
        return ;
        }
      }
    }
  
/**
  // Create a single group that will contain all the items
  FLOAT deadband = 0.0;
  DWORD rate;
  BOOL GroupActive = FALSE; //what is best for this???
  hr = m_OpcServer.AddGroup( T2OLE(GroupName), GroupActive, RqdRate,  // name, active, rate
                           1324, NULL, &deadband,  // handle, bias, band
                           0, &m_GroupHandle, &rate,
                           IID_IOPCGroupStateMgt,   // interface to return
                           m_OpcGroup );              // this holds the group ptr
  if( FAILED(hr) )

	// TODO: Add your control notification handler code here
**/	
  }

void CTestMarshalDlg::OnDisconnect() 
  {
#if WITHSCD
  m_Marshal.Detach();
#else
  m_XXXYYY.Detach();
#endif
  //  m_XObject.Detach();
  }

void CTestMarshalDlg::OnQuerytime() 
  {
//  HRESULT hr = m_XObject.m_pI->ApplicationActivate(TRUE);
//  
//  SCDX_TIMECONTROL TmCB;
//  memset(&TmCB, 0, sizeof(TmCB));
//  TmCB.m_dTime=1.2345;
//  VARIANT_BOOL Ret;
//  hr = m_XObject.m_pI->QueryTime(&TmCB,&Ret);
//  
  int xxxx=0;  
  }
