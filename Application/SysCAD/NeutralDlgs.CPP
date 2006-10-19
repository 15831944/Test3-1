//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
#include "syscad.h"
#include "project.h"
#include "mainfrm.h"
#include "..\schist\hstmain.h"
#include "accnode.h"
#include "tagvdoc.h"
#include "tagvdlgs.h"
#include "grfdoc.h"
#include "toolbars.h"
#include "helpinfo.h"
#include "gendlgs.h"
#include "cmd_mngr.h"
#include "scd_wm.h"
#include "dbgmngr.h"
#include "msgwnd.h"
#include "ordwnd.h"
#include "statswnd.h"
#include "wndslct.h"
#include "explorescd.h"
#include "scdver.h"
#include "scdcmdif.h"
#include "dbhelper.h"

extern "C" 
  {
  #include "grldefs.h"
  }

#include "afxwin.h"
#include ".\neutraldlgs.h"
#include ".\neutralgrf.h"
#include ".\neutralmdl.h"


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//===========================================================================
//
//
//
//===========================================================================

CNeutralExportDBDlg::CNeutralExportDBDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNeutralExportDBDlg::IDD, pParent)
  {
  }

//--------------------------------------------------------------------------

void CNeutralExportDBDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_GRFDATABASE, m_GrfDatabase);
  DDX_Control(pDX, IDC_MDLDATABASE, m_MdlDatabase);
  DDX_Control(pDX, IDC_SEPARATEDBS, m_SeparateDBs);
  DDX_Control(pDX, IDC_WITHCONFIG, m_WithConfig);
  DDX_Control(pDX, IDC_WITHGRAPHICS, m_WithGraphics);
  DDX_Control(pDX, IDC_WITHMODELS, m_WithModels);
  DDX_Control(pDX, IDC_WITHPARAMETERS, m_WithParameters);
  DDX_Control(pDX, IDC_WITHCREATETABLESONLY, m_CreateOnly);
  }

//--------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CNeutralExportDBDlg, CDialog)
  ON_BN_CLICKED(IDC_SEPARATEDBS, OnBnClickedSeparatedbs)
  ON_BN_CLICKED(IDC_WITHCONFIG, OnBnClickedWithconfig)
  ON_EN_CHANGE(IDC_GRFDATABASE, OnEnChangeGrfdatabase)
  ON_BN_CLICKED(IDC_BROWSENEUTRALPATH, OnBnClickedBrowseneutralpath)
END_MESSAGE_MAP()

//--------------------------------------------------------------------------

BOOL CNeutralExportDBDlg::OnInitDialog() 
  {
  CDialog::OnInitDialog();

  Strng S(PrjFiles());
  S+="NeutralDB.MDB";
  CProfINIFile PF(PrjIniFile());
  m_GrfDatabase.SetWindowText(PF.RdStr("NeutralImportExport", "GrfDatabase", S()));
  m_MdlDatabase.SetWindowText(PF.RdStr("NeutralImportExport", "MdlDatabase", S()));
  m_SeparateDBs.SetCheck(PF.RdLong("NeutralImportExport", "SeparateDBs", 0));
  m_WithModels.SetCheck(PF.RdLong("NeutralImportExport", "ExportModels", 1));
  m_WithGraphics.SetCheck(PF.RdLong("NeutralImportExport", "ExportGraphics", 1));
  m_WithConfig.SetCheck(PF.RdLong("NeutralImportExport", "ExportConfiguration", 1));
  m_WithParameters.SetCheck(PF.RdLong("NeutralImportExport", "ExportParameters", 0));

  BOOL Sep=m_SeparateDBs.GetCheck()!=0;
  m_MdlDatabase.EnableWindow(Sep?1:0);

  //UpdateData(FALSE);
  return TRUE;
  }

//--------------------------------------------------------------------------

void CNeutralExportDBDlg::OnOK() 
  {
  UpdateData(TRUE);

  m_GrfDatabase.GetWindowText(m_sGrfDatabase);
  m_MdlDatabase.GetWindowText(m_sMdlDatabase);
  m_bWithModels     = m_WithModels.GetCheck()!=0;
  m_bWithGraphics   = m_WithGraphics.GetCheck()!=0;
  m_bWithConfig     = m_WithConfig.GetCheck()!=0;
  m_bWithParameters = m_WithParameters.GetCheck()!=0;
  //m_bWithState      = m_WithState.GetCheck()!=0;
  m_bCreateOnly     = m_CreateOnly.GetCheck()!=0;

  CProfINIFile PF(PrjIniFile());
  PF.WrStr("NeutralImportExport", "GrfDatabase", m_sGrfDatabase);
  PF.WrStr("NeutralImportExport", "MdlDatabase", m_sMdlDatabase);
  PF.WrLong("NeutralImportExport", "SeparateDBs", m_SeparateDBs.GetCheck());
  PF.WrLong("NeutralImportExport", "ExportModels", m_WithModels.GetCheck());
  PF.WrLong("NeutralImportExport", "ExportGraphics", m_WithGraphics.GetCheck());
  PF.WrLong("NeutralImportExport", "ExportConfiguration", m_WithConfig.GetCheck());
  PF.WrLong("NeutralImportExport", "ExportParameters", m_WithParameters.GetCheck());  

  CDialog::OnOK();
  }

//--------------------------------------------------------------------------
//operator eScdNDBOptions (int i) { return i; }
//operator int (eScdNDBOptions i) { return i; }

eScdNDBOptions CNeutralExportDBDlg::Options()
  {
  int Opts=eScdNDBOptions(eNDB_CommonTables|eNDB_Sequence|eNDB_GroupInserts);
  if (m_bWithGraphics)  
    Opts |= eNDB_Graphics;
  if (m_bWithModels)
    {
    Opts |= eNDB_Models;
    if (m_bWithConfig)    
      Opts |= eNDB_Configuration;
    if (m_bWithParameters)
      Opts |= eNDB_Parameters;
    if (m_bWithState)
      Opts |= eNDB_State;
    }
  if (m_bCreateOnly)
    Opts |= eNDB_BlankTables;
  return eScdNDBOptions (Opts);
  }

//--------------------------------------------------------------------------


void CNeutralExportDBDlg::OnBnClickedSeparatedbs()
  {
  UpdateData(true);
  BOOL Sep=m_SeparateDBs.GetCheck()!=0;
  m_MdlDatabase.EnableWindow(Sep?1:0);
  if (Sep)
    {
    CString S;
    m_GrfDatabase.GetWindowText(S);
    m_MdlDatabase.SetWindowText(S);
    }
  UpdateData(false);
  }

void CNeutralExportDBDlg::OnBnClickedWithconfig()
  {
  }

void CNeutralExportDBDlg::OnEnChangeGrfdatabase()
  {
  if (m_SeparateDBs.GetCheck()==0)
    {
    CString S;
    m_GrfDatabase.GetWindowText(S);
    m_MdlDatabase.SetWindowText(S);
    }
  }

void CNeutralExportDBDlg::OnBnClickedBrowseneutralpath()
  {
  UpdateData(TRUE);
  Strng FName,FPath;
  CString S;
  m_GrfDatabase.GetWindowText(S);
  if (S.GetLength()>0)
    {
    FPath.FnDrivePath((char*)(const char*)S);
    FName.FnNameExt((char*)(const char*)S);
    }
  else
    {
    FName = "*.mdb";
    FPath = "..\\";//PrjPrevDirectory();
    }

  CSCDFileDialog Dlg(TRUE, NULL, FName(), OFN_HIDEREADONLY, "Access Database (*.mdb)|*.mdb||", this);
  Dlg.m_ofn.lpstrInitialDir = FPath();
  Dlg.m_ofn.lpstrTitle = "Browse";
  if (Dlg.DoModal()==IDOK)
    {
    m_GrfDatabase.SetWindowText(Dlg.GetPathName());
    //UpdateSlaveNumTags();
    //UpdateSlaveNumTagsOK();
    }

  UpdateData(FALSE);
  }

//===========================================================================
//
//
//
//===========================================================================

CNeutralImportDBDlg::CNeutralImportDBDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNeutralImportDBDlg::IDD, pParent)
  {
  //{{AFX_DATA_INIT(CNeutralImportDBDlg)
  //}}AFX_DATA_INIT
  }

//--------------------------------------------------------------------------

void CNeutralImportDBDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_GRFDATABASE,   m_GrfDatabase);
  DDX_Control(pDX, IDC_MDLDATABASE,   m_MdlDatabase);
  DDX_Control(pDX, IDC_SEPARATEDBS,   m_SeparateDBs);
  DDX_Control(pDX, IDC_WITHMODELS,    m_WithModels);
  DDX_Control(pDX, IDC_WITHGRAPHICS,  m_WithGraphics);
  DDX_Control(pDX, IDC_WITHCONFIG, m_WithConfig);
  }

//--------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CNeutralImportDBDlg, CDialog)
  ON_BN_CLICKED(IDC_SEPARATEDBS, OnBnClickedSeparatedbs)
  ON_EN_CHANGE(IDC_GRFDATABASE, OnEnChangeGrfdatabase)
  ON_BN_CLICKED(IDC_BROWSENEUTRALPATH, OnBnClickedBrowseneutralpath)
