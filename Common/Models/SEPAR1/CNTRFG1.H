//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992 - 1996

#ifndef  __CNTRFG1_H
#define  __CNTRFG1_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __M_SURGE_H
  #include "m_surge.h"
#endif
#ifndef __GSMODEL_H
  #include "gsmodel.h"
#endif

#ifdef __CNTRFG1_CPP
  #define DllImportExport DllExport
#else
  #define DllImportExport
#endif

// ==========================================================================

//XID xidCntrfugeSpill = ModelXID(1000);
XID xidMoistAnal     = ModelXID(1001);

const byte CSF_SolRec     = 0x01;
const byte CSF_CentConc   = 0x02;
const byte CSF_SolidAnal  = 0x04;
const byte CSF_MoistAnal  = 0x08;
//const byte CSF_SolidConc  = 0x10;

DEFINE_TAGOBJ(Centrifuge1);
class DllImportExport Centrifuge1 : public MN_Surge
  {
  public:
    byte           iMeth;
    //byte           iSolCalc;
    //byte           iUFCalc;
    double         Reqd_SolConc;   // Solids flow solids at 25 dC
    double         Reqd_SolMFrac;  // Solids flow mass fraction
    double         Reqd_SolRec;    // Fraction of solids reporting to Solids;
    double         Reqd_CenConc;   // Centrate solids at 25 dC
    //double         Reqd_UQVol;     // Underflow Volumetric Flowrate

    double         SolFlwFrac;     // Ratio of actual solids flow to solids flow capacity
    double         CenFlwFrac;     // Ratio of actual solids flow to centrate flow capacity
    double         ActCakeSolids;
    double         ActCentrateSolids;
    double         ActCakeSolConc;
    double         ActCentrateSolConc;
    flag           bTrackStatus;

    Centrifuge1(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~Centrifuge1();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual void   EvalJoinPressures(long JoinMask);
    virtual void   EvalProducts(CNodeEvalIndex & NEI);

    DEFINE_CI(Centrifuge1, MN_Surge, 4);

   private:
  };

//=========================================================================== 

#undef DllImportExport

#endif
