//$T Corr.c GC 1.136 01/11/02 11:22:47
// Corr.f -- translated by f2c (version of 23 April 1993  18:34:30).
//   You must link the resulting object file with the libraries:
//	-lf2c -lm   (in that order)
//
#include "stdafx.h"
#include "sc_defs.h"
#include "flwnode.h"
#define  __CORR_CPP

#include "myf2c.h"
#include "PrecipPSD.h"


// **********************************************************************
//     DenLiquor:   Function to calculate liquor density.
//     Based on correlation in the CRC one pass model.

// **********************************************************************
double CPrecipPSD::DenLiquor( CPSDConc & Conc, double tempk)
  {
  // ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
  //     temp          liquor temperature                             deg C
  //

  double temp=K2C(tempk);
  double DenL=0.0;
  if(m_Corr.m_DensCorr == eDensCorrQAL1999 && m_pCnBm)
    {
    #ifndef _RELEASE
    double cs = Conc.c / m_ModelParms.m_Cs2C;
    double DenL_ = 1185.0 + 0.56*((1.88*cs + Conc.a) - 300.0) - 0.6*(temp - 25.0);
    #endif
    //kga todo: this may need fixing??? (ie is it OK to use m_pCnBm ?)
    DenL = m_pCnBm->Rho(som_Liq, tempk, m_pCnBm->Press());
    /*if (1)
      {
      State2Conc( m_CnState, m_CnConc, NULL);
      State2Model( m_CnState, m_pCnBm, m_pCnDist);
      }
    double DenL__ = m_pCnBm->Rho(som_Liq, tempk, m_pCnBm->Press());*/
    }
  else
    {
    double cs = Conc.c / m_ModelParms.m_Cs2C;
    DenL = 1185.0 + 0.56*((1.88*cs + Conc.a) - 300.0) - 0.6*(temp - 25.0);
    }
  return DenL;
  } // DenLiquor


// ***********************************************************************
//     Solub : function to calculate equilibrium solubility.
//     A number of different correlations are used

// ***********************************************************************
double CPrecipPSD::Solub(CPSDConc & Conc, double tempk, double * pIonicStrength/*=NULL*/)
  {
  // Builtin functions
  double sqrt(double), pow_dd(double, double);

  // ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
  //     temp          liquor temperature                             deg C
  //
  double cs = Conc.c / m_ModelParms.m_Cs2C;
  double temp = K2C(tempk);
  double tooc = Conc.toc * 105.99 / 12.011;
  double ret_val = 0.0;
  if (pIonicStrength)
    *pIonicStrength = 0.0;

  if(m_Corr.m_SolCorr == eSolCorrMisra1970)
    {//     Misra 1970 Correlation
    ret_val = Conc.c * Exps(6.2106 - (2486.7 - Conc.c * 1.0876) / tempk);
    }
  else if(m_Corr.m_SolCorr == eSolCorrRodda1989)
    {//     Rodda 1989 Correlation
    ret_val = cs * (temp * 0.005713 + cs * 9.77e-4 - 0.2641);
    }
  else if(m_Corr.m_SolCorr == eSolCorrRodda1990)
    {//     Rodda 1990 Correlation
    ret_val = cs * (temp * 0.004215 + cs * 5.56e-4 - 0.071285);
    }
  else if(m_Corr.m_SolCorr == eSolCorrRodda1991)
    {//     Rodda 1991 Correlation
    ret_val = cs * (temp * 0.006225 + cs * 6.35e-4 + Conc.co3 * 3.34e-4 + tooc * 1.65e-4 - 0.281524);
    }
  else if(m_Corr.m_SolCorr == eSolCorrRosenberg1996)
    {//     Rosenberg 1996 Correlation
    const double alpha0 = -9.2082;
    const double alpha3 = -0.8743;
    const double alpha4 = 0.2149;
    const double E_ = -30960.0;
    const double R = 8.3145; //Universal Gas Constant
    const double k1 = 0.9346;
    const double k2 = 2.0526;
    const double k3 = 2.1714;
    const double k4 = 1.6734;
    
    const double ionic = 0.01887*cs + k1*Conc.cl/58.44 + k2*Conc.co3/105.99 + k3*Conc.so4/142.04 + k4*0.01887*Conc.toc;
    const double I2 = sqrt(ionic);
    const double E = alpha0 * I2 / (1.0 + I2) - ionic * (alpha3 + alpha4 * I2);
    const double tmp_ = E_ / (R * tempk);
    ret_val = 0.96197*cs / (1.0 + pow_dd(10.0, E) / Exps(tmp_));
    if (pIonicStrength)
      *pIonicStrength = ionic;
    }
  else if(m_Corr.m_SolCorr == eSolCorrYoung1997)
    {//     Young 1997 Correlation
    ret_val = cs * (temp * 0.004215 + cs * 5.56e-4 - 0.071285);
    ret_val = cs * Exps(4.26 - (2147.97 - (cs + Conc.co3) * 0.9953) / tempk + Conc.toc * 0.007927);
    }
  else if(m_Corr.m_SolCorr == eSolCorrQAL1999)
    {//     QAL 1999 constants using Rosenberg 1996 Correlation
    const double alpha0 = -8.7168;
    const double alpha3 = -0.7767;
    const double alpha4 = 0.2028;
    const double E_ = -30960.0;
    const double R = 8.314472; //Universal Gas Constant
    const double k1 = 1.5532;
    const double k2 = 1.3437;
    const double k3 = 2.2571;
    const double k4 = 1.1997;
    
    const double ionic = 0.01887*cs + k1*Conc.cl/58.44 + k2*Conc.co3/105.99 + k3*Conc.so4/142.04 + k4*0.01887*Conc.toc;
    const double I2 = sqrt(ionic);
    const double E = alpha0 * I2 / (1.0 + I2) - ionic * (alpha3 + alpha4 * I2);
    const double tmp_ = E_ / (R * tempk);
    ret_val = 0.96197*cs / (1.0 + pow_dd(10.0, E) / Exps(tmp_));
    if (pIonicStrength)
      *pIonicStrength = ionic;
    }

  return ret_val;
  } // Solub


