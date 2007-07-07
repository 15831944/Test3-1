//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
//    Murdoch Bayer Model Transcritical Technologies Pty Ltd Feb 05
//   Time-stamp: <2007-07-03 18:16:11 Rod Stephenson Transcritical Pty Ltd>
// Copyright (C) 2005 by Transcritical Technologies Pty Ltd and KWA
//===========================================================================
#include "stdafx.h"
#include "MurdochBayerSpMdlSM.h"
#include "math.h"
#include "md_headers.h"
#pragma comment(lib, "rpcrt4.lib")

//#pragma optimize("", off)

#if (1 && DBG_MVECTOR)
#define DUMPIT0(S, X)           DumpIt(S,X)
#define DUMPIT1(S, X, A)        DumpIt(S,X,A)
#define DUMPIT2(S, X, A, B)     DumpIt(S,X,A,B)
#define DUMPIT3(S, X, A, B, C)  DumpIt(S,X,A,B,C)
#else
#define DUMPIT0(S, X)           X
#define DUMPIT1(S, X, A)        X
#define DUMPIT2(S, X, A, B)     X
#define DUMPIT3(S, X, A, B, C)  X
#endif
//===========================================================================
//
//
//
//===========================================================================

DEFINE_SPECIEMODEL(MurdochBayerSpMdl)
IMPLEMENT_SPECIEMODEL(MurdochBayerSpMdl, "MurdochSM", "*MurdochBayer", "Murdoch Bayer Specie Model", DLL_GroupName)

//===========================================================================
// Specie Access
static MInitialiseTest InitTest("MurdochSMClass");





MSpeciePtr   Water             (InitTest, "H2O(l)",        false);
MSpeciePtr   Alumina           (InitTest, "NaAl[OH]4(l)",  false);
MSpeciePtr   CausticSoda       (InitTest, "NaOH(l)",       false);
MSpeciePtr   SodiumChloride    (InitTest, "NaCl(l)",       false);
//MSpeciePtr   SodiumFluoride    (InitTest, "NaF(l)",       false);
MSpeciePtr   SodiumSulphate    (InitTest, "Na2SO4(l)",     false);
MSpeciePtr   SodiumCarbonate   (InitTest, "Na2CO3(l)",     false);
MSpeciePtr   SodiumOxalate     (InitTest, "Na2C2O4(l)",    false); //organic
//MSpeciePtr   SodiumSilicate    (InitTest, "Na2SiO3(l)",    false); 
MSpeciePtr   Organics          (InitTest, "Na2C5.2O7.2(l)", false); //organic
//MSpeciePtr   OccSoda           (InitTest, "NaOH*(s)",       false);
MSpeciePtr   THA               (InitTest, "Al[OH]3(s)", false);
MSpeciePtr   AluminaSolid      (InitTest, "Al2O3(s)",      false);

MSpeciePtr   Steam             (InitTest, "H2O(g)",        true);

// ==========================================================================
// Specie Properties Modal Global Parameters


// ==========================================================================
//
// A helper class to assist with calculating iterative concentration and density
// calculations.
//
// ==========================================================================


//===========================================================================
//
//
//
//===========================================================================


bool MurdochBayerSpMdl::NaFactorOK = false;
MArrayI MurdochBayerSpMdl::NaFactor;




MurdochBayerSpMdl::MurdochBayerSpMdl(TaggedObject *pNd)
{
  MInitialise(); // this must be called to initialise the SMDK library if neccessary
  fDoSolubility = false;
  if (!NaFactorOK) {
    for (int sn=0; sn<gs_MVDefn.Count(); sn++) {
      bool hasSodium = false;
      double amount = 1.0;
      MSpecieElements theElements = gs_MVDefn[sn].Elements();
      long eCount = theElements.Count();
      for (int i=0; i<eCount; i++) {
	if (theElements[i].Element().AtomicNumber()==anSODIUM) { // Sodium
	  hasSodium = true;
	  amount = theElements[i].Amount();
	  break;
	}
      }
      if (!hasSodium) {
	NaFactor[sn] = 1.0;
	continue;
      }
      NaFactor[sn] = amount/2.*SodiumCarbonate.MW/gs_MVDefn[sn].MolecularWt();
    }
    NaFactorOK = true;
  }
}


  

//---------------------------------------------------------------------------

MurdochBayerSpMdl::~MurdochBayerSpMdl()
  {
  }

//---------------------------------------------------------------------------

bool MurdochBayerSpMdl::get_IsBaseClassOf(LPCTSTR OtherProgID)
  {
  return false;
  //return S_OK;
  }

//---------------------------------------------------------------------------

LPCTSTR MurdochBayerSpMdl::get_PreferredModelProgID()
  {
  return NULL;
  }

//---------------------------------------------------------------------------

bool MurdochBayerSpMdl::ValidateDataFields()
  {
  bool OK=MSpModelBase::ValidateDataFields();
  //LiquorComposition(fDoCalc);
  return OK;
  }

//---------------------------------------------------------------------------
double MurdochBayerSpMdl::getDLLVersion() 
{
  return version_();
}




//---------------------------------------------------------------------------

