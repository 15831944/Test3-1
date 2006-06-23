// SysCAD.h : main header file for the SYSCAD application
//

#if !defined(AFX_SYSCAD_H__D3AF5224_6715_428C_9973_91CCBD6DBF7F__INCLUDED_)
#define AFX_SYSCAD_H__D3AF5224_6715_428C_9973_91CCBD6DBF7F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "SysCAD_i.h"

/////////////////////////////////////////////////////////////////////////////
// CSysCADApp:
// See SysCAD.cpp for the implementation of this class
//

class  CCmdLineHelper
  {
  public:
    bool Parse(char* pCmdLine);

    WORD          bAllowLoadPrevPrj:1,
                  bLoadPrevPrj:1,
                  bDebugOn:1,
                  bDeveloper:1,
                  bAutomation:1,
                  bAltLicenseLoc:1,
                  bForceNewCopy:1,
                  bUse2000:1,
                  bUse97:1,
                  bForceRuntime:1,
                  bForceDemo:1,
                  bForegroundWnd:1,
                  bMinimiseWnd:1,
                  bAllowLocalCopy:1,
                  bAllowXMenus:1;
    DWORD         dwAffinity;
    long          lGobbleMemUntil;
    long          nAutoRunWait;
    Strng         sAutoRunPrj;
    Strng         sAutoLoadPrj;
    Strng         sAutoLoadSnp;
    Strng         sDebugFile;
    Strng         sCmdLine;
    Strng         sLicenseLoc;
    Strng         sScriptCmd; // for a single command
    CStringArray  ScriptFiles;
  };

const int iProjectTemplate    = 0;
const int iGraphTemplate      = 1;
const int iVisioTemplate      = 2;
const int iTrendTemplate      = 3;
const int iControlTemplate    = 4;
const int iAutomationTemplate = 5;
/**
class CScdWinApp : public CWinApp
  {
  public:
	  CScdWinApp();
	  virtual ~CScdWinApp();

    virtual BOOL InitInstance();
    virtual int ExitInstance();
    virtual BOOL OnIdle(LONG lCount); // return TRUE if more idle processing
    virtual BOOL PreTranslateMessage(MSG* pMsg);

    flag          m_DoAutoLoadCLHPrj:1,
                  m_DoAutoLoadCLHSnp:1,
                  m_DoAutoRunCLHPrj:1,
                  m_DoLoadCLHPrj:1,
                  m_DoExecCLHScript:1,
                  m_WithLocalCopy:1,
                  m_DoOPCServerStartup:1;

    long          m_TimeOfLastIdle;


    CDocTemplate &  Template(int iType) { return *pTemplate[iType]; };
    BOOL            TemplateExists(int iType) { return pTemplate[iType]!=NULL; };

    CDocTemplate &  ProjectTemplate()  { return *pTemplate[iProjectTemplate]; };
    BOOL            ProjectTemplateExists()  { return pTemplate[iProjectTemplate]!=NULL; };
    CDocTemplate &  GraphTemplate()  { return *pTemplate[iGraphTemplate]; };
    BOOL            GraphTemplateExists()  { return pTemplate[iGraphTemplate]!=NULL; };
    CDocTemplate &  VisioTemplate()  { return *pTemplate[iVisioTemplate]; };
    BOOL            VisioTemplateExists()  { return pTemplate[iVisioTemplate]!=NULL; };
    CDocTemplate &  TrendTemplate() { return *pTemplate[iTrendTemplate]; };
    BOOL            TrendTemplateExists() { return pTemplate[iTrendTemplate]!=NULL; };
    CDocTemplate &  ControlTemplate() { return *pTemplate[iControlTemplate]; };
    BOOL            ControlTemplateExists() { return pTemplate[iControlTemplate]!=NULL; };
    CDocTemplate &  AutomationTemplate() { return *pTemplate[iAutomationTemplate]; };
    BOOL            AutomationTemplateExists() { return pTemplate[iAutomationTemplate]!=NULL; };

    int getTemplateCount() { return sizeof(pTemplate)/sizeof(pTemplate[0]); };

    CCmdLineHelper m_CLH;
    CDocTemplate* pTemplate[6];

    virtual LRESULT DoFlushMsgQ(WPARAM wParam, LPARAM lParam)=0;
    virtual void DoOnFileOpen()=0;

    BOOL InitInstStartup();
    BOOL InitIniFile();
    BOOL InitInstRegistry();
    BOOL InitInstDocTemplate();
    BOOL InitInstCmdLineOnly();
    BOOL InitInstFolder();
    BOOL InitInstLicense1(bool &LicenseFailed);
    BOOL InitInstLicense2(bool LicenseFailed);
    BOOL InitInstVersion();
    BOOL InitInstBaseCfg(int & CnvsLoadedCnt, Strng &CnvsFilename);
    BOOL InitInstWinNT();

    void BuildInterfaceWindows();

    int ShutDown();
    BOOL CanClose();
    void StdProfileChanged();
    CDocument* CScdWinApp::DocumentNo(int n);
    BOOL SaveAllModified(); // save before exit
  };

inline CScdWinApp * ScdApp() { return (CScdWinApp *)AfxGetApp(); };
*/