// ***********************************************************************
//     GRate : function to calculate growth rate.
//     Based on correlation by E.T. White (1988)
//     NOTE: this expression uses caustic as Na2O

// ***********************************************************************
double CPrecipPSD::GRate( CPSDConc & Conc, double  tempk, double SolConc, double SSA)
  {
  double  ret_val;

  // Builtin functions
  double exp(double), pow_dd(double , double );

  // ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
  //
  //     Local  variables
  //     a             mass Al2O3/volume liquor ratio                   g/l
  //     astar         mass Al2O3 at equilibrium/volume liquor ratio    g/l
  //     temp          slurry temperature                             deg C
  //     tempk         slurry temperature                             deg K
  //     Solub         solubility correlation
  // ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
  //
  double temp = K2C(tempk); // deg K

  double astar = Solub(Conc, tempk); //kga: astar same as Aeq in old precip model ie "Equilibrium A Conc"
  //double cs = Conc.c / m_ModelParms.m_Cs2C;
  //double ACequilibrium = astar/cs;

  if (m_Corr.m_GrowCorr == eGrowCorrQAL2001)
//    {// QAL Correlation 21 September 2001 Daniel Thomas 
//    //K = K0*exp(-ActivationEnergy/T)*Exps(-TOOC2C*C*k_TOC) * Pow(GTZ(S_out),n_s) * Pow(GTZ(FC),n_fc) * Pow(GTZ(ACeq),n_eq);
//
//    double cs=Conc.c/m_ModelParms.m_Cs2C; 
//    double tooc = 105.99 * Conc.toc / 12.011; //TOOC = TOC*106/12
//    double TOOC2C = tooc/cs;
//    double S_out = cs+Conc.co3; //NOTE: SodaConc (S) includes CausticSoda (NaOH) AND CausticSoda found in SodiumAluminate (NaAl[OH]4) AND SodiumCarbonate (Na2CO3)
//    double FC = cs *(1.0 - 105.99*Conc.acs/101.96);//CausticConc(T_)*(1.0 - SodiumCarbonate.MW*AtoC()/InertAlumina.MW);
//
//    double ACeq = astar/cs;
//
//    const double a=Exps(-m_GlobalVars.m_G_ActivationEnergy/tempk);
//    const double b=Exps(-TOOC2C*cs*m_GlobalVars.m_G_k_TOC);
//    const double c=Pow(GTZ(S_out), m_GlobalVars.m_G_n_s);
//    const double d=Pow(GTZ(FC), m_GlobalVars.m_G_n_fc);
//    const double e=Pow(GTZ(ACeq), m_GlobalVars.m_G_n_eq);
//    const double K = m_GlobalVars.m_G_K0*a*b*c*d*e;
//    ret_val = K;
//    }
//  else if (m_Corr.m_GrowCorr == eGrowCorrQAL2001a)
    {// QAL Correlation 21 September 2001 Daniel Thomas 
    //K = K0*exp(-ActivationEnergy/T)*Exps(-TOOC2C*C*k_TOC) * Pow(GTZ(S_out),n_s) * Pow(GTZ(FC),n_fc) * Pow(GTZ(ACeq),n_eq);

    double cs=Conc.c/m_ModelParms.m_Cs2C; 
    double tooc = 105.99 * Conc.toc / 12.011; //TOOC = TOC*106/12
    double TOOC2C = tooc/cs;
    double S_out = cs+Conc.co3; //NOTE: SodaConc (S) includes CausticSoda (NaOH) AND CausticSoda found in SodiumAluminate (NaAl[OH]4) AND SodiumCarbonate (Na2CO3)
    double FC = cs *(1.0 - 105.99*Conc.acs/101.96);//CausticConc(T_)*(1.0 - SodiumCarbonate.MW*AtoC()/InertAlumina.MW);

    double ACeq = astar/cs;

    const double fAct = Exps(-m_GlobalVars.m_G_ActivationEnergy/tempk);
    const double fTOC = Exps(-TOOC2C*cs*m_GlobalVars.m_G_k_TOC);
    const double fS   = Pow(GTZ(S_out), m_GlobalVars.m_G_n_s);
    const double fFC  = Pow(GTZ(FC), m_GlobalVars.m_G_n_fc);
    const double Diff = Conc.acs-ACeq;
    const double fDrv = Pow(fabs(Diff), m_GlobalVars.m_G_n_);
    const double fSol = Pow(GTZ(SolConc), m_GlobalVars.m_G_n_sol);
    const double fSSA = Pow(GTZ(SSA), m_GlobalVars.m_G_n_ssa);
    
    const double K    = m_GlobalVars.m_G_K0*fAct*fTOC*fS*fFC*fDrv*fSol*fSSA;

    ret_val = K/GTZ(SSA*Conc.a*m_ModelParms.m_RhoH/Conc.cs);// m/hr
    }
  else if (m_Corr.m_GrowCorr == eGrowCorrCRC1998)
    {//     CRC Correlation 1998 for pure liquors
    if(astar >= 1.0)
      {// avoid divide by zero
      ret_val = 3.596 * Exps((1.0/tempk - 1.0/346.16) * -12543.9);

      double d1 = (Conc.a - astar) / astar;
      ret_val *= (d1*d1); // micron/hour
      ret_val *= 1e-6;
      }
    else
      {
      ret_val = 0.0;
      }
    }
  else
    {//     White Correlation 1988 for pure liquors
    if(Conc.c >= 1.0) 
      {// avoid divide by zero
      double d1 = Conc.c / 100.0;
      ret_val = 15.0 * Exps((1.0/tempk - 1.0/346.16) * -7600.0) / pow_dd(d1, 0.5);

      d1 = (Conc.a - astar) / Conc.c;
      ret_val *= (d1 * d1); // micron/hour
      ret_val *= 1e-6; // micron -> metres
      }
    else
      {
      ret_val = 0.;
      }
    }

  if(Conc.a < astar)
    {
    ret_val = -ret_val;
    }

  return ret_val;
  } // GRate


