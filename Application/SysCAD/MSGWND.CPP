//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#include "stdafx.h"

#define __MSGWND_CPP

#include "sc_defs.h"
#include "resource.h"
//#include "gpwfuncs.h"
#include "msgwnd.h"
#include "errorlog.h"
#include "project.h"
#include "accnode.h"
#include "scd_wm.h"
#include "dlgbusy.h"
#include "scdctrls.h"
#include "executiv.h"
#include "cmd_mngr.h"
#include "archmngr.h"
#include "svcconn.h"

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

const int ButtonIdsB[] = {IDC_CLOSE, IDC_CLEARALL, IDC_MSGSON, IDC_CNDSON, IDC_SHOWWHAT, IDC_MSGSOUND};//, IDC_STATICOPTS, IDC_STATICSHOW};
const int CMsgDlgNButtonsB=sizeof(ButtonIdsB)/sizeof(ButtonIdsB[0]);

#define SetWndDef(h) ::SetWindowLong(h, GWL_STYLE, ::GetWindowLong(h, GWL_STYLE) | BS_DEFPUSHBUTTON);
#define ClrWndDef(h) ::SetWindowLong(h, GWL_STYLE, ::GetWindowLong(h, GWL_STYLE) & ~BS_DEFPUSHBUTTON);

const int TWd0=60;
const int TWd1=30;
const int TWd1a=4;
const int TWd2=80;

//===========================================================================

const int MsgListBoxLen = 2000;

//static long lMsgItem_Cnt=0;

class CMsgItem
  {
  public:
    int   iImg;
    DWORD dwFlags;
    long  m_lSeqNo;
    Strng m_sIter, m_sID, m_sSeq, m_sCall, m_sSrc, m_sTxt;
    flag  m_fIncludeInCnt;

    CMsgItem()
      {
      iImg=0;
      dwFlags=0;
      m_lSeqNo=0;
      m_fIncludeInCnt=true;

      //dbgpln("CMsgItem  %i", ++lMsgItem_Cnt);
      }
    ~CMsgItem()
      {
      //dbgpln("~CMsgItem  %i", lMsgItem_Cnt--);
      }
  DEFINE_SPARES(CMsgItem)
  };
IMPLEMENT_SPARES(CMsgItem, MsgListBoxLen+100)

typedef CSCDList <CMsgItem*, CMsgItem*> CMsgList;
typedef CSCDPtrListIterWithPos<CMsgList, CMsgItem*> CMsgListIter;

//===========================================================================
//
//
//
//===========================================================================

class CMsgWnd;
class CMsgDlg;
class CMsgFrm;
class CMsgCommon
  {
  public:
    int             m_iShowWhat;
    CMsgList        m_MsgList, m_CndList;
    //CDialog         *m_pMsgWnd, *m_pMsgDlg;
    CMsgWnd        *m_pMsgWnd;
    CMsgDlg        *m_pMsgDlg;
    CMsgFrm        *m_pMsgFrm;

    flag            m_bMsgCountsOK;

    long            m_CndListCnt;
    long            m_MsgListCnt;
    long            m_nErrorMsgs;
    long            m_nWarningMsgs;
    long            m_nNoteMsgs;

    CMsgCommon();
    flag ItemVisible(CMsgItem &It);
    flag DoRButtonMenu(int Cmd);
    int IndexAndAccessableTag(flag TagRqd, CCustomListCtrl& Ctrl, CString& sTag);
    void PopupListMenu(CCustomListCtrl& Ctrl, CMsgList& List, CWnd * pWnd);
  
    void SetMsgsIncludeFlag(flag On);
    void CountMsgs(); 
    long NErrorMsgs();
    long NWarningMsgs();
    long NNoteMsgs();

  };

static CMsgCommon Shared;

//===========================================================================
//
//
//
//===========================================================================

