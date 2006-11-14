//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __SCHEDULED_MAINTENANCE_H
#define  __SCHEDULED_MAINTENANCE_H

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

//#include <stdatl>
#include <vector>

#ifdef __SCHEDULED_MAINTEANCE_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(SMDKDemoU)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif

//---------------------------------------------------------------------------


enum Type { Type_Simple, Type_nWeekly, Type_nMonthly, Type_nAnnually};
enum Strategy { Strat_Error, Strat_Warning, Strat_Carry };

struct MaintVariables
  {
  public:
    double dDownTime, dPeriod, dOffset;
    long lPeriod;
    CString sDescription;
    Type eType;
    
    bool bRunning;
    double dTotalDowntime;
  };

class ScheduledMaintenance : public MBaseMethod
  {
  public:
    ScheduledMaintenance(MUnitDefBase * pUnitDef, TaggedObject * pNd);
    virtual void    Init();
    virtual void    BuildDataFields();
    virtual bool    ExchangeDataFields();

    //virtual void    EvalProducts();
    virtual void    EvalCtrlInitialise(eScdCtrlTasks Tasks=CO_All); // each Start of Run
    virtual void    EvalCtrlActions(eScdCtrlTasks Tasks=CO_All)   ; // each Iteration - To apply result of control
    //virtual void    EvalCtrlStrategy(eScdCtrlTasks Tasks=CO_All); // each Iteration - To calculate required control actions
    //virtual void    EvalCtrlTerminate(eScdCtrlTasks Tasks=CO_All)           {}; // each End of Run
    //virtual void    EvalStatistics(eScdCtrlTasks Tasks=CO_All)              {}; // each Iteration End to calculate Stats relevant to model

  protected:
    bool bOn;
    Type eType;
    Strategy eStrategy;

    double dCurrentTime;

    std::vector<MaintVariables> tasks;
  };

#endif