// ***********************************************************************
//     NRate : function to calculate nucleation rate.
// ***********************************************************************
double CPrecipPSD::NRate( CPSDConc & Conc, double surf, double tempk)
  {
  double        ret_val, d__1;

  // Builtin functions
  double            pow_dd(double , double );

  double        astar;

  // ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
  //
  //     Local  variables
  //     a             mass Al2O3/volume liquor ratio                   g/l
  //     astar         mass Al2O3 at equilibrium/volume liquor ratio    g/l
  //     temp          slurry temperature                             deg C
  //     tempk         slurry temperature                             deg K
  //     Solub         solubility correlation
  //     Surf          surface area of hydrate                          m^2
  //
  // ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
  //
  if(m_Corr.m_NucCorr == -1)
    {
    INCOMPLETECODE(__FILE__, __LINE__);
    }
  else  // eNucCorrMisra1970
    {
    //     Based on correlation by Misra (1970)
    if(Conc.c >= 1.0)
      {// avoid divide by zero
      astar = Solub(Conc, tempk);
      ret_val = 5e8;

      d__1 = GTZ(Conc.a - astar) / Conc.c;

      ret_val = ret_val * surf * pow_dd(d__1, 2.0);
//dbgpln("N:%12.8f %12.8f %12.8f %12.8f %12.8f", ret_val, astar, Conc.a, Conc.c, K2C(tempk));
      }
    else
      {
      ret_val = 0.0;
      }
    }

  return ret_val;
  } // NRate


