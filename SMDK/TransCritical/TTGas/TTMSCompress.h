//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
//   Time-stamp: <2006-11-15 11:57:12 Rod Stephenson Transcritical Pty Ltd>
// $Nokeywords: $
//===========================================================================
#pragma once


#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

//---------------------------------------------------------------------------

struct throwData 
{
  bool isTandem;     // Tandem Piston
  bool heDisabled;   // Disable 
  bool ceDisabled;
  long heStage;     // Stage 
  long ceStage;
  double heCV;      // Clearance Volumes %
  double ceCV;
  double heDiam;   // Piston Diameters  m
  double ceDiam;
  double heSV;  // Swept Volumes   m^3
  double ceSV; 
  double hePD;     // Piston Displacements, m^3/s
  double cePD;
};

struct stageData 
{
  double pSuction; // Pressure delivered at inlet to cylinder
  double p1;       // Pressure in cylinder during induction
  double p2;       // Pressure in cylinder during discharge
  double pDischarge; // Pressure at outlet of cylinder
  double pOut;      // Pressure at outlet of cooler
  double TSuction;  // Temperature at inlet to cylinder
  double TDischarge; // Temperature at outlet of cylinder
  double TOut;  // Temperature out of cooler
  double pRatio;  // Pressure Ratio
  double zS;      // Compressibilities at suction and discharge
  double zD;
  double qCooler; // Heat rejection intercooler
  double vValve;  // Valve Velocity
};




double linMap(double x, double x1, double y1, double x2, double y2);


class CTTMSCompress : public MBaseMethod
{
  public:
    CTTMSCompress(MUnitDefBase *pUnitDef, TaggedObject * pNd);
    virtual ~CTTMSCompress(void);

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
    double m_dTIntercool;
    double m_dPolyEff;
    double m_dPolyK;


    double m_dStroke;   // Piston Stroke
    double m_dRodDiam;  // Piston Rod Diameter
    
    

    long m_lStages;
    long m_lThrows;
    double cylDiam[6];   // Cylinder Diameter [0]... head end [1]... crank end
    double cylCV[6];     // Cylinder Clearance Volume, % 
                            // cylCV[0][i]...  head end CV
                            // cylCV[1][i]... crank end CV
    

    throwData td[4];   // Mechanical data for frame
    stageData sd[6];   // Process Data for multistage process
    
    

    void fitCurveData();
    double findDP();
    double calcDP(int, double);
    double m_tmp1, m_tmp2, m_tmp3, m_tmp4, m_tmp5;
    double Efficiency(double);
    void fitEffData(void);
    void doSimpleCompressor(MStream & , MStream & );
};
