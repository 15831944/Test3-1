//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __MDLCFG_CPP
#include "scexec.h"
#include "sc_defs.h"
#include "datacnvs.h"
#include "sfe_base.h"
#include "scdver.h"
#include "mdlcfg.h"
#include "scdarray.h"
#include "errorlog.h"
#include "sscolumn.h"
#include "sscolumns.h"
#include "ssgroup.h"
#include "ssgroups.h"
#include "ssstyleset.h"
#include "ssstylesets.h"
#include "ssselbookmarks.h"
#include "ssselbookmarks.h"
#include "nrecipe.h"
#include "dbhelper.h"
#include "dlgbusy.h"
#include "sp_db.h"
#include "drv_base.h"
#include "..\..\utils\devlic\scribble.h"
#include "scdcatids.h"
#include "scdctrls.h"
#include "licbase.h"
//#include "optoff.h"

#pragma comment(lib, "rpcrt4.lib")

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern AFX_EXTENSION_MODULE SCExecDLL;

#define NONNULL(a) (a==NULL ? "" : a)

#define NoneString "(none)"

static struct {char * Name; char *Text; int Branch; } Groups[]=
  {
    {FlwUnitGrp,       "Units",                 0 },
    {FlwLinkGrp,       "Links",                 0 },
    {CtrlLinkGrp,      "CLinks",                0 },
    {ElecLinkGrp,      "ELinks",                0 },
    {AirLinkGrp,       "ALinks",                0 },
    {"SpModel",         "Specie Models",          1 },
    {"SpQuality",       "Specie Qualities",       2 },
    {"PipeFlow",        "Pipe Equations",         3 },
    {"PipeFitting",     "PipeFitting Eqns",       4 },
    {"PipeEntryExit",   "PipeEntryExit Eqns",     5 },
    {"TeeEntry",        "TeeEntry Eqns",          6 },
    {"Pump",            "Pump Eqns",              7 },
    {"Valve",           "Valve Eqns",             8 },
    {"ChkValve",        "ChkValve Eqns",          9 },
    {"Orifice",         "Orifice Eqns",          10 },
    {"GasPump",         "GasPump Eqns",          11 },
    {"SizeChange",      "SizeChange Eqns",       12 },
    {"Leak2Area",       "Leak2Area Eqns",        13 },
    {NULL}
  };

static char * Branches[]=
  {
  "Units/Links",  
  "Specie Models", 
  "Specie Qualities",
  "Pipe Equations", 
  "PipeFitting Eqns",   
  "PipeEntryExit Eqns", 
  "TeeEntry Eqns",      
  "Pump Eqns",          
  "Valve Eqns",         
  "ChkValve Eqns",      
  "Orifice Eqns",       
  "GasPump Eqns",       
  "SizeChange Eqns",    
  "Leak2Area Eqns",     
    {NULL}
  };

static char * Grps[]=
  {
  "General",  
  "Energy Transfer", 
  "Mass Separation",
  "Size Distribution", 
  "Mass Transfer", 
  "Alumina", 
  "Power Distribution",
  "Unknown", 
    {NULL}
  };

static DWORD GrpMsks[]=
  {
  TOC_GRP_GENERAL,  
  TOC_GRP_ENERGY, 
  TOC_GRP_SEPAR,
  TOC_GRP_SIZEDST, 
  TOC_GRP_TRANSFER, 
  TOC_GRP_ALUMINA, 
  TOC_GRP_POWERDIST, 
  0 //not specified
  };

static char *Exclude[]=
  {
  "msvcrt.dll", 
  "mfc42.dll",
  //"crp9516d.dll",
  //"cryp95d.dll",
  "crp9516e.dll",
  "cryp95e.dll",
  "hdk3anim.dll",
  "hdk3ctnt.dll",
  //  "scdopcsrv.dll",
  "scdlib.dll",
  "scexec.dll",
  "xylib.dll",
  "scdcom.dll",
  //"sc_clsv.dll",
  //"sc_rpc.dll",
  "schist.dll",
  "scddesrvr.dll",
  "kwdb.dll",
  "zip32.dll",
  "unzip32.dll",

  //OPC
  //"opc.dll",
  "scdopcsrv.dll",
  "scopcsrv.dll",

  // COM Stuff
  "scdslv.dll",
  "scdif.dll",
  "scdmdl.dll",
  "scdcom.dll",
  //"scdulb.dll",

  // SMDK
  "smdk1.dll",
  "smdkint.dll",

  // Utils
  "scdvb.dll",

  NULL
  };


//word TypeMask=0xf;

long GetType(long x) { return x & 0xff; }
long GetOcc(long x) { return (x & 0xff00)>>8; }
long GetIndex(long x) { return (x & 0xffff0000)>>16; }
long SetType(long x) { return x; }
long SetOcc(long x) { return (x<<8 & 0xff00); }
long SetIndex(long x) { return (x<<16 & 0xffff0000); }

struct CfgItem { DWORD IdCtrl; byte CtrlType; char * Name; char * SctName; flag IsDbl; double DblDef; char * StrDef; char * Comment; flag Invert; };

static CfgItem CfgItemsC[]=
  {
    {IDC_CFGDESC,         0, "Description",        "General", false,             0, "",         NULL},
    {IDC_DEFNETMODE,      1, "Default_NetMode",    "Modes",   false,             0, "ProBal",   NULL},
    {IDC_PROBALALLOWED,   2, "Probal_Allowed",     "Modes",   false,             1, "",         NULL},
    {IDC_PBNODEMODE,      1, "Probal_NodeMode",    "Modes",   false,             0, "Direct",   NULL},
    {IDC_PBLINKMODE,      1, "Probal_LinkMode",    "Modes",   false,             0, "Direct",   NULL},
    {IDC_PBFLOWMODE,      1, "Probal_FlowMode",    "Modes",   false,             0, "Transfer", NULL},
    {IDC_PBHEATMODE,      1, "Probal_HeatMode",    "Modes",   false,             0, "Rigorous", NULL},
    {IDC_DYNAMICALLOWED,  2, "Dynamic_Allowed",    "Modes",   false,             1, "",         NULL},
    {IDC_DYNNODEMODE,     1, "Dynamic_NodeMode",   "Modes",   false,             0, "Buffered", NULL},
    {IDC_DYNLINKMODE,     1, "Dynamic_LinkMode",   "Modes",   false,             0, "Direct",   NULL},
    {IDC_DYNFLOWMODE,     1, "Dynamic_FlowMode",   "Modes",   false,             0, "Simple",   NULL},
    {IDC_DYNHEATMODE,     1, "Dynamic_HeatMode",   "Modes",   false,             0, "Rigorous", NULL},

    {IDC_MAXNODEMODE,     1, "Maximum_NodeMode",   "Modes",   false,             0, "Buffered", NULL},
    {IDC_MAXLINKMODE,     1, "Maximum_LinkMode",   "Modes",   false,             0, "Direct",   NULL},
    {IDC_MAXFLOWMODE,     1, "Maximum_FlowMode",   "Modes",   false,             0, "Full",     NULL},
    {IDC_MAXHEATMODE,     1, "Maximum_HeatMode",   "Modes",   false,             0, "Rigorous", NULL},

    {IDC_STDTEMP,     0, "Std_Temp",               "General",  true,         Std_T, "", "C"},
    {IDC_STDPRESS,    0, "Std_Press",              "General",  true,         Std_P, "", "kPa"},
    {IDC_NORMTEMP,    0, "Norm_Temp",              "General",  true,        Norm_T, "", "C"},
    {IDC_NORMPRESS,   0, "Norm_Press",             "General",  true,        Norm_P, "", "kPa"},
    {IDC_MINTEMP,     0, "Minimum_Temp",           "General",  true,    C_2_K(0.0), "", "C" },
    {IDC_MAXTEMP,     0, "Maximum_Temp",           "General",  true, C_2_K(1000.0), "", "C" },
    {IDC_MINPRESS,    0, "Minimum_Press",          "General",  true,          50.0, "", "kPa"},
    {IDC_MAXPRESS,    0, "Maximum_Press",          "General",  true,       16500.0, "", "kPa"},
    //{IDC_ATMOSPRESS,  0, "Atmospheric_Press",    Ggeneral",   false,             0, "101.287, -11.836e-3, 0.47931-6", NULL},
#if WithSIMPLESPMDL                             
    {IDC_DEFSPMDL,    1, "Default_SpModel",       "General",  false,             0, BaseSpModelName, NULL},
#else                                   
    {IDC_DEFSPMDL,    1, "Default_SpModel",       "General",  false,             0, MassWtMnSpModelName, NULL},
#endif
    {IDC_H2OASAQ,     2, "H2O_As_Aqueous",        "General",  false,             0, "", NULL},
    {0, NULL},
  };

static CfgItem CfgItemsO[]=
  {
    {IDC_STDFNS4H2O    , 2, "UseStdFns4H2O",           "General", false, 1, "", NULL, false},
#if WITHDEFAULTSPDB
    {IDC_CONFIGDB      , 2, "SearchDefaultSpecieDB",   "General", false, 0, "", NULL, false},
#endif
    {IDC_TAGSNUMSTART  , 2, "NumericStartingTagsBad",  "General", false, 1, "", NULL, true},
#if WithNumericTags
    {IDC_TAGSNUMERIC   , 2, "NumericTagsBad",          "General", false, 1, "", NULL, true},
#endif
    {0, NULL},
  };

//===========================================================================

void CDllInfoArray::SortByPriority()
  {
  for (long i=1; i<GetSize(); i++)
    for (long j=i; j>=1; j--)
      {
      if (GetAt(j).iPriority<GetAt(j-1).iPriority)
        {
        CDllInfo temp;
        temp = GetAt(j-1);
        SetAt(j-1, GetAt(j));
        SetAt(j, temp);
        }
      }
  }

//===========================================================================

static void RebuildDllTree(CDllInfoArray &DLLs, CSelectTreeCtrl & Tree)
  {
  //bFileTreeBuilt = 1;
  Tree.DeleteAllItems();

  TV_INSERTSTRUCT IS;
  IS.hInsertAfter = TVI_SORT;
  IS.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
  IS.item.state = 0;
  IS.item.stateMask = 0;
  IS.item.iImage = 0;
  IS.item.iSelectedImage = 0;
  IS.item.cChildren = 0;
  HTREEITEM hDll = NULL;
  for (int i=0; i<DLLs.GetSize(); i++)
    {
    CDllInfo & DLLI = DLLs[i];
    if (DLLI.Type==DT_FlwLib)
      {
      IS.hParent = TVI_ROOT;
      IS.hInsertAfter = TVI_SORT;
      IS.item.iImage = 0;//DLLI.Type==DT_FlwLib!=NULL ? 1 : 0;
      IS.item.iSelectedImage = IS.item.iImage;
      IS.item.lParam = (DLLI.Type==DT_FlwLib);
      Strng Nm(DLLI.Name());
      if (DLLI.m_DependentDLLList.GetSize()>0)
        {
        Nm += "   [";
        for (int j=0; j<DLLI.m_DependentDLLList.GetSize(); j++)
          {
          Nm += DLLI.m_DependentDLLList[j];
          Nm += (j==DLLI.m_DependentDLLList.GetSize()-1 ? "]" : ", ");
          }
        }
      IS.item.pszText = Nm();
      //IS.item.pszText = DLLI.Name();
      hDll = Tree.InsertItem(&IS);
      DLLI.hItem = hDll;

      HTREEITEM hBranch = NULL;
      for (int j=0; Branches[j]; j++)
        {
        flag BranchDone=false;

        HTREEITEM hItem = NULL;
        POSITION pos = DLLI.Mdls.GetHeadPosition();
        while (pos)
          {
          MdlInfo &M=DLLI.Mdls.GetNext(pos);
          if (M.iBranch==j)
            {
            if (!BranchDone)
              {
              BranchDone=true;
              IS.hInsertAfter = TVI_LAST;
              IS.hParent = hDll;
              IS.item.iImage = 0;
              IS.item.iSelectedImage = IS.item.iImage;
              IS.item.lParam = 1;
              IS.item.pszText = Branches[j];
              hBranch = Tree.InsertItem(&IS);
              }
            //dbgpln("       %i %-12.12s  %s",M.Branch,M.Rec.Name(),M.Rec.ShortDesc());
            IS.hInsertAfter = TVI_SORT;
            IS.hParent = hBranch;
            IS.item.iImage = 0;
            IS.item.iSelectedImage = IS.item.iImage;
            IS.item.lParam = 1;
            IS.item.pszText = M.Rec.ShortDesc();
            hItem = Tree.InsertItem(&IS);
            M.hItem=hItem;
            }
          }
        }
      /*** Where to put these reqd species 
      hBranch = NULL;
      flag BranchDone=false;

      HTREEITEM hItem = NULL;
      for (Strng * pS=DLLI.m_RqdSpcs.First(); pS; pS=pS->Next())
      {
      if (!BranchDone)
      {
      BranchDone=true;
      IS.hInsertAfter = TVI_LAST;
      IS.hParent = hDll;
      IS.item.iImage = 1;
      IS.item.iSelectedImage = IS.item.iImage;
      IS.item.lParam = 1;
      IS.item.pszText = "Reqd Species";
      hBranch = Tree.InsertItem(&IS);
      }
      //dbgpln("       %i %-12.12s  %s",M.Branch,M.Rec.Name(),M.Rec.ShortDesc());
      IS.hInsertAfter = TVI_SORT;
      IS.hParent = hBranch;
      IS.item.iImage = 1;
      IS.item.iSelectedImage = IS.item.iImage;
      IS.item.lParam = 1;
      IS.item.pszText = pS->Str();
      hItem = Tree.InsertItem(&IS);
      }
      ***/

      //Tree.Expand(hDll,TVE_EXPAND);
      }
    }
  }

//---------------------------------------------------------------------------

static void RebuildDllGrpTree(CDllInfoArray &DLLs, CSelectTreeCtrl & Tree)
  {
  Tree.DeleteAllItems();

  TV_INSERTSTRUCT IS;
  IS.hInsertAfter = TVI_SORT;
  IS.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
  IS.item.state = 0;
  IS.item.stateMask = 0;
  IS.item.iImage = 0;
  IS.item.iSelectedImage = 0;
  IS.item.cChildren = 0;
  int k=0;
  while (Grps[k])
    {
    HTREEITEM hGrp = NULL;
    for (int j=0; Branches[j]; j++)
      {
      HTREEITEM hBranch = NULL;
      for (int DllCnt=0; DllCnt<DLLs.GetSize(); DllCnt++)
        if (DLLs[DllCnt].Type==DT_FlwLib)
          {
          DLLs[DllCnt].hItem=NULL;
          POSITION pos = DLLs[DllCnt].Mdls.GetHeadPosition();
          while (pos)
            {
            MdlInfo &M=DLLs[DllCnt].Mdls.GetNext(pos);
            if (M.iGrp==k && M.iBranch==j)
              {
              if (hGrp==NULL)
                {
                IS.hParent = TVI_ROOT;
                IS.hInsertAfter = TVI_SORT;
                IS.item.iImage = 0;
                IS.item.iSelectedImage = IS.item.iImage;
                IS.item.lParam = 0;
                IS.item.pszText = Grps[k];
                hGrp = Tree.InsertItem(&IS);
                }
              if (hBranch==NULL)
                {
                IS.hInsertAfter = TVI_LAST;
                IS.hParent = hGrp;
                IS.item.iImage = 0;
                IS.item.iSelectedImage = IS.item.iImage;
                IS.item.lParam = 1;
                IS.item.pszText = Branches[j];
                hBranch = Tree.InsertItem(&IS);
                }
              IS.hInsertAfter = TVI_SORT;
              IS.hParent = hBranch;
              IS.item.iImage = 0;
              IS.item.iSelectedImage = IS.item.iImage;
              IS.item.lParam = 1;
              IS.item.pszText = M.Rec.ShortDesc();
              M.hItem = Tree.InsertItem(&IS);
              }
            }
          }
      }
    k++;
    }
  //*** Where to put these reqd species 

  }


//===========================================================================

class CDLLCfgDlg : public CDialog
  {
  public:
    CDLLCfgDlg(CMdlCfgSheet* pSheet_, bool Editable, CWnd* pParent = NULL);   // standard constructor
    //{{AFX_DATA(CDLLCfgDlg)
    enum { IDD = IDD_DLLCFG_DLG };
    CSelectTreeCtrl	m_DllTree;
    int m_Sort;
    //}}AFX_DATA
    //{{AFX_VIRTUAL(CDLLCfgDlg)
  protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
  protected:
    void BuildTree();
    void SaveTree();
    void BldRqdSpeciesList();
    BOOL UseDLLTree;
    CMdlCfgSheet* pSheet; 
    CImageList ImgList;
    bool m_bEditable;
    //{{AFX_MSG(CDLLCfgDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnSort();
    virtual void OnOK();
    afx_msg void OnShowSpecies();
    //}}AFX_MSG
    afx_msg void OnUpdateRemove(CCmdUI* pCmdUi);
    DECLARE_MESSAGE_MAP()
  public:
  };

//===========================================================================

CDLLCfgDlg::CDLLCfgDlg(CMdlCfgSheet* pSheet_, bool Editable, CWnd* pParent /*=NULL*/)
: CDialog(CDLLCfgDlg::IDD, pParent)
  {
  pSheet = pSheet_;
  //{{AFX_DATA_INIT(CDLLCfgDlg)
  m_Sort = 1; 
  //}}AFX_DATA_INIT
  UseDLLTree = (m_Sort!=0);
  m_bEditable =  Editable;
  }

//---------------------------------------------------------------------------

void CDLLCfgDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CDLLCfgDlg)
  DDX_Control(pDX, IDC_DLLTREE, m_DllTree);
  DDX_Radio(pDX, IDC_DLL_SORT_GROUP, m_Sort);
  //}}AFX_DATA_MAP
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CDLLCfgDlg, CDialog)
  //{{AFX_MSG_MAP(CDLLCfgDlg)
  ON_BN_CLICKED(IDC_DLL_SORT_GROUP, OnSort)
  ON_BN_CLICKED(IDC_DLL_SORT_DLL, OnSort)
  ON_BN_CLICKED(IDC_SHOWSPECIES, OnShowSpecies)
  //}}AFX_MSG_MAP
  //	ON_NOTIFY(TVN_SELCHANGED, IDC_DLLTREE, OnTvnSelchangedDlltree)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CDLLCfgDlg::OnInitDialog() 
  {
  CDialog::OnInitDialog();

  dbgpln("=====================================");
  for (int i=0; i<pSheet->DLLs.GetSize(); i++)
    {
    dbgp("%s", pSheet->DLLs[i].Name());
    if (pSheet->DLLs[i].Type==DT_Unloadable)
      dbgp(" NOT LOADED");
    else if (pSheet->DLLs[i].Type==DT_Other)
      dbgp(" OTHER");
    dbgpln("");

    POSITION pos = pSheet->DLLs[i].Mdls.GetHeadPosition();
    while (pos)
      {
      MdlInfo &M=pSheet->DLLs[i].Mdls.GetNext(pos);
      dbgpln("       %2i %2i %s %-12.12s %s", M.iGroup, M.iBranch, M.fLicMdlBlocked ? "Lck" : "   ", M.Rec.Class(), M.Rec.ShortDesc());
      }
    }
  dbgpln("=====================================");

  m_DllTree.LoadImage();
  BuildTree();

  UpdateDialogControls(this, FALSE);

  Strng Caption;
  Caption.Set("Step 1 of 2 : %s", pSheet->m_CfgFile());
  SetWindowText(Caption());
  if (!m_bEditable)
    {
    //AfxMessageBox("This configuration file is not currently editable");
    //pSheet->GetDlgItem();
    GetDlgItem(IDOK)->EnableWindow(FALSE);
    //GetDlgItem(IDC_DLL_SORT_DLL)->EnableWindow(FALSE);
    //GetDlgItem(IDC_DLL_SORT_GROUP)->EnableWindow(FALSE);
    //GetDlgItem(IDC_SHOWSPECIES)->EnableWindow(FALSE);
    m_DllTree.EnableWindow(FALSE);
    GetDlgItem(IDC_STATIC_NOTEDITABLE)->ShowWindow(SW_SHOW);
    }
  else
    GetDlgItem(IDC_STATIC_NOTEDITABLE)->ShowWindow(SW_HIDE);
  return TRUE;
  }

