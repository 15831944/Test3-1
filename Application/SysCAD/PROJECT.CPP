//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#include "stdafx.h"
#define __PROJECT_CPP
#include "sc_defs.h"
#include "resource.h"
#include "syscad.h"
#include "project.h"
#include "mainfrm.h"
#include "tagdb.h"
#include "drv_mngr.h"
#include "ArchMngr.h"

#include "scdverlic.h"

#if WITHNETSERVER
#include "cs_mngr.h"
#endif                     
#include "..\schist\hstmain.h"
#include "accnode.h"
#include "msgwnd.h"
#include "ordwnd.h"
#include "statswnd.h"
#include "grfdoc.h"
#include "tagvdoc.h"
#include "prjdoc.h"
#include "scdver.h"
#include "dlgbusy.h"
#include "prjdlgs.h"
#include "prjview.h"
#include "chngtag.h"
#include "tknparse.h"
#include "toolbars.h"
#include "tagvtext.h"
#include "tagvtrnd.h"
#include "tagvsplt.h"
#include "cmd_mngr.h"
#include "licbase.h"
#include "scd_wm.h"
#include "slvtool.h"
#include "io.h"
#include "mdlvalue.h"
#include "zipstuff.h"
#include "dbhelper.h"
//#include "visgrfdoc.h"
//#include "visgrffrm.h"
//#include "autodoc.h"
//#include "autofrm.h"
#include "copyblk.h"
#include "wndslct.h"
#include "explorescd.h"
#include "revhist.h"
#include "scdcmdif.h"
#include "marshal.h"
#include "ImpExpSelect.h"
#include "ElectricalImport.h"
#include "flwsolve.h"
#include ".\opcsrvrwrapper.h"
#include "gendlgs.h"
#include "neutralgrf.h"
#include "neutralmdl.h"
#include "neutraldlgs.h"
#include "findtagsdlg.h"
#include "mdlcfg.h"

extern "C"
  {
#include "grldefs.h"
  }

//#include "optoff.h"

// Force search of Version.Lib for Version control Functions
#pragma comment(lib, "version.lib")

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//===========================================================================
//
//
//
//===========================================================================

//const int LatestPrjFileVer = 3; //change for version control when reading .Spj file
//const int LatestPrjFileVer = 4; //window layout now stored in layout.ini, not xxx.Spj; changed 14/5/96
//const int LatestPrjFileVer = 5; //CNM Relative Filenames fixed - allows project name change in FileManager etc.; changed 23/8/96
//const int LatestPrjFileVer = 6; //KGA reorganised toolbars including toolbar saves 10/10/96
//const int LatestPrjFileVer = 7; //KGA added buttons and reorganised toolbar IDs 18/10/96
//const int LatestPrjFileVer = 8; //KGA changed method for saving toolbars (not name dependent) 25/10/96
//const int LatestPrjFileVer = 9; //KGA changed toolbar IDs used for saving toolbars 28/10/96
//const int LatestPrjFileVer = 10; //CNM Added SolverStart Button
//const int LatestPrjFileVer = 11; //CNM Rearrangement of toolbars
//const int LatestPrjFileVer = 12; //KGA Rearrangement of how document names are stored in spj AND Z-Order now in layout.ini 20/2/97 (SysCAD 7.1 Rev 5.2)
//const int LatestPrjFileVer = 13; //KGA Added list of files used by project to spj 4/3/97 (SysCAD 7.1 Rev 5.4)
//const int LatestPrjFileVer = 14; //CNM Added the Concept of invalid numeric tags (SysCAD 7.1 Rev 7.0)
//const int LatestPrjFileVer = 15; //KGA Extened/changed 'Bad Numeric Tag' functionality 29/10/97 (SysCAD 7.1 Rev 8.13)
//const int LatestPrjFileVer = 16; //CNM Tweaked Load to Change FlwSolve Tag to PlantModel 20/07/98 (SysCAD 7.2 Rev 2.7)
//const int LatestPrjFileVer = 17; //CNM Rearranged Directory Structure 01/08/98 (SysCAD 7.2 Rev 2.?)
//const int LatestPrjFileVer = 18; //CNM Rearranged Objects Flowsolver 04/11/98 (SysCAD 7.2 Rev 3.6)
//const int LatestPrjFileVer = 19; //CNM Rearranged Specie Quality Vars 07/05/99 (SysCAD Bld 11)
//const int LatestPrjFileVer = 20; //CNM Allow PID2/PID3 Selection
//const int LatestPrjFileVer = 21; //CNM Default Database = MDB
//const int LatestPrjFileVer = 22; //CNM Change ReactionBlkBuildDataDefn 11/99
//const int LatestPrjFileVer = 23; //CNM Change SizeDistColumnNames 12/99
//const int LatestPrjFileVer = 24; //CNM Fix FB_Eqn & FB_Data 02/2000
//const int LatestPrjFileVer = 25; //CNM Tweak the way toolbar positions are saved/recovered
//const int LatestPrjFileVer = 26; //CNM Extention to control User Functions for water
//const int LatestPrjFileVer = 27; //CNM Added Orig Project VerNo
//const int LatestPrjFileVer = 28; //CNM Added Filing Changes/Fixes --
//const int LatestPrjFileVer = 29; //CNM Added Filing Changes/Fixes -- Oops 28 Sent to KGA too early
//const int LatestPrjFileVer = 30; //KGA Rearrangement of toolbars
//const int LatestPrjFileVer = 31; //CNM $ changes : Project name, path, etc
//const int LatestPrjFileVer = 32; //CNM PB Pressure mods
//const int LatestPrjFileVer = 33; //CNM Run/Idle Mode and PB Pressure mods in FlashTank
//const int LatestPrjFileVer = 34; //CNM Viscosity Upgrade - Flw drops now use calcd viscosity
//const int LatestPrjFileVer = 35; //CNM FB tagging changed
//const int LatestPrjFileVer = 36; //CNM FB tagging changed .. and again
//const int LatestPrjFileVer = 37; //CNM Fix for probelm in cfg file (specie annotation)
//const int LatestPrjFileVer = 38; //CNM Entrainment / Link Data Storage
//const int LatestPrjFileVer = 39; //CNM Fix for "Null FlwEqn"
//const int LatestPrjFileVer = 40; //CNM Fix for FlwEqn Storage
//const int LatestPrjFileVer = 41; //CNM Change to H2O Eqn
//const int LatestPrjFileVer = 42; //CNM Fix For PressDropKFact
//const int LatestPrjFileVer = 43; //CNM Extention for ActiveHold in TagObj
//const int LatestPrjFileVer = 44; //CNM Changes to SpShape
//const int LatestPrjFileVer = 45; //CNM Restructure SpConduit / SpContainer / SpImage .... (Build57)
//const int LatestPrjFileVer = 46; //CNM HeatX1 Restructure ....
//const int LatestPrjFileVer = 47; //CNM Pipe/HX Join VolFlwFX Init/Restructure ... (Build58)
//const int LatestPrjFileVer = 48; //KGA 18/9/01 Tag changes in Filters...
//const int LatestPrjFileVer = 49; //KGA 20/9/01 Tag changes in Washers...
//const int LatestPrjFileVer = 50; //KGA  2/2/02 Rearrangement of toolbars
//const int LatestPrjFileVer = 51; //KGA 11/3/02 Removed option to load specie data from default.mdb
//const int LatestPrjFileVer = 52; //CNM 12/3/02 BuildDataDefn options changed from 32 to 64 bits.
//const int LatestPrjFileVer = 53; //CNM 26/3/02 Tolerance changes.
//const int LatestPrjFileVer = 54; //CNM 23/4/02 Time management.
//const int LatestPrjFileVer = 55; //CNM 23/5/02 More Time management.
//const int LatestPrjFileVer = 56; //CNM  4/6/02 Tweaks to RhoH Finder and other flownets stuff (Build71)
//const int LatestPrjFileVer = 57; //CNM  6/6/02 ProBal Press control improvements
//const int LatestPrjFileVer = 58; //CNM 14/6/02 Tweaks to MinLevel -> LowestIO
//const int LatestPrjFileVer = 59; //CNM 20/6/02 Tweaks to VolFlwFX
//const int LatestPrjFileVer = 60; //CNM 19/8/02 Tweaks to EvalOrder
//const int LatestPrjFileVer = 61; //CNM 19/8/02 Flow Properties Fwd/Rev with options
//const int LatestPrjFileVer = 62; //CNM 05/02/03 Tweaks to PowerSupplies | Ver8.2 26/5/03: IdealGas Flags; New NormPress Value
//const int LatestPrjFileVer = 63; //CNM 03/03/03 Tweaks to PowerSupplies
//const int LatestPrjFileVer = 64; //CNM 09/04/03 PowerDistribution
//const int LatestPrjFileVer = 65; //CNM 23/05/03 IdealGas Flags
//const int LatestPrjFileVer = 66; //CNM 04/06/03 AlcanSpModel Tweaks;
//const int LatestPrjFileVer = 67; //CNM 10/09/03 ADO Tweaks;
//const int LatestPrjFileVer = 68; //CNM 10/09/03 More ADO Tweaks - force a SaveAs;
//const int LatestPrjFileVer = 69; //CNM 10/09/03 NdFlngs etc tag change - force a SaveAs;
//const int LatestPrjFileVer = 70; //CNM 1/03/04 CfgFile path changes;
//const int LatestPrjFileVer = 71; //CNM 16/03/04 Environment Changes;
//const int LatestPrjFileVer = 72; //CNM 11/05/04 Fixes for new DynModes;
//const int LatestPrjFileVer = 73; //CNM 11/05/04 Fixes for new Heat & DynModes;
//const int LatestPrjFileVer = 74; //CNM 06/07/04 Final Fixes for SysVector Tags chages etc;
//const int LatestPrjFileVer = 75; //CNM 06/08/04 Change the way SpAttributes are stored
//const int LatestPrjFileVer = 76; //CNM 09/10/2004 Change the way SpAttributes are stored
//const int LatestPrjFileVer = 77; //CNM 22/11/2004 Change the Way Symbols are stored
//const int LatestPrjFileVer = 78; //CNM 11/01/2005 Change Tear Init Option Names
//const int LatestPrjFileVer = 79; //KGA 02/06/2005 AlcanSpModel Tweaks for organics/oxalate
//const int LatestPrjFileVer = 79; //CNM 15/03/2005 Vars Selection in SpModel & Frac defaults in Reactions

////const int LatestPrjFileVer = 81; //KGA 16/06/2005 Further changes to Vars Selection in SpModel
////const int LatestPrjFileVer = 82; //KGA 14/07/2005 Shell&Tube condensing duty limit change
//const int LatestPrjFileVer = 83; //CNM 18/08/2005 FlowMode changes

//const int LatestPrjFileVer = 81; //KGA 16/06/2005 Further changes to Vars Selection in SpModel
//const int LatestPrjFileVer = 82; //KGA 14/07/2005 Shell&Tube condensing duty limit change

//const int LatestPrjFileVer = 80; //CNM 12/04/2005 Changes for Leaks etc
//const int LatestPrjFileVer = 81; //KGA 16/06/2005 Further changes to Vars Selection in SpModel
//const int LatestPrjFileVer = 82; //KGA 14/07/2005 Shell&Tube condensing duty limit change
//const int LatestPrjFileVer = 83; //CNM 18/08/2005 FlowMode changes
// Spares
//const int LatestPrjFileVer = 90; //CNM 06/10/2005 Changes for Leaks etc REWORK due to divergent paths
//const int LatestPrjFileVer = 91; //CNM 02/10/2005 Fix 4Port Valve to have name convention save as valve
//const int LatestPrjFileVer = 92; //CNM 31/10/2005 Remove "Output" from PID
//const int LatestPrjFileVer = 93; //CNM 15/11/2005 Fix 4PortValve
//const int LatestPrjFileVer = 94; //CNM 11/11/2005 FlowMode fixes
//const int LatestPrjFileVer = 95; //CNM 19/01/2006 remove table FB_Eqn from Database
//const int LatestPrjFileVer = 96; //CNM 19/01/2006 Changed 'Location' to 'PlantArea'
//const int LatestPrjFileVer = 97; //CNM 25/04/2006 Added Evaluator Block
//const int LatestPrjFileVer = 98; //CNM 15/05/2006 Change in 'Time' Management
//const int LatestPrjFileVer = 99; //CNM 15/06/2006 Enabled Evaluator Block
//const int LatestPrjFileVer = 100; //CNM  4/07/2006 New Flow Modes
const int LatestPrjFileVer = 101; //CNM  11/09/2006 Force Fixes to Archiver


//===========================================================================
//
//
//
//===========================================================================

static void dbgDumpFilesAliases()
  {
  flag C,P;
  for (int i=0;i<2; i++)
    {
    if (i==0)
      {
      C=SetUsePreviousCfgAlias(false);
      P=SetUsePreviousPrjAlias(false);
      }
    else
      {
      SetUsePreviousCfgAlias(true);
      SetUsePreviousPrjAlias(true);
      }
    dbgpln("--- %s", i==0 ? "Current":"Previous");
    dbgpln("%-20.20s : %s", "ExeFile"              ,ExeFile()        ? ExeFile()        : "");
    dbgpln("%-20.20s : %s", "ProgFiles"            ,ProgFiles()      ? ProgFiles()      : "");
    dbgpln("%-20.20s : %s", "TemporaryFiles"       ,TemporaryFiles() ? TemporaryFiles() : "");
    dbgpln("%-20.20s : %s", "BaseCfgFiles"         ,BaseCfgFiles()   ? BaseCfgFiles()   : "");
    dbgpln("%-20.20s : %s", "CfgFile"              ,CfgFile()        ? CfgFile()        : "");
    dbgpln("%-20.20s : %s", "CfgFiles"             ,CfgFiles()       ? CfgFiles()       : "");
    dbgpln("%-20.20s : %s", "CfgName"              ,CfgName()        ? CfgName()        : "");
    dbgpln("%-20.20s : %s", "PrjFile"              ,PrjFile()        ? PrjFile()        : "");
    dbgpln("%-20.20s : %s", "PrjFiles"             ,PrjFiles()       ? PrjFiles()       : "");
    dbgpln("%-20.20s : %s", "PrjRmtCpyFiles"       ,PrjRmtCpyFiles() ? PrjRmtCpyFiles() : "");
    dbgpln("%-20.20s : %s", "PrjName"              ,PrjName()        ? PrjName()        : "");
    }
  SetUsePreviousCfgAlias(C);
  SetUsePreviousPrjAlias(P);
  }

//===========================================================================

int gs_FileNewFlag  = 0;
int gs_FileOpenFlag = 0;
int gs_BlockSizeAdjustment = 0;
int gs_ProjectOpenFlag = 0;

#define WithRmtFile 0

char* CWindowLists::MainWndTitle = "SysCAD 9.1 © Kenwalt";
//char* CWindowLists::AccessWndTitle = "AccessWnd";
LPCTSTR CWindowLists::AccessWndTitle(long i) 
  {
  static CString S[NAccessWnds];
  S[i].Format("AccessWnd.%i", i);
  return S[i];
  }
BOOL CWindowLists::WndMaxMode = FALSE;

static char * LayoutFileName = "Scd_Lay.ini";
static char * RmtBackupZipFileName = "RmtFilesBackup.zip";
#if WithRmtFile
static char * RmtFileName = "RmtFiles.Stg";
#endif


//---------------------------------------------------------------------------

CWindowLists::CWindowLists()
  {
  Reset();
  }

//---------------------------------------------------------------------------

void CWindowLists::Reset()
  {
  pMainWnd = NULL;
  for (int i=0; i<NAccessWnds; i++)
    pAccessWnd[i] = NULL;
  pMsgWnd = NULL;
  pOrdWnd = NULL;
  pStatsWnd = NULL;
  pCmdWnd = NULL;
#if WITHSFEWND
  pSFEWnd = NULL;
#endif
  pPrjWnd = NULL;
  TrndWnds.SetSize(0);
  GrfWnds.SetSize(0);
  Wnds.SetSize(0);
  }

//---------------------------------------------------------------------------

void CWindowLists::BuildLists()
  {
  pMainWnd = MainWnd();
  for (int i=0; i<NAccessWnds; i++)
    pAccessWnd[i] = gs_AccessWnds.Frame(i);
  pMsgWnd = CMsgWindow::GetFrame();
  pOrdWnd = COrdWindow::GetFrame();
  pStatsWnd = CStatsWindow::GetFrame();
  pCmdWnd = gs_pCmdFrame;
#if WITHSFEWND
  pSFEWnd = NULL;
#endif
  pPrjWnd = NULL;
  TrndWnds.SetSize(0);
  GrfWnds.SetSize(0);
  if (gs_pPrj/* && gs_pPrj->pPrjDoc*/)
    {
#if WITHSFEWND
    if (gs_pPrj->pFlwLib)
      pSFEWnd = gs_pPrj->pFlwLib->GetSFEFrame();
#endif
    POSITION PosT = ScdApp()->GetFirstDocTemplatePosition();
    while (PosT)
      {
      CDocTemplate* pTempl = ScdApp()->GetNextDocTemplate(PosT);
      POSITION PosD = pTempl->GetFirstDocPosition();
      while (PosD)
        {
        CDocument* pDoc = pTempl->GetNextDoc(PosD);
        POSITION pos = pDoc->GetFirstViewPosition();
        if (pos)
          {
          CView* pFirstView = pDoc->GetNextView(pos);
          CWnd* w = pFirstView->GetParent();
          if (pDoc->IsKindOf(RUNTIME_CLASS(CGrfDoc)))
            {
            ASSERT(w->IsKindOf(RUNTIME_CLASS(CGrfFrameWnd)));
            GrfWnds.Add(w);
            }
#if WITHVISIODOC
          else if (pDoc->IsKindOf(RUNTIME_CLASS(CVisGrfDoc)))
            {
            w = w->GetParent();
            ASSERT(w->IsKindOf(RUNTIME_CLASS(CVisGrfFrame)));
            GrfWnds.Add(w);
            }
#endif
          else if (pDoc->IsKindOf(RUNTIME_CLASS(CTagVwDoc)))
            {
            w = w->GetParent();
            ASSERT(w->IsKindOf(RUNTIME_CLASS(CTagVwSplit)));
            TrndWnds.Add(w);
            }
          else if (pDoc->IsKindOf(RUNTIME_CLASS(CPrjDoc)))
            {
            ASSERT(pPrjWnd==NULL); //Two project windows!!! Why???
            ASSERT(w->IsKindOf(RUNTIME_CLASS(CPrjFrameWnd)));
            pPrjWnd = w;
            }
#if WITHAUTOMATION
          else if (pDoc->IsKindOf(RUNTIME_CLASS(CAutoDoc)))
            {
            ASSERT(w->IsKindOf(RUNTIME_CLASS(CAutoFrame)));
            pAutWnd = w;
            }
#endif
          }
        else
          {
          //ASSERT_ALWAYS(FALSE); //what document/window was not closed properly???
          AfxMessageBox("Window Error!  (Press OK)");
          dbgpln("SHOULD NOT GET HERE!!!!"); //probably two project windows!!!
          }
        }
      }
    }
  }

//---------------------------------------------------------------------------

int CWindowLists::BuildSingleList(BOOL Sorted/*=false*/)
  {
  if (pMainWnd==NULL)
    BuildLists();
  Wnds.SetSize(0);
  Wnds.Add(CTopWindowInfo(pMainWnd, 3, MainWndTitle));

  for (int i=0; i<NAccessWnds; i++)
    Wnds.Add(CTopWindowInfo(pAccessWnd[i], 0, AccessWndTitle(i)));
  Wnds.Add(CTopWindowInfo(pMsgWnd, 0));
  if (pOrdWnd)
    Wnds.Add(CTopWindowInfo(pOrdWnd, 0));
  if (pStatsWnd)
    Wnds.Add(CTopWindowInfo(pStatsWnd, 0));
  Wnds.Add(CTopWindowInfo(pCmdWnd, 0));
#if WITHSFEWND
  if (pSFEWnd)
    Wnds.Add(CTopWindowInfo(pSFEWnd, 0));
#endif
  if (pPrjWnd)
    Wnds.Add(CTopWindowInfo(pPrjWnd, 0));
  for (i=0; i<GrfWnds.GetSize(); i++)
    Wnds.Add(CTopWindowInfo(GrfWnds[i], 1, NULL, i));
  for (i=0; i<TrndWnds.GetSize(); i++)
    Wnds.Add(CTopWindowInfo(TrndWnds[i], 2, NULL, i));
  if (Sorted)
    {
    for (i=1; i<Wnds.GetSize(); i++)
      for (int j=i; j>=1 && (_stricmp(Wnds[j].m_sName, Wnds[j-1].m_sName)<0); j--)
        {
        CTopWindowInfo temp = Wnds[j-1];
        Wnds[j-1] = Wnds[j];
        Wnds[j] = temp;
        }
    }
  return Wnds.GetSize();
  }

//---------------------------------------------------------------------------

int CWindowLists::BuildZOrder(BOOL Sorted)
  {
  if (pMainWnd==NULL || Wnds.GetSize()==0)
    BuildSingleList(Sorted);
  int ZOrd = 0;
  CWnd* pWnd = CWindowLists::GetCurrentTopWindow();
  int j = 0;
  while (pWnd)
    {
    int index = Find(pWnd);
    if (index>=0)
      Wnds[index].iZOrd = ZOrd++;
    pWnd = pWnd->GetNextWindow();
    if (j++>555)
      {
      AfxMessageBox("Error building Z-Order!  (Press OK)");
      pWnd = NULL;
      }
    }
  return Wnds.GetSize();
  }

//---------------------------------------------------------------------------

int CWindowLists::Find(byte Type, char* pTitle)
  {
  if (_strnicmp(pTitle, "Messages:", 9)==0)
    {
    for (int i=0; i<Wnds.GetSize(); i++)
      {
      if ((Wnds[i].iType==0) &&
        _strnicmp(pTitle, (const char*)(Wnds[i].m_sName), 9)==0)
        return i;
      }
    }
  for (int i=0; i<Wnds.GetSize(); i++)
    {
    if ((Wnds[i].iType==Type || Type==255) &&
      _stricmp(pTitle, (const char*)(Wnds[i].m_sName))==0)
      return i;
    }
  return -1;
  }

//---------------------------------------------------------------------------

int CWindowLists::Find(char* pTitle)
  {
  for (int i=0; i<Wnds.GetSize(); i++)
    {
    if (_stricmp(pTitle, (const char*)(Wnds[i].m_sName))==0)
      return i;
    }
  return -1;
  }

//---------------------------------------------------------------------------

int CWindowLists::Find(CWnd* pFindWnd)
  {
  for (int i=0; i<Wnds.GetSize(); i++)
    {
    if (Wnds[i].pWnd==pFindWnd)
      return i;
    }
  return -1;
  }

//---------------------------------------------------------------------------

CFrameWnd* CWindowLists::GetCurrentTopWindow()
  {
  CWnd* pWnd = MainWnd()->GetFocus();
  if (pWnd==NULL)
    return NULL;
  if (!pWnd->IsKindOf(RUNTIME_CLASS(CFrameWnd)))
    pWnd = pWnd->GetTopLevelFrame();
  if (pWnd)
    {
    if (pWnd==MainWnd())
      {//this happens sometimes, eg when the active window was minimized!
      pWnd = MainWnd()->GetTopWindow(); //top window is often not what we expect, find what we actually want...
      while (pWnd && !pWnd->IsKindOf(RUNTIME_CLASS(CMyMDIClient)))
        pWnd = pWnd->GetNextWindow();
      if (pWnd==NULL)
        return NULL;
      pWnd = pWnd->GetTopWindow();
      if (pWnd && !pWnd->IsKindOf(RUNTIME_CLASS(CFrameWnd)))
        pWnd = pWnd->GetTopLevelFrame();
      }
    return (CFrameWnd*)pWnd;
    }
  return NULL;

  /*OLD CODE...
  CWnd* pWnd = MainWnd()->GetFocus();
  if (pWnd==NULL)
  return NULL;
  if (!pWnd->IsKindOf(RUNTIME_CLASS(CFrameWnd)))
  pWnd = pWnd->GetParent();
  if (pWnd && !pWnd->IsKindOf(RUNTIME_CLASS(CFrameWnd)))
  pWnd = pWnd->GetParent();
  if (pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CFrameWnd)))
  return (CFrameWnd*)pWnd;
  return NULL;*/
  }

//---------------------------------------------------------------------------

CDocument* CWindowLists::GetCurrentTopWindowDoc()
  {
  CWnd* w = CWindowLists::GetCurrentTopWindow();
  if (w)
    w = w->GetTopWindow();
  if (w && w->IsKindOf(RUNTIME_CLASS(CSplitterWnd)))
    w = ((CSplitterWnd*)w)->GetPane(0,0);
  if (w && !w->IsKindOf(RUNTIME_CLASS(CView)))
    w = w->GetTopWindow();
  if (w && w->IsKindOf(RUNTIME_CLASS(CView)))
    return ((CView*)w)->GetDocument();
  return NULL;
  }

//---------------------------------------------------------------------------

void CWindowLists::ActivateTopWindowCmds()
  {//ensure the current top window has its commands (toolbars) enabled
  CDocument* pDoc = CWindowLists::GetCurrentTopWindowDoc();
  if (pDoc && pDoc->IsKindOf(RUNTIME_CLASS(DocRoot)))
    ((DocRoot*)pDoc)->OnActivate(true);  //direct cmds to this document AND causes correct toolbar buttons to be activated
  }

//---------------------------------------------------------------------------

CWnd* CWindowLists::GetMDIClientWnd()
  {
  return &(MainWnd()->m_MDIClientWnd);
  }

//---------------------------------------------------------------------------

int CWindowLists::GetTrendWndCount()
  {
  if (gs_pPrj)
    {
    int DocCnt = 0;
    for (int iTemp=iTrendTemplate; iTemp<=iControlTemplate; iTemp++)
      if (ScdApp()->TemplateExists(iTemp))
        {
        POSITION Pos = ScdApp()->Template(iTemp).GetFirstDocPosition();
        while (Pos)
          {
          ScdApp()->Template(iTemp).GetNextDoc(Pos);
          DocCnt++;
          }
        }
      return DocCnt;
    }
  return 0;
  }

//---------------------------------------------------------------------------

int CWindowLists::GetGrfWndCount()
  {
  if (gs_pPrj)
    {
    int DocCnt = 0;
    for (int iTemp=iGraphTemplate; iTemp<=iGraphTemplate/*iVisioTemplate*/; iTemp++)
      if (ScdApp()->TemplateExists(iTemp))
        {
        POSITION Pos = ScdApp()->Template(iTemp).GetFirstDocPosition();
        while (Pos)
          {
          ScdApp()->Template(iTemp).GetNextDoc(Pos);
          DocCnt++;
          }
        }
      return DocCnt;
    }
  return 0;
  }

//---------------------------------------------------------------------------

CDocument* CWindowLists::GetGrfWndByName(char* pTitle)
  {
  if (gs_pPrj && ScdApp()->TemplateExists(iGraphTemplate))
    {
    const int len = strlen(pTitle);
    POSITION Pos = ScdApp()->Template(iGraphTemplate).GetFirstDocPosition();
    while (Pos)
      {
      CDocument* pDoc = (CDocument*)(ScdApp()->Template(iGraphTemplate).GetNextDoc(Pos));
      int len2 = strlen(pDoc->GetTitle());
      if ((len==len2 || len==len2-4) && _strnicmp(pTitle, pDoc->GetTitle(), len)==0)
        {
        return pDoc;
        }
      }
    Strng Fn;
    Fn=PrjFiles();
    Fn+=pTitle;
    CDocument* pDoc = NULL;
    if (!FileExists(Fn()))
      {
      pDoc = (CDocument*)(ScdApp()->Template(iGraphTemplate).OpenDocumentFile(NULL));
      if (pDoc)
        pDoc->SetPathName(Fn());
      else
        LogError("Project", 0, "Document Not Opened: %s", Fn());
      }
    else
      pDoc = (CDocument*)(ScdApp()->Template(iGraphTemplate).OpenDocumentFile(Fn()));
    return pDoc;
    }
  return NULL;
  }

//---------------------------------------------------------------------------

CDocument* CWindowLists::GetGrfWndByIndex(int index)
  {
  if (gs_pPrj && ScdApp()->TemplateExists(iGraphTemplate))
    {
    int Cnt = 0;
    POSITION Pos = ScdApp()->Template(iGraphTemplate).GetFirstDocPosition();
    while (Pos)
      {
      CDocument* pDoc = (CDocument*)(ScdApp()->Template(iGraphTemplate).GetNextDoc(Pos));
      if (index==Cnt)
        {
        return pDoc;
        }
      Cnt++;
      }
    }
  return NULL;
  }

//---------------------------------------------------------------------------

//CDocTemplate* CWindowLists::GetGrfDocTemplate()
//  {
//  return ScdApp()->pGraphTemplate;
//  }
//
////---------------------------------------------------------------------------
//
//CDocTemplate* CWindowLists::GetTrendDocTemplate(int iType)
//  {
//  return ScdApp()->pTrendTemplate[iType];
//  }

//---------------------------------------------------------------------------

void CWindowLists::DetermineActiveGraphics()
  {
  if (gs_pPrj && ScdApp()->TemplateExists(iGraphTemplate))
    {
    const int GrfCount = GetGrfWndCount();
    POSITION Pos = ScdApp()->Template(iGraphTemplate).GetFirstDocPosition();
    while (Pos)
      {
      CGrfDoc* pGrfDoc = (CGrfDoc*)(ScdApp()->Template(iGraphTemplate).GetNextDoc(Pos));
      Strng_List TagList;
      int NTagsGrf = pGrfDoc->GetTagList(TagList);
      int NTags = gs_pPrj->FlwLib()->FE_TagOperation(FETOp_GetActive, TagList);
      bool bFoundOne = false;
      for (pStrng p=TagList.First(); p; p=p->Next())
        {
        if (p->Index())
          {
          bFoundOne = true;
          break;
          }
        }
      pGrfDoc->bModelsActive = bFoundOne;
      }
    }
  }

//---------------------------------------------------------------------------

void CWindowLists::SetGrfTagGroups(bool Force)
  {
  if (gs_pPrj && ScdApp()->TemplateExists(iGraphTemplate))
    {
    const int GrfCount = GetGrfWndCount();
    if (Force || GrfCount!=gs_GrfTagGroups.Count())
      gs_GrfTagGroups.Reset(GrfCount);
    int i=0;
    POSITION Pos = ScdApp()->Template(iGraphTemplate).GetFirstDocPosition();
    while (Pos && i<GrfCount)
      {
      CGrfDoc* pGrfDoc = (CGrfDoc*)(ScdApp()->Template(iGraphTemplate).GetNextDoc(Pos));
      gs_GrfTagGroups.Groups[i] = pGrfDoc->GetTitle();
      gs_GrfTagGroups.Active[i] = pGrfDoc->bModelsActive;
      i++;
      }
    }
  }

//===========================================================================

CPrjFilesLists::CPrjFilesLists()
  {
  pExecFiles = NULL;
  Reset();
  }

//---------------------------------------------------------------------------

CPrjFilesLists::~CPrjFilesLists()
  {
  if (pExecFiles)
    {
    for (int i=0; i<iSize; i++)
      delete pExecFiles[i];
    delete []pExecFiles;
    }
  }

//---------------------------------------------------------------------------

void CPrjFilesLists::Reset()
  {
  if (pExecFiles)
    {
    for (int i=0; i<iSize; i++)
      delete pExecFiles[i];
    delete []pExecFiles;
    }
  iSize = 0;
  iAllocSize = 2;//16;
  pExecFiles = new pCPrjFilesInfo[iAllocSize];
  }

//---------------------------------------------------------------------------

int CPrjFilesLists::Add(CPrjFilesInfo* pFI)
  {
  if (iSize>=iAllocSize)
    {
    iAllocSize += 8;
    CPrjFilesInfo ** pNewFiles = new pCPrjFilesInfo[iAllocSize];
    for (int i=0; i<iSize; i++)
      pNewFiles[i] = pExecFiles[i];
    for (; i<iAllocSize; i++)
      pNewFiles[i] = NULL;
    delete pExecFiles;
    pExecFiles = pNewFiles;
    }
  pExecFiles[iSize++] = pFI;
  return iSize-1;
  }

//---------------------------------------------------------------------------

void CPrjFilesLists::BuildLists()
  {
  Reset();
  if (1)//pExec)
    {
    CExecObj* pPrevExecObj = NULL;
    int index = -1;
    RequestTagInfoRec Rqst(RQ_Files);
    ReplyTagInfoRec Info;
    while (gs_Exec.RequestTagInfo(Rqst, Info))
      {
      if (pPrevExecObj!=Info.pExecObj)
        {
        index = Find(Info.pExecObj->Name());
        if (index<0)
          index = Add(new CPrjFilesInfo(Info.pExecObj->Name()));
        pPrevExecObj = Info.pExecObj;
        }
      DWORD flags = 0;
      if (Info.bForceCopy)
        flags |= EFF_ForceCopy;
      if (Info.bPreventCopy)
        flags |= EFF_PreventCopy;
      if (Info.bCanEditFile)
        flags |= EFF_CanEditFile;
      if (Info.bExcelFile)
        flags |= EFF_ExcelValid;
      if (Info.bData1Valid)
        flags |= EFF_OtherValid;
      if (Info.bDWDataValid)
        {
        flags |= EFF_GrpValid;
        switch (Info.dwData)
          {
          case  1: flags |= EFF_Grp_Models; break;
          case  2: flags |= EFF_Grp_Referenced; break;
          case  3: flags |= EFF_Grp_SpecSheets; break;
          case  4: flags |= EFF_Grp_Configuration; break;
          case 11: flags |= EFF_Grp_Reports; break;
          case 12: flags |= EFF_Grp_CmdScripts; break;
          }
        }
      if (Info.bData1Valid)
        pExecFiles[index]->Files.Add(CExecObjFileInfo(Info.pExecObj, flags, Info.sData(), Info.sData1(), Info.bForceCopy, Info.bPreventCopy));
      else
        pExecFiles[index]->Files.Add(CExecObjFileInfo(Info.pExecObj, flags, Info.sData(), Info.bForceCopy, Info.bPreventCopy));
      }
    }
  }

//---------------------------------------------------------------------------

int CPrjFilesLists::Find(char* pName)
  {
  for (int i=0; i<GetSize(); i++)
    {
    if (_stricmp(pName, pExecFiles[i]->sExecName())==0)
      return i;
    }
  return -1;
  }

//---------------------------------------------------------------------------

int CPrjFilesLists::FindFile(char* pFileName)
  {
  for (int i=0; i<GetSize(); i++)
    {
    for (int j=0; j<pExecFiles[i]->Files.GetSize(); j++)
      if (_stricmp(pExecFiles[i]->Files[j].sFilename(), pFileName)==0)
        return i;
    }
  return -1;
  }

//---------------------------------------------------------------------------

void CPrjFilesLists::CheckExists()
  {
  if (pExecFiles)
    {
    Strng s;
    for (int i=0; i<iSize; i++)
      {
      for (int j=0; j<pExecFiles[i]->Files.GetSize(); j++)
        {
        CExecObjFileInfo &FI = pExecFiles[i]->Files[j];
        s.FnExpand(FI.sFilename());
        FI.bExists = FileExists(s());
        }
      }
    }
  }

//---------------------------------------------------------------------------

const int OtherExtCnt = 7;
const int KnownExtCnt = OtherExtCnt-1;

char* CPrjFilesLists::OtherExts(int index)
  {
  char* Exts[OtherExtCnt] = { ".snp", ".scn", ".pgm", ".rct", ".xls", ".ssc", ".*" };
  return Exts[index];
  }

char* CPrjFilesLists::OtherExtsEx(int index)
  {
  char* ExtsEx[OtherExtCnt] = { ".snp.zip", ".scn.zip", "", "", "", "", "" };
  return ExtsEx[index];
  }

//---------------------------------------------------------------------------

void CPrjFilesLists::CheckUnknown()
  {
  for (int j=0; j<OtherExtCnt; j++)
    OtherFiles[j].RemoveAll();
  CString S;
  Strng s,Ext,NameExt;
  s = PrjFiles();
  s += "*.*";
  WIN32_FIND_DATA fd;
  HANDLE H = FindFirstFile(s(), &fd);
  flag AllDone = (H==INVALID_HANDLE_VALUE);
  while (!AllDone)
    {
    if ((fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)==0)
      {
      S = PrjFiles();
      S += fd.cFileName;
      s.FnContract((char*)(const char*)S);
      if (FindFile(s())==-1)
        {
        Ext.FnExt(fd.cFileName);
        if (Ext())
          {
          NameExt.FnNameExt(fd.cFileName);
          const int l = NameExt.Len();
          for (int j=0; j<KnownExtCnt; j++)
            {
            if (_stricmp(OtherExts(j), Ext())==0)
              break;
            const int el = strlen(OtherExtsEx(j));
            if (el && l>el && _strnicmp(OtherExtsEx(j), &NameExt[l-el], el)==0)
              break;
            }
          OtherFiles[j].Add(S);
          }
        else
          OtherFiles[KnownExtCnt].Add(S);
        }
      }
    AllDone = !FindNextFile(H, &fd);
    }
  FindClose(H);
  }

//===========================================================================
//
//
//
//===========================================================================

const int VersionChar='-';
const int ComStartChar='(';
const int ComEndChar=')';

static char* SwingFileNameToDest(Strng & Fn, Strng & OrigPrj, Strng & DestPrj)
  {
  int Lo=OrigPrj.Length();
  int Lf=Fn.Length();
  // is the path the same to here
  if ((Lo>0) && (Lf>=Lo) &&
    (Fn.XStrNICmp(OrigPrj(), Lo)==0))// &&
    //(Fn[Lo]=='\\'))
    {
    //Strng T1(DestPrj);
    Strng T2(Fn.Right(Lf-Lo));
    Fn=DestPrj;
    Fn+=T2;
    //DestPrj
    //for (int i=0;i<Lo;i++)
    //  Fn[i]=OrigPrj[i];
    }

  return Fn();
  };

//---------------------------------------------------------------------------

inline flag BadSave()  { return false; }
inline flag GoodSave()  { return true; }

//===========================================================================
//
//
//
//===========================================================================

#define MdlTagRules "ModelTagRules"

CUniqueTagRulesBlk::CUniqueTagRulesBlk(LPCTSTR pMdlName, LPCTSTR pBase, char SepChar)
  {
  CProfINIFile PF(CfgFile());
  Strng MdlName(pMdlName&&strlen(pMdlName)>0?pMdlName:pBase);
  // strip version
  char * p=MdlName.XStrRChr('-');
  if (p)
    MdlName.SetLength(p-MdlName());
  //
  m_Base= PF.RdStr(MdlTagRules, MdlName(), (LPTSTR)pBase);
  Strng S(SepChar);
  S=PF.RdStr(MdlTagRules, "SepChar", S());
  if (S.Length()<1)
    S="_";
  m_SepChar=S[0];
  }

//===========================================================================
//
//
//
//===========================================================================

ActiveTagInfo::ActiveTagInfo()
  {
  NTags=NActive=NTagsI=NActiveI=NMissing=NFound=0;
  fIsDLL=0;
  }

//--------------------------------------------------------------------------

void ActiveTagInfo::GetTags()
  {
  TagList.Append("*");
  NTags = gs_pPrj->FlwLib()->FE_TagOperation(FETOp_GetActive, TagList);
  }

//--------------------------------------------------------------------------

void ActiveTagInfo::RebuildTagMap(ActiveTagInfoMap& TagMap)
  {
  TagMap.InitHashTable(FindNextPrimeNumber((NTags*120)/100));
  for (pStrng p=TagList.First(); p; p=p->Next())
    TagMap.SetAt(p->Str(), p);
  }

//--------------------------------------------------------------------------

int ActiveTagInfo::SetActive(flag Active)
  {
  int N=0;
  for (pStrng p=TagList.First(); p; p=p->Next())
    {
    p->SetIndex(Active);
    N++;
    }
  return N;
  }

//===========================================================================

CDDESrvrHelper::CDDESrvrHelper()
  {
  hDDELib = NULL;
  iDDEUseCnt = 0;
  pMngr = NULL;
  }

//---------------------------------------------------------------------------

CDDESrvrHelper::~CDDESrvrHelper()
  {
  ASSERT(iDDEUseCnt==0 && hDDELib==NULL && pMngr==NULL); //this should have been properly closed!
  }

//---------------------------------------------------------------------------
#define DDESRVR_DLL_Name "ScDdeSrvr.dll"

typedef CCommsServerManagerBase* (*fn_DDESrvrDrvOpen)(void);
typedef bool (*fn_DDESrvrDrvClose)(void);

bool CDDESrvrHelper::OpenSrvr()
  {
  ASSERT(pMngr==NULL);
  //CWaitCursor Wait;

  bool DoLoad = (hDDELib==NULL);

  HINSTANCE hLib = hDDELib;
  if (hLib==NULL)
    hLib = AfxLoadLibrary(DDESRVR_DLL_Name);
  if (hLib)
    {
    fn_DDESrvrDrvOpen f1 = (fn_DDESrvrDrvOpen)GetProcAddress(hLib, "OpenDDESrvr");
    fn_DDESrvrDrvClose f2 = (fn_DDESrvrDrvClose)GetProcAddress(hLib, "CloseDDESrvr");
    if (f1==NULL || f2==NULL)
      {
      LogError("DDE Server", LF_Exclamation, "Incorrect %s", DDESRVR_DLL_Name);
      if (DoLoad)
        AfxFreeLibrary(hLib);
      return false; //DLL existed, but was not the expected DLL
      }
    pMngr = f1();
    if (pMngr==NULL)
      {
      LogError("DDE Server", LF_Exclamation, "Error Opening DDE Server for SysCAD");
      if (DoLoad)
        AfxFreeLibrary(hLib);
      return false;
      }
    //all is well in the land of SysCAD DDE Server...
    hDDELib = hLib;
    iDDEUseCnt++;
    }
  else
    {
    LogError("DDE Server", LF_Exclamation, "Unable to load %s or related DLL", DDESRVR_DLL_Name);
    return false;
    }
  return (pMngr!=NULL);
  }

//---------------------------------------------------------------------------

void CDDESrvrHelper::CloseSrvr()
  {
  if (hDDELib)
    {
    //CWaitCursor Wait;
    //CDlgBusy::Open("\n\nClosing DDE Server");
    fn_DDESrvrDrvClose f = (fn_DDESrvrDrvClose)GetProcAddress(hDDELib, "CloseDDESrvr");
    bool b = f();
    if (b)
      pMngr = NULL;
    else
      LogError("DDE Server", LF_Exclamation, "Error Closing SysCAD DDE Server");
    iDDEUseCnt--;
    if (iDDEUseCnt==0)
      {
      BOOL b = AfxFreeLibrary(hDDELib);
      hDDELib = NULL;
      }
    }
  ASSERT(pMngr==NULL);
  }

//---------------------------------------------------------------------------

void CDDESrvrHelper::Options()
  {
  if (pMngr)
    pMngr->OpenStatusWnd();
  }

//---------------------------------------------------------------------------

void CDDESrvrHelper::CloseOptions()
  {
  if (pMngr)
    pMngr->CloseStatusWnd();
  }

//===========================================================================
//
//
//
//===========================================================================

static char * FnPathRelContract(char * Fn)
  {
  static Strng Tmp;
  Tmp=Fn;
  Tmp.FnMakePrjFileRelative();
  Tmp.FnContract();
  return Tmp();
  }

//===========================================================================
//
//
//
//===========================================================================

CProjectSettings::CProjectSettings(CHistSettings & Hst) : m_Hst(Hst)
  {
  ClearSettings();
  };
CProjectSettings::~CProjectSettings()
  {
  };

//---------------------------------------------------------------------------

void CProjectSettings::ClearSettings()
  {
  dbgpln("--------------------- CProjectSettings::Clear");



  //bForceSaveAsNV=false;
  //bDoingSaveAs = false;
  //bDoingSaveAsNV = false;
  //bDoingSaveAsWithName = false;
  //bDoingLoad = false;
  //pPrjDoc=NULL;
  ////  sPrjFile="";
  //bCmdSuccess=0;
#if WITHDRVMAN
  bDrvOpen=0;
  bDrvOn=0;
  bDrvLclTagSrvrOK=1;
  bDrvReadAll=0;
#endif
  //bArcOpen=0;
  m_bArcOn=false;
  m_bArcOpenDBOnRun=1;
  //bIOMOpen=0;
  m_bIOMOn=0;

  m_bDDEOn=0;
  //pDDESrvr=NULL;
  m_bOPCOn=0;
  m_bOPCResetReg=0;
  m_iOPCServerNo=0;
  //;pOPCSrvr=NULL;
  m_bGrfDeferLoad=0;
  m_bGrfAutoLoad=0;
  m_bGrfUpdateAnnot=1;
  m_bGrfAnimationOn=1;
  m_bGrfPromptForZoom=1;
  m_bGrfMoveCursor=1; // CNM
  m_bGrfRegExpOn=0;
  m_bReadOnlyPrj=0;
  //bHstOK=0;
  //bCommsOpen=0;
  m_bAccAutoSaveOn=0;
  m_bAccUpdateOnRun=1;
  m_bAccHistoryInfoVis=0;

  m_sDefGrpLib      = "$BaseCfgFiles\\GroupLibrary.MDB";
  m_sGrfFrameName   = "Extents-A3";
  m_sGrfFrameFilter = "Frames:";

  m_sDrvManagerName="Scd_Drvr.mdb";
  
  m_sArcManagerName="$Prj\\Archive.sac";
  //m_dwArcManagerOptions = //LogItem_Cmd |
  //                        LogItem_Type |
  //                        LogItem_Source |
  //                        //LogItem_IDNo |
  //                        LogItem_IterNo |
  //                        //LogItem_CallNo |
  //                        //LogItem_SeqNo |
  //                        //LogItem_HResult |
  //                        LogItem_Msg |
  //                        //LogItem_Time |
  //                        LogItem_ElapsedTime |
  //                        LogItem_Tag;

  m_sIOMarshalName="*.scm";
  m_sIOMarshalNode="";
#if WITHDRVMAN
  bDrvIgnoresOn=0;
  bDrvForcesOn=0;
  bDrvNoiseOn=1;
  bDrvStartFBKCheck=FBKCheckNever;
#endif
  m_lStatsRefreshCnt=5L;
  m_Hst.m_OnRqd=true;
  m_ReportInfo.iHSearchLen = 10;
  m_ReportInfo.iVSearchLen = 100;
  m_ReportInfo.sNan = "*";
  m_ReportInfo.sBlank = "";
  m_ReportInfo.sIgnoreChars = "=";
  m_bRptExcelLock=1;
  m_bRptExcelCellName=1;
  m_bRptExcelMakeActive=1;
  m_bRptExcelSysCADActive=1;
  m_bRptExcelUpdateLinks=1;
  m_bRptExcelSaveOnComplete=1;
  m_bRptSyscadTags=1;
  m_bShowStatusColours=1;
  m_dwMdlStatusOptions = 0;
  //iOverlayStatusPen=PS_DASH;
  //bTimeChanged=0;
  //bHstChanged=0;
  //bDrvChanged=0;
  //bArcChanged=0;
  //bDocChanged=0;
  //bLoadBusy=0;
  m_bConfigBusy=0;

  m_bDefaultRecordIt=1;
  m_bFlashTrend=1; // CNM - most people who see this seem to like it !!!
  m_bShowNewTrndLineDlg=0;
  //bChangedGrfMenu=0;
  //bChangedRuntimeMenu=0;
  //bDoGrfAutoAccess=0;
  //bBusyAnalysing=0;
#if WITHSCRCYCLES
  bSystemScreenSave=1;
  bSyscadScreenSave=0;
  if (Cycles)
    delete []Cycles;
  if (CycleWnds)
    delete []CycleWnds;
  CycleWnds = NULL;
  if (CyclePos)
    delete []CyclePos;
  CyclePos = NULL;
  if (pCycleDescWnd)
    delete pCycleDescWnd;
  pCycleDescWnd = NULL;
  pOnlyWnd = NULL;
  iScrCycles = 1;
  Cycles = new CScreenSaverInfo[iScrCycles];
  Cycles[0].iWaitTime = 10;
  iCycleNo=-1;
#endif
#if WITHNETSERVER
  bNetOpen=0;
#endif
  m_eSSAction = SSA_Prompt;//SSA_AutoInc;
  m_sSSName = "SnapShot";
  m_sSSPrefix = "Snap_";
  m_iSSNumber = 0;
  m_bSSDoZip = 0;
  m_bToolTips=1;
  m_bFlyBys=1;

  //_asm int 3; // these must be fixed
  m_NetMode=NM_Probal;

  m_PBNodeMode=SM_Direct;              
  m_PBLinkMode=SM_Direct;
  m_PBHeatMode=HM_Full;
  m_PBFlowMode=LFM_Xfer;

  m_DynNodeMode=SM_Buffered;              
  m_DynLinkMode=SM_Direct;
  m_DynHeatMode=HM_Full;
  m_DynFlowMode=LFM_Full;

  m_MaxNodeMode=SM_Buffered;
  m_MaxLinkMode=SM_Direct;
  m_MaxHeatMode=HM_Full;
  m_MaxFlowMode=LFM_Full;

  m_Solver.m_fStateWnd=true;
  m_Solver.m_fStatePinned=false;
  m_Solver.m_fStateErrors=false;
  m_Solver.m_iStateX=-10000;
  m_Solver.m_iStateY=-10000;

  m_bRunOptEmptyAll=0;
  m_bRunOptEmptySpills=0;
  m_bRunOptZeroFlows=0;    
  m_bRunOptInitialiseAll=0;
  m_bRunOptResetStats=0;   
  m_bRunOptOnceOnly=0;    

  m_GrfBehaviour=WB_None;
  m_TrndBehaviour=WB_None;

  //#if WITHNETSERVER
  //  pCS_Mngr->SetServer(false);
  //  pCS_Mngr->SetClientConnect(false, "", false);
  //#endif
  //  MdlLibs.UnLoadFlwDLLs();
  m_fNewTrend=true;
  m_fNewGraph=true;
  //CMdlValueSet::Clear();
  //CRptTagLists::ShutDown();

  //XSetDBFormat(ScdPFUser.RdInt("General", "PrjsDBFormat", DBConnect_JET2000));

  m_bRemoveRevs=true;
  m_bCopyDBFiles=true;

  //m_Save.m_bRqstNew=0;
  //m_Save.m_bIsNewPrj=0;

  //InitColours();

  //SetPrjFileVerNo(LatestPrjFileVer);
  //SetNewPrjFileVerNo(LatestPrjFileVer);
  //SetOrigPrjFileVerNo(LatestPrjFileVer);
  //SetPrjFileVerNoAsLoaded(LatestPrjFileVer);
  //m_PrjFileVerNoAsLoaded=LatestPrjFileVer;

  //if (CfgFile())
  //  {
  //  CProfINIFile PF(CfgFile());
  //  TaggedObject::NumericTagsBad = (PF.RdLong("General", "NumericTagsBad", TaggedObject::NumericTagsBad) != 0);
  //  TaggedObject::NumericStartingTagsBad = (PF.RdLong("General", "NumericStartingTagsBad", TaggedObject::NumericStartingTagsBad) != 0);
  //  TaggedObject::NumericTagsBad = TaggedObject::NumericTagsBad || TaggedObject::NumericStartingTagsBad;
  //  }
  //else
  //  {
  //  TaggedObject::NumericTagsBad = true;
  //  TaggedObject::NumericStartingTagsBad = true;
  //  }

  //m_RmtCpyFolder="";
  //m_LastSnapLoadFn="";
  //gs_Exec.SetRealTime(0);
  //gs_Exec.SetRealTimeMult(1);
  //gs_Exec.StepSizeMax=1.0;
  //gs_Exec.SetTheTime(0.0, "PrjClear");
  //gs_Exec.SetScnType(ScnTyp_Manual, false);
  //gs_Exec.ScnTermAction=ScnTA_Edit;
  //gs_Exec.ScnStopTime=3600.0;
  //gs_Exec.ScnDuration=3600.0;
  //gs_Exec.StepCountMax=1;

  m_TheTime                   = CTimeValue(0.0);
  m_StepSizeMax               = 1.0;
  m_ScnStopTime               = 3600.0;
  m_ScnDuration               = 3600.0;

  m_RealTime                  =  0;
  m_RealTimeMult              = 1.0;
  m_SyncWithClock             = 0;
  m_ScnType                   = ScnTyp_Manual;
  m_ScnTermAction             = ScnTA_Edit;
  m_StepCountMax              = 1;

  m_DisplayTagsOnly          = false;
  m_HideZeros                = false;
  m_ShowGroups               = false;

  m_NumericTagsBad           = true;
  m_NumericStartingTagsBad   = true;
  m_NonNumericTagChr         = '#';

  m_GTBStartTime             = CTimeValue(0.0);
  m_GTBEndTime               = CTimeValue(3600.0);
  m_GTBTrackingTime          = 1;

  m_SpModelFixupStrategy     = FUS_Largest;

  };

//---------------------------------------------------------------------------

flag CProjectSettings::ReadSettings(CProfINIFile & PF, int PrjFileVerNo)
  {
  dbgpln("--------------------- CProjectSettings::Read  %s", PF.Filename());

#if WithRmtFile
  flag RmtOK=true;
  if (UsingPrjLclFiles())
    RmtOK=SetupRmtCopies();
#endif

  Strng sModelCfg = PF.RdStr("General", "ModelLibrary", "");
  sModelCfg = PF.RdStr("General", "ModelConfiguration", sModelCfg());
  sModelCfg = PF.RdStr("General", "CfgFile", sModelCfg());
  sModelCfg.FnExpand();

  SwingCfgFileIfReqd(sModelCfg);

  CProfINIFile PrjCfg(sModelCfg());

  PrjFileVerNo=PF.RdInt("General", "PrjFileVersion", PrjFileVerNo);

  if (PrjFileVerNo>=100)
    {
    m_NetMode     = GetDefNetMode (PF.RdStr("Modes", "NetMode",           PrjCfg.RdStr("Modes", "Default_NetMode", "ProBal")));

    m_PBNodeMode  = GetDefNodeMode(PF.RdStr("Modes", "Probal_NodeMode",   PrjCfg.RdStr("Modes", "Probal_NodeMode",  "Direct")));
    m_PBLinkMode  = GetDefLinkMode(PF.RdStr("Modes", "Probal_LinkMode",   PrjCfg.RdStr("Modes", "Probal_LinkMode",  "Direct")));            
    m_PBFlowMode  = GetDefFlowMode(PF.RdStr("Modes", "Probal_FlowMode",   PrjCfg.RdStr("Modes", "Probal_FlowMode",  "Simple")));
    m_PBHeatMode  = GetDefHeatMode(PF.RdStr("Modes", "Probal_HeatMode",   PrjCfg.RdStr("Modes", "Probal_HeatMode",  "Rigorous")));

    m_DynNodeMode = GetDefNodeMode(PF.RdStr("Modes", "Dynamic_NodeMode",  PrjCfg.RdStr("Modes", "Dynamic_NodeMode", "Buffered")));
    m_DynLinkMode = GetDefLinkMode(PF.RdStr("Modes", "Dynamic_LinkMode",  PrjCfg.RdStr("Modes", "Dynamic_LinkMode", "Direct")));            
    m_DynFlowMode = GetDefFlowMode(PF.RdStr("Modes", "Dynamic_FlowMode",  PrjCfg.RdStr("Modes", "Dynamic_FlowMode", "Simple")));
    m_DynHeatMode = GetDefHeatMode(PF.RdStr("Modes", "Dynamic_HeatMode",  PrjCfg.RdStr("Modes", "Dynamic_HeatMode", "Rigorous")));

    m_MaxNodeMode = GetMaxNodeMode(PrjCfg.RdStr("Modes", "Maximum_NodeMode", "Buffered"));
    m_MaxLinkMode = GetMaxNodeMode(PrjCfg.RdStr("Modes", "Maximum_LinkMode", "Direct"));
    m_MaxFlowMode = GetMaxFlowMode(PrjCfg.RdStr("Modes", "Maximum_FlowMode", "Full"));                        
    m_MaxHeatMode = GetMaxHeatMode(PrjCfg.RdStr("Modes", "Maximum_HeatMode", "Rigorous"));                    
                                                                                                              
    }                                                                                                         
  else if (PrjFileVerNo>=73)
    {
    long NetMd=0, FlwMd=0;
    Strng sRMd=PF.RdStr("General", "RunMode",    "");
    if (sRMd.XStrNICmp("Probal", 6)==0)
      {
      NetMd=NM_Probal;
      FlwMd=LFM_Xfer;
      }
    else if (sRMd.XStrNICmp("Dynamic-Transfer", 16)==0)
      {
      NetMd=NM_Dynamic;
      FlwMd=LFM_Xfer;
      }
    else if (sRMd.XStrNICmp("Dynamic-Flow", 12)==0)
      {
      NetMd=NM_Dynamic;
      FlwMd=LFM_Simple;
      }
    else if (sRMd.XStrNICmp("Dynamic-Full", 12)==0)
      {
      NetMd=NM_Dynamic;
      FlwMd=LFM_Full;
      }
    else
      {
      ASSERT_ALWAYS(FALSE, "Bad RunMode in Old flowsheet");
      }

    long HtMd=0;
    Strng sHMd=PF.RdStr("General", "HeatMode",   "");
    if (sHMd.XStrICmp("Rigorous")==0)
      HtMd=HM_Full;
    else if (sHMd.XStrICmp("Simple")==0)
      HtMd=HM_Reduced;
    else if (sHMd.XStrICmp("None")==0)
      HtMd=HM_None;
    else 
      ASSERT_ALWAYS(FALSE, "Bad HeatMode in Old flowsheet");

    m_NetMode=NetMd;
    
    m_PBNodeMode=SM_Direct;
    m_PBLinkMode=SM_Direct;
    m_PBFlowMode=FlwMd;
    m_PBHeatMode=HtMd;

    m_DynNodeMode=SM_Buffered;
    m_DynLinkMode=SM_Direct;
    m_DynFlowMode=FlwMd;
    m_DynHeatMode=HtMd;

    m_MaxNodeMode=GetMaxNodeMode(PrjCfg.RdStr("Modes", "Maximum_NodeMode", "Buffered"));
    m_MaxLinkMode=GetMaxNodeMode(PrjCfg.RdStr("Modes", "Maximum_LinkMode", "Direct"));;
    m_MaxFlowMode=GetMaxFlowMode(PrjCfg.RdStr("Modes", "Maximum_FlowMode", "Full"));
    m_MaxHeatMode=GetMaxHeatMode(PrjCfg.RdStr("Modes", "Maximum_HeatMode", "Rigorous"));
    }
  else
    {
    long NetMd=0, FlwMd=0;

    bool PB = (PF.RdInt("General", "ProBalMode", true)!=0);
    if (PB)
      {
      NetMd=NM_Probal;
      FlwMd=LFM_Xfer;
      }
    else
      {//NOT probal
      int FlowMd=PF.RdInt("General", "DynamicFlowMode", (PrjFileVerNo>=72));
      if (FlowMd)
        {
        NetMd=NM_Dynamic;
        FlwMd=LFM_Xfer;
        }
      else
        {
        NetMd=NM_Dynamic;
        FlwMd=LFM_Full;
        }
      }
    
    m_NetMode=NetMd;
    
    m_PBNodeMode=SM_Direct;
    m_PBLinkMode=SM_Direct;
    m_PBFlowMode=FlwMd;
    m_PBHeatMode=HM_Full;

    m_DynNodeMode=SM_Buffered;
    m_DynLinkMode=SM_Direct;
    m_DynFlowMode=FlwMd;
    m_DynHeatMode=HM_Full;

    m_MaxNodeMode=GetMaxNodeMode(PrjCfg.RdStr("Modes", "Maximum_NodeMode", "Buffered"));
    m_MaxLinkMode=GetMaxNodeMode(PrjCfg.RdStr("Modes", "Maximum_LinkMode", "Direct"));;
    m_MaxFlowMode=GetMaxFlowMode(PrjCfg.RdStr("Modes", "Maximum_FlowMode", "Full"));
    m_MaxHeatMode=GetMaxHeatMode(PrjCfg.RdStr("Modes", "Maximum_HeatMode", "Rigorous"));
    }

  m_lStatsRefreshCnt      =   Max(1L, PF.RdLong("General", "StatsRefreshCount", m_lStatsRefreshCnt));

  m_bRunOptEmptyAll       =   PF.RdLong("RunStartup", "EmptyAll",     m_bRunOptEmptyAll     ?1:0)!= 0;
  m_bRunOptEmptySpills    =   PF.RdLong("RunStartup", "EmptySpills",  m_bRunOptEmptySpills  ?1:0)!= 0;
  m_bRunOptZeroFlows      =   PF.RdLong("RunStartup", "ZeroFlows",    m_bRunOptZeroFlows    ?1:0)!= 0;    
  m_bRunOptInitialiseAll  =   PF.RdLong("RunStartup", "InitialiseAll",m_bRunOptInitialiseAll?1:0)!= 0;
  m_bRunOptResetStats     =   PF.RdLong("RunStartup", "ResetStats",   m_bRunOptResetStats   ?1:0)!= 0;   
  m_bRunOptOnceOnly       =   PF.RdLong("RunStartup", "OnceOnly",     m_bRunOptOnceOnly    ?1:0)!= 0;    

  if (PrjFileVerNo >= 98)
    {
    m_TheTime             =  CTimeValue(PF.RdInt64("General", "ExecTime", m_TheTime.Raw));
    m_StepSizeMax         =  CTimeValue(PF.RdInt64("General", "StepSizeMax", m_StepSizeMax.Raw));
    m_ScnStopTime         =  CTimeValue(PF.RdInt64("General", "ScnStopTime", m_ScnStopTime.Raw));
    m_ScnDuration         =  CTimeValue(PF.RdInt64("General", "ScnDuration", m_ScnDuration.Raw));
    }
  else
    {
    m_TheTime             =  CTimeValue(PF.RdDouble("General", "ExecTime", m_TheTime.Seconds));
    m_StepSizeMax         =  CTimeValue(PF.RdDouble("General", "StepSizeMax", m_StepSizeMax.Seconds));
    m_ScnStopTime         =  CTimeValue(PF.RdInt64("General", "ScnStopTime", m_ScnStopTime.Raw));
    m_ScnDuration         =  CTimeValue(PF.RdInt64("General", "ScnDuration", m_ScnDuration.Raw));
    //gs_Exec.SetTimeUntilStop(CTimeValue(PF.RdDouble("General", "TimeUntilStop", gs_Exec.TimeUntilStop.Seconds)));
    }

  m_RealTime              =  PF.RdLong("General", "RealTime", m_RealTime) != 0;
  m_RealTimeMult          =  PF.RdDouble("General", "RealTimeMult", m_RealTimeMult);
  m_SyncWithClock         =  PF.RdLong("General", "SyncWithClock", m_SyncWithClock) != 0;
  m_ScnType               =  (eScnTypes)PF.RdInt("General", "TermType", (int)m_ScnType);
  m_ScnTermAction         =  (eScnTermActions)PF.RdInt("General", "TermAction", (int)m_ScnTermAction);
  m_StepCountMax          =  PF.RdLong("General", "MaxRunIters", m_StepCountMax);

  m_sDefGrpLib            =   PF.RdStr("General", "DefaultLibrary", m_sDefGrpLib());
  m_sGrfFrameName         =   PF.RdStr("General", "GraphicsFrame", m_sGrfFrameName());
  m_sGrfFrameFilter       =   PF.RdStr("General", "GraphicsFilter", m_sGrfFrameFilter());

  m_GrfBehaviour          =   PF.RdLong("General",  "GrfBehaviour",  PrjFileVerNo>= 96?WB_Coincident:WB_None);
  m_TrndBehaviour         =   PF.RdLong("General", "TrndBehaviour", PrjFileVerNo>= 96?WB_Coincident:WB_None);

  m_bGrfDeferLoad         =   (PF.RdLong("General", "GrfDeferLoad", m_bGrfDeferLoad) !=  0);
  m_bGrfAutoLoad          =   (PF.RdLong("General", "GrfAutoLoad", m_bGrfAutoLoad) !=  0);
  m_bGrfUpdateAnnot       =   (PF.RdLong("General", "GrfUpdateAnnotation", m_bGrfUpdateAnnot) !=  0);
  m_bGrfAnimationOn       =   (PF.RdLong("General", "GrfAnimationOn", m_bGrfAnimationOn) !=  0);
  m_bGrfPromptForZoom     =   (PF.RdLong("General", "GrfPromptForZoom", m_bGrfPromptForZoom) !=  0);
  m_bGrfMoveCursor        =   (PF.RdLong("General", "GrfMoveCursor", m_bGrfMoveCursor) !=  0);
  m_bGrfRegExpOn          =   (PF.RdLong("General", "GrfRegExpOn", m_bGrfRegExpOn) !=  0);
  m_bReadOnlyPrj          =   (PF.RdLong("General", "ReadOnly", m_bReadOnlyPrj) !=  0);

  m_DisplayTagsOnly       = PF.RdLong("General", "DisplayTagsOnly", m_DisplayTagsOnly) != 0;
  m_HideZeros             = PF.RdLong("General", "HideZeros", m_HideZeros) != 0;
  m_ShowGroups            = PF.RdLong("General", "ShowGroups", m_ShowGroups) != 0;
  m_NumericTagsBad        = PF.RdLong("General", "NumericTagsBad", m_NumericTagsBad) != 0;
  m_NumericStartingTagsBad = PF.RdLong("General", "NumericStartingTagsBad", m_NumericStartingTagsBad) != 0;
  m_NonNumericTagChr      = (char)PF.RdLong("General", "NonNumericTagChr", m_NonNumericTagChr);
  if (!TaggedObject::CheckNonNumericTagChr(m_NonNumericTagChr))
    m_NonNumericTagChr = '#';
  m_NumericTagsBad = m_NumericTagsBad || m_NumericStartingTagsBad;

  m_bRemoveRevs        =(PF.RdLong("Database", "RemovePreviousRevisions", m_bRemoveRevs)!=0);
  m_bCopyDBFiles=(PF.RdLong("Database", "CopyDBFiles", m_bCopyDBFiles)!=0);

  if (PrjFileVerNo>=98)
    {
    m_GTBStartTime.Raw = PF.RdInt64("Trend", "GlobalStartTime", m_GTBStartTime.Raw);
    m_GTBEndTime.Raw   = m_GTBStartTime.Raw + PF.RdInt64("Trend", "GlobalDuration", m_GTBEndTime.Raw-m_GTBStartTime.Raw);
    }
  else
    {
    m_GTBStartTime = CTimeValue(PF.RdDouble("Trend", "GlobalStartTime", m_GTBStartTime.Seconds));
    m_GTBEndTime = CTimeValue(m_GTBStartTime.Seconds + PF.RdDouble("Trend", "GlobalDuration", m_GTBEndTime.Seconds-m_GTBStartTime.Seconds));
    }
  m_GTBTrackingTime = PF.RdInt("Trend", "GlobalTrackingTime", m_GTBTrackingTime)!=0;

  m_Solver.m_fStateWnd=PF.RdInt("Solver", "StatusWnd", m_Solver.m_fStateWnd);
  m_Solver.m_fStatePinned=PF.RdInt("Solver", "StatusPinned", m_Solver.m_fStatePinned);
  m_Solver.m_fStateErrors=PF.RdInt("Solver", "StatusErrors", m_Solver.m_fStateErrors);
  m_Solver.m_iStateX=PF.RdInt("Solver", "StatusX", m_Solver.m_iStateX);
  m_Solver.m_iStateY=PF.RdInt("Solver", "StatusY", m_Solver.m_iStateY);

  m_Hst.ReadSettings(PF);

#if WITHNETSERVER
  OpenNetManager();
  s = PF.RdStr("NetWorkManager", "ClientServer", "");
  pCS_Mngr->SetServer(PF.RdLong("NetWorkManager", "ServerEnabled", pCS_Mngr->ServerEnabled()) !=0);
  gs_Exec.SetCoupling((ExecCoupling)PF.RdLong("NetWorkManager", "Coupling",(long)gs_Exec.Coupling()));
  pCS_Mngr->SetClientConnect(PF.RdLong("NetWorkManager", "ClientConnect", pCS_Mngr->ClientConnected()) != 0,
    s() ? s() : "", gs_Exec.Coupling()==XC_Sync);
#endif

  m_bDefaultRecordIt = (PF.RdLong("General", "DefaultRecordIt", m_bDefaultRecordIt) != 0);
  m_bFlashTrend = (PF.RdLong("General", "FlashTrend", m_bFlashTrend) != 0);
  m_bShowNewTrndLineDlg = (PF.RdLong("General", "ShowNewTrndLineDlg", m_bShowNewTrndLineDlg) != 0);
  m_bToolTips = (PF.RdLong("General", "ToolBarTips", m_bToolTips) != 0);
  m_bFlyBys = (PF.RdLong("General", "ToolBarFlyBys", m_bFlyBys) != 0);

#if WITHSCRCYCLES
  int i;
  //get screen saver info...
  bSystemScreenSave = (PF.RdLong("ScreenSaver", "SystemOn", bSystemScreenSave) != 0);
  bSyscadScreenSave = (PF.RdLong("ScreenSaver", "SysCADOn", bSyscadScreenSave) != 0);
  iScrCycles = PF.RdInt("ScreenSaver", "NoOfCycles", iScrCycles);
  if (Cycles)
    delete []Cycles;
  Cycles = new CScreenSaverInfo[iScrCycles];
  for (i=0; i<iScrCycles; i++)
    {
    char Section[256];
    char Item[256];
    sprintf(Section, "Cycle%i", i);
    Cycles[i].iWaitTime = (WORD)PF.RdLong(Section, "WaitTime", 10);
    s = PF.RdStr(Section, "Description", Section);
    Cycles[i].sDescription = s();
    int WndCnt = PF.RdInt(Section, "WindowCount", 0);
    Cycles[i].Windows.SetSize(WndCnt);
    for (int j=0; j<WndCnt; j++)
      {
      sprintf(Item, "WindowTitle%i", j);
      s = PF.RdStr(Section, Item, "Command");
      Cycles[i].Windows.SetAt(j, s());
      }
    }
#endif

#if WITHDRVMAN
  //get driver info...
  sDrvManagerName = PF.RdStr("DrvManager", "Name", sDrvManagerName());
  if (sDrvManagerName.Find('.')<0)
    sDrvManagerName += ".SCF";
  sDrvManagerName.FnContract();

  bDrvOn = (PF.RdLong("DrvManager", "On", bDrvOn) != 0);
  bDrvLclTagSrvrOK= (PF.RdLong("DrvManager", "LclTagSrvrOK", bDrvLclTagSrvrOK) != 0);
  bDrvReadAll = (PF.RdLong("DrvManager", "ReadAll", bDrvReadAll) != 0);
  bDrvIgnoresOn = (PF.RdLong("DrvManager", "IgnoresOn", bDrvIgnoresOn) != 0);
  bDrvForcesOn = (PF.RdLong("DrvManager", "ForcesOn", bDrvForcesOn) != 0);
  bDrvNoiseOn = (PF.RdLong("DrvManager", "NoiseOn", bDrvNoiseOn) != 0);
  bDrvStartFBKCheck = (byte)PF.RdLong("DrvManager", "StartFBKCheck ", FBKCheckNever);
#endif

  m_sArcManagerName = PF.RdStr("ArcManager", "Name", m_sArcManagerName());
  m_bArcOn = (PF.RdLong("ArcManager", "On", m_bArcOn) != 0);
  if (PrjFileVerNo<101)
    {
    m_sArcManagerName = "$Prj\\Archive.sac";
    //m_bArcOn = true;
    }
  //m_bArcOpenDBOnRun = (PF.RdLong("ArcManager", "OpenDBOnRun", m_bArcOpenDBOnRun) != 0);
  m_sArcManagerName.FnContract();

  m_sIOMarshalName = PF.RdStr("IOMarshal", "Name", m_sIOMarshalName());
  m_sIOMarshalName.FnCheckExtension(".scm");
  m_bIOMOn = (PF.RdLong("IOMarshal", "On", m_bIOMOn) != 0);
  m_sIOMarshalNode = PF.RdStr("IOMarshal", "Node", m_sIOMarshalNode());
  if (m_sIOMarshalNode.Len()==0) 
    m_sIOMarshalName.FnContract();
  else
    m_sIOMarshalName.FnExpand();

  //get report info...
  m_ReportInfo.iHSearchLen = Range(1, PF.RdInt("Reports", "HSearchLen", m_ReportInfo.iHSearchLen), (int)MaxCSVCols);
  m_ReportInfo.iVSearchLen = Range(1, PF.RdInt("Reports", "VSearchLen", m_ReportInfo.iVSearchLen), 2048);
  m_ReportInfo.sNan = PF.RdStr("Reports", "Nan", m_ReportInfo.sNan());
  m_ReportInfo.sBlank = PF.RdStr("Reports", "Blank", m_ReportInfo.sBlank());
  m_ReportInfo.sIgnoreChars = PF.RdStr("Reports", "IgnoreChars", m_ReportInfo.sIgnoreChars());
  m_bRptExcelLock = (PF.RdLong("Reports", "ExcelLock", m_bRptExcelLock) != 0);
  m_bRptExcelCellName = (PF.RdLong("Reports", "ExcelUseCellName", m_bRptExcelCellName) != 0);
  m_bRptExcelMakeActive = (PF.RdLong("Reports", "ExcelAlmaysMakeActive", m_bRptExcelMakeActive) != 0);
  m_bRptExcelSysCADActive = (PF.RdLong("Reports", "ExcelReturnSysCADtoActive", m_bRptExcelSysCADActive) != 0);
  m_bRptExcelUpdateLinks = (PF.RdLong("Reports", "ExcelUpdateLinks", m_bRptExcelUpdateLinks) != 0);
  m_bRptExcelSaveOnComplete = (PF.RdLong("Reports", "ExcelSaveOnComplete", m_bRptExcelSaveOnComplete) != 0);
  m_bRptSyscadTags = (PF.RdLong("Reports", "SyscadTags", m_bRptSyscadTags) != 0);
  m_bAccAutoSaveOn = (PF.RdLong("Access", "AutoSaveOn", m_bAccAutoSaveOn) != 0);
  m_bAccUpdateOnRun = (PF.RdLong("Access", "UpdateOnRun", m_bAccUpdateOnRun) != 0);
  m_bAccHistoryInfoVis = (PF.RdLong("Access", "HistoryInfoVis", m_bAccHistoryInfoVis) != 0);

  //LoadColours(PF);

  m_bDDEOn = (PF.RdLong("DDEManager", "On", m_bDDEOn) != 0);
  //OpenDDEManager();

  m_bOPCOn = (PF.RdLong("OPCManager", "On", m_bOPCOn) != 0);
  m_bOPCResetReg = (PF.RdLong("OPCManager", "ResetRegistry", m_bOPCResetReg) != 0);
  m_iOPCServerNo = PF.RdLong("OPCManager", "ServerNumber", m_iOPCServerNo);

  m_SpModelFixupStrategy = PF.RdLong("SpModel", "FixupStrategy", m_SpModelFixupStrategy);

  return true;
  };

//---------------------------------------------------------------------------

flag CProjectSettings::WriteSettings(CProfINIFile & PF, bool FullSave)
  {
  dbgpln("--------------------- CProjectSettings::Write %s", PF.Filename());

  Strng s;
  //if (bDoingSaveAs && !AllGrfLoaded())
  //  {//Copy all DXF files...
  //  s = m_sPrevPrjFiles;
  //  s += "*.dxf";
  //  WIN32_FIND_DATA fd;
  //  HANDLE H = FindFirstFile(s(), &fd);
  //  flag AllDone = (H==INVALID_HANDLE_VALUE);
  //  while (!AllDone)
  //    {
  //    s = m_sPrevPrjFiles;
  //    s += fd.cFileName;
  //    Copy_File(s(), PrjFiles());
  //    AllDone = !FindNextFile(H, &fd);
  //    }
  //  FindClose(H);
  //  }

  //Strng sPrjBac(PrjFile());
  //sPrjBac = sPrjBac.Left(sPrjBac.Length()-3);
  //sPrjBac += "bac";
  //CopyFile(PrjFile(), sPrjBac(), false); //backup spj

  char DT[64],TM[64];
#if _MSC_VER>=1400
  _strdate_s(DT, sizeof(DT));
  _strtime_s(TM, sizeof(TM));
#else
  _strdate(DT);
  _strtime(TM);
#endif
  //DeleteFile(PrjFile()); //delete old spj file

  char UserBuff[512];
  char CompBuff[512];
  DWORD Sz = sizeof(UserBuff);
  if (!GetUserName(UserBuff, &Sz))
    strcpy(UserBuff, "?????");
  Sz = sizeof(CompBuff);
  if (GetComputerName(CompBuff, &Sz))
    strcpy(CompBuff, "?????");

  PF.WrInt("General", "PrjFileVersion", LatestPrjFileVer);
  PF.WrInt("General", "PrjFileVersionOriginal", OrigPrjFileVerNo());
  PF.WrStr("LastSaveInfo", "SysCADVersion", FullVersion());

  if (FullSave)
    {
    PF.WrStr("General", "PrjName", PrjName());
    PF.WrStr("General", "PrjFiles", PrjFiles());

    PF.WrStr("General", "CfgFile", FnPathRelContract(CfgFile()));
    PF.WrInt("General", "LocalPrjFiles", UsingPrjLclFiles());
    }

  s="-";
  if (strlen(SCD_PATCHNOTE)>0)
    {
    if (strlen(SCD_PATCHDATE)>0)
      s.Set("%s  (%s)", SCD_PATCHNOTE, SCD_PATCHDATE);
    else
      s.Set("%s", SCD_PATCHNOTE);
    }
  PF.WrStr("LastUpdateInfo", "SysCAD_Update", s());
  PF.WrStr("LastUpdateInfo", "LastUpdateDate", DT);
  PF.WrStr("LastUpdateInfo", "LastUpdateTime", TM);
  PF.WrStr("LastUpdateInfo", "WinNT", IsWinNT ? "y" : "n");
  PF.WrStr("LastUpdateInfo", "UserName", UserBuff);
  PF.WrStr("LastUpdateInfo", "ComputerName", CompBuff);


  PF.WrInt64("General", "ExecTime",               m_TheTime.Raw);
  PF.WrInt64("General", "StepSizeMax",            m_StepSizeMax.Raw);
  PF.WrLong("General", "RealTime",                m_RealTime != 0);
  PF.WrDouble("General", "RealTimeMult",          m_RealTimeMult);
  PF.WrLong("General", "SyncWithClock",           m_SyncWithClock != 0);

  PF.WrInt("General", "TermType",                 (int)m_ScnType);
  PF.WrInt("General", "TermAction",               (int)m_ScnTermAction);
  PF.WrInt64("General", "ScnStopTime",            m_ScnStopTime.Raw);
  PF.WrInt64("General", "ScnDuration",            m_ScnDuration.Raw);
  PF.WrLong("General", "MaxRunIters",             m_StepCountMax);
  PF.WrLong("General", "ToolBarTips",             m_bToolTips);
  PF.WrLong("General", "ToolBarFlyBys",           m_bFlyBys);
  PF.WrLong("General", "DefaultRecordIt",         m_bDefaultRecordIt);
  PF.WrLong("General", "FlashTrend",              m_bFlashTrend);
  PF.WrLong("General", "ShowNewTrndLineDlg",      m_bShowNewTrndLineDlg);
  PF.WrLong("General", "StatsRefreshCount",       Max(1L, m_lStatsRefreshCnt));
  //PF.WrInt("General", "ProBalMode", GetProBalMode());
  //PF.WrInt("General", "DynamicFlowMode", GetDynModeFlow());
  //PF.WrStr("General", "SolveMode", gs_Exec.GlblRunModesString());

  PF.WrStr("Modes", "NetMode",                 GetNetModeStr(m_NetMode));

  PF.WrStr("Modes", "Probal_NodeMode",         GetNodeModeStr(m_PBNodeMode));
  PF.WrStr("Modes", "Probal_LinkMode",         GetLinkModeStr(m_PBLinkMode));
  PF.WrStr("Modes", "Probal_HeatMode",         GetHeatModeStr(m_PBHeatMode));
  PF.WrStr("Modes", "Probal_FlowMode",         GetFlowModeStr(m_PBFlowMode));

  PF.WrStr("Modes", "Dynamic_NodeMode",        GetNodeModeStr(m_DynNodeMode));
  PF.WrStr("Modes", "Dynamic_LinkMode",        GetLinkModeStr(m_DynLinkMode));
  PF.WrStr("Modes", "Dynamic_HeatMode",        GetHeatModeStr(m_DynHeatMode));
  PF.WrStr("Modes", "Dynamic_FlowMode",        GetFlowModeStr(m_DynFlowMode));

  PF.WrStr("General", "DefaultLibrary",           m_sDefGrpLib());
  PF.WrStr("General", "GraphicsFrame",            m_sGrfFrameName());
  PF.WrStr("General", "GraphicsFilter",           m_sGrfFrameFilter());
  PF.WrLong("General", "GrfDeferLoad",            m_bGrfDeferLoad);
  PF.WrLong("General", "GrfAutoLoad",             m_bGrfAutoLoad);
  PF.WrLong("General", "GrfUpdateAnnotation",     m_bGrfUpdateAnnot);
  PF.WrLong("General", "GrfAnimationOn",          m_bGrfAnimationOn);
  PF.WrLong("General", "GrfPromptForZoom",        m_bGrfPromptForZoom);
  PF.WrLong("General", "GrfRegExpOn",             m_bGrfRegExpOn);
  PF.WrLong("General", "ReadOnly",                m_bReadOnlyPrj);
  PF.WrLong("General", "DisplayTagsOnly",         m_DisplayTagsOnly);
  PF.WrLong("General", "HideZeros",               m_HideZeros);
  PF.WrLong("General", "ShowGroups",              m_ShowGroups);
  PF.WrLong("General", "NumericTagsBad",          m_NumericTagsBad);
  PF.WrLong("General", "NumericStartingTagsBad",  m_NumericStartingTagsBad);
  PF.WrLong("General", "NonNumericTagChr",        m_NonNumericTagChr);

  PF.WrLong("General", "GrfBehaviour",            m_GrfBehaviour);
  PF.WrLong("General", "TrndBehaviour",           m_TrndBehaviour);

  PF.WrLong("RunStartup", "EmptyAll",             m_bRunOptEmptyAll     ?1:0);
  PF.WrLong("RunStartup", "EmptySpills",          m_bRunOptEmptySpills  ?1:0);
  PF.WrLong("RunStartup", "ZeroFlows",            m_bRunOptZeroFlows    ?1:0);    
  PF.WrLong("RunStartup", "InitialiseAll",        m_bRunOptInitialiseAll?1:0);
  PF.WrLong("RunStartup", "ResetStats",           m_bRunOptResetStats   ?1:0);   
  PF.WrLong("RunStartup", "OnceOnly",             m_bRunOptOnceOnly    ?1:0);    

  PF.WrLong("Database", "RemovePreviousRevisions", m_bRemoveRevs);
  PF.WrLong("Database", "CopyDBFiles",            m_bCopyDBFiles);

  PF.WrInt("Solver", "StatusWnd",                 m_Solver.m_fStateWnd);
  PF.WrInt("Solver", "StatusPinned",              m_Solver.m_fStatePinned);
  PF.WrInt("Solver", "StatusErrors",              m_Solver.m_fStateErrors);
  PF.WrInt("Solver", "StatusX",                   m_Solver.m_iStateX);
  PF.WrInt("Solver", "StatusY",                   m_Solver.m_iStateY);

  PF.WrLong("SpModel", "FixupStrategy",           m_SpModelFixupStrategy );

  if (PrjFileVerNo()>=98)
    {
    PF.WrInt64("Trend", "GlobalStartTime",        m_GTBStartTime.Raw);
    PF.WrInt64("Trend", "GlobalDuration",         m_GTBEndTime.Raw-m_GTBStartTime.Raw);
    }
  else
    {
    __debugbreak();
    //PF.WrDouble("Trend", "GlobalStartTime", CTagVwDoc::GTB.StartTime);
    //PF.WrDouble("Trend", "GlobalDuration", CTagVwDoc::GTB.Duration());
    }
  PF.WrInt("Trend", "GlobalTrackingTime",         m_GTBTrackingTime);


  //gs_HstMngr.GetOnRqd();
  gs_HstMngr.WriteSettings(PF);

#if WITHDRVMAN
  PF.WrStr("DrvManager", "Name", sDrvManagerName());
  PF.WrLong("DrvManager", "On", bDrvOn);
  PF.WrLong("DrvManager", "LclTagSrvrOK", bDrvLclTagSrvrOK);
  PF.WrLong("DrvManager", "ReadAll", bDrvReadAll);
  PF.WrLong("DrvManager", "IgnoresOn", bDrvIgnoresOn);
  PF.WrLong("DrvManager", "ForcesOn", bDrvForcesOn);
  PF.WrLong("DrvManager", "NoiseOn", bDrvNoiseOn);
  PF.WrLong("DrvManager", "StartFBKCheck ", bDrvStartFBKCheck );
#endif

  PF.WrStr("ArcManager", "Name", m_sArcManagerName());
  PF.WrLong("ArcManager", "On", m_bArcOn);
  PF.WrLong("ArcManager", "OpenDBOnRun", m_bArcOpenDBOnRun);

  PF.WrLong("DDEManager", "On", m_bDDEOn);

  PF.WrLong("OPCManager", "On", m_bOPCOn);
  PF.WrLong("OPCManager", "ResetRegistry", m_bOPCResetReg);
  PF.WrLong("OPCManager", "ServerNumber", m_iOPCServerNo);
  SaveMainOPCOptions(PF);
  //DisableMainOPCSrvr();
  //if (pOPCSrvr && pOPCSrvr->OPCSrvrMngr())
  //  pOPCSrvr->OPCSrvrMngr()->LoadOrSaveOptions(PF, false);

  PF.WrLong("IOMarshal", "On", m_bIOMOn);
  m_sIOMarshalNode.LRTrim();
  if (m_sIOMarshalNode())
    {
    Strng S(m_sIOMarshalName);
    S.FnExpand();
    PF.WrStr("IOMarshal", "Name", S());
    }
  else
    PF.WrStr("IOMarshal", "Name", m_sIOMarshalName());
  PF.WrStr("IOMarshal", "Node", m_sIOMarshalNode());

  PF.WrInt("Reports", "HSearchLen", m_ReportInfo.iHSearchLen);
  PF.WrInt("Reports", "VSearchLen", m_ReportInfo.iVSearchLen);
  PF.WrStr("Reports", "Nan", m_ReportInfo.sNan());
  PF.WrStr("Reports", "Blank", m_ReportInfo.sBlank());
  PF.WrStr("Reports", "IgnoreChars", m_ReportInfo.sIgnoreChars());
  PF.WrLong("Reports", "ExcelLock", m_bRptExcelLock);
  PF.WrLong("Reports", "ExcelUseCellName", m_bRptExcelCellName);
  PF.WrLong("Reports", "ExcelAlmaysMakeActive", m_bRptExcelMakeActive);
  PF.WrLong("Reports", "ExcelReturnSysCADtoActive", m_bRptExcelSysCADActive);
  PF.WrLong("Reports", "ExcelUpdateLinks", m_bRptExcelUpdateLinks);
  PF.WrLong("Reports", "ExcelSaveOnComplete", m_bRptExcelSaveOnComplete);
  PF.WrLong("Reports", "SyscadTags", m_bRptSyscadTags);
  PF.WrLong("Access", "AutoSaveOn", m_bAccAutoSaveOn);
  PF.WrLong("Access", "UpdateOnRun", m_bAccUpdateOnRun);
  PF.WrLong("Access", "HistoryInfoVis", m_bAccHistoryInfoVis);

#if WITHNETSERVER
  PF.WrLong("NetWorkManager", "ServerEnabled", pCS_Mngr->ServerEnabled());
  PF.WrLong("NetWorkManager", "ClientConnect", pCS_Mngr->ClientConnected());
  PF.WrStr("NetWorkManager", "ClientServer", pCS_Mngr->ClientServer());
  PF.WrLong("NetWorkManager", "Coupling",(long)gs_Exec.Coupling());
#endif

  int i;
#if WITHSCRCYCLES
  PF.WrLong("ScreenSaver", "SystemOn", bSystemScreenSave);
  PF.WrLong("ScreenSaver", "SysCADOn", bSyscadScreenSave);
  PF.WrInt("ScreenSaver", "NoOfCycles", iScrCycles);
  for (i=0; i<iScrCycles; i++)
    {
    char Section[256];
    char Item[256];
    sprintf(Section, "Cycle%i", i);
    PF.WrLong(Section, "WaitTime", Cycles[i].iWaitTime);
    PF.WrStr(Section, "Description", (char*)(const char*)(Cycles[i].sDescription));
    PF.WrInt(Section, "WindowCount", Cycles[i].Windows.GetSize());
    for (int j=0; j<Cycles[i].Windows.GetSize(); j++)
      {
      sprintf(Item, "WindowTitle%i", j);
      PF.WrStr(Section, Item, (char*)(const char*)(Cycles[i].Windows.GetAt(j)));
      }
    }
#endif

  for (i=0; i<16; i++)
    {
    if (gs_CustomColours[i])
      {
      char Item[32];
      sprintf(Item, "Colour%d", i);
      PF.WrLong("CustomColours", Item, gs_CustomColours[i]);
      }
    }

  ////save documents...
  //for (int j=iGraphTemplate; j<=iControlTemplate; j++)
  //  if (ScdApp()->TemplateExists(j))
  //    {
  //    Strng DocTypeName=DocumentTmplNames[j];
  //    CDocTemplate & Templ = ScdApp()->Template(j);
  //    POSITION Pos = Templ.GetFirstDocPosition();
  //    int DocCount = 0;
  //    PF.WrInt(DocumentSectNames[j], "DocumentCount", DocCount); // Initialise
  //    while (Pos)
  //      {
  //      CDocument* pDoc = Templ.GetNextDoc(Pos);

  //      CString dn = pDoc->GetPathName();
  //      i = dn.ReverseFind('\\');
  //      if (m_Save.m_bIsNewPrj && i>=0)
  //        {
  //        CString d;
  //        d = PrjFiles();
  //        d += dn.Right(dn.GetLength()-i-1);
  //        dn = d;
  //        }

  //      s = dn;
  //      //s.FnCompactPath(50);
  //      s.FnNameExt();
  //      CDlgBusy::SetLine(3, s());
  //      if (pDoc->IsKindOf(RUNTIME_CLASS(DocRoot)))
  //        {
  //        ((DocRoot*)pDoc)->OnActivate(true);  // direct cmds to this document
  //        ((DocRoot*)pDoc)->SaveDocument((LPCTSTR)dn);
  //        }
  //      else if (pDoc->IsKindOf(RUNTIME_CLASS(OleDocRoot)))
  //        {
  //        if (dn.GetLength()==0)
  //          dn=((OleDocRoot*)pDoc)->m_strReqdPathName;
  //        pDoc->DoSave((LPCTSTR)dn, TRUE);
  //        }

  //      dn = pDoc->GetPathName();
  //      if (dn.GetLength()>0)
  //        {
  //        char Entry[256];
  //        sprintf(Entry, "Doc%i", DocCount++);
  //        PF.WrStr(DocumentSectNames[j], Entry, FnContract(s, (LPTSTR)(LPCTSTR)dn));
  //        }
  //      }
  //    PF.WrInt(DocumentSectNames[j], "DocumentCount", DocCount);
  //    }

  //gs_AccessWnds.SaveNdHist(-1);

  ////save list of all files referenced/used by project...
  //CDlgBusy::SetLine(3, "Copying 'Remote' files");

  //int NCopied = CopyOtherFiles(PFL, PF);

  //CDlgBusy::SetLine(3, CompactPrj());
  //if (NCopied>0)
  //  //LogNote("SysCAD", 0, "%i Files Copied to %s", NCopied, BackupFilesFolderName());
  //  LogNote("SysCAD", 0, "%i Files Copied", NCopied);

  //SaveColours(PF);

  //DWORD Opts=0;
  //if (!m_Save.m_bRqstNew)
  //  {
  //  if (m_bRemoveRevs)
  //    Opts |= DBO_RemovePrevRevs;
  //  //if (m_bCopyDBFiles && (PrjFileVerNoAsLoaded()==PrjFileVerNo()))
  //  //  Opts |= DBO_CopyOldDBFiles;
  //  }

  //gs_pPrj->DoSaveDB(XDBFormat(), Opts, PrjFiles(), (bDoingSaveAs||bDoingSaveAsNV ? m_sPrevPrjFiles() : NULL));

  //SaveWindowLayout();

  CProfINIFile PFI(PrjIniFile());
  PFI.WrLong("SnapShot", "Action", m_eSSAction);
  PFI.WrStr("SnapShot", "Name", m_sSSName());
  PFI.WrStr("SnapShot", "Prefix", m_sSSPrefix());
  PFI.WrLong("SnapShot", "Number", m_iSSNumber);
  PFI.WrLong("SnapShot", "DoZip", m_bSSDoZip);


  //if (CfgFile())
  //  ScdPFUser.WrStr("General", "LastCfgFile", CfgFile());

  return true;
  };

//---------------------------------------------------------------------------

flag CProjectSettings::EditSettings(LPCTSTR Title, CProject *pPrj)
  {
  CProjectSettingsDlg Dlg(true, pPrj, this, Title, MainWnd(), 0); // WhichPage>=0 ? WhichPage : PF.RdInt("General", "PrjOptionsPageNo", 0));
  return Dlg.DoModal()==IDOK;
  }

//===========================================================================
//
//
//
//===========================================================================

CProject * gs_pPrj=NULL;
bool CProject::sm_fOpenAltCfg=false;
PrjLoadTypes CProject::sm_LoadTypeRqst=PLT_Null;
bool CProject::sm_SysCADInited=false;

//---------------------------------------------------------------------------

CProject::CProject() : CProjectSettings(gs_HstMngr)
  {
  SetNewPrjFileVerNo(LatestPrjFileVer);

#if WITHSCRCYCLES
  Cycles = NULL;
  CycleWnds = NULL;
  CyclePos = NULL;
  pCycleDescWnd = NULL;
#endif

  m_pFlwLib = NULL;
  Clear();

  EO_Register(pExecName_Project, EOExec_None, 0, 0);
  SetPrjFileVerNoAsLoaded(LatestPrjFileVer);

  m_bWithVersion = WithVersionDefault;
  m_bRemoveOld   = RemoveOldDefault;

#if CREATENETSERVER
  m_pCLRSrvr = NULL;
#endif
  }

//---------------------------------------------------------------------------

CProject::~CProject()
  {
  //delete pFlwLib;

  EO_DeRegister();

  delete pDDESrvr;
  //delete pOPCSrvr;

#if WITHSCRCYCLES
  if (Cycles)
    delete []Cycles;
  if (CycleWnds)
    delete []CycleWnds;
  if (CyclePos)
    delete []CyclePos;
#endif
  }

//---------------------------------------------------------------------------

void CProject::Clear()
  {
  ClearSettings();

  bForceSaveAsNV=false;
  bDoingSaveAs = false;
  bDoingSaveAsNV = false;
  bDoingSaveAsWithName = false;
  bDoingLoad = false;
  pPrjDoc=NULL;
  //  sPrjFile="";
  bCmdSuccess=0;
#if WITHDRVMAN
  bDrvOpen=0;
  bDrvOn=0;
  bDrvLclTagSrvrOK=1;
  bDrvReadAll=0;
#endif
  bArcOpen=0;
  m_bArcOn=0;
  //m_bArcOpenDBOnRun=0;
  bIOMOpen=0;
  //m_bIOMOn=0;

  //m_bDDEOn=0;
  pDDESrvr=NULL;
  bHstOK=0;
  bCommsOpen=0;
#if WITHDRVMAN
  bDrvIgnoresOn=0;
  bDrvForcesOn=0;
  bDrvNoiseOn=1;
  bDrvStartFBKCheck=FBKCheckNever;
#endif
  m_Hst.m_OnRqd=true; //  gs_HstMngr.SetHstOn(true);
  bTimeChanged=0;
  bHstChanged=0;
  bDrvChanged=0;
  bArcChanged=0;
  bDocChanged=0;
  bLoadBusy=0;
  bChangedGrfMenu=0;
  bChangedRuntimeMenu=0;
  bDoGrfAutoAccess=0;
  bBusyAnalysing=0;
#if WITHSCRCYCLES
  bSystemScreenSave=1;
  bSyscadScreenSave=0;
  if (Cycles)
    delete []Cycles;
  if (CycleWnds)
    delete []CycleWnds;
  CycleWnds = NULL;
  if (CyclePos)
    delete []CyclePos;
  CyclePos = NULL;
  if (pCycleDescWnd)
    delete pCycleDescWnd;
  pCycleDescWnd = NULL;
  pOnlyWnd = NULL;
  iScrCycles = 1;
  Cycles = new CScreenSaverInfo[iScrCycles];
  Cycles[0].iWaitTime = 10;
  iCycleNo=-1;
#endif
#if WITHNETSERVER
  bNetOpen=0;
#endif

  m_Solver.m_fStateWnd=true;
  m_Solver.m_fStatePinned=false;
  m_Solver.m_fStateErrors=false;
  m_Solver.m_iStateX=-10000;
  m_Solver.m_iStateY=-10000;

#if WITHNETSERVER
  pCS_Mngr->SetServer(false);
  pCS_Mngr->SetClientConnect(false, "", false);
#endif
  MdlLibs.UnLoadFlwDLLs();

  CRptTagLists::ShutDown();

  XSetDBFormat(ScdPFUser.RdInt("General", "PrjsDBFormat", DBConnect_JET2000));

  m_bWithVersion = false;
  m_bRemoveOld = false;

  m_Save.m_bRqstNew=0;
  m_Save.m_bIsNewPrj=0;

  InitColours();

  SetPrjFileVerNo(LatestPrjFileVer);
  SetNewPrjFileVerNo(LatestPrjFileVer);
  SetOrigPrjFileVerNo(LatestPrjFileVer);
  SetPrjFileVerNoAsLoaded(LatestPrjFileVer);
  //m_PrjFileVerNoAsLoaded=LatestPrjFileVer;

  if (CfgFile())
    {
    CProfINIFile PF(CfgFile());
    TaggedObject::NumericTagsBad = (PF.RdLong("General", "NumericTagsBad", TaggedObject::NumericTagsBad) != 0);
    TaggedObject::NumericStartingTagsBad = (PF.RdLong("General", "NumericStartingTagsBad", TaggedObject::NumericStartingTagsBad) != 0);
    TaggedObject::NumericTagsBad = TaggedObject::NumericTagsBad || TaggedObject::NumericStartingTagsBad;
    }
  else
    {
    TaggedObject::NumericTagsBad = true;
    TaggedObject::NumericStartingTagsBad = true;
    }

  m_RmtCpyFolder="";
  m_LastSnapLoadFn="";

  m_DlgCopyHist=false;
  for (int i=0; i<sizeof(m_DlgCopy[0])/sizeof(m_DlgCopy); i++)
    m_DlgCopy[i]=false;

  //m_RmtCpyLock.Close();
  }

//---------------------------------------------------------------------------

void CProject::SetTagMonitor()
  {
#if WITHTAGMONITOR    
  CProfINIFile Cfg(CfgFile());
  CFieldMonitor::m_On=Cfg.RdInt("General", "TagMonitor", false)!=0;
  if (CFieldMonitor::m_On)
    {
    if (AfxMessageBox("Do you want to run with the TagMonitor Set\n"
      "\n"
      "This Option can slow SysCAD down at runtime",
      MB_YESNO|MB_ICONEXCLAMATION)==IDNO)
      {
      CFieldMonitor::m_On=false;
      }
    }
#else
  CFieldMonitor::m_On=false;
#endif
  };

//---------------------------------------------------------------------------

void CProject::SetShowEditStatus(bool On)
  {
  if (On)
    m_dwMdlStatusOptions &= ~MSO_ShowRunInEdit;
  else
    m_dwMdlStatusOptions |= MSO_ShowRunInEdit;
  if (gs_pTheSFELib)
    gs_pTheSFELib->FE_LoadAllModelStatus();
  }

bool CProject::ShowEditStatus()
  {
  return (m_dwMdlStatusOptions & MSO_ShowRunInEdit)==0;
  }

//---------------------------------------------------------------------------

void CProject::InitColours()
  {
  for (int i=GR_FIRSTUSERCOLOR; i<=GR_LASTUSERCOLOR; i++)
    grl_set_win_color(i, GRREF_NEARWHITE);

  grl_set_win_color(GR_MDLSTATUS_ERROR,    GRREF_MDLSTATUS_ERROR);
  grl_set_win_color(GR_MDLSTATUS_WARNING,  GRREF_MDLSTATUS_WARNING);
  grl_set_win_color(GR_MDLSTATUS_TEAR,     GRREF_MDLSTATUS_TEAR);
  grl_set_win_color(GR_MDLSTATUS_MACMDL,   GRREF_MDLSTATUS_MACMDL);
  grl_set_win_color(GR_MDLSTATUS_OFF,      GRREF_MDLSTATUS_OFF);
  grl_set_win_color(GR_MDLSTATUS_ON,       GRREF_MDLSTATUS_ON);
  grl_set_win_color(GR_MDLSTATUS_LFLWP,    GRREF_MDLSTATUS_LFLWP);
  grl_set_win_color(GR_MDLSTATUS_LFLWN,    GRREF_MDLSTATUS_LFLWN);
  grl_set_win_color(GR_MDLSTATUS_LNOFLW,   GRREF_MDLSTATUS_LNOFLW);
  grl_set_win_color(GR_MDLSTATUS_CLEAR,    GRREF_MDLSTATUS_CLEAR);
  grl_set_win_color(GR_MDLSTATUS_UNKNOWN,  GRREF_MDLSTATUS_UNKNOWN);
  grl_set_win_color(GR_MDLSTATUS_INACTIVE, GRREF_MDLSTATUS_INACTIVE);
  grl_set_win_color(GR_MDLSTATUS_BLACK,    GRREF_MDLSTATUS_BLACK);
  grl_set_win_color(GR_MDLSTATUS_ELEC,     GRREF_MDLSTATUS_ELEC);

  grl_set_win_color(GR_MDLSTATUS_NETTQMP,  GRREF_MDLSTATUS_NETTQMP);
  grl_set_win_color(GR_MDLSTATUS_NETTQMN,  GRREF_MDLSTATUS_NETTQMN);
  grl_set_win_color(GR_MDLSTATUS_UFLW   ,  GRREF_MDLSTATUS_UFLW   );
  grl_set_win_color(GR_MDLSTATUS_UNOFLW ,  GRREF_MDLSTATUS_UNOFLW );
  grl_set_win_color(GR_MDLSTATUS_ISSRC  ,  GRREF_MDLSTATUS_ISSRC  );
  grl_set_win_color(GR_MDLSTATUS_ISSNK  ,  GRREF_MDLSTATUS_ISSNK  );
  grl_set_win_color(GR_MDLSTATUS_LXFER  ,  GRREF_MDLSTATUS_LXFER  );
  grl_set_win_color(GR_MDLSTATUS_LSIMPLE,  GRREF_MDLSTATUS_LSIMPLE);
  grl_set_win_color(GR_MDLSTATUS_LLINEAR,  GRREF_MDLSTATUS_LLINEAR);
  grl_set_win_color(GR_MDLSTATUS_LFULL  ,  GRREF_MDLSTATUS_LFULL  );

  grl_set_win_color(GR_OTHERGRAPHICS,      grl_get_win_color(GR_LIGHTGRAY));
  grl_set_win_color(GR_HIGHLIGHT,          grl_get_win_color(GR_LIGHTMAGENTA));
  };

//---------------------------------------------------------------------------

void CProject::SaveColours(CProfINIFile & PF)
  {
  PF.WrLong("StatusColours", "GrfShowStatusColours", m_bShowStatusColours);
  PF.WrLong("StatusColours", "Options",              m_dwMdlStatusOptions);
  //PF.WrLong("StatusColours", "GrfOverlayStatusPen",  iOverlayStatusPen);
  for (int i=GR_FIRSTUSERCOLOR; i<=GR_LASTUSERCOLOR; i++)
    {
    COLORREF rgb=grl_get_win_color(i);
    if (rgb!=GRREF_NEARWHITE)
      {
      Strng H, C;
      H.Set("%08x", rgb);
      C.Set("C%03i", i);
      PF.WrStr("StatusColours", C(), H());
      }
    }
  PF.WrLong("StatusColours", "ShowInActive",  GrfHelper.iShowMdlStatusInActive);
  PF.WrLong("StatusColours", "ShowError",     GrfHelper.iShowMdlStatusError);
  PF.WrLong("StatusColours", "ShowWarning",   GrfHelper.iShowMdlStatusWarning);
  PF.WrLong("StatusColours", "ShowTear",      GrfHelper.iShowMdlStatusTear);
  PF.WrLong("StatusColours", "ShowMacMdl",    GrfHelper.iShowMdlStatusMacMdl);
  PF.WrLong("StatusColours", "ShowOff",       GrfHelper.iShowMdlStatusOff);
  PF.WrLong("StatusColours", "ShowOn",        GrfHelper.iShowMdlStatusOn);
  PF.WrLong("StatusColours", "ShowLFlwP",     GrfHelper.iShowMdlStatusLFlwP);
  PF.WrLong("StatusColours", "ShowLFlwN",     GrfHelper.iShowMdlStatusLFlwN);
  PF.WrLong("StatusColours", "ShowLNoFlw",    GrfHelper.iShowMdlStatusLNoFlw);
  PF.WrLong("StatusColours", "ShowClear",     GrfHelper.iShowMdlStatusClear);
  PF.WrLong("StatusColours", "ShowUnKnown",   GrfHelper.iShowMdlStatusUnKnown);
  PF.WrLong("StatusColours", "ShowNettQmP",   GrfHelper.iShowMdlStatusNettQmP);
  PF.WrLong("StatusColours", "ShowNettQmN",   GrfHelper.iShowMdlStatusNettQmN);
  PF.WrLong("StatusColours", "ShowUFlw",      GrfHelper.iShowMdlStatusUFlw);
  PF.WrLong("StatusColours", "ShowUNoFlw",    GrfHelper.iShowMdlStatusUNoFlw);
  PF.WrLong("StatusColours", "ShowIsSrc",     GrfHelper.iShowMdlStatusIsSrc);
  PF.WrLong("StatusColours", "ShowIsSnk",     GrfHelper.iShowMdlStatusIsSnk);
  PF.WrLong("StatusColours", "ShowXferLnk",   GrfHelper.iShowMdlStatusXferLnk);
  PF.WrLong("StatusColours", "ShowSimpleLnk", GrfHelper.iShowMdlStatusSimpleLnk);
  PF.WrLong("StatusColours", "ShowLinearLnk", GrfHelper.iShowMdlStatusLinearLnk);
  PF.WrLong("StatusColours", "ShowFullLnk",   GrfHelper.iShowMdlStatusFullLnk);
  }

//---------------------------------------------------------------------------

void CProject::LoadColours(CProfINIFile & PF)
  {
  m_bShowStatusColours = (PF.RdLong("StatusColours", "GrfShowStatusColours", m_bShowStatusColours) != 0);
  m_dwMdlStatusOptions = PF.RdLong("StatusColours", "Options", OrigPrjFileVerNo()<94?MSO_ShowRunInEdit: m_dwMdlStatusOptions);
  //iOverlayStatusPen = (byte)PF.RdLong("StatusColours", "GrfOverlayStatusPen", iOverlayStatusPen);
  if (PrjFileVerNo()>=94)
    {
    for (int i=GR_FIRSTUSERCOLOR; i<=GR_LASTUSERCOLOR; i++)
      {
      Strng C, H;
      C.Set("C%03i", i);
      H=PF.RdStr("StatusColours", C(), "");
      if (H.Length()>0)
        {
        COLORREF rgb;
        int nf=sscanf(H(), "%x", &rgb);
        grl_set_win_color(i, rgb);
        }
      }
    }

  if (PrjFileVerNo()<55)
    {
    GrfHelper.iShowMdlStatusInActive  = PF.RdLong("StatusColours", "ShowInActive",  GrfHelper.iShowMdlStatusError     ? 1:0) ? GrfHelper.iShowMdlStatusError    : MSStyle_Off;
    GrfHelper.iShowMdlStatusError     = PF.RdLong("StatusColours", "ShowError",     GrfHelper.iShowMdlStatusError     ? 1:0) ? GrfHelper.iShowMdlStatusError    : MSStyle_Off;
    GrfHelper.iShowMdlStatusWarning   = PF.RdLong("StatusColours", "ShowWarning",   GrfHelper.iShowMdlStatusWarning   ? 1:0) ? GrfHelper.iShowMdlStatusWarning  : MSStyle_Off;
    GrfHelper.iShowMdlStatusTear      = PF.RdLong("StatusColours", "ShowTear",      GrfHelper.iShowMdlStatusTear      ? 1:0) ? GrfHelper.iShowMdlStatusTear     : MSStyle_Off;
    GrfHelper.iShowMdlStatusMacMdl    = PF.RdLong("StatusColours", "ShowMacMdl",    GrfHelper.iShowMdlStatusMacMdl    ? 1:0) ? GrfHelper.iShowMdlStatusMacMdl   : MSStyle_Off;
    GrfHelper.iShowMdlStatusOff       = PF.RdLong("StatusColours", "ShowOff",       GrfHelper.iShowMdlStatusOff       ? 1:0) ? GrfHelper.iShowMdlStatusOff      : MSStyle_Off;
    GrfHelper.iShowMdlStatusOn        = PF.RdLong("StatusColours", "ShowOn",        GrfHelper.iShowMdlStatusOn        ? 1:0) ? GrfHelper.iShowMdlStatusOn       : MSStyle_Off;
    GrfHelper.iShowMdlStatusLFlwP     = PF.RdLong("StatusColours", "ShowLFlwP",     GrfHelper.iShowMdlStatusLFlwP     ? 1:0) ? GrfHelper.iShowMdlStatusLFlwP    : MSStyle_Off;
    GrfHelper.iShowMdlStatusLFlwN     = PF.RdLong("StatusColours", "ShowLFlwN",     GrfHelper.iShowMdlStatusLFlwN     ? 1:0) ? GrfHelper.iShowMdlStatusLFlwN    : MSStyle_Off;
    GrfHelper.iShowMdlStatusLNoFlw    = PF.RdLong("StatusColours", "ShowLNoFlw",    GrfHelper.iShowMdlStatusLNoFlw    ? 1:0) ? GrfHelper.iShowMdlStatusLNoFlw   : MSStyle_Off;
    GrfHelper.iShowMdlStatusClear     = PF.RdLong("StatusColours", "ShowClear",     GrfHelper.iShowMdlStatusClear     ? 1:0) ? GrfHelper.iShowMdlStatusClear    : MSStyle_Off;
    GrfHelper.iShowMdlStatusUnKnown   = PF.RdLong("StatusColours", "ShowUnKnown",   GrfHelper.iShowMdlStatusUnKnown   ? 1:0) ? GrfHelper.iShowMdlStatusUnKnown  : MSStyle_Off;
    GrfHelper.iShowMdlStatusNettQmP   = PF.RdLong("StatusColours", "ShowNettQmP",   GrfHelper.iShowMdlStatusNettQmP   ? 1:0) ? GrfHelper.iShowMdlStatusNettQmP  : MSStyle_Off;
    GrfHelper.iShowMdlStatusNettQmN   = PF.RdLong("StatusColours", "ShowNettQmN",   GrfHelper.iShowMdlStatusNettQmN   ? 1:0) ? GrfHelper.iShowMdlStatusNettQmN  : MSStyle_Off;
    GrfHelper.iShowMdlStatusUFlw      = PF.RdLong("StatusColours", "ShowUFlw",      GrfHelper.iShowMdlStatusUFlw      ? 1:0) ? GrfHelper.iShowMdlStatusUFlw     : MSStyle_Off;
    GrfHelper.iShowMdlStatusUNoFlw    = PF.RdLong("StatusColours", "ShowUNoFlw",    GrfHelper.iShowMdlStatusUNoFlw    ? 1:0) ? GrfHelper.iShowMdlStatusUNoFlw   : MSStyle_Off;
    GrfHelper.iShowMdlStatusIsSrc     = PF.RdLong("StatusColours", "ShowIsSrc",     GrfHelper.iShowMdlStatusIsSrc     ? 1:0) ? GrfHelper.iShowMdlStatusIsSrc    : MSStyle_Off;
    GrfHelper.iShowMdlStatusIsSnk     = PF.RdLong("StatusColours", "ShowIsSnk",     GrfHelper.iShowMdlStatusIsSnk     ? 1:0) ? GrfHelper.iShowMdlStatusIsSnk    : MSStyle_Off;
    GrfHelper.iShowMdlStatusXferLnk   = PF.RdLong("StatusColours", "ShowXferLnk",   GrfHelper.iShowMdlStatusXferLnk   ? 1:0) ? GrfHelper.iShowMdlStatusXferLnk  : MSStyle_Off;
    GrfHelper.iShowMdlStatusSimpleLnk = PF.RdLong("StatusColours", "ShowSimpleLnk", GrfHelper.iShowMdlStatusSimpleLnk ? 1:0) ? GrfHelper.iShowMdlStatusSimpleLnk: MSStyle_Off;
    GrfHelper.iShowMdlStatusLinearLnk = PF.RdLong("StatusColours", "ShowLinearLnk", GrfHelper.iShowMdlStatusLinearLnk ? 1:0) ? GrfHelper.iShowMdlStatusLinearLnk: MSStyle_Off;
    GrfHelper.iShowMdlStatusFullLnk   = PF.RdLong("StatusColours", "ShowFullLnk",   GrfHelper.iShowMdlStatusFullLnk   ? 1:0) ? GrfHelper.iShowMdlStatusFullLnk  : MSStyle_Off;
    }
  else
    {
    GrfHelper.iShowMdlStatusInActive  = PF.RdLong("StatusColours", "ShowInActive",  GrfHelper.iShowMdlStatusError);
    GrfHelper.iShowMdlStatusError     = PF.RdLong("StatusColours", "ShowError",     GrfHelper.iShowMdlStatusError);
    GrfHelper.iShowMdlStatusWarning   = PF.RdLong("StatusColours", "ShowWarning",   GrfHelper.iShowMdlStatusWarning);
    GrfHelper.iShowMdlStatusTear      = PF.RdLong("StatusColours", "ShowTear",      GrfHelper.iShowMdlStatusTear);
    GrfHelper.iShowMdlStatusMacMdl    = PF.RdLong("StatusColours", "ShowMacMdl",    GrfHelper.iShowMdlStatusMacMdl);
    GrfHelper.iShowMdlStatusOff       = PF.RdLong("StatusColours", "ShowOff",       GrfHelper.iShowMdlStatusOff);
    GrfHelper.iShowMdlStatusOn        = PF.RdLong("StatusColours", "ShowOn",        GrfHelper.iShowMdlStatusOn);
    GrfHelper.iShowMdlStatusLFlwP     = PF.RdLong("StatusColours", "ShowLFlwP",     GrfHelper.iShowMdlStatusLFlwP);
    GrfHelper.iShowMdlStatusLFlwN     = PF.RdLong("StatusColours", "ShowLFlwN",     GrfHelper.iShowMdlStatusLFlwN);
    GrfHelper.iShowMdlStatusLNoFlw    = PF.RdLong("StatusColours", "ShowLNoFlw",    GrfHelper.iShowMdlStatusLNoFlw);
    GrfHelper.iShowMdlStatusClear     = PF.RdLong("StatusColours", "ShowClear",     GrfHelper.iShowMdlStatusClear);
    GrfHelper.iShowMdlStatusUnKnown   = PF.RdLong("StatusColours", "ShowUnKnown",   GrfHelper.iShowMdlStatusUnKnown);
    GrfHelper.iShowMdlStatusNettQmP   = PF.RdLong("StatusColours", "ShowNettQmP",   GrfHelper.iShowMdlStatusNettQmP);
    GrfHelper.iShowMdlStatusNettQmN   = PF.RdLong("StatusColours", "ShowNettQmN",   GrfHelper.iShowMdlStatusNettQmN);
    GrfHelper.iShowMdlStatusUFlw      = PF.RdLong("StatusColours", "ShowUFlw",      GrfHelper.iShowMdlStatusUFlw);
    GrfHelper.iShowMdlStatusUNoFlw    = PF.RdLong("StatusColours", "ShowUNoFlw",    GrfHelper.iShowMdlStatusUNoFlw);
    GrfHelper.iShowMdlStatusIsSrc     = PF.RdLong("StatusColours", "ShowIsSrc",     GrfHelper.iShowMdlStatusIsSrc);
    GrfHelper.iShowMdlStatusIsSnk     = PF.RdLong("StatusColours", "ShowIsSnk",     GrfHelper.iShowMdlStatusIsSnk);
    GrfHelper.iShowMdlStatusXferLnk   = PF.RdLong("StatusColours", "ShowXferLnk",   GrfHelper.iShowMdlStatusXferLnk);
    GrfHelper.iShowMdlStatusSimpleLnk = PF.RdLong("StatusColours", "ShowSimpleLnk", GrfHelper.iShowMdlStatusSimpleLnk);
    GrfHelper.iShowMdlStatusLinearLnk = PF.RdLong("StatusColours", "ShowLinearLnk", GrfHelper.iShowMdlStatusLinearLnk);
    GrfHelper.iShowMdlStatusFullLnk   = PF.RdLong("StatusColours", "ShowFullLnk",   GrfHelper.iShowMdlStatusFullLnk);
    }
  }

//---------------------------------------------------------------------------

flag CProject::Initialise(char * pPrjName)
  {
  Clear();
  if (pPrjName)
    {
    Strng PrjFn(pPrjName);
    PrjFn.FnCheckExtension("spj");
    CDocument* pDoc = ScdApp()->OpenDocumentFile(PrjFn());
    if (pDoc)
      return true;
    else
      LogError("Project", LF_Exclamation, "Unable to open %s", pPrjName);
    }
  MainWnd()->PostMessage(WMU_UPDATEMAINWND, SUB_UPDMAIN_UPDATE, 0); //no project has been opened, bring main window to top
  return false;
  }

//---------------------------------------------------------------------------

int CProject::ShutDown()
  {
  bDocChanged = true; //force project save
  return DoClose(true, true);
  }

//---------------------------------------------------------------------------

//void CProject::SetSolveMode(long Md)
//  {
//  //fProBalMode=PB;
//  gs_Exec.SetSolveMode(Md);//PB ? SM_Direct : SM_DynCurrent);
//  }

//flag CProject::GetProBalMode()
//  {
//  return (gs_Exec.DefNetProbalMode());
//  }

//---------------------------------------------------------------------------

//void CProject::SetDynModeFlow(flag DFlow)
//  {
//  gs_Exec.SetDynMode(DFlow ? DYNFLOWMODE : DYNFULLMODE);
//  }
//
//flag CProject::GetDynModeFlow()
//  {
//  return (gs_Exec.GetSolveMode()==DYNMODE && gs_Exec.GetDynMode()==DYNFLOWMODE);
//  }

////---------------------------------------------------------------------------
//
//flag CProject::CheckForDuplicateClassIds()
//  {
//  flag OK=true;
//
//  for (pTagObjClass p=TagObjClass::FirstClass(); p; p=p->NextClass())
//    {
//    pTagObjClass pOther=p->FindDuplicateClassId(NULL);
//    if (pOther)
//      if (!p->DuplicateExists())
//        {
//        p->SetDuplicateExists(true);
//        pOther->SetDuplicateExists(true);
//        OK=false;
//        LogError("Project", 0,"Duplicate ClassId found %s", p->ClassId());
//        }
//    }
//  return OK;
//  }
//
//---------------------------------------------------------------------------

void CProject::GetSplitInfo(CProfINIFile& PF, CWnd* w, pchar Section, int &SplitCount)
  {
  if (w->IsKindOf(RUNTIME_CLASS(CSplitterWnd)))
    {
    CSplitterWnd* sw = (CSplitterWnd*)w;
    char Sect[256];
    sprintf(Sect, "%s-Split%d", Section, SplitCount++);
    PF.WrInt(Sect, "Rows", sw->GetRowCount());
    PF.WrInt(Sect, "Columns", sw->GetColumnCount());
    char Buff[256];
    if (sw->GetRowCount()>1)
      for (int r=0; r<sw->GetRowCount(); r++)
        {
        int cyCur, cyMin;
        sw->GetRowInfo(r, cyCur, cyMin);
        sprintf(Buff, "Row%d", r);
        PF.WrInt(Sect, Buff, r);
        sprintf(Buff, "CurHeight%d", r);
        PF.WrInt(Sect, Buff, cyCur);
        sprintf(Buff, "MinHeight%d", r);
        PF.WrInt(Sect, Buff, cyMin);
        }

      if (sw->GetColumnCount()>1)
        for (int c=0; c<sw->GetColumnCount(); c++)
          {
          int cxCur, cxMin;
          sw->GetColumnInfo(c, cxCur, cxMin );
          sprintf(Buff, "Column%d", c);
          PF.WrInt(Sect, Buff, c);
          sprintf(Buff, "CurWidth%d", c);
          PF.WrInt(Sect, Buff, cxCur);
          sprintf(Buff, "MinWidth%d", c);
          PF.WrInt(Sect, Buff, cxMin);
          }
    }

  CWnd* w1 = w->GetTopWindow();
  CWnd*w1x = w1;
  while (w1)
    {
    if (!w1->IsKindOf(RUNTIME_CLASS(CFrameWnd)))
      GetSplitInfo(PF, w1, Section, SplitCount);
    w1=w1->GetNextWindow();
    if (w1==w1x)
      w1=NULL;
    }
  }

//---------------------------------------------------------------------------

void CProject::SaveOneWindow(int iNo, LPCTSTR pWindowName, CWnd* pWnd, flag TheMain)//, CProfINIFile & PF)
  {
  CProfINIFile PF;
  if (TheMain)
    {
    PF.SetProfFilename(ScdPFUser.Filename());
    PF.SetUseRegistry(ScdPFUser);
    }
  else
    {
    Strng Fn(PrjFile());
    if (PrjFileVerNo() > 3)
      {
      Fn = PrjFiles();
      Fn += LayoutFileName;
      }
    PF.SetProfFilename(Fn());
    }

  ASSERT(pWnd->IsKindOf(RUNTIME_CLASS(CFrameWnd)) ||
    pWnd->IsKindOf(RUNTIME_CLASS(CSplitterWnd)) ||
    pWnd->IsKindOf(RUNTIME_CLASS(CDialog)));
  WINDOWPLACEMENT wp;
  wp.length = sizeof(wp);
  if (pWnd->GetWindowPlacement(&wp))
    {
    char Section[256], ReducedName[1024], *pChar;
    sprintf(Section, "Window%d", iNo);
    //    strcpy(ReducedName, (char*)(const char*)(WL.Wnds[i].sName));
    strcpy(ReducedName, pWindowName);
    if ((pChar=strchr(ReducedName, ':'))!=NULL)
      *pChar=0;
    PF.WrStr(Section, "Name", ReducedName);
    PF.WrInt(Section, "Flags", wp.flags);
    PF.WrInt(Section, "Show", wp.showCmd);
    PF.WrInt(Section, "MinX", wp.ptMinPosition.x);
    PF.WrInt(Section, "MinY", wp.ptMinPosition.y);
    PF.WrInt(Section, "MaxX", wp.ptMaxPosition.x);
    PF.WrInt(Section, "MaxY", wp.ptMaxPosition.y);
    PF.WrInt(Section, "Left", wp.rcNormalPosition.left);
    PF.WrInt(Section, "Top", wp.rcNormalPosition.top);
    PF.WrInt(Section, "Right", wp.rcNormalPosition.right);
    PF.WrInt(Section, "Bottom", wp.rcNormalPosition.bottom);
    int SplitCount = 0;
    GetSplitInfo(PF, pWnd, Section, SplitCount);
    PF.WrInt(Section, "SplitWindowCount", SplitCount);
    }
  }

//---------------------------------------------------------------------------

void CProject::SaveWindowLayout()
  {
  CWaitMsgCursor Wait("Saving layout of windows and toolbars");
  CProfINIFile PF(PrjFiles(), LayoutFileName);
  DeleteFile(PF.sFilename()); //delete old layout file

  CWindowLists WL;
  const int WndCount = WL.BuildSingleList();

  // Main Wnd is always first in WL;
  ScdPFUser.WrInt("General", "WindowCount", 1);
  SaveOneWindow(0, WL.Wnds[0].m_sName, WL.Wnds[0].pWnd, true);//ScdPFUser);

  PF.WrInt("General", "WindowCount", 0);
  for (int i=1; i<WndCount; i++)
    SaveOneWindow(i-1, WL.Wnds[i].m_sName, WL.Wnds[i].pWnd, false);//PF);

  PF.WrInt("General", "WindowCount", WndCount-1);
  PF.WrInt("General", "MsgWindowSplit", CMsgWindow::GetSplitPos());

  //save Z-Order...
  WL.BuildZOrder();
  int ZCount = 0;
  for (i=0; i<WndCount; i++)
    if (WL.Wnds[i].iZOrd>=0)
      {
      char Entry[64];
      sprintf(Entry, "Window_%d", WL.Wnds[i].iZOrd);
      PF.WrStr("Z_Order", Entry, (char*)(const char*)(WL.Wnds[i].m_sName));
      ZCount++;
      }
    PF.WrInt("General", "ZOrderCount", ZCount);

    //save toolbar states and positions...
    if (pGrfTB)
      {
      pGrfTBMngr->RemoveToolbar((char*)pGrfTB->GetTitle());
      pGrfTB = NULL;
      }
    pGrfTBMngr->RemoveAllToolbars();
    /*const char * OldIni=ScdApp()->m_pszProfileName;
    ScdApp()->m_pszProfileName = PrjFile();
    CMainFrame* pM = (CMainFrame*)AfxGetMainWnd();
    pM->SaveBarState("ToolBars");
    ScdApp()->m_pszProfileName = OldIni;*/

    pMainTBMngr->SaveState(PF.sFilename());
  }

//---------------------------------------------------------------------------

void CProject::PutSplitInfo(CProfINIFile& PF, CWnd* w, pchar Section, int& SplitCount)
  {
  if (w->IsKindOf(RUNTIME_CLASS(CSplitterWnd)))
    {
    CSplitterWnd*sw=(CSplitterWnd*)w;
    char Sect[256];
    sprintf(Sect, "%s-Split%d", Section, SplitCount++);
    //Rows = PF.RdInt(Sect, "Rows", sw->GetRowCount());
    //Cols = PF.RdInt(Sect, "Columns", sw->GetColumnCount());
    char Buff[256];
    if (sw->GetRowCount()>1)
      for (int r=0; r<sw->GetRowCount(); r++)
        {
        sprintf(Buff, "Row%d", r);
        int r1 = PF.RdInt(Sect, Buff, 0);
        sprintf(Buff, "CurHeight%d", r);
        int cyCur = PF.RdInt(Sect, Buff, 0);
        sprintf(Buff, "MinHeight%d", r);
        int cyMin = PF.RdInt(Sect, Buff, 0);
        sw->SetRowInfo(r1, cyCur, cyMin);
        }

      if (sw->GetColumnCount()>1)
        for (int c=0; c<sw->GetColumnCount(); c++)
          {
          sprintf(Buff, "Column%d", c);
          int c1 = PF.RdInt(Sect, Buff, 0);
          sprintf(Buff, "CurWidth%d", c);
          int cxCur = PF.RdInt(Sect, Buff, 0);
          sprintf(Buff, "MinWidth%d", c);
          int cxMin = PF.RdInt(Sect, Buff, 0);
          sw->SetColumnInfo(c1, cxCur, cxMin);
          }
        sw->RecalcLayout();
    }

  CWnd*w1=w->GetTopWindow();
  CWnd*w1x=w1;
  while (w1)
    {
    if (!w1->IsKindOf(RUNTIME_CLASS(CFrameWnd)))
      PutSplitInfo(PF, w1, Section, SplitCount);
    w1=w1->GetNextWindow();
    if (w1==w1x)
      w1=NULL;
    }
  }

//---------------------------------------------------------------------------

flag CProject::PutFrameInfo(CProfINIFile& PF, CWnd* w, pchar Section, Strng& s, CWnd** WndList, int& WndListLen)
  {
  if (w->IsKindOf(RUNTIME_CLASS(CFrameWnd)))
    {
    CString Txt;
    w->GetWindowText(Txt);

    int pos=Txt.Find(" - ");
    if (pos>=0)
      {
      while ((Txt[pos-1]==' ') && (pos>0)) pos--;
      Txt=Txt.Left(pos);
      }
    //Get Working Area
    RECT CR;
    AfxGetMainWnd()->GetTopWindow()->GetClientRect(&CR);
    int MaxX=CR.right-(2*GetSystemMetrics(SM_CXSIZE)+GetSystemMetrics(SM_CXSIZEFRAME));
    int MaxY=CR.bottom-(GetSystemMetrics(SM_CYSIZE)+GetSystemMetrics(SM_CYSIZEFRAME));

    if (_stricmp((const char*)Txt, s())==0 && WndListLen<128)
      {
      WndList[WndListLen++] = w;
      //TRACE("--%s--\n",pName);
      WINDOWPLACEMENT wp;
      w->GetWindowPlacement(&wp);
      wp.flags=PF.RdInt(Section, "Flags", wp.flags);
      wp.showCmd=PF.RdInt(Section, "Show", wp.showCmd);
      wp.ptMinPosition.x=Min(MaxX, PF.RdInt(Section, "MinX", wp.ptMinPosition.x));
      wp.ptMinPosition.y=Min(MaxY, PF.RdInt(Section, "MinY", wp.ptMinPosition.y));
      wp.ptMaxPosition.x=Min(MaxX, PF.RdInt(Section, "MaxX", wp.ptMaxPosition.x));
      wp.ptMaxPosition.y=Min(MaxY, PF.RdInt(Section, "MaxY", wp.ptMaxPosition.y));
      wp.rcNormalPosition.left=PF.RdInt(Section, "Left", wp.rcNormalPosition.left);
      wp.rcNormalPosition.top=PF.RdInt(Section, "Top", wp.rcNormalPosition.top);
      wp.rcNormalPosition.right=PF.RdInt(Section, "Right", wp.rcNormalPosition.right);
      wp.rcNormalPosition.bottom=PF.RdInt(Section, "Bottom", wp.rcNormalPosition.bottom);
      if (wp.rcNormalPosition.left>=MaxX)
        {
        wp.rcNormalPosition.right-=wp.rcNormalPosition.left-MaxX;
        wp.rcNormalPosition.left-=wp.rcNormalPosition.left-MaxX;
        }
      if (wp.rcNormalPosition.top>=MaxY)
        {
        wp.rcNormalPosition.bottom-=wp.rcNormalPosition.top-MaxY;
        wp.rcNormalPosition.top-=wp.rcNormalPosition.top-MaxY;
        }
      wp.length=sizeof(wp);
      for (int i=0; i<NAccessWnds; i++)
        if (w==gs_AccessWnds.Frame(i))
          wp.showCmd=SW_HIDE;
      w->SetWindowPlacement(&wp);

      //int SplitCount = ScdPFUser.RdInt(Section, "SplitWindowCount", 0);
      int SplitCount = 0;
      PutSplitInfo(PF, w, Section, SplitCount);
      ASSERT(PF.RdInt(Section, "SplitWindowCount", -1) == SplitCount);

      //w->RedrawWindow();

      AfxGetMainWnd()->UpdateWindow();
      return true;
      }
    }
  CWnd*w1=w->GetTopWindow();
  CWnd*w1x=w1;
  while (w1)
    {
    if (PutFrameInfo(PF, w1, Section, s, WndList, WndListLen))
      return true;
    w1=w1->GetNextWindow();
    if (w1==w1x)
      w1=NULL;
    }
  return false;
  }

//---------------------------------------------------------------------------

void CProject::RestoreMainWindow()
  {
  RestoreOneWindow(CWindowLists::MainWndTitle, AfxGetMainWnd(), true);

  CProfINIFile PF(PrjFile());
  if (PrjFileVerNo() > 3)
    {
    PF.sFilename = PrjFiles();
    PF.sFilename += LayoutFileName;
    }

  //restore toolbars...
  CMainFrame* pM = (CMainFrame*)AfxGetMainWnd();
  //if (PrjFileVerNo()<34)
  if (PrjFileVerNo()<100) //CNM
    {
    pM->DefaultToolBar(-1, TRUE);
    }
  else
    {
    Strng s = PrjFiles();
    s += LayoutFileName;
    pMainTBMngr->LoadState(s());
    pMainTBMngr->LoadState(s()); //call twice to ensure positions are correct
    }
  pM->UpdateToolBars();

  //restore position of special windows...
  for (int i=0; i<NAccessWnds; i++)
    RestoreOneWindow(CWindowLists::AccessWndTitle(i), gs_AccessWnds.Frame(i), false);
  RestoreOneWindow("Command", gs_pCmdFrame, false);
  RestoreOneWindow("Messages", CMsgWindow::GetFrame(), false);

  //CNM AfxGetMainWnd()->UpdateWindow();
  AfxGetMainWnd()->RedrawWindow();
  }

//---------------------------------------------------------------------------

void CProject::RestoreOneWindow(LPCTSTR pWindowName, CWnd* pWnd, flag TheMain, flag FullRestore)
  {

  CProfINIFile PF;
  if (TheMain)
    {
    PF.SetProfFilename(ScdPFUser.Filename());
    PF.SetUseRegistry(ScdPFUser);
    }
  else
    {
    Strng Fn(PrjFile());
    if (PrjFileVerNo() > 3)
      {
      Fn = PrjFiles();
      Fn += LayoutFileName;
      }
    PF.SetUseRegistry(false);
    PF.SetProfFilename(Fn());
    }

  int WndCount = PF.RdInt("General", "WindowCount", 0);
  for (int i=0; i<WndCount; i++)
    {
    char Section[256];
    sprintf(Section, "Window%i", i);
    Strng s = PF.RdStr(Section, "Name", "");
    if (s.Length()>0 && _stricmp(pWindowName, s())==0)
      {
      CWnd* w = pWnd;
      if (!(w->IsKindOf(RUNTIME_CLASS(CFrameWnd))))// || w->IsKindOf(RUNTIME_CLASS(CDialog))))
        w = w->GetParent();
      //if (w && !w->IsKindOf(RUNTIME_CLASS(CFrameWnd)))
      //  w = w->GetParent();
      if (w && (w->IsKindOf(RUNTIME_CLASS(CFrameWnd))))// || w->IsKindOf(RUNTIME_CLASS(CDialog))))
        {//found window, restore it's position...
        flag IsFrame=w->IsKindOf(RUNTIME_CLASS(CFrameWnd));
        //CString Txt;
        //w->GetWindowText(Txt);
        WINDOWPLACEMENT wp;
        w->GetWindowPlacement(&wp);
        wp.flags = PF.RdInt(Section, "Flags", wp.flags);
        wp.flags &= ~WPF_RESTORETOMAXIMIZED;
        wp.showCmd = PF.RdInt(Section, "Show", wp.showCmd);
        wp.ptMinPosition.x = PF.RdInt(Section, "MinX", wp.ptMinPosition.x);
        wp.ptMinPosition.y = PF.RdInt(Section, "MinY", wp.ptMinPosition.y);
        wp.ptMaxPosition.x = PF.RdInt(Section, "MaxX", wp.ptMaxPosition.x);
        wp.ptMaxPosition.y = PF.RdInt(Section, "MaxY", wp.ptMaxPosition.y);
        wp.rcNormalPosition.left = PF.RdInt(Section, "Left", wp.rcNormalPosition.left);
        wp.rcNormalPosition.top = PF.RdInt(Section, "Top", wp.rcNormalPosition.top);
        wp.rcNormalPosition.right = PF.RdInt(Section, "Right", wp.rcNormalPosition.right);
        wp.rcNormalPosition.bottom = PF.RdInt(Section, "Bottom", wp.rcNormalPosition.bottom);
        if (pWnd!=AfxGetMainWnd())
          {//ensure window is visible in main window...
          RECT CR;
          /*AfxGetMainWnd()->GetClientRect(&CR);
          //int MaxX = CR.right-(2*GetSystemMetrics(SM_CXSIZE)+GetSystemMetrics(SM_CXSIZEFRAME));
          //int MaxY = CR.bottom-(GetSystemMetrics(SM_CYSIZE)+GetSystemMetrics(SM_CYSIZEFRAME));
          int MaxX = CR.right - (2*GetSystemMetrics(SM_CXSIZEFRAME)) - 1;
          int MaxY = CR.bottom - (2*GetSystemMetrics(SM_CYSIZEFRAME)) - 1;*/
          w->GetParent()->GetClientRect(&CR); //get area of MDI framework client window
          const int MaxX = CR.right - 1;
          const int MaxY = CR.bottom - 1;
          if (wp.rcNormalPosition.left>=MaxX)
            {
            const int xx = wp.rcNormalPosition.left - MaxX + GetSystemMetrics(SM_CXSIZE) + 10;
            wp.rcNormalPosition.right -= xx;
            wp.rcNormalPosition.left -= xx;
            }
          if (wp.rcNormalPosition.top>=MaxY)
            {
            const int yy = wp.rcNormalPosition.top - MaxY + GetSystemMetrics(SM_CYSIZE) + 5;
            wp.rcNormalPosition.bottom -= yy;
            wp.rcNormalPosition.top -= yy;
            }
          if (wp.ptMinPosition.x>=MaxX)
            wp.ptMinPosition.x = MaxX - GetSystemMetrics(SM_CXSIZE) - 8;
          if (wp.ptMinPosition.y>=MaxY)
            wp.ptMinPosition.y = MaxY - GetSystemMetrics(SM_CYSIZE) - 3;
          }
        wp.length = sizeof(wp);
        w->SetWindowPlacement(&wp);
        int SplitCount = 0;
        PutSplitInfo(PF, w, Section, SplitCount);
        ASSERT(PF.RdInt(Section, "SplitWindowCount", -1) == SplitCount);
        if (FullRestore)
          {
          for (int i=0; i<NAccessWnds; i++)
            if (w==gs_AccessWnds.Frame(i))
              w->ShowWindow(SW_HIDE); //default - hide access window
          w->BringWindowToTop();
          AfxGetMainWnd()->UpdateWindow();
          }
        }
      if (pWnd==CMsgWindow::GetFrame())
        CMsgWindow::SetSplitPos(PF.RdInt("General", "MsgWindowSplit", 95));
      return; //found window
      }
    }
  }

//---------------------------------------------------------------------------

void CProject::RestoreWindowZOrder()
  {//restore window Z-Order...
  CProfINIFile PF(PrjFiles(), LayoutFileName);
  int ZCount = PF.RdInt("General", "ZOrderCount", 0);
  if (ZCount>0)
    {
    CWindowLists WL;
    WL.BuildSingleList();
    CWnd* pPrvWnd = (CWnd*)&CWnd::wndBottom;
    for (int i=ZCount-1; i>=0; i--)
      {
      char Entry[64];
      sprintf(Entry, "Window_%d", i);
      Strng Name = PF.RdStr("Z_Order", Entry, "");
      if (Name.Len()>0)
        {
        int index = WL.Find(Name());
        if (index>=0)
          {
          WL.Wnds[index].pWnd->SetWindowPos(pPrvWnd, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE /*| SWP_NOACTIVATE*/ | SWP_NOSENDCHANGING);
          pPrvWnd = WL.Wnds[index].pWnd;
          }
        }
      }
    }
  }

//---------------------------------------------------------------------------

flag CProject::DoLoadDB(char* pPrjFiles, char* pOldPrjName)
  {
  bool Failed=true;
  gs_AccessWnds.CloseAccessData(-1, false, false, true);
  gs_AccessWnds.CloseWnd(-1);

  CLoadDBInfo LDBI(DBConnect_Find, DBO_ForRead, pPrjFiles);
  
  if (!gs_Exec.LoadDBase(eLdDB_Open, LDBI)) 
    goto Failed;
  if (!gs_Exec.LoadDBase(eLdDB_Create, LDBI))
    goto Failed;
  
  if (gs_License.IllegalNodeModelCount(eLic_MsgBox)>0)
    {
    LDBI.m_Failed=true;
    goto Failed;
    }

//CNM #if !BYPASSLICENSING //pkh
  if (gs_License.NodeCountExceeded(0, eLic_MsgBox))
    {
    LDBI.m_Failed=true;
    goto Failed;
    }
//CNM #endif //pkh

  if (!gs_Exec.LoadDBase(eLdDB_Connect, LDBI))
    goto Failed;
  if (!gs_Exec.LoadDBase(eLdDB_Load, LDBI))
    goto Failed;

#if WITHDRVMAN
  OpenDrvManager(); //must be loaded after models (tag data base) BUT before 'Other.Dat'
#endif
  ConnectArcManager(); //must be loaded after models (tag data base) BUT before 'Other.Dat'
  OpenIOMarshal(); //???

  if (!gs_Exec.LoadDBase(eLdDB_LoadOther, LDBI))
    goto Failed;

  Failed=false;

Failed:
  gs_Exec.LoadDBase(eLdDB_Close, LDBI); //Close
  XSetDBFormat(LDBI.m_DBFmt);


//#if WITHDRVMAN
//  OpenDrvManager(); //must be loaded after models (tag data base) BUT before 'Other.Dat'
//#endif
//  OpenArcManager(); //must be loaded after models (tag data base) BUT before 'Other.Dat'
//  OpenIOMarshal(); //???
//  gs_Exec.LoadDBase(2, DBConnect_Find, DBO_ForRead, pPrjFiles);
//  //if (Fmt & DBO_Fmts)
//  //  {
//  //m_DBFormat=Fmt & DBO_Fmts;
//  XSetDBFormat(Fmt);
//  }

  return !Failed;
  }

//---------------------------------------------------------------------------

flag CProject::DoSaveDB(int SaveDBFmt, int SaveOpts, char* pPrjFiles, char* pOldPrjFiles)
  {
  gs_AccessWnds.CloseAccessData(-1, true, false, false);
  gs_AccessWnds.CloseWnd(-1);

  int Fmt=gs_Exec.SaveDBase(SaveDBFmt, SaveOpts|DBO_ForWrite|DBO_CompressDB, pPrjFiles, pOldPrjFiles);
  //if (Fmt & DBO_Fmts)
  //  {
  //  //m_DBFormat=Fmt & DBO_Fmts;
  XSetDBFormat(Fmt);
  //  };
  return true;
  }

//---------------------------------------------------------------------------

int CProject::FixSnapshot(pchar FixFn)
  {
  //  return 0;

  int N=0;
  if (!FileExists(FixFn))
    LogNote("SnapFix", 0, "%s not found", FixFn);

  FILE *file=fopen(FixFn, "rt");
  if (!file)
    {
    LogError("SnapFix", 0, "%s not opened", FixFn);
    return 0;
    }

  while (!feof(file))
    {
    char Buff[4096];
    CSVColArray C;
    fgets(Buff, sizeof(Buff), file);
    int nToks=ParseTokenList(Buff, C, " \t\n");
    if (nToks>=3)
      {
      if (_stricmp(C[0], "end")==0)
        goto Done;
      else if (_stricmp(C[0], "set")==0)
        {
        if (_stricmp(C[2], "=")==0)
          C[2]=C[3];

        Strng WrkTag, WrkCnvTxt;
        TaggedObject::SplitTagCnv(C[1], WrkTag, WrkCnvTxt);
        flag UseCnv = (WrkCnvTxt.Length()>0);
        CXM_ObjectTag  ObjTag(WrkTag(), (UseCnv ? TABOpt_ValCnvsOnce : 0));
        CXM_ObjectData ObjData;
        CXM_Route      Route;
        if (gs_Exec.XReadTaggedItem(NULL, ObjTag, ObjData, Route)!=0)
          {
          CPkDataItem * pPItem=ObjData.FirstItem();
          if ((pPItem->Flags() & isParm)!=0)
            {
            PkDataUnion DU;
            if (UseCnv)
              DU.SetTypeString(pPItem->Type(), C[2], pPItem->CnvIndex(), WrkCnvTxt());//, Cnv.Index(), Cnv.Text());
            else
              DU.SetTypeString(tt_Generic, C[2]);
            //if (UseCnv)
            //  DU.SetTypeString(pPItem->Type(), C[2], pPItem->CnvIndex(), WrkCnvTxt());//, Cnv.Index(), Cnv.Text());
            //else
            //  DU.SetTypeString(pPItem->Type(), C[2]);
            CXM_ObjectData ObjData(0, 0, WrkTag(), 0, DU);
            dbgpln("Snapfix Set %s : %s", WrkTag(), C[2]);
            //Ok = (pDoc->XWriteTaggedItem(ObjData, Route)==TOData_OK);
            if (gs_Exec.XWriteTaggedItem(NULL, ObjData, Route)!=TOData_NotFound)
              {
              N++;
              LogNote("SnapFix", 0, "Set %s = %s", C[1], C[2]);
              gs_Exec.m_Seq.NoteManSet(CExecSequence::VSS_SnapFix, WrkTag(), C[2], UseCnv ? WrkCnvTxt() : "");
              }
            else
              LogWarning("SnapFix", 0, "%s NOT set to %s", C[1], C[2]);
            }
          else
            LogWarning("SnapFix", 0, "%s is not a parameter", C[1]);
          }
        else
          LogWarning("SnapFix", 0, "%s not found", C[1]);
        }
      }
    }
Done:
  return N;
  }
//---------------------------------------------------------------------------

flag CProject::DoLoadSnapShotScenario(flag DoSnapShot, pchar pName)
  {
  CWaitMsgCursor Wait(DoSnapShot ? "Loading SnapShot" : "Loading Scenario");
  Strng Fn;//, Ext;
  Fn = pName;
  if (DoSnapShot && Fn.Length()==0)
    Fn = m_sSSName;
  if (Fn.Length()==0)
    return false;
#if WITHZIP
  Strng Ext;
  Ext.FnExt(Fn());
  bool IsZip = (Ext.Len() && _stricmp(Ext(), ".zip")==0);
#else
  bool IsZip = false;
#endif
  if (!IsZip)
    Fn.FnCheckExtension(DoSnapShot ? "snp" : "scn");
  Fn.FnSearchExpand();
#if WITHZIP
  if (IsZip)
    {
    int ret = CZipFile::UnZipOne(Fn());
    if (ret!=0)
      LogError("UnZipFile", LF_Exclamation, "Unable to unzip file '%s'", Fn());
    Fn = Fn.FnDrivePathName();
    }
#endif
  pName = Fn();

  gs_AccessWnds.CloseAccessData(-1, false, false, true);
  gs_AccessWnds.CloseWnd(-1);

  int n = (DoSnapShot ? gs_Exec.LoadSnapShot(PrjFiles(), pName) : gs_Exec.LoadScenario(PrjFiles(), pName));
  gs_pCmd->Print("%i Objects Loaded for %s\n", n, DoSnapShot ? "SnapShot" : "Scenario");

  for (int i=0; i<2; i++)
    {
    Strng Where=(i==0 ? PrjFiles() : CfgFiles());
    Strng Fx;
    Fx.FnDrivePath(pName);
    if (Fx.Length()==0)
      Fx.Set("%s%s", PrjFiles(), pName);
    Fx+="SnapShot.Fix.txt";
    if (FileExists(Fx()))
      FixSnapshot(Fx());

    Fx.FnDrivePath(pName);
    if (Fx.Length()==0)
      Fx.Set("%s%s", PrjFiles(), pName);
    else
      Fx=pName;
    Fx.FnDrivePathName();
    Fx+=".Fix.txt";
    if (FileExists(Fx()))
      FixSnapshot(Fx());
    }

#if WITHZIP
  if (IsZip)
    DeleteFile(Fn());
#endif
  return true;
  }

//---------------------------------------------------------------------------

flag CProject::DoSaveSnapShotScenario(flag DoSnapShot, pchar pName, CExecObjArray *ObjList)
  {
  CWaitMsgCursor Wait(DoSnapShot ? "Saving SnapShot" : "Saving Scenario");
  Strng Fn, Ext;
  Fn = pName;
  if (DoSnapShot && Fn.Length()==0)
    Fn = m_sSSName;
  if (Fn.Length()==0)
    return false;
  Ext.FnExt(Fn());
  if (Ext.Len()==0 || _stricmp(Ext(), (DoSnapShot ? ".snp" : ".scn"))!=0)
    {//force correct extension
    Fn = Fn.Left(Fn.Len()-Ext.Len());
    Fn += (DoSnapShot ? ".snp" : ".scn");
    }
  pName = Fn();

  gs_AccessWnds.CloseAccessData(-1, true, false, false);
  gs_AccessWnds.CloseWnd(-1);

  int n = (DoSnapShot ? gs_Exec.SaveSnapShot(PrjFiles(), pName, ObjList) : gs_Exec.SaveScenario(PrjFiles(), pName, ObjList));
#if WITHZIP
  if (m_bSSDoZip)
    {
    int ret = CZipFile::ZipOne(pName, true);
    if (ret!=0)
      LogError("ZipFile", LF_Exclamation, "Unable to create zip file from '%s'", pName);
    }
#endif
  if (DoSnapShot)
    {
    CProfINIFile PF(PrjIniFile());
    PF.WrLong("SnapShot", "Action", m_eSSAction);
    PF.WrStr("SnapShot", "Name", m_sSSName());
    PF.WrStr("SnapShot", "Prefix", m_sSSPrefix());
    PF.WrLong("SnapShot", "Number", m_iSSNumber);
    }
  gs_pCmd->Print("%i Objects Saved for %s\n", n, DoSnapShot ? "SnapShot" : "Scenario");
  return true;
  }

//---------------------------------------------------------------------------

void CProject::OnFileSavesnapshot()
  {
  flag SaveSS = true;
  Strng s;
  switch (m_eSSAction)
    {
    case SSA_Prompt:
      {
      CSCDFileDialog Dlg(false, NULL, "*.snp",
        OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_SHAREAWARE,
        "SnapShots (*.snp)|*.snp||");
      Dlg.m_ofn.lpstrInitialDir = PrjFiles();
      Dlg.m_ofn.lpstrTitle = "Save SnapShot As";

      if (Dlg.DoModal()==IDOK)
        {
        s=Dlg.GetPathName();
#if WITHZIP
        bool Exists;
        if (m_bSSDoZip)
          {
          Strng s1(s());
          s1 += ".zip";
          Exists = ((FileExists(s()) || FileExists(s1())) && (m_LastSnapLoadFn.XStrICmp(s)!=0));
          }
        else
          Exists = (FileExists(s()) && (m_LastSnapLoadFn.XStrICmp(s)!=0));
#else
        bool Exists = (FileExists(s()) && (m_LastSnapLoadFn.XStrICmp(s)!=0));
#endif
        if (Exists)
          {
          Strng Msg;
          Msg.Set("Snapshot %s\n\nAlready exists. Overwrite ?", s());
          SaveSS = (AfxMessageBox(Msg(), MB_YESNO|MB_ICONQUESTION)==IDYES);
          }
        }
      else
        SaveSS = false;

      break;
      }
    case SSA_AutoInc:
      {
      s.Set("%s%03d", m_sSSPrefix(), m_iSSNumber);
      m_iSSNumber++;
      break;
      }
    case SSA_ReUse:
      s = m_sSSName;
      break;
    }
  if (SaveSS)
    {
    m_LastSnapLoadFn=s();
    DoSaveSnapShotScenario(true, s());
    }
  }

//---------------------------------------------------------------------------

void CProject::OnFileLoadsnapshot()
  {
  Strng s;
#if WITHZIP
  CSCDFileDialog Dlg(true, NULL, "*.snp;*.snp.zip", OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_SHAREAWARE, "SnapShots (*.snp)|*.snp;*.snp.zip|All Files (*.*)|*.*||");
#else
  CSCDFileDialog Dlg(true, NULL, "*.snp", OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_SHAREAWARE, "SnapShots (*.snp)|*.snp|All Files (*.*)|*.*||");
#endif
  Dlg.m_ofn.lpstrInitialDir = PrjFiles();
  Dlg.m_ofn.lpstrTitle = "Open SnapShot";

  //    Dlg.m_ofn.lpfnHook = (LPOFNHOOKPROC)OFNHookProcSPJ;
  //    Dlg.m_ofn.Flags |= OFN_EXPLORER|OFN_ENABLEHOOK ;

  if (Dlg.DoModal()==IDOK)
    {
    m_LastSnapLoadFn=(char*)(const char*)Dlg.GetPathName();
    DoLoadSnapShotScenario(true, (char*)(const char*)Dlg.GetPathName());
    }
  }

//---------------------------------------------------------------------------

void CProject::OnFileSavescenario()
  {
  CSCDFileDialog Dlg(false, NULL, "*.scn", /*OFN_OVERWRITEPROMPT | */OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_SHAREAWARE, "Scenarios (*.scn)|*.scn||)");
  Dlg.m_ofn.lpstrInitialDir = PrjFiles();
  Dlg.m_ofn.lpstrTitle = "Save Scenario As";
  if (Dlg.DoModal()==IDOK)
    {
    Strng s=(char*)(const char*)Dlg.GetPathName();
    s.FnCheckExtension("scn");
    if (FileExists(s()) && (m_LastSnapLoadFn.XStrICmp(s)!=0))
      {
      Strng Msg;
      Msg.Set("Snapshot %s\n\nAlready exists. Overwrite ?", s());
      if (AfxMessageBox(Msg(), MB_YESNO|MB_ICONQUESTION)!=IDYES)
        return;
      }
    m_LastSnapLoadFn=s();
    DoSaveSnapShotScenario(false, s());
    }
  }

//---------------------------------------------------------------------------

void CProject::OnFileLoadscenario()
  {
  Strng s;
#if WITHZIP
  CSCDFileDialog Dlg(true, NULL, "*.scn;*.scn.zip", OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_SHAREAWARE, "Scenarios (*.scn)|*.scn;*.scn.zip|All Files (*.*)|*.*||");
#else
  CSCDFileDialog Dlg(true, NULL, "*.scn", OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_SHAREAWARE, "Scenarios (*.scn) | *.scn |All Files (*.*)|*.*||");
#endif
  Dlg.m_ofn.lpstrInitialDir = PrjFiles();
  Dlg.m_ofn.lpstrTitle = "Open Scenario";
  if (Dlg.DoModal()==IDOK)
    {
    m_LastSnapLoadFn=(char*)(const char*)Dlg.GetPathName();
    DoLoadSnapShotScenario(false, (char*)(const char*)Dlg.GetPathName());
    }
  }

//---------------------------------------------------------------------------

flag CProject::OnChangeCurrentSettings(LPCTSTR Title, int WhichPage)
  {
  if (gs_Exec.TestRunning())
    return false;

  if (!m_bConfigBusy) // will be cleared by closing dialog
    {
    if (MainWnd()->m_pPrjSheet)
      MainWnd()->m_pPrjSheet->BringWindowToTop();
    else
      {
      CProfINIFile PF(PrjIniFile());
      MainWnd()->m_pPrjSheet = new CProjectSettingsDlg(false, this, this, Title, MainWnd(), 
        WhichPage>=0 ? WhichPage : PF.RdInt("General", "PrjOptionsPageNo", 0));
      }
    }

  return true;
  }

//---------------------------------------------------------------------------

flag CProject::OnLoadAltCfg()
  {
  if (gs_Exec.TestRunning())
    return false;
  return true;
  }

//---------------------------------------------------------------------------

void CProject::OpenHistorian()
  {
  dbgpln("--------------------- CProject::OpenHistorian");
  bHstOK=0;
  gs_HstMngr.SetHstOpen(0);//make sure is correct
  gs_HstMngr.SetExecObj(this);//make sure is correct
  gs_HstMngr.SetLicensingOptions(gs_License.MaxHistSizeAllowed(), gs_License.MaxHistFilesAllowed());
  if (m_Hst.m_OnRqd) // gs_HstMngr.HstOn())
    {
    CWaitMsgCursor Wait("Opening historian");
    if (gs_HstMngr.DoOpenHistorian(true))
      bHstOK=1;
    }
  }

//---------------------------------------------------------------------------

void CProject::DeleteHistorian()
  {
  dbgpln("--------------------- CProject::DeleteHistorian");
  if (gs_HstMngr.DoDeleteHistorian())
    CTagVwDoc::RebuildAll(); //force all trends to reconnect etc
  }

//---------------------------------------------------------------------------

void CProject::RestartHistorian()
  {
  dbgpln("--------------------- CProject::RestartHistorian");
  gs_HstMngr.DoRestart();
  }

//---------------------------------------------------------------------------

void CProject::CloseHistorian(BOOL ForExit/*=FALSE*/)
  {
  dbgpln("--------------------- CProject::CloseHistorian");
  if (gs_HstMngr.HstOpen())
    {
    if (ForExit)
      gs_HstMngr.DoClose();
    else
      {
      CWaitMsgCursor Wait("Closing historian");
      CDlgBusy::Open("\n\nClosing Historian");
      gs_HstMngr.DoClose();
      CDlgBusy::Close();
      }
    }
  gs_HstMngr.SetExecObj(NULL);
  }

//---------------------------------------------------------------------------

flag CProject::IsHistorianOpen()
  {
  return gs_HstMngr.IsOpen();
  }

//---------------------------------------------------------------------------

#if WITHDRVMAN
void CProject::OpenDrvManager()
  {
  bDrvOpen=0;
  if (m_bDrvOn && m_sDrvManagerName.Length()>0)
    {
    if (gs_License.AllowDrivers())
      {
      CWaitMsgCursor Wait("Opening driver manager");
      bDrvOpen=1;
      Strng Name, DrvDir, FileExt;
      Name.FnSearchExpand(m_sDrvManagerName(), AF_All|AF_BackupFiles);
      DrvDir.FnDrivePath(Name());
      FileExt.FnNameExt(Name());

      gs_pDrvMan->Configure(DrvDir(), FileExt(), (byte)m_bDrvIgnoresOn, (byte)bDrvForcesOn, (byte)bDrvReadAll, (byte)bDrvNoiseOn, bDrvStartFBKCheck);
      if (gs_pDrvMan->Open()!=0)
        {
        LogError("Driver", LF_Exclamation, "Error Opening Driver Manager");
        bDrvOpen=0;
        }
      }
    else
      {
      LogError("Driver", 0, "Drivers not enable by current licensing options");
      }
    }

  MainWnd()->UpdateStatusBar();
  }

//---------------------------------------------------------------------------

void CProject::CloseDrvManager(BOOL ForExit/*=FALSE*/, BOOL ForProject/*=FALSE*/)
  {
  if (bDrvOpen)
    {
    CWaitMsgCursor Wait("Closing driver manager");
    gs_pDrvMan->Close(!(ForExit || ForProject));
    bDrvOpen=0;
    }
  }

//---------------------------------------------------------------------------

flag CProject::ReloadDrvManager(BOOL WithOptions, char * NewDrvManagerName)
  {
  if (bDrvOn)
    {
    CWaitCursor Wait;
    if (WithOptions)
      gs_pDrvMan->CloseOptions();

    CDlgBusy::Open("\nReloading Drivers");

    char TmpSnap[MAX_PATH];
    strcpy(TmpSnap, TemporaryFiles());
    strcat(TmpSnap, "DrvReload.snp");

    CExecObjArray  Drv;
    CExecObj      *pDrv=gs_pDrvMan;
    Drv.Add(pDrv);

    DoSaveSnapShotScenario(true, TmpSnap, &Drv);

    DisconnectArcManager();
    ASSERT(_CrtCheckMemory());
    CloseDrvManager();

    if (NewDrvManagerName)
      sDrvManagerName=NewDrvManagerName;
    ASSERT(_CrtCheckMemory());
    OpenDrvManager();
    ASSERT(_CrtCheckMemory());
    ConnectArcManager();
    CTagVwDoc::RebuildAll();

    DoLoadSnapShotScenario(true, TmpSnap);

    CDlgBusy::Close();

    if (WithOptions)
      gs_pDrvMan->Options();
    ASSERT(_CrtCheckMemory());
    return true;
    }
  return false;
  }
#endif

//---------------------------------------------------------------------------

void CProject::OpenArcManager(bool FirstOpen)
  {
  bArcOpen=0;
  if (m_bArcOn && m_sArcManagerName.Length()>0)
    {
    if (1)//gs_License.AllowDrivers())
      {
      CWaitMsgCursor Wait("Opening Archive manager");
      bArcOpen=1;
      Strng Name;
      //Name.FnSearchExpand(m_sArcManagerName(), AF_All|AF_BackupFiles);
      Name.FnExpand(m_sArcManagerName());
      //ArcDir.FnDrivePath(Name());
      //FileExt.FnNameExt(Name());

      //gs_pArcMan->Configure(Name(), m_bArcOpenDBOnRun!=0);
      if (gs_pArcMan->Open(Name(), FirstOpen)!=0)
        {
        LogError("Driver", LF_Exclamation, "Error Opening Archive Manager");
        bArcOpen=0;
        }
      }
    else
      {
      LogError("Driver", 0, "Archive not enabled by current licensing options");
      }
    }

  MainWnd()->UpdateStatusBar();
  }

//---------------------------------------------------------------------------

void CProject::ConnectArcManager()
  {
  gs_pArcMan->ConnectAll();
  }

//---------------------------------------------------------------------------

void CProject::DisConnectArcManager()
  {
  gs_pArcMan->DisConnectAll();
  }

//---------------------------------------------------------------------------

void CProject::CloseArcManager(BOOL ForExit/*=FALSE*/, BOOL ForProject/*=FALSE*/)
  {
  if (bArcOpen)
    {
    CWaitMsgCursor Wait("Closing Archive manager");
    gs_pArcMan->Close(!(ForExit || ForProject));
    bArcOpen=0;
    }
  }

//---------------------------------------------------------------------------

flag CProject::ReloadArcManager(BOOL WithOptions, char * NewArcManagerName)
  {
  if (m_bArcOn)
    {
    CWaitCursor Wait;

    if (WithOptions)
      gs_pArcMan->CloseOptions();

    CDlgBusy::Open("\nReloading Archive");

    char TmpSnap[MAX_PATH];
    strcpy(TmpSnap, TemporaryFiles());
    strcat(TmpSnap, "ArcReload.snp");

    CExecObjArray  Arc;
    CExecObj      *pArc=gs_pArcMan;
    Arc.Add(pArc);

    DoSaveSnapShotScenario(true, TmpSnap, &Arc);

    ASSERT(_CrtCheckMemory());
    DisConnectArcManager();
    CloseArcManager();
    if (NewArcManagerName)
      m_sArcManagerName=NewArcManagerName;
    ASSERT(_CrtCheckMemory());
    OpenArcManager(false);
    ConnectArcManager();
    ASSERT(_CrtCheckMemory());
    CTagVwDoc::RebuildAll();

    DoLoadSnapShotScenario(true, TmpSnap);

    CDlgBusy::Close();

    if (WithOptions)
      gs_pArcMan->Options();
    ASSERT(_CrtCheckMemory());
    return true;
    }
  return false;
  }

//---------------------------------------------------------------------------

void CProject::OpenIOMarshal()
  {
  bIOMOpen=0;
  if (m_bIOMOn && m_sIOMarshalName.Length()>0)
    {
    if (gs_License.AllowDrivers())
      {
      CWaitMsgCursor Wait("Opening IOMarshal");
      bIOMOpen=1;

      gs_pIOMarshal->Configure(m_sIOMarshalName(), m_sIOMarshalNode());//, (byte)bDrvIgnoresOn, (byte)bDrvForcesOn, (byte)bDrvReadAll, (byte)bDrvNoiseOn, bDrvStartFBKCheck);
      if (gs_pIOMarshal->Open()!=0)
        {
        LogError("Driver", LF_Exclamation, "Error Opening IOMarshal");
        bIOMOpen=0;
        }
      }
    else
      {
      LogError("Driver", 0, "Drivers not enabled by current licensing options");
      }
    }

  MainWnd()->UpdateStatusBar();
  }

//---------------------------------------------------------------------------

void CProject::CloseIOMarshal(BOOL ForExit/*=FALSE*/, BOOL ForProject/*=FALSE*/)
  {
  if (bIOMOpen)
    {
    CWaitMsgCursor Wait("Closing IOMarshal");
    gs_pIOMarshal->Close(!(ForExit || ForProject));
    Sleep(2500); // Give SysCADMarshal time to shutdown completely
    bIOMOpen=0;
    }
  }

//---------------------------------------------------------------------------

flag CProject::ReloadIOMarshal(BOOL WithOptions, char * NewIOMarshalName, char * NewIOMarshalNode)
  {
  if (m_bIOMOn)
    {
    CWaitCursor Wait;
    if (WithOptions)
      gs_pIOMarshal->CloseOptions();

    CDlgBusy::Open("\nReloading IOMarshal");

    //char TmpSnap[MAX_PATH];
    //strcpy(TmpSnap, TemporaryFiles());
    //strcat(TmpSnap, "IOMReload.snp");

    //CExecObjArray  IOM;
    //CExecObj      *pIOM=gs_pIOMarshal;
    //IOM.Add(pIOM);
    //
    //DoSaveSnapShotScenario(true, TmpSnap, &IOM);

    CloseIOMarshal();
    DisConnectArcManager();

    if (NewIOMarshalName)
      m_sIOMarshalName=NewIOMarshalName;
    if (NewIOMarshalNode)
      m_sIOMarshalNode=NewIOMarshalNode;
    ASSERT(_CrtCheckMemory());
    OpenIOMarshal();
    ASSERT(_CrtCheckMemory());
    ConnectArcManager();
    CTagVwDoc::RebuildAll();

    //DoLoadSnapShotScenario(true, TmpSnap);

    CDlgBusy::Close();

    if (WithOptions)
      gs_pIOMarshal->Options();
    ASSERT(_CrtCheckMemory());
    return true;
    }
  return false;
  }

//---------------------------------------------------------------------------
#if WITHNETSERVER
void CProject::OpenNetManager()
  {
  CWaitCursor Wait;
  CDlgBusy::Open("\n\nOpening Network Manager");
  if (pCS_Mngr->Open()!=0)
    LogError("Network", LF_Exclamation, "Error Opening Network Manager");
  CDlgBusy::Close();
  bNetOpen=1;
  }

//---------------------------------------------------------------------------

void CProject::CloseNetManager(BOOL ForExit/*=FALSE*/)
  {
  if (bNetOpen)
    {
    if (ForExit)
      pCS_Mngr->Close();
    else
      {
      CWaitCursor Wait;
      CDlgBusy::Open("\n\nClosing Network Manager");
      pCS_Mngr->Close();
      CDlgBusy::Close();
      }
    bNetOpen=0;
    }
  }
#endif

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void CProject::OpenDDEManager()
  {
  if (m_bDDEOn)
    {
    //if (gs_License.AllowDDEServer())
      {
      if (pDDESrvr==NULL)
        pDDESrvr = new CDDESrvrHelper;
      if (pDDESrvr->OpenSrvr())
        {//open is OK

        }
      }
      //else
      //  {
      //  LogError("DDE Server", 0, "Not enable by current licensing options");
      //  }
    }
  }

//---------------------------------------------------------------------------

void CProject::CloseDDEManager(BOOL ForExit/*=FALSE*/)
  {
  if (pDDESrvr)
    {
    pDDESrvr->CloseSrvr();
    delete pDDESrvr;
    pDDESrvr = NULL;
    }
  }

//---------------------------------------------------------------------------

//void CProject::OpenOPCManager()
//  {
//  if (bOPCOn)
//    {
//    if (gs_License.AllowOPCServer())
//      {
//      SetOPCServerAllowed(true);
//      if (gs_pMainOPCSrvr)
//        CloseMainOPCSrvr();
//        {
//        COPCSrvrHelper::pMainOPCSrvr->CloseSrvr();
//        delete COPCSrvrHelper::pMainOPCSrvr;
//        COPCSrvrHelper::pMainOPCSrvr = NULL;
//        }
//      if (pOPCSrvr==NULL)
//        pOPCSrvr = new COPCSrvrHelper;
//
//      if (pOPCSrvr->OpenSrvr(ExeFile(), bOPCResetReg, iOPCServerNo))
//        {//open is OK
//        bOPCResetReg=0;
//        }
//      }
//    else
//      {
//      LogError("OPC Server", 0, "Not enable by current licensing options");
//      }
//    }
//  }

//---------------------------------------------------------------------------

//void CProject::CloseOPCManager(BOOL ForExit/*=FALSE*/)
//  {
//  if (pOPCSrvr)
//    {
//    pOPCSrvr->CloseSrvr();
//    delete pOPCSrvr;
//    pOPCSrvr = NULL;
//    SetOPCServerAllowed(false);
//    }
//  }

//---------------------------------------------------------------------------
#define USENEWFINDTAG 0

BOOL CProject::FindTag(LPCTSTR pTag/*=""*/, BOOL DoAutoAccess/*=FALSE*/, BOOL NoErrDlg/*=FALSE*/, BOOL FindNext/*=FALSE*/)
  {//if pTag is specified assume search is in graphics
  char * pGrfPg=(char*)strchr(pTag, '\t');
  if (pGrfPg)
    {
    *pGrfPg++=0;
    NoErrDlg=true;
    }
  BOOL GrfSearch = TRUE;
  Strng sFindTag;
  if (FindNext && TrndPos.sTag.Len()>0)
    {
    GrfSearch = FALSE;
    sFindTag = TrndPos.sTag;
    }
  else
    FindNext = FALSE;
  if (!FindNext)
    {
    if (pTag==NULL || strlen(pTag)==0)
      {
#if USENEWFINDTAG 
      CFindTagsDlg Dlg;
      if (Dlg.DoModal()==IDOK)// && Dlg.sFindTag.GetLength()>0)
        {
        //GrfSearch = !Dlg.bTrendFind;
        //DoAutoAccess = Dlg.m_AutoAccess;
        //sFindTag = (const char*)Dlg.sFindTag;
        }
      else
        return false;
#else
      CFindTagDlg Dlg;
      if (Dlg.DoModal()==IDOK && Dlg.sFindTag.GetLength()>0)
        {
        GrfSearch = !Dlg.bTrendFind;
        DoAutoAccess = Dlg.m_AutoAccess;
        sFindTag = (const char*)Dlg.sFindTag;
        }
      else
        return false;
#endif
      }
    else
      sFindTag = pTag;
    }

  bDoGrfAutoAccess = DoAutoAccess!=0;
  BOOL Found = 0;
  char Buff[256];
  sprintf(Buff, "Searching for tag '%s'", sFindTag());
  if (GrfSearch)
    {//search graphics...
    Strng Cmd("\x1b");
    gs_pCmd->ProcessAStr(Cmd()); //Ensure there are no half complete commands
    CWaitMsgCursor Wait(Buff);
    int DocNotLoadedCnt = 0;
    Strng sFindTagMem(sFindTag);
    for (int iPass=0; iPass<10; iPass++)
      {
      sFindTag=sFindTagMem;
      for (int i=0; i<iPass; i++)
        {
        char * p=sFindTag.XStrRChr('.');
        if (p)
          sFindTag.SetLength(p-sFindTag());
        else
          goto Done;
        }
      for (int iTemp=iGraphTemplate; iTemp<=iVisioTemplate; iTemp++)
        if (ScdApp()->TemplateExists(iTemp))
          {
          POSITION Pos = ScdApp()->Template(iTemp).GetFirstDocPosition();
          while (!Found && Pos)
            {
            CGrfDoc* pGrfDoc = (CGrfDoc*)(ScdApp()->Template(iTemp).GetNextDoc(Pos));
            if (pGrfDoc->GCB.bGrfLoadDefered)
              DocNotLoadedCnt++;
            else if (!pGrfPg || _stricmp(pGrfDoc->GetTitle(), pGrfPg)==0)
              {
              pGrfDoc->OnActivate(true);  // direct cmds to this document
              //        Cmd.Set("mark entity goto %s \r", sFindTag());
              bDoGrfAutoAccess = DoAutoAccess!=0;
              Cmd.Set("find entity goto %s \r", sFindTag());
              Found = (gs_pCmd->ProcessAStr(Cmd())==1);
              }
            }
          }
      }
Done:
    if (Found)
      pStatusBar->SetMsg("Tag '%s' found", sFindTag());
    else
      {
      pStatusBar->SetMsg("Tag '%s' NOT found", sFindTag());
      if (DocNotLoadedCnt>0)
        LogNote(sFindTag(), NoErrDlg ? 0 : LF_Exclamation, "Unable to find tag; %d drawing%s NOT searched (not loaded)", DocNotLoadedCnt, DocNotLoadedCnt>1 ? "s" : "");
      else
        LogNote(sFindTag(), NoErrDlg ? 0 : LF_Exclamation, "Unable to find tag in graphics windows");
      if (DoAutoAccess)
        gs_AccessWnds.AccessNode(-1, sFindTag());
      }
    }
  else
    {//search trends...
    CWaitMsgCursor Wait(Buff);
    if (TrndPos.sTag == sFindTag)
      {
      if (TrndPos.DocPos==NULL)
        {
        TrndPos.DocPos = ScdApp()->TrendTemplate().GetFirstDocPosition();
        TrndPos.iLastPos = 0;
        TrndPos.iFoundCnt = 0;
        }
      }
    else
      {
      TrndPos.sTag = sFindTag;
      TrndPos.DocPos = ScdApp()->TrendTemplate().GetFirstDocPosition();
      TrndPos.iLastPos = 0;
      TrndPos.iFoundCnt = 0;
      }
    CTagVwDoc* pTrndDoc;
    while (!Found && TrndPos.DocPos)
      {
      POSITION PrevDocPos = TrndPos.DocPos;
      pTrndDoc = (CTagVwDoc*)(ScdApp()->TrendTemplate().GetNextDoc(TrndPos.DocPos));
      bDoGrfAutoAccess = DoAutoAccess!=0;
      Found = pTrndDoc->FindTag(TrndPos);
      if (Found)
        {
        TrndPos.DocPos = PrevDocPos;
        TrndPos.iLastPos++;
        }
      }
    if (Found)
      {
      LogNote(sFindTag(), 0, "'%s' found on line %d in %s", sFindTag(), TrndPos.iFoundPos+1, pTrndDoc->GetTitle());
      pStatusBar->SetMsg("'%s' found on line %d in %s", sFindTag(), TrndPos.iFoundPos+1, pTrndDoc->GetTitle());
      }
    else
      {
      pStatusBar->SetMsg("'%s' NOT found", sFindTag());
      if (TrndPos.iFoundCnt>0)
        LogWarning(sFindTag(), 0, "Cannot find more instances of '%s' in trend windows", sFindTag());
      else
        LogWarning(sFindTag(), NoErrDlg ? 0 : LF_Exclamation, "Unable to find '%s' in trend windows", sFindTag());
      }
    }
  return Found;
  }

//---------------------------------------------------------------------------

BOOL CProject::AllGrfLoaded()
  {
  //for (int iTemp=iTrendTemplate; iTemp<iControlTemplate; iTemp++)
  POSITION Pos = ScdApp()->GraphTemplate().GetFirstDocPosition();
  while (Pos)
    {
    CGrfDoc* pGrfDoc = (CGrfDoc*)(ScdApp()->GraphTemplate().GetNextDoc(Pos));
    if (pGrfDoc->GCB.bGrfLoadDefered)
      return FALSE;
    }
  return TRUE;
  }

//---------------------------------------------------------------------------
#if WITHSCRCYCLES
//stuff for screen cycles...
const int MaxTileWnds = 16;
const int MaxStoreWnds = 64;
const int MaxEndMessages = 9;
typedef byte GridType[MaxTileWnds][5];

GridType Grid =
  { {1, 1, 0, 0, 0},
  {2, 1, 1, 0, 0},
  {2, 2, 1, 0, 0},
  {2, 2, 2, 0, 0},
  {3, 2, 2, 1, 0},
  {3, 2, 2, 2, 0},
  {3, 3, 2, 2, 0},
  {3, 3, 3, 2, 0},
  {3, 3, 3, 3, 0},
  {4, 3, 3, 2, 2},
  {4, 3, 3, 3, 2},
  {4, 3, 3, 3, 3},
  {4, 4, 3, 3, 3},
  {4, 4, 4, 3, 3},
  {4, 4, 4, 4, 3},
  {4, 4, 4, 4, 4} };

//---------------------------------------------------------------------------

void CProject::StoreWndList(CWnd* w, flag DoIt)
  {
  if (iCycleWndCnt>=MaxStoreWnds)
    return;
  if (DoIt && w->IsKindOf(RUNTIME_CLASS(CFrameWnd)))
    {
    WINDOWPLACEMENT wp;
    CycleWnds[iCycleWndCnt] = w;
    wp.length = sizeof(wp);
    w->GetWindowPlacement(&wp);
    CyclePos[iCycleWndCnt] = wp;
    wp.showCmd = SW_SHOWNORMAL;
    wp.length = sizeof(wp);
    w->SetWindowPlacement(&wp);
    iCycleWndCnt++;
    }

  CWnd* w1 = w->GetTopWindow();
  CWnd*w1x = w1;
  while (w1)
    {
    StoreWndList(w1, true);
    w1 = w1->GetNextWindow();
    if (w1==w1x)
      w1 = NULL;
    }
  }

//---------------------------------------------------------------------------

void CProject::StartScreenSave()
  {
  if (iCycleNo>=0)
    EndScreenSave();
  CMainFrame* w = MainWnd();
  iMainShowCmd = SW_SHOWMAXIMIZED;
  WINDOWPLACEMENT wp;
  if (CycleWnds)
    delete []CycleWnds;
  if (CyclePos)
    delete []CyclePos;
  CycleWnds = new CWnd*[MaxStoreWnds];
  CyclePos = new WINDOWPLACEMENT[MaxStoreWnds];
  iCycleWndCnt = 0;
  wp.length = sizeof(wp);
  if (w->GetWindowPlacement(&wp))
    {
    iMainShowCmd = wp.showCmd;
    wp.showCmd = SW_SHOWMAXIMIZED;
    wp.length = sizeof(wp);
    w->SetWindowPlacement(&wp);
    }
  BarOn[0] = ((w->m_wndStatusBar.GetStyle() & WS_VISIBLE) != 0);
  w->ShowControlBar(&(w->m_wndStatusBar), FALSE, FALSE);
  /*for (int i=0; i<MaxToolBars; i++)
  {
  BarOn[i+1] = ((w->ToolBars[i].Bar.GetStyle() & WS_VISIBLE) != 0);
  w->ShowControlBar(&(w->ToolBars[i].Bar), FALSE, FALSE);
  }
  w->UpdateToolBars();*/
  StoreWndList(MainWnd(), false);
  if (pCycleDescWnd)
    delete pCycleDescWnd;
  pCycleDescWnd = NULL;
  pOnlyWnd = NULL;
  RotateScreenSave();
  iCycleEndCnt = 0;
  if (iCycleNo<0)
    RestoreWnds();
  }

//---------------------------------------------------------------------------

void CProject::RotateCycle(CWnd* w, CWnd** Wnds, int& WndCnt)
  {
  if (w->IsKindOf(RUNTIME_CLASS(CFrameWnd)))
    {
    CString Txt;
    w->GetWindowText(Txt);
    int index;
    index = Cycles[iCycleNo].FindWindow(Txt);
    if (index>=0 && index<MaxTileWnds)
      {
      Wnds[index] = w;
      WndCnt++;
      }
    }
  CWnd* w1 = w->GetTopWindow();
  CWnd* w1x = w1;
  while (w1)
    {
    RotateCycle(w1, Wnds, WndCnt);
    w1 = w1->GetNextWindow();
    if (w1==w1x)
      w1 = NULL;
    }
  }

//---------------------------------------------------------------------------

void CProject::RotateScreenSave()
  {
  if (iScrCycles<1)
    {
    iCycleNo = -1;
    return;
    }

  WINDOWPLACEMENT wp;
  if (pOnlyWnd)
    {
    wp.length = sizeof(wp);
    if (pOnlyWnd->GetWindowPlacement(&wp))
      {
      wp.showCmd = SW_SHOWNORMAL;
      wp.length = sizeof(wp);
      pOnlyWnd->SetWindowPlacement(&wp);
      }
    pOnlyWnd = NULL;
    }
  flag Done = false;
  flag FoundIt = false;
  int FirstNo = -1;
  while (!Done)
    {
    if (iCycleNo>=0)
      MainWnd()->KillTimer(ID_SCREENCYCLE_TIMER);
    iCycleNo++;
    if (iCycleNo>=iScrCycles)
      iCycleNo = 0;
    if (FirstNo<0)
      FirstNo = iCycleNo;
    else
      {
      if (iCycleNo==FirstNo)
        Done = true;
      }
    if (!Done)
      {
      CWnd* Wnds[MaxTileWnds];
      for (int i=0; i<MaxTileWnds; i++)
        Wnds[i] = NULL;
      int WndCnt = 0;
      RotateCycle(MainWnd(), Wnds, WndCnt);
      if (WndCnt>0)
        {
        // nulls are found in between entries shift all lower enties up above the null
        int actualCnt = 0; //initialize the actual index count
        for (int index=0; index<MaxTileWnds; index++)
          {
          if (Wnds[index] != NULL)
            Wnds[actualCnt++] = Wnds[index];
          }
        ASSERT(WndCnt == actualCnt);
        //for (index=0; index<WndCnt; index++)
        //  Wnds[index]->SetMenu(NULL);
        if (WndCnt==1)
          {
          pOnlyWnd = Wnds[0];
          wp.length = sizeof(wp);
          if (Wnds[0]->GetWindowPlacement(&wp))
            {
            wp.showCmd = SW_SHOWMAXIMIZED;
            wp.length = sizeof(wp);
            Wnds[0]->SetWindowPlacement(&wp);
            }
          }
        else
          {
          for (int Cnt=0; Cnt<WndCnt; Cnt++)
            {
            wp.length = sizeof(wp);
            if (Wnds[Cnt]->GetWindowPlacement(&wp))
              {
              wp.showCmd = SW_SHOWNORMAL;
              wp.length = sizeof(wp);
              Wnds[Cnt]->SetWindowPlacement(&wp);
              }
            }
          RECT r;
          MainWnd()->SetMenu(NULL);
          MainWnd()->GetClientRect(&r);
          int Height = div(r.bottom, Grid[WndCnt-1][0]).quot;
          int y = 0;
          Cnt = 0;
          for (int row=0; row<Grid[WndCnt-1][0]; row++)
            {
            int Width = div(r.right, Grid[WndCnt-1][row+1]).quot;
            int x = 0;
            for (int col=0; col<Grid[WndCnt-1][row+1] && Cnt<WndCnt; col++)
              {
              Wnds[Cnt++]->MoveWindow(x, y, Width, Height);//, false);
              x += Width;
              }
            y += Height;
            }
          for (Cnt=0; Cnt<WndCnt; Cnt++)
            {
            Wnds[Cnt]->BringWindowToTop();
            Wnds[Cnt]->Invalidate();
            }
          MainWnd()->Invalidate();
          }
        MainWnd()->SetMenu(NULL);
        if (pCycleDescWnd==NULL)
          {
          RECT r;
          MainWnd()->GetClientRect(&r);
          //r.top -= 8;
          r.bottom = r.top + 16;
          r.left += 80;
          r.right -= 100;
          pCycleDescWnd = new CTxtWnd();
          pCycleDescWnd->SetBkCol(RGB(255,255,255));
          pCycleDescWnd->SetEnabledCol(RGB(0,0,0));
          pCycleDescWnd->Create(NULL, "", WS_CHILD | WS_VISIBLE/* | WS_CLIPSIBLINGS*/, r, MainWnd(), (UINT)-1);
          }
        pCycleDescWnd->SetText((pchar)(const char*)Cycles[iCycleNo].sDescription);
        pCycleDescWnd->Invalidate();
        MainWnd()->SetTimer(ID_SCREENCYCLE_TIMER, Cycles[iCycleNo].iWaitTime * 1000, NULL);
        Done = true;
        FoundIt = true;
        }
      }
    }
  if (!FoundIt)
    iCycleNo = -1;
  iCycleEndCnt = 0;
  }

//---------------------------------------------------------------------------

void CProject::EndScreenSave()
  {
  if (CycleWnds==NULL)
    {
    iCycleNo = -1;
    return;
    }
  iCycleEndCnt++;
  if (iCycleEndCnt<MaxEndMessages)
    return; //the system seems too sensitive: get messages to end the screen save when we don't want to
  iCycleNo = -1;
  CMainFrame* w = MainWnd();
  w->KillTimer(ID_SCREENCYCLE_TIMER);
  if (pCycleDescWnd)
    delete pCycleDescWnd;
  pCycleDescWnd = NULL;
  WINDOWPLACEMENT wp;
  if (pOnlyWnd)
    {
    wp.length = sizeof(wp);
    if (pOnlyWnd->GetWindowPlacement(&wp))
      {
      wp.showCmd = SW_SHOWNORMAL;
      wp.length = sizeof(wp);
      pOnlyWnd->SetWindowPlacement(&wp);
      }
    pOnlyWnd = NULL;
    }
  RestoreWnds();
  }

//---------------------------------------------------------------------------

void CProject::RestoreWnds()
  {
  CMainFrame* w = MainWnd();
  WINDOWPLACEMENT wp;
  //reset positions of main window
  wp.length = sizeof(wp);
  if (w->GetWindowPlacement(&wp))
    {
    wp.showCmd = iMainShowCmd;
    wp.length = sizeof(wp);
    w->SetWindowPlacement(&wp);
    }

  //reset positions of all child windows
  for (int i=0; i<iCycleWndCnt; i++)
    {
    CyclePos[i].length=sizeof(wp);
    CycleWnds[i]->SetWindowPlacement(&(CyclePos[i]));
    CycleWnds[i]->BringWindowToTop();
    }
  delete []CycleWnds;
  CycleWnds = NULL;
  delete []CyclePos;
  CyclePos = NULL;
  iCycleWndCnt = 0;
  iCycleNo = -1;

  //reset toolbars...
  w->ShowControlBar(&(w->m_wndStatusBar), BarOn[0], FALSE);
  /*for (i=0; i<MaxToolBars; i++)
  w->ShowControlBar(&(w->ToolBars[i].Bar), BarOn[i+1], FALSE);
  w->UpdateToolBars();*/
  }
#endif //end WITHSCRCYCLES
//---------------------------------------------------------------------------

void CProject::BrowseTags()
  {
  gs_HstMngr.BrowseTags();
  }

//---------------------------------------------------------------------------

void CProject::HistorianQuery()
  {
  if (1)
    {
    LogError("History", 0, "Query all tags to csv file not implemented yet...");
    }
  else
    {
    double EndTime = gs_Exec.TheTime.Seconds;
    double Interval = 60.0;
    double Duration = 24.0*60*Interval;
    double StartTime = EndTime - Duration;
    Strng Filename = "c:\\test_qry";
    gs_HstMngr.QueryToFile(Filename(), StartTime, Duration, Interval);
    }
  }

//---------------------------------------------------------------------------

void CProject::CheckLicense(BOOL StartingSolve)
  {
  gs_License.CheckLicense(StartingSolve!=0, m_bIOMOn!=0, CWindowLists::GetTrendWndCount(), CWindowLists::GetGrfWndCount());
  }

//---------------------------------------------------------------------------

void CProject::CheckLicenseConditions()
  {
  gs_License.CheckLicenseConditions(m_bIOMOn!=0, CWindowLists::GetTrendWndCount(), CWindowLists::GetGrfWndCount());
  }

//---------------------------------------------------------------------------

int CProject::PrepareMerge(CMergeProjectsInfo& MPI)
  {
  CDlgBusy::Open("\nPreparing project merge");
  CDlgBusy::SetLine(3, "Build current project file lists");
  //build master trend document lists...
  MPI.MasterTrends.SetSize(CWindowLists::GetTrendWndCount());
  int i = 0;
  for (int iTemp=iTrendTemplate; iTemp<=iControlTemplate; iTemp++)
    if (ScdApp()->TemplateExists(iTemp))
      {
      POSITION Pos = ScdApp()->Template(iTemp).GetFirstDocPosition();
      while (Pos)
        {
        CTagVwDoc* pTrndDoc = (CTagVwDoc*)(ScdApp()->Template(iTemp).GetNextDoc(Pos));
        MPI.MasterTrends[i++] = (const char*)(pTrndDoc->GetTitle());
        }
      }
    //build master graphics document lists...
    MPI.MasterGraphics.SetSize(CWindowLists::GetGrfWndCount());
    i = 0;
    POSITION Pos = ScdApp()->GraphTemplate().GetFirstDocPosition();
    while (Pos)
      {
      CGrfDoc* pGrfDoc = (CGrfDoc*)(ScdApp()->GraphTemplate().GetNextDoc(Pos));
      MPI.MasterGraphics[i++] = (const char*)(pGrfDoc->GetTitle());
      }
    //build master referenced files list...
    MPI.MasterRefFiles.SetSize(0);
    MPI.MasterRefModels.SetSize(0);
    int RefCnt = 0;
    CPrjFilesLists PFL;
    PFL.BuildLists();
    for (i=0; i<PFL.GetSize(); i++)
      {
      for (int j=0; j<PFL.pExecFiles[i]->Files.GetSize(); j++)
        {
        CExecObjFileInfo &FI = PFL.pExecFiles[i]->Files[j];
        if ((FI.dwFlags & EFF_Grp_Referenced) && (FI.dwFlags & EFF_OtherValid))
          {
          MPI.MasterRefFiles.Add(FI.sFilename());
          MPI.MasterRefModels.Add(FI.sOther());
          }
        }
      }

    //perform final checks, prepare merge tag lists, reports etc...
    int err = gs_Exec.PrepareMerge(MPI);
    CDlgBusy::Close();
    if (err!=0)
      LogError("Merge", 0, MPI.sError());
    return err;
  }

//---------------------------------------------------------------------------

flag CProject::Merge()
  {
  gs_AccessWnds.CloseAccessData(-1, false, false, true);
  gs_AccessWnds.CloseWnd(-1);

  return ImportFlwsheet();
  }

//---------------------------------------------------------------------------

flag CProject::ImportFlwsheet()
  {
  Strng TmpContractStr;
  //initialize the MergeProject Helper class...
  CProfINIFile PF(PrjIniFile());
  CMergeProjectsInfo* pMPI = new CMergeProjectsInfo;
  CMergeProjectsInfo& MPI = *pMPI;
  MPI.sSlavePrj = "";
  flag Loop = true;
  while (Loop)
    {
    Loop = false;
    MPI.sReportFile = PrjFiles();
    MPI.sReportFile += "merge.csv";
    MPI.sMasterModelCfg = CfgFile();
    MPI.sTagAppendChars = PF.RdStr("MergeProject", "TagAppendChars", "_");
    MPI.iMasterChangeType = PF.RdInt("MergeProject", "CurrentChangeType", MRG_ChangeDuplicates);
    MPI.iSlaveChangeType = PF.RdInt("MergeProject", "ImportChangeType", MRG_ChangeDuplicates);
    MPI.iMasterRenameType = PF.RdInt("MergeProject", "CurrentRenameType", MRG_Prefix);
    MPI.iSlaveRenameType = PF.RdInt("MergeProject", "ImportRenameType", MRG_Prefix);
    MPI.sMasterPrefix = PF.RdStr("MergeProject", "CurrentPrefix", "C_");
    MPI.sSlavePrefix = PF.RdStr("MergeProject", "ImportPrefix", "I_");
    MPI.sMasterSuffix = PF.RdStr("MergeProject", "CurrentSuffix", "_C");
    MPI.sSlaveSuffix = PF.RdStr("MergeProject", "ImportSuffix", "_I");
    MPI.bMasterNumTagsBAD   = TaggedObject::NumericTagsBad;
    MPI.bMasterStNumTagsBAD = TaggedObject::NumericStartingTagsBad;
    MPI.bMasterNumTagsBAD   = MPI.bMasterNumTagsBAD || MPI.bMasterStNumTagsBAD;

    MPI.bSlaveNumTagsBAD    = true;
    MPI.bSlaveStNumTagsBAD  = true;

    CMergeDlg Dlg(&MPI);
    if (Dlg.DoModal()!=IDOK)
      {
      delete pMPI;
      return false;
      }

    TaggedObject::NumericTagsBad=pMPI->bMasterNumTagsBAD;
    TaggedObject::NumericStartingTagsBad=pMPI->bMasterStNumTagsBAD;

    CWaitMsgCursor Wait("Preparing project merge");
    int err = PrepareMerge(MPI);
    if (err!=0)
      {
      delete pMPI;
      return false;
      }

    //generate merge report dialog...
    PF.WrStr("MergeProject", "TagAppendChars", MPI.sTagAppendChars());
    PF.WrInt("MergeProject", "CurrentChangeType", MPI.iMasterChangeType);
    PF.WrInt("MergeProject", "ImportChangeType", MPI.iSlaveChangeType);
    PF.WrInt("MergeProject", "CurrentRenameType", MPI.iMasterRenameType);
    PF.WrInt("MergeProject", "ImportRenameType", MPI.iSlaveRenameType);
    PF.WrStr("MergeProject", "CurrentPrefix", MPI.sMasterPrefix());
    PF.WrStr("MergeProject", "ImportPrefix", MPI.sSlavePrefix());
    PF.WrStr("MergeProject", "CurrentSuffix", MPI.sMasterSuffix());
    PF.WrStr("MergeProject", "ImportSuffix", MPI.sSlaveSuffix());
    CMergeReportDlg RepDlg(&MPI);
#if CK_LICENSINGON
    if (gs_License.TrendWindowsAllowed()!=CK_InfiniteTrends && MPI.MasterTrends.GetSize()+MPI.SlaveTrends.GetSize()>gs_License.TrendWindowsAllowed())
      {
      LogWarning("SysCAD", 0, "Maximum number of trend windows allowed will exceed limit of %d allowed by license", gs_License.TrendWindowsAllowed());
      RepDlg.bMergeAllowed = 0;
      }
    if (gs_License.GraphicWindowsAllowed()!=CK_InfiniteGrfs && MPI.MasterGraphics.GetSize()+MPI.SlaveGraphics.GetSize()>gs_License.GraphicWindowsAllowed())
      {
      LogWarning("SysCAD", 0, "Maximum number of graphics windows allowed will exceed limit of %d allowed by license", gs_License.GraphicWindowsAllowed());
      RepDlg.bMergeAllowed = 0;
      }
    const int iUnitsAllowed = gs_License.MaxNodesAllowed();
    if (iUnitsAllowed!=CK_InfiniteUnits && MPI.iMasterTagCnt+MPI.iSlaveTagCnt>iUnitsAllowed)
      {
      LogWarning("SysCAD", 0, "Maximum number of units allowed will exceed limit of %d allowed by license", iUnitsAllowed);
      RepDlg.bMergeAllowed = 0;
      }
    if (RepDlg.bMergeAllowed==0)
      {
      LogError("SysCAD", LF_Exclamation, "License limits will not allow a project merge. (see messages)");
      RepDlg.sMessage = "License limits will not allow a project merge.";
      }
#endif
    if (MPI.iProblemRefFiles>0 && (MPI.bIgnoreProbRefFiles==0 || MPI.iRefFilesExistsCnt>0))
      {
      RepDlg.bMergeAllowed = 0;
      //LogError("Merge", LF_Exclamation, "Different referenced files with the same name used in both projects. Please change before merging.");
      LogError("Merge", 0, "Cannot merge because of problem with referenced files.");
      RepDlg.sMessage = "Cannot merge because of problem with referenced files.";
      }
    int RetID = RepDlg.DoModal();
    if (RetID==IDCANCEL)
      Loop = true;
    else if (RetID==IDOK && RepDlg.bMergeAllowed)
      {//do the actual merge...
      CStopWatch SW;
      SW.Start();
      CWaitMsgCursor Wait("Merging projects");
      CDlgBusy::Open("");
      Strng s1;
      CString cs1;
      s1.FnNameExt(MPI.sSlavePrj());
      CDlgBusy::SetLine(1, "Project merge : Importing %s", s1());
      int err = gs_Exec.DoMergeA(MPI);
      if (err!=0)
        {
        delete pMPI;
        CDlgBusy::Close();
        return false;
        }
      //copy and load trends and graphics documents...
      for (int j=0; j<2; j++)
        {
        Strng FullFile,s,s1,s2,s3;
        Strng DocTypeName = (j==0 ? "Graphics" : "Trend");
        CSVector& ChangeLst = (j==0 ? MPI.PostChangeGraphics : MPI.PostChangeTrends);
        CSVector& NewLst = (j==0 ? MPI.PostNewGraphics : MPI.PostNewTrends);
        CSVector& SlaveLst = (j==0 ? MPI.SlaveGraphics : MPI.SlaveTrends);
        for (int i=0; i<SlaveLst.GetSize(); i++)
          {
          int index = ChangeLst.Find(SlaveLst[i]());
          if (index>=0)
            FullFile = NewLst[index]();
          else
            {
            s.FnNameExt(SlaveLst[i]());
            FullFile.Set("%s%s", PrjFiles(), s());
            }
          s3.FnNameExt(FullFile());
          TmpContractStr.FnContract(s3());
          TmpContractStr.FnCompactPath(50);
          CDlgBusy::SetLine(3, "Loading %s : %s", DocTypeName(), TmpContractStr());
          CopyFile(SlaveLst[i](), FullFile(), true);
          if (j==0)
            {
            s1.FnDrivePathName(SlaveLst[i]());
            s1 += ".dxf";
            s2.FnDrivePathName(FullFile());
            s2 += ".dxf";
            CopyFile(s1(), s2(), false);
            }
          ScdApp()->OpenDocumentFile(FullFile());
          }
        }
      //complete the merge...
      CDlgBusy::SetLine(1, "Merging projects");
      err = gs_Exec.DoMergeB(MPI);
      CDlgBusy::SetLine(3, "");
      if (gs_Exec.CheckAllTags(TaggedObject::NumericTagsBad, TaggedObject::NumericStartingTagsBad)<0)
        {
        TaggedObject::NumericTagsBad = 0;
        TaggedObject::NumericStartingTagsBad = 0;
        }
      //CloseHistorian();
      //OpenHistorian();
      CloseIOMarshal();
      DisConnectArcManager();
#if WITHDRVMAN
      CloseDrvManager();
#endif

#if WITHDRVMAN
      OpenDrvManager();
#endif
      ConnectArcManager();
      OpenIOMarshal();
      CDlgBusy::SetLine(3, "Connecting trends");
      CTagVwDoc::RebuildAll();
      CDlgBusy::Close();
      if (err!=0)
        {
        delete pMPI;
        return false;
        }
      CheckLicenseConditions();
      MainWnd()->PostMessage(WMU_UPDATEMAINWND, SUB_UPDMAIN_ACTIVATECMDS, 0);
      MainWnd()->PostMessage(WMU_CMD, SUB_CMD_PRJWNDUPDATE, 0);
      SW.Stop();
      LogNote("SysCAD", LF_Exclamation, "Project merge complete.  Time %s", SW.MinSecDesc(cs1));
      }
    else
      {
      delete pMPI;
      return false;
      }
    }
  delete pMPI;
  return true;
  }

//---------------------------------------------------------------------------

flag CProject::FileImport()
  {
  CSelectImport Dlg;
  if (Dlg.DoModal()==IDOK)
    {
    gs_AccessWnds.CloseAccessData(-1, false, false, true);
    gs_AccessWnds.CloseWnd(-1);

    switch (Dlg.m_lImportWhat)
      {
      case 0:
        {
        CNeutralImportDBDlg Dlg;
        if (Dlg.DoModal()==IDOK)
          {
          CNeutralImportExport NImport;
          if (NImport.DoImportDB(Dlg.Options(), Dlg.m_sGrfDatabase, Dlg.m_sMdlDatabase))
            return true;
          }
        return false;
        }
      case 1:
        return ImportFlwsheet();
      case 2:
        {
        CElectricalImport E;
        return E.Import();
        }
      }
    }
  return false;
  }
//---------------------------------------------------------------------------

flag CProject::FileExport()
  {
  CSelectExport Dlg;
  if (Dlg.DoModal()==IDOK)
    {
    gs_AccessWnds.CloseAccessData(-1, false, false, true);
    gs_AccessWnds.CloseWnd(-1);

    switch (Dlg.m_lExportWhat)
      {
      case 0:
        {
        CNeutralExportDBDlg Dlg;
        if (Dlg.DoModal()==IDOK)
          {
          CDocTemplate & GT = ScdApp()->GraphTemplate();

          CNeutralImportExport NExport;
          NExport.DoExport(Dlg.Options(), Dlg.m_sGrfDatabase, Dlg.m_sMdlDatabase);

          //CNeutralGrfImportExport EGH;
          //EGH.DoExport(GT, Dlg.m_sGrfDatabase);

          //CNeutralMdlImportExport EMH;
          //EMH.DoExport(Dlg.m_sMdlDatabase);
          return true;
          }
        return false;
        }
        //case 1:
        //  return 0;//ExportFlwsheet();
        //case 2:
        //  {
        //  CElectricalImport E;
        //  return E.Import();
        //  }
      }
    }
  return false;
  }
//---------------------------------------------------------------------------

void CProject::LoadHelpFileList()
  {
  flag OK = false;

  CTokenFile Tkns(AF_All|AF_BackupFiles, CfgFile());
  Tkns.SetSeperators(": =,;\t\v\f");
  Tkns.SetWhiteSpace(" \t\v\f");
  Tkns.SetIgnoreComments(true);

  flag HelpListSpecd = false;
  flag KwMdlHelpSpecd = false;

  if (Tkns.Open())
    {
    m_MdlHelpFileList.SetSize(0);

    Strng Tkn = Tkns.NextToken();
    while (!Tkns.AtEOF())
      {
      Strng What(Tkn());
      Tkn = Tkns.NextToken();
      if (Tkn.XStrICmp(":")==0)
        Tkn = Tkns.NextToken();

      //      DoBreak();
      if (What.XStrICmp("ModelHelpFiles")==0)
        {
        HelpListSpecd = true;
        while (!Tkns.AtEOF())
          {
          if (Tkn.XStrICmp("End")==0)
            {
            Tkn = Tkns.NextToken();
            break;
            }
          else
            {
            Strng Name(Tkn());
            if (Name.Length()<=4)
              Name += ".HLP";
            else if (_stricmp(&Name[Name.Length()-4], ".HLP")!=0)
              Name += ".HLP";
            KwMdlHelpSpecd = (KwMdlHelpSpecd || (_stricmp(Name(), "Models.hlp")==0));
            //Strng HelpPath;
            //HelpPath.Set("%sBIN\\%s", RootDirectory(), Name());
            // Does This Help file Exist
            m_MdlHelpFileList.Add(Name());
            Tkn = Tkns.NextToken();
            }
          if (Tkn.XStrICmp(",")==0)
            Tkn = Tkns.NextToken();
          }
        }
      }
    Tkns.Close();
    }
  if (!HelpListSpecd)
    m_MdlHelpFileList.Add("Models.hlp");
  else
    {
    if (!KwMdlHelpSpecd)
      m_MdlHelpFileList.Add("Models.hlp");
    }
  }

//---------------------------------------------------------------------------

flag CProject::ModelHelp(char* pMdlTxt/*=NULL*/, int HelpIndex/*=0*/)
  {
  ASSERT(HelpIndex<m_MdlHelpFileList.GetSize());
  Strng Result;
  FindDLLOrHlp((char*)(const char*)(m_MdlHelpFileList[HelpIndex]), DllFilesPath(), Result);

  flag b;
  if (pMdlTxt==NULL || strlen(pMdlTxt)==0)
    b = ::WinHelp(AfxGetMainWnd()->m_hWnd, Result(), HELP_CONTENTS, 0);
  else
    b = ::WinHelp(AfxGetMainWnd()->m_hWnd, Result(), HELP_PARTIALKEY, (DWORD)pMdlTxt);
  return b;
  }

//---------------------------------------------------------------------------
// ExecObj Overridables

void CProject::EO_OnAppActivate(BOOL bActive)
  {
  }

//---------------------------------------------------------------------------

flag CProject::EO_RequestTagInfo(RequestTagInfoRec& Rqst, ReplyTagInfoRec& Info)
  {
#if WithRmtFile
  const int MaxCnt = 7;
#else
  const int MaxCnt = 6;
#endif
  switch (Rqst.RQ_Type)
    {
    case RQ_Files:
      if (Info.Count()<MaxCnt)
        {
        if (Info.Count()==0)
          {
          Info.sData = PrjFile();
          Info.sData.FnContract();
          return true;
          }
        Info.sData = PrjFiles();
        if (Info.Count()==1)
          Info.sData += LayoutFileName;
        else if (Info.Count()==2)
          Info.sData += PrjIniFileName();
        else if (Info.Count()==3)
          Info.sData += OPT_FILE;// += OPT_FILE; //defined in slvtool.cpp
        else if (Info.Count()==4)
          Info.sData += CCopyBlock::CopyBlkFileName;// defined in copyblk.cpp
        else if (Info.Count()==5)
          Info.sData += RmtBackupZipFileName;
#if WithRmtFile
        else if (Info.Count()==6)
          Info.sData += RmtFileName;
#endif
        Info.sData.FnContract();
        return true;
        }
      break;
    }
  return false;
  }

//---------------------------------------------------------------------------

flag CProject::EO_GotoRunStart(CXM_TimeControl &CB)
  {
  //bool ScrollTrendsToEnd = TimeChanged;
  //if (gs_Exec.RestartHstOnStart)
  //  {

  //  gs_pPrj->CloseHistorian();
  //  gs_pPrj->RestartHistorian();
  //  gs_pPrj->OpenHistorian();
  //  //ScrollTrendsToEnd = true;
  //  //if (gs_Exec.DefNetProbalMode())
  //  //  gs_Exec.SetTime(0.0); //reset time to "zero"
  //  }
  ////if (gs_Exec.ResetTimeOnStart && !gs_Exec.DefNetProbalMode() && !gs_Exec.SyncWithClock())
  ////  {
  ////  gs_Exec.SetTime(gs_Exec.TimeAtStart, gs_Exec.RestartHstOnStart!=eOOO_Off);
  ////  ScrollTrendsToEnd = true;
  ////  }
  ////if (gs_Exec.SetHstTaglistOnStart)
  ////  {
  ////  Strng File(gs_Exec.HstTaglistFile);
  ////  if (File.Len()>0)
  ////    {
  ////    Strng FullFilename(gs_Exec.HstTaglistFolder);
  ////    if (FullFilename.Len()==0)
  ////      {
  ////      FullFilename = PrjFiles();
  ////      gs_Exec.HstTaglistFolder=FullFilename();
  ////      }
  ////    FullFilename.FnExpand();
  ////    FullFilename += File();
  ////    gs_Exec.KeepHistoryFile(FullFilename());
  ////    }
  ////  }

  ////if (gs_Exec.RestartHstOnStart==eOOO_Once)
  ////  gs_Exec.RestartHstOnStart=eOOO_Off;
  ////if (gs_Exec.ResetTimeOnStart==eOOO_Once)
  ////  gs_Exec.ResetTimeOnStart=eOOO_Off;
  ////if (gs_Exec.SetHstTaglistOnStart==eOOO_Once)
  ////  gs_Exec.SetHstTaglistOnStart=eOOO_Off;

  //if (TimeChanged)
  //  CTagVwDoc::AdjustTimebaseToEndAll();
  return true;
  }

//---------------------------------------------------------------------------

flag CProject::EO_GotoRunEnd(CXM_TimeControl &CB, bool TimeChanged)
  {
  bool ScrollTrendsToEnd = TimeChanged;
  if (CB.m_ScnState==Scn_Start && gs_Exec.RestartHstOnStart)
    {

    gs_pPrj->CloseHistorian();
    gs_pPrj->RestartHistorian();
    gs_pPrj->OpenHistorian();
    //ScrollTrendsToEnd = true;
    //if (gs_Exec.DefNetProbalMode())
    //  gs_Exec.SetTime(0.0); //reset time to "zero"
    }
  //if (gs_Exec.ResetTimeOnStart && !gs_Exec.DefNetProbalMode() && !gs_Exec.SyncWithClock())
  //  {
  //  gs_Exec.SetTime(gs_Exec.TimeAtStart, gs_Exec.RestartHstOnStart!=eOOO_Off);
  //  ScrollTrendsToEnd = true;
  //  }
  //if (gs_Exec.SetHstTaglistOnStart)
  //  {
  //  Strng File(gs_Exec.HstTaglistFile);
  //  if (File.Len()>0)
  //    {
  //    Strng FullFilename(gs_Exec.HstTaglistFolder);
  //    if (FullFilename.Len()==0)
  //      {
  //      FullFilename = PrjFiles();
  //      gs_Exec.HstTaglistFolder=FullFilename();
  //      }
  //    FullFilename.FnExpand();
  //    FullFilename += File();
  //    gs_Exec.KeepHistoryFile(FullFilename());
  //    }
  //  }

  //if (gs_Exec.RestartHstOnStart==eOOO_Once)
  //  gs_Exec.RestartHstOnStart=eOOO_Off;
  //if (gs_Exec.ResetTimeOnStart==eOOO_Once)
  //  gs_Exec.ResetTimeOnStart=eOOO_Off;
  //if (gs_Exec.SetHstTaglistOnStart==eOOO_Once)
  //  gs_Exec.SetHstTaglistOnStart=eOOO_Off;

  if (TimeChanged)
    CTagVwDoc::AdjustTimebaseToEndAll();
  return true;
  }

//---------------------------------------------------------------------------

flag CProject::EO_PreStart(CXM_TimeControl &CB)
  {
  if (m_bRunOptEmptySpills)
    {
    Strng_List RqdTags;
    gs_Exec.SetModelState(MSA_EmptySpillTargets, RqdTags);
    }
  if (m_bRunOptEmptyAll)
    {
    Strng_List RqdTags;
    gs_Exec.SetModelState(MSA_Empty, RqdTags);
    }
  if (m_bRunOptZeroFlows)
    {
    Strng_List RqdTags;
    gs_Exec.SetModelState(MSA_ZeroFlows, RqdTags);
    }
  if (m_bRunOptInitialiseAll)
    {
    Strng_List RqdTags;
    gs_Exec.SetModelState(MSA_PreSet, RqdTags);
    }
  if (m_bRunOptOnceOnly)
    {
    Strng_List RqdTags;
    gs_Exec.SetModelState(MSA_PreSet, RqdTags);
    }
  if (m_bRunOptResetStats)
    {
    Strng_List SL;
    gs_Exec.SetModelState(MSA_DynStatsRunInit, SL);
    };   

  if (m_bRunOptOnceOnly)
    {
    m_bRunOptEmptyAll      = 0;     
    m_bRunOptEmptySpills   = 0;
    m_bRunOptZeroFlows     = 0;    
    m_bRunOptInitialiseAll = 0;
    m_bRunOptResetStats    = 0;   
    }
  return true;
  }

//---------------------------------------------------------------------------

flag CProject::EO_Starting(flag fBeginStarting)
  {
  CMainFrame* pWnd=(CMainFrame*)AfxGetApp()->GetMainWnd();//MainWnd();
  if (pWnd->m_pPrjSheet)
    pWnd->m_pPrjSheet->PostMessage(WM_CLOSE);
  return true;
  }

//---------------------------------------------------------------------------

flag CProject::EO_Stopping(flag fBeginStopping)
  {
  if (!fBeginStopping)
    {
    //ScdPFUser.WrDouble("General", "ExecTime", gs_Exec.TheTime);
    // Save Current State
    }
  return true;
  }

//===========================================================================
#if WITHSCRCYCLES
CScreenSaverInfo::CScreenSaverInfo()
  {
  iWaitTime = 10;
  sDescription = "";
  Windows.RemoveAll();
  }

//---------------------------------------------------------------------------

CScreenSaverInfo::~CScreenSaverInfo()
  {
  Windows.RemoveAll();
  }

//---------------------------------------------------------------------------

CScreenSaverInfo& CScreenSaverInfo::operator=(const CScreenSaverInfo& S)
  {
  iWaitTime = S.iWaitTime;
  sDescription = S.sDescription;
  Windows.RemoveAll();
  if (S.Windows.GetSize()>0)
    {
    Windows.SetSize(S.Windows.GetSize());
    for (int i=0; i<Windows.GetSize(); i++)
      Windows.SetAt(i, S.Windows.GetAt(i));
    }
  return *this;
  }

//---------------------------------------------------------------------------
/*#f this returns an index value, such that Wnds[] can match Cycles[]
if the window is not found it returns -1*/
int CScreenSaverInfo::FindWindow(CString& s)
  {
  const char* p = (const char*)s;
  for (int i=0; i<Windows.GetSize(); i++)
    {
    if (_stricmp((const char*)(Windows.GetAt(i)), p)==0)
      return i;
    }
  return -1;
  }
#endif
//===========================================================================
//
//
//
//===========================================================================

flag CProject::DoNew(flag InConstructor)
  {
  Clear();
  SetUsingPrjLclFiles(false);
  SetSymbolicPaths(false);
  gs_CnvsMngr.Clear();
  return true;
  }

static char *DocumentSectNames[]=
  {
  "ProjectDocuments",
  "GraphicsDocuments",
  "VisioDocuments",
  "TrendDocuments",
  "CtrlTrendDocuments",
  "AutomationDocuments"
  };

static char *DocumentTmplNames[]=
  {
  "Project",    // iProjectTemplate
  "Graphics",   // iGraphTemplate
  "Visio",      // iVisioTemplate
  "Trend",      // iTrendTemplate
  "Control",    // iControlTemplate
  "AutoMation", // iAutoTemplate
  };

//===========================================================================

flag CProject::SetupRmtCopies()
  {
#if WithRmtFile
  flag RmtOK=true;
  Strng Fn, FnL, E;
  for (int i=0; i<100; i++)
    {
    Fn.Set("%sTmp%i", TemporaryFiles(), i);
    //Fn.Set("%sTmp%i", PrjFiles(), i);
    if (FileExists(Fn()))
      {
      FnL=Fn;
      FnL.FnCheckEndBSlash();
      FnL+="Lock.Tmp";
      CFile LF;
      CFileException e;
      if (LF.Open(FnL(), CFile::modeCreate|CFile::modeWrite|CFile::shareExclusive/*|CFile::typeText*/, &e))
        break;
      }
    else
      {
      if (!FnCreatePath(Fn(), E))
        {
        LogError("SysCAD", 0, "TempFolder %s Not Created", Fn());
        RmtOK=false;
        }
      }
    break;
    }

  if (RmtOK)
    {
    m_RmtCpyFolder=Fn;
    ClearFolder(m_RmtCpyFolder(), false);

    FnL=m_RmtCpyFolder;
    FnL.FnCheckEndBSlash();
    FnL+="Lock.Tmp";

    CFileException e;
    if (!m_RmtCpyLock.Open(FnL(), CFile::modeCreate|CFile::modeWrite|CFile::shareExclusive/*|CFile::typeText*/, &e))
      {
      char Buff[1024];
      e.GetErrorMessage(Buff, sizeof(Buff));
      LogError("SysCAD", 0, "Lock File %s not opened : %s", FnL(), Buff);
      RmtOK=false;
      }

    if (RmtOK)
      {
      Strng StgFn=PrjFiles();
      StgFn+=RmtFileName;
      ExpandStg(StgFn(), Fn());
      SetPrjRmtCpyFiles(m_RmtCpyFolder());
      }
    }
  return RmtOK;
#else
  return True;
#endif
  }

//===========================================================================

char* CProject::ChkPrjFileName(char *pPrjPath)
  {
  char Fn[512];
  Fn[0] = 0;
  if ((pPrjPath==NULL) || strstr(pPrjPath, "*") || strlen(pPrjPath)==0)
    {
    DWORD dwFlags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_SHAREAWARE;
    char InitDir[512];

    CString s = ScdPFUser.RdStr("General", "LastCfgFile", "");
    //if (s.GetLength()==0)
    //  s = ScdPFUser.RdStr("General", "RecentModelLib", "");
    if (s.GetLength()>0)
      strcat(InitDir, (const char*)s);
    CSCDFileDialog Dlg(true, NULL, "*.spj", dwFlags, "SysCAD project (*.spj)|*.spj||");
    Dlg.m_ofn.lpstrInitialDir = InitDir;
    //Dlg.m_ofn.lpstrTitle = "???";
    if (Dlg.DoModal()==IDOK)
      {
      strcpy(Fn, (const char*)Dlg.GetPathName());
      pPrjPath = Fn;
      }
    }
  return pPrjPath;
  }

//===========================================================================

flag CProject::SetPrjCfgFile(CProfINIFile & PF, Strng &NewModelCfg, CModelInfo & Info)
  {
  //>>>>>>>>>>>>>>>>>>>>>>>>>>>
  // Config File
  Strng sOldModelCfg = PF.RdStr("General", "ModelLibrary", "");
  sOldModelCfg = PF.RdStr("General", "ModelConfiguration", sOldModelCfg());
  sOldModelCfg = PF.RdStr("General", "CfgFile", sOldModelCfg());
  sOldModelCfg.FnExpand();

  SwingCfgFileIfReqd(sOldModelCfg);

  if (!NewModelCfg() && !sOldModelCfg())
    {
    //#pragma chCHECKIT("if sOldModelCfg does not exist the query OpenBackup")

    Strng InitDir(ScdPFUser.RdStr("Folders", "CfgSearchPath", StartupDirectory()));

    CSCDFileDialog Dlg(true, NULL, "*.CFG",
      OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_SHAREAWARE,
      "Missing Configuration (*.CFG)|*.CFG||", ::AfxGetMainWnd());
    Dlg.m_ofn.lpstrInitialDir = InitDir();
    Dlg.m_ofn.lpstrTitle = "Browse for Configurations";
    if (Dlg.DoModal()==IDOK)
      {
      sOldModelCfg=Dlg.GetPathName();
      Strng Path;
      Path.FnDrivePath(sOldModelCfg());
      ScdPFUser.WrStr("Folders", "CfgSearchPath", Path());
      }
    else
      return false;
    }

  Strng sOldModelCfgMem(sOldModelCfg);
  Strng sNewCfg(sOldModelCfg);
  sNewCfg.FnCheckExtension(".Cfg");
  byte Where=0;
  flag AbortLoad=false;

  if (NewModelCfg())
    {
    FnSetTestLocalFiles(true);
    sNewCfg=NewModelCfg();
    }
  else
    {
    // Check to see whether we need to swing CfgFile Source

    // OldCfg    :   D:\A\B\C\4.cfg
    // OldPrj    :   D:\A\B\x.spf
    // NewPrj    :   C:\X\Y\Z\x.spf
    // lStartEql =   ......^         = 7
    // lEndEql              ^....    = 6

    // NewCfg    :   C:\X\Y\Z\C\4.cfg

    Strng PFiles(PrjFiles());
    if (0) // for Testing
      {
      sOldModelCfg = "D:\\A\\B\\C\\4.cfg";
      m_OldPrjFiles  = "D:\\A\\B\\x.spf";
      PFiles       = "C:\\X\\Y\\Z\\x.spf";
      }

    // Find start part of OldProject and OldCfg which are equal
    int iOPrjLen=m_OldPrjFiles.Length();
    int iOCfgLen=sOldModelCfg.Length();
    int iNPrjLen=PFiles.Length();
    int lStartEql=-1;
    int i0=Min(iOCfgLen, iOPrjLen);
    for (int i=0; i<i0; i++)
      {
      if (sOldModelCfg[i]!=m_OldPrjFiles[i])
        break;
      if (sOldModelCfg[i]=='\\')
        lStartEql=i+1;
      }
    if (lStartEql>0)
      {
      // Find end part of OldProject and NewProject which are equal
      int lEndEql=-1;
      int i0=iOPrjLen-1;
      int i1=iNPrjLen-1;
      for ( ; i0>=0 && i1>=0; i0--, i1--)
        {
        if (m_OldPrjFiles[i0]!=PFiles[i1])
          break;
        if (m_OldPrjFiles[i0]=='\\')
          lEndEql=iOPrjLen-i0-1;
        }

      // Overlap ?
      if (lStartEql+lEndEql>=iOPrjLen)
        {
        lEndEql=iOPrjLen-lStartEql;

        Strng SwungModelCfg, CfgRemainder;//, PrjRemainder, NPrjRemainder;
        int lCfgRemainder=iOCfgLen-lStartEql;
        CfgRemainder=sOldModelCfg.Right(lCfgRemainder);

        SwungModelCfg=PFiles.Left(PFiles.Length()-lEndEql);
        SwungModelCfg+=CfgRemainder();
        if (FileExists(SwungModelCfg()))
          sNewCfg=SwungModelCfg;
        }
      }

    Strng Tmp;
    Tmp.FnDrivePath(sNewCfg());
    if (Tmp.Length()==0)
      {
      Tmp=sNewCfg();
      sNewCfg=PrjFiles();
      sNewCfg.FnClearEndBSlash();
      sNewCfg.FnDrivePath();
      sNewCfg+=Tmp();
      }

    // Check Old copy
    if (PrjFileVerNo()>=29)
      {
      Strng DLcl=sNewCfg();
      DLcl.FnExpand();
      DLcl.FnMakePrjFileRelative();
      DLcl.FnContract();
      DLcl.FnMakePathInLocal();
      Strng SymNewCfg=sNewCfg;
      SymNewCfg.FnContract();
      AbortLoad=!FnLocalCompareFiles(SymNewCfg(), sNewCfg(), DLcl());
      }
    else
      {
      Strng FnLcl, DLcl;
      FnLcl.FnNameExt(sNewCfg());
      DLcl=PrjRmtCpyFiles();
      DLcl+=FnLcl;
      AbortLoad=FnSearchDlg(sNewCfg, DLcl, NULL, FnLcl())<0;
      }
    }


  //Info.Saved.m_sCfgFiles=PF.RdStr("General", "CfgFiles", "");

  CModelInfoUpgrade InfoU;
  flag FmtOK=true;
  flag OK = !AbortLoad && (sNewCfg()!=NULL);
  if (OK)
    {
    FmtOK=OK=(InfoU.CheckCfgFileFormat(sNewCfg(), true)>=0);
    if (!FmtOK)
      LogError("Project", 0, "Not Loaded : Old cfg format");
    }
  byte CfgRetCode = (OK ? Info.GetCfgInfo(sNewCfg(), true, true) : 99);
  if (CfgRetCode<2)
    OK=false;
  if (!AbortLoad && FmtOK && !OK)
    OK=Info.GetSetName(PrjFile(), false, PrjFileVerNo(), gs_License.LicCatagories());
  if (CfgRetCode==2)
    OK=false;

  if (OK)
    {
    SetCfgFile(Info.m_sCfgFile());
    SetCfgFiles(Info.m_sCfgFiles());

    CheckCfgFilesName();
    OK=InfoU.CheckDBFileFormat(CfgFiles());
    if (OK)
      Info.EnsureCfgIsInList();

    int Ret=TestCfgFiles();
    if (Ret&0x1)
      LogWarning("Project", 0, "Folder '%s' missing", BaseCfgFiles());
    if (Ret&0x2)
      LogWarning("Project", 0, "Folder '%s' missing", CfgFiles());

    Strng Test1(CfgFiles());
    Test1+="_SysCAD.mdb";
    Strng Test2(CfgFiles());
    Test2+=CfgDBFileName();

    if (FileExists(Test1()) && !FileExists(Test2()))
      if (!MoveFile(Test1(), Test2()))
        LogError("Project", 0, "%s not renamed to %s", Test1(), Test2());
    }

  return OK;
  }

//===========================================================================

flag CProject::CheckPrjCfgFile(CProfINIFile & PF, bool & Do_NotOpenMsg)
  {
  flag OK=true;
  CProfINIFile PrjCfg(CfgFile());
  if (stricmp(PrjCfg.RdStr("Modes", "Default_NetMode", "Nothing"), "Nothing")==0)
    {
    Strng S;
    S.Set("%s\n\n"
          "Is an Old Project Configuration File.\n"
          "Continue to Automatically update this file and Review the Settings ?\n", CfgFile());
    if (AfxMessageBox(S(), MB_ICONQUESTION|MB_YESNO)==IDYES)
      {

      Strng sRMd=PF.RdStr("Modes", "Initial_SolveMode",    "");
      bool IsProbal=sRMd.XStrNICmp("Probal", 6)==0;

      PrjCfg.WrStr("Modes", "Default_NetMode",        IsProbal?"ProBal":"Dynamic");
      
      PrjCfg.WrInt("Modes", "Probal_Allowed",      1);
      PrjCfg.WrStr("Modes", "Probal_NodeMode",     "Direct");
      PrjCfg.WrStr("Modes", "Probal_LinkMode",     "Direct");
      PrjCfg.WrStr("Modes", "Probal_FlowMode",     "Transfer");
      PrjCfg.WrStr("Modes", "Probal_HeatMode",     "Rigorous");

      PrjCfg.WrInt("Modes", "Dynamic_Allowed",     1);
      PrjCfg.WrStr("Modes", "Dynamic_NodeMode",    "Buffered");
      PrjCfg.WrStr("Modes", "Dynamic_LinkMode",    "Direct");
      PrjCfg.WrStr("Modes", "Dynamic_FlowMode",    "Simple");
      PrjCfg.WrStr("Modes", "Dynamic_HeatMode",    "Rigorous");

      PrjCfg.WrStr("Modes", "Maximum_NodeMode",    "Buffered");
      PrjCfg.WrStr("Modes", "Maximum_LinkMode",    "Direct");
      PrjCfg.WrStr("Modes", "Maximum_FlowMode",    "Full");
      PrjCfg.WrStr("Modes", "Maximum_HeatMode",    "Rigorous");

      //LogWarning(CfgFile(), LF_Exclamation, "Project Configuration file has been automatically upgraded for new Solution Modes");

      CMdlCfgSheet CfgEdit(gs_License.LicCatagories(), CfgFile(), "", AfxGetMainWnd(), 0);
      OK = (CfgEdit.pCfgPage!=NULL);
      if (OK)
        OK = (CfgEdit.DoModal()==IDOK);
      }
    else
      {
      OK=false;
      Do_NotOpenMsg=false;
      }
    //LogError("Project", LF_Exclamation, "Project Configuration file Must be Edited to Configure Solution Modes");
    //OK=false;
    }
  return OK;
  }

//===========================================================================

void WriteProjectSet()
  {
  Strng PrjSet(PrjFiles());
  PrjSet.FnClearEndBSlash();
  PrjSet.FnDrivePath();
  PrjSet.FnCheckEndBSlash();
  ScdPFUser.WrStr("ProjectSets", "MostRecent", PrjSet());
  ScdPFUser.WrStr("ProjectSets", CfgFile(), PrjSet());

  }

//===========================================================================

CString ReadProjectSet()
  {
  Strng PrjSet(CfgFiles());
  PrjSet.FnClearEndBSlash();
  PrjSet.FnDrivePath();
  PrjSet.FnCheckEndBSlash();
  return CString(ScdPFUser.RdStr("ProjectSets", CfgFile(), PrjSet()));
  }

//===========================================================================

void CProject::PutSettings()
  {
  dbgpln("--------------------- CProject::PutSettings");


  gs_Exec.SetDefNetMode    (m_NetMode );

  if (m_NetMode==NM_Probal)
    {
    SetDefNodeMode(m_PBNodeMode);
    SetDefLinkMode(m_PBLinkMode);
    SetDefFlowMode(m_PBFlowMode);
    SetDefHeatMode(m_PBHeatMode);
    }
  else
    {
    SetDefNodeMode(m_DynNodeMode);
    SetDefLinkMode(m_DynLinkMode);
    SetDefFlowMode(m_DynFlowMode);
    SetDefHeatMode(m_DynHeatMode);
    }

  SetMaxNodeMode(m_MaxNodeMode);
  SetMaxLinkMode(m_MaxLinkMode);
  SetMaxFlowMode(m_MaxFlowMode);
  SetMaxHeatMode(m_MaxHeatMode);

  gs_Exec.SetTheTime(m_TheTime, "PutSettings");
  gs_Exec.StepSizeMax    = m_StepSizeMax;
  gs_Exec.ScnStopTime    = m_ScnStopTime;
  gs_Exec.ScnDuration    = m_ScnDuration;

  gs_Exec.SetRealTime(m_RealTime != 0);
  gs_Exec.SetRealTimeMult(m_RealTimeMult);
  gs_Exec.SetSyncWithClock(m_SyncWithClock!= 0);
  gs_Exec.SetScnType((eScnTypes)m_ScnType, false);
  gs_Exec.ScnTermAction  = (eScnTermActions)m_ScnTermAction;
  gs_Exec.StepCountMax   = m_StepCountMax;

  SetDisplayTagsOnly(m_DisplayTagsOnly != 0);
  SetHideZeros(m_HideZeros != 0);
  SVI.SetShowGroups(m_ShowGroups != 0);
  TaggedObject::NumericTagsBad = m_NumericTagsBad;
  TaggedObject::NumericStartingTagsBad = m_NumericStartingTagsBad != 0;
  TaggedObject::NonNumericTagChr = m_NonNumericTagChr;


  CTagVwDoc::GTB.StartTime    = m_GTBStartTime;
  CTagVwDoc::GTB.EndTime      = m_GTBEndTime;
  CTagVwDoc::GTB.TrackingTime = m_GTBTrackingTime;

  m_pFlwLib->FE_SetSpModelFixupStrategy(m_SpModelFixupStrategy);

  //gs_HstMngr.SetOnAsRqd();
  }


//===========================================================================

void CProject::GetSettings()
  {
  dbgpln("--------------------- CProject::GetSettings");

  m_NetMode    = DefNetMode();

  if (m_NetMode==NM_Probal)
    {
    m_PBNodeMode   = DefNodeSolveMode();
    m_PBLinkMode   = DefLinkSolveMode();
    m_PBFlowMode   = DefFlowMode();
    m_PBHeatMode   = DefHeatMode();
    }
  else
    {
    m_DynNodeMode  = DefNodeSolveMode();
    m_DynLinkMode  = DefLinkSolveMode();
    m_DynFlowMode  = DefFlowMode();
    m_DynHeatMode  = DefHeatMode();
    }

  m_MaxNodeMode   = MaxNodeMode();
  m_MaxLinkMode   = MaxLinkMode();
  m_MaxFlowMode   = MaxFlowMode();
  m_MaxHeatMode   = MaxHeatMode();

  m_TheTime       = gs_Exec.TheTime;
  m_StepSizeMax   = gs_Exec.StepSizeMax;
  m_ScnStopTime   = gs_Exec.ScnStopTime; 
  m_ScnDuration   = gs_Exec.ScnDuration; 

  m_RealTime      = gs_Exec.RealTime();
  m_RealTimeMult  = gs_Exec.RealTimeMult();
  m_SyncWithClock = gs_Exec.SyncWithClock();
  m_ScnType       = gs_Exec.ScnType;
  m_ScnTermAction = gs_Exec.ScnTermAction;
  m_StepCountMax  = gs_Exec.StepCountMax;

  m_DisplayTagsOnly         = GetDisplayTagsOnly();
  m_HideZeros               = GetHideZeros();
  m_ShowGroups              = SVI.GetShowGroups();
  m_NumericTagsBad          = TaggedObject::NumericTagsBad ;
  m_NumericStartingTagsBad  = TaggedObject::NumericStartingTagsBad;
  m_NonNumericTagChr        = TaggedObject::NonNumericTagChr;

  CTagVwDoc::GTB.StartTime    = m_GTBStartTime;
  CTagVwDoc::GTB.EndTime      = m_GTBEndTime;
  CTagVwDoc::GTB.TrackingTime = m_GTBTrackingTime;

  m_SpModelFixupStrategy      = m_pFlwLib->FE_SpModelFixupStrategy();

  //gs_HstMngr.GetOnRqd();
  }

//===========================================================================

flag CProject::DoOpen(pchar pPrjPath, pchar pNewModelCfg, PrjLoadTypes PrjLoadType)
  {
  dbgpln("--------------------- CProject::DoOpen");

  bool Do_NotOpenMsg=true;
  if (1)
    {
    Strng TmpContractStr;
    Strng NewModelCfg=pNewModelCfg;

    SetUsingPrjLclFiles(PrjLoadType==PLT_Local);
    //m_LoadFromBackupRqd=false;

    FnSetTestLocalFiles(true);
    FnLocalFilesDifferentClear();
    m_pFlwLib = NULL;
    CWaitMsgCursor Wait("Opening project");
    bCmdSuccess = true;
    bDoingLoad = 1;

    pPrjPath = ChkPrjFileName(pPrjPath);
    if (!pPrjPath)
      goto DoneFailed;

    BOOL DoCheckTags=FALSE;
    //flag OK=false;

    // Test Position of SPJ File
    Strng Td, SPrjFiles, SPrjFile, E;
    Td.FnDrivePathName(pPrjPath);
    DWORD Att=GetFileAttributes(Td());
    flag OldStyle=(Att!=0xFFFFFFFF && ((Att&FILE_ATTRIBUTE_DIRECTORY)!=0));
    Td+="\\TagReg.CSV";
    OldStyle=OldStyle && FileExists(Td());

    if (OldStyle)
      {
      LogError(pPrjPath, 0, "Invalid Project Format");
      goto DoneFailed;
      }
    else
      {
      SPrjFile=pPrjPath;
      SPrjFiles.FnDrivePath(pPrjPath);
      }

    CStopWatch SW;
    CString sSWTmp;
    SW.Start();
    Wait.UpdateMsg("Opening project %s", SPrjFile());//FnContract(TmpContractStr, PrjName()));

    bChangedGrfMenu=0;
    bChangedRuntimeMenu=0;

    // Modify PrjFileName if Neccessary
    Strng T;
    T.FnNameExt(SPrjFile());
    if (T.XStrICmp("Project.spj")!=0)
      {
      LogError(pPrjPath, 0, "Invalid Project File Name");
      goto DoneFailed;
      }

    SetPrjFiles(SPrjFiles(), SPrjFile());
    bLoadBusy=1;

    dbgDumpFilesAliases();

    //Try rename layout file;
    Strng OldLayout(PrjFiles());
    OldLayout+="layout.ini";
    if (FileExists(OldLayout()))
      {
      Strng NewLayout(PrjFiles());
      NewLayout+=LayoutFileName;
      rename(OldLayout(), NewLayout());
      }

    ASSERT(_CrtCheckMemory());

    FILE* f = NULL;
    if (GetFileAttributes(PrjFile()) & FILE_ATTRIBUTE_READONLY)
      {
      char Buff[1024];
      sprintf(Buff,"WARNING: Project file is read-only!\n\n"
        "The read-only file attribute should NOT"
        "be set for any files in a project!\n\n"
        "%s\n\n"
        "Continue with project load?", PrjFile());
      if (AfxMessageBox(Buff, MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2)!=IDYES)
        {
        Do_NotOpenMsg =  false;
        goto DoneFailed;
        }
      }
    f = fopen(PrjFile(), "rt");
    if (f==NULL)
      {
      char Buff[1024];
      sprintf(Buff,"WARNING: Project file cannot be read!\n\n"
        "%s\n\n", PrjFile());
      AfxMessageBox(Buff, MB_ICONEXCLAMATION|MB_OK);
      goto DoneFailed;
      }

    //project exists

    char Buff[1024];
    dbgMemoryState("Project Load Start");
    fread(Buff, 1, sizeof(Buff), f);
    Buff[sizeof(Buff)-1] = 0;
    fclose(f);
    if (strstr(Buff, "Syscad Project File"))
      {
      AfxMessageBox("Old style project incompatible!");
      DoNew(false);
      goto DoneFailed;
      }

    // Project Paths etc
    //{
    //Strng s;
    AfxGetMainWnd()->UpdateWindow();

    CProfINIFile PF(SPrjFile());

    gs_CnvsMngr.Recover();

    CModelInfo Info;
    if (!SetPrjCfgFile(PF, NewModelCfg, Info))
      goto DoneFailed;

    dbgDumpFilesAliases();
    //>>>>>>>>>>>>>>>>>>>>>>>>>>>

    if (!CheckPrjCfgFile(PF, Do_NotOpenMsg))
      goto DoneFailed;

    SetOrigPrjFileVerNo(PF.RdInt("General", "PrjFileVersionOriginal", -1));
    SetPrjFileVerNo(PF.RdInt("General", "PrjFileVersion", 0));


    m_OldPrjName = PF.RdStr("General", "PrjName", "");
    m_OldPrjFiles = PF.RdStr("General", "PrjFiles", "");

    if (m_OldPrjFiles.GetLength()==0) // for older versions
      {
      m_OldPrjFiles = PF.RdStr("General", "PrjRootDirectory", "");
      m_OldPrjFiles.FnCheckEndBSlash();
      m_OldPrjFiles += m_OldPrjName;
      }

    int PLFStatePrv=PF.RdInt("General", "LocalPrjFiles", false);
    if (PLFStatePrv && !UsingPrjLclFiles())
      {
      //case PLF_None: break;
      //case PLF_Opening: //break;
      //case PLF_Using: PLFState= PLF_Using; break;
      LogWarning("Project", LF_Exclamation, "This was saved as a Local project");
      SetUsingPrjLclFiles(true);
      }

#if WithRmtFile
    flag RmtOK=true;
    if (UsingPrjLclFiles())
      RmtOK=SetupRmtCopies();
#endif

    ReadSettings(PF, PrjFileVerNo());

    bool IsOldProject=(PrjFileVerNo() < 100);
    if (IsOldProject)
      {
      Strng S;
      S.Set("%s\n\n"
            "Is an old Project and needs an Upgrade.\n"
            "Continue and Review Project Settings ?", PrjFiles());
      if (AfxMessageBox(S(), MB_ICONQUESTION|MB_YESNO)!=IDYES)
        {
        Do_NotOpenMsg =  false;
        goto DoneFailed;
        }
      else if (!EditSettings("Project Settings Review", NULL))
        {
        Do_NotOpenMsg =  false;
        goto DoneFailed;
        }
      }

    CDlgBusy::Open("\nOpening Project");
    CDlgBusy::SetLine(3, PrjName());

    RestoreMainWindow();
#if WithRmtFile
    //if (OK && RmtOK)
    if (RmtOK)
#else
    //if (OK)
    if (1)
#endif
      {
      Strng FlwLibTag, EO_LocationTag;
      FlwLibTag=PlantModelTag;
      EO_LocationTag=PlantModelTag;

      m_pFlwLib=NULL;
      if (!MdlLibs.LoadFlwDLLs(Info.m_sDLLList, FlwLibTag(), EO_LocationTag(), &m_FlwLib))
        goto DoneFailed;

      m_pFlwLib=&m_FlwLib;

      //if (IsOldProject)
      PutSettings();

      SetTagMonitor();

      //AfxGetMainWnd()->UpdateWindow();
      LoadHelpFileList();

      CRptTagLists::StartUp();

#if WITHNETSERVER
      OpenNetManager();
      s = PF.RdStr("NetWorkManager", "ClientServer", "");
      pCS_Mngr->SetServer(PF.RdLong("NetWorkManager", "ServerEnabled", pCS_Mngr->ServerEnabled()) !=0);
      gs_Exec.SetCoupling((ExecCoupling)PF.RdLong("NetWorkManager", "Coupling",(long)gs_Exec.Coupling()));
      pCS_Mngr->SetClientConnect(PF.RdLong("NetWorkManager", "ClientConnect", pCS_Mngr->ClientConnected()) != 0,
        s() ? s() : "", gs_Exec.Coupling()==XC_Sync);
#endif

#if WITHSCRCYCLES
      int i;
      //get screen saver info...
      bSystemScreenSave = (PF.RdLong("ScreenSaver", "SystemOn", bSystemScreenSave) != 0);
      bSyscadScreenSave = (PF.RdLong("ScreenSaver", "SysCADOn", bSyscadScreenSave) != 0);
      iScrCycles = PF.RdInt("ScreenSaver", "NoOfCycles", iScrCycles);
      if (Cycles)
        delete []Cycles;
      Cycles = new CScreenSaverInfo[iScrCycles];
      for (i=0; i<iScrCycles; i++)
        {
        char Section[256];
        char Item[256];
        sprintf(Section, "Cycle%i", i);
        Cycles[i].iWaitTime = (WORD)PF.RdLong(Section, "WaitTime", 10);
        s = PF.RdStr(Section, "Description", Section);
        Cycles[i].sDescription = s();
        int WndCnt = PF.RdInt(Section, "WindowCount", 0);
        Cycles[i].Windows.SetSize(WndCnt);
        for (int j=0; j<WndCnt; j++)
          {
          sprintf(Item, "WindowTitle%i", j);
          s = PF.RdStr(Section, Item, "Command");
          Cycles[i].Windows.SetAt(j, s());
          }
        }
#endif

      for (int i=0; i<16; i++)
        {
        char Item[32];
        sprintf(Item, "Colour%d", i);
        gs_CustomColours[i] = PF.RdLong("CustomColours", Item, gs_CustomColours[i]);
        }

      //gs_HstMngr.SetOnAsRqd();
      gs_HstMngr.SetLicensingOptions(gs_License.MaxHistSizeAllowed(), gs_License.MaxHistFilesAllowed());

      OpenArcManager(true);

      OpenHistorian();
      dbgMemoryState("Historian");


#if WITHDRVMAN
      //get driver info...
      sDrvManagerName = PF.RdStr("DrvManager", "Name", sDrvManagerName());
      if (sDrvManagerName.Find('.')<0)
        sDrvManagerName += ".SCF";
      sDrvManagerName.FnContract();

      bDrvOn = (PF.RdLong("DrvManager", "On", bDrvOn) != 0);
      bDrvLclTagSrvrOK= (PF.RdLong("DrvManager", "LclTagSrvrOK", bDrvLclTagSrvrOK) != 0);
      bDrvReadAll = (PF.RdLong("DrvManager", "ReadAll", bDrvReadAll) != 0);
      bDrvIgnoresOn = (PF.RdLong("DrvManager", "IgnoresOn", bDrvIgnoresOn) != 0);
      bDrvForcesOn = (PF.RdLong("DrvManager", "ForcesOn", bDrvForcesOn) != 0);
      bDrvNoiseOn = (PF.RdLong("DrvManager", "NoiseOn", bDrvNoiseOn) != 0);
      bDrvStartFBKCheck = (byte)PF.RdLong("DrvManager", "StartFBKCheck ", FBKCheckNever);
#endif

      LoadColours(PF);

      OpenDDEManager();

      LoadMainOPCOptions(PF);
      if (!DoLoadDB(PrjFiles(), m_OldPrjName()))
        goto DoneFailed;
      else
        {

        ASSERT(_CrtCheckMemory());

        dbgMemoryState("LoadDatabase");

        // Moved to Callback
        //OpenDrvManager(); //must be loaded after models (tag data base)
        //dbgMemoryState("DriveMan");

        //load documents...
        CDlgBusy::Open("\nOpening Documents");
        for (int j=iGraphTemplate; j<=iControlTemplate; j++)
          {
          if (ScdApp()->TemplateExists(j))
            {
            //Strng Section = (j==0 ? "GraphicsDocuments" : "TrendDocuments");
            //Strng DocTypeName = (j==0 ? "Graphics" : "Trend");
            Strng DocTypeName=DocumentTmplNames[j];

            CDocTemplate & Templ = ScdApp()->Template(j);
            const int DocCount = PF.RdInt(DocumentSectNames[j], "DocumentCount", 0);
            char Entry[256];
            for (i=0; i<DocCount; i++)
              {
              Strng sDoc;
              sprintf(Entry, "Doc%i", i);
              sDoc = PF.RdStr(DocumentSectNames[j], Entry, "");
              sDoc.FnExpand();

              WIN32_FIND_DATA fd;
              HANDLE fh = FindFirstFile(sDoc(), &fd);
              if (fh!=INVALID_HANDLE_VALUE)
                {
                FindClose(fh);
                CDlgBusy::SetLine(1, "Opening %s Document %d of %d:\n\n%s", DocTypeName(), i+1, DocCount, TmpContractStr.FnNameExt(sDoc()));
                //ScdApp()->OpenDocumentFile(sDoc());
                Templ.OpenDocumentFile(sDoc());
                dbgMemoryState(sDoc());
                XFlushAppMsgQ();
                //AfxGetMainWnd()->UpdateWindow();
                }
              else
                LogError("Project", LF_Exclamation, "%s document %s not found", DocTypeName(), sDoc());
              }
            }
          }

        gs_AccessWnds.LoadNdHist(-1);

        ASSERT(_CrtCheckMemory());

        if (PrjFileVerNo()<30)
          m_bRptExcelMakeActive = 1; //change this option

        EnableMainOPCSrvr(m_bOPCOn!=0);

        if (gs_pPrj->m_pFlwLib)
          gs_pPrj->m_pFlwLib->FE_SetSpModelFixupStrategy(PF.RdLong("SpModel", "FixupStrategy", FUS_Largest));

        if (CExploreScd::UseScdExplorer() || PF.RdInt("Explorer", "Open", 0))
          CExploreScd::OpenIt();
        else
          CExploreScd::CloseIt();

        // Data from Ini File

        CProfINIFile PIni(PrjIniFile());
        m_eSSAction = (SnapShotActions)PIni.RdLong("SnapShot", "Action", m_eSSAction);
        m_sSSName = PIni.RdStr("SnapShot", "Name", m_sSSName());
        m_sSSPrefix = PIni.RdStr("SnapShot", "Prefix", m_sSSPrefix());
        m_iSSNumber = (UINT)PIni.RdLong("SnapShot", "Number", m_iSSNumber);
        m_bSSDoZip = (PIni.RdLong("SnapShot", "DoZip", m_bSSDoZip) != 0);

        if (CfgFile())
          ScdPFUser.WrStr("General", "LastCfgFile", CfgFile());
        bLoadBusy=0;

        ASSERT(_CrtCheckMemory());

        //load is finished, get trends to reconnect and fetch trend lines etc...
        CDlgBusy::Open("\nConnecting trends");
        for (int iTemp=iTrendTemplate; iTemp<=iControlTemplate; iTemp++)
          {
          if (ScdApp()->TemplateExists(iTemp))
            {
            POSITION Pos = ScdApp()->Template(iTemp).GetFirstDocPosition();
            while (Pos)
              {
              CTagVwDoc* pTrndDoc = (CTagVwDoc*)(ScdApp()->Template(iTemp).GetNextDoc(Pos));
              CDlgBusy::SetLine(3, FnContract(TmpContractStr, (char*)(const char*)(pTrndDoc->GetTitle())));
              pTrndDoc->OnActivate(true);  // force reconnect etc
              AfxGetMainWnd()->UpdateWindow();
              XFlushAppMsgQ();
              }
            }
          }
        //AfxGetMainWnd()->UpdateWindow();
        CDlgBusy::Close();

        ASSERT(_CrtCheckMemory());

        SW.Stop();

        LogNote("Project", 0, "Load time %s", SW.MinSecDesc(sSWTmp));
        if (m_bReadOnlyPrj)
          LogNote("Project", 0, "Marked as read-only");
        ::AfxTrace(_T("Project loaded: '%s'\n"), _T(PrjName()));


        CheckLicenseConditions();
        CDlgBusy::Close();
        MainWnd()->PostMessage(WMU_UPDATEMAINWND, SUB_UPDMAIN_PRJLOADED); //notify main window that project load is complete
        MainWnd()->PostMessage(WMU_UPDATEMAINWND, SUB_UPDMAIN_UPDATE/*, (LPARAM)pMsgFrame*/); //get main window thread to top AND make message window active top window
        CMsgWindow::Show(false);

        CMainFrame* pM = (CMainFrame*)AfxGetMainWnd();
        pM->SetToolBarSolveMode();


        bDoingLoad = 0;
        bCmdSuccess = true;

        if (DoCheckTags)
          {
          int TagsChngd = gs_Exec.CheckAllTags(TaggedObject::NumericTagsBad, TaggedObject::NumericStartingTagsBad);
          if (TagsChngd<0)
            {
            TaggedObject::NumericTagsBad = 0;
            TaggedObject::NumericStartingTagsBad = 0;
            }
          else if (TagsChngd>0)
            {
            CWaitCursor Wait;
            CloseIOMarshal();
            DisConnectArcManager();
#if WITHDRVMAN
            CloseDrvManager();
            OpenDrvManager();
#endif
            ConnectArcManager();
            OpenIOMarshal();
            CTagVwDoc::RebuildAll();
            }
          }

        ASSERT(_CrtCheckMemory());

        FnSetTestLocalFiles(false);
        long N=FnLocalFilesDifferentCount();
        Strng_List &L=FnLocalFilesDifferentList();
        if (N>0)
          {
          Strng *p;
          for (p=L.First(); p; p=p->Next())
            {
            Strng S="?";
            if (p->Index()&LCF_RmtChanged)
              S="File original changed";
            if (p->Index()&LCF_LclChanged)
              S="File local copy changed";
            if (p->Index()&LCF_RmtMissing)
              S="File original missing";
            if (p->Index()&LCF_LclMissing)
              S="File local copy missing";
            LogNote("Project", 0, "%s : %s", S(), p->Str());
            }
          LogWarning("Project", LF_Exclamation, N==1 ? "%i File changed or missing":"%i Files changed or missing", N);
          FnLocalFilesDifferentClear();
          }

        WriteProjectSet();

        // Old version now loaded - Remember Old Version
        SetPrjFileVerNoAsLoaded(PrjFileVerNo());
        // Set version to latest
        SetPrjFileVerNo(LatestPrjFileVer);
        }
      }


#if CREATENETSERVER
    if (01)//pkh && OK)
      {
      m_pCLRSrvr = new CNETServer;
      m_pCLRSrvr->Startup();
      }
    else
      m_pCLRSrvr = NULL;
#endif

    gs_ProjectOpenFlag++;


    CDlgBusy::Close();
    }

  ScdMainWnd()->PostMessage(WMU_UPDATEMAINWND, SUB_UPDMAIN_BACKGROUND, 0);
  return true;

DoneFailed:

  CloseRmtCpy();

  if (Do_NotOpenMsg && PrjFiles())
    {
    char buff[256];
    sprintf(buff, "Project %s not Opened", PrjFiles());
    AfxMessageBox(buff, MB_ICONEXCLAMATION|MB_OK);
    }

  DoNew(false);
  bLoadBusy=0;

  SetPrjFileVerNoAsLoaded(LatestPrjFileVer);
  SetPrjFileVerNo(LatestPrjFileVer);

  if (CDlgBusy::IsOpen())
    CDlgBusy::Close();

  ((CMainFrame*)AfxGetMainWnd())->UpdateMainToolBar();

  ScdMainWnd()->PostMessage(WMU_UPDATEMAINWND, SUB_UPDMAIN_BACKGROUND, 0);
  return false;
  }

//---------------------------------------------------------------------------

flag CProject::DoPrjPaths(pchar pPrjPath, flag IsNew, CPrjFilesLists* pPFL, BOOL WithVersion, BOOL RemoveOld)
  {
  m_Save.m_bRqstNew=IsNew;

  Strng TmpContractStr;
  Strng NewPrjFile(PrjFile());

  //do not allow project to be saved while main SysCAD window is minimised!
  if (AfxGetMainWnd()->IsIconic())
    AfxGetMainWnd()->ShowWindow(SW_RESTORE);

  //if (PrjFileVerNoAsLoaded()<PrjFileVerNo())
  if ((PrjFileVerNoAsLoaded()<17 && PrjFileVerNo()>=17) ||
    (PrjFileVerNoAsLoaded()<45 && PrjFileVerNo()>=45) ||
    (PrjFileVerNoAsLoaded()<63 && PrjFileVerNo()>=63) ||
    (PrjFileVerNoAsLoaded()<68 && PrjFileVerNo()>=68) ||
    (PrjFileVerNoAsLoaded()<69 && PrjFileVerNo()>=69) ||
    (PrjFileVerNoAsLoaded()<73 && PrjFileVerNo()>=73) ||
    (PrjFileVerNoAsLoaded()<76 && PrjFileVerNo()>=76) /* TOElementTagDigitCnt() Changed*/) 
    if (!bDoingSaveAs && !bDoingSaveAsNV)
      {
      LogWarning("Project", LF_Exclamation, "File format change:\n\nSaveAs or Save Version required");
      bCmdSuccess = false;
      return BadSave();
      }

    if (pPrjPath!=NULL || PrjFile()==NULL || (bDoingSaveAs && !bDoingSaveAsNV))
      {
      Strng HDir(ReadProjectSet());

      if (pPrjPath==NULL || strstr(pPrjPath, "*")!=0 ||
        strlen(pPrjPath)==0 || (PrjFile()==NULL && !IsNew) || bDoingSaveAs)
        {//new project or save as...
        DWORD dwFlags = /*OFN_OVERWRITEPROMPT |*/ OFN_CREATEPROMPT |
          OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_SHAREAWARE;

        Strng NewPrjFolder;
        Strng DlgFileName("*.spj");

        for (;;)
          {
          CSCDFileDialog Dlg(false, "spj", DlgFileName(), dwFlags);//, "SysCAD project (*.spj)|*.spj||");
          Dlg.m_ofn.lpstrInitialDir = HDir();
          Dlg.m_ofn.lpstrTitle = IsNew ? "Create as" : "Save as";
          Dlg.m_DoingPrjOpen=true;
          if (Dlg.DoModal()==IDOK)
            {
            NewPrjFolder = Dlg.GetPathName();
            DlgFileName.FnNameExt(NewPrjFolder());
            }
          else
            {
            bCmdSuccess = false;
            return BadSave();
            }

          // Test Location
          Strng TstFoldPrv, TstPrj, TstPrjPrv, TstExtPrv, TstTmp(Dlg.GetPathName());
          TstPrj.FnName(TstTmp());
          TstPrj.FnCheckExtension("spf");
          TstFoldPrv.FnDrivePath(TstTmp());
          TstFoldPrv.FnClearEndBSlash();
          TstPrjPrv.FnNameExt(TstFoldPrv());
          TstExtPrv.FnExt(TstFoldPrv());
          flag InAnother=true;
          if (TstExtPrv.XStrICmp(".spf")!=0)
            InAnother=false;
          else
            {
            Strng TstNamePrv;
            TstNamePrv.FnName(TstFoldPrv());
            TstNamePrv.FnCheckExtension("spj");
            Strng Tst(TstFoldPrv);
            Tst.FnCheckEndBSlash();
            Tst+=TstNamePrv();
            if (!FileExists(Tst()))
              InAnother=false;
            }
          if (InAnother)
            {
            Strng S;
            S.Set("Set Project %s within %s", TstPrj(), TstPrjPrv());
            if (AfxMessageBox(S(), MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2)==IDYES)
              break;
            }
          else
            break;
          }

        // NewPrjFolder Extension will be "spj"
        // Change to spf
        NewPrjFolder.FnDrivePathName();
        NewPrjFolder.FnCheckExtension("spf");
        Strng Name;
        Name.FnName(NewPrjFolder());
        NewPrjFile=NewPrjFolder;
        NewPrjFile.FnCheckEndBSlash();
        NewPrjFile+="Project";
        NewPrjFile.FnCheckExtension("spj");

        }
      else
        {
        NewPrjFile = pPrjPath;

        // Check that path exists
        if (NewPrjFile.XStrPBrk(":\\")==0)
          {
          Strng N;
          N.FnName(pPrjPath);
          NewPrjFile=HDir;
          NewPrjFile+=N;
          NewPrjFile.FnCheckExtension("spf");
          NewPrjFile.FnCheckEndBSlash();
          NewPrjFile+="Project";
          NewPrjFile.FnCheckExtension("spj");
          NewPrjFile.FnCheckEndBSlash();
          }
        }
      }

    Strng Folder;
    Strng Name;
    Strng ThisVers;
    Strng CommentStr;
    Strng NamePt1;
    Strng NamePt2;
    int ThisNo=-1;
    int NewNo=0;
    int NewWithAdornment=1;
    if (bDoingSaveAs || bDoingSaveAsNV || IsNew && NewWithAdornment)
      {
      //    NewPrjFile
      Folder.FnDrivePath(NewPrjFile());
      Folder.FnClearEndBSlash();
      Folder.FnCheckExtension("spf");
      Name.FnName(Folder());

      int NumLen=0;

      char *pVersionStart=Name.XStrRChr(VersionChar);
      if (pVersionStart && Name.GetLength()>0)
        {
        //pVersionStart+=1;
        ThisVers=pVersionStart+1;
        NumLen=ThisVers.Length();
        if (NumLen>=2 && NumLen<=4)
          {
          flag IsNum=true;
          for (int i=0; i<NumLen; i++)
            if (!isdigit(ThisVers[i]))
              IsNum=false;
          if (IsNum)
            {
            ThisNo=SafeAtoL(ThisVers(),0);
            NewNo=((IsNew && !bDoingSaveAsNV) ? ThisNo : ThisNo+1);
            Name.SetLength(pVersionStart-Name());
            }
          }
        }

      NamePt1=Name;
      NamePt2="";

      char * pCommentEnd=NamePt1.XStrRChr(ComEndChar);
      if (pCommentEnd)
        {
        int iCommentEnd=pCommentEnd-NamePt1();
        NamePt2=NamePt1.Right(NamePt1.Length()-iCommentEnd-1);
        NamePt1.SetLength(iCommentEnd+1);
        char*pCommentStart=NamePt1.XStrRChr(ComStartChar);
        if (pCommentStart && (pCommentStart-NamePt1())>0)
          {
          CommentStr=pCommentStart+1;// Skip (
          CommentStr.SetLength(CommentStr.GetLength()-1);// Strip )
          NamePt1.SetLength(pCommentStart-NamePt1()); // Before (
          }
        }
      else // Check for SubVersion
        {
        char * pFirstVersStart=NamePt1.XStrChr(VersionChar);
        if (pFirstVersStart)
          {
          NamePt2=pFirstVersStart;
          NamePt1.SetLength(pFirstVersStart-NamePt1());
          }
        }
      }

    if (bDoingSaveAs || bDoingSaveAsNV || IsNew)
      {
      CPrjSaveAsDlg  Dlg(m_sPrevPrjFiles(), IsNew);
      Dlg.m_CopyHist = true;
      Dlg.m_VerStr=CommentStr() ? CommentStr() : "";
      int DefMDBF = ScdPFUser.RdInt("General", "DefaultAccessFormat", 0);
      Dlg.m_DbFmt=XDBFormat();//==DBO_CSV ? 2 : DefMDBF==0 ? 0 : 1);

      Dlg.m_RemovePrevRevs=m_bRemoveRevs;
      Dlg.m_CopyDBFromOld=m_bCopyDBFiles;

      Dlg.m_DBOn=true;
      Dlg.m_CopyDBOn=false;//ALWAYS default to false!!! (PrjFileVerNoAsLoaded()==PrjFileVerNo());
      Dlg.m_CopyOn=(bDoingSaveAs || bDoingSaveAsNV);

      if (Dlg.m_CopyOn)
        {
        pPFL->CheckUnknown();
        for (int i=0; i<6; i++)
          Dlg.m_CopyCnt[i] = pPFL->OtherFiles[i].GetSize();
        }
      if (IsNew || Dlg.DoModal()==IDOK)
        {
        if (CommentStr.XStrICmp((LPCTSTR)Dlg.m_VerStr)!=0)
          NewNo=1;
        CommentStr=Dlg.m_VerStr;
        CommentStr.LRTrim();
        memmove(m_DlgCopy, Dlg.m_Copy, sizeof(m_DlgCopy));
        m_DlgCopyHist=Dlg.m_CopyHist;
        XSetDBFormat(Dlg.m_DbFmt);//==0 ? DBO_MDB97 : Dlg.m_DbFmt==1 ? DBO_MDB2000 : DBO_CSV);
        m_bRemoveRevs=Dlg.m_RemovePrevRevs;
        if (Dlg.m_CopyDBOn)
          m_bCopyDBFiles=Dlg.m_CopyDBFromOld;
        }
      else
        {
        bCmdSuccess = false;
        return BadSave();
        }
      }

    if (bDoingSaveAs || bDoingSaveAsNV || IsNew && NewWithAdornment)
      {
      Strng BaseFolder, NewName, CommStr;
      BaseFolder.FnDrivePath(Folder());

      if (CommentStr())
        CommStr.Set("%c%s%c", ComStartChar, CommentStr(), ComEndChar);
      else
        CommStr="";

      BOOL ForceName=!WithVersion;
      BOOL RemoveIt=RemoveOld;
      Name=NamePt1;
      Name+=CommStr;
      Name+=NamePt2;
      Strng Vers0;
      if (ThisNo>=0)
        Vers0.Set("%c%02i", VersionChar, ThisNo);
      if (bDoingSaveAsNV || IsNew && NewWithAdornment)
        {
        Strng NewVers;
        NewVers.Set("%c%02i", VersionChar, NewNo);

        NewName=Name;
        if (ForceName)
          {
          NewPrjFile=BaseFolder;
          NewPrjFile+=NewName;
          NewPrjFile.FnCheckExtension("spf");
          if (FileExists(NewPrjFile()))
            {
            if (!RmtCmdBusy() && !RemoveIt)
              {
              Strng T;
              T.Set("Project '%s' currently exists\n\nRemove It", NewPrjFile());
              if (AfxMessageBox(T(), MB_YESNO|MB_ICONQUESTION)==IDYES)
                RemoveIt=true;
              else
                {
                bCmdSuccess = false;
                return BadSave();
                }
              }

            if (RemoveIt)
              {
              if (!ClearFolder(NewPrjFile(), true))
                {
                LogError("Project", 0, "Project '%s' not removed", NewPrjFile());
                bCmdSuccess = false;
                return BadSave();
                }
              }
            else
              {
              LogError("Project", 0, "Project '%s' currently exists", NewPrjFile());
              bCmdSuccess = false;
              return BadSave();
              }
            }
          }
        else
          {
          NewName+=NewVers;
          // Check does not exist
          for (int Pass=0;;Pass++)
            {
            NewPrjFile=BaseFolder;
            NewPrjFile+=NewName;
            NewPrjFile.FnCheckExtension("spf");
            if (!FileExists(NewPrjFile()))
              break;
            if (1)
              {
              // Bump version # up and try again
              NewNo++;
              NewVers.Set("%c%02i", VersionChar, NewNo);
              NewName=Name;
              NewName+=NewVers;
              }
            else
              {
              // Append another version #
              NewVers=VersionChar;
              NewVers+="00";
              if (Pass==0)
                {
                NewName=Name;
                NewName+=Vers0;
                }
              NewName+=NewVers;
              }
            }
          }
        }
      else
        {
        NewName=Name;
        NewName+=Vers0;
        NewPrjFile=BaseFolder;
        NewPrjFile+=NewName;
        NewPrjFile.FnCheckExtension("spf");
        }

      NewPrjFile.FnCheckEndBSlash();
      NewPrjFile+="Project";
      NewPrjFile.FnCheckExtension("spj");
      }
    else if (!IsNew)
      XSetDBFormat(DBConnect_Find);

    //  bReadOnlyPrj = 0;

    //sPrjFile=NewPrjFile;

    Strng NewPrjFolder;
    NewPrjFolder.FnDrivePath(NewPrjFile());
    NewPrjFolder.FnClearEndBSlash();
    NewPrjFolder.FnCheckExtension("spf");
    Strng NewPrjFolderName(NewPrjFolder);
    NewPrjFolder.FnCheckEndBSlash();

    //flag NewPrj = (PrjFile()==NULL || (_stricmp(PrjFile(), NewPrjFile())!=0));
    m_Save.m_bIsNewPrj = (PrjFile()==NULL || (_stricmp(PrjFile(), NewPrjFile())!=0));
    Strng OrigPrjFiles=PrjFiles();
    Strng OrigPrjFile=PrjFile();

    flag DoCreateErrMsg=true;
    if (bDoingSaveAs && FileExists(NewPrjFolderName()))
      {
      if (NewPrjFolder.XStrICmp(PrjFiles())==0)
        {
        LogError("Project", 0, "Project '%s' currently open - Use Save", NewPrjFolder());
        bCmdSuccess = false;
        return BadSave();
        }
      Strng T;
      T.Set("Overwrite '%s*.*'?\n\nNote: All original contents will be lost!", NewPrjFolder());
      if (AfxMessageBox(T(), MB_YESNO|MB_ICONQUESTION)==IDNO)
        {
        bCmdSuccess = false;
        return BadSave();
        }
      else
        {
        //SetPrjFiles();//NewPrjFolder(), NewPrjFile());
        //SetPrjFiles(OrigPrjFiles(), OrigPrjFile());
        SetPrjRmtCpyFiles(NULL);
        DoCreateErrMsg = ClearFolder(NewPrjFolderName(), true);
        if (!DoCreateErrMsg)
          LogNote("SysCAD", 0, "Deleted contents and folder '%s'", NewPrjFolderName());
        }
      }

    if (m_Save.m_bIsNewPrj || !FileExists(NewPrjFolderName()))
      {
      if (!CreateDirectory(NewPrjFolderName(), NULL))
        if (DoCreateErrMsg)
          {
          Strng E;
          E.GetSystemErrorStr(GetLastError());
          LogError("SysCAD", LF_Exclamation,
            "Project folder not created :\n%s\n%s", NewPrjFolderName(), E());
          SetPrjFiles(OrigPrjFiles(), OrigPrjFile());
          return BadSave();
          }
        SetPrjFiles(NewPrjFolder(), NewPrjFile());
        gs_HstMngr.sHstDataDir = PrjFilesAlias();
        gs_HstMngr.sHstCatDir = PrjFilesAlias();
      }

    if (!IsNew)
      {
      Strng NewCfgFile  = CfgFile();
      Strng NewCfgFiles = CfgFiles();

      SwingFileNameToDest(NewCfgFile, OrigPrjFiles, NewPrjFolder);
      SwingFileNameToDest(NewCfgFiles, OrigPrjFiles, NewPrjFolder);

      SetCfgFile(NewCfgFile());
      SetCfgFiles(NewCfgFiles());

      }
    //  dbgDumpFilesAliases();
    return GoodSave();
  }

// --------------------------------------------------------------------------

#define DBGLN(a) dbgpln("%s----------------------------------------------------------------------",a);
static void ClearRepeatedBackupFolder(Strng & DstFull)
  {
  int DLen=DstFull.Length();
  // Tmp= "\\$LC\\$LC\\"
  Strng Tmp;
  Tmp.Set("\\%s\\%s\\", BackupFilesFolderName(), BackupFilesFolderName());
  if (DLen>9 && Tmp.XStrICmp(&DstFull[DLen-9])==0)
    //if (Where==AF_BackupFiles)
    {
    // Remove Extra $LC
    DstFull.FnClearEndBSlash();
    DstFull.FnDrivePath();
    }
  }

// --------------------------------------------------------------------------

void CSFPInfo::SwingFilePath(flag DoingSaveAs, LPCTSTR Fn)
  {
  const int DoDbg=0;
  if (0)
    {
    dbgpln("-----------------------------");
    dbgpln("SwingFilePath    :%s", Fn);
    }

  SetUsePreviousPrjAlias(DoingSaveAs);

  SrcPrjRel=Fn;
  SrcPrjRel.FnExpand();
  SrcPrjRel.FnMakePrjFileRelative();
  SrcPrjRel.FnContract();

  SrcFull=Fn;
  SrcFull.FnExpand();
  SrcFull.FnRemoveDotDirs();
  char * PF=PrjFiles();
  ASSERT(PF); //this should be valid!
  char * PRF=PrjRmtCpyFiles();

  PrjRCScope=PRF && (SrcFull.XStrNICmp(PRF, strlen(PRF))==0);
  PrjScope=!PrjRCScope && PF && (SrcFull.XStrNICmp(PF, strlen(PF))==0);
  SrcExists=FileExists(SrcFull());

  SetUsePreviousPrjAlias(false);


  if (PrjScope)
    {
    SetUsePreviousPrjAlias(DoingSaveAs);
    DstFull=Fn;
    DstFull.FnContract();

    SetUsePreviousPrjAlias(false);
    DstFull.FnExpand();
    //dbgpln("%s %s  %s","       ","    ",DstFull());
    }
  else if (PrjRCScope)
    {
    SetUsePreviousPrjAlias(DoingSaveAs);
    DstFull=Fn;
    DstFull.FnContract();

    SetUsePreviousPrjAlias(false);
    DstFull.FnExpand();
    DstStgRel=DstFull.Right(DstFull.Length()-strlen(PRF));

    //dbgpln("%s %s  %s","       ","    ",DstFull());
    }
  else
    {
    // Dst = Path Relative to Original Prj
    SetUsePreviousPrjAlias(DoingSaveAs);
    Strng Dst=Fn;
    Dst.FnMakePrjFileRelative(true); // ito Src Project ie Previous
    Dst.FnContractDO();

    SetUsePreviousPrjAlias(false);
    // p points to DstPath - must be without $Prj
    char * p=Dst();
    // remove $Prj
    int lAlias=strlen(PrjFilesAlias());
    if (Dst.XStrNICmp(PrjFilesAlias(), lAlias)==0)
      p=&Dst[lAlias];

    DstFull=PRF;
    ClearRepeatedBackupFolder(DstFull);
    DstFull+=p;
    FnEscapeDotDotPath(DstFull);
    //dbgpln("%s %s  %s","       ","    ",Dst());

    DstStgRel=p;
    FnEscapeDotDotPath(DstStgRel);
    }

  if (SrcExists)
    {
    if (pFI->bPreventCopy)
      CopyReqd=false;
    else if (PrjScope && !PrjRCScope)
      CopyReqd=pFI->bForceCopy;
    else
      CopyReqd=true;
    }
  else
    CopyReqd=false;

  if (DoDbg)
    {
    dbgpln("  SrcPrjRel      :%s", SrcPrjRel());
    dbgpln("  SrcFull        :%s", SrcFull());
    dbgpln("  DstFull        :%s", DstFull());
    dbgpln("  DstStgRel      :%s", DstStgRel());
    dbgpln("  Scope:%s", PrjRCScope?"RmtCpy":PrjScope?"Project":"");
    dbgpln("  Src  :%s", SrcExists?"Exists":"");
    dbgpln("  Copy :%s", pFI->bPreventCopy?"Prevent":pFI->bForceCopy?"Force":"");
    dbgpln("  Act  :%s", CopyReqd?"Copy":"");
    }
  }

// --------------------------------------------------------------------------

flag CProject::CopyOtherFile(CSFPInfo & SFI)
  {
  DBGLN(">>>");
  dbgpln("%s %s  %s",SFI.SrcExists?"Exists ":"Missing", SFI.CopyReqd?"Copy":"   ", SFI.SrcFull());

  if (SFI.CopyReqd)
    {
    dbgpln("%s %s  %s","       ","    ",SFI.DstFull());
    Strng DstFold, E;
    DstFold.FnDrivePath(SFI.DstFull());
    if (!FileExists(DstFold()) && !FnCreatePath(DstFold(), E))
      {
      LogError("SysCAD", LF_Exclamation,
        "Folder not created :\n%s\n%s",DstFold(),E());
      DBGLN("<<<");
      return false;// continue;
      }

    FILETIME SFt, DFt;
    flag SOk=FnModifyTime(SFI.SrcFull(), SFt);
    flag DOk=FnModifyTime(SFI.DstFull(), DFt);
    if (SOk && DOk && (CompareFileTime(&SFt, &DFt)==0))
      {
      DBGLN("<<<");
      return false;// continue;
      }

    // Get and Clear Readonly
    DWORD OrigFileAtts=::GetFileAttributes(SFI.DstFull());
    flag AttsOK=OrigFileAtts!=0xFFFFFFFF;
    if (AttsOK && (OrigFileAtts & FILE_ATTRIBUTE_READONLY))
      ::SetFileAttributes(SFI.DstFull(), OrigFileAtts & ~FILE_ATTRIBUTE_READONLY);

    if (Copy_File(SFI.SrcFull(), DstFold()))
      {

      /* do not set the readonly attribute
      if (!AttsOK)
      {
      OrigFileAtts=::GetFileAttributes(DstFull());
      AttsOK=OrigFileAtts!=0xFFFFFFFF;
      OrigFileAtts |= FILE_ATTRIBUTE_READONLY;
      }
      if (AttsOK)
      ::SetFileAttributes(DstFull(), OrigFileAtts);
      */
      LogNote("SysCAD", 0, "Local Copy:%s",SFI.SrcFull());
      DBGLN("<<<");
      return true;
      }
    else
      {
      // Restore Readonly
      /* do not set the readonly attribute
      if (AttsOK)
      ::SetFileAttributes(DstFull(), OrigFileAtts);
      */
      Strng E;
      E.GetSystemErrorStr(GetLastError());
      LogError("SysCAD", LF_Exclamation,
        "File not copied :\n%s\n%s",SFI.SrcFull(),E());
      }
    }
  DBGLN("<<<");
  return false;
  }

// --------------------------------------------------------------------------

int CProject::CopyOtherFiles(CPrjFilesLists &PFL, CProfINIFile &PF)
  {
  int RefCnt=0;
  int i,j;

  int nFI=0;
  for (i=0; i<PFL.GetSize(); i++)
    for (j=0; j<PFL.pExecFiles[i]->Files.GetSize(); j++)
      nFI++;
  CArray<CSFPInfo, CSFPInfo&> SFPI;
  SFPI.SetSize(nFI);

  int ii=0;
  for (i=0; i<PFL.GetSize(); i++)
    {
    for (j=0; j<PFL.pExecFiles[i]->Files.GetSize(); j++)
      {
      SFPI[ii].pFI=&PFL.pExecFiles[i]->Files[j];
      SFPI[ii].SwingFilePath(bDoingSaveAs, PFL.pExecFiles[i]->Files[j].sFilename());
      ii++;
      }
    }

  PF.WrStr("Files_Used", "Comment", "This list of files is for information only!");
  PF.WrStr("Referenced_Files", "Comment", "This list of files is for project merge only!");
  ii=0;
  for (i=0; i<PFL.GetSize(); i++)
    {
    char Entry[256];
    Strng s;
    for (j=0; j<PFL.pExecFiles[i]->Files.GetSize(); j++)
      {
      CExecObjFileInfo &FI = *SFPI[ii].pFI;

      sprintf(Entry, "%s%i", PFL.pExecFiles[i]->sExecName(), j);

      PF.WrStr("Files_Used", Entry, SFPI[ii].SrcPrjRel());
      if ((FI.dwFlags & EFF_Grp_Referenced) && (FI.dwFlags & EFF_OtherValid))
        {
        sprintf(Entry, "File%i", RefCnt);
        PF.WrStr("Refrenced_Files", Entry, SFPI[ii].SrcPrjRel());
        sprintf(Entry, "Model%i", RefCnt++);
        PF.WrStr("Refrenced_Files", Entry, FI.sOther());
        }
      ii++;
      }
    }

  //copy "other remote" files...
  int NCopied=0;
  ii=0;
  for (i=0; i<PFL.GetSize(); i++)
    {
    for (j=0; j<PFL.pExecFiles[i]->Files.GetSize(); j++)
      {
      CSFPInfo & fi=SFPI[ii];
      if (fi.SrcExists && fi.CopyReqd && fi.PrjScope)
        {
        if (CopyOtherFile(fi))
          NCopied++;
        }
      ii++;
      }
    }

#if WithRmtFile
  //create compound storage file for remote files...
  ii=0;
  Strng LcFn=PrjFiles();
  LcFn+=RmtFileName;

  WCHAR WFn[MAX_PATH];
  AfxA2WHelper(WFn, LcFn(), LcFn.Length()+1);
  IStoragePtr spStorage;
  HRESULT hr=::StgCreateDocfile(WFn,
    STGM_DIRECT | STGM_READWRITE | STGM_CREATE | STGM_SHARE_EXCLUSIVE,
    0, &spStorage);

  if (S_OK==hr)
    {
    for (i=0; i<PFL.GetSize(); i++)
      {
      for (j=0; j<PFL.pExecFiles[i]->Files.GetSize(); j++)
        {
        CSFPInfo & fi=SFPI[ii];
        if (fi.SrcExists && fi.CopyReqd)
          {
          if (fi.PrjScope)
            {
            /*if (CopyOtherFile(fi))
            NCopied++;*/
            }
          else
            {
            if (fi.PrjRCScope)
              { int x=0;}
            else
              { int x=0;}

            char * pSrc=fi.SrcFull();
            char * pDst=fi.DstFull();
            char * pSrcRel=fi.SrcPrjRel();
            char * pStgRel=fi.DstStgRel();

            Strng StPath;
            Strng StName;
            StPath.FnDrivePath(pStgRel);
            StPath.FnClearEndBSlash();
            StName.FnNameExt(pStgRel);

            IStoragePtr sp[32];//Folder;
            sp[0].Attach(spStorage, true);
            int iDeep=0;
            bool GotStore=true;
            if (StPath.Length()>0)
              {
              char X[MAX_PATH];
              strcpy(X, StPath());
              char *p=X;
              while (GotStore)
                {
                char *q=strchr(p, '\\');
                if (q)
                  *q=0;

                WCHAR Nm[MAX_PATH];
                AfxA2WHelper(Nm, p, strlen(p)+1);

                HRESULT hr=sp[iDeep]->OpenStorage(Nm, NULL, STGM_DIRECT | STGM_READWRITE | STGM_SHARE_EXCLUSIVE, NULL,0, &sp[iDeep+1]);
                if (hr==STG_E_FILENOTFOUND)
                  hr=sp[iDeep]->CreateStorage(Nm, STGM_DIRECT | STGM_READWRITE | STGM_CREATE | STGM_SHARE_EXCLUSIVE, 0,0, &sp[iDeep+1]);

                spStorage->Commit(0);
                //dbgpln("XX: %08x > %08x [%08x] %-20s %s", sp[iDeep].GetInterfacePtr(), sp[iDeep+1].GetInterfacePtr(), hr, p, StPath());

                if (hr==S_OK || hr==STG_E_FILEALREADYEXISTS)
                  {
                  //spFolder=sp;//.Attach(sp, true);
                  iDeep++;
                  if (q==NULL)
                    break;
                  p=q+1;
                  }
                else
                  GotStore=false;
                }
              }

            if (GotStore)
              {
              IStreamPtr  spStream;
              WCHAR Nm[MAX_PATH];
              AfxA2WHelper(Nm, StName(), StName.Length()+1);
              hr=sp[iDeep]->CreateStream(Nm, STGM_DIRECT | STGM_READWRITE | STGM_CREATE | STGM_SHARE_EXCLUSIVE,
                0 , 0 , &spStream);
              if (hr==S_OK)
                {
                FILE * f=fopen(fi.SrcFull(), "rb");
                if (f==NULL)
                  LogError("SysCAD", 0, "File %s not opened", fi.SrcFull());
                else
                  {
                  while (f && !feof(f))
                    {
                    char Buff[4096];
                    ULONG n=fread(Buff, 1, sizeof(Buff),f);

                    if (n>0)
                      {
                      ULONG ul;
                      hr=spStream->Write(Buff, n, &ul );
                      if (S_OK!=hr || ul!=n)
                        {
                        LogError("SysCAD", 0, "Stream %s not written", StName());
                        break;
                        }
                      }
                    }
                  fclose(f);
                  }
                //spFolder->Commit( 0 );
                spStorage->Commit(0);
                }
              else
                LogError("SysCAD", 0, "Stream %s not opened", StName());

              }
            else
              LogError("SysCAD", 0, "Storage %s not created", StPath());
            }
          }
        ii++;
        }
      }
    spStorage->Commit(0);
    }
  else
    LogError("SysCAD", 0, "Storage %s not created", LcFn());
#endif

#if WITHZIP
  //create zip backup for remote files...
  ii=0;
  Strng ZpFn(PrjFiles());
  ZpFn += RmtBackupZipFileName;
  DeleteFile(ZpFn());
  CZipFile zf(ZpFn());
  CStringList FnDone;

  for (i=0; i<PFL.GetSize(); i++)
    {
    for (j=0; j<PFL.pExecFiles[i]->Files.GetSize(); j++)
      {
      CSFPInfo & fi=SFPI[ii];
      if (fi.SrcExists && fi.CopyReqd)
        {
        if (fi.PrjScope)
          {
          char * pSrc=fi.SrcFull();
          char * pDst=fi.DstFull();
          // zf.AddCmd(pDst); Why moust 'OTHER' files within the project be backed-up ????
          }
        else
          {
          char * pSrc=fi.SrcFull();
          // Make the path relative to the Project
          Strng Fn(pSrc);
          Fn.FnContract();
          if (Fn.XStrNICmp("$Prj\\~\\", 7)==0)
            {
            Fn=Fn.Right(Fn.Length()-6);
            Fn=".."+Fn;
            }
          Fn.FnUnEscapeDotDotPath(); // any more ".."

          //int iPos;
          //while ((iPos=Fn.Find(".."))>=0)
          //  {
          //  Strng X=Fn.Left(iPos);
          //  Strng Y=Fn.Right(Fn.Length()-iPos-2);
          //  
          //  Fn=X;
          //  Fn+="Parent";
          //  Fn+=Y;
          //  }

          if (!FnDone.Find(Fn())) // CNM dont save multiple copies
            {
            zf.AddCmd(Fn());
            FnDone.AddTail(Fn());
            }
          }
        }
      ii++;
      }
    }

  int ret = zf.ZipIt();
  if (ret!=0)
    LogWarning("SysCAD", 0, "Unable to create zip file '%s'", ZpFn());
#endif

  return NCopied;
  }

// --------------------------------------------------------------------------

static void DoRdStorage(WCHAR * WFn, IStoragePtr & spRtStorage, LPCTSTR Path)
  {
  IStoragePtr spStorage;
  HRESULT hr;
  if (spRtStorage.GetInterfacePtr())
    hr=spRtStorage->OpenStorage(WFn, NULL, STGM_DIRECT | STGM_READWRITE | STGM_SHARE_EXCLUSIVE, NULL, 0, &spStorage);
  else
    hr=::StgOpenStorage(WFn, NULL, STGM_DIRECT | STGM_READWRITE | STGM_SHARE_EXCLUSIVE, NULL, 0, &spStorage);

  if (S_OK==hr)
    {
    IEnumSTATSTGPtr Enum;
    hr=spStorage->EnumElements(0,NULL,0, &Enum);
    if (hr==S_OK)
      {
      STATSTG stg;
      while (S_OK==Enum->Next(1, &stg, NULL))
        {
        char Name[MAX_PATH];
        AfxW2AHelper(Name, stg.pwcsName, wcslen(stg.pwcsName)+1);
        Strng P=Path;
        P.FnCheckEndBSlash();
        P+=Name;

        switch (stg.type)
          {
          case STGTY_STORAGE:
            {
            DoRdStorage(stg.pwcsName, spStorage, P());
            int xxxx=0;
            }
            break;
          case STGTY_STREAM:
            {
            IStreamPtr spStream;
            Strng Folder, E;
            Folder.FnDrivePath(P());
            if (!FnCreatePath(Folder(), E))
              {
              LogError("SysCAD", 0,"Path %s not created:%s", Folder(), E());
              }
            else
              {
              FILE *f=fopen(P(), "wb");
              if (f==NULL)
                {
                LogError("SysCAD", 0,"File %s not opened", P());
                }
              else
                {
                hr=spStorage->OpenStream(stg.pwcsName, NULL, STGM_DIRECT | STGM_READ | STGM_SHARE_EXCLUSIVE, 0, &spStream);
                if (hr==S_OK)
                  {
                  char Buff[4096];
                  ULONG uRead;
                  for (;;)
                    {
                    hr=spStream->Read(Buff, sizeof(Buff), &uRead);
                    if (uRead>0)
                      {
                      ULONG uWrite=fwrite(Buff, 1, uRead, f);
                      if (uWrite!=uRead)
                        LogError("SysCAD", 0,"File %s not written ", P());
                      }
                    else
                      break;
                    }
                  }
                else
                  LogError("SysCAD", 0,"Storage %s not opened", P());
                fclose(f);
                }
              }
            }
            break;
            //STGTY_LOCKBYTES
            //STGTY_PROPERTY
          }
        CoTaskMemFree(stg.pwcsName);
        }
      }
    }
  else
    {
    char Name[MAX_PATH];
    AfxW2AHelper(Name, WFn, wcslen(WFn)+1);

    LogError("SysCAD", 0,"Storage %s not opened", Name);
    }
  }

// --------------------------------------------------------------------------

void CProject::ExpandStg(LPCTSTR StgFn, LPCTSTR Fn)
  {
  WCHAR WFn[MAX_PATH];
  AfxA2WHelper(WFn, StgFn, strlen(StgFn)+1);

  IStoragePtr spStorage;
  DoRdStorage(WFn, spStorage, Fn);
  };

// --------------------------------------------------------------------------

flag CProject::DoSave(flag DoPathChg, pchar pPrjPath, flag IsNew)
  {
  dbgpln("--------------------- CProject::DoSave");

  // Build these lists before 'SetPrjFiles' changes the meaning of Aliases
  //  SetUsePreviousPrjAlias(bDoingSaveAs);
  CPrjFilesLists PFL;
  PFL.BuildLists();

  bCmdSuccess = TRUE;
  if (DoPathChg)
    bCmdSuccess = DoPrjPaths(pPrjPath, IsNew, &PFL);
  if (!bCmdSuccess)
    return bCmdSuccess;

  dbgDumpFilesAliases();
  //Strng CfgF(CfgFile());

  m_bReadOnlyPrj = 0;

  //  sPrjFile = NewPrjFile();
  // NBNBNB if Busy is Open this call gets confused
  //SaveWindowLayout(); //
  CWaitMsgCursor Wait("Saving project");
  CStopWatch SW;
  SW.Start();

  Wait.UpdateMsg("%s project %s", m_Save.m_bIsNewPrj ? "Create" : "Saving", PrjFile());
  CDlgBusy::Open(m_Save.m_bIsNewPrj ? "\nCreate Project" : "\nSaving Project");
  Strng CompactPrj;
  CompactPrj = PrjFile();
  CompactPrj.FnCompactPath(50);
  //FnContract(CompactPrj, PrjFile())
  CDlgBusy::SetLine(3, CompactPrj());

  Strng s;
  if (bDoingSaveAs && !AllGrfLoaded())
    {//Copy all DXF files...
    s = m_sPrevPrjFiles;
    s += "*.dxf";
    WIN32_FIND_DATA fd;
    HANDLE H = FindFirstFile(s(), &fd);
    flag AllDone = (H==INVALID_HANDLE_VALUE);
    while (!AllDone)
      {
      s = m_sPrevPrjFiles;
      s += fd.cFileName;
      Copy_File(s(), PrjFiles());
      AllDone = !FindNextFile(H, &fd);
      }
    FindClose(H);
    }

  Strng sPrjBac(PrjFile());
  sPrjBac = sPrjBac.Left(sPrjBac.Length()-3);
  sPrjBac += "bac";
  CopyFile(PrjFile(), sPrjBac(), false); //backup spj

  char DT[64],TM[64];
  _strdate(DT);
  _strtime(TM);

  DeleteFile(PrjFile()); //delete old spj file

  CProfINIFile PF(PrjFile());

  GetSettings();

  PF.WrStr("General", "PrjName", PrjName());
  PF.WrStr("General", "PrjFiles", PrjFiles());

  PF.WrStr("General", "CfgFile", FnPathRelContract(CfgFile()));
  PF.WrInt("General", "LocalPrjFiles", UsingPrjLclFiles());

  //gs_HstMngr.GetOnRqd();

  WriteSettings(PF, true);

  //gs_HstMngr.WriteSettings(PF);

  SaveMainOPCOptions(PF);

  DisableMainOPCSrvr();

  for (int i=0; i<16; i++)
    {
    if (gs_CustomColours[i])
      {
      char Item[32];
      sprintf(Item, "Colour%d", i);
      PF.WrLong("CustomColours", Item, gs_CustomColours[i]);
      }
    }

  //save documents...
  for (int j=iGraphTemplate; j<=iControlTemplate; j++)
    if (ScdApp()->TemplateExists(j))
      {
      Strng DocTypeName=DocumentTmplNames[j];
      CDocTemplate & Templ = ScdApp()->Template(j);
      POSITION Pos = Templ.GetFirstDocPosition();
      int DocCount = 0;
      PF.WrInt(DocumentSectNames[j], "DocumentCount", DocCount); // Initialise
      while (Pos)
        {
        CDocument* pDoc = Templ.GetNextDoc(Pos);

        CString dn = pDoc->GetPathName();
        i = dn.ReverseFind('\\');
        if (m_Save.m_bIsNewPrj && i>=0)
          {
          CString d;
          d = PrjFiles();
          d += dn.Right(dn.GetLength()-i-1);
          dn = d;
          }

        s = dn;
        //s.FnCompactPath(50);
        s.FnNameExt();
        CDlgBusy::SetLine(3, s());
        if (pDoc->IsKindOf(RUNTIME_CLASS(DocRoot)))
          {
          ((DocRoot*)pDoc)->OnActivate(true);  // direct cmds to this document
          ((DocRoot*)pDoc)->SaveDocument((LPCTSTR)dn);
          }
        else if (pDoc->IsKindOf(RUNTIME_CLASS(OleDocRoot)))
          {
          if (dn.GetLength()==0)
            dn=((OleDocRoot*)pDoc)->m_strReqdPathName;
          pDoc->DoSave((LPCTSTR)dn, TRUE);
          }

        dn = pDoc->GetPathName();
        if (dn.GetLength()>0)
          {
          char Entry[256];
          sprintf(Entry, "Doc%i", DocCount++);
          PF.WrStr(DocumentSectNames[j], Entry, FnContract(s, (LPTSTR)(LPCTSTR)dn));
          }
        }
      PF.WrInt(DocumentSectNames[j], "DocumentCount", DocCount);
      }

    gs_AccessWnds.SaveNdHist(-1);

    //save list of all files referenced/used by project...
    CDlgBusy::SetLine(3, "Copying 'Remote' files");

    int NCopied = CopyOtherFiles(PFL, PF);

    CDlgBusy::SetLine(3, CompactPrj());
    if (NCopied>0)
      //LogNote("SysCAD", 0, "%i Files Copied to %s", NCopied, BackupFilesFolderName());
      LogNote("SysCAD", 0, "%i Files Copied", NCopied);

    SaveColours(PF);

    DWORD Opts=0;
    if (!m_Save.m_bRqstNew)
      {
      if (m_bRemoveRevs)
        Opts |= DBO_RemovePrevRevs;
      //if (m_bCopyDBFiles && (PrjFileVerNoAsLoaded()==PrjFileVerNo()))
      //  Opts |= DBO_CopyOldDBFiles;
      }

    gs_pPrj->DoSaveDB(XDBFormat(), Opts, PrjFiles(), (bDoingSaveAs||bDoingSaveAsNV ? m_sPrevPrjFiles() : NULL));

    CDlgBusy::SetLine(3, "Saving Window Layout");
    SaveWindowLayout();
    CDlgBusy::SetLine(3, CompactPrj());

    CProfINIFile PFI(PrjIniFile());
    PFI.WrLong("SnapShot", "Action", m_eSSAction);
    PFI.WrStr("SnapShot", "Name", m_sSSName());
    PFI.WrStr("SnapShot", "Prefix", m_sSSPrefix());
    PFI.WrLong("SnapShot", "Number", m_iSSNumber);
    PFI.WrLong("SnapShot", "DoZip", m_bSSDoZip);

    DeleteFile(sPrjBac()); //saved all; delete backup

    if (CfgFile())
      ScdPFUser.WrStr("General", "LastCfgFile", CfgFile());

    WriteProjectSet();

    SW.Stop();
    long SaveMin = (long)floor(SW.Secs()/60.0);
    Strng DtTm;
    LogNote("Project", 0, "Saved at %s (save time %d:%02d)", CurDateTime(DtTm), SaveMin, (long)floor(SW.Secs()-(60.0*(double)SaveMin)));
    CDlgBusy::Close();
    bCmdSuccess = true;

    // After the save is equivalent to just saved
    SetPrjFileVerNoAsLoaded(PrjFileVerNo());

    bDoingSaveAs = false;
    bDoingSaveAsNV = false;
    return GoodSave();
  }

//---------------------------------------------------------------------------

void CProject::CloseRmtCpy()
  {
  if (m_RmtCpyFolder())
    {
    m_RmtCpyLock.Close();

    Strng FnL;
    FnL=m_RmtCpyFolder;
    FnL.FnCheckEndBSlash();
    FnL+="Lock.Tmp";
    if (FileExists(FnL()))// && DeleteFile(FnL()))
      {
      ClearFolder(m_RmtCpyFolder(), false);
      }
    m_RmtCpyFolder="";
    }
  }

//---------------------------------------------------------------------------

flag CProject::DoClose(flag ForExit, flag AllowCancel/*=true*/)
  {
  if (gs_Exec.TestRunning())
    return false;

#if CREATENETSERVER
  if (m_pCLRSrvr)
    {
    m_pCLRSrvr->Shutdown();
    //PKH delete m_pCLRSrvr;
    }
#endif

  gs_ProjectOpenFlag=0;

  CWaitMsgCursor Wait("Closing project");

  //CMdlValueSet::Clear();
  //CRptTagLists::Hide();
  CRptTagLists::ShutDown();

#ifdef _DEBUG
  ForExit = 0; //in full debug mode, close everything nicely to test for memory leaks etc
#endif
  //enum DoWhats { DO_SaveExit, DoSaveNoExit, DoNoExit, DoExit}
  flag ExitRqd=true;
  flag SaveRqd=false;
  if (pPrjDoc && (bHstChanged || bDrvChanged || bArcChanged || bTimeChanged || bDocChanged)) //bDocChanged is NOT always updated correctly!!!
    {
    if (PrjFile())
      {
      if (gs_pXCmd->SaveOnExit())
        SaveRqd=true;
      else if (gs_pXCmd->NoSaveOnExit())
        SaveRqd=false;
      else if (!gs_pXCmd->DoNotPrompt())
        {
        char Buff[512];
        UINT nType;
        if (gs_pPrj->m_bReadOnlyPrj && AllowCancel)
          {
          nType = MB_OKCANCEL;
          sprintf(Buff, "Close Project %s ?\n\nIf changes were made, press Cancel and use 'Project Save As' before closing.", PrjFile());
          }
        else
          {
          nType = (AllowCancel ? MB_ICONQUESTION|MB_YESNOCANCEL : MB_ICONQUESTION|MB_YESNO);
          Strng T(PrjFiles());
          T.FnClearEndBSlash();
          sprintf(Buff, "Save Project %s", T());//PrjFile());
          }
        switch (AfxMessageBox(Buff, nType))
          {
          case IDCANCEL :
            ExitRqd=false;
            bDocChanged=1;
            break;
          case IDYES    :
            SaveRqd=true;
            //SolveTool.Close();
            //DoSave(NULL, NULL);
            break;
          }
        }
      }
    if (ExitRqd || SaveRqd)
      {
      bTimeChanged=0;
      bHstChanged=0;
      bDrvChanged=0;
      bArcChanged=0;
      bDocChanged=0;
      }
    }

  if (!ExitRqd)
    return false;

  if (SaveRqd)
    {
    SolveTool.Close();
    //DoPrjPaths(NULL, NULL);
    if (!DoSave(true, NULL, false))
      return false;
    }

  gs_AccessWnds.SaveNdHist(-1);

  SolveTool.Close();

  MainWnd()->ClosingProject(ForExit);

  if (ForExit)
    {
    //Clear Modified all documents...
    CDocument* pDoc = pPrjDoc;
    if (pDoc)
      pDoc->SetModifiedFlag(FALSE);
    int i=0;
    while ((pDoc=ScdApp()->DocumentNo(i++))!=NULL)
      pDoc->SetModifiedFlag(FALSE);
    pPrjDoc = NULL; // CNM
    }
  else
    {
    gs_AccessWnds.CloseAccessData(-1, false, true, true);
    gs_AccessWnds.CloseWnd(-1);
    GrfProjectClosing();

    //close all documents...
    CDocument* pDoc = pPrjDoc;
    if (pDoc && !(((CPrjDoc*)pDoc)->bOnCloseBusy))
      pDoc->OnCloseDocument();
    while ((pDoc=ScdApp()->DocumentNo(0))!=NULL)
      pDoc->OnCloseDocument();
    pPrjDoc = NULL; // CNM

    MdlLibs.UnLoadFlwDLLs();
    m_pFlwLib=NULL;

    ((CMainFrame*)AfxGetMainWnd())->UpdateMainToolBar();


    //    dbgpln("Check Integrators");
    //    for (Integrator *p=Integrator::First; p; p=p->Next)
    //      {
    //      dbgpln("BAD INTEGRATOR %s", p->Tag);
    //      }
    //    dbgpln("======================");

    }

  CloseHistorian(ForExit);
  CloseIOMarshal(ForExit, TRUE);
  CloseArcManager(ForExit, TRUE);
#if WITHDRVMAN 
  CloseDrvManager(ForExit, TRUE);
#endif
  CloseDDEManager(ForExit);
  //CloseOPCManager(ForExit);
  DisableMainOPCSrvr();

#if WITHNETSERVER
  CloseNetManager(ForExit);
  pCS_Mngr->SetServer(false);
  pCS_Mngr->SetClientConnect(false, "", false);
#endif

  SetPrjFiles();
  SetPrjRmtCpyFiles(NULL);

  CloseRmtCpy();

  return true;
  }

//---------------------------------------------------------------------------

flag CProject::CloseForFailedOpen()
  {
  //UnLoadFlwDLLs();
  SetPrjFiles();
  SetPrjRmtCpyFiles(NULL);
  //CloseRmtCpy();
  ((CMainFrame*)AfxGetMainWnd())->UpdateMainToolBar();
  return true;
  }

//===========================================================================
//
//
//
//===========================================================================

flag CProject::OnNew()
  {
  LogSeparator("New",0);
  CModelInfo Info;
  CString RqdPrjCfg=m_sNewPrjCfg;
  CString RqdPrjPath=m_sNewPrjPath;
  LPCTSTR pRqdPrjCfg=RqdPrjCfg.GetLength()>0 ? (LPCTSTR)RqdPrjCfg : NULL;
  LPCTSTR pRqdPrjPath=RqdPrjPath.GetLength()>0 ? (LPCTSTR)RqdPrjPath : NULL;

  BOOL WithVersion = m_bWithVersion;
  BOOL RemoveOld   = m_bRemoveOld;
  m_bWithVersion = WithVersionDefault;
  m_bRemoveOld   = RemoveOldDefault;
  m_sNewPrjCfg="";
  m_sNewPrjPath="";

  TaggedObject::SetRevert2DefaultRqd(false);

  bCmdSuccess = DoClose(false, true);//, false);
  if (bCmdSuccess)
    {
    bCmdSuccess = DoNew(false);

    SetPrjFileFormat(1);

    Strng sModelCfg = ScdPFUser.RdStr("General", "LastCfgFile", "");
    //if (sModelCfg.Length()==0)
    //  sModelCfg = ScdPFUser.RdStr("General", "RecentModelLib", "");
    if (!Info.GetSetName("", true, LatestPrjFileVer, gs_License.LicCatagories(), pRqdPrjCfg))
      return (bCmdSuccess = false);

    m_fNewTrend = Info.fNewTrend!=0;
    m_fNewGraph = Info.fNewGraph!=0;

    SetCfgFile(Info.m_sCfgFile());
    SetCfgFiles(Info.m_sCfgFiles());

    CheckCfgFilesName();
    int Ret=TestCfgFiles();
    if (Ret&0x1)
      LogWarning("Project", LF_Exclamation, "Folder '%s' missing", BaseCfgFiles());
    if (Ret&0x2)
      LogWarning("Project", LF_Exclamation, "Folder '%s' missing", CfgFiles());

    Strng Test1(CfgFiles());
    Test1+="_SysCAD.mdb";
    Strng Test2(CfgFiles());
    Test2+=CfgDBFileName();

    if (FileExists(Test1()) && !FileExists(Test2()))
      if (!MoveFile(Test1(), Test2()))
        LogError("Project", 0, "%s not renamed to %s", Test1(), Test2());

    //bCmdSuccess = DoNew(false);
    }
  if (bCmdSuccess)
    {

    SetPrjFiles();
    ScdPFUser.WrStr("General", "LastCfgFile", CfgFile());
    // This will setup all the paths etc.
    //    bCmdSuccess = DoSave(NULL, /*NULL,*/ true);
    bCmdSuccess = DoPrjPaths((LPTSTR)(LPCTSTR)pRqdPrjPath, true, NULL, WithVersion, RemoveOld);

    // Initialise Mode
    CProfINIFile PrjCfg(CfgFile());

    gs_Exec.SetDefNetMode(PrjCfg.RdStr("Modes", "Default_NetMode",  "ProBal"));

    //int PBOk=PrjCfg.RdInt("General", "ProbalAllowed",  1);
    //int DynOk=PrjCfg.RdInt("General", "DynamicAllowed",  0);

    //if (PBOk && DynOk)
    //  {
    //  CNetModeSelectDlg Dlg(gs_Exec.DefNetMode()==NM_Probal);
    //  if (Dlg.DoModal()==IDOK)
    //    {
    //    gs_Exec.SetDefNetMode(Dlg.m_Probal ? NM_Probal:NM_Dynamic);
    //    }
    //  else
    //    bCmdSuccess=false;
    //  }
    //CProjectSettings Sets;
    CProfINIFile PrjSets(PrjFile());

    PrjSets.WrStr("General", "CfgFile", CfgFile());

    ReadSettings(PrjSets, 10000); // Get Defaults from CfgFile
    if (RmtCmdBusy() || EditSettings("New Project Settings", NULL))
      {
      //WriteSettings(PrjSets, true);
      //PutSettings();

      Info.m_sDLLList.Upper();
      Strng FlwLibTag, EO_LocationTag;

      FlwLibTag=PlantModelTag;
      EO_LocationTag=PlantModelTag;

      m_pFlwLib=NULL;
      bCmdSuccess = MdlLibs.LoadFlwDLLs(Info.m_sDLLList, FlwLibTag(), EO_LocationTag(), &m_FlwLib);
      if (bCmdSuccess)
        {
        m_pFlwLib=&m_FlwLib;

        PutSettings();

        SetTagMonitor();
        LoadHelpFileList();
        //if (!bCmdSuccess)
        //  UnLoadFlwDLLs();

        CRptTagLists::StartUp();

        OpenHistorian();
#if WITHDRVMAN
        OpenDrvManager();
#endif
        OpenArcManager(true);
        OpenIOMarshal();
        
        if (CExploreScd::UseScdExplorer())
          CExploreScd::OpenIt();
        else
          CExploreScd::CloseIt();


        MainWnd()->DefaultToolBar(-1, TRUE);

        //if (DefNetMode()==NM_Probal)
        //  {
        //  SetDefNodeMode(PrjCfg.RdStr("Modes", "Probal_NodeMode", "Direct"));
        //  SetDefLinkMode(PrjCfg.RdStr("Modes", "Probal_LinkMode", "Direct"));
        //  SetDefFlowMode(PrjCfg.RdStr("Modes", "Probal_FlowMode", "Transfer"));
        //  SetDefHeatMode(PrjCfg.RdStr("Modes", "Probal_HeatMode", "Rigorous"));
        //  }
        //else
        //  {
        //  SetDefNodeMode(PrjCfg.RdStr("Modes", "Dynamic_NodeMode", "Direct"));
        //  SetDefLinkMode(PrjCfg.RdStr("Modes", "Dynamic_LinkMode", "Direct"));
        //  SetDefFlowMode(PrjCfg.RdStr("Modes", "Dynamic_FlowMode", "Simple"));
        //  SetDefHeatMode(PrjCfg.RdStr("Modes", "Dynamic_HeatMode", "Rigorous"));
        //  }

        //SetMaxNodeMode(PrjCfg.RdStr("Modes", "Maximum_NodeMode", "Buffered"));
        //SetMaxLinkMode(PrjCfg.RdStr("Modes", "Maximum_LinkMode", "Direct"));
        //SetMaxFlowMode(PrjCfg.RdStr("Modes", "Maximum_FlowMode", "Full"));
        //SetMaxHeatMode(PrjCfg.RdStr("Modes", "Maximum_HeatMode", "Rigorous"));

        //Strng MdStr=TaggedObject::GlblMode2String(SM_Direct, SM_Direct);
        //MdStr=PrjCfg.RdStr("Modes", "Initial_SolveMode", MdStr());
        //if (TaggedObject::GlblSolveMode()==0)
        //  gs_Exec.SetGlblRunModes(SM_Direct, SM_All);
        //MdStr=TaggedObject::GlblMode2String(HM_Full, HM_All);
        //MdStr=PrjCfg.RdStr("Modes", "Initial_HeatMode", MdStr());
        //gs_Exec.SetGlblRunModesString((LPCSTR)MdStr(), HM_All);
        //if (TaggedObject::GlblHeatMode()==0)
        //  gs_Exec.SetGlblRunModes(HM_Full, HM_All);


        //MdStr.LRTrim();
        //bool PB = (MdStr.XStrICmp("Probal")==0);
        //SetProBalMode(PB);
        //if (!PB)
        //  {//NOT probal
        //  SetDynModeFlow(MdStr.XStrICmp("DynamicFlow")==0);
        //  }

        // This will do actual save
        //        bCmdSuccess = DoSave();
  
        gs_ProjectOpenFlag++;
        }
      }
    else
      {
      bCmdSuccess = false;
      }
    }

  return bCmdSuccess;
  }

//---------------------------------------------------------------------------

flag CProject::OnOpen(pchar pPrjPath)
  {
  LogSeparator("Open",0);
  bCmdSuccess=true;

  TaggedObject::SetRevert2DefaultRqd(false);

  //Try rename if possible
  Strng NewPrjPath;
  if (pPrjPath)
    {
    Strng Fn;
    Fn.FnNameExt(pPrjPath);
    if (Fn.XStrICmp("$.spj")==0)
      {
      NewPrjPath.FnDrivePath(pPrjPath);
      NewPrjPath.FnCheckEndBSlash();
      NewPrjPath+="Project.spj";
      if (FileExists(pPrjPath) && !FileExists(NewPrjPath()))
        {
        Strng ss;
        ss.Set("Upgrade project format?\n(Project is less than Build 40)\n\n%s", pPrjPath);
        if (AfxMessageBox(ss(), MB_YESNO|MB_DEFBUTTON2|MB_ICONQUESTION)==IDNO)
          {
          bCmdSuccess=false;
          return bCmdSuccess;
          }
        if (MoveFile(pPrjPath, NewPrjPath()))
          pPrjPath=NewPrjPath();
        else
          {
          LogError("Project", 0, "%s not renamed to %s", pPrjPath, NewPrjPath());
          bCmdSuccess=false;
          return bCmdSuccess;
          }
        }
      }
    }

  Strng NewModelCfg;
  if (1)
    {
    bool LoadFromLocalPrv=false;
    bool Found=false;
    CSVector S;
    S.SetSize(50);
    int j=-1;
    for (int i=0; i<50; i++)
      {
      Strng Entry;
      Entry.Set("Opn%03i",i);
      S[i]=ScdPFUser.RdStr("PreviousOpens", Entry());
      int l=strlen(pPrjPath);
      if (S[i].Length()>l)
        {
        if (S[i].XStrNICmp(pPrjPath, l)==0 && S[i][l]==',')
          {
          j=i;
          LoadFromLocalPrv=(S[j][l+1]=='L');
          Found=true;
          }
        }
      else
        break;
      }

    if (sm_fOpenAltCfg)
      LoadFromLocalPrv = false;

    if ((sm_LoadTypeRqst==PLT_Null) && Found)
      sm_LoadTypeRqst = LoadFromLocalPrv ? PLT_Local : PLT_Normal;

    Strng T;
    T.Set("%s,%s", pPrjPath, sm_LoadTypeRqst==PLT_Local ? "L":"N");
    ScdPFUser.WrStr("PreviousOpens", "Opn000", T());
    int k=1;
    for (int i=0; i<50 && k<50; i++)
      {
      if (i!=j && S[i].Length()>0)
        {
        Strng Entry;
        Entry.Set("Opn%03i",k++);
        ScdPFUser.WrStr("PreviousOpens", Entry(), S[i]());
        }
      }
    }

  if (sm_fOpenAltCfg)
    {
    CModelInfo Info;
    if (Info.GetSetName(pPrjPath, false, PrjFileVerNo(), gs_License.LicCatagories()))
      {
      NewModelCfg=Info.m_sCfgFile();
      m_fNewTrend = Info.fNewTrend!=0;
      m_fNewGraph = Info.fNewGraph!=0;
      }
    else
      bCmdSuccess=false;
    }

  if (bCmdSuccess)
    {
    bCmdSuccess = DoClose(false, true);//, false);
    if (bCmdSuccess)
      {
      bCmdSuccess = DoOpen(pPrjPath, NewModelCfg(), sm_LoadTypeRqst);
      if (bCmdSuccess && !RmtCmdBusy())
        {
        ScdPFUser.WrStr("General", "RecentProject", PrjFile());
        ScdApp()->AddToRecentFileList(PrjFile());
        ScdApp()->StdProfileChanged();
        }
      }
    }
  return bCmdSuccess;
  }

//---------------------------------------------------------------------------

flag CProject::OnClose()
  {
  LogSeparator("Close",0);
  bCmdSuccess = DoClose(false, true);
  if (bCmdSuccess)
    {
    SetPrjFileFormat(1);
    //    if (!RmtCmdBusy())
    //      {
    //      ScdPFUser.WrStr("General", "RecentProject", PrjFile());
    //      ScdApp()->AddToRecentFileList(PrjFile());
    //      ScdApp()->StdProfileChanged();
    //      }
    CMsgWindow::ClearAll();
    bCmdSuccess = DoNew(false);
    }
  ScdMainWnd()->PostMessage(WMU_CMDDONE, ComCmd_CloseProject, 0); //let script cmd mngr know cmd is complete
  return bCmdSuccess;
  }

//---------------------------------------------------------------------------

flag CProject::OnSave(pchar pPrjPath)
  {
  LogSeparator("Save",0);
  if (gs_pPrj->bForceSaveAsNV)
    {
    gs_pPrj->bDoingSaveAs=true;
    gs_pPrj->bDoingSaveAsNV=true;
    }

  if (gs_pPrj->bDoingSaveAs || (pPrjPath && _stricmp(PrjFile(), pPrjPath)!=0))
    {
    bCmdSuccess = OnProjectSaveas(pPrjPath);
    if (bCmdSuccess)
      gs_pPrj->bForceSaveAsNV=false;
    }
  else
    {
    if (gs_pPrj->m_bReadOnlyPrj)
      {
      LogWarning("SysCAD", 0|LF_Exclamation, "Project marked as read-only, use Project Save As option.");
      bCmdSuccess = false;
      return false;
      }
    bCmdSuccess = DoSave(true, pPrjPath, false);
    if (bCmdSuccess)
      {
      if (!RmtCmdBusy())
        {
        ScdPFUser.WrStr("General", "RecentProject", PrjFile());
        ScdApp()->AddToRecentFileList(PrjFile());
        }
      gs_pPrj->bForceSaveAsNV=false;
      }
    }
  CWindowLists::ActivateTopWindowCmds();
  ScdMainWnd()->PostMessage(WMU_CMDDONE, ComCmd_SaveProject, 0); //let script cmd mngr know cmd is complete
  return bCmdSuccess;
  }

//---------------------------------------------------------------------------

flag CProject::OnProjectSaveas(pchar pPrjPath)
  {
  //  ASSERT_ALWAYS(sPrjFile.XStrICmp(PrjFile())==0)

  Strng OldHstDataDir;
  m_sPrevPrjFile   = PrjFile();
  m_sPrevPrjFiles  = PrjFiles();
  OldHstDataDir = gs_HstMngr.sHstDataDir();
  OldHstDataDir.FnExpand(); // Expand before project changes

  //  bDoingSaveAs = true;
  MainWnd()->ClosingProject(false);

  //  CPrjSaveAsDlg Dlg(m_sPrevPrjFiles());
  //  Dlg.m_CopyHist = true;

  bCmdSuccess = DoSave(true, pPrjPath, false);
  if (bCmdSuccess)
    {
    CloseHistorian();
    CloseIOMarshal();
    CloseArcManager();
#if WITHDRVMAN
    CloseDrvManager();
#endif
    Strng s;
    //Strng PrevRoot = m_sPrevPrjFiles;
    //PrevRoot = PrevRoot.Left(PrevRoot.Length() - 4);
    //PrevRoot += '\\';
    //CPrjSaveAsDlg Dlg(m_sPrevPrjFiles());
    //Dlg.m_CopyHist = true;
    //Dlg.DoModal();
    CWaitCursor Wait;
    //spj/ini stuff...
    WIN32_FIND_DATA fd;
    HANDLE H;
    s = m_sPrevPrjFiles;
    s += "*.ini"; // s += "scd_prj.ini";
    H = FindFirstFile(s(), &fd);
    flag AllDone = (H==INVALID_HANDLE_VALUE);
    while (!AllDone)
      {
      s = m_sPrevPrjFiles;
      s += fd.cFileName;
      Copy_File(s(), PrjFiles());
      AllDone = !FindNextFile(H, &fd);
      }
    FindClose(H);
    //historian...
    Strng DstHstDir(gs_HstMngr.sHstDataDir);
    DstHstDir.FnExpand();
    DstHstDir.FnClearEndBSlash();

    s = m_sPrevPrjFiles;
    s += gs_HstMngr.sHistorianName();
    s += ".hed";
    Copy_File(s(), PrjFiles());
    s = m_sPrevPrjFiles;
    s += gs_HstMngr.sHistorianName();
    s += ".cat";
    Copy_File(s(), PrjFiles());
    if (m_DlgCopyHist)
      {//copy historian data...
      s = OldHstDataDir;
      s += gs_HstMngr.sHstScenName();
      s += ".*";
      H = FindFirstFile(s(), &fd);
      flag AllDone = (H==INVALID_HANDLE_VALUE);
      while (!AllDone)
        {
        s = OldHstDataDir;
        s += fd.cFileName;
        Copy_File(s(), DstHstDir());
        AllDone = !FindNextFile(H, &fd);
        }
      FindClose(H);
      }
    for (int i=0; i<KnownExtCnt; i++)
      {
      if (m_DlgCopy[i])
        {//copy all files...
        for (int j=0; j<2; j++)
          if (j==0 || strlen(CPrjFilesLists::OtherExtsEx(i)))
            {
            s = m_sPrevPrjFiles;
            s += '*';
            s += (j==0 ? CPrjFilesLists::OtherExts(i) : CPrjFilesLists::OtherExtsEx(i));
            H = FindFirstFile(s(), &fd);
            flag AllDone = (H==INVALID_HANDLE_VALUE);
            while (!AllDone)
              {
              s = m_sPrevPrjFiles;
              s += fd.cFileName;
              Copy_File(s(), PrjFiles());
              AllDone = !FindNextFile(H, &fd);
              }
            FindClose(H);
            }
        }
      }
    OpenHistorian();
#if WITHDRVMAN
    OpenDrvManager();
#endif
    OpenArcManager(false);
    OpenIOMarshal();
    if (bCmdSuccess && !RmtCmdBusy())
      {
      ScdPFUser.WrStr("General", "RecentProject", PrjFile());
      ScdApp()->AddToRecentFileList(PrjFile());
      ScdApp()->StdProfileChanged();
      }

    Strng T(PrjFile());
    Strng T2;
    T.FnName(PrjFile());
    T2.FnExt(PrjFile());
    T += T2;
    pPrjDoc->SetTitle(T());
    gs_pPrj->pPrjDoc->UpdateAllViews(NULL, 1);

    if (bCmdSuccess)
      SaveWindowLayout(); //layout.ini overwritten, save again!
    //  bDoingSaveAs = false;
    m_sPrevPrjFile = "";
    m_sPrevPrjFiles = "";
    MainWnd()->PostMessage(WMU_UPDATEMAINWND, SUB_UPDMAIN_ACTIVATECMDS, 0);
    MainWnd()->PostMessage(WMU_CMD, SUB_CMD_PRJWNDUPDATE, 0);
    }
  //  else
  //    Clear();

  //set document title and update project window...
  return bCmdSuccess;
  }

//---------------------------------------------------------------------------

int CProject::ClearSelectedNodes()
  {
  POSITION PosT = ScdApp()->GetFirstDocTemplatePosition();
  while (PosT)
    {
    CDocTemplate* pTempl = ScdApp()->GetNextDocTemplate(PosT);
    POSITION PosD = pTempl->GetFirstDocPosition();
    while (PosD)
      {
      CDocument* pDoc = pTempl->GetNextDoc(PosD);
      if (pDoc->IsKindOf(RUNTIME_CLASS(CGrfDoc)))
        {
        ((CGrfDoc*)pDoc)->ClearSelectedTags();
        }
      /*
      else if (pDoc->IsKindOf(RUNTIME_CLASS(CTagVwDoc)))
      {
      }
      else if (pDoc->IsKindOf(RUNTIME_CLASS(CPrjDoc)))
      {
      }
      */
      }
    }
  return 0;
  }

//---------------------------------------------------------------------------

int CProject::BuildSelectedNodeList(Strng_List & List)
  {
  POSITION PosT = ScdApp()->GetFirstDocTemplatePosition();
  while (PosT)
    {
    CDocTemplate* pTempl = ScdApp()->GetNextDocTemplate(PosT);
    POSITION PosD = pTempl->GetFirstDocPosition();
    while (PosD)
      {
      CDocument* pDoc = pTempl->GetNextDoc(PosD);
      if (pDoc->IsKindOf(RUNTIME_CLASS(CGrfDoc)))
        {
        ((CGrfDoc*)pDoc)->GetSelectedTags(List);
        }
      /*
      else if (pDoc->IsKindOf(RUNTIME_CLASS(CTagVwDoc)))
      {
      }
      else if (pDoc->IsKindOf(RUNTIME_CLASS(CPrjDoc)))
      {
      }
      */
      }
    }
  return List.Length();
  }

//===========================================================================
//
//
//
//===========================================================================

flag CProject::ModelsAttached() { return FlwLib()!= NULL; };

//---------------------------------------------------------------------------

pNodeGrfInfo CProject::GetNodeDrawings()
  {
  return FlwLib()!=NULL ? FlwLib()->NodeDrawings()/*SFENodeGrfInfo*/ : NULL;
  }

//---------------------------------------------------------------------------

pchar CProject::RequestModelDLLPath()
  {
  return FlwLib()!=NULL ? FlwLib()->RequestModelDLLPath() : NULL;
  };

//---------------------------------------------------------------------------

pchar CProject::RequestModelDLLTag()
  {
  return FlwLib()!=NULL ? FlwLib()->RequestModelDLLTag() : NULL;
  };

//---------------------------------------------------------------------------

flag CProject::RequestModelInfoByGroupIndex(pchar pGroup, int iIndex, RequestModelInfoRec &Info)
  {
  return FlwLib()!=NULL ? FlwLib()->RequestModelInfoByGroupIndex(pGroup, iIndex, Info) : FALSE;
  };

//---------------------------------------------------------------------------

flag CProject::RequestModelInfoByClassId(pchar pClass, RequestModelInfoRec & Info)
  {
  return FlwLib()!=NULL ? FlwLib()->RequestModelInfoByClassId(pClass, Info) : FALSE;
  };

//---------------------------------------------------------------------------

flag CProject::RequestModelClassId(pchar pTag, Strng & ClassId)
  {
  return FlwLib()!=NULL ? FlwLib()->RequestModelClassId(pTag, ClassId) : FALSE;
  };

//---------------------------------------------------------------------------

flag CProject::RequestModelConnInfo(pchar pTag, RequestConnModelInfoRec & Info)
  {
  return FlwLib()!=NULL ? FlwLib()->RequestModelConnInfo(pTag, Info) : FALSE;
  };

//---------------------------------------------------------------------------

flag CProject::RequestModelIOInfoByClassId(pchar pClass, RequestModelIOInfoArray & Info)
  {
  return FlwLib()!=NULL ? FlwLib()->RequestModelIOInfoByClassId(pClass, Info) : FALSE;
  };

//---------------------------------------------------------------------------

flag CProject::RequestModelIOInfoByIndex(pchar pTag, int iIndex, RequestModelIOInfoRec & Info)
  {
  return FlwLib()!=NULL ? FlwLib()->RequestModelIOInfoByIndex(pTag, iIndex, Info) : FALSE;
  };

//---------------------------------------------------------------------------

flag CProject::RequestModelIOInfoById(pchar pTag, int iId, RequestModelIOInfoRec & Info)
  {
  return FlwLib()!=NULL ? FlwLib()->RequestModelIOInfoById(pTag, iId, Info) : FALSE;
  };

//---------------------------------------------------------------------------

flag CProject::RequestModelIOConn(pchar pTag, int iNo, RequestModelIOConnRec & Info)
  {
  return FlwLib()!=NULL ? FlwLib()->RequestModelIOConn(pTag, iNo, Info) : FALSE;
  };

//---------------------------------------------------------------------------

int CProject::RequestModelStatus(CModelStatusCriterion &Criteria, CModelStatusArray &Status)
  {
  return FlwLib()!=NULL ? FlwLib()->RequestModelStatus(Criteria, Status) : 0;
  };

//---------------------------------------------------------------------------

int CProject::RequestModelStatusChgCnt(CModelStatusCriterion &Criteria, int MaxTest)
  {
  return FlwLib()!=NULL ? FlwLib()->RequestModelStatusChgCnt(Criteria, MaxTest) : 0;
  };

//---------------------------------------------------------------------------

int CProject::RequestTagRefInfo(LPCTSTR Tag, CXRefInfoArray &Refs) 
  { 
  return FlwLib()!=NULL ? FlwLib()->RequestTagRefInfo(Tag, Refs) : 0;
  };

//---------------------------------------------------------------------------

//int CProject::RequestXRefInfoChg() 
//  { 
//  return FlwLib()!=NULL ? FlwLib()->RequestXRefInfoChg() : 0;
//  };

int CProject::GetRctInfo(CRctBlkInformation & RctInfo)
  {
  return FlwLib()!=NULL ? FlwLib()->FE_GetRctInfo(RctInfo) : 0;
  }

int CProject::SetRctInfo(LPCTSTR NodeTag)
  {
  return FlwLib()!=NULL ? FlwLib()->FE_SetRctInfo(NodeTag) : 0;
  }

//---------------------------------------------------------------------------

int CProject::GetNodeWiring(CNodeWiring &NodeWiring)
  {
  return FlwLib()!=NULL ? FlwLib()->FE_GetNodeWiring(NodeWiring) : -1;
  };

//---------------------------------------------------------------------------

int CProject::GetLinkWiring(CLinkWiring &LinkWiring)
  {
  return FlwLib()!=NULL ? FlwLib()->FE_GetLinkWiring(LinkWiring) : -1;
  };

//---------------------------------------------------------------------------

int CProject::SetLinkWiring(CLinkWiring &LinkWiring)
  {
  return FlwLib()!=NULL ? FlwLib()->FE_SetLinkWiring(LinkWiring) : -1;
  };

//---------------------------------------------------------------------------

flag CProject::FlwModelExists(pchar pTag)
  {
  if (FlwLib()!=NULL)
    if (!FlwLib()->FE_TestModelTagUnique(pTag))
      return true;
  return false;
  }

//---------------------------------------------------------------------------

flag CProject::TestModelTagUnique(pchar pTag, CUniqueTagRulesBlk & TRB/*pchar pHdr, char SepChar*/, Strng &UniqueTag, flag HdrAsIs)
  {
  //int HiNum=0;
  int HiNum=1;
  char * pHdr=TRB.BaseTag();
  char SepChar=TRB.SepChar();

  if (pHdr==NULL)
    pHdr=pTag;

  Strng Hdr;
  if (HdrAsIs)
    Hdr=pHdr;
  else
    {
    for (size_t i=0; i<strlen(pHdr); i++)
      if (isalpha(pHdr[i]))
        //if (isupper(pHdr[i]))
        Hdr+=pHdr[i];
    if (Hdr.Length()==0)
      Hdr=pHdr;
    }

  flag TagFound = 0;
  if (pTag && strlen(pTag)>0)
    {
    //Check Model...
    if (FlwLib()!=NULL)
      if (!FlwLib()->FE_TestModelTagUnique(pTag))
        TagFound=1;
    //Check graphics...
    if (TagFound==0)
      {
      POSITION Pos = ScdApp()->GraphTemplate().GetFirstDocPosition();
      while (Pos && TagFound==0)
        {
        DocRoot* pDoc = (DocRoot*)(ScdApp()->GraphTemplate().GetNextDoc(Pos));
        if (!pDoc->TestModelTagUnique(pTag, pHdr, SepChar, UniqueTag, HdrAsIs))
          TagFound=1;
        }
      }
    //Check visio...
    if (TagFound==0 && ScdApp()->VisioTemplateExists())
      {
      POSITION Pos = ScdApp()->VisioTemplate().GetFirstDocPosition();
      while (Pos && TagFound==0)
        {
        OleDocRoot* pDoc = (OleDocRoot*)(ScdApp()->GraphTemplate().GetNextDoc(Pos));
        if (!pDoc->TestModelTagUnique(pTag, pHdr, SepChar, UniqueTag, HdrAsIs))
          TagFound=1;
        }
      }
    }
  else
    TagFound=1;

  if (!TagFound)
    return True;

  while (TagFound)
    {
    TagFound = 0;
    UniqueTag.Set("%s%c%i", Hdr(), SepChar, HiNum++);

    //Check Model...
    if (FlwLib()!=NULL)
      if (!FlwLib()->FE_TestModelTagUnique(UniqueTag()))
        TagFound=1;
    //Check graphics...
    if (TagFound==0)
      {
      POSITION Pos = ScdApp()->GraphTemplate().GetFirstDocPosition();
      while (Pos && TagFound==0)
        {
        DocRoot* pDoc = (DocRoot*)(ScdApp()->GraphTemplate().GetNextDoc(Pos));
        if (!pDoc->TestModelTagUnique(UniqueTag(), pHdr, SepChar, UniqueTag, HdrAsIs))
          TagFound=1;
        }
      }
    //Check visio...
    if (TagFound==0 && ScdApp()->VisioTemplateExists())
      {
      POSITION Pos = ScdApp()->VisioTemplate().GetFirstDocPosition();
      while (Pos && TagFound==0)
        {
        OleDocRoot* pDoc = (OleDocRoot*)(ScdApp()->GraphTemplate().GetNextDoc(Pos));
        if (!pDoc ->TestModelTagUnique(UniqueTag(), pHdr, SepChar, UniqueTag, HdrAsIs))
          TagFound=1;
        }
      }
    }
  return False;
  }

//---------------------------------------------------------------------------

int CProject::AddNodeModel(pchar ModelType, pchar SubClass, pchar Tag)
  {
  if (!FlwLib())
    return False;
  return FlwLib()->FE_DoInsert(ModelType, SubClass, NULL, Tag, NULL, NULL);
  }

//---------------------------------------------------------------------------

int CProject::DeleteNodeModel(pchar Tag)
  {
  VERIFY(FALSE); //OLD CODE 12/97
  if (!FlwLib())
    return False;
  return FlwLib()->FE_DoDelete(Tag);
  }

//---------------------------------------------------------------------------

int CProject::AddNodeConnect(pchar Type, pchar Tag, pchar SrcTag, pchar SrcOut, pchar DstTag, pchar DstIn)
  {
  if (!FlwLib())
    return False;
  Strng Src, Dst;
  Src.Set("%s.%s", SrcTag, SrcOut);
  Dst.Set("%s.%s", DstTag, DstIn);
  return FlwLib()->FE_DoInsert(Type, NULL, NULL, Tag, Src(), Dst());
  }

//---------------------------------------------------------------------------

int CProject::AddConnect(pchar SrcTag, pchar SrcOut, pchar DstTag, pchar DstIn)
  {
  if (!FlwLib())
    return False;
  Strng Src, Dst;
  Src.Set("%s.%s", SrcTag, SrcOut);
  Dst.Set("%s.%s", DstTag, DstIn);
  return FlwLib()->FE_DoConnect(Src(), Dst());
  }

//---------------------------------------------------------------------------

int CProject::DisConnect(pchar SrcTag, pchar SrcOut, pchar DstTag, pchar DstIn)
  {
  if (!FlwLib())
    return False;
  Strng Src, Dst;
  Src.Set("%s.%s", SrcTag, SrcOut);
  Dst.Set("%s.%s", DstTag, DstIn);
  return FlwLib()->FE_DoDisConnect(Src(), Dst());
  }

//---------------------------------------------------------------------------

int CProject::AddConnect(pchar Src, pchar Dst)
  {
  if (!FlwLib())
    return False;
  return FlwLib()->FE_DoConnect(Src, Dst);
  }

//---------------------------------------------------------------------------

int CProject::DisConnect(pchar Src, pchar Dst)
  {
  if (!FlwLib())
    return False;
  return FlwLib()->FE_DoDisConnect(Src, Dst);
  }

//---------------------------------------------------------------------------

DWORD CProject::get_RecordTag(LPCSTR Tag)
  {
  DWORD RetCode = 0;
  //Note:for driver tags, the recording option must be set in the driver slot configuration file
  CXM_Route HRoute;
  flag HistOn = XFindObject(pExecName_Historian, HRoute);
  if (HistOn)
    {
    Strng WrkTag, WrkCnvTxt;
    TaggedObject::SplitTagCnv((LPSTR)Tag, WrkTag, WrkCnvTxt);
    CXM_HistoryExists *xb=new CXM_HistoryExists (0, WrkTag());
    CXMsgLst XM;
    XM.PackMsg(xb);
    RetCode = XSendMessage(XM, HRoute);
    }
  return RetCode;
  }

//---------------------------------------------------------------------------

flag CProject::put_RecordTag(LPCSTR Tag, flag RecordingOn, DWORD & RetCode)
  {
  RetCode = 0;
  bool bRecorded=0;
  //Note:for driver tags, the recording option must be set in the driver slot configuration file
  CXM_Route HRoute;
  flag HistOn = XFindObject(pExecName_Historian, HRoute);
  if (HistOn)
    {
    Strng WrkTag, WrkCnvTxt;
    TaggedObject::SplitTagCnv((LPSTR)Tag, WrkTag, WrkCnvTxt);
    CXM_HistoryExists *xb=new CXM_HistoryExists (0, WrkTag());
    CXMsgLst XM;
    XM.PackMsg(xb);
    RetCode = XSendMessage(XM, HRoute);
    bRecorded = ((RetCode & RC_SM_HistRecOn)!=0);
    if (RetCode & RC_SM_HistExists)
      {
      flag IsRecordingOn = ((RetCode & RC_SM_HistRecOn)!=0);
      if ((IsRecordingOn!=0)!=(RecordingOn!=0))
        {
        XM.Clear();
        CXM_HistRecordingOn *xb=new CXM_HistRecordingOn (WrkTag(), RecordingOn);
        XM.PackMsg(xb);
        RetCode = XSendMessage(XM, HRoute);
        bRecorded = ((RetCode & RC_SM_HistRecOn)!=0);
        return true; //change made
        }
      return false;
      }

    if (RecordingOn)
      {
      CXM_ObjectTag  ObjTag(WrkTag(), TABOpt_AllInfoOnce);//TABOpt_Parms);
      CXM_ObjectData ObjData;
      CXM_Route      Route;
      if (XReadTaggedItem(ObjTag, ObjData, Route))
        {
        CPkDataItem* pItem = ObjData.FirstItem();
        byte cType = pItem->Type();
        if (IsNumData(cType) || (AllowHistStr && IsStrng(cType)))
          {
          CXMsgLst XM;
          DataUnion Val;
          Val.Set(*(pItem->Value()));
          CXM_KeepHistory *xb=new CXM_KeepHistory (&Val, 0, cType, WrkTag(), 
            pItem->CnvIndex(),
            pItem->CnvTxt(),
            pItem->Description(), 
            0.0, 0.0, 0, 0.0, 0.0, 0.0, 0.0,
            0, 0, 0, 0, True, False);
          XM.PackMsg(xb);
          RetCode = XSendMessage(XM, HRoute);
          if (RetCode)
            {
            bRecorded = ((RetCode & RC_SM_HistRecOn)!=0);
            return true; //change made
            }
          }
        }
      }
    }
  return false;
  }

//===========================================================================
//
//
//
//===========================================================================
