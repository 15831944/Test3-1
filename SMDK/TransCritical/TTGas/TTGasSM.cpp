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



MSpeciePtr   Methane           (InitTest, "CH4(g)",        false);
//MSpeciePtr   Steam             (InitTest, "H2O(g)",        true);

// ==========================================================================
// Specie Properties Modal Global Parameters


enum GM_GasStateEqn { GM_Ideal,  GM_SRK, GM_PR,GM_BWR };

// Global/Static data

static const double deltaT = .1;    /// \Delta T for derivative calculations



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
  
  int im = Methane.Index;
  MA.Normalise();


  double MSol=MA.MassFrac(MP_Sol);
  double MLiq=MA.MassFrac(MP_Liq);
  double MGas=MA.MassFrac(MP_Gas);

  double DSol = MSpModelBase::get_Density(MP_Sol, T, P, &MA);
  double DLiq = MSpModelBase::get_Density(MP_Liq, T, P, &MA);
  double DGas=GasDensity(T, P);
  if (DGas==0.0)   
    Log.Message(MMsg_Error, "Gas Density 0.0");

  double vol = 0.0;
  double mass = 0.0;
  if (Phases & MP_Sol) {
    vol+= MSol/DSol;
    mass+= MSol;
  }
  if (Phases & MP_Liq) {
    vol+= MLiq/DLiq;
    mass+= MLiq;
  }
  if (Phases & MP_Gas) {
    vol+= MGas/GTZ(DGas);
    
    mass+= MGas;
  }
  
  if (vol==0.0) return 0.0;
  return mass/vol;
}




double TTGasSM::HRes(double T_K, double p_kPa) {   
  GasZ(T_K, p_kPa);
  return m_dHRes;
}

double TTGasSM::SRes(double  T_K, double p_kPa) {
  GasZ(T_K, p_kPa);
  return m_dSRes;
}






double TTGasSM::GasZ(double T, double P) {
  switch (sm_iGasModel)  {
  case GM_Ideal:
    m_dHRes = m_dSRes = 0.0;
    return 1.0;
  case  GM_SRK: 
  case GM_PR: 
  case GM_BWR:
    return SRKn(T, P, NULL);
  }
  return 1.0;
}