//---------------------------------------------------------------------------

void CDLLCfgDlg::BuildTree()
  {
  Strng Tag, DllName, MdlName;
  if (UseDLLTree)
    RebuildDllTree(pSheet->DLLs, m_DllTree);
  else
    RebuildDllGrpTree(pSheet->DLLs, m_DllTree);

  for (int Cnt=0;;)
    {
    Tag.Set("M%04i", Cnt++);
    DllName = pSheet->Cfg.RdStr("ModelDLLs", Tag(), "???");
    if (DllName.XStrICmp("???")==0)
      break;
    for (int i=0; i<pSheet->DLLs.GetSize(); i++)
      {
      CDllInfo & DLLI = pSheet->DLLs[i];
      if (DLLI.Name.XStrICmp(DllName)==0)
        {
        DLLI.bLoad = true;
        break;
        }
      }
    }

  //mark all models that were previously selected...
  int FailedCnt=0;
  for (Cnt=0;;)
    {
    Tag.Set("S%04i",Cnt++);
    MdlName = pSheet->Cfg.RdStr("Selectable", Tag(), "???");
    if (MdlName.XStrICmp("???")==0)
      {
      switch (FailedCnt)
        {//force these to be selected
        case 0: MdlName.Set("1,%s", BaseSpModelName); break;
        case 1: MdlName = "1,Lockup"; break; //SQLockup
        }
      if (FailedCnt>1)
        break;
      FailedCnt++;
      Cnt--;
      }

    int On=(MdlName.SafeAtoL(1)!=0);
    char * pComma=MdlName.XStrChr(',');
    MdlName=MdlName.Right(MdlName.Length()-(pComma-MdlName()+1));
    MdlName.LRTrim();
    if (MdlName())
      {
      BOOL Found=false;
      for (int i=0; i<pSheet->DLLs.GetSize() && !Found; i++)
        {
        CDllInfo & DLLI = pSheet->DLLs[i];
        POSITION pos = DLLI.Mdls.GetHeadPosition();
        while (pos && !Found)
          {
          MdlInfo &M = DLLI.Mdls.GetNext(pos);
          //_asm int 3;
          if (M.Rec.Class.XStrICmp(MdlName)==0)
            {                                  
            m_DllTree.SetItemSelected(M.hItem, On);
            Found = true;
            }
          }
        }
      }
    }

  //mark all models that are blocked by licensing...
  for (int i=0; i<pSheet->DLLs.GetSize(); i++)
    {
    CDllInfo & DLLI = pSheet->DLLs[i];
    POSITION pos = DLLI.Mdls.GetHeadPosition();
    while (pos)
      {
      MdlInfo &M = DLLI.Mdls.GetNext(pos);
      if (M.fLicMdlBlocked)// || M.fLicModeBlocked)
        m_DllTree.SetItemLocked(M.hItem);
      }
    }

  m_DllTree.FixAllLockImages();
  m_DllTree.FixAllSelectedImages();
  }

//---------------------------------------------------------------------------

void CDLLCfgDlg::SaveTree()
  {
  //determine what DLL files must be loaded...
  if (UseDLLTree)
    {
    for (int i=0; i<pSheet->DLLs.GetSize(); i++)
      {
      CDllInfo & DLLI = pSheet->DLLs[i];
      DLLI.bLoad = m_DllTree.GetItemSelected(DLLI.hItem, false);
      }
    }
  else
    {
    for (int i=0; i<pSheet->DLLs.GetSize(); i++)
      {
      POSITION pos = pSheet->DLLs[i].Mdls.GetHeadPosition();
      while (pos)
        {
        MdlInfo &M = pSheet->DLLs[i].Mdls.GetNext(pos);
        if (m_DllTree.GetItemSelected(M.hItem, false))
          {
          pSheet->DLLs[i].bLoad = 1;
          break;
          }
        }
      }
    }

  //check DLL dependencies...
  char Buff[512];
  int i;
  for (i=0; i<pSheet->DLLs.GetSize(); i++)
    {
    CDllInfo & DLLI = pSheet->DLLs[i];
    if (_stricmp(DLLI.Name(), "flwlib.dll")==0)
      DLLI.bLoad = 1;
    if (DLLI.bLoad)
      {
      for (int j=0; j<DLLI.m_DependentDLLList.GetSize(); j++)
        {
        CString DepDll(DLLI.m_DependentDLLList[j]);
        DepDll += ".dll";
        int k = pSheet->DLLs.FindByName((LPTSTR)(LPCTSTR)DepDll);
        if (k<0)
          {
          sprintf(Buff, "The DLL '%s' required by '%s' has not been found!", (LPCTSTR)DepDll, DLLI.Name());
          AfxMessageBox(Buff);
          }
        else if (!(pSheet->DLLs[k].bLoad))
          {
          pSheet->DLLs[k].bLoad = 1;
          //sprintf(Buff, "The DLL '%s' is required by '%s'.", DepDll(), DLLI.Name());
          //AfxMessageBox(Buff);
          }
        }
      }
    }

  //write list of required DLLs to file...
  int Cnt=0;
  Strng Tag, Old, S;
  for (i=0; i<pSheet->DLLs.GetSize(); i++)
    {
    CDllInfo & DLLI = pSheet->DLLs[i];
    if (DLLI.bLoad || _stricmp(DLLI.Name(), "flwlib.dll")==0)
      {
      Tag.Set("M%04i", Cnt++);
      pSheet->Cfg.WrStr("ModelDLLs", Tag(), DLLI.Name());
      }
    }
  while (1)
    {
    Tag.Set("M%04i", Cnt++);
    Old = pSheet->Cfg.RdStr("ModelDLLs", Tag(), "???");
    if (Old.XStrCmp("???")==0)
      break;
    pSheet->Cfg.WrStr("ModelDLLs", Tag(), (char*)NULL);
    }

  //write list of selectable models to file...
  Cnt=0;
  for (int g=0; Groups[g].Name; g++)
    {
    dbgpln("Grp    :%s", Groups[g].Name);
    bool GrpWritten=false;
    for (int i=0; i<pSheet->DLLs.GetSize(); i++)
      {
      CDllInfo & DLLI = pSheet->DLLs[i];
      dbgpln("  DLL  :%s", DLLI.Name());
      if (DLLI.bLoad)
        {
        POSITION pos = DLLI.Mdls.GetHeadPosition();
        while (pos)
          {
          MdlInfo &M = DLLI.Mdls.GetNext(pos);
          dbgp("    Mdl:%s ", M.Rec.Class);
          if (M.iGroup==g)
            {
            dbgp(" <==========================");
            if (!GrpWritten)
              {
              GrpWritten=true;
              Tag.Set("S%04i",Cnt++);
              S.Set("0,%s", Groups[g].Name);
              pSheet->Cfg.WrStr("Selectable", Tag(), S());
              }
            Tag.Set("S%04i", Cnt++);
            S.Set("%i,%s", m_DllTree.GetItemSelected(M.hItem, false)!=0, M.Rec.Class());
            pSheet->Cfg.WrStr("Selectable", Tag(), S());
            }
          dbgpln("");
          }
        }
      }
    }

  // Delete Old
  while (1)
    {
    Tag.Set("S%04i", Cnt++);
    Old = pSheet->Cfg.RdStr("Selectable", Tag(), "???");
    if (Old.XStrCmp("???")==0)
      break;
    pSheet->Cfg.WrStr("Selectable", Tag(), (char*)NULL);
    }
  }

//---------------------------------------------------------------------------

void CDLLCfgDlg::OnSort() 
  {
  UpdateData(true);
  BOOL NewUseDLLTree = (m_Sort!=0);
  if (NewUseDLLTree!=UseDLLTree)
    {
    SaveTree();
    UseDLLTree = NewUseDLLTree;
    BuildTree();
    }
  }

//---------------------------------------------------------------------------

void CDLLCfgDlg::OnOK() 
  {
  if (m_bEditable)
    {
    UpdateData(true);
    SaveTree();
    BldRqdSpeciesList();
    CDialog::OnOK();
    }
  }

//---------------------------------------------------------------------------

/*void CDLLCfgDlg::OnBrowse() 
{
Strng Folder;
int ret = DllFilesFolder(0, DllFilesPath(), Folder);
CString m_NewFolder,m_NewName;
//UpdateData(TRUE);
CSCDFileDialog Dlg(FALSE, NULL, "*.dll", 0, "SysCAD SMDK DLL Files(*.dll)|*.dll||", this);
Dlg.m_ofn.lpstrInitialDir = Folder();
Dlg.m_ofn.lpstrTitle = "Browse";
if (Dlg.DoModal()==IDOK && Dlg.GetPathName() && strlen(Dlg.GetPathName())>0)
{
CString fn = Dlg.GetPathName();
Strng Tmp((char*)(const char*)fn);  
Tmp.FnNameExt();
CString NewName = Tmp();
CScribble TestDll;
bool OK = true;
if (!(TestDll.ReadScribble(fn) || BYPASSLICENSING))
{
char Buff[512];
sprintf(Buff, "%s not recognised as a SysCAD SMDK DLL file!\n\nAdd anyway?", (const char*)NewName);
OK = (AfxMessageBox(Buff, MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2)==IDYES);
}
if (OK)
{
for (int i=0; i<m_SMDKDLLList.GetItemCount(); i++)
{
CString S = m_SMDKDLLList.GetItemText(i, 0);
if (_stricmp((const char*)S, (const char*)NewName)==0)
{
OK = false;
m_SMDKDLLList.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
m_SMDKDLLList.EnsureVisible(i, false);
}
}
if (OK)
{
int n = m_SMDKDLLList.InsertItem(m_SMDKDLLList.GetItemCount(), (const char*)NewName, 1);
m_SMDKDLLList.SetItemState(n, LVIS_SELECTED, LVIS_SELECTED);
m_SMDKDLLList.EnsureVisible(n, false);
UpdateDialogControls(this, FALSE);
}
}

}
}*/

//---------------------------------------------------------------------------

void CDLLCfgDlg::OnShowSpecies() 
  {
  SaveTree();
  char Buff[2048];
  char bb[128];
  Buff[0] = 0;

  for (int i=0; i<pSheet->DLLs.GetSize(); i++)
    {
    CDllInfo & DLLI = pSheet->DLLs[i];
    if (DLLI.bLoad && DLLI.m_RqdSpcs.GetSize()>0 && _stricmp(DLLI.Name(), "flwlib.dll")!=0)
      {
      for (int j=0; j<DLLI.m_RqdSpcs.GetSize(); j++)
        {
        if (j==0)
          sprintf(bb, "%s:%s\n", DLLI.Name(), DLLI.m_RqdSpcs[j]());
        else
          sprintf(bb, "\t%s\n", DLLI.m_RqdSpcs[j]());
        strcat(Buff, bb);
        }
      }
    }
  if (Buff[0])
    AfxMessageBox(Buff, MB_ICONINFORMATION);
  else
    AfxMessageBox("There are no required species based on the selected DLL files");
  }

//---------------------------------------------------------------------------

void CDLLCfgDlg::BldRqdSpeciesList() 
  {
  for (int i=0; i<pSheet->DLLs.GetSize(); i++)
    {
    CDllInfo & DLLI = pSheet->DLLs[i];
    if (DLLI.bLoad && DLLI.m_RqdSpcs.GetSize()>0 && _stricmp(DLLI.Name(), "flwlib.dll")!=0)
      {
      //dbgpln("DLL - %s", DLLI.Name());
      for (int j=0; j<DLLI.m_RqdSpcs.GetSize(); j++)
        {
        //dbgpln("  Rqd %i %s", DLLI.m_RqdSpcs[j].Index(), DLLI.m_RqdSpcs[j].Str());
        if (pSheet->m_DllRqdSpecies.Find(DLLI.m_RqdSpcs[j](), true)<0)
          {
          //dbgpln("    ADD");
          pSheet->m_DllRqdSpecies.AddStrng(DLLI.m_RqdSpcs[j]);
          }
        }
      }
    }
  }

//===========================================================================
//
//
//
//===========================================================================

IMPLEMENT_DYNAMIC(CMdlCfgSheet, CPropertySheet)

