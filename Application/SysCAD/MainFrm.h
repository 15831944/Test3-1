//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __MAINFRM_H
#define  __MAINFRM_H

#ifndef __EXECUTIV_H
#include "executiv.h"
#endif
#ifndef __GPWFUNCS_H
#include "gpwfuncs.h"
#endif

//---------------------------------------------------------------------------

class CMyMDIClient : public CWnd
  {
  DECLARE_DYNAMIC(CMyMDIClient)
  public:
    CMyMDIClient();
    //{{AFX_VIRTUAL(CMyMDIClient)
    //}}AFX_VIRTUAL
  public:
    virtual ~CMyMDIClient();
  protected:
    //{{AFX_MSG(CMyMDIClient)
    afx_msg void OnPaint();
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  };

//---------------------------------------------------------------------------
#if DotNet
typedef DWORD DOTNETHTASK;
#else
typedef HTASK DOTNETHTASK;
#endif
_FWDDEF(CGenSheet)
_FWDDEF(CPrjSheet)
_FWDDEF(CReportsDlg)
_FWDDEF(CScriptsDlg)
_FWDDEF(CCompareResultsDlg)
_FWDDEF(CDDEStatsDlg)
_FWDDEF(CMainFrame);
class CMainFrame : public CMDIFrameWnd, CExecObj
  {
  friend class CToolbarPage;
  friend class Project;
  DECLARE_DYNAMIC(CMainFrame)
  public:
    CGenSheet*      m_pGenSheet;      //pointer to modeless general property sheet, NULL if not open
    CPrjSheet*      m_pPrjSheet;      //pointer to modeless project property sheet, NULL if not open
    CReportsDlg*    m_pReportsDlg;    //pointer to modeless reports dialog box, NULL if not open
    CScriptsDlg*    m_pCmdScriptsDlg; //pointer to modeless command scripts dialog box, NULL if not open
    CCompareResultsDlg* m_pCompareResDlg; //pointer to modeless compare results dialog box, NULL if not open
    CMyMDIClient    m_MDIClientWnd;   //sub-classed MDI framework client window

    HANDLE          hCfgEditProcess;    //process handle for the editor
    DWORD           dwCfgEditProcessId; //process ID for the editor

    CBitmap         m_ErrorBitmap;
    CBitmap         m_WarnBitmap;
    CBitmap         m_NoteBitmap;
    CBitmap         m_ErrorBWBitmap;
    CBitmap         m_WarnBWBitmap;
    CBitmap         m_NoteBWBitmap;
    CBitmap         m_MsgIcon;

    Strng           m_GoodStr;
    Strng           m_BadStr;
    Strng           m_StepStr;
    
    long            m_nMsgs[3];


    CMainFrame();
    virtual ~CMainFrame();
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    //virtual BOOL PreTranslateMessage(MSG* pMsg);
  	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);
    
    void UpdateStatusBar();
    void UpdateMainToolBar();
    void UpdateToolBars();
    void DefaultToolBar(int index = -1, BOOL ShowNow = FALSE);
    void SetToolBarSolveMode();
    void ClosingProject(flag ForExit);
    
    // ExecObj Overridables
    // Defeat Protected Attribute on CExecObj
    flag           EO_Register(const char* Name_, byte bExec_, int RqdPriority, DWORD StackSize, int HashSize=17)
      { return CExecObj::EO_Register(Name_, bExec_, RqdPriority, StackSize, HashSize); };
    flag           EO_DeRegister()
      { return CExecObj::EO_DeRegister(); };
    virtual flag   EO_Starting(flag fBeginStarting);
    virtual flag   EO_Stopping(flag fBeginStopping);
    virtual flag   EO_RequestTagInfo(RequestTagInfoRec& Rqst, ReplyTagInfoRec& Info);

    virtual flag   EO_QueryTime(const CXM_TimeControl &CB, double &TimeRqd, double &dTimeRqd);
    virtual flag   EO_Start(const CXM_TimeControl &CB);
    virtual void   EO_QuerySubsReqd(CXMsgLst &XM);
    virtual void   EO_QuerySubsAvail(CXMsgLst &XM, CXMsgLst &XMRet);
    virtual flag   EO_ReadSubsData(CXMsgLst &XM);  
    virtual flag   EO_WriteSubsData(CXMsgLst &XM, flag FirstBlock, flag LastBlock);  
    virtual flag   EO_Execute(const CXM_TimeControl &CB, CEOExecReturn &EORet);
    virtual flag   EO_Stop(const CXM_TimeControl &CB);


    #ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
    #endif
  
  protected:
    CCustomStatusBar m_wndStatusBar;  // application status bar
    //CToolBar      m_wndToolBar;

    void DockControlBarRelative(CToolBar* Bar, CToolBar* RelativeTo, flag Left);
    void DoReports();
    bool DeleteMenuItem(UINT ItemID);
    flag RecordTag(LPCTSTR Tag, flag RecordingOn);


    //{{AFX_MSG(CMainFrame)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnClose();
    afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
    afx_msg void OnProjectOptions();
    afx_msg void OnOptionsBrowsetags();
    afx_msg void OnOptionsHistorianQuery();
    afx_msg void OnOptionsDriver();
    afx_msg void OnOptionsArchive();
    afx_msg void OnFileSavesnapshot();
    afx_msg void OnFileLoadsnapshot();
    afx_msg void OnFileSavescenario();
    afx_msg void OnFileLoadscenario();
    afx_msg void OnProjectSaveas();
    afx_msg void OnProjectSaveasNV();
    afx_msg void OnChangeTag();
    afx_msg void OnBulkChangeTag();
    afx_msg void OnFileSavelayout();
    afx_msg void OnFindTag();
    afx_msg void OnFindNext();
    afx_msg void OnOptionsReports();
    afx_msg void OnAnalyse();
    afx_msg void OnUpdateAnalyse(CCmdUI* pCmdUI);
    afx_msg void OnProjectEmpty();
    afx_msg void OnUpdateProjectEmpty(CCmdUI* pCmdUI);
    afx_msg void OnProjectZeroFlows();
    afx_msg void OnUpdateProjectZeroFlows(CCmdUI* pCmdUI);
    afx_msg void OnProjectSteadystate();
    afx_msg void OnUpdateProjectSteadystate(CCmdUI* pCmdUI);
    afx_msg void OnUpdateFileLoadsnapshot(CCmdUI* pCmdUI);
    afx_msg void OnUpdateFileSavesnapshot(CCmdUI* pCmdUI);
    afx_msg void OnUpdateFileLoadscenario(CCmdUI* pCmdUI);
    afx_msg void OnUpdateFileSavescenario(CCmdUI* pCmdUI);
    afx_msg void OnUpdateFileSavelayout(CCmdUI* pCmdUI);
    afx_msg void OnUpdateOptionsBrowsetags(CCmdUI* pCmdUI);
    afx_msg void OnUpdateOptionsHistorianQuery(CCmdUI* pCmdUI);
    afx_msg void OnUpdateOptionsDriver(CCmdUI* pCmdUI);
    afx_msg void OnUpdateOptionsArchive(CCmdUI* pCmdUI);
    afx_msg void OnUpdateOptionsReports(CCmdUI* pCmdUI);
    afx_msg void OnUpdateProjectSaveas(CCmdUI* pCmdUI);
    afx_msg void OnUpdateProjectSaveasNV(CCmdUI* pCmdUI);
    afx_msg void OnProjectConfigure();
    afx_msg void OnUpdateProjectConfigure(CCmdUI* pCmdUI);
    afx_msg void OnUpdateFindTag(CCmdUI* pCmdUI);
    afx_msg void OnUpdateFindNext(CCmdUI* pCmdUI);
    afx_msg void OnUpdateProjectOptions(CCmdUI* pCmdUI);
    afx_msg void OnOptionsCmdscripts();
    afx_msg void OnOptionsCompare();
    afx_msg void OnUpdateOptionsCompare(CCmdUI* pCmdUI);
    afx_msg void OnInitMenu(CMenu* pMenu);
    afx_msg void OnUpdateChangeTag(CCmdUI* pCmdUI);
    afx_msg void OnUpdateBulkChangeTag(CCmdUI* pCmdUI);
    afx_msg void OnUpdateOptionsCmdscripts(CCmdUI* pCmdUI);
    afx_msg void OnProjectMerge();
    afx_msg void OnUpdateProjectMerge(CCmdUI* pCmdUI);
    afx_msg void OnProjectPreset();
    afx_msg void OnUpdateProjectPreset(CCmdUI* pCmdUI);
    afx_msg void OnProjectResetStats();
    afx_msg void OnUpdateProjectResetStats(CCmdUI* pCmdUI);
    afx_msg void OnTagDataTransfer();
    afx_msg void OnUpdateTagDataTransfer(CCmdUI* pCmdUI);
    afx_msg void OnProjectLoadaltcfg();
    afx_msg void OnUpdateProjectLoadaltcfg(CCmdUI* pCmdUI);
    afx_msg void OnOptionsDDE();
    afx_msg void OnUpdateOptionsDDE(CCmdUI* pCmdUI);
    afx_msg void OnOptionsOPCServer();
    afx_msg void OnUpdateOptionsOPCServer(CCmdUI* pCmdUI);
    afx_msg void OnProjectEditCfg();
    afx_msg void OnUpdateProjectEditCfg(CCmdUI* pCmdUI);
    afx_msg void OnProjectEditSdb();
    afx_msg void OnUpdateProjectEditSdb(CCmdUI* pCmdUI);
    afx_msg void OnProjectEditDefSdb();
    afx_msg void OnUpdateProjectEditDefSdb(CCmdUI* pCmdUI);
    afx_msg void OnViewPowersupplies();
    afx_msg void OnUpdateViewPowersupplies(CCmdUI* pCmdUI);
    afx_msg void OnViewIOPoints();
    afx_msg void OnUpdateViewIOPoints(CCmdUI* pCmdUI);
    afx_msg void OnViewChangestats();
    afx_msg void OnUpdateViewChangestats(CCmdUI* pCmdUI);
    afx_msg void OnViewEvalorder();
    afx_msg void OnUpdateViewEvalorder(CCmdUI* pCmdUI);
    afx_msg void OnViewEvalstats();
    afx_msg void OnUpdateViewEvalstats(CCmdUI* pCmdUI);
    afx_msg void OnViewControls();
    afx_msg void OnUpdateViewControls(CCmdUI* pCmdUI);
    afx_msg void OnViewDynamicinfo();
    afx_msg void OnUpdateViewDynamicinfo(CCmdUI* pCmdUI);
    afx_msg void OnViewPlantmodel();
    afx_msg void OnUpdateViewPlantmodel(CCmdUI* pCmdUI);
    afx_msg void OnViewPlantarea();
    afx_msg void OnUpdateViewPlantarea(CCmdUI* pCmdUI);
    afx_msg void OnViewProbalinfo();
    afx_msg void OnUpdateViewProbalinfo(CCmdUI* pCmdUI);
    afx_msg void OnViewSpeciedata();
    afx_msg void OnUpdateViewSpeciedata(CCmdUI* pCmdUI);
    afx_msg void OnViewTearblocks();
    afx_msg void OnUpdateViewTearblocks(CCmdUI* pCmdUI);
    afx_msg void OnViewTolerances();
    afx_msg void OnUpdateViewTolerances(CCmdUI* pCmdUI);
    afx_msg void OnViewRptTagLists();
    afx_msg void OnUpdateViewRptTagLists(CCmdUI* pCmdUI);
    afx_msg void OnViewMessages();
    afx_msg void OnUpdateViewMessages(CCmdUI* pCmdUI);
    afx_msg void OnViewProject();
    afx_msg void OnUpdateViewProject(CCmdUI* pCmdUI);
    afx_msg void OnError();
    afx_msg void OnUpdateError(CCmdUI* pCmdUI);
    afx_msg void OnWarning();
    afx_msg void OnUpdateWarning(CCmdUI* pCmdUI);
    afx_msg void OnActivateApp(BOOL bActive, DOTNETHTASK hTask);
    afx_msg void OnEXECDumpMsgQueue();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnMove(int x, int y);
    afx_msg void OnProjectCompareData();
    afx_msg void OnUpdateProjectCompareData(CCmdUI* pCmdUI);
	  afx_msg void OnActionsProbalSetup();
	  afx_msg void OnUpdateActionsProbalSetup(CCmdUI* pCmdUI);
	  afx_msg void OnActionsDynamicSetup();
	  afx_msg void OnUpdateActionsDynamicSetup(CCmdUI* pCmdUI);
	  afx_msg void OnActionsSetup();
	  afx_msg void OnUpdateActionsSetup(CCmdUI* pCmdUI);
	  afx_msg void OnActionsEdit();
	  afx_msg void OnUpdateActionsEdit(CCmdUI* pCmdUI);
	  afx_msg void OnActionsRun();
	  afx_msg void OnUpdateActionsRun(CCmdUI* pCmdUI);
	  afx_msg void OnActionsRunSteady();
	  afx_msg void OnUpdateActionsRunSteady(CCmdUI* pCmdUI);
	  afx_msg void OnActionsEditTgl();
	  afx_msg void OnUpdateActionsEditTgl(CCmdUI* pCmdUI);
	  afx_msg void OnActionsRunTgl();
	  afx_msg void OnUpdateActionsRunTgl(CCmdUI* pCmdUI);
	  afx_msg void OnUpdateActionsRunPBTgl(CCmdUI* pCmdUI);
	  afx_msg void OnUpdateActionsRunDynTgl(CCmdUI* pCmdUI);
	  afx_msg void OnActionsIdle();
	  afx_msg void OnUpdateActionsIdle(CCmdUI* pCmdUI);
    afx_msg void OnActionsStep();
    afx_msg void OnUpdateActionsStep(CCmdUI* pCmdUI);
    afx_msg void OnActionsSolveNet();
    afx_msg void OnUpdateActionsSolveNet(CCmdUI* pCmdUI);
    afx_msg void OnActionsRunPB();
    afx_msg void OnUpdateActionsRunPB(CCmdUI* pCmdUI);
    afx_msg void OnActionsRunDyn();
    afx_msg void OnUpdateActionsRunDyn(CCmdUI* pCmdUI);
    afx_msg void OnReloadDriver();
    afx_msg void OnUpdateReloadDriver(CCmdUI* pCmdUI);
