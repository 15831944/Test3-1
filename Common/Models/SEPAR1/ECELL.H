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

DEFINE_TAGOBJ(CElectroCell);

class CElectroCell : public MN_Surge
  {
  public:
    CElectroCell(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CElectroCell (){};
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    virtual void   PostConnect(int IONo);
    virtual void   PreDisConnect(int IONo);

    virtual void   EvalJoinPressures(long JoinMask);
    virtual void   EvalProducts(CNodeEvalIndex & NEI);
    virtual void   ConvergeStates(CConvergeStateBlk &CSB);
    virtual void   ClosureInfo();

  protected:
    byte           iMetal;
    double         CathMass;  // Mass of Cathodes
    double         dCellEff;
    double         dPreHeatFrac;
    double         m_dElecEnergyTotal;
    double         m_dElecEnergyReact;
    double         m_dElecEnergyHeat;
    double         m_TempKFeed;
    double         m_TempKProd;
  };

//=========================================================================== 

#undef DllImportExport

#endif
