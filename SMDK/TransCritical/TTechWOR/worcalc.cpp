//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
//   New Precipitation model Transcritical Technologies Pty Ltd Feb 05
//   Time-stamp: <2006-07-11 12:39:54 Rod Stephenson Transcritical Pty Ltd>
// Copyright (C) 2005 by Transcritical Technologies Pty Ltd and KWA
//===========================================================================



void CPrecipitator::AgglomKernel() {
  double dm;
  int min;

  for (int i=0; i<=nClasses; i++) {
    for (int j=0; j<=nClasses; j++) {
      min = Min(i, j);
      dm = 1.0 - Sqr((Lav[i]+Lav[j])/Lav[m_lMaxAgglomClass]);
      if (dm<0.0)
	beta[i][j]=0;
      else
	beta[i][j] = Sqr(Lav[i]+Lav[j])/Lav[min]*dm;
    }
  }
  
}



void CPrecipitator::Agglomeration(double Growth, double SD) {
  double ag; 
  agmin=0.0;
  double fctr = m_dKAgglom/pow(SD, m_dgamma_2)*pow(Growth, m_dgamma_g); 

  for  (int i=1; i <= nClasses; i++) {
    if (beta[i-1][1]==0.0)
      aggl[i]=0.0;
    else {
      aggl[i]=0.0;
      if (i>1) {   /// A+ term
	ag=0.0;
	for (int j=0; j<=i-2 && beta[i-1][j]>0; j++)
	    ag+=exp((j-i+1)*.6931472)*beta[i-1][j]*n[j];
	aggl[i] = n[i-1]*ag + 0.5*beta[i-1][i-1]*Sqr(n[i-1]);
      }
      ag = 0.0;
      for  (int j=0; j<=i-1 && beta[i][j]>0; j++)   // A- term 1
	  ag += exp((j-i)*0.6931472) * beta[i][j] * n[j];
      aggl[i] -= n[i] * ag;
      agmin   -= n[i] * ag;
      ag = 0;
      for  (int j=i;  j <= nClasses  &&  beta[i][j]>0; j++)  // A- term 2
	  ag += beta[i][j] * n[j];
      aggl[i] -= n[i] * ag;
      agmin  -= n[i] * ag;
      aggl[i]*=fctr;

    }
  }
  agmin *= fctr;
}


     
double CPrecipitator::GrowthIn( MStream &s)
{
  
  double hulp, term;
  MIBayer & sB=s.IF<MIBayer>(false);   // Do the checks up front
  double T = s.T;
  double C = sB.CausticConc(T);
  if (C<ZeroLimit)
    {
    m_dsigma = 0.0;
    return 0.0;
    }

  double AC = sB.AtoC();
  double ASat = sB.AluminaConcSat(T);
  double ACEquil = ASat/C;

  switch (m_lGrowthTerm) {
  case GRM_sigma: {
    term = (AC-ACEquil) / (ACEquil*(1-1.0392*AC));
    break;
  }
  case GRM_AAEq: {
    term = sB.AluminaConc(T)-ASat;
    break;
  }
  }
  //m_dsigma = term;
  hulp = m_dSurfaceAct * m_dK_G *
    exp(m_dActEGrowth/T) * pow(fabs(term),m_dgamma_g);
  m_dsigma = m_dActEGrowth/T;

  if (AC > ACEquil)
    return  hulp;
  else 
    return 0.0;
    
}


double CPrecipitator::Soda(MStream &s) {

  double term;
  MIBayer & sB=s.IF<MIBayer>(false);   // Do the checks up front
  double T = s.T;
  double C = sB.CausticConc(T);
  if (C<ZeroLimit)
    {
    return 0.0;
    }
  switch (m_lSodaTerm) {
  case 0: {
    double AC = sB.AtoC();
    double ACEquil = sB.AluminaConcSat(T)/C;
    term  = AC-ACEquil; 
    break;
  }
  case 1:
    term = m_dGrowth*3600*1.0e9;
    break;
  }
  if (term <= 0.0) return 0.0;
  return m_dSodaConst*exp(m_dActESoda/T)*pow(fabs(term), m_dgamma_s);
}


