//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
                     
#include "stdafx.h"
#include "sc_defs.h"
#include "gpwfuncs.h"
#include "resource.h"
#include "helpinfo.h"
#include "chngtag.h"
#include "accnode.h"
#include "selctwnd.h"
#include "copyblk.h"
#include "project.h"
#include "scd_wm.h"
#include "msgwnd.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//===========================================================================
// CChangeTag dialog

CChangeTag::CChangeTag(CWnd* pParent /*=NULL*/)
  : CDialog(CChangeTag::IDD, pParent)
  {
  fCallAccess = False;
  pTTC = NULL;
  //{{AFX_DATA_INIT(CChangeTag)
  m_NewTag = _T("");
  m_OldTag = _T("");
  //}}AFX_DATA_INIT
  }

//---------------------------------------------------------------------------

CChangeTag::CChangeTag(char* InitTag, int CallAccess, CWnd* pParent /*=NULL*/)
  : CDialog(CChangeTag::IDD, pParent)
  {
  pTTC = NULL;
  m_NewTag = _T(InitTag);
  m_OldTag = _T(InitTag);
  fCallAccess=CallAccess;
  }

//---------------------------------------------------------------------------

CChangeTag::~CChangeTag()
  {
  delete pTTC;
  }

//---------------------------------------------------------------------------

void CChangeTag::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CChangeTag)
  DDX_Text(pDX, IDC_NEWTAG, m_NewTag);
  DDX_Text(pDX, IDC_OLDTAG, m_OldTag);
  //}}AFX_DATA_MAP
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CChangeTag, CDialog)
  //{{AFX_MSG_MAP(CChangeTag)
  //}}AFX_MSG_MAP
  ON_EN_CHANGE(IDC_OLDTAG, OnChangeTag)
  ON_EN_CHANGE(IDC_NEWTAG, OnChangeTag)
  ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipNotify)
  ON_UPDATE_COMMAND_UI(IDOK, OnUpdateOK)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

void CChangeTag::OnOK() 
  {
  //gs_pAccWnd->CloseAccessData(False, False, True);
  //gs_pAccWnd->CloseWnd();
  UpdateData(TRUE);
  Strng OldTag(m_OldTag.GetBuffer(0));
  OldTag.Trim(" \t\n\r\v\f");
  Strng NewTag(m_NewTag.GetBuffer(0));
  if (OldTag.XStrICmp(NewTag)!=0)
    {
    CWaitCursor Wait;
    BOOL ok = TRUE;
    if (TaggedObject::ValidateTagChanged(NewTag))
      {
      Strng s;
      s.Set("New tag '%s' is illegal and has been altered to '%s'\nContinue?", (const char*)m_NewTag, NewTag());
      ok = (AfxMessageBox(s(), MB_YESNO|MB_ICONQUESTION)==IDYES);
      if (ok)
        LogNote("ChangeTag", 0, "New tag '%s' altered to '%s'", (const char*)m_NewTag, NewTag());
      }
    if (ok && !gs_pPrj->AllGrfLoaded())
      {
      Strng s;
      s.Set("All the graphics pages have not been loaded!\nTag '%s' may not be changed in the graphics window.\nContinue?", OldTag());
      ok = (AfxMessageBox(s(), MB_YESNO|MB_ICONEXCLAMATION|MB_DEFBUTTON2)==IDYES);
      }
    if (ok)
      {
      pStatusBar->SetMsg("Attempting to change tag %s to %s", OldTag(), NewTag());
      int Err = gs_Exec.ChangeTag(OldTag(), NewTag());
      Strng Txt;
      switch (Err)
        {
        case EOCT_NOTFOUND:
          Txt.Set("Tag %s not found", OldTag());
          break;
        case EOCT_DUPLICATE:
          Txt.Set("Duplicate Tag %s", NewTag());
          break;
        case EOCT_FAILED:
          Txt.Set("FAILURE %s", OldTag());
          break;
        case EOCT_NULLTAG:
          Txt.Set("Valid (not blank) tags must be specified");
          break;
        case EOCT_INVALIDTAG:
          Txt.Set("Valid (all characters must be legal) tags must be specified");
          break;
        case EOCT_BADTAGLEN:
          Txt.Set("New tag length is too long");
          break;
        case EOCT_NOTDIFFER:
          Txt.Set("Tags should differ");
          break;
        case EOCT_NOTALLOWED:
          Txt.Set("Tag %s not allowed to be changed", OldTag());
          break;
        case EOCT_DRVNOTALLOWED:
          Txt.Set("Tag %s is referenced by a driver and cannot be changed while solving", OldTag());
          break;
        case EOCT_ARCNOTALLOWED:
          Txt.Set("Tag %s is referenced by the acrhiver and cannot be changed while solving", OldTag());
          break;
        case EOCT_DONE:
          gs_AccessWnds.CloseAccessData(-1, False, False, True);
          gs_AccessWnds.CloseWnd(-1);
          for (int a=0; a<NAccessWnds; a++)
            {
            if (!fCallAccess && gs_AccessWnds.CurTag(a) && strlen(gs_AccessWnds.CurTag(a))>0 && stricmp(gs_AccessWnds.CurTag(a), OldTag())==0)
              {
              fCallAccess = 1;
              break;
              }
            }
          if (fCallAccess)
            {
            char* pTxt = new char[NewTag.Len()+1];
            strcpy(pTxt, NewTag());
            AfxGetMainWnd()->PostMessage(WMU_TAGACTION, SUB_TAGACTION_ACCESS, (LPARAM)pTxt);
            }

          CDialog::OnOK();
          break;
        }
      if (Txt.Len()>0)
        {
        m_OldTag = OldTag();
        m_NewTag = NewTag();
        MessageBox(Txt(), "Change Tag", MB_OK);
        UpdateData(FALSE);
        }
      else
        pStatusBar->SetMsg("Changed tag %s to %s", OldTag(), NewTag());
      }
    }
  else
    CDialog::OnOK();
  }

//---------------------------------------------------------------------------

BOOL CChangeTag::OnInitDialog() 
  {
  CDialog::OnInitDialog();
  pTTC = new CCustomToolTipCtrl(this);

  for (int a=0; a<NAccessWnds; a++)
    {
    if (m_OldTag.GetLength()==0 && gs_AccessWnds.CurTag(a) && strlen(gs_AccessWnds.CurTag(a))>0)
      {
      m_OldTag = gs_AccessWnds.CurTag(a);
      UpdateData(FALSE);
      }
    }
  if (m_NewTag.GetLength()>0)
    {
    GetDlgItem(IDC_NEWTAG)->SetFocus();
    ((CEdit*)GetDlgItem(IDC_NEWTAG))->SetSel(0, -1);
    return False;
    }
  UpdateDialogControls(this, FALSE);
  return TRUE;
  }

//---------------------------------------------------------------------------

