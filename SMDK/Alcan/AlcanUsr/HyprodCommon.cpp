//===========================================================================
//=== SysCAD SMDK - Hyprod Precipitator Model 2003 : Alcan, Kenwalt       ===
//===========================================================================

#include "stdafx.h"
#define  __HYPRODCOMMON_CPP
#include "HyprodCommon.h"
#if ForceOptimizeOff
#pragma optimize("", off)
#endif

inline long ROUND(double x) { return (long)(x+0.5); }
inline long cINT(double x) { return (long)x; }
inline double SQR(double x) { return x*x; }

static MInitialiseTest InitTest("HyprodPrecipHelper");
static MAqSpeciePtr spAlumina          (InitTest, "Al2O3(l)", false);
static MSpeciePtr   spWater            (InitTest, "H2O(l)", false);
static MSpeciePtr   spTHA              (InitTest, "Al2O3.3H2O(s)", false);
static MAqSpeciePtr spCausticSoda      (InitTest, "NaOH(l)", false);
static MSpeciePtr   spOccSoda          (InitTest, "Na2O(s)", false);
static MSpeciePtr   spOxalate          (InitTest, "Na2C2O4(l)", false);
static MSpeciePtr   spOxalateSPO       (InitTest, "Na2C2O4(s)", false);
//===========================================================================
//double HyprodPrecipHelper::m_dAmbientTemp=C2K(25.0);
//double HyprodPrecipHelper::m_dWindSpeed=10.0/3.6;//10 km/h = 2.7778m/s
bool HyprodPrecipHelper::sm_bCompletePopulation = 1;
bool HyprodPrecipHelper::sm_bUsePrevPSD = 0;
//===========================================================================

HyprodPrecipHelper::HyprodPrecipHelper()
  {
  NIntervals = 0;
  mv2 = 0;

  m_dNumberInService  = 1.0;
  m_dTempImpos        = C2K(65.0);
  m_dInTankCooling    = 0.0;
  m_dVolume           = 1500.0;
  m_dSurface          = 500.0;
  m_dKvFac            = 0.1;
  m_dUCoef            = 100.0;
  m_dLevel            = 1.0;
  m_eAgloModel        = eAgMdl_dlev;
  m_eHeatBalance	  = eHBal_Normal;	
  m_eNuclModel		  = eNucl_Hyprod;
  m_eShearRate		  = 500;

  m_dAgloParam[0]     = 60.0;     // Valeurs initiales, modifier si
  m_dAgloParam[1]     = 100.0;    // NbAgloModel change
  m_dAgloParam[2]     = 20.0;
  m_dAgloParam[3]     = 1.0;
  m_dAgloParam[4]     = 1.0;
  m_dAgloParam[5]     = 0.0002;

  m_dThermalLoss      = 0.0;
  m_dLoss2Ambient     = 0.0;
  m_dTempIn           = C2K(25);
  m_dTempOut          = m_dTempIn;
  m_dACIn             = 0.0;
  m_dACOut            = 0.0;
  m_dSolidConcIn      = 0.0;
  m_dSolidConcOut     = 0.0;
  m_dResTime          = 0.0;
  m_dYield            = 0.0;

  m_bPrevPSDUsed      = 0;
  }

//---------------------------------------------------------------------------

bool HyprodPrecipHelper::InitSizeData(MVector & Vec)
  {
  MIPSD & VecSz  = *Vec.FindIF<MIPSD>();
  NIntervals = VecSz.SizeCount;

  if (NIntervals>MaxHPSizeClasses)
    return false; //too many partitions!

  HPDSize = (VecSz.Size[1]-VecSz.Size[0])*1e6; //4.0
  HPSMin = Max(0.0, VecSz.Size[0]*1e6-HPDSize); //0.0
  //todo: check gap between ALL sizes is equal to HPDSize !

  for (long m=0; m<NIntervals; m++)
    SizeClass[m] = ((m==0) ? (VecSz.Size[m]/2.0) : ((VecSz.Size[m] + VecSz.Size[m-1])/2.0))*1e6; //  MID-POINT Sizes in microns

  return true;
  }

//---------------------------------------------------------------------------

