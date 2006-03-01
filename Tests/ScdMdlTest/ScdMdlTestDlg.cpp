// ScdMdlTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ScdMdlTest.h"
#include "ScdMdlTestDlg.h"

#include <atlbase.h>
#include "..\..\com\ScdMdl\scdcatids.h"
#include "..\..\..\SMDK\Include\scdcomtmpl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define DbgPoint() { int xxx=0; }

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

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
// CScdMdlTestDlg dialog

CScdMdlTestDlg::CScdMdlTestDlg(CWnd* pParent /*=NULL*/)
  : CDialog(CScdMdlTestDlg::IDD, pParent)
{
  //{{AFX_DATA_INIT(CScdMdlTestDlg)
    // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
  // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
  m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CScdMdlTestDlg::~CScdMdlTestDlg()
  {
  OnClear();
  }

void CScdMdlTestDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CScdMdlTestDlg)
  DDX_Control(pDX, IDC_SPMDLLIST, m_SpMdls);
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CScdMdlTestDlg, CDialog)
  //{{AFX_MSG_MAP(CScdMdlTestDlg)
  ON_WM_SYSCOMMAND()
  ON_WM_PAINT()
  ON_WM_QUERYDRAGICON()
  ON_WM_CLOSE()
  ON_BN_CLICKED(IDC_SPDB, OnSpdb)
  ON_BN_CLICKED(IDC_SPMODEL, OnSpmodel)
  ON_BN_CLICKED(IDC_USRSPMDL, OnUsrspmdl)
  ON_BN_CLICKED(IDC_CATEGORY, OnCategory)
  ON_BN_CLICKED(IDC_CLEAR, OnClear)
  ON_BN_CLICKED(IDC_INITIALISE, OnInitialise)
  ON_BN_CLICKED(IDC_ALL, OnAll)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScdMdlTestDlg message handlers

void InitMyCOM(); // forward


BOOL CScdMdlTestDlg::OnInitDialog()
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
  SetIcon(m_hIcon, TRUE);     // Set big icon
  SetIcon(m_hIcon, FALSE);    // Set small icon

  InitMyCOM();
  // TODO: Add extra initialization here

  return TRUE;  // return TRUE  unless you set the focus to a control
}

void CScdMdlTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CScdMdlTestDlg::OnPaint()
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
HCURSOR CScdMdlTestDlg::OnQueryDragIcon()
{
  return (HCURSOR) m_hIcon;
}
void CScdMdlTestDlg::OnClose()
{
    CDialog::OnClose();
}

void CScdMdlTestDlg::OnOK()
{
    CDialog::OnOK();
}

void CScdMdlTestDlg::OnCancel()
{
    CDialog::OnCancel();
}

// ============================================================================
//
//
//
// ============================================================================

class  CStopWatch
  {
  protected:
    LARGE_INTEGER Freq;
    bool bRunning;
    DWORD dwLaps;
    double dCurrent;
    double dAccum;
  public:
    double       Time()      { LARGE_INTEGER Tmp; QueryPerformanceCounter(&Tmp); return ((double)(Tmp.QuadPart))/Freq.QuadPart; }

    void         Start()     { dCurrent -= Time(); dwLaps++; bRunning=1; };
    void         Stop()      { dCurrent += Time(); bRunning=0; };
    double       Secs()      { return dCurrent + (bRunning ? Time(): 0.0); };
    void         ReStart()   { dCurrent=dAccum=0.0; Start(); };
    double       Lap()       { double d1=Secs(); double d2=d1-dAccum; dAccum=d1; dwLaps++; return d2; };
    void         Clear()     { dCurrent=dAccum=0.0; dwLaps=0; bRunning=0; };
    DWORD        Laps()      { return dwLaps; };

    CStopWatch()    { Clear(); QueryPerformanceFrequency(&Freq); }
  };

void CScdMdlTestDlg::LogIt(char * fmt, ...)
  {
  char S[10000]; 
  va_list argptr;
  va_start(argptr,fmt);
  vsprintf(S, fmt, argptr);
  va_end(argptr);
  
  m_SpMdls.AddString(S);
  }

// ============================================================================

//DWORD g_dwCookie;

UINT EO_ExecThread(LPVOID p)
  {
  CoInitializeEx(NULL, COINIT_MULTITHREADED);
  while (1)
    {
    if (1)
      {
      Sleep(100);
      CStopWatch SW;

//      if (g_spSpMdl.GetInterfacePtr())
//        {
//        }

      }
    }
  CoUninitialize();

  return 0;
  }

// ============================================================================
//
//
//
// ============================================================================

IGlobalInterfaceTable *g_pGIT=NULL;

void InitMyCOM()
  {
  if (1)
    {

    CoCreateInstance(CLSID_StdGlobalInterfaceTable,
                     NULL,
                     CLSCTX_INPROC_SERVER,
                     IID_IGlobalInterfaceTable,
                     (void **)&g_pGIT);

    CWinThread *pThread=NULL;
    if ((pThread=AfxBeginThread(EO_ExecThread, (LPVOID)NULL, THREAD_PRIORITY_NORMAL, 10000, CREATE_SUSPENDED))==NULL)
      {
      ::MessageBox(::GetFocus(),"Error Creating Object Exec Thread!","",MB_OK);
      }
    else
      {
      pThread->ResumeThread();
      }
    }
  }