double MurdochBayerSpMdl::get_Density(long Phases, double T, double P, MArray *pMA)
  {
  MArray MA(pMA ? (*pMA) : this);

  const double MSol=(Phases & MP_Sol) ? MA.Mass(MP_Sol) : 0.0;
  const double MLiq=(Phases & MP_Liq) ? MA.Mass(MP_Liq) : 0.0;
  const double MGas=(Phases & MP_Gas) ? MA.Mass(MP_Gas) : 0.0;
  const double MTot=GTZ(MSol+MLiq+MGas);
  const double MSL = MSol+MLiq;
  if (MSL<UsableMass || MSL/MTot<1.0e-6)
    return MSpModelBase::get_Density(Phases, T, P, &MA);

  const double WaterFrac = (MLiq>1e-6 ? MA[Water]/MLiq : 0.0);

  const double FSol=MSol/MTot;
  const double FLiq=MLiq/MTot;
  double Dl=1.0;
  if (FLiq>1.0e-9)
    {
      double * dpd = CheckConverged(T, P, pMA);
      Dl = dpd[iRho_Liq]*1000.;
    }

  return DUMPIT0("Density", DensityMix(FSol, dNAN, FLiq, Dl, (1.0-FSol-FLiq), dNAN, T, P, MA));
  }

//---------------------------------------------------------------------------

double MurdochBayerSpMdl::LiqCpCalc(double T, MArray *MA ) {
  Log.Message(MMsg_Note, "Liq Cp Calculation");
  double *dpData = CheckConverged(T, -1.0, MA);
  return dpData[iCp_Liq];
}

double MurdochBayerSpMdl::LiqHCalc(double T, double P, MArray *pMA ) {
  Log.Message(MMsg_Note, "Liq H Calculation");
  double * dpData = CheckConverged(T, P, pMA);
  return  dpData[iH]-dpData0[iH];
}


double MurdochBayerSpMdl::LiqSCalc(double T,MArray *MA ) {
  //double * dpData = CheckConverged(T, P, MA);
  return  dpData[iS];
}



//---------------------------------------------------------------------------

double MurdochBayerSpMdl::get_msEnthalpy(long Phases, double T, double P, MArray *pMA)
  {
  MArray MA(pMA ? (*pMA) : this);

  const double MSol=(Phases & MP_Sol) ? MA.Mass(MP_Sol) : 0.0;
  const double MLiq=(Phases & MP_Liq) ? MA.Mass(MP_Liq) : 0.0;
  const double MGas=(Phases & MP_Gas) ? MA.Mass(MP_Gas) : 0.0;
  const double MTot=GTZ(MSol+MLiq+MGas);
  const double MSL = MSol+MLiq;
  if (MSL<UsableMass || MSL/MTot<1.0e-6)
    return MSpModelBase::get_msEnthalpy(Phases, T, P, &MA);

  const double WaterFrac = (MLiq>1e-6 ? MA[Water]/MLiq : 0.0);

  const double FSol=MSol/MTot;
  const double FLiq=MLiq/MTot;
  const double Tc=K_2_C(T);

  double Hl=0.0;
  if (FLiq>1.0e-9)
    {
      Hl = LiqHCalc(T, P, pMA) /*LiqCpCalc(MA, Tc) * Tc*/;
    }
  double Hs=(FSol>1.0e-9) ? gs_MVDefn.msHz(MP_Sol, T, P, PropOverides, &MA) : 0.0;
  return DUMPIT0("msEnthalpy", msEnthalpyMix(FSol, Hs, FLiq, Hl, (1.0-FSol-FLiq), dNAN, T, P, MA));
  }

//---------------------------------------------------------------------------

double MurdochBayerSpMdl::get_msEntropy(long Phases, double T, double P, MArray *pMA)
  {
    // double * dpd = CheckConverged(T, P, pMA);
    return dpData[iS];
  }

//---------------------------------------------------------------------------

double MurdochBayerSpMdl::get_msCp(long Phases, double T, double P, MArray *pMA)
  {
  MArray MA(pMA ? (*pMA) : this);

  const double MSol=(Phases & MP_Sol) ? MA.Mass(MP_Sol) : 0.0;
  const double MLiq=(Phases & MP_Liq) ? MA.Mass(MP_Liq) : 0.0;
  const double MGas=(Phases & MP_Gas) ? MA.Mass(MP_Gas) : 0.0;
  const double MTot=GTZ(MSol+MLiq+MGas);
  const double MSL = MSol+MLiq;
  if (MSL<UsableMass || MSL/MTot<1.0e-6)
    return MSpModelBase::get_msCp(Phases, T, P, &MA);

  const double WaterFrac = (MLiq>1e-6 ? MA[Water]/MLiq : 0.0);

  const double FSol=MSol/MTot;
  const double FLiq=MLiq/MTot;

  double Cpl=0.0;
  if (FLiq>1.0e-9)
    {
    Cpl = LiqCpCalc(T, pMA);
    }
  double Cps=2.2;
  return msCpMix(FSol, Cps, FLiq, Cpl, (1.0-FSol-FLiq), dNAN, T, P, MA);
  }

