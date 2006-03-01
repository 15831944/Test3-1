// analdsgn.cpp : implementation file
//

#include "stdafx.h"
#include "sc_defs.h"
#include "syscad.h"
#include "analdsgn.h"
#include "project.h"
#include "gpwfuncs.h"
#include "resource.h"
#include "helpinfo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int MaxItems = 256;

/////////////////////////////////////////////////////////////////////////////
// CAnalyseDesign dialog


CAnalyseDesign::CAnalyseDesign(CWnd* pParent /*=NULL*/)
	: CDialog(CAnalyseDesign::IDD, pParent)
  {
  m_ADIndex=-1;
  pTTC=NULL;
	//{{AFX_DATA_INIT(CAnalyseDesign)
	m_Status = _T("");
	//}}AFX_DATA_INIT
  gs_pPrj->bBusyAnalysing=true;
	Create(CAnalyseDesign::IDD, pParent);
  ShowWindow(SW_SHOW);
  }

CAnalyseDesign::~CAnalyseDesign()
  {
  gs_pPrj->bBusyAnalysing=false;
  delete pTTC;
  }


void CAnalyseDesign::DoDataExchange(CDataExchange* pDX)
  {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAnalyseDesign)
	DDX_Control(pDX, IDC_NODES, m_NodeTree);
	DDX_Control(pDX, IDC_TYPES, m_TypeTree);
	DDX_Text(pDX, IDC_STATUS, m_Status);
	//}}AFX_DATA_MAP
  }


BEGIN_MESSAGE_MAP(CAnalyseDesign, CDialog)
	//{{AFX_MSG_MAP(CAnalyseDesign)
	ON_NOTIFY(NM_SETFOCUS, IDC_TYPES, OnSetfocusTypes)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TYPES, OnSelchangedTypes)
	ON_NOTIFY(TVN_ITEMEXPANDED, IDC_TYPES, OnItemexpandedTypes)
	ON_BN_CLICKED(IDREFRESH, OnRefresh)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAnalyseDesign message handlers

void CAnalyseDesign::PostNcDestroy() 
  {
	// TODO: Add your specialized code here and/or call the base class

	CDialog::PostNcDestroy();
	delete this;
  }

//---------------------------------------------------------------------------

void CAnalyseDesign::OnOK() 
  {
	// TODO: Add extra validation here
  DoTheWork();
	//CDialog::OnOK();
  }

//---------------------------------------------------------------------------

void CAnalyseDesign::OnCancel() 
  {
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
  DestroyWindow();
  }

//---------------------------------------------------------------------------

void CAnalyseDesign::OnRefresh() 
  {
	// TODO: Add your control notification handler code here
	LoadSelectedNodes();
  }

//---------------------------------------------------------------------------

BOOL CAnalyseDesign::OnInitDialog() 
  {
	CDialog::OnInitDialog();
  pTTC = new CCustomToolTipCtrl(this);	
	// TODO: Add extra initialization here

  CBitmap BM;
  BM.LoadBitmap(IDB_FLWTREEIMGS);
  VERIFY(FlwTreeImgList.Create(16, 16, 0, 0, 2));
  FlwTreeImgList.Add(&BM, (CBitmap*)NULL);
  CImageList* pPrev = m_NodeTree.SetImageList(&FlwTreeImgList, TVSIL_NORMAL);

  BuildTypesTree();
	LoadSelectedNodes();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
  }

//---------------------------------------------------------------------------

BOOL CAnalyseDesign::PreTranslateMessage(MSG* pMsg) 
  {
  if (pTTC && HelpMngr.ShowToolTips())
    pTTC->RelayEvent(pMsg);
  return CDialog::PreTranslateMessage(pMsg);
  }

//---------------------------------------------------------------------------

