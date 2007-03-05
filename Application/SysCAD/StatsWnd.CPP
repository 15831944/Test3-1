//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#include "stdafx.h"

#define __STATSWND_CPP

#include "sc_defs.h"
#include "resource.h"
#include "statswnd.h"
#include "project.h"
#include "executiv.h"
#include "syscad.h"
#include "accnode.h"
#include "flwnode.h"

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
// CStatsInfoWnd dialog

class CStatsDigPt
  {
  public:
    //long    m_Xl, m_Yt;
    //long    m_Xr, m_Yb;
    CRect  m_Rect;
    CEvalStatsItem   *m_pItm;
    //CEvalOrderIOItem *m_pIO;
    //LPTSTR  m_pTag;
    CStatsDigPt()
      {
      m_pItm=NULL;
      //m_pIO=NULL;
      };

    LPTSTR Tag()
      {
      //if (m_pIO && m_pIO->m_sRmt())
      //  return m_pIO->m_sRmt();
      //if (m_pItm && m_pItm->m_sTag())
      //  return m_pItm->m_sTag();
      return NULL;
      }

  };

class CStatsInfoWnd : public CDialog
{
// Construction
public:
	CStatsInfoWnd(eTraceValue TV, long X, long Y, CStatsDigPt *pDigPt, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CStatsInfoWnd)
	enum { IDD = IDD_STATSINFO };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

  void SetInfo(eTraceValue TV, long X, long Y, CStatsDigPt *pDigPt);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStatsInfoWnd)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL


// Implementation

  protected:
  
  CPoint            m_Off;
  CStatsDigPt        *m_pDigPt;
  CEvalStatsItem   *m_pItm;
  //CEvalOrderIOItem *m_pIO;
  eTraceValue       m_TV; 
  bool              m_bAllowMove;

  long              m_TotWide, m_TotDeep;

  CPen              pnLg;

  void DrawLine(CDC &DC, CPen &pnG, double X1, double Y1, double X2, double Y2);

	// Generated message map functions
	//{{AFX_MSG(CStatsInfoWnd)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CStatsInfoWnd dialog


CStatsInfoWnd::CStatsInfoWnd(eTraceValue TV, long X, long Y, CStatsDigPt *pDigPt, CWnd* pParent /*=NULL*/)
	: CDialog(CStatsInfoWnd::IDD, pParent),
  pnLg(PS_SOLID, 0, crLg)
  {
   
  //{{AFX_DATA_INIT(CStatsInfoWnd)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
  
  m_bAllowMove=true;
  Create(CStatsInfoWnd::IDD, pParent);
  SetInfo(TV, X, Y, pDigPt);
//  SetWindowText(pDigPt->Tag());
  }


void CStatsInfoWnd::SetInfo(eTraceValue TV, long X, long Y, CStatsDigPt *pDigPt)
  {
  char S[1024];
  S[0]=0;

  m_TV=TV;
  m_Off.x=X;
  m_Off.y=Y;
  m_pDigPt=pDigPt;
  m_pItm=pDigPt->m_pItm;
  //m_pIO=pDigPt->m_pIO;
  
  //if (m_pIO)
  //  {
  //  if (m_pIO->m_lRmtCtrlOrd>=0)
  //    sprintf(S, "C:%i) %s", m_pIO->m_lRmtCtrlOrd, m_pDigPt->Tag());
  //  else
  //    sprintf(S, "P:%i) %s", m_pIO->m_lRmtProcOrd, m_pDigPt->Tag());
  //  }
  //else
  //  {
  //  if (m_pItm->m_lCtrlOrd>=0)
  //    sprintf(S, "C:%i) %s", m_pItm->m_lCtrlOrd, m_pDigPt->Tag());
  //  else
  //    sprintf(S, "P:%i) %s", m_pItm->m_lProcOrd, m_pDigPt->Tag());
  //  }
  SetWindowText(S);
  };

void CStatsInfoWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStatsInfoWnd)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStatsInfoWnd, CDialog)
	//{{AFX_MSG_MAP(CStatsInfoWnd)
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStatsInfoWnd message handlers

void CStatsInfoWnd::OnMouseMove(UINT nFlags, CPoint point) 
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

void CStatsInfoWnd::DrawLine(CDC &DC, CPen &pnG, double X1, double Y1, double X2, double Y2) 
  {
  CPen   *pOldPen = DC.GetCurrentPen();
  DC.SelectObject(pnG); 
  DC.MoveTo((int)X1, (int)Y1);
  DC.LineTo((int)X2, (int)Y2);
  DC.SelectObject(pOldPen); 
  }

