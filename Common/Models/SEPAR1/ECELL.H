//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __ECELL_H
#define  __ECELL_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif  
#ifndef __M_SURGE_H
  #include "m_surge.h"
#endif  

#ifdef __ECELL_CPP
  #define DllImportExport DllExport
#elif !defined(Billiton)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//=========================================================================== 

DEFINE_TAGOBJ(ElectroCell);

class ElectroCell : public MN_Surge
  {
  public:
    byte           iMetal;
    double         CathMass;  // Mass of Cathodes
    double         dCellEff;
    double         dPreHeatFrac;
    double         dHeatFlow;
    double         TempKFeed;
    double         TempKProd;

    ElectroCell(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~ElectroCell (){};
    virtual void   BuildDataDefn(DataDefnBlk & DDB);

    virtual void   EvalJoinPressures(long JoinMask);
    virtual void   EvalProducts(CNodeEvalIndex & NEI);
    virtual void   ClosureInfo();
  private:
  };

//=========================================================================== 

#undef DllImportExport

#endif
