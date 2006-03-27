//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __FSTHREAD_H
#define  __FSTHREAD_H

#ifndef __SC_DEFS_H
  #include <sc_defs.h>
#endif

#ifndef __FLWNODE_H
  #include <flwnode.h>
#endif

#ifdef __FSTHREAD_CPP
  #define DllImportExport DllExport
#elif !defined(FLWLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//===========================================================================
//
//     Multi Threaded Solution
//
//===========================================================================

#define dbgFlwSolveTiming   0/*(WITHDEBUG || 0)*/
#if (dbgFlwSolveTiming)
extern CStopWatch gs_ExecIterTime;
#endif

//-------------------------------------------------------------------------

const byte eFITNull                 = 0;
const byte eFITBase                 = 1;
const byte eFITQuit                 = eFITBase+0; 
const byte eFITEvalDiscrete         = eFITBase+1;
const byte eFITEvalBalance          = eFITBase+2;
const byte eFITCheckXRefs           = eFITBase+3;
const byte eFITEvalCtrlInitialise   = eFITBase+4;  
const byte eFITEvalCtrlActions      = eFITBase+5;  
const byte eFITEvalCtrlStrategy     = eFITBase+6;  
const byte eFITEvalCtrlTerminate    = eFITBase+7;  
const byte eFITEvalStatistics       = eFITBase+8;   
const byte eFITEvalTerminate        = eFITBase+9;   
const byte eFITConfigureJoins       = eFITBase+10;
const byte eFITElectrics            = eFITBase+11;
const byte eFITPowerRequired        = eFITBase+12;
const byte eFITStates               = eFITBase+13;
const byte eFITEvalSteadyState      = eFITBase+14;          
const byte eFITStartStep            = eFITBase+15;          
const byte eFITFlowInfo             = eFITBase+16;          
const byte eFITEvalProductsSurge    = eFITBase+17;          
const byte eFITEvalProducts         = eFITBase+18;          
const byte eFITEvalDerivs           = eFITBase+19;          
const byte eFITEvalIntegral         = eFITBase+20;          
const byte eFITODEOperate           = eFITBase+21;    
                                                        
const byte eFITGrpMeasure           = eFITBase+25;      
const byte eFITGrpTraverse          = eFITBase+26;    
const byte eFITGrpConverge          = eFITBase+27;    
                                                      
const byte eFITPBEvalMakeUpAvail    = eFITBase+30;    
const byte eFITPBEvalMakeUpReqd     = eFITBase+31;    
const byte eFITPBEvalPressures      = eFITBase+32;    
const byte eFITPBEvalProducts       = eFITBase+33;    
const byte eFITPBEvalDiscrete       = eFITBase+34;    
const byte eFITPBEvalBalance        = eFITBase+35;    
const byte eFITPBEvalCtrlInitialise = eFITBase+36;    
const byte eFITPBEvalCtrlActions    = eFITBase+37;    
const byte eFITPBEvalCtrlStrategy   = eFITBase+38;    
const byte eFITPBEvalCtrlTerminate  = eFITBase+39;    
                                                      
const byte eFITMaxCmd               = eFITBase+40;    
                                                      
                                                      
const byte FST_Base             = 100;                
const byte FST_Quit             = FST_Base+0;         
const byte FST_FlwSolve         = FST_Base+1;

//  eFITCmdLast

extern const char* eFITCmdStr(int i);
extern const char* FST_CmdStr(int i);

//===========================================================================
//
//
//
//===========================================================================

class CFlwSlvThreads
  {
  public:
    CArray<CWinThread*, CWinThread*> m_pThread;
    static CMap<DWORD, DWORD, int,int> sm_iThread;
    static int     sm_ThreadSpacing;


    int            NThreads() { return m_pThread.GetSize(); };
    HANDLE         ThreadHnd(int ip) { return m_pThread[ip]->m_hThread; };
    DWORD          ThreadId(int ip) { return m_pThread[ip]->m_nThreadID; };
    CWinThread &   Thread(int ip) { return *(m_pThread[ip]); };

    CFlwSlvThreads(bool IsSlv);
    ~CFlwSlvThreads();

    flag           CreateThreads(int NThreads, AFX_THREADPROC Proc, LPVOID Data);
    void           CloseThreads();

    bool           m_IsSlv;

    //int            dbgFlwSlvStart(int ThreadIndex)  { return ThreadIndex*m_ThreadSpacing; };
    //int            dbgFlwIterStart(int ThreadIndex) { return (ThreadIndex+NThreads())*m_ThreadSpacing; };
    static int     dbgIndent(DWORD ThreadId) 
                      { 
                      int ip;
                      if (sm_iThread.Lookup(ThreadId, ip))
                        return (ip+1)*sm_ThreadSpacing;
                      //  if (m_IsSlv)
                      //  return dbgFlwSlvStart(ThreadIndex);
                      //return dbgFlwIterStart(ThreadIndex); 
                      return 0;
                      };
    
  };