class CMsgWnd : public CFormView
  {
  friend class CMsgFrm;
  public:
	//CMsgWnd(CWnd* pParent = NULL);   // standard constructor

  protected:
    CMsgWnd();           // protected constructor used by dynamic creation
    DECLARE_DYNCREATE(CMsgWnd)

    CImageList m_ImgList;           //bitmap images used in list boxes
    int        m_iSplitPos;         //position of split between list boxes
    int        m_iTrackSplitDiff;   //used while changing split
    flag       m_bAdjustingSplit;   //is split between list boxes being changed
    
    float      m_fErrFrac;
    flag       m_bIgnore;
    flag       m_bInited;
    
    HWND       hBB[CMsgDlgNButtonsB];
    CRect      WinRect, ClntRect, ErrRect, CndRect, TabRect;
    CRect      BRect[CMsgDlgNButtonsB];
    CRect      BBtns;
    LONG       XGap, YGap, TGap, BGap;
    LONG       TabLMrg, TabRMrg;
    int        m_MsgOrigHeight, m_MsgOrigWidth;
  public:
    //{{AFX_DATA(CMsgWnd)
	  enum { IDD = IDD_MSGWND };
	  CCustomListCtrl	m_MsgList;
	  CCustomListCtrl	m_MsgCtrl;
	  BOOL	m_DisplayOn;
	  BOOL	m_SoundOn;
	  BOOL	m_MsgsOn;
	  BOOL	m_CndsOn;
	//}}AFX_DATA
  public:
    inline void SetSplitPos(int SplitPos) { m_iSplitPos = SplitPos; SetListHeights(); };
    inline int GetSplitPos() { return m_iSplitPos; };
    void UpdateWindowText();
    //{{AFX_VIRTUAL(CMsgWnd)
  public:
    virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
  	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
    //}}AFX_VIRTUAL
  protected:
    void MeasureClientCoOrds();
    void ToggleSound(); 
    void ToggleDisplay(); 
    void SetListHeights(flag Centre=false);
    void DoLayout();
    virtual ~CMsgWnd();

    //{{AFX_MSG(CMsgWnd)
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnClose();
    afx_msg void OnClear();
    afx_msg void OnClearall();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnRclickErrlist(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnDblclkErrlist(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnRclickCondlist(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnDblclkCondlist(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnSelendokShowwhat();
    afx_msg void OnContinue();
    afx_msg void OnIgnore();
    afx_msg void OnCndson();
    afx_msg void OnMsgson();
    afx_msg void OnSound();
    //}}AFX_MSG
    afx_msg LRESULT OnLogMsg(WPARAM wParam, LPARAM lParam);
    afx_msg void OnUpdateBtn(CCmdUI* pCmdUi);
    afx_msg void OnMsgMenu(UINT id);
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================
//
//
//
//===========================================================================

static void ClearListAll(CCustomListCtrl& Ctrl, CMsgList & List, flag PostMsg)
  {
  Ctrl.LockWindowUpdate();
  Ctrl.DeleteAllItems();
  while (!List.IsEmpty())
    delete List.RemoveHead();
  //List.RemoveAllToFreeList();
  Shared.m_CndListCnt=-1;
  Shared.m_MsgListCnt=-1;

  if (PostMsg)
    ScdMainWnd()->PostMessage(WMU_UPDATEMSGMENUBTN, 0, 0);
  
  Shared.m_pMsgWnd->UpdateWindowText();
  Ctrl.UnlockWindowUpdate();
  }

static void ClearList(CCustomListCtrl& Ctrl, CMsgList & List, flag PostMsg)
  {
  Ctrl.LockWindowUpdate();
  int i = Ctrl.GetNextItem(-1, LVNI_SELECTED);
  while (i>=0)
    {
    POSITION Pos=(POSITION)Ctrl.GetItemData(i);
    delete List.GetAt(Pos);
    List.RemoveAt(Pos);
    Ctrl.DeleteItem(i);
    i = Ctrl.GetNextItem(-1, LVNI_SELECTED);
    }
  //List.RemoveAllToFreeList();
  Shared.m_CndListCnt=-1;
  Shared.m_MsgListCnt=-1;

  if (PostMsg)
    ScdMainWnd()->PostMessage(WMU_UPDATEMSGMENUBTN, 0, 0);

  Shared.m_pMsgWnd->UpdateWindowText();
  Ctrl.UnlockWindowUpdate();
  }

//===========================================================================
//
//
//
//===========================================================================

IMPLEMENT_DYNCREATE(CMsgWnd, CFormView)

CMsgWnd::CMsgWnd()//CWnd* pParent )
	: CFormView(CMsgWnd::IDD)//, pParent)
  {
 
  //{{AFX_DATA_INIT(CMsgWnd)
	m_DisplayOn = false;
	m_MsgsOn = false;
	m_CndsOn = false;
	m_SoundOn = true;
	//}}AFX_DATA_INIT
  m_bAdjustingSplit = 0;
  m_iSplitPos = -1;
  m_fErrFrac = 0.67f;
  m_bIgnore=false;
 
  XGap=-1;
  YGap=-1;
  TGap=-1;
  BGap=-1;
  m_bInited=false;

  }

//---------------------------------------------------------------------------

CMsgWnd::~CMsgWnd()
  {
  }

//---------------------------------------------------------------------------

void CMsgWnd::DoDataExchange(CDataExchange* pDX)
  {
	CFormView::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CMsgWnd)
	DDX_Control(pDX, IDC_CONDLIST, m_MsgList);
	DDX_Control(pDX, IDC_MSGLIST, m_MsgCtrl);
	DDX_Check(pDX, IDC_MSGSON, m_MsgsOn);
	DDX_Check(pDX, IDC_CNDSON, m_CndsOn);
	DDX_Check(pDX, IDC_MSGSOUND, m_SoundOn);
	//}}AFX_DATA_MAP
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CMsgWnd, CFormView)
  //{{AFX_MSG_MAP(CMsgWnd)
  ON_WM_SIZE()
  ON_BN_CLICKED(IDC_CLOSE, OnClose)
  ON_BN_CLICKED(IDC_CLEARALL, OnClearall)
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_LBUTTONDBLCLK()
  ON_NOTIFY(NM_RCLICK, IDC_MSGLIST, OnRclickErrlist)
  ON_NOTIFY(NM_DBLCLK, IDC_MSGLIST, OnDblclkErrlist)
  ON_NOTIFY(NM_RCLICK, IDC_CONDLIST, OnRclickCondlist)
  ON_NOTIFY(NM_DBLCLK, IDC_CONDLIST, OnDblclkCondlist)
  ON_WM_RBUTTONDOWN()
	ON_CBN_SELENDOK(IDC_SHOWWHAT, OnSelendokShowwhat)
	ON_BN_CLICKED(IDC_CONTINUE, OnContinue)
	ON_BN_CLICKED(IDC_IGNORE, OnIgnore)
	ON_BN_CLICKED(IDC_CNDSON, OnCndson)
	ON_BN_CLICKED(IDC_MSGSON, OnMsgson)
	ON_WM_MOVE()
	ON_WM_GETMINMAXINFO()
	ON_WM_WINDOWPOSCHANGING()
	ON_BN_CLICKED(IDC_MSGSOUND, OnSound)
	//}}AFX_MSG_MAP
  ON_MESSAGE(WMU_LOGMSG, OnLogMsg)
  ON_UPDATE_COMMAND_UI_RANGE(IDC_CLEAR, IDC_CLEAR, OnUpdateBtn)
  //ON_COMMAND_RANGE(IDM_MSG_ACCESS, IDM_MSG_CLEARLINE, OnMenu)
  ON_COMMAND_RANGE(IDM_MSG_CLEARALL, IDM_MSG_CENTRE, OnMsgMenu)
END_MESSAGE_MAP()
      
//---------------------------------------------------------------------------
       
BOOL CMsgWnd::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
  {
  if (CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext))
    {
    m_bInited=true;
    CBitmap BM;
    BM.LoadBitmap(IDB_ERRORIMGS2);
    BOOL b=m_ImgList.Create(16, 12, false, 0, 10);
    m_ImgList.Add(&BM, (CBitmap*)NULL);

    CCustomListCtrl &MsgCtrl = *(CCustomListCtrl*)GetDlgItem(IDC_MSGLIST);
    CCustomListCtrl &CndCtrl = *(CCustomListCtrl*)GetDlgItem(IDC_CONDLIST);

    MsgCtrl.InsertColumn(0, "Source",     LVCFMT_LEFT, 120);
    MsgCtrl.InsertColumn(1, "ID",         LVCFMT_RIGHT, 40);
    MsgCtrl.InsertColumn(2, "Iter",       LVCFMT_RIGHT, 4/*40*/);
    MsgCtrl.InsertColumn(3, "Seq",        LVCFMT_RIGHT, 4/*32*/);
    MsgCtrl.InsertColumn(4, "Call",       LVCFMT_RIGHT, 4/*32*/);
    MsgCtrl.InsertColumn(5, "Message",    LVCFMT_LEFT, 180);
    MsgCtrl.SetImageList(&m_ImgList ,     LVSIL_SMALL);
    MsgCtrl.SetExtendedStyle(MsgCtrl.GetExtendedStyle()|LVS_EX_FULLROWSELECT);

    CndCtrl.InsertColumn(0, "Source",     LVCFMT_LEFT, 120);
    CndCtrl.InsertColumn(1, "ID",         LVCFMT_RIGHT, 40);
    CndCtrl.InsertColumn(2, "Iter",       LVCFMT_RIGHT, 4/*40*/);
    CndCtrl.InsertColumn(3, "Seq",        LVCFMT_RIGHT, 4/*32*/);
    CndCtrl.InsertColumn(4, "Call",       LVCFMT_RIGHT, 4/*32*/);
    CndCtrl.InsertColumn(5, "Condition",  LVCFMT_LEFT, 180);
    CndCtrl.SetImageList(&m_ImgList ,     LVSIL_SMALL);
    CndCtrl.SetExtendedStyle(CndCtrl.GetExtendedStyle()|LVS_EX_FULLROWSELECT);
    
    CComboBox & ShowWhat= *(CComboBox*)GetDlgItem(IDC_SHOWWHAT);
    ShowWhat.AddString("All Messages");
    ShowWhat.AddString("Warnings & Errors");
    ShowWhat.AddString("Errors Only");
    
    m_SoundOn = ScdPFUser.RdInt("Messages", "Sound", m_SoundOn);
    m_DisplayOn = ScdPFUser.RdInt("Messages", "Display", m_DisplayOn);
    Shared.m_iShowWhat = ScdPFUser.RdInt("Messages", "ShowWhat", MWShowNotes);//MWShowWarns);
  	m_MsgsOn = ScdPFUser.RdInt("Messages", "ShowMsgs", 1);
	  m_CndsOn = ScdPFUser.RdInt("Messages", "ShowCnds", 1);

    ShowWhat.SetCurSel(Shared.m_iShowWhat);

    CRect MsgOrigSize;
    MsgCtrl.GetWindowRect(&MsgOrigSize);
    m_MsgOrigHeight = MsgOrigSize.Height();
    m_MsgOrigWidth = MsgOrigSize.Width();
   
    return true;
    }
  return false;
  }

//---------------------------------------------------------------------------

void CMsgWnd::MeasureClientCoOrds()
  {
  BBtns.SetRectEmpty();
  for (int i=0; i<CMsgDlgNButtonsB; i++)
    {
    hBB[i] = ::GetDlgItem(GetSafeHwnd(), ButtonIdsB[i]);
    ::GetWindowRect(hBB[i], &BRect[i]);
    ScreenToClient(&BRect[i]);
    BBtns.UnionRect(&BBtns, &BRect[i]);
    }

  CCustomListCtrl &MsgCtrl = *(CCustomListCtrl*)GetDlgItem(IDC_MSGLIST);
  CCustomListCtrl &CndCtrl = *(CCustomListCtrl*)GetDlgItem(IDC_CONDLIST);
  
  MsgCtrl.GetWindowRect(&ErrRect);
  ScreenToClient(&ErrRect);
  CndCtrl.GetWindowRect(&CndRect);
  ScreenToClient(&CndRect);

  if (XGap<0)
    {
    XGap = ErrRect.left;
    YGap = ErrRect.top;
    TabLMrg=0;
    TabRMrg=0;
    }
  TGap = YGap;
  BGap = BBtns.Height()+2*YGap;
  };

//---------------------------------------------------------------------------

const int MinListLen = 50;

void CMsgWnd::SetListHeights(flag Centre)
  {
  MeasureClientCoOrds();
  const int LstWidth = ErrRect.Width();
  if (Centre)
    m_fErrFrac=0.5f;
  else if (m_iSplitPos<0)
    m_fErrFrac=0.67f;
  else
    m_fErrFrac=Range(0.1F, (m_iSplitPos-YGap)/float(ClntRect.Height()-(TGap+YGap+BGap)), 0.9F);

  DoLayout();
  }

//---------------------------------------------------------------------------

void CMsgWnd::DoLayout() 
  {
  if (!m_bInited)
    return;
  GetWindowRect(&WinRect);
  GetClientRect(&ClntRect);

  int cx=ClntRect.Width();
  int cy=ClntRect.Height();
    
  CCustomListCtrl &MsgCtrl = *(CCustomListCtrl*)GetDlgItem(IDC_MSGLIST);
  CCustomListCtrl &CndCtrl = *(CCustomListCtrl*)GetDlgItem(IDC_CONDLIST);

  MeasureClientCoOrds();
  int iLst=-1;
  if (m_MsgsOn && !m_CndsOn)
    iLst=0;
  else if (!m_MsgsOn && m_CndsOn)
    iLst=1;

  const int wndw = ClntRect.Width();
  const int wndh = ClntRect.Height();
  if (wndw>0)
    {
    int i;
    const int ErrH = ErrRect.Height();
    const int CndH = CndRect.Height();
    const int LstWidth = cx-2*XGap;
    int NewErrH;
    int NewCndH;
    int NewTabH;
    int NewCndY;
    int NewErrY;
    
    switch (iLst)
      {
      case 0:
        MsgCtrl.ShowWindow(SW_SHOW);
        CndCtrl.ShowWindow(SW_HIDE);
        NewErrH = (int)(cy-(TGap+BGap));
        NewErrY = TGap;
        NewCndH = 0;
        NewCndY = TGap;
        NewTabH = (int)(cy-(TGap+BGap));
        break;
      case 1:
        MsgCtrl.ShowWindow(SW_HIDE);
        CndCtrl.ShowWindow(SW_SHOW);
        NewErrH = 0;
        NewErrY = TGap;
        NewCndH = (int)(cy-(TGap+BGap));
        NewCndY = TGap;
        NewTabH = (int)(cy-(TGap+BGap));
        break;
      default:
        MsgCtrl.ShowWindow(SW_SHOW);
        CndCtrl.ShowWindow(SW_SHOW);
        NewErrH = (int)(m_fErrFrac*(cy-(TGap+YGap+BGap)));
        NewErrY = TGap;
        NewCndH = cy-(TGap+YGap+BGap)-NewErrH;
        NewCndY = TGap+NewErrH+YGap;
        NewTabH = (int)(cy-(TGap+BGap));
        m_iSplitPos = TGap+NewErrH+YGap/2;
        break;
      }

    MsgCtrl.SetWindowPos(this, XGap, NewErrY, LstWidth-TabRMrg, NewErrH, SWP_NOACTIVATE|SWP_NOZORDER);
    CndCtrl.SetWindowPos(this, XGap, NewCndY, LstWidth-TabRMrg, NewCndH, SWP_NOACTIVATE|SWP_NOZORDER);

    for (i=CMsgDlgNButtonsB-1; i>=0; i--)
      ::SetWindowPos(hBB[i], NULL, 
                          cx-TabRMrg-XGap-(BBtns.right-BRect[i].left), 
                          cy-BGap+YGap+(BRect[i].top-BBtns.top),
                          1, 1, SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOZORDER);

    for (i=0; i<CMsgDlgNButtonsB; i++)
      ::InvalidateRect(hBB[i], NULL, true);
    }
  }

//---------------------------------------------------------------------------

void CMsgWnd::OnSize(UINT nType, int cx, int cy) 
  {

  CFormView::OnSize(nType, cx, cy);

  if (nType==SIZE_RESTORED && cx>0 && cx<=4096 && cy<=4096)
    DoLayout();
  }

//---------------------------------------------------------------------------

static DWORD LastBeepCount=GetTickCount();
  
LRESULT CMsgWnd::OnLogMsg(WPARAM wParam, LPARAM lParam)
  {
  int MustSelect=0;
  flag bShowIt=0;
  flag bLogIt=0;
  byte mBeep=0;

  if (!m_bInited)
    return false;

  CCustomListCtrl &MsgCtrl = *(CCustomListCtrl*)GetDlgItem(IDC_MSGLIST);
  CCustomListCtrl &CndCtrl = *(CCustomListCtrl*)GetDlgItem(IDC_CONDLIST);

  // These Lock / Unlock pairs make 'other windows flash if syscad is running and minimised
  //MsgCtrl.LockWindowUpdate();
  //CndCtrl.LockWindowUpdate();

  int iNo[2]={-1,-1};
  int iWd[2]={-1,-1};        

  CMsgLogItem Msg;
  for (int NMsg=0; gs_MsgLog.GetWndQMsg(NMsg, Msg); NMsg++)
    {
    char * pWhat="";

    if (XBusy() && (Msg.m_dwFlags & (LogFlag_Fatal | LogFlag_Stop)))
      {
      //AfxMessageBox("SysCAD Stopping", MB_OK); 
      LogNote("SysCAD", 0, "Stopping"); 
      gs_Exec.GotoEditMode();
      }
    else if ((Msg.m_MsgBoxFlags!=0) && !RmtCmdBusy())
      {
      MustSelect=1;
      }

    flag AddMsg=1;
    int iImg=0;
    int iOff=(Msg.m_dwFlags & LogFlag_FromPGM) ? 3 : (Msg.m_dwFlags & LogFlag_FromCOM) ? 6 : 0;
    switch (Msg.m_dwFlags & (LogFlag_Fatal | LogFlag_Stop | LogFlag_Separator | 
                           LogFlag_Error | LogFlag_Warning | LogFlag_Note | 
                           LogFlag_Cmd | LogFlag_CondClear | LogFlag_RngClear))
      {
      case LogFlag_Fatal: 
        iImg=2+iOff;
        mBeep |= 2;
        pWhat= "Fatal";
        bShowIt=1;
        bLogIt=1;
        break;
      case LogFlag_Stop: 
        iImg=2+iOff;
        mBeep |= 2;
        pWhat= "Stop";
        bShowIt=1;
        bLogIt=1;
        break;
      case LogFlag_Error: 
        iImg=2+iOff;
        mBeep |= 2;
        pWhat= "Err";
        bShowIt=1;
        bLogIt=1;
        break;
      case LogFlag_Warning: 
        iImg=1+iOff;
        mBeep |= 1;
        pWhat="Wrn";
        bShowIt=1;
        bLogIt=1;
        break;
      case LogFlag_CondClear:
      case LogFlag_RngClear:
        if (Msg.m_lSeqNo!=0)
          {
          AddMsg=0;
          CMsgListIter Iter(Shared.m_CndList);
          for (CMsgItem *pIt=Iter.First(); pIt; pIt=Iter.Next())
            if (pIt->m_lSeqNo==Msg.m_lSeqNo)
              {
              delete Shared.m_CndList.GetAt(Iter.Pos());
              Shared.m_CndList.RemoveAt(Iter.Pos());
              break;
              }
         
          int iRemove=CndCtrl.GetItemCount();
          while (--iRemove>=0)
            if (CndCtrl.GetItemData(iRemove)==(DWORD)Iter.Pos())
              break;
          if (iRemove>=0)
            CndCtrl.DeleteItem(iRemove);
          else
            {
            pWhat="Clr";
            dbgpln("Missed Clear Cnd %i", Msg.m_lSeqNo);
            }
          }
        else
          pWhat="Clr";

        break;
      case LogFlag_Note:
        iImg  = 0+iOff;
        pWhat = "";
        bLogIt=1;
        break;
      case LogFlag_Separator:
        iImg  = 6;
        pWhat = "";
        break;
      case LogFlag_Cmd:
        iImg  = 0+iOff;
        pWhat = "";
        break;
      default:
        AddMsg=0;
      }
    if (Msg.m_dwFlags & LogFlag_Rng)
      iImg=9;

    if ((Msg.m_dwFlags & LogFlag_ClearErrList))
      {
      //CNM Message gets lost
      //        if ((Flags & LogFlag_ClearErrList))
      //          pMsgCtrl->DeleteAllItems();
      }
    if ((Msg.m_dwFlags & LogFlag_ClearCondList))
      {
      if ((Msg.m_dwFlags & LogFlag_ClearCondList))
        ClearList(CndCtrl, Shared.m_CndList, true);
      }
    else
      {
      if (1 || (Msg.m_dwFlags & LogFlag_AutoClear))
        {
        for (int j=0; j<2; j++)
          {
          CListCtrl* pCtrl=j ? &CndCtrl : &MsgCtrl;
          CMsgList * pList=j ? &Shared.m_CndList : &Shared.m_MsgList;
          while (pList->GetCount()>=MsgListBoxLen)
            {
            POSITION Pos=pList->GetHeadPosition();
            delete pList->RemoveHead();
            if ((pCtrl->GetItemCount()>0) && ((POSITION)pCtrl->GetItemData(0)==Pos))
              pCtrl->DeleteItem(0);
            }
          }  
        }
      if (AddMsg)
        {
        flag IsCond = ((Msg.m_dwFlags & (LogFlag_Cond|LogFlag_Rng))!=0);
        CListCtrl* pCtrl=IsCond ? &CndCtrl : &MsgCtrl;
        CMsgList * pList=IsCond ? &Shared.m_CndList : &Shared.m_MsgList;

        int iCtrl=IsCond ? 0 : 1;
        for (char *p=(char*)Msg.Description(); *p; p++)
          if (*p<' ')
           *p=' ';

        CMsgItem *pIt=new CMsgItem;
        pIt->dwFlags=Msg.m_dwFlags;
        pIt->m_lSeqNo=Msg.m_lSeqNo;
        pIt->iImg=iImg;
        pIt->m_sID.Set("%i", Msg.m_lIDNo);
        pIt->m_sIter.Set(Msg.m_lIterNo>0 ? "%i":"", Msg.m_lIterNo);
        pIt->m_sCall.Set(Msg.m_lCallNo>0 && (pIt->dwFlags&LogFlag_FromCOM)? "%i":"", Msg.m_lCallNo);
        pIt->m_sSeq.Set(Msg.m_lSeqNo>0?"%i":"", Msg.m_lSeqNo);
        pIt->m_sSrc=Msg.Source();
        pIt->m_sTxt=Msg.Description();
        POSITION Pos=pList->AddTail(pIt);

        if (Shared.ItemVisible(*pIt))
          {
          int n=pCtrl->InsertItem(pCtrl->GetItemCount(), LPSTR_TEXTCALLBACK , I_IMAGECALLBACK);
          pCtrl->SetItemText(n, 1, LPSTR_TEXTCALLBACK );
          pCtrl->SetItemText(n, 2, LPSTR_TEXTCALLBACK );
          pCtrl->SetItemText(n, 3, LPSTR_TEXTCALLBACK );
          pCtrl->SetItemText(n, 4, LPSTR_TEXTCALLBACK );
          pCtrl->SetItemText(n, 5, LPSTR_TEXTCALLBACK );
          pCtrl->SetItemData(n, (DWORD)Pos);//Msg.dwSeqNo);
          iNo[iCtrl]=n;
          iWd[iCtrl]=Max(iWd[iCtrl], pCtrl->GetStringWidth(Msg.Description())+20);
          }
        }
      }

    if (bLogIt)
      {
      if (gs_pArcMan)
        gs_pArcMan->LogAMessage(Msg);
#if SYSCAD10         
      if (gs_pPrj->SvcActive)
        gs_pPrj->Svc.LogMessage(Msg.m_dwFlags, Msg.Description());
#endif
      }
    }

  //// Clear Current Selections
  //POSITION pos;
  //pos = pMsgCtrl->GetFirstSelectedItemPosition();
  //while (pos)
  //  {
  //  int nItem = pMsgCtrl->GetNextSelectedItem(pos);
  //  pMsgCtrl->SetItemState(nItem, 0 , LVIS_SELECTED);
  //  }
  //// Clear Current Selections
  //pos = pCndCtrl->GetFirstSelectedItemPosition();
  //while (pos)
  //  {
  //  int nItem = pCndCtrl->GetNextSelectedItem(pos);
  //  pCndCtrl->SetItemState(nItem, 0 , LVIS_SELECTED);
  //  }
  
  if (iNo[0]>=0)
    {
    CndCtrl.EnsureVisible(iNo[0], false);
    CndCtrl.SetItemState(iNo[0], MustSelect ? -1 : 0 , LVIS_SELECTED);
    }
  if (iNo[1]>=0)
    {
    MsgCtrl.EnsureVisible(iNo[1], false);
    MsgCtrl.SetItemState(iNo[1], MustSelect ? -1 : 0 , LVIS_SELECTED);
    }
  if (iWd[0]>0)
    CndCtrl.SetColumnWidth(5, Max(iWd[0], CndCtrl.GetColumnWidth(5)));
  if (iWd[1]>0)
    MsgCtrl.SetColumnWidth(5, Max(iWd[1], MsgCtrl.GetColumnWidth(5)));

  if (m_SoundOn)
    {
    LastBeepCount=GetTickCount();
    if (mBeep & 2)
      {
      Beep(2000, 20);
      //MessageBeep(MB_ICONEXCLAMATION);
      }
    else if (mBeep & 1)
      {
      Beep(1000, 20);
      //MessageBeep(MB_ICONASTERISK);
      }
    }

  m_bIgnore=false;
  if (wParam)
    {
    ScdApp()->EndWaitCursor();
    ScdApp()->EndWaitCursor();
    m_bIgnore=(CMsgWindow::DoModalDlg()==IDOK);
    ScdApp()->BeginWaitCursor();
    ScdApp()->BeginWaitCursor();
    }
  else if (bShowIt && m_DisplayOn && ((wParam&LF_DoAfxMsgBox)==0))
    {
    BringWindowToTop();
    CMsgWindow::Show(true);
    }
  else
    {
    }

  //// FlashWindow(true);
  //MsgCtrl.UnlockWindowUpdate();
  //CndCtrl.UnlockWindowUpdate();

  ScdMainWnd()->PostMessage(WMU_UPDATEMSGMENUBTN, 0, 0);

  UpdateWindowText();


  return m_bIgnore;
  }

//---------------------------------------------------------------------------

void CMsgWnd::UpdateWindowText()
  {
  int nMsgs=Shared.m_MsgList.GetCount();
  int nCnds=Shared.m_CndList.GetCount();
  Strng S;
  S.Set("Messages:%i/%i", nMsgs, nCnds);
  GetParent()->SetWindowText(S());
  }

//---------------------------------------------------------------------------

void CMsgWnd::ToggleSound() 
  {
  m_SoundOn = !m_SoundOn;
  ScdPFUser.WrInt("Messages", "Sound", m_SoundOn);
  }

//---------------------------------------------------------------------------

void CMsgWnd::ToggleDisplay() 
  {
  m_DisplayOn = !m_DisplayOn;
  ScdPFUser.WrInt("Messages", "Display", m_DisplayOn);
  }

//---------------------------------------------------------------------------

void CMsgWnd::OnClear() 
  {
  CCustomListCtrl &CndCtrl = *(CCustomListCtrl*)GetDlgItem(IDC_CONDLIST);
  ClearList(CndCtrl, Shared.m_CndList, true);
  }

//---------------------------------------------------------------------------

void CMsgWnd::OnClearall() 
  {
  CCustomListCtrl &MsgCtrl = *(CCustomListCtrl*)GetDlgItem(IDC_MSGLIST);
  CCustomListCtrl &CndCtrl = *(CCustomListCtrl*)GetDlgItem(IDC_CONDLIST);

  ClearListAll(CndCtrl, Shared.m_CndList, false);
  ClearListAll(MsgCtrl, Shared.m_MsgList, true);
  }

//---------------------------------------------------------------------------

void CMsgWnd::OnClose() 
  {
  GetParent()->ShowWindow(SW_SHOWMINNOACTIVE);
  }

//---------------------------------------------------------------------------

void CMsgWnd::OnUpdateBtn(CCmdUI* pCmdUi)
  {
  CCustomListCtrl &MsgCtrl = *(CCustomListCtrl*)GetDlgItem(IDC_MSGLIST);
  CCustomListCtrl &CndCtrl = *(CCustomListCtrl*)GetDlgItem(IDC_CONDLIST);

  pCmdUi->Enable(MsgCtrl.GetNextItem(-1, LVNI_SELECTED)>=0 || CndCtrl.GetNextItem(-1, LVNI_SELECTED)>=0);
  }

//---------------------------------------------------------------------------

BOOL CMsgWnd::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
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
    case HDN_ENDTRACK: //column width changed
      {
      CCustomListCtrl &MsgCtrl = *(CCustomListCtrl*)GetDlgItem(IDC_MSGLIST);
      CCustomListCtrl &CndCtrl = *(CCustomListCtrl*)GetDlgItem(IDC_CONDLIST);

      flag IsCond=(::GetParent(pNM->hdr.hwndFrom)==GetDlgItem(IDC_CONDLIST)->m_hWnd);
      CListCtrl & Ctrl = IsCond ? CndCtrl : MsgCtrl;
      Ctrl.SetColumnWidth(0, Max(TWd0, Ctrl.GetColumnWidth(0)));
      Ctrl.SetColumnWidth(1, Max(TWd1, Ctrl.GetColumnWidth(1)));
      Ctrl.SetColumnWidth(2, Max(TWd1a, Ctrl.GetColumnWidth(2)));
      Ctrl.SetColumnWidth(3, Max(TWd1a, Ctrl.GetColumnWidth(3)));
      Ctrl.SetColumnWidth(4, Max(TWd1a, Ctrl.GetColumnWidth(4)));
      //CRect Rect;
      //Ctrl.GetWindowRect(&Rect);
      Ctrl.SetColumnWidth(5, Max(TWd2, Ctrl.GetColumnWidth(5)));
      break;
      }
    case LVN_GETDISPINFO:
      {
      NMLVDISPINFO * pnmv = (NMLVDISPINFO*) lParam; 
      flag IsCond=(pnmv->hdr.idFrom==IDC_CONDLIST);
      POSITION Pos=(POSITION)pnmv->item.lParam;
      CMsgList * pList=IsCond ? &Shared.m_CndList : &Shared.m_MsgList;
      CMsgItem &It=*pList->GetAt(Pos);
      switch (pnmv->item.iSubItem)
        {
        case 0:
          pnmv->item.iImage=It.iImg;
          pnmv->item.pszText=It.m_sSrc() ? It.m_sSrc() : "";
          break;
        case 1:
          pnmv->item.pszText=It.m_sID() ? It.m_sID() : "";
          break;
        case 2:
          pnmv->item.pszText=It.m_sIter() ? It.m_sIter() : "";
          break;
        case 3:
          pnmv->item.pszText=It.m_sSeq() ? It.m_sSeq() : "";
          break;
        case 4:
          pnmv->item.pszText=It.m_sCall() ? It.m_sCall() : "";
          break;
        case 5:
          pnmv->item.pszText=It.m_sTxt() ? It.m_sTxt() : "";
          break;
        }
      }
      break;
    }
  BOOL b = CFormView::OnNotify(wParam, lParam, pResult);
  return b;
  }

//---------------------------------------------------------------------------

void CMsgWnd::OnLButtonDown(UINT nFlags, CPoint point) 
  {
  if (point.y>=m_iSplitPos-3 && point.y<=m_iSplitPos+6)
    {
    m_iTrackSplitDiff = point.y - m_iSplitPos;
    m_bAdjustingSplit = 1;
    HCURSOR H = ScdApp()->LoadCursor(AFX_IDC_VSPLITBAR);
    ::SetCursor(H);

    SetCapture();
    }
  CFormView::OnLButtonDown(nFlags, point);
  }

//---------------------------------------------------------------------------

void CMsgWnd::OnLButtonUp(UINT nFlags, CPoint point) 
  {
  if (m_bAdjustingSplit)
    {
    m_bAdjustingSplit = 0;
    ReleaseCapture();
    m_iSplitPos = point.y + m_iTrackSplitDiff;
    SetListHeights();

    }
  CFormView::OnLButtonUp(nFlags, point);
  }

//---------------------------------------------------------------------------

void CMsgWnd::OnLButtonDblClk(UINT nFlags, CPoint point) 
  {
  if (point.y>=m_iSplitPos-3 && point.y<=m_iSplitPos+6)
    SetListHeights(true);
  CFormView::OnLButtonDblClk(nFlags, point);
  }

//---------------------------------------------------------------------------

void CMsgWnd::OnRButtonDown(UINT nFlags, CPoint point) 
  {
  UpdateData(true);
  CCustomListCtrl &MsgCtrl = *(CCustomListCtrl*)GetDlgItem(IDC_MSGLIST);
  CCustomListCtrl &CndCtrl = *(CCustomListCtrl*)GetDlgItem(IDC_CONDLIST);

  CMenu Menu;
  Menu.CreatePopupMenu();
  Menu.AppendMenu(MF_STRING, IDM_MSG_CLEARALL, "Clear &All");
  Menu.AppendMenu(MF_STRING, IDM_MSG_CLEARERR, "Clear All &MsgList");
  Menu.AppendMenu(MF_STRING, IDM_MSG_CLEARCOND, "Clear All &CndList");
  Menu.AppendMenu(MF_STRING, IDM_MSG_SOUND,     "&Sound");
  Menu.AppendMenu(MF_STRING, IDM_MSG_DISPLAYON, "&Display On Error");
  Menu.AppendMenu(MF_STRING, IDM_MSG_CENTRE, "Centre S&plitter");
  if (MsgCtrl.GetItemCount()==0 && CndCtrl.GetItemCount()==0)
    Menu.EnableMenuItem(0, MF_BYPOSITION|MF_GRAYED);
  if (MsgCtrl.GetItemCount()==0)
    Menu.EnableMenuItem(1, MF_BYPOSITION|MF_GRAYED);
  if (CndCtrl.GetItemCount()==0)
    Menu.EnableMenuItem(2, MF_BYPOSITION|MF_GRAYED);
  if (m_SoundOn)
    Menu.CheckMenuItem(3, MF_BYPOSITION|MF_CHECKED);
  if (m_DisplayOn)
    Menu.CheckMenuItem(4, MF_BYPOSITION|MF_CHECKED);
  POINT ScreenPoint = point;
  ClientToScreen(&ScreenPoint);
  int RetCd=Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD, ScreenPoint.x, ScreenPoint.y, this);
  Shared.DoRButtonMenu(RetCd);
  Menu.DestroyMenu();


  CFormView::OnRButtonDown(nFlags, point);
  }