END_MESSAGE_MAP()

//--------------------------------------------------------------------------

BOOL CNeutralImportDBDlg::OnInitDialog() 
  {
  CDialog::OnInitDialog();

  Strng S(PrjFiles());
  S+="NeutralDB.MDB";
  CProfINIFile PF(PrjIniFile());
  m_GrfDatabase.SetWindowText(PF.RdStr("NeutralImportExport", "GrfDatabase", S()));
  m_MdlDatabase.SetWindowText(PF.RdStr("NeutralImportExport", "MdlDatabase", S()));
  m_SeparateDBs.SetCheck(PF.RdLong("NeutralImportExport", "SeparateDBs", 0));
  m_WithModels.SetCheck(PF.RdLong("NeutralImportExport", "ImportModels", 1));
  m_WithGraphics.SetCheck(PF.RdLong("NeutralImportExport", "ImportGraphics", 1));
  m_WithConfig.SetCheck(PF.RdLong("NeutralImportExport", "ImportConfiguration", 1));

  BOOL Sep=m_SeparateDBs.GetCheck()!=0;
  m_MdlDatabase.EnableWindow(Sep?1:0);

  //UpdateData(FALSE);
  return TRUE;
  }

//--------------------------------------------------------------------------

void CNeutralImportDBDlg::OnOK() 
  {
  UpdateData(TRUE);

  m_GrfDatabase.GetWindowText(m_sGrfDatabase);
  m_MdlDatabase.GetWindowText(m_sMdlDatabase);
  m_bWithModels     = m_WithModels.GetCheck()!=0;
  m_bWithGraphics   = m_WithGraphics.GetCheck()!=0;
  m_bWithConfig     = m_WithConfig.GetCheck()!=0;

  CProfINIFile PF(PrjIniFile());
  PF.WrStr("NeutralImportExport", "GrfDatabase", m_sGrfDatabase);
  PF.WrStr("NeutralImportExport", "MdlDatabase", m_sMdlDatabase);
  PF.WrLong("NeutralImportExport", "SeparateDBs", m_SeparateDBs.GetCheck());
  PF.WrLong("NeutralImportExport", "ImportModels", m_WithModels.GetCheck());
  PF.WrLong("NeutralImportExport", "ImportGraphics", m_WithGraphics.GetCheck());
  PF.WrLong("NeutralImportExport", "ImportConfiguration", m_WithConfig.GetCheck());

  CDialog::OnOK();
  }

//--------------------------------------------------------------------------

eScdNDBOptions CNeutralImportDBDlg::Options()
  {
  int Opts=eScdNDBOptions(eNDB_CommonTables|eNDB_Sequence|eNDB_GroupInserts);
  if (m_bWithGraphics)  
    Opts |= eNDB_Graphics;
  if (m_bWithModels)
    {
    Opts |= eNDB_Models;
    if (m_bWithConfig)    
      Opts |= eNDB_Configuration;
    //if (m_bWithParameters)
    //  Opts |= eNDB_Parameters;
    }
  //if (m_bCreateOnly)
  //  Opts |= eNDB_BlankTables;
  return eScdNDBOptions (Opts);
  }

//--------------------------------------------------------------------------

void CNeutralImportDBDlg::OnBnClickedSeparatedbs()
  {
  UpdateData(true);
  BOOL Sep=m_SeparateDBs.GetCheck()!=0;
  m_MdlDatabase.EnableWindow(Sep?1:0);
  if (Sep)
    {
    CString S;
    m_GrfDatabase.GetWindowText(S);
    m_MdlDatabase.SetWindowText(S);
    }
  UpdateData(false);
  }

void CNeutralImportDBDlg::OnEnChangeGrfdatabase()
  {
  if (m_SeparateDBs.GetCheck()==0)
    {
    CString S;
    m_GrfDatabase.GetWindowText(S);
    m_MdlDatabase.SetWindowText(S);
    }
  }

void CNeutralImportDBDlg::OnBnClickedBrowseneutralpath()
  {
  UpdateData(TRUE);
  Strng FName,FPath;
  CString S;
  m_GrfDatabase.GetWindowText(S);
  if (S.GetLength()>0)
    {
    FPath.FnDrivePath((char*)(const char*)S);
    FName.FnNameExt((char*)(const char*)S);
    }
  else
    {
    FName = "*.mdb";
    FPath = "..\\";//PrjPrevDirectory();
    }

  CSCDFileDialog Dlg(TRUE, NULL, FName(), OFN_HIDEREADONLY, "Access Database (*.mdb)|*.mdb||", this);
  Dlg.m_ofn.lpstrInitialDir = FPath();
  Dlg.m_ofn.lpstrTitle = "Browse";
  if (Dlg.DoModal()==IDOK)
    {
    m_GrfDatabase.SetWindowText(Dlg.GetPathName());
    //UpdateSlaveNumTags();
    //UpdateSlaveNumTagsOK();
    }

  UpdateData(FALSE);
  }

//===========================================================================
//
//
//
//===========================================================================

CNeutralSaveGroupDlg::CNeutralSaveGroupDlg(Grf3D_Display * pDsp, CWnd* pParent /*=NULL*/)
	: CDialog(CNeutralSaveGroupDlg::IDD, pParent)
  {
  m_pDsp=pDsp;
  m_bAllOK=false;
  m_iUpdateBusy=0;
  m_Initialised=false;
  }

//--------------------------------------------------------------------------

void CNeutralSaveGroupDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_DATABASE, m_editDatabase);
  DDX_Control(pDX, IDC_WITHCONFIG, m_WithConfig);
  DDX_Control(pDX, IDC_GROUPLIST, m_GroupList);
  DDX_Control(pDX, IDC_GRPDESC, m_GroupDesc);
  DDX_Control(pDX, IDC_LMARGIN, m_LMargin);
  DDX_Control(pDX, IDC_RMARGIN, m_RMargin);
  DDX_Control(pDX, IDC_TMARGIN, m_TMargin);
  DDX_Control(pDX, IDC_BMARGIN, m_BMargin);
  DDX_Control(pDX, IDC_XORGOPTS, m_XOrgOpts);
  DDX_Control(pDX, IDC_YORGOPTS, m_YOrgOpts);
  DDX_Control(pDX, IDC_SYMBOLFRAME, m_SymbolFrame);
  DDX_Control(pDX, IDC_SIZEINFO, m_SizeInfo);
  DDX_Control(pDX, IDC_XORIGIN, m_XOrigin);
  DDX_Control(pDX, IDC_YORIGIN, m_YOrigin);
  DDX_Control(pDX, IDC_TAGLIST, m_TagList);
  DDX_Control(pDX, IDC_TAGFIND, m_String1);
  DDX_Control(pDX, IDC_TAGREPLACE, m_String2);
  DDX_Control(pDX, IDC_MESSAGE, m_Message);
  DDX_Control(pDX, IDOK, m_OK);
  DDX_Control(pDX, IDC_DELETEGRP, m_DeleteGrp);
  DDX_Control(pDX, IDC_WITHSTATE, m_WithState);
  }

//--------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CNeutralSaveGroupDlg, CDialog)
  ON_BN_CLICKED(IDC_WITHCONFIG, OnBnClickedWithconfig)
  ON_EN_CHANGE(IDC_DATABASE, OnEnChangeDatabase)
  ON_BN_CLICKED(IDC_BROWSENEUTRALPATH, OnBnClickedBrowsedatabase)
  ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
  ON_EN_CHANGE(IDC_LMARGIN, OnEnChangeLmargin)
  ON_EN_CHANGE(IDC_RMARGIN, OnEnChangeRmargin)
  ON_EN_CHANGE(IDC_TMARGIN, OnEnChangeTmargin)
  ON_EN_CHANGE(IDC_BMARGIN, OnEnChangeBmargin)
  ON_CBN_SELCHANGE(IDC_XORGOPTS, OnCbnSelchangeXorgopts)
  ON_CBN_SELCHANGE(IDC_YORGOPTS, OnCbnSelchangeYorgopts)
  ON_WM_PAINT()
  ON_EN_CHANGE(IDC_XORIGIN, OnEnChangeXorigin)
  ON_EN_CHANGE(IDC_YORIGIN, OnEnChangeYorigin)
  ON_BN_CLICKED(IDC_FINDOPTIONS, OnBnClickedFindoptions)
  ON_BN_CLICKED(IDC_REPLACEOPTIONS, OnBnClickedReplaceoptions)
  ON_BN_CLICKED(IDC_DOREPLACE, OnBnClickedDoreplace)
  ON_BN_CLICKED(IDC_RESETTAGS, OnBnClickedResettags)
  ON_BN_CLICKED(IDOK, OnBnClickedOk)
  ON_CBN_EDITUPDATE(IDC_GROUPLIST, OnCbnEditupdateGrouplist)
  ON_EN_CHANGE(IDC_GRPDESC, OnEnChangeGrpdesc)
  ON_NOTIFY(NM_DBLCLK, IDC_TAGLIST, OnNMDblclkTaglist)
  ON_NOTIFY(LVN_BEGINLABELEDIT, IDC_TAGLIST, OnLvnBeginlabeleditTaglist)
  ON_NOTIFY(LVN_ENDLABELEDIT, IDC_TAGLIST, OnLvnEndlabeleditTaglist)
  ON_NOTIFY(LVN_ITEMCHANGED, IDC_TAGLIST, OnLvnItemchangedTaglist)
  ON_CBN_SELCHANGE(IDC_GROUPLIST, OnCbnSelchangeGrouplist)
  ON_BN_CLICKED(IDC_DELETEGRP, OnBnClickedDeletegrp)
  ON_BN_CLICKED(IDC_WITHSTATE, OnBnClickedWithstate)
