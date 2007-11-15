//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
#include "gpwfuncs.h"
//#include "appbase.h"
#include "project.h"
#include "gendlgs.h"
#include "toolbars.h"
#include "mainfrm.h"
#include "fixedit.h"
#include "drwhdr.h"
#include "grf3drw.h"
#include "cmdmenu.h"
#include "sfe_clnt.h"
#include "basecmds.h"
#include "accnode.h"
#include "drv_mngr.h"
#include "archmngr.h"
#include "cmd_mngr.h"
#include "dbgmngr.h"
#include "scd_wm.h"
#include "chngtag.h"
#include "slvcfg.h"
#include "slvtool.h"
#include "mdlrunmngr.h"
#include "licbase.h"
#if CK_LICENSINGON
#include "dlgbusy.h"
#endif
#include "analdsgn.h"
#include "sp_db.h"
#include "sdbedit.h"
#include "prjdlgs.h"
#include "msgwnd.h"
#include "prjdoc.h"
#include "prjview.h"
#include "scdver.h" // WITHCTRLOBJ-EDIT
#include "tagvdoc.h"
#include "ordwnd.h"
#include "statswnd.h"
#include "PrjDCmp.h"
#include "WndSlct.h"
#include "explorescd.h"
#include "rep_mngr.h"
#include "scdcmdif.h"
#include ".\mainfrm.h"
#include ".\opcsrvrwrapper.h"
#include "neutralgrf.h"
#include "neutraldlgs.h"
#include "rcteditdlg.h"
#include "bulktagchange.h"

#define dbgMainframe  01

#if dbgMainframe
static CDbgMngr dbgActions  ("MainFrame", "Actions");
#endif

//#include "optoff.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//===========================================================================

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
  //{{AFX_MSG_MAP(CMainFrame)
  ON_WM_CREATE()
  ON_WM_CLOSE()
  ON_WM_ACTIVATE()
  ON_WM_INITMENU()

  ON_COMMAND(ID_PROJECT_OPTIONS, OnProjectOptions)
  ON_COMMAND(ID_OPTIONS_BROWSETAGS, OnOptionsBrowsetags)
  ON_COMMAND(ID_OPTIONS_HISTORIANQUERY, OnOptionsHistorianQuery)
#if WITHDRVMAN
  ON_COMMAND(ID_OPTIONS_DRIVER, OnOptionsDriver)
#endif
  ON_COMMAND(ID_OPTIONS_ARCHIVE, OnOptionsArchive)
  ON_COMMAND(ID_FILE_SAVESNAPSHOT, OnFileSavesnapshot)
  ON_COMMAND(ID_FILE_LOADSNAPSHOT, OnFileLoadsnapshot)
  ON_COMMAND(ID_FILE_SAVESCENARIO, OnFileSavescenario)
  ON_COMMAND(ID_FILE_LOADSCENARIO, OnFileLoadscenario)
  ON_COMMAND(ID_PROJECT_SAVEAS, OnProjectSaveas)
  ON_COMMAND(ID_PROJECT_SAVEASNV, OnProjectSaveasNV)
  ON_COMMAND(ID_CHANGE_TAG, OnChangeTag)
  ON_UPDATE_COMMAND_UI(ID_CHANGE_TAG, OnUpdateChangeTag)
  ON_COMMAND(ID_EDIT_BULKCHANGETAG, OnBulkChangeTag)
  ON_UPDATE_COMMAND_UI(ID_EDIT_BULKCHANGETAG, OnUpdateBulkChangeTag)
  ON_COMMAND(ID_FILE_SAVELAYOUT, OnFileSavelayout)
  ON_COMMAND(ID_FIND_TAG, OnFindTag)
  ON_COMMAND(ID_FIND_NEXT, OnFindNext)
  ON_COMMAND(ID_WNDW_ARRANGESPECIAL, OnWindowArrange)
  ON_COMMAND(ID_WNDW_CASCADE, OnWindowCascade)
  ON_COMMAND(ID_WNDW_TILE, OnWindowTile)
  ON_COMMAND(ID_WNDW_ARRANGE, OnWindowArrangeIcons)
  ON_COMMAND(ID_OPTIONS_REPORTS, OnOptionsReports)
  ON_COMMAND(ID_ACTIONS_Analyse, OnAnalyse)
  ON_UPDATE_COMMAND_UI(ID_ACTIONS_Analyse, OnUpdateAnalyse)
  ON_COMMAND(ID_PROJECT_EMPTY, OnProjectEmpty)
  ON_UPDATE_COMMAND_UI(ID_PROJECT_EMPTY, OnUpdateProjectEmpty)
  ON_COMMAND(ID_PROJECT_ZEROFLOWS, OnProjectZeroFlows)
  ON_UPDATE_COMMAND_UI(ID_PROJECT_ZEROFLOWS, OnUpdateProjectZeroFlows)
  ON_COMMAND(ID_PROJECT_STEADYSTATE, OnProjectSteadystate)
  ON_UPDATE_COMMAND_UI(ID_PROJECT_STEADYSTATE, OnUpdateProjectSteadystate)
  ON_UPDATE_COMMAND_UI(ID_FILE_LOADSNAPSHOT, OnUpdateFileLoadsnapshot)
  ON_UPDATE_COMMAND_UI(ID_FILE_SAVESNAPSHOT, OnUpdateFileSavesnapshot)
  ON_UPDATE_COMMAND_UI(ID_FILE_LOADSCENARIO, OnUpdateFileLoadscenario)
  ON_UPDATE_COMMAND_UI(ID_FILE_SAVESCENARIO, OnUpdateFileSavescenario)
  ON_UPDATE_COMMAND_UI(ID_FILE_SAVELAYOUT, OnUpdateFileSavelayout)
  ON_UPDATE_COMMAND_UI(ID_OPTIONS_BROWSETAGS, OnUpdateOptionsBrowsetags)
  ON_UPDATE_COMMAND_UI(ID_OPTIONS_HISTORIANQUERY, OnUpdateOptionsHistorianQuery)
#if WITHDRVMAN
  ON_UPDATE_COMMAND_UI(ID_OPTIONS_DRIVER, OnUpdateOptionsDriver)
#endif
  ON_UPDATE_COMMAND_UI(ID_OPTIONS_ARCHIVE, OnUpdateOptionsArchive)
  ON_UPDATE_COMMAND_UI(ID_OPTIONS_REPORTS, OnUpdateOptionsReports)
  ON_UPDATE_COMMAND_UI(ID_PROJECT_SAVEAS, OnUpdateProjectSaveas)
  ON_UPDATE_COMMAND_UI(ID_PROJECT_SAVEASNV, OnUpdateProjectSaveasNV)
  ON_COMMAND(ID_PROJECT_SETTINGS, OnProjectSettings)
  ON_UPDATE_COMMAND_UI(ID_PROJECT_SETTINGS, OnUpdateProjectSettings)
  ON_UPDATE_COMMAND_UI(ID_FIND_TAG, OnUpdateFindTag)
  ON_UPDATE_COMMAND_UI(ID_FIND_NEXT, OnUpdateFindNext)
  ON_UPDATE_COMMAND_UI(ID_PROJECT_OPTIONS, OnUpdateProjectOptions)
  ON_COMMAND(ID_OPTIONS_CMDSCRIPTS, OnOptionsCmdscripts)
  ON_COMMAND(ID_OPTIONS_COMPARE, OnOptionsCompare)
  ON_UPDATE_COMMAND_UI(ID_OPTIONS_COMPARE, OnUpdateOptionsCompare)
  //ON_WM_INITMENU()
  ON_UPDATE_COMMAND_UI(ID_OPTIONS_CMDSCRIPTS, OnUpdateOptionsCmdscripts)
  ON_COMMAND(ID_PROJECT_MERGE, OnProjectMerge)
  ON_UPDATE_COMMAND_UI(ID_PROJECT_MERGE, OnUpdateProjectMerge)
  ON_COMMAND(ID_VIEW_EXPLORER, OnExplorer)
  ON_UPDATE_COMMAND_UI(ID_VIEW_EXPLORER, OnUpdateExplorer)
  ON_COMMAND(ID_WNDW_SELECT, OnWindowSelectwindow)
  ON_UPDATE_COMMAND_UI(ID_WNDW_SELECT, OnUpdateWindowSelectwindow)
  ON_UPDATE_COMMAND_UI(ID_WNDW_ARRANGESPECIAL, OnUpdateWindowArrange)
  ON_COMMAND(ID_PROJECT_PRESET, OnProjectPreset)
  ON_UPDATE_COMMAND_UI(ID_PROJECT_PRESET, OnUpdateProjectPreset)
  ON_COMMAND(ID_PROJECT_RESETSTATS, OnProjectResetStats)
  ON_UPDATE_COMMAND_UI(ID_PROJECT_RESETSTATS, OnUpdateProjectResetStats)
  ON_COMMAND(ID_TAGDATATRANSFER, OnTagDataTransfer)
  ON_UPDATE_COMMAND_UI(ID_TAGDATATRANSFER, OnUpdateTagDataTransfer)
  ON_COMMAND(ID_PROJECT_LOADALTCFG, OnProjectLoadaltcfg)
  ON_UPDATE_COMMAND_UI(ID_PROJECT_LOADALTCFG, OnUpdateProjectLoadaltcfg)
  ON_COMMAND(ID_OPTIONS_DDE, OnOptionsDDE)
  ON_UPDATE_COMMAND_UI(ID_OPTIONS_DDE, OnUpdateOptionsDDE)
  ON_COMMAND(ID_OPTIONS_OPCSERVEROPTIONS, OnOptionsOPCServer)
  ON_UPDATE_COMMAND_UI(ID_OPTIONS_OPCSERVEROPTIONS, OnUpdateOptionsOPCServer)
  ON_COMMAND(ID_PROJECT_EDIT_CFG, OnProjectEditCfg)
  ON_UPDATE_COMMAND_UI(ID_PROJECT_EDIT_CFG, OnUpdateProjectEditCfg)
  ON_COMMAND(ID_PROJECT_EDIT_SDB, OnProjectEditSdb)
  ON_UPDATE_COMMAND_UI(ID_PROJECT_EDIT_SDB, OnUpdateProjectEditSdb)
  ON_COMMAND(ID_PROJECT_EDIT_DEFSDB, OnProjectEditDefSdb)
  ON_UPDATE_COMMAND_UI(ID_PROJECT_EDIT_DEFSDB, OnUpdateProjectEditDefSdb)
  ON_COMMAND(ID_VIEW_POWERSUPPLIES, OnViewPowersupplies)
  ON_UPDATE_COMMAND_UI(ID_VIEW_POWERSUPPLIES, OnUpdateViewPowersupplies)
  ON_COMMAND(ID_VIEW_IOPOINTS, OnViewIOPoints)
  ON_UPDATE_COMMAND_UI(ID_VIEW_IOPOINTS, OnUpdateViewIOPoints)
  ON_COMMAND(ID_VIEW_CHANGESTATS, OnViewChangestats)
  ON_UPDATE_COMMAND_UI(ID_VIEW_CHANGESTATS, OnUpdateViewChangestats)
  ON_COMMAND(ID_VIEW_EVALORDER, OnViewEvalorder)
  ON_UPDATE_COMMAND_UI(ID_VIEW_EVALORDER, OnUpdateViewEvalorder)
  ON_COMMAND(ID_VIEW_EVALSTATS, OnViewEvalstats)
  ON_UPDATE_COMMAND_UI(ID_VIEW_EVALSTATS, OnUpdateViewEvalstats)
  ON_COMMAND(ID_VIEW_CONTROLS, OnViewControls)
  ON_UPDATE_COMMAND_UI(ID_VIEW_CONTROLS, OnUpdateViewControls)
  ON_COMMAND(ID_VIEW_DYNAMICINFO, OnViewDynamicinfo)
  ON_UPDATE_COMMAND_UI(ID_VIEW_DYNAMICINFO, OnUpdateViewDynamicinfo)
  ON_COMMAND(ID_VIEW_PLANTMODEL, OnViewPlantmodel)
  ON_UPDATE_COMMAND_UI(ID_VIEW_PLANTMODEL, OnUpdateViewPlantmodel)
  ON_COMMAND(ID_VIEW_PLANTAREA, OnViewPlantarea)
  ON_UPDATE_COMMAND_UI(ID_VIEW_PLANTAREA, OnUpdateViewPlantarea)
  ON_COMMAND(ID_VIEW_PROBALINFO, OnViewProbalinfo)
  ON_UPDATE_COMMAND_UI(ID_VIEW_PROBALINFO, OnUpdateViewProbalinfo)
  ON_COMMAND(ID_VIEW_SPECIEDATA, OnViewSpeciedata)
  ON_UPDATE_COMMAND_UI(ID_VIEW_SPECIEDATA, OnUpdateViewSpeciedata)
  ON_COMMAND(ID_VIEW_TEARBLOCKS, OnViewTearblocks)
  ON_UPDATE_COMMAND_UI(ID_VIEW_TEARBLOCKS, OnUpdateViewTearblocks)
  ON_COMMAND(ID_VIEW_TOLERANCES, OnViewTolerances)
  ON_UPDATE_COMMAND_UI(ID_VIEW_TOLERANCES, OnUpdateViewTolerances)
  ON_COMMAND(ID_VIEW_RPTTAGLISTS, OnViewRptTagLists)
  ON_UPDATE_COMMAND_UI(ID_VIEW_RPTTAGLISTS, OnUpdateViewRptTagLists)
  ON_COMMAND(ID_VIEW_MESSAGES, OnViewMessages)
  ON_UPDATE_COMMAND_UI(ID_VIEW_MESSAGES, OnUpdateViewMessages)
  ON_COMMAND(ID_VIEW_PROJECT, OnViewProject)
  ON_UPDATE_COMMAND_UI(ID_VIEW_PROJECT, OnUpdateViewProject)
  ON_COMMAND(IDM_ERROR, OnError)
  ON_UPDATE_COMMAND_UI(IDM_ERROR, OnUpdateError)
  ON_COMMAND(IDM_WARNING, OnWarning)
  ON_UPDATE_COMMAND_UI(IDM_WARNING, OnUpdateWarning)
  ON_WM_ACTIVATEAPP()
  ON_COMMAND(ID_EXEC_DumpMsgQueue, OnEXECDumpMsgQueue)
  ON_WM_SIZE()
  ON_WM_MOVE()
  ON_COMMAND(ID_PROJECT_COMPARE_DATA, OnProjectCompareData)
  ON_UPDATE_COMMAND_UI(ID_PROJECT_COMPARE_DATA, OnUpdateProjectCompareData)
#if WITHDRVMAN
  ON_COMMAND(ID_RELOADDRIVER, OnReloadDriver)
  ON_UPDATE_COMMAND_UI(ID_RELOADDRIVER, OnUpdateReloadDriver)
#endif
  ON_COMMAND(ID_ACTIONS_PROBALSETUP, OnActionsProbalSetup)
  ON_UPDATE_COMMAND_UI(ID_ACTIONS_PROBALSETUP, OnUpdateActionsProbalSetup)
  ON_COMMAND(ID_ACTIONS_DYNAMICSETUP, OnActionsDynamicSetup)
  ON_UPDATE_COMMAND_UI(ID_ACTIONS_DYNAMICSETUP, OnUpdateActionsDynamicSetup)
  ON_COMMAND(ID_ACTIONS_SETUP, OnActionsSetup)
  ON_UPDATE_COMMAND_UI(ID_ACTIONS_SETUP, OnUpdateActionsSetup)
  ON_COMMAND(ID_ACTIONS_EDIT, OnActionsEdit)
  ON_UPDATE_COMMAND_UI(ID_ACTIONS_EDIT, OnUpdateActionsEdit)
  ON_COMMAND(ID_ACTIONS_RUN, OnActionsRun)
  ON_UPDATE_COMMAND_UI(ID_ACTIONS_RUN, OnUpdateActionsRun)
  ON_COMMAND(ID_ACTIONS_RUNPB, OnActionsRunPB)
  ON_UPDATE_COMMAND_UI(ID_ACTIONS_RUNPB, OnUpdateActionsRunPB)
  ON_COMMAND(ID_ACTIONS_RUNDYN, OnActionsRunDyn)
  ON_UPDATE_COMMAND_UI(ID_ACTIONS_RUNDYN, OnUpdateActionsRunDyn)
  ON_COMMAND(ID_ACTIONS_RUN_STEADY, OnActionsRunSteady)
  ON_UPDATE_COMMAND_UI(ID_ACTIONS_RUN_STEADY, OnUpdateActionsRunSteady)
  ON_COMMAND(ID_ACTIONS_IDLE, OnActionsIdle)
  ON_UPDATE_COMMAND_UI(ID_ACTIONS_IDLE, OnUpdateActionsIdle)
  ON_COMMAND(ID_ACTIONS_STEP, OnActionsStep)
  ON_UPDATE_COMMAND_UI(ID_ACTIONS_STEP, OnUpdateActionsStep)
  ON_COMMAND(ID_ACTIONS_SOLVENET, OnActionsSolveNet)
  ON_UPDATE_COMMAND_UI(ID_ACTIONS_SOLVENET, OnUpdateActionsSolveNet)
  ON_COMMAND(ID_ACTIONS_EDIT_TGL, OnActionsEditTgl)
  ON_UPDATE_COMMAND_UI(ID_ACTIONS_EDIT_TGL, OnUpdateActionsEditTgl)
  ON_COMMAND(ID_ACTIONS_RUN_TGL, OnActionsRunTgl)
  ON_UPDATE_COMMAND_UI(ID_ACTIONS_RUN_TGL, OnUpdateActionsRunTgl)
  ON_COMMAND(ID_ACTIONS_RUNPB_TGL, OnActionsRunTgl)
  ON_UPDATE_COMMAND_UI(ID_ACTIONS_RUNPB_TGL, OnUpdateActionsRunPBTgl)
  ON_COMMAND(ID_ACTIONS_RUNDYN_TGL, OnActionsRunTgl)
  ON_UPDATE_COMMAND_UI(ID_ACTIONS_RUNDYN_TGL, OnUpdateActionsRunDynTgl)
  ON_COMMAND(ID_GRF_ExportDB, OnExportDB)
  ON_UPDATE_COMMAND_UI(ID_GRF_ExportDB, OnUpdateExportDB)
  ON_COMMAND(ID_GRF_ImportDB, OnImportDB)
  ON_UPDATE_COMMAND_UI(ID_GRF_ImportDB, OnUpdateImportDB)
  ON_COMMAND(ID_PROJECT_EDIT_COLOURS, OnProjectEditColours)
  ON_UPDATE_COMMAND_UI(ID_PROJECT_EDIT_COLOURS, OnUpdateProjectEditColours)
  //}}AFX_MSG_MAP
  ON_UPDATE_COMMAND_UI(ID_WNDW_CASCADE, OnUpdateWindowCmd)
  ON_UPDATE_COMMAND_UI(ID_WNDW_TILE, OnUpdateWindowCmd)
  ON_UPDATE_COMMAND_UI(ID_WNDW_ARRANGE, OnUpdateWindowCmd)
  ON_WM_SYSCOMMAND()
  ON_COMMAND_RANGE(IDM_MDLHELPFIRST, IDM_MDLHELPLAST, OnMdlHelpMenu)
  ON_COMMAND(ID_OPTIONS_DebugVisible, OnOptionsDebugVisible)
#if WITHSCRCYCLES
  ON_WM_TIMER()
  ON_COMMAND(ID_VIEW_SCREENSAVER, OnViewScreensaver)
  ON_UPDATE_COMMAND_UI(ID_VIEW_SCREENSAVER, OnUpdateViewScreensaver)
  ON_MESSAGE(WMU_STARTCYCLES, OnStartCycles)
  ON_MESSAGE(WMU_ENDCYCLES, OnEndCycles)
#endif
  ON_MESSAGE(WMU_UPDATEMAINWND, OnUpdateMainWnd)
  ON_MESSAGE(WMU_HSTUPDATESTATS, OnHstUpdateStats)
  ON_MESSAGE(WMU_TAGACTION, OnTagAction)
  ON_MESSAGE(WMU_REPORT, OnGenerateReport)
  ON_MESSAGE(WMU_CMD, OnCmd)
  ON_MESSAGE(WMU_CMDDONE, OnCmdDone)
  ON_MESSAGE(WMU_CMDNEXT, OnCmdNext)
  ON_MESSAGE(WMU_CMDSTATE, OnCmdState)
  ON_MESSAGE(WMU_COM_CMD, OnCOMCmd)
  ON_MESSAGE(WMU_COM_APP, OnCOMApp)
  ON_MESSAGE(WMU_COM_SLV, OnCOMSlv)
  ON_MESSAGE(WMU_COMEVTRELAY, OnCOMEvtRelay)
  ON_MESSAGE(WMU_OLEINFOUPDATE, OnOLEInfoUpdate)
  ON_MESSAGE(WMU_ADDTOQUICKVIEW, OnAddToRptTagLists)
