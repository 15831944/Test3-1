//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#include "stdafx.h"

#define __WIREPANEL_CPP

#include "sc_defs.h"
#include "resource.h"
#include "wirepanel.h"
#include "project.h"
#include "executiv.h"
#include "syscad.h"
#include "accnode.h"
#include "afxcmn.h"
#include "afxwin.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define DOMEMCHECKS 0

#if DOMEMCHECKS
#define MEMCHECKS() {dbgpln("CrtCheck"); if (!_CrtCheckMemory()) DoBreak();};
#else
#define MEMCHECKS()
#endif

//const int ButtonIdsB[] = {IDC_CLOSE};
//const int COrdDlgNButtonsB=sizeof(ButtonIdsB)/sizeof(ButtonIdsB[0]);

#define SetWndDef(h) ::SetWindowLong(h, GWL_STYLE, ::GetWindowLong(h, GWL_STYLE) | BS_DEFPUSHBUTTON);
#define ClrWndDef(h) ::SetWindowLong(h, GWL_STYLE, ::GetWindowLong(h, GWL_STYLE) & ~BS_DEFPUSHBUTTON);

const COLORREF crBlack=RGB(0x00,0x00,0x00);
const COLORREF crGrey=RGB(0x80,0x80,0x80);
const COLORREF crLtGrey=RGB(0xc0,0xc0,0xc0);
const COLORREF crP=RGB(0x00,0x00,0xff);
const COLORREF crC=RGB(0xff,0x00,0x00);
const COLORREF crE=RGB(0xd0,0xd0,0x00);
const COLORREF crA=RGB(0x00,0xff,0xff);
const COLORREF crX=RGB(0xff,0x00,0x00);

const COLORREF crCable=RGB(0x40,0x40,0x40);

const long NWireColours = 7;
const COLORREF crWire[NWireColours]={ RGB(255,  0,  0),   // Red
                                      RGB(255,255,255),   // White
                                      RGB(  0,  0,255),   // Blue
                                      RGB(  0,  0,  0),   // Black
                                      RGB(208,134, 11),   // Brown
                                      RGB(255,165,  0),   // Orange
                                      RGB(128,128,128)};  //Grey

static LPTSTR ValFmt(double V) { return fabs(V)>=1e6 ? "%*.2e " : "%*.2f "; };


//===========================================================================
//
//
//
//===========================================================================

class COrdDigPt
  {
  public:
    //long    m_Xl, m_Yt;
    //long    m_Xr, m_Yb;
    CRect  m_Rect;
    CEvalOrderItem   *m_pItm;
    CEvalOrderIOItem *m_pIO;
    //LPTSTR  m_pTag;
    COrdDigPt()
      {
      m_pItm=NULL;
      m_pIO=NULL;
      };

    LPTSTR Tag()
      {
      if (m_pIO && m_pIO->m_sRmtTag())
        return m_pIO->m_sRmtTag();
      if (m_pItm && m_pItm->m_sTag())
        return m_pItm->m_sTag();
      return NULL;
      }

  };
//===========================================================================
//
//
//
//===========================================================================
class CWPTermStrip;
class CWPGroup;
class CWPWire;
class CWPCable;
class CWirePanel; 

class CWireInfoWnd; //?????????/

//===========================================================================

class CWPTerminal
  {
  public:
    CString             m_sID;
    CString             m_sName;
    long                m_iTerm;
    CWPTermStrip         *m_pTS;

    CWPTerminal(LPCTSTR ID, LPCTSTR Name, long iTerm)
      {
      m_sID       = ID;
      m_sName     = Name;
      m_iTerm     = iTerm;
      m_pTS       = NULL;
      }
  };

//===========================================================================
//
//
//
//===========================================================================

class CWPTermStrip : public CDialog
  {
  DECLARE_DYNAMIC(CWPTermStrip)

  public:
    CWPTermStrip(LPCTSTR ID, LPCTSTR Name, CWPGroup * pGrp= NULL);   // standard constructor
    virtual ~CWPTermStrip();

    // Dialog Data
    enum { IDD = IDD_WIREPANEL_TS };
    CString             m_sID;
    CString             m_sName;
    CPoint              m_Pt;
    CWPGroup             *m_pGrp;
    //CPanelData         *m_pPanel; 
    //CWPTermStrip      *m_pWnd;

    CArray<CWPTerminal*, CWPTerminal*> m_Terminals;

  protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()
  public:
    CListCtrl m_TermList;

    //CWPTermStrip * m_pTS;
    //CWPGroup *m_pGroup;

    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    void DoLayout();
    afx_msg void OnMove(int x, int y);
    afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
    CButton m_ShowInBtn;
    CButton m_ShowOutBtn;
//    afx_msg void OnParentNotify(UINT message, LPARAM lParam);
//    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnBnClickedShowin();
    afx_msg void OnBnClickedShowout();
  };

//===========================================================================
//
//
//
//===========================================================================

class CWPGroup : public CWnd
  {
  DECLARE_DYNAMIC(CWPGroup)

  public:
    CWPGroup(LPCTSTR ID, LPCTSTR Name, CWirePanel * pParent);
    virtual ~CWPGroup();

    CString             m_sID;
    CString             m_sName;
    CPoint              m_Pt;
    CWirePanel         *m_pPanel;
    CArray <CWPTermStrip*, CWPTermStrip*> m_TermStrips;

  protected:
    DECLARE_MESSAGE_MAP()
  public:
//    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnMove(int x, int y);
    afx_msg void OnSize(UINT nType, int cx, int cy);
  };
//===========================================================================
//
//
//
//===========================================================================

class CWPWire
  {
  public:
    CString             m_sID;
    CString             m_sColour;
    CPoint              m_SrcPt, m_DstPt;
    CWPTerminal         *m_pSrc, *m_pDst;
    CWPCable            *m_pCable;

    void                GetSrcWndPt(CPoint &IOPt, CRect &IOSym);
    void                GetDstWndPt(CPoint &IOPt, CRect &IOSym);

    CWPWire(LPCTSTR ID)
      {
      m_sID     = ID;
      m_sColour = "";
      m_pSrc    = NULL;
      m_pDst    = NULL;
      m_pCable  = NULL;
      }
  };

//===========================================================================
//
//
//
//===========================================================================

class CWPCable
  {
  public:
    CString             m_sID;

    CArray<CWPWire*, CWPWire*> m_Wires;

    CWPCable(LPCTSTR ID)
      {
      m_sID=ID;
      }

  };

//===========================================================================
//
//
//
//===========================================================================

class CWPScrollBar : public CScrollBar
{
	DECLARE_DYNAMIC(CWPScrollBar)

public:
	CWPScrollBar();
	virtual ~CWPScrollBar();

protected:
	DECLARE_MESSAGE_MAP()
public:
//  afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
//  afx_msg void OnParentNotify(UINT message, LPARAM lParam);
};


IMPLEMENT_DYNAMIC(CWPScrollBar, CScrollBar)
CWPScrollBar::CWPScrollBar()
{
}

CWPScrollBar::~CWPScrollBar()
{
}


BEGIN_MESSAGE_MAP(CWPScrollBar, CScrollBar)
//  ON_WM_VSCROLL()
//  ON_WM_PARENTNOTIFY()
END_MESSAGE_MAP()


//===========================================================================
//
//
//
//===========================================================================

