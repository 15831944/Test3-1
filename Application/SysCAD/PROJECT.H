//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#ifndef __PROJECT_H
#define __PROJECT_H

#ifndef __SCDVER_H
#include "scdver.h"
#endif

#ifndef __GPWFUNCS_H
#include "gpwfuncs.h"
#endif
#ifndef __EXECUTIV_H
#include "executiv.h"
#endif
#ifndef __SFE_CLNT_H
#include "sfe_clnt.h"
#endif
#ifndef __COMMSSRVRBASE_H
#include "CommsSrvrBase.h"
#endif
#ifndef __ACCNODE_H
#include "accnode.h"
#endif

#include "NETServer.h"
#include "hstmain.h"

//#include ".\opcsrvrwrapper.h"

//#ifdef __PROJECT_CPP
//  #define DllImportExport DllExport
//#elif !defined(SCAPP1)
//  #define DllImportExport DllImport
//#else
  #define DllImportExport
//#endif

//=========================================================================

//trick MFC code to force it to create property sheet buttons
#if _MSC_VER==1100
#define PROPSHEETTRICK0() m_bModeless=0;
#define PROPSHEETTRICK1() m_bModeless=1;
#elif _MSC_VER<1300
#define PROPSHEETTRICK0() ((CPropertySheet*)this)->GetPropSheetHeader()->dwFlags |= PSH_WIZARD;
#define PROPSHEETTRICK1() ((CPropertySheet*)this)->GetPropSheetHeader()->dwFlags &= ~PSH_WIZARD;
#elif _MSC_VER>=1300
#define PROPSHEETTRICK0() ((CPropertySheet*)this)->m_psh.dwFlags |= PSH_WIZARD;
#define PROPSHEETTRICK1() ((CPropertySheet*)this)->m_psh.dwFlags &= ~PSH_WIZARD;
#endif

//=========================================================================

extern DllImportExport int gs_FileNewFlag;
extern DllImportExport int gs_FileOpenFlag;
extern DllImportExport int gs_BlockSizeAdjustment;
extern DllImportExport int gs_ProjectOpenFlag;

//=========================================================================

const bool AllowHistStr = true;

//typedef CWnd* pCWnd;
class CGrfDoc;

class DllImportExport CTopWindowInfo
  {
  public:
    CTopWindowInfo()
      { 
      pWnd = NULL; 
      iType = 0; 
      iZOrd = -1; 
      };
    CTopWindowInfo(CWnd* w, byte Type)
      { 
      pWnd = w; 
      iType = Type; 
      pWnd->GetWindowText(m_sName);
      Strng S(m_sName);
      S.FnName();
      m_sId=S();
      iZOrd = -1; 
      };
    CTopWindowInfo(CWnd* w, byte Type, const char* pName, int ZOrd=-1)
      { 
      pWnd = w; 
      iType = Type; 
      if (pName)
        m_sName = pName; 
      else
        pWnd->GetWindowText(m_sName);
      if (m_sName.GetLength()==0)
        m_sName="Unknown";
      Strng S(m_sName);
      S.FnName();
      m_sId=S();
      iZOrd = ZOrd; 
      };

  public:
    CWnd*    pWnd;   //pointer to window
    byte     iType;  //0=other; 1=graphics; 2=trend; 3=main SysCAD window
    CString  m_sName;  //window title
    CString  m_sId;    //Id Name with out extention without 
    int      iZOrd;  //Sequence in Z-Order: -1 = none, 0 = topmost

  };