CMdlCfgSheet::CMdlCfgSheet(DWORD LicCategory, char *CfgFile, UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
: CPropertySheet(nIDCaption, pParentWnd, iSelectPage)//, //,
//Cfg(Filename)
  {
  ASSERT(FALSE); //DO NOT EXPECT TO GET HERE !!!
  }

//---------------------------------------------------------------------------

CMdlCfgSheet::CMdlCfgSheet(DWORD LicCategory, char *CfgFile, LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
: CPropertySheet(pszCaption, pParentWnd, iSelectPage)//,
//Cfg(Filename)
  {
  dwLicCategory=LicCategory;
  m_CfgFile=CfgFile;
  m_CfgFileTmp=CfgFile;
  m_CfgFileTmp+=".tmp";
  if (FileExists(m_CfgFile()))
    if (!::CopyFile(m_CfgFile(), m_CfgFileTmp(), false))
      LogError("EditCfg", 0, "Cannot copy to File %s", m_CfgFileTmp());
  Cfg.SetProfFilename(m_CfgFileTmp());

  pSfeBase = NULL;
  pCfgPage = NULL;
  pOptPage = NULL;
  pSpcPage = NULL;
  pMdlPage = NULL;
  GetLoadableDLLs(DllFilesPath());
  LoadDLLs();

  Strng SpcTest=Cfg.RdStr("Species", "S0000", "");
  bool Editable = (SpcTest.GetLength()==0 || isdigit(SpcTest[0]));


  CDLLCfgDlg DLLDlg(this, Editable);
  if ((DLLDlg.DoModal()==IDOK) && Editable)
    {
    pCfgPage = new CMdlCfgCfg(this);
    pOptPage = new CMdlCfgOpt(this);
    pSpcPage = new CMdlCfgSpcs(this);
    //pMdlPage = new CMdlCfgMdls(this);

    //  AddPage(pSpcPage);
    AddPage(pCfgPage);
    AddPage(pOptPage);
    AddPage(pSpcPage);
    //AddPage(pMdlPage);

    LoadDLLPropPages();

    Strng Caption;
    Caption.Set("Step 2 of 2 : %s", m_CfgFile());
    SetTitle(Caption());
    }
  else
    UnLoadDLLs();
  }

//---------------------------------------------------------------------------

CMdlCfgSheet::~CMdlCfgSheet()
  {
  delete pCfgPage;
  delete pOptPage;
  delete pSpcPage;
  delete pMdlPage;
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CMdlCfgSheet, CPropertySheet)
  //{{AFX_MSG_MAP(CMdlCfgSheet)
  ON_WM_NCDESTROY()
  ON_WM_CLOSE()
  ON_COMMAND(IDOK,OnOK)
  ON_COMMAND(IDCANCEL,OnCancel)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

int CMdlCfgSheet::GetLoadableDLLs(char * pPath)
  {
  CWaitCursor WC;

  //Strng Path, Tmp;

  for (int iFold=0; ; iFold++)
    {
    Strng Tmp, Folder;
    int ret=DllFilesFolder(iFold, DllFilesPath(), Folder);
    if (ret==1)
      {
      Tmp=Folder;
      Tmp+="*.dll";

      WIN32_FIND_DATA fd;
      HANDLE H = FindFirstFile(Tmp(), &fd);
      flag AllDone = (H==INVALID_HANDLE_VALUE);
      while (!AllDone)
        {
        if ((fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)==0) 
          {
          for (int i=0; Exclude[i]; i++)
            {
            if (_stricmp(fd.cFileName, Exclude[i])==0)
              break;
            }
          if ((Exclude[i]==NULL) && (_stricmp(&(fd.cFileName[strlen(fd.cFileName)-4]), ".dll")==0))
            {
            Tmp=Folder;
            Tmp+=fd.cFileName;
            if (DLLVersionOK(Tmp())==0)
              {
              if (DLLs.FindByName(fd.cFileName)<0)
                {
                CDllInfo I;
                I.Name=fd.cFileName;
                I.FullName=Folder;
                I.FullName+=fd.cFileName;
                DLLs.Add(I);
                }
              else
                {
                LogWarning("TestLoad", 0, "DLL '%s' encountered again (%s)", fd.cFileName, Folder());
                }
              }
            /*else
            {
            CScribble TestDll;
            CString fn;
            fn = Tmp();
            if (TestDll.ReadScribble(fn) || BYPASSLICENSING)
            {
            //Strng s(fd.cFileName);
            //SMDKDLLs.Add(s);
            CDllInfo I;
            I.Name=fd.cFileName;
            I.FullName=Folder;
            I.FullName+=fd.cFileName;
            DLLs.Add(I);
            }
            }*/
            }
          }
        AllDone = !FindNextFile(H, &fd);
        }
      FindClose(H);
      }
    else if (ret==0)
      {
      // Not a Valid Folder
      }
    else if (ret==-1)
      break;
    }
  //  for (int Loop=0; Loop<2; Loop++)
  //    {
  //    Path=ProgFiles();
  //    if (Loop>0)
  //      Path+="Secure\\";
  //    Tmp=Path;
  //    Tmp+="*.dll";
  //
  //    WIN32_FIND_DATA fd;
  //    HANDLE H = FindFirstFile(Tmp(), &fd);
  //    flag AllDone = (H==INVALID_HANDLE_VALUE);
  //    while (!AllDone)
  //      {
  //      if ((fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)==0) 
  //        {
  //        for (int i=0; Exclude[i]; i++)
  //          if (_stricmp(fd.cFileName, Exclude[i])==0)
  //            break;
  //        if (Exclude[i]==NULL)
  //          {
  //          Tmp=Path;
  //          Tmp+=fd.cFileName;
  //          if (DLLVersionOK(Tmp()))
  //            {
  //            if (DLLs.FindByName(fd.cFileName)<0)
  //              {
  //              CDllInfo I;
  //              I.Name=fd.cFileName;
  //              I.FullName=Path;
  //              I.FullName+=fd.cFileName;
  //              DLLs.Add(I);
  //              }
  //            else
  //              {
  //              LogWarning("TestLoad", 0, "DLL '%s' encountered again (%s)", fd.cFileName, Path);
  //              }
  //            }
  //          }
  //        }
  //      AllDone = !FindNextFile(H, &fd);
  //      }
  //    }
  CDllInfo I;
  I.Name="UnKnown.dll";
  I.FullName=ProgFiles();
  I.FullName+=I.Name;
  DLLs.Add(I);

  return DLLs.GetSize();
  }

//---------------------------------------------------------------------------

BOOL CMdlCfgSheet::DoLoad(int i)
  {
  char* pDLLName=DLLs[i].Name();
  CDlgBusy::SetLine(3, pDLLName);
  //DLLs[i].hInst = AfxLoadLibrary(DLLs[i].FullName());
  DLLs[i].hInst = MyAfxLoadLibrary(pDLLName, DllFilesPath());
  DLLs[i].bLoadOK = (DLLs[i].hInst!=NULL);
  if (DLLs[i].bLoadOK)
    {
    BOOL IsFlw = (GetProcAddress(DLLs[i].hInst, "IsFlowLibDLL")!=NULL);
    BOOL IsSmdk = (GetProcAddress(DLLs[i].hInst, "IsSMDKLibDLL")!=NULL);
    BOOL LoadErr = (!IsFlw && !IsSmdk);
    if (!LoadErr)
      {
      if (IsFlw)
        {
        DLLs[i].Type=DT_FlwLib;
        fn_GetDLLInfo pDIFn=(fn_GetDLLInfo)GetProcAddress(DLLs[i].hInst, "GetDLLInfo");
        LoadErr = (pDIFn==NULL);
        if (!LoadErr)
          {
          MDLLInfo DLLInfo;
          pDIFn(&DLLInfo);
          DLLs[i].iPriority = DLLInfo.iPriority;
          for (int j=0; j<DLLInfo.m_DependentDLLList.GetSize(); j++)
            DLLs[i].m_DependentDLLList.Add(DLLInfo.m_DependentDLLList[j]);
          }
        }
      else if (IsSmdk)
        {
        DLLs[i].Type=DT_FlwLib;//DT_SMDK;
        DLLs[i].iPriority = 123; //make smdk DLL's last
        DLLs[i].bSMDK = 1;
        DLLs[i].m_DependentDLLList.Add("MdlBase");
        //DLLs[i].m_DependentDLLList.Add("???");
        }
      }
    if (LoadErr)
      {
      //perhaps a driver dll...
#if WITHDRVMAN
      if ((fn_AddDrv)GetProcAddress(DLLs[i].hInst, "AddDrv")!=NULL)
        {
        DLLs[i].Type=DT_Other;//DT_Driver;
        }
      else
#endif
        {
        DLLs[i].Type=DT_Other;
        LogError("TestLoad", 0, "%s not loaded", pDLLName);
        }
      if (lDoFreeLibraries)
        AfxFreeLibrary(DLLs[i].hInst);
      DLLs[i].hInst=NULL;
      DLLs[i].bLoadOK=0;
      }
    }
  else
    {
    DLLs[i].Type=DT_Unloadable;
    DLLs[i].bLoadOK=0;
    LogError("TestLoad", 0, "%s not loaded", pDLLName);
    }

  if (DLLs[i].hInst)
    {
    BOOL OK = (lDoFreeLibraries ? AfxFreeLibrary(DLLs[i].hInst) : TRUE);
    DLLs[i].hInst=NULL;
    }

  return DLLs[i].bLoadOK;
  }

//---------------------------------------------------------------------------

int CMdlCfgSheet::LoadDLLs()
  {
  CWaitCursor WC;
  ASSERT(pSfeBase==NULL);
  CDlgBusy::Open("\nLoading Available DLL's");
  int iFlw=-1;
  int iMBs=-1;
  int iUnk=-1;
  for (int i=0; i<DLLs.GetSize(); i++)
    {
    DLLs[i].hInst=NULL;
    Strng Fn;
    Fn.FnNameExt(DLLs[i].FullName());
    if (Fn.XStrICmp("FLWLIB.DLL")==0)
      iFlw=i;
    else if (Fn.XStrICmp("UNKNOWN.DLL")==0)
      iUnk=i;
    else if (Fn.XStrICmp("MDLBASE.DLL")==0)
      iMBs=i;
    }
  if (iFlw<0)
    {
    LogError("TestLoad", 0, "FLWLIB.DLL not found");
    return 0;
    }
  if (iMBs<0)
    {
    LogError("TestLoad", 0, "MDLBASE.DLL not found");
    return 0;
    }

  CStopWatch SW;
  SW.Start();
  int DllLoadCnt=0;
  char* pDLLName=DLLs[iFlw].FullName();
  CDlgBusy::SetLine(3, pDLLName);
  DLLs[iFlw].hInst=AfxLoadLibrary(pDLLName);
  //DLLs[iFlw].hInst=MyAfxLoadLibrary(pDLLName, DllFilesPath());
  DLLs[iFlw].bLoadOK=(DLLs[iFlw].hInst!=NULL);
  DLLs[iFlw].Type=DLLs[iFlw].bLoadOK ? DT_FlwLib : DT_Unloadable;
  DLLs[iFlw].iPriority=0;
  DLLs[iFlw].bLoad=1;//must always load this dll
  if (!DLLs[iFlw].bLoadOK)
    {
    LogError("TestLoad", 0, "FLWLIB.DLL not loaded");
    return 0;
    }
  DllLoadCnt++;

  pDLLName=DLLs[iMBs].FullName();
  CDlgBusy::SetLine(3, pDLLName);
  DLLs[iMBs].hInst=AfxLoadLibrary(pDLLName);
  //DLLs[iMBs].hInst=MyAfxLoadLibrary(pDLLName, DllFilesPath());
  DLLs[iMBs].bLoadOK=(DLLs[iMBs].hInst!=NULL);
  DLLs[iMBs].Type=DLLs[iFlw].bLoadOK ? DT_FlwLib : DT_Unloadable;
  DLLs[iMBs].iPriority=0;
  DLLs[iMBs].bLoad=1;//must always load this dll
  if (!DLLs[iMBs].bLoadOK)
    {
    LogError("TestLoad", 0, "MDLBASE.DLL not loaded");
    return 0;
    }
  DllLoadCnt++;

  BOOL OK = (lDoFreeLibraries ? AfxFreeLibrary(DLLs[iMBs].hInst) : TRUE);
  DLLs[iMBs].hInst=NULL;

  OK = (lDoFreeLibraries ? AfxFreeLibrary(DLLs[iFlw].hInst) : TRUE);
  DLLs[iFlw].hInst=NULL;

  for (i=0; i<DLLs.GetSize(); i++)
    if (i!=iFlw && i!=iUnk && i!=iMBs)
      {
      if (DoLoad(i))
        DllLoadCnt++;
      }

    SW.Stop();
    CString s1;
    dbgpln("Loaded %d DLLs  Time %s", DllLoadCnt, SW.MinSecDesc(s1));

    DLLs.SortByPriority();

    //reload required DLLs in correct sequence...
    iFlw=-1;
    iUnk=-1;
    for (i=DLLs.GetSize()-1; i>=0; i--)
      {
      Strng Fn;
      Fn.FnNameExt(DLLs[i].FullName());
      if (Fn.XStrICmp("FLWLIB.DLL")==0)
        iFlw=i;
      if (Fn.XStrICmp("UNKNOWN.DLL")==0)
        iUnk=i;
      }

    pDLLName=DLLs[iFlw].Name();
    CDlgBusy::SetLine(3, pDLLName);
    DLLs[iFlw].hInst=AfxLoadLibrary(pDLLName);
    //DLLs[iFlw].hInst=MyAfxLoadLibrary(pDLLName, DllFilesPath());
    DLLs[iFlw].bLoadOK=(DLLs[iFlw].hInst!=NULL);
    ASSERT(DLLs[iFlw].bLoadOK);

    ASSERT(pSfeBase==NULL);
    fn_AddSfeSrvrBase AddSfeSrvrBase = (fn_AddSfeSrvrBase)GetProcAddress(DLLs[iFlw].hInst, "AddSfeSrvrBase");
    if (AddSfeSrvrBase)
      {
      pSfeBase=AddSfeSrvrBase();

      for (i=0; i<DLLs.GetSize(); i++)
        if (DLLs[i].bLoadOK && i!=iFlw)
          {
          pDLLName=DLLs[i].Name();
          CDlgBusy::SetLine(3, pDLLName);
          DLLs[i].hInst=MyAfxLoadLibrary(pDLLName, DllFilesPath());
          DLLs[i].bLoadOK=(DLLs[i].hInst!=NULL);
          }
      }

    if (pSfeBase)
      {
#ifndef _RELEASE
      TagObjClass::DumpAll();
#endif
      MdlInfo Info;
      for (int j=0; Groups[j].Name; j++)
        {
        dbgpln("------ %s", Groups[j].Name);
        for (int iNo=0; pSfeBase->RequestModelInfoByGroupIndex(Groups[j].Name, iNo, Info.Rec); iNo++)
          {
          Info.Rec.MdlLibName.FnCheckExtension(".dll");
          Info.iGroup=j;
          Info.iBranch=Groups[j].Branch;
          Info.fLicMdlBlocked=((dwLicCategory & TOC_MDL_MASK & Info.Rec.Category)!=(TOC_MDL_MASK & Info.Rec.Category));
          Info.fLicModeBlocked=((dwLicCategory & TOC_SOLVE_MASK & Info.Rec.Category)==0);
          Info.iGrp=0;
          while (GrpMsks[Info.iGrp] && ((GrpMsks[Info.iGrp] & Info.Rec.Category)==0))
            Info.iGrp++;


          dbgpln("%-8s %-8s %s", Info.fLicMdlBlocked?"MdlBlk":"", Info.fLicModeBlocked?"ModeBlk":"", Info.Rec.ShortDesc());
          for (i=0; i<DLLs.GetSize(); i++)
            if (DLLs[i].Name.XStrICmp(Info.Rec.MdlLibName())==0)
              {
              DLLs[i].Mdls.AddTail(Info);
              break;
              }

            if (i>=DLLs.GetSize()) // Not found
              DLLs[iUnk].Mdls.AddTail(Info);
          }
        }

      for (CSpecieBlk * pSp=CSpecieBlk::First(); pSp; pSp=pSp->Next())
        {
        Strng Dll(pSp->m_sDll);
        Dll.FnCheckExtension(".dll");
        for (i=0; i<DLLs.GetSize(); i++)
          {
          if (DLLs[i].Name.XStrICmp(Dll())==0)
            {
            //dbgpln("AAA %i %s", pSp->m_fAllowAq?1:0, pSp->m_sSpName());
            if (DLLs[i].FindRqdSpecie(pSp->m_sSpName())==-1)
              {
              //dbgpln(" BBB");
              Strng S(pSp->m_sSpName());
              S.SetIndex(pSp->m_fAllowAq?1:0);
              DLLs[i].m_RqdSpcs.Add(S);
              }
            break;
            }
          }
        }
      }

    CDlgBusy::Close();
    return DLLs.GetSize();
  }

//---------------------------------------------------------------------------

void CMdlCfgSheet::LoadDLLPropPages()
  {
  if (pSfeBase)
    {
    for (int i=0; i<DLLs.GetSize(); i++)
      {
      CDllInfo & DLLI = DLLs[i];
      if (DLLI.bLoadOK && DLLI.bLoad)
        {
        fn_GetCfgPropPg pPPFn=(fn_GetCfgPropPg)GetProcAddress(DLLI.hInst, "GetCfgPropPg");
        if (pPPFn)
          {
          CMdlCfgBase * pPP;
          int j=0;
          while ((pPP=pPPFn(j, this))!=NULL)
            {
            PropPages.Add(pPP);
            AddPage(pPP);
            j++;
            }
          }
        }
      }
    }
  }

//---------------------------------------------------------------------------

int CMdlCfgSheet::UnLoadDLLs()
  {
  for (int i=0; i<PropPages.GetSize(); i++)
    delete PropPages[i];
  PropPages.SetSize(0);
  delete pSfeBase;
  pSfeBase=NULL;

  for (i=DLLs.GetSize()-1; i>=0; i--)
    if (DLLs[i].hInst)
      if (lDoFreeLibraries)
        AfxFreeLibrary(DLLs[i].hInst);

  return DLLs.GetSize();
  }

//---------------------------------------------------------------------------

//const word OccMask=0x000f;
//const dword SrcMask =0x0000000f;
//const dword VPMask  =0x00000010;
//const dword OccMask =0x00000700;
//const dword SolMask =0x00000100;
//const dword LiqMask =0x00000200;
//const dword VapMask =0x00000400;

BOOL CMdlCfgSheet::GetSpecieNames(char * Filename, int Src) 
  {                                                        
  flag Ok=false;
  CFileStatus State;
  if (CFile::GetStatus(Filename, State))
    {
    char* SpColNames[] = { "Name", "Compound", "Phase", "Occurence", "Vp", "Ts", "Te", NULL };
    CDBHelper DB((LPSTR)(LPCSTR)EscapeQuote(Filename), "Species");
    if (DB.Open(DBConnect_Default, SpColNames[0], DBH_HasHeadings|DBH_ReadOnly|DBH_ForwardOnly|DBH_TrimStrings))

      {
      Ok=true;
      if (DB.CheckFieldNames(SpColNames))
        {
        Strng S, FriendOcc1, FriendOcc2;
        CStringArray Values;
        while (!DB.IsEOF())
          {
          DB.GetNextRow(Values);
          Strng Name(Values[0]);
          Strng Compound(Values[1]);
          Strng PhaseName(Values[2]);
          Strng OccName(Values[3]);
          Strng VPFunc(Values[4]);
          Strng Ts(Values[5]);
          Strng Te(Values[6]);
          S.Set("%s(%s)", Compound(), PhaseName());
          //byte Occ=0;
          FriendOcc1="";
          FriendOcc2="";
          SpCfgItem X;
          X.Src=Src;
          X.Occ=0;
          dword OccVal=0;
          if (OccName=="s")
            X.Occ = BOT_Solid; 
          else if (OccName=="l" || OccName=="aq")
            {
            FriendOcc1="g"; 
            X.Occ = BOT_Liquid; 
            }
          else if (OccName=="g")
            {
            FriendOcc1="l"; 
            FriendOcc2="aq"; 
            X.Occ = BOT_Gas; 
            }
          X.HasVP=(VPFunc.Length());
          X.m_Ts=SafeAtoF(Ts(), 0);
          X.m_Te=SafeAtoF(Te(), -1);
          X.m_Lo=25;
          X.m_Hi=25;
          //S.SetIndex(OccVal /*| (HasVP ? VPMask : 0) | (SrcMask&Src)*/);
          for (int f=0; f<m_Species.GetSize(); f++)
            if (m_Species[f].Comp.XStrCmp(S)==0)
              break;
          if (f>=m_Species.GetSize())
            {
            X.Comp=S;
            X.Name=Name;
            m_Species.Add(X);
            }
          // Try Find Friend
          if (FriendOcc1())
            {
            Strng Friend1, Friend2;
            Friend1.Set("%s(%s)", Compound(), FriendOcc1());
            Friend2.Set("%s(%s)", Compound(), FriendOcc2());

            for (int f=0; f<m_Species.GetSize(); f++)
              if (m_Species[f].Comp.XStrCmp(Friend1())==0 || m_Species[f].Comp.XStrCmp(Friend2())==0)
                break;
            if ((f<m_Species.GetSize()) && (X.HasVP || m_Species[f].HasVP))//Comp.Index()&VPMask)))
              {
              for (int i=0; i<m_FlashSpecies.GetSize(); i++)
                if (m_FlashSpecies[i].XStrCmp(Compound())==0)
                  break;
              if (i==m_FlashSpecies.GetSize())
                m_FlashSpecies.Add(Compound);
              }
            }
          }
        }
      else
        {
        LogError("SpecieDB", 0, "Incorrect field names: Table 'Species' in '%s'", Filename);
        Ok = false;
        }
      DB.Close();
      }
    else
      {
      LogError("SpecieDB", 0, "Unable to open Table 'Species' in '%s'", Filename);
      Ok = false;
      }
    }
  return Ok;
  }

//---------------------------------------------------------------------------

BOOL CMdlCfgSheet::OnInitDialog()
  {

  //INCOMPLETECODEMSG("CfgItemsC");
  m_SDBCfg.FnDrivePath(GetCfgFile());
  m_SDBCfg+=CfgDBFileName();

  if (FileExists(m_SDBCfg()))
    GetSpecieNames(m_SDBCfg(), 1);
  else
    m_SDBCfg="";
#if WITHDEFAULTSPDB
  SDBDef=BaseCfgFiles();
  SDBDef+=BCfgDBFileName();
  if (FileExists(SDBDef()))
    GetSpecieNames(SDBDef(), 0);
  else
    SDBDef="";
#endif

  if (pSfeBase)
    {
    RequestModelInfoRec Info;
    for (int iNo=0; pSfeBase->RequestModelInfoByGroupIndex("SpModel", iNo, Info); iNo++)
      {
      SpMdlInfo I(Info.ShortDesc(), Info.Class(), Info.MdlLibName());
      SpModels.Add(I);
      }
    }

  BOOL b = CPropertySheet::OnInitDialog();

  GetDlgItem(ID_APPLY_NOW)->ShowWindow(SW_HIDE);
  GetDlgItem(IDHELP)->ShowWindow(SW_HIDE);

  m_DefaultDesc.FnName(m_CfgFile());
  //CfgItems[0].StrDef=m_DefaultDesc();

  int iAct=ScdPFUser.RdInt("General", "MdlCfgSheet", -1);
  if (iAct>=0)
    SetActivePage(iAct);
  return b;
  }

//---------------------------------------------------------------------------

void CMdlCfgSheet::OnOK()
  {
  ASSERT_VALID(this);
  CPropertyPage * pAct=GetActivePage();
  int iAct=-1;
  for (int i=0; i<GetPageCount(); i++)
    {
    GetPage(i)->OnOK();
    if (pAct==GetPage(i))
      iAct=i;
    }

  if (GetActivePage()->OnKillActive())
    {
    char DT[64],TM[64];
    _strdate(DT);
    _strtime(TM);
    Cfg.WrStr("LastSaveInfo", "SysCADVersion", FullVersion());
    Cfg.WrStr("LastSaveInfo", "LastSaveDate", DT);
    Cfg.WrStr("LastSaveInfo", "LastSaveTime", TM);
    Cfg.WrStr("LastSaveInfo", "CfgFile", m_CfgFile());
    Cfg.WrStr("LastSaveInfo", "WinNT", IsWinNT ? "y" : "n");
    char Buff[512];
    DWORD Sz = sizeof(Buff);
    if (GetUserName(Buff, &Sz))
      Cfg.WrStr("LastSaveInfo", "UserName", Buff);
    Sz = sizeof(Buff);
    if (GetComputerName(Buff, &Sz))
      Cfg.WrStr("LastSaveInfo", "ComputerName", Buff);
    Cfg.Flush();
    if (!Move_FileEx(m_CfgFileTmp(), m_CfgFile(), MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH))
      LogError("EditCfg", LF_Exclamation, "%s not Saved", m_CfgFile());

    UnLoadDLLs();
    EndDialog(IDOK);
    }
  ScdPFUser.WrInt("General", "MdlCfgSheet", iAct);
  }

//---------------------------------------------------------------------------

void CMdlCfgSheet::OnCancel()
  {
  ASSERT_VALID(this);

  for (int i=0; i<GetPageCount(); i++)
    GetPage(i)->OnCancel();

  if (!::DeleteFile(m_CfgFileTmp())) 
    LogError("EditCfg", LF_Exclamation, "%s not Deleted", m_CfgFile());

  EndDialog(IDCANCEL);
  UnLoadDLLs();
  }

//---------------------------------------------------------------------------

void CMdlCfgSheet::OnClose()
  {
  ASSERT_VALID(this);
  OnCancel();
  }

//---------------------------------------------------------------------------

flag CMdlCfgSheet::IsStatsPage()
  {
  return FALSE;//(GetActivePage() && GetActivePage()->IsKindOf(RUNTIME_CLASS(CStatsPage)));
  }

//---------------------------------------------------------------------------

void CMdlCfgSheet::OnNcDestroy()
  {
  CPropertySheet::OnNcDestroy();
  //  delete this;
  }

//===========================================================================
//
//
//
//===========================================================================

CMdlCfgCfg::CMdlCfgCfg(CMdlCfgSheet * Sheet)
: CMdlCfgBase(CMdlCfgCfg::IDD, Sheet)
  {
  //{{AFX_DATA_INIT(CMdlCfgCfg)
  //m_AtmosPress = _T("");
  m_CfgFiles = _T("");
  m_Desc = _T("");
  m_MaxPress = 0.0;
  m_MaxTemp = 0.0;
  m_MinPress = 0.0;
  m_MinTemp = 0.0;
  m_NormPress = 0.0;
  m_NormTemp = 0.0;
  m_StdPress = 0.0;
  m_StdTemp = 0.0;
  m_bPBAllowed=true;
  m_bDynAllowed=true;
  m_bH2OAsAq=false;

  //}}AFX_DATA_INIT
  }

//---------------------------------------------------------------------------

void CMdlCfgCfg::DoDataExchange(CDataExchange* pDX)
  {
  CMdlCfgBase::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CMdlCfgCfg)
  DDX_Control(pDX, IDC_DEFNETMODE, m_DefNetMode);
  DDX_Control(pDX, IDC_PBNODEMODE, m_PBNodeMode);
  DDX_Control(pDX, IDC_PBFLOWMODE, m_PBFlowMode);
  DDX_Control(pDX, IDC_PBHEATMODE, m_PBHeatMode);
  DDX_Control(pDX, IDC_DYNNODEMODE, m_DynNodeMode);
  DDX_Control(pDX, IDC_DYNFLOWMODE, m_DynFlowMode);
  DDX_Control(pDX, IDC_DYNHEATMODE, m_DynHeatMode);
  DDX_Control(pDX, IDC_MAXNODEMODE, m_MaxNodeMode);
  DDX_Control(pDX, IDC_MAXFLOWMODE, m_MaxFlowMode);
  DDX_Control(pDX, IDC_MAXHEATMODE, m_MaxHeatMode);
  DDX_Control(pDX, IDC_DEFSPMDL, m_DefSpMdl);
  //DDX_Text(pDX, IDC_CFGFILES, m_CfgFiles);
  DDX_Text(pDX, IDC_CFGDESC, m_Desc);
  DDX_Text(pDX, IDC_MAXPRESS, m_MaxPress);
  DDV_MinMaxDouble(pDX, m_MaxPress, 50., 90000.);
  DDX_Text(pDX, IDC_MAXTEMP, m_MaxTemp);
  DDV_MinMaxDouble(pDX, m_MaxTemp, -100., 5000.);
  DDX_Text(pDX, IDC_MINPRESS, m_MinPress);
  DDV_MinMaxDouble(pDX, m_MinPress, 5., 22000.);
  DDX_Text(pDX, IDC_MINTEMP, m_MinTemp);
  DDV_MinMaxDouble(pDX, m_MinTemp, -273., 2000.);
  DDX_Text(pDX, IDC_NORMPRESS, m_NormPress);
  DDV_MinMaxDouble(pDX, m_NormPress, 50., 200.);
  DDX_Text(pDX, IDC_NORMTEMP, m_NormTemp);
  DDV_MinMaxDouble(pDX, m_NormTemp, -10., 100.);
  DDX_Text(pDX, IDC_STDPRESS, m_StdPress);
  DDV_MinMaxDouble(pDX, m_StdPress, 50., 200.);
  DDX_Text(pDX, IDC_STDTEMP, m_StdTemp);
  DDV_MinMaxDouble(pDX, m_StdTemp, -10., 100.);
  DDX_Check(pDX, IDC_PROBALALLOWED, m_bPBAllowed);
  DDX_Check(pDX, IDC_DYNAMICALLOWED, m_bDynAllowed);
  DDX_Check(pDX, IDC_H2OASAQ, m_bH2OAsAq);
  //}}AFX_DATA_MAP
  }

BEGIN_MESSAGE_MAP(CMdlCfgCfg, CMdlCfgBase)
  //{{AFX_MSG_MAP(CMdlCfgCfg)
  ON_CBN_SELCHANGE(IDC_DEFNETMODE, OnSelchangeNetMode)
  ON_CBN_SELCHANGE(IDC_MAXNODEMODE, OnSelchangeMaxNodeMode)
  ON_CBN_SELCHANGE(IDC_MAXFLOWMODE, OnSelchangeMaxFlowMode)
  ON_CBN_SELCHANGE(IDC_MAXHEATMODE, OnSelchangeMaxHeatMode)
  ON_BN_CLICKED(IDC_PROBALALLOWED, OnProbalAllowed)
  ON_BN_CLICKED(IDC_DYNAMICALLOWED, OnDynamicAllowed)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CMdlCfgCfg::OnInitDialog() 
  {
  CMdlCfgBase::OnInitDialog();

  m_FlashComp.AddString(NoneString);
  for (int f=0; f<m_FlashSpecies.GetSize(); f++)
    m_FlashComp.AddString(m_FlashSpecies[f]());

  for (int s=0; s<SpModels.GetSize(); s++)
    {
    Strng DllNm(SpModels[s].DLLName());
    DllNm += ".dll";
    int i = DLLs.FindByName(DllNm());
    if (i>=0 && DLLs[i].bLoad)
      {
      m_DefSpMdl.AddString(SpModels[s].ShortDesc());
      m_DefSpMdlIndex.Add(s);
      }
    }

  if (gs_License.ProBalOK())
    m_DefNetMode.AddString(GlblMode2LPTSTR(NM_Probal, NM_All, eGM_Name));
  if (gs_License.DynamicOK())
    m_DefNetMode.AddString(GlblMode2LPTSTR(NM_Dynamic, NM_All, eGM_Name));

  m_PBNodeMode.AddString(GlblMode2LPTSTR(SM_Direct, SM_All, eGM_Name));
  m_DynNodeMode.AddString(GlblMode2LPTSTR(SM_Direct, SM_All, eGM_Name));
  if (gs_License.DynamicOK())
    {
    m_DynNodeMode.AddString(GlblMode2LPTSTR(SM_Inline, SM_All, eGM_Name));
    m_DynNodeMode.AddString(GlblMode2LPTSTR(SM_Buffered, SM_All, eGM_Name));
    }

  m_PBFlowMode.AddString(GlblMode2LPTSTR(LFM_Xfer, LFM_All, eGM_Name));
  m_DynFlowMode.AddString(GlblMode2LPTSTR(LFM_Xfer, LFM_All, eGM_Name));
  if (gs_License.DynamicOK())
    {
    m_DynFlowMode.AddString(GlblMode2LPTSTR(LFM_Simple, LFM_All, eGM_Name));
    m_DynFlowMode.AddString(GlblMode2LPTSTR(LFM_Linear, LFM_All, eGM_Name));
    }
  if (gs_License.AllowDynamicFull())
    {
    m_DynFlowMode.AddString(GlblMode2LPTSTR(LFM_Full, LFM_All, eGM_Name));
    }

  m_PBHeatMode.AddString(GlblMode2LPTSTR(HM_None, HM_All, eGM_Name));
  m_PBHeatMode.AddString(GlblMode2LPTSTR(HM_Reduced, HM_All, eGM_Name));
  m_PBHeatMode.AddString(GlblMode2LPTSTR(HM_Full, HM_All, eGM_Name));

  m_DynHeatMode.AddString(GlblMode2LPTSTR(HM_None, HM_All, eGM_Name));
  m_DynHeatMode.AddString(GlblMode2LPTSTR(HM_Reduced, HM_All, eGM_Name));
  m_DynHeatMode.AddString(GlblMode2LPTSTR(HM_Full, HM_All, eGM_Name));

  char Buff[256], *pStr=Buff;
  for (int i=0; i<m_DynNodeMode.GetCount(); i++)
    {
    m_DynNodeMode.GetLBText(i, pStr);
    m_MaxNodeMode.AddString(pStr);
    }

  for (int i=0; i<m_DynFlowMode.GetCount(); i++)
    {
    m_DynFlowMode.GetLBText(i, pStr);
    m_MaxFlowMode.AddString(pStr);
    }
  for (int i=0; i<m_DynHeatMode.GetCount(); i++)
    {
    m_DynHeatMode.GetLBText(i, pStr);
    m_MaxHeatMode.AddString(pStr);
    }

  Strng Data,S;
  int iDM, iNetMd;
  int iPNodeMd, iPFlowMd, iPHeatMd;
  int iDNodeMd, iDFlowMd, iDHeatMd;
  int iNodeMx,  iFlowMx,  iHeatMx;

  for (int i=0; CfgItemsC[i].Name; i++)
    {
    DWORD Id=CfgItemsC[i].IdCtrl;
    switch (Id)
      {
      case 0: break;
      case IDC_DEFSPMDL:    iDM=i;      break;
      case IDC_DEFNETMODE:  iNetMd=i;   break;
      case IDC_DYNNODEMODE: iDNodeMd=i; break;
      case IDC_DYNLINKMODE:             break;
      case IDC_DYNFLOWMODE: iDFlowMd=i; break;
      case IDC_DYNHEATMODE: iDHeatMd=i; break;
      case IDC_PBNODEMODE:  iPNodeMd=i; break;
      case IDC_PBLINKMODE:              break;
      case IDC_PBFLOWMODE:  iPFlowMd=i; break;
      case IDC_PBHEATMODE:  iPHeatMd=i; break;
      case IDC_MAXNODEMODE: iNodeMx=i;  break;
      case IDC_MAXLINKMODE:             break;
      case IDC_MAXFLOWMODE: iFlowMx=i;  break;
      case IDC_MAXHEATMODE: iHeatMx=i;  break;
      default:
        {
        CEdit* pEdit=CfgItemsC[i].CtrlType==0 ? (CEdit*)GetDlgItem(Id) : NULL;
        if (CfgItemsC[i].IsDbl)
          {
          double D=Cfg.RdDouble(CfgItemsC[i].SctName, CfgItemsC[i].Name, CfgItemsC[i].DblDef);
          if (Id==IDC_STDTEMP || Id==IDC_NORMTEMP || Id==IDC_MINTEMP || Id==IDC_MAXTEMP)
            D=K_2_C(D);
          Data.Set("%.3f", D);
          pEdit->SetWindowText(Data());
          }
        else if (CfgItemsC[i].CtrlType==2)
          {
          BOOL b = (Cfg.RdInt(CfgItemsC[i].SctName, CfgItemsC[i].Name, (int)CfgItemsC[i].DblDef)!=0);
          if (CfgItemsC[i].Invert)
            b=!b;
          ((CButton*)GetDlgItem(Id))->SetCheck(b ? 1 : 0);
          }
        else
          {
          S=Cfg.RdStr(CfgItemsC[i].SctName, CfgItemsC[i].Name, CfgItemsC[i].StrDef);
          S.Trim();
          if (S.Length()==0)
            S=CfgItemsC[i].StrDef;
          pEdit->SetWindowText(S());
          }
        }      
      }
    }


  S=Cfg.RdStr(CfgItemsC[iDM].SctName, CfgItemsC[iDM].Name, CfgItemsC[iDM].StrDef);
  if (S.Length()==0)
#if WithSIMPLESPMDL 
    S=BaseSpModelName;
#else
    S=MassWtMnSpModelName;
#endif
  for (i=0; i<m_DefSpMdlIndex.GetSize(); i++)
    {
    if (S.XStrICmp(SpModels[m_DefSpMdlIndex[i]].Name())==0)
      break;
    }
  if (i==m_DefSpMdlIndex.GetSize())
    {
    LogWarning("EditCfg", 0, "Specie Properties Model '%s' not found in selected model DLL list", S());
    i=0;
    }
  int iSel=((CComboBox*)GetDlgItem(IDC_DEFSPMDL))->SetCurSel(i);


  S=Cfg.RdStr(CfgItemsC[iNetMd].SctName, CfgItemsC[iNetMd].Name, CfgItemsC[iNetMd].StrDef);
  S.Trim();
  if (S.Length()==0)
    iSel=m_DefNetMode.SetCurSel(0);
  else
    iSel=m_DefNetMode.SelectString(-1, S());
  if (iSel<0)
    iSel=m_DefNetMode.SetCurSel(0);


  S=Cfg.RdStr(CfgItemsC[iDNodeMd].SctName, CfgItemsC[iDNodeMd].Name, CfgItemsC[iDNodeMd].StrDef);
  S.Trim();
  if (S.Length()==0)
    iSel=m_DynNodeMode.SetCurSel(2);
  else
    iSel=m_DynNodeMode.SelectString(-1, S());
  if (iSel<0)
    iSel=m_DynNodeMode.SetCurSel(2);
  // Force to 'Buffered'
  iSel=m_DynNodeMode.SetCurSel(2);

  S=Cfg.RdStr(CfgItemsC[iDFlowMd].SctName, CfgItemsC[iDFlowMd].Name, CfgItemsC[iDFlowMd].StrDef);
  S.Trim();
  if (S.Length()==0)
    iSel=m_DynFlowMode.SetCurSel(2);
  else
    iSel=m_DynFlowMode.SelectString(-1, S());
  if (iSel<0)
    iSel=m_DynFlowMode.SetCurSel(2);

  S=Cfg.RdStr(CfgItemsC[iDHeatMd].SctName, CfgItemsC[iDHeatMd].Name, CfgItemsC[iDHeatMd].StrDef);
  S.Trim();
  if (S.Length()==0)
    iSel=m_DynHeatMode.SetCurSel(2);
  else
    iSel=m_DynHeatMode.SelectString(-1, S());
  if (iSel<0)
    iSel=m_DynHeatMode.SetCurSel(2);

  S=Cfg.RdStr(CfgItemsC[iPNodeMd].SctName, CfgItemsC[iPNodeMd].Name, CfgItemsC[iPNodeMd].StrDef);
  S.Trim();
  if (S.Length()==0)
    iSel=m_PBNodeMode.SetCurSel(2);
  else
    iSel=m_PBNodeMode.SelectString(-1, S());
  if (iSel<0)
    iSel=m_PBNodeMode.SetCurSel(2);

  S=Cfg.RdStr(CfgItemsC[iPFlowMd].SctName, CfgItemsC[iPFlowMd].Name, CfgItemsC[iPFlowMd].StrDef);
  S.Trim();
  if (S.Length()==0)
    iSel=m_PBFlowMode.SetCurSel(2);
  else
    iSel=m_PBFlowMode.SelectString(-1, S());
  if (iSel<0)
    iSel=m_PBFlowMode.SetCurSel(2);

  S=Cfg.RdStr(CfgItemsC[iPHeatMd].SctName, CfgItemsC[iPHeatMd].Name, CfgItemsC[iPHeatMd].StrDef);
  S.Trim();
  if (S.Length()==0)
    iSel=m_PBHeatMode.SetCurSel(2);
  else
    iSel=m_PBHeatMode.SelectString(-1, S());
  if (iSel<0)
    iSel=m_PBHeatMode.SetCurSel(2);


  S=Cfg.RdStr(CfgItemsC[iNodeMx].SctName, CfgItemsC[iNodeMx].Name, CfgItemsC[iNodeMx].StrDef);
  S.Trim();
  if (S.Length()==0)
    iSel=m_MaxNodeMode.SetCurSel(2);
  else
    iSel=m_MaxNodeMode.SelectString(-1, S());
  if (iSel<0)
    iSel=m_MaxNodeMode.SetCurSel(2);

  // Force to 'Buffered'
  iSel=m_MaxNodeMode.SetCurSel(2);

  S=Cfg.RdStr(CfgItemsC[iFlowMx].SctName, CfgItemsC[iFlowMx].Name, CfgItemsC[iFlowMx].StrDef);
  S.Trim();
  if (S.Length()==0)
    iSel=m_MaxFlowMode.SetCurSel(2);
  else
    iSel=m_MaxFlowMode.SelectString(-1, S());
  if (iSel<0)
    iSel=m_MaxFlowMode.SetCurSel(2);

  S=Cfg.RdStr(CfgItemsC[iHeatMx].SctName, CfgItemsC[iHeatMx].Name, CfgItemsC[iHeatMx].StrDef);
  S.Trim();
  if (S.Length()==0)
    iSel=m_MaxHeatMode.SetCurSel(2);
  else
    iSel=m_MaxHeatMode.SelectString(-1, S());
  if (iSel<0)
    iSel=m_MaxHeatMode.SetCurSel(2);

  int PrjFileVerNo=Cfg.RdInt("General", "PrjFileVersion", 0);

  m_bH2OAsAq = (PrjFileVerNo>=111);

  CheckModes();

  bInited=true;
  return TRUE;
  }

void CMdlCfgCfg::CheckModes()
  {
  //if (!m_bDynAllowed)
  //  m_bPBAllowed=true;
  //if (!m_bPBAllowed)
  //  m_bDynAllowed=true;

  int iNetMode = m_DefNetMode.GetCurSel();
  //bool IsDyn = iNetMode>0;

  int iPBNodeMode = m_PBNodeMode.GetCurSel();
  int iPBFlowMode = m_PBFlowMode.GetCurSel();
  int iPBHeatMode = m_PBHeatMode.GetCurSel();

  int iDynNodeMode = m_DynNodeMode.GetCurSel();
  int iDynFlowMode = m_DynFlowMode.GetCurSel();
  int iDynHeatMode = m_DynHeatMode.GetCurSel();

  int iMaxNodeMode = m_MaxNodeMode.GetCurSel();
  int iMaxFlowMode = m_MaxFlowMode.GetCurSel();
  int iMaxHeatMode = m_MaxHeatMode.GetCurSel();

  m_PBHeatMode.ResetContent();
  m_DynNodeMode.ResetContent();
  m_DynFlowMode.ResetContent();
  m_DynHeatMode.ResetContent();

  m_MaxNodeMode.ResetContent();
  m_MaxFlowMode.ResetContent();
  m_MaxHeatMode.ResetContent();

  m_DynNodeMode.AddString(GlblMode2LPTSTR(SM_Direct, SM_All, eGM_Name));
  if (gs_License.DynamicOK())
    {
    m_DynNodeMode.AddString(GlblMode2LPTSTR(SM_Inline, SM_All, eGM_Name));
    m_DynNodeMode.AddString(GlblMode2LPTSTR(SM_Buffered, SM_All, eGM_Name));
    }

  m_DynFlowMode.AddString(GlblMode2LPTSTR(LFM_Xfer, LFM_All, eGM_Name));
  if (gs_License.DynamicOK())
    {
    m_DynFlowMode.AddString(GlblMode2LPTSTR(LFM_Simple, LFM_All, eGM_Name));
    m_DynFlowMode.AddString(GlblMode2LPTSTR(LFM_Linear, LFM_All, eGM_Name));
    }
  if (gs_License.AllowDynamicFull())
    {
    m_DynFlowMode.AddString(GlblMode2LPTSTR(LFM_Full, LFM_All, eGM_Name));
    }

  m_DynHeatMode.AddString(GlblMode2LPTSTR(HM_None, HM_All, eGM_Name));
  m_DynHeatMode.AddString(GlblMode2LPTSTR(HM_Reduced, HM_All, eGM_Name));
  m_DynHeatMode.AddString(GlblMode2LPTSTR(HM_Full, HM_All, eGM_Name));

  m_PBHeatMode.AddString(GlblMode2LPTSTR(HM_None, HM_All, eGM_Name));
  m_PBHeatMode.AddString(GlblMode2LPTSTR(HM_Reduced, HM_All, eGM_Name));
  m_PBHeatMode.AddString(GlblMode2LPTSTR(HM_Full, HM_All, eGM_Name));

  char Buff[256], *pStr=Buff;
  for (int i=0; i<m_DynNodeMode.GetCount(); i++)
    {
    m_DynNodeMode.GetLBText(i, pStr);
    m_MaxNodeMode.AddString(pStr);
    }

  for (int i=0; i<m_DynFlowMode.GetCount(); i++)
    {
    m_DynFlowMode.GetLBText(i, pStr);
    m_MaxFlowMode.AddString(pStr);
    }
  for (int i=0; i<m_DynHeatMode.GetCount(); i++)
    {
    m_DynHeatMode.GetLBText(i, pStr);
    m_MaxHeatMode.AddString(pStr);
    }

  while (m_PBNodeMode.DeleteString(iMaxNodeMode+1)!=LB_ERR) {};
  while (m_PBFlowMode.DeleteString(iMaxFlowMode+1)!=LB_ERR) {};
  while (m_PBHeatMode.DeleteString(iMaxHeatMode+1)!=LB_ERR) {};

  m_PBNodeMode.SetCurSel(Min(iPBNodeMode, m_PBNodeMode.GetCount()-1));
  m_PBFlowMode.SetCurSel(Min(iPBFlowMode, m_PBFlowMode.GetCount()-1));
  m_PBHeatMode.SetCurSel(Min(iPBHeatMode, m_PBHeatMode.GetCount()-1));

  while (m_DynNodeMode.DeleteString(iMaxNodeMode+1)!=LB_ERR) {};
  while (m_DynFlowMode.DeleteString(iMaxFlowMode+1)!=LB_ERR) {};
  while (m_DynHeatMode.DeleteString(iMaxHeatMode+1)!=LB_ERR) {};

  m_DynNodeMode.SetCurSel(Min(iDynNodeMode, m_DynNodeMode.GetCount()-1));
  m_DynFlowMode.SetCurSel(Min(iDynFlowMode, m_DynFlowMode.GetCount()-1));
  m_DynHeatMode.SetCurSel(Min(iDynHeatMode, m_DynHeatMode.GetCount()-1));

  m_MaxNodeMode.SetCurSel(iMaxNodeMode);
  m_MaxFlowMode.SetCurSel(iMaxFlowMode);
  m_MaxHeatMode.SetCurSel(iMaxHeatMode);

  m_PBNodeMode.EnableWindow(0 && m_bPBAllowed);
  m_PBFlowMode.EnableWindow(0 && m_bPBAllowed);
  m_PBHeatMode.EnableWindow(m_bPBAllowed);

  m_DynNodeMode.EnableWindow(m_bDynAllowed);
  m_DynFlowMode.EnableWindow(m_bDynAllowed);
  m_DynHeatMode.EnableWindow(m_bDynAllowed);
  
  }

//---------------------------------------------------------------------------

void CMdlCfgCfg::OnOK() 
  {
  CMdlCfgBase::OnOK();
  }

//---------------------------------------------------------------------------

BOOL CMdlCfgCfg::OnSetActive() 
  {
  BOOL OK=CMdlCfgBase::OnSetActive();

  return OK;
  }

//---------------------------------------------------------------------------

BOOL CMdlCfgCfg::OnKillActive() 
  {
  BOOL OK=CMdlCfgBase::OnKillActive();
  if (bInited)
    {
    CString CS;
    Strng Data,S;
    int Md, iDM, iNetMd;
    int iPNodeMd, iPFlowMd, iPHeatMd;
    int iDNodeMd, iDFlowMd, iDHeatMd;
    int iNodeMx,  iFlowMx,  iHeatMx;
    for (int i=0; CfgItemsC[i].Name; i++)
      {
      DWORD Id=CfgItemsC[i].IdCtrl;
      switch (Id)
        {
        case 0: break;
        case IDC_DEFSPMDL:    iDM=i;      break;
        case IDC_DEFNETMODE:  iNetMd=i;   break;
        case IDC_PBNODEMODE:  iPNodeMd=i; break;
        case IDC_PBLINKMODE:              break;
        case IDC_PBFLOWMODE:  iPFlowMd=i; break;
        case IDC_PBHEATMODE:  iPHeatMd=i; break;
        case IDC_DYNNODEMODE: iDNodeMd=i; break;
        case IDC_DYNLINKMODE:             break;
        case IDC_DYNFLOWMODE: iDFlowMd=i; break;
        case IDC_DYNHEATMODE: iDHeatMd=i; break;
        case IDC_MAXNODEMODE: iNodeMx=i;  break;
        case IDC_MAXLINKMODE:             break;
        case IDC_MAXFLOWMODE: iFlowMx=i;  break;
        case IDC_MAXHEATMODE: iHeatMx=i;  break;
        default:
          {
          CWnd* pWnd=(CWnd*)GetDlgItem(Id);
          pWnd->GetWindowText(CS);
          if (CfgItemsC[i].IsDbl)
            {
            double D=SafeAtoF((const char*)CS);//Cfg.RdDouble("General", CfgItemsC[i].Name, CfgItemsC[i].DblDef);
            if (Id==IDC_STDTEMP || Id==IDC_NORMTEMP || Id==IDC_MINTEMP || Id==IDC_MAXTEMP)
              D=C_2_K(D);
            Cfg.WrDouble(CfgItemsC[i].SctName, CfgItemsC[i].Name, D);
            }
          else if (CfgItemsC[i].CtrlType==2)
            {
            BOOL b = ((CButton*)GetDlgItem(Id))->GetCheck()!=0 ? 1 : 0;
            if (CfgItemsC[i].Invert)
              b=!b;
            Cfg.WrInt(CfgItemsC[i].SctName, CfgItemsC[i].Name, b);
            }
          else
            {
            if (CS==NoneString)
              CS="";
            if ((/*Id==IDC_CFGFILES ||*/ Id==IDC_CFGHOME) && CS.GetLength()>0 && CS[CS.GetLength()-1]=='\\')
              CS.Left(CS.GetLength()-1);
            Cfg.WrStr(CfgItemsC[i].SctName, CfgItemsC[i].Name, NONNULL((const char*)CS));
            }
          }     
        }
      }

    int iSel;

    iSel=((CComboBox*)GetDlgItem(IDC_DEFSPMDL))->GetCurSel();
    S=(iSel>=0 ? SpModels[m_DefSpMdlIndex[iSel]].Name() : NoneString);
    Cfg.WrStr(CfgItemsC[iDM].SctName, CfgItemsC[iDM].Name, NONNULL(S()));

    iSel=m_DefNetMode.GetCurSel();
    Md=((iSel==0 || iSel==CB_ERR) ? NM_Probal : NM_Dynamic);
    S=GlblMode2LPTSTR(Md, NM_All, eGM_Name);
    Cfg.WrStr(CfgItemsC[iNetMd].SctName, CfgItemsC[iNetMd].Name, NONNULL(S()));

    iSel=m_PBNodeMode.GetCurSel();
    Md=((iSel==0 || iSel==CB_ERR) ? SM_Direct : (iSel==1 ? SM_Inline : SM_Buffered));
    S=GlblMode2LPTSTR(Md, SM_All, eGM_Name);
    Cfg.WrStr(CfgItemsC[iPNodeMd].SctName, CfgItemsC[iPNodeMd].Name, NONNULL(S()));

    iSel=m_PBFlowMode.GetCurSel();
    Md=((iSel==0 || iSel==CB_ERR) ? LFM_Xfer : (iSel==1 ? LFM_Simple : (iSel==2 ? LFM_Linear : LFM_Full)));
    S=GlblMode2LPTSTR(Md, LFM_All, eGM_Name);
    Cfg.WrStr(CfgItemsC[iPFlowMd].SctName, CfgItemsC[iPFlowMd].Name, NONNULL(S()));

    iSel=m_PBHeatMode.GetCurSel();
    Md=((iSel==2 || iSel==CB_ERR) ? HM_Full : (iSel==1 ? HM_Reduced : HM_None));
    S=GlblMode2LPTSTR(Md, HM_All, eGM_Name);
    Cfg.WrStr(CfgItemsC[iPHeatMd].SctName, CfgItemsC[iPHeatMd].Name, NONNULL(S()));

    iSel=m_DynNodeMode.GetCurSel();
    Md=((iSel==0 || iSel==CB_ERR) ? SM_Direct : (iSel==1 ? SM_Inline : SM_Buffered));
    Md=SM_Buffered; // Force to Buffered
    S=GlblMode2LPTSTR(Md, SM_All, eGM_Name);
    Cfg.WrStr(CfgItemsC[iDNodeMd].SctName, CfgItemsC[iDNodeMd].Name, NONNULL(S()));

    iSel=m_DynFlowMode.GetCurSel();
    Md=((iSel==0 || iSel==CB_ERR) ? LFM_Xfer : (iSel==1 ? LFM_Simple : (iSel==2 ? LFM_Linear : LFM_Full)));
    S=GlblMode2LPTSTR(Md, LFM_All, eGM_Name);
    Cfg.WrStr(CfgItemsC[iDFlowMd].SctName, CfgItemsC[iDFlowMd].Name, NONNULL(S()));

    iSel=m_DynHeatMode.GetCurSel();
    Md=((iSel==2 || iSel==CB_ERR) ? HM_Full : (iSel==1 ? HM_Reduced : HM_None));
    S=GlblMode2LPTSTR(Md, HM_All, eGM_Name);
    Cfg.WrStr(CfgItemsC[iDHeatMd].SctName, CfgItemsC[iDHeatMd].Name, NONNULL(S()));

    iSel=m_MaxNodeMode.GetCurSel();
    Md=((iSel==0 || iSel==CB_ERR) ? SM_Direct : (iSel==1 ? SM_Inline : SM_Buffered));
    Md=SM_Buffered; // Force to Buffered
    S=GlblMode2LPTSTR(Md, SM_All, eGM_Name);
    Cfg.WrStr(CfgItemsC[iNodeMx].SctName, CfgItemsC[iNodeMx].Name, NONNULL(S()));

    iSel=m_MaxFlowMode.GetCurSel();
    Md=((iSel==0 || iSel==CB_ERR) ? LFM_Xfer : (iSel==1 ? LFM_Simple : (iSel==2 ? LFM_Linear : LFM_Full)));
    S=GlblMode2LPTSTR(Md, LFM_All, eGM_Name);
    Cfg.WrStr(CfgItemsC[iFlowMx].SctName, CfgItemsC[iFlowMx].Name, NONNULL(S()));

    iSel=m_MaxHeatMode.GetCurSel();
    Md=((iSel==2 || iSel==CB_ERR) ? HM_Full : (iSel==1 ? HM_Reduced : HM_None));
    S=GlblMode2LPTSTR(Md, HM_All, eGM_Name);
    Cfg.WrStr(CfgItemsC[iHeatMx].SctName, CfgItemsC[iHeatMx].Name, NONNULL(S()));
    }  
  return OK;
  }

//---------------------------------------------------------------------------

void CMdlCfgCfg::OnSelchangeNetMode()     { CheckModes(); };
void CMdlCfgCfg::OnSelchangeMaxNodeMode() { CheckModes(); };
void CMdlCfgCfg::OnSelchangeMaxFlowMode() { CheckModes(); };
void CMdlCfgCfg::OnSelchangeMaxHeatMode() { CheckModes(); };

void CMdlCfgCfg::OnProbalAllowed() 
  {
  UpdateData(true);
  if (!m_bDynAllowed && !m_bPBAllowed)
    m_bDynAllowed=true;
  CheckModes();
  UpdateData(false);
  }
void CMdlCfgCfg::OnDynamicAllowed() 
  {
  UpdateData(true);
  if (!m_bDynAllowed && !m_bPBAllowed)
    m_bPBAllowed=true;
  CheckModes();
  UpdateData(false);
  }


//===========================================================================
//
//
//
//===========================================================================

CMdlCfgOpt::CMdlCfgOpt(CMdlCfgSheet * Sheet)
: CMdlCfgBase(CMdlCfgOpt::IDD, Sheet)
  {
  //{{AFX_DATA_INIT(CMdlCfgOpt)
  //}}AFX_DATA_INIT
  }

//---------------------------------------------------------------------------

void CMdlCfgOpt::DoDataExchange(CDataExchange* pDX)
  {
  CMdlCfgBase::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CMdlCfgOpt)
  //}}AFX_DATA_MAP
  }

