//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992
#include "stdafx.h"

#include "sc_defs.h"
//#include "datacnvs.h"

#define  __FSTHREAD_CPP
#include "flwsolve.h"
#include "flwnets.h"
#include "fsthread.h"

#include "dbgmngr.h"
#include "scd_wm.h"

//#include "optoff.h"

//===========================================================================

#define dbgFlwSolve       WITHDEBUG

//===========================================================================

#if (dbgFlwSolve)
static CDbgMngr dbgThreads            ("FlwSolve", "Threads");
static CDbgMngr dbgThreadsJob         ("FlwSolve", "Threads-Job");
static CDbgMngr dbgThreadsSleep       ("FlwSolve", "Threads-Sleep");
static CDbgMngr dbgThreadsSeq         ("FlwSolve", "Threads-Seq");
static CDbgMngr dbgThreadsTiming      ("FlwSolve", "Threads-Timing");
#endif

#if (dbgFlwSolveTiming)
static CDbgMngr dbgDumpThreadOverHead ("FlwSolve", "Threads-DumpOverHead");
CStopWatch gs_ExecIterTime;
#endif

const char* eFITCmdStr(int i)
  {
  switch (i)
    {
    case eFITNull:                  return "Null";
    case eFITQuit:                  return "Quit";
    case eFITEvalDiscrete:          return "Discrete";
    case eFITEvalBalance:           return "Balance";
    case eFITCheckXRefs:            return "CheckXRefs";
    case eFITEvalCtrlInitialise:    return "EvalCtrlInitialise";
    case eFITEvalCtrlActions:       return "EvalCtrlActions";
    case eFITEvalCtrlStrategy:      return "EvalCtrlStrategy";
    case eFITEvalCtrlTerminate:     return "EvalCtrlTerminate";
    case eFITEvalStatistics:        return "EvalStatistics";
    case eFITEvalTerminate:         return "EvalTerminate";
    case eFITConfigureJoins:        return "ConfigureJoins";
    case eFITElectrics:             return "PowerAvailable";
    case eFITPowerRequired:         return "PowerRequired";
    case eFITStates:                return "States";
    case eFITEvalSteadyState:       return "EvalSteadyState";
    case eFITStartStep:             return "StartStep";
    case eFITFlowInfo:              return "FlowInfo";
    case eFITEvalProducts:          return "EvalProducts";
    case eFITEvalProductsSurge:     return "EvalProductsSurge";
    case eFITEvalDerivs:            return "EvalDerivs";
    case eFITEvalIntegral:          return "EvalIntegral";
    case eFITODEOperate:            return "ODEOperate";
   
    case eFITGrpMeasure:            return "GrpMeasure";
    case eFITGrpTraverse:           return "GrpTraverse";
    case eFITGrpConverge:           return "GrpConverge";

    case eFITPBEvalMakeUpAvail:     return "PBEvalMakeupAvail";
    case eFITPBEvalMakeUpReqd :     return "PBEvalMakeupReqd";
    case eFITPBEvalPressures:       return "PBEvalPressures";
    case eFITPBEvalProducts:        return "PBEvalProducts";
    case eFITPBEvalDiscrete:        return "PBEvalDiscrete";
    case eFITPBEvalCtrlInitialise:  return "PBEvalCtrlInitialise";
    case eFITPBEvalCtrlActions:     return "PBEvalCtrlActions";
    case eFITPBEvalCtrlStrategy:    return "PBEvalCtrlStrategy";
    case eFITPBEvalCtrlTerminate:   return "PBEvalCtrlTerminate";
    default:                        return "??";
    }
  };

const char* FST_CmdStr(int i)
  {
  switch (i)
    {
    case FST_FlwSolve:        return "FlwSolve";
    default:                  return "??";
    }
  };

//===========================================================================
//
//
//
//===========================================================================