#if USESCDEXPLORER
    afx_msg void OnExplorer();
    afx_msg void OnUpdateExplorer(CCmdUI* pCmdUI);
#else
    afx_msg void OnWindowSelectwindow();
    afx_msg void OnUpdateWindowSelectwindow(CCmdUI* pCmdUI);
#endif
    afx_msg void OnWindowArrange();
    afx_msg void OnUpdateWindowArrange(CCmdUI* pCmdUI);
    afx_msg void OnWindowCascade();
    afx_msg void OnWindowTile();
    afx_msg void OnWindowArrangeIcons();
    afx_msg void OnExportDB();
    afx_msg void OnUpdateExportDB(CCmdUI* pCmdUI);
    afx_msg void OnImportDB();
    afx_msg void OnUpdateImportDB(CCmdUI* pCmdUI);
    //}}AFX_MSG
    afx_msg void OnUpdateWindowCmd(CCmdUI* pCmdUI);
#if WITHSCRCYCLES
    afx_msg void OnTimer(UINT nIDEvent);
    afx_msg void OnViewScreensaver();
    afx_msg void OnUpdateViewScreensaver(CCmdUI* pCmdUI);
#endif
    afx_msg void OnOptionsDebugVisible();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnMdlHelpMenu(UINT nID);
    afx_msg void OnMeasureItem( int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct );
    afx_msg void OnDrawItem( int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct );
