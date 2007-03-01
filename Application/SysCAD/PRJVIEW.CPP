//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
#include "resource.h"
#include "gpfuncs.h"
#include "gpwfuncs.h"
#include "prjview.h"
#include "project.h"
//#include "prjexec.h"
#include "grfdoc.h"
#include "accnode.h"
#include "chngtag.h"
#include "selctwnd.h"
#include "nrecipe.h"
#include "scd_wm.h"
#include "apprunmngr.h"
#include "sfe_base.h"
//#include "optoff.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//===========================================================================

IMPLEMENT_DYNCREATE(CPrjForm, CFormView)

int CPrjForm::iOrdCol[4]={1,0,2,3};
int CPrjForm::iOrdDir[4]={1,1,1,1};
int CPrjForm::iTROrdCol[5]={0,1,2,3,4};
int CPrjForm::iTROrdDir[5]={1,1,1,1,1};

CPrjForm::CPrjForm()
  : CFormView(CPrjForm::IDD)
  {
  //{{AFX_DATA_INIT(CPrjForm)
  m_Symbolic = SymbolicPaths();
  m_FileView = 0;
  m_ModelGroup = 0;
  //}}AFX_DATA_INIT
  bShowAllClasses = 0;
  bMdlTreeBuilt = 0;
  bFileTreeBuilt = 0;
  bCnvTreeBuilt = 0;
  bTagRefListBuilt = 0;
  bEnvListBuilt = 0;
  bMissingNotified = 0;

  nFilesInList = 0;
  nTagRefInList = 0;

  hCfgProcess = NULL;             //process handle for the editor
  dwCfgProcessId = 0;          //process ID for the editor
  }

//---------------------------------------------------------------------------

CPrjForm::~CPrjForm()
  {
  }

//---------------------------------------------------------------------------

void CPrjForm::DoDataExchange(CDataExchange* pDX)
  {
  CFormView::DoDataExchange(pDX);

  if (!pDX->m_bSaveAndValidate)
    m_Symbolic = SymbolicPaths();

  //{{AFX_DATA_MAP(CPrjForm)
	DDX_Control(pDX, IDC_PF_FILELIST, FileList);
  DDX_Control(pDX, IDC_PF_ENVLIST,  EnvList);
  DDX_Control(pDX, IDC_PF_MDLTREE,  MdlTree);
  DDX_Control(pDX, IDC_PF_FILETREE, FileTree);
  DDX_Control(pDX, IDC_PF_CNVTREE,  CnvTree);
  DDX_Control(pDX, IDC_PF_REFTAGS,  TagRefList);
  DDX_Control(pDX, IDC_PF_TAB, m_Tab);
	DDX_Check(pDX, IDC_PF_SYMBOLIC, m_Symbolic);
	DDX_Radio(pDX, IDC_PF_FOLDERRADIO, m_FileView);
	DDX_Radio(pDX, IDC_PF_TYPERADIO, m_ModelGroup);
	//}}AFX_DATA_MAP

  if (pDX->m_bSaveAndValidate)
    {
    SetSymbolicPaths(m_Symbolic);
    ScdPFUser.WrInt("General", "SymbolicPaths", m_Symbolic);
    }
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CPrjForm, CFormView)
  //{{AFX_MSG_MAP(CPrjForm)
  ON_BN_CLICKED(IDC_PF_REFRESH, OnRefresh)
  ON_BN_CLICKED(IDC_PF_BUTTON2, OnButton2)
  ON_WM_SIZE()
  ON_NOTIFY(TCN_SELCHANGE, IDC_PF_TAB, OnSelchangeTab)
  ON_NOTIFY(NM_RCLICK, IDC_PF_ENVLIST, OnRclickEnvList)
  ON_NOTIFY(NM_DBLCLK, IDC_PF_ENVLIST, OnDblclkEnvList)
  ON_NOTIFY(NM_RCLICK, IDC_PF_MDLTREE, OnRclickMdlTree)
  ON_NOTIFY(NM_DBLCLK, IDC_PF_MDLTREE, OnDblclkMdlTree)
  ON_NOTIFY(NM_RCLICK, IDC_PF_FILETREE, OnRclickFileTree)
  ON_NOTIFY(NM_DBLCLK, IDC_PF_FILETREE, OnDblclkFileTree)
	ON_NOTIFY(HDN_ITEMCLICK, IDC_PF_FILELIST, OnItemclickPfFilelist)
  ON_NOTIFY(NM_RCLICK, IDC_PF_REFTAGS, OnRclickRefTagList)
  ON_NOTIFY(NM_DBLCLK, IDC_PF_REFTAGS, OnDblclkRefTagList)
	ON_BN_CLICKED(IDC_PF_SYMBOLIC, OnSymbolic)
	ON_BN_CLICKED(IDC_PF_FOLDERRADIO, OnFileViewFolder)
	ON_BN_CLICKED(IDC_PF_GROUPRADIO, OnFileViewGroup)
	ON_BN_CLICKED(IDC_PF_LISTRADIO, OnFileViewList)
	ON_BN_CLICKED(IDC_PF_GROUPBYGRF, OnRefresh)
  ON_WM_CREATE()
	ON_BN_CLICKED(IDC_PF_TYPERADIO, OnModelGroupRadio)
	ON_BN_CLICKED(IDC_PF_GRAPHICSRADIO, OnModelGroupRadio)
	//}}AFX_MSG_MAP
  ON_COMMAND_RANGE(IDM_PRJ_ACCESS, IDM_PRJ_ACCESSLAST, OnMenu)
  ON_COMMAND_RANGE(IDM_PRJ_FILEEDIT, IDM_PRJ_FILEACCESS, OnMenu)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

#ifdef _DEBUG
void CPrjForm::AssertValid() const
  {
  CFormView::AssertValid();
  }

void CPrjForm::Dump(CDumpContext& dc) const
  {
  CFormView::Dump(dc);
  }
#endif //_DEBUG

//---------------------------------------------------------------------------

void CPrjForm::OnInitialUpdate()
  {
  iPrevTab = -2;
  CFormView::OnInitialUpdate();
  //restore window position...
  CString Title = GetDocument()->GetTitle();
  gs_pPrj->RestoreOneWindow((char*)(const char*)Title, this, false);

  TC_ITEM Item;
  Item.mask = TCIF_TEXT;
  Item.pszText = "Files";
  m_Tab.InsertItem(0, &Item);
  Item.pszText = "Models";
  m_Tab.InsertItem(1, &Item);
  Item.pszText = "Environment";
  m_Tab.InsertItem(2, &Item);
  Item.pszText = "Conversions";
  m_Tab.InsertItem(3, &Item);
  Item.pszText = "RefTags";
  m_Tab.InsertItem(4, &Item);
  m_Tab.SetCurSel(0);
  CBitmap BM;
  BM.LoadBitmap(IDB_FILELISTIMGS);
  VERIFY(FileImgList.Create(16, 16, FALSE, 0, 2));
  FileImgList.Add(&BM, (CBitmap*)NULL);
  CImageList* pPrev = FileTree.SetImageList(&FileImgList, TVSIL_NORMAL);
//  char Buff[512];
//  sprintf(Buff, "%s  :  %s", CfgFile(), PrjFile());
//  SetDlgItemText(IDC_PF_TXT, Buff);

  EnvList.InsertColumn(0, "Name",  LVCFMT_LEFT, 20);
  EnvList.InsertColumn(1, "Value", LVCFMT_LEFT, 20);
  EnvList.SetImageList(&FileImgList , LVSIL_SMALL);

  FileList.InsertColumn(0, "File", LVCFMT_LEFT, 20);
  FileList.InsertColumn(1, "Path", LVCFMT_LEFT, 20);
  FileList.InsertColumn(2, "Group", LVCFMT_LEFT, 20);
  FileList.InsertColumn(3, "Exists", LVCFMT_LEFT, 20);
  FileList.SetImageList(&FileImgList, LVSIL_SMALL);

  TagRefList.InsertColumn(0, "Referenced By", LVCFMT_LEFT, 40);
  TagRefList.InsertColumn(1, "Type", LVCFMT_LEFT, 40);
  TagRefList.InsertColumn(2, "Referenced Tag", LVCFMT_LEFT, 40);
  //TagRefList.InsertColumn(3, "Gets", LVCFMT_LEFT, 40);
  //TagRefList.InsertColumn(4, "Sets", LVCFMT_LEFT, 40);

  ChangeTab();
  }

//---------------------------------------------------------------------------

void CPrjForm::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
  {
  if (lHint==1)
    {
    //char Buff[512];
    //sprintf(Buff, "%s  :  %s", CfgFile(), PrjFile());
    //SetDlgItemText(IDC_PF_TXT, Buff);
    //SetDlgItemText(IDC_PF_TXT, PrjFile());
    }
  if (lHint==2)
    {
    bMdlTreeBuilt = 0;
    bFileTreeBuilt = 0;
    bCnvTreeBuilt = 0;
    bTagRefListBuilt = 0;
    bEnvListBuilt = 0;
    switch (m_Tab.GetCurSel())
      {
      case 0: RebuildFileTree(); break;
      case 1: RebuildMdlTree(); break;
      case 2: RebuildEnvList(); break;
      case 3: RebuildCnvTree(); break;
      case 4: RebuildTagRefList(); break;
      }
    }
  else
    CFormView::OnUpdate(pSender, lHint, pHint);
  }

//---------------------------------------------------------------------------

const int MinPrjFormWidth = 420;
const int MinPrjFormHeight = 360;
const int TreeMaxWidth = 750;//460;
const int TreeEdgeMargin = 10;
const int TabEdgeMargin = 4;