END_MESSAGE_MAP()

//--------------------------------------------------------------------------

inline CString GetWndText(CEdit & W)
  {
  CString S;
  W.GetWindowText(S);
  return S; 
  }

void CNeutralSaveGroupDlg::SetDBNameFull(LPCTSTR Fn)
  {
  m_sDatabaseSymb=Fn;
  m_sDatabaseSymb.FnContract();
  m_sDatabaseFull=Fn;
  }
void CNeutralSaveGroupDlg::SetDBNameSymb(LPCTSTR Fn)
  {
  m_sDatabaseSymb=Fn;
  Strng X;
  X.FnExpand((LPTSTR)Fn);
  m_sDatabaseFull=X();
  }

CString CNeutralSaveGroupDlg::Value2Text(double M)
  {
  CString S;
  S.Format("%.1f", M);
  return S;
  }
double CNeutralSaveGroupDlg::Text2Value(LPCTSTR S, double Mn, double Mx)
  {
  return Range(Mn, SafeAtoF(S, 5), Mx); 
  }
double CNeutralSaveGroupDlg::Text2Value(CEdit & E, double Mn, double Mx)
  {
  return Range(Mn, SafeAtoF(GetWndText(E), 5), Mx); 
  }


//--------------------------------------------------------------------------

static LPCTSTR CDlgC0="Model Tag";
static LPCTSTR CDlgC1="Tag In Group";


BOOL CNeutralSaveGroupDlg::OnInitDialog() 
  {
  CDialog::OnInitDialog();
  CProfINIFile PF(PrjIniFile());

  SetDBNameSymb(PF.RdStr("NeutralGroup", "Database", "$CfgFiles\\GroupLibrary.MDB"));
  m_editDatabase.SetWindowText(m_sDatabaseSymb());

#if WITHGRFGROUPCONFIG
  if (CFieldMonitor::m_On)
#else
  if (0)
#endif
    {
    m_WithConfig.SetCheck(PF.RdLong("CreateGroup", "WithConfig", 1));
    m_WithState.SetCheck(PF.RdLong("CreateGroup", "WithState", 0));
    }
  else
    {
    m_WithConfig.SetCheck(0);
    m_WithState.SetCheck(0);
    m_WithConfig.EnableWindow(false);
    m_WithState.EnableWindow(false);
    }
    
  m_XOrgOpts.AddString("As Digitised");
  m_XOrgOpts.AddString("Left");
  m_XOrgOpts.AddString("Centre");
  m_XOrgOpts.AddString("Right");
  m_XOrgOpts.AddString("Align Left");
  m_XOrgOpts.AddString("Align Centre");
  m_XOrgOpts.AddString("Align Right");
  m_XOrgOpts.AddString("Manual");

  m_YOrgOpts.AddString("As Digitised");
  m_YOrgOpts.AddString("Top");
  m_YOrgOpts.AddString("Centre");
  m_YOrgOpts.AddString("Bottom");
  m_YOrgOpts.AddString("Align Top");
  m_YOrgOpts.AddString("Align Centre");
  m_YOrgOpts.AddString("Align Bottom");
  m_YOrgOpts.AddString("Manual");

  m_XOrgOpts.SelectString(0, PF.RdStr("CreateGroup", "XOrigin", "As Digitised"));
  m_YOrgOpts.SelectString(0, PF.RdStr("CreateGroup", "YOrigin", "As Digitised"));

  m_LMargin.SetWindowText(Value2Text(PF.RdDouble("CreateGroup", "LeftMargin",   5.0)));
  m_RMargin.SetWindowText(Value2Text(PF.RdDouble("CreateGroup", "RightMargin",  5.0)));
  m_TMargin.SetWindowText(Value2Text(PF.RdDouble("CreateGroup", "TopMargin",    5.0)));
  m_BMargin.SetWindowText(Value2Text(PF.RdDouble("CreateGroup", "BottomMargin", 5.0)));
  
  m_OrigMan.m_X=PF.RdDouble("CreateGroup", "ManualOriginX", 0.0);
  m_OrigMan.m_Y=PF.RdDouble("CreateGroup", "ManualOriginY", 0.0);

  m_GroupDesc.SetWindowText("<A Description>");

  m_SymbolFrame.GetClientRect(m_SymRectClient);

  m_TagList.InsertColumn(0, CDlgC0, LVCFMT_LEFT, 50);
  m_TagList.InsertColumn(1, CDlgC1, LVCFMT_LEFT, 50);
  
  int RTags=m_OldTags.GetCount();
  int NTags=m_NewTags.GetCount();
  ASSERT(NTags==RTags);

  Strng TPath;
  for (int i=0; i<RTags; i++)
    {
    int Pos=m_TagList.InsertItem(i, m_OldTags[i]);
    m_TagList.SetItemText(Pos, 1, m_NewTags[i]);
    m_TagList.SetItemData(Pos, i);
    }

  int W=m_TagList.GetStringWidth("XXXXXXXXXXXXX");
  int Col0Len=ScdPFUser.RdInt("CreateGroup", "Column0", -1);
  int Col1Len=ScdPFUser.RdInt("CreateGroup", "Column1", -1);

  if (Col0Len<0)
    {
    Col0Len=W;
    Col1Len=W;
    for (i=0; i<RTags; i++)
      {
      Col0Len=Max(Col0Len, m_TagList.GetStringWidth((const char*)(m_OldTags[i]))+20);
      Col1Len=Max(Col1Len, m_TagList.GetStringWidth((const char*)(m_NewTags[i]))+20);
      }
    }

  m_TagList.SetColumnWidth(0, Col0Len);
  m_TagList.SetColumnWidth(1, Col1Len);

  m_String1.SetWindowText(PF.RdStr("CreateGroup", "FindString", "^"));
  m_String2.SetWindowText(PF.RdStr("CreateGroup", "ReplaceString", "<ID>"));
    
  CString S;
  S.Format("Size: %.1f * %.1f", m_Hi.m_X-m_Lo.m_X, m_Hi.m_Y-m_Lo.m_Y);
  m_SizeInfo.SetWindowText(S);

  m_Initialised=true;

  DoChecks();

  return TRUE;
  }

//--------------------------------------------------------------------------

void CNeutralSaveGroupDlg::OnOK() 
  {
  UpdateData(TRUE);

  CProfINIFile PF(PrjIniFile());

  SetDBNameSymb(GetWndText(m_editDatabase));

  PF.WrStr("NeutralGroup", "Database", m_sDatabaseSymb());
  #if WITHGRFGROUPCONFIG
  PF.WrLong("CreateGroup", "WithConfig", m_WithConfig.GetCheck());
  PF.WrLong("CreateGroup", "WithState", m_WithState.GetCheck());
  #endif

  CString Sx, Sy;
  m_XOrgOpts.GetWindowText(Sx);
  m_YOrgOpts.GetWindowText(Sy);
  PF.WrStr("CreateGroup", "XOrigin", Sx);
  PF.WrStr("CreateGroup", "YOrigin", Sy);

  PF.WrDouble("CreateGroup", "LeftMargin",   Text2Value(m_LMargin));
  PF.WrDouble("CreateGroup", "RightMargin",  Text2Value(m_RMargin));
  PF.WrDouble("CreateGroup", "TopMargin",    Text2Value(m_TMargin));
  PF.WrDouble("CreateGroup", "BottomMargin", Text2Value(m_BMargin));

  PF.WrDouble("CreateGroup", "ManualOriginX", m_OrigMan.m_X);
  PF.WrDouble("CreateGroup", "ManualOriginY", m_OrigMan.m_Y);

  //PF.WrInt("CreateGroup", "FixupRule", GetFixupRule());
  //PF.WrStr("CreateGroup", "Prefix", GetPrefix());
  //PF.WrStr("CreateGroup", "Suffix", GetSuffix());
  CString FindStr, ReplaceStr;
  m_String1.GetWindowText(FindStr);
  m_String2.GetWindowText(ReplaceStr);
  PF.WrStr("CreateGroup", "FindString", FindStr);
  PF.WrStr("CreateGroup", "ReplaceString", ReplaceStr);

  CDialog::OnOK();
  }

//--------------------------------------------------------------------------

void CNeutralSaveGroupDlg::OnBnClickedCancel()
  {
  // TODO: Add your control notification handler code here
  m_bAllOK=false;
  OnCancel();
  }

