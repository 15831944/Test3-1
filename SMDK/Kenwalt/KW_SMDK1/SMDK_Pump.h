//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

//---------------------------------------------------------------------------

class CPumpValve : public MBaseMethod
  {
  public:
    CPumpValve(MUnitDefBase * pUnitDef, TaggedObject * pNd);
    virtual ~CPumpValve(void);

    virtual void Init();
    virtual void BuildDataFields();
    virtual bool ExchangeDataFields();
    virtual bool ValidateDataFields();
    virtual void EvalProducts();
    virtual bool EvalJoinPressures();
    virtual void ClosureInfo(MClosureInfo & CI);

  protected:
    // Params
    bool           bIsentropic;
    long           iModel;
    long           iPressMethod;
    double         dPressBoost_Rqd;
    double         dPout_Rqd;
    double         dTout_Rqd;
    double         dDuty_Rqd;

    // Results
    double         dTout;
    double         dPout;
    double         dDuty;
    double         dTin;
    double         dPin;
  };

//---------------------------------------------------------------------------
