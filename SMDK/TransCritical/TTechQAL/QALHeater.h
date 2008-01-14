//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
//   Time-stamp: <2006-11-01 11:54:00 Rod Stephenson Transcritical Pty Ltd>
// $Nokeywords: $
//===========================================================================
#pragma once



#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif
#include "..\TTCommon\STHX.h"

//---------------------------------------------------------------------------

class CQALHeater : public MBaseMethod, CSTHx
  {
  friend class CCondensateFinder;
  public:
    CQALHeater(MUnitDefBase *pUnitDef, TaggedObject * pNd);
    virtual ~CQALHeater(void);

    virtual void    Init();
    virtual void    BuildDataFields();
    virtual bool    ExchangeDataFields();
    virtual bool    ConfigureJoins();
    virtual bool    EvalJoinPressures();
    virtual void    EvalProducts();
    virtual void     ClosureInfo(MClosureInfo & CI);

    // Macro Model (Flash Train)...
    virtual void    MacroMdlEvaluate(eScdMacroMdlEvals Eval)  {};
    virtual bool    MacroMdlValidNd(int iIONo)                { return true; };
    virtual void    MacroMdlAddNd(int iIONo)                  {};
    virtual void    MacroMdlRemoveNd(int iIONo)               {};
    virtual CMacroMdlBase* MacroMdlActivate()                 { return m_FTC; };
    virtual void    MacroMdlDeactivate()                      {};

  protected:
    void DoCondensingHeater(MStream & ShellI, MStream & TubeI, MStream & ShellO, MStream & TubeO);
    // Heat Transfer Area is referred to INSIDE of tubes, based on distance between tubeplates
    // Pressure drop requires additional length of tubes across tubeplates.


    void flowRE(MStream &, MStream &);

    bool m_bSSScaling;
    bool m_bTSScaling;
    bool m_bOnline;       // If not online, only dP, no calculation of heat transfer

    long m_lOpMode;
    long m_lHxMode;
    long m_lTSHxMode;
    long m_lSSHxMode;
    long m_lTSdPMode;
    long m_lSSdPMode;

    double  m_dHTC;
    double m_dHTEffArea;    // Effective Area

    double m_dTSHTC;  // Tubeside heat transfer coefficient
    double m_dSSHTC;  // Shellside heat transfer coefficient

    double m_dSSKdFactor;  // Kd factor for shellside
    double m_dTSKdFactor;  // Kd factor for tubeside


    double m_dG;     //Mass flow kg/s/m^2
    double m_dTSVel;  // Tubeside velocity
    double m_dReIn, m_dReOut;    //Reynolds no
    double m_dftIn, m_dftOut;    //Friction Factor
    double m_dNuGIn, m_dNuGout;   //Nussalt Number
    double m_dPdLIn, m_dPdLOut;   //Pressure Gradient
    double m_dP;     //Pressure Drop


    double  m_dUA;
    double  m_dLMTD;
    double  m_dDuty;
    double  m_dLMTDFactor;
    double  m_dSScaleConductivity;   // Shell Scale Conductivity 
    double m_dQmVent, m_dSecQm;
    

    MVLEBlk         m_VLE;
    MFT_Condenser   m_FTC;

#ifdef TTDEBUG
    double m_tmp1,m_tmp2, m_tmp3, m_tmp4, m_tmp5 ;
#endif

  };
