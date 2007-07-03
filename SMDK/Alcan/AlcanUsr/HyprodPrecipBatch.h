//===========================================================================
//=== SysCAD SMDK - Hyprod Batch Precipitator Model 2003 : Alcan, Kenwalt ===
//===========================================================================

#ifndef  __HYPRODPRECIPBATCH_H
#define  __HYPRODPRECIPBATCH_H

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif
#ifndef __HYPRODCOMMON_H
#include "HyprodCommon.h"
#endif

#ifdef __HYPRODPRECIPBATCH_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(AlcanU)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif

//---------------------------------------------------------------------------

class BatchPrecip : public MBaseMethod, public HyprodPrecipHelper
  {
  public:
    BatchPrecip(MUnitDefBase * pUnitDef, TaggedObject * pNd);
    virtual void Init();
    virtual void BuildDataFields();
	  virtual bool ExchangeDataFields();
    virtual bool ValidateDataFields();
    virtual void EvalProducts();
    virtual void ClosureInfo(MClosureInfo & CI);

  protected:
  private:
    bool            m_bOn;
    long            m_lNumberInService;
    MStreamI        m_QProd;

    //Batch specific
    bool            m_bUseCircTime;
    bool            m_bHasLiquor;
    bool            m_bAgglomONwhileFilling;
    double          m_dNbTankAvailable ,
                    m_dFillingTempDrop ,
                    m_dSeedCharge      ,
                    m_dCirculationTime ,
                    m_dFillingTime     ,
                    m_dDrawOffTime     ,
                    m_dNbSeededDay     ,
                    m_dSeedingTime     ;

    double          PerformAluminaSolubility(MVector & Vec, double TRqd, double ARqd, double THARqd, double NoPerSec, double Na2OFac, double SSat, bool & ConvergeErr);
    bool            PrecipBatchSS(double dTime, MVector & Prod, double CurLevel);
    bool            PrecipBatch(double dTime, MVector & Prod, double CurLevel,bool AgglomON);
    bool            BatchCycle(MVector & Slurry, MVector & Liquor, MVector & Prod);
    bool            RunSteady(MVector & Slurry, MVector & Liquor, MVector & Prod);
  };

#undef DllImportExport

#endif