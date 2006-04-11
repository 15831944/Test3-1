


/************************************


Implemented from the BRAHMA model Pascal CODE



Function ACequilf(     Density : prec;
                       Temp    : prec;
                       LiqCompK: LiquorCompositionTypeK;
                   Var CEquil  : prec): prec;

const
  T0 = 273.15;

Var
  WH2O, Mtot, Molality, MolalityOld, BPR, H2OAct, LnKeq : prec;
  Keq, ACin, ACuit, DeltaAk, WeightH2ONew : prec;
  C_Na, Factor : prec;
  LoopCounter  : integer;


begin
  With LiqCompK do ACin := Ak / Ck;

  WH2O := WeightH2O( Mtot, LiqCompK );
  Molality := 1000*Mtot/WH2O;

  LoopCounter:=0;
  repeat
    MolalityOld := Molality;

    BPR :=  0.00182 + 0.55379 * PowI(molality/10,7)
          + 0.0040625 * Molality * (Temp+T0)
          + (1/(temp+T0)) * (-286.66*Molality + 29.919 * PowI(Molality,2)
             + 0.6228 * PowI(Molality,3))
          - 0.032647 * Molality * PowI(Molality*(temp+T0)/1000,2)
          + PowI((temp+T0)/1000,5) * ( 5.90705* Molality - 0.57532 *
              PowI(Molality,2) + 0.10417 * PowI(Molality,3));

    H2OAct := Exp( 10519/1.986 * ( 1/(T0+100+BPR) - 1/(100+T0) ) );

    LnKeq := -(1/8.31051) * ( PDat.KEq_A1 / (temp+T0)
                            + PDat.KEq_B1 * ( (temp-25)/(temp+T0) - Ln((temp+T0)/298.15))
                            + PDat.KEq_C1 * (1 + 0.00536 * Molality ));
    Keq := SQRT( Exp( LnKeq ) );

    ACuit := 1 / ( 1 + Keq * PowI(H2OAct,2) ) * 102/106;
    DeltaAk := LiqCompK.Ck * ( ACin - ACuit );
    WeightH2Onew := WH2O - ( DeltaAk/102 * 72 );
    Molality := 1000 * Mtot / WeightH2ONew;
    inc(LoopCounter);
  until (Abs( 1 - Molality/MolalityOld ) < 1e-5) or (LoopCounter>100);

  ACEquilf := ACuit;
end;


*/


double ACin = AtoC();

MArray MA(this);


double KEq_A1 = -64149; 
double KEq_B1 = 69.92;  
double KEq_C1 = 166.465;

double WH20, Mtot;
double  molality = Molality(MA, WH20, Mtot);
double ACUit;
for (int i=0; i<100; i++) {
  double molalityOld = molality;

  double BPR = BoilPtElev(MA,  T_);
  double H2OAct = exp( 10519/1.986 * ( 1/C2K(100+BPR) - 1/C2K(100) ) );
  
  double LnKeq = -(1/8.31051) * ( KEq_A1 / T_
				  + KEq_B1 * ( (K2C(T_)-25)/T_ - Ln(T_/298.15))
				  + KEq_C1 * (1 + 0.00536 * molality ));
  double Keq = sqrt( exp( LnKeq ) );
  
  ACUit = 1 / ( 1 + Keq * Sqr(H2OAct) ) * 102/106;
  double DeltaAk = /* Ck * */ ( ACin - ACUit );
  double WeightH2ONew = WH20 - ( DeltaAk/102 * 72 );
  molality = 1000 * Mtot / WeightH2ONew;
  if (fabs( 1 - molality/molalityOld ) < 1e-5) break;
}


double C = CausticConc(T_);
if (C<1.0e-12)
  return 0.0;
const double ASat = ACUit * C;
return ASat;


