//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
/ $Nokeywords: $
//===========================================================================
#include "stdafx.h"
#include "SKOSpMdl.h"
#include "math.h"
#include "md_headers.h"
#include <complex> 
using namespace std;

#pragma comment(lib, "rpcrt4.lib")
#pragma optimize ("",off)


//===========================================================================
//
//
//
//===========================================================================

// The Species
//SiO2(s)
//CH3OH(l)
//N2(l)
//H2O(l) 
//SiH4-30(g)
//SiH4-29(g)
//SiH4-28(g)
//O2(g)
//N2(g)
//H2(g)
//Air(g)
//H2O(g)
//

static MInitialiseTest InitTest("CSKOSMClass");

//MSpeciePtr   ???                (InitTest, "SiO2(s)",        false);
//MSpeciePtr   ???                (InitTest, "CH3OH(l)",       false);
//MSpeciePtr   ???                (InitTest, "N2(l)",          false);
MSpeciePtr   Water              (InitTest, "H2O(l)",         false);
MSpeciePtr   Silane30           (InitTest, "SiH4-30(g)",     false);
MSpeciePtr   Silane29           (InitTest, "SiH4-29(g)",     false);
MSpeciePtr   Silane28           (InitTest, "SiH4-28(g)",     false); //organic
MSpeciePtr   Silane30l          (InitTest, "SiH4-30(l)",     false);
MSpeciePtr   Silane29l          (InitTest, "SiH4-29(l)",     false);
MSpeciePtr   Silane28l          (InitTest, "SiH4-28(l)",     false); //organic
MSpeciePtr   Oxygen             (InitTest, "O2(g)",          false); //organic
MSpeciePtr   Nitrogen           (InitTest, "N2(g)",          false);
MSpeciePtr   Hydrogen           (InitTest, "H2(g)",          false);
MSpeciePtr   Air                (InitTest, "Air(g)",         false);
MSpeciePtr   Steam              (InitTest, "H2O(g)",         false);
MSpeciePtr   Methane            (InitTest, "CH4(g)",         false);
MSpeciePtr   Methanel           (InitTest, "CH4(g)",         false);



DEFINE_SPECIEMODEL(CSKOSpMdl)
IMPLEMENT_SPECIEMODEL(CSKOSpMdl, "SKOSM", "*SKO", "SKO User Specie Model", DLL_GroupName)

CSKOSpMdl::CSKOSpMdl(TaggedObject *pNd)
  {
  MInitialise(); // this must be called to initialise the SMDK library if neccessary
  //m_dDummyPropertyF=456;
  //m_dDummyPropertyS=567;
  }

//---------------------------------------------------------------------------

CSKOSpMdl::~CSKOSpMdl()
  {
  }

//---------------------------------------------------------------------------

bool CSKOSpMdl::get_IsBaseClassOf(LPCTSTR OtherProgID)
  {
  return false;
  //return S_OK;
  }

//---------------------------------------------------------------------------

LPCTSTR CSKOSpMdl::get_PreferredModelProgID()
  {
  return NULL;
  }

//---------------------------------------------------------------------------