class CWirePanel : public CFormView
  {
  friend class CWirePanelFrm;
  public:

  protected:
    CWirePanel();           // protected constructor used by dynamic creation
    DECLARE_DYNCREATE(CWirePanel)

    CString         m_sRootTag;
    CArray <CWPTerminal*, CWPTerminal*>     m_Terminals;
    CArray <CWPTermStrip*, CWPTermStrip*>   m_TermStrips;
    CArray <CWPGroup*, CWPGroup*>           m_Groups;
    CArray <CWPWire*, CWPWire*>             m_Wires;
    CArray <CWPCable*, CWPCable*>           m_Cables;

    class CWGData
      {
      public:
        CWPWire  *m_pWire;
        CPoint m_SrcPt, m_DstPt;
        CRect  m_SrcSym, m_DstSym;
      };

    CArray<CWGData, CWGData&> m_WGData;

    static GDIBlkCfg GDICfg;
    GDIBlk           ScrGB;

    CPoint        m_Size;
    CPoint        m_Pos;

    bool          m_bFirstDraw;
    bool          m_InitValuesSet;

    CRect         m_InitClntRect;

    SCROLLINFO    m_ScInfoX;
    SCROLLINFO    m_ScInfoY;

    flag          m_bInited;

    CArray <COrdDigPt, COrdDigPt&> m_DigPts;
    long m_nDigPts;

    CRgn          m_TitleClipRgn, m_ViewClipRgn, m_ClipRgn;

    CWireInfoWnd *m_pInfoWnd;
    COrdDigPt    *m_pLastDig;

    CPen         m_pnCable, m_pnWire[NWireColours];
    CBrush       m_brCable, m_brWire[NWireColours];

    public:
    //{{AFX_DATA(CWirePanel)
	  enum { IDD = IDD_WIREPANEL_WND };
    CStatic	m_ViewArea;
    CWPScrollBar	m_ScrollV;
    CWPScrollBar	m_ScrollH;
  	//}}AFX_DATA

  public:
   void SetRootTag(LPCTSTR pRootTag);

    CWPTerminal *   FindTerminal(LPCTSTR ID);
    CWPTerminal *   AddTerminal(LPCTSTR ID, LPCTSTR Name, CWPTermStrip*pTS);
    CWPTermStrip *  FindTermStrip(LPCTSTR ID);
    CWPTermStrip *  AddTermStrip(LPCTSTR ID, LPCTSTR Name, CWPGroup *pGrp);
    CWPGroup *      FindGroup(LPCTSTR ID);
    CWPGroup *      AddGroup(LPCTSTR ID, LPCTSTR Name);
    CWPWire *       FindWire(LPCTSTR ID);
    CWPWire *       AddWire(LPCTSTR ID, CWPCable* pCable);
    CWPCable *      FindCable(LPCTSTR ID);
    CWPCable *      AddCable(LPCTSTR ID);
    void          AddCables(CWPTermStrip * pTS);

    void AttachTermStrip(CWPGroup * pGrp, CWPTermStrip *pTS);
    void AttachTerminal(CWPTermStrip *pTS, CWPTerminal *pTrm);

    void ClearConnectInfo();
    void BuildFrom(CLinkWiring & LW);
    void GetConnectInfo();

    void SetScrollBars();
    void UpdateWindowText();
    void RedrawGraph();
    int  DoRefresh() { OnRefresh(); return 0; };

    //{{AFX_VIRTUAL(CWirePanel)
	public:
    virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	  virtual void OnInitialUpdate();
	protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
    virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
    virtual void OnDraw(CDC* pDC);
	//}}AFX_VIRTUAL
  protected:
    void          DoLayout();
    virtual       ~CWirePanel();

    COrdDigPt *   FindDigPt(UINT nFlags, CPoint point);
    void          ShowInfo(CPoint point, COrdDigPt *pPt);
    void          CloseInfo();

    void OnActivateFrame( UINT nState, CFrameWnd* pFrameWnd )
      {
      CFormView::OnActivateFrame( nState, pFrameWnd );
      switch (nState)
        {
        case WA_ACTIVE:
        case WA_CLICKACTIVE:
          OnRefresh();
          break;
        case WA_INACTIVE:;
        }
      };


    //{{AFX_MSG(CWirePanel)
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnClose();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnRefresh();
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg LRESULT OnCloseOrdInfo(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSelchangeShowwhat();
	//}}AFX_MSG
    //afx_msg LRESULT OnLogMsg(WPARAM wParam, LPARAM lParam);
    afx_msg void OnUpdateBtn(CCmdUI* pCmdUi);
    //afx_msg void OnMsgMenu(UINT id);
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================
//
//
//
//===========================================================================

class CIDStr 
  {
  public:
    CString S;
    CIDStr(CWPGroup * pGrp, LPCTSTR TSNm=NULL, LPCTSTR TNm=NULL)
      {
      S=pGrp->m_sID;
      if (TSNm)
        {
        S+=":";
        S+=TSNm;
        if (TNm)
          {
          S+=":";
          S+=TNm;
          }
        }
      };
    CIDStr(CWPCable * pCable, long i)
      {
      S.Format("%s:%i", pCable->m_sID, i);
      };
    operator LPCTSTR() { return S; };
  };

//===========================================================================
//
//
//
//===========================================================================

class CWireInfoWnd : public CDialog
{
// Construction
public:
	CWireInfoWnd(eTraceValue TV, long X, long Y, COrdDigPt *pDigPt, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWireInfoWnd)
	enum { IDD = IDD_WIREPANEL_INFO };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

  void SetInfo(eTraceValue TV, long X, long Y, COrdDigPt *pDigPt);

  // Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWireInfoWnd)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL


// Implementation

  protected:

  CPoint            m_Off;
  COrdDigPt        *m_pDigPt;
  CEvalOrderItem   *m_pItm;
  CEvalOrderIOItem *m_pIO;
  eTraceValue       m_TV;
  bool              m_bAllowMove;

  double            m_TotWide, m_TotDeep;

  CPen              m_pnLtGrey;

  void DrawLine(CDC &DC, CPen &pnG, double X1, double Y1, double X2, double Y2);

	// Generated message map functions
	//{{AFX_MSG(CWireInfoWnd)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//===========================================================================
// CWPTermStrip dialog

IMPLEMENT_DYNAMIC(CWPTermStrip, CDialog)
CWPTermStrip::CWPTermStrip(LPCTSTR ID, LPCTSTR Name, CWPGroup* pGrp /*=NULL*/)
: CDialog(CWPTermStrip::IDD, (CWnd*)pGrp),
  m_pGrp(pGrp)
  {
  m_sID     = ID;
  m_sName   = Name;
  m_pGrp    = pGrp;
  //m_pPanel  = NULL; 

  //m_ShowInBtn.LoadBitmaps(IDB_LEFTLEFT, IDB_RIGHTRIGHT);
  //m_ShowOutBtn.LoadBitmaps(MAKEINTRESOURCE(IDB_RIGHTRIGHT), MAKEINTRESOURCE(IDB_LEFTLEFT));

  //m_ShowInBtn.SizeToContent();

  Create(CWPTermStrip::IDD, (CWnd*)pGrp);
  }

//---------------------------------------------------------------------------

CWPTermStrip::~CWPTermStrip()
  {
  }

//---------------------------------------------------------------------------

void CWPTermStrip::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_TERMLIST, m_TermList);
  DDX_Control(pDX, IDC_SHOWIN, m_ShowInBtn);
  DDX_Control(pDX, IDC_SHOWOUT, m_ShowOutBtn);
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CWPTermStrip, CDialog)
  ON_WM_SIZE()
  ON_WM_MOVE()
  ON_WM_MOVING()
//  ON_WM_PARENTNOTIFY()
//  ON_WM_VSCROLL()
  ON_BN_CLICKED(IDC_SHOWIN, OnBnClickedShowin)
  ON_BN_CLICKED(IDC_SHOWOUT, OnBnClickedShowout)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CWPTermStrip::OnInitDialog()
  {
  CDialog::OnInitDialog();

  //m_ShowInBtn.SubclassDlgItem(IDC_SHOWIN, this);
  //m_ShowInBtn.SizeToContent();

  m_TermList.InsertColumn(0, "Terminal");

  DoLayout();
  return TRUE;  
  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
  }

//---------------------------------------------------------------------------
// CWPTermStrip message handlers

void CWPTermStrip::OnSize(UINT nType, int cx, int cy)
  {
  CDialog::OnSize(nType, cx, cy);

  DoLayout();
  m_pGrp->Invalidate();
  m_pGrp->m_pPanel->Invalidate();
  }

//---------------------------------------------------------------------------

void CWPTermStrip::OnMove(int x, int y)
  {
  CDialog::OnMove(x, y);

  m_pGrp->Invalidate();
  m_pGrp->m_pPanel->Invalidate();
  }

//---------------------------------------------------------------------------

void CWPTermStrip::OnMoving(UINT fwSide, LPRECT pRect)
  {
  CDialog::OnMoving(fwSide, pRect);

  CRect rctMe(pRect);
  CRect rctPanel;
  m_pGrp->GetClientRect(&rctPanel);
  m_pGrp->ClientToScreen(&rctPanel);

  if (rctMe.left<rctPanel.left)
    rctMe.OffsetRect(-(rctMe.left-rctPanel.left),0);
  
  if (rctMe.top<rctPanel.top)
    rctMe.OffsetRect(0, -(rctMe.top-rctPanel.top));
  
  if (rctMe.right>rctPanel.right)
    rctMe.OffsetRect(-(rctMe.right-rctPanel.right),0);
  
  if (rctMe.bottom>rctPanel.bottom)
    rctMe.OffsetRect(0, -(rctMe.bottom-rctPanel.bottom));

  *pRect=rctMe;
  }

//---------------------------------------------------------------------------

void CWPTermStrip::DoLayout()
  {
  CRect rctClnt;
  GetClientRect(&rctClnt);

  CRect rctShowOut, rctList;
  HWND hShowOut, hList;

  //...
  hShowOut= ::GetDlgItem(GetSafeHwnd(), IDC_SHOWOUT);
  ::GetWindowRect(hShowOut, &rctShowOut);
  ScreenToClient(&rctShowOut);

  long w=rctShowOut.Width();
  rctShowOut.right=rctClnt.right;
  rctShowOut.left=rctClnt.right-w;

  ::SetWindowPos(hShowOut, m_hWnd, rctShowOut.left, rctShowOut.top, rctShowOut.Width(), rctShowOut.Height(), SWP_NOACTIVATE|SWP_NOZORDER);
  ::InvalidateRect(hShowOut, NULL, true);

  //...
  hList= ::GetDlgItem(GetSafeHwnd(), IDC_TERMLIST);
  ::GetWindowRect(hList, &rctList);
  ScreenToClient(&rctList);

  rctList.right=rctClnt.right;
  rctList.left=rctClnt.left;
  rctList.bottom=rctClnt.bottom;

  ::SetWindowPos(hList, m_hWnd, rctList.left, rctList.top, rctList.Width(), rctList.Height(), SWP_NOACTIVATE|SWP_NOZORDER);
  ::InvalidateRect(hList, NULL, true);

  long EW=::GetSystemMetrics(SM_CXEDGE);
  if (m_TermList.m_hWnd != NULL)
    m_TermList.SetColumnWidth(0, rctList.Width()-2*EW); 
  }

void CWPTermStrip::OnBnClickedShowin()
  {
  m_pGrp->m_pPanel->AddCables(this);
  }

void CWPTermStrip::OnBnClickedShowout()
  {
  // TODO: Add your control notification handler code here
  }

//===========================================================================
//
//
//
//===========================================================================
// CWPGroup
// CWPGroup

IMPLEMENT_DYNAMIC(CWPGroup, CWnd)

CWPGroup::CWPGroup(LPCTSTR ID, LPCTSTR Name, CWirePanel * pParent)
  {
  m_sID     = ID;
  m_sName   = Name;
  m_pPanel  = pParent; 

  HICON hIcon = ScdApp()->LoadIcon(IDI_WIREWND);
  CString ClassName = AfxRegisterWndClass(CS_DBLCLKS|CS_NOCLOSE, 0, 0, hIcon);

  CRect rct(0,0, 10,10);
  CreateEx(WS_EX_TRANSPARENT|WS_EX_TOOLWINDOW , ClassName, "A TS Group", 
            WS_CHILD | WS_OVERLAPPED | WS_CAPTION | /*WS_SYSMENU | WS_BORDER*/ WS_THICKFRAME , 
            rct, (CWnd*)&pParent->m_ViewArea, 0);
  }

//---------------------------------------------------------------------------

CWPGroup::~CWPGroup()
  {
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CWPGroup, CWnd)
//  ON_WM_MOUSEMOVE()
  ON_WM_MOVE()
  ON_WM_SIZE()
END_MESSAGE_MAP()


void CWPGroup::OnMove(int x, int y)
  {
  CWnd::OnMove(x, y);

  m_pPanel->Invalidate();
  }

//---------------------------------------------------------------------------

void CWPGroup::OnSize(UINT nType, int cx, int cy)
  {
  CWnd::OnSize(nType, cx, cy);

  m_pPanel->Invalidate();
  }


//===========================================================================
//
//
//
//===========================================================================

class CWirePanel;
class CWirePanelFrm;

static CWirePanel *s_pWirePanelWnd=NULL;
static CWirePanelFrm *s_pWirePanelFrm=NULL;

GDIBlkCfg  CWirePanel::GDICfg("WiringPanel");

//===========================================================================
//
//
//
//===========================================================================

IMPLEMENT_DYNCREATE(CWirePanel, CFormView)

CWirePanel::CWirePanel()
	: CFormView(CWirePanel::IDD)
  {

  //{{AFX_DATA_INIT(CWirePanel)
	//}}AFX_DATA_INIT

  for (int i=0; i<NWireColours; i++)
    {
    m_pnWire[i].CreatePen(PS_SOLID, 0, crWire[i]);
    m_brWire[i].CreateSolidBrush(crWire[i]);
    }

  m_pnCable.CreatePen(PS_SOLID, 0, crBlack);
  m_brCable.CreateSolidBrush(crLtGrey);
    
  m_bInited=false;

  m_Size=CPoint(1,1);
  m_Pos=CPoint(0,0);
  
  m_WGData.SetSize(0, 64);

  m_InitValuesSet=false;
  m_bFirstDraw=true;

  CDC DC;
  DC.CreateCompatibleDC(NULL);
  CDCResChk ResChk(DC);
  ScrGB.Create(&GDICfg, this, &DC, false);
  ScrGB.Attach(&DC, NULL); //  this attach detach will set up internal variables
  ScrGB.Detach();          //

  m_pInfoWnd=NULL;
  m_pLastDig=NULL;
  m_nDigPts=0;
  }

//---------------------------------------------------------------------------

CWirePanel::~CWirePanel()
  {
  }

//---------------------------------------------------------------------------

void CWirePanel::DoDataExchange(CDataExchange* pDX)
  {
	CFormView::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CWirePanel)
	DDX_Control(pDX, IDC_VIEWAREA,   m_ViewArea);
	DDX_Control(pDX, IDC_SCROLLBARV, m_ScrollV);
	DDX_Control(pDX, IDC_SCROLLBARH, m_ScrollH);
	//}}AFX_DATA_MAP
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CWirePanel, CFormView)
  //{{AFX_MSG_MAP(CWirePanel)
  ON_WM_SIZE()
  ON_BN_CLICKED(IDC_CLOSE, OnClose)
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_LBUTTONDBLCLK()
  ON_WM_RBUTTONDOWN()
  ON_WM_HSCROLL()
  ON_WM_VSCROLL()
  ON_BN_CLICKED(IDC_REFRESH, OnRefresh)
  ON_WM_MOUSEMOVE()
  ON_WM_KEYDOWN()
  ON_MESSAGE(WMU_CLOSEORDINFO, OnCloseOrdInfo)
  ON_WM_MOVE()
  ON_WM_WINDOWPOSCHANGING()
	//ON_BN_CLICKED(IDC_FULLDESCI, OnRefresh)
	//ON_BN_CLICKED(IDC_FULLDESCO, OnRefresh)
	//ON_BN_CLICKED(IDC_SHOWIOINX, OnRefresh)
	//ON_BN_CLICKED(IDC_SHOWIOABS, OnRefresh)
	//ON_BN_CLICKED(IDC_SHOWIOREL, OnRefresh)
	//ON_BN_CLICKED(IDC_SHOWIOVAL, OnRefresh)
	//ON_BN_CLICKED(IDC_SHOWIONONE, OnRefresh)
	//ON_BN_CLICKED(IDC_SHOWTEARSONLY, OnRefresh)
	//ON_BN_CLICKED(IDC_FILTERNC, OnRefresh)
	//ON_CBN_SELCHANGE(IDC_SHOWWHAT, OnSelchangeShowwhat)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CWirePanel::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
  {
  if (CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext))
    {
    m_bInited=true;
    //CBitmap BM;
    //BM.LoadBitmap(IDB_ERRORIMGS2);
    //BOOL b=m_ImgList.Create(16, 12, false, 0, 10);
    //m_ImgList.Add(&BM, (CBitmap*)NULL);

    GetWindowRect(&m_InitClntRect);
    
    return true;
    }
  return false;
  }

//---------------------------------------------------------------------------