//---------------------------------------------------------------------------

void CMsgWnd::OnMsgMenu(UINT id)
  {
  CCustomListCtrl &MsgCtrl = *(CCustomListCtrl*)GetDlgItem(IDC_MSGLIST);
  CCustomListCtrl &CndCtrl = *(CCustomListCtrl*)GetDlgItem(IDC_CONDLIST);

  switch (id)
    {
    case IDM_MSG_CLEARALL:
      ClearListAll(CndCtrl, Shared.m_CndList, false);
      ClearListAll(MsgCtrl, Shared.m_MsgList, true);
      break;
    case IDM_MSG_CLEARERR:
      ClearListAll(MsgCtrl, Shared.m_MsgList, true);
      break;
    case IDM_MSG_CLEARCOND:
      ClearListAll(CndCtrl, Shared.m_CndList, true);
      break;
    case IDM_MSG_SOUND:
      UpdateData(true);
      ToggleSound();
      UpdateData(false);
      break;
    case IDM_MSG_DISPLAYON:
      UpdateData(true);
      ToggleDisplay();
      UpdateData(false);
      break;
    case IDM_MSG_CENTRE:
      SetListHeights(true);
      break;
    }
  }

//---------------------------------------------------------------------------

void CMsgWnd::OnRclickErrlist(NMHDR* pNMHDR, LRESULT* pResult) 
  {
  CCustomListCtrl &MsgCtrl = *(CCustomListCtrl*)GetDlgItem(IDC_MSGLIST);
  Shared.PopupListMenu(MsgCtrl, Shared.m_MsgList, this);
  *pResult = 0;
  }