//  ON_MESSAGE(WMU_ADDTOHISTORY, OnAddToHistory)
  ON_MESSAGE(WMU_UPDATEMSGMENUBTN, OnUpdateMsgMenuBtn)
  ON_MESSAGE(WMU_FLUSHMSGQ, OnFlushMsgQ)
  ON_MESSAGE(WMU_EXECUPDATEDISPLAY, OnExecUpdateDisplay)
#if WITHDRVMAN
  ON_MESSAGE(WMU_RELOADDRIVER, OnReloadDriver)
#endif
  ON_MESSAGE(WMU_GRFTAGGROUPS, OnGrfTagGroups)
  ON_MESSAGE(WMU_NODEAUDIT, OnNodeAudit)
  ON_MESSAGE(WMU_EDITRCTDLG, OnEditRctDlg)

  //ON_MESSAGE(WMU_EXECACTION, OnExecAction)
  ON_MESSAGE(WMU_EXCEPTION, OnException)
  ON_MESSAGE(WMU_SETLICENSE, OnSetLicense)
  ON_MESSAGE(WMU_CHKLICENSE, OnChkLicense)

  // Global help commands
  ON_COMMAND(ID_HELP_FINDER, CMDIFrameWnd::OnHelpFinder)
  ON_COMMAND(ID_HELP, CMDIFrameWnd::OnHelp)
  ON_COMMAND(ID_CONTEXT_HELP, CMDIFrameWnd::OnContextHelp)
  ON_COMMAND(ID_DEFAULT_HELP, CMDIFrameWnd::OnHelpFinder)
  ON_WM_MEASUREITEM()
  ON_WM_DRAWITEM()
  ON_COMMAND(ID_FILE_IMPORT, OnFileImport)
  ON_COMMAND(ID_FILE_EXPORT, OnFileExport)
  ON_UPDATE_COMMAND_UI(ID_FILE_IMPORT, OnUpdateFileImport)
  ON_UPDATE_COMMAND_UI(ID_FILE_EXPORT, OnUpdateFileExport)
  ON_COMMAND(ID_DELETEDEBUGFILE, OnDeleteDebugFile)
  ON_COMMAND(ID_SCD10_EXPORT, &CMainFrame::OnScd10Export)
  ON_UPDATE_COMMAND_UI(ID_SCD10_EXPORT, &CMainFrame::OnUpdateScd10Export)
  END_MESSAGE_MAP()

//---------------------------------------------------------------------------

static UINT BASED_CODE indicators[] =
  {
  ID_SEPARATOR,     // status line indicator
  ID_INDICATOR_0,   // current document
  ID_INDICATOR_1,   // graphics position/SysCAD simulation time
  ID_INDICATOR_2,   // state: stopped/starting/building/step size
  ID_INDICATOR_3,   // number of iterations
  ID_INDICATOR_4,   // oper status
  ID_INDICATOR_5,   // used sometimes
  ID_INDICATOR_6,   // used sometimes
  };

//---------------------------------------------------------------------------

CToolBarMngr GrfTBMngr;
CToolBarMngr MainTBMngr;

//---------------------------------------------------------------------------

inline flag EnablePrjOK() { return gs_pPrj && gs_pPrj->pPrjDoc; };
inline flag EnableFiling() { return EnablePrjOK() && XFiling(); };
inline flag EnableNotFiling() { return !XFiling(); };
inline flag EnableBusy() { return EnablePrjOK() && XBusy(); }; //starting, running, paused, or stopping
inline flag EnableNotBusy() { return EnablePrjOK() && !XBusy(); };
inline flag EnableAnalysing() { return EnablePrjOK() && gs_pPrj->bBusyAnalysing; };
inline flag EnableNotAnalysing() { return EnablePrjOK() && !gs_pPrj->bBusyAnalysing; };
#if WITHDRVMAN
inline flag EnableDriverOK() { return EnablePrjOK() && gs_pPrj->bDrvOn; };
#endif
inline flag EnableDDEServerOK() { return EnablePrjOK() && gs_pPrj->pDDESrvr; };
inline flag EnableOPCServerOK() { return EnablePrjOK() && gs_pMainOPCSrvr; };//gs_pPrj->pOPCSrvr; };
inline flag EnableArchiverOK() { return EnablePrjOK() && gs_pPrj->m_bArcOn; };
inline flag EnableHistorianOK() { return EnablePrjOK() && gs_pPrj->bHstOK; };
inline flag EnableIOMarshalOK() { return EnablePrjOK() && gs_pPrj->m_bIOMOn; };
inline flag EnableNotStopped() { return gs_pCmd && !gs_pCmd->IsDlgBusy(); };

//---------------------------------------------------------------------------

CMainFrame::CMainFrame()
  {
  m_pGenSheet = NULL;
  m_pPrjSheet = NULL;
  m_pReportsDlg = NULL;
  m_pCmdScriptsDlg = NULL;
  m_pCompareResDlg = NULL;

  hCfgEditProcess=NULL;
  dwCfgEditProcessId=0;

  m_ErrorBitmap.LoadMappedBitmap(IDB_IMGERROR);
  m_WarnBitmap.LoadMappedBitmap(IDB_IMGWARNING);
  m_NoteBitmap.LoadMappedBitmap(IDB_IMGNOTE);
  m_ErrorBWBitmap.LoadBitmap(IDB_IMGERRORBW);
  m_WarnBWBitmap.LoadBitmap(IDB_IMGWARNINGBW);
  m_NoteBWBitmap.LoadBitmap(IDB_IMGNOTEBW);
  //m_MsgIcon.LoadBitmap(IDI_MSGWND);
  m_MsgIcon.LoadMappedBitmap(IDI_MSGWND);


  m_nMsgs[0]=0;
  m_nMsgs[1]=0;
  m_nMsgs[2]=0;

  m_GoodStr="-";
  m_BadStr="-";
  m_StepStr="-";
  }

//---------------------------------------------------------------------------

CMainFrame::~CMainFrame()
  {
  #if WITHSCRCYCLES
  if (gs_pPrj && gs_pPrj->iCycleNo>=0)
    gs_pPrj->EndScreenSave();
  #endif
  }

//---------------------------------------------------------------------------

void CMainFrame::ClosingProject(flag ForExit)
  {
  if (gs_pPrj->pDDESrvr)
    gs_pPrj->pDDESrvr->CloseOptions();
  //if (gs_pPrj->pOPCSrvr)
  //  gs_pPrj->pOPCSrvr->CloseOptions();

  if (!ForExit)
    {
    if (m_pGenSheet)
      m_pGenSheet->DestroyWindow();
    if (m_pPrjSheet)
      ((CWnd*)m_pPrjSheet)->DestroyWindow();
    if (m_pReportsDlg)
      m_pReportsDlg->DestroyWindow();
    if (m_pCmdScriptsDlg)
      m_pCmdScriptsDlg->DestroyWindow();
    if (m_pCompareResDlg)
      m_pCompareResDlg->DestroyWindow();
    }
  }

//---------------------------------------------------------------------------

flag CMainFrame::EO_Starting(flag fBeginStarting)
  {
  if (fBeginStarting)
    BeginWaitCursor();
  else
    EndWaitCursor();
  return True;
  }

//---------------------------------------------------------------------------

flag CMainFrame::EO_Stopping(flag fBeginStopping)
  {
  if (fBeginStopping)
    BeginWaitCursor();
  else
    EndWaitCursor();
  return True;
  }

//---------------------------------------------------------------------------

flag CMainFrame::EO_RequestTagInfo(RequestTagInfoRec& Rqst, ReplyTagInfoRec& Info)
  {
  static int ReqScriptsPos = -1;
  switch (Rqst.RQ_Type)
    {
    case RQ_Files:
      {
      if (Info.Count()==0)
        ReqScriptsPos = -1;
      if (ReqScriptsPos==-1)
        {
        CRPFLIArray* pList;
        if (Info.Count()==0)
          {
          pList = new CRPFLIArray;
          Info.SetPos((long)pList);
          int RSel, SSel, MSel;
          CReportsDlg::BuildList(*pList, RSel, SSel, MSel);
          }
        else
          pList = (CRPFLIArray*)Info.GetPos();
        ASSERT(pList);
        if (Info.Count()<pList->GetSize())
          {
          Info.bCanEditFile = false;
          Info.bExcelFile = true;
          Info.bForceCopy = true;
          Info.bPreventCopy = false;
          Info.bDWDataValid = 1;
          Info.dwData = 11;
          CReportsProfileLineInfo* pRPLI = pList->GetAt(Info.Count());
          Info.sData = pRPLI->FullFilename();
          Info.sData.FnContract();
          delete pRPLI;
          return TRUE;
          }
        else
          {
          delete pList;
          ReqScriptsPos = Info.Count();
          CSPFLIArray* pLst = new CSPFLIArray;
          Info.SetPos((long)pLst);
          CScriptsDlg::BuildList(*pLst);
          }
        }
      CSPFLIArray* pLst = (CSPFLIArray*)Info.GetPos();
      ASSERT(pLst);
      if (Info.Count()-ReqScriptsPos<pLst->GetSize())
        {
        Info.bCanEditFile = true;
        Info.bExcelFile = false;
        Info.bForceCopy = true;
        Info.bPreventCopy = false;
        Info.bDWDataValid = true;
        Info.dwData = 12;
        CScriptsProfileLineInfo* pSPLI = pLst->GetAt(Info.Count()-ReqScriptsPos);
        Info.sData = pSPLI->FullFilename();
        Info.sData.FnContract();
        delete pSPLI;
        return TRUE;
        }
      else
        {
        delete pLst;
        Info.SetPos(0);
        }
      break;
      }
    }
  return False;
  }

//---------------------------------------------------------------------------

flag CMainFrame::EO_QueryTime(CXM_TimeControl &CB, CTimeValue &TimeRqd, CTimeValue &dTimeRqd)
  {
  return True;
  };

//---------------------------------------------------------------------------

flag CMainFrame::EO_Start(CXM_TimeControl &CB)
  {
//  gs_TheRunMngr.DoEO_Starting();
  return True;
  };

//---------------------------------------------------------------------------

void CMainFrame::EO_QuerySubsReqd(CXMsgLst &XM)
  {
  const pchar RqdTags[] = {"$Dyn.GoodIters",
                           "$Dyn.BadIters",
                           "$Dyn.Steps",
                           NULL};
  CXM_ReadIndexedData * pMsg=(CXM_ReadIndexedData *)XM.MsgPtr(XM_ReadIndexedData);
  flag ReadAll=pMsg->ReadAll;
  long DataIndex=pMsg->Start ? 0 : pMsg->LastIndex+1;

  Strng Tg;
  XM.Clear();
  for (; RqdTags[DataIndex]; DataIndex++)
    {
    Tg=RqdTags[DataIndex];

    CXM_DataRequest *DRqst=new CXM_DataRequest (DataIndex, Tg(), TABOpt_AllInfoOnce, XIO_In);
    if (!XM.PackMsg(DRqst))
      {
      delete DRqst;
      break;
      }
    }
  };

//---------------------------------------------------------------------------

void CMainFrame::EO_QuerySubsAvail(CXMsgLst &XM, CXMsgLst &XMRet)
  {
  //XM.Clear();
  };

//---------------------------------------------------------------------------

flag CMainFrame::EO_ReadSubsData(CXMsgLst &XM)
  {
  flag DataRead=0;
  return DataRead;
  };

//---------------------------------------------------------------------------

flag CMainFrame::EO_WriteSubsData(CXMsgLst &XM, flag FirstBlock, flag LastBlock)
  {
  if (FirstBlock)
    {
    }

  for (long i=0; i<XM.NoMsgs(); i++)
    {
    CXM_ObjectData* pMsg = XM.ObjectData();
    long DataIndex=pMsg->Index;
    CPkDataItem* pItem=pMsg->FirstItem();
    PkDataUnion* pData=pMsg->FirstItem()->Value();
    if (IsData(pData->Type()))
      {
      char* pStr=pData->GetString("%i", "%5.2f");
      switch (DataIndex)
        {
        case 0: m_GoodStr = pStr; break;
        case 1: m_BadStr = pStr; break;
        case 2: m_StepStr = pStr; break;
        }
      }
    }

  if (DefNetDynamicMode())
    {
    char Buff[256];
    strcpy(Buff, "S:");
    strcat(Buff, m_StepStr());
    if (m_GoodStr!=m_StepStr)
      {
      strcat(Buff, " G:");
      strcat(Buff, m_GoodStr());
      }
    if (m_BadStr!="0")
      {
      strcat(Buff, " B:");
      strcat(Buff, m_BadStr());
      }
    pStatusBar->UpdateIndicator(3, Buff, TRUE);
    }
  return True;
  };

//---------------------------------------------------------------------------

flag CMainFrame::EO_Execute(CXM_TimeControl &CB, CEOExecReturn &EORet)
  {
  //gs_TheRunMngr.DoEO_Execute();
  return False;
  };

//---------------------------------------------------------------------------

flag CMainFrame::EO_Stop(CXM_TimeControl &CB)
  {
  return True;
  };

//---------------------------------------------------------------------------

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
  {
  int Bord=GetSystemMetrics(SM_CYCAPTION);
  cs.x=Bord;
  cs.y=Bord;
  cs.cx=GetSystemMetrics(SM_CXSCREEN)-2*Bord;
  cs.cy=GetSystemMetrics(SM_CYSCREEN)-2*Bord;

  cs.style |= FWS_PREFIXTITLE;
  //CString MainFrameClass;
  //MainFrameClass.LoadString(IDS_MAINFRAMECLASS);
  //
  //cs.lpszClass=(const char *)MainFrameClass;

  return CMDIFrameWnd::PreCreateWindow(cs);
  }

//---------------------------------------------------------------------------

void CMainFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
  {
  // Override CMDIFrameWnd::OnUpdateFrameTitle(BOOL bAddToTitle)

  if ((GetStyle() & FWS_ADDTOTITLE) == 0)
    return;     // leave it alone!

  Strng Txt;

  if (gs_License.Blocked())
    Txt="LICENSE EXCEEDED ";
  else if (gs_License.DemoMode())
    Txt="DEMO MODE ";
  else if (gs_License.AcademicMode())
    Txt="ACADEMIC MODE ";

  if (PrjName())
    {
    Txt+=PrjName();
    if (UsingPrjLclFiles())
      Txt+="(Lcl)";
    if (DefNetProbalMode())
      Txt+= " - ProBal ";
    else if (DefNetDynamicMode())
      Txt+= " - Dynamic";
    else
      Txt+= " - ?????";
    }
  //#if _MSC_VER < 1400
  //Txt+= " [VS2003]";
  //#endif
  #if _MSC_VER >= 1400
  Txt+= " [VS2005]"; //revert to above when #785 speed issue resolved
  #endif
  UpdateFrameTitleForDocument(Txt());
  }

//---------------------------------------------------------------------------

#ifdef _DEBUG
void CMainFrame::AssertValid() const
  {
  CMDIFrameWnd::AssertValid();
  }

void CMainFrame::Dump(CDumpContext& dc) const
  {
  CMDIFrameWnd::Dump(dc);
  }

#endif //_DEBUG

//---------------------------------------------------------------------------

extern void ReleaseCOMIfNeccessary(); // In SysCAD

void CMainFrame::OnClose()
  {
  if (gs_Exec.TestRunning())
    return;
  if (!ScdApp()->CanClose())
    return;
  if (ScdApp()->ShutDown())
    {
    //ReleaseCOMIfNeccessary(); // In SysCAD
    CMDIFrameWnd::OnClose();
    }
  }

//---------------------------------------------------------------------------

LRESULT CMainFrame::OnUpdateMainWnd(WPARAM wParam, LPARAM lParam)
  {
//#define SUB_UPDMAIN_UPDATE            1 //update main window and bring window specified by lParam (if any) to top
//#define SUB_UPDMAIN_PRJLOADED         2 //project just loaded
//#define SUB_UPDMAIN_ACTIVATECMDS      3 //ensure top window commands are active
//#define SUB_UPDMAIN_BACKGROUND        4 //refresh main window frame background
//#define SUB_UPDMAIN_ACTIVATEAPP       5 //same as SUB_UPDMAIN_UPDATE but also activate syscad
//#define SUB_UPDMAIN_STARTED           6 //SysCAD startup completed
  ASSERT(wParam!=0);
  const bool BringToForeground = (wParam==SUB_UPDMAIN_ACTIVATEAPP);
  if (wParam==SUB_UPDMAIN_ACTIVATEAPP)
    wParam = SUB_UPDMAIN_UPDATE;
  if (wParam==SUB_UPDMAIN_PRJLOADED && gs_pPrj)
    {//just finished loading project
    gs_pPrj->RestoreWindowZOrder();
    if (gs_pPrj->pPrjDoc)
      gs_pPrj->pPrjDoc->UpdateAllViews(NULL, 1);
    }
  if (wParam==SUB_UPDMAIN_PRJLOADED || wParam==SUB_UPDMAIN_ACTIVATECMDS)
    {//ensure the current top window has its commands (toolbars) enabled...
    CWindowLists::ActivateTopWindowCmds();
    }
  if (wParam==SUB_UPDMAIN_UPDATE)
    {//bring specified window (if any) to top, update main window and check license...
    CWnd* pWnd = (CWnd*)lParam;
    if (pWnd)
      pWnd->BringWindowToTop();
    else
      BringWindowToTop();
    if (BringToForeground)
      SetForegroundWindow();
    UpdateWindow();
    //if (gs_pPrj)
    //  gs_pPrj->CheckLicenseConditions();
    }
  if (wParam==SUB_UPDMAIN_UPDATE || wParam==1 || wParam==SUB_UPDMAIN_BACKGROUND)
    {//repaint main MDI frame background...
    m_MDIClientWnd.Invalidate();
    }
  if (wParam==SUB_UPDMAIN_PRJLOADED && ScdPFUser.RdInt("General", "CascadeOnLoad", 0)!=0)
    PostMessage(WM_COMMAND, ID_WINDOW_CASCADE, 0);
  if (wParam==SUB_UPDMAIN_PRJLOADED && gs_pPrj)
    {
    //dbgpln("TODO - Fix Modes");

    //if (gs_Exec.GlblDynFullMode() && !gs_License.AllowDynamicFull())
    //  {
    //  LogError("License", 0, "Dynamic(Full) mode not enabled by licensing, changing to Dynamic(Flow) mode!");
    //  gs_Exec.SetGlblRunModes(SM_Inline, SM_All);
    //  }
    }
  //if (wParam==SUB_UPDMAIN_PRJLOADED)
  //  gs_LicenseCnt.CountUnits(); //ensure count of
  if (wParam==SUB_UPDMAIN_PRJLOADED)
    {
    CWindowLists::DetermineActiveGraphics();
    CWindowLists::SetGrfTagGroups(true);
    }
  if (wParam==SUB_UPDMAIN_PRJLOADED)
    {
    //let script cmd mngr know cmd is complete...
    ScdMainWnd()->PostMessage(WMU_CMDDONE, ComCmd_LoadProject, 0); //NBNB: this must be the last project loaded action
    }

  #if (!CK_LICENSINGON)
  static int BadCount = 0;
  if (wParam==SUB_UPDMAIN_UPDATE)
    {
    SYSTEMTIME ST, ET;
    GetSystemTime(&ST);
    FILETIME FT;
    if (FnCreateTime(ExeFile(), FT))
      {
      if (FileTimeToSystemTime(&FT, &ET))
        {
        long a = ST.wYear*365+ST.wMonth*30+ST.wDay;
        long b = ET.wYear*365+ET.wMonth*30+ET.wDay;
        if (a-b>120 || a-b<-30 || (a-b>60 && rand()/(double)RAND_MAX*100.0<(a-b)-55.0))
          {
          BadCount++;
          if (gs_pPrj && gs_pPrj->pPrjDoc)
            {
            CProfINIFile PF(PrjIniFile());
            long Bad = PF.RdInt("General", "Count", 0);
            PF.WrLong("General", "Count", ++Bad);
            }
          if (BadCount<3)
            {
            LogError("SysCAD", LF_Exclamation, "Licensing error. Contact suppliers!");
            PostMessage(WM_COMMAND, ID_APP_EXIT, 0);
            }
          }
        }
      }
    }
  if (BadCount>0)
    {
    Sleep(BadCount*500);
    if (BadCount>2)
      _asm int 3;
    PostMessage(WM_COMMAND, ID_APP_EXIT, 0);
    }
  #endif
  if (wParam==SUB_UPDMAIN_STARTED)
    {
    //LogNote("SysCAD", 0, "Started as Embedded, start OPC Server!");
    //if (1 || gs_License.AllowOPCServer())
    //  {
    //  if (COPCSrvrHelper::pMainOPCSrvr==NULL)
    //    COPCSrvrHelper::pMainOPCSrvr = new COPCSrvrHelper;
    //  if (COPCSrvrHelper::pMainOPCSrvr->OpenSrvr(ExeFile(), 0, 0))//bOPCResetReg, iOPCServerNo))
    //    {//open is OK
    //    //bOPCResetReg=0;
    //    }
    //  }
    //else
    //  {
    //  LogError("OPC Server", 0, "Not enable by current licensing options");
    //  }
    }

  return True;
  }