// ***********************************************************************
//     ARate : function to calculate agglomeration rate.
//     Based on correlation by Ilievski (1991)
//     NOTE: this expression uses caustic as Na2O
// ***********************************************************************
double CPrecipPSD::ARate( CPSDConc & Conc, double  tempk)
  {
  double ret_val;

  // Builtin functions
  double            pow_dd(double , double );

  // ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
  //     astar         mass Al2O3 at equilibrium/volume liquor ratio    g/l
  //     temp          slurry temperature                             deg C
  //     Solub         solubility correlation
  // ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
  //
  //     Based on correlation by Ilievski (1991)
  if(m_Corr.m_AgglCorr == -1)
    {
    INCOMPLETECODE(__FILE__, __LINE__);
    }
  else // eAgglCorrIlievski1991
    {
    if (Conc.c >= 1.)
      {// avoid divide by zero
      double astar = Solub(Conc, tempk);
      double d1 = (Conc.a - astar) / Conc.c;
      ret_val = 5e-4;  // ml/number/hour
      ret_val *= 1e-6; // m^3/number/hour
      ret_val *= pow_dd(d1, 4.0);
      }
    else
      {
      ret_val = 0.0;
      }
    }

  return ret_val;
  } // ARate


// ***********************************************************************
//     SodaC : function to calculate ratio of bound soda as
//     mass Na2O/mass Al(OH)3
//     NOTE: this expression uses caustic as Na2O
// ***********************************************************************
double CPrecipPSD::SodaC( CPSDConc & Conc, double  tempk)
  {
  double  ret_val;

  // ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
  //
  //     Local  variables
  //     astar         mass Al2O3 at equilibrium/volume liquor ratio    g/l
  //     temp          slurry temperature                             deg C
  //     Solub         solubility correlation
  // ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
  //
  double astar = Solub(Conc, tempk); //kga: astar same as Aeq in old precip model ie "Equilibrium A Conc"
  double temp  = K2C(tempk); // deg K

  if(m_Corr.m_SodaCorr == eSodaCorrQAL1999)
    {//     Based on QAL correlation (1999)
    double cs = Conc.c / m_ModelParms.m_Cs2C;
    double tooc = 105.99 * Conc.toc / 12.011;
    double S = cs+Conc.co3; //NOTE: SodaConc (S) includes CausticSoda (NaOH) AND CausticSoda found in SodiumAluminate (NaAl[OH]4) AND SodiumCarbonate (Na2CO3)
    double d1 = (Conc.a - astar) / cs;
    double k1x = m_GlobalVars.m_K1_BoundSoda * (0.000598*cs - 0.00036*temp + 0.019568*tooc/cs) * (1.0 - 0.758*cs/S);
    ret_val = k1x * d1 * d1;
    //double k1x = dK1 * (0.000598*ACF.C_at25 - 0.00036*K_2_C(ACF.T) + 0.019568*ACF.TOOC2C) * (1.0 - 0.758*ACF.C2S);
    //BoundSoda = k1x * Sqr(ACF.ACout-ACF.ACeq);
    }
  else if(m_Corr.m_SodaCorr == eSodaCorrOhkawa1985)
    {//     Based on correlation by Ohkawa(1985)
    double d1 = (Conc.a - astar) / Conc.c;
    ret_val = 1.27e-5 * Exps(2535.0/tempk) * d1 * d1;
    }
  else if(m_Corr.m_SodaCorr == eSodaCorrQAL1988)
    {//     Based on QAL correlation (1988)
    double cs = Conc.c / m_ModelParms.m_Cs2C;
    double tooc = 105.99 * Conc.toc / 12.011;
    double d1 = (Conc.a - astar) / cs;
    ret_val = (0.000598*cs - 0.00036*temp + 0.019568*tooc/cs) * d1 * d1;
    ret_val *= m_ModelParms.m_Hyd2A * m_ModelParms.m_Cs2C; //(0.654 * 0.585) = 0.38259
    }
  else if(m_Corr.m_SodaCorr == eSodaCorrCRC1993)
    {//     Based on CRC correlation (1993)
    double d1 = Conc.a - astar;
    ret_val = 2.733e-6 * d1 * d1;
    ret_val *= m_ModelParms.m_Hyd2A;
    }
  else
    {//     Based on correlation by Sang(1988)
    double d1 = Conc.a - astar;
    ret_val = 4.74e-6 * d1 * d1;
    ret_val *= m_ModelParms.m_Hyd2A;
    }

  return ret_val;
  } // SodaC

