//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
#include "scdver.h"
#include "..\resource.h"
#include "helpinfo.h"
#include "mdlslctu.h"
#include "project.h"
#include "licbase.h"
#include "msgwnd.h"
#include "sfe_base.h"
#include "mdlslctu.h"
#include ".\mdlslctu.h"
//#include "optoff.h"

#ifdef _DEBUG
#undef THIS_FILE    
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//===========================================================================
//
//
//
//===========================================================================

void CInsertUnitDlg::PutEditNumber(CEdit & Ed, double X)
  {
  CString S;
  long Y=(long)X;
  if (Y==X)
    S.Format("%i", Y);
  else
    S.Format("%.2f", X);
  Ed.SetWindowText(S);
  };

double CInsertUnitDlg::GetEditNumber(CEdit & Ed)
  {
  CString S;
  Ed.GetWindowText(S);
  return SafeAtoF(S);
  };

//---------------------------------------------------------------------------

static int AppendSymNames(LPTSTR Path, LPTSTR Prefix, CStringList & SymList)
  {
  int n=0;
  Strng Fn;
  WIN32_FIND_DATA fd;
  HANDLE hFiles= FindFirstFile(Path, &fd);
  flag AllDone = (hFiles==INVALID_HANDLE_VALUE);
  while (!AllDone)
    {
    Fn.FnName(fd.cFileName);
    CString Lb;
    if (Prefix)
      Lb.Format("%s:%s", Prefix, Fn());
    else
      Lb=Fn();
    if (SymList.Find(Lb)==NULL)
      {
      SymList.AddTail(Lb);
      n++;
      }
    AllDone = !FindNextFile(hFiles, &fd);
    }
  FindClose(hFiles);
  return n;
  }

//===========================================================================
//
//
//
//===========================================================================

const LPCTSTR SeparString     = "-----------------------------------------------";
const LPCTSTR SelectedSymbol  = "$SelectedSymbol";
const LPTSTR MRUFmt           = "$MRU.%02i";

//---------------------------------------------------------------------------

WINDOWPLACEMENT CInsertUnitDlg::sm_WP;
flag CInsertUnitDlg::sm_bWPOk = 0;
int  CInsertUnitDlg::sm_LastUnitModel=-1;
int  CInsertUnitDlg::sm_LastLinkModel=-1;
int  CInsertUnitDlg::sm_LastCLnkModel=-1;
int  CInsertUnitDlg::sm_LastELnkModel=-1;
int  CInsertUnitDlg::sm_LastALnkModel=-1;

//---------------------------------------------------------------------------

CInsertUnitDlg::CInsertUnitDlg(CDlgWhat What, flag IsConstructSymbol, pGrfComCmdBlk Mdl, CWnd* pParent /*=NULL*/)
  : m_bCreated(false), CDialog(CInsertUnitDlg::IDD, pParent)
  , m_bExistingModel(FALSE)
  {
  m_What                  = What;
  m_bIsConstructSymbol    = IsConstructSymbol;
  m_bInited               = false;
  m_iHoldUpd              = 0;
  gs_pCmd->SetDlgBusy();
  m_pMdl                  = Mdl;
  m_bTagChgd              = 0;
  m_bUsingPrjSymbols      = 0;
  //m_bUsePrevSymb          = 0;
  //m_bUsePrevSymbDirect    = 0;
  //m_pBaseTag              = NULL;
  m_pTTC                  = NULL;
  m_nEntities             = 0;
  m_iPrevSelSymbol        = -1;
  m_bMdlValid             = false;

  SetPFSection();

  m_pDsp                  = NULL;
  m_pVwPort               = NULL;
  m_pDrw                  = NULL;

  m_DXFLoaded             = false;
  m_BMPLoaded             = false;
  m_DefLoaded             = false;
  m_bCUOrigSymbolFound    = true;
  m_bSymbolChanged        = false;

  m_RecoveringData        = 0;
  m_HoldFeedback          = 0;

  m_OriginalScales.m_bInUse=false;

  //{{AFX_DATA_INIT(CInsertUnitDlg)
  //m_Rotate = 0.0;//m_Rotate_Mem;
  //m_Scl_X = 100.0;//m_SclX_Mem;
  //m_Scl_Y = 100.0;//m_SclY_Mem;
  m_HideTag = FALSE;
  m_AllSymbols = FALSE;
	//}}AFX_DATA_INIT

  m_PolyCnt=0;

  int nAdded=0;

  for (int PathPass=0; PathPass<2 ; PathPass++)
    {
    m_bUsingPrjSymbols=(PathPass==0);
    Strng SymPath(m_bUsingPrjSymbols==1 ? GrfSymbolFiles() : BaseGrfSymbolFiles());
    Strng Path, Fn, Ext("dxf");

    WIN32_FIND_DATA fdd;
    Path.Set("%s*.*", SymPath());
    HANDLE hFolders=FindFirstFile(Path(), &fdd);

    flag AllFolders= (hFolders==INVALID_HANDLE_VALUE);
    while (!AllFolders)
      {
      if ((fdd.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_HIDDEN)) == FILE_ATTRIBUTE_DIRECTORY)
        {
        if (fdd.cFileName[0] != '.')
          {
          m_DrwGrpList.AddTail(fdd.cFileName);
          Path.Set("%s%s\\*.%s", SymPath(), fdd.cFileName, Ext());
          nAdded+=AppendSymNames(Path(), fdd.cFileName, m_DrwSymbols);
          }
        }
      AllFolders = !FindNextFile(hFolders, &fdd);
      }
    }

  Create(CInsertUnitDlg::IDD, pParent);

  m_bCreated=true;
  }

//---------------------------------------------------------------------------

CInsertUnitDlg::~CInsertUnitDlg()
  {
  delete m_pTTC;
  gs_pCmd->SetDlgBusy(0);
  
  //if m_pDsp)
  //  m_pDsp->SetViewDrawing(NULL);
  delete m_pDrw;
  delete m_pDsp;
  // delete m_pVwPort; this gets deleted in previous line
  }

//---------------------------------------------------------------------------

void CInsertUnitDlg::Reset()
  {
  sm_LastUnitModel = -1;
  sm_LastLinkModel = -1;
  sm_LastCLnkModel = -1;
  sm_LastCLnkModel = -1;
  sm_LastELnkModel = -1;
  sm_LastALnkModel = -1;
  }

//---------------------------------------------------------------------------

void CInsertUnitDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CInsertUnitDlg)
  //DDX_Text(pDX, IDC_MDLROT, m_Rotate);
  //DDX_Text(pDX, IDC_MDLSCL_X, m_Scl_X);
  //DDX_Text(pDX, IDC_MDLSCL_Y, m_Scl_Y);
  DDX_Check(pDX, IDC_HideTag, m_HideTag);
  DDX_Check(pDX, IDC_ALLSYMB, m_AllSymbols);
  //}}AFX_DATA_MAP
  DDX_Control(pDX, IDC_MDL_TREE, m_MdlTree);
  DDX_Control(pDX, IDC_STATICFRAME, m_SymbolFrame);
  DDX_Control(pDX, IDC_MDLSYMBOL, m_SymbolList);
  DDX_Control(pDX, IDC_MDLTAG, m_MdlTag);
  DDX_Control(pDX, IDC_MDLROT, m_Rotate);
  DDX_Control(pDX, IDC_MDLSCL_X, m_Scl_X);
  DDX_Control(pDX, IDC_MDLSCL_Y, m_Scl_Y);
  DDX_Control(pDX, IDC_DRWGROUPSTATIC, m_DrwGrpStatic);
  DDX_Check(pDX, IDC_EXISTINGMODEL, m_bExistingModel);
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CInsertUnitDlg, CDialog)
  //{{AFX_MSG_MAP(CInsertUnitDlg)
  ON_WM_DESTROY()
  ON_EN_CHANGE(IDC_MDLTAG, OnChangeMdltag)
  //ON_CBN_SELCHANGE(IDC_MDLSYMBOL, OnSelchangeMdlsymbol)
  //ON_CBN_EDITCHANGE(IDC_MDLSYMBOL, OnEditchangeMdlsymbol)
  ON_WM_PAINT()
  ON_BN_CLICKED(ID_MDLDOC, OnMdldoc)
  //ON_BN_CLICKED(ID_MDLHELP, OnMdlhelp)
  //ON_BN_CLICKED(IDC_MORE, OnMore)
  ON_BN_CLICKED(IDC_ALLSYMB, OnAllsymb)
	ON_BN_CLICKED(IDB_IWIN, OnIwin)
	ON_BN_CLICKED(IDB_CWIN, OnCwin)
  //}}AFX_MSG_MAP
  ON_UPDATE_COMMAND_UI(IDOK, OnUpdateOKBtn)
  ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipNotify)
  ON_NOTIFY(TVN_SELCHANGED, IDC_MDL_TREE, OnTvnSelchangedMdlTree)
  //ON_NOTIFY(TVN_SELCHANGING, IDC_MDL_TREE, OnTvnSelchangingMdlTree)
  ON_EN_CHANGE(IDC_MDLSCL_X, OnEnChangeMdlsclX)
  ON_EN_CHANGE(IDC_MDLSCL_Y, OnEnChangeMdlsclY)
  ON_EN_CHANGE(IDC_MDLROT, OnEnChangeMdlsclRot)
  ON_LBN_SELCHANGE(IDC_MDLSYMBOL, OnLbnSelchangeMdlsymbol)
  ON_BN_CLICKED(IDC_HideTag, OnBnClickedHidetag)
  ON_STN_CLICKED(IDC_STATICFRAME, OnStnClickedStaticframe)
  ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

