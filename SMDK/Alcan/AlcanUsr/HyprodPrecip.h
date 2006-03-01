//===========================================================================
//=== SysCAD SMDK - Hyprod Precipitator Model 2003 : Alcan, Kenwalt       ===
//===========================================================================

#ifndef  __HYPRODPRECIP_H
#define  __HYPRODPRECIP_H

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif
#ifndef __HYPRODCOMMON_H
#include "HyprodCommon.h"
#endif

#ifdef __HYPRODPRECIP_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(AlcanU)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif

//---------------------------------------------------------------------------

//DEFINE_SURGEMETHOD(Precipitator)

class Precipitator : public MBaseMethod, public HyprodPrecipHelper
  {
  friend class BatchPrecip;
  public:
    Precipitator(MUnitDefBase * pUnitDef, TaggedObject * pNd);
    virtual void Init();
    virtual void BuildDataFields();
    virtual bool ExchangeDataFields();
    virtual bool ValidateDataFields();
    virtual void EvalProducts();
    virtual void ClosureInfo(MClosureInfo & CI);

  protected:

  private:
    //inputs:
    bool            m_bOn;
    bool            m_bBypassOn;
    double          m_dBypass;
    MStream         m_QProd;

    //results:
    double          m_dBypassFlow;

    bool            Precip(MVector & Feed, MVector & Prod, double Factor);
    bool            RunSteady(MStream & Feed, MStream & Prod);
  };

//---------------------------------------------------------------------------

#undef DllImportExport

#endif