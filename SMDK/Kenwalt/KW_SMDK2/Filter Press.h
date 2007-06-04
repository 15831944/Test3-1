//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __FILTER_PRESS_H
#define  __FILTER_PRESS_H

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

#include <map>

#ifdef __FILTER_PRESS_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(SMDKDemoU)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif

//---------------------------------------------------------------------------

using namespace std;

enum FiltrateMethod { FM_SolidsToFiltrateFrac, FM_FiltrateConc };
enum WashMethod { WM_ConstantEfficiency, WM_WashRatio };

class FilterPress : public MBaseMethod
{
public:
    FilterPress(MUnitDefBase * pUnitDef, TaggedObject * pNd);
    virtual void    Init();
    virtual void    BuildDataFields();
	virtual bool	ExchangeDataFields();
	virtual bool	ValidateDataFields();

	virtual bool	EvalJoinPressures();
    virtual void    EvalProducts();

	virtual bool	PreStartCheck();

protected:
	FiltrateMethod eFiltrateMethod;

	//Liquor Seperation in filter:
	double dReqSolidsToFiltrate;	//[%] For SolidsToFiltrateFrac method
	double dReqFiltSolConc;			//[g/L] For FiltrateConc method
	double dSolidsToFiltUsed;		//[g/L] From either of the previous, we calculate this
	double dReqCakeMoisture;		//[%]

	//Wash Process:
	WashMethod eWashMethod;
	double dReqWashEfficiency;		//[%]
	
	//Results:
	double dWashRatio;				//[%]
	double dWashEfficiency;			//[%]
	double dCakeSolids;				//[%]
	double dFiltSolids;				//[%]
	double dCakeSolConc;			//[g/L]
	double dFiltSolConc;			//[g/L]
	int nWashCompSpecie;
	double dWashCompEff;	//[%]

	bool bTrackStatus;

	//Pressure stuff:
	double dPressureDrop;
	double dMembraneRes;
	double dViscosity;
	double dSpecCakeResistance;
	double dArea;

	bool bFilt2Washing, bCakeDischarge;

	void SetNote(int id, LPCSTR str);
};

#endif