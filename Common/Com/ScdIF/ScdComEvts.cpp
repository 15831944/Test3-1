// ComCom.cpp: implementation of the CComCom class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#define __SCDCOMEVTS_CPP
#include "resource.h"
#include "scdcomevts.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//=========================================================================
//
//
//
//=========================================================================

HWND CScdCOCmdBase::s_hWnd4Msgs=NULL;
static std::vector<CScdCOCmdWaiting> /*CScdCOCmdBase::*/gs_WaitingEvtBlks;
static UINT /*CScdCOCmdBase::*/gs_lWaitingEvtBlkCount=0;
static CRITICAL_SECTION /*CScdCOCmdBase::*/gs_CritSect;

class CScdCOCmdBaseInit
  {
  public:
    CScdCOCmdBaseInit()
      {
      InitializeCriticalSection(&gs_CritSect);
      }
    ~CScdCOCmdBaseInit()
      {
      DeleteCriticalSection(&gs_CritSect);
      }
  };

static CScdCOCmdBaseInit gs_CritInit;

CScdCOCmdBase::CScdCOCmdBase(DWORD MsgID)
  {
  m_MsgID= MsgID;
  m_bSyncCalls=true;
  //m_bEventsOn=true;
  for (long i=0; i<MaxComEvents; i++)
    {
    m_hEventRcvd[i]=INVALID_HANDLE_VALUE;
    m_lEventData[i]=0;
    }
  m_dwSyncTimeOut=1000000;
  }

//-------------------------------------------------------------------------

CScdCOCmdBase::~CScdCOCmdBase()
  {
  for (long i=0; i<MaxComEvents; i++)
    if (m_hEventRcvd[i]!=INVALID_HANDLE_VALUE)
      CloseHandle(m_hEventRcvd[i]);

//  for (i=0; i<m_Events2Fire.size(); i++)
//    m_Events2Fire[i]->Release();
  }

//-------------------------------------------------------------------------

void CScdCOCmdBase::AddChildEvents(IUnknown * pUnk)
  {
/// Remove this - temp
//  return;

//  IUnknown *p;
//  if (SUCCEEDED(pUnk->QueryInterface(IID_IScdASyncEvents, (void**)&p)))
//    {
//    int n=m_Events2Fire.size();
//    m_Events2Fire.resize(n+1);
//    m_Events2Fire[n]=(IScdASyncEvents*)p;
//    }
  };

//-------------------------------------------------------------------------

void CScdCOCmdBase::InitCOCmdStuff(long EvId)
  {
  if (!m_bSyncCalls)
    return;
  _ASSERTE(EvId<MaxComEvents);
  if (m_hEventRcvd[EvId]==INVALID_HANDLE_VALUE)
    m_hEventRcvd[EvId]=CreateEvent(NULL, FALSE, FALSE, NULL);

  AppendWaitingEventBlk(EvId, this);
  }

//-------------------------------------------------------------------------

UINT CScdCOCmdBase::ExecCOCmdStuff(long EvId)
  {
  if (!m_bSyncCalls)
    return SS_CallASync;
  _ASSERTE(EvId<MaxComEvents);

  UINT Ret=SS_CallReturned;
  if (WaitForSingleObject(m_hEventRcvd[EvId], m_dwSyncTimeOut)==WAIT_TIMEOUT)
    Ret=SS_CallTimeout;
  RemoveWaitingEventBlk(this);
  return Ret;
  }

//-------------------------------------------------------------------------

void CScdCOCmdBase::InitCOCmdStuff(long *EvIds, long nEvts)
  {
  if (!m_bSyncCalls)
    return;
  _ASSERTE(nEvts<MaxComEvents);
  for (long i=0; i<nEvts; i++)
    {
    if (m_hEventRcvd[EvIds[i]]==INVALID_HANDLE_VALUE)
      m_hEventRcvd[EvIds[i]]=CreateEvent(NULL, FALSE, FALSE, NULL);
    }

  AppendWaitingEventBlk(EvIds, nEvts, this);
  };

//-------------------------------------------------------------------------

UINT CScdCOCmdBase::ExecCOCmdStuff(long *EvIds, long nEvts)
  {
  if (!m_bSyncCalls)
    return SS_CallASync;
  HANDLE h[MaxComEvents];
  _ASSERTE(nEvts<MaxComEvents);
  for (long i=0; i<nEvts; i++)
    {
    _ASSERTE(EvIds[i]<MaxComEvents);
    h[i]=m_hEventRcvd[EvIds[i]];
    }

  UINT Ret=SS_CallReturned;
  if (WaitForMultipleObjects(nEvts, h, FALSE, m_dwSyncTimeOut)==WAIT_TIMEOUT)
    Ret=SS_CallTimeout;
  RemoveWaitingEventBlk(this);
  return Ret;
  };

