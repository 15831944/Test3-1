//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#include "stdafx.h"
#include "sc_defs.h"
#define __EXECUTIV_CPP
#include "executiv.h"
#include "tagdb.h"
#include <sys\timeb.h>
#include "scd_wm.h"
#include "dlgbusy.h"
#include "..\..\Com\ScdCOM\ScdCOMIF.h"
#include "dbgmngr.h"
#include "datacnvs.h"
#include "atlconv.h"
#include "dbhelper.h"
//#include "scdcomcmdsatlconv.h"
//#include "optoff.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define EXECMAINTHREAD

#define dbgExecutive  (1 || WITHDEBUG)

#if dbgExecutive
static CDbgMngr dbgBuildDataLists ("Executive", "BuildDataLists");
static CDbgMngr dbgCompare        ("Executive", "Compare");
static CDbgMngr dbgDumpConns      ("Executive", "DumpConns");
static CDbgMngr dbgExecReg        ("Executive", "ExecReg");
static CDbgMngr dbgExecSeq        ("Executive", "ExecSeq");
static CDbgMngr dbgExecSeqSimple  ("Executive", "ExecSeq.Simple");
static CDbgMngr dbgExecSeqTm      ("Executive", "ExecSeq.Tm");
static CDbgMngr dbgExecSeqTm1     ("Executive", "ExecSeq.Tm1");
static CDbgMngr dbgLoadTags       ("Executive", "LoadTags");
static CDbgMngr dbgLongIdleLoop   ("Executive", "LongIdleLoop");
static CDbgMngr dbgMemoryUse      ("Executive", "Memory.Use");
static CDbgMngr dbgMsgs           ("Executive", "Msgs");
static CDbgMngr dbgDBMsgs         ("Executive", "Msgs.DB");
static CDbgMngr dbgFullDBMsgs     ("Executive", "Msgs.DB.Full");
static CDbgMngr dbgFullMsgs       ("Executive", "Msgs.Full");
static CDbgMngr dbgSnapMsgs       ("Executive", "Msgs.Snap");
static CDbgMngr dbgFullSnapMsgs   ("Executive", "Msgs.Snap.Full");
static CDbgMngr dbgSubsMsgs       ("Executive", "Msgs.Subs");
static CDbgMngr dbgXSendMsgs      ("Executive", "Msgs.XSend");
static CDbgMngr dbgOtherData      ("Executive", "OtherData");
static CDbgMngr dbgResultsValid   ("Executive", "ResultsValid");
static CDbgMngr dbgRouteCmp       ("Executive", "RouteCmp");
static CDbgMngr dbgTimingStats    ("Executive", "TimingStats");

static CDbgMngr dbgTimeOn         ("_Debug", "Time-Markers");
static CDbgMngr dbgTimeDiffOn     ("_Debug", "Time-Diff-Markers");
#endif

static CDbgMngr dbgSmallDBGFile   ("_Debug", "SmallDBGFile");

#if dbgExecutive
static CStopWatch gs_TimeSW;  // The iteration Timer
#endif

#define dbgStats        (dbgExecutive && 0)
#define dbgThreadTimes  0

//---------------------------------------------------------------------------

inline double GetProcTime()
  {
  FILETIME tCreat, tExit, tKernel, tUser;
  GetProcessTimes(GetCurrentProcess(), &tCreat, &tExit, &tKernel, &tUser);
  LARGE_INTEGER lKernel = {tKernel.dwLowDateTime, tKernel.dwHighDateTime };
  LARGE_INTEGER lUser   = {tUser.dwLowDateTime, tUser.dwHighDateTime };

  return (lKernel.QuadPart+lUser.QuadPart)*1.0e-7; // 100's of nanosecs ticks
  }

//===========================================================================
//
///
//
//===========================================================================

void XFlushAppMsgQ()
  {
  ::WaitForInputIdle(GetCurrentProcess(), 200);
  };
void XDoAppIdle()
  {
  };

//===========================================================================
//
///
//
//===========================================================================

double GetCurrentTime_1970()
  {
  struct timeb t;
  ftime(&t);
  return t.time+t.millitm/1000.0;
  }

//===========================================================================

#define dbgSearchForTag   0
#define dbgSearchForTag1  0

//===========================================================================
// Executive Commands
const UINT EXM_NULL          = 0;
const UINT EXM_START         = 1;
const UINT EXM_STARTPAUSE    = 2;
const UINT EXM_STEP          = 3;
const UINT EXM_STEPNOADV     = 4;
const UINT EXM_QUERYTIMES    = 5;
const UINT EXM_BUILDLISTS    = 6;
const UINT EXM_STARTLOOP     = 7;
const UINT EXM_DOREADALL     = 8;
const UINT EXM_DOREAD        = 9;
const UINT EXM_READALL       = 10;
const UINT EXM_READ          = 11;
const UINT EXM_LASTREAD      = 12;
const UINT EXM_READALLDONE   = 13;
const UINT EXM_READDONE      = 14;
const UINT EXM_LASTREADDONE  = 15;
const UINT EXM_DOWRITE       = 16;
const UINT EXM_DOWRITEALL    = 17;
const UINT EXM_DOLASTWRITE   = 18;
const UINT EXM_WRITEALL      = 19;
const UINT EXM_WRITE         = 20;
const UINT EXM_LASTWRITE     = 21;
const UINT EXM_WRITEDONE     = 22;
const UINT EXM_LASTWRITEDONE = 23;
const UINT EXM_STEPDONE      = 24;
const UINT EXM_DOTEST        = 25;
const UINT EXM_DOEXEC        = 26;
const UINT EXM_EXEC          = 27;
const UINT EXM_EXECDONE      = 28;
const UINT EXM_EXECALLDONE   = 29;
const UINT EXM_BUMPTIME      = 30;
const UINT EXM_STOP          = 31;
const UINT EXM_STOPNOIO      = 32;
const UINT EXM_STOPPED       = 33;
const UINT EXM_QUIT          = 34;
const UINT EXM_FILING        = 35;
const UINT EXM_GOTOEDIT      = 36;
const UINT EXM_GOTOIDLE      = 37;
const UINT EXM_GOTORUN       = 38;
const UINT EXM_DOONESTEP     = 39;


//const UINT EXM_MD_Current    = 0x0000;
//const UINT EXM_MD_PB         = 0x0100;
//const UINT EXM_MD_DYN        = 0x0200;
//
//inline UINT XCMD(UINT C) { return C & 0x00FF; }
//inline UINT XMD(UINT C)  { return C & 0x0F00; }

// temp UNDO
inline UINT XCMD(UINT C) { return C; }


const pchar EXM_MSGS[] =
  {
  "NULLMSG      ",
  "START        ",
  "STARTPAUSE   ",
  "STEP         ",
  "STEPNOADV    ",
  "QUERYTIMES   ",
  "BUILDLISTS   ",
  "STARTLOOP    ",
  "DOREADALL    ",
  "DOREAD       ",
  "READALL      ",
  "READ         ",
  "LASTREAD     ",
  "READALLDONE  ",
  "READDONE     ",
  "LASTREADDONE ",
  "DOWRITE      ",
  "DOWRITEALL   ",
  "DOLASTWRITE  ",
  "WRITEALL     ",
  "WRITE        ",
  "LASTWRITE    ",
  "WRITEDONE    ",
  "LASTWRITEDONE",
  "STEPDONE     ",
  "DOTEST       ",
  "DOEXEC       ",
  "EXEC         ",
  "EXECDONE     ",
  "EXECALLDONE  ",
  "BUMPTIME     ",
  "STOP         ",
  "STOPNOIO     ",
  "STOPPED      ",
  "QUIT         ",
  "FILING       ",
  "GOTOEDIT     ",
  "GOTOIDLE     ",
  "GOTORUN      ",
  "DOONESTEP    ",
  };

//===========================================================================

void CMergeProjectsInfo::MakeNewTag(char* pTag, flag ForMaster, Strng& sNewTag)
  {
  if (ForMaster)
    {
    if (iMasterChangeType!=MRG_ChangeNone)
      {
      switch (iMasterRenameType)
        {
        case MRG_Prefix: sNewTag.Set("%s%s", sMasterPrefix(), pTag); break;
        case MRG_Suffix: sNewTag.Set("%s%s", pTag, sMasterSuffix()); break;
        }
      }
    else
      sNewTag = pTag;
    }
  else
    {
    if (iSlaveChangeType!=MRG_ChangeNone)
      {
      switch (iSlaveRenameType)
        {
        case MRG_Prefix: sNewTag.Set("%s%s", sSlavePrefix(), pTag); break;
        case MRG_Suffix: sNewTag.Set("%s%s", pTag, sSlaveSuffix()); break;
        }
      }
    else
      sNewTag = pTag;
    }
  }

//===========================================================================

CXTagBlk::CXTagBlk()
  {
  wAllocSize=0;

  Options=0;
  eAction=XIO_In;
  lIndex=0;
  wAllocSize=0;

  bClientEnd=0;
  bDataChgd=0;
  bWriting=0;
  pNxtChg=NULL;
  pPrvChg=NULL;
  m_pDataItem=NULL;

  iNextInMap=-1;
  //Rmt=0;
  };

//---------------------------------------------------------------------------

CXTagBlk::~CXTagBlk()
  {
  if (m_pDataItem)
    delete m_pDataItem;
  };

void ObjTagIndex::Clear()
  {
  iObjExecIndex=-1;
  };
void ObjTagIndex::Set(int iObj_, int iTag_)
  {
  iObjExecIndex=iObj_;
  iTag=iTag_;
  };
//===========================================================================

flag CXTagBlk::PutDataChgd(CPkDataItem * pItem)
  {
  flag Chg=0;
  dword DOSz=pItem->ItemSize();
  if (m_pDataItem==NULL)
    {
    m_pDataItem=CPkDataItem::Create(*pItem);
    Chg=1;
    }
  else if ((m_pDataItem->ItemSize() != DOSz) || (*m_pDataItem!=*pItem))
    {
    Chg=1;
    if (pItem->MaxDataSize()>m_pDataItem->MaxDataSize())
      {
      delete m_pDataItem;
      m_pDataItem=CPkDataItem::Create(*pItem);
      }
    else
      *m_pDataItem=*pItem;
    }

  return Chg;
  };

//---------------------------------------------------------------------------

void CXTagBlk::BuildExtTag(char* pTag, XIOAction Act, flag bClient, Strng& KeyTag)
  {
  KeyTag.Set("%c%c%c_%s", bClient ? 'C' : 'S',
                         Act & XIO_In ?  'I' : '_',
                         Act & XIO_Out ? 'O' : '_',
                         pTag);
  }

//---------------------------------------------------------------------------

char* CXTagBlk::ExtractTag(Strng &ExtTag)
  {
  return &ExtTag[4];
  };

//===========================================================================

CXTagBlk* ObjTagIndex::pTag()
  {
  //ASSERT(!Valid() || iTag<gs_Exec.Objs_Exec[iObjExecIndex]->aTags.GetSize());
  return Valid() ? gs_Exec.Objs_Exec[iObjExecIndex]->aTags[iTag] : NULL;
  };

//===========================================================================

double ThreadTimes[MaxXObjects];
double PrvThreadTimes[MaxXObjects];

UINT EO_ExecThread(LPVOID pEO_)
  {
  int OInx;
  CExecObj *pEO=(CExecObj*)pEO_;

  HRESULT hr=::CoInitializeEx(NULL, COINIT_MULTITHREADED);
  if (FAILED(hr))
    {
    LogError(pEO->Name(), 0, "CoInitializeEx failed %08x", hr);
    }

  SCDTRY

    #if dbgExecutive
    if (dbgExecReg())
      dbgpln("EO_Thread Entry [%i] %s", pEO->Id(), pEO->Name());
    TRACE("EO_Thread Entry [%i] %s\n", pEO->Id(), pEO->Name());
    #endif
    //pEO->SetThreadId(GetCurrentThreadId());
    pEO->EO_ThreadInitialise();
    MSG Msg;
    while (pEO->Active() && GetMessage(&Msg, NULL, 0,0))
      if (Msg.message!=WMU_EXECCOMMAND /*IDM_EXECUTIVE_CMD*/)
        TRACE("BAD MESSAGE ExecThread %x\n",Msg.message);
      else
        {
        #if dbgExecutive
        CStopWatch SW;
        if (dbgTimingStats())
          {
          SW.Start();
          }
        #endif
        UINT Cmd=XCMD(Msg.wParam);
        #if dbgExecutive
        if (dbgExecSeq())
          dbgpln("Exec::                                    %s  %8.2f  %s",
                  EXM_MSGS[XCMD(Cmd)], gs_Exec.TimeCB().m_Time-3600.0*floor(gs_Exec.TimeCB().m_Time/3600.0), pEO->Name());
        #endif
        switch (Cmd)
          {
          case EXM_READALL :
          case EXM_READ :
            pEO->XReadSyncData(Cmd==EXM_READALL);
            gs_Exec.PostCmd(pEO, Cmd==EXM_READALL ? EXM_READALLDONE : EXM_READDONE, (long)pEO);
            break;
          case EXM_LASTREAD :
            pEO->XReadSyncData(true);//false);
            gs_Exec.PostCmd(pEO, EXM_LASTREADDONE, (long)pEO);
            break;
          case EXM_WRITEALL :
          case EXM_WRITE :
            pEO->XWriteSyncData(Cmd==EXM_WRITEALL);
            gs_Exec.PostCmd(pEO, EXM_WRITEDONE, (long)pEO);
            break;
          case EXM_LASTWRITE :
            pEO->XWriteSyncData(false);
            gs_Exec.PostCmd(pEO, EXM_LASTWRITEDONE, (long)pEO);
            break;
          case EXM_EXEC :
            OInx=pEO->ObjExecIndex();
            gs_Exec.ThrdRet[OInx].Clear();
            pEO->EO_Execute(gs_Exec.TimeCB(), gs_Exec.ThrdRet[OInx]);
            gs_Exec.PostCmd(pEO, EXM_EXECDONE, (long)pEO);

            #if dbgThreadTimes
            if (1)
              {
              FILETIME tCreat, tExit, tKernel, tUser;
              GetThreadTimes(GetCurrentThread(), &tCreat, &tExit, &tKernel, &tUser);
              LARGE_INTEGER lKernel = {tKernel.dwLowDateTime, tKernel.dwHighDateTime };
              LARGE_INTEGER lUser   = {tUser.dwLowDateTime, tUser.dwHighDateTime };
              ThreadTimes[OInx]=(lKernel.QuadPart+lUser.QuadPart)*1.0e-7; // 100's of nanosecs ticks
              }
            #endif
            break;
          case EXM_QUIT :
            break;

          default:
            TRACE("BAD MESSAGE ExecThread %x\n",Msg.message);
          }
        #if dbgExecutive
        if (dbgTimingStats())
          dbgpln("EX %s    : %10.6f %s",EXM_MSGS[XCMD(Cmd)], SW.Lap(), pEO ? pEO->Name() : "EXEC");
        #endif
        }

    pEO->EO_ThreadTerminate();

  SCDCATCH1(pEO->Name());
#ifndef _RELEASE
  DoBreak();
    int xxx=0;
#endif
  SCDCATCH2(pEO->Name());

  ::CoUninitialize();

  #if dbgExecutive
  if (dbgExecReg())
    dbgpln("EO_Thread Exit  [%i] %s",pEO->Id(), pEO->Name());
  TRACE("EO_Thread Exit  [%i] %s\n",pEO->Id(), pEO->Name());
  #endif
  return 0;
  }

//===========================================================================

CExecObj::CExecObj()
  //: aTags(*(new CXTagArray)),
  //  mTags(*(new CXTagMap))
  {
  InitializeCriticalSectionAndSpinCount(&csObjLock, 4000);

  lId=-1;
  nTags=0;
  nHashSize=17;
  pObjThread=NULL;
  //dwThreadId=0;
  bServer=0;
  bClient=0;
  bSelfConnect=0;
  bMarkedForDelete=0;
  bSyncDataIO=1;
  bAllowForceWriteOnStart=0;
  bListUpdateReqd=0;
  bQueryAvailExecd=0;
  dEOTime=0.0;
  aTags.SetSize(0, 64);
  }

//---------------------------------------------------------------------------

CExecObj::~CExecObj()
  {
  if (!bMarkedForDelete)
    EO_DeRegister();
  ClearDataLists();
  //delete &aTags;
  //delete &mTags;
  DeleteCriticalSection(&csObjLock);
  };

//===========================================================================

flag CExecObj::EO_Register(const char* pName_, byte bExec_, int RqdPriority, DWORD StackSize, int HashSize/*=17*/)
  {
  bExec=bExec_;
  nHashSize=HashSize;
  if (lId>=0)
    EO_DeRegister();
  if (strrchr(pName_, '\\'))
    pName_=strrchr(pName_, '\\');
  sName=pName_;
  #if dbgExecutive
  if (dbgExecReg())
    dbgpln("Register:%s===============================",sName());
  TRACE("Register:%s===============================\n",sName());
  #endif
  lId=-1;
  bReadAllData=1;

  gs_Exec.Register(this);

  if (bExec & (EOWrite_Thread | EORead_Thread | EOExec_Thread))
    {
    RqdPriority=Min(RqdPriority,MaxRqdPriority);

    //hExitEvent = CreateEvent(NULL, false, false, NULL);
    bActive=1;

    if (StackSize>0)
      StackSize+=0;//PkDataListLength*4; // for Safety


    if ((pObjThread = AfxBeginThread(EO_ExecThread, (LPVOID)this, THREAD_PRIORITY_NORMAL, StackSize, CREATE_SUSPENDED))==NULL)
      {
      MessageBox(GetFocus(),"Error Creating Object Exec Thread!","",MB_OK);
      bActive=0;

      //SetThreadPriorityBoost(pObjThread->m_hThread, true);

      //return 1;
      }
    else
      {
      pObjThread->ResumeThread();

      _clearfp();                                                               // clear any outstanding exceptions
      _controlfp(/*_EM_ZERODIVIDE|_EM_OVERFLOW|*/_EM_DENORMAL|_EM_UNDERFLOW|_EM_INEXACT, _MCW_EM); //Set New fpControl

      if (RqdPriority!=0)
        {
        //int Pri=GetThreadPriority(hThread);
        //SetThreadPriority(pObjThread->m_hThread,Range(-2,RqdPriority,2));
        SetThreadPriority(pObjThread->m_hThread,RqdPriority);
        }
      SetThreadName(pObjThread->m_nThreadID, pName_);
      }
    }
  return true;
  }

//---------------------------------------------------------------------------

flag CExecObj::EO_DeRegister()
  {
  if (lId>=0)
    {
    #if dbgExecutive
    if (dbgExecReg())
      dbgpln("DeRegister:%s===============================",sName());
    TRACE("DeRegister:%s===============================\n",sName());
    #endif
    bActive=0;
    if (bExec & (EOWrite_Thread | EORead_Thread | EOExec_Thread))
      {
      bActive=0;
      HANDLE hThread=pObjThread->m_hThread;
      PostThreadMessage(ThreadId(), WMU_EXECCOMMAND, MAKEWPARAM(EXM_QUIT,0), 0);
      WaitForSingleObject(hThread, INFINITE);

      //CloseHandle(hExitEvent);
      //CloseHandle(hThread);
      };
    flag ToDelete=bMarkedForDelete;
    gs_Exec.DeRegister(this);
    ClearDataLists();
    if (!ToDelete)
      lId=-1;
    }
  return true;
  };

//---------------------------------------------------------------------------

void CExecObj::EO_MarkForDelete() { bMarkedForDelete=1; };
void CExecObj::EO_AllowForceWriteOnStart(BOOL On) { bAllowForceWriteOnStart=On; };
void CExecObj::EO_SynchronousDataIO() { bSyncDataIO=1; };
void CExecObj::EO_ASynchronousDataIO() { bSyncDataIO=0; };

//===========================================================================

flag CExecObj::EO_ChangeName(const char* pName_)
  {
  if (strrchr(pName_, '\\'))
    pName_=strrchr(pName_, '\\');
  sName=pName_;
  #if dbgExecutive
  if (dbgExecReg())
    dbgpln("ChangeName:%s ----------------------------",sName());
  #endif
  return true;
  }

//---------------------------------------------------------------------------

void CExecObj::ClearDataLists()
  {
  for (int i=0; i<aTags.GetSize(); i++)
    delete aTags[i];
  aTags.RemoveAll();
  nTags=0;
  mTags.RemoveAll();
  mTags.InitHashTable(nHashSize); // a Prime number ?
  }

//---------------------------------------------------------------------------

int CExecObj::AddXTagBlk(int Index, CXTagBlk * p)
  {
  int I;
  if (mTags.Lookup(p->sExtTag(), I))
    {
    while (aTags[I]->iNextInMap>=0)
      I=aTags[I]->iNextInMap;
    aTags[I]->iNextInMap=Index;
    p->iNextInMap=-1;
    }
  else
    mTags.SetAt(p->sExtTag(), Index);
  //aTags.SetAtGrow(Index, p);
  //nTags=Max(nTags, Index+1);
  return 0;
  };

//---------------------------------------------------------------------------

int CExecObj::RemoveXTagBlk(int Index, CXTagBlk * p)
  {
  int I;
  if (mTags.Lookup(p->sExtTag(), I))
    {
    if (aTags[I]->iNextInMap<0)
      mTags.RemoveKey(p->sExtTag());
    else
      {
      if (I==Index)
        {
        mTags.RemoveKey(p->sExtTag());
        int I1=aTags[I]->iNextInMap;
        mTags.SetAt(aTags[I1]->sExtTag(), aTags[I1]->lIndex);
        }
      else
        {
        while (aTags[I]->iNextInMap!=Index)
          I=aTags[I]->iNextInMap;
        aTags[I]->iNextInMap=aTags[Index]->iNextInMap;
        }
      }

    }
  else
    {
    ASSERT(0); // should never get here !
    }

  //aTags.RemoveAt(Index, p);
  //mTags.SetAt(p->sExtTag(), Index);
  //nTags=Max(nTags, Index+1);
  p->iNextInMap=-1;
  return 0;
  };

//---------------------------------------------------------------------------

flag CExecObj::LoadTags(CXMsgLst &XM, flag ClientEnd, long &LastIndex)
  {
  //t n=;
  if (XM.NoMsgs()>0)
    {
    while (XM.MsgAvail())
      {
      CXM_DataRequest * pR=NULL;
      CXM_DataAvailable * pA=NULL;
      if (XM.MsgAvail(XM_DataRequest))
        pR=XM.DataRequest();
      else
        pA=XM.DataAvailable();
      int Index=pR ? pR->Index : pA->Index;
      CXTagBlk* p=new CXTagBlk;

      if (pR)
        {
//dbgpln("Rqd %08x%08x %s", pR->Options, pR->Tag);
        CXTagBlk::BuildExtTag(pR->Tag, pR->Action, ClientEnd, p->sExtTag);
        p->Options=pR->Options;
        p->eAction=pR->Action;
        }
      else
        {
//dbgpln("Avl %16s %s", "", pA->Tag);
        CXTagBlk::BuildExtTag(pA->Tag, pA->Action, ClientEnd, p->sExtTag);
        p->Options=0; //???????????????????????????????
        p->eAction=pA->Action;
        }
      p->lIndex=Index;

      LastIndex=Index;
      p->bClientEnd=ClientEnd;
      p->bDataChgd=0;
      p->pNxtChg=NULL;
      p->pPrvChg=NULL;
      p->wAllocSize=0;
      //p->m_pDataBuff=NULL;
      p->Rmt.Clear();
      if (p->eAction == XIO_InOut)
        p->bWriting=p->bClientEnd;
      else
        p->bWriting=((p->eAction & XIO_In) != 0);

      AddXTagBlk(Index, p);
      aTags.SetAtGrow(Index, p);
      //mTags.SetAt(p->sExtTag(), Index);
      nTags=Max(nTags, Index+1);

      #if dbgExecutive
      if (dbgLoadTags())
      dbgpln("--->LoadTag %s %3i %s : %s%s %s", ClientEnd ? "Clnt ":"Srvr ",
             Index, sName(),
             p->eAction & XIO_In ? "I" : " ",
             p->eAction & XIO_Out ? "O" : " ", p->sExtTag());
      #endif
      }

    return true;
    }
  return false;
  }

//---------------------------------------------------------------------------

flag CExecObj::UpdateTags(CXMsgLst &XM, flag ClientEnd, long &LastIndex)
  {
  //t n=;
  if (XM.NoMsgs()>0)
    {
    Strng ExtTag;
    while (XM.MsgAvail())
      {
      CXM_DataRequest * pR=NULL;
      CXM_DataAvailable * pA=NULL;
      if (XM.MsgAvail(XM_DataRequest))
        pR=XM.DataRequest();
      else
        pA=XM.DataAvailable();
      int Index=pR ? pR->Index : pA->Index;

      //ol Found;
      ObjTagIndex OTI;
      Strng NewExtTag;
      if (pR)
        {
        CXTagBlk::BuildExtTag(pR->Tag, pR->Action, ClientEnd, NewExtTag);
        //Found=SearchForTag(pR->Tag, pR->Action, ClientEnd, OTI);
        }
      else
        {
        CXTagBlk::BuildExtTag(pA->Tag, pA->Action, ClientEnd, NewExtTag);
        //Found=SearchForTag(pA->Tag, pA->Action, ClientEnd, OTI);
        }

      LastIndex=Index;

      flag FixIt=1;
      flag NewOne=0;
      CXTagBlk* p;
      if (Index<aTags.GetSize() && (aTags[Index]!=NULL))
        {
        if (aTags[Index]->sExtTag.XStrCmp(NewExtTag)==0)
          FixIt=0;
        else
          {
          p=aTags[Index];
          RemoveXTagBlk(Index, p);
          }
        }
      else
        {
        p=new CXTagBlk;
        NewOne=1;
        }

      if (FixIt)
        {
        if (pR)
          {
          CXTagBlk::BuildExtTag(pR->Tag, pR->Action, ClientEnd, p->sExtTag);
          p->Options=pR->Options;
          p->eAction=pR->Action;
          }
        else
          {
          CXTagBlk::BuildExtTag(pA->Tag, pA->Action, ClientEnd, p->sExtTag);
          p->Options=0;
          p->eAction=pA->Action;
          }


        p->lIndex=Index;

        p->bClientEnd=ClientEnd;
        p->bDataChgd=0;
        p->pNxtChg=NULL;
        p->pPrvChg=NULL;
        p->wAllocSize=0;
        //p->m_pDataBuff=NULL;
        p->Rmt.Clear();
        if (p->eAction == XIO_InOut)
          p->bWriting=p->bClientEnd;
        else
          p->bWriting=((p->eAction & XIO_In) != 0);

        //if (NewOne)
        AddXTagBlk(Index, p);
        aTags.SetAtGrow(Index, p);
        //mTags.SetAt(p->sExtTag(), Index);
        nTags=Max(nTags, Index+1);

        #if dbgExecutive
        if (dbgLoadTags())
        dbgpln("UpdtTag %s %3i %s : %s%s %s", ClientEnd ? "Clnt ":"Srvr ",
               Index, sName(),
               p->eAction & XIO_In ? "I" : " ",
               p->eAction & XIO_Out ? "O" : " ", p->sExtTag());
        #endif
        }
      }

    return true;
    }
  return false;
  }

//---------------------------------------------------------------------------

flag CExecObj::SearchForTag(pchar pTag, XIOAction Action, flag ClientEnd, rObjTagIndex OTI)
  {
  Strng ExtTag;
  CXTagBlk::BuildExtTag(pTag, Action, ClientEnd, ExtTag);
  int Index;
  if (mTags.Lookup(ExtTag(), Index))
    {
    OTI.iObjExecIndex=iObjExecIndex;
    OTI.iTag=Index;

    #if dbgSearchForTag1
    dbgpln(" -FOUND");
    #endif
    #if dbgSearchForTag
    CXTagBlk* p=aTags[Index];
    dbgpln("Search Found [%3i:%3i] %2i %s @ %s", iObjExecIndex, Index, Action,p->Tag(), pTag, sName());
    #endif
    return true;
    }

  return false;
  }

//---------------------------------------------------------------------------

flag CExecObj::PutData(CXTagBlk &b, CPkDataItem * pItem, flag ForceRead)
  {
  if (b.PutDataChgd(pItem))
    {
    b.bDataChgd=1;
    }

  return b.bDataChgd;
  }

//---------------------------------------------------------------------------

flag CExecObj::XReadSyncData(flag ForceRead)
  {
  if (!bSyncDataIO)
    return false;
  else
    {

    CXMsgLst XM;
    flag Start=1;
    long LastIndex=0;
    ForceRead = ForceRead || bReadAllData;
    bReadAllData=0;
    while (1)
      {
      XM.Clear();
      //CXM_ReadIndexedData RID(Start, ForceRead, LastIndex);
      XM.PackMsg(new CXM_ReadIndexedData(Start, ForceRead, LastIndex));
      Start=0;
      if(EO_ReadSubsData(XM))
        {
        #if dbgExecutive
        if (dbgSubsMsgs())
          XM.dbgDump((flag)dbgFullMsgs(), sName());
        #endif
        while (XM.MsgAvail())
          {
          CXM_ObjectData *pData=XM.ObjectData();
          CPkDataItem * pItem=pData->List.FirstItem();
          long Index=pData->Index;
          ASSERT(Index>=0 && Index<aTags.GetSize());
          CXTagBlk* LclBlk=aTags[Index];
          bool InError=false;
          if (!pItem)
            {
            InError=true;
            pItem=CPkDataItem::Create(1024);
            PkDataUnion Data;
            Data.SetTypeString(tt_Strng, "Missing");
            pItem->AppendValue(0, Data);
            pItem->AppendTag(LclBlk->sExtTag(), "");
            }

          if (LclBlk && pItem)
            {
            flag IORev;
            IORev=(LclBlk->eAction == XIO_InOut && LclBlk->bClientEnd);
            if (IORev)
              LclBlk->bWriting=!LclBlk->bWriting;
            if (!LclBlk->bWriting)
              {
              PutData(*LclBlk, pItem, ForceRead);
              if (LclBlk->Rmt.Valid())
                {
                CXTagBlk &RmtBlk=*(LclBlk->Rmt.pTag());
                PutData(RmtBlk, pItem, ForceRead);
                if (IORev)
                  RmtBlk.bWriting=!RmtBlk.bWriting;
                }
              }
            }
          if (InError)
            delete pItem;
          LastIndex=Index;
          }
        }
      else
        break;
      }
    return true;
    }
  }

//---------------------------------------------------------------------------

flag CExecObj::XWriteSyncData(flag ForceWrite)
  {
  if (!bSyncDataIO)
    return false;
  else
    {
    //flag ResWasValid=TaggedObject::GlblResultsValid();
    CXMsgLst XM;
    flag FirstBlock=true;
    for (int i=0; i<nTags; i++)
      {
      flag flush=0;
      ASSERT(i<aTags.GetSize());
      CXTagBlk* LclBlk=aTags[i];
      if (LclBlk)
        {
        if (LclBlk->bWriting)
          {
          if (LclBlk->Rmt.Valid() && LclBlk->Rmt.pTag()->m_pDataItem)
            PutData(*LclBlk, LclBlk->Rmt.pTag()->m_pDataItem, false);
          if (LclBlk->Changed() || (bAllowForceWriteOnStart && ForceWrite && LclBlk->m_pDataItem/*Why do I need?*/))
            {
            if (XM.PackMsg(new CXM_ObjectData(i, 0, LclBlk->m_pDataItem)))
              LclBlk->ClearChanged();
            else
              flush=1;
            }
          }
        if (LclBlk->eAction == XIO_InOut)
          LclBlk->bWriting=LclBlk->bClientEnd;
        if (flush)
          {
          #if dbgExecutive
          if (dbgSubsMsgs())
              XM.dbgDump((flag)dbgFullMsgs(), sName());
          #endif
          EO_WriteSubsData(XM, FirstBlock, false);
          FirstBlock=false;
          XM.Clear();
          i--;
          }
        }
      }

    #if dbgExecutive
    if (dbgSubsMsgs())
        XM.dbgDump((flag)dbgFullMsgs(), sName());
    #endif
    EO_WriteSubsData(XM, FirstBlock, true);
    FirstBlock=false;
    XM.Clear();
    //if (TaggedObject::GlblResultsValidChg(true))
    XGlblResultValidity();//TaggedObject::GlblResultsValid());
//    if (TaggedObject::GlblResultsValid()!=ResWasValid)
//      XGlblResultValidity(TaggedObject::GlblResultsValid());
    return true;
    }
  }

//---------------------------------------------------------------------------

flag CExecObj::XReadASyncData(CXMsgLst &XM, flag ReadAll, long &Index)
  {
  if (bSyncDataIO)
    return false;
  else
    {
    XM.Clear();
    for (flag Done=0; Index<nTags && !Done; Index++)
      {
      CXTagBlk* LclBlk=aTags[Index];
      if (LclBlk)
        {
        if (LclBlk->bWriting)
          {
          if (LclBlk->Rmt.Valid() && LclBlk->Rmt.pTag()->m_pDataItem)
            PutData(*LclBlk, LclBlk->Rmt.pTag()->m_pDataItem, false);
          if (LclBlk->Changed())
            {
            INCOMPLETECODE1("Test This Code");
            if (XM.PackMsg(new CXM_ObjectData(Index, 0, LclBlk->m_pDataItem)))
              LclBlk->ClearChanged();
            else
              {
              Done=1;
              Index--;
              }
            }
          }
        if (LclBlk->eAction == XIO_InOut)
          LclBlk->bWriting=LclBlk->bClientEnd;
        }
      }

    #if dbgExecutive
    if (dbgSubsMsgs())
        XM.dbgDump((flag)dbgFullMsgs(), sName());
    #endif
    //EO_WriteSubsData(XM, true);
    //XM.Clear();
    return XM.NoMsgs()>0;
    }
  };

//---------------------------------------------------------------------------

flag CExecObj::XWriteASyncData(CXMsgLst &XM)
  {
  return true;
  };

//---------------------------------------------------------------------------

flag CExecObj::XReadASyncDataItem(long Index, CXM_ObjectData &ObjData)
  {
  // todo
  return false;
  };

//---------------------------------------------------------------------------

int CExecObj::XChangeTag(pchar pOldTag, pchar pNewTag) { return gs_Exec.ChangeTag(pOldTag, pNewTag); };
int CExecObj::XDeleteTag(pchar pDelTag) { return gs_Exec.DeleteTag(pDelTag); };

flag CExecObj::XFindObject(char* pName, CXM_Route &Route, flag AllowSelf) { return gs_Exec.XFindObject(this, pName, Route, AllowSelf);};
flag CExecObj::XFindObject(const char* pName, CXM_Route &Route, flag AllowSelf) { return gs_Exec.XFindObject(this, (char*)pName, Route, AllowSelf);};
DWORD CExecObj::XSendMessage(CXMsgLst &XM, CXM_Route &Route, flag FromExecThread) { return gs_Exec.XSendMessage(this, XM, Route, FromExecThread);};
//flag CExecObj::XOpenTransaction()  { return gs_Exec.XOpenTransaction(this); };
flag CExecObj::XBuildDataLists()   { return gs_Exec.XBuildDataLists(this); };
flag CExecObj::XBuildMyDataLists() { return gs_Exec.XBuildMyDataLists(this); };
//flag CExecObj::XUpdateData()       { return gs_Exec.XUpdateData(this); };
DWORD CExecObj::XReadTaggedItem(CXM_ObjectTag & ObjTag, CXM_ObjectData &ObjData, CXM_Route &Route) { return gs_Exec.XReadTaggedItem(this, ObjTag, ObjData, Route); };
int  CExecObj::XWriteTaggedItem(CXM_ObjectData &ObjData, CXM_Route &Route) { return gs_Exec.XWriteTaggedItem(this, ObjData, Route); };
void CExecObj::XIdle() { gs_Exec.GotoIdleMode(); };
void CExecObj::XStop() { gs_Exec.GotoEditMode(); };
void CExecObj::XGlblResultValidity(int IsValid, int IsAlwaysValid) { gs_Exec.XGlblResultValidity(IsValid, IsAlwaysValid); };
//flag CExecObj::XCloseTransaction() { return gs_Exec.XCloseTransaction(this); };

