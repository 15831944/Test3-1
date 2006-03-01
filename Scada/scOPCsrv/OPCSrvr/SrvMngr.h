//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __SRVMNGR_H
#define  __SRVMNGR_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef  __SCDVER_H
#include "scdver.h"
#endif
#if WITHOPC
#ifndef __COMMSSRVRBASE_H
#include "CommsSrvrBase.h"
#endif

//--------------------------------------------------------------------------
class CScdOPCCallBack; //derived from CExecObj
class COPCSrvrStatsDlg;

class CScdOPCManager : public CCommsServerManagerBase
  {
  friend class COPCSrvrStatsDlg;
  friend class COPCSrvrOptionsDlg;
  friend class CScdOPCCallBack;
  public:
    static bool       sm_bInitOK;
    static CScdOPCManager* sm_pTheMngr;
    static bool       Init(LPCTSTR ExeFile, bool ResetReg, int Server=0);
    static bool       Stop();

  public:
    CScdOPCManager(CScdOPCCallBack* pCB);
    virtual ~CScdOPCManager();
    virtual char*     Name()                { return "OPC"; };
    virtual void      LoadOrSaveOptions(CProfINIFile & PF, bool Loading);
    virtual bool      ServerOK()            { return pTheCB!=NULL; };
    virtual bool      OpenStatusWnd();
    virtual bool      IsStatusWndOpen()     { return (pTheOPCStatsDlg!=NULL); };
    virtual bool      CloseStatusWnd();
    virtual void      UpdateStatusWnd(WPARAM wParam=0, LPARAM lParam=0);
    
    virtual bool      AllowInvalidTags()    { return m_bAllowInvalidTags; }
    virtual bool      Enabled()             { return m_bEnabled; };
    virtual bool      SetEnable(bool On);    

    CScdOPCCallBack*  OPCExec()             { return pTheCB; };
    bool              LogNotes()            { return bDoLogNotes; };



  protected:
    CScdOPCCallBack*  pTheCB;
    COPCSrvrStatsDlg* pTheOPCStatsDlg;
    bool              bShowStatsOnConnect,
                      bDoLogNotes,
                      bForceOnStart,
                      bForceOnStop;
    long              iForceCnt;
    bool              m_bEnabled;
    bool              m_bAllowInvalidTags;
  };

//--------------------------------------------------------------------------
#endif
#endif