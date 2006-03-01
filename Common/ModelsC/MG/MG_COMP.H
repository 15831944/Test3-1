//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __COMP_H
#define  __COMP_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __MODELS_H
  #include "models.h"
#endif
//#ifndef __M_XFER_H
//  #include "m_xfer.h"
//#endif
//#ifndef __M_BASE_H
//  #include <m_base.h>
//#endif

#ifndef __2D_FN_H
  #include "2d_fn.h"
#endif
#ifndef __3D_FN_H
  #include "3d_fn.h"
#endif

#ifdef __COMP_CPP
  #define DllImportExport DllExport
#else
  #define DllImportExport
#endif

//===========================================================================

DEFINE_TAGOBJ(X_Comp);
/*#C:
Basic Mixed Tank - Multiple Inputs and Outputs
*/

const byte CTPress = 1;
const byte CTSpeed = 1;

class X_Comp : public MN_Xfer
  {
  public:
    double         P_Compressability;    // Obvious
    double         P_MolecularWeight;    // Obvious
    double         P_SpecificHeatRatio;  // Specific Heat Ratio from flashing ??
    double         P_SetpointPressure;   // Setpoint Pressure set by the user
    double         P_PolytropicEff;      // Polytropic efficiency.Set by user as a physical constant for the compressor
    double         P_MaxPower;           // Maximum power supplied to compressor
    unsigned char  P_Status;             // on or off

    // Not parameters
    // Added CNM
    byte           Control;
    double         MinSpeed;
    double         MaxSpeed;
    double         SpdSpt;
    double         SpdTau;
    double         SpdFbk;
    double         ActSpeed;
    double         TheBoost;

    double         SpeedRatio;         // Speed % for a given QV point
    double         EfficiencyRatio;    // Eff % for a given QV point
    double         Pin;                // Pressure on inlet flange
    double         Pout;               // Pressure on outlet flange
    double         QinFlange;          // MassFlow on inlet flange
    double         QoutFlange;         // MassFlow on outlet flange
    double         QblowFlange;        // MassFlow on blowoff flange
    double         Tin;                // Temperature on inlet flange
    double         Tout;               // Temperature on outlet flange
    double         CurrentQOut;        // Current Flow rate out - damped
    double         CurrentPout;        // Current pressure out - damped
    double         CurrentPin;         // Current Pressure input - damped
    double         CurrentPow;         // Current power used by compression
    double         QvSrg;              // Flow at which surging begins
    double         QvIn;               // Actual Vol FLow
    double         MFlow;              // Flow in MMSCFD
    double         X_SetpointPressure;

    double         Boost;
    double         Resist;
    double         P2prv;
    double         P1prv;
    double         Qprv;
    double         Bprv;
    double         Rprv;
    double         ResistPrv;
    double         Rdelta;
    int            Iter;

    C2DFn          SurgeCondition;     // Inlet Pressure Versus volume flow function
    C3DFn          Speed;              // Given Pin,Qin what is the motor speed
    C3DFn          Efficiency;         // Given Pin,Qin what is the Efficiency


    X_Comp(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~X_Comp (){};

    double         Afac();
    double         Bfac();
    double         Pwr_Curve(  double Q,double P1,double P2 );
    double         Flw_Curve(double Pwr,double P1,double P2 );
    double         P2_Curve (  double Q,double P1,double Pwr);


    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual void   PostConnect(int IONo);
    virtual void   PreDisConnect(int IONo);
    //virtual flag   EvalFlowEquations(CSpPropInfo *pProps, int iJoinId, int FE);
    virtual flag   EvalFlowEquations(CSpPropInfo *pProps, int IONo, int FE);
    virtual void   EvalDerivs();
    virtual void   EvalDiscrete();

    //virtual void   StartSolution(rFlwEqnBlk FE);
    virtual void   EvalProductsInit(EvalProductsInitTasks Task);
    virtual void   EvalProducts();

  private:



  };

//===========================================================================

#undef DllImportExport

#endif