class ThreadDataBlk 
  {
  public:

    ThreadDataBlk(UINT Cmd, LPVOID This, LPVOID Data, LPVOID MoreData=NULL);
    ~ThreadDataBlk();
    void StartJob(CFlwSlvThreads & FST, int NJobs=-1);
    void SignalJobDone();
    void WaitJobDone();

    int                 iCmd;
    int                 nJobs;
    HANDLE              hThreadsDone;
    union 
      {
      LPVOID            pThis;
      FlwSlvDyn*        pDyn;
      CSubNetBlk*       pSNB;
      CFullNetBlk*      pFNB;
      FlwSlvPB*         pPB;
      };
    union
      {
      LPVOID            pData;
      CLkFlwNodeIter*   pFNI;
      CLkGrpFlwIter*    pGFI;
      CLkNodeEvalIter*  pNEI;
      
      CFlwNodeListArray * pFNLA;
      };
    union
      {
      LPVOID            m_pMoreData;
      CODEDataBlock *   m_pODE;
      };
  };


ThreadDataBlk::ThreadDataBlk(UINT Cmd, LPVOID This, LPVOID Data, LPVOID MoreData)
  {
  iCmd=Cmd;
  nJobs=0;
  pThis=This;
  pData=Data;
  m_pMoreData=MoreData;
  hThreadsDone=CreateSemaphore((LPSECURITY_ATTRIBUTES)NULL, 0, 0x7fffffff, NULL);
  }

ThreadDataBlk::~ThreadDataBlk()
  {
  if (hThreadsDone)
    CloseHandle(hThreadsDone);
  hThreadsDone=NULL;
  }

void ThreadDataBlk::StartJob(CFlwSlvThreads & FST, int NJobs)
  {
  nJobs=NJobs;
  if (nJobs<=0)
    nJobs=FST.NThreads();
  for (int ip=0; ip<nJobs; ip++)
    {
    #if dbgFlwSolve
    if (dbgThreadsSeq())
      dbgpln("%*sPost %4i >> %4i %-20.20s ", 
        FST.dbgIndent(GetCurrentThreadId()), "", GetCurrentThreadId(), FST.ThreadId(ip), 
           iCmd>=FST_Base ? FST_CmdStr(iCmd) : eFITCmdStr(iCmd));
    #endif
    FST.Thread(ip).PostThreadMessage(WMU_FLWSOLVECMD, MAKEWPARAM(iCmd,ip), (long)(LPVOID)this);
    }
  }

//---------------------------------------------------------------------------

void ThreadDataBlk::SignalJobDone()
  {
  ReleaseSemaphore(hThreadsDone, 1, NULL);
  #if dbgFlwSolve
  if (dbgThreadsSeq())
    dbgpln("%*sSignal %4i", CFlwSlvThreads::dbgIndent(GetCurrentThreadId()), "", GetCurrentThreadId());
  #endif
  }

//---------------------------------------------------------------------------

void ThreadDataBlk::WaitJobDone()
  {
  for (int ip=0; ip<nJobs; ip++)
    {
    WaitForSingleObject(hThreadsDone, INFINITE);
    #if dbgFlwSolve
    if (dbgThreadsSeq())
      dbgpln("%*sDone %2i @ %4i", CFlwSlvThreads::dbgIndent(GetCurrentThreadId()), "", ip, GetCurrentThreadId());
    #endif
    }
  #if dbgFlwSolve
  if (dbgThreadsSeq())
    dbgpln("Done - ALL @ %4i", GetCurrentThreadId());
  #endif
  }

//===========================================================================
//
//                           Dynamic Solve Threads
//
//===========================================================================
        
