// CAddToArchive.cpp : implementation file
//

#include "stdafx.h"
#include "sc_defs.h"
#include "resource.h"
#include "syscad.h"
#include "archdlgs.h"
#include "project.h"
#include "scd_wm.h"
#include "msgwnd.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAddToArchive dialog


CAddToArchive::CAddToArchive(TagInfoBlk &IB, CArchiveManager * pAM, CWnd* pParent /*=NULL*/)
  : CDialog(CAddToArchive::IDD, pParent),
  m_pAM(pAM)
  {
  //{{AFX_DATA_INIT(CAddToArchive)
  m_bAverage = TRUE;
  m_bCount = FALSE;
  m_bCurrent = FALSE;
  m_bMaximum = FALSE;
  m_bMinimum = FALSE;
  m_sTag = "";
  //}}AFX_DATA_INIT
  m_sTag.Format("%s.%s", IB.ObjTag(), IB.RefTag());
  m_iType=IB.Type();
  m_iCnv=IB.Cnv().Index();
  m_sCnvTxt=IB.Cnv().Text();
  }


void CAddToArchive::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CAddToArchive)
  DDX_Control(pDX, IDADD, m_OnAdd);
  DDX_Control(pDX, IDC_ARCTABLES, m_TableList);
  DDX_Check(pDX, IDC_ARC_AVERAGE, m_bAverage);
  DDX_Check(pDX, IDC_ARC_COUNT, m_bCount);
  DDX_Check(pDX, IDC_ARC_CURRENT, m_bCurrent);
  DDX_Check(pDX, IDC_ARC_MAXIMUM, m_bMaximum);
  DDX_Check(pDX, IDC_ARC_MINIMUM, m_bMinimum);
  DDX_Text(pDX, IDC_ARCTAG, m_sTag);
  //}}AFX_DATA_MAP
  }


BEGIN_MESSAGE_MAP(CAddToArchive, CDialog)
  //{{AFX_MSG_MAP(CAddToArchive)
  ON_NOTIFY(NM_CLICK, IDC_ARCTABLES, OnClickArctables)
  ON_BN_CLICKED(IDADD, OnAdd)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddToArchive message handlers

BOOL CAddToArchive::OnInitDialog() 
  {
  CDialog::OnInitDialog();

  m_TableList.InsertColumn(0, "Tables",  LVCFMT_LEFT, 10);

  m_TableList.DeleteAllItems();
  int Wide=1;
  for (int i=0; i<m_pAM->m_Tables.GetSize(); i++)
    {
    
    int n=m_TableList.InsertItem(i+1, m_pAM->m_Tables[i]->m_sName);
    m_TableList.SetItemData(n, (DWORD)i);
    };
  CSize sz(1,1);
  sz=m_TableList.ApproximateViewRect(sz);
  CRect rct;
  m_TableList.GetWindowRect(&rct);
  m_TableList.SetColumnWidth(0, Max((long)sz.cx, (long)(rct.Width()-4)));
  
  m_OnAdd.EnableWindow(FALSE);

  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
  }

void CAddToArchive::OnClickArctables(NMHDR* pNMHDR, LRESULT* pResult) 
  {
  m_OnAdd.EnableWindow(m_TableList.GetItemCount()>0);
  
  *pResult = 0;
  }

void CAddToArchive::OnAdd() 
  {
  CString Table;
  UpdateData(TRUE);

  POSITION pos = m_TableList.GetFirstSelectedItemPosition();
  if (pos )
    {
    int nItem = m_TableList.GetNextSelectedItem(pos);
    Table=m_TableList.GetItemText(nItem ,0);   

    if (m_bCount)   m_pAM->AppendField(m_sTag, Table, ADBFn_Count,     tt_Long, 0, "", "");
    if (m_bAverage) m_pAM->AppendField(m_sTag, Table, ADBFn_PeriodAvg, m_iType, m_iCnv, m_sCnvTxt(), "");
    if (m_bCurrent) m_pAM->AppendField(m_sTag, Table, ADBFn_Instant,   m_iType, m_iCnv, m_sCnvTxt(), "");
    if (m_bMaximum) m_pAM->AppendField(m_sTag, Table, ADBFn_Maximum,   m_iType, m_iCnv, m_sCnvTxt(), "");
    if (m_bMinimum) m_pAM->AppendField(m_sTag, Table, ADBFn_Minimum,   m_iType, m_iCnv, m_sCnvTxt(), "");

    }
  }

