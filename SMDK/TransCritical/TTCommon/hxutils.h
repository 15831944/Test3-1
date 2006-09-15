#include <cmath> 


double ft(double Re);
double NuG(double Re, double Pr);
double NuR(double Re, double Pr);
double J_l(double s1, double s2);
double J_c(double x);
double J_b(double x);
double jk(double Re);
double NuD1(double Re, double Pr);
double NuD(double Re, double Pr);
double NuD10(double Re,double  Pr, double tubeOD, 
	     double tPitch, double lPitch=0.0);
double phi(double tubeOD, double tPitch, double lPitch=0.0);
double LL(double x, double x1,double y1, double x2, double y2);
double ff(double Re);
double R_l0(double x);
double R_l25(double x);
double R_l50(double x );
double R_l75(double x );
double R_l100(double x );
double LinMap(double x, double x1, double y1,double  x2, double y2);
double R_l(double s1, double s2 );
double R_b(double x );