void CWirePanel::DoLayout()
  {
  if (!m_bInited)
    return;

  if (!::IsWindow(m_ViewArea.m_hWnd))
    return;

  CRect ClntRect, ViewRect, ScrlHRect, ScrlVRect;

  GetClientRect(&ClntRect);
  //m_TitleArea.GetWindowRect(&TitleRect);
  m_ViewArea.GetWindowRect(&ViewRect);
  m_ScrollH.GetWindowRect(&ScrlHRect);
  m_ScrollV.GetWindowRect(&ScrlVRect);

  //ScreenToClient(&TitleRect);
  ScreenToClient(&ViewRect);
  ScreenToClient(&ScrlHRect);
  ScreenToClient(&ScrlVRect);

  int ButtonIdsB[]= { IDC_CLOSE };

  const int NButtons=sizeof(ButtonIdsB)/sizeof(ButtonIdsB[0]);

  HWND       hBB[NButtons];
  CRect      BRect[NButtons];
  CRect      &ClsRect=BRect[0];

  for (int i=0; i<NButtons; i++)
    {
    hBB[i] = ::GetDlgItem(GetSafeHwnd(), ButtonIdsB[i]);
    ::GetWindowRect(hBB[i], &BRect[i]);
    ScreenToClient(&BRect[i]);
    }

   int LGap=ViewRect.left;
  int TGap=ViewRect.top;
  int Cx=ClntRect.Width();
  int Cy=ClntRect.Height();
  int ViewW=Cx-LGap-ScrlVRect.Width()-ClsRect.Width()-LGap-LGap;
  int ViewH=Cy-TGap-ScrlHRect.Height()-TGap;

  m_ViewArea.SetWindowPos(this, LGap, TGap, ViewW, ViewH, SWP_NOACTIVATE|SWP_NOZORDER);

  m_ScrollV.SetWindowPos(this, LGap+ViewW, TGap, ScrlVRect.Width(), ViewH, SWP_NOACTIVATE|SWP_NOZORDER);
  m_ScrollH.SetWindowPos(this, LGap, TGap+ViewH, ViewW, ScrlHRect.Height(), SWP_NOACTIVATE|SWP_NOZORDER);

  CPoint ClsPos(Cx-LGap-ClsRect.Width()-LGap, Cy-TGap-ClsRect.Height()/*-TGap*/);
  CPoint WndMove(ClsPos.x-ClsRect.left, ClsPos.y-ClsRect.top);

  for (i=0; i<NButtons; i++)
    ::SetWindowPos(hBB[i], m_hWnd, WndMove.x+BRect[i].left, WndMove.y+BRect[i].top,
                   BRect[i].Width(), BRect[i].Height(), SWP_NOACTIVATE|SWP_NOZORDER);
  for (i=0; i<NButtons; i++)
    ::InvalidateRect(hBB[i], NULL, true);

  //m_IOToleranceSlide.SetPageSize(1);
  //m_IOToleranceSlide.SetLineSize(1);
  //m_IOToleranceSlide.SetRange(0,100);
  //m_IOToleranceSlide.SetTicFreq(10);
  //m_IOToleranceSlide.SetPos(m_iIODispTol);
  //m_dIODispTol=0;
  }

//---------------------------------------------------------------------------

void CWirePanel::OnSize(UINT nType, int cx, int cy)
  {
//
  CFormView::OnSize(nType, cx, cy);

  if (nType==SIZE_RESTORED && cx>0 && cx<=32000 && cy<=32000)
    {
    DoLayout();
    SetScrollBars();
    }
  }

//---------------------------------------------------------------------------

void CWirePanel::SetRootTag(LPCTSTR pRootTag)
  {
  m_sRootTag=pRootTag; 
  // etc
  };

//---------------------------------------------------------------------------

void CWirePanel::ClearConnectInfo()
  {
  // etc
  };

//---------------------------------------------------------------------------

CWPTerminal * CWirePanel::FindTerminal(LPCTSTR ID)
  {
  for (int iC=0; iC<m_Terminals.GetSize(); iC++)
    if (m_Terminals[iC]->m_sID.CompareNoCase(ID)==0)
      return m_Terminals[iC];
  return NULL;
  }
CWPTerminal * CWirePanel::AddTerminal(LPCTSTR ID, LPCTSTR Name, CWPTermStrip*pTS)
  {
  dbgpln("AddTerminal  :%s", ID);
  m_Terminals.Add(new CWPTerminal(ID, Name, -1));
  CWPTerminal *pTrm=m_Terminals[m_Terminals.GetUpperBound()];
  
  pTrm->m_pTS=pTS;

  CListCtrl &L=pTS->m_TermList;
  L.InsertItem(L.GetItemCount(), Name);
  return pTrm;
  }

//---------------------------------------------------------------------------

CWPTermStrip * CWirePanel::FindTermStrip(LPCTSTR ID)
  {
  for (int iC=0; iC<m_TermStrips.GetSize(); iC++)
    if (m_TermStrips[iC]->m_sID.CompareNoCase(ID)==0)
      return m_TermStrips[iC];
  return NULL;
  }
CWPTermStrip * CWirePanel::AddTermStrip(LPCTSTR ID, LPCTSTR Name, CWPGroup *pGrp)
  {
  dbgpln("AddTermStrip :%s", ID);
  m_TermStrips.Add(new CWPTermStrip(ID, Name, pGrp));
  CWPTermStrip * pTS=m_TermStrips[m_TermStrips.GetUpperBound()];

  pGrp->m_TermStrips.Add(pTS);
  pTS->SetWindowText(Name);
  pTS->SetWindowPos(&CWnd::wndTop, 10+65*(pGrp->m_TermStrips.GetSize()-1),10,60,150, SWP_SHOWWINDOW);
  return pTS;
  }

//---------------------------------------------------------------------------

CWPGroup * CWirePanel::FindGroup(LPCTSTR ID)
  {
  for (int iC=0; iC<m_Groups.GetSize(); iC++)
    if (m_Groups[iC]->m_sID.CompareNoCase(ID)==0)
      return m_Groups[iC];
  return NULL;
  }
CWPGroup * CWirePanel::AddGroup(LPCTSTR ID, LPCTSTR Name)
  {
  dbgpln("AddGroup     :%s", ID);
  m_Groups.Add(new CWPGroup(ID, Name, this));
  CWPGroup * pGrp=m_Groups[m_Groups.GetUpperBound()];

  pGrp->SetWindowText(Name);
  pGrp->SetWindowPos(&CWnd::wndTop, 10+250*(m_Groups.GetSize()-1),10,200,200, SWP_SHOWWINDOW);

  return pGrp;
  }

//---------------------------------------------------------------------------

CWPWire * CWirePanel::FindWire(LPCTSTR ID)
  {
  for (int iC=0; iC<m_Wires.GetSize(); iC++)
    if (m_Wires[iC]->m_sID.CompareNoCase(ID)==0)
      return m_Wires[iC];
  return NULL;
  }
CWPWire * CWirePanel::AddWire(LPCTSTR ID, CWPCable* pCable)
  {
  dbgpln("AddWire      :%s", ID);
  m_Wires.Add(new CWPWire(ID));
  CWPWire *pW=m_Wires[m_Wires.GetUpperBound()];
  pW->m_pCable=pCable;
  return pW;
  }

//---------------------------------------------------------------------------

CWPCable * CWirePanel::FindCable(LPCTSTR ID)
  {
  for (int iC=0; iC<m_Cables.GetSize(); iC++)
    if (m_Cables[iC]->m_sID.CompareNoCase(ID)==0)
      return m_Cables[iC];
  return NULL;
  }
CWPCable * CWirePanel::AddCable(LPCTSTR ID)
  {
  dbgpln("AddCable     :%s", ID);
  m_Cables.Add(new CWPCable(ID));
  return m_Cables[m_Cables.GetUpperBound()];
  }

//---------------------------------------------------------------------------

void CWirePanel::AttachTerminal(CWPTermStrip *pTS, CWPTerminal *pTrm)
  {
  for (int i=0; i<pTS->m_Terminals.GetSize(); i++)
    if (pTS->m_Terminals[i]==pTrm)
      goto GotIt;
  pTrm->m_iTerm=pTS->m_Terminals.GetSize();
  pTS->m_Terminals.Add(pTrm);

GotIt:;
 
  };

void CWirePanel::AttachTermStrip(CWPGroup * pGrp, CWPTermStrip *pTS)
  {
  for (int i=0; i<m_TermStrips.GetSize(); i++)
    if (m_TermStrips[i]==pTS)
      goto GotIt;
  m_TermStrips.Add(pTS);

GotIt:;

  };

//---------------------------------------------------------------------------

void CWirePanel::AddCables(CWPTermStrip * pTS)
  {
  /**
    CLinkWiring  LW(m_sRootTag);
    gs_pPrj->GetLinkWiring(LW);
    BuildFrom(LW);
**/
  };

//---------------------------------------------------------------------------

void CWirePanel::BuildFrom(CLinkWiring & LW)
  {
  CWPCable *pCable=FindCable(LW.m_LinkTag);
  if (!pCable)
    pCable=AddCable(LW.m_LinkTag);
  
  CWPGroup *pSrcGrp=FindGroup(LW.m_SrcNdTag);
  if (!pSrcGrp)
    pSrcGrp=AddGroup(LW.m_SrcNdTag, LW.m_SrcNdTag);

  CWPGroup *pDstGrp=FindGroup(LW.m_DstNdTag);
  if (!pDstGrp)
    pDstGrp=AddGroup(LW.m_DstNdTag, LW.m_DstNdTag);

  CString ID;
  for (int i=0; i<LW.m_SrcTerminals.GetSize(); i++)
    {
    CLinkWiring::CTerm &I=LW.m_SrcTerminals[i];
    
    CIDStr TSID(pSrcGrp, I.m_sTermStripName);
    CIDStr TID(pSrcGrp, I.m_sTermStripName, I.m_sTerminalName);

    ID.Format("%s:%s:%s",pSrcGrp->m_sID, I.m_sTermStripName, I.m_sTerminalName);
    CWPTermStrip *pSrcTS=FindTermStrip(TSID);
    CWPTerminal  *pSrcTrm=FindTerminal(TID);

    if (!pSrcTS)
      pSrcTS=AddTermStrip(TSID, I.m_sTermStripName, pSrcGrp);
    if (!pSrcTrm)
      pSrcTrm=AddTerminal(TID, I.m_sTerminalName, pSrcTS);
    AttachTermStrip(pSrcGrp, pSrcTS);
    AttachTerminal(pSrcTS, pSrcTrm);
    }

  for (i=0; i<LW.m_DstTerminals.GetSize(); i++)
    {
    CLinkWiring::CTerm &I=LW.m_DstTerminals[i];

    CIDStr TSID(pDstGrp, I.m_sTermStripName);
    CIDStr TID(pDstGrp, I.m_sTermStripName, I.m_sTerminalName);
    
    CWPTermStrip *pDstTS=FindTermStrip(TSID);
    CWPTerminal  *pDstTrm=FindTerminal(TID);

    if (!pDstTS)
      pDstTS=AddTermStrip(TSID, I.m_sTermStripName, pDstGrp);
    if (!pDstTrm)
      pDstTrm=AddTerminal(TID, I.m_sTerminalName, pDstTS);
    AttachTermStrip(pDstGrp, pDstTS);
    AttachTerminal(pDstTS, pDstTrm);
    }

  for (int w=0; ; w++)
    {
    for (int iSrc=LW.m_SrcTerminals.GetSize()-1; iSrc>=0; iSrc--)
      if (LW.m_SrcTerminals[iSrc].m_iWire==w)
        break;
    for (int iDst=LW.m_DstTerminals.GetSize()-1; iDst>=0; iDst--)
      if (LW.m_DstTerminals[iDst].m_iWire==w)
        break;
    if (iSrc>=0 && iDst>=0)
      {
      CIDStr SrcID(pSrcGrp, LW.m_SrcTerminals[iSrc].m_sTermStripName, LW.m_SrcTerminals[iSrc].m_sTerminalName);
      CIDStr DstID(pDstGrp, LW.m_DstTerminals[iDst].m_sTermStripName, LW.m_DstTerminals[iDst].m_sTerminalName);
      CIDStr WireID(pCable, w);
      
      CWPTerminal * pSrc=FindTerminal(SrcID);
      CWPTerminal * pDst=FindTerminal(DstID);

      CWPWire *pWire=FindWire(WireID);
      if (!pWire)
        pWire=AddWire(WireID, pCable);

      pWire->m_pSrc=pSrc;
      pWire->m_pDst=pDst;

      //long n=m_Wiring.GetItemCount();
      //CString No;
      //No.Format("%i", n+1);

      //CWPWire *pWire=FindWire
      //m_Wiring.InsertItem(n,No);
      //m_Wiring.SetItemText(n,1,Src);
      //m_Wiring.SetItemText(n,2,Dst);
      //m_Wiring.SetItemData(n,(iDst<<16)|iSrc);
      }
    else if (iSrc>=0 || iDst>=0)
      {
      LogError("Wiring", 0, "Missing wire");
      }
    else
      break;
    }

  if (1)
    {
    //CButton *pB= new CButton();
    //CRect rct(20,20, 600,600);
    //pB->Create("XXXXXXX", WS_CHILD|WS_VISIBLE|BS_GROUPBOX, rct, this, 0);
    
    }
  }

