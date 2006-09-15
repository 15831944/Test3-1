#include "stdafx.h"

#include "brahmapsd.h"

const double L[] = {

  1.58740105197 ,
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
  256.0 ,
  322.539788773 ,
  406.374669304 ,
  512.0 
};

const double Lav[] = {

  1.78179743628 ,
  2.24492409662 ,
  2.82842712475 ,
  3.56359487256 ,
  4.48984819324 ,
  5.65685424949 ,
  7.12718974512 ,
  8.97969638647 ,
  11.313708499 ,
  14.2543794902 ,
  17.9593927729 ,
  22.627416998 ,
  28.5087589805 ,
  35.9187855459 ,
  45.2548339959 ,
  57.017517961 ,
  71.8375710918 ,
  90.5096679919 ,
  114.035035922 ,
  143.675142184 ,
  181.019335984 ,
  228.070071844 ,
  287.350284367 ,
  362.038671968 ,
  456.140143688 ,
  574.700568734 
};

const static double r = 1.25992105;
const static double conA  = (r*r+r+1)/3;                 // { 1.282441 }
const static double conV  = (r*r*r+r*r+r+1)/4 / conA;    // { 1.139882 }






BrahmaPSD::BrahmaPSD(MStream &s) 
{
  MIPSD & sP = s.IF<MIPSD>(false);
  //if (IsNothing(sP)) return;

  double rhoS = 2420;  // s.Density(MP_Sol, bayerRefTemp);    // Solids Density
  double solidsContent = s.MassFlow(MP_Sol)/s.VolumeFlow(MP_All, bayerRefTemp);// kg solids/m3 slurry @ref
  v = solidsContent/rhoS;    // m^3 solids/m^3 slurry
  double sumar = 0.0, sumv=0.0, sumn=0.0;
  for (int i = 0; i<=nClasses; i++) {
    double L2 = Sqr(L[i]);
    double L3 = L2*L[i];
    double avvol = PI/6.*L3*DIST_CORR*1.0e-18; 
    n[i] = v*sP.getFrac(i)/avvol;
    sumn += n[i];
    sumar += L2*n[i];
    sumv += L3*n[i];
  }
  m_dSumAr = PI*1.0e-12*conA*sumar;
  m_dSumVol = PI/6*1.0e-18*DIST_CORR*sumv;
  //m_dSumVol = v;
  m_dSumN = sumn;
  
}




double BrahmaPSD::put(MStream &s)
{
 
  MIPSD & sP = s.IF<MIPSD>(false);
  // if (IsNothing(sP)) return 0.0;
  double sumfr = 0.0;
  for (int i=0; i<=nClasses; i++) {
    double L3 = Sqr(L[i])*L[i];
    double avvol = PI/6.*L3*DIST_CORR*1.0e-18; 
    double frac = n[i]*avvol/m_dSumVol;
    sumfr+= frac;
    sP.putFrac(i, frac);
  }
  return sumfr;
}



BrahmaPSD::BrahmaPSD(double *n_, double SD) 
{
  double sumar = 0.0, sumv=0.0, sumn=0.0;
  for (int i=0; i<=nClasses; i++) {
    n[i]=n_[i]*SD;
    double z = L[i]*L[i]*n[i];
    sumar += z;
    sumv += z*L[i];
    sumn += n[i];
  }
  m_dSumAr = PI*1.0e-12*conA*sumar;
  m_dSumVol = PI/6*1.0e-18*DIST_CORR*sumv;
  m_dSumN = sumn;
}



BrahmaPSD::BrahmaPSD()
{

}

// Modify the PSD in a MStream



