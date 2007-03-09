//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __SCHEDULED_MAINTENANCE_H
#define  __SCHEDULED_MAINTENANCE_H

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

#include <vector>

#ifdef __SCHEDULED_MAINTEANCE_CPP
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
    double dDesiredDowntime, dDowntime;
	double dDesiredPeriod, dPeriod;
	double dOffset, dNextShutdown;
    CString sDescription;

	CString sTag; int nTagID;
	double dOnValue, dOffValue;
    
    bool bRunning;
    double dTotalDowntime, dBackedUpDowntime;
  };

class ScheduledMaintenance : public MBaseMethod
  {
  public:
    ScheduledMaintenance(MUnitDefBase * pUnitDef, TaggedObject * pNd);
    virtual void    Init();
    virtual void    BuildDataFields();
    virtual bool    ExchangeDataFields();
	virtual bool	ValidateDataFields();

	virtual bool PreStartCheck();

    //virtual void    EvalProducts();
    virtual void    EvalCtrlInitialise(eScdCtrlTasks Tasks=CO_All); // each Start of Run
    virtual void    EvalCtrlActions(eScdCtrlTasks Tasks=CO_All)   ; // each Iteration - To apply result of control
    //virtual void    EvalCtrlStrategy(eScdCtrlTasks Tasks=CO_All); // each Iteration - To calculate required control actions
    //virtual void    EvalCtrlTerminate(eScdCtrlTasks Tasks=CO_All)           {}; // each End of Run
    //virtual void    EvalStatistics(eScdCtrlTasks Tasks=CO_All)              {}; // each Iteration End to calculate Stats relevant to model

  protected:
    bool bOn;

    double dCurrentTime;
	bool bForceIntegralPeriod, bForceIntegralDowntime;
    std::vector<MaintVariables> tasks;

    void SetSize(long size);
	void Reset();
	void RevalidateParameters();
  };

#endif