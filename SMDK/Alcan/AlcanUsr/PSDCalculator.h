//===========================================================================
//=== SysCAD SMDK - Hyprod PSD Definition Model 2006 : Alcan, Kenwalt     ===
//===========================================================================

#ifndef  __PSDCALCULATOR_H
#define  __PSDCALCULATOR_H

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

#ifdef ____PSDCALCULATOR_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(AlcanU)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif

//---------------------------------------------------------------------------

class PSDCalculator : public MBaseMethod
  {
  public:
    PSDCalculator(MUnitDefBase * pUnitDef, TaggedObject * pNd);
    virtual ~PSDCalculator();
    virtual void Init();
    virtual void BuildDataFields();
    virtual bool ExchangeDataFields();
    virtual void EvalProducts();

  protected:
    PSDPersonality *personality;
    bool bSetThePSD;

    void SetPersonality();
  };

#endif