BOOL CChangeTag::PreTranslateMessage(MSG* pMsg) 
  {
  if (pTTC && HelpMngr.ShowToolTips())
    pTTC->RelayEvent(pMsg);
  return CDialog::PreTranslateMessage(pMsg);
  }

//---------------------------------------------------------------------------

BOOL CChangeTag::OnToolTipNotify(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
  {
  if (pTTC && HelpMngr.ShowToolTips())
    return pTTC->OnToolTipNotify(pNMHDR, CChangeTag::IDD);
  return FALSE;
  }

//---------------------------------------------------------------------------

void CChangeTag::OnChangeTag() 
  {
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

void CChangeTag::OnUpdateOK(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(((CEdit*)GetDlgItem(IDC_NEWTAG))->LineLength()>0 && ((CEdit*)GetDlgItem(IDC_OLDTAG))->LineLength()>0);
  }

//===========================================================================
// CFindTagDlg dialog

BOOL CFindTagDlg::bPrevAutoAccess = 0;

CFindTagDlg::CFindTagDlg(CWnd* pParent /*=NULL*/)
  : CDialog(CFindTagDlg::IDD, pParent)
  {
  pTTC = NULL;
  //{{AFX_DATA_INIT(CFindTagDlg)
  m_AutoAccess = FALSE;
  m_ZoomPrompt = FALSE;
  m_MoveCursor = FALSE;
	m_RegExpOn = FALSE;
	//}}AFX_DATA_INIT
  bTrendFind = 0;
  }

//---------------------------------------------------------------------------

CFindTagDlg::~CFindTagDlg()
  {
  delete pTTC;
  }

//---------------------------------------------------------------------------

void CFindTagDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CFindTagDlg)
  DDX_Control(pDX, IDC_FINDTAGCOMBO2, m_Tag2Combo);
  DDX_Control(pDX, IDC_FINDTAGCOMBO, m_TagCombo);
  DDX_Check(pDX, IDC_GRFAUTOACCESS, m_AutoAccess);
  DDX_Check(pDX, IDC_GRFZOOMPROMPT, m_ZoomPrompt);
  DDX_Check(pDX, IDC_GRFMOVECURSOR, m_MoveCursor);
	DDX_Check(pDX, IDC_REGEXPON, m_RegExpOn);
	//}}AFX_DATA_MAP
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CFindTagDlg, CDialog)
  //{{AFX_MSG_MAP(CFindTagDlg)
  ON_BN_CLICKED(IDC_FIND, OnFind)
  ON_BN_CLICKED(IDC_FIND2, OnFind2)
	ON_CBN_SETFOCUS(IDC_FINDTAGCOMBO, OnSetfocusFindtagcombo)
	ON_CBN_SETFOCUS(IDC_FINDTAGCOMBO2, OnSetfocusFindtagcombo2)
	//}}AFX_MSG_MAP
  ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipNotify)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CFindTagDlg::OnInitDialog() 
  {
  CDialog::OnInitDialog();
  sFindTag = "";
  char Buffer[8192];
  CProfINIFile PF(PrjIniFile());
  pTTC = new CCustomToolTipCtrl(this);
  if (PF.RdSection("FindTags", Buffer, sizeof(Buffer))>0)
    {
    char* pBuff = Buffer;
    while (strlen(pBuff)>0)
      {
      char* pName;
      char* pFindTxt;
      PF.GetProfItem(pBuff, pName, pFindTxt);
      if (pFindTxt)
        m_TagCombo.AddString(pFindTxt);
      }
    }
  if (m_TagCombo.GetCount()>0)
    m_TagCombo.SetCurSel(0);
  if (PF.RdSection("FindTrendTags", Buffer, sizeof(Buffer))>0)
    {
    char* pBuff = Buffer;
    while (strlen(pBuff)>0)
      {
      char* pName;
      char* pFindTxt;
      PF.GetProfItem(pBuff, pName, pFindTxt);
      if (pFindTxt)
        m_Tag2Combo.AddString(pFindTxt);
      }
    }
  if (m_Tag2Combo.GetCount()>0)
    m_Tag2Combo.SetCurSel(0);
  m_AutoAccess = CFindTagDlg::bPrevAutoAccess;
  m_ZoomPrompt = gs_pPrj->bGrfPromptForZoom;
  m_MoveCursor = gs_pPrj->bGrfMoveCursor;
  m_RegExpOn = gs_pPrj->bGrfRegExpOn;
  //GetDlgItem(IDC_GRFAUTOACCESS)->EnableWindow(FALSE); //TEMP

  UpdateData(FALSE);
  return TRUE;
  }

//---------------------------------------------------------------------------

BOOL CFindTagDlg::PreTranslateMessage(MSG* pMsg) 
  {
  if (pTTC && HelpMngr.ShowToolTips())
    pTTC->RelayEvent(pMsg);
  return CDialog::PreTranslateMessage(pMsg);
  }

//---------------------------------------------------------------------------

