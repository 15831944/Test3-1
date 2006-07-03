//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __SPLITTER_H
#define  __SPLITTER_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif  
#ifndef __M_XFER_H
  #include "m_xfer.h"
#endif  
#ifndef __2D_FN_H
  #include "2d_fn.h"
#endif  

#ifdef __SPLITTER_CPP
  #define DllImportExport DllExport
#elif !defined(Separ1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//=========================================================================== 

DEFINE_TAGOBJ(Splitter);

class Splitter : public MdlNode
  {
  public:
    //byte           iType;
    double         OSDens;   // Achieved Underflow Density
    double         USDens;   // Achieved Overflow Density(Clarity)
    byte           iSplitMethod;  // This determines the splitting method
    double         RqdSolidsToOS; // Required fraction of feed solids to oversize
    double         RqdLiqToOS;    // Required fraction of feed liquids to oversize
    double         RqdOSSolids;   // required solids fraction in oversize
    short          iSep1;    // First Specie to be separated
    short          iSep2;    // Second Specie to be separated
    short          iSep3;    // Third Specie to be separated
    double         OverS;    // Amount of specified species reporting to the splitter oversize
	  double         OSSolids, USSolids;
    double         ActOSSolidsConc25, ActUSSolidsConc25;
    flag           bTrackStatus;
    //flag           bShowQFeed;
    SpConduit      QFeed;

    Splitter(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~Splitter();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    virtual void   ConfigureJoins();
    virtual void   EvalJoinPressures(long JoinMask);

    virtual void   EvalProducts(CNodeEvalIndex & NEI);
    virtual dword  ModelStatus();

    DEFINE_CI(Splitter, MdlNode, 4);
  private:
  };

//=========================================================================== 

#undef DllImportExport

#endif
