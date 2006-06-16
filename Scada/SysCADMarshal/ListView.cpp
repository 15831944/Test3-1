//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  ListView.cpp
//  $Author    :  Jim Hansen
//
//  Description:  ListView that displays tags in report mode
//
//
//**************************************************************************
#include "stdafx.h"
#include "slotcommon.h"
#include "SysCADMarshal.h"
#include "SysCADMarshalDoc.h"

#include "ListView.h"
#include "Mainfrm.h"
#include "slotmngr.h"
//#include "opcstuff.h"
#include "slotcommon.h"
#include "finditdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


LPARAM lastCol = 555;

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CValueEdit, CEdit)
  ON_WM_CHAR()
  ON_WM_KEYDOWN()
  ON_WM_SETFOCUS()
  ON_WM_KILLFOCUS()
  ON_WM_DESTROY()
  //  ON_COMMAND(ID_EDIT_PASTE, OnPaste)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------
CValueEdit::~CValueEdit(void)
  {
  TRACE("x");
  }

//---------------------------------------------------------------------------

void CValueEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
  {
  switch(nChar)
    {
    case VK_RETURN: if(m_pEdtCtrlView) m_pEdtCtrlView->SetFocus(); break;
    case VK_ESCAPE: if(m_pEdtCtrlView) m_pEdtCtrlView->SetFocus(); break;
    default:        CEdit::OnKeyDown(nChar, nRepCnt, nFlags); break;
    }
  }

//---------------------------------------------------------------------------

void CValueEdit::OnDestroy(void)
  {
  CString T;
  GetWindowText(T);

  CFullValue FV(OPC_QUALITY_GOOD);
  COleVariant &V=FV;
  V=m_pDoc->m_EdtValue.m_vValue;

  double f=0;
  T.TrimLeft();
  T.TrimRight();
  if (T.GetLength()>2 && T[0]=='0' && (T[1]=='x' || T[1]=='X'))
    {
    ReportError("List", 0, "Incomplete - Cannot yet parse Hex Value %s", T);
    V.ChangeType(VT_R8);
    f=V.dblVal;
    }
  else
    f = atof(T);

  switch( m_pDoc->m_EdtValue.Type())
    {
    case VT_EMPTY:
    case VT_NULL:
      V.vt = VT_R8;
      V.dblVal = 0.0;
      break;
    case VT_BOOL:
      V.boolVal = f!=0.0;
      break;
    case VT_UI1: // uchar
      V.bVal = (unsigned char)f;
      break;
    case VT_UI2: // uchar
      V.uiVal = (unsigned short)f;
      break;
    case VT_UI4: // uchar
      V.ulVal = (unsigned long)f;
      break;
    case VT_I1 :  // long
      V.cVal = (char)f;
      break;
    case VT_I2 :   // short
      V.iVal = (short)f;
      break;
    case VT_I4 :  // long
      V.lVal = (long)f;
      break;
    case VT_R4 :
      V.fltVal = (float)f;
      break;
    case VT_R8 :
      V.dblVal = f;
      break;
    default: ASSERT( FALSE );
    }

  if (m_pDoc->m_EdtDst==eCSD_Link)
    gs_SlotMngr.SendSimValue2Link(eCSD_Manual, m_pDoc->m_EdtItem, gs_SlotMngr.GetTransactionID(), FV, true);
  else
    gs_SlotMngr.AppendChange(eCSD_Manual, -1, m_pDoc->m_EdtDst, m_pDoc->m_EdtItem, gs_SlotMngr.GetTransactionID(), FV, NULL, true);
  gs_SlotMngr.RequestQueueFlush();

  m_pEdtCtrlView->m_pEdit = NULL;

  CEdit::OnDestroy();
  }

//---------------------------------------------------------------------------

void CValueEdit::OnKillFocus(CWnd *pNewWnd)
  {
  CEdit::OnKillFocus(pNewWnd);
  DestroyWindow();
  };

//---------------------------------------------------------------------------

CValueEdit * CValueEdit::Start(CFullValue & Value, eConnSrcDst Dst, long Item, RECT & Rct, CTagView*pTagView)
  {
  if (Value.Type()==VT_BOOL)
    {
    CFullValue FV(OPC_QUALITY_GOOD);
    COleVariant &V=FV;
    V=Value.m_vValue;
    V.boolVal = !V.boolVal;
    gs_SlotMngr.AppendChange(eCSD_Manual, -1, Dst, Item, gs_SlotMngr.GetTransactionID(), FV, NULL, true);
    gs_SlotMngr.RequestQueueFlush();
    return NULL;
    }

  //OnEditProperties();
  CValueEdit * pEdit = new CValueEdit;

  pEdit->Create(ES_RIGHT | WS_CHILD | WS_VISIBLE | WS_BORDER, Rct, pTagView, 1);
  pEdit->m_pEdtCtrlView=pTagView;
  pEdit->m_pDoc=pTagView->GetDocument();
  pEdit->SetFont(pTagView->GetListCtrl().GetFont());

  pEdit->m_pDoc->m_EdtValue=Value;
  pEdit->m_pDoc->m_EdtDst=Dst;
  pEdit->m_pDoc->m_EdtItem=Item;

  CString T;
  VariantToString(Value, T, gs_bAsHex);

  pEdit->SetWindowText(T);
  pEdit->SetSel(0,-1);
  pEdit->SetFocus();
  return pEdit;
  }

//-------------------------------------------------------------------

//     No  Typ  Tag  SimT SVal DevT DVal Src  Dst CVal  Tim   DT  Dly  Chg  Ign  Trn  Err Shed CdBk sCon sTyp sRng sMod sDrv  Dsc 
static long gs_ColWd[MaxViews][MaxColumns] = 
  {
    {  50,  50, 150,  150,  80, 150,  80,  0,   0,   0,  60,   0,   0,  55,  55,  55,  80,   0,   0,   0,   0,   0,   0,   0, 150, -1},
    {  50,  50, 150,  150,  80, 150,  80,  0,   0,   0,  60,   0,   0,  55,  55,  55,  80,   0,   0,   0,   0,   0,   0,   0, 150, -1},
    {  50,   0, 150,    0,   0,   0,   0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  80, 150,   0,   0,   0,   0,   0, 150, -1},
    {  70,  50, 150,    0,   0,   0,   0, 50,  50,  80,  60,  60,  60,  55,   0,  55,  80,   0,   0,   0,   0,   0,   0,   0,   0, -1},
    {  50,   0, 150,    0,   0,   0,   0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 150,  60,  80,  80, 100, 150, -1},
  };

//-------------------------------------------------------------------

// CTagView
IMPLEMENT_DYNCREATE(CTagView, CListView)
BEGIN_MESSAGE_MAP(CTagView, CListView)
  //{{AFX_MSG_MAP(CTagView)
  ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemchanged)
  ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
  ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
  ON_WM_CREATE()
  ON_WM_TIMER()
  ON_WM_VSCROLL()
  ON_WM_SIZE()
  ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
  ON_WM_KEYUP()
  ON_WM_KEYDOWN()
  ON_WM_DESTROY()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//-------------------------------------------------------------------

// CTagView construction/destruction
CTagView::CTagView(void)
  {
  // set the CListCtrl attributes
  m_dwDefaultStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT;

  m_pEdit = NULL;
  m_pCfgDlg = NULL;

  //m_SlotColWidth[0]=0;
  //m_LinkColWidth[0]=0;
  //m_CdBlkColWidth[0]=0;
  //m_ChgColWidth[0]=0;  

  m_lLastSlotTop=-1;
  m_lLastLinkTop=-1;
  m_lLastCdBlkTop=-1;

  }

CTagView::~CTagView(void)
  {
  }

//-------------------------------------------------------------------

BOOL CTagView::PreCreateWindow(CREATESTRUCT &cs)
  {
  // TODO: Modify the Window class or styles here by modifying
  //  the CREATESTRUCT cs
  return CListView::PreCreateWindow(cs);
  }

//-------------------------------------------------------------------

int CTagView::OnCreate(LPCREATESTRUCT lpCreateStruct)
  {
  if(CListView::OnCreate(lpCreateStruct) == -1) return -1;

  // Create the Image List
  m_ctlImage.Create(IDB_IMAGELIST, 16, 0, RGB(255, 0, 255));
  m_ctlImage.SetBkColor(GetSysColor(COLOR_WINDOW));

  //ImageList_SetOverlayImage(m_ctlImage, 5,1);
  //ImageList_SetOverlayImage(m_ctlImage, 6,2);
  //ImageList_SetOverlayImage(m_ctlImage, 7,3);

  /// Attach image list to List
  CListCtrl &list = GetListCtrl();
  list.SetImageList(&m_ctlImage, LVSIL_SMALL);

  CSysCADMarshalDoc *pDoc = GetDocument();
  pDoc->SetTagView(this);

  list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_SUBITEMIMAGES);

  return 0;
  }