BOOL CFindTagDlg::OnToolTipNotify(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
  {
  if (pTTC && HelpMngr.ShowToolTips())
    return pTTC->OnToolTipNotify(pNMHDR, CFindTagDlg::IDD);
  return FALSE;
  }

//---------------------------------------------------------------------------


void CFindTagDlg::OnOK() 
{
  // TODO: Add extra validation here
  UpdateData(TRUE);
  gs_pPrj->bGrfPromptForZoom = m_ZoomPrompt;
  gs_pPrj->bGrfMoveCursor = m_MoveCursor;
  gs_pPrj->bGrfRegExpOn = m_RegExpOn;
  gs_pPrj->bDoGrfAutoAccess = m_AutoAccess;

  CDialog::OnOK();
}
//---------------------------------------------------------------------------

void CFindTagDlg::OnFind() 
  {
  UpdateData(TRUE);
  bPrevAutoAccess = m_AutoAccess;
  gs_pPrj->bGrfPromptForZoom = m_ZoomPrompt;
  gs_pPrj->bGrfMoveCursor = m_MoveCursor;
  gs_pPrj->bGrfRegExpOn = m_RegExpOn;
  gs_pPrj->bDoGrfAutoAccess = m_AutoAccess;
  BOOL bChanged = 0;
  CString sCurSel;
  int nIndex = m_TagCombo.GetCurSel();
  if (nIndex==CB_ERR)
    {
    m_TagCombo.GetWindowText(sCurSel);
    if (sCurSel.GetLength()>0 && m_TagCombo.FindStringExact(-1, (const char*)sCurSel)==CB_ERR)
      {
      bChanged = 1;
      m_TagCombo.InsertString(0, (const char*)sCurSel);
      //m_TagCombo.SetCurSel(0);
      }
    }
  else
    {
    m_TagCombo.GetLBText(nIndex, sCurSel);
    if (nIndex>0)
      {
      bChanged = 1;
      m_TagCombo.DeleteString(nIndex);
      m_TagCombo.InsertString(0, (const char*)sCurSel);
      //m_TagCombo.SetCurSel(0);
      }
    }
  sFindTag = sCurSel;

  if (bChanged && sFindTag.GetLength()>0)
    {//list changed, save it...
    char Buff[8192];
    CProfINIFile PF(PrjIniFile());
    int Pos = 0;
    for (int i=0; i<16 && i<m_TagCombo.GetCount(); i++)
      {
      CString FindTxt;
      m_TagCombo.GetLBText(i, FindTxt);
      int Len = FindTxt.GetLength();
      if (Len>0)
        {
        sprintf(&Buff[Pos], "Find_%02d=%s", i, (const char*)FindTxt);
        Pos = Pos + 8 + Len;
        Buff[Pos++] = 0;
        }
      }
    Buff[Pos++] = 0;
    Buff[Pos++] = 0;
    PF.WrSection("FindTags", Buff);
    }
  bTrendFind = 0;
  EndDialog(IDOK);
  }

//---------------------------------------------------------------------------

void CFindTagDlg::OnFind2() 
  {
  UpdateData(TRUE);
  BOOL bChanged = 0;
  CString sCurSel;
  int nIndex = m_Tag2Combo.GetCurSel();
  if (nIndex==CB_ERR)
    {
    m_Tag2Combo.GetWindowText(sCurSel);
    if (sCurSel.GetLength()>0 && m_Tag2Combo.FindStringExact(-1, (const char*)sCurSel)==CB_ERR)
      {
      bChanged = 1;
      m_Tag2Combo.InsertString(0, (const char*)sCurSel);
      //m_Tag2Combo.SetCurSel(0);
      }
    }
  else
    {
    m_Tag2Combo.GetLBText(nIndex, sCurSel);
    if (nIndex>0)
      {
      bChanged = 1;
      m_Tag2Combo.DeleteString(nIndex);
      m_Tag2Combo.InsertString(0, (const char*)sCurSel);
      //m_Tag2Combo.SetCurSel(0);
      }
    }
  sFindTag = sCurSel;

  if (bChanged && sFindTag.GetLength()>0)
    {//list changed, save it...
    char Buff[8192];
    CProfINIFile PF(PrjIniFile());
    int Pos = 0;
    for (int i=0; i<16 && i<m_Tag2Combo.GetCount(); i++)
      {
      CString FindTxt;
      m_Tag2Combo.GetLBText(i, FindTxt);
      int Len = FindTxt.GetLength();
      if (Len>0)
        {
        sprintf(&Buff[Pos], "Find_%02d=%s", i, (const char*)FindTxt);
        Pos = Pos + 8 + Len;
        Buff[Pos++] = 0;
        }
      }
    Buff[Pos++] = 0;
    Buff[Pos++] = 0;
    PF.WrSection("FindTrendTags", Buff);
    }
  bTrendFind = 1;
  EndDialog(IDOK);
  }

//---------------------------------------------------------------------------

void CFindTagDlg::OnSetfocusFindtagcombo() 
  {
  SetDefID(IDC_FIND);
  }

//---------------------------------------------------------------------------

void CFindTagDlg::OnSetfocusFindtagcombo2() 
  {
  SetDefID(IDC_FIND2);
  }

//===========================================================================

const int ACB_Selected  = 1;
const int ACB_Blocks    = 2;
const int ACB_Database  = 3;
const int ACB_Others    = 4;

CCopyTagBlkDlg::CCopyTagBlkDlg(CExecObj* EO, char* ModelClass, char* Tag/*=NULL*/, CWnd* pParent/*=NULL*/)
  : CDialog(CCopyTagBlkDlg::IDD, pParent)
  //,AW(*gs_pAccWnd)
  {
  pEO = EO;
  sModelClass = ModelClass;
  sModelClass.LRTrim(" \t");
  sTag = Tag;
  sTag.LRTrim(" \t");
  iSrc = (sTag.Len()>0) ? ACB_Selected : ACB_Blocks;
  iDst = ACB_Others;
  bShowDefaults = 1;
  iWinMoreWidth = -1;
  pTTC = NULL;
  //{{AFX_DATA_INIT(CCopyTagBlkDlg)
  //}}AFX_DATA_INIT
  }

//---------------------------------------------------------------------------

CCopyTagBlkDlg::~CCopyTagBlkDlg()
  {
  delete pTTC;
  }

//---------------------------------------------------------------------------

void CCopyTagBlkDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CCopyTagBlkDlg)
  DDX_Control(pDX, IDC_CPYDETAILSLIST, PreviewList);
  DDX_Control(pDX, IDC_CPYUNIT, m_UnitsCombo);
  DDX_Control(pDX, IDC_CPYMODEL, m_ModelCombo);
  DDX_Control(pDX, IDC_CPYLBLKSLIST, LBlksList);
  DDX_Control(pDX, IDC_CPYRBLKSLIST, RBlksList);
  DDX_Control(pDX, IDC_CPYOTHERUNITSLIST, OtherUnitsList);
  DDX_Control(pDX, IDC_CPYTREE, m_CopyTree);
  //}}AFX_DATA_MAP
  if (!pDX->m_bSaveAndValidate)
    {
    ((CButton*)GetDlgItem(IDC_CPYBLOCKSSRC))->SetCheck(bShowDefaults);
    }
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CCopyTagBlkDlg, CDialog)
  //{{AFX_MSG_MAP(CCopyTagBlkDlg)
  ON_BN_CLICKED(IDC_CPYSELECTEDSRC, OnCpySelectedSrc)
  ON_BN_CLICKED(IDC_CPYBLOCKSSRC, OnCpyBlocksSrc)
  ON_BN_CLICKED(IDC_CPYBLOCKSDST, OnCpyBlocksDst)
  ON_BN_CLICKED(IDC_CPYDATABASESRC, OnCpyDatabaseSrc)
  ON_BN_CLICKED(IDC_CPYOTHERDST, OnCpyOtherDst)
  ON_BN_CLICKED(IDB_CPYNEW, OnNewBlock)
  ON_BN_CLICKED(IDB_CPYDELETE, OnDeleteBlock)
  ON_BN_CLICKED(IDB_CPYMAKEDEFAULT, OnMakeDefaultBlock)
  ON_BN_CLICKED(IDB_CPYTRANSFER, OnTransfer)
  ON_CBN_SELCHANGE(IDC_CPYMODEL, OnSelchangeCpymodel)
  ON_CBN_SELCHANGE(IDC_CPYUNIT, OnSelchangeCpyunit)
  ON_LBN_SELCHANGE(IDC_CPYRBLKSLIST, OnSelchangeCpyrblkslist)
  ON_LBN_SELCHANGE(IDC_CPYOTHERUNITSLIST, OnSelchangeCpyotherunitslist)
  ON_LBN_SELCHANGE(IDC_CPYLBLKSLIST, OnSelchangeCpylblkslist)
  ON_BN_CLICKED(IDC_CPYMORE, OnMoreLess)
  ON_BN_CLICKED(IDC_CPYSELECTALL, OnSelectAll)
  //}}AFX_MSG_MAP
  ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipNotify)
  ON_CONTROL(STVN_SELECTEDCHANGED, IDC_CPYTREE, OnTreeSelectedChanged)
  //ON_CONTROL(CLBN_CHKCHANGE, IDC_CPYRBLKSLIST, OnCheckChangeRBlkList)
  ON_CLBN_CHKCHANGE(IDC_CPYRBLKSLIST, OnCheckChangeRBlkList)
  ON_UPDATE_COMMAND_UI(IDB_CPYDELETE, OnUpdateCpyDelete)
  ON_UPDATE_COMMAND_UI(IDB_CPYTRANSFER, OnUpdateTransfer)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

