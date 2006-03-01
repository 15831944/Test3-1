#pragma once  
//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

//---------------------------------------------------------------------------

class SimpleReactor : public MBaseMethod
  {
  public:
    SimpleReactor(MUnitDefBase * pUnitDef, TaggedObject * pNd);
    virtual void Init();
    virtual void BuildDataFields();
    virtual bool ExchangeDataFields();
    virtual void EvalProducts();

  protected:
    MReactionBlk    m_RB;
    CString         m_sDesc;
    double          m_dFeedT;
    double          m_dProdT;
  };

//---------------------------------------------------------------------------