// ============================================================================

void CScdMdlTestDlg::OnInitialise()
  {
  HRESULT hr=S_OK;

  if (g_spApp.GetInterfacePtr()==NULL)
    hr=g_spApp.CreateInstance(__uuidof(ScdApplication));
  if (FAILED(hr))
    DbgPoint();

  if (g_spEnv.GetInterfacePtr()==NULL)
    hr=g_spEnv.CreateInstance(__uuidof(ScdEnvironment));
  if (FAILED(hr))
    DbgPoint();

  CComBSTR CfgFile("C:\\SysCAD82\\AlcanGoveDyn\\AlcanGoveCOM.cfg");
  hr=g_spEnv->InitialiseCfg((BSTR)CfgFile);

  }

// ============================================================================

void CScdMdlTestDlg::OnSpdb()
  {
  if (g_spEnv.GetInterfacePtr())
    {
    HRESULT hr=g_spEnv.QueryInterface(IID_IScdSpDB, &g_spSpDB);
    if (FAILED(hr))
      LogIt("%s", "SpecieDB Error Return");
    else if (!g_spSpDB.GetInterfacePtr())
      LogIt("%s", "SpecieDB NULL PTR");
    else
      {
      LogIt("%s", "SpecieDB OK");
      
      long Cnt=g_spSpDB->GetSpecieCount(); 
      LogIt("Specie Count %i",Cnt); 
      for (int i=0; i<Cnt; i++)
        {
        _bstr_t S,T;   
        S=g_spSpDB->GetSymbol(i);
        T=g_spSpDB->GetTag(i);
        LogIt("%2i %s   [%s]",i, (LPCTSTR)S, (LPCTSTR)T); 
        }

    //    g_spSpDB.Globalize(spSpDB);

    //    CComBSTR CfgFile("ABC");
    //    HRESULT hr;
    //    LIP(IScdSpDB) lipDB(g_spSpDB, hr);

    //    lipDB->Open((BSTR)CfgFile);
      }
    }
  }

// ============================================================================

void CScdMdlTestDlg::OnSpmodel()
  {
  try
    {
    if (g_spSpDB.GetInterfacePtr())
      {
      double t,d,c,v1,v2;

      HRESULT hr=S_OK;
      IScdSpDBPtr spDB;
      IScdSpModelPtr spSpMdl;
      IScdSpPropsPtr spProps;
      IScdSpAluminaPropsPtr spAProps;

      spSpMdl.CreateInstance(__uuidof(ScdSpModel));
      if (FAILED(hr))
        DbgPoint();

      hr=spSpMdl.QueryInterface(IID_IScdSpDB, &spDB);
      if (FAILED(hr))
        DbgPoint();
      LogIt("Specie Cnt from Mdl I/F:%i", spDB->GetSpecieCount());
      
      CComBSTR sH2O("H2O(l)");
      long iH2O=spDB->GetSpecieIndex((BSTR)sH2O, false);
      spSpMdl->PutSpecieMass(iH2O, 1000.0);

      t=d=c=v1=v2=-1;

      t=spSpMdl->GetMass(spDB->GetSomAll());
      v1=spSpMdl->GetVolume(spDB->GetSomAll(), 273.16+25, 101.325);
      v2=spSpMdl->GetVolume(spDB->GetSomAll(), 273.16+100, 101.325);
      if (SUCCEEDED(spSpMdl.QueryInterface(IID_IScdSpProps, &spProps)))
        d=spProps->GetDensity(spDB->GetSomAll(),273.16+25, 101.325);
      if (SUCCEEDED(spSpMdl.QueryInterface(IID_IScdSpAluminaProps, &spAProps)))
        c=spAProps->GetC2S();

      LogIt("Mass:%g   V25:%g   V100:%g   Dens25:%g   C2S:%g", t,v1,v2,d,c);

      DbgPoint()
      }
    }
  catch (...)
    {
    DoBreak();
    }
  }

// ============================================================================

