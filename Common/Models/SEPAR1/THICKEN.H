//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __THICKEN_H
#define  __THICKEN_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif  
#ifndef __M_SURGE_H
  #include "m_surge.h"
#endif  
#ifndef __2D_FN_H
  #include "2d_fn.h"
#endif  

#ifdef __THICKEN_CPP
  #define DllImportExport DllExport
#elif !defined(Separ1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//=========================================================================== 

DEFINE_TAGOBJ(Thickener);

class Thickener : public MN_Surge
  {
  public:
    double         m_ActBedHgt;     // Height of slurry/liqour interface

    byte           m_iRqdOFMethod;
    double         m_RqdUFSolids;  // (MB) Required Underflow LS Ratio
    double         m_RqdOFSolids;  // (MB) Required Overflow SolidsFrac
    double         m_RqdOFSolConc; // (MB) Required Overflow Solids Conc @ 25
    double         m_RqdBedHgt;    // (MB) Bed Level at which these occur 

    double         m_IntOFSolidsUsed;
    double         m_UFSolids;   // Achieved Underflow Density
    double         m_OFSolids;   // Achieved Overflow Density(Clarity)
    double         m_ActUFSolidsConc25;
    double         m_ActOFSolidsConc25;
    double         m_ActUFSolRec; // Underflow solids recovery
    flag           m_bTrackStatus;

    C2DFn          UFSolidsFn; // Underflow Density vs. Bedhgt Function
    C2DFn          OFSolidsFn;  // Overflow Density vs. Bedhgt Function

    Thickener(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~Thickener();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);

    virtual void   EvalJoinPressures(long JoinMask);
    virtual void   EvalProducts(CNodeEvalIndex & NEI);
    // ConditionBlk Override
    DEFINE_CI(Thickener, MN_Surge, 6);
  private:
  };

//=========================================================================== 

#undef DllImportExport

#endif
