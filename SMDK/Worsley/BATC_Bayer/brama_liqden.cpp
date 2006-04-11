// Liquid Density Calculations from BRAHMA model
// One version uses TOC, the other TOS
// There are difficulties in matching SysCAD species model with the Brahma model
// Notes
//  Original code is density in g/cc, modified to g/l or kg/m^3


// Based on following Pascal Code

/* Function Density_l_in( T : prec; LiqCompL: LiquorCompositionTypeL): prec;

Var
  term1, term2, term3, term4, sum : prec;

begin
  with LiqCompL do
  begin
  term1 := 0.5 * (1 - 9.528321E-4*(T-20) - 2.64664e-6*Sqr(T-20) );
  term2 := 5e-4* SQRT(1e6 + 4e3*(0.8347*C + (0.74 + C * 4.2e-4) * A
                          + 1.048*(S-C) + 0.8007*Cl*106/71 + 1.2517*SO4*106/96 + 1.35*TOS) );
  term3 := 1.0018 * Pow( 10, -3.36587 + 0.01136 * (S-C) );
  term4 := (C - 130) * 4e-5;
  sum := term1 + term2 + term3 + term4;
  if sum > 2 then sum := 2;
  density_l_in := 0.9982 * sum;
  end;
end;



begin
  with LiqCompL do
  begin
    case PDat.Choice_Dens_Term of
      1 : begin {Old}
        term1 := 0.5 * (1 - 9.528321E-4*(T-20) - 2.64664e-6*Sqr(T-20) );
        term2 := 5e-4* SQRT(1e6 + 4e3*(0.8347*C + (0.74 + C * 4.2e-4) * A
                                + 1.048*(S-C) + 0.8007*Cl*106/71 + 1.2517*SO4*106/96 + 1.35*TOS) );
        term3 := 1.0018 * Pow( 10, -3.36587 + 0.01136 * (S-C) );
        term4 := (C - 130) * 4e-5;
        sum := term1 + term2 + term3 + term4;
        sum := 0.9982 * sum;
      end;
      2 : begin {New}
        term1 := 1.0149 + 7.0127E-4*C + 4.4411E-4*A;
        term2 := 7.4899E-4 * (S-C);
        term3 := 7.800E-4  * SO4 * 142/96 + 9.576E-4 * CL * 58/35.5;
        term4 := 1.6847E-3 * TOC-3.0501E-4 * (T - 25) - 2.877E-6 * SQR(T - 25);
        sum := term1 + term2 + term3 + term4;
      end;
    end; {case}
    if sum > 2 then sum := 2;
    density_l_in := sum;
  end;
end;

*/


// Old Brahma Density Equation...
double CBayerConcs::LiquorDensEqn5(double Tc,
				   double C, double S, double A, 
				   double Cl, double SO4, double TOS)
{




  double  term1 =  0.5 * (1 - 9.528321E-4*(Tc-20) - 2.64664e-6*Sqr(Tc-20) ); 
  double  term2 = 5e-4* Sqrt(1e6 + 4e3*(0.8347*C + (0.74 + C * 4.2e-4) * A
					+ 1.048*(S-C) + 0.8007*Cl*106/71 + 1.2517*SO4*106/96 + 1.35*TOS) );
  double  term3 = 1.0018 * pow( 10. , -3.36587 + 0.01136 * (S-C) );
  double  term4 = (C - 130) * 4e-5;
  double sum = (term1 + term2 + term3 + term4)*998.2;
  

  if (sum > 2000.) sum = 2000.0;
  
  return sum;
}

// New Brahma Density Equation...

double CBayerConcs::LiquorDensEqn6(double Tc,
				   double C, double S, double A, 
				   double Cl, double SO4, double TOC)
{

  const double term1 = 1.0149 + 7.0127E-4*C + 4.4411E-4*A;
  const double term2 = 7.4899E-4 * (S-C);
  const double term3 = 7.800E-4  * SO4 * 142/96 + 9.576E-4 * Cl * 58/35.5;
  const double term4 = 1.6847E-3 * TOC-3.0501E-4 * (Tc - 25) - 2.877E-6 * Sqr(Tc - 25);
  double sum = (term1 + term2 + term3 + term4)*1000;
  

  if (sum > 2000.) sum = 2000.0;
  
  return sum;
}