BEGIN_MESSAGE_MAP(CMdlCfgOpt, CMdlCfgBase)
  //{{AFX_MSG_MAP(CMdlCfgOpt)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CMdlCfgOpt::OnInitDialog() 
  {
  CMdlCfgBase::OnInitDialog();

  //  m_FlashComp.AddString(NoneString);
  //  for (int f=0; f<FlashSpecies.GetSize(); f++)
  //    m_FlashComp.AddString(FlashSpecies[f]());
  //
  //  for (int s=0; s<SpModels.GetSize(); s++)
  //    m_DefSpMdl.AddString(SpModels[s].ShortDesc());

  Strng Data,S;
  //int iFC, iDM;
  for (int i=0; CfgItemsO[i].Name; i++)
    {
    DWORD Id=CfgItemsO[i].IdCtrl;
    switch (Id)
      {
      case 0: break;
        //case IDC_FLASHCOMP: iFC=i; break;
        //case IDC_DEFSPMDL: iDM=i; break;
      default:
        {
        CEdit* pEdit=CfgItemsO[i].CtrlType==0 ? (CEdit*)GetDlgItem(Id) : NULL;
        if (CfgItemsO[i].IsDbl)
          {
          double D=Cfg.RdDouble(CfgItemsO[i].SctName, CfgItemsO[i].Name, CfgItemsO[i].DblDef);
          if (Id==IDC_STDTEMP || Id==IDC_NORMTEMP || Id==IDC_MINTEMP || Id==IDC_MAXTEMP)
            D=K_2_C(D);
          Data.Set("%.2f", D);
          pEdit->SetWindowText(Data());
          }
        else if (CfgItemsO[i].CtrlType==2)
          {
          BOOL b = (Cfg.RdInt(CfgItemsO[i].SctName, CfgItemsO[i].Name, (int)CfgItemsO[i].DblDef)!=0);
          if (CfgItemsO[i].Invert)
            b=!b;
          ((CButton*)GetDlgItem(Id))->SetCheck(b ? 1 : 0);
          }
        else
          {
          S=Cfg.RdStr(CfgItemsO[i].SctName, CfgItemsO[i].Name, CfgItemsO[i].StrDef);
          S.Trim();
          if (S.Length()==0)
            S=CfgItemsO[i].StrDef;
          pEdit->SetWindowText(S());
          }
        }      
      }
    }

#if !WITHDEFAULTSPDB
  GetDlgItem(IDC_CONFIGDB)->EnableWindow(FALSE);
  GetDlgItem(IDC_CONFIGDB)->ShowWindow(SW_HIDE);
#endif
#if WITH_H2O_FNS_ALWAYS
  GetDlgItem(IDC_STDFNS4H2O)->EnableWindow(FALSE);
#else
  GetDlgItem(IDC_STDFNS4H2O)->EnableWindow(TRUE);
#endif
#if !WithNumericTags
  GetDlgItem(IDC_TAGSNUMERIC)->EnableWindow(FALSE);
  //GetDlgItem(IDC_TAGSNUMERIC)->ShowWindow(SW_HIDE);
#endif

  bInited=true;
  return TRUE;
  }