void CPrjForm::OnSize(UINT nType, int cx, int cy)
  {
  CFormView::OnSize(nType, cx, cy);
  if ((nType==SIZE_RESTORED && cx<=4096 && cy<=4096))// && (cy>=MinPrjFormHeight-1 || cx>=MinPrjFormWidth-1))
    {
    CRect WndRct;
    GetWindowRect(&WndRct);
    CRect MdlRct;
    if (MdlTree.GetSafeHwnd())
      {
      MdlTree.GetWindowRect(&MdlRct);
      int cxx = Min(TreeMaxWidth, cx - int(MdlRct.left - WndRct.left) - TreeEdgeMargin);
      int cyy = cy - (MdlRct.top - WndRct.top) - TreeEdgeMargin;
      MdlTree.SetWindowPos   (this, MdlRct.left, MdlRct.top, cxx, cyy, SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);
      FileTree.SetWindowPos  (this, MdlRct.left, MdlRct.top, cxx, cyy, SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);
      FileList.SetWindowPos  (this, MdlRct.left, MdlRct.top, cxx, cyy, SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);
      CnvTree.SetWindowPos   (this, MdlRct.left, MdlRct.top, cxx, cyy, SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);
      TagRefList.SetWindowPos(this, MdlRct.left, MdlRct.top, cxx, cyy, SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);
      EnvList.SetWindowPos   (this, MdlRct.left, MdlRct.top, cxx, cyy, SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);
      const int x = MdlRct.left + cxx;
      m_Tab.GetWindowRect(&MdlRct);
      cyy = cy - (MdlRct.top - WndRct.top) - TabEdgeMargin;
      cxx = Min(x + TabEdgeMargin - (int)MdlRct.left, cx - int(MdlRct.left - WndRct.left) - TabEdgeMargin);
      m_Tab.SetWindowPos(this, MdlRct.left, MdlRct.top, cxx, cyy, SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);
//      CWnd* w = GetDlgItem(IDC_PF_TXT);
//      if (w)
//        {
//        w->GetWindowRect(&MdlRct);
////        cxx = Min(x + TabEdgeMargin - (int)MdlRct.left, cx - (MdlRct.left - WndRct.left) - TabEdgeMargin);
//        cxx =  cx - (MdlRct.left - WndRct.left) - TabEdgeMargin;
//        w->SetWindowPos(this, MdlRct.left, MdlRct.top, cxx, MdlRct.Height(), SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);
//        }
      }
    }
  }

//---------------------------------------------------------------------------

void CPrjForm::DoShowHide()
  {
  UpdateData(TRUE);
  int CurTab = m_Tab.GetCurSel();
  switch (CurTab)
    {
    case 0: if (!bFileTreeBuilt)   RebuildFileTree(); break;
    case 1: if (!bMdlTreeBuilt)    RebuildMdlTree(); break;
    case 2: if (!bEnvListBuilt)    RebuildEnvList(); break;
    case 3: if (!bCnvTreeBuilt)    RebuildCnvTree(); break;
    case 4: if (!bTagRefListBuilt) RebuildTagRefList(); break;
    }

  const int TabPages = 5;
  const int MaxCtrls = 8;

  CWnd* w[TabPages][MaxCtrls];
  byte s[TabPages][MaxCtrls];
  byte e[TabPages][MaxCtrls];

  memset(w, 0, sizeof(w));
  memset(s, 1, sizeof(s));
  memset(e, 1, sizeof(e));

  //file tab...
  w[0][0] = &FileTree;
  s[0][0] = (m_FileView!=2);
  w[0][1] = &FileList;
  s[0][1] = (m_FileView==2);
  w[0][2] = GetDlgItem(IDC_PF_BUTTON2);
  e[0][2] = 0;
  w[0][3] = GetDlgItem(IDC_PF_SYMBOLIC);
  w[0][4] = GetDlgItem(IDC_PF_GROUPBOX);
  w[0][5] = GetDlgItem(IDC_PF_FOLDERRADIO);
  w[0][6] = GetDlgItem(IDC_PF_GROUPRADIO);
  w[0][7] = GetDlgItem(IDC_PF_LISTRADIO);
  //model tab...
  w[1][0] = &MdlTree;
  w[1][1] = GetDlgItem(IDC_PF_GROUPBOX2);
  w[1][2] = GetDlgItem(IDC_PF_TYPERADIO);
  w[1][3] = GetDlgItem(IDC_PF_GRAPHICSRADIO);
  //environment variables tab...
  w[2][0] = &EnvList;
  w[2][1] = GetDlgItem(IDC_PF_SYMBOLIC);
  //conversion units tab...
  w[3][0] = &CnvTree;
  //XRef tags tab...
  w[4][0] = &TagRefList;

  //hide & disable previous controls...
  for (int i=0; i<TabPages; i++)
    if (i!=CurTab)
      for (int j=0; j<MaxCtrls; j++)
        if (w[i][j])
          {
          w[i][j]->EnableWindow(FALSE);
          w[i][j]->ShowWindow(SW_HIDE);
          }
  //show & enable controls...
  i=CurTab;
  for (int j=0; j<MaxCtrls; j++)
    if (w[i][j])
      {
      int On=(s[i][j]);
      w[i][j]->EnableWindow(On && e[i][j]);
      w[i][j]->ShowWindow(On ? SW_SHOW : SW_HIDE);
      }
  }

//---------------------------------------------------------------------------

void CPrjForm::OnFileViewFolder()
  {
  bFileTreeBuilt = 0;
  DoShowHide();
  }

void CPrjForm::OnFileViewGroup()
  {
  bFileTreeBuilt = 0;
  DoShowHide();
  }

void CPrjForm::OnFileViewList()
  {
  bFileTreeBuilt = 0;
  DoShowHide();
  }

//---------------------------------------------------------------------------

void CPrjForm::ChangeTab()
  {
  int CurTab = m_Tab.GetCurSel();
  if (CurTab!=iPrevTab)
    {
    DoShowHide();

    iPrevTab = CurTab;
    }
  }

//---------------------------------------------------------------------------

void CPrjForm::OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult)
  {
  ChangeTab();
  *pResult = 0;
  }

//---------------------------------------------------------------------------

void CPrjForm::OnItemclickPfFilelist(NMHDR* pNMHDR, LRESULT* pResult)
  {
	HD_NOTIFY *phdn = (HD_NOTIFY *) pNMHDR;
	// TODO: Add your control notification handler code here
  int i=phdn->iItem;

	*pResult = 0;
  }

//---------------------------------------------------------------------------

BOOL CPrjForm::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
  {
  HD_NOTIFY* pNM = (HD_NOTIFY*)lParam;
  switch (pNM->hdr.code)
    {
    case HDN_ITEMCLICK:
      {
  	  LPNMHEADER phdn = (LPNMHEADER) lParam;
      int i=phdn->iItem;
      if (m_Tab.GetCurSel()==4)
        {
        if (i==iTROrdCol[0])
          iTROrdDir[0]*=-1;
        else if (i==iTROrdCol[1])
          {
          iTROrdCol[1]=iTROrdCol[0];
          iTROrdDir[1]=iTROrdDir[0];
          iTROrdCol[0]=i;
          iTROrdDir[0]=1;
          }
        else if (i==iTROrdCol[2])
          {
          iTROrdCol[2]=iTROrdCol[1];
          iTROrdDir[2]=iTROrdDir[1];
          iTROrdCol[1]=iTROrdCol[0];
          iTROrdDir[1]=iTROrdDir[0];
          iTROrdCol[0]=i;
          iTROrdDir[0]=1;
          }
        else if (i==iTROrdCol[3])
          {
          iTROrdCol[3]=iTROrdCol[2];
          iTROrdDir[3]=iTROrdDir[2];
          iTROrdCol[2]=iTROrdCol[1];
          iTROrdDir[2]=iTROrdDir[1];
          iTROrdCol[1]=iTROrdCol[0];
          iTROrdDir[1]=iTROrdDir[0];
          iTROrdCol[0]=i;
          iTROrdDir[0]=1;
          }
        else if (i==iTROrdCol[4])
          {
          iTROrdCol[4]=iTROrdCol[3];
          iTROrdDir[4]=iTROrdDir[3];
          iTROrdCol[3]=iTROrdCol[2];
          iTROrdDir[3]=iTROrdDir[2];
          iTROrdCol[2]=iTROrdCol[1];
          iTROrdDir[2]=iTROrdDir[1];
          iTROrdCol[1]=iTROrdCol[0];
          iTROrdDir[1]=iTROrdDir[0];
          iTROrdCol[0]=i;
          iTROrdDir[0]=1;
          }
        SortTagRefList();
        TagRefList.RedrawItems(0, nTagRefInList-1);
        TagRefList.UpdateWindow();
        }
      else if (m_Tab.GetCurSel()==0)
        {
        if (i==iOrdCol[0])
          iOrdDir[0]*=-1;
        else if (i==iOrdCol[1])
          {
          iOrdCol[1]=iOrdCol[0];
          iOrdDir[1]=iOrdDir[0];
          iOrdCol[0]=i;
          iOrdDir[0]=1;
          }
        else if (i==iOrdCol[2])
          {
          iOrdCol[2]=iOrdCol[1];
          iOrdDir[2]=iOrdDir[1];
          iOrdCol[1]=iOrdCol[0];
          iOrdDir[1]=iOrdDir[0];
          iOrdCol[0]=i;
          iOrdDir[0]=1;
          }
        else if (i==iOrdCol[3])
          {
          iOrdCol[3]=iOrdCol[2];
          iOrdDir[3]=iOrdDir[2];
          iOrdCol[2]=iOrdCol[1];
          iOrdDir[2]=iOrdDir[1];
          iOrdCol[1]=iOrdCol[0];
          iOrdDir[1]=iOrdDir[0];
          iOrdCol[0]=i;
          iOrdDir[0]=1;
          }
        SortFileList();
        FileList.RedrawItems(0, nFilesInList-1);
        FileList.UpdateWindow();
        }
      break;
      }
    case LVN_GETDISPINFO:
      {
      NMLVDISPINFO * pnmv = (NMLVDISPINFO*)lParam;
      if (pNM->hdr.idFrom==IDC_PF_REFTAGS && pnmv->item.lParam<m_TagRefOrder.GetSize())
        {
        CPrjFormTagRefItem &TRI=*(m_TagRefOrder[pnmv->item.lParam]);
        switch (pnmv->item.iSubItem)
          {
          case 0:
            pnmv->item.pszText=TRI.Used();
            break;
          case 1:
            pnmv->item.pszText=TRI.Type();
            break;
          case 2:
            pnmv->item.pszText=TRI.Tag();
            break;
          case 3:
            pnmv->item.pszText=TRI.sGetCnt();
            break;
          case 4:
            pnmv->item.pszText=TRI.sSetCnt();
            break;
          }
        }
      else if (pNM->hdr.idFrom==IDC_PF_FILELIST && pnmv->item.lParam<m_FilesOrder.GetSize())
        {
        CPrjFormFileItem &FFI=*(m_FilesOrder[pnmv->item.lParam]);
        switch (pnmv->item.iSubItem)
          {
          case 0:
            //pnmv->item.iImage=It.iImg;
            pnmv->item.pszText=FFI.Fn();
            break;
          case 1:
            pnmv->item.pszText=FFI.Path();
            break;
          case 2:
            pnmv->item.pszText=FFI.Grp();
            break;
          case 3:
            pnmv->item.pszText=FFI.Exists();
            break;
          }
        }
      break;
      }
    default:
      {
      int xx=0;
      break;
      }
    }

	return CFormView::OnNotify(wParam, lParam, pResult);
  }