//---------------------------------------------------------------------------

LRESULT CMainFrame::OnCmd(WPARAM wParam, LPARAM lParam)
  {
  switch (wParam)
    {
    case SUB_CMD_RUNSCRIPT:
      {
      char* pTxt = NULL;
      if (lParam)
        pTxt = (char*)lParam;
      if (pTxt && strlen(pTxt)>0 && FileExists(pTxt) && gs_pXCmd)
        {
        gs_pXCmd->AddCmds(pTxt);
        MainWnd()->PostMessage(WMU_CMDNEXT, ComCmd_ExecNextCmd, 0);
        }
      if (pTxt)
        delete []pTxt;
      break;
      }

    case SUB_CMD_GENERATEOLEREPORT:
      {
      CXM_OleExcelReport* pXB = (CXM_OleExcelReport*)lParam;
      ASSERT(pXB && pXB->Id==XM_OleExcelReport);

      gs_AccessWnds.PostMessage(-1, WMU_ACCREFRESHSAVE);

      CXM_Route Route;
      if (gs_pPrj->XFindObject(pExecName_OLE, Route))
        {
        CXMsgLst XM;
        CXM_Route Rt(Route);
        XM.SetOnHeap(false);
        XM.PackMsg(pXB);
        XM.PackMsg(&Rt);
        gs_pPrj->XSendMessage(XM, Route);

        }
      else
        ASSERT_ALWAYS(FALSE, "OLE Exec Obj does not exist!", __FILE__, __LINE__);
      delete pXB;
      break;
      }

    case SUB_CMD_CMDLINE:
      {//command line parameters from another instance of SysCAD...
      CString CmdLine;
      CmdLine = ScdPFUser.RdStr("General", "CmdLine");
      CCmdLineHelper CLH;
      Strng Tmp;
      if (CmdLine.GetLength()>8 && strncmp((const char*)CmdLine, "++Quote++", 9)==0)
        {
        Tmp = "\"";
        Tmp += CmdLine.Mid(9, 4096);
        }
      else
        Tmp = (const char*)CmdLine;
      if (Tmp.Len()>8 && strncmp(&Tmp[Tmp.Len()-9], "++Quote++", 9)==0)
        {
        Tmp = Tmp.Left(Tmp.Len()-9);
        Tmp += '"';
        }
      CmdLine = Tmp();
      CLH.Parse((char*)(const char*)CmdLine);
      LogNote("SysCAD", 0, "Received command line:%s", CLH.sCmdLine());
      dbgpln("*** Process command line from message:'%s' ***", CLH.sCmdLine());
      //look for script files, ignore the rest
      if (CLH.ScriptFiles.GetSize()>0)
        {
        for (int i=0; i<CLH.ScriptFiles.GetSize(); i++)
          gs_pXCmd->AddCmds((char*)(const char*)(CLH.ScriptFiles[i]));
        MainWnd()->PostMessage(WMU_CMDNEXT, ComCmd_ExecNextCmd, 0);
        }
      if (CLH.sScriptCmd.Length()>0)
        {
        gs_pXCmd->AddCmd(CLH.sScriptCmd());
        MainWnd()->PostMessage(WMU_CMDNEXT, ComCmd_ExecNextCmd, 0);
        }
      break;
      }

    case SUB_CMD_ADDTOARCHIVE:
      {
      ASSERT(lParam);
      TagInfoBlk * IB = (TagInfoBlk*)lParam;
      gs_pArcMan->AddFieldDlg(*IB);
      delete IB;
      break;
      }

    case SUB_CMD_ARCHIVEEXECUTE:
      {
      gs_pArcMan->Execute(gs_TimeCB);
      //gs_pArcMan->SetEvent();
      break;
      }

    case SUB_CMD_ARCHIVESTEPDONE:
      {
      gs_pArcMan->StepDone(gs_TimeCB);
      gs_pArcMan->SetEvent();
      break;
      }

    case SUB_CMD_ARCHIVESTART:
      {
      gs_pArcMan->Start(gs_TimeCB);
      gs_pArcMan->SetEvent();
      break;
      }

    case SUB_CMD_ARCHIVESTOP:
      {
      gs_pArcMan->Stop();
      gs_pArcMan->SetEvent();
      break;
      }

    case SUB_CMD_OPCSRVRSTATS:
      {
      OnOptionsOPCServer();
      break;
      }

    case SUB_CMD_PRJWNDUPDATE:
      {//get project window to refresh trees...
      if (EnablePrjOK())
        gs_pPrj->pPrjDoc->UpdateAllViews(NULL, 2);
      break;
      }

    case SUB_CMD_HISTORYON:
    case SUB_CMD_HISTORYOFF:
      {
      ASSERT(lParam);
      TagInfoBlk * IB = (TagInfoBlk*)lParam;
      Strng T(IB->ObjTag());
      T+=".";
      T+=IB->RefTag();
      RecordTag(T(), wParam==SUB_CMD_HISTORYON);
      delete IB;
      break;
      }
    case SUB_CMD_ALLHISTORYON:
    case SUB_CMD_ALLHISTORYOFF:
      {
      ASSERT(lParam);
      TagInfoBlk * IB = (TagInfoBlk*)lParam;
      CTagTrees TagTrees;
      TagTrees.Rebuild(RQ_Tags);
      CModelTypeListArray& List = TagTrees.GetList();
      for (int i=0; i<List.GetSize(); i++)
        {
        if (List[i]->GetSize()>0)
          {
          Strng S=List[i]->ModelName();
          for (int j=0; j<List[i]->GetSize(); j++)
            {
            Strng T=(*List[i])[j];
            T+=".";
            T+=IB->RefTag();
            RecordTag(T(), wParam==SUB_CMD_ALLHISTORYON);
            int xxx=0;
            }
          }
        }
      delete IB;
      break;
      }
    case SUB_CMD_ALLMDLHISTORYON:
    case SUB_CMD_ALLMDLHISTORYOFF:
      {
      ASSERT(lParam);
      TagInfoBlk * IB = (TagInfoBlk*)lParam;
      Strng ClassNV(GetClassIdNoVersion(IB->ObjClassId()));
      CTagTrees TagTrees;
      TagTrees.Rebuild(RQ_Tags);
      CModelTypeListArray& List = TagTrees.GetList();
      for (int i=0; i<List.GetSize(); i++)
        {
        if (List[i]->GetSize()>0)
          {
          Strng S=List[i]->ModelName();
          Strng SNV(GetClassIdNoVersion(List[i]->ModelName()));
          if (SNV.XStrICmp(ClassNV)==0)
            {
            for (int j=0; j<List[i]->GetSize(); j++)
              {
              Strng T=(*List[i])[j];
              T+=".";
              T+=IB->RefTag();
              RecordTag(T(), wParam==SUB_CMD_ALLMDLHISTORYON);
              int xxx=0;
              }
            }
          }
        }
      delete IB;
      break;
      }
    default:
      ASSERT_ALWAYS(FALSE, "Unexpected SUB_CMD_xxx for WMU_CMD", __FILE__, __LINE__);
      break;
    }
  return true;
  }

//---------------------------------------------------------------------------

flag CMainFrame::RecordTag(LPCTSTR Tag, flag RecordingOn)
  {
  //Note:for driver tags, the recording option must be set in the driver slot configuration file
  if (strlen(Tag)>0 && Tag[0]!='\'' && Tag[0]!='=')
    {
    CXM_Route HRoute;
    flag HistOn = (XFindObject(pExecName_Historian, HRoute));
    if (HistOn)
      {
      Strng WrkTag, WrkCnvTxt;
      TaggedObject::SplitTagCnv((LPTSTR)Tag, WrkTag, WrkCnvTxt);
      CXM_HistoryExists *xb=new CXM_HistoryExists (0, WrkTag());
      CXMsgLst XM;
      XM.PackMsg(xb);
      DWORD RetCode = XSendMessage(XM, HRoute);
      bool bRecorded = ((RetCode & RC_SM_HistRecOn)!=0);
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
          return True; //change made
          }
        return False;
        }

      if (RecordingOn)
        {
        CXM_ObjectTag  ObjTag(WrkTag(), TABOpt_AllInfoOnce);//TABOpt_Parms);
        CXM_ObjectData ObjData;
        CXM_Route      Route;
        if (XReadTaggedItem(ObjTag, ObjData, Route))
          {
          CPkDataItem * pItem = ObjData.FirstItem();
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
            DWORD RetCode = XSendMessage(XM, HRoute);
            if (RetCode)
              {
              bRecorded = ((RetCode & RC_SM_HistRecOn)!=0);
              return True; //change made
              }
            }
          }
        }
      }
    }
  return False;
  }

//---------------------------------------------------------------------------

void CMainFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
  {
  if (gs_pPrj && gs_pPrj->bDoingLoad)
    return;

  CMDIFrameWnd::OnActivate(nState, pWndOther, bMinimized);
  }

//---------------------------------------------------------------------------

void CMainFrame::OnActivateApp(BOOL bActive, DOTNETHTASK hTask)
  {
  if (gs_pPrj && gs_pPrj->bDoingLoad)
    return;

  CMDIFrameWnd::OnActivateApp(bActive, (DWORD)hTask);

  if (gs_pPrj && gs_pPrj->bDoingLoad)
    return;

  //if (pExec)
  //  {
  if (bActive)
    CDbgMngr::LoadAll();

  gs_Exec.OnAppActivate(bActive);
  //  }
  }

//---------------------------------------------------------------------------

void CMainFrame::OnEXECDumpMsgQueue()
  {
  gs_Exec.DumpMsgHistory();
  }

//---------------------------------------------------------------------------

bool CMainFrame::DeleteMenuItem(UINT ItemID)
  {
  bool DoneOne=false;
  CMenu* pMenu = GetMenu();
  if (pMenu)
    {
LookforNext:
    int TopLevel = 0;
    while (TopLevel<(int)(pMenu->GetMenuItemCount()))
      {
      if (pMenu->DeleteMenu(ItemID, MF_BYCOMMAND))
        {
        DoneOne=true;
        goto LookforNext; 
        }
      if (pMenu->GetMenuItemID(TopLevel)==-1)
        {
        CMenu* pSub = pMenu->GetSubMenu(TopLevel);
        ASSERT(pSub);
        if (pSub->DeleteMenu(ItemID, MF_BYCOMMAND))
          {
          DoneOne=true;
          goto LookforNext; 
          }
        }
      TopLevel++;
      }
    }
  return DoneOne;
  }

//---------------------------------------------------------------------------

void CMainFrame::OnInitMenu(CMenu* pMenu)
  {
  CMDIFrameWnd::OnInitMenu(pMenu);
  if (gs_pPrj && !gs_pPrj->bChangedGrfMenu)
    {
#if WithHelpFileList
    ASSERT(gs_pPrj->m_MdlHelpFileList.GetSize()<IDM_MDLHELPLAST-IDM_MDLHELPFIRST);
#endif
    pMenu = GetMenu();
    if (pMenu)
      {
      int k = 1;
      while (pMenu->DeleteMenu(IDM_MDLHELPFIRST+k, MF_BYCOMMAND))
        k++;
#if WithHelpFileList
      int TopLevel = 0;
      flag Found = False;
      while (!Found && TopLevel<(int)(pMenu->GetMenuItemCount()))
        {
        if (pMenu->GetMenuItemID(TopLevel)==-1)
          {
          int j = 0;
          CMenu* pSub = pMenu->GetSubMenu(TopLevel);
          ASSERT(pSub);
          while (!Found && j<(int)pSub->GetMenuItemCount())
            {
            if (pSub->GetMenuItemID(j)==ID_HELP_UserDocs)
              {
              gs_pPrj->bChangedGrfMenu = 1;
              Found = True;
              if (gs_pPrj->m_MdlHelpFileList.GetSize()<2)
                {
                pSub->ModifyMenu(j, MF_BYPOSITION|MF_STRING, ID_HELP_UserDocs, "&Models");
                }
              else
                {
                CString MdlHlp;
                MdlHlp = gs_pPrj->m_MdlHelpFileList[0].Left(gs_pPrj->m_MdlHelpFileList[0].GetLength()-4);
                char Buff[256];
                sprintf(Buff, "&Models  (%s)", (const char*)MdlHlp);
                pSub->ModifyMenu(j, MF_BYPOSITION|MF_STRING, ID_HELP_UserDocs, Buff);
                for (k=1; k<gs_pPrj->m_MdlHelpFileList.GetSize(); k++)
                  {
                  MdlHlp = gs_pPrj->m_MdlHelpFileList[k].Left(gs_pPrj->m_MdlHelpFileList[k].GetLength()-4);
                  sprintf(Buff, "Models  (%s)", (const char*)MdlHlp);
                  pSub->InsertMenu(j+k, MF_BYPOSITION|MF_STRING, IDM_MDLHELPFIRST+k, Buff);
                  }
                }
              }
            j++;
            }
          }
        TopLevel++;
        }
      DrawMenuBar();
#endif
      }
    }

  pMenu = GetMenu();
  #if (!CK_LICENSINGON)
  //remove all menu items related to licencing...
  if (pMenu && pMenu->DeleteMenu(ID_CK_LIC_RESETSERVICE, MF_BYCOMMAND))
    {
    /*for (UINT ID = ID_CK_LIC_RESETSERVICE+1; ID<=ID_CK_LIC_INFO; ID++)
      pMenu->DeleteMenu(ID, MF_BYCOMMAND); this does not remove separators !!! */
    int TopLevel = 0;
    flag Found = False;
    while (!Found && TopLevel<(int)(pMenu->GetMenuItemCount()))
      {
      if (pMenu->GetMenuItemID(TopLevel)==-1)
        {
        int j = 0;
        CMenu* pSub = pMenu->GetSubMenu(TopLevel);
        ASSERT(pSub);
        while (!Found && j<(int)pSub->GetMenuItemCount())
          {
          if (pSub->GetMenuItemID(j)==-1)
            {
            CMenu* pSubSub = pSub->GetSubMenu(j);
            ASSERT(pSubSub);
            if (pSubSub->GetMenuItemID(0)==ID_CK_LIC_AUTH)
              {
              pSub->DeleteMenu(j, MF_BYPOSITION);
              Found = True;
              }
            }
          j++;
          }
        }
      TopLevel++;
      }
    DrawMenuBar();
    }
  #endif

  //remove selected menu items...
  if (pMenu)
    {
    //if (gs_pPrj)
      {
      //if (!gs_pPrj->bChangedRuntimeMenu)
      CMenu* pSub = (pMenu->GetMenuItemID(0)==-1) ? pMenu->GetSubMenu(0) : NULL;
      if (pSub && pSub->GetMenuItemID(0)==ID_MENU_CLEANUP_REQD)
        {
        /*if (gs_License.IsRunTime())
        {
        const UINT DelList[] = {
        ID_PROJECT_EDIT_CFG,
        ID_GRF_Insert,
        ID_GRF_ChangeUnit,
        ID_GRF_Connect,
        ID_GRF_ConstructUnit,
        ID_GRF_ConstructLink,
        ID_GRF_ConstructTie,
        ID_GRF_ConstructSymbol,
        ID_GRF_MoveEntity,
        ID_GRF_AlterEntity,
        ID_GRF_MoveLink,
        ID_GRF_ExplodeUnit,
        ID_GRF_DeleteUnit,
        ID_GRF_CreateFill,
        ID_GRF_CreateDynamic,
        ID_GRF_CreateAnnotation,
        ID_GRF_CreateToggle,
        ID_GRF_UpdateFill,
        ID_GRF_UpdateDynamic,
        ID_GRF_UpdateAnnotation,
        ID_GRF_UpdateToggle,
        ID_GRF_LoadDrawing,
        ID_GRF_ImportPDSFile,
        ID_GRF_ConfigLayers,
        ID_GRF_DeleteSymbol,
        ID_GRF_ExplodeSymbol,
        ID_GRF_SaveSymbols,
        ID_GRF_CreateGroup,
        ID_GRF_InsertGroup,
        0 };
        for (int i=0; DelList[i]!=0; i++)
        DeleteMenuItem(DelList[i]);
        }*/
        if (!ScdApp()->m_CLH.bAllowLocalCopy)
          {
          //while (DeleteMenuItem(ID_PROJECT_OPENLOCAL)) { /* repeat as item appears many times in different menus */}
          DeleteMenuItem(ID_PROJECT_OPENLOCAL);
          }

#if !KeepOldTearObject
        DeleteMenuItem(ID_VIEW_TEARBLOCKS);
#endif

#if (!WITHGRFGROUP)
        DeleteMenuItem(ID_GRF_CreateGroup);
        DeleteMenuItem(ID_GRF_InsertGroup);
#endif
#if (!WITHCTRLOBJ)
        DeleteMenuItem(ID_VIEW_CONTROLS);
#endif
#if (!WITHORDEROBJ)
        DeleteMenuItem(ID_VIEW_EVALORDER);
#endif
#if (!WITHIOOBJ)
        DeleteMenuItem(ID_VIEW_IOPOINTS);
#endif
#if (!WITHSCRCYCLES)
        DeleteMenuItem(ID_VIEW_SCREENSAVER);
#endif
#if (!WITHIMPORTPDS)
        DeleteMenuItem(ID_GRF_ImportPDSFile);
#endif
#if (!WITHANALYSE)
        DeleteMenuItem(ID_ACTIONS_Analyse);
#endif
#if (!SYSCAD10)
        DeleteMenuItem(ID_FILE_Save);
        DeleteMenuItem(ID_FILE_Load);
        DeleteMenuItem(ID_SCD10_EXPORT);
#endif
        //DeleteMenuItem(ID_GRF_ZoomIso);
        DeleteMenuItem(ID_MENU_CLEANUP_REQD);
        //gs_pPrj->bChangedRuntimeMenu=1;
        DrawMenuBar();
        }
       
      }
    }

  if (pMenu)
    {
    CMenu FitMenu;
    FitMenu.CreatePopupMenu();
    CTagVwDoc::CreateFitMenu(FitMenu, true, true);

    int TopLevel = 0;
    flag Found = False;
    while (!Found && TopLevel<(int)(pMenu->GetMenuItemCount()))
      {
      if (pMenu->GetMenuItemID(TopLevel)==-1)
        {
        CMenu* pSub = pMenu->GetSubMenu(TopLevel);
        ASSERT(pSub);
        if (pSub->ModifyMenu(ID_TREND_FITALL, MF_BYCOMMAND|MF_POPUP, (UINT)FitMenu.m_hMenu, "&Fit All"))
          {
          FitMenu.Detach();
          Found = True;
          }
        }
      TopLevel++;
      }
    DrawMenuBar();
    }

  if (1)
    {
    //MENUITEMINFO MInfo;
    //MInfo.cbSize=sizeof(MInfo);
//  //  MInfo.fMask=MIIM_CHECKMARKS|MIIM_DATA|MIIM_ID|MIIM_STATE|MIIM_SUBMENU|MIIM_TYPE;
    //MInfo.fMask=MIIM_ID|MIIM_STATE|MIIM_TYPE;
    //pMenu->GetMenuItemInfo(IDM_ERROR, &MInfo);
    //pMenu->Get
    //if (MInfo.hbmpChecked==NULL)
    //  {
    //  //ErrorBitmap
    //  //pMenu->SGetMenuItemInfo(IDM_ERROR, &MInfo);
    //
    //pMenu->ModifyMenu(IDM_ERROR, MF_BYCOMMAND|MF_OWNERDRAW, IDM_ERROR, &m_ErrorBitmap);
    //pMenu->SetMenuItemBitmaps(IDM_ERROR, MF_BYCOMMAND, &m_ErrorBitmap, &m_ErrorBWBitmap);
    //pMenu->CheckMenuItem(IDM_ERROR, 1);//MF_BYCOMMAND, &m_ErrorBitmap, &m_ErrorBWBitmap);
    //pMenu->Set->CheckMenuItem(IDM_ERROR, 1);//MF_BYCOMMAND, &m_ErrorBitmap, &m_ErrorBWBitmap);
    //pMenu->DrawItem(->SetChecked(MenuItemBitmaps(IDM_ERROR, MF_BYCOMMAND, &m_ErrorBitmap, &m_ErrorBWBitmap);
    //  }
    //int xxx=0;
    //DrawMenuBar();
    }
  }

