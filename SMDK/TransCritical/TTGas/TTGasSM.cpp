//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#include "stdafx.h"
#include "TTGasSM.h"
#include "math.h"
#include "md_headers.h"
//#pragma optimize("", off)

//===========================================================================
//
//
//
//===========================================================================

DEFINE_SPECIEMODEL(TTGasSM)
IMPLEMENT_SPECIEMODEL(TTGasSM, "TTGasSM", "*Gas", "Transtech Gas Species Model", DLL_GroupName)

//===========================================================================
// Specie Access
static MInitialiseTest InitTest("TTGasSMClass");



//MSpeciePtr   Methane           (InitTest, "CH4(g)",        false);
//MSpeciePtr   Steam             (InitTest, "H2O(g)",        true);

// ==========================================================================
// Specie Properties Modal Global Parameters


enum GM_GasStateEqn { GM_Ideal,  GM_SRK, GM_PR,GM_BWR };

// Global/Static data















byte TTGasSM::sm_iGasModel      = GM_SRK;


TTGasSM::TTGasSM(TaggedObject *pNd)
  {
  MInitialise(); // this must be called to initialise the SMDK library if neccessary

  }

//---------------------------------------------------------------------------

TTGasSM::~TTGasSM()
  {
  }

//---------------------------------------------------------------------------

bool TTGasSM::get_IsBaseClassOf(LPCTSTR OtherProgID)
	{
  return false;
  //return S_OK;
	}

//---------------------------------------------------------------------------

LPCTSTR TTGasSM::get_PreferredModelProgID()
{
  return NULL;
}

//---------------------------------------------------------------------------

bool TTGasSM::ValidateDataFields()
  {
  bool OK=MSpModelBase::ValidateDataFields();
  return OK;
  }

//---------------------------------------------------------------------------

double TTGasSM::get_Density(long Phases, double T, double P, MArray *pMA)
  {
  MArray MA(pMA ? *pMA : this);
  double MSol=0;
  double MLiq=0;
  double MGas=0;
  long   SpecieCount=gs_MVDefn.Count();
  //double * M=MassVector;
  for (int i=0; i<SpecieCount; i++)
    {
    long Ph=gs_MVDefn[i].Phase();
    if (Ph & MP_Sol)
      MSol+=MA[i];
    else if (Ph & MP_Liq)
      MLiq+=MA[i];
    else
      MGas+=MA[i];
    }

  double DSol=2000;
  double DLiq=1000;
  //double mwt = Mass(MP_Gas)/GTZ(Moles(MP_Gas));
  //double DGas=101.325/8.3143*P/GTZ(T)*mwt;
  double DGas = 1;
  return (MSol+MLiq+MGas)/GTZ(MSol/DSol+ MLiq/DLiq+ MGas/DGas);
  }


double TTGasSM::get_MWT(MArray *pMA) {
  MArray MA(pMA ? *pMA : this);
  long   SpecieCount=gs_MVDefn.Count();
  double mtot=0.0, mwtot=0.0;
  for (int i=0; i<SpecieCount; i++) {
    //mtot += MA[i];
    mwtot += MA[i]*gs_MVDefn[i].MolecularWt();
  }
  return mwtot/MA.Mass();
  // MoleWt();
}


//---------------------------------------------------------------------------

double TTGasSM::GasCpCalc(double Tk, double Pbar)
{
  MArray MA(this);
  return MSpModelBase::get_msCp(MP_Gas, Tk, 1.0, &MA);
}

double TTGasSM::GasHCalc(double Tk, double Pbar)
{
  return 2.2*Tk;
}


//---------------------------------------------------------------------------

double TTGasSM::get_msEnthalpy(long Phases, double T, double P, MArray *pMA)
  {
  MArray MA(pMA ? (*pMA) : this);
  return MSpModelBase::get_msEnthalpy(Phases, T, P, pMA);
  }

//---------------------------------------------------------------------------

double TTGasSM::get_msEntropy(long Phases, double T, double P, MArray *pMA)
  {
  return MSpModelBase::get_msEntropy(Phases, T, P, pMA);
  }

//---------------------------------------------------------------------------