//-------------------------------------------------------------------------

LRESULT CScdCOCmdBase::SendComCmd(long Cmd, long Data)
  {
  CScdCOCmdBlk * p=new CScdCOCmdBlk(Cmd, Data);
  return ::SendMessage(s_hWnd4Msgs, m_MsgID, Cmd, (long)p);

//  return AfxGetMainWnd()->SendMessage(m_MsgID, Cmd, (long)p);
  }

//-------------------------------------------------------------------------

UINT CScdCOCmdBase::PostComCmd(long Cmd, long Data)
  {
  InitCOCmdStuff(Cmd);
  CScdCOCmdBlk * p=new CScdCOCmdBlk(Cmd, Data);
  //while (!::ScdMainWnd()->PostMessage(m_MsgID, Cmd, (long)p))
  while (!::PostMessage(s_hWnd4Msgs, m_MsgID, Cmd, (long)p))
    {
    Sleep(10);
    }
  return ExecCOCmdStuff(Cmd);
  }

//-------------------------------------------------------------------------

UINT CScdCOCmdBase::PostComCmd(long Cmd, long *EvIds, long nEvts, long Data)
  {
  InitCOCmdStuff(EvIds, nEvts);
  CScdCOCmdBlk * p=new CScdCOCmdBlk(Cmd, Data);
  //while (!::ScdMainWnd()->PostMessage(m_MsgID, Cmd, (long)p))
  while (!::PostMessage(s_hWnd4Msgs, m_MsgID, Cmd, (long)p))
    {
    Sleep(10);
    }
  return ExecCOCmdStuff(EvIds, nEvts);
  }

//-------------------------------------------------------------------------

void CScdCOCmdBase::AppendWaitingEventBlk(long EvId, CScdCOCmdBase *pEvtBlk)
  {
  EnterCriticalSection(&gs_CritSect);

  if (gs_WaitingEvtBlks.size()<gs_lWaitingEvtBlkCount+1)
    gs_WaitingEvtBlks.resize(gs_lWaitingEvtBlkCount+1);

  gs_WaitingEvtBlks[gs_lWaitingEvtBlkCount].m_lEvId=EvId;
  gs_WaitingEvtBlks[gs_lWaitingEvtBlkCount].m_pEvtBlk=pEvtBlk;
  gs_lWaitingEvtBlkCount++;

  LeaveCriticalSection(&gs_CritSect);
  };

//-------------------------------------------------------------------------

void CScdCOCmdBase::AppendWaitingEventBlk(long *EvIds, long nEvts, CScdCOCmdBase *pEvtBlk)
  {
  EnterCriticalSection(&gs_CritSect);

  if (gs_WaitingEvtBlks.size()<gs_lWaitingEvtBlkCount+nEvts)
    gs_WaitingEvtBlks.resize(gs_lWaitingEvtBlkCount+nEvts);

  for (int i=0; i<nEvts; i++)
    {
    gs_WaitingEvtBlks[gs_lWaitingEvtBlkCount].m_lEvId=EvIds[i];
    gs_WaitingEvtBlks[gs_lWaitingEvtBlkCount].m_pEvtBlk=pEvtBlk;
    gs_lWaitingEvtBlkCount++;
    }

  LeaveCriticalSection(&gs_CritSect);
  };

//-------------------------------------------------------------------------

void CScdCOCmdBase::RemoveWaitingEventBlk(CScdCOCmdBase *pEvtBlk)
  {
  EnterCriticalSection(&gs_CritSect);

  for (UINT i=0; i<gs_lWaitingEvtBlkCount; i++)
    {
    if (gs_WaitingEvtBlks[i].m_pEvtBlk==pEvtBlk)
      {
      UINT is=i;
      UINT ie=i+1;
      while (ie<gs_lWaitingEvtBlkCount && gs_WaitingEvtBlks[ie].m_pEvtBlk==pEvtBlk)
        ie++;
      for (int k=0; ie+k<gs_lWaitingEvtBlkCount; k++)
        gs_WaitingEvtBlks[is+k]=gs_WaitingEvtBlks[ie+k];
      gs_lWaitingEvtBlkCount-=ie-is;
      }
    }

  LeaveCriticalSection(&gs_CritSect);
  return ;
  };

