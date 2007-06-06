//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __FLOATATION_TANK_H
#define  __FLOATATION_TANK_H

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

#include <vector>

#ifdef __FLOATATION_TANK_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(SMDKDemoU)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif

//---------------------------------------------------------------------------

using namespace std;

enum FT_SpecType {FTST_ByCompound, FTST_ByElement};

class FloatationTank : public MBaseMethod
  {
  public:
    FloatationTank(MUnitDefBase * pUnitDef, TaggedObject * pNd);
    virtual void    Init();
    virtual void    BuildDataFields();
	virtual bool	ExchangeDataFields();
	virtual bool	ValidateDataFields();

    virtual void    EvalProducts();
	virtual bool	PreStartCheck();

  protected:
	FT_SpecType			eSpecType;
	vector<MDDValueLst>	vSolidElements;

    int					nPrimary1, nPrimary2;
	vector<int>			vPrimaryIndices;

	double				dPrimaryRecovery, dPrimaryGrade;
	double				dReqPrimaryRecovery, dReqPrimaryGrade;
	CString				sPrimariesFound;

	vector<int>			vSecondaryIndices;
	//vector<vector<MDDValueLst>> vSecondaryMDDLists;
	vector<double>		vSecondaryRecoveries, vReqSecondaryRecoveries;

	vector<int>			vOtherIndices; //Keep it as a class variable to allow optimization.
	CString				sOthersFound;

	double				dWaterFrac, dReqWaterFrac;

	void UpdatePrimaryIndices();
	void UpdateOtherIndices();
	//void UpdateMDDLists();
	void SetSecondaryCount(int newSize);
  };

#endif