//---------------------------------------------------------------------------

void CMainFrame::OnMdlHelpMenu(UINT nID)
  {
  if (gs_pPrj)
    gs_pPrj->ModelHelp(NULL, nID - IDM_MDLHELPFIRST);
  }

//---------------------------------------------------------------------------

void CMainFrame::OnSysCommand( UINT nID, LPARAM lParam )
  {
  #if WITHSCRCYCLES
  switch (nID)
    {
    case SC_SCREENSAVE://Activate Screen Saver...
      if (gs_pPrj && !gs_License.Blocked())
        {
        if (gs_pPrj->bSystemScreenSave)
          CMDIFrameWnd::OnSysCommand(nID, lParam);
        else
          {
          if (gs_pPrj->bSyscadScreenSave && gs_pPrj->iCycleNo<0)
            SendMessage(WMU_STARTCYCLES);
            //PostMessage(WMU_STARTCYCLES);
            //gs_pPrj->StartScreenSave();
          }
        }
      break;
    //case SC_TASKLIST:
    //  ASSERT(FALSE); //force debug break, can continue
    default:
      CMDIFrameWnd::OnSysCommand(nID, lParam);
    }
  #else
  CMDIFrameWnd::OnSysCommand(nID, lParam);
  #endif
  }

//---------------------------------------------------------------------------
#if WITHSCRCYCLES
void CMainFrame::OnTimer(UINT nIDEvent)
  {
  switch (nIDEvent)
    {
    case ID_SCREENCYCLE_TIMER:
      gs_pPrj->RotateScreenSave();
      break;
    default:
      CMDIFrameWnd::OnTimer(nIDEvent);
    }
  }

//---------------------------------------------------------------------------

LRESULT CMainFrame::OnStartCycles(WPARAM wParam, LPARAM lParam)
  {
  gs_pPrj->StartScreenSave();
  return True;
  }

//---------------------------------------------------------------------------

LRESULT CMainFrame::OnEndCycles(WPARAM wParam, LPARAM lParam)
  {
  gs_pPrj->EndScreenSave();
  return True;
  }

//---------------------------------------------------------------------------

void CMainFrame::OnViewScreensaver()
  {
  if (gs_pPrj && !gs_pPrj->bSystemScreenSave && gs_pPrj->bSyscadScreenSave && !gs_License.Blocked())
    OnSysCommand(SC_SCREENSAVE, 0);
  }

void CMainFrame::OnUpdateViewScreensaver(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(gs_pPrj && !gs_pPrj->bSystemScreenSave && gs_pPrj->bSyscadScreenSave && EnableNotStopped() && !gs_License.Blocked());
  }
#endif //end WITHSCRCYCLES
//---------------------------------------------------------------------------

LRESULT CMainFrame::OnHstUpdateStats(WPARAM wParam, LPARAM lParam)
  {
  if (m_pGenSheet && m_pGenSheet->IsStatsPage())
    {//if stats dialog is up, change it
    m_pGenSheet->pStatsPage->PostMessage(WMU_HSTUPDATESTATS, wParam, lParam);
    }
  return True;
  }

//---------------------------------------------------------------------------

void CMainFrame::OnProjectOptions()
  {//General options...
  if (m_pGenSheet)
    m_pGenSheet->BringWindowToTop();
  else
    {
    int PrevPageNo = 0;
    if (gs_pPrj && gs_pPrj->pPrjDoc)
      {
      CProfINIFile PF(PrjIniFile());
      PrevPageNo = PF.RdInt("General", "GenOptionsPageNo", 0);
      }
    m_pGenSheet = new CGenSheet("General", this, PrevPageNo);
    }
  }

void CMainFrame::OnUpdateProjectOptions(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(EnableNotStopped());
  }

//---------------------------------------------------------------------------

void CMainFrame::OnProjectSettings()
  {
  if (gs_pPrj && !gs_License.Blocked())
    gs_pPrj->OnChangeCurrentSettings("Project Settings");
  }

void CMainFrame::OnUpdateProjectSettings(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(EnableNotBusy() && EnableNotAnalysing() && EnableNotStopped() && !gs_License.Blocked());
  }

//---------------------------------------------------------------------------

void CMainFrame::OnProjectEditCfg()
  {
  if (!EnablePrjOK() && gs_License.AllowFullLic())
    {
    CWaitCursor Wait;
    CModelInfo MI;
    MI.EditCfgFile(NULL, gs_License.LicCatagories());
    }
  }

void CMainFrame::OnUpdateProjectEditCfg(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(!EnablePrjOK() && gs_License.AllowFullLic());
  }

//---------------------------------------------------------------------------

void CMainFrame::OnProjectEditSdb()
  {
  Strng s, BrowseFolder;
  CSCDFileDialog Dlg(true, NULL, CfgDBFileName(), OFN_CREATEPROMPT|OFN_HIDEREADONLY/*OFN_FILEMUSTEXIST*/,
                     "SysCAD Databases (*.mdb)|*.mdb||");
  if (CfgFiles())
    Dlg.m_ofn.lpstrInitialDir = CfgFiles();
  else
    {
    BrowseFolder = BaseCfgFiles();
    if (BrowseFolder.FindI(BCfgFolderName())>=0)
      BrowseFolder = BrowseFolder.Left(BrowseFolder.FindI(BCfgFolderName()));
    BrowseFolder = ScdPFUser.RdStr("ProjectEdit", "EditSdbFolder", BrowseFolder());
    Dlg.m_ofn.lpstrInitialDir = BrowseFolder();
    }
  Dlg.m_ofn.lpstrTitle = "Open SysCAD Database";

  if (Dlg.DoModal()==IDOK)
    {
    ScdPFUser.WrStr("ProjectEdit", "EditSdbFolder", Dlg.GetPathName());
    CSCDDatabase::DoSDBEdit(Dlg.GetPathName());
    }
  }

void CMainFrame::OnUpdateProjectEditSdb(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(/*CfgFiles() && */(SCDFileFormat()>0) && !gs_License.Blocked());
  }

//---------------------------------------------------------------------------

void CMainFrame::OnProjectEditDefSdb()
  {
  Strng s;
  CSCDFileDialog Dlg(true, NULL, BCfgDBFileName(), OFN_CREATEPROMPT|OFN_HIDEREADONLY/*OFN_FILEMUSTEXIST*/,
                     "SysCAD Databases (*.mdb)|*.mdb||");
  Strng BrowseFolder = ScdPFUser.RdStr("ProjectEdit", "EditDefSdbFolder", BaseCfgFiles());
  Dlg.m_ofn.lpstrInitialDir = BrowseFolder();
  Dlg.m_ofn.lpstrTitle = "Open SysCAD Database";

  if (Dlg.DoModal()==IDOK)
    {
    ScdPFUser.WrStr("ProjectEdit", "EditDefSdbFolder", Dlg.GetPathName());
    CSCDDatabase::DoSDBEdit(Dlg.GetPathName());
    }
  }

void CMainFrame::OnUpdateProjectEditDefSdb(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable((SCDFileFormat()>0) && !gs_License.Blocked());
  }

//---------------------------------------------------------------------------

void CMainFrame::OnProjectLoadaltcfg()
  {
  if (gs_pPrj && !gs_License.Blocked())
    {
    CProject::sm_fOpenAltCfg=true;
    ScdApp()->DoOnFileOpen();
    CProject::sm_fOpenAltCfg=false;
    }
//    gs_pPrj->OnLoadAltCfg();
  }

void CMainFrame::OnUpdateProjectLoadaltcfg(CCmdUI* pCmdUI)
  {
//  pCmdUI->Enable(!gs_License.Blocked());
  pCmdUI->Enable((!EnablePrjOK() || EnableNotBusy() && EnableNotAnalysing() && EnableNotStopped()) && !gs_License.Blocked());
  }

//---------------------------------------------------------------------------

void CMainFrame::OnProjectSaveas()
  {
  if (gs_pPrj)
//    gs_pPrj->pPrjDoc->DoSave(NULL);
    {
    gs_pPrj->bDoingSaveAs=true;
    gs_pPrj->pPrjDoc->OnSaveDocument("XXX"); // will prompt for name
//    gs_pPrj->OnProjectSaveas(NULL, NULL);
    gs_pPrj->bDoingSaveAs=false;
    }
  }

void CMainFrame::OnUpdateProjectSaveas(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(EnableNotBusy() && EnableNotAnalysing() && EnableNotStopped());
  }

//---------------------------------------------------------------------------

void CMainFrame::OnProjectSaveasNV()
  {
  if (gs_pPrj)
//    gs_pPrj->pPrjDoc->DoSave(NULL);
    {
    gs_pPrj->bDoingSaveAsNV=true;
    gs_pPrj->bDoingSaveAs=true;
    gs_pPrj->pPrjDoc->OnSaveDocument("XXX"); // will prompt for name
//    gs_pPrj->OnProjectSaveas(NULL, NULL);
    gs_pPrj->bDoingSaveAs=false;
    gs_pPrj->bDoingSaveAsNV=false;
    }
  }

void CMainFrame::OnUpdateProjectSaveasNV(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(EnableNotBusy() && EnableNotAnalysing() && EnableNotStopped());
  }

//---------------------------------------------------------------------------

void CMainFrame::OnProjectZeroFlows()
  {
  CWaitMsgCursor Wait("Zeroing Flowsheet Flows");
  Strng_List SL;
  gs_Exec.SetModelState(MSA_ZeroFlows, SL);
  }

void CMainFrame::OnUpdateProjectZeroFlows(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(!gs_License.Blocked() && EnableNotBusy() && EnableNotAnalysing() && EnableNotStopped());
  }

//---------------------------------------------------------------------------

void CMainFrame::OnProjectEmpty()
  {
  CWaitMsgCursor Wait("Emptying Flowsheet");

  Strng_List SL;
  gs_Exec.SetModelState(MSA_Empty, SL);
  }

void CMainFrame::OnUpdateProjectEmpty(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(!gs_License.Blocked() /*&& EnableNotBusy()*/ && EnableNotAnalysing() && EnableNotStopped());
  }

//---------------------------------------------------------------------------

void CMainFrame::OnProjectPreset()
  {
  CWaitMsgCursor Wait("Flowsheet Preset");
  Strng_List SL;
  gs_Exec.SetModelState(MSA_PreSet, SL);
  }

void CMainFrame::OnUpdateProjectPreset(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(!gs_License.Blocked() && EnableNotBusy() && EnableNotAnalysing() && EnableNotStopped());
  }

//---------------------------------------------------------------------------

void CMainFrame::OnProjectResetStats()
  {
  CWaitMsgCursor Wait("Reset Flowsheet Statistics");
  Strng_List SL;
  gs_Exec.SetModelState(MSA_DynStatsRunInit, SL);
  }

void CMainFrame::OnUpdateProjectResetStats(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(!gs_License.Blocked() && !DefNetProbalMode() /*&& EnableNotBusy()*/ && EnableNotAnalysing() && EnableNotStopped());
  }

//---------------------------------------------------------------------------

void CMainFrame::OnProjectSteadystate()
  {
  }

void CMainFrame::OnUpdateProjectSteadystate(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(False && EnableNotBusy() && EnableNotAnalysing() && EnableNotStopped());
  }

//---------------------------------------------------------------------------

void CMainFrame::OnProjectMerge()
  {
  if (gs_pPrj && !gs_License.Blocked())
    gs_pPrj->Merge();
  }

void CMainFrame::OnUpdateProjectMerge(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(!gs_License.Blocked() && EnableNotBusy() && EnableNotAnalysing() && EnableNotStopped());
  }

//---------------------------------------------------------------------------

void CMainFrame::OnFileImport()
  {
  if (gs_pPrj && !gs_License.Blocked())
    gs_pPrj->FileImport();
  }

void CMainFrame::OnUpdateFileImport(CCmdUI *pCmdUI)
  {
  pCmdUI->Enable(!gs_License.Blocked() && EnableNotBusy() && EnableNotAnalysing() && EnableNotStopped());
  }
//---------------------------------------------------------------------------

void CMainFrame::OnFileExport()
  {
  if (gs_pPrj && !gs_License.Blocked())
    gs_pPrj->FileExport();
  }

void CMainFrame::OnUpdateFileExport(CCmdUI *pCmdUI)
  {
  pCmdUI->Enable(!gs_License.Blocked() && EnableNotBusy() && EnableNotAnalysing() && EnableNotStopped());
  }
//---------------------------------------------------------------------------

void CMainFrame::OnDeleteDebugFile()
  {
  dbgfiletrunc();
  }

//---------------------------------------------------------------------------

void CMainFrame::OnProjectCompareData()
  {
  CPrjDataCompare Dlg;
  if (Dlg.DoModal()==IDOK)
    {
    Dlg.DoCompare();

    int xxx=0;
    }
  }

void CMainFrame::OnUpdateProjectCompareData(CCmdUI* pCmdUI)
  {
  //pCmdUI->Enable(!gs_License.Blocked() && !EnablePrjOK());
  pCmdUI->Enable(!gs_License.Blocked() && (!EnablePrjOK() ||(EnableNotBusy() && EnableNotAnalysing() && EnableNotStopped())));
  }

//---------------------------------------------------------------------------

void CMainFrame::OnOptionsBrowsetags()
  {
  if (gs_pPrj)
    gs_pPrj->BrowseTags();
  }

void CMainFrame::OnUpdateOptionsBrowsetags(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(EnableHistorianOK() && EnableNotStopped() && !gs_License.Blocked());
  }
//---------------------------------------------------------------------------

void CMainFrame::OnOptionsHistorianQuery()
  {
  if (gs_pPrj)
    gs_pPrj->HistorianQuery();
  }

void CMainFrame::OnUpdateOptionsHistorianQuery(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(EnableHistorianOK() && EnableNotStopped() && !gs_License.Blocked());
  }

//---------------------------------------------------------------------------
#if WITHDRVMAN
void CMainFrame::OnOptionsDriver()
  {
  if (gs_pDrvMan && gs_License.AllowDrivers())
    gs_pDrvMan->Options();
  }

void CMainFrame::OnUpdateOptionsDriver(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(gs_License.AllowDrivers() && EnableDriverOK() && EnableNotStopped() && !gs_License.Blocked());
  }
//---------------------------------------------------------------------------

void CMainFrame::OnReloadDriver()
  {
  if (XBusy())
    AfxMessageBox("Must be Stopped to Reload Drivers", MB_OK);
  else
    {
    flag OK=gs_pPrj->ReloadDrvManager(FALSE);
    }
  }

void CMainFrame::OnUpdateReloadDriver(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(EnableNotBusy() && EnableDriverOK() && !gs_License.Blocked());
  }
#endif

//---------------------------------------------------------------------------

void CMainFrame::OnOptionsArchive()
  {
  if (gs_pArcMan)// && gs_License.AllowDrivers())
    gs_pArcMan->Options();
  }

void CMainFrame::OnUpdateOptionsArchive(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(/*gs_License.AllowDrivers() &&*/ EnableArchiverOK() && EnableNotStopped() && !gs_License.Blocked());
  }

//---------------------------------------------------------------------------

void CMainFrame::OnOptionsDebugVisible()
  {
  bDebugOptionsVisible = !bDebugOptionsVisible;
  LogNote("Debug", 0, "Debug options page will %s be visible", bDebugOptionsVisible ? "now" : "NOT");
  }

//---------------------------------------------------------------------------

void CMainFrame::OnFileSavesnapshot()
  {
  if (gs_pPrj && !gs_License.Blocked())
    gs_pPrj->OnFileSavesnapshot();
  }

void CMainFrame::OnUpdateFileSavesnapshot(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(EnablePrjOK() && EnableNotStopped() && !gs_License.Blocked());
  }

//---------------------------------------------------------------------------

void CMainFrame::OnFileLoadsnapshot()
  {
  if (gs_pPrj && !gs_License.Blocked())
    gs_pPrj->OnFileLoadsnapshot();
  }

void CMainFrame::OnUpdateFileLoadsnapshot(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(EnablePrjOK() && EnableNotStopped() && !gs_License.Blocked());
  }

//---------------------------------------------------------------------------

void CMainFrame::OnFileSavescenario()
  {
  if (gs_pPrj && !gs_License.Blocked())
    gs_pPrj->OnFileSavescenario();
  }

void CMainFrame::OnUpdateFileSavescenario(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(EnablePrjOK() && EnableNotStopped() && !gs_License.Blocked());
  }

//---------------------------------------------------------------------------

void CMainFrame::OnFileLoadscenario()
  {
  if (gs_pPrj && !gs_License.Blocked())
    gs_pPrj->OnFileLoadscenario();
  }

void CMainFrame::OnUpdateFileLoadscenario(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(EnablePrjOK() && EnableNotStopped() && !gs_License.Blocked());
  }

//---------------------------------------------------------------------------

void CMainFrame::OnFileSavelayout()
  {
  if (gs_pPrj)
    gs_pPrj->SaveWindowLayout();
  }

void CMainFrame::OnUpdateFileSavelayout(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(EnablePrjOK());
  }

//---------------------------------------------------------------------------

void CMainFrame::OnChangeTag()
  {
//  if (gs_Exec.TestRunning())
//    return;
  if (!gs_License.Blocked())
    {
    CChangeTag ChgTag;
    ChgTag.DoModal();
    }
  }

void CMainFrame::OnUpdateChangeTag(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(!gs_License.Blocked() && EnablePrjOK() && EnableNotStopped());
  }

//---------------------------------------------------------------------------

void CMainFrame::OnBulkChangeTag()
  {
  if (!gs_License.Blocked())
    {
    CBulkTagChange ChgTags;
    ChgTags.DoModal();
    }
  }

void CMainFrame::OnUpdateBulkChangeTag(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(!gs_License.Blocked() && EnablePrjOK() && EnableNotStopped());
  }

//---------------------------------------------------------------------------

void CMainFrame::OnTagDataTransfer()
  {
  if (!gs_License.Blocked())
    {
    CCopyTagBlkDlg CopyBlkDlg(this, NULL, NULL);
    CopyBlkDlg.DoModal();
    }
  }

void CMainFrame::OnUpdateTagDataTransfer(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(!gs_License.Blocked() && EnablePrjOK() && EnableNotStopped());
  }

//---------------------------------------------------------------------------

LRESULT CMainFrame::OnTagAction(WPARAM wParam, LPARAM lParam)
  {
  char* pTxt = NULL;
  if (lParam)
    pTxt = (char*)lParam;
  if (gs_pPrj)
    {
    switch (wParam)
      {
      case SUB_TAG_ACCESS:
        if (pTxt)
          gs_AccessWnds.AccessNode(-1, pTxt);
        break;
      case SUB_TAG_ACCNOUPDHIST:
        if (pTxt)
          gs_AccessWnds.AccessNode(-1, pTxt, true, false);
        break;
      case SUB_TAG_CHANGE:
        if (pTxt)
          {
          CChangeTag ChgTag(pTxt, False);
          ChgTag.DoModal();
          }
        else
          {
          CChangeTag ChgTag;
          ChgTag.DoModal();
          }
        break;
      case SUB_TAG_MDLHELP:
        if (pTxt)
          {
          CXM_ObjectTag ObjTag(pTxt, TABOpt_Parms);//TABOpt_AllInfoOnce);
          CXM_ObjectData ObjData;
          CXM_Route Route;
          if (XReadTaggedItem(ObjTag, ObjData, Route))
            {
            CPkDataItem * pItem = ObjData.FirstItem();
            Strng Class;
            Class = pItem->Class();
            pTagObjClass pTagObj = TagObjClass::FindClassId(Class());
            if (pTagObj==NULL)
              pTagObj = TagObjClass::FindClassName(Class());
            if (pTagObj)
              Class = pTagObj->ClassIdNoVersion();//pTagObj->ShortDesc();
            gs_pPrj->ModelHelp(Class());
            }
          }
        else
          gs_pPrj->ModelHelp();
        break;
      case SUB_TAG_COPYBLK:
        {
        CCopyTagBlkDlg CopyBlkDlg(this, NULL, pTxt);
        CopyBlkDlg.DoModal();
        break;
        }
      case SUB_TAG_ACCREFRESHSAVE:
        {//refresh the access window
        for (int i=0; i<NAccessWnds; i++)
          {
          if (gs_AccessWnds.CurTag(i) && (pTxt==NULL || _stricmp(pTxt, gs_AccessWnds.CurTag(i))==0))
            gs_AccessWnds.PostMessage(i, WMU_ACCREFRESHSAVE);
          }
        break;
        }
      default:
      //case SUB_TAG_FIND_KWIK:
      //case SUB_TAG_FINDANDACCESS_KWIK:
      //case SUB_TAG_FIND:
      //case SUB_TAG_FINDANDACCESS:
      //case SUB_TAG_FIND_NOERRDLG_KWIK:
      //case SUB_TAG_FINDANDACCESS_NOERRDLG_KWIK:
      //case SUB_TAG_FIND_NOERRDLG:
      //case SUB_TAG_FINDANDACCESS_NOERRDLG:
        {
        dword Options=0;
        Options |= wParam&SUB_TAG_ACCESS      ? FTO_DoAccess:0;
        Options |= wParam&SUB_TAG_ACCNOUPDHIST? FTO_DoAccessNoUpd:0;
        Options |= wParam&SUB_TAG_MARK        ? (wParam&SUB_TAG_KWIK ? FTO_HighliteKwik:FTO_HighliteSlow):0;
        Options |= wParam&SUB_TAG_GOTO        ? FTO_MoveCursor:0;
        Options |= wParam&SUB_TAG_NOERRDLG    ? FTO_NoErrDlg:0;
        Options |= wParam&SUB_TAG_NOGRAPHICS  ? FTO_SkipNoGraphicsErr:0;

        if (pTxt)
          gs_pPrj->FindTag(pTxt, NULL, NULL, -1, Options);
        else
          gs_pPrj->FindTag("", NULL, NULL, -1, Options);
        break;
        }
        break;
      }
    }
  if (pTxt)
    delete []pTxt;
  return True;
  }

