//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __NOISE_CPP
#include "sc_defs.h"
#include "vectors.h"
#include "noise.h"

#define DBG_RANDOM 0

//===========================================================================

CRandom GlobalRnd;

//===========================================================================

CRandom::CRandom()
  {
  //idum = -13; //seed
  srand( (unsigned)time( NULL ) );
  idum = rand()*-1; //seed
  iy = 0;
  }

//---------------------------------------------------------------------------

CRandom::CRandom(long seed)
  {
  idum = abs(seed) * -1; 
  iy = 0;
  }

//---------------------------------------------------------------------------

#define IA 16807
#define IM 2147483647
#define AM (1.0/IM)
#define IQ 127773
#define IR 2836
#define NTAB 32
#define NDIV (1+(IM-1)/NTAB)
#define EPS 1.2e-7
#define RNMX (1.0-EPS)

double CRandom::Rnd()
  {//see Numerical Recipes for c (2nd Edition)- function: ran1(long* idum)
  int j;
  long k;
  //static long iy=0;
  //static long iv[NTAB];
  double temp;
  if (idum <= 0 || !iy) 
    {
    if (-(idum) < 1)
      idum = 1;
    else 
      idum = -(idum);
    for (j=NTAB+7;j>=0;j--) 
      {
      k = (idum)/IQ;
      idum = IA*(idum-k*IQ)-IR*k;
      if (idum < 0) 
        idum += IM;
      if (j < NTAB) 
        iv[j] = idum;
      }
    iy = iv[0];
    }
  k = (idum)/IQ;
  idum = IA*(idum-k*IQ)-IR*k;
  if (idum < 0) 
    idum += IM;
  j = iy/NDIV;
  iy = iv[j];
  iv[j] = idum;
  if ((temp = AM*iy) > RNMX) 
    return RNMX;
  else 
    return temp;
  }
#undef IA
#undef IM
#undef AM
#undef IQ
#undef IR
#undef NTAB
#undef NDIV
#undef EPS
#undef RNMX

//===========================================================================

CBaseNoise::CBaseNoise(pCNoise pNoise) 
  : dMean(pNoise->m_dMean),
    dOutput(pNoise->m_dOutput),
    dG_StdDev(pNoise->m_dStdDev)
  {
  CommonConstruct();
  }

//---------------------------------------------------------------------------

//CBaseNoise::CBaseNoise(pGCClassVar pClassVar)
//  : dMean(((pGCDoubleVar)pClassVar->GetVarByName("Mean"))->m_var),  
//    dOutput(((pGCDoubleVar)pClassVar->GetVarByName("Output"))->m_var),  
//    dG_StdDev(((pGCDoubleVar)pClassVar->GetVarByName("StdDev"))->m_var)
//  {
//  CommonConstruct();
//  }
 
//---------------------------------------------------------------------------

CBaseNoise::CBaseNoise(double& Mean, double& StdDev, double& Output)
  : dMean(Mean),  
    dOutput(Output),  
    dG_StdDev(StdDev)
  {
  CommonConstruct();
  }

//---------------------------------------------------------------------------

CBaseNoise::~CBaseNoise()
  {
  if (pRnd!=&GlobalRnd)
    delete pRnd;
  }

//---------------------------------------------------------------------------

void CBaseNoise::CommonConstruct()
  {
  eNoiseType = NS_Gaus1;
  bG_iset = 0;
  dG_gset = 0.0;
  dP_sq = -1.0;  
  dP_alxm = -1.0;
  dP_g = -1.0;   
  dP_oldm = -1.0;
  dOutput = 0.0;
  dMean = 0.0;
  dG_StdDev = 1.0;
  pRnd = &GlobalRnd;
  }

//---------------------------------------------------------------------------

void CBaseNoise::SetGlobalNoiseSeed(long seed)
  {
  GlobalRnd.SetSeed(seed);
  }

//---------------------------------------------------------------------------

void CBaseNoise::SetGlobal(BOOL UseGlobal)
  {
  if (UseGlobal)
    {
    if (pRnd!=&GlobalRnd)
      delete pRnd;
    pRnd = &GlobalRnd;
    }
  else
    {
    if (pRnd==&GlobalRnd)
      pRnd = new CRandom;
    }
  }

//---------------------------------------------------------------------------

flag CBaseNoise::IsGlobal() 
  { 
  return (pRnd==&GlobalRnd); 
  };

//---------------------------------------------------------------------------

void CBaseNoise::SetType(NoiseTypes NoiseType) 
  { 
  eNoiseType = NoiseType;
  }

