#pragma once

//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __MD_HEADERS_H
#include "ex_headers.h"
#endif
//#include "..\TTCommon\STHx.h"
#include "STHx.h"

//**/ #define TTDEBUG

//---------------------------------------------------------------------------

double  ft(double);              // Basic HT and dP correlations for tubeside
double NuG(double, double);

class CShellTube2 : public MBaseMethod, CSTHx
  {
  friend class CCondensateFinder;
  friend class CSimpleSolverFn;
  public:
    CShellTube2(MUnitDefBase * pUnitDef, TaggedObject * pNd);
    virtual ~CShellTube2(void);

    virtual void    Init();
    virtual void    BuildDataFields();
    //    virtual bool    ExchangeDataFields();
    virtual bool    ConfigureJoins();
    virtual bool    EvalJoinPressures();
    virtual void    EvalProducts();
    virtual void    ClosureInfo(MClosureInfo & CI);

    // Macro Model (Flash Train)...
    virtual void    MacroMdlEvaluate(eScdMacroMdlEvals Eval)  {};
    virtual bool    MacroMdlValidNd(int iIONo)                { return true; };
    virtual void    MacroMdlAddNd(int iIONo)                  {};
    virtual void    MacroMdlRemoveNd(int iIONo)               {};
    virtual CMacroMdlBase* MacroMdlActivate()                 { return m_FTC; };
    virtual void    MacroMdlDeactivate()                      {};

  protected:
    void DoSimpleHeater(MStream & ShellI, MStream & TubeI, 
			    MStream & ShellO, MStream & TubeO);


    void DoCondensingHeater(MStream & ShellI, MStream & TubeI, 
			    MStream & ShellO, MStream & TubeO);

    void DoLiveSteamHeater(MStream & ShellI, MStream & TubeI, 
			   MStream & ShellO, MStream & TubeO, MStream & VentO);
    

    // Heat Transfer Area is referred to INSIDE of tubes, 
    // based on distance between tubeplates
    // Pressure drop requires additional length of tubes across tubeplates.
    void flowRE(MStream &, MStream &);
    double LMTD1(double TbTi, double TbTo, double ShTi, double ShTo);
    
    bool bOnline;     // If online
    bool m_bMTDCalcMode;     // if true use RMTD formulation
    bool m_bEnvironHX; // Environmental HX

    long m_lOpMode;    // Operating Mode (not implemented, always uses condensing)
    long m_lHxMode;    // Heat Transfer calc mode - 0 lumped, 1 detailed
    long m_lTSHxMode;  // Modes for Tubeside and Shellside HT and dP calculations
    long m_lSSHxMode;  // Presently no detailed calculations implemented
    long m_lTSdPMode;  
    long m_lSSdPMode;

    long m_lEnvHxMode;  // Environmental Hx Mode
    double m_dEnvHeatLoss; // Heat loss to environment
    double m_dLossPerQm;   // Env loss per kg
    double m_dEnvHTC;

    double m_dCondFlow;    // Condensate flow
    double m_dSatT;        // Saturation Temperature and Pressure
    double m_dSatP;
    double m_dSST;         // Shellside condensing temp


    double m_dSSCp;        // Average specific heats of fluids
    double m_dTSCp;

    double m_dSSHTC;       // Shellside and Tubeside film coefficients
    double m_dTSHTC;

    double m_dTSTin;       // Temperatures on tube and shell side streams
    double m_dTSTout;
    double m_dSSTin;
    double m_dSSTout;
    double m_dTSPin;       // Pressures on tube and shell side streams
    double m_dTSPout;
    double m_dSSPin;
    double m_dSSPout;
    double m_dEntranceC, m_dExitC;   // Correction factors for entrance and exit
    
    double m_dQmTS;
    double m_dQmSS;
    


    double m_dG;     //Mass flow kg/s/m^2
    double m_dReIn, m_dReOut;    //Reynolds no
    double m_dftIn, m_dftOut;    //Friction Factor
    double m_dNuGIn, m_dNuGout;   //Nussalt Number
    double m_dPdLIn, m_dPdLOut;   //Pressure Gradient
    double m_dPEntrance;   // Entrance and exit pressure drops
    double m_dP;     //Total Pressure Drop
    double  m_dHTC;    // Overall Heat Transfer Coeff

    double m_dUA;
    double m_dLMTD;
    double m_dRMTD;           // Reactive MTD

    double m_dDuty;           // Heat Exchanged
    double m_dTotDuty;        // Hx plus environment loss

    double m_dLMTDFactor;

    double m_dRHt;            // Reaction Heat in kJ/kg

    double m_dQmVentRqd;      // Vent flow
    double dHf;               // Total Reaction Heat
    

    double m_dActualDuty;
    double m_dReactionDamping;
    double m_dDeltaTOld;

    MReactionBlk    m_RB;     // Reactions
    MVLEBlk         m_VLE;    // Vapor-Liquid equilibrium
    MFT_Condenser   m_FTC;    // Flash Train

#ifdef TTDEBUG
#include "ttdebug.h"
    double m_tmp1,m_tmp2, m_tmp3, m_tmp4, m_tmp5, m_tmp6, m_tmp7, m_tmp8 ;
#endif


  };