class DllImportExport CWindowLists
  {
  public:
    CWnd*   pMainWnd;   //CMainFrame<--CMDIFrameWnd
    CWnd*   pAccessWnd[NAccessWnds]; //CFrm<--CMDIChildWnd
    CWnd*   pMsgWnd;    //CMsgFrm<--CMDIChildWnd
    CWnd*   pOrdWnd;    //COrdFrm<--CMDIChildWnd
    CWnd*   pStatsWnd;  //COrdFrm<--CMDIChildWnd
    CWnd*   pCmdWnd;    //CFrm<--CMDIChildWnd
#if WITHSFEWND 
    CWnd*   pSFEWnd;    //CEngStateFrm<--CMDIChildWnd
#endif
    CWnd*   pAutWnd;    //
    CWnd*   pPrjWnd;    //CPrjFrameWnd<--CMDIChildWnd
    CArray<CWnd*, CWnd*> TrndWnds; //CTagVwSplit<--CMDIChildWnd
    CArray<CWnd*, CWnd*> GrfWnds;  //CGrfFrameWnd<--CMDIChildWnd
    CArray<CTopWindowInfo, CTopWindowInfo&> Wnds; //list of ALL windows
    static char* MainWndTitle;
    //static char* AccessWndTitle;
    static LPCTSTR AccessWndTitle(long i);
    static BOOL WndMaxMode;

    CWindowLists();
    void Reset();
    void BuildLists();
    int BuildSingleList(BOOL Sorted=false);
    int BuildZOrder(BOOL Sorted=false);
    int Find(byte Type, char* pTitle);
    int Find(char* pTitle);
    int Find(CWnd* pFindWnd);
    static CFrameWnd* GetCurrentTopWindow();
    static CDocument* GetCurrentTopWindowDoc();
    static void ActivateTopWindowCmds();
    static CStatusBar* GetStatusBar();
    static CWnd* GetMDIClientWnd();
    static int GetTrendWndCount();
    static int GetGrfWndCount();
    static CDocument* GetGrfWndByName(char* pTitle);
    static CDocument* GetGrfWndByIndex(int index);
    static void SetWndMaxMode(BOOL WndMaxMode_) { WndMaxMode = WndMaxMode_; };
    static BOOL GetWndMaxMode() { return WndMaxMode; };
    
    static void DetermineActiveGraphics();
    static void SetGrfTagGroups(bool Force);
  };

//=========================================================================

//list off "ExecObj File Flags"...
const DWORD EFF_CanEditFile       = 0x00000001;
const DWORD EFF_OtherValid        = 0x00000002;
const DWORD EFF_GrpValid          = 0x00000004;
const DWORD EFF_ExcelValid        = 0x00000008;
const DWORD EFF_Grp_Models        = 0x00000100;
const DWORD EFF_Grp_Referenced    = 0x00000200;
const DWORD EFF_Grp_SpecSheets    = 0x00000400;
const DWORD EFF_Grp_Configuration = 0x00000800;
const DWORD EFF_Grp_Reports       = 0x00010000;
const DWORD EFF_Grp_CmdScripts    = 0x00020000;
const DWORD EFF_ForceCopy         = 0x00040000;
const DWORD EFF_PreventCopy       = 0x00080000;

_FWDDEF(CExecObjFileInfo)
class DllImportExport CExecObjFileInfo
  {
  public:
    CExecObj*   pExecObj;      //executive object using file
    DWORD       dwFlags;       //flags
    Strng       sFilename;     //full filename
    Strng       sOther;        //extra data
    flag        bForceCopy:1,
                bPreventCopy:1,
                bExists:1;

    CExecObjFileInfo()
      { pExecObj = NULL; dwFlags = 0; bForceCopy=bPreventCopy=bExists=false; };
    CExecObjFileInfo(CExecObj* pExec, DWORD Flags, char* pFilename, flag ForceCopy, flag PreventCopy)
      { pExecObj = pExec; dwFlags = Flags; sFilename = pFilename; bForceCopy=ForceCopy; bPreventCopy=PreventCopy; bExists=false; };
    CExecObjFileInfo(CExecObj* pExec, DWORD Flags, char* pFilename, char* pOther, flag ForceCopy, flag PreventCopy)
      { pExecObj = pExec; dwFlags = Flags; sFilename = pFilename; sOther = pOther; bForceCopy=ForceCopy;bPreventCopy=PreventCopy;bExists=false; };
    CExecObjFileInfo& operator=(const CExecObjFileInfo &FI)
      { pExecObj = FI.pExecObj; dwFlags = FI.dwFlags; sFilename = FI.sFilename; sOther = FI.sOther; bForceCopy=FI.bForceCopy; bPreventCopy=FI.bPreventCopy; bExists=FI.bExists; return *this; };
  };

_FWDDEF(CPrjFilesInfo)
class CPrjFilesInfo
  {
  public:
    Strng   sExecName;    //name of executive object type using files
    CArray<CExecObjFileInfo, CExecObjFileInfo&> Files; //list of ALL files used by this exec object

    CPrjFilesInfo() { };
    CPrjFilesInfo(char* pExecName) { sExecName = pExecName; };
    //~CPrjFilesInfo() {  };
  };