//-------------------------------------------------------------------------

void CScdCOCmdBase::DoEventMsg(long Evt, long Data)
  {
//  ? search a static list of outstanding CScdCOCmdBase waiting for the result of an event
  if (m_bSyncCalls)
    {
    _ASSERTE(Evt<MaxComEvents);
    if (m_hEventRcvd[Evt]!=INVALID_HANDLE_VALUE)
      {
      m_lEventData[Evt]=Data;
      ::SetEvent(m_hEventRcvd[Evt]);
      }
    }
  FireTheEvent(Evt, Data);
//  for (int i=0; i<m_Events2Fire.size(); i++)
//    m_Events2Fire[i]->DoEventMsg(Evt,Data);
  }

//-------------------------------------------------------------------------

void CScdCOCmdBase::FireEventMsgs(WPARAM w, LPARAM l)
  {
  EnterCriticalSection(&gs_CritSect);
  for (UINT i=0; i<gs_lWaitingEvtBlkCount; i++)
    if (w==gs_WaitingEvtBlks[i].m_lEvId)
      gs_WaitingEvtBlks[i].m_pEvtBlk->DoEventMsg(w, l);
  LeaveCriticalSection(&gs_CritSect);
  }

//=========================================================================

CScdCOEventSender::CScdCOEventSender(DWORD ThreadId, DWORD MsgEvtId)
  {
  m_dwThreadId = ThreadId;
  m_dwEvtMsgId = MsgEvtId;
  }

//-------------------------------------------------------------------------

CScdCOEventSender::~CScdCOEventSender()
  {
  };

//-------------------------------------------------------------------------

void CScdCOEventSender::Init(DWORD ThreadId, DWORD MsgEvtId)
  {
  m_dwThreadId = ThreadId;
  m_dwEvtMsgId = MsgEvtId;
  }

//-------------------------------------------------------------------------

void CScdCOEventSender::Send(long Evt, /*CScdCOCmdBlk * pBlk,*/ long Data)
  {
  //if (pBlk==NULL)
  //  {
  //  _asm int 3;
  //  }
  AtlTrace("CScdCOEventSender::Send %08x\n", m_dwThreadId);
  int i=1000;
  while (i-- && !PostThreadMessage(m_dwThreadId, m_dwEvtMsgId, Evt, Data/*(LPARAM)pBlk*/))
    Sleep(1);
  if (i<=0)
    {
    throw "SendEventMsg Failed";
    AtlTrace("  CScdCOEventSender::Send - Failed\n");
    }
  }

//=========================================================================

CScdCOEventSenders::CScdCOEventSenders()
  {
  m_nXmtrs=0;
  InitializeCriticalSection(&m_CritSect);
  }

//-------------------------------------------------------------------------

CScdCOEventSenders::~CScdCOEventSenders()
  {
  DeleteCriticalSection(&m_CritSect);
  }

//-------------------------------------------------------------------------

void CScdCOEventSenders::Add(CScdCOEventSender * p)
  {
  EnterCriticalSection(&m_CritSect);
  ATLASSERT(m_nXmtrs<4);
  m_pXmtrs[m_nXmtrs++]=p;
  m_pXmtrs[m_nXmtrs]=NULL;
  LeaveCriticalSection(&m_CritSect);
  };

//-------------------------------------------------------------------------

void CScdCOEventSenders::Remove(CScdCOEventSender * p)
  {
  EnterCriticalSection(&m_CritSect);
  for (long i=0; i<m_nXmtrs; i++)
    if (m_pXmtrs[i]==p)
      break;
  for (; i<m_nXmtrs; i++)
    m_pXmtrs[i]=m_pXmtrs[i+1];
  LeaveCriticalSection(&m_CritSect);
  };

//-------------------------------------------------------------------------

void CScdCOEventSenders::Send(long Evt, /*CScdCOCmdBlk * pBlk,*/ long Data)
  {
  //if (pBlk && pBlk->m_lCmd<0)
  //  pBlk->m_lCmd=Evt;
  EnterCriticalSection(&m_CritSect);
  for (long i=0; i<m_nXmtrs; i++)
     m_pXmtrs[i]->Send(Evt,/*pBlk,*/Data);
  LeaveCriticalSection(&m_CritSect);
  };

//=========================================================================

CScdCOEventSenders gs_Events;

//=========================================================================
//
//
//
//=========================================================================