void CScdMdlTestDlg::OnUsrspmdl()
  {
  try
    {
    if (g_spSpDB.GetInterfacePtr())
      {
      double t,d,c,v1,v2;

      HRESULT hr=S_OK;
      IScdSpDBPtr DB;
      IDemoSMPtr spUsrMdl;
      IScdSpModelPtr spSpMdl;
      IScdSpPropsPtr spProps, spPropsx;
      IScdSpAluminaPropsPtr spAProps, spAPropsx;

      spUsrMdl.CreateInstance(__uuidof(DemoSM));
      if (FAILED(hr))
        DbgPoint();
      
      if (10)
        {
        hr=spUsrMdl.QueryInterface(IID_IScdSpModel, &spSpMdl);
        if (FAILED(hr))
          DbgPoint();

        hr=spUsrMdl.QueryInterface(IID_IScdSpDB, &DB);
        if (FAILED(hr))
          DbgPoint();
        LogIt("Specie Cnt from UsrMdl I/F:%i", DB->GetSpecieCount());

  //      // Check that we can go back
  //      IDemoSMPtr spUsrMdlXX;
  //      hr=spSpMdl.QueryInterface(IID_IDemoSM, &spUsrMdlXX);

        CComBSTR sH2O("H2O(l)");
        CComBSTR sAl("Al2O3(l)");
        CComBSTR sNa("NaOH(l)");
      
        long iH2O=DB->GetSpecieIndex((BSTR)sH2O, false);
        long iAl=DB->GetSpecieIndex((BSTR)sAl, false);
        long iNa=DB->GetSpecieIndex((BSTR)sNa, false);
        spSpMdl->PutSpecieMass(iH2O, 1000.0);
        for (int xxx=0; xxx<2; xxx++)
          {
          if (xxx>0)
            {
            spSpMdl->PutSpecieMass(iAl, 100.0);
            spSpMdl->PutSpecieMass(iNa, 100.0);
            }

          t=d=c=v1=v2=-1;

          t=spSpMdl->GetMass(DB->GetSomAll());
          v1=spSpMdl->GetVolume(DB->GetSomAll(), 273.16+25, 101.325);
          v2=spSpMdl->GetVolume(DB->GetSomAll(), 273.16+100, 101.325);
          if (SUCCEEDED(spSpMdl.QueryInterface(IID_IScdSpProps, &spProps)))
            d=spProps->GetDensity(DB->GetSomAll(), 273.16+25, 101.325);
          if (SUCCEEDED(spSpMdl.QueryInterface(IID_IScdSpAluminaProps, &spAProps)))
            c=spAProps->GetC2S();

          LogIt("Std Mass:%g   V25:%g   V100:%g   Dens25:%g   C2S:%g", t,v1,v2,d,c);

          t=d=c=v1=v2=-1;

          t=spSpMdl->GetMass(DB->GetSomAll());
          v1=spSpMdl->GetVolume(DB->GetSomAll(), 273.16+25, 101.325);
          v2=spSpMdl->GetVolume(DB->GetSomAll(), 273.16+100, 101.325);
          if (SUCCEEDED(spUsrMdl.QueryInterface(IID_IScdSpProps, &spPropsx)))
            d=spPropsx->GetDensity(DB->GetSomAll(), 273.16+25, 101.325);
          if (SUCCEEDED(spUsrMdl.QueryInterface(IID_IScdSpAluminaProps, &spAPropsx)))
            c=spAPropsx->GetC2S();

          LogIt("Usr Mass:%g   V25:%g   V100:%g   Dens25:%g   C2S:%g", t,v1,v2,d,c);
          }
        }

      DbgPoint()
      }
    }
  catch (...)
    {
    DoBreak();
    }

  }

// ============================================================================

void CScdMdlTestDlg::OnCategory()
  {
  //  m_SpMdls.ResetContent();

  // We want to get all classes that are members of the
  // Named Drawing Objects COM category.
  ICatInformation *pCatInfo = NULL;
  HRESULT hr;

  // Get the categories manager.
  hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr, 0,
              CLSCTX_SERVER, IID_ICatInformation, (void**)&pCatInfo);

  // Now get em!
  if(SUCCEEDED(hr))
    {
    IEnumCLSID *pCLSIDS = NULL;
    CATID catids[1];
    catids[0] = CATID_SpecieModel;

    hr = pCatInfo->EnumClassesOfCategories(1, catids, -1, 0, &pCLSIDS);
    CLSID clsid[20];
    LPOLESTR progID;

    do
      {
      DWORD numb = 0;
      hr = pCLSIDS->Next(20, clsid, &numb);
      if(SUCCEEDED(hr))
        {
        for(DWORD i = 0; i < numb; i++)
          {
          ProgIDFromCLSID(clsid[i], &progID);
          char buff[30];
          WideCharToMultiByte(CP_ACP, NULL, progID, -1, buff, 30,
                    NULL, NULL);
          m_SpMdls.AddString(buff);
          }
        }

      }
    while(hr == S_OK);
    pCLSIDS->Release();

    }
  pCatInfo->Release();

  }

// ============================================================================

void CScdMdlTestDlg::OnClear()
  {
  //if (spSpMdl.GetInterfacePtr())
  //  spSpMdl.Release();
  //if (spProps.GetInterfacePtr())
  //  spProps.Release();
  //if (spAProps.GetInterfacePtr())
  //  spAProps.Release();
  //if (spUProps.GetInterfacePtr())
  //  spUProps.Release();
  //if (spUAProps.GetInterfacePtr())
  //  spUAProps.Release();

  if (g_spSpDB.GetInterfacePtr())
    g_spSpDB.Release();
  
  if (g_spEnv.GetInterfacePtr())
    g_spEnv.Release();
  }

// ============================================================================

void CScdMdlTestDlg::OnAll()
  {
  OnInitialise();
  OnSpdb();
  OnSpmodel();
  //OnUsespmdl();

  OnClear();
  } 

