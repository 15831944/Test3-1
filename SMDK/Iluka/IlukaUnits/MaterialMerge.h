#pragma once  
//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

//---------------------------------------------------------------------------

class MaterialMerge : public MBaseMethod
  {
  public:
    MaterialMerge(MUnitDefBase * pUnitDef, TaggedObject * pNd);
    ~MaterialMerge();
    virtual void Init();
    virtual void BuildDataFields();
    virtual bool ExchangeDataFields();
    virtual bool ValidateDataFields();

  public:
    virtual void EvalProducts();

  protected:
    int         m_sMaterialType;
  };

//---------------------------------------------------------------------------
