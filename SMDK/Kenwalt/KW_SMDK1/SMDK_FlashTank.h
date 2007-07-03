#pragma once

//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

//---------------------------------------------------------------------------

class CFlashTank : public MBaseMethod
  {
  friend class CSimpleSolverFn;
  public:
    CFlashTank(MUnitDefBase * pUnitDef, TaggedObject * pNd);
    virtual ~CFlashTank(void);

    virtual void    Init();
    virtual void    BuildDataFields();
    virtual bool    ExchangeDataFields();

    virtual void    EvalMakeUpReqd();
    virtual void    EvalMakeUpAvail();
    virtual bool    EvalJoinPressures();
    virtual void    EvalProducts();
    virtual void    ClosureInfo(MClosureInfo & CI);

    // Flash Train - MacroMdls
    virtual void    MacroMdlEvaluate(eScdMacroMdlEvals Eval);
    virtual bool    MacroMdlValidNd(int iIONo)                { return m_OnLine; };
    virtual void    MacroMdlAddNd(int iIONo)                  {};
    virtual void    MacroMdlRemoveNd(int iIONo)               {};
    virtual CMacroMdlBase* MacroMdlActivate()                 { return m_FTF; };
    virtual void    MacroMdlDeactivate()                      {};

  protected:

    //void DoSimpleHeater(MStream & ShellI, MStream & TubeI, MStream & ShellO, MStream & TubeO);

    //double          m_FlashPRqd;
    //double          dRqdDuty;
    //double          dActualDuty;
    //double          dFeedT;
    //double          dProdT;

    //long            m_lOpMode;

    //double          m_dHTC;
    //double          m_dArea;
    //double          m_dUA;
    //double          m_dLMTD;
    //double          m_dDuty;
    //double          m_dLMTDFactor;

    //MReactionBlk    m_RB;
    bool            m_OnLine;
    MVLEBlk         m_VLE;
    MFT_Flash       m_FTF;
    MProbalPCtrl    m_PCtrl;
    MStreamI        m_Feed; // keep the feed in here - also save it.
    MStreamI        m_WrkStrm; // temporary work 
  };