//---------------------------------------------------------------------------

double MurdochBayerSpMdl::get_SaturationT(double P, MArray *pMA)
  {
  MArray MA(pMA ? (*pMA) : this);

  // CheckConverged(-1.0, P, &MA);;

  double PureSatT = MSpModelBase::get_SaturationT(P, &MA);
  if (MA.Mass(MP_Liq)/GTZ(MA.Mass())<1.0e-6)
    return DUMPIT1("SaturationTPure", PureSatT, P);
  double *dpData = CheckConverged(this->Temperature, P, pMA);
  return PureSatT+dpData[iBPE];
  }

//---------------------------------------------------------------------------

double MurdochBayerSpMdl::get_SaturationP(double T, MArray *pMA)
{
  double *dpData = CheckConverged(T, this->Pressure, pMA);
  return dpData[iP_sat]*100.0;
}

//---------------------------------------------------------------------------

double MurdochBayerSpMdl::BoilPtElev(double P, MArray*pMA)
{
  Log.Message(MMsg_Warning, "Boiling POint Elevation, t  %8.2f p  %8.2f", this->Temperature, P);
  double * dpData = CheckConverged(this->Temperature, P, pMA);
  return dpData[iBPE];
  }

//---------------------------------------------------------------------------

double MurdochBayerSpMdl::get_DynamicViscosity(long Phases, double T, double P, MArray *pMA)
  {
  if (Phases & MP_Gas)
    return MSpModelBase::get_DynamicViscosity(Phases, T, P, pMA); //todo : check!

  MArray MA(pMA ? (*pMA) : this);


  double Tc = Max(-41.0, K_2_C(T));
  const double C = 200;
  const double A = 100.;

  return exp ( 0.58-1.816*log(Tc+42.0) + 
    (0.5*C + 0.43*A) / (Tc+42.0) );


  }

//---------------------------------------------------------------------------
//  Not implemented; use base case
double MurdochBayerSpMdl::get_ThermalConductivity(long Phases, double T, double P, MArray *pMA)
  {
  return MSpModelBase::get_ThermalConductivity(Phases, T, P, pMA);
  }

//===========================================================================
//
//
//
//===========================================================================
// The properties that are defined for visibility in the SysCAD Access properties window
// in addition, these properties are accessible in code by name.

enum 
  {
  idBPE_Factor,
  idRho_Factor,
  idMinSolCp,


  idDLLVersion,
  idSMDKVersion,
  idCalcSolubilities,
  idAluminaConc25,
  idCausticConc25,
  idSodaConc25,
  idAtoC,
  idCtoS,
  idCltoC,
  idCarbonateConc25,
  idSolidsConc25,
  idFreeCaustic25,
  idNaSO4Conc25,
  idNaClConc25,
  idNa2C2O4Conc25,

  idSeparator1,

  idOxalateConc25,
  idTotalOrganics25,
  idChlorineConc25,
  idSulphateConc25,
  idOrganateConc25,

  idSeparator2,

  idLVolume25,
  idSLVolume25,
  idLDensity25,
  idSLDensity25,

  idSeparator3,	
  // At temperature  
  idAluminaConc,
  idCausticConc,
  idSodaConc,
  idTOC,
  idSolidsConc,
  idSodiumCarbonateConc,

  idSeparator4,	


  //  idSeparator5,	

  idAluminaConcSat,
  idAtoCSaturation,
  idSSNRatio,
  idIonicStrength,
  idBoilPtElev,
  idTHAMassFlow,
  idTHADens,


  idMPI_EndOfProps 
  };

//---------------------------------------------------------------------------

long MurdochBayerSpMdl::DefinedPropertyCount()
  {
  return idMPI_EndOfProps + 1 + MSpModelBase::DefinedPropertyCount();
  }

//---------------------------------------------------------------------------

/*static MPropertyInfo::MStringValueP SVOrg[]={
//{"Organics and Ratio", DLM_TotOrganics},
{ "TOC and Oxalate", DLM_TOC},
{0}};*/


#define MP_GPH  MP_GlobalProp|MP_Parameter|MP_InitHidden
#define MP_CP   MP_ConfigProp|MP_Parameter
#define MP_RN   MP_Result|MP_NoFiling
#define MP_RNH  MP_Result|MP_NoFiling|MP_InitHidden