void CStatsInfoWnd::OnPaint() 
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

  CStatsDigPt & DP=*m_pDigPt;
  LPTSTR pTag=DP.Tag();;
  
  m_TotWide=100;
  m_TotDeep=30;

  long ItWd=3;
  long TagWd=7;
  long ColWd=10;
  long HeadDeep= 1;

  //if (m_pIO)
  //  {
  //  CTraceArray &TA=m_pIO->m_Trc; 
  //
  //  m_TotWide=((TagWd+1)+(ItWd+1)+(ColWd+1)*TA.GetSize())*BlkW+1;
  //  m_TotDeep=(1+1+1+1+0.5);
  //  if (m_TV!=TV_Value)
  //    m_TotDeep+=0.5;
  //  m_TotDeep+=MaxChngTrcVals-((m_TV==TV_Value)?1:0);
  //  m_TotDeep*=BlkH;
  //  m_TotDeep+=HeadDeep+1;
  //  }  

  CRect WRct;
  GetWindowRect(&WRct);

  bool DoMove=false;
  if (m_bAllowMove)
    {
    if (m_TotWide>ClientRct.right || m_TotDeep>ClientRct.bottom)// || m_TotDeep<ClientRct.bottom)
    //if (m_TotWide!=ClientRct.right || m_TotDeep!=ClientRct.bottom)
      {
      WRct.right+=m_TotWide-ClientRct.right;
      WRct.bottom+=m_TotDeep-ClientRct.bottom;
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
    //char S[1024];//, S1[1024];

    //if (DP.m_pItm->m_lCtrlOrd>=0)
    //  sprintf(S, "C:%5i) %s", DP.m_pItm->m_lCtrlOrd, pTag);
    //else
    //  sprintf(S, "P:%5i) %s", DP.m_pItm->m_lProcOrd, pTag);
    //
    //dc.DrawText(S, strlen(S), Rct, DT_TOP|DT_NOPREFIX|DT_LEFT); 
    //Rct.OffsetRect(0, BlkH);
    
    //if (m_pIO)
    //  {
    //  CTraceArray &TA=m_pIO->m_Trc; 
    //  char * pS=S;
    //  pS=S+sprintf(S, "%-*.*s ", TagWd,TagWd,"Tag");
    //  long x=ClientRct.left+(pS-S)*BlkW-BlkW/2;
    //  DrawLine(dc, pnLg, x, ClientRct.top, x, ClientRct.bottom);
    //
    //  pS+=sprintf(pS, "%*.*s ", ItWd,ItWd,"It");
    //  
    //  for (int j=0; j<TA.GetSize(); j++)
    //    {
    //    long x=ClientRct.left+(pS-S)*BlkW-BlkW/2;
    //    DrawLine(dc, pnLg, x, ClientRct.top, x, ClientRct.bottom);
    //    pS+=sprintf(pS, "%*.*s ", ColWd, ColWd, TA[j].DescTag());
    //    }
    //  x=ClientRct.left+(pS-S)*BlkW-BlkW/2;
    //  DrawLine(dc, pnLg, x, ClientRct.top, x, ClientRct.bottom);
    //
    //  dc.DrawText(S, strlen(S), Rct, DT_TOP|DT_NOPREFIX|DT_LEFT); 
    //  Rct.OffsetRect(0, BlkH);
    //
    //  pS=S+sprintf(S, "%*.*s ",TagWd,TagWd,"");
    //  pS+=sprintf(pS, "%*.*s ", ItWd,ItWd,"");
    //  for (j=0; j<TA.GetSize(); j++)
    //    pS+=sprintf(pS, "%*.*s ", ColWd, ColWd, TA[j].CnvStr()?TA[j].CnvStr():"");
    //  dc.DrawText(S, strlen(S), Rct, DT_TOP|DT_NOPREFIX|DT_LEFT); 
    //  Rct.OffsetRect(0, BlkH);
    //  DrawLine(dc, pnLg, 0, Rct.top+BlkH/4, ClientRct.right, Rct.top+BlkH/4);
    //  Rct.OffsetRect(0, BlkH/2);
    //
    //  pS=S+sprintf(S, "%-*.*s ", TagWd,TagWd,"Value");
    //  pS+=sprintf(pS, "%*i ", ItWd, 0);
    //  for (j=0; j<TA.GetSize(); j++)
    //    {
    //    double V=TA[j].Val(0);
    //    if (Valid(V))
    //      pS+=sprintf(pS, ValFmt(V), ColWd, V);
    //    else if (IsQuietNAN(V))
    //      pS+=sprintf(pS, "%*.*s ", ColWd, ColWd, ".");
    //    else
    //      pS+=sprintf(pS, "%*.*s ", ColWd, ColWd, "*");
    //    }
    //  dc.DrawText(S, strlen(S), Rct, DT_TOP|DT_NOPREFIX|DT_LEFT); 
    //  Rct.OffsetRect(0, BlkH);
    //  if (m_TV!=TV_Value)
    //    {
    //    DrawLine(dc, pnLg, 0, Rct.top+BlkH/4, ClientRct.right, Rct.top+BlkH/4);
    //    Rct.OffsetRect(0, BlkH/2);
    //    }
    //
    //  if (0 && (m_TV==TV_RelChg))
    //    {
    //    pS=S+sprintf(S, "%-*.*s ", TagWd,TagWd,"");
    //    pS=S+sprintf(pS, "%*.*s ", ItWd, ItWd, "");
    //    for (j=0; j<TA.GetSize(); j++)
    //      pS+=sprintf(pS, "%*.*s ", ColWd, ColWd, "%");
    //    dc.DrawText(S, strlen(S), Rct, DT_TOP|DT_NOPREFIX|DT_LEFT); 
    //    Rct.OffsetRect(0, BlkH);
    //    }
    //
    //  LPCTSTR pPerc=(m_TV==TV_RelChg)?"%":" ";
    //  double Scl=(m_TV==TV_RelChg) ? 100:1;
    //  long Iter1=(m_TV==TV_Value)?1:0;
    //  //long IterOff=(m_TV==TV_Value)?0:-1;
    //  for (int i=Iter1; i<MaxChngTrcVals; i++)
    //    {
    //    static LPCTSTR RowHd[]= { "TV_None", "dV(Abs)", "dV(%)", "Value" };
    //
    //    pS=S+sprintf(S, "%-*.*s ", TagWd,TagWd,RowHd[m_TV]);
    //    pS+=sprintf(pS, "%*i ", ItWd, -i);
    //    for (int j=0; j<TA.GetSize(); j++)
    //      {
    //      double V=dQuietNAN;
    //      for (int ii=0; ii<TA[j].NVals(); ii++)
    //        if (TA[j].IterNo(ii)==(CTraceItem::CurrentIter()-i))
    //           {
    //           V=TA[j].CalcValue(m_TV, ii);
    //           break;
    //           }
    //      if (Valid(V))
    //        {
    //        if (m_TV==TV_Value)
    //          pS+=sprintf(pS, ValFmt(V), ColWd, V);
    //        else if (fabs(V)>1.0e-20)
    //          pS+=sprintf(pS, "%+*.5f ", ColWd, V*Scl);
    //        else
    //          pS+=sprintf(pS, "%*.*s ", ColWd, ColWd, "0");
    //        }
    //      else if (IsQuietNAN(V))
    //        pS+=sprintf(pS, "%*.*s ", ColWd, ColWd, ".");
    //      else
    //        pS+=sprintf(pS, "%*.*s ", ColWd, ColWd, "*");
    //      }
    //    dc.DrawText(S, strlen(S), Rct, DT_TOP|DT_NOPREFIX|DT_LEFT); 
    //    Rct.OffsetRect(0, BlkH);
    //    }
    //  }
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

static struct tColHeads
  {
  bool      m_bShowIt;
  eFNdSWs   m_eWhat;
  LPCSTR    m_Txt;
  } ColHeads[]=
  {
    { false,  FSW_Null,                   "Null"            },                  
    { true,   FSW_EvalJoinPressures,      "JoinPress"       },     
    { true,   FSW_EvalJoinFlows,          "JoinFlws"        },         
    { true,   FSW_EvalFlowEquations,      "FlowEqn"         },     
    { true,   FSW_EvalState,              "State"           },             
    { false,  FSW_ConfigureJoins,         "CfgJoins"        },        
    { false,  FSW_PostConfigureJoins,     "PostCfgJ"        },    
    { true,   FSW_StartStep,              "StartStep"       },             
    { true,   FSW_EvalSteadyState,        "SteadyState"     },       
    { true,   FSW_EvalPressureSens,       "PressSens"       },      
    { false,  FSW_EvalProductsInit,       "ProductsInit"    },      
    { true,   FSW_EvalProducts,           "Products"        },          
    { true,   FSW_EvalDerivs,             "Derivs"          },            
    { true,   FSW_EvalIntegral,           "Integral"        },          
    { true,   FSW_ODEOperate,             "ODEOperate"      },            
    { true,   FSW_EvalDiscrete,           "Discrete"        },          
    { true,   FSW_EvalAudit,              "Audit"           },          
    { false,  FSW_BuildCtrlNdList,        "BuildCtrlNdList" },  
    { false,  FSW_EvalCtrlInitialise,     "CtrlInit"        },    
    { true,   FSW_EvalCtrlActions,        "CtrlActs"        },       
    { true,   FSW_EvalCtrlStrategy,       "CtrlStrat"       },      
    { false,  FSW_EvalCtrlTerminate,      "CtrlTerm"        },     
    { true,   FSW_EvalStatistics,         "Statistics"      },        
    { true,   FSW_PBEvalMakeUpAvail,      "PBMakeUpAvl"     },     
    { true,   FSW_PBEvalMakeUpReqd,       "PBMakeUpRqd"     },      
    { true,   FSW_PBEvalConvergence,      "PBConverge"      },     
    { true,   FSW_EvalMsAGData,           "EvalMsAGData"    },
    { false,  FSW_Max,                    "Max"             },                    
  };


//class CStatsColumn
//  {
//  public:
//    int m_iFirstTag, m_iLastTag;
//    int m_iWidth;
//  };
//
//class CStatsColumns : public CArray <CStatsColumn, CStatsColumn&> {};

class CStatsWnd;
class CStatsFrm;

static CStatsWnd *s_pStatsWnd=NULL;
static CStatsFrm *s_pStatsFrm=NULL;

// must match the strings in the Control; 
enum CurSels { CS_None, CS_Val, CS_Abs, CS_Rel, CS_MaxAbs, CS_MaxRel };

const long WithXOff     = 0x00000001;
const long WithYOff     = 0x00000002;
const long WithXYOff    = 0x00000003;
const long WithBackGrnd = 0x00000004;
const long WithBox      = 0x00000008;

const long MaxValCols=5; // must expand for more values
class CStatsWnd : public CFormView
  {
  friend class CStatsFrm;
  public:
	//CStatsWnd(CWnd* pParent = NULL);   // standard constructor

  protected:
    CStatsWnd();           // protected constructor used by dynamic creation
    DECLARE_DYNCREATE(CStatsWnd)

    static GDIBlkCfg GDICfg;
    GDIBlk           ScrGB;
      
    CEvalStatsArray m_Info;
    //CEvalOrderArray CInfo;
    //CXRefStatusArray   m_Refs; //TODO : use global gs_PE.Refs!!!
    //CStatsColumns     m_Cols; 

    CPoint        m_Size;
    CPoint        m_Pos;
    long          m_lTgWide, m_lClsWide, m_lColWide;//, m_lOutWide;
    
    bool          m_bFirstDraw;
    bool          m_InitValuesSet;

    CRect         m_InitClntRect;

    SCROLLINFO    m_ScInfoX;
    SCROLLINFO    m_ScInfoY;

    CImageList    m_ImgList;           //bitmap images used in list boxes
    flag          m_bInited;

    int           m_iDispTol;
    double        m_dDispTol;
    
    //long          m_nValuesShown;
    long          m_nVisCols;
    bool          m_bColOn[FSW_Max];
    double        m_dTimeScl;

    long          m_InxWd;
    long          m_ChgWd;

    //CMap <char*, char*, long, long> m_mapNodes;
    //CArray <long,long&> m_iRefSrc;
    //CArray <long,long&> m_iRefDst;

    CArray <CStatsDigPt, CStatsDigPt&> m_DigPts;
    long m_nDigPts;

    CRgn          m_TitleClipRgn, m_ViewClipRgn, m_ClipRgn;

    CStatsInfoWnd  *m_pInfoWnd;
    CStatsDigPt    *m_pLastDig; 

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
  
    //bool m_bDoProc;
    //bool m_bShowTrsOnly;
    //bool m_bShowIOInx;

    //eTraceValue m_TV;
    long m_lCurSel;

    //bool m_bShowIOAbs;
    //bool m_bShowIORel;
    //bool m_bShowIOVal;
    //bool m_bFilterNC; 
    //long m_lIValueId1;
    //long m_lINValueIds;  
    //long m_lOValueId1;
    //long m_lONValueIds;  

    void  SetFlagsEtc();

    public:
    //{{AFX_DATA(CStatsWnd)
	enum { IDD = IDD_STATSWND };
	CStatic	m_UnitsStatic;
	CComboBox	m_ShowWhat;
	  CButton	m_FilterNC;
	  CSliderCtrl m_ToleranceSlide;
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
    void GetStatsInfo();
    void RedrawGraph();
    //void DoMeasures(CEvalStatsItem &I, CEvalOrderIOItem &IO, eTraceValue TV, long &InWide, long &OutWide);
    int  DoRefresh() { OnRefresh(); return 0; };

    //{{AFX_VIRTUAL(CStatsWnd)
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
    virtual       ~CStatsWnd();

    double        DrawStr(CEvalStatsItem *pItm, CEvalOrderIOItem *pIO, double C, double R, long Opts, char * fmt, ...);
    double        DrawVert(CDC &DC, CRect &GraphR, CPen &pnG, double X, double dX);
    void          DrawAllVerts(CDC &DC, CRect &GraphR, CPen &pnG, double X);


    CStatsDigPt *   FindDigPt(UINT nFlags, CPoint point); 
    void          ShowInfo(CPoint point, CStatsDigPt *pPt);
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


    //{{AFX_MSG(CStatsWnd)
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

GDIBlkCfg  CStatsWnd::GDICfg("OrderWnd");

//===========================================================================
//
//
//
//===========================================================================

IMPLEMENT_DYNCREATE(CStatsWnd, CFormView)

CStatsWnd::CStatsWnd()//CWnd* pParent )
	: CFormView(CStatsWnd::IDD),
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
  //{{AFX_DATA_INIT(CStatsWnd)
	//}}AFX_DATA_INIT
  m_bInited=false;

  m_Size=CPoint(1,1);
  m_Pos=CPoint(0,0);

  memset(&m_ScInfoX, 0, sizeof(m_ScInfoX));
  memset(&m_ScInfoY, 0, sizeof(m_ScInfoY));

  m_lColWide=10;
//  m_lOutWide=10;
  m_lTgWide=10;
  m_lClsWide=10;
  m_InitValuesSet=false;
  m_bFirstDraw=true;

  m_dDispTol=0;
  m_iDispTol=0;

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

  m_lCurSel=2;
  }

//---------------------------------------------------------------------------

CStatsWnd::~CStatsWnd()
  {
  }

//---------------------------------------------------------------------------

void CStatsWnd::DoDataExchange(CDataExchange* pDX)
  {
	CFormView::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CStatsWnd)
	DDX_Control(pDX, IDC_UNITSSTATIC, m_UnitsStatic);
	DDX_Control(pDX, IDC_SHOWWHAT, m_ShowWhat);
	DDX_Control(pDX, IDC_FILTERNC, m_FilterNC);
  DDX_Control(pDX, IDC_IOSLIDE, m_ToleranceSlide);
	DDX_Control(pDX, IDC_VIEWAREA, m_ViewArea);
	DDX_Control(pDX, IDC_TITLEAREA, m_TitleArea);
	DDX_Control(pDX, IDC_SCROLLBARV, m_ScrollV);
	DDX_Control(pDX, IDC_SCROLLBARH, m_ScrollH);
	//}}AFX_DATA_MAP
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CStatsWnd, CFormView)
  //{{AFX_MSG_MAP(CStatsWnd)
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
       
BOOL CStatsWnd::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
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

void CStatsWnd::DoLayout() 
  {
  if (!m_bInited)
    return;

  if (!::IsWindow(m_ViewArea.m_hWnd))
    return;
  if (!::IsWindow(m_TitleArea.m_hWnd))
    return;
  
  if (!m_InitValuesSet)
    {
    m_ShowWhat.AddString("Calls");
    m_ShowWhat.AddString("TotalTime");
    m_ShowWhat.AddString("Time/Call");

    m_InitValuesSet=true;
    m_ShowWhat.SelectString(0,"Time/Call");
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

  int ButtonIdsB[]= { IDC_CLOSE, 
                      IDC_IOSTATIC,
                      IDC_REFRESH, 
                      IDC_SHOWWHAT, IDC_IOSLIDE,
                      IDC_UNITSSTATIC,
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

  m_ToleranceSlide.SetPageSize(1);
  m_ToleranceSlide.SetLineSize(1);
  m_ToleranceSlide.SetRange(0,100);
  m_ToleranceSlide.SetTicFreq(10);
  m_ToleranceSlide.SetPos(m_iDispTol);
  //m_dDispTol=0;
  }

//---------------------------------------------------------------------------

void CStatsWnd::OnSize(UINT nType, int cx, int cy) 
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

void CStatsWnd::SetFlagsEtc()
  {
  m_lCurSel=m_ShowWhat.GetCurSel();
  //const tInfo &t=m_bDoProc ? PInfo[m_lCurSel] : CInfo[m_lCurSel];
  //ASSERT(m_lCurSel==t.CS);
  //
  //m_bDoProc       = m_ShowProc.GetCheck()==BST_CHECKED;  
  //m_bShowTrsOnly  = m_ShowTearsOnly.GetCheck()==BST_CHECKED;
  //m_bShowIOInx    = m_ShowIOInx.GetCheck()==BST_CHECKED;
  }

//---------------------------------------------------------------------------

const double YStart=0.5;
const double XStart=1;
//const double XGap=5;
const double YGap=0.3;
const long iPDigs=4;

void CStatsWnd::GetStatsInfo()
  {
  if (gs_pPrj==NULL)
    return;
  if (gs_pPrj->m_pFlwLib==NULL)
    return;
  
  CloseInfo();

  CWaitCursor Wait;

  SetFlagsEtc();
  //bool DoProc=m_ShowProc.GetCheck()!=0;  

  m_InxWd=5;
  m_ChgWd=11;

  long iRet=gs_pPrj->m_pFlwLib->FE_GetEvalStats(m_Info);

  m_lTgWide=10;
  m_lClsWide=10;
  m_lColWide=10;

  m_dTimeScl=1;
  m_nVisCols=0;
  for (int col=0; col<FSW_Max; col++)
    m_bColOn[col]=0;

  double MaxTime=0;
  for (int t=0; t<m_Info.GetSize(); t++)
    {
    CEvalStatsItem &I=m_Info[t];
    m_lTgWide=Max(m_lTgWide, (long)strlen(I.m_sTag()));
    m_lClsWide=Max(m_lClsWide, (long)strlen(I.m_sClass()));
    for (int col=0; col<FSW_Max; col++)
      {
      ASSERT(ColHeads[col].m_eWhat==col);
      if (ColHeads[col].m_bShowIt && I.m_Items[col].m_lCallCount>0)
        {
        if (!m_bColOn[col])
          {
          m_nVisCols++;
          m_bColOn[col]=1;
          }
        switch (m_lCurSel)
          {
          case 1:  MaxTime=Max(MaxTime, I.m_Items[col].m_dTimeTot); break;
          case 2:  MaxTime=Max(MaxTime, I.m_Items[col].m_dTimeTot/Max(1L, I.m_Items[col].m_lCallCount)); break;
          }
        };
      }
    }
  if (MaxTime<1)
    m_dTimeScl=1e6;
  else if (MaxTime<1000)
    m_dTimeScl=1e3;
  
  bool PrevHasNOIO=false;

  double Y=YStart, X=XStart;
  Y+=1+m_Info.GetSize()+YGap;
    
  m_Size.y=(long)(Y*ScrGB.RowHgt());
  m_Size.x=(long)((XStart+iPDigs+1+m_lTgWide+1+m_lClsWide+2+m_nVisCols*(1+m_lColWide))*ScrGB.ColWdt());
  //m_Size.x=(long)((XStart+iPDigs+1+m_lColWide+2+m_lINValueIds*m_ChgWd+XGap+m_lTgWide+XGap+m_lONValueIds*m_ChgWd+m_lOutWide+2)*ScrGB.ColWdt());



  SetScrollBars();
  Invalidate();
  }

//---------------------------------------------------------------------------

void CStatsWnd::SetScrollBars()
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

void CStatsWnd::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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

void CStatsWnd::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
  {
  CSliderCtrl *pSlide=dynamic_cast<CSliderCtrl *>(pScrollBar);
  if (pSlide)
    {
    switch (nSBCode)
      {
      case TB_BOTTOM        : m_iDispTol=100; break;
      case TB_LINEDOWN      : m_iDispTol+=1; break;
      case TB_LINEUP        : m_iDispTol-=1; break;
      case TB_PAGEDOWN      : m_iDispTol+=2; break;
      case TB_PAGEUP        : m_iDispTol-=2; break;
      case TB_THUMBPOSITION : // Scroll to the absolute position. The current position is provided in nPos.
      case TB_THUMBTRACK    : m_iDispTol=nPos; break; // Drag scroll box to specified position. The current position is provided in nPos.
      case TB_TOP           : m_iDispTol=0; break;
      case TB_ENDTRACK: break;
      }

    m_dDispTol= (m_iDispTol>0) ? pow(10.0, m_iDispTol*0.1-10) : 0;

    GetStatsInfo();
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

void CStatsWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
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

double CStatsWnd::DrawStr(CEvalStatsItem *pItm, CEvalOrderIOItem *pIO, double C, double R, long Opts, char * fmt, ...)
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

  char * pTag=pIO?pIO->m_sRmtTag():NULL;
  if (!pTag)
    pTag=pItm?pItm->m_sTag():NULL;
  if (pTag)
    {
    m_DigPts.SetSize(m_nDigPts+1, 256); 
    m_DigPts[m_nDigPts].m_Rect.SetRect(x, y, xr, yb);
    //m_DigPts[m_nDigPts].m_pTag=pTag;
    m_DigPts[m_nDigPts].m_pItm=pItm;
    //m_DigPts[m_nDigPts].m_pIO=pIO;
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
  if (0)//Marker)
    {
    POINT Mrk[]=
      {
        {x+EdgeW, y+EdgeW}, 
        {x+EdgeW+3, y+EdgeW}, 
        {x+EdgeW, y+EdgeW+3}
      };
    COLORREF MrkColor=RGB(0,128,255);
    CBrush MrkBrush(MrkColor);
    CBrush *OldBrush=dc.SelectObject(&MrkBrush);
    CPen MrkPen(PS_SOLID,0,MrkColor);
    CPen *OldPen=dc.SelectObject(&MrkPen);
    dc.Polygon(Mrk, sizeof(Mrk)/sizeof(Mrk[0]));
    dc.SelectObject(OldBrush);
    dc.SelectObject(OldPen);
    }

  //CFont * OldFont=NULL;
  if (fItalic)
    dc.SelectObject(ScrGB.pItlFont[0]); // Italic
  COLORREF TC;
  TC=gbc.crText[TxtInv];
  int OldTextColor=dc.SetTextColor(TC);

  int OldBkMode=dc.SetBkMode(TRANSPARENT);
  const int JustFlags[] = {DT_LEFT, DT_CENTER, DT_RIGHT, 0};
  int JustFlag=0;

  dc.DrawText(buff, strlen(buff), Txt, DT_END_ELLIPSIS|DT_SINGLELINE|DT_BOTTOM|DT_NOPREFIX|JustFlag);
  dc.SetBkMode(OldBkMode);
  dc.SetTextColor(OldTextColor);
  
  dc.SelectObject(pOldBrush);
  dc.SelectObject(pOldFont);
  dc.SelectObject(pOldPen);
  return C+Len;
  }

//#define XP(X) ((X)-m_Pos.x+GraphR.left)
//#define YP(Y) ((Y)-m_Pos.y+GraphR.top)

#define XP(X) (short(X-m_Pos.x))
#define YP(Y) (short(Y-m_Pos.y))

inline long TLen(char * s) { return Max(0L,(long)strlen(s)); };

//---------------------------------------------------------------------------

double CStatsWnd::DrawVert(CDC &DC, CRect &GraphR, CPen &pnG, double X, double dX) 
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

void CStatsWnd::DrawAllVerts(CDC &DC, CRect &GraphR, CPen &pnG, double X) 
  {
  return ;
  
  X=DrawVert(DC, GraphR, pnG, X, iPDigs+1);
  }

//---------------------------------------------------------------------------

void CStatsWnd::OnDraw(CDC* pDC) 
  {
  if (m_bFirstDraw)
    {
    m_bFirstDraw=false;
    GetStatsInfo();
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
  long col=0;
  m_nDigPts=0;
  
  SetFlagsEtc();

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

  Y=(GraphR.top+2.0)/ScrGB.RowHgt();
  X=XStart;
  X=XStart+(GraphR.left)/ScrGB.ColWdt();

  X=DrawStr(NULL, NULL, X, Y, WithXOff|WithBox, "%*s:", iPDigs, "Seq");
  
  X=DrawStr(NULL, NULL, X, Y, WithXOff|WithBox, " %-*.*s",m_lTgWide,m_lTgWide,"Node");
  X=DrawStr(NULL, NULL, X, Y, WithXOff|WithBox, " %-*.*s",m_lClsWide,m_lClsWide,"Class");
  for (col=0; col<FSW_Max; col++)
    if (m_bColOn[col])
      {
      ASSERT(ColHeads[col].m_eWhat==col);
      X=DrawStr(NULL, NULL, X, Y, WithXOff|WithBox, " %*.*s",m_lColWide,m_lColWide,ColHeads[col].m_Txt);
      }

  Y+=1;

  m_ViewArea.GetWindowRect(&GraphR);
  ScreenToClient(&GraphR);
  m_ViewClipRgn.DeleteObject();
  m_ViewClipRgn.CreateRectRgnIndirect(&GraphR);
  Err = ExtSelectClipRgn(DC.m_hDC, (HRGN)m_ViewClipRgn.m_hObject, RGN_COPY);
  Y=(GraphR.top+2.0)/ScrGB.RowHgt();
  X=XStart;

  m_ClipRgn.DeleteObject();
  m_ClipRgn.CreateRectRgn(0,0,0,0);
  m_ClipRgn.CombineRgn(&m_TitleClipRgn,&m_ViewClipRgn,RGN_OR);
  
  CString SS;
  switch (m_lCurSel)
    {
    case 0: SS="Counts";  break;
    case 1: 
    case 2:
      if (m_dTimeScl==1e3)
        SS="mSecs";
      else if (m_dTimeScl==1e6)
        SS="uSecs";
      else
        SS="Secs";
      break;
    }
  m_UnitsStatic.SetWindowText(SS);

  bool FirstPass=true;
  for (long t=0; t<m_Info.GetSize(); t++)
    {
    CEvalStatsItem &Itm=m_Info[t];

    bool DrawSep=false;//t>0;
    
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

    long NTot=1;

    long ApproxYEnd=YP((Y+NTot)*Yh);
    long ApproxYStart=YP(Y*Yh);
    if (ApproxYEnd<0 || ApproxYStart>1000)
      {
      Y+=NTot;
      continue;
      }

    for (int i=0; i<NTot; i++)
      {
      // Index
      X=1;
      if (FirstPass)
        DrawAllVerts(DC, GraphR, pnLg, X); 
      if (i==0)
        X=DrawStr(&Itm, NULL, X, Y, WithXYOff|WithBox, "%*i:", iPDigs, t);//m_bDoProc ? Itm.m_lProcOrd:Itm.m_lCtrlOrd);
      else 
        X+=iPDigs+1;
      
      if (i==0)
        {
        X=DrawStr(&Itm, NULL, X, Y, WithXYOff|WithBox, " %-*.*s",m_lTgWide,m_lTgWide,Itm.m_sTag());
        X=DrawStr(&Itm, NULL, X, Y, WithXYOff|WithBox, " %-*.*s",m_lClsWide,m_lClsWide,Itm.m_sClass());
        for (col=0; col<FSW_Max; col++)
          if (m_bColOn[col])
            {
            CString SS;
            CEvalStatsRec & I=Itm.m_Items[col];
            if (I.m_lCallCount>0)
              {
              switch (m_lCurSel)
                {
                case 0:
                  SS.Format("%*i", m_lColWide,I.m_lCallCount);
                  break;
                case 1:
                  SS.Format("%*.3f",m_lColWide,I.m_dTimeTot*m_dTimeScl);
                  break;
                case 2:
                  SS.Format("%*.3f",m_lColWide,I.m_dTimeTot*m_dTimeScl/Max(1L, I.m_lCallCount));
                  break;
                }
              }
            else
              SS="";
            X=DrawStr(&Itm, NULL, X, Y, WithXYOff|WithBox, " %*s", m_lColWide, (LPCSTR)SS);
            }
        }
      else
        X+=m_lTgWide;

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

void CStatsWnd::UpdateWindowText()
  {
  }

//---------------------------------------------------------------------------

void CStatsWnd::OnClose() 
  {
  GetParent()->ShowWindow(SW_SHOWMINNOACTIVE);
  }

//---------------------------------------------------------------------------

void CStatsWnd::RedrawGraph()
  {
  CRect GraphR;
  m_ViewArea.GetWindowRect(&GraphR);
  ScreenToClient(&GraphR);
  RedrawWindow(&GraphR);
  }

//---------------------------------------------------------------------------

void CStatsWnd::OnRefresh() 
  {
  GetStatsInfo();
	RedrawGraph();
  }

void CStatsWnd::OnShowproc() 
  {
  GetStatsInfo();
	RedrawGraph();
  }

void CStatsWnd::OnShowctrl() 
  {
  GetStatsInfo();
	RedrawGraph();
  }

//void CStatsWnd::OnFulldesc() 
//  {
//  GetStatsInfo();
//	RedrawGraph();
//  }

//---------------------------------------------------------------------------

void CStatsWnd::OnUpdateBtn(CCmdUI* pCmdUi)
  {
  //CCustomListCtrl &MsgCtrl = *(CCustomListCtrl*)GetDlgItem(IDC_MSGLIST);
  //CCustomListCtrl &CndCtrl = *(CCustomListCtrl*)GetDlgItem(IDC_CONDLIST);

  pCmdUi->Enable(true);//MsgCtrl.GetNextItem(-1, LVNI_SELECTED)>=0 || CndCtrl.GetNextItem(-1, LVNI_SELECTED)>=0);
  }

//---------------------------------------------------------------------------

BOOL CStatsWnd::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
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

void CStatsWnd::OnLButtonDown(UINT nFlags, CPoint point) 
  {
  CFormView::OnLButtonDown(nFlags, point);
  
  if (nFlags & MK_LBUTTON)
    ShowInfo(point, FindDigPt(nFlags, point));
  else
    CloseInfo();
  }

//---------------------------------------------------------------------------

void CStatsWnd::OnLButtonUp(UINT nFlags, CPoint point) 
  {
  CFormView::OnLButtonUp(nFlags, point);

  CloseInfo();
  }

//---------------------------------------------------------------------------

void CStatsWnd::OnLButtonDblClk(UINT nFlags, CPoint point) 
  {
  CFormView::OnLButtonDblClk(nFlags, point);
  }

//---------------------------------------------------------------------------

CStatsDigPt * CStatsWnd::FindDigPt(UINT nFlags, CPoint point) 
  {
  for (long i=m_nDigPts-1; i>=0; i--)
    {
    if (m_DigPts[i].m_Rect.PtInRect(point))
      return &m_DigPts[i];
    }
  return NULL;
  }

//---------------------------------------------------------------------------

void CStatsWnd::OnRButtonDown(UINT nFlags, CPoint point) 
  {
  UpdateData(true);

  CStatsDigPt * pPt=FindDigPt(nFlags, point);
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
        gs_AccessWnds./*gs_pAccWnd->*/AccessNode(-1, pPt->Tag());
        break;
      case IDM_SLV_FIND:
        gs_pPrj->FindTag(pPt->Tag(), NULL, NULL, -1, FTO_MoveCursor|FTO_HighliteSlow);
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

void CStatsWnd::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
  {
  if (bActivate)
    {
    GetStatsInfo();
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

void CStatsWnd::ShowInfo(CPoint point, CStatsDigPt *pPt)
  {
  if (pPt)
    {
    //CRect CRct;
    //GetClientRect(&CRct);
    //ClientToScreen(&CRct);
    //point.x+=CRct.left+15;
    //point.y+=CRct.top;
    //if (!m_pInfoWnd)
    //  {
    //  m_pInfoWnd=new CStatsInfoWnd(m_TV, point.x, point.y, pPt, this);//, Pinned, pParent);
    //  m_pInfoWnd->SetWindowPos(&CWnd::wndTop, point.x, point.y,0,0,SWP_NOSIZE|SWP_SHOWWINDOW|SWP_NOACTIVATE);
    //  }
    //else
    //  {
    //  m_pInfoWnd->SetInfo(m_TV, point.x, point.y, pPt);
    //  m_pInfoWnd->SetWindowPos(&CWnd::wndTop, point.x, point.y,0,0,SWP_NOSIZE|SWP_SHOWWINDOW|SWP_NOACTIVATE);
    //  m_pInfoWnd->RedrawWindow();//Invalidate();
    //  }
    }
  else 
    CloseInfo();
  m_pLastDig=pPt;
  }

//---------------------------------------------------------------------------

void CStatsWnd::CloseInfo()
  {
  if (m_pInfoWnd)
    m_pInfoWnd->DestroyWindow();
  m_pInfoWnd=NULL;
  }

//---------------------------------------------------------------------------

void CStatsWnd::OnMouseMove(UINT nFlags, CPoint point) 
  {
  CFormView::OnMouseMove(nFlags, point);
  if (nFlags & MK_LBUTTON)
    ShowInfo(point, FindDigPt(nFlags, point));
  else
    CloseInfo();
  }

//---------------------------------------------------------------------------

LRESULT CStatsWnd::OnCloseOrdInfo(WPARAM wParam, LPARAM lParam)
  {
  CloseInfo();
  return 0;
  }

//===========================================================================
//
//
//
//===========================================================================

class CStatsFrm : public CMDIChildWnd
  {
    DECLARE_DYNCREATE(CStatsFrm)
    CStatsFrm();           // protected constructor used by dynamic creation
  public:
    //{{AFX_VIRTUAL(CStatsFrm)
    //}}AFX_VIRTUAL
  protected:
    virtual ~CStatsFrm();
    //{{AFX_MSG(CStatsFrm)
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnClose();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  };

//---------------------------------------------------------------------------

IMPLEMENT_DYNCREATE(CStatsFrm, CMDIChildWnd)

CStatsFrm::CStatsFrm()
  {
  }

//---------------------------------------------------------------------------

CStatsFrm::~CStatsFrm()
  {
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CStatsFrm, CMDIChildWnd)
  //{{AFX_MSG_MAP(CStatsFrm)
  ON_WM_SETFOCUS()
  ON_WM_CLOSE()
  ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

void CStatsFrm::OnSetFocus(CWnd* pOldWnd) 
  {
  CStatsWnd* pMsg = (CStatsWnd*)GetTopWindow();
  if (pMsg)
    {
    pMsg->UpdateData(false);
    pMsg->DoLayout();
    //pMsg->UpdateDialogControls(pMsg, false);
    }
  CMDIChildWnd::OnSetFocus(pOldWnd);
  }

//---------------------------------------------------------------------------

void CStatsFrm::OnClose() 
  {
  //CMDIChildWnd::OnClose(); Do NOT call base, we are not allowed to close this window!
  }

//---------------------------------------------------------------------------

void CStatsFrm::OnSize(UINT nType, int cx, int cy) 
  {
  CMDIChildWnd::OnSize(nType, cx, cy);
  if (nType==SIZE_MAXIMIZED)
    CWindowLists::SetWndMaxMode(true);
  else if (nType==SIZE_MINIMIZED || nType==SIZE_RESTORED)
    CWindowLists::SetWndMaxMode(false);
  }

void CStatsWnd::OnSelchangeShowwhat() 
  {
  bool Enable = (m_ShowWhat.GetCurSel()==CS_Rel) || (m_ShowWhat.GetCurSel()==CS_MaxRel);
  m_FilterNC.EnableWindow(Enable);
  m_ToleranceSlide.EnableWindow(Enable);

  OnRefresh();	
  }

void CStatsWnd::OnInitialUpdate() 
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

void CStatsWindow::Register()
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
  CC.m_pNewViewClass=RUNTIME_CLASS(CStatsWnd);
  s_pStatsFrm = new CStatsFrm;
  hIcon = ScdApp()->LoadIcon(IDI_ORDWND);
  ClassName = AfxRegisterWndClass(CS_DBLCLKS|CS_NOCLOSE, 0, 0, hIcon);
  if (s_pStatsFrm->Create(ClassName.GetBuffer(0), 
                        "Runtime Statistics", WS_CHILD | WS_OVERLAPPED | 
                        WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | 
                        WS_MINIMIZEBOX | WS_MAXIMIZEBOX, 
                        Rect, NULL, &CC))
    {
    s_pStatsWnd = (CStatsWnd*)s_pStatsFrm->GetTopWindow();
//    RegisterMsgWnd(s_pStatsWnd->m_hWnd);
    s_pStatsFrm->ShowWindow(SW_MINIMIZE);
    }
  }

//---------------------------------------------------------------------------

int CStatsWindow::Show(flag DoBringToTop)
  {
  if (s_pStatsFrm && DoBringToTop && (s_pStatsFrm->GetFocus()!=s_pStatsFrm))
    {
    s_pStatsFrm->MDIActivate(); //bring the window to the front
    if (s_pStatsFrm->IsIconic())
      return s_pStatsFrm->ShowWindow(SW_RESTORE);
    }

  return 0;
  };

//---------------------------------------------------------------------------

int CStatsWindow::Refresh()
  {
  if (s_pStatsFrm && s_pStatsWnd)
    {
    if (!s_pStatsFrm->IsIconic())
      return s_pStatsWnd->DoRefresh();
    }
  return 0;
  };

//---------------------------------------------------------------------------

//int CStatsWindow::Hide()          { return s_pStatsFrm->ShowWindow(SW_HIDE); };
//int CStatsWindow::Reduce()        { return s_pStatsFrm->ShowWindow(SW_SHOWMINNOACTIVE); };

CFormView* CStatsWindow::GetWnd() { return s_pStatsWnd; };
CMDIChildWnd* CStatsWindow::GetFrame() { return s_pStatsFrm; };

//===========================================================================
//
//
//
//===========================================================================