void CTagView::SetColumns(bool Initial, eViews What)
  {
  CListCtrl &list = GetListCtrl();
  //switch (What)
  //  {
  //  case ViewSlots:   Wds = InitSlotsWidth;   ColWds = m_SlotColWidth; break;
  //  case ViewEdits:   Wds = InitEditsWidth;   ColWds = m_EditColWidth; break;
  //  case ViewLinks:   Wds = InitLinksWidth;   ColWds = m_LinkColWidth;break;
  //  case ViewCdBlks:  Wds = InitCdBlksWidth;  ColWds = m_CdBlkColWidth;break;
  //  case ViewChanges: Wds = InitChgsWidth;    ColWds = m_ChgColWidth;break;
  //  }
  if (Initial)
    {
    for (int i=0; i<MaxViews; i++)
      {
      //for (int j=0; j<MaxColumns; j++)
      //  m_ColWidths[i][j]= gs_ColWd[i][j];
      CString Line;
      Line=ScdPFUser.RdStr("ColWidths", pszViewNames[i], "");
      if (Line.GetLength()>0)
        {
        for (int k=0, j=0; k>=0 && j<MaxColumns; j++)
          gs_ColWd[i][j]=Range(0, SafeAtoI(Line.Tokenize(",", k)), 350);
        }
      }
    m_ColView=What;
    }
  else if (What!=m_ColView)
    {
    KeepColumnWidths();
    }

  m_ColView=What;
  long *Wds=gs_ColWd[m_ColView];
  long ColWds[MaxColumns];

  CSysCADMarshalDoc *pDoc = GetDocument();
  bool Cfg=pDoc->m_bViewCfg;
  bool Sts=pDoc->m_bViewStats;

  int i=0;
  int j=0;
  if (Wds[i]>0       ) { m_lColumn_NO       = j; ColWds[j]=Wds[i]; j++; } else { m_lColumn_NO       = -1; }; i++;
  if (Wds[i]>0       ) { m_lColumn_TYPE     = j; ColWds[j]=Wds[i]; j++; } else { m_lColumn_TYPE     = -1; }; i++;
  if (Wds[i]>0       ) { m_lColumn_TAG      = j; ColWds[j]=Wds[i]; j++; } else { m_lColumn_TAG      = -1; }; i++;
  if (Wds[i]>0 && Cfg) { m_lColumn_SIMTAG   = j; ColWds[j]=Wds[i]; j++; } else { m_lColumn_SIMTAG   = -1; }; i++;
  if (Wds[i]>0       ) { m_lColumn_SIMVAL   = j; ColWds[j]=Wds[i]; j++; } else { m_lColumn_SIMVAL   = -1; }; i++;
  if (Wds[i]>0 && Cfg) { m_lColumn_DEVTAG   = j; ColWds[j]=Wds[i]; j++; } else { m_lColumn_DEVTAG   = -1; }; i++;
  if (Wds[i]>0       ) { m_lColumn_DEVVAL   = j; ColWds[j]=Wds[i]; j++; } else { m_lColumn_DEVVAL   = -1; }; i++;
  if (Wds[i]>0       ) { m_lColumn_SRC      = j; ColWds[j]=Wds[i]; j++; } else { m_lColumn_SRC      = -1; }; i++;
  if (Wds[i]>0       ) { m_lColumn_DST      = j; ColWds[j]=Wds[i]; j++; } else { m_lColumn_DST      = -1; }; i++;
  if (Wds[i]>0       ) { m_lColumn_CHGVAL   = j; ColWds[j]=Wds[i]; j++; } else { m_lColumn_CHGVAL   = -1; }; i++;
  if (Wds[i]>0       ) { m_lColumn_TIME     = j; ColWds[j]=Wds[i]; j++; } else { m_lColumn_TIME     = -1; }; i++;
  if (Wds[i]>0 && Sts) { m_lColumn_DTIME    = j; ColWds[j]=Wds[i]; j++; } else { m_lColumn_DTIME    = -1; }; i++;
  if (Wds[i]>0 && Sts) { m_lColumn_DELAY    = j; ColWds[j]=Wds[i]; j++; } else { m_lColumn_DELAY    = -1; }; i++;
  if (Wds[i]>0 && Sts) { m_lColumn_CHGCNT   = j; ColWds[j]=Wds[i]; j++; } else { m_lColumn_CHGCNT   = -1; }; i++;
  if (Wds[i]>0 && Sts) { m_lColumn_IGNORE   = j; ColWds[j]=Wds[i]; j++; } else { m_lColumn_IGNORE   = -1; }; i++;
  if (Wds[i]>0 && Sts) { m_lColumn_TRANS    = j; ColWds[j]=Wds[i]; j++; } else { m_lColumn_TRANS    = -1; }; i++;
  if (Wds[i]>0 && Cfg) { m_lColumn_ERROR    = j; ColWds[j]=Wds[i]; j++; } else { m_lColumn_ERROR    = -1; }; i++;
  if (Wds[i]>0 && Cfg) { m_lColumn_SCHED    = j; ColWds[j]=Wds[i]; j++; } else { m_lColumn_SCHED    = -1; }; i++;
  if (Wds[i]>0 && Cfg) { m_lColumn_CDBLK    = j; ColWds[j]=Wds[i]; j++; } else { m_lColumn_CDBLK    = -1; }; i++;
  if (Wds[i]>0 && Cfg) { m_lColumn_sCON     = j; ColWds[j]=Wds[i]; j++; } else { m_lColumn_sCON     = -1; }; i++;
  if (Wds[i]>0 && Cfg) { m_lColumn_sTYP     = j; ColWds[j]=Wds[i]; j++; } else { m_lColumn_sTYP     = -1; }; i++;
  if (Wds[i]>0 && Cfg) { m_lColumn_sRNG     = j; ColWds[j]=Wds[i]; j++; } else { m_lColumn_sRNG     = -1; }; i++;
  if (Wds[i]>0 && Cfg) { m_lColumn_sMOD     = j; ColWds[j]=Wds[i]; j++; } else { m_lColumn_sMOD     = -1; }; i++;
  if (Wds[i]>0 && Cfg) { m_lColumn_sDRV     = j; ColWds[j]=Wds[i]; j++; } else { m_lColumn_sDRV     = -1; }; i++;
  if (Wds[i]>0       ) { m_lColumn_DESC     = j; ColWds[j]=Wds[i]; j++; } else { m_lColumn_DESC     = -1; }; i++;


  list.DeleteAllItems();
  while(list.DeleteColumn(0))
    {};

  if (m_lColumn_NO      >=0) list.InsertColumn(m_lColumn_NO,      _T("No"),             LVCFMT_RIGHT,ColWds[m_lColumn_NO    ], m_lColumn_NO);
  if (m_lColumn_TYPE    >=0) list.InsertColumn(m_lColumn_TYPE,    _T("Type"),           LVCFMT_LEFT, ColWds[m_lColumn_TYPE  ], m_lColumn_TYPE);
  if (m_lColumn_TAG     >=0) list.InsertColumn(m_lColumn_TAG,     _T("Marshal Tag"),    LVCFMT_LEFT, ColWds[m_lColumn_TAG   ], m_lColumn_TAG);
  if (m_lColumn_SIMTAG  >=0) list.InsertColumn(m_lColumn_SIMTAG,  _T("Simulator Tag"),  LVCFMT_LEFT, ColWds[m_lColumn_SIMTAG], m_lColumn_SIMTAG);
  if (m_lColumn_SIMVAL  >=0) list.InsertColumn(m_lColumn_SIMVAL,  _T("SimValue"),       LVCFMT_RIGHT,ColWds[m_lColumn_SIMVAL], m_lColumn_SIMVAL);
  if (m_lColumn_DEVTAG  >=0) list.InsertColumn(m_lColumn_DEVTAG,  _T("OPC Tag"),        LVCFMT_LEFT, ColWds[m_lColumn_DEVTAG], m_lColumn_DEVTAG);
  if (m_lColumn_DEVVAL  >=0) list.InsertColumn(m_lColumn_DEVVAL,  _T("OPC Value"),      LVCFMT_RIGHT,ColWds[m_lColumn_DEVVAL], m_lColumn_DEVVAL);
  if (m_lColumn_SRC     >=0) list.InsertColumn(m_lColumn_SRC,     _T("Src"),            LVCFMT_LEFT, ColWds[m_lColumn_SRC   ], m_lColumn_SRC);
  if (m_lColumn_DST     >=0) list.InsertColumn(m_lColumn_DST,     _T("Dst"),            LVCFMT_LEFT, ColWds[m_lColumn_DST   ], m_lColumn_DST);
  if (m_lColumn_CHGVAL  >=0) list.InsertColumn(m_lColumn_CHGVAL,  _T("ChgValue"),       LVCFMT_RIGHT,ColWds[m_lColumn_CHGVAL], m_lColumn_CHGVAL);
  if (m_lColumn_TIME    >=0) list.InsertColumn(m_lColumn_TIME,    _T("Time"),           LVCFMT_RIGHT,ColWds[m_lColumn_TIME  ], m_lColumn_TIME);
  if (m_lColumn_DTIME   >=0) list.InsertColumn(m_lColumn_DTIME,   _T("DTime"),          LVCFMT_RIGHT,ColWds[m_lColumn_DTIME ], m_lColumn_DTIME);
  if (m_lColumn_DELAY   >=0) list.InsertColumn(m_lColumn_DELAY,   _T("Delay"),          LVCFMT_RIGHT,ColWds[m_lColumn_DELAY ], m_lColumn_DELAY);
  if (m_lColumn_CHGCNT  >=0) list.InsertColumn(m_lColumn_CHGCNT,  _T("Changes"),        LVCFMT_RIGHT,ColWds[m_lColumn_CHGCNT], m_lColumn_CHGCNT);
  if (m_lColumn_IGNORE  >=0) list.InsertColumn(m_lColumn_IGNORE,  _T("Skipped"),        LVCFMT_RIGHT,ColWds[m_lColumn_IGNORE], m_lColumn_IGNORE);
  if (m_lColumn_TRANS   >=0) list.InsertColumn(m_lColumn_TRANS,   _T("TransID"),        LVCFMT_RIGHT,ColWds[m_lColumn_TRANS ], m_lColumn_TRANS);
  if (m_lColumn_ERROR   >=0) list.InsertColumn(m_lColumn_ERROR,   _T("Error"),          LVCFMT_LEFT, ColWds[m_lColumn_ERROR ], m_lColumn_ERROR);
  if (m_lColumn_SCHED   >=0) list.InsertColumn(m_lColumn_SCHED,   _T("Schedule"),       LVCFMT_LEFT, ColWds[m_lColumn_SCHED ], m_lColumn_SCHED);
  if (m_lColumn_CDBLK   >=0) list.InsertColumn(m_lColumn_CDBLK,   _T("CodeBlock"),      LVCFMT_LEFT, ColWds[m_lColumn_CDBLK ], m_lColumn_CDBLK);
  if (m_lColumn_sCON    >=0) list.InsertColumn(m_lColumn_sCON,    _T("Connects"),       LVCFMT_LEFT, ColWds[m_lColumn_sCON  ], m_lColumn_sCON);
  if (m_lColumn_sTYP    >=0) list.InsertColumn(m_lColumn_sTYP,    _T("Type"),           LVCFMT_LEFT, ColWds[m_lColumn_sTYP  ], m_lColumn_sTYP);
  if (m_lColumn_sRNG    >=0) list.InsertColumn(m_lColumn_sRNG,    _T("Range"),          LVCFMT_LEFT, ColWds[m_lColumn_sRNG  ], m_lColumn_sRNG);
  if (m_lColumn_sMOD    >=0) list.InsertColumn(m_lColumn_sMOD,    _T("Modifier"),       LVCFMT_LEFT, ColWds[m_lColumn_sMOD  ], m_lColumn_sMOD);
  if (m_lColumn_sDRV    >=0) list.InsertColumn(m_lColumn_sDRV,    _T("OPC Address"),    LVCFMT_LEFT, ColWds[m_lColumn_sDRV  ], m_lColumn_sDRV);
  if (m_lColumn_DESC    >=0) list.InsertColumn(m_lColumn_DESC,    _T("Description"),    LVCFMT_LEFT, ColWds[m_lColumn_DESC  ], m_lColumn_DESC);

  }

