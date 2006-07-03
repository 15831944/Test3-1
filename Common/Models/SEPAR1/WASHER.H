//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __WASHER_H
#define  __WASHER_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __M_SURGE_H
  #include "m_surge.h"
#endif
#ifndef __2D_FN_H
  #include "2d_fn.h"
#endif

#ifdef __WASHER_CPP
  #define DllImportExport DllExport
#elif !defined(Separ1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//===========================================================================

DEFINE_TAGOBJ(Washer);

class Washer : public MN_Surge
  {
  public:
		double				 MixEff;     // The degree of imperfect mixing in the washer
    flag           bMixEffLiqOnly; //does mixing effeceincy apply to liquids only
    double         BedHgt;     // Height of slurry/liqour interface

    byte           iRqdOFMethod;
    byte           iRqdBypassMethod; //does the bypass go to overflow or underflow
    double         RqdUFSolids;  // (MB) Required Underflow LS Ratio
    double         RqdOFSolids;  // (MB) Required Overflow SolidsFrac
    double         RqdOFSolConc; // (MB) Required Overflow Solids Conc @ 25
    double         IntOFSolidsUsed;
    double         IntUFSolidsUsed;
    double         UFSolids;   // Achieved Underflow Density
    double         OFSolids;   // Achieved Overflow Density(Clarity)
    double         BypassQm;   // Total flowrate of bypass from input stream
    double         ActUFSolidsConc25, ActOFSolidsConc25;
    byte           iScanEffSpecie; //eg caustic
    double         ActScandrettEff;
    flag           bTrackStatus;

    C2DFn          UFSolidsFn; // Underflow Density vs. Bedhgt Function
    C2DFn          OFSolidsFn;  // Overflow Density vs. Bedhgt Function

		SpConduit			 Qw;

    Washer(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~Washer();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);

    virtual void   EvalJoinPressures(long JoinMask);
    virtual void   EvalProducts(CNodeEvalIndex & NEI);
    // ConditionBlk Override
    DEFINE_CI(Washer, MN_Surge, 6);
  private:
  };

//===========================================================================

#undef DllImportExport

#endif