#if dbgFlwSolveTiming 
#define DUMPOVERHEAD                                              \
if (dbgThreadsTiming())\
{                                                                 \
  SWExec.Stop();                                                  \
  dbgpln("   [%4i]%-20.20s Time:%10.3fms Idle:%10.3fms Exec:%10.3fms %5.1f%% ------", \
  GetCurrentThreadId(), eFITCmdStr(TDB->iCmd),                    \
  1e3*gs_ExecIterTime.Secs(),                                     \
  1e3*SWIdle.Secs(),                                              \
  1e3*SWExec.Secs(),                                              \
  100*SWExec.Secs()/GTZ(SWExec.Secs()+SWIdle.Secs()));            \
  SWIdle.Clear();                                                 \
  SWIdle.Start();                                                 \
}
#else
#define DUMPOVERHEAD                                              
#endif


UINT FlwIterThread(LPVOID pFST_)
  {
  CFlwSlvThreads * pFST=(CFlwSlvThreads*)pFST_;

  SCDTRY

    #if (dbgFlwSolve)
    if (dbgThreads())
      dbgpln("%*sNdI %4i FlwSlvDyn Entry", pFST->dbgIndent(GetCurrentThreadId()), "", GetCurrentThreadId());
    #endif
    #if (dbgFlwSolveTiming)
    CStopWatch SWIdle;
    CStopWatch SWExec;
    SWIdle.Start();
    #endif
    MSG Msg;
    flag Done=false;
    while (!Done && GetMessage(&Msg, NULL, 0,0))
      {
      long ThreadIndex=HIWORD(Msg.wParam);
      if (Msg.message!=WMU_FLWSOLVECMD)
        dbgpln("%sNdI %4i BAD MESSAGE %x\n", pFST->dbgIndent(GetCurrentThreadId()), "", GetCurrentThreadId(),Msg.message);
      else
        {
        ThreadDataBlk * TDB=(ThreadDataBlk*)Msg.lParam;
        if (TDB==NULL)
          {
          Done=true;
          }
        else
          {
          #if dbgFlwSolve
          if (dbgThreads())
            dbgpln("%*sNdI %4i %s", pFST->dbgIndent(GetCurrentThreadId()), "", GetCurrentThreadId(), eFITCmdStr(TDB->iCmd));
          if (dbgThreadsSleep())
            Sleep(0);
          #endif
          #if (dbgFlwSolveTiming)
          SWIdle.Stop();
          SWExec.Clear();
          SWExec.Start();
          #endif
          switch (TDB->iCmd)
            {
            case eFITEvalDiscrete:
              TDB->pDyn->FITDiscrete((*TDB->pFNLA)[ThreadIndex]);//*TDB->pFNI);
              DUMPOVERHEAD;
              TDB->SignalJobDone();
              break;
            case eFITEvalBalance:
              TDB->pDyn->FITEvalBalance(*TDB->pNEI);
              DUMPOVERHEAD;
              TDB->SignalJobDone();
              break;
            case eFITEvalCtrlActions:
              TDB->pDyn->FITEvalCtrlActions((*TDB->pFNLA)[ThreadIndex]);//*TDB->pFNI);
              DUMPOVERHEAD;
              TDB->SignalJobDone();
              break;
            case eFITEvalCtrlStrategy:
              TDB->pDyn->FITEvalCtrlStrategy((*TDB->pFNLA)[ThreadIndex]);//*TDB->pFNI);
              DUMPOVERHEAD;
              TDB->SignalJobDone();
              break;
            case eFITEvalStatistics:
              TDB->pDyn->FITEvalStatistics(*TDB->pFNI);
              DUMPOVERHEAD;
              TDB->SignalJobDone();
              break;
            case eFITConfigureJoins:
              TDB->pDyn->FITConfigureJoins((*TDB->pFNLA)[ThreadIndex]);
              DUMPOVERHEAD;
              TDB->SignalJobDone();
              break;
            //case eFITPowers:
            //  TDB->pDyn->FITPowers(*TDB->pFNI);
            //  TDB->SignalJobDone();
            //  break;
            case eFITStates:
              TDB->pDyn->FITStates((*TDB->pFNLA)[ThreadIndex]);//*TDB->pFNI);
              DUMPOVERHEAD;
              TDB->SignalJobDone();
              break;
            case eFITStartStep:
              TDB->pDyn->FITStartStep((*TDB->pFNLA)[ThreadIndex]);//*TDB->pFNI);
              DUMPOVERHEAD;
              TDB->SignalJobDone();
              break;
            case eFITFlowInfo:
              TDB->pDyn->FITFlowInfo((*TDB->pFNLA)[ThreadIndex]);//*TDB->pFNI);
              DUMPOVERHEAD;
              TDB->SignalJobDone();
              break;
            case eFITEvalProductsSurge:
              TDB->pDyn->FITEvalProductsSurge(*TDB->pNEI);
              DUMPOVERHEAD;
              TDB->SignalJobDone();
              break;
            case eFITEvalProducts:
              TDB->pDyn->FITEvalProducts(*TDB->pNEI);
              DUMPOVERHEAD;
              TDB->SignalJobDone();
              break;
            case eFITEvalDerivs:
              TDB->pDyn->FITEvalDerivs(*TDB->pNEI);
              DUMPOVERHEAD;
              TDB->SignalJobDone();
              break;
            case eFITEvalIntegral:
              TDB->pDyn->FITEvalIntegral(*TDB->pNEI);
              DUMPOVERHEAD;
              TDB->SignalJobDone();
              break;
            case eFITODEOperate:
              TDB->pDyn->FITODEOperate((*TDB->pFNLA)[ThreadIndex], *TDB->m_pODE);
              DUMPOVERHEAD;
              TDB->SignalJobDone();
              break;

            case eFITGrpMeasure:
              TDB->pSNB->FITGrpMeasure(*TDB->pGFI);
              DUMPOVERHEAD;
              TDB->SignalJobDone();
              break;
            case eFITGrpTraverse:
              TDB->pSNB->FITGrpTraverse(*TDB->pGFI);
              DUMPOVERHEAD;
              TDB->SignalJobDone();
              break;
            case eFITGrpConverge:
              TDB->pSNB->FITGrpConverge(*TDB->pGFI);
              DUMPOVERHEAD;
              TDB->SignalJobDone();
              break;
 
            case eFITPBEvalMakeUpAvail:
              TDB->pPB->FITPBEvalMakeUpAvail(*TDB->pNEI);
              DUMPOVERHEAD;
              TDB->SignalJobDone();
              break;
            case eFITPBEvalMakeUpReqd:
              TDB->pPB->FITPBEvalMakeUpReqd(*TDB->pNEI);
              DUMPOVERHEAD;
              TDB->SignalJobDone();
              break;
            case eFITPBEvalPressures:
              TDB->pPB->FITPBEvalPressures(*TDB->pNEI);
              DUMPOVERHEAD;
              TDB->SignalJobDone();
              break;
            case eFITPBEvalProducts:
              TDB->pPB->FITPBEvalProducts(*TDB->pNEI);
              DUMPOVERHEAD;
              TDB->SignalJobDone();
              break;
            case eFITPBEvalDiscrete:
              TDB->pPB->FITPBEvalDiscrete((*TDB->pFNLA)[ThreadIndex]);//*TDB->pFNI);
              DUMPOVERHEAD;
              TDB->SignalJobDone();
              break;
            case eFITPBEvalCtrlActions:
              TDB->pPB->FITPBEvalCtrlActions((*TDB->pFNLA)[ThreadIndex]);//*TDB->pFNI);
              DUMPOVERHEAD;
              TDB->SignalJobDone();
              break;
            case eFITPBEvalCtrlStrategy:
              TDB->pPB->FITPBEvalCtrlStrategy((*TDB->pFNLA)[ThreadIndex]);//*TDB->pFNI);
              DUMPOVERHEAD;
              TDB->SignalJobDone();
              break;
          
            case FST_Quit :
              Done=true;
              break;

            default:
              dbgpln("%*sNdI %4i BAD Command %x\n", pFST->dbgIndent(GetCurrentThreadId()), "", GetCurrentThreadId(), TDB->iCmd);
            }
          #if dbgFlwSolve
          if (dbgThreads())
            dbgpln("%*sNdI %4i %s", pFST->dbgIndent(GetCurrentThreadId()), "", GetCurrentThreadId(), "Done");
          if (dbgThreadsSleep())
            Sleep(0);
          #endif
          }
        }
      }

    #if dbgFlwSolve
    if (dbgThreads())
      dbgpln("%*sNdI %4i FlwSlvDyn Exit", pFST->dbgIndent(GetCurrentThreadId()), "", GetCurrentThreadId());
    #endif
  
    
  SCDCATCH("Flw Iteration Thread")

  return 0;
  }

