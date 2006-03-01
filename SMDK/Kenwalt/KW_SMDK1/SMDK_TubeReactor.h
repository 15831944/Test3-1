#pragma once

//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

//---------------------------------------------------------------------------

class CTubeReactor : public MBaseMethod
  {
  friend class CSimpleSolverFn;
  friend class CCondensingSolverFn;
  friend class CCondensateFinder;
  public:
    CTubeReactor(MUnitDefBase * pUnitDef, TaggedObject * pNd);
    virtual ~CTubeReactor(void);

    virtual void    Init();
    virtual void    BuildDataFields();
    virtual bool    ExchangeDataFields();
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
    void DoSimpleHeater(MStream & ShellI, MStream & TubeI, MStream & ShellO, MStream & TubeO);
    void DoCondensingHeater(MStream & ShellI, MStream & TubeI, MStream & ShellO, MStream & TubeO);

    long            m_lOpMode;

    double          m_dHTC;
    double          m_dArea;
    double          m_dUA;
    double          m_dLMTD;
    double          m_dDuty;
    double          m_dLMTDFactor;

    MReactionBlk    m_RB;
    MVLEBlk         m_VLE;
    MFT_Condenser   m_FTC;
  };
