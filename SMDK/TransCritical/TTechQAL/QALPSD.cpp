#include "stdafx.h"


#include "QALPSD.h"


// Get the volume in bin i
static MInitialiseTest InitTest("Precip");

static MSpeciePtr  spTHA           (InitTest, "Al[OH]3(s)", false);



double QALPSD::getVol(int i) 
{
  return 0.;

}


double QALPSD::getMass()
{
  return 0.;  
}
double QALPSD::getMass(int i)
{  
return 0.;
}
double QALPSD::getFrac(int i)
{
  if  (m_dVol==0.0) 
    return 0.0;
  double D2 = Sqr(D[i]);
  double D3 = D2*D[i];
  double avvol = PI/6.*D3*DIST_CORR*1.0e-18; 
  return n[i]*avvol/m_dVol;
}


QALPSD::QALPSD(MStream &s) 
{  

  MIPSD & sP = s.IF<MIPSD>(false);
  const double THAMass = s.MassVector[spTHA];         // Al[OH]3(s)
  

  double solidsContent = THAMass/s.Mass();// kg PSD solids/kg slurry @ref
  
  //dbg.log("solids content %12.7f", solidsContent);

  double sumar = 0.0, sumv=0.0, sumn=0.0;
  int nClasses=sP.getSizeCount();
  for (int i = 0; i<nClasses; i++) {
    double D = sP.getSize(i)*1.0e6;
    double D2 = Sqr(D/*[i]*/);
    double D3 = D2*D/*[i]*/;
    double avvol = PI/6.*D3*DIST_CORR*1.0e-18;
    if (i==0) avvol = PI/6.*D3*.125*1.0e-18;
    n[i] = solidsContent*sP.getFrac(i)/avvol/2420;
    sumn += n[i];
    sumar += D2*n[i];
    sumv += D3*n[i];
  }
  m_dArea = PI*1.0e-12*AREA_CORR*sumar;
  m_dVol = PI/6*1.0e-18*DIST_CORR*sumv;
  m_dNumber = sumn;
  

}          // Fetch all the numbers to local N, return total