//---------------------------------------------------------------------------

void CMsgWnd::OnDblclkErrlist(NMHDR* pNMHDR, LRESULT* pResult) 
  {
  CCustomListCtrl &MsgCtrl = *(CCustomListCtrl*)GetDlgItem(IDC_MSGLIST);
  CString sLastTag;
  if (Shared.IndexAndAccessableTag(true, MsgCtrl, sLastTag)>=0)
    gs_AccessWnds.AccessNode(-1, (char*)(const char*)sLastTag);
  *pResult = 0;
  }

//---------------------------------------------------------------------------

void CMsgWnd::OnRclickCondlist(NMHDR* pNMHDR, LRESULT* pResult) 
  {
  CCustomListCtrl &CndCtrl = *(CCustomListCtrl*)GetDlgItem(IDC_CONDLIST);
  Shared.PopupListMenu(CndCtrl, Shared.m_CndList, this);
  *pResult = 0;
  }

//---------------------------------------------------------------------------

void CMsgWnd::OnDblclkCondlist(NMHDR* pNMHDR, LRESULT* pResult) 
  {
  CString sLastTag;
  CCustomListCtrl &CndCtrl = *(CCustomListCtrl*)GetDlgItem(IDC_CONDLIST);
  if (Shared.IndexAndAccessableTag(true, CndCtrl, sLastTag)>=0)
    gs_AccessWnds.AccessNode(-1, (char*)(const char*)sLastTag);
  *pResult = 0;
  }

