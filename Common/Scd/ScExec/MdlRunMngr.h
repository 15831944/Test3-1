//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#ifndef __MDLRUNMNGR_H
#define __MDLRUNMNGR_H

#ifndef __EXECUTIV_H
#include "executiv.h"
#endif
//#ifndef __SFE_CLNT_H
//#include "sfe_clnt.h"
//#endif
#ifdef __MDLRUNMNGR_CPP
  #define DllImportExport DllExport
#elif !defined(SCEXEC)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//===========================================================================

class CBaseRunManager
  {
  public:
    CBaseRunManager() {};
    ~CBaseRunManager() {};

    //virtual flag Stopped(bool Done)=0; 
    //virtual flag Idling(bool Done)=0; 
    //virtual flag Running(bool Done)=0; 
    //virtual flag IdlingDsp(bool Done)=0; 
    //virtual flag RunningDsp(bool Done)=0; 

    virtual void GotoEditMode(bool Done)=0;
    virtual void GotoIdleMode(bool Done)=0;
    virtual void GotoRunMode(bool Done, CExecutive::RqdRunMode Md)=0;
    virtual void DoStep(bool Done, CDoOneStepInfo Info)=0;

    virtual void SetDynamicMode(bool Done)=0;
    virtual void SetProbalMode(bool Done)=0;
    virtual void DoEO_Starting()=0;
    virtual void DoEO_Execute()=0;
    
//    inline flag AutomationBusy() { return gs_Exec.AutomationBusy(); };
//    inline void SetAutomationBusy(bool Busy) { gs_Exec.bAutomationBusy = Busy; };
    //inline CXRefStatusArray& GetRefs() { return RefsHelper.GetRefs(); };

  };

class DllImportExport CMdlRunManager //: CBaseRunManager
  {
  public:
    CMdlRunManager();
    ~CMdlRunManager();

    flag Stopped(); 
    flag Idling(); 
    flag Running(); 
    flag IdlingDsp(); 
    flag RunningDsp(); 
    
    void GotoEditMode();
    void GotoIdleMode();
    void GotoRunMode(CExecutive::RqdRunMode Md, int SSAction);
    void DoStep(CDoOneStepInfo Info);

    void SetProbalMode();
    void SetDynFlowMode();
    void SetDynFullMode();
    void SetDynamicMode();
    //void SetDynFlowOrFull(bool DFlow);
    void DoEO_Starting();
    void DoEO_Execute();
    
    inline flag AutomationBusy() { return gs_Exec.AutomationBusy(); };
    inline void SetAutomationBusy(bool Busy) { gs_Exec.bAutomationBusy = Busy; };
//    inline CXRefStatusArray& GetRefs() { return RefsHelper.GetRefs(); };

  public:
    //valRefsHelper RefsHelper;
    CBaseRunManager *m_pAppMngr;
    bool m_ComUIActive;
  protected:
    //xxx
  private:
  };

extern DllImportExport CMdlRunManager gs_TheRunMngr;

#undef DllImportExport
#endif
