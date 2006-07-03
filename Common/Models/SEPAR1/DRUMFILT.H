//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __DRUMFILT_H
#define  __DRUMFILT_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif  
#ifndef __M_SURGE_H
  #include "m_surge.h"
#endif  
#ifndef __2D_FN_H
  #include "2d_fn.h"
#endif  

#ifdef __DRUMFILT_CPP
  #define DllImportExport DllExport
#elif !defined(Separ1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//=========================================================================== 

DEFINE_TAGOBJ(DrumFilt);

class DrumFilt : public MN_Surge
  {
  public:
    byte           iRqdFiltMethod;
    double         RqdCakeMoist, ActCakeSolids, ActCakeSolConc;
    double         RqdFiltSolids, FiltSolidsUsed, ActFiltSolids;
    double         RqdBypass;
    double         RqdFiltSolConc, ActFiltSolConc;//, ActFiltSolConcT;
    //byte           iScanEffSpecie; //eg caustic
    //double         ActScandrettEff;
    byte           iWashEffSpecie; //eg caustic
    double         dSpWashEff;
    flag           bTrackStatus;
    flag           fOn;

    SpConduit      Qw;

    DrumFilt(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~DrumFilt();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);

    virtual void   EvalJoinPressures(long JoinMask);
    virtual void   EvalProducts(CNodeEvalIndex & NEI);

    // ConditionBlk Override
    DEFINE_CI(DrumFilt, MN_Surge, 4);

  private:
  };

//=========================================================================== 

#undef DllImportExport

#endif