//---------------------------------------------------------------------------

void CMdlCfgOpt::OnOK() 
  {
  CMdlCfgBase::OnOK();
  }

//---------------------------------------------------------------------------

BOOL CMdlCfgOpt::OnSetActive() 
  {
  BOOL OK=CMdlCfgBase::OnSetActive();

  return OK;
  }

//---------------------------------------------------------------------------

BOOL CMdlCfgOpt::OnKillActive() 
  {
  BOOL OK=CMdlCfgBase::OnKillActive();
  if (bInited)
    {
    CString CS;
    Strng Data,S;
    //int iFC, iDM;
    for (int i=0; CfgItemsO[i].Name; i++)
      {
      DWORD Id=CfgItemsO[i].IdCtrl;
      switch (Id)
        {
        case 0: break;
          //case IDC_FLASHCOMP: iFC=i; break;
          //case IDC_DEFSPMDL: iDM=i; break;
        default:
          {
          CWnd* pWnd=(CWnd*)GetDlgItem(Id);
          pWnd->GetWindowText(CS);
          if (CfgItemsO[i].IsDbl)
            {
            double D=SafeAtoF((const char*)CS);//Cfg.RdDouble("General", CfgItemsO[i].Name, CfgItemsO[i].DblDef);
            if (Id==IDC_STDTEMP || Id==IDC_NORMTEMP || Id==IDC_MINTEMP || Id==IDC_MAXTEMP)
              D=C_2_K(D);
            Cfg.WrDouble(CfgItemsO[i].SctName, CfgItemsO[i].Name, D);
            }
          else if (CfgItemsO[i].CtrlType==2)
            {
            BOOL b = ((CButton*)GetDlgItem(Id))->GetCheck()!=0 ? 1 : 0;
            if (CfgItemsO[i].Invert)
              b=!b;
            Cfg.WrInt(CfgItemsO[i].SctName, CfgItemsO[i].Name, b);
            }
          else
            {
            if (CS==NoneString)
              CS="";
            if ((/*Id==IDC_CFGFILES ||*/ Id==IDC_CFGHOME) && CS.GetLength()>0 && CS[CS.GetLength()-1]=='\\')
              CS.Left(CS.GetLength()-1);
            Cfg.WrStr(CfgItemsO[i].SctName, CfgItemsO[i].Name, NONNULL((const char*)CS));
            }
          }     
        }
      }

#if !WITHDEFAULTSPDB
    int PrevVal = Cfg.RdInt("General", "SearchDefaultSpecieDB", 0);
    if (PrevVal)
      Cfg.WrInt("General", "SearchDefaultSpecieDB", 0);
#endif
    }  
  return OK;
  }