void CExecObj::SetServer(flag Server) { bServer=Server; bClient=0; if (!bServer) SetSelfConnect(0);};
void CExecObj::SetClient(flag Client) { bClient=Client; bServer=0; if (!bClient) SetSelfConnect(0);};
void CExecObj::SetSelfConnect(flag SelfConnect) { bSelfConnect=SelfConnect; };

//===========================================================================

int CCompLineArray::FindInsertPos(double Err)
  {
  if (GetSize()==0)
    return 0;
  if (Err>GetAt(0).dErr)
    return 0;
  else if (GetSize()==1)
    return 1;
  int MaxPos = GetSize()-1;
  if (Err<GetAt(MaxPos).dErr)
    return GetSize();
  int MinPos = 0;
  //WORD Cnt = 0;
  int Diff = MaxPos - MinPos;
  while (Diff>1)
    {
    const int TestPos = (int)(Diff/2) + MinPos;
    if (Err>GetAt(TestPos).dErr)
      MaxPos = TestPos;
    else
      MinPos = TestPos;
    Diff = MaxPos - MinPos;
    }
  if (Diff==1)
    return (Err>GetAt(MinPos).dErr ? MinPos : MaxPos);
  return MinPos;
  }

//---------------------------------------------------------------------------

void CCompareInfo::AddErr(double Err, char* pErr, char* pTag, char* pV1, char* pV2)
  {
  if (bWindow || bShowSortedFile)
    {
    if (List.GetSize()<iMaxCount || Err>dMinErr)
      {
      if (List.GetSize()==iMaxCount)
        List.RemoveAt(iMaxCount-1);
      int i = List.FindInsertPos(Err);
      //dbgpln("--- %2d : Insert %g at %d ---", List.GetSize(), Err, i);
      //for (int j=0; j<List.GetSize(); j++)
      //  dbgpln("%3d  %g", j, List[j].dErr);
      List.InsertAt(i, CCompResultLine(Err, pErr, pTag, pV1, pV2));
      if (List.GetSize()==1)
        dMinErr = Err;
      else
        dMinErr = Min(dMinErr, Err);
      }
    }
  else
    {
    if (bListHeaderRqd)
      {
      // NB Must Test
      Strng s;
      s.Set("Tag\t%sValue1\tValue2\tError\n", (/*Info.*/bShowAllTags ? "Changed\t" : ""));
      pOutF->WriteString(s());
      bListHeaderRqd=false;
      }

    pOutF->WriteString(pTag);
    if (bShowAllTags)
      pOutF->WriteString(Err>1.0 ? "\tY" : "\t");
    Strng s;
    s.Set("\t%s\t%s\t%g\n", pV1, pV2, Err);
    pOutF->WriteString(s());
    }
  }

//===========================================================================
//
//
//
//===========================================================================


class LclXSendMsgBlk
  {
  public:
    CEvent     m_Done;
    CExecObj  *m_pEO;
    CXMsgLst   *m_pXM;
    CXM_Route  *m_pRoute;
    DWORD      m_Return;
  };

UINT ExecThread(LPVOID pExec_)
  {
  CExecutive* pExec=(CExecutive*)pExec_;
  #if dbgExecutive
  if (dbgExecReg())
    dbgpln("Executive_Thread Entry");
  TRACE("Executive_Thread Entry\n");
  #endif

  SCDTRY

    if (0) // force crash
      {
      double x=1;
      double y=0;
      double z=x/y;
      }

    MSG Msg;
    flag bBusy=1;
    while (bBusy && GetMessage(&Msg, NULL, 0,0))
      switch (Msg.message)
        {
        case WMU_EXECCOMMAND:
          {
          UINT Cmd=XCMD(LOWORD(Msg.wParam));
          gs_Exec.DoOneExecMessage(Msg.wParam, Msg.lParam);
          if (Cmd==EXM_QUIT)
            bBusy=0;
          break;
          }
        case WMU_XSENDMESSAGE:
          {
          LclXSendMsgBlk *pLXSM=(LclXSendMsgBlk *)Msg.lParam;
          pLXSM->m_Return=gs_Exec.XSendMessage(pLXSM->m_pEO, *pLXSM->m_pXM, *pLXSM->m_pRoute);
          pLXSM->m_Done.SetEvent();
          break;
          }
        case WM_TIMER:
          if (Msg.wParam==gs_Exec.TimerId())
            gs_Exec.DoTimer();
          else
            {};
          break;
        default:
          TRACE("BAD MESSAGE ExecThread %x\n", Msg.message);
        }

    #if dbgExecutive
    if (dbgExecReg())
      dbgpln("Executive_Thread Exit");
    TRACE("Executive_Thread Exit\n");
    #endif

    gs_Exec.ClearThread();

  SCDCATCH("ExecutiveThread")

  AfxEndThread(0);
  return 0;
  }

//===========================================================================
//
//
//
//===========================================================================

CExecutive::CExecutive() :
  XSW(this),
  XSWDsp(this),
  m_Seq(this)
  {

  Ids=0;


  pExecThread=NULL;
  pSolveTool=NULL;

  InitializeCriticalSection(&csExecLock);
  InitializeCriticalSection(&csTimerLock);
  m_hAppIdleDone=CreateEvent(NULL, false, false, NULL);
  m_hStopSleep=CreateEvent(NULL, false, false, NULL);

  m_dwLoopCount=0;

  DynTmCB.m_bRealTime      = false;
  DynTmCB.m_bSyncWithClock = false;
  DynTmCB.m_bHoldAdv       = false;
  DynTmCB.m_Time           = 0.0;
  DynTmCB.m_StepSize       = 1.0;
  DynTmCB.m_StepSizeMax    = 1.0;
  DynTmCB.m_StepSizeNxt    = dNAN;
  DynTmCB.m_StartTime      = dNAN;
  DynTmCB.m_StopTime       = dNAN;
  DynTmCB.m_RealTimeMult   = 1.0;
  DynTmCB.m_nSteps         = 0;
  DynTmCB.m_nMaxSteps      = -1;

  DynTmCB.m_iRunTerm       = RTRM_None;
  DynTmCB.m_dTimeUntilStop = 3600.0;

  DynTmCB.m_EqnCB.Cfg.InitDynamic();

  PBTmCB.m_bRealTime      = false;
  PBTmCB.m_bSyncWithClock = false;
  PBTmCB.m_bHoldAdv       = false;
  PBTmCB.m_Time           = 0.0;
  PBTmCB.m_StepSize       = 1.0;
  PBTmCB.m_StepSizeMax    = 1.0;
  PBTmCB.m_StepSizeNxt    = dNAN;
  PBTmCB.m_StartTime      = dNAN;
  PBTmCB.m_StopTime       = dNAN;
  PBTmCB.m_RealTimeMult   = 1.0;
  PBTmCB.m_nSteps         = 0;
  PBTmCB.m_nMaxSteps      = -1;
  PBTmCB.m_iRunTerm       = RTRM_None;
  PBTmCB.m_dTimeUntilStop = 1.0;

  PBTmCB.m_EqnCB.Cfg.InitProBal();

  wState = EXF_Stopped;

  fDoingPB=false;

  bActive=true;
  bStopReqd=false;
  bStepReqd=false;
  bPauseReqd=false;
  bListRebuildReqd=false;
  bListUpdateReqd=false;
  bRegistrationListtoChange=false;
  bSolveListRebuildReqd=false;
  bHoldAdv=false;
  bAutomationBusy=false;
  m_bSendIdleEvent=false;
  m_bSendStopEvent=false;
  m_ReadWriteAllReqd=false;
  m_GotoCmdOnceStopped=EXM_NULL;

  m_bDidPauseWork=false;
  m_lPauseUpdateCount=0;
  m_lWaitCompleteCount=0;
  m_lWaitCompleteTime=0;

  eFileWhat=EF_Null;

  lLastTickCount=0;

  lExecThreadsBusy=0;
  lExecThreadsDone=0;

  eCoupling=XC_None;
  NObjs_Reg=0;
  bSelfConnect=0;
  bTimerExpired=0;
  bWorkDone=0;
  iTimerId=0;

  for (int i=0; i<MaxXObjects; i++)
    ThrdRet[i].Clear();

  #if dbgKeepMsgHistory
  for (long l=0; l<MaxMsgHistory; l++)
    MsgHistory[l].Cmd=0;
  lMsgHistory=-1;
  #endif

  bDoAppActivate=0;
  bDoGlblResultValidity=false;

  eRqdState=MSA_Null;

  lUpdateTagDataIterCount=0;

	m_bHstRestartOnStart = 0;
  m_bHstSetTaglistOnStart = 0;
	m_bResetTimeOnStart = 0;
  m_dOnStartTime = 0.0;

  m_dRunTime=0.0;
  m_dTheStartTime=dNAN;
  m_dTheStopTime=dNAN;
  m_dDynProgress=-1.0;

  LastLowerPriority=0;

  m_DBFormat=DBConnect_JET2000;
  };

//---------------------------------------------------------------------------

CExecutive::~CExecutive()
  {
  if (pExecThread)
    {
    HANDLE hThread=pExecThread->m_hThread;
    PostThreadMessage(ExecThreadId(), WMU_EXECCOMMAND, MAKEWPARAM(EXM_QUIT,0), 0);
    WaitForSingleObject(hThread,INFINITE);
    }

  bActive=0;

  CloseHandle(m_hAppIdleDone);
  CloseHandle(m_hStopSleep);

  DeleteCriticalSection(&csExecLock);
  DeleteCriticalSection(&csTimerLock);
  };

//---------------------------------------------------------------------------

void CExecutive::Initialise()
  {
  if (pExecThread!=NULL)
    return;

  if ((pExecThread = AfxBeginThread(ExecThread, (LPVOID)this, THREAD_PRIORITY_NORMAL, 1000*1024L, CREATE_SUSPENDED))==NULL)
    {
    MessageBox(GetFocus(),"Error Creating Executive Thread!","",MB_OK);
    VERIFY(0);
    //return 1;
    }
  else
    {
    pExecThread->ResumeThread();
    SetThreadName(pExecThread->m_nThreadID, "Executive");
    }
  };

//---------------------------------------------------------------------------

void CExecutive::Terminate()
  {
  if (pExecThread==NULL)
    return;
  HANDLE hThread=pExecThread->m_hThread;
  PostThreadMessage(ExecThreadId(), WMU_EXECCOMMAND, MAKEWPARAM(EXM_QUIT,0), 0);
  WaitForSingleObject(hThread,INFINITE);
  pExecThread=NULL;
  }

//---------------------------------------------------------------------------

flag CExecutive::Register(CExecObj *pEO, flag FixingRegistrations)
  {
  LockExec();
  if (Busy/*Running*/() && !FixingRegistrations)
    {
    Objs_ToReg.Add(pEO);
    bRegistrationListtoChange=true;
    #if dbgExecutive
    if (dbgExecReg())
      dbgpln("Add to Reg List");
    #endif
    }
  else
    {
    ASSERT(Objs_Reg.GetSize() < MaxXObjects);
    pEO->lId=Ids++;
    Ids=Max(0L, Ids);

    #if dbgExecutive
    if (dbgExecReg())
      dbgpln("EO_Register     [%i] %s", pEO->lId, pEO->sName());
    #endif

    CExecObj *Objs[MaxXObjects];
    for (int N=0; N<Objs_Reg.GetSize(); N++)
      Objs[N]=Objs_Reg[N];
    Objs[N++]=pEO;

    Objs_Reg.SetSize(0, 16);

    int iSelfConnClnt=-1, iSelfConnSrvr=-1;
    for (int i=0; i<N; i++)
      if (Objs[i]->SelfConnect())
        {
        if (Objs[i]->Server())
          iSelfConnSrvr=i;
        if (Objs[i]->Client())
          iSelfConnClnt=i;
        }

    bSelfConnect=(iSelfConnClnt>=0 && iSelfConnSrvr>=0);

    // Client Connect First if SelfConnected
    if (bSelfConnect)
      {
      Objs_Reg.Add(Objs[iSelfConnClnt]);
      Objs[iSelfConnClnt]=NULL;
      }

    // Add All Non Network Stuff
    for (i=0; i<N; i++)
      if (Objs[i]!=NULL && !Objs[i]->Server() && !Objs[i]->Client())
        {
        Objs_Reg.Add(Objs[i]);
        Objs[i]=NULL;
        }

    // Add All Servers
    for (i=0; i<N; i++)
      if (Objs[i]!=NULL && Objs[i]->Server())
        {
        Objs_Reg.Add(Objs[i]);
        Objs[i]=NULL;
        }

    // Add All Other Clients
    for (i=0; i<N; i++)
      if (Objs[i]!=NULL && Objs[i]->Client())
        {
        Objs_Reg.Add(Objs[i]);
        Objs[i]=NULL;
        }

    bSolveListRebuildReqd=true;

    #if dbgExecutive
    if (dbgExecReg())
      {
      dbgpln("Registered Objects");
      for (i=0; i<Objs_Reg.GetSize(); i++)
        {
        dbgp("    %3i) %-4.4s",i,Objs_Reg[i]->SelfConnect() ? "Self": "");
        dbgp(" %-6.6s",Objs_Reg[i]->Client() ? "Client" : Objs_Reg[i]->Server() ? "Server" : "");
        dbgpln(" %s",Objs_Reg[i]->sName());
        }
      }
    #endif
    }

  UnlockExec();

  return true;
  };

//---------------------------------------------------------------------------

flag CExecutive::DeRegister(CExecObj *pEO, flag FixingRegistrations)
  {
  LockExec();
  if (Busy/*Running*/() && !FixingRegistrations)
    {
    // in case this obj is marked for Reg but not yet registered
    flag MustDeReg=1;
    for (int i=0; i<Objs_ToReg.GetSize(); i++)
      if (Objs_ToReg[i]==pEO)
        {
        Objs_ToReg[i]=NULL;
        MustDeReg=0;
        }

    if (MustDeReg)
      {
      Objs_ToDeReg.Add(pEO);
      bRegistrationListtoChange=true;
      }
    #if dbgExecutive
    if (dbgExecReg())
      dbgpln("Add to DeReg List");
    #endif
    }
  else
    {
    #if dbgExecutive
    if (dbgExecReg())
     dbgpln("EO_DeRegister   [%i] %s", pEO->lId, pEO->sName());
    #endif

    for (int i=0; i<Objs_Reg.GetSize(); i++)
      if (Objs_Reg[i]==pEO)
        break;
    if (i<Objs_Reg.GetSize()) // if not found means that not yet registered
      Objs_Reg.RemoveAt(i);

    #if dbgExecutive
    if (dbgExecReg())
      {
      dbgpln("Registered Objects");
      for (i=0; i<Objs_Reg.GetSize(); i++)
        {
        dbgp("    %3i) %-4.4s",i,Objs_Reg[i]->SelfConnect() ? "Self": "");
        dbgp(" %-6.6s",Objs_Reg[i]->Client() ? "Client" : Objs_Reg[i]->Server() ? "Server" : "");
        dbgpln(" %s",Objs_Reg[i]->sName());
        }
      }
    #endif

    if (pEO->bMarkedForDelete)
      {
      #if dbgExecutive
      if (dbgExecReg())
        dbgpln("Executive Delete ExecObject : %s", pEO->sName());
      #endif
      delete pEO;
      }
    }

  UnlockExec();

  return true;
  };

//---------------------------------------------------------------------------

flag CExecutive::FixRegistrations()
  {
  LockExec();

  #if dbgExecutive
  if (dbgExecSeq() || dbgExecReg())
    dbgpln(">> FixRegistrations");
  #endif
  int i;
  for (i=0; i<Objs_ToReg.GetSize(); i++)
    if (Objs_ToReg[i])
      Register(Objs_ToReg[i], true);

  for (i=0; i<Objs_ToDeReg.GetSize(); i++)
    if (Objs_ToDeReg[i])
      DeRegister(Objs_ToDeReg[i], true);

  Objs_ToReg.SetSize(0);
  Objs_ToDeReg.SetSize(0);
  bRegistrationListtoChange=false;

  #if dbgExecutive
  if (dbgExecSeq() || dbgExecReg())
    dbgpln("<< FixRegistrations");
  #endif

  UnlockExec();
  return true;
  }

//---------------------------------------------------------------------------

int CExecutive::ChangeTag(pchar pOldTag, pchar pNewTag)
  {
  if (pOldTag==NULL || pNewTag==NULL || pOldTag[0]==0 || pNewTag[0]==0)
    return EOCT_NULLTAG;
  if (stricmp(pOldTag, pNewTag)==0)
    return EOCT_NOTDIFFER;
  if (strlen(pNewTag)>SCDPartTagLen-2)
    return EOCT_BADTAGLEN;
  if (TaggedObject::TestValidTagDot(pNewTag)>0)
    return EOCT_INVALIDTAG;
  if (TaggedObject::TestIfReservedTag(pNewTag))
    return EOCT_RESERVED;

  flag OK=0;
  const int Size = Objs_Reg.GetSize();
  for (int OInx=0; OInx<Size; OInx++)
    {
    int Err=Objs_Reg[OInx]->EO_QueryChangeTag(pOldTag, pNewTag);
    ASSERT(Err!=EOCT_DONE);
    if (Err==EOCT_FOUND)
      OK=1;
    else if ((Err==EOCT_NOTALLOWED) || (Err==EOCT_DRVNOTALLOWED) || (Err==EOCT_ARCNOTALLOWED) || (Err==EOCT_RESERVED) || (Err==EOCT_DUPLICATE))
      return Err;
    }

  if (!OK)
    return EOCT_NOTFOUND;

  int Err=EOCT_DONE;
  for (OInx=0; OInx<Size; OInx++)
    {
    int e=Objs_Reg[OInx]->EO_ChangeTag(pOldTag, pNewTag);
    if ((e!=EOCT_DONE) && (e!=EOCT_NOTFOUND))
      {
      LogError(pOldTag, 0, "Tag change failure in %s for '%s'->'%s'", Objs_Reg[OInx]->Name(), pOldTag, pNewTag);
      Err=EOCT_FAILED;
      }
    }

  return Err;
  }

//---------------------------------------------------------------------------

int CExecutive::DeleteTag(pchar pDelTag)
  {
  if (pDelTag==NULL || pDelTag[0]==0)
    return EODT_NULLTAG;
  if (TaggedObject::TestValidTagDot(pDelTag)>0)
    return EODT_INVALIDTAG;
  if (TaggedObject::TestIfReservedTag(pDelTag))
    return EODT_RESERVED;

  flag OK = 0;
  const int Size = Objs_Reg.GetSize();
  for (int OInx=0; OInx<Size; OInx++)
    {
    int Err = Objs_Reg[OInx]->EO_QueryDeleteTag(pDelTag);
    ASSERT(Err!=EODT_DONE);
    if (Err==EODT_FOUND)
      OK = 1;
    else if (Err==EODT_NOTALLOWED)
      return Err;
    }

  if (!OK)
    return EODT_NOTFOUND;

  int Err = EODT_DONE;
  for (OInx=0; OInx<Size; OInx++)
    {
    int e=Objs_Reg[OInx]->EO_DeleteTag(pDelTag);
    if ((e!=EODT_DONE) && (e!=EODT_NOTFOUND))
      {
      LogError(pDelTag, 0, "Tag delete failure in %s", Objs_Reg[OInx]->Name());
      Err = EODT_FAILED;
      }
    }

  return Err;
  }

//---------------------------------------------------------------------------

int CExecutive::CheckAllTags(flag NewNumTagsBad, flag NewNumStartingBad, flag Prompt/*=1*/)
  {//return number of tags changed; return -1 if tags require changing but user selected cancel
  if (!NewNumTagsBad && !NewNumStartingBad)
    return 0;
  const int PrevNumericTagsBad = TaggedObject::NumericTagsBad;
  const int PrevNumericStartingTagsBad = TaggedObject::NumericStartingTagsBad;
  TaggedObject::NumericTagsBad = NewNumTagsBad;
  TaggedObject::NumericStartingTagsBad = NewNumStartingBad;
  CSVector BadTags;
  BadTags.SetSize(32);
  int BadCnt = 0;
  RequestTagInfoRec ARqst(RQ_Tags);
  ARqst.sExcludeExecObj = pExecName_DrvMngr; //do NOT include driver tags
  ReplyTagInfoRec AInfo;
  while (RequestTagInfo(ARqst, AInfo))
    {
    if (TaggedObject::TestValidTagDot(AInfo.sData())>0)
      {
      if (BadCnt>=BadTags.GetSize())
        BadTags.SetSize(BadTags.GetSize()+16);
      BadTags[BadCnt] = AInfo.sData();
      BadCnt++;
      }
    }
  if (BadCnt>0)
    {
    BOOL ok = true;
    Strng s, sNewTag;
    if (Prompt)
      {
      sNewTag = BadTags[0]();
      TaggedObject::ValidateTagDot(sNewTag);
      s.Set("%d tag%s illegal and need%s changing.\nExample: '%s' --> '%s'\nContinue?", BadCnt,
            (BadCnt>1) ? "s are" : " is", (BadCnt>1) ? "" : "s", BadTags[0](), sNewTag());
      if (AfxMessageBox(s(), MB_YESNO|MB_ICONQUESTION)!=IDYES)
        {
        TaggedObject::NumericTagsBad = PrevNumericTagsBad;
        TaggedObject::NumericStartingTagsBad = PrevNumericStartingTagsBad;
        return -1;
        }
      }
    if (ok)
      {
      for (int i=0; i<BadCnt; i++)
        {
        sNewTag = BadTags[i]();
        TaggedObject::ValidateTagDot(sNewTag);
        ChangeTag(BadTags[i](), sNewTag());
        }
      LogNote("Executive", 0, "%d %s changed", BadCnt, (BadCnt>1) ? "tags were" : "tag was");
      }
    }
  TaggedObject::NumericTagsBad = PrevNumericTagsBad;
  TaggedObject::NumericStartingTagsBad = PrevNumericStartingTagsBad;
  return BadCnt;
  }

//---------------------------------------------------------------------------

flag CExecutive::StartThreads(byte ThrdFlag, UINT Cmd, word Extra)
  {
  for (int OInx=0; OInx<Objs_Exec.GetSize(); OInx++)
    if (Objs_Exec[OInx]->bExec & ThrdFlag)
      {
      lExecThreadsBusy++;
      PostCmd(0, Cmd, 0, Objs_Exec[OInx], Extra);
      }

  lExecThreadsBusy++;
  return 1;
  }

//---------------------------------------------------------------------------

LONG CExecutive::PostCmd(CExecObj *pSrcEO, word Cmd, LONG lParam, CExecObj *pEO, word Extra)
  {
  #if dbgExecutive
  if (dbgExecSeq())
    dbgpln("Post::%s %s > %s",
            EXM_MSGS[XCMD(Cmd)],
            pSrcEO ? pSrcEO->sName() : "App",
            pEO ? pEO->sName() : "App");
  #endif

  while (!PostThreadMessage(pEO!=NULL ? pEO->ThreadId() : ExecThreadId(),
                            WMU_EXECCOMMAND, MAKEWPARAM(Cmd,Extra), lParam))
    {
    dbgpln("PostCmd Failed %i [%08x]", Cmd, pEO);
    Sleep(10);
    }

  return 1;
  }

//---------------------------------------------------------------------------

void CExecutive::LowerPriority()
  {
  if (LastLowerPriority==0)
    pExecThread->SetThreadPriority(THREAD_PRIORITY_BELOW_NORMAL);
  LastLowerPriority=Max((DWORD)1, GetTickCount());
  };

//---------------------------------------------------------------------------

void CExecutive::DoTimer()
  {
  if (bActive)
    {
    EnterCriticalSection(&csTimerLock);

    if (bWorkDone)
      {
      bWorkDone=0;
      #if dbgExecutive
      if (dbgExecSeqTm())
        dbgpln("Work Done     Post ---- [@%12.4f]",gs_TimeSW.Time());
      #endif
      PostCmd(0, EXM_BUMPTIME);
      }
    else
      {
      bTimerExpired=1;
      #if dbgExecutive
      if (dbgExecSeqTm())
        dbgpln("Timer Expired      ---- [@%12.4f]",gs_TimeSW.Time());
      #endif
      }

    LeaveCriticalSection(&csTimerLock);
    }
  }

//---------------------------------------------------------------------------

void CExecutive::UpdateDisplays(flag ForceUpd, flag ColoursChgd)
  {
  //return ;

  if (AfxGetMainWnd())
    AfxGetMainWnd()->PostMessage(WMU_EXECUPDATEDISPLAY, 0, (long)&TimeCB());

  XFlushAppMsgQ();
//static AppDoIdleFn    pAppDoIdleFn=NULL;

  for (int EInx=0; EInx<Objs_Reg.GetSize(); EInx++)
    if (Objs_Reg[EInx]->bExec & EODisplayUpdate)
      Objs_Reg[EInx]->EO_DisplayUpdate(ForceUpd, ColoursChgd, TimeCB());
  }

//---------------------------------------------------------------------------

#if dbgKeepMsgHistory
void CExecutive::DumpMsgHistory()
  {
//  #if dbgExecutive
//  if (dbgExecSeq())
//    dbgpln("Exec::                                    %s  %18.6f  App.%s",
//           EXM_MSGS[XCMD(Cmd)], TimeCB().m_Time-3600.0*floor(gs_Exec.TimeCB().m_Time/3600.0), pEO ? pEO->Name() : "");
//  #endif
//
//  if (lMsgHistory<0)
//    for (long l=0; l<MaxMsgHistory; l++)
//      MsgHistory[l].Cmd=0;

  dbgpln("RegObjs    : -----------------------------------------------------------");
  for (int i=0; i<Objs_Reg.GetSize(); i++)
    {
    dbgp("    %3i) %-4.4s",i,Objs_Reg[i]->SelfConnect() ? "Self": "");
    dbgp(" %-6.6s",Objs_Reg[i]->Client() ? "Client" : Objs_Reg[i]->Server() ? "Server" : "");
    dbgpln(" %s",Objs_Reg[i]->sName());
    }

  dbgpln("MsgHistory : -----------------------------------------------------------");
  long l=lMsgHistory;
  do
    {
    l=(l+1)%MaxMsgHistory;
    MsgHistoryRec &M=MsgHistory[l];
    if (M.Cmd>0)
      {
      dbgpln("    %s  %18.6f  %s",EXM_MSGS[XCMD(M.Cmd)], M.Time, M.pEO ? M.pEO->Name() : "                     SysCAD");
      }
    }
  while (l!=lMsgHistory);
  dbgpln("------------------------------------------------------------------------");

  }
#endif

//---------------------------------------------------------------------------

const long SleepWaitTime = 100;

