//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992 - 1995

#ifndef  __PGM_D_H
#define  __PGM_D_H

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif
#ifndef __SC_DEFS_H
#include "sc_defs.h"
#endif  
#ifndef __VECTORS_H
#include "vectors.h"
#endif
#include "resource.h"

#ifdef __PGM_D_CPP
  #define DllImportExport DllExport
#elif !defined(SCEXEC)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//===========================================================================
_FWDDEF(GCInsMngr)
_FWDDEF(CPGMDbgMngr)

const int MaxListWndCols = 4;

const long LW_COL1 = 1;
const long LW_COL2 = 2;
const long LW_LINE = 4;
const long LW_SYMB1 = 8;
const long LW_SYMB2 = 16;

_FWDDEF(CListWndHelper)
class DllImportExport CListWndHelper
  {
  public:
    CSVector StrList[MaxListWndCols];
    CLVector FlagsList;
    int ColWidths[MaxListWndCols];
    int iColsUsed;
    int iMaxWidth;
    int iMaxLines;
    int iCurLine;
    int iNextLine;
    int iMargin;
    int iHScrollPos;
    int iVScrollPos;
    COLORREF Color1;
    WORD bHorzLines:1,
         bVertLines:1,
         bForceHScroll:1,
         bForceVScroll:1,
         bWrapArround:1,
         bAllowSelect:1,
         bUseSelectSymb:1;

    CListWndHelper();
    void ToggleFlag(int line, long flag);
    void ClearLine(int line);
    inline void IncLine() { iNextLine++; if (iNextLine>=iMaxLines) iNextLine = 0; };
  };

class CListWnd : public CWnd
  {
  protected:
    static CFont Font;        //font to be used
    static flag bCreatedFont; //has the font been created
	  CScrollBar	VScrollBar;   //vertical scroll bar for scrolling the list
	  CScrollBar	HScrollBar;   //horizontal scroll bar for scrolling the list
  public:
    pCListWndHelper pInfo;  //
    int iStartLine;         //current first line number
    int iEndLine;           //current last line number
    int iHeight;            //number of lines in the window
    int iWidth;             //number of characters that can be shown in the window
    int iMaxVScroll;        //maximum 
    int iMaxHScroll;        //maximum 
  public:
	  CListWnd(int Height, int Width);
	  virtual ~CListWnd();
    void SetCurLine(int CurLine, byte Style = 1, flag Repaint = True); //0=start, 1=mid, 2=end
    void AttachHelper(pCListWndHelper Info);
    void InitScrollBars();
    void UpdateScrollBars();
	  //{{AFX_VIRTUAL(CListWnd)
	  //}}AFX_VIRTUAL
  protected:
	  //{{AFX_MSG(CListWnd)
	  afx_msg void OnPaint();
	  afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	  afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	  afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	  //}}AFX_MSG
	  DECLARE_MESSAGE_MAP()
  };

//===========================================================================

_FWDDEF(CPGMDebugWnd)
class CPGMDebugWnd : public CDialog
  {
  public:
    CListWnd CodeWnd;
    CListWnd VarWnd;
  protected:
    pCPGMDbgMngr pDbgMngr;
    pCPGMDbgMngr pPrevDbgMngr;
    flag bDoBreak:1;
  public:
	  CPGMDebugWnd(CWnd* pParent);
    ~CPGMDebugWnd();

    void Close();
    void Hook(pCPGMDbgMngr DbgMngr);
    void UnHook(pCPGMDbgMngr DbgMngr);

	  //{{AFX_DATA(CPGMDebugWnd)
	  enum { IDD = IDD_PGMDEBUG };
	  CString	m_PgmName;
	  int		m_LineNo;
	  int		m_VarOption;
	  //}}AFX_DATA
	  //{{AFX_VIRTUAL(CPGMDebugWnd)
	  protected:
	  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	  virtual void PostNcDestroy();
	  //}}AFX_VIRTUAL

  protected:
    //{{AFX_MSG(CPGMDebugWnd)
	  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
    afx_msg LRESULT OnUpdateData(WPARAM wParam, LPARAM lParam);
    afx_msg void OnBtnPressed(UINT nID);
    //afx_msg void OnUpdateBtn(CCmdUI* pCmdUi);//, UINT nID);
    afx_msg void OnUpdateStep(CCmdUI* pCmdUi);
    afx_msg void OnUpdateRun(CCmdUI* pCmdUi);
    afx_msg void OnUpdateBreak(CCmdUI* pCmdUi);
    afx_msg void OnUpdateClose(CCmdUI* pCmdUi);
    afx_msg void OnUpdateBreakpt(CCmdUI* pCmdUi);
    afx_msg void OnUpdateWatch(CCmdUI* pCmdUi);
    afx_msg void OnVaroption(UINT nID);
    afx_msg void OnUpdateVaroption(CCmdUI* pCmdUi);
	  DECLARE_MESSAGE_MAP()
  };

//===========================================================================

_FWDDEF(CPGMDbg)
class CPGMDbg
  {
  public:
    CPGMDbg();
    int     iLineNo;    //PGM file line number
    flag    bBreakLine:1,
            bBreakPt:1;
  };

_FWDDEF(GCVar)
class DllImportExport CPGMDbgMngr
  {
  public:
    static pCPGMDebugWnd pWnd;
    static HANDLE        hEvent;
    CListWndHelper       CodeInfo;
    CListWndHelper       VarInfo;
    CListWndHelper       VarChngdInfo;
    CListWndHelper       VarWatchInfo;
    byte                 bRunTillBreak:1,
                         bPaused:1,
                         bAllVars:1;
    int                  iLineNo;
    int                  iVarOption;
    pGCInsMngr           pInsMngr;
    Strng                sPgmName;

    CPGMDbgMngr();
    ~CPGMDbgMngr();
    static void Open();
    static void Close();
    void Reset();
    void ConnectWindow();
    void DisConnectWindow();
    void InitInfo(rGCInsMngr IB, int LastLine);
    void Advance(pCPGMDbg pDbg, pCPGMDbg pNxtDbg, rGCInsMngr IB);
    void AddLine(int LineNo, const char* p);
    void BuildWatchInfo();
  protected:
    void ListVarValues(int& Cnt, pGCVar pVar);
    void ListVarChngdValues();
    void ListVarWatchValues();
    void InitVarList(int& Cnt, pGCVar pVar, pchar pClassName);
  };

//===========================================================================

#undef DllImportExport

#ifndef __PGM_E_H
#include "pgm_e.h"
#endif

#endif