double CSKOSpMdl::get_Density(long Phases, double T, double P, MArray *pMA) 
  {   
  // Result in Ns/m^2 = Pa.s
  MArray & MA = pMA ? *pMA : MassArray;

  bool InRange = true;
  if (!InRange) 
    return MSpModelBase::get_Density(Phases, T, P, pMA);

  double TotalRhoVap = 0.0, TotalRhoLiq = 0.0, TotalRho;
  if (Phases & MP_Gas)
    {
    // Loop Thru Gasses
        double TotalMVap = 0.0; 
    double rho;
    for (int s=0; s<gs_MVDefn.Count(); s++)
      {
      if (gs_MVDefn[s].IsGas())
	  {
		double MW = gs_MVDefn[s].MolecularWt();
        TotalMVap += MA[s]/MW;  // Calculate total moles of vapour
      }
	  }
    if (TstGTZ(TotalMVap))
      {
      for (int s=0; s<gs_MVDefn.Count(); s++)
        {
        if (gs_MVDefn[s].IsGas())
          {
			  double R = 8.314; 
			  double MW = gs_MVDefn[s].MolecularWt();
			  double MF = MA[s]/MW/TotalMVap;     // Calculate mole fraction of component s in vapour
			  double w = gs_MVDefn[s].Acentricity() ;
			  double Tc = gs_MVDefn[s].TCritical() ;     
			  double Pc = gs_MVDefn[s].PCritical() ;	                             	
			  double Tr = T/Tc ;
			  double Pr = P/Pc ; 
			  double m, alfa, a, b, A, B;
			  double a1, a2, a3;
			  double Q, D, RR, z;
			  double getal, z2a, z2b, z2c, z2d, z3a, z3b, z3c, z3d;
			  complex <double> S; 
			  complex <double> TT;
			  complex <double> z1;
			  complex <double> z21edeel, z22edeel, z31edeel, z32edeel;

			  m = 0.37464 + 1.54226*w - 0.26992*pow(w,2);
			  alfa = pow(1 + m*(1 - pow(Tr,0.5)),2);
			  a = 0.45724*pow((R*Tc),2)/(Pc*1000)*alfa;
			  b = 0.0778*R*Tc/(Pc*1000);
			  A = a*P*1000/pow((R*T),2);
			  B = b*P*1000/(R*T);
			  a1 = B-1;
			  a2 = A-3*pow(B,2)-2*B;
			  a3 = -A*B + pow(B,2)+ pow(B,3);
			  Q = (3*a2 - pow(a1,2))/9;
			  RR = (9*a1*a2 - 27*a3 - 2*pow(a1,3))/54;
			  D = pow(Q,3) + pow(RR,2);

			  if (D > 0){
				  complex <double> QRterm(pow(abs(D),0.5),0);	
				  S = pow((RR + QRterm),0.3333333333);
				  TT = pow((RR - QRterm),0.333333333);
				  z1 = S + TT - a1/3;
				  z = abs(z1);
			  }  // end if (D > 0)

			  else {
				  complex <double> QRterm(0,pow(abs(D),0.5));
				  S = pow((RR + QRterm),0.3333333333);
				  TT = pow((RR - QRterm),0.333333333);
				  z1 = S + TT - a1/3;
				  z21edeel = -0.5*(S + TT) - a1/3;
				  getal = pow(3.0,0.5)/2;
				  z22edeel = getal*(S - TT);
				  z2a = real(z21edeel);
				  z2b = imag(z21edeel);
				  z2c = real(z22edeel);
				  z2d = imag(z22edeel);
				  complex <double> z2((z2a-z2d),(z2b+z2c));
				  z31edeel = -0.5*(S + TT) - a1/3;
				  getal = -pow(3.0,0.5)/2;
				  z32edeel = getal*(S - TT);
				  z3a = real(z31edeel);
				  z3b = imag(z31edeel);
				  z3c = real(z32edeel);
				  z3d = imag(z32edeel);
				  complex <double> z3((z3a-z3d),(z3b+z3c));
				  double z1abs = abs(z1);
				  double z2abs = abs(z2);
				  double z3abs = abs(z3);

				  if (D < 0) {

					  if (z1abs > z2abs)
						  z = z1abs;
					  else z = z2abs;
					  if (z < z3abs)
						  z = z3abs;
				  }	// end if (D < 0)

				  else {

					  if (z1abs = z2abs)
						  z = z1abs;
					  else if (z1abs = z3abs)
						  z = z1abs;
					  else if (z2abs = z3abs)
						  z = z2abs;
				  } // end else

			  }   // end else (D <=0)


			  if (z < 0)
			  {
				  Log.Message(MMsg_Error, "%s", "Silane Density error"); 
				  z = 1;
			  }

			  rho = P*MW/R/T/z;

            
          TotalRhoVap += MF*rho;
          }  // end if gas
        } //  end for loop
      } // end if (TstGTZ(TotalMVap))
    }  // end if (phases & MP_gas)

  if (Phases & MP_SL)
    {
    // Loop Thru Solids & Liquids
    double TotalMassLiq = 0.0; 
    double rho, Mass, Vol;
    for (int s=0; s<gs_MVDefn.Count(); s++)
    {
		if (gs_MVDefn[s].IsLiquid())
        TotalMassLiq += MA[s];
	}
    if (TstGTZ(TotalMassLiq))
      {
      for (int s=0; s<gs_MVDefn.Count(); s++)
        {
        if (gs_MVDefn[s].IsLiquid())
          { 
          double MF = MA[s]/TotalMassLiq;	
          if (s==Silane28l || s==Silane29l || s==Silane30l)
            rho = -0.000002*pow(T,4) + 0.001*pow(T,3) - 0.2355*pow(T,2) + 22.128*T - 3.0755	;
          else 
            { 
            Mass = MA[s];
            Vol = MA[s]*gs_MVDefn[s].msVolume(T, P);
            rho = Mass/GTZ(Vol);
            }
          TotalRhoLiq += MF*rho;
          } // end if is liquid
        } // end for loop
      }  // end if (TstGTZ(TotalMassLiq))
    }  // end if (phases & MP_Liq) 
  
  double GasMF=MA.MassFrac(MP_Gas);
  double LiqMF=MA.MassFrac(MP_Liq);
  TotalRho = GasMF*TotalRhoVap + LiqMF*TotalRhoLiq;
  return Max(1e-3, TotalRho); 
  }   // end function get density