//===========================================================================
//
//
//
//===========================================================================

class FlwSlvThreadControl
  {
  public:
    CFlwSlvThreads   FST, FIT;

    FlwSlvThreadControl() : FST(true), FIT(false) {};
    ~FlwSlvThreadControl() {};

    flag           CreateThreads(int NThreads);
    void           CloseThreads();

    void           DoJob(int NJobs, UINT Cmd, LPVOID This, LPVOID Data);
    void           DoJob(UINT Cmd, LPVOID This, LPVOID Data, LPVOID MoreData=NULL);

    flag           On()       { return FIT.NThreads()>0; };    
    int            NThreads() { return FIT.NThreads(); };    
  };

extern FlwSlvThreadControl  FSTC;

//===========================================================================
//
//
//
//===========================================================================

template<class CLIST, class ARG_TYPE>
class CLockedListIter : public CObject
  {
  protected:
    CLIST *         pList;
    POSITION        pos;
    flag            bFwd, bStarted;
    CRITICAL_SECTION CritSect;
    HANDLE          hSema;

    int             nThreads, nWaitCount;
    flag            fDone;

  public:
    flag            RetFlag;
    int             RetInt;

    LPVOID          pOtherData;

  public:
    CLockedListIter() 
      {
      pList=NULL; 
      pos=NULL;
      bStarted=false; 
      InitializeCriticalSectionAndSpinCount(&CritSect, 4000);
      // for Win2K
      // InitializeCriticalSectionAndSpinCount(&CritSect, 4000);
      hSema=CreateSemaphore((LPSECURITY_ATTRIBUTES)NULL, 0, 0x7fffffff, NULL);
      nThreads=0;
      nWaitCount=0;
      fDone=false;
      RetFlag=true;
      RetInt=0;
      pOtherData=NULL;

      //dbgpln("CLockedListIter %08x %08x ", this, pList);
      };
	  CLockedListIter(CLIST *Lst, LPVOID OtherData=NULL) 
      {
      pList=Lst; 
      pos=NULL;
      bStarted=0; 
      InitializeCriticalSectionAndSpinCount(&CritSect, 4000);
      // for Win2K
      // InitializeCriticalSectionAndSpinCount(&CritSect, 4000);
      hSema=CreateSemaphore((LPSECURITY_ATTRIBUTES)NULL, 0, 0x7fffffff, NULL);
      nThreads=0;
      nWaitCount=0;
      fDone=false;
      RetFlag=true;
      RetInt=0;
      pOtherData=OtherData;

      //dbgpln("CLockedListIter %08x %08x ", this, pList);
      };
    ~CLockedListIter() 
      { 
      //dbgpln("~CLockedListIter %08x %08x ", this, pList);
      CloseHandle(hSema);
      DeleteCriticalSection(&CritSect); 
      };

    void Initialise(CLIST *Lst, int Threads)
      {
      pList=Lst; 
      pos=NULL;
      bStarted=0; 
      nThreads=Threads;
      nWaitCount=0;
      fDone=false;
      }

    flag GetNext(ARG_TYPE A)
      { 
      EnterCriticalSection(&CritSect); 
      flag OK=true;
      if (!bStarted) 
        pos = pList->GetHeadPosition();
      if (pos) 
        A=pList->GetNext(pos);
      else
        OK=false;
      bStarted=true; 
      LeaveCriticalSection(&CritSect); 
      return OK; 
      };
    
    void AddHead(ARG_TYPE A)
      {
      EnterCriticalSection(&CritSect);
      pList->AddHead(A);
      LeaveCriticalSection(&CritSect);
      ReleaseSemaphore(hSema, 1, NULL);
      };

    void AddTail(ARG_TYPE A)
      {
      EnterCriticalSection(&CritSect);
      pList->AddTail(A);
      LeaveCriticalSection(&CritSect);
      ReleaseSemaphore(hSema, 1, NULL);
      };

    flag RemoveNext(ARG_TYPE A, int * pCnt=NULL)
      {
      EnterCriticalSection(&CritSect);
      nWaitCount++;
      if (nWaitCount==nThreads && pList->IsEmpty() && !fDone)
        {
        fDone=true;
        ReleaseSemaphore(hSema, nThreads, NULL);
        }
      LeaveCriticalSection(&CritSect);

      WaitForSingleObject(hSema, INFINITE);

      EnterCriticalSection(&CritSect);
      nWaitCount--;
      flag OK=!fDone && !pList->IsEmpty();
      if (OK)
        A=pList->RemoveHead();
      if (pCnt)
        *pCnt=pList->GetCount();
      LeaveCriticalSection(&CritSect);
      return OK;
      };
    CLIST & List() { return *pList; }

  };

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

#undef DllImportExport

#endif  // __FSTHREAD_H