class CSysCADApp : public CWinApp
  {
  public:
	  CSysCADApp();

    int Run( );
    BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

    // Overrides
	  // ClassWizard generated virtual function overrides
	  //{{AFX_VIRTUAL(CSysCADApp)
	  public:
	  virtual BOOL InitInstance();
		  virtual int ExitInstance();
	  //}}AFX_VIRTUAL

  // Implementation
	  // CNM - COleTemplateServer m_server;
		  // Server object for document creation
	  //{{AFX_MSG(CSysCADApp)
	  afx_msg void OnAppAbout();
    afx_msg void OnHelpSyscad();
    afx_msg void OnHelpPgm();
    afx_msg void OnHelpModels();
    afx_msg void OnProjectSaveall();
    afx_msg void OnFileOpen();
    afx_msg void OnFileNew();
    afx_msg void OnUpdateFileNew(CCmdUI* pCmdUI);
    afx_msg void OnUpdateFileOpen(CCmdUI* pCmdUI);
    afx_msg void OnUpdateProjectSaveall(CCmdUI* pCmdUI);
    afx_msg void OnProjectOpen();
    afx_msg void OnUpdateProjectOpen(CCmdUI* pCmdUI);
    afx_msg void OnProjectOpenLocal();
    afx_msg void OnUpdateProjectOpenLocal(CCmdUI* pCmdUI);
    afx_msg void OnProjectClose();
    afx_msg void OnUpdateProjectCloseNoSave(CCmdUI* pCmdUI);
    afx_msg void OnProjectCloseNoSave();
    afx_msg void OnUpdateProjectClose(CCmdUI* pCmdUI);
    afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
    afx_msg void OnUpdateFileSaveAs(CCmdUI* pCmdUI);
    afx_msg void OnUpdateFileClose(CCmdUI* pCmdUI);
    afx_msg void OnUpdateAppExit(CCmdUI* pCmdUI);
    afx_msg void OnProjectNew();
    afx_msg void OnUpdateProjectNew(CCmdUI* pCmdUI);
    //}}AFX_MSG
    afx_msg void OnLicense(UINT nID);
    afx_msg void OnUpdateLicense(CCmdUI* pCmdUI);
    afx_msg BOOL OnOpenRecentFile(UINT nID);

    DECLARE_MESSAGE_MAP()
  private:
	  BOOL m_bATLInited;
  private:
	  BOOL InitATL();

#if !OBJECTSONTHREAD
    CComPtr<IScdASyncEvents>  m_ptheApp;
    DWORD                     m_dwRegActObj;
#endif

  public:

    BOOL DoPromptFileName(CString& fileName, UINT nIDSTitle, DWORD lFlags, BOOL bOpenFileDialog, CDocTemplate* pTemplate, BOOL AddAllFilter);
    void OnFileNewX();
    void OnFileOpenX();
    void DoOnFileOpen() { OnFileOpen(); };
    virtual LRESULT DoFlushMsgQ(WPARAM wParam, LPARAM lParam);

    void AddToRecentFileList(LPCTSTR lpszPathName);

    //virtual BOOL InitInstance();
    //virtual int ExitInstance();
    virtual BOOL OnIdle(LONG lCount); // return TRUE if more idle processing
    virtual BOOL PreTranslateMessage(MSG* pMsg);

    flag          m_DoAutoLoadCLHPrj:1,
                  m_DoAutoLoadCLHSnp:1,
                  m_DoAutoRunCLHPrj:1,
                  m_DoLoadCLHPrj:1,
                  m_DoAutoRunCLHMinimise:1,
                  m_DoExecCLHScript:1,
                  m_WithLocalCopy:1,
                  m_DoOPCServerStartup:1;

    long          m_DoAutoRunCLHWait;
    long          m_TimeOfLastIdle;
    long          m_HoldAddToRecentFileList;


    CDocTemplate &  Template(int iType) { return *pTemplate[iType]; };
    BOOL            TemplateExists(int iType) { return pTemplate[iType]!=NULL; };

    CDocTemplate &  ProjectTemplate()  { return *pTemplate[iProjectTemplate]; };
    BOOL            ProjectTemplateExists()  { return pTemplate[iProjectTemplate]!=NULL; };
    CDocTemplate &  GraphTemplate()  { return *pTemplate[iGraphTemplate]; };
    BOOL            GraphTemplateExists()  { return pTemplate[iGraphTemplate]!=NULL; };
    CDocTemplate &  VisioTemplate()  { return *pTemplate[iVisioTemplate]; };
    BOOL            VisioTemplateExists()  { return pTemplate[iVisioTemplate]!=NULL; };
    CDocTemplate &  TrendTemplate() { return *pTemplate[iTrendTemplate]; };
    BOOL            TrendTemplateExists() { return pTemplate[iTrendTemplate]!=NULL; };
    CDocTemplate &  ControlTemplate() { return *pTemplate[iControlTemplate]; };
    BOOL            ControlTemplateExists() { return pTemplate[iControlTemplate]!=NULL; };
    CDocTemplate &  AutomationTemplate() { return *pTemplate[iAutomationTemplate]; };
    BOOL            AutomationTemplateExists() { return pTemplate[iAutomationTemplate]!=NULL; };

    int getTemplateCount() { return sizeof(pTemplate)/sizeof(pTemplate[0]); };

    CCmdLineHelper m_CLH;
    CDocTemplate* pTemplate[6];

    //LRESULT DoFlushMsgQ(WPARAM wParam, LPARAM lParam);//=0;
    //void DoOnFileOpen() { OnFileOpen(); };//=0;

    BOOL InitInstStartup();
    BOOL InitIniFile();
    BOOL InitInstRegistry();
    BOOL InitInstDocTemplate();
    BOOL InitInstCmdLineOnly();
    BOOL InitInstFolder();
    BOOL InitInstLicense1(bool &LicenseFailed);
    BOOL InitInstLicense2(bool LicenseFailed);
    BOOL InitInstVersion();
    BOOL InitInstBaseCfg(int & CnvsLoadedCnt, Strng &CnvsFilename);
    BOOL InitInstWinNT();

    void BuildInterfaceWindows();

    int ShutDown();
    BOOL CanClose();
    void StdProfileChanged();
    CDocument* DocumentNo(int n);
    BOOL SaveAllModified(); // save before exit

//    afx_msg void OnAppExit();
};

inline CSysCADApp * ScdApp() { return (CSysCADApp *)AfxGetApp(); };

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SYSCAD_H__D3AF5224_6715_428C_9973_91CCBD6DBF7F__INCLUDED_)
