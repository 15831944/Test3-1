#pragma once  
//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

//---------------------------------------------------------------------------

class SimpleHeater : public MBaseMethod
  {
  public:
    SimpleHeater(MUnitDefBase * pUnitDef, TaggedObject * pNd);
    virtual void Init();
    virtual void BuildDataFields();
    virtual void EvalProducts();
    virtual void ClosureInfo(MClosureInfo & CI);

  protected:
    bool          bOnLine;
    double        dRqdDuty;
    double        dActualDuty;
    double        dFeedT;
    double        dProdT;
  };

//---------------------------------------------------------------------------
