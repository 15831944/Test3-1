//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __OLE_MNGR_H
#define  __OLE_MNGR_H

#ifndef __EXECUTIV_H
#include "executiv.h"
#endif
#ifndef __OLE_BASE_H
#include "ole_base.h"
#endif

// ==========================================================================

_FWDDEF(COleExec)
class COleExec : public CExecObj
  {
  public:
    DWORD            dwThreadId;  //ID of thread that services OLE automation

    COleExec();
    virtual ~COleExec();

    // CExecObj Overrides
    virtual DWORD  EO_Message(CXMsgLst &XM, CXM_Route &Route);
    virtual flag   EO_QueryTime(const CXM_TimeControl &CB, double &TimeRqd, double &dTimeRqd);
    virtual flag   EO_Start(const CXM_TimeControl &CB);
    virtual void   EO_QuerySubsReqd(CXMsgLst &XM);
    virtual void   EO_QuerySubsAvail(CXMsgLst &XM, CXMsgLst &XMRet);
    virtual flag   EO_ReadSubsData(CXMsgLst &XM);  
    virtual flag   EO_WriteSubsData(CXMsgLst &XM, flag FirstBlock, flag LastBlock);
    virtual flag   EO_Execute(const CXM_TimeControl &CB, CEOExecReturn &EORet);
    virtual flag   EO_Stop(const CXM_TimeControl &CB);
    virtual int    EO_CanClose(Strng_List & Problems);
  };

//===========================================================================

_FWDDEF(COleThread)
class COleThread : public CWinThread
  {
  public:
    DECLARE_DYNCREATE(COleThread);

    int iQueueCnt;                   //number of items in queue
  private:
    COleAutoQueueItem* pQueue;       //pointer to start of queue of OLE items to be serviced
    CRITICAL_SECTION QueueSection;   //used for locks
    flag bQuitNow:1;                 //abort OLE stuff and quit

    inline void Lock() { EnterCriticalSection(&QueueSection); };
    inline void Unlock() { LeaveCriticalSection(&QueueSection); };
  public:
    inline BOOL IsBusy() { return (iQueueCnt>0); };
    void AddQueueItem(COleBaseAuto* pNewItem);
    void CancelQueueItems();
    virtual BOOL InitInstance();
    virtual int ExitInstance();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
  };

//=============================================================================
#endif