//-------------------------------------------------------------------

void CTagView::OnInitialUpdate(void)
  {
  SetColumns(true, ViewSlots);

  //   VERIFY( DDE_ID == SetTimer( DDE_ID, 100, NULL ) );
  CListView::OnInitialUpdate();
  }

//-------------------------------------------------------------------

void  CTagView::OnDestroy(void)
  {
  KeepColumnWidths();
  for (int j=0; j<MaxViews; j++)
    {
    CString Line, S;
    for (int i=0; i<MaxColumns; i++)
      {
      S.Format("%i,", gs_ColWd[j][i]);
      Line+=S;
      }
    ScdPFUser.WrStr("ColWidths", pszViewNames[j], Line);
    }
  };

//-------------------------------------------------------------------

void  CTagView::KeepColumnWidths()
  {
  CListCtrl &list = GetListCtrl();
  long *Wds=gs_ColWd[m_ColView];
  // Keep ColWIdths
  int N=list.GetHeaderCtrl()->GetItemCount();
  int i=0;
  int j=0;    
  if (m_lColumn_NO       >=0 && j<N) { Wds[i]= list.GetColumnWidth(j++); } i++;
  if (m_lColumn_TYPE     >=0 && j<N) { Wds[i]= list.GetColumnWidth(j++); } i++;
  if (m_lColumn_TAG      >=0 && j<N) { Wds[i]= list.GetColumnWidth(j++); } i++;
  if (m_lColumn_SIMTAG   >=0 && j<N) { Wds[i]= list.GetColumnWidth(j++); } i++;
  if (m_lColumn_SIMVAL   >=0 && j<N) { Wds[i]= list.GetColumnWidth(j++); } i++;
  if (m_lColumn_DEVTAG   >=0 && j<N) { Wds[i]= list.GetColumnWidth(j++); } i++;
  if (m_lColumn_DEVVAL   >=0 && j<N) { Wds[i]= list.GetColumnWidth(j++); } i++;
  if (m_lColumn_SRC      >=0 && j<N) { Wds[i]= list.GetColumnWidth(j++); } i++;
  if (m_lColumn_DST      >=0 && j<N) { Wds[i]= list.GetColumnWidth(j++); } i++;
  if (m_lColumn_CHGVAL   >=0 && j<N) { Wds[i]= list.GetColumnWidth(j++); } i++;
  if (m_lColumn_TIME     >=0 && j<N) { Wds[i]= list.GetColumnWidth(j++); } i++;
  if (m_lColumn_DTIME    >=0 && j<N) { Wds[i]= list.GetColumnWidth(j++); } i++;
  if (m_lColumn_DELAY    >=0 && j<N) { Wds[i]= list.GetColumnWidth(j++); } i++;
  if (m_lColumn_CHGCNT   >=0 && j<N) { Wds[i]= list.GetColumnWidth(j++); } i++;
  if (m_lColumn_IGNORE   >=0 && j<N) { Wds[i]= list.GetColumnWidth(j++); } i++;
  if (m_lColumn_TRANS    >=0 && j<N) { Wds[i]= list.GetColumnWidth(j++); } i++;
  if (m_lColumn_ERROR    >=0 && j<N) { Wds[i]= list.GetColumnWidth(j++); } i++;
  if (m_lColumn_SCHED    >=0 && j<N) { Wds[i]= list.GetColumnWidth(j++); } i++;
  if (m_lColumn_CDBLK    >=0 && j<N) { Wds[i]= list.GetColumnWidth(j++); } i++;
  if (m_lColumn_sCON     >=0 && j<N) { Wds[i]= list.GetColumnWidth(j++); } i++;
  if (m_lColumn_sTYP     >=0 && j<N) { Wds[i]= list.GetColumnWidth(j++); } i++;
  if (m_lColumn_sRNG     >=0 && j<N) { Wds[i]= list.GetColumnWidth(j++); } i++;
  if (m_lColumn_sMOD     >=0 && j<N) { Wds[i]= list.GetColumnWidth(j++); } i++;
  if (m_lColumn_sDRV     >=0 && j<N) { Wds[i]= list.GetColumnWidth(j++); } i++;
  if (m_lColumn_DESC     >=0 && j<N) { Wds[i]= list.GetColumnWidth(j++); } i++;
  ASSERT_ALWAYS(j==N, "Bad Column count")
  }
//-------------------------------------------------------------------

// CTagView diagnostics
#ifdef _DEBUG
void CTagView::AssertValid(void) const
  {
  CListView::AssertValid();
  }

void CTagView::Dump(CDumpContext &dc) const
  {
  CListView::Dump(dc);
  }

CSysCADMarshalDoc *CTagView::GetDocument(void)  // non-debug version is inline
  {
  ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSysCADMarshalDoc)));
  return (CSysCADMarshalDoc *) m_pDocument;
  }
#endif //_DEBUG

//---------------------------------------------------------------------------

LPCSTR DirnString(CSlotCfg & S)
  {
  if (S.m_State.m_nError)            return "ERR";
  if (S.m_bRead && S.m_bWrite) return "rw";
  if (S.m_bRead)             return "r";
  if (S.m_bWrite)            return "w";

  return "";
  }

//-------------------------------------------------------------------

long ImageNo(CSlotCfg & S)
  {
  long i=18;
  if (S.m_bRead && S.m_bWrite)
    i += 3;
  else if (S.m_bRead)
    i += 2;
  else if (S.m_bWrite)
    i += 1;
  if (S.m_pLink)
    i += 8;
  return i;
  }

//-------------------------------------------------------------------

long ImageNo(CSlotCfgStrings & S)
  {
  long i=0;
  if (S.m_eDoWhat == SlotCfgUpdate)
    i = 25+8;
  else if (S.m_eDoWhat == SlotCfgAdd)
    i = 24+8;
  else if (S.m_eDoWhat == SlotCfgDelete)
    i = 26+8;
  return i;
  }

//-------------------------------------------------------------------

long TypeImageNo(bool Hold, VARTYPE vt)
  {
  long i;
  if (Hold)
    i = 9;
  else if ( vt == VT_EMPTY || vt == VT_NULL )
    i = 0;
  else if ( vt == VT_BOOL)
    i = 13;
  else if (IsFloatDataVT(vt))
    i = 10;
  else if (IsUnsignedDataVT(vt))
    i = 11;
  else    
    i = 12;
  return i;
  }

//---------------------------------------------------------------------------

long ValueImageNo(CSlotCfg & S)
  {
  long i=5;
  if (S.m_State.m_nError)
    i += 2;
  if ((S.m_wQuality & OPC_QUALITY_MASK) != OPC_QUALITY_GOOD )
    i += 1;
  return i;
  }

//---------------------------------------------------------------------------

long ImageNo(CLinkCfg & S)
  {
  long i=14+8;
  if (S.m_bGet && S.m_bSet)
    i += 3;
  else if (S.m_bGet)
    i += 1;
  else if (S.m_bSet)
    i += 2;
  return i;
  }

//---------------------------------------------------------------------------

long ValueImageNo(CLinkCfg & S)
  {
  long i=5;
  if (S.m_State.m_nError)
    i += 2;
  if ((S.m_wQuality & OPC_QUALITY_MASK) != OPC_QUALITY_GOOD )
    i += 1;
  return i;
  }

//-------------------------------------------------------------------

long TypeImageNo(CChangeItem & C)
  {
  long i;
  if ( C.Type() == VT_EMPTY || C.Type() == VT_NULL )
    i = 0;
  else if ( C.Type() == VT_BOOL)
    i = 12;
  else if (IsFloatDataVT(C.Type()))
    i = 9;
  else if (IsUnsignedDataVT(C.Type()))
    i = 10;
  else    
    i = 11;
  return i;
  }

//-------------------------------------------------------------------

long ImageNo(CChangeItem & C)
  {
  long i=23+8;

  switch (C.m_eSrc)
    {
    case eCSD_Device:         i=20; break;
    case eCSD_Simulator:      i=15; break;
    case eCSD_File:           
    case eCSD_Manual:         i=22; break;
    default:
      switch (C.m_eDst)
        {
        case eCSD_Device:     i=19; break;
        case eCSD_Simulator:  i=16; break;
        default:
          {
          }
        }
    };
  return i;
  }

//-------------------------------------------------------------------

