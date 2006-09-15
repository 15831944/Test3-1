//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
//   New Precipitation model Transcritical Technologies Pty Ltd Feb 05
//   Time-stamp: <2006-06-27 09:52:28 Rod Stephenson Transcritical Pty Ltd>
//===========================================================================

#ifndef __MD_HEADERS_H
#include "md_headers.h"

#endif
//---------------------------------------------------------------------------




extern const double L[];
extern const double Lav[];



const static int nClasses=25;

static const double DIST_CORR = 1.46183;         // Correction for size variation over bin
static const double bayerRefTemp=298.15;



class BrahmaPSD {
  double n[nClasses+1];
  double m_dSumN;
  double m_dSumAr;
  double m_dSumVol;
  double v;
 public:
  // BrahmaPSD(double*);
  BrahmaPSD(double *n, double x = 1.0) ;

  BrahmaPSD(MStream &);
  BrahmaPSD();
  double getv() {return v;}
  double getN() {return m_dSumN;}
  double getArea() {return m_dSumAr;}
  double getVol() {return m_dSumVol;}
  double getN(int i) { return n[i];}
  void getN(double *n_) {for (int i=0; i<=nClasses; i++) n_[i] = n[i];}
  void setN(double x, int i)  { n[i] = x;}
  void setN(double*n_) {for (int i=0; i<=nClasses; i++) n[i] = n_[i];}
  double put(MStream &);
  double SSA();
};


