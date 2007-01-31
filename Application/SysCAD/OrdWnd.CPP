//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#include "stdafx.h"

#define __ORDWND_CPP

#include "sc_defs.h"
#include "resource.h"
#include "ordwnd.h"
#include "project.h"
#include "executiv.h"
#include "syscad.h"
#include "accnode.h"

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

const COLORREF crB=RGB(0x00,0x00,0x00);
const COLORREF crG=RGB(0x80,0x80,0x80);
const COLORREF crLg=RGB(0xc0,0xc0,0xc0);
const COLORREF crP=RGB(0x00,0x00,0xff);
const COLORREF crC=RGB(0xff,0x00,0x00);
const COLORREF crE=RGB(0xd0,0xd0,0x00);
const COLORREF crA=RGB(0x00,0xff,0xff);
const COLORREF crX=RGB(0xff,0x00,0x00);

static LPTSTR ValFmt(double V) { return fabs(V)>=1e6 ? "%*.2e " : "%*.2f "; };


/////////////////////////////////////////////////////////////////////////////
// COrdInfoWnd dialog

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

class COrdInfoWnd : public CDialog
{
// Construction
public:
	COrdInfoWnd(eTraceValue TV, long X, long Y, COrdDigPt *pDigPt, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COrdInfoWnd)
	enum { IDD = IDD_ORDINFO };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

  void SetInfo(eTraceValue TV, long X, long Y, COrdDigPt *pDigPt);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COrdInfoWnd)
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

  CPen              pnLg;

  void DrawLine(CDC &DC, CPen &pnG, double X1, double Y1, double X2, double Y2);

	// Generated message map functions
	//{{AFX_MSG(COrdInfoWnd)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// COrdInfoWnd dialog


COrdInfoWnd::COrdInfoWnd(eTraceValue TV, long X, long Y, COrdDigPt *pDigPt, CWnd* pParent /*=NULL*/)
	: CDialog(COrdInfoWnd::IDD, pParent),
  pnLg(PS_SOLID, 0, crLg)
  {
   
  //{{AFX_DATA_INIT(COrdInfoWnd)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
  
  m_bAllowMove=true;
  Create(COrdInfoWnd::IDD, pParent);
  SetInfo(TV, X, Y, pDigPt);
//  SetWindowText(pDigPt->Tag());
  }


void COrdInfoWnd::SetInfo(eTraceValue TV, long X, long Y, COrdDigPt *pDigPt)
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

void COrdInfoWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COrdInfoWnd)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COrdInfoWnd, CDialog)
	//{{AFX_MSG_MAP(COrdInfoWnd)
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COrdInfoWnd message handlers

void COrdInfoWnd::OnMouseMove(UINT nFlags, CPoint point) 
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

void COrdInfoWnd::DrawLine(CDC &DC, CPen &pnG, double X1, double Y1, double X2, double Y2) 
  {
  CPen   *pOldPen = DC.GetCurrentPen();
  DC.SelectObject(pnG); 
  DC.MoveTo((int)X1, (int)Y1);
  DC.LineTo((int)X2, (int)Y2);
  DC.SelectObject(pOldPen); 
  }

void COrdInfoWnd::OnPaint() 
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
      DrawLine(dc, pnLg, x, ClientRct.top, x, ClientRct.bottom);

      pS+=sprintf(pS, "%*.*s ", ItWd,ItWd,"It");
      
      for (int j=0; j<TA.GetSize(); j++)
        {
        long x=ClientRct.left+(pS-S)*BlkW-BlkW/2;
        DrawLine(dc, pnLg, x, ClientRct.top, x, ClientRct.bottom);
        pS+=sprintf(pS, "%*.*s ", ColWd, ColWd, TA[j].DescTag());
        }
      x=ClientRct.left+(pS-S)*BlkW-BlkW/2;
      DrawLine(dc, pnLg, x, ClientRct.top, x, ClientRct.bottom);

      dc.DrawText(S, strlen(S), Rct, DT_TOP|DT_NOPREFIX|DT_LEFT); 
      Rct.OffsetRect(0, BlkH);

      pS=S+sprintf(S, "%*.*s ",TagWd,TagWd,"");
      pS+=sprintf(pS, "%*.*s ", ItWd,ItWd,"");
      for (j=0; j<TA.GetSize(); j++)
        pS+=sprintf(pS, "%*.*s ", ColWd, ColWd, TA[j].CnvStr()?TA[j].CnvStr():"");
      dc.DrawText(S, strlen(S), Rct, DT_TOP|DT_NOPREFIX|DT_LEFT); 
      Rct.OffsetRect(0, BlkH);
      DrawLine(dc, pnLg, 0, Rct.top+BlkH/4, ClientRct.right, Rct.top+BlkH/4);
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
        DrawLine(dc, pnLg, 0, Rct.top+BlkH/4, ClientRct.right, Rct.top+BlkH/4);
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

//===========================================================================
//
//
//
//===========================================================================

const long WithXOff     = 0x00000001;
const long WithYOff     = 0x00000002;
const long WithXYOff    = 0x00000003;
const long WithBackGrnd = 0x00000004;
const long WithBox      = 0x00000008;

class COrdColumn
  {
  public:
    int m_iFirstTag, m_iLastTag;
    int m_iWidth;
  };

class COrdColumns : public CArray <COrdColumn, COrdColumn&> {};

class COrdWnd;
class COrdFrm;

static COrdWnd *s_pOrdWnd=NULL;
static COrdFrm *s_pOrdFrm=NULL;

// must match the strings in the Control; 
enum CurSels { CS_None, CS_Val, CS_Abs, CS_Rel, CS_MaxAbs, CS_MaxRel };