void CExecutive::DoOneExecMessage(UINT wParam, LONG lParam)
  {
  #if defined(_DEBUG)
  static dbgMemDumpBlk MDP={0,0,0,{NULL}};
  #endif

  if (LastLowerPriority>0)
    if (abs(int(GetTickCount()-LastLowerPriority))>1000)
      {
      pExecThread->SetThreadPriority(THREAD_PRIORITY_NORMAL);
      LastLowerPriority=0;
      }

  int EInx, RInx;
  UINT Cmd=XCMD(LOWORD(wParam));
  //UINT Md=XMD(LOWORD(wParam));
  CExecObj *pEO=NULL;//(CExecObj*)lParam;
  #if dbgExecutive
  if (dbgExecSeqTm())
    {
    dbgpln("Exec Time          ---- [@%12.4f] %s%s%s%s%s%s %s %08x %18.6f  App.%s",gs_TimeSW.Time(),

    bActive?"A":".",
    bStopReqd?"S":".",
    bStepReqd?"T":".",
    bPauseReqd?"P":".",
    bListRebuildReqd?"R":".",
    bListUpdateReqd?"U":".",

    EXM_MSGS[XCMD(Cmd)], wState, TimeCB().m_Time-3600.0*floor(gs_Exec.TimeCB().m_Time/3600.0), pEO ? pEO->Name() : "");
    }
  else if (dbgExecSeq())
    {
    dbgpln("Exec::                                    %s %08x %18.6f  App.%s",
           EXM_MSGS[XCMD(Cmd)], wState, TimeCB().m_Time-3600.0*floor(gs_Exec.TimeCB().m_Time/3600.0), pEO ? pEO->Name() : "");
    }
  #endif

  #if dbgKeepMsgHistory
  lMsgHistory=(lMsgHistory+1)%MaxMsgHistory;
  MsgHistory[lMsgHistory].Cmd=Cmd;
  MsgHistory[lMsgHistory].pEO=pEO;
  MsgHistory[lMsgHistory].Time=TimeCB().m_Time;
  #endif

  #if dbgExecutive
  CStopWatch SW;
  if (dbgTimingStats())
    {
    SW.Start();
    }
  #endif

  switch (Cmd)
    {
    case EXM_GOTOEDIT:
      if (Busy())
        {
        m_bSendStopEvent=true;
        bStopReqd=true;
        bPauseReqd=false;
        if (TimeCB().m_bRealTime)
          {
          while (!PostThreadMessage(ExecThreadId(), WM_TIMER, gs_Exec.iTimerId, 0))
            Sleep(10);
          //bWorkDone=0;
          //bTimerExpired=0;
          //PostCmd(0, EXM_BUMPTIME);
          }
        SetStopSleep();
        }
      else
        AfxGetMainWnd()->PostMessage(WMU_CMDSTATE, ComState_Stop, 0);
      break;
    case EXM_GOTOIDLE:
      if (Stopping())
        {
        m_GotoCmdOnceStopped=EXM_GOTOIDLE;
        }
      else if (Busy())
        {
        if (!Waiting())
          {
          m_bSendIdleEvent=true;
          bPauseReqd=true;
          }
        else
          {
          #if dbgExecutive
          if (dbgExecSeqTm())
            dbgpln("Post Idle B        ---- [@%12.4f]",gs_TimeSW.Time());
          #endif
          AfxGetMainWnd()->PostMessage(WMU_CMDSTATE, ComState_Idle, 0);
          }
        SetStopSleep();
        }
      else
        {//prestart
        for (RInx=0; RInx<Objs_Reg.GetSize(); RInx++)
          Objs_Reg[RInx]->EO_PrePreStart();

        switch (RunTermination())
          {
          case RTRM_None:
          case RTRM_ForIters:
          case RTRM_SteadyState:
            SetStopTime(dNAN);
            break;
          case RTRM_For:
          case RTRM_Until:
            SetStopTime(Time()+TimeUntilStop());
            break;
          }

        SetStepSizeMax(StepSizeMax());

        bPauseReqd=false;
        bStepReqd=false;
        m_bSendIdleEvent=true;
        PostCmd(0, EXM_STARTPAUSE);
        }
      break;
    case EXM_GOTORUN:
      if (Stopping())
        {
        m_GotoCmdOnceStopped=EXM_GOTORUN;
        }
      else if (Busy())
        {
        bPauseReqd=false;
        bStepReqd=false;
        SetStopSleep();
        }
      else
        {//prestart
        switch ((RqdRunMode)lParam)
          {
          case RRM_PBMd :
            SetGlblRunModes(SM_Direct, SM_All);
            break;
          case RRM_DynMd :
            SetGlblRunModes(SM_DynCurrent, SM_All);
            break;
          case RRM_DynMdFlow :
            SetGlblRunModes(SM_Inline, SM_All);
            break;
          case RRM_DynMdFull :
            SetGlblRunModes(SM_Buffered, SM_All);
            break;
          case RRM_Current:
            break;
          }

        for (RInx=0; RInx<Objs_Reg.GetSize(); RInx++)
          Objs_Reg[RInx]->EO_PrePreStart();

        switch (RunTermination())
          {
          case RTRM_None:
          case RTRM_ForIters:
          case RTRM_SteadyState:
            SetStopTime(dNAN);
            break;
          case RTRM_For:
          case RTRM_Until:
            SetStopTime(Time()+TimeUntilStop());
            break;
          }

        SetStepSizeMax(StepSizeMax());

        PostCmd(0, EXM_START);
        }
      break;
    case EXM_DOONESTEP:
      if (Busy())
        {
        bStepReqd=true;
        CDoOneStepInfo Info;
        Info.lParam=lParam;
        m_lWaitCompleteTime=Info.lWaitForNextMS;
        SetStopSleep();
        }
      else
        {
        CDoOneStepInfo Info;
        Info.lParam=lParam;
        switch (RunTermination())
          {
          case RTRM_None:
          case RTRM_ForIters:
          case RTRM_SteadyState:
            SetStopTime(dNAN);
            break;
          case RTRM_For:
          case RTRM_Until:
            SetStopTime(Time()+TimeUntilStop());
            break;
          }

        SetStepSizeMax(StepSizeMax());
        m_lWaitCompleteTime=(lParam & 0xFFFF);
        if (eFileWhat==EF_Null)
          PostCmd(0, (lParam&0x10000)/*HoldAdv*/ ? EXM_STEPNOADV : EXM_STEP);
        else
          LogNote("Executive", 0, "Filing operation busy");
        }
      break;
    case EXM_START:
    case EXM_STARTPAUSE:
      m_dTheStartTime=TimeCB().m_Time; // CNM I would guess that these need to be set also in STARTPAUSE
      m_dTheStopTime=dNAN;
      #if dbgExecutive
      gs_TimeSW.ReStart();
      #endif
    case EXM_STEP:
    case EXM_STEPNOADV:
      #if dbgExecutive
      if (!gs_TimeSW.Running())
        gs_TimeSW.ReStart();
      #endif
      gs_swExecute.Start();
      //m_ThisCmdMd=Md;
      //switch (Md)
      //  {
      //  case EXM_MD_Current:
      //    m_ThisCmdMd=
      //    break;
      //  case EXM_MD_PB:
      //    break;
      //  case EXM_MD_DYN:
      //    break;
      //  }

      ClrCI(1);

      XSW.Clear(true);
      XSWDsp = XSW;
      XSW.m_swRun.Start();

      if (Busy() /*|| Running()*/ || Filing())
        break;
      #if (dbgExecutive && defined(_DEBUG))
      dbgGetMemStats(MDP);
      if (dbgMemoryUse())
        dbgDumpMemBlks(MDP, "Execute Start");
      #endif

      //XSW.StepBegin();
      XSW.m_swStart.ReStart();

      for (RInx=0; RInx<Objs_Reg.GetSize(); RInx++)
        Objs_Reg[RInx]->EO_PreStart();

      ////XSW.StepBegin();
      //XSW.m_swStart.ReStart();

      wState&=(~EXF_StartError);
      wState|=EXF_Busy;
      if (fDoingPB)
        wState|=EXF_SolvingPB;
      else
        wState&=(~EXF_SolvingPB);

      bWorkDone=false;
      bTimerExpired=false;
      bPauseReqd=(Cmd==EXM_STARTPAUSE);
      bStopReqd=false;
      bStepReqd=(Cmd==EXM_STEP || Cmd==EXM_STEPNOADV);
      bHoldAdv=(Cmd==EXM_STEPNOADV);
      bRegistrationListtoChange=true;
      bListRebuildReqd=false;
      bListUpdateReqd=false;

      TimeCB().m_bHoldAdv = bHoldAdv;
      TimeCB().m_nSteps = 0;
      gs_MsgLog.SetMsgIteration(0);

      if (!SolvingPB() && !TimeCB().m_bRealTime)
        if (Valid(TimeCB().m_StopTime) && TimeCB().m_Time>=TimeCB().m_StopTime-1.0e-6)
          {
          LogWarning("Executive", 0, "Stop Time <= Current Time");
          wState&=~EXF_Busy;
          break;
          }

      UpdateDisplays();

      //ConfigureArchive();

      wState|=EXF_Starting;
      wState&=~EXF_Stopped;

//      if (TimeCB().m_bRealTime)
//        iTimerId=::SetTimer(NULL, 0, (DWORD)(TimeCB().m_StepSizeMaxMS()/TimeCB().m_dRealTimeMult), NULL);
//      else
//        iTimerId=0;

      for (RInx=0; RInx<Objs_Reg.GetSize(); RInx++)
        Objs_Reg[RInx]->EO_Starting(true);

      UpdateDisplays();
      PostCmd(0, EXM_QUERYTIMES);
      XSW.m_swStart.Stop();
      XSW.m_dStartingTime = XSW.m_swStart.Secs();
      break;

    case EXM_QUERYTIMES:
      XSW.m_swQuery.ReStart();
      if (bRegistrationListtoChange)
        FixRegistrations();
      if (QueryStartExec())
        {
        bListRebuildReqd=1;
        PostCmd(0, EXM_BUILDLISTS);
        }
      else
        {
        UpdateDisplays();
        if (SolvingPB())
          {
          pSolveTool->TermProblem();
          //RunSW.Stop();
          pSolveTool=NULL;
          }
        PostCmd(0, EXM_STOPNOIO);
        m_bSendStopEvent=true;
        }
      XSW.m_swQuery.Stop();
      break;

    case EXM_BUILDLISTS:
      XSW.m_swBuild.ReStart();
      if (bListRebuildReqd)
        {
        wState|=EXF_Building;
        UpdateDisplays();
        ClearDataLists();
        ConfigureHistorian();
        BuildDataLists();
        bListRebuildReqd=0;
        bListUpdateReqd=0;
        for (EInx=0; EInx<Objs_Exec.GetSize(); EInx++)
          Objs_Exec[EInx]->bReadAllData=1;
        wState&=(~EXF_Building);
        UpdateDisplays();
        }
      else if (bListUpdateReqd)
        {
        wState|=EXF_Updating;
        UpdateDisplays();
        UpdateDataLists();
        bListUpdateReqd=0;
        for (EInx=0; EInx<Objs_Exec.GetSize(); EInx++)
          Objs_Exec[EInx]->bReadAllData=1;
        wState&=(~EXF_Updating);
        UpdateDisplays();
        }

//      if (TimeCB().m_bRealTime && iTimerId==0)
//        iTimerId=::SetTimer(NULL, 0, (DWORD)(TimeCB().StepSizeMaxMS()/TimeCB().m_RealTimeMult), NULL);
//      else
//        iTimerId=0;

      ASSERT(lExecThreadsBusy==lExecThreadsDone);
      lExecThreadsBusy=0;
      lExecThreadsDone=0;

      for (EInx=0; EInx<Objs_Exec.GetSize(); EInx++)
        Objs_Exec[EInx]->SetEOTime(TimeCB().m_Time);
      m_dTheStartTime=TimeCB().m_Time; //reset incase start time was changed
      XSW.m_swBuild.Stop();
      PostCmd(0, EXM_DOREADALL);
      break;

    case EXM_DOREADALL:
      XSW.m_swRead.ReStart();
      StartThreads(EORead_Thread, EXM_READALL);
      for (EInx=0; EInx<Objs_Exec.GetSize(); EInx++)
        if (Objs_Exec[EInx]->bExec & EORead_Msg)
          Objs_Exec[EInx]->XReadSyncData(1);
      PostCmd(0, EXM_READALLDONE);
      break;

    case EXM_READALLDONE:
      lExecThreadsDone++;
      if ((lExecThreadsDone==lExecThreadsBusy))
        {
        XSW.m_swRead.Stop();
        #if dbgExecutive
        if (dbgExecSeqTm1())
          {
          dbgpln("Exec Time          ----  =%12.4fms        %s",1e3*XSW.m_swRead.Secs(), EXM_MSGS[XCMD(Cmd)]);
          dbgpln("                   ----  =%12.4fms #%5i LoadAddress",1e3*TagAccessBlk::m_LoadAddress.m_Time, TagAccessBlk::m_LoadAddress.m_Calls);
          dbgpln("                   ----  =%12.4fms #%5i GetDefined",1e3*TagAccessBlk::m_GetDefined.m_Time, TagAccessBlk::m_GetDefined.m_Calls);
          dbgpln("                   ----  =%12.4fms #%5i PutDefined",1e3*TagAccessBlk::m_PutDefined.m_Time, TagAccessBlk::m_PutDefined.m_Calls);
          dbgpln("                   ----  =%12.4fms #%5i ValidateDefined",1e3*TagAccessBlk::m_ValidateDefined.m_Time, TagAccessBlk::m_ValidateDefined.m_Calls);
          TagAccessBlk::ResetTimers();
          }
        #endif
        PostCmd(0, EXM_DOWRITEALL);
        }
      break;

    case EXM_DOREAD:

      if (m_ReadWriteAllReqd)
        {
        m_ReadWriteAllReqd=false;
        PostCmd(0, EXM_DOREADALL);
        break;
        }
      XSW.m_swRead.ReStart();
      StartThreads(EORead_Thread, EXM_READ);
      for (EInx=0; EInx<Objs_Exec.GetSize(); EInx++)
        if (Objs_Exec[EInx]->bExec & EORead_Msg)
          Objs_Exec[EInx]->XReadSyncData(lUpdateTagDataIterCount>0);
      lUpdateTagDataIterCount=Range(0L, lUpdateTagDataIterCount-1L, 10L);
      PostCmd(0, EXM_READDONE);
      break;

    case EXM_READDONE:
      lExecThreadsDone++;
      if ((lExecThreadsDone==lExecThreadsBusy))
        {
        XSW.m_swRead.Stop();
        #if dbgExecutive
        if (dbgExecSeqTm1())
          {
          dbgpln("Exec Time          ----  =%12.4fms        %s",1e3*XSW.m_swRead.Secs(), EXM_MSGS[XCMD(Cmd)]);
          dbgpln("                   ----  =%12.4fms #%5i LoadAddress",1e3*TagAccessBlk::m_LoadAddress.m_Time, TagAccessBlk::m_LoadAddress.m_Calls);
          dbgpln("                   ----  =%12.4fms #%5i GetDefined",1e3*TagAccessBlk::m_GetDefined.m_Time, TagAccessBlk::m_GetDefined.m_Calls);
          dbgpln("                   ----  =%12.4fms #%5i PutDefined",1e3*TagAccessBlk::m_PutDefined.m_Time, TagAccessBlk::m_PutDefined.m_Calls);
          dbgpln("                   ----  =%12.4fms #%5i ValidateDefined",1e3*TagAccessBlk::m_ValidateDefined.m_Time, TagAccessBlk::m_ValidateDefined.m_Calls);
          TagAccessBlk::ResetTimers();
          }
        #endif
        PostCmd(0, EXM_DOWRITE);
        }
      break;

    case EXM_DOWRITEALL:
      XSW.m_swWrite.ReStart();
      StartThreads(EOWrite_Thread, EXM_WRITEALL);
      for (EInx=0; EInx<Objs_Exec.GetSize(); EInx++)
        {
        if (Objs_Exec[EInx]->bExec & EOWrite_Msg)
          Objs_Exec[EInx]->XWriteSyncData(true);
        }
      PostCmd(0, EXM_WRITEDONE);
      break;

    case EXM_DOWRITE:
      XSW.m_swWrite.ReStart();
      StartThreads(EOWrite_Thread, EXM_WRITE);
      for (EInx=0; EInx<Objs_Exec.GetSize(); EInx++)
        {
        if (Objs_Exec[EInx]->bExec & EOWrite_Msg)
          Objs_Exec[EInx]->XWriteSyncData(false);
        }
      PostCmd(0, EXM_WRITEDONE);
      break;

    case EXM_WRITEDONE:
      lExecThreadsDone++;
      if (lExecThreadsDone==lExecThreadsBusy)
        {
        XSW.m_swWrite.Stop();
        #if dbgExecutive
        if (dbgExecSeqTm1())
          {
          dbgpln("Exec Time          ----  =%12.4fms        %s",1e3*XSW.m_swWrite.Secs(), EXM_MSGS[XCMD(Cmd)]);
          dbgpln("                   ----  =%12.4fms #%5i LoadAddress",1e3*TagAccessBlk::m_LoadAddress.m_Time, TagAccessBlk::m_LoadAddress.m_Calls);
          dbgpln("                   ----  =%12.4fms #%5i GetDefined",1e3*TagAccessBlk::m_GetDefined.m_Time, TagAccessBlk::m_GetDefined.m_Calls);
          dbgpln("                   ----  =%12.4fms #%5i PutDefined",1e3*TagAccessBlk::m_PutDefined.m_Time, TagAccessBlk::m_PutDefined.m_Calls);
          dbgpln("                   ----  =%12.4fms #%5i ValidateDefined",1e3*TagAccessBlk::m_ValidateDefined.m_Time, TagAccessBlk::m_ValidateDefined.m_Calls);
          TagAccessBlk::ResetTimers();
          }
        #endif

        if (Starting())
          {
          TaggedObject::SetRunning(true);
          for (RInx=0; RInx<Objs_Reg.GetSize(); RInx++)
            Objs_Reg[RInx]->EO_Starting(false);
          }
          // Start Timer if neccessary

        static DWORD sm_RealTmTimerTicks=0;
        if (TimeCB().m_bRealTime)
          {
          DWORD RqdRealTmTimerTicks=(DWORD)(TimeCB().StepSizeMaxMS()/TimeCB().m_RealTimeMult);
          if (iTimerId==0 || RqdRealTmTimerTicks!=sm_RealTmTimerTicks)
            {
            sm_RealTmTimerTicks=RqdRealTmTimerTicks;
            if (iTimerId!=0)
              {
              ::KillTimer(NULL, iTimerId);
              iTimerId=0;
              }
            if (iTimerId==0)
              iTimerId=::SetTimer(NULL, 0, sm_RealTmTimerTicks, NULL);
            }
          }
        else
          {
          if (iTimerId!=0)
            {
            ::KillTimer(NULL, iTimerId);
            iTimerId=0;
            }
          }

        UpdateDisplays();


        if (!TimeCB().m_bRealTime)
          if (Valid(TimeCB().m_StopTime) && TimeCB().m_Time>=TimeCB().m_StopTime-1.0e-6)
            GotoIdleMode();
        PostCmd(0, EXM_STEPDONE);
        }
      break;

    case EXM_STEPDONE:
      //XSW.Write.ReStart();
      for (EInx=0; EInx<Objs_Exec.GetSize(); EInx++)
        {
        Objs_Exec[EInx]->EO_StepDone();
        }
      PostCmd(0, EXM_DOTEST);
      break;

    case EXM_DOTEST:
      {
      wState|=EXF_BetweenSteps;
      if (bDoAppActivate)
        DoAppActivate();
      if (eRqdState!=MSA_Null)
        {
        DoSetModelState(eRqdState, sRqdTags);
        eRqdState=MSA_Null;
        }
      if (bDoGlblResultValidity && !bStopReqd)
        DoGlblResultValidity();

      #if (dbgExecutive && defined(_DEBUG))

//      if (!_CrtCheckMemory())
//        {
//        _asm int 3;
//        };

      if (dbgMemoryUse())
        dbgDumpMemBlks(MDP, "Execute Step");
      dbgGetMemStats(MDP);
      #endif

      if (Waiting())
        {
        }
      else
        {
        XSW.StepEnd();
        XSWDsp = XSW;
        XSW.Clear(false);
        XSW.StepBegin();
        }

      wState&=(~EXF_BetweenSteps);

      wState&=(~EXF_Starting);
      wState&=(~EXF_StartError);
      wState|=EXF_Running;

      flag DoEndPause=!bPauseReqd;

      m_dwLoopCount++;
      if (bPauseReqd)
        {
        if (!Waiting())
          {
          XSW.m_swStep.Stop();
          XSW.m_swRun.Stop();
          wState|=EXF_Waiting;
          }

        if (Waiting() && !m_bDidPauseWork)
          {
          if (m_lWaitCompleteCount>0)
            m_lWaitCompleteCount--;

          if (m_lWaitCompleteCount<=0)
            {
            #if dbgExecutive
            if (dbgExecSeq())
              dbgpln("---- Start Pause : COMPLETE  [@%12.4f]",gs_TimeSW.Time());
            #endif
            ::WaitForSingleObject(m_hAppIdleDone, 1000);
            m_bDidPauseWork=true;
            wState|=EXF_Paused;
            UpdateDisplays();
            if (SolvingPB())
              {
              pSolveTool->BeginPause();
              }
            for (RInx=0; RInx<Objs_Reg.GetSize(); RInx++)
              Objs_Reg[RInx]->EO_BeginPause(TimeCB());
            if (m_bSendIdleEvent)
              {
              #if dbgExecutive
              if (dbgExecSeqTm())
                dbgpln("Post Idle A        ---- [@%12.4f]",gs_TimeSW.Time());
              #endif
              m_bSendIdleEvent=false;
              AfxGetMainWnd()->PostMessage(WMU_CMDSTATE, ComState_Idle, 0); //let script cmd mngr know cmd is complete
              }
            }
          else
            {
            #if dbgExecutive
            if (dbgExecSeq())
              dbgpln("---- Start Pause : %2i  [@%12.4f]",m_lWaitCompleteCount,gs_TimeSW.Time());
            #endif
            }
          }
        #if dbgExecutive
        if (dbgLongIdleLoop())
          ::WaitForSingleObject(m_hStopSleep, SleepWaitTime*100); // 10 secs
        else
          ::WaitForSingleObject(m_hStopSleep, SleepWaitTime);
        #else
        ::WaitForSingleObject(m_hStopSleep, SleepWaitTime);
        #endif
        }
      else
        {
        ::WaitForSingleObject(m_hAppIdleDone, 1000);
        }


      UINT NextCmd;
      if (bRegistrationListtoChange)
        NextCmd=EXM_QUERYTIMES;
      else if (bListRebuildReqd || bListUpdateReqd)
        NextCmd=EXM_BUILDLISTS;
      else if (bStopReqd)
        {
        DoEndPause=true;
        NextCmd=EXM_STOP;
        }
      else if (bStepReqd)
        {
        //nPauseCount=10;
        #if dbgExecutive
        if (dbgExecSeq())
          dbgpln("---- Start Step  : %2i  [%6i]",m_lWaitCompleteCount,m_lWaitCompleteTime);
        #endif
        m_lWaitCompleteCount=m_lWaitCompleteTime/SleepWaitTime;
        m_lWaitCompleteTime=0;
        bStepReqd=false;
        DoEndPause=Waiting();
        if (!Waiting())
          bStopReqd=true;
        NextCmd=EXM_DOEXEC;
        }
      else if (Waiting())
        {
        if (m_lPauseUpdateCount++>=5) // effectively only update every 0.5 secs
          {
          m_lPauseUpdateCount=0;
          NextCmd=EXM_DOREAD;
          }
        else
          NextCmd=EXM_DOTEST;
        }
      else
        NextCmd=EXM_DOEXEC;

      if (DoEndPause && Waiting())
        {
        wState&=(~EXF_Waiting);
        bStepReqd=false;
        if (m_bDidPauseWork)
          {
          #if dbgExecutive
          if (dbgExecSeq())
            dbgpln("---- End   Pause : COMPLETE");
          #endif
          m_bDidPauseWork=false;
          wState&=(~EXF_Paused);
          if (1)
            {
            UpdateDisplays();
            if (SolvingPB())
              {
              pSolveTool->EndPause();
              }
            for (RInx=0; RInx<Objs_Reg.GetSize(); RInx++)
              Objs_Reg[RInx]->EO_EndPause(TimeCB());
            }
          XSW.m_swRun.Start();
          XSW.m_swStep.Start();
          }
        else
          {
          #if dbgExecutive
          if (dbgExecSeq())
            dbgpln("---- End   Pause : Partial");
          #endif
          }
        }

      PostCmd(0, NextCmd);

      break;
      }

    case EXM_DOEXEC:
      TagAccessBlk::ResetTimers();
      gs_swFlwSolve.Start();

      XSW.m_swExec.ReStart();

      #if dbgExecutive
      if (dbgTimeDiffOn())
        dbgtime(3);
      else if (dbgTimeOn())
        dbgtime(1);
      else
        dbgtime(0);
      if (dbgExecSeqSimple())
        dbgpln("Step Exec ------------------------------------------------------------------- %10.2f", gs_Exec.TimeCB().m_Time);
      #endif

      CDbgMngr::ChkFileSize(dbgSmallDBGFile() ? 5000:30000, 0); // clear debug file if too big

      if (!TimeCB().m_bRealTime)
        {
        if (Valid(TimeCB().m_StepSizeNxt))
          TimeCB().m_StepSize = Min(TimeCB().m_StepSizeNxt, TimeCB().m_StepSizeMax);
        else
          TimeCB().m_StepSize = TimeCB().m_StepSizeMax;
        if (Valid(TimeCB().m_StopTime))
          TimeCB().m_StepSize=Min(TimeCB().m_StepSize, TimeCB().m_StopTime-TimeCB().m_Time);
        TimeCB().m_StepSize=Max(1.0e-3, TimeCB().m_StepSize);
        TimeCB().m_StepSizeNxt=dNAN;
        }

      m_Seq.PlayTheList(m_Seq.SequenceNo());

      if (SolvingPB())
        {
        EqnSlv.BeginStep(EqnCB());
        pSolveTool->StepProblemBegin();
        }
      else
        TimeCB().m_nSteps++;

      StartThreads(EOExec_Thread, EXM_EXEC);
      for (EInx=0; EInx<Objs_Exec.GetSize(); EInx++)
        if (Objs_Exec[EInx]->bExec & EOExec_Msg)
          {
          ThrdRet[EInx].Clear();
          Objs_Exec[EInx]->EO_Execute(TimeCB(), ThrdRet[EInx]);
          }
      m_Seq.IncSequenceNo();
      gs_MsgLog.IncMsgIteration();
      gs_EventLog.IncIteration();

      PostCmd(0, EXM_EXECDONE);

      break;

    case EXM_EXECDONE:
      lExecThreadsDone++;
      if (lExecThreadsDone==lExecThreadsBusy)
        PostCmd(0, EXM_EXECALLDONE);
      break;

    case EXM_EXECALLDONE:
      gs_swFlwSolve.Stop();

      XSW.m_swExec.Stop();
      #if dbgExecutive
      if (dbgExecSeqTm1())
        {
        dbgpln("Exec Time          ----  =%12.4fms        %s",1e3*XSW.m_swExec.Secs(), EXM_MSGS[XCMD(Cmd)]);
        dbgpln("                   ----  =%12.4fms #%5i LoadAddress",1e3*TagAccessBlk::m_LoadAddress.m_Time, TagAccessBlk::m_LoadAddress.m_Calls);
        dbgpln("                   ----  =%12.4fms #%5i GetDefined",1e3*TagAccessBlk::m_GetDefined.m_Time, TagAccessBlk::m_GetDefined.m_Calls);
        dbgpln("                   ----  =%12.4fms #%5i PutDefined",1e3*TagAccessBlk::m_PutDefined.m_Time, TagAccessBlk::m_PutDefined.m_Calls);
        dbgpln("                   ----  =%12.4fms #%5i ValidateDefined",1e3*TagAccessBlk::m_ValidateDefined.m_Time, TagAccessBlk::m_ValidateDefined.m_Calls);
        TagAccessBlk::ResetTimers();
        }
      #endif

      XSW.m_swExecDn.ReStart();

      if (SolvingPB())
        {
        EqnSlvCtrlBlk &ECB=EqnCB();
        EqnSlv.EndStep(ECB);
        pSolveTool->StepProblemEnd();//*this, TimeCB().m_EqnCB);
        if (!ECB.ConfigOK())
          bStopReqd=true;
        else if (ECB.Converged() && (ECB.NConvergedIters()>=ECB.Cfg.iRqdCnvrgdIters) ||
                 (ECB.NIters()>=ECB.Cfg.iMaxIters))
          {
          if (EqnCB().Cfg.fIdleWhenDone)
            bPauseReqd=true;
          else
            bStopReqd=true;
          }

        ECB.BumpNIters();
        }

      #if dbgThreadTimes
      if (1)
        {
        FILETIME tCreat, tExit, tKernel, tUser;
        GetThreadTimes(GetCurrentThread(), &tCreat, &tExit, &tKernel, &tUser);
        LARGE_INTEGER lKernel = {tKernel.dwLowDateTime, tKernel.dwHighDateTime };
        LARGE_INTEGER lUser   = {tUser.dwLowDateTime, tUser.dwHighDateTime };
        double ThisThreadTimes=(lKernel.QuadPart+lUser.QuadPart)*1.0e-7; // 100's of nanosecs ticks
        static double PrvThisThreadTimes=0;

        dbgp(" %10.6f",ThisThreadTimes-PrvThisThreadTimes);
        PrvThisThreadTimes=ThisThreadTimes;
        for (EInx=0; EInx<Objs_Exec.GetSize(); EInx++)
          {
          dbgp(" %10.6f",ThreadTimes[EInx]-PrvThreadTimes[EInx]);
          PrvThreadTimes[EInx]=ThreadTimes[EInx];
          }
        dbgpln("");
        }
      #endif

      if (1)
        {
        UINT ComSt;
        if (SolvingPB())
          ComSt=ComState_StepProbal;
        else
          ComSt=ComState_StepDynamic;
        AfxGetMainWnd()->PostMessage(WMU_CMDSTATE, ComSt, 0);
        }

      gs_swExecute.Stop();
      if (TimeCB().m_bRealTime)
        {
        EnterCriticalSection(&csTimerLock);
        if (bTimerExpired)
          {//running too slowly...
          bTimerExpired=false;
          SetCI(1); //perhaps display "how" slowly as part of message?
          #if dbgExecutive
          if (dbgExecSeqTm())
            dbgpln("Timer Expired Post ---- [@%12.4f]",gs_TimeSW.Time());
          #endif
          PostCmd(0, EXM_BUMPTIME);
          }
        else
          {
          bWorkDone=1;
          ClrCI(1);
          #if dbgExecutive
          if (dbgExecSeqTm())
            dbgpln("Work Done          ---- [@%12.4f]",gs_TimeSW.Time());
          #endif
          }
        LeaveCriticalSection(&csTimerLock);
        }
      else
        PostCmd(0, EXM_BUMPTIME);
      XSW.m_swExecDn.Stop();
      break;

    case EXM_BUMPTIME:
      gs_swExecute.Start();
      if (TimeCB().m_bRealTime)
        {
        int GotTheTime=0;
        for (EInx=0; EInx<Objs_Exec.GetSize(); EInx++)
          if (Objs_Exec[EInx]->bExec & (EOExec_Thread | EOExec_Msg))
            if (Valid(ThrdRet[EInx].m_Time) && Objs_Exec[EInx]->Client())
              {
              TimeCB().m_Time = ThrdRet[EInx].m_Time;
              GotTheTime++;
              #if dbgExecutive
              if (dbgExecSeq())
                dbgpln("Bump Time RT       ---- [@%12.4f] T:%8.2f %s",gs_TimeSW.Time(), ThrdRet[EInx].m_Time, Objs_Exec[EInx]->Name());
              #endif
              }
        if (GotTheTime==0 && TimeCB().m_bSyncWithClock)
          TimeCB().m_Time=GetCurrentTime_1970();
        else
          TimeCB().m_Time+=TimeCB().m_StepSize;
        }
      else //if (!TimeCB().m_bRealTime)
        {
        TimeCB().m_Time+=TimeCB().m_StepSize;
        for (EInx=0; EInx<Objs_Exec.GetSize(); EInx++)
          if (Objs_Exec[EInx]->bExec & (EOExec_Thread | EOExec_Msg))
            {
            if (Valid(ThrdRet[EInx].m_Time))
              TimeCB().m_Time = Min(TimeCB().m_Time, ThrdRet[EInx].m_Time);
            if (Valid(ThrdRet[EInx].m_StepSizeNxt))
              TimeCB().m_StepSizeNxt = (Valid(TimeCB().m_StepSizeNxt) ? Min(TimeCB().m_StepSizeNxt, ThrdRet[EInx].m_StepSizeNxt) : ThrdRet[EInx].m_StepSizeNxt);
            #if dbgExecutive
            if (dbgExecSeq())
              dbgpln("Bump Time          ---- [@%12.4f] T:%8.2f  dTNxt:%8.2f %s",
                     gs_TimeSW.Time(), Valid(ThrdRet[EInx].m_Time) ? ThrdRet[EInx].m_Time : -1.0,
                     Valid(ThrdRet[EInx].m_StepSizeNxt) ? ThrdRet[EInx].m_StepSizeNxt : -1.0,
                     Objs_Exec[EInx]->Name());
            #endif
            }
        #if dbgExecutive
        if (1)
          {
          static double PrevTm=0;
          static double PrevPt=0;
          static double RunSP=0;
          if (dbgExecSeqTm() && PrevTm!=0.0)
            {
            double D=gs_TimeSW.Time()-PrevTm;
            double PT=GetProcTime()-PrevPt;
            double RS=TimeCB().m_StepSize/GTZ(D);
            double PTF=PT/GTZ(D);
            RunSP=0.75*RunSP+0.25*RS;
            dbgpln("IterSpeed    ---- DT:%12.4f IRS:%12.4f%% ARS:%12.4f%% ProcT:%12.4f PTF:%12.4f%%", D, RS*100, RunSP*100, PT, PTF*100);
            }
          PrevTm=gs_TimeSW.Time();
          PrevPt=GetProcTime();
          }
        #endif
        if (Valid(TimeCB().m_StopTime) && TimeCB().m_Time>=TimeCB().m_StopTime-1.0e-6)
          GotoIdleMode();
        }

      for (EInx=0; EInx<Objs_Exec.GetSize(); EInx++)
        Objs_Exec[EInx]->SetEOTime(TimeCB().m_Time);

      PostCmd(0, EXM_DOREAD);
      break;

    case EXM_STOP:
    case EXM_STOPNOIO:

      XSW.m_swStop.ReStart();
      wState&=(~EXF_Running);
      wState|=EXF_Stopping;
      UpdateDisplays();
      for (RInx=0; RInx<Objs_Reg.GetSize(); RInx++)
        Objs_Reg[RInx]->EO_Stopping(true);
      TaggedObject::SetRunning(false);

      for (EInx=0; EInx<Objs_Exec.GetSize(); EInx++)
        Objs_Exec[EInx]->EO_Stop(TimeCB());

      m_dTheStopTime=TimeCB().m_Time;

      // Bump Time so that Next 'Read/Write' do not set data with the same time.
      //The "problem" with this is that when stopped gs_Exec.Time() does not return the correct stop time, to "fix" use gs_Exec.TheStopTime()
      if (!TimeCB().m_bRealTime)
        TimeCB().m_Time+=TimeCB().m_StepSize; //what would happen if we don't increment this?

      bStopReqd=false;
      bStepReqd=false;
      //bHoldAdv =false;

      //wState|=EXF_Stopped;
      if (iTimerId)
        {
        ::KillTimer(NULL, iTimerId);
        iTimerId=0;
        }

      UpdateDisplays();

      if (SolvingPB())
        {
        wState&=(~EXF_SolvingPB);
        if (pSolveTool)
          pSolveTool->TermProblem();
        pSolveTool=NULL;
        }

      if (bDoAppActivate)
        DoAppActivate();
      if (eRqdState!=MSA_Null)
        {
        DoSetModelState(eRqdState, sRqdTags);
        eRqdState=MSA_Null;
        }
      if (bDoGlblResultValidity)
        DoGlblResultValidity();

      //wState&=(~(EXF_Stopping));
      //wState&=(~(EXF_Busy));
      //wState|=EXF_Stopped;

      if (Cmd==EXM_STOP)
        {
        StartThreads(EORead_Thread, EXM_LASTREAD);
        for (EInx=0; EInx<Objs_Exec.GetSize(); EInx++)
          if (Objs_Exec[EInx]->bExec & EORead_Msg)
            Objs_Exec[EInx]->XReadSyncData(true);
        PostCmd(0, EXM_LASTREADDONE);
        }
      else
        PostCmd(0, EXM_STOPPED);
      break;

    case EXM_LASTREADDONE:
      lExecThreadsDone++;
      if ((lExecThreadsDone==lExecThreadsBusy))
        PostCmd(0, EXM_DOLASTWRITE);
      break;

    case EXM_DOLASTWRITE:
      XSW.m_swWrite.ReStart();
      StartThreads(EOWrite_Thread, EXM_LASTWRITE);
      for (EInx=0; EInx<Objs_Exec.GetSize(); EInx++)
        {
        if (Objs_Exec[EInx]->bExec & EOWrite_Msg)
          Objs_Exec[EInx]->XWriteSyncData(false);
        }
      PostCmd(0, EXM_LASTWRITEDONE);
      break;

    case EXM_LASTWRITEDONE:
      lExecThreadsDone++;
      if (lExecThreadsDone==lExecThreadsBusy)
        PostCmd(0, EXM_STOPPED);
      break;

    case EXM_STOPPED:

      wState&=(~(EXF_Stopping));
      wState&=(~(EXF_Busy));
      wState|=EXF_Stopped;

      for (RInx=0; RInx<Objs_Reg.GetSize(); RInx++)
        Objs_Reg[RInx]->EO_Stopping(false);
      //TimeCB().m_bStopping=0;

      UpdateDisplays();

      AfxGetMainWnd()->PostMessage(WMU_UPDATEMAINWND, SUB_UPDMAIN_ACTIVATECMDS, 0); //after stopping ensure active/top window has commands enabled
      if (m_bSendStopEvent)
        {
        m_bSendStopEvent=false;
        AfxGetMainWnd()->PostMessage(WMU_CMDSTATE, ComState_Stop, 0); //let script cmd mngr know cmd is complete
        }

      //UpdateDisplays();

//      LogSeparator("Stopped",0,"=======================================");
      #if (dbgExecutive && defined(_DEBUG))
      if (dbgMemoryUse())
        dbgDumpMemBlks(MDP, "Execute Done");
      dbgGetMemStats(MDP);
      #endif
      XSW.m_bLastStep=1;
      XSW.StepEnd();
      XSW.m_swStop.Stop();
      XSW.m_swRun.Stop();
      XSWDsp = XSW;
      XSW.Clear(true);
      gs_swExecute.Stop();
      #if dbgExecutive
      gs_TimeSW.Stop();
      #endif
      if (m_GotoCmdOnceStopped!=EXM_NULL)
        {
        PostCmd(0, m_GotoCmdOnceStopped);
        m_GotoCmdOnceStopped=EXM_NULL;
        }
      break;

    case EXM_QUIT :
      ::KillTimer(NULL, iTimerId);
      break;
//    case EXM_FILING:
//      switch (eFileWhat)
//        {
//        case EF_LoadDBReqd:
//          DoLoadDBase(SaveLoadRoot(), SaveLoadLabel());
//          break;
//        case EF_SaveDBReqd:
//          DoSaveDBase(SaveLoadRoot(), SaveLoadLabel(), SaveLoadOldPrjFiles());
//          break;
//        case EF_LoadSnapReqd:
//          DoLoadSnapShotScenario(true, SaveLoadRoot(), SaveLoadSnapShotScenarioName());
//          break;
//        case EF_SaveSnapReqd:
//          DoSaveSnapShotScenario(true, SaveLoadRoot(), SaveLoadSnapShotScenarioName());
//          break;
//        case EF_LoadScenarioReqd:
//          DoLoadSnapShotScenario(false, SaveLoadRoot(), SaveLoadSnapShotScenarioName());
//          break;
//        case EF_SaveScenarioReqd:
//          DoSaveSnapShotScenario(false, SaveLoadRoot(), SaveLoadSnapShotScenarioName());
//          break;
//        }
//      eFileWhat=EF_Null;
//      break;
    }
  #if dbgExecutive
  if (dbgTimingStats())
    dbgpln("ExecLap %s   [= %10.6f] %s",EXM_MSGS[XCMD(Cmd)], SW.Lap(), pEO ? pEO->Name() : "EXEC");
  #endif
  }

//---------------------------------------------------------------------------

double CExecutive::QueryCurrentTime()
  {
  if (bRegistrationListtoChange)
    FixRegistrations();
  double ATimeRqd=dNAN, AdTimeRqd=dNAN;
  for (int EInx=0; EInx<Objs_Reg.GetSize(); EInx++)
    {
    double TimeRqd=dNAN, dTimeRqd=dNAN;
    if (Objs_Reg[EInx]->EO_QueryTime(TimeCB(), TimeRqd, dTimeRqd))
      {
      if (!TimeCB().m_bSyncWithClock)
        {
        if (Valid(TimeRqd))
          ATimeRqd=Valid(ATimeRqd) ? Max(ATimeRqd, TimeRqd) : TimeRqd;
        }
      }
    }
  if (Valid(ATimeRqd))
    return Max(Time(), ATimeRqd);
  return Time();
  }

//---------------------------------------------------------------------------

flag CExecutive::QueryStartExec()
  {
  int EInx;

  // Exec only those that are flagged
  Objs_Exec.SetSize(0, 16);
  Objs_Search.SetSize(0, 16);

  for (int RInx=0; RInx<Objs_Reg.GetSize(); RInx++)
    {
    CExecObj *pEO=Objs_Reg[RInx];
    pEO->iObjExecIndex=-1;
    if (pEO->bExec>0)
      {
      int EInx;
      for (EInx=0; EInx<Objs_Exec.GetSize(); EInx++)
        if (pEO->bExec > Objs_Exec[EInx]->bExec)
          break;
      Objs_Exec.InsertAt(EInx, pEO);
      Objs_Search.Add(pEO);
      }
    }
  for (EInx=0; EInx<Objs_Exec.GetSize(); EInx++)
    Objs_Exec[EInx]->iObjExecIndex=EInx;

  #if dbgExecutive
  if (dbgExecReg())
    {
    dbgp("Exec_Objs   : ");
    for (EInx=0; EInx<Objs_Exec.GetSize(); EInx++)
      dbgp("%s, ", Objs_Exec[EInx]->sName());
    dbgpln("");

    dbgp("Search_Objs : ");
    for (int SInx=0; SInx<Objs_Search.GetSize(); SInx++)
      dbgp("%s, ", Objs_Search[SInx]->sName());
    dbgpln("");
    }
  #endif

  const int MaxBad=8;
  CExecObj *pBad[MaxBad];
  int nBad=0;
  if (TimeCB().m_bRealTime)
    {
    CXM_TimeControl & TCB=TimeCB();
    if (TCB.m_bSyncWithClock)
      TCB.m_Time=GetCurrentTime_1970();
    lLastTickCount=GetTickCount();
    long RqdTimeMax=(long)(TCB.StepSizeMaxMS()/TCB.m_RealTimeMult);
    if (lLastTickCount>RqdTimeMax)
      lLastTickCount-=RqdTimeMax;
    TCB.m_StepSize=TCB.m_StepSizeMax;
    }

  flag OK=1;
  double ATimeRqd=dNAN;
  double AdTimeRqd=SolvingPB() ? 1.0 : TimeCB().m_StepSize;//dNAN; // Use Last Step Size
  for (EInx=0; EInx<Objs_Exec.GetSize(); EInx++)
    {
    double TimeRqd=dNAN, dTimeRqd=dNAN;
    if (Objs_Exec[EInx]->EO_QueryTime(TimeCB(), TimeRqd, dTimeRqd))
      {
      if (!TimeCB().m_bSyncWithClock)
        {
        if (Valid(TimeRqd))
          ATimeRqd=Valid(ATimeRqd) ? Max(ATimeRqd, TimeRqd) : TimeRqd;
        if (Valid(dTimeRqd))
          AdTimeRqd=Valid(AdTimeRqd) ? Max(AdTimeRqd, dTimeRqd) : dTimeRqd;
        }
      }
    else
      {
      OK=0;
      if (nBad<MaxBad)
        pBad[nBad++]=Objs_Exec[EInx];
      }
    }

  if (!OK)
    {
    wState|=EXF_StartError;
    char buff[512];
    strcpy(buff, "Startup Failure:Bad Times");
    while (nBad>0)
      {
      strcat(buff, "\n");
      strcat(buff, pBad[--nBad]->sName());
      }
    //AfxMessageBox(buff, MB_ICONEXCLAMATION|MB_OK);
    LogError("SysCAD", LF_Exclamation, buff);
    return false;
    }

  if (TimeCB().m_bRealTime)
    {
    //if (Valid(ATimeRqd))
    //  TimeCB().m_Time=ATimeRqd;
    //if (Valid(AdTimeRqd))
    //  TimeCB().m_dTime=AdTimeRqd;
    }
  else
    {
    if (Valid(ATimeRqd))
      TimeCB().m_Time=ATimeRqd;
    if (Valid(AdTimeRqd))
      TimeCB().m_StepSize=AdTimeRqd;
    else
      {
      //AfxMessageBox("Startup Failure\nNo TimeBase Generator", MB_ICONEXCLAMATION|MB_OK);
      wState|=EXF_StartError;
      LogError("SysCAD", LF_Exclamation, "Startup Failure\nNo TimeBase Generator");
      return false;
      }
    }

  for (EInx=0; EInx<Objs_Exec.GetSize(); EInx++)
    {
    if (!Objs_Exec[EInx]->EO_Start(TimeCB()))
      {
      OK=0;
      if (nBad<MaxBad)
        pBad[nBad++]=Objs_Exec[EInx];
      }
    }

  if (!OK)
    {
    /*char buff[512];
    strcpy(buff, "Startup Failure - Errors in :");
    while (nBad>0)
      {
      strcat(buff, "\n");
      strcat(buff, pBad[--nBad]->sName());
      }
    LogError("SysCAD", LF_Exclamation, buff);*/
    wState|=EXF_StartError;
    AfxGetMainWnd()->PostMessage(WMU_TAGACTION, SUB_TAGACTION_ACCREFRESHSAVE, (LPARAM)0);
    LogError("SysCAD", LF_Exclamation, "Startup Failure!  (see preceding messages)");
    return false;
    }

  if (SolvingPB())
    {
    EqnCB().Init();
    pSolveTool->InitProblem();
    //RunSW.ReStart();
    }

  if (OK)
    {
    //NB: After a project load m_StepSize is not correct so should NOT be used!!!
    //TimeCB().m_StepSizeNxt=TimeCB().m_StepSize; // dTime is Taken from dTimeNxt
    }

  return OK;
  }