//===========================================================================
//
//
//
//===========================================================================
//
//CMdlCfgMdls::CMdlCfgMdls(CMdlCfgSheet * Sheet)
//: CMdlCfgBase(CMdlCfgMdls::IDD, Sheet)
//  {
//  //{{AFX_DATA_INIT(CMdlCfgMdls)
//  m_Sort = 1; 
//  //}}AFX_DATA_INIT
//  UseDLLTree = (m_Sort!=0);
//  //  m_DllPath=Cfg.RdStr("General", "DllPath", "")();
//  }
//
////---------------------------------------------------------------------------
//
//void CMdlCfgMdls::DoDataExchange(CDataExchange* pDX)
//  {
//  CMdlCfgBase::DoDataExchange(pDX);
//  //{{AFX_DATA_MAP(CMdlCfgMdls)
//  DDX_Control(pDX, IDC_DLLTREE, m_DllTree);
//  DDX_Radio(pDX, IDC_DLL_SORT_GROUP, m_Sort);
//  //}}AFX_DATA_MAP
//  }
//
//
//BEGIN_MESSAGE_MAP(CMdlCfgMdls, CMdlCfgBase)
//  //{{AFX_MSG_MAP(CMdlCfgMdls)
//  ON_BN_CLICKED(IDC_DLL_SORT_GROUP, OnSort)
//  ON_BN_CLICKED(IDC_DLL_SORT_DLL, OnSort)
//  //}}AFX_MSG_MAP
//END_MESSAGE_MAP()
//
////---------------------------------------------------------------------------
//
//BOOL CMdlCfgMdls::OnInitDialog() 
//  {
//  CMdlCfgBase::OnInitDialog();
//
//  dbgpln("=====================================");
//  for (int i=0; i<DLLs.GetSize(); i++)
//    {
//    dbgp("%s", DLLs[i].Name());
//    if (DLLs[i].Type==DT_Unloadable)
//      dbgp(" NOT LOADED");
//    else if (DLLs[i].Type==DT_Other)
//      dbgp(" OTHER");
//    dbgpln("");
//
//    POSITION pos = DLLs[i].Mdls.GetHeadPosition();
//    while (pos)
//      {
//      MdlInfo &M=DLLs[i].Mdls.GetNext(pos);
//      dbgpln("       %2i %2i %s %-12.12s  %s", M.iGroup, M.iBranch, M.fLicMdlBlocked ? "Lck" : "   ", M.Rec.Name(), M.Rec.ShortDesc());
//      }
//    }
//  dbgpln("=====================================");
//
//  m_DllTree.LoadImage();
//
//  BuildTree();
//
//  bInited=true;
//  return TRUE;
//  }
//
////---------------------------------------------------------------------------
//
//void CMdlCfgMdls::BuildTree()
//  {
//  Strng Tag, DllName, MdlName;
//  if (UseDLLTree)
//    RebuildDllTree(DLLs, m_DllTree);
//  else
//    RebuildDllGrpTree(DLLs, m_DllTree);
//
//  for (int Cnt=0;;)
//    {
//    Tag.Set("M%04i", Cnt++);
//    DllName=Cfg.RdStr("ModelDLLs", Tag(), "???");
//    if (DllName.XStrICmp("???")==0)
//      break;
//    for (int i=0; i<DLLs.GetSize(); i++)
//      if (DLLs[i].Name.XStrICmp(DllName)==0)
//        {
//        DLLs[i].bLoad = true;
//        break;
//        }
//    }
//
//  //mark all models that were previously selected...
//  int FailedCnt=0;
//  for (Cnt=0;;)
//    {
//    Tag.Set("S%04i",Cnt++);
//    MdlName=Cfg.RdStr("Selectable", Tag(), "???");
//    if (MdlName.XStrICmp("???")==0)
//      {
//      switch (FailedCnt)
//        {//force these to be selected
//        case 0: MdlName.Set("1,%s", BaseSpModelName); break;
//        case 1: MdlName = "1,Lockup"; break; //SQLockup
//        }
//      if (FailedCnt>1)
//        break;
//      FailedCnt++;
//      Cnt--;
//      }
//
//    int On=(MdlName.SafeAtoL(1)!=0);
//    char * pComma=MdlName.XStrChr(',');
//    MdlName=MdlName.Right(MdlName.Length()-(pComma-MdlName()+1));
//    MdlName.LRTrim();
//    if (MdlName())
//      {
//      BOOL Found=false;
//      for (int i=0; i<DLLs.GetSize() && !Found; i++)
//        {
//        POSITION pos = DLLs[i].Mdls.GetHeadPosition();
//        while (pos && !Found)
//          {
//          MdlInfo &M=DLLs[i].Mdls.GetNext(pos);
//          if (M.Rec.Name.XStrICmp(MdlName)==0)
//            {
//            m_DllTree.SetItemSelected(M.hItem, On);
//            Found = true;
//            }
//          }
//        }
//      }
//    }
//
//  //mark all models that are blocked by licensing...
//  for (int i=0; i<DLLs.GetSize(); i++)
//    {
//    POSITION pos = DLLs[i].Mdls.GetHeadPosition();
//    while (pos)
//      {
//      MdlInfo &M=DLLs[i].Mdls.GetNext(pos);
//      if (M.fLicMdlBlocked)// || M.fLicModeBlocked)
//        m_DllTree.SetItemLocked(M.hItem);
//      }
//    }
//
//  m_DllTree.FixAllLockImages();
//  m_DllTree.FixAllSelectedImages();
//  }
//
////---------------------------------------------------------------------------
//
//void CMdlCfgMdls::SaveTree()
//  {
//  if (UseDLLTree)
//    {
//    for (int i=0; i<DLLs.GetSize(); i++)
//      DLLs[i].bLoad = m_DllTree.GetItemSelected(DLLs[i].hItem, false);
//    }
//  else
//    {
//    for (int i=0; i<DLLs.GetSize(); i++)
//      {
//      POSITION pos = DLLs[i].Mdls.GetHeadPosition();
//      while (pos)
//        {
//        MdlInfo &M=DLLs[i].Mdls.GetNext(pos);
//        if (m_DllTree.GetItemSelected(M.hItem, false))
//          {
//          DLLs[i].bLoad = 1;
//          break;
//          }
//        }
//      }
//    }
//
//  int Cnt=0;
//  Strng Tag, Old, S;
//  for (int i=0; i<DLLs.GetSize(); i++)
//    if (DLLs[i].bLoad || _stricmp(DLLs[i].Name(), "flwlib.dll")==0)
//      {
//      Tag.Set("M%04i", Cnt++);
//      Cfg.WrStr("ModelDLLs", Tag(), DLLs[i].Name());
//      }
//    while (1)
//      {
//      Tag.Set("M%04i", Cnt++);
//      Old=Cfg.RdStr("ModelDLLs", Tag(), "???");
//      if (Old.XStrCmp("???")==0)
//        break;
//      Cfg.WrStr("ModelDLLs", Tag(), (char*)NULL);
//      }
//
//    Cnt=0;
//    for (int g=0; Groups[g].Name; g++)
//      {
//      Tag.Set("S%04i",Cnt++);
//      S.Set("0,%s", Groups[g].Name);
//      Cfg.WrStr("Selectable", Tag(), S());
//      for (int i=0; i<DLLs.GetSize(); i++)
//        {
//        if (DLLs[i].bLoad)
//          {
//          POSITION pos = DLLs[i].Mdls.GetHeadPosition();
//          while (pos)
//            {
//            MdlInfo &M=DLLs[i].Mdls.GetNext(pos);
//            if (M.iGroup==g)
//              {
//              Tag.Set("S%04i", Cnt++);
//              S.Set("%i,%s", m_DllTree.GetItemSelected(M.hItem, false)!=0, M.Rec.Name());
//              Cfg.WrStr("Selectable", Tag(), S());
//              }
//            }
//          }
//        }
//      }
//
//    while (1)
//      {
//      Tag.Set("S%04i", Cnt++);
//      Old=Cfg.RdStr("Selectable", Tag(), "???");
//      if (Old.XStrCmp("???")==0)
//        break;
//      Cfg.WrStr("Selectable", Tag(), (char*)NULL);
//      }
//  }
//
////---------------------------------------------------------------------------
//
//void CMdlCfgMdls::OnSort() 
//  {
//  UpdateData(true);
//  BOOL NewUseDLLTree = (m_Sort!=0);
//  if (NewUseDLLTree!=UseDLLTree)
//    {
//    SaveTree();
//    UseDLLTree = NewUseDLLTree;
//    BuildTree();
//    }
//  }
//
////---------------------------------------------------------------------------
//
//void CMdlCfgMdls::OnOK() 
//  {
//  CMdlCfgBase::OnOK();
//  }
//
////---------------------------------------------------------------------------
//
//BOOL CMdlCfgMdls::OnSetActive() 
//  {
//  BOOL OK=CMdlCfgBase::OnSetActive();
//
//  return OK;
//  }
//
////---------------------------------------------------------------------------
//
//BOOL CMdlCfgMdls::OnKillActive() 
//  {
//  BOOL OK=CMdlCfgBase::OnKillActive();
//  if (bInited)
//    {
//    SaveTree();
//    //Cfg.WrStr("General", "DllPath", (char*)(const char*)m_DllPath);
//    }
//
//  return OK;
//  }

//===========================================================================
//
//
//
//===========================================================================

CMdlCfgSpcs::CMdlCfgSpcs(CMdlCfgSheet * Sheet)
: CMdlCfgBase(CMdlCfgSpcs::IDD, Sheet)
  {
  //{{AFX_DATA_INIT(CMdlCfgSpcs)
  //m_LoTol = 0.0;
  //m_HiTol = 0.0;
  //m_LoDef = 0.0;
  //m_HiDef = 0.0;
  //m_LoAllow = 0.0;
  //m_HiAllow = 0.0;
  m_bUseIdeal = FALSE;
  //}}AFX_DATA_INIT
  iSpDefaults=0;
  }

//---------------------------------------------------------------------------

void CMdlCfgSpcs::DoDataExchange(CDataExchange* pDX)
  {
  CMdlCfgBase::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CMdlCfgSpcs)
  DDX_Control(pDX, IDC_FILTER, m_Filter);
  DDX_Control(pDX, IDC_SPCFGWHAT, m_What);
  DDX_Control(pDX, IDC_SPDEFAULTS, m_Defaults);
  DDX_Control(pDX, IDC_SPDBLIST, m_SpDBList);
  DDX_Control(pDX, IDC_SPCFGLIST, m_SpCfgList);
  DDX_Text(pDX, IDC_LOTOL, m_LoTol);
  //DDV_MinMaxDouble(pDX, m_LoTol, 0., 2000.);
  DDX_Text(pDX, IDC_HITOL, m_HiTol);
  //DDV_MinMaxDouble(pDX, m_HiTol, 0., 2000.);
  DDX_Text(pDX, IDC_LODEF, m_LoDef);
  DDX_Text(pDX, IDC_HIDEF, m_HiDef);
  DDX_Text(pDX, IDC_LOALLOW, m_LoAllow);
  DDX_Text(pDX, IDC_HIALLOW, m_HiAllow);
  DDX_Check(pDX, IDC_USEIDEAL, m_bUseIdeal);
  //}}AFX_DATA_MAP
  }


BEGIN_MESSAGE_MAP(CMdlCfgSpcs, CMdlCfgBase)
  //{{AFX_MSG_MAP(CMdlCfgSpcs)
  ON_BN_CLICKED(IDC_SPADD, OnSpadd)
  ON_BN_CLICKED(IDC_SPREMOVE, OnSpremove)
  ON_BN_CLICKED(IDC_SPMOVEUP, OnSpmoveup)
  ON_BN_CLICKED(IDC_SPMOVEDN, OnSpmovedn)
  ON_BN_CLICKED(IDC_SPSEPAR, OnSpsepar)
  ON_NOTIFY(NM_DBLCLK, IDC_SPDBLIST, OnDblclkSpdblist)
  ON_NOTIFY(NM_DBLCLK, IDC_SPCFGLIST, OnDblclkSpcfglist)
  ON_NOTIFY(LVN_ENDLABELEDIT, IDC_SPCFGLIST, OnEndlabeleditSpcfglist)
  ON_NOTIFY(NM_CLICK, IDC_SPCFGLIST, OnClickSpcfglist)
  ON_NOTIFY(NM_SETFOCUS, IDC_SPCFGLIST, OnSetfocusSpcfglist)
  ON_NOTIFY(LVN_DELETEITEM, IDC_SPCFGLIST, OnDeleteitemSpcfglist)
  ON_NOTIFY(LVN_INSERTITEM, IDC_SPCFGLIST, OnInsertitemSpcfglist)
  ON_NOTIFY(LVN_KEYDOWN, IDC_SPCFGLIST, OnKeydownSpcfglist)
  ON_BN_CLICKED(IDC_SPDEFAULTS, OnSpdefaults)
  ON_NOTIFY(LVN_KEYDOWN, IDC_SPDBLIST, OnKeydownSpdblist)
  ON_NOTIFY(NM_CLICK, IDC_SPDBLIST, OnClickSpdblist)
  ON_CBN_SELCHANGE(IDC_FILTER, OnSelchangeFilter)
  ON_EN_CHANGE(IDC_HITOL, OnChangeHitol)
  ON_EN_CHANGE(IDC_LOTOL, OnChangeLotol)
  ON_BN_CLICKED(IDC_USEIDEAL, OnUseideal)
  ON_NOTIFY(LVN_ITEMCHANGED, IDC_SPCFGLIST, OnItemchangedSpcfglist)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

static int SpDBTest(void * p, void * q) 
  { 
  SpCfgItem * r1=(SpCfgItem *)p;
  SpCfgItem * r2=(SpCfgItem *)q;
  char *c1=(r1->Comp)();
  char *c2=(r2->Comp)();
  while (*c1!=0 && !isalpha(*c1)) c1++;
  while (*c2!=0 && !isalpha(*c2)) c2++;
  return _stricmp(c1,c2)<0; 
  }

void CMdlCfgSpcs::BuildSpDBList()
  {
  m_SpDBList.LockWindowUpdate();

  //build list of all available species based on filter...
  int Filter = Max(0, m_Filter.GetCurSel());
  CArray <SpCfgItem*, SpCfgItem*>  SpOrder; //move to .h
  SpOrder.SetSize(m_Species.GetSize());
  for (int i=0; i<m_Species.GetSize(); i++)
    {
    m_Species[i].Index=i;
    SpOrder[i]=&m_Species[i];
    }

  if (m_Species.GetSize()>0)
    HpSort(i, (void**)&SpOrder[0], SpDBTest);

  m_SpDBList.DeleteAllItems();
  BOOL AddIt;
  for (i=0; i<SpOrder.GetSize(); i++)
    {
#if WITHDEFAULTSPDB
    if (bExcludeDefSpDB)
      {
#endif
      switch (Filter)
        {
        case  0: AddIt = (SpOrder[i]->Src>0); break;
        case  1: AddIt = (SpOrder[i]->Src>0) && (SpOrder[i]->Occ==BOT_Gas); break;
        case  2: AddIt = (SpOrder[i]->Src>0) && (SpOrder[i]->Occ==BOT_Liquid); break;
        case  3: AddIt = (SpOrder[i]->Src>0) && (SpOrder[i]->Occ==BOT_Solid); break;
        default: AddIt = true; break;
        }
#if WITHDEFAULTSPDB
      }
    else
      {
      switch (Filter)
        {
        case  0: AddIt = true; break;
        case  1: AddIt = (SpOrder[i]->Src>0); break;
        case  2: AddIt = (SpOrder[i]->Src==0); break;
        case  3: AddIt = (SpOrder[i]->Occ==BOT_Gas); break;
        case  4: AddIt = (SpOrder[i]->Occ==BOT_Liquid); break;
        case  5: AddIt = (SpOrder[i]->Occ==BOT_Solid); break;
        case  6: AddIt = (SpOrder[i]->Src>0) && (SpOrder[i]->Occ==BOT_Gas); break;
        case  7: AddIt = (SpOrder[i]->Src>0) && (SpOrder[i]->Occ==BOT_Liquid); break;
        case  8: AddIt = (SpOrder[i]->Src>0) && (SpOrder[i]->Occ==BOT_Solid); break;
        case  9: AddIt = (SpOrder[i]->Src==0) && (SpOrder[i]->Occ==BOT_Gas); break;
        case 10: AddIt = (SpOrder[i]->Src==0) && (SpOrder[i]->Occ==BOT_Liquid); break;
        case 11: AddIt = (SpOrder[i]->Src==0) && (SpOrder[i]->Occ==BOT_Solid); break;
        default: AddIt = true; break;
        }
      }
#endif
    if (AddIt)
      {
      int n=m_SpDBList.InsertItem(m_SpDBList.GetItemCount(), SpOrder[i]->Comp(), 0);
      m_SpDBList.SetItemData(n, SetIndex(SpOrder[i]->Index)|SetOcc(SpOrder[i]->Occ));
#if WITHDEFAULTSPDB
      m_SpDBList.SetItemText(n, 1, SpOrder[i]->Src>0 ? "Cfg" : "");
#endif
      }
    }

  //mark all selected species in list...
  LVFINDINFO FI;
  FI.flags=LVFI_STRING;

  //int CfgFilePrjFileVerNo=Cfg.RdInt("General", "PrjFileVersion", 0);

  Strng Tag, Tmp;
  for (i=0; ; i++)
    {
    Tag.Set("S%04i",i);
    Tmp = Cfg.RdStr("Species", Tag(), "");
    if (Tmp.Len()>2)
      {
      CSVColArray C;
      int nToks=ParseTokenList(Tmp(), C, ",");

      DWORD Data=0;
      DWORD Type=SafeAtoL(C[0], 0);
      Type=FixSpecieTypeId(Type, m_lCfgFilePrjFileVerNo);
      Strng Spc(C[1]);
      if (Spc() && Type==spl_SpecieId)
        {
        FI.psz=Spc();
        int iSrcPos=m_SpDBList.FindItem(&FI, -1);
        if (iSrcPos>=0)
          m_SpDBList.SetItemImage(iSrcPos, 1);
        }
      }
    else
      break;
    }

  iPrevFilter = Filter;
  m_SpDBList.UnlockWindowUpdate();
  }

//---------------------------------------------------------------------------

BOOL CMdlCfgSpcs::OnInitDialog() 
  {
  CMdlCfgBase::OnInitDialog();

  m_What.SetWindowText("");
  m_SpDBList.InsertColumn(0, "Specie",   LVCFMT_LEFT, 150);
#if WITHDEFAULTSPDB
  m_SpDBList.InsertColumn(1, "DB",       LVCFMT_LEFT, 35);
#endif
  //  m_SpDBList.InsertColumn(2, "Name",     LVCFMT_LEFT, 100);
  m_SpCfgList.InsertColumn(0, "Specie",  LVCFMT_LEFT, 150);
  m_SpCfgList.InsertColumn(1, "Ideal",   LVCFMT_RIGHT, 38);
  m_SpCfgList.InsertColumn(2, "LoTol",   LVCFMT_RIGHT, 0);
  m_SpCfgList.InsertColumn(3, "HiTol",   LVCFMT_RIGHT, 0);
  m_SpCfgList.InsertColumn(4, "LoTemp",  LVCFMT_RIGHT, 55);
  m_SpCfgList.InsertColumn(5, "HiTemp",  LVCFMT_RIGHT, 55);

  CBitmap BM;                                           
  VERIFY(BM.LoadBitmap(IDB_SPLISTIMGS));
  VERIFY(ImgList.Create(12, 12, FALSE, 0, 9));
  VERIFY(ImgList.Add(&BM, (CBitmap*)NULL)>=0);
  m_SpDBList.SetImageList(&ImgList, LVSIL_SMALL);
  m_SpCfgList.SetImageList(&ImgList, LVSIL_SMALL);

#if WITHDEFAULTSPDB
  bExcludeDefSpDB = !Cfg.RdInt("General", "SearchDefaultSpecieDB", 0);
#endif
  bNumericStartTagsBad = Cfg.RdInt("General", "NumericStartingTagsBad", 1);
#if WITH_H2O_FNS_ALWAYS
  bUseStdFns4H2O = Cfg.RdInt("General", "UseStdFns4H2O", 1);
#else
  bUseStdFns4H2O = Cfg.RdInt("General", "UseStdFns4H2O", 1);
#endif

  m_lCfgFilePrjFileVerNo=Cfg.RdInt("General", "PrjFileVersion", 0);

  ChangeSpDefDB();

  Strng Tmp;
  Tmp.Set("Default %d", iSpDefaults+1);
  SetDlgItemText(IDC_SPDEFAULTS, Tmp());
  EnableEditButtons();

  bInited=true;
  return TRUE;
  }

//---------------------------------------------------------------------------

static LPCTSTR DefaultIdealStr(long PrjFileVerNo) { return PrjFileVerNo>=65?"Y":""; };

//---------------------------------------------------------------------------

void CMdlCfgSpcs::OnOK() 
  {
  if (bInited)
    {
    }	
  CMdlCfgBase::OnOK();
  }

//---------------------------------------------------------------------------

void CMdlCfgSpcs::SaveSpList()
  {
  int Cnt=0;
  Cfg.WrInt("General", "PrjFileVersion", PrjFileVerNo());
  Strng Tag, Val, Old;
  for (int i=0; i<m_SpCfgList.GetItemCount(); i++)
    {
    CString S=m_SpCfgList.GetItemText(i,0);
    CString Ideal=m_SpCfgList.GetItemText(i,1);
    CString Lo=m_SpCfgList.GetItemText(i,2);
    CString Hi=m_SpCfgList.GetItemText(i,3);
    Ideal=(Ideal.GetLength()>0 && Ideal[0]=='Y')?"1":"0";
    DWORD D=GetType(m_SpCfgList.GetItemData(i));
    Tag.Set("S%04i",Cnt++);
    if (D==spl_SpecieId)
      Val.Set("%i,%s,%s,%s,%s",D,(LPCTSTR)S,(LPCTSTR)Lo,(LPCTSTR)Hi,(LPCTSTR)Ideal);
    else
      Val.Set("%i,%s",D,(LPCTSTR)S);
    Cfg.WrStr("Species", Tag(), Val());
    }
  while (1)
    {
    Tag.Set("S%04i",Cnt++);
    Old=Cfg.RdStr("Species", Tag(), "???");
    if (Old.XStrCmp("???")==0)
      break;
    Cfg.WrStr("Species", Tag(), (char*)NULL);
    }
  }

//---------------------------------------------------------------------------