//===========================================================================

UINT FlwSlvThread(LPVOID pFST_)
  {
  CFlwSlvThreads * pFST=(CFlwSlvThreads*)pFST_;

  SCDTRY

    #if dbgFlwNets
    if (dbgThreads())
      dbgpln("%*sFsj %4i FullNetBlkThread Entry", pFST->dbgIndent(GetCurrentThreadId()), "", GetCurrentThreadId());
    #endif
    MSG Msg;
    flag Done=false;
    while (!Done && GetMessage(&Msg, NULL, 0,0))
      {
      long ThreadIndex=HIWORD(Msg.wParam);
      if (Msg.message!=WMU_FLWSOLVECMD)
        dbgpln("%*sFsj %4i BAD MESSAGE %x\n", pFST->dbgIndent(GetCurrentThreadId()), "", GetCurrentThreadId(),Msg.message);
      else
        {
        ThreadDataBlk * TDB=(ThreadDataBlk*)Msg.lParam;
        if (TDB==NULL)
          {
          Done=true;
          }
        else
          {
          #if dbgFlwNets
          if (dbgThreads())
            dbgpln("%*sFsj %4i %-20.20s ***********************************************************", ThreadIndex*ThreadSpacing, GetCurrentThreadId(), FST_CmdStr(TDB->iCmd));
          if (dbgThreadsSleep())
            Sleep(0);
          #endif
          switch (TDB->iCmd)
            {
            case FST_FlwSolve:
              TDB->pFNB->FSTFlwSolve();
              TDB->SignalJobDone();
              break;
            case FST_Quit :
              Done=true;
              break;

            default:
              dbgpln("Fsj %4i BAD Command %x\n", GetCurrentThreadId(),TDB->iCmd);
            }
          #if dbgFlwNets
          if (dbgThreads())
            dbgpln("%*sFsj %4i Done ***********************************************************", ThreadIndex*ThreadSpacing, GetCurrentThreadId());
          if (dbgThreadsSleep())
            Sleep(0);
          #endif
          }
        }
      }

    #if dbgFlwNets
    if (dbgThreads())
      dbgpln("%*sFsj %4i FullNetBlkThread Exit", pFST->dbgIndent(GetCurrentThreadId()), "", GetCurrentThreadId());
    #endif

  SCDCATCH("Flw Solve Thread")

  return 0;
  }