//---------------------------------------------------------------------------

double CSKOSpMdl::get_msEnthalpy(long Phases, double T, double P, MArray *pMA)
  {
  return MSpModelBase::get_msEnthalpy(Phases, T, P, pMA);
  }

//---------------------------------------------------------------------------

double CSKOSpMdl::get_msEntropy(long Phases, double T, double P, MArray *pMA)
  {
  return MSpModelBase::get_msEntropy(Phases, T, P, pMA);
  }

//---------------------------------------------------------------------------

double CSKOSpMdl::get_msCp(long Phases, double T, double P, MArray *pMA)
  {
  return MSpModelBase::get_msCp(Phases, T, P, pMA);
  }

//---------------------------------------------------------------------------

double CSKOSpMdl::get_SaturationT(double P, MArray *pMA)
  {
  return MSpModelBase::get_SaturationT(P, pMA);
  }

//---------------------------------------------------------------------------

double CSKOSpMdl::get_SaturationP(double T, MArray *pMA)
  {
  return MSpModelBase::get_SaturationP(T, pMA);
  }

//---------------------------------------------------------------------------

double CSKOSpMdl::get_DynamicViscosity(long Phases, double T, double P, MArray *pMA)
  {
  // Result in Ns/m^2 = Pa.s
  MArray & MA = pMA ? *pMA : MassArray;
  MArray mu, MF, MW;

  bool InRange = true;
  if (!InRange) 
    return MSpModelBase::get_DynamicViscosity(Phases, T, P, pMA);

  double TotalMuVap = 0.0, TotalMuLiq = 0.0; // Result in Ns/m^2 = Pa.s
  double Tc=K2C(T), Visc;

  if (Phases & MP_Gas)
    {
    double TotalMass = 0, parameter = 0, mumix = 0;
    double Q, Qtop, Qbot, fact, fact2;
    int count = 0;


    // Loop Thru Gasses
    for (int s=0; s<gs_MVDefn.Count(); s++)
      if (gs_MVDefn[s].IsGas())
        TotalMass += MA[s];	// Calculate total mass of gas

    if (TstGTZ(TotalMass))
      {
      for (int s=0; s<gs_MVDefn.Count(); s++)
        {
        if (gs_MVDefn[s].IsGas())
          {
          MF[s] = MA[s]/TotalMass;
          MW[s] = gs_MVDefn[s].MolecularWt();
          if (s==Silane28 || s==Silane29 || s==Silane30)
            //mu[s] = 0.000004*Tc + 0.0011;   // Viscosity in mPa.s or cP
            mu[s] =  (3.5117*Tc + 1065.1)/100000;
          else if (s ==Methane)
            mu[s] =0.004;
          else if (s==Nitrogen)
            mu[s] = (4E-07*Tc + 0.0002)*1000;
          else if (s==Hydrogen)
            mu[s] = (2.0008*Tc+841.14)/100000;
          else if (Tc < 250)
            mu[s] = 0.00003 * Tc + 0.0089;
          else if (Tc >= 250 && Tc < 310)
            mu[s]= 0.00004 * Tc + 0.0062;
          else if (Tc >= 310)
            mu[s]= 0.000003 * Sqr(Tc) - 0.0017 * Tc + 0.2823;
          mu[s] *= 0.001;
          }
        }

      for (int i=0; i<gs_MVDefn.Count(); i++)// use Perry method to calculate mixture viscosity
        {
        if (gs_MVDefn[i].IsGas() && MF[i]>1e-30)
          { 
          for (int j=0; j<gs_MVDefn.Count(); j++)
            {	  
            if (gs_MVDefn[j].IsGas() && MF[j]>1e-30)
              { 
              if (j != i)
                {
                fact = pow((mu[i]/mu[j]),0.5)*pow((MW[j]/MW[i]),0.25);	
                Qtop = 1 + pow(fact,2);
                fact2 =1+MW[i]/MW[j];
                Qbot = 2.82842712474619*pow(fact2,0.5);
                Q = Qtop/Qbot;
                parameter += Q*MF[j]/MF[i];
                }
              else break;
              }
            }
          TotalMuVap += mu[i]/(1+parameter);  
          }
        }  
      }
    }

  if (Phases & MP_SL)
    { 
    double TotalMass = 0, count = 0;

    // Loop Thru Liquids
    for (int s=0; s<gs_MVDefn.Count(); s++)
      if (gs_MVDefn[s].IsLiquid())
        TotalMass += MA[s];

    if (TstGTZ(TotalMass))
      {
      for (int s=0; s<gs_MVDefn.Count(); s++)
        {
        if (gs_MVDefn[s].IsLiquid())
          { 
          MF[s] = MA[s]/TotalMass;
          MW[s] = gs_MVDefn[s].MolecularWt();
          if (s==Silane28l || s==Silane29l || s==Silane30l)
            mu[s] = 0.0000007*exp(-0.018*Tc);
          else if (s==Methanel)
            mu[s] = 0.15;
          else if (Tc > 0.01 && Tc < 100)
            mu[s] = 1.4851 * Exps(-0.018 * Tc);
          else if (Tc >= 100 && Tc < 200)
            mu[s] = 0.5629 * Exps(-0.0073 * Tc);
          else if (Tc >= 200 && Tc < 370)
            mu[s] = -0.0004 * Tc + 0.2188; 
          else
            mu[s] = 1; // Hard Wired;    
          mu[s]*=0.001;	
          TotalMuLiq += MF[s]*mu[s];    // use mass weighted mean 
          }
        }
      }
    }
  double GasMF=MA.MassFrac(MP_Gas);
  double LiqMF=MA.MassFrac(MP_Liq);
  Visc = GasMF*TotalMuVap+LiqMF*TotalMuLiq;

  if (0)
    {
    Dbg.PrintLn("==============================");
    for (int s=0; s<gs_MVDefn.Count(); s++)
      Dbg.PrintLn("mu[%3i] = %12.4e %7.3f%%", s, mu[s], MF[s]*100);
    Dbg.PrintLn("Gas    %7.3f%% %12.4e", GasMF*100, TotalMuVap);
    Dbg.PrintLn("Sl     %7.3f%% %12.4e", LiqMF*100, TotalMuLiq);
    Dbg.PrintLn("Visc =          %12.4e", Visc);
    }

  return GTZ(Visc);
  //...
  }