//---------------------------------------------------------------------------
const char* MissingFromDrawingsTxt = "{Missing from drawings}";
const char* MissingFromDatabaseTxt = "Missing from database";

void CPrjForm::OnMenu(UINT id)
  {
  switch (id)
    {
    case IDM_PRJ_FILEACCESS:
    case IDM_PRJ_ACCESS:
      gs_AccessWnds.AccessNode(-1, sCurrentSeln());
      break;
    case IDM_PRJ_FIND:
      gs_pPrj->FindTag(sCurrentSeln(), NULL, NULL, FTO_MoveCursor|FTO_Highlite);
      break;
    case IDM_PRJ_CHANGETAG:
      {
      CChangeTag ChgTag(sCurrentSeln(), False);
      ChgTag.DoModal();
      break;
      }
    case IDM_PRJ_DELETEALL:
      sCurrentSeln = MissingFromDrawingsTxt;
      if (MdlTree.GetParentItem(hCurrentSeln))
        hCurrentSeln = MdlTree.GetParentItem(hCurrentSeln);
    case IDM_PRJ_DELETE:
      {
      const bool MissingDrawing = (_stricmp(sCurrentSeln(), MissingFromDrawingsTxt)==0);
      char Buff[1024];
      if (MissingDrawing)
        sprintf(Buff, "Are you sure you want to delete ALL the 'missing' models from the database?");
      else
        sprintf(Buff, "Are you sure you want to delete the model '%s' from the database?", sCurrentSeln());
      if (AfxMessageBox(Buff, MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2)==IDYES)
        {
        SFEFlwLib* pFlwLib = gs_pPrj->FlwLib();
        ASSERT(pFlwLib);
        if (MissingDrawing)
          {
          long DeleteCnt = 0;
          HTREEITEM hParent = hCurrentSeln;//MdlTree.GetParentItem(hCurrentSeln);
          while (hParent && MdlTree.ItemHasChildren(hParent))
            {
            HTREEITEM h = MdlTree.GetChildItem(hParent);
            CString Tag = MdlTree.GetItemText(h);
            int RetCode = gs_Exec.DeleteTag((char*)(const char*)Tag);
            if (RetCode!=EODT_DONE)
              {
              LogError((const char*)Tag, 0, "Model not deleted");
              }
            else
              {
              DeleteCnt++;
              }
            MdlTree.DeleteItem(h);
            }
          if (hParent && !MdlTree.ItemHasChildren(hParent))
            MdlTree.DeleteItem(hParent);
          if (DeleteCnt)
            LogNote("Project", 0, "%d models deleted", DeleteCnt);
          }
        else
          {
          int RetCode = gs_Exec.DeleteTag(sCurrentSeln());
          if (RetCode!=EODT_DONE)
            {
            LogError(sCurrentSeln(), LF_Exclamation, "Model not deleted");
            }
          else
            {
            ASSERT(hCurrentSeln);
            HTREEITEM hParent = MdlTree.GetParentItem(hCurrentSeln);
            MdlTree.DeleteItem(hCurrentSeln);
            if (!MdlTree.ItemHasChildren(hParent))
              MdlTree.DeleteItem(hParent);
            }
          }
        }
      break;
      }
    case IDM_PRJ_FILEEDIT:
      {
      if (dwCurrentData==EFF_CanEditFile)
        {
        Strng NPad=TxtEditCmdString(sCurrentSeln());
        STARTUPINFO si;
        memset(&si, 0, sizeof(si));
        si.cb = sizeof(si);
        si.wShowWindow = SW_SHOWDEFAULT;
        PROCESS_INFORMATION pi;
        CreateProcess(NULL, NPad(), NULL, NULL, FALSE, 0, NULL, PrjFiles(), &si, &pi);
        }
      else
        {
        CXM_OleExcelReport* pXB = new CXM_OleExcelReport(NULL, sCurrentSeln(), "", SUB_REPORT_OPEN);
        ScdMainWnd()->PostMessage(WMU_CMD, SUB_CMD_GENERATEOLEREPORT, (LPARAM)pXB);
        }
      break;
      }
    case IDM_PRJ_LISTTOCLIPBOARD:
      {
      HTREEITEM hParent = (MdlTree.GetParentItem(hCurrentSeln) ? MdlTree.GetParentItem(hCurrentSeln) : hCurrentSeln);
      if (hParent)
        {
        HTREEITEM h = MdlTree.GetChildItem(hParent);
        CString Buffer;
        while (h)
          {
          CString S = MdlTree.GetItemText(h);
          S += "\n";
          Buffer += S;
          h = MdlTree.GetNextItem(h, TVGN_NEXT);
          }
        CopyTextToClipboard(this, (LPTSTR)(LPCTSTR)Buffer);
        }
      break;
      }
    default:
      {
      RequestModelIOConnRec ConnInfo;
      int j = id - IDM_PRJ_ACCESSFIRST;
      if (gs_pPrj->FlwLib()->RequestModelIOConn(sCurrentSeln(), j, ConnInfo))
        gs_AccessWnds.AccessNode(-1, ConnInfo.Tag());
      break;
      }
    }
  }

//---------------------------------------------------------------------------

void CPrjForm::OnRefresh()
  {
  switch (m_Tab.GetCurSel())
    {
    case 0: RebuildFileTree(); break;
    case 1: RebuildMdlTree(); break;
    case 2: RebuildEnvList(); break;
    case 3: RebuildCnvTree(); break;
    case 4: RebuildTagRefList(); break;
    }
  }

//---------------------------------------------------------------------------

void CPrjForm::OnButton2()
  {
  }

//---------------------------------------------------------------------------

void CPrjForm::OnRclickEnvList(NMHDR* pNMHDR, LRESULT* pResult)
  {
  *pResult = 0;
  }

//---------------------------------------------------------------------------

void CPrjForm::OnDblclkEnvList(NMHDR* pNMHDR, LRESULT* pResult)
  {
  int nItem = EnvList.GetNextItem(-1, LVNI_SELECTED);
  if (nItem>=0)
    {
    char Buff[1024];
    EnvList.GetItemText(nItem, 0, Buff, sizeof(Buff));
    if (_stricmp(Buff, "CfgFile")==0)
      {
      EnvList.GetItemText(nItem, 1, Buff, sizeof(Buff));
      if (strlen(Buff)>0)
        {
        DWORD ExitCode = 0;
        if (hCfgProcess)
          GetExitCodeProcess(hCfgProcess, &ExitCode);
        if (ExitCode==STILL_ACTIVE)
          {
          ActivateApp(dwCfgProcessId);
          }
        else
          {
          Strng ss;
          hCfgProcess = NULL;
          Strng NPad=TxtEditCmdString(Buff);
          STARTUPINFO si;
          memset(&si, 0, sizeof(si));
          si.cb = sizeof(si);
          si.wShowWindow = SW_SHOWDEFAULT;
          PROCESS_INFORMATION pi;
          if (CreateProcess(NULL, NPad(), NULL, NULL, FALSE, 0, NULL, PrjFiles(), &si, &pi))
            {
            hCfgProcess = pi.hProcess;
            dwCfgProcessId = pi.dwProcessId;
            }
          }
        }

      int i=0;
      }

    }
  *pResult = 0;
  }

//---------------------------------------------------------------------------