//--------------------------------------------------------------------------

void CNeutralSaveGroupDlg::OnBnClickedWithconfig()
  {
  if (m_WithConfig.GetCheck()==0)
    m_WithState.SetCheck(0);
  }

void CNeutralSaveGroupDlg::OnBnClickedWithstate()
  {
  if (m_WithState.GetCheck()!=0)
    m_WithConfig.SetCheck(1);
  }

void CNeutralSaveGroupDlg::OnEnChangeDatabase()
  {
  CString S;
  m_editDatabase.GetWindowText(S);
  SetDBNameSymb(S);
  UpdateGroupList();
  }

void CNeutralSaveGroupDlg::OnBnClickedBrowsedatabase()
  {
  UpdateData(TRUE);
  Strng FName,FPath;
  CString S;
  m_editDatabase.GetWindowText(S);
  SetDBNameSymb(S);
  if (m_sDatabaseFull.GetLength()>0)
    {
    FPath.FnDrivePath((char*)(const char*)m_sDatabaseFull);
    FName.FnNameExt((char*)(const char*)m_sDatabaseFull);
    }
  else
    {
    FName = "*.mdb";
    FPath = "..\\";//PrjPrevDirectory();
    }

  CSCDFileDialog Dlg(TRUE, NULL, m_sDatabaseFull, OFN_HIDEREADONLY, "Access Database (*.mdb)|*.mdb||", this);
  Dlg.m_ofn.lpstrInitialDir = FPath();
  Dlg.m_ofn.lpstrTitle = "Browse";
  if (Dlg.DoModal()==IDOK)
    {
    SetDBNameFull(Dlg.GetPathName());
    m_editDatabase.SetWindowText(m_sDatabaseSymb());
    //UpdateSlaveNumTags();
    //UpdateSlaveNumTagsOK();
    }

  UpdateData(FALSE);
  }

void CNeutralSaveGroupDlg::AddTag(LPCTSTR Tag) 
  { 
  int n=m_OldTags.GetSize();
  for (int i=0; i<n && (m_OldTags[i].CompareNoCase(Tag)<=0); i++) {} ;
  if (i<n)
    {
    m_OldTags.InsertAt(i, Tag); 
    m_NewTags.InsertAt(i, Tag); 
    }
  else
    {
    m_OldTags.Add(Tag); 
    m_NewTags.Add(Tag); 
    }
  };

void CNeutralSaveGroupDlg::UpdateGroupList()
  {
  m_GroupList.ResetContent();

  if (FileExists((LPTSTR)(LPCTSTR)m_sDatabaseFull))
    {
    int nGroups = 0;
    HTREEITEM hSelected=NULL, hFirst=NULL;
    CDocTemplate & GT = ScdApp()->GraphTemplate();
    CNeutralImportExport EGH;
    CNeutralGrpDescList List;
    EGH.SetGroup("*");
    if (EGH.GetGroups(List, m_sDatabaseFull)>0)
      {
      POSITION Pos=List.GetHeadPosition();
      while (Pos)
        {
        CNeutralGrpDescItem &S=List.GetNext(Pos);
        m_GroupList.AddString(S.m_sGroup);
        nGroups++;
        }
      }
    }
  else
    { 
    //???
    }
  };

bool CNeutralSaveGroupDlg::FindGroup(LPCTSTR Name)
  {
  bool Found=false;
  if (FileExists((LPTSTR)(LPCTSTR)m_sDatabaseFull))
    {
    CNeutralImportExport EGH;
    CNeutralGrpDescList List;
    EGH.SetGroup("*");
    if (EGH.GetGroups(List, m_sDatabaseFull)>0)
      {
      POSITION Pos=List.GetHeadPosition();
      while (Pos)
        {
        CNeutralGrpDescItem &S=List.GetNext(Pos);
        if (S.m_sGroup.CompareNoCase(Name)==0)
          {
          Found=true;
          break;
          }
        }
      }
    }
  else
    { 
    //???
    }
  return Found;
  };

void CNeutralSaveGroupDlg::DeleteGroup(LPCTSTR Name)
  {
  if (FindGroup(Name))
    {
    CNeutralImportExport EGH;
    if (!EGH.DeleteGroup(Name, m_sDatabaseFull))
      {
      AfxMessageBox("Delete Failed - Not Yet Implemented");
      }
    UpdateGroupList();
    }
  }

CString CNeutralSaveGroupDlg::GetDatabaseName()
  {
  return m_sDatabaseFull;
  };
CString CNeutralSaveGroupDlg::GetGroup()
  {
  return m_sGroupName;
  };
bool CNeutralSaveGroupDlg::IsGroupValid()
  {
  return m_sGroupName.GetLength()>0;
  };

//---------------------------------------------------------------------------

void CNeutralSaveGroupDlg::OnPaint() 
  {
  CPaintDC dc(this); // device context for painting
  CDCResChk ResChk(dc);

  bool Done=false;
  if (1)
    {
    CPaintDC dc(&m_SymbolFrame); // device context for painting
    CDCResChk ResChk(dc);

    double MaxX=Max(m_OrigMan.m_X+m_Lo.m_X, Max(m_Origin.m_X,m_AlignHi.m_X));
    double MinX=Min(m_OrigMan.m_X+m_Lo.m_X, Min(m_Origin.m_X,m_AlignLo.m_X));
    double MaxY=Max(m_OrigMan.m_Y+m_Lo.m_Y, Max(m_Origin.m_Y,m_AlignHi.m_Y));
    double MinY=Min(m_OrigMan.m_Y+m_Lo.m_Y, Min(m_Origin.m_Y,m_AlignLo.m_Y));

    double XScl=m_SymRectClient.Width()/GTZ(MaxX-MinX);
    double YScl=m_SymRectClient.Height()/GTZ(MaxY-MinY);
    double Scl=Min(XScl, YScl)*0.9;

    int PixXMid=m_SymRectClient.Width()/2;
    double XMid=0.5*(m_AlignHi.m_X+m_AlignLo.m_X);
    int PixYMid=m_SymRectClient.Height()/2;
    double YMid=0.5*(m_AlignHi.m_Y+m_AlignLo.m_Y);

    #define XSCL(X) int(PixXMid+(X-XMid)*Scl)
    #define YSCL(Y) int(PixXMid-(Y-YMid)*Scl)
    
    dc.FillRect(&m_SymRectClient, &CBrush(RGB(0,0,0)));
    //  //CGdiObject * p=dc.SelectStockObject(WHITE_PEN);
    // The Symbol Bound Box
    CPen PenS(PS_SOLID, 1, RGB(192,192,192));
    CGdiObject * p=dc.SelectObject(&PenS);
    dc.MoveTo(XSCL(m_Lo.m_X), YSCL(m_Lo.m_Y));
    dc.LineTo(XSCL(m_Lo.m_X), YSCL(m_Hi.m_Y));
    dc.LineTo(XSCL(m_Hi.m_X), YSCL(m_Hi.m_Y));
    dc.LineTo(XSCL(m_Hi.m_X), YSCL(m_Lo.m_Y));
    dc.LineTo(XSCL(m_Lo.m_X), YSCL(m_Lo.m_Y));

    // Align Bound Box
    CPen PenD(PS_DOT, 1, RGB(192,192,192));
    dc.SelectObject(&PenD);
    dc.MoveTo(XSCL(m_AlignLo.m_X), YSCL(m_AlignLo.m_Y));
    dc.LineTo(XSCL(m_AlignLo.m_X), YSCL(m_AlignHi.m_Y));
    dc.LineTo(XSCL(m_AlignHi.m_X), YSCL(m_AlignHi.m_Y));
    dc.LineTo(XSCL(m_AlignHi.m_X), YSCL(m_AlignLo.m_Y));
    dc.LineTo(XSCL(m_AlignLo.m_X), YSCL(m_AlignLo.m_Y));

    // The Origin
    CPen PenDS(PS_SOLID, 1, RGB(0, 255, 0));
    dc.SelectObject(&PenDS);
    dc.MoveTo(0,                        YSCL(m_Origin.m_Y));
    dc.LineTo(m_SymRectClient.Width(),  YSCL(m_Origin.m_Y));
    dc.MoveTo(XSCL(m_Origin.m_X),       0);
    dc.LineTo(XSCL(m_Origin.m_X),       m_SymRectClient.Height());

    // The Dig Origin
    int CrossSize1=3;
    CPen PenDO(PS_SOLID, 1, RGB(255, 0, 0));
    dc.SelectObject(&PenDO);
    dc.MoveTo(XSCL(m_OrigDig.m_X-CrossSize1), YSCL(m_OrigDig.m_Y-CrossSize1));
    dc.LineTo(XSCL(m_OrigDig.m_X+CrossSize1), YSCL(m_OrigDig.m_Y+CrossSize1));
    dc.MoveTo(XSCL(m_OrigDig.m_X+CrossSize1), YSCL(m_OrigDig.m_Y-CrossSize1));
    dc.LineTo(XSCL(m_OrigDig.m_X-CrossSize1), YSCL(m_OrigDig.m_Y+CrossSize1));

    // The Man Origin
    int CrossSize2=5;
    CPen PenDM(PS_SOLID, 1, RGB(0, 0, 255));
    dc.SelectObject(&PenDM);
    dc.MoveTo(XSCL(m_OrigMan.m_X+m_Lo.m_X-CrossSize2), YSCL(m_OrigMan.m_Y+m_Lo.m_Y));
    dc.LineTo(XSCL(m_OrigMan.m_X+m_Lo.m_X+CrossSize2), YSCL(m_OrigMan.m_Y+m_Lo.m_Y));
    dc.MoveTo(XSCL(m_OrigMan.m_X+m_Lo.m_X),            YSCL(m_OrigMan.m_Y+m_Lo.m_Y-CrossSize2));
    dc.LineTo(XSCL(m_OrigMan.m_X+m_Lo.m_X),            YSCL(m_OrigMan.m_Y+m_Lo.m_Y+CrossSize2));

    dc.SelectObject(p);
    }
  else
    {
    CPaintDC dc(&m_SymbolFrame); // device context for painting
    CDCResChk ResChk(dc);
    dc.FillRect(&m_SymRectClient, &CBrush(RGB(0,0,0)));
    }
  // Do not call CDialog::OnPaint() for painting messages
  }

