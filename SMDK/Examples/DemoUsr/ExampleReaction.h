#pragma once  
//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

//---------------------------------------------------------------------------

class SimpleReaction : public MBaseMethod
  {
  public:
    SimpleReaction(MUnitDefBase * pUnitDef, TaggedObject * pNd);
    virtual void Init();
    virtual void BuildDataFields();
    virtual bool ExchangeDataFields();
    virtual void EvalProducts();

  protected:
    //MReactionBlk    m_RB;
    MStream m_QProd;
    double m_dSolPrecip;
    double dFeedT;
    double dProdT;
  };

//---------------------------------------------------------------------------