//---------------------------------------------------------------------------

void CMsgWnd::OnSelendokShowwhat() 
  {
  CCustomListCtrl &MsgCtrl = *(CCustomListCtrl*)GetDlgItem(IDC_MSGLIST);
  CCustomListCtrl &CndCtrl = *(CCustomListCtrl*)GetDlgItem(IDC_CONDLIST);
  CComboBox &ShowWhat= *(CComboBox *)GetDlgItem(IDC_SHOWWHAT);

  Shared.m_iShowWhat=ShowWhat.GetCurSel();
  ScdPFUser.WrInt("Messages", "ShowWhat", Shared.m_iShowWhat);

  for (int j=0; j<2; j++)
    {
    CListCtrl* pCtrl=j ? &CndCtrl : &MsgCtrl;
    CMsgList * pList=j ? &Shared.m_CndList : &Shared.m_MsgList;

    pCtrl->DeleteAllItems();
    CMsgListIter Iter(*pList);
    for (CMsgItem *pIt=Iter.First(); pIt; pIt=Iter.Next())
      {
      if (Shared.ItemVisible(*pIt))
        {
        int n=pCtrl->InsertItem(pCtrl->GetItemCount(), LPSTR_TEXTCALLBACK , I_IMAGECALLBACK);
        pCtrl->SetItemText(n, 1, LPSTR_TEXTCALLBACK );
        pCtrl->SetItemText(n, 2, LPSTR_TEXTCALLBACK );
        pCtrl->SetItemText(n, 3, LPSTR_TEXTCALLBACK );
        pCtrl->SetItemText(n, 4, LPSTR_TEXTCALLBACK );
        pCtrl->SetItemText(n, 5, LPSTR_TEXTCALLBACK );
        pCtrl->SetItemData(n, (DWORD)Iter.Pos());
        }
      }
    }  
  }

//---------------------------------------------------------------------------

void CMsgWnd::OnContinue() 
  {
  m_bIgnore=false;
  }

//---------------------------------------------------------------------------

void CMsgWnd::OnIgnore() 
  {
  m_bIgnore=true;
  }

//---------------------------------------------------------------------------

void CMsgWnd::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
  {
	CFormView::OnActivateView(bActivate, pActivateView, pDeactiveView);
  if (bActivate)
    {
    CButton &Close = *(CButton*)GetDlgItem(IDC_CLOSE);
    //ClrWndDef(m_Continue.m_hWnd);
    SetWndDef(Close.m_hWnd);
    SetFocus();
    Close.SetFocus();
    }
  }

//---------------------------------------------------------------------------

void CMsgWnd::OnCndson() 
  {
	UpdateData(true);
  m_CndsOn=!m_CndsOn;
  if (!m_CndsOn)
    m_MsgsOn=true;
  ScdPFUser.WrInt("Messages", "ShowMsgs", m_MsgsOn);
  ScdPFUser.WrInt("Messages", "ShowCnds", m_CndsOn);
  UpdateData(false);
	DoLayout();
  }

//---------------------------------------------------------------------------

void CMsgWnd::OnMsgson() 
  {
	UpdateData(true);
  m_MsgsOn=!m_MsgsOn;
  if (!m_MsgsOn)
    m_CndsOn=true;
  ScdPFUser.WrInt("Messages", "ShowMsgs", m_MsgsOn);
  ScdPFUser.WrInt("Messages", "ShowCnds", m_CndsOn);
  UpdateData(false);
  DoLayout();	
  }

//---------------------------------------------------------------------------

void CMsgWnd::OnSound() 
  {
  UpdateData(true);
  ToggleSound();
  UpdateData(false);
  }

//===========================================================================
//
//
//
//===========================================================================

class CMsgFrm : public CMDIChildWnd
  {
    DECLARE_DYNCREATE(CMsgFrm)
    CMsgFrm();           // protected constructor used by dynamic creation
  public:
    //{{AFX_VIRTUAL(CMsgFrm)
    //}}AFX_VIRTUAL
  protected:
    virtual ~CMsgFrm();
    //{{AFX_MSG(CMsgFrm)
    afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnClose();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  };

//---------------------------------------------------------------------------

IMPLEMENT_DYNCREATE(CMsgFrm, CMDIChildWnd)

CMsgFrm::CMsgFrm()
  {
  }

//---------------------------------------------------------------------------

CMsgFrm::~CMsgFrm()
  {
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CMsgFrm, CMDIChildWnd)
  //{{AFX_MSG_MAP(CMsgFrm)
  ON_WM_GETMINMAXINFO()
  ON_WM_SETFOCUS()
  ON_WM_CLOSE()
  ON_WM_SIZE()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

void CMsgFrm::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
  {
  CMDIChildWnd::OnGetMinMaxInfo(lpMMI);
  if (GetTopWindow())
    {
    CMsgWnd &MW=*((CMsgWnd*)GetTopWindow());
    if (MW.m_bInited)
      {
      MW.MeasureClientCoOrds();
      CRect Wr;
      CRect Cr;
      MW.GetClientRect(&Cr);
      GetWindowRect(&Wr);
      int cx = (Wr.right-Wr.left) - (Cr.right-Cr.left);
      int cy = (Wr.bottom-Wr.top) - (Cr.bottom-Cr.top);
      int xmin = 5*MW.XGap + Max(MW.BBtns.Width(), MW.m_MsgOrigWidth) + cx;
      int ymin = 6*MW.YGap + MW.BBtns.Height() + (MW.m_MsgOrigHeight*2) + cy;
      lpMMI->ptMinTrackSize.x = xmin;
      lpMMI->ptMinTrackSize.y = ymin;
      }
    }
  }

//---------------------------------------------------------------------------

void CMsgFrm::OnSetFocus(CWnd* pOldWnd) 
  {
  CMsgWnd* pMsg = (CMsgWnd*)GetTopWindow();
  if (pMsg)
    {
    pMsg->UpdateData(false);
    //pMsg->UpdateDialogControls(pMsg, false);
    }
  CMDIChildWnd::OnSetFocus(pOldWnd);
  }

//---------------------------------------------------------------------------

void CMsgFrm::OnClose() 
  {
  //CMDIChildWnd::OnClose(); Do NOT call base, we are not allowed to close this window!
  }

//---------------------------------------------------------------------------

void CMsgFrm::OnSize(UINT nType, int cx, int cy) 
  {
  CMDIChildWnd::OnSize(nType, cx, cy);
  if (nType==SIZE_MAXIMIZED)
    CWindowLists::SetWndMaxMode(true);
  else if (nType==SIZE_MINIMIZED || nType==SIZE_RESTORED)
    CWindowLists::SetWndMaxMode(false);

  CMsgWnd &MW=*((CMsgWnd*)GetTopWindow());
  for (int i=0; i<2; i++)
    {
    CCustomListCtrl &Ctrl = *(CCustomListCtrl*)MW.GetDlgItem(i==0 ? IDC_MSGLIST : IDC_CONDLIST);
    Ctrl.SetColumnWidth(0, Max(TWd0, Ctrl.GetColumnWidth(0)));
    Ctrl.SetColumnWidth(1, Max(TWd1, Ctrl.GetColumnWidth(1)));
    Ctrl.SetColumnWidth(2, Max(TWd1a, Ctrl.GetColumnWidth(2)));
    Ctrl.SetColumnWidth(3, Max(TWd1a, Ctrl.GetColumnWidth(3)));
    Ctrl.SetColumnWidth(4, Max(TWd1a, Ctrl.GetColumnWidth(4)));
    //CRect Rect;
    //Ctrl.GetWindowRect(&Rect);
    Ctrl.SetColumnWidth(5, Max(TWd2, Ctrl.GetColumnWidth(5)));
    }
  }