void CNeutralSaveGroupDlg::UpdateCalcdPts(bool UpdateOrgText)
  {
  if (!m_iUpdateBusy)
    {
    m_iUpdateBusy++;
    m_AlignLo.m_X = m_Lo.m_X-Text2Value(m_LMargin);
    m_AlignLo.m_Y = m_Lo.m_Y-Text2Value(m_BMargin);
    m_AlignHi.m_X = m_Hi.m_X+Text2Value(m_RMargin);
    m_AlignHi.m_Y = m_Hi.m_Y+Text2Value(m_TMargin);

    switch (m_XOrgOpts.GetCurSel())
      {
      case 0: m_Origin.m_X = m_OrigDig.m_X;                     break;  //  As Digitised
      case 1: m_Origin.m_X = m_Lo.m_X;                          break;  //  Left
      case 2: m_Origin.m_X = 0.5*(m_Hi.m_X+m_Lo.m_X);           break;  //  Centre
      case 3: m_Origin.m_X = m_Hi.m_X;                          break;  //  Right
      case 4: m_Origin.m_X = m_AlignLo.m_X;                     break;  //  Align Left
      case 5: m_Origin.m_X = 0.5*(m_AlignHi.m_X+m_AlignLo.m_X); break;  //  Align Centre
      case 6: m_Origin.m_X = m_AlignHi.m_X;                     break;  //  Align Right
      case 7: m_Origin.m_X = m_OrigMan.m_X+m_Lo.m_X;            break;  //  Manual
      }
   
    switch (m_YOrgOpts.GetCurSel())
      {
      case 0: m_Origin.m_Y = m_OrigDig.m_Y;                     break;  //  As Digitised
      case 1: m_Origin.m_Y = m_Hi.m_Y;                          break;  //  Top
      case 2: m_Origin.m_Y = 0.5*(m_Hi.m_Y+m_Lo.m_Y);           break;  //  Centre
      case 3: m_Origin.m_Y = m_Lo.m_Y;                          break;  //  Bottom
      case 4: m_Origin.m_Y = m_AlignHi.m_Y;                     break;  //  Align Top 
      case 5: m_Origin.m_Y = 0.5*(m_AlignHi.m_Y+m_AlignLo.m_Y); break;  //  Align Centre
      case 6: m_Origin.m_Y = m_AlignLo.m_Y;                     break;  //  Align Bottom 
      case 7: m_Origin.m_Y = m_OrigMan.m_Y+m_Lo.m_Y;            break;  //  Manual
      }

    if (1 || UpdateOrgText)
      {
      CString Sw,Sv;
      int S,E;
      m_XOrigin.GetSel(S,E);
      m_XOrigin.GetWindowText(Sw);
      Sv=Value2Text(m_Origin.m_X-m_Lo.m_X);
      if (Sv.Compare(Sw)!=0)
        m_XOrigin.SetWindowText(Sv);
      m_XOrigin.SetSel(S,E);

      m_YOrigin.GetSel(S,E);
      m_YOrigin.GetWindowText(Sw);
      Sv=Value2Text(m_Origin.m_Y-m_Lo.m_Y);
      if (Sv.Compare(Sw)!=0)
        m_YOrigin.SetWindowText(Sv);
      m_YOrigin.SetSel(S,E);
      }

    Pt_3f Pt(m_Origin.m_X, m_Origin.m_Y, m_Origin.m_Z);
    Pt_SLW PtW;
    m_pDsp->SetCPtWorld(Pt, PtW);
    m_pDsp->Move_Dig_CrossHair(PtW);

    m_iUpdateBusy--;
    }
  }

void CNeutralSaveGroupDlg::ReDrawSymbols()
  {
  InvalidateRect(m_SymRectClient, TRUE); 
  m_SymbolFrame.Invalidate();
  }

void CNeutralSaveGroupDlg::OnEnChangeLmargin()        { UpdateCalcdPts(true); ReDrawSymbols(); }
void CNeutralSaveGroupDlg::OnEnChangeRmargin()        { UpdateCalcdPts(true); ReDrawSymbols(); }
void CNeutralSaveGroupDlg::OnEnChangeTmargin()        { UpdateCalcdPts(true); ReDrawSymbols(); }
void CNeutralSaveGroupDlg::OnEnChangeBmargin()        { UpdateCalcdPts(true); ReDrawSymbols(); }
void CNeutralSaveGroupDlg::OnCbnSelchangeXorgopts()   { UpdateCalcdPts(true); ReDrawSymbols(); }
void CNeutralSaveGroupDlg::OnCbnSelchangeYorgopts()   { UpdateCalcdPts(true); ReDrawSymbols(); }
void CNeutralSaveGroupDlg::OnEnChangeXorigin()        
  {
  bool UpdateTxt=false;
  if (!m_iUpdateBusy)
    {
    m_XOrgOpts.SetCurSel(7);
    double x=Text2Value(m_XOrigin, -1001.0, 1001.0); 
    UpdateTxt=fabs(x)>=1000.0;
    m_OrigMan.m_X = Range(-1000.0, x, 1000.0); 
    }
  UpdateCalcdPts(UpdateTxt); 
  ReDrawSymbols(); 
  }
void CNeutralSaveGroupDlg::OnEnChangeYorigin()
  { 
  bool UpdateTxt=false;
  if (!m_iUpdateBusy)
    {
    m_YOrgOpts.SetCurSel(7);
    double y=Text2Value(m_YOrigin, -1001.0, 1001.0); 
    UpdateTxt=fabs(y)>=1000.0;
    m_OrigMan.m_Y = Range(-1000.0, y, 1000.0); 
    }
  UpdateCalcdPts(UpdateTxt); 
  ReDrawSymbols(); 
  }

void CNeutralSaveGroupDlg::DoChecks()
  {
  if (m_Initialised)
    {
    m_Message.SetWindowText("Ready");
    m_bAllOK=true;

    if (m_bAllOK)
      {
      CNeutralImportExport NExport;
      NExport.SetGroup(GetGroup());
      NExport.SetGroupDesc(m_sGroupDesc);
      //NExport.SetExportTagList(&Dlg.m_OldTags, &Dlg.m_NewTags);
      NExport.SetExportCoOrds(m_Origin, m_Lo, m_Hi, m_AlignLo, m_AlignHi);
      NExport.SetExportTagList(&m_OldTags, &m_NewTags);
      CString ErrMsg;
      if (!NExport.DoExportValidate(eNDB_StdExport, ErrMsg))
        {
        m_Message.SetWindowText(ErrMsg);//"Invalid Group Selection (Missing Link End Points)");
        m_bAllOK=false;
        }
      }

    if (m_bAllOK)
      {
      // Check Unique
      int NTags=m_NewTags.GetCount();
      for (int i=0; i<NTags && m_bAllOK; i++)
        {
        for (int j=0; j<NTags && m_bAllOK; j++)
          {
          if ((i!=j) && (m_NewTags[i].CompareNoCase(m_NewTags[j])==0))
            {
            CString S;
            S.Format(" Tag %s is not unique", m_NewTags[j]);
            m_Message.SetWindowText(S);
            m_bAllOK=false;
            }
          }
        }
      }

    m_GroupList.GetWindowText(m_sGroupName);
    if (m_bAllOK)
      {
      if (m_sGroupName.GetLength()<=0)
        {
        m_Message.SetWindowText("Invalid Group Name");
        m_bAllOK=false;
        }
      }

    m_GroupDesc.GetWindowText(m_sGroupDesc);
    m_sGroupDesc.TrimLeft();
    if (m_bAllOK)
      {
      //if (m_sGroupDesc.GetLength()<=0 || m_sGroupDesc[0]=='<')
      //  {
      //  m_Message.SetWindowText("Invalid Group Description");
      //  m_bAllOK=false;
      //  }
      }

    m_DeleteGrp.EnableWindow(FindGroup(m_sGroupName));
    m_OK.EnableWindow(m_bAllOK);
    }
  };

