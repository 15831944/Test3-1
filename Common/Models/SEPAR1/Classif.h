//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty)

#ifndef  __CLASSIF_H
#define  __CLASSIF_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif  
#ifndef __M_SURGE_H
  #include "m_surge.h"
#endif  
#ifndef __2D_FN_H
  #include "2d_fn.h"
#endif  

#ifdef __CLASSIF_CPP
  #define DllImportExport DllExport
#elif !defined(Separ1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ==========================================================================
 
DEFINE_TAGOBJ(Classifier);

class DllImportExport Classifier : public MN_Surge
  {
  public:
    byte           iUFCalc;
    double         Reqd_UFConc;   // Underflow solids at 25 dC in g/L
    double         Reqd_UFSolRec; // Fraction of solids reporting to U/F;
    double         Reqd_OFConc;   // Overflow solids at 25 dC in g/L
    double         Reqd_UQVol;    // Underflow Volumetric Flowrate
    double         UFSolids;
    double         OFSolids;
    double         ActUFSolidsConc25, ActOFSolidsConc25;
    double         ActUFSolRec; // Underflow solids recovery
    flag           bTrackStatus;

    Classifier(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~Classifier();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual void   EvalJoinPressures(long JoinMask);
    virtual void   EvalProducts(CNodeEvalIndex & NEI);

    // ConditionBlk Override
    DEFINE_CI(Classifier, MN_Surge, 8);

   private:
  };

//=========================================================================== 

#undef DllImportExport

#endif
