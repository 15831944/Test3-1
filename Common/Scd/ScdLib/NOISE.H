//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __NOISE_H
#define  __NOISE_H

//#ifndef __PGM_E_H
//#include "pgm_e.h"
//#endif  

#ifdef __NOISE_CPP
  #define DllImportExport DllExport
#elif !defined(SCDLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//===========================================================================

#define NTAB 32

class DllImportExport CRandom
  {
  public:
    CRandom();
    CRandom(long seed);
    void SetSeed(long seed) { idum = abs(seed) * -1; };
    double Rnd();

    long          iy;
    long          idum;         //seed
    long          iv[NTAB];
  };

#undef NTAB

//extern DllImportExport CRandom GlobalRnd;

//===========================================================================
_FWDDEF(CBaseNoise)
_FWDDEF(CNoise)
//_FWDDEF(GCNoise)

typedef enum NoiseTypes { NS_Gaus1, NS_Gaus, NS_Flat1, NS_Flat, NS_Pois1, NS_Pois, NS_Gamma1, NS_Gamma, NS_Weibull1, NS_Weibull, NS_Last };

class DllImportExport CBaseNoise
  {
  protected:
    NoiseTypes    eNoiseType;    //type of noise generator
    double&       dMean;         //value arround which noise is generated
    double&       dOutput;       //last value generated
    double&       dG_StdDev;     //Gaussian Standard Deviation; Flat Min/Max deviation
  private:
    flag          bG_iset;       //used by Gaus random generator
    double        dG_gset;       //used by Gaus random generator
    double        dP_sq;         //used by Poisson random generator
    double        dP_alxm;       //used by Poisson random generator
    double        dP_g;          //used by Poisson random generator
    double        dP_oldm;       //used by Poisson random generator
    CRandom*      pRnd;          //is this pointing to its own random integer sequence (if NULL use global)
  public:
    CBaseNoise(pCNoise pNoise);
//    CBaseNoise(pGCClassVar pClassVar);
    CBaseNoise(double& Mean, double& StdDev, double& Output);
    ~CBaseNoise();
  private:
    void CommonConstruct();
    double gammln(double xx);
  public:
    void SetType(NoiseTypes NoiseType);
    void SetGlobal(BOOL UseGlobal);
    void SetSeed(long seed);
    double GetVal();
    double RndGaus();
    double RndPoisson();
    double RndGamma();
    double RndWeibull();
    void TestGaus(int N = 20, int NPts = 2000);
    void TestPoisson(int N = 20, int NPts = 5000);
    void TestGamma(int N = 20, int NPts = 5000);
    void TestWeibull(int N = 20, int NPts = 5000);
    void TestRnd(flag UseRnd1 = False, int NPts = 100, int seed = -13);
    inline double SystemRnd() { return (double)rand()/(double)RAND_MAX; };
    inline void SetSystemSeed(long seed) { srand(abs(seed)); };
    inline NoiseTypes NoiseType() { return eNoiseType; };
    inline void Init(double Mean, double StdDev) { dMean = Mean; dG_StdDev = StdDev; }; 
    //inline flag IsGlobal() { return (pRnd==&GlobalRnd); };
    flag IsGlobal();

    static void SetGlobalNoiseSeed(long seed);
  };

//===========================================================================

class DllImportExport CNoise : public CBaseNoise
  {
  public:
    double m_dMean;
    double m_dStdDev;
    double m_dOutput;
    CNoise();
  };

//===========================================================================
// Moved to PGM_N
/*
class GCNoise : public GCClassVar
  {
  public:
    GCNoise(rGCInsMngr IB);
    virtual void Init(pGCClassVar pClassVar);
    virtual void Done(pGCClassVar pClassVar);
    virtual double CallFunct(rGCInsMngr IB, pvoid pSubClass, short FunctId, pGCClassVar pClassVar);
    virtual void OnSave(FilingControlBlock &FCB, pvoid pSubClass);
    virtual flag OnLoad(FilingControlBlock &FCB, pvoid pSubClass);
  };
*/
//===========================================================================

#undef DllImportExport

#endif
