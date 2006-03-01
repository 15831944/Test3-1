//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __MG_SYS13_H
#define  __MG_SYS13_H

#ifndef __SC_DEFS_H
  #include <sc_defs.h>
#endif
#ifndef __FLWNODE_H
  #include <flwnode.h>
#endif
#ifndef __MODELS_H
  #include "models.h"
#endif
//#ifndef __MDLNODE_H
//  #include <m_base.h>
//#endif

#define DllImportExport

DEFINE_TAGOBJ(Sys13);
class Sys13 : public MdlNode
  {
  public:
    // Inputs
    flag           bLC13111;
    flag           bLC13114;
    flag           bLC13117;
    flag           bLC13120;
    flag           bPKA1301A;
    flag           bPKA1301B;
    flag           bPK1302A;
    flag           bPK1302B;
    flag           bUHA1301A;
    flag           bUHA1301B;
//    RemoteData     ContactLvl;

    // Outputs
    SpContainer     VH1301;
    SpContainer     VH1302;
    SpContainer     VGA1301;
    SpContainer     VBA1301;
    SpContainer     HBA1301;
    SpContainer     TEGStore;

    SpConduit      Q1, Q2, Q3, Q4, Q5, Q6, Q7, Q8, Q9, Qa, Qb, Qc;

    double         VLVin_Posn;
    double         VH1301_Lvl;
    double         VH1301_P;
    double         VH1301_T;
    double         VH1302_Lvl;
    double         VH1302_P;
    double         VH1302_T;
    double         VGA1301_LvlSet;
    double         VGA1301_Lvl;
    double         VGA1301_P;
    double         VGA1301_T;
    double         HBA1301_LvlSet;
    double         HBA1301_Lvl;
    double         HBA1301_P;
    double         HBA1301_T;
    double         VBA1301_Lvl;
    double         VBA1301_T;
    double         FEA13050_Qm;
    double         VB1001_LvlSet;
    double         VB1001_Lvl;
    double         QmBoil;
    double         QmCirc;
    double         QmXfer;
    double         QmFill;
    double         QmSetl;
    double         QmRich;
    double         QmLean;
    double         Q1m, Q2m, Q3m, Q4m, Q5m, Q6m, Q7m, Q8m, Q9m, Qam, Qbm, Qcm;

    Sys13(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~Sys13(){};
//            void   Initialise();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);

    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual void   EvalJoinPressures();
    virtual void   ConfigureJoins();
    //virtual flag   EvalFlowEquations(CSpPropInfo *pProps, int iJoinId, int FE);
    virtual flag   EvalFlowEquations(CSpPropInfo *pProps, int IONo, int FE);
    virtual void   EvalProducts();
    virtual void   EvalDerivs();
    virtual void   EvalDiscrete();
    virtual void   ApplyDerivs(double dTime, flag DoDbg);

  private:
  };

// --------------------------------------------------------------------------
#undef DllImportExport

#endif