//===========================================================================
//
//
//
//===========================================================================

class CMsgDlg : public CDialog
  {
// Construction
public:
	CMsgDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMsgDlg();

// Dialog Data
	//{{AFX_DATA(CMsgDlg)
	enum { IDD = IDD_MSGDLG };
	CStatic	m_ErrText;
	CComboBox	m_ShowWhat;
	CStatic	m_Icon;
	CCustomListCtrl	m_MsgCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMsgDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:

  CImageList        m_ImgList;         //bitmap images used in list boxes
  int               m_NonModalState;
  UINT              m_MsgCmd;

  // Generated message map functions
	//{{AFX_MSG(CMsgDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnContinue();
	afx_msg void OnIgnore();
	afx_msg void OnRclickMsglist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDblclkMsglist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelendokShowwhat();
	afx_msg void OnOK();
	afx_msg void OnCancel();
	afx_msg void OnClickMsglist(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
  afx_msg void OnMsgMenu(UINT id);
	DECLARE_MESSAGE_MAP()

  };

//---------------------------------------------------------------------------

CMsgDlg::CMsgDlg(CWnd* pParent )
	: CDialog(CMsgDlg::IDD, pParent)
  {
	//{{AFX_DATA_INIT(CMsgDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
  }

//---------------------------------------------------------------------------

CMsgDlg::~CMsgDlg()
  {
  Shared.m_pMsgDlg=NULL;
  }

//---------------------------------------------------------------------------

void CMsgDlg::DoDataExchange(CDataExchange* pDX)
  {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMsgDlg)
	DDX_Control(pDX, IDC_ERRTEXT, m_ErrText);
	DDX_Control(pDX, IDC_SHOWWHAT, m_ShowWhat);
	DDX_Control(pDX, IDC_EXICON, m_Icon);
	DDX_Control(pDX, IDC_MSGLIST, m_MsgCtrl);
	//}}AFX_DATA_MAP
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CMsgDlg, CDialog)
	//{{AFX_MSG_MAP(CMsgDlg)
	ON_BN_CLICKED(IDC_CONTINUE, OnContinue)
	ON_BN_CLICKED(IDC_IGNORE, OnIgnore)
	ON_NOTIFY(NM_RCLICK, IDC_MSGLIST, OnRclickMsglist)
	ON_WM_RBUTTONDOWN()
	ON_NOTIFY(NM_DBLCLK, IDC_MSGLIST, OnDblclkMsglist)
	ON_CBN_SELENDOK(IDC_SHOWWHAT, OnSelendokShowwhat)
	ON_COMMAND(IDOK, OnOK)
	ON_COMMAND(ID_MSGCANCEL, OnCancel)
	ON_NOTIFY(NM_CLICK, IDC_MSGLIST, OnClickMsglist)
	//}}AFX_MSG_MAP
  ON_COMMAND_RANGE(IDM_MSG_CLEARALL, IDM_MSG_CENTRE, OnMsgMenu)

END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CMsgDlg::OnInitDialog() 
  {
	CDialog::OnInitDialog();

  WINDOWPLACEMENT WP;
  WP.length=sizeof(WP);
  GetWindowPlacement(&WP);
  m_NonModalState=WP.showCmd;

  m_MsgCmd=CMsgWindow::GetShowCmd();
  //CMsgWindow::Reduce();
  CMsgWindow::Hide();

  CBitmap BM;
  BM.LoadBitmap(IDB_ERRORIMGS2);
  BOOL b=m_ImgList.Create(16, 12, false, 0, 10);
  m_ImgList.Add(&BM, (CBitmap*)NULL);
  
  m_MsgCtrl.InsertColumn(0, "Source",     LVCFMT_LEFT, 104);
  m_MsgCtrl.InsertColumn(1, "ID",         LVCFMT_RIGHT, 32);
  m_MsgCtrl.InsertColumn(2, "Iter",       LVCFMT_RIGHT, 4/*34*/);
  m_MsgCtrl.InsertColumn(3, "Seq",        LVCFMT_RIGHT, 4/*34*/);
  m_MsgCtrl.InsertColumn(4, "Call",       LVCFMT_RIGHT, 4/*34*/);
  m_MsgCtrl.InsertColumn(5, "Message",    LVCFMT_LEFT, 356);
  m_MsgCtrl.SetImageList(&m_ImgList ,     LVSIL_SMALL);

  Shared.m_iShowWhat = ScdPFUser.RdInt("Messages", "ShowWhat", MWShowNotes);//MWShowWarns);
  m_ShowWhat.AddString("All Messages");
  m_ShowWhat.AddString("Warnings & Errors");
  m_ShowWhat.AddString("Errors Only");
  m_ShowWhat.SetCurSel(Shared.m_iShowWhat);

  m_Icon.SetIcon(ScdApp()->LoadStandardIcon(IDI_EXCLAMATION));

  int LastVisible=-1;
  CMsgListIter Iter(Shared.m_MsgList);
  CMsgItem *pLastIt=NULL;
  for (CMsgItem *pIt=Iter.First(); pIt; pIt=Iter.Next())
    {
    if (Shared.ItemVisible(*pIt))
      {
      int n=m_MsgCtrl.InsertItem(m_MsgCtrl.GetItemCount(), LPSTR_TEXTCALLBACK , I_IMAGECALLBACK);
      m_MsgCtrl.SetItemText(n, 1, LPSTR_TEXTCALLBACK );
      m_MsgCtrl.SetItemText(n, 2, LPSTR_TEXTCALLBACK );
      m_MsgCtrl.SetItemText(n, 3, LPSTR_TEXTCALLBACK );
      m_MsgCtrl.SetItemText(n, 4, LPSTR_TEXTCALLBACK );
      m_MsgCtrl.SetItemText(n, 5, LPSTR_TEXTCALLBACK );
      m_MsgCtrl.SetItemData(n, (DWORD)Iter.Pos());
      LastVisible=n;

      //int iWd=m_MsgCtrl.GetStringWidth(pTxt)+20);
      //if (iWd>0)
      //  MsgCtrl.SetColumnWidth(1, Max(iWd[1], MsgCtrl.GetColumnWidth(1)));
      }
    pLastIt=pIt;
    }

  if (LastVisible>=0)
    m_MsgCtrl.EnsureVisible(LastVisible, false);

  if (pLastIt)
    {
    Strng S;
    S.Concat(pLastIt->m_sSrc(), " : ",pLastIt->m_sTxt());
    m_ErrText.SetWindowText(S());
    }

  Shared.m_pMsgDlg=this;  	

	return true;  // return true unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return false
  }

//---------------------------------------------------------------------------

BOOL CMsgDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
  {
  BOOL b = CDialog::OnNotify(wParam, lParam, pResult);
  HD_NOTIFY* pNM = (HD_NOTIFY*)lParam;
  switch (pNM->hdr.code)
    {
    case HDN_DIVIDERDBLCLICK:
    case HDN_ENDTRACK: //column width changed
      {
      //NOTE SetColumnWidth does NOT seem to work for HDN_ENDTRACK !!!
      CListCtrl & MsgCtrl = *(CListCtrl*)GetDlgItem(IDC_MSGLIST);
      BOOL B;
      if (pNM->iItem==0)
        {
        B = MsgCtrl.SetColumnWidth(0, Max(20, MsgCtrl.GetColumnWidth(0)));
        B = MsgCtrl.SetColumnWidth(1, Max(20, MsgCtrl.GetColumnWidth(1)));
        B = MsgCtrl.SetColumnWidth(2, Max(4/*20*/, MsgCtrl.GetColumnWidth(2)));
        B = MsgCtrl.SetColumnWidth(3, Max(4/*20*/, MsgCtrl.GetColumnWidth(3)));
        B = MsgCtrl.SetColumnWidth(4, Max(4/*20*/, MsgCtrl.GetColumnWidth(4)));
        B = MsgCtrl.SetColumnWidth(5, Max(20, MsgCtrl.GetColumnWidth(5)));
        }
      CRect ErrRect;
      MsgCtrl.GetWindowRect(&ErrRect);
      const int W = ErrRect.right - ErrRect.left - 20;// - MsgCtrl.GetColumnWidth(5);???
      B = MsgCtrl.SetColumnWidth(5, Range(10, MsgCtrl.GetColumnWidth(5), W));//Max(20, W));
      //const int W1 = ErrRect.right - MsgCtrl.GetColumnWidth(0) - MsgCtrl.GetColumnWidth(1) - MsgCtrl.GetColumnWidth(2) - MsgCtrl.GetColumnWidth(3) - MsgCtrl.GetColumnWidth(4);
      //B = MsgCtrl.SetColumnWidth(5, Range(10, W1, W));
      break;
      }
    case LVN_GETDISPINFO:
      {
      NMLVDISPINFO * pnmv = (NMLVDISPINFO*) lParam; 
      flag IsCond=(pnmv->hdr.idFrom==IDC_CONDLIST);
      POSITION Pos=(POSITION)pnmv->item.lParam;
      CMsgList * pList=IsCond ? &Shared.m_CndList : &Shared.m_MsgList;
      CMsgItem &It=*pList->GetAt(Pos);
      switch (pnmv->item.iSubItem)
        {
        case 0:
          pnmv->item.iImage=It.iImg;
          pnmv->item.pszText=It.m_sSrc() ? It.m_sSrc() : "";
          break;
        case 1:
          pnmv->item.pszText=It.m_sID() ? It.m_sID() : "";
          break;
        case 2:
          pnmv->item.pszText=It.m_sIter() ? It.m_sIter() : "";
          break;
        case 3:
          pnmv->item.pszText=It.m_sSeq() ? It.m_sSeq() : "";
          break;
        case 4:
          pnmv->item.pszText=It.m_sCall() ? It.m_sCall() : "";
          break;
        case 5:
          pnmv->item.pszText=It.m_sTxt() ? It.m_sTxt() : "";
          break;
        }
      }
      break;
    }
  return b;
  }

//---------------------------------------------------------------------------

void CMsgDlg::OnContinue() 
  {
  OnCancel();
  }