long MurdochBayerSpMdl::DefinedPropertyInfo(long Index, MPropertyInfo & Info)
  {//define a list of all properties: This sets what is displayed in the access window.
  long Inx=Index-MSpModelBase::DefinedPropertyCount();
  switch(Inx)
    {

    case idBPE_Factor   : Info.Set(ePT_Double,    "",  "BPE_Factor", MC_Frac, "%", 0,0,   MP_GPH, "BPE Factor"); return Inx;
    case idRho_Factor   : Info.Set(ePT_Double,    "",  "Rho_Factor", MC_Frac, "%", 0,0,   MP_GPH, "Rho Factor"); return Inx;
    case idMinSolCp   	: Info.Set(ePT_Double,    "", "MinSolCp",  MC_CpMs,  "kJ/kg.C", 0, 0,  MP_GPH, "Global minimum solids Cp"); return Inx;



    case idDLLVersion      :  Info.Set(ePT_Double,    "", "DLL.Version",  MC_,"",  0, 0,  MP_RN,    "DLL Version"); return Inx;
    case idSMDKVersion      : Info.Set(ePT_Double,   "", "SMDK.Version",  MC_,"",  0, 0,  MP_RN,    "SMDK Version"); return Inx;
    case idCalcSolubilities : 
      Info.Set(ePT_Bool,"", "Solubility",    MC_, "",    0, 0,    MP_CP|MP_CheckBox, ""); return Inx;

    case idAluminaConc25 : 
      Info.SetStructName("Props"); //NB: The "struct name" is used as part of tag! Recomend "Props" for compatability with other properties models.
      Info.SetText("MurdochBayer Liquor Values @ 25");
      Info.Set(ePT_Double,    "A", "AluminaConc",MC_Conc, "g/L",    0, 0,  MP_RN,    "Alumina Concentration @ 25"); return Inx;
    case idCausticConc25    : 

      Info.Set(ePT_Double,    "C", "CausticConc",   MC_Conc, "g/L",    0, 0,  MP_RN,    "Caustic Concentration @ 25"); return Inx;

    case idAtoC		    : Info.Set(ePT_Double,    "", "A/C",  MC_,"",  0, 0,  MP_RN,    "A to C ratio"); return Inx;
    case idCtoS		    : Info.Set(ePT_Double,    "", "C/S",  MC_,"",  0, 0,  MP_RN,    "C to S ratio"); return Inx;
    case idCltoC	    : Info.Set(ePT_Double,    "", "Cl/C", MC_,"",  0, 0,  MP_RN,    "Cl to C ratio"); return Inx;
    case idNaSO4Conc25	    : Info.Set(ePT_Double,    "", "Sulphate*@25",  MC_Conc, "g/L",    0, 0,  MP_RNH,    "SodiumSulphate Concentration @ 25"); return Inx;
    case idNaClConc25	    : Info.Set(ePT_Double,    "", "Chloride*@25",  MC_Conc, "g/L",    0, 0,  MP_RNH,    "SodiumChloride Concentration @ 25"); return Inx;
    case idNa2C2O4Conc25    : Info.Set(ePT_Double,    "", "Oxalate*@25",	   MC_Conc, "g/L",    0, 0,  MP_RNH,    "SodiumOxalate Concentration @ 25"); return Inx;
    case idFreeCaustic25    : Info.Set(ePT_Double,    "FC", "FreeCaustic", MC_Conc, "g/L",    0, 0,  MP_RNH,    "Free Caustic Concentration @ 25"); return Inx;
    case idSolidsConc25	    : Info.Set(ePT_Double,    "", "SolidsConc",  MC_Conc, "g/L",    0, 0,  MP_RN,    "Solids Concentration @ 25");return Inx;

    case idSeparator1	    : Info.SetText("..."); return Inx;

    case idSodaConc25:
      Info.Set(ePT_Double,    "S", "SodaConc",		MC_Conc, "g/L",    0, 0,  MP_RN,    "Soda Concentration @ 25"); return Inx;
    case idCarbonateConc25  : Info.Set(ePT_Double,    "", "SodiumCarbConc",	   MC_Conc, "g/L",    0, 0,  MP_RN,    "SodiumCarbonate Concentration @ 25"); return Inx;
    case idSulphateConc25   : Info.Set(ePT_Double,    "", "Na2SO4",   MC_Conc, "g/L",    0, 0,  MP_RN,    "SodiumSulphate Concentration @ 25"); return Inx;
    case idChlorineConc25   : Info.Set(ePT_Double,    "", "NaCl",   MC_Conc, "g/L",    0, 0,  MP_RNH,    "SodiumChloride Concentration @ 25"); return Inx;
    case idOxalateConc25    :       Info.SetText("--Concentrations @ 25, as Na2CO3 Equiv----");

      Info.Set(ePT_Double,    "", "Oxalate",   MC_Conc, "g/L",    0, 0,  MP_RN,    "SodiumOxalate Concentration @ 25"); return Inx;
    case idOrganateConc25   : Info.Set(ePT_Double,    "", "Organate@25",	   MC_Conc, "g/L",    0, 0,  MP_RNH,    "Organate Concentration @ 25"); return Inx;
    case idTotalOrganics25  : Info.Set(ePT_Double,    "", "TOOC",    MC_Conc, "g/L",    0, 0,  MP_RN,    "Total Organic Concentration @ 25"); return Inx;

    case idSeparator2  	  : Info.SetText("..."); return Inx;

    case idLVolume25	  : Info.SetText("MurdochBayer Liquor Values @25---");
      Info.Set(ePT_Double,    "", "LVolFlow25",MC_Qv,   "L/s",    0, 0,  MP_RN,    "Liquor Volumetric flowrate @ 25"); return Inx;
    case idSLVolume25	  : Info.Set(ePT_Double,    "", "SLVolFlow25",    MC_Qv,   "L/s",    0, 0,  MP_RN,    "Slurry Volumetric flowrate @ 25"); return Inx;
    case idLDensity25     : Info.Set(ePT_Double,    "", "LRho25",    MC_Rho,  "g/L",    0, 0,  MP_RN,    "Liquor Density @ 25"); return Inx;
    case idSLDensity25    : Info.Set(ePT_Double,    "", "SLRho25",   MC_Rho,  "g/L",    0, 0,  MP_RN,    "Slurry Density @ 25"); return Inx;


    case idSeparator3	  : Info.SetText("..."); return Inx;

    case idAluminaConc	  : Info.SetText("MurdochBayer Liquor Values @ Temp");
      Info.Set(ePT_Double,    "A@T", "AluminaConcT",   MC_Conc, "g/L",    0, 0,  MP_RN,    "Alumina Concentration @ T"); return Inx;
    case idSodiumCarbonateConc : Info.Set(ePT_Double,  "C@T", "Carbonate@T",   MC_Conc, "g/L",    0, 0,  MP_RNH,    "SodiumCarbonate Concentration @ T"); return Inx;
    case idTOC	          : Info.Set(ePT_Double,    "", "TOC@T",MC_Conc, "g/L",    0, 0,  MP_RN,  "Total Organic Carbon Concentration @ T"); return Inx;
    case idSolidsConc	  : Info.Set(ePT_Double,    "", "SolidsConcT",   MC_Conc, "g/L",    0, 0,  MP_RN,    "Solids Concentration @ T"); return Inx;

    case idSeparator4	  : Info.SetText("..."); return Inx;

    case idCausticConc	  : 
      Info.Set(ePT_Double,    "", "CausticConcT", MC_Conc, "g/L",    0, 0,  MP_RN,    "Caustic Concentration @ T"); return Inx;
    case idSodaConc  : Info.Set(ePT_Double,    "S@T", "SodaConcT",    MC_Conc, "g/L",    0, 0,  MP_RN,    "Soda Concentration @ T"); return Inx;

      //    case idSeparator5	  : Info.SetText("..."); return Inx;

    case idBoilPtElev     : 
      Info.Set(ePT_Double,    "", "BoilPtElev",MC_dT,   "C", 0, 0,  MP_RN,  "Boiling Point Elevation"); return Inx;

    case idIonicStrength  : Info.Set(ePT_Double,    "I", "IonicStrength", MC_, "",    0, 0,  MP_RN,  "Ionic Strength"); return Inx;



    case idAluminaConcSat : Info.SetText("--Other MurdochBayer Liquor Properties @ T-");
      Info.Set(ePT_Double,    "ASat", "A_Saturation", MC_Conc, "g/L",    0, 0,  MP_RN,  "Alumina Saturation Concentration @ T"); return Inx;
    case idAtoCSaturation : Info.Set(ePT_Double,    "A/CSat", "A/C_Saturation", MC_,"",  0, 0,  MP_RNH,  "Alumina Saturation to Caustic ratio @ T"); return Inx;
    case idSSNRatio       : Info.Set(ePT_Double,    "", "SSN_Ratio",  MC_,"",  0, 0,  MP_RN,  "A/C actual to ASat/C ratio @ T"); return Inx;
    case idTHAMassFlow    : Info.Set(ePT_Double,    "", "THAMassFlow",    MC_Qm,   "kg/s",   0, 0,  MP_RN,    "THA flow rate"); return Inx;
    case idTHADens        : Info.Set(ePT_Double,    "", "THADens",   MC_Rho,  "kg/m^3", 0, 0,  MP_RNH,    "THA Density"); return Inx;


    case idMPI_EndOfProps : return MPI_EndOfProps;    

    default: 
      {
      char Buff[16];
      sprintf(Buff, "Unused%d", Inx);
      Info.Set(ePT_Double, "", Buff, MC_, "", 0, 0, MP_Null, ""); 
      return Inx;
      }
    }
  return MPI_MoreProps;
  }