long CTagView::InsertItem(CSlotCfg *pSlot)
  {
  CSysCADMarshalDoc *pDoc = GetDocument();
  CListCtrl     &list = GetListCtrl();

  CString N;
  N.Format("%i", pSlot->m_lSlot);

  LV_ITEM lv_item;
  lv_item.mask = LVIF_TEXT|LVIF_IMAGE| LVIF_PARAM;
  lv_item.iImage = ImageNo(*pSlot);
  lv_item.iItem = list.GetItemCount();
  lv_item.iSubItem = m_lColumn_NO;
  lv_item.pszText = (LPSTR)(LPCSTR)N;
  lv_item.lParam = (long)pSlot;
  list.SetItem(&lv_item);

  int ListIndex = list.InsertItem(&lv_item);

  list.SetItem(ListIndex,m_lColumn_TAG,LVIF_TEXT,(LPSTR)(LPCSTR)pSlot->m_sTag,0,0,0,0);
  if (pSlot->m_pLink)
    list.SetItem(ListIndex,m_lColumn_SIMTAG,LVIF_TEXT,(LPSTR)(LPCSTR)pSlot->m_pLink->m_sSimTag,0,0,0,0);
  list.SetItem(ListIndex,m_lColumn_DEVTAG,LVIF_TEXT,(LPSTR)(LPCSTR)pSlot->m_sOPCTag,0,0,0,0);
  list.SetItem(ListIndex,m_lColumn_DESC,LVIF_TEXT,(LPSTR)(LPCSTR)pSlot->m_sDesc,0,0,0,0);

  list.SetItemData(ListIndex,(long)pSlot);
  return ListIndex;
  }

//-------------------------------------------------------------------

void CTagView::SetRuntimeValues(long ListIndex, CSlotCfg * pSlot)
  {
  //  dbgpln("SRTV:Slot %4i  %4i", ListIndex, pSlot->m_lSlot);

  CSysCADMarshalDoc *pDoc = GetDocument();
  CListCtrl     &list = GetListCtrl();

  LV_ITEM lv_item;
  if (m_lColumn_TYPE>=0)
    {
    lv_item.mask      = LVIF_IMAGE|LVIF_TEXT;
    lv_item.iImage    = TypeImageNo(pSlot->m_State.m_bHold, pSlot->Type());
    lv_item.iItem     = ListIndex;
    lv_item.iSubItem  = m_lColumn_TYPE;
    lv_item.pszText   = (LPSTR)TypeToString(pSlot->Type());
    list.SetItem(&lv_item);
    }

  CString V;

  if (pSlot->m_pLink)
    {
    VariantToString(*pSlot->m_pLink, V, gs_bAsHex);
    lv_item.mask      = LVIF_IMAGE|LVIF_TEXT;
    lv_item.iImage    = ValueImageNo(*pSlot->m_pLink);
    lv_item.iItem     = ListIndex;
    lv_item.iSubItem  = m_lColumn_SIMVAL;
    lv_item.pszText   = (LPSTR)(LPCSTR)V;
    list.SetItem(&lv_item);
    }  

  VariantToString(*pSlot, V, gs_bAsHex);
  lv_item.mask      = LVIF_IMAGE|LVIF_TEXT;
  lv_item.iImage    = ValueImageNo(*pSlot);
  lv_item.iItem     = ListIndex;
  lv_item.iSubItem  = m_lColumn_DEVVAL;
  lv_item.pszText   = (LPSTR)(LPCSTR)V;
  list.SetItem(&lv_item);


  TimeStampToString(*pSlot, V, true, NULL);
  list.SetItem(ListIndex, m_lColumn_TIME, LVIF_TEXT, (LPSTR)(LPCSTR)V,0,0,0,0);
  V.Format("%i", pSlot->m_lValueChanges);
  list.SetItem(ListIndex, m_lColumn_CHGCNT, LVIF_TEXT, (LPSTR)(LPCSTR)V,0,0,0,0);
  V.Format("%i", pSlot->m_lValuesIgnored);
  list.SetItem(ListIndex, m_lColumn_IGNORE, LVIF_TEXT, (LPSTR)(LPCSTR)V,0,0,0,0);
  V.Format(pSlot->m_dwTransactionID>=0?"%i":"", pSlot->m_dwTransactionID);
  list.SetItem(ListIndex, m_lColumn_TRANS, LVIF_TEXT, (LPSTR)(LPCSTR)V,0,0,0,0);
  list.SetItem(ListIndex,m_lColumn_ERROR,LVIF_TEXT,SErr_String(pSlot->m_State.m_nError),0,0,0,0);

  }

//-------------------------------------------------------------------

long CTagView::InsertItem(CLinkCfg * pLink)
  {
  CSysCADMarshalDoc *pDoc = GetDocument();
  CListCtrl     &list = GetListCtrl();

  CString N;
  N.Format("%i", pLink->m_lLink);

  LV_ITEM lv_item;
  lv_item.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
  lv_item.iImage = ImageNo(*pLink);
  lv_item.iItem = list.GetItemCount();
  lv_item.iSubItem = m_lColumn_NO;
  lv_item.pszText = (LPSTR)(LPCSTR)N;
  lv_item.lParam = (long)pLink;

  int ListIndex = list.InsertItem(&lv_item);

  list.SetItem(ListIndex,m_lColumn_TAG,LVIF_TEXT,   (LPCSTR)pLink->m_sTag,0,0,0,0);

  LPCSTR BadStr="";//pLink->m_State.SimTagValid?"":"Bad:";
  N.Format(pLink->m_sCnvTxt.GetLength()>0?"%s%s (%s)":"%s%s", BadStr, pLink->m_sSimTag, pLink->m_sCnvTxt);

  list.SetItem(ListIndex,m_lColumn_DEVTAG,LVIF_TEXT,(LPSTR)(LPCSTR)pLink->m_pSlot->m_sOPCTag,0,0,0,0);

  list.SetItem(ListIndex,m_lColumn_SIMTAG,LVIF_TEXT,(LPCSTR)N,0,0,0,0);
  //list.SetItem(ListIndex,m_lColumn_CONN,LVIF_TEXT,  (LPCSTR)pLink->m_sConnectTag,0,0,0,0);
  list.SetItem(ListIndex,m_lColumn_DESC,LVIF_TEXT,pLink->m_sDesc,0,0,0,0);
  list.SetItemData(ListIndex,(long)pLink);
  return ListIndex;
  }

//-------------------------------------------------------------------

void CTagView::SetRuntimeValues(long ListIndex, CLinkCfg * pLink)
  {
  //  dbgpln("SRTV:Link %4i  %4i", ListIndex, pLink->m_lLink);


  CSysCADMarshalDoc *pDoc = GetDocument();
  CListCtrl     &list = GetListCtrl();

  LV_ITEM lv_item;
  //lv_item.mask = LVIF_IMAGE|LVIF_STATE;
  //lv_item.iImage = ImageNo(*pLink);
  //lv_item.iItem = ListIndex;
  //lv_item.iSubItem = m_lColumn_NO;
  //list.SetItem(&lv_item);

  if (m_lColumn_TYPE>=0)
    {
    lv_item.mask      = LVIF_IMAGE|LVIF_TEXT;
    lv_item.iImage    = TypeImageNo(pLink->m_State.m_bHold, pLink->Type());
    lv_item.iItem     = ListIndex;
    lv_item.iSubItem  = m_lColumn_TYPE;
    lv_item.pszText   = (LPSTR)TypeToString(pLink->Type());
    list.SetItem(&lv_item);
    }

  CString V;
  VariantToString(*pLink, V, gs_bAsHex);
  lv_item.mask      = LVIF_IMAGE|LVIF_TEXT;
  lv_item.iImage    = ValueImageNo(*pLink);
  lv_item.iItem     = ListIndex;
  lv_item.iSubItem  = m_lColumn_SIMVAL;
  lv_item.pszText   = (LPSTR)(LPCSTR)V;
  list.SetItem(&lv_item);

  VariantToString(*pLink->m_pSlot, V, gs_bAsHex);
  lv_item.mask      = LVIF_IMAGE|LVIF_TEXT;
  lv_item.iImage    = ValueImageNo(*pLink->m_pSlot);
  lv_item.iItem     = ListIndex;
  lv_item.iSubItem  = m_lColumn_DEVVAL;
  lv_item.pszText   = (LPSTR)(LPCSTR)V;
  list.SetItem(&lv_item);

  TimeStampToString(*pLink, V, true, NULL);
  list.SetItem(ListIndex, m_lColumn_TIME, LVIF_TEXT, (LPSTR)(LPCSTR)V,0,0,0,0);
  V.Format("%i", pLink->m_lValueChanges);
  list.SetItem(ListIndex, m_lColumn_CHGCNT, LVIF_TEXT, (LPSTR)(LPCSTR)V,0,0,0,0);
  V.Format("%i", pLink->m_lValuesIgnored);
  list.SetItem(ListIndex, m_lColumn_IGNORE, LVIF_TEXT, (LPSTR)(LPCSTR)V,0,0,0,0);
  V.Format("%i", pLink->m_dwTransactionID);
  list.SetItem(ListIndex, m_lColumn_TRANS, LVIF_TEXT, (LPSTR)(LPCSTR)V,0,0,0,0);
  list.SetItem(ListIndex,m_lColumn_ERROR,LVIF_TEXT,SErr_String(pLink->m_State.m_nError),0,0,0,0);
  }

//-------------------------------------------------------------------

