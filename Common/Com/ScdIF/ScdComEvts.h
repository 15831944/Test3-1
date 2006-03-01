// ComCom.h: interface for the CComCom class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCDCOMEVTS_H__00BA0271_074E_4644_97D6_0F13D81677E9__INCLUDED_)
#define AFX_SCDCOMEVTS_H__00BA0271_074E_4644_97D6_0F13D81677E9__INCLUDED_

//#if _MSC_VER > 1000
//#pragma once
//#endif // _MSC_VER > 1000

#include <atlbase.H>
#include "scdif.h"
#include "scdcomcmds.h"


#if defined(__SCDCOMEVTS_CPP)
  #define DllImportExport __declspec(dllexport)
#elif !defined(SCDIF)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif

//=========================================================================

#include <vector>
#include "scdcomcmds.h"


class CScdCOCmdBase; // forward

class DllImportExport CScdCOCmdBlk
  {
  public:
     long              m_lCmd;
     long              m_lData;

     CScdCOCmdBlk(long Cmd, long Data)
       {
       m_lCmd    = Cmd;
       m_lData   = Data;
       };
  };

class DllImportExport CScdCOCmdWaiting
  {
  public:
    CScdCOCmdBase * m_pEvtBlk;
    long            m_lEvId;

  };

class DllImportExport CScdCOCmdBase
  {
  friend class CScdCOCmdBaseInit;

  public:
  enum 
    {
    SS_CallReturned = 1,
    SS_CallTimeout  = 2,
    SS_CallASync    = 3
    };

    static HWND s_hWnd4Msgs;

    CScdCOCmdBase(DWORD MsgID);
    ~CScdCOCmdBase();

  private:

    DWORD         m_MsgID;
    VARIANT_BOOL  m_bSyncCalls;    // Calls are syncronous if true
  //  VARIANT_BOOL  m_bEventsOn;     // Events are generated
    DWORD         m_dwSyncTimeOut; // Timeout wait for sync calls;

    // these events are triggered when event received
    long          m_iFirstEventId;
    HANDLE        m_hEventRcvd[MaxComEvents];
    LPARAM        m_lEventData[MaxComEvents];

    // Utility Methods
  protected:
    void          InitCOCmdStuff(long EvId);
    UINT          ExecCOCmdStuff(long EvId);
    void          InitCOCmdStuff(long *EvIds, long nEvts);
    UINT          ExecCOCmdStuff(long *EvIds, long nEvts);

    void          AddChildEvents(IUnknown * pUnk);

    void          AppendWaitingEventBlk(long EvId, CScdCOCmdBase *pEvtBlk);
    void          AppendWaitingEventBlk(long *EvIds, long nEvts, CScdCOCmdBase *pEvtBlk);
    void          RemoveWaitingEventBlk(CScdCOCmdBase *pEvtBlk);

  public:
    LRESULT       SendComCmd(long Cmd, long Data=0);
    UINT          PostComCmd(long Cmd, long Data=0);
    UINT          PostComCmd(long Cmd, long *EvIds, long nEvts, long Data=0);
    static void   FireEventMsgs(WPARAM w, LPARAM l);
    void          DoEventMsg(long Evt, long Data);
    virtual void  FireTheEvent(long Evt, long Data) = 0;
  };

//=========================================================================

class DllImportExport CScdCOEventSender
  {
  public:
    CScdCOEventSender(DWORD ThreadId=0, DWORD MsgEvtId=0);
    ~CScdCOEventSender();

    void Init(DWORD ThreadId, DWORD MsgEvtId);
    void Send(long Evt, /*CScdCOCmdBlk * pBlk,*/ long Data);

  protected:
    DWORD m_dwThreadId;
    DWORD m_dwEvtMsgId;
  };

//=========================================================================

class DllImportExport CScdCOEventSenders
  {
  public:
    CScdCOEventSenders();
    ~CScdCOEventSenders();

    void Add(CScdCOEventSender * p);
    void Remove(CScdCOEventSender * p);
    void Send(long Evt, /*CScdCOCmdBlk * pBlk,*/ long Data);

  protected:
    long             m_nXmtrs;
    CScdCOEventSender   * m_pXmtrs[5];
    CRITICAL_SECTION m_CritSect;

  };

extern DllImportExport CScdCOEventSenders gs_Events;

//=========================================================================

#undef DllImportExport


#endif // !defined(AFX_SCDCOMEVTS_H__00BA0271_074E_4644_97D6_0F13D81677E9__INCLUDED_)