void CPrjForm::OnRclickMdlTree(NMHDR* pNMHDR, LRESULT* pResult)
  {
  SFEFlwLib* pFlwLib = gs_pPrj->FlwLib();
  ASSERT(pFlwLib && pFlwLib==gs_pTheSFELib);
  TV_HITTESTINFO HTI;
  HTI.pt = MdlTree.PrevDownPoint;
  HTREEITEM Hit = MdlTree.HitTest(&HTI);
  if (Hit)
    {
    const bool OnLine = ((HTI.flags & TVHT_ONITEMRIGHT|TVHT_TOLEFT|TVHT_TORIGHT)!=0);
    const bool OnItem = ((HTI.flags & TVHT_ONITEM/*|TVHT_ONITEMRIGHT|TVHT_ONITEMLABEL*/)!=0);
    if (OnItem || (HTI.flags & TVHT_ONITEMRIGHT) || (HTI.flags & TVHT_ONITEMICON))
      {
      HTREEITEM hParent = MdlTree.GetParentItem(HTI.hItem);
      HTREEITEM hTestItem = (hParent ? hParent : HTI.hItem);
      CString ParentTxt;
      if (hTestItem)
        ParentTxt = MdlTree.GetItemText(hTestItem);
      const bool MissingDrawing = (_stricmp((const char*)ParentTxt, MissingFromDrawingsTxt)==0);
      const bool MissingDatabase = (ParentTxt.Find(MissingFromDatabaseTxt)>0);

      if (hParent)
        {
        hCurrentSeln = HTI.hItem;
        CString Tag = MdlTree.GetItemText(HTI.hItem);

        sCurrentSeln = (const char*)Tag;
        int i=sCurrentSeln.XStrCSpn(" ");
        if (i>0)
          sCurrentSeln.SetLength(i);

        Strng s;
        s.Set("&Access %s...", sCurrentSeln());
        CMenu Menu;
        Menu.CreatePopupMenu();
        Menu.AppendMenu(MF_STRING, IDM_PRJ_ACCESS, s());
        Menu.AppendMenu(MF_STRING, IDM_PRJ_FIND, "&Find...");
        Menu.AppendMenu(MF_STRING, IDM_PRJ_CHANGETAG, "C&hange Tag...");
        if (MissingDrawing)
          {
          Menu.EnableMenuItem(IDM_PRJ_FIND, MF_BYCOMMAND|MF_GRAYED);
          Menu.AppendMenu(MF_STRING, IDM_PRJ_DELETE, "&Delete");
          }
        if (MissingDatabase)
          {
          Menu.EnableMenuItem(IDM_PRJ_ACCESS, MF_BYCOMMAND|MF_GRAYED);
          Menu.EnableMenuItem(IDM_PRJ_CHANGETAG, MF_BYCOMMAND|MF_GRAYED);
          }
        RequestModelIOConnRec ConnInfo;
        int j = 0;
        while (pFlwLib->RequestModelIOConn(sCurrentSeln(), j++, ConnInfo) && j<IDM_PRJ_ACCESSLAST-IDM_PRJ_ACCESSFIRST)
          {
          Strng Tg;
          RequestModelIOInfoRec IOInfo;
          if (pFlwLib->RequestModelIOInfoById(sCurrentSeln(), ConnInfo.iId, IOInfo))
            Tg.Set("%s : %s", IOInfo.Name(), ConnInfo.Tag());
          else
            Tg=ConnInfo.Tag();

          if (j==1)
            Menu.AppendMenu(MF_SEPARATOR);
          //s.Set("Access %s...", ConnInfo.Tag());
          s.Set("Access %s...", Tg());
          Menu.AppendMenu(MF_STRING, IDM_PRJ_ACCESSFIRST+j-1, s());
          }
        Menu.AppendMenu(MF_SEPARATOR);
        Menu.AppendMenu(MF_STRING, IDM_PRJ_LISTTOCLIPBOARD, "&Copy list to clipboard");
        if (MissingDrawing)
          Menu.AppendMenu(MF_STRING, IDM_PRJ_DELETEALL, "Delete ALL missing models");
        CRect Rect;
        MdlTree.GetWindowRect(&Rect);
        Menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, Rect.left+MdlTree.PrevDownPoint.x, Rect.top+MdlTree.PrevDownPoint.y, this);
        Menu.DestroyMenu();
        }
      else
        {
        hCurrentSeln = HTI.hItem;
        MdlTree.SelectItem(HTI.hItem);
        HTREEITEM h = MdlTree.GetChildItem(HTI.hItem);
        if (h)
          MdlTree.EnsureVisible(h);
        CMenu Menu;
        Menu.CreatePopupMenu();
        Menu.AppendMenu(MF_STRING, IDM_PRJ_LISTTOCLIPBOARD, "Copy list to clipboard");
        if (MissingDrawing)
          {
          sCurrentSeln = MissingFromDrawingsTxt;
          Menu.AppendMenu(MF_STRING, IDM_PRJ_DELETEALL, "Delete all missing models");
          }
        CRect Rect;
        MdlTree.GetWindowRect(&Rect);
        Menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, Rect.left+MdlTree.PrevDownPoint.x, Rect.top+MdlTree.PrevDownPoint.y, this);
        Menu.DestroyMenu();
        }
      }
    }
  *pResult = 0;
  }

//---------------------------------------------------------------------------

void CPrjForm::OnDblclkMdlTree(NMHDR* pNMHDR, LRESULT* pResult)
  {
  HTREEITEM h = MdlTree.GetSelectedItem();
  if (h && MdlTree.GetParentItem(h))
    {
    Strng Tag;
    Tag = (const char *)MdlTree.GetItemText(h);
    int i=Tag.XStrCSpn(" ");
    if (i>0)
      Tag.SetLength(i);
    gs_AccessWnds.AccessNode(-1, Tag());
    }
  *pResult = 0;
  }

//---------------------------------------------------------------------------

void CPrjForm::OnModelGroupRadio()
  {
  int Prev = m_ModelGroup;
  UpdateData(TRUE);
  if (m_ModelGroup!=Prev)
    RebuildMdlTree();
  }

//---------------------------------------------------------------------------

void CPrjForm::RebuildMdlTree()
  {
  bMdlTreeBuilt = 1;
  MdlTree.DeleteAllItems();
  SFEFlwLib* pFlwLib = gs_pPrj->FlwLib();
  if (pFlwLib)
    {
    CWaitMsgCursor WaitMsg("Building object tree");
    UpdateData(TRUE);
    TagTrees.Rebuild(RQ_TagsConns);
    bool GroupByGrf = (m_ModelGroup==1);
    BOOL m_ShowConnected = FALSE;
    TV_INSERTSTRUCT IS;
    IS.hInsertAfter = TVI_SORT;
    IS.item.mask = TVIF_TEXT;
    IS.item.state = 0;
    IS.item.stateMask = 0;
    IS.item.iImage = 0;
    IS.item.iSelectedImage = 0;
    IS.item.cChildren = 0;
    IS.item.lParam = 0;
    if (!GroupByGrf)
      {//group by model type
      RequestModelInfoRec MInfo;
      CModelTypeListArray& List = TagTrees.GetList();
      for (int j=0; j<List.GetSize(); j++)
        {
        CModelTypeList* pTagList = List[j];
        IS.hParent = TVI_ROOT;
        IS.item.pszText = pTagList->ModelName();
        HTREEITEM hRoot = (bShowAllClasses ? MdlTree.InsertItem(&IS) : NULL);
        for (int k=0; k<pTagList->GetSize(); k++)
          {
          if (hRoot==NULL)
            hRoot = MdlTree.InsertItem(&IS);
          IS.hParent = hRoot;
          IS.item.pszText = pTagList->GetTagAt(k);
          HTREEITEM h = MdlTree.InsertItem(&IS);
          if (m_ShowConnected)
            {
            RequestModelIOConnRec ConnInfo;
            int jj = 0;
            while (pFlwLib->RequestModelIOConn(pTagList->GetTagAt(k), jj++, ConnInfo))
              {
              IS.hParent = h;
              IS.item.pszText = ConnInfo.Tag();
              HTREEITEM hh = MdlTree.InsertItem(&IS);
              }
            }
          }
        }
      }
    if (gs_pPrj->AllGrfLoaded())
      {
      flag Missing = 0;
      ActiveTagInfoMap TagMap;
      ActiveTagInfo* pATI = new ActiveTagInfo;
      pATI->GetTags();
      pATI->RebuildTagMap(TagMap);
      pATI->SetActive(False); // SetIndex to Zero
      for (int iTmpl=iGraphTemplate; iTmpl<=iVisioTemplate; iTmpl++)
        if (ScdApp()->TemplateExists(iTmpl))
          {
          POSITION Pos = ScdApp()->Template(iTmpl).GetFirstDocPosition();
          while (Pos)
            {
            HTREEITEM hMissingRoot = NULL;
            HTREEITEM hRoot;
            CGrfDoc* pGDoc = (CGrfDoc*)(ScdApp()->Template(iTmpl).GetNextDoc(Pos));
            Strng GrfTitle((const char*)(pGDoc->GetTitle()));
            if (GroupByGrf)
              {
              IS.hParent = TVI_ROOT;
              IS.item.pszText = GrfTitle();
              hRoot = MdlTree.InsertItem(&IS);
              }
            ActiveTagInfo* pAT = new ActiveTagInfo;
            pAT->NTags = pGDoc->GetTagList(pAT->TagList);
            for (pStrng pT=pAT->TagList.First(); pT; pT=pT->Next())
              {
              if (GroupByGrf)
                {
                IS.hParent = hRoot;
                IS.item.pszText = pT->Str();
                MdlTree.InsertItem(&IS);
                }
              pStrng p0;
              if (TagMap.Lookup(pT->Str(), p0))
                p0->SetIndex(1); // Found
              else
                {
                Missing = 1;
                if (pAT->NMissing==0)
                  {
                  Strng s;
                  s.Set("{%s: %s}", GrfTitle(), MissingFromDatabaseTxt);
                  IS.hParent = TVI_ROOT;
                  IS.item.pszText = s();
                  hMissingRoot = MdlTree.InsertItem(&IS);
                  }
                IS.hParent = hMissingRoot;
                IS.item.pszText = pT->Str();
                MdlTree.InsertItem(&IS);
                pAT->NMissing++;
                }
              }
            delete pAT;
            }
          }
      pATI->NMissing = 0;
      pStrng pT = pATI->TagList.First(); // Skip the first one - FlwSolve
      for (pT=pT->Next(); pT; pT=pT->Next())
        if (pT->Index()==0)
          {
          if (stricmp(pT->Str(), PlantAreaTag)==0)
            {
            //special model, do not report
            }
          else
            {
            Missing = 1;
            if (pATI->NMissing==0)
              {
              IS.hParent = TVI_ROOT;
              IS.item.pszText = (char*)MissingFromDrawingsTxt;
              IS.hParent = MdlTree.InsertItem(&IS);
              }
            IS.item.pszText = pT->Str();
            MdlTree.InsertItem(&IS);
            pATI->NMissing++;
            }
          }
      delete pATI;
      if (Missing && !bMissingNotified)
        {
        LogError("Project", LF_Exclamation, "Models are missing from drawings or database, see project window.");
        bMissingNotified = 1;
        }
      }
    }
  }