//---------------------------------------------------------------------------

void CBaseNoise::SetSeed(long seed)
  {//NB calling SetGlobal after this may throw away results
  switch (eNoiseType)
    {
    case NS_Gaus1:
    case NS_Flat1:
    case NS_Pois1:
    case NS_Gamma1:
    case NS_Weibull1:
      pRnd->SetSeed(seed);
      break;
    case NS_Gaus:
    case NS_Flat:
    case NS_Pois:
    case NS_Gamma:
    case NS_Weibull:
      SetSystemSeed(seed);
      break;
    }
  bG_iset = 0;
  dG_gset = 0.0;
  dP_sq = -1.0;  
  dP_alxm = -1.0;
  dP_g = -1.0;   
  dP_oldm = -1.0;
  }

//---------------------------------------------------------------------------

double CBaseNoise::GetVal()
  {
  switch (eNoiseType)
    {
    case NS_Gaus1:
    case NS_Gaus:
      {
      dOutput = dMean + (RndGaus() * dG_StdDev);
      break;
      }
    case NS_Flat1:
      dOutput = dMean + (((pRnd->Rnd() * 2.0) - 1.0) * dG_StdDev);
      break;
    case NS_Flat:
      dOutput = dMean + (((SystemRnd() * 2.0) - 1.0) * dG_StdDev);
      break;
    case NS_Pois1:
    case NS_Pois:
      {
      dOutput = RndPoisson();
      break;
      }
    case NS_Gamma1:
    case NS_Gamma:
      {
      dOutput = RndGamma();
      break;
      }
    case NS_Weibull1:
    case NS_Weibull:
      {
      dOutput = RndWeibull();
      break;
      }
    default:
      ASSERT(FALSE); //noise type not coded !!!
      dOutput = 0.0;
      break;
    }
  return dOutput;
  }

//---------------------------------------------------------------------------

//returns a number roughly between -4 and 4
//standard deviation is 1.0
double CBaseNoise::RndGaus()
  {//see Numerical Recipes for c (2nd Edition) - function: gasdev(long *idum)
  //static int bG_iset=0;
  //static float dG_gset;
  double fac,rsq,v1,v2;

  if (bG_iset == 0) 
    {
    do
      {
      if (eNoiseType==NS_Gaus)
        {
        v1 = 2.0*SystemRnd()-1.0;
        v2 = 2.0*SystemRnd()-1.0;
        }
      else
        {
        v1 = 2.0*pRnd->Rnd()-1.0;
        v2 = 2.0*pRnd->Rnd()-1.0;
        }
      rsq = v1*v1 + v2*v2;
      } while (rsq >= 1.0 || rsq == 0.0);
    fac = sqrt(-2.0 * log(rsq) / rsq);
    dG_gset = v1*fac;
    bG_iset = 1;
    return v2*fac;
    }
  else 
    {
    bG_iset = 0;
    return dG_gset;
    }
  }

//---------------------------------------------------------------------------
//logarithm of gamma function
double CBaseNoise::gammln(double xx)
  {//see Numerical Recipes for c (2nd Edition) - function: gammln(float xx)
  double x,y,tmp,ser;
  static double cof[6] = 
    { 76.18009172947146,    -86.50532032941677,
      24.01409824083091,     -1.231739572450155,
      0.1208650973866179e-2, -0.5395239384953e-5 };
  int j;

  y = x = xx;
  tmp = x+5.5;
  tmp -= (x+0.5)*log(tmp);
  ser = 1.000000000190015;
  for (j=0; j<=5; j++) 
    ser += cof[j]/++y;
  return -tmp+log(2.5066282746310005*ser/x);
  }

//---------------------------------------------------------------------------

