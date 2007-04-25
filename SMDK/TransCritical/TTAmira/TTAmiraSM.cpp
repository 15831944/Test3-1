//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
//    Amira Bayer Model Transcritical Technologies Pty Ltd Feb 05
//   Time-stamp: <2007-04-25 04:09:00 Rod Stephenson Transcritical Pty Ltd>
// Copyright (C) 2005 by Transcritical Technologies Pty Ltd and KWA
//===========================================================================
#include "stdafx.h"
#include "TTAmiraSM.h"
#include "math.h"
#include "md_headers.h"
#pragma comment(lib, "rpcrt4.lib")

#define FORCECNVG 0
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

DEFINE_SPECIEMODEL(AmiraBayer)
IMPLEMENT_SPECIEMODEL(AmiraBayer, "AmiraSM", "*Amira", "Amira Bayer Specie Model", DLL_GroupName)

//===========================================================================
// Specie Access
static MInitialiseTest InitTest("AmiraSMClass");


//AmiraBayer::hAmiraDLL = NULL;



MSpeciePtr   Water             (InitTest, "H2O(l)",        false);
MSpeciePtr   Alumina           (InitTest, "NaAl[OH]4(l)",  false);
MSpeciePtr   CausticSoda       (InitTest, "NaOH(l)",       false);
MSpeciePtr   SodiumChloride    (InitTest, "NaCl(l)",       false);
MSpeciePtr   SodiumSulphate    (InitTest, "Na2SO4(l)",     false);
MSpeciePtr   SodiumCarbonate   (InitTest, "Na2CO3(l)",     false);
MSpeciePtr   SodiumOxalate     (InitTest, "Na2C2O4(l)",    false); //organic
//MSpeciePtr   SodiumSilicate    (InitTest, "Na2SiO3(l)",    false); 
//MSpeciePtr   Organics          (InitTest, "Na2C5.2O7.2(l)", false); //organic
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

AmiraBayer::AmiraBayer(TaggedObject *pNd)
  {
  MInitialise(); // this must be called to initialise the SMDK library if neccessary
  //if (!hAmiraDLL)
  //hAmiraDLL = LoadLibrary("amira");
  

  }

//---------------------------------------------------------------------------

AmiraBayer::~AmiraBayer()
  {
  }

//---------------------------------------------------------------------------

bool AmiraBayer::get_IsBaseClassOf(LPCTSTR OtherProgID)
  {
  return false;
  //return S_OK;
  }

//---------------------------------------------------------------------------

LPCTSTR AmiraBayer::get_PreferredModelProgID()
  {
  return NULL;
  }

//---------------------------------------------------------------------------

bool AmiraBayer::ValidateDataFields()
  {
  bool OK=MSpModelBase::ValidateDataFields();
  //LiquorComposition(fDoCalc);
  return OK;
  }

//---------------------------------------------------------------------------

double AmiraBayer::get_Density(long Phases, double T, double P, MArray *pMA)
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
    CheckConverged(pMA);

    Dl = dpData[iRho_Liq];
    }

  return DUMPIT0("Density", DensityMix(FSol, dNAN, FLiq, Dl, (1.0-FSol-FLiq), dNAN, T, P, MA));
  }

//---------------------------------------------------------------------------

double AmiraBayer::LiqCpCalc(MArray & MA, double Tc)
  {
  CheckConverged(&MA);

  return 4.184;
  }

double AmiraBayer::LiqHCalc(MArray & MA, double Tc)
  {
  CheckConverged(&MA);

  return  4.184*(Tc-273.15);
  }


//---------------------------------------------------------------------------

double AmiraBayer::get_msEnthalpy(long Phases, double T, double P, MArray *pMA)
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
    Hl = LiqHCalc(MA, Tc) /*LiqCpCalc(MA, Tc) * Tc*/;
    }
  double Hs=(FSol>1.0e-9) ? gs_MVDefn.msHz(MP_Sol, T, P, PropOverides, &MA) : 0.0;
  return DUMPIT0("msEnthalpy", msEnthalpyMix(FSol, Hs, FLiq, Hl, (1.0-FSol-FLiq), dNAN, T, P, MA));
  }