//---------------------------------------------------------------------------

void CPrjForm::OnRclickFileTree(NMHDR* pNMHDR, LRESULT* pResult)
  {
  SFEFlwLib* pFlwLib = gs_pPrj->FlwLib();
  ASSERT(pFlwLib);
  TV_HITTESTINFO HTI;
  HTI.pt = FileTree.PrevDownPoint;
  if (FileTree.HitTest(&HTI) && ((HTI.flags & TVHT_ONITEM) || (HTI.flags & TVHT_ONITEMRIGHT) || (HTI.flags & TVHT_ONITEMICON)))
    {
    DWORD W = FileTree.GetItemData(HTI.hItem);
    if (W)
      {
      CMenu Menu;
      Menu.CreatePopupMenu();
      hCurrentSeln = HTI.hItem;
      CString Txt = FileTree.GetItemText(HTI.hItem);
      sCurrentSeln = (const char*)Txt;
      dwCurrentData = W;
      Strng s;
      if (W==EFF_CanEditFile || W==EFF_ExcelValid)
        {
        Strng fn;
        if (m_FileView==0)
          MakeFilename(FileTree.GetItemText(FileTree.GetParentItem(HTI.hItem)), Txt, fn, sCurrentSeln);
        else
          MakeFilename(Txt, Txt, fn, sCurrentSeln);
        s.Set("&Edit %s...", fn);
        Menu.AppendMenu(MF_STRING, IDM_PRJ_FILEEDIT, s());
        }
      else if (W==EFF_OtherValid)
        {
        s.Set("&Access %s...", (const char*)Txt);
        Menu.AppendMenu(MF_STRING, IDM_PRJ_FILEACCESS, s());
        Menu.AppendMenu(MF_STRING, IDM_PRJ_FIND, "&Find...");
        }
      CRect Rect;
      FileTree.GetWindowRect(&Rect);
      Menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, Rect.left+FileTree.PrevDownPoint.x, Rect.top+FileTree.PrevDownPoint.y, this);
      Menu.DestroyMenu();
      }
    else
      {
      FileTree.SelectItem(HTI.hItem);
      HTREEITEM h = FileTree.GetChildItem(HTI.hItem);
      if (h)
        FileTree.EnsureVisible(h);
      }
    }
  *pResult = 0;
  }

//---------------------------------------------------------------------------

void CPrjForm::OnDblclkFileTree(NMHDR* pNMHDR, LRESULT* pResult)
  {
  HTREEITEM h = FileTree.GetSelectedItem();
  if (h)
    {
    const DWORD W = FileTree.GetItemData(h);
    switch (W)
      {
      case 0: break;
      case EFF_CanEditFile://edit file
      case EFF_ExcelValid:
        {
        Strng fn,ffn;
        if (m_FileView==0)
          MakeFilename(FileTree.GetItemText(FileTree.GetParentItem(h)), FileTree.GetItemText(h), fn, ffn);
        else
          MakeFilename(FileTree.GetItemText(h), FileTree.GetItemText(h), fn, ffn);
        if (W==EFF_CanEditFile)
          {
          Strng NPad=TxtEditCmdString(ffn());
          STARTUPINFO si;
          memset(&si, 0, sizeof(si));
          si.cb = sizeof(si);
          si.wShowWindow = SW_SHOWDEFAULT;
          PROCESS_INFORMATION pi;
          CreateProcess(NULL, NPad(), NULL, NULL, FALSE, 0, NULL, PrjFiles(), &si, &pi);
          }
        else
          {
          CXM_OleExcelReport* pXB = new CXM_OleExcelReport(NULL, ffn(), "", SUB_REPORT_OPEN);
          ScdMainWnd()->PostMessage(WMU_CMD, SUB_CMD_GENERATEOLEREPORT, (LPARAM)pXB);
          }
        break;
        }
      case EFF_OtherValid://access object
        {
        CString Tag = FileTree.GetItemText(h);
        gs_AccessWnds.AccessNode(-1, (char*)(const char*)Tag);
        break;
        }
      }
    }
  *pResult = 0;
  }

//---------------------------------------------------------------------------