double CBaseNoise::RndPoisson()
  {//see Numerical Recipes for c (2nd Edition) - function: poidev(float xm, long *idum)
  //static double sq,alxm,g,oldm=(-1.0);
  double em,t,y;

  if (dMean < 12.0) 
    {
    if (dMean != dP_oldm) 
      {
      dP_oldm = dMean;
      dP_g = exp(-dMean);
      }
    em = -1;
    t = 1.0;
    if (eNoiseType==NS_Pois)
      {
      do 
        {
        ++em;
        t *= SystemRnd();
        } while (t > dP_g);
      }
    else
      {
      do 
        {
        ++em;
        t *= pRnd->Rnd();
        } while (t > dP_g);
      }
    }
  else 
    {
    if (dMean != dP_oldm)
      {
      dP_oldm = dMean;
      dP_sq = sqrt(2.0*dMean);
      dP_alxm = log(dMean);
      dP_g = dMean*dP_alxm-gammln(dMean+1.0);
      }
    if (eNoiseType==NS_Pois)
      {
      do
        {
        do 
          {
          y = tan(PI*SystemRnd());
          em = dP_sq*y+dMean;
          } while (em < 0.0);
        em = floor(em);
        t = 0.9*(1.0+y*y)*exp(em*dP_alxm-gammln(em+1.0)-dP_g);
        } while (SystemRnd() > t);
      }
    else
      {
      do
        {
        do 
          {
          y = tan(PI*pRnd->Rnd());
          em = dP_sq*y+dMean;
          } while (em < 0.0);
        em = floor(em);
        t = 0.9*(1.0+y*y)*exp(em*dP_alxm-gammln(em+1.0)-dP_g);
        } while (pRnd->Rnd() > t);
      }
    }
  return em;
  }

//---------------------------------------------------------------------------

//gamma-law distribution random deviates
double CBaseNoise::RndGamma()
  {//see Numerical Recipes for c (2nd Edition) - function: gamdev(int ia, long *idum)
  int j,ia;
  double am,e,s,v1,v2,x,y;
  ia = (int)dMean;
  if (ia<1)
    ia = 1;
  if (ia < 6)
    {
    x = 1.0;
    if (eNoiseType==NS_Gamma)
      {
      for (j=1; j<=ia; j++)
        x *= SystemRnd();
      }
    else
      {
      for (j=1; j<=ia; j++)
        x *= pRnd->Rnd();
      }
    x = -log(x);
    } 
  else 
    {
    if (eNoiseType==NS_Gamma)
      {
      do
        {
        do 
          {
          do 
            {
            v1 = SystemRnd();
            v2 = 2.0 * SystemRnd() - 1.0;
            } while (v1*v1+v2*v2 > 1.0);
          y = v2/v1;
          am = ia-1;
          s = sqrt(2.0*am+1.0);
          x = s*y+am;
          } while (x <= 0.0);
        e = (1.0+y*y) * exp(am*log(x/am)-s*y);
        } while (SystemRnd() > e);
      }
    else
      {
      do
        {
        do 
          {
          do 
            {
            v1 = pRnd->Rnd();
            v2 = 2.0 * pRnd->Rnd() - 1.0;
            } while (v1*v1+v2*v2 > 1.0);
          y = v2/v1;
          am = ia-1;
          s = sqrt(2.0*am+1.0);
          x = s*y+am;
          } while (x <= 0.0);
        e = (1.0+y*y) * exp(am*log(x/am)-s*y);
        } while (pRnd->Rnd() > e);
      }
    }
  return x;
  }

//---------------------------------------------------------------------------

double CBaseNoise::RndWeibull()
  {//from paper from Data Analysis Australia (from Argyle Diamond Mines) Sept 1997
  //Weibull Distributions; the probability density function is given by:
  //  f(t) = a^-b * b * t^(b-1) * exp(-(t/a)^b)
  // A Weibull variable can be expressed as:
  //  T = a * [ (-ln(U))^(1/b) ]   where U is a uniform random variable between 0 and 1

  //use dMean for a and dStdDev for b !!!
  const double a = dMean;
  const double b = dG_StdDev;
  ASSERT(b>0.0);
  const double r = (eNoiseType==NS_Weibull) ? SystemRnd() : pRnd->Rnd();
  return a * pow(-1.0 * log(r), 1.0 / b);
  }

//---------------------------------------------------------------------------