void CMdlCfgSpcs::ChangeSpDefDB()
  {
  m_Filter.ResetContent();
  m_Filter.AddString("All");
#if WITHDEFAULTSPDB
  if (!bExcludeDefSpDB)
    {
    m_Filter.AddString("Config DB");
    m_Filter.AddString("Default DB");
    }
#endif
  m_Filter.AddString("All vapours");
  m_Filter.AddString("All liquids");
  m_Filter.AddString("All solids");
#if WITHDEFAULTSPDB
  if (!bExcludeDefSpDB)
    {
    m_Filter.AddString("Config DB vapours");
    m_Filter.AddString("Config DB liquids");
    m_Filter.AddString("Config DB solids");
    m_Filter.AddString("Default DB vapours");
    m_Filter.AddString("Default DB liquids");
    m_Filter.AddString("Default DB solids");
    }
#endif
  m_Filter.SetCurSel(0);

  BuildSpDBList(); //filter MUST be all for first attempt

  //mark images of species in user specie list...
  m_SpCfgList.DeleteAllItems();
  LVFINDINFO FI;
  FI.flags=LVFI_STRING;

  flag bNumericStartTagsBad = (Cfg.RdInt("General", "NumericStartingTagsBad", 1)!=0);
  //long CfgFilePrjFileVerNo=Cfg.RdInt("General", "PrjFileVersion", 0);

  Strng Tag, Tmp;
  int Cnt=0;
  for (int i=0; ; i++)
    {
    Tag.Set("S%04i",i);
    Tmp=Cfg.RdStr("Species", Tag(), "");
    if (Tmp.Length()>2)
      {
      CSVColArray C;
      int nToks=ParseTokenList(Tmp(), C, ",");

      DWORD Data=0;
      long Type=SafeAtoL(C[0], 0);
      Type=FixSpecieTypeId(Type, m_lCfgFilePrjFileVerNo);

      Strng Spc(C[1]);
      if (Spc())
        {
        char Buff[512];
        int iImgNo=0;
#if WithNumericTags
        int TagErr=TaggedObject::TestValidTag(Spc(), bNumericStartTagsBad, true);
#else
        int TagErr=TaggedObject::TestValidTag(Spc(), bNumericStartTagsBad);
#endif
        switch (TagErr)
          {
          case 0:
            iImgNo = 1;
            break;
          case 2:
            sprintf(Buff, "Specie Tag '%s' starts with a Digit\n\nIgnore, remove, rename or change tag-rules option.", Spc());
            AfxMessageBox(Buff, MB_OK|MB_ICONEXCLAMATION);
            iImgNo = 6;
            break;
          default:
            sprintf(Buff, "Invalid Specie Tag '%s'", Spc());
            AfxMessageBox(Buff, MB_OK|MB_ICONEXCLAMATION);
            iImgNo = 7;
            break;
          }

        FI.psz=Spc();
        Strng LoTol(nToks>2 ? C[2]:"0");
        Strng HiTol(nToks>3 ? C[3]:"0");
        Strng Ideal(nToks>4 ? C[4]:m_lCfgFilePrjFileVerNo>=65?"1":"0");

        Ideal=(Ideal.GetLength()>0 && Ideal[0]=='1')?"Yes":"";
        int iDstPos=m_SpCfgList.FindItem(&FI, -1);
        if (iDstPos<0 || Type==spl_Text || Type==spl_Page)
          { // Not in list, add it
          int iSrcPos=m_SpDBList.FindItem(&FI, -1);
          switch (Type)
            {
            case spl_SpecieId:
              if (iSrcPos>=0)
                Data = m_SpDBList.GetItemData(iSrcPos);
              else
                iImgNo = 2;
              break;
            case spl_SolSum:
            case spl_LiqSum:
            case spl_VapSum:   
            case spl_AllSum:   
              iImgNo = 3;
              break;
            case spl_Text:
              iImgNo = 4;
              break;
            case spl_Page:
              iImgNo = 5;
              break;
            }
          m_SpCfgList.InsertItem(Cnt, Spc(), iImgNo);
          m_SpCfgList.SetItemData(Cnt, SetType(Type)|Data);
          if (Type==spl_SpecieId)
            {
            Strng LoTmp("???");
            Strng HiTmp("???");
            int iSpc=m_Species.FindBySpecieName(Spc());
            if (iSpc>=0 && m_Species[iSpc].m_Ts<m_Species[iSpc].m_Te)    
              {
              double TLo= K2C(m_Species[iSpc].m_Ts)-SafeAtoF(LoTol, 0);
              LoTmp.Set("%.2f", TLo);
              double THi= K2C(m_Species[iSpc].m_Te)+SafeAtoF(HiTol, 0);
              HiTmp.Set("%.2f", THi);
              }

            m_SpCfgList.SetItem(Cnt, 1, LVIF_TEXT, Ideal(),0,0,0,0);
            m_SpCfgList.SetItem(Cnt, 2, LVIF_TEXT, LoTol(),0,0,0,0);
            m_SpCfgList.SetItem(Cnt, 3, LVIF_TEXT, HiTol(),0,0,0,0);
            m_SpCfgList.SetItem(Cnt, 4, LVIF_TEXT, LoTmp(),0,0,0,0);
            m_SpCfgList.SetItem(Cnt, 5, LVIF_TEXT, HiTmp(),0,0,0,0);
            }
          Cnt++;                
          }
        }
      }
    else
      break;
    }

  for (int i=0; i<m_DllRqdSpecies.GetSize(); i++)
    {
    //dbgpln("xx %i %s", m_DllRqdSpecies[i].Index(), m_DllRqdSpecies[i].Str());
    LVFINDINFO FI;
    FI.flags=LVFI_STRING;
    FI.psz=m_DllRqdSpecies[i]();
    int iDstPos=m_SpCfgList.FindItem(&FI, -1);
    if (iDstPos<0)
      {
      DWORD Data=0;
      long Type=0;
      int iImgNo=8;
      m_SpCfgList.InsertItem(Cnt, m_DllRqdSpecies[i](), iImgNo);
      m_SpCfgList.SetItemData(Cnt, SetType(Type)|Data);
      Cnt++;
      }
    }

#if WITHDEFAULTSPDB
  Strng s;
  s = "Config DB:";
  s += SDBCfg();
  m_SpCfgDB.SetWindowText(s());
  s = "Default DB:";
  s += SDBDef();
  if (bExcludeDefSpDB)
    s += "   (Excluded!)";
  m_SpDefDB.SetWindowText(s());

  m_SpCfgDB.SetWindowText(m_SDBCfg());
  m_SpDefDB.SetWindowText("");
#endif

#if WITHDEFAULTSPDB
  bPrevExcludeDefSpDB = bExcludeDefSpDB;
#endif
  bPrevNumericStartTagsBad = bNumericStartTagsBad;
  }

//---------------------------------------------------------------------------

BOOL CMdlCfgSpcs::OnSetActive() 
  {
  BOOL OK=CMdlCfgBase::OnSetActive();

  if (bInited)
    {
#if WITHDEFAULTSPDB
    bExcludeDefSpDB = !Cfg.RdInt("General", "SearchDefaultSpecieDB", 0);
#endif
    bNumericStartTagsBad = Cfg.RdInt("General", "NumericStartingTagsBad", 1);
#if WITH_H2O_FNS_ALWAYS   
    bUseStdFns4H2O = Cfg.RdInt("General", "UseStdFns4H2O", 1);
#else                   
    bUseStdFns4H2O = Cfg.RdInt("General", "UseStdFns4H2O", 1);
#endif                 

#if WITHDEFAULTSPDB
    if (bPrevExcludeDefSpDB != bExcludeDefSpDB ||
      bPrevNumericStartTagsBad != bNumericStartTagsBad)
#else
    if (bPrevNumericStartTagsBad != bNumericStartTagsBad)
#endif
      ChangeSpDefDB();
    }

  return OK;
  }

//---------------------------------------------------------------------------

BOOL CMdlCfgSpcs::OnKillActive() 
  {
  BOOL OK=CMdlCfgBase::OnKillActive();

  if (bInited)
    {
    SaveSpList();

#if !WITHDEFAULTSPDB
    int PrevVal = Cfg.RdInt("General", "SearchDefaultSpecieDB", 0);
    if (PrevVal)
      Cfg.WrInt("General", "SearchDefaultSpecieDB", 0);
#endif
    }

  return OK;
  }

//---------------------------------------------------------------------------

void CMdlCfgSpcs::OnSelchangeFilter() 
  {
  int Filter = m_Filter.GetCurSel();
  if (Filter!=iPrevFilter)
    {
    SaveSpList();
    BuildSpDBList();
    }
  }

//---------------------------------------------------------------------------

void CMdlCfgSpcs::OnSpadd() 
  {
  int OneDone=false;
  m_SpCfgList.LockWindowUpdate();
  int j=Max(0, m_SpCfgList.GetFirstSelectedItem());//+1);
  POSITION pos = m_SpDBList.GetFirstSelectedItemPosition();
  int iLastSel=-1;
  while (pos)
    {
    iLastSel=m_SpDBList.GetNextSelectedItem(pos);
    CString S=m_SpDBList.GetItemText(iLastSel,0);

    int iPos=m_SpCfgList.FindStringExact(-1, S);
    if (iPos<0) // Not Found
      {
      int n=m_SpCfgList.InsertItem(j++, S, 1);
      m_SpCfgList.SetItemData(n, SetType(spl_SpecieId)|m_SpDBList.GetItemData(iLastSel));
      m_SpDBList.SetItemImage(iLastSel, 1);
      m_SpCfgList.SetItem(n, 1, LVIF_TEXT, m_lCfgFilePrjFileVerNo>=65?"Y":"", 0,0,0,0);
      m_SpCfgList.SetItem(n, 2, LVIF_TEXT, "25", 0,0,0,0);
      m_SpCfgList.SetItem(n, 3, LVIF_TEXT, "25", 0,0,0,0);

      Strng LoTmp("???");
      Strng HiTmp("???");
      int iSpc=m_Species.FindBySpecieName((LPTSTR)(LPCTSTR)S);
      if (iSpc>=0 && m_Species[iSpc].m_Ts<m_Species[iSpc].m_Te)    
        {
        double TLo= K2C(m_Species[iSpc].m_Ts)-25;
        LoTmp.Set("%.2f", TLo);
        double THi= K2C(m_Species[iSpc].m_Te)+25;
        HiTmp.Set("%.2f", THi);
        }

      m_SpCfgList.SetItem(n, 4, LVIF_TEXT, LoTmp(), 0,0,0,0);
      m_SpCfgList.SetItem(n, 5, LVIF_TEXT, HiTmp(), 0,0,0,0);
      OneDone=true;
      }
    }

  if (OneDone)
    {
    m_SpCfgList.SetItemState(-1, 0, LVIS_SELECTED);
    m_SpCfgList.SetItemState(j-1, LVIS_SELECTED, LVIS_SELECTED);
    m_SpCfgList.EnsureVisible(j-1, false);
    m_SpCfgList.UnlockWindowUpdate();
    if (iLastSel)
      {
      m_SpDBList.SetItemState(-1, 0, LVIS_SELECTED);
      m_SpDBList.SetItemState(iLastSel, LVIS_SELECTED, LVIS_SELECTED);
      }
    }
  else
    m_SpCfgList.UnlockWindowUpdate();
  }

//---------------------------------------------------------------------------

void CMdlCfgSpcs::OnSpremove() 
  {
  m_SpCfgList.LockWindowUpdate();
  int iFirst=m_SpCfgList.GetItemCount();
  int j;
  while ((j=m_SpCfgList.GetFirstSelectedItem())>=0)
    {
    DWORD Type=GetType(m_SpCfgList.GetItemData(j));
    if (Type==spl_SpecieId)
      {
      CString S=m_SpCfgList.GetItemText(j,0);
      int iPos=m_SpDBList.FindStringExact(-1, S);
      if (iPos>=0)
        m_SpDBList.SetItemImage(iPos, 0);
      }

    int T=GetType(m_SpCfgList.GetItemData(j));
    if (T==spl_SpecieId || T==spl_Text || T==spl_Page)
      m_SpCfgList.DeleteItem(j);
    else
      m_SpCfgList.SetItemState(j, 0, LVIS_SELECTED);
    iFirst=Min(iFirst, j);
    }
  if (iFirst<m_SpCfgList.GetItemCount())
    {
    m_SpCfgList.SetItemState(iFirst, LVIS_SELECTED, LVIS_SELECTED);
    m_SpCfgList.EnsureVisible(iFirst, false);
    }
  m_SpCfgList.UnlockWindowUpdate();
  }

//---------------------------------------------------------------------------

void CMdlCfgSpcs::OnSpmoveup() 
  {
  int aMove[1000];
  int iMove=0;
  int iDst=m_SpCfgList.GetItemCount();

  POSITION pos = m_SpCfgList.GetFirstSelectedItemPosition();
  while (pos && iMove<1000)
    {
    aMove[iMove]=m_SpCfgList.GetNextSelectedItem(pos);
    iDst=Min(iDst,aMove[iMove]);
    iMove++;
    }

  char Buff[6][256];
  iDst=Max(0, iDst-1);
  int iFirst=iDst;
  m_SpCfgList.LockWindowUpdate();
  for (int j=0; j<iMove; j++, iDst++)
    {
    LVITEM LVI[6];
    for (int k=0; k<6; k++)
      {
      LVI[k].mask=LVIF_TEXT|(k==0 ? LVIF_IMAGE|LVIF_STATE|LVIF_PARAM : 0);
      LVI[k].iItem=aMove[j];
      LVI[k].iSubItem=k;
      LVI[k].stateMask=0xFFFFFFFF;
      LVI[k].pszText=Buff[k];
      LVI[k].cchTextMax=sizeof(Buff[k]);
      m_SpCfgList.GetItem(&LVI[k]);
      }

    m_SpCfgList.DeleteItem(aMove[j]);

    LVI[0].iItem=iDst;
    int i=m_SpCfgList.InsertItem(&LVI[0]);
    for (int k=1; k<6; k++)
      {
      LVI[k].iItem=i;
      m_SpCfgList.SetItem(&LVI[k]);
      }
    UpdateTTols(i, GetIndex(LVI[0].lParam));
    }

  m_SpCfgList.EnsureVisible(iFirst, false);
  m_SpCfgList.UnlockWindowUpdate();
  }

//---------------------------------------------------------------------------

void CMdlCfgSpcs::OnSpmovedn() 
  {
  int aMove[1000];
  int iMove=0;
  int iDst=-1;

  POSITION pos = m_SpCfgList.GetFirstSelectedItemPosition();
  while (pos && iMove<1000)
    {
    aMove[iMove]=m_SpCfgList.GetNextSelectedItem(pos);
    iDst=Max(iDst,aMove[iMove]);
    iMove++;
    }

  char Buff[6][256];
  m_SpCfgList.LockWindowUpdate();
  iDst=Min(m_SpCfgList.GetItemCount()-1, iDst+1);
  int iFirst=iDst;
  for (int j=iMove-1; j>=0; j--, iDst--)
    {
    LVITEM LVI[6];
    for (int k=0; k<6; k++)
      {
      LVI[k].mask=LVIF_TEXT|(k==0 ? LVIF_IMAGE|LVIF_STATE|LVIF_PARAM : 0);
      LVI[k].iItem=aMove[j];
      LVI[k].iSubItem=k;
      LVI[k].stateMask=0xFFFFFFFF;
      LVI[k].pszText=Buff[k];
      LVI[k].cchTextMax=sizeof(Buff[k]);
      m_SpCfgList.GetItem(&LVI[k]);
      }

    m_SpCfgList.DeleteItem(aMove[j]);

    LVI[0].iItem=iDst;
    int i=m_SpCfgList.InsertItem(&LVI[0]);
    for (int k=1; k<6; k++)
      {
      LVI[k].iItem=i;
      m_SpCfgList.SetItem(&LVI[k]);
      }
    }


  m_SpCfgList.EnsureVisible(iFirst, false);
  //UpdateTTols(iFirst, 
  m_SpCfgList.UnlockWindowUpdate();
  }

//---------------------------------------------------------------------------

void CMdlCfgSpcs::OnSpsepar() 
  {
  int j=Max(0, m_SpCfgList.GetFirstSelectedItem());//+1);
  CString S("--------------");
  int n=m_SpCfgList.InsertItem(j, S, 4);

  m_SpCfgList.SetItemState(-1, 0, LVIS_SELECTED);
  m_SpCfgList.SetItemData(n, spl_Text);
  m_SpCfgList.SetItemState(n, LVIS_SELECTED, LVIS_SELECTED);
  m_SpCfgList.EnsureVisible(n, false);
  m_SpCfgList.SetFocus();	
  m_SpCfgList.EditLabel(n);	
  }

//---------------------------------------------------------------------------

void CMdlCfgSpcs::OnDblclkSpdblist(NMHDR* pNMHDR, LRESULT* pResult) 
  {
  OnSpadd();
  *pResult = 0;
  }

//---------------------------------------------------------------------------

void CMdlCfgSpcs::OnDblclkSpcfglist(NMHDR* pNMHDR, LRESULT* pResult) 
  {
  int j=m_SpCfgList.GetFirstSelectedItem();
  if (j>=0)
    {
    m_SpCfgList.SetItemState(-1, 0, LVIS_SELECTED);
    m_SpCfgList.SetItemState(j, LVIS_SELECTED, LVIS_SELECTED);
    m_SpCfgList.EnsureVisible(j, false);
    DWORD Data=GetType(m_SpCfgList.GetItemData(j));	
    if (Data>spl_SpecieId)
      m_SpCfgList.EditLabel(j);	
    }
  *pResult = 0;
  }

//---------------------------------------------------------------------------

void CMdlCfgSpcs::OnEndlabeleditSpcfglist(NMHDR* pNMHDR, LRESULT* pResult) 
  {
  LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
  CEdit *pEdit=m_SpCfgList.GetEditControl();
  if (pEdit)
    {
    CString S;
    pEdit->GetWindowText(S);
    int j=m_SpCfgList.GetFirstSelectedItem();
    if (j>=0)
      m_SpCfgList.SetItemText(j, 0, S);
    }

  *pResult = 0;
  }

//---------------------------------------------------------------------------

void CMdlCfgSpcs::OnSetfocusSpcfglist(NMHDR* pNMHDR, LRESULT* pResult) 
  {
  EnableEditButtons();
  *pResult = 0;
  }

//---------------------------------------------------------------------------

void CMdlCfgSpcs::OnDeleteitemSpcfglist(NMHDR* pNMHDR, LRESULT* pResult) 
  {
  NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
  EnableEditButtons();	
  *pResult = 0;
  }

//---------------------------------------------------------------------------

void CMdlCfgSpcs::OnInsertitemSpcfglist(NMHDR* pNMHDR, LRESULT* pResult) 
  {
  NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
  EnableEditButtons();	
  *pResult = 0;
  }

//---------------------------------------------------------------------------