//===========================================================================

extern CArchiveSheet* pArcSheet = NULL;

//===========================================================================
//
//
//
//===========================================================================

IMPLEMENT_DYNAMIC(CArchiveSheet, CPropertySheet)

CArchiveSheet::CArchiveSheet(CArchiveManager * pAM, UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
  : CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
  {
  ASSERT(FALSE);
  bInited=false;
  }

//---------------------------------------------------------------------------

CArchiveSheet::CArchiveSheet(CArchiveManager * pAM, LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
  : CPropertySheet(pszCaption, pParentWnd, iSelectPage)
  {
  bInited=false;
  }

//---------------------------------------------------------------------------

CArchiveSheet::~CArchiveSheet()
  {
  pArcSheet = NULL; //set global pointer to property sheet to null
  //dialog just closed, ensure main SysCAD window becomes active ...
  ScdMainWnd()->PostMessage(WMU_UPDATEMAINWND, SUB_UPDMAIN_UPDATE, 0);
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CArchiveSheet, CPropertySheet)
  //{{AFX_MSG_MAP(CArchiveSheet)
  ON_WM_NCDESTROY()
  ON_WM_CLOSE()
  ON_COMMAND(IDOK,OnOK)
  ON_COMMAND(IDCANCEL,OnCancel)
  //}}AFX_MSG_MAP
  //ON_MESSAGE(WMU_DRVUPDATESTATS, OnUpdateStats)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CArchiveSheet::OnInitDialog() 
  {
  ASSERT(m_bModeless); //this code only needed when creating modeless property sheet!
  
  PROPSHEETTRICK0()
  BOOL b = CPropertySheet::OnInitDialog();
  PROPSHEETTRICK1()

  GetDlgItem(IDHELP)->ShowWindow(SW_HIDE);
  bInited=true;
  return b;
  }

//---------------------------------------------------------------------------

void CArchiveSheet::OnOK() 
  {
  ASSERT_VALID(this);
  //StoreCurPageNo();
  for (int i=0; i<GetPageCount(); i++)
    GetPage(i)->OnOK();
  if (GetActivePage()->OnKillActive())
    {
    //GetActivePage()->OnOK();
    DeletePages();
    EndDialog(IDOK);
    }
  else
    DeletePages();
  }

//---------------------------------------------------------------------------

void CArchiveSheet::OnCancel() 
  {
  ASSERT_VALID(this);
  //StoreCurPageNo();
  for (int i=0; i<GetPageCount(); i++)
    GetPage(i)->OnCancel();
  DeletePages();
  EndDialog(IDCANCEL);
  }

//---------------------------------------------------------------------------

void CArchiveSheet::OnClose()
  {
  ASSERT_VALID(this);
  OnCancel();
  }

//---------------------------------------------------------------------------

void CArchiveSheet::OnNcDestroy() 
  {
  CPropertySheet::OnNcDestroy();
  delete this;
  }

//---------------------------------------------------------------------------

void CArchiveSheet::DeletePages()
  {
  for (int i=0; i<GetPageCount(); i++)
    delete GetPage(i);
  }

//---------------------------------------------------------------------------

void CArchiveSheet::UpdateStats()
  {
  if (bInited)
    {
    PostMessage(WMU_DRVUPDATESTATS, (WPARAM)0, (LPARAM)0);
    CPropertyPage* p = GetPage(0);
    if (p && p->GetSafeHwnd())
      p->PostMessage(WMU_DRVUPDATEBTNS, (WPARAM)1, (LPARAM)0);
    }
  }

//---------------------------------------------------------------------------

void CArchiveSheet::Stop()
  {
  if (bInited)
    {
    CPropertyPage* p = GetPage(0);
    if (p && p->GetSafeHwnd())
      p->PostMessage(WMU_DRVUPDATEBTNS, (WPARAM)0, (LPARAM)0);
    }
  }

// --------------------------------------------------------------------------

//LRESULT CArchiveSheet::OnUpdateStats(WPARAM wParam, LPARAM lParam)
//  {
//  if (wParam)
//    SetActivePage(1);
//  CPropertyPage* p = GetActivePage();
//  if (p && p->GetSafeHwnd())
//    p->SendMessage(WMU_DRVUPDATESTATS, wParam, lParam);
//  return TRUE;
//  }

//---------------------------------------------------------------------------

void CArchiveSheet::DisplayTag(char* pGotoTag)
  {
  if (bInited && pGotoTag)
    {
    char* pTxt = new char[strlen(pGotoTag)+1];
    strcpy(pTxt, pGotoTag);
    PostMessage(WMU_DRVUPDATESTATS, (WPARAM)1, (LPARAM)pTxt);
    }
//  SetActivePage(1);
//  CPropertyPage* p = GetActivePage();
//  if (p && pGotoTag)
//    {
//    char* pTxt = new char[strlen(pGotoTag)+1];
//    strcpy(pTxt, pGotoTag);
//    p->PostMessage(WMU_DRVUPDATESTATS, (WPARAM)0, (LPARAM)pTxt);
//    }
  }

//===========================================================================
//
//
//
//===========================================================================

IMPLEMENT_DYNCREATE(CArcTablesPage, CPropertyPage)

CArcTablesPage::CArcTablesPage() : CPropertyPage(CArcTablesPage::IDD)
  {
  bDidInit = False;
  bTbTreeBuilt = false;
  //{{AFX_DATA_INIT(CArcTablesPage)
	//}}AFX_DATA_INIT
  }

//---------------------------------------------------------------------------

CArcTablesPage::~CArcTablesPage()
  {
  }

//---------------------------------------------------------------------------

void CArcTablesPage::DoDataExchange(CDataExchange* pDX)
  {
  CPropertyPage::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CArcTablesPage)
  DDX_Control(pDX, IDC_TABTREE, m_TbTree);
	//}}AFX_DATA_MAP
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CArcTablesPage, CPropertyPage)
  //{{AFX_MSG_MAP(CArcTablesPage)
	ON_NOTIFY(NM_RCLICK, IDC_TABTREE, OnRclickTabtree)
  ON_WM_KILLFOCUS()
	ON_NOTIFY(TVN_KEYDOWN, IDC_TABTREE, OnKeydownTabtree)
	//}}AFX_MSG_MAP
  ON_MESSAGE(WMU_DRVUPDATEBTNS, OnUpdateBtns)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CArcTablesPage::OnInitDialog() 
  {
  CPropertyPage::OnInitDialog();
  bDidInit = True;

  CBitmap BM;
  BM.LoadBitmap(IDB_ARCHIVEIMAGES);
  VERIFY(m_TbImgList.Create(16, 16, FALSE, 0, 2));
  m_TbImgList.Add(&BM, (CBitmap*)NULL);
  CImageList* pPrev = m_TbTree.SetImageList(&m_TbImgList, TVSIL_NORMAL);

  RebuildTbTree();

  UpdateDialogControls(this, FALSE);
  UpdateData(FALSE);
  return TRUE;
  }

// --------------------------------------------------------------------------

void CArcTablesPage::OnOK()
  {
  }

//---------------------------------------------------------------------------

LRESULT CArcTablesPage::OnUpdateBtns(WPARAM wParam, LPARAM lParam)
  {
  return 0;
  };

//---------------------------------------------------------------------------

inline HTREEITEM InsertItem(CTreeCtrl & Tree, HTREEITEM hPar,  long Img, LPCTSTR Txt, TV_INSERTSTRUCT &IS)
  {
  IS.hParent = hPar;
  IS.item.iImage = Img;
  IS.item.iSelectedImage = IS.item.iImage;
  IS.item.pszText = (LPTSTR)Txt;
  return Tree.InsertItem(&IS);
  }

//---------------------------------------------------------------------------
// problems with local time
CString MySecstoHMS(long Secs)
  {
  long H,M,S;
 
  S=Secs % 60;
  Secs-=S;
  M=(Secs % 3600);
  Secs-=M;
  M/=60;
  H=Secs/60;
  CString Str;
  Str.Format("%02i:%02i:%02i", H,M,S);
  return Str;
  }

void CArcTablesPage::RebuildTbTree()
  {
  UpdateData(TRUE);
  bTbTreeBuilt = 0;

  if (1)//m_pAM->Active())
    {
    CWaitMsgCursor WaitMsg("Building Table Tree");
    UpdateData(TRUE);
    m_TbTree.DeleteAllItems();
    CString Buff;

    TV_INSERTSTRUCT IS;
    IS.hInsertAfter = TVI_LAST;//TVI_SORT;
    IS.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
    IS.item.state = 0;
    IS.item.stateMask = 0;
    IS.item.iImage = 0;
    IS.item.iSelectedImage = 0;
    IS.item.cChildren = 0;
    
    for (int t=0; t<m_pAM->TableCount(); t++)
      {
      CArchiveDBTable & Tb=m_pAM->GetTable(t);

      HTREEITEM hTb = InsertItem(m_TbTree, TVI_ROOT, 0, Tb.m_sName, IS);
      if (Tb.m_bPeriodOn)
        {
        Buff.Format("Every %s offset by %s", (LPCTSTR)MySecstoHMS(Tb.m_lPeriod), (LPCTSTR)MySecstoHMS(Tb.m_lOffset));
        InsertItem(m_TbTree, hTb, 4, Buff, IS);
        }
      for (int i=0; i<Tb.TriggerCount(); i++)
        {
        CArchiveTrigger & Trg=Tb.GetTrigger(i);
        HTREEITEM hTr=InsertItem(m_TbTree, hTb, 2, Trg.m_Src.m_sTag, IS);
        if (Trg.m_bRise)
          {
          Buff.Format("Rising Edge of %f", Trg.m_dDeltaRise);
          InsertItem(m_TbTree, hTr, 5, Buff, IS);
          }
        if (Trg.m_bFall)
          {
          Buff.Format("Falling Edge of %f", Trg.m_dDeltaFall);
          InsertItem(m_TbTree, hTr, 6, Buff, IS);
          }
        if (Trg.m_bAny)
          {
          Buff.Format("Change of %f", Trg.m_dDeltaAny);
          InsertItem(m_TbTree, hTr, 7, Buff, IS);
          }
        }
      for (int f=0; f<Tb.FieldCount(); f++)
        {
        CArchiveDBField & Fld=Tb.GetField(f);
        InsertItem(m_TbTree, hTb, 1, Fld.m_sFldTag, IS);
        }
      }
    }
  bTbTreeBuilt = 1;
  }

//---------------------------------------------------------------------------

void CArcTablesPage::OnRclickTabtree(NMHDR* pNMHDR, LRESULT* pResult) 
  {
  TV_HITTESTINFO HTI;
  HTI.pt = m_TbTree.PrevDownPoint;
  if (m_TbTree.HitTest(&HTI) && ((HTI.flags & TVHT_ONITEM) || (HTI.flags & TVHT_ONITEMRIGHT) || (HTI.flags & TVHT_ONITEMICON)))
    {
    AfxMessageBox ("TODO: Must configure / add Period or Triggers" );
    }
	*pResult = 0;
  }

//---------------------------------------------------------------------------

void CArcTablesPage::OnKeydownTabtree(NMHDR* pNMHDR, LRESULT* pResult) 
  {
	TV_KEYDOWN* pTVKeyDown = (TV_KEYDOWN*)pNMHDR;
  if (pTVKeyDown->wVKey==VK_DELETE)
    {
    AfxMessageBox ("TODO: Must delete Tables Fields Triggers etc" );
    }
  
	*pResult = 0;
  }

//===========================================================================
//
//
//
//===========================================================================

IMPLEMENT_DYNCREATE(CArcInputPage, CPropertyPage)

CArcInputPage::CArcInputPage() : CPropertyPage(CArcInputPage::IDD)
  {
  bDidInit = False;
  //{{AFX_DATA_INIT(CArcInputPage)
  //}}AFX_DATA_INIT
  }

//---------------------------------------------------------------------------

CArcInputPage::~CArcInputPage()
  {
  }

//---------------------------------------------------------------------------

void CArcInputPage::DoDataExchange(CDataExchange* pDX)
  {
  CPropertyPage::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CArcInputPage)
  DDX_Control(pDX, IDC_SUBSDATATREE, m_SDTree);
  //}}AFX_DATA_MAP
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CArcInputPage, CPropertyPage)
  //{{AFX_MSG_MAP(CArcInputPage)
	ON_BN_CLICKED(IDC_ADDINPUT, OnAddinput)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CArcInputPage::OnInitDialog() 
  {
  CPropertyPage::OnInitDialog();
  bDidInit = True;

  CBitmap BM;
  BM.LoadBitmap(IDB_ARCHIVEIMAGES);
  VERIFY(m_SDImgList.Create(16, 16, FALSE, 0, 2));
  m_SDImgList.Add(&BM, (CBitmap*)NULL);
  CImageList* pPrev = m_SDTree.SetImageList(&m_SDImgList, TVSIL_NORMAL);

  RebuildSDTree();

  UpdateDialogControls(this, FALSE);
  UpdateData(FALSE);
  return TRUE;
  }