//---------------------------------------------------------------------------

void CMsgDlg::OnIgnore() 
  {
  OnOK();
  }

//---------------------------------------------------------------------------

void CMsgDlg::OnOK() 
  {
  CMsgWindow::SetShowCmd(m_MsgCmd);
  CDialog::OnOK();
  }

//---------------------------------------------------------------------------

void CMsgDlg::OnCancel() 
  {
  CMsgWindow::SetShowCmd(m_MsgCmd);
  CDialog::OnCancel();
  }

//---------------------------------------------------------------------------

void CMsgDlg::OnRclickMsglist(NMHDR* pNMHDR, LRESULT* pResult) 
  {
  //Shared.PopupListMenu(m_MsgCtrl, Shared.m_MsgList, this);
  *pResult = 0;
  }

//---------------------------------------------------------------------------

void CMsgDlg::OnDblclkMsglist(NMHDR* pNMHDR, LRESULT* pResult) 
  {
  //CString sLastTag;
  //if (Shared.IndexAndAccessableTag(true, m_MsgCtrl, sLastTag)>=0)
  //  gs_pAccWnd->AccessNode((char*)(const char*)sLastTag);
  *pResult = 0;
  }

//---------------------------------------------------------------------------

void CMsgDlg::OnRButtonDown(UINT nFlags, CPoint point) 
  {
  UpdateData(true);
  CMenu Menu;
  Menu.CreatePopupMenu();
  Menu.AppendMenu(MF_STRING, IDM_MSG_CLEARERR, "Clear All &MsgList");
  if (m_MsgCtrl.GetItemCount()==0)
    Menu.EnableMenuItem(1, MF_BYPOSITION|MF_GRAYED);
  POINT ScreenPoint = point;
  ClientToScreen(&ScreenPoint);
  int RetCd=Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD, ScreenPoint.x, ScreenPoint.y, this);
  Shared.DoRButtonMenu(RetCd);
  Menu.DestroyMenu();

  CDialog::OnRButtonDown(nFlags, point);
  }

//---------------------------------------------------------------------------

void CMsgDlg::OnMsgMenu(UINT id)
  {
  switch (id)
    {
    case IDM_MSG_CLEARALL:
    case IDM_MSG_CLEARERR:
      ClearListAll(m_MsgCtrl, Shared.m_MsgList, true);
      break;
    case IDM_MSG_CLEARCOND:
      break;
    }
  }

//---------------------------------------------------------------------------

void CMsgDlg::OnSelendokShowwhat() 
  {
  Shared.m_iShowWhat=m_ShowWhat.GetCurSel();
  m_MsgCtrl.DeleteAllItems();
  CMsgListIter Iter(Shared.m_MsgList);
  for (CMsgItem *pIt=Iter.First(); pIt; pIt=Iter.Next())
    {
    if (Shared.ItemVisible(*pIt))
      {
      int n=m_MsgCtrl.InsertItem(m_MsgCtrl.GetItemCount(), LPSTR_TEXTCALLBACK , I_IMAGECALLBACK);
      m_MsgCtrl.SetItemText(n, 1, LPSTR_TEXTCALLBACK );
      m_MsgCtrl.SetItemText(n, 2, LPSTR_TEXTCALLBACK );
      m_MsgCtrl.SetItemText(n, 3, LPSTR_TEXTCALLBACK );
      m_MsgCtrl.SetItemText(n, 4, LPSTR_TEXTCALLBACK );
      m_MsgCtrl.SetItemText(n, 5, LPSTR_TEXTCALLBACK );
      m_MsgCtrl.SetItemData(n, (DWORD)Iter.Pos());
      }
    }
  }

void CMsgDlg::OnClickMsglist(NMHDR* pNMHDR, LRESULT* pResult) 
  {
	// TODO: Add your control notification handler code here
//  pNMHDR, LRESULT* pResult) 
//  m_MsgCtrl.Get;

  NMLISTVIEW *p=(NMLISTVIEW *)pNMHDR;

  //POSITION pos = m_MsgCtrl.GetFirstSelectedItemPosition();
  if (p && p->iItem>=0)
    {
    int nItem = p->iItem;//m_MsgCtrl.GetNextSelectedItem(pos);
    CString S=m_MsgCtrl.GetItemText(nItem, 0);
    CString T=m_MsgCtrl.GetItemText(nItem, 5);
    S+=" : ";
    S+=T;
  	m_ErrText.SetWindowText(S);
    }

	*pResult = 0;
  }

//===========================================================================
//
//
//
//===========================================================================

CMsgCommon::CMsgCommon()
  {
  m_iShowWhat=0;
  m_pMsgWnd=NULL;
  m_pMsgDlg=NULL;

  m_CndListCnt=-1;
  m_MsgListCnt=-1;
  m_nErrorMsgs=0;
  m_nWarningMsgs=0;
  m_nNoteMsgs=0;
  }
//---------------------------------------------------------------------------


flag CMsgCommon::ItemVisible(CMsgItem &It)
  {
  static DWORD RqdFlags[]=
    {
    LogFlag_Fatal|LogFlag_Stop|LogFlag_Separator|LogFlag_Error|LogFlag_Warning|LogFlag_Note, 
    LogFlag_Fatal|LogFlag_Stop|LogFlag_Separator|LogFlag_Error|LogFlag_Warning, 
    LogFlag_Fatal|LogFlag_Stop|LogFlag_Separator|LogFlag_Error, 
    };

  return (It.dwFlags&RqdFlags[m_iShowWhat]) != 0;
  };

//---------------------------------------------------------------------------

flag CMsgCommon::DoRButtonMenu(int Cmd)
  {
  if (m_pMsgDlg)
    m_pMsgDlg->SendMessage(WM_COMMAND, Cmd, 0);
  if (m_pMsgWnd)
    m_pMsgWnd->SendMessage(WM_COMMAND, Cmd, 0);

  return true;  
  };

//---------------------------------------------------------------------------

int CMsgCommon::IndexAndAccessableTag(flag TagRqd, CCustomListCtrl& Ctrl, CString& sTag)
  {
  sTag = "";
  LV_HITTESTINFO HTI;
  HTI.pt.y = Ctrl.PrevDownPoint.y;
  HTI.pt.x = 10;
  int iLastIndex = Ctrl.HitTest(&HTI);
  if (gs_pPrj && gs_pPrj->pPrjDoc && iLastIndex>=0 && (HTI.flags & LVHT_ONITEM))
    {
    sTag = Ctrl.GetItemText(iLastIndex, 0);
    if (sTag.GetLength()>0 && TaggedObject::TestValidTag((char*)(const char*)sTag)==0)
      {
      const int i = sTag.Find('.');
      if (i>=0)
        sTag = sTag.Left(i);
      CXM_ObjectTag ObjTag((char*)(const char*)sTag, TABOpt_Exists);
      CXM_ObjectData ObjData;
      CXM_Route Route;
      if (gs_pPrj->XReadTaggedItem(ObjTag, ObjData, Route))
        return iLastIndex;
      sTag="";
      }
    if (TagRqd)
      return -1;
    else
      return iLastIndex;
    }
  return -1;
  }

//---------------------------------------------------------------------------