//---------------------------------------------------------------------------

void CMainFrame::OnFindTag()
  {
  //INCOMPLETECODE(__FILE__,__LINE__);
  if (gs_pPrj)
    gs_pPrj->FindTag("",NULL, NULL, -1, 0);
  }

void CMainFrame::OnUpdateFindTag(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(EnablePrjOK() && EnableNotStopped());
  }

//---------------------------------------------------------------------------

void CMainFrame::OnFindNext()
  {
  //INCOMPLETECODE(__FILE__,__LINE__);
  if (gs_pPrj)
    gs_pPrj->FindTag("", NULL, NULL, -1, FTO_FindNext);// FALSE, FALSE, TRUE);
  }

void CMainFrame::OnUpdateFindNext(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(EnablePrjOK() && EnableNotStopped());
  }

//---------------------------------------------------------------------------

void CMainFrame::OnExplorer()
  {
  CExploreScd::OpenIt();
  }

void CMainFrame::OnUpdateExplorer(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(EnableNotStopped());
  }

void CMainFrame::OnWindowSelectwindow()
  {
  CExploreScd::OpenIt();
  }

void CMainFrame::OnUpdateWindowSelectwindow(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(EnableNotStopped());
  }

//---------------------------------------------------------------------------

void CMainFrame::OnWindowArrange()
  {
  CExploreScd::ArrangeWindows();
  }

void CMainFrame::OnUpdateWindowArrange(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(EnableNotStopped());
  }

//---------------------------------------------------------------------------

void CMainFrame::OnWindowCascade()
  {
  CMDIFrameWnd::OnCommand(ID_WINDOW_CASCADE, 0);
  }
void CMainFrame::OnWindowTile()
  {
  CMDIFrameWnd::OnCommand(ID_WINDOW_TILE_HORZ, 0);
  }
void CMainFrame::OnWindowArrangeIcons()
  {
  CMDIFrameWnd::OnCommand(ID_WINDOW_ARRANGE, 0);
  }

void CMainFrame::OnUpdateWindowCmd(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(MDIGetActive() != NULL);
  }

//---------------------------------------------------------------------------

void CMainFrame::OnOptionsCmdscripts()
  {
  if (m_pCmdScriptsDlg==NULL && !gs_License.Blocked())
    m_pCmdScriptsDlg = new CScriptsDlg(this);
  }

void CMainFrame::OnUpdateOptionsCmdscripts(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(EnableNotStopped() && !gs_License.Blocked());
  }

//---------------------------------------------------------------------------

void CMainFrame::OnOptionsCompare()
  {
  if (!gs_License.Blocked())
    {
    CCompareDlg Dlg;
    Dlg.DoModal();
    }
  }

void CMainFrame::OnUpdateOptionsCompare(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(EnableNotBusy() && EnableNotAnalysing() && EnableNotStopped() && !gs_License.Blocked());
  }

//---------------------------------------------------------------------------

void CMainFrame::OnOptionsDDE()
  {
  if (EnableDDEServerOK())
    gs_pPrj->pDDESrvr->Options();
  }

void CMainFrame::OnUpdateOptionsDDE(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(EnableDDEServerOK() && EnableNotStopped() && !gs_License.Blocked());
  }

//---------------------------------------------------------------------------

void CMainFrame::OnOptionsOPCServer()
  {
  if (EnableOPCServerOK())
//    gs_pPrj->pOPCSrvr->Options();
    gs_pMainOPCSrvr->Options();
  }

void CMainFrame::OnUpdateOptionsOPCServer(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(EnableOPCServerOK() && EnableNotStopped() && !gs_License.Blocked());
  }

//---------------------------------------------------------------------------

void CMainFrame::OnOptionsReports()
  {
  gs_pPrj->CheckLicense(FALSE);
  if (!gs_License.Blocked())
    DoReports();
  }

void CMainFrame::OnUpdateOptionsReports(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(EnablePrjOK() && EnableNotStopped() && !gs_License.Blocked());
  }

//---------------------------------------------------------------------------

void CMainFrame::DoReports()
  {
  if (m_pReportsDlg==NULL && !gs_License.Blocked())
    m_pReportsDlg = new CReportsDlg(this);
  }

//---------------------------------------------------------------------------

LRESULT CMainFrame::OnGenerateReport(WPARAM wParam, LPARAM lParam)
  {
  switch (wParam)
    {
    case SUB_REPACTION_GENERATE:
      {
      CXM_RepTrendDB* pXB = (CXM_RepTrendDB*)lParam;
      CXM_Route Route;
      if (gs_pPrj->XFindObject(pExecName_Rep, Route))
        {
        CXMsgLst XM;
        XM.PackMsg(pXB);
        gs_pPrj->XSendMessage(XM, Route);
        }
      else
        ASSERT_ALWAYS(FALSE, "Rep Exec Obj does not exist!", __FILE__, __LINE__);
      delete pXB;
      break;
      }
    case SUB_REPACTION_START:
      CRepExec::Start1((void*)lParam);
      CRepExec::Start2((void*)lParam);
      break;
    case SUB_REPACTION_PROCESS:
      CRepExec::WriteData((void*)lParam);
      break;
    case SUB_REPACTION_END:
      CRepExec::End((void*)lParam);
      break;
    }
  return true;
  }

//---------------------------------------------------------------------------

LRESULT CMainFrame::OnCmdDone(WPARAM wParam, LPARAM lParam)
  {//for script commands...
  VERIFY(gs_pXCmd);
#if dbgMainframe
  if (dbgActions())
    dbgpln("<------ACT------ OnCmdDone%-20s %5i %i", ScdCOMCmd2Str(wParam), lParam, wParam==ComCmd_ExecNextCmd);
#endif
  gs_pXCmd->CmdComplete(wParam);
  CmdExec().DoCOMEvt(wParam, lParam);
  return True;
  }

//---------------------------------------------------------------------------

LRESULT CMainFrame::OnCmdNext(WPARAM wParam, LPARAM lParam)
  {//for script commands...
  VERIFY(gs_pXCmd);
#if dbgMainframe
  if (dbgActions())
    dbgpln("<------ACT------ OnCmdNext");
#endif
  gs_pXCmd->ExecCmds();
  return True;
  }

//---------------------------------------------------------------------------

LRESULT CMainFrame::OnCmdState(WPARAM wParam, LPARAM lParam)
  {//for script commands...
  VERIFY(gs_pXCmd);
#if dbgMainframe
  if (dbgActions())
    dbgpln("<------ACT------ OnCmdState %-20s %5i %i", ScdCOMCmd2Str(wParam), lParam, wParam==ComCmd_ExecNextCmd);
#endif
  gs_pXCmd->CmdComplete(wParam);
  CmdExec().DoCOMEvt(wParam, lParam);
  return True;
  }

//---------------------------------------------------------------------------

LRESULT CMainFrame::OnOLEInfoUpdate(WPARAM wParam, LPARAM lParam)
  {//message from ole automation manager...
  gs_AccessWnds.PostMessage(-1, WMU_ACCREFRESHCANCEL);
  return True;
  }

//---------------------------------------------------------------------------

LRESULT CMainFrame::OnAddToRptTagLists(WPARAM wParam, LPARAM lParam)
  {
  ASSERT(lParam);
  TagInfoBlk * IB = (TagInfoBlk*)lParam;
  if (wParam==0)
    CMdlValueShow::AddItem(*IB);
  else
    CRptTagLists::Show(IB->ObjClassId());
  delete IB;
  return True;
  }

//---------------------------------------------------------------------------

void CMainFrame::OnAnalyse()
  {
  #if (WITHANALYSE)
  if (gs_License.AllowAnalyse() && EnableNotBusy() && EnableNotStopped())
    {
//TODO Must be Expanded ...

    CAnalyseDesign *AD=new CAnalyseDesign(this);
//    AS.DoModal();
/*
    CMenu Menu;
    Menu.CreatePopupMenu();

    Menu.AppendMenu(MF_STRING, IDM_DSTIO_0+0,  "Solve for Flow");
    Menu.AppendMenu(MF_STRING, IDM_DSTIO_0+1,  "System Curve");
    Menu.AppendMenu(MF_STRING, IDM_DSTIO_0+2,  "Flash Analysis");

    int i;
    POINT ScreenPoint = {500,350};//(int)EI.Fld->Xe(ScrGB), (int)EI.Fld->Ye(ScrGB)};// = point;
    ClientToScreen(&ScreenPoint);
    //CRect ClickRect(0,0,2048,2048);
    int RetCd=Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_RETURNCMD,
      ScreenPoint.x, ScreenPoint.y, this, NULL);//&ClickRect);
    if (RetCd)
      i=RetCd-IDM_DSTIO_0;
    else
      i=0;
    Menu.DestroyMenu();

    gs_pPrj->FlwLib()->FE_DoSolveNet();
*/
    }
  #endif
  }

//---------------------------------------------------------------------------

void CMainFrame::OnUpdateAnalyse(CCmdUI* pCmdUI)
  {
  #if (WITHANALYSE)
  pCmdUI->Enable(gs_License.AllowAnalyse() && EnableNotBusy() && EnableNotStopped());
  #else
  pCmdUI->Enable(0);
  #endif
  }

//---------------------------------------------------------------------------

void CMainFrame::DockControlBarRelative(CToolBar* Bar, CToolBar* RelativeTo, flag Left)
  {//see mfc example docktool
  CRect rect;
  DWORD dw;
  UINT n;
  //get MFC to adjust the dimensions of all docked ToolBars so that GetWindowRect will be accurate
  RecalcLayout();
  RelativeTo->GetWindowRect(&rect);
  if (Left)
    rect.OffsetRect(1,0); //left of
  else
    rect.OffsetRect(0,1); //below
  dw=RelativeTo->GetBarStyle();
  n = 0;
  n = (dw&CBRS_ALIGN_TOP) ? AFX_IDW_DOCKBAR_TOP : n;
  n = (dw&CBRS_ALIGN_BOTTOM && n==0) ? AFX_IDW_DOCKBAR_BOTTOM : n;
  n = (dw&CBRS_ALIGN_LEFT && n==0) ? AFX_IDW_DOCKBAR_LEFT : n;
  n = (dw&CBRS_ALIGN_RIGHT && n==0) ? AFX_IDW_DOCKBAR_RIGHT : n;
  // When we take the default parameters on rect, DockControlBar will dock
  // each Toolbar on a seperate line.  By calculating a rectangle, we in effect
  // are simulating a Toolbar being dragged to that location and docked.
  DockControlBar(Bar,n,&rect);
  }

//---------------------------------------------------------------------------

void CMainFrame::UpdateStatusBar()
  {
  int nIndicators = 5;
  StatusBarDriverIndicator = -1;
  StatusBarCmdIndicator = -1;
  UINT nID,nStyle;
  int cxWidth;
  /*StatusBarDriverIndicator = nIndicators++;
  m_wndStatusBar.GetPaneInfo(StatusBarDriverIndicator, nID, nStyle, cxWidth);
  if (gs_pPrj->bDrvOn)
    m_wndStatusBar.SetPaneInfo(StatusBarDriverIndicator, nID, nStyle & (~SBPS_DISABLED) | SBPS_NOBORDERS, 0);
  else
    m_wndStatusBar.SetPaneInfo(StatusBarDriverIndicator, nID, nStyle | SBPS_DISABLED & (~SBPS_NOBORDERS), 3*7);
  StatusBarCmdIndicator = nIndicators++;
  m_wndStatusBar.GetPaneInfo(StatusBarCmdIndicator, nID, nStyle, cxWidth);
  if (gs_pXCmd && pCmdExec->CmdCount()>0)
    m_wndStatusBar.SetPaneInfo(StatusBarCmdIndicator, nID, nStyle & (~SBPS_DISABLED) | SBPS_NOBORDERS, 0);
  else
    m_wndStatusBar.SetPaneInfo(StatusBarCmdIndicator, nID, nStyle | SBPS_DISABLED & (~SBPS_NOBORDERS), 3*7);*/
#if WITHDRVMAN
  if (gs_pPrj->bDrvOn)
    StatusBarDriverIndicator = nIndicators++;
#endif
  if (gs_pXCmd && gs_pXCmd->CmdCount()>0)
    StatusBarCmdIndicator = nIndicators++;
  if (m_wndStatusBar.SetIndicators(indicators, nIndicators+1))
    {
    m_wndStatusBar.SetNoOfIndicators(nIndicators);
    if (StatusBarDriverIndicator>=0)
      {
      m_wndStatusBar.GetPaneInfo(StatusBarDriverIndicator, nID, nStyle, cxWidth);
      m_wndStatusBar.SetPaneInfo(StatusBarDriverIndicator, nID, nStyle, 7*3);
      }
    if (StatusBarCmdIndicator>=0)
      {
      m_wndStatusBar.GetPaneInfo(StatusBarCmdIndicator, nID, nStyle, cxWidth);
      m_wndStatusBar.SetPaneInfo(StatusBarCmdIndicator, nID, nStyle, 7*6);
      }
    }
  m_wndStatusBar.Invalidate();
  }

//---------------------------------------------------------------------------
//IDs of all toolbar buttons in a bitmap, this MUST match the resource...

static UINT BASED_CODE ToolbarIds[] =
  {
  ID_FILE_NEW,
  ID_FILE_OPEN,
  ID_FILE_SAVE,
  ID_PROJECT_SAVEALL,
  ID_PROJECT_SAVEASNV,
  ID_FIND_TAG,
  ID_PROJECT_OPTIONS,
  ID_OPTIONS_DRIVER,
  ID_RELOADDRIVER,
  ID_FILE_PRINT,
  ID_WNDW_SELECT,
  ID_OPTIONS_REPORTS,
  ID_ACTIONS_EDIT_TGL,
  ID_ACTIONS_RUN_TGL,
  ID_ACTIONS_STEP,
  ID_ACTIONS_SETUP,
  ID_ACTIONS_RUN,
  ID_ACTIONS_IDLE,
  ID_ACTIONS_EDIT,
  ID_OPTIONS_ARCHIVE,
  ID_FILE_SAVESNAPSHOT,
  ID_FILE_LOADSNAPSHOT,
  ID_ONLINE_PGMLANGUAGE,
  ID_ONLINE_HOME,
  ID_ONLINE_MODELS,
  ID_CK_LIC_AUTH,
  ID_CK_LIC_INFO,
  ID_PROJECT_EDIT_DEFSDB,
  ID_PROJECT_EDIT_SDB,
  ID_PROJECT_EDIT_CFG,
  ID_PROJECT_LOADALTCFG,
  ID_TREND_SCROLL_BACKWARDFAST,
  ID_TREND_SCROLL_BACKWARD,
  ID_TREND_SCROLL_FORWARD,
  ID_TREND_SCROLL_FORWARDFAST,
  ID_TREND_END,
  ID_TREND_ZOOM_IN,
  ID_TREND_ZOOM_OUT,
  ID_TREND_SET,
  ID_TREND_GLOBAL,
  ID_TREND_REFRESH,
  ID_GRF_Insert,
  ID_GRF_Connect,
  ID_GRF_ConstructUnit,
  ID_GRF_ConstructLink,
  ID_GRF_ConstructTie,
  ID_GRF_ConstructSymbol,
  ID_GRF_DeleteUnit,
  ID_GRF_ExplodeUnit,
  ID_GRF_Zoom,
  ID_GRF_DoZoomOnce,
  ID_GRF_DoZoomAll,
  ID_GRF_DoRedraw,
  ID_GRF_MarkEntities,
  ID_GRF_TagAnnot,
  ID_GRF_MoveEntity,
  ID_GRF_AlterEntity,
  ID_GRF_MoveLink,
  ID_GRF_ChangeUnit,
  ID_GRF_HelpUnit,
  ID_GRF_CreateFill,
  ID_GRF_CreateDynamic,
  ID_GRF_CreateAnnotation,
  ID_GRF_CreateToggle,
  ID_GRF_InsertText,
  ID_GRF_UpdateFill,
  ID_GRF_UpdateDynamic,
  ID_GRF_UpdateAnnotation,
  ID_GRF_UpdateToggle,
  ID_GRF_UpdateText,
  ID_GRF_RefreshAllAnnotation,
  ID_ACTIONS_PROBALSETUP,
  ID_ACTIONS_DYNAMICSETUP,
  ID_ACTIONS_RUNPB_TGL,
  ID_ACTIONS_RUNDYN_TGL,
  ID_ACTIONS_RUNPB,
  ID_ACTIONS_RUNDYN,
  ID_GRF_SaveSymbols,
  ID_GRF_InsertGroup,
  ID_GRF_CreateGroup,
  ID_VIEW_EXPLORER,
  ID_ACTIONS_RUN_STEADY,
  ID_PROJECT_EDIT_SETTINGS,
  ID_PROJECT_EDIT_COLOURS,
  };

static UINT BASED_CODE GrfSymbsIds[] =
  {
  ID_GRF_Esc,
  ID_GRF_Done,
  ID_GRF_ZoomLeft,
  ID_GRF_ZoomRight,
  ID_GRF_ZoomUp,
  ID_GRF_ZoomDown,
  ID_GRF_ZoomIn,
  ID_GRF_ZoomOut,
  ID_GRF_ZoomAll,
  ID_GRF_Redraw,
  ID_GRF_IWin,
  ID_GRF_CWin,
  ID_GRF_CLear,
  ID_GRF_DumpAllToFile,
  ID_GRF_ScalePlus,
  ID_GRF_ScaleMinus,
  ID_GRF_RotatePlus,
  ID_GRF_RotateMinus,
  ID_GRF_Dialog,
  ID_GRF_Back,
  ID_GRF_Ortho,
  ID_GRF_Free,
  ID_GRF_Break,
  };

//---------------------------------------------------------------------------
//default IDs of toolbar buttons...

static UINT BASED_CODE SysCADToolBarIds[] = //no project loaded
  {
  ID_FILE_NEW,
  ID_FILE_OPEN,
  ID_PROJECT_LOADALTCFG,
  ID_PROJECT_EDIT_SETTINGS,
    ID_SEPARATOR,
  ID_PROJECT_OPTIONS,
  ID_PROJECT_EDIT_CFG,
  ID_PROJECT_EDIT_SDB,
  ID_PROJECT_EDIT_DEFSDB,
    ID_SEPARATOR,
  ID_CK_LIC_AUTH,
  ID_CK_LIC_INFO,
    ID_SEPARATOR,
  ID_ONLINE_HOME,
  };

#define AltToolbarStyle 0

static UINT BASED_CODE UnknownModeToolBarIds[] = //project loaded
  {
  ID_FILE_NEW,
  ID_FILE_OPEN,
  //ID_FILE_SAVE,
  ID_PROJECT_SAVEALL,
  ID_PROJECT_SAVEASNV,
    ID_SEPARATOR,
  ID_VIEW_EXPLORER,
  ID_PROJECT_OPTIONS,
  //ID_OPTIONS_DRIVER,
  ID_FILE_PRINT,
  //ID_WNDW_SELECT,
  ID_OPTIONS_REPORTS,
    ID_SEPARATOR,
#if AltToolbarStyle
  ID_ACTIONS_RUN,
  ID_ACTIONS_IDLE,
  ID_ACTIONS_EDIT,
#else
  ID_ACTIONS_EDIT_TGL,
  ID_ACTIONS_RUN_TGL,
#endif
  ID_ACTIONS_STEP,
  ID_ACTIONS_RUN_STEADY,
  ID_ACTIONS_SETUP,
//    ID_SEPARATOR,
//  ID_PROJECT_EDIT_COLOURS
  };