//---------------------------------------------------------------------------

DWORD MurdochBayerSpMdl::GetPropertyVisibility(long Index)
  {//determine visibility of list of all properties
  switch (Index-MSpModelBase::DefinedPropertyCount())
    {
    //case idDefnLiqMethod: 
    default:  return ePVis_All; 
    }
  return ePVis_All;
  }

//---------------------------------------------------------------------------
// Stringifying operators GV...  Get Value
// Just tidy up the case structure.

#define GV(x, type) case id##x :Value = type##x; return 
#define GVAL(x)   case id##x : Value = d##x; return
#define GVAL2(v, x) case id##v : /*CheckConverged();*/ Value = x; return
#define GVALF(x) case id##x : Value = x##(); return
#define GVALFT(x) case id##x : Value = x##(T); return

void MurdochBayerSpMdl::GetPropertyValue(long Index, ULONG Phase/*=MP_All*/, double T/*=NAN*/, double P/*=NAN*/, MPropertyValue & Value)
  {//define method of retrieving values for list of all properties
  switch (Index-MSpModelBase::DefinedPropertyCount())
    {


    case idDLLVersion: Value=getDLLVersion(); return;
    case idSMDKVersion: Value= 1.01; return;
      GVAL2(CalcSolubilities, fDoSolubility); 
    GVALF(THAMassFlow);
    GVALFT(THADens);
    GVALFT(CausticConc);
    GVALFT(AluminaConc);
    GVALFT(SodaConc);
    GVALFT(SodiumCarbonateConc);

    GVAL2(CausticConc25, dpData[iTC]);
    GVAL2(AluminaConc25, dpData[iAl2O3]);
    GVAL2(SodaConc25, dpData[iTA]);
    GVAL2(CarbonateConc25, 104.);
    GVAL2(NaClConc25, 105.);
    GVAL2(NaSO4Conc25, 106.);
    GVAL2(Na2C2O4Conc25,107.);
    GVALFT(SolidsConc);
    GVALF(SolidsConc25);
    GVAL2(FreeCaustic25,FreeCaustic(C_2_K(25.0)));
    GVALFT(TOC);


    GVALF(AtoC);
    GVALF(CtoS);
    GVALF(CltoC);
    
    case idBoilPtElev	: Value=BoilPtElev(P, NULL);        return; 
      
      GVALF(OrganateConc25);
      GVALF(OxalateConc25);
      GVALF(TotalOrganics25);
      GVALF(ChlorineConc25);
      GVALF(SulphateConc25);
      GVALFT(AluminaConcSat);
      GVALF(IonicStrength);
      GVALFT(AtoCSaturation);
      GVALFT(SSNRatio);
      GVALF(LVolume25);
      GVALF(SLVolume25);
      GVALF(LDensity25);
      GVALF(SLDensity25);

    default: Value=0.0; return;
    }
  }