void CNeutralSaveGroupDlg::OnBnClickedFindoptions()
  {
  CMenu Menu;
  Menu.CreatePopupMenu();

  Menu.AppendMenu(MF_STRING, 100, "Start of Tag");
  Menu.AppendMenu(MF_STRING, 101, "End Of Tag");
  Menu.AppendMenu(MF_STRING, 102, "");

  CPoint curPoint;
  GetCursorPos(&curPoint);

  int RetCd=Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD, curPoint.x, curPoint.y, this);
  Menu.DestroyMenu();                                           
  CString FndTxt;
  m_String1.GetWindowText(FndTxt);
  switch (RetCd)
    {
    case 100: FndTxt+="^"; break;
    case 101: FndTxt+="$"; break;
    case 102: break;
    }
  m_String1.SetWindowText(FndTxt);
  }

void CNeutralSaveGroupDlg::OnBnClickedReplaceoptions()
  {
  // TODO: Add your control notification handler code here
  }

void CNeutralSaveGroupDlg::OnBnClickedDoreplace()
  {
  IRegExpPtr *pRE= new IRegExpPtr("VBScript.RegExp");
  CString FndTxt;
  CString RepTxt;
  m_String1.GetWindowText(FndTxt);
  m_String2.GetWindowText(RepTxt);

  _bstr_t bFndTxt(FndTxt);
  _bstr_t bRepTxt(RepTxt);

  int RTags=m_OldTags.GetCount();
  for (int i=0; i<RTags; i++)
    {
    _bstr_t NewTag(m_NewTags[i]);
    
    (*pRE)->Pattern = bFndTxt;
    NewTag=(*pRE)->Replace(NewTag, bRepTxt);
    m_NewTags[i]=(LPCTSTR)NewTag;

    m_TagList.SetItemText(i, 1, m_NewTags[i]);
    }
  DoChecks();
  delete pRE;
  }

void CNeutralSaveGroupDlg::OnBnClickedResettags()
  {
  int RTags=m_OldTags.GetCount();
  for (int i=0; i<RTags; i++)
    {
    m_NewTags[i]=m_OldTags[i];
    m_TagList.SetItemText(i, 1, m_NewTags[i]);
    }
  }

void CNeutralSaveGroupDlg::OnEnChangeGrpdesc()
  {
  DoChecks();
  }

void CNeutralSaveGroupDlg::OnCbnEditupdateGrouplist()
  {
  DoChecks();
  }

void CNeutralSaveGroupDlg::OnCbnSelchangeGrouplist()
  {
  int i=m_GroupList.GetCurSel();
  if (i>=0)
    {
    CString Tmp;
    m_GroupList.GetLBText(i, Tmp);
    m_GroupList.SetWindowText(Tmp);
    }
  DoChecks();
  }

void CNeutralSaveGroupDlg::OnBnClickedOk()
  {
  UpdateData();

  DoChecks();
  if (m_bAllOK)
    {
    CNeutralImportExport NExport;
    NExport.SetGroup(GetGroup());
    NExport.SetGroupDesc(m_sGroupDesc);
    //NExport.SetExportTagList(&Dlg.m_OldTags, &Dlg.m_NewTags);
    NExport.SetExportCoOrds(m_Origin, m_Lo, m_Hi, m_AlignLo, m_AlignHi);
    NExport.SetExportTagList(&m_OldTags, &m_NewTags);
    long Opts = eNDB_StdExport&~eNDB_Configuration;
    if (m_WithConfig.GetCheck()!=0)
      Opts|=eNDB_Configuration|eNDB_Parameters;
    if (m_WithState.GetCheck()!=0)
      Opts|=eNDB_State;
    NExport.DoExport((eScdNDBOptions)Opts, m_sDatabaseFull, m_sDatabaseFull);
    OnOK();
    }
  //else
  //  {
  //  AfxMessageBox("Invalid GroupName");
  //  }
  }

void CNeutralSaveGroupDlg::OnNMDblclkTaglist(NMHDR *pNMHDR, LRESULT *pResult)
  {
  long i=m_TagList.GetSelectedCount()>0 ? m_TagList.GetSelectionMark() : -1;
  if (i>=0)
    {
    CNeutralEditTagDlg Dlg;
    Dlg.m_OldTagName=CDlgC0;
    Dlg.m_NewTagName=CDlgC1;
    Dlg.m_OldTag = m_TagList.GetItemText(i, 0);
    Dlg.m_NewTag = m_TagList.GetItemText(i, 1);
    if (Dlg.DoModal())
      {
      m_NewTags[i]=Dlg.m_NewTag;
      m_TagList.SetItemText(i,1,Dlg.m_NewTag);
      }
    }

  *pResult = 0;
  }


void CNeutralSaveGroupDlg::OnLvnBeginlabeleditTaglist(NMHDR *pNMHDR, LRESULT *pResult)
  {
  NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
  // TODO: Add your control notification handler code here
  *pResult = 0;
  }

void CNeutralSaveGroupDlg::OnLvnEndlabeleditTaglist(NMHDR *pNMHDR, LRESULT *pResult)
  {
  NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
  int i=pDispInfo->item.iItem;
  if (i>=0 && pDispInfo->item.pszText!=NULL)
    {
    CString NewTag;
    NewTag=m_TagList.GetItemText(i, 0);
    int d=m_TagList.GetItemData(i);
    m_NewTags[d]=pDispInfo->item.pszText;
    *pResult = TRUE;
    return;
    }
  *pResult = FALSE;
  }


void CNeutralSaveGroupDlg::OnLvnItemchangedTaglist(NMHDR *pNMHDR, LRESULT *pResult)
  {
  LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
  DoChecks();
  *pResult = 0;
  }

void CNeutralSaveGroupDlg::OnBnClickedDeletegrp()
  {
  DeleteGroup(m_sGroupName);
  DoChecks();
  }

//===========================================================================
//
//
//
//===========================================================================

CNeutralInsertGroupDlg::CNeutralInsertGroupDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNeutralInsertGroupDlg::IDD, pParent)
  {
  //{{AFX_DATA_INIT(CNeutralInsertGroupDlg)
  //}}AFX_DATA_INIT
  Create(CNeutralInsertGroupDlg::IDD, pParent);
  }

//--------------------------------------------------------------------------

void CNeutralInsertGroupDlg::Reset()
  {
  //sm_LastUnitModel = -1;
  //sm_LastLinkModel = -1;
  //sm_LastCLnkModel = -1;
  //sm_LastCLnkModel = -1;
  //sm_LastELnkModel = -1;
  //sm_LastALnkModel = -1;
  }

//--------------------------------------------------------------------------

void CNeutralInsertGroupDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_DATABASE, m_editDatabase);
  //DDX_Control(pDX, IDC_MDLDATABASE, m_MdlDatabase);
  //DDX_Control(pDX, IDC_SEPARATEDBS, m_SeparateDBs);
  DDX_Control(pDX, IDC_GROUPTREE, m_GroupTree);
  DDX_Control(pDX, IDC_TAGFIXRULES, m_FixupRule);
  DDX_Control(pDX, IDC_DESC, m_Desc);
  DDX_Control(pDX, IDC_TAGLIST, m_TagList);
  DDX_Control(pDX, IDC_TAGFIND, m_String1);
  DDX_Control(pDX, IDC_TAGREPLACE, m_String2);
  DDX_Control(pDX, IDC_MESSAGE, m_Message);
  DDX_Control(pDX, IDC_TAGFIXRULES, m_FixupRule);
  DDX_Control(pDX, IDC_STRNM1, m_StrNm1);
  DDX_Control(pDX, IDC_STRNM2, m_StrNm2);
  }

//--------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CNeutralInsertGroupDlg, CDialog)
  //ON_BN_CLICKED(IDC_SEPARATEDBS, OnBnClickedSeparatedbs)
  ON_EN_CHANGE(IDC_DATABASE, OnEnChangedatabase)
  ON_BN_CLICKED(IDC_BROWSENEUTRALPATH, OnBnClickedBrowsedatabase)
  //ON_EN_CHANGE(IDC_MDLDATABASE, OnEnChangeMdldatabase)
  ON_CBN_SELCHANGE(IDC_TAGFIXRULES, OnCbnSelchangeTagfixrules)
  ON_NOTIFY(TVN_SELCHANGED, IDC_GROUPTREE, OnTvnSelchangedGrouptree)
  ON_BN_CLICKED(IDC_DOREPLACE, OnBnClickedDoreplace)
  ON_BN_CLICKED(IDC_RESETTAGS, OnBnClickedResettags)
  ON_BN_CLICKED(IDC_FINDOPTIONS, OnBnClickedFindoptions)
  ON_BN_CLICKED(IDC_REPLACEOPTIONS, OnBnClickedReplaceoptions)
  ON_NOTIFY(NM_DBLCLK, IDC_TAGLIST, OnNMDblclkTaglist)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