static UINT BASED_CODE TrendToolBarIds[] =
  {
  ID_TREND_SCROLL_BACKWARDFAST,
  ID_TREND_SCROLL_BACKWARD,
    ID_SEPARATOR,
  ID_TREND_SCROLL_FORWARD,
  ID_TREND_SCROLL_FORWARDFAST,
  ID_TREND_END,
    ID_SEPARATOR,
  ID_TREND_ZOOM_IN,
  ID_TREND_ZOOM_OUT,
//    ID_SEPARATOR,
//  ID_TREND_SET,
//    ID_SEPARATOR,
//  ID_TREND_GLOBAL,
  };

static UINT BASED_CODE GrfToolBarIds[] =
  {
  ID_GRF_Insert,
  ID_GRF_Connect,
  ID_GRF_InsertText,
    ID_SEPARATOR,
  ID_GRF_TagAnnot,
  ID_GRF_MoveEntity,
  ID_GRF_AlterEntity,
  ID_GRF_MoveLink,
  ID_GRF_ChangeUnit,
  ID_GRF_DeleteUnit,
    ID_SEPARATOR,
  ID_GRF_DoZoomOnce,
  ID_GRF_DoZoomAll,
  ID_GRF_Zoom,
  ID_GRF_DoRedraw,
  ID_FIND_TAG,
#if WITHGRFGROUP
  ID_SEPARATOR,
  ID_GRF_CreateGroup,
  ID_GRF_InsertGroup,
#endif
  };

static UINT BASED_CODE ConstructToolBarIds[] =
  {
  ID_GRF_ConstructUnit,
  ID_GRF_ConstructLink,
  ID_GRF_ConstructTie,
  ID_GRF_ConstructSymbol,
    ID_SEPARATOR,
  ID_GRF_ExplodeUnit,
    ID_SEPARATOR,
  ID_GRF_DoZoomOnce,
  ID_GRF_DoZoomAll,
  ID_GRF_Zoom,
  ID_GRF_DoRedraw,
  ID_FIND_TAG,
    ID_SEPARATOR,
  ID_GRF_DeleteUnit,
  };

static UINT BASED_CODE DynamicToolBarIds[] =
  {
  ID_GRF_CreateFill,
  ID_GRF_CreateDynamic,
  ID_GRF_CreateAnnotation,
  ID_GRF_CreateToggle,
    ID_SEPARATOR,
  ID_GRF_UpdateFill,
  ID_GRF_UpdateDynamic,
  ID_GRF_UpdateAnnotation,
  ID_GRF_UpdateToggle,
    ID_SEPARATOR,
  ID_GRF_RefreshAllAnnotation,
  };

static UINT BASED_CODE OtherToolBarIds[] =
  {
  ID_OPTIONS_DRIVER,
    ID_SEPARATOR,
  ID_FILE_SAVESNAPSHOT,
  ID_FILE_LOADSNAPSHOT,
    ID_SEPARATOR,
  //ID_FIND_TAG,
  ID_GRF_MarkEntities,
  ID_GRF_MoveEntity,
  ID_GRF_AlterEntity,
  //ID_GRF_Zoom,
  //ID_GRF_DoZoomAll,
  };


static UINT BASED_CODE GeneralToolBarIds[] =
  {
  //ID_FILE_SAVESNAPSHOT,
  //ID_FILE_LOADSNAPSHOT,
  //  ID_SEPARATOR,
  //ID_FIND_TAG,
  //  ID_SEPARATOR,
  ID_GRF_HelpUnit,
  ID_ONLINE_PGMLANGUAGE,
  ID_ONLINE_HOME,
  };
/*KGA: 6 May 2005: Remove runtime toolbars
static UINT BASED_CODE SysCADToolBarIds_RunTime[] = //no project loaded
  {
  ID_FILE_NEW,
  ID_FILE_OPEN,
  ID_PROJECT_LOADALTCFG,
    ID_SEPARATOR,
  ID_PROJECT_OPTIONS,
  ID_PROJECT_EDIT_DEFSDB,
    ID_SEPARATOR,
  ID_CK_LIC_AUTH,
  ID_CK_LIC_INFO,
    ID_SEPARATOR,
  ID_HELP_SYSCAD,
  };

static UINT BASED_CODE GrfToolBarIds_RunTime[] =
  {
  ID_GRF_TagAnnot,
    ID_SEPARATOR,
  ID_GRF_DoZoomOnce,
  ID_GRF_DoZoomAll,
  ID_GRF_Zoom,
  ID_GRF_DoRedraw,
  ID_FIND_TAG,
  };


static UINT BASED_CODE ConstructToolBarIds_RunTime[] =
  {
  ID_GRF_DoZoomOnce,
  ID_GRF_DoZoomAll,
  ID_GRF_Zoom,
  ID_GRF_DoRedraw,
  ID_FIND_TAG,
  };

static UINT BASED_CODE DynamicToolBarIds_RunTime[] =
  {
  ID_GRF_RefreshAllAnnotation,
  };

static UINT BASED_CODE OtherToolBarIds_RunTime[] =
  {
  ID_OPTIONS_DRIVER,
    ID_SEPARATOR,
  ID_FILE_SAVESNAPSHOT,
  ID_FILE_LOADSNAPSHOT,
    ID_SEPARATOR,
  ID_GRF_RefreshAllAnnotation,
  ID_GRF_MarkEntities,
  };
*/
static CDynToolBarInfo MainToolBars[] =
  {
    { "SysCAD",    IDR_TOOLBARS, IDW_TOOLBAR_0, sizeof(SysCADToolBarIds)/sizeof(UINT),    SysCADToolBarIds    },
    { "Graphics",  IDR_TOOLBARS, IDW_TOOLBAR_1, sizeof(GrfToolBarIds)/sizeof(UINT),       GrfToolBarIds       },
    { "Trend",     IDR_TOOLBARS, IDW_TOOLBAR_2, sizeof(TrendToolBarIds)/sizeof(UINT),     TrendToolBarIds     },
    { "Construct", IDR_TOOLBARS, IDW_TOOLBAR_3, sizeof(ConstructToolBarIds)/sizeof(UINT), ConstructToolBarIds },
    { "Dynamics",  IDR_TOOLBARS, IDW_TOOLBAR_4, sizeof(DynamicToolBarIds)/sizeof(UINT),   DynamicToolBarIds   },
    { "Other",     IDR_TOOLBARS, IDW_TOOLBAR_5, sizeof(OtherToolBarIds)/sizeof(UINT),     OtherToolBarIds     },
    { "Help",      IDR_TOOLBARS, IDW_TOOLBAR_6, sizeof(GeneralToolBarIds)/sizeof(UINT),   GeneralToolBarIds   },
  };

//---------------------------------------------------------------------------

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
  {
  if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
    return -1;

  // Subclass the MDIClient window.
  if (!m_MDIClientWnd.SubclassWindow(m_hWndMDIClient))
    return -1; //Failed to subclass MDI client window

//  if (!m_wndToolBar.Create(this))
//    return -1;
//  if (!m_wndToolBar.LoadToolBar(IDR_MOREGRF))
//    return -1;

  //create status bar...
  if (!m_wndStatusBar.Create(this))
    return -1;    //Failed to create status bar
  if (!m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT)))
    return -1;    //Failed to set indicators for status bar
  m_wndStatusBar.UseLocks();
  m_wndStatusBar.SetNoOfIndicators(sizeof(indicators)/sizeof(UINT) - 1);

//m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

//m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
//EnableDocking(CBRS_ALIGN_ANY);
//DockControlBar(&m_wndToolBar);

  EnableDocking(CBRS_ALIGN_ANY);// | CBRS_FLOAT_MULTI);

  //create main toolbars...
  MainTBMngr.Initialise(this);
  MainTBMngr.AddBitmap(IDR_TOOLBARS, ToolbarIds, sizeof(ToolbarIds)/sizeof(UINT));
  pMainTBMngr = &MainTBMngr;
  const int TBCount = sizeof(MainToolBars)/sizeof(CDynToolBarInfo);
  for (int i=0; i<TBCount; i++)
    {
    CDynToolBar* pTB = MainTBMngr.AddToolbar(MainToolBars[i].pName, TRUE, MainToolBars[i].iToolbarID);
    pTB->AddButtonsOnce(MainToolBars[i].iBitmapID, MainToolBars[i].IDs, MainToolBars[i].iLen);
    pTB->Create(TRUE, FALSE, CBRS_ALIGN_ANY, CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);//, (i==0));
    }
  DockControlBarRelative(MainTBMngr[1], MainTBMngr[0], True);
  DockControlBarRelative(MainTBMngr[2], MainTBMngr[1], True);
  DockControlBarRelative(MainTBMngr[4], MainTBMngr[3], True);
  DockControlBarRelative(MainTBMngr[5], MainTBMngr[4], True);
  DockControlBarRelative(MainTBMngr[6], MainTBMngr[5], True);
  for (i=1; i<TBCount; i++)
    {
    ShowControlBar(MainTBMngr[i], FALSE, FALSE);
    //MainTBMngr[i]->ModifyStyle(0, TBSTYLE_FLAT);
    }

  //Initialize graphics dynamic toolbars...
  GrfTBMngr.Initialise(this);
  GrfTBMngr.AddBitmap(IDR_GRFSYMBOLS, GrfSymbsIds, sizeof(GrfSymbsIds)/sizeof(UINT));
  pGrfTBMngr = &GrfTBMngr;

  return 0;
  }

//---------------------------------------------------------------------------

void CMainFrame::UpdateMainToolBar()
  {//no project is loaded, change toolbar & menu...
  const int TBCount = sizeof(MainToolBars)/sizeof(CDynToolBarInfo);
  for (int i=1; i<TBCount; i++)
    ShowControlBar(MainTBMngr[i], FALSE, FALSE);
  MainTBMngr[0]->ChangeButtons(SysCADToolBarIds, sizeof(SysCADToolBarIds)/sizeof(UINT));
  ShowControlBar(MainTBMngr[0], TRUE, TRUE);
  CMenu Menu;
  Menu.LoadMenu(IDR_MAINFRAME);
  SetMenu(&Menu);
  Menu.Detach();
  }

//---------------------------------------------------------------------------

void CMainFrame::UpdateToolBars()
  {
  const int TBCount = sizeof(MainToolBars)/sizeof(CDynToolBarInfo);
  for (int i=0; i<TBCount; i++)
    {
    DWORD dwBarStyle = MainTBMngr[i]->GetBarStyle();
    dwBarStyle = (gs_pPrj->m_bToolTips ? (dwBarStyle | CBRS_TOOLTIPS) : (dwBarStyle & (~CBRS_TOOLTIPS)));
    dwBarStyle = (gs_pPrj->m_bFlyBys ? (dwBarStyle | CBRS_FLYBY) : (dwBarStyle & (~CBRS_FLYBY)));
    MainTBMngr[i]->SetBarStyle(dwBarStyle);
    }
  m_wndStatusBar.Invalidate();
  RecalcLayout();
  }

//---------------------------------------------------------------------------

void CMainFrame::DefaultToolBar(int index, BOOL ShowNow)
  {
  const int DfltShowNowList[]={True, True, True, False, False, False, False};
  int i = (index<0 ? 0 : index), iPrv=-1;
  int TBCount = (index<0 ? sizeof(MainToolBars)/sizeof(CDynToolBarInfo) : index+1);
  for ( ; i<TBCount; i++)
    {
    if (i==0)
      MainTBMngr[i]->ChangeButtons(UnknownModeToolBarIds, sizeof(UnknownModeToolBarIds)/sizeof(UINT));
    else
      MainTBMngr[i]->ChangeButtons(MainToolBars[i].IDs, MainToolBars[i].iLen);

    flag Show=(index<0) ? DfltShowNowList[i] : ShowNow;

    if (Show && !MainTBMngr[i]->IsWindowVisible())
      {
      ShowControlBar(MainTBMngr[i], TRUE, TRUE);
      if (index<0 && iPrv>=0)
        DockControlBarRelative(MainTBMngr[i], MainTBMngr[iPrv], True);
      iPrv=i;
      }
    else if (!Show && MainTBMngr[i]->IsWindowVisible())
      ShowControlBar(MainTBMngr[i], FALSE, TRUE);
    MainTBMngr[i]->SetTitle(MainToolBars[i].pName);
    }
  //UpdateToolBars();
  RecalcLayout();
  }

//---------------------------------------------------------------------------

void CMainFrame::SetToolBarSolveMode()
  {
  const int TBCount = (sizeof(MainToolBars)/sizeof(CDynToolBarInfo));
  const int IsDynMode=DefNetDynamicMode();
  UINT ID1a=ID_ACTIONS_RUN_TGL;
  UINT ID2a=IsDynMode ? ID_ACTIONS_RUNPB_TGL : ID_ACTIONS_RUNDYN_TGL;
  UINT IDRa=IsDynMode ? ID_ACTIONS_RUNDYN_TGL : ID_ACTIONS_RUNPB_TGL;
  UINT ID1b=ID_ACTIONS_RUN;
  UINT ID2b=IsDynMode ? ID_ACTIONS_RUNPB : ID_ACTIONS_RUNDYN;
  UINT IDRb=IsDynMode ? ID_ACTIONS_RUNDYN : ID_ACTIONS_RUNPB;

  for (int i=0; i<TBCount; i++)
    {
    MainTBMngr[i]->ChangeButtonIDS(ID1a, ID2a, IDRa);
    MainTBMngr[i]->ChangeButtonIDS(ID1b, ID2b, IDRb);
    }

  //UpdateToolBars();
  RecalcLayout();
  }

//===========================================================================

CMyMDIClient::CMyMDIClient()
  {
  }

//---------------------------------------------------------------------------

CMyMDIClient::~CMyMDIClient()
  {
  }

//---------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(CMyMDIClient, CWnd)

BEGIN_MESSAGE_MAP(CMyMDIClient, CWnd)
  //{{AFX_MSG_MAP(CMyMDIClient)
  ON_WM_PAINT()
  ON_WM_SIZE()
  ON_WM_MOVE()
  ON_WM_RBUTTONDOWN()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

void CMyMDIClient::OnPaint()
  {
  bool DoPaint = (gs_pPrj && gs_pPrj->LoadBusy());
  #if CK_LICENSINGON
  bool TextBackground = (gs_License.DemoMode() || gs_License.AcademicMode() || gs_License.AllowMineServe() || gs_License.Blocked());
  if (!DoPaint)
    DoPaint = TextBackground;
  #endif
  if (DoPaint)
    {
    CPaintDC dc(this); // device context for painting
    CDCResChk ResChk(dc);
    CRect Rect;
    GetClientRect(&Rect);
    dc.FillSolidRect(&Rect, RGB(128, 128, 128));
    dc.SetBkColor(RGB(128, 128, 128));
    CFont * pOldFont=NULL;

    if (1)
      {
      CRect SRect=Rect;
      ClientToScreen(SRect);
      int xx=0;
      }


    #if CK_LICENSINGON
    if (TextBackground)
      {
      int NDown   = 5;
      int NAcross = 3;

      if (gs_License.Blocked())
        NAcross = 1;

      int Width   = Rect.Width();
      int Height  = Rect.Height();
      int DHeight = Height/(NDown+1);
      int DWidth  = Width/(NAcross);
      CFont Font;
      //Font.CreatePointFont(gs_License.Blocked() ? 280 : 360, "Arial");
      Font.CreatePointFont(gs_License.Blocked() ? 200 : 120, "Arial");
      pOldFont=dc.SelectObject(&Font);

      CString Msg;
      if (gs_License.Blocked())
        {
        Msg="License conditions exceeded, restart SysCAD.";
        }
      else if (gs_License.AcademicMode())
        {
        Msg.Format("Academic License : %s", gs_License.AcademicName());
        }
      else if (gs_License.AllowMineServeCOM())
        {
        Msg.Format("mineSERV License");
        }
      else if (gs_License.DemoMode())
        {
        if (gs_License.DidInitCrypkey())
          Msg="Using SysCAD in Demo Mode";
        else
          Msg="Using Demo License, Licensing service Error!";
        }
      CRect TxtRct;
      dc.DrawText(Msg,  Msg.GetLength(),  TxtRct, DT_CALCRECT|DT_TOP|DT_NOPREFIX|DT_LEFT); 
      for (int i=1; i<=NDown; i++)
        {
        for (int j=1; j<=NAcross; j++)
          dc.TextOut(j*DWidth-DWidth/2-TxtRct.Width()/2, i*DHeight/*-DHeight/2*/-TxtRct.Height()/2, Msg, Msg.GetLength());
        }
      }
    #endif             
    if (gs_pPrj && gs_pPrj->LoadBusy())
      {
      CFont Font;
      Font.CreatePointFont(100, "Arial");
      pOldFont=dc.SelectObject(&Font);
      Strng s;
      if (PrjFile() && strlen(PrjFile())>0)
        {
        s="Busy loading project : '";
        s+=PrjFile();
        s+="'";
        dc.TextOut(16, 10, s(), s.Len());
        }              
      if (CfgFile() && strlen(CfgFile())>0)
        {
        s="Using configuration file : '";
        s+=CfgFile();
        s+="'";
        dc.TextOut(16, 32, s(), s.Len());
        }
      }

    if (pOldFont)
      dc.SelectObject(pOldFont);
    }
  else
    {
    //// CNM - Added this back - seems to fix a problem with no licensing
    CWnd::OnPaint();
    }

  // Do not call CWnd::OnPaint() for painting messages
  }

//---------------------------------------------------------------------------

void CMyMDIClient::OnMove(int cx, int cy)
  {
  };

//---------------------------------------------------------------------------

void CMyMDIClient::OnSize(UINT Type, int cx, int cy)
  {
  Invalidate();
  };

//---------------------------------------------------------------------------

void CMyMDIClient::OnRButtonDown(UINT nFlags, CPoint point)
  {
  CMenu Menu;
  //Menu.CreatePopupMenu();
  Menu.LoadMenu(IDR_MDICLIENT_MENU);
  POINT ScreenPoint = point;
  CMenu* pMenuTrackPopup;
  if (EnablePrjOK() && (EnableAnalysing() || EnableBusy()))
    pMenuTrackPopup = Menu.GetSubMenu(1);
  else
    pMenuTrackPopup = Menu.GetSubMenu(0);
  // Convert the mouse point to screen coordinates since that is what TrackPopupMenu() expects.
  ClientToScreen(&ScreenPoint);
  pMenuTrackPopup->TrackPopupMenu(TPM_RIGHTBUTTON, ScreenPoint.x, ScreenPoint.y, AfxGetMainWnd(), NULL);
  Menu.DestroyMenu();
  CWnd::OnRButtonDown(nFlags, point);
  }

//---------------------------------------------------------------------------

inline void DoGlblAccess(char *pTag)
  {
  CWaitMsgCursor Wait(" ");
  Wait.UpdateMsg("Access %s", pTag);
  gs_AccessWnds.AccessNode(-1, pTag);
  }

//---------------------------------------------------------------------------

void CMainFrame::OnViewChangestats()
  {
  DoGlblAccess("$Chg");
  }

void CMainFrame::OnUpdateViewChangestats(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(EnablePrjOK() && EnableNotStopped());
  }

//---------------------------------------------------------------------------

void CMainFrame::OnViewPowersupplies()
  {
  DoGlblAccess("$Pwr");
  }

void CMainFrame::OnUpdateViewPowersupplies(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(EnablePrjOK() && EnableNotStopped());
  }

//---------------------------------------------------------------------------

void CMainFrame::OnViewIOPoints()
  {
  DoGlblAccess("$IO");
  }

void CMainFrame::OnUpdateViewIOPoints(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(EnablePrjOK() && EnableNotStopped());
  }

//---------------------------------------------------------------------------

void CMainFrame::OnViewEvalorder()
  {
  //DoGlblAccess("$Order");
  if (COrdWindow::GetFrame()==NULL)
    COrdWindow::Register();
  COrdWindow::Show(true);
  }

void CMainFrame::OnUpdateViewEvalorder(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(EnablePrjOK() && EnableNotStopped());
  }

//---------------------------------------------------------------------------

void CMainFrame::OnViewEvalstats()
  {
  //DoGlblAccess("$Order");
  if (CStatsWindow::GetFrame()==NULL)
    CStatsWindow::Register();
  CStatsWindow::Show(true);
  }

