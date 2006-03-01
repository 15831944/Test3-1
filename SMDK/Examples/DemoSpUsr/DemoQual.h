//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __DemoQual_H_
#define __DemoQual_H_
#include "..\..\..\SMDK\include\md_headers.h"
#include <vector>

class CDemoQual : public MSpQualityBase
  {
  public:
	  CDemoQual(TaggedObject * pNd);
	  ~CDemoQual();

    // CPropertyBase Overides
    long           DefinedPropertyCount();
    long           DefinedPropertyInfo(long Index, MPropertyInfo & Info);

    DWORD          GetPropertyVisibility(long Index);
    void           GetPropertyValue(long Index, ULONG Phase, double T, double P, MPropertyValue & Value);
    void           PutPropertyValue(long Index, MPropertyValue & Value);

    double         m_dSG;
    double         m_dSetProp;


    // SpQuality Overides
    virtual void   ExchangeSpecies(MSpQualityBase * Other);

    virtual bool   EquilRqd4HEval();// { return false; };
    virtual void   Equilibrate();

    virtual void   ZeroMass();
    virtual void   ZeroDeriv();
    virtual void   ScaleMass(long Phase, double Mult);
    virtual void   ScaleMass(CArray<int,int> &SpIds, double Mult);

    virtual void   SetMassF(MSpQualityBase * QualSet);
    virtual void   AddMassF(MSpQualityBase * QualAdd, MArray & MAdd);
    virtual void   SubMassF(MSpQualityBase * QualSub, MArray & MSub);

    virtual void   Copy(MSpQualityBase * QualCopy);
    virtual void   AddDeriv(MSpQualityBase * Qual2, double Sgn_);
    virtual void   AddDiscrete(MSpQualityBase * Qual2, double Sgn_);

  };

#endif //__DemoQual_H_