void CPrjForm::RebuildFileTree()
  {
  UpdateData(TRUE);
  bFileTreeBuilt = 0;

  SFEFlwLib* pFlwLib = gs_pPrj->FlwLib();
  if (pFlwLib)
    {
    CWaitMsgCursor WaitMsg("Building file list tree");
    UpdateData(TRUE);
    Strng s;
    Strng Fn,Path,XPath;

    CPrjFilesLists PFL;
    PFL.BuildLists();
    PFL.CheckExists();
    PFL.CheckUnknown();

    FileTree.DeleteAllItems();
    FileList.DeleteAllItems();

    m_FilesInList.SetSize(0, 16);
    nFilesInList=0;

    TV_INSERTSTRUCT IS;
    IS.hInsertAfter = TVI_SORT;
    IS.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
    IS.item.state = 0;
    IS.item.stateMask = 0;
    IS.item.iImage = 0;
    IS.item.iSelectedImage = 0;
    IS.item.cChildren = 0;
    HTREEITEM hExec = NULL;
    HTREEITEM hNotExists = NULL;

    int W0=0, W1=0, W2=0, W3=0;
    if (m_FileView==0)
      {
      for (int i=0; i<PFL.GetSize(); i++)
        {
        for (int j=0; j<PFL.pExecFiles[i]->Files.GetSize(); j++)
          {
          CExecObjFileInfo &FI = PFL.pExecFiles[i]->Files[j];
          Fn.FnNameExt(FI.sFilename());
          //Strng s1,s2;
          Path.FnDrivePath(FI.sFilename());
          XPath = Path();
          if (m_Symbolic)
            XPath.FnContract();
          else
            XPath.FnExpand();

          if (!FI.bExists && hNotExists==NULL)
            {
            IS.hParent = TVI_ROOT;
            IS.item.iImage = 1;
            IS.item.iSelectedImage = IS.item.iImage;
            IS.item.pszText = "Not Found";
            hNotExists = FileTree.InsertItem(&IS);
            }

          HTREEITEM hPar = FileTree.FindItem(XPath(), FI.bExists ? 0 : 1);
          if (hPar==NULL)
            {
            IS.hParent = (FI.bExists ? TVI_ROOT : hNotExists);
            IS.item.iImage = 1;
            IS.item.iSelectedImage = IS.item.iImage;
            IS.item.pszText = XPath();
            hPar = FileTree.InsertItem(&IS);
            }
          flag InsertIt = 1;
          HTREEITEM h = NULL;
          if (FI.dwFlags & EFF_OtherValid)
            {
            h = FileTree.FindChildItem(hPar, Fn());
            if (h)
              InsertIt = 0;
            }
          if (m_FilesInList.Find(FI.sFilename())>=0)
            InsertIt=0;
          if (InsertIt)
            {
            IS.hParent = hPar;
            Strng Grp=" ";
            if (FI.dwFlags & EFF_GrpValid)
              {
              if (FI.dwFlags & EFF_Grp_Models)
                Grp = "Models";
              else if (FI.dwFlags & EFF_Grp_Referenced)
                Grp = "Referenced";
              else if (FI.dwFlags & EFF_Grp_SpecSheets)
                Grp = "Spec sheets";
              else if (FI.dwFlags & EFF_Grp_Configuration)
                Grp = "Configuration";
              else if (FI.dwFlags & EFF_Grp_Reports)
                Grp = "Reports";
              else if (FI.dwFlags & EFF_Grp_CmdScripts)
                Grp = "Command Scripts";
              /*h = FileTree.FindItem(Grp(), 1);
              if (h==NULL)
                {
                IS.item.iImage = 1;
                IS.item.iSelectedImage = IS.item.iImage;
                IS.item.lParam = 0;
                IS.item.pszText = Grp();
                h = FileTree.InsertItem(&IS);
                }
              IS.hParent = h;*/
              }
            //insert filename...
            IS.item.iImage = ( (FI.dwFlags & EFF_ExcelValid) ? 6 :
                             ( (FI.dwFlags & EFF_CanEditFile) ? 3 :
                             ( (FI.dwFlags & EFF_OtherValid) ? 1 : 0 )));
            IS.item.iSelectedImage = IS.item.iImage;
            IS.item.lParam = ( (FI.dwFlags & EFF_ExcelValid) ? EFF_ExcelValid :
                             ( (FI.dwFlags & EFF_CanEditFile) ? EFF_CanEditFile : 0 ));
            IS.item.pszText = Fn();
            h = FileTree.InsertItem(&IS);

            CPrjFormFileItem FFI;
            FFI.Grp=Grp();
            FFI.Fn=Fn();
            FFI.Path=Path();
            FFI.Exists=FI.bExists?"":"No";
            m_FilesInList.SetAtGrow(nFilesInList++, FFI);
            }
          if (FI.dwFlags & EFF_OtherValid)
            {
            IS.item.iImage = 4;
            IS.item.iSelectedImage = IS.item.iImage;
            IS.item.lParam = EFF_OtherValid; //can access model
            IS.hParent = h;
            IS.item.pszText = FI.sOther();
            FileTree.InsertItem(&IS);
            }
          }
        }
      for (i=0; i<6; i++)
        {
        for (int j=0; j<PFL.OtherFiles[i].GetSize(); j++)
          {
          s = PFL.OtherFiles[i].GetAt(j);
          //s.FnExpand();
          Fn.FnNameExt(s());
          Path.FnDrivePath(s());
          XPath = Path();
          if (m_Symbolic)
            XPath.FnContract();
          else
            XPath.FnExpand();

          s = Fn();
          HTREEITEM hPar = FileTree.FindItem(XPath(), 0);
          if (hPar==NULL)
            {
            IS.hParent = TVI_ROOT;
            IS.item.iImage = 1;
            IS.item.iSelectedImage = IS.item.iImage;
            IS.item.pszText = XPath();
            hPar = FileTree.InsertItem(&IS);
            }
          IS.hParent = hPar;
          IS.item.iImage = 0;
          IS.item.iSelectedImage = IS.item.iImage;
          IS.item.pszText = s();
          FileTree.InsertItem(&IS);

          if (m_FilesInList.Find(s())<0)
            {
            Strng Grp="Other";
            CPrjFormFileItem FFI;
            FFI.Grp=Grp();
            FFI.Fn=Fn();
            FFI.Path=Path();
            FFI.Exists="";
            m_FilesInList.SetAtGrow(nFilesInList++, FFI);
            }
          }
        }
      }
    else
      {
      for (int i=0; i<PFL.GetSize(); i++)
        {
        IS.hParent = TVI_ROOT;
        IS.item.iImage = 1;
        IS.item.iSelectedImage = IS.item.iImage;
        IS.item.lParam = 0;
        IS.item.pszText = PFL.pExecFiles[i]->sExecName();
        hExec = FileTree.InsertItem(&IS);
        for (int j=0; j<PFL.pExecFiles[i]->Files.GetSize(); j++)
          {
          CExecObjFileInfo &FI = PFL.pExecFiles[i]->Files[j];
          Fn.FnNameExt(FI.sFilename());
          Path.FnExpand(FI.sFilename());
          Path.FnDrivePath();

          Strng s=Fn;
          if (Path() && _stricmp(Path(), PrjFiles())!=0)
            s.Set("%s    [ %s ]", Fn(), Path());

          flag InsertIt = 1;
          HTREEITEM h = NULL;
          if (FI.dwFlags & EFF_OtherValid)
            {
            h = FileTree.FindItem(s(), 2);
            if (h)
              InsertIt = 0;
            }
          if (m_FilesInList.Find(FI.sFilename())>=0)
            InsertIt=0;
          if (InsertIt)
            {
            if (!FI.bExists && hNotExists==NULL)
              {
              IS.hParent = TVI_ROOT;
              IS.item.iImage = 1;
              IS.item.iSelectedImage = IS.item.iImage;
              IS.item.pszText = "Not Found";
              hNotExists = FileTree.InsertItem(&IS);
              }
            IS.hParent = hExec;
            Strng Grp=" ";
            if (FI.dwFlags & EFF_GrpValid)
              {
              if (FI.dwFlags & EFF_Grp_Models)
                Grp = "Models";
              else if (FI.dwFlags & EFF_Grp_Referenced)
                Grp = "Referenced";
              else if (FI.dwFlags & EFF_Grp_SpecSheets)
                Grp = "Spec sheets";
              else if (FI.dwFlags & EFF_Grp_Configuration)
                Grp = "Configuration";
              else if (FI.dwFlags & EFF_Grp_Reports)
                Grp = "Reports";
              else if (FI.dwFlags & EFF_Grp_CmdScripts)
                Grp = "Command Scripts";
              if (FI.bExists)
                {
                h = FileTree.FindItem(Grp(), 1);
                if (h==NULL)
                  {
                  IS.item.iImage = 1;
                  IS.item.iSelectedImage = IS.item.iImage;
                  IS.item.lParam = 0;
                  IS.item.pszText = Grp();
                  h = FileTree.InsertItem(&IS);
                  }
                IS.hParent = h;
                }
              }
            //insert filename...
            if (!FI.bExists)
              IS.hParent = hNotExists;
            IS.item.iImage = ( (FI.dwFlags & EFF_ExcelValid) ? 6 :
                             ( (FI.dwFlags & EFF_CanEditFile) ? 3 :
                             ( (FI.dwFlags & EFF_OtherValid) ? 1 : 0 )));
            IS.item.iSelectedImage = IS.item.iImage;
            IS.item.lParam = ( (FI.dwFlags & EFF_ExcelValid) ? EFF_ExcelValid :
                             ( (FI.dwFlags & EFF_CanEditFile) ? EFF_CanEditFile : 0 ));
            IS.item.pszText = s();
            h = FileTree.InsertItem(&IS);

            if (m_FilesInList.Find(FI.sFilename())<0)
              {
              CPrjFormFileItem FFI;
              FFI.Grp=Grp();
              FFI.Fn=Fn();
              FFI.Path=Path();
              FFI.Exists=FI.bExists?"":"No";
              m_FilesInList.SetAtGrow(nFilesInList++, FFI);
              }
            }
          if (FI.dwFlags & EFF_OtherValid)
            {
            IS.item.iImage = 4;
            IS.item.iSelectedImage = IS.item.iImage;
            IS.item.lParam = EFF_OtherValid; //can access model
            IS.hParent = h;
            IS.item.pszText = FI.sOther();
            FileTree.InsertItem(&IS);
            }
          }
        }
      IS.hParent = TVI_ROOT;
      for (i=0; i<6; i++)
        {
        for (int j=0; j<PFL.OtherFiles[i].GetSize(); j++)
          {
          s = PFL.OtherFiles[i].GetAt(j);
          //s.FnExpand();
          Fn.FnNameExt(s());
          Path.FnDrivePath(s());

          s = Fn();
          if (Path() && _stricmp(Path(), PrjFiles())!=0)
            s.Set("%s    [ %s ]", Fn(), Path());
          if (IS.hParent==TVI_ROOT)
            {
            IS.item.iImage = 1;
            IS.item.iSelectedImage = IS.item.iImage;
            IS.item.pszText = "Other";
            IS.hParent = FileTree.InsertItem(&IS);
            }
          IS.item.iImage = 0;
          IS.item.iSelectedImage = IS.item.iImage;
          IS.item.pszText = s();
          FileTree.InsertItem(&IS);

          if (m_FilesInList.Find(s())<0)
            {
            Strng Grp="Other";
            CPrjFormFileItem FFI;
            FFI.Grp=Grp();
            FFI.Fn=Fn();
            FFI.Path=Path();
            FFI.Exists="";
            m_FilesInList.SetAtGrow(nFilesInList++, FFI);
            }
          }
        }
      }

    const int UnknownIndex = 6;
    if (PFL.OtherFiles[UnknownIndex].GetSize()>0)
      {
      IS.hParent = TVI_ROOT;
      IS.item.iImage = 1;
      IS.item.iSelectedImage = IS.item.iImage;
      IS.item.lParam = 0;
      IS.item.pszText = "Unknown";
      IS.hParent = FileTree.InsertItem(&IS);
      for (int j=0; j<PFL.OtherFiles[UnknownIndex].GetSize(); j++)
        {
        IS.item.iImage = 0;
        IS.item.iSelectedImage = IS.item.iImage;
        s = PFL.OtherFiles[UnknownIndex].GetAt(j);
        //s.FnExpand();
        Fn.FnNameExt(s());
        Path.FnDrivePath(s());

        s = Fn();
        if (m_FileView!=0 && Path() && _stricmp(Path(), PrjFiles())!=0)
          s.Set("%s    [ %s ]", Fn(), Path());

        IS.item.pszText = s();
        FileTree.InsertItem(&IS);

        if (m_FilesInList.Find(s())<0)
          {
          Strng Grp="Unknown";
          CPrjFormFileItem FFI;
          FFI.Grp=Grp();
          FFI.Fn=Fn();
          FFI.Path=Path();
          FFI.Exists="";
          m_FilesInList.SetAtGrow(nFilesInList++, FFI);
          }
        }
      }

    m_FilesOrder.SetSize(nFilesInList);
    for (int i=0; i<nFilesInList; i++)
      {
      m_FilesOrder[i]=&m_FilesInList[i];

      FileList.InsertItem(i, LPSTR_TEXTCALLBACK);//_IMAGECALLBACK);
      FileList.SetItemText(i, 1, LPSTR_TEXTCALLBACK );
      FileList.SetItemText(i, 2, LPSTR_TEXTCALLBACK );
      FileList.SetItemText(i, 3, LPSTR_TEXTCALLBACK );
      FileList.SetItemData(i, (DWORD)i);
      W0=Max(W0, FileList.GetStringWidth(m_FilesInList[i].Fn()));
      W1=Max(W1, FileList.GetStringWidth(m_FilesInList[i].Path()));
      W2=Max(W2, FileList.GetStringWidth(m_FilesInList[i].Grp()));
      W3=Max(W3, FileList.GetStringWidth(m_FilesInList[i].Exists()));
      }

    FileList.SetColumnWidth(0, Max(W0+30, FileList.GetColumnWidth(0)));
    FileList.SetColumnWidth(1, Max(W1+20, FileList.GetColumnWidth(1)));
    FileList.SetColumnWidth(2, Max(W2+20, FileList.GetColumnWidth(2)));
    FileList.SetColumnWidth(3, Max(W3+20, FileList.GetColumnWidth(3)));
    ChkSymbolic();
    SortFileList();
    }
  bFileTreeBuilt = 1;
  }

//---------------------------------------------------------------------------