//---------------------------------------------------------------------------

void CExecutive::ClearDataLists()
  {
  for (int RInx=0; RInx<Objs_Reg.GetSize(); RInx++)
    Objs_Reg[RInx]->ClearDataLists();

  }

//---------------------------------------------------------------------------

int CExecutive::CanClose(Strng_List & Problems)
  {
  //int Ret=Running() || TimeCB().m_bSolvingPB ? EO_CanClose_No : EO_CanClose_Yes;
  int Ret=Busy() ? EO_CanClose_No : EO_CanClose_Yes;
  for (int RInx=0; RInx<Objs_Reg.GetSize(); RInx++)
    Ret=Max(Ret, Objs_Reg[RInx]->EO_CanClose(Problems));
  if (Ret==EO_CanClose_Yes)
    gs_EventLog.CloseFile();
  return Ret;
  }

//---------------------------------------------------------------------------

void CExecutive::DoConfigureArchive()
  {
  CXM_Route HRoute;
  flag GotArchive=XFindObject(NULL, (char*)pExecName_ArcMngr, HRoute);
  if (GotArchive)
    {
    //CXMsgLst XM;

    for (int EInx=0; EInx<Objs_Reg.GetSize(); EInx++)
      {
      CExecObj *pEO=Objs_Reg[EInx];
      CXMsgLst XM;
      flag Start=1;
      long LastIndex=0;
      for (;;)
        {
        XM.Clear();
        XM.PackMsg(new CXM_ReadIndexedData(Start, 0, LastIndex));
        pEO->EO_QueryArchiveCfg(XM);
        Start=0;
        #if dbgExecutive
        if (dbgMsgs())
          XM.dbgDump((flag)dbgFullMsgs(), pEO->sName());
        #endif
        if (XM.MsgAvail(XM_KeepArchive))
          {
          CXM_Route HRouteCopy(HRoute);
          HRouteCopy=HRoute;
          XSendMessage(NULL, XM, HRoute);
          CXM_Long *pLong = XM.Long();
          LastIndex=pLong->lValue;
          }
        else
          break;
        }
      }

    }
  }

//---------------------------------------------------------------------------

void CExecutive::DoConfigureHistorian()
  {
  CXM_Route HRoute;
  flag GotHistorian=XFindObject(NULL, (char*)pExecName_Historian, HRoute);
  if (GotHistorian)
    {
    CXMsgLst XM;

    for (int EInx=0; EInx<Objs_Exec.GetSize(); EInx++)
      {
      CExecObj *pEO=Objs_Exec[EInx];
      CXMsgLst XM;
      flag Start=1;
      long LastIndex=0;
      for (;;)
        {
        XM.Clear();
        XM.PackMsg(new CXM_ReadIndexedData(Start, 0, LastIndex));
        pEO->EO_QueryHistoryCfg(XM);
        Start=0;
        #if dbgExecutive
        if (dbgMsgs())
          XM.dbgDump((flag)dbgFullMsgs(), pEO->sName());
        #endif
        if (XM.MsgAvail(XM_KeepHistory))
          {
          CXM_Route HRouteCopy(HRoute);
          HRouteCopy=HRoute;
          XSendMessage(NULL, XM, HRoute);
          CXM_Long *pLong = XM.Long();
          LastIndex=pLong->lValue;
          }
        else
          break;
        }
      }

    }
  }

//---------------------------------------------------------------------------

bool CExecutive::KeepHistoryFile(char* pFilename)
  {//add list of tags in specified file to list of tags to be recorded
  if (pFilename && strlen(pFilename))
    {
    Strng Fn(pFilename);
    Strng Path(pFilename);
    Path = Path.FnDrivePath();
    if (Path.Len()==0)
      {
      Fn = PrjFiles();
      Fn += pFilename;
      }
    if (FileExists(Fn()))
      {
      CXM_Route HRoute;
      flag GotHistorian=XFindObject(NULL, (char*)pExecName_Historian, HRoute);
      if (GotHistorian)
        {
        CXM_KeepHistoryFile * xb = new CXM_KeepHistoryFile(Fn());
        CXMsgLst XM;
        XM.PackMsg(xb);
        DWORD RetCode = XSendMessage(NULL, XM, HRoute);
        return true;
        }
      }
    else
      {
      LogWarning("Historian", 0, "Record taglist file not found:%s", Fn());
      }
    }
  return false;
  }

//---------------------------------------------------------------------------

const pchar StateDescs [] =
  { "Stopped",
    "Idling",
    "Solving",
    "Running",
    "Building",
    "Updating",
    "Starting",
    "Stopping",
    "???" };

int CExecutive::StateIndex()
  {
  if (wState & EXF_Building)
    return 4;
  else if (wState & EXF_Updating)
    return 5;
  else if (wState & EXF_Starting)
    return 6;
  else if (wState & EXF_Stopping)
    return 7;
  else if (wState & EXF_Stopped)
    return 0;
  else if (wState & EXF_Paused)
    return 1;
  else if (Busy())
    {
    if (wState & EXF_SolvingPB)
      return 2;
    else
      return 3;
    }
  return 8;
  }

//---------------------------------------------------------------------------

char* CExecutive::StateDesc(int index)
  {
  return StateDescs[index];
  }

//---------------------------------------------------------------------------

char* CExecutive::StateDesc()
  {
  return StateDescs[StateIndex()];
  }
/*
const word EXF_Stopped        = 0x00000001;
const word EXF_Running        = 0x00000002;
const word EXF_Starting       = 0x00000004;
const word EXF_Stopping       = 0x00000008;
const word EXF_Building       = 0x00000010;
const word EXF_Updating       = 0x00000020;
const word EXF_Waiting        = 0x00000040;
const word EXF_Paused         = 0x00000080;
const word EXF_SolvingPB      = 0x00000100;
const word EXF_Busy           = 0x00000200;
const word EXF_BetweenSteps   = 0x00000400;
const word EXF_StartError     = 0x00000800;
inline flag   CExecutive::Starting()                    { return (wState & EXF_Starting)!=0;};
inline flag   CExecutive::StartError()                  { return (wState & EXF_StartError)!=0;};
inline flag   CExecutive::Running()                     { return (wState & EXF_Running)!=0;};
inline flag   CExecutive::Stopping()                    { return (wState & EXF_Stopping)!=0;};
inline flag   CExecutive::Busy()                        { return (wState & EXF_Busy)!=0;}; //starting OR running OR stopping OR paused
inline flag   CExecutive::SolvingPB()                   { return (wState & EXF_SolvingPB)!=0;}; //in ProBal mode
inline flag   CExecutive::Waiting()                     { return (wState & EXF_Waiting)!=0;};
inline flag   CExecutive::Paused()                      { return (wState & EXF_Paused)!=0;};
inline flag   CExecutive::BetweenSteps()                { return (wState & EXF_BetweenSteps)!=0;};
inline flag   CExecutive::AutomationBusy()              { return bAutomationBusy;};
inline flag   CExecutive::Filing()                      { return (eFileWhat!=EF_Null);};
inline double CExecutive::Time()                        { return TimeCB().m_Time;};
inline double CExecutive::StepSizeMax()                 { return TimeCB().m_StepSizeMax;};
inline flag   CExecutive::SyncWithClock()               { return TimeCB().m_bSyncWithClock;};
inline flag   CExecutive::RealTime()                    { return TimeCB().m_bRealTime;};
inline double CExecutive::RealTimeMult()                { return TimeCB().m_RealTimeMult;};
inline long   CExecutive::MaxRunSteps()                 { return TimeCB().m_nMaxSteps;};
inline DWORD  CExecutive::LoopCount()                   { return m_dwLoopCount; };

inline flag   CExecutive::Stopped()                     { return (wState & EXF_Busy)==0;}; //starting OR running OR stopping OR paused*/

//---------------------------------------------------------------------------

void CExecutive::BuildDataLists()
  {
  #if dbgExecutive
  if (dbgBuildDataLists())
    dbgpln("BuildDataLists >>>>>>>>>>>>>>>>>>>>>>>>>>");
  CStopWatch SW;
  if (dbgTimingStats())
    SW.Start();
  #endif
  for (int EInx=0; EInx<Objs_Exec.GetSize(); EInx++)
    {
    CExecObj *pEO=Objs_Exec[EInx];
    CXMsgLst XM;
    flag Start=1;
    long LastIndex=0;
    do
      {
      XM.Clear();
      XM.PackMsg(new CXM_ReadIndexedData(Start, 0, LastIndex));
      pEO->EO_QuerySubsReqd(XM);
      Start=0;
      #if dbgExecutive
      if (dbgMsgs())
        XM.dbgDump((flag)dbgFullMsgs(), pEO->sName());
      #endif
      }
    while (pEO->LoadTags(XM, true, LastIndex));
      pEO->bListUpdateReqd=0;
    }

  #if dbgExecutive
  if (dbgTimingStats())
    dbgpln("BuildDataLists Query      : %10.6f",SW.Lap());
  #endif

  for (EInx=0; EInx<Objs_Exec.GetSize(); EInx++)
    Objs_Exec[EInx]->EO_QuerySubsReqdDone();

  #if dbgExecutive
  if (dbgTimingStats())
    dbgpln("BuildDataLists QueryDone  : %10.6f",SW.Lap());
  #endif

  for (EInx=0; EInx<Objs_Exec.GetSize(); EInx++)
    {
    CExecObj *r=Objs_Exec[EInx];
    CXMsgLst XM;
    flag Start=1;
    long LastIndex=0;
    for (int i=0; i < r->nTags; i++)
      {
      //CXM_DataRequest DR(0, "", TABOpt_XRefStatus, XIO_In);
      CXTagBlk* b=r->aTags[i];
      flag Flush=0, LastTag=(i == r->nTags-1);
      if (b && b->bClientEnd)
        {
        flag Found=0;
        if (!b->Rmt.Valid())
          for (int SInx=0; SInx<Objs_Search.GetSize() && !Found; SInx++)
            Found=Objs_Search[SInx]->SearchForTag(b->Tag(), EO_ReverseAction(b->eAction), false, b->Rmt);
        if (!Found)
          {
          //dbgpln("DR  %08x%08x %s", b->Options, b->Tag());
          //DR.Set(i, b->Tag(), b->Options/*|TABOpt_XRefStatus*/, EO_ReverseAction(b->eAction));
          if (!XM.PackMsg(new CXM_DataRequest(i, b->Tag(), b->Options/*|TABOpt_XRefStatus*/, EO_ReverseAction(b->eAction))))
            Flush=1;
          }
        }

      while (LastTag && (XM.NoMsgs()>0) || Flush)
        {
        #if dbgExecutive
        if (dbgMsgs())
            XM.dbgDump((flag)dbgFullMsgs(), "Query Available");
        #endif
        for (int SInx=0; SInx<Objs_Search.GetSize(); SInx++)
          {
          CExecObj *a=Objs_Search[SInx];
          XM.RewindReadPosition();
          CXMsgLst XMr;
          a->EO_QuerySubsAvail(XM, XMr);
          a->bQueryAvailExecd=1;
          #if dbgExecutive
          if (dbgMsgs())
              XMr.dbgDump((flag)dbgFullMsgs(), a->sName());
          #endif
          a->LoadTags(XMr, false, LastIndex);
          }
        XM.Clear();
        if (Flush)
          {
          //CXM_ReadIndexedData RID(Start, false, LastIndex);
          //XM.PackMsg(RID);
          //VERIFY(XM.PackMsg(DR));//"TA", i, Buff, strlen(Buff)));
          }
        Flush=0;
        }
      }
    }

  #if dbgExecutive
  if (dbgTimingStats())
    dbgpln("BuildDataLists Search     : %10.6f",SW.Lap());
  #endif

  for (EInx=0; EInx<Objs_Exec.GetSize(); EInx++)
    {
    CExecObj *r=Objs_Exec[EInx];
    CXMsgLst XM;
    for (int i=0; i < r->nTags; i++)
      {
      CXTagBlk* b=r->aTags[i];
      if (b && b->bClientEnd)
        {
        flag Found;
        if ((Found=b->Rmt.Valid())==0)
          for (int SInx=0; SInx<Objs_Search.GetSize() && !Found; SInx++)
            Found=Objs_Search[SInx]->SearchForTag(b->Tag(), EO_ReverseAction(b->eAction), false, b->Rmt);
        if (!Found) //&& !bListRebuildRequest)
          {
          //LogWarning("Executive", 0, "Tag not found : %s", b->Tag());
          #if dbgExecutive
          if (dbgDumpConns())
            dbgpln("Tag MISSING %i %s.%s", i, r->sName(), b->sExtTag());
          #endif
          CXM_TagNotAvail *pXTag=new CXM_TagNotAvail(i, b->Tag());
          if (!XM.PackMsg(pXTag))
            {
            r->EO_TagsNotAvail(XM);
            XM.Clear();
            XM.PackMsg(pXTag);
            }
          }
        }
      }
    if (XM.NoMsgs()>0)
      r->EO_TagsNotAvail(XM);
    }

  #if dbgExecutive
  if (dbgTimingStats())
    dbgpln("BuildDataLists Search 1   : %10.6f",SW.Lap());
  #endif

  for (EInx=0; EInx<Objs_Exec.GetSize(); EInx++)
    if (Objs_Exec[EInx]->bQueryAvailExecd)
    {
    Objs_Exec[EInx]->EO_QuerySubsAvailDone();
    Objs_Exec[EInx]->bQueryAvailExecd=0;
    }

  #if dbgExecutive
  if (dbgTimingStats())
    dbgpln("BuildDataLists Avail Done : %10.6f",SW.Lap());
  #endif

  #if dbgExecutive
  if (dbgDumpConns())
    {
    int l1=0,l2=0;
    for (EInx=0; EInx<Objs_Exec.GetSize(); EInx++)
      {
      CExecObj *r=Objs_Exec[EInx];
      l1=Max(l1, (int)strlen(r->sName()));
      for (int i=0; i < r->nTags; i++)
        if (r->aTags[i]!=NULL)
          l2=Max(l2, (int)strlen(r->aTags[i]->sExtTag()));
      }

    for (EInx=0; EInx<Objs_Exec.GetSize(); EInx++)
      {
      CExecObj *r=Objs_Exec[EInx];
      for (int i=0; i < r->nTags; i++)
        {
        CXTagBlk* b=r->aTags[i];
        if (b!=NULL && b->Rmt.Valid())
          {
          CXTagBlk* a=b->Rmt.pTag();
          dbgpln("%-*s [%3i] %-*s %s%s %-*s [%3i] ",
                 l1,r->sName(),i, l2, b->sExtTag(), b->eAction & XIO_In ? "from": "    ",
                 b->eAction & XIO_Out ? "to": "  ", l1,Objs_Exec[b->Rmt.iObjExecIndex]->sName(),b->Rmt.iTag);
          }
        }
      }
    }
  #endif
  }

//---------------------------------------------------------------------------

void CExecutive::UpdateDataLists()
  {
  #if dbgExecutive
  if (dbgBuildDataLists())
    dbgpln("UpdateDataLists >>>>>>>>>>>>>>>>>>>>>>>>>>");
  #endif

  for (int EInx=0; EInx<Objs_Exec.GetSize(); EInx++)
    if (Objs_Exec[EInx]->bListUpdateReqd)
      {
      CExecObj *pEO=Objs_Exec[EInx];
      CXMsgLst XM;
      flag Start=1;
      long LastIndex=0;
      do
        {
        XM.Clear();
        XM.PackMsg(new CXM_ReadIndexedData(Start, 0, LastIndex));
        pEO->EO_QuerySubsReqd(XM);
        Start=0;
        #if dbgExecutive
        if (dbgMsgs())
          XM.dbgDump((flag)dbgFullMsgs(), pEO->sName());
        #endif
        }
      while (pEO->UpdateTags(XM, true, LastIndex));
      }

  for (EInx=0; EInx<Objs_Exec.GetSize(); EInx++)
    if (Objs_Exec[EInx]->bListUpdateReqd)
      Objs_Exec[EInx]->EO_QuerySubsReqdDone();

  for (EInx=0; EInx<Objs_Exec.GetSize(); EInx++)
    if (Objs_Exec[EInx]->bListUpdateReqd)
      {
      CExecObj *pEO=Objs_Exec[EInx];
      CXMsgLst XM;
      flag Start=1;
      long LastIndex=0;
      for (int i=0; i < pEO->nTags; i++)
        {
        //CXM_DataRequest DR(0, "", 0, XIO_In);
        CXTagBlk* b=pEO->aTags[i];
        flag Flush=0, LastTag=(i == pEO->nTags-1);
        if (b && b->bClientEnd && !b->Rmt.Valid())
          {
          flag Found=0;
          for (int SInx=0; SInx<Objs_Search.GetSize() && !Found; SInx++)
            Found=Objs_Search[SInx]->SearchForTag(b->Tag(), EO_ReverseAction(b->eAction), false, b->Rmt);
          if (!Found)
            {
            if (!XM.PackMsg(new CXM_DataRequest(i, b->Tag(), b->Options, EO_ReverseAction(b->eAction))))
              Flush=1;
            }
          }

        while (LastTag && (XM.NoMsgs()>0) || Flush)
          {
          #if dbgExecutive
          if (dbgMsgs())
              XM.dbgDump((flag)dbgFullMsgs(), "Query Available");
          #endif
          for (int SInx=0; SInx<Objs_Search.GetSize(); SInx++)
            {
            CExecObj *a=Objs_Search[SInx];
            XM.RewindReadPosition();
            CXMsgLst XMr;
            a->EO_QuerySubsAvail(XM, XMr);
            a->bQueryAvailExecd=1;
            #if dbgExecutive
            if (dbgMsgs())
                XMr.dbgDump((flag)dbgFullMsgs(), a->sName());
            #endif
            a->UpdateTags(XMr, false, LastIndex);
            }
          XM.Clear();
          if (Flush)
            {
            //CXM_ReadIndexedData RID(Start, false, LastIndex);
            //XM.PackMsg(RID);
            //VERIFY(XM.PackMsg(DR));//"TA", i, Buff, strlen(Buff)));
            }
          Flush=0;
          }
        }
      }

  for (EInx=0; EInx<Objs_Exec.GetSize(); EInx++)
    if (Objs_Exec[EInx]->bListUpdateReqd)
      {
      CExecObj *pEO=Objs_Exec[EInx];
      CXMsgLst XM;
      for (int i=0; i < pEO->nTags; i++)
        {
        CXTagBlk* b=pEO->aTags[i];
        if (b && b->bClientEnd)
          {
          flag Found;
          if ((Found=b->Rmt.Valid())==0)
            for (int SInx=0; SInx<Objs_Search.GetSize() && !Found; SInx++)
              Found=Objs_Search[SInx]->SearchForTag(b->Tag(), EO_ReverseAction(b->eAction), false, b->Rmt);
          if (!Found) //&& !bListRebuildRequest)
            {
            //LogWarning("Executive", 0, "Tag not found : %s", b->Tag());
            #if dbgExecutive
            if (dbgDumpConns())
              dbgpln("Tag MISSING %i %s.%s", i, pEO->sName(), b->sExtTag());
            #endif
            CXM_TagNotAvail *pXTag=new CXM_TagNotAvail(i, b->Tag());
            if (!XM.PackMsg(pXTag))
              {
              pEO->EO_TagsNotAvail(XM);
              XM.Clear();
              XM.PackMsg(pXTag);
              }
            }
          }
        }
      if (XM.NoMsgs()>0)
        pEO->EO_TagsNotAvail(XM);
      }

  for (EInx=0; EInx<Objs_Exec.GetSize(); EInx++)
    if (Objs_Exec[EInx]->bQueryAvailExecd)
      {
      CExecObj *pEO=Objs_Exec[EInx];
      pEO->EO_QuerySubsAvailDone();
      pEO->bListUpdateReqd=0;
      pEO->bQueryAvailExecd=0;
      }


  #if dbgExecutive
  if (dbgDumpConns())
    {
    int l1=0,l2=0;
    for (EInx=0; EInx<Objs_Exec.GetSize(); EInx++)
      {
      CExecObj *pEO=Objs_Exec[EInx];
      l1=Max(l1, (int)strlen(pEO->sName()));
      for (int i=0; i < pEO->nTags; i++)
        if (pEO->aTags[i]!=NULL)
          l2=Max(l2, (int)strlen(pEO->aTags[i]->sExtTag()));
      }

    for (EInx=0; EInx<Objs_Exec.GetSize(); EInx++)
      {
      CExecObj *pEO=Objs_Exec[EInx];
      for (int i=0; i < pEO->nTags; i++)
        {
        CXTagBlk* b=pEO->aTags[i];
        if (b!=NULL && b->Rmt.Valid())
          {
          CXTagBlk* a=b->Rmt.pTag();
          dbgpln("%-*s [%3i] %-*s %s%s %-*s [%3i] ",
                 l1,pEO->sName(),i, l2, b->sExtTag(), b->eAction & XIO_In ? "from": "    ",
                 b->eAction & XIO_Out ? "to": "  ", l1,Objs_Exec[b->Rmt.iObjExecIndex]->sName(),b->Rmt.iTag);
          }
        }
      }
    }
  #endif
  };

//---------------------------------------------------------------------------

ExecCoupling CExecutive::Coupling() { return eCoupling; };

//---------------------------------------------------------------------------

ExecCoupling CExecutive::SetCoupling(ExecCoupling XC)
  {
  eCoupling=XC;
  return eCoupling;
  };

//---------------------------------------------------------------------------

long CExecutive::GlblRunModes()
  {
  return TaggedObject::GlblRunModes();
  };

//---------------------------------------------------------------------------

void CExecutive::SetGlblRunModes(long RunMds, long RunMdsMsk)
  {
  if ((GlblRunModes()&&SM_All)!=0 && (RunMds==SM_DynCurrent) && (RunMdsMsk==SM_All))
    RunMds=GlblRunModes() & SM_All;

  if (RunMdsMsk&SM_All)
    {
    if (RunMds&SM_Direct)
      { ASSERT_ALWAYS((RunMds&SM_All) == SM_Direct,   "Multiple Solve Modes"); }
    if (RunMds&SM_Inline)
      { ASSERT_ALWAYS((RunMds&SM_All) == SM_Inline,   "Multiple Solve Modes"); }
    if (RunMds&SM_Buffered)
      { ASSERT_ALWAYS((RunMds&SM_All) == SM_Buffered, "Multiple Solve Modes"); }
    }
  if (RunMdsMsk&HM_All)
    {
    if (RunMds&HM_None)
      { ASSERT_ALWAYS((RunMds&HM_All) == HM_None,     "Multiple Heat Modes"); }
    if (RunMds&HM_Simple)
      { ASSERT_ALWAYS((RunMds&HM_All) == HM_Simple,   "Multiple Heat Modes"); }
    if (RunMds&HM_Full)
      { ASSERT_ALWAYS((RunMds&HM_All) == HM_Full,     "Multiple Heat Modes"); }
    }

  if (wState&EXF_Busy)
    return;

  if ((TaggedObject::GlblRunModes()&RunMdsMsk)==(RunMds&RunMdsMsk))
    return;

  TaggedObject::SetGlblRunModes(RunMds, RunMdsMsk);

  if (RunMdsMsk&SM_All)
    {
    if ((RunMds&SM_All)==SM_Direct)
      SwapToProBal();
    else
      ReturnFromProBal();
    }
  //CNM

  for (int OInx=0; OInx<Objs_Reg.GetSize(); OInx++)
    Objs_Reg[OInx]->EO_SetSolveMode(RunMds, RunMdsMsk);
  };

//---------------------------------------------------------------------------

Strng CExecutive::GlblRunModesString(long MdMsk)
  {
  return TaggedObject::GlblMode2String(TaggedObject::GlblRunModes(), MdMsk);
  };

//---------------------------------------------------------------------------

void CExecutive::SetGlblRunModesString(LPCSTR RunMds, long RunMdsMsk)
  {
  if (RunMds)
    SetGlblRunModes(TaggedObject::String2GlblMode(RunMds), RunMdsMsk);
  };

//---------------------------------------------------------------------------
//
//byte CExecutive::GetDynMode()
//  {
//  return TaggedObject::GetGlblDynMode();
//  };
//
////---------------------------------------------------------------------------
//
//void CExecutive::SetDynMode(byte NewDynMode)
//  {
//  if (wState&EXF_Busy)
//    return;
//
//  if (TaggedObject::GlblRunModes()==DYNMODE)
//    {
//    TaggedObject::SetGlblDynMode(NewDynMode);
//    return;
//    }
//
//  TaggedObject::SetGlblDynMode(NewDynMode);
//  ReturnFromProBal();
//
//  for (int OInx=0; OInx<Objs_Reg.GetSize(); OInx++)
//    Objs_Reg[OInx]->EO_SetSolveMode(DYNMODE);
//  };
//
//---------------------------------------------------------------------------

flag CExecutive::TestRunning()
  {
  if (Busy())
    {
    LogWarning("Executive", MB_ICONWARNING, "SysCAD Still %s", Waiting() ? "Idling" : "Solving");
    gs_MsgLog.ClrMsgBoxSkip();
    return true;
    }

  return false;
  };

//---------------------------------------------------------------------------

void CExecutive::GotoEditMode()
  {
  PostCmd(0, EXM_GOTOEDIT);//EXM_STOP);
  SetStopSleep();
  }

void CExecutive::GotoRunMode(RqdRunMode Md)
  {
  PostCmd(0, EXM_GOTORUN/*EXM_START*/, (LONG)Md);
  SetStopSleep();
  }

void CExecutive::GotoIdleMode()
  {
  PostCmd(0, EXM_GOTOIDLE);
  SetStopSleep();
  }

void CExecutive::DoOneStep(CDoOneStepInfo Info)
  {
  //CStopWatch SWf;
  //dbgpln("Exec DoOneStep        %6i", Info.lWaitForNextMS);

  PostCmd(0, EXM_DOONESTEP, Info.lParam);
  SetStopSleep();
  }

//---------------------------------------------------------------------------

void CExecutive::SetSolveTool(SolveToolBase * SolveTool)
  {
  if (Busy())
    return;
  pSolveTool=SolveTool;
  }

//---------------------------------------------------------------------------

void CExecutive::SwapToProBal()
  {
  if(!fDoingPB)
    PBTmCB.m_Time=DynTmCB.m_Time;
  fDoingPB=true;
  }

void CExecutive::ReturnFromProBal()
  {
  //double T=CB.Time;
  if (fDoingPB)
    DynTmCB.m_Time=PBTmCB.m_Time;
  fDoingPB=false;
  }

//---------------------------------------------------------------------------

void CExecutive::RequestReadWriteAll()
  {
  m_ReadWriteAllReqd=true;
  };

void CExecutive::BeginBulkChange()
  {
  for (int OInx=0; OInx<Objs_Reg.GetSize(); OInx++)
    Objs_Reg[OInx]->EO_BeginBulkChange();
  SetHoldValidateData(true, true, true);
  }
void CExecutive::EndBulkChange()
  {
  gs_Exec.SetHoldValidateData(false, true, true);
  for (int OInx=0; OInx<Objs_Reg.GetSize(); OInx++)
    Objs_Reg[OInx]->EO_EndBulkChange();
  };

//---------------------------------------------------------------------------

void   CExecutive::SetSyncWithClock(flag SyncWithClock_)
  {
  TimeCB().m_bSyncWithClock=SyncWithClock_;
  if (SyncWithClock_)
    {
    SetRealTime(true);
    SetRealTimeMult(1.0);
    };
  };
void   CExecutive::SetRealTime(flag RealTime_)   { TimeCB().m_bRealTime=RealTime_;};
void   CExecutive::SetRealTimeMult(double M)     { TimeCB().m_RealTimeMult=Range(0.001,M,50000.0);};
flag CExecutive::SetTime(double Time_, bool Rewind)
  {
  DynTmCB.m_Time=PBTmCB.m_Time=Time_;
  for (int EInx=0; EInx<Objs_Reg.GetSize(); EInx++)
    {
    if (!Objs_Reg[EInx]->EO_SetTime(Time_, Rewind))
      {
      return false;
      }
    }
  return true;
  };
void CExecutive::SetStartTime(double StartTime_) { TimeCB().m_StartTime=StartTime_; };
void CExecutive::SetStopTime(double StopTime_) { TimeCB().m_StopTime=StopTime_; };
void CExecutive::SetStepSizeMax(double StepSizeMax_)
  {
  //if (StepSizeMax()!=StepSizeMax_)
    {
    TimeCB().m_StepSizeMax=StepSizeMax_;
    //TimeCB().m_StepSizeNxt=dNAN;
    }
  };
void CExecutive::SetMaxRunSteps(long MaxIters_) { TimeCB().m_nMaxSteps=MaxIters_; };

//---------------------------------------------------------------------------

//static CSolveDlg *pSolveDlg=NULL;
void CExecutive::OpenSolver()
  {
  };

//---------------------------------------------------------------------------

void CExecutive::DoAppActivate()
  {
  #if dbgExecutive
  if (dbgExecSeq())
    dbgpln("DoAppActivate -------------------------------------- >>>");
  #endif

  for (int OInx=0; OInx<Objs_Reg.GetSize(); OInx++)
    {
    CExecObj *a=Objs_Reg[OInx];
    a->LockXO();
    if (bDoAppActivate&1)
      a->EO_OnAppActivate(true);
    if (bDoAppActivate&2)
      a->EO_OnAppActivate(false);
    a->UnlockXO();
    }
  if (bDoAppActivate&2)
    gs_EventLog.FlushFile();
  bDoAppActivate=0;
  #if dbgExecutive
  if (dbgExecSeq())
    dbgpln("DoAppActivate -------------------------------------- <<<");
  #endif
  }

//---------------------------------------------------------------------------

void CExecutive::OnAppActivate(BOOL bActive)
  {
  #if dbgExecutive
  if (dbgExecSeq())
    dbgpln("OnAppActivate");
  #endif
  bDoAppActivate |= (bActive ? 1 : 2);
  if (!Busy/* Running*/())
    DoAppActivate();
  }

//---------------------------------------------------------------------------

flag CExecutive::RequestTagInfo(RequestTagInfoRec& Rqst, ReplyTagInfoRec& Info)
  {
  if (Rqst.bFirst)
    {
    Info.OInx = 0;
    if (Info.OInx<Objs_Reg.GetSize() && Rqst.sExcludeExecObj.Len()!=0 && stricmp(Objs_Reg[Info.OInx]->Name(), Rqst.sExcludeExecObj())==0)
      Info.OInx++;
    Info.iCount = -1;
    }

  while (Info.OInx<Objs_Reg.GetSize())
    {
    Info.bData1Valid = 0;
    Info.bDWDataValid = 0;
    Info.bCanEditFile = 0;
    Info.bExcelFile = 0;
    Info.bForceCopy = 0;
    Info.bPreventCopy = 0;
    Info.pExecObj = Objs_Reg[Info.OInx];
    Info.iCount++;
    if (Info.pExecObj->EO_RequestTagInfo(Rqst, Info))
      {
      Rqst.bFirst = false;
      return true;
      }
    Info.OInx++;
    if (Info.OInx<Objs_Reg.GetSize() && Rqst.sExcludeExecObj.Len()!=0 && stricmp(Objs_Reg[Info.OInx]->Name(), Rqst.sExcludeExecObj())==0)
      Info.OInx++;
    Rqst.bFirst = true;
    Info.iCount = -1;
    }
  return false;
  }

//---------------------------------------------------------------------------

void CExecutive::DoSetModelState(eScdMdlStateActs RqdState, Strng_List &RqdTags)
  {
  for (int OInx=0; OInx<Objs_Reg.GetSize(); OInx++)
    {
    CExecObj *a=Objs_Reg[OInx];
    a->LockXO();
    a->EO_SetModelState(RqdState, RqdTags);
    a->UnlockXO();
    }
  UpdateDisplays(true);
  }

//---------------------------------------------------------------------------

void CExecutive::SetModelState(eScdMdlStateActs RqdState, Strng_List &RqdTags)
  {
  if (Busy/*Running*/())
    {
    eRqdState=RqdState;
    sRqdTags=RqdTags;
    }
  else
    DoSetModelState(RqdState, RqdTags);
  }

//---------------------------------------------------------------------------

flag CExecutive::GetModelAction(Strng & Tag, MdlActionArray & Acts)
  {
  for (int OInx=0; OInx<Objs_Reg.GetSize(); OInx++)
    {
    CExecObj *a=Objs_Reg[OInx];
    a->LockXO();
    if (a->EO_GetModelAction(Tag, Acts))
      {
      a->UnlockXO();
      return true;
      }
    a->UnlockXO();
    }
  return false;
  }

//---------------------------------------------------------------------------

flag CExecutive::SetModelAction(Strng & Tag, MdlAction & Act)
  {
  for (int OInx=0; OInx<Objs_Reg.GetSize(); OInx++)
    {
    CExecObj *a=Objs_Reg[OInx];
    a->LockXO();
    if (a->EO_SetModelAction(Tag, Act))
      {
      a->UnlockXO();
      return true;
      }
    a->UnlockXO();
    }
  return false;
  }

//---------------------------------------------------------------------------

flag CExecutive::XFindObject(CExecObj *pEO, pchar pName, CXM_Route &Route, flag AllowSelf)
  {
  LockExec();

  for (int OInx=0; OInx<Objs_Reg.GetSize(); OInx++)
    {
    CExecObj *a=Objs_Reg[OInx];
    if (a!=pEO || AllowSelf)
      {
      flag GotRoute=0;
      if (stricmp(pName, Objs_Reg[OInx]->Name())==0)
        GotRoute=1;
      else if (a->Client() && a->EO_FindObject(pName, Route))
        GotRoute=1;

      if (GotRoute)
        {
        //Route.GetComputerName
        NodeNameBuff NdName;
        CXM_Route::MakeNodeName(a->Name(), NdName);
        flag OK=Route.AddNodeRev(a->Id(), NdName);
        ASSERT(OK);
        if (pEO)
          {
          CXM_Route::MakeNodeName(pEO->Name(), NdName);
          OK=Route.AddNodeRev(pEO->Id(), NdName);
          ASSERT(OK);
          }
        UnlockExec();
        return true;
        }
      }
    }
  UnlockExec();
  return false;
  }

