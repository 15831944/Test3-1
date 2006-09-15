#include "hxutils.h"
#define pi 3.1415926535


double  ft(double Re) 
{
  // Friction Factor for Tubeside Flow - Mills'''
  double z = (0.79 * log(Re) - 1.64);
  return 1./(z*z);
}




double NuG(double Re, double Pr) {

  // Gnielinskis formula for Nussalt number//
  double f = ft(Re);
  return (f/8) * (Re-1000)*Pr/(1+12.7*sqrt(f/8)*(pow(Pr, (2. / 3.)) - 1));
}


double NuR(double Re, double Pr) {  // Simple Nussalt correlation
  return 0.023*pow(Re, 0.8)*pow(Pr, 0.4);
}
  

double J_l(double s1, double s2) {
  // Correction factor for baffle leakage effects on HTC
  // Perry, Fig 10.21//
  double p = 0.58 - (0.58 - 0.19) * s1;
  double q = 0.81 - (0.81 - 0.62) * s1;
 double r = 0.86 - (0.86 - 0.715) * s1;
  double s = 0.89 - (0.89 - 0.789) * s1;
  if (s2 > 0.2)
    return q - (q - p) * (s2 - 0.2) / 0.5;
  if (s2 > 0.1)
    return r - (r - q) * (s2 - 0.1) / 0.1;
  if (s2 > 0.05)
    return s - (s - r) * (s2 - 0.05) / 0.05;
  return  1 - (1 - s) * s2 / 0.05;
}
			       

double J_c(double x) {
  // Correction Factor for baffle configuration effects
  // Perry, Fig 10.20//

  double tmp = (0.5310547 + x * (-0.02324819 + x * (-0.4211332)));
  double tmp2 = 1 + x * (-1.690487 + x * (2.026821 + x * 
					  (-2.098109 + x * 0.8484907)));
  return tmp / tmp2;
}
			       

double J_b(double x) {
  //   Correction Factor for Bypass Flow//
  return (1.000113 + x * (2.637655 + x * (-1.129395))) /
    (1 + x * (3.704389 + x * (3.842838)));
}
			       


double jk(double Re) {
  // jk for tube bank: Perry 10.19//
  double   x = log10(Re);
  if (Re > 1000) 
    return pow(10., (-0.6001735 + x * (-0.36008)));
  
        
  else if  (Re > 10)
    return pow(10.,  (0.09901971 + x * (-0.132939 + x * 
					(-0.8356038 + x * 
					 (0.1127422))) / 
		      (1. + x * (0.6155369))));
  else  
    return pow(10.,  (0.2626817 + x * (-0.7312016)));
}
			       

double NuD1(double Re, double Pr) {
  // Nussalt number based on above correlation//
  return jk(Re)*Re*pow(Pr, (1/3.));
}
			       
    
double NuD(double Re, double Pr) {
  // Nussalt number for flow over cylinder, used as basis for tube banks//

    
  double tmp = 0.62*sqrt(Re)*pow(Pr, (1/3.))/
    pow((1+pow((0.4/Pr),(2/3.))),0.25);
  if (Re<10000)
    return 0.3+ tmp;
  else if (Re<400000)
    return 0.3+tmp*(1+sqrt(Re/282000.));
  else
    return 0.3+tmp*pow(1+pow((Re/282000.),0.625),0.8);
}

double NuD10(double Re, double Pr, double tubeOD, double tPitch, double lPitch) 
{    
  return phi(tubeOD, tPitch, lPitch)*NuD(Re, Pr);
}



double phi(double tubeOD, double tPitch, double lPitch) {
    // Arrangement factor, Mills 4.116, 4.117. If lPitch is not given,
    // staggared arrangement applies, otherwise aligned//
  double pT = tPitch/tubeOD;
  if (lPitch==0.0) 
    return 1+2/(3.*pT);
  double pL=lPitch/tubeOD;
  double psi;
  if (pL>=1.)
    psi=1-pi/(4*pT);
  else
    psi=1-pi/(4*pT*pL);
  return 1+0.7/pow(psi,1.5)*(lPitch/tPitch-.3)/pow (lPitch/tPitch+.7, 2);
}



double LL(double  x,  double x1,double y1, double x2, double y2) {
  double b = log(y1 / y2) / log(x1 / x2);
  return pow (y1*x/x1, b);
}


double ff(double Re) {
    //Friction Factor for Shellside flow - Perry Figure 10.25//

  if (Re > 1000)
    return LL(Re, 1000., 0.27, 100000., 0.12);
  else if (Re > 100) {
    double x = log(Re);
    return exp((17.65544 + x*(-5.650442 + x*(0.3480207))) /
	       (1 + x * (0.3826142)));
  }
  
  else
    return LL(Re, 1, 70, 100, 0.7);
}

// Support functions for R_l, pressure drop correction for baffle leakage effects

double R_l0(double x) {
    return (0.9996376 + x * (5.333918)) /
      (1. + x * (11.75918 + x * (-5.43241 + x * (22.47808))));
}


double R_l25(double x) {
    return (0.9986195 + x * (0.1098581)) /
      (1. + x * (7.484978 + x * (-19.34124 + x * (28.30375))));
}

double R_l50(double x ) {
    return (0.9999084 + x * (-1.156581)) /
      (1. + x * (7.148102 + x * (-21.34003 + x * (25.93534))));
}



double R_l75(double x ) {
    return (0.9998338 + x * (-0.7673905)) /
      (1 + x * (10.8405 + x * (-35.02375 + x * (84.00896))));
}



double R_l100(double x ) {
  return (1. + x * (1.947125)) /
      (1 + x * (24.16802 + x * (-113.3414 + x * (561.1653))));
}



double LinMap(double x, double x1, double y1, double x2, double y2) {
  return y1 + (x - x1) * (y2 - y1) / (x2 - x1);
}

double R_l(double s1, double s2 ) {
    // Baffle leakage correction for pressure drop, Perry, Fig 10.26//
  double y1,y2;
  if (s1 > 0.75) {
    y1 = R_l75(s2);
    y2 = R_l100(s2);
    return LinMap(s1, 0.75, y1, 1, y2);
  }
  else if (s1 > 0.5) {
    y1 = R_l50(s2);
    y2 = R_l75(s2);
      return LinMap(s1, 0.5, y1, 0.75, y2);
  }
  else if  (s1 > 0.25) {
    y1 = R_l25(s2);
    y2 = R_l50(s2);
    return LinMap(s1, 0.25, y1, 0.5, y2);
  }   
  else {
    y1 = R_l0(s2);
    y2 = R_l25(s2);
      return LinMap(s1, 0, y1, 0.25, y2);
  }
}



double R_b(double x ) {
    // Correction for pressure drop on bypass flow, Pery Fig 10.27//
    return (0.9996853 + x * (-1.604417 + x * (0.7292036))) /
      (1. + x * (2.402047 + x * (1.008021)));
}



//  Test for R_b
// for i in range(15) {
//     x=i/20.
//     print x, "\t", R_b(x)


// for i in range(20) {
//     x=i/20.
//     if i<=15: print x, "\t", R_l(0, x), "\t", R_l(.25, x), "\t",
//     if i<=12: print R_l(.5,x), "\t",
//     if i<=8: print R_l(.75,x), "\t",
//     if i<=7: print R_l(1.,x), "\t",
//     print
