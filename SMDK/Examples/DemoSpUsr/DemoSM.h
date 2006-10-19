//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __DemoSM_H_
#define __DemoSM_H_
#include "md_headers.h"
#include <vector>

class CDemoSM : public MSpModelBase
  {
  public:
	  CDemoSM(TaggedObject *pNd);
	  ~CDemoSM();

    void              BuildDataFields();
    bool              ExchangeDataFields();
    bool              ValidateDataFields();

	  bool              get_IsBaseClassOf(LPCTSTR OtherProgID);
	  LPCTSTR           get_PreferredModelProgID();
	  double            get_Density(long Phases, double T, double P, MArray * pMA);
	  double            get_msEnthalpy(long Phases, double T, double P, MArray * pMA);
	  double            get_msEntropy(long Phases, double T, double P, MArray * pMA);
	  double            get_msCp(long Phases, double T, double P, MArray * pMA);
	  double            get_SaturationT(double P, MArray * pMA);
	  double            get_SaturationP(double T, MArray * pMA);
    double            get_DynamicViscosity(long Phases, double T, double P, MArray * pMA);
    double            get_ThermalConductivity(long Phases, double T, double P, MArray * pMA);

    //double            BoilPtElev(double T_, double MassL);

    long              DefinedPropertyCount();
    long              DefinedPropertyInfo(long Index, MPropertyInfo & Info);

    DWORD             GetPropertyVisibility(long Index);
    void              GetPropertyValue(long Index, ULONG Phase, double T, double P, MPropertyValue & Value);
    void              PutPropertyValue(long Index, MPropertyValue & Value);

    double            m_dDummyPropertyS;
    double            m_dDummyPropertyF;
  };

#endif //__DemoSM_H_