//---------------------------------------------------------------------------

void CWirePanel::GetConnectInfo()
  {
  if (gs_pPrj==NULL)
    return;
  if (gs_pPrj->m_pFlwLib==NULL)
    return;

  ClearConnectInfo();
  if (m_sRootTag.GetLength()>0)
    {
    CLinkWiring  LW(m_sRootTag);
    if (gs_pPrj->GetLinkWiring(LW)>=0)
      {
      
      CNodeWiring  SNW(LW.m_SrcNdTag);
      gs_pPrj->GetNodeWiring(SNW);

      CNodeWiring  DNW(LW.m_DstNdTag);
      gs_pPrj->GetNodeWiring(DNW);

      BuildFrom(LW);

      CProfINIFile PF(PrjIniFile());
      for (int iCable=0;  ; iCable++)
        {
        CString T;
        T.Format("Cable:%s:%i", m_sRootTag, iCable);
        CString S=PF.RdStr("Wiring", T, "");
        if (S.GetLength()==0)
          break;

        CLinkWiring  LW(S);
        gs_pPrj->GetLinkWiring(LW);
        BuildFrom(LW);
        }
      }

  //if (m_pTermStrips.GetSize()<6)
  //  {
  //  static long xy1[]={120,80,  420,180,   720,80};
  //  static long xy2[]={0,20, 80,20,  20,20, 100,20,  0,20, 80,20};
  //  for (int i=0; i<2; i++)
  //    {

  //    m_pTSGroups.SetAtGrow(i, new CWPGroup(this));

  //    //m_pTSGroups[i]->ShowWindow(SW_NORMAL);
  //    m_pTSGroups[i]->SetWindowPos(&CWnd::wndTop, xy1[i*2]-40,xy1[i*2+1]-40,0,0, SWP_NOSIZE|SWP_SHOWWINDOW);

  //    for (int j=0; j<2; j++)
  //      {

  //      long t=i*2+j;
  //      m_pTermStrips.SetAtGrow(t, new CWPTermStrip(m_pTSGroups[i]));

  //      m_pTermStrips[t]->m_TermList.InsertItem(0, "Red");
  //      m_pTermStrips[t]->m_TermList.InsertItem(1, "White");
  //      m_pTermStrips[t]->m_TermList.InsertItem(2, "Blue");

  //    // p->BringWindowToTop();
  //      //m_pTermStrips[t]->ShowWindow(SW_NORMAL);
  //      m_pTermStrips[t]->SetWindowPos(&CWnd::wndTop, xy2[t*2],xy2[t*2+1],0,0, SWP_NOSIZE|SWP_SHOWWINDOW);
  //      }
  //    }


  //  }



  //CloseInfo();

  //CWaitCursor Wait;

  //SetFlagsEtc();
  ////bool DoProc=m_ShowProc.GetCheck()!=0;
  ////bool ShowTrsOnly=m_ShowTearsOnly.GetCheck()==BST_CHECKED;
  ////bool ShowIOInx=m_ShowIOInx.GetCheck()==BST_CHECKED;
  //////bool ShowIOAbs=m_ShowWhat.GetCurSel()==CS_Abs;
  //////bool ShowIORel=m_ShowWhat.GetCurSel()==CS_Rel;
  //////bool ShowIOVal=m_ShowWhat.GetCurSel()==CS_Val;
  //////long NValueIds=(ShowIOAbs || ShowIORel || ShowIOVal) ? (DoProc ? 4 : 1):0;
  ////
  //m_InxWd=5;
  //m_ChgWd=11;
  ////
  ////eTraceValue TV=TV_None;
  ////long NValueIds=0;
  ////long CurSel=m_ShowWhat.GetCurSel();
  ////switch (CurSel)
  ////  {
  ////  case CS_Abs:    TV=TV_AbsChg;  NValueIds=(DoProc ? 4 : 1);  break;
  ////  case CS_Rel:    TV=TV_RelChg;  NValueIds=(DoProc ? 4 : 1);  break;
  ////  case CS_Val:    TV=TV_Value;   NValueIds=(DoProc ? 4 : 1);  break;
  ////  case CS_MaxRel: TV=TV_RelChg;  NValueIds=(DoProc ? 1 : 1);  break;
  ////  case CS_MaxAbs: TV=TV_AbsChg;  NValueIds=(DoProc ? 1 : 1);  break;
  ////  }

  //long iRet=gs_pPrj->pFlwLib->FE_GetEvalOrder(m_ShowProc.GetCheck()==0,
  //                                            m_FullDescI.GetCheck()!=0,
  //                                            m_FullDescO.GetCheck()!=0,
  //                                            m_TV, m_Info);

  //m_lTgWide=10;
  //m_lInWide=10;
  //m_lOutWide=10;
  //m_nValuesShown=0;

  //for (int ic=0; ic<MaxValCols; ic++)
  //  {
  //  m_dMax[ic]=0;
  //  m_dScale[ic]=1;
  //  m_iPower[ic]=0;
  //  }

  //for (int t=0; t<m_Info.GetSize(); t++)
  //  {
  //  CEvalOrderItem &I=m_Info[t];
  //  I.m_dMax=0;
  //  m_lTgWide=Max(m_lTgWide, (long)strlen(I.m_sTag()));
  //  for (int i=0; i<I.m_FIOs.GetSize(); i++)
  //    DoMeasures(I, I.m_FIOs[i], m_TV, m_lInWide, m_lOutWide);
  //  for (i=0; i<I.m_CIOs.GetSize(); i++)
  //    DoMeasures(I, I.m_CIOs[i], m_TV, m_lInWide, m_lOutWide);
  //  for (i=0; i<I.m_XIOs.GetSize(); i++)
  //    DoMeasures(I, I.m_XIOs[i], m_TV, m_lInWide, m_lOutWide);
  //  }

  //if (0 && (m_TV!=TV_Value))
  //  {
  //  for (int ic=0; ic<MaxValCols; ic++)
  //    {
  //    double l=log10(GTZ(m_dMax[ic]));//-1.999;
  //    m_iPower[ic]=(int)(l<0 ? l-0.9999999:l);
  //    m_dScale[ic]=pow(10, m_iPower[ic]);
  //    }
  //  }

  //Strng SValsShown;
  //SValsShown.Set("%s:%i", m_TV!=TV_Value?"Chgs":"Vals", m_nValuesShown);
  //m_ChgCount.SetWindowText(SValsShown());

  //bool PrevHasNOIO=false;

  //double Y=YStart, X=XStart;
  //Y+=1;
  //for (t=0; t<m_Info.GetSize(); t++)
  //  {
  //  CEvalOrderItem &I=m_Info[t];
  //  long NoFIOs=I.m_FIOs.GetSize();
  //  long NoCIOs=I.m_CIOs.GetSize();
  //  long NoXIOs=I.m_XIOs.GetSize();

  //  long NFIn=0, NFOut=0;
  //  long NCIn=0, NCOut=0;
  //  long NXIn=0, NXOut=0;
  //  long NTFIn=0, NTFOut=0;
  //  long NTCIn=0, NTCOut=0;
  //  long NTXIn=0, NTXOut=0;
  //  bool PrevInSeq=false;
  //  bool NextInSeq=false;
  //  if (m_bDoProc)
  //    {
  //    for (long i=0; i<NoFIOs; i++)
  //      {
  //      CEvalOrderIOItem &F=m_Info[t].m_FIOs[i];
  //      long iRmtOrd=F.m_lRmtProcOrd;
  //      if (F.m_bIn && iRmtOrd==I.m_lProcOrd-1)
  //        PrevInSeq=true;
  //      else if (F.m_bOut && iRmtOrd==I.m_lProcOrd+1)
  //        NextInSeq=true;
  //      }
  //    }
  //  else
  //    {
  //    for (long i=0; i<NoCIOs; i++)
  //      {
  //      CEvalOrderIOItem &C=m_Info[t].m_CIOs[i];
  //      long iRmtOrd=C.m_lRmtCtrlOrd;
  //      if (C.m_bIn && iRmtOrd==I.m_lCtrlOrd-1)
  //        PrevInSeq=true;
  //      else if (C.m_bOut && iRmtOrd==I.m_lCtrlOrd+1)
  //        NextInSeq=true;
  //      }
  //    for (i=0; i<NoXIOs; i++)
  //      {
  //      CEvalOrderIOItem &X=m_Info[t].m_XIOs[i];
  //      long iRmtOrd=X.m_lRmtCtrlOrd;
  //      if (X.m_bIn && iRmtOrd==I.m_lCtrlOrd-1)
  //        PrevInSeq=true;
  //      else if (X.m_bOut && iRmtOrd==I.m_lCtrlOrd+1)
  //        NextInSeq=true;
  //      }
  //    }
  //  for (long i=0; i<NoFIOs; i++)
  //    CountIO(m_Info[t].m_FIOs[i], m_bShowTrsOnly, NFIn, NFOut, NTFIn, NTFOut);
  //  for (i=0; i<NoCIOs; i++)
  //    CountIO(m_Info[t].m_CIOs[i], m_bShowTrsOnly, NCIn, NCOut, NTCIn, NTCOut);
  //  for (i=0; i<NoXIOs; i++)
  //    CountIO(m_Info[t].m_XIOs[i], m_bShowTrsOnly, NXIn, NXOut, NTXIn, NTXOut);

  //  bool ThisHasNOIO=m_bDoProc ? (NFIn==0 && NFOut==0) : (NCIn+NXIn==0 && NCOut+NXOut==0);
  //  if (PrevInSeq || PrevHasNOIO && ThisHasNOIO)
  //    {}
  //  else
  //    Y+=YGap;

  //  PrevHasNOIO=ThisHasNOIO;

  //  int NTot;
  //  if (m_bDoProc)
  //    NTot=Max(1L, Max((PrevInSeq?NTFIn-1:NTFIn)+NTCIn+NTXIn, (NextInSeq?NTFOut-1:NTFOut)+NTCOut+NTXOut));
  //  else
  //    NTot=Max(1L, Max((PrevInSeq?NTCIn-1:NTCIn)+NTXIn, (NextInSeq?NTCOut-1:NTCOut)+NTXOut));
  //  Y+=NTot;
  //  }
  //if (1)
  //  Y+=YGap;

  //m_Size.y=(long)(Y*ScrGB.RowHgt());
  //m_Size.x=(long)((XStart+iPDigs+1+m_lInWide+2+m_lINValueIds*m_ChgWd+XGap+m_lTgWide+XGap+m_lONValueIds*m_ChgWd+m_lOutWide+2)*ScrGB.ColWdt());
    
    }

  //SetScrollBars();
  //Invalidate();
  }

//---------------------------------------------------------------------------

void CWirePanel::SetScrollBars()
  {
  CRect Cr, r;
  if (m_ViewArea.m_hWnd==NULL)
    return;
  m_ViewArea.GetClientRect(&Cr);

  m_Pos.x=Range(0L, m_Pos.x, Max(1L, long(m_Size.x-Cr.Width())));
  m_Pos.y=Range(0L, m_Pos.y, Max(1L, long(m_Size.y-Cr.Height())));

  m_ScInfoX.cbSize=sizeof(m_ScInfoX);
  m_ScInfoX.fMask=SIF_ALL|SIF_DISABLENOSCROLL;
  m_ScInfoX.nMin=0;
  m_ScInfoX.nMax=Max(1L, m_Size.x/ScrGB.ColWdt());
  m_ScInfoX.nPage=Cr.Width()/ScrGB.ColWdt();
  m_ScInfoX.nPos=m_Pos.x/ScrGB.ColWdt();
  m_ScInfoX.nTrackPos=0;
  m_ScrollH.SetScrollInfo(&m_ScInfoX, true);

  m_ScInfoY.cbSize=sizeof(m_ScInfoY);
  m_ScInfoY.fMask=SIF_ALL|SIF_DISABLENOSCROLL;
  m_ScInfoY.nMin=0;
  m_ScInfoY.nMax=Max(1L, m_Size.y/ScrGB.RowHgt());
  m_ScInfoY.nPage=Cr.Height()/ScrGB.RowHgt();
  m_ScInfoY.nPos=m_Pos.y/ScrGB.RowHgt();
  m_ScInfoY.nTrackPos=0;
  m_ScrollV.SetScrollInfo(&m_ScInfoY, true);
  }