BOOL CAnalyseDesign::OnToolTipNotify(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
  {
  if (pTTC && HelpMngr.ShowToolTips())
    return pTTC->OnToolTipNotify(pNMHDR, CAnalyseDesign::IDD);
  return FALSE;
  }

//---------------------------------------------------------------------------

//void CAnalyseDesign::OnActivate( UINT nState, CWnd* pWndOther, BOOL bMinimized )
//  {
//  CDialog::OnActivate( nState, pWndOther, bMinimized );
//  };

//void CAnalyseDesign::OnActivateFrame( UINT nState, CFrameWnd* pFrameWnd )
//  {
//  int xxx=0;  
//  };

//---------------------------------------------------------------------------

BOOL CAnalyseDesign::BuildTypesTree()
  {
  m_TypeTree.DeleteAllItems();
  
  HTREEITEM hClass[MaxItems];
  HTREEITEM hLastSelected=NULL, hItem;
  Strng     sClassName[MaxItems];
  int nClass=0;


  CProfINIFile PF(PrjIniFile());
  Strng LastSelectedAD=PF.RdStr("AnalyseDesign", "LastSelected", "");

  Strng Class, Name;
  for (int index=0; ; index++)
    {
    if (!gs_pTheSFELib->FE_AD_GetID(index, Class, Name))
      break;
    for (int iClass=0; iClass<nClass; iClass++)
      if (Class.XStrICmp(sClassName[iClass])==0)
        break;

    if (iClass==nClass)
      {
      if (nClass==MaxItems)
        {
        LogWarning("SysCAD", 0, "Analysis Tool %s - %s not Added", Class(), Name());
        continue;
        }
      nClass++;
      sClassName[iClass]=Class();

      TV_INSERTSTRUCT Ins;
      Ins.hParent             = TVI_ROOT;       
      Ins.hInsertAfter        = TVI_SORT;  
      Ins.item.mask           = TVIF_TEXT|TVIF_PARAM;
      Ins.item.state          = 0;
      Ins.item.stateMask      = 0;
      Ins.item.hItem          = NULL;
      Ins.item.pszText        = Class();         
      Ins.item.cchTextMax     = 0;      
      Ins.item.iImage         = 0;          
      Ins.item.iSelectedImage = 0;  
      Ins.item.cChildren      = 0;       
      Ins.item.lParam         = MAKELPARAM((WORD)MaxItems, (WORD)0);

      hClass[iClass] = m_TypeTree.InsertItem(&Ins);
      }
    TV_INSERTSTRUCT Ins;
    Ins.hParent             = hClass[iClass];
    Ins.hInsertAfter        = TVI_SORT;  
    Ins.item.mask           = TVIF_TEXT|TVIF_PARAM;
    Ins.item.state          = 0;
    Ins.item.stateMask      = 0;
    Ins.item.hItem          = NULL;
    Ins.item.pszText        = Name();         
    Ins.item.cchTextMax     = 0;      
    Ins.item.iImage         = 0;
    Ins.item.iSelectedImage = 0;  
    Ins.item.cChildren      = 0;       
    Ins.item.lParam         = MAKELPARAM((WORD)index, (WORD)0);
    
    hItem = m_TypeTree.InsertItem(&Ins);
    if (Name.XStrICmp(LastSelectedAD)==0)
      hLastSelected=hItem;

    }
  if (hLastSelected)
    {
    m_TypeTree.Select(hLastSelected, TVGN_CARET);
    //m_TypeTree.EnsureVisible(hLastSelected);
    }

  return m_TypeTree.GetCount()>0;
  };

//---------------------------------------------------------------------------

int CAnalyseDesign::GetSelectList()
  {
  SelectedTags.Clear();
  return gs_pPrj->BuildSelectedNodeList(SelectedTags);
  //return 0;
  }
        
//---------------------------------------------------------------------------

int CAnalyseDesign::SendSlctList2ADObj()
  {
  gs_pTheSFELib->FE_AD_Select(m_ADIndex, "TheObj");
  gs_pTheSFELib->FE_AD_SetSelectedTags(SelectedTags);
  return 0;
  }

//---------------------------------------------------------------------------

int CAnalyseDesign::GetADObjStatus()
  {
  if (m_ADIndex>=0)
    {
    Strng X;
    gs_pTheSFELib->FE_AD_GetStatus(X);
    m_Status=X();
    }
  else
    m_Status="";
  UpdateData(false);

  return 0;
  }

//---------------------------------------------------------------------------

int CAnalyseDesign::GetADObjTree()
  {
  m_NodeTree.DeleteAllItems();
  
  CFlwTree Tree;
  gs_pTheSFELib->FE_AD_GetTagTree (Tree);

  int iLvl=0;
  CFlwTreeItem * pItem[MaxItems];
  HTREEITEM hItem[MaxItems], hRoot=NULL;
  for (iLvl=0; iLvl<MaxItems; iLvl++)
    {
    hItem[iLvl]=NULL;
    pItem[iLvl]=NULL;
    }

  iLvl=0;
  CFlwTreeItem * p=Tree.pChild;
  while (p)
    {
//    hItem[iLvl]=NULL;
//    pItem[iLvl]=p;

    TV_INSERTSTRUCT Ins;
    Ins.hParent             = iLvl>0 ? hItem[iLvl-1]: TVI_ROOT;       
    Ins.hInsertAfter        = TVI_LAST;//TVI_SORT;  
    Ins.item.mask           = TVIF_TEXT|TVIF_PARAM|TVIF_IMAGE;
    Ins.item.state          = 0;
    Ins.item.stateMask      = 0;
    Ins.item.hItem          = NULL;
    Ins.item.pszText        = p->sText();         
    Ins.item.cchTextMax     = 0;      
    Ins.item.iImage         = p->iImage;          
    Ins.item.iSelectedImage = 0;  
    Ins.item.cChildren      = 0;       
    Ins.item.lParam         = MAKELPARAM((WORD)0, (WORD)0);

    pItem[iLvl]=p;
    hItem[iLvl]=m_NodeTree.InsertItem(&Ins);

    if (p->pChild)
      {
      iLvl++;
      p=p->pChild;
      }
    else
      {
      while (iLvl>=0)
        {
        p=pItem[iLvl]->pNext;
        if (p)
          break;
        iLvl--;
        }
      }
    }

  return m_NodeTree.GetCount()>0;
  }

//---------------------------------------------------------------------------

void CAnalyseDesign::LoadSelectedNodes()
  {
  GetSelectList();
  SendSlctList2ADObj();
  GetADObjStatus();
  GetADObjTree();
  };

//---------------------------------------------------------------------------

void CAnalyseDesign::OnSetfocusTypes(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

//---------------------------------------------------------------------------

void CAnalyseDesign::OnSelchangedTypes(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
  m_ADIndex=LPARAM(m_TypeTree.GetItemData(pNMTreeView->itemNew.hItem));
  if (m_ADIndex<MaxItems)
    {
    CProfINIFile PF(PrjIniFile());
    PF.WrStr("AnalyseDesign", "LastSelected", (const char *)m_TypeTree.GetItemText(pNMTreeView->itemNew.hItem));

    LoadSelectedNodes();
    }
  else
    {
    m_ADIndex=-1;
    m_Status="";
    UpdateData(false);
    }
	
  *pResult = 0;
}

//---------------------------------------------------------------------------

void CAnalyseDesign::OnItemexpandedTypes(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

//---------------------------------------------------------------------------

void CAnalyseDesign::DoTheWork()
  {
  gs_pTheSFELib->FE_AD_Go();
//TODO Do The Work
  }

//---------------------------------------------------------------------------