class DllImportExport CPrjFilesLists
  {
  public:
    //CArray<CPrjFilesInfo, CPrjFilesInfo&> ExecFiles; //list of ALL files
    CPrjFilesInfo ** pExecFiles; //list of ALL files
    CStringArray OtherFiles[7];
  protected:
    int iAllocSize;
    int iSize;

  public:
    CPrjFilesLists();
    ~CPrjFilesLists();
    void Reset();
    int Add(CPrjFilesInfo* pFI);
    void BuildLists();
    int Find(char* pName);
    int FindFile(char* pFileName);
    void CheckExists();
    void CheckUnknown();
    static char* OtherExts(int index);
    static char* OtherExtsEx(int index);
    inline int GetSize() { return iSize; };
  };

//=========================================================================

#if WITHSCRCYCLES
#define ID_SCREENCYCLE_TIMER   124

class DllImportExport CScreenSaverInfo
  {
  public:
    WORD         iWaitTime;
    CString      sDescription;
    CStringArray Windows;

    CScreenSaverInfo();
    ~CScreenSaverInfo();
    CScreenSaverInfo&  operator=(const CScreenSaverInfo& S);
    int FindWindow(CString& s);
  };
#endif

//===========================================================================

class CDdeReportInfo
  {
  public:
    int      iHSearchLen;  //max number of horizontal spreadsheet cells to search for key
    int      iVSearchLen;  //max number of vertical spreadsheet cells to search for key
    Strng    sNan;         //default string to use for NAN
    Strng    sBlank;       //default string to use for invalid tag
    Strng    sIgnoreChars; //ignore cells starting with any of these characters

    CDdeReportInfo() { iHSearchLen = 10; iVSearchLen = 100; sNan = "*"; sBlank = ""; sIgnoreChars = ""; };
  };

//===========================================================================

class FindTrendTagHelper
  {
  public:
    FindTrendTagHelper()
      {
      iLastPos = 0;
      iFoundPos = -1;
      DocPos = NULL;
      iFoundCnt = 0;
      };
    FindTrendTagHelper(LPCSTR pTag_, POSITION Pos)
      {
      sTag      = pTag_;
      iLastPos  = 0;
      iFoundPos = -1;
      DocPos    = Pos;
      iFoundCnt = 0;
      };

  public:
    Strng     sTag;
    int       iLastPos;
    int       iFoundPos;
    POSITION  DocPos;
    int       iFoundCnt;
    Strng     sLineTag;
  };

//===========================================================================

class CSFPInfo
  {
  public:
    //Strng SrcPrj;
    Strng SrcPrjRel;
    Strng SrcFull;
    Strng DstFull;
    Strng DstStgRel;
    
    flag PrjScope;
    flag PrjRCScope;
    flag SrcExists;
    flag CopyReqd;

    CExecObjFileInfo * pFI;
    
    void SwingFilePath(flag DoingSaveAs, LPCTSTR Fn);
  };

//===========================================================================

class CUniqueTagRulesBlk
  {
  protected:
    Strng m_MdlName;
    Strng m_Base;
    char  m_SepChar;
  public:

    CUniqueTagRulesBlk(LPCTSTR pMdlName, LPCTSTR pBase, char SepChar='_');

    LPTSTR BaseTag() { return m_Base(); }
    char   SepChar() { return m_SepChar; }

    void   SetBaseTag(LPTSTR B) { m_Base=B; }
    void   SetSepChar(char C) { m_SepChar=C; }
  };

//===========================================================================

class ActiveTagInfoMap : public CMap <char*, char*, pStrng, pStrng> {}; 

class ActiveTagInfo
  {
  public:
    Strng Name;
    Strng Missing;
    Strng_List TagList;
    int NTags;
    int NActive;
    int NTagsI;
    int NActiveI;
    int NMissing;
    int NFound;
    flag fIsDLL;

  public:
    ActiveTagInfo();
    void GetTags();
    void RebuildTagMap(ActiveTagInfoMap& TagMap);
    int SetActive(flag Active);
  };

//===========================================================================

class CDDESrvrHelper
  {
  protected:
    HINSTANCE      hDDELib;    //handle for DLL
    byte           iDDEUseCnt; //counter for DLL use
    CCommsServerManagerBase* pMngr; //pointer to the one and only global DDE Server Manager

  public:
    CDDESrvrHelper();
    ~CDDESrvrHelper();
    bool           OpenSrvr();
    void           CloseSrvr();
    void           Options();
    void           CloseOptions();
    CCommsServerManagerBase* DDESrvrMngr() { return pMngr; };
  };