//---------------------------------------------------------------------------
const long ScrollGap=1;

void CWirePanel::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
  {
  if (pScrollBar)
    {
    switch (nSBCode)
      {
      case SB_BOTTOM        : m_Pos.y=(m_ScInfoY.nMax-m_ScInfoY.nPage+ScrollGap)*ScrGB.RowHgt(); break;
      case SB_LINEDOWN      : m_Pos.y+=ScrGB.RowHgt(); break;
      case SB_LINEUP        : m_Pos.y-=ScrGB.RowHgt(); break;
      case SB_PAGEDOWN      : m_Pos.y+=m_ScInfoY.nPage*ScrGB.RowHgt(); break;
      case SB_PAGEUP        : m_Pos.y-=m_ScInfoY.nPage*ScrGB.RowHgt(); break;
      case SB_THUMBPOSITION :  // Scroll to the absolute position. The current position is provided in nPos.
      case SB_THUMBTRACK    : m_Pos.y=nPos*ScrGB.RowHgt(); break; // Drag scroll box to specified position. The current position is provided in nPos.
      case SB_TOP           : m_Pos.y=0; break;
      case SB_ENDSCROLL     : break;
      }

    m_Pos.y=Range(0L, m_Pos.y, Max(0L, long(m_ScInfoY.nMax-m_ScInfoY.nPage+ScrollGap)*ScrGB.RowHgt()));
    pScrollBar->SetScrollPos(m_Pos.y/ScrGB.RowHgt());

    RedrawWindow(NULL, &m_ClipRgn);
    }
  else
    CFormView::OnVScroll(nSBCode, nPos, pScrollBar);
  }

//---------------------------------------------------------------------------

void CWirePanel::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
  {
  //CSliderCtrl *pSlide=dynamic_cast<CSliderCtrl *>(pScrollBar);
  //if (pSlide)
  //  {
  //  switch (nSBCode)
  //    {
  //    case TB_BOTTOM        : m_iIODispTol=100; break;
  //    case TB_LINEDOWN      : m_iIODispTol+=1; break;
  //    case TB_LINEUP        : m_iIODispTol-=1; break;
  //    case TB_PAGEDOWN      : m_iIODispTol+=2; break;
  //    case TB_PAGEUP        : m_iIODispTol-=2; break;
  //    case TB_THUMBPOSITION : // Scroll to the absolute position. The current position is provided in nPos.
  //    case TB_THUMBTRACK    : m_iIODispTol=nPos; break; // Drag scroll box to specified position. The current position is provided in nPos.
  //    case TB_TOP           : m_iIODispTol=0; break;
  //    case TB_ENDTRACK: break;
  //    }

  //  //m_dIODispTol= (m_iIODispTol>0) ? pow(10.0, m_iIODispTol*0.1-10) : 0;

  //  GetConnectInfo();
	 // RedrawGraph();
  //  }
  //else 
  if (pScrollBar)
    {
    switch (nSBCode)
      {
      case SB_BOTTOM        : m_Pos.x=(m_ScInfoX.nMax-m_ScInfoX.nPage+ScrollGap)*ScrGB.ColWdt(); break;
      case SB_LINEDOWN      : m_Pos.x+=ScrGB.ColWdt(); break;
      case SB_LINEUP        : m_Pos.x-=ScrGB.ColWdt(); break;
      case SB_PAGEDOWN      : m_Pos.x+=m_ScInfoX.nPage*ScrGB.ColWdt(); break;
      case SB_PAGEUP        : m_Pos.x-=m_ScInfoX.nPage*ScrGB.ColWdt(); break;
      case SB_THUMBPOSITION :  // Scroll to the absolute position. The current position is provided in nPos.
      case SB_THUMBTRACK    : m_Pos.x=nPos*ScrGB.ColWdt(); break; // Drag scroll box to specified position. The current position is provided in nPos.
      case SB_TOP           : m_Pos.x=0; break;
      case SB_ENDSCROLL     : break;
      }

    m_Pos.x=Range(0L, m_Pos.x, Max(0L, long(m_ScInfoX.nMax-m_ScInfoX.nPage+ScrollGap)*ScrGB.ColWdt()));
    pScrollBar->SetScrollPos(m_Pos.x/ScrGB.ColWdt());

    RedrawWindow(NULL, &m_ClipRgn);
    }
  else
    CFormView::OnHScroll(nSBCode, nPos, pScrollBar);
  }

//---------------------------------------------------------------------------

void CWirePanel::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
  {
  CFormView::OnKeyDown(nChar, nRepCnt, nFlags);
  switch (nChar)
    {
    case VK_NEXT:
      m_Pos.x+=m_ScInfoX.nPage*ScrGB.ColWdt();
      break;
    case VK_PRIOR:
      m_Pos.x-=m_ScInfoX.nPage*ScrGB.ColWdt();
      break;
    }
  m_Pos.x=Range(0L, m_Pos.x, Max(0L, long(m_ScInfoX.nMax-m_ScInfoX.nPage+ScrollGap)*ScrGB.ColWdt()));
  m_ScrollV.SetScrollPos(m_Pos.x/ScrGB.ColWdt());

  CRect VA;
  m_ViewArea.GetClientRect(&VA);
  RedrawWindow(&VA);
  };

//---------------------------------------------------------------------------

//double CWirePanel::DrawStr(CEvalOrderItem *pItm, CEvalOrderIOItem *pIO, double C, double R, long Opts, char * fmt, ...)
//  {
//  char buff[1024];
//  va_list argptr;
//  va_start(argptr,fmt);
//  vsprintf(buff, fmt, argptr);
//  va_end(argptr);
//  int Len=strlen(buff);
//
//  GDIBlkCfg &gbc = ScrGB.GBC();
//  CDC &dc = ScrGB.DC();
//  CDCResChk ResChk(dc);
//  CBrush *pOldBrush=dc.GetCurrentBrush();
//  CFont  *pOldFont =dc.GetCurrentFont();
//  CPen   *pOldPen  =dc.GetCurrentPen();
//
//  int EdgeW=ScrGB.EdgeWdt();
//  int RowH=ScrGB.RowHgt();
//  int ColW=ScrGB.ColWdt();
//  int x=(int)(C*ColW) - (Opts & WithXOff ? m_Pos.x : 0);
//  int y=(int)(R*RowH) - (Opts & WithYOff ? m_Pos.y : 0);
//
//  int xr=x+ColW*Len;
//  int yb=y+RowH;
//
//  char * pTag=pIO?pIO->m_sRmt():NULL;
//  if (!pTag)
//    pTag=pItm?pItm->m_sTag():NULL;
//  if (pTag)
//    {
//    m_DigPts.SetSize(m_nDigPts+1, 256);
//    m_DigPts[m_nDigPts].m_Rect.SetRect(x, y, xr, yb);
//    //m_DigPts[m_nDigPts].m_pTag=pTag;
//    m_DigPts[m_nDigPts].m_pItm=pItm;
//    m_DigPts[m_nDigPts].m_pIO=pIO;
//    m_nDigPts++;
//    }
//  int TxtInv=0;
//  CBrush *pBackBrush;
//  if (ScrGB.IsPrinting())
//    pBackBrush=ScrGB.pBrushTxtFore;
//  else
//    pBackBrush=ScrGB.pBrushTxtBack;
//
//  flag IsButton=false;
//  flag fItalic=false;
//
//  if (Opts & WithBackGrnd)
//    {
//    CRect TxtBck  (x+EdgeW, y+EdgeW, xr, yb);
//    dc.FillRect(&TxtBck, pBackBrush);
//    }
//  else if (Opts & WithBox)
//    {
//    POINT TxtBox[]=
//      {
//        {x, y},
//        {xr, y},
//        {xr, yb},
//        {x, yb},
//        {x, y},
//      };
//    CPen   *pOldPen = dc.GetCurrentPen();
//    dc.SelectObject(m_pnLtGrey);
//    dc.Polyline(TxtBox, 5);
//    dc.SelectObject(pOldPen);
//    }
//
//  CRect Txt  (x+EdgeW, y+EdgeW, xr+1, yb);
//
//  if (fItalic)
//    dc.SelectObject(ScrGB.pItlFont[0]); // Italic
//  COLORREF TC;
//  TC=gbc.crText[TxtInv];
//  int OldTextColor=dc.SetTextColor(TC);
//
//  int OldBkMode=dc.SetBkMode(TRANSPARENT);
//  const int JustFlags[] = {DT_LEFT, DT_CENTER, DT_RIGHT, 0};
//  int JustFlag=0;//JustFlags[Just];
//
//  dc.DrawText(buff, strlen(buff), Txt, DT_END_ELLIPSIS|DT_SINGLELINE|DT_BOTTOM|DT_NOPREFIX|JustFlag);
//  dc.SetBkMode(OldBkMode);
//  dc.SetTextColor(OldTextColor);
//
//  dc.SelectObject(pOldBrush);
//  dc.SelectObject(pOldFont);
//  dc.SelectObject(pOldPen);
//  return C+Len;
//  }

#define XP(X) (short(X-m_Pos.x))
#define YP(Y) (short(Y-m_Pos.y))

inline long TLen(char * s) { return Max(0L,(long)strlen(s)); };

////---------------------------------------------------------------------------
//
//double CWirePanel::DrawIOInxStr(CEvalOrderItem *pItm, CEvalOrderIOItem *pIO, double X, double Y, long InxWd, bool DoProc)
//  {
//  long l=DoProc ? pIO->m_lRmtProcOrd:pIO->m_lRmtCtrlOrd;
//  if (l>=0)
//    return DrawStr(pItm, pIO, X, Y, WithXYOff|WithBox, "%*i ", InxWd-1, l);
//  return X+InxWd;
//  }
//
////---------------------------------------------------------------------------
//
//double CWirePanel::DrawIOChgStr(CEvalOrderItem *pItm, CEvalOrderIOItem *pIO, long Id, double X, double Y, long ChgWd, eTraceValue TV, bool DoProc)
//  {
//  double Val=dNAN;
//  if (Id==0)
//    {
//    Val=pIO->m_dMax;
//    }
//  else
//    {
//    for (int i=0; i<pIO->m_Trc.GetSize(); i++)
//      {
//      if (pIO->m_Trc[i].Id()==Id)
//        {
//        Val=pIO->m_Trc[i].CalcValue(TV);
//        break;
//        }
//      }
//    }
//  if (Valid(Val))
//    {
//    long Opts=WithXYOff|WithBox;
//    if (TV==TV_RelChg && fabs(Val)>=m_dIODispTol)
//      Opts|=WithBackGrnd;
//    switch (TV)
//      {
//      case TV_AbsChg: Val=Val/m_dScale[Id];        break;
//      case TV_RelChg: Val=100*Val/m_dScale[Id];    break;
//      case TV_Value:                               break;
//      }
//    if (fabs(Val)<1.0e-20)
//      return DrawStr(pItm, pIO, X, Y, Opts, "%*.*s ", ChgWd-1, ChgWd-1, "0");
//    if (TV==TV_Value)
//      return DrawStr(pItm, pIO, X, Y, Opts, ValFmt(Val), ChgWd-1, Val);
//    return DrawStr(pItm, pIO, X, Y, Opts, "%*.4f ", ChgWd-1, Val);
//    }
//  return DrawStr(pItm, pIO, X, Y, WithXYOff|WithBox, "%*.*s ", ChgWd-1, ChgWd-1, "");
//  //return X+ChgWd;
//  }
//
////---------------------------------------------------------------------------
//
//double CWirePanel::DrawVert(CDC &DC, CRect &GraphR, CPen &pnG, double X, double dX)
//  {
//  X+=dX;
//  long Ys=long(GraphR.top);
//  long Ye=long(GraphR.bottom);
//  long Xs=long(X*ScrGB.ColWdt());
//  long Xe=long(X*ScrGB.ColWdt());
//  CPen   *pOldPen = DC.GetCurrentPen();
//  DC.SelectObject(pnG);
//  DC.MoveTo(XP(Xs), YP(Ys));
//  DC.LineTo(XP(Xe), YP(Ye));
//  DC.SelectObject(pOldPen);
//  return X;
//  }
//
////---------------------------------------------------------------------------
////---------------------------------------------------------------------------
//
//void CWirePanel::DrawAllVerts(CDC &DC, CRect &GraphR, CPen &pnG, double X)
//  {
//  return ;
//
//  X=DrawVert(DC, GraphR, pnG, X, iPDigs+1);
//  X=DrawVert(DC, GraphR, pnG, X, m_lInWide+2);
//
//  for (long id=m_lIValueId1; id<m_lIValueId1+m_lINValueIds; id++)
//    X=DrawVert(DC, GraphR, pnG, X, m_ChgWd);
//  X=DrawVert(DC, GraphR, pnG, X, XGap);
//  if (m_bShowIOInx)
//    X=DrawVert(DC, GraphR, pnG, X, m_InxWd);
//  X=DrawVert(DC, GraphR, pnG, X, m_lTgWide);
//  X=DrawVert(DC, GraphR, pnG, X, XGap);
//
//  if (m_bShowIOInx)
//    X=DrawVert(DC, GraphR, pnG, X, m_InxWd);
//  for (id=m_lOValueId1; id<m_lOValueId1+m_lONValueIds; id++)
//    X=DrawVert(DC, GraphR, pnG, X, m_ChgWd);
//
//  X=DrawVert(DC, GraphR, pnG, X, m_lOutWide+2);
//  }