void HyprodPrecipHelper::InitAgloData(eAgglomModel eAgloModel, double AgloParam)
  {
  long m,n;
  double SQ;
  mv2 = 1+ROUND(cINT(( SizeClass[NIntervals-1]/1.2599 -HPSMin)/HPDSize));
  for (m=0; m<mv2; m++)
    {
    for (n=m; n<NIntervals; n++)
      {
      SQ = (long)(pow( pow(SizeClass[m],3)+pow(SizeClass[n],3),1./3. )/HPDSize );
      Q[n][m] = ROUND(cINT((SQ-HPSMin/HPDSize)));
      if (Q[n][m]<NIntervals)
        {
        Vli[n][m] = ( pow(SizeClass[m],3) + pow(SizeClass[n],3)) / pow(SizeClass[Q[n][m]],3);
        switch (eAgloModel)
          {
          case eAgMdl_dlev:
            Kva[n][m]=8e-10/1.66*pow(SizeClass[n]+SizeClass[m], 0.1)
              /(1+0.28*pow(SizeClass[n]/AgloParam, 3))
              /(1+0.28*pow(SizeClass[m]/AgloParam, 3));
            break;
          case eAgMdl_LowAgit:
            Kva[n][m]=8e-10/1.66*0.21*pow(SizeClass[n]+SizeClass[m], 0.5)
              /(1+0.5*pow(SizeClass[n]/AgloParam, 3))
              /(1+0.5*pow(SizeClass[m]/AgloParam, 3));
			if (m== 0)  
				Kva[n][m]=Kva[n][m]/10;// Refinement introduced in 19 July 05 to explain 
			if (m == 1) 
				Kva[n][m]=Kva[n][m]/3;// the higher apparent nucleation rate in BATCH tanks
            break;
          case eAgMdl_HighAgit:
            Kva[n][m]=8e-10/1.66*pow(SizeClass[n]+SizeClass[m], 0.1)
              /(1+0.6*pow(SizeClass[n]/AgloParam, 3))
              /(1+0.6*pow(SizeClass[m]/AgloParam, 3));
            break;
          case eAgMdl_SizeInd:
            Kva[n][m] = 8e-10/1.66*0.5;
            break;
          case eAgMdl_EquiSize:
            if (n == m)
              Kva[n][m] = 8e-10/1.66*1.0;
            else
              Kva[n][m] = 0.0;
            break;
          case eAgMdl_Groneweg:
            Kva[n][m] = 8e-10/1.66*1.7*exp(-AgloParam*
              SizeClass[n]*(SizeClass[n]+1)) * exp(-AgloParam*SizeClass[m] * (SizeClass[m]+1));
            break;
          } //end of Case

        /*if (Kva[n][m]<2.0E-30)
          EnoughCollision[n][m]=false;
        else
          EnoughCollision[n][m]=true;*/
        }
      }
    }
  }

//---------------------------------------------------------------------------

double HyprodPrecipHelper::get_Na2OFactor(MVector & V)
  {
  const long PropIndex = V.Properties.Index("Na2OFactor");
  if (PropIndex>=0)
    return V.Properties[PropIndex].DValue();
  //Na2OFactor not found!  Use default
  return 1.37;
  }

  double HyprodPrecipHelper::get_NuclFactor(MVector & V)
  {
  const long PropIndex = V.Properties.Index("Nucleation");
  if (PropIndex>=0)
    return V.Properties[PropIndex].DValue();
  //Na2OFactor not found!  Use default
  return 1.37;
  }

//---------------------------------------------------------------------------

void HyprodPrecipHelper::ApplyAgglom(double GRate, double dTime, double KvFac, double ProdNNtl)
  {//variation of Particles due to Agglomeration by Collision
  long m,n;
  const double Consts = 9.0E-2 * 6e9 * 121.0 * (pow(3600.0, 1.27)/3600.0);
  const double Const_boucle2 = Consts * dTime * KvFac * pow(GRate, 1.27)/pow(ProdNNtl, 1.26);
  const long HPMStep=10;
  for (m=0; m<mv2; m++)
    {
    //for (n=m; n<(m+HPMStep); n++) KGA: 15/9/04 This causes a problem when n>=NIntervals!!!
    for (n=m; n<Min(m+HPMStep,NIntervals); n++)
      {
      if (Q[n][m]<NIntervals)
        {
        const double RA = (Kva[n][m]*ProdHyPsd[n]*ProdHyPsd[m]) * Const_boucle2;
        dPSD[m]       += -RA;
        dPSD[n]       += -RA;
        dPSD[Q[n][m]] += RA*Vli[n][m];
        }
      }
    }
  }

