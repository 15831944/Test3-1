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
#include "liccount.h"
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

//const int PrjFileVer = 3; //change for version control when reading .Spj file
//const int PrjFileVer = 4; //window layout now stored in layout.ini, not xxx.Spj; changed 14/5/96
//const int PrjFileVer = 5; //CNM Relative Filenames fixed - allows project name change in FileManager etc.; changed 23/8/96
//const int PrjFileVer = 6; //KGA reorganised toolbars including toolbar saves 10/10/96
//const int PrjFileVer = 7; //KGA added buttons and reorganised toolbar IDs 18/10/96
//const int PrjFileVer = 8; //KGA changed method for saving toolbars (not name dependent) 25/10/96
//const int PrjFileVer = 9; //KGA changed toolbar IDs used for saving toolbars 28/10/96
//const int PrjFileVer = 10; //CNM Added SolverStart Button
//const int PrjFileVer = 11; //CNM Rearrangement of toolbars
//const int PrjFileVer = 12; //KGA Rearrangement of how document names are stored in spj AND Z-Order now in layout.ini 20/2/97 (SysCAD 7.1 Rev 5.2)
//const int PrjFileVer = 13; //KGA Added list of files used by project to spj 4/3/97 (SysCAD 7.1 Rev 5.4)
//const int PrjFileVer = 14; //CNM Added the Concept of invalid numeric tags (SysCAD 7.1 Rev 7.0)
//const int PrjFileVer = 15; //KGA Extened/changed 'Bad Numeric Tag' functionality 29/10/97 (SysCAD 7.1 Rev 8.13)
//const int PrjFileVer = 16; //CNM Tweaked Load to Change FlwSolve Tag to PlantModel 20/07/98 (SysCAD 7.2 Rev 2.7)
//const int PrjFileVer = 17; //CNM Rearranged Directory Structure 01/08/98 (SysCAD 7.2 Rev 2.?)
//const int PrjFileVer = 18; //CNM Rearranged Objects Flowsolver 04/11/98 (SysCAD 7.2 Rev 3.6)
//const int PrjFileVer = 19; //CNM Rearranged Specie Quality Vars 07/05/99 (SysCAD Bld 11)
//const int PrjFileVer = 20; //CNM Allow PID2/PID3 Selection
//const int PrjFileVer = 21; //CNM Default Database = MDB
//const int PrjFileVer = 22; //CNM Change ReactionBlkBuildDataDefn 11/99
//const int PrjFileVer = 23; //CNM Change SizeDistColumnNames 12/99
//const int PrjFileVer = 24; //CNM Fix FB_Eqn & FB_Data 02/2000
//const int PrjFileVer = 25; //CNM Tweak the way toolbar positions are saved/recovered
//const int PrjFileVer = 26; //CNM Extention to control User Functions for water
//const int PrjFileVer = 27; //CNM Added Orig Project VerNo
//const int PrjFileVer = 28; //CNM Added Filing Changes/Fixes --
//const int PrjFileVer = 29; //CNM Added Filing Changes/Fixes -- Oops 28 Sent to KGA too early
//const int PrjFileVer = 30; //KGA Rearrangement of toolbars
//const int PrjFileVer = 31; //CNM $ changes : Project name, path, etc
//const int PrjFileVer = 32; //CNM PB Pressure mods
//const int PrjFileVer = 33; //CNM Run/Idle Mode and PB Pressure mods in FlashTank
//const int PrjFileVer = 34; //CNM Viscosity Upgrade - Flw drops now use calcd viscosity
//const int PrjFileVer = 35; //CNM FB tagging changed
//const int PrjFileVer = 36; //CNM FB tagging changed .. and again
//const int PrjFileVer = 37; //CNM Fix for probelm in cfg file (specie annotation)
//const int PrjFileVer = 38; //CNM Entrainment / Link Data Storage
//const int PrjFileVer = 39; //CNM Fix for "Null FlwEqn"
//const int PrjFileVer = 40; //CNM Fix for FlwEqn Storage
//const int PrjFileVer = 41; //CNM Change to H2O Eqn
//const int PrjFileVer = 42; //CNM Fix For PressDropKFact
//const int PrjFileVer = 43; //CNM Extention for ActiveHold in TagObj
//const int PrjFileVer = 44; //CNM Changes to SpShape
//const int PrjFileVer = 45; //CNM Restructure SpConduit / SpContainer / SpImage .... (Build57)
//const int PrjFileVer = 46; //CNM HeatX1 Restructure ....
//const int PrjFileVer = 47; //CNM Pipe/HX Join VolFlwFX Init/Restructure ... (Build58)
//const int PrjFileVer = 48; //KGA 18/9/01 Tag changes in Filters...
//const int PrjFileVer = 49; //KGA 20/9/01 Tag changes in Washers...
//const int PrjFileVer = 50; //KGA  2/2/02 Rearrangement of toolbars
//const int PrjFileVer = 51; //KGA 11/3/02 Removed option to load specie data from default.mdb
//const int PrjFileVer = 52; //CNM 12/3/02 BuildDataDefn options changed from 32 to 64 bits.
//const int PrjFileVer = 53; //CNM 26/3/02 Tolerance changes.
//const int PrjFileVer = 54; //CNM 23/4/02 Time management.
//const int PrjFileVer = 55; //CNM 23/5/02 More Time management.
//const int PrjFileVer = 56; //CNM  4/6/02 Tweaks to RhoH Finder and other flownets stuff (Build71)
//const int PrjFileVer = 57; //CNM  6/6/02 ProBal Press control improvements
//const int PrjFileVer = 58; //CNM 14/6/02 Tweaks to MinLevel -> LowestIO
//const int PrjFileVer = 59; //CNM 20/6/02 Tweaks to VolFlwFX
//const int PrjFileVer = 60; //CNM 19/8/02 Tweaks to EvalOrder
//const int PrjFileVer = 61; //CNM 19/8/02 Flow Properties Fwd/Rev with options
//const int PrjFileVer = 62; //CNM 05/02/03 Tweaks to PowerSupplies | Ver8.2 26/5/03: IdealGas Flags; New NormPress Value
//const int PrjFileVer = 63; //CNM 03/03/03 Tweaks to PowerSupplies
//const int PrjFileVer = 64; //CNM 09/04/03 PowerDistribution
//const int PrjFileVer = 65; //CNM 23/05/03 IdealGas Flags
//const int PrjFileVer = 66; //CNM 04/06/03 AlcanSpModel Tweaks;
//const int PrjFileVer = 67; //CNM 10/09/03 ADO Tweaks;
//const int PrjFileVer = 68; //CNM 10/09/03 More ADO Tweaks - force a SaveAs;
//const int PrjFileVer = 69; //CNM 10/09/03 NdFlngs etc tag change - force a SaveAs;
//const int PrjFileVer = 70; //CNM 1/03/04 CfgFile path changes;
//const int PrjFileVer = 71; //CNM 16/03/04 Environment Changes;
//const int PrjFileVer = 72; //CNM 11/05/04 Fixes for new DynModes;
//const int PrjFileVer = 73; //CNM 11/05/04 Fixes for new Heat & DynModes;
//const int PrjFileVer = 74; //CNM 06/07/04 Final Fixes for SysVector Tags chages etc;
//const int PrjFileVer = 75; //CNM 06/08/04 Change the way SpAttributes are stored
//const int PrjFileVer = 76; //CNM 09/10/2004 Change the way SpAttributes are stored
//const int PrjFileVer = 77; //CNM 22/11/2004 Change the Way Symbols are stored
//const int PrjFileVer = 78; //CNM 11/01/2005 Change Tear Init Option Names
//const int PrjFileVer = 79; //KGA 02/06/2005 AlcanSpModel Tweaks for organics/oxalate
//const int PrjFileVer = 79; //CNM 15/03/2005 Vars Selection in SpModel & Frac defaults in Reactions

////const int PrjFileVer = 81; //KGA 16/06/2005 Further changes to Vars Selection in SpModel
////const int PrjFileVer = 82; //KGA 14/07/2005 Shell&Tube condensing duty limit change
//const int PrjFileVer = 83; //CNM 18/08/2005 FlowMode changes

//const int PrjFileVer = 81; //KGA 16/06/2005 Further changes to Vars Selection in SpModel
//const int PrjFileVer = 82; //KGA 14/07/2005 Shell&Tube condensing duty limit change

//const int PrjFileVer = 80; //CNM 12/04/2005 Changes for Leaks etc
//const int PrjFileVer = 81; //KGA 16/06/2005 Further changes to Vars Selection in SpModel
//const int PrjFileVer = 82; //KGA 14/07/2005 Shell&Tube condensing duty limit change
//const int PrjFileVer = 83; //CNM 18/08/2005 FlowMode changes
// Spares
//const int PrjFileVer = 90; //CNM 06/10/2005 Changes for Leaks etc REWORK due to divergent paths
//const int PrjFileVer = 91; //CNM 02/10/2005 Fix 4Port Valve to have name convention save as valve
//const int PrjFileVer = 92; //CNM 31/10/2005 Remove "Output" from PID
//const int PrjFileVer = 93; //CNM 15/11/2005 Fix 4PortValve
//const int PrjFileVer = 94; //CNM 11/11/2005 FlowMode fixes
//const int PrjFileVer = 95; //CNM 19/01/2006 remove table FB_Eqn from Database
const int PrjFileVer = 96; //CNM 19/01/2006 Changed 'Location' to 'PlantArea'

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
    dbgpln("%-20.20s : %s", "CfgHome"              ,CfgHome()        ? CfgHome()        : "");
    dbgpln("%-20.20s : %s", "PrjFile"              ,PrjFile()        ? PrjFile()        : "");
    dbgpln("%-20.20s : %s", "PrjFiles"             ,PrjFiles()       ? PrjFiles()       : "");
    dbgpln("%-20.20s : %s", "PrjRmtCpyFiles"       ,PrjRmtCpyFiles() ? PrjRmtCpyFiles() : "");
    dbgpln("%-20.20s : %s", "PrjName"              ,PrjName()        ? PrjName()        : "");
    }
  SetUsePreviousCfgAlias(C);
  SetUsePreviousPrjAlias(P);
  }

//===========================================================================

#define WithRmtFile 0

char* CWindowLists::MainWndTitle = "SysCAD 9.0 © Kenwalt";
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
      pDoc->SetPathName(Fn());
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

Project * gs_pPrj=NULL;
bool Project::fOpenAltCfg=false;
PrjLoadTypes Project::m_LoadTypeRqst=PLT_Null;

//---------------------------------------------------------------------------

bool Project::m_SysCADInited=false;

Project::Project()
  {

  SetNewPrjFileVerNo(PrjFileVer);

#if WITHSCRCYCLES
  Cycles = NULL;
  CycleWnds = NULL;
  CyclePos = NULL;
  pCycleDescWnd = NULL;
#endif

  pFlwLib = NULL;
  Clear();

  EO_Register(pExecName_Project, EOExec_None, 0, 0);
  SetPrjFileVerNoAsLoaded(PrjFileVer);

  m_bWithVersion = WithVersionDefault;
  m_bRemoveOld   = RemoveOldDefault;
  }

//---------------------------------------------------------------------------

Project::~Project()
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

