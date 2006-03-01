//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __FU_MG_H
#define  __FU_MG_H

#ifndef __SC_DEFS_H
  #include <sc_defs.h>
#endif
#ifndef __FLWNODE_H
  #include <flwnode.h>
#endif
#ifndef __MODELS_H
  #include "models.h"
#endif
//#ifndef __VALVE_GP_H
//  #include "valve_gp.h"
//#endif

#define DllImportExport

const int NBHPVs = 4;

DEFINE_TAGOBJ(BHPValve);

class BHPValve : public MN_SrcSnk
  {
  public:
    double         PSetP, PGapP;
    flag           bAuto[NBHPVs];
    double         YRqd[NBHPVs], Res[NBHPVs], QPt[NBHPVs], PPt[NBHPVs];
    flag           Flash;
    double         TempO;
    //mhmpVLInterface   VL_Ifi;
    //mhmpVLInterface   VL_Ifo;

    Strng          FlareTag;
    pMdlNode       FlareDrum;

    BHPValve(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~BHPValve (){};
//            void   Initialise();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual void   EvalJoinPressures();
    //virtual flag   EvalFlowEquations(CSpPropInfo *pProps, int iJoinId, int FE);
    virtual flag   EvalFlowEquations(CSpPropInfo *pProps, int IONo, int FE);
    virtual void   EvalDerivs();
    virtual void   EvalCtrlActions();
  };

#undef DllImportExport

#endif

