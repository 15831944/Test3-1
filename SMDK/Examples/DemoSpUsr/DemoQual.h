//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __DemoQual_H_
#define __DemoQual_H_
#include "md_headers.h"
#include <vector>

DEFINE_SPECIEQUALITY(CDemoQual)
class CDemoQual : public MSpQualityBase
  {
  ATTACH_SPECIEQUALITY(CDemoQual)
                  
  public:
	  CDemoQual(TaggedObject * pNd);
	  ~CDemoQual();

    void           BuildDataFields();
    bool           ExchangeDataFields();
    bool           ValidateDataFields();

    // CPropertyBase Overides
    long           DefinedPropertyCount();
    long           DefinedPropertyInfo(long Index, MPropertyInfo & Info);

    DWORD          GetPropertyVisibility(long Index);
    void           GetPropertyValue(long Index, ULONG Phase, double T, double P, MPropertyValue & Value);
    void           PutPropertyValue(long Index, MPropertyValue & Value);

    long           m_SpCount;
    long           m_PSDCount;
    long           m_DSDCount;

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

    virtual void   SetMassF(MSpQualityBase * QualSet, MArray & MSet);
    virtual void   AddMassF(MSpQualityBase * QualAdd, MArray & MAdd);
    virtual void   SubMassF(MSpQualityBase * QualSub, MArray & MSub);

    virtual void   Copy(MVector &V2, MSpQualityBase * QualCopy);
    virtual void   AddDeriv(MVector &V2, MSpQualityBase * Qual2, double Sgn_, double DeltaTime);
    virtual void   AddDiscrete(MVector &V2, MSpQualityBase * Qual2, double Sgn_, double DeltaTime);

  };

#endif //__DemoQual_H_