//===========================================================================

enum SnapShotActions {SSA_Prompt, SSA_AutoInc, SSA_ReUse};
enum PrjLoadTypes    {PLT_Null, PLT_Normal, PLT_Local};

const BOOL WithVersionDefault=true;
const BOOL RemoveOldDefault=false;

// Window Behaviour
const int WB_None           = 0x00;
const int WB_MoveTogether   = 0x01;
const int WB_SizeTogether   = 0x02;
const int WB_Together       = WB_MoveTogether|WB_SizeTogether;
const int WB_Coincident     = 0x04;

class CProject; // forward 
//_FWDDEF(CProject)
class DllImportExport CProjectSettings
  {
  public:

    CProjectSettings(CHistSettings & Hst);
    ~CProjectSettings();

    void          ClearSettings();

    flag          ReadSettings(CProfINIFile & PF, int PrjFileVerNo);
    flag          WriteSettings(CProfINIFile & PF, bool FullSave);

    flag          EditSettings(LPCTSTR Title, CProject * pPrj);

  public:

    long          m_NetMode;

    long          m_PBNodeMode;
    long          m_PBLinkMode;
    long          m_PBHeatMode;
    long          m_PBFlowMode;

    long          m_DynNodeMode;
    long          m_DynLinkMode;
    long          m_DynHeatMode;
    long          m_DynFlowMode;

    long          m_MaxNodeMode;
    long          m_MaxLinkMode;
    long          m_MaxHeatMode;
    long          m_MaxFlowMode;

#if WITHDRVMAN
    flag          m_bDrvOn,
                  m_bDrvLclTagSrvrOK,
                  m_bDrvReadAll,
                  //bDrvChanged,
                  m_bDrvIgnoresOn,
                  m_bDrvForcesOn,
                  m_bDrvNoiseOn;
#endif
    flag          m_bDDEOn,               //SysCAD DDE Server on
                  m_bOPCOn,             //SysCAD OPC Server on
                  m_bOPCResetReg,       //Reset SysCAD OPC Register settings next time OPC started
                  m_bGrfDeferLoad,        //should the loading of the graphics dxf files be delayed
                  m_bGrfAutoLoad,         //if drawing not loaded, load it without prompt when window is accessed
                  m_bGrfUpdateAnnot,      //after a run/solve update all annotation blocks
                  m_bGrfAnimationOn,      //update grafics animation while running/solving
                  m_bGrfUpdateAnimat,     //after a run/solve update all animation (fills and bars)
                  m_bGrfPromptForZoom,    //pan graphics when finding tag
                  m_bGrfMoveCursor,       //move cursor when finding tag
                  m_bGrfRegExpOn,         // Test Using Regular Expression 
                  m_bRptExcelLock,        //OLE report option
                  m_bRptExcelCellName,    //OLE report option
                  m_bRptExcelMakeActive,  //OLE report option
                  m_bRptExcelSysCADActive,//OLE report option
                  m_bRptExcelUpdateLinks, //OLE report option
                  m_bRptExcelSaveOnComplete,//OLE report option
                  m_bRptSyscadTags,       //OLE report option
                  m_bReadOnlyPrj,         //can the project be saved on itself
                  m_bAccAutoSaveOn,       //Indicates Access Window autosave state
                  m_bAccUpdateOnRun,      //Indicates Access Window to update on run transitions
                  m_bAccHistoryInfoVis,   //Indicates historian recorded tags are shown in Access Window
                  m_bSSDoZip,             //Autozip snapshots/scenarios
                  m_bArcOn,               //Archiver On
                  m_bArcOpenDBOnRun,     // Delay open Arc DB until run
                  m_bIOMOn,
                  m_bRunOptEmptyAll,           // RunStartupOptions
                  m_bRunOptZeroFlows,    
                  m_bRunOptInitialiseAll,
                  m_bRunOptEmptySpills,
                  m_bRunOptResetStats,   
                  m_bRunOptOnceOnly;    

    int           m_GrfBehaviour,
                  m_TrndBehaviour;

#if WITHDRVMAN
    byte          bDrvStartFBKCheck;
#endif

    Strng          m_sDefGrpLib;
    Strng          m_sGrfFrameName;
    Strng          m_sGrfFrameFilter;
    
    SnapShotActions m_eSSAction;         //HowTo Handle a SnapShot save 
    Strng          m_sSSName;            //Current Name of SnapShot
    Strng          m_sSSPrefix;          //5 letter SnapShot prefix for AutoInc 
    UINT           m_iSSNumber;          //SnapShot number for AutoInc 
    
    CDdeReportInfo m_ReportInfo;         //DDE report options

#if WITHSCRCYCLES
    int            iScrCycles;         //number of screen cycles in SysCAD screen saver
    CScreenSaverInfo* Cycles;          //Screen saver cycle information
    int            iCycleNo;           //index no of current window cycle, -1 = none
    UINT           iMainShowCmd;       //flags for reseting main window after screen save
    int            iCycleEndCnt;       //count of number of messages to end screen saver have been recieved
    CWnd**         CycleWnds;          //array of all windows
    WINDOWPLACEMENT* CyclePos;         //array of all windows initial positions
    int            iCycleWndCnt;       //number of windows stored in arrays
    CWnd*          pOnlyWnd;
    CTxtWnd*       pCycleDescWnd;      //Title window for cycle
    flag           BarOn[8];           //array remembering state of toolbars and status bar
    flag           bSystemScreenSave,  //True if system screen save must be activated
                   bSyscadScreenSave;  //True if SysCAD screen save must be activated
#endif
    flag             m_bToolTips,          //True if tool tips must be shown
                     m_bFlyBys,            //True if flybys must be shown
                     m_bDefaultRecordIt,   //True if tags added to trend window must be recorded
                     m_bFlashTrend,        //True if trend must flash when cursor over trend line
                     m_bShowNewTrndLineDlg;//True if tag trending options dialog must be shown for a new trended tag
                   
    //flag             bLoadBusy,          //Flag Set to indicate that a Project Load is in Progress
    flag             m_bConfigBusy;        //Flag set to indicate project config window open
                   
    Strng            m_sIOMarshalName;     //IOMarshal name
    Strng            m_sIOMarshalNode;     //IOMarshal Node - IP Address 
    Strng            m_sDrvManagerName;    //driver manager name
    Strng            m_sArcManagerName;    //Archive manager name
    long             m_lStatsRefreshCnt;   //number of iterations after which stats must be refreshed

    //DWORD            m_dwArcManagerOptions;    //Archive manager name

    CHistSettings    & m_Hst;

    struct  tSolverSettings 
      {
      flag         m_fStateWnd;        // Status display window is required
      flag         m_fStatePinned;     // Status window Pinned
      flag         m_fStateErrors;     // Status window shows errors
      int          m_iStateX;          // Status window X Pos
      int          m_iStateY;          // Status window Y Pos
      }            m_Solver;

    flag             m_fNewTrend, 
                     m_fNewGraph;      // New Project Options
                   
    flag             m_bRemoveRevs;
    flag             m_bCopyDBFiles;
                   
    int              m_iOPCServerNo;       // CNM should maybe go
                   
    dword            m_dwMdlStatusOptions;

    CTimeValue       m_TheTime;
    CTimeValue       m_StepSizeMax;
    CTimeValue       m_ScnStopTime;
    CTimeValue       m_ScnDuration;

    long             m_RealTime;
    double           m_RealTimeMult;
    long             m_SyncWithClock;
    eScnTypes        m_ScnType;
    eScnTermActions  m_ScnTermAction;
    long             m_StepCountMax;

    flag             m_DisplayTagsOnly,
                     m_HideZeros,
                     m_ShowGroups;
    
    flag             m_NumericTagsBad,
                     m_NumericStartingTagsBad;

    char             m_NonNumericTagChr;

    CTimeValue       m_GTBStartTime;
    CTimeValue       m_GTBEndTime;
    flag             m_GTBTrackingTime;

    long             m_SpModelFixupStrategy;

  };

