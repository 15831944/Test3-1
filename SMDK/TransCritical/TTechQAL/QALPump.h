//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
//   Time-stamp: <2006-11-01 11:55:07 Rod Stephenson Transcritical Pty Ltd>
// $Nokeywords: $
//===========================================================================
#pragma once


#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

//---------------------------------------------------------------------------


double linMap(double x, double x1, double y1, double x2, double y2);


class CQALPump : public MBaseMethod
{
  public:
    CQALPump(MUnitDefBase *pUnitDef, TaggedObject * pNd);
    virtual ~CQALPump(void);

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
    double m_dRPM;     // Actual pump speed
    long iPD;          // Number of data sets
    double m_dPumpData[4][4];
    double m_dRawPumpData[4][4][2]; // Up to four data sets of four pairs (Q, dP)
    double m_dEffData[5][2]; // Five pairs of efficiency data
    double m_dEff[3];  // coefficients for eff curve
    double m_dRPMs[4];
    double m_dPmax[4];
    double m_dP;       // Pressure boost
    double m_dQQ;
    double m_dEf;
    double m_dPwr;     // Pump Power
    void fitCurveData();
    double findDP();
    double calcDP(int, double);
    double m_tmp1, m_tmp2, m_tmp3, m_tmp4, m_tmp5;
    double Efficiency(double);
    void fitEffData(void);


};
