//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __CMD_MNGR_H
#define  __CMD_MNGR_H

#ifndef __EXECUTIV_H
#include "executiv.h"
#endif

// ==========================================================================

_FWDDEF(CCmdExec)

// ==========================================================================

class CCmdBase
  {
  public:
    WORD        CmdID;       //unique command ID
    WORD        CmdDoneID;   //unique command/status ID
    //CCmdBase*   pNext;       //next command in linked list
    DWORD       flags;       //command flags

    CCmdBase(WORD ID, WORD DoneID=0);
    virtual ~CCmdBase();
    virtual int ExecCmd(CCmdExec* pXCmd) = 0;
    virtual flag ExecNow(CCmdExec* pXCmd) { return 0; };
    virtual char* Desc(char* Buff);
  };

class CDocComInfo
  {
  public:
    enum {DCI_Add, DCI_Remove, DCI_GetCount, DCI_GetTitle, DCI_FindTitle} m_DoWhat;
    CString  m_sIndex;
    long     m_lIndex;
    CString  m_sTitle;

    long     m_lCount;

    CDocComInfo()
      {
      m_DoWhat=DCI_Add;
      m_lIndex=0;
      m_lCount=0;
      }  
  };

// ==========================================================================

class CCmdExec : public CExecObj
  {
  public:
    CList <CCmdBase*, CCmdBase*> Cmds; //first command to be executed
    //CCmdBase*    pFirstCmd;    //first command to be executed
    //CCmdBase*    pLastCmd;     //last command to be executed
    //int          iCmdCount;    //number of commands in script
    CTimeValue   dStartTime;   //for running simulation timer
    CTimeValue   dCurTime;     //current simulation time
    DWORD        dwIterCount;  //syscad iteration counter
    int          iCmdAttempts; //
    WORD         wCmdWaiting;  //ID of command waiting to be completed before executing next command
    CCompareInfo CompInfo;     //parameters used by compare scenarios
    flag         bResetNextCompOutFile:1, //used by compare scenarios
                 bDoNotPrompt:1, //do not prompt for current command (eg when closing prj, save old prj)
                 bIdling:1,      //
                 bSaveOnExit:1,  // Set if Save is required on Exit
                 bNoSaveOnExit:1, // Set if Save is not required on Exit
                 bCOMCmdBusy;     // Set if COMCmd is busy
  public:
    CCmdExec();
    virtual ~CCmdExec();

  protected:
    void Reset();
    CCmdBase* AddCmd(CCmdBase* pNewCmd);
    void DelCmds();
  public:
    int AddCmd(char* Txt);
    int AddCmds(char* pFileName);
    int ExecCmds();
    int CmdComplete(WORD CmdID);
    void AbortScripts();
    inline int CmdCount() { return Cmds.GetCount(); };//iCmdCount; };
    inline flag DoNotPrompt() { return bDoNotPrompt || bCOMCmdBusy; };
    inline flag SaveOnExit() { return bSaveOnExit; };
    inline flag NoSaveOnExit() { return bNoSaveOnExit; };
    inline void SetIdle() { bIdling=1; };

    // Used by cominterface
    LRESULT DoCOMCmd(long From, long Cmd, CScdCOCmdBlk * pCmdBlk, long Data);
    LRESULT DoCOMEvt(/*long From,*/ WPARAM wParam, LPARAM lParam);

    // CExecObj Overrides
    virtual DWORD  EO_Message(CXMsgLst &XM, CXM_Route &Route);
    virtual flag   EO_QueryTime(CXM_TimeControl &CB, CTimeValue &TimeRqd, CTimeValue &dTimeRqd);
    virtual flag   EO_Start(CXM_TimeControl &CB);
    virtual void   EO_QuerySubsReqd(CXMsgLst &XM);
    virtual void   EO_QuerySubsAvail(CXMsgLst &XM, CXMsgLst &XMRet);
    virtual flag   EO_ReadSubsData(CXMsgLst &XM);
    virtual flag   EO_WriteSubsData(CXMsgLst &XM, flag FirstBlock, flag LastBlock);
    virtual flag   EO_Execute(CXM_TimeControl &CB, CEOExecReturn &EORet);
    virtual flag   EO_Stop(CXM_TimeControl &CB);
    virtual flag   EO_Stopping(flag fBeginStopping);
    virtual int    EO_CanClose(Strng_List & Problems);
  };

extern CCmdExec* gs_pXCmd;

inline CCmdExec & CmdExec() { return *gs_pXCmd; }
inline int RmtCmdBusy() { return gs_pXCmd ? ((gs_pXCmd->CmdCount()>0) || gs_pXCmd->bCOMCmdBusy) : False; }
//inline int ScriptRunning() { return gs_pXCmd ? gs_pXCmd->CmdCount()>0 : False; }

//=============================================================================
#endif


