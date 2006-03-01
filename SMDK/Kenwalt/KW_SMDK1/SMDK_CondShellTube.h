#pragma once

//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

//---------------------------------------------------------------------------

class CCondShellAndTube : public MBaseMethod
  {
  friend class CST_CondensingSolverFn;
  friend class CST_CondensateFinder;
  public:
    CCondShellAndTube(MUnitDefBase * pUnitDef, TaggedObject * pNd);
    virtual ~CCondShellAndTube(void);

    virtual void    Init();
    virtual void    BuildDataFields();
    virtual bool    ExchangeDataFields();
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
    void DoCondensingHeater(MStream & ShellI, MStream & TubeI, MStream & ShellO, MStream & TubeO);

    double          m_dHTC;
    double          m_dArea;
    double          m_dLMTD;
    double          m_dDuty;

    MVLEBlk         m_VLE; //VLE helper class
    MFT_Condenser   m_FTC; //flash train steam demand (sink)
  };
