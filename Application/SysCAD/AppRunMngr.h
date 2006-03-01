//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#ifndef __APPRUNMNGR_H
#define __PRJEXEC_H

#ifndef __EXECUTIV_H
#include "executiv.h"
#endif
#ifndef __SFE_CLNT_H
#include "sfe_clnt.h"
#endif
#ifndef __MDLRUNMNGN_H
#include "mdlrunmngr.h"
#endif

//===========================================================================

class CAppRunManager : public CBaseRunManager
  {
  public:
    CAppRunManager();
    ~CAppRunManager();

    flag Stopped(bool Done); 
    flag Idling(bool Done); 
    flag Running(bool Done); 
    flag IdlingDsp(bool Done); 
    flag RunningDsp(bool Done); 

    void GotoEditMode(bool Done);
    void GotoIdleMode(bool Done);
    void GotoRunMode(bool Done, CExecutive::RqdRunMode Md);
    void DoStep(bool Done, CDoOneStepInfo Info);

    void SetDynamicMode(bool Done);
    void SetProbalMode(bool Done);
    void DoEO_Starting();
    void DoEO_Execute();
    
//    inline flag AutomationBusy() { return gs_Exec.AutomationBusy(); };
//    inline void SetAutomationBusy(bool Busy) { gs_Exec.bAutomationBusy = Busy; };

  public:
//    bool m_ComUIActive;
  };

#endif