double TTGasSM::get_msCp(long Phases, double T, double P, MArray *pMA)
  {
  MArray MA(pMA ? (*pMA) : this);
  return MSpModelBase::get_msCp(MP_Gas, T, P, pMA);
  }

//---------------------------------------------------------------------------
// Dew Point Calculation
double TTGasSM::get_SaturationT(double P, MArray *pMA)
  {
  MArray MA(pMA ? (*pMA) : this);
  return 250.;
  

  }

//---------------------------------------------------------------------------




//---------------------------------------------------------------------------

 double TTGasSM::get_DynamicViscosity(long Phases, double T, double P, MArray *pMA)
   {
   return MSpModelBase::get_DynamicViscosity(Phases, T, P, pMA);
   }

//---------------------------------------------------------------------------

double TTGasSM::get_ThermalConductivity(long Phases, double T, double P, MArray *pMA)
  {
  return MSpModelBase::get_ThermalConductivity(Phases, T, P, pMA);
  }


enum {
//=== MP_GlobalProp ===
  idGasModel,

//=== MP_ConfigProp ===
  idDefineGas,
  idRqdMWT,
  // Normal
  idSeparator1				  ,

  idGasDensity,
  idNormalDensity,
  idGasMWT,
  idGasCp,
  idGasViscosity,
  idGasConductivity,
  idMPI_EndOfProps };

//---------------------------------------------------------------------------

long TTGasSM::DefinedPropertyCount()
  {
  return idMPI_EndOfProps + 1 + MSpModelBase::DefinedPropertyCount();
  }

//---------------------------------------------------------------------------

static MPropertyInfo::MStringValueP SVGM[]={
  { "Ideal", GM_Ideal },  
  { "Soave-Redlich-Kwong", GM_SRK },
  { "Peng-Robinson", GM_PR   },
  { "Benedict-Webb-Rubin", GM_BWR },
  {0}};



#define MP_RN   MP_Result|MP_NoFiling
#define MP_RNH  MP_Result|MP_NoFiling|MP_InitHidden