// --------------------------------------------------------------------------

void CArcInputPage::OnOK()
  {
  if (bDidInit)
    {
    }
  }

// --------------------------------------------------------------------------

void CArcInputPage::OnCancel()
  {
  if (bDidInit)
    {
    }
  }

//---------------------------------------------------------------------------

void CArcInputPage::RebuildSDTree()
  {
  UpdateData(TRUE);
  bSDTreeBuilt = 0;

  if (1)//m_pAM->Active())
    {
    CWaitMsgCursor WaitMsg("Building Subscription tree");
    UpdateData(TRUE);
    m_SDTree.DeleteAllItems();

    TV_INSERTSTRUCT IS;
    IS.hInsertAfter = TVI_SORT;
    IS.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
    IS.item.state = 0;
    IS.item.stateMask = 0;
    IS.item.iImage = 0;
    IS.item.iSelectedImage = 0;
    IS.item.cChildren = 0;
    
    for (int i=0; i<m_pAM->InputCount(); i++)
      {
      CArchiveInput & In=m_pAM->GetInput(i);
      CString Buff;

      if (In.m_sCnvTxt.GetLength()>0)
        Buff.Format("%s (%s)", (LPCTSTR)In.m_sScdMTag, In.m_sCnvTxt());
      else
        Buff=In.m_sScdMTag;
      HTREEITEM hIn = InsertItem(m_SDTree, TVI_ROOT, 3, Buff, IS);

      for (int t=0; t<m_pAM->TableCount(); t++)
        {
        CArchiveDBTable & Tb=m_pAM->GetTable(t);
        for (int tr=0; tr<Tb.TriggerCount(); tr++)
          {
          CArchiveTrigger & Trg=Tb.GetTrigger(tr);
          if (Trg.m_Src.m_lInputIndex==i)
            {
            Buff=Tb.m_sName;
            Buff+=" (";
            if (Trg.m_bRise)
              Buff+=",Rising";
            if (Trg.m_bFall)
              Buff+=",Falling";
            if (Trg.m_bAny)
              Buff+=",Change";
            Buff+=")";
            int i=Buff.Find("(,");
            if (i>=0)
              Buff.Delete(i+1);
            InsertItem(m_SDTree, hIn, 2, Buff, IS);
            }
          }
        }

      for (int f=0; f<m_pAM->FieldCount(); f++)
        {
        CArchiveDBField & Fld=m_pAM->GetField(f);
        if (Fld.m_Src.m_lInputIndex==i)
          {
          if (Fld.m_lTableIndex >=0)
            Buff.Format("%s.%s", (LPCTSTR)m_pAM->GetTable(Fld.m_lTableIndex).m_sName, (LPCTSTR)Fld.m_sFldTag);
          else
            Buff.Format("[%s.%s] Table Missing", (LPCTSTR)Fld.m_sTable, (LPCTSTR)Fld.m_sFldTag);
          InsertItem(m_SDTree, hIn, 1, Buff, IS);
          }
        }

      }
    }
  bSDTreeBuilt = 1;
  }

void CArcInputPage::OnAddinput() 
  {
  ::AfxMessageBox("TODO: Must Add an Input");	
  }

//===========================================================================
//
//
//
//===========================================================================