void Project::Clear()
  {
  bForceSaveAsNV=false;
  bDoingSaveAs = false;
  bDoingSaveAsNV = false;
  bDoingSaveAsWithName = false;
  bDoingLoad = false;
  pPrjDoc=NULL;
//  sPrjFile="";
  bCmdSuccess=0;
  bDrvOpen=0;
  bDrvOn=0;
  bDrvLclTagSrvrOK=1;
  bDrvReadAll=0;
  bArcOpen=0;
  bArcOn=0;
  bArcOpenDBOnRun=0;
  bIOMOpen=0;
  bIOMOn=0;

  bDDEOn=0;
  pDDESrvr=NULL;
  m_bOPCOn=0;
  m_bOPCResetReg=0;
  m_iOPCServerNo=0;
  //;pOPCSrvr=NULL;
  bGrfDeferLoad=0;
  bGrfAutoLoad=0;
  bGrfUpdateAnnot=1;
  bGrfAnimationOn=1;
  bGrfPromptForZoom=1;
  bGrfMoveCursor=1; // CNM
  bGrfRegExpOn=0;
  bReadOnlyPrj=0;
  bHstOK=0;
  bCommsOpen=0;
  bAccAutoSaveOn=0;
  bAccUpdateOnRun=1;
  bAccHistoryInfoVis=0;
  gs_Exec.SetRealTime(0);
  gs_Exec.SetRealTimeMult(1);
  gs_Exec.SetStepSizeMax(1.0);
  gs_Exec.SetTime(0.0);
  gs_Exec.SetRunTermination(RTRM_None);
  gs_Exec.SetTimeUntilStop(3600.0);
  gs_Exec.SetMaxRunSteps(1);

  m_sDefGrpLib      = "$BaseCfgFiles\\GroupLibrary.MDB";
  m_sGrfFrameName   = "Extents-A3";
  m_sGrfFrameFilter = "Frames:";

  sDrvManagerName="Scd_Drvr.mdb";
  sArcManagerName="ScdArch.sac";
  sIOMarshalName="*.scm";
  sIOMarshalNode="";
  bDrvIgnoresOn=0;
  bDrvForcesOn=0;
  bDrvNoiseOn=1;
  bDrvStartFBKCheck=FBKCheckNever;
  lStatsRefreshCnt=5L;
  HstMngr.SetHstOn(true);
  ReportInfo.iHSearchLen = 10;
  ReportInfo.iVSearchLen = 100;
  ReportInfo.sNan = "*";
  ReportInfo.sBlank = "";
  ReportInfo.sIgnoreChars = "=";
  bRptExcelLock=1;
  bRptExcelCellName=1;
  bRptExcelMakeActive=1;
  bRptExcelSysCADActive=1;
  bRptExcelUpdateLinks=1;
  bRptExcelSaveOnComplete=1;
  bRptSyscadTags=1;
  bShowStatusColours=1;
  m_dwMdlStatusOptions = 0;
  //iOverlayStatusPen=PS_DASH;
  bTimeChanged=0;
  bHstChanged=0;
  bDrvChanged=0;
  bArcChanged=0;
  bDocChanged=0;
  bLoadBusy=0;
  bConfigBusy=0;
  bDefaultRecordIt=1;
  bFlashTrend=1; // CNM - most people who see this seem to like it !!!
  bShowNewTrndLineDlg=0;
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
  eSSAction = SSA_Prompt;//SSA_AutoInc;
  sSSName = "SnapShot";
  sSSPrefix = "Snap_";
  iSSNumber = 0;
  bSSDoZip = 0;
  bToolTips=1;
  bFlyBys=1;
  gs_Exec.SetGlblRunModes(SM_Direct, SM_All);
  Solver.fStateWnd=true;
  Solver.fStatePinned=false;
  Solver.fStateErrors=false;
  Solver.iStateX=-10000;
  Solver.iStateY=-10000;

  m_bRunOptEmptyAll=0;
  m_bRunOptEmptySpills=0;
  m_bRunOptZeroFlows=0;    
  m_bRunOptInitialiseAll=0;
  m_bRunOptResetStats=0;   
  m_bRunOptOnceOnly=0;    

#if WITHNETSERVER
  pCS_Mngr->SetServer(false);
  pCS_Mngr->SetClientConnect(false, "", false);
#endif
  MdlLibs.UnLoadFlwDLLs();
  fNewTrend=true;
  fNewGraph=true;
  //CMdlValueSet::Clear();
  CRptTagLists::ShutDown();

  XSetDBFormat(ScdPFUser.RdInt("General", "PrjsDBFormat", DBConnect_JET2000));

  m_bRemoveRevs=true;
  m_bCopyDBFiles=true;

  m_Save.m_bRqstNew=0;
  m_Save.m_bIsNewPrj=0;

  InitColours();

  SetPrjFileVerNo(PrjFileVer);
  SetNewPrjFileVerNo(PrjFileVer);
  SetOrigPrjFileVerNo(PrjFileVer);
  SetPrjFileVerNoAsLoaded(PrjFileVer);
  //m_PrjFileVerNoAsLoaded=PrjFileVer;

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
  //m_RmtCpyLock.Close();
  }

//---------------------------------------------------------------------------

void Project::SetShowEditStatus(bool On)
  {
  if (On)
    m_dwMdlStatusOptions &= ~MSO_ShowRunInEdit;
  else
    m_dwMdlStatusOptions |= MSO_ShowRunInEdit;
  if (gs_pTheSFELib)
    gs_pTheSFELib->FE_LoadAllModelStatus();
  }

bool Project::ShowEditStatus()
  {
  return (m_dwMdlStatusOptions & MSO_ShowRunInEdit)==0;
  }

//---------------------------------------------------------------------------

void Project::InitColours()
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

