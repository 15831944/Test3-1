//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
//   Time-stamp: <2006-11-08 13:14:18 Rod Stephenson Transcritical Pty Ltd>
// $Nokeywords: $
//===========================================================================
#pragma once


#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

//---------------------------------------------------------------------------


double linMap(double x, double x1, double y1, double x2, double y2);


class CTTCompress : public MBaseMethod
{
  public:
    CTTCompress(MUnitDefBase *pUnitDef, TaggedObject * pNd);
    virtual ~CTTCompress(void);

    virtual void    Init();
    virtual void    BuildDataFields();
    //    virtual bool    ExchangeDataFields();
    virtual bool    ConfigureJoins();
    virtual bool    EvalJoinPressures();
    virtual void    EvalProducts();
    virtual void    ClosureInfo(MClosureInfo & CI);

  protected:
    bool m_bRawData;
    double m_dQv;      // Volume Flow
    long m_lPumpMode;   
    long m_lCalcMode;
    double m_dRPM;     // Actual pump speed
    long iPD;          // Number of data sets
    double m_dP;       // Final Pressure
    double m_dDeltaP;   // Pressure Boost
    double m_dPRatio;   // Pressure Ratio
    double m_dQQ;
    double m_dEff;
    double m_dPwr;     // Pump Power
    double m_dMWT;
    double m_dGasCp;
    double m_dGasCv;
    double m_dGasGamma;
    double m_dTIn;
    double m_dTOut;
    double m_dPolyEff;
    double m_dPolyK;

    void fitCurveData();
    double findDP();
    double calcDP(int, double);
    double m_tmp1, m_tmp2, m_tmp3, m_tmp4, m_tmp5;
    double Efficiency(double);
    void fitEffData(void);
    void doSimpleCompressor(MStream & , MStream & );
};
