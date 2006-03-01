//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992
#ifndef  __MG_SYS14_H
#define  __MG_SYS14_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __MODELS_H
  #include "models.h"
#endif
//#ifndef __MDLNODE_H
//  #include <m_base.h>
//#endif
//#ifndef __FLWNODE_H
//  #include <flwnode.h>
//#endif

#define DllImportExport

const byte NoofWells = 18;

typedef struct
  {
  flag             bSSV1,bMSV2,bWSV3,/*bBPV8,*/bHCV5,bHCV6,bHCV7;
  double           PI3,DPI3,/*DPI6,*/CV4,PI5,BPV1,HCV7,NQv,NQvPt,ChkOpnPt,ChkOpnSt,
                   WellP,WellR,FQv,pNQv,InerP;
  }  AWell;
_RDEF(AWell);
_PDEF(AWell);

DEFINE_TAGOBJ(Sys14);
class Sys14 : public MdlNode
  {
  public:
    //SpContainer    Contents; // mhm added
    SpImage        Contents; // mhm added
    AWell          Wells[NoofWells];
    double         FlrNQv, FlrQvPt, ManR, NomManP, InertiaTau, InertiaScl;
    double         TI201, PI201, TI202, PI203;
    flag           bBDV14003, bBDV14004, bBDV14007, bBDV14008;
    flag           bESDV14005, bESDV14006;
    //double         HCV14001, HCV14002, HCV14003, HCV14004;
    double         ChkChar;
    double         DP6Tau;

    double         T_Rqd;    // mhm added


    Sys14(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~Sys14(){};
//            void   Initialise();

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    void           EvalWell(rAWell Wl);
    void           EvalWellPair(rAWell X, rAWell Y);

    //   virtual void   Dyn_EvalSrcResistance();
    virtual flag   InitialiseSolution();
    virtual flag   TerminateSolution();
    virtual void   EvalJoinPressures();
    virtual void   ConfigureJoins();
//    virtual flag   EvalFlowEquations(CSpPropInfo *pProps, int iJoinId, int FE); // no change
    virtual flag   EvalFlowEquations(CSpPropInfo *pProps, int IONo, int FE);  // no change
    virtual void   EvalProducts();
    virtual void   EvalDerivs();
    virtual void   EvalDiscrete();
  private:
  };

#undef DllImportExport

#endif