long CTagView::InsertItem(CCdBlkCfg * pCdBlk)
  {
  CSysCADMarshalDoc *pDoc = GetDocument();
  CListCtrl     &list = GetListCtrl();

  CString N;
  LV_ITEM lv_item;
  lv_item.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
  lv_item.iImage = 4;
  lv_item.iSubItem = m_lColumn_NO;

  N.Format("%i", pCdBlk->m_lCdBlk);
  lv_item.iItem = list.GetItemCount();
  lv_item.pszText = N.GetBuffer(0);
  lv_item.lParam = pCdBlk->m_lCdBlk;

  int ListIndex = list.InsertItem(&lv_item);
  list.SetItem(ListIndex,m_lColumn_TAG,LVIF_TEXT,(LPSTR)(LPCSTR)pCdBlk->m_sTag,0,0,0,0);
  list.SetItem(ListIndex,m_lColumn_SCHED,LVIF_TEXT,(LPSTR)(LPCSTR)pCdBlk->m_sSchedule,0,0,0,0);
  list.SetItem(ListIndex,m_lColumn_CDBLK,LVIF_TEXT,(LPSTR)(LPCSTR)pCdBlk->m_sCdBlk,0,0,0,0);
  list.SetItem(ListIndex,m_lColumn_DESC,LVIF_TEXT,(LPSTR)(LPCSTR)pCdBlk->m_sDescription,0,0,0,0);
  //list.SetItem(ListIndex,m_lColumn_ERROR,LVIF_TEXT,SErr_String(pCdBlk->m_State.m_nError),0,0,0,0);
  return ListIndex;
  }

//-------------------------------------------------------------------

long CTagView::InsertItem(CChangeItem *pChg)
  {
  CSysCADMarshalDoc *pDoc = GetDocument();
  CListCtrl     &list = GetListCtrl();

  CChangeItem &C=*pChg;
  CString V;
  V.Format("%i", C.m_dwNumber);

  LV_ITEM lv_item;
  lv_item.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
  lv_item.iImage = ImageNo(*pChg);
  lv_item.iSubItem = m_lColumn_NO;
  lv_item.iItem = list.GetItemCount();
  lv_item.pszText = (LPSTR)(LPCSTR)V;
  lv_item.lParam = NULL;

  int ListIndex = list.InsertItem(&lv_item);
  LPCSTR Tag="??";
  switch (C.m_eDst)
    {
    case eCSD_Slot: Tag=pDoc->m_SlotCfgs[C.m_lDstInx]->m_sTag; break;
    case eCSD_Link: Tag=pDoc->m_LinkCfgs[C.m_lDstInx]->m_sTag; break;
    default:
      {
      switch (C.m_eSrc)
        {
        case eCSD_Slot: Tag=pDoc->m_SlotCfgs[C.m_lSrcInx]->m_sTag; break;
        case eCSD_Link: Tag=pDoc->m_LinkCfgs[C.m_lSrcInx]->m_sTag; break;
        default:;
        };
      }
    };

  CString TSrc;
  LPCTSTR pSrc=SrcDstString(C.m_eSrc);
  LPCTSTR pDst=SrcDstString(C.m_eDst);

  if (pChg->m_bRefresh)
    {
    TSrc.Format("*%s", pSrc);
    pSrc=TSrc;
    }

  list.SetItem(ListIndex,m_lColumn_TAG,LVIF_TEXT,Tag,0,0,0,0);
  list.SetItem(ListIndex,m_lColumn_SRC,LVIF_TEXT,pSrc,0,0,0,0);
  list.SetItem(ListIndex,m_lColumn_DST,LVIF_TEXT,pDst,0,0,0,0);

  lv_item.mask      = LVIF_IMAGE|LVIF_TEXT;
  lv_item.iImage    = TypeImageNo(false, C.Type());
  lv_item.iItem     = ListIndex;
  lv_item.iSubItem  = m_lColumn_TYPE;
  lv_item.pszText   = (LPSTR)TypeToString(C.Type());
  list.SetItem(&lv_item);

  VariantToString(C, V, gs_bAsHex);
  list.SetItem(ListIndex, m_lColumn_CHGVAL, LVIF_TEXT, (LPSTR)(LPCSTR)V,0,0,0,0);

  TimeStampToString(*pChg, V, true, NULL);
  list.SetItem(ListIndex, m_lColumn_TIME, LVIF_TEXT, (LPSTR)(LPCSTR)V,0,0,0,0);
  if (pChg->m_dwDelayTimer>0)
    V.Format("%i", pChg->m_dwDelayTimer);
  else
    V.Format("");
  list.SetItem(ListIndex, m_lColumn_DELAY, LVIF_TEXT, (LPSTR)(LPCSTR)V,0,0,0,0);
  V.Format("%i", pChg->m_dwTransactionID);
  list.SetItem(ListIndex, m_lColumn_TRANS, LVIF_TEXT, (LPSTR)(LPCSTR)V,0,0,0,0);

  list.SetItemData(ListIndex,(long)pChg);
  return ListIndex;
  }

//-------------------------------------------------------------------

long CTagView::InsertItem(CSlotCfgStrings *pSlot)
  {
  CSysCADMarshalDoc *pDoc = GetDocument();
  CListCtrl     &list = GetListCtrl();

  CString N;
  N.Format("%i", pSlot->m_lNo);

  LV_ITEM lv_item;
  lv_item.mask = LVIF_TEXT|LVIF_IMAGE| LVIF_PARAM;
  lv_item.iImage = ImageNo(*pSlot);
  lv_item.iItem = list.GetItemCount();
  lv_item.iSubItem = m_lColumn_NO;
  lv_item.pszText = (LPSTR)(LPCSTR)N;
  lv_item.lParam = (long)pSlot;
  list.SetItem(&lv_item);

  int ListIndex = list.InsertItem(&lv_item);

  list.SetItem(ListIndex,m_lColumn_TAG,LVIF_TEXT,(LPSTR)(LPCSTR)pSlot->m_sTag,0,0,0,0);
  list.SetItem(ListIndex,m_lColumn_sCON,LVIF_TEXT,(LPSTR)(LPCSTR)pSlot->m_sConnects,0,0,0,0);
  list.SetItem(ListIndex,m_lColumn_sTYP,LVIF_TEXT,(LPSTR)(LPCSTR)pSlot->m_sType,0,0,0,0);
  list.SetItem(ListIndex,m_lColumn_sDRV,LVIF_TEXT,(LPSTR)(LPCSTR)pSlot->m_sDriver,0,0,0,0);
  list.SetItem(ListIndex,m_lColumn_sMOD,LVIF_TEXT,(LPSTR)(LPCSTR)pSlot->m_sModifier,0,0,0,0);
  list.SetItem(ListIndex,m_lColumn_sRNG,LVIF_TEXT,(LPSTR)(LPCSTR)pSlot->m_sRange,0,0,0,0);
  list.SetItem(ListIndex,m_lColumn_DESC,LVIF_TEXT,(LPSTR)(LPCSTR)pSlot->m_sDesc,0,0,0,0);

  list.SetItemData(ListIndex,(long)pSlot);
  return ListIndex;
  }

// =======================================================================
//
//
//
// =======================================================================

