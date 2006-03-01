// PLC5Dlg.h : header file
//

#if !defined(AFX_PLC5DLG_H__4ADAF474_F3F5_49D4_A72F_9AD9F2C95554__INCLUDED_)
#define AFX_PLC5DLG_H__4ADAF474_F3F5_49D4_A72F_9AD9F2C95554__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "afxtempl.h"
#include "PLC.h"

//---------------------------------------------------------------------------

CString FnDrivePath(LPCTSTR FilePath);
CString FnNameExt(LPCTSTR FilePath);
CString FnDrivePathName(LPCTSTR FilePath);
CString FnPathNameExt(LPCTSTR FilePath);
int FileExists(LPCTSTR FileName);
int FnCreatePath(LPCTSTR pPath);

/////////////////////////////////////////////////////////////////////////////
// CPLC5Dlg dialog

class CPLC5Dlg : public CDialog
  {
// Construction
public:
	CPLC5Dlg(CWnd* pParent = NULL);	// standard constructor
	~CPLC5Dlg();	// standard destructor

// Dialog Data
	//{{AFX_DATA(CPLC5Dlg)
	enum { IDD = IDD_PLC5_DIALOG };
	CStatic	m_TbHeader;
	CStatic	m_LdHeader;
	CTreeCtrl	m_Tree;
	CStatic	m_LdStatic;
	CStatic	m_TbStatic;
	CScrollBar	m_TbScroll;
	CScrollBar	m_LdScroll;
	CString	m_State;
	int		m_DisplayFmt;
	CString	m_StatusMsg;
	CString	m_sGoto;
	BOOL	m_Remote;
	BOOL	m_Reset;
	float	m_RunMult;
	UINT	m_StepMS;
	UINT	m_IterMS;
	UINT	m_UpdateMS;
	CString	m_ExecTime;
	CString	m_UpdTime;
	BOOL	m_ChgMonitorOn;
	//}}AFX_DATA

  CLadderDisplayList    m_DispLst;

  CPLC5Array            m_Plc;
  CPLC5OPCCallBack     *m_MyOPCCallBack; 

  CTransferBlkArray     m_Xfer;
  CForceBlkArray        m_Force;

  long                  m_TimeTillUpdate;

  CString               m_sInitFile;
  long                  m_lSequenceNo;
  
  long                  m_HistLine1;
  
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPLC5Dlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
//protected:
public:

	void PaintLadder(CPaintDC & dc, CRect & StaticCRct, CRect & TheRct, CPoint &TheOff);
	void PaintTable(CPaintDC & dc, CRect & TheCRct, CRect & TheRct, CPoint &TheOff);
	void PaintChangeHistory(CPaintDC & dc, CRect & TheCRct, CRect & TheRct, CPoint &TheOff);
	void PaintDynamics(CPaintDC & dc);
  void RedrawTheDisplay(byte What=0xff, UINT flags = RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE );
	void FixScrollBar();
  void SetRunState(int iState=-1);
  void SetUpDlg();

  HICON m_hIcon;
  

  CFont m_TheFontL;
  CFont m_TheFontT;
  byte m_HeightMax;
  int  m_LastTxtCol;

  int  m_iPcIndex;
  int  m_iTbIndex;
  int  m_iLdIndex;
  int  m_iWdIndex;
  int  m_iBtIndex;

  int  m_iState;
  int  m_iReqdState;
  int  m_iStepEndState;

  CPoint m_CharSz;
  CPoint m_TbShft;
  CPoint m_LdShft;

  UINT m_TimerID;

  double  m_EvalTime;
  double  m_DispTime;
  long    m_nStepsReqd;
  long    m_nItsPerStep;
  long    m_nActualIterMS;
  long    m_nStepNo;
  long    m_nIterNo;
  bool    m_RunningSlow;
  DWORD   m_RunningTicks;

  CBitmap m_StoppedBitmap;
  CBitmap m_RunningBitmap;
  CBitmap m_StepBitmap;

  bool    m_OPCServerStarted;

public:
    //void FinishOPCCommand(DWORD Resp);
  int  Open(LPCTSTR FNFullFile);
  int  Open(LPCTSTR FNRoot, LPCTSTR FNFile);
  int  Close();

  BOOL OPCStart();
  void OPCStop();
  BOOL OPCRegister();
  BOOL OPCUnregister();

//protected:
	void AddCommsBlk(LPCTSTR DstP, LPCTSTR DstA, LPCTSTR SrcP, LPCTSTR SrcA, long Len);
	void AddForceBlk(LPCTSTR DstP, LPCTSTR DstA, long Len, float * Values);
	void BuildPrj(LPCTSTR FNRoot, LPCTSTR FNPrj);
  int  AddAPlc(int N, LPCTSTR FNRoot, LPCTSTR FNFile, LPCTSTR FnOptProfile);

  void GotoStop();
  void GotoRun();
  void GotoIdle();
  void StartTheTimer();
  void StopTheTimer();
  bool TimerRunning() { return m_TimerID!=0; }

  int DoLoadSnapshot(LPCTSTR Fn);
  int DoSaveSnapshot(LPCTSTR Fn);
  int DoLoadBacktrack(LPCTSTR Fn);
  int DoSaveBacktrack(LPCTSTR Fn);

  void ActivateInterface(bool On);
  
	// Generated message map functions
	//{{AFX_MSG(CPLC5Dlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnClickTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnStep();
	afx_msg void OnIter();
	afx_msg void OnRun();
	afx_msg void OnStop();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRadiodec();
	afx_msg void OnRadiohex();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnChangeEditgoto();
	afx_msg void OnSetfocusEditgoto();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnFindinladder();
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnGotorun();
	afx_msg void OnGotostop();
	afx_msg void OnGotoidle();
	afx_msg void OnLoadsnap();
	afx_msg void OnSavesnap();
	afx_msg void OnLoadback();
	afx_msg void OnSaveback();
	afx_msg void OnChangeMonitor();
	//}}AFX_MSG

  afx_msg BOOL OnGetToolTipNotify( UINT id, NMHDR * pTTTStruct, LRESULT * pResult );
  afx_msg LRESULT OnCOMCmd(WPARAM, LPARAM); 
  afx_msg LRESULT OnInitLoad(WPARAM, LPARAM); 
//  afx_msg LRESULT OnOpcCmd(WPARAM, LPARAM); 

	DECLARE_MESSAGE_MAP()

  int PointInClient(CPoint &point);
  void SetDisplayTable(int iPc, int iTb, int iWd=-1, int iBt=-1);
  void SetDisplayLadder(int iPc, int iLd);
  void SetDisplaySymbol(int iPc, int iSm);

  long NPlcs() { return m_Plc.GetSize(); };

  CToolTipCtrl m_ToolTip;

  };

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLC5DLG_H__4ADAF474_F3F5_49D4_A72F_9AD9F2C95554__INCLUDED_)