//---------------------------------------------------------------------------
// Stringifying operators  Put Value....
#define PV(x, type) case id##x : type##x = Value; return 
#define PVAL(x)   case id##x : d##x = Value; return


void MurdochBayerSpMdl::PutPropertyValue(long Index, MPropertyValue & Value)
  {//define method of setting values for list of all properties
  switch (Index-MSpModelBase::DefinedPropertyCount())
    {
    case idCalcSolubilities	: fDoSolubility=Value;     return;
    default:
      break;
    }
  }




// Digester A/C Equilibrium






// 
double MurdochBayerSpMdl::DRatio(double T_) 
{
  return dpData[iRho_Liq]*1000./(dpData[iWT]/GTZ(dpData[iV25]));    
}

//---------------------------------------------------------------------------

double MurdochBayerSpMdl::CausticConc(double T_)
{
  double *dpData = CheckConverged(T_);
  return dpData[iTC]*DRatio(T_);
}



//---------------------------------------------------------------------------

double MurdochBayerSpMdl::AluminaConc(double T_)
{
  double *dpData = CheckConverged(T_);
  return dpData[iAl2O3]*DRatio(T_);
}

//---------------------------------------------------------------------------

double MurdochBayerSpMdl::SodaConc(double T_)
{
  double *dpData = CheckConverged(T_);
  return dpData[iTA]*DRatio(T_);
}

//---------------------------------------------------------------------------

double MurdochBayerSpMdl::SodiumCarbonateConc(double T_)
{
  double *dpData = CheckConverged(T_);  
  return Comp_gL[iNa2CO3]*DRatio(T_);
}

//---------------------------------------------------------------------------

double MurdochBayerSpMdl::SodiumSulphateConc(double T_)
{
  double *dpData = CheckConverged(T_);
  return Comp_gL[iNa2SO4]*DRatio(T_);
}

//---------------------------------------------------------------------------

double MurdochBayerSpMdl::SodiumOxalateConc(double T_)
{
  double *dpData = CheckConverged(T_);
  return Comp_gL[iOxalate]*DRatio(T_);
}

//---------------------------------------------------------------------------

double MurdochBayerSpMdl::SolidsConc(double T_)
{
  const double mt=Mass(MP_Sol);
  //  Log.Message(MMsg_Warning, "SolidsConc");
  return (mt>=UsableMass) ? mt/GTZ(Mass(MP_SL))*get_Density(MP_SL, T_, Pressure, NULL) : 0.0;
}

double MurdochBayerSpMdl::SolidsConc25()
{
  const double mt=Mass(MP_Sol);
  // Log.Message(MMsg_Warning, "SolidsConc25");
  return (mt>=UsableMass) ? mt/GTZ(Mass(MP_SL))*get_Density(MP_SL, C_2_K(25.0), Pressure, NULL) : 0.0;
}



double MurdochBayerSpMdl::TOC(double T_)
{
  //CheckConverged();

  double Toc = 100.;
  return Max(0.0, Toc*DRatio(T_));
}

//---------------------------------------------------------------------------



double MurdochBayerSpMdl::FreeCaustic(double T_)
{
  return CausticConc(T_) * (1.0 - AtoC()*::SodiumCarbonate.MW/::AluminaSolid.MW);
}

//---------------------------------------------------------------------------



double MurdochBayerSpMdl::AtoC()
{
  CheckConverged();
  if (dpData[iTC]==0.0)
    return 0.0;
  return dpData[iAl2O3]/dpData[iTC];
}



//---------------------------------------------------------------------------

