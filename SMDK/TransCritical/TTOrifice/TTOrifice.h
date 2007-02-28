#pragma once

//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

//**/ #define TTDEBUG

//---------------------------------------------------------------------------

enum SlipModels {SFM_Homo,  SFM_Lock,  SFM_Faus,  SFM_Thom,  SFM_Zivi,
SFM_Baro,  SFM_Mood,  SFM_Wall,  SFM_HNEM, SFM_Default};

enum OpMode {OM_Simple, OM_Full};



double dcf(double);

class CTTOrifice : public MBaseMethod
  {

  public:
    CTTOrifice(MUnitDefBase * pUnitDef, TaggedObject * pNd);
    virtual ~CTTOrifice(void);

    virtual void    Init();
    virtual void    BuildDataFields();
    //    virtual bool    ExchangeDataFields();
    virtual bool    ConfigureJoins();
    virtual bool    EvalJoinPressures();
    virtual void    EvalProducts();
    virtual void    ClosureInfo(MClosureInfo & CI);

    double massVelocity(MStream  ms, double pIn, double pOut);
    double chokeMassVelocity(MStream  ms, double pIn);
    double ValveCv();
    double orificeDeltaP(MStream ms);
    double criticalVelocity(MStream ms, int slipMode = SFM_Default);
    double criticalFlow(MStream ms, int slipMode = SFM_Default);    

  protected:
    MVLEBlk         m_VLE;    // Vapor-Liquid equilibrium
    bool bOn;     // Bypass if not...
    bool bPassThru;    // If on, pass stream through unaltered
    bool bControlValve;  // Include control valve dP
    bool bValveLinear;   // Interpolate linearly between data points.

    long m_lSlipMode;
    long m_lOpMode;

    double dIn;
    double dPipe;
    double dOut;
    double dThick;
    double dFlashP;
    double dFlashdT;
    double dxVapor;
    double dSlipDensity;
    double dInletDensity;
    double dDensityCorrection;  // If new variable is here, crashes

    double dEntryK;    // K value for pipe run up to orifice
    double dPin;
    double dPout;
    double dPOrificeIn;
    double dMassFlow;
    double dMassFlow1;
    double dMassFlow2;
    double dMassFlow3;
    double dMassFlow4;
    double dSatP;
    double dTin;
    double dLevel;     // Inlet hydrostatic level
    double dOrificeHead;  // Additional level drop to orifice inlet
    double dMassVelocity;
    double dPCritical;  // Critical Pressure in orifice choke flow
    double dPOutActual;
    double dPValve;         // (Variable) Pressure drop in valve
    double dValvePosition;  // Position of valve, 0-100%
    double dSinglePhaseDP;
    double dValvePressureDrop;
    double dValveCvOpen;
    double dValveCvClosed;
    double dValveK;
    double dPipeVelocity;
    double dChokeVelocity;       
    double dHomogChokeVelocity;
    double dChokeMassVelocity;
    double dHomogMassChokeVelocity;
    long lValveDataPts;      // 10 for angle, 11 for %, 2 for simple case.
    double dValveData[11];  // Data for control valve, Cv at data points
    double dValveCv;  // Valve Cv at position

  };



