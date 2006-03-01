//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __FU_MG_H
#define  __FU_MG_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif  
#ifndef __FLWNODE_H
  #include "flwnode.h"
#endif 
#ifndef  __MG_SEP_H
  #include "mg_sep.h"
#endif 
//#include "sp_cont.h"

#define DllImportExport 

// -------------------------------------------------------------------------- 

DEFINE_TAGOBJ(GlycolCont);

class GlycolCont : public FlashTank
  {
  public:
    double         TEG_T;
    double         TEG_P;
    double         TEG_TrayVol;
    double         Act_Lvl;
    double         TEG_Lvl;
    double         BedEfficiency;
    double         X2Est;
    double         FArEst;
    double         RemoveFrac;
    double         h2oout;
    double         Dew_Point_In;   // remove later for efficiency
    double         Dew_Point_Out;  // remove later for efficiency


    GlycolCont(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~GlycolCont(){};
   
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual void   EvalDerivs();
    virtual void   EvalProducts();

    virtual void   ApplyDerivs(double dTime, flag DoDbg);
    virtual void   ConvergeStates();
    virtual flag   EvalPressureSens();

    void   SetVLEMixFrac();//double H2OSettled)  

    double DewPt( pSpModel M, double P);
    double Absorb(rSpContainer Contents,double OpTemp, double OpPres, double BedEfficiency,
                  double GasOutEst, rSpConduit TEGI,
                  rdouble X2Est, rdouble FArEst, pchar Tag);
    double WaterContent (double OpTempL, double OpPresL, double TEGConc, rdouble dWdTEG);

  private:
  };

// -------------------------------------------------------------------------- 

#undef DllImportExport 

#endif


