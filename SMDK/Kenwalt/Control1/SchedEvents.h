//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __SCHEDEVENTS_H
#define  __SCHEDEVENTS_H

#if defined(Control1)
#include "ex_headers.h"
#else
#include "md_headers.h"
#endif

#include <vector>

#ifdef __SCHEDEVENTS_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(SMDKDemoU)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif

//---------------------------------------------------------------------------

struct MaintVariables
  {
  public:
	MaintVariables(MTagIO & TagIO);

    double dDesiredDowntime, dDowntime;
    double dDesiredPeriod, dPeriod;
    double dOffset, dNextShutdown, dNextAbsoluteShutdown, dTimeToNextShutdown;
    CString sDescription;

    MTagIOSubscription tagSubs;
    double dOnValue, dOffValue;

    bool bRunning;
    double dTotalDowntime, dBackedUpDowntime;
  };

class ScheduledEvents : public MBaseMethod
  {
  public:
    ScheduledEvents(MUnitDefBase * pUnitDef, TaggedObject * pNd);
    virtual void    Init();
    virtual void    BuildDataFields();
    virtual bool    ExchangeDataFields();
    virtual bool    ValidateDataFields();

    virtual bool    PreStartCheck();
    virtual void    SetState(MStatesToSet SS);

    //virtual void    EvalProducts();
    virtual void    EvalCtrlInitialise(eScdCtrlTasks Tasks=CO_All); // each Start of Run
    virtual void    EvalCtrlActions(eScdCtrlTasks Tasks=CO_All)   ; // each Iteration - To apply result of control
    //virtual void    EvalCtrlStrategy(eScdCtrlTasks Tasks=CO_All); // each Iteration - To calculate required control actions
    //virtual void    EvalCtrlTerminate(eScdCtrlTasks Tasks=CO_All)           {}; // each End of Run
    //virtual void    EvalStatistics(eScdCtrlTasks Tasks=CO_All)              {}; // each Iteration End to calculate Stats relevant to model

  protected:
    bool bOn;

    double dCurrentTime;
    bool bForceIntegralPeriod;
    bool bForceIntegralDowntime;
    std::vector<MaintVariables*> tasks;

    void SetSize(long size);
    void Reset();
	void RevalidateParameters();
	bool CheckTags();
  };

#endif