void CBaseNoise::TestGaus(int N, int NPts)
  {
  #if DBG_RANDOM
  N = (int)(N/2)*2;
  int N2 = N/2;
  int* dist = new int[N+1];

  for (int j=0; j<=N; j++)
    dist[j] = 0;
  for (int i=0; i<=NPts; i++)
    {
    j = (int)((0.51 + 0.1*RndGaus())*N);
    //j = (int)(0.5+0.2*N*RndGaus()) + N2;
    if ((j>=0) && (j<=N))
      dist[j]++;
    }

  double dd = RndGaus();
  double Max = dd;
  double Min = dd;
  double Ttl = dd;
  for (j=0; j<NPts; j++)
    {
    dd = RndGaus();
    Ttl += dd;
    if (dd<Min)
      Min = dd;
    if (dd>Max)
      Max = dd;
    }

  dbgpln("=========== Gaussian Random generator test ===========");
  dbgpln("=== Ttl:%8.4f  Min:%8.4f  Max:%8.4f", Ttl, Min, Max);
  dbgpln("=== Normally distributed deviate of %d points over %d bins", NPts, N+1);
  const int LLen = 50;
  //int iscal = (int)(NPts*(LLen+1)/dist[N2]);
  int iscal = 32555;
  for (j=0; j<=N; j++)
    if (dist[j]>0 && (int)(NPts*(LLen+1)/dist[j])<iscal)
      iscal = (int)(NPts*(LLen+1)/dist[j]);
  for (j=0; j<=N; j++)
    {
    dd = (double)dist[j]/NPts;
    int klim = (int)(iscal * dd);
    if (klim>LLen)
      klim = LLen;
    dbgp("=== %2d : %4d | %8.4f : %8.4f  ", j, dist[j], j/(0.25*N), dd);
    for (i=0; i<klim; i++)
      dbgp("*");
    dbgpln("");
    }
  delete []dist;
  #endif
  }

//---------------------------------------------------------------------------

void CBaseNoise::TestPoisson(int N, int NPts)
  {
  #if DBG_RANDOM
  N = (int)(N/2)*2;
  //int N2 = N/2;
  int* dist = new int[N+1];
  int i,j,k,klim;

  for (j=0; j<=N; j++) 
    dist[j] = 0;
  ASSERT(dMean>0.0);
  ASSERT(dMean<N);
  for (i=1; i<=NPts; i++) 
    {
    j = (int)(0.5+RndPoisson());
    if ((j >= 0) && (j <= N)) 
      ++dist[j];
    }

  double dd = RndPoisson();
  double Max = dd;
  double Min = dd;
  double Ttl = dd;
  for (j=0; j<NPts; j++)
    {
    dd = RndPoisson();
    Ttl += dd;
    if (dd<Min)
      Min = dd;
    if (dd>Max)
      Max = dd;
    }
  dbgpln("=========== Poisson Random generator test ===========");
  dbgpln("=== Ttl:%8.4f  Min:%8.4g  Max:%8.4g  Ave:%8.4f", Ttl, Min, Max, Ttl/(NPts+1));
  dbgpln("=== Poisson distributed deviate of %d points over %d bins", NPts, N+1);
  dbgpln("=== Mean:%8.4f", dMean);
  dbgpln("===  x :      %8s %10s", "p(x)", "graph:");
  const int LLen = 50;
  int iscal = 200;
  int sum = 0;
  for (j=0; j<=N; j++)
    {
    sum += dist[j];
    dd = (double)dist[j]/NPts;
    klim = (int)(iscal*dd);
    if (klim > LLen)
      klim = LLen;
    dbgp("=== %2d : %4d %8.4f ", j, dist[j], dd);
    for (k=1; k<=klim; k++)
      dbgp("*");
    dbgpln("   (%d)", sum);
    }
  delete []dist;
  #endif
  }

//---------------------------------------------------------------------------

void CBaseNoise::TestGamma(int N, int NPts)
  {
  #if DBG_RANDOM
  N = (int)(N/2)*2;
  //int N2 = N/2;
  int* dist = new int[N+1];
  int i,j,k,klim;

  for (j=0; j<=N; j++) 
    dist[j] = 0;
  ASSERT(dMean>1.0);
  ASSERT(dMean<N);
  for (i=1; i<=NPts; i++) 
    {
    j = (int)(0.5+RndGamma());
    if ((j >= 0) && (j <= N)) 
      ++dist[j];
    }

  double dd = RndGamma();
  double Max = dd;
  double Min = dd;
  double Ttl = dd;
  for (j=0; j<NPts; j++)
    {
    dd = RndGamma();
    Ttl += dd;
    if (dd<Min)
      Min = dd;
    if (dd>Max)
      Max = dd;
    }
  dbgpln("=========== Gamma-Law Random generator test ===========");
  dbgpln("=== Ttl:%8.4f  Min:%8.4g  Max:%8.4g  Ave:%8.4f", Ttl, Min, Max, Ttl/(NPts+1));
  dbgpln("=== Gamma-Law distributed deviate of %d points over %d bins", NPts, N+1);
  dbgpln("=== Mean:%8.4f", dMean);
  dbgpln("===  x :      %8s %10s", "p(x)", "graph:");
  const int LLen = 50;
  int iscal = 200;
  int sum = 0;
  for (j=0; j<=N; j++)
    {
    sum += dist[j];
    dd = (double)dist[j]/NPts;
    klim = (int)(iscal*dd);
    if (klim > LLen)
      klim = LLen;
    dbgp("=== %2d : %4d %8.4f ", j, dist[j], dd);
    for (k=1; k<=klim; k++)
      dbgp("*");
    dbgpln("   (%d)", sum);
    }
  delete []dist;
  #endif
  }