void CInsertUnitDlg::SetPFSection()
  {
  m_sSection="Insert.";
  switch (m_What)
    {
    case CDlgWhat_InsUnit: m_sSection+="InsUnit"; break;
    case CDlgWhat_ConUnit: m_sSection+="ConUnit"; break;
    case CDlgWhat_ConCLnk: m_sSection+="ConCLnk"; break;
    case CDlgWhat_ConELnk: m_sSection+="ConELnk"; break;
    case CDlgWhat_ConALnk: m_sSection+="ConALnk"; break;
    case CDlgWhat_ConLink: m_sSection+="ConLink"; break;
    case CDlgWhat_ChgUnit: m_sSection+="ChgUnit"; break;
    case CDlgWhat_InsSymb: m_sSection+="InsSymb"; break;
    default:               m_sSection+="?" ; break;
    };
  };

//---------------------------------------------------------------------------

void CInsertUnitDlg::SetMdlPFSection(LPCTSTR Mdl)
  {
  m_sMdlSection=m_sSection+".Mdl.";
  m_sMdlSection+=Mdl;
  };

//---------------------------------------------------------------------------

void CInsertUnitDlg::SetSymPFSection(LPCTSTR Sym)
  {
  m_sSymSection=m_sSection+".Sym.";
  m_sSymSection+=Sym;
  };

//---------------------------------------------------------------------------

LPTSTR CInsertUnitDlg::GetGroup()
  {
  switch (m_What)
    {
    case CDlgWhat_InsUnit: return FlwUnitGrp;
    case CDlgWhat_ConUnit: return FlwUnitGrp;
    case CDlgWhat_ConCLnk: return CtrlLinkGrp;
    case CDlgWhat_ConELnk: return ElecLinkGrp;
    case CDlgWhat_ConALnk: return AirLinkGrp;
    case CDlgWhat_ConLink: return FlwLinkGrp;
    case CDlgWhat_ChgUnit: return FlwUnitGrp;
    case CDlgWhat_InsSymb: return FlwUnitGrp;
    default:               return NULL;
    };
  };

bool CInsertUnitDlg::IsUnit()
  {
  switch (m_What)
    {
    case CDlgWhat_InsUnit: 
    case CDlgWhat_ConUnit: 
    case CDlgWhat_ChgUnit: 
    case CDlgWhat_InsSymb: return true;
    case CDlgWhat_ConCLnk: 
    case CDlgWhat_ConELnk: 
    case CDlgWhat_ConALnk: 
    case CDlgWhat_ConLink: 
    default:               return false;
    };
  };

bool CInsertUnitDlg::IsConstruct()
  {
  switch (m_What)
    {
    case CDlgWhat_InsUnit: 
    case CDlgWhat_ChgUnit:
    case CDlgWhat_InsSymb: return false;
    case CDlgWhat_ConUnit: 
    case CDlgWhat_ConCLnk: 
    case CDlgWhat_ConELnk: 
    case CDlgWhat_ConALnk: 
    case CDlgWhat_ConLink: 
    default:               return true;
    };
  };

//---------------------------------------------------------------------------

BOOL CInsertUnitDlg::OnInitDialog() 
  {
  CWaitCursor Wt;
  CDialog::OnInitDialog();
  CProfINIFile PF(PrjIniFile());

  PutEditNumber(m_Scl_X, 100);
  PutEditNumber(m_Scl_Y, 100);
  PutEditNumber(m_Rotate, 0);

  m_HideTag = (PF.RdInt(m_sSection, "$HideTag", 1)!=0);
  m_AllSymbols = (PF.RdInt(m_sSection, "$AllSymbols", 0)!=0);
  m_bExistingModel = false;
  UpdateData(FALSE);
  
  CBitmap BM;
  BM.LoadBitmap(IDB_MDLSELECTTREE);
  m_MdlSlctImgList.Create(16, 16, FALSE, 0, 4);
  m_MdlSlctImgList.Add(&BM, (CBitmap*)NULL);
  m_MdlTree.SetImageList(&m_MdlSlctImgList, TVSIL_NORMAL);
  
  IWinBitmap.LoadMappedBitmap(IDB_BTN_IWIN);
  CWinBitmap.LoadMappedBitmap(IDB_BTN_CWIN);
  ((CButton*)GetDlgItem(IDB_IWIN))->SetBitmap((HBITMAP)IWinBitmap);
  ((CButton*)GetDlgItem(IDB_CWIN))->SetBitmap((HBITMAP)CWinBitmap);

  CRect WinRect;
  GetWindowRect(&WinRect);
  if (m_bIsConstructSymbol)
    {
    GetDlgItem(ID_MDLDOC)->EnableWindow(FALSE);
    GetDlgItem(IDC_MDL_TREE)->EnableWindow(FALSE);
    //GetDlgItem(IDC_MORE)->EnableWindow(FALSE);
    GetDlgItem(IDC_HideTag)->EnableWindow(FALSE);
    GetDlgItem(IDC_MDLSYMBOL)->ShowWindow(SW_HIDE);
    GetDlgItem(IDB_IWIN)->ShowWindow(SW_SHOW);
    GetDlgItem(IDB_CWIN)->ShowWindow(SW_SHOW);
    SetWindowText("Construct Symbol");
    }
  else if (m_What==CDlgWhat_InsUnit)
    {
    SetWindowText("Insert Unit");
    GetDlgItem(IDC_MDLSYMBOL)->ShowWindow(SW_SHOW);
    GetDlgItem(IDB_IWIN)->ShowWindow(SW_HIDE);
    GetDlgItem(IDB_CWIN)->ShowWindow(SW_HIDE);
    }
  else if (m_What==CDlgWhat_ChgUnit || m_What==CDlgWhat_InsSymb)
    {
    SetWindowText(m_What==CDlgWhat_ChgUnit ? "Change Symbol" : "Insert Symbol");
    GetDlgItem(IDC_MDLSYMBOL)->ShowWindow(SW_SHOW);
    GetDlgItem(IDB_IWIN)->ShowWindow(SW_HIDE);
    GetDlgItem(IDB_CWIN)->ShowWindow(SW_HIDE);

    //m_MdlTree.EnableWindow(FALSE);
    GetDlgItem(IDC_TAGSTATIC)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_MDLTAG)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_MDL_TREE)->ShowWindow(SW_HIDE);

    WINDOWPLACEMENT WP;
    WP.length = sizeof(WP);
    int XShift=0;
    GetDlgItem(IDC_MDL_TREE)->GetWindowPlacement(&WP);
    XShift=WP.rcNormalPosition.left;
    GetDlgItem(IDC_MDLSYMBOL)->GetWindowPlacement(&WP);
    XShift-=WP.rcNormalPosition.left;
    CWnd * pWnd=GetWindow(GW_CHILD);
    while (pWnd)
      {
      pWnd->GetWindowPlacement(&WP);
      pWnd->SetWindowPos(NULL, WP.rcNormalPosition.left+XShift, WP.rcNormalPosition.top, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
      pWnd=pWnd->GetWindow(GW_HWNDNEXT);
      }
    GetWindowPlacement(&WP);
    SetWindowPos(NULL, 0, 0, XShift+WP.rcNormalPosition.right-WP.rcNormalPosition.left, WP.rcNormalPosition.bottom-WP.rcNormalPosition.top, SWP_NOREPOSITION|SWP_NOZORDER);

    //GetDlgItem(IDC_MDLSCL_X)->EnableWindow(FALSE);
    //GetDlgItem(IDC_MDLROT)->EnableWindow(FALSE);
    }
  else
    {
    if (IsUnit())
      m_HideTag = (PF.RdInt(m_sSection, "$HideTagConstructUnit", 0)!=0);
    else
      m_HideTag = (PF.RdInt(m_sSection, "$HideTagConstructLink", 0)!=0);
    SetWindowText(IsUnit() ? "Construct Unit" : "Construct Link");
    GetDlgItem(IDC_MDLSYMBOL)->ShowWindow(SW_HIDE);
    GetDlgItem(IDB_IWIN)->ShowWindow(SW_SHOW);
    GetDlgItem(IDB_CWIN)->ShowWindow(SW_SHOW);

    m_SymbolFrame.EnableWindow(FALSE);
    GetDlgItem(IDC_ALLSYMB)->EnableWindow(FALSE);
    GetDlgItem(IDC_MDLSCL_Y)->EnableWindow(FALSE);
    GetDlgItem(IDC_MDLSCL_X)->EnableWindow(FALSE);
    GetDlgItem(IDC_MDLROT)->EnableWindow(FALSE);
    //GetDlgItem(IDC_STATICFRAME)->EnableWindow(FALSE);
    //GetDlgItem(IDC_STATICSCALE)->EnableWindow(FALSE);
    GetDlgItem(IDC_STATICX)->EnableWindow(FALSE);
    GetDlgItem(IDC_STATICY)->EnableWindow(FALSE);
    GetDlgItem(IDC_STATICROT)->EnableWindow(FALSE);
    }
  
  //Do tool tip stuff such as changing tooltip for More/Less Button
  m_pTTC = new CCustomToolTipCtrl(this);
  //m_pTTC->UpdateTipText("Show model rotation, symbols, scaling, description and hide-tag", GetDlgItem(IDC_MORE));
  
  //ON_NOTIFY(TVN_SELCHANGING, IDC_MDL_TREE, OnTvnSelchangingMdlTree)
  ((CSpinButtonCtrl*)GetDlgItem(IDC_SPINX))->SetRange(-1000, 1000);
  ((CSpinButtonCtrl*)GetDlgItem(IDC_SPINY))->SetRange(-1000, 1000);
  ((CSpinButtonCtrl*)GetDlgItem(IDC_SPINROT))->SetRange(-360, 360);

  UDACCEL SpinAcc[] = {{0,5}, {1,10}, {2,20}};
  ((CSpinButtonCtrl*)GetDlgItem(IDC_SPINX))->SetAccel(3, SpinAcc);
  ((CSpinButtonCtrl*)GetDlgItem(IDC_SPINY))->SetAccel(3, SpinAcc);
  ((CSpinButtonCtrl*)GetDlgItem(IDC_SPINROT))->SetAccel(3, SpinAcc);

  UpdateDialogControls(this, FALSE);

  int Left=PF.RdInt(m_sSection, "$Window.Left", -3000);
  int Top=PF.RdInt(m_sSection, "$Window.Top", -3000);

  if (Left>-3000)
    {
    //Left=Range(0, Left, GetSystemMetrics(SM_CXVIRTUALSCREEN)-10);
    //Top=Range(0, Top, GetSystemMetrics(SM_CYVIRTUALSCREEN)-GetSystemMetrics(SM_CYCAPTION));
    //SetWindowPos(NULL, Left, Top, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
    SetVisibleWindowPos(this, Left, Top);
    }
  else
    CenterWindow();

  m_SymbolFrame.GetClientRect(&m_SymRectClient);

  CEdit* pTag = (CEdit*)GetDlgItem(IDC_MDLTAG);
  char buff[512];                      
  pTag->GetWindowText(buff, sizeof(buff));
  XStrTrim(buff);
  if (strlen(buff)>0)
    {
    pTag->SetFocus();
    m_bInited=true;
    return TRUE;
    }

  EnableToolTips();
  m_bInited=true;

  m_SymbolList.ResetContent();
  //LISTBOXINFO LI;
  //pSymList->GetListBoxInfo(&LI);
  //CEdit Ed;
  //Ed.Attach(LI.hwndItem);
  //Ed.SetReadOnly();


  int nModels = m_MLH.BuildList(m_sSection, GetGroup(), NULL, &m_MdlTree, &PF);
  if (nModels==0)
    {
    LogError("GrfCmds", 0, "No 'FlwUnit' Models Available");
    EndDialog(IDCANCEL);
    return FALSE;
    }

  ChangeModel((LPTSTR)(LPCTSTR)GetSelMdlName(), 0);

  ShowWindow(SW_SHOW);    

  return FALSE;
  }