void CMainFrame::OnUpdateViewEvalstats(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(EnablePrjOK() && EnableNotStopped());
  }

//---------------------------------------------------------------------------

void CMainFrame::OnViewControls()
  {
#if WITHCTRLEDIT
  DoGlblAccess("$Ctrl");
#else
  CTagVwDocC*pDoc=NULL;
  //int iTemplate=1; // CtrlTrends
  if (!ScdApp()->TemplateExists(iControlTemplate))
    return;

  CDocTemplate &CtrlTemplate=ScdApp()->Template(iControlTemplate);
  POSITION Pos = CtrlTemplate.GetFirstDocPosition();
  while (Pos && pDoc==NULL)
    pDoc=dynamic_cast<CTagVwDocC*>(CtrlTemplate.GetNextDoc(Pos));
  if (pDoc)
    {
    CWnd *w=NULL;
    POSITION Pos=pDoc->GetFirstViewPosition();
    if (Pos)
      w=pDoc->GetNextView(Pos);
    while (w && !(w->IsKindOf(RUNTIME_CLASS(CFrameWnd))))
      w = w->GetParent();
    if (w && (w->IsKindOf(RUNTIME_CLASS(CFrameWnd))))
      w->BringWindowToTop();
    }
  else
    {
    CtrlTemplate.OpenDocumentFile(NULL);
    }
#endif
  }

void CMainFrame::OnUpdateViewControls(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(EnablePrjOK() && EnableNotStopped());
  }

//---------------------------------------------------------------------------

void CMainFrame::OnViewDynamicinfo()
  {
  DoGlblAccess("$Dyn");
  }

void CMainFrame::OnUpdateViewDynamicinfo(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(EnablePrjOK() && EnableNotStopped());
  }

//---------------------------------------------------------------------------

void CMainFrame::OnViewPlantmodel()
  {
  DoGlblAccess(PlantModelTag);
  }

void CMainFrame::OnUpdateViewPlantmodel(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(EnablePrjOK() && EnableNotStopped());
  }

//---------------------------------------------------------------------------

void CMainFrame::OnViewPlantarea()
  {
  DoGlblAccess(PlantAreaTag);
  }

void CMainFrame::OnUpdateViewPlantarea(CCmdUI* pCmdUI)
  {
  //pCmdUI->Enable(0);
  pCmdUI->Enable(EnablePrjOK() && EnableNotStopped());
  }

//---------------------------------------------------------------------------

void CMainFrame::OnViewProbalinfo()
  {
  DoGlblAccess("$PB");
  }

void CMainFrame::OnUpdateViewProbalinfo(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(EnablePrjOK() && EnableNotStopped());
  }

//---------------------------------------------------------------------------

void CMainFrame::OnViewSpeciedata()
  {
  DoGlblAccess("$SDB");
  }

void CMainFrame::OnUpdateViewSpeciedata(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(EnablePrjOK() && EnableNotStopped());
  }

//---------------------------------------------------------------------------

void CMainFrame::OnViewTearblocks()
  {
  DoGlblAccess("$Tear");
  }

void CMainFrame::OnUpdateViewTearblocks(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(EnablePrjOK() && EnableNotStopped());
  }

//---------------------------------------------------------------------------

void CMainFrame::OnViewTolerances()
  {
  DoGlblAccess("$Tol");
  }

void CMainFrame::OnUpdateViewTolerances(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(EnablePrjOK());// && EnableNotStopped());
  }

//---------------------------------------------------------------------------

void CMainFrame::OnViewRptTagLists()
  {
  CRptTagLists::Show();
  }

void CMainFrame::OnUpdateViewRptTagLists(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(EnablePrjOK() && EnableNotStopped());

  }

//---------------------------------------------------------------------------

void CMainFrame::OnViewMessages()
  {
  CMsgWindow::Show(true);
  }

void CMainFrame::OnUpdateViewMessages(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(true);
  }

//---------------------------------------------------------------------------

void CMainFrame::OnViewProject()
  {
  CMDIChildWnd * pPrjWnd=NULL;
  if (gs_pPrj)
    {
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
          if (pDoc->IsKindOf(RUNTIME_CLASS(CPrjDoc)))
            {
            ASSERT(pPrjWnd==NULL); //Two project windows!!! Why???
            ASSERT(w->IsKindOf(RUNTIME_CLASS(CPrjFrameWnd)));
            pPrjWnd = (CMDIChildWnd*) w;
            }
          }
        }
      }
    }
  if (pPrjWnd)
    {
    pPrjWnd->MDIActivate(); //bring the window to the front
    if (pPrjWnd->IsIconic())
      pPrjWnd->ShowWindow(SW_RESTORE);
    }
  }

void CMainFrame::OnUpdateViewProject(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(EnablePrjOK());
  }

//---------------------------------------------------------------------------

void CMainFrame::OnError()
  {
  CMsgWindow::Show(true);
  }

void CMainFrame::OnUpdateError(CCmdUI* pCmdUI)
  {
  }

//---------------------------------------------------------------------------

void CMainFrame::OnWarning()
  {
  }

void CMainFrame::OnUpdateWarning(CCmdUI* pCmdUI)
  {
  }

//---------------------------------------------------------------------------

void CMainFrame::OnMeasureItem( int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct )
  {
  MEASUREITEMSTRUCT &MIS=*lpMeasureItemStruct;
  if (MIS.itemID==IDM_ERROR)
    {
    int x=::GetSystemMetrics(SM_CXMENUSIZE);
    int y=::GetSystemMetrics(SM_CYMENUSIZE);
    MIS.itemWidth=x*6;
    MIS.itemHeight=y;
    }
  /*else if (MIS.itemID==IDM_GRFJUMP)
    {
    int x=::GetSystemMetrics(SM_CXMENUSIZE);
    int y=::GetSystemMetrics(SM_CYMENUSIZE);
    MIS.itemWidth=x*6;
    MIS.itemHeight=y;
    }*/
  };

//---------------------------------------------------------------------------

void CMainFrame::OnDrawItem( int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct )
  {
  DRAWITEMSTRUCT & DIS=*lpDrawItemStruct;
  if (DIS.itemID==IDM_ERROR)
    {
    CDC dc;
    CRect Rct(0,0,0,0), RctD(0,0,0,0);
    dc.Attach(DIS.hDC);
    //TODO RESTORE Checks

    //CDCResChk ResChk(dc);
    CBitmap* poldbmp = dc.GetCurrentBitmap();//NULL;//memdc.SelectObject(&m_MsgIcon);

    dc.DrawEdge(&DIS.rcItem, EDGE_ETCHED, BF_LEFT);

    DWORD OldTxtColour=dc.SetTextColor(::GetSysColor(COLOR_MENUTEXT));

    CBitmap *CBM[3], *CBMBW[3];
    CBM[0]=&m_ErrorBitmap;
    CBM[1]=&m_WarnBitmap;
    CBM[2]=&m_NoteBitmap;
    CBMBW[0]=&m_ErrorBWBitmap;
    CBMBW[1]=&m_WarnBWBitmap;
    CBMBW[2]=&m_NoteBWBitmap;

    BITMAP BM[3];
    m_ErrorBitmap.GetBitmap(&BM[0]);
    m_WarnBitmap.GetBitmap(&BM[1]);
    m_NoteBitmap.GetBitmap(&BM[2]);

    m_nMsgs[0] = CMsgWindow::NErrorMsgs();
    m_nMsgs[1] = CMsgWindow::NWarningMsgs();
    m_nMsgs[2] = CMsgWindow::NNoteMsgs();

    int w=::GetSystemMetrics(SM_CXMENUSIZE);
    int h=::GetSystemMetrics(SM_CYMENUSIZE);
    int y=DIS.rcItem.top;
    int x=DIS.rcItem.left;
    CDC memdc;
    memdc.CreateCompatibleDC(&dc);
    //TODO RESTORE Checks
    //CDCResChk ResChkMem(memdc);
    CBitmap* pmemoldbmp = memdc.GetCurrentBitmap();//NULL;//memdc.SelectObject(&m_MsgIcon);

    char Buff[36];
    int XSpc=(BM[0].bmWidth*2)/3;
    x+=XSpc;

    for (int i=0; i<3; i++)
      {
      memdc.SelectObject(m_nMsgs[i] ? CBM[i] : CBMBW[i]);
      dc.BitBlt(x, y+(h-BM[i].bmHeight)/2, BM[i].bmWidth, BM[i].bmHeight, &memdc, 0, 0, SRCCOPY);
      x+=BM[i].bmWidth+2;
      ::_ltoa(m_nMsgs[i], Buff, 10);
      Rct.SetRect(0,0,0,0);
      dc.DrawText(Buff, strlen(Buff), &Rct, DT_CALCRECT|DT_TOP|DT_NOPREFIX|DT_LEFT);
      Rct.OffsetRect(x, y+(h-Rct.Height())/2);
      dc.DrawText(Buff, strlen(Buff), &Rct, DT_TOP|DT_NOPREFIX|DT_LEFT);
      x+=Rct.Width()+XSpc;
      }

    dc.SelectObject(poldbmp);
    memdc.SelectObject(pmemoldbmp);

    dc.SetTextColor(OldTxtColour);

    dc.Detach();
    }
  /*else if (DIS.itemID==IDM_GRFJUMP)
    {
    CDC dc;
    CRect Rct(0,0,0,0), RctD(0,0,0,0);
    dc.Attach(DIS.hDC);
    //TODO RESTORE Checks

    //CDCResChk ResChk(dc);
    CBitmap* poldbmp = dc.GetCurrentBitmap();//NULL;//memdc.SelectObject(&m_MsgIcon);

    dc.DrawEdge(&DIS.rcItem, EDGE_ETCHED, BF_LEFT);

    DWORD OldTxtColour=dc.SetTextColor(::GetSysColor(COLOR_MENUTEXT));

    CBitmap *pBM=&m_ErrorBitmap;

    BITMAP BM[3];
    m_ErrorBitmap.GetBitmap(&BM[0]);

    int w=::GetSystemMetrics(SM_CXMENUSIZE);
    int h=::GetSystemMetrics(SM_CYMENUSIZE);
    int y=DIS.rcItem.top;
    int x=DIS.rcItem.left;
    CDC memdc;
    memdc.CreateCompatibleDC(&dc);
    //TODO RESTORE Checks
    //CDCResChk ResChkMem(memdc);
    CBitmap* pmemoldbmp = memdc.GetCurrentBitmap();//NULL;//memdc.SelectObject(&m_MsgIcon);

    char Buff[36];
    int XSpc=(BM[0].bmWidth*2)/3;
    x+=XSpc;

      memdc.SelectObject(pBM);
      dc.BitBlt(x, y+(h-BM[0].bmHeight)/2, BM[0].bmWidth, BM[0].bmHeight, &memdc, 0, 0, SRCCOPY);
      x+=BM[0].bmWidth+2;
      //::_ltoa(m_nMsgs[i], Buff, 10);
      //Rct.SetRect(0,0,0,0);
      //dc.DrawText(Buff, strlen(Buff), &Rct, DT_CALCRECT|DT_TOP|DT_NOPREFIX|DT_LEFT);
      //Rct.OffsetRect(x, y+(h-Rct.Height())/2);
      //dc.DrawText(Buff, strlen(Buff), &Rct, DT_TOP|DT_NOPREFIX|DT_LEFT);
      //x+=Rct.Width()+XSpc;

    dc.SelectObject(poldbmp);
    memdc.SelectObject(pmemoldbmp);

    dc.SetTextColor(OldTxtColour);

    dc.Detach();
    }*/
  else
    DoBreak();
  };

//---------------------------------------------------------------------------

LRESULT CMainFrame::OnUpdateMsgMenuBtn(WPARAM wParam, LPARAM lParam)
  {
  CMenu * pMenu=GetMenu();
  if (pMenu)
    {
    if (pMenu->GetMenuState(IDM_ERROR, MF_BYCOMMAND)==0xFFFFFFFF)
      {
      pMenu->AppendMenu(MF_OWNERDRAW, IDM_ERROR);
      DrawMenuBar();
      }
    else if (m_nMsgs[0] != CMsgWindow::NErrorMsgs() ||
             m_nMsgs[1] != CMsgWindow::NWarningMsgs() ||
             m_nMsgs[2] != CMsgWindow::NNoteMsgs())
      {
      DrawMenuBar();
      }
    /*if (pMenu->GetMenuState(IDM_GRFJUMP, MF_BYCOMMAND)==0xFFFFFFFF)
      {
      pMenu->AppendMenu(MF_OWNERDRAW, IDM_GRFJUMP);
      DrawMenuBar();
      }*/
    /*else if (prevnoofgrf!=noofgrf)
      {
      DrawMenuBar();
      }*/
    }
  return True;
  }

//---------------------------------------------------------------------------

bool CMainFrame::GetWorkRect(CRect & ClientRect, CRect & ScreenRect)
  {
  CWnd * p = GetTopWindow();
  //CMyMDIClient*p=dynamic_cast<CMyMDIClient*>(GetTopWindow());
  while (p)
    {
    CMyMDIClient*pp=dynamic_cast<CMyMDIClient*>(p);
    if (pp)
      {
      pp->GetClientRect(ClientRect);
      ScreenRect=ClientRect;
      pp->ClientToScreen(ScreenRect);
      return true;
      }
    else
      p=p->GetNextWindow();
    }
  return false;
  }

//---------------------------------------------------------------------------

bool CMainFrame::GetInitRect(int Which, CRect & InitRect)
  {
  CRect ClientRect, ScreenRect;
  if (GetWorkRect(ClientRect, ScreenRect))
    {
    switch (Which)
      {
      case 0: // Explorer
        InitRect=CRect(ScreenRect.left, ScreenRect.top, ScreenRect.left+150, ScreenRect.top+(ScreenRect.Height()*3)/4);
        break;
      case 1: // Grf
        InitRect=CRect(150, 0, 0+(ClientRect.right*3)/4, (ClientRect.bottom*5)/8);
        break;
      case 2: // Trnd
        InitRect=CRect(150, (ClientRect.bottom*5)/8, 0+(ClientRect.right*3)/4, (ClientRect.bottom*7)/8);
        break;
      }
    return true;
    }
  return false;
  };

//---------------------------------------------------------------------------

void CMainFrame::OnSize(UINT nType, int cx, int cy)
  {
  CMDIFrameWnd::OnSize(nType, cx, cy);

  if (CProject::sm_SysCADInited && !gs_pPrj->bDoingLoad)
    CProject::SaveOneWindow(0, CWindowLists::MainWndTitle, AfxGetMainWnd(), gs_ProjectOpenFlag==0);
  }

//---------------------------------------------------------------------------

void CMainFrame::OnMove(int x, int y)
  {
  CMDIFrameWnd::OnMove(x, y);
  CExploreScd::MainWndMovedTo(x,y);

  if (CProject::sm_SysCADInited && !gs_pPrj->bDoingLoad)
    CProject::SaveOneWindow(0, CWindowLists::MainWndTitle, AfxGetMainWnd(), gs_ProjectOpenFlag==0);
  }

//---------------------------------------------------------------------------

LRESULT CMainFrame::OnFlushMsgQ(WPARAM wParam, LPARAM lParam)
  {
  return ScdApp()->DoFlushMsgQ(wParam, lParam);
  }

//---------------------------------------------------------------------------
#if WITHDRVMAN
LRESULT CMainFrame::OnReloadDriver(WPARAM wParam, LPARAM lParam)
  {
  return gs_pPrj->ReloadDrvManager(TRUE);
  }
#endif
//---------------------------------------------------------------------------

LRESULT CMainFrame::OnCOMCmd(WPARAM wParam, LPARAM lParam)
  {
  return CmdExec().DoCOMCmd(0, wParam, NULL, lParam);
  }

//---------------------------------------------------------------------------

LRESULT CMainFrame::OnCOMApp(WPARAM wParam, LPARAM lParam)
  {
  CScdCOCmdBlk * pCmdBlk = (CScdCOCmdBlk *)lParam;
  return CmdExec().DoCOMCmd(1, wParam, pCmdBlk, pCmdBlk->m_lData);
  }

//---------------------------------------------------------------------------

LRESULT CMainFrame::OnCOMSlv(WPARAM wParam, LPARAM lParam)
  {
  INCOMPLETECODEMSG("Should not get here", __FILE__, __LINE__);
  CScdCOCmdBlk * pCmdBlk = (CScdCOCmdBlk *)lParam;
  return CmdExec().DoCOMCmd(2, wParam, pCmdBlk, pCmdBlk->m_lData);
  }

//---------------------------------------------------------------------------

LRESULT CMainFrame::OnCOMEvtRelay(WPARAM wParam, LPARAM lParam)
  {
  //gs_Events.Send(wParam, lParam);
  return 0;
  }

//---------------------------------------------------------------------------

LRESULT CMainFrame::OnExecUpdateDisplay(WPARAM wParam, LPARAM lParam)
  {
  static int IdlingCnt = 0;
  const char* StatusRotate = "/-\\|";


  bool OnlyStatusLine = (wParam&0x01)!=0;
  //dbgpln("ExecUpdateDisplay %s", OnlyStatusLine?"Status":"All");

  CXM_TimeControl & CB=*(CXM_TimeControl*)lParam;
  const int ExecStateIndex = gs_Exec.StateIndex();
  if (gs_Exec.Busy())
    {
    const int IsDynMode=DefNetDynamicMode();
    const double Progress = IsDynMode ? gs_Exec.DynProgress : -1.0;
    CString Buff2;
    if (IsDynMode)
      {
      if (CB.m_bRealTime)
        {
        __time64_t t=(__time64_t)CB.m_TheTime.Seconds;
        struct tm T, *pT=_localtime64(&t);
        if (pT)
          {
          memcpy(&T, pT, sizeof(T));
          Buff2.Format("%2i:%02i:%02i", T.tm_hour, T.tm_min, T.tm_sec);//t % 60); // t % 60 used because t may have
          }
        else
          Buff2="??:??:??";
        }
      else
        {
        if (ValidTime(CB.m_TheTime))
          {
          if (Progress<0.0)
            Buff2=CB.m_TheTime.Format(TD_Time|(CB.m_StepSize>=60.0?TD_IntegralSecs:0));
          else
            {
            CTimeValue d = gs_Exec.TheTime-gs_Exec.TimeStarted;
            Buff2=d.Format(TD_Time|TD_Days|((CB.m_StepSize>=60.0 || d>3600.0) ? TD_IntegralSecs:0));
            }
          }
        else
          Buff2="??:??:??";
        }
      if (Buff2.GetLength())
        pStatusBar->UpdateIndicator(1, Buff2.GetBuffer(0), true);
      }
    else
      {//do nothing indicator updated elsewhere
      }

    //switch (ExecStateIndex)
    //  {
    //  case 2: //ProBal solving...
    //    break; //do nothing indicator updated elsewhere
    //  case 1: //Idling...
    //    Buff2.Format("Idling %c", StatusRotate[IdlingCnt++]);
    //    if (IdlingCnt>3)
    //      IdlingCnt=0;
    //    pStatusBar->UpdateIndicator(2, Buff2.GetBuffer(0), true);
    //    break;
    //  case 3: ////Dynamic running...
    //    if (CB.m_bSyncWithClock)
    //      pStatusBar->UpdateIndicator(2, gs_Exec.StateDesc(ExecStateIndex), true);
    //    else
    //      {
    //      if (Progress<0.0)
    //        {
    //        if (CB.m_StepSize>3600.0)
    //          Buff2.Format("StepSize:%.3gh", CB.m_StepSize.Seconds/3600.0);
    //        else
    //          Buff2.Format("StepSize:%.4g", CB.m_StepSize.Seconds);
    //        }
    //      else
    //        Buff2.Format("%.2f%%", Progress*100.0);

    //      pStatusBar->UpdateIndicator(2, Buff2.GetBuffer(0), true);
    //      }
    //    break;
    //  default:
    //    //pStatusBar->UpdateIndicator(2, gs_Exec.StateDesc(ExecStateIndex), true);
    //    break;
    //  }

    if (0 && gs_Exec.Waiting())
      {
      Buff2.Format("Idling %c", StatusRotate[IdlingCnt++]);
      if (IdlingCnt>3)
        IdlingCnt=0;
      pStatusBar->UpdateIndicator(2, Buff2.GetBuffer(0), true);
      }
    else if (gs_Exec.SolvingPB())
      {
      //do nothing indicator updated elsewhere
      }
    else
      {
      if (CB.m_bSyncWithClock)
        pStatusBar->UpdateIndicator(2, gs_Exec.StateDesc(ExecStateIndex), true);
      else
        {
        if (Progress<0.0)
          {
          if (CB.m_StepSize>3600.0)
            Buff2.Format("StepSize:%.3gh", CB.m_StepSize.Seconds/3600.0);
          else
            Buff2.Format("StepSize:%.4g", CB.m_StepSize.Seconds);
          }
        else
          Buff2.Format("%.2f%%", Progress*100.0);

        pStatusBar->UpdateIndicator(2, Buff2.GetBuffer(0), true);
        }
      }

      
    //  case 3: ////Dynamic running...
    //    if (CB.m_bSyncWithClock)
    //      pStatusBar->UpdateIndicator(2, gs_Exec.StateDesc(ExecStateIndex), true);
    //    else
    //      {
    //      if (Progress<0.0)
    //        {
    //        if (CB.m_StepSize>3600.0)
    //          Buff2.Format("StepSize:%.3gh", CB.m_StepSize.Seconds/3600.0);
    //        else
    //          Buff2.Format("StepSize:%.4g", CB.m_StepSize.Seconds);
    //        }
    //      else
    //        Buff2.Format("%.2f%%", Progress*100.0);

    //      pStatusBar->UpdateIndicator(2, Buff2.GetBuffer(0), true);
    //      }
    //    break;
    //  default:
    //    //pStatusBar->UpdateIndicator(2, gs_Exec.StateDesc(ExecStateIndex), true);
    //    break;
    //  }

    pStatusBar->UpdateIndicator(4, gs_Exec.StateDesc(ExecStateIndex), true);
    }
  else
    {
    pStatusBar->UpdateIndicator(1, "", true);
    //pStatusBar->UpdateIndicator(2, gs_Exec.StateDesc(), true);
    pStatusBar->UpdateIndicator(2, "Stopped", true);
    pStatusBar->UpdateIndicator(4, gs_Exec.StateDesc(ExecStateIndex), true);
    //pStatusBar->UpdateIndicator(4, "", true);
    }

  if (!OnlyStatusLine)
    {
    CMdlValueShow::UpdateAll();
    CMdlGraphicShow::UpdateAll();
    gs_AccessWnds.DoDeferredAccess(-1);
    }

#if WITHDRVMAN
  if (gs_pPrj->bDrvOn)
    {
    char Buff5[32];
    Buff5[0]=0;
    if (gs_pDrvMan->NoDrivers()>0)
      {
      for (int i=0; i<gs_pDrvMan->NoDrivers(); i++)
        switch (gs_pDrvMan->DriverState(i))
          {
          case DS_Inactive: strcat(Buff5, "-"); break;
          case DS_Active: strcat(Buff5, "a"); break;
          case DS_Error: strcat(Buff5, "E"); break;
          }
      }
    else
      strcat(Buff5, "NoIO");
    if (StatusBarDriverIndicator>=0)
      pStatusBar->UpdateIndicator(StatusBarDriverIndicator, Buff5, true);
    }
#endif

  return 0;
  }