long TTGasSM::DefinedPropertyInfo(long Index, MPropertyInfo & Info)
  {//define a list of all properties: This sets what is displayed in the access window.
  long Inx=Index-MSpModelBase::DefinedPropertyCount();
  switch(Inx)
    {
    case idGasModel :
      Info.SetText("--Global Gas Properties----------");
      Info.SetStructName("Config");
      Info.Set(ePT_Long, "", "GasModel",   SVGM,    MP_GlobalProp|MP_Parameter,  "Global Gas Model"); return Inx;
    case idDefineGas : 
      Info.SetText("--DefineGas-------------");
      Info.SetStructName("Gas"); //NB: The "struct name" is used as part of tag!
      Info.Set(ePT_Bool,"", "DefineGas",    MC_, "",    0, 0,    MP_ConfigProp|MP_Parameter|MP_CheckBox, ""); return Inx;
    case idRqdMWT	    : Info.Set(ePT_Double,   "", "RqdMWT",     MC_, "",    0, 0,    MP_ConfigProp|MP_Parameter, "The required MWT"); return Inx;

    case idGasDensity: 
      Info.SetStructName("Props"); //NB: The "struct name" is used as part of tag! Recomend "Props" for compatability with other properties models.
      Info.SetText("--Gas Properties-----------");
      Info.Set(ePT_Double,    "", "GasDensity",MC_Conc, "g/L",    0, 0,  MP_RN,    "Gas Density @ T,P"); return Inx;


    case idSeparator1	    : Info.SetText("..."); return Inx;

    case idGasMWT :
      Info.Set(ePT_Double,    "", "GasMWT",MC_, "",    0, 0,  MP_RN,    "Molecular Wt"); return Inx;
      
    case idNormalDensity:
      Info.Set(ePT_Double,    "", "NormalDensity",MC_Conc, "g/L",    0, 0,  MP_RN,    "Gas Density @1atm, 0C"); return Inx;
    case idGasCp:
      Info.Set(ePT_Double,    "", "SpecificHeat",MC_Conc, "g/L",    0, 0,  MP_RN,    "Specific Heat @T,P"); return Inx;
    case idGasViscosity:
      Info.Set(ePT_Double,    "", "Viscosity",MC_Conc, "g/L",    0, 0,  MP_RN,    "Viscosity @T,P"); return Inx;
    case idGasConductivity:
      Info.Set(ePT_Double,    "", "Conductivity",MC_Conc, "g/L",    0, 0,  MP_RN,    "Conductivity @T,P"); return Inx;

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

DWORD TTGasSM::GetPropertyVisibility(long Index)
  {//determine visibility of list of all properties
  switch (Index-MSpModelBase::DefinedPropertyCount())
    {
    case idRqdMWT: return fDoCalc ? ePVis_All : (ePVis_DynFull|ePVis_DynFlow|ePVis_Probal|ePVis_File);
    default:  return ePVis_All; 
    }
  return ePVis_All;
  }

//---------------------------------------------------------------------------
// Stringifying operators GV...  Get Value
#define GV(x, type) case id##x :Value = type##x; return 
#define GVAL(x)   case id##x : Value = d##x; return
#define GVAL2(v, x) case id##v : Value = x; return
#define GVALF(x) case id##x : Value = x##(); return
#define GVALFT(x) case id##x : Value = x##(T); return
#define GVALFTP(x) case id##x : Value = x##(T,P); return

void TTGasSM::GetPropertyValue(long Index, ULONG Phase/*=MP_All*/, double T/*=NAN*/, double P/*=NAN*/, MPropertyValue & Value)
  {//define method of retrieving values for list of all properties
  switch (Index-MSpModelBase::DefinedPropertyCount())
    {
      GV(GasModel, sm_i);
      GVAL2(DefineGas, fDoCalc);
      GVAL2(GasMWT, get_MWT(NULL));
      GVAL(RqdMWT);
      GVAL2(NormalDensity, NormalDensity());
    default: Value=0.0; return;
    }
  }

//---------------------------------------------------------------------------
// Stringifying operators  Put Value....
#define PV(x, type) case id##x : type##x = Value; return 
#define PVAL(x)   case id##x : d##x = Value; return


void TTGasSM::PutPropertyValue(long Index, MPropertyValue & Value)
{//define method of setting values for list of all properties
  switch (Index-MSpModelBase::DefinedPropertyCount())
    {
    case idGasModel   : sm_iGasModel=(byte)(long)Value;     return;
      
    case idDefineGas		: fDoCalc=Value;     return;
      PVAL(RqdMWT); 
    }
}

#define SWAP(a, b) {double tmp=a; a=b; b=tmp;}

void Order(double *x)
{
    if (x[1]<x[2]) SWAP(x[1],x[2])
		       if (x[0]<x[1]) {
			   SWAP(x[0],x[1])
			       if (x[1]<x[2]) SWAP(x[1],x[2])
						  }
}


int cubic(double a1, double a2, double a3, double x[])
{
    double Q, R;
    double a12 = a1*a1;
    double Z;
    Q = (a12-3*a2)/9; R = (2*a1*a12-9*a1*a2+27*a3)/54;
    Z = Q*Q*Q - R*R;
    if (Z == 0.0) {
	if (Q==0.0) {
	    x[0] = -a1/3;
	    return 1;                     // Single root of order 3
	} else {
	    x[0] = 2*sqrt(Q)-a1/3;
	    x[1] = -sqrt(Q)-a1/3;
	    return 2;                     //  Two Real roots, one of order 2

	}
    } else if (Z < 0.0) {
	double z1 = sqrt(-Z);
	double z2 = pow(z1 + fabs(R), 1.0/3.0);
	z1 = z2 + Q/z2;
	if (R>0) z1 = -z1;
	x[0] = z1 - a1/3;
	return 1;                          // Single real root (two complex roots)
    } else {
	double theta = R/sqrt(Q*Q*Q);
	double Q2 = -2.0*sqrt(Q);
	theta = acos(theta);
	x[0] = Q2*cos(theta/3.0)-a1/3.0;
	x[1] = Q2*cos((theta+2*PI)/3.0)-a1/3.0;
	x[2] = Q2*cos((theta+4*PI)/3.0)-a1/3.0;
	Order(x);
	return 3;                          // Three real roots
    }                                      
}