double TTGasSM::SRKn(double T, double P, MArray *pMA)
// 	Soave-Redlich-Kwong Equations of State for multicomponent mixtures
{
  MArray MA(pMA ? *pMA : this);
  MA.Normalise();
  

  long   SpecieCount=gs_MVDefn.Count();

  double alpha[MAXCOMP], tr[MAXCOMP];
  double m[MAXCOMP], wi, ac[MAXCOMP], b[MAXCOMP];
  double aiaQ[MAXCOMP], bib[MAXCOMP];
  double bb = 0, aca = 0;
  double beta=0.0;
  double A, B;
  double c1=0.0, c2 = 0.0;
  for (int i=0; i<SpecieCount; i++) {
    double tc = gs_MVDefn[i].TCritical();
    double pc = gs_MVDefn[i].PCritical();
    tr[i] = T/tc;
    b[i] = .08667*R_c*tc/pc;
    ac[i] = .42747*Sqr(R_c*tc)/pc;
    wi = gs_MVDefn[i].Acentricity();
    m[i] = 0.480 + wi*(1.574-.176*wi);
    alpha[i] = 1+m[i]*(1-sqrt(tr[i]));
    aiaQ[i] = tc*alpha[i]/sqrt(pc);
    bib[i] = tc/pc;
  }
  
  for (int i=0; i<SpecieCount; i++) {
    double yi = MA[i];
    c1 += yi*aiaQ[i];
    c2 += yi*bib[i];
    if (yi==0.0) continue;
    bb += yi*b[i];
    for (int j=0; j<SpecieCount; j++)  {
      double yj = MA[j];
      if (yj==0.0) continue;
      double tmp = yi*yj*sqrt(ac[i]*ac[j]);  // *(1.0-k[ID[i]][ID[j]]);
      aca += tmp*alpha[i]*alpha[j];
      beta += tmp*((1+m[i])*alpha[j]+(1+m[j])*alpha[i]);
    }
  }
  
  beta /= 2.0;
  if (fabs(bb) <= 1.0e-3) {
    m_dHRes = m_dSRes = 0.0;
    return 1.0;
  }

  for (int i=0; i<SpecieCount; i++) {
    bib[i] /= c2;
    aiaQ[i] /= c1;
  }
  A = aca*P/Sqr(R_c*T);
  B = bb*P/R_c/T;
  double z[3];
  int nr = cubic(-1.0, A-B*(1+B), -A*B, z);
  in3RootRgn = (nr>1) ? true : false;
  double ZZ = z[0];
  double mwt = MoleWt();
  m_dHRes = (beta/bb*log(1+B/ZZ)+R_c*T*(1-ZZ))/mwt; // Reduced Enthalpy, kJ/kMol
  phiV1 = ZZ - 1 - log(ZZ - B) - A / B * log(1 + B / ZZ);
  m_dSRes = R_c/mwt*phiV1 + m_dHRes/T ;
  phiL1 = in3RootRgn ? z[2] - 1 - log(z[2] - B) - A / B * log(1 + B / z[2]) : phiV1;
  double zz;
  for (int i=0; i<SpecieCount; i++) {
    if (in3RootRgn) {
      zz = z[2];
      phiL[i] = bib[i]*(zz-1)-log(zz-B)-A/B*(2*aiaQ[i]-bib[i])*log(1+B/zz);
      zz = z[0];
      phiV[i] = bib[i]*(zz-1)-log(zz-B)-A/B*(2*aiaQ[i]-bib[i])*log(1+B/zz);
    } else {
      zz = z[0];
      phiV[i] = phiL[i] = bib[i]*(zz-1)-log(zz-B)-A/B*(2*aiaQ[i]-bib[i])*log(1+B/zz);
    }
  }
  return z[0];
}







//---------------------------------------------------------------------------

double TTGasSM::get_msEnthalpy(long Phases, double T, double P, MArray *pMA)
  {
  MArray MA(pMA ? (*pMA) : this);
  return MSpModelBase::get_msEnthalpy(Phases, T, P, pMA) - (Phases & MP_Gas ? HRes(T, P) : 0.0);
  }

//---------------------------------------------------------------------------

double TTGasSM::get_msEntropy(long Phases, double T, double P, MArray *pMA)
  {
  return MSpModelBase::get_msEntropy(Phases, T, P, pMA) - (Phases & MP_Gas ? SRes(T, P) : 0.0);
  }

//---------------------------------------------------------------------------

