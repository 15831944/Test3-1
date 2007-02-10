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
  friend class CCondensateFinder;
  friend class CSimpleSolverFn;
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

    double dEntryK; 
    double dPin;
    double dPout;
    double dPOrificein;
    double dMassFlow;
    


  };



