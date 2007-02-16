#pragma once

//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

//**/ #define TTDEBUG

//---------------------------------------------------------------------------





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
    
    

  protected:
    MVLEBlk         m_VLE;    // Vapor-Liquid equilibrium
    long m_lSlipMode;
    double dIn;
    double dPipe;
    double dOut;
    double dThick;
    double dFlashP;
    double dFlashdT;
    double dxVapor;
    double dSlipDensity;

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

  };