// CTagView message handlers
void CTagView::OnUpdate(CView *pSender, LPARAM lHint, CObject *pHint)
  {
  //  CListView::OnUpdate(pSender, lHint, pHint);

  CSysCADMarshalDoc *pDoc = GetDocument();
  CListCtrl     &list = GetListCtrl();

  long Hint=TheHint(lHint);
  long HIndex=HintIndex(lHint);
  bool DoAll=false;

  switch (Hint)
    {
    case HINT_UPDATE_CFG:
    case HINT_UPDATE_TAGS:
      DoAll=true;
      break;
    case HINT_UPDATE_CHGLOG:
      if (pDoc->m_lView==ViewChanges)
        DoAll=true;
      break;
    case HINT_CLEAR_ALL:
      list.DeleteAllItems();
      return;
    case HINT_CHANGE_VIEW:
      {
      CHeaderCtrl *pHd=list.GetHeaderCtrl();
      list.SetRedraw(false);
      pHd->SetRedraw(false);
      SetColumns(false, pDoc->m_lView);
      pHd->SetRedraw(true);
      list.SetRedraw(true);
      return;
      }
    case HINT_ERROR_ADDED:
      return;
    case HINT_SETSELECTION:
      pDoc->m_lMarkedItem = HIndex;
      //pDoc->m_lView=((HintFlags(lHint)==eFindStr_SlotTag)||(HintFlags(lHint)==eFindStr_OPCTag) ? ViewSlots:ViewLinks);
      break;
    }

  if (DoAll)
    {
    list.SetRedraw(false);
    //dbgpln("CTagView::OnUpdate DoAll  %3i %4i %4i", pDoc->m_lView, pDoc->m_lDeviceSelect, pDoc->m_lMarkedItem);
    switch (pDoc->m_lView)
      {
      case ViewSlots:
        {
        long ReqdVis=-1;
        list.DeleteAllItems();
        UpdateWindow();
        list.SetItemCount(pDoc->m_SlotCfgs.GetSize());
        for( int i=0; i<pDoc->m_SlotCfgs.GetSize(); i++)
          {
          // GROUP ??
          pDoc->m_SlotCfgs[i]->m_bInFilter=pDoc->m_SlotCfgs[i]->InFilter(pDoc->m_lDeviceSelect, pDoc->m_Selection);
          if (!pDoc->m_SlotCfgs[i]->m_bInFilter)
            {
            pDoc->m_IndexOfSlotInList[i]=-1;
            continue;
            }
          long ListIndex=InsertItem(pDoc->m_SlotCfgs[i]);
          pDoc->m_IndexOfSlotInList[i] = ListIndex;
          if (pDoc->m_lMarkedItem==ListIndex)
            ReqdVis=ListIndex;
          }

        if (m_lLastSlotTop>=0)
          {
          long i=m_lLastSlotTop+list.GetCountPerPage()-1;
          if (i>=list.GetItemCount())
            i=list.GetItemCount()-1;
          list.EnsureVisible(i, false);
          }
        if (ReqdVis>=0)
          {
          list.EnsureVisible(ReqdVis, false);
          //list.SetSelectionMark(ReqdVis);
          list.SetItemState(ReqdVis, LVIS_SELECTED, LVIS_SELECTED);
          }

        SetVisibleSlots(list.GetTopIndex(), list.GetCountPerPage()); 
        if (ReqdVis>=0)
          {
          //list.EnsureVisible(ReqdVis, false);
          //list.SetSelectionMark(ReqdVis);
          //list.SetItemState(ReqdVis, LVIS_SELECTED, LVIS_SELECTED);
          }
        break;
        }
      case ViewLinks:
        {
        long ReqdVis=-1;
        list.DeleteAllItems();
        UpdateWindow();
        list.SetItemCount(pDoc->m_LinkCfgs.GetSize());
        for( int i=0; i<pDoc->m_LinkCfgs.GetSize(); i++)
          {
          CLinkCfg &L=*(pDoc->m_LinkCfgs[i]);                                 //?? IS THE FIND DONE  HERE 
          // GROUP ??
          pDoc->m_LinkCfgs[i]->m_bInFilter=pDoc->m_LinkCfgs[i]->InFilter(pDoc->m_lDeviceSelect, pDoc->m_Selection);
          if (!pDoc->m_LinkCfgs[i]->m_bInFilter)
            {
            pDoc->m_IndexOfLinkInList[i]=-1;
            continue;
            }
          long ListIndex=InsertItem(pDoc->m_LinkCfgs[i]);
          pDoc->m_IndexOfLinkInList[i] = ListIndex;
          if (pDoc->m_lMarkedItem==ListIndex)
            ReqdVis=ListIndex;
          }
        if (m_lLastLinkTop>=0)
          {
          long i=m_lLastLinkTop+list.GetCountPerPage()-1;
          if (i>=list.GetItemCount())
            i=list.GetItemCount()-1;
          list.EnsureVisible(i, false);
          }
        if (ReqdVis>=0)
          {
          list.EnsureVisible(ReqdVis, false);
          list.SetItemState(ReqdVis, LVIS_SELECTED, LVIS_SELECTED);
          //list.SetSelectionMark(ReqdVis);
          }
        SetVisibleLinks(list.GetTopIndex(), list.GetCountPerPage()); 
        if (ReqdVis>=0)
          {
          //list.EnsureVisible(ReqdVis, false);
          //list.SetItemState(ReqdVis, LVIS_SELECTED, LVIS_SELECTED);
          //list.SetSelectionMark(ReqdVis);
          }
        break;
        }
      case ViewEdits:
        {
        list.DeleteAllItems();
        UpdateWindow();
        for( int i=0; i<pDoc->m_SlotEdits.GetSize(); i++)
          InsertItem(pDoc->m_SlotEdits[i]);
        break;
        }
      case ViewCdBlks:
        {
        list.DeleteAllItems();
        UpdateWindow();
        for( int i=0; i<pDoc->m_CdBlkCfgs.GetSize(); i++)
          InsertItem(pDoc->m_CdBlkCfgs[i]);
        break;
        }
      case ViewChanges:
        {
        list.DeleteAllItems();
        UpdateWindow();
        list.SetItemCount(pDoc->m_ChangeList.Count());
        CChangeItem *pChg=pDoc->m_ChangeList.Head();
        for( int i=0; pChg!=NULL; i++, pChg=pChg->m_pNext)
          if (pDoc->m_bChgLogAll || pChg->PeriperalIO()) 
            {
            bool DoIt=true;
            if (pChg->m_eSrc==eCSD_Slot)
              DoIt=pDoc->m_SlotCfgs[pChg->m_lSrcInx]->InFilter(pDoc->m_lDeviceSelect, pDoc->m_Selection);
            else if (pChg->m_eDst==eCSD_Slot)
              DoIt=pDoc->m_SlotCfgs[pChg->m_lDstInx]->InFilter(pDoc->m_lDeviceSelect, pDoc->m_Selection);
            else if (pChg->m_eSrc==eCSD_Link)
              DoIt=pDoc->m_LinkCfgs[pChg->m_lSrcInx]->InFilter(pDoc->m_lDeviceSelect, pDoc->m_Selection);
            else if (pChg->m_eDst==eCSD_Link)
              DoIt=pDoc->m_LinkCfgs[pChg->m_lDstInx]->InFilter(pDoc->m_lDeviceSelect, pDoc->m_Selection);
            if (DoIt)
              InsertItem(pChg);
            }
        if (pDoc->m_bChgLogTrack && list.GetItemCount())
          list.EnsureVisible(list.GetItemCount()-1, false);
        break;
        }
      }

    dbgpln("ListRedraw");   
    list.SetRedraw(true);
    //FixColumnWidths();
    }
  //else if (Hint==HINT_UPDATE_WIDTHS)
  //  FixColumnWidths();

  switch (Hint)
    {
    case HINT_UPDATE_ONESLOTVALUE:
      if (pDoc->m_lView==ViewSlots)
        SetRuntimeValues(pDoc->m_IndexOfSlotInList[HIndex], pDoc->m_SlotCfgs[HIndex]); 
      break;
    case HINT_UPDATE_ONELINKVALUE:
      if (pDoc->m_lView==ViewLinks)
        SetRuntimeValues(pDoc->m_IndexOfLinkInList[HIndex], pDoc->m_LinkCfgs[HIndex]); 
      break;
    case HINT_UPDATE_TRACKING:
      if (pDoc->m_bChgLogTrack && list.GetItemCount())
        list.EnsureVisible(list.GetItemCount()-1, false);
      break;
    case HINT_UPDATE_CHANGES:
      {
      CChangeBlock *pBlk=(CChangeBlock*)pHint;
      if (pBlk && pBlk->m_nChanges)
        {
        if (pDoc->m_lView==ViewChanges)
          {
          CString N;
          list.SetRedraw(false);
          long Top=list.GetTopIndex();
          long Last=list.GetItemCount();
          for (int i=0; i<pBlk->m_nChanges; i++)
            {
            CChangeItem * pChg=pBlk->m_Changes[i];
            pDoc->m_ChangeList.AddTail(pChg);
            if (pDoc->m_bChgLogAll || pChg->PeriperalIO())
              InsertItem(pChg);
            }
          while (pDoc->m_ChangeList.Count()>MaxHistoryListLen)
            {
            CChangeItem *pChg=pDoc->m_ChangeList.RemoveHead();
            if (pChg==(CChangeItem *)list.GetItemData(0))
              list.DeleteItem(0);
            delete pChg;
            }
          if (pDoc->m_bChgLogTrack)
            list.EnsureVisible(list.GetItemCount()-1, false);
          list.SetRedraw(true);
          }
        else
          {
          for (int i=0; i<pBlk->m_nChanges; i++)
            pDoc->m_ChangeList.AddTail(pBlk->m_Changes[i]);
          while (pDoc->m_ChangeList.Count()>MaxHistoryListLen)
            {
            CChangeItem *pChg=pDoc->m_ChangeList.RemoveHead();
            delete pChg;
            }
          }

        }

      delete pBlk;
      break;
      }
    }
  }

//-------------------------------------------------------------------

void CTagView::SetVisibleSlots(long Slot1, long Number)
  {
  //dbgpln("CTagView::SetVisibleSlots");
  CSysCADMarshalDoc *pDoc = GetDocument();
  CListCtrl     &list = GetListCtrl();
  const int MaxListItems = list.GetItemCount();

  if (Number>0 && pDoc->m_IndexOfSlotInList.GetSize())
    {
    long N=Min(Number, (long)MaxListItems);
    long *piSlots=new long[N];
    for (int i=0; i<N; i++)
      {
      piSlots[i]=((CSlotCfg*)list.GetItemData(Slot1+i))->m_lSlot;
      //dbgpln("  %4i) %4i", i, piSlots[i]);
      }
    gs_SlotMngr.SetVisibleItems(ViewSlots, true, N, piSlots);
    }
  else
    gs_SlotMngr.SetVisibleItems(ViewSlots, true, 0, 0);
  }                                                  

//-------------------------------------------------------------------

void CTagView::SetVisibleLinks(long Link1, long Number)
  {
  //dbgpln("CTagView::SetVisibleLinks");
  CSysCADMarshalDoc *pDoc = GetDocument();
  CListCtrl     &list = GetListCtrl();
  const int MaxListItems = list.GetItemCount();
  if (Number>0 && pDoc->m_IndexOfLinkInList.GetSize())
    {
    long N=Min(Number, (long)MaxListItems);
    long *piLinks=new long[N];
    for (int i=0; i<N; i++)
      {
      piLinks[i]=((CLinkCfg*)list.GetItemData(Link1+i))->m_lLink;
      //dbgpln("  %4i) %4i", i, piLinks[i]);
      }
    gs_SlotMngr.SetVisibleItems(ViewLinks, true, N, piLinks);
    }
  else
    gs_SlotMngr.SetVisibleItems(ViewLinks, true, 0, 0);

  }

//-------------------------------------------------------------------

