//===========================================================================
//=== SysCAD SMDK - Hyprod Precipitator Model 2003 : Alcan, Kenwalt       ===
//===========================================================================
#ifndef  __HYPRODCOMMON_H
#define  __HYPRODCOMMON_H
#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif
#ifdef __HYPRODCOMMON_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(AlcanU)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif

//===========================================================================
//========== Change these 'global' options as required ==========
#define ForceOptimizeOff 1
#define UseLicensing 0
#define VersionDescription "Version 1.0 Build 17 (1st Aug 2005)"
//===========================================================================

const double THADens=2420.0; //should get this from SysCAD specie database???

const DWORD MLicense_Alcan = 0x00040000;

const long MaxHPSizeClasses=100;

//---------------------------------------------------------------------------

const int nbAgloModel = 6;

enum eAgglomModel
  {
  eAgMdl_dlev,
  eAgMdl_LowAgit,
  eAgMdl_HighAgit,
  eAgMdl_SizeInd,
  eAgMdl_EquiSize,
  eAgMdl_Groneweg
  };

enum eHeatBalance
  {
  eHBal_Normal,
  eHBal_ImposedTemp,
  eHBal_OptimumTemp
  };

enum eNuclModel
  {
  eNucl_Hyprod,
  eNucl_NBrown,
  eNucl_LowShearRate,
  eNucl_HighShearRate,
  eNucl_LowSolidsConc,
  eNucl_Hybrid,
  };
//---------------------------------------------------------------------------

class HyprodPrecipHelper
  {
  public:
    HyprodPrecipHelper();
    bool InitSizeData(MVector & Vec);
    void InitAgloData(eAgglomModel eAgloModel, double AgloParam);
    void ApplyAgglom(double GRate, double dTime, double KvFac, double ProdNNtl);
    double get_PrecipRate(MVector & V, double Ssurf);
	double get_NucleationRate(eNuclModel eNucleation,MVector & V, double Ssurf, double ShRate);
    int MassBalance(MVector & Vec, double TRqd, double ARqd, double THARqd, double NoPerSec, double Na2OFac, double SSat, bool & ConvergeErr);
    int PrecipSS(MVector & Feed, MVector & Prod, MStream & QProdPrev);
    void HeatBalance(MVector & Feed, MVector & Prod, double T);
    void HeatBalance(MVector & Prod, double SolidsVariation, double enthIn, double dTime, double CurLevel);
    bool SetPSDfromPrev(MVector & V, MStream & QProdPrev);
    void MassFrac2HyprodDist(double SizeClass[],
                             double MassFrac[],
                             double PS[],
                             long NIntervals,
                             double &SSurf);
    void HyprodDist2MassFrac(double SizeClass[],
                             double PS[],
                             double MassFrac[],
                             long NIntervals);
    void CalcSSurgNNtl(double SizeClass[],
                       double HyPsd[],
                       long NIntervals,
                       double &SSurf,
                       double &NNtl);
    double get_Na2OFactor(MVector & V);
	double get_NuclFactor(MVector & V);

  protected:
    void DoHeatBalance(MVector & Prod, double enthIn, double T, double SolidsVariation, double dTime, double CurLevel);

  protected:
    //static double   m_dAmbientTemp;
    //static double   m_dWindSpeed;

    //global inputs:
    static bool     sm_bCompletePopulation;
    static bool     sm_bUsePrevPSD;
    //Inputs...
    double          m_dNumberInService;
    double          m_dVolume,
                    m_dSurface,
                    m_dKvFac,
                    m_dUCoef,
                    m_dLevel,
                    m_dTempImpos,
                    m_dInTankCooling,
					m_eShearRate;// !! used in AMIRA Nucleation MODELS
    eAgglomModel    m_eAgloModel;
    eHeatBalance    m_eHeatBalance;
	eNuclModel		m_eNuclModel;

    double          m_dAgloParam[nbAgloModel];
    //Outputs...
    double          m_dThermalLoss;
    double          m_dLoss2Ambient;
    double          m_dTempIn,
                    m_dTempOut,
                    m_dACIn,
                    m_dACOut,
                    m_dSolidConcIn,
                    m_dSolidConcOut,
                    m_dResTime,
                    m_dYield;
    bool            m_bPrevPSDUsed;

    //Size data...
    double  SizeClass[MaxHPSizeClasses];  //  MID-POINT Sizes in microns
    long    NIntervals;
    double  HPDSize;
    double  HPSMin;

    long    Q[MaxHPSizeClasses][MaxHPSizeClasses]; //agglom data
    double  Vli[MaxHPSizeClasses][MaxHPSizeClasses]; //agglom data
    double  Kva[MaxHPSizeClasses][MaxHPSizeClasses]; //agglom data
    long    mv2;

    double  dPSD[MaxHPSizeClasses];

    double  ProdHyPsd[MaxHPSizeClasses];
  };

//---------------------------------------------------------------------------


#undef DllImportExport

#endif