//---------------------------------------------------------------------------

double HyprodPrecipHelper::get_PrecipRate(MVector & V, double Ssurf)
  {
  const double T=V.T;
  MIBayer & VecB = *V.GetIF<MIBayer>();
  const double gpl=VecB.SolidsConc(C2K(25.0));
  const double GRate=VecB.GrowthRate();
  // the GTZ enforces the signs if growth goes to Zero
  return - GTZ(GRate)*THADens/1000.0/2.0/(spTHA.MW/spAlumina.MW) *Ssurf* gpl/**(1.0 / (1.0 - gpl/THADens))*/;
  }
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

double HyprodPrecipHelper::get_NucleationRate(eNuclModel eNucleation,MVector & V, double Ssurf, double ShRate)
  {
  const double T =V.T;
  double nuclRate =0;
  MIBayer & VecB = *V.GetIF<MIBayer>();
  const double NuclFact = get_NuclFactor(V);
  const double HypNuclModel =VecB.NucleationRate(Ssurf);// !!the original Hyprod Nucleation Model
  const double gpl=VecB.SolidsConc(C2K(25.0));
  const double SuperGPL  = VecB.AluminaConc(C2K(25.0))- VecB.AluminaConcSat(T);
  const double RelSuper  = VecB.AluminaConc(C2K(25.0))/VecB.AluminaConcSat(T) - 1 ;
  switch (eNucleation)
          {
          case eNucl_Hyprod:
          nuclRate = HypNuclModel;
          break;
		  case eNucl_NBrown:
		  nuclRate =NuclFact* 0.0009 *exp(21000/(1.987*T))*SuperGPL
			  *Pow(10, (11.67-0.000301*Sqr(T-273) -0.76*Pow((gpl*Ssurf),0.3)  -4.8*log(SuperGPL) )) /3600/1000000; 
          break;
		  case eNucl_LowShearRate:
		  nuclRate = NuclFact*1.80e12 *Pow(ShRate,2.25) * Pow(RelSuper,2.4) * gpl *exp(-102/8.314*1000/T);
          break;
		  case eNucl_HighShearRate:
		  nuclRate = NuclFact*1.58e24 *Pow(ShRate,2.25) * Pow(RelSuper,4.7) * gpl *exp(-180/8.314*1000/T);
          break;
		  case eNucl_LowSolidsConc:
     	  nuclRate = NuclFact*4.39e72 * Pow(ShRate,2.25) * Pow(RelSuper,17) * gpl *exp(-500/8.314*1000/T);       
          break;
		  case eNucl_Hybrid:
		  nuclRate = 1.58e24 * Pow(ShRate,2.25) * Pow(RelSuper,4.7) * gpl *exp(-180/8.314*1000/T);
		  nuclRate  *= 4.39e72 *Pow(ShRate,2.25) *Pow(RelSuper,17) * gpl *exp(-500/8.314*1000/T); 
		  nuclRate = Sqrt(nuclRate);
          break;
		  }

  return nuclRate;
  }

//---------------------------------------------------------------------------