void CTagView::OnDblclk(NMHDR *pNMHDR, LRESULT *pResult)
  {
  CSysCADMarshalDoc *pDoc = GetDocument();
  NMITEMACTIVATE *pNMI=(NMITEMACTIVATE*)pNMHDR;

  CListCtrl& list = GetListCtrl();
  POSITION Pos=list.GetFirstSelectedItemPosition();
  pDoc->m_lMarkedItem=list.GetNextSelectedItem(Pos);

  if (pDoc->m_lMarkedItem>=0)
    {
    int N=Max(Max(m_lColumn_SIMVAL, m_lColumn_DEVVAL), m_lColumn_CHGVAL);
    for (int i=1; i<=N; i++)
      {
      list.GetSubItemRect(pDoc->m_lMarkedItem, i, LVIR_BOUNDS, m_Rct);
      if (pNMI->ptAction.x>m_Rct.left && pNMI->ptAction.x<m_Rct.right)
        break;
      }
    //list.GetItemRect(pDoc->m_lMarkedItem, &m_Rct, LVIR_BOUNDS);
    //for (int i=0; i<m_lColumn_CHGVAL; i++)
    //  m_Rct.left+=list.GetColumnWidth(i);
    //m_Rct.right=m_Rct.left+list.GetColumnWidth(i);
    if (i<=N)
      {
      switch (pDoc->m_lView)
        {
        case ViewSlots:
          {
          CSlotCfg & S=*((CSlotCfg*)list.GetItemData(pDoc->m_lMarkedItem));
          if (pNMI->iSubItem==m_lColumn_TAG)
            m_pCfgDlg=CSlotCfgDlg::Start(S, this);
          else if (pNMI->iSubItem==m_lColumn_SIMVAL && S.m_pLink)
            {
            CLinkCfg & L=*S.m_pLink;
            m_pEdit=CValueEdit::Start(L, eCSD_Link, L.m_lLink, m_Rct, this);
            }
          else if (pNMI->iSubItem==m_lColumn_DEVVAL)
            m_pEdit=CValueEdit::Start(S, eCSD_Slot, S.m_lSlot, m_Rct, this);
          else if (pNMI->iSubItem==m_lColumn_TYPE)
            gs_SlotMngr.SetHold(eCSD_Slot, S.m_lSlot, !S.m_State.m_bHold);
          break;
          }
        case ViewLinks:
          {
          CLinkCfg & L=*((CLinkCfg*)list.GetItemData(pDoc->m_lMarkedItem));
          if (pNMI->iSubItem==m_lColumn_TAG)
            {  }
          else if (pNMI->iSubItem==m_lColumn_SIMVAL)
            m_pEdit=CValueEdit::Start(L, eCSD_Link, L.m_lLink, m_Rct, this);
          else if (pNMI->iSubItem==m_lColumn_DEVVAL)
            {
            CSlotCfg &S=*L.m_pSlot;
            m_pEdit=CValueEdit::Start(S, eCSD_Slot, S.m_lSlot, m_Rct, this);
            }
          else if (pNMI->iSubItem==m_lColumn_TYPE)
            gs_SlotMngr.SetHold(eCSD_Link, L.m_lLink, !L.m_State.m_bHold);
          break;
          }
        case ViewEdits:
          {
          CSlotCfgStrings & S=*((CSlotCfgStrings*)list.GetItemData(pDoc->m_lMarkedItem));
          if (pNMI->iSubItem>=0)
            m_pCfgDlg=CSlotCfgDlg::Start(S, this);
          break;
          }
        }
      }
    }

  *pResult = 0;
  }

//-------------------------------------------------------------------

void CTagView::OnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
  {
  //CSysCADMarshalDoc *pDoc = GetDocument();
  //NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
  //CListView::OnItemchanged(pNMHDR, pResult)
  //FixVisibleItems();

  //*pResult = 0;
  }

//-------------------------------------------------------------------

// Column sorting proc
int CALLBACK CompareTagsSlots( long I1, long I2, LPARAM lCol )
  {
  int nCmp = 0;
  CSlotCfg *S1=(CSlotCfg*)I1;
  CSlotCfg *S2=(CSlotCfg*)I2;
  switch( lCol )
    {
    case 1:   nCmp = S1->m_lSlot-S2->m_lSlot;                              break;
    case 2:   nCmp = S1->m_sTag.CompareNoCase(S2->m_sTag);                 break; 
    case 4:   nCmp = S1->m_sOPCTag.CompareNoCase(S2->m_sOPCTag);           break; 
    case 14:  nCmp = S1->m_sDesc.CompareNoCase(S2->m_sDesc);               break; 
      //    case 3: nCmp = S1->Type()-S2->Type();                                break;
      //    //case m_lColumn_DIRN:  break;
      //    //case m_lColumn_VALUE: break;
      //    //case m_lColumn_TIME:  
      //    //  {
      //    //  COleDateTime t1(S1->m_ftTimeStamp);
      //    //  COleDateTime t2(S2->m_ftTimeStamp);
      //    //  COleDateTimeSpan ts=t1-t2;
      //    //  double d=ts;
      //    //  if (d <0) nCmp=-1;
      //    //  else if (d>0) nCmp=1;
      //    //  break;
      //    //  }
      //    //case m_lColumn_CHGCNT:nCmp = S1->m_lValueChanges-S2->m_lValueChanges;              break;
      //    //case m_lColumn_TRANS: nCmp = S1->m_dwTransactionID-S2->m_dwTransactionID;          break;
      //    case m_lColumn_ERROR: break;                                                       
      //    case m_lColumn_DESC:  nCmp = S1->m_sDesc.CompareNoCase(S2->m_sDesc);               break;
      //    case m_lColumn_CONN:  nCmp = S1->m_sConnects.CompareNoCase(S2->m_sConnects);       break;
    }
  if(lastCol == lCol)  // reverse order
    nCmp = -nCmp;
  return nCmp;
  }

//-------------------------------------------------------------------

// Column sorting proc
int CALLBACK CompareTagsLinks( long I1, long I2, LPARAM lCol )
  {
  int nCmp = 0;
  CLinkCfg *S1=(CLinkCfg*)I1;
  CLinkCfg *S2=(CLinkCfg*)I2;
  switch( lCol )
    {
    case 1:   nCmp = S1->m_lLink-S2->m_lLink;                             break;
    case 2:   nCmp = S1->m_sTag.CompareNoCase(S2->m_sTag);                break; 
    case 3:   nCmp = S1->m_sSimTag.CompareNoCase(S2->m_sSimTag);          break; 
    case 14:  nCmp = S1->m_sDesc.CompareNoCase(S2->m_sDesc);              break; 
      //case 4:   nCmp = S1->m_sOPSimTag.CompareNoCase(S2->m_sSimTag);          break; 
      //case m_lColumn_TYPE:  nCmp = S1->Type()-S2->Type();                               break;
      //case m_lColumn_DIRN:  break;
      //case m_lColumn_VALUE: break;
      //case m_lColumn_TIME:  
      //  {
      //  COleDateTime t1(S1->m_ftTimeStamp);
      //  COleDateTime t2(S2->m_ftTimeStamp);
      //  COleDateTimeSpan ts=t1-t2;
      //  double d=ts;
      //  if (d <0) nCmp=-1;
      //  else if (d>0) nCmp=1;
      //  break;
      //  }
      //case m_lColumn_CHGCNT:nCmp = S1->m_lValueChanges-S2->m_lValueChanges;             break;
      //case m_lColumn_TRANS: nCmp = S1->m_dwTransactionID-S2->m_dwTransactionID;         break;
      //case m_lColumn_ERROR: break;
      //case m_lColumn_CONN:  nCmp = S1->m_sConnectTag.CompareNoCase(S2->m_sConnectTag);  break; 
    }
  if(lastCol == lCol)  // reverse order
    nCmp = -nCmp;
  return nCmp;
  }
// sort the list based on the column clicked
// sort the list based on the column clicked

// reverse the order of sorting if the same col is clicked again
void CTagView::OnColumnclick(NMHDR *pNMHDR, LRESULT *pResult)
  {
  NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
  CListCtrl& list = GetListCtrl();
  list.SetRedraw( FALSE );
  CSysCADMarshalDoc *pDoc = GetDocument();

  int iCmp=-1;
  int iSub=pNMListView->iSubItem;
  if      (iSub==m_lColumn_NO    )   iCmp = 1;
  else if (iSub==m_lColumn_TAG   )   iCmp = 2;
  else if (iSub==m_lColumn_SIMTAG)   iCmp = 3;
  else if (iSub==m_lColumn_DEVTAG)   iCmp = 4;
  else if (iSub==m_lColumn_TYPE  )   iCmp = 5;
  else if (iSub==m_lColumn_SIMVAL)   iCmp = 6;
  else if (iSub==m_lColumn_DEVVAL)   iCmp = 7;
  else if (iSub==m_lColumn_TIME  )   iCmp = 8;
  else if (iSub==m_lColumn_CHGCNT)   iCmp = 9;
  else if (iSub==m_lColumn_TRANS )   iCmp = 10;
  else if (iSub==m_lColumn_ERROR )   iCmp = 11;
  else if (iSub==m_lColumn_CONN  )   iCmp = 12;
  else if (iSub==m_lColumn_DESC  )   iCmp = 14;

  switch (pDoc->m_lView)
    {
    case ViewSlots: 
      {
      list.SortItems( (PFNLVCOMPARE)CompareTagsSlots, iCmp); 
      for (long l=0; l<list.GetItemCount(); l++)
        {
        CSlotCfg *pSlot=(CSlotCfg*)list.GetItemData(l);
        pDoc->m_IndexOfSlotInList[pSlot->m_lSlot]=l;
        }
      }
    break;
    case ViewLinks: 
      {
      if (0)
        {
        for (long l=0; l<list.GetItemCount(); l++)
          dbgpln(">>IndexOfLinkInList[%4i]=%4i", l, pDoc->m_IndexOfLinkInList[l]);
        }
      list.SortItems( (PFNLVCOMPARE)CompareTagsLinks, iCmp); 
      for (long l=0; l<list.GetItemCount(); l++)
        {
        CLinkCfg*pLink=(CLinkCfg*)list.GetItemData(l);
        pDoc->m_IndexOfLinkInList[pLink->m_lLink]=l;
        }
      if (0)
        {
        for (long l=0; l<list.GetItemCount(); l++)
          dbgpln("  IndexOfLinkInList[%4i]=%4i", l, pDoc->m_IndexOfLinkInList[l]);
        }
      break;
      }
    }
  list.SetRedraw( TRUE );
  FixVisibleItems();

  if( lastCol != iCmp)
    lastCol = iCmp;
  else
    lastCol = 555;
  *pResult = 0;
  }

//-------------------------------------------------------------------

void CTagView::OnTimer(UINT nIDEvent)
  {
  }