//---------------------------------------------------------------------------

void CBaseNoise::TestWeibull(int N, int NPts)
  {
  #if DBG_RANDOM
  N = (int)(N/2)*2;
  //int N2 = N/2;
  int* dist = new int[N+1];
  int i,j,k,klim;

  for (j=0; j<=N; j++) 
    dist[j] = 0;
  ASSERT(dMean>1.0);
  //ASSERT(dMean<N);
  for (i=1; i<=NPts; i++) 
    {
    j = (int)(0.5+RndWeibull());
    if ((j >= 0) && (j <= N)) 
      ++dist[j];
    }

  double dd = RndWeibull();
  double Max = dd;
  double Min = dd;
  double Ttl = dd;
  for (j=0; j<NPts; j++)
    {
    dd = RndWeibull();
    Ttl += dd;
    if (dd<Min)
      Min = dd;
    if (dd>Max)
      Max = dd;
    }
  dbgpln("=========== Weibull Random generator test ===========");
  dbgpln("=== Ttl:%8.4f  Min:%8.4g  Max:%8.4g  Ave:%8.4f", Ttl, Min, Max, Ttl/(NPts+1));
  dbgpln("=== Weibull distributed deviate of %d points over %d bins", NPts, N+1);
  dbgpln("=== a:%8.4f  b:%8.4f", dMean, dG_StdDev);
  dbgpln("===  x :      %8s %10s", "p(x)", "graph:");
  const int LLen = 20;
  int iscal = 200;
  int sum = 0;
  for (j=0; j<=N; j++)
    {
    sum += dist[j];
    dd = (double)dist[j]/NPts;
    klim = (int)(iscal*dd);
    if (klim > LLen)
      klim = LLen;
    dbgp("=== %2d : %4d %8.4f ", j, dist[j], dd);
    for (k=1; k<=klim; k++)
      dbgp("*");
    dbgpln("   (%d)", sum);
    }
  delete []dist;
  #endif
  }

//---------------------------------------------------------------------------

void CBaseNoise::TestRnd(flag UseRnd1, int NPts, int seed)
  {
  #if DBG_RANDOM
  SetSeed(seed);
  dbgpln("\n=========== Random generator test =========== Rnd1:%d,  seed:%d", UseRnd1, seed);
  double Ttl = 0.0;
  for (int j=0; j<NPts; j++)
    {
    double d = (UseRnd1 ? pRnd->Rnd() : Rnd());
    Ttl += d;
    if (j<10)
      dbgpln("%2d : %8.4g", j, d);
    }
  dbgpln("=== Test over %d points.  Mean:%8.8f\n", NPts, Ttl/NPts);
  #endif
  }

//===========================================================================

CNoise::CNoise():
  CBaseNoise(this)
  {
  }

//===========================================================================