//===========================================================================
//
//
//
//===========================================================================
int CFlwSlvThreads::  sm_ThreadSpacing=30;
CMap<DWORD, DWORD, int,int> CFlwSlvThreads::sm_iThread;

CFlwSlvThreads::CFlwSlvThreads(bool IsSlv)
  {
  m_IsSlv=IsSlv;
  }

//---------------------------------------------------------------------------

CFlwSlvThreads::~CFlwSlvThreads()
  {
  };

//---------------------------------------------------------------------------

flag CFlwSlvThreads::CreateThreads(int NThreads, AFX_THREADPROC Proc, LPVOID Data)
  {
  m_pThread.SetSize(NThreads);
  if (NThreads)
    {
    //int RqdPriority=THREAD_PRIORITY_NORMAL;
    int RqdPriority=THREAD_PRIORITY_HIGHEST/*ABOVE_NORMAL*/;
    UINT StackSize=0;
    for (int ip=0; ip<NThreads; ip++)
      {
      if ((m_pThread[ip] = AfxBeginThread(Proc, Data, THREAD_PRIORITY_ABOVE_NORMAL, StackSize, CREATE_SUSPENDED))==NULL)
        MessageBox(GetFocus(),"Error Creating CFlwSolver Thread!","",MB_OK);
      else
        {
        Thread(ip).ResumeThread();

        _clearfp();                                                               // clear any outstanding exceptions
        _controlfp(_EM_DENORMAL|_EM_OVERFLOW|_EM_UNDERFLOW|_EM_INEXACT, _MCW_EM);	//Set New fpControl

        if (RqdPriority!=0)
          SetThreadPriority(ThreadHnd(ip), RqdPriority);
        SetThreadName(m_pThread[ip]->m_nThreadID, "FlwSolver");
        sm_iThread.SetAt(ThreadId(ip), ip+(!m_IsSlv?0:NThreads));
        }
      }
    }
  return true;
  }