const long MaxValCols=7; // must expand for more values
class COrdWnd : public CFormView
  {
  friend class COrdFrm;
  public:
	//COrdWnd(CWnd* pParent = NULL);   // standard constructor

  protected:
    COrdWnd();           // protected constructor used by dynamic creation
    DECLARE_DYNCREATE(COrdWnd)

    static GDIBlkCfg GDICfg;
    GDIBlk           ScrGB;
      
    CEvalOrderArray m_Info;
    //CEvalOrderArray CInfo;
    CXRefInfoArray   m_Refs; //TODO : use global gs_PE.Refs!!!
    COrdColumns     m_Cols; 

    CPoint        m_Size;
    CPoint        m_Pos;
    long          m_lTgWide, m_lInWide, m_lOutWide;
    
    bool          m_bFirstDraw;
    bool          m_InitValuesSet;

    CRect         m_InitClntRect;

    SCROLLINFO    m_ScInfoX;
    SCROLLINFO    m_ScInfoY;

    CImageList    m_ImgList;           //bitmap images used in list boxes
    flag          m_bInited;

    int           m_iIODispTol;
    double        m_dIODispTol;
    long          m_nValuesShown;
    double        m_dMax[MaxValCols];
    double        m_dScale[MaxValCols];
    int           m_iPower[MaxValCols];

    long          m_InxWd;
    long          m_ChgWd;

    CMap <char*, char*, long, long> m_mapNodes;
    CArray <long,long&> m_iRefSrc;
    CArray <long,long&> m_iRefDst;

    CArray <COrdDigPt, COrdDigPt&> m_DigPts;
    long m_nDigPts;

    CRgn          m_TitleClipRgn, m_ViewClipRgn, m_ClipRgn;

    COrdInfoWnd  *m_pInfoWnd;
    COrdDigPt    *m_pLastDig; 

    CPen pnB;
    CPen pnG;
    CPen pnLg;
    CPen pnP;
    CPen pnC;
    CPen pnE;
    CPen pnA;
    CPen pnX;
    CBrush brB;
    CBrush brP;
    CBrush brC;
    CBrush brE;
    CBrush brA;
    CBrush brX;
  
    bool m_bDoProc;
    bool m_bShowTrsOnly;
    bool m_bShowIOInx;

    eTraceValue m_eITV;
    eTraceValue m_eOTV;
    long m_lCurSel;

    bool m_bShowIOAbs;
    bool m_bShowIORel;
    bool m_bShowIOVal;
    bool m_bFilterNC; 
    long m_lIValueId1;
    long m_lINValueIds;  
    long m_lOValueId1;
    long m_lONValueIds;  

    void  SetFlagsEtc();

    public:
    //{{AFX_DATA(COrdWnd)
	  enum { IDD = IDD_ORDWND };
	  CComboBox	m_ShowWhat;
	  CButton	m_FilterNC;
	  CButton	m_FullDescI;
	  CButton	m_FullDescO;
	  CButton	m_ShowProc;
	  CButton	m_ShowIOInx;
	  CSliderCtrl m_IOToleranceSlide;
    CStatic     m_ChgCount;
	  CButton	m_ShowTearsOnly;
    CStatic	m_ViewArea;
    CStatic	m_TitleArea;
    CScrollBar	m_ScrollV;
    CScrollBar	m_ScrollH;
  	//}}AFX_DATA
  
  public:
    //inline void SetSplitPos(int SplitPos) { m_iSplitPos = SplitPos; SetListHeights(); };
    //inline int GetSplitPos() { return m_iSplitPos; };
    void SetScrollBars();
    void UpdateWindowText();
    void GetOrderInfo();
    void RedrawGraph();
    void DoMeasures(CEvalOrderItem &I, CEvalOrderIOItem &IO, eTraceValue TV, long &InWide, long &OutWide);
    int  DoRefresh() { OnRefresh(); return 0; };

    //{{AFX_VIRTUAL(COrdWnd)
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
    virtual       ~COrdWnd();

    double        DrawStr(CEvalOrderItem *pItm, CEvalOrderIOItem *pIO, double C, double R, long Opts, char * fmt, ...);
    double        DrawIOInxStr(CEvalOrderItem *pItm, CEvalOrderIOItem *pIO, double X, double Y, long InxWd, bool m_bDoProc);
    double        DrawIOChgStr(CEvalOrderItem *pItm, CEvalOrderIOItem *pIO, long Id, double X, double Y, long ChgWd, eTraceValue TV, bool m_bDoProc);
    //void          AddDigPt(double X, double Y, LPTSTR Tag);
    double        DrawVert(CDC &DC, CRect &GraphR, CPen &pnG, double X, double dX);
    void          DrawAllVerts(CDC &DC, CRect &GraphR, CPen &pnG, double X);


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


    //{{AFX_MSG(COrdWnd)
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
  	afx_msg void OnShowproc();
	  afx_msg void OnShowctrl();
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg LRESULT OnCloseOrdInfo(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSelchangeShowwhat();
	//}}AFX_MSG
    //afx_msg LRESULT OnLogMsg(WPARAM wParam, LPARAM lParam);
    afx_msg void OnUpdateBtn(CCmdUI* pCmdUi);
    //afx_msg void OnMsgMenu(UINT id);
    DECLARE_MESSAGE_MAP()
  };

GDIBlkCfg  COrdWnd::GDICfg("OrderWnd");

//===========================================================================
//
//
//
//===========================================================================

IMPLEMENT_DYNCREATE(COrdWnd, CFormView)

COrdWnd::COrdWnd()//CWnd* pParent )
	: CFormView(COrdWnd::IDD),
  pnB(PS_SOLID, 0, crB),
  pnG(PS_SOLID, 0, crG),
  pnLg(PS_SOLID, 0, crLg),
  pnP(PS_SOLID, 0, crP),
  pnC(PS_SOLID, 0, crC),
  pnE(PS_SOLID, 0, crE),
  pnA(PS_SOLID, 0, crA),
  pnX(PS_SOLID, 0, crX),
  brB(crB),
  brP(crP),
  brC(crC),
  brE(crE),
  brA(crA),
  brX(crX)
  {
  //{{AFX_DATA_INIT(COrdWnd)
	//}}AFX_DATA_INIT
  m_bInited=false;

  m_Size=CPoint(1,1);
  m_Pos=CPoint(0,0);

  memset(&m_ScInfoX, 0, sizeof(m_ScInfoX));
  memset(&m_ScInfoY, 0, sizeof(m_ScInfoY));

  m_lInWide=10;
  m_lOutWide=10;
  m_lTgWide=10;
  m_InitValuesSet=false;
  m_bFirstDraw=true;

  m_dIODispTol=0;
  m_iIODispTol=0;

  m_nDigPts=0;
  m_InxWd=5;
  m_ChgWd=11;

  CDC DC;
  DC.CreateCompatibleDC(NULL);
  CDCResChk ResChk(DC);
  ScrGB.Create(&GDICfg, this, &DC, false);
  ScrGB.Attach(&DC, NULL); //  this attach detach will set up internal variables
  ScrGB.Detach();          //

  m_pInfoWnd=NULL;
  m_pLastDig=NULL;
  }

//---------------------------------------------------------------------------

COrdWnd::~COrdWnd()
  {
  }

//---------------------------------------------------------------------------

void COrdWnd::DoDataExchange(CDataExchange* pDX)
  {
	CFormView::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(COrdWnd)
	DDX_Control(pDX, IDC_SHOWWHAT, m_ShowWhat);
	DDX_Control(pDX, IDC_FILTERNC, m_FilterNC);
	DDX_Control(pDX, IDC_FULLDESCI, m_FullDescI);
	DDX_Control(pDX, IDC_FULLDESCO, m_FullDescO);
	DDX_Control(pDX, IDC_SHOWPROC, m_ShowProc);
	DDX_Control(pDX, IDC_SHOWIOINX, m_ShowIOInx);
  DDX_Control(pDX, IDC_IOSLIDE, m_IOToleranceSlide);
  DDX_Control(pDX, IDC_CHGCNTSTATIC, m_ChgCount);
	DDX_Control(pDX, IDC_SHOWTEARSONLY, m_ShowTearsOnly);
	DDX_Control(pDX, IDC_VIEWAREA, m_ViewArea);
	DDX_Control(pDX, IDC_TITLEAREA, m_TitleArea);
	DDX_Control(pDX, IDC_SCROLLBARV, m_ScrollV);
	DDX_Control(pDX, IDC_SCROLLBARH, m_ScrollH);
	//}}AFX_DATA_MAP
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(COrdWnd, CFormView)
  //{{AFX_MSG_MAP(COrdWnd)
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
	ON_BN_CLICKED(IDC_SHOWPROC, OnShowproc)
	ON_BN_CLICKED(IDC_SHOWCTRL, OnShowctrl)
  ON_WM_KEYDOWN()
  ON_MESSAGE(WMU_CLOSEORDINFO, OnCloseOrdInfo)
  ON_WM_MOVE()
  ON_WM_WINDOWPOSCHANGING()
	ON_BN_CLICKED(IDC_FULLDESCI, OnRefresh)
	ON_BN_CLICKED(IDC_FULLDESCO, OnRefresh)
	ON_BN_CLICKED(IDC_SHOWIOINX, OnRefresh)
	ON_BN_CLICKED(IDC_SHOWIOABS, OnRefresh)
	ON_BN_CLICKED(IDC_SHOWIOREL, OnRefresh)
	ON_BN_CLICKED(IDC_SHOWIOVAL, OnRefresh)
	ON_BN_CLICKED(IDC_SHOWIONONE, OnRefresh)
	ON_BN_CLICKED(IDC_SHOWTEARSONLY, OnRefresh)
	ON_BN_CLICKED(IDC_FILTERNC, OnRefresh)
	ON_CBN_SELCHANGE(IDC_SHOWWHAT, OnSelchangeShowwhat)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
      
//---------------------------------------------------------------------------
       
BOOL COrdWnd::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
  {
  if (CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext))
    {
    m_bInited=true;
    CBitmap BM;
    BM.LoadBitmap(IDB_ERRORIMGS2);
    BOOL b=m_ImgList.Create(16, 12, false, 0, 10);
    m_ImgList.Add(&BM, (CBitmap*)NULL);

    GetWindowRect(&m_InitClntRect);

    return true;
    }
  return false;
  }

//---------------------------------------------------------------------------

void COrdWnd::DoLayout() 
  {
  if (!m_bInited)
    return;

  if (!::IsWindow(m_ViewArea.m_hWnd))
    return;
  if (!::IsWindow(m_TitleArea.m_hWnd))
    return;
  
  if (!m_InitValuesSet)
    {
    m_ShowWhat.AddString("None");
    m_ShowWhat.AddString("Values");
    m_ShowWhat.AddString("Abs Chgs");
    m_ShowWhat.AddString("Rel Chgs");
    m_ShowWhat.AddString("Max Abs Chgs");
    m_ShowWhat.AddString("Max Rel Chgs");

    m_InitValuesSet=true;
    m_FullDescI.SetCheck(0);
    m_FullDescO.SetCheck(1);
    m_ShowProc.SetCheck(1);
    m_ShowIOInx.SetCheck(0);
    m_ShowWhat.SelectString(0,"Max Rel Chgs");
    m_ShowTearsOnly.SetCheck(0);
    }

  CRect ClntRect, TitleRect, ViewRect, ScrlHRect, ScrlVRect;

  GetClientRect(&ClntRect);
  m_TitleArea.GetWindowRect(&TitleRect);
  m_ViewArea.GetWindowRect(&ViewRect);
  m_ScrollH.GetWindowRect(&ScrlHRect);
  m_ScrollV.GetWindowRect(&ScrlVRect);

  ScreenToClient(&TitleRect);    
  ScreenToClient(&ViewRect);    
  ScreenToClient(&ScrlHRect);   
  ScreenToClient(&ScrlVRect);   

  int ButtonIdsB[]= { IDC_CLOSE, IDC_DESCSTATIC,
                      IDC_FULLDESCI, IDC_FULLDESCO, 
                      IDC_REFRESH, 
                      IDC_SHOWPROC, IDC_SHOWCTRL,
                      IDC_SEQSTATIC, IDC_DESCSTATIC,
                      IDC_SHOWIOINX, 
                      IDC_SHOWWHAT, IDC_IOSLIDE,
                      IDC_CHGCNTSTATIC,
                      /*IDC_SHOWIONONE,*/ IDC_IOSTATIC,
                      IDC_SHOWTEARSONLY,
                      IDC_FILTERNC};

  const int NButtons=sizeof(ButtonIdsB)/sizeof(ButtonIdsB[0]);

  HWND       hBB[NButtons];
  CRect      BRect[NButtons];
  CRect      &ClsRect=BRect[0];
  CRect      &DscRect=BRect[1];

  for (int i=0; i<NButtons; i++)
    {
    hBB[i] = ::GetDlgItem(GetSafeHwnd(), ButtonIdsB[i]);
    ::GetWindowRect(hBB[i], &BRect[i]);
    ScreenToClient(&BRect[i]);
    }

  int LGap=TitleRect.left;
  int TGap=TitleRect.top;
  int Cx=ClntRect.Width();
  int Cy=ClntRect.Height();
  int TitleW=Cx-LGap-ScrlVRect.Width()-LGap-DscRect.Width()-LGap;
  int TitleH=TitleRect.Height();
  int ViewW=Cx-LGap-ScrlVRect.Width()-LGap-DscRect.Width()-LGap;
  int ViewH=Cy-TGap-ScrlHRect.Height()-TGap-TitleH;

  m_TitleArea.SetWindowPos(this, LGap, TGap, TitleW, TitleH, SWP_NOACTIVATE|SWP_NOZORDER);
  m_ViewArea.SetWindowPos(this, LGap, TGap+TitleH, ViewW, ViewH, SWP_NOACTIVATE|SWP_NOZORDER);
  
  m_ScrollV.SetWindowPos(this, LGap+ViewW, TGap+TitleH, ScrlVRect.Width(), ViewH, SWP_NOACTIVATE|SWP_NOZORDER);
  m_ScrollH.SetWindowPos(this, LGap, TGap+TitleH+ViewH, ViewW, ScrlHRect.Height(), SWP_NOACTIVATE|SWP_NOZORDER);

  CPoint ClsPos(Cx-LGap-ClsRect.Width()-LGap, Cy-TGap-ClsRect.Height()-TGap);
  CPoint WndMove(ClsPos.x-ClsRect.left, ClsPos.y-ClsRect.top);

  for (i=0; i<NButtons; i++)
    ::SetWindowPos(hBB[i], m_hWnd, WndMove.x+BRect[i].left, WndMove.y+BRect[i].top,
                   BRect[i].Width(), BRect[i].Height(), SWP_NOACTIVATE|SWP_NOZORDER);
  for (i=0; i<NButtons; i++)
    ::InvalidateRect(hBB[i], NULL, true);

  m_IOToleranceSlide.SetPageSize(1);
  m_IOToleranceSlide.SetLineSize(1);
  m_IOToleranceSlide.SetRange(0,100);
  m_IOToleranceSlide.SetTicFreq(10);
  m_IOToleranceSlide.SetPos(m_iIODispTol);
  //m_dIODispTol=0;
  }

//---------------------------------------------------------------------------

void COrdWnd::OnSize(UINT nType, int cx, int cy) 
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

static void CountIO(CEvalOrderIOItem &F, bool ShowTrsOnly, long & NIn, long & NOut, long & NTIn, long & NTOut)
  {
  if (F.m_bIn)
    {
    NIn++;
    if (!ShowTrsOnly || F.m_bTear)
      NTIn++;
    }
  else if (F.m_bOut)
    {
    NOut++;
    if (!ShowTrsOnly || F.m_bTear)
      NTOut++;
    }
  }

//---------------------------------------------------------------------------

const long Ignore = -99999;

static bool SkipInput(CEvalOrderIOItem &C, bool ShowTrsOnly, bool TestCtrlOrd, long iOrd)
  {
  bool Skip=(!C.m_bIn || 
             ((TestCtrlOrd ? C.m_lRmtCtrlOrd:C.m_lRmtProcOrd)==iOrd) || 
             (C.m_bIn && C.m_bOut) || 
             (ShowTrsOnly && !C.m_bTear)
            );
//  dbgpln(" %sIn  %s%s%s %5i %5i %s",
//    Skip?"Skip":"    ",
//    C.m_bIn?"In":"  ", C.m_bOut?"Out":"   ", C.m_bTear?"Tear":"    ", 
//    C.m_lRmtProcOrd, C.m_lRmtCtrlOrd, C.m_sRmt());
  return Skip;
  }

//---------------------------------------------------------------------------

static bool SkipOutput(CEvalOrderIOItem &C, bool ShowTrsOnly, bool TestCtrlOrd, bool SkipIfNext, long iOrd)
  {
  bool Skip=((C.m_bIn && !(C.m_bIn && C.m_bOut)) || 
             (SkipIfNext && ((TestCtrlOrd ? C.m_lRmtCtrlOrd:C.m_lRmtProcOrd)==iOrd)) || 
             (ShowTrsOnly && !C.m_bTear)
            );
//  dbgpln(" %sOut %s%s%s %5i %5i %s",
//    Skip?"Skip":"    ",
//    C.m_bIn?"In":"  ", C.m_bOut?"Out":"   ", C.m_bTear?"Tear":"    ", 
//    C.m_lRmtProcOrd, C.m_lRmtCtrlOrd, C.m_sRmt());
  return Skip;
  }

//---------------------------------------------------------------------------

void COrdWnd::DoMeasures(CEvalOrderItem &I, CEvalOrderIOItem &IO, eTraceValue TV, long &InWide, long &OutWide)
  {                     
  if (IO.m_bIn)
    InWide=Max(InWide, (long)strlen(IO.m_sRmtDesc()));
  if (IO.m_bOut)
    OutWide=Max(OutWide, (long)strlen(IO.m_sRmtDesc()));
  IO.m_dMax=0;
  for (int i=0; i<IO.m_Trc.GetSize(); i++)
    {
    CTraceItem &T=IO.m_Trc[i];
    double Val=T.CalcValue(TV);
    if (T.Id()>=0 && Valid(Val))
      {
      double aVal=fabs(Val);
      if (aVal>=m_dIODispTol)
        m_nValuesShown++;
      long ii=T.Id();
      //m_dMax[ii]=Max(m_dMax[ii], aVal);     // Max for Channel
      //m_dMax[0]=Max(m_dMax[0], aVal);       // Max for all Channels
      //I.m_dMax=Max(I.m_dMax, aVal);         // Max for Item
      //IO.m_dMax=Max(IO.m_dMax, aVal);       // Max for IO Item
      
      if (aVal>fabs(m_dMax[ii]))              // Max for Channel
        m_dMax[ii]=Val; 
      if (aVal>fabs(m_dMax[0]))               // Max for all Channels
        m_dMax[0]=Val; 
      if (aVal>fabs(I.m_dMax))                // Max for Item
        I.m_dMax=Val; 
      if (aVal>fabs(IO.m_dMax))               // Max for IO Item
        IO.m_dMax=Val; 
      }
    }
  }

//---------------------------------------------------------------------------

void COrdWnd::SetFlagsEtc()
  {
  // Id 0 = MaxChg in IO
  //    1 = MeasVar
  //    2 = Qm
  //    3 = T
  //    4 = P
  //    5 = Sf
  //    6 = Lf
  struct tInfo { CurSels CS; long IId1, INIds; eTraceValue m_eITV; long OId1, ONIds; eTraceValue m_eOTV; bool FiltOK; };
  static const tInfo PInfo[] =
    {
      { CS_None,    0, 0, TV_None,   0, 0, TV_None,    false},
      { CS_Val,     0, 0, TV_Value,  1, 6, TV_Value,   false},
      { CS_Abs,     0, 1, TV_AbsChg, 1, 6, TV_AbsChg,  false},
      { CS_Rel,     0, 1, TV_RelChg, 1, 6, TV_RelChg,  true },
      { CS_MaxAbs,  0, 1, TV_AbsChg, 0, 1, TV_AbsChg,  false},
      { CS_MaxRel,  0, 1, TV_RelChg, 0, 1, TV_RelChg,  true },
    };                    
  static const tInfo CInfo[] =
    {                     
      { CS_None,    0, 0, TV_None,   0, 0, TV_None,    false},
      { CS_Val,     1, 1, TV_Value,  1, 1, TV_Value,   false},
      { CS_Abs,     1, 1, TV_AbsChg, 1, 1, TV_AbsChg,  false},
      { CS_Rel,     1, 1, TV_RelChg, 1, 1, TV_RelChg,  true },
      { CS_MaxAbs,  0, 1, TV_AbsChg, 0, 1, TV_AbsChg,  false},
      { CS_MaxRel,  0, 1, TV_RelChg, 0, 1, TV_RelChg,  true },
    };

  m_lCurSel=m_ShowWhat.GetCurSel();
  const tInfo &t=m_bDoProc ? PInfo[m_lCurSel] : CInfo[m_lCurSel];
  ASSERT(m_lCurSel==t.CS);
  
  m_bDoProc       = m_ShowProc.GetCheck()==BST_CHECKED;  
  m_bShowTrsOnly  = m_ShowTearsOnly.GetCheck()==BST_CHECKED;
  m_bShowIOInx    = m_ShowIOInx.GetCheck()==BST_CHECKED;
  m_bShowIOAbs    = m_lCurSel==CS_Abs;
  m_bShowIORel    = m_lCurSel==CS_Rel;
  m_bShowIOVal    = m_lCurSel==CS_Val;
  m_bFilterNC     = t.FiltOK && (m_FilterNC.GetCheck()==BST_CHECKED);
  m_eITV          = t.m_eITV;
  m_lIValueId1     = t.IId1;
  m_lINValueIds    = t.INIds;
  m_eOTV          = t.m_eOTV;
  m_lOValueId1     = t.OId1;
  m_lONValueIds    = t.ONIds;
  }

//---------------------------------------------------------------------------

const double YStart=0.5;
const double XStart=1;
const double XGap=5;
const double YGap=0.3;
const long iPDigs=4;

void COrdWnd::GetOrderInfo()
  {
  if (gs_pPrj==NULL)
    return;
  if (gs_pPrj->m_pFlwLib==NULL)
    return;
  
  CloseInfo();

  CWaitCursor Wait;

  SetFlagsEtc();
  //bool DoProc=m_ShowProc.GetCheck()!=0;  
  //bool ShowTrsOnly=m_ShowTearsOnly.GetCheck()==BST_CHECKED;
  //bool ShowIOInx=m_ShowIOInx.GetCheck()==BST_CHECKED;
  ////bool ShowIOAbs=m_ShowWhat.GetCurSel()==CS_Abs;
  ////bool ShowIORel=m_ShowWhat.GetCurSel()==CS_Rel;
  ////bool ShowIOVal=m_ShowWhat.GetCurSel()==CS_Val;
  ////long NValueIds=(ShowIOAbs || ShowIORel || ShowIOVal) ? (DoProc ? 4 : 1):0;
  //
  m_InxWd=5;
  m_ChgWd=11;
  //
  //eTraceValue TV=TV_None;
  //long NValueIds=0;
  //long CurSel=m_ShowWhat.GetCurSel();
  //switch (CurSel)
  //  {
  //  case CS_Abs:    TV=TV_AbsChg;  NValueIds=(DoProc ? 4 : 1);  break;
  //  case CS_Rel:    TV=TV_RelChg;  NValueIds=(DoProc ? 4 : 1);  break;
  //  case CS_Val:    TV=TV_Value;   NValueIds=(DoProc ? 4 : 1);  break;
  //  case CS_MaxRel: TV=TV_RelChg;  NValueIds=(DoProc ? 1 : 1);  break;
  //  case CS_MaxAbs: TV=TV_AbsChg;  NValueIds=(DoProc ? 1 : 1);  break;
  //  }

  long iRet=gs_pPrj->m_pFlwLib->FE_GetEvalOrder(m_ShowProc.GetCheck()==0, 
                                              m_FullDescI.GetCheck()!=0, 
                                              m_FullDescO.GetCheck()!=0, 
                                              m_eOTV, m_Info);

  m_lTgWide=10;
  m_lInWide=10;
  m_lOutWide=10;
  m_nValuesShown=0;
  
  for (int ic=0; ic<MaxValCols; ic++)
    {
    m_dMax[ic]=0;
    m_dScale[ic]=1;
    m_iPower[ic]=0;
    }

  for (int t=0; t<m_Info.GetSize(); t++)
    {
    CEvalOrderItem &I=m_Info[t];
    I.m_dMax=0;
    m_lTgWide=Max(m_lTgWide, (long)strlen(I.m_sTag()));
    for (int i=0; i<I.m_FIOs.GetSize(); i++)
      DoMeasures(I, I.m_FIOs[i], m_eOTV, m_lInWide, m_lOutWide);
    for (i=0; i<I.m_CIOs.GetSize(); i++)
      DoMeasures(I, I.m_CIOs[i], m_eOTV, m_lInWide, m_lOutWide);
    for (i=0; i<I.m_XIOs.GetSize(); i++)
      DoMeasures(I, I.m_XIOs[i], m_eOTV, m_lInWide, m_lOutWide);
    }
  
  if (0 && (m_eOTV!=TV_Value))
    {
    for (int ic=0; ic<MaxValCols; ic++)
      {
      double l=log10(GTZ(m_dMax[ic]));//-1.999;
      m_iPower[ic]=(int)(l<0 ? l-0.9999999:l);
      m_dScale[ic]=pow(10.0, m_iPower[ic]);
      }
    }

  Strng SValsShown;
  SValsShown.Set("%s:%i", m_eOTV!=TV_Value?"Chgs":"Vals", m_nValuesShown);
  m_ChgCount.SetWindowText(SValsShown());

  bool PrevHasNOIO=false;

  double Y=YStart, X=XStart;
  Y+=1;
  for (t=0; t<m_Info.GetSize(); t++)
    {
    CEvalOrderItem &I=m_Info[t];
    long NoFIOs=I.m_FIOs.GetSize();
    long NoCIOs=I.m_CIOs.GetSize();
    long NoXIOs=I.m_XIOs.GetSize();

    long NFIn=0, NFOut=0;
    long NCIn=0, NCOut=0;
    long NXIn=0, NXOut=0;
    long NTFIn=0, NTFOut=0;
    long NTCIn=0, NTCOut=0;
    long NTXIn=0, NTXOut=0;
    bool PrevInSeq=false;
    bool NextInSeq=false;
    if (m_bDoProc)
      {
      for (long i=0; i<NoFIOs; i++)
        {
        CEvalOrderIOItem &F=m_Info[t].m_FIOs[i];
        long iRmtOrd=F.m_lRmtProcOrd;
        if (F.m_bIn && iRmtOrd==I.m_lProcOrd-1)
          PrevInSeq=true;
        else if (F.m_bOut && iRmtOrd==I.m_lProcOrd+1)
          NextInSeq=true;
        }
      }
    else
      {
      for (long i=0; i<NoCIOs; i++)
        {
        CEvalOrderIOItem &C=m_Info[t].m_CIOs[i];
        long iRmtOrd=C.m_lRmtCtrlOrd;
        if (C.m_bIn && iRmtOrd==I.m_lCtrlOrd-1)
          PrevInSeq=true;
        else if (C.m_bOut && iRmtOrd==I.m_lCtrlOrd+1)
          NextInSeq=true;
        }
      for (i=0; i<NoXIOs; i++)
        {
        CEvalOrderIOItem &X=m_Info[t].m_XIOs[i];
        long iRmtOrd=X.m_lRmtCtrlOrd;
        if (X.m_bIn && iRmtOrd==I.m_lCtrlOrd-1)
          PrevInSeq=true;
        else if (X.m_bOut && iRmtOrd==I.m_lCtrlOrd+1)
          NextInSeq=true;
        }
      }
    for (long i=0; i<NoFIOs; i++)
      CountIO(m_Info[t].m_FIOs[i], m_bShowTrsOnly, NFIn, NFOut, NTFIn, NTFOut);
    for (i=0; i<NoCIOs; i++)
      CountIO(m_Info[t].m_CIOs[i], m_bShowTrsOnly, NCIn, NCOut, NTCIn, NTCOut);
    for (i=0; i<NoXIOs; i++)
      CountIO(m_Info[t].m_XIOs[i], m_bShowTrsOnly, NXIn, NXOut, NTXIn, NTXOut);

    bool ThisHasNOIO=m_bDoProc ? (NFIn==0 && NFOut==0) : (NCIn+NXIn==0 && NCOut+NXOut==0);
    if (PrevInSeq || PrevHasNOIO && ThisHasNOIO)
      {}
    else
      Y+=YGap;

    PrevHasNOIO=ThisHasNOIO;

    int NTot;
    if (m_bDoProc)
      NTot=Max(1L, Max((PrevInSeq?NTFIn-1:NTFIn)+NTCIn+NTXIn, (NextInSeq?NTFOut-1:NTFOut)+NTCOut+NTXOut));
    else
      NTot=Max(1L, Max((PrevInSeq?NTCIn-1:NTCIn)+NTXIn, (NextInSeq?NTCOut-1:NTCOut)+NTXOut));
    Y+=NTot;
    }
  if (1)
    Y+=YGap;
    
  Y*=1.5;
  m_Size.y=(long)(Y*ScrGB.RowHgt());
  m_Size.x=(long)((XStart+iPDigs+1+m_lInWide+2+m_lINValueIds*m_ChgWd+XGap+m_lTgWide+XGap+m_lONValueIds*m_ChgWd+m_lOutWide+2)*ScrGB.ColWdt());

  SetScrollBars();
  Invalidate();
  }

//---------------------------------------------------------------------------

void COrdWnd::SetScrollBars()
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

void COrdWnd::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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

void COrdWnd::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
  {
  CSliderCtrl *pSlide=dynamic_cast<CSliderCtrl *>(pScrollBar);
  if (pSlide)
    {
    switch (nSBCode)
      {
      case TB_BOTTOM        : m_iIODispTol=100; break;
      case TB_LINEDOWN      : m_iIODispTol+=1; break;
      case TB_LINEUP        : m_iIODispTol-=1; break;
      case TB_PAGEDOWN      : m_iIODispTol+=2; break;
      case TB_PAGEUP        : m_iIODispTol-=2; break;
      case TB_THUMBPOSITION : // Scroll to the absolute position. The current position is provided in nPos.
      case TB_THUMBTRACK    : m_iIODispTol=nPos; break; // Drag scroll box to specified position. The current position is provided in nPos.
      case TB_TOP           : m_iIODispTol=0; break;
      case TB_ENDTRACK: break;
      }

    m_dIODispTol= (m_iIODispTol>0) ? pow(10.0, m_iIODispTol*0.1-10) : 0;

    GetOrderInfo();
	  RedrawGraph();
    }
  else if (pScrollBar)
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

void COrdWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
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

double COrdWnd::DrawStr(CEvalOrderItem *pItm, CEvalOrderIOItem *pIO, double C, double R, long Opts, char * fmt, ...)
  {
  char buff[1024];
  va_list argptr;
  va_start(argptr,fmt);
  vsprintf(buff, fmt, argptr);
  va_end(argptr);
  int Len=strlen(buff);

  GDIBlkCfg &gbc = ScrGB.GBC();
  CDC &dc = ScrGB.DC();
  CDCResChk ResChk(dc);
  CBrush *pOldBrush=dc.GetCurrentBrush();
  CFont  *pOldFont =dc.GetCurrentFont();
  CPen   *pOldPen  =dc.GetCurrentPen();

  int EdgeW=ScrGB.EdgeWdt();
  int RowH=ScrGB.RowHgt();
  int ColW=ScrGB.ColWdt();
  int x=(int)(C*ColW) - (Opts & WithXOff ? m_Pos.x : 0);
  int y=(int)(R*RowH) - (Opts & WithYOff ? m_Pos.y : 0);

  int xr=x+ColW*Len;
  int yb=y+RowH;

  char * pTag=pIO?pIO->m_sRmtDesc():NULL;
  if (!pTag)
    pTag=pItm?pItm->m_sTag():NULL;
  if (pTag)
    {
    m_DigPts.SetSize(m_nDigPts+1, 256); 
    m_DigPts[m_nDigPts].m_Rect.SetRect(x, y, xr, yb);
    //m_DigPts[m_nDigPts].m_pTag=pTag;
    m_DigPts[m_nDigPts].m_pItm=pItm;
    m_DigPts[m_nDigPts].m_pIO=pIO;
    m_nDigPts++;
    }
  int TxtInv=0;
  CBrush *pBackBrush;
  if (ScrGB.IsPrinting())
    pBackBrush=ScrGB.pBrushTxtFore;
  else
    pBackBrush=ScrGB.pBrushTxtBack;

  flag IsButton=false;
  flag fItalic=false;

  if (Opts & WithBackGrnd)
    {
    CRect TxtBck  (x+EdgeW, y+EdgeW, xr, yb);
    dc.FillRect(&TxtBck, pBackBrush);
    }
  else if (Opts & WithBox)
    {
    POINT TxtBox[]=
      {
        {x, y}, 
        {xr, y}, 
        {xr, yb}, 
        {x, yb}, 
        {x, y}, 
      };
    CPen   *pOldPen = dc.GetCurrentPen();
    dc.SelectObject(pnLg); 
    dc.Polyline(TxtBox, 5);
    dc.SelectObject(pOldPen); 
    }

  CRect Txt  (x+EdgeW, y+EdgeW, xr+1, yb);

  if (fItalic)
    dc.SelectObject(ScrGB.pItlFont[0]); // Italic
  COLORREF TC;
  TC=gbc.crText[TxtInv];
  int OldTextColor=dc.SetTextColor(TC);

  int OldBkMode=dc.SetBkMode(TRANSPARENT);
  const int JustFlags[] = {DT_LEFT, DT_CENTER, DT_RIGHT, 0};
  int JustFlag=0;//JustFlags[Just];

  dc.DrawText(buff, strlen(buff), Txt, DT_END_ELLIPSIS|DT_SINGLELINE|DT_BOTTOM|DT_NOPREFIX|JustFlag);
  dc.SetBkMode(OldBkMode);
  dc.SetTextColor(OldTextColor);
  
  dc.SelectObject(pOldBrush);
  dc.SelectObject(pOldFont);
  dc.SelectObject(pOldPen);
  return C+Len;
  }

#define XP(X) (long(X-m_Pos.x))      
#define YP(Y) (long(Y-m_Pos.y))

inline long TLen(char * s) { return Max(0L,(long)strlen(s)); };

//---------------------------------------------------------------------------

double COrdWnd::DrawIOInxStr(CEvalOrderItem *pItm, CEvalOrderIOItem *pIO, double X, double Y, long InxWd, bool DoProc)
  {
  long l=DoProc ? pIO->m_lRmtProcOrd:pIO->m_lRmtCtrlOrd;
  if (l>=0)
    return DrawStr(pItm, pIO, X, Y, WithXYOff|WithBox, "%*i ", InxWd-1, l);
  return X+InxWd;
  }

//---------------------------------------------------------------------------

double COrdWnd::DrawIOChgStr(CEvalOrderItem *pItm, CEvalOrderIOItem *pIO, long Id, double X, double Y, long ChgWd, eTraceValue TV, bool DoProc)
  {
  double Val=dNAN;
  if (Id==0)
    {
    Val=pIO->m_dMax;
    }
  else
    {
    for (int i=0; i<pIO->m_Trc.GetSize(); i++)
      {
      if (pIO->m_Trc[i].Id()==Id)
        {
        Val=pIO->m_Trc[i].CalcValue(TV);
        break;
        }
      }
    }
  if (Valid(Val))
    {
    long Opts=WithXYOff|WithBox;
    if (TV==TV_RelChg && fabs(Val)>=m_dIODispTol)
      Opts|=WithBackGrnd;
    switch (TV)
      {
      case TV_AbsChg: Val=Val/m_dScale[Id];        break;
      case TV_RelChg: Val=100*Val/m_dScale[Id];    break;
      case TV_Value:                               break;
      }
    if (fabs(Val)<1.0e-20)
      return DrawStr(pItm, pIO, X, Y, Opts, "%*.*s ", ChgWd-1, ChgWd-1, "0");
    if (TV==TV_Value)
      return DrawStr(pItm, pIO, X, Y, Opts, ValFmt(Val), ChgWd-1, Val);
    return DrawStr(pItm, pIO, X, Y, Opts, "%*.4f ", ChgWd-1, Val);
    }
  return DrawStr(pItm, pIO, X, Y, WithXYOff|WithBox, "%*.*s ", ChgWd-1, ChgWd-1, "");
  //return X+ChgWd;
  }

//---------------------------------------------------------------------------

double COrdWnd::DrawVert(CDC &DC, CRect &GraphR, CPen &pnG, double X, double dX) 
  {
  X+=dX;
  long Ys=long(GraphR.top);
  long Ye=long(GraphR.bottom);
  long Xs=long(X*ScrGB.ColWdt());
  long Xe=long(X*ScrGB.ColWdt());
  CPen   *pOldPen = DC.GetCurrentPen();
  DC.SelectObject(pnG); 
  DC.MoveTo(XP(Xs), YP(Ys));
  DC.LineTo(XP(Xe), YP(Ye));
  DC.SelectObject(pOldPen); 
  return X;
  }

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void COrdWnd::DrawAllVerts(CDC &DC, CRect &GraphR, CPen &pnG, double X) 
  {
  return ;
  
  X=DrawVert(DC, GraphR, pnG, X, iPDigs+1);
  X=DrawVert(DC, GraphR, pnG, X, m_lInWide+2); 

  for (long id=m_lIValueId1; id<m_lIValueId1+m_lINValueIds; id++)
    X=DrawVert(DC, GraphR, pnG, X, m_ChgWd); 
  X=DrawVert(DC, GraphR, pnG, X, XGap); 
  if (m_bShowIOInx)
    X=DrawVert(DC, GraphR, pnG, X, m_InxWd); 
  X=DrawVert(DC, GraphR, pnG, X, m_lTgWide); 
  X=DrawVert(DC, GraphR, pnG, X, XGap); 

  if (m_bShowIOInx)
    X=DrawVert(DC, GraphR, pnG, X, m_InxWd); 
  for (id=m_lOValueId1; id<m_lOValueId1+m_lONValueIds; id++)
    X=DrawVert(DC, GraphR, pnG, X, m_ChgWd); 

  X=DrawVert(DC, GraphR, pnG, X, m_lOutWide+2); 
  }

//---------------------------------------------------------------------------
#define ProcLast 1

//eTraceValue InputIOValMap( eTraceValue I)
//  {
//  switch (I)
//    {
//    case CS_Abs: return CS_MaxAbs;
//    case CS_Rel: return CS_MaxRel;
//    default: return I;
//    }
//  }

void COrdWnd::OnDraw(CDC* pDC) 
  {
  if (m_bFirstDraw)
    {
    m_bFirstDraw=false;
    GetOrderInfo();
    }
  
  CDCResChk ResChk(pDC);

  pDC->SetMapMode(MM_TEXT);
  ScrGB.Attach(pDC, NULL);

  CDC &DC = *pDC;

  CFont *pOldFont=DC.GetCurrentFont();
  CGdiObject* pOldBrush = (CGdiObject*)DC.SelectObject(ScrGB.pBrushGrfBack);
  COLORREF OldBkColor = DC.SetBkColor(ScrGB.crGrfBack);
  CPen* pOldPen = DC.SelectObject(ScrGB.pPenTxtBord);

  long RowH=ScrGB.RowHgt();
  long ColW=ScrGB.ColWdt();
  long RowH2=ScrGB.RowHgt()/2;
  long ColW2=ScrGB.ColWdt()/2;
  long RowH3=ScrGB.RowHgt()/3;
  long ColW3=ScrGB.ColWdt()/3;
  
  m_nDigPts=0;
  
  SetFlagsEtc();

  bool SkipIfNext   = m_lCurSel == CS_None;//!m_bShowIOAbs && !m_bShowIORel && !m_bShowIOVal;
  bool PrevHasNOIO  = false;
  
  const long Yh=ScrGB.RowHgt();
  const long Yhh=Yh/2;
  const long Xw=ScrGB.ColWdt();
  const long Xwh=Xw/2;
  const long Xwq=Xw/4;
  enum {W_Null=0, 
        W_Norm, 
        W_Tear, 
        W_Src, 
        W_Snk, 
        W_Mask=0x0000ffff};

  double X,Y;
  CPoint Org;
  CRect GraphR;

  m_TitleArea.GetWindowRect(&GraphR);
  ScreenToClient(&GraphR);
  m_TitleClipRgn.DeleteObject();
  m_TitleClipRgn.CreateRectRgnIndirect(&GraphR);
  long Err = ExtSelectClipRgn(DC.m_hDC, (HRGN)m_TitleClipRgn.m_hObject, RGN_COPY);//AND);
  //CPoint Org = DC.GetWindowOrg();
  long tlen=4;
  long hSpc=(m_lTgWide-tlen)/2;
  long hSpc1=(m_lTgWide-tlen)-hSpc;

  Y=(GraphR.top+2.0)/ScrGB.RowHgt();//YStart;
  X=XStart;

  X=DrawStr(NULL, NULL, X, Y, WithXOff, "%*s:", iPDigs, "Seq");
  X=DrawStr(NULL, NULL, X, Y, WithXOff|WithBackGrnd, " %*.*s ", m_lInWide, m_lInWide, "Inputs");

  for (long id=m_lIValueId1; id<m_lIValueId1+m_lINValueIds; id++)
    {
    //                    TV_AbsChg, TV_RelChg, TV_Value,
    //const LPTSTR FmtS[]={"%s%s*e%i", "%%%s%s*e%i", "%s%s"};
    const LPTSTR FmtS[]={"", "%s%s", "%%%s%s", "%s%s"};
    Strng S;
    S.Set(FmtS[(long)m_eITV], (m_bShowIOVal?"":"d"), CTraceItem::IdStr(id), m_iPower[id]);
    X=DrawStr(NULL, NULL, X, Y, WithXOff|WithBackGrnd, "%*.*s ", m_ChgWd-1,m_ChgWd-1, S());
    }
  
  X+=XGap;
  if (m_bShowIOInx)
    X+=m_InxWd;
  
  X=DrawStr(NULL, NULL, X, Y, WithXOff|WithBackGrnd, "%*s%s%*.*s",hSpc,"","Node",hSpc1,hSpc1,"");
  X+=XGap;

  if (m_bShowIOInx)
    X+=m_InxWd;                                              
  
  for (id=m_lOValueId1; id<m_lOValueId1+m_lONValueIds; id++)
    {
    //                    TV_AbsChg, TV_RelChg, TV_Value,
    //const LPTSTR FmtS[]={"", "%s%s*e%i", "%%%s%s*e%i", "%s%s"};
    const LPTSTR FmtS[]={"", "%s%s", "%%%s%s", "%s%s"};
    Strng S;
    S.Set(FmtS[(long)m_eOTV], (m_bShowIOVal?"":"d"), CTraceItem::IdStr(id), m_iPower[id]);
    X=DrawStr(NULL, NULL, X, Y, WithXOff|WithBackGrnd, "%*.*s ", m_ChgWd-1,m_ChgWd-1, S());
    }

  X=DrawStr(NULL, NULL, X, Y, WithXOff|WithBackGrnd, " %-*.*s ", m_lOutWide, m_lOutWide, "Outputs");
  Y+=1;

  m_ViewArea.GetWindowRect(&GraphR);
  ScreenToClient(&GraphR);
  m_ViewClipRgn.DeleteObject();
  m_ViewClipRgn.CreateRectRgnIndirect(&GraphR);
  Err = ExtSelectClipRgn(DC.m_hDC, (HRGN)m_ViewClipRgn.m_hObject, RGN_COPY);//AND);
  Y=(GraphR.top+2.0)/ScrGB.RowHgt();//YStart;
  //Y=YStart;
  X=XStart;

  m_ClipRgn.DeleteObject();
  m_ClipRgn.CreateRectRgn(0,0,0,0);
  m_ClipRgn.CombineRgn(&m_TitleClipRgn, &m_ViewClipRgn, RGN_OR);
  
  long ItemSkipNo=0;
  long PrevItemSkipNo=0;
  bool FirstPass=true;
  for (long t=0; t<m_Info.GetSize(); t++)
    {
    CEvalOrderItem &Itm=m_Info[t];

//dbgpln("===========================");
//dbgpln("%4i %s", t, Itm.m_sTag());
    long NoFIOs=Itm.m_FIOs.GetSize();
    long NoCIOs=Itm.m_CIOs.GetSize();
    long NoXIOs=Itm.m_XIOs.GetSize();

    long NFIn=0, NFOut=0;
    long NCIn=0, NCOut=0;
    long NXIn=0, NXOut=0;
    long NTFIn=0, NTFOut=0;
    long NTCIn=0, NTCOut=0;
    long NTXIn=0, NTXOut=0;
    bool PrevInSeq=false;
    bool NextInSeq=false;
    if (m_bDoProc)
      {
      for (long i=0; i<NoFIOs; i++)
        {
        CEvalOrderIOItem &F=m_Info[t].m_FIOs[i];
        long iRmtOrd=F.m_lRmtProcOrd;
        if (F.m_bIn && iRmtOrd==Itm.m_lProcOrd-1)
          PrevInSeq=true;
        else if (F.m_bOut && iRmtOrd==Itm.m_lProcOrd+1)
          NextInSeq=true;
        }
      }
    else
      {
      for (long i=0; i<NoCIOs; i++)
        {
        CEvalOrderIOItem &C=m_Info[t].m_CIOs[i];
        long iRmtOrd=C.m_lRmtCtrlOrd;
        if (C.m_bIn && iRmtOrd==Itm.m_lCtrlOrd-1)
          PrevInSeq=true;
        else if (C.m_bOut && iRmtOrd==Itm.m_lCtrlOrd+1)
          NextInSeq=true;
        }
      for (i=0; i<NoXIOs; i++)
        {
        CEvalOrderIOItem &X=m_Info[t].m_XIOs[i];
        long iRmtOrd=X.m_lRmtCtrlOrd;
        if (X.m_bIn && iRmtOrd==Itm.m_lCtrlOrd-1)
          PrevInSeq=true;
        else if (X.m_bOut && iRmtOrd==Itm.m_lCtrlOrd+1)
          NextInSeq=true;
        }
      }

//PrevInSeq=false;
//NextInSeq=false;

    for (long i=0; i<NoFIOs; i++)
      CountIO(m_Info[t].m_FIOs[i], m_bShowTrsOnly, NFIn, NFOut, NTFIn, NTFOut);
    for (i=0; i<NoCIOs; i++)
      CountIO(m_Info[t].m_CIOs[i], m_bShowTrsOnly, NCIn, NCOut, NTCIn, NTCOut);
    for (i=0; i<NoXIOs; i++)
      CountIO(m_Info[t].m_XIOs[i], m_bShowTrsOnly, NXIn, NXOut, NTXIn, NTXOut);

    ItemSkipNo = (m_bFilterNC && (Itm.m_dMax<m_dIODispTol)) ? ItemSkipNo+1 : 0;

    bool ThisHasNOIO=m_bDoProc ? (NFIn==0 && NFOut==0) : (NCIn+NXIn==0 && NCOut+NXOut==0);
    bool DrawSep=t>0;
    if (PrevInSeq)
      DrawSep=false;
    if (PrevHasNOIO && ThisHasNOIO)
      DrawSep=false;
    if (ItemSkipNo)
      DrawSep=false;
    if ((ItemSkipNo==0) && (PrevItemSkipNo>0))
      DrawSep=true;
    
    if (DrawSep)
      {
      long Xs=long(GraphR.left);
      long Xe=long(GraphR.right);
      long Ys=long((Y+0.15)*Yh);
      long Ye=long((Y+0.15)*Yh);
      DC.SelectObject(pnG); 
      DC.MoveTo(XP(Xs), YP(Ys));
      DC.LineTo(XP(Xe), YP(Ye));
      Y+=YGap;
      }

    PrevHasNOIO=ThisHasNOIO;

    long IIn=0, IOut=0;
    long CIn=0, COut=0;
    long XIn=0, XOut=0;
    long NTot;
    bool MustShowValues=(m_eOTV!=TV_None);
    if (m_bDoProc)
      NTot=Max(1L, Max((PrevInSeq?NTFIn-1:NTFIn)+NTCIn+NTXIn, (NextInSeq&&!MustShowValues?NTFOut-1:NTFOut)+NTCOut+NTXOut));
    else
      NTot=Max(1L, Max((PrevInSeq?NTCIn-1:NTCIn)+NTXIn, (NextInSeq&&!MustShowValues&&!MustShowValues?NTCOut-1:NTCOut)+NTXOut));

    PrevItemSkipNo=ItemSkipNo;
    if (ItemSkipNo)
      continue;

    long ApproxYEnd=YP((Y+NTot)*Yh);
    long ApproxYStart=YP(Y*Yh);
    if (ApproxYEnd<0 || ApproxYStart>1000)
      {
      Y+=NTot;
      continue;
      }
    double Y0,X0;
    for (i=0; i<NTot; i++)
      {
      // Index
      X=1;
      if (FirstPass)
        DrawAllVerts(DC, GraphR, pnLg, X); 
      if (i==0)
        X=DrawStr(&Itm, NULL, X, Y, WithXYOff, "%*i:", iPDigs, m_bDoProc ? Itm.m_lProcOrd:Itm.m_lCtrlOrd);
      else 
        X+=iPDigs+1;
      
      // The inputs
      if (m_bDoProc)
        {
        //while (IIn<NoFIOs && 
        //       (!Itm.m_FIOs[IIn].m_bIn || 
        //         (Itm.m_FIOs[IIn].m_lRmtProcOrd==Itm.m_lProcOrd-1) ||
        //         (ShowTrsOnly && !Itm.m_FIOs[IIn].m_bTear)))
        while (IIn<NoFIOs && SkipInput(Itm.m_FIOs[IIn], m_bShowTrsOnly, false, Itm.m_lProcOrd-1))
          IIn++;
        }
      long What=W_Null;
#if !ProcLast
      if (1 && m_bDoProc && NFIn<1 && i==0)
        {
        What=(NFOut ? W_Src:W_Null)|EOC_Proc;
        X+=m_ChgWd*m_lINValueIds;
        if (m_bShowIOInx)
          X+=m_InxWd;
        X+=m_lInWide+2;
        }
      else if (m_bDoProc && IIn<NoFIOs)
        {
        CEvalOrderIOItem &FIO=Itm.m_FIOs[IIn];
        LPTSTR pS=FIO.m_sRmt();
        long tlen1=TLen(pS);
        long Wd=Max(0L,m_lInWide-tlen1);
        What=FIO.m_bTear ? W_Tear|EOC_Proc : W_Norm|EOC_Proc;
        X=DrawStr(&Itm, &FIO, X, Y, WithXYOff|WithBackGrnd, " %*.*s%s ", Wd,Wd, "", pS);
        for (long id=m_lIValueId1; id<m_lIValueId1+m_lINValueIds; id++)
          X=DrawIOChgStr(&Itm, &FIO, id, X, Y, m_ChgWd, m_TV, m_bDoProc);
        if (m_bShowIOInx)
          X=DrawIOInxStr(&Itm, &FIO, X, Y, m_InxWd, true);
        IIn++;
        }
      else
#endif
        {
        while (CIn<NoCIOs && SkipInput(Itm.m_CIOs[CIn], m_bShowTrsOnly, !m_bDoProc, m_bDoProc ? Ignore:Itm.m_lCtrlOrd-1))
          CIn++;

        if (CIn<NoCIOs)
          {
          CEvalOrderIOItem &CIO=Itm.m_CIOs[CIn];
          LPTSTR pS=CIO.m_sRmtDesc();
          long tlen1=TLen(pS);
          long Wd=Max(0L,m_lInWide-tlen1);
          What=CIO.m_dwType|(CIO.m_bTear ? W_Tear : W_Norm);
          X=DrawStr(&Itm, &CIO, X, Y, WithXYOff|WithBackGrnd, " %*.*s%s ", Wd,Wd, "", pS);
          for (long id=m_lIValueId1; id<m_lIValueId1+m_lINValueIds; id++)
            X=DrawIOChgStr(&Itm, &CIO, id, X, Y, m_ChgWd, m_eITV, m_bDoProc);
          if (m_bShowIOInx)
            X=DrawIOInxStr(&Itm, &CIO, X, Y, m_InxWd, false);
          CIn++;
          }
        else
          {
          while (XIn<NoXIOs && SkipInput(Itm.m_XIOs[XIn], m_bShowTrsOnly, false, Ignore))
            XIn++;
          if (XIn<NoXIOs)
            {
            CEvalOrderIOItem &XIO=Itm.m_XIOs[XIn];
            LPTSTR pS=XIO.m_sRmtDesc();
            long tlen1=TLen(pS);
            long Wd=Max(0L,m_lInWide-tlen1);
            What=XIO.m_bTear ? W_Tear|EOC_XRef:W_Norm|EOC_XRef;
            X=DrawStr(&Itm, &XIO, X, Y, WithXYOff|WithBackGrnd, " %*.*s%s ", Wd,Wd, "", pS);
            for (long id=m_lIValueId1; id<m_lIValueId1+m_lINValueIds; id++)
              X=DrawIOChgStr(&Itm, &XIO, id, X, Y, m_ChgWd, m_eITV, m_bDoProc);
            if (m_bShowIOInx)
              X=DrawIOInxStr(&Itm, &XIO, X, Y, m_InxWd, false);
            XIn++;
            }
#if !ProcLast
          else
#else
          else if (1 && m_bDoProc && NFIn<1 && i==0)
            {
            What=(NFOut ? W_Src:W_Null)|EOC_Proc;
            X+=m_ChgWd*m_lINValueIds;
            if (m_bShowIOInx)
              X+=m_InxWd;
            X+=m_lInWide+2;
            }
          else if (m_bDoProc && IIn<NoFIOs)
            {
            CEvalOrderIOItem &FIO=Itm.m_FIOs[IIn];
            LPTSTR pS=FIO.m_sRmtDesc();
            long tlen1=TLen(pS);
            long Wd=Max(0L,m_lInWide-tlen1);
            What=FIO.m_bTear ? W_Tear|EOC_Proc : W_Norm|EOC_Proc;
            X=DrawStr(&Itm, &FIO, X, Y, WithXYOff|WithBackGrnd, " %*.*s%s ", Wd,Wd, "", pS);
            for (long id=m_lIValueId1; id<m_lIValueId1+m_lINValueIds; id++)
              X=DrawIOChgStr(&Itm, &FIO, id, X, Y, m_ChgWd, m_eITV, m_bDoProc);
            if (m_bShowIOInx)
              X=DrawIOInxStr(&Itm, &FIO, X, Y, m_InxWd, true);
            IIn++;
            }
#endif
          else
            {
            X+=m_ChgWd*m_lINValueIds;
            if (m_bShowIOInx)
              X+=m_InxWd;
            X+=m_lInWide+2;
            }
          }
        }

      if (i==0)
        Y0=Y;
      if (What & W_Mask)
        {
        long Xs=long(X*Xw)+2;
        long Xe=long((X+XGap)*Xw);
        long Ys=long((Y+0.5)*Yh);
        long Ye=long((Y0+0.5)*Yh);
        long Xv=(Xs+Xe)/2;
        long IXs=Xs;
        long IXe=IXs+Xw-2;
        long IYs=Ys-Yhh/2;
        long IYe=Ys+Yhh/2;
        switch (What & EOC_Mask)
          {
          case EOC_Proc:
            DC.SelectObject(pnP); 
            DC.SelectObject(brP); 
#if ProcLast
            Xv+=Xw/2;
#endif
            break;
          case EOC_Ctrl: 
            DC.SelectObject(pnC); 
            DC.SelectObject(brC); 
            DC.Ellipse(XP(IXs), YP(IYs), XP(IXe), YP(IYe));
            Xs+=Xw;
#if !ProcLast
            Xv+=Xw/2;
#endif
            break;
          case EOC_Elec: 
            DC.SelectObject(pnE); 
            DC.SelectObject(brE); 
            DC.Ellipse(XP(IXs), YP(IYs), XP(IXe), YP(IYe));
            Xs+=Xw;
#if !ProcLast
            Xv+=Xw/2;
#endif
            break;
          case EOC_Air: 
            DC.SelectObject(pnA); 
            DC.SelectObject(brA); 
            DC.Ellipse(XP(IXs), YP(IYs), XP(IXe), YP(IYe));
            Xs+=Xw;
#if !ProcLast
            Xv+=Xw/2;
#endif
            break;
          case EOC_XRef:
            DC.SelectObject(pnX);
            DC.SelectObject(brX);
            DC.MoveTo(XP(IXs), YP(IYs));
            DC.LineTo(XP(IXs), YP(IYe));
            DC.LineTo(XP(IXe), YP(IYe));
            DC.LineTo(XP(IXe), YP(IYs));
            DC.LineTo(XP(IXs), YP(IYs));
            Xs+=Xw;
#if !ProcLast
            Xv+=Xw/2;
#endif
            break;
          }
        switch (What & W_Mask)
          {
          case W_Norm: 
            {
            DC.MoveTo(XP(Xs), YP(Ys));
            break;
            }
          case W_Tear: 
            {
            DC.MoveTo(XP(Xs), YP(Ys+Yhh-1));
            DC.LineTo(XP(Xs), YP(Ys-Yhh));
            DC.MoveTo(XP(Xs+Xwh), YP(Ys+Yhh-1));
            DC.LineTo(XP(Xs+Xwh), YP(Ys-Yhh));

            DC.MoveTo(XP(Xs+Xwh), YP(Ys));
            break;
            }
          case W_Src: 
            {
            DC.MoveTo(XP(Xs),         YP(Ys+Yhh-1));
            DC.LineTo(XP(Xs+Xwh),     YP(Ys));
            DC.LineTo(XP(Xs),         YP(Ys-Yhh));
            DC.MoveTo(XP(Xs+Xwh),     YP(Ys+Yhh-1));
            DC.LineTo(XP(Xs+Xwh+Xwh), YP(Ys));
            DC.LineTo(XP(Xs+Xwh),     YP(Ys-Yhh));
            
            DC.MoveTo(XP(Xs+Xwh), YP(Ys));
            break;
            }
          }
        // The Line
        if (0) // Diagonal
          {
          DC.LineTo(XP(Xs+Xw), YP(Ys));
          DC.LineTo(XP(Xe-Xw), YP(Ye));
          }
        else
          {
          DC.LineTo(XP(Xv), YP(Ys));
          DC.LineTo(XP(Xv), YP(Ye));
          DC.LineTo(XP(Xe-Xw), YP(Ye));
          }

        DC.SelectObject(pnB);
        DC.SelectObject(brB);
        POINT AH[]={XP(Xe),YP(Ye),XP(Xe-Xw),YP(Ye-3),XP(Xe-Xw),YP(Ye+3)};
        DC.Polygon(&AH[0], 3);
        }

      // The Node
      X+=XGap;

      if (i==0)
        {
        tlen=TLen(Itm.m_sTag());
        long hSpc=(m_lTgWide-tlen)/2;
        long hSpc1=(m_lTgWide-tlen)-hSpc;
        X=DrawStr(&Itm, NULL, X, Y, WithXYOff|WithBackGrnd, "%*s%s%*.*s",hSpc,"",Itm.m_sTag(),hSpc1,hSpc1,"");
        }
      else if (NextInSeq && (i==(NTot-1)))
        {
        long Xs=long((X+m_lTgWide/2)*Xw);
        long Xe=Xs;
        long Ys=long((Y0+1)*Yh);
        long Ye=long((Y+1)*Yh);
        DC.SelectObject(m_bDoProc ? pnP : pnC); 
        DC.MoveTo(XP(Xs), YP(Ys));
        DC.LineTo(XP(Xe), YP(Ye));
        POINT AH[]={XP(Xe),YP(Ye),XP(Xe-2),YP(Ye-Yhh),XP(Xe+2),YP(Ye-Yhh)};
        DC.Polygon(&AH[0], 3);
        X+=m_lTgWide;
        }
      else
        X+=m_lTgWide;

      // Outputs
      X0=X;
      X+=XGap;

      What=W_Null;
      if (m_bDoProc)
        {
        while (IOut<NoFIOs && SkipOutput(Itm.m_FIOs[IOut], m_bShowTrsOnly, false, SkipIfNext, Itm.m_lProcOrd+1))
          IOut++;
        }
      bool FillShape=false;
      bool SelfRef=false;
      bool GoesToNext=false;
#if !ProcLast
      if (m_bDoProc && (NFOut<1) && (i==0))
        {
        X+=m_lOutWide+1;
        What=(NFIn ? W_Snk:W_Null)|EOC_Proc;
        }
      else if (m_bDoProc && IOut<NoFIOs)
        {
        CEvalOrderIOItem &FIO=Itm.m_FIOs[IOut];
        LPTSTR pS=FIO.m_sRmt();
        long tlen1=TLen(pS);
        if (m_bShowIOInx)
          X=DrawIOInxStr(&Itm, &FIO, X, Y, m_InxWd, true);
        for (long id=m_lOValueId1; id<m_lOValueId1+m_lONValueIds; id++)
          X=DrawIOChgStr(&Itm, &FIO, id, X, Y, m_ChgWd, m_TV, m_bDoProc);
        What=FIO.m_bTear ? W_Tear|EOC_Proc : W_Norm|EOC_Proc;
        SelfRef=(FIO.m_bIn && FIO.m_bOut);
        GoesToNext=FIO.m_lRmtProcOrd==Itm.m_lProcOrd+1;
        if (!GoesToNext)
          X=DrawStr(&Itm, &FIO, X, Y, WithXYOff|(FIO.m_lRmtProcOrd!=Itm.m_lProcOrd+1?WithBackGrnd:0), " %-*s ", m_lOutWide, pS);
        IOut++;
        }
      else
#endif
        {
        while (COut<NoCIOs && SkipOutput(Itm.m_CIOs[COut], m_bShowTrsOnly, !m_bDoProc, SkipIfNext, m_bDoProc ? Ignore:Itm.m_lCtrlOrd+1))
          COut++;
        if (COut<NoCIOs)
          {
          CEvalOrderIOItem &CIO=Itm.m_CIOs[COut];
          LPTSTR pS=CIO.m_sRmtDesc();
          if (m_bShowIOInx)
            X=DrawIOInxStr(&Itm, &CIO, X, Y, m_InxWd, false);
          for (long id=m_lOValueId1; id<m_lOValueId1+m_lONValueIds; id++)
            X=DrawIOChgStr(&Itm, &CIO, id, X, Y, m_ChgWd, m_eOTV, m_bDoProc);
          What=CIO.m_dwType|(CIO.m_bTear ? W_Tear : W_Norm);
          SelfRef=(CIO.m_bIn && CIO.m_bOut);
          GoesToNext=CIO.m_lRmtCtrlOrd==Itm.m_lCtrlOrd+1;
          if (!GoesToNext)
            X=DrawStr(&Itm, &CIO, X, Y, WithXYOff|(CIO.m_lRmtCtrlOrd!=Itm.m_lCtrlOrd+1?WithBackGrnd:0), " %-*s ", m_lOutWide, pS);
          COut++;
          }
        else
          {
          while (XOut<NoXIOs && SkipOutput(Itm.m_XIOs[XOut], m_bShowTrsOnly, !m_bDoProc, SkipIfNext, m_bDoProc ? Ignore:Itm.m_lCtrlOrd+1))
            XOut++;
          if (XOut<NoXIOs)
            {
            CEvalOrderIOItem &XIO=Itm.m_XIOs[XOut];
            LPTSTR pS=XIO.m_sRmtDesc();
            if (m_bShowIOInx)
              X=DrawIOInxStr(&Itm, &XIO, X, Y, m_InxWd, false);
            for (long id=m_lOValueId1; id<m_lOValueId1+m_lONValueIds; id++)
              X=DrawIOChgStr(&Itm, &XIO, id, X, Y, m_ChgWd, m_eOTV, m_bDoProc);
            What=XIO.m_bTear ? W_Tear|EOC_XRef : W_Norm|EOC_XRef;
            SelfRef=(XIO.m_bIn && XIO.m_bOut);
            FillShape=!XIO.m_bOwner; 
            GoesToNext=Itm.m_lCtrlOrd>=0 ? (XIO.m_lRmtCtrlOrd==Itm.m_lCtrlOrd+1) : (XIO.m_lRmtProcOrd==Itm.m_lProcOrd+1);
            if (1)//TRUE(!GoesToNext)
              X=DrawStr(&Itm, &XIO, X, Y, WithXYOff|(true/*XIO.m_lRmtProcOrd!=Itm.m_lProcOrd+1*/?WithBackGrnd:0), " %-*s ", m_lOutWide, pS);
            XOut++;
            }
#if !ProcLast
          else
#else 
          else if (m_bDoProc && (NFOut<1) && (i==0))
            {
            X+=m_lOutWide+1;
            What=(NFIn ? W_Snk:W_Null)|EOC_Proc;
            }
          else if (m_bDoProc && IOut<NoFIOs)
            {
            CEvalOrderIOItem &FIO=Itm.m_FIOs[IOut];
            LPTSTR pS=FIO.m_sRmtDesc();
            long tlen1=TLen(pS);
            if (m_bShowIOInx)
              X=DrawIOInxStr(&Itm, &FIO, X, Y, m_InxWd, true);
            for (long id=m_lOValueId1; id<m_lOValueId1+m_lONValueIds; id++)
              X=DrawIOChgStr(&Itm, &FIO, id, X, Y, m_ChgWd, m_eOTV, m_bDoProc);
            What=FIO.m_bTear ? W_Tear|EOC_Proc : W_Norm|EOC_Proc;
            SelfRef=(FIO.m_bIn && FIO.m_bOut);
            GoesToNext=FIO.m_lRmtProcOrd==Itm.m_lProcOrd+1;
            if (!GoesToNext)
              X=DrawStr(&Itm, &FIO, X, Y, WithXYOff|(FIO.m_lRmtProcOrd!=Itm.m_lProcOrd+1?WithBackGrnd:0), " %-*s ", m_lOutWide, pS);
            IOut++;
            }
          else
#endif
            {
            if (m_bShowIOInx)
              X+=m_InxWd;
            for (long id=m_lOValueId1; id<m_lOValueId1+m_lONValueIds; id++)
              X+=m_ChgWd;
            X+=m_lOutWide+2;
            }
          }
        }
      
      if (What & W_Mask)
        {
        long Xs=long(X0*Xw)+2;
        long Xe=long((X0+XGap)*Xw);
        long Ys=long((Y0+0.5)*Yh);
        long Ye=long((Y+0.5)*Yh);
        long Xv=(Xs+Xe)/2;
        long IXs=Xe-Xw;
        long IXe=IXs+Xw-2;
        long IYs=Ye-Yhh/2;
        long IYe=Ye+Yhh/2;
        switch (What & EOC_Mask)
          {
          case EOC_Proc: 
            DC.SelectObject(pnP); 
            DC.SelectObject(brP); 
#if ProcLast
            Xv-=Xw/2;
#endif
            break;
          case EOC_Ctrl:
            DC.SelectObject(pnC);
            DC.SelectObject(brC);
            DC.Ellipse(XP(IXs), YP(IYs), XP(IXe), YP(IYe));
            Xe-=Xw;
#if !ProcLast
            Xv-=Xw/2;
#endif
            break;
          case EOC_Elec:
            DC.SelectObject(pnE);
            DC.SelectObject(brE);
            DC.Ellipse(XP(IXs), YP(IYs), XP(IXe), YP(IYe));
            Xe-=Xw;
#if !ProcLast
            Xv-=Xw/2;
#endif
            break;
          case EOC_Air:
            DC.SelectObject(pnA);
            DC.SelectObject(brA);
            DC.Ellipse(XP(IXs), YP(IYs), XP(IXe), YP(IYe));
            Xe-=Xw;
#if !ProcLast
            Xv-=Xw/2;
#endif
            break;
          case EOC_XRef:
            DC.SelectObject(pnX);
            DC.SelectObject(brX);
            
            if (FillShape)
              {
              CRect rct(XP(IXs), YP(IYs), XP(IXe), YP(IYe));
              DC.FillRect(&rct, &brX);
              }
            DC.MoveTo(XP(IXs), YP(IYs));
            DC.LineTo(XP(IXs), YP(IYe));
            DC.LineTo(XP(IXe), YP(IYe));
            DC.LineTo(XP(IXe), YP(IYs));
            DC.LineTo(XP(IXs), YP(IYs));
            Xe-=Xw;
#if !ProcLast
            Xv-=Xw/2;
#endif
            break;
          }

        if (GoesToNext)   
          {                  
          int w=(Xw*6)/4;     
          int h=(Yh*3)/4;
          //CRect   BRct(XP(Xs-w), YP(Ye), XP(Xs+w), YP(Ye+h));
          //CPoint  SPt(XP(Xs), YP(Ye+h));
          //CPoint  EPt(XP(Xs), YP(Ye));
          //DC.MoveTo(SPt);
          //DC.Arc(BRct, SPt, EPt);

          DC.MoveTo(XP(Xs),     YP(Ye));
          DC.LineTo(XP(Xs+w),   YP(Ye));
          DC.LineTo(XP(Xs+w),   YP(Ye+h));
          DC.LineTo(XP(Xs+w/2), YP(Ye+h));

          //DC.SelectObject(pnB);
          //DC.SelectObject(brB);
          POINT AH[]={XP(Xs+w/2),YP(Ye+h),XP(Xs+w/2+2),YP(Ye+h-2),XP(Xs+w/2+2),YP(Ye+h+2)};
          DC.Polygon(&AH[0], 3);
          
          //DC.MoveTo(XP(Xs+Xw), YP(Ye));
          //DC.LineTo(XP(Xs+Xw), YP(Ye));
          //POINT AH[]={XP(Xs+Xw),YP(Ys),XP(Xs),YP(Ys-3),XP(Xs),YP(Ys+3)};
          
          }
        else
          {
          // The Line
          DC.MoveTo(XP(Xs), YP(Ys));
          DC.LineTo(XP(Xv), YP(Ys));
          DC.LineTo(XP(Xv), YP(Ye));

          switch (What & W_Mask)
            {
            case W_Norm: 
              {
              DC.LineTo(XP(Xe),    YP(Ye));
              break;
              }
            case W_Tear: 
              {
              DC.LineTo(XP(Xe-Xwh),   YP(Ye));

              DC.MoveTo(XP(Xe),     YP(Ye+Yhh-1));
              DC.LineTo(XP(Xe),     YP(Ye-Yhh));
              DC.MoveTo(XP(Xe-Xwh), YP(Ye+Yhh-1));
              DC.LineTo(XP(Xe-Xwh), YP(Ye-Yhh));
              break;
              }
            case W_Snk: 
              {
              DC.LineTo(XP(Xe-Xw+Xwh),YP(Ye));

              DC.MoveTo(XP(Xe-Xw),        YP(Ye+Yhh-1));
              DC.LineTo(XP(Xe-Xw+Xwh),    YP(Ye));
              DC.LineTo(XP(Xe-Xw),        YP(Ye-Yhh));
              DC.MoveTo(XP(Xe-Xw+Xwh),     YP(Ye+Yhh-1));
              DC.LineTo(XP(Xe-Xw+Xwh+Xwh), YP(Ye));
              DC.LineTo(XP(Xe-Xw+Xwh),     YP(Ye-Yhh));
              break;
              }
            }
          if (SelfRef)
            {
          //DC.SelectObject(pnB);
            DC.MoveTo(XP(Xe-Xw+Xwh+Xwh), YP(Ye+2));
            DC.LineTo(XP(Xs),         YP(Ye+2));
            POINT AH[]={XP(Xs),YP(Ye+2),XP(Xs+Xw),YP(Ye+2-3),XP(Xs+Xw),YP(Ye+2+3)};
            DC.Polygon(&AH[0], 3);
            }

          DC.SelectObject(pnB);
          DC.SelectObject(brB);
          POINT AH[]={XP(Xs+Xw),YP(Ys),XP(Xs),YP(Ys-3),XP(Xs),YP(Ys+3)};
          DC.Polygon(&AH[0], 3);
          }
        }

      Y+=1;
      }
    FirstPass=false;
    }
  if (1)
    { // Final Line
    long Xs=long(GraphR.left);
    long Xe=long(GraphR.right);
    long Ys=long((Y+0.15)*Yh);
    long Ye=long((Y+0.15)*Yh);
    DC.SelectObject(pnB); 
    DC.MoveTo(XP(Xs), YP(Ys));
    DC.LineTo(XP(Xe), YP(Ye));
    Y+=YGap;
    }

  DC.SelectObject(pOldPen);
  DC.SetBkColor(OldBkColor);
    
  DC.SelectObject(pOldBrush);
  DC.SelectObject(pOldFont);
  m_TitleClipRgn.DeleteObject();
  ScrGB.Detach();
  }
	
//---------------------------------------------------------------------------

void COrdWnd::UpdateWindowText()
  {
  }

//---------------------------------------------------------------------------

void COrdWnd::OnClose() 
  {
  GetParent()->ShowWindow(SW_SHOWMINNOACTIVE);
  }

//---------------------------------------------------------------------------

void COrdWnd::RedrawGraph()
  {
  CRect GraphR;
  m_ViewArea.GetWindowRect(&GraphR);
  ScreenToClient(&GraphR);
  RedrawWindow(&GraphR);
  }

//---------------------------------------------------------------------------

void COrdWnd::OnRefresh() 
  {
  GetOrderInfo();
	RedrawGraph();
  }

void COrdWnd::OnShowproc() 
  {
  GetOrderInfo();
	RedrawGraph();
  }

void COrdWnd::OnShowctrl() 
  {
  GetOrderInfo();
	RedrawGraph();
  }

//void COrdWnd::OnFulldesc() 
//  {
//  GetOrderInfo();
//	RedrawGraph();
//  }

//---------------------------------------------------------------------------

void COrdWnd::OnUpdateBtn(CCmdUI* pCmdUi)
  {
  //CCustomListCtrl &MsgCtrl = *(CCustomListCtrl*)GetDlgItem(IDC_MSGLIST);
  //CCustomListCtrl &CndCtrl = *(CCustomListCtrl*)GetDlgItem(IDC_CONDLIST);

  pCmdUi->Enable(true);//MsgCtrl.GetNextItem(-1, LVNI_SELECTED)>=0 || CndCtrl.GetNextItem(-1, LVNI_SELECTED)>=0);
  }

//---------------------------------------------------------------------------

BOOL COrdWnd::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
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

void COrdWnd::OnLButtonDown(UINT nFlags, CPoint point) 
  {
  CFormView::OnLButtonDown(nFlags, point);
  
  if (nFlags & MK_LBUTTON)
    ShowInfo(point, FindDigPt(nFlags, point));
  else
    CloseInfo();
  }

//---------------------------------------------------------------------------

void COrdWnd::OnLButtonUp(UINT nFlags, CPoint point) 
  {
  CFormView::OnLButtonUp(nFlags, point);

  CloseInfo();
  }

//---------------------------------------------------------------------------

void COrdWnd::OnLButtonDblClk(UINT nFlags, CPoint point) 
  {
  CFormView::OnLButtonDblClk(nFlags, point);
  }

//---------------------------------------------------------------------------

COrdDigPt * COrdWnd::FindDigPt(UINT nFlags, CPoint point) 
  {
  for (long i=m_nDigPts-1; i>=0; i--)
    {
    if (m_DigPts[i].m_Rect.PtInRect(point))
      return &m_DigPts[i];
    }
  return NULL;
  }

//---------------------------------------------------------------------------

void COrdWnd::OnRButtonDown(UINT nFlags, CPoint point) 
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
        gs_pPrj->FindTag(pPt->Tag());
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

void COrdWnd::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
  {
  if (bActivate)
    {
    GetOrderInfo();
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

void COrdWnd::ShowInfo(CPoint point, COrdDigPt *pPt)
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
      m_pInfoWnd=new COrdInfoWnd(m_eOTV, point.x, point.y, pPt, this);
      m_pInfoWnd->SetWindowPos(&CWnd::wndTop, point.x, point.y,0,0,SWP_NOSIZE|SWP_SHOWWINDOW|SWP_NOACTIVATE);
      }
    else
      {
      m_pInfoWnd->SetInfo(m_eOTV, point.x, point.y, pPt);
      m_pInfoWnd->SetWindowPos(&CWnd::wndTop, point.x, point.y,0,0,SWP_NOSIZE|SWP_SHOWWINDOW|SWP_NOACTIVATE);
      m_pInfoWnd->RedrawWindow();//Invalidate();
      }
    }
  else 
    CloseInfo();
  m_pLastDig=pPt;
  }

//---------------------------------------------------------------------------

void COrdWnd::CloseInfo()
  {
  if (m_pInfoWnd)
    m_pInfoWnd->DestroyWindow();
  m_pInfoWnd=NULL;
  }

//---------------------------------------------------------------------------

void COrdWnd::OnMouseMove(UINT nFlags, CPoint point) 
  {
  CFormView::OnMouseMove(nFlags, point);
  if (nFlags & MK_LBUTTON)
    ShowInfo(point, FindDigPt(nFlags, point));
  else
    CloseInfo();
  }

//---------------------------------------------------------------------------

LRESULT COrdWnd::OnCloseOrdInfo(WPARAM wParam, LPARAM lParam)
  {
  CloseInfo();
  return 0;
  }

//===========================================================================
//
//
//
//===========================================================================

class COrdFrm : public CMDIChildWnd
  {
    DECLARE_DYNCREATE(COrdFrm)
    COrdFrm();           // protected constructor used by dynamic creation
  public:
    //{{AFX_VIRTUAL(COrdFrm)
    //}}AFX_VIRTUAL
  protected:
    virtual ~COrdFrm();
    //{{AFX_MSG(COrdFrm)
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnClose();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  };

//---------------------------------------------------------------------------

IMPLEMENT_DYNCREATE(COrdFrm, CMDIChildWnd)

COrdFrm::COrdFrm()
  {
  }

//---------------------------------------------------------------------------

COrdFrm::~COrdFrm()
  {
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(COrdFrm, CMDIChildWnd)
  //{{AFX_MSG_MAP(COrdFrm)
  ON_WM_SETFOCUS()
  ON_WM_CLOSE()
  ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

void COrdFrm::OnSetFocus(CWnd* pOldWnd) 
  {
  COrdWnd* pMsg = (COrdWnd*)GetTopWindow();
  if (pMsg)
    {
    pMsg->UpdateData(false);
    pMsg->DoLayout();
    //pMsg->UpdateDialogControls(pMsg, false);
    }
  CMDIChildWnd::OnSetFocus(pOldWnd);
  }

//---------------------------------------------------------------------------

void COrdFrm::OnClose() 
  {
  //CMDIChildWnd::OnClose(); Do NOT call base, we are not allowed to close this window!
  }

//---------------------------------------------------------------------------

void COrdFrm::OnSize(UINT nType, int cx, int cy) 
  {
  CMDIChildWnd::OnSize(nType, cx, cy);
  if (nType==SIZE_MAXIMIZED)
    CWindowLists::SetWndMaxMode(true);
  else if (nType==SIZE_MINIMIZED || nType==SIZE_RESTORED)
    CWindowLists::SetWndMaxMode(false);


  //COrdWnd *MW=dynamic_cast<COrdWnd*>(GetTopWindow());
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

void COrdWnd::OnSelchangeShowwhat() 
  {
  bool Enable = (m_ShowWhat.GetCurSel()==CS_Rel) || (m_ShowWhat.GetCurSel()==CS_MaxRel);
  m_FilterNC.EnableWindow(Enable);
  m_IOToleranceSlide.EnableWindow(Enable);

  OnRefresh();	
  }

void COrdWnd::OnInitialUpdate() 
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

void COrdWindow::Register()
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
  CC.m_pNewViewClass=RUNTIME_CLASS(COrdWnd);
  s_pOrdFrm = new COrdFrm;
  hIcon = ScdApp()->LoadIcon(IDI_ORDWND);
  ClassName = AfxRegisterWndClass(CS_DBLCLKS|CS_NOCLOSE, 0, 0, hIcon);
  if (s_pOrdFrm->Create(ClassName.GetBuffer(0), 
                        "Order of Evaluation", WS_CHILD | WS_OVERLAPPED | 
                        WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | 
                        WS_MINIMIZEBOX | WS_MAXIMIZEBOX, 
                        Rect, NULL, &CC))
    {
    s_pOrdWnd = (COrdWnd*)s_pOrdFrm->GetTopWindow();
//    RegisterMsgWnd(s_pOrdWnd->m_hWnd);
    s_pOrdFrm->ShowWindow(SW_MINIMIZE);
    }
  }

//---------------------------------------------------------------------------

int COrdWindow::Show(flag DoBringToTop)
  {
  if (s_pOrdFrm && DoBringToTop && (s_pOrdFrm->GetFocus()!=s_pOrdFrm))
    {
    s_pOrdFrm->MDIActivate(); //bring the window to the front
    if (s_pOrdFrm->IsIconic())
      return s_pOrdFrm->ShowWindow(SW_RESTORE);
    }

  return 0;
  };

//---------------------------------------------------------------------------

int COrdWindow::Refresh()
  {
  if (s_pOrdFrm && s_pOrdWnd)
    {
    if (!s_pOrdFrm->IsIconic())
      return s_pOrdWnd->DoRefresh();
    }
  return 0;
  };

//---------------------------------------------------------------------------

//int COrdWindow::Hide()          { return s_pOrdFrm->ShowWindow(SW_HIDE); };
//int COrdWindow::Reduce()        { return s_pOrdFrm->ShowWindow(SW_SHOWMINNOACTIVE); };

CFormView* COrdWindow::GetWnd() { return s_pOrdWnd; };
CMDIChildWnd* COrdWindow::GetFrame() { return s_pOrdFrm; };

//===========================================================================
//
//
//
//===========================================================================

