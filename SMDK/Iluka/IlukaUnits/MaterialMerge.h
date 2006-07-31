#pragma once  
//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

#include <string>
#include <vector>

//---------------------------------------------------------------------------

class MaterialMerge : public MBaseMethod
  {
  //---------------------------------------------------------------------------

  int allCount;
  int destCount;
  int phaseCount;

  int *destIndex;
  std::string *destText;

  double **map;
  std::string **mapText;

  bool *makeObject;
  bool *enableA;
  bool *enableB;

  double *totals;

  MDDValueLst *targetTypeDropDown;

  std::vector<std::string> phases;
  std::vector<std::string> phasestmp;


  public:
    MaterialMerge(MUnitDefBase * pUnitDef, TaggedObject * pNd);
    ~MaterialMerge();
    virtual void Init();
    virtual void BuildDataFields();
    virtual bool ExchangeDataFields();
    virtual bool ValidateDataFields();

    void resetMappingVariables();

  public:
    virtual void EvalProducts();

  protected:
    int         m_sMaterialType;
  };

//---------------------------------------------------------------------------