//---------------------------------------------------------------------------

double AmiraBayer::get_msEntropy(long Phases, double T, double P, MArray *pMA)
  {
  return MSpModelBase::get_msEntropy(Phases, T, P, pMA);
  }

//---------------------------------------------------------------------------

double AmiraBayer::get_msCp(long Phases, double T, double P, MArray *pMA)
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
    Cpl = LiqCpCalc(MA, K_2_C(T));
    }
  double Cps=2.2;
  return msCpMix(FSol, Cps, FLiq, Cpl, (1.0-FSol-FLiq), dNAN, T, P, MA);
  }

//---------------------------------------------------------------------------

double AmiraBayer::get_SaturationT(double P, MArray *pMA)
  {
  MArray MA(pMA ? (*pMA) : this);

  CheckConverged(&MA);

  double PureSatT = MSpModelBase::get_SaturationT(P, &MA);
  if (MA.Mass(MP_Liq)/GTZ(MA.Mass())<1.0e-6)
    return DUMPIT1("SaturationTPure", PureSatT, P);


  const double TotNa2C = NaOnCS();
  double SatT = 300.;
  return DUMPIT3("SaturationT", SatT, P, TotNa2C, LiqConcs25.Liq[::CausticSoda]);

  }

//---------------------------------------------------------------------------

double AmiraBayer::get_SaturationP(double T, MArray *pMa)
  {

    return 1000.;
    

  }

//---------------------------------------------------------------------------

double AmiraBayer::BoilPtElev(MArray & MA, double P)
  {

  double PureSatT = MSpModelBase::get_SaturationT(P, &MA);
  return DUMPIT1("BoilPtElev", get_SaturationT(P, &MA)  -  PureSatT, P);
  }

//---------------------------------------------------------------------------

double AmiraBayer::get_DynamicViscosity(long Phases, double T, double P, MArray *pMA)
  {
  if (Phases & MP_Gas)
    return MSpModelBase::get_DynamicViscosity(Phases, T, P, pMA); //todo : check!

  MArray MA(pMA ? (*pMA) : this);

  CheckConverged(&MA);

  double Tc = Max(-41.0, K_2_C(T));
  const double C = 200;
  const double A = 100.;

  return exp ( 0.58-1.816*log(Tc+42.0) + 
    (0.5*C + 0.43*A) / (Tc+42.0) );


  }

//---------------------------------------------------------------------------
//  Not implemented; use base case
double AmiraBayer::get_ThermalConductivity(long Phases, double T, double P, MArray *pMA)
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
  idMinSolCp        	,

  idK1_BoundSodaCalc,

  idG0H2o, 

  idAluminaConc25			,
  idCausticConc25			,
  idSodaConc25				  ,
  idAtoC					,
  idCtoS					,
  idCltoC	      ,
  idTOC25					    ,
  idCarbonateConc25		,
  idSolidsConc25			  ,
  idFreeCaustic25      ,
  idNaOnCS, 
  idTOS25					    ,
  idNaSO4Conc25			  ,
  idNaClConc25				  ,
  idNa2C2O4Conc25			,
  idTOStoTOC           ,

  idSeparator1				  ,

  idOxalateConc25			,
  idTotalOrganics25		,
  idChlorineConc25			,
  idSulphateConc25			,
  idTotalNa25				  ,
  idOrganateConc25			,

  idSeparator2				  ,

  idLVolume25				  ,
  idSLVolume25				  ,
  idLDensity25 			  ,
  idSLDensity25			  ,

  idSeparator3				  ,	
  // At temperature  
  idAluminaConc			  ,
  idCausticConc			  ,
  idSodaConc				    ,
  idTOC					      ,
  idSolidsConc				  ,
  idSodiumCarbonateConc,

  idSeparator4				  ,	


  //  idSeparator5				  ,	

  idAluminaConcSat   ,
  idAtoCSaturation			,
  idSSNRatio				    ,
  idAluminaSSN ,
  idIonicStrength    ,
  idBoilPtElev				  ,
  idBoundSodaSat  ,
  idTHAMassFlow   			,
  idTHADens				    ,


  idMPI_EndOfProps 
  };

