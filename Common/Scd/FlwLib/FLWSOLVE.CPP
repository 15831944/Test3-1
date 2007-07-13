//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include "sc_defs.h"
#include "datacnvs.h"

#define  __FLWSOLVE_CPP
#include "flwsolve.h"

#include "flw_err.h"
#include "errorlog.h"
#include "dbgmngr.h"
#include "resource.h"
#include "executiv.h"
#include "sfe_init.h"
#include "scd_wm.h"
#include "nrecipe.h"
#include "power.h"
#include "surfaces.h"
#include "vlebase.h"
#include "mdlrunmngr.h"
#include "noise.h"
#include "sp_react.h"

#include "COMStuff.h"
#include "licbase.h"
#include <limits.h>
#include <Psapi.h>
#pragma comment(lib, "Psapi.lib")

//#include "optoff.h"

//===========================================================================
//temp force solver threads to zero until multi-threaded solver bug is fixed
#define FORCERQDTHREADS    0
// Allow (in Dynamic) Ctrl aspects of models to run multithreaded.
#define ALLOWCTRLTHREADING 0

#define WithRestartPGM 0

//#define ClearSolutionOnStop    01
#define MOVEDYNSTATS   1
#define MYASSERT(x) /*ASSERT(x)*/

#define PROGRESSLEVEL 0
#if (PROGRESSLEVEL>0)

#define STARTPROGRESS                       \
  CStopWatch SW; SW.Start();                \
  double ProcessorTime = GetProcessorTime(); 

#define SHOWPROGRESS(L, x)                                            \
  {                                                                   \
  if (PROGRESSLEVEL>=L)                                               \
    {                                                                 \
    int y=50-strlen(x);                                               \
    double Lp=SW.Lap();                                               \
    double Pt=GetProcessorTime();                                     \
    dbgpln("%s %*.*s %10.1fms %6.2f%%", x, y, y,                      \
      "======================================================",       \
      Lp*1e3, 100*(Pt-ProcessorTime)/GTZ(Max(1,FSTC.NThreads())*Lp)); \
    ProcessorTime = Pt;                                               \
    }                                                                 \
  };
#else

#define STARTPROGRESS ;
#define SHOWPROGRESS(L,x) {};

#endif
//===========================================================================

#define dbgMSSmooth            01
#define dbgFlwSolve            (1 || WITHDEBUG)
#define dbgFlwSolveTS0         (1 || WITHDEBUG)
#define dbgFlwSolveTS1         (0 || WITHDEBUG)

#if (dbgFlwSolve)

#define dbgRunControl          0
#define dbgflows               0
#define dbgDerivatives         0
#define dbgTraceMemUse1        0
#define dbgTraceMemState       0
#define dbgTraceMemState1      0
#define dbgSetZs               0
#define TimingLaps             0
#define TimingSteps            0
#define TimePrestart           0
#define DoLapTime              0
#define dbgTestConnectivity    0
#define TimingStats            NDSTOPWATCHES

static CDbgMngr dbgBuildOrder         ("FlwSolve",  "BuildOrd");

#if WithIOChgMonitor
static CDbgMngr dbgDumpChanges        ("FlwNode",   "DumpChanges");
static CDbgMngr dbgDumpChangesIn      ("FlwNode",   "DumpChangesIn");
#endif

static CDbgMngr dbgDumpDerivs         ("FlwSolve",  "Dump-Derivs");
static CDbgMngr dbgDumpStates         ("FlwSolve",  "Dump-States");
#if (WITHTIMERS)
static CDbgMngr dbgDumpStopWatch      ("FlwSolve",  "Dump-StopWatch");
#endif
static CDbgMngr dbgDuplicate          ("FlwSolve",  "Duplicate");
static CDbgMngr dbgEvalOrdChg         ("FlwSolve",  "EvalOrd-Chg");
static CDbgMngr dbgEvalOrdChgIO       ("FlwSolve",  "EvalOrd-ChgIO");
static CDbgMngr dbgEvalOrdFind        ("FlwSolve",  "EvalOrd-Find");
static CDbgMngr dbgEvalOrdSummary     ("FlwSolve",  "EvalOrd-Summary");
static CDbgMngr dbgGlobalLinks        ("FlwSolve",  "GlobalLinks");
static CDbgMngr dbgMacroMdls          ("FlwSolve",  "MacroMdls");
static CDbgMngr dbgMacroMdls1         ("FlwSolve",  "MacroMdls1");
static CDbgMngr dbgMemoryUse          ("FlwSolve",  "MemoryUse");
static CDbgMngr dbgNdStats            ("FlwSolve",  "NdStats");
static CDbgMngr dbgNdStatsAll         ("FlwSolve",  "NdStatsAll");
static CDbgMngr dbgNdStatsIter        ("FlwSolve",  "NdStatsIter");
static CDbgMngr dbgNoteIOChgs         ("FlwSolve",  "NoteIOChgs");
static CDbgMngr dbgNoteNdCall         ("FlwSolve",  "NoteNdCall");
static CDbgMngr dbgRunStats           ("FlwSolve",  "RunStats");
static CDbgMngr dbgRunStatsIter       ("FlwSolve",  "RunStatsIter");
static CDbgMngr dbgShowFlows          ("FlwSolve",  "ShowFlows");
static CDbgMngr dbgShowTagsInPreamble ("FlwSolve",  "ShowTagsInPreamble");
static CDbgMngr dbgSkipFindObj        ("FlwSolve",  "SkipFindObj");
static CDbgMngr dbgSkipLoadObj        ("FlwSolve",  "SkipLoadObj");
static CDbgMngr dbgTearSetup          ("FlwSolve",  "Tears-Setup");
static CDbgMngr dbgTestClosure        ("FlwSolve",  "Tests-Closure");
static CDbgMngr dbgTearCnvgLoop       ("FlwSolve",  "Tears-CnvgLoop");
static CDbgMngr dbgThreads            ("FlwSolve",  "Threads");
static CDbgMngr dbgThreadsJob         ("FlwSolve",  "Threads-Job");
static CDbgMngr dbgThreadsNodes       ("FlwSolve",  "Threads-Nodes");
static CDbgMngr dbgThreadsSeq         ("FlwSolve",  "Threads-Seq");
static CDbgMngr dbgThreadsSleep       ("FlwSolve",  "Threads-Sleep");
static CDbgMngr dbgThreadsTiming      ("FlwSolve",  "Threads-Timing");
static CDbgMngr dbgThreadsForceOff    ("FlwSolve",  "Threads-ForceOff");
static CDbgMngr dbgXRefs              ("FlwSolve",  "XRefs");
static CDbgMngr dbgXRefsBuild         ("FlwSolve",  "XRefs-Build");
static CDbgMngr dbgXRefsSetup         ("FlwSolve",  "XRefs-Setup");

static CDbgMngr dbgMSDumpAGData       ("ProBal",    "MS-DumpAGData");
static CDbgMngr dbgMSDumpMtx          ("ProBal",    "MS-DumpMtx");
static CDbgMngr dbgMSDumpResults      ("ProBal",    "MS-DumpResults");
static CDbgMngr dbgMSForceBuild       ("ProBal",    "MS-ForceBuild");

static CDbgMngr dbgDumpSurfaces       ("Surfaces",  "Dump");

static CDbgMngr dbgXRefStatus         ("XRefs",     "Status");

static CDbgMngr dbgSetZsValue         ("Models",      "SetZsValue");
static CDbgMngr dbgSetZsTrace         ("Models",      "SetZsTrace");

static int *NdCallCntArray=NULL;
static long NdCallCntLen=0;

static long DoNoteIOChgsOn=false;

inline void dbgsep() { dbgpln("----------------------------------------------------------------------------"); };

#else

#define TimingStats            0
#define dbgRunControl          0
#define dbgflows               0
#define dbgDerivatives         0
#define dbgTraceMemUse1        0
#define dbgTraceMemState       0
#define dbgTraceMemState1      0
//#define dbgSetZs               0
#define TimingLaps             0
#define TimingSteps            0
#define DoLapTime              0
#define dbgTestConnectivity    0

#endif

#if TimingLaps || TimingSteps
//#define NDSTOPWATCH(a) (a)
CStopWatch s_swTiming;
#define TIMINGLAP(a) dbgpln("TL:%-20.20s  %6.3f",a,::s_swTiming.Lap());
#else
#define TIMINGLAP(a)
//#define NDSTOPWATCH(a)
#endif


//===========================================================================
//
//
//
//===========================================================================

#ifdef   _DEBUG
#define  SET_CRT_DEBUG_FIELD(a) \
            _CrtSetDbgFlag((a) | _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG))
#define  CLEAR_CRT_DEBUG_FIELD(a) \
            _CrtSetDbgFlag(~(a) & _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG))
#else
#define  SET_CRT_DEBUG_FIELD(a)   ((void) 0)
#define  CLEAR_CRT_DEBUG_FIELD(a) ((void) 0)
#endif

//===========================================================================
//
//
//
//===========================================================================

XID xidWorstMagInt0                   = FlwXID(1000);
XID xidWorstMagInt1                   = FlwXID(1001);
XID xidWorstMagInt2                   = FlwXID(1002);
XID xidWorstMagInt3                   = FlwXID(1003);
XID xidWorstMagInt4                   = FlwXID(1004);
XID xidWorstMagInt5                   = FlwXID(1005);
XID xidWorstMagInt6                   = FlwXID(1006);
XID xidWorstMagInt7                   = FlwXID(1007);
XID xidWorstMagInt8                   = FlwXID(1008);
XID xidWorstMagInt9                   = FlwXID(1009);
                                               
XID xidLnkEPSAbs                      = FlwXID(1010);
XID xidLnkEPSRel                      = FlwXID(1011);
XID xidLnkNetDerate                   = FlwXID(1012);
XID xidLnkQmMatchAbs                  = FlwXID(1013);
XID xidLnkQmMatchRel                  = FlwXID(1014);

XID xidH2OProperties                  = FlwXID(1015);
XID xidUseSurfaces                    = FlwXID(1016);
XID xidDoSmoothing                    = FlwXID(1017);
XID xidNThreads                       = FlwXID(1018);
XID xidRqdThreads                     = FlwXID(1019);

XID xidDefaultArea                    = FlwXID(1020);
//XID xidMissSpillAreas                 = FlwXID(2022);
XID xidODEMethod                      = FlwXID(1521);
XID xidODESSCtrl                      = FlwXID(1522);
XID xidODETime                        = FlwXID(1523);
XID xidTimeF                          = FlwXID(1524);
XID xidTimeD                          = FlwXID(1525);
XID xidStepSizeF                      = FlwXID(1526);
XID xidStepSizeD                      = FlwXID(1527);
XID xidStepCount                      = FlwXID(1528);

XID xidODETimeInc                     = FlwXID(2026);
XID xidStepSizeRqd                    = FlwXID(2027);
XID xidODETimeIncMxRqd                = FlwXID(2028);
XID xidODETimeIncMnRqd                = FlwXID(2029);
XID xidODEDerivEstInc                 = FlwXID(2030);
XID xidODESettleTime                  = FlwXID(2031);
XID xidODEMag_Eps_A                   = FlwXID(2032);
XID xidODEMag_Eps_R                   = FlwXID(2033);
XID xidODESafetyScale                 = FlwXID(2034);
XID xidODEGrowLimit                   = FlwXID(2035);
XID xidODEGrow                        = FlwXID(2036);
XID xidODEOKShrinkLimit               = FlwXID(2037);
XID xidODEOKShrink                    = FlwXID(2038);
XID xidODEBADShrink                   = FlwXID(2039);
XID xidODESteps                       = FlwXID(2040);
XID xidODEGoodIters                   = FlwXID(2041);
XID xidODEBadIters                    = FlwXID(2042);
XID xidODENStatesInt                  = FlwXID(2043);
XID xidODENStatesSR                   = FlwXID(2044);
XID xidODEIters4Step                  = FlwXID(2045);
XID xidODETimeDesc                    = FlwXID(2046);
XID xidODETimeFullDesc                = FlwXID(2047);
XID xidRealTimeOn                     = FlwXID(2048);
XID xidRealTimeMultiplier             = FlwXID(2049);

XID xidResultsValid                   = FlwXID(2050);
XID xidConditionCnt                   = FlwXID(2051);
XID xidSolveIters                     = FlwXID(2052);
XID xidSolveItersBad                  = FlwXID(2053);
XID xidSolveUnknowns                  = FlwXID(2054);
XID xidSolveBadError                  = FlwXID(2055);
XID xidSolveBadLimit                  = FlwXID(2056);
XID xidSolveWorstErr                  = FlwXID(2057);

XID xidIterPerStep                    = FlwXID(2059);
XID xidSolveNets                      = FlwXID(2060);
XID xidSolveSubNets                   = FlwXID(2061);
XID xidSolveNodes                     = FlwXID(2062);
XID xidSolveFxdLnks                   = FlwXID(2063);
XID xidSolveLoneLnks                  = FlwXID(2064);
XID xidSolveNetLnks                   = FlwXID(2065);

XID xidStartupDirectory               = FlwXID(2070);
XID xidStartupDrive                   = FlwXID(2071);
XID xidProgFiles                      = FlwXID(2072);
XID xidTemporaryFiles                 = FlwXID(2073);
XID xidBaseCfgFiles                   = FlwXID(2074);
XID xidCfgName                        = FlwXID(2075);
XID xidCfgFile                        = FlwXID(2076);
XID xidCfgFiles                       = FlwXID(2077);
XID xidPrjName                        = FlwXID(2079);
XID xidPrjFile                        = FlwXID(2080);
XID xidPrjFiles                       = FlwXID(2081);
XID xidPrjBackupFiles                 = FlwXID(2082);
XID xidPrjIniFile                     = FlwXID(2083);
XID xidPrjRmtCpyFiles                 = FlwXID(2084);

XID xidXSWStart                       = FlwXID(2090);
XID xidXSWQuery                       = FlwXID(2091);
XID xidXSWBuild                       = FlwXID(2092);
XID xidXSWExec                        = FlwXID(2093);
XID xidXSWExecDn                      = FlwXID(2094);
XID xidXSWRead                        = FlwXID(2095);
XID xidXSWWrite                       = FlwXID(2096);
XID xidXSWStop                        = FlwXID(2097);
XID xidXSWRunSpdI                     = FlwXID(2098);
XID xidXSWRunSpdMA                    = FlwXID(2099);
XID xidXSWRunSpdA                     = FlwXID(2100);
XID xidXSWStepI                       = FlwXID(2101);
XID xidXSWStepMA                      = FlwXID(2102);
XID xidXSWStepA                       = FlwXID(2103);
XID xidXSWProcUseI                    = FlwXID(2104);
XID xidXSWProcUseMA                   = FlwXID(2105);
XID xidXSWProcUseA                    = FlwXID(2106);
XID xidXSWSecs                        = FlwXID(2107);
XID xidXSWSecsDesc                    = FlwXID(2108);
XID xidXSWSimSecs                     = FlwXID(2109);
XID xidXSWSimSecsDesc                 = FlwXID(2110);
XID xidXSWSteps                       = FlwXID(2111);
XID xidXSWMVAvgPts                    = FlwXID(2112);
XID xidXSWSecsStart                   = FlwXID(2113);

XID xidDTTime                         = FlwXID(2180);
XID xidDTTimeHMS                      = FlwXID(2181);
XID xidDTTimeDHMS                     = FlwXID(2182);
XID xidDTTimeDate                     = FlwXID(2183);
XID xidDTmSecsCount                   = FlwXID(2184);
XID xidDTSecsCount                    = FlwXID(2185);
XID xidDTMinsCount                    = FlwXID(2186);
XID xidDTHoursCount                   = FlwXID(2187);
XID xidDTDaysCount                    = FlwXID(2188);
XID xidDTDateDay                      = FlwXID(2189);
XID xidDTDateMonth                    = FlwXID(2190);
XID xidDTDateYear                     = FlwXID(2191);
XID xidDTDateDayOfWeek                = FlwXID(2192);
XID xidDTDateDayOfYear                = FlwXID(2193);
XID xidDTDateDaysInMonth              = FlwXID(2194);
XID xidDTDateDaysInYear               = FlwXID(2195);
XID xidDTDateDayOfWeekDesc            = FlwXID(2196);
XID xidDTDateMonthDesc                = FlwXID(2197);

XID xidScnState                       = FlwXID(2200);
XID xidManualScnRestart               = FlwXID(2201);
XID xidReStartIfScnBusy               = FlwXID(2202);
XID xidReStartIfScnComplete           = FlwXID(2203);

XID xidScnType                        = FlwXID(2210);
XID xidScnTypeLast                    = FlwXID(2211);
XID xidScnTermAction                  = FlwXID(2212);
XID xidRunDurationF                   = FlwXID(2213);
XID xidRunStopTimeF                   = FlwXID(2214);
XID xidRunDurationD                   = FlwXID(2215);
XID xidRunStopTimeD                   = FlwXID(2216);
XID xidStepCountMax                   = FlwXID(2217);
XID xidRunTimeToStopF                 = FlwXID(2218);
XID xidRunTimeToStopD                 = FlwXID(2219);
XID xidScnTimeF                       = FlwXID(2220);
XID xidScnTimeD                       = FlwXID(2221);
XID xidScnTimeMaxF                    = FlwXID(2222);
XID xidScnTimeMaxD                    = FlwXID(2223);

XID xidPGMExecTimeOut                 = FlwXID(2230);
XID xidRandomSeed                     = FlwXID(2231);

XID xidDIOSpillsVents                 = FlwXID(2240); 
XID xidDIOLeaks                       = FlwXID(2241);       
XID xidDIOAudit                       = FlwXID(2242);       

XID xidProgress                       = FlwXID(2300);
XID xidClock                          = FlwXID(2301);
XID xidClockToGo                      = FlwXID(2302);
XID xidClockTotal                     = FlwXID(2303);
XID xidElapsedSteps                   = FlwXID(2304);

XID xidDynCmdScnRestart               = FlwXID(2314);
XID xidDynCmdResetTime                = FlwXID(2315);
XID xidDynCmdRestartHist              = FlwXID(2316);

//XID xidConvergeTm                     = FlwXID(2120);
//XID xidFlwSolveTm                     = FlwXID(2121);
//XID xidFlwWashTm                      = FlwXID(2122);
//XID xidDiscreteTm                     = FlwXID(2123);
//XID xidCtrlActionsTm                  = FlwXID(2124);
//XID xidCtrlStrategyTm                 = FlwXID(2125);

XID xidGlblSWExec0                    = FlwXID(3000);
XID xidGlblSWExecTot0                 = FlwXID(3500);
XID xidGlblSWFlw0                     = FlwXID(4000);
XID xidGlblSWFlwTot0                  = FlwXID(4500);

XID xidHistTagFile                    = FlwXID(5025);
XID xidHistTagFolder                  = FlwXID(5026);
XID xidHistTagEdit                    = FlwXID(5027);
XID xidHistTagBrowse                  = FlwXID(5028);
XID xidHistSetTagsNow                 = FlwXID(5029);

XID xidGenVerStr                      = FlwXID(5030);
XID xidGenUpdateVerStr                = FlwXID(5031);
XID xidGenMajorBuildNo                = FlwXID(5032);
XID xidGenBuildNo                     = FlwXID(5033);
XID xidGenBuildDate                   = FlwXID(5034);
XID xidGenDateAndTime                 = FlwXID(5035);
XID xidGenTime1970                    = FlwXID(5036);

XID xidSetStateAll                    = FlwXID(5040);
XID xidIncludeSpcCnvs                 = FlwXID(5041);
XID xidShowSpcRangeErrors             = FlwXID(5042);
XID xidShowSpcXRangeErrors            = FlwXID(5043);

XID xidDynStatReset                   = FlwXID(5100);
XID xidDynStatStopTm                  = FlwXID(5101);
XID xidAtStartRestartHist             = FlwXID(5102);
XID xidAtStartSetHistTags             = FlwXID(5103);
XID xidAtStartResetTime               = FlwXID(5104);
XID xidAtStartRestartArchive          = FlwXID(5105);
XID xidAtStartRestartPGM              = FlwXID(5106);
XID xidAtStartRestartPrf              = FlwXID(5107);
XID xidAtStartTimeF                   = FlwXID(5108);
XID xidAtStartTimeD                   = FlwXID(5109);
//XID xidEventLogOn                     = FlwXID(5110);
XID xidAtStartAllZeroFlows            = FlwXID(5111);        
XID xidAtStartAllEmpty                = FlwXID(5112);            
XID xidAtStartAllPreSet               = FlwXID(5113);           
XID xidAtStartAllSteadyState          = FlwXID(5114);      
XID xidAtStartAllDynStatsRunInit      = FlwXID(5115);  
XID xidAtStartAllEmptySpillTargets    = FlwXID(5116);


XID xidMCloseTolAbs                   = FlwXID(5151);
XID xidMCloseTolRel                   = FlwXID(5152);
XID xidHCloseTolAbs                   = FlwXID(5153);
XID xidHCloseTolRel                   = FlwXID(5154);
XID xidHCloseTolRel2                  = FlwXID(5155);
XID xidMCloseTol                      = FlwXID(5156);
XID xidHCloseTol                      = FlwXID(5157);
XID xidHCloseTol2                     = FlwXID(5158);

XID xidExecState                      = FlwXID(5160);
XID xidExecStateDesc                  = FlwXID(5161);
XID xidExecStart                      = FlwXID(5162);
XID xidExecIdle                       = FlwXID(5163);
XID xidExecStop                       = FlwXID(5164);

XID xidSSPTolAbs                      = FlwXID(5171);
XID xidSSPTolRel                      = FlwXID(5172);
XID xidSSQmTolAbs                     = FlwXID(5173);
XID xidSSQmTolRel                     = FlwXID(5174);

XID xidNormT                          = FlwXID(5196);
XID xidNormP                          = FlwXID(5197);
XID xidStdT                           = FlwXID(5198);
XID xidStdP                           = FlwXID(5199);
XID xidNoOfTears                      = FlwXID(5200);
XID xidNetCnvCount                    = FlwXID(5201);
XID xidUDFCnvCount                    = FlwXID(5202);
XID xidNetMaxResRatio                 = FlwXID(5203);
XID xidNetFastConverge                = FlwXID(5204);
XID xidNetResRunAvgStart              = FlwXID(5205);
XID xidNetResRunAvgCount              = FlwXID(5206);

XID xidNetVFlowOptions                = FlwXID(5210);
XID xidNetLFlowOptions                = FlwXID(5211);

XID xidSetStateAllZeroFlows           =  FlwXID(5250);
XID xidSetStateAllEmpty               =  FlwXID(5251);
XID xidSetStateAllPreSet              =  FlwXID(5252);
XID xidSetStateAllSteadyState         =  FlwXID(5253);
XID xidSetStateAllPBInit              =  FlwXID(5254);
XID xidSetStateAllDynStatsRunInit     =  FlwXID(5255);
XID xidSetStateAllEmptySpillTargets   =  FlwXID(5256);

XID xidSMMemoryLoad                   = FlwXID(5300);
XID xidSMTotalPhys                    = FlwXID(5301);
XID xidSMAvailPhys                    = FlwXID(5302);
XID xidSMTotalPageFile                = FlwXID(5303);
XID xidSMAvailPageFile                = FlwXID(5304);
XID xidSMTotalVirtual                 = FlwXID(5305);
XID xidSMAvailVirtual                 = FlwXID(5306);

XID xidPMPageFaultCount               = FlwXID(5310);
XID xidPMPeakWorkingSetSize           = FlwXID(5311);
XID xidPMWorkingSetSize               = FlwXID(5312);
XID xidPMQuotaPeakPagedPoolUsage      = FlwXID(5313);
XID xidPMQuotaPagedPoolUsage          = FlwXID(5314);
XID xidPMQuotaPeakNonPagedPoolUsage   = FlwXID(5315);
XID xidPMQuotaNonPagedPoolUsage       = FlwXID(5316);
XID xidPMPagefileUsage                = FlwXID(5317);
XID xidPMPeakPagefileUsage            = FlwXID(5318);

const long MaxGrfPages = 128;
XID xidGrfCount                       = FlwXID(5350);
XID xidGrfFirstActive                 = FlwXID(5351);
XID xidGrfLastActive                  = xidGrfFirstActive+(MaxGrfPages*2);

const long ThTmSpacing = 8*eFITMaxCmd;
XID xidThTmThreadId                   = xidGrfLastActive   + 1;
XID xidThTmCallSets                   = xidThTmThreadId    + ThTmSpacing;
XID xidThTmCalls                      = xidThTmCallSets    + ThTmSpacing;
XID xidThTmTotal                      = xidThTmCalls       + ThTmSpacing;
XID xidThTmLockTotal                  = xidThTmTotal       + ThTmSpacing;
XID xidThTmExecTotal                  = xidThTmLockTotal   + ThTmSpacing;
XID xidThTmUnLockTotal                = xidThTmExecTotal   + ThTmSpacing;
XID xidThTmWaitTotal                  = xidThTmUnLockTotal + ThTmSpacing;
XID xidThTmLast                       = xidThTmWaitTotal   + ThTmSpacing;
// Leave 8(ThreadCnt) * eFITMaxCmds

//===========================================================================
//
//
//
//===========================================================================

#if (dbgFlwSolve)
void SetUpLoopDebug(FlwNode *pRoot)
  {
  if (dbgNoteNdCall())
    {
    if (NdCallCntArray)
      {
      int j=NdCallCntLen * eFITMaxCmd;
      for (int i=0; i<j; i++)
        NdCallCntArray[i]=0;
      }
    }

  DoNoteIOChgsOn=dbgNoteIOChgs();
  if (DoNoteIOChgsOn)
    {
    FlwNode *p=NULL;
    while (pRoot->GetObj_All(p))
      {
      for (int i=0; i<p->NoFlwIOs(); i++)
        p->IOConduitMem(i)->QCopy(*(p->IOConduit(i)));
      }
    }
  }

//--------------------------------------------------------------------------

void DoNoteIOChgs(FlwNode *p)
  {
  if (!DoNoteIOChgsOn)
    return;

  int First=1;
  for (int i=0; i<p->NoFlwIOs(); i++)
    {
    if (p->IO_Out(i))
      {
      SpConduit &Was=*p->IOConduitMem(i);
      SpConduit &Now=*p->IOConduit(i);

      double T0=Was.Temp();
      double P0=Was.Press();
      double M0=Was.QMass();

      double T1=Now.Temp();
      double P1=Now.Press();
      double M1=Now.QMass();

      int TChg=fabs(T1-T0)>0.01;
      int PChg=fabs(P1-P0)>0.01;
      int MChg=fabs(M1-M0)>0.01;
      if (TChg || PChg || MChg)
        {
        if (TChg)
          dbgp(" T:%8.2f > %8.2f [+%8.2f]", T0,T1,T1-T0);
        else
          dbgp("   %8s   %8s  %8s ", "","","");
        if (PChg)
          dbgp(" P:%8.2f > %8.2f [+%8.2f]", P0,P1,P1-P0);
        else
          dbgp("   %8s   %8s  %8s ", "","","");
        if (MChg)
          dbgp(" M:%8.2f > %8.2f [+%8.2f]", M0,M1,M1-M0);
        else
          dbgp("   %8s   %8s  %8s ", "","","");
        if (First)
          dbgp(" %s", p->FullObjTag());
        else
          dbgp(" %*s", strlen(p->FullObjTag()), "");
        dbgpln(" > %s",p->Nd_Rmt(i)->FullObjTag());
        First=0;
        First=0;
        }
      }
    p->IOConduitMem(i)->QCopy(*(p->IOConduit(i)));
    }
  }

//---------------------------------------------------------------------------

inline void NoteNdCall(int What, FlwNode * p)
  {
  if (dbgNoteNdCall() && NdCallCntLen>0)
    {
    if (NdCallCntArray==NULL)
      {
      NdCallCntArray=new int[NdCallCntLen * eFITMaxCmd];
      int j=NdCallCntLen * eFITMaxCmd;
      for (int i=0; i<j; i++)
        NdCallCntArray[i]=0;
      }
    if (p->iAllIndex>=0)
      NdCallCntArray[p->iAllIndex*eFITMaxCmd+What]++;
    }
  }

#define NoteMTItem(What, p)                                                   \
  {                                                                           \
  if (dbgThreads())                                                           \
    dbgpln("%-20s %4i ------ %s", eFITCmdStr(What), GetCurrentThreadId(), p->Tag());   \
  if (dbgThreadsSleep())                                                      \
    Sleep(0);                                                                 \
  }

static int NoteNdIOChgsOldWhat=0;

#define NoteNdIOChgs(What, p)                                             \
  if (DoNoteIOChgsOn)                                                     \
    {                                                                     \
    if (What!=NoteNdIOChgsOldWhat)                                        \
      dbgpln(">>---------------------- %s", eFITCmdStr(What));            \
    NoteNdIOChgsOldWhat=What;                                             \
    if (DoNoteIOChgsOn)                                                   \
      DoNoteIOChgs(p);                                                    \
    }

//---------------------------------------------------------------------------

//
//#define Preamble(p, What, Clk)                                                      \
//{                                                                                   \
//dbgWhat=What;                                                                       \
//if (1)/*DoFullPrePost && DoPartPrePost )*/                                                \
//  {                                                                                 \
//  if (dbgSWLockOn) dbgSWLock.Stop(); else { dbgSWWait.Stop(); dbgSWLock.Clear(); }  \
//  dbgSWXDbg.ReStart();                                                              \
//  if (0 && (dbgShowTagsInPreamble() || dbgThreadsSeq()) && PreambleTag[What])       \
//    {                                                                               \
//    dbgpln("%-20s --- %4i ------------ %s ",                                        \
//      eFITCmdStr(What), GetCurrentThreadId(), p->FullObjTag());                     \
//    }                                                                               \
//  if (Clk!=FSW_Null) NDSTARTSW(p, Clk);                                             \
//  if (What!=eFITNull)                                                               \
//    {                                                                               \
//    NoteMTItem(What, p);                                                            \
//    NoteNdCall(What, p);                                                            \
//    }                                                                               \
//  dbgSWXDbg.Stop();                                                                 \
//  dbgSWExec.ReStart();                                                              \
//  }                                                                                 \
//}
//
//#define Postamble(p, What, Clk)                                             \
//{                                                                           \
//if (1)/*DoFullPrePost && DoPartPrePost )*/                                        \
//  {                                                                         \
//  dbgSWExec.Stop();                                                         \
//  dbgSWXDbg.ReStart();                                                      \
//  if (What!=eFITNull)                                                       \
//    {                                                                       \
//    NoteNdIOChgs(What, p);                                                  \
//    }                                                                       \
//  if (Clk!=FSW_Null) NDSTOPSW(p, Clk);                                      \
//  dbgSWXDbg.Stop();                                                         \
//  if (DoFullPrePost)                                                        \
//    {                                                                       \
//    double L=1e6*dbgSWLock.Secs();                                          \
//    double X=1e6*dbgSWExec.Secs();                                          \
//    double U=1e6*dbgSWUnLock.Secs();                                        \
//    double O=1e6*dbgSWWait.Secs();                                          \
//    dbgpln("Time %4i %-20.20s ----- L:%10.1f X:%10.1f U:%10.1f O:%10.1f L:%5.1f%% X:%5.1f%% U:%5.1f%% O:%5.1f%% ------ %s",   \
//    GetCurrentThreadId(), eFITCmdStr(What),                                 \
//        L,X,U,O,                                                            \
//        100*L/GTZ(L+X+U+O),                                                 \
//        100*X/GTZ(L+X+U+O),                                                 \
//        100*U/GTZ(L+X+U+O),                                                 \
//        100*O/GTZ(L+X+U+O),                                                 \
//        p->FullObjTag());                                                   \
//    }                                                                       \
//  dbgLockTotal+=dbgSWLock.Secs();                                           \
//  dbgExecTotal+=dbgSWExec.Secs();                                           \
//  dbgUnLockTotal+=dbgSWUnLock.Secs();                                       \
//  dbgWaitTotal+=dbgSWWait.Secs();                                           \
//  dbgSWUnLock.ReStart();                                                    \
//  dbgSWWait.ReStart();                                                      \
//  dbgSWUnLockDn=false;                                                      \
//  }                                                                         \
//}


#else


#define NoteMTItem(What, p)   {}
#define NoteNdCall(What, p)   {}
#define NoteNdIOChgs(What, p)   {}
//#define Preamble(p, What, Clk)  {}
//#define Postamble(p, What, Clk) {}
//#define PostambleUnLock()       {}
//#define PostambleTl()           {}

#endif

//===========================================================================
//
//
//
//===========================================================================

#ifndef _RELEASE
#define CALC_EVAL_ORDER(NJ) NJ.Nd.m_Clusters[NJ.m_iCluster].m_iProcOrderActual = InterlockedIncrement(&gs_EvalProductsActualCount);
static long gs_EvalProductsActualCount=0;
#else
#define CALC_EVAL_ORDER(Nd) {}
#endif

//===========================================================================
//
//
//
//===========================================================================

class CThreadTimer;
class CThreadTimerAccum
  {
  public:
    CThreadTimerAccum()
      {
      m_bValid=false;
      m_ThreadId=0;
      Reset();
      }

    void Reset()
      {
      m_Total=0;
      m_WaitTotal=0;
      m_LockTotal=0;
      m_ExecTotal=0;
      m_UnLockTotal=0;
      m_nCalls=0;
      m_nCallSets=0;
      }

    void Add(CThreadTimer & T);

  //protected:
    CCriticalSection m_Sect;

    bool          m_bValid;
    long          m_nCalls;
    long          m_nCallSets;
    long          m_ThreadId;

    double        m_Total;
    double        m_WaitTotal;
    double        m_LockTotal;
    double        m_ExecTotal;
    double        m_UnLockTotal;

  };

//---------------------------------------------------------------------------

class CThreadTiming
  {
  public:
    CThreadTiming()
      {
      m_nThreads=0;
      Initialise(0);
      };

    ~CThreadTiming()
      {
      CSingleLock Lk(&m_Sect, true);
      for (int i=0; i<m_Accums.GetSize(); i++)
        delete m_Accums[i];
      };

    void Initialise(int NThreads)
      {
      CSingleLock Lk(&m_Sect, true);
      m_nThreads=NThreads+2;
      for (int i=0; i<m_Accums.GetSize(); i++)
        delete m_Accums[i];

      m_ThreadIds.SetSize(0);
      m_Accums.SetSize(m_nThreads*eFITMaxCmd);
      for (int i=0; i<m_nThreads*eFITMaxCmd; i++)
        m_Accums[i]=new CThreadTimerAccum;
      };

    void Reset()
      {
      CSingleLock Lk(&m_Sect, true);
      int NN=m_Accums.GetSize();
      for (int i=0; i<NN; i++)
        m_Accums[i]->Reset();
      }

    CThreadTimerAccum * GetById(DWORD ThreadId, int What)
      {
      CSingleLock Lk(&m_Sect, true);
      for (int ThreadNo=0; ThreadNo<m_ThreadIds.GetSize(); ThreadNo++)
        if (m_ThreadIds[ThreadNo]==ThreadId)
          return m_Accums[ThreadNo*eFITMaxCmd+What];

      m_ThreadIds.SetAtGrow(ThreadNo, ThreadId);

      //int TN=ThreadNo+1;
      m_nThreads=Max(m_nThreads, long(ThreadNo+1));
      int NO=m_Accums.GetSize();
      int NN=m_nThreads*eFITMaxCmd;
      if (NN>NO)
        {
        m_Accums.SetSize(NN);
        while (NO<NN)
          m_Accums[NO++]=new CThreadTimerAccum;
        }
      for (int i=ThreadNo*eFITMaxCmd; i<(ThreadNo+1)*eFITMaxCmd; i++)
        {
        m_Accums[i]->m_bValid=true;
        m_Accums[i]->m_ThreadId=GetCurrentThreadId();
        }
      return m_Accums[ThreadNo*eFITMaxCmd+What];
      };

    CThreadTimerAccum * GetByIndex(int ThreadNo, int What)
      {
      CSingleLock Lk(&m_Sect, true);
      return m_Accums[ThreadNo*eFITMaxCmd+What];
      };

    int ThreadCount()
      {
      CSingleLock Lk(&m_Sect, true);
      return m_nThreads;//m_ThreadIds.GetCount();
      };

    void Dump();

  protected:
    long                                              m_nThreads;
    CArray <DWORD, DWORD>                             m_ThreadIds;
    CArray <CThreadTimerAccum*, CThreadTimerAccum*>   m_Accums;
    CCriticalSection                                  m_Sect;
  };

static CThreadTiming gs_ThreadTiming;

//---------------------------------------------------------------------------

#define DOTHREADSHOWNODES 1
#define DOTHREADTIMERALL 0
#define DOTHREADTIMERTOTAL 0
class CThreadTimer
  {
  friend class CThreadTimerAccum;
  public:
    CThreadTimer(int What, int NdSW)
      {
      m_What=What;
      #if (DOTHREADTIMERTOTAL || DOTHREADTIMERALL)
      m_Total.ReStart();
      m_bComplete=false;
      #if (DOTHREADTIMERALL)
      //m_iThread=-1;
      m_NdSW=NdSW;
      m_WaitTotal=0;
      m_LockTotal=0;
      m_ExecTotal=0;
      m_UnLockTotal=0;
      m_iDone=-1;
      m_nCalls=0;
      //m_SW.ReStart();
      m_Memory=0;//m_Total.Secs();
      #endif
      #endif
      #if DOTHREADSHOWNODES 
      #if dbgFlwSolve
      if (dbgThreadsNodes())
        {
        int clen=strlen(eFITCmdStr(m_What))+2;
        int part1=(CFlwSlvThreads::sm_ThreadSpacing-clen)/2;
        int part2=CFlwSlvThreads::sm_ThreadSpacing-clen-part1;
        dbgpln("%*s%*.*s %s %*.*s", 18+CFlwSlvThreads::dbgIndent(GetCurrentThreadId()), "",   
          part1,part1,"##################",
          eFITCmdStr(m_What),
          part2,part2,"##################");
        }
      #endif
      #endif
      }
    ~CThreadTimer ()
      {
      #if (DOTHREADTIMERALL || DOTHREADTIMERTOTAL)
      if (!m_bComplete)
        Complete();
      #endif
      }

    inline void StartLoop(FlwNode * pNd)
      {
      #if DOTHREADSHOWNODES 
      m_pNd=pNd;
      #endif
      #if (DOTHREADTIMERALL)
      m_pNd=pNd;
      double t=m_Total.Secs();
      m_WaitTotal+=t-m_Memory;//m_SW.Lap();
      m_Memory=t;
      m_iDone=0;
      m_nCalls++;
      #elif (DOTHREADTIMERTOTAL)
      m_nCalls++;
      #endif
      #if DOTHREADSHOWNODES 
      #if dbgFlwSolve
      if (dbgThreadsNodes())
        {
        //dbglock(); // matched in LockDone()
        }
      #endif
      #endif
      }

    inline void LockDone()
      {
      #if DOTHREADSHOWNODES 
      #if dbgFlwSolve
      if (dbgThreadsNodes())
        {
        dbgpln("%-18s%*s-->   %8s   %s", eFITCmdStr(m_What), CFlwSlvThreads::dbgIndent(GetCurrentThreadId()), "", "", m_pNd->Tag());
        //dbgunlock(); // matched in StartLoop()
        }
      #endif
      #endif
      #if (DOTHREADTIMERALL)
      double t=m_Total.Secs();
      m_LockTotal+=t-m_Memory;//m_SW.Lap();
      m_Memory=t;
      m_iDone++;
      if (m_NdSW!=FSW_Null)
        NDSTARTSW(m_pNd, m_NdSW);
      #else if (DOTHREADSHOWNODES)
      m_Memory=m_Total.Secs();
      #endif
      }

    inline void ExecDone()
      {
      #if DOTHREADSHOWNODES 
      #if dbgFlwSolve
      if (dbgThreadsNodes())
        {
        //dbglock();// matched in EndLoop()
        dbgpln("%*s<--   %8.3fus %s", 18+CFlwSlvThreads::dbgIndent(GetCurrentThreadId()), "", 1.0e6*(m_Total.Secs()-m_Memory), m_pNd->Tag());
        }
      #endif
      #endif
      #if (DOTHREADTIMERALL)
      if (m_NdSW!=FSW_Null)
        NDSTOPSW(m_pNd, m_NdSW);
      double t=m_Total.Secs();
      m_ExecTotal+=t-m_Memory;//m_SW.Lap();
      m_Memory=t;
      m_iDone++;
      #endif
      }

    inline void EndLoop()
      {
      #if (DOTHREADTIMERALL)
      double t=m_Total.Secs();
      m_UnLockTotal+=t-m_Memory;//m_SW.Lap();
      m_Memory=t;
      //m_SW.ReStart();
      m_iDone++;
      #endif
      #if DOTHREADSHOWNODES 
      #if dbgFlwSolve
      if (dbgThreadsNodes())
        {
        //dbgunlock(); // matched in ExecDone()
        }
      #endif
      #endif
      }

    inline void Complete()
      {
      #if (DOTHREADTIMERTOTAL || DOTHREADTIMERALL)
      m_Total.Stop();
      #if (DOTHREADTIMERALL)
      double t=m_Total.Secs();
      m_WaitTotal+=t-m_Memory;//m_SW.Lap();
      //m_Memory=t;

      if (m_nCalls>0 && m_iDone!=3)
        dbgpln("%-20.20s [%4i]----- TIMING PORTIONS MISSING");

      CThreadTimerAccum *pAcc=gs_ThreadTiming.GetById(GetCurrentThreadId(), m_What);
      pAcc->Add(*this);
      #endif

      m_bComplete=true;
      #if dbgFlwSolve
      if (dbgThreadsTiming())
        {
        #if (DOTHREADTIMERALL)
        dbgpln("[%4i]%-20.20s Loops:%4i T:%8.3fms L:%7.3fms X:%8.3fms U:%7.3fms O:%8.3fms L:%3i%% X:%3i%% U:%3i%% O:%3i%% ------ ",
          GetCurrentThreadId(),
          eFITCmdStr(m_What),
          m_nCalls,
          1e3*m_Total.Secs(),
          1e3*m_LockTotal,
          1e3*m_ExecTotal,
          1e3*m_UnLockTotal,
          1e3*m_WaitTotal,
          long(100*m_LockTotal/GTZ(m_LockTotal+m_ExecTotal+m_UnLockTotal+m_WaitTotal)),
          long(100*m_ExecTotal/GTZ(m_LockTotal+m_ExecTotal+m_UnLockTotal+m_WaitTotal)),
          long(100*m_UnLockTotal/GTZ(m_LockTotal+m_ExecTotal+m_UnLockTotal+m_WaitTotal)),
          long(100*m_WaitTotal/GTZ(m_LockTotal+m_ExecTotal+m_UnLockTotal+m_WaitTotal)));
        #elif (DOTHREADTIMERTOTAL)
          dbgpln("   [%4i]%-20.20s Time:%10.3fms       %10s Exec:%10.1fus",
            GetCurrentThreadId(), eFITCmdStr(m_What),
            1e3*gs_ExecIterTime.Secs(),
            "",
            1e6*m_Total.Secs());
        #endif
        }
      #endif
      #endif
      #if DOTHREADSHOWNODES 
      #if dbgFlwSolve
      if (dbgThreadsNodes())
        {
        int part1=CFlwSlvThreads::sm_ThreadSpacing;
        dbgpln("%*s%*.*s", 18+CFlwSlvThreads::dbgIndent(GetCurrentThreadId()), "",   
          part1,part1,"###############################");
        }
      #endif
      #endif
      }
  protected:
    FlwNode     * m_pNd;
    int           m_What;
    int           m_NdSW;
    CStopWatch    m_Total;
    //CStopWatch    m_SW;
    double        m_Memory;
    double        m_WaitTotal;
    double        m_LockTotal;
    double        m_ExecTotal;
    double        m_UnLockTotal;
    bool          m_bComplete;
    int           m_iDone;
    int           m_nCalls;

  };

//---------------------------------------------------------------------------

void CThreadTimerAccum::Add(CThreadTimer & T)
  {
  m_Sect.Lock();
  m_Total       += T.m_Total.Secs();
  m_WaitTotal   += T.m_WaitTotal;
  m_LockTotal   += T.m_LockTotal;
  m_ExecTotal   += T.m_ExecTotal;
  m_UnLockTotal += T.m_UnLockTotal;
  m_nCalls      += T.m_nCalls;
  m_nCallSets   += 1;
  m_Sect.Unlock();
  }

void CThreadTiming::Dump()
  {
  return;

  dbgpln("CThreadTiming::Dump");
  for (int w=0; w<eFITMaxCmd; w++)
    {
    int nT=ThreadCount();
    for (int t=0; t<nT; t++)
      {
      CThreadTimerAccum &A =*GetByIndex(t, w);
      if (A.m_nCallSets==0)
        continue;
      dbgpln("%-20.20s[%i] Id=%i", eFITCmdStr(w), t, m_ThreadIds[t]);
      dbgpln("     CallSets     %i",    A.m_nCallSets);
      dbgpln("     Calls        %i",    A.m_nCalls);
      dbgpln("     Total        %8.3f", 1e6*A.m_Total);
      dbgpln("     LockTotal    %8.3f", 1e6*A.m_LockTotal);
      dbgpln("     ExecTotal    %8.3f", 1e6*A.m_ExecTotal);
      dbgpln("     UnLockTotal  %8.3f", 1e6*A.m_UnLockTotal);
      dbgpln("     WaitTotal    %8.3f", 1e6*A.m_WaitTotal);
      }
    }
  };

//---------------------------------------------------------------------------

const bool FITVisible[]=
  {
  false,    //eFITNull                 = 0;
  false,    //eFITBase                 = 1; eFITQuit                 = eFITBase+0;
  true,     //eFITEvalDiscrete         = eFITBase+1;
  true,     //eFITEvalAudit            = eFITBase+2;
  true,     //eFITBuildCtrlNdList      = eFITBase+3;
  false,    //eFITEvalCtrlInitialise   = eFITBase+4;
  true,     //eFITEvalCtrlActions      = eFITBase+5;
  true,     //eFITEvalCtrlStrategy     = eFITBase+6;
  false,    //eFITEvalCtrlTerminate    = eFITBase+7;
  true,     //eFITEvalStatistics       = eFITBase+8;
  true,     //eFITEvalTerminate        = eFITBase+9;
  true,     //eFITConfigureJoins       = eFITBase+10;
  true,     //eFITElectrics            = eFITBase+11;
  true,     //eFITPowerRequired        = eFITBase+12;
  true,     //eFITStates               = eFITBase+13;
  true,     //eFITEvalSteadyState      = eFITBase+14;
  true,     //eFITStartStep            = eFITBase+15;
  true,     //eFITFlowInfo             = eFITBase+16;
  true,     //eFITEvalProductsSurge    = eFITBase+17;
  true ,    //eFITEvalProducts         = eFITBase+18;
  true,     //eFITEvalDerivs           = eFITBase+19;
  true,     //eFITEvalIntegral         = eFITBase+20;
  true,     //eFITODEOperate           = eFITBase+21
  false,    //                                    22
  false,    //                                    23
  false,    //                                    24
  true,     //eFITGrpMeasure           = eFITBase+25;
  true,     //eFITGrpTraverse          = eFITBase+26;
  true,     //eFITGrpConverge          = eFITBase+27;
  false,    //                                    28
  false,    //                                    29
  false,    //eFITPBEvalMakeUpAvail    = eFITBase+30;
  false,    //eFITPBEvalMakeUpReqd     = eFITBase+31;
  false,    //eFITPBEvalPressures      = eFITBase+32;
  false,    //eFITPBEvalProducts       = eFITBase+33;
  false,    //eFITPBEvalDiscrete       = eFITBase+34;
  false,    //eFITPBEvalAudit        = eFITBase+35;
  false,    //eFITPBEvalCtrlInitialise = eFITBase+36;
  false,    //eFITPBEvalCtrlActions    = eFITBase+37;
  false,    //eFITPBEvalCtrlStrategy   = eFITBase+38;
  false,    //eFITPBEvalCtrlTerminate  = eFITBase+39;
  false,    //                                    40
  false,    //                                    41
  false,    //                                    42
  false,    //eFITMaxCmd               = eFITBase+43;
  false,
  false,
  false,
  };

//===========================================================================
//
//
//
//===========================================================================

static CGlblStopWatch  gs_swCtrlOverHead    (gs_swFlwSolve, "Slv", "CtrlOverHead",  1, (CGlblStopWatch::eShowDbg|CGlblStopWatch::eShowAccess|CGlblStopWatch::eShowTotal));
static CGlblStopWatch  gs_swPowerRequired   (gs_swFlwSolve, "Slv", "PowerRequired", 2);
static CGlblStopWatch  gs_swElectrics       (gs_swFlwSolve, "Slv", "Electrics",     3);
static CGlblStopWatch  gs_swCtrlActions     (gs_swFlwSolve, "Slv", "CtrlActions",   4);
static CGlblStopWatch  gs_swCtrlStrategy    (gs_swFlwSolve, "Slv", "CtrlStrategy",  5);
static CGlblStopWatch  gs_swConfigJoins     (gs_swFlwSolve, "Slv", "ConfigJoins",   6);
static CGlblStopWatch  gs_swNetworkSolve    (gs_swFlwSolve, "Slv", "NetworkSolve",  7);
static CGlblStopWatch  gs_swModelSolve      (gs_swFlwSolve, "Slv", "ModelSolve",    8);
static CGlblStopWatch  gs_swDiscrete        (gs_swFlwSolve, "Slv", "Discrete",      9);
static CGlblStopWatch  gs_swBalance         (gs_swFlwSolve, "Slv", "Balance",      10);
static CGlblStopWatch  gs_swChangeTrace     (gs_swFlwSolve, "Slv", "ChangeTrace",  11);

//===========================================================================
//
//
//
//===========================================================================

void GlblTopyChgFun(dword Data)
  {
  ((CFlwSolver*)Data)->UnDoGlobalLinks();
  };

//===========================================================================
//
//
//
//===========================================================================

flag CollectGrpNodes(CMacroMdlClassDef *pC, CMacroMdl *pG, FlwNode *p, int IONo, flag StartNd)
  {
  #if dbgFlwSolve
  if (dbgMacroMdls1())
    dbgpln("Collect        :                %s", p->FullObjTag());
  #endif
  p->Busy=1;
  CMacroMdlIO * pMMIO=pG->ValidNd(mmio_MODEL, p, IONo);
  flag GotOne=0;
  if (pMMIO)
    {
    if (0)
      {
      LogError(p->FullObjTag(), 0, "Bad Flow Node groups ");
      return false;
      }
  #if dbgFlwSolve
    if (dbgMacroMdls())
      dbgpln("Valid  : %8x %2i %s", pG, IONo, p->FullObjTag());
  #endif

    pG->AddNd(p, IONo, pMMIO);
    p->Done=1;

    // Go Down All the IO's and try to connect
    for (int i=0; i<p->NoFlwIOs(); i++)
      if ((i==IONo) || pG->ValidNd(mmio_MODEL, p, i))
        {
        FlwNode * pR=p->Nd_Rmt(i);
        if (!pR->Busy && !pR->Done)
          CollectGrpNodes(pC, pG, pR, p->IOIONo_Rmt(i), false);
        }

    GotOne=1;
    }
  else
    {
    pMMIO=pG->ValidNd(mmio_CONNECT, p, IONo);
    if (pMMIO)
      {
      flag GotSome=0;
      for (int i=0; i<p->NoFlwIOs(); i++)
        if ((i==IONo) || pG->ValidNd(mmio_CONNECT, p, i))
          {
          FlwNode * pR=p->Nd_Rmt(i);
#if dbgFlwSolve
          if (dbgMacroMdls1())
            dbgpln("     ..: %2i %i%i %s >> %s", i, pR->Busy, pR->Done, p->FullObjTag(), pR->FullObjTag());
#endif
          if (!pR->Busy && !pR->Done)
            if (CollectGrpNodes(pC, pG, pR, p->IOIONo_Rmt(i), false))
              GotSome=true;
          }
      if (GotSome)
        {
        pG->AddNd(p, IONo, pMMIO);
#if dbgFlwSolve
        if (dbgMacroMdls())
          dbgpln("Connect: %8x %2i %s", pG, IONo, p->FullObjTag());
#endif
        GotOne=1;
        }
      p->Done=1;
      }
    }
#if dbgFlwSolve
  if (dbgMacroMdls())
    if (!GotOne)
      dbgpln("       :                %s", p->FullObjTag());
#endif
  p->Busy=0;
  return GotOne;
  }

//===========================================================================
//
//
//
//===========================================================================

FlwSlvBlk::FlwSlvBlk(CFlwSolver * pFlwSlvNd) : \
FlwSlvNd(*pFlwSlvNd),
m_ProcTOList1(m_TOList[0]),
m_ProcTOList2(m_TOList[1]),
m_CtrlTOList1(m_TOList[2]),
m_CtrlTOList2(m_TOList[3])
  {
  m_SolutionBlksOk=0;
  m_fProcTOSeqValid=0;
  m_fCtrlTOSeqValid=0;

  m_pProcTOSequence = new CNodeEvalIndexList;
  m_pProcTOSequencePrv = new CNodeEvalIndexList;
  m_pCtrlTOSequence = new CNodeEvalIndexList;
  m_pCtrlTOSequencePrv = new CNodeEvalIndexList;
  m_pCombTOSequence = new CNodeEvalIndexList;
  m_pCombTOSequencePrv = new CNodeEvalIndexList;

  m_SFGInsStack.SetSize(0,256);
  m_SFGOutsStack.SetSize(0,256);
  }

//--------------------------------------------------------------------------

FlwSlvBlk::~FlwSlvBlk()
  {
  delete m_pProcTOSequence;
  delete m_pProcTOSequencePrv;
  delete m_pCtrlTOSequence;
  delete m_pCtrlTOSequencePrv;
  delete m_pCombTOSequence;
  delete m_pCombTOSequencePrv;
  }

//--------------------------------------------------------------------------

void FlwSlvBlk::BuildDataDefn(DataDefnBlk & DDB)
  {
  };

//--------------------------------------------------------------------------

flag FlwSlvBlk::DataXchg(DataChangeBlk & DCB)
  {

  switch (DCB.lHandle)
    {
    case xidNoOfTears:
      DCB.L=m_TearFlanges.GetCount();
      return 1;
    default:
      //if (DCB.lHandle>=xidTear0 && DCB.lHandle<xidTear0+TearSrcNodes.GetCount())
      //  {
      //  POSITION pos1=TearSrcNodes.FindIndex(DCB.lHandle-xidTear0);
      //  POSITION pos2=TearDstNodes.FindIndex(DCB.lHandle-xidTear0);
      //  if (pos1 && pos2)
      //    {
      //    FlwNode * p1=TearSrcNodes.GetAt(pos1);
      //    FlwNode * p2=TearDstNodes.GetAt(pos2);
      //    static Strng S;
      //    S="?";
      //    if (p1 && p2)
      //      S.Concat(p1->FullObjTag(), "/", p2->FullObjTag());
      //    DCB.pC=S();
      //    return 1;
      //    }
      //
      //  }
      ;
    }

  return 0;
  }

//--------------------------------------------------------------------------

flag FlwSlvBlk::CreateSolutionBlks(int NRqdThreads)
  {
  ClearSolutionBlks(false);

  CFlwNodeIter All(m_AllNds);
  for (FlwNode * p=All.First(); (p); p=All.Next())
    p->fSolutionBusy=true;

  m_fProcTOSeqValid=0;
  m_fCtrlTOSeqValid=0;

  ASSERT(m_ProcessNds.GetCount()==0);
  ASSERT(m_PwrCtrlNds.GetCount()==0);
  ASSERT(m_StandAloneNds.GetCount()==0);
  ASSERT(m_Flanges.GetCount()==0);
  ASSERT(m_Terminals.GetCount()==0);

  if (InitialiseSolutionAll())
    {
    CFlwNodeIter All(m_AllNds);
    for (FlwNode * p=All.First(); (p); p=All.Next())
      {
      p->NetNo=-1;
      p->SetNodeTypeFlags();
      p->m_InPwrCtrlSeq=false;
      }

    // FOrce all 'Audit' node to the end
    for (p=All.First(); (p); p=All.Next())
      {
      if ((p->m_dwType & nc_Audit)==0)
        {
        if (p->IsProcessSeq())
          m_ProcessNds.AddTail(p);
        if (p->m_dwType & nc_StandAlone)
          m_StandAloneNds.AddTail(p);
        }
      }

    for (p=All.First(); (p); p=All.Next())
      {
      if ((p->m_dwType & nc_Audit)!=0)
        {
        if (p->IsProcessSeq())
          m_ProcessNds.AddTail(p);
        if (p->m_dwType & nc_StandAlone)
          m_StandAloneNds.AddTail(p);
        }
      }

    for (p = All.First(); (p); p = All.Next())
      {
      if (p->IsLnk())
        {
        CFlange* pF=p->IOFlange(0);
        if (!pF->fInFlangeList)
          {
          m_Flanges.AddTail(pF);
          pF->fInFlangeList=true;
          // Clear System Tears in Case Left Lying around
          pF->SetTearType(TT_NoTear, NULL);
          }
        pF=p->IOFlange(1);
        if (!pF->fInFlangeList)
          {
          m_Flanges.AddTail(pF);
          pF->fInFlangeList=true;
          // Clear System Tears in Case Left Lying around
          pF->SetTearType(TT_NoTear, NULL);
          }
        }
      }
    // Check for flanges between nodes which are not links.
    for (p = All.First(); (p); p = All.Next())
      {
      if (!p->IsLnk())
        {
        for (int i=0; i<p->NoFlwIOs(); i++)
          {
          CFlange* pF=p->IOFlange(i);
          if (!pF->fInFlangeList)
            {
            m_Flanges.AddTail(pF);
            pF->fInFlangeList=true;
            //pF->SetConnection(p, i, p->Nd_Rmt(i), p->IOIONo_Rmt(i));
            // Clear System Tears in Case Left Lying around
            pF->SetTearType(TT_NoTear, NULL);
            }
          }
        }
      }

    // split Nodes into separate lists for each thread
    int NThreads=FSTC.NThreads();
    for (int it=0; it<m_ProcessNdsLA.GetSize(); it++)
      m_ProcessNdsLA[it].RemoveAll();
    m_ProcessNdsLA.SetSize(NThreads);

    if (NThreads>0)
      {
      int it=0;
      CFlwNodeIter Proc(m_ProcessNds);
      for (p = Proc.First(); (p); p = Proc.Next())
        m_ProcessNdsLA[((it++)%NThreads)].AddTail(p);
      }

    return true;
    }

  ClearSolutionBlks(false);
  return false;
  }

//--------------------------------------------------------------------------

int FlwSlvBlk::ClearSolutionBlks(flag ClearAll)
  {
  FlwSlvNd.StructureChanged(NULL);
  /*
#ifdef _DEBUG
  dbgpln("FlwNetMemory PreClear Tot:%8li  Cur:%8li  Max:%8li",
                                AfxGetAllocState()->m_lTotalAlloc,
                                AfxGetAllocState()->m_lCurAlloc,
                                AfxGetAllocState()->m_lMaxAlloc);
#endif
  */
  m_SolutionBlksOk=0;

  CFlwNodeIter All(m_AllNds);
  for (FlwNode * p=All.First(); (p); p=All.Next())
    p->fSolutionBusy=false;

  m_ExcNds.RemoveAllToFreeList();
  m_ProcessNds.RemoveAllToFreeList();
  m_PwrCtrlNds.RemoveAllToFreeList();
  m_StandAloneNds.RemoveAllToFreeList();

  CFlangeIter F(m_Flanges);
  for (CFlange* pF = F.First(); (pF); pF=F.Next())
    pF->fInFlangeList=false;
  m_Flanges.RemoveAllToFreeList();
  m_TearFlanges.RemoveAllToFreeList();

  CTerminalIter T(m_Terminals);
  for (CTerminal* pT = T.First(); (pT); pT=T.Next())
    pT->m_fInTerminalList=false;
  m_Terminals.RemoveAllToFreeList();

  m_TearTerminals.RemoveAllToFreeList();
  m_TearXRefs.RemoveAllToFreeList();

  // Dont Clear these 2 lists - to enable Flwsolve to view Tears once Stopped
  //TearSrcNodes.RemoveAllToFreeList();
  //TearDstNodes.RemoveAllToFreeList();

  TerminateSolutionAll();

  SwapAndClearTOSequence(TOSL_All);

  if (ClearAll)
    {
    m_AllNds.RemoveAllToFreeList();
    ClearTOSequence(TOSL_All, true);
    m_fProcTOSeqValid=0;
    m_fCtrlTOSeqValid=0;
    FlwSlvNd.StructureChanged(&FlwSlvNd);
    DeleteMacroMdls();

    //TearSrcNodes.RemoveAllToFreeList();
    //TearDstNodes.RemoveAllToFreeList();
    }

  /*
#ifdef _DEBUG
  dbgpln("FlwNetMemory Cleared  Tot:%8li  Cur:%8li  Max:%8li",
                                AfxGetAllocState()->m_lTotalAlloc,
                                AfxGetAllocState()->m_lCurAlloc,
                                AfxGetAllocState()->m_lMaxAlloc);
#endif
  */
  return 0;
  }

//--------------------------------------------------------------------------

void FlwSlvBlk::BuildCtrlNdList()
  {
  dbgpln("BuildCtrlNdList===========================================================");

  m_PwrCtrlNds.RemoveAllToFreeList();

  CTerminalIter T(m_Terminals);
  for (CTerminal* pT = T.First(); (pT); pT=T.Next())
    pT->m_fInTerminalList=false;
  m_Terminals.RemoveAllToFreeList();

  m_TearTerminals.RemoveAllToFreeList();
  m_TearXRefs.RemoveAllToFreeList();
  
  ClearTOSequence(TOSL_Ctrl|TOSL_Comb);
  m_fCtrlTOSeqValid=0;

  //ASSERT(m_ProcessNds.GetCount()==0);
  ASSERT(m_PwrCtrlNds.GetCount()==0);
  //ASSERT(m_StandAloneNds.GetCount()==0);
  //ASSERT(m_Flanges.GetCount()==0);
  ASSERT(m_Terminals.GetCount()==0);

  CFlwNodeIter All(m_AllNds);
  for (FlwNode * p=All.First(); (p); p=All.Next())
    {
    p->m_InPwrCtrlSeq=false;
    }

  for (FlwNode * p=All.First(); (p); p=All.Next())
    //{
    //if (p->UpdateXRefListsReqd())
    p->CNodeXRefMngr::UpdateXRefLists(false);
    //}

  #if dbgFlwSolve
  if (dbgXRefsBuild())
    {
    dbgsep();
    dbgpln("BuildCtrlNdList");
    dbgpln("                                                         N=Near F=Far S=Set G=Get T=Tear s=SeqStart");
    dbgpln("          Tag                         SrcTag                                            DstTag");
    }
  #endif
  for (p=All.First(); (p); p=All.Next())
    {
    if (!p->IsProcessSeq() || p->m_XRefsOwnedByMe[XR_Near].GetSize()>0 || p->NoCtrls()>0)
      {
      p->m_InPwrCtrlSeq=true;
      m_PwrCtrlNds.AddTail(p);
      }

    #if dbgFlwSolve
    if (dbgXRefsBuild())
      {
      for (int iRef=0; iRef<p->m_XRefsOwnedByMe[XR_Near].GetSize(); iRef++)
        {
        CXRefItem *pRef=p->m_XRefsOwnedByMe[XR_Near][iRef];
        FlwNode *pSrc=dynamic_cast<FlwNode*>(pRef->SrcNd());
        FlwNode *pDst=dynamic_cast<FlwNode*>(pRef->DstNd());
        if (iRef==0)
          dbgp("NearXRef: %s%s %-20s %-40s",
               p->IsProcessSeq()?"P":" ",
               p->InPwrCtrlSeq()?"C":" ",
               p->FullObjTag(),
               pRef->SFGNodeTag(p));
        else
          dbgp("        : %s%s %-20s %-40s",
               " " , " ", " \"", pRef->SFGNodeTag(p));
        static LPCTSTR XX[]={"N", "F", "L", "?"};
        dbgp("  %s %s %s", XX[pRef->m_eReach], pRef->m_bMustGet?"G":" ",pRef->m_bMustSet?"S":" ");
        dbgp(" %s %s", pRef->m_bIsTear?"T":" ", pRef->m_bIsEvalSeqStart?"s":" ");
        if (pSrc && pDst)
          {
          dbgp(" [%s] %-20s >> [%s] %-20s ",
            pSrc->InPwrCtrlSeq()?"Ctrl":"    ",
            pSrc->FullObjTag(),
            pDst->InPwrCtrlSeq()?"Ctrl":"    ",
            pDst->FullObjTag());
          }
        else
          {
          dbgp(" [%s] %-20s >> [%s] %-20s ",
            pSrc?(pSrc->InPwrCtrlSeq()?"Ctrl":"    "):"NULL",
            pSrc?pSrc->FullObjTag():"",
            pDst?(pDst->InPwrCtrlSeq()?"Ctrl":"    "):"NULL",
            pDst?pDst->FullObjTag():"");
          }
        dbgp(" %-20s", pRef->SFGNodeTag(p));
        dbgpln("");
        }
      }
    #endif
    }

  for (p = All.First(); (p); p = All.Next())
    {
    if (p->IsCLnk())
      {
      CTerminal* pT=p->Ctrl_Terminal(0);
      if (!pT->m_fInTerminalList)
        {
        m_Terminals.AddTail(pT);
        pT->m_fInTerminalList=true;
        //// Clear System Tears in Case Left Lying around
        //pT->SetTearType(TT_NoTear);
        }
      pT=p->Ctrl_Terminal(1);
      if (!pT->m_fInTerminalList)
        {
        m_Terminals.AddTail(pT);
        pT->m_fInTerminalList=true;
        //// Clear System Tears in Case Left Lying around
        //pT->SetTearType(TT_NoTear);
        }
      }
    }
  }

//--------------------------------------------------------------------------

flag FlwSlvBlk::PreStartCheckAll()
  {
  int nBad=0;//flag Ok=1;
#if dbgTestConnectivity
  dbgpln("%8s %3s %4s %6s %6s %7s %14s %8s",
         "Rmt", "fIn", "Flgs", "DscSlf", "DscRmt", "IONoRmt", "P", "Conn");
#endif

  ConditionBlk::SetSeqNoAtRunStart();

  CFlwNodeIter All(m_AllNds);
  for (FlwNode * p=All.First(); (p); p=All.Next())
    {
    p->ClrAllCIs();
    #if dbgFlwSolve
    if (dbgXRefsBuild())// && p->m_bXRefsValid)
      dbgpln("PreStart:UnlinkAllXRefs ------------------------------------------------ %s", p->GetOwnerTag());
    #endif
    p->UnlinkAllXRefs();
    p->RequestUpdateXRefLists();
    }

  TIMINGLAP("PreStart:XRefs");

  for (FlwNode * p=All.First(); (p); p=All.Next())
    {
    flag Ok=1;

#if dbgTestConnectivity
    dbgpln("%60s %s", "", p->FullObjTag());
#endif

#if TimePrestart
CStopWatch SW;
SW.Start();
#endif
    if (!p->PreStartCheck())
      {
      p->SetCI(16);
      Ok=0;
      }
#if TimePrestart
dbgpln("  %16.2f %-15s %s", SW.LapTime()*1e6, p->ClassId(), p->FullObjTag());
#endif

    p->m_fValidateDataComplete=false;
    ValidateDataBlk VDB;
    if (!p->ValidateDefinedData(VDB))
      {
      p->SetCI(17);
      Ok=0;
      }
    else
      p->ClrCI(17);
    if (!p->m_fValidateDataComplete && !p->bHoldValidateData)
      LogError(p->FullObjTag(), 0, "Incomplete ValidateData");

    for (int i=0; i<p->NoFlwIOs(); i++)
      {
      dword md1=p->SolveMethod();
      dword md2=p->Nd_Rmt(i)->SolveMethod();
      flag MdOk=0;

      if (p->NetProbalMethod())
        {
        if (md1 & md2 & SM_Direct)
          MdOk=1;
        }
      else
        {
        if ((md1 & SM_DynBoth|SM_Direct) && (md2 & SM_DynBoth|SM_Direct))
          MdOk=1;
        }
      //if (md1 & md2 & SM_Direct)
      //  MdOk=1;
      //else if ((md1 & SM_DynBoth) && (md2 & SM_DynBoth))
      //  MdOk=1;

      if (!MdOk)
        {
        LogError(p->FullObjTag(), 0, "Bad SolveMethod @ %s", p->Nd_Rmt(i)->FullObjTag());
        Ok=0;
        }

      int ir=p->IOIONo_Rmt(i);
      if (p->Nd_Rmt(i)->Nd_Rmt(ir)!=p)
        {
        LogError("Solver", 0, "Bad Connection %s Connects to %s & %s",
           p->Nd_Rmt(i)->FullObjTag(),
           p->FullObjTag(),
           p->Nd_Rmt(i)->Nd_Rmt(ir)->FullObjTag());

        Ok=0;
        }

#if dbgTestConnectivity
      CIORec & io=p->IOs[i];
      dbgpln("%8x %6i %6i %7i %-10s  %s",
             io.Rmt, io.m_iIODescSelf, io.m_iIODescRmt, io.m_iIONoRmt,
             p->IODesc_Self(i)->pName, p->Nd_Rmt(i)->FullObjTag());
#endif
      }
    if (!Ok)
      nBad++;
    }
  TIMINGLAP("PreStart:Other");
  return nBad==0;
  }

//--------------------------------------------------------------------------

flag FlwSlvBlk::InitialiseSolutionAll()
  {
  //TargetTags.InitialiseSolution();
  FlwSlvNd.ClrCI(1);

  flag OK=true;
  CFlwNodeIter All(m_AllNds);
  for (FlwNode * p=All.First(); (p); p=All.Next())
    {
    p->BeforeNode();
    if (!p->InitialiseSolution())
      OK=false;
    p->AfterNode(NPM_InitialiseSolution);
    }
  return OK;
  }

//--------------------------------------------------------------------------

flag FlwSlvBlk::TerminateSolutionAll()
  {
  CFlwNodeIter All(m_AllNds);
  for (FlwNode * p=All.First(); (p); p=All.Next())
    {
    p->BeforeNode(NPM_TerminateSolution);
    p->TerminateSolution();
    p->AfterNode();
    }
  //TargetTags.TerminateSolution();
  return true;
  }

//--------------------------------------------------------------------------

void FlwSlvBlk::StartSolutionAll()
  {
  CFlwNodeIter All(m_AllNds);
  for (FlwNode * p=All.First(); (p); p=All.Next())
    {
    p->StartSolution();
    p->InitFlowInfo();
    
    p->m_AuditOptionRmt=Audit_Ignore;
    for (int j=0; j<p->NoFlwIOs(); j++)
      {
      p->m_AuditOptionRmt=Max(p->m_AuditOptionRmt, p->Nd_Rmt(j)->m_AuditOption);
      }
    
    }
  TIMINGLAP("Start Solution");

  }

//---------------------------------------------------------------------------
                      
void FlwSlvBlk::FixAllNdsLists()
  {
  int NThreads=FSTC.NThreads();
  for (int it=0; it<m_AllNdsLA.GetSize(); it++)
    m_AllNdsLA[it].RemoveAll();
  m_AllNdsLA.SetSize(NThreads);

  if (NThreads>0)
    {
    CFlwNodeIter Proc(m_AllNds);
    int it=0;
    for (FlwNode * p = Proc.First(); (p); p = Proc.Next())
      m_AllNdsLA[((it++)%NThreads)].AddTail(p);
    }
  };

//---------------------------------------------------------------------------

void FlwSlvBlk::FITConfigureJoins(CFlwNodeList & List)//CLkFlwNodeIter & Iter)
  {
  CThreadTimer  XT(eFITConfigureJoins, FSW_ConfigureJoins);
  FlwNode * p;
  //while (Iter.GetNext(p))
  CFlwNodeIter Iter(List);
  for (p=Iter.First(); p; p=Iter.Next())
    {
    //XT.StartLoop(p);
    //p->LockFlanges();
    p->LockNode();
    //XT.LockDone();//(p, eFITConfigureJoins, FSW_ConfigureJoins);
    p->PreConfigureJoins();
    p->ConfigureJoins();
    p->PostConfigureJoins();
    //XT.ExecDone();//Postamble(p, eFITConfigureJoins, FSW_ConfigureJoins);
    //p->UnLockFlanges();
    p->UnLockNode();
    //XT.EndLoop();//PostambleUnLock();
    }
  XT.Complete();//PostambleTl();
  }

//--------------------------------------------------------------------------

void FlwSlvBlk::ConfigureJoinsAll()
  {
  CStopWatchLap SWLapConfigJoins(gs_swConfigJoins);
  FlwSlvNd.fInConfigureJoins=1;
  if (FSTC.NThreads())
    {
    FSTC.DoJob(eFITConfigureJoins, this, &m_ProcessNdsLA);//TCFlw);
    }
  else
    {
    CThreadTimer  XT(eFITConfigureJoins, FSW_ConfigureJoins);
    CFlwNodeIter Flw(m_ProcessNds);
    for (FlwNode * p=Flw.First(); (p); p=Flw.Next())
      {
      XT.StartLoop(p);
      XT.LockDone();//(p, eFITConfigureJoins, FSW_ConfigureJoins);
      p->PreConfigureJoins();
      p->ConfigureJoins();
      p->PostConfigureJoins();
      XT.ExecDone();//Postamble(p, eFITConfigureJoins, FSW_ConfigureJoins);
      XT.EndLoop();
      }
    XT.Complete();//PostambleTl();
    }
  FlwSlvNd.fInConfigureJoins=0;
  }

//--------------------------------------------------------------------------

flag FlwSlvBlk::BuildMacroMdls()
  {
  flag OK=true;
  DeleteMacroMdls();

  dbgpln("BuildMacroMdls --");

  for (FlwNode * p = NULL; FlwSlvNd.GetObj_All(p); )
    p->Busy=p->Done=p->Break=0;

  CMacroMdlClassDef *pC=(CMacroMdlClassDef*)TagObjClass::FindGroupId("CMacroMdl");
  while (pC)
    {
    // Test that it is not the base class
    if (pC!=&CMacroMdlClass)
      {
      CMacroMdl *pG=NULL;
      for (FlwNode * p = NULL; FlwSlvNd.GetObj_All(p); )
        if (!p->Done && !p->Busy)
          {
          if (pG==NULL)
            pG=(CMacroMdl*)pC->Construct(NULL, "ATAG", &FlwSlvNd, TOA_Embedded);
          // Is this a Candidate for a CMacroMdl ?
          if (pG->ValidNd(mmio_MODEL, p, -1))
            {
#if dbgFlwSolve
            if (dbgMacroMdls())
              dbgpln("CMacroMdl : %s", pC->ClassId());
#endif
            for (int j=0; pG && (j<p->NoFlwIOs()); j++)
              {
              // Are any of the Required IO Pts connected and hence to a Matching Model ?
              if (pG->ValidNd(mmio_MODEL, p, j))
                {
                if (!CollectGrpNodes(pC, pG, p, j, true))
                  OK=false;
                if (pG->ValidModel())
                  {
                  MacroMdls.AddTail(pG);
#if dbgFlwSolve
                  if (dbgMacroMdls                  ())
                    dbgpln("CMacroMdl : -- Valid");
#endif
                  }
                else
                  {
                  delete pG;
#if dbgFlwSolve
                  if (dbgMacroMdls())
                    dbgpln("CMacroMdl : -- INVALID");
#endif
                  }
                pG=NULL;
                }
              }
            }
          }
      delete pG;
      pG=NULL;
      }
    pC=(CMacroMdlClassDef*)pC->NextClassInGrp();
    }
  MacroMdlIter MMs(MacroMdls);
  for (CMacroMdl* pMM=MMs.First(); pMM; pMM=MMs.Next())
    if (!pMM->PBInitialise())
      OK=0;
  return OK;
  };

//--------------------------------------------------------------------------

void FlwSlvBlk::DeleteMacroMdls()
  {
  if (MacroMdls.GetCount()>0)
    {
    dbgpln("DeleteMacroMdls ---");

    MacroMdlIter MMs(MacroMdls);
    for (CMacroMdl* pMM=MMs.First(); pMM; pMM=MMs.Next())
      pMM->PBTerminate();

    for (pMM=MMs.First(); pMM; pMM=MMs.Next())
      delete pMM;
    MacroMdls.RemoveAll();
    }
  };

//--------------------------------------------------------------------------

void SetUpFlwNodeEdges(FlwNode * p, long JoinMask, flag DoingDynMode, CSFGIOStack & pIns, CSFGIOStack & pOuts, int & iIns, int & iOuts)
  {
  for (int i = 0; i < p->NoProcessIOs(); i++)
    {
    CFlange *pF=p->IOFlange(i);
    long IOJoinMask = (1 << p->IO_Join_Id(i));
    if ((IOJoinMask & JoinMask)==0)
      goto Continue;

    if (pF->RqdTearType()>=TT_ManualTear)
      goto Continue;

    if (!DoingDynMode || !p->IO_EvalSeqStartOK(i))
      {
      if (p->IOEvalSeq_In(i))
        pIns.SetAtGrow(iIns++, pF);
      else if (p->IOEvalSeq_Out(i))
        pOuts.SetAtGrow(iOuts++, pF);
      //ASSERT_ALWAYS(iIns>=MaxEdgeNds, "Insufficient MaxEdgeNds", __FILE__, __LINE__);
      //ASSERT_ALWAYS(iOuts<=MaxEdgeNds, "Insufficient MaxEdgeNds", __FILE__, __LINE__);
      }
    else if (p->IsLnk() && i<p->NoProcLnkIOs())
      {
      if (p->IOEvalSeq_In(i))
        {
        bool OpenNotClosed=!p->IO_Closed_Self(i) && !p->IO_Closed_Rmt(i) && (p->IO_Open_Self(i) || p->IO_Open_Rmt(i));
        if ((OpenNotClosed && !p->IO_EvalSeqStartDenied(i)) || p->IO_EvalSeqStartOK(i))
          pF->SetIsEvalSeqStart(true);
      //else if (p->GetActiveHold())
      //  pF->SetIsEvalSeqStart(true);
        }
      }

Continue:
    #if dbgFlwSolve
    if (dbgEvalOrdFind())
      {
      double QmEstIn=p->IOQmEst_In(i);
      Strng Me, You;
      Me.Set("%s.%s", p->FullObjTag(), p->IODesc_Self(i)->IOName());
      You=p->Nd_Rmt(i)->FullObjTag();
      dbgpln("SetUpFlwNodeEdges %-3s %-3s %-3s %-6s %-10s %-2s %-3s %20s %5s %30s %s %s",
        DoingDynMode?"Dyn":"",
        p->IsLnk()?"Lnk":"",
        i<p->NoProcLnkIOs()?"MIO":"",
        pF->RqdTearType()?"Manual":"",
        p->IO_EvalSeqStartOK(i)?"StartOK":"",
        p->IOEvalSeq_In(i)?"In":"",
        p->IOEvalSeq_Out(i)?"Out":"",
        DbgFltString(p->IOQmEst_In(i)),
        pF->IsEvalSeqStart()?"START":"",
        Me(),
        (QmEstIn>0 ? "<":QmEstIn<0 ? ">":"-"),
        You()
        );
      }
    #endif

    }
  }

//--------------------------------------------------------------------------

void SetUpFlwNodeEdges(FlwNode * pNd, flag DoingDynMode, CSignalFlowGraph &SigFlwGrf, CSFGIOStack & pIns, CSFGIOStack & pOuts)
  {
  // Connect all Inputs to all Outputs via Edges
  long JoinMask, jj=0;
  while (pNd->EvalProductClusters(jj++, JoinMask))
    {
    //CSFGNodeInfo * pIns[MaxEdgeNds];
    //CSFGNodeInfo * pOuts[MaxEdgeNds];
    int iIns=0, iOuts=0;
    // Collect Parent & Children if Internal
    if (pNd->Internal)
      {
      FlwNode * pParent = pNd->Parent();
      while (pParent->Internal)
        pParent = pParent->Parent();

      FlwNode * p1=NULL;
      while (pParent->GetObj_All(p1))
        SetUpFlwNodeEdges(p1, JoinMask, DoingDynMode, pIns, pOuts, iIns, iOuts);
      pNd=pParent;
      }
    else
      {
      SetUpFlwNodeEdges(pNd, JoinMask, DoingDynMode, pIns, pOuts, iIns, iOuts);
      }

    //if (iIns>=MaxEdgeNds || iOuts>=MaxEdgeNds)
    //  LogError(pNd->FullObjTag(), 0, "Bad number of Tearnodes");
    for (int i = 0; i < iIns; i++)
      for (int j = 0; j < iOuts; j++)
        //if (pIns[i]->RqdTearType()!=TT_ManualTear && pOuts[j]->RqdTearType()!=TT_ManualTear)
        SigFlwGrf.AddEdge(pIns[i], pOuts[j], pNd->FullObjTag());
    }
  }

//--------------------------------------------------------------------------

void SetUpCtrlNodeEdges(FlwNode * p, flag DoingDynMode, CSFGIOStack & pIns, CSFGIOStack & pOuts, int & iIns, int & iOuts)
  {
  for (int i = p->Ctrl1(); i < p->CtrlN(); i++)
    {
    CTerminal *pT=p->Ctrl_Terminal(i);
    //CTerminal *pT=p->Ctrl_Terminal(i);
    if (p->InPwrCtrlSeq() && p->Nd_Rmt(i)->InPwrCtrlSeq())
      {
      if (/*DoingDynMode ||*/!p->Ctrl_EvalSeqStartOK(i))
        {
        if (p->Ctrl_In(i))
          pIns.SetAtGrow(iIns++, pT);
        else if (p->Ctrl_Out(i))
          pOuts.SetAtGrow(iOuts++, pT);
        //ASSERT_ALWAYS(iIns<=MaxEdgeNds, "Insufficient MaxEdgeNds", __FILE__, __LINE__);
        //ASSERT_ALWAYS(iOuts<=MaxEdgeNds, "Insufficient MaxEdgeNds", __FILE__, __LINE__);
        }
      else if (p->IsCLnk())
        {
        if (p->Ctrl_In(i) && p->Ctrl_EvalSeqStartOK(i))
          pT->SetIsEvalSeqStart(true);
        }
      }
    }

  for (i=0; i<p->NoXRefs2Me(); i++)
    {
    CXRefItem *pRef=p->CXRef(i);
    FlwNode *pSrc=p->CXRef_SrcNd(i);
    FlwNode *pDst=p->CXRef_DstNd(i);

    //if (_stricmp(pRef->SFGNodeTag(p), "L_Gro_PT_Feed.QiI.SQMt")==0)
    //  { int xxx=0;};

    #if dbgFlwSolve
    if (dbgXRefsSetup())
      {
      static LPCTSTR XX[]={"Near","Far ","Lcl "," ?? "};
      dbgp("  %8s: %-20s", i==0?"NearXRef":"", i==0?p->FullObjTag():"");
      dbgp(" %s %s %s",
        XX[pRef->m_eReach],
        pRef->MustGet()?"Get":"   ",
        pRef->MustSet()?"Set":"   ");
      if (pSrc && pDst)
        {
        dbgp(" [%s >> %s] %s",
          pSrc->InPwrCtrlSeq()?"Ctrl":"    ",
          pDst->InPwrCtrlSeq()?"Ctrl":"    ",
          (pSrc == pDst) ? "SelfLoop":(pSrc->InPwrCtrlSeq() && pDst->InPwrCtrlSeq())?"AddIO   ":"        ");
        }
      else
        {
        dbgp(" [%s >> %s] %s",
          pSrc?(pSrc->InPwrCtrlSeq()?"Ctrl":"    "):"NULL",
          pDst?(pDst->InPwrCtrlSeq()?"Ctrl":"    "):"NULL",
          "        ");
        }
      dbgp(" %-20s", pRef->SFGNodeTag(p));
      dbgpln("");
      }
    #endif
    // This test would imply that both src and dest are FlwNodes which ius not neccessarily true
    //if (!pSrc || !pDst)
    //  LogWarning(p->FullObjTag(), 0, "Incomplete XRef %s", pRef->m_TAB.sTag());

    if (pSrc && pDst && pSrc->InPwrCtrlSeq() && pDst->InPwrCtrlSeq())
      {
      if (1 && (pSrc == pDst))
        {
        // selfloop ignore
        }
      else if (!p->CXRef_EvalSeqStartOK(i))
        {
        if (pRef->DstNd()==p) // If GetSet then handle as a Set
          pIns.SetAtGrow(iIns++,pRef);
        else
          pOuts.SetAtGrow(iOuts++, pRef);
        }
      else
        {
        pRef->SetIsEvalSeqStart(true);
        }
      }
    }
  }

//--------------------------------------------------------------------------

void SetUpCtrlNodeEdges(FlwNode * pNd, flag DoingDynMode, CSignalFlowGraph &SigFlwGrf, CSFGIOStack & pIns, CSFGIOStack & pOuts)
  {
  // Connect all Inputs to all Outputs via Edges

  //CSFGNodeInfo * pIns[MaxEdgeNds];
  //CSFGNodeInfo * pOuts[MaxEdgeNds];
  int iIns=0, iOuts=0;
  // Collect Parent & Children if Internal
  if (pNd->Internal)
    {
    FlwNode * pParent = pNd->Parent();
    while (pParent->Internal)
       pParent = pParent->Parent();

    FlwNode * p1=NULL;
    while (pParent->GetObj_All(p1))
      SetUpCtrlNodeEdges(p1, DoingDynMode, pIns, pOuts, iIns, iOuts);
    pNd=pParent;
    }
  else
    {
    SetUpCtrlNodeEdges(pNd, DoingDynMode, pIns, pOuts, iIns, iOuts);
    }

  //if (iIns>=MaxEdgeNds || iOuts>=MaxEdgeNds)
  //  LogError(pNd->FullObjTag(), 0, "Bad number of Tearnodes");
  for (int i = 0; i < iIns; i++)
    for (int j = 0; j < iOuts; j++)
      {
      if (pIns[i] && pOuts[j])
        SigFlwGrf.AddEdge(pIns[i], pOuts[j], pNd->FullObjTag());
      else
        {
        if (1)
          dbgpln("BAD Edge Data %s", pNd->FullObjTag());
        }
      }
  }

//--------------------------------------------------------------------------

flag FlwSlvBlk::BuildSignalFlowGraph(flag DoingDynMode, byte Which)
  {
  flag OK=1;
  FlwNode * p;
  if (Which & TOSL_Proc)
    {
    m_ProcSigFlwGrf.Clear();

    //Add All Real Links As SFG Nodes
    CFlangeIter F(m_Flanges);
    for (CFlange* pF = F.First(); (pF); pF=F.Next())
      if (pF->RqdTearType()<TT_ManualTear)
        m_ProcSigFlwGrf.AddNode(pF, pF->fIsBuffered ? TP_First : pF->TearPriority());

    CFlwNodeIter Flw(m_ProcessNds);
    for (p = Flw.First(); (p); p=Flw.Next())
      SetUpFlwNodeEdges(p, DoingDynMode, m_ProcSigFlwGrf, m_SFGInsStack, m_SFGOutsStack);
    }
  if (Which & TOSL_Ctrl)
    {
    m_CtrlSigFlwGrf.Clear();
    CTerminalIter T(m_Terminals);
    for (CTerminal* pT = T.First(); (pT); pT=T.Next())
      if (pT->m_pNdSrc->InPwrCtrlSeq() && pT->m_pNdSrc->GetActive() && pT->m_pNdDst->InPwrCtrlSeq() && pT->m_pNdDst->GetActive())
        m_CtrlSigFlwGrf.AddNode(pT,TP_Normal);

    CXRefPtrIter X(FlwSlvNd.m_XRefsList);
    for (CXRefItem * pX = X.First(); (pX); pX=X.Next())
      {
      FlwNode *pSrc=dynamic_cast<FlwNode*>(pX->SrcNd());
      FlwNode *pDst=dynamic_cast<FlwNode*>(pX->DstNd());
      if (pSrc && pSrc->InPwrCtrlSeq() && pSrc->GetActive() &&
          pDst && pDst->InPwrCtrlSeq() && pDst->GetActive())
        m_CtrlSigFlwGrf.AddNode(pX,TP_Normal);
      }
    #if dbgFlwSolve
    if (dbgXRefsSetup())
      {
      dbgsep();
      dbgpln("SetUpCtrlNodeEdges");
      }
    #endif

    CFlwNodeIter Ctrl(m_PwrCtrlNds);
    for (p = Ctrl.First(); (p); p=Ctrl.Next())
      SetUpCtrlNodeEdges(p, DoingDynMode, m_CtrlSigFlwGrf, m_SFGInsStack, m_SFGOutsStack);
    }

  return OK;
  }

//-------------------------------------------------------------------------

void FlwSlvBlk::SetUpTearLinks(CSFGNodeInfoList & TearNodeList)//CFlangeList & TearNodeList)
  {
  CSFGNodeInfoIter Tears(TearNodeList);
  for (pCSFGNodeInfo pTearInfo=Tears.First(); pTearInfo; pTearInfo=Tears.Next())
    {
    CFlange* pTearFlng=dynamic_cast<CFlange*>(pTearInfo->SFGNodePtr());
    /**/
    FlwNode * pTearNd;
    int iTearIO;
    if (pTearFlng->m_pNdSrc->MoveTearFlange(pTearFlng->m_iIOSrc, pTearNd, iTearIO))
      pTearFlng=pTearNd->IOFlange(iTearIO);
    else if (pTearFlng->m_pNdDst->MoveTearFlange(pTearFlng->m_iIODst, pTearNd, iTearIO))
      pTearFlng=pTearNd->IOFlange(iTearIO);

    if (pTearFlng->RqdTearType()>=TT_ManualTear)
      pTearFlng->SetTearType(pTearFlng->RqdTearType(), pTearNd);
    else
      pTearFlng->SetTearType(TT_SystemTear, pTearNd);
    m_TearFlanges.AddTail(pTearFlng);

    pTearFlng->SetIsEvalSeqStart(true);

    FlwSlvNd.StructureChanged(&FlwSlvNd);

#if dbgFlwSolve
    if (dbgTearSetup())
      dbgpln("Tear - %s %i %s %s",
             (pTearFlng->RqdTearType()==TT_ManualTear ? "Man":
             (pTearFlng->RqdTearType()==TT_Break      ? "Brk":
              pTearFlng->RqdTearType()==TT_SystemTear ? "Sys":"   ")),
             pTearFlng->TearPriority(),
             pTearFlng->fIsBuffered ? "Buff":"    ",
             pTearFlng->Tag());
      //dbgpln("Tear - %s > %s [%s]", pTear->Tag(),pTear->Nd_Rmt(in)->Tag(), p->Tag());
#endif
    }
  }

//-------------------------------------------------------------------------

void FlwSlvBlk::SetUpTearCLinks(CSFGNodeInfoList & TearNodeList)//CFlangeList & TearNodeList)
  {
  CSFGNodeInfoIter Tears(TearNodeList);
  for (pCSFGNodeInfo pTearInfo=Tears.First(); pTearInfo; pTearInfo=Tears.Next())
    {
    CTerminal * pTearTerm=dynamic_cast<CTerminal*>(pTearInfo->SFGNodePtr());
    CXRefItem * pTearXRef=dynamic_cast<CXRefItem*>(pTearInfo->SFGNodePtr());

    if (pTearTerm)
      {
      pTearTerm->SetIsTear(true);
      m_TearTerminals.AddTail(pTearTerm);
      pTearTerm->SetIsEvalSeqStart(true);

      FlwSlvNd.StructureChanged(&FlwSlvNd);

      #if dbgFlwSolve
      if (dbgTearSetup())
        dbgpln("TearTerm - %s", pTearTerm->SFGNodeTag());
      #endif
      }
    else if (pTearXRef)
      {
      pTearXRef->SetIsTear(true);
      m_TearXRefs.AddTail(pTearXRef);
      pTearXRef->SetIsEvalSeqStart(true);

      FlwSlvNd.StructureChanged(&FlwSlvNd);

      #if dbgFlwSolve
      if (dbgTearSetup())
        dbgpln("TearXRef - %s", pTearXRef->SFGNodeTag());
      #endif
      }
    }
  }

//-------------------------------------------------------------------------

inline LPCTSTR DotNum(long i)
  {
  static Strng X[20];
  static long ix=-1;
  ix=(ix+1)%20;
  if (i>0)
    X[ix].Set("%i", i);
  else if (i<0)
    X[ix]="-";
  else
    X[ix]=".";
  return X[ix]();
  }

//-------------------------------------------------------------------------

inline LPCTSTR SpcNum(long i)
  {
  static Strng X[20];
  static long ix=-1;
  ix=(ix+1)%20;
  if (i>=0)
    X[ix].Set("%i", i);
  else
    X[ix]=" ";
  return X[ix]();
  }

//-------------------------------------------------------------------------

flag FlwSlvBlk::FindTOSequenceStart(byte Which)
  {
  CFlwNodeIter All(m_AllNds);
  CFlwNodeIter Flw(m_ProcessNds);
  CFlwNodeIter Ctrl(m_PwrCtrlNds);
  FlwNode * p;
              
  bool DoingProc=(Which & TOSL_Proc)!=0;
  bool DoingCtrl=(Which & TOSL_Ctrl)!=0;

  if (DoingProc)
    {
    for (int i=0; i<2; i++)
      m_TOList[i].RemoveAllToFreeList();

    for (p = Flw.First(); (p); p=Flw.Next())
      {
      for (int c=0; c<p->NClusters(); c++)
        {
        CJoinCluster &C=p->m_Clusters[c];
        C.m_nFlwIOsInReqd=C.m_nFlwIOsIn;
        #if dbgFlwSolve
        if (dbgEvalOrdFind())
          dbgpln("Cluster <%2i> F:%3i I:%3i R:%3i ------------------------------------%s", c,C.m_nFlwIOs,C.m_nFlwIOsIn,C.m_nFlwIOsInReqd,p->FullObjTag());
        #endif
        //for (int i = 0; i < p->NoFlwIOs(); i++)
        for (int io=0, i; (i=C.IONo(io))>=0; io++)
          {
          if (p->IOEvalSeq_In(i) && p->Nd_Rmt(i)->GetActive() && p->IOFlange(i)->IsEvalSeqStart())
            C.m_nFlwIOsInReqd--;

          #if dbgFlwSolve
          if (dbgEvalOrdFind())
            dbgpln("  %3i %-3s %-6s %-8s %3i  %s", i,
            p->IOEvalSeq_In(i)?"In":"",
            p->Nd_Rmt(i)->GetActive()?"Active":"",
            p->IOFlange(i)->IsEvalSeqStart()?"SeqStart":"",
            C.m_nFlwIOsInReqd,
            p->Nd_Rmt(i)->FullObjTag());
          #endif
          }
        }
      }

    for (p = Flw.First(); (p); p=Flw.Next())
      {
      for (int c=0; c<p->NClusters(); c++)
        {
        CJoinCluster &C=p->m_Clusters[c];
        int NAfter=0;
        int NBefore=0;
        #if dbgFlwSolve
        if (dbgEvalOrdFind())
          dbgpln("FindFlwDependents %s<%i>", p->FullObjTag(),c);
        #endif
        
        for (int io=0, i; (i=C.IONo(io))>=0; io++)
          {
          #if dbgFlwSolve
          if (dbgEvalOrdFind())
            {
            dbgpln("  %-3s %-3s %-3s %-6s %10s > %s",
              p->IOEvalSeq_Out(i) && p->Nd_Rmt(i)->GetActive() && !p->IOFlange(i)->IsEvalSeqStart()?"Add":"",
              p->IOEvalSeq_Out(i)?"Out":"",
              p->Nd_Rmt(i)->GetActive()?"Act":"",
              !p->IOFlange(i)->IsEvalSeqStart()?"!Start":"",
              p->IODesc_Self(i)->IOName(),
              p->Nd_Rmt(i)->FullObjTag());
            }
          #endif
          
          if (p->Nd_Rmt(i)->GetActive() && !p->IOFlange(i)->IsEvalSeqStart())
            {
            if (p->IOEvalSeq_Out(i))
              C.m_EvalAfterMeIOs.SetAtGrow(NAfter++, i);
            else if (p->IOEvalSeq_In(i))
              C.m_EvalBeforeMeIOs.SetAtGrow(NBefore++, i);
            }
          }
        C.m_EvalAfterMeIOs.SetSize(NAfter);
        C.m_EvalBeforeMeIOs.SetSize(NBefore);
        }
      }

    // List 0 @ periphery
    for (p = Flw.First(); (p); p=Flw.Next())
      {
      for (int c=0; c<p->NClusters(); c++)
        {
        CJoinCluster &C=p->m_Clusters[c];
        if (C.m_nFlwIOsInReqd==0 && C.m_nFlwIOsOut>0)
          m_ProcTOList1.AddTail(CNodeEvalIndex(p,c,-1, true,true));
        }
      }

    // List 1
    // Why is this done like this - 
    if (ProcTOSequencePrv.GetCount()>0)
      {
      CNodeEvalIndexIter ProcOrderPrev(ProcTOSequencePrv); 
      for (CNodeEvalIndex NJ=ProcOrderPrev.First(); ProcOrderPrev.ItemValid(); NJ=ProcOrderPrev.Next())
        {
        CJoinCluster &C=NJ.Cluster;
        if (C.m_nFlwIOsIn==0 && C.m_nFlwIOsOut==0)
          {
          #if dbgFlwSolve
          if (dbgEvalOrdFind())
            dbgpln(">>>>>>>>>>>> <%2i>%s",NJ.m_iCluster, NJ.Nd.Tag());
          #endif
          m_ProcTOList2.AddTail(CNodeEvalIndex(&NJ.Nd,NJ.m_iCluster,-1, ST_Start, false));
          }
        }
      }
    else
      {
      for (p = Flw.First(); (p); p=Flw.Next())
        {
        for (int c=0; c<p->NClusters(); c++)
          {
          CJoinCluster &C=p->m_Clusters[c];
          if (C.m_nFlwIOsIn==0 && C.m_nFlwIOsOut==0)
            m_ProcTOList2.AddTail(CNodeEvalIndex(p,c,-1,ST_Isolated,false));
          }
        }
      }
    }

  if (DoingCtrl)
    {
    for (int i=2; i<4; i++)
      m_TOList[i].RemoveAllToFreeList();

    for (p = Ctrl.First(); (p); p=Ctrl.Next())
      {
      p->m_nCtrlsInReqd=p->m_nSeqCtrlIOsIn+p->m_nSeqXRefsIn;
      for (i = 0; i < p->NoCtrls(); i++)
        {
        if (p->Ctrl_In(i) &&
          p->Nd_Rmt(i)->GetActive() &&
          p->Nd_Rmt(i)->InPwrCtrlSeq() &&
          p->Ctrl_Terminal(i)->IsEvalSeqStart())
          {
          p->m_nCtrlsInReqd--;
          }
        }
      for (i = 0; i < p->NoXRefs2Me(); i++)
        {
        if (p->CXRef_In(i))
          {
          FlwNode *pRmt=p->CXRef_RmtNd(i);
          if (pRmt &&
            pRmt->GetActive() &&
            pRmt->InPwrCtrlSeq() &&
            !p->CXRef_SelfLoop(i) &&
            p->CXRef_IsEvalSeqStart(i))
            {
            p->m_nCtrlsInReqd--;
            }
          }
        }
      }

    for (p = Ctrl.First(); (p); p=Ctrl.Next())
      {
      int NDeps=0;
      for (i = 0; i < p->NoCtrls(); i++)
        {
        if (p->Ctrl_Out(i) &&
          p->Nd_Rmt(i)->GetActive() &&
          p->Nd_Rmt(i)->InPwrCtrlSeq() &&
          !p->Ctrl_Terminal(i)->IsEvalSeqStart())
          {
          p->m_CtrlDependentIOs.SetAtGrow(NDeps++, i);
          }
        }
      for (i = 0; i < p->NoXRefs2Me(); i++)
        {
        if (p->CXRef_Out(i))
          {
          FlwNode *pRmt=p->CXRef_RmtNd(i);
          if (pRmt &&
            pRmt->GetActive() &&
            pRmt->InPwrCtrlSeq() &&
            !p->CXRef_SelfLoop(i) &&
            !p->CXRef_IsEvalSeqStart(i))
            {
            int NegI=-(i+1);
            p->m_CtrlDependentIOs.SetAtGrow(NDeps++, NegI);
            }
          }
        }
      p->m_CtrlDependentIOs.SetSize(NDeps);
      }

    // List 2 @ periphery
    for (p = Ctrl.First(); (p); p=Ctrl.Next())
      if (p->CtrlsInReqd()==0 && p->CtrlsOut()>0)
        m_CtrlTOList1.AddTail(CNodeEvalIndex(p,-1,-1,ST_Start,true));
    // List 3
    for (p = Ctrl.First(); (p); p=Ctrl.Next())
      {
      if (p->CtrlsIn()==0 && p->CtrlsOut()==0)
        m_CtrlTOList2.AddTail(CNodeEvalIndex(p,-1,-1,ST_Isolated,false));
      }
    }



#if dbgFlwSolve
  if (dbgEvalOrdFind())
    {
    for (p = All.First(); (p); p=All.Next())
      {
      for (int c=0; c<p->NClusters(); c++)
        p->m_Clusters[c].m_nFlwIOsInDone=0;
      p->m_nCtrlsInDone=0;
      }

    for (int pass=0; pass<2; pass++)
      if (DoingProc && (pass==0) || DoingCtrl && (pass==1))
        {
        CFlwNodeIter Nds(pass==0 ? m_ProcessNds:m_PwrCtrlNds);
        dbgsep();
        dbgpln("%-10s     : %s", "Dependency", "Dependents[InCnt, InRqd]...");
        for (p = Nds.First(); (p); p=Nds.Next())
          {
          int DoHed=1;
          for (int c=0; c<p->NClusters(); c++)
            {
            CJoinCluster &C=p->m_Clusters[c];
            CIArray & PDeps=C.m_EvalAfterMeIOs;
            for (int d=0; d<PDeps.GetSize(); d++)
              {
              FlwNode * pRmt=p->Nd_Rmt(PDeps[d]);
              CJoinCluster *pC=p->IO_Cluster_Rmt(PDeps[d]);
              if (DoHed--==1)
                dbgpln("%-s:", p->FullObjTag());
              if (!pC)
                dbgp("                NO CLUSTER ");
              else if (pC && pC->m_nFlwIOsInReqd)
                dbgp("                P:[%3i:%3i]", ++(pC->m_nFlwIOsInDone),pC->m_nFlwIOsInReqd);
              else
                dbgp("                P:[%3s:%3s] no Flws In", "","");
              dbgpln(" %s", pRmt->FullObjTag());
              }
            }

          DoHed=1;
          CIArray & CDeps=p->m_CtrlDependentIOs;
          for (int d=0; d<CDeps.GetSize(); d++)
            {
            FlwNode * pRmt=NULL;
            long iRmt=CDeps[d];
            if (iRmt>=0)
              {
              pRmt=p->Nd_Rmt(iRmt);
              if (DoHed--==1)
                dbgpln("%-s:", p->FullObjTag());
              if (pRmt->CtrlsInReqd())
                dbgp("                C:[%3i:%3i]", ++(pRmt->m_nCtrlsInDone),pRmt->CtrlsInReqd());
              else
                dbgp("                C:[%3s:%3s] no Ctrls In", "","");
              dbgpln(" %s", pRmt->FullObjTag());
              }
            }

          DoHed=1;
          for (d=0; d<CDeps.GetSize(); d++)
            {
            FlwNode * pRmt=NULL;
            long iRmt=CDeps[d];
            if (iRmt<0)
              {
              iRmt=-(iRmt+1);
              pRmt=static_cast<FlwNode*>(p->m_XRefs2Me[iRmt]->DstNd());
              if (DoHed==1)
                {
                dbgpln("%-s:", p->FullObjTag());
                DoHed--;
                }
              DoHed--;
              if (pRmt->CtrlsInReqd())
                dbgp("                X:[%3i:%3i]", ++(pRmt->m_nCtrlsInDone),pRmt->CtrlsInReqd());
              else
                dbgp("                X:[%3s:%3s] no Ctrls In", "","");
              dbgpln(" %s", pRmt->FullObjTag());
              //if (DoHed<-1)
              //  {
              //  DoHed=0;
              //  dbgpln("");
              //  dbgp("         %-10s:", "");
              //  }
              }
            }
          }
        //dbgpln("");
        }

      dbgsep();
      // reset counters
      dbgpln("      [%5s %5s %5s %5s %5s %5s %5s %5s %3s %3s %3s %3s %3s %3s %3s %3s %3s %3s] ",
        "Pi","Po","PiR","PTr","Pi","Po","PiR","PTr","Ci","Co","sCi","sCo","Xi","Xo","sXi","sXo","CiR","CTr");
      for (pass=0; pass<2; pass++)
        {
        long NdCnt=0;
        if (DoingProc && (pass==0) || DoingCtrl && (pass==1))
          {
          CFlwNodeIter Nds(pass==0 ? m_ProcessNds:m_PwrCtrlNds);
          for (p = Nds.First(); (p); p=Nds.Next())
            {
            dbglock();
            dbgp("%4i ", NdCnt++);
            if (DoingProc)//p->IsProcSeq())
              dbgp("%s[",DoingProc?"P":"C");
            int c;
            for (c=0; c<p->NClusters(); c++)
              {
              CJoinCluster &C=p->m_Clusters[c];
              dbgp("%5s %5s %5s %5s ",
                DotNum(C.m_nFlwIOsIn),DotNum(C.m_nFlwIOsOut),DotNum(C.m_nFlwIOsInReqd),
                DotNum(C.m_nFlwIOsIn-C.m_nFlwIOsInReqd));
              }
            for ( ; c<2; c++)
              dbgp("%5s %5s %5s %5s ","","","","");

            dbgp("%3s %3s %3s %3s %3s %3s %3s %3s %3s %3s] ",
              DotNum(p->m_nCtrlIOsIn),DotNum(p->m_nCtrlIOsOut),
              DotNum(p->m_nSeqCtrlIOsIn),DotNum(p->m_nSeqCtrlIOsOut),
              DotNum(p->m_nXRefsIn),DotNum(p->m_nXRefsOut),
              DotNum(p->m_nSeqXRefsIn),DotNum(p->m_nSeqXRefsOut),
              DotNum(p->m_nCtrlsInReqd),
              DotNum(p->CtrlsIn()-p->CtrlsInReqd()));
            //else
            //  dbgp("C[%5s %5s %5s %5s %3s %3s %3s %3s %3s %3s %3s %3s %3s %3s] ",
            //    DotNum(p->m_nFlwIOsIn),DotNum(p->m_nFlwIOsOut),DotNum(p->m_nFlwIOsInReqd),
            //    DotNum(p->m_nFlwIOsIn-p->m_nFlwIOsInReqd),
            //    DotNum(p->m_nCtrlIOsIn),DotNum(p->m_nCtrlIOsOut),
            //    DotNum(p->m_nSeqCtrlIOsIn),DotNum(p->m_nSeqCtrlIOsOut),
            //    DotNum(p->m_nXRefsIn),DotNum(p->m_nXRefsOut),
            //    DotNum(p->m_nSeqXRefsIn),DotNum(p->m_nSeqXRefsOut),
            //    DotNum(p->m_nCtrlsInReqd),
            //    DotNum(p->CtrlsIn()-p->CtrlsInReqd()));
            //}

            if (DoingProc)
              {
              for (int c=0; c<p->NClusters(); c++)
                {
                CJoinCluster &C=p->m_Clusters[c];
                if (C.m_nFlwIOsInDone<C.m_nFlwIOsInReqd)
                  {
                  int NTears=0;
                  int NSeqStart=0;
                  for (int io=0, i; (i=C.IONo(io))>=0; io++)
                    {
                    if (p->IOFlange(i)->IsTear())
                      NTears++;
                    if (p->IOFlange(i)->IsEvalSeqStart())
                      NSeqStart++;
                    }
                  dbgp("PIn Missing : [%2i of %2i / Tears:%2i / SeqStart:%2i]",
                    C.m_nFlwIOsInDone, C.m_nFlwIOsInReqd, NTears, NSeqStart);
                  }
                }
              }

            if (DoingCtrl && p->m_nCtrlsInDone<p->CtrlsInReqd())
              {
              int NTears=0;
              int NSeqStart=0;
              for (int i=0; i<p->NoCtrls(); i++)
                {
                if (p->Ctrl_Terminal(i)->IsTear())
                  NTears++;
                if (p->Ctrl_Terminal(i)->IsEvalSeqStart())
                  NSeqStart++;
                }
              for (i=0; i<p->NoXRefs2Me(); i++)
                {
                if (p->CXRef(i)->IsTear())
                  NTears++;
                if (p->CXRef(i)->IsEvalSeqStart())
                  NSeqStart++;
                }
              dbgp("CIn Missing : [%2i of %2i / Tears:%2i / SeqStart:%2i]",
                p->m_nCtrlsInDone, p->CtrlsInReqd(), NTears, NSeqStart);
              }
            if (p->NClusters()>1)
              dbgp("<%2i>", p->NClusters());
            else
              dbgp("    ");
            dbgp(" %-25s", p->FullObjTag());
            dbgpln("");
            dbgunlock();

            for (int c=0; c<p->NClusters(); c++)
              p->m_Clusters[c].m_nFlwIOsInDone=0;
            p->m_nCtrlsInDone=0;
            }
          }
        }
      dbgpln("");
      dbgsep();

      int i0=DoingProc?0:2;
      int in=DoingCtrl?4:2;
      for (int i=i0; i<in; i++)
        {
        static LPCTSTR What[]={
          "Connected Process",
            "Isolated  Process",
            "Connected Control",
            "Isolated  Control"
          };
        dbgpln("List %i --- %s ----------------",i, What[i]);
        CNodeEvalIndex NJ;
        CNodeEvalIndexIter It1(m_TOList[i]);
        for (NJ=It1.First(); It1.ItemValid(); NJ=It1.Next())
          dbgpln(" : %-15s %-25s", NJ.Nd.Class()->ClassName(), NJ.Nd.FullObjTag());
        }
    }
#endif
  return true;
  }

//-------------------------------------------------------------------------

void FlwSlvBlk::BuildTOSequence(byte Which)
  {
  if (Which & (TOSL_Proc|TOSL_Ctrl))
    ClearTOSequence(TOSL_Comb);

  if (Which & TOSL_Proc)
    {
    #if dbgFlwSolve
    if (dbgBuildOrder())
      {
      for (int i=0; i<2; i++)
        {
        dbgpln("Proc List %i ------------------------------------------------------------", i);
        CNodeEvalIndexIter FO(m_TOList[i]);
        for (CNodeEvalIndex NJ=FO.First(); FO.ItemValid(); NJ=FO.Next())
          dbgpln("%s ", NJ.Nd.FullObjTag());
        }
      }
    #endif

    ClearTOSequence(TOSL_Proc);

    CFlwNodeIter Flw(m_ProcessNds);
    for (FlwNode * p = Flw.First(); (p); p=Flw.Next())
      {
      for (int c=0; c<p->NClusters(); c++)
        {
        CJoinCluster & C=p->m_Clusters[c];
        C.m_iProcOrder=iNotInEvalOrder;
        C.m_iProcSeq=iNotInEvalOrder;
        C.m_iSeqType=ST_Undefined;
#ifndef _RELEASE  
        C.m_iProcOrderActual=iNotInEvalOrder;
#endif
        C.m_nFlwIOsInDone=0;
        }
      }

    // --------------------------
    CNodeEvalIndexList Lst0;
    CNodeEvalIndexIter FO(m_ProcTOList1);
    CNodeEvalIndex NJ;
    for (NJ=FO.First(); FO.ItemValid(); NJ=FO.Next())
      Lst0.AddTail(NJ);

    long iOrder=0;

    #if dbgFlwSolve
    if (dbgBuildOrder())
      dbgpln("Proc List 1 ------------------------------------------------------------");
    #endif

    while (!Lst0.IsEmpty())
      {
      CNodeEvalIndex NJ=Lst0.RemoveHead();
      CJoinCluster &C=NJ.Cluster;
      p=NJ.m_pNd;
      if (C.m_iProcOrder>=0)
        {
        CString S;
        S.Format("%s already in Evaluation Order", p->Tag());
        ASSERT_ALWAYS(FALSE, (LPTSTR)(LPCTSTR)S, __FILE__, __LINE__);
        }

      C.m_iProcOrder=iOrder++;
      
      C.m_iSeqType = NJ.m_iSeqType;
      switch (NJ.m_iSeqType)
        {
        case ST_Isolated:     
          {
          C.m_iProcSeq=0;
          break;
          };
        case ST_Start:     
          {
          C.m_iProcSeq=1;
          break;
          }
        case ST_Depends:
          {
          C.m_iProcSeq=0;
          for (int i=0; i<C.m_EvalBeforeMeIOs.GetCount(); i++)
            {
            long iSeq=p->IO_Cluster_Rmt(C.m_EvalBeforeMeIOs[i])->m_iProcSeq;
            C.m_iProcSeq=Max(C.m_iProcSeq, iSeq+1);
            }
          break;
          }
        default:
          C.m_iProcSeq=-1;
        }
      ProcTOSequence.AddTail(NJ);

      #if dbgFlwSolve
      if (dbgBuildOrder())
        dbgpln("%s", NJ.m_pNd->FullObjTag());
      #endif

      //p->m_nFlwIOsInDone=0;
      CIArray & After=C.m_EvalAfterMeIOs;
      for (int d=0; d<After.GetSize(); d++)
        {
        #if dbgFlwSolve
        if (dbgBuildOrder())
          dbgp("                        %3i %3i ", After[d], p->m_IOs[After[d]].iClusterId);
        #endif

        FlwNode * pRmt=p->Nd_Rmt(After[d]);
        CJoinCluster *pCRmt=p->IO_Cluster_Rmt(After[d]);
        if (pCRmt)
          {
          #if dbgFlwSolve
          if (dbgBuildOrder())
            dbgp("  C:%2i i:%4i o:%4i dn:%4i rq:%4i", pCRmt->m_iCluster, pCRmt->m_nFlwIOsIn, pCRmt->m_nFlwIOsOut, pCRmt->m_nFlwIOsInDone+1, pCRmt->m_nFlwIOsInReqd);
          #endif
          if (InterlockedIncrement(&(pCRmt->m_nFlwIOsInDone))==pCRmt->m_nFlwIOsInReqd)
            {
            #if dbgFlwSolve
            if (dbgBuildOrder())
              dbgp("  ADD %s", pRmt->Tag());
            #endif
            Lst0.AddHead(CNodeEvalIndex(pRmt, pCRmt->m_iCluster, pCRmt->m_iJoinMask, ST_Depends,true));
            // pRmt->m_nFlwIOsInDone=0;
            }
          else
            {
            #if dbgFlwSolve
            if (dbgBuildOrder())
              dbgp("      %s", pRmt->Tag());
            #endif
            }
          }
        else
          {
          dbgp("BAD RMT CLUSTER %s", pRmt->Tag());
          }

        #if dbgFlwSolve
        if (dbgBuildOrder())
          dbgpln("");
        #endif
        }
      }

    // --------------------------
    CNodeEvalIndexIter F1(m_ProcTOList2);
    #if dbgFlwSolve
    if (dbgBuildOrder())
      dbgpln("Proc List 2 ------------------------------------------------------------");
    #endif
    for (NJ=F1.First(); F1.ItemValid(); NJ=F1.Next())
      {
      NJ.Cluster.m_iProcOrder=iOrder++;
      ProcTOSequence.AddTail(NJ);
      #if dbgFlwSolve
      if (dbgBuildOrder())
        dbgpln("  %s", NJ.m_pNd->FullObjTag());
      #endif
      };

    CNodeEvalIndexIter ProcOrder(ProcTOSequence);
    for (NJ=ProcOrder.First(); ProcOrder.ItemValid(); NJ=ProcOrder.Next())
      NJ.Cluster.m_nFlwIOsInDone=0;
    }

  if (Which & TOSL_Ctrl)
    {
    if (0)
      {
      for (int i=2; i<4; i++)
        {
        dbgpln("Ctrl List %i ------------------------------------------------------------", i);
        CNodeEvalIndexIter FO(m_TOList[i]);
        for (CNodeEvalIndex NJ=FO.First(); FO.ItemValid(); NJ=FO.Next())
          dbgpln("%s ", NJ.Nd.FullObjTag());
        }
      CFlwNodeIter Ctrl(m_PwrCtrlNds);
      for (FlwNode * p = Ctrl.First(); (p); p=Ctrl.Next())
        dbgpln("PCN:%s ", p->FullObjTag());
      }

    ClearTOSequence(TOSL_Ctrl);

    CFlwNodeIter Ctrl(m_PwrCtrlNds);
    for (FlwNode * p = Ctrl.First(); (p); p=Ctrl.Next())
      {
      p->m_iCtrlOrder=iNotInEvalOrder;
      p->m_nCtrlsInDone=0;
      }

    // --------------------------
    CNodeEvalIndexList Lst2;
    CNodeEvalIndexIter F2(m_CtrlTOList1);
    CNodeEvalIndex NJ;
    for (NJ=F2.First(); F2.ItemValid(); NJ=F2.Next())
      Lst2.AddTail(NJ);

    long iOrder=0;
    while (!Lst2.IsEmpty())
      {
      CNodeEvalIndex NJ=Lst2.RemoveHead();
      p=NJ.m_pNd;
      if (p->m_iCtrlOrder>=0)
        {
        CString S;
        S.Format("%s already in Evaluation Order", p->Tag());
        ASSERT_ALWAYS(FALSE, (LPTSTR)(LPCTSTR)S, __FILE__, __LINE__);
        }
      p->m_iCtrlOrder=iOrder++;
      CtrlTOSequence.AddTail(NJ);
      if (0)
        dbgpln("Ctrl List 1 %s", NJ.m_pNd->FullObjTag());

      //p->m_nCtrlsInDone=0;
      CIArray & Deps=p->m_CtrlDependentIOs;
      for (int d=0; d<Deps.GetSize(); d++)
        {
        FlwNode * pRmt=NULL;
        long iRmt=Deps[d];
        if (iRmt<0)
          {
          iRmt=-(iRmt+1);
          pRmt=static_cast<FlwNode*>(p->m_XRefs2Me[iRmt]->DstNd());
          }
        else
          pRmt=p->Nd_Rmt(iRmt);
        if (InterlockedIncrement(&(pRmt->m_nCtrlsInDone))==pRmt->CtrlsInReqd())
          {
          Lst2.AddHead(CNodeEvalIndex(pRmt,-1,-1, ST_Depends,true));
          //pRmt->m_nCtrlsInDone=0;
          }
        }
      }

    // --------------------------
    CNodeEvalIndexIter F3(m_CtrlTOList2);
    for (NJ=F3.First(); F3.ItemValid(); NJ=F3.Next())
      {
      NJ.Nd.m_iCtrlOrder=iOrder++;
      CtrlTOSequence.AddTail(NJ);
      if (0)
        dbgpln("Ctrl List 2 %s", NJ.m_pNd->FullObjTag());
      };

    if (0)
      {
      CFlwNodeIter Ctrl(m_PwrCtrlNds);
      for (FlwNode * p = Ctrl.First(); (p); p=Ctrl.Next())
        {
        dbgpln("XXX %5i %5i %5i %s", p->m_iCtrlOrder, p->m_nCtrlsInReqd, p->m_nCtrlsInDone, p->Tag());
        }
      }

    CNodeEvalIndexIter CtrlOrder(CtrlTOSequence);
    for (NJ=CtrlOrder.First(); CtrlOrder.ItemValid(); NJ=CtrlOrder.Next())
      NJ.Nd.m_nCtrlsInDone=0;
    }

  if (Which & (TOSL_Proc|TOSL_Ctrl))
    {
    CNodeEvalIndexIter ProcOrder(ProcTOSequence);
    for (CNodeEvalIndex NJ=ProcOrder.First(); ProcOrder.ItemValid(); NJ=ProcOrder.Next())
      CombTOSequence.AddTail(NJ);
    CNodeEvalIndexIter CtrlOrder(CtrlTOSequence);
    for (NJ=CtrlOrder.First(); CtrlOrder.ItemValid(); NJ=CtrlOrder.Next())
      {
      if (NJ.m_iCluster<0 || NJ.Cluster.m_iProcOrder<0)
        CombTOSequence.AddTail(NJ);
      }
    }

  // some checks
  if (1)
    {
    if (Which & TOSL_Proc)
      {
      Strng sClstr, S;
      int nCluster=0;
      CFlwNodeIter Flw(m_ProcessNds);
      for (FlwNode * p = Flw.First(); (p); p=Flw.Next())
        {
        for (int c=0; c<p->NClusters(); c++)
          {
          if (p->m_Clusters[c].m_iProcOrder<0)
            {
            if (nCluster==0)
              sClstr.Set("%i", c);
            else
              {
              S.Set(",%i", c);
              sClstr+=S;
              }
            nCluster++;
            //LogError(p->FullObjTag(), 0, "Cluster %i Not in Process Evaluation Sequence [%s]", c, p->ClassId());
            };
          }

        //switch (nCluster)
        //  {
        //  case 0:   p->ClrCI(34); break;
        //  case 1:   p->SetCI(34, "E\tCluster %s Not in Process Evaluation Sequence ", sClstr()); break;
        //  default:  p->SetCI(34, "E\tClusters %s Not in Process Evaluation Sequence ", sClstr()); break;
        //  }
        switch (nCluster)
          {
          case 0:   break;
          case 1:   LogError(p->FullObjTag(), 0, "Cluster %s Not in Process Evaluation Sequence ", sClstr()); break;
          default:  LogError(p->FullObjTag(), 0, "Clusters %s Not in Process Evaluation Sequence ", sClstr()); break;
          }
        nCluster=0;
        }
      }
    if (Which & TOSL_Ctrl)
      {
      CFlwNodeIter Flw(m_PwrCtrlNds);
      for (FlwNode * p = Flw.First(); (p); p=Flw.Next())
        {
        if (p->m_iCtrlOrder<0)
          p->SetCI(34, "E\tNot in Control Evaluation Sequence");
        else
          p->ClrCI(34);
        }
      }
    }
  }

//-------------------------------------------------------------------------

flag FlwSlvBlk::SolveTOSequenceThrd(int What, byte Which, LPVOID OtherData)
  {
  #if dbgFlwSolve
  if (dbgThreadsNodes())
    dbgpln("FlwSlvBlk::SolveTOSequenceThrd");
  #endif
  flag RetFlag=true;
  if (Which & TOSL_Proc)
    {
    for (int i=0; i<2; i++)
      {
      if (m_TOListWork[i].GetCount()>0)
        LogStop("Solver", LF_Hand, "Bad Solver Queue - Process Stopping At Start [%i] = %i", i, m_TOListWork[i].GetCount());

      m_TOListIter[i].RetFlag=true;
      m_TOListIter[i].Initialise(&m_TOListWork[i], FSTC.NThreads());
      CNodeEvalIndexIter C1(m_TOList[i]);
      for (CNodeEvalIndex NJ=C1.First(); C1.ItemValid(); NJ=C1.Next())
        {
        m_TOListIter[i].AddTail(NJ);
        #if dbgFlwSolve
        if (dbgThreadsNodes())
          dbgpln("ProcInit %3i) 0x%08x %s", m_TOListIter[i].List().GetCount(), NJ.JoinMask, NJ.Nd.FullObjTag());
        #endif
        }
      
      m_TOListIter[i].pOtherData=OtherData;
      FSTC.DoJob(What, this, &m_TOListIter[i], OtherData);

      RetFlag = RetFlag && m_TOListIter[i].RetFlag;

      if (m_TOListWork[i].GetCount()>0)
        LogStop("Solver", LF_Hand, "Bad Solver Queue - Process Stopping At End [%i] = %i", i, m_TOListWork[i].GetCount());

      }
    }
  if (Which & TOSL_Ctrl)
    {
    for (int i=2; i<4; i++)
      {
      if (m_TOListWork[i].GetCount()>0)
        LogStop("Solver", LF_Hand, "Bad Solver Queue - Ctrl Stopping at Start [%i] = %i", i, m_TOListWork[i].GetCount());

      m_TOListIter[i].RetFlag=true;
      m_TOListIter[i].Initialise(&m_TOListWork[i], FSTC.NThreads());
      CNodeEvalIndexIter C1(m_TOList[i]);
      for (CNodeEvalIndex NJ=C1.First(); C1.ItemValid(); NJ=C1.Next())
        {
        m_TOListIter[i].AddTail(NJ);
        #if dbgFlwSolve
        if (dbgThreadsNodes())
          dbgpln("CtrlInit %3i) %s", m_TOListIter[i].List().GetCount(), NJ.Nd.FullObjTag());
        #endif
        }
      m_TOListIter[i].pOtherData=OtherData;
      FSTC.DoJob(What, this, &m_TOListIter[i], OtherData);

      RetFlag = RetFlag && m_TOListIter[i].RetFlag;

      if (m_TOListWork[i].GetCount()>0)
        LogStop("Solver", LF_Hand, "Bad Solver Queue - Ctrl Stopping at End [%i] = %i", i, m_TOListWork[i].GetCount());
      }
    }
  return RetFlag;

  }

//-------------------------------------------------------------------------

void FlwSlvBlk::SummariseTOSequence(byte Which)
  {
//  int iOrder=0;
  bool DoingProc=(Which & TOSL_Proc)!=0;
  bool DoingCtrl=(Which & TOSL_Ctrl)!=0;

  CNodeEvalIndex NJ;
  for (int Pass=0; Pass<2; Pass++)
    {
    CNodeEvalIndexIter It(Pass==0 ? ProcTOSequence : CtrlTOSequence);

    for (NJ=It.First(); It.ItemValid(); NJ=It.Next())
      {
      FlwNode & Nd=NJ.Nd;
      NJ.Nd.m_sEvalOrder="";
      NJ.Nd.m_sEvalSeq="";
#ifndef _RELEASE
      NJ.Nd.m_sEvalOrderDiff="";
#endif
      }
    }

  #if dbgFlwSolve
  if (dbgEvalOrdSummary())
    {
    dbgsep();
    dbgpln("Find EvalOrder:");
    if (DoingProc) dbgpln(" TearFlanges  :%3i", m_TearFlanges.GetCount());
    if (DoingCtrl) dbgpln(" TearTerminals:%3i", m_TearTerminals.GetCount());
    if (DoingCtrl) dbgpln(" TearXRefs    :%3i", m_TearXRefs.GetCount());
    //dbgpln("");
    for (int Pass=0; Pass<2; Pass++)
      {
      if (DoingProc && (Pass==0) || DoingCtrl && (Pass==1))
        {
        dbgpln("%-40s      Lnks Proc Audt [%5s %5s %5s %5s %3s %3s %3s %3s %3s %3s %3s %3s %3s %3s] ", Pass==0 ? "Process":"Control",
          "Pi","Po","PiR","PTr","Ci","Co","sCi","sCo","Xi","Xo","sXi","sXo","CiR","CTr");
        long NdCnt=0;
        CNodeEvalIndexIter It(Pass==0 ? ProcTOSequence : CtrlTOSequence);
        CNodeEvalIndex NJ;
        for (NJ=It.First(); It.ItemValid(); NJ=It.Next())
          {
          FlwNode &Nd=NJ.Nd;
          CJoinCluster *pC=NJ.m_iCluster>=0 ? &NJ.Cluster : NULL;
          dbglock();
          if (Nd.IsProcessSeq())
            {

            for (int i=0; i<Nd.NoFlwIOs(); i++)
              if (Nd.IOEvalSeq_In(i))
                if (Nd.IOFlange(i)->IsTear())
                  dbgpln("--> Recycle %s",Nd.Nd_Rmt(i)->FullObjTag());
            }
          else
            {
            //for (int i=0; i<Nd.NoCtrls(); i++)
            //  if (Nd.Ctrl_In(i))
            //    if (Nd.Ctrl_Terminal(i)->IsTear())
            //      dbgpln("--> Recycle %s",Nd.Nd_Rmt(i)->FullObjTag());
            }

          dbgp("%4i ", NdCnt++);
          if (pC)
            dbgp(pC->m_iProcOrder>=0 ? "%4i ":"   . ", pC->m_iProcOrder);
          else 
            dbgp("   . ");
          dbgp(Nd.m_iCtrlOrder>=0 ? "%4i ":"   . ", Nd.m_iCtrlOrder);
          dbgp("%2s ", NJ.m_bInSequence?"Sq":"  ");
          if (Nd.IsProcessSeq())
            {
            dbgp("  ");
            Strng Q1(" "),Q2(" ");
            if (Nd.IsProcess())
              {
              if (pC->m_nFlwIOsIn==0 && pC->m_nFlwIOsOut==0)
                Q1="-";
              else if (pC->m_nFlwIOsIn==0)
                Q1="Source";
              else if (pC->m_nFlwIOsOut==0)
                Q1="Sink";
              else
                Q1="Process";
              }
            else if (Nd.IsLnk() || (pC && pC->m_nFlwIOsIn== 1 && pC->m_nFlwIOsOut==1))
              {
              Q1="Lnk";
              FlwBlk &FB=*Nd.IOFB(0,0);//*(Nd.FBlk());
              if (fabs(FB.GetQm())>1.0e99)
                Q2.Set("+++");
              else if (fabs(FB.GetQm())<1.0e-20)
                Q1="-";
              else if (fabs(FB.GetQm())>1.0e6 || fabs(FB.GetQm())<1.0e-2)
                Q2.Set("%14.5g",fabs(FB.GetQm()));
              else
                Q2.Set("%14.4f",fabs(FB.GetQm()));
              }
            else if (pC->m_nFlwIOsIn == pC->m_nFlwIOsOut && pC->m_nFlwIOsIn > 0)
              Q1="Tie";
            else if (pC->m_nFlwIOsIn > pC->m_nFlwIOsOut)
              Q1="Mixer";
            else if (pC->m_nFlwIOsIn < pC->m_nFlwIOsOut)
              Q1="Split";
            else
              Q1="--";
            dbgp("");

            dbgp("");
            dbgp("%-10s %14s %4i %4i %4i [%5s %5s %5s %5s %3s %3s %3s %3s %3s %3s %3s %3s %3s %3s] ",
              Q1(),Q2(), 
              Nd.NoProcLnkIOs(),
              Nd.NoProcessIOs(),Nd.NoMaterialIOs(),
              DotNum(pC ? pC->m_nFlwIOsIn:-1),DotNum(pC ? pC->m_nFlwIOsOut:-1),
              DotNum(pC ? pC->m_nFlwIOsInReqd:-1),DotNum(pC ? (pC->m_nFlwIOsIn-pC->m_nFlwIOsInReqd):-1),
              DotNum(Nd.m_nCtrlIOsIn),DotNum(Nd.m_nCtrlIOsOut),
              DotNum(Nd.m_nSeqCtrlIOsIn),DotNum(Nd.m_nSeqCtrlIOsOut),
              DotNum(Nd.m_nXRefsIn),DotNum(Nd.m_nXRefsOut),
              DotNum(Nd.m_nSeqXRefsIn),DotNum(Nd.m_nSeqXRefsOut),
              DotNum(Nd.m_nCtrlsInReqd),
              DotNum(Nd.CtrlsIn()-Nd.CtrlsInReqd()));
            dbgp(" : %-15s ", Nd.Class()->ClassName());
            if (Nd.NClusters()>1)
              dbgp("<%2i>", pC->m_iCluster);
            else
              dbgp("    ");
            dbgp("%-25s", Nd.FullObjTag());

            if (Nd.NoFlwIOs())
              {
              dbgp(" [");
              for (int j=0; j<Nd.NoFlwIOs(); j++)
                if (Nd.IO_Dirn(j)==FD_In)
                  dbgp(" %s",Nd.Nd_Rmt(j)->FullObjTag());
              dbgp(" ] ");
              }
            }
          else
            {
            dbgp("  ");
            Strng Q1(" "),Q2(" ");
            if (Nd.IsCLnk())// || (Nd.CtrlsIn()== 1 && Nd.CtrlsOut()==1))
              {
              Q1="CLnk";
              Q2.Set("%14.4f",Nd.CIO_Value(0));
              }
            else if (Nd.CtrlsIn()>0 || Nd.CtrlsOut()>0)
              Q1="Ctrl";
            else
              Q1="--";

            dbgp("%-10s %14s %4i %4s %4s [%5s %5s %5s %5s %3s %3s %3s %3s %3s %3s %3s %3s %3s %3s] ",
              Q1(),Q2(),
              Nd.NoCIOs(), "", "",
              DotNum(pC?pC->m_nFlwIOsIn:-1),DotNum(pC?pC->m_nFlwIOsOut:-1),
              DotNum(pC?pC->m_nFlwIOsInReqd:-1),DotNum(pC?(pC->m_nFlwIOsIn-pC->m_nFlwIOsInReqd):-1),
              DotNum(Nd.m_nCtrlIOsIn),DotNum(Nd.m_nCtrlIOsOut),
              DotNum(Nd.m_nSeqCtrlIOsIn),DotNum(Nd.m_nSeqCtrlIOsOut),
              DotNum(Nd.m_nXRefsIn),DotNum(Nd.m_nXRefsOut),
              DotNum(Nd.m_nSeqXRefsIn),DotNum(Nd.m_nSeqXRefsOut),
              DotNum(Nd.m_nCtrlsInReqd),
              DotNum(Nd.CtrlsIn()-Nd.CtrlsInReqd()));
            dbgp(" : %-15s      %-25s", Nd.Class()->ClassName(), Nd.FullObjTag());

            if (Nd.NoCtrls())
              {
              dbgp(" [");
              for (int j=0; j<Nd.NoCtrls(); j++)
                if (Nd.Ctrl_In(j))
                  dbgp(" %s",Nd.Nd_Rmt(j)->FullObjTag());
              dbgp(" ] ");
              }
            }

          dbgpln("");
          dbgunlock();

          if (Nd.IsProcessSeq())
            {
            for (int i=0; i<Nd.NoFlwIOs(); i++)
              if (Nd.IOEvalSeq_Out(i))
                if (Nd.IOFlange(i)->IsTear())
                  dbgpln("<-- Recycle %s",Nd.Nd_Rmt(i)->FullObjTag());
                //else if(Nd.IOIsBuffer(i))
                //  dbgpln("<-- Buffer  %s",Nd.Nd_Rmt(i)->FullObjTag());
            }
          else
            {
            }

          }
        }
      }

    if (Which & TOSL_Proc)
      {
      CFlwNodeIter All(m_AllNds);
      for (FlwNode * p = All.First(); (p); p=All.Next())
        {
        bool ProcEvalOK=true;
        for (int c=0; c<p->NClusters(); c++)
          if (p->m_Clusters[c].m_iProcOrder<0)
            ProcEvalOK=false;
        if (!ProcEvalOK)
          dbgpln("No Proc Evaluation %s", p->FullObjTag());
        //if (!ProcEvalOK && p->m_iCtrlOrder<0)
        //  dbgpln("No Evaluation %s", p->FullObjTag());
        }
      }
    }
  #endif
  }

//-------------------------------------------------------------------------

inline long TLen(char * s) { return Max(0L,(long)strlen(s)); };
void FlwSlvBlk::DumpEvalOrder(byte Which)
  {
  //FlwNode *p;
  bool DoingProc=(Which & TOSL_Proc)!=0;
  bool DoingCtrl=(Which & TOSL_Ctrl)!=0;

  bool UseConnID = 0;
  long iTgWide=10;
  long iSDWide=10;
  for (int Pass=(DoingProc?0:1); Pass<(DoingCtrl?2:1); Pass++)
    {
    CNodeEvalIndexIter It(Pass==0? ProcTOSequence : CtrlTOSequence);
    for (CNodeEvalIndex NJ=It.First(); It.ItemValid(); NJ=It.Next())
      {
      iTgWide=Max(iTgWide, (long)strlen(NJ.Nd.FullObjTag()));
      for (int i=0; i<NJ.Nd.NoXRefs2Me(); i++)
        iSDWide=Max(iSDWide, (long)strlen(NJ.Nd.m_XRefs2Me[i]->SFGNodeTag(NJ.m_pNd)));
      if (UseConnID)
        for (int i=NJ.Nd.Ctrl1(); i<NJ.Nd.CtrlN(); i++)
          {
          Strng ID, Tg;
          NJ.Nd.Ctrl_GetConnIDStr(i, ID, Tg);
          iSDWide=Max(iSDWide, (long)ID.GetLength());
          iSDWide=Max(iSDWide, (long)Tg.GetLength());
          }
      }
    }
  iSDWide=Max(iSDWide, iTgWide);

  for (Pass=(DoingProc?0:1); Pass<(DoingCtrl?2:1); Pass++)
    {
    bool DoProc=(Pass==0);
    CNodeEvalIndexIter It(DoProc ? ProcTOSequence : CtrlTOSequence);

    int iPDigs=3;
    bool PrevHasNOIO=false;
    for (CNodeEvalIndex NJ=It.First(); It.ItemValid(); NJ=It.Next())
      {
//      int NIn=0, NOut=0;
      int NTFIn=0, NTFOut=0;
      int NTCIn=0, NTCOut=0;
      int NTXIn=0, NTXOut=0;
      bool PrevInSeq=false;
      bool NextInSeq=false;
      if (DoProc)
        {
        for (int i=0; i<NJ.Nd.NoFlwIOs(); i++)
          {
          _asm int 3;
          /*** Cluster Mods
          int iRmtOrd=NJ.Nd.Nd_Rmt(i)->m_iProcOrder;
          if (NJ.Nd.IOEvalSeq_In(i) && iRmtOrd==NJ.Nd.m_iProcOrder-1)
            PrevInSeq=true;
          else if (NJ.Nd.IOEvalSeq_Out(i) && iRmtOrd==NJ.Nd.m_iProcOrder+1)
            NextInSeq=true;
          ***/
          }
        }
      else
        {
        for (int i=NJ.Nd.Ctrl1(); i<NJ.Nd.CtrlN(); i++)
          {
          _asm int 3;
          /*** Cluster Mods
          int iRmtOrd=NJ.Nd.Nd_Rmt(i)->m_iCtrlOrder;
          if (NJ.Nd.Ctrl_In(i) && iRmtOrd==NJ.Nd.m_iCtrlOrder-1 && iRmtOrd>0)
            PrevInSeq=true;
          else if (NJ.Nd.Ctrl_Out(i) && iRmtOrd==NJ.Nd.m_iCtrlOrder+1)
            NextInSeq=true;
          ***/
          }
        }
      for (int i=0; i<NJ.Nd.NoFlwIOs(); i++)
        {
        if (NJ.Nd.IOEvalSeq_In(i))
          NTFIn++;
        else if (NJ.Nd.IOEvalSeq_Out(i))
          NTFOut++;
        }
      for (i=0; i<NJ.Nd.NoCtrls(); i++)
        {
        if (NJ.Nd.Ctrl_In(i))
          NTCIn++;
        else if (NJ.Nd.Ctrl_Out(i))
          NTCOut++;
        }
      for (i=0; i<NJ.Nd.NoXRefs2Me(); i++)
        {
        if (NJ.Nd.m_XRefs2Me[i]->DstNd()==NJ.m_pNd)
          NTXIn++;
        else
          NTXOut++;
        }

      bool ThisHasNOIO=DoProc ? (NTFIn==0 && NTFOut==0) : (NTCIn+NTXIn==0 && NTCOut+NTXOut==0);
      if (PrevInSeq || PrevHasNOIO && ThisHasNOIO)
        {}
      else
        dbgpln("");

      PrevHasNOIO=ThisHasNOIO;

      int IIn=0, IOut=0;
      int CIn=NJ.Nd.Ctrl1(), COut=NJ.Nd.Ctrl1();
      int XIn=0, XOut=0;
      int NTot;
      if (DoProc)
        NTot=Max(1, Max((PrevInSeq?NTFIn-1:NTFIn)+NTCIn+NTXIn, (NextInSeq?NTFOut-1:NTFOut)+NTCOut+NTXOut));
      else
        NTot=Max(1, Max((PrevInSeq?NTCIn-1:NTCIn)+NTXIn, (NextInSeq?NTCOut-1:NTCOut)+NTXOut));
      for (i=0; i<NTot; i++)
        {
        long tlen, tlen1;

        if (i==0)
          dbgp("%*i:", iPDigs, DoProc ? NJ.Cluster.m_iProcOrder:NJ.Nd.m_iCtrlOrder);
        else
          dbgp("%*s:", iPDigs, "");

        if (DoProc)
          {
          _asm int 3;
          /*** Cluster Mods
          while (IIn<NJ.Nd.NoFlwIOs() && (!NJ.Nd.IOEvalSeq_In(IIn) || NJ.Nd.Nd_Rmt(IIn)->m_iProcOrder==NJ.Cluster.m_iProcOrder-1))
            IIn++;
          ***/
          }
       if (DoProc && NTFIn<1 && i==0)
          dbgp(" %*.*s %s|", iSDWide,iSDWide, "", NTFOut>0?">>>":"   ");
        else if (DoProc && IIn<NJ.Nd.NoFlwIOs())
          {
          tlen1=TLen(NJ.Nd.Nd_Rmt(IIn)->FullObjTag());
          int Wd=Max(0L,iSDWide-tlen1);
          if (NJ.Nd.IOFlange(IIn)->IsTear())
            dbgp(" %*.*s%s ==>|", Wd,Wd, "", NJ.Nd.Nd_Rmt(IIn)->FullObjTag());
          else
            dbgp(" %*.*s%s -->|", Wd,Wd, "", NJ.Nd.Nd_Rmt(IIn)->FullObjTag());
          IIn++;
          }
        else
          {
          if (DoProc)
            {
            while (CIn<NJ.Nd.CtrlN() && (!NJ.Nd.Ctrl_In(CIn)))
              CIn++;
            }
          else
            {
            while (CIn<NJ.Nd.CtrlN() && (!NJ.Nd.Ctrl_In(CIn) || NJ.Nd.Nd_Rmt(CIn)->m_iCtrlOrder==NJ.Nd.m_iCtrlOrder-1))
              CIn++;
            }
          if (CIn<NJ.Nd.CtrlN())
            {
            LPTSTR pS;
            if (UseConnID)
              {
              Strng ID, Tg;
              NJ.Nd.Ctrl_GetConnIDStr(CIn, ID, Tg);
              pS=ID();
              }
            else
              pS=NJ.Nd.Nd_Rmt(CIn)->FullObjTag();
            tlen1=TLen(pS);
            int Wd=Max(0L,iSDWide-tlen1);
            if (NJ.Nd.Ctrl_Terminal(CIn)->IsTear())
              dbgp(" %*.*s%s C=>|", Wd,Wd, "", pS);
            else
              dbgp(" %*.*s%s C->|", Wd,Wd, "", pS);
            CIn++;
            }
          else
            {
            if (DoProc)
              {
              while (XIn<NJ.Nd.NoXRefs2Me() && (NJ.Nd.m_XRefs2Me[XIn]->DstNd()!=NJ.m_pNd))
                XIn++;
              }
            else
              {
              while (XIn<NJ.Nd.NoXRefs2Me() && (NJ.Nd.m_XRefs2Me[XIn]->DstNd()!=NJ.m_pNd || static_cast<FlwNode*>(NJ.Nd.m_XRefs2Me[XIn]->DstNd())->m_iCtrlOrder==NJ.Nd.m_iCtrlOrder-1))
                XIn++;
              }
            if (XIn<NJ.Nd.NoXRefs2Me())
              {
              tlen1=TLen((LPTSTR)NJ.Nd.m_XRefs2Me[XIn]->SFGNodeTag(NJ.m_pNd));
              int Wd=Max(0L,iSDWide-tlen1);
              if (NJ.Nd.m_XRefs2Me[XIn]->IsTear())
                dbgp(" %*.*s%s X=>|", Wd,Wd, "", NJ.Nd.m_XRefs2Me[XIn]->SFGNodeTag(NJ.m_pNd));
              else
                dbgp(" %*.*s%s X->|", Wd,Wd, "", NJ.Nd.m_XRefs2Me[XIn]->SFGNodeTag(NJ.m_pNd));
              XIn++;
              }
            else
              {
              dbgp(" %*.*s    |", iSDWide,iSDWide, "");
              }
            }
          }

        if (i==0)
          {
          tlen=TLen(NJ.Nd.FullObjTag());
          int hSpc=(iTgWide-tlen)/2;
          int hSpc1=(iTgWide-tlen)-hSpc;
          dbgp("%*s%s%*.*s|",hSpc,"",NJ.Nd.FullObjTag(),hSpc1,hSpc1,"");
          }
        else if (NextInSeq)
          {
          tlen=1;
          int hSpc=(iTgWide-tlen)/2;
          int hSpc1=(iTgWide-tlen)-hSpc;
          dbgp("%*s%s%*.*s|",hSpc,"","|",hSpc1,hSpc1,"");
          }
        else
          dbgp("%-*.*s|", iTgWide,iTgWide, "");

        if (DoProc)
          {
          _asm int 3;
          /*** Cluster Mods
          while (IOut<NJ.Nd.NoFlwIOs() && (!NJ.Nd.IOEvalSeq_Out(IOut) || NJ.Nd.Nd_Rmt(IOut)->m_iProcOrder==NJ.Cluster.m_iProcOrder+1))
            IOut++;
          ***/
          }
        //else
        //  {
        //  while (IOut<NJ.Nd.NoFlwIOs() && (!NJ.Nd.IOEvalSeq_Out(IOut)))
        //    IOut++;
        //  }
        if (DoProc && NTFOut<1 && i==0)
          dbgp("%s", NTFIn>0?">>>":"   ");
        else if (DoProc && IOut<NJ.Nd.NoFlwIOs())
          {
          tlen1=TLen(NJ.Nd.Nd_Rmt(IOut)->FullObjTag());
          if (NJ.Nd.IOFlange(IOut)->IsTear())
            dbgp("==> %s", NJ.Nd.Nd_Rmt(IOut)->FullObjTag());
          else //if (NOut>0)
            dbgp("--> %s", NJ.Nd.Nd_Rmt(IOut)->FullObjTag());
          //else
          //  dbgp("     %s", "");
          IOut++;
          }
        else
          {
          if (DoProc)
            {
            while (COut<NJ.Nd.CtrlN() && (!NJ.Nd.Ctrl_Out(COut)))
              COut++;
            }
          else
            {
            while (COut<NJ.Nd.CtrlN() && (!NJ.Nd.Ctrl_Out(COut) || NJ.Nd.Nd_Rmt(COut)->m_iCtrlOrder==NJ.Nd.m_iCtrlOrder+1))
              COut++;
            }
          if (COut<NJ.Nd.CtrlN())
            {
            LPTSTR pS;
            if (UseConnID)
              {
              Strng ID, Tg;
              NJ.Nd.Ctrl_GetConnIDStr(COut, ID, Tg);
              pS=ID();
              }
            else
              pS=NJ.Nd.Nd_Rmt(COut)->FullObjTag();
            if (NJ.Nd.Ctrl_Terminal(COut)->IsTear())
              dbgp("C=> %s", pS);
            else
              dbgp("C-> %s", pS);
            COut++;
            }
          else
            {
            if (DoProc)
              {
              while (XOut<NJ.Nd.NoXRefs2Me() && (NJ.Nd.m_XRefs2Me[XOut]->SrcNd()!=NJ.m_pNd))
                XOut++;
              }
            else
              {
              while (XOut<NJ.Nd.NoXRefs2Me() && (NJ.Nd.m_XRefs2Me[XOut]->SrcNd()!=NJ.m_pNd || static_cast<FlwNode*>(NJ.Nd.m_XRefs2Me[XOut]->SrcNd())->m_iCtrlOrder==NJ.Nd.m_iCtrlOrder+1))
                XOut++;
              }
            if (XOut<NJ.Nd.NoXRefs2Me())
              {
              if (NJ.Nd.m_XRefs2Me[XOut]->IsTear())
                dbgp("X=> %s", NJ.Nd.m_XRefs2Me[XOut]->SFGNodeTag(NJ.m_pNd));
              else
                dbgp("X-> %s", NJ.Nd.m_XRefs2Me[XOut]->SFGNodeTag(NJ.m_pNd));
              XOut++;
              }
            else
              dbgp("     %s", "");
            }
          }
        dbgpln("");
        }
      }
    }
  }

//---------------------------------------------------------------------------

void FlwSlvBlk::ClearTOSequence(byte Which, bool DoPrevious)
  {
  if (Which & TOSL_Proc)
    {
    ProcTOSequence.RemoveAllToFreeList();
    m_fProcTOSeqValid=0;
#if dbgFlwSolve
    if (dbgEvalOrdSummary())
      dbgpln("Clear Proc TOSequence");
#endif
    }
  if (Which & TOSL_Ctrl)
    {
    CtrlTOSequence.RemoveAllToFreeList();
    m_fCtrlTOSeqValid=0;
#if dbgFlwSolve
    if (dbgEvalOrdSummary())
      dbgpln("Clear Ctrl TOSequence");
#endif
    }
  if (Which & TOSL_Comb)
    {
    CombTOSequence.RemoveAllToFreeList();
#if dbgFlwSolve
    if (dbgEvalOrdSummary())
      dbgpln("Clear Comb TOSequence");
#endif
    }

  if (DoPrevious)
    {
    if (Which & TOSL_Proc)
      ProcTOSequencePrv.RemoveAllToFreeList();
    if (Which & TOSL_Ctrl)
      CtrlTOSequencePrv.RemoveAllToFreeList();
    if (Which & TOSL_Comb)
      CombTOSequencePrv.RemoveAllToFreeList();
    }
  };

//---------------------------------------------------------------------------

void FlwSlvBlk::SwapAndClearTOSequence(byte Which)
  {
  if ((Which & TOSL_Proc) && (m_pProcTOSequence->GetCount()>0))
    {
#if dbgFlwSolve
    if (dbgEvalOrdSummary())
      dbgpln("Swap and Clear Proc TOSequence");
#endif
    Exchange(m_pProcTOSequence, m_pProcTOSequencePrv);
    m_pProcTOSequence->RemoveAllToFreeList();
    m_fProcTOSeqValid=0;
    }
  if ((Which & TOSL_Ctrl) && (m_pCtrlTOSequence->GetCount()>0))
    {
#if dbgFlwSolve
    if (dbgEvalOrdSummary())
      dbgpln("Swap and Clear Ctrl TOSequence");
#endif
    Exchange(m_pCtrlTOSequence, m_pCtrlTOSequencePrv);
    m_pCtrlTOSequence->RemoveAllToFreeList();
    m_fCtrlTOSeqValid=0;
    }
  if ((Which & TOSL_All) && (m_pCombTOSequence->GetCount()>0))
    {
#if dbgFlwSolve
    if (dbgEvalOrdSummary())
      dbgpln("Swap and Clear Comb TOSequence");
#endif
    Exchange(m_pCombTOSequence, m_pCombTOSequencePrv);
    m_pCombTOSequence->RemoveAllToFreeList();
    }
  };

//---------------------------------------------------------------------------

void FlwSlvBlk::FindTOSequence(byte Flags, byte Which)
  {
  if (Which & TOSL_Proc)
    {
    CFlwNodeIter All(m_AllNds);
    for (FlwNode * p = All.First(); (p); p=All.Next())
      if (p->IsLnk())
        {
        FlwBlk &FB=*p->IOFB(0,0);//*(p->FBlk());
//dbgpln("%10.3e %s", FB.GetQmEst(), p->Tag());
        sint NewDirn = (sint)ZSign(FlwExists(FB.GetQmEst()) ? FB.GetQmEst() : 0.0);
        if (FB.PrevDirn!= NewDirn)
          {
          #if dbgFlwSolve
          if (dbgEvalOrdChg())
            dbgpln("Q Changed %i -> %i %14g %s" ,FB.PrevDirn, NewDirn, FB.GetQm(), p->FullObjTag());
          #endif
          FB.PrevDirn = NewDirn;
          m_fProcTOSeqValid=0;
          }
        }
    }

  bool DoingDynMode=(Flags & FTS_DynMode);

  int DoDump=0;
  if (Which & TOSL_Proc)
    {
    if (!m_fProcTOSeqValid)
      {
      DoDump=1;

      SwapAndClearTOSequence(TOSL_Proc);

      m_TearFlanges.RemoveAllToFreeList();

      FlwSlvNd.StructureChanged(&FlwSlvNd);

      //dbgpln("EvalProductClusters -----------------------------");
      FlwNode * p;
      CFlwNodeIter Flw(m_ProcessNds);
      for (p = Flw.First(); (p); p=Flw.Next())
        {
        long Mask;
        int c=0;

        for (int i = 0; i < p->NoFlwIOs(); i++)
          {
          p->SetIOEvalSeq_WasIn (i, p->IOEvalSeq_In(i));
          p->SetIOEvalSeq_WasOut(i, p->IOEvalSeq_Out(i));
          }

        //dbgpln("%s", p->Tag());
        while (p->EvalProductClusters(c, Mask))
          {
          if (p->m_Clusters.GetSize()<=c)
            p->m_Clusters.SetSize(c+1);
          CJoinCluster &C=p->m_Clusters[c];
          C.m_iCluster=c;
          C.m_iJoinMask=Mask;
          C.m_iProcOrder=iNotInEvalOrder;
          C.m_nFlwIOsIn=0;
          C.m_nFlwIOsOut=0;
          C.m_nFlwIOsInDone=0;
          C.m_nFlwIOsInReqd=0;
          C.m_nFlwIOs=0;
          //dbgp("                             %2i %#06.6x : ", c, Mask);
          for (int io=0; io<p->NoFlwIOs(); io++)
            {
            if (io<p->NoProcLnkIOs())
              {
              if (fabs(p->IOQmEst_In(io)-p->Nd_Rmt(io)->IOQmEst_Out(p->IOIONo_Rmt(io)))>1.0e-3)
                {
                dbgpln("BAD FLOW ESTIMATES %15.2f != %15.f @ %s > %s",
                  p->IOQmEst_In(io), p->Nd_Rmt(io)->IOQmEst_Out(p->IOIONo_Rmt(io)),
                  p->FullObjTag(), p->Nd_Rmt(io)->FullObjTag());
                }
              }
            if ((1<<p->IO_Join_Id(io)) & Mask)
              {
              //dbgp(" %2i", io);
              C.m_IOs.SetAtGrow(C.m_nFlwIOs++, io);
              p->SetIO_Cluster_Id(io, c);
              }
            }
          //dbgpln("");

          c++;
          }
        p->m_Clusters.SetSize(c);

        p->Busy = p->Done = 0;
        }
      ///dbgpln("-------------------------------------------------");

      for (p = Flw.First(); (p); p=Flw.Next())
        {
        ASSERT_RDB(p->IsProcessSeq(), "Bad Proc Seq", __FILE__, __LINE__);
        if (p->IsLnk())
          {
          ASSERT(p->NClusters()==1);
          bool Fwd      = !p->IO_Out(0);
          bool Zero0    = p->IO_Zero(0);
          bool Zero1    = p->IO_Zero(1);
          bool WasIn0   = p->IOEvalSeq_WasIn (0);
          bool WasOut0  = p->IOEvalSeq_WasOut(0);
          bool WasIn1   = p->IOEvalSeq_WasIn (1);
          bool WasOut1  = p->IOEvalSeq_WasOut(1);
          
          if (p->fDoDbgBrk)
            { int xxx=0; }

          p->SetIOEvalSeq_In (0, Fwd && !Zero0);
          p->SetIOEvalSeq_Out(0, !Fwd && !Zero0);
          p->SetIOEvalSeq_In (1, !Fwd && !Zero1);
          p->SetIOEvalSeq_Out(1, Fwd && !Zero1);

          if (p->IOEvalSeq_In(0) || p->IOEvalSeq_In(1))
            p->m_Clusters[0].m_nFlwIOsIn++;
          if (p->IOEvalSeq_Out(0) || p->IOEvalSeq_Out(1))
            p->m_Clusters[0].m_nFlwIOsOut++;

          if (0)
            dbgpln("Lnk %15.5e %15.5e %-5s %-5s %-5s %i %i %s", p->IOQmEst_In(0), p->IOQmEst_In(1), Fwd?"Fwd":"",Zero0?"Zero0":"",Zero1?"Zero1":"",p->m_Clusters[0].m_nFlwIOsIn, p->m_Clusters[0].m_nFlwIOsOut, p->Tag());

          for (int i=p->NoProcLnkIOs(); i<p->NoFlwIOs(); i++)
            {

            p->SetIOEvalSeq_In(i, p->IO_In(i) && !p->IO_Zero(i));
            p->SetIOEvalSeq_Out(i, p->IO_Out(i) && !p->IO_Zero(i));
            if (p->IOEvalSeq_In(i))
              p->m_Clusters[0].m_nFlwIOsIn++;
            if (p->IOEvalSeq_Out(i))
              p->m_Clusters[0].m_nFlwIOsOut++;
  
            //if (IsDirectIOId(p->IOId_Self(i)))
            //  {
            //  p->SetIOEvalSeq_Out(i, true);
            //  p->m_nFlwIOsOut++;
            //  }
            }
          }
        else
          {
          if (0)
            dbgpln("Nd   %s", p->Tag());
          for (int i = 0; i < p->NoFlwIOs(); i++)
            {
            if (0)
              dbgpln("  IO %5i %15.5e %-5s %-5s >> %s", i, p->IOQmEst_In(i), p->IO_In(i)?"In":"  ", p->IO_Out(i)?"Out":"   ", p->Nd_Rmt(i)->Tag());

            if (!p->IO_In(i) && !p->IO_Out(i))
              {
              p->SetIOEvalSeq_In(i, false);
              p->SetIOEvalSeq_Out(i, false);
              }
            else
              {
              p->SetIOEvalSeq_In(i, p->IO_In(i));
              p->SetIOEvalSeq_Out(i, p->IO_Out(i));
              }
            if (p->IO_Cluster_Id(i)>=0)
              {
              if (p->IOEvalSeq_In(i))
                p->m_Clusters[p->IO_Cluster_Id(i)].m_nFlwIOsIn++;
              if (p->IOEvalSeq_Out(i))
                p->m_Clusters[p->IO_Cluster_Id(i)].m_nFlwIOsOut++;
              }
            }
          }
        for (int c=0; c<p->m_Clusters.GetCount(); c++)
          {
          CJoinCluster &C=p->m_Clusters[c];
          C.m_nFlwIOsInReqd=C.m_nFlwIOsIn;
          }

        #if dbgFlwSolve
        if (dbgEvalOrdFind())
          {                                                                
          dbgpln("EvalSeq ----------------------------------------------------------------------------------------------------- %-3s %s",
            p->IsLnk()?"Lnk":"", p->FullObjTag());

          for (int c=0; c<p->m_Clusters.GetCount(); c++)
            {
            CJoinCluster &C=p->m_Clusters[c];
            dbgpln("  Cluster<%3i> FlwsInRqd %i FlwsIn %i FlwsOut %i", c, C.m_nFlwIOsInReqd, C.m_nFlwIOsIn, C.m_nFlwIOsOut);
            for (int ci=0; ci<C.NoIOs(); ci++)
              {
              int i=C.IONo(ci);
              //p->IO_Cluster_Self(i) ? p->IO_Cluster_Self(i)->m_nFlwIOsIn: -1,
              //p->IO_Cluster_Self(i) ? p->IO_Cluster_Self(i)->m_nFlwIOsOut: -1,

              bool OpenNotClosed=!p->IO_Closed_Self(i) && !p->IO_Closed_Rmt(i) && (p->IO_Open_Self(i) || p->IO_Open_Rmt(i));
              //dbgpln("    %-5s %-6s %-6s %-6s %-6s %-5s %-5s %4i %4i %-10s %-7s          ->%s",
              dbgpln("    IO[%3i] %-5s %-6s %-6s %-6s %-6s %-5s %-5s %-10s %-7s          ->%s",
                i,
                OpenNotClosed?"OpenNC":"",
                p->IO_Closed_Self(i)?"ClsSlf":"",
                p->IO_Closed_Rmt(i)?"ClsRmt":"",
                p->IO_Open_Self(i)?"OpnSlf":"",
                p->IO_Open_Rmt(i)?"OpnRmt":"",
                p->IOEvalSeq_In(i)?"EsIn":"",
                p->IOEvalSeq_Out(i)?"EsOut":"",
                //p->IO_Cluster_Self(i) ? p->IO_Cluster_Self(i)->m_nFlwIOsIn: -1,
                //p->IO_Cluster_Self(i) ? p->IO_Cluster_Self(i)->m_nFlwIOsOut: -1,
                p->IO_EvalSeqStartDenied(i)?"StartDeny":"",
                p->IO_EvalSeqStartOK(i)?"StartOK":"",
                p->Nd_Rmt(i)->FullObjTag());
              }
            }
          }
        #endif
        }

      CSFGNodeInfoList ManualTearSFGNodes;
      CFlangeIter F(m_Flanges);
      for (CFlange* pF = F.First(); (pF); pF=F.Next())
        {
        pF->fIsBuffered = (pF->fIsBufferPos && DoingDynMode) || (pF->RqdTearType()==TT_Break);
        pF->SetIsEvalSeqStart(false);
        pF->SetTearType(TT_NoTear, NULL);
        // Preferred Tears @ Buffers
        if (pF->RqdTearType()==TT_ManualTear || pF->RqdTearType()==TT_Break)
          ManualTearSFGNodes.AddTail((CSFGNodeInfo*)pF);
        }

      SetUpTearLinks(ManualTearSFGNodes);
      BuildSignalFlowGraph(DoingDynMode, Which);

      if (m_ProcSigFlwGrf.TearDirect()>0)
        SetUpTearLinks(m_ProcSigFlwGrf.TearNodeList());

#if 01
      // Corrected Version
      FindTOSequenceStart(Which);
      BuildTOSequence(Which);
      SummariseTOSequence(Which);
      InitTearConvergence(EqnCB());
      m_fProcTOSeqValid=true;
#else                                                                10001000  
      // Incorrect Version (Slow)
      m_fProcTOSeqValid=FindTOSequenceStart(Which);
      if (m_fProcTOSeqValid)
        {
        BuildTOSequence(Which);
        SummariseTOSequence(Which);
        InitTearConvergence(EqnCB());
        }
#endif
      }
    }


  if (Which & TOSL_Ctrl)
    {
    if (!m_fCtrlTOSeqValid)
      {
      DoDump=1;
      SwapAndClearTOSequence(TOSL_Ctrl);
      m_TearTerminals.RemoveAllToFreeList();
      m_TearXRefs.RemoveAllToFreeList();

      FlwSlvNd.StructureChanged(&FlwSlvNd);

      CFlwNodeIter Ctrl(m_PwrCtrlNds);
      for (FlwNode * p = Ctrl.First(); (p); p=Ctrl.Next())
        {
        p->m_iCtrlOrder=iNotInEvalOrder;

        p->Busy = p->Done = 0;

        p->m_nCtrlIOsIn=0;
        p->m_nCtrlIOsOut=0;
        p->m_nSeqCtrlIOsIn=0;
        p->m_nSeqCtrlIOsOut=0;
        p->m_nXRefsIn=0;
        p->m_nXRefsOut=0;
        p->m_nSeqXRefsIn=0;
        p->m_nSeqXRefsOut=0;
        p->m_nCtrlsInDone=0;
        p->m_nCtrlsInReqd=0;
        }

      #if dbgFlwSolve
      if (dbgXRefs())
        {
        dbgsep();
        dbgpln("      %-20s [ Xi  Xo CXi CXo]", "");
        }
      #endif

      for (p = Ctrl.First(); (p); p=Ctrl.Next())
        {
        ASSERT_RDB(p->InPwrCtrlSeq(), "Bad Ctrl Seq", __FILE__, __LINE__);
        for (int i = p->Ctrl1(); i < p->CtrlN(); i++)
          {
          if (p->Ctrl_In(i))
            p->m_nCtrlIOsIn++;
          else if (p->Ctrl_Out(i))
            p->m_nCtrlIOsOut++;
          if (p->InPwrCtrlSeq() && p->Nd_Rmt(i)->InPwrCtrlSeq())
            {
            if (p->Ctrl_In(i))
              p->m_nSeqCtrlIOsIn++;
            else if (p->Ctrl_Out(i))
              p->m_nSeqCtrlIOsOut++;
            }
          }

        for (i=0; i<p->NoXRefs2Me(); i++)
          {
          CXRefItem *pRef=p->CXRef(i);
          FlwNode *pSrc=p->CXRef_SrcNd(i);
          FlwNode *pDst=p->CXRef_DstNd(i);
          if (pDst==p)
            p->m_nXRefsIn++;
          if (pSrc==p)
            p->m_nXRefsOut++;
          if (pSrc && pSrc->InPwrCtrlSeq() && pDst && pDst->InPwrCtrlSeq())
            {
            // protect against selfloop
            if (!p->CXRef_SelfLoop(i))
              {
              if (pDst==p)
                p->m_nSeqXRefsIn++;
              if (pSrc==p)
                p->m_nSeqXRefsOut++;
              }
            }
          #if dbgFlwSolve
          if (dbgXRefs())
            {
            dbgpln("XRef: %-20s [%3i %3i %3i %3i] %-20s",
              i==0?p->FullObjTag():"",
              p->m_nXRefsIn,p->m_nXRefsOut,
              p->m_nSeqXRefsIn,p->m_nSeqXRefsOut,
              pRef->SFGNodeTag(p));
            }
          #endif
          }
        }
      CTerminalIter T(m_Terminals);
      for (CTerminal* pT = T.First(); (pT); pT=T.Next())
        {
        pT->SetIsEvalSeqStart(false);
        pT->SetIsTear(false);
        }

      CXRefPtrIter X(FlwSlvNd.m_XRefsList);
      for (CXRefItem * pX = X.First(); (pX); pX=X.Next())
        {
        pX->SetIsEvalSeqStart(false);
        pX->SetIsTear(false);
        }

      BuildSignalFlowGraph(DoingDynMode, Which);
      if (m_CtrlSigFlwGrf.TearDirect()>0)
        SetUpTearCLinks(m_CtrlSigFlwGrf.TearNodeList());

#if 01
      FindTOSequenceStart(Which);
      BuildTOSequence(Which);
      SummariseTOSequence(Which);
      m_fCtrlTOSeqValid=true;
#else
      m_fCtrlTOSeqValid=FindTOSequenceStart(Which);
      if (m_fCtrlTOSeqValid)
        {
        BuildTOSequence(Which);
        SummariseTOSequence(Which);
        }
#endif
      }
    }
  if (0 && DoDump)
    DumpEvalOrder(Which);
  };

//--------------------------------------------------------------------------

void FlwSlvBlk::InitTearConvergence(EqnSlvCtrlBlk & EqnCB)
  {
  EqnSlv().DeActivateAllTears();
  EqnSlv().ActivateAllReqdTears();
  EqnSlv().InitialiseActiveTears(EqnCB);
//  EqnSlv().RemoveInActiveTears();
  }

//--------------------------------------------------------------------------

void FlwSlvBlk::InitialiseAllTears(EqnSlvCtrlBlk & EqnCB)
  {
  EqnSlv().InitialiseActiveTears(EqnCB);
  }

//--------------------------------------------------------------------------

void FlwSlvBlk::RestartAllTears(EqnSlvCtrlBlk & EqnCB)
  {
  EqnSlv().RestartAllActiveTears(EqnCB);
  }

//--------------------------------------------------------------------------

void FlwSlvBlk::TransferAllTears(EqnSlvCtrlBlk & EqnCB)
  {
  EqnSlv().TransferAllTears(EqnCB);
  }

//--------------------------------------------------------------------------

flag FlwSlvBlk::TestTearConvergence(long ConvergeLoopCnt, EqnSlvCtrlBlk & EqnCB, Strng_List &BadTearTags, Strng_List & BadTearInfo)
  {
  return EqnSlv().TestTearConvergence(ConvergeLoopCnt, EqnCB, BadTearTags, BadTearInfo);
  }

//===========================================================================

const int MaxSpModelCount=16;
struct CSpModelCount
  {
  SpConduit*    m_pCd;
  TagObjClass*  m_pClass;
  int           m_iCount;
  double        m_Qt;
  };

//===========================================================================

double FlwSlvBlk::TestBalance(const EqnSlvCtrlBlk & TCtrl, CNodeEvalIndexList &Nds, pStrng_List pTBReport)
  {
  SpMArray GSpI, GSpO;
  double GQtI=0.0, GQtO=0.0;
  double GHtI=0.0, GHtO=0.0;
  Strng RptLn;

  if (pTBReport) pTBReport->Clear();
/********
  dbgpln("========================== TestBalance ==========================");
  CFlwNodeIter Nd(Nds);
  for (FlwNode * p = Nd.First(); (p) ; p = Nd.Next())
    {
    //Conduit LSpOOB;
    SpMArray LSpI, LSpO;
    double LHtI=0.0, LHtO=0.0;

    double RefTemp=C_2_K(0.0);
    int iSrcSnk=p->ClosureInfo(RefTemp, LSpI, LHtI, LSpO, LHtO);

    double LQtI=LSpI.Mass();
    double LQtO=LSpO.Mass();

    int nModels=0;
    CSpModelCount Models[MaxSpModelCount];

    for (int i=0; i<p->NoFlwIOs(); i++)
      {
      SpConduit & C=*(p->IOConduit(i));
      if (p->IO_In(i))
        {
        LSpI += C.MArray();
        LQtI += C.QMass();
        LHtI += C.totEnthalpy();
        }
      else
        {
        LSpO += C.MArray();
        LQtO += C.QMass();
        LHtO += C.totEnthalpy();
        }

      for (int j=0; j<nModels; j++)
        if (Models[j].pClass==C.pModel->Class())
          break;
      j=Min(j, MaxSpModelCount-1);
      if (j==nModels)
        {
        nModels++;
        Models[j].pClass=C.pModel->Class();
        Models[j].iCount=0;
        }
      Models[j].iCount++;
      }

    GSpI += LSpI;
    GQtI += LQtI;
    GHtI += LHtI;
    GSpO += LSpO;
    GQtO += LQtO;
    GHtO += LHtO;

    SpMArray SpErr;
    double QtErr=(LQtI-LQtO)/NZ(LQtI+LQtO);
    double HtErr=(LHtI-LHtO)/NZ(LHtI+LHtO);
    double MaxErr=Max(fabs(QtErr), fabs(HtErr));
    double MaxSpErr=0.0;
    for (int s=0; s < SDB.No(); s++)
      {
      SpErr[s]=(LSpI[s]-LSpO[s])/NZ(LSpI[s]+LSpO[s]);
      MaxSpErr=Max(MaxSpErr, fabs(SpErr[s]));
      }

    flag DoRpt=0;
    switch (iSrcSnk)
      {
      case -1: DoRpt = (Max(MaxErr, MaxSpErr)>EqnCB().ReportTol()) && (EqnCB().Report() & ESR_Srcs); break;
      case  1: DoRpt = (Max(MaxErr, MaxSpErr)>EqnCB().ReportTol()) && (EqnCB().Report() & ESR_Snks); break;
      case  0: DoRpt = (((Max(MaxErr, MaxSpErr)>EqnCB().ReportTol()) && (EqnCB().Report() & ESR_Species)) ||
                        ((MaxErr>EqnCB().ReportTol()) && (EqnCB().Report() & ESR_Errors))); break;
      }

    if (DoRpt)
      {
      Strng Why;
      switch (iSrcSnk)
        {
        case -1: Why="Src"; break;
        case  1: Why="Snk"; break;
        case  0: Why="Err"; break;
        }
      dbgpln("%-7.7s ----- In  ---------- Out  ---- Error", Why());
      if (fabs(QtErr)>EqnCB().ReportTol())
        {
        dbgpln("  %14.6g  %14.6g  %9.4f%%  %s.%s",LQtI, LQtO, 100.0*Range(-1.0, QtErr, 1.0), p->FullObjTag(), "Qm Total");
        if (pTBReport)
          {
          RptLn.Set("%s\t%s\t%s\t%14.6g\t%14.6g\t%9.4f", Why(), p->FullObjTag(), "Qm Total",LQtI, LQtO, 100.0*Range(-1.0, QtErr, 1.0));
          pTBReport->Append(RptLn());
          }
        }

      if (fabs(HtErr)>EqnCB().ReportTol())
        {
        dbgpln(" %14.6g  %14.6g  %9.4f%%  %s.%s",LHtI, LHtO, 100.0*Range(-1.0, HtErr, 1.0), p->FullObjTag(), "totEnthalpy");
        if (pTBReport)
          {
          RptLn.Set("%s\t%s\t%s\t%14.6g\t%14.6g\t%9.4f", Why(), p->FullObjTag(), "totEnthalpy",LHtI, LHtO, 100.0*Range(-1.0, HtErr, 1.0));
          pTBReport->Append(RptLn());
          }
        }

      for (s=0; s < SDB.No(); s++)
        if (fabs(SpErr[s])>EqnCB().ReportTol())
          {
          dbgpln(" %14.6g  %14.6g  %9.4f%%  %s.%s",LSpI[s], LSpO[s], 100.0*Range(-1.0, SpErr[s], 1.0), p->FullObjTag(), SDB[s].SymOrTag());
          if (pTBReport)
            {
            RptLn.Set("%s\t%s\t%s\t%14.6g\t%14.6g\t%9.4f", Why(), p->FullObjTag(), SDB[s].SymOrTag(), LSpI[s], LSpO[s], 100.0*Range(-1.0, SpErr[s], 1.0));
            pTBReport->Append(RptLn());
            }
          }
      }
    }

  SpMArray SpErr;
  double QtErr=(GQtI-GQtO)/NZ(GQtI+GQtO);
  double HtErr=(GHtI-GHtO)/NZ(GHtI+GHtO);
  double MaxErr=Max(fabs(QtErr), fabs(HtErr));
  double MaxSpErr=0.0;
  for (int s=0; s < SDB.No(); s++)
    {
    SpErr[s]=(GSpI[s]-GSpO[s])/NZ(GSpI[s]+GSpO[s]);
    MaxSpErr=Max(MaxSpErr, fabs(SpErr[s]));
    }

  flag DoRpt = (((Max(MaxErr, MaxSpErr)>EqnCB().ReportTol()) && (EqnCB().Report() & ESR_Species)) ||
                ((MaxErr>EqnCB().ReportTol()) && (EqnCB().Report() & ESR_Errors)));
  if (DoRpt)
    {
    dbgpln("OverAll----- In  ---------- Out  ---- Error");
    if (fabs(QtErr)>EqnCB().ReportTol())
      {
      dbgpln("  %14.6g  %14.6g  %9.4f%%  %s.%s",GQtI, GQtO, 100.0*Range(-1.0, QtErr, 1.0), FlwSlvNd.FullObjTag(), "Mass Flow");
      if (pTBReport)
        {
        RptLn.Set("Err\t%s\t%s\t%14.6g\t%14.6g\t%9.4f", "Overall", "Mass Flow",GQtI, GQtO, 100.0*Range(-1.0, QtErr, 1.0));
        pTBReport->Append(RptLn());
        }
      }
    if (fabs(HtErr)>EqnCB().ReportTol())
      {
      dbgpln(" %14.6g  %14.6g  %9.4f%%  %s.%s",GHtI, GHtO, 100.0*Range(-1.0, HtErr, 1.0), FlwSlvNd.FullObjTag(), "totEnthalpy");
      if (pTBReport)
        {
        RptLn.Set("Err\t%s\t%s\t%14.6g\t%14.6g\t%9.4f", "Overall", "totEnthalpy",GHtI, GHtO, 100.0*Range(-1.0, HtErr, 1.0));
        pTBReport->Append(RptLn());
        }
      }

    for (s=0; s < SDB.No(); s++)
      if (fabs(SpErr[s])>EqnCB().ReportTol())
        {
        dbgpln(" %14.6g  %14.6g  %9.4f%%  %s.%s",GSpI[s], GSpO[s], 100.0*Range(-1.0, SpErr[s], 1.0), FlwSlvNd.FullObjTag(), SDB[s].SymOrTag());
        if (pTBReport)
          {
          RptLn.Set("Err\t%s\t%s\t%14.6g\t%14.6g\t%9.4f", "Overall", SDB[s].SymOrTag(), GSpI[s], GSpO[s], 100.0*Range(-1.0, SpErr[s], 1.0));
          pTBReport->Append(RptLn());
          }
        }
    }

  dbgpln("=================================================================");

  return MaxErr;
  */
  return 0.0;
  }

//===========================================================================

void FlwSlvBlk::ShowFlows(CNodeEvalIndexList &Nds, LPCTSTR Where)
  {
  dbgpln("========================== %s", Where);
  CNodeEvalIndexIter Nd(Nds);
  for (CNodeEvalIndex NJ = Nd.First(); Nd.ItemValid() ; NJ = Nd.Next())
    {
    dbgpln("-----%s",NJ.Nd.FullObjTag());
    for (int i=0; i<NJ.Nd.NoFlwIOs(); i++)
      dbgpln("  %2i %-10.10s Qm:%14.6g Est:%14.6g  > %s",i,NJ.Nd.IODesc_Self(i)->IOName(),NJ.Nd.IOQm_In(i), NJ.Nd.IOQmEst_In(i), NJ.Nd.Nd_Rmt(i)->FullObjTag());

    for (int j=0; j<NJ.Nd.NoJoins(); j++)
      {
      CJoinRec &J=NJ.Nd.Joins[j];
      if (J.fIs2IO)// && J.HasFBlk())//())
        {
        FlwBlk &FB=*J.IOFB(0,0);//*(J.FBlk());
        dbgp("  Lnk:");
        //byte tt= NJ.Nd.Internal ? TT_NoTear : (J.Flng(0)->IsTear() ? J.Flng(0)->TearType() : J.Flng(1)->TearType());
        int tt= J.Flng(0)?(J.Flng(0)->IsTear() ? J.Flng(0)->TearType() : J.Flng(1)?J.Flng(1)->TearType():-1):-2;
        dbgp("  %+8.8s", tt==-1 ? "?1" : tt==-2 ? "?0" : tt==TT_SystemTear ? "Tear" : tt==TT_ManualTear ? "Man_Tear" : tt==TT_Break ? "Break" : "");
        Strng Q;
        if (fabs(FB.GetQm())>1.0e99)
          Q.Set(FB.GetQm()>0 ? "+++" : "---");
        else if (fabs(FB.GetQm())>1.0e6)
          Q.Set("%14.5g",FB.GetQm());
        else
          Q.Set("%14.7f",FB.GetQm());
        dbgp(" %14s", Q());
        dbgp(" Est:%14.6g", FB.GetQmEst());
        dbgpln("      {%2i}%.20s",j,NJ.Nd.FullObjTag());
        }
      else if (J.fIsTie)//())
        {
        double Q,Sq=0,SAq=0;
        for (int i = 0; i < J.NConns(); i++)
          {
          Q = J.IOFB(i,0)->GetQm()*J.QSgn(i);
          Sq = Sq + Q;
          if (Q >= 0.0)
            SAq = SAq + fabs(Q);
          }
        dbgp("  Tie : %#13.6g             ",SAq);
        if (fabs(Sq/Max(1.0e-10,SAq))>1.0e-8 && !J.fInXferNet)
          dbgp("err:%#13.6g %%  ", 100.0*Sq/Max(1.0e-10,SAq));
        else
          dbgp("                     ");
        dbgpln(" {%2i}%.20s",j,NJ.Nd.FullObjTag());
        }
      }
    }
  dbgpln("==========================");
  }

//--------------------------------------------------------------------------

void FlwSlvBlk::EvalAllPowerRequired()
  {
  CThreadTimer  XT(eFITPowerRequired, FSW_Null);
  CStopWatchLap SWLapPowerRequired(gs_swPowerRequired);

  CFlwNodeIter All(m_AllNds);

  for (FlwNode * p=All.First(); (p); p=All.Next())
    {
    XT.StartLoop(p);
    XT.LockDone();//(p, eFITPowerRequired, FSW_Null);
    p->EvalPowerRequired();
    XT.ExecDone();//Postamble(p, eFITPowerRequired, FSW_Null);
    XT.EndLoop();
    }
  XT.Complete();//PostambleTl();
  }

//--------------------------------------------------------------------------

void FlwSlvBlk::EvalAllElectrics(bool ConvergeIt)
  {
  CThreadTimer  XT(eFITElectrics, FSW_Null);
  CStopWatchLap SWLapElectrics(gs_swElectrics);

  CFlwNodeIter All(m_AllNds);

  m_TSCA.InitialiseCollection();
  for (FlwNode * p=All.First(); (p); p=All.Next())
    {
    XT.StartLoop(p);
    XT.LockDone();//(p, eFITElectrics, FSW_Null);
    p->CollectElectrics(m_TSCA);
    XT.ExecDone();//Postamble(p, eFITElectrics, FSW_Null);
    XT.EndLoop();
    }
  m_TSCA.SolveElectrics(ConvergeIt);
  XT.Complete();//PostambleTl();
  }

//--------------------------------------------------------------------------

void FlwSlvBlk::InitClosure()
  {
  CFlwNodeIter All(m_AllNds);
  SpModel *pRefMdl=NULL;
  for (FlwNode * p=All.First(); (p); p=All.Next())
    {
    if (!p->NetProbalMethod())
      {
      p->GetClosureInfo(false, true);
      for (int j=0; j<p->NoProcessJoins(); j++)
        {
        CClosureInfo & CI=p->m_Closure[j];
        CI.m_InitialContent=CI.m_ContentMass;
        CI.m_InitialHeat=CI.m_ContentHeat;
        }
      }
    for (int j=0; j<p->m_Closure.GetSize(); j++)
      {
      CClosureInfo & CI=p->m_Closure[j];
      CI.m_NettMass=dNAN;
      CI.m_NettHeat=dNAN;
      }
    }
  };

//--------------------------------------------------------------------------

void FlwSlvBlk::MarkClosureDataAvail(CNodeEvalIndexList &List)
  {
  CNodeEvalIndexIter L(List);
  for (CNodeEvalIndex NJ=L.First(); L.ItemValid(); NJ=L.Next())
    NJ.Nd.m_Closure.SetDataAvail(true);
  }

//--------------------------------------------------------------------------

void FlwSlvBlk::MarkClosureDataInvalid(CFlwNodeList &List)
  {
  CFlwNodeIter L(List);
  for (FlwNode * p=L.First(); (p); p=L.Next())
    {
    p->m_Closure.SetDataValid(false);
    p->m_Closure.SetTestDone(false);
    }
  }

//--------------------------------------------------------------------------

void FlwSlvBlk::TestClosure(CNodeEvalIndexList &TOSequence)
  {
  flag WrtHd=1;
  CNodeEvalIndexIter Order(TOSequence);
  double DTime=GTZ(ICGetTimeInc());
  for (CNodeEvalIndex NJ=Order.First(); Order.ItemValid(); NJ=Order.Next())
    {
    if (NJ.Nd.m_Closure.TestDone())
      continue;

    #define dbgClosure 0
    #if dbgClosure
    dbgpln("Test Closure -------------------------------------------------------- %s", NJ.Nd.Tag());
    #endif

    NJ.Nd.m_Closure.Setup(true, NJ.Nd.NoJoins());
    for (int iJoin=0; iJoin<NJ.Nd.NoJoins(); iJoin++)
      {
      CClosureInfo &CI=NJ.Nd.m_Closure[iJoin];
      CJoinRec & J=NJ.Nd.Joins[iJoin];

      int nModels=0;
      CSpModelCount Models[MaxSpModelCount];
      SpConduit* CdPtrs[MaxIOList];
      bool  IsIn[MaxIOList];
      bool  IsOut[MaxIOList];

      CSpMdlSlct Slct;
      for (int c=0; c<J.NConns(); c++)
        {
        IsIn[c]   = J.FBQm_In(c)>0.0;
        IsOut[c]  = J.FBQm_In(c)<0.0;
        SpConduit* & Cd = CdPtrs[c];
        if (J.IsIO(c))
          {
          int i=J.IONo(c);
          CFlange* pF=NJ.Nd.IOFlange(i);
          if (pF->IsTear() && !pF->fIsBuffered)
            {
            if (NJ.Nd.IO_In(i))
              Cd=pF->pQIO;// TearOutput
            else
              Cd=pF->pQIn;// TearInput;
            }
          else
            Cd=NJ.Nd.IOConduit(i);
          }
        else
          {
          Cd=&J.Link(c)->Cd;
          }
        for (int m=0; m<nModels; m++)
          if (Models[m].m_pClass==Cd->pModel->Class())
            break;
        m=Min(m, MaxSpModelCount-1);
        if (m==nModels)
          {
          nModels++;
          Models[m].m_pClass=Cd->pModel->Class();
          Models[m].m_pCd=Cd;
          Models[m].m_iCount=0;
          Models[m].m_Qt=0.0;
          }
        Models[m].m_iCount++;
        Models[m].m_Qt+=Cd->QMass();
        Slct.Add(Cd->Model(), Cd->QMass());

        #if dbgClosure
        dbgpln("JC %s %2i.%2i %18s %18s %18s --> %s", J.IsIO(c)?"IO ":"Lnk", iJoin, c,
          DbgFltString(J.FBQm_In(c),3,3), DbgFltString(Cd->QMass(),3,3), "", J.Nd_Rmt(c)->Tag());
        #endif
        }

      CI.m_pRefMdl=Slct.RequiredMdl();
      NJ.Nd.GetClosureInfo(true, true);

      double QmIn   = CI.m_MassGain;
      double QmOut  = CI.m_MassLoss;
      double HtIn   = CI.m_HfGain+CI.m_PowerIn+CI.m_EHXPowerIn+CI.m_RCTPowerIn;
      double HtOut  = CI.m_HfLoss;

      //dbgpln("-------------------- %s", NJ.Nd.FullObjTag());
      for (c=0; c<J.NConns(); c++)
        {
        SpConduit* Cd=CdPtrs[c];
        if (IsIn[c])//NJ.Nd.IOQm_In(i)>0.0)
          {
          //SpMIn += pC->MArray();
          QmIn += Cd->QMass();
          if (SpModel::iEnthalpyStrategy==SPMES_BestTemp ||
            (int)(Cd->Model()->RefTemp()-CI.m_pRefMdl->RefTemp()))
            HtIn += CI.m_pRefMdl->totHf(som_ALL, Cd->Temp(), Cd->Press(), Cd->pMArray());
          else
            HtIn += Cd->totHf();
          }
        else if (IsOut[c])//NJ.Nd.IOQm_Out(i)>0.0)
          {
          //SpMOut += Cd->MArray();
          QmOut += Cd->QMass();
          if (SpModel::iEnthalpyStrategy==SPMES_BestTemp ||
            (int)(Cd->Model()->RefTemp()-CI.m_pRefMdl->RefTemp()))
            HtOut += CI.m_pRefMdl->totHf(som_ALL, Cd->Temp(), Cd->Press(), Cd->pMArray());
          else
            HtOut += Cd->totHf();
          }
        }

      double CnQmIn = 0;
      double CnQmOut = 0;
      double CnHtIn = 0;
      double CnHtOut = 0;
      if (!NJ.Nd.NetProbalMethod())
        {
        NJ.Nd.GetClosureInfo(false, true);

        CnQmIn += CI.m_InitialContent/DTime;
        CnQmOut += CI.m_ContentMass/DTime;
        CnHtIn += CI.m_InitialHeat/DTime;
        CnHtOut += CI.m_ContentHeat/DTime;
        }

      double TotQmIn  = QmIn+CnQmIn;
      double TotQmOut = QmOut+CnQmOut;
      double TotHtIn  = HtIn+CnHtIn;
      double TotHtOut = HtOut+CnHtOut;

      double NettQm = TotQmIn-TotQmOut;
      double NettHt = TotHtIn-TotHtOut;

      #if dbgClosure
      dbgpln("NettQm       %18s %18s %18s", DbgFltString(TotQmIn,3,3), DbgFltString(TotQmOut,3,3), DbgFltString(NettQm,3,3));
      dbgpln("NettHt       %18s %18s %18s", DbgFltString(TotHtIn,3,3), DbgFltString(TotHtOut,3,3), DbgFltString(NettHt,3,3));
      #endif

      flag QtOK     = FlwSlvNd.m_MCloseTol.ConvergedVV(TotQmOut, TotQmIn);
      double QtErr  = FlwSlvNd.m_MCloseTol.GetActualError();
      flag HtOK     = NJ.Nd.HeatSkipMethod() || FlwSlvNd.m_HCloseTol.ConvergedVV(TotHtOut, TotHtIn);
      flag HtOK2    = NJ.Nd.HeatSkipMethod() || FlwSlvNd.m_HCloseTol2.ConvergedVV(TotHtOut, TotHtIn);
      double HtErr  = FlwSlvNd.m_HCloseTol.GetActualError();

      Strng SpMsg(" ");
      if (nModels>1)
        SpMsg.Set("- %i Specie Models", nModels);

      if (gs_ClosureOption & CloseTest_Cnds)
        {
        if (HtOK && QtOK)
          NJ.Nd.ClrCI(9, true);
        else
          {
          if (gs_ClosureOption & CloseTest_Idle)
            NJ.Nd.ExecObj()->XIdle();
          if (!HtOK && !QtOK)
            NJ.Nd.SetCI(9, "W\t%s :Join[%i]: Bad Mass & Energy Balance [%7.4f%%] {%7.4f/%7.4f} %s", NJ.Nd.ClassId(), iJoin,Max(fabs(QtErr), fabs(HtErr))*100.0,NettQm,NettHt,SpMsg());
          else if (!HtOK && QtOK)
            NJ.Nd.SetCI(9, "%s\t%s:Join[%i]: Bad Energy Balance [%7.4f%%] {%7.4f} %s", (HtOK2&&nModels>1) ? "N":"W", NJ.Nd.ClassId(), iJoin,fabs(HtErr)*100.0, NettHt, SpMsg());
          else
            NJ.Nd.SetCI(9, "W\t%s:Join[%i]: Bad Mass Balance [%7.4f%%] {%7.4f}", NJ.Nd.ClassId(), iJoin,fabs(QtErr)*100.0, NettQm);
          }
        }
      if (gs_ClosureOption & CloseTest_Msgs)
        {
        if (HtOK && QtOK)
          { }
        else
          {
          if (gs_ClosureOption & CloseTest_Idle)
            NJ.Nd.ExecObj()->XIdle();
          if (!HtOK && !QtOK)
            LogWarning(NJ.Nd.FullObjTag(), 0, "%s:Join[%i]: Bad Mass & Energy Balance [%7.4f%%] {%7.4f/%7.4f} %s", NJ.Nd.ClassId(), iJoin,Max(fabs(QtErr), fabs(HtErr))*100.0,NettQm,NettHt,SpMsg());
          else if (!HtOK && QtOK)
            {
            if (HtOK2&&nModels>1)
              LogNote(NJ.Nd.FullObjTag(), 0, "%s:Join[%i]: Bad Energy Balance [%7.4f%%] {%7.4f} %s", NJ.Nd.ClassId(), iJoin,fabs(HtErr)*100.0, NettHt, SpMsg());
            else
              LogWarning(NJ.Nd.FullObjTag(), 0, "%s:Join[%i]: Bad Energy Balance [%7.4f%%] {%7.4f} %s", NJ.Nd.ClassId(), iJoin,fabs(HtErr)*100.0, NettHt, SpMsg());
            }
          else
            LogWarning(NJ.Nd.FullObjTag(), 0, "%s:Join[%i]: Bad Mass Balance [%7.4f%%] {%7.4f}", NJ.Nd.ClassId(), iJoin,fabs(QtErr)*100.0, NettQm);
          }
        }

      if (dbgfile())
        {
        flag DoRpt = 0 && (!QtOK || !HtOK);

        if (DoRpt)
          {
          if (WrtHd)
            {
            dbgpln("========================== Test Models ==========================");
            dbgpln("%-7.7s ----- In  ---------------------- Out  --------------- Error", "");
            }
          WrtHd=0;
          Strng Why="   ";
          dbgp("CloseTst:%-3s", Why());
          dbgpln(" | Qm %12.4g %12.4g CnQm %12.4g %12.4g | %7.2f%% | %-10s %s",QmIn, QmOut, CnQmIn, CnQmOut, 100.0*Range(-1.0, QtErr, 1.0), NJ.Nd.ClassId(), NJ.Nd.FullObjTag());
          dbgp("            ");
          dbgpln(" | Ht %12.4g %12.4g CnHt %12.4g %12.4g | %7.2f%% | ",HtIn, HtOut, CnHtIn, CnHtOut, 100.0*Range(-1.0, HtErr, 1.0));
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

IMPLEMENT_TAGOBJ(FlwSlvPB, "FS_PB", "FS_PB", "", "", "$PB", TOC_SYSTEM, "The FlwSlvPB", "The FlwSlvPB");

FlwSlvPB::FlwSlvPB(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  FlwSlvBlk((CFlwSolver*)pAttach),
  CTNode(pClass_, TagIn, pAttach, eAttach),
  IntCB(0.0, 1.0)
  {
  //Should never get used
  ASSERT_ALWAYS(FALSE, "Should never get used", __FILE__, __LINE__);
#if DOPBSMOOTHING
  pSSMS=NULL;
#endif
  };

//--------------------------------------------------------------------------

FlwSlvPB::FlwSlvPB(CFlwSolver * pFlwSlvNd) :
  FlwSlvBlk(pFlwSlvNd),
  CTNode(&FlwSlvPBClass, "$PB", NULL, TOA_Free),
  IntCB(0.0, 1.0)
  {
  g_pPBIntCB=&IntCB;
  bDoSystemDefn=false;
#if DOPBSMOOTHING
  pSSMS=NULL;
#endif
  CRqdPressCtrl::m_dLowQmFracNC=PrjFileVerNo()<57 ? 0.0 : 0.0001;
  }

//--------------------------------------------------------------------------

FlwSlvPB::~FlwSlvPB()
  {
  ClearSolutionBlks(true);
  g_pPBIntCB=&IntCB;
#if DOPBSMOOTHING
  delete pSSMS;
#endif
  }

//--------------------------------------------------------------------------

void FlwSlvPB::BuildDataDefn(DataDefnBlk & DDB)
  {
  if (DDB.BeginStruct(&FlwSlvNd, "PB", NULL, DDB_NoPage))
    {
    static DDBValueLst DDB0[] = {
      {TCM_DirectSubs,  "DirectSubs"},
      {TCM_AdaptSubs,   "AdaptSubs"},
      {TCM_Wegstein,    "Wegstein"},
      //{TCM_Kalman,      "Kalman"},
      {0}};
    DDB.Text(" ");
    DDB.Text("Tolerance Defaults");
    DDB.Double("EPS_Rel",           "", DC_Frac, "%",   &PBEqnCB().Cfg.dEps_Rel,       &FlwSlvNd, isParm );
    DDB.Double("EPS_Abs",           "", DC_Frac, "%",   &PBEqnCB().Cfg.dEps_Abs,       &FlwSlvNd, isParm );
    DDB.Text("Convergence Defaults");
    DDB.Long  ("MaxIters",          "", DC_    , "",    &PBEqnCB().Cfg.iMaxIters,      &FlwSlvNd, isParm );
    DDB.Short ("CnvgdIters",        "", DC_    , "",    &PBEqnCB().Cfg.iRqdCnvrgdIters,&FlwSlvNd, isParm );
    DDB.Byte  ("TearMethod",  "Method", DC_    , "",    &PBEqnCB().Cfg.iConvergeMeth,  &FlwSlvNd, 0/*isParm */);
    //DDB.Byte  ("TearMethod",  "Method", DC_    , "",    &PBEqnCB().Cfg.iConvergeMeth,  &FlwSlvNd, 0/*isParm */, DDB0);
    DDB.Double("Damping",           "", DC_Frac, "%",   &PBEqnCB().Cfg.dDamping,       &FlwSlvNd, isParm );
    DDB.Double("DampingGrowth",     "", DC_,     "",    &PBEqnCB().Cfg.dDampFctGrowth, &FlwSlvNd, isParm );
    DDB.Double("DampingDecay",      "", DC_,     "",    &PBEqnCB().Cfg.dDampFctDecay,  &FlwSlvNd, isParm );
    DDB.Short ("WegsteinDelay",     "", DC_,     "",    &PBEqnCB().Cfg.iWA_Delay,      &FlwSlvNd, isParm );
    DDB.Double("WegsteinBound",     "", DC_,     "",    &PBEqnCB().Cfg.dWA_Bound,      &FlwSlvNd, isParm );
    DDB.Double("WegsteinClamp",     "", DC_,     "",    &PBEqnCB().Cfg.dWA_Clamping,   &FlwSlvNd, isParm );

    DDB.Text(" ");
    #if ALLOWMASSSMOOTH
    DDB.CheckBoxBtn("MassSmoothing","", DC_,     "",    &PBEqnCB().Cfg.fMassSmooth,    &FlwSlvNd, isParm);
    #else
    DDB.CheckBoxBtn("MassSmoothing","", DC_,     "",    &PBEqnCB().Cfg.fMassSmooth,    &FlwSlvNd, 0 | InitHidden);
    #endif
    DDB.CheckBoxBtn("ResultsValid", "", DC_,     "",    xidResultsValid,               &FlwSlvNd, 0/*isParm */);
    DDB.Double("LowQmFracOther",    "", DC_Frac, "%",   &CRqdPressCtrl::m_dLowQmFracNC, &FlwSlvNd, isParm|SetOnChange);

    DDB.Text(" ");
    if (DDB.BeginStruct(this, "Closure"))
      {//tolerances used to test for closure of mass and/or energy balances
      if (PrjFileVerNo()<53 || !DDB.ForFileSnpScn())
        {
        FlwSlvNd.m_MCloseTol.GetAbsDD().BuildDataDefn(DDB, "MsAbs",     "", DC_Qm,   "kg/s", xidMCloseTolAbs,  &FlwSlvNd, "Def", "Ovr");
        FlwSlvNd.m_MCloseTol.GetRelDD().BuildDataDefn(DDB, "MsRel",     "", DC_Frac, "%",    xidMCloseTolRel,  &FlwSlvNd, "Def", "Ovr");
        FlwSlvNd.m_HCloseTol.GetAbsDD().BuildDataDefn(DDB, "HtAbs",     "", DC_Pwr,  "kJ/s", xidHCloseTolAbs,  &FlwSlvNd, "Def", "Ovr");
        FlwSlvNd.m_HCloseTol.GetRelDD().BuildDataDefn(DDB, "HtRel",     "", DC_Frac, "%",    xidHCloseTolRel,  &FlwSlvNd, "Def", "Ovr");
        FlwSlvNd.m_HCloseTol2.GetRelDD().BuildDataDefn(DDB, "HtRel2Mdl",  "", DC_Frac, "%",    xidHCloseTolRel2, &FlwSlvNd, "Def", "Ovr");
        }

//      DDB.Double("MsAbs",     "", DC_Qm,   "kg/s", &m_dMCloseTolAbs,  &FlwSlvNd, isParm);
//      DDB.Double("MsRel",     "", DC_Frac, "%",    &m_dMCloseTolRel,  &FlwSlvNd, isParm);
//      DDB.Double("HtAbs",     "", DC_Pwr,  "kJ/s", &m_dHCloseTolAbs,  &FlwSlvNd, isParm);
//      DDB.Double("HtRel",     "", DC_Frac, "%",    &m_dHCloseTolRel,  &FlwSlvNd, isParm);
//      DDB.Double("HtRel2Mdl", "", DC_Frac, "%",    &m_dHCloseTolRel2, &FlwSlvNd, isParm);
      }
    DDB.EndStruct();

    if (dbgtestfileopen())
      {
      DDB.Long("SlvSequence", "",  DC_ , "",    &CtrlSeq().m_iSequence, &FlwSlvNd, isResult|noFile|noSnap);
      DDB.Long("SlvCount",    "",  DC_ , "",    &CtrlSeq().m_iCount, &FlwSlvNd, isResult|noFile|noSnap);
      DDB.Long("SlvVar",      "",  DC_ , "",    &CtrlSeq().m_iVar, &FlwSlvNd, isResult|noFile|noSnap);
      }

    }

  DDB.EndStruct();
  }

//--------------------------------------------------------------------------

void FlwSlvPB::BuildDataDefnOld(DataDefnBlk & DDB)
  {

  if (DDB.BeginStruct(&FlwSlvNd, "PB", NULL, DDB_RqdPage))
    {
    DDB.Text(" ");
    DDB.Double("EPS_Rel",         "", DC_Frac, "%",   &PBEqnCB().Cfg.dEps_Rel,       &FlwSlvNd, isParm );
    DDB.Double("EPS_Abs",         "", DC_Frac, "%",   &PBEqnCB().Cfg.dEps_Abs,       &FlwSlvNd, isParm );
    DDB.Long  ("MaxIters",        "", DC_    , "",    &PBEqnCB().Cfg.iMaxIters,      &FlwSlvNd, isParm );
    DDB.Double("Damping",         "", DC_Frac, "%",   &PBEqnCB().Cfg.dDamping,       &FlwSlvNd, isParm );
    DDB.Byte  ("Method",          "", DC_    , "",    &PBEqnCB().Cfg.iConvergeMeth,  &FlwSlvNd, 0/*isParm */);
    DDB.Bool  ("MassSmoothing",   "", DC_    , "",    &PBEqnCB().Cfg.fMassSmooth,     &FlwSlvNd, isParm);
    DDB.Bool  ("ResultsValid",    "", DC_    , "",    xidResultsValid,               &FlwSlvNd, 0/*isParm */);

    DDB.Text(" ");
    DDB.Text("Ave Statistics");
    DDB.Long("Iters",    "",  DC_ , "",    xidSolveIters    , &FlwSlvNd, isResult|noFile|noSnap);
    DDB.Long("Unknowns", "",  DC_ , "",    xidSolveUnknowns , &FlwSlvNd, isResult|noFile|noSnap);
    DDB.Long("BadError", "",  DC_ , "",    xidSolveBadError , &FlwSlvNd, isResult|noFile|noSnap);
    DDB.Long("BadLimit", "",  DC_ , "",    xidSolveBadLimit , &FlwSlvNd, isResult|noFile|noSnap);
    }
  DDB.EndStruct();

  };

//--------------------------------------------------------------------------

flag FlwSlvPB::DataXchg(DataChangeBlk & DCB)
  {
  if (CTNode::DataXchg(DCB))
    return 1;

  if (FlwSlvBlk::DataXchg(DCB))
    return 1;

  switch (DCB.lHandle)
    {
    case xidMCloseTolAbs:
      return FlwSlvNd.m_MCloseTol.GetAbsDD().DataXchg(DCB, xidMCloseTolAbs, &FlwSlvNd);
    case xidMCloseTolRel:
      return FlwSlvNd.m_MCloseTol.GetRelDD().DataXchg(DCB, xidMCloseTolRel, &FlwSlvNd);
    case xidHCloseTolAbs:
      return FlwSlvNd.m_HCloseTol.GetAbsDD().DataXchg(DCB, xidHCloseTolAbs, &FlwSlvNd);
    case xidHCloseTolRel:
      return FlwSlvNd.m_HCloseTol.GetRelDD().DataXchg(DCB, xidHCloseTolRel, &FlwSlvNd);
    case xidHCloseTolRel2:
      return FlwSlvNd.m_HCloseTol2.GetRelDD().DataXchg(DCB, xidHCloseTolRel2, &FlwSlvNd);
    }
  return 0;
  }

//--------------------------------------------------------------------------

flag FlwSlvPB::ValidateData(ValidateDataBlk & VDB)
  {
  return 1;
  }

//--------------------------------------------------------------------------

flag FlwSlvPB::CreateSolutionBlks(int NRqdThreads)//flag fProBal_)
  {
  if (FlwSlvBlk::CreateSolutionBlks(NRqdThreads))
    {
    LnkNds.RemoveAllToFreeList();
    CFlwNodeIter All(m_AllNds);
    for (FlwNode * p=All.First(); (p); p=All.Next())
      {
      if (p->IsLnk())
        {
        //p->FBlk()->SetFixed(false);
        LnkNds.AddTail(p);
        }
      for (int i=0; i<p->NoFlwIOs(); i++)
        for (int j=0; j<p->NIOFBs(i); j++)
          {
          p->IOFB(i,j)->m_fFixed=false;
          #if (0||dbgTrackQmModes)
          dbgpln("%-20s fb%04i", "FBSetFixed", p->IOFB(i,j)->m_iNo);
          #endif
          }
      p->Busy=p->Done=p->Break=0;
      }

    m_SolutionBlksOk=true;

    flag OK=m_SolutionBlksOk || m_PwrCtrlNds.GetCount()>0 || m_StandAloneNds.GetCount()>0;

    // Find And Set Up All Flow CTNode Groups
    // NB Delete All Flw
    //MacroMdlIter MMs(MacroMdls);


    OK = OK && BuildMacroMdls();

    for (p=All.First(); (p); p=All.Next())
      p->Busy=p->Done=p->Break=0;

    return OK;
    }

  return false;
  }

//--------------------------------------------------------------------------

int FlwSlvPB::ClearSolutionBlks(flag ClearAll)
  {
  //DeleteMacroMdls();

  LnkNds.RemoveAllToFreeList();
  return FlwSlvBlk::ClearSolutionBlks(ClearAll);
  }

//--------------------------------------------------------------------------

inline static void ProcLoopStart(CLkNodeEvalIter & Iter, CNodeEvalIndex & NEI, int Cnt, bool Dolock=true)
  {
  #if dbgFlwSolve
  if (dbgThreads())
    dbgpln("ProcLoopStart %4i (QLen:%3i) %s", GetCurrentThreadId(), Cnt, NEI.Nd.Tag());
  if (dbgThreadsSleep())
    Sleep(0);
  #endif
  if (Dolock)
    {
    //NEI.Nd.LockFlanges();
    NEI.Nd.LockNode();
    if (NEI.m_iCluster>=0)
      {
      CJoinCluster &C=NEI.Cluster;
      C.m_nFlwIOsInDone=0;
      #if dbgFlwSolve
      if (dbgThreadsNodes())
        {
        dbglock();
        dbgp("%*s     %3i %3i Rst %s", 18+CFlwSlvThreads::dbgIndent(GetCurrentThreadId()), "", 
          C.m_nFlwIOsInDone,C.m_nFlwIOsInReqd, NEI.Nd.Tag());
        dbgp(" [%4i]", C.m_iCluster);
        dbgpln("");
        dbgunlock();
        }
      #endif  
      }
    }
  }

//--------------------------------------------------------------------------

inline static void ProcLoopEnd(CLkNodeEvalIter & Iter, CNodeEvalIndex & NEI, int Cnt, bool Dolock=true)
  {
  if (Dolock)
    {
    //NEI.Nd.UnLockFlanges();
    NEI.Nd.UnLockNode();
    }
  //** Cluster Mods
  if (NEI.m_iCluster>=0)
    {
    //ASSERT(NEI.Nd.NClusters()==0);
    CJoinCluster &C=NEI.Cluster;
    C.m_nFlwIOsInDone=0;
    CIArray & Deps=C.m_EvalAfterMeIOs;

    #if dbgFlwSolve
    if (0 && dbgThreadsNodes())
      dbgpln("%*s  PLE [%i] %s", 18+CFlwSlvThreads::dbgIndent(GetCurrentThreadId()), "", Deps.GetSize(), NEI.Nd.Tag());
    #endif

    for (int d=0; d<Deps.GetSize(); d++)
      {
      //C.
      FlwNode * pRmt=NEI.Nd.Nd_Rmt(Deps[d]);

      CJoinCluster *pCRmt=NEI.Nd.IO_Cluster_Rmt(Deps[d]);
      if (pCRmt)
        {
        if (InterlockedIncrement(&pCRmt->m_nFlwIOsInDone)==pCRmt->m_nFlwIOsInReqd)//.FlwIOsInReqd())
          {
          //InterlockedExchange(&pCRmt->m_nFlwIOsInDone, 0);
          //      Iter.AddTail(pRmt);
          Iter.AddTail(CNodeEvalIndex(pRmt, pCRmt->m_iCluster, pCRmt->m_iJoinMask, ST_Depends,true));
         
          #if dbgFlwSolve
          if (dbgThreadsNodes())
            {
            dbglock();
            dbgp("%*s %2i) %3i %3i Go  %s", 18+CFlwSlvThreads::dbgIndent(GetCurrentThreadId()), "", 
              Deps[d],pCRmt->m_nFlwIOsInDone,pCRmt->m_nFlwIOsInReqd,  pRmt->Tag());
            //dbgp(" 0x%08x", pCRmt);
            dbgp(" [%4i]", pCRmt->m_iCluster);
            dbgpln("");
            dbgunlock();
            }
          #endif  
          }
        else
          {
          #if dbgFlwSolve
          if (dbgThreadsNodes())
            {
            dbglock();
            dbgp("%*s %2i) %3i %3i +   %s", 18+CFlwSlvThreads::dbgIndent(GetCurrentThreadId()), "",
              Deps[d], pCRmt->m_nFlwIOsInDone,pCRmt->m_nFlwIOsInReqd, pRmt->Tag());
            //dbgp(" 0x%08x", pCRmt);
            dbgp(" [%4i]", pCRmt->m_iCluster);
            dbgpln("");
            dbgunlock();
            }
          #endif
          }
        }
      else
        {
        dbgpln("Bad Rmt Cluster %s", pRmt->Tag());
        }
      }
    }

  #if dbgFlwSolve
  if (dbgThreadsSleep())
    Sleep(0);
  #endif
  }

//--------------------------------------------------------------------------

static void CtrlLoopStart(CLkFlwNodeIter & Iter, FlwNode * p, int Cnt)
  {
  #if dbgFlwSolve
  if (dbgThreads())
    dbgpln("CtrlLoopStart %-20s %4i (QLen:%3i)", p->FullObjTag(), GetCurrentThreadId(), Cnt);
  if (dbgThreadsSleep())
    Sleep(0);
  #endif
  p->LockCtrls();
  }

//--------------------------------------------------------------------------

static void CtrlLoopEnd(CLkFlwNodeIter & Iter, FlwNode * p, int Cnt)
  {
  p->UnLockCtrls();
  p->m_nCtrlsInDone=0;
  CIArray & Deps=p->m_CtrlDependentIOs;
  for (int d=0; d<Deps.GetSize(); d++)
    {
    FlwNode * pRmt;
    long iRmt=Deps[d];
    if (iRmt<0)
      {
      iRmt=-(iRmt+1);
      pRmt=static_cast<FlwNode*>(p->m_XRefs2Me[iRmt]->DstNd());
      }
    else
      pRmt=p->Nd_Rmt(iRmt);

      #if dbgFlwSolve
      if (dbgThreads())
        {
        dbgp("CtrlLoopEnd   %-20s %4i %3i %3i %3i %08x", p->FullObjTag(), GetCurrentThreadId(), d, Deps[d], iRmt, pRmt);
        dbgp(" %3i %3i %s", pRmt->m_nCtrlsInDone+1, pRmt->CtrlsInReqd(), pRmt->FullObjTag());
        }
      #endif
    if (InterlockedIncrement(&(pRmt->m_nCtrlsInDone))==pRmt->CtrlsInReqd())
      {
      Iter.AddTail(pRmt);
      #if dbgFlwSolve
      if (dbgThreads())
        dbgp(" ++ ");
      #endif
      }
    #if dbgFlwSolve
    if (dbgThreads())
      dbgpln("");
    #endif
    }

  #if dbgFlwSolve
  if (dbgThreadsSleep())
    Sleep(0);
  #endif
  }

//--------------------------------------------------------------------------

void FlwSlvPB::FITPBEvalMakeUpAvail(CLkNodeEvalIter & Iter)
  {
  CNodeEvalIndex NJ;
  int Cnt;
  CThreadTimer  XT(eFITPBEvalMakeUpAvail, FSW_PBEvalMakeUpAvail);
  while (Iter.RemoveNext(NJ, &Cnt))
    {
    FlwNode * p=&NJ.Nd;
    XT.StartLoop(p);
    ProcLoopStart(Iter, NJ, Cnt);
    XT.LockDone();//(p, eFITPBEvalMakeUpAvail, FSW_PBEvalMakeUpAvail);
    p->EvalPBMakeUpAvail(NJ.JoinMask);
    XT.ExecDone();//Postamble(p, eFITPBEvalMakeUpAvail, FSW_PBEvalMakeUpAvail);
    ProcLoopEnd(Iter, NJ, Cnt);
    XT.EndLoop();//PostambleUnLock();
    }
  XT.Complete();//PostambleTl();
  };
void FlwSlvPB::FITPBEvalMakeUpReqd(CLkNodeEvalIter & Iter)
  {
  CNodeEvalIndex NJ;
  int Cnt;
  CThreadTimer  XT(eFITPBEvalMakeUpReqd, FSW_PBEvalMakeUpReqd);
  while (Iter.RemoveNext(NJ, &Cnt))
    {
    FlwNode * p=&NJ.Nd;
    XT.StartLoop(p);
    XT.LockDone();//(p, eFITPBEvalMakeUpReqd, FSW_PBEvalMakeUpReqd);
    ProcLoopStart(Iter, NJ, Cnt);
    p->EvalPBMakeUpReqd(NJ.JoinMask);
    XT.ExecDone();//Postamble(p, eFITPBEvalMakeUpReqd, FSW_PBEvalMakeUpReqd);
    ProcLoopEnd(Iter, NJ, Cnt);
    XT.EndLoop();//PostambleUnLock();
    }
  XT.Complete();//PostambleTl();
  };
void FlwSlvPB::FITPBEvalPressures(CLkNodeEvalIter & Iter)
  {
  CNodeEvalIndex NJ;
  int Cnt;
  CThreadTimer  XT(eFITPBEvalPressures, FSW_EvalJoinPressures);
  while (Iter.RemoveNext(NJ, &Cnt))
    {
    FlwNode * p=&NJ.Nd;
    XT.StartLoop(p);
    XT.LockDone();//(p, eFITPBEvalPressures, FSW_EvalJoinPressures);
    ProcLoopStart(Iter, NJ, Cnt);
    p->EvalJoinPressures(NJ.JoinMask);
    XT.ExecDone();//Postamble(p, eFITPBEvalPressures, FSW_EvalJoinPressures);
    ProcLoopEnd(Iter, NJ, Cnt);
    XT.EndLoop();//PostambleUnLock();
    }
  XT.Complete();//PostambleTl();
  };
void FlwSlvPB::FITPBEvalProducts(CLkNodeEvalIter & Iter)
  {
  CNodeEvalIndex NJ;
  int Cnt;
  CThreadTimer  XT(eFITPBEvalProducts, FSW_EvalProducts);
  while (Iter.RemoveNext(NJ, &Cnt))
    {
    FlwNode * p = &NJ.Nd;
    XT.StartLoop(p);
    XT.LockDone();//(p, eFITPBEvalProducts, FSW_EvalProducts);
    ProcLoopStart(Iter, NJ, Cnt);
    if (p->fEvalProductsCalled)
      { int xxx=0; };
    p->BeforeNode(NPM_BeforeEvalProducts);
    CALC_EVAL_ORDER(NJ);
    p->EvalProducts(NJ);
    p->AfterNode(NPM_AfterEvalProducts);
    p->fEvalProductsCalled=true;
    XT.ExecDone();//Postamble(p, eFITPBEvalProducts, FSW_EvalProducts);
    ProcLoopEnd(Iter, NJ, Cnt);
    XT.EndLoop();//PostambleUnLock();
    }
  XT.Complete();//PostambleTl();
  };

void FlwSlvPB::FITPBEvalDiscrete(CFlwNodeList & List)//CLkFlwNodeIter & Iter)
  {
  FlwNode * p;
  CThreadTimer  XT(eFITPBEvalDiscrete, FSW_EvalDiscrete);
  //while (Iter.GetNext(p))
  CFlwNodeIter Iter(List);
  for (p=Iter.First(); p; p=Iter.Next())
    {
    XT.StartLoop(p);
    //p->LockFlanges();
    p->LockNode();
    XT.LockDone();//(p, eFITPBEvalDiscrete, FSW_EvalDiscrete);
    p->EvalFlowInfoEnd();
    p->EvalDiscrete();
    for (int i=0; i<p->NoFlwIOs(); i++)
      p->IOFlange(i)->SetUseStatus(p->IOConduit(i)->QMass()>SmallPosFlow);
    XT.ExecDone();//Postamble(p, eFITPBEvalDiscrete, FSW_EvalDiscrete);
    //p->UnLockFlanges();
    p->UnLockNode();
    XT.EndLoop();//PostambleUnLock();
    #if dbgFlwSolve
    if (dbgThreadsSleep())
      Sleep(0);
    #endif
    }
  XT.Complete();//PostambleTl();
  };

void FlwSlvPB::FITPBEvalCtrlActions(CFlwNodeList & List)//CLkFlwNodeIter & Iter)
  {
  FlwNode * p;
  CThreadTimer  XT(eFITPBEvalCtrlActions, FSW_EvalCtrlActions);
  //while (Iter.GetNext(p))
  CFlwNodeIter Iter(List);
  for (p=Iter.First(); p; p=Iter.Next())
    {
    XT.StartLoop(p);
    //p->LockFlanges();
    p->LockNode();
    XT.LockDone();//(p, eFITPBEvalCtrlActions, FSW_EvalCtrlActions);
    p->BeforeNode();
    p->EvalCtrlActions(CO_All);
    p->AfterNode();
    XT.ExecDone();//Postamble(p, eFITPBEvalCtrlActions, FSW_EvalCtrlActions);
    //p->UnLockFlanges();
    p->UnLockNode();
    XT.EndLoop();//PostambleUnLock();
    #if dbgFlwSolve
    if (dbgThreadsSleep())
      Sleep(0);
    #endif
    }
  XT.Complete();//PostambleTl();
  };

void FlwSlvPB::FITPBEvalCtrlStrategy(CFlwNodeList & List)//CLkFlwNodeIter & Iter)
  {
  FlwNode * p;
  CThreadTimer  XT(eFITPBEvalCtrlStrategy, FSW_EvalCtrlStrategy);
  //while (Iter.GetNext(p))
  CFlwNodeIter Iter(List);
  for (p=Iter.First(); p; p=Iter.Next())
    {
    XT.StartLoop(p);
    //p->LockFlanges();
    p->LockNode();
    XT.LockDone();//(p, eFITPBEvalCtrlStrategy, FSW_EvalCtrlStrategy);
    p->BeforeNode();
    p->EvalCtrlStrategy(CO_All);
    p->AfterNode();
    XT.ExecDone();//Postamble(p, eFITPBEvalCtrlStrategy, FSW_EvalCtrlStrategy);
    //p->UnLockFlanges();
    p->UnLockNode();
    XT.EndLoop();//PostambleUnLock();
    #if dbgFlwSolve
    if (dbgThreadsSleep())
      Sleep(0);
    #endif
    }
  XT.Complete();//PostambleTl();
  };

void FlwSlvPB::EvalAllDiscrete()
  {
  CThreadTimer  XT(eFITPBEvalDiscrete, FSW_EvalDiscrete);
  CNodeEvalIndexIter CombOrder(CombTOSequence);
  for (CNodeEvalIndex NJ=CombOrder.First(); CombOrder.ItemValid(); NJ=CombOrder.Next())
    {
    #if dbgFlwSolve
    if (dbgThreads())
      dbgpln("PB Eval Discrete %s", NJ.Nd.Tag());
    #endif
    XT.StartLoop(NJ.m_pNd);
    XT.LockDone();//(p, eFITPBEvalDiscrete, FSW_EvalDiscrete);
    NJ.Nd.EvalDiscrete();
    for (int i=0; i<NJ.Nd.NoFlwIOs(); i++)
      NJ.Nd.IOFlange(i)->SetUseStatus(NJ.Nd.IOConduit(i)->QMass()>SmallPosFlow);
    XT.ExecDone();//Postamble(p, eFITPBEvalDiscrete, FSW_EvalDiscrete);
    XT.EndLoop();
    }
  XT.Complete();//PostambleTl();
  }

void FlwSlvPB::EvalAllBalance()
  {
  if (1) // must be called for Makeups and Bleeds
    {
    for (int Pass=0; Pass<2; Pass++)
      {
      CThreadTimer  XT(eFITPBEvalAudit, FSW_EvalAudit);
      CNodeEvalIndexIter CombOrder(CombTOSequence);
      for (CNodeEvalIndex NJ=CombOrder.First(); CombOrder.ItemValid(); NJ=CombOrder.Next())
        {
        #if dbgFlwSolve
        if (dbgThreads())
          dbgpln("PB Eval Balance %s", NJ.Nd.Tag());
        #endif
        XT.StartLoop(NJ.m_pNd);
        XT.LockDone();//(p, eFITPBEvalDiscrete, FSW_EvalDiscrete);
        NJ.Nd.EvalAudit(Pass);
        XT.ExecDone();//Postamble(p, eFITPBEvalDiscrete, FSW_EvalDiscrete);
        XT.EndLoop();
        }
      XT.Complete();//PostambleTl();
      }
    }
  }

void FlwSlvPB::EvalAllCtrlInitialise(eScdCtrlTasks Tasks)
  {
  CThreadTimer  XT(eFITPBEvalCtrlInitialise, FSW_EvalCtrlInitialise);
  CFlwNodeIter All(m_AllNds);//m_AllTOSequence);
  for (FlwNode * p=All.First(); (p); p=All.Next())
    {
    XT.StartLoop(p);
    XT.LockDone();//(p, eFITPBEvalCtrlInitialise, FSW_EvalCtrlInitialise);
    p->BeforeNode();
    p->EvalCtrlInitialise(Tasks);
    p->AfterNode();
    XT.ExecDone();//Postamble(p, eFITPBEvalCtrlInitialise, FSW_EvalCtrlInitialise);
    XT.EndLoop();//PostambleUnLock();
    }
  XT.Complete();//PostambleTl();
  }

void FlwSlvPB::EvalAllCtrlStrategy(eScdCtrlTasks Tasks)
  {
  CThreadTimer  XT(eFITPBEvalCtrlStrategy, FSW_EvalCtrlStrategy);
  CNodeEvalIndexIter CtrlOrder(CtrlTOSequence);
  for (CNodeEvalIndex NJ=CtrlOrder.First(); CtrlOrder.ItemValid(); NJ=CtrlOrder.Next())
    {
    XT.StartLoop(NJ.m_pNd);
    XT.LockDone();//(p, eFITPBEvalCtrlStrategy, FSW_EvalCtrlStrategy);
    NJ.Nd.BeforeNode();
    NJ.Nd.EvalCtrlStrategy(Tasks);
    NJ.Nd.AfterNode();
    XT.ExecDone();//Postamble(p, eFITPBEvalCtrlStrategy, FSW_EvalCtrlStrategy);
    XT.EndLoop();
    MYASSERT(_CrtCheckMemory());
    }
  XT.Complete();//PostambleTl();
  }

void FlwSlvPB::EvalAllCtrlActions(eScdCtrlTasks Tasks)
  {
  CThreadTimer  XT(eFITPBEvalCtrlActions, FSW_EvalCtrlActions);
  CNodeEvalIndexIter CombOrder(CombTOSequence);
  for (CNodeEvalIndex NJ=CombOrder.First(); CombOrder.ItemValid(); NJ=CombOrder.Next())
    {
    XT.StartLoop(NJ.m_pNd);
    XT.LockDone();//(p, eFITPBEvalCtrlActions, FSW_EvalCtrlActions);
    NJ.Nd.BeforeNode();
    NJ.Nd.EvalCtrlActions(Tasks);
    NJ.Nd.AfterNode();
    XT.ExecDone();//Postamble(p, eFITPBEvalCtrlActions, FSW_EvalCtrlActions);
    XT.EndLoop();
    }
  XT.Complete();//PostambleTl();
  }

void FlwSlvPB::EvalAllCtrlTerminate(eScdCtrlTasks Tasks)
  {
  CThreadTimer  XT(eFITPBEvalCtrlTerminate, FSW_EvalCtrlTerminate);
  CFlwNodeIter All(m_AllNds);//m_AllTOSequence);
  for (FlwNode * p=All.First(); (p); p=All.Next())
    {
    XT.StartLoop(p);
    XT.LockDone();//(p, eFITPBEvalCtrlTerminate, FSW_EvalCtrlTerminate);
    p->BeforeNode();
    p->EvalCtrlTerminate(Tasks);
    p->AfterNode();
    XT.ExecDone();//Postamble(p, eFITPBEvalCtrlTerminate, FSW_EvalCtrlTerminate);
    XT.EndLoop();
    }
  XT.Complete();//PostambleTl();
  }

//--------------------------------------------------------------------------

//void FlwSlvPB::EvalAllPowerRequired()
//  {
//  gs_PwrSupplies.ZeroAll();
//
//  CFlwNodeIter Proc(ProcTOSequence);
//  for (FlwNode * p=Proc.First(); (p); p=Proc.Next())
//    {
//    XT.StartLoop(p);
//    XT.LockDone();//(p, eFITPowerRequired, FSW_Null);
//    p->EvalCtrlStrategy(eScdCtrlTasks Tasks);
//    p->CheckDataRanges();
//    XT.ExecDone();//Postamble(p, eFITPowerRequired, FSW_Null);
//    XT.EndLoop();
//    }
//
//  CFlwNodeIter Ctrl(m_CtrlTOSequence);
//  for (p=Ctrl.Last(); (p); p=Ctrl.Prev())
//    {
//    XT.StartLoop(p);
//    XT.LockDone();//(p, eFITElectrics, FSW_Null);
//    p->EvalCtrlActions(eScdCtrlTasks Tasks);
//    p->CheckDataRanges();
//    XT.ExecDone();//Postamble(p, eFITElectrics, FSW_Null);
//    XT.EndLoop();
//    }
//  }
//
//--------------------------------------------------------------------------

void FlwSlvPB::DoSolnInit(bool DoReInit)
  {
  CFlwNodeIter N(m_AllNds);
  for (FlwNode *p=N.First(); (p); p=N.Next())
    {
    p->NodeFlwTask(DoReInit ? NFT_PBReInit : NFT_PBInit);
    p->ResetMakeUpCnt();
    }

  if (0)
    {
    for (p=N.First(); (p); p=N.Next())
      {
      int Fst=1;
      for (int i=0; i<p->NoFlwIOs(); i++)
        {
        for (int fe=0; fe<p->NIOFBs(i); fe++)
          {
          dbgpln("%2i,%2i %20s %20s %20s %s", i, fe, DbgFltString(p->IOFB(i, fe)->GetQmEst()), DbgFltString(p->IOQmEst_In(i)), DbgFltString(p->IOQm_In(i)), Fst?p->Tag():"");
          Fst=0;
          }
        }
      }
    }

  for (;;)
    {
    bool ExtractsDone=true;
    CFlwNodeIterWithPos NP(m_AllNds);
    for (p=NP.First(); (p); p=NP.Next())
      {

      if (p->NodeFlwTask(NFT_PBQueryRemove)>0)
        {
        p->SetActiveHoldAuto(true);
        p->NodeFlwTask(NFT_PBRemove);
        m_AllNds.RemoveAt(NP.Pos());
        m_ExcNds.AddTail(p);
        ExtractsDone=false;
        #if dbgFlwSolve
        if (dbgEvalOrdFind())
          dbgpln("Remove %s", p->FullObjTag());
        #endif
        }
      //if (p->GetActiveHold())
      //  p->SetCI(25, p->GetActiveHoldIsAuto() ? "Hold Active (Auto)":"Hold Active");
      //else
      //  p->ClrCI(25);
      }
    if (ExtractsDone)
      break;
    }

  ConfigureJoinsAll();

  for (p=N.First(); p; p=N.Next())
    p->BuildJoinList();
  for (p=N.First(); p; p=N.Next())
    p->ConnectJoinList();

  fLogClearWasOn=gs_MsgLog.AutoClear(true)!=0;
  gs_MsgLog.ClearCondList();

  FindTOSequence(FSTC.NThreads() ? FTS_ThrdSeq : 0, TOSL_All);
  }

//--------------------------------------------------------------------------

int FlwSlvPB::InitSolution(const EqnSlvCtrlBlk & EQCtrl, CFlwNodeList &AllNdsIn)
  {
  m_AllNds.RemoveAllToFreeList();
#if TimingSteps
  s_swTiming.ReStart();
  dbgpln("InitSolution  %12.3f %12.3f ---------------", s_swTiming.Lap()*1e3,s_swTiming.Secs()*1e3);
#endif

//  SetSolutionType(DynSoln);

  gs_ThreadTiming.Initialise(FlwSlvNd.NRqdThreads());
  FSTC.CreateThreads(FlwSlvNd.NRqdThreads());

  CFlwNodeIter AllIn(AllNdsIn);
  for (FlwNode * p=AllIn.First(); (p); p=AllIn.Next())
    {
    m_AllNds.AddTail(p);
#if TimingStats
    if (1 || dbgNdStats())
      NDADDSW(p)
#endif
    }
  FixAllNdsLists();

  if (!PreStartCheckAll())
    {
    return 1;
    }
  if (!CreateSolutionBlks(FlwSlvNd.NRqdThreads()))
    {
    return 2;
    }

  CFlange::ResetMakeUpCntGlbl();
  m_ExcNds.RemoveAllToFreeList();

  DoSolnInit(true);//false);

  if (1)
    {
    EvalAllCtrlInitialise(CO_All);

    if (BuildCtrlNdListReqd())
      {
      CStopWatchLap SWLapCtrlOverHead(gs_swCtrlOverHead);
      BuildCtrlNdList();
      FindTOSequence((FSTC.NThreads() ? FTS_ThrdSeq : 0), TOSL_Ctrl);
      }
    }


#if TimingSteps
  dbgpln("Started       %12.3f %12.3f ---------------", s_swTiming.Lap()*1e3,s_swTiming.Secs()*1e3);
#endif
  return 0;// 0 - No Errors
  };

//--------------------------------------------------------------------------

int FlwSlvPB::StepSolution(const EqnSlvCtrlBlk & EQCtrl)
  {
#if dbgProBal
  if (dbgIters())
    {
    dbgpln("=============================================================================");
    dbgpln("============================== Iteration: %i ================================",EqnCB().nIters+1);
    dbgpln("=============================================================================");
    }
#endif
#if TimingSteps
  //SW.ReStart();
  dbgpln("StartStep     %12.3f %12.3f ---------------", s_swTiming.Lap()*1e3,s_swTiming.Secs()*1e3);
#endif

  STARTPROGRESS;
  
  gs_ThreadTiming.Reset();

  if (1)
    {
    long DoReInit=0;
    CFlwNodeIter N(m_AllNds);
    for (FlwNode * p=N.First(); (p && !DoReInit); p=N.Next())
      DoReInit=p->NodeFlwTask(NFT_PBQueryReInit);
    CFlwNodeIter E(m_ExcNds);
    for (p=E.First(); (p && !DoReInit); p=E.Next())
      DoReInit=p->NodeFlwTask(NFT_PBQueryReInit);
    if (DoReInit)
      {
      for (p=E.First(); (p); p=E.Next())
        {
        m_AllNds.AddTail(p);
        p->SetActiveHoldAuto(false);
        }
      m_ExcNds.RemoveAllToFreeList();

      DoSolnInit(true);
      }
    FixAllNdsLists();
    }

  CFlwNodeIter All(m_AllNds);
  MarkClosureDataInvalid(m_AllNds);

  #if (dbgFlwSolve && WithIOChgMonitor)
  if (dbgDumpChanges())
    {
    for (FlwNode * p=All.First(); (p); p=All.Next())
      p->MonitorPreEvalProducts();
    }
  #endif


#if DOPBSMOOTHING
  flag fDoMassSmoothing=DoMassSmoothing();
  if (fDoMassSmoothing)
    SolvePBSmoothingInitial(EQCtrl);
#endif

  CFlange::BumpMakeUpCntGlbl();
  for (FlwNode * p=All.First(); (p); p=All.Next())
    {
    p->NodeFlwTask(NFT_PBStep);
    p->BumpMakeUpCnt();
    }

  FlwSlvNd.ClearRefCnts();

  CNodeEvalIndexIter ProcOrder(ProcTOSequence);
  CNodeEvalIndexIter CtrlOrder(CtrlTOSequence);
  CNodeEvalIndexIter CombOrder(CombTOSequence);

  // Evaluate makeup species available at nodes in forward order
  if (FSTC.NThreads())
    SolveTOSequenceThrd(eFITPBEvalMakeUpAvail, TOSL_Proc);
  else
    {
    CThreadTimer  XT(eFITPBEvalMakeUpAvail, FSW_PBEvalMakeUpAvail);
    for (CNodeEvalIndex NJ=ProcOrder.First(); ProcOrder.ItemValid(); NJ=ProcOrder.Next())
      {
      XT.StartLoop(NJ.m_pNd);
      XT.LockDone();//(p, eFITPBEvalMakeUpAvail, FSW_PBEvalMakeUpAvail);
      NJ.Nd.EvalPBMakeUpAvail(NJ.JoinMask);
      XT.ExecDone();//Postamble(p, eFITPBEvalMakeUpAvail, FSW_PBEvalMakeUpAvail);
      XT.EndLoop();
      }
    XT.Complete();//PostambleTl();
    }

#if DoLapTime
  dbgpln("Lap Avail----------------------------------- %10.3f", SW.Lap()*1000);
#endif

  MacroMdlIter MMs(MacroMdls);
  for (CMacroMdl* pMM=MMs.First(); pMM; pMM=MMs.Next())
    pMM->PBPreJoinPressures();

  CThreadTimer  XT(eFITPBEvalMakeUpReqd, FSW_PBEvalMakeUpReqd);
  // Evaluate makeup species required at nodes in reverse order
  for (CNodeEvalIndex NJ=ProcOrder.Last(); ProcOrder.ItemValid(); NJ=ProcOrder.Prev())
    {
    XT.StartLoop(NJ.m_pNd);
    XT.LockDone();//(p, eFITPBEvalMakeUpReqd, FSW_PBEvalMakeUpReqd);
    //dbgpln("EvalPBMakeUpReqd %s", NJ.Nd.FullObjTag());
    NJ.Nd.EvalPBMakeUpReqd(NJ.JoinMask);
    XT.ExecDone();//Postamble(p, eFITPBEvalMakeUpReqd, FSW_PBEvalMakeUpReqd);
    XT.EndLoop();
    }
  XT.Complete();//PostambleTl();
#if DoLapTime
  dbgpln("Lap Reqd------------------------------------ %10.3f", SW.Lap()*1000);
#endif
  // Evaluate Pressures At Nodes in Forward Order ?
  if (FSTC.NThreads())
    SolveTOSequenceThrd(eFITPBEvalPressures, TOSL_Proc);
  else
    {
    CThreadTimer  XT(eFITPBEvalPressures, FSW_EvalJoinPressures);
    for (CNodeEvalIndex NJ=ProcOrder.First(); ProcOrder.ItemValid(); NJ=ProcOrder.Next())
      {
      XT.StartLoop(NJ.m_pNd);
      XT.LockDone();//(p, eFITPBEvalPressures, FSW_EvalJoinPressures);
      NJ.Nd.EvalJoinPressures(NJ.JoinMask);
      XT.ExecDone();//Postamble(p, eFITPBEvalPressures, FSW_EvalJoinPressures);
      XT.EndLoop();
      }
    XT.Complete();//PostambleTl();
    }
  for (pMM=MMs.First(); pMM; pMM=MMs.Next())
    pMM->PBPreEvalProducts();

#if DoLapTime
  dbgpln("Lap PreEval--------------------------------- %10.3f", SW.Lap()*1000);
#endif

  //ASSERT(_CrtCheckMemory());

  for (CNodeEvalIndex NJ=ProcOrder.First(); ProcOrder.ItemValid(); NJ=ProcOrder.Next())
    {
    NDSTARTSW(NJ.m_pNd, FSW_EvalProductsInit);
    NJ.Nd.EvalProductsInit(EPI_SourceEval);
    NDSTOPSW(NJ.m_pNd, FSW_EvalProductsInit);
    }

  // Evaluate Products At Nodes in Forward Order
  if (FSTC.NThreads())
    SolveTOSequenceThrd(eFITPBEvalProducts, TOSL_Proc);
  else
    {
    CNodeEvalIndex NJ;
    SCDTRY
      {
      CThreadTimer  XT(eFITPBEvalPressures, FSW_EvalProducts);
      for (NJ=ProcOrder.First(); ProcOrder.ItemValid(); NJ=ProcOrder.Next())
        {
#if dbgFlwSolve
        if (dbgThreads())
          dbgpln("PB Eval Products %s", NJ.Nd.Tag());
#endif
        XT.StartLoop(NJ.m_pNd);
        XT.LockDone();//(p, eFITPBEvalProducts, FSW_EvalProducts);
        NJ.Nd.BeforeNode(NPM_BeforeEvalProducts);
        CALC_EVAL_ORDER(NJ);
        NJ.Nd.EvalProducts(NJ);
        NJ.Nd.AfterNode(NPM_AfterEvalProducts);
        //ASSERT(_CrtCheckMemory());
        NJ.Nd.fEvalProductsCalled=true;
        XT.ExecDone();//Postamble(p, eFITPBEvalPressures, FSW_EvalProducts);
        XT.EndLoop();
        NJ.m_pNd=NULL;
        }
      XT.Complete();//PostambleTl();
      }
    SCDCATCH("FlwSlvDyn::FITEvalProducts", NJ.m_pNd?NJ.m_pNd->FullObjTag():NULL)
    }

#if DoLapTime
  dbgpln("Lap Products-------------------------------- %10.3f", SW.Lap()*1000);
#endif

  for (pMM=MMs.First(); pMM; pMM=MMs.Next())
    pMM->PBPostEvalProducts();

#if DoLapTime
  dbgpln("Lap PostEval-------------------------------- %10.3f", SW.Lap()*1000);
#endif

  MarkClosureDataAvail(ProcTOSequence);
#if (dbgFlwSolve)
  if (gs_ClosureOption!=CloseTest_Off || dbgTestClosure())
    TestClosure(ProcTOSequence);
#endif

#if DOPBSMOOTHING
  if (fDoMassSmoothing)
    SolvePBSmoothingEstimate(EQCtrl);
#endif

  if (0 && FSTC.NThreads())
    {
    CLkFlwNodeIter TCAll(&m_AllNds);
    FSTC.DoJob(eFITPBEvalDiscrete, this, &TCAll);
    FSTC.DoJob(eFITPBEvalCtrlStrategy, this, &TCAll);
    FSTC.DoJob(eFITPBEvalCtrlActions, this, &TCAll);
    }
  else
    {
    EvalAllDiscrete();
    EvalAllBalance();
    //EvalAllPowerRequired();
    if (BuildCtrlNdListReqd())
      {
      CStopWatchLap SWLapCtrlOverHead(gs_swCtrlOverHead);
      BuildCtrlNdList();
      FindTOSequence((FSTC.NThreads() ? FTS_ThrdSeq : 0), TOSL_Ctrl);
      }
    EvalAllCtrlStrategy(CO_All);
    EvalAllCtrlActions(CO_All);
    //EvalAllPowerAvailable();
    }

  #if (dbgFlwSolve && WithIOChgMonitor)
  if (dbgDumpChanges())
    {
    for (CNodeEvalIndex NJ=ProcOrder.First(); ProcOrder.ItemValid(); NJ=ProcOrder.Next())
      NJ.Nd.MonitorPostEvalProducts();
    }
  #endif

  for (p=All.First(); (p); p=All.Next())
    p->CheckDataRanges(true);

  // Test TearFlanges Converged
  Strng_List BadTearTags, BadTearInfo;
  EqnCB().SetConverged(EqnCB().NBadLimit()==0);
  TestTearConvergence(EQCtrl.NIters(), EqnCB(), BadTearTags, BadTearInfo);
  CtrlSeq().TestConverged(EqnCB());
  CtrlSeq().StepSequencer();

#if DoLapTime
  dbgpln("Lap TestTear-------------------------------- %10.3f", SW.Lap()*1000);
#endif

  flag Done=EqnCB().Converged();
  gs_Exec.XGlblResultValidity(Done);

#if DOPBSMOOTHING
  if (fDoMassSmoothing)
    SolvePBSmoothingEvaluate(EqnCB());
#endif

#if DoLapTime
  dbgpln("Lap Smooth---------------------------------- %10.3f", SW.Lap()*1000);
#endif

  #if (dbgFlwSolve)
  if (dbgShowFlows() && EqnCB().NIters()==0)
    ShowFlows(ProcTOSequence, "End Of Step");
  #endif

  #if (dbgFlwSolve && WithIOChgMonitor)
  if (dbgDumpChanges())
    {
    for (CNodeEvalIndex NJ=ProcOrder.First(); ProcOrder.ItemValid(); NJ=ProcOrder.Next())
      NJ.Nd.MonitorPostTestTears();
    }
  #endif


#if TimingSteps
  dbgpln("Step Done     %12.3f %12.3f ---------------", SW.Lap()*1e3,SW.Secs()*1e3);
#endif

#if TimingStats
  if (dbgNdStats() && dbgNdStatsIter())
    {
    DumpNdStats(NDS_LapTime, 0);
//    DumpNdStats(NDS_LapCount);
    }
#endif

  gs_ThreadTiming.Dump();

#if DoLapTime
  dbgpln("Lap Done------------------------------------ %10.3f", SW.Lap()*1000);
#endif

  return true;
  };

//--------------------------------------------------------------------------

int FlwSlvPB::TermSolution(const EqnSlvCtrlBlk & EQCtrl)
  {
  //TestBalance(TOSequence, 1.0e-5, /*ESR_Snks | ESR_Srcs |*/ ESR_Errors /*| ESR_Species */);
#if TimingSteps
  dbgpln("TermSolution  %12.3f %12.3f ---------------", s_swTiming.Lap()*1e3,s_swTiming.Secs()*1e3);
#endif

#if TimingStats
  if (dbgNdStats())
    {
    DumpNdStats(NDS_TotalTime, 0);
    DumpNdStats(NDS_TotalCounts, 0);

    //CFlwNodeIter All(m_AllNds);
    //for (FlwNode * p=All.First(); (p); p=All.Next())
    //  ?NDCLRSW(p);
    }
#endif

  if (1)
    {
    if (BuildCtrlNdListReqd())
      {
      CStopWatchLap SWLapCtrlOverHead(gs_swCtrlOverHead);
      BuildCtrlNdList();
      FindTOSequence((FSTC.NThreads() ? FTS_ThrdSeq : 0), TOSL_Ctrl);
      }
    EvalAllCtrlTerminate(CO_All);
    }


  CFlwNodeIter All(m_AllNds);
  for (FlwNode * p=All.First(); (p); p=All.Next())
    p->CheckDataRanges(true);

  CFlwNodeIter Exc(m_ExcNds);
  for (p=Exc.First(); (p); p=Exc.Next())
    p->SetActiveHoldAuto(false);

  TestClosure(ProcTOSequence);

  gs_MsgLog.AutoClear(fLogClearWasOn);
  ClearSolutionBlks(false);

  FSTC.CloseThreads();

  return 0;
  };

//--------------------------------------------------------------------------

int FlwSlvPB::GetSolutionError(const EqnSlvCtrlBlk & EQCtrl, Strng & RptLine)
  {
  if (EqnCB().Report()!=0)
    {
    if (ProcTOSequence.GetCount()>0)
      {
      TestBalance(EQCtrl, ProcTOSequence, &TBReport);
      if (TBReport.Length()==0)
        TBReport.Append("Nothing Fits Selection");
      }
    else
      TBReport.Append("No Units to Test");
    }
  if (TBReport.Length()>0)
    {
    pStrng p=TBReport.First();
    TBReport.Remove(p);
    RptLine = p->Str();
    delete p;
    return 1;
    }
  return -1;
  }

//--------------------------------------------------------------------------

//void FlwSlvPB::TestClosure(CFlwNodeList &TOSequence)
//  {
//  flag WrtHd=1;
//  CFlwNodeIter Order(TOSequence);
//  for (FlwNode * p=Order.First(); p; p=Order.Next())
//    {
//    int iSrcSnk;
//    SpMArray LSpI, LSpO;//, LSp;
//    double LHtI=0.0, LHtO=0.0;//, LHt;
//    double LQtI=0.0, LQtO=0.0;//, LHt;
//
//    int nModels=0;
//    CSpModelCount Models[MaxSpModelCount];
//    SpConduit* CdPtrs[MaxIOList];
//
//    CSpMdlSlct Slct;
//    for (int i=0; i<p->NoFlwIOs(); i++)
//      {
//      SpConduit* & pC = CdPtrs[i];
//      CFlange* pF=p->IOFlange(i);
//      if (pF->IsTear() && !pF->fIsBuffered)
//        {
//        if (p->IO_In(i))
//          pC=pF->pQIO;// TearOutput
//        else
//          pC=pF->pQIn;// TearInput;
//        }
//      else
//        pC=p->IOConduit(i);
//
//      for (int j=0; j<nModels; j++)
//        if (Models[j].pClass==pC->pModel->Class())
//          break;
//      j=Min(j, MaxSpModelCount-1);
//      if (j==nModels)
//        {
//        nModels++;
//        Models[j].pClass=pC->pModel->Class();
//        Models[j].pC=pC;
//        Models[j].iCount=0;
//        Models[j].Qt=0.0;
//        }
//      Models[j].iCount++;
//      Models[j].Qt+=pC->QMass();
//      Slct.Add(pC->Model(), pC->QMass());
//      }
//
//    SpModel *EnthMdl=Slct.RequiredMdl();
//
//    //double RefTemp=C_2_K(0.0);
//    iSrcSnk=p->ClosureInfo(*EnthMdl, LSpI, LHtI, LSpO, LHtO);
//    LQtI+=LSpI.Mass();
//    LQtO+=LSpO.Mass();
//
//    for (i=0; i<p->NoFlwIOs(); i++)
//      {
//      SpConduit* pC=CdPtrs[i];
////      CFlange* pF=p->IOFlange(i);
////      if (pF->IsTear() && !pF->fIsBuffered)
////        {
////        if (p->IO_In(i))
////          pC=pF->pQIO;// TearOutput
////        else
////          pC=pF->pQIn;// TearInput;
////        }
////      else
////        pC=p->IOConduit(i);
////
//      if (p->IO_In(i))
//        {
//        LSpI += pC->MArray();
//        LQtI += pC->QMass();
//        if (SpModel::iEnthalpyStrategy==SPMES_BestTemp ||
//            (int)(pC->Model()->RefTemp()-EnthMdl->RefTemp()))
//          LHtI += EnthMdl->totHf(som_ALL, pC->Temp(), pC->Press(), pC->pMArray());
//        else
//          LHtI += pC->totHf();
//        }
//      else
//        {
//        LSpO += pC->MArray();
//        LQtO += pC->QMass();
//        if (SpModel::iEnthalpyStrategy==SPMES_BestTemp ||
//            (int)(pC->Model()->RefTemp()-EnthMdl->RefTemp()))
//          LHtO += EnthMdl->totHf(som_ALL, pC->Temp(), pC->Press(), pC->pMArray());
//        else
//          LHtO += pC->totHf();
//        }
//      }
//
//    p->m_Closure.m_NettMass = (LQtI-LQtO);
//    p->m_Closure.m_NettHeat = (LHtI-LHtO);
//
//    double QtDiff=(LQtI-LQtO);
//    double HtDiff=(LHtI-LHtO);
//    double QtErr=QtDiff/GTZ(Max(fabs(LQtI),fabs(LQtO)));
//    double HtErr=HtDiff/GTZ(Max(fabs(LHtI),fabs(LHtO)));
//    double MaxErr=Max(fabs(QtErr), fabs(HtErr));
//    //SpMArray SpErr;
//    //double MaxSpErr=0.0;
//    //for (int s=0; s < SDB.No(); s++)
//    //  {
//    //  SpErr[s]=(LSpI[s]-LSpO[s])/NZ(LSpI[s]+LSpO[s]);
//    //  MaxSpErr=Max(MaxSpErr, fabs(SpErr[s]));
//    //  }
//
//    //double Err;
//    //flag QtOK = ConvergedDV(QtErr, 1.0, m_dMCloseTolAbs, m_dMCloseTolRel, Err);
//    flag QtOK = m_MCloseTol.ConvergedDV(QtErr, 1.0);
//    /*flag HtOK =ConvergedDV(HtErr, 1.0, m_dHCloseTolAbs, m_dHCloseTolRel, Err);
//    flag HtOK2=ConvergedDV(HtErr, 1.0, m_dHCloseTolAbs, m_dHCloseTolRel2, Err);*/
//    //kga : 8/3/2002 : Changed from the above two lines to that shown below.
//    //error occurs when LQtI~=0.0 and LHtO~=0.0 then HtErr~=1.0 because temperature is very close to low limit
//    //specific example oxygen only pipe at T=25dC and temperature low limit of SPDB range =25dC
//    flag HtOK,HtOK2;
//    if (fabs(LQtI)<1.0e-8 && fabs(LQtO)<1.0e-8)
//      {
//      HtOK =m_HCloseTol.ConvergedDV(HtDiff, LQtI);//HtErr);
//      HtOK2=m_HCloseTol2.ConvergedDV(HtDiff, LQtI);
//      }
//    else
//      {
//      HtOK =m_HCloseTol.ConvergedDV(HtErr, 1.0);
//      HtOK2=m_HCloseTol2.ConvergedDV(HtErr, 1.0);
//      }
//
//    Strng SpMsg(" ");
//    if (nModels>1)
//      SpMsg.Set("- %i Specie Models", nModels);
//
//    if (HtOK && QtOK)
//      p->ClrCI(9, true);
//    else if (!HtOK && !QtOK)
//      p->SetCI(9, "W\tBad Mass & Energy Balance [%7.4f%%] {%7.4f/%7.4f} %s", Max(fabs(QtErr), fabs(HtErr))*100.0,QtDiff,HtDiff,SpMsg());
//    else if (!HtOK && QtOK)
//      p->SetCI(9, "%s\tBad Energy Balance [%7.4f%%] {%7.4f} %s", (HtOK2&&nModels>1) ? "N":"W", fabs(HtErr)*100.0, HtDiff, SpMsg());
//    else
//      p->SetCI(9, "W\tBad Mass Balance [%7.4f%%] {%7.4f}", fabs(QtErr)*100.0, QtDiff);
//
//    if (dbgfile())
//      {
//      flag DoRpt=0;
//      switch (iSrcSnk)
//        {
//        case -1: DoRpt = 0; break;
//        case  1: DoRpt = 0; break;
//        case  0: DoRpt = !QtOK || !HtOK; break;
//        }
//
//      if (DoRpt)
//        {
//        if (WrtHd)
//          dbgpln("========================== Test Models ==========================");
//        WrtHd=0;
//        Strng Why;
//        switch (iSrcSnk)
//          {
//          case -1: Why="Src"; break;
//          case  1: Why="Snk"; break;
//          case  0: Why="Err"; break;
//          }
//        dbgpln("%-7.7s ----- In  ---------- Out  ---- Error", Why());
//        dbgpln("  %14.6g  %14.6g  %9.4f%%  %s.%s",LQtI, LQtO, 100.0*Range(-1.0, QtErr, 1.0), p->FullObjTag(), "Qm Total");
//        dbgpln("  %14.6g  %14.6g  %9.4f%%  %s.%s",LHtI, LHtO, 100.0*Range(-1.0, HtErr, 1.0), p->FullObjTag(), "totEnthalpy");
//        }
//      }
//    }
//  }

//--------------------------------------------------------------------------
#if DOPBSMOOTHING

void FlwSlvPB::SolvePBSmoothingInitial(const EqnSlvCtrlBlk & EQCtrl)
  {
  FlwNode *p;
  CFlwNodeIter Order(TOSequence);
  for (p=Order.First(); p; p=Order.Next())
    p->ClearMsIndices();
  }
void FlwSlvPB::SolvePBSmoothingEstimate(const EqnSlvCtrlBlk & EQCtrl)
  {
  FlwNode *p;
  CFlwNodeIter Order(TOSequence);
  //for (p=Order.First(); p; p=Order.Next())
  //  p->ClearMsIndices();
  for (p=Order.First(); p; p=Order.Next())
    {
    NDSTARTSW(p, FSW_EvalMsAGData);
    p->EvalMsAGDatas();
    NDSTOPSW(p, FSW_EvalMsAGData);
    }
  }

//--------------------------------------------------------------------------

void FlwSlvPB::SolvePBSmoothingEvaluate(const EqnSlvCtrlBlk & EQCtrl)
  {
  FlwNode *p;
  CFlwNodeIter Order(TOSequence);

  for (p=Order.Last(); p; p=Order.Prev())
    p->MergeMsAGDatas();
  for (p=Order.First(); p; p=Order.Next())
    p->MarkMsIndices();
  CMSIndexInit Index;
  for (p=Order.First(); p; p=Order.Next())
    p->SetMsIndices(Index);
  m_nMtxOrder=Index.m_iSpIndex-1;

  #if dbgMSSmooth
  if (dbgMSDumpAGData())
    for (p=Order.First(); p; p=Order.Next())
      p->DumpMsAGDatas();

  if (1 || dbgMSForceBuild())
    {
    delete pSSMS;
    pSSMS=NULL;
    }
  #endif

  bool MustAnalyse=false;
  flag fAnalyseOK=true;

  if (pSSMS==NULL)
    {
    pSSMS = new SparseSolver(m_nMtxOrder, 1.0e6);
    MustAnalyse=true;
    }

  int Eqn=1;
  for (p=Order.First(); p; p=Order.Next())
    p->AddMsEqns(*pSSMS, Eqn);

  if (MustAnalyse)
    {
    #if dbgMSSmooth
    if (dbgMSDumpMtx())
      {
      pSSMS->RowPrint((m_nMtxOrder > 1));
      dbgfileflush(1);
      dbgfileflush(0);
      }
    #endif

    // This Does not always detect singular matrices
    int Err=pSSMS->Analyse();

    #if dbgMSSmooth
    if (dbgMSDumpMtx())
      {
      Mtx_Info info;
      pSSMS->Report_Stats(&info);

      dbgpln("Matrix Order               %7i", m_nMtxOrder);
      dbgpln("Elements NonZero/ FillIns  %7i  / %7i", info.nonzerocount, info.num_fill_ins);
      dbgpln("Sparcity Before / After    %7.3f -> %7.3f%%", info.sparcity_before, info.sparcity_after);
      dbgpln("-----------------------");

      }
    #endif
    MustAnalyse = false;
    fAnalyseOK = (Err==SMTX_NONE);
    if (!fAnalyseOK)
      {
      LogError("Solver", 0, "Matrix Analysis Error %s",SparseSolver::ErrorString(Err));

      delete pSSMS;
      pSSMS=NULL;
      }
    }

  if (fAnalyseOK)
    {
//    flag Fail=false;
//    DWORD FPP_State=FPP_EnableExceptions();
//    try
//      {
    int Err=pSSMS->Solve();
    if (Err==SMTX_NONE)
      {
//      FPP_RestoreExceptions(FPP_State);

      #if dbgMSSmooth
      if (0 && dbgMSDumpResults() && pSSMS)
        for (int i=1; i<=m_nMtxOrder; i++)
          dbgpln("%4i == %16.10g",i,pSSMS->Solution(i));
      #endif

      for (p=Order.First(); p; p=Order.Next())
        p->AdjustMsFlows(*pSSMS);
      for (p=Order.First(); p; p=Order.Next())
        p->UpdateMsSoln(*pSSMS);
      FlwSlvNd.ClrCI(3);
      }
    //catch (...)
    else
      {
      //FPP_RestoreExceptions(FPP_State);
      //LogError("Solver", 0, "Matrix Solution Error");
      LogError("Solver", 0, "Matrix Solution Error %s",SparseSolver::ErrorString(Err));
      delete pSSMS;
      pSSMS=NULL;
      FlwSlvNd.SetCI(3);
      }
    }

  }

#endif
//--------------------------------------------------------------------------

void FlwSlvPB::DumpNdStats(int What, double TotTime)
  {
  TotTime=GTZ(TotTime);

#if NDSTOPWATCHES
  dbgp("StopWatches - ");
  switch (What)
    {
    case NDS_LapTime:       dbgpln(" LapTime:       mSeconds");   break;
    case NDS_LapTimeFrac:   dbgpln(" LapTimeFrac:   %");          break;
    case NDS_TotalTime:     dbgpln(" TotalTime:     mSeconds");   break;
    case NDS_TotalTimeFrac: dbgpln(" TotalTimeFrac: %");          break;
    case NDS_LapCount:      dbgpln(" Laps:          ");           break;
    case NDS_TotalCounts:   dbgpln(" TotalLaps:     ");           break;
    }

  dbgp(" %12.12s", "EvalJoinPressures");
  dbgp(" %12.12s", "EvalProducts");
  dbgp(" %12.12s", "EvalDiscrete");
  dbgp(" %12.12s", "EvalPBMakeUpAvail");
  dbgp(" %12.12s", "EvalPBMakeUpReqd");
  dbgp(" %12.12s", "EvalPBConvergence");
  dbgp(" %12.12s", "EvalMsAGDatas");
  dbgpln(" Tag");

  double MaxSW=0.0;
  CFlwNodeIter All(m_AllNds);
  for (FlwNode * p=All.First(); (p); p=All.Next())
    for (int i=0; i<FSW_Max; i++)
      MaxSW=Max(MaxSW, p->SW(i).Secs());

  for (p=All.First(); (p); p=All.Next())
    {
    #if dbgFlwSolve
    flag DoIt=(flag)dbgNdStatsAll();
    #else
    flag DoIt=0;
    #endif
    for (int i=0; i<FSW_Max; i++)
      DoIt=DoIt || p->SW(i).Secs()>0.05*MaxSW;
    if (DoIt)
      {
      int Selection[]= {FSW_EvalJoinPressures,
                        FSW_EvalProducts,
                        FSW_EvalDiscrete,
                        FSW_PBEvalMakeUpAvail,
                        FSW_PBEvalMakeUpReqd,
                        FSW_PBEvalConvergence,
                        FSW_EvalMsAGData,-1};

      for (int i=0, j; (j=Selection[i])>=0; i++)
        {
        switch (What)
          {
          case NDS_LapTime:
            dbgp(" %12.6f", p->SW(j).LapTime());
            break;
          case NDS_LapTimeFrac:
            dbgp("%12.6f%%", p->SW(j).LapTime()/TotTime);
            break;
          case NDS_TotalTime:
            dbgp(" %12.6f", p->SW(j).Secs());
            break;
          case NDS_TotalTimeFrac:
            dbgp("%12.6f%%", p->SW(j).Secs()/TotTime);
            break;
          case NDS_LapCount:
          case NDS_TotalCounts:
            dbgp(" %12i", p->SW(j).Laps());
            break;
          }
        }

      dbgpln(" %s", p->Tag());
      }
    }
#endif
  }

//===========================================================================
//
//
//
//===========================================================================

IMPLEMENT_TAGOBJ(FlwSlvDyn, "FS_Dyn", "FS_Dyn", "", "", "$Dyn", TOC_SYSTEM, "The FlwSlvDyn", "The FlwSlvDyn");

//===========================================================================
//
//
//
//===========================================================================

FlwSlvDyn::FlwSlvDyn(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  FlwSlvBlk((CFlwSolver*)pAttach),
  FlwNets(m_AllNds),
  CTNode(pClass_, TagIn, pAttach, eAttach),
  IntCB(0.0, 1.0),
  m_ODE(&IntCB, 0.01, 60.0, 1000, ODE_Euler, ODE_SSC_Fixed),
  m_SSPressTol(TBF_DynSys, "SteadyState:Pressure",     1.0e-3, 1.0e-4, 100, TBF_UseAbs|TBF_UseRel, DC_P, "kPa"),
  m_SSQmTol(TBF_DynSys, "SteadyState:MassFlow",     1.0e-3, 1.0e-4, 100, TBF_UseAbs|TBF_UseRel, DC_Qm, "kg/s")
  {
  ASSERT_ALWAYS(FALSE, "Should never get used", __FILE__, __LINE__);
  Construct();
  };

FlwSlvDyn::FlwSlvDyn(CFlwSolver * pFlwSlvNd) :
  FlwSlvBlk(pFlwSlvNd),
  FlwNets(m_AllNds),
  CTNode(&FlwSlvDynClass, "$Dyn", NULL, TOA_Free),
  IntCB(0.0, 1.0),
  m_ODE(&IntCB, 0.01, 60.0, 1000, ODE_Euler, ODE_SSC_Fixed),
  m_SSPressTol(TBF_DynSys, "SteadyState:Pressure",     1.0e-3, 1.0e-4, 100, TBF_UseAbs|TBF_UseRel, DC_P, "kPa"),
  m_SSQmTol(TBF_DynSys, "SteadyState:MassFlow",     1.0e-3, 1.0e-4, 100, TBF_UseAbs|TBF_UseRel, DC_Qm, "kg/s")
  {
  Construct();
  }

void FlwSlvDyn::Construct()
  {
  g_pODE=&m_ODE;
  g_pDynIntCB=&IntCB;
  m_ODE.SetMagConvergence(5.0e-4, 1.0e-5); // was 1.0e-5
  bDoSystemDefn=false;

  m_SolutionBlksOk=0;
  //StopTime=dNAN;

  m_SSTest=eSS_Network;
  m_SSStopCount=3;
  m_SSMaxIters=100;
  m_SSSteady=0;
  m_SSIters=0;
  m_SSMaxChange=0;

  gs_PressDropKFactMult=1.0;
  if (PrjFileVerNo()<42)
    gs_PressDropKFactMult=10.0; // correct error that was in the formula

  dwIterNo=0;
  m_EvalCtrlAt=(PrjFileVerNo()<60)?ECA_Start:ECA_End;
  }

//--------------------------------------------------------------------------

FlwSlvDyn::~FlwSlvDyn()
  {
  ClearSolutionBlks(true);
  g_pODE=NULL;
  g_pDynIntCB=NULL;
  }

//--------------------------------------------------------------------------

void FlwSlvDyn::BuildDataDefn(DataDefnBlk & DDB)
  {
  if (DDB.BeginStruct(&FlwSlvNd, "Dyn", NULL, DDB_NoPage))
    {
    DDB.Text("");
    DDB.Text("Executive");

    static DDBValueLst DDBIncCtrl[]=
      {
        {ODE_SSC_Fixed, "Fixed"},
        {ODE_SSC_Var_1, "Variable"},
        {0}
      };

    if (PrjFileVerNo()<55)
      {
      DDB.Byte  ("StepSizeCtrl",      "", DC_    , "",    xidODESSCtrl,       &FlwSlvNd, 0/*isParm*/, DDBIncCtrl);

      DDB.Double("Time",              "", DC_Time,  "s",   xidODETime,          &FlwSlvNd, 0 | DDEF_NOCOMPARE);
      DDB.Double("dTime",             "", DC_Time,  "s",   xidODETimeInc,       &FlwSlvNd, 0/*isParm*/ );
      //DDB.Double("StopTime",          "", DC_Time,  "s",   &StopTime,           &FlwSlvNd, isParm|NAN_OK );
      DDB.Double("StepSize",          "", DC_Time,  "s",   xidStepSizeRqd,      &FlwSlvNd, isParm );
      DDB.Double("dTimeMax",          "", DC_Time,  "s",   xidODETimeIncMxRqd,  &FlwSlvNd, isParm );
      DDB.Double("dTimeMin",          "", DC_Time,  "s",   xidODETimeIncMnRqd,  &FlwSlvNd, isParm );
      }
    else
      {
      //DDB.Text  ("");
      DDB.String("Time.Fmt",          "", DC_TimeStr, "DyTm.", xidTimeF,             &FlwSlvNd, 0/*isParm*/ );
      DDB.Double("Time",              "", DC_Time,    "s",     xidTimeD,             &FlwSlvNd, 0/*isParm*/|InitHidden );
      //DDB.Text  ("");
      DDB.String("StepSize.Fmt",      "", DC_dTimeStr, "Tm.",  xidStepSizeF,         &FlwSlvNd, isParm );
      //DDB.Text  ("");
      DDB.Double("StepSize",          "", DC_Time,    "s",     xidStepSizeD,         &FlwSlvNd, isParm |InitHidden);
      DDB.Long  ("StepCount",         "", DC_,        "",      xidStepCount,         &FlwSlvNd, 0);
      //DDB.String("TimeDesc",          "", DC_TimeStr, "Tm.", xidODETimeDesc,       &FlwSlvNd, InitHidden);
      //DDB.String("TimeFullDesc",      "", DC_TimeStr, "TmDt", xidODETimeFullDesc,   &FlwSlvNd, InitHidden);
      DDB.CheckBoxBtn("RealTimeOn",   "", DC_,     "",    xidRealTimeOn,        &FlwSlvNd, isParmStopped, DDBOnOff);
      DDB.Double("RealTimeMultiplier","", DC_Frac, "%",   xidRealTimeMultiplier,&FlwSlvNd, isParm);
      //DDB.Text  ("");
      }

    DDB.Text  ("");
    if (DDB.BeginStruct(&FlwSlvNd, "TimeRec", NULL, DDB_NoPage))
      {
      DDB.Long  ("mSecs",             "", DC_,      "",    xidDTmSecsCount,     &FlwSlvNd, isResult|noFileAtAll);
      DDB.Long  ("Secs",              "", DC_,      "",    xidDTSecsCount,      &FlwSlvNd, isResult|noFileAtAll);
      DDB.Long  ("Mins",              "", DC_,      "",    xidDTMinsCount,      &FlwSlvNd, isResult|noFileAtAll);
      DDB.Long  ("Hours",             "", DC_,      "",    xidDTHoursCount,     &FlwSlvNd, isResult|noFileAtAll);
      DDB.Long  ("Days",              "", DC_,      "",    xidDTDaysCount,      &FlwSlvNd, isResult|noFileAtAll);
      }
    DDB.EndStruct();
    if (DDB.BeginStruct(&FlwSlvNd, "DateRec", NULL, DDB_NoPage))
      {
      DDB.Long  ("Day",              "", DC_,      "",    xidDTDateDay,        &FlwSlvNd, isResult|noFileAtAll);
      DDB.Long  ("Month",            "", DC_,      "",    xidDTDateMonth,      &FlwSlvNd, isResult|noFileAtAll);
      DDB.Long  ("Year",             "", DC_,      "",    xidDTDateYear,       &FlwSlvNd, isResult|noFileAtAll);
      DDB.Long  ("DayOfWeek",        "", DC_,      "",    xidDTDateDayOfWeek,  &FlwSlvNd, isResult|noFileAtAll);
      DDB.TagComment("1=Sunday");
      DDB.String("DayOfWeekDesc",    "", DC_,      "",    xidDTDateDayOfWeekDesc,&FlwSlvNd, isResult|noFileAtAll);
      DDB.Long  ("DayOfYear",        "", DC_,      "",    xidDTDateDayOfYear,  &FlwSlvNd, isResult|noFileAtAll);
      DDB.String("MonthDesc",        "", DC_,      "",    xidDTDateMonthDesc,  &FlwSlvNd, isResult|noFileAtAll);
      DDB.Long  ("DaysInMonth",      "", DC_,      "",    xidDTDateDaysInMonth,&FlwSlvNd, isResult|noFileAtAll);
      DDB.Long  ("DaysInYear",       "", DC_,      "",    xidDTDateDaysInYear, &FlwSlvNd, isResult|noFileAtAll);
      }
    DDB.EndStruct();
    DDB.Text  ("");

    //DDB.Page  ("Scenario", DDB_RqdPage);
    //DDB.Text  ("");
    if (DDB.BeginStruct(&FlwSlvNd, "Scenario", NULL, DDB_RqdPage))
      {
      DDB.Text("");
      DDB.Text("Specification");

      static DDBValueLst DDBTypes[] = 
        {
          {ScnTyp_Manual,       "Manual"},
          {ScnTyp_Duration,     "Duration"},
          {ScnTyp_StopTime,     "StopTime"},
          {ScnTyp_Steps,        "Steps"},
          {ScnType_SteadyState,  "SteadyState"},
          {0},
        };
      DDB.Long("Type",              "", DC_,          "",       xidScnType,         &FlwSlvNd, isParmStopped|SetOnChange, DDBTypes);
      DDB.Visibility(NSHM_All, gs_Exec.ScnType==ScnTyp_Duration);
      DDB.String("DurationRqd.Fmt", "", DC_dTimeStr,  "DyTm0.", xidRunDurationF,    &FlwSlvNd, isParmStopped);
      DDB.Double("DurationRqd",     "", DC_Time,      "s",      xidRunDurationD,    &FlwSlvNd, isParmStopped);
      DDB.Visibility(NSHM_All, gs_Exec.ScnType==ScnTyp_StopTime);
      DDB.String("StopTimeRqd.Fmt", "", DC_TimeStr,   "DyTm0.", xidRunStopTimeF,    &FlwSlvNd, isParmStopped);
      DDB.Double("StopTimeRqd",     "", DC_Time,      "s",      xidRunStopTimeD,    &FlwSlvNd, isParmStopped|InitHidden);
      DDB.Visibility(NSHM_All, gs_Exec.ScnType==ScnTyp_Steps);
      DDB.Long  ("StepCountRqd",    "", DC_,          "",       xidStepCountMax,   &FlwSlvNd, isParmStopped);
      DDB.Visibility();

      static DDBValueLst DDBScnState[] = {{Scn_Off, "Off"}, {Scn_Start, "Start"}, {Scn_Busy, "Busy"}, {Scn_Complete, "Complete"}, {Scn_Ignore, "Ignore"}, {}};

      DDB.Byte  ("ScnState",       "",  DC_, "",  xidScnState,             &FlwSlvNd, 0,                         DDBScnState);
      DDB.String("StopTime.Fmt",    "", DC_TimeStr,   "DyTm0.", xidRunTimeToStopF,  &FlwSlvNd, InitHidden);
      DDB.Double("StopTime",        "", DC_Time,      "s",      xidRunTimeToStopD,  &FlwSlvNd, InitHidden|NAN_OK);
      DDB.String("ScnTime.Fmt",     "", DC_dTimeStr,  "DyTm0.", xidScnTimeF,        &FlwSlvNd,  InitHidden);
      DDB.Double("ScnTime",         "", DC_Time,      "s",      xidScnTimeD,        &FlwSlvNd,  InitHidden|NAN_OK);
      DDB.String("ScnTimeEnd.Fmt",  "", DC_dTimeStr,  "DyTm0.", xidScnTimeMaxF,     &FlwSlvNd,  InitHidden);
      DDB.Double("ScnTimeEnd",      "", DC_Time,      "s",      xidScnTimeMaxD,     &FlwSlvNd,  InitHidden|NAN_OK);
      DDB.Visibility(NSHM_All, false, true, true, true);
      DDB.Long  ("Type.Last",       "", DC_,          "",       xidScnTypeLast,     &FlwSlvNd, isParmStopped, DDBTypes);
      DDB.Visibility();

      static DDBValueLst DDBIfBusy[]     =  { {Scr_Continue, "Continue"}, {Scr_ReStart,  "ReStart"}, {}};
      static DDBValueLst DDBIfComplete[] =  { {Sbr_Block,    "Ignore"},   {Sbr_ReStart,  "ReStart"}, {}};
      DDB.Text("");
      DDB.Text  ("Start Action");
      DDB.Visibility(NSHM_All, gs_Exec.ScnType==ScnTyp_Manual);
      DDB.Byte("IfManualScn",    "",  DC_, "",  xidManualScnRestart,     &FlwSlvNd, isParmStopped|SetOnChange, DDBIfBusy);
      DDB.Visibility(NSHM_All, gs_Exec.ScnType!=ScnTyp_Manual);
      DDB.Byte("IfScnBusy",      "",  DC_, "",  xidReStartIfScnBusy,     &FlwSlvNd, isParmStopped|SetOnChange, DDBIfBusy);
      DDB.Byte("IfScnComplete",  "",  DC_, "",  xidReStartIfScnComplete, &FlwSlvNd, isParmStopped|SetOnChange, DDBIfComplete);
      DDB.Visibility();

      DDB.Text("");
      DDB.Text("Elapsed Time & Progress");
      //double dddd=0;
      DDB.Double("Progress",              "", DC_Frac, "%",   xidProgress,      &FlwSlvNd, isParmConstruct|NAN_OK);
      DDB.Double("Elapsed.Time",          "", DC_Time, "s",   xidClock,         &FlwSlvNd, 0|noFileAtAll);
      DDB.Double("Elapsed.EstToGo",       "", DC_Time, "s",   xidClockToGo,     &FlwSlvNd, 0|noFileAtAll);
      DDB.Double("Elapsed.EstTotal",      "", DC_Time, "s",   xidClockTotal,    &FlwSlvNd, 0|noFileAtAll);
      DDB.Long  ("Elapsed.Steps",         "", DC_,     "",    xidElapsedSteps,  &FlwSlvNd, isParmConstruct);

      DDB.Text  ("");
      //DDB.Text  ("At Scenario Start");

      if (DDB.BeginStruct(&FlwSlvNd, "Start", NULL, DDB_NoPage))
        {
        DDB.CheckBoxBtn("SetTime",              "", DC_,        "",       xidAtStartResetTime,            &FlwSlvNd, (gs_Exec.SyncWithClock() ? 0 : isParmStopped), DDBOnOff);
        DDB.String     ("StartAt.Fmt",          "", DC_TimeStr, "DyTm0.", xidAtStartTimeF,                &FlwSlvNd, (gs_Exec.SyncWithClock() ? 0 : isParmStopped));
        DDB.Double     ("StartAt",              "", DC_Time,    "s",      xidAtStartTimeD,                &FlwSlvNd, (gs_Exec.SyncWithClock() ? 0 : isParmStopped)|InitHidden);
        DDB.Text  ("");
        DDB.CheckBoxBtn("RestartHistorian",     "", DC_,        "",       xidAtStartRestartHist,          &FlwSlvNd, isParmStopped, DDBOnOff);//(gs_Exec.ResetTimeOnStart?0:isParmStopped), DDBOnOff);
        if (gs_Exec.m_ResetTimeOnStart || gs_Exec.m_RestartHstOnStart)
          DDB.TagComment("Yes");
        DDB.CheckBoxBtn("SetHistorianTagList",  "", DC_,        "",       xidAtStartSetHistTags,          &FlwSlvNd, isParmStopped, DDBOnOff);
        DDB.CheckBoxBtn("RestartArchive",       "", DC_,        "",       xidAtStartRestartArchive,       &FlwSlvNd, isParmStopped, DDBOnOff);
        DDB.Text  ("");
        DDB.CheckBoxBtn("EmptySpills",          "", DC_,        "",       xidAtStartAllEmptySpillTargets, &FlwSlvNd, isParmStopped, DDBOnOff);
        DDB.CheckBoxBtn("Empty",                "", DC_,        "",       xidAtStartAllEmpty,             &FlwSlvNd, isParmStopped, DDBOnOff);
        #if WithRestartPGM
        DDB.CheckBoxBtn("InitialisePGM",        "", DC_,        "",       xidAtStartRestartPGM,           &FlwSlvNd, isParmStopped, DDBOnOff);
        #endif
        DDB.CheckBoxBtn("RestartProfiles",      "", DC_,        "",       xidAtStartRestartPrf,           &FlwSlvNd, isParmStopped, DDBOnOff);
        DDB.CheckBoxBtn("ResetStats",           "", DC_,        "",       xidAtStartAllDynStatsRunInit,   &FlwSlvNd, isParmStopped, DDBOnOff);
        /*DDB.Text  ("");
        DDB.CheckBoxBtn("ZeroFlows",            "", DC_,        "",       xidAtStartAllZeroFlows,         &FlwSlvNd, isParmStopped, DDBOnOff);
        DDB.CheckBoxBtn("Preset",               "", DC_,        "",       xidAtStartAllPreSet,            &FlwSlvNd, isParmStopped, DDBOnOff);
        DDB.CheckBoxBtn("SteadyState",          "", DC_,        "",       xidAtStartAllSteadyState,       &FlwSlvNd, isParmStopped, DDBOnOff);*/
        //DDB.Text  ("");
        }
      DDB.EndStruct();

      DDB.Text("");
      //DDB.Text("At Scenario End");
      if (DDB.BeginStruct(&FlwSlvNd, "End", NULL, DDB_NoPage))
        {
        static DDBValueLst DDBActs[] = { /*{ScnTA_None, "None"},*/ { ScnTA_Edit, "Stop"}, { ScnTA_Idle, "Idle"}, {0}, };
        DDB.Long("Action",    "",          DC_,        "",   xidScnTermAction,   &FlwSlvNd, isParmStopped|SetOnChange, DDBActs);
        }
      DDB.EndStruct();


      }
    DDB.EndStruct();


    if (DDB.BeginStruct(&FlwSlvNd, "Cmds", NULL, DDB_RqdPage))
      {
      static DDBValueLst DDBDoIt[] = 
        {
          {0, "Execute"},
          {1, "DoIt"},
          {0},
        };
      DDB.Text("");
      DDB.Text("Scenario");
      DDB.Button("Reset Scenario",       "", DC_,    "", xidDynCmdScnRestart,  &FlwSlvNd, isParmStopped|noFileAtAll/*, DDBDoIt*/);
      DDB.Text("");
      DDB.Button("SetTime",             "", DC_,    "", xidDynCmdResetTime,   &FlwSlvNd, (gs_Exec.SyncWithClock() ? 0 : isParmStopped)|noFileAtAll/*, DDBDoIt*/);//, DDBOnOff);
      DDB.Button("RestartHistorian",    "", DC_,    "", xidDynCmdRestartHist, &FlwSlvNd, isParmStopped|noFileAtAll/*, DDBDoIt*/);//, DDBOnOff);
      //DDB.Button("SetHistorianTagList",  "", DC_,        "",       xidAtStartSetHistTags, &FlwSlvNd, isParmStopped, DDBOnOff);
      //DDB.String     ("TimeAtStart.Fmt",      "", DC_TimeStr, "DyTm0.", xidAtStartTimeF,       &FlwSlvNd, (gs_Exec.SyncWithClock() ? 0 : isParmStopped));
      //DDB.Double     ("TimeAtStart",          "", DC_Time,    "s",      xidAtStartTimeD,       &FlwSlvNd, (gs_Exec.SyncWithClock() ? 0 : isParmStopped)|InitHidden);

      DDB.Text("");
      DDB.Button("ZeroFlows",           "", DC_, "", xidSetStateAllZeroFlows,        &FlwSlvNd, isParmStopped|noFileAtAll/*, DDBDoIt*/);
      DDB.Button("Empty",               "", DC_, "", xidSetStateAllEmpty,            &FlwSlvNd, isParmStopped|noFileAtAll/*, DDBDoIt*/);
      DDB.Button("Preset",              "", DC_, "", xidSetStateAllPreSet,           &FlwSlvNd, isParmStopped|noFileAtAll/*, DDBDoIt*/);
      DDB.Button("SteadyState",         "", DC_, "", xidSetStateAllSteadyState,      &FlwSlvNd, isParmStopped|noFileAtAll/*, DDBDoIt*/);
      DDB.Button("DynStatsRunInit",     "", DC_, "", xidSetStateAllDynStatsRunInit,  &FlwSlvNd, isParmStopped|noFileAtAll/*, DDBDoIt*/);
      DDB.Button("EmptySpillTargets",   "", DC_, "", xidSetStateAllEmptySpillTargets,&FlwSlvNd, isParmStopped|noFileAtAll/*, DDBDoIt*/);
      }
    DDB.EndStruct();

    static DDBValueLst DDBMethod[] = 
      {
        {ODE_Euler,    "Euler"},
        {ODE_RK2,      "Runge-Kutta 2"},
        {ODE_RK4,      "Runge-Kutta 4"},
        {ODE_LastMethod,  "Self",         MDD_Hidden}, // Remove Old
        {ODE_LastMethod,  "Implicit",     MDD_Hidden}, // Remove Old
        {0}
      };
    static DDBValueLst DDBCtrl[]=
      {
        {ECA_Start, "Start"},
        {ECA_End,   "End"},
        {0}
      };

    DDB.Text  ("");
    if (PrjFileVerNo()>=55)
      {
      //DDB.Text  ("");
      DDB.Page  ("Integration", DDB_RqdPage);
      DDB.Text  ("");
      DDB.Byte  ("Method",            "", DC_,     "",    xidODEMethod,        &FlwSlvNd, 0/*isParm*/, DDBMethod);
      DDB.Byte  ("TimeIncCtrl",       "", DC_,     "",    xidODESSCtrl,         &FlwSlvNd, 0/*isParm*/, DDBIncCtrl);
      DDB.Long  ("IterationsPerStep", "", DC_,     "",    xidIterPerStep,       &FlwSlvNd, isParm );
      DDB.Double("TimeInc",           "", DC_Time, "s",   xidODETimeInc,        &FlwSlvNd, 0/*isParm*/ );
      DDB.Double("TimeIncMax",        "", DC_Time, "s",   xidODETimeIncMxRqd,   &FlwSlvNd, 0/*isParm*/ );
      DDB.Double("TimeIncMin",        "", DC_Time, "s",   xidODETimeIncMnRqd,   &FlwSlvNd, isParm );
      DDB.Double("DerivEstInc",       "", DC_Time, "s",   xidODEDerivEstInc,    &FlwSlvNd, isParm );
      DDB.Double("SettleTime",        "", DC_Time, "s",   xidODESettleTime,     &FlwSlvNd, isParm );
      DDB.Byte  ("CtrlEval",          "", DC_,     "",    &m_EvalCtrlAt,        &FlwSlvNd, isParmStopped, DDBCtrl);
      }
    //if (OrigPrjFileVerNo()<98)
    //  {
    //  DDB.Text  ("At startup...");
    //  DDB.CheckBoxBtn("RestartHistorian", "", DC_,   "",   xidAtStartRestartHist,   &FlwSlvNd, isParm, DDBOnOff);
    //  DDB.CheckBoxBtn("SetHistorianTagList", "", DC_, "",  xidAtStartSetHistTags,   &FlwSlvNd, isParm, DDBOnOff);
    //  DDB.CheckBoxBtn("ResetTime",    "", DC_,       "",   xidAtStartResetTime,     &FlwSlvNd, gs_Exec.SyncWithClock() ? 0 : isParm, DDBOnOff);
    //  DDB.String("TimeAtStart",       "", DC_,       "",   xidAtStartTime,          &FlwSlvNd, gs_Exec.SyncWithClock() ? 0 : isParm);
    //  }
    //DDB.Text  ("At startup...");

    DDB.Text  ("");
    DDB.Double("Mag_Eps_Abs",       "", DC_,      "",    xidODEMag_Eps_A,     &FlwSlvNd, isParm );
    DDB.Double("Mag_Eps_Rel",       "", DC_,      "",    xidODEMag_Eps_R,     &FlwSlvNd, isParm );
    DDB.Double("Int_Safety",        "", DC_,      "",    xidODESafetyScale,   &FlwSlvNd, isParm | InitHidden);
    DDB.Double("GrowLimit",         "", DC_Frac,  "%",   xidODEGrowLimit,     &FlwSlvNd, isParm | InitHidden);
    DDB.Double("Grow",              "", DC_Frac,  "%",   xidODEGrow,          &FlwSlvNd, isParm | InitHidden);
    DDB.Double("OKShrinkLimit",     "", DC_Frac,  "%",   xidODEOKShrinkLimit, &FlwSlvNd, isParm | InitHidden);
    DDB.Double("OKShrink",          "", DC_Frac,  "%",   xidODEOKShrink,      &FlwSlvNd, isParm | InitHidden);
    DDB.Double("BADShrink",         "", DC_Frac,  "%",   xidODEBADShrink,     &FlwSlvNd, isParm | InitHidden);
    DDB.Text  ("");
    DDB.Long  ("Steps",             "", DC_,      "",    xidODESteps,         &FlwSlvNd, noFile|noSnap);
    DDB.Long  ("GoodIters",         "", DC_,      "",    xidODEGoodIters,     &FlwSlvNd, noFile|noSnap);
    DDB.Long  ("BadIters",          "", DC_,      "",    xidODEBadIters,      &FlwSlvNd, noFile|noSnap);
    DDB.Long  ("Iters4Step",        "", DC_,      "",    xidODEIters4Step,    &FlwSlvNd, noFile|noSnap);

    DDB.Text  ("");
    DDB.String("WorstMagInt0",      "", DC_,      "",    xidWorstMagInt0,     &FlwSlvNd, InitHidden | noFile|noSnap);
    DDB.String("WorstMagInt1",      "", DC_,      "",    xidWorstMagInt1,     &FlwSlvNd, InitHidden | noFile|noSnap);
    DDB.String("WorstMagInt2",      "", DC_,      "",    xidWorstMagInt2,     &FlwSlvNd, InitHidden | noFile|noSnap);
    DDB.String("WorstMagInt3",      "", DC_,      "",    xidWorstMagInt3,     &FlwSlvNd, InitHidden | noFile|noSnap);
    DDB.String("WorstMagInt4",      "", DC_,      "",    xidWorstMagInt4,     &FlwSlvNd, InitHidden | noFile|noSnap);
    DDB.String("WorstMagInt5",      "", DC_,      "",    xidWorstMagInt5,     &FlwSlvNd, InitHidden | noFile|noSnap);
    DDB.String("WorstMagInt6",      "", DC_,      "",    xidWorstMagInt6,     &FlwSlvNd, InitHidden | noFile|noSnap);
    DDB.String("WorstMagInt7",      "", DC_,      "",    xidWorstMagInt7,     &FlwSlvNd, InitHidden | noFile|noSnap);
    DDB.String("WorstMagInt8",      "", DC_,      "",    xidWorstMagInt8,     &FlwSlvNd, InitHidden | noFile|noSnap);
    DDB.String("WorstMagInt9",      "", DC_,      "",    xidWorstMagInt9,     &FlwSlvNd, InitHidden | noFile|noSnap);
    DDB.Text  ("");
    //if (DDB.BeginStruct(&FlwSlvNd, "DateTime", NULL, DDB_NoPage))
    //  {
    //  //DDB.Double("Time",            "", DC_Time,  "s",   xidDTTime,           &FlwSlvNd, isResult|noFileAtAll);
    //  //DDB.String(""/*"TimeDesc"*/,     "HMS", DC_TimeStr, "Tm",  xidDTTimeHMS,        &FlwSlvNd, isResult|noFileAtAll);
    //  //DDB.String("",          "DayHMS", DC_TimeStr, "DyTm",    xidDTTimeDHMS,       &FlwSlvNd, isResult|noFileAtAll);
    //  //DDB.String(""/*"TimeFullDesc"*/,"HMSDate", DC_TimeStr, "TmDt", xidDTTimeDate,       &FlwSlvNd, isResult|noFileAtAll);
    //  DDB.Long  ("mSecsCount",      "", DC_,      "",    xidDTmSecsCount,     &FlwSlvNd, isResult|noFileAtAll);
    //  DDB.Long  ("SecsCount",       "", DC_,      "",    xidDTSecsCount,      &FlwSlvNd, isResult|noFileAtAll);
    //  DDB.Long  ("MinsCount",       "", DC_,      "",    xidDTMinsCount,      &FlwSlvNd, isResult|noFileAtAll);
    //  DDB.Long  ("HoursCount",      "", DC_,      "",    xidDTHoursCount,     &FlwSlvNd, isResult|noFileAtAll);
    //  DDB.Long  ("DaysCount",       "", DC_,      "",    xidDTDaysCount,      &FlwSlvNd, isResult|noFileAtAll);
    //  DDB.Long  ("Day",             "", DC_,      "",    xidDTDateDay,        &FlwSlvNd, isResult|noFileAtAll);
    //  DDB.Long  ("Month",           "", DC_,      "",    xidDTDateMonth,      &FlwSlvNd, isResult|noFileAtAll);
    //  DDB.Long  ("Year",            "", DC_,      "",    xidDTDateYear,       &FlwSlvNd, isResult|noFileAtAll);
    //  }
    //DDB.EndStruct();


    DDB.Page  ("Network", DDB_RqdPage);
    DDB.Text(" ");
    DDB.Text("Link Convergence");
    if (PrjFileVerNo()<53 || !DDB.ForFileSnpScn())
      {
      FlwNets.m_LnkEPS.GetAbsDD().BuildDataDefn(DDB, "Lnk_Eps_Abs", "", DC_, "", xidLnkEPSAbs, &FlwSlvNd, "Def", "Ovr");
      FlwNets.m_LnkEPS.GetRelDD().BuildDataDefn(DDB, "Lnk_Eps_Rel", "", DC_, "", xidLnkEPSRel, &FlwSlvNd, "Def", "Ovr");
      }
    DDB.Text(" ");
    DDB.Text("Network Convergence");
    DDB.Double("Net_DeRating",      "", DC_,      "",    xidLnkNetDerate,      &FlwSlvNd, isParm );
    DDB.Double("Net_MaxResRatio",   "", DC_,      "",    xidNetMaxResRatio,    &FlwSlvNd, isParm );
    DDB.Long  ("NetCnvCount",       "", DC_,      "",    xidNetCnvCount,       &FlwSlvNd, isParm );
    DDB.Long  ("Net_ResRunAvgStart","", DC_,      "",    xidNetResRunAvgStart, &FlwSlvNd, isParm );
    DDB.Long  ("Net_ResRunAvgCount","", DC_,      "",    xidNetResRunAvgCount, &FlwSlvNd, isParm );
    DDB.Long  ("UDFCnvCount",       "", DC_,      "",    xidUDFCnvCount,       &FlwSlvNd, isParm );
    DDB.CheckBoxBtn("FastConverge", "", DC_,      "",    xidNetFastConverge,   &FlwSlvNd, isParm, DDBOnOff);
    //DDB.CheckBoxBtn("ForceVapCalcs","", DC_,      "",    xidNetForceVFlow,     &FlwSlvNd, isParm, DDBOnOff);
    //DDB.CheckBoxBtn("ForceLiqCalcs","", DC_,      "",    xidNetForceLFlow,     &FlwSlvNd, isParm, DDBOnOff);

    DDB.Text(" ");
    DDB.Text("Flow Calculations");
    static DDBValueLst DDBNetCalcs[]={
      {0,  "Allow"},
      {1,  "Force"},
      {2,  "Block"},
      {0}};
    DDB.Byte ("NetFlow.Vapour","", DC_,      "",    xidNetVFlowOptions, &FlwSlvNd, isParm, DDBNetCalcs);
    DDB.Byte ("NetFlow.Liquid","", DC_,      "",    xidNetLFlowOptions, &FlwSlvNd, isParm, DDBNetCalcs);

    static DDBValueLst DDBShowAs[]={
      {FBB_ShowAs_Off,   "Off"},
      {FBB_ShowAs_Note,  "Note"},
      {FBB_ShowAs_Warn,  "Warning"},
      {FBB_ShowAs_Error, "Error"},
      {0}};
    DDB.Text(" ");
    DDB.Text  ("Velocity Limits");
    DDB.Byte("ShowVelLimitAs",      "",           DC_,      "",    &CFlwBlkBase::sm_iShowVelLimitAs,  &FlwSlvNd, isParm, DDBShowAs);
    DDB.Double("VelLimit.Liq",      "VelLimitSL", DC_Ldt,   "m/s", &CFlwBlkBase::sm_dVelLimitSL,      &FlwSlvNd, isParm);
    DDB.Double("VelLimit.Vap",      "VelLimitV",  DC_Ldt,   "m/s", &CFlwBlkBase::sm_dVelLimitV,       &FlwSlvNd, isParm);
    DDB.Text(" ");
    DDB.Text  ("Mass Flow Differences");
    DDB.Byte("ShowQmDiffAs",        "",           DC_,      "",    &CFlwBlkBase::sm_iShowQmDiffAs,    &FlwSlvNd, isParm, DDBShowAs);
    CFlwBlkBase::m_QmMatchTol.GetAbsDD().BuildDataDefn(DDB, "QmMatchAbs", "", DC_Qm,    "kg/s",    xidLnkQmMatchAbs, &FlwSlvNd, "Def", "Ovr");
    CFlwBlkBase::m_QmMatchTol.GetRelDD().BuildDataDefn(DDB, "QmMatchRel", "", DC_Frac,  "%",       xidLnkQmMatchRel, &FlwSlvNd, "Def", "Ovr");

    DDB.Text  ("");
    DDB.Text  ("Options");
    DDB.String("DefaultArea",       "", DC_,      "",    xidDefaultArea,  &FlwSlvNd, isParmStopped|isTag);
    //DDB.String("MissingSpills",     "", DC_,      "",    xidMissSpillAreas,  &FlwSlvNd, MultiLineStr|noFile|noSnap);
    DDB.Byte  ("Momentum",          "", DC_,      "",    &CFlwBlkBase::fMomentumOn,  &FlwSlvNd, isParm, DDBOnOff);
    DDB.Double("KFactMult",         "", DC_Frac,  "",    &gs_PressDropKFactMult, &FlwSlvNd, isParm);

    DDB.Page  ("Tears", DDB_RqdPage);
    //DDB.Double("Tear_EPS_Rel",      "", DC_Frac,  "%",   &DynEqnCB().Cfg.dEps_Rel,       &FlwSlvNd, isParm);
    //DDB.Double("Tear_EPS_Abs",      "", DC_Frac,  "%",   &DynEqnCB().Cfg.dEps_Abs,       &FlwSlvNd, isParm);
    if (DDB.ForFiling() && DDB.DoingPutData())
      {
      DDB.Long  ("Tear_MaxIters",     "", DC_,      "",    &DynEqnCB().Cfg.iMaxIters,      &FlwSlvNd, isParm);
      DDB.Double("Tear_Damping",      "", DC_Frac,  "%",   &DynEqnCB().Cfg.dDamping,       &FlwSlvNd, isParm);
      DDB.Byte  ("Tear_Method",       "", DC_,      "",    &DynEqnCB().Cfg.iConvergeMeth,  &FlwSlvNd, 0/*isParm */);
      }

    static DDBValueLst DDBTM[] =
      {
        {TCM_DirectSubs,  "DirectSubs"},
        {TCM_AdaptSubs,   "AdaptSubs"},
        {TCM_Wegstein,    "Wegstein"},
        //{TCM_Kalman,      "Kalman"},
        {0}
      };

    DDB.Text(" ");
    DDB.Text("Tolerance Defaults");
    DDB.Double("Tear.EPS_Rel",           "", DC_Frac, "%",   &DynEqnCB().Cfg.dEps_Rel,       &FlwSlvNd, isParm );
    DDB.Double("Tear.EPS_Abs",           "", DC_Frac, "%",   &DynEqnCB().Cfg.dEps_Abs,       &FlwSlvNd, isParm );
    DDB.Text(" ");
    DDB.Text("Convergence Defaults");
    DDB.Long  ("Tear.MaxIters",          "", DC_    , "",    &DynEqnCB().Cfg.iMaxIters,      &FlwSlvNd, isParm );
    //DDB.Short ("Tear.CnvgdIters",        "", DC_    , "",    &DynEqnCB().Cfg.iRqdCnvrgdIters,&FlwSlvNd, isParm );
    DDB.Byte  ("Tear.Method",            "", DC_    , "",    &DynEqnCB().Cfg.iConvergeMeth,  &FlwSlvNd, isParm, DDBTM);
    DDB.Double("Tear.Damping",           "", DC_Frac, "%",   &DynEqnCB().Cfg.dDamping,       &FlwSlvNd, isParm );
    DDB.Double("Tear.DampingGrowth",     "", DC_,     "",    &DynEqnCB().Cfg.dDampFctGrowth, &FlwSlvNd, isParm );
    DDB.Double("Tear.DampingDecay",      "", DC_,     "",    &DynEqnCB().Cfg.dDampFctDecay,  &FlwSlvNd, isParm );
    DDB.Short ("Tear.WegsteinDelay",     "", DC_,     "",    &DynEqnCB().Cfg.iWA_Delay,      &FlwSlvNd, isParm );
    DDB.Double("Tear.WegsteinBound",     "", DC_,     "",    &DynEqnCB().Cfg.dWA_Bound,      &FlwSlvNd, isParm );
    DDB.Double("Tear.WegsteinClamp",     "", DC_,     "",    &DynEqnCB().Cfg.dWA_Clamping,   &FlwSlvNd, isParm );

    DDB.Page  ("SS Monitor", DDB_RqdPage);
    static DDBValueLst DDBSSTests[]={
      {eSS_Off,       "Off"},
      {eSS_Network,   "Network"},
      {0}};
    DDB.Text  ("");
    DDB.Text  ("SteadyState Tests");
    DDB.Long  ("SS.Test",         "", DC_,      "",   (long*)&m_SSTest,       &FlwSlvNd, isParm, DDBSSTests);
    DDB.Long  ("SS.StopCount",    "", DC_,      "",   &m_SSStopCount,         &FlwSlvNd, isParm);//, DDBOnOff);
    DDB.Long  ("SS.MaxIters",     "", DC_,      "",   &m_SSMaxIters,          &FlwSlvNd, isParm);//, DDBOnOff);
    DDB.Text  ("Progress");
    DDB.Long  ("SS.Iteration",    "", DC_,      "",   &m_SSIters,             &FlwSlvNd, isResult);//, DDBOnOff);
    DDB.Long  ("SS.Steady",       "", DC_,      "",   &m_SSSteady,            &FlwSlvNd, isResult);//, DDBOnOff);
    DDB.Double("SS.MaxChange",    "", DC_Frac,  "%",  &m_SSMaxChange,         &FlwSlvNd, isResult);//, DDBOnOff);
    DDB.String("SS.MaxChangeTag", "", DC_,      "",   &m_SSMaxTag,            &FlwSlvNd, isResult|isTag);//, DDBOnOff);

    DDB.Text  ("Tolerances");
    m_SSPressTol.GetAbsDD().BuildDataDefn(DDB,  "SS.P.Abs",   "", DC_P,     "kPa",    xidSSPTolAbs,   &FlwSlvNd, "Def", "Ovr");
    m_SSPressTol.GetRelDD().BuildDataDefn(DDB,  "SS.P.Rel",   "", DC_Frac,  "%",      xidSSPTolRel,   &FlwSlvNd, "Def", "Ovr");
    m_SSQmTol.GetAbsDD().BuildDataDefn(DDB,     "SS.Qm.Abs",  "", DC_Qm,    "kg/s",   xidSSQmTolAbs,  &FlwSlvNd, "Def", "Ovr");
    m_SSQmTol.GetRelDD().BuildDataDefn(DDB,     "SS.Qm.Rel",  "", DC_Frac,  "%",      xidSSQmTolRel,  &FlwSlvNd, "Def", "Ovr");

    DDB.Page  ("Options", DDB_RqdPage);

    //if (DDB.BeginStruct(&FlwSlvNd, "EventLog", NULL, DDB_NoPage))
    //  {
    //  //DDB.Double("StopTime",          "", DC_Time,   "s",  &StopTime,               &FlwSlvNd, isParm|NAN_OK );
    //  DDB.Text  ("");
    //  DDB.CheckBoxBtn("On",   "", DC_,       "",   xidEventLogOn,           &FlwSlvNd, isParm, DDBOnOff);
    //  DDB.Long  ("Iteration", "", DC_,       "",   &gs_EventLog.lIteration, &FlwSlvNd, isResult|InitHidden);
    //  DDB.Double("StartTime", "", DC_Time,   "s",  &gs_EventLog.dStartTime, &FlwSlvNd, isResult|noView);
    //  }
    //DDB.EndStruct();

    DDB.Text  ("");
    if (DDB.BeginStruct(&FlwSlvNd, "HistorianTagList", NULL, DDB_NoPage))
      {
      //DDB.Text  ("Historian Tag List...");
      DDB.String("Folder",            "", DC_,       "",   xidHistTagFolder,        &FlwSlvNd, isResult);
      DDB.String("File",              "", DC_,       "",   xidHistTagFile,          &FlwSlvNd, isParm);
      DDB.Button("Edit",              "", DC_,       "",   xidHistTagEdit,          &FlwSlvNd, isParm);
      DDB.Button("Browse",            "", DC_,       "",   xidHistTagBrowse,        &FlwSlvNd, isParm);
      DDB.Button("Set_TagList_Now",   "", DC_,       "",   xidHistSetTagsNow,       &FlwSlvNd, isParm);
      }
    DDB.EndStruct();


    }
  DDB.EndStruct();
  }

//--------------------------------------------------------------------------

void FlwSlvDyn::BuildDataDefnOld(DataDefnBlk & DDB)
  {
  if (DDB.BeginStruct(&FlwSlvNd, "Dyn", NULL, DDB_RqdPage))
    {
    DDB.Page("Integration");
    static DDBValueLst DDB0[] = {
      {ODE_Euler,    "Euler"},
      {ODE_RK2,      "Runge-Kutta 2"},
      {ODE_RK4,      "Runge-Kutta 4"},
      {ODE_LastMethod,  "Self",         MDD_Hidden}, // Remove Old
      {ODE_LastMethod,  "Implicit",     MDD_Hidden}, // Remove Old
      {0}};
    DDB.Byte  ("Method",            "", DC_    , "",    xidODEMethod,       &FlwSlvNd, 0/*isParm*/, DDB0);
    static DDBValueLst DDB1[]={
      {ODE_SSC_Fixed, "Fixed"},
      {ODE_SSC_Var_1, "Variable"},
      {0}};
    DDB.Byte  ("StepSizeCtrl",      "", DC_    , "",    xidODESSCtrl,       &FlwSlvNd, 0/*isParm*/, DDB1);

    DDB.Double("Time",              "", DC_Time,  "s",   xidODETime,          &FlwSlvNd, 0 | DDEF_NOCOMPARE);
    DDB.Double("dTime",             "", DC_Time,  "s",   xidODETimeInc,       &FlwSlvNd, 0/*isParm*/ );
    //DDB.Double("StopTime",          "", DC_Time,  "s",   &StopTime,           &FlwSlvNd, isParm|NAN_OK );
    DDB.Double("StepSize",          "", DC_Time,  "s",   xidStepSizeRqd,      &FlwSlvNd, isParm );
    DDB.Double("dTimeMax",          "", DC_Time,  "s",   xidODETimeIncMxRqd,  &FlwSlvNd, isParm );
    DDB.Double("dTimeMin",          "", DC_Time,  "s",   xidODETimeIncMnRqd,  &FlwSlvNd, isParm );

    DDB.Double("Mag_Eps_Abs",       "", DC_,     "",    xidODEMag_Eps_A,      &FlwSlvNd, isParm );
    DDB.Double("Mag_Eps_Rel",       "", DC_,     "",    xidODEMag_Eps_R,      &FlwSlvNd, isParm );
    //DDB.Double("Int_dT_Max",        "", DC_,     "",    &IC.dTimeMax,       &FlwSlvNd, isParm );
    DDB.Double("Int_Safety",        "", DC_,     "",    xidODESafetyScale,    &FlwSlvNd, isParm | InitHidden);
    DDB.Double("GrowLimit",         "", DC_Frac, "%",   xidODEGrowLimit,      &FlwSlvNd, isParm | InitHidden);
    DDB.Double("Grow",              "", DC_Frac, "%",   xidODEGrow,           &FlwSlvNd, isParm | InitHidden);
    DDB.Double("OKShrinkLimit",     "", DC_Frac, "%",   xidODEOKShrinkLimit,  &FlwSlvNd, isParm | InitHidden);
    DDB.Double("OKShrink",          "", DC_Frac, "%",   xidODEOKShrink,       &FlwSlvNd, isParm | InitHidden);
    DDB.Double("BADShrink",         "", DC_Frac, "%",   xidODEBADShrink,      &FlwSlvNd, isParm | InitHidden);
    DDB.Text  ("");
    DDB.Long  ("GoodIters",         "", DC_    , "",    xidODEGoodIters,            &FlwSlvNd, noFile|noSnap);
    DDB.Long  ("BadIters",          "", DC_    , "",    xidODEBadIters,             &FlwSlvNd, noFile|noSnap);
    DDB.Long  ("Iters4Step",        "", DC_,     "",    xidODEIters4Step,         &FlwSlvNd, noFile|noSnap);
    DDB.String("WorstMagInt0",      "", DC_    , "",    xidWorstMagInt0,    &FlwSlvNd, InitHidden | noFile|noSnap);
    DDB.String("WorstMagInt1",      "", DC_    , "",    xidWorstMagInt1,    &FlwSlvNd, InitHidden | noFile|noSnap);
    DDB.String("WorstMagInt2",      "", DC_    , "",    xidWorstMagInt2,    &FlwSlvNd, InitHidden | noFile|noSnap);
    DDB.String("WorstMagInt3",      "", DC_    , "",    xidWorstMagInt3,    &FlwSlvNd, InitHidden | noFile|noSnap);
    DDB.String("WorstMagInt4",      "", DC_    , "",    xidWorstMagInt4,    &FlwSlvNd, InitHidden | noFile|noSnap);
    DDB.String("WorstMagInt5",      "", DC_    , "",    xidWorstMagInt5,    &FlwSlvNd, InitHidden | noFile|noSnap);
    DDB.String("WorstMagInt6",      "", DC_    , "",    xidWorstMagInt6,    &FlwSlvNd, InitHidden | noFile|noSnap);
    DDB.String("WorstMagInt7",      "", DC_    , "",    xidWorstMagInt7,    &FlwSlvNd, InitHidden | noFile|noSnap);
    DDB.String("WorstMagInt8",      "", DC_    , "",    xidWorstMagInt8,    &FlwSlvNd, InitHidden | noFile|noSnap);
    DDB.String("WorstMagInt9",      "", DC_    , "",    xidWorstMagInt9,    &FlwSlvNd, InitHidden | noFile|noSnap);

    DDB.Text  ("");
    DDB.Text  ("Network");
    DDB.Double("Lnk_Eps_Abs",       "", DC_,      "",    xidLnkEPSAbs,         &FlwSlvNd, isParm );
    DDB.Double("Lnk_Eps_Rel",       "", DC_,      "",    xidLnkEPSRel,         &FlwSlvNd, isParm );
    DDB.Double("Net_DeRating",      "", DC_,      "",    xidLnkNetDerate,      &FlwSlvNd, isParm );
    DDB.Long  ("NetCnvCount",       "", DC_,      "",    xidNetCnvCount,       &FlwSlvNd, isParm );
    DDB.Long  ("UDFCnvCount",       "", DC_,      "",    xidUDFCnvCount,       &FlwSlvNd, isParm );

    DDB.Text  ("");
    //DDB.String("SpillArea",         "", DC_, "",   xidDefaultArea,  &FlwSlvNd, isParmStopped|isTag);
    //DDB.String("MissingSpills",  "", DC_, "",   xidMissSpillAreas,  &FlwSlvNd, MultiLineStr|noFile|noSnap);
    DDB.Byte  ("Momentum",          "", DC_, "",   &CFlwBlkBase::fMomentumOn,  &FlwSlvNd, isParm, DDBOnOff);


    DDB.Text  ("");
    DDB.Page("DTears", DDB_RqdPage);
    DDB.Text  ("Tears");
    DDB.Double("Tear_EPS_Rel",      "", DC_Frac, "%",   &DynEqnCB().Cfg.dEps_Rel,       &FlwSlvNd, isParm);
    DDB.Double("Tear_EPS_Abs",      "", DC_Frac, "%",   &DynEqnCB().Cfg.dEps_Abs,       &FlwSlvNd, isParm);
    DDB.Long  ("Tear_MaxIters",     "", DC_    , "",    &DynEqnCB().Cfg.iMaxIters,      &FlwSlvNd, isParm);
    DDB.Double("Tear_Damping",      "", DC_Frac, "%",   &DynEqnCB().Cfg.dDamping,       &FlwSlvNd, isParm);
    DDB.Byte  ("Tear_Method",       "", DC_    , "",    &DynEqnCB().Cfg.iConvergeMeth,  &FlwSlvNd, 0/*isParm */);

//    DDB.Page("Statistics", DDB_RqdPage);
//    DDB.Text("Run Statistics");
//    DDB.Double("RunMultI",          "", DC_Frac, "%",   &Stats.MultI,       &FlwSlvNd, noFile|noSnap);
//    DDB.Double("RunMultO",          "", DC_Frac, "%",   &Stats.MultO,       &FlwSlvNd, noFile|noSnap);
//    DDB.Double("RunLoadI",          "", DC_Frac, "%",   &Stats.LoadI,       &FlwSlvNd, noFile|noSnap);
//    DDB.Double("RunLoadO",          "", DC_Frac, "%",   &Stats.LoadO,       &FlwSlvNd, noFile|noSnap);
//    DDB.Double("dTFrac",            "", DC_Frac, "%",   &Stats.dTFrac,      &FlwSlvNd, noFile|noSnap);
//    DDB.Double("ProcessorUse",      "", DC_Frac, "%",   &Stats.ThrdProcUse, &FlwSlvNd, noFile|noSnap);
//    DDB.Double("IterTime",          "", DC_Time, "s",   &Stats.IterTime,    &FlwSlvNd, noFile|noSnap);
//
//    DDB.Text("Ave Statistics");
//    DDB.Long("NStatesIntegrate",    "",  DC_ , "",   xidODENStatesInt, &FlwSlvNd, noFile|noSnap);
//    DDB.Long("NStatesSaveRest",     "",  DC_ , "",   xidODENStatesSR , &FlwSlvNd, noFile|noSnap);
    }
  DDB.EndStruct();
  };

//--------------------------------------------------------------------------

flag FlwSlvDyn::DataXchg(DataChangeBlk & DCB)
  {
  if (CTNode::DataXchg(DCB))
    return 1;

  if (FlwSlvBlk::DataXchg(DCB))
    return 1;

  switch (DCB.lHandle)
    {
    case xidODEMethod:
      if (DCB.rB)
        ODE().SetMethod(*DCB.rB);
      DCB.B=ODE().Method();
      return 1;
    case xidODESSCtrl:
      if (DCB.rB)
        ODE().SetStepSizeControl(*DCB.rB);
      DCB.B=ODE().StepSizeControl();
      return 1;
    case xidODETime:
      if (DCB.rD)
        IntCB.SetTime(*DCB.rD);
      DCB.D=IntCB.GetTime();
      return 1;
    case xidTimeF:
      DCB.pC=(LPTSTR)gs_Exec.TheTime.Format(DCB.pDataInfo->pCnvTxt);
      return 1;
    case xidTimeD:
      DCB.D=gs_Exec.TheTime.Seconds;
      return 1;
    case xidStepSizeF:
      if (DCB.rpC)
        {
        CTimeValue TV;
        if (TV.Parse(DCB.rpC))
          {
          double NSteps=Max(1.0, TimeCB().m_StepSizeMax.Seconds/GTZ(IntCB.GetTimeIncMxRqd()));
          gs_Exec.StepSizeMax=TV;

          IntCB.SetTimeIncMxRqd(gs_Exec.StepSizeMax/NSteps);
          IntCB.SetTimeInc(IntCB.GetTimeIncMxRqd());
          }
        }
      DCB.pC=(LPTSTR)gs_Exec.StepSizeMax.Format(DCB.pDataInfo->pCnvTxt);
      return 1;
    case xidStepSizeD:
      if (DCB.rD)
        {
        double NSteps=Max(1.0, TimeCB().m_StepSizeMax.Seconds/GTZ(IntCB.GetTimeIncMxRqd()));
        gs_Exec.StepSizeMax=*DCB.rD;

        IntCB.SetTimeIncMxRqd(gs_Exec.StepSizeMax/NSteps);
        IntCB.SetTimeInc(IntCB.GetTimeIncMxRqd());
        }
      DCB.D=gs_Exec.StepSizeMax.Seconds;
      return 1;
    case xidStepCount:
      DCB.L=gs_Exec.StepCount;
      return 1;
    case xidODETimeDesc:
      DCB.pC=(LPTSTR)gs_Exec.TheTime.Format(TD_Time);
      return 1;
    case xidODETimeFullDesc:
      DCB.pC=(LPTSTR)gs_Exec.TheTime.Format(TD_TimeDate);
      return 1;
    case xidODETimeInc:
      if (DCB.rD)
        IntCB.SetTimeInc(*DCB.rD);
      DCB.D=IntCB.GetTimeInc();
      return 1;
    case xidStepSizeRqd:
      if (DCB.rD)
        TimeCB().m_StepSizeMax=*DCB.rD;
      DCB.D=TimeCB().m_StepSizeMax;
      return 1;
    case xidIterPerStep:
      if (DCB.rL)
        {
        IntCB.SetTimeIncMxRqd(TimeCB().m_StepSizeMax.Seconds/Max(1L,*DCB.rL));
        IntCB.SetTimeInc(IntCB.GetTimeIncMxRqd());
        }
      DCB.L=(long)(TimeCB().m_StepSizeMax.Seconds/IntCB.GetTimeInc());
      return 1;
    case xidODETimeIncMxRqd:
      if (DCB.rD)
        IntCB.SetTimeIncMxRqd(Min(*DCB.rD, TimeCB().m_StepSizeMax.Seconds));
      DCB.D=IntCB.GetTimeIncMxRqd();
      return 1;
    case xidODETimeIncMnRqd:
      if (DCB.rD)
        IntCB.SetTimeIncMnRqd(Min(IntCB.GetTimeIncMxRqd(), *DCB.rD));
      DCB.D=IntCB.GetTimeIncMnRqd();
      return 1;
    case xidODEDerivEstInc:
      if (DCB.rD)
        IntCB.SetDerivEstInc(*DCB.rD);
      DCB.D=IntCB.GetDerivEstInc();
      return 1;
    case xidODESettleTime:
      if (DCB.rD)
        IntCB.SetSettleTime(*DCB.rD);
      DCB.D=IntCB.GetSettleTime();
      return 1;
    case xidODEMag_Eps_A:
      if (DCB.rD)
        ODE().SetMagEPSAbs(*DCB.rD);
      DCB.D=ODE().GetMagEPSAbs();
      return 1;
    case xidODEMag_Eps_R:
      if (DCB.rD)
        ODE().SetMagEPSRel(*DCB.rD);
      DCB.D=ODE().GetMagEPSRel();
      return 1;
    case xidODESafetyScale:
      if (DCB.rD)
        ODE().SetSafetyScale(*DCB.rD);
      DCB.D=ODE().GetSafetyScale();
      return 1;
    case xidODEGrowLimit:
      if (DCB.rD)
        ODE().SetGrowLimit(*DCB.rD);
      DCB.D=ODE().GetGrowLimit();
      return 1;
    case xidODEGrow:
      if (DCB.rD)
        ODE().SetGrow(*DCB.rD);
      DCB.D=ODE().GetGrow();
      return 1;
    case xidODEOKShrinkLimit:
      if (DCB.rD)
        ODE().SetOKShrinkLimit(*DCB.rD);
      DCB.D=ODE().GetOKShrinkLimit();
      return 1;
    case xidODEOKShrink:
      if (DCB.rD)
        ODE().SetOKShrink(*DCB.rD);
      DCB.D=ODE().GetOKShrink();
      return 1;
    case xidODEBADShrink:
      if (DCB.rD)
        ODE().SetBADShrink(*DCB.rD);
      DCB.D=ODE().GetBADShrink();
      return 1;
    case xidODESteps:
      DCB.L=TimeCB().m_StepCount;//XSW().nStepCnt;
      return 1;
    case xidODEGoodIters:
      DCB.L=ODE().GetGoodIters();
      return 1;
    case xidODEBadIters:
      DCB.L=ODE().GetBadIters();
      return 1;
    case xidODEIters4Step:
      DCB.L=ODE().GetIters4Step();
      return 1;
    case xidODENStatesInt:
      DCB.L=ODE().GetNStatesIntegrate();
      return 1;
    case xidODENStatesSR:
      DCB.L=ODE().GetNStatesSaveState();
      return 1;
    case xidRealTimeOn:
      if (DCB.rB)
        gs_Exec.SetRealTime(*DCB.rB);
      DCB.B=gs_Exec.RealTime();
      return 1;
    case xidRealTimeMultiplier:
      if (DCB.rD)
        gs_Exec.SetRealTimeMult(*DCB.rD);
      DCB.D=gs_Exec.RealTimeMult();
      return 1;
    case xidLnkEPSAbs:
      return FlwNets.m_LnkEPS.GetAbsDD().DataXchg(DCB, xidLnkEPSAbs, &FlwSlvNd);
    case xidLnkEPSRel:
      return FlwNets.m_LnkEPS.GetRelDD().DataXchg(DCB, xidLnkEPSRel, &FlwSlvNd);
    case xidLnkQmMatchAbs:
      return CFlwBlkBase::m_QmMatchTol.GetAbsDD().DataXchg(DCB, xidLnkQmMatchAbs, &FlwSlvNd);
    case xidLnkQmMatchRel:
      return CFlwBlkBase::m_QmMatchTol.GetRelDD().DataXchg(DCB, xidLnkQmMatchRel, &FlwSlvNd);
    case xidLnkNetDerate:
      if (DCB.rD)
        FlwNets.m_NetDeRating=*DCB.rD;
      DCB.D=FlwNets.m_NetDeRating;
      return 1;
    case xidNetCnvCount:
      if (DCB.rL)
        FlwNets.m_lNetCnvCnt=Range(10L,*DCB.rL,100L);
      DCB.L=FlwNets.m_lNetCnvCnt;
      return 1;
    case xidUDFCnvCount:
      if (DCB.rL)
        FlwNets.m_lMaxUDFIters=Range(10L,*DCB.rL,100L);
      DCB.L=FlwNets.m_lMaxUDFIters;
      return 1;
    case xidNetMaxResRatio:
      if (DCB.rD)
        FlwNets.m_dNetMaxResRatio=*DCB.rD;
      DCB.D=FlwNets.m_dNetMaxResRatio;
      return 1;
    case xidNetFastConverge:
      if (DCB.rB)
        FlwNets.m_fAllowFastConverge=*DCB.rB;
      DCB.B=FlwNets.m_fAllowFastConverge;
      return 1;
    case xidNetResRunAvgStart:
      if (DCB.rL)
        FlwNets.m_lResRunAvgStart=*DCB.rL;
      DCB.L=FlwNets.m_lResRunAvgStart;
      return 1;
    case xidNetResRunAvgCount:
      if (DCB.rL)
        FlwNets.m_lResRunAvgCount=*DCB.rL;
      DCB.L=FlwNets.m_lResRunAvgCount;
      return 1;
    case xidNetVFlowOptions:
      if (DCB.rB)
        {
        switch (*DCB.rB)
          {
          case 0: // Allow
            CFlwBlkData::sm_iFlwPh_Force &= ~FPh_Gasses;
            CFlwBlkData::sm_iFlwPh_Block &= ~FPh_Gasses;
            break;
          case 1: // Force
            CFlwBlkData::sm_iFlwPh_Force |= FPh_Gasses;
            CFlwBlkData::sm_iFlwPh_Block &= ~FPh_Gasses;
            break;
          case 2: // Block
            CFlwBlkData::sm_iFlwPh_Force &= ~FPh_Gasses;
            CFlwBlkData::sm_iFlwPh_Block |= FPh_Gasses;
            break;
          }
        }
      if (CFlwBlkData::sm_iFlwPh_Force & FPh_Gasses)
        DCB.B=1; // Force
      else if (CFlwBlkData::sm_iFlwPh_Block & FPh_Gasses)
        DCB.B=2; // Block
      else
        DCB.B=0;
      return 1;
    case xidNetLFlowOptions:
      if (DCB.rB)
        {
        switch (*DCB.rB)
          {
          case 0: // Allow
            CFlwBlkData::sm_iFlwPh_Force &= ~FPh_Liquid;
            CFlwBlkData::sm_iFlwPh_Block &= ~FPh_Liquid;
            break;
          case 1: // Force
            CFlwBlkData::sm_iFlwPh_Force |= FPh_Liquid;
            CFlwBlkData::sm_iFlwPh_Block &= ~FPh_Liquid;
            break;
          case 2: // Block
            CFlwBlkData::sm_iFlwPh_Force &= ~FPh_Liquid;
            CFlwBlkData::sm_iFlwPh_Block |= FPh_Liquid;
            break;
          }
        }
      if (CFlwBlkData::sm_iFlwPh_Force & FPh_Liquid)
        DCB.B=1; // Force
      else if (CFlwBlkData::sm_iFlwPh_Block & FPh_Liquid)
        DCB.B=2; // Block
      else
        DCB.B=0;
      return 1;
    //case xidEventLogOn:
    //  if (DCB.rB)
    //    gs_EventLog.SetUseLog(*DCB.rB ? 1 : 0);
    //  DCB.B=gs_EventLog.UsingLog();
    //  return 1;
    case xidHistTagFolder:
      {
      Strng Path(gs_Exec.HstTaglistFolder);
      if (DCB.rpC)
        {
        Path = DCB.rpC;
        Path.FnContract();
        gs_Exec.HstTaglistFolder=DCB.rpC;
        }
      if (DCB.ForFileSnpScn())
        {
        Path.FnContract();//ensure it is always in contract form!
        }
      else
        {
        if (SymbolicPaths())
          Path.FnContract();
        else
          Path.FnExpand();
        }
      m_sDispHistTagListPath = Path();
      DCB.pC = m_sDispHistTagListPath();
      return 1;
      }
    case xidHistTagFile:
      if (DCB.rpC)
        gs_Exec.HstTaglistFile=DCB.rpC;
      DCB.pC=gs_Exec.HstTaglistFile;
      return 1;
    case xidHistTagEdit:
      if (DCB.rB && (*DCB.rB!=0))
        {
        Strng FullFilename;
        HANDLE hProcess = NULL;
        DWORD dwProcessId = 0;
        Strng File(gs_Exec.HstTaglistFile);
        if (File.Len()==0)
          {
          File = "HistTagList.txt";
          gs_Exec.HstTaglistFolder=PrjFiles();
          gs_Exec.HstTaglistFile=File();
          FullFilename = gs_Exec.HstTaglistFolder;
          FullFilename.FnExpand();
          FullFilename += File();
          FILE* f = fopen(FullFilename(), "wt");
          if (f)
            {
            char Buff[2048];
            strcpy(Buff, "Tag\n");
            fwrite(Buff, sizeof(char), strlen(Buff), f);
            fclose(f);
            }
          FullFilename = "";
          }
        Strng Path(gs_Exec.HstTaglistFolder);
        int RetCode = DCB.DoEditBtn(hProcess, dwProcessId, Path(), File(), NULL, &FullFilename, true);
        if (RetCode==-1)
          {
          FILE* f = fopen(FullFilename(), "wt");
          if (f)
            {
            char Buff[2048];
            strcpy(Buff, "Tag\n");
            fwrite(Buff, sizeof(char), strlen(Buff), f);
            fclose(f);
            }
          RetCode = DCB.DoEditBtn(hProcess, dwProcessId, Path(), File(), NULL);
          }
        }
      DCB.B=0;
      return 1;
    case xidHistTagBrowse:
      if (DCB.rB && (*DCB.rB!=0))
        {
        Strng Path(gs_Exec.HstTaglistFolder);
        Strng File(gs_Exec.HstTaglistFile);
        DCB.DoBrowseBtn(NULL/*"csv"*/, "SysCAD Historian Tag List", Path, File);
        gs_Exec.HstTaglistFolder=Path();
        gs_Exec.HstTaglistFile=File();
        }
      DCB.B=0;
      return 1;
    case xidHistSetTagsNow:
      if (DCB.rB && (*DCB.rB!=0))
        {
        Strng File(gs_Exec.HstTaglistFile);
        if (File.Len()>0)
          {
          Strng FullFilename(gs_Exec.HstTaglistFolder);
          if (FullFilename.Len()==0)
            {
            FullFilename = PrjFiles();
            gs_Exec.HstTaglistFolder=FullFilename();
            }
          FullFilename.FnExpand();
          FullFilename += File();
          gs_Exec.KeepHistoryFile(FullFilename());
          }
        }
      DCB.B=0;
      return 1;
    case xidAtStartResetTime:
      if (DCB.rB)
        gs_Exec.ResetTimeOnStart=*DCB.rB;
      DCB.B=gs_Exec.ResetTimeOnStart;
      return 1;
    case xidAtStartRestartHist:
      if (DCB.rB)
        gs_Exec.m_RestartHstOnStart=*DCB.rB;
      DCB.B=gs_Exec.m_RestartHstOnStart;// || gs_Exec.ResetTimeOnStart;
      return 1;
    case xidAtStartSetHistTags:
      if (DCB.rB)
        gs_Exec.m_SetHstTaglistOnStart=*DCB.rB;
      DCB.B=gs_Exec.m_SetHstTaglistOnStart;
      return 1;
    case xidAtStartRestartArchive:
      if (DCB.rB)
        gs_Exec.m_RestartArchiveOnStart=*DCB.rB;
      DCB.B=gs_Exec.m_RestartArchiveOnStart;
      return 1;
    case xidAtStartRestartPGM:
      #if WithRestartPGM
      if (DCB.rB)
        gs_Exec.RestartPGMOnStart=*DCB.rB;
      DCB.B=gs_Exec.RestartPGMOnStart;
      #else
      DCB.B=0;
      #endif
      return 1;
    case xidAtStartRestartPrf:
      if (DCB.rB)
        gs_Exec.m_RestartPrfOnStart=*DCB.rB;
      DCB.B=gs_Exec.m_RestartPrfOnStart;
      return 1;
    case xidAtStartTimeF:
    case xidAtStartTimeD:
      {
      if (DCB.rpC || DCB.rD)
        {
        CTimeValue RqdTime;
        flag OK=true;
        if (DCB.lHandle==xidAtStartTimeF)
          OK = RqdTime.Parse(DCB.rpC);//HMSDatetoSecs(DCB.rpC, RqdTime);
        else
          RqdTime.Seconds=*DCB.rD;

        if (OK)
          {
          if (IntCB.GetTimeInc()>86400.0*0.999) //1day
            {
            RqdTime=RqdTime.Round(CTimeValue(3600.0));
            //RqdTime = floor(RqdTime/3600.0+0.5)*3600.0; //round to hour
            }
          else if (IntCB.GetTimeInc()>3600.0*0.999) //1hour
            {
            RqdTime=RqdTime.Round(CTimeValue(60.0));
            //RqdTime = floor(RqdTime/60.0+0.5)*60.0; //round to minute
            }
          else if (IntCB.GetTimeInc()>60.0*0.999) //1minute
            {
            RqdTime=RqdTime.Round(CTimeValue(1.0));
            //RqdTime = floor(RqdTime/1.0+0.5)*1.0; //round to second
            }
          gs_Exec.TimeAtStart=Max(0.0, RqdTime);
          }
        }
      if (DCB.lHandle==xidAtStartTimeF)
        DCB.pC=(LPTSTR)gs_Exec.TimeAtStart.Format(TD_Time|TD_DaysJoined);
      else
        DCB.D=gs_Exec.TimeAtStart.Seconds;
      return 1;
      }

    case xidAtStartAllZeroFlows:
      if (DCB.rB)
        {
        };
      DCB.B=0;
      return 1;
    case xidAtStartAllPreSet:
      if (DCB.rB)
        {
        };
      DCB.B=0;
      return 1;
    case xidAtStartAllSteadyState:
      if (DCB.rB)
        {
        };
      DCB.B=0;
      return 1;
    case xidAtStartAllEmptySpillTargets:
      if (DCB.rB)
        gs_Exec.m_RestartEmptySpills = *DCB.rB;
      DCB.B=gs_Exec.m_RestartEmptySpills;
      return 1;
    case xidAtStartAllEmpty:
      if (DCB.rB)
        gs_Exec.m_RestartEmptyAll = *DCB.rB;
      DCB.B=gs_Exec.m_RestartEmptyAll;
      return 1;
    case xidAtStartAllDynStatsRunInit:
      if (DCB.rB)
        gs_Exec.m_RestartResetStats = *DCB.rB;
      DCB.B=gs_Exec.m_RestartResetStats;
      return 1;

    case xidDTTime:
      DCB.D=gs_Exec.TheTime.Seconds;
      return 1;
    case xidDTTimeHMS:
      DCB.pC=(LPTSTR)gs_Exec.TheTime.Format(TD_Time|TD_TrimTime);
      return 1;
    case xidDTTimeDHMS:
      DCB.pC=(LPTSTR)gs_Exec.TheTime.Format(TD_Time|TD_DaysJoined|TD_TrimTime);
      return 1;
    case xidDTTimeDate:
      DCB.pC=(LPTSTR)gs_Exec.TheTime.Format(TD_TimeDate);
      return 1;
    case xidDTmSecsCount:
      DCB.L=gs_Exec.TheTime.TimemSeconds;
      return 1;
    case xidDTSecsCount:
      DCB.L=gs_Exec.TheTime.TimeSeconds;
      return 1;
    case xidDTMinsCount:
      DCB.L=gs_Exec.TheTime.TimeMinutes;
      return 1;
    case xidDTHoursCount:
      DCB.L=gs_Exec.TheTime.TimeHours;
      return 1;
    case xidDTDaysCount:
      DCB.L=gs_Exec.TheTime.TimeDays;
      return 1;
    case xidDTDateDay:
      DCB.L=gs_Exec.TheTime.DateDay;
      return 1;
    case xidDTDateMonth:
      DCB.L=gs_Exec.TheTime.DateMonth;
      return 1;
    case xidDTDateYear:
      DCB.L=gs_Exec.TheTime.DateYear;
      return 1;
    case xidDTDateDayOfWeek:
      DCB.L=gs_Exec.TheTime.DayOfWeek;
      return 1;
    case xidDTDateDayOfYear:
      DCB.L=gs_Exec.TheTime.DayOfYear;
      return 1;
    case xidDTDateDaysInMonth :
      DCB.L=gs_Exec.TheTime.DaysInMonth;
      return 1;
    case xidDTDateDaysInYear:
      DCB.L=gs_Exec.TheTime.DaysInYear;
      return 1;
    case xidDTDateDayOfWeekDesc:
      DCB.pC=(LPTSTR)gs_Exec.TheTime.DayOfWeekDesc;
      return 1;
    case xidDTDateMonthDesc:
      DCB.pC=(LPTSTR)gs_Exec.TheTime.DateMonthDesc;
      return 1;

    case xidScnState:
      if (DCB.rB)
        gs_Exec.ScnState=(eScnState)*DCB.rB;
      DCB.B=gs_Exec.ScnState;
      return 1;
    case xidManualScnRestart:
      if (DCB.rB)
        gs_Exec.ManualScnReStart=(eScnContRest)*DCB.rB;
      DCB.B=gs_Exec.ManualScnReStart;
      return 1;
    case xidReStartIfScnBusy:
      if (DCB.rB)
        gs_Exec.ReStartIfScnBusy=(eScnContRest)*DCB.rB;
      DCB.B=gs_Exec.ReStartIfScnBusy;
      return 1;
    case xidReStartIfScnComplete:
      if (DCB.rB)
        gs_Exec.ReStartIfScnComplete=(eScnBlkRest)*DCB.rB;
      DCB.B=gs_Exec.ReStartIfScnComplete;
      return 1;

    case xidScnType:
      if (DCB.rL)
        gs_Exec.SetScnType((eScnTypes)*DCB.rL, !DCB.ForFileSnpScn());
      DCB.L=gs_Exec.ScnType;
      return 1;
    case xidScnTypeLast:
      if (DCB.rL)
        gs_Exec.ScnTypeLast=(eScnTypes)*DCB.rL;
      DCB.L=gs_Exec.ScnTypeLast;
      return 1;
    case xidScnTermAction:
      if (DCB.rL)
        gs_Exec.ScnTermAction=(eScnTermActions)*DCB.rL;
      DCB.L=gs_Exec.ScnTermAction;
      return 1;
    case xidRunDurationF:
      if (DCB.rpC)
        gs_Exec.ScnDuration=CTimeValue(DCB.rpC);
      DCB.pC=(LPTSTR)gs_Exec.ScnDuration.Format(TD_Time|TD_DaysJoined|TD_TrimTime|TD_IntegralSecs);
      return 1;
    case xidRunDurationD:
      if (DCB.rD)
        gs_Exec.ScnDuration=*DCB.rD;
      DCB.D=gs_Exec.ScnDuration.Seconds;
      return 1;
    case xidRunStopTimeF:
      if (DCB.rpC)
        gs_Exec.ScnStopTime=CTimeValue(DCB.rpC);
      DCB.pC=(LPTSTR)gs_Exec.ScnStopTime.Format(TD_Time|TD_DaysJoined|TD_TrimTime|TD_IntegralSecs);
      return 1;
    case xidRunStopTimeD:
      if (DCB.rD)
        gs_Exec.ScnStopTime=*DCB.rD;
      DCB.D=gs_Exec.ScnStopTime.Seconds;
      return 1;
    case xidStepCountMax:
      if (DCB.rL)
        gs_Exec.StepCountMax=*DCB.rL;
      DCB.L=gs_Exec.StepCountMax;
      return 1;
    case xidRunTimeToStopF:
      if (DCB.rpC)
        gs_Exec.TimeToStop=CTimeValue(DCB.rpC);
      DCB.pC=(LPTSTR)gs_Exec.TimeToStop.Format(TD_Time|TD_DaysJoined|TD_TrimTime|TD_IntegralSecs);
      return 1;
    case xidRunTimeToStopD:
      if (DCB.rD)
        gs_Exec.TimeToStop=*DCB.rD;
      DCB.D=gs_Exec.TimeToStop.Seconds;
      return 1;
    case xidScnTimeF:
      if (DCB.rpC)
        gs_Exec.ScnTime=CTimeValue(DCB.rpC);
      DCB.pC=(LPTSTR)gs_Exec.ScnTime.Format(TD_Time|TD_DaysJoined|TD_TrimTime|TD_IntegralSecs);
      return 1;
    case xidScnTimeD:
      if (DCB.rD)
        gs_Exec.ScnTime=*DCB.rD;
      DCB.D=gs_Exec.ScnTime.Seconds;
      return 1;
    case xidScnTimeMaxF:
      if (DCB.rpC)
        gs_Exec.ScnTimeMax=CTimeValue(DCB.rpC);
      DCB.pC=(LPTSTR)gs_Exec.ScnTimeMax.Format(TD_Time|TD_DaysJoined|TD_TrimTime|TD_IntegralSecs);
      return 1;
    case xidScnTimeMaxD:
      if (DCB.rD)
        gs_Exec.ScnTimeMax=*DCB.rD;
      DCB.D=gs_Exec.ScnTimeMax.Seconds;
      return 1;

    case xidProgress:
      if (DCB.rD)
        gs_Exec.DynProgress=Valid(*DCB.rD) ? *DCB.rD : -1.0;
      DCB.D=(gs_Exec.DynProgress>=0 ? gs_Exec.DynProgress : dNAN);
      return 1;
    case xidClock:
      //if (DCB.rD)
      //  gs_Exec.Clock=*DCB.rD;
      DCB.D=gs_Exec.Clock;
      return 1;
    case xidClockToGo:
      //if (DCB.rD)
      //  gs_Exec.ClockEstToGo=*DCB.rD;
      DCB.D=gs_Exec.ClockEstToGo;
      return 1;
    case xidClockTotal:
      //if (DCB.rD)
      //  gs_Exec.ClockEstTotal=*DCB.rD;
      DCB.D=gs_Exec.ClockEstTotal;
      return 1;
    case xidElapsedSteps:
      if (DCB.rL)
        gs_Exec.StepCount=*DCB.rL;
      DCB.L=gs_Exec.StepCount;
      return 1;

    case xidDynCmdScnRestart:
      if (DCB.rB && (*DCB.rB!=0))
        {
        CWaitCursor Wait;
        gs_Exec.ResetRunState(true, 0);
        };
      DCB.B=0;
      return 1;
    case xidDynCmdResetTime:
      if (DCB.rB && (*DCB.rB!=0))
        {
        CWaitCursor Wait;
        //gs_Exec.ResetRunState(true, 0);
        };
      DCB.B=0;
      return 1;
    case xidDynCmdRestartHist:
      if (DCB.rB && (*DCB.rB!=0))
        {
        CWaitCursor Wait;
        //gs_Exec.ResetRunState(true, 0);
        };
      DCB.B=0;
      return 1;

    case xidSSPTolAbs:
      return m_SSPressTol.GetAbsDD().DataXchg(DCB, xidSSPTolAbs, &FlwSlvNd);
    case xidSSPTolRel:
      return m_SSPressTol.GetAbsDD().DataXchg(DCB, xidSSPTolRel, &FlwSlvNd);
    case xidSSQmTolAbs:
      return m_SSQmTol.GetAbsDD().DataXchg(DCB, xidSSQmTolAbs, &FlwSlvNd);
    case xidSSQmTolRel:
      return m_SSQmTol.GetAbsDD().DataXchg(DCB, xidSSQmTolRel, &FlwSlvNd);

    default:
      if (DCB.lHandle>=xidWorstMagInt0 && DCB.lHandle<=xidWorstMagInt9)
        {
        static Strng Buff;
        Strng Tag;
        double Tol;
        int i=DCB.lHandle-xidWorstMagInt0;
        switch (ODE().BadTolInfo(i, Tag, Tol))
          {
          case 1: Buff.Set("%s * %6.2f", Tag(), Tol); break;
          case 2: Buff.Set("%s - %6.2f", Tag(), Tol); break;
          case 3: Buff.Set("%s Lo %6.2f", Tag(), Tol); break;
          case 4: Buff.Set("%s Hi %6.2f", Tag(), Tol); break;
          default : Buff=""; break;
          }
        DCB.pC=Buff();
        return 1;
        }
    }

  return 0;
  }

//--------------------------------------------------------------------------

flag FlwSlvDyn::ValidateData(ValidateDataBlk & VDB)
  {
  return 1;
  }

//--------------------------------------------------------------------------

flag FlwSlvDyn::CreateSolutionBlks(int NRqdThreads)
  {
  FlwNets.CreateSolutionBlks(NRqdThreads);
  FlwSlvNd.ClrCI(4);

  if (FlwSlvBlk::CreateSolutionBlks(NRqdThreads))
    {
    m_SolutionBlksOk=true;
    flag OK=m_SolutionBlksOk || m_PwrCtrlNds.GetCount()>0 || m_StandAloneNds.GetCount()>0;
    return OK;
    }

  return false;
  }

//--------------------------------------------------------------------------

int FlwSlvDyn::ClearSolutionBlks(flag ClearAll)
  {
  FlwNets.ClearSolutionBlks();

  int Ret=FlwSlvBlk::ClearSolutionBlks(ClearAll);

  return Ret;
  }

//---------------------------------------------------------------------------

//void FlwSlvDyn::FITPowerRequired(CLkFlwNodeIter & Iter)
//  {
//  FlwNode * p;
//  while (Iter.GetNext(p))
//    {
//    XT.StartLoop(p);
//    XT.LockDone();//(p, eFITPowers, FSW_Null);
//    p->LockFlanges();
//    p->EvalPower();
//    p->CheckDataRanges();
//    XT.ExecDone();//Postamble(p, eFITPowers, FSW_Null);
//    p->UnLockFlanges();
//    XT.EndLoop();
//    }
//  }

//---------------------------------------------------------------------------

void FlwSlvDyn::FITStates(CFlwNodeList & List)//CLkFlwNodeIter & Iter)
  {
  FlwNode * p;
  CThreadTimer  XT(eFITStates, FSW_Null);
  //while (Iter.GetNext(p))
  CFlwNodeIter Iter(List);
  for (p=Iter.First(); p; p=Iter.Next())
    {
    XT.StartLoop(p);
    //p->LockFlanges();
    p->LockNode();
    XT.LockDone();//(p, eFITStates, FSW_Null);
    p->EvalState();
    p->CheckDataRanges(true);
    XT.ExecDone();//Postamble(p, eFITStates, FSW_Null);
    //p->UnLockFlanges();
    p->UnLockNode();
    XT.EndLoop();//PostambleUnLock();
    }
  XT.Complete();//PostambleTl();
  }

//--------------------------------------------------------------------------

void FlwSlvDyn::EvalAllStates()
  {
  if (FSTC.NThreads())
    {
    //CLkFlwNodeIter TCAll(&m_AllNds);
    FSTC.DoJob(eFITStates, this, &m_AllNdsLA);//, &TCAll);
    }
  else
    {
    CThreadTimer  XT(eFITStates, FSW_Null);
    CFlwNodeIter All(m_AllNds);
    for (FlwNode * p=All.First(); (p); p=All.Next())
      {
      XT.StartLoop(p);
      XT.LockDone();//(p, eFITStates, FSW_Null);
      p->EvalState();
      p->CheckDataRanges(true);
      XT.ExecDone();//Postamble(p, eFITStates, FSW_Null);
      XT.EndLoop();
      }
    XT.Complete();//PostambleTl();
    }
  }

//---------------------------------------------------------------------------

void FlwSlvDyn::FITStartStep(CFlwNodeList & List)//CLkFlwNodeIter & Iter)
  {
  FlwNode * p;
  CThreadTimer  XT(eFITStartStep, FSW_StartStep);
  //while (Iter.GetNext(p))
  CFlwNodeIter Iter(List);
  for (p=Iter.First(); p; p=Iter.Next())
    {
    XT.StartLoop(p);
    //p->LockFlanges();
    p->LockNode();
    XT.LockDone();//(p, eFITStartStep, FSW_StartStep);
    p->BeforeNode();
    p->StartStep();
    p->AfterNode(NPM_StartStep);
    p->StepFlowInfo();
    XT.ExecDone();//Postamble(p, eFITStartStep, FSW_StartStep);
    //p->UnLockFlanges();
    p->UnLockNode();
    XT.EndLoop();//PostambleUnLock();
    }
  XT.Complete();//PostambleTl();
  }

//--------------------------------------------------------------------------

void FlwSlvDyn::ODEStartStep()
  {
  FlwSlvNd.SetCI(4, gs_PressDropKFactMult!=1.0);

  if (FSTC.NThreads())
    {
    //CLkFlwNodeIter TCAll(&m_AllNds);
    FSTC.DoJob(eFITStartStep, this, &m_AllNdsLA);//, &TCAll);
    }
  else
    {
    CThreadTimer  XT(eFITStartStep, FSW_StartStep);
    CFlwNodeIter All(m_AllNds);
    for (FlwNode * p=All.First(); (p); p=All.Next())
      {
      XT.StartLoop(p);
      XT.LockDone();//(p, eFITStartStep, FSW_StartStep);
      p->BeforeNode();
      p->StartStep();
      p->AfterNode(NPM_StartStep);
      p->StepFlowInfo();
      XT.ExecDone();//Postamble(p, eFITStartStep, FSW_StartStep);
      XT.EndLoop();
      }
    XT.Complete();//PostambleTl();
    }
  }

//---------------------------------------------------------------------------

void FlwSlvDyn::FITFlowInfo(CFlwNodeList & List)//CLkFlwNodeIter & Iter)
  {
  FlwNode * p;
  CThreadTimer  XT(eFITFlowInfo, FSW_Null);
  //while (Iter.GetNext(p))
  CFlwNodeIter Iter(List);
  for (p=Iter.First(); p; p=Iter.Next())
    {
    XT.StartLoop(p);
    //p->LockFlanges();
    p->LockNode();
    XT.LockDone();//(p, eFITFlowInfo, FSW_Null);
    p->EvalFlowInfoStart();
    p->EvalJoinPressures(-1);
    if (p->fEvalPressSensReqd)
      p->EvalPressureSens();
    XT.ExecDone();//Postamble(p, eFITFlowInfo, FSW_Null);
    //p->UnLockFlanges();
    p->UnLockNode();
    XT.EndLoop();//PostambleUnLock();
    }
  XT.Complete();//PostambleTl();
  }

//---------------------------------------------------------------------------

void FlwSlvDyn::FITEvalProductsSurge(CLkNodeEvalIter & Iter)
  {
  
  STARTPROGRESS;
  SHOWPROGRESS(10, "EvalProductsSurge 1");
  //FlwNode * p;
  CNodeEvalIndex NJ;
  int Cnt;
  CThreadTimer  XT(eFITEvalProductsSurge, FSW_EvalProductsSurge);

  while (Iter.RemoveNext(NJ, &Cnt))
    {
    FlwNode * p=&NJ.Nd;
    
    XT.StartLoop(p);
    ProcLoopStart(Iter, NJ, Cnt);
    //p->LockFlanges();
    //p->LockNode();
    XT.LockDone();//(p, eFITEvalProductsSurge, FSW_EvalProductsSurge);
    // Cluster Mods
    p->EvalProductsSurge(NJ);
    p->fEvalProductsSurgeCalled=true;
    XT.ExecDone();//Postamble(p, eFITEvalProductsSurge, FSW_EvalProductsSurge);
    ProcLoopEnd(Iter, NJ, Cnt);
    XT.EndLoop();//PostambleUnLock();

    #if dbgFlwSolve
    if (dbgThreadsSleep())
      Sleep(0);
    #endif
    }
  XT.Complete();//PostambleTl();

  SHOWPROGRESS(1, "EvalProductsSurge 2");
  }

//---------------------------------------------------------------------------

void FlwSlvDyn::FITEvalProducts(CLkNodeEvalIter & Iter)
  {
  STARTPROGRESS;
  SHOWPROGRESS(10, "EvalProducts 1");

  CNodeEvalIndex NJ;
  int Cnt;
  FlwNode * p=NULL;
  SCDTRY
    {
    CThreadTimer  XT(eFITEvalProducts, FSW_EvalProducts);
    while (Iter.RemoveNext(NJ, &Cnt))
      {
      p=&NJ.Nd;
      XT.StartLoop(p);
      ProcLoopStart(Iter, NJ, Cnt);
      //    p->LockFlanges();
      //if (p->SSMode())
      //  {
      //  XT.LockDone();//(p, eFITEvalSteadyState, FSW_EvalSteadyState);
      //  p->EvalSteadyState();
      //  XT.ExecDone();//Postamble(p, eFITEvalSteadyState, FSW_EvalSteadyState);
      //  }
      XT.LockDone();//(p, eFITEvalProducts, FSW_EvalProducts);
      //dbgpln("---------------------------- %s", p->FullObjTag());
      p->BeforeNode(NPM_BeforeEvalProducts);
      CALC_EVAL_ORDER(NJ);
      p->EvalProducts(NJ);
      p->AfterNode(NPM_AfterEvalProducts);
      p->fEvalProductsCalled=true;
      XT.ExecDone();//Postamble(p, eFITEvalProducts, FSW_EvalProducts);

      ProcLoopEnd(Iter, NJ, Cnt);//false);
      XT.EndLoop();//PostambleUnLock();
#if dbgFlwSolve
      if (dbgThreadsSleep())
        Sleep(0);
#endif
      p=NULL;
      }
    XT.Complete();//PostambleTl();
    }
  SCDCATCH("FlwSlvDyn::FITEvalProducts", p?p->FullObjTag():NULL)
  SHOWPROGRESS(1, "EvalProducts 2");
  }

//---------------------------------------------------------------------------

void FlwSlvDyn::FITEvalDerivs(CLkNodeEvalIter & Iter)
  {
  STARTPROGRESS;
  SHOWPROGRESS(10, "EvalDerivs 1");

  CNodeEvalIndex NJ;
  int Cnt;
  CThreadTimer  XT(eFITEvalDerivs, FSW_EvalDerivs);
  while (Iter.RemoveNext(NJ, &Cnt))
    {
    FlwNode * p=&NJ.Nd;
    XT.StartLoop(p);
    //p->LockFlanges();
    ProcLoopStart(Iter, NJ, Cnt);
    XT.LockDone();//(p, eFITEvalDerivs, FSW_EvalDerivs);
    p->EvalDerivs(NJ);
    XT.ExecDone();//Postamble(p, eFITEvalDerivs, FSW_EvalDerivs);
    ProcLoopEnd(Iter, NJ, Cnt);
    XT.EndLoop();//PostambleUnLock();

    #if dbgFlwSolve
    if (dbgThreadsSleep())
      Sleep(0);
    #endif
    }
  XT.Complete();//PostambleTl();

  SHOWPROGRESS(1, "EvalDerivs 2");
  }

//---------------------------------------------------------------------------

void FlwSlvDyn::FITEvalIntegral(CLkNodeEvalIter & Iter)
  {
  STARTPROGRESS;
  SHOWPROGRESS(10, "EvalIntegral 1");

  CNodeEvalIndex NJ;
  int Cnt;
  CThreadTimer  XT(eFITEvalIntegral, FSW_EvalIntegral);
  while (Iter.RemoveNext(NJ, &Cnt))
    {
    FlwNode * p=&NJ.Nd;
    XT.StartLoop(p);
    //p->LockFlanges();
    ProcLoopStart(Iter, NJ, Cnt);
    XT.LockDone();//(p, eFITEvalIntegral, FSW_EvalIntegral);
    p->EvalIntegral(NJ);
    XT.ExecDone();//Postamble(p, eFITEvalIntegral, FSW_EvalIntegral);
    ProcLoopEnd(Iter, NJ, Cnt);
    XT.EndLoop();//PostambleUnLock();
    #if dbgFlwSolve
    if (dbgThreadsSleep())
      Sleep(0);
    #endif
    }
  XT.Complete();//PostambleTl();

  SHOWPROGRESS(1, "EvalIntegral 2");
  }

//---------------------------------------------------------------------------

void FlwSlvDyn::ODEDerivs()
  {
  CStopWatchLap SWLapTotalDerivative(gs_swTotalDerivative);

#if dbgDerivatives
  dbgpln("Derivs %s  %10.4f %10.4f",IntCB.Desc,IntCB.Time,IntCB.dTime);
#endif

  CFlwNodeIter Ctrl(m_PwrCtrlNds);
  for (FlwNode * p = Ctrl.First(); (p); p = Ctrl.Next())
    if (p->NetDynamicMethod())
      p->EvalDerivs(CNodeEvalIndex(p,-1,-1,ST_Isolated,false));
  //for (FlwNode * p = Ctrl.First(); (p); p = Ctrl.Next())
  //  if (p->NetDynamicMethod())
  //    p->PostEvalDerivs();

  if (FlwNets.ProblemOK())
    {
    if (1) // Solve the Network
      {
      CStopWatchLap SWLapNetworkSolve(gs_swNetworkSolve);

      if (FSTC.NThreads())
        {
        //CLkFlwNodeIter TCAll(&m_AllNds);
        FSTC.DoJob(eFITFlowInfo, this, &m_AllNdsLA);//, &TCAll);
        }
      else
        {
        CFlwNodeIter All(m_AllNds);
        CThreadTimer  XT(eFITFlowInfo, FSW_Null);
        for (FlwNode * p = All.First(); (p); p = All.Next())
          {
          XT.StartLoop(p);
          XT.LockDone();//(p, eFITFlowInfo, FSW_Null);
          p->EvalFlowInfoStart();
          p->EvalJoinPressures(-1);
          if (p->fEvalPressSensReqd)
            p->EvalPressureSens();
          XT.ExecDone();//Postamble(p, eFITFlowInfo, FSW_Null);
          XT.EndLoop();
          }
        XT.Complete();//PostambleTl();
        }

      if (ICStepReStart())
        FlwNets.RestoreQmEst();
      else if (ICStepStart())
        {
        FlwNets.SaveQmEst();
        FlwNets.SaveQmGood();
        FlwNets.SavePGood();
        }
      FlwNets.SetQmEstAndQm();

      FlwNets.Solve(dwIterNo);
      }

    if (1) // Solve the Models
      {
      CStopWatchLap SWLapModelSolve(gs_swModelSolve);

      CFlwNodeIter All(m_AllNds);
      for (p=All.First(); p; p=All.Next())
        p->SetIntegralDone(false);

      FindTOSequence(FTS_DynMode|(FSTC.NThreads() ? FTS_ThrdSeq : 0), TOSL_Proc);

#if (dbgFlwSolve)
      if (dbgShowFlows())
        ShowFlows(ProcTOSequence, "After FindTOSequence");
#endif

      RestartAllTears(EqnCB());

      Strng_List BadTearTags, BadTearInfo;
      const int MaxTearLoop=EqnCB().Cfg.iMaxIters;
      for (int TearLoop=0; ; )
        {
        #if (dbgFlwSolve)
        if (dbgTearCnvgLoop())
          {
          char buff[100];
          memset(buff, TearLoop==0?'=':'-', 89);
          buff[89]=0;
          dbgpln("%s TearCnvgLoop [%3i]", buff, TearLoop);
          }
        #endif

        ODE().SaveEvalProducts(this);

        CNodeEvalIndexIter ProcOrder(ProcTOSequence);
        #if (dbgFlwSolve && WithIOChgMonitor)
        if (dbgDumpChanges())
          {
          for (FlwNode * p=All.First(); (p); p=All.Next())
            p->MonitorPreEvalProducts();
          }
        #endif

        for (CNodeEvalIndex NJ=ProcOrder.First(); ProcOrder.ItemValid(); NJ=ProcOrder.Next())
          {
          NDSTARTSW(NJ.m_pNd, FSW_EvalProductsInit);
          NJ.Nd.EvalProductsInit(EPI_SourceEval);
          NDSTOPSW(NJ.m_pNd, FSW_EvalProductsInit);
          }
        for (NJ=ProcOrder.First(); ProcOrder.ItemValid(); NJ=ProcOrder.Next())
          {
          NDSTARTSW(NJ.m_pNd, FSW_EvalProductsInit);
          NJ.Nd.EvalProductsInit(EPI_FwdEval);
          NDSTOPSW(NJ.m_pNd, FSW_EvalProductsInit);
          }
        for (NJ=ProcOrder.Last(); ProcOrder.ItemValid(); NJ=ProcOrder.Prev())
          {
          NDSTARTSW(NJ.m_pNd, FSW_EvalProductsInit);
          NJ.Nd.EvalProductsInit(EPI_RevEval);
          NDSTOPSW(NJ.m_pNd, FSW_EvalProductsInit);
          }

        if (FSTC.NThreads())
          SolveTOSequenceThrd(eFITEvalProductsSurge, TOSL_Proc);
        else
          {
          CNodeEvalIndexIter ProcOrder(ProcTOSequence);
          //CFlwNodeIter All(m_AllNds);
          CThreadTimer  XT(eFITEvalProductsSurge, FSW_EvalProductsSurge);
          for (NJ=ProcOrder.First(); ProcOrder.ItemValid(); NJ=ProcOrder.Next())
            {
            XT.StartLoop(NJ.m_pNd);
            XT.LockDone();//(p, eFITEvalProductsSurge, FSW_EvalProductsSurge);
            NJ.Nd.EvalProductsSurge(NJ);
            NJ.Nd.fEvalProductsSurgeCalled=true;
            XT.ExecDone();//Postamble(p, eFITEvalProductsSurge, FSW_EvalProductsSurge);
            XT.EndLoop();
            }
          XT.Complete();//PostambleTl();
          }

        if (FSTC.NThreads())
          {
          if (1)
            {
            SolveTOSequenceThrd(eFITEvalProducts, TOSL_Proc);
            }
          else
            {

            for (int i=0; i<50; i++)
              {
              CStopWatch    m_Total;
              m_Total.ReStart();
              SolveTOSequenceThrd(eFITEvalProducts, TOSL_Proc);
              dbgpln("--- %8.3fms", 1000*m_Total.Secs());
              }
            }
          }
        else
          {
          CNodeEvalIndexIter ProcOrder(ProcTOSequence);
          //CFlwNodeIter All(m_AllNds);
          CThreadTimer  XT(eFITEvalProducts, FSW_EvalSteadyState);
          for (NJ=ProcOrder.First(); ProcOrder.ItemValid(); NJ=ProcOrder.Next())
            {
            XT.StartLoop(NJ.m_pNd);
            //if (p->SSMode())
            //  {
            //  XT.LockDone();//(p, eFITNull, FSW_EvalSteadyState);
            //  p->EvalSteadyState();
            //  XT.ExecDone();//Postamble(p, eFITNull, FSW_EvalSteadyState);
            //  }
            XT.LockDone();//(p, eFITEvalProducts, FSW_EvalProducts);
            NJ.Nd.BeforeNode(NPM_BeforeEvalProducts);
            CALC_EVAL_ORDER(NJ)
            NJ.Nd.EvalProducts(NJ);
            NJ.Nd.AfterNode(NPM_AfterEvalProducts);
            NJ.Nd.fEvalProductsCalled=true;
            XT.ExecDone();//Postamble(p, eFITEvalProducts, FSW_EvalProducts);
            XT.EndLoop();
            }
          XT.Complete();//PostambleTl();
          }

        #if (dbgFlwSolve && WithIOChgMonitor)
        if (dbgDumpChanges())
          {
          dbgpln("================================================================================= MonitorPostEvalProducts");
          CNodeEvalIndexIter ProcOrder(ProcTOSequence);
          for (CNodeEvalIndex NJ=ProcOrder.First(); ProcOrder.ItemValid(); NJ=ProcOrder.Next())
            NJ.Nd.MonitorPostEvalProducts();
          dbgpln("================================================================================= ");
          }
        #endif

        #if (dbgFlwSolve)
        if (dbgShowFlows())
          ShowFlows(ProcTOSequence, "After Eval Products");
        #endif

        EqnCB().SetConverged(EqnCB().NBadLimit()==0);
        flag TearsConverged=TestTearConvergence(TearLoop, EqnCB(), BadTearTags, BadTearInfo);

        // If they are then we are Done
        if (TearsConverged)
          break;

        if (++TearLoop>=MaxTearLoop)
          break;

        FlwNets.SetQmEstAndQm();//DynSoln);

        ODE().RestoreEvalProducts(this);
        }

      if (FSTC.NThreads())
        SolveTOSequenceThrd(eFITEvalDerivs, TOSL_All);
      else
        {
        CNodeEvalIndexIter CombOrder(CombTOSequence);
        CThreadTimer  XT(eFITEvalDerivs, FSW_EvalDerivs);
        for (CNodeEvalIndex NJ=CombOrder.First(); CombOrder.ItemValid(); NJ=CombOrder.Next())
          if (NJ.Nd.NetDynamicMethod())
            {
            XT.StartLoop(NJ.m_pNd);
            XT.LockDone();//(p, eFITEvalDerivs, FSW_EvalDerivs);
            NJ.Nd.EvalDerivs(NJ);
            XT.ExecDone();//Postamble(p, eFITEvalDerivs, FSW_EvalDerivs);
            XT.EndLoop();
            }
        XT.Complete();//PostambleTl();
        }

      //if (1)
      //  {
      //  CNodeEvalIndexIter CombOrder(m_AllTOSequence);
      //  CThreadTimer  XT(eFITEvalDerivs, FSW_EvalDerivs);
      //  for (CNodeEvalIndex NJ=CombOrder.First(); CombOrder.ItemValid(); NJ=CombOrder.Next())
      //    if (NJ.Nd.NetDynamicMethod())
      //      NJ.Nd.PostEvalDerivs();
      //  }
      if (TearLoop>=MaxTearLoop && MaxTearLoop>0)
        {
        LogError("Solver", 0, "Max TearConverge Iterations");
        for (Strng *p=BadTearTags.First(); p; p=BadTearInfo.Next())
          LogNote("Solver", 0, "Tear not Converged %s", p->Str());
        int xxx=0;
        }

#if dbgFlwSolve
      if (dbgDumpDerivs())
        {
        CNodeEvalIndexIter CombOrder(CombTOSequence);
        for (CNodeEvalIndex NJ=CombOrder.First(); CombOrder.ItemValid(); NJ=CombOrder.Next())
          if (dbgDumpDerivs(NJ.Nd.FullTag()))
            NJ.Nd.DumpDerivs();
        }
#endif

      }
    }

#if dbgflows
  CFlwNodeIter All(m_AllNds);
  dbgp("Tags:");
  for (p=All.First(); (p); p=All.Next())
    if (p->IsLnk())
      dbgp(" %12.12s",p->FullObjTag());
  dbgpln("");
  dbgp("Flows:");
  for (p=All.First(); (p); p=All.Next())
    if (p->IsLnk())
      dbgp(" %12.7f",p->FBlk()->Qm);
  dbgpln("");
#endif

  //#if dbgDerivatives
  //if (FlwNets)
  //  {
  //  if (dbgf)
  //    FlwNets.DumpFlows(dbgf, IntCB.Time, IntCB.dTime, IntCB.Desc);
  //
  //  for (p=FlwNets.FindTOSequence(); p!=NULL; p=p->NxtDerivEval)
  //    {
  //    if (p->xIsLnk)
  //      {
  //      rFlange Fi = *p->IOFlange(p->IOIn(0) ? 0 : 1);
  //      rFlange Fo = *p->IOFlange(p->IOIn(0) ? 1 : 0);
  //      dbgpln("SL:%14.6g, V:%14.6g | SL:%14.6g, V:%14.6g %s ",Fi.Q.MassFlow(som_SL), Fi.Q.MassFlow(som_Gas),Fi.Q.EnergyFlow(som_SL), Fi.Q.EnergyFlow(som_Gas), p->FullObjTag());
  //      dbgpln("SL:%14.6g, V:%14.6g | SL:%14.6g, V:%14.6g %s ",Fo.Q.MassFlow(som_SL), Fo.Q.MassFlow(som_Gas),Fo.Q.EnergyFlow(som_SL), Fo.Q.EnergyFlow(som_Gas), p->FullObjTag());
  //      }
  //    }
  //  }
  //#endif

  //dbgpln("{{{{{{{ %s <<<<<<<<",Desc_);
  //return 0;
  }

//---------------------------------------------------------------------------

void FlwSlvDyn::FITODEOperate(CFlwNodeList & List, CODEDataBlock & ODE)//CLkFlwNodeIter & Iter)
  {
  FlwNode * p;
  CThreadTimer  XT(eFITODEOperate, FSW_ODEOperate);
  //while (Iter.GetNext(p))
  CFlwNodeIter Iter(List);
  for (p=Iter.First(); p; p=Iter.Next())
    {
    XT.StartLoop(p);
    //p->LockFlanges();
    p->LockNode();
    XT.LockDone();//(p, eFITODEOperate, FSW_ODEOperate);
    p->ODEOperate(ODE);
    XT.ExecDone();//Postamble(p, eFITODEOperate, FSW_ODEOperate);
    //p->UnLockFlanges();
    p->UnLockNode();
    XT.EndLoop();//PostambleUnLock();
    }
  XT.Complete();//PostambleTl();
  }

//---------------------------------------------------------------------------

void FlwSlvDyn::ODEOperate(CODEDataBlock & ODB)
  {
//dbgpln(">>>>>>>>>>> FlwSlvDyn::ODEOperate");
  if (FSTC.NThreads())
    {
    //lwNodeListODE & ListODE(m_AllNdsLA, ODB);
    //FSTC.DoJob(eFITODEOperate, this, &ListODE);
    //CLkFlwNodeIter FNI(&m_AllNds, &ODB);
    FSTC.DoJob(eFITODEOperate, this, &m_AllNdsLA, &ODB);
    }
  else
    {
    CFlwNodeIter All(m_AllNds);
    CThreadTimer  XT(eFITODEOperate, FSW_ODEOperate);
    for (FlwNode * p=All.First(); (p); p=All.Next())
      if (p->NetDynamicMethod())
        {
        XT.StartLoop(p);
        XT.LockDone();//(p, eFITODEOperate, FSW_ODEOperate);
        p->ODEOperate(ODB);
        XT.ExecDone();//Postamble(p, eFITODEOperate, FSW_ODEOperate);
        XT.EndLoop();
        }
    XT.Complete();//PostambleTl();
    }
//dbgpln("<<<<<<<<<<< FlwSlvDyn::ODEOperate");
  }

//---------------------------------------------------------------------------

void FlwSlvDyn::FITDiscrete(CFlwNodeList & List)//CLkFlwNodeIter & Iter)
  {
  FlwNode * p;
  CThreadTimer  XT(eFITEvalDiscrete, FSW_EvalDiscrete);

  CFlwNodeIter Iter(List);
  for (p=Iter.First(); p; p=Iter.Next())
    {
    XT.StartLoop(p);
    //p->LockFlanges();
    p->LockNode();
    XT.LockDone();//(p, eFITEvalDiscrete, FSW_EvalDiscrete);
    p->EvalFlowInfoEnd();
    p->EvalDiscrete();
    for (int i=0; i<p->NoFlwIOs(); i++)
      {
      for (int j=0; j<p->NIOFBs(i); j++)
        p->IOFB(i,j)->EvalDiscrete();
      p->IOFlange(i)->SetUseStatus(p->IOConduit(i)->QMass()>SmallPosFlow);
      }
    XT.ExecDone();//Postamble(p, eFITEvalDiscrete, FSW_EvalDiscrete);
    //p->UnLockFlanges();
    p->UnLockNode();
    XT.EndLoop();//PostambleUnLock();
    }
  XT.Complete();//PostambleTl();
  }

//---------------------------------------------------------------------------

void FlwSlvDyn::FITEvalCtrlActions(CFlwNodeList & List)//CLkFlwNodeIter & Iter)
  {
  FlwNode * p;
  CThreadTimer  XT(eFITEvalCtrlActions, FSW_EvalCtrlActions);
  //while (Iter.GetNext(p))
  CFlwNodeIter Iter(List);
  for (p=Iter.First(); p; p=Iter.Next())
    {
    XT.StartLoop(p);
    p->LockCtrls();
    XT.LockDone();//(p, eFITEvalCtrlActions, FSW_EvalCtrlActions);
    p->BeforeNode();
    p->EvalCtrlActions(CO_All);
    p->AfterNode();
    XT.ExecDone();//Postamble(p, eFITEvalCtrlActions, FSW_EvalCtrlActions);
    p->UnLockCtrls();
    XT.EndLoop();//PostambleUnLock();
    }
  XT.Complete();//PostambleTl();
  }

//---------------------------------------------------------------------------

void FlwSlvDyn::FITEvalCtrlStrategy(CLkFlwNodeIter & Iter)
  {
  FlwNode * p;
  //int Cnt;
  CThreadTimer  XT(eFITEvalCtrlStrategy, FSW_EvalCtrlStrategy);
  while (Iter.RemoveNext(p))
    {
    XT.StartLoop(p);
    CtrlLoopStart(Iter, p, Iter.List().GetCount());
    XT.LockDone();//(p, eFITEvalCtrlStrategy, FSW_EvalCtrlStrategy);
    p->BeforeNode();
    p->EvalCtrlStrategy(CO_All);
    p->AfterNode();
    XT.ExecDone();//Postamble(p, eFITEvalCtrlStrategy, FSW_EvalCtrlStrategy);
    CtrlLoopEnd(Iter, p, Iter.List().GetCount());
    XT.EndLoop();//PostambleUnLock();
    #if dbgFlwSolve
    if (dbgThreadsSleep())
      Sleep(0);
    #endif
    }
  XT.Complete();//PostambleTl();
  }

//--------------------------------------------------------------------------

//bool FlwSlvDyn::EvalAllElectrics(long What)
//  {
//  CFlwNodeIter Ctrl(m_CtrlTOSequence);
//  for (FlwNode * p=Ctrl.First(); (p); p=Ctrl.Next())
//    {
//    XT.StartLoop(p);
//    XT.LockDone();//(p, eFITElectrics, FSW_Null);
//    p->EvalElectrics(What);
//    p->CheckDataRanges();
//    XT.ExecDone();//Postamble(p, eFITElectrics, FSW_Null);
//    XT.EndLoop();
//    }
//  return true;
//  }

//--------------------------------------------------------------------------

//void FlwSlvDyn::EvalAllPowerAvailable()
//  {
//  CFlwNodeIter Ctrl(m_CtrlTOSequence);
//  for (FlwNode * p=Ctrl.First(); (p); p=Ctrl.Next())
//    {
//    XT.StartLoop(p);
//    XT.LockDone();//(p, eFITElectrics, FSW_Null);
//    p->EvalCtrlActions(eScdCtrlTasks Tasks);
//    p->CheckDataRanges();
//    XT.ExecDone();//Postamble(p, eFITElectrics, FSW_Null);
//    XT.EndLoop();
//    }
//  }
//
////--------------------------------------------------------------------------
//
//void FlwSlvDyn::EvalAllPowerRequired()
//  {
//  gs_PwrSupplies.ZeroAll();
//
//  CFlwNodeIter Proc(ProcTOSequence);
//  for (FlwNode * p=Proc.First(); (p); p=Proc.Next())
//    {
//    XT.StartLoop(p);
//    XT.LockDone();//(p, eFITPowerRequired, FSW_Null);
//    p->EvalCtrlStrategy(eScdCtrlTasks Tasks);
//    p->CheckDataRanges();
//    XT.ExecDone();//Postamble(p, eFITPowerRequired, FSW_Null);
//    XT.EndLoop();
//    }
//
//  CFlwNodeIter Ctrl(m_CtrlTOSequence);
//  for (p=Ctrl.Last(); (p); p=Ctrl.Prev())
//    {
//    XT.StartLoop(p);
//    XT.LockDone();//(p, eFITPowerRequired, FSW_Null);
//    p->EvalCtrlStrategy(eScdCtrlTasks Tasks);
//    p->CheckDataRanges();
//    XT.ExecDone();//Postamble(p, eFITPowerRequired, FSW_Null);
//    XT.EndLoop();
//    }
//  }

//---------------------------------------------------------------------------

void FlwSlvDyn::FITEvalStatistics(CFlwNodeList & List)
  {
  FlwNode * p;
  CThreadTimer  XT(eFITEvalStatistics, FSW_EvalStatistics);
  CFlwNodeIter Iter(List);
  for (p=Iter.First(); p; p=Iter.Next())
    {
    XT.StartLoop(p);
    p->LockNode();
    XT.LockDone();//(p, eFITEvalStatistics, FSW_EvalStatistics);
    p->EvalStatistics(CO_All);
    XT.ExecDone();//Postamble(p, eFITEvalStatistics, FSW_EvalStatistics);
    p->UnLockNode();
    XT.EndLoop();//PostambleUnLock();

    #if dbgFlwSolve
    if (dbgThreadsSleep())
      Sleep(0);
    #endif
    }
  XT.Complete();//PostambleTl();
  }

//---------------------------------------------------------------------------

void FlwSlvDyn::EvalAllDiscrete()
  {
  CStopWatchLap SWLapDiscrete(gs_swDiscrete);
  if (FSTC.NThreads())
    {
    //CLkFlwNodeIter TCAll(&m_AllNds);
    FSTC.DoJob(eFITEvalDiscrete, this, &m_AllNdsLA);
    }
  else
    {
    CFlwNodeIter All(m_AllNds);
    CThreadTimer  XT(eFITEvalDiscrete, FSW_EvalDiscrete);
    for (FlwNode * p=All.First(); (p); p=All.Next())
      {
      XT.StartLoop(p);
      XT.LockDone();//(p, eFITEvalDiscrete, FSW_EvalDiscrete);
      p->EvalFlowInfoEnd();
      p->EvalDiscrete();
      for (int i=0; i<p->NoFlwIOs(); i++)
        {
        for (int j=0; j<p->NIOFBs(i); j++)
          p->IOFB(i,j)->EvalDiscrete();
        p->IOFlange(i)->SetUseStatus(p->IOConduit(i)->QMass()>SmallPosFlow);
        }
      XT.ExecDone();//Postamble(p, eFITEvalDiscrete, FSW_EvalDiscrete);
      XT.EndLoop();
      }
    XT.Complete();//PostambleTl();
    }

  }

//---------------------------------------------------------------------------

void FlwSlvDyn::FITEvalAudit(CLkNodeEvalIter & Iter)
  {
  CNodeEvalIndex NJ;
  int Cnt;
  int Pass=*(int*)Iter.pOtherData;
  ASSERT(Pass>=0 && Pass<2);
  CThreadTimer  XT(eFITEvalAudit, FSW_EvalAudit);
  while (Iter.RemoveNext(NJ, &Cnt))
    {
    FlwNode * p=&NJ.Nd;
    XT.StartLoop(p);
    ProcLoopStart(Iter, NJ, Cnt);
    XT.LockDone();//(p, eFITEvalAudit, FSW_EvalAudit);
    p->EvalAudit(Pass);
    XT.ExecDone();//Postamble(p, eFITEvalAudit, FSW_EvalAudit);
    ProcLoopEnd(Iter, NJ, Cnt);
    //p->UnLockNode();
    XT.EndLoop();//PostambleUnLock();

    #if dbgFlwSolve
    if (dbgThreadsSleep())
      Sleep(0);
    #endif
    }
  XT.Complete();//PostambleTl();
  }

//---------------------------------------------------------------------------

void FlwSlvDyn::EvalAllBalance()
  {
  CStopWatchLap SWLapDiscrete(gs_swBalance);
  if (1) // must be called for Makeups and Bleeds

    {
    for (int Pass=0; Pass<2; Pass++)
      {
      if (1 && FSTC.NThreads())
        SolveTOSequenceThrd(eFITEvalAudit, TOSL_All, &Pass);
      else
        {
        CNodeEvalIndexIter CombOrder(CombTOSequence);
        CThreadTimer  XT(eFITEvalAudit, FSW_EvalAudit);
        for (CNodeEvalIndex NJ=CombOrder.First(); CombOrder.ItemValid(); NJ=CombOrder.Next())
          {
          XT.StartLoop(NJ.m_pNd);
          XT.LockDone();//(p, eFITEvalDiscrete, FSW_EvalDiscrete);
          NJ.Nd.EvalAudit(Pass);
          XT.ExecDone();//Postamble(p, eFITEvalDiscrete, FSW_EvalDiscrete);
          XT.EndLoop();
          }
        XT.Complete();//PostambleTl();
        }
      }
    }
  }

//---------------------------------------------------------------------------

bool FlwSlvBlk::BuildCtrlNdListReqd()
  {
  CStopWatchLap SWLapCtrlOverHead(gs_swCtrlOverHead);

  bool DoBuild=false;
  CFlwNodeIter All(m_AllNds);
  CThreadTimer  XT(eFITBuildCtrlNdList, FSW_BuildCtrlNdList);
  int N=0;
  for (FlwNode * p=All.First(); (p); p=All.Next(), N++)
    {
    XT.StartLoop(p);
    XT.LockDone();

    bool WasActive=p->XRefListActive();
    if (p->TestXRefListActive()!=WasActive)
      {
      //dbgpln("UpdateXRefListsReqd %4i) %s", N, p->Tag());
      DoBuild=true;
      }

    if (p->UpdateXRefListsReqd())
      {
      //dbgpln("UpdateXRefListsReqd %4i) %s", N, p->Tag());
      DoBuild=true;
      }
    if (p->m_bXRefListsChanged)
      {
      //dbgpln("XRefListsChanged %4i) %s", N, p->Tag());
      DoBuild=true;
      }

    XT.ExecDone();
    XT.EndLoop();
    }
  XT.Complete();
  return DoBuild;
  }

void FlwSlvDyn::EvalAllCtrlInitialise(eScdCtrlTasks Tasks)
  {
  STARTPROGRESS;
  SHOWPROGRESS(10, "EvalAllCtrlInitialise");

  CFlwNodeIter All(m_AllNds);
  CThreadTimer  XT(eFITEvalCtrlInitialise, FSW_EvalCtrlInitialise);
  for (FlwNode * p=All.First(); (p); p=All.Next())
    {
    XT.StartLoop(p);
    XT.LockDone();
    p->BeforeNode();
    p->EvalCtrlInitialise(Tasks);
    p->AfterNode();

    //dbgpln("   %10.1fms %s", SW.Lap()*1e3, p->FullObjTag());

    XT.ExecDone();
    XT.EndLoop();
    }
  XT.Complete();
  SHOWPROGRESS(1, "EvalAllCtrlInitialise");
  }

void FlwSlvDyn::EvalAllCtrlActions(eScdCtrlTasks Tasks)
  {
//dbgpln("FlwSlvDyn::EvalAllCtrlActions()");
  CStopWatchLap SWLapCtrlActions(gs_swCtrlActions);
  if (ALLOWCTRLTHREADING && FSTC.NThreads())
    {
    //CLkFlwNodeIter TCAll(&m_AllNds);
    FSTC.DoJob(eFITEvalCtrlActions, this, &m_AllNdsLA);
    }
  else
    {
    CFlwNodeIter All(m_AllNds);
    CThreadTimer  XT(eFITEvalCtrlActions, FSW_EvalCtrlActions);
    for (FlwNode * p=All.First(); (p); p=All.Next())
      {
      XT.StartLoop(p);
      XT.LockDone();//(p, eFITEvalCtrlActions, FSW_EvalCtrlActions);
      p->BeforeNode();
      p->EvalCtrlActions(Tasks);
      p->AfterNode();
      XT.ExecDone();//Postamble(p, eFITEvalCtrlActions, FSW_EvalCtrlActions);
      XT.EndLoop();
      }
    XT.Complete();//PostambleTl();
    }
  }

void FlwSlvDyn::EvalAllCtrlStrategy(eScdCtrlTasks Tasks)
  {
  CStopWatchLap SWLapCtrlStrategy(gs_swCtrlStrategy);
  if (ALLOWCTRLTHREADING && FSTC.NThreads())
    {
    SolveTOSequenceThrd(eFITEvalCtrlStrategy, TOSL_Ctrl);
    }
  else
    {
    CNodeEvalIndexIter Ctrl(CtrlTOSequence);
    CThreadTimer  XT(eFITEvalCtrlStrategy, FSW_EvalCtrlStrategy);
    for (CNodeEvalIndex NJ=Ctrl.First(); Ctrl.ItemValid(); NJ=Ctrl.Next())
      {
      XT.StartLoop(NJ.m_pNd);
      XT.LockDone();//(p, eFITEvalCtrlStrategy, FSW_EvalCtrlStrategy);
      NJ.Nd.BeforeNode();
      NJ.Nd.EvalCtrlStrategy(Tasks);
      NJ.Nd.AfterNode();
      XT.ExecDone();//Postamble(p, eFITEvalCtrlStrategy, FSW_EvalCtrlStrategy);
      XT.EndLoop();
      MYASSERT(_CrtCheckMemory());
      }
    XT.Complete();//PostambleTl();
    }
  }

void FlwSlvDyn::EvalAllCtrlTerminate(eScdCtrlTasks Tasks)
  {
  //gs_swCtrlTerminate.Start();
  CFlwNodeIter All(m_AllNds);
  CThreadTimer  XT(eFITEvalCtrlTerminate, FSW_EvalCtrlTerminate);
  for (FlwNode * p=All.First(); (p); p=All.Next())
    {
    XT.StartLoop(p);
    XT.LockDone();//(p, eFITEvalCtrlTerminate, FSW_EvalCtrlTerminate);
    p->BeforeNode();
    p->EvalCtrlTerminate(Tasks);
    p->AfterNode();
    XT.ExecDone();//Postamble(p, eFITEvalCtrlTerminate, FSW_EvalCtrlTerminate);
    XT.EndLoop();
    }
  //gs_swCtrlTerminate.Stop();
  XT.Complete();//PostambleTl();
  }

void FlwSlvDyn::EvalAllStatistics(eScdCtrlTasks Tasks)
  {
  //gs_swStatistics.Start();
  //if (0) // temporary disable FSTC.NThreads())
  if (FSTC.NThreads())
    {
    FSTC.DoJob(eFITEvalStatistics, this, &m_AllNdsLA);//, &TCAll);
   //SolveTOSequenceThrd(eFITEvalStatistics, TOSL_All);
    }
  else
    {
    CNodeEvalIndexIter CombOrder(CombTOSequence);
    CThreadTimer  XT(eFITEvalStatistics, FSW_EvalStatistics);
    for (CNodeEvalIndex NJ=CombOrder.First(); CombOrder.ItemValid(); NJ=CombOrder.Next())
      {
      XT.StartLoop(NJ.m_pNd);
      XT.LockDone();//(p, eFITEvalStatistics, FSW_EvalStatistics);
      NJ.Nd.EvalStatistics(Tasks);
      XT.ExecDone();//Postamble(p, eFITEvalStatistics, FSW_EvalStatistics);
      XT.EndLoop();
      }
    XT.Complete();//PostambleTl();
    }
  //gs_swStatistics.Stop();
  }

//--------------------------------------------------------------------------

flag FlwSlvDyn::QueryTime(CXM_TimeControl &CB, CTimeValue &TimeRqd, CTimeValue &dTimeRqd)
  {
  // CNM
  double StartScale=1.0; //0.5;
  IntCB.SetTimeInc(Max(IntCB.GetTimeInc()*StartScale, IntCB.GetTimeIncMnRqd()));

  if (!Valid(IntCB.GetTimeInc()))
    IntCB.SetTimeInc(1.0);

//  if (ODE().StepSizeControl()==ODE_SSC_Fixed)
//    dTimeRqd=Min(CB.dTimeMax, IntCB.GetTimeIncMxRqd());
//  else
//    dTimeRqd=IntCB.GetTimeInc();
  TimeRqd.Seconds=IntCB.GetTime();

  //CheckFlwSolnSettings(true);
  return true;
  }

//--------------------------------------------------------------------------

flag FlwSlvDyn::Start(CXM_TimeControl &CB, CFlwNodeList &AllNdsIn)
  {
#if TimingLaps
  s_swTiming.ReStart();
  TIMINGLAP("FLWSHEET START");
#endif
  STARTPROGRESS;
  SHOWPROGRESS(10, "DynStart 1")

  gs_ThreadTiming.Initialise(FlwSlvNd.NRqdThreads());
  FSTC.CreateThreads(FlwSlvNd.NRqdThreads());

  ODE().Initialise();
  m_AllNds.RemoveAllToFreeList();

  CFlwNodeIter AllIn(AllNdsIn);
  for (FlwNode * p=AllIn.First(); (p); p=AllIn.Next())
    {
    m_AllNds.AddTail(p);
#if TimingStats
    if (1 || dbgNdStats())
      {
      NDADDSW(p)
      // A TEMPORARY FIX -- bRunning not inited in a Stopwatch
      //for (int i=0; i<FSW_Max; i++)
      //  p->SW(i).Clear();
      }
#endif
    }
  FixAllNdsLists();
  // ?? BUG m_AllNds.AddHead(pAllNdsIn);

  SHOWPROGRESS(1, "DynStart 2");

  CFlwNodeIter All(m_AllNds);
  for (p=All.First(); (p); p=All.Next())
    p->NodeFlwTask(NFT_DynInit);

  if (CB.m_ScnState==Scn_Start)
    {
    SHOWPROGRESS(1, "DynStart 2.1");
    if (gs_Exec.m_RestartEmptySpills)
      {
      Strng_List RqdTags;
      gs_Exec.SetModelState(MSA_EmptySpillTargets, RqdTags);
      }
    if (gs_Exec.m_RestartEmptyAll)
      {
      Strng_List RqdTags;
      gs_Exec.SetModelState(MSA_Empty, RqdTags);
      }

    }

  SHOWPROGRESS(1, "DynStart 3");

#if dbgRunControl
  dbgpln("RunCon:Start  :%s %s",Running?"Running":"", WasStopped?"WasStopped":"");
#endif
  if (m_AllNds.GetCount()==0)
    {
    LogNote("Solver", 0, "No Dynamic Models");
    //LogError("Solver", LF_Exclamation, "No Dynamic Models");
    //return 0;
    }

  SHOWPROGRESS(1, "DynStart 4");

  m_SSSteady=0;
  m_SSIters=0;

  if (PreStartCheckAll())
    {
    SHOWPROGRESS(1, "DynStart 5");

    TIMINGLAP("PreStart");
    EvalAllStates();
    TIMINGLAP("EvalAllStates");

    SHOWPROGRESS(1, "DynStart 6");

    if (!CreateSolutionBlks(FlwSlvNd.NRqdThreads()))
      {
      LogWarning("Solver", 0, "Cannot Create Solution Blocks");
      return 2;
      }
    TIMINGLAP("CreateSolution");

    SHOWPROGRESS(1, "DynStart 7");

    if (1)
      {
      EvalAllElectrics(false); // this is here to create connections etc and termstrip etc.
      SHOWPROGRESS(1, "DynStart 7.1");
      
      if (CB.m_ScnState==Scn_Start)
        {
        eScdCtrlTasks Tasks=eScdCtrlTasks(0);
        #if WithRestartPGM
        if (CB.m_RestartPGMOnStart)
          Tasks=eScdCtrlTasks(Tasks|CO_InitPGM);
        #endif
        if (CB.m_RestartPrfOnStart)
          Tasks=eScdCtrlTasks(Tasks|CO_InitPrf);
        EvalAllCtrlInitialise(Tasks);
        SHOWPROGRESS(1, "DynStart 7.2");

        if (gs_Exec.m_RestartResetStats)
          {
          Strng_List SL;
          gs_Exec.SetModelState(MSA_DynStatsRunInit, SL);
          }
        }

      if (BuildCtrlNdListReqd())// || true)
        {
        SHOWPROGRESS(1, "DynStart 7.3");
        CStopWatchLap SWLapCtrlOverHead(gs_swCtrlOverHead);
        BuildCtrlNdList();
        SHOWPROGRESS(1, "DynStart 7.4");
        FindTOSequence(FTS_DynMode|(FSTC.NThreads() ? FTS_ThrdSeq : 0), TOSL_Ctrl);
        SHOWPROGRESS(1, "DynStart 7.5");
        }

      // force rebuild next time;
      m_fCtrlTOSeqValid=0;
      }

    SHOWPROGRESS(1, "DynStart 8");

    ConfigureJoinsAll();
    TIMINGLAP("ConfigJoins");

    SHOWPROGRESS(1, "DynStart 9");

    if (!FlwNets.Initialise())
      {
      LogWarning("Solver", 0, "Cannot Initialise FlwNets");
      return 3;
      }

    SHOWPROGRESS(1, "DynStart A");

    TIMINGLAP("Initialise");
    StartSolutionAll();
    TIMINGLAP("Start Solution");

    SHOWPROGRESS(1, "DynStart B");

    ODE().StartUp(this);
    TIMINGLAP("Started");

    SHOWPROGRESS(1, "DynStart C");
    }
  else
    {
    LogWarning("Solver", 0, "Problem with PreStart Checks");
    return 1;
    }

  return 0;
  }

//--------------------------------------------------------------------------

flag FlwSlvDyn::ReStart()
  {

  if (!CreateSolutionBlks(FlwSlvNd.NRqdThreads()))
    {
    LogWarning("Solver", 0, "Cannot Create Solution Blocks\n");
    return 2;
    }

  ConfigureJoinsAll();

  if (!FlwNets.Initialise(/*Lnk_Eps_A, Lnk_Eps_R, Net_DeRating*/))
    {
    LogWarning("Solver", 0, "Cannot Initialise FlwNets\n");
    return 3;
    }

  return 0;
  }

//--------------------------------------------------------------------------

flag FlwSlvDyn::Execute(CXM_TimeControl &CB, CEOExecReturn &EORet)
  {
  #if (dbgFlwSolveTiming)
  gs_ExecIterTime.Clear();
  gs_ExecIterTime.Start();
  #endif
  #if dbgFlwSolve
  static CStopWatch SWTime;
  static double SWTime0=-1;
  if (SWTime0<0)
    SWTime0=SWTime.Time();
  if (dbgThreadsTiming())
    dbgpln("Threads - Exec Start %10.3f %s", SWTime.Time()-SWTime0, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
  SWTime0=SWTime.Time();
  #endif
  TIMINGLAP("Execute");
  
  STARTPROGRESS;
  SHOWPROGRESS(10, "Execute 1");

  gs_ThreadTiming.Reset();

  IntCB.SetTime(CB.m_TheTime.Seconds);
  long NIters;

  double TimeInc;
  if (ODE().StepSizeControl()==ODE_SSC_Fixed)
    {
    TimeInc=Min(CB.m_StepSize.Seconds, IntCB.GetTimeIncMxRqd());
    NIters=long(0.999+CB.m_StepSize.Seconds/GTZ(TimeInc));
    TimeInc=CB.m_StepSize.Seconds/NIters;
    IntCB.SetTimeInc(TimeInc);
    }
  else
    {
    NIters=100;
    TimeInc=IntCB.GetTimeIncMx();
    }
  //StopTime=CB.m_TimeToStop.Seconds;
  dwIterNo++;

  CFlwNodeIter All(m_AllNds);
  for (FlwNode * p=All.First(); (p); p=All.Next())
    {
    p->fEvalProductsSurgeCalled=false;
    p->fEvalProductsCalled=false;
    for (int i=0; i<p->NoFlwIOs(); i++)
      p->IOConduit(i)->ClrModified();
    
    }

  SHOWPROGRESS(1, "Execute 2");

  MarkClosureDataInvalid(m_AllNds);

  //dbgpln("Clear All XRef Cnts ==============");
  if (1)
    {
    CStopWatchLap SWLapCtrlOverHead(gs_swCtrlOverHead);
    FlwSlvNd.ClearRefCnts();
    }

  if (m_SSTest)
    {
    m_SSSteady++;
    m_SSIters++;
    }
  else
    {
    m_SSSteady    = 0;
    m_SSIters     = 0;
    }
  m_SSMaxChange = 0;
  m_SSMaxTag    = "";

  SHOWPROGRESS(1, "Execute 3");
  
  bool DoClosureTests=(gs_ClosureOption!=CloseTest_Off);
  bool StopRequired=false;
  for ( ;NIters>0 && !StopRequired;NIters--)
    {

    CheckStopConditions(CB);

    if (m_SSTest)
      {
      for (FlwNode * p=All.First(); (p); p=All.Next())
        p->SteadyStateInit(m_SSTest);
      }

    double MdlMaxTimeInc=1e10;
    for (FlwNode * p=All.First(); (p); p=All.Next())
      MdlMaxTimeInc=Min(MdlMaxTimeInc, p->GetMaxTimeInc());

    double TimeInc4Step=Min(TimeInc, MdlMaxTimeInc);
    IntCB.SetTimeInc(TimeInc4Step);

    SHOWPROGRESS(1, "Execute 3");

    MYASSERT(_CrtCheckMemory());
    EvalAllCtrlActions(CO_All);
    MYASSERT(_CrtCheckMemory());
    //EvalAllPowerAvailable();
    EvalAllElectrics(false);
    MYASSERT(_CrtCheckMemory());

    SHOWPROGRESS(1, "Execute 4");

    if (m_EvalCtrlAt==ECA_Start)
      {
      if (BuildCtrlNdListReqd())
        {
        CStopWatchLap SWLapCtrlOverHead(gs_swCtrlOverHead);
        BuildCtrlNdList();
        FindTOSequence(FTS_DynMode|(FSTC.NThreads() ? FTS_ThrdSeq : 0), TOSL_Ctrl);
        }
      EvalAllCtrlStrategy(CO_All);
      }
    MYASSERT(_CrtCheckMemory());

    SHOWPROGRESS(1, "Execute 5");

    ConfigureJoinsAll();
    TIMINGLAP("ConfigJoins");
    MYASSERT(_CrtCheckMemory());

    // Moved to ensure that JoinInfo is OK
    if (DoClosureTests)
      InitClosure();

    SHOWPROGRESS(1, "Execute 6");

    if (CB.m_StepSize>0.0)
      {
      ODE().Integrate(this, IntCB.GetTime()+TimeInc4Step, TimeInc4Step, true, CB.m_bHoldAdv && !CB.m_bSyncWithClock);
      SHOWPROGRESS(1, "Execute 6.1");
      FlwNets.CheckForErrors();
      SHOWPROGRESS(1, "Execute 7");
      TIMINGLAP("Integrate");
      TransferAllTears(EqnCB());
      SHOWPROGRESS(1, "Execute 8");
      TIMINGLAP("Tears");
      EvalAllDiscrete();
      SHOWPROGRESS(1, "Execute 9");
      EvalAllBalance();
      SHOWPROGRESS(1, "Execute A");
      EvalAllPowerRequired();
      SHOWPROGRESS(1, "Execute B");
      EvalAllElectrics(true);
      SHOWPROGRESS(1, "Execute C");
      TIMINGLAP("Discrete");
      CtrlSeq().StepSequencer();
      SHOWPROGRESS(1, "Execute D");
      }
    else
      {
      double dTMem=IntCB.GetTimeInc();
      IntCB.SetTimeInc(0.0);
      EvalAllDiscrete();
      EvalAllBalance();
      EvalAllPowerRequired();
      EvalAllElectrics(true);
      TIMINGLAP("Discrete");
      CtrlSeq().StepSequencer();
      IntCB.SetTimeInc(dTMem);
      }


    MYASSERT(_CrtCheckMemory());
    EORet.m_Time.Seconds=IntCB.GetTime();
    //dTimeNext=IntCB.GetTimeIncNext();
    EvalAllStates();
    MYASSERT(_CrtCheckMemory());
    SHOWPROGRESS(1, "Execute E");

    EvalAllStatistics(CO_All);
    MYASSERT(_CrtCheckMemory());

    SHOWPROGRESS(1, "Execute F");

    if (m_EvalCtrlAt==ECA_End)
      {
      if (BuildCtrlNdListReqd())
        {
        CStopWatchLap SWLapCtrlOverHead(gs_swCtrlOverHead);
        MYASSERT(_CrtCheckMemory());
        SHOWPROGRESS(1, "Execute G.1");
        BuildCtrlNdList();
        SHOWPROGRESS(1, "Execute G.2");
        
        MYASSERT(_CrtCheckMemory());
        FindTOSequence(FTS_DynMode|(FSTC.NThreads() ? FTS_ThrdSeq : 0), TOSL_Ctrl);
        SHOWPROGRESS(1, "Execute G.3");
        
        }
      MYASSERT(_CrtCheckMemory());
      EvalAllCtrlStrategy(CO_All);
      }
    MYASSERT(_CrtCheckMemory());
    SHOWPROGRESS(1, "Execute G");

    MarkClosureDataAvail(ProcTOSequence);
    if (DoClosureTests)
      TestClosure(ProcTOSequence);

    if (m_SSTest)
      {
      for (p=All.First(); (p); p=All.Next())
        {
        if (!p->SteadyStateTest(m_SSTest, m_SSPressTol, m_SSQmTol, m_SSMaxChange, m_SSMaxTag))
          m_SSSteady=0;
        }
      }

    SHOWPROGRESS(1, "Execute I");

    if (CheckStopRequired(CB, false, false))
      StopRequired=true;
    }
  MYASSERT(_CrtCheckMemory());

  if (NIters<0 && ODE().StepSizeControl()!=ODE_SSC_Fixed)
    {
    LogError("SOlver", 0, "Too many Iterations in Step");
    }

  SHOWPROGRESS(1, "Execute J");

  for (p=All.First(); (p); p=All.Next())
    {
    if (p->IsProcessSeq())
      {
      if (!p->fEvalProductsSurgeCalled)
        {int xxx=0;};//  dbgpln("!EvalProductsSurgeCalled %s", p->FullObjTag());
      if (!p->fEvalProductsCalled)
        {int xxx=0;};//  dbgpln("!EvalProductsCalled %s", p->FullObjTag());
      p->SetCI(23, !p->fEvalProductsSurgeCalled);
      p->SetCI(24, !p->fEvalProductsCalled);
      }
    }

#if TimingStats
  if (dbgNdStats() && dbgNdStatsIter())
    {
    DumpNdStats(NDS_LapTime);
    DumpNdStats(NDS_LapCount);
    }
#endif

  gs_ThreadTiming.Dump();

  #if dbgFlwSolve
  if (dbgThreadsTiming())
    dbgpln("Threads - Exec Done  %10.3f %s", SWTime.Time()-SWTime0, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
  SWTime0=SWTime.Time();
  #endif

  SHOWPROGRESS(1, "Execute K");
  return true;
  }

//--------------------------------------------------------------------------

void FlwSlvDyn::CheckStopConditions(CXM_TimeControl &CB)
  {
  };

//--------------------------------------------------------------------------

flag FlwSlvDyn::CheckStopRequired(CXM_TimeControl &CB, bool Starting, bool ShowMsgs)
  {
  LPCTSTR Msg = Starting ? "Start prevented" : "Stopping";

  //if (FlwSlvNd.fStopOnError && FlwNets.StopReqd())
  //  {
  //  LogError("Solver", LF_Hand, "Stopping : Errors Exist");
  //  return true;
  //  }
  //else
  if (gs_Exec.SteadyStateRqd() || (CB.m_ScnType==ScnType_SteadyState))
    {
    if (m_SSSteady>=m_SSStopCount)
      {
      CB.m_ScnState=Scn_Complete;
      if (ShowMsgs)
        LogNote("Solver", 0, "%s : Reached Steady State", Msg);
      return true;
      }
    else if (m_SSIters>=m_SSMaxIters)
      {
      CB.m_ScnState=Scn_Complete;
      if (ShowMsgs)
        LogWarning("Solver", 0, "%s : Reached Max Iterations", Msg);
      return true;
      }
    }
  else if (CB.m_ScnType==ScnTyp_Steps && CB.m_StepCount>=CB.m_StepCountMax)
    {
    CB.m_ScnState=Scn_Complete;
    if (ShowMsgs)
      LogNote("Solver", 0, "%s : Reached Max Iterations", Msg);
    return true;
    }
  else if (CB.m_ScnType==ScnTyp_Duration && CB.m_TheTime>=CB.m_TimeToStop)
    {
    CB.m_ScnState=Scn_Complete;
    if (ShowMsgs)
      LogNote("Solver", 0, "%s : Reached Duration", Msg);
    return true;
    }
  else if (CB.m_ScnType==ScnTyp_StopTime && CB.m_TheTime>=CB.m_TimeToStop)
    {
    CB.m_ScnState=Scn_Complete;
    if (ShowMsgs)
      {
      if (CB.m_TheTime>CB.m_TimeToStop)
        LogWarning("Solver", 0, "%s : Passed StopTime", Msg);
      else
        LogNote("Solver", 0, "%s : Reached StopTime", Msg);
      }
    return true;
    }
  return false;
  }

//--------------------------------------------------------------------------

flag FlwSlvDyn::Stop(CXM_TimeControl &CB)
  {
  if (1)
    {
    if (BuildCtrlNdListReqd())
      {
      CStopWatchLap SWLapCtrlOverHead(gs_swCtrlOverHead);
      //BuildCtrlNdListReqd();
      BuildCtrlNdList();
      FindTOSequence(FTS_DynMode|(FSTC.NThreads() ? FTS_ThrdSeq : 0), TOSL_Ctrl);
      }

    if (CB.m_ScnState==Scn_Complete)
      {
      EvalAllCtrlTerminate(CO_All);
      }
    // force rebuild next time;
    m_fCtrlTOSeqValid=0;
    }

  if (CB.m_StepCount>0)
    FlwNets.CheckForErrors();

  CFlwNodeIter All(m_AllNds);
  for (FlwNode * p=All.First(); (p); p=All.Next())
    p->NodeFlwTask(NFT_DynTerm);

  ClearSolutionBlks(false);

  FSTC.CloseThreads();


#if TimingStats
  if (dbgRunStats())
    DumpRunStats(true);

  if (dbgNdStats())
    {
    DumpNdStats(NDS_TotalTime);
    DumpNdStats(NDS_TotalCounts);
    }

  //for (p=All.First(); (p); p=All.Next())
  //  NDCLRSW(p);
#endif

  return true;
  }

//--------------------------------------------------------------------------

void FlwSlvDyn::DumpRunStats(flag Total)
  {
#if NDSTOPWATCHES
  dbgpln("Timing - Seconds");
  dbgpln("       Overall        :  %10.3f ",gs_swTotalExecTicks.Secs());
  //dbgpln("       CnvState       :  %10.3f  %10.3f%%", gs_swTotalCnvStateTicks.Time(),gs_swTotalCnvStateTicks.Secs()/GTZ(gs_swTotalExecTicks.Secs())*100.0);
  dbgpln("       Derivs         :  %10.3f  %10.3f%%", gs_swTotalDerivative      .Secs(),gs_swTotalDerivative    .Secs()/GTZ(gs_swTotalExecTicks.Secs())*100.0);
  dbgpln("         SolveNet     :  %10.3f  %10.3f%%", gs_swNetworkSolve         .Secs(), gs_swNetworkSolve      .Secs()/GTZ(gs_swTotalExecTicks.Secs())*100.0);
  if (FlwNets.ProblemOK())
    {
    dbgpln("           Converge   :  %10.3f  %10.3f%%", gs_swConvergeTicks      .Secs(), gs_swConvergeTicks     .Secs()/GTZ(gs_swTotalExecTicks.Secs())*100.0);
    dbgpln("           Measure    :  %10.3f  %10.3f%%", gs_swMeasureTicks       .Secs(), gs_swMeasureTicks      .Secs()/GTZ(gs_swTotalExecTicks.Secs())*100.0);
    dbgpln("           Solve      :  %10.3f  %10.3f%%", gs_swSolveTicks         .Secs(), gs_swSolveTicks        .Secs()/GTZ(gs_swTotalExecTicks.Secs())*100.0);
    dbgpln("           LoadSurge  :  %10.3f  %10.3f%%", gs_swLoadSurgeTicks     .Secs(), gs_swLoadSurgeTicks    .Secs()/GTZ(gs_swTotalExecTicks.Secs())*100.0);
    dbgpln("           LoadSurge  :  %10.3f  %10.3f%%", gs_swLoadSurgeTicks     .Secs(), gs_swLoadSurgeTicks    .Secs()/GTZ(gs_swTotalExecTicks.Secs())*100.0);
    dbgpln("           CalcIndices:  %10.3f  %10.3f%%", gs_swCalcIndicesTicks   .Secs(), gs_swCalcIndicesTicks  .Secs()/GTZ(gs_swTotalExecTicks.Secs())*100.0);
    dbgpln("           MtxBuild   :  %10.3f  %10.3f%%", gs_swMtxBuildTicks      .Secs(), gs_swMtxBuildTicks     .Secs()/GTZ(gs_swTotalExecTicks.Secs())*100.0);
    dbgpln("           MtxSolve   :  %10.3f  %10.3f%%", gs_swMtxSolveTicks      .Secs(), gs_swMtxSolveTicks     .Secs()/GTZ(gs_swTotalExecTicks.Secs())*100.0);
    dbgpln("           MtxExtract :  %10.3f  %10.3f%%", gs_swMtxExtractTicks    .Secs(), gs_swMtxExtractTicks   .Secs()/GTZ(gs_swTotalExecTicks.Secs())*100.0);
    dbgpln("           ChkJoins   :  %10.3f  %10.3f%%", gs_swChkJoinsTicks      .Secs(), gs_swChkJoinsTicks     .Secs()/GTZ(gs_swTotalExecTicks.Secs())*100.0);
    dbgpln("           EvalJoinQ  :  %10.3f  %10.3f%%", gs_swEvalJoinQTicks     .Secs(), gs_swEvalJoinQTicks    .Secs()/GTZ(gs_swTotalExecTicks.Secs())*100.0);
    dbgpln("           EvalJoinP  :  %10.3f  %10.3f%%", gs_swEvalJoinPTicks     .Secs(), gs_swEvalJoinPTicks    .Secs()/GTZ(gs_swTotalExecTicks.Secs())*100.0);
    dbgpln("           FixedFlwQ  :  %10.3f  %10.3f%%", gs_swSolveFxdFlwQTicks  .Secs(), gs_swSolveFxdFlwQTicks .Secs()/GTZ(gs_swTotalExecTicks.Secs())*100.0);
    dbgpln("           FixedFlwP  :  %10.3f  %10.3f%%", gs_swSolveFxdFlwPTicks  .Secs(), gs_swSolveFxdFlwPTicks .Secs()/GTZ(gs_swTotalExecTicks.Secs())*100.0);
    dbgpln("           FixTopo    :  %10.3f  %10.3f%%", gs_swFixTopoTicks       .Secs(), gs_swFixTopoTicks      .Secs()/GTZ(gs_swTotalExecTicks.Secs())*100.0);
    dbgpln("           OverHead   :  %10.3f  %10.3f%%", gs_swOverHeadTicks      .Secs(), gs_swOverHeadTicks     .Secs()/GTZ(gs_swTotalExecTicks.Secs())*100.0);
    }
  dbgpln("         SolveModels  :  %10.3f  %10.3f%%", gs_swModelSolve         .Secs(), gs_swModelSolve        .Secs()/GTZ(gs_swTotalExecTicks.Secs())*100.0);
  //dbgpln("       FlwsOut        :  %10.3f  %10.3f%%", DerivFlwsOutTicks        .Secs(), DerivFlwsOutTicks      .Secs()/GTZ(gs_swTotalExecTicks.Secs())*100.0);
  //dbgpln("       Derivs       :  %10.3f  %10.3f%%", DerivDerivsTicks          .Secs(), DerivDerivsTicks       .Secs()/GTZ(gs_swTotalExecTicks.Secs())*100.0);

  dbgpln("       CtrlOverHead   :  %10.3f  %10.3f%%", gs_swCtrlOverHead       .Secs(), gs_swCtrlOverHead      .Secs()/GTZ(gs_swTotalExecTicks.Secs())*100.0);
  dbgpln("       PowerRequired  :  %10.3f  %10.3f%%", gs_swPowerRequired      .Secs(), gs_swPowerRequired     .Secs()/GTZ(gs_swTotalExecTicks.Secs())*100.0);
  dbgpln("       Electrics      :  %10.3f  %10.3f%%", gs_swElectrics          .Secs(), gs_swElectrics         .Secs()/GTZ(gs_swTotalExecTicks.Secs())*100.0);
  dbgpln("       CtrlActions    :  %10.3f  %10.3f%%", gs_swCtrlActions        .Secs(), gs_swCtrlActions       .Secs()/GTZ(gs_swTotalExecTicks.Secs())*100.0);
  dbgpln("       CtrlStrategy   :  %10.3f  %10.3f%%", gs_swCtrlStrategy       .Secs(), gs_swCtrlStrategy      .Secs()/GTZ(gs_swTotalExecTicks.Secs())*100.0);
  dbgpln("       ConfigJoins    :  %10.3f  %10.3f%%", gs_swConfigJoins        .Secs(), gs_swConfigJoins       .Secs()/GTZ(gs_swTotalExecTicks.Secs())*100.0);
  dbgpln("       NetworkSolve   :  %10.3f  %10.3f%%", gs_swNetworkSolve       .Secs(), gs_swNetworkSolve      .Secs()/GTZ(gs_swTotalExecTicks.Secs())*100.0);
  dbgpln("       ModelSolve     :  %10.3f  %10.3f%%", gs_swModelSolve         .Secs(), gs_swModelSolve        .Secs()/GTZ(gs_swTotalExecTicks.Secs())*100.0);
  dbgpln("       Discrete       :  %10.3f  %10.3f%%", gs_swDiscrete           .Secs(), gs_swDiscrete          .Secs()/GTZ(gs_swTotalExecTicks.Secs())*100.0);
  dbgpln("       Balance        :  %10.3f  %10.3f%%", gs_swBalance            .Secs(), gs_swBalance           .Secs()/GTZ(gs_swTotalExecTicks.Secs())*100.0);
  dbgpln("       ChangeTrace    :  %10.3f  %10.3f%%", gs_swChangeTrace        .Secs(), gs_swChangeTrace       .Secs()/GTZ(gs_swTotalExecTicks.Secs())*100.0);

#endif
  }

//--------------------------------------------------------------------------

void FlwSlvDyn::DumpNdStats(int What)
  {
  int Selection[]=
    {
    FSW_ConfigureJoins,
    FSW_EvalJoinPressures,
    FSW_EvalFlowEquations,
    FSW_EvalProducts,
    FSW_EvalProductsSurge,
    FSW_EvalDerivs,
    FSW_ODEOperate,
    FSW_EvalSteadyState,
    FSW_EvalCtrlStrategy,
    FSW_EvalDiscrete,
    -1
    };
#if NDSTOPWATCHES
  dbgp("StopWatches - ");
  switch (What)
    {
    case NDS_LapTime:       dbgpln(" LapTime:       mSeconds");   break;
    case NDS_LapTimeFrac:   dbgpln(" LapTimeFrac:   %");          break;
    case NDS_TotalTime:     dbgpln(" TotalTime:     mSeconds");   break;
    case NDS_TotalTimeFrac: dbgpln(" TotalTimeFrac: %");          break;
    case NDS_LapCount:      dbgpln(" Laps:          ");           break;
    case NDS_TotalCounts:   dbgpln(" TotalLaps:     ");           break;
    }
  dbgp("    %12.12s", "ConfigureJoins");
  dbgp(" %12.12s", "JoinPressures");
  dbgp(" %12.12s", "FlowEquations");
  dbgp(" %12.12s", "Products");
  dbgp(" %12.12s", "ProdSurge");
  dbgp(" %12.12s", "Derivs");
  dbgp(" %12.12s", "ConvergeStates");
  dbgp(" %12.12s", "SteadyState");
  dbgp(" %12.12s", "CtrlStrategy");
  dbgp(" %12.12s", "Discrete");
  dbgpln(" Tag");

  double MaxSW=0.0;
  CFlwNodeIter All(m_AllNds);
  for (FlwNode * p=All.First(); (p); p=All.Next())
    for (int i=0; i<FSW_Max; i++)
      MaxSW=Max(MaxSW, p->SW(i).Secs());

  for (p=All.First(); (p); p=All.Next())
    {
    #if dbgFlwSolve
    flag DoIt=(flag)dbgNdStatsAll();
    #else
    flag DoIt=0;
    #endif
    for (int i=0; i<FSW_Max; i++)
      DoIt=DoIt || p->SW(i).Secs()>0.01*MaxSW;
    if (DoIt)
      {

      for (int i=0, j; (j=Selection[i])>=0; i++)
        {
        switch (What)
          {
          case NDS_LapTime:
            dbgp(" %12.3f", p->SW(j).LapTime()*1e3);
            break;
          case NDS_TotalTime:
            dbgp(" %12.3f", p->SW(j).Secs()*1e3);
            break;
          case NDS_LapCount:
          case NDS_TotalCounts:
            dbgp(" %12i", p->SW(j).Laps());
            break;
          }
        }
      dbgpln(" %-15s %s", p->ClassId(), p->Tag());
      }
    }
#endif
  }

//===========================================================================
//
//                                  Flw Solver
//
//===========================================================================

IMPLEMENT_TAGOBJ(CFlwSolver, "FlwSolve", "FlwSolve", "", "", "SFE", TOC_SYSTEM, "The Solver", "The Solver");

//===========================================================================

CFlwSolver::CFlwSolver(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) : \
  FlwNode(pClass_, TagIn, pAttach, eAttach),
  Dyn(this),
  PB(this),
  SDBObj(&SDBObjectClass,     "$SDB", this, TOA_Embedded),//NULL, TOA_Free),
  TearObj(&TearObjectClass,   "$Tear", NULL, TOA_Free),
#if WITHCTRLOBJ
  CtrlObj(&CtrlObjectClass,   "$Ctrl", NULL, TOA_Free),
#endif
#if WITHORDEROBJ
  OrderObj(&OrderObjectClass, "$Order", NULL, TOA_Free),
#endif
#if WITHCHANGEOBJ
  ChgObj(&ChgObjectClass,     "$Chg", NULL, TOA_Free),
#endif
  TolObj(&TolObjectClass,     "$Tol", NULL, TOA_Free),
#if WITHIOOBJ
  IOObj(&IOObjectClass,       "$IO", NULL, TOA_Free),
#endif
  PwrObj(&PwrObjectClass,     "$Pwr", NULL, TOA_Free),
  m_MCloseTol(TBF_BothSys, "Closure:MassFlow",     1.0e-4, 1.0e-4, 100, TBF_UseAbs|TBF_UseRel, DC_Qm, "kg/s"),
  m_HCloseTol(TBF_BothSys, "Closure:Enthalpy", 1.0e-4, 1.0e-4, 100, TBF_UseAbs|TBF_UseRel, DC_Pwr, "kJ/s"),
  m_HCloseTol2(TBF_BothSys, "Closure:Enthalpy2", 1.0e-4, 1.0e-4, 100, TBF_UseAbs|TBF_UseRel, DC_Pwr, "kJ/s")
  {
  //OrderObj.AttachMeToObj(this, TOA_HoldOps),

  bDoSystemDefn=false;

  fStopOnError=1;
  AttachClassInfo(nc_FlwSolver, NULL, &NullFlwGroup);
  m_pRoot       = this;

  fGlblLinksDone=false;
  fKeepGlblLinks=true;
  m_fHoldDoGlobalLinks=false;
  m_bMustCreateObjsFromFile=false;

  FlushDbg=0;
//  InitializeCriticalSection(&xCritSect);
  //TargetTags.Init(this);

  XData.SetSize(0, 64);
  LastNotPacked=-1;
  Rpt=NULL;
  iSaveWhat=100;
//  fSaveNode=false;
//  fSaveTear=false;
  pSaveNode=NULL;
  pSaveTear=NULL;
  iSaveIONo=0;
  m_RQ_Files_Cnt=0;
  m_bThreadStats=false;

  //TODO Sort out Hyper Threading Processor Count
  SYSTEM_INFO si;
  GetSystemInfo(&si);
  nProcessors=si.dwNumberOfProcessors;
#if FORCERQDTHREADS
  nRqdThreads = 0;
#else
  nRqdThreads=-1;
#endif

  m_RandomSeed=dNAN;

  gs_Exec.XGlblResultValidity(false);
  iLclResultsValid=-1;

  SpModel::iEnthalpyStrategy=SPMES_BestTemp;
  //SpModel::iSpMdlMixStrategy=(OrigPrjFileVerNo()<27) ? SPMMS_PreBld35 : SPMMS_Bld35;
  SpModel::iSpMdlMixStrategy=SPMMS_Bld35; // 28/11/2001 from ver 8.2 Build 63 force this to "new" method ALWAYS!

  /*#if WithQAL
  if (QALOptions())
    {
    SpModel::iEnthalpyStrategy=SPMES_ConserveH;
    //SpModel::iSpMdlMixStrategy=SPMMS_PreBld35;
    }
  #endif*/

  fADInited=false;
  m_ADIndexGrp=-1;
  m_ADIndexObj=-1;

  NetMaxResRatio=NetMaxResRatioInit;
  CFlwBlkData::sm_iFlwPh_Block=FPh_None;
  CFlwBlkData::sm_iFlwPh_Force=FPh_None;

  fSolutionForRoot=true;

  gs_PwrSupplies.Initialise(true);

  m_lUnDoGlobalLinksCount=1;
  DDBDataInfo::gs_TagRefGlblFn=this;
  CTagRef::gs_TagRefGlblFn=this;

  GCInsMngr::sm_ExecTimeout=10;
  CCnMixSet::GlblInitialise();


  InitialiseEnvironVars();
  }


//--------------------------------------------------------------------------

CFlwSolver::~CFlwSolver()
  {
  XDataClear();

  UnDoGlobalLinks();
  EqnSlv().DisConnectAllTears();
  EqnSlv().RemoveAllTears();

  //TargetTags.Term();
  gs_PwrSupplies.Terminate();
  CTagRef::gs_TagRefGlblFn=NULL;
  DDBDataInfo::gs_TagRefGlblFn=NULL;
  CCnMixSet::GlblTerminate();
  }

//--------------------------------------------------------------------------

static void FnSearchReplaceParts(Strng & Fn, LPTSTR Old, LPTSTR New)
  {
  int lo=strlen(Old);
  int lf=Fn.Length();
  int n=lf-lo+1;
  for (int i=0; i<n; i++)
    if ((i==0 || Fn[i-1]=='\\') && (i+lo==lf || Fn[i+lo]=='\\'))
      if (_strnicmp(&Fn[i], Old, lo)==0)
        {
        // found - replace
        Strng T;
        T=Fn.Left(i);
        T+=New;
        T+=Fn.Right(lf-(i+lo));
        Fn=T;
        break;
        }
  }

//--------------------------------------------------------------------------

bool CFlwSolver::ParseSpecieDef1(int nToks, CSVColArray & C, CDBInitRec &CDBI, int CfgFilePrjFileVerNo, int InsertIndex)
  {
  int Type=SafeAtoL(C[0]);
  Type=FixSpecieTypeId(Type, CfgFilePrjFileVerNo);

  if (C[1])
    {
    switch (Type)
      {
      case spl_SpecieId:  SVI.ParseSpecie("", C[1], C[2], C[3], C[4], CDBI, 0, InsertIndex);  break;
      case spl_SolSum:    SVI.ParseCalculation(C[1], "", "SumS", CDBI, 0, InsertIndex);       break;
      case spl_LiqSum:    SVI.ParseCalculation(C[1], "", "SumL", CDBI, 0, InsertIndex);       break;
      case spl_VapSum:    SVI.ParseCalculation(C[1], "", "SumG", CDBI, 0, InsertIndex);       break;
      case spl_AllSum:    SVI.ParseCalculation(C[1], "", "SumSLG", CDBI, 0, InsertIndex);     break;
      case spl_Text:      SVI.ParseAnnotation("Text", C[1], CDBI, 0, InsertIndex);        break;
      case spl_Page:      SVI.ParseAnnotation("Page", C[1], CDBI, 0, InsertIndex);        break;
      }
    }
  return true;
  }


//--------------------------------------------------------------------------
//
//
//
//--------------------------------------------------------------------------

bool CFlwSolver::ParseSpecieDef2(int nToks, CSVColArray & C, CDBInitRec &CDBI, int CfgFilePrjFileVerNo)
  {
  //int s=SVI.GetCount();
  CSysVecItem * p = NULL;
  //SVI.Add(p);
  // NBNB Check Validity of Data
  if (_stricmp(C[0], "Specie")==0)
    p=SVI.ParseSpecie(nToks, C, CDBI);
  else if (_stricmp(C[0], "Attribute")==0)
    p=SVI.ParseAttribute(nToks, C, CDBI);
  else if (_stricmp(C[0], "Calculation")==0)
    p=SVI.ParseCalculation(nToks, C, CDBI);
  else if (_stricmp(C[0], "Marker")==0)
    p=SVI.ParseAnnotation(nToks, C, CDBI, SVA_Marker);
  else if (_stricmp(C[0], "Text")==0)
    p=SVI.ParseAnnotation(nToks, C, CDBI, SVA_Text);
  else if (_stricmp(C[0], "Page")==0)
    p=SVI.ParseAnnotation(nToks, C, CDBI, SVA_OptPage);
  else if (_stricmp(C[0], "RqdPage")==0)
    p=SVI.ParseAnnotation(nToks, C, CDBI, SVA_RqdPage);
  else
    LogError("SpecieConfig", LF_Exclamation, "Unknown specie config type %s", C[0]);

  return true;
  }

//--------------------------------------------------------------------------

bool CFlwSolver::LoadSpecieData(LPCTSTR CfgFile, LPCTSTR PrjSDBName, bool WithMsgs)
  {
  CDBInitRec CDBI(C_2_K(0.0), C_2_K(1000.0), 50.0, 350.0, false);

  CProfINIFile CfgPF(CfgFile);
  Strng Tmp;

  long CfgFilePrjFileVerNo=CfgPF.RdInt("General", "PrjFileVersion", 0);

  SetStdTemp(CfgPF.RdDouble("General", "Std_Temp", Std_T));
  SetStdPress(CfgPF.RdDouble("General", "Std_Press", Std_P));
  SetNormTemp(CfgPF.RdDouble("General", "Norm_Temp", Norm_T));
  SetNormPress(CfgPF.RdDouble("General", "Norm_Press", Norm_P));
  if (OrigPrjFileVerNo()<62)
    {
    if (fabs(StdPress()-101.31)<1.0e-6)
      {
      SetStdPress(101.325);
      CfgPF.WrDouble("General", "Std_Press", StdPress());
      }
    if (fabs(NormPress()-101.31)<1.0e-6)
      {
      SetNormPress(101.325);
      CfgPF.WrDouble("General", "Norm_Press", NormPress());
      }
    }
  CDBI.MinT=CfgPF.RdDouble("General", "Minimum_Temp", C_2_K(0.0));
  CDBI.MaxT=CfgPF.RdDouble("General", "Maximum_Temp", C_2_K(1000.0));
  CDBI.MinP=CfgPF.RdDouble("General", "Minimum_Press", 10.0);
  CDBI.MaxP=CfgPF.RdDouble("General", "Maximum_Press", 5000.0);
  BOOL ExcludeDefSpDB = !CfgPF.RdInt("General", "SearchDefaultSpecieDB", 0);
  //BOOL ExcludeDefSpDB = true;
#if WITH_H2O_FNS_ALWAYS
  CDBI.UseStdFns4H2O = true;
#else
  CDBI.UseStdFns4H2O = CfgPF.RdInt("General", "UseStdFns4H2O", (OrigPrjFileVerNo()<27 ? 0 : 1));
#endif
#if WithMG
  if (MGOptions())
    CDBI.UseStdFns4H2O = false;
#endif

  Tmp=CfgPF.RdStr("General", "Default_SpModel");
  CheckSpMdlName(Tmp);
  if (Tmp())
    if (SpModelClass.FindClassId(Tmp()))
      SpModelOwner::DefaultModel=Tmp;
    else
      LogError("SysCAD", LF_Exclamation, "Unknown Specie Model '%s'", Tmp());

  SVI.Init();

  //  Strng DllName;


  Tmp=CfgPF.RdStr("Species", "S0000", "");
  if (Tmp() && Tmp.XStrChr(','))
    {
    CSVColArray C;
    int nToks=ParseTokenList(Tmp(), C, "", MaxCSVCols, ',');
    bool OldFmt=isdigit(C[0][0])!=0;
    if (OldFmt)
      {
      for (int i=0; ; i++)
        {
        Strng Tag;
        Tag.Set("S%04i",i);
        Tmp=CfgPF.RdStr("Species", Tag(), "");
        if (Tmp() && Tmp.XStrChr(','))
          {
          int nToks=ParseTokenList(Tmp(), C, "", MaxCSVCols, ',');
          ParseSpecieDef1(nToks, C, CDBI, CfgFilePrjFileVerNo);
          }
        else
          break;
        }
      }
    else
      {
      for (int i=0; ; i++)
        {
        Strng Tag;
        Tag.Set("S%04i",i);
        Tmp=CfgPF.RdStr("Species", Tag(), "");
        if (Tmp() && Tmp.XStrChr(','))
          {
          CSVColArray C;
          int nToks=ParseTokenList(Tmp(), C, "", MaxCSVCols, ',', true);
          ParseSpecieDef2(nToks, C, CDBI, CfgFilePrjFileVerNo);
          }
        else
          break;
        }
      }
    }

  for (int i=0; ; i++)
    {
    Strng Tag;
    Tag.Set("P%04i",i);
    Tmp=CfgPF.RdStr("PhaseNames", Tag(), "");
    if (Tmp() && Tmp.XStrChr(','))
      CDBI.m_PhNames.Append(Tmp());
    else
      break;
    }

  bool H2OAsAqueous=CfgPF.RdInt("General", "H2O_As_Aqueous", 0)!=0;
  if (!CDB.Initialise(CDBI, (LPSTR)PrjSDBName, !ExcludeDefSpDB, H2OAsAqueous))
    {
    //CDB.DumpTest();
    LogError("SpecieDB", LF_Exclamation, "Specie database failed to initialise (see preceding messages)");
    return false;
    }

  Tmp=CfgPF.RdStr("SpecieTemplates", "Names", "");
  if (Tmp() && Tmp.XStrChr(','))
    {
    CSVColArray C;
    int nToks=ParseTokenList(Tmp(), C, "", MaxCSVCols, ',');
    SVI.ParseSpecieTemplateNames2(nToks, C, CDBI);
    for (int i=0; ; i++)
      {
      Strng Tag;
      Tag.Set("T%04i",i);
      Tmp=CfgPF.RdStr("SpecieTemplates", Tag(), "");
      if (Tmp() && Tmp.XStrChr(','))
        {
        int nToks=ParseTokenList(Tmp(), C, "", MaxCSVCols, ',');
        SVI.ParseSpecieTemplateValues2(nToks, C, CDBI);
        }
      else
        break;
      }
    SVI.FinaliseSpecieTemplateValues();
    }

  SpModel::InitialiseHumidity();

  CDbgMngr DbgDBDump("Specie_DB", "Dump");
  if (DbgDBDump())
    CDB.DumpTest();
  return true;
  }

//--------------------------------------------------------------------------
//
//
//
//--------------------------------------------------------------------------

flag CFlwSolver::Init()
  {
  flag SfeInitOK=True;

  m_PrjSDBName=CfgFiles();
  m_PrjSDBName+=CfgDBFileName();
  m_PrjCnvsDBName=m_PrjSDBName; //cnvs DB same as specie DB

  m_PrjSDBName.FnExpand();
  m_PrjCnvsDBName.FnExpand();

  //load...
  int CnvsLoadedCnt = 0;
  if (PrjFileVerNo()<51)
    m_PrjCnvsDBName.FnSearchExpand(NULL, AF_CfgFiles|AF_BackupFiles);
  if (FileExists(m_PrjCnvsDBName()))
    CnvsLoadedCnt = gs_CnvsMngr.LoadCnvsADO(m_PrjCnvsDBName(), true);
  else
    m_PrjCnvsDBName="";
  if (CnvsLoadedCnt>0)
    LogNote("SysCAD", 0, "%d new conversions loaded from %s", CnvsLoadedCnt, m_PrjCnvsDBName());
  m_PrjCnvsDBName.FnMakePrjFileRelative();
  m_PrjCnvsDBName.FnContract();

  if (PrjFileVerNo()<51)
    m_PrjSDBName.FnSearchExpand(NULL, AF_CfgFiles|AF_BackupFiles);
  if (!FileExists(m_PrjSDBName()))
    {
    LogError("SysCAD", LF_Exclamation, "File not found: %s", m_PrjSDBName());
    //m_PrjSDBName="";
    }

  if (!LoadSpecieData(CfgFile(), m_PrjSDBName(), true))
    {
    //LogError("SysCAD", LF_Exclamation, "File not found: %s", m_PrjSDBName());
    return false;
    }

  //CSpecieBlk::AttachSelectableFns();
  //TagObjClass::UpdateSelectConditions();
  //CSpecieBlk::DetachSelectableFns();
  CSpecieBlk::TestInitialiseOK();

  CProfINIFile CfgPF(CfgFile());
  Strng Tmp;

  for (int i=0; ; i++)
    {
    Strng Tag, Cls, Tmp2;
    Tag.Set("S%04i",i);
    Tmp=CfgPF.RdStr("Selectable", Tag(), "");
    if (Tmp())
      {
      int Code=Tmp.SafeAtoL();
      flag On=(Code!=0);
      char * pComma=Tmp.XStrChr(',');
      Tmp=Tmp.Right(Tmp.Length()-(pComma-Tmp()+1));
      Tmp.LRTrim();
      char * pComma2=Tmp.XStrChr(',');
      if (pComma2)
        {
        Tmp2=Tmp.Right(Tmp.Length()-(pComma2-Tmp()+1));
        Tmp2.LRTrim();
        Tmp=Tmp.Left((pComma2-Tmp()));
        Tmp.LRTrim();
        }

      if (Tmp.XStrCmp("PipeEqn")==0)
        Tmp="PipeFlow";
      else if (Tmp.XStrCmp("Simple")==0)
        Tmp=BaseSpModelName;
      else if (Tmp.XStrCmp("Steam")==0)
        Tmp=BaseSpModelName;
      else if (Tmp.XStrCmp("BaseSpMdl")==0)
        Tmp=BaseSpModelName;

      if (CCOMObjManager::AddClass(Tmp(), Tmp2()))
        {
        }
      else if (Tmp())
        {
        TagObjClass *pG=TagObjClass::FindGroupId(Tmp());
        if (pG)
          {
          for ( ; pG; pG=pG->NextClassInGrp())
            pG->SetSelectable(On);
          }
        else
          {
          TagObjClass *p=TagObjClass::FindClassId(Tmp(), true, Tmp2());
          Strng Tg;
          Tg.Set(Tmp2() ? "%s.%s" : "%s", Tmp(), Tmp2());
          if (p)
            {
            p->SetSelectable(On);
            if (On && !p->Selectable())
              LogError("SysCAD", 0, "%s Model Class not available", Tg());
            }
          else
            {
            if (stricmp(Tg(), "CEnvelope-1")!=0)
              LogError("SysCAD", LF_Exclamation, "%s Unknown Model Class", Tg());
            }
          }
        }
      }
    else
      break;
    }

  // Force Some On
  static LPTSTR ForceOn[]=
    {
      "Feed_XPG-1",
      "Area-1",
      "Pipe-1",
      NULL,
    };
  for (int i=0; ForceOn[i]; i++)
    {
    TagObjClass *p=TagObjClass::FindClassId(ForceOn[i], true, NULL);
    if (p)
      p->SetSelectable(true);
    else
      LogError("SysCAD", LF_Exclamation, "Required %s Model Class not available", ForceOn[i]);
    }

  for (SfeInitClass* p=SfeInitClass::First(); p; p=p->Next())
    {
    if (!p->LoadConfiguration(CfgPF))
      SfeInitOK=false;
    }
  if (!SfeInitOK)
    return false;

  m_PrjSDBName.FnMakePrjFileRelative();
  m_PrjSDBName.FnContract();

  CDbgMngr DbgSpTest("SpContainer", "Test");
  if (DbgSpTest())
    SpContainerDumpTest();


  for (p=SfeInitClass::First(); p; p=p->Next())
    {
    if (!p->Initialise())
      SfeInitOK=False;
    }
  if (!SfeInitOK)
    return false;

  gs_Environment.Validate();

  #ifndef _RELEASE
  TagObjClass* pTOC = TagObjClass::FirstClass();
  while (pTOC)
    {
    //dword dw = pTOC->Category();
    dword dwCatMdl = pTOC->CategoryMdl();
    int Cnt=0;
    dword dwTest = TOC_STD_KENWALT;
    for (i=0; i<12; i++)
      {
      if ((dwCatMdl & dwTest)!=0)
        Cnt++;
      dwTest = dwTest >> 1;
      }
    if (Cnt==0)
      dbgpln("License Model Option NOT set for:               %s", pTOC->ClassName());
    if (Cnt>1)
      dbgpln("Only ONE License Model Option should be set for:%s", pTOC->ClassName());

    dword dwCatGrp = pTOC->CategoryGrp();
    Cnt=0;
    dwTest = TOC_GRP_GENERAL;
    for (i=0; i<12; i++)
      {
      if ((dwCatGrp & dwTest)!=0)
        Cnt++;
      dwTest = dwTest >> 1;
      }
    if (Cnt==0)
      dbgpln("Group Option NOT set for:                       %s", pTOC->ClassName());
    if (Cnt>1)
      dbgpln("Only ONE Group Option should be set for:        %s", pTOC->ClassName());

    pTOC = pTOC->NextClass();
    }
  #endif

  TagObjClass* pC = TagObjClass::FindClassId("Area-1");
  if (pC)
    {
    m_pPlantArea=InsertNode(m_pRoot, "Area-1", "", "", (pchar)PlantAreaTag, NULL, TOA_Free);
    }
  else
    {
    m_pPlantArea=NULL;
    ASSERT_ALWAYS(m_pPlantArea!=0, "$PlantArea not created", __FILE__, __LINE__);
    }

  return SfeInitOK;
  }

//--------------------------------------------------------------------------

flag CFlwSolver::Term()
  {
  XDataClear();

  UnDoGlobalLinks();
  EqnSlv().DisConnectAllTears();
  EqnSlv().RemoveAllTears();

  //TargetTags.Term();
  gs_PwrSupplies.Terminate();

  // delete all child objs(units/links)
  FlwNode*pD=NULL,*pD1;
  GetObj_All(pD); // Skip myself
  if (GetObj_All(pD))
    {
    // First Disconnect all;
    pD1=pD;
    while (pD)
      {
      pD->DisConnectAll();
      GetObj_All(pD);
      }
    // Now delete all;
    pD=pD1;
    while (pD)
      {
      pD1=pD;
      GetObj_All(pD);
      delete pD1;
      }
    }

  // CHeck all connects removed
  if (FlwNode::sm_ConnectList.GetCount()>0)
    dbgpln("Connects: %i NOT REMOVED From List", FlwNode::sm_ConnectList.GetCount());
  if (FlwNode::sm_ConnectMap.GetCount()>0)
    dbgpln("Connects: %i NOT REMOVED From MAP", FlwNode::sm_ConnectMap.GetCount());

  m_pPlantArea=NULL;

  SfeInitClass* p=SfeInitClass::First();
  while (p)
    {
    p->Terminate();
    p=p->Next();
    }
  CDB.Terminate();
  SVI.Term();

  return true;
  }

//--------------------------------------------------------------------------

void CFlwSolver::XDataClear()
  {
  for (int i=0; i<XData.GetSize(); i++)
    delete XData[i].pTAB;
  XData.SetSize(0, 64);
  LastNotPacked=-1;
  }

//--------------------------------------------------------------------------

TaggedObject * CFlwSolver::FindObjTag(pchar pSrchTag, flag SrchAll, int &ObjTagLen, int MinTagLen)
  {
  TaggedObject * pObj=CTNode::FindObjTag(pSrchTag, SrchAll, ObjTagLen, MinTagLen);
  if (pObj)
    return pObj;

  CConnect * pConn=FlwNode::FindConnect(pSrchTag, SrchAll, ObjTagLen, MinTagLen);
  if (pConn)
    {
    pObj=dynamic_cast<TaggedObject*>(pConn);
    return pObj;
    }

  pObj=TearVarBlk::staticFindObjTag(pSrchTag, SrchAll, ObjTagLen, MinTagLen);
  if (pObj)
    return pObj;

  pObj=Dyn.FindObjTag(pSrchTag, SrchAll, ObjTagLen, MinTagLen);
  if (pObj)
    return pObj;

  pObj=PB.FindObjTag(pSrchTag, SrchAll, ObjTagLen, MinTagLen);
  if (pObj)
    return pObj;

  pObj=SDBObj.FindObjTag(pSrchTag, SrchAll, ObjTagLen, MinTagLen);
  if (pObj)
    return pObj;

  pObj=TearObj.FindObjTag(pSrchTag, SrchAll, ObjTagLen, MinTagLen);
  if (pObj)
    return pObj;

#if WITHCTRLOBJ
  pObj=CtrlObj.FindObjTag(pSrchTag, SrchAll, ObjTagLen, MinTagLen);
  if (pObj)
    return pObj;
#endif

#if WITHCHANGEOBJ
  pObj=ChgObj.FindObjTag(pSrchTag, SrchAll, ObjTagLen, MinTagLen);
  if (pObj)
    return pObj;
#endif

  pObj=TolObj.FindObjTag(pSrchTag, SrchAll, ObjTagLen, MinTagLen);
  if (pObj)
    return pObj;

#if WITHORDEROBJ
  pObj=OrderObj.FindObjTag(pSrchTag, SrchAll, ObjTagLen, MinTagLen);
  if (pObj)
    return pObj;
#endif

  pObj=PwrObj.FindObjTag(pSrchTag, SrchAll, ObjTagLen, MinTagLen);
  if (pObj)
    return pObj;

#if WITHIOOBJ
  pObj=IOObj.FindObjTag(pSrchTag, SrchAll, ObjTagLen, MinTagLen);
  if (pObj)
    return pObj;
#endif

  return pObj;
  };

//--------------------------------------------------------------------------

static DDBValueLst DDBEnthStrat[] = 
  {
    {SPMES_BestTemp,  "Best_Temperature"},
    {SPMES_ConserveH, "Conserve_Heat"},
    {0}
  };
static DDBValueLst DDBSpMixStrat[] = 
  {
    {SPMMS_PreBld35,  "Pre_Build_35"},
    {SPMMS_Bld35,     "Build_35"},
    {0}
  };
static DDBValueLst DDBCloseTest[] = 
  {
    {CloseTest_Off,         "Off"},
    {CloseTest_Msgs,        "On-Messages"},
    {CloseTest_Cnds,        "On-Conditions"},
    {CloseTest_Both,        "On-Both"},
    {CloseTest_IdleBoth,    "On-Both-Idle"},
    {0}
  };
static DDBValueLst DDBSpillReporting[] = 
  {
    {SpillReports_Off,        "Off"},
    {SpillReports_Msgs ,      "On-Messages"},
    {SpillReports_Cnds ,      "On-Conditions"},
    {SpillReports_Both ,      "On-Both"},
    {SpillReports_IdleUnits,  "On-Both-IdleUnits"},
    {SpillReports_IdleLinks,  "On-Both-IdleLinks"},
    {SpillReports_IdleBoth ,  "On-Both-Idle"},
    {0}
  };
static DDBValueLst DDBAuditOptions[] = 
  {
    {Audit_AllOff,   "AllOff"   },
    //{-1,             "-------",  MDD_NoSelect},
    {Audit_Off,      "Off"      },
    {Audit_Flows,    "Flows"    },
    {Audit_Totals,   "Totals"   },
    {Audit_Balance,  "Balance"  },
    {0}
  };

static DDBValueLst DDBRangeChecks[] = 
  {
    {0,                                     "None"    },
    {RngChk_Temperature,                    "T"       },
    {RngChk_Density,                        "Rho"     },
    {RngChk_Temperature|RngChk_Density,     "T,Rho"   },
    {0}
  };


void CFlwSolver::BuildDataDefn(DataDefnBlk & DDB)
  {
  if (PrjFileVerNo() < 18)
    {
    BuildDataDefnOld(DDB);
    return;
    }

  //==========PlantModel==========


  if (DDB.BeginStruct(this))
    {

    DDB.Text  ("");
    DDB.Text  ("Commands");
    DDB.Visibility();
    DDB.Long  ("State",                 "", DC_, "",  xidExecState,                   this, InitHidden|DDEF_WRITEPROTECT|noFile|noScenario|noSnap);
    DDB.String("StateDesc",             "", DC_, "",  xidExecStateDesc,               this, DDEF_WRITEPROTECT|noFile|noScenario|noSnap);
    DDB.Button("Cmd.Start",             "", DC_, "", xidExecStart,                    this, isParm);
    DDB.Button("Cmd.Idle",              "", DC_, "", xidExecIdle,                     this, isParm);
    DDB.Button("Cmd.Stop",              "", DC_, "", xidExecStop,                     this, isParm);
    //DDB.Text  ("");
    DDB.Bool("SetStateAll",             "", DC_, "",  xidSetStateAll,                 this, isParm|noFileAtAll|InitHidden, SetStateStrs);
    DDB.Button("Cmd.ZeroFlows",         "", DC_, "", xidSetStateAllZeroFlows,         this, isParm|noFileAtAll|InitHidden);
    DDB.Button("Cmd.Empty",             "", DC_, "", xidSetStateAllEmpty,             this, isParm|noFileAtAll|InitHidden);
    DDB.Button("Cmd.Preset",            "", DC_, "", xidSetStateAllPreSet,            this, isParm|noFileAtAll|InitHidden);
    DDB.Button("Cmd.SteadyState",       "", DC_, "", xidSetStateAllSteadyState,       this, isParm|noFileAtAll|InitHidden);
    DDB.Button("Cmd.PBInit",            "", DC_, "", xidSetStateAllPBInit,            this, isParm|noFileAtAll|InitHidden);
    DDB.Button("Cmd.DynStatsRunInit",   "", DC_, "", xidSetStateAllDynStatsRunInit,   this, isParm|noFileAtAll|InitHidden);
    DDB.Button("Cmd.EmptySpillTargets", "", DC_, "", xidSetStateAllEmptySpillTargets, this, isParm|noFileAtAll|InitHidden);

    // For Compatibility with older flowsheets
    //DDB.Text  ("");
    DDB.Double("Dyn.Time",              "", DC_Time, "s", xidODETime,                 this, InitHidden|noFile|noScenario|noSnap);
    DDB.Double("Dyn.dTime",             "", DC_Time, "s", xidODETimeInc,              this, InitHidden|noFile|noScenario|noSnap);
    //DDB.Double("Dyn.StopTime",          "", DC_Time, "s", &Dyn.StopTime,              this, InitHidden|noFile|noScenario|noSnap|isParm);
    DDB.Double("Dyn.dTimeMax",          "", DC_Time, "s", xidODETimeIncMxRqd,         this, InitHidden|noFile|noScenario|noSnap|isParm);
    DDB.Double("Dyn.dTimeMin",          "", DC_Time, "s", xidODETimeIncMnRqd,         this, InitHidden|noFile|noScenario|noSnap|isParm);

    DDB.Text  ("");
    DDB.Text("Balance & Closure");
    DDB.Double("TestMassTolerance",     "", DC_Frac, "%", &SpModel::dTestToleranceMassFrac,   this, isParm|SetOnChange);
    DDB.Long  ("ClosureTests",          "", DC_,     "",  &gs_ClosureOption,                  this, isParmStopped|SetOnChange, DDBCloseTest);
    DDB.Long  ("SpillReporting",        "", DC_,     "",  &gs_SpillOption,                    this, isParm       |SetOnChange, DDBSpillReporting);
    DDB.Long  ("AuditLevel",            "", DC_,     "",  &gs_AuditOption,                    this, isParmStopped|SetOnChange, DDBAuditOptions);
    DDB.Long  ("RangeChecks",           "", DC_,     "",  &gs_RangeChecks,                    this, isParmStopped|SetOnChange, DDBRangeChecks);
    
    DDB.Text  ("");
    DDB.Text  ("Display Limits");
    DDB.Double("ZeroFlow",              "", DC_,     "",  &DisplayZeroFlow,           this, isParmStopped);
    DDB.Double("Zero",                  "", DC_,     "",  &DisplayZeroLimit,          this, isParmStopped);

    //DDB.Text  ("");
    //DDB.Text  ("Information");
    //DDB.Long  ("ConditionCount",        "", DC_, "",   xidConditionCnt,                    this, isResult);

    DDB.Text  ("");
    DDB.Text  ("Options");
    DDB.Byte  ("StopOnError",           "", DC_,     "",  &fStopOnError,     this, isParmStopped|SetOnChange, DDBYesNo);
    DDB.Double("PGM_ExecuteTimeout",    "", DC_Time, "s", xidPGMExecTimeOut, this, isParmStopped);
    DDB.Double("RandomSeedAtStart",     "", DC_,     "",  xidRandomSeed,     this, isParmStopped|NAN_OK);

    DDB.Text  ("");
    DDB.Text  ("MultiProcessing");
    // Removed By CNM - why would ew need these ???
    //DDB.Byte  ("KeepGlblLinks",         "", DC_,     "",  &fKeepGlblLinks,   this, isParmStopped|SetOnChange, DDBYesNo);
    //DDB.Byte  ("GlblLinksDone",         "", DC_,     "",  &fGlblLinksDone,   this, DDEF_WRITEPROTECT|noFile|noScenario|noSnap, DDBYesNo);
    DDB.Long  ("NoOfProcessors",        "", DC_,     "",  &nProcessors,      this, DDEF_WRITEPROTECT|noFile|noScenario|noSnap);
    DDB.Long  ("NoOfSolveThreads",      "", DC_,     "",  xidNThreads,       this, 0);
    DDB.Long  ("NoOfSolveThreadsRqd",   "", DC_,     "",  xidRqdThreads,     this, isParmStopped);

    DDB.Text  ("");
    DDB.Text("DirectIOs");
    if (DDB.BeginStruct(this, "DIO", NULL, DDB_NoPage))
      {
      static DDBValueLst DDBSV[]= { {eDIOSlct_None, "None"}, {eDIOSlct_All, "All"}, /*{eDIOSlct_Selected, "Selected"}, */{0} };
      DDB.Long("ConnectSpills",         "", DC_,   "",  xidDIOSpillsVents,          this, isParmStopped, DDBSV);
      DDB.Long("ConnectLeaks",          "", DC_,   "",  xidDIOLeaks,                this, isParmStopped, DDBSV);
      DDB.Long("ConnectAudit",          "", DC_,   "",  xidDIOAudit,                this, isParmStopped, DDBSV);
      DDB.Text(" ");
      }
    DDB.EndStruct();


    //DDB.Text  ("Executive");
    //if (DDB.BeginStruct(this, "Exec", NULL, DDB_NoPage))

/*
    DDB.Text  ("Environment");
    DDB.String("StartupDirectory ",    "", DC_, "", xidStartupDirectory  ,  this, noFile|noScenario|noSnap);
    DDB.String("StartupDrive     ",    "", DC_, "", xidStartupDrive      ,  this, noFile|noScenario|noSnap);
    DDB.String("ProgFiles        ",    "", DC_, "", xidProgFiles         ,  this, noFile|noScenario|noSnap);
    DDB.String("TemporaryFiles   ",    "", DC_, "", xidTemporaryFiles    ,  this, noFile|noScenario|noSnap);
    DDB.String("BaseCfgFiles     ",    "", DC_, "", xidBaseCfgFiles      ,  this, noFile|noScenario|noSnap);
    DDB.String("CfgName          ",    "", DC_, "", xidCfgName           ,  this, noFile|noScenario|noSnap);
    DDB.String("CfgFile          ",    "", DC_, "", xidCfgFile           ,  this, noFile|noScenario|noSnap);
    DDB.String("CfgFiles         ",    "", DC_, "", xidCfgFiles          ,  this, noFile|noScenario|noSnap);
    DDB.String("PrjName          ",    "", DC_, "", xidPrjName           ,  this, noFile|noScenario|noSnap);
    DDB.String("PrjFile          ",    "", DC_, "", xidPrjFile           ,  this, noFile|noScenario|noSnap);
    DDB.String("PrjFiles         ",    "", DC_, "", xidPrjFiles          ,  this, noFile|noScenario|noSnap);
*/

    //==========General==========
    DDB.Page("Species", DDB_RqdPage);
    DDB.Text  ("");
    //TODO Check surfaces & smoothing for H2O
    //flag AllowSurfaces = true;
#ifdef _RELEASE
    flag AllowSurfaces = false; //KGA: 15/9/05 Why was this true if surfaces, etc is buggy!?!?
#else
    flag AllowSurfaces = true;
#endif
    if (!AllowSurfaces)
      CSurfaceCommon::m_UseSurface=0;//force off
#ifdef _RELEASE
    flag AllowSmoothing = false;
#else
    flag AllowSmoothing = true;
#endif
    if (!AllowSmoothing)
      CSurfaceCommon::m_DoSmoothing=0;//force off

    //DDB.Text  ("");
    DDB.Text  ("Specie Database");
    DDB.Double("NormalTemp",    "NormT",  DC_T,   "C",    xidNormT,       this, isResult|noFileAtAll);
    DDB.Double("NormalPress",   "NormP",  DC_P,   "kPa",  xidNormP,       this, isResult|noFileAtAll);
    DDB.Double("StandardTemp",  "StdT",   DC_T,   "C",    xidStdT,        this, isResult|noFileAtAll);
    DDB.Double("StandardPress", "StdP",   DC_P,   "kPa",  xidStdP,        this, isResult|noFileAtAll);
    DDB.Text  ("");
    DDB.Long  ("EnthalpyStrategy",      "", DC_,     "",  (long*)&SpModel::iEnthalpyStrategy, this, isParmStopped|SetOnChange, DDBEnthStrat);
    DDB.Long  ("H2OProperties",         "", DC_,     "",  xidH2OProperties,                   this, isParmStopped|SetOnChange, CDB.H2ODDBList());
    DDB.CheckBoxBtn("FunctionSurfaces", "", DC_,     "",  xidUseSurfaces,                     this, AllowSurfaces ? isParmStopped : 0);
    DDB.CheckBoxBtn("SmoothSurfaces",   "", DC_,     "",  xidDoSmoothing,                     this, AllowSmoothing ? isParmStopped : 0);
#if UseNewSurfaces
    //DDB.Long  ("SurfaceElements",       "", DC_,     "",  &CSurface3DFrame::m_nGlblElementCnt,  this, InitHidden|noFile|noScenario|noSnap);
    DDB.Long  ("SurfaceElements",       "", DC_,     "",  &CSurface3DFrame::m_nGlblFrameCnt,  this, InitHidden|noFile|noScenario|noSnap);
#else
    DDB.Long  ("SurfaceElements",       "", DC_,     "",  &CSurface3DElement::m_nGlblElementCnt,  this, InitHidden|noFile|noScenario|noSnap);
#endif
    DDB.CheckBoxBtn("IncludeSpcCnvs",   "", DC_,     "",  xidIncludeSpcCnvs,                  this, isParmStopped);
    DDB.Visibility(SM_All|HM_Reduced|HM_Full);
    DDB.CheckBoxBtn("ShowSpcRangeErrors", "", DC_,   "",  xidShowSpcRangeErrors,              this, isParmStopped);
    DDB.CheckBoxBtn("ShowSpcXRangeErrors", "", DC_,  "",  xidShowSpcXRangeErrors,             this, isParmStopped);


    DDB.Visibility();
    DDB.Text  ("");
    //DDB.Long  ("SpMdlMixStrategy",      "", DC_,     "",  (long*)&SpModel::iSpMdlMixStrategy, this, isParmStopped|SetOnChange, DDBSpMixStrat);
    //DDB.Text  ("Atmosphere...");
    gs_Environment.BuildDataDefn(this, "Environment", DDB);


    /*
    Environment
    Altitude      m
    AmbientTemp   C
    DrybulbTemp   C
    WetbulbTemp   C


    */

        //DDB.Visibility();
    //DDB.Text  ("");
    //DDB.Text  ("System...");
    if (DDB.BeginStruct(this, "System", NULL, DDB_RqdPage))
      {
      DDB.Text  ("");
      DDB.Text  ("Executable Build...");
      DDB.String("VersionOnly",     "", DC_,       "", xidGenVerStr,       this, InitHidden|isResult|noFile|noScenario|noSnap);
      DDB.String("Version",         "", DC_,       "", xidGenUpdateVerStr, this, isResult|noFile|noScenario|noSnap);
      DDB.Long  ("MajorBuildNo",    "", DC_,       "", xidGenMajorBuildNo, this, isResult|noFile|noSnap);
      DDB.Long  ("BuildNo",         "", DC_,       "", xidGenBuildNo,      this, isResult|noFile|noSnap);
      DDB.String("BuildDate",       "", DC_,       "", xidGenBuildDate,    this, isResult|noFile|noScenario|noSnap);

      DDB.Text  ("");
      DDB.Text  ("Project...");
      DDB.String("PrjName",         "", DC_,       "", xidPrjName,         this, isResult|noFile|noScenario|noSnap);
      DDB.String("PrjFolder",       "", DC_,       "", xidPrjFiles,        this, isResult|noFile|noScenario|noSnap);
      DDB.String("PrjFile",         "", DC_,       "", xidPrjFile,         this, isResult|noFile|noScenario|noSnap);
      DDB.String("CfgFile",         "", DC_,       "", xidCfgFile,         this, isResult|noFile|noScenario|noSnap);
      DDB.Text  ("");
      DDB.String("DateAndTime",     "", DC_,       "", xidGenDateAndTime,  this, isResult|noFile|noScenario|noSnap);
      DDB.Double("Time1970",        "", DC_Time,  "s", xidGenTime1970,     this, InitHidden|isResult|noFile|noScenario|noSnap);
      }
    DDB.EndStruct();


    //==========Pages==========
    if (DDB.BeginStruct(this, "Pages", NULL, DDB_RqdPage))
      {
      if (DDB.ForView() && DDB.DoingGetData())
        gs_GrfTagGroups.Rebuild();

      DDB.Long  ("Count",           "", DC_,       "", xidGrfCount,        this, isResult|noFile|noScenario|noSnap);
      const long GrfCnt = gs_GrfTagGroups.Count();
      Strng GrfTag;
      for (int i=0; i<GrfCnt; i++)
        {
        GrfTag.Set("Grf[%d].Name", i);
        DDB.String(GrfTag(), "", DC_,   "",  xidGrfFirstActive+(i*2)+0, this, isResult|noFile|noScenario|noSnap);
        GrfTag.Set("Grf[%d].Active", i);
        DDB.CheckBoxBtn(GrfTag(), "", DC_,   "",  xidGrfFirstActive+(i*2)+1, this, isParmStopped|noFile|noScenario|noSnap);
        }
      }
    DDB.EndStruct();


    //==========Stats==========
    DDB.Page("Statistics", DDB_RqdPage);
    if (DDB.BeginStruct(this, "Stats", NULL, DDB_NoPage))
      {
      DDB.Visibility(NM_Dynamic|SM_All|HM_All);
      DDB.Long  ("Steps",           "", DC_,       "", xidXSWSteps,        this, isResult|noFileAtAll);
      DDB.Visibility();
      DDB.Long  ("Iterations",      "", DC_,       "", xidSolveIters,      this, isResult|noFileAtAll);
      DDB.Visibility(NM_Dynamic|SM_All|HM_All);
      DDB.Long  ("BadIterations",   "", DC_,       "", xidSolveItersBad,   this, isResult|noFile|noSnap);
      DDB.Double("SimulationTime",  "", DC_Time,  "s", xidXSWSimSecs,      this, InitHidden|isResult|noFile|noSnap);
      DDB.String("SimulationTimeDesc", "", DC_,    "", xidXSWSimSecsDesc,  this, isResult|noFileAtAll);
      DDB.Visibility();
      DDB.Double("StartingTime",    "", DC_Time,  "s", xidXSWSecsStart,    this, InitHidden|isResult|noFile|noSnap);
      DDB.Double("SolveTime",       "", DC_Time,  "s", xidXSWSecs,         this, InitHidden|isResult|noFile|noSnap);
      DDB.String("SolveTimeDesc",   "", DC_,       "", xidXSWSecsDesc,     this, isResult|noFileAtAll);
      DDB.Double("StepTm",          "", DC_Time,  "s", xidXSWStepI,        this, isResult|noFileAtAll|DDEF_NOCOMPARE);
      DDB.Double("StepTmMoveAvg",   "", DC_Time,  "s", xidXSWStepMA,       this, isResult|noFileAtAll|DDEF_NOCOMPARE);
      DDB.Double("StepTmAvg",       "", DC_Time,  "s", xidXSWStepA,        this, isResult|noSnap|DDEF_NOCOMPARE);
      DDB.Double("RunSpd",          "", DC_Frac,  "%", xidXSWRunSpdI,      this, isResult|noFileAtAll|DDEF_NOCOMPARE);
      DDB.Double("RunSpdMoveAvg",   "", DC_Frac,  "%", xidXSWRunSpdMA,     this, isResult|noFileAtAll|DDEF_NOCOMPARE);
      DDB.Double("RunSpdAvg",       "", DC_Frac,  "%", xidXSWRunSpdA,      this, isResult|noSnap|DDEF_NOCOMPARE);
      DDB.Double("ProcessorUse",    "", DC_Frac,  "%", xidXSWProcUseI,     this, isResult|noFileAtAll);
      DDB.Double("ProcessorUseMoveAvg", "", DC_Frac, "%", xidXSWProcUseMA, this, InitHidden|isResult|noFileAtAll);
      DDB.Double("ProcessorUseAvg", "", DC_Frac,  "%", xidXSWProcUseA,     this, isResult|noSnap);
      DDB.Long  ("MoveAvgPoints",   "", DC_,      "",  xidXSWMVAvgPts,     this, isParm);


      DDB.Text  ("");
      DDB.Double("Memory.PageFaultCount",            "",            DC_,"",        xidPMPageFaultCount,             this, isResult|InitHidden|noFileAtAll|DDEF_NOCOMPARE);
      DDB.Double("Memory.PeakWorkingSetSize",        "",            DC_Bytes,"Mb", xidPMPeakWorkingSetSize,         this, isResult|noFileAtAll|DDEF_NOCOMPARE);
      DDB.Double("Memory.WorkingSetSize",            "Memory.Used", DC_Bytes,"Mb", xidPMWorkingSetSize,             this, isResult|InitHidden|noFileAtAll|DDEF_NOCOMPARE);
      DDB.Double("Memory.QuotaPeakPagedPoolUsage",   "",            DC_Bytes,"Mb", xidPMQuotaPeakPagedPoolUsage,    this, isResult|InitHidden|noFileAtAll|DDEF_NOCOMPARE);
      DDB.Double("Memory.QuotaPagedPoolUsage",       "",            DC_Bytes,"Mb", xidPMQuotaPagedPoolUsage,        this, isResult|InitHidden|noFileAtAll|DDEF_NOCOMPARE);
      DDB.Double("Memory.QuotaPeakNonPagedPoolUsage","",            DC_Bytes,"Mb", xidPMQuotaPeakNonPagedPoolUsage, this, isResult|InitHidden|noFileAtAll|DDEF_NOCOMPARE);
      DDB.Double("Memory.QuotaNonPagedPoolUsage",    "",            DC_Bytes,"Mb", xidPMQuotaNonPagedPoolUsage,     this, isResult|InitHidden|noFileAtAll|DDEF_NOCOMPARE);
      DDB.Double("Memory.PagefileUsage",             "",            DC_Bytes,"Mb", xidPMPagefileUsage,              this, isResult|InitHidden|noFileAtAll|DDEF_NOCOMPARE);
      DDB.Double("Memory.PeakPagefileUsage",         "",            DC_Bytes,"Mb", xidPMPeakPagefileUsage,          this, isResult|InitHidden|noFileAtAll|DDEF_NOCOMPARE);

      DDB.Double("Memory.Load",          "", DC_Frac, "%",   xidSMMemoryLoad,    this, isResult|InitHidden|noFileAtAll|DDEF_NOCOMPARE);
      DDB.Double("Memory.TotalPhys",     "", DC_Bytes, "Mb", xidSMTotalPhys,     this, isResult|InitHidden|noFileAtAll|DDEF_NOCOMPARE);
      DDB.Double("Memory.AvailPhys",     "", DC_Bytes, "Mb", xidSMAvailPhys,     this, isResult|InitHidden|noFileAtAll|DDEF_NOCOMPARE);
      DDB.Double("Memory.TotalPageFile", "", DC_Bytes, "Mb", xidSMTotalPageFile, this, isResult|InitHidden|noFileAtAll|DDEF_NOCOMPARE);
      DDB.Double("Memory.AvailPageFile", "", DC_Bytes, "Mb", xidSMAvailPageFile, this, isResult|InitHidden|noFileAtAll|DDEF_NOCOMPARE);
      DDB.Double("Memory.TotalVirtual",  "", DC_Bytes, "Mb", xidSMTotalVirtual,  this, isResult|InitHidden|noFileAtAll|DDEF_NOCOMPARE);
      DDB.Double("Memory.AvailVirtual",  "", DC_Bytes, "Mb", xidSMAvailVirtual,  this, isResult|InitHidden|noFileAtAll|DDEF_NOCOMPARE);

      DDB.Text  ("");
      DDB.Visibility(NM_Probal|SM_All|HM_All);
      DDB.Long("Unknowns",        "", DC_,      "",  xidSolveUnknowns,  this, isResult|noFile|noSnap);
      DDB.Long("BadError",        "", DC_,      "",  xidSolveBadError,  this, isResult|noFile|noSnap);
      DDB.Long("BadLimit",        "", DC_,      "",  xidSolveBadLimit,  this, isResult|noFile|noSnap);
      DDB.Double("WorstErr",      "", DC_Frac,  "",  xidSolveWorstErr,  this, isResult|noFile|noSnap);

      DDB.Visibility(NM_Dynamic|SM_All|HM_All);
      DDB.Long("Nets",            "", DC_,      "",  xidSolveNets,      this, isResult|noFile|noSnap);
      DDB.Long("SubNets",         "", DC_,      "",  xidSolveSubNets,   this, isResult|noFile|noSnap);
      DDB.Long("Nodes",           "", DC_,      "",  xidSolveNodes,     this, isResult|noFile|noSnap);
      DDB.Long("FxdLnks",         "", DC_,      "",  xidSolveFxdLnks,   this, isResult|noFile|noSnap);
      DDB.Long("LoneLnks",        "", DC_,      "",  xidSolveLoneLnks,  this, isResult|noFile|noSnap);
      DDB.Long("NetLnks",         "", DC_,      "",  xidSolveNetLnks,   this, isResult|noFile|noSnap);

      DDB.Text  ("");
      DDB.Double("Exec.StartTm",       "", DC_Frac,  "%", xidXSWStart,       this, isResult|noScenario|noSnap|DDEF_NOCOMPARE);
      DDB.Double("Exec.QueryTm",       "", DC_Frac,  "%", xidXSWQuery,       this, isResult|noScenario|noSnap|DDEF_NOCOMPARE);
      DDB.Double("Exec.BuildTm",       "", DC_Frac,  "%", xidXSWBuild,       this, isResult|noScenario|noSnap|DDEF_NOCOMPARE);
      DDB.Double("Exec.ExecTm",        "", DC_Frac,  "%", xidXSWExec,        this, isResult|noScenario|noSnap|DDEF_NOCOMPARE);
      DDB.Double("Exec.ExecDoneTm",    "", DC_Frac,  "%", xidXSWExecDn,      this, isResult|noScenario|noSnap|DDEF_NOCOMPARE);
      DDB.Double("Exec.ReadTm",        "", DC_Frac,  "%", xidXSWRead,        this, isResult|noScenario|noSnap|DDEF_NOCOMPARE);
      DDB.Double("Exec.WriteTm",       "", DC_Frac,  "%", xidXSWWrite,       this, isResult|noScenario|noSnap|DDEF_NOCOMPARE);
      DDB.Double("Exec.StopTm",        "", DC_Frac,  "%", xidXSWStop,        this, isResult|noScenario|noSnap|DDEF_NOCOMPARE);

      //DDB.Text  ("");
      ////DDB.Double("ConvergeTm",    "", DC_Frac,  "%", xidConvergeTm,     this, isResult|noScenario|noSnap|DDEF_NOCOMPARE);
      //DDB.Double("NetSolveTm",    "", DC_Frac,  "%", xidFlwSolveTm,     this, isResult|noScenario|noSnap|DDEF_NOCOMPARE);
      //DDB.Double("FlwSolveTm",    "", DC_Frac,  "%", xidFlwWashTm,      this, isResult|noScenario|noSnap|DDEF_NOCOMPARE);
      //DDB.Double("DiscreteTm",    "", DC_Frac,  "%", xidDiscreteTm,     this, isResult|noScenario|noSnap|DDEF_NOCOMPARE);
      //DDB.Double("CtrlStrategyTm","", DC_Frac,  "%", xidCtrlStrategyTm, this, isResult|noScenario|noSnap|DDEF_NOCOMPARE);
      //DDB.Double("CtrlActionsTm", "", DC_Frac,  "%", xidCtrlActionsTm,  this, isResult|noScenario|noSnap|DDEF_NOCOMPARE);
      DDB.Visibility();

      CString PrevGrp;
      int GrpCnt=0;

      for (int i=0; i<gs_swExecute.GetSize(); i++)
        {
        CGlblStopWatch &SW=*gs_swExecute[i];
        if (SW.Show()&CGlblStopWatch::eShowAccess)
          {
          if (PrevGrp.CompareNoCase(SW.Group())!=0)
            {
            DDB.Text("");
            DDB.Page("...", DDB_OptPage);
            PrevGrp=SW.Group();
            GrpCnt=(SW.Show()&CGlblStopWatch::eShowTotal) ? 0 : -10000;
            }
          DDB.Double((LPTSTR)SW.FullName(), "", DC_Frac,  "%", xidGlblSWExec0+i,  this, isResult|noScenario|noSnap|DDEF_NOCOMPARE);
          GrpCnt++;
          if (GrpCnt>0 && (i==gs_swExecute.GetSize()-1 || _stricmp(PrevGrp, gs_swFlwSolve[i+1]->Group())!=0))
            {
            CString S(SW.Group());
            S+=".Total";
            DDB.Double((LPTSTR)(LPCTSTR)S, "", DC_Frac,  "%", xidGlblSWExecTot0+i,  this, isResult|noScenario|noSnap|DDEF_NOCOMPARE);
            }
          }
        }
      GrpCnt=0;
      for (int i=0; i<gs_swFlwSolve.GetSize(); i++)
        {
        CGlblStopWatch &SW=*gs_swFlwSolve[i];
        if (SW.Show()&CGlblStopWatch::eShowAccess)
          {
          if (PrevGrp.CompareNoCase(SW.Group())!=0)
            {
            DDB.Text("");
            DDB.Page("...", DDB_OptPage);
            PrevGrp=SW.Group();
            GrpCnt=(SW.Show()&CGlblStopWatch::eShowTotal) ? 0 : -10000;
            }
          DDB.Double((LPTSTR)SW.FullName(), "", DC_Frac,  "%", xidGlblSWFlw0+i,  this, isResult|noScenario|noSnap|DDEF_NOCOMPARE);
          GrpCnt++;
          if (GrpCnt>1 && (i==gs_swFlwSolve.GetSize()-1 || _stricmp(PrevGrp, gs_swFlwSolve[i+1]->Group())!=0))
            {
            CString S(SW.Group());
            S+=".Total";
            DDB.Double((LPTSTR)(LPCTSTR)S, "", DC_Frac,  "%", xidGlblSWFlwTot0+i,  this, isResult|noScenario|noSnap|DDEF_NOCOMPARE);
            }
          }
        }

      if (m_bThreadStats)
        DDB.Page("ThreadStats", DDB_RqdPage);
      else
        DDB.Text(" ");
      DDB.CheckBoxBtn("ThreadStatistics", "", DC_, "", &m_bThreadStats, this, isParm);
      if (m_bThreadStats)
        DDB.Text(" ");

      if (m_bThreadStats && !DDB.ForFileSnpScn())
        {
        if (DDB.BeginArray(this, "Thread", "FS_ThStats", 0))
          {
          Strng Tg;
          for (int w=0; w<eFITMaxCmd; w++)
            {
            if (FITVisible[w])
              {
              int nT=gs_ThreadTiming.ThreadCount();
              for (int t=0; t<nT; t++)
                {
                CThreadTimerAccum &A =*gs_ThreadTiming.GetByIndex(t, w);
                Tg.Set("%s.%i", eFITCmdStr(w), t);
                if (t==0)
                  DDB.Page("...", DDB_OptPage);
                if (DDB.BeginElement(this, Tg(), NULL, -1, DDB_NoPage))//t==0?DDB_OptPage:DDB_NoPage))
                  {
                  int x=t*eFITMaxCmd+w;
                  DDB.Long("ThreadId",  "", DC_,      "",   xidThTmThreadId    +  x,  this, InitHidden);
                  DDB.Long("CallSets",  "", DC_,      "",   xidThTmCallSets    +  x,  this, InitHidden);
                  DDB.Long("Calls",     "", DC_,      "",   xidThTmCalls       +  x,  this, 0);
                  DDB.Double("Total",   "", DC_Time,  "us", xidThTmTotal       +  x,  this, 0);
                  DDB.Double("Lock",    "", DC_Time,  "us", xidThTmLockTotal   +  x,  this, 0);
                  DDB.Double("Exec",    "", DC_Time,  "us", xidThTmExecTotal   +  x,  this, 0);
                  DDB.Double("UnLock",  "", DC_Time,  "us", xidThTmUnLockTotal +  x,  this, 0);
                  DDB.Double("Wait",    "", DC_Time,  "us", xidThTmWaitTotal   +  x,  this, 0);
                  }
                }
              }
            }
          }
        DDB.EndArray();
        }
      }
    DDB.EndStruct();

    DDB.Visibility(NM_Dynamic|SM_All|HM_All);
    }
  DDB.EndStruct();

  if (DDB.BeginObject(this, "Globals", "FS_Globals", NULL, DDB_RqdPage))
    {
    for (TagObjClass * pClass=TagObjClass::FirstClass(); (pClass); pClass=pClass->NextClass())
      { 
      fn_BuildDataDefnGlobal pBDD=pClass->GlobalFnBuildDataDefn();
      DDB.Visibility(SM_All|HM_All);
      if (pBDD)
        {
        if ((*pBDD)(pClass, this, DDB))
          {
          }
        }
      };
    }
  DDB.EndObject();
  }

//--------------------------------------------------------------------------

void CFlwSolver::BuildDataDefnOld(DataDefnBlk & DDB)
  {
  if (DDB.BeginStruct(this))
    {

    DDB.Text  ("");
    DDB.Byte  ("StopOnError",      "", DC_, "",   &fStopOnError,    this, isParm, DDBYesNo);
    DDB.Byte  ("KeepGlblLinks",    "", DC_, "",   &fKeepGlblLinks,  this, isParm, DDBYesNo);
    DDB.Byte  ("GlblLinksDone",    "", DC_, "",   &fGlblLinksDone,  this, DDEF_WRITEPROTECT|noFile|noScenario|noSnap, DDBYesNo);
    DDB.Long  ("NoOfProcessors",   "", DC_, "",   &nProcessors,     this, DDEF_WRITEPROTECT|noFile|noScenario|noSnap);
    DDB.Long  ("NoOfSolveThreads", "", DC_, "",   &nRqdThreads,     this, isParmStopped);
    DDB.Text  ("");

    DDB.Page("Atmosphere", DDB_RqdPage);
    gs_Environment.BuildDataDefn(this, "Atmos", DDB);

    PB.BuildDataDefnOld(DDB);
    Dyn.BuildDataDefnOld(DDB);

    Strng Commnt, Nm;
    int i=0,iPg=0;
    TearVarBlkIter TCBs(TearVarBlk::List);
    for (TearVarBlk *pT=TCBs.First(); pT; pT=TCBs.Next(), i++)
      if (!pT->IsGroupBlk())
        {
        if ((i)%25==0)
          {
          Nm.Set("Tears%i", iPg++);
          DDB.Page(Nm(), DDB_RqdPage);
          }
        Nm.Set("T_%02i", i);

        DDBValueLst DDB0[] = {{ 0, pT->Tag()}, {1,  "Busy..."}, {0}};
        DDB.Button(Nm(),   "",  DC_ , "",   xidATearTag,   this, isParm|isTag, DDB0);
        }
    }
  DDB.EndStruct();
  };

//--------------------------------------------------------------------------

flag CFlwSolver::DataXchg(DataChangeBlk & DCB)
  {
  if (gs_Environment.DataXchg(this, DCB))
    return 1;

  if (FlwNode::DataXchg(DCB))
    return 1;
  if (PB.DataXchg(DCB))
    return 1;
  if (Dyn.DataXchg(DCB))
    return 1;

  for (TagObjClass * pClass=TagObjClass::FirstClass(); (pClass); pClass=pClass->NextClass())
    { 
    fn_DataXchgGlobal pDX=pClass->GlobalFnDataXchg();
    if (pDX && (*pDX)(pClass, this, DCB))
      return 1;
    };

  switch (DCB.lHandle)
    {
    case xidH2OProperties:
      if (DCB.rL)
        {
        if (PrjFileVerNo()<41)
          CDB.SelectH2OProperties(IF97H2OName);
        else
          CDB.SelectH2OProperties(*DCB.rL);
        if (strcmp(CDB.H2OPropertiesSelectedClassId(), IF97H2OName)==0)
          {//for now, force smoothing & surfaces ON for this method
//          CSurfaceCommon::m_DoSmoothing=1;
//          CSurfaceCommon::m_UseSurface=1;
          }
        }
      DCB.L=CDB.H2OPropertiesSelected();
      return 1;
    case xidUseSurfaces:
      if (DCB.rB)
        {
        CSurfaceCommon::m_UseSurface=(*DCB.rB!=0);
//        CSurfaceCommon::m_DoSmoothing=CSurfaceCommon::m_UseSurface;
        }
      DCB.B=CSurfaceCommon::m_UseSurface;
      return 1;
    case xidDoSmoothing:
      if (DCB.rB)
        {
        CSurfaceCommon::m_DoSmoothing=(*DCB.rB!=0);
//        CSurfaceCommon::m_UseSurface=CSurfaceCommon::m_DoSmoothing;
        }
      DCB.B=CSurfaceCommon::m_DoSmoothing;
      return 1;

    case xidIncludeSpcCnvs:
      if (DCB.rB)
        gs_WithSpecieCnvs=(*DCB.rB!=0);
      DCB.B=gs_WithSpecieCnvs;
      return 1;
    case xidShowSpcRangeErrors:
      if (DCB.rB)
        SpModel::m_RangeErrorsOn=(*DCB.rB!=0);
      DCB.B=SpModel::m_RangeErrorsOn;
      return 1;
    case xidShowSpcXRangeErrors:
      if (DCB.rB)
        SpModel::m_RangeExtErrorsOn=(*DCB.rB!=0);
      DCB.B=SpModel::m_RangeExtErrorsOn;
      return 1;

    case xidSetStateAll:
      if (DCB.rB)
        {
        CWaitCursor Wait;
        Strng_List SL;
        gs_Exec.SetModelState((eScdMdlStateActs)*DCB.rB, SL);
        }
      DCB.B=0;
      return 1;

    case xidSetStateAllZeroFlows:
      if (DCB.rB && (*DCB.rB!=0))
        {
        CWaitCursor Wait;
        Strng_List SL;
        gs_Exec.SetModelState(MSA_ZeroFlows, SL);
        }
      DCB.B=0;
      return 1;
    case xidSetStateAllEmpty:
      if (DCB.rB && (*DCB.rB!=0))
        {
        CWaitCursor Wait;
        Strng_List SL;
        gs_Exec.SetModelState(MSA_Empty, SL);
        }
      DCB.B=0;
      return 1;
    case xidSetStateAllPreSet:
      if (DCB.rB && (*DCB.rB!=0))
        {
        CWaitCursor Wait;
        Strng_List SL;
        gs_Exec.SetModelState(MSA_PreSet, SL);
        }
      DCB.B=0;
      return 1;
    case xidSetStateAllSteadyState:
      if (DCB.rB && (*DCB.rB!=0))
        {
        CWaitCursor Wait;
        Strng_List SL;
        gs_Exec.SetModelState(MSA_SteadyState, SL);
        }
      DCB.B=0;
      return 1;
    case xidSetStateAllPBInit:
      if (DCB.rB && (*DCB.rB!=0))
        {
        CWaitCursor Wait;
        Strng_List SL;
        gs_Exec.SetModelState(MSA_PBInit, SL);
        }
      DCB.B=0;
      return 1;
    case xidSetStateAllDynStatsRunInit:
      if (DCB.rB && (*DCB.rB!=0))
        {
        CWaitCursor Wait;
        Strng_List SL;
        gs_Exec.SetModelState(MSA_DynStatsRunInit, SL);
        }
      DCB.B=0;
      return 1;
    case xidSetStateAllEmptySpillTargets:
      if (DCB.rB && (*DCB.rB!=0))
        {
        CWaitCursor Wait;
        Strng_List SL;
        gs_Exec.SetModelState(MSA_EmptySpillTargets, SL);
        }
      DCB.B=0;
      return 1;

    case xidNThreads:
      DCB.L=NRqdThreads();
      return 1;
    case xidRqdThreads:
      if (DCB.rL)
        {
        nRqdThreads=(*DCB.rL);
        gs_ThreadTiming.Initialise(NRqdThreads());
        }
      DCB.L=nRqdThreads;
      return 1;

    case xidDIOSpillsVents:
      if (DCB.rL)
        {
        CDirectFlwIO::sm_ConnectVentSpills=(eDIOConnectSelects)*DCB.rL;
        UnDoGlobalLinks();
        }
      DCB.L=CDirectFlwIO::sm_ConnectVentSpills;
      return 1;
    case xidDIOLeaks:
      if (DCB.rL)
        {
        CDirectFlwIO::sm_ConnectLeaks=(eDIOConnectSelects)*DCB.rL;
        UnDoGlobalLinks();
        }
      DCB.L=CDirectFlwIO::sm_ConnectLeaks;
      return 1;
    case xidDIOAudit:
      if (DCB.rL)
        {
        CDirectFlwIO::sm_ConnectAudit=(eDIOConnectSelects)*DCB.rL;
        UnDoGlobalLinks();
        }
      DCB.L=CDirectFlwIO::sm_ConnectAudit;
      return 1;
    case xidPGMExecTimeOut:
      if (DCB.rD)
        GCInsMngr::sm_ExecTimeout=Range(1.0, *DCB.rD, 1000.0);
      DCB.D=GCInsMngr::sm_ExecTimeout;
      return 1;

    case xidRandomSeed:
      if (DCB.rD)
        {
        m_RandomSeed = *DCB.rD;
        if (Valid(m_RandomSeed))
          m_RandomSeed = -1.0 * abs(m_RandomSeed);
        }
      DCB.D=m_RandomSeed;
      return 1;

    case xidExecState: DCB.L=gs_Exec.StateIndex(); return 1;
    case xidExecStateDesc: DCB.pC=gs_Exec.StateDesc(); return 1;
    case xidExecStart:
      if (DCB.rB && (*DCB.rB!=0))
        gs_TheRunMngr.GotoRunMode(CExecutive::RRM_Current, -1);
        //ScdMainWnd()->PostMessage(WMU_EXECACTION, SUB_EXECACTION_START, (LPARAM)0);
      DCB.B=0;
      return 1;
    case xidExecIdle:
      if (DCB.rB && (*DCB.rB!=0))
        gs_TheRunMngr.GotoIdleMode();
        //ScdMainWnd()->PostMessage(WMU_EXECACTION, SUB_EXECACTION_IDLE, (LPARAM)0);
      DCB.B=0;
      return 1;
    case xidExecStop:
      if (DCB.rB && (*DCB.rB!=0))
        gs_TheRunMngr.GotoEditMode();
        //ScdMainWnd()->PostMessage(WMU_EXECACTION, SUB_EXECACTION_STOP, (LPARAM)0);
      DCB.B=0;
      return 1;

    case xidStartupDirectory: DCB.pC=::StartupDirectory() ; return 1;
    case xidStartupDrive    : DCB.pC=::StartupDrive()     ; return 1;
    case xidProgFiles       : DCB.pC=::ProgFiles()        ; return 1;
    case xidTemporaryFiles  : DCB.pC=::TemporaryFiles()   ; return 1;
    case xidBaseCfgFiles    : DCB.pC=::BaseCfgFiles()     ; return 1;
    case xidCfgName         : DCB.pC=::CfgName()          ; return 1;
    case xidCfgFile         : DCB.pC=::CfgFile()          ; return 1;
    case xidCfgFiles        : DCB.pC=::CfgFiles()         ; return 1;
    case xidPrjName         : DCB.pC=::PrjName()          ; return 1;
    case xidPrjFile         : DCB.pC=::PrjFile()          ; return 1;
    case xidPrjFiles        : DCB.pC=::PrjFiles()         ; return 1;
    case xidPrjRmtCpyFiles  : DCB.pC=::PrjRmtCpyFiles()   ; return 1;
    case xidPrjIniFile      : DCB.pC=::PrjIniFile()       ; return 1;

    case xidConditionCnt    :
      {
      DCB.L=0;
      FlwNode*p=NULL;
      while (GetObj_All(p))
        if (p->GetActive())
          DCB.L+=p->CountAllCIs(1000);
      };
      return 1;

    case xidXSWSteps        : DCB.L=TimeCB().m_StepCount; return 1;//XSW().nStepCnt; return 1;
    case xidSolveIters      : DCB.L=NetProbalMethod() ? PBEqnCB().NIters() : ODE().GetGoodIters(); return 1;
    case xidSolveItersBad   : DCB.L=NetProbalMethod() ? 0 : ODE().GetBadIters(); return 1;
    case xidXSWSecsStart    : DCB.D=XSW().m_dStartingTime; return 1;
    case xidXSWSecs         : DCB.D=XSW().m_swRun.Secs(); return 1;
    case xidXSWSecsDesc     : DCB.pC=(LPTSTR)XSW().m_swRun.MinSecHunDesc(SecsBuff); return 1;
    case xidXSWSimSecs      : DCB.D=XSW().m_dSimulatedTime.Seconds; return 1;
    case xidXSWSimSecsDesc  : DCB.pC=(LPTSTR)(LPCTSTR)(XSW().m_dSimulatedTime.Format(TD_Time|TD_Days)); return 1;
    case xidXSWStepI        : DCB.D=XSW().StepTimeInst(); return 1;
    case xidXSWStepMA       : DCB.D=XSW().StepTimeMvAvg(); return 1;
    case xidXSWStepA        : DCB.D=XSW().StepTimeAvg(); return 1;
    case xidXSWRunSpdI      : DCB.D=XSW().MultiplierInst(); return 1;
    case xidXSWRunSpdMA     : DCB.D=XSW().MultiplierMvAvg(); return 1;
    case xidXSWRunSpdA      : DCB.D=XSW().MultiplierAvg(); return 1;
    case xidXSWProcUseI     : DCB.D=XSW().ProcUseInst(); return 1;
    case xidXSWProcUseMA    : DCB.D=XSW().ProcUseMvAvg(); return 1;
    case xidXSWProcUseA     : DCB.D=XSW().ProcUseAvg(); return 1;
    case xidXSWMVAvgPts     : if (DCB.rL)
                                {
                                XSW(false).m_nMvAvgPts=Range(1L,*DCB.rL, MaxMvAvgPts);
                                XSW(false).m_nMvAvgCnt=0;
                                }
                              DCB.L=XSW(false).m_nMvAvgPts;
                              return 1;

    case xidXSWStart        : DCB.D=XSW().m_swStart.Secs()/GTZ(XSW().TotalSecs());  return 1;
    case xidXSWQuery        : DCB.D=XSW().m_swQuery.Secs()/GTZ(XSW().TotalSecs());  return 1;
    case xidXSWBuild        : DCB.D=XSW().m_swBuild.Secs()/GTZ(XSW().TotalSecs());  return 1;
    case xidXSWExec         : DCB.D=XSW().m_swExec.Secs()/GTZ(XSW().TotalSecs());   return 1;
    case xidXSWExecDn       : DCB.D=XSW().m_swExecDn.Secs()/GTZ(XSW().TotalSecs()); return 1;
    case xidXSWRead         : DCB.D=XSW().m_swRead.Secs()/GTZ(XSW().TotalSecs());   return 1;
    case xidXSWWrite        : DCB.D=XSW().m_swWrite.Secs()/GTZ(XSW().TotalSecs());  return 1;
    case xidXSWStop         : DCB.D=XSW().m_swStop.Secs()/GTZ(XSW().TotalSecs());   return 1;

    case xidPMPageFaultCount             :  {  PROCESS_MEMORY_COUNTERS PM;  PM.cb=sizeof(PM);  GetProcessMemoryInfo(GetCurrentProcess(), &PM, sizeof(PM)); DCB.D=PM.PageFaultCount;              return 1; }
    case xidPMPeakWorkingSetSize         :  {  PROCESS_MEMORY_COUNTERS PM;  PM.cb=sizeof(PM);  GetProcessMemoryInfo(GetCurrentProcess(), &PM, sizeof(PM)); DCB.D=PM.PeakWorkingSetSize;          return 1; }
    case xidPMWorkingSetSize             :  {  PROCESS_MEMORY_COUNTERS PM;  PM.cb=sizeof(PM);  GetProcessMemoryInfo(GetCurrentProcess(), &PM, sizeof(PM)); DCB.D=PM.WorkingSetSize;              return 1; }
    case xidPMQuotaPeakPagedPoolUsage    :  {  PROCESS_MEMORY_COUNTERS PM;  PM.cb=sizeof(PM);  GetProcessMemoryInfo(GetCurrentProcess(), &PM, sizeof(PM)); DCB.D=PM.QuotaPeakPagedPoolUsage;     return 1; }
    case xidPMQuotaPagedPoolUsage        :  {  PROCESS_MEMORY_COUNTERS PM;  PM.cb=sizeof(PM);  GetProcessMemoryInfo(GetCurrentProcess(), &PM, sizeof(PM)); DCB.D=PM.QuotaPagedPoolUsage;         return 1; }
    case xidPMQuotaPeakNonPagedPoolUsage :  {  PROCESS_MEMORY_COUNTERS PM;  PM.cb=sizeof(PM);  GetProcessMemoryInfo(GetCurrentProcess(), &PM, sizeof(PM)); DCB.D=PM.QuotaPeakNonPagedPoolUsage;  return 1; }
    case xidPMQuotaNonPagedPoolUsage     :  {  PROCESS_MEMORY_COUNTERS PM;  PM.cb=sizeof(PM);  GetProcessMemoryInfo(GetCurrentProcess(), &PM, sizeof(PM)); DCB.D=PM.QuotaNonPagedPoolUsage;      return 1; }
    case xidPMPagefileUsage              :  {  PROCESS_MEMORY_COUNTERS PM;  PM.cb=sizeof(PM);  GetProcessMemoryInfo(GetCurrentProcess(), &PM, sizeof(PM)); DCB.D=PM.PagefileUsage;               return 1; }
    case xidPMPeakPagefileUsage          :  {  PROCESS_MEMORY_COUNTERS PM;  PM.cb=sizeof(PM);  GetProcessMemoryInfo(GetCurrentProcess(), &PM, sizeof(PM)); DCB.D=PM.PeakPagefileUsage;           return 1; }

    case xidSMMemoryLoad      :  {  MEMORYSTATUS MS;  MS.dwLength=sizeof(MS);  GlobalMemoryStatus(&MS); DCB.D=MS.dwMemoryLoad*0.01     ;  return 1; }
    case xidSMTotalPhys       :  {  MEMORYSTATUS MS;  MS.dwLength=sizeof(MS);  GlobalMemoryStatus(&MS); DCB.D=MS.dwTotalPhys           ;  return 1; }
    case xidSMAvailPhys       :  {  MEMORYSTATUS MS;  MS.dwLength=sizeof(MS);  GlobalMemoryStatus(&MS); DCB.D=MS.dwAvailPhys           ;  return 1; }
    case xidSMTotalPageFile   :  {  MEMORYSTATUS MS;  MS.dwLength=sizeof(MS);  GlobalMemoryStatus(&MS); DCB.D=MS.dwTotalPageFile       ;  return 1; }
    case xidSMAvailPageFile   :  {  MEMORYSTATUS MS;  MS.dwLength=sizeof(MS);  GlobalMemoryStatus(&MS); DCB.D=MS.dwAvailPageFile       ;  return 1; }
    case xidSMTotalVirtual    :  {  MEMORYSTATUS MS;  MS.dwLength=sizeof(MS);  GlobalMemoryStatus(&MS); DCB.D=MS.dwTotalVirtual        ;  return 1; }
    case xidSMAvailVirtual    :  {  MEMORYSTATUS MS;  MS.dwLength=sizeof(MS);  GlobalMemoryStatus(&MS); DCB.D=MS.dwAvailVirtual        ;  return 1; }

    case xidSolveUnknowns   : DCB.L=NetProbalMethod() ? PBEqnCB().NUnknowns() : 0; return 1;
    case xidSolveBadError   : DCB.L=NetProbalMethod() ? PBEqnCB().NBadError() : 0; return 1;
    case xidSolveBadLimit   : DCB.L=NetProbalMethod() ? PBEqnCB().NBadLimit() : 0; return 1;
    case xidSolveWorstErr   : DCB.D=NetProbalMethod() ? PBEqnCB().TheWorstErr() : 0.0; return 1;
    //case xidSolveWorstErr   : DCB.D=NetProbalMethod() ? Max(PBEqnCB().WorstOtherErr(), PBEqnCB().WorstErr()) : 0.0; return 1;//(PBEqnCB().Converged() ? PBEqnCB().WorstOtherErr() : PBEqnCB().WorstErr()) : 0.0; return 1;

    case xidSolveNets       : DCB.L=NetDynamicMethod() ? Dyn.FlwNets.GetNumNets():       0; return 1;
    case xidSolveSubNets    : DCB.L=NetDynamicMethod() ? Dyn.FlwNets.GetNumSubNets(-1):  0; return 1;
    case xidSolveNodes      : DCB.L=NetDynamicMethod() ? Dyn.FlwNets.GetNumNodes(-1):    0; return 1;
    case xidSolveFxdLnks    : DCB.L=NetDynamicMethod() ? Dyn.FlwNets.GetNumFxdLnks(-1):  0; return 1;
    case xidSolveLoneLnks   : DCB.L=NetDynamicMethod() ? Dyn.FlwNets.GetNumLoneLnks(-1): 0; return 1;
    case xidSolveNetLnks    : DCB.L=NetDynamicMethod() ? Dyn.FlwNets.GetNumNetLnks(-1):  0; return 1;

    //case xidConvergeTm      : DCB.D=NetDynamicMethod() && (XSW().TotalSecs()>1e-6) ? Dyn.m_swCnvState.TimeFrac() : 0;  return 1;
    //case xidFlwSolveTm      : DCB.D=NetDynamicMethod() && (XSW().TotalSecs()>1e-6) ? Dyn.m_swImpNetSolve.TimeFrac() : 0;  return 1;
    //case xidFlwWashTm       : DCB.D=NetDynamicMethod() && (XSW().TotalSecs()>1e-6) ? Dyn.m_swImpFlwSolve.TimeFrac() : 0;  return 1;
    //case xidDiscreteTm      : DCB.D=NetDynamicMethod() && (XSW().TotalSecs()>1e-6) ? Dyn.m_swDiscrete.TimeFrac() : 0;  return 1;
    //case xidCtrlStrategyTm  : DCB.D=NetDynamicMethod() && (XSW().TotalSecs()>1e-6) ? Dyn.m_swCtrlStrategy.TimeFrac() : 0;  return 1;
    //case xidCtrlActionsTm   : DCB.D=NetDynamicMethod() && (XSW().TotalSecs()>1e-6) ? Dyn.m_swCtrlActions.TimeFrac() : 0;  return 1;

    case xidGenVerStr       : DCB.pC=FullVersion(); return 1;
    case xidGenUpdateVerStr : DCB.pC=FullVersionAndUpdate(); return 1;
    case xidGenMajorBuildNo : DCB.L=MajorBuildNumber(); return 1;
    case xidGenBuildNo      : DCB.L=BuildNumber(); return 1;
    case xidGenBuildDate    : DCB.pC=BuildDate(); return 1;
    case xidGenDateAndTime  : DCB.pC=CurDateTime(SecsBuff); return 1;
    case xidGenTime1970     : DCB.D=GetCurrentTime_1970().Seconds; return 1;

    case xidGrfCount        : DCB.L=gs_GrfTagGroups.Count(); return 1;

    case xidNormT           : DCB.D=NormTemp(); return 1;
    case xidNormP           : DCB.D=NormPress(); return 1;
    case xidStdT            : DCB.D=StdTemp(); return 1;
    case xidStdP            : DCB.D=StdPress(); return 1;

    case xidATearTag:
      DCB.pC="";  // Ignore
      return 1;
    case xidDefaultArea:
      if (DCB.rpC)
        gs_DirectFlwIOTagDefault=DCB.rpC;
      DCB.pC=gs_DirectFlwIOTagDefault();
      return 1;
    //case xidMissSpillAreas:
    //  {
    //  static Strng S;
    //  S="";
    //  int i=0;
    //  for (Strng *p=gs_DirectFlwIOTagsMissing.First(); p!=NULL; p=gs_DirectFlwIOTagsMissing.Next())
    //    {
    //    if (i++>0)
    //      S+="\n";
    //    S+=p->Str();
    //    }
    //  DCB.pC=S.Length() > 0 ? S() : " ";
    //  return 1;
    //  }
    case xidResultsValid:
      if (DCB.rB)
        {
        gs_Exec.XGlblResultValidity(*DCB.rB);
        iLclResultsValid=*DCB.rB ? 1 : 0;
        }
      DCB.B=TaggedObject::GlblResultsValid();
      return 1;
    default:
      {
      if (DCB.lHandle>=xidGrfFirstActive && DCB.lHandle<=xidGrfLastActive)
        {
        int index = (DCB.lHandle-xidGrfFirstActive)/2;
        switch ((DCB.lHandle-xidGrfFirstActive)%2)
          {
          case 0:
            DCB.pC=gs_GrfTagGroups.GetName(index);
            return 1;
          case 1:
            if (DCB.rB)
              {
              gs_GrfTagGroups.SetActive(index, *DCB.rB!=0);
              }
            DCB.B=gs_GrfTagGroups.IsActive(index);
            return 1;
          }
        }
      if (DCB.lHandle>=xidGlblSWExec0 && DCB.lHandle<xidGlblSWExec0+500)
        {
        DCB.D=gs_swExecute.TimeFrac(DCB.lHandle-xidGlblSWExec0);
        return 1;
        }
      if (DCB.lHandle>=xidGlblSWExecTot0 && DCB.lHandle<xidGlblSWExecTot0+500)
        {
        DCB.D=gs_swExecute.TotalTimeFrac(DCB.lHandle-xidGlblSWFlwTot0);
        return 1;
        }
      if (DCB.lHandle>=xidGlblSWFlw0 && DCB.lHandle<xidGlblSWFlw0+500)
        {
        DCB.D=gs_swFlwSolve.TimeFrac(DCB.lHandle-xidGlblSWFlw0);
        return 1;
        }
      if (DCB.lHandle>=xidGlblSWFlwTot0 && DCB.lHandle<xidGlblSWFlwTot0+500)
        {
        DCB.D=gs_swFlwSolve.TotalTimeFrac(DCB.lHandle-xidGlblSWFlwTot0);
        return 1;
        }
      if (DCB.lHandle>=xidThTmThreadId && DCB.lHandle<xidThTmLast)
        {
        int x=DCB.lHandle-xidThTmThreadId;
        int c=(x/ThTmSpacing)*ThTmSpacing;
        int t=(x-c)/eFITMaxCmd;
        int w=(x-c)%eFITMaxCmd;
        CThreadTimerAccum &A=*gs_ThreadTiming.GetByIndex(t,w);
        switch (c+xidThTmThreadId)
          {
          case xidThTmThreadId    : DCB.L=A.m_bValid ? A.m_ThreadId     : -1;  return 1;
          case xidThTmCallSets    : DCB.L=A.m_bValid ? A.m_nCallSets    : -1;  return 1;
          case xidThTmCalls       : DCB.L=A.m_bValid ? A.m_nCalls       : -1;  return 1;
          case xidThTmTotal       : DCB.D=A.m_bValid ? A.m_Total        : dNAN;  return 1;
          case xidThTmLockTotal   : DCB.D=A.m_bValid ? A.m_LockTotal    : dNAN;  return 1;
          case xidThTmExecTotal   : DCB.D=A.m_bValid ? A.m_ExecTotal    : dNAN;  return 1;
          case xidThTmUnLockTotal : DCB.D=A.m_bValid ? A.m_UnLockTotal  : dNAN;  return 1;
          case xidThTmWaitTotal   : DCB.D=A.m_bValid ? A.m_WaitTotal    : dNAN;  return 1;
          }
        }
      }
    }
  return 0;
  }

//--------------------------------------------------------------------------

flag CFlwSolver::ValidateData(ValidateDataBlk & VDB)
  {//Note: This is not called at EO startup because CFlwSolver is not a "normal active node"
  flag OK=PB.ValidateData(VDB);
  nRqdThreads=Range(-1L, nRqdThreads, nProcessors+4L);
#if FORCERQDTHREADS
  nRqdThreads = 0;
#endif
  OK=OK && FlwNode::ValidateData(VDB);
  if (!gs_Environment.ValidateData(VDB))
    OK=false;

  return OK;
  }

//--------------------------------------------------------------------------

void CFlwSolver::SetState(eScdMdlStateActs RqdState)
  {
  FlwNode::SetState(RqdState);
  switch (RqdState)
    {
    /*case MSA_PBInit:
    case MSA_ZeroFlows:
    case MSA_Empty:
    case MSA_PreSet:
    case MSA_SteadyState:
      break;*/
    case MSA_DynStatsRunInit:
      //__debugbreak(); // to restore
      //Dyn.DynStats.ResetStats();
      //gs_EventLog.StartFile(true);
      if (1)
        {
        if (Valid(m_RandomSeed))
          {
          srand((long)(m_RandomSeed));
          CBaseNoise::SetGlobalNoiseSeed((long)(m_RandomSeed));
          }
        else
          {
          unsigned int s = (unsigned)time(NULL);
          srand(s);
          CBaseNoise::SetGlobalNoiseSeed(s);
          }
        }
      break;
    }
  }

//--------------------------------------------------------------------------

int CFlwSolver::FilesUsed(CFilesUsedArray & Files)
  {
  int Cnt = 0;
  Strng File(gs_Exec.HstTaglistFile);
  if (File.Len()>0)
    {
    Strng Fn(gs_Exec.HstTaglistFolder);
    Fn.FnExpand();
    Fn += File();
    Files.AddFile(Fn(), FU_CopyFile|FU_EditTxt);
    Cnt++;
    }
  return Cnt;
  }

//--------------------------------------------------------------------------

long CFlwSolver::NRqdThreads()
  {
  #if (dbgFlwSolve)
  if (dbgThreadsForceOff())
    return 0;
  #endif
  //TODO Sort out Hyper Threading Processor Count
  return (nRqdThreads < 0 ? ((nProcessors>1) ? nProcessors : 0) : nRqdThreads);
  };

//--------------------------------------------------------------------------

int CFlwSolver::EO_GetSolutionError(EqnSlvCtrlBlk & EQCtrl, Strng & RptLine)
  {
  return PB.GetSolutionError(EQCtrl, RptLine);
  }

//--------------------------------------------------------------------------

void CFlwSolver::Set_All_Zs()
  {

  for (FlwNode * p=NULL; GetObj_All(p); )
    p->m_ZState=Z_Unknown;

  CFlwNodeIndexList List;
  // Find Known Nodes
  for (FlwNode * p=NULL; GetObj_All(p); )
    {
    if (p->GetActive())
      p->SetDatums(0, List, -1);
    }

  // Process from here
  while (!List.IsEmpty())
    {
    CFlwNodeIndex X=List.RemoveHead();
    X.m_pNd->SetDatums(2, List, X.m_IOIn);
    }

  // Find Remaining UnKnown Nodes
  for (FlwNode * p=NULL; GetObj_All(p); )
    {
    if (p->GetActive())
      p->SetDatums(1, List, -1);
    }

  // Process from here
  while (!List.IsEmpty())
    {
    CFlwNodeIndex X=List.RemoveHead();
    X.m_pNd->SetDatums(2, List, X.m_IOIn);
    }

  for (FlwNode * p=NULL; GetObj_All(p); )
    {
    if (p->GetActive())
      p->SetDatumsDone();
    }

  p=NULL;
  while (GetObj_All(p))
    {
    if (p->GetActive())
      {
      for (int i=0; i<p->NoFlwIOs(); i++)
        {
        if (p->IOFlange(i) && !Valid(p->IODatum_Flng(i)))
          dbgpln("Invalid CFlange Datum %s > %s", p->Tag(), p->Nd_Rmt(i)->Tag());
        }
      }
    }
  }

// --------------------------------------------------------------------------

void CFlwSolver::Set_All_Sizes()
  {
  #if (dbgFlwSolve)
  if (dbgSetZsValue() || dbgSetZsTrace())
    dbgpln("Set_Sizes================================");
  #endif
  FlwNode * p=NULL;
  while (GetObj_All(p))
    if (p->GetActive())
      p->Set_Sizes();
  }

//==========================================================================
//
//
//
//==========================================================================


static bool GlobalLinksBusy() { return (gs_Exec.Running() && !gs_Exec.Starting() && !gs_Exec.BetweenSteps()); }

void CFlwSolver::DoGlobalLinks(int DoingStart)
  {
  if (GlobalLinksBusy())
    return;

  if (m_fHoldDoGlobalLinks)
    return;

  if (fKeepGlblLinks)// ForceIt)
    {
    if (fGlblLinksDone)
      return;

    #if (dbgFlwSolve)
    if (dbgGlobalLinks())
      dbgpln("DoGlobalLinks - Start ---------------------------------");
    #endif

    STARTPROGRESS;
    SHOWPROGRESS(1, "DoGlobalLinks");
    
    // First Remove Bad Ones
    FlwNode *p=NULL;
    FlwNode *pDel=NULL;
    GetObj_All(p);
    while (p)
      {
      p->m_fGlobalConnectBusy=true;
      
      for (int io=0; io<p->NoIOs(); io++)
        {
        if (p->m_dwType & (nc_MLnk))
          {
          if (p->NoProcLnkIOs()<2 || p->Nd_Rmt(io)==NULL) // allow for spill
            goto DeleteIt;
          }
        else if (p->m_dwType & (nc_ELnk|nc_CLnk|nc_ALnk))
          {
          if (p->NoIOs()<2 || p->Nd_Rmt(io)==NULL)
            goto DeleteIt;
          }
        else if (p->Nd_Rmt(io)==NULL)
        if (p->Nd_Rmt(io)==NULL)
          p->Detach(io--);
        }
      GetObj_All(p);
      continue;

DeleteIt:
      pDel=p;
      GetObj_All(p);
      LogError(pDel->Tag(), 0, "Missing connections - Deleted");
      pDel->DisConnectAll();
      delete pDel;
      }

    SHOWPROGRESS(1, "DoGlobalLinks 1");

    p=NULL;
    while (GetObj_All(p))
      {
      if (p->GetActive())
        {
        if (p->NetMethod()==0)
          LogWarning(Tag(), 0, "No Allowed NetMethod %s", ClassId());
        if (p->SolveMethod()==0)
          LogWarning(Tag(), 0, "No Allowed SolveMethod %s", ClassId());
        if (p->HeatMethod()==0)
          LogWarning(Tag(), 0, "No Allowed HeatMethod %s", ClassId());
    
        //p->SetFlowMode(p->FlowMode()); // update FlowMode requirements
        //double t0=SW.Secs();
        p->GlobalConnect();
        //dbgpln("  GlobalConnect %10.1fus %-20s %s", (SW.Secs()-t0)*1e6, p->ClassId(), p->FullObjTag());
        }
      }

    SHOWPROGRESS(1, "DoGlobalLinks 2");

    p=NULL;
    while (GetObj_All(p))
      if (p->GetActive())
        p->PostGlobalConnect();

    if (DefNetProbalMode())
      PB.BuildMacroMdls();

    SHOWPROGRESS(1, "DoGlobalLinks 3");

    EqnSlv().ConnectAllTears(false);
    EqnSlv().RemoveUnusedTears();

    SHOWPROGRESS(1, "DoGlobalLinks 4");

    p=NULL;
    while (GetObj_All(p))
      {
      p->m_fValidateDataComplete=false;
      p->m_fRevalidate=false;

      ValidateDataBlk VDB(false);
      bool OK=(p->ValidateDefinedData(VDB)!=0);
      p->SetCI(17, !OK);

      if (!p->m_fValidateDataComplete && !p->bHoldValidateData)
        LogError(p->FullObjTag(), 0, "Incomplete ValidateData");
      }

    SHOWPROGRESS(1, "DoGlobalLinks 5");

    if (1)
      {
      // Propagate FlowModes
      p=NULL;
      while (GetObj_All(p))
        {
        if ((p->m_FlowModeRqd & LFM_Propagate)==0)
          p->m_FlowModeRqd=0;
        p->m_FlowModePass=0;
        }
      p=NULL;
      //dbgpln("RESTORE PropagateNetInfo !!!!!!!!!!!");
      if (01)
        {
        while (GetObj_All(p))
          {
          if (p->m_FlowModePass==0)
            {
            if (p->m_FlowModeRqd != 0)
              {
              CPropagateNetInfoCtrl Ctrl(eScdGetFlowMode, 0);
              p->PropagateNetInfo(Ctrl, -1);
              if (Ctrl.m_ReqdMethod!=0)
                {

                Ctrl.m_Task=eScdSetFlowMode;
                Ctrl.m_Pass++;

                p->PropagateNetInfo(Ctrl, -1);
                }
              }
            else
              {
              //p->SetxFlowMode(p->FlowMode(), p->m_FlowModeObj); // update FlowMode requirements down to FB's etc
              }
            }
          }
        }
      }

    SHOWPROGRESS(1, "DoGlobalLinks 6");

    Set_All_Sizes();
    TIMINGLAP("SetAllSizes");

    SHOWPROGRESS(1, "DoGlobalLinks 7");

    Set_All_Zs();
    TIMINGLAP("SetAllZ");

    SHOWPROGRESS(1, "DoGlobalLinks 8");


    fGlblLinksDone=true;
  
    if (1)
      {
      p=NULL;
      while (GetObj_All(p))
        {
        p->m_fGlobalConnectBusy=false;
        p->m_fValidateDataComplete=false;
        if (p->m_fRevalidate)
          {
          ValidateDataBlk VDB(false);
          bool OK=(p->ValidateDefinedData(VDB)!=0);
          p->SetCI(17, !OK);

          if (!p->m_fValidateDataComplete && !p->bHoldValidateData)
            LogError(p->FullObjTag(), 0, "Incomplete ValidateData");
          }
        p->m_fRevalidate=false;
        }
      }

    //gs_pTheSFELib->SetLoadAllModelStatus();
    //gs_pTheSFELib->SetLoadAllModelAssocGrfs();

    XUpdateDisplays(true, false);
    SHOWPROGRESS(1, "GlobalLinksDone");

    #if (dbgFlwSolve)
    if (dbgGlobalLinks())
      dbgpln("DoGlobalLinks - Done  ---------------------------------");
    #endif
    }
  }

//--------------------------------------------------------------------------

void CFlwSolver::UnDoGlobalLinks()
  {
  if (!fGlblLinksDone)
    return;

  m_lUnDoGlobalLinksCount=(m_lUnDoGlobalLinksCount+1)%(LONG_MAX-10);

  if (GlobalLinksBusy())
    return;

  #if (dbgFlwSolve)
  if (dbgGlobalLinks())
    dbgpln("UnDoGlobalLinks - Start ---------------------------------");
  #endif


  STARTPROGRESS;
  SHOWPROGRESS(10, "UnDoGlobalLinks 1");

  EqnSlv().DisConnectAllTears();

  PB.DeleteMacroMdls();

  PB.m_TSCA.TerminateCollection();
  Dyn.m_TSCA.TerminateCollection();
  //TSCA.TerminateCollection();

  SHOWPROGRESS(1, "UnDoGlobalLinks 2");
  
  PB.ClearSolutionBlks(true);
  Dyn.ClearSolutionBlks(true);

  SHOWPROGRESS(1, "UnDoGlobalLinks 3");

  int NFlwNodes=0;
  for (FlwNode * p=NULL; GetObj_All(p); )
    NFlwNodes++;


  m_TempNdPtrs.SetSize(Max(m_TempNdPtrs.GetSize(), NFlwNodes));

  int iNd=0;
  for (FlwNode * p=NULL; GetObj_All(p); )
    m_TempNdPtrs[iNd++]=p;

  // this in done in reverse order to improve speed
  //double S0=SW.Secs();
  while (iNd>0)
    {
    p=m_TempNdPtrs[--iNd];

    //dbgpln("Disconnect %s", p->Tag());

    p->GlobalDisConnect();
    //double S1=SW.Secs();
    ////dbgpln("  %10.1fus %-20s %s ", (S1-S0)*1e6, p->ClassId(), p->FullObjTag());
    //S0=S1;
    //#if dbgFlwSolve
    //if (dbgXRefsBuild())// && p->m_bXRefsValid)
    //  dbgpln("UnDoGlobalLinks:UnlinkAllXRefs ------------------------------------------------ %s", p->GetOwnerTag());
    //#endif
    p->UnlinkAllXRefs();
    }

  SHOWPROGRESS(1, "UnDoGlobalLinks 3.5");

  p=NULL;
  while (GetObj_All(p))
    {
    if ((p->m_FlowModeRqd & LFM_Propagate)==0)
      p->m_FlowModeRqd=0;
    p->m_FlowModeObj=NULL;
    }

  //for (FlwNode * p=NULL; GetObj_All(p); )
  //  {
  //  //dbgpln("Disconnect %s", p->Tag());

  //  //p->GlobalDisConnect();
  //  #if dbgFlwSolve
  //  if (dbgXRefsBuild())// && p->m_bXRefsValid)
  //    dbgpln("UnDoGlobalLinks:UnlinkAllXRefs ------------------------------------------------ %s", p->GetOwnerTag());
  //  #endif
  //  p->UnlinkAllXRefs();
  //  }

  SHOWPROGRESS(1, "UnDoGlobalLinks 4");

  // Must remove XRefs in case their contained info goes invalid. (eg. A referenced node gets deleted)
  for (p=NULL; GetObj_All(p); )
    {
    #if dbgFlwSolve
    if (dbgXRefsBuild())// && p->m_bXRefsValid)
      dbgpln("UnDoGlobalLinks:RemoveAllXRefs ------------------------------------------------ %s", p->GetOwnerTag());
    #endif
    p->RemoveAllXRefs();
    }
  SHOWPROGRESS(1, "UndoGlobalLinksDone");

  #if (dbgFlwSolve)
  if (dbgGlobalLinks())
    dbgpln("UnDoGlobalLinks - Done  ---------------------------------");
  #endif

  fGlblLinksDone=false;
  }

//==========================================================================
//
//
//
//==========================================================================

long CFlwSolver::MarkFlwNodesPressNetwork(FlwNode * pNd)
  {
  long N=0;

  for (int j=0; j<pNd->NoProcessJoins(); j++)
    {
    int NetNo=pNd->Joins[j].m_NetNo;
    int SubNo=pNd->Joins[j].m_SubNetNo;
    if (NetNo>=0 && SubNo>=0)
      {
      FlwNode *p=NULL;
      while (GetObj_All(p))
        {
        for (int j1=0; j1<p->NoProcessJoins(); j1++)
          {
          if (NetNo==p->Joins[j1].m_NetNo && SubNo==p->Joins[j1].m_SubNetNo)
            {
            p->fMarked=true;
            break;
            }
          }
        }
      }
    }
  return N;
  };

//--------------------------------------------------------------------------

long CFlwSolver::MarkFlwNodesFlowNetwork(FlwNode * pNd)
  {
  long N=0;
  return N;
  };

//==========================================================================
//
//
//
//==========================================================================

int CFlwSolver::FE_DoInsert(char *ClassIn, char *SubClassIn, char *PrimaryCfgIn, char *NTag, char *SrcTag, char *DstTag, FlwNode **pNd)
  {
  FlwNode * p;

  UnDoGlobalLinks();

  if (FindObj(NTag))
    return 2007;

  p=InsertNode(m_pRoot, ClassIn, SubClassIn, PrimaryCfgIn, NTag, NULL, TOA_Free);
  if (pNd)
    *pNd=p;
  if (p)
    {
    if (SrcTag)
      {
      SCDFullTag T;

      strcpy(T, NTag);
      strcat(T,TagSeparator);
      strcat(T,"I");
      int err=ConnectTag(m_pRoot, SrcTag, T);
      if (err)
        return err;

      T[strlen(T)-1] = 'O';
      err=ConnectTag(m_pRoot, T, DstTag);
      if (err)
        return err;
      }

    return 0;
    }

  return NErr_NotFound;
  }

//--------------------------------------------------------------------------

int CFlwSolver::FE_DoInsertRoot(char * RootName)
  {
  UnDoGlobalLinks();
  InsertRoot(RootName);

  return 0;
  }

//--------------------------------------------------------------------------

int CFlwSolver::FE_DoDelete(char * Tag)
  {
  UnDoGlobalLinks();


  FlwNode * p = (FlwNode *)FindObj(Tag);
  if (p)
    {
    p->DisConnectAll();
    delete p;
    return 0;
    }
  return NErr_NotFound;
  }

//--------------------------------------------------------------------------

int CFlwSolver::FE_DoConnect(char * SrcTag, char *DstTag)
  {
  UnDoGlobalLinks();
  int Res=ConnectTag(m_pRoot, SrcTag, DstTag);
  return Res;
  }

//--------------------------------------------------------------------------

int CFlwSolver::FE_DoDisConnect(char * SrcTag, char *DstTag)
  {
  UnDoGlobalLinks();
  int Res=DisConnectTag(m_pRoot, SrcTag, DstTag);
  return Res;
  }

//--------------------------------------------------------------------------

int CFlwSolver::FE_SetCommon(char * Tag, char * EqpDesc, char * EqpMemo, char * EqpIdStr, char * EqpLocation, char * PlantArea, char * EqpGUID)
  {
  FlwNode * p = (FlwNode *)FindObj(Tag);
  if (p)
    {
    p->SetEqpDesc(EqpDesc ? EqpDesc:"");
    p->SetEqpMemo(EqpMemo ? EqpMemo:"");
    p->SetEqpIdStr(EqpIdStr ? EqpIdStr:"");
    p->SetEqpLocation(EqpLocation ? EqpLocation:"");
    p->SetAreaTag( PlantArea ? PlantArea :"");
    p->SetEqpGUID(EqpGUID ? EqpGUID:"");
    return 0;
    }
  return NErr_NotFound;
  };

//--------------------------------------------------------------------------

int CFlwSolver::FE_GetCommon(char * Tag, Strng & EqpDesc, Strng & EqpMemo, Strng & EqpIdStr, Strng & EqpLocation, Strng & PlantArea, Strng & EqpGUID)
  {
  FlwNode * p = (FlwNode *)FindObj(Tag);
  if (p)
    {
    EqpDesc  = p->EqpDesc();
    EqpMemo  = p->EqpMemo();
    EqpIdStr = p->EqpIdStr();
    EqpLocation = p->EqpLocation();
    PlantArea = p->GetAreaTagDisp();//.GetLength()>0 ? p->m_sAreaTag():"";
    EqpGUID  = p->EqpGUID();
    return 0;
    }
  return NErr_NotFound;
  };

//--------------------------------------------------------------------------

class CLTHelp
  {
  public:
    CFlwSolver   & m_Root;
    Strng_List  & m_TagList;
    flag          m_DoingAll;
    Strng       * m_pS;


    CLTHelp(CFlwSolver * pRoot, Strng_List & TagList) :
      m_Root(*pRoot),
      m_TagList(TagList)
      {
      m_pS=m_TagList.Find("*");
      m_DoingAll=(m_pS!=NULL);

      if (m_DoingAll)
        m_TagList.Clear();
      };

    FlwNode * TagLoopGetStart(FlwNode *p)
      {
      if (m_Root.GetObj_All(p))
        {
        if (m_DoingAll)
          m_pS=new Strng(p->FullObjTag());
        else
          m_pS=m_TagList.Find(p->Tag());
        }
      return p;
      };
    void TagLoopGetEnd(FlwNode *p, int i)
      {
      m_pS->SetIndex(i);
      if (m_DoingAll)
        m_TagList.Append(m_pS);
      };

    FlwNode * TagLoopSetStart(FlwNode *p)
      {
      m_Root.GetObj_All(p);
      if (!m_DoingAll)
        m_pS=m_TagList.Find(p->Tag());
      return p;
      }
    void TagLoopSetEnd(FlwNode *p)
      {
      }

    CConnect * ConnLoopGetStart(POSITION &Pos, CConnect * pPrev)
      {   
      CConnect *p=NULL;
      if (pPrev==NULL)
        Pos = FlwNode::sm_ConnectList.GetHeadPosition();
      if (Pos)
        p = FlwNode::sm_ConnectList.GetNext(Pos);

      if (p)
        {
        //LPSTR pTag=p->Tag();
        //if (pTag && strlen(pTag)>0)
        //  {
        if (p->Tag())
          {
          if (m_DoingAll)
            m_pS=new Strng(p->Tag());
          else
            m_pS=m_TagList.Find(p->Tag());
          }
        else
          {
          m_pS=NULL;
          int xxx=0;
          }
        //  }
        }

      return p;
      };
    void ConnLoopGetEnd(bool DoIt, CConnect *p, int i)
      {
      if (DoIt && m_pS && m_pS->GetLength()>0)
        {
        //dbgpln("<<<<<<< %i 0x%08x 0x%08x %s", i, p, m_pS, m_pS->Str());
        m_pS->SetIndex(i);
        if (m_DoingAll)
          m_TagList.Append(m_pS);
        }
      else
        {
        int xxx=0;
        }
      };

    //CConnectMap::CPair * ConnLoopGetStart(CConnectMap::CPair *p)
    //  {      
    //  if (p==NULL)
    //    p = FlwNode::sm_ConnectsMap.PGetFirstAssoc( );
    //  else
    //    p = FlwNode::sm_ConnectsMap.PGetNextAssoc( p );

    //  if (p)
    //    {
    //    if (m_DoingAll)
    //      m_pS=new Strng(p->value->Tag());
    //    else
    //      m_pS=m_TagList.Find(p->value->Tag());
    //    }

    //  return p;
    //  };
    //void ConnLoopGetEnd(CConnectMap::CPair *p, int i)
    //  {
    //  m_pS->SetIndex(i);
    //  if (m_DoingAll)
    //    m_TagList.Append(m_pS);
    //  };

  };

//--------------------------------------------------------------------------

int CFlwSolver::FE_TagOperation(FE_TagOps Act, Strng_List & TagList)
  {
  TryDoGlobalLinks();

  int N=0;

  CLTHelp LTH(this,TagList);

  FlwNode *pNd=NULL;
  CConnect *pCn=NULL;
  POSITION Pos=NULL;
  int i;
  switch (Act)
    {
    case FETOp_GetActive:
      while (pNd=LTH.TagLoopGetStart(pNd))
        {
        if (LTH.m_pS)
          {
          i=pNd->GetActive();
          N++;
          LTH.TagLoopGetEnd(pNd, i);
          }
        }
      break;
    case FETOp_GetHold:
      while (pNd=LTH.TagLoopGetStart(pNd))
        {
        if (LTH.m_pS)
          {
          if (pNd->GetActiveHoldOK())
            i=pNd->GetActiveHold();
          else
            i=-1;
          N++;
          LTH.TagLoopGetEnd(pNd, i);
          }
        }
      break;
    case FETOp_SetActive:
      UnDoGlobalLinks();
      while (pNd=LTH.TagLoopGetStart(pNd))
        {
        if (LTH.m_pS)
          {
          pNd->SetActive(LTH.m_pS->Index()!=0);
          N++;
          LTH.TagLoopSetEnd(pNd);
          }
        }
      TryDoGlobalLinks();
      break;
    case FETOp_SetHold:
      UnDoGlobalLinks();
      while (pNd=LTH.TagLoopGetStart(pNd))
        {
        if (LTH.m_pS)
          {
          pNd->SetActiveHold(LTH.m_pS->Index()!=0);
          N++;
          LTH.TagLoopSetEnd(pNd);
          }
        }
      TryDoGlobalLinks();
      break;
    case FETOp_MarkPrsNets:
      while (pNd=LTH.TagLoopGetStart(pNd))
        {
        if (LTH.m_pS)
          {
          MarkFlwNodesPressNetwork(pNd);
          N++;
          LTH.TagLoopSetEnd(pNd);
          }
        }
      break;
    case FETOp_MarkFlwNets:
      break;
    case FETOp_MrkCrit1:
      break;
    case FETOp_MrkCrit2:
      break;
    case FETOp_MrkCrit3:
      break;
    case FETOp_ClrMarked:
      while (pNd=LTH.TagLoopGetStart(pNd))
        {
        if (LTH.m_pS)
          {
          pNd->fMarked=false;
          N++;
          LTH.TagLoopSetEnd(pNd);
          }
        }
      break;
    case FETOp_GetActiveConns:
      {
      while (pCn=LTH.ConnLoopGetStart(Pos, pCn))
        {
        //dbgpln("ConnLoop 0x%08x %s", pCn, LTH.m_pS->Str());
        
        bool DoIt=dynamic_cast<CFlange*>(pCn)!=NULL;
        int iFlowType=FlwType_Null;
        int iUseStatus=pCn->UseStatus();

        if (DoIt && LTH.m_pS)
          {
          if (pCn->m_pNdSrc && pCn->m_pNdSrc->GetActive())
            {
            int IOId = pCn->m_pNdSrc->IOId_Self(pCn->m_iIOSrc);
            if (IsMLnkIOId(IOId))         iFlowType=FlwType_Proc;
            else if (IsMakeupIOId(IOId))  iFlowType=FlwType_Makeup;
            else if (IsBleedIOId(IOId))   iFlowType=FlwType_Bleed;
            else if (IsSpillIOId(IOId))   iFlowType=FlwType_Spill;
            else if (IsVentIOId(IOId))    iFlowType=FlwType_Vent;
            else if (IsLeakIOId(IOId))    iFlowType=FlwType_Leak;
            else                          iFlowType=FlwType_Other;
            }
          N++;
          }
        LTH.ConnLoopGetEnd(DoIt, pCn, PackConnect(iFlowType, iUseStatus));
        }
      break;
      }
    default:
      {
      }
    }
  return N;
  }

//--------------------------------------------------------------------------

int CFlwSolver::FE_SetHoldGlobalLinks(bool On)
  {
  m_fHoldDoGlobalLinks=On;
  if (!On)
    TryDoGlobalLinks();
  return true;
  }

//--------------------------------------------------------------------------

byte CFlwSolver::FE_FlwMomentumOn()
  {
  return CFlwBlkBase::fMomentumOn;
  }

//--------------------------------------------------------------------------

void CFlwSolver::FE_SetFlwMomentumOn(byte On)
  {
  CFlwBlkBase::fMomentumOn=On;
  }

//--------------------------------------------------------------------------

void CFlwSolver::DoTraceChanges(dword Flags)
  {
#if WITHCHANGEOBJ
  ChgObj.StartAddition();

  if (!ChgObj.AllowCollection())
    return;
#endif

  CFlwNodeList &Order = (NetProbalMethod() ? PB.m_AllNds : Dyn.m_AllNds);
  CFlwNodeIter It(Order);

  if (Flags & TCO_DoInitialise)
    {
    for (FlwNode * p = It.First(); p; p=It.Next())
      p->InitChangeTrace();
    }

  if (Flags & TCO_DoCollect)
    {
    CTraceItem::BumpCount();
    for (FlwNode * p = It.First(); p; p=It.Next())
      p->TraceChanges();
    }

#if WITHCHANGEOBJ
  if (Flags & TCO_DoCollect)
    {
    CFlwNodeList &Order = (NetProbalMethod() ? PB.m_AllNds : Dyn.m_AllNds);
    CFlwNodeIter It(Order);
    for (FlwNode * p = It.First(); p; p=It.Next())
      ChgObj.DoAddition(Flags, p->GetChangeTrace(Flags));
    }

  if (Flags & TCO_DoSort)
    ChgObj.DoSort();
#endif
  };

//--------------------------------------------------------------------------
/*
flag CFlwSolver::RequestModelInfoByGroupIndex(pchar pGroup, int iNo, RequestModelInfoRec &Info)
  {
  Info.Name="";
  Info.Desc="";
  TagObjClass* p = TagObjClass::FindClassIndexed(pGroup, iNo);
  if (p)
    {
    Info.Name=p->ClassId();
    Info.Desc=p->Desc();
    Info.ShortDesc=p->ShortDesc();
    Info.TagInitialID=p->TagInitialID();
    return true;
    }
  return false;
  }
*/
//--------------------------------------------------------------------------

flag CFlwSolver::RequestModelIOInfoByClassId(pchar pClass, RequestModelIOInfoArray & Info)
  {
  INCOMPLETECODE(__FILE__, __LINE__);
//flag CSfeSrvrBase::RequestModelIOInfo(pchar pTag, int iIndex, RequestModelIOInfoRec & Info)
//  {
  Info.SetSize(0);

  TagObjClass* pC = TagObjClass::FindClassId(pClass);
  if (pC==NULL)
    return false;
  FlwNode * p=dynamic_cast<FlwNode*>(pC->Construct(NULL, "?????????", NULL, TOA_Free));
  if (p)
    {
    for (int n=0; ; n++)
      if (p->IODesc_SelfIndexed(n)==NULL)
        break;
    Info.SetSize(n);

    for (int i=0; i<n; i++)
      {
      IOAreaRec * pIO=p->IODesc_SelfIndexed(i);
      if (pIO)
        {
        Info[i].Name=pIO->IOName();
        Info[i].Desc=pIO->IODesc();
        Info[i].iId=pIO->m_Id;
        Info[i].Input=(pIO->m_Dirn & LIO_In)!=0;
        Info[i].Output=(pIO->m_Dirn & LIO_Out)!=0;
        Info[i].nIORqd=pIO->m_nIORqd;
        Info[i].nIOMax=pIO->m_nIOMax;
        Info[i].Dirn=pIO->m_Dirn;
        Info[i].iGrp=IOGRP(pIO->m_dwFlags);
        Info[i].m_dwType=pIO->m_dwType;
        Info[i].m_sDefaultTS=pIO->m_pDefaultTS ? pIO->m_pDefaultTS:"";
        Info[i].m_Hidden=(pIO->m_dwFlags&IOHidden)!=0;
        Info[i].m_GrfTarget=pIO->m_GrfTarget;
        }
      }
    p->DisConnectAll();
    delete p;
    }
  return Info.GetSize()>0;
  }

//--------------------------------------------------------------------------

flag CFlwSolver::RequestModelIOInfoByIndex(pchar pTag, int iIndex, RequestModelIOInfoRec & Info)
  {
  int ObjTagLen=0;
  FlwNode * p=dynamic_cast<FlwNode*>(CTNode::FindObjTag(pTag, false, ObjTagLen, 0));
  if (p)
    {
    IOAreaRec * pIO=p->IODesc_SelfIndexed(iIndex);
    if (pIO)
      {
      Info.Name=pIO->IOName();
      Info.Desc=pIO->IODesc();
      Info.iId=pIO->m_Id;
      Info.Input=(pIO->m_Dirn & LIO_In)!=0;
      Info.Output=(pIO->m_Dirn & LIO_Out)!=0;
      Info.nIORqd=pIO->m_nIORqd;
      Info.nIOMax=pIO->m_nIOMax;
      Info.Dirn=pIO->m_Dirn;
      Info.iGrp=IOGRP(pIO->m_dwFlags);
      Info.m_dwType=pIO->m_dwType;
      Info.m_sDefaultTS=pIO->m_pDefaultTS ? pIO->m_pDefaultTS:"";
      Info.m_Hidden=(pIO->m_dwFlags&IOHidden)!=0;
      Info.m_GrfTarget=pIO->m_GrfTarget;
      return true;
      }
    }
  return false;
  }

//--------------------------------------------------------------------------

flag CFlwSolver::RequestModelIOInfoById(pchar pTag, int iId, RequestModelIOInfoRec & Info)
  {
  int ObjTagLen=0;
  FlwNode * p=dynamic_cast<FlwNode*>(CTNode::FindObjTag(pTag, false, ObjTagLen, 0));
  if (p)
    {
    IOAreaRec * pIO=p->IODesc_SelfId(iId);
    if (pIO)
      {
      Info.Name=pIO->IOName();
      Info.Desc=pIO->IODesc();
      Info.iId=pIO->m_Id;
      Info.Input=(pIO->m_Dirn & LIO_In)!=0;
      Info.Output=(pIO->m_Dirn & LIO_Out)!=0;
      Info.nIORqd=pIO->m_nIORqd;
      Info.nIOMax=pIO->m_nIOMax;
      Info.Dirn=pIO->m_Dirn;
      Info.iGrp=IOGRP(pIO->m_dwFlags);
      Info.m_dwType=pIO->m_dwType;
      Info.m_sDefaultTS=pIO->m_pDefaultTS ? pIO->m_pDefaultTS:"";
      Info.m_Hidden=(pIO->m_dwFlags&IOHidden)!=0;
      return true;
      }
    }
  return false;
  }

//--------------------------------------------------------------------------

flag CFlwSolver::RequestModelIOConn(pchar pTag, int iNo, RequestModelIOConnRec & Info)
  {
  int ObjTagLen=0;
  FlwNode * p=dynamic_cast<FlwNode*>(CTNode::FindObjTag(pTag, false, ObjTagLen, 0));
  if (p && iNo<p->NoProcessIOs() && p->Nd_Rmt(iNo))
    {
    Info.iId = p->IOId_Self(iNo);
    Info.Tag = p->Nd_Rmt(iNo)->FullObjTag();
    Info.ClassName = p->Nd_Rmt(iNo)->Class()->ClassName();
    Info.ClassId   = p->Nd_Rmt(iNo)->ClassId();
    Info.Group     = p->Nd_Rmt(iNo)->Group();
    Info.iGrp      = IOGRP(p->IODesc_Self(iNo)->m_dwFlags);
    if (p->NoProcLnkIOs()==2)
      Info.iDirn = (iNo==0) ? 1 : -1;
    else if (p->Nd_Rmt(iNo)->NoProcLnkIOs()==2)
      Info.iDirn = (p->IOIONo_Rmt(iNo)==0) ? -1 : 1;
    else
      Info.iDirn = p->IODesc_Self(iNo)->m_Dirn & LIO_In ? 1 : -1;
    Info.fAutoConnect= p->IOAutoConnect(iNo)!=0;
    Info.fIsDirectConnect = IsDirectIOId(Info.iId);
    return true;
    }
  return false;
  }

//--------------------------------------------------------------------------

int CFlwSolver::RequestModelStatus(CModelStatusCriterion &Criteria, CModelStatusArray &Status)
  {
  m_dwMdlStatusOptions=Criteria.m_dwMdlStatusOptions;
  if (!GetStatus())
    {
    Status.SetSize(0);
    return 0;
    }

  int i=0;
  for (FlwNode * p = NULL; GetObj_All(p); )
    {
    if (i>=Status.GetSize())
      Status.SetSize(i+256);

    char * pTag=p->FullObjTag();
    if (pTag)
      {
      Status[i].m_Tag=p->FullObjTag();
      Status[i].m_dwFlags=p->ModelStatus();
      
      //dbgpln("**Nd Get %8x  %8x %s", p->m_dwPrevStatus, Status[i].m_dwFlags, Status[i].m_Tag());
      
      p->m_dwPrevStatus=Status[i].m_dwFlags;
      i++;
      }
    }

  POSITION Pos=FlwNode::sm_ConnectList.GetHeadPosition();
  while (Pos)
    {
    CConnect *pCn=FlwNode::sm_ConnectList.GetNext(Pos);
    CFlange * pF=dynamic_cast<CFlange*>(pCn);
    if (pF)
      {
      if (i>=Status.GetSize())
        Status.SetSize(i+256);

      LPSTR t=pCn->Tag();
      if (t && strlen(t)>0)
        {
        Status[i].m_Tag=t;
        Status[i].m_dwFlags=pF->ModelStatus();
        pF->m_dwPrevStatus=Status[i].m_dwFlags;
        i++;
        }
      //dbgpln("**Fl Get %8x  %8x %s", pF->m_dwPrevStatus, Status[i].m_dwFlags, Status[i].m_Tag());
      }
    }

  //CConnectMap::CPair *pX=FlwNode::sm_ConnectsMap.PGetFirstAssoc();
  //while (pX)
  //  {
  //  CFlange * pF=dynamic_cast<CFlange*>(pX->value);
  //  
  //  if (pF)
  //    {
  //    if (i>=Status.GetSize())
  //      Status.SetSize(i+256);

  //    Status[i].m_Tag=pX->key;
  //    Status[i].m_dwFlags=pF->ModelStatus();

  //    //dbgpln("**Fl Get %8x  %8x %s", pF->m_dwPrevStatus, Status[i].m_dwFlags, Status[i].m_Tag());

  //    pF->m_dwPrevStatus=Status[i].m_dwFlags;
  //    i++;
  //    }

  //  pX=FlwNode::sm_ConnectsMap.PGetNextAssoc(pX);
  //  }

  if (i<Status.GetSize())
    Status.SetSize(i); // size it down
    
  return Status.GetSize();
  }

//--------------------------------------------------------------------------

int CFlwSolver::RequestModelStatusChgCnt(CModelStatusCriterion &Criteria, int MaxTest)
  {
  m_dwMdlStatusOptions=Criteria.m_dwMdlStatusOptions;
  if (!GetStatus())
    {
    return 0;
    }
  int Cnt=0;
  for (FlwNode * p = NULL; (Cnt != MaxTest) && GetObj_All(p); )
    {
    //dbgpln("** Tst %8x  %8x %s", p->dwPrevStatus, p->ModelStatus(), p->FullObjTag());
    if (p->m_dwPrevStatus!=p->ModelStatus())
      Cnt++;
    }

  POSITION Pos=FlwNode::sm_ConnectList.GetHeadPosition();
  while (Pos)
    {
    CConnect *pCn=FlwNode::sm_ConnectList.GetNext(Pos);
    CFlange * pF=dynamic_cast<CFlange*>(pCn);
    if (pF)
      {
      //dbgpln("**Fl Get %8x  %8x %s", pF->m_dwPrevStatus, Status[i].m_dwFlags, Status[i].m_Tag());

      LPSTR t=pCn->Tag();
      if (t && strlen(t)>0)
        {
        if (pF->m_dwPrevStatus!=pF->ModelStatus())
          Cnt++;
        }
      }
    }

  //CConnectMap::CPair *pX=FlwNode::sm_ConnectsMap.PGetFirstAssoc();
  //while (pX && (Cnt != MaxTest))
  //  {
  //  CFlange * pF=dynamic_cast<CFlange*>(pX->value);
  //  if (pF)
  //    {
  //    if (pF->m_dwPrevStatus!=pF->ModelStatus())
  //      Cnt++;
  //    }

  //  pX=FlwNode::sm_ConnectsMap.PGetNextAssoc(pX);
  //  }

  return Cnt;
  }

//--------------------------------------------------------------------------

flag CFlwSolver::RequestModelAssocGrfsMasks(bool SetClass, bool SetNode,LPCSTR NdTag, CModelAssocGrfMasks & Msks)
  {
  int ObjTagLen=0;

  if (SetClass || SetNode)
    {
    POSITION Pos=Msks.GetHeadPosition();
    while (Pos)
      {
      CModelAssocGrfMask & Msk=Msks.GetNext(Pos);
      FlwNode * p=dynamic_cast<FlwNode*>(CTNode::FindObjTag((LPSTR)(LPCSTR)Msk.m_Tag, false, ObjTagLen, 0));
      if (p)
        {
        if (SetClass)
          p->AssocGrfShowClass()=Msk.m_ShowClass;
        if (SetNode)
          p->AssocGrfShow()=Msk.m_ShowNode;
        }
      }
    return true;
    }
  else if (NdTag)
    {
    FlwNode * p=dynamic_cast<FlwNode*>(CTNode::FindObjTag((LPSTR)NdTag, false, ObjTagLen, 0));
    if (p)
      {
      CModelAssocGrfMask Msk(NdTag, p->AssocGrfShowClass(), p->AssocGrfShow());
      Msks.AddTail(Msk);
      return true;
      }
    }
  else
    {
    for (FlwNode * p = NULL; GetObj_All(p); )
      {
      CModelAssocGrfMask Msk(p->FullObjTag(), p->AssocGrfShowClass(), p->AssocGrfShow());
      Msks.AddTail(Msk);
      }
    return true;
    }
  return false;
  }

//--------------------------------------------------------------------------

flag CFlwSolver::RequestModelAssocGrfs(LPCTSTR Tag, CMdlAssocGrfList & Grfs)
  {
  Grfs.RemoveAll();
  if (Tag)
    {
    int ObjTagLen=0;
    FlwNode * p=dynamic_cast<FlwNode*>(CTNode::FindObjTag((LPTSTR)Tag, false, ObjTagLen, 0));
    if (p)
      {
      return p->ModelAssocGrfs(Grfs);
      }
    return false;
    }

  for (FlwNode * p = NULL; GetObj_All(p); )
    {
    p->ModelAssocGrfs(Grfs);
    }
  return true;
  };

//--------------------------------------------------------------------------

int CFlwSolver::RequestModelAssocGrfsChgCnt(LPCTSTR Tag, CMdlAssocGrfList **pGrfs, CMdlAssocGrfMap **pMap)
  {
  return 0;
  }

//--------------------------------------------------------------------------

int CFlwSolver::RequestTagRefInfo(LPCTSTR Tag, CXRefInfoArray &Info)
  {
  if (Tag && strlen(Tag)>0)
    {
    long N=0;
    Strng S(Tag);
    S.Lower();
    CTagRefsMapItem* pMapItem;
    if (m_TagRefsMap.Lookup((LPCTSTR)S(), pMapItem))
      {
      POSITION Pos=pMapItem->m_pTagRefs.GetHeadPosition();
      while (Pos)
        {
        pMapItem->m_pTagRefs.GetNext(Pos);
        N++;
        }
      }
    Info.SetSize(N);
    long i=0;
    if (m_TagRefsMap.Lookup((LPCTSTR)S(), pMapItem))
      {
      POSITION Pos=pMapItem->m_pTagRefs.GetHeadPosition();
      while (Pos)
        {
        CTagRef * pItem=pMapItem->m_pTagRefs.GetNext(Pos);
        Info[i++]->Copy(*pItem, false);
        }
      }
    }
  else
    {
    long N=0;
    POSITION Pos=m_TagRefsList.GetHeadPosition();
    for (long i=0; Pos; i++)
      {
      CTagRef *pItem=m_TagRefsList.GetNext(Pos);
      if (!pItem->m_bIsLcl)
        N++;
      }
    Info.SetSize(N);
    Pos=m_TagRefsList.GetHeadPosition();
    for (long i=0; Pos; )
      {
      CTagRef *pItem=m_TagRefsList.GetNext(Pos);
      if (!pItem->m_bIsLcl)
        Info[i++]->Copy(*pItem, false);
      }
    }

  return Info.GetSize();
  }

//--------------------------------------------------------------------------

flag CFlwSolver::RequestModelClassId(pchar pTag, Strng & ClassId)
  {
  int ObjTagLen=0;
  FlwNode * p=dynamic_cast<FlwNode*>(CTNode::FindObjTag(pTag, false, ObjTagLen, 0));
  ClassId = (p!=NULL ? p->ClassId() : "");
  return (p!=NULL);
  }

//--------------------------------------------------------------------------

flag CFlwSolver::RequestModelConnInfo(pchar pTag, RequestConnModelInfoRec & Info)
  {
  int ObjTagLen=0;
  FlwNode * p=dynamic_cast<FlwNode*>(CTNode::FindObjTag(pTag, false, ObjTagLen, 0));
  if (p && !p->Internal)
    {
    bool GrpOK=_stricmp(p->Group(), FlwLinkGrp)==0 ||
      _stricmp(p->Group(), ElecLinkGrp)==0 ||
      _stricmp(p->Group(), CtrlLinkGrp)==0;
    if (GrpOK)
      {
      ASSERT_RDB(p->NoProcLnkIOs()<=2, "A link should have two or less IOs!", __FILE__, __LINE__);
      for (int iNo=0; iNo<p->NoProcLnkIOs(); iNo++)
        {
        if (iNo==0)
          {
          Info.SrcTag = p->Nd_Rmt(iNo)->FullObjTag();
          }
        else
          {
          Info.DstTag = p->Nd_Rmt(iNo)->FullObjTag();
          }
        }
      return true;
      }
    }
  return false;
  }

//--------------------------------------------------------------------------

int CFlwSolver::FE_GetRctInfo(CRctBlkInformation & RctInfo)
  {
  //class CRctBlkInformation
  //{
  //public:
  //  class CNdRct
  //    {
  //    public:
  //      CNdRct()
  //        {
  //        m_iSeqNo = 0;
  //        m_iBasis = RctNull;
  //        }
  //      CNdRct(const CNdRct & Other)
  //        {
  //        *this=Other;
  //        }
  //      CNdRct & operator = (const CNdRct & Other)
  //        {
  //        m_iSeqNo    = Other.m_iSeqNo;
  //        m_iBasis    = Other.m_iBasis;
  //        m_sTag      = Other.m_sTag;
  //        m_sComment  = Other.m_sComment;
  //        m_sDefn     = Other.m_sDefn;
  //        }

  //    public:
  //      int                m_iSeqNo;
  //      eScdReactionBasis  m_iBasis;
  //      CString            m_sTag;
  //      CString            m_sComment;
  //      CString            m_sDefn;
  //    };

  //  class CRctBlk
  //    { 
  //    public:
  //      CRctBlk() 
  //        {
  //        }
  //      CRctBlk(const CRctBlk & Other)
  //        {
  //        *this=Other;
  //        }
  //      CRctBlk & operator=(const CRctBlk & Other)
  //        {
  //        m_sNdTag    = Other.m_sNdTag;
  //        m_sRctFile  = Other.m_sRctFile;
  //        m_sComment  = Other.m_sComment;
  //        m_Rcts.SetSize(Other.m_Rcts.GetSize());
  //        for (int i=0; i<Other.m_Rcts.GetSize(); i++)
  //          m_Rcts[i]=Other.m_Rcts[i];
  //        }

  //    public: 
  //      CString     m_sNdTag; 
  //      CString     m_sRctFile; 
  //      CString     m_sComment; 
  //      CArray <CNdRct, CNdRct&> m_Rcts;
  //    };

  //  CArray <CRctBlk, CRctBlk&> m_Nds;


  int RBCount=0;
  POSITION Pos=CReactionBase::sm_AllRBs.GetHeadPosition();
  while (Pos)
    {
    CReactionBase *pRB=CReactionBase::sm_AllRBs.GetNext(Pos);
    if (pRB->Enabled())
      RBCount++;
    }

  RctInfo.m_RBs.SetSize(RBCount);

  int iRB=0;
  Pos=CReactionBase::sm_AllRBs.GetHeadPosition();
  while (Pos)
    {
    CReactionBase *pRB=CReactionBase::sm_AllRBs.GetNext(Pos);
    if (pRB->Enabled())
      {
      CRctBlkInformation::CRctBlk &RBI=RctInfo.m_RBs[iRB++];

      RBI.m_sNdTag    = pRB->Nd()->FullObjTag();
      RBI.m_sBlkTag   = pRB->RBTag();
      RBI.m_sRctFile  = pRB->RCTName();
      RBI.m_sComment  = pRB->Comment();
      RBI.m_Rcts.SetSize(pRB->GetRctCount());
      for (int i=0; i<pRB->GetRctCount(); i++)
        {
        CRctBlkInformation::CRct & Rct=RBI.m_Rcts[i];
        
        Rct.m_iBasis    = pRB->GetRctBasis(i);
        Rct.m_sComment  = pRB->GetRctComment(i);
        Rct.m_sTag      = pRB->GetRctTag(i);
        Rct.m_sDefn     = pRB->GetRctString(i);
        Rct.m_iSeqNo    = pRB->GetRctSequence(i);
        }
      }
    }

  return RBCount;
  };

//--------------------------------------------------------------------------

int CFlwSolver::FE_SetRctInfo(LPCTSTR NodeTag)
  {
  return 0;
  };

//--------------------------------------------------------------------------

int CFlwSolver::FE_GetNodeWiring(CNodeWiring &NodeWiring)
  {
  int ObjTagLen=0;
  FlwNode * p=dynamic_cast<FlwNode*>(CTNode::FindObjTag((LPTSTR)(LPCTSTR)NodeWiring.m_NodeTag, false, ObjTagLen, 0));
  return p->GetNodeWiring(NodeWiring);
  };

//--------------------------------------------------------------------------

int CFlwSolver::FE_GetLinkWiring(CLinkWiring &LinkWiring)
  {
  int ObjTagLen=0;
  FlwNode * p=dynamic_cast<FlwNode*>(CTNode::FindObjTag((LPTSTR)(LPCTSTR)LinkWiring.m_LinkTag, false, ObjTagLen, 0));
  return p->GetLinkWiring(LinkWiring);
  };

//--------------------------------------------------------------------------

int CFlwSolver::FE_SetLinkWiring(CLinkWiring &LinkWiring)
  {
  int ObjTagLen=0;
  FlwNode * p=dynamic_cast<FlwNode*>(CTNode::FindObjTag((LPTSTR)(LPCTSTR)LinkWiring.m_LinkTag, false, ObjTagLen, 0));
  return p->SetLinkWiring(LinkWiring);
  };

//--------------------------------------------------------------------------

long CFlwSolver::FE_GetTopologyCount()
  {
  return m_lUnDoGlobalLinksCount;
  };

//--------------------------------------------------------------------------

long CFlwSolver::FE_GetNodes(DWORD What, CFlwNodeArray & Nodes)
  {
  What &= ~nc_FlwSolver;
  FlwNode * p=NULL;
  UINT N=0;
  while (GetObj_All(p))
    if (p->m_dwType & What)
      N++;

  Nodes.SetSize(N, 256);
  p=NULL;
  N=0;
  while (GetObj_All(p))
    {
    if (p->m_dwType & What)
      Nodes[N++]=p;
    }
  return Nodes.GetSize();
  };

//--------------------------------------------------------------------------

FlwNode *CFlwSolver::FE_FindNode(LPCTSTR Tag)
  {
  FlwNode * p=NULL;
  UINT N=0;
  while (GetObj_All(p))
    if (TaggedObject::TagCmp(p->Tag(), (LPTSTR)Tag))
      return p;
  return NULL;
  };

//--------------------------------------------------------------------------

//void CFlwSolver::FE_ElecComponentsChanged()
//  {
//  };

//--------------------------------------------------------------------------

void CFlwSolver::FE_UnDoGlobalLinks()
  {
  UnDoGlobalLinks();
  };

long CFlwSolver::FE_SpModelFixupStrategy()
  {
  return SpModel::FixupStrategy();
  }
void CFlwSolver::FE_SetSpModelFixupStrategy(long Strategy)
  {
  SpModel::SetFixupStrategy(Strategy);
  }

//--------------------------------------------------------------------------

flag CFlwSolver::FE_TestModelTagUnique(pchar pTag)
  {
  int ObjTagLen=0;
  TaggedObject * p=dynamic_cast<TaggedObject *>(CTNode::FindObjTag(pTag, false, ObjTagLen, 0));

  if (p)
    return false;

  CConnect * pConn=FlwNode::FindConnect(pTag, false, ObjTagLen, 0);
  if (pConn)
    p=dynamic_cast<TaggedObject*>(pConn);

  return (p==NULL);
  };

//--------------------------------------------------------------------------

class CMdlFileFindInfo
  {
  public:
    CStringArray Models;
    CSVector OtherModels; //Array files
    CFilesUsedArray Files;
    CStringArray FileObjs;
    CStringArray Specs;
    int FileCount;
    int MdlCount;
    int OtherCount;
    int SpecCount;

    CMdlFileFindInfo() { FileCount = 0; MdlCount = 0; OtherCount = 0; SpecCount = 0; };
  };

void CFlwSolver::EO_OnAppActivate(BOOL bActive)
  {
  STARTPROGRESS;
  //dbgpln("EO_OnAppActivate ======================================================== %10.1fms", SW.Lap()*1e3);

  FlwNode * p=NULL;
  while (GetObj_All(p))
    {
    p->OnAppActivate(bActive);
    }

  //dbgpln("EO_OnAppActivate ======================================================== %10.1fms", SW.Lap()*1e3);
  }

flag CFlwSolver::EO_RequestTagInfo(RequestTagInfoRec& Rqst, ReplyTagInfoRec& Info)
  {
  switch (Rqst.RQ_Type)
    {
    case RQ_Files:
      {
      if (Rqst.bFirst)
        {//build lists...
        m_RQ_Files_Cnt=0;

        CMdlFileFindInfo* pMFFI = new CMdlFileFindInfo;
        Info.SetPos((long)pMFFI);
        TagObjClass* pTOC = TagObjClass::FirstClass();
        while (pTOC)
          {
          pTOC->SetFlagged(0);
          pTOC = pTOC->NextClass();
          }
        pSaveNode = NULL;
        while (GetObj_All(pSaveNode))
          {
          if (!pSaveNode->Internal)
            {
            //
            // CNM - removed - does not appear to do anything usefull
            //
            //if (!pSaveNode->Class()->Flagged())
            //  {
            //  pSaveNode->Class()->SetFlagged(1);
            //  CXM_ObjectTag ObjTag(pSaveNode->Tag(), 0);
            //  CXM_ObjectData OD;
            //  CXM_Route Route;

            //  if (EO_ReadTaggedItem(ObjTag, OD, Route))
            //    {
            //    CPkDataList & DList = OD.List;

            //    CPkDataIter Iter;
            //    CPkDataItem* pItem=DList.FirstItem(Iter);
            //    if (pItem)
            //      pItem=DList.NextItem(Iter);
            //    for (  ; pItem; pItem=DList.NextItem(Iter))
            //      {
            //      const byte Type = pItem->Type();
            //      if (Type==tt_Object || Type==tt_Array)
            //        {
            //        pchar pClass = pItem->Class();
            //        TagObjClass* pp = TagObjClass::FindClassId(pClass, true);
            //        if (pp)
            //          pp->SetFlagged(1);
            //        //else if (pMFFI->OtherModels.Find(pClass)<0 && (XDBFormat()==DBO_CSV))
            //        //  pMFFI->OtherModels.Add(pClass);
            //        }
            //      }
            //    }
            //  }
            int iFU = pSaveNode->FilesUsed(pMFFI->Files);
            while (iFU-->0)
              pMFFI->FileObjs.Add(pSaveNode->Tag());
            }
          }
        pTOC = TagObjClass::FirstClass();
        while (pTOC)
          {
          Strng Fn(pTOC->ClassIdNoVersion());
          Fn += ".scs";
          Strng FnS;
          FnS.FnSearchExpand(Fn(), AF_All);
          if (FileExists(FnS()))
            pMFFI->Specs.Add(FnS());
          pTOC = pTOC->NextClass();
          }
        }
      CMdlFileFindInfo* pMFFI = (CMdlFileFindInfo*)Info.GetPos();
      Info.bDWDataValid = 1;
      if (m_RQ_Files_Cnt<7)
        {
        char *p=NULL;
        int MaxCnt=5;
        while (p==NULL && (m_RQ_Files_Cnt<MaxCnt))
          {
          switch (m_RQ_Files_Cnt)
            {
            case 0: p=CfgFile();         break;
            case 1: p=m_PrjCnvsDBName(); break;
            case 2: p=m_PrjSDBName();    break;
            case 3: p="Other.dat";       break;
            case 4: p=MDBName;   break;
            }
          if (p)
            {
            Info.dwData       = m_RQ_Files_Cnt < 3 ? 4 : 1;
            Info.bCanEditFile = false;
            Info.bForceCopy   = false;
            Info.bExcelFile   = false;
            Info.bPreventCopy = (m_RQ_Files_Cnt==1);//because m_PrjCnvsDBName==m_PrjSDBName
            Info.sData        = m_RQ_Files_Cnt < 3 ? "" : PrjFiles();
            Info.sData += p;
            Info.sData.FnContract();
            m_RQ_Files_Cnt++;
            return true;
            }
          m_RQ_Files_Cnt++;
          }
        }
      if (pMFFI->MdlCount<pMFFI->Models.GetSize())
        {
        Info.dwData = 1;
        Info.sData = (const char*)(pMFFI->Models[pMFFI->MdlCount++]);
        Info.sData.FnContract();
        return true;
        }
      else if (pMFFI->FileCount<pMFFI->Files.GetSize())
        {
        Info.dwData = 2;
        Info.sData=(char*)(const char*)(pMFFI->Files[pMFFI->FileCount].Name);
        Info.sData.FnContract();
        Info.bCanEditFile = pMFFI->Files[pMFFI->FileCount].EditTxt();
        Info.bExcelFile = pMFFI->Files[pMFFI->FileCount].EditExcel();
        Info.bForceCopy = pMFFI->Files[pMFFI->FileCount].CopyFile();
        Info.bPreventCopy = false;
        Info.bData1Valid = 1;
        Info.sData1 = (const char*)(pMFFI->FileObjs[pMFFI->FileCount]);
        Info.sData1.FnContract();
        pMFFI->FileCount++;
        return true;
        }
      else if (pMFFI->OtherCount<pMFFI->OtherModels.GetSize())
        {
        Info.dwData = 1;
        Info.sData = PrjFiles();
        Info.sData += "Database\\";
        Info.sData += pMFFI->OtherModels[pMFFI->OtherCount++];
        Info.sData += ".csv";
        Info.sData.FnContract();
        return true;
        }
      else if (pMFFI->SpecCount<pMFFI->Specs.GetSize())
        {
        Info.dwData = 3;
        Info.sData = pMFFI->Specs[pMFFI->SpecCount++];
        Info.sData.FnContract();
        return true;
        }
      delete pMFFI;
      break;
      }
    case RQ_Tags: //return info on all tags based on request criteria...
    case RQ_TagsConns: //return info on all tags based on request criteria...
      if (Rqst.bFirst)
        pSaveNode = NULL;
      while (GetObj_All(pSaveNode))
        {
        if (!pSaveNode->Internal)
          {
          if (Rqst.pGroup==NULL || _stricmp(Rqst.pGroup, pSaveNode->ClassId())==0)
            {
            Info.sData = pSaveNode->FullObjTag();
            if (Rqst.RQ_Type==RQ_TagsConns)
              {
              Strng_List TagLst;
              int NConns=pSaveNode->GetConnectionTags(TagLst);
              if (NConns)
                {
                pStrng p;
                flag NeedComma=false;
                flag NeedBrace=false;
                Info.sData += " ";
                for (p=TagLst.First(); p; p=TagLst.Next())
                  if (p->Index()>0)
                    {
                    if (NeedComma)
                      Info.sData += ",";
                    Info.sData += " \"";
                    Info.sData += p->Str();
                    Info.sData += "\"";
                    NeedComma=true;
                    }
                NeedComma=false;
                for (p=TagLst.First(); p; p=TagLst.Next())
                  if (p->Index()==0)
                  {
                  if (!NeedBrace)
                    Info.sData += " [";
                  if (NeedComma)
                    Info.sData += ", ";
                  Info.sData += p->Str();
                  NeedBrace=true;
                  NeedComma=true;
                  }
                if (NeedBrace)
                  Info.sData += "]";
                }
              }
            return true;
            }
          }
        }
      break;
    case RQ_TagCount:
      if (Rqst.bFirst)
        {
        Info.bDWDataValid = 1;
        Info.dwData = 0;
        pSaveNode = NULL;
        while (GetObj_All(pSaveNode))
          {
          if (!pSaveNode->Internal)
            {
            if (Rqst.pGroup==NULL || _stricmp(Rqst.pGroup, pSaveNode->ClassId())==0)
              Info.dwData++;
            }
          }
        return true;
        }
      break;
    case RQ_TagCountCategory:
      if (Rqst.bFirst)
        {
        Info.bDWDataValid = 1;
        Info.dwData = 0;
        pSaveNode = NULL;
        while (GetObj_All(pSaveNode))
          {
          if (!pSaveNode->Internal)
            {
            //if ((pSaveNode->Class()->CategoryMdl() & Rqst.dwReqData)!=0) WRONG
            const DWORD dw1 = pSaveNode->Class()->CategoryMdl();
            if ((dw1 & Rqst.dwReqData)==dw1) //all mdl test criteria met
              Info.dwData++;
            else
              LogNote("FlwSolve", 0, "Unit not Licensed %s", pSaveNode->FullObjTag());
            }
          }
        return true;
        }
      break;
    }
  return false;
  }

//--------------------------------------------------------------------------

void CFlwSolver::EO_SetModelState(eScdMdlStateActs RqdState, Strng_List &RqdTags)
  {
  STARTPROGRESS;
  SHOWPROGRESS(10, "EO_SetModelState");
  if (gs_Exec.SolvingPB())
    {
    //PB.ClearSolutionBlks(true);
    }
  else
    {
    // removed to fix #661 
    // Dyn.ClearSolutionBlks(false);
    }

  if (RqdTags.Length()>0)
    {
    for (pStrng pTag=RqdTags.First(); pTag; pTag=RqdTags.Next())
      {
      int ObjTagLen=0;
      FlwNode * p=dynamic_cast<FlwNode*>(CTNode::FindObjTag(pTag->Str(), false, ObjTagLen, 0));
      if (p)
        p->SetState(RqdState);
      }
    }
  else
    {
    for (FlwNode * p = NULL; GetObj_All(p); )
      {
////TODO Temporary - Improve/Remove
//
//      for (int i=0; i<p->NoIOs(); i++)
//        for (int j=0; j<p->NIOFBs(i); j++)
//          p->IOFB(i,j)->ResetData(false);
//
      p->SetState(RqdState);
      }
    if (RqdState==MSA_PBInit)
      {
      CFlwNodeList AllNds;
      FlwNode * p = NULL;
      while (GetObj_All(p))
        {
        if (p->NetProbalMethod())
          AllNds.AddTail(p);
        }


      EqnSlvCtrlBlk EQCtrl;
      if (PB.InitSolution(EQCtrl, AllNds)==0)
        {
        PB.InitialiseAllTears(EQCtrl);
        PB.TermSolution(EQCtrl);
        }
      }
    }

  if (gs_Exec.SolvingPB())
    {
    //PB.ClearSolutionBlks(true);
    }
  else //if (gs_Exec.SolvingPB())
    {
    // removed to fix #661 
    //Dyn.ReStart();
    }

  SHOWPROGRESS(1, "EO_SetModelState");
  };

//--------------------------------------------------------------------------

flag CFlwSolver::EO_GetModelAction(LPCTSTR Tag, CMdlActionArray & Acts)
  {
  int ObjTagLen=0;
  FlwNode * p=dynamic_cast<FlwNode*>(CTNode::FindObjTag((LPTSTR)Tag, false, ObjTagLen, 0));
  if (p)
    return p->GetModelAction(Acts);
  return false;
  };

//--------------------------------------------------------------------------

flag CFlwSolver::EO_SetModelAction(LPCTSTR Tag, CMdlAction & Act)
  {
  int ObjTagLen=0;
  FlwNode * p=dynamic_cast<FlwNode*>(CTNode::FindObjTag((LPTSTR)Tag, false, ObjTagLen, 0));
  if (p)
    return p->SetModelAction(Act);
  return false;
  };

//--------------------------------------------------------------------------

flag CFlwSolver::EO_GetModelGraphic(LPCTSTR Tag, CMdlGraphicArray & Grfs)
  {
  int ObjTagLen=0;
  FlwNode * p=dynamic_cast<FlwNode*>(CTNode::FindObjTag((LPTSTR)Tag, false, ObjTagLen, 0));
  if (p)
    return p->GetModelGraphic(Grfs);
  return false;
  };

//--------------------------------------------------------------------------

flag CFlwSolver::EO_OperateModelGraphic(LPCTSTR Tag, CMdlGraphicWnd & Wnd, CMdlGraphic & Grf)
  {
  int ObjTagLen=0;
  FlwNode * p=dynamic_cast<FlwNode*>(CTNode::FindObjTag((LPTSTR)Tag, false, ObjTagLen, 0));
  if (p)
    return p->OperateModelGraphic(Wnd, Grf);
  return false;
  };

//--------------------------------------------------------------------------

int CFlwSolver::EO_QueryChangeTag(pchar pOldTag, pchar pNewTag)
  {
  if (!FE_TestModelTagUnique(pNewTag))
    return EOCT_DUPLICATE;
  if (TaggedObject::TestIfReservedTag(pOldTag))
    return EOCT_NOTALLOWED;
  if (TaggedObject::TestIfReservedTag(pNewTag))
    return EOCT_RESERVED;
  int ObjTagLen=0;
  TaggedObject * p=CTNode::FindObjTag(pOldTag, false, ObjTagLen, 0);
  
  if (p==NULL)
    {
    CConnect * pConn=FlwNode::FindConnect(pOldTag, false, ObjTagLen, 0);
    if (pConn)
      p=dynamic_cast<TaggedObject*>(pConn);
    }

  if (p==NULL)
    return EOCT_NOTFOUND;
  if (_stricmp("FlwSolve", p->ClassId())==0)
    return EOCT_NOTALLOWED;
  if (_stricmp(pOldTag, PlantAreaTag)==0)
    return EOCT_NOTALLOWED;
  return EOCT_FOUND;
  }

//--------------------------------------------------------------------------

int CFlwSolver::EO_ChangeTag(pchar pOldTag, pchar pNewTag)
  {
  if (!FE_TestModelTagUnique(pNewTag))
    return EOCT_DUPLICATE;
  int ObjTagLen=0;
  TaggedObject * pTO=CTNode::FindObjTag(pOldTag, false, ObjTagLen, 0);

  if (pTO==NULL)
    {
    CConnect * pConn=FlwNode::FindConnect(pOldTag, false, ObjTagLen, 0);
    if (pConn)
      pTO=dynamic_cast<TaggedObject*>(pConn);
    }

  if (pTO==NULL)
    return EOCT_FAILED;
  if (TaggedObject::TestIfReservedTag(pOldTag))
    return EOCT_RESERVED;

  UnDoGlobalLinks();

  pTO->SetTag(pNewTag); //changes the tag in the solver

  //ask all models if they want to do anything internally for the change tag...
  FlwNode * p=NULL;
  while (GetObj_All(p))
    {
    if (!p->Internal)
      p->ChangeTag(pOldTag, pNewTag);
    }

  return EOCT_DONE;
  }

//--------------------------------------------------------------------------

int CFlwSolver::EO_ChangeTagDone(pchar pOldTag, pchar pNewTag)
  {
  TryDoGlobalLinks();// reconnect tags so that graphics status will be found
  return EOCT_DONE;
  }

//--------------------------------------------------------------------------

int CFlwSolver::EO_QueryDeleteTag(pchar pDelTag)
  {
  FlwNode * p = (FlwNode *)FindObj(pDelTag);
  if (p==NULL)
    return EODT_NOTFOUND;
  if (TaggedObject::TestIfReservedTag(pDelTag))
    return EODT_RESERVED;
  return (_stricmp("FlwSolve", p->ClassId())==0) ? EODT_NOTALLOWED : EODT_FOUND;
  }

//--------------------------------------------------------------------------

int CFlwSolver::EO_DeleteTag(pchar pDelTag)
  {
  UnDoGlobalLinks();
  FlwNode * p = (FlwNode *)FindObj(pDelTag);
  if (p)
    {
    dbgpln("DELETE %s %s ", pDelTag, p->FullObjTag());
    p->DisConnectAll();
    delete p;

    //ask all models if they want to do anything internally for the delete tag...
    pSaveNode = NULL;
    while (GetObj_All(pSaveNode))
      {
      if (!pSaveNode->Internal)
        {
        pSaveNode->DeleteTag(pDelTag);
        }
      }

    return EODT_DONE;
    }
  return EODT_FAILED;
  }

//--------------------------------------------------------------------------

int CFlwSolver::EO_DeleteTagDone(pchar pDelTag)
  {
  TryDoGlobalLinks();
  return EODT_DONE;
  }

//--------------------------------------------------------------------------

int CFlwSolver::EO_CanClose(Strng_List & Problems)
  {
  TryDoGlobalLinks();
  // Must Test for Changes
  return EO_CanClose_Yes;
  }

//--------------------------------------------------------------------------

void CFlwSolver::EO_SetSolveMode()
  {
  UnDoGlobalLinks();

  }

//--------------------------------------------------------------------------

flag CFlwSolver::EO_Starting(flag fBeginStarting)
  {
  STARTPROGRESS;
  SHOWPROGRESS(10, "EO_Starting");

  if (fBeginStarting)
    MSMFnRanges::BumpGlblIter();

  // Undo to Redo -- in case mode has changed
  ASSERT(_CrtCheckMemory());
  gs_MsgLog.SetDefer(true);

  if (fBeginStarting)
    {
    gs_FSIdleRequested=false;
    DoGlobalLinks(true);
    }

  gs_MsgLog.SetDefer(false);
  gs_MsgLog.FlushWndQ();
  ASSERT(_CrtCheckMemory());

  SHOWPROGRESS(1, "EO_Starting");
  return true;
  }

//--------------------------------------------------------------------------

flag CFlwSolver::EO_SetTime(CTimeValue TimeRqd, bool Rewind)
  {
  STARTPROGRESS;
  SHOWPROGRESS(10, "EO_SetTime");

  ODE().SetTime(TimeRqd);

  SHOWPROGRESS(1, "EO_SetTime");
  return True;
  };

//--------------------------------------------------------------------------

flag CFlwSolver::EO_QueryTime(CXM_TimeControl &CB, CTimeValue &TimeRqd, CTimeValue &dTimeRqd)
  {
  STARTPROGRESS;
  SHOWPROGRESS(10, "EO_QueryTime");
  // Undo to Redo -- in case mode has changed

  if (gs_Exec.SolvingPB())
    {
    dTimeRqd=CTimeValue(1.0); // ProBal Mode always steps 1 Sec at a time.
    TimeCB().m_StepSizeMax=CTimeValue(1.0);
    }
  else
    {
    Dyn.QueryTime(CB, TimeRqd, dTimeRqd);
    }
  TimeRqd.Seconds=ODE().GetTime();

  ASSERT(_CrtCheckMemory());

  SHOWPROGRESS(1, "EO_QueryTime");
  return true;
  }

//--------------------------------------------------------------------------

static void SetConnectedActive(FlwNode * p)
  {
  // Infuture get more clever !!!
  for (int i=0; i<p->NoProcessIOs(); i++)
    if (!p->Nd_Rmt(i)->GetActive())
      {
      p->Nd_Rmt(i)->SetActive(true);
      SetConnectedActive(p->Nd_Rmt(i));
      }
  };

//--------------------------------------------------------------------------

static int HpEvalPosn(void * p, void * q)
  {
  FlwNode * r1=(FlwNode *)p;
  FlwNode * r2=(FlwNode *)q;
  int Cmp=(r1->m_iEvalPosn-r2->m_iEvalPosn);
  return Cmp<0;
  }

//--------------------------------------------------------------------------

flag CFlwSolver::EO_GotoRunStart(CXM_TimeControl &CB)
  {
  //dbgpln("CFlwSolver::EO_GotoRunStart");
  
  TryDoGlobalLinks();
  
  return true;
  };

//--------------------------------------------------------------------------

#define RESTORE_EO_Start 0

flag CFlwSolver::EO_GotoRunEnd(CXM_TimeControl &CB, bool TimeChanged)
  {
#if RESTORE_EO_Start
  return 1;
#endif

  //dbgpln("CFlwSolver::EO_GotoRunEnd");

  // Set the Time - regardless
  ODE().SetTime(CB.m_TheTime.Seconds);
  //ODE().ClSetItersTotal(0);

  STARTPROGRESS;
  SHOWPROGRESS(10, "EO_GotoRunEnd");

  gs_MsgLog.SetDefer(true);

  // Check All Connected are Active
  FlwNode * p = NULL;
  if (GetObj_All(p))
    {
    p->SetActive(true); // Force Flwsolver to be active
    while (GetObj_All(p))
      if (p->GetActive())
        SetConnectedActive(p);
    }

  bool OK=1;

  SHOWPROGRESS(1, "EO_GotoRunEnd 1");

  if (!CDB.H2OPropertiesSelectedBlk().Class()->LicenseOK())
    {
    OK=0;
    LogError("Solver", 0, "H2O Properties model '%s' not enabled by licensing", CDB.H2OPropertiesSelectedBlk().Class()->ShortDesc());
    }
  if (CDB.Bad())
    {
    OK=0;
    LogError("Solver", 0, "Bad Component Database");
    }

  SHOWPROGRESS(1, "EO_GotoRunEnd 2");

  CFlwNodeList AllNds;

  if (0)
    {
    int NFlwNodes=0;
    for (FlwNode * p=NULL; GetObj_All(p); )
      {
      if (p->GetActive())
        {
        NFlwNodes++;
        AllNds.AddTail(p);
        }
      else
        p->iAllIndex=-1;
      }
    }
  else if (1)
    {
    int NFlwNodes=0;
    for (FlwNode * p=NULL; GetObj_All(p); )
      {
      if (p->GetActive())
        NFlwNodes++;
      }

    m_TempNdPtrs.SetSize(Max(m_TempNdPtrs.GetSize(), NFlwNodes));

    int nObj=0;
    for (FlwNode * p=NULL; GetObj_All(p); )
      {
      if (p->GetActive())
        m_TempNdPtrs[nObj++]=p;
      else
        p->iAllIndex=-1;
      }

    HpSort(NFlwNodes, (void**)&m_TempNdPtrs[0], HpEvalPosn);

    for (int i=0; i<NFlwNodes; i++)
      {
      FlwNode *p=m_TempNdPtrs[i];
      AllNds.AddTail(m_TempNdPtrs[i]);
      }
    }
  //else
  //  {

  //  p = NULL;
  //  if (GetObj_All(p))
  //    {
  //    p->iAllIndex=-1; //skip Flwsolver
  //    //p->SetNodeSolveMode();
  //    for (int nObj=0; GetObj_All(p); nObj++)
  //      {
  //      if (p->GetActive())
  //        {
  //        CFlwNodeIterWithPos All(AllNds);
  //        for (FlwNode * pp=All.First(); (pp); pp=All.Next())
  //          if (pp->m_iEvalPosn>p->m_iEvalPosn)
  //            break;

  //        if (pp)
  //          AllNds.InsertBefore(All.Pos(), p);       
  //        else
  //          AllNds.AddTail(p);
  //        }
  //      else
  //        p->iAllIndex=-1;
  //      }
  //    }
  //  }

  SHOWPROGRESS(1, "EO_GotoRunEnd 3");

  int nBad=0, nAct=0;//Dyn=0, nPB=0;
  CFlwNodeIter All(AllNds);
  for (FlwNode * p=All.First(); (p); p=All.Next())
    {
    p->iAllIndex=nAct++;
    if (!p->ConnectionsOK())
      nBad++;
    //dbgpln("All: %4i %s", p->iAllIndex, p->FullObjTag());
    }

  #if (dbgFlwSolve)
  if (nAct!=NdCallCntLen)
    {
    delete NdCallCntArray;
    NdCallCntArray=NULL;
    NdCallCntLen=nAct;
    }
  #endif

  SHOWPROGRESS(1, "EO_GotoRunEnd 4");

  XDataClear();

  if (nBad==0 && OK)
    {
    if (gs_Exec.SolvingPB())
      {
      OK=(PB.InitSolution(CB.m_EqnCB, AllNds)==0);
      SHOWPROGRESS(1, "EO_GotoRunEnd 4.1p");
      CtrlSeq().Initialise();
      SHOWPROGRESS(1, "EO_GotoRunEnd 4.2p");
      DoTraceChanges(TCO_DoInitialise | TCO_DoCalcs | TCO_DoCollect | TCO_DoSort);
      SHOWPROGRESS(1, "EO_GotoRunEnd 4.3p");
      gs_Exec.XGlblResultValidity(false, false);
      SHOWPROGRESS(1, "EO_GotoRunEnd 4.4p");
      }
    else
      {
      OK=(Dyn.Start(CB, AllNds)==0);
      SHOWPROGRESS(1, "EO_GotoRunEnd 4.1d");
      CtrlSeq().Initialise();
      SHOWPROGRESS(1, "EO_GotoRunEnd 4.2d");
      DoTraceChanges(TCO_DoInitialise | TCO_DoCalcs | TCO_DoCollect | TCO_DoSort);
      SHOWPROGRESS(1, "EO_GotoRunEnd 4.3d");
      gs_Exec.XGlblResultValidity(true, true);
      SHOWPROGRESS(1, "EO_GotoRunEnd 4.4d");
      }
    }
  else
    OK=false;
  SHOWPROGRESS(1, "EO_GotoRunEnd 5");

  ASSERT(_CrtCheckMemory());

  gs_MsgLog.SetDefer(false);
  gs_MsgLog.FlushWndQ();
  SHOWPROGRESS(1, "EO_GotoRunEnd 6");

  return OK;
  }

//--------------------------------------------------------------------------

flag CFlwSolver::EO_PreStart(CXM_TimeControl &CB)
  {
  //dbgpln("CFlwSolver::EO_PreStart");
  return true;
  }

//--------------------------------------------------------------------------

flag CFlwSolver::EO_Start(CXM_TimeControl &CB)
  {
  //dbgpln("CFlwSolver::EO_Start");

#if !RESTORE_EO_Start
  return 1;
#endif

#ifndef _RELEASE
  SpModel::SetDoTestEstTP(10);
#endif
  // Set the Time - regardless
  ODE().SetTime(CB.m_TheTime.Seconds);
  //ODE().ClSetItersTotal(0);

  STARTPROGRESS;
  SHOWPROGRESS(10, "EO_Start");

  gs_MsgLog.SetDefer(true);

  // Check All Connected are Active
  FlwNode * p = NULL;
  if (GetObj_All(p))
    {
    p->SetActive(true); // Force Flwsolver to be active
    while (GetObj_All(p))
      if (p->GetActive())
        SetConnectedActive(p);
    }

  bool OK=1;

  SHOWPROGRESS(1, "EO_Start 1");

  if (!CDB.H2OPropertiesSelectedBlk().Class()->LicenseOK())
    {
    OK=0;
    LogError("Solver", 0, "H2O Properties model '%s' not enabled by licensing", CDB.H2OPropertiesSelectedBlk().Class()->ShortDesc());
    }
  if (CDB.Bad())
    {
    OK=0;
    LogError("Solver", 0, "Bad Component Database");
    }

  SHOWPROGRESS(1, "EO_Start 2");

  CFlwNodeList AllNds;

  if (0)
    {
    int NFlwNodes=0;
    for (FlwNode * p=NULL; GetObj_All(p); )
      {
      if (p->GetActive())
        {
        NFlwNodes++;
        AllNds.AddTail(p);
        }
      else
        p->iAllIndex=-1;
      }
    }
  else if (1)
    {
    int NFlwNodes=0;
    for (FlwNode * p=NULL; GetObj_All(p); )
      {
      if (p->GetActive())
        NFlwNodes++;
      }

    m_TempNdPtrs.SetSize(Max(m_TempNdPtrs.GetSize(), NFlwNodes));

    int nObj=0;
    for (FlwNode * p=NULL; GetObj_All(p); )
      {
      if (p->GetActive())
        m_TempNdPtrs[nObj++]=p;
      else
        p->iAllIndex=-1;
      }

    HpSort(NFlwNodes, (void**)&m_TempNdPtrs[0], HpEvalPosn);

    for (int i=0; i<NFlwNodes; i++)
      {
      FlwNode *p=m_TempNdPtrs[i];
      AllNds.AddTail(m_TempNdPtrs[i]);
      }
    }
  //else
  //  {

  //  p = NULL;
  //  if (GetObj_All(p))
  //    {
  //    p->iAllIndex=-1; //skip Flwsolver
  //    //p->SetNodeSolveMode();
  //    for (int nObj=0; GetObj_All(p); nObj++)
  //      {
  //      if (p->GetActive())
  //        {
  //        CFlwNodeIterWithPos All(AllNds);
  //        for (FlwNode * pp=All.First(); (pp); pp=All.Next())
  //          if (pp->m_iEvalPosn>p->m_iEvalPosn)
  //            break;

  //        if (pp)
  //          AllNds.InsertBefore(All.Pos(), p);       
  //        else
  //          AllNds.AddTail(p);
  //        }
  //      else
  //        p->iAllIndex=-1;
  //      }
  //    }
  //  }

  SHOWPROGRESS(1, "EO_Start 3");

  int nBad=0, nAct=0;//Dyn=0, nPB=0;
  CFlwNodeIter All(AllNds);
  for (FlwNode * p=All.First(); (p); p=All.Next())
    {
    p->iAllIndex=nAct++;
    if (!p->ConnectionsOK())
      nBad++;
    //dbgpln("All: %4i %s", p->iAllIndex, p->FullObjTag());
    }

  #if (dbgFlwSolve)
  if (nAct!=NdCallCntLen)
    {
    delete NdCallCntArray;
    NdCallCntArray=NULL;
    NdCallCntLen=nAct;
    }
  #endif

  SHOWPROGRESS(1, "EO_Start 4");

  XDataClear();

  if (nBad==0 && OK)
    {
    if (gs_Exec.SolvingPB())
      {
      OK=(PB.InitSolution(CB.m_EqnCB, AllNds)==0);
      SHOWPROGRESS(1, "EO_Start 4.1p");
      CtrlSeq().Initialise();
      SHOWPROGRESS(1, "EO_Start 4.2p");
      DoTraceChanges(TCO_DoInitialise | TCO_DoCalcs | TCO_DoCollect | TCO_DoSort);
      SHOWPROGRESS(1, "EO_Start 4.3p");
      gs_Exec.XGlblResultValidity(false, false);
      SHOWPROGRESS(1, "EO_Start 4.4p");
      }
    else
      {
      OK=(Dyn.Start(CB, AllNds)==0);
      SHOWPROGRESS(1, "EO_Start 4.1d");
      CtrlSeq().Initialise();
      SHOWPROGRESS(1, "EO_Start 4.2d");
      DoTraceChanges(TCO_DoInitialise | TCO_DoCalcs | TCO_DoCollect | TCO_DoSort);
      SHOWPROGRESS(1, "EO_Start 4.3d");
      gs_Exec.XGlblResultValidity(true, true);
      SHOWPROGRESS(1, "EO_Start 4.4d");
      }
    }
  else
    OK=false;
  SHOWPROGRESS(1, "EO_Start 5");

  ASSERT(_CrtCheckMemory());

  gs_MsgLog.SetDefer(false);
  gs_MsgLog.FlushWndQ();
  SHOWPROGRESS(1, "EO_Start 6");

  return OK;
  }

//--------------------------------------------------------------------------

void CFlwSolver::EO_QuerySubsReqd(CXMsgLst &XM)
  {
  STARTPROGRESS;
  SHOWPROGRESS(10, "EO_QuerySubsReqd");

  FlwNode * pNd=NULL;
  if (GetObj_All(pNd))
    pNd->ClearDDBLists();

  pNd = NULL;
  CXM_ReadIndexedData * pMsg=(CXM_ReadIndexedData *)XM.MsgPtr(XM_ReadIndexedData);
  flag ReadAll=pMsg->ReadAll, FindNd;
  long DataIndex=pMsg->Start ? 0 : pMsg->LastIndex;
  if (pMsg->Start)
    {
    XDataClear();
    FindNd=0;
    }
  else
    {
    FindNd=1;
    }

  XM.Clear();

  pNd = NULL;
  flag Done=0;
  while (GetObj_All(pNd))
    {
    long MdlIndex=-1;
    if (FindNd)
      if (XData[DataIndex].pNd==pNd)
        {
        FindNd=0;
        MdlIndex=XData[DataIndex].lMdlIndex;
        DataIndex++;
        }

    if (!FindNd)
      {
      CXM_DataRequest *pDR=new CXM_DataRequest(0, "", 0, XIO_Null);
      while (pNd->QuerySubsReqd(*pDR, MdlIndex) && !Done)
        {
        FSubsDataItem FI = {pDR->Action, NULL, pNd, MdlIndex};//, NULL};
        XData.Add(FI);
        pDR->Index=DataIndex;
        if (XM.PackMsg(pDR))
          DataIndex++;
        else
          Done=1;
        pDR=new CXM_DataRequest(0, "", 0, XIO_Null);
        }
      delete pDR;
      }
    }

  SHOWPROGRESS(1, "EO_QuerySubsReqd");

  ASSERT(_CrtCheckMemory());
  }

//--------------------------------------------------------------------------

void CFlwSolver::EO_QuerySubsAvail(CXMsgLst &XM, CXMsgLst &XMRet)
  {
  STARTPROGRESS;
  SHOWPROGRESS(10, "EO_QuerySubsAvail");

  TagAccessBlk *p = new TagAccessBlk();

  while (XM.MsgAvail())
    {
    CXM_DataRequest * Msg=(CXM_DataRequest *)XM.MsgPtr(XM_DataRequest);

    if (p==NULL)
      p=new TagAccessBlk();
    p->Init(this/*&FlwSlvNd*/, Msg->Tag, Msg->Options, TU_IndividuallyUnique, true/*Allow Change Management*/);

    //dbgpln("-->> %s", Msg->Tag);
    if (p->LoadAddress(true))
      {
      long DataIndex=XData.GetSize();//Len();
      FSubsDataItem FI = {Msg->Action, p, NULL, 0};//, NULL};
      XData.Add(FI);

      p=NULL;
      XMRet.PackMsg(new CXM_DataAvailable(DataIndex, Msg->Tag, Msg->Action));
      }
    }
  if (p)
    delete p;

  SHOWPROGRESS(1, "EO_QuerySubsAvail");
  }

//--------------------------------------------------------------------------

flag CFlwSolver::EO_ReadSubsData(CXMsgLst &XM)
  {
  STARTPROGRESS;
  SHOWPROGRESS(10, "EO_ReadSubsData");

  flag DataRead=0;
  CXM_ReadIndexedData * pMsg=(CXM_ReadIndexedData *)XM.MsgPtr(XM_ReadIndexedData);
  flag ReadAll=pMsg->ReadAll;
  long DataIndex=pMsg->Start ? 0 : pMsg->LastIndex+1;

  XM.Clear();
  while (DataIndex<XData.GetSize())//Len())
    {
    if (XData[DataIndex].Action & XIO_Out || ReadAll)
      {
      FSubsDataItem &XD = XData[DataIndex];

      CXM_ObjectData *pData=new CXM_ObjectData(DataIndex);

      flag Chg=0;
      TagAccessBlk *pTAB=NULL;
      if (XD.pTAB!=NULL)
        {
        //TagAccessBlk *pTAB=XD.pTAB;
        XD.pTAB->GetDefinedData(pData->List, TABOpt_XRefStatus);
        //pData->SetSize();
        Chg=XD.pTAB->Changed() || ReadAll;
        }
      else if (XD.pNd->ReadSubsData(*pData, XD.lMdlIndex))
        Chg=1;

      if (Chg || LastNotPacked==DataIndex)
        {
        if (XM.PackMsg(pData))
          {
          DataRead=1;
          LastNotPacked=-1;
          if (pTAB)
            pTAB->GotAllDataFromObject();
          }
        else
          {
          LastNotPacked=DataIndex;
          break;
          }
        }
      else
        delete pData;
      }
    DataIndex++;
    }

  SHOWPROGRESS(1, "EO_ReadSubsData");
  return DataRead;
  }

//--------------------------------------------------------------------------

flag CFlwSolver::EO_WriteSubsData(CXMsgLst &XM, flag FirstBlock, flag LastBlock)
  {
  STARTPROGRESS;
  SHOWPROGRESS(10, "EO_WriteSubsData");

  SetGlblTopologyChg(GlblTopyChgFun, (dword)this);
//  return Dyn.WriteSubsData(XM, FirstBlock, LastBlock);
  for (long i=0; i<XM.NoMsgs(); i++)
    {
    CXM_ObjectData *pData = (CXM_ObjectData *)XM.MsgPtr(XM_ObjectData);
    FSubsDataItem &XD = XData[pData->Index];
    if (XD.pTAB!=NULL)
      XD.pTAB->PutDefinedData(pData->List);
    else
      XD.pNd->WriteSubsData(*pData, XD.lMdlIndex);
    }
  SetGlblTopologyChg(NULL, 0);

  SHOWPROGRESS(1, "EO_WriteSubsData");
  return true;
  }

//--------------------------------------------------------------------------

DWORD CFlwSolver::EO_ReadTaggedItem(CXM_ObjectTag & ObjTag, CXM_ObjectData & ObjData, CXM_Route &Route)
  {
  //dbgpln("CFlwSolver Read Once ============================================================");
  //if (!gs_Exec.Running() && !gs_Exec.SolvingPB())
  //  PB.BuildMacroMdls();
  DWORD RetCode = 0;
  TryDoGlobalLinks();
  TagAccessBlk TAB(this, ObjTag.Tag, ObjTag.Options/*|RunModes()*/, TU_IndividuallyUnique);
  if (TAB.LoadAddress(true))
    {
    RetCode = (RC_RTI_Exists | RC_RTI_SolverTag);
    if (TAB.IsNumData())
      RetCode |= RC_RTI_NumData;
    else if (TAB.IsStrng())
      RetCode |= RC_RTI_StrData;
    if (TAB.Flags() & DDEF_PARAM)
      RetCode |= RC_RTI_IsParm;
    ObjData.List.Clear();
    //PkDataListInfo Info(&ObjData.List);
    //if (ObjTag.Options & (TABOpt_ForFile|TABOpt_ForView|TABOpt_ForSnapShot|TABOpt_ForScenario))
    //  {
    //  Info.m_StrMap.InitHashTable(1999);
    //  ObjData.List.SetInfo(&Info);
    //  }
    if ((ObjTag.Options & TABOpt_Exists)==0)
      TAB.GetDefinedData(ObjData.List, 0);
    //ObjData.SetSize();
    }

  return RetCode;
  }

//--------------------------------------------------------------------------

int CFlwSolver::EO_WriteTaggedItem(CXM_ObjectData &ObjData, CXM_Route &Route)
  {
  SetGlblTopologyChg(GlblTopyChgFun, (dword)this);

  CPkDataItem * pItem=ObjData.FirstItem();
  TagAccessBlk TAB(this, pItem->SymOrTag(), ObjData.dwOptions, TU_IndividuallyUnique);
  if (TAB.LoadAddress(true))
    {
    int Ret=TAB.PutDefinedData(ObjData.List);
    SetGlblTopologyChg(NULL, 0);
    return Ret;
    }

  SetGlblTopologyChg(NULL, 0);
  gs_Exec.XGlblResultValidity(true);
  return TOData_NotFound;
  }

//--------------------------------------------------------------------------

flag CFlwSolver::EO_Execute(CXM_TimeControl &CB, CEOExecReturn &EORet)
  {
  //dbgpln("CFlwSolver::EO_Execute");

  CStopWatchLap SWLapOverall(gs_swTotalExecTicks);
  STARTPROGRESS;
  SHOWPROGRESS(10, "EO_Execute 1");

  flag ret;

  ODE().ClearIters4Step();

  gs_MsgLog.SetDefer(true);
  MSMFnRanges::BumpGlblIter();

  #if (dbgFlwSolve)
  SetUpLoopDebug(this);
  #endif

  ASSERT(_CrtCheckMemory());

  if (gs_Exec.SolvingPB())
    {
    EORet.m_Time=CB.m_TheTime+CB.m_StepSize;
    EORet.m_StepSizeNxt=CB.m_StepSize;
    ICSetTime(CB.m_TheTime.Seconds);
    ICSetTimeInc(CB.m_StepSize.Seconds);
    ret=PB.StepSolution(CB.m_EqnCB)!=0;
    DoTraceChanges(TCO_DoCalcs | TCO_DoCollect | TCO_DoSort);
    gs_MsgLog.SetDefer(false);
    gs_MsgLog.FlushWndQ();

    ASSERT(_CrtCheckMemory());

    goto Done;//return ret;
    }

  ASSERT(_CrtCheckMemory());

  //dbgpln("Dyn.Execute >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
  ret=Dyn.Execute(CB, EORet);

  SHOWPROGRESS(1, "EO_Execute 2");

  ASSERT(_CrtCheckMemory());

  if (1)
    {
    CStopWatchLap SWLapChangeTrace(gs_swChangeTrace);
    DoTraceChanges(TCO_DoCalcs | TCO_DoCollect | TCO_DoSort);
    }

  SHOWPROGRESS(1, "EO_Execute 3");

  gs_MsgLog.SetDefer(false);
  gs_MsgLog.FlushWndQ();

  ASSERT(_CrtCheckMemory());

Done:

  #if (dbgFlwSolve)
  if (dbgNoteNdCall())
    {
    dbgpln("============================ Node Call Count ===================================");
    if (NdCallCntArray)
      {
      bool DoIt[eFITMaxCmd];
      LPCTSTR Ttl[eFITMaxCmd];
      for (int i=0; i<eFITMaxCmd; i++)
        {
        Ttl[i]=eFITCmdStr(i);
        if (Ttl[i][0]=='?')
          Ttl[i]=NULL;
        DoIt[i]=(i==0) || Ttl[i-1] || Ttl[i];
        if (!DoIt[i])
          continue;
        for (int j=0; j<i; j++)
          if (DoIt[j])
            dbgp(" |");
        dbgpln(" +-%s", Ttl[i]?Ttl[i]:"");
        }
      for (int j=0; j<i; j++)
        if (DoIt[j])
          dbgp("--");
      dbgpln("--| Pi Pd | Proc | Ci Cd | Ctrl");

      static int InitOrds=1;
      static CArray <FlwNode*,FlwNode*> ProcOrd, CtrlOrd;
      if (InitOrds)
        {
        InitOrds=0;
        ProcOrd.SetSize(0,1000);
        CtrlOrd.SetSize(0,1000);
        }
      for (i=0; i<ProcOrd.GetSize(); i++)
        ProcOrd[i]=NULL;
      for (i=0; i<CtrlOrd.GetSize(); i++)
        CtrlOrd[i]=NULL;

      _asm int 3;
      /*** Cluster Mods
      FlwNode *p=NULL;
      while (GetObj_All(p))
        {
        if (p->m_iProcOrder>=0)
          ProcOrd.SetAtGrow(p->m_iProcOrder, p);
        if (p->m_iCtrlOrder>=0)
          CtrlOrd.SetAtGrow(p->m_iCtrlOrder, p);
        }

      for (int f=0; f<ProcOrd.GetSize(); f++)
        if (ProcOrd[f])
          {
          dbgp("P");
          p=ProcOrd[f];
          for (int i=0; i<eFITMaxCmd; i++)
            if (DoIt[i])
              {
              int c=p->iAllIndex>=0 ? NdCallCntArray[p->iAllIndex*eFITMaxCmd+i] : -1;
              dbgp("%2s", DotNum(c));
              }
          dbgpln(" | %2i %2i | %4s | %2i %2i | %4s %s",
            p->FlwIOsInReqd(), p->m_nFlwIOsInDone, SpcNum(p->m_iProcOrder),
            p->CtrlsInReqd(), p->m_nCtrlsInDone, SpcNum(p->m_iCtrlOrder), p->FullObjTag());
          }
      for (f=0; f<CtrlOrd.GetSize(); f++)
        if (CtrlOrd[f])
          {
          dbgp("C");
          p=CtrlOrd[f];
          for (int i=0; i<eFITMaxCmd; i++)
            if (DoIt[i])
              {
              int c=p->iAllIndex>=0 ? NdCallCntArray[p->iAllIndex*eFITMaxCmd+i] : -1;
              dbgp("%2s", DotNum(c));
              }
          dbgpln(" | %2i %2i | %4s | %2i %2i | %4s %s",
            p->FlwIOsInReqd(), p->m_nFlwIOsInDone, SpcNum(p->m_iProcOrder),
            p->CtrlsInReqd(), p->m_nCtrlsInDone, SpcNum(p->m_iCtrlOrder), p->FullObjTag());
          }
      p=NULL;
      while (GetObj_All(p))
        if (p->m_iProcOrder<0 && p->m_iCtrlOrder<0)
          {
          dbgp(" ");
          for (int i=0; i<eFITMaxCmd; i++)
            if (DoIt[i])
              {
              int c=p->iAllIndex>=0 ? NdCallCntArray[p->iAllIndex*eFITMaxCmd+i] : -1;
              dbgp("%2s", DotNum(c));
              }
          dbgpln(" | %2i %2i | %4s | %2i %2i | %4s %s",
            p->FlwIOsInReqd(), p->m_nFlwIOsInDone, SpcNum(p->m_iProcOrder),
            p->CtrlsInReqd(), p->m_nCtrlsInDone, SpcNum(p->m_iCtrlOrder), p->FullObjTag());
          }
      ***/
      }
    dbgpln("================================================================================");
    }
  #endif

  #if (WITHTIMERS)
  if (dbgDumpStopWatch())
    {
    dbgpln(">>>>>>>>>>>>>>>>>>>>>> %10.6f <<<<<<<<<<<<<<<<<<<<<<", gs_swFlwSolve.StepTime());


    CString PrevGrp;
    for (int i=0; i<gs_swExecute.GetSize(); i++)
      {
      CGlblStopWatch &SW=*gs_swExecute[i];
      dbgpln("  %-10s %-15s %10.6f %10.3f%%", PrevGrp.CompareNoCase(SW.Group())!=0?SW.Group():"", SW.Name(), SW.Time(), 100*SW.TimeFrac());
      PrevGrp=SW.Group();
      }
    for (int i=0; i<gs_swFlwSolve.GetSize(); i++)
      {
      CGlblStopWatch &SW=*gs_swFlwSolve[i];
      dbgpln("  %-10s %-15s %10.6f %10.3f%%", PrevGrp.CompareNoCase(SW.Group())!=0?SW.Group():"", SW.Name(), SW.Time(), 100*SW.TimeFrac());
      PrevGrp=SW.Group();
      }
    }

  #endif

  SHOWPROGRESS(1, "EO_Execute 4");

  return ret;
  }

//--------------------------------------------------------------------------

flag CFlwSolver::EO_StepDone(CXM_TimeControl &CB, flag StepExecuted)
  {
  STARTPROGRESS;
  SHOWPROGRESS(10, "EO_StepDone");

  //dbgpln("Test All XRefs Cnts ==============");
  CTagRefMap::CPair *pPair=m_TagRefsMap.PGetFirstAssoc();
  while (pPair)
    {
    CTagRefsMapItem *pMI=pPair->value;
    CTagRefStatus   W=pMI->GetStatusWord();
    //dbgpln("Test : %04x %s %s", W, (W & FFM_HasMultiSetCnts)?"Multi":"     ",(LPCTSTR)pMI->m_sTag);
    if (W & FFM_HasMultiSetCnts)
      {
      if (!pMI->m_bErrorDone)
        {
        pMI->m_bErrorDone=true;
        CString S;
        S.Format("Multiple Sets on %s {", (LPCTSTR)pMI->m_sTag);

        POSITION Pos=pMI->m_pTagRefs.GetHeadPosition();
        int iX=0;
        while (Pos && iX<5)
          {
          CTagRef * pItem=pMI->m_pTagRefs.GetNext(Pos);
          if (pItem->SetCnt()>0)
            {
            if (iX++>0)
              S+=",";
            S+=pItem->m_sRmtTag();////SrcNd() ? pItem->SrcNd()->Tag() : "?SrcTg?";
            }
          }

        if (Pos)
          {
          POSITION Pos=pMI->m_pTagRefs.GetHeadPosition();
          int i=1;
          while (Pos)
            {
            pMI->m_pTagRefs.GetNext(Pos);
            i++;
            }
          CString SN;
          SN.Format(",%i Others", i);
          S+=SN;
          //S+=",...";
          }
        S+="}";
        LogError("Solver", 0, S);
        }
      }
    else
      pMI->m_bErrorDone=false;
    pPair=m_TagRefsMap.PGetNextAssoc(pPair);
    }

  gs_FSIdleRequested=false;//reset flag after iteration

  //if (gs_Exec.SolvingPB())
  //  {
  //  PB.StepDone();
  //  goto true;
  //  }

  //Dyn.StepDone();

  SHOWPROGRESS(1, "EO_StepDone");
  return true;
  };

//--------------------------------------------------------------------------

flag CFlwSolver::EO_QueryStop(CXM_TimeControl &CB, bool Starting)
  {
  if (!gs_Exec.SolvingPB())
    return Dyn.CheckStopRequired(CB, Starting, true);
    //return Dyn.m_StopRequired;
  return false;
  };

flag CFlwSolver::EO_Stop(CXM_TimeControl &CB)
  {
  STARTPROGRESS;
  SHOWPROGRESS(10, "EO_Stop");
  //XDataClear();

  gs_MsgLog.SetDefer(true);

  flag Ret;
  if (gs_Exec.SolvingPB())
    {
    Ret=PB.TermSolution(CB.m_EqnCB)!=0;
    gs_Exec.XGlblResultValidity(-1, false);
    }
  else
    {
    Ret=Dyn.Stop(CB);
    gs_Exec.XGlblResultValidity(true, false);
    }

  DoTraceChanges(TCO_DoCalcs | TCO_DoCollect | TCO_DoSort);

  gs_MsgLog.SetDefer(false);
  gs_MsgLog.FlushWndQ();

  //gs_EventLog.CloseFile();

  SHOWPROGRESS(1, "EO_Stop");
  return Ret;
  }

//--------------------------------------------------------------------------

flag CFlwSolver::EO_Stopping(flag BeginStopping)
  {
  STARTPROGRESS;
  SHOWPROGRESS(10, "EO_Stopping");

  if (!BeginStopping)
    XDataClear();

  ASSERT(_CrtCheckMemory());

  #if dbgFlwSolve
  if (dbgDumpSurfaces())
    {
    for (int i=0; i<CSurfaceCommon::List.GetSize(); i++)
      CSurfaceCommon::List[i]->Dump();
    }
  #endif

  #if (dbgFlwSolve)
  NdCallCntLen=0;
  delete NdCallCntArray;
  NdCallCntArray=NULL;
  #endif

  SHOWPROGRESS(1, "EO_Stopping");
  return true;//Ret;
  }

//---------------------------------------------------------------------------

flag CFlwSolver::EO_BeginPause(CXM_TimeControl &CB)
  {
  if (gs_Exec.SolvingPB())
    {
    PB.TestClosure(PB.ProcTOSequence);
    }
  return true;
  };

//---------------------------------------------------------------------------

flag CFlwSolver::EO_EndPause(CXM_TimeControl &CB)
  {
  return true;
  };

//---------------------------------------------------------------------------

flag CFlwSolver::EO_BeginSave(FilingControlBlock &FCB)
  {
  EqnSlv().FindAllTearsInUse();

  UnDoGlobalLinks();
  if (fGlblLinksDone)
    {
    LogError(Tag(), LF_Exclamation, "UnDoGlobalLinks FAILED");
    }

  TRACE1("CFlwSolver - Save BEGIN %s\n", FCB.Label());

  Label=FCB.Label();
  iSaveWhat=0;
  //fSaveNode=true;
  pSaveNode=NULL;
  //fSaveTear=true;
  pSaveTear=TearVarBlk::List.GetHeadPosition();

  //CProfINIFile PF(PrjFile());
  //PF.WrStr("General", "CnvDB",   CnvsFilename());
  ////PF.WrStr("General", "LibSDB",  LibSDBName());
  //PF.WrStr("General", "PrjSDB",  PrjSDBName());

  FlwNode * p=NULL;
  while (GetObj_All(p))
    p->OnSave(FCB.Label());
  return true;
  };

//---------------------------------------------------------------------------

flag CFlwSolver::EO_SaveDefinedData(FilingControlBlock &FCB, Strng &Tag, CXMsgLst &XM)
  {
  //TABOptions Opts= (FCB.Type()==TOF_DBase) ? TABOpt_ForFile|TABOpt_AllInfo : TABOpt_ForSnapShot/*|TABOpt_AllInfo*/;
  TABOptions Opts;
  switch (FCB.Type())
    {
    case TOF_DBase:    Opts = NSHM_All|TABOpt_ForFile|TABOpt_AllInfo; break;
    case TOF_SnapShot: Opts = NSHM_All|TABOpt_ForSnapShot; break;
    case TOF_Scenario: Opts = NSHM_All|TABOpt_ForScenario; break;
    }

  typedef enum {
    SW_0,
    SW_1,
    SW_2,
    SW_3,
    SW_4,
#if WITHCHANGEOBJ
    SW_5,
#endif
    SW_6,
    SW_7,
#if WITHCTRLOBJ
    SW_CTRL,
#endif
#if WITHORDEROBJ
    SW_ORDER,
#endif
    SW_POWER,
#if WITHIOOBJ
    SW_IO
#endif
    } SWTCHNO ;
//  #if WITHCTRLOBJ && WITHORDEROBJ
//    #define SW_CTRL  8
//    #define SW_ORDER 9
//    #define SW_POWER 10
//  #elif WITHCTRLOBJ
//    #define SW_CTRL  8
//    #define SW_POWER 9
//  #elif WITHORDEROBJ
//    #define SW_ORDER 8
//    #define SW_POWER 9
//  #else
//    #define SW_POWER 8
//  #endif
//  #if WITHIOOBJ
//    #define SW_IO (SW_POWER+1)
//  #endif
  CXM_ObjectData *pData=new CXM_ObjectData(0);
  switch ((SWTCHNO)iSaveWhat)
    {
    case SW_0:
      while (GetObj_All(pSaveNode))
        if (!pSaveNode->Internal)
          break;
      if (pSaveNode)
        {
        Tag=pSaveNode->Tag();
        pSaveNode->GetDefinedData(Tag(), pData->List, Opts, NULL);
        //pData->SetSize();
        XM.PackMsg(pData);
        pSaveNode->ClearDDBLists();
        return true;
        }
      iSaveWhat++;
    case SW_1:
      iSaveWhat++;
      Tag=PB.Tag();
      PB.GetDefinedData(Tag(), pData->List, Opts, NULL);
      //pData->SetSize();
      XM.PackMsg(pData);
      PB.ClearDDBLists();
      return true;
    case SW_2:
      iSaveWhat++;
      Tag=Dyn.Tag();
      Dyn.GetDefinedData(Tag(), pData->List, Opts, NULL);
      //pData->SetSize();
      XM.PackMsg(pData);
      Dyn.ClearDDBLists();
      return true;
    case SW_3:
      iSaveWhat++;
      Tag=SDBObj.Tag();
      SDBObj.GetDefinedData(Tag(), pData->List, Opts, NULL);
      //pData->SetSize();
      XM.PackMsg(pData);
      SDBObj.ClearDDBLists();
      return true;
    case SW_4:
      iSaveWhat++;
      Tag=TearObj.Tag();
      TearObj.GetDefinedData(Tag(), pData->List, Opts, NULL);
      //pData->SetSize();
      XM.PackMsg(pData);
      TearObj.ClearDDBLists();
      return true;
#if WITHCHANGEOBJ
    case SW_5:
      iSaveWhat++;
      Tag=ChgObj.Tag();
      ChgObj.GetDefinedData(Tag(), pData->List, Opts, NULL);
      //pData->SetSize();
      XM.PackMsg(pData);
      ChgObj.ClearDDBLists();
      return true;
#endif
    case SW_6:
      iSaveWhat++;
      Tag=TolObj.Tag();
      TolObj.GetDefinedData(Tag(), pData->List, Opts, NULL);
      //pData->SetSize();
      XM.PackMsg(pData);
      TolObj.ClearDDBLists();
      return true;
    case SW_7:
      if (pSaveTear)
        {
        TearVarBlk *pT=TearVarBlk::List.GetNext(pSaveTear);
        if (pT)
          {
          Tag=pT->FullObjTag();
          pT->GetDefinedData(Tag(), pData->List, Opts, NULL);
          //pData->SetSize();
          XM.PackMsg(pData);
          pT->ClearDDBLists();
          return true;
          }
        }
      else
        iSaveWhat++;
      // TO KGA have you been able to test that the objects below here get saved ???
      // CNM - change to return tue - stuff below did not get saved
      return true;
#if WITHCTRLOBJ
    case SW_CTRL:
      iSaveWhat++;
      Tag=CtrlObj.Tag();
      CtrlObj.GetDefinedData(Tag(), pData->List, Opts, NULL);
      //pData->SetSize();
      XM.PackMsg(pData);
      CtrlObj.ClearDDBLists();
      return true;
#endif
#if WITHORDEROBJ
    case SW_ORDER:
      iSaveWhat++;
      Tag=OrderObj.Tag();
      OrderObj.GetDefinedData(Tag(), pData->List, Opts, NULL);
      //pData->SetSize();
      XM.PackMsg(pData);
      OrderObj.ClearDDBLists();
      return true;
#endif
    case SW_POWER:
      iSaveWhat++;
      Tag=PwrObj.Tag();
      PwrObj.GetDefinedData(Tag(), pData->List, Opts, NULL);
      //pData->SetSize();
      XM.PackMsg(pData);
      PwrObj.ClearDDBLists();
      return true;
#if WITHIOOBJ
    case SW_IO:
      iSaveWhat++;
      Tag=IOObj.Tag();
      IOObj.GetDefinedData(Tag(), pData->List, Opts, NULL);
      //pData->SetSize();
      XM.PackMsg(pData);
      IOObj.ClearDDBLists();
      return true;
#endif
    default:
      break;
    }

  delete pData;
  return false;
  };

//---------------------------------------------------------------------------

flag CFlwSolver::EO_SaveOtherData(FilingControlBlock &FCB)
  {
  if (pSaveNode)
    return pSaveNode->GetOtherData(FCB);
  return false;
  }

//---------------------------------------------------------------------------

flag CFlwSolver::EO_SaveConn(FilingControlBlock &FCB, CXMsgLst &XM)
  {
  while (GetObj_All(pSaveNode))
    {
    if (!pSaveNode->Internal)
      {
      int nios=0;
      int Start[4] = {0,                             pSaveNode->CIO1(), pSaveNode->EIO1(), pSaveNode->AIO1()};
      int End[4] =   {pSaveNode->NoProcLnkIOs(),     pSaveNode->CION(), pSaveNode->EION(), pSaveNode->AION()};
      for (int pass=0; pass<4; pass++)
        {
        for (int i=Start[pass]; i<End[pass]; i++)
          {
          FlwNode * pSelf=pSaveNode;
          FlwNode * pRmt=pSaveNode->Nd_Rmt(i);
          if (!pSaveNode->Internal || !pSaveNode->Nd_Rmt(i)->Internal)
            nios++;
          }
        }
      if (nios>0)
        break;
      }
    }

  if (pSaveNode)
    {
    int Start[4] = {0,                          pSaveNode->CIO1(), pSaveNode->EIO1(), pSaveNode->AIO1()};
    int End[4] =   {pSaveNode->NoProcLnkIOs(),  pSaveNode->CION(), pSaveNode->EION(), pSaveNode->AION()};
    for (int pass=0; pass<4; pass++)
      {
      for (int i=Start[pass]; i<End[pass]; i++)
        {
        FlwNode * pSelf=pSaveNode;
        FlwNode * pRmt=pSaveNode->Nd_Rmt(i);
        if (!pSelf->Internal || !pRmt->Internal)
          {
          Strng STag, DTag, TTag, CTag;

          //        TTag=pSelf->FullIOTag(OId_NodeIO, pSelf->IOs[i].IODescSelf, TU_IndividuallyUnique, false);
          TTag=pSelf->FullIOTag(OId_NodeIO, pSelf->IODescNo_Self(i), TU_IndividuallyUnique, false);
          while (pSelf->Internal)
            pSelf=pSelf->Parent();
          STag=strstr(TTag(), pSelf->Tag());

          int j=pSaveNode->IOIONo_Rmt(i);
          //        TTag=pRmt->FullIOTag(OId_NodeIO, pRmt->IOs[j].IODescSelf, TU_IndividuallyUnique, false);
          TTag=pRmt->FullIOTag(OId_NodeIO, pRmt->IODescNo_Self(j), TU_IndividuallyUnique, false);
          while (pRmt->Internal)
            pRmt=pRmt->Parent();
          DTag=strstr(TTag(), pRmt->Tag());

          flag Swap= (STag.XStrICmp(DTag()) > 0);
          pchar pSrc=(Swap ? DTag() : STag());
          pchar pDst=(Swap ? STag() : DTag());
          CTag.Concat(pSrc, ".", pDst);

          CXM_ObjectData *pOD= new CXM_ObjectData(0, 0);//, &List);
          CPkDataList    & List= pOD->List;
          //PkDataListInfo  ListInfo(&List);
          PkDataUnion   DU;
          //CPkDataItem *   pPItem=List.FirstItem();

          List.SetStructureS(/*pPItem,*/ tt_Object, CTag(), "Connect", "", "", "", NULL, NULL, 0);

          DU.SetTypeString(tt_Strng, STag());
          List.SetDataValue(/*pPItem,*/ "Src", DU);

          DU.SetTypeString(tt_Strng, DTag());
          List.SetDataValue(/*pPItem,*/ "Dst", DU);

          DU.SetTypeLong(tt_Long, i);
          // CNM Changed InIONo to DstIONo
          List.SetDataValue(/*pPItem,*/ "SrcIONo", DU);

          DU.SetTypeLong(tt_Long, j);
          List.SetDataValue(/*pPItem,*/ "DstIONo", DU);

          CConnect *pConn=NULL;
          switch (pass)
            {
            case 0:  pConn=dynamic_cast<CConnect*>(pSelf->IOFlange(i)); break;
            case 1:  pConn=dynamic_cast<CConnect*>(pSelf->Ctrl_Terminal(i)); break;
            //case 2:  pConn=dynamic_cast<CConnect*>(pSelf->IOFlange(i)); break;
            //case 3:  pConn=dynamic_cast<CConnect*>(pSelf->IOFlange(i)); break;
            }
          if (pConn)
            {
            DU.SetTypeString(tt_Strng, pConn->Tag());
            List.SetDataValue(/*pPItem,*/ "ConnectTag", DU);
            DU.SetTypeLong(tt_Long, pConn->UseStatus());
            List.SetDataValue(/*pPItem,*/ "UseStatus", DU);
            }

          List.SetStructureE(/*pPItem,*/ StructEndType(tt_Object), CTag());

          //CXM_ObjectData *pOD= new CXM_ObjectData(0, 0, &List);

          XM.PackMsg(pOD);
          }
        }
      }
    return true;
    }

  return false;
  };

//---------------------------------------------------------------------------

flag CFlwSolver::EO_EndSave(FilingControlBlock &FCB)
  {
  if (TaggedObject::Revert2DefaultRqd())
    {
    FlwNode* p=NULL;
    while (GetObj_All(p))
      p->Revert2Default();
    }
  TryDoGlobalLinks();
//  dbgpln("GetDataFromObj:%g",xx);
  TRACE0("CFlwSolver - Save END\n");

  return false;
  };

//---------------------------------------------------------------------------

flag CFlwSolver::EO_BeginLoad(FilingControlBlock &FCB)
  {
  gs_PwrSupplies.Terminate();
  gs_PwrSupplies.Initialise(true);

  TRACE0("CFlwSolver - Load BEGIN\n");
  Label=FCB.Label();
#if dbgTraceMemState
  dbgMemoryState("Flw Start Load");
#endif

  UnDoGlobalLinks();

  m_bMustCreateObjsFromFile = (FCB.Type()==TOF_DBase) || !GlobalLinksBusy();
  if (m_bMustCreateObjsFromFile)
    EqnSlv().RemoveAllTears();
  return true;
  };

//---------------------------------------------------------------------------

flag CFlwSolver::EO_LoadDefinedData(FilingControlBlock &FCB, CXMsgLst &XM)
  {
  SetGlblTopologyChg(GlblTopyChgFun, (dword)this);

  flag Loaded=true;
  while (XM.MsgAvail())
    {
    CXM_ObjectData *pData=(CXM_ObjectData *)XM.MsgPtr(XM_ObjectData);
    CPkDataItem * pItem=pData->FirstItem();

    if (pItem && pItem->Type()==tt_Object)
      {
      #if (dbgTraceMemUse1 && defined(_DEBUG))
      dbgMemDumpBlk MDP;
      if (dbgMemoryUse())
        dbgGetMemStats(MDP);
      #endif

      int ObjTagLen=0;

      pTaggedObject pObj=NULL;

      // CHeck to see whether this is a Tear type class ?
      TagObjClass *pClassDef= TagObjClass::FindClassId(pItem->Class(), true);
      TearVarBlkClassDef* pTVBClassDef = dynamic_cast<TearVarBlkClassDef*>(pClassDef);  //error: pb2 points to a B, not a D
//      TearVarBlkClassDef* pTVBClassDef = NULL;//dynamic_cast<TearVarBlkClassDef*>(pClassDef); //error: pb2 points to a B, not a D
//      try
//        {
//        pTVBClassDef = dynamic_cast<TearVarBlkClassDef*>(pClassDef);  //error: pb2 points to a B, not a D
//        }
//      catch  (bad_cast* e)
//        {
//        int cxxx=0;
//        }
//      catch  (...)
//        {
//        int cxxx=0;
//        };

      if (pTVBClassDef)
        {
        pObj=TearVarBlk::Find(pItem->SymOrTag());
        if ((pObj==NULL) && m_bMustCreateObjsFromFile)
          {
          if (pClassDef)
            pObj=TearVarBlk::Add(static_cast<TearVarBlk*>(pClassDef->Construct(NULL, pItem->SymOrTag(), NULL, TOA_Free)));
          }
        if (pObj)
          {
          CPkDataIter Iter;
          DWORD dwOptions=TABOpt_ForFile|TABOpt_AllInfo;
          pObj->PutDefinedData(pItem->SymOrTag(), pData->List, Iter, dwOptions, NULL);
          }
        else if (m_bMustCreateObjsFromFile && FCB.CreateObj())
          {
          Loaded=false;
          LogError(pItem->SymOrTag(), LF_Exclamation, "Object %s Not Constructed", pItem->Class());
          }
        }
      else
        {
        pObj=FindObjTag(pItem->SymOrTag(), false, ObjTagLen, 0);
        if ((pObj==NULL) && (FCB.Type()==TOF_DBase))
          {
          // Prevent any other FlwSolvers Etc being created
          if (_stricmp(pItem->Class(), CFlwSolverClass.ClassId())==0)
            goto Done;
          if (_stricmp(pItem->Class(), FlwSlvPBClass.ClassId())==0)
            goto Done;
          if (_stricmp(pItem->Class(), FlwSlvDynClass.ClassId())==0)
            goto Done;
          if (_stricmp(pItem->Class(), SDBObjectClass.ClassId())==0)
            goto Done;
          if (_stricmp(pItem->Class(), TearObjectClass.ClassId())==0)
            goto Done;
#if WITHCTRLOBJ
          if (_stricmp(pItem->Class(), CtrlObjectClass.ClassId())==0)
            goto Done;
#endif
#if WITHCHANGEOBJ
          if (_stricmp(pItem->Class(), ChgObjectClass.ClassId())==0)
            goto Done;
#endif
          if (_stricmp(pItem->Class(), TolObjectClass.ClassId())==0)
            goto Done;
#if WITHORDEROBJ
          if (_stricmp(pItem->Class(), OrderObjectClass.ClassId())==0)
            goto Done;
#endif
          if (_stricmp(pItem->Class(), PwrObjectClass.ClassId())==0)
            goto Done;
#if WITHIOOBJ
          if (_stricmp(pItem->Class(), IOObjectClass.ClassId())==0)
            goto Done;
#endif
          pObj=InsertNode(this, pItem->Class(), pItem->SubClass(), pItem->PrimaryCfg(), pItem->SymOrTag(), NULL, TOA_Free);//pchar TypIdIn, pchar TagIn)
Done:;
          }

        if (pObj)
          {
          if (FCB.Type()==TOF_DBase || !pObj->bSkipSnapLoad)
            {
            TagAccessBlk TAB(this, pItem->SymOrTag(), TABOpt_ForFile|TABOpt_AllInfo, TU_IndividuallyUnique);
            if (TAB.LoadAddress(true))
              {
              TAB.PutDefinedData(pData->List);
              TAB.ClearDDBLists();
              //Loaded=true;
              }
            }
          }
        else if ((_stricmp(pItem->Class(), "TbPID")==0) ||
                 (_stricmp(pItem->Class(), "TbPID2")==0))
          {
          LogNote(pItem->SymOrTag(), 0, "Object %s Removed", pItem->Class());
          }
#if !WITHCTRLOBJ
        else if (_stricmp(pItem->Class(), "FS_Ctrl")==0)
          {
          LogNote(pItem->SymOrTag(), 0, "Object %s Skipped", pItem->Class());
          }
#endif
        else if (FCB.Type()==TOF_DBase && FCB.CreateObj())
          {
          Loaded=false;
          LogError(pItem->SymOrTag(), LF_Exclamation, "Object %s Not Constructed", pItem->Class());
          }
        }

      #if (dbgTraceMemUse1 && defined(_DEBUG))
      if (dbgMemoryUse())
        dbgDumpMemBlks(MDP, pItem->TagStr());
      #endif

      #if dbgTraceMemState1
      dbgMemoryState("Load Node     ");
      #endif
      }
    }
  SetGlblTopologyChg(NULL, 0);
  return Loaded;
  };

//---------------------------------------------------------------------------

flag CFlwSolver::EO_LoadOtherData(FilingControlBlock &FCB)
  {
  int ObjTagLen=0;
  TaggedObject *pObj=CTNode::FindObjTag(FCB.ObjTag(), false, ObjTagLen, 0);
  if (pObj)
    {
    pObj->PutOtherData(FCB);
    return true;
    }
  LogError("Load SnapShot", LF_Exclamation, "Object %s Not Found", FCB.ObjTag());
  return false;
  }

//---------------------------------------------------------------------------

flag CFlwSolver::EO_LoadConn(FilingControlBlock &FCB, CXMsgLst &XM)
  {
  while (XM.MsgAvail())
    {
    CXM_ObjectData *pData=(CXM_ObjectData *)XM.MsgPtr(XM_ObjectData);
    CPkDataIter Iter;
    CPkDataItem * pItem=pData->FirstItem(Iter);

    //pItem=pData->NextItem(pItem);// Skip Connect Object Start Name
    if (pItem)
      {
      #if (dbgTraceMemUse1 && defined(_DEBUG))
      dbgMemDumpBlk MDP;
      if (dbgMemoryUse())
        dbgGetMemStats(MDP);
      #endif
      pchar pSrc=NULL;
      pchar pDst=NULL;
      pchar pConnectTag=NULL;
      int SIONo=-1;
      int DIONo=-1;
      int UseStatus=0;

      while (pItem)
        {
        pchar pTag=pItem->SymOrTag();
        if (_stricmp(pTag, "Src")==0)
          pSrc=pItem->Value()->GetString();
        else if (_stricmp(pTag, "Dst")==0)
          pDst=pItem->Value()->GetString();
        else if (_stricmp(pTag, "ConnectTag")==0)
          pConnectTag=pItem->Value()->GetString();
        else if (_stricmp(pTag, "UseStatus")==0)
          UseStatus=pItem->Value()->GetLong();
        // Renamed InIONo to SrcIONo
        else if (_stricmp(pTag, "SrcIONo")==0 || _stricmp(pTag, "InIONo")==0 || _stricmp(pTag, "DstIONo")==0)
          {
          int i;
          if (pItem->Type()==tt_Generic)
            {
            pchar psio=pItem->Value()->GetString();
            i=(psio && isdigit(*psio)) ? atol(psio) : -1;
            }
          else if (IsIntData(pItem->Type()))
            i=pItem->Value()->GetLong();
          else
            i=-1;
          if (pTag[0]=='I' || pTag[0]=='S')
            SIONo=i;
          else
            DIONo=i;
          }
        pItem=pData->NextItem(Iter);// Skip to Next
        }

      ConnectTag(m_pRoot, pSrc, pDst, SIONo, DIONo, pConnectTag, UseStatus);
      #if dbgTraceMemState1
            dbgMemoryState("Connect       ");
      #endif
      #if (dbgTraceMemUse1 && defined(_DEBUG))
      char buff[512];
      sprintf(buff, "%s > %s", pSrc, pDst);
      if (dbgMemoryUse())
        dbgDumpMemBlks(MDP, buff);
      #endif
      }
    }
  return true;
  };


//---------------------------------------------------------------------------

flag CFlwSolver::EO_EndConnect(FilingControlBlock &FCB)
  {
  FlwNode *p=NULL;
  while (GetObj_All(p))
    p->CleanupConnections();

  return true;
  }
//---------------------------------------------------------------------------

flag CFlwSolver::EO_EndLoad(FilingControlBlock &FCB)
  {
  if (iLclResultsValid>=0)
    {
    gs_Exec.XGlblResultValidity(iLclResultsValid);
    iLclResultsValid=-1;
    }

  ASSERT(_CrtCheckMemory());

#if dbgTraceMemState
  dbgMemoryState("Flw End Load A");
#endif
  TRACE0("CFlwSolver - Load END\n");
  FlwNode * p=NULL;
  while (GetObj_All(p))
    {
    p->OnLoad(Label());
#ifdef _DEBUG
    if (_stricmp(p->Tag(), "T_125")==0)
      {
      int xxx=0;
      }
#endif
    }
#if dbgTraceMemState
  dbgMemoryState("Flw End Load B");
#endif
  StructureChanged(NULL);

  ASSERT(_CrtCheckMemory());

  TryDoGlobalLinks();

  m_bMustCreateObjsFromFile=false;

  return true;
  };


//--------------------------------------------------------------------------

void CFlwSolver::ADInit()
  {
  if (fADInited)
    return;
  TagObjClass *pC=(TagObjClass*)TagObjClass::FindGroupId(ADObjectGrp);
  for (int Index=0; pC; Index++)
    {
    ADGrps.SetSize(Index+1);
    CADGroup &ADG=ADGrps[Index];
    ADG.pClass=pC;
    Strng SDesc=pC->ShortDesc();
    int l=SDesc.Length();
    int p=SDesc.XStrChr(':')-SDesc();
    ADG.Name=SDesc.Right(l-p-1);
    ADG.Class=SDesc.Left(p);

    pC=pC->NextClassInGrp();
    }
  fADInited=true;
  }

//--------------------------------------------------------------------------

int CFlwSolver::DuplicateNetwork(CSArray & SelectedTags, FlwNode * pRoot)
  {
  UnDoGlobalLinks();

  #if dbgFlwSolve
  dbgDuplicate.PgmSet(1);
  if (dbgDuplicate())
    dbgpln("Duplicate Network");
  #endif

  int NTags=SelectedTags.GetSize();
  CFlwNodeArray SrcNds;
  SrcNds.SetSize(NTags);

  CFlwNodeArray Nds;
  CIArray PreExisted;
  Nds.SetSize(NTags);
  PreExisted.SetSize(NTags);

  // Remove Unneeded - part 1
  FlwNode *p=pRoot;
  while (pRoot->GetObj_Tree(p))
    p->Busy = false;

  for (int t=0; t<SelectedTags.GetSize(); t++)
    {
    char * pTag=SelectedTags[t]();
    int MinTagLen=0;
    int ObjTagLen=0;
    SrcNds[t]=(FlwNode*)CTNode::FindObjTag(pTag, true, ObjTagLen, MinTagLen);
    Nds[t]=(FlwNode*)pRoot->CTNode::FindObjTag(pTag, true, ObjTagLen, MinTagLen);
    PreExisted[t]=Nds[t]!=NULL;
    #if dbgFlwSolve
    if (dbgDuplicate())
      if (PreExisted[t])
        dbgpln("  Exists : %s", pTag);
    #endif
    }

  for (t=0; t<SelectedTags.GetSize(); t++)
    {
    char * pTag=SelectedTags[t]();
    int MinTagLen=0;
    int ObjTagLen=0;
    if (PreExisted[t])
      Nds[t]->Busy=true;
    else
      {
      #if dbgFlwSolve
      if (dbgDuplicate())
        dbgpln("  Insert : %s", pTag);
      #endif
      Nds[t]=pRoot->InsertNode(pRoot, SrcNds[t]->ClassId(), SrcNds[t]->GetPrimaryCfg(), NULL, pTag, NULL, TOA_Free);
      if (Nds[t]==NULL)
        {
        DoBreak(); // should never get here
        }
      Nds[t]->Busy=true;
      }
    }

  Strng_List Conns;
  for (int tsrc=0; tsrc<SelectedTags.GetSize(); tsrc++)
    if (!PreExisted[tsrc])
    {
    FlwNode *p=SrcNds[tsrc];
    for (int i=0; i<p->NoProcessIOs(); i++)
      {
      FlwNode * pSelf=p;
      FlwNode * pRmt=p->Nd_Rmt(i);
      Strng RmtTag(pRmt->FullObjTag());
      flag RmtSelected=false;
      for (int tdst=0; !RmtSelected && tdst<SelectedTags.GetSize(); tdst++)
        if (SelectedTags[tdst].XStrICmp(RmtTag())==0)
          RmtSelected=true;

      if (RmtSelected && /*!PreExisted[tdst] &&*/ (!pSelf->Internal || !pRmt->Internal))
        {
        Strng STag, DTag, TTag, CTag, IONo;//, TmpTag;

        TTag=pSelf->FullIOTag(OId_NodeIO, pSelf->IODescNo_Self(i), TU_IndividuallyUnique, false);
        while (pSelf->Internal)
          pSelf=pSelf->Parent();
        STag=strstr(TTag(), pSelf->Tag());

        int j=p->IOIONo_Rmt(i);
        TTag=pRmt->FullIOTag(OId_NodeIO, pRmt->IODescNo_Self(j), TU_IndividuallyUnique, false);
        while (pRmt->Internal)
          pRmt=pRmt->Parent();
        DTag=strstr(TTag(), pRmt->Tag());

        flag Swap= (STag.XStrICmp(DTag()) > 0);
        pchar pSrc=(Swap ? DTag() : STag());
        pchar pDst=(Swap ? STag() : DTag());
        CTag.Concat(pSrc, ".", pDst);

        for (Strng *pConn=Conns.First(); pConn; pConn=Conns.Next())
          if (pConn->XStrICmp(CTag)==0)
            break;

        if (pConn==NULL)
          {
          // Not found Make Connection;
          int xxx=0;
          Conns.Append(CTag());
          #if dbgFlwSolve
          if (dbgDuplicate())
            dbgpln("  Connect %s", CTag());
          #endif

          int SIONo=i, DIONo=j;
          pRoot->ConnectTag(pRoot, STag(), DTag(), SIONo, DIONo);
          }
        }
      }
    }

  // Remove Unneeded - part 2
  p=pRoot;
  pRoot->GetObj_Tree(p);
  while (p)
    if (p->Busy)
      {
      pRoot->GetObj_Tree(p);
      }
    else
      {
      FlwNode *pDel=p;
      pRoot->GetObj_Tree(p);
      #if dbgFlwSolve
      if (dbgDuplicate())
        dbgpln("  Delete %s ",pDel->FullObjTag());
      #endif
      pDel->DisConnectAll();
      delete pDel;
      }

  return 0;
  };

//--------------------------------------------------------------------------

int CFlwSolver::AnalyseNetwork(CFlwTree & Tree)
  {
  // Simple;
  FlwNode *pN=NULL;
  CFlwTreeItem *pI=NULL;
  if (GetObj_Tree(pN))
    {
    Tree.sText=pN->FullObjTag();
    int xxx=3;
    while (GetObj_Tree(pN))
      {
      if (pI)
        pI=pI->AddSibling(pN->FullObjTag(), 0, xxx++);//3);
      else
        pI=Tree.AddChild(pN->FullObjTag(), 0, 0);
      }
    }
  return 0;
  };

//--------------------------------------------------------------------------

int CFlwSolver::FE_AD_GetID(int Index, Strng & Class, Strng & Name)
  {
  if (!fADInited)
    ADInit();
  if (Index>=0 &&Index<ADGrps.GetSize())
    {
    CADGroup &ADG=ADGrps[Index];
    Name  = ADG.Name;
    Class = ADG.Class;
    return true;
    }
  return false;
  };

//--------------------------------------------------------------------------

int CFlwSolver::FE_AD_Select(int Index, char * Tag)
  {
  if (Index>=0 &&Index<ADGrps.GetSize())
    {
    CADGroup &ADG=ADGrps[Index];
    for (int i=0; i<ADG.Obj.GetSize(); i++)
      if (TaggedObject::TagCmp(ADG.Obj[i]->Tag(), Tag)>0)
        break;
    if (i>=ADG.Obj.GetSize())
      {
      CADObject * p= (CADObject*)(ADG.pClass->Construct(NULL, Tag, NULL, TOA_Free));
      ADG.Obj.SetAtGrow(i, p);
      p->pSrcFS=this;
      p->pFS=new CFlwSolver(&CFlwSolverClass, Tag, NULL, TOA_Free);
      }
    m_ADIndexGrp=Index;
    m_ADIndexObj=i;
    return 1;
    }
  m_ADIndexGrp=-1;
  m_ADIndexObj=-1;
  return 0;
  };

//--------------------------------------------------------------------------

int CFlwSolver::FE_AD_SetSelectedTags(Strng_List &SelectedTags)
  {
  if (m_ADIndexGrp<0)
    return 0;
  CADGroup  &ADG=ADGrps[m_ADIndexGrp];
  CADObject &ADO=*ADG.Obj[m_ADIndexObj];

  return ADO.SetSelectedTags(SelectedTags);
  };

//--------------------------------------------------------------------------

int CFlwSolver::FE_AD_GetTagTree (CFlwTree & Tree)
  {
  if (m_ADIndexGrp<0)
    return 0;
  CADGroup  &ADG=ADGrps[m_ADIndexGrp];
  CADObject &ADO=*ADG.Obj[m_ADIndexObj];

  return ADO.GetTagTree(Tree);
  };

//--------------------------------------------------------------------------

int CFlwSolver::FE_AD_GetStatus(Strng & Status)
  {
  if (m_ADIndexGrp<0)
    {
    Status="Invalid Design Object";
    return 0;
    }

  CADGroup  &ADG=ADGrps[m_ADIndexGrp];
  CADObject &ADO=*ADG.Obj[m_ADIndexObj];
  return ADO.GetStatus(Status);
//  Status.Set("%i,%i : %s : %s", m_ADIndexGrp, m_ADIndexObj, ADG.pClass->ClassId(), ADO.Tag());
//  return 1;
  };

//--------------------------------------------------------------------------

int CFlwSolver::FE_AD_Go()
  {
  if (m_ADIndexGrp<0)
    return 0;
  CADGroup  &ADG=ADGrps[m_ADIndexGrp];
  CADObject &ADO=*ADG.Obj[m_ADIndexObj];

  return 0;
  };

//--------------------------------------------------------------------------

int CFlwSolver::FE_IOPnt_Add(LPCTSTR Tag, bool Input, LPCTSTR Desc, byte Type, LPCTSTR ValueStr, LPCTSTR ValueCnvs)
  {
  return IOObj.AddTag(Tag, Input, Desc, Type, ValueStr, ValueCnvs);
  };

//--------------------------------------------------------------------------

int CFlwSolver::FE_IOPnt_Remove(LPCTSTR Tag)
  {
  return IOObj.RemoveTag(Tag);
  };

//--------------------------------------------------------------------------

int CFlwSolver::FE_GetEvalOrder(bool GetCtrl, bool GetFullDescI, bool GetFullDescO, eTraceValue TraceWhat, CEvalOrderArray &Info)
  {
  CNodeEvalIndexList &ProcOrder =(NetProbalMethod() ? PB.ProcTOSequenceDsp : Dyn.ProcTOSequenceDsp);
  CNodeEvalIndexList &CtrlOrder =(NetProbalMethod() ? PB.CtrlTOSequenceDsp : Dyn.CtrlTOSequenceDsp);

  int NP=ProcOrder.GetCount();
  int NC=CtrlOrder.GetCount();
  Info.SetSize(GetCtrl ? NC : NP);
  int j=0;
  CNodeEvalIndexList &Order =(GetCtrl ? CtrlOrder: ProcOrder);

  CNodeEvalIndexIter It(Order);
  for (CNodeEvalIndex NJ=It.First(); It.ItemValid(); NJ=It.Next())
    {
    FlwNode *p=NJ.m_pNd;
    FlwNode &Nd=NJ.Nd;
    CJoinCluster *pC=NJ.m_iCluster>=0 ? &NJ.Cluster : NULL;

    if (pC && Nd.NClusters()>1)
      Info[j].m_sTag.Set("%s<%i>", Nd.FullObjTag(), pC->m_iCluster);
    else
      Info[j].m_sTag=Nd.FullObjTag();
    Info[j].m_lCluster=pC ? pC->m_iCluster:-1;
    Info[j].m_lProcOrd=pC ? pC->m_iProcOrder:-1;
    Info[j].m_lCtrlOrd=Nd.m_iCtrlOrder;

    if (pC)
      {
      for (int io=0, i; (i=pC->IONo(io))>=0; io++) {}
      Info[j].m_FIOs.SetSize(io);
      }
    else
      Info[j].m_FIOs.SetSize(Nd.NoFlwIOs());
    for (int io=0, i; ; io++)
      {
      if (pC)
        i=pC->IONo(io);
      else
        i=io<Nd.NoFlwIOs() ? io:-1;
      if (i<0)
        break;
      CEvalOrderIOItem &IO=Info[j].m_FIOs[io];
      IO.m_bIn=Nd.IOEvalSeq_In(i)!=0;
      IO.m_bOut=Nd.IOEvalSeq_Out(i)!=0;
      IO.m_bTear=Nd.IOFlange(i)->IsTear()!=0;
      IO.m_bOwner=false;
      IO.m_sRmtTag=Nd.Nd_Rmt(i)->FullObjTag();
      IO.m_lRmtProcOrd=Nd.IO_Cluster_Rmt(i) ? Nd.IO_Cluster_Rmt(i)->m_iProcOrder : -1;
      IO.m_lRmtCtrlOrd=Nd.Nd_Rmt(i)->m_iCtrlOrder;
      if (IO.m_sRmtTag.GetLength()<1)
        IO.m_sRmtTag="?";
      IO.m_sRmtDesc=IO.m_sRmtTag;
      IO.m_Trc=Nd.IOChanges(i);
      IO.m_dwType=EOC_Proc;
      }

    Info[j].m_CIOs.SetSize(Nd.NoCtrls());
    for (i=Nd.Ctrl1(); i<Nd.CtrlN(); i++)
      {
      CEvalOrderIOItem &IO=Info[j].m_CIOs[i-Nd.Ctrl1()];
      IO.m_bIn=Nd.Ctrl_In(i)!=0;
      IO.m_bOut=Nd.Ctrl_Out(i)!=0;
      IO.m_bTear=Nd.Ctrl_Terminal(i)->IsTear()!=0;
      IO.m_bOwner=false;
      IO.m_sRmtTag=Nd.Nd_Rmt(i)->FullObjTag();
      IO.m_lRmtProcOrd=Nd.IO_Cluster_Rmt(i) ? Nd.IO_Cluster_Rmt(i)->m_iProcOrder : -1;
      IO.m_lRmtCtrlOrd=Nd.Nd_Rmt(i)->m_iCtrlOrder;
      if (IO.m_sRmtTag.GetLength()<1)
        IO.m_sRmtTag="?";
      IO.m_sRmtDesc=IO.m_sRmtTag;

      IO.m_Trc=Nd.IOChanges(i);
      if (Nd.IOType_Self(i)&nc_CLnk)
        IO.m_dwType=EOC_Ctrl;
      else if (Nd.IOType_Self(i)&nc_ELnk)
        IO.m_dwType=EOC_Elec;
      else if (Nd.IOType_Self(i)&nc_ALnk)
        IO.m_dwType=EOC_Air;
      else
        IO.m_dwType=EOC_Null;
      }

    Info[j].m_XIOs.SetSize(Nd.NoXRefs2Me());
    for (i=0; i<Nd.NoXRefs2Me(); i++)
      {
      CEvalOrderIOItem &IO=Info[j].m_XIOs[i];
      CXRefItem &XR=*(Nd.m_XRefs2Me[i]);
      FlwNode * pOther=static_cast<FlwNode*>(XR.DstNd()==p ?XR.SrcNd():XR.DstNd());
      IO.m_bIn=XR.DstNd()==p;
      IO.m_bOut=XR.SrcNd()==p;
      IO.m_bTear=XR.IsTear()!=0;
      IO.m_bOwner=XR.Owner()==p;

      if (IO.m_bIn)
        {
        IO.m_sRmtTag=pOther ? pOther->FullObjTag() : "?";
        IO.m_sRmtDesc=GetFullDescI ? XR.SFGNodeTag(p) : (pOther ? pOther->FullObjTag() : "...?...");
        }
      else
        {
        IO.m_sRmtTag=pOther ? pOther->FullObjTag() : "?";
        IO.m_sRmtDesc=GetFullDescO ? XR.SFGNodeTag(p) : (pOther ? pOther->FullObjTag() : "...?...");
        }
      //IO.m_lRmtProcOrd=(pOther ? pOther->m_iProcOrder : iNotInEvalOrder);
      
      if (0)
        dbgpln("IO.m_lRmtProcOrd=(pOther ? pOther->m_iProcOrder : iNotInEvalOrder");
      
      IO.m_lRmtCtrlOrd=(pOther ? pOther->m_iCtrlOrder : iNotInEvalOrder);
      if (IO.m_sRmtTag.GetLength()<1)
        IO.m_sRmtTag="?";
      if (IO.m_sRmtDesc.GetLength()<1)
        IO.m_sRmtDesc="?";

      IO.m_Trc.SetSize(1);
      IO.m_Trc[0]=XR.m_Value.m_Chg;
      IO.m_dwType=EOC_XRef;

      //XferTrace(TraceWhat, IO, 0, XR.m_Chg);

      if (0)
        {
        dbgpln("%4i %30s %30s %3i %30s %3i %30s  %4i %4i",
              j,
              Nd.FullObjTag(),
              IO.m_sRmtDesc(),
              IO.m_bIn, static_cast<FlwNode*>(XR.DstNd())->FullObjTag(),
              IO.m_bOut, static_cast<FlwNode*>(XR.SrcNd())->FullObjTag(),
              IO.m_lRmtProcOrd, IO.m_lRmtCtrlOrd);
        };

      }
    j++;
    }
  return Info.GetSize();
  }

//--------------------------------------------------------------------------

int CFlwSolver::FE_GetEvalStats(CEvalStatsArray &Info)
  {
  CNodeEvalIndexList &ProcOrder =(NetProbalMethod() ? PB.ProcTOSequenceDsp : Dyn.ProcTOSequenceDsp);
  CNodeEvalIndexList &CtrlOrder =(NetProbalMethod() ? PB.CtrlTOSequenceDsp : Dyn.CtrlTOSequenceDsp);

  int NP=ProcOrder.GetCount();
  int NC=CtrlOrder.GetCount();

  Info.SetSize(NC+NP);
  long j=0;

  CNodeEvalIndexIter PIt(ProcOrder);
  for (CNodeEvalIndex NJ=PIt.First(); PIt.ItemValid(); NJ=PIt.Next())
    {
    FlwNode * p=NJ.m_pNd;
    Info[j].m_sTag=p->FullObjTag();
    Info[j].m_sClass=p->ClassId();
    Info[j].m_Items.SetSize(FSW_Max);
    #if (NDSTOPWATCHES)
    if (p->pSWs)
      {
      for (int i=0; i<FSW_Max; i++)
        Info[j].m_Items[i]=p->SW(i);
      }
    #endif
    j++;
    }
  CNodeEvalIndexIter CIt(CtrlOrder);
  for (CNodeEvalIndex NJ=CIt.First(); CIt.ItemValid(); NJ=CIt.Next())
    {
    FlwNode * p=NJ.m_pNd;
    Info[j].m_sTag=p->FullObjTag();
    Info[j].m_sClass=p->ClassId();
    Info[j].m_Items.SetSize(FSW_Max);
    #if (NDSTOPWATCHES)
    if (p->pSWs)
      {
      for (int i=0; i<FSW_Max; i++)
        Info[j].m_Items[i]=p->SW(i);
      }
    #endif
    j++;
    }

  return Info.GetSize();
  }

//--------------------------------------------------------------------------

int CFlwSolver::FE_GetNodeList(CNodeList&List)
  {
  CNodeListItem I;
  FlwNode *p=NULL;
  long SeqNo=0;
  while (GetObj(p))
    {
    I.m_sTag        = p->Tag();
    I.m_sClass      = p->ClassId();
    I.m_sSubClass   = p->SubClassId();
    I.m_sPrimaryCfg = p->GetPrimaryCfg();
    I.m_sEqpDesc    = p->EqpDesc();
    I.m_sEqpMemo    = p->EqpMemo();
    I.m_sEqpIdStr   = p->EqpIdStr();
    I.m_sEqpLocation = p->EqpLocation();
    I.m_sPlantArea  = p->GetAreaTagDisp(); 
    I.m_sEqpGUID    = p->EqpGUID();
    I.m_lSeqNo      = SeqNo++;
    I.m_bIsLnk      = /*(p->NoProcessIOs()==2) &&*/ (p->IsMLnk() || p->IsCLnk() || p->IsELnk() || p->IsALnk());
    if (I.m_bIsLnk)
      {
      I.m_sSrcNd    = p->Nd_Rmt(0)->Tag();
      I.m_sDstNd    = p->Nd_Rmt(1)->Tag();
      I.m_sSrcIO    = p->IODesc_Rmt(0)->IOName();
      I.m_sDstIO    = p->IODesc_Rmt(1)->IOName();
      I.m_lSrcIOInx = p->IODescNo_Rmt(0);
      I.m_lDstIOInx = p->IODescNo_Rmt(1);
      }
    List.AddTail(I);
    }
  return SeqNo;
  };

//--------------------------------------------------------------------------

int CFlwSolver::FE_GetNodeConfiguration(BOOL AllParms, BOOL AllState, LPCTSTR NdTag, CNodeConfigList&List)
  {
  List.RemoveAll();
  CTNode * p=FindObj((LPTSTR)NdTag);
  if (p==NULL)
    return -1;

  UnDoGlobalLinks();

  dbgpln("CFlwSolver::FE_GetNodeConfiguration %s", NdTag);

  CXM_ObjectData ObjData(0);
  flag Ok = false;

  TagAccessBlk TAB(this, (LPTSTR)NdTag, TABOpt_AllInfo|TABOpt_ForFile/* TABOpt_ForSnapShot*//*|TABOpt_XRefStatus*//*|RunModes()*/, TU_IndividuallyUnique);
  if (TAB.LoadAddress(true))
    {
    ObjData.List.Clear();
    Ok=TAB.GetDefinedData(ObjData.List, 0);
    }


  if (Ok)
    {
    int iNo=0;

    CPkDataIter Iter;
    int ReportedMaxFieldsError = 0;
    flag FirstPage=1;
    for (CPkDataItem* pPItem=ObjData.FirstItem(Iter); pPItem; )
      {
      int ItemAdvanced=false;
      DDEF_Flags Flags=pPItem->Flags();
      byte Type=pPItem->Type();
      flag Vis=1;
      //if (IsData(pPItem->Type()) || (pPItem->Type()==tt_Text))
      if (!IsLayout(Type))
        {
        Vis=((Flags & MODEVISIBLE)!=0 && (Flags & DDEF_NOFILE)==0);
        bool Rqd=(AllParms && (Flags & DDEF_PARAM)!=0) || (AllState && (Flags & DDEF_RESULT)==0);

        if ((Vis && Rqd) || IsStructure(Type))
          {
          CNodeConfigItem I;
          I.m_sNdTag=NdTag;
          I.m_ttType=Type;
          I.m_dwChgSeqNo=-1;
          I.m_sFldTag=pPItem->SymOrTag();
          I.m_sCnvStr=pPItem->CnvTxt();
          I.m_sValue=pPItem->Value()->GetString("%i", "%.30g");
          List.AddTail(I);

          if (0)
            dbgpln("Cfg Field :%6i %08x %-7s %-30s %= %s", iNo, List.GetTailPosition(), tt_TypeString(Type), pPItem->SymOrTag(), "?");

          iNo++;
          }
        }

      pPItem=ObjData.NextItem(Iter);
      }

    POSITION Pos=List.GetHeadPosition();
    POSITION IPos=NULL;
    while (Pos) 
      {
      POSITION SPos=Pos;
      CNodeConfigItem &Is=List.GetNext(Pos);
      if (IsStructStart(Is.m_ttType))
        {
        POSITION EPos=Pos;
        CNodeConfigItem &Ie=List.GetNext(Pos);
        if (IsStructEnd(Ie.m_ttType))
          {
          //remove both;
          List.RemoveAt(SPos);
          List.RemoveAt(EPos);
          // Go Back One
          POSITION PosC=Pos;
          List.GetPrev(Pos);
          if (Pos==NULL)
            Pos=PosC;
          if (0)
            dbgpln("Remove: %08x %08x %08x ", SPos, EPos, Pos);
          }
        }
      }
    }


//#if WITHTAGMONITOR
//  CFieldMonitorTO &TOMon = p->Monitor();
//  CFieldMonitorTOCls &ClsMon = p->ClassMonitor();
//
//  POSITION Pos=ClsMon.m_List.GetHeadPosition();
//  while (Pos)
//    {
//    CFieldMonitor & FldMon=ClsMon.m_List.GetNext(Pos);
//    if (FldMon.m_iIndex<TOMon.m_CurrentValue.GetSize())
//      {
//      DataUnion * CurrentV=TOMon.m_CurrentValue[FldMon.m_iIndex];
//      if (CurrentV && !CurrentV->Equal(FldMon.m_InitialValue))
//        {
//        CNodeConfigItem I;
//        I.m_sNdTag=p->FullObjTag();
//        I.m_dwChgSeqNo=FldMon.m_dwChgSeqNo;
//        //if (FldMon.m_sFldCnv())
//        //  I.m_sFldTag.Format("%s (%s)", FldMon.m_sFldTag(), FldMon.m_sFldCnv());
//        //else
//        I.m_sFldTag=FldMon.m_sFldTag();
//        I.m_sCnvStr=FldMon.m_sFldCnv();
//        //I.m_bIsDbl=IsFloatData(CurrentV->iType)!=0;
//        //if (I.m_bIsDbl)
//        //  I.m_dValue=CurrentV->GetDouble();
//        //else
//        I.m_sValue=CurrentV->GetString("%i", "%.30g");
//        POSITION Pos=List.GetHeadPosition();
//        POSITION IPos=NULL;
//        while (Pos) // In Change Order
//          {
//          IPos=Pos;
//          CNodeConfigItem &II=List.GetNext(Pos);
//          if (II.m_dwChgSeqNo>I.m_dwChgSeqNo)
//            break;
//          }
//        if (IPos && Pos)
//          List.InsertBefore(IPos, I);
//        else
//          List.AddTail(I);
//
//        //List.AddTail(I);
//        //dbgpln("FldMon Change    :%-40s = %s", FldMon.m_sFldTag.Buffer(), I.m_sValue);
//        }
//      else
//        {
//        //dbgpln("FldMon No Change :%-20s", FldMon.m_sFldTag);
//        }
//      }
//    }
//#endif

  const bool WithNdCfgDbg=false;
  if (WithNdCfgDbg)
    dbgpln("NdCfg : %-30s --------------------------------------", NdTag);
  POSITION Pos=List.GetHeadPosition();
  for (int i=0; Pos; i++)
    {
    POSITION XPos=Pos;
    CNodeConfigItem &I=List.GetNext(Pos);
    I.m_dwChgSeqNo=i;
    if (WithNdCfgDbg)
      dbgpln("NdCfg :%6i %-7s %-40s = %s", I.m_dwChgSeqNo, tt_TypeString(I.m_ttType), I.m_sFldTag, I.m_sValue);
      //dbgpln("NdCfg :%6i %08x %7s %-40s = %s", I.m_dwChgSeqNo, XPos, tt_TypeString(I.m_ttType), I.m_sFldTag, I.m_sValue);
    }

  return List.GetCount();
  };

//--------------------------------------------------------------------------


int CFlwSolver::FE_GetNodeBalanceInfo(CNodeBalanceInfo & Balance, CNodeAuditInfo & Audit)
  {
  FlwNode * p=dynamic_cast<FlwNode*>(FindObj((LPTSTR)(LPCTSTR)Balance.m_NodeTag));
  if (p==NULL)
    return -1;
  //LPTSTR StdName="Standard";

  double  Qm[2][MaxIOList];
  int     Inx[2][MaxIOList];
  int     N[2]={0,0};
  int     nJoins=0;
  for (int i=0; i<p->NoFlwIOs(); i++)
    {
    int iDir=(p->IOQm_In(i)>0)?0:1;
    Qm[iDir][N[iDir]]=p->IOQm_In(i);
    Inx[iDir][N[iDir]]=i;
    N[iDir]++;
    nJoins=Max(nJoins, p->IO_Join_Id(i)+1);
    }
  Balance.SetJoinCount(nJoins);

  ////Init RefT
  //for (int i=0; i<nJoins; i++)
  //  Balance.m_Joins[i].m_RefT=dNAN;

  // HpSort ...????????
  for (int iDir=0; iDir<2; iDir++)
    {
    int i=1;
    while (i<N[iDir])
      {
      if (Qm[iDir][i-1]<Qm[iDir][i])
        {
        Exchange(Qm[iDir][i-1], Qm[iDir][i]);
        Exchange(Inx[iDir][i-1], Inx[iDir][i]);
        if (i>1)
          i--;
        }
      else
        i++;
      }
    CNodeBalanceIOArray &A=(iDir==0)?Balance.m_Feeds:Balance.m_Prods;
    int Sgn=(iDir==0)?1:-1;
    A.SetSize(N[iDir]);
    for (int i=0; i<N[iDir]; i++)
      {
      int io=Inx[iDir][i];
      CNodeBalanceIO &I=A[i];

      I.m_Tag         = p->Nd_Rmt(io)->FullObjTag();
      I.m_IOPort      = p->IODesc_Self(io)->IOName();
      I.m_SpModelName = p->IOConduit(io)->Model()->Class()->ShortDesc();
      I.m_iJoinId     = p->IO_Join_Id(io);
      I.m_iSgn        = Sgn;
      I.m_Qm          = p->IOQm_In(io)*Sgn;
      I.m_T           = p->IOConduit(io)->Temp();
      I.m_P           = p->IOConduit(io)->Press();
      I.m_Rho         = p->IOConduit(io)->Rho();
      I.m_Sf          = p->IOConduit(io)->MassFrac(som_Sol);
      I.m_Lf          = p->IOConduit(io)->MassFrac(som_Liq);
      I.m_Vf          = p->IOConduit(io)->MassFrac(som_Gas);

      }
    }

  for (int i=0; i<nJoins; i++)
    {
    CNodeBalanceJoin &J=Balance.m_Joins[i];
    J.m_JoinMassFlwIn       = p->GetBalanceValue( eBV_JoinMassFlwIn, i);
    J.m_JoinMassGain        = p->GetBalanceValue( eBV_JoinMassGain, i);
    J.m_JoinMassFlwOut      = p->GetBalanceValue( eBV_JoinMassFlwOut, i);
    J.m_JoinFeedCpT         = p->GetBalanceValue( eBV_JoinFeedCpT, i);
    J.m_JoinProdCpT         = p->GetBalanceValue( eBV_JoinProdCpT, i);
    J.m_JoinHfIn            = p->GetBalanceValue( eBV_JoinHfIn, i);
    J.m_JoinHfGain          = p->GetBalanceValue( eBV_JoinHfGain, i);
    J.m_JoinHsGain          = p->GetBalanceValue( eBV_JoinHsGain, i);
    J.m_JoinHfOut           = p->GetBalanceValue( eBV_JoinHfOut, i);
    J.m_JoinHfChg           = p->GetBalanceValue( eBV_JoinHfChg, i);
    J.m_JoinHsIn            = p->GetBalanceValue( eBV_JoinHsIn, i);
    J.m_JoinHsOut           = p->GetBalanceValue( eBV_JoinHsOut, i);
    J.m_JoinHsChg           = p->GetBalanceValue( eBV_JoinHsChg, i);
    J.m_JoinPowerIn         = p->GetBalanceValue( eBV_JoinPowerIn, i);

    J.m_RCTMassGain         = p->GetBalanceValue( eBV_RCTMassGain, i);
    J.m_RCTHfGain           = p->GetBalanceValue( eBV_RCTHfGain, i);
    J.m_RCTHsGain           = p->GetBalanceValue( eBV_RCTHsGain, i);
    J.m_RCTHOR_Std0         = p->GetBalanceValue( eBV_RCTHOR_Std0, i);
    J.m_RCTHOR_Mdl0         = p->GetBalanceValue( eBV_RCTHOR_Mdl0, i);
    J.m_RCTHOR_Used0        = p->GetBalanceValue( eBV_RCTHOR_Used0, i);
    J.m_RCTHOR_Diff0        = p->GetBalanceValue( eBV_RCTHOR_Diff0, i);
    J.m_RCTPowerIn          = p->GetBalanceValue( eBV_RCTPowerIn, i);
    J.m_VLEHsGain           = p->GetBalanceValue( eBV_VLEHsGain, i);
    J.m_EHXPowerIn          = p->GetBalanceValue( eBV_EHXPowerIn, i);
    }

  // HpSort ...????????
  for (int iDir=0; iDir<2; iDir++)
    {
    CNodeBalanceIOArray &A=(iDir==0)?Balance.m_Feeds:Balance.m_Prods;
    int Sgn=(iDir==0)?1:-1;
    A.SetSize(N[iDir]);
    for (int i=0; i<N[iDir]; i++)
      {
      int io=Inx[iDir][i];
      CNodeBalanceIO &I=A[i];
      CNodeBalanceJoin &J=Balance.m_Joins[I.m_iJoinId];
      CSysVector * pM  = p->IOConduit(io)->pMArray();
      //double T = p->IOConduit(io)->Temp();
      //double P = p->IOConduit(io)->Press();

      I.m_msCp0     = /*QStd().*/p->IOConduit(io)->msCp(som_ALL,  C2K(0.0), I.m_P, pM);
      I.m_msCpT     = /*QStd().*/p->IOConduit(io)->msCp(som_ALL,  I.m_T, I.m_P, pM);
      I.m_msHsT     = /*QStd().*/p->IOConduit(io)->msHs(som_ALL,  I.m_T, I.m_P, pM);
      I.m_msHzT     = /*QStd().*/p->IOConduit(io)->msHz(som_ALL,  I.m_T, I.m_P, pM);
      I.m_msHfT     = /*QStd().*/p->IOConduit(io)->msHf(som_ALL,  I.m_T, I.m_P, pM);
      I.m_totHsT    = /*QStd().*/p->IOConduit(io)->totHs(som_ALL, I.m_T, I.m_P, pM);
      I.m_totHzT    = /*QStd().*/p->IOConduit(io)->totHz(som_ALL, I.m_T, I.m_P, pM);
      I.m_totHfT    = /*QStd().*/p->IOConduit(io)->totHf(som_ALL, I.m_T, I.m_P, pM);
      }
    }

  if (1)      // Audit
    {
    FlwNode * p=dynamic_cast<FlwNode*>(FindObj((LPTSTR)(LPCTSTR)Audit.m_NodeTag));
    if (p==NULL)
      return -1;

    p->GetBalanceReport(Audit);
    }
  return 1;
  };

//--------------------------------------------------------------------------

CTagRefStatus CFlwSolver::GetTagRefStatus(LPCTSTR Tag)
  {
  CTagRefStatus W=FFM_Off;
  if (Tag)
    {
    Strng S(Tag);
    S.Lower();
    CTagRefsMapItem* pMapItem;
    if (m_TagRefsMap.Lookup((LPCTSTR)S(), pMapItem))
      {
      W=pMapItem->GetStatusWord();
      #if dbgFlwSolve
      if (dbgXRefStatus())
        dbgpln("GetTagRefStatus: %04x %s", W, Tag);
      #endif
      return W;
      }
    }

  #if dbgFlwSolve
  if (dbgXRefStatus())
    dbgpln("GetTagRefStatus:    - %s", Tag);
  #endif
  return W;
  };

//--------------------------------------------------------------------------

flag CFlwSolver::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    //case 1: pS="W\tSpillAreas Missing";  return 1;
    //case 2: pS="N\tIntegration too slow for Realtime Project"; return 1;
    case 3: pS="W\tMass Smoothing Problem"; return 1;
    case 4: pS="W\tKFact Multiplier not Unity"; return 1;
    default:
      return FlwNode::CIStrng(No, pS);
    }
  };

//==========================================================================
//
//
//
//==========================================================================

IMPLEMENT_TAGOBJ(CADObject, "ADObjBase", "ADObject", "", "", "FG", TOC_SYSTEM, "FlowNodeGrp", "Long Descriptor")
IMPLEMENT_SPARES(CADObject, 10);

CADObject::CADObject(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  TaggedObject(pClass_, TagIn, pAttach, eAttach)
  {
  pFS=NULL;
  pSrcFS=NULL;
  };

//--------------------------------------------------------------------------

CADObject::~CADObject()
  {
  };

//--------------------------------------------------------------------------

int CADObject::SetSelectedTags(Strng_List &SelectedTags)
  {
  m_SelectedTags.SetSize(SelectedTags.Length());
  Strng *p=SelectedTags.First();
  for (int i=0; i<m_SelectedTags.GetSize(); i++)
    {
    m_SelectedTags[i]=p->Str();
    p=SelectedTags.Next();
    }
  pSrcFS->DuplicateNetwork(m_SelectedTags, pFS);
  return 1;
  };

//--------------------------------------------------------------------------

int CADObject::GetTagTree(CFlwTree & Tree)
  {
//  m_SelectedTags.SetSize(SelectedTags.Length());
//  Strng *p=SelectedTags.First();
//  for (int i=0; i<m_SelectedTags.GetSize(); i++)
//    {
//    m_SelectedTags[i]=p->Str();
//    p=SelectedTags.Next();
//    }
  pFS->AnalyseNetwork(Tree);
  return 1;
  };

//--------------------------------------------------------------------------

int CADObject::GetStatus(Strng & Status)
  {
  Status.Set("%s : %s", Class()->ClassId(), Tag());
  return 1;
  };

//===========================================================================
//
//
//
//===========================================================================