//---------------------------------------------------------------------------

void CWPWire::GetSrcWndPt(CPoint &IOPt, CRect &IOSym)
  {
  CRect   IRct, LRct;
  long iTrm=m_pSrc->m_iTerm;
  CWnd &LWnd=*m_pSrc->m_pTS;
  CListCtrl &C=m_pSrc->m_pTS->m_TermList;
  C.GetItemRect(iTrm, &IRct, LVIR_BOUNDS);
  C.ClientToScreen(&IRct);
  LWnd.GetWindowRect(&LRct);

  long H=IRct.Height()/3;
  long W=IRct.Height()/2;

  IRct.left=LRct.left;
  IRct.right=LRct.right;
  IRct.top=Range(LRct.top+2L, IRct.top+H, LRct.bottom-3L);
  IRct.bottom=Range(LRct.top+2L, IRct.bottom-H, LRct.bottom-3L);

  IOPt=CPoint(IRct.right+W, (IRct.top+IRct.bottom)/2);
  IOSym=CRect(IRct.right, IRct.top, IRct.right+W, IRct.bottom);
  }

void CWPWire::GetDstWndPt(CPoint &IOPt, CRect &IOSym)
  {
  CRect   IRct, LRct;
 
  long iTrm=m_pDst->m_iTerm;
  CWnd &LWnd=*m_pDst->m_pTS;
  CListCtrl &C=m_pDst->m_pTS->m_TermList;
  C.GetItemRect(iTrm, &IRct, LVIR_BOUNDS);
  C.ClientToScreen(&IRct);
  LWnd.GetWindowRect(&LRct);

  long H=IRct.Height()/3;
  long W=IRct.Height()/2;

  IRct.left=LRct.left;
  IRct.right=LRct.right;
  IRct.top=Range(LRct.top+2L, IRct.top+H, LRct.bottom-3L);
  IRct.bottom=Range(LRct.top+2L, IRct.bottom-H, LRct.bottom-3L);
  
  IOPt=CPoint(IRct.left-W, (IRct.top+IRct.bottom)/2);
  IOSym=CRect(IRct.left-W, IRct.top, IRct.left, IRct.bottom);
  }


const long CableStubX=8;

void CWirePanel::OnDraw(CDC* pDC)
  {
  if (m_bFirstDraw)
    {
    m_bFirstDraw=false;
    GetConnectInfo();
    SetScrollBars();
    }

  CDCResChk ResChk(pDC);

  pDC->SetMapMode(MM_TEXT);
  ScrGB.Attach(pDC, NULL);

  CDC &DC = *pDC;

  CFont *pOldFont=DC.GetCurrentFont();
  CGdiObject* pOldBrush = (CGdiObject*)DC.SelectObject(ScrGB.pBrushGrfBack);
  COLORREF OldBkColor = DC.SetBkColor(ScrGB.crGrfBack);
  CPen* pOldPen = DC.SelectObject(ScrGB.pPenTxtBord);

  CRect GraphR;

  for (int c=0; c<m_Cables.GetSize(); c++)
    {
    long NCount=0;

    for (int w=0; w<m_Wires.GetSize(); w++)
      if (m_Wires[w]->m_pCable==m_Cables[c])
        {
        CWGData D;
        D.m_pWire=m_Wires[w];
        m_Wires[w]->GetSrcWndPt(D.m_SrcPt, D.m_SrcSym);
        m_Wires[w]->GetDstWndPt(D.m_DstPt, D.m_DstSym);
        ScreenToClient(&D.m_SrcPt);
        ScreenToClient(&D.m_DstPt);
        ScreenToClient(&D.m_SrcSym);
        ScreenToClient(&D.m_DstSym);
        m_WGData.SetAtGrow(NCount++, D);
        }

    long iXSrc    = -10000;
    long iYSrcT   = +10000;
    long iYSrcB   = -10000;
    long iXDst    = +10000;
    long iYDstT   = +10000;
    long iYDstB   = -10000;
    long iXSrcCb  = -10000;
    long iXDstCb  = +10000;

    for (w=0; w<NCount; w++)
      {
      CRect GrpRct;

      if (1)
        {
        CWPGroup *pSrcGrp = m_WGData[w].m_pWire->m_pSrc->m_pTS->m_pGrp;
        pSrcGrp->GetWindowRect(&GrpRct);
        ScreenToClient(&GrpRct);
        iXSrcCb=Max(iXSrcCb, GrpRct.right);
        
        CWPGroup *pDstGrp = m_WGData[w].m_pWire->m_pDst->m_pTS->m_pGrp;
        pDstGrp->GetWindowRect(&GrpRct);
        ScreenToClient(&GrpRct);
        iXDstCb=Min(iXDstCb, GrpRct.left);
        }

      CPoint &SrcPt=m_WGData[w].m_SrcPt;
      if (SrcPt.x>iXSrc)
        {
        iXSrc   = SrcPt.x;
        iYSrcT  = SrcPt.y;
        iYSrcB  = SrcPt.y;
        }
      else if (SrcPt.x==iXSrc)
        {
        iYSrcT  = Min(iYSrcT, SrcPt.y);
        iYSrcB  = Max(iYSrcB, SrcPt.y);
        }
      CPoint &DstPt=m_WGData[w].m_DstPt;
      if (DstPt.x<iXDst)
        {
        iXDst   = DstPt.x;
        iYDstT  = DstPt.y;
        iYDstB  = DstPt.y;
        }
      else if (DstPt.x==iXDst)
        {
        iYDstT  = Min(iYDstT, DstPt.y);
        iYDstB  = Max(iYDstB, DstPt.y);
        }
      }

    iXSrc=Max(iXSrc+CableStubX, iXSrcCb);
    iXDst=Min(iXDst-CableStubX, iXDstCb);

    long iYSrc=YP(iYSrcT+iYSrcB)/2;
    long iYDst=YP(iYDstT+iYDstB)/2;
   
    for (w=0; w<NCount; w++)
      {
      CPoint &SrcPt=m_WGData[w].m_SrcPt;
      CPoint &DstPt=m_WGData[w].m_DstPt;
      CRect  &SrcSym=m_WGData[w].m_SrcSym;
      CRect  &DstSym=m_WGData[w].m_DstSym;

      DC.SelectObject(m_pnWire[w]);
      DC.SelectObject(m_brWire[w]);
      POINT SrcTri[]={
        {SrcSym.left,  SrcSym.top}, 
        {SrcSym.right, (SrcSym.top+SrcSym.bottom)/2}, 
        {SrcSym.left,  SrcSym.bottom}};
      DC.Polygon(SrcTri, 3);
      DC.MoveTo(XP(SrcPt.x), YP(SrcPt.y));
      DC.LineTo(XP(iXSrc), YP(iYSrc));

     
      DC.SelectObject(m_pnWire[w]);
      DC.SelectObject(m_brWire[w]);
      DC.MoveTo(XP(iXDst), YP(iYDst));
      DC.LineTo(XP(DstPt.x), YP(DstPt.y));
      POINT DstTri[]={
        {DstSym.right,  DstSym.top},
        {DstSym.right,  DstSym.bottom},
        {DstSym.left,   (DstSym.top+DstSym.bottom)/2}};
      DC.Polygon(DstTri, 3);
      }

    DC.SelectObject(m_pnCable);
    DC.SelectObject(m_brCable);

    POINT SrcCb[]={
      {iXSrc, iYSrc+2}, 
      {iXDst, iYDst+2}, 
      {iXDst, iYDst-2}, 
      {iXSrc, iYSrc-2},};
    DC.Polygon(SrcCb, 4);
    }

  DC.SelectObject(pOldPen);
  DC.SetBkColor(OldBkColor);

  DC.SelectObject(pOldBrush);
  DC.SelectObject(pOldFont);
  m_TitleClipRgn.DeleteObject();
  ScrGB.Detach();
  }

//---------------------------------------------------------------------------

void CWirePanel::UpdateWindowText()
  {
  }

//---------------------------------------------------------------------------

void CWirePanel::OnClose()
  {
  GetParent()->ShowWindow(SW_SHOWMINNOACTIVE);
  }

//---------------------------------------------------------------------------

void CWirePanel::RedrawGraph()
  {
  CRect GraphR;
  m_ViewArea.GetWindowRect(&GraphR);
  ScreenToClient(&GraphR);
  RedrawWindow(&GraphR);
  }

//---------------------------------------------------------------------------

void CWirePanel::OnRefresh()
  {
  GetConnectInfo();
	RedrawGraph();
  }

//---------------------------------------------------------------------------

void CWirePanel::OnUpdateBtn(CCmdUI* pCmdUi)
  {
  //CCustomListCtrl &MsgCtrl = *(CCustomListCtrl*)GetDlgItem(IDC_MSGLIST);
  //CCustomListCtrl &CndCtrl = *(CCustomListCtrl*)GetDlgItem(IDC_CONDLIST);

  pCmdUi->Enable(true);//MsgCtrl.GetNextItem(-1, LVNI_SELECTED)>=0 || CndCtrl.GetNextItem(-1, LVNI_SELECTED)>=0);
  }

//---------------------------------------------------------------------------

BOOL CWirePanel::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
  {
  HD_NOTIFY* pNM = (HD_NOTIFY*)lParam;
  switch (pNM->hdr.code)
    {
    case TCN_SELCHANGE:
      {
      NMHDR * lpnmhdr = (LPNMHDR) lParam;
      DoLayout();
      }
      break;
//    case HDN_ENDTRACK: //column width changed
//      {
//      CCustomListCtrl &MsgCtrl = *(CCustomListCtrl*)GetDlgItem(IDC_MSGLIST);
//      CCustomListCtrl &CndCtrl = *(CCustomListCtrl*)GetDlgItem(IDC_CONDLIST);
//
//      flag IsCond=(::GetParent(pNM->hdr.hwndFrom)==GetDlgItem(IDC_CONDLIST)->m_hWnd);
//      CListCtrl & Ctrl = IsCond ? CndCtrl : MsgCtrl;
//      int TWd=60;
//      Ctrl.SetColumnWidth(0, Max(TWd, Ctrl.GetColumnWidth(0)));
//      CRect Rect;
//      Ctrl.GetWindowRect(&Rect);
//      Ctrl.SetColumnWidth(1, Max(TWd, Ctrl.GetColumnWidth(1)));
//      break;
//      }
//    case LVN_GETDISPINFO:
//      {
//      NMLVDISPINFO * pnmv = (NMLVDISPINFO*) lParam;
//      flag IsCond=(pnmv->hdr.idFrom==IDC_CONDLIST);
//      POSITION Pos=(POSITION)pnmv->item.lParam;
//      COrdList * pList=IsCond ? &Shared.m_CndList : &Shared.m_MsgList;
//      COrdItem &It=pList->GetAt(Pos);
//      switch (pnmv->item.iSubItem)
//        {
//        case 0:
//          pnmv->item.iImage=It.iImg;
//          pnmv->item.pszText=It.sSrc() ? It.sSrc() : "";
//          break;
//        case 1:
//          pnmv->item.pszText=It.sTxt() ? It.sTxt() : "";
//          break;
//        }
//      }
//      break;
    }
  BOOL b = CFormView::OnNotify(wParam, lParam, pResult);
  return b;
  }

//---------------------------------------------------------------------------

void CWirePanel::OnLButtonDown(UINT nFlags, CPoint point)
  {
  CFormView::OnLButtonDown(nFlags, point);

  if (nFlags & MK_LBUTTON)
    ShowInfo(point, FindDigPt(nFlags, point));
  else
    CloseInfo();
  }

//---------------------------------------------------------------------------

