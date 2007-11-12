//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __STATSSINGLEVAR_H
#define  __STATSSINGLEVAR_H

#if defined(Control1)
#include "ex_headers.h"
#else
#include "md_headers.h"
#endif

#include <list>
#include <vector>

#ifdef __STATSSINGLEVAR_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(SMDKDemoU)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif

//#define SVS_KEEP_RECORD
#define HI_RES_HISTO 200

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

	virtual void SetState(MStatesToSet SS);
	
	virtual bool GetModelGraphic(CMdlGraphicArray &Grfs);
	virtual bool OperateModelGraphic(CMdlGraphicWnd &Wnd, CMdlGraphic &gfx);

  protected:
    bool bOn;
	bool bAutoReset;

	MTagIOSubscription tagSubs;
	MCnv TagCnv;
	MCnvFamily TagCnvFamily;
	int nTagCnvUsed;
	vector<MDDValueLst> TagCnvList;

	CString sGraphUnit;

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
#ifdef SVS_KEEP_RECORD
	list<double> cRecord;
#else
	long lHiResHistoBuckets[HI_RES_HISTO];
	long lUnderrange, lOverrange;
	long lRecordsSinceHistoReset;
#endif
	void SetTag(LPCSTR newTag);
	vector<MDDValueLst> Cnvs;

	void Reset();
	void RecalculateStats(double newEntry);
	void RecalculateHistoBuckets();
  };

#endif