double TTGasSM::get_msCp(long Phases, double T, double P, MArray *pMA) {
  MArray MA(pMA ? (*pMA) : this);
  
  double baseCp = MSpModelBase::get_msCp(Phases, T, P, pMA);
  if (Phases & MP_Gas) {
    baseCp -= (HRes(T+deltaT, P)-HRes(T, P))/deltaT;
  }
  return baseCp;
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
  idSeparator1,

  idGasDensity,
  idNormalDensity,
  idGasMWT,
  idGasZ,
  idHRes,
  idSRes,
  idGasCp,
  idGasViscosity,
  idGasConductivity,
  idThreeRootRegion,
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


    return d.result

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
    case idRqdMWT	    : Info.Set(ePT_Double,   "", "RqdMWT",     MC_, "", 0, 0,   MP_ConfigProp|MP_Parameter, "The required MWT"); return Inx;

    case idGasDensity: 
      Info.SetStructName("Props"); //NB: The "struct name" is used as part of tag! Recomend "Props" for compatability with other properties models.
      Info.SetText("--Gas Properties-----------");
      Info.Set(ePT_Double,    "", "GasDensity",MC_Conc, "g/L",    0, 0,  MP_RN,    "Gas Density @ T,P"); return Inx;


    case idSeparator1	    : Info.SetText("..."); return Inx;

    case idGasMWT :
      Info.Set(ePT_Double,    "", "GasMWT",MC_, "",    0, 0,  MP_RN,    "Molecular Wt"); return Inx;
      
    case idNormalDensity:
      Info.Set(ePT_Double,    "", "NormalDensity",MC_Conc, "g/L", 0, 0,  MP_RN, "Gas Density @1atm, 0C"); return Inx;
    case idGasZ:
      Info.Set(ePT_Double,  "Z", "Compressibility",MC_, "",  0, 0,  MP_RN,   "Gas Compressibility @T,P"); return Inx;
    case idHRes:
      Info.Set(ePT_Double,  "HRes", "Residual.Enthalpy",MC_HMs, "",  0, 0,  MP_RNH,   "Gas Residual Enthalpy @T,P"); return Inx;
    case idSRes:
      Info.Set(ePT_Double,  "SRes", "Residual.Entropy",MC_SMs, "",  0, 0,  MP_RNH,   "Gas Residual Entropy @T,P"); return Inx;

    case idGasCp:
      Info.Set(ePT_Double,    "", "SpecificHeat",MC_Conc, "g/L",    0, 0,  MP_RN,    "Specific Heat @T,P"); return Inx;
    case idGasViscosity:
      Info.Set(ePT_Double,    "", "Viscosity",MC_Conc, "g/L",    0, 0,  MP_RN,    "Viscosity @T,P"); return Inx;
    case idGasConductivity:
      Info.Set(ePT_Double,    "", "Conductivity",MC_Conc, "g/L",    0, 0,  MP_RN,    "Conductivity @T,P"); return Inx;
    case idThreeRootRegion:
      Info.Set(ePT_Bool,"", "ThreeRootRgn",    MC_, "",    0, 0,    MP_RN | MP_CheckBox, ""); return Inx;

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
      GVAL2(GasMWT, MoleWt());
      GVAL2(GasZ, GasZ(T, P));
      GVALFTP(HRes);
      GVALFTP(SRes);
      GVAL(RqdMWT);
      GVALFTP(GasDensity);
      GVAL2(NormalDensity, NormalDensity());
      GVAL2(ThreeRootRegion, in3RootRgn);
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


void Order(double *x)
{
  if (x[1]<x[2]) Exchange(x[1],x[2]);
		   if (x[0]<x[1]) {
		     Exchange(x[0],x[1]);
		     if (x[1]<x[2]) Exchange(x[1],x[2]);
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
/************
fugacity (self, t, p, phase=None):
        bib=[0.0]*self.nComp
        aiaQ=[0.0]*self.nComp
        c1=c2=0.0
        for i in range(self.nComp):
            g=self.gasList[i]
            tci= g.tc 
            pci= g.pc
            bib[i]=tci/pci
            m = 0.48 + g.wac * (1.574 - 0.176 * g.wac)
            alphaQ = (1 + m * (1 - sqrt(t/tci)))
            aiaQ[i]=tci*alphaQ/sqrt(pci)
            c1+=self.x[i]*aiaQ[i]
            c2+=self.x[i]*bib[i]
        A=0.42747*p/(t*t)*c1*c1
        B=0.08664*p/t*c2
        z=cubic(-1., A - B * (1 + B), -A * B)
        print z
        for i in range(0, self.nComp):
            bib[i]/=c2
            aiaQ[i]/=c1
        if phase:
            z=z[1][-1]
        else:
            z=z[1][0]
        result=[]
        for i in range(0, self.nComp):
            lnPHI=bib[i]*(z-1)-log(z-B)-A/B*(2*aiaQ[i]-bib[i])*log(1+B/z)
            result.append(exp(lnPHI))
        return result
******************/