double MurdochBayerSpMdl::CtoS()
{
  CheckConverged();
  if (dpData[iTA]==0.0)
    return 0.0;
  return dpData[iTC]/dpData[iTA];
}

//---------------------------------------------------------------------------

double MurdochBayerSpMdl::CltoC()
{
  CheckConverged();
  if (dpData[iTC]==0.0)
    return 0.0;
  return Comp_gL[iNaCl]/dpData[iTC];
}

//---------------------------------------------------------------------------

double MurdochBayerSpMdl::Na2SO4toC()
{
  CheckConverged();
  if (dpData[iTC]==0.0)
    return 0.0;
  return Comp_gL[iNa2SO4]/dpData[iTC];  
}

//---------------------------------------------------------------------------

double MurdochBayerSpMdl::THAMassFlow()
{
  MArray M(this);
  return M[::THA];
}

//---------------------------------------------------------------------------

double MurdochBayerSpMdl::THADens(double T_)
{
  return gs_MVDefn[::THA].Density(T_, StdP, NULL);
}

//---------------------------------------------------------------------------

double MurdochBayerSpMdl::IonicStrength()
  {
    //CheckConverged();
  return 5.90;
  }

//---------------------------------------------------------------------------

double MurdochBayerSpMdl::AluminaConcSat(double T_)
{
  Bayer(T_, this->Pressure, MArray(this), dpData, true);
  return Solmkg[iSGibbsite];
}

//---------------------------------------------------------------------------


double MurdochBayerSpMdl::LVolume25()
  {
  const double mt=Mass(MP_Liq);
  double lv = (mt / LDensity25());
  Log.Message(MMsg_Warning, "LVolume25: mt %8.2f lv %8.2f", mt, lv);
  return ((mt>=UsableMass) ? lv : 0.0);
  }

double MurdochBayerSpMdl::SLVolume25()
  {
  const double mt=Mass(MP_SL);
  //Log.Message(MMsg_Warning, "SLVolume25");
  return ((mt>=UsableMass) ? (mt / get_Density(MP_SL, C_2_K(25.0), Pressure, NULL)) : 0.0);
  }

//---------------------------------------------------------------------------

double MurdochBayerSpMdl::LDensity25()
  {
    //Log.Message(MMsg_Warning, "LDensity25");
    double * dpData = CheckConverged();
  return dpData[iWT]/GTZ(dpData[iV25]);
  }

double MurdochBayerSpMdl::SLDensity25()
  {
    //Log.Message(MMsg_Warning, "SLDensity25");
  return (get_Density(MP_SL, C_2_K(25.0), Pressure, NULL));
  }

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

double MurdochBayerSpMdl::AtoCSaturation(double T_)
  {
  double C = CausticConc(T_);
  if (C<1.0e-12)
    return 0.0;
  const double ACsat = AluminaConcSat(T_) / GTZ(C);
  return ACsat;
  }

//---------------------------------------------------------------------------



double MurdochBayerSpMdl::SSNRatio(double T_)
  {   // AtoC actual / AtoC saturation ==> A/ASat
    return AtoCSaturation(T_)/GTZ(AtoC());
  }

//---------------------------------------------------------------------------



/* double MurdochBayerSpMdl::Na2CO3toS()
  {
    //CheckConverged();

return 2.0;
} */

//---------------------------------------------------------------------------

double MurdochBayerSpMdl::OrganateConc25()
{//Organic Na2C5O7 + NaOrg Conc @ 25
  CheckConverged();  
  return Comp_gL[iFormate]+Comp_gL[iAcetate];
}

//---------------------------------------------------------------------------

double MurdochBayerSpMdl::OxalateConc25()
{
  CheckConverged();
  return Comp_gL[iOxalate];
}

//---------------------------------------------------------------------------

double MurdochBayerSpMdl::TotalOrganics25()
{
  CheckConverged();
  return Comp_gL[iFormate]+Comp_gL[iAcetate] + Comp_gL[iOxalate];
}
//---------------------------------------------------------------------------

double MurdochBayerSpMdl::ChlorineConc25()
{
  CheckConverged();
  return Comp_gL[iNaCl];
}

//---------------------------------------------------------------------------

double MurdochBayerSpMdl::SulphateConc25()
{
  CheckConverged();
  return Comp_gL[iNa2SO4];
}

//---------------------------------------------------------------------------






