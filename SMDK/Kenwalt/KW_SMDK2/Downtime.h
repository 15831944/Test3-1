//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __DOWNTIME_H
#define  __DOWNTIME_H

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

#ifndef __TIMINGSTRUCTS_H
#include "TimingStructs.h"
#endif

#include <vector>

#ifdef __DOWNTIME_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(SMDKDemoU)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif

//---------------------------------------------------------------------------

struct DowntimeVariables
  {
  public:
	DowntimeVariables(MTagIO & TagIO);

	ScheduledTiming MajorMaintenance;
	ScheduledTiming MinorMaintenance;

	FailureTiming MajorFailure;
	FailureTiming MinorFailure;

	MTagIOSubscription TagToSet;
	double dOnValue, dOffValue;
	MTagIOSubscription TagToTest;
	double dOnThreshold;
	double dTestValue;

	DT_RunningStates eCurrentState;

	CString sDescription;

	double dTotalDowntime;

	void Reset();
	bool ValidateDataFields(double deltaT, bool bForceIntegralPeriod, bool bForceIntegralDowntime);
	void RevalidateParameters(double deltaT, bool bForceIntegralPeriod, bool bForceIntegralDowntime);
  };

class Downtime : public MBaseMethod
  {
  public:
    Downtime(MUnitDefBase * pUnitDef, TaggedObject * pNd);
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
	bool bAutoReset;

    double dCurrentTime;
    bool bForceIntegralPeriod;
    bool bForceIntegralDowntime;
    std::vector<DowntimeVariables*> tasks;

    void SetSize(long size);
    void Reset();
	bool CheckTags();
  };

#endif