static int SpPFTest(void * p, void * q)
  {
  CPrjFormFileItem * r1=(CPrjFormFileItem *)p;
  CPrjFormFileItem * r2=(CPrjFormFileItem *)q;
  int Cmp=0;
  for (int i=0; i<4 && Cmp==0; i++)
    switch (CPrjForm::iOrdCol[i])
      {
      case 0: Cmp=_stricmp(r1->Fn(), r2->Fn())*CPrjForm::iOrdDir[i]; break;
      case 1: Cmp=_stricmp(r1->Path() ? r1->Path() : "", r2->Path() ? r2->Path() : "")*CPrjForm::iOrdDir[i]; break;
      case 2: Cmp=_stricmp(r1->Grp(), r2->Grp())*CPrjForm::iOrdDir[i]; break;
      case 3: Cmp=_stricmp(r1->Exists() ? r1->Exists() : "", r2->Exists() ? r2->Exists() : "")*CPrjForm::iOrdDir[i]; break;
      }
  return Cmp<0;
  };

void CPrjForm::SortFileList()
  {
  HpSort(nFilesInList, (void**)&m_FilesOrder[0], SpPFTest);
  };

//---------------------------------------------------------------------------

static void AdjustText(CCustomTreeCtrl & FileTree, HTREEITEM H, flag m_Symbolic)
  {
  Strng S,F,P;
  H=FileTree.GetChildItem(H);
  while (H)
    {
    AdjustText(FileTree, H, m_Symbolic);

    S=(LPCTSTR)FileTree.GetItemText(H);
    char * p=S.XStrChr('[');
    char * q=S.XStrChr(']');
    if (p && q)
      {
      *p='\0';
      *q='\0';
      F=S;
      P=(p+1);
      F.LRTrim();
      P.LRTrim();
      if (m_Symbolic)
        P.FnContract();
      else
        P.FnExpand();
      S.Set("%s  [ %s ]", F(), P());
      FileTree.SetItemText(H,S());
      }

    H=FileTree.GetNextSiblingItem(H);
    }
  }

//---------------------------------------------------------------------------

void CPrjForm::ChkSymbolic()
  {
  UpdateData(TRUE);
	SetSymbolicPaths(m_Symbolic);

  if (m_FileView==0)
    {
    Strng S;
    HTREEITEM H=FileTree.GetRootItem();
    while (H!=NULL)
      {
      S=(LPCTSTR)FileTree.GetItemText(H);
      if (m_Symbolic)
        S.FnContract();
      else
        S.FnExpand();
      FileTree.SetItemText(H,S());

      H=FileTree.GetNextSiblingItem(H);
      }
    }
  else
    {
    HTREEITEM H=FileTree.GetRootItem();
    while (H!=NULL)
      {
      AdjustText(FileTree, H, m_Symbolic);

      H=FileTree.GetNextSiblingItem(H);
      }
    }

  for (int i=0; i<nFilesInList; i++)
    {
    CPrjFormFileItem &I=m_FilesInList[i];
    Strng s=I.Path+I.Fn;
    if (m_Symbolic)
      s.FnContract();
    else
      s.FnExpand();
    I.Path.FnDrivePath(s());
    }

  Strng S;
  int n=EnvList.GetItemCount();
  for (i=0; i<n; i++)
    {
    S=(LPCTSTR)EnvList.GetItemText(i,1);
    if (m_Symbolic)
      S.FnContract();
    else
      S.FnExpand();
    EnvList.SetItemText(i,1,S());
    }
  }

//---------------------------------------------------------------------------

void CPrjForm::OnSymbolic()
  {
	ChkSymbolic();
  FileList.RedrawItems(0, nFilesInList-1);
  FileList.UpdateWindow();
  }

//---------------------------------------------------------------------------

void CPrjForm::MakeFilename(CString PathTxt, CString FileTxt, Strng& FilenameOnly, Strng& FullFilename)
  {
  if (FileTxt.Find("  [ ")>=0)
    {
    FilenameOnly = FileTxt.Left(FileTxt.Find("  [ "));
    FilenameOnly.Trim();
    }
  else
    {
    FilenameOnly = (const char*)FileTxt;
    }
  Strng path;
  if (PathTxt.Find("  [ ")>=0)
    {
    path = PathTxt.Mid(PathTxt.Find("  [ "), 512);
    path.Trim(" []");
    }
  else
    {
    if (PathTxt.GetLength()>0 && PathTxt!=FileTxt)
      path = PathTxt;
    else
      path = PrjFiles();
    }
  FullFilename = path();
  FullFilename += FilenameOnly;
  FullFilename.FnExpand();
  }

//---------------------------------------------------------------------------

static int TagRefTest(void * p, void * q)
  {
  CPrjFormTagRefItem * r1=(CPrjFormTagRefItem *)p;
  CPrjFormTagRefItem * r2=(CPrjFormTagRefItem *)q;
  int Cmp=0;
  for (int i=0; i<5 && Cmp==0; i++)
    switch (CPrjForm::iTROrdCol[i])
      {
      case 0: Cmp=_stricmp(r1->Used(), r2->Used())*CPrjForm::iTROrdDir[i]; break;
      case 1: Cmp=_stricmp(r1->Type(), r2->Type())*CPrjForm::iTROrdDir[i]; break;
      case 2: Cmp=_stricmp(r1->Tag() , r2->Tag() )*CPrjForm::iTROrdDir[i]; break;
      case 3: Cmp=(r1->iGetCnt-r2->iGetCnt)*CPrjForm::iTROrdDir[i]; break;
      case 4: Cmp=(r1->iSetCnt-r2->iSetCnt)*CPrjForm::iTROrdDir[i]; break;
      }
  return Cmp<0;
  };

void CPrjForm::SortTagRefList()
  {
  if (nTagRefInList>1)
    HpSort(nTagRefInList, (void**)&m_TagRefOrder[0], TagRefTest);
  };

//---------------------------------------------------------------------------

void CPrjForm::RebuildCnvTree()
  {
  bCnvTreeBuilt = 1;
  CnvTree.DeleteAllItems();
  TV_INSERTSTRUCT IS;
  IS.hInsertAfter = TVI_SORT;
  IS.item.mask = TVIF_TEXT;// | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
  IS.item.state = 0;
  IS.item.stateMask = 0;
  IS.item.iImage = 0;
  IS.item.iSelectedImage = 0;
  IS.item.cChildren = 0;
  IS.item.lParam = 0;
  Strng s;
  for (int i=1; i<Cnvs.GetSize(); i++)
    {
    CDataCnv* p = Cnvs[i];
    if (p)
      {
      IS.hParent = TVI_ROOT;
      if (p->Xform())
        s.Set("%s    [ %s ] %s  (%s)", p->Desc(), p->Txt(), p->Fam(), p->XformName());
      else
        s.Set("%s    [ %s ] %s", p->Desc(), p->Txt(), p->Fam());
      IS.item.pszText = s();
      HTREEITEM h = CnvTree.InsertItem(&IS);
      IS.hParent = h;
      }
    while (p)
      {
      if (p==Cnvs[i])
        s.Set("%s      SI Unit", p->Txt());
      else
        {
        // CNM Inverted Scale & Reversed Sign of offset
        s.Set("%s      Scale:%.8g", p->Txt(), 1.0/p->SclValue());
        if (p->OffValue()!=0.0)
          {
          if (p->Xform()==2)// GaugeP
            s.Append("  Offset:AtmosP");
          else
            s.Append("  Offset:%.8g", -p->OffValue());
          }
        }
      IS.item.pszText = s();
      CnvTree.InsertItem(&IS);
      p = p->Next();
      }
    }
  //gs_CnvsMngr
  }

//---------------------------------------------------------------------------

void CPrjForm::RebuildEnvList()
  {
  bEnvListBuilt = 1;
  EnvList.DeleteAllItems();
  UpdateData(TRUE);
  Strng s,ss;

  struct sElem { char * pName; char * pPath; int iIcon; };

  sElem AA[]=
    {
      { "BaseCfgFiles"      ,  BaseCfgFiles()      , 1 },
      //{ "CfgName"           ,  CfgName()           , 5 },
      { "CfgFile"           ,  CfgFile()           , 0 },
      { "CfgFiles"          ,  CfgFiles()          , 1 },
      { "PrjFile"           ,  PrjFile()           , 0 },
      { "PrjFiles"          ,  PrjFiles()          , 1 },
      { "ProgFiles"         ,  ProgFiles()         , 1 },
      { "TemporaryFiles"    ,  TemporaryFiles()    , 1 },
      { "WorkFolder"        ,  StartupDirectory()  , 1 },
      { "TextEditor"        ,  TxtEditor()         , 9 },
      { NULL},
    };

  int W0=0, W1=0;
  for (int i=0; AA[i].pName; i++)
    {
    char *pName=AA[i].pName;
    char *pValue=AA[i].pPath;

//    iImg=Msg.dwFlags & LogFlag_FromPGM ? 5 : 2;
    int n=EnvList.InsertItem(EnvList.GetItemCount(), pName, AA[i].iIcon);
    EnvList.SetItemText(n, 1, pValue);
    //EnvList.SetItemData(n, Msg.dwSeqNo);
    EnvList.EnsureVisible(n, False);
    //EnvList.SetItemState(n, MustSelect ? -1 : 0 , LVIS_SELECTED);
    W0=Max(W0, EnvList.GetStringWidth(pName));
    W1=Max(W1, EnvList.GetStringWidth(pValue));
    }
  EnvList.SetColumnWidth(0, Max(W0+30, EnvList.GetColumnWidth(0)));
  EnvList.SetColumnWidth(1, Max(W1+20, EnvList.GetColumnWidth(1)));
  }

//---------------------------------------------------------------------------