void CMsgCommon::PopupListMenu(CCustomListCtrl& Ctrl, CMsgList& List, CWnd* pWnd)
  {
  CString sLastTag;
  int TagIndex = IndexAndAccessableTag(false, Ctrl, sLastTag);
  if (TagIndex>=0)
    {
    CMenu Menu;
    Menu.CreatePopupMenu();
    Strng s;
    flag GotTag=(sLastTag.GetLength()>0);
    if (GotTag)
      s.Set("&Access %s...", (const char*)sLastTag);
    else
      s = "&Access...";
    const int SelCnt = Ctrl.GetSelectedCount();

    Menu.AppendMenu(MF_STRING|(GotTag?0:MF_GRAYED), IDM_MSG_ACCESS, s());
    Menu.AppendMenu(MF_STRING|(GotTag?0:MF_GRAYED), IDM_MSG_FIND, "&Find...");
    Menu.AppendMenu(MF_SEPARATOR);
    Menu.AppendMenu(MF_STRING, IDM_MSG_CLEARLINE, "C&lear");

    CMenu CurrentMenu;
    CurrentMenu.CreatePopupMenu();
    CurrentMenu.AppendMenu(MF_STRING, IDM_MSG_COPYLINETXT, "&Msg/Cnd Only");
    CurrentMenu.AppendMenu(MF_STRING, IDM_MSG_COPYLINE, "&Full Text");
    CMenu SelectedMenu;
    SelectedMenu.CreatePopupMenu();
    SelectedMenu.AppendMenu(MF_STRING, IDM_MSG_COPYLINETXT_SEL, "&Msg/Cnd Only");
    SelectedMenu.AppendMenu(MF_STRING, IDM_MSG_COPYLINE_SEL, "&Full Text");
    CMenu AllMenu;
    AllMenu.CreatePopupMenu();
    AllMenu.AppendMenu(MF_STRING, IDM_MSG_COPYLINETXT_ALL, "&Msg/Cnd Only");
    AllMenu.AppendMenu(MF_STRING, IDM_MSG_COPYLINE_ALL, "&Full Text");

    Menu.AppendMenu(MF_POPUP, (unsigned int)CurrentMenu.m_hMenu, "&Copy Current");
    Menu.AppendMenu(MF_POPUP, (unsigned int)SelectedMenu.m_hMenu, "&Copy Selected");
    Menu.AppendMenu(MF_POPUP, (unsigned int)AllMenu.m_hMenu, "&Copy All");

    //Menu.AppendMenu(MF_STRING, IDM_MSG_COPYLINETXT, "Copy &Text");
    //Menu.AppendMenu(MF_STRING, IDM_MSG_COPYLINE, "&Copy Line");
    //Menu.AppendMenu(MF_SEPARATOR);
    //Menu.AppendMenu(MF_STRING|(SelCnt>1?0:MF_GRAYED), IDM_MSG_COPYLINETXT_ALL, "Copy Selected Text");
    //Menu.AppendMenu(MF_STRING|(SelCnt>1?0:MF_GRAYED), IDM_MSG_COPYLINE_ALL, "Copy Selected Line");

    CRect Rect;
    Ctrl.GetWindowRect(&Rect);
    int RetCd=Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD, Rect.left+Ctrl.PrevDownPoint.x, Rect.top+Ctrl.PrevDownPoint.y, pWnd);
    Menu.DestroyMenu();
    switch (RetCd)
      {
      case IDM_MSG_ACCESS:
        gs_AccessWnds.AccessNode(-1, (char*)(const char*)sLastTag);
        break;
      case IDM_MSG_FIND:
        gs_pPrj->FindTag((char*)(const char*)sLastTag, NULL, NULL, -1, FTO_MoveCursor|FTO_HighliteSlow);
        break;
      case IDM_MSG_CLEARLINE:
        {
        POSITION Pos=(POSITION)Ctrl.GetItemData(TagIndex);
        if (Pos)
          {
          delete List.GetAt(Pos);
          List.RemoveAt(Pos);
          Ctrl.DeleteItem(TagIndex);
          }
        break;
        }
      case IDM_MSG_COPYLINETXT:
      case IDM_MSG_COPYLINE:
        {
        POSITION Pos=(POSITION)Ctrl.GetItemData(TagIndex);
        if (Pos)
          {
          CMsgItem& It = *List.GetAt(Pos);
          if (RetCd==IDM_MSG_COPYLINETXT)
            {
            if (It.m_sTxt.Len())
              CopyTextToClipboard(pWnd, It.m_sTxt());
            }
          else
            {
            Strng s(It.m_sSrc());
            s += '\t';
            s += It.m_sID();
            s += '\t';
            s += It.m_sSeq();
            s += '\t';
            s += It.m_sCall();
            s += '\t';
            s += It.m_sTxt();
            CopyTextToClipboard(pWnd, s());
            }
          }
        break;
        }
      case IDM_MSG_COPYLINETXT_SEL:
      case IDM_MSG_COPYLINE_SEL:
        {
        int nItem=-1;
        Strng s;
        for (int i=0; i<SelCnt; i++)
          {
          nItem = Ctrl.GetNextItem(nItem, LVNI_SELECTED);
          ASSERT(nItem != -1);
          POSITION Pos = (POSITION)Ctrl.GetItemData(nItem);
          if (Pos)
            {
            CMsgItem& It = *List.GetAt(Pos);
            if (RetCd==IDM_MSG_COPYLINE_SEL)
              {
              s += It.m_sSrc();
              s += '\t';
              s += It.m_sID();
              s += '\t';
              s += It.m_sSeq();
              s += '\t';
              s += It.m_sCall();
              s += '\t';
              }
            s += It.m_sTxt();
            s += "\r\n";
            }
          }
        CopyTextToClipboard(pWnd, s());
        break;
        }
      case IDM_MSG_COPYLINETXT_ALL:
      case IDM_MSG_COPYLINE_ALL:
        {
        int nItem=-1;
        Strng s;
        for (int i=0; i<Ctrl.GetItemCount(); i++)
          {
          nItem = i;//Ctrl.GetItemCount(.GetNextItem(nItem, LVNI_SELECTED);
          ASSERT(nItem != -1);
          POSITION Pos = (POSITION)Ctrl.GetItemData(nItem);
          if (Pos)
            {
            CMsgItem& It = *List.GetAt(Pos);
            if (RetCd==IDM_MSG_COPYLINE_ALL)
              {
              s += It.m_sSrc();
              s += '\t';
              s += It.m_sID();
              s += '\t';
              s += It.m_sSeq();
              s += '\t';
              s += It.m_sCall();
              s += '\t';
              }
            s += It.m_sTxt();
            s += "\r\n";
            }
          }
        CopyTextToClipboard(pWnd, s());
        break;
        }
      }
    }
  }

//---------------------------------------------------------------------------

void CMsgCommon::SetMsgsIncludeFlag(flag On) 
  { 
  for (int j=0; j<2; j++)
    {
    CMsgList & List=j ? m_CndList : m_MsgList;
    CMsgListIter Iter(List);
    for (CMsgItem *pIt=Iter.First(); pIt; pIt=Iter.Next())
      pIt->m_fIncludeInCnt=On;
    }
  }

//---------------------------------------------------------------------------

void CMsgCommon::CountMsgs() 
  { 
  if (m_CndListCnt==m_CndList.GetCount() && m_MsgListCnt==m_MsgList.GetCount())
    return;
  m_nErrorMsgs=0;
  m_nWarningMsgs=0;
  m_nNoteMsgs=0;

  for (int j=0; j<2; j++)
    {
    CMsgList & List=j ? m_CndList : m_MsgList;
    CMsgListIter Iter(List);
    for (CMsgItem *pIt=Iter.First(); pIt; pIt=Iter.Next())
      if (pIt->m_fIncludeInCnt)
        {
        if (pIt->dwFlags & (LogFlag_Fatal | LogFlag_Stop | LogFlag_Error))
          m_nErrorMsgs++;
        else if (pIt->dwFlags & LogFlag_Warning)
          m_nWarningMsgs++;
        else if (pIt->dwFlags & LogFlag_Note)
          m_nNoteMsgs++;
        }
    }  
  m_CndListCnt=m_CndList.GetCount();
  m_MsgListCnt=m_MsgList.GetCount();
  }

//---------------------------------------------------------------------------

long CMsgCommon::NErrorMsgs()   { CountMsgs(); return m_nErrorMsgs;  };
long CMsgCommon::NWarningMsgs() { CountMsgs(); return m_nWarningMsgs; };
long CMsgCommon::NNoteMsgs()    { CountMsgs(); return m_nNoteMsgs;   };

//===========================================================================
//
//
//
//===========================================================================

void CMsgWindow::Register()
  {
  CRect Rect;
  CRect MainRect;
  HICON hIcon;
  CString ClassName;
  CCreateContext CC;

  AfxGetMainWnd()->GetClientRect(&MainRect);
  int w = (MainRect.right - MainRect.left - 30) / 3;

  //message window...  4
  Rect.left = 0;
  Rect.right = w * 2;
  Rect.top = 0;
  Rect.bottom = 200;
  CC.m_pCurrentDoc=NULL;
  CC.m_pNewDocTemplate=NULL;
  CC.m_pLastView=NULL;
  CC.m_pCurrentFrame=NULL;
  CC.m_pNewViewClass=RUNTIME_CLASS(CMsgWnd);
  Shared.m_pMsgFrm = new CMsgFrm;
  hIcon = ScdApp()->LoadIcon(IDI_MSGWND);
  ClassName = AfxRegisterWndClass(CS_DBLCLKS|CS_NOCLOSE, 0, 0, hIcon);
  if (Shared.m_pMsgFrm->Create(ClassName.GetBuffer(0), 
                        "Messages", WS_CHILD | WS_OVERLAPPED | 
                        WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | 
                        WS_MINIMIZEBOX | WS_MAXIMIZEBOX, 
                        Rect, NULL, &CC))
    {
    Shared.m_pMsgWnd = (CMsgWnd*)Shared.m_pMsgFrm->GetTopWindow();
    gs_MsgLog.RegisterMsgWnd(Shared.m_pMsgWnd->m_hWnd, WMU_LOGMSG);
    Shared.m_pMsgFrm->ShowWindow(SW_MINIMIZE);
    }
  }

void CMsgWindow::StartUp()      { };
void CMsgWindow::ShutDown()     { };

void CMsgWindow::ClearAll()     { Shared.DoRButtonMenu(IDM_MSG_CLEARALL);  };

UINT CMsgWindow::GetShowCmd()     
  { 
  WINDOWPLACEMENT WP;
  WP.length=sizeof(WP);
  Shared.m_pMsgFrm->GetWindowPlacement(&WP);
  return WP.showCmd;
  };
void CMsgWindow::SetShowCmd(UINT Cmd)     
  { 
  Shared.m_pMsgFrm->ShowWindow(Cmd);
  };

int CMsgWindow::Show(flag DoBringToTop)
  {
  int Ret=0;
  if (DoBringToTop && (Shared.m_pMsgFrm->GetFocus()!=Shared.m_pMsgFrm))
    {
    Shared.m_pMsgFrm->MDIActivate(); //bring the window to the front
    if (Shared.m_pMsgFrm->IsIconic())
      Ret=Shared.m_pMsgFrm->ShowWindow(SW_RESTORE);
    }

  Shared.m_pMsgFrm->UpdateWindow();
  return Ret;//Shared.m_pMsgFrm->ShowWindow(SW_RESTORE);
  };//SHOWNORMAL); };//SW_SHOWNOACTIVATE);
int CMsgWindow::Hide()          { return Shared.m_pMsgFrm->ShowWindow(SW_HIDE); };
int CMsgWindow::Reduce()        { return Shared.m_pMsgFrm->ShowWindow(SW_SHOWMINNOACTIVE); };

int CMsgWindow::DoModalDlg()    { CMsgDlg Dlg(AfxGetMainWnd()); return Dlg.DoModal(); };

CFormView* CMsgWindow::GetWnd() { return Shared.m_pMsgWnd; };
CMDIChildWnd* CMsgWindow::GetFrame() { return Shared.m_pMsgFrm; };

void CMsgWindow::SetSplitPos(int SplitPos) { Shared.m_pMsgWnd->SetSplitPos(SplitPos); }
int CMsgWindow::GetSplitPos()   { return Shared.m_pMsgWnd->GetSplitPos(); };

void CMsgWindow::SetMsgsIncludeFlag(flag On) { Shared.SetMsgsIncludeFlag(On);  };
long CMsgWindow::NErrorMsgs()   { return Shared.NErrorMsgs();  };
long CMsgWindow::NWarningMsgs() { return Shared.NWarningMsgs(); };
long CMsgWindow::NNoteMsgs()    { return Shared.NNoteMsgs();   };

//===========================================================================
//
//
//
//===========================================================================