void CMdlCfgSpcs::EnableEditButtons()
  {
  POSITION DBpos = m_SpDBList.GetFirstSelectedItemPosition();
  int nSel=0, iSel=-1, iFirst=-1;
  POSITION pos = m_SpCfgList.GetFirstSelectedItemPosition();
  if (pos)
    {
    iSel=m_SpCfgList.GetNextSelectedItem(pos);
    iFirst=iSel;
    nSel++;
    }
  while (pos && nSel<2)
    {
    iSel=m_SpCfgList.GetNextSelectedItem(pos);
    nSel++;
    }

  GetDlgItem(IDC_SPADD)->EnableWindow(DBpos!=NULL);
  GetDlgItem(IDC_SPREMOVE)->EnableWindow(iSel>=0);
  GetDlgItem(IDC_SPMOVEUP)->EnableWindow(iFirst>0);
  GetDlgItem(IDC_SPMOVEDN)->EnableWindow(iSel<m_SpCfgList.GetItemCount()-1 && iSel>=0);

  UINT On=(iSel>=0 && 
           m_SpCfgList.GetSelectedCount()==1 &&
           GetType(m_SpCfgList.GetItemData(iSel))==spl_SpecieId) ? 1:0;
  GetDlgItem(IDC_USEIDEAL)->EnableWindow(TRUE);
  GetDlgItem(IDC_USEIDEAL)->EnableWindow(On && GetOcc(m_SpCfgList.GetItemData(iSel))==BOT_Gas);

  GetDlgItem(IDC_LOTOL)->EnableWindow(On);
  GetDlgItem(IDC_HITOL)->EnableWindow(On);
  GetDlgItem(IDC_LODEF)->EnableWindow(On);
  GetDlgItem(IDC_HIDEF)->EnableWindow(On);
  GetDlgItem(IDC_LOALLOW)->EnableWindow(On);
  GetDlgItem(IDC_HIALLOW)->EnableWindow(On);

  //UINT Show=(iSel>=0 && GetType(m_SpCfgList.GetItemData(iSel))==spl_SpecieId) ? SW_SHOW:SW_HIDE;
  //GetDlgItem(IDC_USEIDEAL)->EnableWindow(TRUE);
  //GetDlgItem(IDC_USEIDEAL)->ShowWindow(Show);
  //if (Show==SW_SHOW)
  //  GetDlgItem(IDC_USEIDEAL)->EnableWindow(GetOcc(m_SpCfgList.GetItemData(iSel))==BOT_Gas);

  //GetDlgItem(IDC_LOTOL)->ShowWindow(Show);
  //GetDlgItem(IDC_HITOL)->ShowWindow(Show);
  //GetDlgItem(IDC_LODEF)->ShowWindow(Show);
  //GetDlgItem(IDC_HIDEF)->ShowWindow(Show);
  //GetDlgItem(IDC_LOALLOW)->ShowWindow(Show);
  //GetDlgItem(IDC_HIALLOW)->ShowWindow(Show);
  //GetDlgItem(IDC_TEMPEXLOSTATIC)->ShowWindow(Show);
  //GetDlgItem(IDC_TEMPEXHISTATIC)->ShowWindow(Show);
  }

//---------------------------------------------------------------------------

void CMdlCfgSpcs::UpdateWhat(UINT Type, LPCTSTR Comp, int Index)
  {
  CString S;
  switch (Type)
    {
    case spl_VapSum:  S= "Sum of all Vapours"; break;
    case spl_LiqSum:  S= "Sum of all Liquids"; break;
    case spl_SolSum:  S= "Sum of all Solids";  break;
    case spl_AllSum:  S= "Sum of all Phases"; break;
    case spl_Text:    S= "Text Label"; break;
    case spl_SpecieId:
      {
      if (Index>=0 && Index<m_Species.GetSize() && m_Species[Index].Name())
        {
        S+= m_Species[Index].Name();
        S+= "    \n";
        }
      S+=Comp;
      }
    }

  m_What.SetWindowText(S);
  }

//---------------------------------------------------------------------------

void CMdlCfgSpcs::UpdateTTols(int ListIndex, int SpcIndex)
  {

  CString Spc=m_SpCfgList.GetItemText(ListIndex,0);

  if (SpcIndex>=0 && m_SpCfgList.GetSelectedCount()==1)
    {
    m_LoTol.Format("%.2f", SafeAtoF(m_SpCfgList.GetItemText(ListIndex,2)));
    m_HiTol.Format("%.2f", SafeAtoF(m_SpCfgList.GetItemText(ListIndex,3)));
    m_LoAllow.Format("%.2f", SafeAtoF(m_SpCfgList.GetItemText(ListIndex,4)));
    m_HiAllow.Format("%.2f", SafeAtoF(m_SpCfgList.GetItemText(ListIndex,5)));

    double LoDef;
    LoDef=SafeAtoF(m_LoAllow)+SafeAtoF(m_LoTol);
    m_LoDef.Format("%.2f", LoDef);

    double HiDef;
    HiDef=SafeAtoF(m_HiAllow)-SafeAtoF(m_HiTol);
    m_HiDef.Format("%.2f", HiDef);

    CString Ideal=m_SpCfgList.GetItemText(ListIndex,1);
    m_bUseIdeal=Ideal[0]=='Y';
    }
  else
    {
    m_LoTol="";
    m_HiTol="";
    m_LoAllow="";
    m_HiAllow="";
    m_LoDef="";
    m_HiDef="";
    m_bUseIdeal=false;
    }
  UpdateData(FALSE);
  }

//---------------------------------------------------------------------------

void CMdlCfgSpcs::OnChangeHitol() 
  {
  int iSel=-1;
  POSITION pos = m_SpCfgList.GetFirstSelectedItemPosition();
  if (pos)
    {
    iSel=m_SpCfgList.GetNextSelectedItem(pos);

    double All=SafeAtoF(m_HiAllow);
    double Tol=SafeAtoF(m_HiTol);
    double Def=All-Tol;

    UpdateData(TRUE);

    double NewTol=SafeAtoF(m_HiTol);
    double NewTolChk=Range(0.0, NewTol, Min(200.0, Def-K2C(1.0)));
    All=Def+NewTolChk;
    if (NewTol!=NewTolChk)
      m_LoTol.Format("%.2f", NewTol);
    m_LoAllow.Format("%.2f", All);

    m_SpCfgList.SetItem(iSel, 3, LVIF_TEXT, m_HiTol, 0, 0, 0, 0);
    m_SpCfgList.SetItem(iSel, 5, LVIF_TEXT, m_HiAllow, 0, 0, 0, 0);

    UpdateData(FALSE);
    }
  }

void CMdlCfgSpcs::OnChangeLotol() 
  {
  int iSel=-1;
  POSITION pos = m_SpCfgList.GetFirstSelectedItemPosition();
  if (pos)
    {
    iSel=m_SpCfgList.GetNextSelectedItem(pos);

    double All=SafeAtoF(m_LoAllow);
    double Tol=SafeAtoF(m_LoTol);
    double Def=All+Tol;

    UpdateData(TRUE);

    double NewTol=SafeAtoF(m_LoTol);
    double NewTolChk=Range(0.0, NewTol, Min(200.0, Def-K2C(1.0)));
    All=Def-NewTolChk;
    if (NewTol!=NewTolChk)
      m_LoTol.Format("%.2f", NewTol);
    m_LoAllow.Format("%.2f", All);

    m_SpCfgList.SetItem(iSel, 2, LVIF_TEXT, m_LoTol, 0, 0, 0, 0);
    m_SpCfgList.SetItem(iSel, 4, LVIF_TEXT, m_LoAllow, 0, 0, 0, 0);

    UpdateData(FALSE);
    }
  }

void CMdlCfgSpcs::OnUseideal() 
  {
  int iSel=-1;
  POSITION pos = m_SpCfgList.GetFirstSelectedItemPosition();
  if (pos)
    {
    iSel=m_SpCfgList.GetNextSelectedItem(pos);
    UpdateData(TRUE);
    CString S;
    S=m_bUseIdeal?"Yes":"";
    m_SpCfgList.SetItem(iSel, 1, LVIF_TEXT, S, 0, 0, 0, 0);
    }
  }

//---------------------------------------------------------------------------

void CMdlCfgSpcs::OnKeydownSpdblist(NMHDR* pNMHDR, LRESULT* pResult) 
  {
  LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
  POSITION pos = m_SpDBList.GetFirstSelectedItemPosition();
  if (pos!=NULL)
    {
    int i=m_SpDBList.GetNextSelectedItem(pos);
    if (pLVKeyDow->wVKey==VK_DOWN)
      i++;
    else if (pLVKeyDow->wVKey==VK_UP)
      i--;
    i=Range(0, i, m_SpDBList.GetItemCount()-1);
    UpdateWhat(GetType(m_SpDBList.GetItemData(i)), m_SpDBList.GetItemText(i,0), GetIndex(m_SpDBList.GetItemData(i)));
    }
  else
    m_What.SetWindowText("");
  EnableEditButtons();

  *pResult = 0;
  }

//---------------------------------------------------------------------------

void CMdlCfgSpcs::OnClickSpdblist(NMHDR* pNMHDR, LRESULT* pResult) 
  {
  POSITION pos = m_SpDBList.GetFirstSelectedItemPosition();
  if (pos!=NULL)
    {
    int i=m_SpDBList.GetNextSelectedItem(pos);
    UpdateWhat(GetType(m_SpDBList.GetItemData(i)), m_SpDBList.GetItemText(i,0), GetIndex(m_SpDBList.GetItemData(i)));
    }
  else
    m_What.SetWindowText("");
  EnableEditButtons();
  *pResult = 0;
  }

//---------------------------------------------------------------------------

void CMdlCfgSpcs::OnKeydownSpcfglist(NMHDR* pNMHDR, LRESULT* pResult) 
  {
  LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
  //EnableEditButtons();

  POSITION pos = m_SpCfgList.GetFirstSelectedItemPosition();
  if (pos!=NULL)
    {
    int i=m_SpCfgList.GetNextSelectedItem(pos);
    if (pLVKeyDow->wVKey==VK_DOWN)
      i++;
    else if (pLVKeyDow->wVKey==VK_UP)
      i--;
    i=Range(0, i, m_SpCfgList.GetItemCount()-1);
    if (GetType(m_SpCfgList.GetItemData(i))==spl_SpecieId)
      {
      CString Spc=m_SpCfgList.GetItemText(i,0);
      int Index=GetIndex(m_SpCfgList.GetItemData(i));
      UpdateWhat(spl_SpecieId, Spc, Index);
      UpdateTTols(i, Index);
      }
    else
      {
      UpdateWhat(GetType(m_SpCfgList.GetItemData(i)), "", -1);
      UpdateTTols(-1, -1);
      }
    }
  else
    {
    m_What.SetWindowText("");
    UpdateTTols(-1, -1);
    }
  EnableEditButtons();

  *pResult = 0;
  }

//---------------------------------------------------------------------------

void CMdlCfgSpcs::OnItemchangedSpcfglist(NMHDR* pNMHDR, LRESULT* pResult) 
  {
  NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
  // TODO: Add your control notification handler code here

  POSITION pos = m_SpCfgList.GetFirstSelectedItemPosition();
  if (pos!=NULL)
    {
    int i=m_SpCfgList.GetNextSelectedItem(pos);
    //if (pLVKeyDow->wVKey==VK_DOWN)
    //  i++;
    //else if (pLVKeyDow->wVKey==VK_UP)
    //  i--;
    i=Range(0, i, m_SpCfgList.GetItemCount()-1);
    if (GetType(m_SpCfgList.GetItemData(i))==spl_SpecieId)
      {
      CString Spc=m_SpCfgList.GetItemText(i,0);
      int Index=GetIndex(m_SpCfgList.GetItemData(i));
      UpdateWhat(spl_SpecieId, Spc, Index);
      UpdateTTols(i, Index);
      }
    else
      {
      UpdateWhat(GetType(m_SpCfgList.GetItemData(i)), "", -1);
      UpdateTTols(-1, -1);
      }
    }
  else
    {
    m_What.SetWindowText("");
    UpdateTTols(-1, -1);
    }
  EnableEditButtons();
  *pResult = 0;
  }

//---------------------------------------------------------------------------

void CMdlCfgSpcs::OnClickSpcfglist(NMHDR* pNMHDR, LRESULT* pResult) 
  {
  POSITION pos = m_SpCfgList.GetFirstSelectedItemPosition();
  if (pos!=NULL)
    {
    int i=m_SpCfgList.GetNextSelectedItem(pos);
    if (GetType(m_SpCfgList.GetItemData(i))==spl_SpecieId)
      {
      CString Spc=m_SpCfgList.GetItemText(i,0);
      int Index=GetIndex(m_SpCfgList.GetItemData(i));
      UpdateWhat(spl_SpecieId, Spc, Index);
      UpdateTTols(i, Index);
      }
    else
      {
      UpdateWhat(GetType(m_SpCfgList.GetItemData(i)), "", -1);
      UpdateTTols(-1, -1);
      }
    }
  else
    {
    m_What.SetWindowText("");
    UpdateTTols(-1, -1);
    }
  EnableEditButtons();
  *pResult = 0;
  }

//---------------------------------------------------------------------------

int CMdlCfgSpcs::FndFirstItem(int BOT_Phase)
  {
  for (int iItem=0; iItem<m_SpCfgList.GetItemCount(); iItem++)
    if (GetType(m_SpCfgList.GetItemData(iItem))==spl_SpecieId &&
      GetOcc(m_SpCfgList.GetItemData(iItem))==BOT_Phase)
      return iItem;
  return -1;
  }

int CMdlCfgSpcs::FndLastItem(int BOT_Phase)
  {
  if (BOT_Phase<0)
    return m_SpCfgList.GetItemCount()-1;

  int iItem=-1;
  for (int i=0; i<m_SpCfgList.GetItemCount(); i++)
    if (GetType(m_SpCfgList.GetItemData(i))==spl_SpecieId &&
      GetOcc(m_SpCfgList.GetItemData(i))==BOT_Phase)
      iItem=i;
  return iItem;
  }

int CALLBACK SpCfgFn(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
  {
  //const int Tst = (GetOcc(lParam1) - GetOcc(lParam2));
  //const BOOL b = (Tst<=0);
  const LPARAM b = ((GetOcc(lParam1) - GetOcc(lParam2))<=0);
  return (lParamSort==b ? 1 : -1);
  };

void CMdlCfgSpcs::OnSpdefaults() 
  {
  flag HasSol=false;
  flag HasLiq=false;
  flag HasVap=false;
  for (int i=0; i<m_SpCfgList.GetItemCount(); )
    {
    if ((GetType(m_SpCfgList.GetItemData(i)))!=spl_SpecieId)
      m_SpCfgList.DeleteItem(i);
    else
      {
      dword Msk=GetOcc(m_SpCfgList.GetItemData(i));
      HasSol=HasSol||(Msk==BOT_Solid);
      HasLiq=HasLiq||(Msk==BOT_Liquid);
      HasVap=HasVap||(Msk==BOT_Gas);
      i++;
      }
    }

  m_SpCfgList.SortItems(SpCfgFn, (iSpDefaults==0 || iSpDefaults==2));

  int iItem=0;
  int Cnt=0;
  switch (iSpDefaults)
    {
    case 0:
      Cnt=m_SpCfgList.InsertItem(0, "Vapours", 3);
      m_SpCfgList.SetItemData(Cnt, spl_VapSum);
      Cnt=m_SpCfgList.InsertItem(1, "Liquids", 3);
      m_SpCfgList.SetItemData(Cnt, spl_LiqSum);
      Cnt=m_SpCfgList.InsertItem(2, "Solids", 3);
      m_SpCfgList.SetItemData(Cnt, spl_SolSum);
      Cnt=m_SpCfgList.InsertItem(3, "Total", 3);
      m_SpCfgList.SetItemData(Cnt, spl_AllSum);
      break;
    case 1:
      Cnt=m_SpCfgList.InsertItem(0, "Solids", 3);
      m_SpCfgList.SetItemData(Cnt, spl_SolSum);
      Cnt=m_SpCfgList.InsertItem(1, "Liquids", 3);
      m_SpCfgList.SetItemData(Cnt, spl_LiqSum);
      Cnt=m_SpCfgList.InsertItem(2, "Vapours", 3);
      m_SpCfgList.SetItemData(Cnt, spl_VapSum);
      Cnt=m_SpCfgList.InsertItem(3, "Total", 3);
      m_SpCfgList.SetItemData(Cnt, spl_AllSum);
      break;
    }
  switch (iSpDefaults)
    {
    case 0:
    case 1:
      if (HasSol)
        {
        iItem=FndFirstItem(BOT_Solid);
        Cnt=m_SpCfgList.InsertItem(iItem, "---Solids---", 4);
        m_SpCfgList.SetItemData(Cnt, spl_Text);
        }
      if (HasLiq)
        {
        iItem=FndFirstItem(BOT_Liquid);
        Cnt=m_SpCfgList.InsertItem(iItem, "---Liquids---", 4);
        m_SpCfgList.SetItemData(Cnt, spl_Text);
        }
      if (HasVap)
        {
        iItem=FndFirstItem(BOT_Gas);
        Cnt=m_SpCfgList.InsertItem(iItem, "---Vapours---", 4);
        m_SpCfgList.SetItemData(Cnt, spl_Text);
        }
      if (0)
        {
        iItem=FndLastItem()+1;
        Cnt=m_SpCfgList.InsertItem(iItem, "---Total-----", 4);
        m_SpCfgList.SetItemData(Cnt, spl_Text);
        }
      break;

    case 2: 
      if (HasVap)
        {
        iItem=FndFirstItem(BOT_Gas);
        Cnt=m_SpCfgList.InsertItem(iItem, "Vapours", 3);
        m_SpCfgList.SetItemData(Cnt, spl_VapSum);
        }
      if (HasLiq)
        {
        iItem=FndFirstItem(BOT_Liquid);
        if (HasVap)
          {
          Cnt=m_SpCfgList.InsertItem(iItem, "--------------", 4);
          m_SpCfgList.SetItemData(Cnt, spl_Text);
          iItem++;
          }
        Cnt=m_SpCfgList.InsertItem(iItem, "Liquids", 3);
        m_SpCfgList.SetItemData(Cnt, spl_LiqSum);
        }
      if (HasSol)
        {
        iItem=FndFirstItem(BOT_Solid);
        if (HasVap||HasLiq)
          {
          Cnt=m_SpCfgList.InsertItem(iItem, "--------------", 4);
          m_SpCfgList.SetItemData(Cnt, spl_Text);
          iItem++;
          }
        Cnt=m_SpCfgList.InsertItem(iItem, "Solids", 3);
        m_SpCfgList.SetItemData(Cnt, spl_SolSum);
        }
      if (1)
        {
        iItem=FndLastItem()+1;
        Cnt=m_SpCfgList.InsertItem(iItem, "--------------", 4);
        m_SpCfgList.SetItemData(Cnt, spl_Text);
        iItem++;
        Cnt=m_SpCfgList.InsertItem(iItem, "Total", 3);
        m_SpCfgList.SetItemData(Cnt, spl_AllSum);
        }
      break;

    case 3: 
      if (HasSol)
        {
        iItem=FndFirstItem(BOT_Solid);
        Cnt=m_SpCfgList.InsertItem(iItem, "Solids", 3);
        m_SpCfgList.SetItemData(Cnt, spl_SolSum);
        }
      if (HasLiq)
        {
        iItem=FndFirstItem(BOT_Liquid);
        if (HasSol)
          {
          Cnt=m_SpCfgList.InsertItem(iItem, "--------------", 4);
          m_SpCfgList.SetItemData(Cnt, spl_Text);
          iItem++;
          }
        Cnt=m_SpCfgList.InsertItem(iItem, "Liquids", 3);
        m_SpCfgList.SetItemData(Cnt, spl_LiqSum);
        }
      if (HasVap)
        {
        iItem=FndFirstItem(BOT_Gas);
        if (HasSol||HasLiq)
          {
          Cnt=m_SpCfgList.InsertItem(iItem, "--------------", 4);
          m_SpCfgList.SetItemData(Cnt, spl_Text);
          iItem++;
          }
        Cnt=m_SpCfgList.InsertItem(iItem, "Vapours", 3);
        m_SpCfgList.SetItemData(Cnt, spl_VapSum);
        }
      if (1)
        {
        iItem=FndLastItem()+1;
        Cnt=m_SpCfgList.InsertItem(iItem, "--------------", 4);
        m_SpCfgList.SetItemData(Cnt, spl_Text);
        iItem++;
        Cnt=m_SpCfgList.InsertItem(iItem, "Total", 3);
        m_SpCfgList.SetItemData(Cnt, spl_AllSum);
        }
      break;
    }

  iSpDefaults=(iSpDefaults+1)%4;

  Strng Tmp;
  Tmp.Set("Default %d", iSpDefaults+1);
  SetDlgItemText(IDC_SPDEFAULTS, Tmp());
  }

  