//---------------------------------------------------------------------------

DWORD CExecutive::XSendMessage(CExecObj *pEO, CXMsgLst &XM, CXM_Route &Route, flag FromExecThread)
  {
  if (FromExecThread)
    {
//    LockExec();
    LclXSendMsgBlk LXSM;
    LXSM.m_pEO=pEO;
    LXSM.m_pXM=&XM;
    LXSM.m_pRoute=&Route;

    PostThreadMessage(ExecThreadId(), WMU_XSENDMESSAGE, 0, (long)&LXSM);

    WaitForSingleObject(LXSM.m_Done.m_hObject, INFINITE);
//    UnlockExec();
    return LXSM.m_Return;
    }

  CXM_Route LclRoute;
  LclRoute=Route;
  LockExec();
  if (pEO!=NULL)
    {
    #ifdef _DEBUG
    NodeNameBuff NdName;
    CXM_Route::MakeNodeName(pEO->sName(), NdName);
    #if dbgExecutive
    if (dbgRouteCmp())
      dbgpln("RouteCmp:%s<>%s  %i<>%i", LclRoute.NodeName(0), NdName, LclRoute.NodeObjId(0), pEO->Id());
    #endif
    ASSERT(stricmp(LclRoute.NodeName(0), NdName)==0 && LclRoute.NodeObjId(0)==pEO->Id());
    #endif
    LclRoute.RemoveNode(0);
    }

  long iIdDst=LclRoute.NodeObjId(0);

  CExecObj *pDst=NULL;
  for (int OInx=0; OInx<Objs_Reg.GetSize(); OInx++)
    if (Objs_Reg[OInx]->Id()==iIdDst)
      {
      pDst=Objs_Reg[OInx];
      break;
      }
  if (pDst)
    {
    #ifdef _DEBUG
    NodeNameBuff NdName;
    CXM_Route::MakeNodeName(pDst->sName(), NdName);
    #if dbgExecutive
    if (dbgRouteCmp())
      dbgpln("RouteCmp:%s<>%s  %i<>%i", LclRoute.NodeName(0), NdName, LclRoute.NodeObjId(0), pDst->Id());
    #endif
    ASSERT(stricmp(LclRoute.NodeName(0), NdName)==0 && LclRoute.NodeObjId(0)==pDst->Id());
    #endif
    LclRoute.RemoveNode(0);

    #if dbgExecutive
    if ( dbgXSendMsgs())
      XM.dbgDump((flag)dbgFullMsgs(), pDst->sName());
    #endif
    UnlockExec();
    return pDst->EO_Message(XM, LclRoute);
    }
  UnlockExec();
  return 0;
  }

//---------------------------------------------------------------------------

flag CExecutive::XBuildDataLists(CExecObj *pEO)
  {
  #if dbgExecutive
  if (dbgMsgs())
    dbgpln("======================= XBuildDataLists ==========================");
  #endif

  bListRebuildReqd=1;
  //bListRebuildRequest=1;
  return true;
  };

//---------------------------------------------------------------------------

flag CExecutive::XBuildMyDataLists(CExecObj *pEO)
  {
  #if dbgExecutive
  if (dbgMsgs())
    dbgpln("======================= XBuildMyDataLists ==========================");
  #endif

  //bListRebuildReqd=1;
  bListUpdateReqd=1;
  pEO->bListUpdateReqd=1;

  // NBNB This should be made more efficient

  return true;
  };

//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//#pragma optimize("", off)

DWORD CExecutive::XReadTaggedItem(CExecObj *pEO, CXM_ObjectTag & ObjTag, CXM_ObjectData & ObjData, CXM_Route &Route)//, CXMsgLst &XM)
  {
  DWORD RetCode = 0;
  CExecObj *XObjs[256];
  LockExec();
  for (int NObjs=0; NObjs<Objs_Reg.GetSize(); NObjs++)
    XObjs[NObjs]=Objs_Reg[NObjs];
  UnlockExec();
  ASSERT_ALWAYS(NObjs<256, "XReadTaggedItem NObjs>=256")

  for (int OInx=0; OInx<NObjs; OInx++)
    {
    CExecObj *a=XObjs[OInx];
    if (a!=pEO)
      {
      a->LockXO();
      RetCode = a->EO_ReadTaggedItem(ObjTag, ObjData, Route);
      a->UnlockXO();
      if (RetCode)
        {
        #if dbgExecutive
        if (dbgMsgs())
          {
          //_asm int 3;
          CXMsgLst XM;
          XM.SetOnHeap(false);
          XM.PackMsg(&ObjData);
          XM.dbgDump((flag)dbgFullMsgs(), a->sName());
          }
        #endif
        NodeNameBuff NdName;
        CXM_Route::MakeNodeName(a->sName(), NdName);
        flag OK=Route.AddNodeRev(a->Id(), NdName);
        ASSERT(OK);
        if (pEO)
          {
          CXM_Route::MakeNodeName(pEO->Name(), NdName);
          OK=Route.AddNodeRev(pEO->Id(), NdName);
          ASSERT(OK);
          }
        //UnlockExec();
        return RetCode;
        }
      }
    }
  //UnlockExec();
  return 0;
  }

//#pragma optimize("", on)

//---------------------------------------------------------------------------

int CExecutive::XWriteTaggedItem(CExecObj *pEO, CXM_ObjectData &ObjData, CXM_Route &Route)
  {
  CXM_Route LclRoute;
  LclRoute=Route;

  CExecObj *XObjs[256];
  LockExec();
  for (int NObjs=0; NObjs<Objs_Reg.GetSize(); NObjs++)
    XObjs[NObjs]=Objs_Reg[NObjs];
  UnlockExec();
  ASSERT_ALWAYS(NObjs<256, "XWriteTaggedItem NObjs>=256")

  #ifdef _DEBUG
  NodeNameBuff NdName;
  CXM_Route::MakeNodeName(pEO->sName(), NdName);
  ASSERT(stricmp(LclRoute.NodeName(0), NdName)==0 && LclRoute.NodeObjId(0)==pEO->Id());
  #endif
  LclRoute.RemoveNode(0);

  long iIdDst=LclRoute.NodeObjId(0);

  CExecObj *pDst=NULL;
  for (int OInx=0; OInx<NObjs; OInx++)
    if (XObjs[OInx]->Id()==iIdDst)
      {
      pDst=XObjs[OInx];
      break;
      }
  if (pDst)
    {
    //flag ResWasValid=TaggedObject::GlblResultsValid();
    #ifdef _DEBUG
    CXM_Route::MakeNodeName(pDst->sName(), NdName);
    ASSERT(stricmp(LclRoute.NodeName(0), NdName)==0 && LclRoute.NodeObjId(0)==pDst->Id());
    #endif
    LclRoute.RemoveNode(0);

    #if dbgExecutive
    if (dbgMsgs())
      {
      CXMsgLst XM;
      XM.SetOnHeap(false);
      XM.PackMsg(&ObjData);
      XM.dbgDump((flag)dbgFullMsgs(), pDst->sName());
      }
    #endif
    pDst->LockXO();
    int ret=pDst->EO_WriteTaggedItem(ObjData, LclRoute);
    pDst->UnlockXO();
    XGlblResultValidity();
    return ret;
    }
  return TOData_NotFound;
  };

//---------------------------------------------------------------------------

void CExecutive::XGlblResultValidity(int IsValid, int IsAlwaysValid)
  {
  if (IsValid>=0)
    TaggedObject::SetGlblResultsValid(IsValid);

  if (IsAlwaysValid>=0)
    TaggedObject::SetGlblResultsAlwaysValid(IsAlwaysValid);

  if (TaggedObject::GlblResultsAlwaysValidChg(false) ||
      !TaggedObject::GlblResultsAlwaysValid() && TaggedObject::GlblResultsValidChg(false))
    bDoGlblResultValidity=true;
  #if dbgExecutive
  if (dbgResultsValid())
    dbgpln("XGlblResultValidity(%2i,%2i) %s", IsValid, IsAlwaysValid, bDoGlblResultValidity ? "DoGlbl":"-");
  #endif
  };

//---------------------------------------------------------------------------

void CExecutive::DoGlblResultValidity()
  {
  #if dbgExecutive
  if (dbgResultsValid())
    dbgpln("Exec::                                    %s Val%i AlVal%i",
           "DoGlblResultValidity",
           TaggedObject::GlblResultsValid(),
           TaggedObject::GlblResultsAlwaysValid());
  #endif
  TaggedObject::GlblResultsAlwaysValidChg(true);
  TaggedObject::GlblResultsValidChg(true);

  DWORD RetCode = 0;
  for (int OInx=0; OInx<Objs_Reg.GetSize(); OInx++)
    {
    CExecObj *a=Objs_Reg[OInx];
    a->LockXO();
    a->EO_GlblResultValidity(TaggedObject::GlblResultsValid(), TaggedObject::GlblResultsAlwaysValid());
    a->UnlockXO();
    }
  bDoGlblResultValidity=false;
  };

//---------------------------------------------------------------------------

int CExecutive::SetHoldValidateData(flag On, flag WithLocks, flag MessagesOff)
  {
  if (On==TaggedObject::bHoldValidateData)
    return On;
  int n=0;
  int WasOn;
  if (On)
    {
    if (WithLocks)
      LockExec();
    WasOn=TaggedObject::SetHoldValidateData(true, MessagesOff); // In case
    }
  else
    {
    WasOn=TaggedObject::SetHoldValidateData(false, MessagesOff); // In case
    if (WithLocks)
      UnlockExec();
    }
  return WasOn;
  };

//---------------------------------------------------------------------------

typedef byte SSFileRecType;
const   byte SS_End            = 0x96;
const   byte SS_DefinedData    = 0x97;
const   byte SS_OtherData      = 0x98;

#pragma pack(push, 1)
const LPCTSTR VerRecHeadStr="SysCAD Data Image File V1.0\r\n\x1a";
const long    VerRecPrjVerPos=strlen(VerRecHeadStr)+1+sizeof(word);
class CVersionRec
  {
  public:
    char          cStr[256];

    CVersionRec()
      {
      memset(cStr, 0, sizeof(cStr));
      strcpy(cStr, VerRecHeadStr);
      word Ver=0x0100;
      memcpy(&cStr[strlen(cStr)+1], &Ver, sizeof(Ver));
      long VerNo=::PrjFileVerNo();
      memcpy(&cStr[VerRecPrjVerPos], &VerNo, sizeof(VerNo));
      };
    dword Length() { return 64; /* Allow spares */ };
    long PrjFileVerNo()
      {
      return *((long*)(&cStr[VerRecPrjVerPos]));
      }
  };

class DataHead
  {
  public:
    SSFileRecType eType;
    DWORD         dwLength;
    byte          wHdLen;
    char          cObjTag[256];

    DataHead()
      {
      eType=SS_End;
      memset(cObjTag, 0, sizeof(cObjTag));
      wHdLen=((char*)cObjTag-(char*)this);
      dwLength=wHdLen;
      };
    DataHead(SSFileRecType Type, DWORD DataLength, char *XObj, char *Tag)
      {
      eType=Type;
      //dwLength=Length;
      wHdLen=((char*)cObjTag-(char*)this);
      ASSERT((strlen(XObj)+1+strlen(Tag)+1)<256);
      strcpy(cObjTag, XObj);
      wHdLen+=strlen(XObj)+1;
      strcpy(&cObjTag[strlen(XObj)+1], Tag);
      wHdLen+=strlen(Tag)+1;
      dwLength=wHdLen+DataLength;
      };
    dword FixedLength()  { return ((char*)cObjTag-(char*)this); };
    dword ObjTagLength() { return wHdLen-FixedLength();};
    dword HeaderLength() { return wHdLen;};
    dword RecordLength() { return dwLength;};
    dword DataLength()   { return dwLength-wHdLen;};
    void SetEndOfData(dword PosOfHd, dword EndPos) { dwLength=EndPos-PosOfHd;};
    char *XName() { return cObjTag; };
    char *ObjTag() { return &cObjTag[strlen(cObjTag)+1]; };

  };

#pragma pack(pop)

// Fix/kludge class for reading old snapshots
class DataHeadChgd
  {
  public:
    Strng XName, ObjTag;

  public:
    DataHeadChgd(DataHead & DH)
      {
      XName=DH.XName();
      ObjTag=DH.ObjTag();
      if (XName.XStrICmp("MG.DLL")==0 ||
          XName.XStrICmp("QAL.DLL")==0 ||
          XName.XStrICmp("QAL96.DLL")==0 ||
          XName.XStrICmp("QAL97.DLL")==0)
        XName=PlantModelTag;
      };

    void ChgXName(const char * XObj) { XName=XObj; };
//    char *XName() { return XName();  };
//    char *ObjTag() { return sObjTag(); };

  };

//---------------------------------------------------------------------------

int CExecutive::SaveDBase(long DBFmt, DWORD DBOpts, char* pRoot, char* pOldPrjFiles)
  {
  LockExec();
  CDlgBusy::Open("");
  int Ret=DoSaveDBase(DBFmt, DBOpts, pRoot, pOldPrjFiles);
  CDlgBusy::Close();
  UnlockExec();
  return Ret;
  }

//---------------------------------------------------------------------------

