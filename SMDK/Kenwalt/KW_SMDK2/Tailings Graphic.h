//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __TAILINGS_GRAPHIC_H
#define  __TAILINGS_GRAPHIC_H

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

#include <vector>

#ifdef __TAILINGS_GRAPHIC_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(SMDKDemoU)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif

using namespace std;

//---------------------------------------------------------------------------

enum InterpolationMethods {IM_Linear};

class TailingsGraphic : public MBaseMethod
  {
  public:
    TailingsGraphic(MUnitDefBase * pUnitDef, TaggedObject * pNd);
    virtual void    Init();
    virtual void    BuildDataFields();
    virtual bool    ExchangeDataFields();
	virtual bool	ValidateDataFields();

	virtual bool PreStartCheck();

    //virtual void    EvalProducts();
    //virtual void    EvalCtrlInitialise(eScdCtrlTasks Tasks=CO_All); // each Start of Run
    virtual void    EvalCtrlActions(eScdCtrlTasks Tasks=CO_All)   ; // each Iteration - To apply result of control
    //virtual void    EvalCtrlStrategy(eScdCtrlTasks Tasks=CO_All); // each Iteration - To calculate required control actions
    //virtual void    EvalCtrlTerminate(eScdCtrlTasks Tasks=CO_All)           {}; // each End of Run
    //virtual void    EvalStatistics(eScdCtrlTasks Tasks=CO_All)              ; // each Iteration End to calculate Stats relevant to model

	//virtual void SetState(MStatesToSet SS);
	
	virtual bool GetModelGraphic(CMdlGraphicArray &Grfs);
	virtual bool OperateModelGraphic(CMdlGraphicWnd &Wnd, CMdlGraphic &gfx);

  protected:
	//User entered data:
	CString sTankName;
	double dMoistFrac, dSatConc, dEvapRate, dRainRate;
	int nDataPointCount;
	vector<double> vInterpolationHeights, vInterpolationAreas;
	vector<double> vSortedHeights, vSortedAreas;
	InterpolationMethods eIntMethod;
	long lEvapRateUnits;
	
	//Model calculated data:
    //double dTopArea;
	double dMaxCapacity;
	vector<double> vVolumeLookup;
	double dFluidLevel, dSolidLevel, dFSA;

	//Data read from the tank:
    MTagIOSubscription FluidMassSubs, SolidMassSubs;
	double dFluidMass, dSolidMass;
	MTagIOSubscription ConcSubs;
	double dConc;
	CString sConcSpecies;
	MTagIOSubscription VolSubs, LiquidDensitySubs;

	//Functions:
	void SetIntPtCount(int count);
	void SortInterpolationPoints();
	void RecalculateVolumes();
	bool RecalculateLevels();
	double CalcLevel(double volume);
	double CalcArea(double height);

	void SetTags();

	POINT* GetDamPoints(double damHeight, CRect insideRect, int& ptCount);
  };

#endif