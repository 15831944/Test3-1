//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __DRYER_H
#define  __DRYER_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif  
#ifndef __M_SURGE_H
  #include "m_surge.h"
#endif  

#ifdef __DRYER_CPP
  #define DllImportExport DllExport
#elif !defined(MDLLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//=========================================================================== 

DEFINE_TAGOBJ(Dryer);

class Dryer : public MN_Surge
  {
  public:
    double         RqdFiltSolids, RqdCakeMoist;
    double         CakeMoist, TReqd;
    double         FiltSol, CakeLiq;
    double         m_EnergyRqd;
    double         dWaterEvaporated;
    flag           fOn;

    Dryer(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~Dryer (){};
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual void   EvalJoinPressures(long JoinMask);
    virtual void   EvalProducts(CNodeEvalIndex & NEI);
    virtual void   ClosureInfo();
  private:
  };

//=========================================================================== 

#undef DllImportExport

#endif