//===========================================================================
// FindTagOptions
const dword FTO_Null             = 0x00000000;
const dword FTO_DoAccess         = 0x00000001;
const dword FTO_MoveCursor       = 0x00000002;
const dword FTO_Highlite         = 0x00000004;
const dword FTO_NoErrDlg         = 0x00000008;
const dword FTO_FindNext         = 0x00000010;

//_FWDDEF(CProject)
class DllImportExport CProject : public CExecObj, public CProjectSettings
  {
  public:
    static bool   sm_fOpenAltCfg;
    static bool   sm_SysCADInited;

  public:
    flag          bHstOK,
                  bCommsOpen,
                  bTimeChanged,
                  bHstChanged,
                  bDocChanged,
                  bDrvOpen,
                  bDrvChanged,
                  bNetOpen,
                  bNetChanged,
                  bForceSaveAsNV,       //set to True to force the next save to be save as New Version
                  bDoingLoad,           //set to True if currently loading
                  bDoingSaveAs,         //set to True if the current save is a project save as
                  bDoingSaveAsWithName, //set to True if the current saveas must use name specd
                  bDoingSaveAsNV,       //set to True if the current save is a project save as New Version
                  bChangedGrfMenu,      //has the graphics menu been altered
                  bChangedRuntimeMenu,  //has the menu been altered for run-time license
                  bDoGrfAutoAccess,     //temp flag used when finding tag in graphics
                  bBusyAnalysing,       //Indicates an analysis is busy
                  bArcOpen,
                  bArcChanged,
                  bIOMOpen,
                  bIOMChanged;
    
    Strng          m_sPrevPrjFile;        //previously used project file, for project save as
    Strng          m_sPrevPrjFiles;       //previously used project files, for project save as

    static PrjLoadTypes sm_LoadTypeRqst; // Used to remeber what type of load is reqd
    flag           bCmdSuccess;        //was last new,open,save,close command successful

    CDocument*     pPrjDoc;            //pointer to one-and-only-one project document
    flag           bLoadBusy;           //Flag Set to indicate that a Project Load is in Progress

    CStringArray   m_MdlHelpFileList;    //list of model help files, the first in list is used for context sensitive help

    Strng          m_LastSnapLoadFn;       // To Test for Prompted Filename when saving

    Strng          m_OldPrjName;        // Name when last loaded
    Strng          m_OldPrjFiles;       // Path when last loaded

    Strng          m_RmtCpyFolder;
    CFile          m_RmtCpyLock;

    struct tPrjSave
      {
      flag m_bRqstNew;
      flag m_bIsNewPrj;
      }            m_Save;

    BOOL           m_DlgCopyHist;
    BOOL           m_DlgCopy[6];

    CDDESrvrHelper*pDDESrvr;           

    CString        m_sNewPrjCfg;
    CString        m_sNewPrjPath;
    BOOL           m_bWithVersion;
    BOOL           m_bRemoveOld;

    bool           ShowStatus();
    void           SetShowStatus(bool On);
    bool           ShowEditStatus();
    void           SetShowEditStatus(bool On);

  protected:
    FindTrendTagHelper TrndPos;

  public:
    SFEFlwLib      m_FlwLib, *m_pFlwLib;
    SFEMdlLibArray MdlLibs;

#if SYSCAD10
    CNETServer   * m_pCLRSrvr;
#endif
  public:

    CProject();
    ~CProject();
    void           Clear();
    void           SetTagMonitor();
    void           InitColours();
    void           LoadColours(CProfINIFile & PF);
    void           SaveColours(CProfINIFile & PF);
    flag           Initialise(char * pPrjName);
    int            ShutDown();

    flag           DoNew(flag InConstructor);
    flag           SetupRmtCopies();

    char*          ChkPrjFileName(char *pPrjPath);
    flag           SetPrjCfgFile(CProfINIFile & PF, Strng &NewModelCfg, CModelInfo & Info);
    flag           CheckPrjCfgFile(CProfINIFile & PF, bool & Do_NotOpenMsg);
    void           PutSettings();
    void           GetSettings();

    flag           DoOpen(pchar pPrjPath, pchar NewModelCfg, PrjLoadTypes PrjLoadType);
    
    flag           DoClose(flag ForExit, flag AllowCancel);
    flag           CloseForFailedOpen();
    flag           DoPrjPaths(pchar pPrjPath, flag IsNew, 
                              CPrjFilesLists* pPFL = NULL,
                              BOOL WithVersion = WithVersionDefault,
                              BOOL RemoveOld   = RemoveOldDefault
                              );
    void           CloseRmtCpy();
    flag           DoSave(flag DoPathChg, pchar pPrjPath, flag IsNew);
    flag           DoRestore();
    flag           DoSaveDB(int SaveDBFmt, int SaveOpts, char* pPrjFiles, char* pOldPrjFiles);
    flag           DoLoadDB(char* pPrjFiles, char* pOldPrjName);
//    long           LoadDBCallBack(long When);

    flag           CopyOtherFile(CSFPInfo & FI);
    int            CopyOtherFiles(CPrjFilesLists &PFL, CProfINIFile &PF);
    void           ExpandStg(LPCTSTR StgFn, LPCTSTR Fn);

    int            FixSnapshot(pchar FixFn);
    flag           DoSaveSnapShotScenario(flag DoSnapShot, pchar pName, CExecObjArray *ObjList=NULL);
    flag           DoLoadSnapShotScenario(flag DoSnapShot, pchar pName);
    void           OnFileSavesnapshot();
    void           OnFileLoadsnapshot();
    void           OnFileSavescenario();
    void           OnFileLoadscenario();

    static void    SaveOneWindow(int iNo, LPCTSTR pWindowName, CWnd* pWnd, flag TheMain);//, CProfINIFile & PF);
    void           SaveWindowLayout();
    void           RestoreMainWindow();
    static void    RestoreOneWindow(LPCTSTR pWindowName, CWnd* pWnd, flag TheMain, flag FullRestore=true);
    void           RestoreWindowZOrder();
    
    void           SetNewPrjCfg(LPCTSTR Cfg) { m_sNewPrjCfg = Cfg; };
    void           SetNewPrjPath(LPCTSTR Path) { m_sNewPrjPath = Path; };
    void           SetNewPrjOptions(BOOL WithVersion, BOOL RemoveOld) { m_bWithVersion=WithVersion; m_bRemoveOld=RemoveOld; };
    flag           OnNew();
    flag           OnOpen(pchar pPrjPath);
    flag           OnClose();
    flag           OnSave(pchar pPrjPath);
    flag           OnProjectSaveas(pchar pPrjPath = NULL);
    flag           OnChangeCurrentSettings(LPCTSTR Title, int WhichPage=-1);
    flag           OnLoadAltCfg();

    void           OpenHistorian();
    void           DeleteHistorian();
    void           RestartHistorian();
    void           CloseHistorian(BOOL ForExit = FALSE);
    flag           IsHistorianOpen();
    DWORD          get_RecordTag(LPCSTR Tag);
    flag           put_RecordTag(LPCSTR Tag, flag RecordingOn, DWORD & RetCode);

#if WITHDRVMAN
    void           OpenDrvManager();
    void           CloseDrvManager(BOOL ForExit = FALSE, BOOL ForProject = FALSE);
    flag           ReloadDrvManager(BOOL WithOptions, char * NewDrvManagerName=NULL);
#endif

    void           OpenArcManager(bool FirstOpen);
    void           ConnectArcManager();
    void           DisConnectArcManager();
    void           CloseArcManager(BOOL ForExit = FALSE, BOOL ForProject = FALSE);
    flag           ReloadArcManager(BOOL WithOptions, char * NewArcManagerName=NULL);
    void           RestartArcManager();

    void           OpenIOMarshal();
    void           CloseIOMarshal(BOOL ForExit = FALSE, BOOL ForProject = FALSE);
    flag           ReloadIOMarshal(BOOL WithOptions, char * NewIOMarshalName=NULL, char * NewIOMarshalNode=NULL);

    void           OpenNetManager();
    void           CloseNetManager(BOOL ForExit = FALSE);

    void           OpenDDEManager();
    void           CloseDDEManager(BOOL ForExit = FALSE);

    //void           OpenOPCManager();
    //void           CloseOPCManager(BOOL ForExit = FALSE);

    flag           FindTag(LPCTSTR pTag, LPCTSTR pGrf, LPCTSTR pTrnd, dword Options);
    flag           AllGrfLoaded();
    void           BrowseTags();
    void           HistorianQuery();
    void           CheckLicense(BOOL StartingSolve);
    void           CheckLicenseConditions();
    //int            CheckNoOfIllegalUnits();
    flag           Merge();
    flag           ImportFlwsheet();
    flag           FileImport();
    flag           FileExport();
    flag           ModelHelp(char* pMdlTxt=NULL, int HelpIndex=0);
#if WITHSCRCYCLES
    void           StartScreenSave();
    void           RotateScreenSave();
    void           EndScreenSave();
#endif

    inline flag    LoadBusy() { return bLoadBusy; }; // Flag Set to indicate when a Project Load is in Progress
//    inline void    SetEngine(char* pEngineName) { sModelCfg = pEngineName; }; //assumes only one model library is used

    SFEFlwLib     *FlwLib() { return m_pFlwLib; };

    int           ClearSelectedNodes();
    int           BuildSelectedNodeList(Strng_List & List);



  protected:
    static void    GetSplitInfo(CProfINIFile& PF, CWnd* w, pchar Section, int& SplitCount);
    static void    PutSplitInfo(CProfINIFile& PF, CWnd* w, pchar Section, int& SplitCount);
    flag           PutFrameInfo(CProfINIFile& PF, CWnd* w, pchar Section, Strng& s, CWnd** WndList, int& WndListLen);
#if WITHSCRCYCLES
    void           RotateCycle(CWnd* w, CWnd** Wnds, int& WndCnt);
    void           StoreWndList(CWnd* w, flag DoIt);
    void           RestoreWnds();
#endif
    int            PrepareMerge(CMergeProjectsInfo& MPI);
    void           LoadHelpFileList();

    // ExecObj Overridables
    virtual void   EO_OnAppActivate(BOOL bActive);
    virtual flag   EO_RequestTagInfo(RequestTagInfoRec& Rqst, ReplyTagInfoRec& Info);
    virtual flag   EO_GotoRunStart(CXM_TimeControl &CB);
    virtual flag   EO_GotoRunEnd(CXM_TimeControl &CB, bool TimeChanged);
    virtual flag   EO_PreStart(CXM_TimeControl &CB);
    virtual flag   EO_Starting(flag fBeginStarting);
    virtual flag   EO_Stopping(flag fBeginStopping);

  public:
    flag          ModelsAttached();
    pNodeGrfInfo  GetNodeDrawings();
    pchar         RequestModelDLLPath();
    pchar         RequestModelDLLTag();
    flag          RequestModelInfoByGroupIndex(pchar pGroup, int iIndex, RequestModelInfoRec &Info);
    flag          RequestModelInfoByClassId(pchar pClass, RequestModelInfoRec & Info);
    flag          RequestModelIOInfoByClassId(pchar pClass, RequestModelIOInfoArray & Info);
    flag          RequestModelIOInfoByIndex(pchar pTag, int iIndex, RequestModelIOInfoRec & Info);
    flag          RequestModelIOInfoById(pchar pTag, int iId, RequestModelIOInfoRec & Info);
    flag          RequestModelIOConn(pchar pTag, int iNo, RequestModelIOConnRec & Info);
    int           RequestModelStatus(CModelStatusCriterion &Criteria, CModelStatusArray &Status);
    int           RequestModelStatusChgCnt(CModelStatusCriterion &Criteria, int MaxTest);
    int           RequestTagRefInfo(LPCTSTR Tag, CXRefInfoArray &Refs);
    //int           RequestXRefInfoChg();
    flag          RequestModelClassId(pchar pTag, Strng & ClassId);
    flag          RequestModelConnInfo(pchar pTag, RequestConnModelInfoRec & Info);
    flag          FlwModelExists(pchar pTag);
    flag          TestModelTagUnique(pchar pTag, CUniqueTagRulesBlk & TRB/*pchar pHdr, char SepChar*/, Strng &UniqueTag, flag HdrAsIs);

    int           GetRctInfo(CRctBlkInformation & RctInfo);
    int           SetRctInfo(LPCTSTR NodeTag);

    int           GetNodeWiring(CNodeWiring &NodeWiring);
    int           GetLinkWiring(CLinkWiring &LinkWiring);
    int           SetLinkWiring(CLinkWiring &LinkWiring);

    int           AddNodeModel(pchar ModelType, pchar SubClass, pchar Tag);
    int           DeleteNodeModel(pchar Tag);
    int           AddNodeConnect(pchar Type, pchar Tag, pchar SrcTag, pchar SrcOut, pchar DstTag, pchar DstIn);
    int           AddConnect(pchar SrcTag, pchar SrcOut, pchar DstTag, pchar DstIn);
    int           DisConnect(pchar SrcTag, pchar SrcOut, pchar DstTag, pchar DstIn);
    int           AddConnect(pchar Src, pchar Dst);
    int           DisConnect(pchar Src, pchar Dst);

  };

extern DllImportExport CProject * gs_pPrj;

#undef DllImportExport 

//===========================================================================

#endif
