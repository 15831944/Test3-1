//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __RANDOM_FAILURE_H
#define  __RANDOM_FAILURE_H

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

#include <vector>

#ifdef __RANDOM_FAILURE_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(SMDKDemoU)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif

//---------------------------------------------------------------------------


enum PDFType { PDFType_Constant, PDFType_Exponential, PDFType_Normal };

struct FailureVariables
  {
  public:
	FailureVariables();

    double dAvgDowntime, dDowntimeStdDev;
	double dAvgUptime, dUptimeStdDev;
    CString sDescription;
    PDFType eFailureType;
	PDFType eRepairType;

	double dOnValue, dOffValue;
    
	double dNextFailure;	//When the task will next fail
	double dRepairsDone;	//When the task will be repaired
	double dBackedUpDowntime;	//How much downtime the task needs in actual iterations
    bool bRunning;			//Whether the task is running (in actual iterations)
    double dTotalDowntime;	//
	long lFailureCount;
  };

class RandomFailure : public MBaseMethod
  {
  public:
    RandomFailure(MUnitDefBase * pUnitDef, TaggedObject * pNd);
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
    std::vector<FailureVariables*> tasks;

    void SetSize(long size);
	void Reset();
	void RevalidateDataFields();
	bool CheckTags();

	void FailItem(FailureVariables& task);
	void RepairItem(FailureVariables& task);
	double CalculateEvent(PDFType ePDF, double dAverage, double dStdDev);
  };

#endif