//---------------------------------------------------------------------------

BOOL CInsertUnitDlg::PreTranslateMessage(MSG* pMsg) 
  {
  if (m_pTTC && HelpMngr.ShowToolTips())
    m_pTTC->RelayEvent(pMsg);
  return CDialog::PreTranslateMessage(pMsg);
  }

//---------------------------------------------------------------------------

BOOL CInsertUnitDlg::OnToolTipNotify(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
  {
  if (m_pTTC && HelpMngr.ShowToolTips())
    return m_pTTC->OnToolTipNotify(pNMHDR, CInsertUnitDlg::IDD);
  return FALSE;
  }

//---------------------------------------------------------------------------

void CInsertUnitDlg::OnDestroy() 
  {
  CProfINIFile PF(PrjIniFile());

  HTREEITEM hItem=m_MdlTree.GetSelectedItem();
  CString T=hItem?m_MdlTree.GetItemText(hItem):"";
  T.TrimLeft();
  if (T.GetLength()>=0)
    {
    //m_LastModel = pMdlCB->GetCurSel();
    m_Model=T;

    HTREEITEM hParent = TVI_ROOT, hItem;
    CString Txt[10];
    long iDepth=0;
    hItem=m_MdlTree.GetNextItem(TVI_ROOT, TVGN_CHILD);
    while (hItem && iDepth>=0)
      {
      UINT State=m_MdlTree.GetItemState(hItem, TVIS_EXPANDED );
      if (1)
        {
        CString S;
        for (int i=0; i<iDepth; i++)
          S+=Txt[i];
        S+=m_MdlTree.GetItemText(hItem);    
        CString Sect(m_sSection);
        Sect+=".Tree";
        PF.WrInt(Sect, S, State);
        }
      
      HTREEITEM h=m_MdlTree.GetNextItem(hItem, TVGN_CHILD);
      if (h)
        {
        Txt[iDepth]=m_MdlTree.GetItemText(hItem);        
        Txt[iDepth]+=":";        
        iDepth++;
        hItem=h;
        }
      else
        {
        h=m_MdlTree.GetNextItem(hItem, TVGN_NEXT);
        if (h)
          {
          hItem=h;
          }
        else
          {
          // Go One up & One along 
          hItem=m_MdlTree.GetNextItem(hItem, TVGN_PARENT);
          if (hItem)
            hItem=m_MdlTree.GetNextItem(hItem, TVGN_NEXT);
          iDepth--;
          }
        }
      }

    PF.WrStr(m_sSection, "$LastInsert", m_ModelClass());
    PF.WrInt(m_sSection, "$HideTag", m_HideTag);
    PF.WrInt(m_sSection, "$AllSymbols", m_AllSymbols);
    }


  sm_WP.length = sizeof(sm_WP);
  sm_bWPOk = GetWindowPlacement(&sm_WP);

  PF.WrInt(m_sSection, "$Window.Left", sm_WP.rcNormalPosition.left);
  PF.WrInt(m_sSection, "$Window.Top", sm_WP.rcNormalPosition.top);

  CDialog::OnDestroy();
  }

//---------------------------------------------------------------------------

void CInsertUnitDlg::OnPaint() 
  {
  //if (!m_bCreated)
  //  {
  //  // finalise the initial painting
  CPaintDC dc(this); // device context for painting
  CDCResChk ResChk(dc);

  bool Done=false;
  if (GetSelMdlIndex()>=0)
    {
    if (m_DefLoaded)
      {
      CPaintDC dc(&m_SymbolFrame); // device context for painting
      CDCResChk ResChk(dc);
      dc.FillRect(&m_SymRectClient, &CBrush(RGB(0,0,0)));
      //CGdiObject * p=dc.SelectStockObject(WHITE_PEN);
      CPen Pen(PS_SOLID, 1, RGB(192,192,192));
      CGdiObject * p=dc.SelectObject(&Pen);
      dc.PolyPolyline(&m_PolyPts[0], &m_PolyPtCnt[0], m_PolyCnt);
      dc.SelectObject(p);
      }
    else if (m_BMPLoaded)
      {
      CPaintDC dc(&m_SymbolFrame); // device context for painting
      CDCResChk ResChk(dc);
      m_SymbolBmp.Paint(&dc, &m_SymRectClient, TRUE, TRUE);
      Done=true;
      }
    else if (m_DXFLoaded && m_pDsp)
      {
      CPaintDC dc(&m_SymbolFrame); // device context for painting
      CDCResChk ResChk(dc);
      m_pDsp->Open(&dc, &m_SymRectClient);
      m_pDsp->Paint(m_SymRectClient);
      m_pDsp->Close();
      Done=true;
      }
    else if (m_What==CDlgWhat_InsUnit || m_What==CDlgWhat_ChgUnit || m_What==CDlgWhat_InsSymb)
      {
      CPaintDC dc(&m_SymbolFrame); // device context for painting
      CDCResChk ResChk(dc);
      dc.FillRect(&m_SymRectClient, &CBrush(RGB(255,0,0)));
      }
    }
  else
    {
    CPaintDC dc(&m_SymbolFrame); // device context for painting
    CDCResChk ResChk(dc);
    //dc.FillRect(&m_SymRectClient, &CBrush(RGB(0,0,0)));
    }
  
 
  // Do not call CDialog::OnPaint() for painting messages
  }

//---------------------------------------------------------------------------

void CInsertUnitDlg::OnOK() 
  {
  UpdateData(TRUE);
  if (!m_HoldFeedback)
    gs_pCmd->ExtendCmdLine("=\r");
  }

//---------------------------------------------------------------------------

void CInsertUnitDlg::OnUpdateOKBtn(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(!IsConstruct() || (m_nEntities>0));
  }

//---------------------------------------------------------------------------

void CInsertUnitDlg::OnCancel() 
  {
  UpdateData(TRUE);
  //if (m_AllowFeedback)
  gs_pCmd->ExtendCmdLine("\x1b\x1b"); //send twice to ensure Cancel works
  }

//---------------------------------------------------------------------------

void CInsertUnitDlg::OnMdldoc() 
  {
  if (m_pMdl && gs_pPrj)
    {
    CString S = GetSelMdlName();
    TagObjClass* pC=TagObjClass::FindClassId((LPSTR)(LPCTSTR)S);
    if (pC)
      S = pC->ClassIdNoVersion();
    gs_pPrj->ModelHelp((LPSTR)(LPCTSTR)S);
    }
  }

////---------------------------------------------------------------------------
//
//void CInsertUnitDlg::OnMdlhelp() 
//  {
//  ScdMainWnd()->PostMessage(WM_COMMAND, ID_HELP_SYSCAD, 0);
//  }
//
//---------------------------------------------------------------------------

void CInsertUnitDlg::SetTag(pchar Tag)
  {
  m_MdlTag.SetWindowText(Tag);
  }

//---------------------------------------------------------------------------

void CInsertUnitDlg::SetTagHidden(bool Hidden) 
  {
  m_HideTag=Hidden;
  UpdateData(false);
  }

//---------------------------------------------------------------------------

void CInsertUnitDlg::SetClass(char * AClass)
  {
  CString C(AClass);
  int iPos;
  while ((iPos=C.Find(":"))>=0)
    C.Delete(0, iPos+1);

  HTREEITEM hParent = TVI_ROOT, hItem;
  CString Txt[10];
  long iDepth=0;
  hItem=m_MdlTree.GetNextItem(TVI_ROOT, TVGN_CHILD);
  while (hItem && iDepth>=0)
    {
    CString S=m_MdlTree.GetItemText(hItem);
    if (S==C)
      {
      m_MdlTree.SelectItem(hItem);
      break;
      }

    HTREEITEM h=m_MdlTree.GetNextItem(hItem, TVGN_CHILD);
    if (h)
      {
      iDepth++;
      hItem=h;
      }
    else
      {
      h=m_MdlTree.GetNextItem(hItem, TVGN_NEXT);
      if (h)
        {
        hItem=h;
        }
      else
        {
        // Go One up & One along 
        hItem=m_MdlTree.GetNextItem(hItem, TVGN_PARENT);
        if (hItem)
          hItem=m_MdlTree.GetNextItem(hItem, TVGN_NEXT);
        iDepth--;
        }
      }
    }

  }

//---------------------------------------------------------------------------

void CInsertUnitDlg::SetBlockName(char * BlkName, char * UnitDrwGroup, char * UnitClass)
  {
  if (!BlkName)
    return;
  
  m_CUOrigBlkName=BlkName;
  m_CUOrigSymName="";
  m_bCUOrigSymbolFound=false;

  Strng SymName(BlkName);
  int SymLen=SymName.Len();
  int GrpLen=strlen(UnitDrwGroup);
  if ((SymLen>GrpLen) && 
      (SymName.XStrNICmp(UnitDrwGroup, GrpLen)==0) && 
      (SymName[GrpLen]=='$'))
    {
    SymName[GrpLen]=':';
    char * p=SymName.XStrChr('_');
    while (p)
      {
      *p=' ';
      p=SymName.XStrChr('_');
      }
    POSITION Pos=m_DrwSymbols.GetHeadPosition();
    while (Pos && !m_bCUOrigSymbolFound)
      {
      CString &S=m_DrwSymbols.GetNext(Pos);
      if (SymName.XStrICmp(S)==0)
        {
        m_CUOrigSymName=SymName;
        m_bCUOrigSymbolFound=true;
        }
      }
    }

  m_DrwGroup=UnitDrwGroup;

  FillSymbolList();
  Strng SrchSym(SymName);
  if (!m_AllSymbols)
    {
    int iColon=SrchSym.Find(':');
    if (iColon>0)
      SrchSym=SrchSym.Right(SrchSym.GetLength()-(iColon+1));
    }

  int iIndex=m_SymbolList.FindString(-1, SrchSym());
  if (iIndex>=0)
    {
    m_SymbolList.SetCurSel(iIndex);
    m_SymbolName=SymName();
    LoadSelectedSymbol();
    }
  };

//---------------------------------------------------------------------------

void CInsertUnitDlg::SetOriginalScales(double XScl, double YScl, double Rotation)
  {
  m_OriginalScales.m_bInUse=true;
  m_OriginalScales.m_dXScl=XScl;
  m_OriginalScales.m_dXScl=YScl;
  m_OriginalScales.m_dRot=Rotation;

  PutEditNumber(m_Scl_X, XScl*100);
  PutEditNumber(m_Scl_Y, YScl*100);
  PutEditNumber(m_Rotate, Rotation);
  UpdateData(FALSE);
  };

//---------------------------------------------------------------------------

void CInsertUnitDlg::ChooseNewTag()
  {
  MakeUniqueTag();
  }

//---------------------------------------------------------------------------

void CInsertUnitDlg::MakeUniqueTag()
  {
  if (!m_bTagChgd && m_ModelClass())
    {
    CUniqueTagRulesBlk TRB(m_ModelClass(), m_BaseTag());
    Strng Tag(TRB.BaseTag()), RetTag;
    Tag += TRB.SepChar();
    Tag += "1";
    if (!gs_pPrj->TestModelTagUnique(Tag(), TRB, RetTag, False))
      {
      Tag = RetTag();
      if (m_BaseTag.Len()==0 && RetTag.XStrRChr(TRB.SepChar()))
        {
        m_BaseTag = RetTag();
        m_BaseTag.SetLength(RetTag.XStrRChr(TRB.SepChar())-RetTag());
        }
      }

    SetTag(Tag());
    m_bTagChgd = 0;
    }
  }

//---------------------------------------------------------------------------

void CInsertUnitDlg::ChangeModel(LPTSTR MdlName, DWORD MdlIndex)
  {
  if (m_iHoldUpd==0)
    UpdateData(true);
  m_iHoldUpd++;

  if (MdlName==NULL && MdlIndex==0xFFFF)
    {
    m_BaseTag     = "";
    m_ModelClass  = "";
    m_DrwGroup    = "";
    FillSymbolList();
    LoadSelectedSymbol();//OnLbnSelchangeMdlsymbol();//OnSelchangeMdlsymbol();
    }
  else
    {

    if (MdlName)
      m_MLH.Find(MdlName);
    else
      m_MLH.Find(MdlIndex);
    m_BaseTag     = m_MLH.m_pBaseTag ? *m_MLH.m_pBaseTag:"";
    m_ModelClass  = m_MLH.m_pModelClass ? *m_MLH.m_pModelClass : "";
    m_DrwGroup    = m_MLH.m_pDrwGroup ? *m_MLH.m_pDrwGroup:"";

    m_SymbolList.ResetContent();
    if (m_MLH.m_pDescStr)
      {
      FillSymbolList();
      LoadSelectedSymbol();//OnLbnSelchangeMdlsymbol();//OnSelchangeMdlsymbol();
      }
    else
      {
      m_BaseTag = "";
      m_ModelClass= "";
      }

    MakeUniqueTag();
    }
  m_iHoldUpd--;
  if (m_iHoldUpd==0)
    UpdateData(false);
  //SetSymbol(m_SymbolName());
  }

//---------------------------------------------------------------------------

int CInsertUnitDlg::GetSelectedSymbol(CString & Symbol)
  {
  int iLstIndex = m_SymbolList.GetCurSel();
  if (iLstIndex>=0)
    {
    m_SymbolList.GetText(iLstIndex, Symbol);
    Symbol=Symbol.Trim();
    if (Symbol.Find(':')<0)
      {
      if (Symbol!=OriginalUnitSymbol && Symbol!=DefaultUnitSymbol && Symbol!=SeparString)
        {
        CString Symb=Symbol;
        Symbol=m_DrwGroup();
        Symbol+=":";
        Symbol+=Symb;
        }
      }
    }
  return iLstIndex;
  }

//---------------------------------------------------------------------------

CString SymbolOnly(LPCTSTR Sym)
  {
  CString S=Sym;
  int i=S.Find(':');
  if (i>=0)
    S=S.Right(S.GetLength()-(i+1));
  return S;
  };

//---------------------------------------------------------------------------

CString DrwGroupOnly(LPCTSTR Sym)
  {
  CString S=Sym;
  int i=S.Find(':');
  if (i>=0)
    S=S.Left(i);
  else
    S="";
  return S;
  };

//---------------------------------------------------------------------------

bool CInsertUnitDlg::GetSelectedSymbolFile(LPCTSTR Ext, Strng &SymName, Strng &BlkName, Strng &SymPath)
  {
  CString Symb;
  if (GetSelectedSymbol(Symb)>=0)
    {
    if (Symb.Compare(OriginalUnitSymbol)==0)
      {
      SymName=m_CUOrigBlkName;
      SymPath="";
      BlkName=m_CUOrigBlkName;
      return true;
      }
    else
      {
      if (Symb.Compare(DefaultUnitSymbol)==0)
        Symb=m_ModelClass();

      if (Symb.GetLength()>0)
        {
        Strng Folder(m_DrwGroup()), NameOnly;
        int iColon=Symb.Find(':');
        if (iColon>0)
          {
          Folder=Symb.Left(iColon);
          //Symb.Delete(0, iColon+1);
          SymName=Symb;
          NameOnly=Symb.Right(Symb.GetLength()-(iColon+1));
          }
        else
          {
          SymName=Folder();
          SymName+=":";
          SymName+=Symb;
          NameOnly=Symb;
          }
        SymPath.Set("%s%s\\%s.%s", m_bUsingPrjSymbols ? GrfSymbolFiles() : BaseGrfSymbolFiles(), Folder(), NameOnly(), Ext);
        BlkName=MakeValidBlockName(SymName());
        return true;
        }
      }
    }
  return false;
  }

//---------------------------------------------------------------------------

void CInsertUnitDlg::AdjustMRU(bool DoUpdateLock)
  {
  CProfINIFile PF(PrjIniFile());

  //Strng Disc(m_AllSymbols ? "All":m_DrwGroup());
  CString/*Strng*/ MruLb, MruSym;

  CString Mdl=GetSelMdlName();
  Mdl=Mdl.Trim();
  SetMdlPFSection(Mdl);

  if (DoUpdateLock)
    m_SymbolList.LockWindowUpdate();
  int iSepar=m_SymbolList.FindStringExact(-1, SeparString);
  if (iSepar>=0)
    {
    for (int i=0; i<iSepar; i++)
      m_SymbolList.DeleteString(0);
    }
  else
    m_SymbolList.InsertString(0, SeparString);

  iSepar=0;

  for (int im=0; im<SymMRUMax; im++)
    {
    MruLb.Format(MRUFmt, im);
    MruSym = PF.RdStr(m_sMdlSection, MruLb, NullUnitSymbol);
    if (MruSym == NullUnitSymbol)
      break;
    if (m_DrwSymbols.Find(MruSym))
      {
      if (m_AllSymbols) 
        m_SymbolList.InsertString(iSepar++, MruSym);
      else
        {
        CString SymOnly=SymbolOnly(MruSym);
        bool UseSymOnly = m_SymbolList.FindStringExact(iSepar+1, SymOnly)>=0;
        m_SymbolList.InsertString(iSepar++, UseSymOnly ? SymOnly:MruSym);
        }
      }
    }

  int iDefault=m_SymbolList.FindStringExact(-1, DefaultUnitSymbol);
  if (iDefault>iSepar)
    {
    if (iSepar>=SymMRUMax)
      {
      m_SymbolList.DeleteString(iSepar-1);
      iSepar--;
      }
    m_SymbolList.InsertString(iSepar++, DefaultUnitSymbol);
    }

  if (DoUpdateLock)
    m_SymbolList.UnlockWindowUpdate();
  }

//---------------------------------------------------------------------------

void CInsertUnitDlg::SaveLastSymbol()
  {
  CProfINIFile PF(PrjIniFile());

  CString S;
  if (GetSelectedSymbol(S)>=0)
    {
    CString Mdl=GetSelMdlName();
    Mdl=Mdl.Trim();
    SetMdlPFSection(Mdl);
    PF.WrStr(m_sMdlSection, SelectedSymbol, S);
    }
  }

//---------------------------------------------------------------------------

void CInsertUnitDlg::SelectLastSymbol(LPCTSTR RqdSymbol)
  {
  CProfINIFile PF(PrjIniFile());
  SetMdlPFSection(m_ModelClass());
  Strng LastSymb;
  if (RqdSymbol)
    LastSymb = RqdSymbol;
  else
    LastSymb = PF.RdStr(m_sMdlSection, SelectedSymbol, "");

  int i = -1;
  if (LastSymb.Len()>0)
    {
    //Try AsIs then try adjusting
    i = m_SymbolList.FindStringExact(-1, LastSymb());
    if (i==-1)
      {
      int iColon=LastSymb.Find(':');
      if (iColon>0)
        LastSymb=LastSymb.Right(LastSymb.GetLength()-(iColon+1));
      }
    i = m_SymbolList.FindStringExact(-1, LastSymb());

    }
  if (i==-1)
    {
    if (m_AllSymbols)
      LastSymb.Set("%s:%s", m_DrwGroup(), DefaultUnitSymbol);
    else
      LastSymb = DefaultUnitSymbol;
    i = m_SymbolList.FindStringExact(-1, LastSymb());
    }

  m_SymbolList.SetCurSel(Max(0, i));
  }

//---------------------------------------------------------------------------

void CInsertUnitDlg::FillSymbolList()
  {
  CWaitCursor Wait;

  m_SymbolList.LockWindowUpdate();

  CString GrpTxt("Drawing Group: ");
  m_SymbolList.ResetContent();

  if (m_bMdlValid && m_DrwGroup())
    {
    if (!m_bCUOrigSymbolFound)
      m_SymbolList.AddString(OriginalUnitSymbol);
    m_SymbolList.AddString(DefaultUnitSymbol);

    if (m_AllSymbols)
      {
      GrpTxt+="ALL";
      POSITION Pos=m_DrwSymbols.GetHeadPosition();
      while (Pos)
        {
        CString &S=m_DrwSymbols.GetNext(Pos);
        m_SymbolList.AddString(S);
        }
      }
    else
      {
      GrpTxt+=m_DrwGroup();
      int GrpLen=m_DrwGroup.GetLength();
      POSITION Pos=m_DrwSymbols.GetHeadPosition();
      while (Pos)
        {
        CString &S=m_DrwSymbols.GetNext(Pos);
        if (strncmp(S, m_DrwGroup(), GrpLen)==0 && S[GrpLen]==':')
          m_SymbolList.AddString(S.Right(S.GetLength()-(GrpLen+1)));
        }
      }

    AdjustMRU(false);
    if (m_What==CDlgWhat_ChgUnit)
      {
      if (!m_bCUOrigSymbolFound)
        SelectLastSymbol(OriginalUnitSymbol);
      else
        SelectLastSymbol(m_CUOrigSymName());
      }
    else
      SelectLastSymbol();
    }
  else
    {                              
    m_BaseTag = "";
    for (word i=0,c=0; i<m_ModelClass.Len(); i++)
      if (isupper(m_ModelClass[i]))
        {
        m_BaseTag.SetLength(c+1);
        m_BaseTag[c++] = m_ModelClass[i];
        }
    }

  m_DrwGrpStatic.SetWindowText(GrpTxt);
  m_SymbolList.UnlockWindowUpdate();
  }

//---------------------------------------------------------------------------

bool CInsertUnitDlg::LoadBitmap()
  {
  bool Ret=false;
  m_SymbolBmp.Clear();

  Strng Name, Blk, Path;
  if (GetSelectedSymbolFile("BMP", Name, Blk, Path))
    {
    if (m_SymbolBmp.LoadBitmap(Path(), 4))
      Ret=true;
    }
  InvalidateRect(m_SymRectClient, TRUE); 
  m_SymbolFrame.Invalidate();
  return Ret;
  }

//---------------------------------------------------------------------------

bool CInsertUnitDlg::LoadDXF()
  {
  //if (!m_bCreated)
  //  return false;
  bool Ret=false;

  m_SymbolBmp.Clear();


  Strng Name, Blk, Path;
  if (GetSelectedSymbolFile("DXF", Name, Blk, Path))
    {
    if (FileExists(Path()))
      {
      if (m_pDsp==NULL)
        {
        m_pDsp    = new Grf3D_Display(&m_SymbolFrame, m_SymRectClient.Width()*10, m_SymRectClient.Height()*10);
        m_pVwPort = m_pDsp->AddView(Vw_Top, 0., 0., 1., 1., NULL);
        }

      delete m_pDrw;
      m_pDrw = new DXF_Drawing(m_pDsp, "", True);
      m_pDsp->SetViewDrawing(m_pDrw, m_pVwPort);
      int err = m_pDrw->Load(Path(), Load_All, false);//test_mode);
      m_pDsp->Open(NULL, &m_SymRectClient);
      m_pDsp->SetZoom(Zoom_All); 
      //m_pDsp->SetZoom(Zoom_Page, -10, -10, 20, 20); 
      m_pDsp->SetZoom(Zoom_PanRel, 0.0, 0.0, 1.1, 1.1);
      m_pDsp->Close();
      Ret=true;
      }
    }
   
  InvalidateRect(m_SymRectClient, TRUE); 
  m_SymbolFrame.Invalidate();
  return Ret;
  }

//---------------------------------------------------------------------------

static inline double Deg2Rad(double x) { return x*PI/180; };
static inline double Rad2Deg(double x) { return x*180/PI; };

bool CInsertUnitDlg::LoadDefDraw()
  {
  //if (!m_bCreated)
  //  return false;
  bool Ret=false;
  NodeGrfInfo * pNGI=gs_pPrj->GetNodeDrawings();

  int subfig = 1;
  if (pNGI==NULL)
    return false;

  #define DORANGE(X,Y) { XMn=Min(XMn,X);XMx=Max(XMx,X);YMn=Min(YMn,Y);YMx=Max(YMx,Y); };
  #define INITRANGE() { XMn=1e10; XMx=-XMn; YMn=XMn; YMx=XMx; };
  #define LOADPT(pt, x, y) { pt.X = x; pt.Y = y; pt.Z=0.0;};
  double XMn, XMx, YMn, YMx, X, Y;
  char *pIOPt,*pVar,*pType;

  // Scan for Extents
  bool GotIt=false;
  INITRANGE();
  for (long Code=pNGI->GetLVal(TRUE); Code != DD_EndList; Code=pNGI->GetLVal())
    {
    switch (Code)
      {
      case DD_Name:
        {
        CString Symbol(pNGI->GetPChar());
        CString DrwGrp(Symbol);
        int iColon=Symbol.Find(':');
        if (iColon>0)
          {
          DrwGrp=Symbol.Left(iColon);
          Symbol=Symbol.Right(Symbol.GetLength()-iColon-1);
          }
        GotIt=m_ModelClass.XStrICmp((LPCTSTR)Symbol)==0;
        break;
        }
      case DD_Scale:
        pNGI->SetScale(pNGI->GetDVal()/1000.0);
        break;
      case DD_In:
      case DD_Out:
        pIOPt=pNGI->GetPChar();
        while (pNGI->GetCoOrd(X,Y))
          DORANGE(X,Y);
        break;
      case DD_Poly:
        while (pNGI->GetCoOrd(X,Y))
          DORANGE(X,Y);
        break;
      case DD_Arc:
        if (pNGI->GetCoOrd(X,Y))
          {
          double Rad=pNGI->GetLength();
          DORANGE(X+Rad,Y+Rad);
          DORANGE(X-Rad,Y-Rad);
          double S=0,E=360;
          pNGI->GetPair(S,E);
          }
        break;
      case DD_Arc3:
        while (pNGI->GetCoOrd(X,Y))
          DORANGE(X,Y);
        break;
      case DD_TagPos:
        {
        double X,Y;
        if (pNGI->GetCoOrd(X,Y))
          {
          //int iJust=pNGI->GetLVal();
          //int iVisible=pNGI->GetLVal();
          DORANGE(X,Y);
          }
        break;
        }
      case DD_ValuePos:
        {
        double X,Y;
        if (pNGI->GetCoOrd(X,Y))
          {
          //int iJust=pNGI->GetLVal();
          //int iVisible=pNGI->GetLVal();
          DORANGE(X,Y);
          }
        break;
        }
      case DD_Dynm:
        pVar  = pNGI->GetAChar();
        pType = pNGI->GetAChar();
        while (pNGI->GetCoOrd(X,Y))
          DORANGE(X,Y);
        break;
      case DD_End:
        if (GotIt)
          goto Continue;
        INITRANGE();
        pNGI->SetScale(1.0);
        break;
      default: ;
      }
    }

Continue:

  double XScl=m_SymRectClient.Width()/GTZ(XMx-XMn);
  double YScl=m_SymRectClient.Height()/GTZ(YMx-YMn);
  double Scl=Min(XScl, YScl);
  double XOff=-XMn*XScl;
  double YOff=-YMn*YScl;

  #define XSCL(X) int(XOff+X*Scl*0.9)
  #define YSCL(Y) int(YOff-Y*Scl*0.9)

  m_PolyCnt=0;
  int PtCnt=0;

  for (Code=pNGI->GetLVal(TRUE); Code != DD_EndList; Code=pNGI->GetLVal())
    {
    switch (Code)
      {
      case DD_Name:
        {
        CString Symbol(pNGI->GetPChar());
        CString DrwGrp(Symbol);
        int iColon=Symbol.Find(':');
        if (iColon>0)
          {
          DrwGrp=Symbol.Left(iColon);
          Symbol=Symbol.Right(Symbol.GetLength()-iColon-1);
          }
        GotIt=m_ModelClass.XStrICmp((LPCTSTR)Symbol)==0;
        break;
        }
      case DD_Scale:
        pNGI->SetScale(pNGI->GetDVal()/1000.0);
        break;
      case DD_In:
      case DD_Out:
        pNGI->GetPChar();
        while (pNGI->GetCoOrd(X,Y))
          {
          if (GotIt)
            {
            }
          };
        break;
      case DD_Dynm:
        pVar = pNGI->GetAChar();
        pType = pNGI->GetAChar();
        while (pNGI->GetCoOrd(X,Y))
          {
          if (GotIt)
            {
            }
          }
        break;
      case DD_Poly:
        if(pNGI->GetCoOrd(X,Y))
          {
          if (GotIt)
            {
            m_PolyPts.SetAtGrow(PtCnt++, CPoint(XSCL(X),YSCL(Y)));
            m_PolyPtCnt.SetAtGrow(m_PolyCnt, 1);
            while (pNGI->GetCoOrd(X,Y))
              {
              m_PolyPts.SetAtGrow(PtCnt++, CPoint(XSCL(X),YSCL(Y)));
              m_PolyPtCnt[m_PolyCnt]++;
              }
            m_PolyCnt++;
            }
          }
        break;
      case DD_Arc:
        if (pNGI->GetCoOrd(X,Y))
          {
          double Rad=pNGI->GetLength();
          double S=0,E=360;
          pNGI->GetPair(S,E);
          if (GotIt)
            {
            long NSegs=Max(1,int((E-S)*6/90+0.5)); // 6 segs per 90deg
            double dA=(E-S)/NSegs;
            double A=S;
            double ax=X+Rad*cos(Deg2Rad(A));
            double ay=Y+Rad*sin(Deg2Rad(A));
            m_PolyPts.SetAtGrow(PtCnt++, CPoint(XSCL(ax),YSCL(ay)));
            m_PolyPtCnt.SetAtGrow(m_PolyCnt, 1);
            for (int i=0; i<NSegs; i++)
              {
              A+=dA;
              ax=X+Rad*cos(Deg2Rad(A));
              ay=Y+Rad*sin(Deg2Rad(A));
              m_PolyPts.SetAtGrow(PtCnt++, CPoint(XSCL(ax),YSCL(ay)));
              m_PolyPtCnt[m_PolyCnt]++;
              }
            m_PolyCnt++;
            }
          }
        break;
      case DD_Arc3:
        {
        double Xs,Xm,Xe,Ys,Ym,Ye;
        if (pNGI->GetCoOrd(Xs,Ys) && pNGI->GetCoOrd(Xm,Ym) && pNGI->GetCoOrd(Xe,Ye))
          {
          if (GotIt)
            {
            double A[3], B[3], C[3];
            A[0]=2*(Xs-Xm);
            A[1]=2*(Xe-Xs);
            A[2]=2*(Xm-Xe);
            B[0]=Xs*Xs-Xm*Xm+Ys*Ys-Ym*Ym;
            B[1]=Xe*Xe-Xs*Xs+Ye*Ye-Ys*Ys;
            B[2]=Xm*Xm-Xe*Xe+Ym*Ym-Ye*Ye;
            C[0]=-2*(Ys*Ys-Ym*Ym);
            C[1]=-2*(Ye*Ye-Ys*Ys);
            C[2]=-2*(Ym*Ym-Ye*Ye);

            int i1=0;
            int i2=1;
            for (int j=0; j<3; j++) // 3 combinations
              {
              if (A[i1]*C[i2]!=0 || C[i1]*B[i2]!=0)
                break;
              if (j==0)
                i2=2;
              else if (j==1)
                i1=1;
              }
            if (j<3) // AllOK
              {
              const double Xa=B[i2]*C[i1]-B[i1]*C[i2];
              const double Xb=A[i1]*C[i2]-C[i1]*B[i2];
              const double X=Xa/NZ(Xb);
              //double X=(B[i2]*C[i1]-B[i1]*C[i2])/(A[i1]*C[i2]-C[i1]*B[i2]);
              const double Y=-(X*A[i1]+B[i1])/C[i1];
              double Rad=sqrt(Sqr(Xs-X)+Sqr(Ys-Y));
              double Sa=Rad2Deg(atan2(Ys-Y, Xs-X));
              double Ma=Rad2Deg(atan2(Ym-Y, Xm-X));
              double Ea=Rad2Deg(atan2(Ye-Y, Xe-X));
              if ((Ma-Sa)*(Ea-Ma)<0) // must move End
                Ea=Ma+(Ma-Sa);

              long NSegs=Max(1,int(fabs(Ea-Sa)*6/90+0.5)); // 6 segs per 90deg
              double dA=(Ea-Sa)/NSegs;
              double An=Sa;
              double ax=X+Rad*cos(Deg2Rad(An));
              double ay=Y+Rad*sin(Deg2Rad(An));
              m_PolyPts.SetAtGrow(PtCnt++, CPoint(XSCL(ax),YSCL(ay)));
              m_PolyPtCnt.SetAtGrow(m_PolyCnt, 1);
              for (int i=0; i<NSegs; i++)
                {
                An+=dA;
                ax=X+Rad*cos(Deg2Rad(An));
                ay=Y+Rad*sin(Deg2Rad(An));
                m_PolyPts.SetAtGrow(PtCnt++, CPoint(XSCL(ax),YSCL(ay)));
                m_PolyPtCnt[m_PolyCnt]++;
                }
              m_PolyCnt++;
              }
            }
          }
        break;
        }
      case DD_TagPos:
        if(pNGI->GetCoOrd(X,Y))
          {
          if (GotIt)
            {
            //int iJust=pNGI->GetLVal();
            //int iVisible=pNGI->GetLVal();
            //m_PolyPts.SetAtGrow(PtCnt++, CPoint(XSCL(X),YSCL(Y)));
            //m_PolyPtCnt.SetAtGrow(m_PolyCnt, 1);
            //while (pNGI->GetCoOrd(X,Y))
            //  {
            //  m_PolyPts.SetAtGrow(PtCnt++, CPoint(XSCL(X),YSCL(Y)));
            //  m_PolyPtCnt[m_PolyCnt]++;
            //  }
            //m_PolyCnt++;
            }
          }
        break;
      case DD_ValuePos:
        if(pNGI->GetCoOrd(X,Y))
          {
          if (GotIt)
            {
            //int iJust=pNGI->GetLVal();
            //int iVisible=pNGI->GetLVal();
            //m_PolyPts.SetAtGrow(PtCnt++, CPoint(XSCL(X),YSCL(Y)));
            //m_PolyPtCnt.SetAtGrow(m_PolyCnt, 1);
            //while (pNGI->GetCoOrd(X,Y))
            //  {
            //  m_PolyPts.SetAtGrow(PtCnt++, CPoint(XSCL(X),YSCL(Y)));
            //  m_PolyPtCnt[m_PolyCnt]++;
            //  }
            //m_PolyCnt++;
            }
          }
        break;
      case DD_End:
        if (GotIt)
          {
          Ret=true;
          goto Done;
          }
        pNGI->SetScale(1.0);
        break;
      default: ;
      }
    }

Done:

  InvalidateRect(m_SymRectClient, TRUE); 
  m_SymbolFrame.Invalidate();
  return Ret;
  }

//---------------------------------------------------------------------------

bool CInsertUnitDlg::LoadSelectedSymbol() 
  {
  bool Chg=false;
  if (m_SymbolList.GetCurSel()<0)
    m_SymbolList.SetCurSel(0);

  CString Sym;
  int iSel=GetSelectedSymbol(Sym);
  if (iSel>=0)
    {
    if (Sym.Compare(SeparString )==0)
      {
      if (iSel>m_iPrevSelSymbol)
        m_SymbolList.SetCurSel(iSel+1);
      else 
        m_SymbolList.SetCurSel(iSel-1);
      }
    m_iPrevSelSymbol=m_SymbolList.GetCurSel();

    switch (m_What)
      {
      case CDlgWhat_InsUnit:
      case CDlgWhat_ChgUnit:
      case CDlgWhat_InsSymb:
        {
        m_DefLoaded=false;
        m_BMPLoaded=false;
        m_DXFLoaded=false;

        if (LoadDXF())
          {
          m_DXFLoaded=true;
          Chg=true;
          }
        else if (LoadBitmap())
          {
          m_BMPLoaded=true;
          Chg=true;
          }    
        else if (LoadDefDraw())
          {
          m_DefLoaded=true;
          Chg=true;
          };
        if (Chg)
          RecoverMdlScale();
        return Chg;
        }
      }
    }
  return false;
  }

//---------------------------------------------------------------------------

void CInsertUnitDlg::SaveMdlData() 
  {
  if (m_RecoveringData)
    return;

  int iMdlIndex = GetSelMdlIndex();
  CString Symbol;
  int iLstIndex=GetSelectedSymbol(Symbol);
  if (iLstIndex>=0 && iMdlIndex>=0)
    {
    CProfINIFile PF(PrjIniFile());

    char *p, MdlData[2048];
    CString Mdl=GetSelMdlName();
    Mdl.TrimLeft();
    Mdl.TrimRight();

    strcpy(MdlData, Symbol);
    strcat(MdlData, ".");
    p=MdlData+strlen(MdlData);
    SetMdlPFSection(Mdl);
    if (m_OriginalScales.m_bInUse)
      {
      // Dont Save the data
      }
    else
      {
      strcpy(p, "XFactor");
      PF.WrDouble(m_sMdlSection, MdlData, GetEditNumber(m_Scl_X)/100);
      strcpy(p, "YFactor");
      PF.WrDouble(m_sMdlSection, MdlData, GetEditNumber(m_Scl_Y)/100);
      strcpy(p, "Rotation");
      PF.WrDouble(m_sMdlSection, MdlData, GetEditNumber(m_Rotate));
      }

    Strng MruLb0, MruLb1, MruSym;
    int im;
    for (im=0; im<SymMRUMax; im++)
      {
      MruLb0.Set(MRUFmt, im-1);
      MruLb1.Set(MRUFmt, im);
      MruSym = PF.RdStr(m_sMdlSection, MruLb1(), NullUnitSymbol);
      if (MruSym.XStrICmp(Symbol)==0 || MruSym.XStrICmp(NullUnitSymbol)==0)
        break;
      }

    for (im=Min(im, SymMRUMax-1); im>0; im--)
      {
      MruLb0.Set(MRUFmt, im-1);
      MruLb1.Set(MRUFmt, im);
      MruSym = PF.RdStr(m_sMdlSection, MruLb0(), NullUnitSymbol);
      PF.WrStr(m_sMdlSection, MruLb1(), MruSym());
      }

    MruLb0.Set(MRUFmt, 0);
    PF.WrStr(m_sMdlSection, MruLb0(), Symbol);
    }
  }

//---------------------------------------------------------------------------

void CInsertUnitDlg::RecoverMdlScale() 
  {
  m_RecoveringData++;        
  int iMdlIndex = GetSelMdlIndex();
  CString Symbol;
  int iLstIndex=GetSelectedSymbol(Symbol);
  if (iLstIndex>=0 && iMdlIndex>=0)
    {
    char *p, MdlData[2048];

    CString Mdl=GetSelMdlName();
    Mdl.TrimLeft();
    Mdl.TrimRight();

    strcpy(MdlData, Symbol);
    strcat(MdlData, ".");
    p=MdlData+strlen(MdlData);
    SetMdlPFSection(Mdl);

    if (m_OriginalScales.m_bInUse)
      {
      PutEditNumber(m_Scl_X, m_OriginalScales.m_dXScl*100);
      PutEditNumber(m_Scl_Y, m_OriginalScales.m_dXScl*100);
      PutEditNumber(m_Rotate, m_OriginalScales.m_dRot);
      }
    else
      {
      CProfINIFile PF(PrjIniFile());
      strcpy(p, "XFactor");
      double X=PF.RdDouble(m_sMdlSection, MdlData, 1);
      strcpy(p, "YFactor");
      double Y=PF.RdDouble(m_sMdlSection, MdlData, 1);
      strcpy(p, "Rotation");
      double Rot=PF.RdDouble(m_sMdlSection, MdlData, 0);
      
      PutEditNumber(m_Scl_X, X*100);
      PutEditNumber(m_Scl_Y, Y*100);
      PutEditNumber(m_Rotate, Rot);
      }

    if (m_iHoldUpd==0)
      UpdateData(false);
    }
  m_RecoveringData--;        
  }

//---------------------------------------------------------------------------

void CInsertUnitDlg::CompleteForUse(bool ForceUpdateSymLists) 
  {
  UpdateData(true);

  CString T=GetSelMdlName();
  T=T.Trim();
  m_Model=T;

  m_MdlTag.GetWindowText(T);
  T=T.Trim();
  m_Tag=T;

  Strng Path;
  GetSelectedSymbolFile("DXF", m_SymbolName, m_BlockName, Path);
  }

void CInsertUnitDlg::SaveLastModelData() 
  {

  SaveMdlData();

  //if (/*m_bSymbolChanged ||*/ ForceUpdateSymLists)
    {
    m_bSymbolChanged=false;
    SaveLastSymbol();
    AdjustMRU(true);
    SelectLastSymbol();
    }
  }

//---------------------------------------------------------------------------

void CInsertUnitDlg::SetEntityCount(int Entities)
  {
  m_nEntities = Entities;
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

void CInsertUnitDlg::SetCWin(flag On)
  {
  CButton *p=(CButton *)GetDlgItem(IDB_CWIN);
  p->SetState(On);
  }

//---------------------------------------------------------------------------

void CInsertUnitDlg::SetIWin(flag On)
  {
  CButton *p=(CButton *)GetDlgItem(IDB_IWIN);
  p->SetState(On);
  }

//---------------------------------------------------------------------------

CString CInsertUnitDlg::GetSelMdlName()
  {
  HTREEITEM hItem=m_MdlTree.GetSelectedItem();
  if (hItem)
    {
    DWORD i=m_MdlTree.GetItemData(hItem);
    if (i>=0 && i<(DWORD)m_MLH.m_ModelClassLst.Length())
      return CString(m_MLH.m_ModelClassLst.AtIndex(i)->Str());
    //return m_MdlTree.GetItemText(hItem);
    }
  return CString("");
  }

//---------------------------------------------------------------------------

long CInsertUnitDlg::GetSelMdlIndex()
  {
  HTREEITEM hItem=m_MdlTree.GetSelectedItem();
  if (hItem)
    {
    DWORD Data=m_MdlTree.GetItemData(hItem);
    if (Data<0xFFFF)
      return Data;
    }
  return -1;
  }

//---------------------------------------------------------------------------

void CInsertUnitDlg::OnIwin() 
  {
  if (!m_HoldFeedback)
    gs_pCmd->ExtendCmdLine(" iwin ");
  SetIWin(TRUE);
  }

//---------------------------------------------------------------------------

void CInsertUnitDlg::OnCwin() 
  {
  if (!m_HoldFeedback)
    gs_pCmd->ExtendCmdLine(" cwin ");
  SetCWin(TRUE);
  }

//---------------------------------------------------------------------------

void CInsertUnitDlg::OnChangeMdltag() 
  {
  CString Tag;
  m_MdlTag.GetWindowText(Tag);
  Tag=Tag.Trim();
  m_bTagChgd = Tag.GetLength()>0;
  if (!m_bTagChgd)
    ChangeModel((LPTSTR)(LPCTSTR)GetSelMdlName(), 0);
  }

//---------------------------------------------------------------------------

void CInsertUnitDlg::OnTvnSelchangedMdlTree(NMHDR *pNMHDR, LRESULT *pResult)
  {
  LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
  
  m_bMdlValid=false;
  HTREEITEM hItem=m_MdlTree.GetSelectedItem();
  bool Done=false;
  if (hItem)
    {
    CString S=m_MdlTree.GetItemText(hItem);
    DWORD Data=m_MdlTree.GetItemData(hItem);
    if (Data<0xFFFF)
      {
      m_bMdlValid=true;
      ChangeModel(NULL, Data);
      Done=true;
      }
    }

  if (!Done)
    {
    ChangeModel(NULL, 0xFFFF);
    InvalidateRect(m_SymRectClient, TRUE); 
    m_SymbolFrame.Invalidate();
    }
  
  *pResult = 0;
  }

//---------------------------------------------------------------------------

void CInsertUnitDlg::OnAllsymb() 
  {
  UpdateData(TRUE);
  //m_bUsePrevSymb = 1;
  ChangeModel((LPTSTR)(LPCTSTR)GetSelMdlName(), 0);
  //m_bUsePrevSymb = 0;
  }

//---------------------------------------------------------------------------

void CInsertUnitDlg::OnLbnSelchangeMdlsymbol()
  {
  if (m_iHoldUpd==0)
    UpdateData(true);
  m_iHoldUpd++;

  bool Chg=LoadSelectedSymbol();

  m_iHoldUpd--;
  if (m_iHoldUpd==0)
    {
    UpdateData(false);
    if (Chg && (m_What==CDlgWhat_ChgUnit) && m_bCreated && !m_HoldFeedback)
      {
      m_bSymbolChanged=true;
      gs_pCmd->ExtendCmdLine("=");
      };
    }
  }

//---------------------------------------------------------------------------

void CInsertUnitDlg::OnEnChangeMdlsclX()
  {
  if (m_bCreated)
    {
    UpdateData(TRUE);

    if (m_What==CDlgWhat_ChgUnit && !m_HoldFeedback)
      gs_pCmd->ExtendCmdLine("=");

    InvalidateRect(m_SymRectClient, TRUE); 
    m_SymbolFrame.Invalidate();

    //SaveMdlData();
    }
  }


//---------------------------------------------------------------------------

void CInsertUnitDlg::OnEnChangeMdlsclY()
  {
  if (m_bCreated)
    {
    UpdateData(TRUE);

    if (m_What==CDlgWhat_ChgUnit && !m_HoldFeedback)
      gs_pCmd->ExtendCmdLine("=");

    InvalidateRect(m_SymRectClient, TRUE); 
    m_SymbolFrame.Invalidate();

    //SaveMdlData();
    }
  }

//---------------------------------------------------------------------------

void CInsertUnitDlg::OnEnChangeMdlsclRot()
  {
  if (m_bCreated)
    {
    UpdateData(TRUE);

    if (m_What==CDlgWhat_ChgUnit && !m_HoldFeedback)
      gs_pCmd->ExtendCmdLine("=");
    
    InvalidateRect(m_SymRectClient, TRUE); 
    m_SymbolFrame.Invalidate();

    //SaveMdlData();
    }
  }

//---------------------------------------------------------------------------

void CInsertUnitDlg::OnBnClickedHidetag()
  {
  if (m_What==CDlgWhat_ChgUnit && !m_HoldFeedback)
    gs_pCmd->ExtendCmdLine("=");
  }

//===========================================================================
//
//
//
//===========================================================================


void CInsertUnitDlg::OnStnClickedStaticframe()
  {
  // TODO: Add your control notification handler code here
  }

void CInsertUnitDlg::OnBnClickedOk()
  {
  // TODO: Add your control notification handler code here
  OnOK();
  }