#if WITHSCRCYCLES
    afx_msg LRESULT OnStartCycles(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEndCycles(WPARAM wParam, LPARAM lParam);
#endif
    afx_msg LRESULT OnUpdateMainWnd(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnHstUpdateStats(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnTagAction(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnGenerateReport(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnCmd(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnCmdDone(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnCmdNext(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnCmdState(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnOLEInfoUpdate(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnAddToRptTagLists(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnUpdateMsgMenuBtn(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnFlushMsgQ(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnReloadDriver(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnCOMCmd(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnCOMApp(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnCOMSlv(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnCOMEvtRelay(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnExecUpdateDisplay(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnExecAction(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnGrfTagGroups(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnNodeAudit(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEditRctDlg(WPARAM wParam, LPARAM lParam);

    DECLARE_MESSAGE_MAP()

    void GoToIdleMode();
  public:
    afx_msg void OnFileImport();
    afx_msg void OnUpdateFileImport(CCmdUI *pCmdUI);
    afx_msg void OnFileExport();
    afx_msg void OnUpdateFileExport(CCmdUI *pCmdUI);
    afx_msg void OnDeleteDebugFile();
};

//inline CMainFrame* MainWnd() { return (CMainFrame*)SysCAD.m_pMainWnd; };
//inline CMainFrame* MainWnd() { return (CMainFrame*)AfxGetMainWnd(); }; //this causes a crash in release!

// force to always get app window even when a server
inline CMainFrame* MainWnd() { return (CMainFrame*)(CMainFrame*)AfxGetApp()->GetMainWnd(); };

//===========================================================================

#endif