void CNeutralInsertGroupDlg::SetDBNameFull(LPCTSTR Fn)
  {
  m_sDatabaseSymb=Fn;
  m_sDatabaseSymb.FnContract();
  m_sDatabaseFull=Fn;
  }
void CNeutralInsertGroupDlg::SetDBNameSymb(LPCTSTR Fn)
  {
  m_sDatabaseSymb=Fn;
  Strng X;
  X.FnExpand((LPTSTR)Fn);
  m_sDatabaseFull=X();
  }

//--------------------------------------------------------------------------
static struct {long What; LPCTSTR Disp; LPCTSTR Nm1; LPCTSTR Nm2; } FURNms[] =
  {
    {eFixup_NoChange,       "No Change",       "",             ""         },
    {eFixup_Prefix,         "Add Prefix",      "Prefix:",      ""         },
    {eFixup_Suffix,         "Add Suffix",      "",             "Suffix:"  },
    {eFixup_PrefixSuffix,   "Add Pre/Suffix",  "Prefix:",      "Suffix:"  },
    {eFixup_RegExpression,  "Regular Exp",     "Find:",        "Replace:" },
    {eFixup_ExchangeTags,    "NewTag List",     "",             ""         },
  };

static LPCTSTR IDlgC0="Tag In Group";
static LPCTSTR IDlgC1="Model Tag";

BOOL CNeutralInsertGroupDlg::OnInitDialog() 
  {
  CDialog::OnInitDialog();

  CBitmap BM;
  BM.LoadBitmap(IDB_MDLSELECTTREE);
  m_SlctImgList.Create(16, 16, FALSE, 0, 4);
  m_SlctImgList.Add(&BM, (CBitmap*)NULL);
  m_GroupTree.SetImageList(&m_SlctImgList, TVSIL_NORMAL);

  CProfINIFile PF(PrjIniFile());

  SetDBNameSymb(PF.RdStr("NeutralGroup", "Database", "$CfgFiles\\GroupLibrary.MDB"));
  m_editDatabase.SetWindowText(m_sDatabaseSymb());

  m_TagList.InsertColumn(0, IDlgC0,   LVCFMT_LEFT, 50);
  m_TagList.InsertColumn(1, IDlgC1,   LVCFMT_LEFT, 50);

  int W=m_TagList.GetStringWidth("XXXXXXXXXXXXX");

  m_TagList.SetColumnWidth(0, ScdPFUser.RdInt("InsertGroup", "Column0", W));
  m_TagList.SetColumnWidth(1, ScdPFUser.RdInt("InsertGroup", "Column1", W));

  for (int i=0; i<sizeof(FURNms)/sizeof(FURNms[0]); i++)
    m_FixupRule.AddString(FURNms[i].Disp);

  m_FixupRule.SetCurSel(PF.RdInt("InsertGroup", "FixupRule", 4));

  m_String1.SetWindowText(PF.RdStr("InsertGroup", "String1", "<ID>"));
  m_String2.SetWindowText(PF.RdStr("InsertGroup", "String2", "A_"));

  FixTagRuleDisp();

  DoChecks();

  UpdateDialogControls(this, FALSE);

  ShowWindow(SW_SHOW);  
  //UpdateData(FALSE);
  return TRUE;
  }

//--------------------------------------------------------------------------

void CNeutralInsertGroupDlg::OnOK() 
  {
  UpdateData(TRUE);
  CProfINIFile PF(PrjIniFile());
  CString S;
  m_editDatabase.GetWindowText(S);
  SetDBNameSymb(S);
  PF.WrStr("NeutralGroup", "Database", m_sDatabaseSymb());

  //PF.WrInt("InsertGroup", "FixupRule", GetFixupRule());
  //PF.WrStr("InsertGroup", "Prefix", GetPrefix());
  //PF.WrStr("InsertGroup", "Suffix", GetSuffix());

  //m_MdlDatabase.GetWindowText(m_sMdlDatabase);
  //PF.WrStr("NeutralImportExport", "MdlDatabase", m_sMdlDatabase);
  //PF.WrLong("NeutralImportExport", "SeparateDBs", m_SeparateDBs.GetCheck());
  CDialog::OnOK();
  gs_pCmd->ExtendCmdLine("\r");
  }

//--------------------------------------------------------------------------

//void CNeutralInsertGroupDlg::OnBnClickedSeparatedbs()
//  {
//  UpdateData(true);
//  // TODO: Add your control notification handler code here
//  BOOL Sep=m_SeparateDBs.GetCheck()!=0;
//  m_MdlDatabase.EnableWindow(Sep?1:0);
//  if (Sep)
//    {
//    CString S;
//    m_GrfDatabase.GetWindowText(S);
//    m_MdlDatabase.SetWindowText(S);
//    }
//  UpdateData(false);
//  }

void CNeutralInsertGroupDlg::OnEnChangedatabase()
  {
  CString S;
  m_editDatabase.GetWindowText(S);
  SetDBNameSymb(S);
  UpdateGroupTree();
  }

void CNeutralInsertGroupDlg::OnBnClickedBrowsedatabase()
  {
  UpdateData(TRUE);
  Strng FName,FPath;
  CString S;
  m_editDatabase.GetWindowText(S);
  SetDBNameSymb(S);
  if (m_sDatabaseFull.GetLength()>0)
    {
    FPath.FnDrivePath((char*)(const char*)m_sDatabaseFull);
    FName.FnNameExt((char*)(const char*)m_sDatabaseFull);
    }
  else
    {
    FName = "*.mdb";
    FPath = "..\\";//PrjPrevDirectory();
    }

  CSCDFileDialog Dlg(TRUE, NULL, FName(), OFN_HIDEREADONLY, "Access Database (*.mdb)|*.mdb||", this);
  Dlg.m_ofn.lpstrInitialDir = FPath();
  Dlg.m_ofn.lpstrTitle = "Browse";
  if (Dlg.DoModal()==IDOK)
    {
    SetDBNameFull(Dlg.GetPathName());
    m_editDatabase.SetWindowText(m_sDatabaseSymb());
    }

  UpdateData(FALSE);
  }


void CNeutralInsertGroupDlg::UpdateGroupTree()
  {
  CProfINIFile *pPF=NULL; // to be completed
  CString      Section;   // to be completed

  m_GroupTree.DeleteAllItems();
  m_GroupList.RemoveAll();

  if (FileExists((LPTSTR)(LPCTSTR)m_sDatabaseFull))
    {
    int nGroups = 0;
    HTREEITEM hSelected=NULL, hFirst=NULL;
    CDocTemplate & GT = ScdApp()->GraphTemplate();
    CNeutralImportExport EGH;
    EGH.SetGroup("*");
    if (EGH.GetGroups(m_GroupList, m_sDatabaseFull)>=0)
      {
      POSITION Pos=m_GroupList.GetHeadPosition();
      while (Pos)
        {
        CNeutralGrpDescItem &S=m_GroupList.GetNext(Pos);


        HTREEITEM hParent = TVI_ROOT, hItem;
        char Buff[2048];
        char *pS=Buff, *pE;
        strcpy(Buff, S.m_sGroup);
        while ((pE=strchr(pS, ':'))!=NULL)
          {
          *pE=0;
          XStrTrim(pS, " ");
          for (hItem=m_GroupTree.GetNextItem(hParent, TVGN_CHILD);
            hItem!=NULL; 
            hItem=m_GroupTree.GetNextItem(hItem, TVGN_NEXT))
            {
            CString S=m_GroupTree.GetItemText(hItem);
            if (S.CompareNoCase(pS)==0)
              break;
            }
          if (hItem==NULL)
            {
            XStrTrim(pS, " ");
            hItem=m_GroupTree.InsertItem(pS, 0, 0, hParent);
            m_GroupTree.SortChildren(hParent);
            m_GroupTree.SetItemData(hItem, 0x0);//FFFF);
            m_GroupTree.SetItemImage(hItem, 0, 1);
            }
          hParent=hItem;
          pS=pE+1;
          }

        XStrTrim(pS, " ");
        hItem=m_GroupTree.InsertItem(pS, 0, 0, hParent);
        //if (_stricmp(MInfo.Class(), InitialSelect)==0)
        //  hSelected=hItem;
        if (hFirst==NULL)
          hFirst=hItem;
        m_GroupTree.SetItemData(hItem, (DWORD_PTR)&S);
        m_GroupTree.SortChildren(hParent);
        m_GroupTree.SetItemImage(hItem, 2, 3);
  
        if (pPF)
          {
          // Restore State of Tree
          HTREEITEM hParent = TVI_ROOT, hItem;
          CString Txt[10];
          long iDepth=0;
          hItem=m_GroupTree.GetNextItem(TVI_ROOT, TVGN_CHILD);
          while (hItem && iDepth>=0)
            {
            UINT State=m_GroupTree.GetItemState(hItem, TVIS_EXPANDED );
            if (1)
              {
              CString S;
              for (int i=0; i<iDepth; i++)
                S+=Txt[i];
              S+=m_GroupTree.GetItemText(hItem);        
              UINT State=(UINT)pPF->RdInt(Section, S, 0);
              if (State&TVIS_EXPANDED)
                {
                m_GroupTree.Expand(hItem, TVE_EXPAND);        
                //??????????
                }
              }

            HTREEITEM h=m_GroupTree.GetNextItem(hItem, TVGN_CHILD);
            if (h)
              {
              Txt[iDepth]=m_GroupTree.GetItemText(hItem);        
              Txt[iDepth]+=":";        
              iDepth++;
              hItem=h;
              }
            else
              {
              h=m_GroupTree.GetNextItem(hItem, TVGN_NEXT);
              if (h)
                {
                hItem=h;
                }
              else
                {
                // Go One up & One along 
                hItem=m_GroupTree.GetNextItem(hItem, TVGN_PARENT);
                if (hItem)
                  hItem=m_GroupTree.GetNextItem(hItem, TVGN_NEXT);
                iDepth--;
                }
              }
            }
          }
        nGroups++;
        }
      }
    }
  else
    { 
    //???
    }
  };