int HyprodPrecipHelper::PrecipSS(MVector & Feed, MVector & Prod, MStream & QProdPrev)
  {
  double T0;
  double CAx1, CAx2;
  double PrecipRate1, Err2, ResTime, ProdVolFlow;

  const double Na2OFac = get_Na2OFactor(Feed);
  T0=Prod.T;
  MIBayer & FeedB=*Feed.GetIF<MIBayer>();
  MIBayer & ProdB=*Prod.GetIF<MIBayer>();
  MIBayer & PrevProdB=*QProdPrev.FindIF<MIBayer>();
  const bool PrevValid = !IsNothing(PrevProdB);
  MISSA & ProdSSA=*Prod.GetIF<MISSA>();
  if (FeedB.SolidsConc(C2K(25.0))<1e-12)
    {
    return -1;//Log.SetCondition(6, MMsg_Error, "PrecipSS Failed - No Solids");
    }
  if (FeedB.CausticConc(C2K(25.0))<1e-12)
    {
    return -2;//Log.SetCondition(6, MMsg_Error, "PrecipSS Failed - No Caustic");
    }

  double Sx;
  if (!sm_bCompletePopulation && sm_bUsePrevPSD)
    {
    MISSA & PrevSSA=*QProdPrev.FindIF<MISSA>();// this is the SSA of the last Popul run to use in "NO POpul mode"
    if (IsNothing(PrevSSA))
      {
	    m_bPrevPSDUsed = 0;
      Sx=ProdSSA.SpecificSurfaceAreaMass(); //m^2/g//PROBLEM!!! Prev does not have SSA, use feed
      }
    else
      {
      m_bPrevPSDUsed = 1;
      Sx=PrevSSA.SpecificSurfaceAreaMass(); //m^2/g
      //if (PrevValid)
      //  T0=m_QProd.T;
      }
    }
  else
    {
	  m_bPrevPSDUsed = 0;
    Sx= m_dInTankSSA ; // Spec Surface that is manually entered by user m^2/g
    }

  Sx=Range(0.020, Sx, 0.085);
  
  double CAlum0  = ProdB.AluminaConc(C2K(25.0));// changed 18/3/05 to match A/C with PSD mode was ProdB.AluminaConc(C2K(25.0))
  double gpl0    = ProdB.SolidsConc(C2K(25.0)); 
  double CEq0    = ProdB.AluminaConcSat(T0);

  
  ProdVolFlow = Prod.Volume(); //m^3/s
  ResTime = m_dVolume * m_dLevel / ProdVolFlow; //seconds

  if (CAlum0 >= CEq0)  // Il y a forcément précipitation
    {
    //CAx1 = CAlum0 - (CAlum0 - CEq0)/3.0;       // Hypothèses de départ
    CAx2 = CAlum0 - (CAlum0 - CEq0)/5.0;
    const int MaxCount = 2000;
    PrecipRate1 = get_PrecipRate(Prod, Sx);
    double damping = 0.5;
    //damping = ( CEq0 - CAlum0 -PrecipRate1 * ResTime) / (CAx2 -CAlum0  -PrecipRate1 * ResTime);
    if (fabs(PrecipRate1) > 0.01)
		  damping = 0.95;
    int count = 0;
    do
      {
      bool CErr;
      double Ain1   = ProdB.AluminaConc(C2K(25.0));// changed 18/3/05 to match A/C with PSD mode ProdB.AluminaConc(C2K(25.0));
      double Cin1   = ProdB.CausticConc(C2K(25.0)); // "  "     ProdB.CausticConc(C2K(25.0));
      double ASat1  = ProdB.AluminaConcSat(T0);
      double gpl1;
      double SSat1  = (Ain1-ASat1)/GTZ(Cin1);
      double NoPerSec1 = ProdSSA.PartNumPerSec();
      PrecipRate1 = get_PrecipRate(Prod, Sx)*1.075; // FIXED factor to take into account that in POP mode nucleation always create higher prec rate
      CAx1 = (1.0-damping) * (CAlum0 + PrecipRate1 * ResTime) + damping*CAx2;
      if (CAx1 < CEq0) 
        CAx1 = CEq0 + 1 + count;// this is to prevent the case when the Alumina concentration would be too low on the first iteration
      //original!!  if (MassBalance(Prod, T0, CAx1, dNAN, NoPerSec1, Na2OFac, SSat1, CErr)!=0)
  	  gpl1 = gpl0 + (CAlum0 - CAx1)*spTHA.MW/spAlumina.MW;
      if (MassBalance(Prod, dNAN, dNAN, gpl1, NoPerSec1, Na2OFac, SSat1, CErr)!=0)
        {
        int xx=0;//maybe panic ??
        }

	    HeatBalance(Feed, Prod, T0);  //Try this???

      T0=Prod.T;
      ProdVolFlow = Prod.Volume(); //m^3/s

      ResTime = m_dVolume * m_dLevel / ProdVolFlow; // required to REevaluate the Residence time as the temperature CHANGES.. diffent volumetric flow!!
      Err2=fabs(CAx1-CAx2);
      CAx2=CAx1;
      count++;
      }
    while ((fabs(Err2) > 0.001) && (count<MaxCount));
    m_dResTime = ResTime;
	if (count==MaxCount)
      return -4;//Log.SetCondition(6, MMsg_Error, "PrecipSS Failed - Infinite solve loop");

    // contents of Prod are now 'At Equilrium'
    }    // CAlum0 > CEq
  else
    {
    return -3;//Log.Message(MMsg_Note, "Pas Precipitation");
    }

  return 0;
  }