double* MurdochBayerSpMdl::CheckConverged(double T, double P, MArray *pMA, bool calcSolubility)
{
  //if (pMA->==&MassArray) 
  //  {
  //  int xxx=0;
  //  }
  if (!pMA /*|| (pMA==&MassArray)*/) {   
    // Calculation for *this* species
    if ((T<0.0  || T==this->Temperature) && 
	(P<0.0  || P==this->Pressure))  { // Call for stream conditions and composition
      Log.Message(MMsg_Note, "Stream Conditions...");
      if (!TestMStateValid(0))
	Log.Message(MMsg_Note, "M State Invalid...");
      if (!TestHStateValid(0))
	Log.Message(MMsg_Note, "H State Invalid...");

      if (!TestMStateValid(0) || !TestHStateValid(0))  {  // Composition Changed, recalculate
	Bayer(this->Temperature, this->Pressure, MArray(this), dpData);
	Bayer(C2K(25.0), this->Pressure, MArray(this), dpData25);
	Bayer(C2K(0), 101.325, MArray(this), dpData0);
	SetMStateValid(0);
	SetHStateValid(0);
      }
      // Results in DPData 
      return dpData;
    }
    if (T==C2K(25.0)) {    // At reference temperature (25 C)
      Log.Message(MMsg_Note, "Reference (25 C)");
      if (!TestMStateValid(0))    // Composition Changed, recalculate
	Bayer(C2K(25.0), this->Pressure, MArray(this), dpData25); // Results in DPData 
      return dpData25;
    }
    if (T==C2K(0.0)) {    //  At Enthalpy/Entropy reference temperature 
      Log.Message(MMsg_Note, "Reference (0 C)");
      if (!TestMStateValid(0))    // Composition Changed, recalculate
	Bayer(C2K(0.0), 101.325, MArray(this), dpData0); // Results in DPData0 
      return dpData0;
    }
    if (T==C2K(20.0)) {    //  At Enthalpy/Entropy reference temperature 
      Log.Message(MMsg_Note, "Reference (20 C)");
      if (!TestMStateValid(0))    // Composition Changed, recalculate
	Bayer(C2K(20.0), this->Pressure, MArray(this), dpData20); // Results in DPData20 
      return dpData20;
    }
    // T specified but not equal to 25C or 0C
    Log.Message(MMsg_Note, "Stream MArray, Other Temperature...");
    Bayer(T, this->Pressure, MArray(this), dpDataX); // Results in DPDataX
    return dpDataX;
    

  } else {   // MArray specified, recalculate.
    Log.Message(MMsg_Note, "Specified MArray...");
    
    
    if (T<0) T = this->Temperature;
    if (P<0) P = this->Pressure; 
    Bayer(T, P, *pMA, dpDataX); // Results in DPData 
    return dpDataX;
  }
}






void MurdochBayerSpMdl::Bayer(double T_K, 
			      double p_kPa, 
			      MArray  & MA, 
			      double *DPData, 
			      bool calcSolubility) {
  if (!DPData)
    DPData = dpData;   // Primary copy
  Log.Message(MMsg_Note, "Recalculating... T = %8.2f P = %8.2f", T_K, p_kPa);

  long iFlags;
  double TempC = T_K-273.15;
  double Pressure = p_kPa/100.;
  double InComp[12];
  const double massFlow = MA.Mass();
  if (massFlow <  1.0e-8) return;
  


  double x = MA[CausticSoda]/massFlow;
  double y = MA[Alumina]/massFlow;

  InComp[0] = InComp[1] = 0.0;
  InComp[2] = 78./118.*y*100.;      // Al[OH]3    
  InComp[3] = (40/118.*y+x)*100.;    // NaOH       
  InComp[4] = MA[SodiumChloride]*100./massFlow;   // NaCl       
  InComp[5] = MA[SodiumCarbonate]*100./massFlow;  // Na2CO3     
  InComp[6] = MA[SodiumSulphate]*100./massFlow;   //  Na2SO4     
  InComp[7] = MA[SodiumOxalate]*100./massFlow;    //  Na2C2O4    
  InComp[10] = 0.0; // MA[SodiumFluoride]*100.;    //  NaF        

  iFlags = (calcSolubility ? 1 :0);
  



  const double acetate = 0.88*MA[Organics]*100./massFlow;
  const double formate = 0.12*MA[Organics]*100./massFlow;
  
  InComp[8] = acetate;      //  NaAcetate  
  InComp[9] = formate;	    //  NaFormate  

  //Log.Message(MMsg_Warning, "Bayer Density Call...T %8.2f P %8.2f", TempC, Pressure);
  

	      //, InComp[2], InComp[3], InComp[4],
//	      InComp[5], InComp[6], InComp[7]);
//


  bayer_(
	 /*double*, */  &TempC,
	 /*long*,   */  &InUnits,     
	 /*double*, */  &Pressure,
	 /*long*,   */  &NInComp,     
	 /*double*, */  InComp,      
	 /*double*  */  DPData,
	 /*long*,   */  &NOutComp,    
	 /*double*, */  Comp_molkg,
	 /*double*, */  Comp_molL,
	 /*double*, */  Comp_molL25,
	 /*double*, */  Comp_mpercent,
	 /*double*, */  Comp_gL,
	 /*long*,   */  &NOC,         
	 /*double*, */  OC,
	 /*long*,   */  &NGamma,      
	 /*double*, */  Gamma,
	 /*long*,   */  &NSI,         
	 /*double*, */  SI,
	 /*long*,   */  &NSol,        
	 /*double*, */  SolML,
	 /*double*  */  Solmkg,
	 /*long*    */  &iFlags
	 );


}









#if DBG_MVECTOR

double  MurdochBayerSpMdl::DumpIt(LPCTSTR FnTag, double V, double x1, double x2, double x3)
{
  return V;
};

bool MurdochBayerSpMdl::TestStateValid(int iNo)
{
  return MSpModelBase::TestStateValid(iNo);
};

#endif