//#define GETCHECK(ID)     (((CButton*)GetDlgItem((ID)))->GetCheck())
#define SETCHECK(ID, S)  ((CButton*)GetDlgItem((ID)))->SetCheck((S));
#define SETSRCDESC(T)    (GetDlgItem(IDC_CPYSRCDESC))->SetWindowText((T));
#define SETDSTDESC(T)    (GetDlgItem(IDC_CPYDSTDESC))->SetWindowText((T));
#define MOVEWND(ID, R)   (GetDlgItem((ID)))->MoveWindow(&(R));

CTagTrees CCopyTagBlkDlg::TagTrees;

BOOL CCopyTagBlkDlg::OnInitDialog() 
  {
  CDialog::OnInitDialog();

  m_CopyTree.LoadImage();

  LBlksList.SetCheckStyle(BS_CHECKBOX);
  //RBlksList.SetCheckStyle(BS_CHECKBOX);

  /*CRect WndRect;
  GetWindowRect(WndRect);
  //GetClientRect(ClntRect);
  GetDlgItem(IDC_CPYTREE)->GetWindowRect(SrcTreeRect);
  GetDlgItem(IDC_CPYLBLKSLIST)->GetWindowRect(DstDefRect);
  GetDlgItem(IDC_CPYOTHERLIST)->GetWindowRect(DstUnitRect);
  GetDlgItem(IDC_CPYUNIT)->GetWindowRect(SrcComboRect);
  const int xo = -(WndRect.left+GetSystemMetrics(SM_CXFIXEDFRAME));
  const int yo = -(WndRect.top+GetSystemMetrics(SM_CYFIXEDFRAME)+GetSystemMetrics(SM_CYCAPTION));
  SrcTreeRect.OffsetRect(xo, yo);
  DstDefRect.OffsetRect(xo, yo);
  DstUnitRect.OffsetRect(xo, yo);
  SrcComboRect.OffsetRect(xo, yo);
  DstUnitRect.left = DstDefRect.left;
  DstTreeRect = DstUnitRect;
  DstTreeRect.top = SrcTreeRect.top;
  SrcDefRect = SrcTreeRect;
  SrcDefRect.top = DstDefRect.top;
  SrcUnitRect = SrcDefRect;
  DstComboRect = SrcComboRect;
  DstComboRect.left = DstUnitRect.left;
  DstComboRect.right = DstComboRect.left + SrcComboRect.Width();*/

  pTTC = new CCustomToolTipCtrl(this);
  pTTC->UpdateTipText("Show source preview", GetDlgItem(IDC_CPYMORE));
  pTTC->UpdateTipText("Select all", GetDlgItem(IDC_CPYSELECTALL));
  
  TagTrees.Rebuild(RQ_Tags);

  if (sTag.Len()>0 && sModelClass.Len()==0)
    {
    CXM_ObjectTag ObjTag(sTag(), TABOpt_Parms);//TABOpt_AllInfoOnce);
    CXM_ObjectData ObjData;
    CXM_Route Route;
    if (pEO->XReadTaggedItem(ObjTag, ObjData, Route))
      {
      CPkDataItem * pItem = ObjData.FirstItem();
      sModelClass = pItem->Class();
      pTagObjClass pTagObj = TagObjClass::FindClassId(sModelClass());
      if (pTagObj==NULL)
        pTagObj = TagObjClass::FindClassName(sModelClass());
      if (pTagObj)
        sModelClass = pTagObj->ShortDesc();
      }
    else
      sTag = "";
    }
  iSrc = (sTag.Len()>0) ? ACB_Selected : ACB_Blocks;

  CModelTypeListArray& List = TagTrees.GetList();
  for (int i=0; i<List.GetSize(); i++)
    if (List[i]->GetSize()>0)
      m_ModelCombo.AddString(List[i]->ModelName());
  if (m_ModelCombo.GetCount()>0)
    {
    i = m_ModelCombo.FindStringExact(-1, sModelClass());
    if (i<0)
      {
      i = 0;
      CString Txt;
      m_ModelCombo.GetLBText(i, Txt);
      sModelClass = (const char*)Txt;
      }
    m_ModelCombo.SetCurSel(i);
    }
  BuildUnitsList();
  i = m_UnitsCombo.FindStringExact(-1, sTag());
  if (i<0 || sTag.Len()==0)
    {
    i = 0;
    CString Txt;
    m_UnitsCombo.GetLBText(i, Txt);
    sTag = (const char*)Txt;
    }
  m_UnitsCombo.SetCurSel(i);
  BuildTree();
  BuildOtherUnitsList();
  UpdateDescriptors();

  BuildBlockList();

  CRect WinRect,Rect;
  GetWindowRect(&WinRect);
  GetDlgItem(IDC_CPYMORE)->GetWindowRect(&Rect);
  iWinMoreWidth = WinRect.right - Rect.right - 6;
  CProfINIFile PF(PrjIniFile());
  bShowMore = (PF.RdInt("General", "DataXFer_ShowMore", 0)!=0);
  if (!bShowMore)
    {
    bShowMore = !bShowMore;
    OnMoreLess();
    }

  UpdateDialogControls(this, FALSE);
  return TRUE;
  }

//---------------------------------------------------------------------------

BOOL CCopyTagBlkDlg::PreTranslateMessage(MSG* pMsg) 
  {
  if (pTTC && HelpMngr.ShowToolTips())
    pTTC->RelayEvent(pMsg);
  return CDialog::PreTranslateMessage(pMsg);
  }

//---------------------------------------------------------------------------