//---------------------------------------------------------------------------

int HyprodPrecipHelper::MassBalance(MVector & Vec, double TRqd, double ARqd, double THARqd, double NoPerSec, double Na2OFac, double SSat, bool & ConvergeErr)
  {
  //   2 AlO2 + 4 H2O <==> Al2O3.3H2O + 2 OH
  //or Na2O + Al2O3 + 4 H2O <==> Al2O3.3H2O + 2 NaOH

  // THADelta is the Fraction of Alumina which precipitates as the hydrate
  // ie THADelta is deposition rate of new THA crystal

  const double ESoda=2335.0; //constant is  2335K and NOT 2535 19 May DRA: I just found another mistake 

  bool AdjustT=!Valid(TRqd);
  bool SetTHA=Valid(THARqd);
  
  double T=AdjustT? Vec.T : TRqd;

  double &AluminaMass  = Vec.MassVector[spAlumina];     // Al2O3
  double &WaterMass    = Vec.MassVector[spWater];       // H2O
  double &THAMass      = Vec.MassVector[spTHA];         // Al2O3.3H2O
  double &CausticMass  = Vec.MassVector[spCausticSoda]; // NaOH
  double &Na2OMass     = Vec.MassVector[spOccSoda];     // Na2O
  double &OxLiqMass    = Vec.MassVector[spOxalate];     // Na2C2O4(l)
  double &OxSolMass    = Vec.MassVector[spOxalateSPO];  // Na2C2O4(s)

  double OxFrac = 0.00;//TODO FIX THIS
  OxLiqMass    = OxLiqMass-(OxLiqMass*OxFrac);
  OxSolMass    = OxSolMass+(OxLiqMass*OxFrac); 

  const double Fact = spAlumina.MW/spTHA.MW; // 0.654;
  MIBayer & BVec=*Vec.GetIF<MIBayer>();

  for (int Iter=100; Iter; Iter--)
    {
    if (AdjustT)
      T=Vec.T;
	double CC= BVec.CausticConc(C2K(25.0));
    double A=BVec.AluminaConc(C2K(25.0));
    double THA=BVec.SolidsConc(C2K(25.0));
    double THADelta;
    if (SetTHA)
      {
      if (fabs(THA-THARqd)<1.0e-12*THARqd)
        break;
      THADelta = THAMass*(THARqd/GTZ(THA)-1.);
      }
    else
      {
      if (fabs(A-ARqd)<1.0e-12*ARqd)
        break;
      THADelta = AluminaMass*(1.0-ARqd/GTZ(A))/Fact;
      }
	

	double alumSSat = CC *SSat;
    const double LclVar = Na2OFac*7.1868e-8*exp(ESoda/T)*Pow(alumSSat, 1.0955);// 19 May DRA: I just found another mistake
    double XSoda = THADelta*LclVar;

    double dAluminaMass = - Fact*THADelta;
    double dTHAMass     = + THADelta;
    double dWaterMass   = - (1.0-Fact)*THADelta + spWater.MW/spOccSoda.MW*XSoda;
    double dCausticMass = - 2*spCausticSoda.MW/spOccSoda.MW*XSoda;
    double dNa2OMass    = + XSoda;
    double Scl;

    for(;;)
      {
      Scl=1.0;
      if (AluminaMass + Scl*dAluminaMass <0.0)
        Scl=Min(Scl, 1.0-(AluminaMass + Scl*dAluminaMass)/NZ(dAluminaMass));
      if (THAMass     + Scl*dTHAMass     <0.0)
        Scl=Min(Scl, 1.0-(THAMass + Scl*dTHAMass)/NZ(dTHAMass));
      if (WaterMass   + Scl*dWaterMass   <0.0)
        Scl=Min(Scl, 1.0-(WaterMass + Scl*dWaterMass)/NZ(dWaterMass));
      if (CausticMass + Scl*dCausticMass <0.0)
        Scl=Min(Scl, 1.0-(CausticMass + Scl*dCausticMass)/NZ(dCausticMass));
      if (Na2OMass    + Scl*dNa2OMass    <0.0)
        Scl=Min(Scl, 1.0-(Na2OMass + Scl*dNa2OMass)/NZ(dNa2OMass));
      
      if (Scl<(1.0-1e-12))
        {
        dAluminaMass *= Scl; 
        dTHAMass     *= Scl; 
        dWaterMass   *= Scl; 
        dCausticMass *= Scl; 
        dNa2OMass    *= Scl; 
        }
      else
        break;
      }
    
    if (fabs(dAluminaMass)<1e-22)
      {//problem!!!
      Iter=0;
      break;
      }

    //adjust masses...
    AluminaMass = AluminaMass + dAluminaMass;
    THAMass     = THAMass     + dTHAMass;    
    WaterMass   = WaterMass   + dWaterMass;  
    CausticMass = CausticMass + dCausticMass;
    Na2OMass    = Na2OMass    + dNa2OMass;   

    Vec.MarkStateChanged(); //this forces recalculation of temperature / enthalpy based on new stream makeup
    }

  ConvergeErr = (Iter==0);

  MISSA & VecSSA=*Vec.GetIF<MISSA>();
  if (NoPerSec>0.0 && !IsNothing(VecSSA))
    {
    VecSSA.SetSAMFromFlow(BVec.THAMassFlow(), NoPerSec);
    }
  return (ConvergeErr ? -1 : 0);
  }

