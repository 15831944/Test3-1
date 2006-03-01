//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __REP_MNGR_H
#define  __REP_MNGR_H

#ifndef __EXECUTIV_H
#include "executiv.h"
#endif

// ==========================================================================

_FWDDEF(CRepExec)
class CRepExec : public CExecObj
  {
  public:
    CRepExec();
    virtual ~CRepExec();
    static void Start1(void* p);
    static void Start2(void* p);
    static void WriteData(void* p);
    static void End(void* p);

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
#endif


