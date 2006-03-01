#pragma once

//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

//---------------------------------------------------------------------------

class CKoueValle : public MBaseMethod
  {
  friend class CKV_Solver;
  //friend class CST_CondensateFinder;

  public:
    CKoueValle(MUnitDefBase * pUnitDef, TaggedObject * pNd);
    virtual ~CKoueValle(void);

    virtual void    Init();
    virtual void    BuildDataFields();
    virtual bool    ExchangeDataFields();
    virtual bool    EvalJoinPressures();
    virtual void    EvalProducts();
    virtual void    ClosureInfo(MClosureInfo & CI);

    // Macro Model (Flash Train)...
    //virtual void    MacroMdlEvaluate(eScdMacroMdlEvals Eval)  {};
    //virtual bool    MacroMdlValidNd(int iIONo)                { return true; };
    //virtual void    MacroMdlAddNd(int iIONo)                  {};
    //virtual void    MacroMdlRemoveNd(int iIONo)               {};
    //virtual CMacroMdlBase* MacroMdlActivate()                 { return m_FTC; };
    //virtual void    MacroMdlDeactivate()                      {};

  protected:
    void DoCondensingHeater(MStream & GasI, MStream & CoolI, MStream & GasO, MStream & LiqO, MStream & CoolO);

    double          m_HTC;
    double          m_Area;
    long            m_SegCount;
	double			m_tubeno;
    //double          m_LMTD;
    double          m_Duty;
    double          m_Damping;
    double          m_Tolerance;
    double          m_ToleranceN2T;
    double          m_TN2OutEst;


    //MVLEBlk         m_VLE; //VLE helper class
    //MFT_Condenser   m_FTC; //flash train steam demand (sink)
  };