void CWirePanel::OnLButtonUp(UINT nFlags, CPoint point)
  {
  CFormView::OnLButtonUp(nFlags, point);

  CloseInfo();
  }

//---------------------------------------------------------------------------

void CWirePanel::OnLButtonDblClk(UINT nFlags, CPoint point)
  {
  CFormView::OnLButtonDblClk(nFlags, point);
  }

//---------------------------------------------------------------------------

COrdDigPt * CWirePanel::FindDigPt(UINT nFlags, CPoint point)
  {
  for (long i=m_nDigPts-1; i>=0; i--)
    {
    if (m_DigPts[i].m_Rect.PtInRect(point))
      return &m_DigPts[i];
    }
  return NULL;
  }

//---------------------------------------------------------------------------

void CWirePanel::OnRButtonDown(UINT nFlags, CPoint point)
  {
  UpdateData(true);

  COrdDigPt * pPt=FindDigPt(nFlags, point);
  if (pPt)
    {
    CRect Rect;
    GetWindowRect(&Rect);

    CMenu Menu;
    Menu.CreatePopupMenu();
    Strng s;
    s.Set("&Access %s...", pPt->Tag());
    Menu.AppendMenu(MF_STRING, IDM_SLV_ACCESS, s());
    s.Set("&Find %s...", pPt->Tag());
    Menu.AppendMenu(MF_STRING|(0/*IsTearTag||IsFlashTrnTag*/?MF_GRAYED:0), IDM_SLV_FIND, s());
    //Menu.AppendMenu(MF_STRING, IDM_SLV_COPY, "&Copy tag");
    //Menu.AppendMenu(MF_STRING|(IsTearTag||IsFlashTrnTag?0:MF_GRAYED), IDM_SLV_COPYEXTRA, "Copy tag &Extra");
    //Menu.AppendMenu(MF_STRING|MF_GRAYED, IDM_SLV_COPYALL, "Copy all &tags");
    int RetCd = Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD, Rect.left+point.x, Rect.top+point.y, this);//Rect.left+Ctrl.PrevDownPoint.x, Rect.top+Ctrl.PrevDownPoint.y, pWnd);
    Menu.DestroyMenu();
    switch (RetCd)
      {
      case IDM_SLV_ACCESS:
        gs_AccessWnds.AccessNode(-1, pPt->Tag());
        break;
      case IDM_SLV_FIND:
        gs_pPrj->FindTag(pPt->Tag(), NULL, NULL, FTO_MoveCursor);
        break;
      //case IDM_SLV_COPY:
      //case IDM_SLV_COPYEXTRA:
      //  {
      //  Strng s;
      //  if (IsTearTag || IsFlashTrnTag)
      //    {
      //    int index = sTag.Find(".", 1);
      //    if (index>=0)
      //      {
      //      CString RTag = sTag.Mid(index+1);
      //      if (RetCd==IDM_SLV_COPYEXTRA)
      //        s.Set("%s.V.[%s].Meas\r\n%s.V.[%s].Error", (const char*)sObjTag, (const char*)RTag, (const char*)sObjTag, (const char*)RTag);//todo:add the cnvs....???
      //      else
      //        s.Set("%s.V.[%s].Meas", (const char*)sObjTag, (const char*)RTag);//todo:add the cnvs....???
      //      }
      //    else
      //      s = (const char*)sTag;
      //    }
      //  else// if (IsControlTag)
      //    {
      //    s.Set("%s.Cfg.[???].Meas", (const char*)sObjTag);
      //    //todo: for a PID control tag, we need to get to the required actual tag......
      //    }
      //  CopyTextToClipboard(this, s());
      //  break;
      //  }
      //case IDM_SLV_COPYALL:
      //  {
      //  Strng s;
      //  for (int i=0; i<MAX_EQNSLV_WORST; i++)
      //    {
      //    //todo
      //    }
      //  CopyTextToClipboard(this, s());
      //  break;
      //  }

      int xxx=0;
      }
    }

  CFormView::OnRButtonDown(nFlags, point);
  }

//---------------------------------------------------------------------------

void CWirePanel::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
  {
  if (bActivate)
    {
    GetConnectInfo();
    DoLayout();
    }

	CFormView::OnActivateView(bActivate, pActivateView, pDeactiveView);
  if (bActivate)
    {
    CButton &Close = *(CButton*)GetDlgItem(IDC_CLOSE);
    //ClrWndDef(m_Continue.m_hWnd);
    SetWndDef(Close.m_hWnd);
    SetFocus();
    Close.SetFocus();
    }
  else
    CloseInfo();
  }

//---------------------------------------------------------------------------

void CWirePanel::ShowInfo(CPoint point, COrdDigPt *pPt)
  {
  if (pPt)
    {
    CRect CRct;
    GetClientRect(&CRct);
    ClientToScreen(&CRct);
    point.x+=CRct.left+15;
    point.y+=CRct.top;
    if (!m_pInfoWnd)
      {
      //m_pInfoWnd=new CWireInfoWnd(m_TV, point.x, point.y, pPt, this);
      //m_pInfoWnd->SetWindowPos(&CWnd::wndTop, point.x, point.y,0,0,SWP_NOSIZE|SWP_SHOWWINDOW|SWP_NOACTIVATE);
      }
    else
      {
      //m_pInfoWnd->SetInfo(m_TV, point.x, point.y, pPt);
      //m_pInfoWnd->SetWindowPos(&CWnd::wndTop, point.x, point.y,0,0,SWP_NOSIZE|SWP_SHOWWINDOW|SWP_NOACTIVATE);
      //m_pInfoWnd->RedrawWindow();//Invalidate();
      }
    }
  else
    CloseInfo();
  m_pLastDig=pPt;
  }

//---------------------------------------------------------------------------

void CWirePanel::CloseInfo()
  {
  if (m_pInfoWnd)
    m_pInfoWnd->DestroyWindow();
  m_pInfoWnd=NULL;
  }

//---------------------------------------------------------------------------

void CWirePanel::OnMouseMove(UINT nFlags, CPoint point)
  {
  CFormView::OnMouseMove(nFlags, point);
  if (nFlags & MK_LBUTTON)
    ShowInfo(point, FindDigPt(nFlags, point));
  else
    CloseInfo();
  }

//---------------------------------------------------------------------------

LRESULT CWirePanel::OnCloseOrdInfo(WPARAM wParam, LPARAM lParam)
  {
  CloseInfo();
  return 0;
  }

//===========================================================================
//
//
//
//===========================================================================

class CWirePanelFrm : public CMDIChildWnd
  {
    DECLARE_DYNCREATE(CWirePanelFrm)
    CWirePanelFrm();           // protected constructor used by dynamic creation
  public:
    //{{AFX_VIRTUAL(CWirePanelFrm)
    //}}AFX_VIRTUAL
  protected:
    virtual ~CWirePanelFrm();
    //{{AFX_MSG(CWirePanelFrm)
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnClose();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  public:
  };

//---------------------------------------------------------------------------

IMPLEMENT_DYNCREATE(CWirePanelFrm, CMDIChildWnd)

CWirePanelFrm::CWirePanelFrm()
  {
  }

//---------------------------------------------------------------------------

CWirePanelFrm::~CWirePanelFrm()
  {
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CWirePanelFrm, CMDIChildWnd)
  //{{AFX_MSG_MAP(CWirePanelFrm)
  ON_WM_SETFOCUS()
  ON_WM_CLOSE()
  ON_WM_SIZE()
	//}}AFX_MSG_MAP
  ON_WM_ACTIVATE()
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

void CWirePanelFrm::OnSetFocus(CWnd* pOldWnd)
  {
  CWirePanel* pMsg = (CWirePanel*)GetTopWindow();
  if (pMsg)
    {
    pMsg->UpdateData(false);
    pMsg->DoLayout();
    //pMsg->UpdateDialogControls(pMsg, false);
    }
  CMDIChildWnd::OnSetFocus(pOldWnd);
  }

//---------------------------------------------------------------------------

void CWirePanelFrm::OnClose()
  {
  //CMDIChildWnd::OnClose(); Do NOT call base, we are not allowed to close this window!
  }

//---------------------------------------------------------------------------

void CWirePanelFrm::OnSize(UINT nType, int cx, int cy)
  {
  CMDIChildWnd::OnSize(nType, cx, cy);
  if (nType==SIZE_MAXIMIZED)
    CWindowLists::SetWndMaxMode(true);
  else if (nType==SIZE_MINIMIZED || nType==SIZE_RESTORED)
    CWindowLists::SetWndMaxMode(false);


  //CWirePanel *MW=dynamic_cast<CWirePanel*>(GetTopWindow());
  //if (MW)
  //  MW->DoLayout();
//  for (int i=0; i<2; i++)
//    {
//    CCustomListCtrl &Ctrl = *(CCustomListCtrl*)MW.GetDlgItem(i==0 ? IDC_MSGLIST : IDC_CONDLIST);
//    int TWd0=60, TWd1=60;
//    Ctrl.SetColumnWidth(0, Max(TWd0, Ctrl.GetColumnWidth(0)));
//    CRect Rect;
//    Ctrl.GetWindowRect(&Rect);
//    const int W = Rect.Width() - 20 - Ctrl.GetColumnWidth(0);
//    Ctrl.SetColumnWidth(1, Max(TWd1, Max(W,  Ctrl.GetColumnWidth(1))));
//    }
  }

void CWirePanel::OnSelchangeShowwhat()
  {
  //bool Enable = (m_ShowWhat.GetCurSel()==CS_Rel) || (m_ShowWhat.GetCurSel()==CS_MaxRel);
  //m_FilterNC.EnableWindow(Enable);
  //m_IOToleranceSlide.EnableWindow(Enable);

  OnRefresh();
  }

void CWirePanel::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

  OnRefresh();
	// TODO: Add your specialized code here and/or call the base class

}
//===========================================================================
//
//
//
//===========================================================================

void CWirePanelWindow::Register()
  {
  CRect Rect;
  CRect MainRect;
  HICON hIcon;
  CString ClassName;
  CCreateContext CC;

  AfxGetMainWnd()->GetClientRect(&MainRect);
  int w = Max(500,((MainRect.Width() - 30) * 5)/ 6);
  int h = Max(300,((MainRect.Height() - 30) *7)/ 8);

  //eval order window...  4
  Rect.left = 0;
  Rect.right = w;
  Rect.top = 0;
  Rect.bottom = h;
  CC.m_pCurrentDoc=NULL;
  CC.m_pNewDocTemplate=NULL;
  CC.m_pLastView=NULL;
  CC.m_pCurrentFrame=NULL;
  CC.m_pNewViewClass=RUNTIME_CLASS(CWirePanel);
  s_pWirePanelFrm = new CWirePanelFrm;
  hIcon = ScdApp()->LoadIcon(IDI_WIREWND);
  ClassName = AfxRegisterWndClass(CS_DBLCLKS|CS_NOCLOSE, 0, 0, hIcon);
  if (s_pWirePanelFrm->Create(ClassName.GetBuffer(0),
                        "Wiring Panel", WS_CHILD | WS_OVERLAPPED |
                        WS_CAPTION | WS_SYSMENU | WS_THICKFRAME |
                        WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
                        Rect, NULL, &CC))
    {
    s_pWirePanelWnd = (CWirePanel*)s_pWirePanelFrm->GetTopWindow();
//    RegisterMsgWnd(s_pWirePanelWnd->m_hWnd);
    s_pWirePanelFrm->ShowWindow(SW_MINIMIZE);
    }
  }

//---------------------------------------------------------------------------

int CWirePanelWindow::Show(LPCTSTR pRootTag, flag DoBringToTop)
  {
  if (GetFrame()==NULL)
    Register();
  s_pWirePanelWnd->SetRootTag(pRootTag);
  if (s_pWirePanelFrm && DoBringToTop && (s_pWirePanelFrm->GetFocus()!=s_pWirePanelFrm))
    {
    s_pWirePanelFrm->MDIActivate(); //bring the window to the front
    if (s_pWirePanelFrm->IsIconic())
      return s_pWirePanelFrm->ShowWindow(SW_RESTORE);
    }

  return 0;
  };

//---------------------------------------------------------------------------

int CWirePanelWindow::Refresh()
  {
  if (s_pWirePanelFrm && s_pWirePanelWnd)
    {
    if (!s_pWirePanelFrm->IsIconic())
      return s_pWirePanelWnd->DoRefresh();
    }
  return 0;
  };

//---------------------------------------------------------------------------

//int CWirePanelWindow::Hide()          { return s_pWirePanelFrm->ShowWindow(SW_HIDE); };
//int CWirePanelWindow::Reduce()        { return s_pWirePanelFrm->ShowWindow(SW_SHOWMINNOACTIVE); };

