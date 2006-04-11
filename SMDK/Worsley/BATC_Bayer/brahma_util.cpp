
double BATCBayerSM::Molality(MArray & MA, double &WH20, double &Mtot) 
{
  double TLiq=MA.Mass(MP_Liq); // Total Liquid kg/s
  if (TLiq<1.0e-9)
    return 0.0;
  const double giAl2O3    = 1000 * MA[Alumina] / TLiq * 2.0 * 81.97 / ::Alumina.MW; // 81.97 = MW of NaAlO2
  const double giNaOH     = 1000 * MA[CausticSoda] / TLiq
    - (1000 * MA[Alumina] / TLiq * 2.0 * ::CausticSoda.MW/::Alumina.MW);
  const double giNa2CO3   = 1000 * MA[SodiumCarbonate] / TLiq;
  const double giNaCl     = 1000 * MA[SodiumChloride] / TLiq;
  const double giNa2SO4   = 1000 * MA[SodiumSulphate] / TLiq;
  const double giNaF      = 1000 * MA[SodiumFluoride] / TLiq;
  //const double giNa2C5O7  = 1000 * MA[Organics] / TLiq;
  const double giNaOrg    = 1000 * MA[Organics1] / TLiq;
  const double giNa2C2O4  = 1000 * MA[SodiumOxalate] / TLiq;
  
  const double giSum = giAl2O3 + giNaOH + giNa2CO3 + giNaCl + giNa2SO4 + giNaF + /*giNa2C5O7 + */giNaOrg + giNa2C2O4;
  
   WH20  = 1000.0 - giSum;
  
  //Gram moles per kg liq
  const double gmAl2O3    = giAl2O3 / 81.97; // 81.97 = MW of NaAlO2
  const double gmNaOH     = giNaOH / ::CausticSoda.MW;
  const double gmNa2CO3   = giNa2CO3 / ::SodiumCarbonate.MW;
  const double gmNaCl     = giNaCl / ::SodiumChloride.MW;
  const double gmNa2SO4   = giNa2SO4 / ::SodiumSulphate.MW;
  const double gmNaF      = giNaF / ::SodiumFluoride.MW;
  //const double gmNa2C5O7  = giNa2C5O7 / ::Organics.MW;
  const double gmNaOrg    = giNaOrg / ::Organics1.MW;
  const double gmNa2C2O4  = giNa2C2O4 / ::SodiumOxalate.MW;

  Mtot = gmAl2O3 + gmNaOH + gmNa2CO3 + gmNaCl + gmNa2SO4 + gmNaF + /*gmNa2C5O7 + */gmNaOrg + gmNa2C2O4;

  // Total gmoles/kg H2O = molality
  return 1000.0 * Mtot/Max(0.1, WH20);
}