// Nucleation Rate; particles into bin[1] per hour per m^3

double CPrecipitator::NucleationIn( MStream &s)  
{
  
  double term;
  MIBayer & sB=s.IF<MIBayer>(false);   // Do the checks up front
  double T = s.T;
  double C = sB.CausticConc(T);
  double ASat = sB.AluminaConcSat(T);
  switch (m_lNuclTerm) {
   
  case GRM_AAEq:     
    term  = sB.AluminaConc(T)-ASat;
    break;
  case GRM_sigma: {
    double AC = sB.AtoC();  
    double ACEquil = ASat/C;
    term  = (AC-ACEquil) / (ACEquil*(1-1.0392*AC));
    break;
  }
  }
  if (term <= 0) return 0;
  return m_dNuclPlantCorr * m_dK_N * exp(m_dActENucleation/T) *
             pow(fabs(term), m_dgamma_N) * m_PSD.getArea();
}
  

void CPrecipitator::Numbers(MStream & Tank) 
{

  static const double bayerRefTemp = 273.15+25;
  static const double r = 1.25992104989;
  static const double ConA = 3*r;     //{ 3.7798}
  static const double ConB = -3.;      //{-3.0000}

  double GrowthRate[nClasses+1];
  double deltaN[nClasses+1];

  double Growth = GrowthIn(Tank);

  m_dGrowth = Growth/1.0e6/3600;
  m_dNuclN = NucleationIn(Tank);

  dSoda = Soda(Tank);
  double SD = Tank.MassFlow(MP_Sol)/Tank.VolumeFlow(MP_All);
  double term;
  Agglomeration(Growth, SD);

  if (!bGrowthOn) Growth=0.0;
  // Check for largest growth???
  double gs = 0.0;
  for (int i=1; i<=nClasses; i++) {
    if (i<nClasses) 
      term = n[i]*ConB;
    else
      term = 0;
    if (i>1) term+=n[i-1]*ConA;
    GrowthRate[i] = Growth/L[i]*term/3600;
    gs += GrowthRate[i];
  }
  deltaN[0] = gs;

  for (int i=1; i<=nClasses; i++) {
    double dn = (Feed.VolumeFlow(MP_All, bayerRefTemp)*m_PSDin.getN(i)
		   -Tank.VolumeFlow(MP_All, bayerRefTemp)*n[i])/dTankVol;
    if (bAgglomOn) dn+=aggl[i]/3600; 
    if (bGrowthOn) dn+=GrowthRate[i];
    deltaN[i] = dn;
  }
  
  if (bNuclOn) {
      deltaN[1] += m_dNuclN/3600;
  }
  NewN[0] = 0;
  double acc = m_dAcc;
  // Reduce acceleration if iteration number is large... usually a sign that
  // there is overshoot
  if (m_lIterations > 500) acc /=2;
  if (m_lIterations > 1000) acc /=2;
  if (m_lIterations > 1500) acc /=2;
  


  // Check that none of the rates lead to negative numbers...			     
  for (int i=1; i<=nClasses; i++) {
    if (deltaN[i]<0) {
      double atmp = -(1-dAggMinLim)*n[i]/deltaN[i];
      if (atmp<acc) {
	acc = atmp;
      }
    }
  }
  
  for (int i=1; i<=nClasses; i++) {
    NewN[i] = n[i] + deltaN[i]*acc;
  }


  //  if (!err)
  for (int i=1; i<=nClasses; i++) {

    switch (iDType) {
    case 0:
      dd[6][i]=GrowthRate[i];
      break;
    case 1:
      dd[6][i]=aggl[i]/3600;
    break;
    case 2:
      dd[6][i]=deltaN[i];
      break;
    case 3:
      dd[6][i]=n[i];
      break;
    case 4:
      dd[6][i]=NewN[i];
      break;
    }
  }
  




}