void CTagView::FixColumnWidths()
  {
  //  CSysCADMarshalDoc *pDoc = GetDocument();
  //  CListCtrl     &list = GetListCtrl();
  //
  //  TmpColWidth[m_lColumn_TYPE  ]=list.GetColumnWidth(m_lColumn_TYPE  );
  //  //TmpColWidth[m_lColumn_DIRN  ]=list.GetColumnWidth(m_lColumn_DIRN  );
  //  TmpColWidth[m_lColumn_CHGCNT]=list.GetColumnWidth(m_lColumn_CHGCNT);
  //  TmpColWidth[m_lColumn_TRANS ]=list.GetColumnWidth(m_lColumn_TRANS );
  //  TmpColWidth[m_lColumn_CONN  ]=list.GetColumnWidth(m_lColumn_CONN  );
  //
  //
  //  if (TmpColWidth[m_lColumn_TYPE  ])  ColWidth[m_lColumn_TYPE  ]  = TmpColWidth[m_lColumn_TYPE  ];
  //  //if (TmpColWidth[m_lColumn_DIRN  ])  ColWidth[m_lColumn_DIRN  ]  = TmpColWidth[m_lColumn_DIRN  ];
  //  if (TmpColWidth[m_lColumn_CHGCNT])  ColWidth[m_lColumn_CHGCNT]  = TmpColWidth[m_lColumn_CHGCNT];
  //  if (TmpColWidth[m_lColumn_TRANS ])  ColWidth[m_lColumn_TRANS ]  = TmpColWidth[m_lColumn_TRANS ];
  //  if (TmpColWidth[m_lColumn_CONN  ])  ColWidth[m_lColumn_CONN  ]  = TmpColWidth[m_lColumn_CONN  ];
  //
  //  bool Cfg=pDoc->m_bViewCfg;
  //  bool Sts=pDoc->m_bViewStats;
  //
  //  list.SetColumnWidth(m_lColumn_TYPE,    Cfg ? ColWidth[m_lColumn_TYPE  ] : 0);
  //  //list.SetColumnWidth(m_lColumn_DIRN,    Cfg ? ColWidth[m_lColumn_DIRN  ] : 0);
  //  list.SetColumnWidth(m_lColumn_CHGCNT,  Sts ? ColWidth[m_lColumn_CHGCNT] : 0);
  //  list.SetColumnWidth(m_lColumn_TRANS,   Sts ? ColWidth[m_lColumn_TRANS ] : 0);
  //  list.SetColumnWidth(m_lColumn_CONN,    Cfg ? ColWidth[m_lColumn_CONN  ] : 0);
  }

void CTagView::FixVisibleItems() 
  {
  CSysCADMarshalDoc *pDoc = GetDocument();
  CListCtrl     &list = GetListCtrl();
  dbgpln("ListSize:%i",  list.GetItemCount());
  if (list.GetItemCount()>0)
    {
    switch (pDoc->m_lView)
      {
      case ViewSlots:
        {
        m_lLastSlotTop=list.GetTopIndex(); 
        SetVisibleSlots(list.GetTopIndex(), list.GetCountPerPage());
        break;
        }
      case ViewLinks:
        {
        m_lLastLinkTop=list.GetTopIndex(); 
        SetVisibleLinks(list.GetTopIndex(), list.GetCountPerPage());
        break;
        }
      };
    }
  }

void CTagView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
  {
  if (nSBCode==SB_ENDSCROLL)
    {
    FixVisibleItems();
    }

  CListView::OnVScroll(nSBCode, nPos, pScrollBar);
  }

void CTagView::OnSize(UINT nType, int cx, int cy) 
  {
  CListView::OnSize(nType, cx, cy);

  FixVisibleItems();
  }

void CTagView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
  {
  CListView::OnKeyDown(nChar, nRepCnt, nFlags);
  FixVisibleItems();
  };

void CTagView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
  {
  CListView::OnKeyUp(nChar, nRepCnt, nFlags);
  //FixVisibleItems();
  };

void CTagView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
  {
  CListView::OnChar(nChar, nRepCnt, nFlags);
  };

void CTagView::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
  {
  CSysCADMarshalDoc *pDoc = GetDocument();
  NMITEMACTIVATE *pNMI=(NMITEMACTIVATE*)pNMHDR;

  CListCtrl& list = GetListCtrl();
  POSITION Pos=list.GetFirstSelectedItemPosition();
  long lItem=list.GetNextSelectedItem(Pos);
  CRect Rct;
  GetWindowRect(&Rct);
  int CopyWhat=0;
  //CString ClipCopy;
  if (lItem>=0)
    {
    switch (pDoc->m_lView)
      {
      case ViewSlots:
        {
        CSlotCfg & S=*((CSlotCfg*)list.GetItemData(lItem));
        CString GotoLinkString;
        if (S.m_pLink)
          GotoLinkString.Format("&Goto Link %i", S.m_pLink->m_lLink);
        else
          GotoLinkString="&Goto Link -";

        CMenu Menu;
        Menu.CreatePopupMenu();
        Menu.AppendMenu(MF_STRING, 100+pNMI->iSubItem, "&Find Slot");
        Menu.AppendMenu(MF_STRING|(S.m_pLink==NULL?MF_GRAYED:0), 130, GotoLinkString);
        Menu.AppendMenu(MF_SEPARATOR);
        Menu.AppendMenu(MF_STRING, 141, "Copy Selected");
        Menu.AppendMenu(MF_STRING, 142, "Copy All");
        //Menu.AppendMenu(MF_STRING, 103, "&xGoto Link");
        int MenuI=Menu.TrackPopupMenu(TPM_RETURNCMD|TPM_LEFTALIGN|TPM_RIGHTBUTTON, Rct.left+pNMI->ptAction.x,Rct.top+pNMI->ptAction.y, this);///*(CWnd*)*/&View());
        Menu.DestroyMenu();

        switch (MenuI)
          {
          case 130:
            {
            pDoc->m_lView=ViewLinks;
            //pDoc->m_lDeviceSelect=-1;//S.m_pLink->m_lLink;
            pDoc->m_lMarkedItem=S.m_pLink->m_lLink;

            pDoc->UpdateAllViews(NULL, MakeHint(HINT_CHANGE_VIEW));
            pDoc->UpdateAllViews(NULL, MakeHint(HINT_UPDATE_TAGS));
            break;
            }
          case 141:
          case 142:
            {
            CopyWhat=MenuI-140;
            break;
            }
          }
        int xxx=0;
        break;
        }
      case ViewLinks:
        {
        CLinkCfg & L=*((CLinkCfg*)list.GetItemData(lItem));
        CString GotoSlotString;
        GotoSlotString.Format("&Goto Slot %i", L.m_pSlot->m_lSlot);

        CMenu Menu;
        Menu.CreatePopupMenu();
        Menu.AppendMenu(MF_STRING, 101, "&Find Link");
        Menu.AppendMenu(MF_STRING, 102, GotoSlotString);
        Menu.AppendMenu(MF_SEPARATOR);
        Menu.AppendMenu(MF_STRING, 141, "Copy Selected");
        Menu.AppendMenu(MF_STRING, 142, "Copy All");
        int MenuI=Menu.TrackPopupMenu(TPM_RETURNCMD|TPM_LEFTALIGN|TPM_RIGHTBUTTON, Rct.left+pNMI->ptAction.x,Rct.top+pNMI->ptAction.y, this);///*(CWnd*)*/&View());
        Menu.DestroyMenu();
        switch (MenuI)
          {
          case 102:
            {
            pDoc->m_lView=ViewSlots;
            //pDoc->m_lDeviceSelect=-1;//L.m_pSlot->m_lSlot;//-1;
            pDoc->m_lMarkedItem=L.m_pSlot->m_lSlot;

            pDoc->UpdateAllViews(NULL, MakeHint(HINT_CHANGE_VIEW));
            pDoc->UpdateAllViews(NULL, MakeHint(HINT_UPDATE_TAGS));
            break;
            }
          case 141:
          case 142:
            CopyWhat=MenuI-140;
            break;
          }

        int xxx=0;
        break;
        }
      case ViewCdBlks:
      case ViewChanges:
        {
        CMenu Menu;
        Menu.CreatePopupMenu();
        Menu.AppendMenu(MF_STRING, 141, "Copy Selected");
        Menu.AppendMenu(MF_STRING, 142, "Copy All");
        int MenuI=Menu.TrackPopupMenu(TPM_RETURNCMD|TPM_LEFTALIGN|TPM_RIGHTBUTTON, Rct.left+pNMI->ptAction.x,Rct.top+pNMI->ptAction.y, this);///*(CWnd*)*/&View());
        Menu.DestroyMenu();
        switch (MenuI)
          {
          case 141:
          case 142:
            CopyWhat=MenuI-140;
            break;
          }
        break;
        }
      }
    }
  
  if (CopyWhat)
    {
    CString ClipString;
    char buff[2048], itmbuff[256];
    strcpy(buff, "");
    for (int c=0; c<list.GetHeaderCtrl()->GetItemCount(); c++)
      {
      LVCOLUMN col;
      col.iSubItem=c;
      col.mask=LVCF_TEXT;
      col.pszText=itmbuff;
      list.GetColumn(c, &col);
      if (c>0)
        strcat(buff, "\t");
      strcat(buff, col.pszText);
      }
    strcat(buff, "\r\n");
    ClipString+=buff;
    bool DoSel=CopyWhat==1;
    int i=-1;
    POSITION Pos;
    if (DoSel)
      Pos=list.GetFirstSelectedItemPosition();
    for (;;)
      {
      if (DoSel)
        {
        if (Pos)
          i=list.GetNextSelectedItem(Pos);
        else
          break;
        }
      else
        {
        i++;
        if (i>=list.GetItemCount())
          break;
        }
      strcpy(buff, "");
      for (int c=0; c<list.GetHeaderCtrl()->GetItemCount(); c++)
        {
        if (c>0)
          strcat(buff, "\t");
        strcat(buff, list.GetItemText(i,c));//itm.pszText);
        }
      strcat(buff, "\r\n");
      ClipString+=buff;
      }

    CopyTextToClipboard(this, (LPTSTR)(LPCTSTR)ClipString);

    int xxx=0;
    }
  *pResult = 0;
  }
