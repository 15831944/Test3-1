//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __TAGVTRND_H
#define __TAGVTRND_H

//-------------------------------------------------------------------------

_FWDDEF(CTagVwTrend)

struct TrendPenRec
  {
  HPEN hPen;
  HPEN hOldPen;
  };

CTagVwTrend_FWDDEF(CTagVwTrend);

//===========================================================================

class CLabelWnd : public CWnd
  {

  DEFINE_MEMSTATS(CLabelWnd)

  public:
	  CLabelWnd(CTagVwTrend * Trnd);
	  //{{AFX_VIRTUAL(CLabelWnd)
	  //}}AFX_VIRTUAL
	  virtual ~CLabelWnd() {};
    void SetText(pchar p1, pchar p2 = "") { Txt1 = p1; Txt2 = p2; };
    void SetTextToFit(pchar p1, pchar p2 = "", byte xpos = 0, byte ypos = 0);
    static CFont*       pFont;
    static int          CharX;
    static int          CharY;
    //static COLORREF     BkCol;
    //static COLORREF     TxCol;
//    void SetCols(COLORREF NewBkCol, COLORREF NewTxtCol) { BkCol = NewBkCol; TxCol = NewTxtCol; };
    void SetFont(CFont* Font) { pFont = Font; };
  protected:
    Strng       Txt1;
    Strng       Txt2;
    byte        bMultiLine:1;

    CTagVwTrend *pTrnd;

	  //{{AFX_MSG(CLabelWnd)
	  afx_msg void OnPaint();
	  //}}AFX_MSG
	  DECLARE_MESSAGE_MAP()
  };

//===========================================================================

class CTagVwTrend : public CView//, public CExecObj
{
friend class CTrendSlot;
friend class CTagVwDoc;
friend class CLabelWnd;

DECLARE_DYNCREATE(CTagVwTrend)
protected:
  CTagVwTrend();      // protected constructor used by dynamic creation

protected://public:
  //CTrendSlot       Trend[MaxTrends];
  long             m_lNChanges;

  CFont            *Font[3];
  CRect            CurrentPosR,
                   HeadR, TrendR, BordR, FootR,
                   DrawR, ReDrawR;

  CLabelWnd        STWnd,ETWnd,DurWnd; //label windows

  CSize            CrSz, CharSize[3];
  
  double           DrwStartTime;
  double           DrwEndTime;
  double           DrwCurrentTime;
  UINT             MouseFlags;

  long             LastTimeCursPos;

  double           TimeOfLastBuild;

  int              LastTrndNo;
  TrackTrend       TrkTrend;

  flag             bCurrentPrinting:1,
                   bPrinting:1,
                   bDispCleared:1,
                   bDoBuild:1,
                   bUseColorPrint:1;

  long             DblClkTm;
  char             DblClks;
  CPoint           DblPts[2];
  CPoint           LastMousePos;

  //07 5538 2533 apt21

protected:
  pCTagVwDoc       Doc() {return (pCTagVwDoc)(GetDocument());};
  void             LockDoc() {((pCTagVwDoc)(GetDocument()))->Lock();};
  void             FreeDoc() {((pCTagVwDoc)(GetDocument()))->Free();};

  void             SetTrendPosition(CDC *pDC, CRect *CRReqd);
  virtual          void OnDraw(CDC* pDC);    // overridden to draw this view
  //virtual          BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
  void             DrawTrend(int iNo, CDC* pDC, flag DoingAll, flag OnlyNew);
  void             DrawGrid(CDC* pDC);
  void             DrawInfo(CDC* pDC);
  void             UpdateTrend(int iTrnd, long Flags);
  void             HiLiteTrend(int iTrnd, flag On);
  //void             BlinkTrend();
  virtual          void OnInitialUpdate();
  void             ClearDisplay();
  virtual          void OnUpdate(CView*pSender, LPARAM lHint, CObject* pHint);

  void             TryScrollWindow();
  void             PointtoLP(POINT &Pt);

  void             ToggleDigCursor(POINT &Pt);
  void             UpdateTimeCurs(double Time, COLORREF BackGndColour, COLORREF TimeColour);
  void             SetRangeValues(flag FixIndicator);
  void             SetDigValues(TrackTrend &WrkTrnd);
  void             ShowLabels();

  int              NoTrends() { return Doc()->NoTrends(); };

protected:
  virtual ~CTagVwTrend();
  virtual BOOL PreCreateWindow(CREATESTRUCT &cs);
  virtual void OnActivateView( BOOL bActivate, CView* pActivateView, CView* pDeactiveView );
  virtual BOOL OnPreparePrinting( CPrintInfo* pInfo );
  virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
  virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
  virtual void OnEndPrintPreview( CDC* pDC, CPrintInfo* pInfo, POINT point, CPreviewView* pView );
  virtual void OnPrint(CDC *pDC, CPrintInfo* pInfo);

public:
  void DoOnFilePrint(BOOL UseColors);
  void DoOnFilePrintPreview();

protected:
  //{{AFX_MSG(CTagVwTrend)
  afx_msg void OnMove(int x, int y);
  afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
  afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg LRESULT OnUpdateByMsg(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnScrollByMsg(WPARAM wParam, LPARAM lParam);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

#endif
