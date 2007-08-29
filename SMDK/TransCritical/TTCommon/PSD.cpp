#include "stdafx.h"

#include "PSD.h"






double PSD::getVol(int i) 
{
  return 0.;
}


double PSD::getMass()
{
  return 0.;  
}
double PSD::getMass(int i)
{  
return 0.;
}

double PSD::getFrac(int i)
{
  if  (m_dVol==0.0) 
    return 0.0;
  double D2 = Sqr(D[i]);
  double D3 = D2*D[i];
  double avvol = PI/6.*D3*m_dVolumeCorrection; 
  return n[i]*avvol/m_dVol;
}

void PSD::putN(MStream &s) 
{
  double *v = new double[nClasses];
  double sum = 0.0;
  MIPSD & sP = s.IF<MIPSD>(false);
  for (int i=0; i<nClasses; i++) {
    v[i] = n[i]*D[i]*D[i]*D[i];
    sum +=  v[i];
  }
  for (int i=0; i<nClasses; i++)
    sP.putFrac(i, v[i]/sum);
  

  delete [] v;
  
}




// PSD::PSD(MStream &s, MSpeciePtr p) doesnt work!
PSD::PSD(MStream &s, MSpeciePtr &p) 
{  
  MIPSD & sP = s.IF<MIPSD>(false);
  double &PSDMass      = s.MassVector[p];         //
  const double solidsContent = PSDMass/s.Mass();        // kg PSD solids/kg slurry @ref
  solidsDensity = gs_MVDefn[p].Density();
  
  m_dVolumeCorrection = 1.0;
  m_dAreaCorrection = 1.0;

  
  double sumar = 0.0, sumv=0.0, sumn=0.0;
  nClasses=sP.getSizeCount();
  n = new double[nClasses];
  D=  new double[nClasses]; 
  sP.ExtractSizes(D);

  // Bin 0 is special... 

  double D2 = Sqr(D[0]);
  double D3 = D[0]*D2;
  double avvol = PI/6.*D3*.125;
  n[0] = solidsContent*sP.getFrac(0)/avvol/solidsDensity;
  sumn = n[0];
  sumar = D2*n[0];
  sumv =  D3*n[0];
  
  for (int i = 1; i<nClasses; i++) {
    D2 = Sqr(D[i]);
    D3 = D2*D[i];
    avvol = PI/6.*D3*m_dVolumeCorrection;
    n[i] = solidsContent*sP.getFrac(i)/avvol/solidsDensity;
    sumn += n[i];
    sumar += D2*n[i];
    sumv += D3*n[i];
  }
  m_dArea = PI*m_dAreaCorrection*sumar;
  m_dVol = PI/6*m_dVolumeCorrection*sumv;
  m_dNumber = sumn;

}          






PSD::~PSD() 
{
  delete [] n;
  delete [] D;
}


double PSD::Nucleation(double N0, int bin0) 
{
  ndot[bin0] += N0;
  double D2 = Sqr(D[bin0]);
  double dv = N0*PI/6.*D2*D[bin0]*m_dVolumeCorrection;
  return dv*solidsDensity;
}

double PSD::Growth(double deltaR, int from) 
{
  // This applies only when volume ratios are 2:1 for adjacent bins...
  const double aa = 6.0*m_dAreaCorrection/m_dVolumeCorrection;
  double thp = 0.0;  // Total hydrate from precip
  for (int i=from; i<nClasses-1; i++) {
    const double dni = aa*deltaR/D[i]*n[i];
    ndot[i] -= dni;
    ndot[i+1] += dni;
    thp += dni*D[i]*D[i]*D[i];
}
  // Last slot is special... no higher slot to grow into so add numbers...
  const int j = nClasses-1;
  ndot[j] = aa*deltaR/D[j]*n[j];
  thp += ndot[j]*D[j]*D[j]*D[j];
  // return sum_from^NClasses-1 ndot[j]*vbar[j]*rho
  thp *= (solidsDensity*m_dVolumeCorrection);
  return thp;
}


void PSD::Agglomeration(double arate, AgglomKernel& ak) 
{
  for (int i=0; i<nClasses-1; i++) {
    int k = i+1;
    // Special case when j=i
    // ie agglomeration of particles of equal sizes.
    double NN = ak.beta[i][i]*Sqr(n[i])*arate;
    ndot[i] -= NN;
    ndot[k] += NN/2.;
    // And the rest
    for (int j=0; j<i; j++) {
      k = i;  // Destination bin
      if (k+1 < nClasses) {
	NN = ak.beta[i][j]*n[i]*n[j]*arate;
	ndot[i] -= NN;
	ndot[j] -= NN;
	ndot[k] += (1.-pow(.5, i-j))*NN;
	ndot[k+1] += pow(.5, i-j)*NN;
      }
    }
  }
}


AgglomKernel::AgglomKernel(int nc, double aAgg, double *D): nClasses(nc) {
  
  for (int i=0; i<nClasses; i++) {
    for (int j=0; j<=i; j++) {
      beta[i][j] = beta[j][i] = exp(-(D[i]*D[i]+D[j]*D[j])/aAgg);
    }
  }
}