//---------------------------------------------------------------------------

long AmiraBayer::DefinedPropertyCount()
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

long AmiraBayer::DefinedPropertyInfo(long Index, MPropertyInfo & Info)
  {//define a list of all properties: This sets what is displayed in the access window.
  long Inx=Index-MSpModelBase::DefinedPropertyCount();
  switch(Inx)
    {

    case idBPE_Factor   : Info.Set(ePT_Double,    "",  "BPE_Factor", MC_Frac, "%", 0,0,   MP_GPH, "BPE Factor"); return Inx;
    case idRho_Factor   : Info.Set(ePT_Double,    "",  "Rho_Factor", MC_Frac, "%", 0,0,   MP_GPH, "Rho Factor"); return Inx;
    case idMinSolCp   	: Info.Set(ePT_Double,    "", "MinSolCp",  MC_CpMs,  "kJ/kg.C", 0, 0,  MP_GPH, "Global minimum solids Cp"); return Inx;




    case idAluminaConc25 : 
      Info.SetStructName("Props"); //NB: The "struct name" is used as part of tag! Recomend "Props" for compatability with other properties models.
      Info.SetText("AmiraBayer Liquor Values @ 25");
      Info.Set(ePT_Double,    "A", "AluminaConc",MC_Conc, "g/L",    0, 0,  MP_RN,    "Alumina Concentration @ 25"); return Inx;
    case idCausticConc25    : 

      Info.Set(ePT_Double,    "C", "CausticConc",   MC_Conc, "g/L",    0, 0,  MP_RN,    "Caustic Concentration @ 25"); return Inx;

    case idAtoC		    : Info.Set(ePT_Double,    "", "A/C",  MC_,"",  0, 0,  MP_RN,    "A to C ratio"); return Inx;
    case idCtoS		    : Info.Set(ePT_Double,    "", "C/S",  MC_,"",  0, 0,  MP_RN,    "C to S ratio"); return Inx;
    case idCltoC	    : Info.Set(ePT_Double,    "", "Cl/C", MC_,"",  0, 0,  MP_RN,    "Cl to C ratio"); return Inx;
    case idTOStoTOC 	    : Info.Set(ePT_Double,    "", "TOS/TOC",   MC_,"",  0, 0,  MP_RNH,    "TOS to TOC ratio"); return Inx;
    case idTOC25	    : Info.Set(ePT_Double,    "", "TOC",    MC_Conc, "g/L",    0, 0,  MP_RN,  "Total Organic Carbon Concentration @ 25"); return Inx;
    case idTOS25	    : Info.Set(ePT_Double,    "", "TOS@25",    MC_Conc, "g/L",    0, 0,  MP_RNH,  "Total Organic Soda Concentration @ 25"); return Inx;
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
    case idTotalNa25	    : Info.Set(ePT_Double,    "", "TotalNa",MC_Conc, "g/L",    0, 0,  MP_RN,    "Total Sodium Concentration @ 25"); return Inx;

    case idSeparator2  	  : Info.SetText("..."); return Inx;

    case idLVolume25	  : Info.SetText("AmiraBayer Liquor Values @25-----------------");
      Info.Set(ePT_Double,    "", "LVolFlow25",MC_Qv,   "L/s",    0, 0,  MP_RN,    "Liquor Volumetric flowrate @ 25"); return Inx;
    case idSLVolume25	  : Info.Set(ePT_Double,    "", "SLVolFlow25",    MC_Qv,   "L/s",    0, 0,  MP_RN,    "Slurry Volumetric flowrate @ 25"); return Inx;
    case idLDensity25     : Info.Set(ePT_Double,    "", "LRho25",    MC_Rho,  "g/L",    0, 0,  MP_RN,    "Liquor Density @ 25"); return Inx;
    case idSLDensity25    : Info.Set(ePT_Double,    "", "SLRho25",   MC_Rho,  "g/L",    0, 0,  MP_RN,    "Slurry Density @ 25"); return Inx;

    case idG0H2o:          Info.Set(ePT_Double,    "", "G0H2O",   MC_,  "",    0, 0,  MP_RN,    "Water Free Energy"); return Inx;


    case idSeparator3	  : Info.SetText("..."); return Inx;

    case idAluminaConc	  : Info.SetText("AmiraBayer Liquor Values @ Temp");
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
    case idNaOnCS         : Info.Set(ePT_Double,    "", "NaOnCS",   MC_,  "", 0, 0,  MP_RNH,    "Total Sodium on Caustic"); return Inx;

    case idIonicStrength  : Info.Set(ePT_Double,    "I", "IonicStrength", MC_, "",    0, 0,  MP_RN,  "Ionic Strength"); return Inx;



    case idBoundSodaSat   : Info.Set(ePT_Double,    "", "BoundSodaSat", MC_Frac, "%",    0, 0,  MP_RN,  "Bound Soda Saturation"); return Inx;
    case idAluminaConcSat : Info.SetText("--Other AmiraBayer Liquor Properties @ T-------");
      Info.Set(ePT_Double,    "ASat", "A_Saturation", MC_Conc, "g/L",    0, 0,  MP_RN,  "Alumina Saturation Concentration @ T"); return Inx;
    case idAtoCSaturation : Info.Set(ePT_Double,    "A/CSat", "A/C_Saturation", MC_,"",  0, 0,  MP_RNH,  "Alumina Saturation to Caustic ratio @ T"); return Inx;
    case idSSNRatio       : Info.Set(ePT_Double,    "", "SSN_Ratio",  MC_,"",  0, 0,  MP_RN,  "A/C actual to ASat/C ratio @ T"); return Inx;
    case idAluminaSSN     : Info.Set(ePT_Double,    "", "Alumina_SSN",  MC_,"",  0, 0,  MP_RN,  "Supersaturated Alumina/FreeCaustic"); return Inx;
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

DWORD AmiraBayer::GetPropertyVisibility(long Index)
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
#define GVAL2(v, x) case id##v : CheckConverged(); Value = x; return
#define GVALF(x) case id##x : Value = x##(); return
#define GVALFT(x) case id##x : Value = x##(T); return

void AmiraBayer::GetPropertyValue(long Index, ULONG Phase/*=MP_All*/, double T/*=NAN*/, double P/*=NAN*/, MPropertyValue & Value)
  {//define method of retrieving values for list of all properties
  switch (Index-MSpModelBase::DefinedPropertyCount())
    {


    GVALF(THAMassFlow);
    GVALFT(THADens);
    GVALFT(CausticConc);
    GVALFT(AluminaConc);
    GVALFT(SodaConc);
    GVALFT(SodiumCarbonateConc);

    GVAL2(CausticConc25, 101.);
    GVAL2(AluminaConc25, 102.);
    GVAL2(SodaConc25, 103.);
    GVAL2(CarbonateConc25, 104.);
    GVAL2(NaClConc25, 105.);
    GVAL2(NaSO4Conc25, 106.);
    GVAL2(Na2C2O4Conc25,107.);
    GVALFT(SolidsConc);
    GVALF(SolidsConc25);
    GVAL2(FreeCaustic25,FreeCaustic(C_2_K(25.0)));
    GVALFT(TOC);
    GVALF(TOC25);
    GVALF(TOS25);


    GVALF(AtoC);
    GVALF(CtoS);
    GVALF(CltoC);
    GVALF(TOStoTOC);

    case idBoilPtElev	: Value=BoilPtElev(MArray(this), P);        return; 
    case idG0H2o : Value = G0H2O(P/100., T); return;
      GVALF(LVolume25);
      GVALF(SLVolume25);
      GVALF(OrganateConc25);
      GVALF(OxalateConc25);
      GVALF(TotalOrganics25);
      GVALF(ChlorineConc25);
      GVALF(SulphateConc25);
      GVALF(TotalNa25);
      GVALFT(AluminaConcSat);
      GVALF(IonicStrength);
      GVALFT(AtoCSaturation);
      GVALFT(SSNRatio);
      GVALFT(AluminaSSN);
      GVALF(LDensity25);
      GVALF(SLDensity25);
      GVALF(NaOnCS);

    default: Value=0.0; return;
    }
  }

//---------------------------------------------------------------------------
// Stringifying operators  Put Value....
#define PV(x, type) case id##x : type##x = Value; return 
#define PVAL(x)   case id##x : d##x = Value; return


void AmiraBayer::PutPropertyValue(long Index, MPropertyValue & Value)
  {//define method of setting values for list of all properties
  switch (Index-MSpModelBase::DefinedPropertyCount())
    {
    default:
      break;
    }
  }




// Digester A/C Equilibrium






// 
double AmiraBayer::DRatio(double T_) 
  {
  return 1.2;
  }

//---------------------------------------------------------------------------

double AmiraBayer::CausticConc(double T_, MArray *pMA)
  {
  CheckConverged(pMA);

  return 100.;
  }

//---------------------------------------------------------------------------

double AmiraBayer::CausticConc(double T_)
  {
  return CausticConc(T_, &MArray(this));
  }

//---------------------------------------------------------------------------

double AmiraBayer::AluminaConc(double T_)
  {
  CheckConverged();

  return 50.;
  }

//---------------------------------------------------------------------------

double AmiraBayer::SodaConc(double T_)
  {
  CheckConverged();

  return 200.;
  }

//---------------------------------------------------------------------------

double AmiraBayer::SodiumCarbonateConc(double T_)
  {
  CheckConverged();

  return 200.;
  }

//---------------------------------------------------------------------------

double AmiraBayer::SodiumSulphateConc(double T_)
  {
  CheckConverged();

  return 200.;
  }

//---------------------------------------------------------------------------

double AmiraBayer::SodiumOxalateConc(double T_)
  {
  CheckConverged();

  return 200.;
  }

//---------------------------------------------------------------------------

double AmiraBayer::SolidsConc(double T_)
  {
  const double mt=Mass(MP_Sol);
  return (mt>=UsableMass) ? mt/GTZ(Mass(MP_SL))*get_Density(MP_SL, T_, Pressure, NULL) : 0.0;
  }

double AmiraBayer::SolidsConc25()
  {
  const double mt=Mass(MP_Sol);
  return (mt>=UsableMass) ? mt/GTZ(Mass(MP_SL))*get_Density(MP_SL, C_2_K(25.0), Pressure, NULL) : 0.0;
  }



double AmiraBayer::TOC(double T_)
  {
  CheckConverged();

  double Toc = 100.;
  return Max(0.0, Toc*DRatio(T_));
  }

double AmiraBayer::TOC25()
  {
  CheckConverged();

  return 100.;
  }
//---------------------------------------------------------------------------

double AmiraBayer::TOS(double T_)
  {
  CheckConverged();

return 100.;
  }

double AmiraBayer::TOS25()
  {
  CheckConverged();

 return 100.;
  }

//---------------------------------------------------------------------------

double AmiraBayer::TOStoTOC()
  {
  const double TOC = TOC25();
  if (TOC>1.0e-6) {
    const double TOS = TOS25();
    return TOS/TOC;
    }
  return 0.0;
  }

//---------------------------------------------------------------------------

double AmiraBayer::FreeCaustic(double T_, MArray *pMA)
  {
  return CausticConc(T_, pMA) * (1.0 - AtoC(pMA)*::SodiumCarbonate.MW/::AluminaSolid.MW);
  }

//---------------------------------------------------------------------------

double AmiraBayer::FreeCaustic(double T_)
  {
  return FreeCaustic(T_, &MArray(this));
  }

//---------------------------------------------------------------------------

double AmiraBayer::AtoC(MArray *pMA)
  {
  CheckConverged(pMA);

  return .75;
  }

//---------------------------------------------------------------------------

double AmiraBayer::AtoC()
  {
    return AtoC(&MArray(this));
  }

//---------------------------------------------------------------------------

double AmiraBayer::CtoS()
  {
  CheckConverged();

  return .89;
  }

//---------------------------------------------------------------------------

double AmiraBayer::CltoC()
  {
  CheckConverged();

  return .12;
  }

//---------------------------------------------------------------------------

double AmiraBayer::Na2SO4toC()
  {
  CheckConverged();

  return .34;
  }

//---------------------------------------------------------------------------

double AmiraBayer::THAMassFlow()
  {
  MArray M(this);
  return M[::THA];
  }

//---------------------------------------------------------------------------

double AmiraBayer::THADens(double T_)
  {
  return gs_MVDefn[::THA].Density(T_, StdP, NULL);
  }

//---------------------------------------------------------------------------

double AmiraBayer::IonicStrength()
  {
  CheckConverged();
  return 5.90;
  }

//---------------------------------------------------------------------------
// The original SySCAD implementation had a major bug in that the switch/case constructs had no
// break statements, hence the code always fell though all the cases to the final one, ie Alpart02

double AmiraBayer::AluminaConcSat(double T_)
  {
    return 300.;
  }

//---------------------------------------------------------------------------


double AmiraBayer::LVolume25()
  {
  const double mt=Mass(MP_Liq);
  return ((mt>=UsableMass) ? (mt / get_Density(MP_Liq, C_2_K(25.0), Pressure, NULL)) : 0.0);
  }

double AmiraBayer::SLVolume25()
  {
  const double mt=Mass(MP_SL);
  return ((mt>=UsableMass) ? (mt / get_Density(MP_SL, C_2_K(25.0), Pressure, NULL)) : 0.0);
  }

//---------------------------------------------------------------------------

double AmiraBayer::LDensity25()
  {
  return (get_Density(MP_Liq, C_2_K(25.0), Pressure, NULL));
  }

double AmiraBayer::SLDensity25()
  {
  return (get_Density(MP_SL, C_2_K(25.0), Pressure, NULL));
  }

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

double AmiraBayer::AtoCSaturation(double T_)
  {
  double C = CausticConc(T_);
  if (C<1.0e-12)
    return 0.0;
  const double ACsat = AluminaConcSat(T_) / GTZ(C);
  return ACsat;
  }

//---------------------------------------------------------------------------


double AmiraBayer::BoundSodaSat(double T_)
  {
  CheckConverged();

  return .2;
  }

double AmiraBayer::SSNRatio(double T_)
  {   // AtoC actual / AtoC saturation ==> A/ASat
  return 1.1;
  }

//---------------------------------------------------------------------------




double AmiraBayer::TotalNa25()
  {//TotalNa Conc @ 25 expressed as Na2CO3
  CheckConverged();

  double Sodium = 321;

  return Sodium;
  }

//---------------------------------------------------------------------------

double AmiraBayer::NaOnCS() 
  { 
  return 1.0;
  }


double AmiraBayer::Na2CO3toS()
  {
  CheckConverged();

return 2.0;
  }

//---------------------------------------------------------------------------

double AmiraBayer::OrganateConc25()
  {//Organic Na2C5O7 + NaOrg Conc @ 25
  CheckConverged();

  return .5;
  }

//---------------------------------------------------------------------------

double AmiraBayer::OxalateConc25()
  {
  CheckConverged();

  return 0.6;
  }

//---------------------------------------------------------------------------

double AmiraBayer::TotalOrganics25()
  {
  CheckConverged();

  return 0.7;
  }
//---------------------------------------------------------------------------

double AmiraBayer::ChlorineConc25()
  {
  CheckConverged();

  return 0.85;
  }

//---------------------------------------------------------------------------

double AmiraBayer::SulphateConc25()
  {
  CheckConverged();
  
  return 0.9;
  }

//---------------------------------------------------------------------------



double AmiraBayer::TOOCtoC()
  {
  CheckConverged();
  return 0.95;
  }



double AmiraBayer::AluminaSSN(double T_)
  {
  return 1.3;
  }


void AmiraBayer::CheckConverged(MArray *pMA)
  {
  //static int xxx=0;
  //xxx++;
  //if (1 || Dbg.Marked())
  //  {
  //  Dbg.Print("CC: %4i %-20s", xxx, Tag);
  //  Dbg.PrintLn("");
  //  }

  MArray MA(pMA ? (*pMA) : this);
  if ( !TestStateValid(0) || FORCECNVG )
    RecalcAmira();
  }

void AmiraBayer::RecalcAmira() 
{


  //Bayer(300., 100., )

  return;
}






void AmiraBayer::Bayer(double T_K, double p_kPa, double *x) {
  double TempC = T_K-273.15;
  double Pressure = p_kPa/100.;
  double InComp[NInComp];
  for (int i=0; i<NInComp; i++) 
    InComp[i] = x[i]*100.;
  bayer_(
	 /*double*, */  &TempC,
	 /*long*,   */  &InUnits,     
	 /*double*, */  &Pressure,
	 /*long*,   */  &NInComp,     
	 /*double*, */  InComp,      
	 /*double*  */  dpData,

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
	 /*double*  */  Solmkg
	 );
}









#if DBG_MVECTOR

double  AmiraBayer::DumpIt(LPCTSTR FnTag, double V, double x1, double x2, double x3)
  {
  //static xxx=0;
  //xxx++;
  //if (1 || Dbg.Marked())
  //  {
  //  if (Valid(x3))
  //    Dbg.PrintLn("%5i] %-20s %-30s %25.12g  (%25.12g %25.12g %25.12g)", xxx, FnTag, Tag, V, x1,x2,x3);
  //  else if (Valid(x2))
  //    Dbg.PrintLn("%5i] %-20s %-30s %25.12g  (%25.12g %25.12g)", xxx, FnTag, Tag, V, x1,x2);
  //  else if (Valid(x1))
  //    Dbg.PrintLn("%5i] %-20s %-30s %25.12g  (%25.12g)", xxx, FnTag, Tag, V, x1);
  //  else
  //    Dbg.PrintLn("%5i] %-20s %-30s %25.12g  ()", xxx, FnTag, Tag, V);
  //  }

  return V;
  };

bool AmiraBayer::TestStateValid(int iNo)
  {
  //static int xxx=0;
  //xxx++;
  //if (1 || Dbg.Marked())
  //  {
  //  m_dbgM.SetSize(gs_MVDefn.Count());
  //  Dbg.Print("%3s %4i %-20s ", MSpModelBase::TestStateValid(iNo)?"OK":"", xxx, Tag);
  //  Dbg.Print(" Rho25) %.14g", LiqConcs25.Density25);

  //  if (fabs(Vector.msHfRaw()-m_dbgtotHf)>1e-12)
  //    Dbg.Print(" Hf) %.14g>%.14g", m_dbgtotHf, Vector.msHfRaw());
  //  m_dbgtotHf=Vector.msHfRaw();
  //  for (int i=0; i<gs_MVDefn.Count(); i++) 
  //    {
  //    if (fabs(M[i]-m_dbgM[i])>1e-12)
  //      Dbg.Print(" %i) %.14g>%.14g", i, m_dbgM[i], M[i]);
  //    m_dbgM[i]=M[i];
  //    }
  //  Dbg.PrintLn("");
  //  }

  return MSpModelBase::TestStateValid(iNo);
  };

#endif