// Moved to PGM_N
/*
const short Idf_NoiseInit = 1;
const short Idf_NoiseSetType = 2;
const short Idf_NoiseSetGlobal = 3;
const short Idf_NoiseSetSeed = 4;
const short Idf_NoiseGetVal = 5;

//---------------------------------------------------------------------------

GCNoise::GCNoise(rGCInsMngr IB) : GCClassVar(IB.m_pVarList, "NOISE", VarClassDefn)
  {
  AddVar(IB, "Output",    &GCDouble, VarConst);
  AddVar(IB, "Mean",      &GCDouble);
  AddVar(IB, "StdDev",    &GCDouble);
  //AddVar(IB, "NPts",       &GCLong);
  AddFunct(IB, "Init", 5, False, Idf_NoiseInit); //Init(mean, stddev, typ, global, seed)
  AddFunct(IB, "SetType", 1, False, Idf_NoiseSetType); //SetType(typ)
  AddFunct(IB, "SetGlobal", 1, False, Idf_NoiseSetGlobal); //SetGlobal(bit global)
  AddFunct(IB, "SetSeed", 1, False, Idf_NoiseSetSeed); //SetSeed(int seed)
  AddFunct(IB, "GetVal", 0, False, Idf_NoiseGetVal); //GetVal()
  }

//---------------------------------------------------------------------------

void GCNoise::Init(pGCClassVar pClassVar)
  {
  //pClassVar->m_pSubClass = (pvoid)new CBaseNoise(pClassVar);
  pClassVar->m_pSubClass = (pvoid)new CBaseNoise(((pGCDoubleVar)pClassVar->GetVarByName("Mean"))->m_var, ((pGCDoubleVar)pClassVar->GetVarByName("StdDev"))->m_var, ((pGCDoubleVar)pClassVar->GetVarByName("Output"))->m_var);
  }
  
//---------------------------------------------------------------------------

void GCNoise::Done(pGCClassVar pClassVar)
  {
  delete (pCBaseNoise)(pClassVar->m_pSubClass);
  }

//---------------------------------------------------------------------------

double GCNoise::CallFunct(rGCInsMngr IB, pvoid pSubClass, short FunctId, pGCClassVar pClassVar)
  {
  pCBaseNoise pNoise = (pCBaseNoise)pSubClass;
  switch (FunctId)
    {
    case Idf_NoiseInit:
      {
      long Seed = IB.GetLParm();
      BOOL UseGlobal = (IB.GetCParm()!=0);
      byte NoiseTyp = Range(0, (int)IB.GetCParm(), (int)NS_Last-1);
      pNoise->SetType((NoiseTypes)NoiseTyp);
      pNoise->SetGlobal(UseGlobal);
      pNoise->SetSeed(Seed);
      double StdDev = IB.GetDParm();
      pNoise->Init(IB.GetDParm(), StdDev);
      break;
      }
    case Idf_NoiseSetType:
      {
      byte NoiseTyp = Range(0, (int)IB.GetCParm(), (int)NS_Last-1);
      pNoise->SetType((NoiseTypes)NoiseTyp);
      break;
      }
    case Idf_NoiseSetGlobal:
      pNoise->SetGlobal(IB.GetCParm()!=0);
      break;
    case Idf_NoiseSetSeed:
      pNoise->SetSeed(IB.GetLParm());
      break;
    case Idf_NoiseGetVal:
      return pNoise->GetVal();
      break;
    default:
      ASSERT(FALSE); //function not defined
    }
  return 0.0;
  }

//---------------------------------------------------------------------------

void GCNoise::OnSave(FilingControlBlock &FCB, pvoid pSubClass)
  {
  pCBaseNoise pNoise = (pCBaseNoise)pSubClass;
  GCFCBAppendRec(FCB, '{', "NOISE", NULL, 0); // Start Internal Vars
  SaveVal(FCB, "Type", (byte)pNoise->NoiseType());
  SaveVal(FCB, "UseGlobal", (byte)pNoise->IsGlobal());
  GCFCBAppendRec(FCB, '}', "NOISE", NULL, 0); // End Internal Vars
  }

//---------------------------------------------------------------------------
    
flag GCNoise::OnLoad(FilingControlBlock &FCB, pvoid pSubClass)
  {
  pCBaseNoise pNoise = (pCBaseNoise)pSubClass;
  for (;;)
    {
    GCFCBBuff Buff;
    GCFCBReadBuff(FCB, Buff);

    //dbgpln("..%5i: %c %3i %s",FCB.SetFilePointer(0, FILE_CURRENT), Buff.Hd.Id, Buff.Hd.nTotalLen, Buff.Name());

    switch (Buff.Hd.Id)
      {
      case '}':
        return True;
      case '{':
        break;
      default:
        {
        byte by;
        if (Buff.Try("Type", by)) 
          {
          pNoise->SetType((NoiseTypes)by);
          break;
          }
        if (Buff.Try("UseGlobal", by)) 
          {
          pNoise->SetGlobal(by!=0);
          break;
          }
        break;
        }
      }
    }
  return True;
  }
*/  
//===========================================================================

/*
Member Functions

Init (double Mean, double StdDev, byte NoiseType, bit Global, long Seed)

Parameters

Mean:Double field. Specifies the mean.
StdDev:Double field. Specifies the standard deviation for Gaussian distribution.
NoiseType:Byte field. Specifies the noise generator type required See SetType function for more details.
Global:Bit field. Must the random numbers be calculated from a global generator. See SetGlobal function for more details.
Seed:Long field. A seed number for the random generator.
 
Return Value :

Zero.

Description :

This initialises all the parameters for the random number generator.


SetGlobal (bit Global)

Parameters

Global:Bit field. Must the random numbers be calculated from a global generator.

Return Value :

Zero.

Description :

This must be called after the SetType or Init functions. It negates the affect of the SetSeed function, so it must be called before the SetSeed function. This is only meaningful for 

*/