//---------------------------------------------------------------------------

LRESULT CMainFrame::OnGrfTagGroups(WPARAM wParam, LPARAM lParam)
  {
  switch (wParam)
    {
    case SUB_GTG_BUILD:
      CWindowLists::SetGrfTagGroups(lParam!=0);
      break;
    case SUB_GTG_ACTIVATE:
      SetGrfPageActive((int)lParam, true);
      break;
    case SUB_GTG_DEACTIVATE:
      SetGrfPageActive((int)lParam, false);
      break;
    }
  return 0;
  }
//---------------------------------------------------------------------------

LRESULT CMainFrame::OnNodeAudit(WPARAM wParam, LPARAM lParam)
  {
  char* pTxt = NULL;
  if (lParam)
    pTxt = (char*)lParam;
  if (pTxt)
    {
    CNodeBalanceInfo Balance(pTxt);
    CNodeAuditInfo Audit(pTxt, wParam);
    if (gs_pTheSFELib->FE_GetNodeBalanceInfo(Balance, Audit))
      {
      if (wParam & (SUB_NA_ADT_MASS | SUB_NA_ADT_MASSTOTAL | SUB_NA_ADT_HEAT | SUB_NA_ADT_HEATTOTAL))
        DumpAdtBalance(wParam, NULL, Audit);
      else
        DumpClsBalance((wParam & SUB_NA_CLS_COMPACT)!=0, (wParam & SUB_NA_CLS_SUMMARY)!=0, NULL, Balance);
      }
    delete []pTxt;
    }
  return 0;
  }

//---------------------------------------------------------------------------

LRESULT CMainFrame::OnEditRctDlg(WPARAM wParam, LPARAM lParam)
  {
  LPCTSTR pTag = NULL, pFileName = NULL;
  if (wParam)
    pTag = (LPCTSTR)wParam;
  if (lParam)
    pFileName = (LPCTSTR)lParam;
  if (pTag)
    {
    CRctEditDlg Dlg(pTag, pFileName);
    Dlg.DoModal();
    //CNodeAuditInfo Audit(pTxt, wParam);
    //if (gs_pTheSFELib->FE_GetNodeBalanceInfo(Balance, Audit))
    //  {
    //  if (wParam & (SUB_NA_ADT_MASS | SUB_NA_ADT_MASSTOTAL | SUB_NA_ADT_HEAT | SUB_NA_ADT_HEATTOTAL))
    //    DumpAdtBalance(wParam, NULL, Audit);
    //  else
    //    DumpClsBalance((wParam & SUB_NA_CLS_COMPACT)!=0, (wParam & SUB_NA_CLS_SUMMARY)!=0, NULL, Balance);
    //  }
    delete []pTag;
    delete []pFileName;
    }
  return 0;
  }

////---------------------------------------------------------------------------
//
//LRESULT CMainFrame::OnExecAction(WPARAM wParam, LPARAM lParam)
//  {
//  switch (wParam)
//    {
//    case SUB_EXECACTION_START: OnActionsRun(); break;//gs_TheRunMngr.GotoRunMode(CExecutive::RRM_Current); break;
//    case SUB_EXECACTION_IDLE : OnActionsIdle(); break;//gs_TheRunMngr.GotoIdleMode(); break;
//    case SUB_EXECACTION_STOP : OnActionsEdit(); break;//gs_TheRunMngr.GotoEditMode(); break;
//    case SUB_EXECACTION_STEP : OnActionsStep(); break;
//    default: DoBreak();
//    }
//  return 0;
//  }
//
//---------------------------------------------------------------------------

void CMainFrame::OnActionsProbalSetup()
  {
#if dbgMainframe
  if (dbgActions())
    dbgpln("------ACT------> OnActionsProbalsetup");
#endif

  if (gs_Exec.Initialised() && gs_License.ProBalOK())
    {
    CMsgWindow::SetMsgsIncludeFlag(false);
    CSolverCfgSheet SolverCfg("ProBal Solver", this, CSolverCfgSheet::iCurrentPage);
    if ((SolverCfg.DoModal()==IDOK))
      {
      gs_pPrj->CheckLicense(TRUE);
      LogSeparator("Set ProBal Mode",0);
      gs_TheRunMngr.SetProbalMode();
      SetToolBarSolveMode();
      }
    }
  }

void CMainFrame::OnUpdateActionsProbalSetup(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(XDoingPB() && (gs_License.AllowProBal() || gs_License.AllowProBalLite()) && EnableNotBusy() && EnableNotAnalysing() && EnableNotStopped());
  }

//---------------------------------------------------------------------------

void CMainFrame::OnActionsDynamicSetup()
  {
#if dbgMainframe
  if (dbgActions())
    dbgpln("------ACT------> OnActionsDynamicsetup");
#endif
  if (gs_License.DynamicOK() && EnableNotBusy() && EnableNotAnalysing() && EnableNotStopped())
    {
    CMsgWindow::SetMsgsIncludeFlag(false);
    CSolverCfgSheet SolverCfg("Dynamic", NULL, CSolverCfgSheet::iCurrentPage);
    if (SolverCfg.DoModal()==IDOK)
      {
      LogSeparator("Set Dynamic Mode",0);
      gs_pPrj->CheckLicense(TRUE);
      gs_TheRunMngr.SetDynamicMode();
      SetToolBarSolveMode();
      }
    }
  }

void CMainFrame::OnUpdateActionsDynamicSetup(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(!XDoingPB() && gs_License.DynamicOK() && EnableNotBusy() && EnableNotAnalysing() && EnableNotStopped());
  }

//---------------------------------------------------------------------------

void CMainFrame::OnActionsSetup()
  {
  int IsDynMode=DefNetDynamicMode();
  if (IsDynMode)
    OnActionsDynamicSetup();
  else
    OnActionsProbalSetup();
  }

void CMainFrame::OnUpdateActionsSetup(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(EnableNotBusy() && EnableNotAnalysing() && EnableNotStopped());
  }

//---------------------------------------------------------------------------

void CMainFrame::OnActionsEdit()
  {
#if dbgMainframe
  if (dbgActions())
    dbgpln("------ACT------> OnActionsEdit");
#endif
  gs_TheRunMngr.GotoEditMode();
  }

void CMainFrame::OnUpdateActionsEdit(CCmdUI* pCmdUI)
  {
  //pCmdUI->SetCheck(gs_TheRunMngr.Stopped() ? 1 : 0);
  pCmdUI->Enable(EnableNotStopped() && !gs_TheRunMngr.Stopped() && !gs_License.Blocked());
  }

void CMainFrame::OnActionsEditTgl()
  {
#if dbgMainframe
  if (dbgActions())
    dbgpln("------ACT------> OnActionsEditTgl");
#endif
  if (gs_TheRunMngr.Stopped())
    gs_TheRunMngr.GotoIdleMode();
  else
    gs_TheRunMngr.GotoEditMode();
  }

void CMainFrame::OnUpdateActionsEditTgl(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(true);
  pCmdUI->SetCheck(gs_TheRunMngr.Stopped() ? 1 : 0);
  pCmdUI->Enable(EnableNotStopped() && (gs_TheRunMngr.RunningDsp()||gs_TheRunMngr.IdlingDsp()||gs_TheRunMngr.Stopped()) && gs_TheRunMngr.m_ComUIActive && !gs_License.Blocked());
  }

//---------------------------------------------------------------------------

void CMainFrame::OnActionsRun()
  {
#if dbgMainframe
  if (dbgActions())
    dbgpln("------ACT------> OnActionsRun");
#endif
  
  gs_TheRunMngr.GotoRunMode(CExecutive::RRM_Current, -1);
  }

void CMainFrame::OnUpdateActionsRun(CCmdUI* pCmdUI)
  {
  //pCmdUI->SetCheck(gs_TheRunMngr.RunningDsp() ? 1 : 0);
  pCmdUI->Enable(EnableNotStopped() && !gs_TheRunMngr.RunningDsp() && !gs_License.Blocked());
  }

//---------------------------------------------------------------------------

void CMainFrame::OnActionsRunSteady()
  {
#if dbgMainframe
  if (dbgActions())
    dbgpln("------ACT------> OnActionsRun");
#endif
  gs_pPrj->CheckLicense(TRUE);
  gs_TheRunMngr.GotoRunMode(CExecutive::RRM_Current, 1);
  }

void CMainFrame::OnUpdateActionsRunSteady(CCmdUI* pCmdUI)
  {
  //pCmdUI->SetCheck(gs_TheRunMngr.RunningDsp() ? 1 : 0);
  pCmdUI->Enable(EnableNotStopped() && !gs_TheRunMngr.RunningDsp() && !gs_License.Blocked());
  }

//---------------------------------------------------------------------------

void CMainFrame::OnActionsRunPB()
  {
#if dbgMainframe
  if (dbgActions())
    dbgpln("------ACT------> OnActionsRunPB");
#endif
  CMsgWindow::SetMsgsIncludeFlag(false);
  //LogSeparator("Set ProBal Mode", 0);
  gs_pPrj->CheckLicense(TRUE);
  gs_TheRunMngr.GotoRunMode(CExecutive::RRM_PBMd, -1);
  }

void CMainFrame::OnUpdateActionsRunPB(CCmdUI* pCmdUI)
  {
  //pCmdUI->SetCheck(gs_TheRunMngr.RunningDsp() ? 1 : 0);
  pCmdUI->Enable(EnableNotStopped() && XDoingPB() && !gs_TheRunMngr.RunningDsp() && !gs_License.Blocked());
  }

void CMainFrame::OnActionsRunDyn()
  {
#if dbgMainframe
  if (dbgActions())
    dbgpln("------ACT------> OnActionsRunDyn");
#endif
  CMsgWindow::SetMsgsIncludeFlag(false);
  //LogSeparator("Set Dynamic Mode",0);
  gs_pPrj->CheckLicense(TRUE);
  gs_TheRunMngr.GotoRunMode(CExecutive::RRM_DynMd, -1);
  }

void CMainFrame::OnUpdateActionsRunDyn(CCmdUI* pCmdUI)
  {
  //pCmdUI->SetCheck(gs_TheRunMngr.RunningDsp() ? 1 : 0);
  pCmdUI->Enable(EnableNotStopped() && !XDoingPB() && !gs_TheRunMngr.RunningDsp() && !gs_License.Blocked());
  }

void CMainFrame::OnActionsRunTgl()
  {
#if dbgMainframe
  if (dbgActions())
    dbgpln("------ACT------> OnActionsRunTgl");
#endif
  if (gs_TheRunMngr.IdlingDsp() || gs_TheRunMngr.Stopped())
    gs_TheRunMngr.GotoRunMode(CExecutive::RRM_Current, gs_TheRunMngr.Stopped() ? -1:1);
  else
    gs_TheRunMngr.GotoIdleMode();
  }

void CMainFrame::OnUpdateActionsRunTgl(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(true);
  pCmdUI->SetCheck(gs_TheRunMngr.RunningDsp() ? 1 : 0);
  pCmdUI->Enable(EnableNotStopped() && (gs_TheRunMngr.RunningDsp()||gs_TheRunMngr.IdlingDsp()||gs_TheRunMngr.Stopped()) && gs_TheRunMngr.m_ComUIActive && !gs_License.Blocked());
  }
void CMainFrame::OnUpdateActionsRunPBTgl(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(true);
  pCmdUI->SetCheck(gs_TheRunMngr.RunningDsp() ? 1 : 0);
  pCmdUI->Enable(EnableNotStopped() && XDoingPB() && (gs_TheRunMngr.RunningDsp()||gs_TheRunMngr.IdlingDsp()||gs_TheRunMngr.Stopped()) && gs_TheRunMngr.m_ComUIActive && !gs_License.Blocked());
  }
void CMainFrame::OnUpdateActionsRunDynTgl(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(true);
  pCmdUI->SetCheck(gs_TheRunMngr.RunningDsp() ? 1 : 0);
  pCmdUI->Enable(EnableNotStopped() && !XDoingPB() && (gs_TheRunMngr.RunningDsp()||gs_TheRunMngr.IdlingDsp()||gs_TheRunMngr.Stopped()) && gs_TheRunMngr.m_ComUIActive && !gs_License.Blocked());
  }

//---------------------------------------------------------------------------

void CMainFrame::OnActionsIdle()
  {
#if dbgMainframe
  if (dbgActions())
    dbgpln("------ACT------> OnActionsIdle");
#endif

  gs_TheRunMngr.GotoIdleMode();
  }

void CMainFrame::OnUpdateActionsIdle(CCmdUI* pCmdUI)
  {
  //pCmdUI->SetCheck(gs_TheRunMngr.IdlingDsp() ? 1 : 0);
  pCmdUI->Enable(EnableNotStopped() && !gs_TheRunMngr.IdlingDsp() && !gs_License.Blocked());
  }

//---------------------------------------------------------------------------

void CMainFrame::OnActionsStep()
  {
#if dbgMainframe
  if (dbgActions())
    dbgpln("------ACT------> OnActionsStep");
#endif

// CNM
// if (gs_TheRunMngr.Idling() || (/*!XDoingPB() &&*/ gs_TheRunMngr.Stopped()))
// DO the tests in the Enable.
// If a command comes from the COM I/F then it must execute
  CDoOneStepInfo Info;
  Info.lParam=0;
  gs_TheRunMngr.DoStep(Info);
  }

void CMainFrame::OnUpdateActionsStep(CCmdUI* pCmdUI)
  {
  // CNM
  pCmdUI->Enable(EnableNotStopped() && (gs_TheRunMngr.IdlingDsp() || (/*!XDoingPB() &&*/ gs_TheRunMngr.Stopped())) && !gs_License.Blocked());
  }

//---------------------------------------------------------------------------

void CMainFrame::OnActionsSolveNet()
  {
#if dbgMainframe
  if (dbgActions())
    dbgpln("------ACT------> OnActionsSolveNet");
#endif
  }

void CMainFrame::OnUpdateActionsSolveNet(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(FALSE && gs_TheRunMngr.IdlingDsp() && !gs_License.Blocked());
  }

//---------------------------------------------------------------------------

void CMainFrame::OnProjectEditColours()
  {
  gs_pPrj->SetShowEditStatus(gs_pPrj->ShowEditStatus()?0:1); // Toggle It

  CWindowLists WL;
  const int WndCount = WL.BuildSingleList();

  for (int i=1; i<WndCount; i++)
    {
    if (WL.Wnds[i].pWnd->IsKindOf(RUNTIME_CLASS(CGrfFrameWnd)))
      WL.Wnds[i].pWnd->Invalidate();
    }

  AfxGetMainWnd()->RedrawWindow();
  }

void CMainFrame::OnUpdateProjectEditColours(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(true);
  pCmdUI->SetCheck(gs_pPrj->ShowEditStatus()?1:0);
    
  //pCmdUI->Enable(EnableNotStopped() && (gs_TheRunMngr.RunningDsp()||gs_TheRunMngr.IdlingDsp()||gs_TheRunMngr.Stopped()) && gs_TheRunMngr.m_ComUIActive && !gs_License.Blocked());
  }

//---------------------------------------------------------------------------

#define WithNewImportExport 0
#if WithNewImportExport
#include "grfdoc.h"
#endif

void CMainFrame::OnExportDB()
  {
#if WithNewImportExport
  CNeutralExportDBDlg Dlg;
  if (Dlg.DoModal()==IDOK)
    {
    CDocTemplate & GT = ScdApp()->GraphTemplate();
    CNeutralGrfImportExport EGH;
_asm int 3;
//EGH.DoExport(GT, (char*)(const char*)Dlg.m_Folder, (char*)(const char*)Dlg.m_GrfDatabase);
    }
#endif
  }

void CMainFrame::OnUpdateExportDB(CCmdUI* pCmdUI)
  {
#if WithNewImportExport
  pCmdUI->Enable(true);//!gs_License.Blocked());
#else
  pCmdUI->Enable(false);
#endif
  }

//---------------------------------------------------------------------------

void CMainFrame::OnImportDB()
  {
#if WithNewImportExport
  CNeutralImportDBDlg Dlg;
  if (Dlg.DoModal()==IDOK)
    {
    CDocTemplate & GT = ScdApp()->GraphTemplate();
    CNeutralGrfImportExport EGH;
    EGH.DoImportDB(GT, Dlg.m_sGrfDatabase);
    }
#endif
  }

void CMainFrame::OnUpdateImportDB(CCmdUI* pCmdUI)
  {
#if WithNewImportExport
  pCmdUI->Enable(!gs_License.Blocked());
#else
  pCmdUI->Enable(false);
#endif
  }

//===========================================================================

LRESULT CMainFrame::OnException(WPARAM wParam, LPARAM lParam)
  {
  Strng *pS=(Strng*)wParam;
  bool  *pDone=(bool*)lParam;

  CopyTextToClipboard(this, pS->Str());

  Strng S;
  S= *pS;
  S+="\n\nThis information has been copied to the clipboard\n"; 


  AfxMessageBox(S(), MB_OK);
  *pDone=true;
  AfxGetApp()->PostThreadMessage(WM_QUIT, 0, 0);
  return 0;
  }

//===========================================================================

LRESULT CMainFrame::OnSetLicense(WPARAM wParam, LPARAM lParam)
  {
  gs_License.SetForMineServeMsg(wParam, lParam);
  return 0;
  }

//===========================================================================

LRESULT CMainFrame::OnChkLicense(WPARAM wParam, LPARAM lParam)
  {
  gs_License.DoCheckLicense(wParam, lParam);
  
  ScdMainWnd()->PostMessage(WMU_UPDATEMAINWND, SUB_UPDMAIN_BACKGROUND, 0);
  return 0;
  }

//===========================================================================


void CMainFrame::OnScd10Export()
  {
#if SYSCAD10
  gs_pPrj->Export2Scd10();
#endif
  }

void CMainFrame::OnUpdateScd10Export(CCmdUI *pCmdUI)
  {
#if SYSCAD10
  pCmdUI->Enable(true);//EnablePrjOK() && !EnableNotStopped());
#else
  pCmdUI->Enable(false);
#endif
  }
