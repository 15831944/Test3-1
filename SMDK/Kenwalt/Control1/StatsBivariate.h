//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __STATSBIVARIATE_H
#define  __STATSBIVARIATE_H

#if defined(Control1)
#include "ex_headers.h"
#else
#include "md_headers.h"
#endif

#include <list>

#ifdef __STATSBIVARIATE_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(SMDKDemoU)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif

//#define MVS_KEEP_RECORD

//---------------------------------------------------------------------------

using namespace std;

//---------------------------------------------------------------------------

struct ValuePair
{
public:
	double v1, v2;
};

enum GraphType { GT_SCATTER, GT_DENSITY };

class BivarStats : public MBaseMethod
  {
  public:
    BivarStats(MUnitDefBase * pUnitDef, TaggedObject * pNd);
    virtual void    Init();
    virtual void    BuildDataFields();
    virtual bool    ExchangeDataFields();
	virtual bool	ValidateDataFields();

	virtual bool PreStartCheck();

    //virtual void    EvalProducts();
    virtual void    EvalCtrlInitialise(eScdCtrlTasks Tasks=CO_All); // each Start of Run
    //virtual void    EvalCtrlActions(eScdCtrlTasks Tasks=CO_All)   ; // each Iteration - To apply result of control
    virtual void    EvalCtrlStrategy(eScdCtrlTasks Tasks=CO_All); // each Iteration - To calculate required control actions
    //virtual void    EvalCtrlTerminate(eScdCtrlTasks Tasks=CO_All)           {}; // each End of Run
    //virtual void    EvalStatistics(eScdCtrlTasks Tasks=CO_All)              ; // each Iteration End to calculate Stats relevant to model

	virtual void SetState(MStatesToSet SS);
	
	virtual bool GetModelGraphic(CMdlGraphicArray &Grfs);
	virtual bool OperateModelGraphic(CMdlGraphicWnd &Wnd, CMdlGraphic &gfx);

  protected:
    bool bOn;
	bool bAutoReset;

	GraphType graphType;

	MTagIOSubscription tagSubs0, tagSubs1;
	MCnv tagCnv0, tagCnv1;
	bool bTag0Initialised, bTag1Initialised;
	double dHistoMinX, dHistoMinY;
	double dHistoMaxX, dHistoMaxY;
	long lHistoCount;

	double dCorrelation;
	double dValue[2];      /*CNM*/
	double dAverage[2];
	double dStdDev[2];
	double dMin[2];
	double dMax[2];
	long* pHistoBucketCounts;
	long lRecordCount;

	double dSumX[2];
	double dSumX2[2];
	double dSumXY;
#ifdef MVS_KEEP_RECORD
	list<ValuePair> cRecord;
#endif

	void Reset();
	void RecalculateStats(double newEntry1, double newEntry2);
	void RecalculateHistoBuckets();
  };

#endif
