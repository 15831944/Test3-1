//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
//    QAL Precip Model. Transcritical Technologies Pty Ltd copyright 2004
//   Time-stamp: <2007-05-22 23:41:30 Rod Stephenson Transcritical Pty Ltd>
// Copyright (C) 2005 by Transcritical Technologies Pty Ltd and KWA
//===========================================================================

#ifndef  __QALPSD_H
#define  __QALPSD_H

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif



// Helper Class for basic PSD information
const static int nClasses = 21;
const double DIST_CORR = .72137298;
const double AREA_CORR = .9;



const double D[] = {
  2.0 ,
  2.51984209979 ,
  3.17480210394 ,
  4.0 ,
  5.03968419958 ,
  6.34960420787 ,
  8.0 ,
  10.0793683992 ,
  12.6992084157 ,
  16.0 ,
  20.1587367983 ,
  25.3984168315 ,
  32.0 ,
  40.3174735966 ,
  50.796833663 ,
  64.0 ,
  80.6349471933 ,
  101.593667326 ,
  128.0 ,
  161.269894387 ,
  203.187334652 ,
};




class QALPSD 
{
  double n[nClasses];
  double m_dVol, m_dArea, m_dNumber;
  double m_dVolumeCorrection;
 public:
  QALPSD() {}
  QALPSD(MStream &);
  QALPSD(MStream &, double);
  
  double getArea() {return m_dArea;}
  
  double getVol() {return m_dVol;}         // Get total volume of particles in PSD
  double getVol(int);    // Get volume in bin i
  double getMass();        
  double getMass(int);
  double getFrac(int);
  double getN();          // Fetch all the numbers to local N, return total
  double getN(int i) {return n[i];}     // Fetch numbers in bin i
  double getNTot() {return m_dNumber;}
  

};

#endif    // __QALPSD_H