int CPrjForm::TagRefHitTest()
  {
  LVHITTESTINFO HTI;
  HTI.pt = TagRefList.PrevDownPoint;
  HTI.pt.x = 8;
  int item = TagRefList.HitTest(&HTI);
  if (item>=0 && ((HTI.flags & LVHT_ONITEMLABEL) || (HTI.flags & LVHT_ONITEMICON)))
    {
    TagRefList.EnsureVisible(item, false);
    TagRefList.SetItemState(item, -1, LVIS_SELECTED);
    CString Tag;
    if (TagRefList.PrevDownPoint.x<TagRefList.GetColumnWidth(0)+TagRefList.GetColumnWidth(1))
      {
      Tag = TagRefList.GetItemText(item, 0);
      sCurrentSeln = (const char*)Tag;
      }
    else
      {
      Tag = TagRefList.GetItemText(item, 2);
      sCurrentSeln = (const char*)Tag;
      }
    int i=sCurrentSeln.XStrCSpn(".");
    if (i>0)
      sCurrentSeln.SetLength(i);
    i=sCurrentSeln.XStrCSpn(" ");
    if (i>0)
      sCurrentSeln.SetLength(i);
    }
  return item;
  }

//---------------------------------------------------------------------------

void CPrjForm::OnRclickRefTagList(NMHDR* pNMHDR, LRESULT* pResult)
  {
  int item = TagRefHitTest();
  if (item>=0)
    {
    Strng s;
    s.Set("&Access %s...", sCurrentSeln());
    CMenu Menu;
    Menu.CreatePopupMenu();
    Menu.AppendMenu(MF_STRING, IDM_PRJ_ACCESS, s());
    Menu.AppendMenu(MF_STRING, IDM_PRJ_FIND, "&Find...");
    //Menu.AppendMenu(MF_SEPARATOR);
    CRect Rect;
    TagRefList.GetWindowRect(&Rect);
    Menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, Rect.left+TagRefList.PrevDownPoint.x, Rect.top+TagRefList.PrevDownPoint.y, this);
    Menu.DestroyMenu();
    }
  *pResult = 0;
  }

//---------------------------------------------------------------------------

void CPrjForm::OnDblclkRefTagList(NMHDR* pNMHDR, LRESULT* pResult)
  {
  int item = TagRefHitTest();
  if (item>=0)
    gs_AccessWnds.AccessNode(-1, sCurrentSeln());
  *pResult = 0;
  }

//---------------------------------------------------------------------------

void CPrjForm::RebuildTagRefList()
  {
  LockWindowUpdate();
  bTagRefListBuilt = 0;
  TagRefList.DeleteAllItems();
  m_TagRefList.SetSize(0, 16);
  m_TagRefOrder.SetSize(0);
  nTagRefInList=0;
  SFEFlwLib* pFlwLib = gs_pPrj->FlwLib();
  if (pFlwLib)
    {
    CWaitMsgCursor WaitMsg("Building tag references");

    //UpdateData(TRUE);

    //gs_pAppRunMngr->RefsHelper.ReBuild(true);
    //CXRefStatusArray& Refs = gs_pAppRunMngr->GetRefs();
    CXRefInfoArray Refs;
    gs_pTheSFELib->RequestTagRefInfo(NULL, Refs);
    //->XRefStatus();
    #ifndef  _RELEASE
    dbgpln("XRefs         Gets  Sets ");
    for (long iii=0; iii<Refs.GetSize(); iii++)
      {
      CTagRefInfoItem &Ri=Refs[iii];
      dbglock();
      static LPCTSTR XX[]={"Near","Far ","Lcl "," ?? "};
      dbgp("%s %s %s  %4d  %4d",
        Ri.m_fIsGetRef ? "Get":"   ",
        Ri.m_fIsSetRef ? "Set":"   ",
        Ri.m_fIsIO     ? "IO":"   ",
        Ri.m_iGetCnt, Ri.m_iSetCnt);
      dbgp("                : %-20s", Ri.m_sRefTag());
      dbgp("                : %-20s", Ri.m_sRmtTag());
      dbgpln(Ri.m_sCnv() ? "(%s)" : "", Ri.m_sCnv());
      dbgunlock();
      }
    #endif
    Strng s;
    for (int i=0; i<Refs.GetSize(); i++)
      {
      CTagRefInfoItem &Ri = Refs[i];
      Strng SType, Gs, Ss;
      LPCTSTR pLine=Ri.m_fIsIO?"=========":"---------";
      LPCTSTR Ga=Ri.m_fIsIO?"<":"<<";
      LPCTSTR Sa=Ri.m_fIsIO?">":">>";
      Gs.Set(Ri.m_iGetCnt>0?"%i":" ", Ri.m_iGetCnt);
      Ss.Set(Ri.m_iSetCnt>0?"%i":" ", Ri.m_iSetCnt);
      if (Ri.m_fIsGetRef && Ri.m_fIsSetRef)
        SType.Set("(%s)%s%2.2s%s(%s)", Gs(), Ga, pLine, Sa, Ss());
      else if (Ri.m_fIsGetRef)
        SType.Set("(%s)%s%3.3s", Gs(), Ga, pLine);
      else if (Ri.m_fIsSetRef)
        SType.Set("%3.3s%s(%s)", pLine, Sa, Ss());
      else
        SType.Set("Err");
      //if (Tag.Len()<1)
      //  Tag = "?";
      s = Ri.m_sRefTag();
      if (Ri.m_sCnv())
        s.Append(" (%s)", Ri.m_sCnv());
      static LPCTSTR XX[]={"","  {Far}","  {Lcl}"," <<<<< ?? "};
      //s += XX[Ri.m_eReach];
      CPrjFormTagRefItem TRI;
      TRI.Used = Ri.m_sRmtTag();
      TRI.Type = SType();
      TRI.Tag = s();
      TRI.iGetCnt = Ri.m_fIsGetRef ? Ri.m_iGetCnt : -1;
      TRI.iSetCnt = Ri.m_fIsSetRef ? Ri.m_iSetCnt : -1;
      if (TRI.iGetCnt<0)
        TRI.sGetCnt = ".";
      else
        TRI.sGetCnt.Set("%d", TRI.iGetCnt);
      if (TRI.iSetCnt<0)
        TRI.sSetCnt = ".";
      else
        TRI.sSetCnt.Set("%d", TRI.iSetCnt);
      m_TagRefList.SetAtGrow(nTagRefInList++, TRI);
      }

    int W0=0, W1=0, W2=0;
    m_TagRefOrder.SetSize(nTagRefInList);
    for (i=0; i<nTagRefInList; i++)
      {
      m_TagRefOrder[i]=&m_TagRefList[i];

      TagRefList.InsertItem(i, LPSTR_TEXTCALLBACK);
      TagRefList.SetItemText(i, 1, LPSTR_TEXTCALLBACK);
      TagRefList.SetItemText(i, 2, LPSTR_TEXTCALLBACK);
      //TagRefList.SetItemText(i, 3, LPSTR_TEXTCALLBACK);
      //TagRefList.SetItemText(i, 4, LPSTR_TEXTCALLBACK);
      TagRefList.SetItemData(i, (DWORD)i);
      W0=Max(W0, TagRefList.GetStringWidth(m_TagRefList[i].Used()));
      W1=Max(W1, TagRefList.GetStringWidth(m_TagRefList[i].Type()));
      W2=Max(W2, TagRefList.GetStringWidth(m_TagRefList[i].Tag()));
      }

    TagRefList.SetColumnWidth(0, Max(W0+30, TagRefList.GetColumnWidth(0)));
    TagRefList.SetColumnWidth(1, Max(W1+20, TagRefList.GetColumnWidth(1)));
    TagRefList.SetColumnWidth(2, Max(W2+30, TagRefList.GetColumnWidth(2)));
    }
  bTagRefListBuilt = 1;
  UnlockWindowUpdate();
  SortTagRefList();
  }

//===========================================================================

IMPLEMENT_DYNCREATE(CPrjFrameWnd, CMDIChildWnd)

CPrjFrameWnd::CPrjFrameWnd()
  {
  }

//---------------------------------------------------------------------------

CPrjFrameWnd::~CPrjFrameWnd()
  {
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CPrjFrameWnd, CMDIChildWnd)
  //{{AFX_MSG_MAP(CPrjFrameWnd)
  ON_WM_GETMINMAXINFO()
  ON_WM_SIZE()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CPrjFrameWnd::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
  {
  if (message==WM_CLOSE)
    {//intercept close message...
    if (gs_Exec.Busy())
      {
      LogError("SysCAD", 0, "Must not be running");
      return True;
      }
    /*if (AfxMessageBox("Are you sure you want to close the project?", MB_YESNO)==IDNO)
      return True;*/
    if (GetActiveDocument())
      GetActiveDocument()->SetModifiedFlag(FALSE);
    }

  return CMDIChildWnd::OnWndMsg(message, wParam, lParam, pResult);
  }

//---------------------------------------------------------------------------

void CPrjFrameWnd::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI)
  {
  CMDIChildWnd::OnGetMinMaxInfo(lpMMI);
  lpMMI->ptMinTrackSize.x = MinPrjFormWidth + (4 * GetSystemMetrics(SM_CXBORDER)) + (2 * GetSystemMetrics(SM_CXEDGE));
  lpMMI->ptMinTrackSize.y = MinPrjFormHeight + GetSystemMetrics(SM_CYCAPTION) + (4 * GetSystemMetrics(SM_CYBORDER)) + (2 * GetSystemMetrics(SM_CYEDGE));
  }

//---------------------------------------------------------------------------

BOOL CPrjFrameWnd::PreCreateWindow(CREATESTRUCT& cs)
  {
  //cs.style &= (~WS_MAXIMIZEBOX);
  cs.style |= WS_MINIMIZE;
  return CMDIChildWnd::PreCreateWindow(cs);
  }

//---------------------------------------------------------------------------

void CPrjFrameWnd::OnSize(UINT nType, int cx, int cy)
  {
  CMDIChildWnd::OnSize(nType, cx, cy);
  if (nType==SIZE_MAXIMIZED)
    CWindowLists::SetWndMaxMode(TRUE);
  else if (nType==SIZE_MINIMIZED || nType==SIZE_RESTORED)
    CWindowLists::SetWndMaxMode(FALSE);
  }

//---------------------------------------------------------------------------
//===========================================================================