void Project::SaveColours(CProfINIFile & PF)
  {
  PF.WrLong("StatusColours", "GrfShowStatusColours", bShowStatusColours);
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

void Project::LoadColours(CProfINIFile & PF)
  {
  bShowStatusColours = (PF.RdLong("StatusColours", "GrfShowStatusColours", bShowStatusColours) != 0);
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

flag Project::Initialise(char * pPrjName)
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

int Project::ShutDown()
  {
  bDocChanged = true; //force project save
  return DoClose(true, true);
  }

//---------------------------------------------------------------------------

//void Project::SetSolveMode(long Md)
//  {
//  //fProBalMode=PB;
//  gs_Exec.SetSolveMode(Md);//PB ? SM_Direct : SM_DynCurrent);
//  }

//flag Project::GetProBalMode()
//  {
//  return (gs_Exec.GlblProbalMode());
//  }

//---------------------------------------------------------------------------

//void Project::SetDynModeFlow(flag DFlow)
//  {
//  gs_Exec.SetDynMode(DFlow ? DYNFLOWMODE : DYNFULLMODE);
//  }
//
//flag Project::GetDynModeFlow()
//  {
//  return (gs_Exec.GetSolveMode()==DYNMODE && gs_Exec.GetDynMode()==DYNFLOWMODE);
//  }

////---------------------------------------------------------------------------
//
//flag Project::CheckForDuplicateClassIds()
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

void Project::GetSplitInfo(CProfINIFile& PF, CWnd* w, pchar Section, int &SplitCount)
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

void Project::SaveOneWindow(int iNo, LPCTSTR pWindowName, CWnd* pWnd, flag TheMain)//, CProfINIFile & PF)
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

void Project::SaveWindowLayout()
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

void Project::PutSplitInfo(CProfINIFile& PF, CWnd* w, pchar Section, int& SplitCount)
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

flag Project::PutFrameInfo(CProfINIFile& PF, CWnd* w, pchar Section, Strng& s, CWnd** WndList, int& WndListLen)
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

void Project::RestoreMainWindow()
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
  if (PrjFileVerNo()<34)
    {
    pM->DefaultToolBar(-1, TRUE);
    /*const char * OldIni=ScdApp()->m_pszProfileName;
    ScdApp()->m_pszProfileName = PrjFile();
    pM->LoadBarState("ToolBars");
    ScdApp()->m_pszProfileName = OldIni;*/
    }
  else
    {
    /*const char* OldIni = ScdApp()->m_pszProfileName;
    ScdApp()->m_pszProfileName = PrjFile();
    pM->LoadBarState("NTB");
    ScdApp()->m_pszProfileName = OldIni;*/

    // Command IDs Changed which break this restore - accept defaults
    if (PrjFileVerNo()>=50)
      {
      Strng s = PrjFiles();
      s += LayoutFileName;
      pMainTBMngr->LoadState(s());
      pMainTBMngr->LoadState(s()); //call twice to ensure positions are correct
      }
    else
      pM->DefaultToolBar(-1, TRUE); // force defaults
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

void Project::RestoreOneWindow(LPCTSTR pWindowName, CWnd* pWnd, flag TheMain, flag FullRestore)
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

void Project::RestoreWindowZOrder()
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
////typedef long (*fn_LoadDBCallBack)(long When, pvoid pData);
//long FnLoadDBCallBack(long When, pvoid pData)
//  {
//  Project * gs_pPrj=(Project*)pData;
//  return gs_pPrj->LoadDBCallBack(When);
//  };
//
//
//long Project::LoadDBCallBack(long When)
//  {
//  switch (When)
//    {
//    case 1: // Tags Created
//      break;
//    case 2: // Conns Made
//      break;
//    case 3: // Tags Loaded
//      OpenDrvManager(); //must be loaded after models (tag data base) BUT before 'Other.Dat'
//      OpenArcManager(); //must be loaded after models (tag data base) BUT before 'Other.Dat'
//      break;
//    case 4: // OtherData Loaded
//      break;
//    }
//  return 0;
//  }

//---------------------------------------------------------------------------

flag Project::DoLoadDB(char* pPrjFiles, char* pOldPrjName)
  {
  gs_AccessWnds.CloseAccessData(-1, false, false, true);
  gs_AccessWnds.CloseWnd(-1);

  int Fmt=gs_Exec.LoadDBase(1, DBConnect_Find, DBO_ForRead, pPrjFiles);
  OpenDrvManager(); //must be loaded after models (tag data base) BUT before 'Other.Dat'
  OpenArcManager(); //must be loaded after models (tag data base) BUT before 'Other.Dat'
  OpenIOMarshal(); //???
  gs_Exec.LoadDBase(2, DBConnect_Find, DBO_ForRead, pPrjFiles);
  //if (Fmt & DBO_Fmts)
  //  {
    //m_DBFormat=Fmt & DBO_Fmts;
    XSetDBFormat(Fmt);
  //  }
  return true;
  }

//---------------------------------------------------------------------------

flag Project::DoSaveDB(int SaveDBFmt, int SaveOpts, char* pPrjFiles, char* pOldPrjFiles)
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

int Project::FixSnapshot(pchar FixFn)
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

flag Project::DoLoadSnapShotScenario(flag DoSnapShot, pchar pName)
  {
  CWaitMsgCursor Wait(DoSnapShot ? "Loading SnapShot" : "Loading Scenario");
  Strng Fn;//, Ext;
  Fn = pName;
  if (DoSnapShot && Fn.Length()==0)
    Fn = sSSName;
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

flag Project::DoSaveSnapShotScenario(flag DoSnapShot, pchar pName, CExecObjArray *ObjList)
  {
  CWaitMsgCursor Wait(DoSnapShot ? "Saving SnapShot" : "Saving Scenario");
  Strng Fn, Ext;
  Fn = pName;
  if (DoSnapShot && Fn.Length()==0)
    Fn = sSSName;
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
  if (bSSDoZip)
    {
    int ret = CZipFile::ZipOne(pName, true);
    if (ret!=0)
      LogError("ZipFile", LF_Exclamation, "Unable to create zip file from '%s'", pName);
    }
  #endif
  if (DoSnapShot)
    {
    CProfINIFile PF(PrjIniFile());
    PF.WrLong("SnapShot", "Action", eSSAction);
    PF.WrStr("SnapShot", "Name", sSSName());
    PF.WrStr("SnapShot", "Prefix", sSSPrefix());
    PF.WrLong("SnapShot", "Number", iSSNumber);
    }
  gs_pCmd->Print("%i Objects Saved for %s\n", n, DoSnapShot ? "SnapShot" : "Scenario");
  return true;
  }

//---------------------------------------------------------------------------

void Project::OnFileSavesnapshot()
  {
  flag SaveSS = true;
  Strng s;
  switch (eSSAction)
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
        if (bSSDoZip)
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
      s.Set("%s%03d", sSSPrefix(), iSSNumber);
      iSSNumber++;
      break;
      }
    case SSA_ReUse:
      s = sSSName;
      break;
    }
  if (SaveSS)
    {
    m_LastSnapLoadFn=s();
    DoSaveSnapShotScenario(true, s());
    }
  }

//---------------------------------------------------------------------------

void Project::OnFileLoadsnapshot()
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

void Project::OnFileSavescenario()
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

void Project::OnFileLoadscenario()
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

flag Project::OnConfigure()
  {
  if (gs_Exec.TestRunning())
    return false;

  if (!bConfigBusy) // will be cleared by closing dialog
    {
    if (MainWnd()->m_pPrjSheet)
      MainWnd()->m_pPrjSheet->BringWindowToTop();
    else
      {
      CProfINIFile PF(PrjIniFile());
      MainWnd()->m_pPrjSheet = new CPrjSheet("Project", MainWnd(), PF.RdInt("General", "PrjOptionsPageNo", 0));
      }
    }

  return true;
  }

//---------------------------------------------------------------------------

flag Project::OnLoadAltCfg()
  {
  if (gs_Exec.TestRunning())
    return false;
  return true;
  }

//---------------------------------------------------------------------------

void Project::OpenHistorian()
  {
  bHstOK=0;
  HstMngr.SetHstOpen(0);//make sure is correct
  HstMngr.SetExecObj(this);//make sure is correct
  HstMngr.SetLicensingOptions(gs_License.MaxHistSizeAllowed(), gs_License.MaxHistFilesAllowed());
  if (HstMngr.HstOn())
    {
    CWaitMsgCursor Wait("Opening historian");
    if (HstMngr.DoOpenHistorian(true))
      bHstOK=1;
    }
  }

//---------------------------------------------------------------------------

void Project::DeleteHistorian()
  {
  if (HstMngr.DoDeleteHistorian())
    CTagVwDoc::RebuildAll(); //force all trends to reconnect etc
  }

//---------------------------------------------------------------------------

void Project::RestartHistorian()
  {
  HstMngr.DoRestart();
  }

//---------------------------------------------------------------------------

void Project::CloseHistorian(BOOL ForExit/*=FALSE*/)
  {
  if (HstMngr.HstOpen())
    {
    if (ForExit)
      HstMngr.DoClose();
    else
      {
      CWaitMsgCursor Wait("Closing historian");
      CDlgBusy::Open("\n\nClosing Historian");
      HstMngr.DoClose();
      CDlgBusy::Close();
      }
    }
  HstMngr.SetExecObj(NULL);
  }

//---------------------------------------------------------------------------

flag Project::IsHistorianOpen()
  {
  return HstMngr.IsOpen();
  }

//---------------------------------------------------------------------------

void Project::OpenDrvManager()
  {
  bDrvOpen=0;
  if (bDrvOn && sDrvManagerName.Length()>0)
    {
    if (gs_License.AllowDrivers())
      {
      CWaitMsgCursor Wait("Opening driver manager");
      bDrvOpen=1;
      Strng Name, DrvDir, FileExt;
      Name.FnSearchExpand(sDrvManagerName(), AF_All|AF_BackupFiles);
      DrvDir.FnDrivePath(Name());
      FileExt.FnNameExt(Name());

      gs_pDrvMan->Configure(DrvDir(), FileExt(), (byte)bDrvIgnoresOn, (byte)bDrvForcesOn, (byte)bDrvReadAll, (byte)bDrvNoiseOn, bDrvStartFBKCheck);
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

void Project::CloseDrvManager(BOOL ForExit/*=FALSE*/, BOOL ForProject/*=FALSE*/)
  {
  if (bDrvOpen)
    {
    CWaitMsgCursor Wait("Closing driver manager");
    gs_pDrvMan->Close(!(ForExit || ForProject));
    bDrvOpen=0;
    }
  }

//---------------------------------------------------------------------------

flag Project::ReloadDrvManager(BOOL WithOptions, char * NewDrvManagerName)
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

    CloseArcManager();
    ASSERT(_CrtCheckMemory());
    CloseDrvManager();

    if (NewDrvManagerName)
      sDrvManagerName=NewDrvManagerName;
    ASSERT(_CrtCheckMemory());
    OpenDrvManager();
    ASSERT(_CrtCheckMemory());
    OpenArcManager();
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

//---------------------------------------------------------------------------

void Project::OpenArcManager()
  {
  bArcOpen=0;
  if (bArcOn && sArcManagerName.Length()>0)
    {
    if (1)//gs_License.AllowDrivers())
      {
      CWaitMsgCursor Wait("Opening Archive manager");
      bArcOpen=1;
      Strng Name;
      Name.FnSearchExpand(sArcManagerName(), AF_All|AF_BackupFiles);
      //ArcDir.FnDrivePath(Name());
      //FileExt.FnNameExt(Name());

      gs_pArcMan->Configure(Name(), bArcOpenDBOnRun);
      if (gs_pArcMan->Open()!=0)
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

void Project::CloseArcManager(BOOL ForExit/*=FALSE*/, BOOL ForProject/*=FALSE*/)
  {
  if (bArcOpen)
    {
    CWaitMsgCursor Wait("Closing Archive manager");
    gs_pArcMan->Close(!(ForExit || ForProject));
    bArcOpen=0;
    }
  }

//---------------------------------------------------------------------------

flag Project::ReloadArcManager(BOOL WithOptions, char * NewArcManagerName)
  {
  if (bArcOn)
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
    CloseArcManager();
    if (NewArcManagerName)
      sArcManagerName=NewArcManagerName;
    ASSERT(_CrtCheckMemory());
    OpenArcManager();
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

void Project::OpenIOMarshal()
  {
  bIOMOpen=0;
  if (bIOMOn && sIOMarshalName.Length()>0)
    {
    if (gs_License.AllowDrivers())
      {
      CWaitMsgCursor Wait("Opening IOMarshal");
      bIOMOpen=1;

      gs_pIOMarshal->Configure(sIOMarshalName(), sIOMarshalNode());//, (byte)bDrvIgnoresOn, (byte)bDrvForcesOn, (byte)bDrvReadAll, (byte)bDrvNoiseOn, bDrvStartFBKCheck);
      if (gs_pIOMarshal->Open()!=0)
        {
        LogError("Driver", LF_Exclamation, "Error Opening IOMarshal");
        bIOMOpen=0;
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

void Project::CloseIOMarshal(BOOL ForExit/*=FALSE*/, BOOL ForProject/*=FALSE*/)
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

flag Project::ReloadIOMarshal(BOOL WithOptions, char * NewIOMarshalName, char * NewIOMarshalNode)
  {
  if (bIOMOn)
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
    CloseArcManager();

    if (NewIOMarshalName)
      sIOMarshalName=NewIOMarshalName;
    if (NewIOMarshalNode)
      sIOMarshalNode=NewIOMarshalNode;
    ASSERT(_CrtCheckMemory());
    OpenIOMarshal();
    ASSERT(_CrtCheckMemory());
    OpenArcManager();
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
void Project::OpenNetManager()
  {
  CWaitCursor Wait;
  CDlgBusy::Open("\n\nOpening Network Manager");
  if (pCS_Mngr->Open()!=0)
    LogError("Network", LF_Exclamation, "Error Opening Network Manager");
  CDlgBusy::Close();
  bNetOpen=1;
  }

//---------------------------------------------------------------------------

void Project::CloseNetManager(BOOL ForExit/*=FALSE*/)
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

void Project::OpenDDEManager()
  {
  if (bDDEOn)
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

void Project::CloseDDEManager(BOOL ForExit/*=FALSE*/)
  {
  if (pDDESrvr)
    {
    pDDESrvr->CloseSrvr();
    delete pDDESrvr;
    pDDESrvr = NULL;
    }
  }

//---------------------------------------------------------------------------

//void Project::OpenOPCManager()
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

//void Project::CloseOPCManager(BOOL ForExit/*=FALSE*/)
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

BOOL Project::FindTag(LPCTSTR pTag/*=""*/, BOOL DoAutoAccess/*=FALSE*/, BOOL NoErrDlg/*=FALSE*/, BOOL FindNext/*=FALSE*/)
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

  gs_pPrj->bDoGrfAutoAccess = DoAutoAccess;
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
              gs_pPrj->bDoGrfAutoAccess = DoAutoAccess;
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
      gs_pPrj->bDoGrfAutoAccess = DoAutoAccess;
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

BOOL Project::AllGrfLoaded()
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

void Project::StoreWndList(CWnd* w, flag DoIt)
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

void Project::StartScreenSave()
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

void Project::RotateCycle(CWnd* w, CWnd** Wnds, int& WndCnt)
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

void Project::RotateScreenSave()
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

void Project::EndScreenSave()
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

void Project::RestoreWnds()
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

void Project::BrowseTags()
  {
  HstMngr.BrowseTags();
  }

//---------------------------------------------------------------------------

void Project::HistorianQuery()
  {
  if (1)
    {
    LogError("History", 0, "Query all tags to csv file not implemented yet...");
    }
  else
    {
    double EndTime = gs_Exec.TimeCB().m_Time;
    double Interval = 60.0;
    double Duration = 24.0*60*Interval;
    double StartTime = EndTime - Duration;
    Strng Filename = "c:\\test_qry";
    HstMngr.QueryToFile(Filename(), StartTime, Duration, Interval);
    }
  }

//---------------------------------------------------------------------------

#if CK_USECRYPKEY
const int CK_DoFullCheck = (int)((double)RAND_MAX * 0.05); //do complete full check 5% of time
const int CK_DoCheck = (int)((double)RAND_MAX * 0.12); //do full check 12% of time
static DWORD PrevCheck = GetTickCount();
#else
void CheckLicenseDate()
  {
  const int i = rand();
  if (i<(int)((double)RAND_MAX * 0.30))
    {//crude check if software is legal
    SYSTEMTIME ST, ET;
    GetSystemTime(&ST);
    FILETIME FT;
    if (FnCreateTime(ExeFile(), FT))
      {
      if (FileTimeToSystemTime(&FT, &ET))
        {
        long a = ST.wYear*365+ST.wMonth*30+ST.wDay;
        long b = ET.wYear*365+ET.wMonth*30+ET.wDay;
        if (a-b>202 || a-b<-33)
          {
          Sleep(5555);
          _asm int 3;
          }
        }
      }
    }
  }
#endif

void Project::CheckLicense(BOOL StartingSolve)
  {
  #if CK_USECRYPKEY
  if (!gs_License.Blocked())
    {
    if (StartingSolve && (gs_License.DemoMode() || gs_License.TrialMode()))
      AfxMessageBox(gs_License.DemoMode() ? "SysCAD is unlicensed !\n\nCurrently using Demo mode." : "SysCAD is unlicensed !\n\nCurrently using Trial version.");

    //make occasional call to crypkey library/dll...
    const int i = rand();
    if (i<CK_DoFullCheck)
      gs_License.QuickCheck(2);
    else if (i<CK_DoCheck)
      gs_License.QuickCheck(1);
    else
      gs_License.QuickCheck(0);
    if (GetTickCount() - PrevCheck>600000) //if conditions haven't been checked for 600 seconds, check...
      CheckLicenseConditions();
    }
  #else
  CheckLicenseDate();
  #endif
  }

//---------------------------------------------------------------------------

void Project::CheckLicenseConditions()
  {
  #if CK_USECRYPKEY
  PrevCheck = GetTickCount();
  if (!gs_License.Blocked())
    {
    flag Block = 0;
    if (!gs_License.AllowDrivers() && bIOMOn)
      {
      LogWarning("SysCAD", 0, "License does not allow drivers");
      Block = 1;
      }
    if (!gs_License.AllowDrivers() && bDrvOn)
      {
      LogWarning("SysCAD", 0, "License does not allow drivers");
      Block = 1;
      }
    //if (!gs_License.AllowOPCServer() && bOPCOn)
    //  {
    //  LogWarning("SysCAD", 0, "License does not allow OPC Server");
    //  //Block = 1; don't block, instead don't load
    //  CloseOPCManager();
    //  }
    /*if (!gs_License.AllowFullHist() && HstMngr.LicenseExceeded(gs_License.MaxHistSizeAllowed(), gs_License.MaxHistFilesAllowed()))
      {
      LogWarning("SysCAD", 0, "Historian data files exceed limit allowed by license");
      Block = 1;
      }*/
    if (gs_License.TrendWindowsAllowed()!=CK_InfiniteTrends && CWindowLists::GetTrendWndCount()>gs_License.TrendWindowsAllowed())
      {
      LogWarning("SysCAD", 0, "Maximum number of trend windows allowed exceeds limit of %d allowed by license", gs_License.TrendWindowsAllowed());
      Block = 1;
      }
    if (gs_License.GraphicWindowsAllowed()!=CK_InfiniteGrfs && CWindowLists::GetGrfWndCount()>gs_License.GraphicWindowsAllowed())
      {
      LogWarning("SysCAD", 0, "Maximum number of graphics windows allowed exceeds limit of %d allowed by license", gs_License.GraphicWindowsAllowed());
      Block = 1;
      }
    const int iUnitsAllowed = (gs_Exec.GlblProbalMode() ? gs_License.ProbalUnitsAllowed() : gs_License.DynUnitsAllowed());
    if (iUnitsAllowed!=CK_InfiniteUnits && gs_pPrj->pFlwLib)
      {
      gs_LicenseCnt.CalcUnits();
      if (gs_LicenseCnt.NoOfUnits()>iUnitsAllowed)
        Block = 1;
      }
    if (gs_pPrj->pFlwLib)
      {
      gs_LicenseCnt.CalcLicUnits();
      if (gs_LicenseCnt.NoOfIllegalUnits()>0)
        Block = 1;
      }

    if (Block)
      {
      LogError("SysCAD", LF_Exclamation, "Current project exceeds license limits (see messages)\n\nThe majority of SysCAD commands and functions have been disabled.\n\nPlease exit SysCAD. (Save project if required)");
      gs_License.SetBlocked();
      AfxGetMainWnd()->PostMessage(WMU_UPDATEMAINWND, SUB_UPDMAIN_BACKGROUND, 0);
      }
    }
  #endif
  }

//---------------------------------------------------------------------------

int Project::PrepareMerge(CMergeProjectsInfo& MPI)
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

flag Project::Merge()
  {
  gs_AccessWnds.CloseAccessData(-1, false, false, true);
  gs_AccessWnds.CloseWnd(-1);

  return ImportFlwsheet();
  }

//---------------------------------------------------------------------------

flag Project::ImportFlwsheet()
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
    #if CK_USECRYPKEY
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
    const int iUnitsAllowed = (gs_Exec.GlblProbalMode() ? gs_License.ProbalUnitsAllowed() : gs_License.DynUnitsAllowed());
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
      CloseArcManager();
      CloseDrvManager();

      OpenDrvManager();
      OpenArcManager();
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

flag Project::FileImport()
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

flag Project::FileExport()
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

void Project::LoadHelpFileList()
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
    MdlHelpFileList.SetSize(0);

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
            MdlHelpFileList.Add(Name());
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
    MdlHelpFileList.Add("Models.hlp");
  else
    {
    if (!KwMdlHelpSpecd)
      MdlHelpFileList.Add("Models.hlp");
    }
  }

//---------------------------------------------------------------------------

flag Project::ModelHelp(char* pMdlTxt/*=NULL*/, int HelpIndex/*=0*/)
  {
  ASSERT(HelpIndex<MdlHelpFileList.GetSize());
  Strng Result;
  FindDLLOrHlp((char*)(const char*)(MdlHelpFileList[HelpIndex]), DllFilesPath(), Result);

  flag b;
  if (pMdlTxt==NULL || strlen(pMdlTxt)==0)
    b = ::WinHelp(AfxGetMainWnd()->m_hWnd, Result(), HELP_CONTENTS, 0);
  else
    b = ::WinHelp(AfxGetMainWnd()->m_hWnd, Result(), HELP_PARTIALKEY, (DWORD)pMdlTxt);
  return b;
  }

//---------------------------------------------------------------------------
// ExecObj Overridables

void Project::EO_OnAppActivate(BOOL bActive)
  {
  }

//---------------------------------------------------------------------------

flag Project::EO_RequestTagInfo(RequestTagInfoRec& Rqst, ReplyTagInfoRec& Info)
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

flag Project::EO_PrePreStart()
  {
  bool ScrollTrendsToEnd = false;
  if (gs_Exec.HstRestartOnStart())
    {
    gs_pPrj->CloseHistorian();
    gs_pPrj->RestartHistorian();
    gs_pPrj->OpenHistorian();
    ScrollTrendsToEnd = true;
    if (gs_Exec.GlblProbalMode())
      gs_Exec.SetTime(0.0); //reset time to "zero"
    }
  if (gs_Exec.ResetTimeOnStart() && !gs_Exec.GlblProbalMode() && !gs_Exec.SyncWithClock())
    {
    gs_Exec.SetTime(gs_Exec.OnStartTime());
    ScrollTrendsToEnd = true;
    }
  if (gs_Exec.HstSetTaglistOnStart())
    {
    gs_Exec.KeepHistoryFile(gs_Exec.HstTaglistFile());
    }
  if (ScrollTrendsToEnd)
    CTagVwDoc::AdjustTimebaseToEndAll();
  return true;
  }

//---------------------------------------------------------------------------

flag Project::EO_PreStart()
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

flag Project::EO_Starting(flag fBeginStarting)
  {
  CMainFrame* pWnd=(CMainFrame*)AfxGetApp()->GetMainWnd();//MainWnd();
  if (pWnd->m_pPrjSheet)
    pWnd->m_pPrjSheet->PostMessage(WM_CLOSE);
  return true;
  }

//---------------------------------------------------------------------------

flag Project::EO_Stopping(flag fBeginStopping)
  {
  if (!fBeginStopping)
    {
    //ScdPFUser.WrDouble("General", "ExecTime", gs_Exec.Time());
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

flag Project::DoNew(flag InConstructor)
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

flag Project::SetupRmtCopies()
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

flag Project::DoOpen(pchar pPrjPath, pchar pNewModelCfg, PrjLoadTypes PrjLoadType)
  {
  Strng TmpContractStr;
  Strng NewModelCfg=pNewModelCfg;

  SetUsingPrjLclFiles(PrjLoadType==PLT_Local);
  //m_LoadFromBackupRqd=false;

  FnSetTestLocalFiles(true);
  FnLocalFilesDifferentClear();
  pFlwLib = NULL;
  CWaitMsgCursor Wait("Opening project");
  bCmdSuccess = true;
  bDoingLoad = 1;
  char Fn[512];
  Fn[0] = 0;
  if ((pPrjPath==NULL) || strstr(pPrjPath, "*") || strlen(pPrjPath)==0)
    {
    DWORD dwFlags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_SHAREAWARE;
    char InitDir[512];

    CString s = ScdPFUser.RdStr("General", "RecentModelCfg", "");
    if (s.GetLength()==0)
      s = ScdPFUser.RdStr("General", "RecentModelLib", "");
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

  BOOL DoCheckTags=FALSE;
  flag OK=false;
  if (pPrjPath)
    {
    //Strng PrjName, E;
    //PrjName.FnName(pPrjPath);

    // Test Position of SPJ File
    Strng Td, SPrjFiles, SPrjFile, E;
    Td.FnDrivePathName(pPrjPath);
    DWORD Att=GetFileAttributes(Td());
    flag OldStyle=(Att!=0xFFFFFFFF && ((Att&FILE_ATTRIBUTE_DIRECTORY)!=0));
    Td+="\\TagReg.CSV";
    OldStyle=OldStyle && FileExists(Td());
    flag DoTheOpen=true;
    if (OldStyle)
      {
      if (AfxMessageBox("Upgrade Project to 7.2.3(or Later) format",
                        MB_YESNO|MB_DEFBUTTON2|MB_ICONQUESTION)==IDNO)
        {
        DoTheOpen=false;
        }
      else
        {

        SPrjFile=pPrjPath;
        SPrjFiles.FnDrivePathName(pPrjPath);

        Strng T, NewPrjFile;
        NewPrjFile.FnDrivePathName(SPrjFile());
        NewPrjFile.FnCheckEndBSlash();
        T.FnNameExt(SPrjFile());
        NewPrjFile+=T();
        if (MoveFile(SPrjFile(), NewPrjFile()))
          {
          SPrjFile=NewPrjFile;

          Strng NewPrjFiles(SPrjFiles);
          NewPrjFiles.FnCheckExtension("spf");
          //NewPrjFilesName.FnNameExt(NewPrjFiles());
          if (MoveFile(SPrjFiles(), NewPrjFiles()))
            {
            SPrjFiles=NewPrjFiles;
            SPrjFile=NewPrjFiles;
            SPrjFile.FnCheckEndBSlash();
#if PrjSPJ
            SPrjFile+="Project.spj";
#elif DollarSPJ
            SPrjFile+="$.spj";
#else
#if UScoreSPJ
            SPrjFile+="_";
#endif
            SPrjFile+=T();
#endif
            }
          else
            {
            E.GetSystemErrorStr(GetLastError());
            LogError("SysCAD", LF_Exclamation,
              "Project folder not renamed :\n%s\n%s", SPrjFiles(), E());
            }
          }
        else
          {
          E.GetSystemErrorStr(GetLastError());
          LogError("SysCAD", LF_Exclamation,
            "Project file not moved :\n%s\n%s", SPrjFile(), E());
          }
        }
      }
    else
      {
      SPrjFile=pPrjPath;
      SPrjFiles.FnDrivePath(pPrjPath);
      }

    CStopWatch SW;
    CString sSWTmp;
    SW.Start();
    CDlgBusy::Open("\nOpening Project");
    Wait.UpdateMsg("Opening project %s", SPrjFile());//FnContract(TmpContractStr, PrjName()));
    if (DoTheOpen)
      {
      bChangedGrfMenu=0;
      bChangedRuntimeMenu=0;

      // Modify PrjFileName if Neccessary
      Strng T;
      T.FnNameExt(SPrjFile());
      //if (SPrjFile.XStrCmp("$")!=0)
#if PrjSPJ
      if (T.XStrICmp("Project.spj")!=0)
        {
        T.FnDrivePath(SPrjFile());
        T+="Project.spj";
        if (!MoveFile(SPrjFile(), T()))
          {
          LogError("Project",0, "%s not renamed to %s", SPrjFile(), T());
          DoTheOpen=false;
          }
        else
          SPrjFile=T();
        }
#elif DollerSPJ
      if (T.XStrICmp("$.spj")!=0)
        {
        T.FnDrivePath(SPrjFile());
        T+="$.spj";
        if (!MoveFile(SPrjFile(), T()))
          {
          LogError("Project",0, "%s not renamed to %s", SPrjFile(), T());
          DoTheOpen=false;
          }
        else
          SPrjFile=T();
        }
#endif
      // SetPrjFile(SPrjFile());
      SetPrjFiles(SPrjFiles(), SPrjFile());
      CDlgBusy::SetLine(3, PrjName());
      // SetPrjFilename(pPrjPath);
      bLoadBusy=1;
      //AfxGetMainWnd()->PostMessage(WMU_UPDATEMAINWND, SUB_UPDMAIN_BACKGROUND, 0);

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
      }

    ASSERT(_CrtCheckMemory());

    char Buff[1024];
    FILE* f = NULL;
    if (DoTheOpen)
      {
      OK = true;
      WIN32_FIND_DATA fd;
      HANDLE H = FindFirstFile(PrjFile(), &fd);
      if (H!=INVALID_HANDLE_VALUE)
        {//project exists
        FindClose(H);
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
          {
          sprintf(Buff,"WARNING: Project file is read-only!\n\nThe read-only file attribute should NOT be set for all the files in a project!\n\n%s\n\n  Continue with project load?", PrjFile());
          if (AfxMessageBox(Buff, MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2)!=IDYES)
            OK = false;
          }
        }
      if (OK)
        {
        f = fopen(PrjFile(), "rt");
        if (f==NULL)
          OK = false;
        }
      }
    if (f!=NULL)
      {//project exists
      dbgMemoryState("Project Load Start");
      fread(Buff, 1, sizeof(Buff), f);
      Buff[sizeof(Buff)-1] = 0;
      fclose(f);
      OK = true;
      if (strstr(Buff, "Syscad Project File"))
        {
        AfxMessageBox("Old style project incompatible!");
        DoNew(false);
        OK = false;
        }
      if (OK)
        {
        Strng s;

        AfxGetMainWnd()->UpdateWindow();
        CProfINIFile PF(SPrjFile());

        gs_CnvsMngr.Recover();

        SetOrigPrjFileVerNo(PF.RdInt("General", "PrjFileVersionOriginal", -1));
        SetPrjFileVerNo(PF.RdInt("General", "PrjFileVersion", 0));

        if (PrjFileVerNo()>=73)
          {
          gs_Exec.SetGlblRunModesString(PF.RdStr("General", "RunMode", ""), SM_All);
          gs_Exec.SetGlblRunModesString(PF.RdStr("General", "HeatMode", ""), HM_All);
          }
        else
          {
          bool PB = (PF.RdInt("General", "ProBalMode", true)!=0);
          gs_Exec.SetGlblRunModes(HM_Full, HM_All);
          if (PB)
            gs_Exec.SetGlblRunModes(SM_Direct, SM_All);
          else
            {//NOT probal
            int FlowMd=PF.RdInt("General", "DynamicFlowMode", (PrjFileVerNo()>=72));
            gs_Exec.SetGlblRunModes(FlowMd ? SM_Inline : SM_Buffered, SM_All);
            }
          }

        OldPrjName = PF.RdStr("General", "PrjName", "");
        OldPrjFiles = PF.RdStr("General", "PrjFiles", "");

        if (0)//this needs to go back some-where...
          {//THIS NEEDS MORE WORK !!!!!!!!!!!!!!
          Strng Tmp=PrjFile();
          Tmp=Tmp.FnDrivePath();
          if (Tmp.XStrICmp(OldPrjFiles)!=0)
            {
            Strng Tmpp;
            Tmpp.Set("Project has moved from:\n\n%s\n\nSelect alternate Cfg file ?", OldPrjFiles());
            if (AfxMessageBox(Tmpp(), MB_ICONQUESTION|MB_YESNO)==IDYES)
              {
              CModelInfo Info;
              if (Info.GetSetName(pPrjPath, false, PrjFileVerNo(), gs_License.LicCatagories()))
                NewModelCfg=Info.m_sCfgFile();
              }
            }
          }

        if (OldPrjFiles.GetLength()==0) // for older versions
          {
          OldPrjFiles = PF.RdStr("General", "PrjRootDirectory", "");
          OldPrjFiles.FnCheckEndBSlash();
          OldPrjFiles += OldPrjName;
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

        lStatsRefreshCnt = Max(1L, PF.RdLong("General", "StatsRefreshCount", lStatsRefreshCnt));

        m_bRunOptEmptyAll     = PF.RdLong("RunStartup", "EmptyAll",     m_bRunOptEmptyAll     ?1:0)!=0;
        m_bRunOptEmptySpills  = PF.RdLong("RunStartup", "EmptySpills",  m_bRunOptEmptySpills  ?1:0)!=0;
        m_bRunOptZeroFlows    = PF.RdLong("RunStartup", "ZeroFlows",    m_bRunOptZeroFlows    ?1:0)!=0;    
        m_bRunOptInitialiseAll= PF.RdLong("RunStartup", "InitialiseAll",m_bRunOptInitialiseAll?1:0)!=0;
        m_bRunOptResetStats   = PF.RdLong("RunStartup", "ResetStats",   m_bRunOptResetStats   ?1:0)!=0;   
        m_bRunOptOnceOnly     = PF.RdLong("RunStartup", "OnceOnly",     m_bRunOptOnceOnly    ?1:0)!=0;    

        gs_Exec.SetTime(PF.RdDouble("General", "ExecTime", gs_Exec.Time()));
        gs_Exec.SetStepSizeMax(PF.RdDouble("General", "StepSizeMax", gs_Exec.StepSizeMax()));
        gs_Exec.SetRealTime(PF.RdLong("General", "RealTime", gs_Exec.RealTime()) != 0);
        gs_Exec.SetRealTimeMult(PF.RdDouble("General", "RealTimeMult", gs_Exec.RealTimeMult()));
        gs_Exec.SetRunTermination((RunTerms)PF.RdInt("General", "RunTerm", (int)gs_Exec.RunTermination()));
        gs_Exec.SetTimeUntilStop(PF.RdDouble("General", "TimeUntilStop", gs_Exec.TimeUntilStop()));
        gs_Exec.SetMaxRunSteps(PF.RdLong("General", "MaxRunIters", gs_Exec.MaxRunSteps()));

        m_sDefGrpLib = PF.RdStr("General", "DefaultLibrary", m_sDefGrpLib());
        m_sGrfFrameName = PF.RdStr("General", "GraphicsFrame", m_sGrfFrameName());
        m_sGrfFrameFilter = PF.RdStr("General", "GraphicsFilter", m_sGrfFrameFilter());

        bGrfDeferLoad = (PF.RdLong("General", "GrfDeferLoad", bGrfDeferLoad) != 0);
        bGrfAutoLoad = (PF.RdLong("General", "GrfAutoLoad", bGrfAutoLoad) != 0);
        bGrfUpdateAnnot = (PF.RdLong("General", "GrfUpdateAnnotation", bGrfUpdateAnnot) != 0);
        bGrfAnimationOn = (PF.RdLong("General", "GrfAnimationOn", bGrfAnimationOn) != 0);
        bGrfPromptForZoom = (PF.RdLong("General", "GrfPromptForZoom", bGrfPromptForZoom) != 0);
        bGrfMoveCursor= (PF.RdLong("General", "GrfMoveCursor", bGrfMoveCursor) != 0);
        bGrfRegExpOn= (PF.RdLong("General", "GrfRegExpOn", bGrfRegExpOn) != 0);
        bReadOnlyPrj = (PF.RdLong("General", "ReadOnly", bReadOnlyPrj) != 0);
        SetDisplayTagsOnly(PF.RdLong("General", "DisplayTagsOnly", GetDisplayTagsOnly()) != 0);
        SetHideZeros(PF.RdLong("General", "HideZeros", GetHideZeros()) != 0);
        SVI.SetShowGroups(PF.RdLong("General", "ShowGroups", SVI.GetShowGroups()) != 0);
        TaggedObject::NumericTagsBad = (PF.RdLong("General", "NumericTagsBad", TaggedObject::NumericTagsBad) != 0);
        TaggedObject::NumericStartingTagsBad = (PF.RdLong("General", "NumericStartingTagsBad", TaggedObject::NumericStartingTagsBad) != 0);
        TaggedObject::NonNumericTagChr = (char)PF.RdLong("General", "NonNumericTagChr", TaggedObject::NonNumericTagChr);
        if (PrjFileVerNo()==14)
          TaggedObject::NumericStartingTagsBad = TaggedObject::NumericTagsBad;
        if (PrjFileVerNo()<15)
          DoCheckTags = TRUE;
        if (!TaggedObject::CheckNonNumericTagChr(TaggedObject::NonNumericTagChr))
          TaggedObject::NonNumericTagChr = '#';
        TaggedObject::NumericTagsBad = TaggedObject::NumericTagsBad || TaggedObject::NumericStartingTagsBad;

        m_bRemoveRevs=(PF.RdLong("Database", "RemovePreviousRevisions", m_bRemoveRevs)!=0);
        m_bCopyDBFiles=(PF.RdLong("Database", "CopyDBFiles", m_bCopyDBFiles)!=0);

        CTagVwDoc::GTB.StartTime = PF.RdDouble("Trend", "GlobalStartTime", CTagVwDoc::GTB.StartTime);
        CTagVwDoc::GTB.EndTime = CTagVwDoc::GTB.StartTime + PF.RdDouble("Trend", "GlobalDuration", CTagVwDoc::GTB.Duration());
        CTagVwDoc::GTB.TrackingTime = (PF.RdInt("Trend", "GlobalTrackingTime", CTagVwDoc::GTB.TrackingTime)!=0);

        Solver.fStateWnd=PF.RdInt("Solver", "StatusWnd", Solver.fStateWnd);
        Solver.fStatePinned=PF.RdInt("Solver", "StatusPinned", Solver.fStatePinned);
        Solver.fStateErrors=PF.RdInt("Solver", "StatusErrors", Solver.fStateErrors);
        Solver.iStateX=PF.RdInt("Solver", "StatusX", Solver.iStateX);
        Solver.iStateY=PF.RdInt("Solver", "StatusY", Solver.iStateY);

        if (PrjFileVerNo()<54)
          {
          gs_Exec.SetRealTime(PF.RdLong("Historian", "RealTime", gs_Exec.RealTime()) != 0);
          gs_Exec.SetStepSizeMax(PF.RdDouble("Historian", "TimeMax", gs_Exec.StepSizeMax()));
          }
        /*kga 15/8/01 removed because does not recover correctly for real time
        if (gs_Exec.RealTime())
          {
          long OldScan=PF.RdLong("Historian", "ScanTime", 1);
          gs_Exec.SetTimeIncMax(OldScan*0.001);
          }
*/

        Strng sOldModelCfg = PF.RdStr("General", "ModelLibrary", "");
        sOldModelCfg = PF.RdStr("General", "ModelConfiguration", sOldModelCfg());
        sOldModelCfg = PF.RdStr("General", "CfgFile", sOldModelCfg());
        sOldModelCfg.FnExpand();

        if (!NewModelCfg() && !sOldModelCfg())
          {

#pragma chCHECKIT("if sOldModelCfg does not exist the query OpenBackup")

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
            OK=false;
          }

        Strng sOldModelCfgMem(sOldModelCfg);
        Strng sNewCfg(sOldModelCfg);
        sNewCfg.FnCheckExtension(".Cfg");
        byte Where=0;
        flag AbortLoad=false;

        if (OK)
          {
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
              OldPrjFiles  = "D:\\A\\B\\x.spf";
              PFiles       = "C:\\X\\Y\\Z\\x.spf";
              }

            // Find start part of OldProject and OldCfg which are equal
            int iOPrjLen=OldPrjFiles.Length();
            int iOCfgLen=sOldModelCfg.Length();
            int iNPrjLen=PFiles.Length();
            int lStartEql=-1;
            int i0=Min(iOCfgLen, iOPrjLen);
            for (int i=0; i<i0; i++)
              {
              if (sOldModelCfg[i]!=OldPrjFiles[i])
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
                if (OldPrjFiles[i0]!=PFiles[i1])
                  break;
                if (OldPrjFiles[i0]=='\\')
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
          }

        CModelInfo Info;
        Info.Saved.m_sCfgFiles=PF.RdStr("General", "CfgFiles", "");
        Info.Saved.m_sCfgHome=PF.RdStr("General", "CfgHome", "");
        CModelInfoUpgrade InfoU;
        flag FmtOK=true;
        OK = !AbortLoad && (sNewCfg()!=NULL);
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
          SetCfgHome(Info.m_sCfgHome());
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
        dbgDumpFilesAliases();

        RestoreMainWindow();
        #if WithRmtFile
        if (OK && RmtOK)
        #else
        if (OK)
        #endif
          {
          Strng FlwLibTag, EO_LocationTag;
          FlwLibTag=PlantModelTag;
          EO_LocationTag=PlantModelTag;
          if (PrjFileVerNo()<16)
            {
            //CTagDBase::sOldFlwLibTag.Set("%s", sOldModelCfgMem());
            CTagDBase::sOldFlwLibLocation.Set("%s.DLL", sOldModelCfgMem());
            }
          else
            {
            //CTagDBase::sOldFlwLibTag="";
            CTagDBase::sOldFlwLibLocation="";
            }

          pFlwLib=NULL;
          OK = MdlLibs.LoadFlwDLLs(Info.m_sDLLList, FlwLibTag(), EO_LocationTag(), &m_FlwLib);
          if (OK)
            pFlwLib=&m_FlwLib;

//LogNote("Project", 0, "DLLs time %s", SW.MinSecDesc(sSWTmp));

          if (OK)
            {
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

            bDefaultRecordIt = (PF.RdLong("General", "DefaultRecordIt", bDefaultRecordIt) != 0);
            bFlashTrend = (PF.RdLong("General", "FlashTrend", bFlashTrend) != 0);
            bShowNewTrndLineDlg = (PF.RdLong("General", "ShowNewTrndLineDlg", bShowNewTrndLineDlg) != 0);
            bToolTips = (PF.RdLong("General", "ToolBarTips", bToolTips) != 0);
            bFlyBys = (PF.RdLong("General", "ToolBarFlyBys", bFlyBys) != 0);

            int i;
            #if WITHSCRCYCLES
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

            for (i=0; i<16; i++)
              {
              char Item[32];
              sprintf(Item, "Colour%d", i);
              CustomColours[i] = PF.RdLong("CustomColours", Item, CustomColours[i]);
              }

            if (0)
              {
              CDlgBusy::Open("\n\nOpening Communications");
              CDlgBusy::Close();
              bCommsOpen=1;
              }

            HstMngr.LoadSettings(PF);
            HstMngr.SetLicensingOptions(gs_License.MaxHistSizeAllowed(), gs_License.MaxHistFilesAllowed());

            OpenHistorian();
            dbgMemoryState("Historian");

//LogNote("Project", 0, "Hist time %s", SW.MinSecDesc(sSWTmp));

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

            sArcManagerName = PF.RdStr("ArcManager", "Name", sArcManagerName());
            bArcOn = (PF.RdLong("ArcManager", "On", bArcOn) != 0);
            bArcOpenDBOnRun = (PF.RdLong("ArcManager", "OpenDBOnRun", bArcOpenDBOnRun) != 0);
            sArcManagerName.FnContract();

            sIOMarshalName = PF.RdStr("IOMarshal", "Name", sIOMarshalName());
            sIOMarshalName.FnCheckExtension(".scm");
            sIOMarshalName.FnContract();
            bIOMOn = (PF.RdLong("IOMarshal", "On", bIOMOn) != 0);
            sIOMarshalNode = PF.RdStr("IOMarshal", "Node", sIOMarshalNode());

            //get report info...
            ReportInfo.iHSearchLen = Range(1, PF.RdInt("Reports", "HSearchLen", ReportInfo.iHSearchLen), (int)MaxCSVCols);
            ReportInfo.iVSearchLen = Range(1, PF.RdInt("Reports", "VSearchLen", ReportInfo.iVSearchLen), 2048);
            ReportInfo.sNan = PF.RdStr("Reports", "Nan", ReportInfo.sNan());
            ReportInfo.sBlank = PF.RdStr("Reports", "Blank", ReportInfo.sBlank());
            ReportInfo.sIgnoreChars = PF.RdStr("Reports", "IgnoreChars", ReportInfo.sIgnoreChars());
            bRptExcelLock = (PF.RdLong("Reports", "ExcelLock", bRptExcelLock) != 0);
            bRptExcelCellName = (PF.RdLong("Reports", "ExcelUseCellName", bRptExcelCellName) != 0);
            bRptExcelMakeActive = (PF.RdLong("Reports", "ExcelAlmaysMakeActive", bRptExcelMakeActive) != 0);
            bRptExcelSysCADActive = (PF.RdLong("Reports", "ExcelReturnSysCADtoActive", bRptExcelSysCADActive) != 0);
            bRptExcelUpdateLinks = (PF.RdLong("Reports", "ExcelUpdateLinks", bRptExcelUpdateLinks) != 0);
            bRptExcelSaveOnComplete = (PF.RdLong("Reports", "ExcelSaveOnComplete", bRptExcelSaveOnComplete) != 0);
            bRptSyscadTags = (PF.RdLong("Reports", "SyscadTags", bRptSyscadTags) != 0);
            bAccAutoSaveOn = (PF.RdLong("Access", "AutoSaveOn", bAccAutoSaveOn) != 0);
            bAccUpdateOnRun = (PF.RdLong("Access", "UpdateOnRun", bAccUpdateOnRun) != 0);
            bAccHistoryInfoVis = (PF.RdLong("Access", "HistoryInfoVis", bAccHistoryInfoVis) != 0);

            LoadColours(PF);

            bDDEOn = (PF.RdLong("DDEManager", "On", bDDEOn) != 0);
            OpenDDEManager();

            m_bOPCOn = (PF.RdLong("OPCManager", "On", m_bOPCOn) != 0);
            m_bOPCResetReg = (PF.RdLong("OPCManager", "ResetRegistry", m_bOPCResetReg) != 0);
            m_iOPCServerNo = PF.RdLong("OPCManager", "ServerNumber", m_iOPCServerNo);

            LoadMainOPCOptions(PF);
            //if (pOPCSrvr && pOPCSrvr->OPCSrvrMngr())
            //  pOPCSrvr->OPCSrvrMngr()->LoadOrSaveOptions(PF, true);

            gs_pPrj->DoLoadDB(PrjFiles(), OldPrjName());

            ASSERT(_CrtCheckMemory());

            dbgMemoryState("LoadDatabase");

            // Moved to Callback
            //OpenDrvManager(); //must be loaded after models (tag data base)
            //dbgMemoryState("DriveMan");

            //load documents...
            CDlgBusy::Open("\nOpening Documents");
            if (PrjFileVerNo()<12)
              {
              const int DocCount = PF.RdInt("General", "DocumentCount", 0);
              char Section[256];
              for (i=0; i<DocCount; i++)
                {
                Strng sDoc;
                sprintf(Section, "Doc%i", i);
                sDoc = PF.RdStr(Section, "Name", "");
                sDoc.FnExpand();

                WIN32_FIND_DATA fd;
                HANDLE fh = FindFirstFile(sDoc(), &fd);
                if (fh!=INVALID_HANDLE_VALUE)
                  {
                  FindClose(fh);
                  CDlgBusy::SetLine(1, "Opening Document %d of %d:\n\n%s", i+1, DocCount, TmpContractStr.FnNameExt(sDoc()));
                  ScdApp()->OpenDocumentFile(sDoc());
                  dbgMemoryState(sDoc());
                  //AfxGetMainWnd()->UpdateWindow();
                  XFlushAppMsgQ();
                  }
                else
                  LogError("Project", LF_Exclamation, "Document %s not found", sDoc());
                }
              }
            else
              {
              for (int j=iGraphTemplate; j<=iControlTemplate; j++)
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
              bRptExcelMakeActive = 1; //change this option
            CDlgBusy::Close();
//LogNote("Project", 0, "Docs time %s", SW.MinSecDesc(sSWTmp));

            EnableMainOPCSrvr(m_bOPCOn);

            }
          }
        //else
        //  {
        //  m_RmtCpyFolder="";
        //  }

        // Overide any other setting for Time which may have been set
        gs_Exec.SetTime(gs_Exec.QueryCurrentTime());

        if (gs_pPrj->pFlwLib)
          gs_pPrj->pFlwLib->FE_SetSpModelFixupStrategy(PF.RdLong("SpModel", "FixupStrategy", FUS_Largest));

#if USESCDEXPLORER
        if (CExploreScd::UseSelectWndList())
          {
          int WSOpn=PF.RdInt("ScdExplorer", "Open", -1);
          if (WSOpn>0)
            CExploreScd::OpenIt();
          else if (WSOpn==0)
            CExploreScd::CloseIt();
          }
#else
        if (CWndSlctWnd::UseSelectWndList())
          {
          int WSOpn=PF.RdInt("WindowSelector", "Open", -1);
          if (WSOpn>0)
            CWndSlctWnd::OpenIt();
          else if (WSOpn==0)
            CWndSlctWnd::CloseIt();
          }
#endif
        }
      }
    else
      OK = false;

    if (!OK)
      {
      CloseRmtCpy();

      if (PrjFiles())
        {
        char buff[256];
        sprintf(buff, "Project %s not Opened", PrjFiles());
        AfxMessageBox(buff, MB_ICONEXCLAMATION|IDOK);
        }
      DoNew(false);
      bLoadBusy=0;

      SetPrjFileVerNoAsLoaded(PrjFileVer);
      SetPrjFileVerNo(PrjFileVer);
      }
    else
      {
      CProfINIFile PF(PrjIniFile());
      eSSAction = (SnapShotActions)PF.RdLong("SnapShot", "Action", eSSAction);
      sSSName = PF.RdStr("SnapShot", "Name", sSSName());
      sSSPrefix = PF.RdStr("SnapShot", "Prefix", sSSPrefix());
      iSSNumber = (UINT)PF.RdLong("SnapShot", "Number", iSSNumber);
      bSSDoZip = (PF.RdLong("SnapShot", "DoZip", bSSDoZip) != 0);

      if (CfgFile())
        ScdPFUser.WrStr("General", "RecentModelCfg", CfgFile());
      bLoadBusy=0;

      ASSERT(_CrtCheckMemory());

      //load is finished, get trends to reconnect and fetch trend lines etc...
      CDlgBusy::Open("\nConnecting trends");
      for (int iTemp=iTrendTemplate; iTemp<=iControlTemplate; iTemp++)
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
      //AfxGetMainWnd()->UpdateWindow();
      CDlgBusy::Close();

      ASSERT(_CrtCheckMemory());

      SW.Stop();
      if (OK)
        {
        LogNote("Project", 0, "Load time %s", SW.MinSecDesc(sSWTmp));
        if (bReadOnlyPrj)
          LogNote("Project", 0, "Marked as read-only");
        ::AfxTrace(_T("Project loaded: '%s'\n"), _T(PrjName()));
        }
      else
        ((CMainFrame*)AfxGetMainWnd())->UpdateMainToolBar();
      }

    CheckLicenseConditions();
    CDlgBusy::Close();
    MainWnd()->PostMessage(WMU_UPDATEMAINWND, SUB_UPDMAIN_PRJLOADED); //notify main window that project load is complete
    MainWnd()->PostMessage(WMU_UPDATEMAINWND, SUB_UPDMAIN_UPDATE/*, (LPARAM)pMsgFrame*/); //get main window thread to top AND make message window active top window
    CMsgWindow::Show(false);

    CMainFrame* pM = (CMainFrame*)AfxGetMainWnd();
    pM->SetToolBarSolveMode();
    }

  bDoingLoad = 0;
  bCmdSuccess = OK;

  if (OK)
    {
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
        CloseArcManager();
        CloseDrvManager();
        OpenDrvManager();
        OpenArcManager();
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

    if (CfgHome())
      ScdPFUser.WrStr("General", "RecentBaseProjectDir", CfgHome());

    // Old version now loaded - Remember Old Version
    SetPrjFileVerNoAsLoaded(PrjFileVerNo());
    // Set version to latest
    SetPrjFileVerNo(PrjFileVer);
    }
  return OK;
  }

//---------------------------------------------------------------------------
#define UseDirectories 0

flag Project::DoPrjPaths(pchar pPrjPath, flag IsNew, CPrjFilesLists* pPFL, BOOL WithVersion, BOOL RemoveOld)
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
    #if UseDirectories
    Strng DlgPrjHome("DlgPrjHome-");
    DlgPrjHome+=CfgName();
    Strng HDir(ScdPFUser.RdStr("Directories", DlgPrjHome(), ""));
    if (HDir.GetLength()==0)
      if (CfgHome()!=NULL)
        HDir=CfgHome();
      else
        HDir=TemporaryFiles();
    HDir.FnClearEndBSlash();
    #else
    Strng HDir(CfgHome());
    /*//rather than check for cfgfiles in path, perhaps test if specie mdb is in this folder...?
    const int HDirLen = HDir.Len();
    if (HDirLen>9 && _strnicmp(&HDir[HDirLen-9], "CfgFiles", 7)==0)
      HDir=HDir.Left(HDirLen-9);*/
    #endif

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
#if PrjSPJ
      NewPrjFile+="Project";
#elif DollarSPJ
      NewPrjFile+="$";
#else
#if UScoreSPJ
      NewPrjFile+="_";
#endif
      NewPrjFile+=Name;
#endif
      NewPrjFile.FnCheckExtension("spj");

      #if UseDirectories
      Strng PHome;//(NewPrjFile);
      PHome.FnDrivePath(NewPrjFolder());
      ScdPFUser.WrStr("Directories", DlgPrjHome(), PHome());
      #endif
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
#if PrjSPJ
        NewPrjFile+="Project";
#elif DollarSPJ
        NewPrjFile+="$";
#else
#if UScoreSPJ
        NewPrjFile+="_";
#endif
        NewPrjFile+=N;
#endif
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
    CPrjSaveAsDlg  Dlg(sOldPrjFiles(), IsNew);
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
#if PrjSPJ
    NewPrjFile+="Project";
#elif DollarSPJ
    NewPrjFile+="$";
#else
#if UScoreSPJ
    NewPrjFile+="_";
#endif
    NewPrjFile+=NewName;
#endif
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
    HstMngr.sHstDataDir = PrjFilesAlias();
    HstMngr.sHstCatDir = PrjFilesAlias();
    }

  if (!IsNew)
    {
    Strng NewCfgFile  = CfgFile();
    Strng NewCfgFiles = CfgFiles();
    Strng NewCfgHome  = CfgHome();

    SwingFileNameToDest(NewCfgFile, OrigPrjFiles, NewPrjFolder);
    SwingFileNameToDest(NewCfgFiles, OrigPrjFiles, NewPrjFolder);
    SwingFileNameToDest(NewCfgHome, OrigPrjFiles, NewPrjFolder);

    SetCfgFile(NewCfgFile());
    SetCfgFiles(NewCfgFiles());
    SetCfgHome(NewCfgHome());

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

flag Project::CopyOtherFile(CSFPInfo & SFI)
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

int Project::CopyOtherFiles(CPrjFilesLists &PFL, CProfINIFile &PF)
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
        if (PrjFileVerNo()<31)
          {
          if (_stricmp(Name, OldCfgFolderName())==0)
            strcpy(Name, DefCfgFolderName());
          else if (_stricmp(Name, OldCfgDBFileName())==0)
            strcpy(Name, CfgDBFileName());
          else if (_stricmp(Name, OldBCfgFolderName())==0)
            strcpy(Name, BCfgFolderName());
          else if (_stricmp(Name, OldBCfgDBFileName())==0)
            strcpy(Name, BCfgDBFileName());
          }
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

void Project::ExpandStg(LPCTSTR StgFn, LPCTSTR Fn)
  {
  WCHAR WFn[MAX_PATH];
  AfxA2WHelper(WFn, StgFn, strlen(StgFn)+1);

  IStoragePtr spStorage;
  DoRdStorage(WFn, spStorage, Fn);
  };

// --------------------------------------------------------------------------

static char * FnPathRelContract(char * Fn)
  {
  static Strng Tmp;
  Tmp=Fn;
  Tmp.FnMakePrjFileRelative();
  Tmp.FnContract();
  return Tmp();
  }

// --------------------------------------------------------------------------

flag Project::DoSave(flag DoPathChg, pchar pPrjPath, flag IsNew)
  {
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

  bReadOnlyPrj = 0;

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
    s = sOldPrjFiles;
    s += "*.dxf";
    WIN32_FIND_DATA fd;
    HANDLE H = FindFirstFile(s(), &fd);
    flag AllDone = (H==INVALID_HANDLE_VALUE);
    while (!AllDone)
      {
      s = sOldPrjFiles;
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
  PF.WrInt("General", "PrjFileVersion", PrjFileVer);
  PF.WrInt("General", "PrjFileVersionOriginal", OrigPrjFileVerNo());
  PF.WrStr("LastSaveInfo", "SysCADVersion", FullVersion());
  s="-";
  if (strlen(SCD_PATCHNOTE)>0)
    {
    if (strlen(SCD_PATCHDATE)>0)
      s.Set("%s  (%s)", SCD_PATCHNOTE, SCD_PATCHDATE);
    else
      s.Set("%s", SCD_PATCHNOTE);
    }
  PF.WrStr("LastSaveInfo", "SysCAD_Update", s());
  PF.WrStr("LastSaveInfo", "LastSaveDate", DT);
  PF.WrStr("LastSaveInfo", "LastSaveTime", TM);
  PF.WrStr("LastSaveInfo", "WinNT", IsWinNT ? "y" : "n");
  char Buff[512];
  DWORD Sz = sizeof(Buff);
  if (GetUserName(Buff, &Sz))
    PF.WrStr("LastSaveInfo", "UserName", Buff);
  Sz = sizeof(Buff);
  if (GetComputerName(Buff, &Sz))
    PF.WrStr("LastSaveInfo", "ComputerName", Buff);

  SetPrjFileVerNo(PrjFileVer);
  PF.WrStr("General", "PrjName", PrjName());
  PF.WrStr("General", "PrjFiles", PrjFiles());

  PF.WrStr("General", "CfgFile", FnPathRelContract(CfgFile()));
  PF.WrStr("General", "CfgFiles", FnPathRelContract(CfgFiles()));
  PF.WrStr("General", "CfgHome", FnPathRelContract(CfgHome()));
  //PF.WrInt("General", "BackupUsage", 0);
  PF.WrInt("General", "LocalPrjFiles", UsingPrjLclFiles());

  PF.WrDouble("General", "ExecTime", gs_Exec.Time());
  PF.WrDouble("General", "StepSizeMax", gs_Exec.StepSizeMax());
  PF.WrLong("General", "RealTime", gs_Exec.RealTime() != 0);
  PF.WrDouble("General", "RealTimeMult", gs_Exec.RealTimeMult());

  PF.WrInt("General", "RunTerm", (int)gs_Exec.RunTermination());
  PF.WrDouble("General", "TimeUntilStop", gs_Exec.TimeUntilStop());
  PF.WrLong("General", "MaxRunIters", gs_Exec.MaxRunSteps());
  PF.WrLong("General", "ToolBarTips", bToolTips);
  PF.WrLong("General", "ToolBarFlyBys", bFlyBys);
  PF.WrLong("General", "DefaultRecordIt", bDefaultRecordIt);
  PF.WrLong("General", "FlashTrend", bFlashTrend);
  PF.WrLong("General", "ShowNewTrndLineDlg", bShowNewTrndLineDlg);
  PF.WrLong("General", "StatsRefreshCount", Max(1L, lStatsRefreshCnt));
  //PF.WrInt("General", "ProBalMode", GetProBalMode());
  //PF.WrInt("General", "DynamicFlowMode", GetDynModeFlow());
  //PF.WrStr("General", "SolveMode", gs_Exec.GlblRunModesString());
  PF.WrStr("General", "RunMode", gs_Exec.GlblRunModesString(SM_All));
  PF.WrStr("General", "HeatMode", gs_Exec.GlblRunModesString(HM_All));
  PF.WrStr("General", "DefaultLibrary", m_sDefGrpLib());
  PF.WrStr("General", "GraphicsFrame", m_sGrfFrameName());
  PF.WrStr("General", "GraphicsFilter", m_sGrfFrameFilter());
  PF.WrLong("General", "GrfDeferLoad", bGrfDeferLoad);
  PF.WrLong("General", "GrfAutoLoad", bGrfAutoLoad);
  PF.WrLong("General", "GrfUpdateAnnotation", bGrfUpdateAnnot);
  PF.WrLong("General", "GrfAnimationOn", bGrfAnimationOn);
  PF.WrLong("General", "GrfPromptForZoom", bGrfPromptForZoom);
  PF.WrLong("General", "GrfRegExpOn", bGrfRegExpOn);
  PF.WrLong("General", "ReadOnly", bReadOnlyPrj);
  PF.WrLong("General", "DisplayTagsOnly", GetDisplayTagsOnly());
  PF.WrLong("General", "HideZeros", GetHideZeros());
  PF.WrLong("General", "ShowGroups", SVI.GetShowGroups());
  PF.WrLong("General", "NumericTagsBad", TaggedObject::NumericTagsBad);
  PF.WrLong("General", "NumericStartingTagsBad", TaggedObject::NumericStartingTagsBad);
  PF.WrLong("General", "NonNumericTagChr", TaggedObject::NonNumericTagChr);

  PF.WrLong("RunStartup", "EmptyAll",     m_bRunOptEmptyAll     ?1:0);
  PF.WrLong("RunStartup", "EmptySpills",  m_bRunOptEmptySpills  ?1:0);
  PF.WrLong("RunStartup", "ZeroFlows",    m_bRunOptZeroFlows    ?1:0);    
  PF.WrLong("RunStartup", "InitialiseAll",m_bRunOptInitialiseAll?1:0);
  PF.WrLong("RunStartup", "ResetStats",   m_bRunOptResetStats   ?1:0);   
  PF.WrLong("RunStartup", "OnceOnly",     m_bRunOptOnceOnly    ?1:0);    

  PF.WrLong("Database", "RemovePreviousRevisions", m_bRemoveRevs);
  PF.WrLong("Database", "CopyDBFiles", m_bCopyDBFiles);

  PF.WrInt("Solver", "StatusWnd", Solver.fStateWnd);
  PF.WrInt("Solver", "StatusPinned", Solver.fStatePinned);
  PF.WrInt("Solver", "StatusErrors", Solver.fStateErrors);
  PF.WrInt("Solver", "StatusX", Solver.iStateX);
  PF.WrInt("Solver", "StatusY", Solver.iStateY);

  PF.WrLong("SpModel", "FixupStrategy", gs_pPrj->pFlwLib->FE_SpModelFixupStrategy());

  PF.WrDouble("Trend", "GlobalStartTime", CTagVwDoc::GTB.StartTime);
  PF.WrDouble("Trend", "GlobalDuration", CTagVwDoc::GTB.Duration());
  PF.WrInt("Trend", "GlobalTrackingTime", CTagVwDoc::GTB.TrackingTime);

  HstMngr.SaveSettings(PF);

  PF.WrStr("DrvManager", "Name", sDrvManagerName());
  PF.WrLong("DrvManager", "On", bDrvOn);
  PF.WrLong("DrvManager", "LclTagSrvrOK", bDrvLclTagSrvrOK);
  PF.WrLong("DrvManager", "ReadAll", bDrvReadAll);
  PF.WrLong("DrvManager", "IgnoresOn", bDrvIgnoresOn);
  PF.WrLong("DrvManager", "ForcesOn", bDrvForcesOn);
  PF.WrLong("DrvManager", "NoiseOn", bDrvNoiseOn);
  PF.WrLong("DrvManager", "StartFBKCheck ", bDrvStartFBKCheck );

  PF.WrStr("ArcManager", "Name", sArcManagerName());
  PF.WrLong("ArcManager", "On", bArcOn);
  PF.WrLong("ArcManager", "OpenDBOnRun", bArcOpenDBOnRun);

  PF.WrLong("DDEManager", "On", bDDEOn);

  PF.WrLong("OPCManager", "On", m_bOPCOn);
  PF.WrLong("OPCManager", "ResetRegistry", m_bOPCResetReg);
  PF.WrLong("OPCManager", "ServerNumber", m_iOPCServerNo);
  SaveMainOPCOptions(PF);
  DisableMainOPCSrvr();
  //if (pOPCSrvr && pOPCSrvr->OPCSrvrMngr())
  //  pOPCSrvr->OPCSrvrMngr()->LoadOrSaveOptions(PF, false);

  PF.WrLong("IOMarshal", "On", bIOMOn);
  PF.WrStr("IOMarshal", "Name", sIOMarshalName());
  PF.WrStr("IOMarshal", "Node", sIOMarshalNode());

  PF.WrInt("Reports", "HSearchLen", ReportInfo.iHSearchLen);
  PF.WrInt("Reports", "VSearchLen", ReportInfo.iVSearchLen);
  PF.WrStr("Reports", "Nan", ReportInfo.sNan());
  PF.WrStr("Reports", "Blank", ReportInfo.sBlank());
  PF.WrStr("Reports", "IgnoreChars", ReportInfo.sIgnoreChars());
  PF.WrLong("Reports", "ExcelLock", bRptExcelLock);
  PF.WrLong("Reports", "ExcelUseCellName", bRptExcelCellName);
  PF.WrLong("Reports", "ExcelAlmaysMakeActive", bRptExcelMakeActive);
  PF.WrLong("Reports", "ExcelReturnSysCADtoActive", bRptExcelSysCADActive);
  PF.WrLong("Reports", "ExcelUpdateLinks", bRptExcelUpdateLinks);
  PF.WrLong("Reports", "ExcelSaveOnComplete", bRptExcelSaveOnComplete);
  PF.WrLong("Reports", "SyscadTags", bRptSyscadTags);
  PF.WrLong("Access", "AutoSaveOn", bAccAutoSaveOn);
  PF.WrLong("Access", "UpdateOnRun", bAccUpdateOnRun);
  PF.WrLong("Access", "HistoryInfoVis", bAccHistoryInfoVis);

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
    if (CustomColours[i])
      {
      char Item[32];
      sprintf(Item, "Colour%d", i);
      PF.WrLong("CustomColours", Item, CustomColours[i]);
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

  gs_pPrj->DoSaveDB(XDBFormat(), Opts, PrjFiles(), (bDoingSaveAs||bDoingSaveAsNV ? sOldPrjFiles() : NULL));

  CDlgBusy::SetLine(3, "Saving Window Layout");
  SaveWindowLayout();
  CDlgBusy::SetLine(3, CompactPrj());

  CProfINIFile PFI(PrjIniFile());
  PFI.WrLong("SnapShot", "Action", eSSAction);
  PFI.WrStr("SnapShot", "Name", sSSName());
  PFI.WrStr("SnapShot", "Prefix", sSSPrefix());
  PFI.WrLong("SnapShot", "Number", iSSNumber);
  PFI.WrLong("SnapShot", "DoZip", bSSDoZip);

  DeleteFile(sPrjBac()); //saved all; delete backup

  if (CfgFile())
    ScdPFUser.WrStr("General", "RecentModelCfg", CfgFile());
  if (CfgHome())
    ScdPFUser.WrStr("General", "RecentBaseProjectDir", CfgHome());

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

void Project::CloseRmtCpy()
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

flag Project::DoClose(flag ForExit, flag AllowCancel/*=true*/)
  {
  if (gs_Exec.TestRunning())
    return false;

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
        if (gs_pPrj->bReadOnlyPrj && AllowCancel)
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
    pFlwLib=NULL;

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
  CloseDrvManager(ForExit, TRUE);
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

flag Project::CloseForFailedOpen()
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

flag Project::OnNew()
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

  bCmdSuccess = DoClose(false, true);//, false);
  if (bCmdSuccess)
    {
    bCmdSuccess = DoNew(false);

    SetPrjFileFormat(1);

    Strng sModelCfg = ScdPFUser.RdStr("General", "RecentModelCfg", "");
    if (sModelCfg.Length()==0)
      sModelCfg = ScdPFUser.RdStr("General", "RecentModelLib", "");
    if (!Info.GetSetName("", true, PrjFileVer, gs_License.LicCatagories(), pRqdPrjCfg))
      return (bCmdSuccess = false);

    fNewTrend = Info.fNewTrend;
    fNewGraph = Info.fNewGraph;

    SetCfgFile(Info.m_sCfgFile());
    SetCfgHome(Info.m_sCfgHome());
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
    ScdPFUser.WrStr("General", "RecentModelCfg", CfgFile());
    // This will setup all the paths etc.
//    bCmdSuccess = DoSave(NULL, /*NULL,*/ true);
    bCmdSuccess = DoPrjPaths((LPTSTR)(LPCTSTR)pRqdPrjPath, true, NULL, WithVersion, RemoveOld);

    if (bCmdSuccess)
      {
      Info.m_sDLLList.Upper();
      Strng FlwLibTag, EO_LocationTag;

      FlwLibTag=PlantModelTag;
      EO_LocationTag=PlantModelTag;

      pFlwLib=NULL;
      bCmdSuccess = MdlLibs.LoadFlwDLLs(Info.m_sDLLList, FlwLibTag(), EO_LocationTag(), &m_FlwLib);
      if (bCmdSuccess)
        {
        pFlwLib=&m_FlwLib;
        LoadHelpFileList();
      //if (!bCmdSuccess)
      //  UnLoadFlwDLLs();

        CRptTagLists::StartUp();

        OpenHistorian();
        OpenDrvManager();
        OpenArcManager();
        OpenIOMarshal();
        MainWnd()->DefaultToolBar(-1, TRUE);

        // Initialise Mode
        CProfINIFile PCfg(CfgFile());
        Strng MdStr=TaggedObject::GlblMode2String(SM_Direct, SM_Direct);
        MdStr=PCfg.RdStr("General", "Initial_SolveMode", MdStr());
        gs_Exec.SetGlblRunModesString((LPCSTR)MdStr(), SM_All);
        if (TaggedObject::GlblSolveMode()==0)
          gs_Exec.SetGlblRunModes(SM_Direct, SM_All);
        MdStr=TaggedObject::GlblMode2String(HM_Full, HM_All);
        MdStr=PCfg.RdStr("General", "Initial_HeatMode", MdStr());
        gs_Exec.SetGlblRunModesString((LPCSTR)MdStr(), HM_All);
        if (TaggedObject::GlblHeatMode()==0)
          gs_Exec.SetGlblRunModes(HM_Full, HM_All);
        //MdStr.LRTrim();
        //bool PB = (MdStr.XStrICmp("Probal")==0);
        //SetProBalMode(PB);
        //if (!PB)
        //  {//NOT probal
        //  SetDynModeFlow(MdStr.XStrICmp("DynamicFlow")==0);
        //  }
        
        // This will do actual save
//        bCmdSuccess = DoSave();
        }
      }
    }
  return bCmdSuccess;
  }

//---------------------------------------------------------------------------

flag Project::OnOpen(pchar pPrjPath)
  {
  LogSeparator("Open",0);
  bCmdSuccess=true;

  //Try rename if possible
  #if PrjSPJ
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
  #endif

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

    if (fOpenAltCfg)
      LoadFromLocalPrv = false;

    if ((m_LoadTypeRqst==PLT_Null) && Found)
      m_LoadTypeRqst = LoadFromLocalPrv ? PLT_Local : PLT_Normal;

    Strng T;
    T.Set("%s,%s", pPrjPath, m_LoadTypeRqst==PLT_Local ? "L":"N");
    ScdPFUser.WrStr("PreviousOpens", "Opn000", T());
    int k=1;
    for (i=0; i<50 && k<50; i++)
      if (i!=j && S[i].Length()>0)
        {
        Strng Entry;
        Entry.Set("Opn%03i",k++);
        ScdPFUser.WrStr("PreviousOpens", Entry(), S[i]());
        }
    }

  if (fOpenAltCfg)
    {
    CModelInfo Info;
    if (Info.GetSetName(pPrjPath, false, PrjFileVerNo(), gs_License.LicCatagories()))
      {
      NewModelCfg=Info.m_sCfgFile();
      fNewTrend = Info.fNewTrend;
      fNewGraph = Info.fNewGraph;
      }
    else
      bCmdSuccess=false;
    }

  if (bCmdSuccess)
    {
    bCmdSuccess = DoClose(false, true);//, false);
    if (bCmdSuccess)
      {
      bCmdSuccess = DoOpen(pPrjPath, NewModelCfg(), m_LoadTypeRqst);
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

flag Project::OnClose()
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
  AfxGetMainWnd()->PostMessage(WMU_CMDDONE, ComCmd_CloseProject, 0); //let script cmd mngr know cmd is complete
  return bCmdSuccess;
  }

//---------------------------------------------------------------------------

flag Project::OnSave(pchar pPrjPath)
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
    if (gs_pPrj->bReadOnlyPrj)
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
  AfxGetMainWnd()->PostMessage(WMU_CMDDONE, ComCmd_SaveProject, 0); //let script cmd mngr know cmd is complete
  return bCmdSuccess;
  }

//---------------------------------------------------------------------------

flag Project::OnProjectSaveas(pchar pPrjPath)
  {
//  ASSERT_ALWAYS(sPrjFile.XStrICmp(PrjFile())==0)

  Strng OldHstDataDir;
  sOldPrjFile   = PrjFile();
  sOldPrjFiles  = PrjFiles();
  OldHstDataDir = HstMngr.sHstDataDir();
  OldHstDataDir.FnExpand(); // Expand before project changes

//  bDoingSaveAs = true;
  MainWnd()->ClosingProject(false);

//  CPrjSaveAsDlg Dlg(sOldPrjFiles());
//  Dlg.m_CopyHist = true;

  bCmdSuccess = DoSave(true, pPrjPath, false);
  if (bCmdSuccess)
    {
    CloseHistorian();
    CloseIOMarshal();
    CloseArcManager();
    CloseDrvManager();
    Strng s;
    //Strng PrevRoot = sOldPrjFiles;
    //PrevRoot = PrevRoot.Left(PrevRoot.Length() - 4);
    //PrevRoot += '\\';
    //CPrjSaveAsDlg Dlg(sOldPrjFiles());
    //Dlg.m_CopyHist = true;
    //Dlg.DoModal();
    CWaitCursor Wait;
    //spj/ini stuff...
    WIN32_FIND_DATA fd;
    HANDLE H;
    s = sOldPrjFiles;
    s += "*.ini"; // s += "scd_prj.ini";
    H = FindFirstFile(s(), &fd);
    flag AllDone = (H==INVALID_HANDLE_VALUE);
    while (!AllDone)
      {
      s = sOldPrjFiles;
      s += fd.cFileName;
      Copy_File(s(), PrjFiles());
      AllDone = !FindNextFile(H, &fd);
      }
    FindClose(H);
    //historian...
    Strng DstHstDir(HstMngr.sHstDataDir);
    DstHstDir.FnExpand();
    DstHstDir.FnClearEndBSlash();

    s = sOldPrjFiles;
    s += HstMngr.sHistorianName();
    s += ".hed";
    Copy_File(s(), PrjFiles());
    s = sOldPrjFiles;
    s += HstMngr.sHistorianName();
    s += ".cat";
    Copy_File(s(), PrjFiles());
    if (m_DlgCopyHist)
      {//copy historian data...
      s = OldHstDataDir;
      s += HstMngr.sHstScenName();
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
            s = sOldPrjFiles;
            s += '*';
            s += (j==0 ? CPrjFilesLists::OtherExts(i) : CPrjFilesLists::OtherExtsEx(i));
            H = FindFirstFile(s(), &fd);
            flag AllDone = (H==INVALID_HANDLE_VALUE);
            while (!AllDone)
              {
              s = sOldPrjFiles;
              s += fd.cFileName;
              Copy_File(s(), PrjFiles());
              AllDone = !FindNextFile(H, &fd);
              }
            FindClose(H);
            }
        }
      }
    OpenHistorian();
    OpenDrvManager();
    OpenArcManager();
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
    sOldPrjFile = "";
    sOldPrjFiles = "";
    MainWnd()->PostMessage(WMU_UPDATEMAINWND, SUB_UPDMAIN_ACTIVATECMDS, 0);
    MainWnd()->PostMessage(WMU_CMD, SUB_CMD_PRJWNDUPDATE, 0);
    }
//  else
//    Clear();

  //set document title and update project window...
  return bCmdSuccess;
  }

//---------------------------------------------------------------------------

int Project::ClearSelectedNodes()
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

int Project::BuildSelectedNodeList(Strng_List & List)
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

flag Project::ModelsAttached() { return FlwLib()!= NULL; };

//---------------------------------------------------------------------------

pNodeGrfInfo Project::GetNodeDrawings()
  {
  return FlwLib()!=NULL ? FlwLib()->NodeDrawings()/*SFENodeGrfInfo*/ : NULL;
  }

//---------------------------------------------------------------------------

pchar Project::RequestModelDLLPath()
  {
  return FlwLib()!=NULL ? FlwLib()->RequestModelDLLPath() : NULL;
  };

//---------------------------------------------------------------------------

pchar Project::RequestModelDLLTag()
  {
  return FlwLib()!=NULL ? FlwLib()->RequestModelDLLTag() : NULL;
  };

//---------------------------------------------------------------------------

flag Project::RequestModelInfoByGroupIndex(pchar pGroup, int iIndex, RequestModelInfoRec &Info)
  {
  return FlwLib()!=NULL ? FlwLib()->RequestModelInfoByGroupIndex(pGroup, iIndex, Info) : FALSE;
  };

//---------------------------------------------------------------------------

flag Project::RequestModelInfoByClassId(pchar pClass, RequestModelInfoRec & Info)
  {
  return FlwLib()!=NULL ? FlwLib()->RequestModelInfoByClassId(pClass, Info) : FALSE;
  };

//---------------------------------------------------------------------------

flag Project::RequestModelClassId(pchar pTag, Strng & ClassId)
  {
  return FlwLib()!=NULL ? FlwLib()->RequestModelClassId(pTag, ClassId) : FALSE;
  };

//---------------------------------------------------------------------------

flag Project::RequestModelConnInfo(pchar pTag, RequestConnModelInfoRec & Info)
  {
  return FlwLib()!=NULL ? FlwLib()->RequestModelConnInfo(pTag, Info) : FALSE;
  };

//---------------------------------------------------------------------------

flag Project::RequestModelIOInfoByClassId(pchar pClass, RequestModelIOInfoArray & Info)
  {
  return FlwLib()!=NULL ? FlwLib()->RequestModelIOInfoByClassId(pClass, Info) : FALSE;
  };

//---------------------------------------------------------------------------

flag Project::RequestModelIOInfoByIndex(pchar pTag, int iIndex, RequestModelIOInfoRec & Info)
  {
  return FlwLib()!=NULL ? FlwLib()->RequestModelIOInfoByIndex(pTag, iIndex, Info) : FALSE;
  };

//---------------------------------------------------------------------------

flag Project::RequestModelIOInfoById(pchar pTag, int iId, RequestModelIOInfoRec & Info)
  {
  return FlwLib()!=NULL ? FlwLib()->RequestModelIOInfoById(pTag, iId, Info) : FALSE;
  };

//---------------------------------------------------------------------------

flag Project::RequestModelIOConn(pchar pTag, int iNo, RequestModelIOConnRec & Info)
  {
  return FlwLib()!=NULL ? FlwLib()->RequestModelIOConn(pTag, iNo, Info) : FALSE;
  };

//---------------------------------------------------------------------------

int Project::RequestModelStatus(CModelStatusCriterion &Criteria, CModelStatusArray &Status)
  {
  return FlwLib()!=NULL ? FlwLib()->RequestModelStatus(Criteria, Status) : 0;
  };

//---------------------------------------------------------------------------

int Project::RequestModelStatusChgCnt(CModelStatusCriterion &Criteria, int MaxTest)
  {
  return FlwLib()!=NULL ? FlwLib()->RequestModelStatusChgCnt(Criteria, MaxTest) : 0;
  };

//---------------------------------------------------------------------------

int Project::RequestTagRefInfo(LPCTSTR Tag, CXRefInfoArray &Refs) 
  { 
  return FlwLib()!=NULL ? FlwLib()->RequestTagRefInfo(Tag, Refs) : 0;
  };

//---------------------------------------------------------------------------

//int Project::RequestXRefInfoChg() 
//  { 
//  return FlwLib()!=NULL ? FlwLib()->RequestXRefInfoChg() : 0;
//  };

int Project::GetRctInfo(CRctBlkInformation & RctInfo)
  {
  return FlwLib()!=NULL ? FlwLib()->FE_GetRctInfo(RctInfo) : 0;
  }

int Project::SetRctInfo(LPCTSTR NodeTag)
  {
  return FlwLib()!=NULL ? FlwLib()->FE_SetRctInfo(NodeTag) : 0;
  }

//---------------------------------------------------------------------------

int Project::GetNodeWiring(CNodeWiring &NodeWiring)
  {
  return FlwLib()!=NULL ? FlwLib()->FE_GetNodeWiring(NodeWiring) : -1;
  };

//---------------------------------------------------------------------------

int Project::GetLinkWiring(CLinkWiring &LinkWiring)
  {
  return FlwLib()!=NULL ? FlwLib()->FE_GetLinkWiring(LinkWiring) : -1;
  };

//---------------------------------------------------------------------------

int Project::SetLinkWiring(CLinkWiring &LinkWiring)
  {
  return FlwLib()!=NULL ? FlwLib()->FE_SetLinkWiring(LinkWiring) : -1;
  };
    
//---------------------------------------------------------------------------

flag Project::FlwModelExists(pchar pTag)
  {
  if (FlwLib()!=NULL)
    if (!FlwLib()->FE_TestModelTagUnique(pTag))
      return true;
  return false;
  }

//---------------------------------------------------------------------------

flag Project::TestModelTagUnique(pchar pTag, CUniqueTagRulesBlk & TRB/*pchar pHdr, char SepChar*/, Strng &UniqueTag, flag HdrAsIs)
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

int Project::AddNodeModel(pchar ModelType, pchar SubClass, pchar Tag)
  {
  if (!FlwLib())
    return False;
  return FlwLib()->FE_DoInsert(ModelType, SubClass, NULL, Tag, NULL, NULL);
  }

//---------------------------------------------------------------------------

int Project::DeleteNodeModel(pchar Tag)
  {
  VERIFY(FALSE); //OLD CODE 12/97
  if (!FlwLib())
    return False;
  return FlwLib()->FE_DoDelete(Tag);
  }

//---------------------------------------------------------------------------

int Project::AddNodeConnect(pchar Type, pchar Tag, pchar SrcTag, pchar SrcOut, pchar DstTag, pchar DstIn)
  {
  if (!FlwLib())
    return False;
  Strng Src, Dst;
  Src.Set("%s.%s", SrcTag, SrcOut);
  Dst.Set("%s.%s", DstTag, DstIn);
  return FlwLib()->FE_DoInsert(Type, NULL, NULL, Tag, Src(), Dst());
  }

//---------------------------------------------------------------------------

int Project::AddConnect(pchar SrcTag, pchar SrcOut, pchar DstTag, pchar DstIn)
  {
  if (!FlwLib())
    return False;
  Strng Src, Dst;
  Src.Set("%s.%s", SrcTag, SrcOut);
  Dst.Set("%s.%s", DstTag, DstIn);
  return FlwLib()->FE_DoConnect(Src(), Dst());
  }

//---------------------------------------------------------------------------

int Project::DisConnect(pchar SrcTag, pchar SrcOut, pchar DstTag, pchar DstIn)
  {
  if (!FlwLib())
    return False;
  Strng Src, Dst;
  Src.Set("%s.%s", SrcTag, SrcOut);
  Dst.Set("%s.%s", DstTag, DstIn);
  return FlwLib()->FE_DoDisConnect(Src(), Dst());
  }

//---------------------------------------------------------------------------

int Project::AddConnect(pchar Src, pchar Dst)
  {
  if (!FlwLib())
    return False;
  return FlwLib()->FE_DoConnect(Src, Dst);
  }

//---------------------------------------------------------------------------

int Project::DisConnect(pchar Src, pchar Dst)
  {
  if (!FlwLib())
    return False;
  return FlwLib()->FE_DoDisConnect(Src, Dst);
  }

//---------------------------------------------------------------------------

DWORD Project::get_RecordTag(LPCSTR Tag)
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

flag Project::put_RecordTag(LPCSTR Tag, flag RecordingOn, DWORD & RetCode)
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