int  CExecutive::DoSaveDBase(long DBFmt, DWORD DBOpts, char* pRoot, char* pOldPrjFiles)
  {
  CStopWatch SaveTime;
  SaveTime.Start();

  Strng RtFolder(pRoot);
  RtFolder.FnClearEndBSlash();
  FnCompactPath(50, RtFolder);

  CDlgBusy::SetLine(1, "Saving : %s",RtFolder());
  CDlgBusy::SetLine(3, "Open database...");
  CTagDBase TagDB;

  int SvDBFmt=TagDB.Open(DBFmt, DBOpts, pRoot, pOldPrjFiles, NULL);
  if (SvDBFmt<0)//PrjFiles, NULL, pOldPrjName); // to move
    {
    LogError("Save DataBase", LF_Exclamation, "Cannot Open Database");
    return 0;
    }

  FilingControlBlock FCB(pRoot, DefaultCfgLabel, TOF_DBase);
  if (pOldPrjFiles)
    FCB.SetSaveAs(pOldPrjFiles);
  int nObjs=0;
  int nCons=0;
  Strng Fn;
  Fn.Set("%sOther.dat", pRoot);

  HANDLE H=CreateFile(Fn(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
  if (H!=INVALID_HANDLE_VALUE)
    {
    flag WrtOK=true;
    DWORD nBytesWritten;

    CVersionRec VR;
    WrtOK&=WriteFile(H, &VR, VR.Length(), &nBytesWritten, NULL);

    ASSERT(TagDB.IsOpen()); // Is Open
    TagDB.SetChanged(false); //temp

    CArray <int, int> DoObj;
    DoObj.SetSize(Objs_Reg.GetSize());

    TagDB.FindRevisionNo(DBRev_MostRecent);
    TagDB.BumpRevisionNo();
    TagDB.StartSaveSequence("Node");

    for (int OInx=0; OInx<Objs_Reg.GetSize(); OInx++)
      {
      CExecObj *r=Objs_Reg[OInx];
      DoObj[OInx]=r->EO_BeginSave(FCB);
      if (DoObj[OInx])
        {
        // double l1=0,l2=0;
        CXMsgLst XM;
        XM.Clear();
        Strng Tag;
        // l1-=GetTickCount();
        while (r->EO_SaveDefinedData(FCB, Tag, XM))
          {
          int n=nObjs+1;
          if ((n%Min(n,10))==0)
            CDlgBusy::SetLine(3, "Saving Object : %i", n);
          DWORD StartPos, EndPos;
          // l1+=GetTickCount();
          #if dbgExecutive
          if (dbgDBMsgs())
            XM.dbgDump((flag)dbgFullDBMsgs(), r->sName(), "Save");
          #endif

          // l2-=GetTickCount();
          if (XM.MsgAvail())
            {
            CXM_ObjectData* pData=XM.ObjectData();
            CPkDataItem * pPItem=pData->List.FirstItem();
            if (pPItem)//!pData->List.EndOfList(/*pPItem*/))
              {
              TagDB.SaveObject("Node", pData);
              nObjs++;
              }
            }

          if (Tag.Length()>0)
            {
            StartPos=SetFilePointer(H, 0, NULL, FILE_CURRENT);
            DataHead DHo(SS_OtherData, 0, r->Name(), Tag());

            WrtOK&=WriteFile(H, &DHo, DHo.HeaderLength(), &nBytesWritten, NULL);
            FCB.InitialiseForWrite(H, StartPos+DHo.HeaderLength());

            if (WrtOK && r->EO_SaveOtherData(FCB))
              {
              EndPos=SetFilePointer(H, 0, NULL, FILE_CURRENT);
              DHo.SetEndOfData(StartPos, EndPos);
              SetFilePointer(H, StartPos, NULL, FILE_BEGIN);
              WrtOK&=WriteFile(H, &DHo, DHo.HeaderLength(), &nBytesWritten, NULL);
              SetFilePointer(H, EndPos, NULL, FILE_BEGIN);

              #if dbgExecutive
              if (dbgOtherData())
                dbgpln("%6i:%2i %3i %5i %6i %s:%s",StartPos, ((byte)DHo.eType)-((byte)SS_End), DHo.HeaderLength(), DHo.DataLength(), DHo.RecordLength(), DHo.XName(), DHo.ObjTag());
              #endif
              }
            else
              SetFilePointer(H, StartPos, NULL, FILE_BEGIN);
            }
          XM.Clear();
          }
        CDlgBusy::SetLine(3, "Saving Object : %i", nObjs);

        WrtOK&=SetEndOfFile(H);
        }
      }


    TagDB.StartSaveSequence("Conn");

    for (OInx=0; OInx<Objs_Reg.GetSize(); OInx++)
      {
      CExecObj* r=Objs_Reg[OInx];
      if (DoObj[OInx])
        {
        // double l1=0,l2=0;
        CXMsgLst XM;
        XM.Clear();
        Strng Tag;
        // l1+=GetTickCount();
        // dbgpln("Getting %g",l1);
        // dbgpln("Saving  %g",l2);
        int n=0;
        XM.Clear();
        while (r->EO_SaveConn(FCB, XM))
          {
          #if dbgExecutive
          if (dbgDBMsgs())
            XM.dbgDump((flag)dbgFullDBMsgs(), r->sName(), "SaveConn");
          #endif

          n++;
          if ((n%Min(n,10))==0)
            CDlgBusy::SetLine(3, "Saving Connection: %i", n);
          while (XM.MsgAvail())
            {
            CXM_ObjectData* pObjData=XM.ObjectData();
            TagDB.SaveObject("Conn", pObjData);
            }

          nCons++;
          XM.Clear();
          }
        CDlgBusy::SetLine(3, "Saving Connection: %i", nCons);
        r->EO_EndSave(FCB);
        }
      }

    CDlgBusy::SetLine(3, "Saved %i objects (and %i connections)", nObjs, nCons);

    SSFileRecType eType=SS_End;
    WrtOK&=WriteFile(H, &eType, sizeof(eType), &nBytesWritten, NULL);

    CloseHandle(H);

    TagDB.Flush(true);

    for (OInx=0; OInx<Objs_Reg.GetSize(); OInx++)
      Objs_Reg[OInx]->EO_SaveComplete(FCB);

    }
  else
    {
    LogError("Save DataBase", LF_Exclamation, "Cannot Open file \n'%s'\n%s", Fn(), FindWinError(GetLastError()));
    }
  //bSavingProject=0;

  CDlgBusy::SetLine(4, "Close database...");
  TagDB.Close();

  SaveTime.Stop();
  double SaveSecs = SaveTime.Secs();
  const long SaveMin = (long)floor(SaveSecs/60.0);
  LogNote("DataBase", 0, "%i Objects Saved (%d:%02d)", nObjs, SaveMin, (long)floor(SaveSecs-(60.0*(double)SaveMin)));

  return SvDBFmt;
  };

//---------------------------------------------------------------------------

int CExecutive::LoadDBase(int Pass, long DBFmt, DWORD DBOpts, char * pRoot)
  {
  LockExec();
  CDlgBusy::Open("", true);
  int Ret;
  switch (Pass)
    {
    case 1:
      Ret=DoLoadDBase1(DBFmt, DBOpts, pRoot);
      break;
    case 2:
      Ret=DoLoadDBase2(DBFmt, DBOpts, pRoot);
      break;
    }
  CDlgBusy::Close();
  UnlockExec();
  return Ret;
  }

//---------------------------------------------------------------------------

int CExecutive::DoLoadDBase1(long DBFmt, DWORD DBOpts, char * pRoot)
  {
  m_LDB.m_LoadTime.Start();

  m_LDB.m_RtFolder=pRoot;
  m_LDB.m_RtFolder.FnClearEndBSlash();
  m_LDB.m_RtFolder.FnCompactPath(50);

  CDlgBusy::SetLine(1, "Loading : %s", m_LDB.m_RtFolder());
  CDlgBusy::SetLine(3, "Open database...");
  CStopWatch OpenSW;
  OpenSW.Start();

  CTagDBase TagDB;
  int LdDBFmt=TagDB.Open(DBFmt, DBOpts, pRoot, NULL, NULL);
  if (LdDBFmt<0)
    {
    LogError("Load DataBase", LF_Exclamation, "Cannot Open Database");
    TagDB.Close();
    //CDlgBusy::Close();
    return LdDBFmt;
    }
  OpenSW.Stop();
  dbgpln("LoadDB Open time: %8.3g", OpenSW.Secs());

  FilingControlBlock FCB(pRoot, DefaultCfgLabel, TOF_DBase);

  //CArray <flag, flag> LoadIt;
  m_LDB.m_LoadIt.SetSize(Objs_Reg.GetSize());
  for (int OInx=0; OInx<Objs_Reg.GetSize(); OInx++)
    m_LDB.m_LoadIt[OInx]=Objs_Reg[OInx]->EO_BeginLoad(FCB);

  LockExec();
  SetHoldValidateData(true, false, true);

  TagDB.FindRevisionNo(DBRev_MostRecent);
  TagDB.StartLoadSequence("Node");

  CXMsgLst XM;
  CStopWatch Elapsed, LOSW, TLOSW;
  m_LDB.m_AllLoaded=true;
  m_LDB.m_Cancelled=false;
  int n=0;
  m_LDB.m_nObjs=0;
  for (OInx=0; !m_LDB.m_Cancelled && OInx<Objs_Reg.GetSize(); OInx++)
    {
    CExecObj * r=Objs_Reg[OInx];
    if (m_LDB.m_LoadIt[OInx])//r->EO_BeginLoad(FCB))
      {
      FCB.SetCreateObj(true);

      Elapsed.Start();
      TLOSW.Start();
      LOSW.Start();
      //OD.Clear();
      //OD.List.Clear();
      //OD.SetSize();
      CXM_ObjectData OD(0);
      while (!m_LDB.m_Cancelled && TagDB.LoadObject(&OD, true)>=0)
        {
        LOSW.Stop();
        XM.Clear();
        XM.SetOnHeap(false);
        XM.PackMsg(&OD);
        n++;
        if ((n%Min(n,10))==0)
          {
          CDlgBusy::SetLine(3, "Creating Object :%i" ,n);
          XFlushAppMsgQ();
          }
        m_LDB.m_Cancelled=CDlgBusy::IsCancelled();
        #if dbgExecutive
        if (dbgDBMsgs())
          XM.dbgDump((flag)dbgFullDBMsgs(), r->sName(), "Create");
        #endif

        if (!r->EO_LoadDefinedData(FCB, XM))
          m_LDB.m_AllLoaded=false;
        m_LDB.m_nObjs++;
        #if dbgStats
        //if ((n%10)==0)
          dbgpln("Create TmElapsed %i, %10.5f/Obj",n,Elapsed.Secs()/Max(1,n));
        #endif
        LOSW.Start();
        }
      TLOSW.Stop();
      LOSW.Stop();
      CDlgBusy::SetLine(3,"Creating Object :%i",n);

      dbgpln("EO_LoadDB %-15s Create    %8.3g = %8.5g/Obj [LO:%8.3f]",
             r->sName(),Elapsed.Secs(),Elapsed.Secs()/Max(1L,m_LDB.m_nObjs), LOSW.Secs());
      Elapsed.ReStart();

      FCB.SetCreateObj(false);
      }
    }

  if (m_LDB.m_AllLoaded)
    {
//    if (pCallBack)
//      (*pCallBack)(1, pCallData);
//    // In case Objs_Reg got bigger - drvmanager etc.
//    if (Objs_Reg.GetSize()>LoadIt.GetSize())
//      {
//      int i=LoadIt.GetSize();
//      LoadIt.SetSize(Objs_Reg.GetSize());
//      for ( ; i<Objs_Reg.GetSize(); i++)
//        LoadIt[i]=Objs_Reg[i]->EO_BeginLoad(FCB);
//      }

    TagDB.StartLoadSequence("Conn");
    for (OInx=0; !m_LDB.m_Cancelled && OInx<Objs_Reg.GetSize(); OInx++)
      {
      CExecObj *r=Objs_Reg[OInx];
      if (m_LDB.m_LoadIt[OInx])//r->EO_BeginLoad(FCB))
        {
        n=0;
        TLOSW.ReStart();
        LOSW.ReStart();
        CXM_ObjectData OD(0);
        while (!m_LDB.m_Cancelled && TagDB.LoadObject(&OD, false)>=0)
          {
          LOSW.Stop();
          XM.Clear();
          XM.SetOnHeap(false);
          if (XM.PackMsg(&OD))
            {
            n++;
            if ((n%Min(n,10))==0)
              {
              CDlgBusy::SetLine(3,"Creating Connection :%i",n);
              XFlushAppMsgQ();
              }
            m_LDB.m_Cancelled=CDlgBusy::IsCancelled();
            #if dbgExecutive
            if (dbgDBMsgs())
              XM.dbgDump((flag)dbgFullDBMsgs(), r->sName(), "Connect");
            #endif

            if (!r->EO_LoadConn(FCB, XM))
              m_LDB.m_AllLoaded=false;
            }
          LOSW.Start();
          }
        TLOSW.Stop();
        LOSW.Stop();
        CDlgBusy::SetLine(3, "Creating Connection :%i", n);

        dbgpln("EO_LoadDB %-15s Connect   %8.3g = %8.5g/Obj [LO:%8.3f]",r->sName(),Elapsed.Secs(),Elapsed.Secs()/Max(1,n), LOSW.Secs());
        Elapsed.ReStart();
        }
      }
    }
  if (m_LDB.m_AllLoaded)
    {
//    if (pCallBack)
//      (*pCallBack)(2, pCallData);
//    // In case Objs_Reg got bigger - drvmanager etc.
//    if (Objs_Reg.GetSize()>LoadIt.GetSize())
//      {
//      int i=LoadIt.GetSize();
//      LoadIt.SetSize(Objs_Reg.GetSize());
//      for ( ; i<Objs_Reg.GetSize(); i++)
//        LoadIt[i]=Objs_Reg[i]->EO_BeginLoad(FCB);
//      }

    TagDB.StartLoadSequence("Node");
    for (OInx=0; !m_LDB.m_Cancelled && OInx<Objs_Reg.GetSize(); OInx++)
      {
      CExecObj *r=Objs_Reg[OInx];
      if (m_LDB.m_LoadIt[OInx])//r->EO_BeginLoad(FCB))
        {
        n=0;
        TLOSW.ReStart();
//           TLOSW.Stop();
        LOSW.ReStart();
        CXM_ObjectData OD(0);
        while (!m_LDB.m_Cancelled && TagDB.LoadObject(&OD, false)>=0)
          {
          LOSW.Stop();
          XM.Clear();
          XM.SetOnHeap(false);
          if (XM.PackMsg(&OD))
            {
            n++;
            if ((n%Min(n,10))==0)
              {
              CDlgBusy::SetLine(3, "Loading Object :%i of %i", n, m_LDB.m_nObjs);
              XFlushAppMsgQ();
              }
//else CDlgBusy::SetLine(3, "Loading Object :%i of %i", n, m_LDB.m_nObjs);
            m_LDB.m_Cancelled=CDlgBusy::IsCancelled();
            #if dbgExecutive
            if (dbgDBMsgs())
              XM.dbgDump((flag)dbgFullDBMsgs(), r->sName(), "Load");
            CStopWatch SWxxx;
            #endif
            #if dbgStats
            SWxxx.Start();
            #endif
            TLOSW.Start();
            r->EO_LoadDefinedData(FCB, XM);
            TLOSW.Stop();
            #if dbgStats
            //if ((n%10)==0)
              dbgpln("Load Tm %10.6f TmElapsed %i, %10.5f/Obj", SWxxx.Secs(), n, Elapsed.Secs()/Max(1,n));
            #endif
            }
          LOSW.Start();
          }
        TLOSW.Stop();
        LOSW.Stop();
        //TLOSW.Stop();
        CDlgBusy::SetLine(3, "Loading Object :%i of %i", n, m_LDB.m_nObjs);

        dbgpln("EO_LoadDB %-15s Load      %8.3g = %8.5g/Obj [LO:%8.3f][TLO:%8.3f]",r->sName(), Elapsed.Secs(), Elapsed.Secs()/Max(1,n), LOSW.Secs(), TLOSW.Secs());
        }
      }
    }
  TagDB.Close();
  UnlockExec();
  m_LDB.m_LoadTime.Stop();
  return LdDBFmt;
  }

//---------------------------------------------------------------------------

int CExecutive::DoLoadDBase2(long DBFmt, DWORD DBOpts, char * pRoot)
  {
  CStopWatch LoadTime;
  LoadTime.Start();

  LockExec();

  //Strng RtFolder(pRoot);
  //RtFolder.FnClearEndBSlash();
  //RtFolder.FnCompactPath(50);
  FilingControlBlock FCB(pRoot, DefaultCfgLabel, TOF_DBase);

  CDlgBusy::SetLine(1, "Loading : %s", m_LDB.m_RtFolder());
  if (m_LDB.m_AllLoaded && !m_LDB.m_Cancelled)
    {
//    if (pCallBack)
//      (*pCallBack)(3, pCallData);

    // In case Objs_Reg got bigger - drvmanager etc.
    if (Objs_Reg.GetSize()>m_LDB.m_LoadIt.GetSize())
      {
      int i=m_LDB.m_LoadIt.GetSize();
      m_LDB.m_LoadIt.SetSize(Objs_Reg.GetSize());
      for ( ; i<Objs_Reg.GetSize(); i++)
        m_LDB.m_LoadIt[i]=Objs_Reg[i]->EO_BeginLoad(FCB);
      }

    Strng Fn;
    Fn.Set("%sOther.dat", pRoot);

    CDlgBusy::SetLine(3, "Loading Other Data");

    HANDLE H=CreateFile(Fn(), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (H!=INVALID_HANDLE_VALUE)
      {
      DWORD nBytesRead;
      CVersionRec VRRead, VRCurrent;
      ReadFile(H, &VRRead, VRRead.Length(), &nBytesRead, NULL);
      if ((VRRead.Length()!=nBytesRead) || (VRRead.Length()!=VRCurrent.Length()))
        {
        LogError("Load DataBase", LF_Exclamation, "Bad Data File\n'%s'", Fn());
        CloseHandle(H);
        //CDlgBusy::Close();
        return 0;
        }
      FCB.SetPrjFileVerNo(VRRead.PrjFileVerNo());

      for (;;)
        {
        DataHead DH;
        DWORD CurPos=SetFilePointer(H, 0, NULL, FILE_CURRENT);
        ReadFile(H, &DH, DH.FixedLength(), &nBytesRead, NULL);
        if (nBytesRead!=DH.FixedLength())
          ASSERT(nBytesRead>=sizeof(DH.eType));

        if (DH.eType==SS_End)
          break;
        else if (DH.eType!=SS_DefinedData && DH.eType!=SS_OtherData)
          {
          LogError("Executive", LF_Exclamation, "File format bad\n%s", Fn());
          break;
          }


        ReadFile(H, &DH.cObjTag, DH.ObjTagLength(), &nBytesRead, NULL);
        ASSERT(nBytesRead==DH.ObjTagLength());

        DataHeadChgd DHC(DH);
  //      Strng XName(DH.XName());

        if (PrjFileVerNo()<16 && DHC.XName.XStrICmp(CTagDBase::sOldFlwLibLocation)==0)
          {
          DHC.ChgXName(PlantModelTag);
          }

        #if dbgExecutive
        if (dbgOtherData())
          dbgpln("%6i:%2i %3i %5i %6i %s:%s",CurPos, ((byte)DH.eType)-((byte)SS_End), DH.HeaderLength(), DH.DataLength(), DH.RecordLength(), DHC.XName(), DHC.ObjTag());
        #endif

        for (int OInx=0; OInx<Objs_Reg.GetSize(); OInx++)
          if (stricmp(Objs_Reg[OInx]->Name(), DHC.XName())==0)
            break;

        if (Objs_Reg.GetSize()>m_LDB.m_LoadIt.GetSize())
          {
          int i=m_LDB.m_LoadIt.GetSize();
          m_LDB.m_LoadIt.SetSize(Objs_Reg.GetSize());
          for ( ; i<Objs_Reg.GetSize(); i++)
            m_LDB.m_LoadIt[i]=Objs_Reg[i]->EO_BeginLoad(FCB);
          }

        if (OInx<Objs_Reg.GetSize() && m_LDB.m_LoadIt[OInx])
          {
          FCB.SetObjTag(DHC.ObjTag());
          switch (DH.eType)
            {
            case SS_DefinedData:
              //Skip (if it occurs which it should not)
              break;
            case SS_OtherData:
              FCB.InitialiseForRead(H, CurPos+DH.HeaderLength(), CurPos+DH.RecordLength());
              SetFilePointer(H, CurPos+DH.HeaderLength(), NULL, FILE_BEGIN);
              Objs_Reg[OInx]->EO_LoadOtherData(FCB);
              break;
            default:
              VERIFY(0);
              break;
            }
          }
        SetFilePointer(H, CurPos+DH.RecordLength(), NULL, FILE_BEGIN);
        }

      CloseHandle(H);
      }
//    if (pCallBack)
//      (*pCallBack)(4, pCallData);
//
//    // In case Objs_Reg got bigger - drvmanager etc.
//    if (Objs_Reg.GetSize()>LoadIt.GetSize())
//      {
//      int i=LoadIt.GetSize();
//      LoadIt.SetSize(Objs_Reg.GetSize());
//      for ( ; i<Objs_Reg.GetSize(); i++)
//        LoadIt[i]=Objs_Reg[i]->EO_BeginLoad(FCB);
//      }
    }

  SetHoldValidateData(false, false, true);

  UnlockExec();

  for (int OInx=0; OInx<Objs_Reg.GetSize(); OInx++)
    if (OInx<m_LDB.m_LoadIt.GetSize() && m_LDB.m_LoadIt[OInx])
      Objs_Reg[OInx]->EO_EndLoad(FCB);


  for (OInx=0; OInx<Objs_Reg.GetSize(); OInx++)
    Objs_Reg[OInx]->EO_LoadComplete(FCB);

  //TagDB.Close();
  //bLoadingProject=0;
  LoadTime.Stop();
  double LoadSecs = m_LDB.m_LoadTime.Secs() + LoadTime.Secs();
  const long LoadMin = (long)floor(LoadSecs/60.0);
  LogNote("DataBase", 0, "%i Objects Loaded (%d:%02d)", m_LDB.m_nObjs, LoadMin, (long)floor(LoadSecs-(60.0*(double)LoadMin)));
  return 0;//LdDBFmt;
  }

//---------------------------------------------------------------------------

int CExecutive::SaveSnapShot(char * pRoot, char * pSSName, CExecObjArray *ObjList)
  {
  LockExec();
  CDlgBusy::Open("");
  int Ret=DoSaveSnapShotScenario(true, pRoot, pSSName, ObjList);
  CDlgBusy::Close();
  UnlockExec();
  return Ret;
  }

//---------------------------------------------------------------------------

int CExecutive::SaveScenario(char * pRoot, char * pSSName, CExecObjArray *ObjList)
  {
  LockExec();
  CDlgBusy::Open("");
  int Ret=DoSaveSnapShotScenario(false, pRoot, pSSName, ObjList);
  CDlgBusy::Close();
  UnlockExec();
  return Ret;
  }

//---------------------------------------------------------------------------

int CExecutive::DoSaveSnapShotScenario(flag DoSnapShot, char * pRoot, char * pSSName, CExecObjArray *ObjList)
  {
  CStopWatch SaveTime;
  SaveTime.Start();

  FilingControlBlock FCB(pRoot, pSSName, DoSnapShot ? TOF_SnapShot : TOF_Scenario);

  int nObjs=0;
  Strng Fn, Path(pSSName);
  if (Path.Length()==0)
    Fn.Set("%s%s", pRoot, pSSName);
  else
    Fn=pSSName;
  Strng FnD;
  FnD.FnContract(Fn());

  CExecObjArray &Objs = ObjList ? *ObjList : Objs_Reg;

//CDlgBusy::Open("");
  CDlgBusy::SetLine(1, "Saving %s : %s", DoSnapShot ? "Snapshot" : "Scenario", FnD());
  HANDLE H = CreateFile(Fn(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
  if (H!=INVALID_HANDLE_VALUE)
    {
    flag WrtOK=true;
    DWORD nBytesWritten;

    CVersionRec VR;
    WrtOK&=WriteFile(H, &VR, VR.Length(), &nBytesWritten, NULL);

    int n=0;
    for (int OInx=0; OInx<Objs.GetSize(); OInx++)
      {
      CExecObj *r=Objs[OInx];
      if (r->sName.Length()<=0)
        continue;

      FCB.SetXName(r->Name());
      if (r->EO_BeginSave(FCB))
        {
        Strng Tag;
        CXMsgLst XM;
        XM.Clear();

        while (WrtOK && r->EO_SaveDefinedData(FCB, Tag, XM))
          {

          n++;
          if ((n%Min(n,10))==0)
            {
            CDlgBusy::SetLine(3, "Saving Object :%i", n);
            XFlushAppMsgQ();
            }
          #if dbgExecutive
          if (dbgSnapMsgs())
            XM.dbgDump((flag)dbgFullSnapMsgs(), r->sName(), "Save");
          #endif
          //ASSERT(Tag.Length()>0);

          DWORD StartPos, EndPos;
          if (XM.MsgAvail() && r->sName.Length()>0)
            {
            CXM_ObjectData *pData=XM.ObjectData();

            DWORD ListLen=sizeof(ListLen);
            CPkDataList & List=pData->List;
            CPkDataIter Iter;
            for (CPkDataItem * pItem=List.FirstItem(Iter); pItem; pItem=List.NextItem(Iter))
              ListLen+=sizeof(pItem->m_wContents)+sizeof(pItem->m_wSize)+pItem->m_wSize;


            DataHead DHd(SS_DefinedData, ListLen/*pData->List.Size()*/, r->Name(), Tag());
            StartPos=SetFilePointer(H, 0, NULL, FILE_CURRENT);

            #if dbgExecutive
            if (dbgOtherData())
              dbgpln("%6i:%2i %3i %5i %6i %s:%s",StartPos, ((byte)DHd.eType)-((byte)SS_End), DHd.HeaderLength(), DHd.DataLength(), DHd.RecordLength(), DHd.XName(), DHd.ObjTag());
            #endif

            WrtOK&=WriteFile(H, &DHd, DHd.HeaderLength(), &nBytesWritten, NULL);

            WrtOK&=WriteFile(H, &ListLen, sizeof(ListLen), &nBytesWritten, NULL);

            for (CPkDataItem * pItem=List.FirstItem(Iter); pItem; pItem=List.NextItem(Iter))
              {
              word Sz=sizeof(pItem->m_wContents)+sizeof(pItem->m_wSize)+pItem->m_wSize;
              ASSERT(sizeof(Sz)==sizeof(pItem->m_wSize));
              WrtOK&=WriteFile(H, &pItem->m_wContents, sizeof(pItem->m_wContents), &nBytesWritten, NULL);
              WrtOK&=WriteFile(H, &Sz, sizeof(Sz), &nBytesWritten, NULL);
              WrtOK&=WriteFile(H, pItem->Buffer(), pItem->m_wSize, &nBytesWritten, NULL);

              //ListLen+=sizeof(pItem->m_wContents)+sizeof(pItem->m_wSize)+pItem->m_wSize;
              }

            //WrtOK&=WriteFile(H, &pData->List, pData->List.Size(), &nBytesWritten, NULL);

            #ifdef _DEBUG
            EndPos=SetFilePointer(H, 0, NULL, FILE_CURRENT);
            ASSERT(EndPos==StartPos+DHd.HeaderLength()+ListLen);
            ASSERT(EndPos==StartPos+DHd.RecordLength());
            #endif
            nObjs++;
            }

          if (Tag.Length()>0)
            {
            StartPos=SetFilePointer(H, 0, NULL, FILE_CURRENT);
            DataHead DHo(SS_OtherData, 0, r->Name(), Tag());

            WrtOK&=WriteFile(H, &DHo, DHo.HeaderLength(), &nBytesWritten, NULL);
            FCB.InitialiseForWrite(H, StartPos+DHo.HeaderLength());

            if (WrtOK && r->EO_SaveOtherData(FCB))
              {
              EndPos=SetFilePointer(H, 0, NULL, FILE_CURRENT);
              DHo.SetEndOfData(StartPos, EndPos);
              SetFilePointer(H, StartPos, NULL, FILE_BEGIN);
              WrtOK&=WriteFile(H, &DHo, DHo.HeaderLength(), &nBytesWritten, NULL);
              SetFilePointer(H, EndPos, NULL, FILE_BEGIN);

              #if dbgExecutive
              if (dbgOtherData())
                dbgpln("%6i:%2i %3i %5i %6i %s:%s",StartPos, ((byte)DHo.eType)-((byte)SS_End), DHo.HeaderLength(), DHo.DataLength(), DHo.RecordLength(), DHo.XName(), DHo.ObjTag());
              #endif

              }
            else
              SetFilePointer(H, StartPos, NULL, FILE_BEGIN);
            }
          XM.Clear();
          }
        WrtOK&=SetEndOfFile(H);

        r->EO_EndSave(FCB);
        if (!WrtOK)
          LogError(r->Name(), LF_Exclamation, "Writing %s File", DoSnapShot ? "Snapshot" : "Scenario");
        }
      }

    for (OInx=0; OInx<Objs.GetSize(); OInx++)
      Objs[OInx]->EO_SaveComplete(FCB);

    SSFileRecType eType=SS_End;
    WrtOK&=WriteFile(H, &eType, sizeof(eType), &nBytesWritten, NULL);

    CloseHandle(H);//HANDLE H=CreateFile(Fn(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
    }
  else
    LogError(DoSnapShot ? "Snapshot" : "Scenario", LF_Exclamation,
             "Cannot open file:\n%s\n%s", Fn(), FindWinError(GetLastError()));
  //CDlgBusy::Close();

  SaveTime.Stop();
  long SaveMin = (long)floor(SaveTime.Secs()/60.0);
  Fn=Fn.FnNameExt(FnD());
  LogNote(DoSnapShot ? "Snapshot" : "Scenario", 0, "%i Objects Saved to %s (%d:%02d)", nObjs, Fn(), SaveMin, (long)floor(SaveTime.Secs()-(60.0*(double)SaveMin)));

  AfxGetMainWnd()->PostMessage(WMU_CMDDONE, DoSnapShot ? ComCmd_SaveSnapshot : ComCmd_SaveScenario, 0); //let script cmd mngr know cmd is complete

  return nObjs;
  }

//---------------------------------------------------------------------------

int CExecutive::LoadSnapShot(char * pRoot, char * pSSName)
  {
  LockExec();
  CDlgBusy::Open("");
  int Ret=DoLoadSnapShotScenario(true, pRoot, pSSName);
  CDlgBusy::Close();
  UnlockExec();
  return Ret;
  }

//---------------------------------------------------------------------------

int CExecutive::LoadScenario(char * pRoot, char * pSSName)
  {
  LockExec();
  CDlgBusy::Open("");
  int Ret=DoLoadSnapShotScenario(false, pRoot, pSSName);
  CDlgBusy::Close();
  UnlockExec();
  return Ret;
  }

//---------------------------------------------------------------------------

int CExecutive::DoLoadSnapShotScenario(flag DoSnapShot, char * pRoot, char * pSSName)
  {
  CWaitCursor Wait;

  CStopWatch LoadTime;
  LoadTime.Start();

  FilingControlBlock FCB(pRoot, pSSName, DoSnapShot ? TOF_SnapShot : TOF_Scenario);

  int nObjs=0;
  Strng Fn, Path(pSSName);
  if (Path.Length()==0)
    Fn.Set("%s%s", pRoot, pSSName);
  else
    Fn=pSSName;

  Strng FnD;
  FnD.FnContract(Fn());

  //CDlgBusy::Open("");
  CDlgBusy::SetLine(1, "Loading %s : %s", DoSnapShot ? "Snapshot" : "Scenario", FnD());

  HANDLE H = CreateFile(Fn(), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
  if (H!=INVALID_HANDLE_VALUE)
    {
    DWORD nBytesRead;
    CVersionRec VRRead, VRCurrent;
    ReadFile(H, &VRRead, VRRead.Length(), &nBytesRead, NULL);
    if ((VRRead.Length()!=nBytesRead) || (VRRead.Length()!=VRCurrent.Length()))
      {
      LogError("Loading", LF_Exclamation, "Bad %s File", DoSnapShot ? "Snapshot" : "Scenario");
      CloseHandle(H);
      AfxGetMainWnd()->PostMessage(WMU_CMDDONE, DoSnapShot ? ComCmd_LoadSnapshot : ComCmd_LoadScenario, 0); //let script cmd mngr know cmd is complete
      return 0;
      }

    FCB.SetPrjFileVerNo(VRRead.PrjFileVerNo());

    CArray <flag, flag> LoadIt;
    LoadIt.SetSize(Objs_Reg.GetSize());
    for (int OInx=0; OInx<Objs_Reg.GetSize(); OInx++)
      LoadIt[OInx]=Objs_Reg[OInx]->EO_BeginLoad(FCB);

    SetHoldValidateData(true, true, true);

    int n=0;
    for (;;)
      {
      DataHead DH;

      DWORD CurPos=SetFilePointer(H, 0, NULL, FILE_CURRENT);
      ReadFile(H, &DH, DH.FixedLength(), &nBytesRead, NULL);
      if (nBytesRead!=DH.FixedLength())
        ASSERT(nBytesRead>=sizeof(DH.eType));

      if (DH.eType==SS_End)
        break;

      ReadFile(H, &DH.cObjTag, DH.ObjTagLength(), &nBytesRead, NULL);
      ASSERT(nBytesRead==DH.ObjTagLength());

      DataHeadChgd DHC(DH);

      #if dbgExecutive
      if (dbgOtherData())
        dbgpln("%6i:%2i %3i %5i %6i %s:%s",CurPos, ((byte)DH.eType)-((byte)SS_End), DH.HeaderLength(), DH.DataLength(), DH.RecordLength(), DHC.XName(), DHC.ObjTag());
      #endif

      for (int OInx=0; OInx<Objs_Reg.GetSize(); OInx++)
        if (stricmp(Objs_Reg[OInx]->Name(), DHC.XName())==0)
          break;

      if (OInx<Objs_Reg.GetSize() && LoadIt[OInx])
        {
        FCB.SetObjTag(DHC.ObjTag());
        switch (DH.eType)
          {
          case SS_DefinedData:
            {
            CXMsgLst XM;
            CXM_ObjectData *pOD=new CXM_ObjectData(0);

            //ReadFile(H, &pOD->List, DH.DataLength(), &nBytesRead, NULL);
            //ASSERT(nBytesRead==DH.DataLength());
            if (1)
              {

              DWORD ListLen;
              ReadFile(H, &ListLen, sizeof(ListLen), &nBytesRead, NULL);
              ASSERT(nBytesRead==sizeof(ListLen));
              ListLen-=sizeof(ListLen); // adjust

              CPkDataItemOld Buff; // space for one item
              DWORD HdSz=sizeof(Buff.m_wContents)+sizeof(Buff.m_wSize);
              for (DWORD iWord=0; iWord<ListLen; )
                {
                ReadFile(H, &Buff, HdSz, &nBytesRead, NULL);
                ASSERT(nBytesRead==HdSz);
                iWord+=nBytesRead;

                ReadFile(H, &Buff.m_cData[0], Buff.m_wSize-HdSz, &nBytesRead, NULL);
                ASSERT(nBytesRead==Buff.m_wSize-HdSz);
                iWord+=nBytesRead;

                //got the item
                pOD->List.AddTail(CPkDataItem::Create(Buff));

                }
              }

            XM.Clear();
            if (XM.PackMsg(pOD))
              {
              #if dbgExecutive
              if (dbgSnapMsgs())
                XM.dbgDump((flag)dbgFullSnapMsgs(), DHC.XName(), "LoadSnap");
              #endif

              n++;
              if ((n%Min(n,10))==0)
                {
                CDlgBusy::SetLine(3, "Loading Object :%i", n);
                XFlushAppMsgQ();
                }

              Objs_Reg[OInx]->EO_LoadDefinedData(FCB, XM);
              nObjs++;
              }
            else
              {
              int xxx=0;
              }

            }
            break;
          case SS_OtherData:
            FCB.InitialiseForRead(H, CurPos+DH.HeaderLength(), CurPos+DH.RecordLength());
            SetFilePointer(H, CurPos+DH.HeaderLength(), NULL, FILE_BEGIN);
            Objs_Reg[OInx]->EO_LoadOtherData(FCB);
            break;
          default:
            VERIFY(0);
            break;
          }
        }
      SetFilePointer(H, CurPos+DH.RecordLength(), NULL, FILE_BEGIN);
      }

    SetHoldValidateData(false, true, true);
    //TaggedObject::SetHoldValidateData(false);

    for (OInx=0; OInx<Objs_Reg.GetSize(); OInx++)
      if (LoadIt[OInx])
        Objs_Reg[OInx]->EO_EndLoad(FCB);

    for (OInx=0; OInx<Objs_Reg.GetSize(); OInx++)
      Objs_Reg[OInx]->EO_LoadComplete(FCB);

    CloseHandle(H);
    }
  else
    LogError(DoSnapShot ? "Snapshot" : "Scenario", LF_Exclamation, "Cannot open file\n:%s\n%s", Fn(), FindWinError(GetLastError()));

  //CDlgBusy::Close();

  LoadTime.Stop();
  const long LoadMin = (long)floor((LoadTime.Secs()+0.49)/60.0);
  Fn=Fn.FnNameExt(FnD());
  LogNote(DoSnapShot ? "Snapshot" : "Scenario", 0, "%i Objects Loaded from %s (%d:%02d)", nObjs, Fn(), LoadMin, (long)floor(LoadTime.Secs()+0.49-(60.0*(double)LoadMin)));

  AfxGetMainWnd()->PostMessage(WMU_CMDDONE, DoSnapShot ? ComCmd_LoadSnapshot : ComCmd_LoadScenario, 0); //let script cmd mngr know cmd is complete


  return nObjs;
  }

//---------------------------------------------------------------------------

const double ObjNotFoundErrVal = 1.0e14;
const double OneNANErrVal      = 1.0e13;
const double StringDiffErrVal  = 1.0e12;
const double NotFoundErrVal    = 1.0e11;

inline char * TextErrorValue(double Err)
  {
  if (Err==ObjNotFoundErrVal) return "Object Not Found";
  if (Err==OneNANErrVal     ) return "NAN";
  if (Err==StringDiffErrVal ) return "String";
  if (Err==NotFoundErrVal   ) return "Not Found";
  return "";
  }

//---------------------------------------------------------------------------

int CExecutive::IndexMessage(CPkDataList* pDList, int MsgNo, CCompareWorkSpace &WS)//, CDWordArray& Itms, CWordArray& Ind, CStringArray& Tags)
  {
  //const int GuessSize = (int)(pDList->Size()/22);
  //WS.Reset();
  //Itms.SetSize(GuessSize, 32);
  //Ind.SetSize(GuessSize, 32);
  int n = 0;
  WORD TagLevel = 0;
  WORD TagIndex = 0;
  CStringArray TagAtLevel;
  TagAtLevel.SetSize(1, 8);
  char* pFirstMsgByte = (char*)(pDList->FirstItem());
  CPkDataIter Iter;
  for (CPkDataItem* pItem=pDList->FirstItem(Iter); pItem; pItem=pDList->NextItem(Iter))
    {
    const byte Type = pItem->Type();
    if (!pItem->Empty() && pItem->Contains(PDI_Tag) && pItem->Contains(PDI_Value) && Type!=tt_Blob)
      {
      if (IsData(Type))
        {
        //if (n<GuessSize)
        //  {
        //  Itms[n] = (DWORD)((char*)pItem-pFirstMsgByte);
        //  Ind[n] = TagIndex;
        //  }
        //else
        //  {
          WS.ItmsAdd(MsgNo, (DWORD)((char*)pItem-pFirstMsgByte));
          WS.IndsAdd(MsgNo, TagIndex);
        //  }

/*        //CNM*/ dbgpln("  %3i %s", TagLevel, pItem->SymOrTag());
        n++;
        }
//      else if (Type==tt_Struct)
      else if (IsBlockStart(Type))
        {
        TagLevel++;
/*        //CNM*/ dbgpln("  %3i %s", TagLevel, pItem->SymOrTag());
        if (TagAtLevel.GetSize()<=TagLevel)
          TagAtLevel.SetSize(TagLevel+1);
        TagAtLevel.SetAt(TagLevel, TagAtLevel[TagLevel-1]);
        if (TagAtLevel[TagLevel].GetLength()>0)
          TagAtLevel[TagLevel]+=".";
        TagAtLevel[TagLevel]+=pItem->SymOrTag();
        WS.TagsAdd(TagAtLevel[TagLevel]);
//        TagIndex = Tags.GetSize()-1;
        TagIndex = WS.NTags()-1;
/*        //CNM*/ dbgpln("< %3i %s", TagLevel, (const char*)TagAtLevel[TagLevel]);
        }
//      else if (Type==tt_Struct_E)
      else if (IsBlockEnd(Type))
        {
        TagLevel--;
        if (TagLevel==0)
          TagIndex = 0;
        else
          TagIndex--;
/*        //CNM*/ dbgpln("> %3i ", TagLevel);
        }
      }
    }
  //Itms.SetSize(n);
  //Ind.SetSize(n);
  return n;
  }

//---------------------------------------------------------------------------

int CExecutive::CompareData(CCompareInfo& Info, CCompareWorkSpace &WS, char* pTag, CXMsgLst& XM1, CXMsgLst& XM2)
  {
  //XM1.dbgDump(true, "", "Compare1");
  //XM2.dbgDump(true, "", "Compare2");

  //LoadTimeDefined.Start();

  //go through each message and build index to individual tags...
  int n[2];
  char* pFirstMsgByte[2];
  WS.Reset();
  //CDWordArray Itms1;
  //CDWordArray Itms2;
  //CWordArray Ind1;
  //CWordArray Ind2;
  //CStringArray Tags;
  //Tags.SetSize(0, 32);
  WS.TagsAdd("");
  for (int i=0; i<2; i++)
    {
    CXMsgLst& XM = (i==0 ? XM1 : XM2);
    //CDWordArray& Itms = (i==0 ? Itms1 : Itms2);
    //CWordArray& Ind = (i==0 ? Ind1 : Ind2);
    if (XM.MsgAvail())
      {
      XM.RewindReadPosition();
ASSERT_ALWAYS(XM.MsgId()==XM_ObjectData, "Bad Message"); //what message is this ???
      CXM_ObjectData *pb = XM.ObjectData();

//pb->List.dbgDump(true, "");

      n[i] = IndexMessage(&(pb->List), i, WS);//, Itms, Ind, Tags);
      pFirstMsgByte[i] = (char*)(pb->List.FirstItem());
ASSERT_ALWAYS(!XM.MsgAvail(), "Too Many Messages"); //only expect one object in message!!!
      XM.RewindReadPosition();
      }
    }

  //for each tag in the first message, find corresponding tag in 2nd message and compare...
  long l1,l2;
  double d1,d2,Err;
  Strng s1,s2,TheTag;
  flag ShowStrings = (Info.bShowAllTags || Info.bShowStrings);
  const int n1 = n[0];
  const int n2 = n[1];
  const char* pFirstMsgByte1 = pFirstMsgByte[0];
  const char* pFirstMsgByte2 = pFirstMsgByte[1];
  CByteArray ItmsFound;
  ItmsFound.SetSize(n2);
  for (i=0; i<n2; i++)
    ItmsFound[i] = 0;
  int FindPos = 0;
  for (i=0; i<n1; i++)
    {
//    CPkDataItem* pItem1 = (CPkDataItem*)(pFirstMsgByte1 + Itms1[i]);
    CPkDataItem* pItem1 = (CPkDataItem*)(pFirstMsgByte1 + WS.Itm(0, i));
    const byte Type1 = pItem1->Type();
    if (IsData(Type1) && pItem1->Contains(PDI_Tag) && pItem1->Contains(PDI_Value) && (!IsStrng(Type1) || ShowStrings))
      {
      //find tag in 2nd message...
      Strng Tag1 = pItem1->SymOrTag();
      const int len = Tag1.Len();
      CPkDataItem* pItem2 = NULL;
      flag FoundTag = 0;
      int j = (FindPos<n2 ? FindPos : 0);
      const int LastPos = (FindPos>0 ? FindPos-1 : n2-1);
      while (j!=LastPos)
        {
        if (!ItmsFound[j])
          {
//          pItem2 = (CPkDataItem*)(pFirstMsgByte2 + Itms2[j]);
          pItem2 = (CPkDataItem*)(pFirstMsgByte2 + WS.Itm(1, j));
          if ((int)strlen(pItem2->SymOrTag())==len && strnicmp(Tag1(), pItem2->SymOrTag(), len)==0 && WS.Tag(WS.Ind(0, i))==WS.Tag(WS.Ind(1, j)))
            {
            ItmsFound[j] = 1;
            FindPos = j+1;
            FoundTag = 1;
            break;
            }
          }
        j++;
        if (j>=n2)
          j = 0;
        }

      //output results...
//      if (Ind1[i]==0)
//        TheTag.Set("%s.%s", pTag, Tag1());
//      else
//        TheTag.Set("%s.%s.%s", pTag, Tags[Ind1[i]], Tag1());
      TheTag.Set("%s.%s", WS.Tag(WS.Ind(0, i)), Tag1());
      //if (pItem1->Contains(PDI_CnvInfo))
      //  dbgpln(" Cnv:'%s'[%i]", pItem1->CnvTxt(), pItem1->CnvIndex()); there is no cnv info for tags!
      if (FoundTag)
        {
        if ((pItem1->Flags()&DDEF_NOCOMPARE)==0 && (pItem2->Flags()&DDEF_NOCOMPARE)==0)
          {
          flag AreDiff = 0;
          flag Show = 1;
          Err=0.0;
          if (IsIntData(Type1))
            {
            l1 = pItem1->Value()->GetLong();
            l2 = pItem2->Value()->GetLong();
            //AreDiff = (l1!=l2);
            AreDiff = !ConvergedVV((double)l1, (double)l2, Info.dAbsTolerance, Info.dRelTolerance, Err);
            Err*=Info.dRelTolerance;
            }
          else if (IsFloatData(Type1))
            {
            d1 = pItem1->Value()->GetDouble();
            d2 = pItem2->Value()->GetDouble();
            if (Valid(d1) && Valid(d2))
              {
              AreDiff = !ConvergedVV(d1, d2, Info.dAbsTolerance, Info.dRelTolerance, Err);
              Err*=Info.dRelTolerance;
              }
            else
              {
              AreDiff = (Valid(d1) || Valid(d2));
              Err = (AreDiff ? OneNANErrVal : 1.0);
              }
            }
          else if (IsStrng(Type1))
            {
            s1 = pItem1->Value()->GetString();
            s2 = pItem2->Value()->GetString();
            AreDiff = (s1!=s2);
            Err = (AreDiff ? StringDiffErrVal : 1.0);
            Show = (Info.bShowAllTags || Info.bShowStrings);
            }
          if (AreDiff)
            Info.iNoTagsDifferent++;
          if (Info.bShowAllTags || (AreDiff && Show))
            Info.AddErr(Err, TextErrorValue(Err), TheTag(), pItem1->Value()->GetString(), pItem2->Value()->GetString());
          #if dbgExecutive
          if (dbgCompare())
            dbgpln("%3i: %-28s : %s %12.3g '%20s' '%20s'", i, TheTag(), AreDiff ? "Y" : " ", Err, pItem1->Value()->GetString(), pItem2->Value()->GetString());
          #endif
          }
        else
          {
          // Flagged to skip compare
          }
        }
      else
        {
        Info.iNoTagsNotMatched++;
        if (Info.bShowAllTags || Info.bShowAllMissing)
          Info.AddErr(NotFoundErrVal, TextErrorValue(NotFoundErrVal), TheTag(), pItem1->Value()->GetString(), "----------");
        #if dbgExecutive
        if (dbgCompare())
          dbgpln("%3i: %-28s : Y %12.3g '%20s' '%20s'", i, TheTag(), NotFoundErrVal, pItem1->Value()->GetString(), "-------------------");
        #endif
        }
      }
    }

  //output results for all tags not used in 2nd message...
  for (i=0; i<n2; i++)
    {
    if (!ItmsFound[i])
      {
      CPkDataItem* pItem2 = (CPkDataItem*)(pFirstMsgByte2 + WS.Itm(1, i));
      const byte Type2 = pItem2->Type();
      if (IsData(Type2) && pItem2->Contains(PDI_Tag) && pItem2->Contains(PDI_Value) && (!IsStrng(Type2) || ShowStrings))
        {
        Info.iNoTagsNotMatched++;
        if (Info.bShowAllTags || Info.bShowAllMissing)
          {
          //if (Ind2[i]==0)
          //  TheTag.Set("%s.%s", pTag, pItem2->SymOrTag());
          //else
          //  TheTag.Set("%s.%s.%s", pTag, Tags[Ind2[i]], pItem2->SymOrTag());
          TheTag.Set("%s.%s", WS.Tag(WS.Ind(1, i)), pItem2->SymOrTag());
          Info.AddErr(NotFoundErrVal, TextErrorValue(NotFoundErrVal), TheTag(), "----------", pItem2->Value()->GetString());
          }
        #if dbgExecutive
        if (dbgCompare())
          {
          //if (Ind2[i]==0)
          //  TheTag.Set("%s.%s", pTag, pItem2->SymOrTag());
          //else
          //  TheTag.Set("%s.%s.%s", pTag, Tags[Ind2[i]], pItem2->SymOrTag());
          TheTag.Set("%s.%s", WS.Tag(WS.Ind(1, i)), pItem2->SymOrTag());
          dbgpln("%3i: %-28s : Y %12.3g '%20s' '%20s'", i, TheTag(), NotFoundErrVal, "-------------------", pItem2->Value()->GetString());
          }
        #endif
        }
      }
    }

  if (Info.bShowAllTags && !Info.bWindow && !Info.bShowSortedFile)
    Info.pOutF->WriteString("--------------------------------------------------\n");

  //LoadTimeDefined.Stop();
  return 0;
  }

//---------------------------------------------------------------------------

class CCompareScenarioHelper
  {
  protected:
    WORD wCmdID;
  public:
    CCompareScenarioHelper(WORD CmdID) { wCmdID = CmdID; };
    ~CCompareScenarioHelper() { AfxGetMainWnd()->PostMessage(WMU_CMDDONE, wCmdID, 0); }; //let script cmd mngr know cmd is complete
  };

int CExecutive::CompareScenarios(CCompareInfo& Info)
  {
  CWaitCursor Wait;

  WORD ID = Info.bCurrent ? ComCmd_CompareScenarioToCurrent : ComCmd_CompareScenarios;
  CCompareScenarioHelper CompScenHelper(ID); //destructor does work
  //initialize...
  CStopWatch CompareTime;
  CompareTime.Start();
  if (Info.bWindow || Info.bShowSortedFile)
    Info.bShowAllTags = 0; //meaningless to show all tags
  Info.iNoObjsCompared = 0;
  Info.iNoObjsNotFound = 0;
  Info.iNoTagsDifferent = 0;
  Info.iNoTagsNotMatched = 0;
  Info.List.SetSize(0, Info.iMaxCount);

  //build filenames...
  Strng Fn1, Path1(Info.sFilename1());
  if (Path1.Length()==0)
    Fn1.Set("%s%s", Info.pRoot, Info.sFilename1());
  else
    Fn1 = Info.sFilename1();
  Strng Fn2, Path2(Info.sFilename2());
  if (Path2.Length()==0)
    Fn2.Set("%s%s", Info.pRoot, Info.sFilename2());
  else
    Fn2 = Info.sFilename2();

  Strng Fn1D, Fn2D;
  Fn1D.FnContract(Fn1());
  Fn2D.FnContract(Fn2());

  Strng Dots;
  CDlgBusy::Open("\nComparing Scenarios");
  CDlgBusy::SetLine(3, Fn1());//Fn1D());
  CDlgBusy::SetLine(4, "and  %s", Info.bCurrent ? "Current Values" : Fn2());//Fn2D());

  //open and initialise report output file...
  CStdioFile OutF;
  if (!Info.bWindow)
    {
    UINT nOpenFlags = (Info.bResetOutFile ? CFile::modeCreate | CFile::modeWrite | CFile::typeText : CFile::modeWrite | CFile::typeText);
    CFileStatus State;
    if (CFile::GetStatus(Info.sOutFilename(), State)==0)
      {
      Strng NewDir;
      NewDir.FnDrivePath(Info.sOutFilename());
      if (NewDir.Len()>0)
        CreateDirectory(NewDir(), NULL);
      nOpenFlags |= CFile::modeCreate;
      }
    CFileException e;
    if (!OutF.Open(Info.sOutFilename(), nOpenFlags, &e))
      {
      LogError("Comparing", LF_Exclamation, "Cannot open output file %s. Error %d", Info.sOutFilename(), e.m_cause);
      CDlgBusy::Close();
      return -1;
      }
    if (Info.bResetOutFile)
      {
      Strng s;
      s.Set("\"Tag comparison generated by SysCAD %s, %s\"\n", VersionNo(), FullCopyrightNotice());
      OutF.WriteString(s());
      }
    else
      OutF.SeekToEnd();
    Info.pOutF = &OutF;
    Strng s,DtTm;
    s.Set("\n");
    OutF.WriteString(s());
    s.Set("'%s\tProject\t%s\n", CurDateTime(DtTm), PrjName());
    OutF.WriteString(s());
    s.Set("\tValues1\t%s\n", Fn1D());
    OutF.WriteString(s());
    s.Set("\tValues2\t%s\n", Info.bCurrent ? "Current" : Fn2D());
    OutF.WriteString(s());
    //s.Set("Tag\t%sValue1\tValue2\tError\n", (Info.bShowAllTags ? "Changed\t" : ""));
    //OutF.WriteString(s());
    Info.bListHeaderRqd=true;
    }

  //build lookup table of tag object positions in 2nd file...
  Dots += '.';
  CDlgBusy::SetLine(1, "Comparing Scenarios%s", Dots());
  CStringArray ObjTags;
  CDWordArray ObjInd;
  int FindPos = 0;
  HANDLE H2 = NULL;
  if (!Info.bCurrent)
    {
    H2 = CreateFile(Fn2(), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (H2!=INVALID_HANDLE_VALUE)
      {
      DWORD nBytesRead;
      CVersionRec VRRead, VRCurrent;
      ReadFile(H2, &VRRead, VRRead.Length(), &nBytesRead, NULL);
      if ((VRRead.Length()!=nBytesRead) || (VRRead.Length()!=VRCurrent.Length()))
        {
        LogError("Comparing", LF_Exclamation, "Bad File:%s", Fn2D());
        CloseHandle(H2);
        CDlgBusy::Close();
        return -2;
        }

      //FCB.SetPrjFileVerNo(VRRead.PrjFileVerNo());

      for (;;)
        {
        DataHead DH;
        DWORD CurPos = SetFilePointer(H2, 0, NULL, FILE_CURRENT);
        ReadFile(H2, &DH, DH.FixedLength(), &nBytesRead, NULL);
        if (nBytesRead!=DH.FixedLength())
          ASSERT(nBytesRead>=sizeof(DH.eType));

        if (DH.eType==SS_End)
          break;

        ReadFile(H2, &DH.cObjTag, DH.ObjTagLength(), &nBytesRead, NULL);
        ASSERT(nBytesRead==DH.ObjTagLength());

        DataHeadChgd DHC(DH);

        #if dbgExecutive
        if (dbgOtherData())
          dbgpln("%6i:%2i %3i %5i %6i %s:%s", CurPos, ((byte)DH.eType)-((byte)SS_End), DH.HeaderLength(), DH.DataLength(), DH.RecordLength(), DHC.XName(), DHC.ObjTag());
        #endif
        for (int OInx=0; OInx<Objs_Reg.GetSize(); OInx++)
          if (stricmp(Objs_Reg[OInx]->Name(), DHC.XName())==0)
            break;

        if (OInx<Objs_Reg.GetSize())
          {
          switch (DH.eType)
            {
            case SS_DefinedData:
              {
              ObjTags.Add(DHC.ObjTag());
              ObjInd.Add(CurPos);
              break;
              }
            case SS_OtherData:
              //FCB.InitialiseForRead(H2, CurPos+DH.HeaderLength(), CurPos+DH.RecordLength());
              //SetFilePointer(H1, CurPos+DH.HeaderLength(), NULL, FILE_BEGIN);
              //Objs_Reg[OInx]->EO_LoadOtherData(FCB);
              break;
            default:
              VERIFY(0);
              break;
            }
          }
        SetFilePointer(H2, CurPos+DH.RecordLength(), NULL, FILE_BEGIN);
        if ((ObjTags.GetSize()%200)==0)
          {
          Dots += '.';
          CDlgBusy::SetLine(1, "Comparing Scenarios%s", Dots());
          }
        }
      }
    else
      {
      LogError("Comparing", LF_Exclamation, "Cannot open snapshot/scenario file:%s", Fn2D());
      CDlgBusy::Close();
      return -3;
      }
    }
  const int nObjs2 = ObjTags.GetSize();

  //Go through 1st file (snapshot or scenario) load each object and compare it to the current
  //object in memory or another object loaded from another file...
  Dots += '.';
  CDlgBusy::SetLine(1, "Comparing Scenarios%s", Dots());
  HANDLE H1 = CreateFile(Fn1(), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
  if (H1!=INVALID_HANDLE_VALUE)
    {
    DWORD nBytesRead,nBytesRead2;
    CVersionRec VRRead, VRCurrent;
    ReadFile(H1, &VRRead, VRRead.Length(), &nBytesRead, NULL);
    if ((VRRead.Length()!=nBytesRead) || (VRRead.Length()!=VRCurrent.Length()))
      {
      LogError("Comparing", LF_Exclamation, "Bad File:%s", Fn1D());
      if (!Info.bWindow)
        OutF.Close();
      if (H2)
        CloseHandle(H2);
      CloseHandle(H1);
      CDlgBusy::Close();
      return -4;
      }

    //FCB.SetPrjFileVerNo(VRRead.PrjFileVerNo());

    CCompareWorkSpace WS;

    FilingControlBlock FCB(Info.pRoot, Info.sFilename1(), /*TOF_SnapShot*/TOF_Scenario);
    for (;;)
      {
      DataHead DH;
      DWORD CurPos = SetFilePointer(H1, 0, NULL, FILE_CURRENT);
      ReadFile(H1, &DH, DH.FixedLength(), &nBytesRead, NULL);
      if (nBytesRead!=DH.FixedLength())
        ASSERT(nBytesRead>=sizeof(DH.eType));

      if (DH.eType==SS_End)
        break;

      ReadFile(H1, &DH.cObjTag, DH.ObjTagLength(), &nBytesRead, NULL);
      ASSERT(nBytesRead==DH.ObjTagLength());

      DataHeadChgd DHC(DH);

      #if dbgExecutive
      if (dbgOtherData())
        dbgpln("%6i:%2i %3i %5i %6i %s:%s", CurPos, ((byte)DH.eType)-((byte)SS_End), DH.HeaderLength(), DH.DataLength(), DH.RecordLength(), DHC.XName(), DHC.ObjTag());
      #endif
      for (int OInx=0; OInx<Objs_Reg.GetSize(); OInx++)
        if (stricmp(Objs_Reg[OInx]->Name(), DHC.XName())==0)
          break;

      if (OInx<Objs_Reg.GetSize())
        {
        FCB.SetObjTag(DHC.ObjTag());
        switch (DH.eType)
          {
          case SS_DefinedData:
            {
            CXM_ObjectData *pOD1=new CXM_ObjectData(0);//CXM_ObjectData OD1(0);

            //ReadFile(H1, &pOD1->List, DH.DataLength(), &nBytesRead, NULL);
            //ASSERT(nBytesRead==DH.DataLength());
            if (1)
              {
              DWORD ListLen;
              ReadFile(H1, &ListLen, sizeof(ListLen), &nBytesRead, NULL);
              ASSERT(nBytesRead==sizeof(ListLen));
              ListLen-=sizeof(ListLen); // adjust

              CPkDataItemOld Buff; // space for one item
              DWORD HdSz=sizeof(Buff.m_wContents)+sizeof(Buff.m_wSize);
              for (DWORD iWord=0; iWord<ListLen; )
                {
                ReadFile(H1, &Buff, HdSz, &nBytesRead, NULL);
                ASSERT(nBytesRead==HdSz);
                iWord+=nBytesRead;

                ReadFile(H1, &Buff.m_cData[0], Buff.m_wSize-HdSz, &nBytesRead, NULL);
                ASSERT(nBytesRead==Buff.m_wSize-HdSz);
                iWord+=nBytesRead;

                //got the item
                pOD1->List.AddTail(CPkDataItem::Create(Buff));
                }
              }

            //pOD1->SetSize();
            CXMsgLst XM1;
            if (XM1.PackMsg(pOD1))
              {
              flag TagNotFoundErr = 0;
              if (Info.bCurrent)
                {
                //find object from memory...
                CXM_ObjectTag ObjTag(DHC.ObjTag(), TABOpt_ForScenario);
                CXM_ObjectData *pOD2=new CXM_ObjectData;//CXM_ObjectData OD2;
                CXM_Route Route;
                Objs_Reg[OInx]->LockXO();
                if (Objs_Reg[OInx]->EO_ReadTaggedItem(ObjTag, *pOD2, Route))
                  {
                  Objs_Reg[OInx]->UnlockXO();
                  CXMsgLst XM2;
                  if (XM2.PackMsg(pOD2))
                    {
                    #if dbgExecutive
                    if (dbgSnapMsgs())
                      {
                      //XM.dbgDump((flag)dbgFullSnapMsgs(), DH.XName(), "LoadSnap");
                      XM1.dbgDump(true, DHC.XName(), "Compare1");
                      XM2.dbgDump(true, DHC.XName(), "Compare2");
                      }
                    #endif
                    CompareData(Info, WS, DHC.ObjTag(), XM1, XM2);
                    Info.iNoObjsCompared++;
                    }
                  }
                else
                  {
                  Objs_Reg[OInx]->UnlockXO();
                  TagNotFoundErr = 1;
                  }
                }
              else
                {
                //find object from 2nd file...
                Strng Tag1 = DHC.ObjTag();
                const int len = Tag1.Len();
                flag FoundObjTag = 0;
                int j = (FindPos<nObjs2 ? FindPos : 0);
                const int LastPos = (FindPos>0 ? FindPos-1 : nObjs2-1);
                while (j!=LastPos)
                  {
                  if (ObjTags[j].GetLength()==len && strnicmp(Tag1(), (const char*)ObjTags[j], len)==0)
                    {
                    FindPos = j+1;
                    FoundObjTag = 1;
                    break;
                    }
                  j++;
                  if (j>=nObjs2)
                    j = 0;
                  }
                if (FoundObjTag)
                  {
                  //dbgpln("Tag:%s  at %d", (const char*)ObjTags[j], ObjInd[j]);
                  const DWORD APos = SetFilePointer(H2, ObjInd[j], NULL, FILE_BEGIN);
                  ASSERT(APos!=0xFFFFFFFF);
                  DataHead DH2;
                  ReadFile(H2, &DH2, DH2.FixedLength(), &nBytesRead2, NULL);
                  ASSERT(DH2.eType!=SS_End);
                  ReadFile(H2, &DH2.cObjTag, DH2.ObjTagLength(), &nBytesRead2, NULL);
                  ASSERT(nBytesRead2==DH2.ObjTagLength());
                  DataHeadChgd DHC2(DH2);
                  #if dbgExecutive
                  if (dbgOtherData())
                    dbgpln("%6i:%2i %3i %5i %6i %s:%s", APos, ((byte)DH2.eType)-((byte)SS_End), DH2.HeaderLength(), DH2.DataLength(), DH2.RecordLength(), DHC2.XName(), DHC2.ObjTag());
                  #endif
                  for (int OInx=0; OInx<Objs_Reg.GetSize(); OInx++)
                    if (stricmp(Objs_Reg[OInx]->Name(), DHC2.XName())==0)
                      break;
                  ASSERT(OInx<Objs_Reg.GetSize());
                  ASSERT(DH.eType==SS_DefinedData);
                  FilingControlBlock FCB2(Info.pRoot, Info.sFilename2(), /*TOF_SnapShot*/TOF_Scenario);
                  FCB2.SetObjTag(DHC2.ObjTag());
                  switch (DH.eType)
                    {
                    case SS_DefinedData:
                      {
                      CXM_ObjectData *pOD2=new CXM_ObjectData(0);//CXM_ObjectData OD2(0);
                      //ReadFile(H2, &pOD2->List, DH2.DataLength(), &nBytesRead2, NULL);
                      //ASSERT(nBytesRead2==DH2.DataLength());
                      if (1)
                        {
                        DWORD ListLen;
                        ReadFile(H2, &ListLen, sizeof(ListLen), &nBytesRead, NULL);
                        ASSERT(nBytesRead==sizeof(ListLen));
                        ListLen-=sizeof(ListLen); // adjust

                        CPkDataItemOld Buff; // space for one item
                        DWORD HdSz=sizeof(Buff.m_wContents)+sizeof(Buff.m_wSize);
                        for (DWORD iWord=0; iWord<ListLen; )
                          {
                          ReadFile(H2, &Buff, HdSz, &nBytesRead, NULL);
                          ASSERT(nBytesRead==HdSz);
                          iWord+=nBytesRead;

                          ReadFile(H2, &Buff.m_cData[0], Buff.m_wSize-HdSz, &nBytesRead, NULL);
                          ASSERT(nBytesRead==Buff.m_wSize-HdSz);
                          iWord+=nBytesRead;

                          //got the item
                          pOD2->List.AddTail(CPkDataItem::Create(Buff));
                          }
                        }

                      //pOD2->SetSize();
                      CXMsgLst XM2;
                      if (XM2.PackMsg(pOD2))
                        {
                        #if dbgExecutive
                        if (dbgSnapMsgs())
                          {
                          //XM.dbgDump((flag)dbgFullSnapMsgs(), DH.XName(), "LoadSnap");
                          XM1.dbgDump(true, DHC.XName(), "Compare1");
                          XM2.dbgDump(true, DHC.XName(), "Compare2");
                          }
                        #endif
                        CompareData(Info, WS, DHC2.ObjTag(), XM1, XM2);
                        Info.iNoObjsCompared++;
                        }
                      break;
                      }
                    case SS_OtherData: VERIFY(0); break;
                    default: VERIFY(0); break;
                    }
                  }
                else
                  TagNotFoundErr = 1;
                }
              if (TagNotFoundErr)
                {
                Info.iNoObjsNotFound++;
                Info.AddErr(ObjNotFoundErrVal, TextErrorValue(ObjNotFoundErrVal), DHC.ObjTag(), "----------", "----------");
                }
              }
            break;
            }
          case SS_OtherData:
            //FCB.InitialiseForRead(H1, CurPos+DH.HeaderLength(), CurPos+DH.RecordLength());
            //SetFilePointer(H1, CurPos+DH.HeaderLength(), NULL, FILE_BEGIN);
            //Objs_Reg[OInx]->EO_LoadOtherData(FCB);
            break;
          default:
            VERIFY(0);
            break;
          }
        }
      SetFilePointer(H1, CurPos+DH.RecordLength(), NULL, FILE_BEGIN);
      if ((Info.iNoObjsCompared%200)==0)
        {
        Dots += '.';
        CDlgBusy::SetLine(1, "Comparing Scenarios%s", Dots());
        }
      }

    CloseHandle(H1);
    }
  else
    LogError("Comparing", LF_Exclamation, "Cannot open file:\n%s\n%s", Fn1D(), FindWinError(GetLastError()));

  if (H2)
    CloseHandle(H2);
  Dots += '.';
  CDlgBusy::SetLine(1, "Comparing Scenarios%s", Dots());
  if (!Info.bWindow)
    {
    if (Info.bListHeaderRqd)
      {
      // NB Must Test
      Strng s;
      s.Set("Tag\t%sValue1\tValue2\tError\n", (Info.bShowAllTags ? "Changed\t" : ""));
      OutF.WriteString(s());
      Info.bListHeaderRqd=false;
      }
    if (Info.bShowSortedFile)
      {
      CCompResultLine ResLine;
      Strng s;
      for (int i=0; i<Info.List.GetSize(); i++)
        {
        ResLine = Info.List[i];
        s.Set("%s\t%s%s\t%s\t%g\n", ResLine.sTag(), (Info.bShowAllTags ? "\t" : ""),
          ResLine.sValue1(), ResLine.sValue2(), ResLine.dErr);
        OutF.WriteString(s());
        }
      }
    OutF.WriteString("\n\n");
    OutF.Close();
    }
  Info.pOutF = NULL;
  CDlgBusy::Close();

  CompareTime.Stop();
  const long CompareMin = (long)floor(CompareTime.Secs()/60.0);
  const long CompareSecs = (long)floor(CompareTime.Secs()-(60.0*(double)CompareMin));
  LogNote("Compare", 0, "%i Objects compared; time (%d:%02d)", Info.iNoObjsCompared, CompareMin, CompareSecs);
  return Info.iNoObjsCompared;
  }

//---------------------------------------------------------------------------

void CExecutive::PrepareMergeChangeTags(CMergeProjectsInfo& MPI)
  {//determine how to change tags to ensure all tags in new merged project are unique...
  CDlgBusy::SetLine(3, "Check projects for duplicate tags");

  //NBNB Assume that temp tags will always be unique!

  const char * TempTagBuff = "&$_TEMP_$&";
  CXM_ObjectData ObjData;
  CXM_Route Route;
  int i;
  Strng sTag,sNewTag,sTempTag;
  //1) Change all tags as per request...
  if (MPI.iMasterChangeType==MRG_ChangeDuplicates)
    {
    for (i=0; i<MPI.iSlaveTagCnt; i++)
      {
      sTag = MPI.SlaveTags[i];
      CXM_ObjectTag ObjTag(sTag(), TABOpt_Exists);
      Route.Clear();
      MPI.pFlwExec->LockXO();
      if (MPI.pFlwExec->EO_ReadTaggedItem(ObjTag, ObjData, Route))
        {
        MPI.pFlwExec->UnlockXO();
        MPI.PreChangeTags.Add(sTag());
        MPI.MakeNewTag(sTag(), true, sNewTag);
        MPI.PreNewTags.Add(sNewTag());
        sTempTag = TempTagBuff;
        sTempTag += sTag();
        MPI.PreTempTags.Add(sTempTag());
        if (MPI.iSlaveChangeType==MRG_ChangeDuplicates)
          {
          MPI.PostChangeTags.Add(sTag());
          MPI.MakeNewTag(sTag(), false, sNewTag);
          MPI.PostNewTags.Add(sNewTag());
          }
        }
      else
        MPI.pFlwExec->UnlockXO();
      }
    }
  else
    {
    MPI.PreChangeTags.SetSize(MPI.iMasterTagCnt);
    MPI.PreTempTags.SetSize(MPI.iMasterTagCnt);
    MPI.PreNewTags.SetSize(MPI.iMasterTagCnt);
    int Cnt = 0;
    RequestTagInfoRec ARqst(RQ_Tags);
    ARqst.sExcludeExecObj = pExecName_DrvMngr; //do NOT include driver tags
    ReplyTagInfoRec AInfo;
    while (RequestTagInfo(ARqst, AInfo))
      {
      if (stricmp(AInfo.sData(), MPI.sFlwExecName())!=0)
        {
        MPI.PreChangeTags[Cnt] = AInfo.sData;
        MPI.MakeNewTag(AInfo.sData(), true, sNewTag);
        MPI.PreNewTags[Cnt] = sNewTag;
        sTempTag = TempTagBuff;
        sTempTag += AInfo.sData();
        MPI.PreTempTags[Cnt] = sTempTag;
        Cnt++;
        }
      }
    }
  if (MPI.iSlaveChangeType==MRG_ChangeAllTags)
    {
    MPI.PostChangeTags.SetSize(MPI.iSlaveTagCnt);
    MPI.PostNewTags.SetSize(MPI.iSlaveTagCnt);
    for (i=0; i<MPI.iSlaveTagCnt; i++)
      {
      sTag = MPI.SlaveTags[i];
      MPI.PostChangeTags[i] = sTag;
      MPI.MakeNewTag(sTag(), false, sNewTag);
      MPI.PostNewTags[i] = sNewTag();
      }
    }
  else if (MPI.iSlaveChangeType==MRG_ChangeDuplicates && MPI.iMasterChangeType==MRG_ChangeNone)
    {
    for (i=0; i<MPI.iSlaveTagCnt; i++)
      {
      sTag = MPI.SlaveTags[i];
      CXM_ObjectTag ObjTag(sTag(), TABOpt_Exists);
      Route.Clear();
      MPI.pFlwExec->LockXO();
      DWORD Fnd = MPI.pFlwExec->EO_ReadTaggedItem(ObjTag, ObjData, Route);
      MPI.pFlwExec->UnlockXO();
      if (Fnd)
        {
        MPI.PostChangeTags.Add(sTag());
        MPI.MakeNewTag(sTag(), false, sNewTag);
        MPI.PostNewTags.Add(sNewTag());
        }
      }
    }

  //2) Determine if the merged project tags will be unique, AND
  //   if no tag name clashes will occur while renaming tags

  MPI.PreFlags.SetSize(MPI.PreNewTags.GetSize());
  MPI.PostFlags.SetSize(MPI.PostNewTags.GetSize());
  MPI.iMasterExtraChngCnt = 0;
  MPI.iSlaveExtraChngCnt = 0;
  //a) Ensure that the combination of all unchanged master tags and the new tags for
  //   the master project are unique...
  if (MPI.iMasterChangeType==MRG_ChangeNone)
    {
    for (i=0; i<MPI.PreNewTags.GetSize(); i++)
      MPI.PreFlags[i] = false;
    }
  else
    {
    for (i=0; i<MPI.PreNewTags.GetSize(); i++)
      {
      flag UniqueTag = false;
      flag RenameAgain = false;
      int j;
      while (!UniqueTag)
        {
        UniqueTag = true;
        if (MPI.iMasterChangeType==MRG_ChangeDuplicates)
          {
          CXM_ObjectTag ObjTag(MPI.PreNewTags[i](), TABOpt_Exists);
          Route.Clear();
          MPI.pFlwExec->LockXO();
          UniqueTag = !(MPI.pFlwExec->EO_ReadTaggedItem(ObjTag, ObjData, Route));
          MPI.pFlwExec->UnlockXO();
          if (!UniqueTag && MPI.PreChangeTags.Find(MPI.PreNewTags[i]())<0)
            UniqueTag = true;
          }
        if (UniqueTag)
          {
          j = MPI.PreNewTags.Find(MPI.PreNewTags[i]());
          if (j==i)
            j = MPI.PreNewTags.Find(MPI.PreNewTags[i](), false, i+1);
          if (j>=0)
            UniqueTag = false;
          }
        if (!UniqueTag)
          {
          MPI.PreNewTags[i] += MPI.sTagAppendChars();
          RenameAgain = true;
          }
        }
      MPI.PreFlags[i] = RenameAgain;
      if (RenameAgain)
        MPI.iMasterExtraChngCnt++;
      }
    }

  //b) Ensure none of the master tags (just before load slave) are the same as
  //   the slave tags (as they are loaded)...
  if (MPI.iMasterChangeType==MRG_ChangeDuplicates)
    {
    for (i=0; i<MPI.SlaveTags.GetSize(); i++)
      {
      CXM_ObjectTag ObjTag(MPI.SlaveTags[i](), TABOpt_Exists);
      Route.Clear();
      MPI.pFlwExec->LockXO();
      DWORD Fnd = MPI.pFlwExec->EO_ReadTaggedItem(ObjTag, ObjData, Route);
      MPI.pFlwExec->UnlockXO();
      if (Fnd && MPI.PreChangeTags.Find(MPI.SlaveTags[i]())<0)
        {
        MPI.PreChangeTags.Add(MPI.SlaveTags[i]());
        MPI.PreNewTags.Add(MPI.SlaveTags[i]());
        sTempTag = TempTagBuff;
        sTempTag += MPI.SlaveTags[i]();
        MPI.PreTempTags.Add(sTempTag());
        }
      }
    }

  //c) Ensure that the combination of all unchanged slave tags and the new tags for
  //   the slave project are unique...
  if (MPI.iSlaveChangeType==MRG_ChangeNone)
    {
    for (i=0; i<MPI.PostNewTags.GetSize(); i++)
      MPI.PostFlags[i] = false;
    }
  else
    {
    for (i=0; i<MPI.PostNewTags.GetSize(); i++)
      {
      flag UniqueTag = false;
      flag RenameAgain = false;
      int j;
      while (!UniqueTag)
        {
        UniqueTag = (MPI.SlaveTags.Find(MPI.PostNewTags[i]())<0);
        if (UniqueTag)
          {
          j = MPI.PostNewTags.Find(MPI.PostNewTags[i]());
          if (j==i)
            j = MPI.PostNewTags.Find(MPI.PostNewTags[i](), false, i+1);
          if (j>=0)
            UniqueTag = false;
          }
        if (!UniqueTag)
          {
          MPI.PostNewTags[i] += MPI.sTagAppendChars();
          RenameAgain = true;
          }
        }
      MPI.PostFlags[i] = RenameAgain;
      if (RenameAgain)
        MPI.iSlaveExtraChngCnt++;
      }
    }

  //This test is not esential, but improves the changes made to tags
  //d) Ensure that tags that are changed in master are not used in the slave project...
  if (MPI.iMasterChangeType!=MRG_ChangeNone)
    {
    for (i=0; i<MPI.PreNewTags.GetSize(); i++)
      {
      flag FirstTest = true;
      flag UniqueTag = false;
      flag RenameAgain = false;
      Strng TestTag = MPI.PreNewTags[i]();
      while (!UniqueTag)
        {
        UniqueTag = (MPI.PostNewTags.Find(TestTag())<0);
        if (UniqueTag)
          {
          if (MPI.iSlaveChangeType==MRG_ChangeDuplicates)
            {
            if (MPI.SlaveTags.Find(TestTag())>=0 && MPI.PostChangeTags.Find(TestTag())<0)
              UniqueTag = false;
            }
          else if (MPI.iSlaveChangeType==MRG_ChangeNone)
            {
            if (MPI.SlaveTags.Find(TestTag())>=0)
              UniqueTag = false;
            }
          }
        if (UniqueTag && !FirstTest)
          {
          int k = MPI.PreNewTags.Find(TestTag());
          if (k==i)
            k = MPI.PreNewTags.Find(TestTag(), false, i+1);
          if (k>=0)
            UniqueTag = false;
          if (UniqueTag && MPI.iSlaveChangeType!=MRG_ChangeAllTags)
            {
            CXM_ObjectTag ObjTag(TestTag(), TABOpt_Exists);
            Route.Clear();
            MPI.pFlwExec->LockXO();
            DWORD Fnd = MPI.pFlwExec->EO_ReadTaggedItem(ObjTag, ObjData, Route);
            MPI.pFlwExec->UnlockXO();
            if (Fnd && MPI.PreChangeTags.Find(TestTag())<0)
              UniqueTag = false;
            }
          }
        if (!UniqueTag)
          {
          MPI.PreNewTags[i] += MPI.sTagAppendChars();
          TestTag = MPI.PreNewTags[i]();
          RenameAgain = true;
          }
        FirstTest = false;
        }
      if (RenameAgain && !MPI.PreFlags[i])
        {
        MPI.PreFlags[i] = RenameAgain;
        MPI.iMasterExtraChngCnt++;
        }
      }
    }

  //e) Ensure that the final combination of tags is unique...
  for (i=0; i<MPI.SlaveTags.GetSize(); i++)
    {
    flag FirstTest = true;
    flag UniqueTag = false;
    flag RenameAgain = false;
    int j = MPI.PostChangeTags.Find(MPI.SlaveTags[i]());
    flag FromNew = (j>=0);
    Strng TestTag = (FromNew ? MPI.PostNewTags[j]() : MPI.SlaveTags[i]());
    while (!UniqueTag)
      {
      UniqueTag = (MPI.PreNewTags.Find(TestTag())<0);
      if (UniqueTag && MPI.iMasterChangeType==MRG_ChangeDuplicates)
        {
        CXM_ObjectTag ObjTag(TestTag(), TABOpt_Exists);
        Route.Clear();
        MPI.pFlwExec->LockXO();
        DWORD Fnd = MPI.pFlwExec->EO_ReadTaggedItem(ObjTag, ObjData, Route);
        MPI.pFlwExec->UnlockXO();
        if (Fnd && MPI.PreChangeTags.Find(TestTag())<0)
          UniqueTag = false;
        }
      if (UniqueTag && !FirstTest)
        {
        int k = MPI.PostNewTags.Find(TestTag());
        if (FromNew && k==j)
          k = MPI.PostNewTags.Find(TestTag(), false, j+1);
        if (k>=0)
          UniqueTag = false;
        if (UniqueTag && MPI.iSlaveChangeType!=MRG_ChangeAllTags)
          {
          k = MPI.SlaveTags.Find(TestTag());
          if (k>=0)
            UniqueTag = false;
          }
        }
      if (!UniqueTag)
        {
        if (!FromNew)
          {
          MPI.PostFlags.Add(0);
          j = MPI.PostChangeTags.Add(TestTag());
          MPI.PostNewTags.Add(TestTag());
          FromNew = true;
          }
        MPI.PostNewTags[j] += MPI.sTagAppendChars();
        TestTag = MPI.PostNewTags[j]();
        RenameAgain = true;
        }
      FirstTest = false;
      }
    if (RenameAgain && !MPI.PostFlags[i])
      {
      MPI.PostFlags[j] = RenameAgain;
      MPI.iSlaveExtraChngCnt++;
      }
    }
  }

//---------------------------------------------------------------------------

int CExecutive::CheckMerge(CMergeProjectsInfo& MPI)
  {
  CWaitCursor Wait;
  MPI.SlaveTags.SetSize(0);
  MPI.PreChangeTags.SetSize(0);
  MPI.PreTempTags.SetSize(0);
  MPI.PreNewTags.SetSize(0);
  MPI.PostChangeTags.SetSize(0);
  MPI.PostNewTags.SetSize(0);
  MPI.PreFlags.SetSize(0);
  MPI.PostFlags.SetSize(0);
  MPI.SlaveRefFiles.SetSize(0);
  MPI.SlaveRefModels.SetSize(0);
  MPI.PostChangeRefFiles.SetSize(0);
  MPI.PostNewRefFiles.SetSize(0);
  MPI.iProblemRefFiles = 0;
  MPI.iRefFilesUnknownCnt = 0;
  MPI.iRefFilesExistsCnt = 0;
  MPI.bIgnoreProbRefFiles = 0;
  MPI.sError = "";
  MPI.iErrorNo = 0;
  //check parameters...
  MPI.sFlwExecName = PlantModelTag;
  if (MPI.sLabel.Len()==0)
    MPI.sLabel = DefaultCfgLabel;
  TaggedObject::ValidateTagDot(MPI.sMasterPrefix);
  TaggedObject::ValidateTagDot(MPI.sSlavePrefix);
  TaggedObject::ValidateTagDot(MPI.sMasterSuffix);
  TaggedObject::ValidateTagDot(MPI.sSlaveSuffix);
  TaggedObject::ValidateTagDot(MPI.sTagAppendChars);
  if (MPI.sTagAppendChars.Len()==0)
    MPI.sTagAppendChars = "_";
  if (MPI.sMasterPrefix.Len()==0)
    MPI.sMasterPrefix = "C_";
  if (MPI.sSlavePrefix.Len()==0)
    MPI.sSlavePrefix = "I_";
  if (MPI.sMasterSuffix.Len()==0)
    MPI.sMasterSuffix = "_C";
  if (MPI.sSlaveSuffix.Len()==0)
    MPI.sSlaveSuffix = "_I";
  if (MPI.sSlavePrj.Len()==0)
    {
    MPI.sError = "A project to be imported must be specified";
    MPI.iErrorNo = -1;
    return MPI.iErrorNo;
    }
  if (stricmp(PrjName(), MPI.sSlavePrj())==0)
    {
    MPI.sError = "Cannot merge project onto itself";
    MPI.iErrorNo = -2;
    return MPI.iErrorNo;
    }
  CFileStatus State;
  if (CFile::GetStatus(MPI.sSlavePrj(), State)==0)
    {
    MPI.sError.Set("Cannot find import project '%s'", MPI.sSlavePrj());
    MPI.iErrorNo = -3;
    return MPI.iErrorNo;
    }
  Strng RDLw(MPI.sSlavePrj());
  RDLw.Lower();
  char* p = strstr(RDLw(), ".spj");
  if (p==NULL)
    {
    MPI.sError.Set("Expect SysCAD spj file '%s'", MPI.sSlavePrj());
    MPI.iErrorNo = -4;
    return MPI.iErrorNo;
    }
  //MPI.sSlavePrjDir = MPI.sSlavePrj.Left(MPI.sSlavePrj.Len()-4);
  //MPI.sSlavePrjDir += "\\";
  MPI.sSlavePrjDir = MPI.sSlavePrj.Left(MPI.sSlavePrj.ReverseFind('\\')+1);

  //check slave project for compatability...
  CProfINIFile PF(MPI.sSlavePrj());
  if (PF.RdInt("General", "PrjFileVersion", 0)<13)
    {
    MPI.sError.Set("Import project version number is old. To upgrade, load and save it (%s)", MPI.sSlavePrj());
    MPI.iErrorNo = -5;
    return MPI.iErrorNo;
    }
  MPI.sSlaveModelCfg = PF.RdStr("General", "ModelConfiguration", "");
  MPI.sSlaveModelCfg = PF.RdStr("General", "CfgFile", MPI.sSlaveModelCfg()); // Incase newer version
  //if (MPI.sSlaveModelCfg.Len()==0 || stricmp(MPI.sMasterModelCfg(), MPI.sSlaveModelCfg())!=0)
  //  {
  Strng Fn1(MPI.sSlaveModelCfg), Fn2(MPI.sMasterModelCfg());
  if (Fn1())
    Fn1.FnNameExt();
  Fn2.FnNameExt();
  if (Fn1.Len()==0 || stricmp(Fn1(), Fn2())!=0)
    {
    Strng S;
    S.Set("Import project model configuration does not match current project.\n\n"
          "Current:%s\n"
          "Import:%s\n\n"
          "Continue ?",
          MPI.sMasterModelCfg()?MPI.sMasterModelCfg():"",
          MPI.sSlaveModelCfg()?MPI.sSlaveModelCfg():"");
    if (AfxMessageBox(S(), MB_YESNO|MB_ICONQUESTION)==IDNO)
      {
      MPI.sError.Set("Import project model configuration does not match current project. (Current:%s  Import:%s)", MPI.sMasterModelCfg(), MPI.sSlaveModelCfg());
      MPI.iErrorNo = -6;
      return MPI.iErrorNo;
      }
    }
  //}

  //find flow solver executive object...
  MPI.pFlwExec = NULL;
  for (int OInx=0; OInx<Objs_Reg.GetSize(); OInx++)
    if (stricmp(Objs_Reg[OInx]->Name(), MPI.sFlwExecName())==0)
      {
      MPI.pFlwExec = Objs_Reg[OInx];
      break;
      }
  if (MPI.pFlwExec==NULL)
    {
    MPI.sError.Set("Cannot find flow solver engine '%s'", MPI.sFlwExecName());
    MPI.iErrorNo = -7;
    return MPI.iErrorNo;
    }
  return MPI.iErrorNo;
  }

//---------------------------------------------------------------------------

int CExecutive::PrepareMerge(CMergeProjectsInfo& MPI)
  {
  CWaitCursor Wait;
  MPI.sError = "";
  MPI.iErrorNo = 0;

  //open and initialise report output file...
  CDlgBusy::SetLine(3, "Open report file");
  CFileStatus State;
  if (CFile::GetStatus(MPI.sReportFile(), State)==0)
    {
    Strng NewDir;
    NewDir.FnDrivePath(MPI.sReportFile());
    if (NewDir.Len()>0)
      CreateDirectory(NewDir(), NULL);
    }
  CStdioFile OutF;
  CFileException e;
  if (!OutF.Open(MPI.sReportFile(), CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::typeText, &e))
    {
    MPI.sError.Set("Cannot open report file %s. Error %d", MPI.sReportFile(), e.m_cause);
    MPI.iErrorNo = -8;
    return MPI.iErrorNo;
    }

  //build slave trend and graphics document lists...
  CDlgBusy::SetLine(3, "Build and check imported project\ntrend and graphics file lists");
  CProfINIFile PF(MPI.sSlavePrj());
  int i;
  for (int j=0; j<3; j++)
    {
    Strng Section = (j==0 ? "GraphicsDocuments" : (j==1 ? "TrendDocuments" : "CtrlTrendDocuments"));
    //Strng DocTypeName = (j==0 ? "Graphics" : "Trend");
    CSVector& Lst = (j==0 ? MPI.SlaveGraphics : MPI.SlaveTrends);
    const int DocCount = PF.RdInt(Section(), "DocumentCount", 0);
    int Offset = (j==2 ? Lst.GetSize() : 0);
    Lst.SetSize(Offset+DocCount);
    char Entry[256];
    for (i=0; i<DocCount; i++)
      {
      Strng sDoc;
      sprintf(Entry, "Doc%i", i);
      sDoc = PF.RdStr(Section(), Entry, "");
      if (sDoc.Len()>1 && sDoc[0]=='.' && sDoc[1]=='\\')
        sDoc = sDoc.Right(sDoc.Len()-2);
      if (sDoc.ReverseFind('\\')>=0)
        sDoc = sDoc.Right(sDoc.Len()-sDoc.ReverseFind('\\')-1); //remove $PrjFiles
      Lst[Offset+i] = MPI.sSlavePrjDir();
      Lst[Offset+i] += sDoc();
      }
    }
  //check for clashes in trend and graphics names...
  for (j=0; j<2; j++)
    {
    CSVector& MasterLst = (j==0 ? MPI.MasterGraphics : MPI.MasterTrends);
    CSVector& ChangeLst = (j==0 ? MPI.PostChangeGraphics : MPI.PostChangeTrends);
    CSVector& NewLst = (j==0 ? MPI.PostNewGraphics : MPI.PostNewTrends);
    ChangeLst.SetSize(0);
    NewLst.SetSize(0);
    CSVector MasterFileNames;
    MasterFileNames.SetSize(MasterLst.GetSize());
    int i;
    for (i=0; i<MasterLst.GetSize(); i++)
      MasterFileNames[i].FnNameExt(MasterLst[i]());
    CSVector& SlaveLst = (j==0 ? MPI.SlaveGraphics : MPI.SlaveTrends);
    CSVector SlaveFileNames;
    SlaveFileNames.SetSize(SlaveLst.GetSize());
    for (i=0; i<SlaveLst.GetSize(); i++)
      SlaveFileNames[i].FnNameExt(SlaveLst[i]());
    for (i=0; i<SlaveFileNames.GetSize(); i++)
      {
      Strng FullFile;
      WIN32_FIND_DATA fd;
      HANDLE H;
      flag Found = (MasterFileNames.Find(SlaveFileNames[i]())>=0);
      if (!Found)
        {
        FullFile.Set("%s%s", PrjFiles(), SlaveFileNames[i]());
        H = FindFirstFile(FullFile(), &fd);
        Found = (H!=INVALID_HANDLE_VALUE);
        FindClose(H);
        }
      if (Found)
        {//duplicate name, find unique alternative...
        Strng Name, s, PrevExt;
        PrevExt = PrevExt.FnExt(SlaveFileNames[i]());
        if (j==0)
          Name = "Grf_";
        else
          Name = ((PrevExt.XStrICmp(".scc")==0) ? "Controls_" : "Trend_");
        flag FoundNewName = 0;
        int Cnt = 1;
        while (!FoundNewName)
          {
          s.Set("%s%d%s", Name(), Cnt, PrevExt());
          FullFile.Set("%s%s", PrjFiles(), s());
          if (MasterFileNames.Find(s())<0 && SlaveFileNames.Find(s(), false, i+1)<0)
            {
            H = FindFirstFile(FullFile(), &fd);
            if (H==INVALID_HANDLE_VALUE)
              FoundNewName = 1;
            FindClose(H);
            }
          Cnt++;
          }
        ChangeLst.Add(SlaveLst[i]());
        NewLst.Add(FullFile());
        MasterFileNames.Add(s());
        }
      else
        {
        MasterFileNames.Add(SlaveFileNames[i]());
        }
      }
    }
  CDlgBusy::SetLine(4, "");

  //build slave referenced files list...
  CDlgBusy::SetLine(3, "Build imported referenced files list");
  Strng RefFile,RefModel;
  int RefCnt = 0;
  char Entry[256];
  sprintf(Entry, "File%i", RefCnt);
  RefFile = PF.RdStr("Refrenced_Files", Entry, "");
  while (RefFile.Len()>0)
    {
    sprintf(Entry, "Model%i", RefCnt);
    RefModel = PF.RdStr("Refrenced_Files", Entry, "");
    MPI.SlaveRefFiles.Add(RefFile());
    MPI.SlaveRefModels.Add(RefModel());
    RefCnt++;
    sprintf(Entry, "File%i", RefCnt);
    RefFile = PF.RdStr("Refrenced_Files", Entry, "");
    }
  //check for clashes in referenced files
  Strng s,ss;
  CSVector MasterFilenames;
  MasterFilenames.SetSize(MPI.MasterRefFiles.GetSize());
  //CByteArray MasterPrjDir;
  //MasterPrjDir.SetSize(MPI.MasterRefFiles.GetSize());
  const int PrjFilesAliasLen = strlen(PrjFilesAlias());
  for (i=0; i<MPI.MasterRefFiles.GetSize(); i++)
    {
    if (MPI.MasterRefFiles[i].Len()>PrjFilesAliasLen && MPI.MasterRefFiles[i][PrjFilesAliasLen]!='~'  && strnicmp(PrjFilesAlias(), MPI.MasterRefFiles[i](), PrjFilesAliasLen)==0)
      s = MPI.MasterRefFiles[i].Right(MPI.MasterRefFiles[i].Len()-PrjFilesAliasLen);
    else
      s.FnNameExt(MPI.MasterRefFiles[i]());
    MasterFilenames[i] = s();
    //ss.FnDrivePath(MPI.MasterRefFiles[i]());
    //MasterPrjDir[i] = (stricmp(ss(), PrjFiles())==0);
    }
  for (i=0; i<MPI.SlaveRefFiles.GetSize(); i++)
    {
    s.FnNameExt(MPI.SlaveRefFiles[i]());
    ss.FnDrivePath(MPI.SlaveRefFiles[i]());
    if (ss.XStrICmp(PrjFilesAlias())==0)
      {
      ss=MPI.sSlavePrjDir();
      MPI.SlaveRefFiles[i]=ss;
      MPI.SlaveRefFiles[i]+=s;
      }
    else if (ss.Len()>PrjFilesAliasLen && ss[PrjFilesAliasLen]!='~' && strnicmp(PrjFilesAlias(), ss(), PrjFilesAliasLen)==0)
      {
      s=ss.Right(ss.Len()-PrjFilesAliasLen);
      s+=ss.FnNameExt(MPI.SlaveRefFiles[i]());
      ss=MPI.sSlavePrjDir();
      MPI.SlaveRefFiles[i]=ss;
      MPI.SlaveRefFiles[i]+=s;
      }

    //const int SlavePrjDirLen = MPI.sSlavePrjDir.Len();
    //if (ss.Len()>=SlavePrjDirLen && strnicmp(ss(), MPI.sSlavePrjDir(), SlavePrjDirLen)==0 && MPI.PostChangeRefFiles.Find(MPI.SlaveRefFiles[i]())<0)
    if (stricmp(ss(), MPI.sSlavePrjDir())==0 && MPI.PostChangeRefFiles.Find(MPI.SlaveRefFiles[i]())<0)
      {
      MPI.PostChangeRefFiles.Add(MPI.SlaveRefFiles[i]());
      int index = MasterFilenames.Find(s());
      if (index>=0 && stricmp(MPI.MasterRefFiles[index](), MPI.SlaveRefFiles[i]())!=0)
        {
        MPI.iProblemRefFiles++;
        MPI.iRefFilesUnknownCnt++;
        ss.Set("Unknown");//"%s%s", PrjFiles(), s());
        }
      else
        {
        ss.Set("%s%s", PrjFiles(), s());
        if (CFile::GetStatus(ss(), State))
          {
          MPI.iProblemRefFiles++;
          MPI.iRefFilesExistsCnt++;
          ss.Set("Exists");
          }
        }
      MPI.PostNewRefFiles.Add(ss());
      }
    }

  //build list of all slave project tags from slave data base...
  CDlgBusy::SetLine(3, "Build imported tag list");
  //TagDBase* pTDB = new TagDBase;
  //TagDBase& TDB = *pTDB;
  CTagDBase TDB;
  //TDB.Open(false, MPI.sSlavePrjDir(), NULL, NULL);
  TDB.Open(DBConnect_Find, 0, MPI.sSlavePrjDir(), NULL, NULL);
  CXM_ObjectData OD(0);
  MPI.iSlaveTagCnt = 0;
  MPI.SlaveTags.SetSize(64);
  TDB.FindRevisionNo(DBRev_MostRecent);
  TDB.StartLoadSequence("Node");
  while (TDB.LoadObject(&OD, true)>=0)
    {
    //OD.List.dbgDump(true, "");
    CPkDataItem* pItem = OD.List.FirstItem(); //only need first item...
    if (pItem)
      {
      const byte Type = pItem->Type();
      if (!pItem->Empty() && pItem->Contains(PDI_Tag) && pItem->Contains(PDI_Value)
          && Type!=tt_Blob && stricmp(pItem->SymOrTag(), MPI.sFlwExecName())!=0)
        {
        dbgpln("Found tag:%s   Value:%s   Type:%s  Class:%s", pItem->SymOrTag(), pItem->Value()->GetString(), tt_TypeString(Type), pItem->Class()?pItem->Class():"");
        char* pClass=pItem->Class();
        if (pClass && (
          stricmp(pClass, "FS_PB")==0 ||
          stricmp(pClass, "FS_Dyn")==0 ||
          stricmp(pClass, "FS_SDB")==0 ||
          stricmp(pClass, "FS_Tears")==0 ||
          stricmp(pClass, "FS_Chgs")==0 ||
          stricmp(pClass, "FS_Tols")==0 ||
          stricmp(pClass, "FS_Ctrl")==0 ||
          stricmp(pClass, "FS_Order")==0 ||
          stricmp(pClass, "FS_Pwr")==0 ||
          stricmp(pClass, "FS_IO")==0 ||
          stricmp(pClass, "TbFlange")==0 ||
          stricmp(pClass, "TbPID2")==0 ||
          stricmp(pClass, "TbPID")==0 ||
          stricmp(pClass, "TbFlshTrn")==0 ||
          stricmp(pClass, "Area")==0) )
          {
          // Skip It
          }
        else
          {
          if (MPI.iSlaveTagCnt>=MPI.SlaveTags.GetSize())
            MPI.SlaveTags.SetSize(MPI.SlaveTags.GetSize()+32);
          MPI.SlaveTags[MPI.iSlaveTagCnt] = pItem->SymOrTag();
          MPI.iSlaveTagCnt++;
          }
        }
      }
    }
  MPI.SlaveTags.SetSize(MPI.iSlaveTagCnt);
  TDB.Close();
  //delete pTDB;
  //pTDB = NULL;

  //calc no of tags in master...
  MPI.iMasterTagCnt = 0;
  RequestTagInfoRec Rqst(RQ_TagCount);
  Rqst.sExcludeExecObj = pExecName_DrvMngr; //do NOT include driver tags
  ReplyTagInfoRec Info;
  while (RequestTagInfo(Rqst, Info))
    MPI.iMasterTagCnt += Info.dwData;
  MPI.iMasterTagCnt--; //do not include flwsolve tag

  //determine how to change tags to ensure all tags in new merged project are unique...
  PrepareMergeChangeTags(MPI);

  //determine if allowed to continue with potential referenced files errors...
  if (MPI.iErrorNo==0 && MPI.iRefFilesUnknownCnt>0)
    {
    ss = "Referenced files with same name in both projects:\n  ";
    for (i=0; i<MPI.PostChangeRefFiles.GetSize(); i++)
      {
      if (stricmp(MPI.PostNewRefFiles[i](), "Unknown")==0)
        {
        ss += MPI.PostChangeRefFiles[i]();
        ss += "\n  ";
        }
      }
    ss += "\nMust these files be ignored and NOT copied?\n";
    if (AfxMessageBox(ss(), MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2)==IDYES)
      MPI.bIgnoreProbRefFiles = 1;
    }

  //generate report output file...
  CDlgBusy::SetLine(3, "Generate merge report file");
  Strng s1,s2,DtTm;
  s.Set("'%s\n", CurDateTime(DtTm));
  OutF.WriteString(s());
  s.Set("Project merge report generated by SysCAD %s, %s\n\n", VersionNo(), FullCopyrightNotice());
  OutF.WriteString(s());
  s.Set("Current project:%s\n", PrjName());
  OutF.WriteString(s());
  s.Set("Import project:%s\n", MPI.sSlavePrj());
  OutF.WriteString(s());
  OutF.WriteString("\n,Current,Import\n");
  s1.FnNameExt(PrjName());
  s2.FnNameExt(MPI.sSlavePrj());
  s.Set("Project:,%s,%s\n", s1(), s2());
  OutF.WriteString(s());
  s.Set("Number of objects:,%d,%d\n", MPI.iMasterTagCnt, MPI.iSlaveTagCnt);
  OutF.WriteString(s());
  s.Set("Number of tags to be changed:,%d,%d\n", MPI.NoPreChangeTags(), MPI.NoPostChangeTags());
  OutF.WriteString(s());
  s.Set("Number of changed tags requiring additional characters:,%d,%d\n", MPI.iMasterExtraChngCnt, MPI.iSlaveExtraChngCnt);
  OutF.WriteString(s());
  s.Set("Number of trend windows:,%d,%d\n", MPI.MasterTrends.GetSize(), MPI.SlaveTrends.GetSize());
  OutF.WriteString(s());
  s.Set("Number of graphics windows:,%d,%d\n", MPI.MasterGraphics.GetSize(), MPI.SlaveGraphics.GetSize());
  OutF.WriteString(s());
  s.Set("Number of trend windows to be changed:,0,%d\n", MPI.PostChangeTrends.GetSize());
  OutF.WriteString(s());
  s.Set("Number of graphics windows to be changed:,0,%d\n", MPI.PostChangeGraphics.GetSize());
  OutF.WriteString(s());
  if (MPI.PreNewTags.GetSize()>0)
    {
    OutF.WriteString("\nTags changed in current project:\n");
    for (int i=0; i<MPI.PreNewTags.GetSize(); i++)
      {
      s.Set("%s,%s,%s\n", MPI.PreChangeTags[i](), MPI.PreNewTags[i](), MPI.PreFlags[i] ? "Extra '_' appended" : "");
      OutF.WriteString(s());
      }
    }
  if (MPI.PostNewTags.GetSize()>0)
    {
    OutF.WriteString("\nTags changed in import project:\n");
    for (i=0; i<MPI.PostNewTags.GetSize(); i++)
      {
      s.Set("%s,%s,%s\n", MPI.PostChangeTags[i](), MPI.PostNewTags[i](), MPI.PostFlags[i] ? "Extra '_' appended" : "");
      OutF.WriteString(s());
      }
    }
  if (MPI.PostChangeTrends.GetSize()>0)
    {
    OutF.WriteString("\nTrend window names changed in import project:\n");
    for (i=0; i<MPI.PostChangeTrends.GetSize(); i++)
      {
      s1.FnNameExt(MPI.PostChangeTrends[i]());
      s2.FnNameExt(MPI.PostNewTrends[i]());
      s.Set("%s,%s\n", s1(), s2());
      OutF.WriteString(s());
      }
    }
  if (MPI.PostChangeGraphics.GetSize()>0)
    {
    OutF.WriteString("\nGraphic window names changed in import project:\n");
    for (i=0; i<MPI.PostChangeGraphics.GetSize(); i++)
      {
      s1.FnNameExt(MPI.PostChangeGraphics[i]());
      s2.FnNameExt(MPI.PostNewGraphics[i]());
      s.Set("%s,%s\n", s1(), s2());
      OutF.WriteString(s());
      }
    }
  if (MPI.MasterRefFiles.GetSize()>0)
    {
    OutF.WriteString("\nFiles referenced by current project:\n");
    for (i=0; i<MPI.MasterRefFiles.GetSize(); i++)
      {
      s1.FnExpand(MPI.MasterRefFiles[i]());
      s.Set("%s,%s\n", s1(), MPI.MasterRefModels[i]());
      OutF.WriteString(s());
      }
    }
  if (MPI.SlaveRefFiles.GetSize()>0)
    {
    OutF.WriteString("\nFiles referenced by import project:\n");
    for (i=0; i<MPI.SlaveRefFiles.GetSize(); i++)
      {
      s.Set("%s,%s\n", MPI.SlaveRefFiles[i](), MPI.SlaveRefModels[i]());
      OutF.WriteString(s());
      }
    }
  if (MPI.PostChangeRefFiles.GetSize()>0)
    {
    OutF.WriteString("\nReferenced files copied from imported project to current project:\n");
    bool FoundOne = false;
    for (i=0; i<MPI.PostChangeRefFiles.GetSize(); i++)
      {
      if (MPI.bIgnoreProbRefFiles && stricmp(MPI.PostNewRefFiles[i](), "Unknown")==0)
        FoundOne = true;
      else
        {
        s.Set("%s,%s\n", MPI.PostChangeRefFiles[i](), MPI.PostNewRefFiles[i]());
        OutF.WriteString(s());
        }
      }
    if (FoundOne)
      {
      OutF.WriteString("\nWarning: Referenced files from imported project that will NOT be copied to current project:\n");
      for (i=0; i<MPI.PostChangeRefFiles.GetSize(); i++)
        {
        if (MPI.bIgnoreProbRefFiles && stricmp(MPI.PostNewRefFiles[i](), "Unknown")==0)
          {
          s.Set("%s\n", MPI.PostChangeRefFiles[i]());
          OutF.WriteString(s());
          }
        }
      }
    }

  OutF.WriteString("\n");
  OutF.Close();
  return MPI.iErrorNo;
  }

//---------------------------------------------------------------------------

int CExecutive::DoMergeA(CMergeProjectsInfo& MPI)
  {
  //change required tags in current project...
  CDlgBusy::SetLine(3, "Changing tags in current project");
  for (int i=0; i<MPI.PreChangeTags.GetSize(); i++)
    {
    int Err = ChangeTag(MPI.PreChangeTags[i](), MPI.PreTempTags[i]());
    ASSERT(Err==EOCT_DONE);
    if (Err!=EOCT_DONE)
      LogError("Merge", LF_Exclamation, "A: Change tag failed  %s --> %s", MPI.PreChangeTags[i](), MPI.PreTempTags[i]());
    }

  //copy relavent files...
  for (i=0; i<MPI.PostChangeRefFiles.GetSize(); i++)
    {
    if (stricmp(MPI.PostNewRefFiles[i](), "Unknown")!=0)
      {
      Strng ss;
      ss.FnDrivePath(MPI.PostNewRefFiles[i]());
      if (ss.CmpLastChar('\\'))
        ss=ss.Left(ss.Len()-1);
      if (!FileExists(ss()))
        {
        if (CreateDirectory(ss(), NULL)==0)
          LogError("Merge", LF_Exclamation, "Unable to create folder %s", ss());
        }
      if (!CopyFile(MPI.PostChangeRefFiles[i](), MPI.PostNewRefFiles[i](), true))
        LogError("Merge", LF_Exclamation, "Unable to copy file %s", MPI.PostChangeRefFiles[i]());
      }
    }

  //load the slave project models...
  CDlgBusy::SetLine(3, "Importing models");
  LoadDBase(1, DBConnect_Find, 0, MPI.sSlavePrjDir()); // Dont load Other Data
  //load other stuff, drivers, etc???
  LoadDBase(2, DBConnect_Find, 0, MPI.sSlavePrjDir()); // Now load Other Data
  return 0;
  }

//---------------------------------------------------------------------------

int CExecutive::DoMergeB(CMergeProjectsInfo& MPI)
  {
  //change required tags in merged project...
  CDlgBusy::SetLine(3, "Changing tags in imported project");
  for (int i=0; i<MPI.PostChangeTags.GetSize(); i++)
    {
    int Err = ChangeTag(MPI.PostChangeTags[i](), MPI.PostNewTags[i]());
    //ASSERT(Err==EOCT_DONE);
    if (Err!=EOCT_DONE)
      LogError("Merge", LF_Exclamation, "B: Change tag failed  %s --> %s", MPI.PostChangeTags[i](), MPI.PostNewTags[i]());
    }
  CDlgBusy::SetLine(3, "Changing tags in current project");
  for (i=0; i<MPI.PreTempTags.GetSize(); i++)
    {
    int Err = ChangeTag(MPI.PreTempTags[i](), MPI.PreNewTags[i]());
    //ASSERT(Err==EOCT_DONE);
    if (Err!=EOCT_DONE)
      LogError("Merge", LF_Exclamation, "C: Change tag failed  %s --> %s", MPI.PreTempTags[i](), MPI.PreNewTags[i]());
    }
  return 0;
  }

//---------------------------------------------------------------------------
/*
flag CExecutive::XCloseTransaction(CExecObj *pEO)
  {
  UnlockExec();
  return true;
  };
*/
//---------------------------------------------------------------------------

void CExecutive::SetSelfConnect(CExecObj *pClnt/*=NULL*/, CExecObj *pSrvr/*=NULL*/)
  {
  };

//---------------------------------------------------------------------------

flag CExecutive::CIStrng(int No, pchar & pS)
  {
  switch (No-CBContext())
    {
    case  1: pS="E\tRunning Slowly";     return 1;
    case  2: pS="";                      return 1;
    default:
      return ConditionBlk::CIStrng(No, pS);
    }
  };
                                           \
//===========================================================================
//
//
//
//===========================================================================

CXStopWatches::CXStopWatches(CExecutive * pX) :
  X(*pX)
  {
  SYSTEM_INFO si;
  GetSystemInfo(&si);
  m_nNoOfProcessors=si.dwNumberOfProcessors;
  m_nMvAvgPts=10;
  Clear(true);
  };

//---------------------------------------------------------------------------

CXStopWatches & CXStopWatches::operator=(CXStopWatches & SW)
  {
  m_swStart = SW.m_swStart;
  m_swQuery = SW.m_swQuery;
  m_swBuild = SW.m_swBuild;
  m_swExec = SW.m_swExec;
  m_swExecDn = SW.m_swExecDn;
  m_swRead = SW.m_swRead;
  m_swWrite = SW.m_swWrite;
  m_swStop = SW.m_swStop;
  m_swStep =SW.m_swStep;
  m_swRun = SW.m_swRun;

  for (int i=0; i<MaxMvAvgPts; i++)
    {
    m_dStepTimeI[i]=SW.m_dStepTimeI[i];
    m_dMultiplierI[i]=SW.m_dMultiplierI[i];
    m_dProcUseI[i]=SW.m_dProcUseI[i];
    }
  m_dProcTime=SW.m_dProcTime;
  m_dProcTimePrv=SW.m_dProcTimePrv;

  m_dSimulatedTime=SW.m_dSimulatedTime;
  m_dStartingTime=SW.m_dStartingTime;
  m_dStepTimeTtl=SW.m_dStepTimeTtl;
  m_dMultiplierTtl=SW.m_dMultiplierTtl;
  m_dProcUseTtl=SW.m_dProcUseTtl;

  m_nStepCnt=SW.m_nStepCnt;
  m_nMvAvgCnt=SW.m_nMvAvgCnt;
  m_nMvAvgPts=SW.m_nMvAvgPts;

  return *this;
  };

//---------------------------------------------------------------------------

void   CXStopWatches::Clear(bool All)
  {
  m_swStart.Clear();
  m_swQuery.Clear();
  m_swBuild.Clear();
  m_swExec.Clear();
  m_swExecDn.Clear();
  m_swRead.Clear();
  m_swWrite.Clear();
  m_swStop.Clear();
  m_swStep.Clear();
  if (All)
    m_swRun.Clear();

  m_dStepTimeI[m_nMvAvgCnt%m_nMvAvgPts]=0.0;
  m_dMultiplierI[m_nMvAvgCnt%m_nMvAvgPts]=0.0;
  m_dProcUseI[m_nMvAvgCnt%m_nMvAvgPts]=0.0;
  m_dProcTime=0.0;

  if (All)
    {
    m_bStepStarted=0;
    m_bLastStep=0;
    m_dProcTimePrv=0.0;
    m_dSimulatedTime=0.0;
    m_dStartingTime=0.0;
    m_dStepTimeTtl=0.0;
    m_dMultiplierTtl=0.0;
    m_dProcUseTtl=0.0;
    m_nStepCnt=0;
    m_nMvAvgCnt=0;
    }
  };

//---------------------------------------------------------------------------

double CXStopWatches::TotalSecs()
  {
  return m_swStart.Secs()+m_swQuery.Secs()+m_swBuild.Secs()+m_swExec.Secs()+m_swExecDn.Secs()+m_swRead.Secs()+m_swWrite.Secs()+m_swStop.Secs();
  };

//---------------------------------------------------------------------------

void CXStopWatches::StepBegin()
  {
  m_bStepStarted=1;
  m_swStep.ReStart();
  m_dProcTimePrv=GetProcTime();

  //dbgpln("StepBegin %4i  %12.6f", nStepCnt,dProcTimePrv);
  };

//---------------------------------------------------------------------------

void CXStopWatches::StepEnd()
  {//includes "stopping" step!
  m_swStep.Stop();

  if (!m_bStepStarted)
    return;

  const double ProcTime=GetProcTime();
  const double ProcTimeDiff = (ProcTime-m_dProcTimePrv);

  CXM_TimeControl & CB=TimeCB();

  int i=m_nMvAvgCnt%m_nMvAvgPts;
  m_dStepTimeI[i] = m_swStep.Secs();
  m_dMultiplierI[i] = CB.m_StepSize/GTZ(m_dStepTimeI[i]);
  m_dProcUseI[i] = ProcTimeDiff/(m_nNoOfProcessors*GTZ(m_dStepTimeI[i]));

  m_nMvAvgCnt++;
  m_nStepCnt++;

  //dbgpln("StepEnd   %6i %6i %12.6f  %12.6f  %12.6f  %12.6f  %12.6f",
  //  m_nStepCnt, m_nMvAvgCnt,
  //  ProcTimeDiff, CB.m_StepSize,
  //  m_dStepTimeI[i], m_dMultiplierI[i], m_dProcUseI[i]);

  m_dStepTimeTtl += m_dStepTimeI[i];
  m_dMultiplierTtl += m_dMultiplierI[i];
  m_dProcUseTtl += m_dProcUseI[i];

  if (!m_bLastStep)
    {
    m_dSimulatedTime += CB.m_StepSize;
    }

  m_dProcTimePrv=ProcTime;
  m_bStepStarted=false;
  }

//---------------------------------------------------------------------------

double CXStopWatches::StepTimeInst()    { return m_dStepTimeI[(m_nMvAvgCnt-1)%m_nMvAvgPts]; };
double CXStopWatches::StepTimeAvg()     { return m_dStepTimeTtl/Max(1L, m_nStepCnt); };
double CXStopWatches::StepTimeMvAvg()   { int j=Min(m_nMvAvgCnt, m_nMvAvgPts); double T=0; for (int i=0; i<j; i++)  T+=m_dStepTimeI[i%m_nMvAvgPts]; return T/Max(1, j); };

double CXStopWatches::MultiplierInst()  { return m_dMultiplierI[(m_nMvAvgCnt-1)%m_nMvAvgPts]; };
double CXStopWatches::MultiplierAvg()   { return m_dMultiplierTtl/Max(1L, m_nStepCnt); };
double CXStopWatches::MultiplierMvAvg() { int j=Min(m_nMvAvgCnt, m_nMvAvgPts); double T=0; for (int i=0; i<j; i++)  T+=m_dMultiplierI[i%m_nMvAvgPts]; return T/Max(1, j); };

double CXStopWatches::ProcUseInst()     { return m_dProcUseI[(m_nMvAvgCnt-1)%m_nMvAvgPts]; };
double CXStopWatches::ProcUseAvg()      { return m_dProcUseTtl/Max(1L, m_nStepCnt); };
double CXStopWatches::ProcUseMvAvg()    { int j=Min(m_nMvAvgCnt, m_nMvAvgPts); double T=0; for (int i=0; i<j; i++)  T+=m_dProcUseI[i%m_nMvAvgPts]; return T/Max(1, j); };


//===========================================================================
//
//
//
//===========================================================================

CMdlGeneralInfo* CMdlGeneralInfo::pFirst = NULL;

CMdlGeneralInfo::CMdlGeneralInfo()
  {
  bInitRequest=1;
  iGroupIndex=0;
  iItemIndex=0;
  pNext=NULL;
  if (pFirst==NULL)
    pFirst = this;
  else
    {
    CMdlGeneralInfo* pMGI = pFirst;
    while (pMGI->pNext)
      pMGI = pNext;
    pMGI->pNext = this;
    }
  }

//---------------------------------------------------------------------------

CMdlGeneralInfo::~CMdlGeneralInfo()
  {
  if (pFirst==this)
    {
    pFirst = pNext;
    }
  else
    {
    CMdlGeneralInfo* pMGI = pFirst;
    while (pMGI->pNext!=this)
      pMGI = pNext;
    pMGI->pNext = pMGI->pNext->pNext;
    }
  }

//---------------------------------------------------------------------------

void CMdlGeneralInfo::Init()
  {
  bInitRequest=1;
  iGroupIndex=0;
  iItemIndex=0;
  Headings.SetSize(0);
  }

//===========================================================================
//
//
//
//===========================================================================

CExecSequence::CExecSequence(CExecutive * pExec) :
  m_Exec(*pExec)
  {
  m_lSeqNo=0;
  m_HoldPlay=false;
  };

//---------------------------------------------------------------------------

void CExecSequence::SetSequenceNo(long SeqNo)
  {
  CSingleLock Lock(&m_Sect, true);
  m_lSeqNo=SeqNo;
  };

//---------------------------------------------------------------------------

long CExecSequence::IncSequenceNo(long Inc/*=1*/)
  {
  CSingleLock Lock(&m_Sect, true);
  m_lSeqNo+=Inc;
  return m_lSeqNo;
  };

//---------------------------------------------------------------------------

void CExecSequence::PlayTheList(long Up2SeqNo)
  {
  CSingleLock Lock(&m_Sect, true);
  if (!m_HoldPlay)
    {
    while (!m_ReplayList.IsEmpty())
      {
      CExecSeqItem &I=m_ReplayList.GetHead();
      if (I.m_lSeqNo<=Up2SeqNo)
        {
        CExecSeqItem &PlayItem=m_ReplayList.RemoveHead();

        long lRet=0;
        Strng WrkTag, WrkCnvTxt;
        TaggedObject::SplitTagCnv(PlayItem.m_sTag(), WrkTag, WrkCnvTxt);
        flag UseCnv = (WrkCnvTxt.Length()>0);
        CXM_ObjectTag ObjTag(WrkTag(), (UseCnv ? TABOpt_ValCnvsOnce : 0));
        CXM_ObjectData ObjData;
        CXM_Route Route;
        if (m_Exec.XReadTaggedItem(NULL, ObjTag, ObjData, Route))
          {
          CPkDataItem * pItem = ObjData.FirstItem();
          if (IsNumData(pItem->Type()))
            {
            if (UseCnv && Cnvs[(pItem->CnvIndex())]->Find(WrkCnvTxt())==NULL)
              {
              UseCnv = 0;
              LogWarning("Replay", 0, "Valid conversion expected %s", WrkTag());
              lRet=2;
              }
            }
          else
            {
            //LogWarning("Script Cmds", 0, "Script command SetTag: Valid tag expected (%s)", WrkTag());
            //return 1;
            }

          PkDataUnion DU;
          VARIANT R;
          ::VariantInit(&R);
          if (IsFloatData(pItem->Type()))
            {
            int PutIt=false;
            int ConvIt=true;
            if (PlayItem.m_vtValue.vt==VT_BSTR)
              {
              USES_CONVERSION;
              LPCTSTR S=OLE2T(PlayItem.m_vtValue.bstrVal);
              if (S[0]=='*')
                {
                R.vt=VT_R8;
                R.dblVal=dNAN;
                PutIt=true;
                ConvIt=false;
                UseCnv=false;
                }
              }
            if (ConvIt)
              PutIt=SUCCEEDED(::VariantChangeType(&R, &PlayItem.m_vtValue, 0, VT_R8));

            if (PutIt)
              {
              if (UseCnv)
                DU.SetTypeDouble(pItem->Type(), R.dblVal, pItem->CnvIndex(), WrkCnvTxt());
              else
                DU.SetTypeDouble(pItem->Type(), R.dblVal);
              }
            else
              {
              LogWarning("Replay", 0, "Type change failed (%s)", WrkTag());
              lRet=3;
              }
            }
          else if (IsNumData(pItem->Type()))
            {
            if (SUCCEEDED(::VariantChangeType(&R, &PlayItem.m_vtValue, 0, VT_I4)))
              {
              if (UseCnv)
                DU.SetTypeLong(pItem->Type(), R.lVal, pItem->CnvIndex(), WrkCnvTxt());
              else
                DU.SetTypeLong(pItem->Type(), R.lVal);
              }
            else
              {
              LogWarning("Replay", 0, "Type change failed (%s)", WrkTag());
              lRet=4;
              }
            }
          else
            {
            USES_CONVERSION;
            if (SUCCEEDED(::VariantChangeType(&R, &PlayItem.m_vtValue, 0, VT_BSTR)))
              DU.SetTypeString(pItem->Type(), OLE2T(R.bstrVal));
            else
              {
              LogWarning("Replay", 0, "Type change failed (%s)", WrkTag());
              lRet=5;
              }
            }

          CXM_ObjectData ObjData(0, 0, WrkTag(), 0, DU);
          if ((lRet==0) && (m_Exec.XWriteTaggedItem(NULL, ObjData, Route)!=TOData_OK))
            {
            LogWarning("Replay", 0, "Write failed %s", WrkTag());
            lRet=6;
            }
          else
            {
            if (IsFloatData(pItem->Type()))
              gs_Exec.m_Seq.NoteManSet(CExecSequence::VSS_Replay, WrkTag(), R.dblVal, UseCnv ? WrkCnvTxt() : "");
            else if (IsNumData(pItem->Type()))
              gs_Exec.m_Seq.NoteManSet(CExecSequence::VSS_Replay, WrkTag(), R.lVal, UseCnv ? WrkCnvTxt() : "");
            else
              {
              USES_CONVERSION;
              gs_Exec.m_Seq.NoteManSet(CExecSequence::VSS_Replay, WrkTag(), OLE2T(R.bstrVal), "");
              }
            }
          }
        else
          {
          LogWarning("Replay", 0, "Valid tag expected (%s)", WrkTag());
          //lRet=1;
          }
        }
      else
        break;
      }
    };
  };

//---------------------------------------------------------------------------

void CExecSequence::ClearTheList(long Up2SeqNo)
  {
  CSingleLock Lock(&m_Sect, true);
  while (!m_ReplayList.IsEmpty())
    {
    CExecSeqItem &I=m_ReplayList.GetHead();
    if (I.m_lSeqNo<=Up2SeqNo)
      m_ReplayList.RemoveHead();
    else
      break;
    }
  };

//---------------------------------------------------------------------------

void CExecSequence::VariableSetReplayItem(long SeqNo, LPCTSTR Tag, const VARIANT Value, LPCTSTR CnvTxt)
  {
  CSingleLock Lock(&m_Sect, true);

  POSITION Pos=m_ReplayList.GetHeadPosition();
  while (Pos!=NULL)
    {
    CExecSeqItem & I=m_ReplayList.GetNext(Pos);
    if (I.m_lSeqNo>=SeqNo)
      break;
    }

  CExecSeqItem I;
  I.m_lSeqNo=SeqNo;
  I.m_sTag=Tag;
  I.m_vtValue=Value;
  I.m_sCnvTxt=CnvTxt;

  if (Pos)
    m_ReplayList.InsertBefore(Pos, I);
  else
    m_ReplayList.AddTail(I);
  };

//---------------------------------------------------------------------------

void CExecSequence::VariableSetReplayCtrl(long Command, long Param)
  {
  CSingleLock Lock(&m_Sect, true);
  switch (Command)
    {
    case VSC_ClearTo:
      ClearTheList(Param);
      break;
    case VSC_ClearAll:
      ClearTheList(LONG_MAX);
      break;
    case VSC_PlayTo:
      m_HoldPlay=false;
      PlayTheList(Param);
      break;
    case VSC_PlayAll:
      m_HoldPlay=false;
      PlayTheList(LONG_MAX);
      break;
    case VSC_SetHold:
      m_HoldPlay=Param!=0;
      break;
    }
  };

//---------------------------------------------------------------------------

void CExecSequence::NoteManSet(long Source, LPCTSTR Tag, double Value, LPCTSTR CnvTxt)
  {
  EVM_VariableSet *pMVS=(EVM_VariableSet*)CoTaskMemAlloc(sizeof(EVM_VariableSet));

  pMVS->m_lSource=Source;
  pMVS->m_lSequenceNo=m_lSeqNo;
  pMVS->m_bsTag=A2BSTR(Tag);
  pMVS->m_vValue.vt=VT_R8;
  pMVS->m_vValue.dblVal=Value;
  pMVS->m_bsCnvTxt=A2BSTR(CnvTxt);

  // FireOnEVent will free pMVS
  ::AfxGetMainWnd()->SendMessage(WMU_COMEVTRELAY, ComCmd_OnVariableSet, (LPARAM)pMVS);
  };

//---------------------------------------------------------------------------

void CExecSequence::NoteManSet(long Source, LPCTSTR Tag, long Value, LPCTSTR CnvTxt)
  {
  EVM_VariableSet *pMVS=(EVM_VariableSet*)CoTaskMemAlloc(sizeof(EVM_VariableSet));

  pMVS->m_lSource=Source;
  pMVS->m_lSequenceNo=m_lSeqNo;
  pMVS->m_bsTag=A2BSTR(Tag);
  pMVS->m_vValue.vt=VT_I4;
  pMVS->m_vValue.lVal=Value;
  pMVS->m_bsCnvTxt=A2BSTR(CnvTxt);

  // FireOnEVent will free pMVS
  ::AfxGetMainWnd()->SendMessage(WMU_COMEVTRELAY, ComCmd_OnVariableSet, (LPARAM)pMVS);
  };

//---------------------------------------------------------------------------

void CExecSequence::NoteManSet(long Source, LPCTSTR Tag, LPCTSTR Value, LPCTSTR CnvTxt)
  {
  EVM_VariableSet *pMVS=(EVM_VariableSet*)CoTaskMemAlloc(sizeof(EVM_VariableSet));

  pMVS->m_lSource=Source;
  pMVS->m_lSequenceNo=m_lSeqNo;
  pMVS->m_bsTag=A2BSTR(Tag);
  pMVS->m_vValue.vt=VT_BSTR;
  pMVS->m_vValue.bstrVal=A2BSTR(Value);
  pMVS->m_bsCnvTxt=A2BSTR(CnvTxt);

  // FireOnEVent will free pMVS
  ::AfxGetMainWnd()->SendMessage(WMU_COMEVTRELAY, ComCmd_OnVariableSet, (LPARAM)pMVS);
  };

//===========================================================================
//
//
//
//===========================================================================

CEventLog::CEventLog()
  {
  bUseEventLog = 0;
  bStartRqd = 1;
  bDeleteOldRqd = 0;
  pEventLogFile = NULL;
  lIteration = 0;
  dStartTime = 0.0;
  }

//---------------------------------------------------------------------------

CEventLog::~CEventLog()
  {
  CloseFile();
  }

//---------------------------------------------------------------------------

void CEventLog::SetUseLog(bool UseLog)
  {
  bUseEventLog = (UseLog ? 1 : 0);
  if (!bUseEventLog && pEventLogFile)
    CloseFile();
  }

//---------------------------------------------------------------------------

void CEventLog::StartFile(bool DeleteOld)
  {
  if (pEventLogFile)
    CloseFile();
  bStartRqd = 1;
  bDeleteOldRqd = (DeleteOld ? 1 : 0);
  lIteration = 0;
  dStartTime = TimeCB().m_Time;
  }

//---------------------------------------------------------------------------

void CEventLog::CloseFile()
  {
  if (pEventLogFile)
    {
    fclose(pEventLogFile);
    pEventLogFile = NULL;
    }
  bStartRqd = 1;
  bDeleteOldRqd = 0;
  }

//---------------------------------------------------------------------------

void CEventLog::FlushFile()
  {
  if (pEventLogFile)
    {
    fflush(pEventLogFile);
    }
  }

//---------------------------------------------------------------------------

void CEventLog::LogEvent(char* pMsg)
  {
  if (bUseEventLog)
    {
    if (bStartRqd)
      {
      Strng Filename(PrjFiles());
      Filename += "EventLog.csv";
      pEventLogFile = fopen(Filename(), bDeleteOldRqd ? "wt" : "wt+");
      if (pEventLogFile && bDeleteOldRqd)
        {
        char Buff[256];
        sprintf(Buff, "Iteration,DateTime,Seconds,EventMessage\n");
        fwrite(Buff, sizeof(char), strlen(Buff), pEventLogFile);
        }
      bStartRqd = 0;
      }

    if (pEventLogFile)
      {
      char DateBuff[128];
      char Buff[256];
      sprintf(Buff, "%d,%s,%g,\"", lIteration, SecstoDateHMS(TimeCB().m_Time, DateBuff, true), TimeCB().m_Time-dStartTime);
      fwrite(Buff, sizeof(char), strlen(Buff), pEventLogFile);
      if (pMsg)
        fwrite(pMsg, sizeof(char), strlen(pMsg), pEventLogFile);
      sprintf(Buff, "\"\n");
      fwrite(Buff, 1, 2, pEventLogFile);
      }
    }
  }

//===========================================================================

CEventLog gs_EventLog;

//===========================================================================
//
//
//
//===========================================================================

CExecutive gs_Exec;

//===========================================================================
//
//
//
//===========================================================================