//---------------------------------------------------------------------------

double CSKOSpMdl::get_ThermalConductivity(long Phases, double T, double P, MArray *pMA)
  {
  return MSpModelBase::get_ThermalConductivity(Phases, T, P, pMA);
  }

//===========================================================================
//
//
//
//===========================================================================

void CSKOSpMdl::BuildDataFields(void)
  {
  };

//---------------------------------------------------------------------------

bool CSKOSpMdl::ExchangeDataFields()
  {
  return false;
  };

//---------------------------------------------------------------------------

bool CSKOSpMdl::ValidateDataFields()
  { 
  //_asm int 3;
  bool OK=MSpModelBase::ValidateDataFields();
  //InputCalcs(fDoCalc, Temperature);
  return true; 
  }; 

//===========================================================================
//
//
//
//===========================================================================

long CSKOSpMdl::DefinedPropertyCount()
  {
  return MSpModelBase::DefinedPropertyCount() + 1;
  }

//---------------------------------------------------------------------------

long CSKOSpMdl::DefinedPropertyInfo(long Index, MPropertyInfo & Info)
  {
  long Inx=(Index-MSpModelBase::DefinedPropertyCount());
  switch (Inx)
    {
    case 0: 
      Info.SetStructName("Props"); //NB: The "struct name" is used as part of tag! Recommend "Props" for compatability with other properties models.
      Info.Set(ePT_Double,    "Abundance",       "", MC_,    "",       0,      0,  MP_HardMin,            "Abundance Ratio"); 
      return Inx;
      //case 1: 
      //  Info.Set(ePT_Double,    "Settable",    "", MC_,    "",       0,      0,  MP_HardMin|MP_Settable,   "A Desc"); 
      //  return Inx;
    }
  return -1;
  };

//---------------------------------------------------------------------------

DWORD CSKOSpMdl::GetPropertyVisibility(long Index)
  {
  switch (Index-MSpModelBase::DefinedPropertyCount())
    {
    case 0: return ePVis_All;
    default: return ePVis_All;
    }
  };

//---------------------------------------------------------------------------

void CSKOSpMdl::GetPropertyValue(long Index, ULONG Phase/*=MP_All*/, double T/*=NAN*/, double P/*=NAN*/, MPropertyValue & Value)
  {
  switch (Index-MSpModelBase::DefinedPropertyCount())
    {
    case   0: Value=-1.2345;  return ;
    case   1: Value=m_dDummyPropertyS;  return ;
    };
  };

//---------------------------------------------------------------------------

void CSKOSpMdl::PutPropertyValue(long Index, MPropertyValue & Value)
  {
  switch (Index-MSpModelBase::DefinedPropertyCount())
    {
    case  1: m_dDummyPropertyS=Value; return;
    };
  };

//---------------------------------------------------------------------------
