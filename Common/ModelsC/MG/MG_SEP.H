//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __MG_SEP_H
#define  __MG_SEP_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#define BASIC1
#ifndef __MODELS_H
  #include "models.h"
#endif
//#ifndef __M_SRCSNK_H
//  #include "m_srcsnk.h"
//#endif
//#ifndef __SURGE_H
//  #include "surge.h"
//#endif
//#ifndef __M_TANK_GP_H
//  #include "tank_gp.h"
//#endif
#ifndef __FLWNODE_H
  #include <flwnode.h>
#endif

#ifndef __SM_OIL_H
  #include "sm_oil.h"
#endif

#ifndef __SP_CONT_H
  #include "sp_cont.h"
#endif

#ifdef __MG_SEP_CPP
  #define DllImportExport DllExport
#else
  #define DllImportExport
#endif

// --------------------------------------------------------------------------
DEFINE_TAGOBJ(Flare);
_FWDDEF(FlashTank);
_FWDDEF(Flare);
class Flare : public MN_SrcSnk
  {
  public:
    double         PSetP, PGapP;
    //flag           Flash;
    double         TempO;
    Strng          FlareTag;
    pFlashTank     FlareDrum;

    Flare (pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~Flare (){};
//    void           Initialise();
    virtual void   BuildDataDefn(DataDefnBlk &DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
//    virtual flag   EvalFlowEquations(CSpPropInfo *pProps, int iJoinId, int FE);
    virtual flag   EvalFlowEquations(CSpPropInfo *pProps, int IONo, int FE);
    virtual void   EvalProducts();
    virtual void   EvalDerivs();
    virtual void   EvalDiscrete();
    virtual void   EvalJoinPressures();
  };

// --------------------------------------------------------------------------

DEFINE_TAGOBJ(FlashTank);
class FlashTank : public Tank
  {
  public:
    //flag           Flash, FlashIn;
    //double         P_Rqd;

//    double         EvapTau, VFrac_H, VFrac_L;

    FlashTank(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~FlashTank(){};
//            void   Initialise();
     double        mDewPt(pMdlNode This);
     double        mH2oVVFrac(pMdlNode This);
    virtual void   BuildDataDefn(DataDefnBlk &DDB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
//    void           EvalDerivs_Flash_____(rSpContainer M);
    //virtual void   EvalDerivs();
    virtual void   ConvergeStates();
  private:
  };

// --------------------------------------------------------------------------



/* cnm**
DEFINE_TAGOBJ(FlareDrum);

_FWDDEF(FlareDrum);
class FlareDrum : public FlashTank
  {
  public:
    double         FlareRate;

    FlareDrum(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~FlareDrum(){};
//            void   Initialise();
    virtual void   BuildDataDefn(DataDefnBlk &DDB);
            flag   DataXchg(DataChangeBlk & DCB);
    virtual void   EvalProducts();
    virtual void   EvalDerivs();
    virtual void   Dyn_EvalState();
  private:
  };
**/
// --------------------------------------------------------------------------

DEFINE_TAGOBJ(VL_Sep);
_FWDDEF(VL_Sep);
class VL_Sep : public FlashTank
  {
  public:
    VL_Sep (pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~VL_Sep (){};
    //virtual void   EvalProducts();
    //virtual void   EvalDerivs();
  private:
  };

// --------------------------------------------------------------------------

DEFINE_TAGOBJ(VLL_Sep);
_FWDDEF(VLL_Sep);
class VLL_Sep : public FlashTank
  {
  public:
    double         BootVolume;
    double         H2ORemEff;  // hss 20/01/97 Defines efficiency of water removal from the boot

    double         H2oSettled;
    double         dH2oSettled;
    dword          H2OValidFlags;
    Integrator   * pH2oSettled;
    Integrator   * pMEGSettled;

    double         ExtraH2oSettled;

    double         MEGSettled;  //hss 12/01/2000 Add facility to remove MEG via the boot.
    double         dMEGSettled;
    double         TotalSettled;

    VLL_Sep (pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~VLL_Sep ();

    //            void   Initialise();
    virtual void   BuildDataDefn(DataDefnBlk &DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual flag   InitialiseSolution();
    virtual void   EvalProducts();
    virtual void   EvalDerivs();
    virtual void   ApplyDerivs(double dTime, flag DoDbg);
    virtual void   ConvergeStates();
    virtual flag   EvalPressureSens();

  protected:
    void SetVLEMixFrac(double H2OSettled);
  };

// ---------------------------------------------------------------------------

const word xidBH2oIf         =  50000;
const word xidBootLev        =  50001;
const word xidBSettle        =  50002;
const word xidBSetTau        =  50003;
const word xidFlrRate        =  50004;
// ---------------------------------------------------------------------------

#undef DllImportExport

#endif