void CNeutralInsertGroupDlg::LoadTagList()
  {
  CNeutralImportExport EGH;
  m_OldTags.SetSize(0);
  m_NewTags.SetSize(0);
  m_TagList.DeleteAllItems();

  if (EGH.EnumerateGroupTags(eScdNDBOptions(eNDB_Sequence|eNDB_Models|eNDB_Configuration|eNDB_Parameters|eNDB_State),
                             GetGroup(), m_OldTags, m_sDatabaseFull)>0)
    {
    int GTags=m_OldTags.GetCount();
  
    for (int i=0; i<GTags; i++)
      {
      m_NewTags.Add(m_OldTags[i]);
      int Pos=m_TagList.InsertItem(i, m_OldTags[i]);
      m_TagList.SetItemText(Pos, 1, m_NewTags[i]);
      m_TagList.SetItemData(Pos, i);
      }
    }
  }

void CNeutralInsertGroupDlg::FixTagRuleDisp()
  {
  int iFix=m_FixupRule.GetCurSel();
  LPCTSTR Nm1="";
  LPCTSTR Nm2="";
  for (int i=0; i<sizeof(FURNms)/sizeof(FURNms[0]); i++)
    if (iFix==FURNms[i].What)
      {
      Nm1=FURNms[i].Nm1;
      Nm2=FURNms[i].Nm2;
      break;
      }
  m_String1.EnableWindow(strlen(Nm1)>0);
  m_String2.EnableWindow(strlen(Nm2)>0);
  m_StrNm1.SetWindowText(Nm1);
  m_StrNm2.SetWindowText(Nm2);
  };

void CNeutralInsertGroupDlg::OnCbnSelchangeTagfixrules()
  {
  FixTagRuleDisp();
  }

bool CNeutralInsertGroupDlg::CompleteForUse()
  {
  return DoChecks();
  };

void CNeutralInsertGroupDlg::ResetAfterImport()
  {
  OnBnClickedResettags();
  };

eScdTagFixupRules CNeutralInsertGroupDlg::GetFixupRule() 
  {
  return (eScdTagFixupRules)m_FixupRule.GetCurSel(); 
  };
CString CNeutralInsertGroupDlg::GetString1()
  {
  CString S;
  m_String1.GetWindowText(S);
  return S;
  }
CString CNeutralInsertGroupDlg::GetString2()
  {
  CString S;
  m_String2.GetWindowText(S);
  return S;
  }
CStringArray * CNeutralInsertGroupDlg::GetOldTags()
  {
  return &m_OldTags;
  }
CStringArray * CNeutralInsertGroupDlg::GetNewTags()
  {
  return &m_NewTags;
  }
CString CNeutralInsertGroupDlg::GetDatabaseName()
  {
  return m_sDatabaseFull;
  };
CString CNeutralInsertGroupDlg::GetGroup()
  {
  CString S;
  HTREEITEM h=m_GroupTree.GetSelectedItem();
  while (h)
    {
    CString Txt=m_GroupTree.GetItemText(h);
    if (S.GetLength()>0)
      Txt+=":"+S;
    S=Txt;
    h=m_GroupTree.GetParentItem(h);
    }
  return S;
  };
bool CNeutralInsertGroupDlg::IsGroupSelected()
  {
  HTREEITEM h=m_GroupTree.GetSelectedItem();
  return (h!=NULL);
  };


void CNeutralInsertGroupDlg::OnTvnSelchangedGrouptree(NMHDR *pNMHDR, LRESULT *pResult)
  {
  CString SS=CNeutralInsertGroupDlg::GetGroup();

  //LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
  HTREEITEM h=m_GroupTree.GetSelectedItem();
  if (h)
    {
    CNeutralGrpDescItem *p=(CNeutralGrpDescItem*)m_GroupTree.GetItemData(h);
    if (p)
      {
      m_Desc.SetWindowText(p->m_Desc);
      }
    else
      m_Desc.SetWindowText("");
    }
  
  LoadTagList();
  
  *pResult = 0;
  }

void CNeutralInsertGroupDlg::OnBnClickedDoreplace()
  {
  int RTags=m_OldTags.GetCount();
  for (int i=0; i<RTags; i++)
    {
    m_NewTags[i]=CNeutralGroupTagEditor::AdjustTag(m_NewTags[i], GetFixupRule(), GetString1(), GetString2(), GetOldTags(), GetNewTags());//CStringArray * OldTags, CStringArray * NewTags)
    m_TagList.SetItemText(i, 1, m_NewTags[i]);
    }
  DoChecks();
  }

void CNeutralInsertGroupDlg::OnBnClickedResettags()
  {
  int RTags=m_OldTags.GetCount();
  for (int i=0; i<RTags; i++)
    {
    m_NewTags[i]=m_OldTags[i];
    m_TagList.SetItemText(i, 1, m_NewTags[i]);
    }
  }

void CNeutralInsertGroupDlg::OnBnClickedFindoptions()
  {
  // TODO: Add your control notification handler code here
  }

void CNeutralInsertGroupDlg::OnBnClickedReplaceoptions()
  {
  // TODO: Add your control notification handler code here
  }

bool CNeutralInsertGroupDlg::DoChecks()
  {
  bool OK=true;
  m_Message.SetWindowText("Ready for Insert at Dig Pt");

  CNeutralImportExport NImport;
  NImport.SetGroup(GetGroup());
  //NExport.SetGroupDesc(m_sGroupDesc);
  //NExport.SetExportTagList(&Dlg.m_OldTags, &Dlg.m_NewTags);
  //NExport.SetImportTagFixups(&m_OldTags, &m_NewTags);
  NImport.SetImportTagFixups(GetFixupRule(), GetString1(), GetString2(), GetOldTags(), GetNewTags());
  CString ErrMsg;
  if (!NImport.DoImportValidate(eNDB_StdExport, ErrMsg))
    {
    m_Message.SetWindowText(ErrMsg);
    OK=false;
    }
  return OK;
  };

void CNeutralInsertGroupDlg::OnNMDblclkTaglist(NMHDR *pNMHDR, LRESULT *pResult)
  {
  long i=m_TagList.GetSelectedCount()>0 ? m_TagList.GetSelectionMark() : -1;
  if (i>=0)
    {
    CNeutralEditTagDlg Dlg;
    Dlg.m_OldTagName=IDlgC0;
    Dlg.m_NewTagName=IDlgC1;
    Dlg.m_OldTag = m_TagList.GetItemText(i, 0);
    Dlg.m_NewTag = m_TagList.GetItemText(i, 1);
    if (Dlg.DoModal())
      {
      m_NewTags[i]=Dlg.m_NewTag;
      m_TagList.SetItemText(i,1,Dlg.m_NewTag);
      }
    }

  *pResult = 0;
  }

//===========================================================================
//
//
//
//===========================================================================

// CNeutralEditTagDlg dialog

IMPLEMENT_DYNAMIC(CNeutralEditTagDlg, CDialog)
CNeutralEditTagDlg::CNeutralEditTagDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNeutralEditTagDlg::IDD, pParent)
  , m_OldTagName(_T(""))
  , m_NewTagName(_T(""))
  , m_OldTag(_T(""))
  , m_NewTag(_T(""))
  {
}

CNeutralEditTagDlg::~CNeutralEditTagDlg()
{
}

void CNeutralEditTagDlg::DoDataExchange(CDataExchange* pDX)
{
CDialog::DoDataExchange(pDX);
DDX_Text(pDX, IDC_OLDTAGNAME, m_OldTagName);
DDX_Text(pDX, IDC_NEWTAGNAME, m_NewTagName);
DDX_Text(pDX, IDC_OLDTAG, m_OldTag);
DDX_Text(pDX, IDC_NEWTAG, m_NewTag);
}


BEGIN_MESSAGE_MAP(CNeutralEditTagDlg, CDialog)
END_MESSAGE_MAP()


// CNeutralEditTagDlg message handlers
// D:\Scd.Cnm\DevBld\_Src\Scd\SysCAD\NeutralDlgs.CPP : implementation file
//