//---------------------------------------------------------------------------

void HyprodPrecipHelper::HeatBalance(MVector & Feed, MVector & Prod, double T)
  {
  const double TFeed=Feed.T;
  const double PFeed=Feed.P;
  //const double CpFeed=Feed.Properties["totCp"].DValue(MP_All, TFeed, PFeed);
  const double CpFeed=Feed.totCp(MP_All, TFeed, PFeed);
  const double enthIn = CpFeed * K2C(TFeed); // Enthalpie IN
  //const double ProdMassFlow = Prod.Mass(MP_All);
  const double SldFeed = Feed.Mass(MP_Sol);
  const double SldProd = Prod.Mass(MP_Sol);
  const double SolidsVariation = SldProd - SldFeed;
  DoHeatBalance(Prod, enthIn, T, SolidsVariation, 1.0, 1.0);
  }

//---------------------------------------------------------------------------

void HyprodPrecipHelper::HeatBalance(MVector & Prod, double SolidsVariation, double enthIn, double dTime, double CurLevel)
  {
  const double T = Prod.T; 
  DoHeatBalance(Prod, enthIn, T, SolidsVariation, dTime, CurLevel);
  }

//---------------------------------------------------------------------------

void HyprodPrecipHelper::DoHeatBalance(MVector & Prod, double enthIn, double T, double SolidsVariation, double dTime, double CurLevel)
  {
  //const double TProd=Prod.T;
  const double PProd=Prod.P;
  //NB: msCp * MassFlow = totCp
  const double CpProd = Prod.totCp(MP_All, T, PProd);

  if (!m_eHeatBalance == eHBal_Normal)
    T = m_dTempImpos;   //force fixed temperature
  const double Windspeed = WindSpeed()*3.6; //convert to km/h
  const double AmbientT = AmbientTemp();
  double U = 4.88* (1.45E-8* (pow(T-0.016, 4)-pow(AmbientT-0.16, 4) )
           +0.617*pow(T-AmbientT, 1.25)
           *Sqrt(Windspeed*88.0/69.0 + 1.0))
              / (T-AmbientT) /1.8;
  U *= 0.0011622222; //convert from kcal/h.m^2.C to kW/m^2.C
  U = 1.0 / ( 1.0/U + 1.0/GTZ(m_dUCoef));

  const double SurfaceT = m_dSurface * CurLevel * m_dNumberInService;
  m_dLoss2Ambient = U*SurfaceT*(T-AmbientT);

  const double HydDiff = SolidsVariation*102./156.;
  if (! m_eHeatBalance == eHBal_Normal)
    {//use fixed imposed temperature
    m_dInTankCooling = 0.0;
    Prod.SetTP(T, PProd);
    }
  else
    {
    //kJ/kg.K 4.184
    //heat of reaction = 145 kCal/kg
    double TOut = (HydDiff*145.0*4.184 - m_dLoss2Ambient*dTime - m_dInTankCooling*dTime + enthIn) / CpProd;
    Prod.SetTP(C2K(TOut), PProd);
    }
  }

