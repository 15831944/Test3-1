//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __STATS_H
#define  __STATS_H

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

#include <list>

#ifdef __STATS_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(SMDKDemoU)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif

//---------------------------------------------------------------------------

/*struct DoubleItem
{
public:
	double val;
	doubleItem* next;
}

struct DoubleIterator
{
public:
	DoubleIterator operator++(DoubleIterator iterator);
	double operator*(DoubleIterator iterator);
	double* Value();
protected:
	DoubleItem* item;
}

class DoubleList
{
public:
	DoubleList();
	~DoubleList();
	void AddValue(double Value);
	doubleIterator getStart();
	doubleIterator getEnd();
protected:
	doubleItem* start;
	doubleItem* end;
}*/

using namespace std;

//---------------------------------------------------------------------------

class SingleVarStats : public MBaseMethod
  {
  public:
    SingleVarStats(MUnitDefBase * pUnitDef, TaggedObject * pNd);
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
	
	virtual bool GetModelGraphic(CMdlGraphicArray &Grfs);
	virtual bool OperateModelGraphic(CMdlGraphicWnd &Wnd, CMdlGraphic &gfx);

  protected:
    bool bOn;

    CString sTag;
	long lTagID;
	double dHistoMin;
	double dHistoMax;
	long lHistoCount;

	double dAverage;
	double dStdDev;
	double dMin;
	double dMax;
	long* pHistoBucketCounts;
	double* pHistoBucketBorders;
	long lRecordCount;

	double dSumX;
	double dSumX2;
	list<double> cRecord;

	void Reset();
	void RecalculateStats(double newEntry);
	void RecalculateHistoBuckets();
  };

#endif