BOOL CCopyTagBlkDlg::OnToolTipNotify(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
  {
  if (pTTC && HelpMngr.ShowToolTips())
    return pTTC->OnToolTipNotify(pNMHDR, CCopyTagBlkDlg::IDD);
  return FALSE;
  }

//---------------------------------------------------------------------------

flag CCopyTagBlkDlg::BuildTree()
  {
  m_CopyTree.DeleteAllItems();

#if 1

  if (sTag.Len()>0)
    {
    TV_INSERTSTRUCT Ins;
    Ins.hParent             = TVI_ROOT;       
    Ins.hInsertAfter        = TVI_LAST;//TVI_SORT;  
    Ins.item.mask           = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
    Ins.item.state          = 0;
    Ins.item.stateMask      = 0;
    Ins.item.hItem          = NULL;
    Ins.item.pszText        = NULL;         
    Ins.item.cchTextMax     = 0;      
    Ins.item.iImage         = 0;          
    Ins.item.iSelectedImage = 0;  
    Ins.item.cChildren      = 0;       
    Ins.item.lParam         = 0;
    CTagTree TT;
    TT.Build(sTag());
    TT.PopulateTree(m_CopyTree, Ins, isParm, 0);//, ButtonFld);
    }

#else

  int iLvl=0;
  const int MaxItems = 256;
  HTREEITEM hItem[MaxItems], hRoot=NULL;
  for (iLvl=0; iLvl<MaxItems; iLvl++)
    hItem[iLvl]=NULL;

  Strng Txt;
  int PrvLvl=0;
  for (int di = 0; (di < AW.NData); di++ )
    {
    rAccNdData d = AW.Data[di];
//    dbgpln("Fld %3i Typ:%-16s S:%i E:%i Lvl:%3i :%s",
//          di, tt_TypeString(d.iType), d.iStrtFld, d.iEndFld, d.iLvl, d.sSymOrTag());

    ASSERT(d.iLvl<MaxItems);
    for (int i=PrvLvl+1; i<=d.iLvl; i++)
      hItem[i] = NULL;

    if (!IsLayout(d.iType) && 
        !IsStructEnd(d.iType) && 
        (!IsData(d.iType)/* || (d.fIsParam && !bBlocksOnly)*/))
      {
      TV_INSERTSTRUCT Ins;
      Ins.hParent        = TVI_ROOT;       
      Ins.hInsertAfter   = TVI_LAST;  
      Ins.item.mask      = TVIF_TEXT|TVIF_PARAM;
      Ins.item.state     = 0;
      Ins.item.stateMask = 0;

      if (IsData(d.iType))
        Txt.Set("%s (%s)", d.sSymOrTag(), d.sValue());
      else
        Txt = d.sSymOrTag();
      Ins.item.hItem          = NULL;
      Ins.item.pszText        = Txt();         
      Ins.item.cchTextMax     = 0;      
      Ins.item.iImage         = 0;          
      Ins.item.iSelectedImage = 0;  
      Ins.item.cChildren      = 0;       
      Ins.item.lParam         = MAKELPARAM((WORD)di, (WORD)0);
   
      if (d.iLvl>0)
        {
        int i = d.iLvl-1;
        while (i>0 && hItem[i]==NULL)
          i--;
        Ins.hParent = hItem[i];
        hItem[d.iLvl] = m_CopyTree.InsertItem(&Ins);
        }
      else
        {
        hItem[d.iLvl] = m_CopyTree.InsertItem(&Ins);
        hRoot = hItem[d.iLvl];
        }
      }
    
    PrvLvl = d.iLvl;
    }

#endif

  if (m_CopyTree.GetRootItem())
    m_CopyTree.Expand(m_CopyTree.GetRootItem(), TVM_EXPAND);
  return m_CopyTree.GetCount()>0;
  }

//---------------------------------------------------------------------------

void CCopyTagBlkDlg::BldTreeSelList(HTREEITEM h, char* TagPrefix, int& Cnt, CSVector& List)
  {
  CString s;
  int i;
  while (h)
    {
    BOOL b = m_CopyTree.GetItemSelected(h);
    if (b)
      {
      //rAccNdData d = AW.Data[LOWORD(dw)];
      //s = d.sFullTag();
      s = m_CopyTree.GetItemText(h);
      i = s.Find(" (");
      if (i>=0)
        s = s.Left(i);
      List[Cnt].Set("%s%s", TagPrefix, (const char*)s);
      Cnt++;
      }
    if (m_CopyTree.ItemHasChildren(h))
      {
      if (!b)
        {
        s = m_CopyTree.GetItemText(h);
        i = s.Find(" (");
        if (i>=0)
          s = s.Left(i);
        }
      Strng ss;
      ss.Set("%s%s.", TagPrefix, (const char*)s);
      BldTreeSelList(m_CopyTree.GetChildItem(h), ss(), Cnt, List);
      }
    h = m_CopyTree.GetNextSiblingItem(h);
    }
  }

//---------------------------------------------------------------------------

int CCopyTagBlkDlg::BuildTreeSelList(CSVector& List, char* pRootTag/*=NULL*/)
  {
  int Cnt = m_CopyTree.CntSelected(m_CopyTree.GetRootItem());
  if (Cnt>0)
    {
    const int SelCnt = Cnt;
    List.SetSize(SelCnt);
    Cnt = 0;
    Strng ss;
    if (pRootTag)
      ss = pRootTag;
    else
      ss = sTag();
    ss += '.';
    BldTreeSelList(/*TVI_ROOT*/m_CopyTree.GetRootItem(), ss(), Cnt, List);
    ASSERT(Cnt==SelCnt);
    return SelCnt;
    }
  return 0;
  }

//---------------------------------------------------------------------------

void CCopyTagBlkDlg::BuildUnitsList()
  {
  m_UnitsCombo.ResetContent();
  CModelTypeListArray& List = TagTrees.GetList();
  int index = -1;
  while ((index=TagTrees.FindPartModelName(sModelClass(),index))>=0)
    {
    CModelTypeList* pTagList = List[index];
    for (int i=0; i<pTagList->GetSize(); i++)
      m_UnitsCombo.AddString(pTagList->GetTagAt(i));
    }
  }

//---------------------------------------------------------------------------

void CCopyTagBlkDlg::BuildOtherUnitsList()
  {
  //const int PrevSelCnt = OtherUnitsList.GetSelCount();
  OtherUnitsList.ResetContent();
  CModelTypeListArray& List = TagTrees.GetList();
  int i;
  int index = -1;
  while ((index=TagTrees.FindPartModelName(sModelClass(),index))>=0)
    {
    CModelTypeList* pTagList = List[index];
    for (i=0; i<pTagList->GetSize(); i++)
      {
      if (iSrc!=ACB_Selected || stricmp(sTag(), pTagList->GetTagAt(i))!=0)
        OtherUnitsList.AddString(pTagList->GetTagAt(i));
      }
    }
  }

//---------------------------------------------------------------------------

void CCopyTagBlkDlg::BuildBlockList()
  {
  const int LPrevSel = LBlksList.GetCurSel();
  CString LTxt;
  if (LPrevSel>=0)
    LBlksList.GetText(LPrevSel, LTxt);
  const int RPrevSel = RBlksList.GetCurSel();
  CString RTxt;
  if (RPrevSel>=0)
    RBlksList.GetText(RPrevSel, RTxt);
  int i,j;
  if (NewBlocks.GetSize()>0)
    {
    NewBlocksFlags.SetSize(NewBlocks.GetSize());
    for (i=0; i<NewBlocks.GetSize(); i++)
      {
      j = RBlksList.FindStringExact(-1, NewBlocks[i]());
      NewBlocksFlags[i] = ((j<0 || RBlksList.GetCheck(j)!=0) ? 1 : 0);
      }
    }
  CSVector List;
  CWordArray FlagList;
  int DefaultIndex;
  CCopyBlock::BuildBlockList(sModelClass(), List, FlagList, DefaultIndex);
  LBlksList.ResetContent();
  RBlksList.ResetContent();
  for (i=0; i<List.GetSize(); i++)
    {
    j = LBlksList.AddString(List[i]());
    LBlksList.SetCheck(j, FlagList[i]!=0);
    j = RBlksList.AddString(List[i]());
    RBlksList.SetCheck(j, FlagList[i]!=0);
    }
  for (i=0; i<NewBlocks.GetSize(); i++)
    {
    if (RBlksList.FindStringExact(-1, NewBlocks[i]())<0)
      {
      j = RBlksList.AddString(NewBlocks[i]());
      RBlksList.SetCheck(j, NewBlocksFlags[i]);
      }
    }

  if (LBlksList.GetCount()>0)
    {
    i = 0;
    if (LPrevSel>=0)
      {
      i = LBlksList.FindStringExact(-1, (const char*)LTxt);
      if (i<0)
        i = 0;
      }
    LBlksList.SetCurSel(i);
    }
  if (RBlksList.GetCount()>0)
    {
    i = 0;
    if (RPrevSel>=0)
      {
      i = RBlksList.FindStringExact(-1, (const char*)RTxt);
      if (i<0)
        i = 0;
      }
    RBlksList.SetCurSel(i);
    }
  }

//---------------------------------------------------------------------------

void CCopyTagBlkDlg::BuildPreviewList()
  {
  if (bShowMore)
    {
    PreviewList.ResetContent();
    CSVector List;
    if (iSrc==ACB_Selected)
      {
      //PreviewList.AddString("Not implemented");
      BuildTreeSelList(List, "x");
      }
    else if (iSrc==ACB_Blocks)
      {
      const int LBlkSelIndex = LBlksList.GetCurSel();
      if (LBlkSelIndex>=0)
        {
        CString Txt;
        LBlksList.GetText(LBlkSelIndex, Txt);
        CCopyBlock::BuildBlockTagsList(sModelClass(), (char*)(const char*)Txt, LBlksList.GetCheck(LBlkSelIndex), List);
        }
      }
    for (int i=0; i<List.GetSize(); i++)
      PreviewList.AddString(List[i]());
    }
  }

//---------------------------------------------------------------------------

flag CCopyTagBlkDlg::UpdateDescriptors()
  {
  const int Cnt = 9;
  CWnd* w[Cnt];
  w[0] = GetDlgItem(IDC_CPYUNIT);
  w[1] = GetDlgItem(IDC_CPYSELECTALL);
  w[2] = GetDlgItem(IDC_CPYTREE);
  w[3] = GetDlgItem(IDC_CPYOTHERUNITSLIST);
  w[4] = GetDlgItem(IDC_CPYLBLKSLIST);
  w[5] = GetDlgItem(IDC_CPYRBLKSLIST);
  w[6] = GetDlgItem(IDB_CPYNEW);
  w[7] = GetDlgItem(IDB_CPYDELETE);
  w[8] = GetDlgItem(IDB_CPYMAKEDEFAULT);
  BOOL b[Cnt];
  b[0] = (iSrc==ACB_Selected);
  b[1] = b[0];
  b[2] = b[0];
  b[3] = (iDst==ACB_Others);
  b[4] = (iSrc==ACB_Blocks);
  b[5] = (iDst==ACB_Blocks);
  b[6] = b[5];
  b[7] = b[5];
  b[8] = b[5];

  if (iSrc==ACB_Selected)
    {
    SETSRCDESC("Source: Selected Unit");
    }
  else if (iSrc==ACB_Blocks)
    {
    SETSRCDESC("Source: Block List");
    }
  else if (iSrc==ACB_Database)
    {
    ASSERT(FALSE);
    SETSRCDESC("Source: Remote Database");
    }

  if (iDst==ACB_Blocks)
    {
    SETDSTDESC("Destination: Block List");
    }
  else if (iDst==ACB_Others)
    {
    SETDSTDESC("Destination: Other Units");
    }

  for (int i=0; i<Cnt; i++)
    {
    w[i]->ShowWindow(b[i] ? SW_SHOWNA : SW_HIDE);
    w[i]->EnableWindow(b[i]);
    }

  SETCHECK(IDC_CPYSELECTEDSRC, iSrc==ACB_Selected);
  SETCHECK(IDC_CPYBLOCKSSRC, iSrc==ACB_Blocks);
  SETCHECK(IDC_CPYDATABASESRC, iSrc==ACB_Database);

  SETCHECK(IDC_CPYBLOCKSDST, iDst==ACB_Blocks);
  SETCHECK(IDC_CPYOTHERDST, iDst==ACB_Others);

  GetDlgItem(IDB_CPYMAKEDEFAULT)->EnableWindow(FALSE); //temp
  GetDlgItem(IDC_CPYDATABASESRC)->ShowWindow(SW_HIDE); //temp

  UpdateDialogControls(this, FALSE);
  return 0;
  }

//---------------------------------------------------------------------------

void CCopyTagBlkDlg::OnSelchangeCpymodel() 
  {
  const int i = m_ModelCombo.GetCurSel();
  if (i>=0)
    {
    CString Txt;
    m_ModelCombo.GetLBText(i, Txt);
    if (stricmp((const char*)Txt, sModelClass())!=0)
      {
      sModelClass = (const char*)Txt;
      NewBlocks.SetSize(0);
      NewBlocksFlags.SetSize(0);
      iSrc = ACB_Blocks;
      iDst = ACB_Others;
      BuildUnitsList();
      m_UnitsCombo.GetLBText(0, Txt);
      sTag = (const char*)Txt;
      m_UnitsCombo.SetCurSel(0);
      BuildTree();
      BuildOtherUnitsList();
      BuildBlockList();
      BuildPreviewList();
      UpdateDescriptors();
      }
    }
  }

//---------------------------------------------------------------------------

void CCopyTagBlkDlg::OnSelchangeCpyunit() 
  {
  const int i = m_UnitsCombo.GetCurSel();
  if (i>=0)
    {
    CString Txt;
    m_UnitsCombo.GetLBText(i, Txt);
    if (stricmp((const char*)Txt, sTag())!=0)
      {
      sTag = (const char*)Txt;
      BuildTree();
      BuildOtherUnitsList();
      BuildPreviewList();
      UpdateDescriptors();
      }
    }
  }

//---------------------------------------------------------------------------

void CCopyTagBlkDlg::OnCpySelectedSrc() 
  {
  iSrc=ACB_Selected;
  if (iDst==ACB_Others)
    BuildOtherUnitsList();
  BuildPreviewList();
  UpdateDescriptors();
  }

//---------------------------------------------------------------------------

void CCopyTagBlkDlg::OnCpyDatabaseSrc() 
  {
  VERIFY(FALSE); //not implemented
  iSrc = ACB_Database;
  UpdateDescriptors();
  }

//---------------------------------------------------------------------------

void CCopyTagBlkDlg::OnCpyBlocksSrc() 
  {
  iSrc = ACB_Blocks;
  if (iDst==ACB_Others)
    BuildOtherUnitsList();
  BuildBlockList();
  BuildPreviewList();
  UpdateDescriptors();
  }

//---------------------------------------------------------------------------

void CCopyTagBlkDlg::OnCpyBlocksDst() 
  {
  iDst = ACB_Blocks;
  BuildBlockList();
  UpdateDescriptors();
  }

//---------------------------------------------------------------------------

void CCopyTagBlkDlg::OnCpyOtherDst() 
  {
  iDst = ACB_Others;
  BuildOtherUnitsList();
  UpdateDescriptors();
  }

//---------------------------------------------------------------------------


void CCopyTagBlkDlg::OnSelchangeCpyrblkslist() 
  {
  UpdateDialogControls(this, FALSE);
  }

void CCopyTagBlkDlg::OnSelchangeCpyotherunitslist() 
  {
  UpdateDialogControls(this, FALSE);
  }

void CCopyTagBlkDlg::OnSelchangeCpylblkslist() 
  {
  BuildPreviewList();
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

void CCopyTagBlkDlg::OnOK() 
  {
  CProfINIFile PF(PrjIniFile());
  PF.WrInt("General", "DataXFer_ShowMore", bShowMore);
  CDialog::OnOK();
  }

//---------------------------------------------------------------------------

void CCopyTagBlkDlg::OnMoreLess() 
  {
  bShowMore = !bShowMore;
  SetDlgItemText(IDC_CPYMORE, bShowMore ? "<<&Less" : ">>&More");
  GetDlgItem(IDC_CPYDETAILSLIST)->EnableWindow(bShowMore);
  CRect WinRect;
  GetWindowRect(&WinRect);
  if (bShowMore)
    WinRect.right += iWinMoreWidth;
  else
    WinRect.right -= iWinMoreWidth;
  MoveWindow(&WinRect, TRUE);
  BuildPreviewList();
  pTTC->UpdateTipText(bShowMore ? "Hide source preview" : "Show source preview", GetDlgItem(IDC_CPYMORE));
  }

//---------------------------------------------------------------------------

void CCopyTagBlkDlg::OnNewBlock() 
  {
  CStringDlg Dlg("Copy Block Name", "Block name:");
  if (Dlg.DoModal()==IDOK)
    {
    Strng s(Dlg.Data());
    s.LRTrim(" \t\n[]()");
    s.ReplaceChars(" []()", '_');
    if (s.Len()>0)
      {
      int i = RBlksList.FindStringExact(-1, s());
      if (i<0)
        {
        i = RBlksList.AddString(s());
        RBlksList.SetCheck(i, TRUE);
        NewBlocks.SetSize(NewBlocks.GetSize()+1);
        NewBlocks[NewBlocks.GetSize()-1] = s();
        }
      if (i>=0)
        RBlksList.SetCurSel(i);
      UpdateDialogControls(this, FALSE);
      }
    }
  }

//---------------------------------------------------------------------------

void CCopyTagBlkDlg::OnDeleteBlock() 
  {
  int i = RBlksList.GetCurSel();
  if (i>=0)
    {
    CString Txt;
    RBlksList.GetText(i, Txt);
    CCopyBlock::RemoveBlock(sModelClass(), (char*)(const char*)Txt);
    if (RBlksList.DeleteString(i)>0)
      RBlksList.SetCurSel(Max(i-1, 0));
    i = NewBlocks.Find((char*)(const char*)Txt);
    if (i>=0)
      {
      NewBlocks.Swop(i, NewBlocks.GetSize()-1);
      NewBlocks.SetSize(NewBlocks.GetSize()-1);
      }
    BuildBlockList();
    UpdateDialogControls(this, FALSE);
    }
  } 

void CCopyTagBlkDlg::OnUpdateCpyDelete(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(RBlksList.GetCurSel()!=LB_ERR);
  }

//---------------------------------------------------------------------------

void CCopyTagBlkDlg::OnCheckChangeRBlkList()
  {
  int i = RBlksList.GetCurSel();
  ASSERT(i>=0);
  CString Txt;
  RBlksList.GetText(i, Txt);
  CCopyBlock::MoveBlockLocation(sModelClass(), (char*)(const char*)Txt, RBlksList.GetCheck(i));
  BuildBlockList();
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

void CCopyTagBlkDlg::OnMakeDefaultBlock()
  {
  int i = LBlksList.GetCurSel();
  if (i>=0)
    {
    UpdateDialogControls(this, FALSE);
    }
  }

//---------------------------------------------------------------------------

void CCopyTagBlkDlg::OnSelectAll() 
  {
  m_CopyTree.SelectAll(!m_CopyTree.IsAnItemSelected(m_CopyTree.GetRootItem()));
  }

//---------------------------------------------------------------------------

void CCopyTagBlkDlg::OnTreeSelectedChanged()
  {
  BuildPreviewList();
  pTTC->UpdateTipText(m_CopyTree.IsAnItemSelected(m_CopyTree.GetRootItem()) ? "Unselect all" : "Select all", GetDlgItem(IDC_CPYSELECTALL));
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------
/*
AccDataBlk* CCopyTagBlkDlg::CopyFromNode() 
  {
  TV_ITEM Item = { TVIF_HANDLE | TVIF_PARAM | TVIF_STATE };
  Item.hItem = m_CopyTree.GetSelectedItem();
  if (m_CopyTree.GetItem(&Item))
    {
    CXM_ObjectData ObjData(0);
    Strng Tag, BlkName, RefTag;

    if (AW.BuildDataMessage(LOWORD(Item.lParam), Tag, RefTag, BlkName, ObjData))
      {
      return new AccDataBlk(Tag(), RefTag(), BlkName(), &ObjData);
      }
    }
  return NULL;
  }
*/
//---------------------------------------------------------------------------
/*
void CCopyTagBlkDlg::CopyToNode(AccDataBlk * pData) 
  {
//  TV_ITEM Item = { TVIF_HANDLE | TVIF_PARAM | TVIF_STATE };
//  Item.hItem=m_CopyTree.GetSelectedItem();
//  if (m_CopyTree.GetItem(&Item))
//    {
//    int xxx=0;
//    //m_CopyTree.SelectItem(Item.hItem);
////    CXM_ObjectTag ObjTag(AW.NdTag(), TABOpt_AllInfo|TABOpt_ForView);
//    CXM_ObjectData ObjData(0);
//    Strng RefTag;
//
//    //void AccNodeWnd::BuildDataMessage(long DataIndex, CXM_ObjectTag &ObjTag, CXM_ObjectData &ObjData)
//    if (pData)
//      {
//      int xxx=0;
//      //if (AW.BuildDataMessage(Item.lParam, RefTag, ObjData))
//      //  {
//      //  delete pData;
//      //  CXM_ObjectData * pData = (CXM_ObjectData*) new char[ObjData.Length()+100];
//      //  memcpy(pData, &ObjData, ObjData.Length());
//      pData->List.dbgDump(1, "DataXfer : To");
//
////        }
//      }
//    //AW.BuildDataMessage(Item.lParam);
//    }
  }
*/
//---------------------------------------------------------------------------

//AccDataBlk* pDefault=NULL;

void CCopyTagBlkDlg::OnTransfer() 
  {
  UpdateData(TRUE);
  const int SelCnt = OtherUnitsList.GetSelCount();
  const int LBlkSelIndex = LBlksList.GetCurSel();
  const int RBlkSelIndex = RBlksList.GetCurSel();
  if ( (iSrc==ACB_Selected) && 
       ( (iDst==ACB_Others && SelCnt>0) || 
         (iDst==ACB_Blocks && RBlkSelIndex>=0) ) &&
       (m_CopyTree.IsAnItemSelected(m_CopyTree.GetRootItem())) )
    {
    CString Txt;
    CSVector TreeList;
    const int TreeSelCnt = BuildTreeSelList(TreeList);
    if (TreeSelCnt>0)
      {
      CCopyBlock CB(sModelClass(), pEO);
      CB.SetSrcTag(sTag());
      ASSERT(iSrc==ACB_Selected);
      if (iDst==ACB_Others)
        {
        int* pSelItems = new int[SelCnt];
        CSVector DstList(SelCnt);
        OtherUnitsList.GetSelItems(SelCnt, pSelItems);
        for (int i=0; i<SelCnt; i++)
          {
          OtherUnitsList.GetText(pSelItems[i], Txt);
          DstList[i] = (const char*)Txt;
          }
        CB.SetDstList(DstList);
        delete []pSelItems;
        }
      else if (iDst==ACB_Blocks)
        {
        RBlksList.GetText(RBlkSelIndex, Txt);
        CB.SetDstBlock((char*)(const char*)Txt, RBlksList.GetCheck(RBlkSelIndex));
        }

      int Cnt = CB.WriteTagValues(TreeList);
      if (Cnt>0)
        {
        Cnt = CB.CopyTagBlocks();
        if (iDst==ACB_Others)
          LogNote("BlockCopy", 0, "%d tag%s copied", Cnt, Cnt>0 ? "s" : "");
        }

      }
    }
  else if ( (iSrc==ACB_Blocks) && 
            (LBlkSelIndex>=0) &&
            ( (iDst==ACB_Others && SelCnt>0) ||
              (iDst==ACB_Blocks && RBlkSelIndex>=0) ) )
    {
    CString LTxt;
    LBlksList.GetText(LBlkSelIndex, LTxt);
    CCopyBlock CB(sModelClass(), pEO);
    CB.SetSrcBlock((char*)(const char*)LTxt, LBlksList.GetCheck(LBlkSelIndex));
    ASSERT(iSrc==ACB_Blocks);
    BOOL Valid = TRUE;
    if (iDst==ACB_Blocks)
      {
      CString RTxt;
      RBlksList.GetText(RBlkSelIndex, RTxt);
      if (LTxt==RTxt)
        Valid = FALSE;
      else
        CB.SetDstBlock((char*)(const char*)RTxt, RBlksList.GetCheck(RBlkSelIndex));
      }
    else
      {
      CString Txt;
      int* pSelItems = new int[SelCnt];
      CSVector DstList(SelCnt);
      OtherUnitsList.GetSelItems(SelCnt, pSelItems);
      for (int i=0; i<SelCnt; i++)
        {
        OtherUnitsList.GetText(pSelItems[i], Txt);
        DstList[i] = (const char*)Txt;
        }
      CB.SetDstList(DstList);
      delete []pSelItems;
      }
    if (Valid)
      {
      int Cnt = CB.CopyTagBlocks();
      if (iDst==ACB_Others)
        LogNote("BlockCopy", 0, "%d tag%s copied", Cnt, Cnt>0 ? "s" : "");
      }
    }

  /*AccDataBlk* pSrc = NULL;
  switch (iSrc)
    {
    case ACB_Selected:
      pSrc = CopyFromNode();
      break;
    case ACB_Blocks:
      pSrc = pDefault;//CopyFromNode();
      break;

    }

  if (pSrc)
    {
    switch (iDst)
      {
      case ACB_Selected:
        CopyToNode(pSrc);
        break;
      case ACB_Blocks:
        pDefault=pSrc;//CopyFromNode();
        break;
      case ACB_Others:

        break;
      }
    }*/
  }

//---------------------------------------------------------------------------

void CCopyTagBlkDlg::OnUpdateTransfer(CCmdUI* pCmdUi)
  {
  const int SelCnt = OtherUnitsList.GetSelCount();
  const int LBlkSelIndex = LBlksList.GetCurSel();
  const int RBlkSelIndex = RBlksList.GetCurSel();
  BOOL Valid = (
    ( (iSrc==ACB_Selected) && 
      ( (iDst==ACB_Others && SelCnt>0) || 
        (iDst==ACB_Blocks && RBlkSelIndex>=0) ) &&
      (m_CopyTree.IsAnItemSelected(m_CopyTree.GetRootItem())) ) ||
    ( (iSrc==ACB_Blocks) && 
      (LBlkSelIndex>=0) &&
      ( (iDst==ACB_Others && SelCnt>0) ||
        (iDst==ACB_Blocks && RBlkSelIndex>=0) ) ) );
  if (Valid && iSrc==ACB_Blocks && iDst==ACB_Blocks)
    {
    CString s1,s2;
    LBlksList.GetText(LBlkSelIndex, s1);
    RBlksList.GetText(RBlkSelIndex, s2);
    if (s1==s2)
      Valid = FALSE;
    }
  pCmdUi->Enable(Valid);
  }

//---------------------------------------------------------------------------