CFormView* CWirePanelWindow::GetWnd() { return s_pWirePanelWnd; };
CMDIChildWnd* CWirePanelWindow::GetFrame() { return s_pWirePanelFrm; };

//===========================================================================
//
//
//
//===========================================================================


//void CWPTermStrip::OnParentNotify(UINT message, LPARAM lParam)
//  {
//  CDialog::OnParentNotify(message, lParam);
//
//  // TODO: Add your message handler code here
//  }

//void CWPTermStrip::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
//  {
//  // TODO: Add your message handler code here and/or call default
//
//  CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
//  }

//void CWPScrollBar::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
//  {
//  // TODO: Add your message handler code here and/or call default
//
//  CScrollBar::OnVScroll(nSBCode, nPos, pScrollBar);
//  }

//void CWPScrollBar::OnParentNotify(UINT message, LPARAM lParam)
//  {
//  CScrollBar::OnParentNotify(message, lParam);
//
//  // TODO: Add your message handler code here
//  }


//---------------------------------------------------------------------------

//===========================================================================
//
//
//
//===========================================================================
// CWireInfoWnd dialog

//===========================================================================
//
//
//
//===========================================================================

CWireInfoWnd::CWireInfoWnd(eTraceValue TV, long X, long Y, COrdDigPt *pDigPt, CWnd* pParent /*=NULL*/)
	: CDialog(CWireInfoWnd::IDD, pParent),
  m_pnLtGrey(PS_SOLID, 0, crLtGrey)
  {

  //{{AFX_DATA_INIT(CWireInfoWnd)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

  m_bAllowMove=true;
  Create(CWireInfoWnd::IDD, pParent);
  SetInfo(TV, X, Y, pDigPt);
//  SetWindowText(pDigPt->Tag());
  }


void CWireInfoWnd::SetInfo(eTraceValue TV, long X, long Y, COrdDigPt *pDigPt)
  {
  char S[1024];

  m_TV=TV;
  m_Off.x=X;
  m_Off.y=Y;
  m_pDigPt=pDigPt;
  m_pItm=pDigPt->m_pItm;
  m_pIO=pDigPt->m_pIO;

  if (m_pIO)
    {
    if (m_pIO->m_lRmtCtrlOrd>=0)
      sprintf(S, "C:%i) %s", m_pIO->m_lRmtCtrlOrd, m_pDigPt->Tag());
    else
      sprintf(S, "P:%i) %s", m_pIO->m_lRmtProcOrd, m_pDigPt->Tag());
    }
  else
    {
    if (m_pItm->m_lCtrlOrd>=0)
      sprintf(S, "C:%i) %s", m_pItm->m_lCtrlOrd, m_pDigPt->Tag());
    else
      sprintf(S, "P:%i) %s", m_pItm->m_lProcOrd, m_pDigPt->Tag());
    }
  SetWindowText(S);
  };

void CWireInfoWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWireInfoWnd)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWireInfoWnd, CDialog)
	//{{AFX_MSG_MAP(CWireInfoWnd)
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWireInfoWnd message handlers

void CWireInfoWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CDialog::OnMouseMove(nFlags, point);
  point.Offset(m_Off);
  if (!m_pDigPt->m_Rect.PtInRect(point))
  //if (abs(point.x-m_pDigPt->m_Rect.CenterPoint().x)> m_pDigPt->m_Rect.Width())
    {
    if (GetParent())
      GetParent()->PostMessage(WMU_CLOSEORDINFO,0,0);
    }
  }

void CWireInfoWnd::DrawLine(CDC &DC, CPen &pnG, double X1, double Y1, double X2, double Y2)
  {
  CPen   *pOldPen = DC.GetCurrentPen();
  DC.SelectObject(pnG);
  DC.MoveTo((int)X1, (int)Y1);
  DC.LineTo((int)X2, (int)Y2);
  DC.SelectObject(pOldPen);
  }

void CWireInfoWnd::OnPaint()
  {
	CPaintDC dc(this); // device context for painting
  CDCResChk ResChk(dc);

  CRect ClientRct;
  GetClientRect(&ClientRct);

  CFont * OldFont=dc.SelectObject(GetFont()); // Italic
  int OldBkMode=dc.SetBkMode(TRANSPARENT);
  CPen * OldPen=dc.GetCurrentPen();

  GetClientRect(&ClientRct);
  CRect RctX;
  RctX.SetRect(0,0,1,1);

  CRect RctT=RctX;
  CRect RctC=RctX;
  CRect RctCI=RctX;

  dc.DrawText("X",  1,  RctX, DT_CALCRECT|DT_TOP|DT_NOPREFIX|DT_LEFT);

  long BlkW=RctX.Width();
  long BlkH=RctX.Height();
  long BlkWTot=BlkW*10;//NBlks;

  COrdDigPt & DP=*m_pDigPt;
  LPTSTR pTag=DP.Tag();;

  m_TotWide=100;
  m_TotDeep=30;

  long ItWd=3;
  long TagWd=7;
  long ColWd=10;
  long HeadDeep= 1;

  if (m_pIO)
    {
    CTraceArray &TA=m_pIO->m_Trc;

    m_TotWide=((TagWd+1)+(ItWd+1)+(ColWd+1)*TA.GetSize())*BlkW+1;
    m_TotDeep=(1+1+1+1+0.5);
    if (m_TV!=TV_Value)
      m_TotDeep+=0.5;
    m_TotDeep+=MaxChngTrcVals-((m_TV==TV_Value)?1:0);
    m_TotDeep*=BlkH;
    m_TotDeep+=HeadDeep+1;
    }

  CRect WRct;
  GetWindowRect(&WRct);

  bool DoMove=false;
  if (m_bAllowMove)
    {
    if (m_TotWide>ClientRct.right || m_TotDeep>ClientRct.bottom)// || m_TotDeep<ClientRct.bottom)
    //if (m_TotWide!=ClientRct.right || m_TotDeep!=ClientRct.bottom)
      {
      WRct.right+=(long)m_TotWide-ClientRct.right;
      WRct.bottom+=(long)m_TotDeep-ClientRct.bottom;
      DoMove=true;
      }
    }

  if (DoMove)
    {
    m_bAllowMove=false;
    Invalidate();
    MoveWindow(WRct);
    }
  else
    {
    m_bAllowMove=true;

    CRect Rct=ClientRct;

    Rct=ClientRct;
    Rct.top+=HeadDeep+1;
    Rct.bottom+=HeadDeep+1;
    //dc.DrawText(pTag, strlen(pTag), Rct, DT_TOP|DT_NOPREFIX|DT_LEFT);
    //Rct.OffsetRect(0, BlkH);
    char S[1024];//, S1[1024];

    //if (DP.m_pItm->m_lCtrlOrd>=0)
    //  sprintf(S, "C:%5i) %s", DP.m_pItm->m_lCtrlOrd, pTag);
    //else
    //  sprintf(S, "P:%5i) %s", DP.m_pItm->m_lProcOrd, pTag);
    //
    //dc.DrawText(S, strlen(S), Rct, DT_TOP|DT_NOPREFIX|DT_LEFT);
    //Rct.OffsetRect(0, BlkH);

    if (m_pIO)
      {
      CTraceArray &TA=m_pIO->m_Trc;
      char * pS=S;
      pS=S+sprintf(S, "%-*.*s ", TagWd,TagWd,"Tag");
      long x=ClientRct.left+(pS-S)*BlkW-BlkW/2;
      DrawLine(dc, m_pnLtGrey, x, ClientRct.top, x, ClientRct.bottom);

      pS+=sprintf(pS, "%*.*s ", ItWd,ItWd,"It");

      for (int j=0; j<TA.GetSize(); j++)
        {
        long x=ClientRct.left+(pS-S)*BlkW-BlkW/2;
        DrawLine(dc, m_pnLtGrey, x, ClientRct.top, x, ClientRct.bottom);
        pS+=sprintf(pS, "%*.*s ", ColWd, ColWd, TA[j].DescTag());
        }
      x=ClientRct.left+(pS-S)*BlkW-BlkW/2;
      DrawLine(dc, m_pnLtGrey, x, ClientRct.top, x, ClientRct.bottom);

      dc.DrawText(S, strlen(S), Rct, DT_TOP|DT_NOPREFIX|DT_LEFT);
      Rct.OffsetRect(0, BlkH);

      pS=S+sprintf(S, "%*.*s ",TagWd,TagWd,"");
      pS+=sprintf(pS, "%*.*s ", ItWd,ItWd,"");
      for (j=0; j<TA.GetSize(); j++)
        pS+=sprintf(pS, "%*.*s ", ColWd, ColWd, TA[j].CnvStr()?TA[j].CnvStr():"");
      dc.DrawText(S, strlen(S), Rct, DT_TOP|DT_NOPREFIX|DT_LEFT);
      Rct.OffsetRect(0, BlkH);
      DrawLine(dc, m_pnLtGrey, 0, Rct.top+BlkH/4, ClientRct.right, Rct.top+BlkH/4);
      Rct.OffsetRect(0, BlkH/2);

      pS=S+sprintf(S, "%-*.*s ", TagWd,TagWd,"Value");
      pS+=sprintf(pS, "%*i ", ItWd, 0);
      for (j=0; j<TA.GetSize(); j++)
        {
        double V=TA[j].Val(0);
        if (Valid(V))
          pS+=sprintf(pS, ValFmt(V), ColWd, V);
        else if (IsQuietNAN(V))
          pS+=sprintf(pS, "%*.*s ", ColWd, ColWd, ".");
        else
          pS+=sprintf(pS, "%*.*s ", ColWd, ColWd, "*");
        }
      dc.DrawText(S, strlen(S), Rct, DT_TOP|DT_NOPREFIX|DT_LEFT);
      Rct.OffsetRect(0, BlkH);
      if (m_TV!=TV_Value)
        {
        DrawLine(dc, m_pnLtGrey, 0, Rct.top+BlkH/4, ClientRct.right, Rct.top+BlkH/4);
        Rct.OffsetRect(0, BlkH/2);
        }

      if (0 && (m_TV==TV_RelChg))
        {
        pS=S+sprintf(S, "%-*.*s ", TagWd,TagWd,"");
        pS=S+sprintf(pS, "%*.*s ", ItWd, ItWd, "");
        for (j=0; j<TA.GetSize(); j++)
          pS+=sprintf(pS, "%*.*s ", ColWd, ColWd, "%");
        dc.DrawText(S, strlen(S), Rct, DT_TOP|DT_NOPREFIX|DT_LEFT);
        Rct.OffsetRect(0, BlkH);
        }

      LPCTSTR pPerc=(m_TV==TV_RelChg)?"%":" ";
      double Scl=(m_TV==TV_RelChg) ? 100:1;
      long Iter1=(m_TV==TV_Value)?1:0;
      //long IterOff=(m_TV==TV_Value)?0:-1;
      for (int i=Iter1; i<MaxChngTrcVals; i++)
        {
        static LPCTSTR RowHd[]= { "TV_None", "dV(Abs)", "dV(%)", "Value" };

        pS=S+sprintf(S, "%-*.*s ", TagWd,TagWd,RowHd[m_TV]);
        pS+=sprintf(pS, "%*i ", ItWd, -i);
        for (int j=0; j<TA.GetSize(); j++)
          {
          double V=dQuietNAN;
          for (int ii=0; ii<TA[j].NVals(); ii++)
            if (TA[j].IterNo(ii)==(CTraceItem::CurrentIter()-i))
               {
               V=TA[j].CalcValue(m_TV, ii);
               break;
               }
          if (Valid(V))
            {
            if (m_TV==TV_Value)
              pS+=sprintf(pS, ValFmt(V), ColWd, V);
            else if (fabs(V)>1.0e-20)
              pS+=sprintf(pS, "%+*.5f ", ColWd, V*Scl);
            else
              pS+=sprintf(pS, "%*.*s ", ColWd, ColWd, "0");
            }
          else if (IsQuietNAN(V))
            pS+=sprintf(pS, "%*.*s ", ColWd, ColWd, ".");
          else
            pS+=sprintf(pS, "%*.*s ", ColWd, ColWd, "*");
          }
        dc.DrawText(S, strlen(S), Rct, DT_TOP|DT_NOPREFIX|DT_LEFT);
        Rct.OffsetRect(0, BlkH);
        }
      }
    }


  dc.SetBkMode(OldBkMode);
  dc.SelectObject(OldFont);
  dc.SelectObject(OldPen);

	// Do not call CDialog::OnPaint() for painting messages
  }
