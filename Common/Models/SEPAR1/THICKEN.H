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
    double         BedHgt;     // Height of slurry/liqour interface

    byte           iRqdOFMethod;
    double         RqdUFSolids;  // (MB) Required Underflow LS Ratio
    double         RqdOFSolids;  // (MB) Required Overflow SolidsFrac
    double         RqdOFSolConc; // (MB) Required Overflow Solids Conc @ 25
    double         IntOFSolidsUsed;
    double         UFSolids;   // Achieved Underflow Density
    double         OFSolids;   // Achieved Overflow Density(Clarity)
    double         ActUFSolidsConc25, ActOFSolidsConc25;
    double         ActUFSolRec; // Underflow solids recovery
    flag           bTrackStatus;

    C2DFn          UFSolidsFn; // Underflow Density vs. Bedhgt Function
    C2DFn          OFSolidsFn;  // Overflow Density vs. Bedhgt Function

    Thickener(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~Thickener();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);

    virtual void   EvalJoinPressures(long JoinMask);
    virtual void   EvalProducts(long JoinMask);
    // ConditionBlk Override
    DEFINE_CI(Thickener, MN_Surge, 6);
  private:
  };

//=========================================================================== 

#undef DllImportExport

#endif