//---------------------------------------------------------------------------

void CFlwSlvThreads::CloseThreads()
  {
  for (int ip=0; ip<NThreads(); ip++)
    {
    HANDLE hThread=ThreadHnd(ip);
    Thread(ip).m_bAutoDelete=FALSE;
    BOOL OK=Thread(ip).PostThreadMessage(WMU_FLWSOLVECMD, MAKEWPARAM(FST_Quit,ip), 0);
    ASSERT_ALWAYS(OK, "CFlwSlvThreads::CloseThreads - PostThreadMessage FAILED");
    #if dbgFlwSolve
    if (!OK && dbgThreads())
      dbgpln("%*sPost Done %i", CFlwSlvThreads::dbgIndent(GetCurrentThreadId()), "", OK);
    #endif
    WaitForSingleObject(hThread,INFINITE);
    #if dbgFlwSolve
    if (!OK && dbgThreads())
      dbgpln("%*sWait Done", CFlwSlvThreads::dbgIndent(GetCurrentThreadId()), "");
    #endif
    delete m_pThread[ip];
    }
  m_pThread.SetSize(0);
  }

//===========================================================================
//
//
//
//===========================================================================

FlwSlvThreadControl  FSTC;

flag FlwSlvThreadControl::CreateThreads(int NThreads) 
  {
  CFlwSlvThreads::sm_iThread.InitHashTable(13);

  flag f1=FST.CreateThreads(NThreads, FlwSlvThread, (LPVOID)&FST);
  flag f2=FIT.CreateThreads(NThreads, FlwIterThread, (LPVOID)&FIT);

  return f1 && f2;
  };
void FlwSlvThreadControl::CloseThreads() 
  {
  FST.CloseThreads();
  FIT.CloseThreads();
  CFlwSlvThreads::sm_iThread.RemoveAll();
  };


void FlwSlvThreadControl::DoJob(int NJobs, UINT Cmd, LPVOID This, LPVOID Data)
  {
  ThreadDataBlk TDB(Cmd, This, Data);
  if (Cmd>=FST_Base)
    TDB.StartJob(FST, NJobs);
  else
    TDB.StartJob(FIT, NJobs);
  TDB.WaitJobDone();
  };
void FlwSlvThreadControl::DoJob(UINT Cmd, LPVOID This, LPVOID Data, LPVOID MoreData)
  {
  ThreadDataBlk TDB(Cmd, This, Data, MoreData);
  if (Cmd>=FST_Base)
    TDB.StartJob(FST);
  else
    TDB.StartJob(FIT);
  TDB.WaitJobDone();
  };

//===========================================================================
//
//
//
//===========================================================================