//---------------------------------------------------------------------------

bool HyprodPrecipHelper::SetPSDfromPrev(MVector & V, MStream & QProdPrev)
  {
  MIPSD & ProdSz = *V.FindIF<MIPSD>();
  MIPSD & PrevSz = *QProdPrev.FindIF<MIPSD>();
  if (!IsNothing(ProdSz) && !IsNothing(PrevSz))
    {//Force ProdPSD = previous product PSD
    //ProdSz = PrevSz; //doesn't work, Kenwalt to fix
    double *PrevPSD=PrevSz.getFracVector(0);
    ProdSz.ReplaceFracVector(PrevPSD, 0);
    m_bPrevPSDUsed = 1;
    return true;
    }
  return false;
  }
//---------------------------------------------------------------------------

void HyprodPrecipHelper::MassFrac2HyprodDist( double SizeClass[],
                          double MassFrac[],
                          double PS[],
                          long NIntervals,
                          double &SSurf)
  {
  const double WFact  = PI/6*2.42e-12 ; //      { = Pi/6 * 2.42 g/cm3 * 10^-12 cm3/µm3}
  const double SFact  = PI*1e-8;        //      { = Pi * 10^-8 cm2/µm2 }
  const double WSFact = WFact/SFact;    //      { = WFact / SFact }

  double SS, WS;

  double SCumul = 0.0;
  for (long i=0; i<NIntervals; i++)
    {
    WS = MassFrac[i];
    PS[i] = WS / pow(SizeClass[i], 3) / WFact;
    SS = WS / SizeClass[i] / WSFact;
    SCumul += SS;                     // surface en cm2/g
    }
  SSurf = SCumul/10000.0;
  }

//---------------------------------------------------------------------------

void HyprodPrecipHelper::HyprodDist2MassFrac(double SizeClass[],
                         double PS[],
                         double MassFrac[],
                         long NIntervals)
  {
  double Tot=0.0;
  for (long i=0; i<NIntervals; i++)
    {
    MassFrac[i] = PS[i] * (PI/6) * pow(SizeClass[i],3);  // conversion en poids
    Tot+=MassFrac[i];
    }

  #if ForceOptimizeOff
  //check for errors if debug is on
  if (Tot<0.0)
    throw MMdlException(0,"HyprodDist2MassFrac:Negative sum of total PSD mass!"); 
  for (i=0; i<NIntervals; i++)
    if (MassFrac[i]<0.0)
      {
      throw MMdlException(0,"HyprodDist2MassFrac:Negative PSD!"); 
      break;
      }
  #endif

  //convert to fraction...
  Tot=GTZ(Tot);
  for (i=0; i<NIntervals; i++)
    MassFrac[i]/=Tot;
  }

//---------------------------------------------------------------------------

void HyprodPrecipHelper::CalcSSurgNNtl(double SizeClass[],
                   double HyPsd[],
                   long NIntervals,
                   double &SSurf,
                   double &NNtl)
  {
  //NB for calculation of NNTtl HyPsd MUST be in actual conc (not per 1 gpl)
  double M=0.0; 
  double NParTot=0.0;
  double S=0.0;
  for (long n=0; n<NIntervals; n++)
    {
    M       += HyPsd[n] * pow(SizeClass[n],3);
    NParTot += HyPsd[n];
    S       += Sqr(SizeClass[n])*HyPsd[n];
    }
  M = M*PI/6*2.42e-12;
  SSurf = S*PI/GTZ(M*1e12);
  NNtl = NParTot;
  }

//---------------------------------------------------------------------------
//===========================================================================
