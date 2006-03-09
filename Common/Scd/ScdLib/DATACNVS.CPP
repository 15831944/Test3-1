//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================


#include "stdafx.h"
#define  __DATACNVS_CPP
#include "sc_defs.h"
#include "datacnvs.h"
#include "dbhelper.h"
#include "errorlog.h"
#include "dbgmngr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define dbgCnvs     1

#if dbgCnvs
static CDbgMngr dbgCnvsList("General", "Cnvs");
#endif

//===========================================================================
// User Configurable Constants

double gs_StdPress      = 101.325; //changed from 101.31 May 2003 
double gs_StdTemp       = C2K(20.0); 
double gs_NormPress     = 101.325; //changed from 101.31 May 2003
double gs_NormTemp      = C2K(0.0); 

void   SetStdPress(double P)    { gs_StdPress=P; }
void   SetStdTemp(double T)     { gs_StdTemp=T; }
void   SetNormPress(double P)   { gs_NormPress=P; }
void   SetNormTemp(double T)    { gs_NormTemp=T; }

//===========================================================================
// Atmospheric Conditions

double gs_BaseElevation     = 0;

double gs_AtmP_C0       = 101.325; // CNM make this agree with MDB conversions 2870491;
double gs_AtmP_C1       = -11.83693496e-3;
double gs_AtmP_C2       = 0.479317541e-6;

double gs_AtmD_C0       = 1.224805469;
double gs_AtmD_C1       = -0.116626159e-3;
double gs_AtmD_C2       = 0.003798009e-6;

double gs_AmbientTemp   = C2K(25);
double gs_WindSpeed     = 0;
double gs_WindDirection = 0;
double gs_RelHumidity   = 0.2;

void SetBaseElevation(double E)
  { 
  gs_BaseElevation     = E; 
  };

void SetWindSpeed(double V)      { gs_WindSpeed     = V; };
void SetWindDirection(double V)  { gs_WindDirection = V; };    
void SetRelHumidity(double V)    { gs_RelHumidity   = V; };
void SetAmbientTemp(double V)    { gs_AmbientTemp   = V; };

double AtmosPress(double RelElevation)
  { 
  RelElevation+=gs_BaseElevation; 
  return gs_AtmP_C0 + RelElevation *( gs_AtmP_C1 + RelElevation *gs_AtmP_C2); 
  };
double AtmosDensity(double RelElevation)
  {
  RelElevation+=gs_BaseElevation; 
  return gs_AtmD_C0 + RelElevation *( gs_AtmD_C1 + RelElevation *gs_AtmD_C2);
  };

void InitialiseEnvironVars()
  {
  gs_StdPress      = 101.325;  
  gs_StdTemp       = (ZeroCinK+20.0); 
  gs_NormPress     = 101.325; 
  gs_NormTemp      = (ZeroCinK+0.0); 

  gs_BaseElevation     = 0;

  gs_AtmP_C0       = 101.325;
  gs_AtmP_C1       = -11.83693496e-3;
  gs_AtmP_C2       = 0.479317541e-6;

  gs_AtmD_C0       = 1.224805469;
  gs_AtmD_C1       = -0.116626159e-3;
  gs_AtmD_C2       = 0.003798009e-6;

  gs_AmbientTemp   = C2K(25);
  gs_WindSpeed     = 0;
  gs_WindDirection = 0;
  gs_RelHumidity   = 0.2;
  };

class CInitEnvVars
  {
  public:
    CInitEnvVars() { InitialiseEnvironVars(); };
  };

static CInitEnvVars gs_InitENvCars;

//---------------------------------------------------------------------------

//static double XAtmos(double Z, double &T, double &PP)
//  {
//  const double KK = 0;
//  const double K = 34.163195;
//  const double C1 = 0.001;
//
//  double H = C1 * Z / (1 + C1 * Z / 6356.766);
//
//  if (H > 11.0) goto L1290;
//  T = 288.15 - 6.5 * H;
//  PP = pow(288.15 / T, - K / 6.5);
//  goto L1420;
//
//L1290:
//  if (H > 20.0) goto L1310;
//  T = 216.65;
//  PP = .22336 * exp ( - K * (H - 11) / 216.65);
//  goto L1420;
//
//L1310:
//  if (H > 32.0) goto L1330;
//  T = 216.65 + (H - 20);
//  PP = .054032 * pow(216.65 / T, K);
//  goto L1420;
//
//L1330:
//  if (H > 47.0) goto L1350;
//  T = 228.65 + 2.8 * (H - 32);
//  PP = .0085666 * pow(228.65 / T,  K / 2.8);
//  goto L1420;
//
//L1350:
//  if( H > 51.0) goto L1370;
//  T = 270.65;
//  PP = .0010945 * exp ( - K * (H - 47) / 270.65);
//  goto L1420;
//
//L1370:
//  if (H > 71.) goto L1390;
//  T = 270.65 - 2.8 * (H - 51);
//  PP = 0.00066063 * pow(270.65 / T,  - K / 2.8);
//  goto L1420;
//L1390:
//  if (H>84.852)
//    return dNAN;
//  T = 214.65 - 2 * (H - 71);
//  PP = 3.9046E-05 * pow(214.65 / T, - K / 2);
//
//L1420:
//  return PP / (T / 288.15);
//  }
//
//
//static double XAtmosPress(double Z)
//  {
//  const double PL = 101.325;
//  double T, PP;
//  double RR=XAtmos(Z, T, PP);
//  return PL*PP;
//  }
//static double XAtmosTemp(double Z)
//  {
//  const double TL = 288.15;
//  double T, PP;
//  double RR=XAtmos(Z, T, PP);
//  double TS = T/288.15;
//  return TL*TS;
//  }
//static double XAtmosDens(double Z)
//  {
//  const double RL = 1.225;
//  double T, PP;
//  double RR=XAtmos(Z, T, PP);
//  return RL*RR;
//  }

//===========================================================================

CCnvIndex    DC_         = 0;
CCnvIndex    DC_Ang      = 0;
CCnvIndex    DC_DP       = 0;
CCnvIndex    DC_PpS      = 0;
CCnvIndex    DC_PpM      = 0;
CCnvIndex    DC_PpVol    = 0;
CCnvIndex    DC_E        = 0;
CCnvIndex    DC_Frac     = 0;
CCnvIndex    DC_FracRate = 0;
CCnvIndex    DC_HCap     = 0;
CCnvIndex    DC_HCapF    = 0;
CCnvIndex    DC_L        = 0;
CCnvIndex    DC_Ldt      = 0;
CCnvIndex    DC_M        = 0;
CCnvIndex    DC_Qm       = 0;
CCnvIndex    DC_QmSlew   = 0;
CCnvIndex    DC_Qv       = 0;
CCnvIndex    DC_QvSlew   = 0;
CCnvIndex    DC_Qe       = 0;
CCnvIndex    DC_NQv      = 0;
CCnvIndex    DC_NVol     = 0;
CCnvIndex    DC_P        = 0;
CCnvIndex    DC_Pwr      = 0;
CCnvIndex    DC_pS       = 0;
CCnvIndex    DC_Rho      = 0;
CCnvIndex    DC_T        = 0;
CCnvIndex    DC_Time     = 0;
CCnvIndex    DC_Vol      = 0;
CCnvIndex    DC_WI       = 0;
CCnvIndex    DC_HMs      = 0;
CCnvIndex    DC_HMl      = 0;
CCnvIndex    DC_HV       = 0;
CCnvIndex    DC_HNV      = 0;
CCnvIndex    DC_SMs      = 0;
CCnvIndex    DC_SMl      = 0;
CCnvIndex    DC_Ent      = 0;
CCnvIndex    DC_EntMl    = 0;
CCnvIndex    DC_CpMs     = 0;
CCnvIndex    DC_CpMl     = 0;
CCnvIndex    DC_Conc     = 0;
CCnvIndex    DC_Area     = 0;
CCnvIndex    DC_HTC      = 0;
CCnvIndex    DC_HCond    = 0;
CCnvIndex    DC_UA       = 0;
CCnvIndex    DC_dT       = 0;
CCnvIndex    DC_dTdt     = 0;
CCnvIndex    DC_I        = 0;
CCnvIndex    DC_V        = 0;
CCnvIndex    DC_Epm      = 0;
CCnvIndex    DC_Visc     = 0;
CCnvIndex    DC_MpL      = 0;
CCnvIndex    DC_KgMl     = 0;
CCnvIndex    DC_QKgMl    = 0;
CCnvIndex    DC_Cond     = 0;
CCnvIndex    DC_SpCnt    = 0;
CCnvIndex    DC_Cnt      = 0;
CCnvIndex    DC_CntRate  = 0;
CCnvIndex    DC_SurfTens = 0;
CCnvIndex    DC_EntRate  = 0;
CCnvIndex    DC_DPperVol = 0;
CCnvIndex    DC_SurfAreaM= 0;
CCnvIndex    DC_SurfAreaL= 0;
CCnvIndex    DC_FracSol  = 0;
CCnvIndex    DC_FracLiq  = 0;
CCnvIndex    DC_FracVap  = 0;
CCnvIndex    DC_FracPh00 = 0;
CCnvIndex    DC_FracPh01 = 0;
CCnvIndex    DC_FracPh02 = 0;
CCnvIndex    DC_FracPh03 = 0;
CCnvIndex    DC_FracPh04 = 0;
CCnvIndex    DC_FracPh05 = 0;
CCnvIndex    DC_FracPh06 = 0;
CCnvIndex    DC_FracPh07 = 0;
CCnvIndex    DC_FracPh08 = 0;
CCnvIndex    DC_FracPh09 = 0;
CCnvIndex    DC_FracPh10 = 0;
CCnvIndex    DC_FracPh11 = 0;
CCnvIndex    DC_FracPh12 = 0;
CCnvIndex    DC_FracPh13 = 0;
CCnvIndex    DC_FracPh14 = 0;
CCnvIndex    DC_FracPh15 = 0;
CCnvIndex    DC_FracPh16 = 0;
CCnvIndex    DC_FracPh17 = 0;
CCnvIndex    DC_FracPh18 = 0;
CCnvIndex    DC_FracPh19 = 0;
CCnvIndex    DC_FracPh20 = 0;
CCnvIndex    DC_FracPh21 = 0;
CCnvIndex    DC_FracPh22 = 0;
CCnvIndex    DC_FracPh23 = 0;
CCnvIndex    DC_FracPh24 = 0;
CCnvIndex    DC_FracPh25 = 0;
CCnvIndex    DC_FracPh26 = 0;
CCnvIndex    DC_FracPh27 = 0;
CCnvIndex    DC_FracPh28 = 0;
CCnvIndex    DC_FracPh29 = 0;
CCnvIndex    DC_FracPh30 = 0;
CCnvIndex    DC_FracPh31 = 0;
CCnvIndex    DC_CosPhi   = 0;
CCnvIndex    DC_ThermTau = 0;
CCnvIndex    DC_Ohms     = 0;
CCnvIndex    DC_Bytes    = 0;
CCnvIndex    DC_ConcRate = 0;
CCnvIndex    DC_KinVisc  = 0;
CCnvIndex    DC_ElectVA  = 0;
CCnvIndex    DC_Humidity = 0;

//===========================================================================

CDataCnvFam::CDataCnvFam(char* Fam, char* Desc, double Min, double Max_)
  {
  if (Desc==NULL || strlen(Desc)==0)
    Desc = ""; //"-";
  pFam = new char[strlen(Fam)+1];
  strcpy(pFam, Fam);
  pDesc = new char[strlen(Desc)+1];
  strcpy(pDesc, Desc);
  pSICnv = NULL;
  dMin = Min;
  dMax = Max(Max_, Min);
  }

//---------------------------------------------------------------------------

CDataCnvFam::~CDataCnvFam()
  {
  delete []pFam;
  delete []pDesc;
  }

//===========================================================================

CDataCnv::CDataCnv(CDataCnvFam* SI, char* Txt, double Scl, double Off, flag PrjCnv, long iXform)
  {
  pSI = SI;
  if (strlen(Txt)==0)
    Txt = ""; //"-";
  pTxt = new char[strlen(Txt)+1];
  strcpy(pTxt, Txt);
  dScl = Scl;
  dOff = Off;
  dOffMult = 1;
  bPrjCnv = PrjCnv;
  m_pNxt = NULL;
	m_iInx = 0;
  m_iXform=iXform;
  }

//---------------------------------------------------------------------------

CDataCnv::~CDataCnv() 
  {
  delete []pTxt;
  if (m_pNxt)
    delete m_pNxt;
  else
    delete pSI;
  }

//---------------------------------------------------------------------------

CDataCnv* CDataCnv::Find(char* SecTxt)
  {
  if (SecTxt==NULL)
    return NULL;
  CDataCnv* p = this;
  while (p && (strcmp(SecTxt, p->pTxt) != 0))
    p = p->m_pNxt;
  return p;
  }

//---------------------------------------------------------------------------

CDataCnv* CDataCnv::At(char* SecTxt)
  { 
  CDataCnv* p = this;
  while (p && (strcmp(SecTxt, p->pTxt)!=0))
    p = p->m_pNxt;
  if (p==NULL)
    return this;
  return p;
  }

//---------------------------------------------------------------------------

CDataCnv* CDataCnv::AddSecondaryCnv(char* Txt, double Scl, double Off, flag PrjCnv)//, flag OffsetIsAtmos)
  {
  CDataCnv* p = new CDataCnv(pSI, Txt, Scl, Off, PrjCnv, m_iXform);//, OffsetIsAtmos);
  CDataCnv* pe = this;
  while (pe->m_pNxt)
    pe = pe->m_pNxt;
  pe->m_pNxt = p;
	p->m_iInx=pe->m_iInx+1;
  return p;
  }

long CDataCnv::Count()
	{
  CDataCnv* pe = this;
	for (long i=1; pe->m_pNxt; i++)
    pe = pe->m_pNxt;
	return i;
	};
CDataCnv* CDataCnv::operator[](long Index)
	{
	if (Index<0)
		return NULL;
  CDataCnv* pe = this;
	while (pe && pe->m_iInx!=Index)
    pe = pe->m_pNxt;
	return pe;
	};

//---------------------------------------------------------------------------

void CDataCnv::Clear()
  {
  if (m_pNxt)
    {
    if (m_pNxt->PrjCnv())
      {//remove completely from linked list and then delete
      CDataCnv* pToDel = m_pNxt;
      m_pNxt = pToDel->m_pNxt;
      pToDel->m_pNxt = NULL;
      pToDel->pSI = NULL;
      delete pToDel;
      }
    if (m_pNxt)
      m_pNxt->Clear();
    }
  }

//---------------------------------------------------------------------------

LPCTSTR CDataCnv::XformName()
  {
  switch (m_iXform)
    {
    case 0: return "";
    case 1: return "COS";
    case 2: return "GaugeP";
    }
  return "?";
  };

//---------------------------------------------------------------------------

double CDataCnv::XformNormal(double x)
  {
  switch (m_iXform)
    {
    case 1: return (acos(x)-dOff*dOffMult)/dScl;
    case 2: return x/dScl+(dOff!=0.0 ? AtmosPress(0) : 0.0);
    }
  return x;
  };

//---------------------------------------------------------------------------

double CDataCnv::XformHuman(double x)
  {
  switch (m_iXform)
    {
    case 1: return (cos(x)*dScl)+dOff*dOffMult; 
    case 2: return (x-(dOff!=0.0 ? AtmosPress(0):0.0))*dScl;
    }
  return x;
  };

//===========================================================================

CCnvsMngr::CCnvsMngr()
  {
  Cnvs.SetSize(0,1024);
  }

//---------------------------------------------------------------------------

CCnvsMngr::~CCnvsMngr()
  {
  for (int i=0; i< Cnvs.GetSize(); i++)
    delete Cnvs[i];
  Cnvs.SetSize(0,1024);
  }

//---------------------------------------------------------------------------

void CCnvsMngr::Init()
  {
  if (Cnvs.GetSize()==0)
    {
    //            AddSI(Family     , SI units     , description                     ,    min ,            max, PrjCnv)
    DC_         = AddSI(""         , ""           , "Constants"                     ,    0.0 ,          100.0, false);
    DC_Frac     = AddSI("Frac"     , ""           , "Fraction"                      ,    0.0 ,            1.0, false);
    DC_FracRate = AddSI("FracRate" , "/s"         , "Fractional Rate"               ,    0.0 ,            1.0, false);
    DC_L        = AddSI("L"        , "m"          , "Length"                        ,    0.0 ,         1000.0, false);
    DC_M        = AddSI("M"        , "kg"         , "Mass"                          ,    0.0 ,         1000.0, false);
    DC_P        = AddSI("P"        , "kPa"        , "Pressure"                      ,    0.0 ,         1000.0, false, 2);
    DC_T        = AddSI("T"        , "K"          , "Temperature"                   ,  Std_T ,    Std_T+100.0, false);
    DC_Qm       = AddSI("Qm"       , "kg/s"       , "Mass flow"                     ,    0.0 ,          100.0, false);
    DC_QmSlew   = AddSI("QmSlew"   , "kg/s/s"     , "Mass flow Slewrate"            ,    0.0 ,          100.0, false);
    DC_Qv       = AddSI("Qv"       , "m^3/s"      , "Volumetric flow"               ,    0.0 ,          100.0, false);
    DC_QvSlew   = AddSI("QvSlew"   , "m^3/s/s"    , "Volumetric flow SlewRate"      ,    0.0 ,          100.0, false);
    DC_Area     = AddSI("Area"     , "m^2"        , "Area"                          ,    0.0 ,            1.0, false);
    DC_Vol      = AddSI("Vol"      , "m^3"        , "Volume"                        ,    0.0 ,            1.0, false);
    DC_Rho      = AddSI("Rho"      , "kg/m^3"     , "Density"                       ,    0.0 ,         3000.0, false);
    DC_Time     = AddSI("Time"     , "s"          , "Time"                          ,    0.0 ,         3600.0, false);
    DC_E        = AddSI("E"        , "kJ"         , "Energy"                        ,    0.0 ,         1000.0, false);
    DC_Pwr      = AddSI("Pwr"      , "kJ/s"       , "Power"                         ,    0.0 ,          100.0, false);
    DC_HMs      = AddSI("H(Ms)"    , "kJ/kg"      , "Enthalpy"                      ,    0.0 ,            1.0, false);
    DC_HMl      = AddSI("H(Ml)"    , "kJ/kmol"    , "Molar Enthalpy"                ,    0.0 ,            1.0, false);
    DC_HV       = AddSI("H(V)"     , "kJ/m^3"     , "Volume Enthalpy"               ,    0.0 ,            1.0, false);
    DC_HNV      = AddSI("H(NV)"    , "kJ/Nm^3"    , "Norm Volume Enthalpy"          ,    0.0 ,            1.0, false);
    DC_SMs      = AddSI("S(Ms)"    , "kJ/kg.K"    , "Entropy"                       ,    0.0 ,            1.0, false);
    DC_SMl      = AddSI("S(Ml)"    , "kJ/kmol.K"  , "Molar Entropy"                 ,    0.0 ,            1.0, false);
    DC_CpMs     = AddSI("Cp(Ms)"   , "kJ/kg.C"    , "Cp"                            ,    0.0 ,            1.0, false);
    DC_CpMl     = AddSI("Cp(Ml)"   , "kJ/kmol.C"  , "Molar Cp"                      ,    0.0 ,            1.0, false);
    DC_HCap     = AddSI("HCap"     , "kJ/dC"      , "Heat Capacity"                 ,    0.0 ,          100.0, false);
    DC_HCapF    = AddSI("HCapF"    , "kJ/dC/s"    , ""                              ,    0.0 ,          100.0, false);
    DC_HTC      = AddSI("HTC"      , "kW/m^2.K"   , "Heat Transfer Coeff"           ,    0.0 ,            1.0, false);
    DC_HCond    = AddSI("HCond"    , "kW/m.K"     , "Heat Conductivity"             ,    0.0 ,            1.0, false);
    DC_DP       = AddSI("DP"       , "kPa"        , "Pressure difference"           ,    0.0 ,         1000.0, false);
    DC_PpS      = AddSI("PpS"      , "kPa/s"      , "Pressure Slew Rate"            ,    0.0 ,         1000.0, false);
    DC_PpM      = AddSI("PpM"      , "kPa/kg"     , "Pressure Mass Slew Rate"       ,    0.0 ,         1000.0, false);
    DC_PpVol    = AddSI("PpVol"    , "kPa/m^3"    , "Pressure Volume Slew Rate"     ,    0.0 ,         1000.0, false);
    DC_dT       = AddSI("dT"       , "K"          , "Change in Temperature"         , -100.0 ,          100.0, false);
    DC_dTdt     = AddSI("dTdt"     , "C/s"        , "Rate of Change in Temperature" , -100.0 ,          100.0, false);
    DC_Ldt      = AddSI("Ldt"      , "m/s"        , "Velocity"                      ,    0.0 ,         1000.0, false);
    DC_Qe       = AddSI("Qe"       , "kJ/s"       , "EnergyFlow"                    ,    0.0 ,          100.0, false);
    DC_NQv      = AddSI("NQv"      , "Nm^3/s"     , "Normal Volumetric flow"        ,    0.0 ,          100.0, false);
    DC_NVol     = AddSI("NVol"     , "Nm^3"       , "Normal Volume"                 ,    0.0 ,          100.0, false);
    DC_pS       = AddSI("pS"       , "/s"         , "Per unit time"                 ,    0.0 ,         3600.0, false);
    DC_Conc     = AddSI("Conc"     , "kg/m^3"     , "Concentration"                 ,    0.0 ,            1.0, false);
    DC_UA       = AddSI("UA"       , "kW/K"       , "Heat Transfer"                 ,    0.0 ,            1.0, false);
    DC_Epm      = AddSI("Epm"      , "kW/t"       , "Energy per mass"               ,    0.0 ,         1000.0, false);
    DC_Visc     = AddSI("Visc"     , "Ns/m^2"     , "Viscosity"                     ,    0.0 ,            1.0, false);//(10 * Poise)
    DC_MpL      = AddSI("MpL"      , "kg/m"       , "Mass Loading"                  ,    0.0 ,         1000.0, false);
    DC_KgMl     = AddSI("KgMl"     , "kmol"       , "Moles"                         ,    0.0 ,         1000.0, false);
    DC_QKgMl    = AddSI("QKgMl"    , "kmol/s"     , "Molar flow"                    ,    0.0 ,         1000.0, false);
    DC_I        = AddSI("I"        , "A"          , "Current"                       ,    0.0 ,         1000.0, false);
    DC_V        = AddSI("V"        , "V"          , "Potential Diff"                ,    0.0 ,         1000.0, false);
    DC_Cond     = AddSI("Cond"     , "S/m"        , "Conductivity"                  ,    0.0 ,            1.0, false);
    DC_WI       = AddSI("WI"       , "kWs/kg"     , "Work Index"                    ,    0.0 ,            1.0, false);
    DC_Ang      = AddSI("Ang"      , "rad"        , "Angle"                         ,    0.0 ,  6.28318530718, false);
    DC_SpCnt    = AddSI("SpCnt"    , "#/kg"       , "SpecificCount"                 ,    0.0 ,            1.0, false);
    DC_Cnt      = AddSI("Cnt"      , "#"          , "Count"                         ,    0.0 ,         1000.0, false);
    DC_CntRate  = AddSI("CntRate"  , "#/s"        , "CountRate"                     ,    0.0 ,         1000.0, false);
    DC_SurfTens = AddSI("SurfTens" , "N/m"        , "Surface Tension"               ,    0.0 ,         1000.0, false);
    DC_EntRate  = AddSI("EntRate"  , "Frac.s/kg"  , "EntrainRate"                   ,    0.0 ,            1.0, false);
    DC_DPperVol = AddSI("DPperVol" , "kPa/m^3/s"  , "PressDropPerVolFlow"           ,    0.0 ,            1.0, false);
    DC_SurfAreaM= AddSI("SurfAreaM", "m^2/g"      , "Surface Area (Mass)"           ,    0.0 ,            1.0, false); //"Specific Surface Area"
    DC_SurfAreaL= AddSI("SurfAreaL", "m^2/L"      , "Surface Area (Volume)"         ,    0.0 ,            1.0, false); //"Total Surface Area"
    DC_FracSol  = AddSI("Frac(Sol)", ""           , "Fraction (Solids)"             ,    0.0 ,            1.0, false);
    DC_FracLiq  = AddSI("Frac(Liq)", ""           , "Fraction (Liquids)"            ,    0.0 ,            1.0, false);
    DC_FracVap  = AddSI("Frac(Vap)", ""           , "Fraction (Vapour)"             ,    0.0 ,            1.0, false);
    DC_FracPh00 = AddSI("Frac(Ph00)", ""          , "Fraction (Ph00)"               ,    0.0 ,            1.0, false);
    DC_FracPh01 = AddSI("Frac(Ph01)", ""          , "Fraction (Ph01)"               ,    0.0 ,            1.0, false);
    DC_FracPh02 = AddSI("Frac(Ph02)", ""          , "Fraction (Ph02)"               ,    0.0 ,            1.0, false);
    DC_FracPh03 = AddSI("Frac(Ph03)", ""          , "Fraction (Ph03)"               ,    0.0 ,            1.0, false);
    DC_FracPh04 = AddSI("Frac(Ph04)", ""          , "Fraction (Ph04)"               ,    0.0 ,            1.0, false);
    DC_FracPh05 = AddSI("Frac(Ph05)", ""          , "Fraction (Ph05)"               ,    0.0 ,            1.0, false);
    DC_FracPh06 = AddSI("Frac(Ph06)", ""          , "Fraction (Ph06)"               ,    0.0 ,            1.0, false);
    DC_FracPh07 = AddSI("Frac(Ph07)", ""          , "Fraction (Ph07)"               ,    0.0 ,            1.0, false);
    DC_FracPh08 = AddSI("Frac(Ph08)", ""          , "Fraction (Ph08)"               ,    0.0 ,            1.0, false);
    DC_FracPh09 = AddSI("Frac(Ph09)", ""          , "Fraction (Ph09)"               ,    0.0 ,            1.0, false);
    DC_FracPh10 = AddSI("Frac(Ph10)", ""          , "Fraction (Ph10)"               ,    0.0 ,            1.0, false);
    DC_FracPh11 = AddSI("Frac(Ph11)", ""          , "Fraction (Ph11)"               ,    0.0 ,            1.0, false);
    DC_FracPh12 = AddSI("Frac(Ph12)", ""          , "Fraction (Ph12)"               ,    0.0 ,            1.0, false);
    DC_FracPh13 = AddSI("Frac(Ph13)", ""          , "Fraction (Ph13)"               ,    0.0 ,            1.0, false);
    DC_FracPh14 = AddSI("Frac(Ph14)", ""          , "Fraction (Ph14)"               ,    0.0 ,            1.0, false);
    DC_FracPh15 = AddSI("Frac(Ph15)", ""          , "Fraction (Ph15)"               ,    0.0 ,            1.0, false);
    DC_FracPh16 = AddSI("Frac(Ph16)", ""          , "Fraction (Ph16)"               ,    0.0 ,            1.0, false);
    DC_FracPh17 = AddSI("Frac(Ph17)", ""          , "Fraction (Ph17)"               ,    0.0 ,            1.0, false);
    DC_FracPh18 = AddSI("Frac(Ph18)", ""          , "Fraction (Ph18)"               ,    0.0 ,            1.0, false);
    DC_FracPh19 = AddSI("Frac(Ph19)", ""          , "Fraction (Ph19)"               ,    0.0 ,            1.0, false);
    DC_FracPh20 = AddSI("Frac(Ph20)", ""          , "Fraction (Ph20)"               ,    0.0 ,            1.0, false);
    DC_FracPh21 = AddSI("Frac(Ph21)", ""          , "Fraction (Ph21)"               ,    0.0 ,            1.0, false);
    DC_FracPh22 = AddSI("Frac(Ph22)", ""          , "Fraction (Ph22)"               ,    0.0 ,            1.0, false);
    DC_FracPh23 = AddSI("Frac(Ph23)", ""          , "Fraction (Ph23)"               ,    0.0 ,            1.0, false);
    DC_FracPh24 = AddSI("Frac(Ph24)", ""          , "Fraction (Ph24)"               ,    0.0 ,            1.0, false);
    DC_FracPh25 = AddSI("Frac(Ph25)", ""          , "Fraction (Ph25)"               ,    0.0 ,            1.0, false);
    DC_FracPh26 = AddSI("Frac(Ph26)", ""          , "Fraction (Ph26)"               ,    0.0 ,            1.0, false);
    DC_FracPh27 = AddSI("Frac(Ph27)", ""          , "Fraction (Ph27)"               ,    0.0 ,            1.0, false);
    DC_FracPh28 = AddSI("Frac(Ph28)", ""          , "Fraction (Ph28)"               ,    0.0 ,            1.0, false);
    DC_FracPh29 = AddSI("Frac(Ph29)", ""          , "Fraction (Ph29)"               ,    0.0 ,            1.0, false);
    DC_FracPh30 = AddSI("Frac(Ph30)", ""          , "Fraction (Ph30)"               ,    0.0 ,            1.0, false);
    DC_FracPh31 = AddSI("Frac(Ph31)", ""          , "Fraction (Ph31)"               ,    0.0 ,            1.0, false);
    DC_ThermTau = AddSI("ThermTau" , "dC/s.C"     , "Thermal TimeConst"             ,    0.0 ,            1.0, false);
    DC_CosPhi   = AddSI("Cos(<)"   , "Cos(<)"     , "PowerFactor"                   ,    0.0 ,            1.0, false, 1);
    DC_Ohms     = AddSI("Ohms"     , "Ohms"       , "Electrical Resistance"         ,    0.0 ,            1.0, false);
    DC_Bytes    = AddSI("Bytes"    , "bytes"      , "Memory Size"                   ,    0.0 ,            1.0, false);
    DC_ConcRate = AddSI("ConcRate" , "kg/m^3/s"   , "Concentration Change Rate"     ,    0.0 ,            1.0, false);
    DC_KinVisc  = AddSI("KinVisc"  , "m^2/s"      , "Kinematic Viscosity (Stoke)"   ,    0.0 ,            1.0, false);
    DC_ElectVA  = AddSI("ElectVA"  , "VA"         , "VoltAmps (V*A)"                ,    0.0 ,            1.0, false);
    DC_Humidity = AddSI("Humidity" , "kg(l)/kg(v)"  , "Specific Humidity"           ,    0.0 ,            0.01, false);
    //NB: ensure new cnvs are added to SMDK (md_share2.h and md_defn.cpp)!

    //add some required secondary cnvs...
    Cnvs[DC_]->AddSecondaryCnv("CONSTANT", 1.0, 0.0, false);
    Cnvs[DC_FracSol]->AddSecondaryCnv("%(Sol)", 100.0, 0.0, false);
    Cnvs[DC_FracLiq]->AddSecondaryCnv("%(Liq)", 100.0, 0.0, false);
    Cnvs[DC_FracVap]->AddSecondaryCnv("%(Vap)", 100.0, 0.0, false);
    Cnvs[DC_FracVap]->AddSecondaryCnv("%(Gas)", 100.0, 0.0, false);
    Cnvs[DC_FracPh00]->AddSecondaryCnv("%(Ph00)", 100.0, 0.0, false);
    Cnvs[DC_FracPh01]->AddSecondaryCnv("%(Ph01)", 100.0, 0.0, false);
    Cnvs[DC_FracPh02]->AddSecondaryCnv("%(Ph02)", 100.0, 0.0, false);
    Cnvs[DC_FracPh03]->AddSecondaryCnv("%(Ph03)", 100.0, 0.0, false);
    Cnvs[DC_FracPh04]->AddSecondaryCnv("%(Ph04)", 100.0, 0.0, false);
    Cnvs[DC_FracPh05]->AddSecondaryCnv("%(Ph05)", 100.0, 0.0, false);
    Cnvs[DC_FracPh06]->AddSecondaryCnv("%(Ph06)", 100.0, 0.0, false);
    Cnvs[DC_FracPh07]->AddSecondaryCnv("%(Ph07)", 100.0, 0.0, false);
    Cnvs[DC_FracPh08]->AddSecondaryCnv("%(Ph08)", 100.0, 0.0, false);
    Cnvs[DC_FracPh09]->AddSecondaryCnv("%(Ph09)", 100.0, 0.0, false);
    Cnvs[DC_FracPh10]->AddSecondaryCnv("%(Ph10)", 100.0, 0.0, false);
    Cnvs[DC_FracPh11]->AddSecondaryCnv("%(Ph11)", 100.0, 0.0, false);
    Cnvs[DC_FracPh12]->AddSecondaryCnv("%(Ph12)", 100.0, 0.0, false);
    Cnvs[DC_FracPh13]->AddSecondaryCnv("%(Ph13)", 100.0, 0.0, false);
    Cnvs[DC_FracPh14]->AddSecondaryCnv("%(Ph14)", 100.0, 0.0, false);
    Cnvs[DC_FracPh15]->AddSecondaryCnv("%(Ph15)", 100.0, 0.0, false);
    Cnvs[DC_FracPh16]->AddSecondaryCnv("%(Ph16)", 100.0, 0.0, false);
    Cnvs[DC_FracPh17]->AddSecondaryCnv("%(Ph17)", 100.0, 0.0, false);
    Cnvs[DC_FracPh18]->AddSecondaryCnv("%(Ph18)", 100.0, 0.0, false);
    Cnvs[DC_FracPh19]->AddSecondaryCnv("%(Ph19)", 100.0, 0.0, false);
    Cnvs[DC_FracPh20]->AddSecondaryCnv("%(Ph20)", 100.0, 0.0, false);
    Cnvs[DC_FracPh21]->AddSecondaryCnv("%(Ph21)", 100.0, 0.0, false);
    Cnvs[DC_FracPh22]->AddSecondaryCnv("%(Ph22)", 100.0, 0.0, false);
    Cnvs[DC_FracPh23]->AddSecondaryCnv("%(Ph23)", 100.0, 0.0, false);
    Cnvs[DC_FracPh24]->AddSecondaryCnv("%(Ph24)", 100.0, 0.0, false);
    Cnvs[DC_FracPh25]->AddSecondaryCnv("%(Ph25)", 100.0, 0.0, false);
    Cnvs[DC_FracPh26]->AddSecondaryCnv("%(Ph26)", 100.0, 0.0, false);
    Cnvs[DC_FracPh27]->AddSecondaryCnv("%(Ph27)", 100.0, 0.0, false);
    Cnvs[DC_FracPh28]->AddSecondaryCnv("%(Ph28)", 100.0, 0.0, false);
    Cnvs[DC_FracPh29]->AddSecondaryCnv("%(Ph29)", 100.0, 0.0, false);
    Cnvs[DC_FracPh30]->AddSecondaryCnv("%(Ph30)", 100.0, 0.0, false);
    Cnvs[DC_FracPh31]->AddSecondaryCnv("%(Ph31)", 100.0, 0.0, false);

    }
  }

//---------------------------------------------------------------------------

void CCnvsMngr::Reset()
  {
  for (int i=0; i< Cnvs.GetSize(); i++)
    delete Cnvs[i];
  Cnvs.SetSize(0,1024);
  }

//---------------------------------------------------------------------------

CCnvIndex CCnvsMngr::AddSI(char* Fam, char* Txt, char* Desc, double Min, double Max_, flag PrjCnv, long iXform)
  {
  ASSERT(FindCnv(Fam)==-1); //should NOT already exist!
  CDataCnvFam* pSI = new CDataCnvFam(Fam, Desc, Min, Max_);
  Cnvs.Add(new CDataCnv(pSI, Txt, 1.0, 0.0, PrjCnv, iXform));
  pSI->pSICnv = Cnvs[Cnvs.GetSize()-1];
  return (Cnvs.GetSize()-1);
  }

//---------------------------------------------------------------------------

enum CNVColsADO
  {
  denCnv,
  denUnit,
  denSclOff
  };

BOOL CCnvsMngr::ParseCnvLineADO(int LineNo, CStringArray& Values, BOOL LogErrs, flag PrjCnv)
  {
  //get cnv...
  if (Values[denCnv].GetLength()==0)
    {
    if (LogErrs)
      LogError("Cnvs", 0, "Conversion not specified. Line %u:(%s  %s  %s)", LineNo, (const char*)Values[denCnv], (const char*)Values[denUnit], (const char*)Values[denSclOff]);
    return False;
    }

  //get scale, offset...
  flag Primary = True;
  double Scl = 1.0;
  double Ofs = 0.0;
  if (Values[denSclOff].GetLength()>0)
    {
    CSVColArray f;
    int Quote;
    int nParms = ParseCSVFunction((char*)(const char*)Values[denSclOff], f, Quote) - 1;
    if (Quote)
      {
      if (LogErrs)
        LogError("Cnvs", 0, "Valid ScaleOffset expected. Line %u:(%s  %s  %s)", LineNo, (const char*)Values[denCnv], (const char*)Values[denUnit], (const char*)Values[denSclOff]);
      return False;
      }
    if (stricmp(f[0], "SIUnit")==0)
      Primary = True;
    else if (stricmp(f[0], "scloff")==0)
      {
      if (nParms<2 || nParms>3)
        {
        if (LogErrs)
          LogError("Cnvs", 0, "Valid ScaleOffset expected 'SclOff(Scale,Offset)'. Line %u:(%s  %s  %s)", LineNo, (const char*)Values[denCnv], (const char*)Values[denUnit], (const char*)Values[denSclOff]);
        return False;
        }
      Scl = SafeAtoF(f[1]);
      Ofs = SafeAtoF(f[2]);
      Primary = False;
      }
    else
      {
      if (LogErrs)
        LogError("Cnvs", 0, "Valid ScaleOffset expected. Line %u:(%s  %s  %s)", LineNo, (const char*)Values[denCnv], (const char*)Values[denUnit], (const char*)Values[denSclOff]);
      return False;
      }
    }
  
  //add cnv...
  CCnvIndex index = FindCnv((char*)(const char*)Values[denCnv]);
  if ((index)<0 && !Primary)
    {//Add new conversion family!!!

    // This Data should be updated by later records
    index=AddSI((char*)(const char*)Values[denCnv], "???", "", 0.0, 1.0, PrjCnv);
    }

  if ((index)<0 && Primary)
    {//Add new conversion family!!!
    index = AddSI((char*)(const char*)Values[denCnv], (char*)(const char*)Values[denUnit], "", 0.0, 1.0, PrjCnv);
    }
  else if (index>=0 && !Primary)
    {
    if (Cnvs[index]->Find((char*)(const char*)Values[denUnit]))
      {
      //if (LogErrs)
      //  LogError("Cnvs", 0, "Line %u: Unit '%s' for '%s' allready defined", LineNo, (const char*)Values[denUnit], (const char*)Values[denCnv]);
      return False;
      }
    Cnvs[index]->AddSecondaryCnv((char*)(const char*)Values[denUnit], Scl, Ofs, PrjCnv);
    }
  else if (index>=0 && Primary)
    {
    if (Cnvs[index]->Find((char*)(const char*)Values[denUnit]))
      {
      //if (LogErrs)
      //  LogError("Cnvs", 0, "Line %u: Unit '%s' for '%s' allready defined", LineNo, (const char*)Values[denUnit], (const char*)Values[denCnv]);
      return False;
      }
    Cnvs[index]->AddSecondaryCnv((char*)(const char*)Values[denUnit], 1.0, 0.0, PrjCnv);

    //char *&pTxt=Cnvs[index]->SICnv()->pTxt;
    //delete pTxt;
    //pTxt=new char[strlen(Values[denUnit])+1];    //conversion text
    //strcpy(pTxt, Values[denUnit]);


    ////if (LogErrs)
    ////  LogError("Cnvs", 0, "Line %u: SIUnit for '%s' allready defined", LineNo, (const char*)Values[denCnv]);
    //return False;
    }
  else
    {
    if (LogErrs)
      LogError("Cnvs", 0, "SIUnit expected first. Line %u:(%s  %s  %s)", LineNo, (const char*)Values[denCnv], (const char*)Values[denUnit], (const char*)Values[denSclOff]);
    return False;
    }

  if (0)
    {
    dbgpln("%s %s %s", Values[0], Values[1], Values[2]);
    if (index>=0)
      {
      CDataCnv*p=Cnvs[index];
      long i=0;
      while (p)
        {
        dbgpln("%3i %14.2f %08x %08x %s", i++, 1/p->dScl, p, p->SICnv(), p->pTxt);
        p=p->m_pNxt;
        };
      }
    else
      dbgpln(" ???? index<0");
    }
  return True;
  }

//---------------------------------------------------------------------------

//enum CNVColsCSV
//  {
//  cenCnv,
//  cenUnit,
//  cenDefault,
//  cenSclOff,
//  cenMinMax,
//  cenDescription
//  };
//
//BOOL CCnvsMngr::LoadCnvLineCSV(DWORD LineNo, char* pLine, BOOL LogErrs, flag PrjCnv)
//  {
//  CSVColArray c
//  CSVColArray f
//  int Quote,nParms;
//  int nFlds = ParseCSVTokens(pLine, c, Quote);
//  if (Quote)
//    {
//    if (LogErrs)
//      LogError("Cnvs", 0, "scd_cnvs file line %u: Quotes mismatched", LineNo);
//    return False;
//    }
//  if (nFlds<4)
//    {
//    if (LogErrs)
//      LogError("Cnvs", 0, "scd_cnvs file line %u: Too few fields", LineNo);
//    return False;       
//    }
//
//  //get cnv...
//  if (c[cenCnv]==NULL || strlen(c[cenCnv])==0)
//    {
//    if (LogErrs)
//      LogError("Cnvs", 0, "scd_cnvs file line %u: Conversion not specified", LineNo);
//    return False;
//    }
//
//  //get default...
//  flag Default = (c[cenDefault] && strlen(c[cenDefault])>0 && ((c[cenDefault])[0]=='y' || (c[cenDefault])[0]=='Y'));
//
//  //get scale, offset...
//  flag Primary = True;
//  double Scl = 1.0;
//  double Ofs = 0.0;
//  if (c[cenSclOff] && strlen(c[cenSclOff])>0)
//    {
//    nParms = ParseCSVFunction(c[cenSclOff], f, Quote) - 1;
//    if (Quote)
//      {
//      if (LogErrs)
//        LogError("Cnvs", 0, "scd_cnvs file line %u: Valid ScaleOffset expected", LineNo);
//      return False;
//      }
//    if (stricmp(f[0],"SIUnit")==0)
//      Primary = True;
//    else if (stricmp(f[0],"scloff")==0)
//      {
//      if (nParms!=2)
//        {
//        if (LogErrs)
//          LogError("Cnvs", 0, "scd_cnvs file line %u: Valid ScaleOffset expected 'SclOff(Scale,Offset)'", LineNo);
//        return False;
//        }
//      Scl = SafeAtoF(f[1]);
//      Ofs = SafeAtoF(f[2]);
//      Primary = False;
//      }
//    else
//      {
//      if (LogErrs)
//        LogError("Cnvs", 0, "scd_cnvs file line %u: Valid ScaleOffset expected", LineNo);
//      return False;
//      }
//    }
//  
//  //get min, max...
//  double Min = 0.0;
//  double Max = 1.0;
//  if (c[cenMinMax] && strlen(c[cenMinMax])>0)
//    {
//    nParms = ParseCSVFunction(c[cenMinMax], f, Quote) - 1;
//    if (Quote)
//      {
//      if (LogErrs)
//        LogError("Cnvs", 0, "scd_cnvs file line %u: Valid MinMax expected", LineNo);
//      return False;
//      }
//    if (stricmp(f[0],"MinMax")==0)
//      {
//      if (nParms!=2)
//        {
//        if (LogErrs)
//          LogError("Cnvs", 0, "scd_cnvs file line %u: Valid MinMax expected 'MinMax(Min,Max)'", LineNo);
//        return False;
//        }
//      Min = SafeAtoF(f[1]);
//      Max = SafeAtoF(f[2]);
//      }
//    else
//      {
//      if (LogErrs)
//        LogError("Cnvs", 0, "scd_cnvs file line %u: Valid MinMax expected", LineNo);
//      return False;
//      }
//    }
//  else
//    {
//    //if (LogErrs)
//    //  LogError("Cnvs", 0, "scd_cnvs file line %u: Valid MinMax expected", LineNo);
//    //return False;
//    }
//
//  //add cnv...
//  CCnvIndex index = FindCnv(c[cenCnv]);
//  if ((index)<0 && Primary)
//    {//Add new conversion family!!!
//    index = AddSI(c[cenCnv], c[cenUnit], c[cenDescription], Min, Max, PrjCnv);
//    }
//  else if (index>=0 && !Primary)
//    {
//    if (Cnvs[index]->Find(c[cenUnit]))
//      {
//      //if (LogErrs)
//      //  LogError("Cnvs", 0, "scd_cnvs file line %u: Unit '%s' for '%s' allready defined", LineNo, c[cenUnit], c[cenCnv]);
//      return False;
//      }
//    //Cnvs[index]->SecondaryCnv(c[cenUnit], Scl, Ofs, Min, Max);
//    Cnvs[index]->AddSecondaryCnv(c[cenUnit], Scl, Ofs, PrjCnv, false);
//    }
//  else if (index>=0 && Primary)
//    {
//    //if (LogErrs)
//    //  LogError("Cnvs", 0, "scd_cnvs file line %u: SIUnit for '%s' allready defined", LineNo, c[cenCnv]);
//    return False;
//    }
//  else
//    {
//    if (LogErrs)
//      LogError("Cnvs", 0, "scd_cnvs file line %u: SIUnit expected first", LineNo);
//    return False;
//    }
//  //if (Default)
//  //  Cnvs[index]->DefCnvIs(c[cenUnit]);
//  return True;
//  }

//---------------------------------------------------------------------------

int CCnvsMngr::LoadCnvsADO(char* Filename, flag PrjCnv, BOOL LogErrs/*=TRUE*/)
  {
  int NewCnvsLoadedCnt = 0;
  if (Cnvs.GetSize()==0)
    Init();

/*CStopWatch SW;
SW.Start();*/

  BOOL OK = TRUE;
  CFileStatus State;
  if (CFile::GetStatus(Filename, State))
    {
    char* ColNames[] = {"Conversion", "Unit", "ScaleOffset", NULL};
    CDBHelper DB(Filename, "Cnvs");
    //CExcelDBHelper DB(Filename, "Cnvs");
    if (DB.Open(DBConnect_Default, ColNames[0], DBH_HasHeadings|DBH_ReadOnly|DBH_ForwardOnly|DBH_TrimStrings))
    //if (DB.Open("", TRUE, FALSE, TRUE))
      {
      if (DB.CheckFieldNames(ColNames))
        {
        CStringArray Values;
        while (!DB.IsEOF())
          {
          DB.GetNextRow(Values);
          if (ParseCnvLineADO(DB.GetLineNo(), Values, LogErrs, PrjCnv))
            NewCnvsLoadedCnt++;
          }
        }
      else
        {
        if (LogErrs)
          LogError("Cnvs", 0, "Correct column field names for '%s' not found.", Filename);
        OK = FALSE;
        }
      DB.Close();
      }
    else
      {
      if (LogErrs)
        {
        LogError("Cnvs", 0, "Unable to open '%s'", Filename);
        LogError("Cnvs", 0, "%s", DB.ErrorString());
        }
      OK = FALSE;
      }
    }
  else
    {
    if (LogErrs)
      LogError("Cnvs", 0, "'%s' not found", Filename);
    OK = FALSE;
    }
  

  #if dbgCnvs
  if (dbgCnvsList())
    DumpCnvs();
  #endif
  if (!OK)
    return -1;
  return NewCnvsLoadedCnt;
  }

//---------------------------------------------------------------------------

CCnvIndex CCnvsMngr::AddPri(char* Fam, char* Txt, double Min, double Max_, char *Desc, flag PrjCnv)
  {
  if (Cnvs.GetSize()==0)
    Init();

  CCnvIndex index = FindCnv(Fam);
  if ((index)<0)
    index = AddSI(Fam, Txt, Desc ? Desc : "??", Min, Max_, PrjCnv);
  return index;
  }

//---------------------------------------------------------------------------

CCnvIndex CCnvsMngr::AddSec(CCnvIndex index, char* Txt, double Scl, double Off, flag PrjCnv)
  {
  if (index>=0)
    {
    if (Cnvs[index]->Find(Txt))
      return index;
    Cnvs[index]->AddSecondaryCnv(Txt, Scl, Off, PrjCnv);
    }
  return index;
  }

//---------------------------------------------------------------------------

CCnvIndex CCnvsMngr::FindCnv(char* pFamName)
  {
  int i=0;
  while (i<Cnvs.GetSize())
    {
    if (strcmp(pFamName, Cnvs[i]->pSI->pFam)==0)
      return i;
    i++;
    }
  return -1;
  }

//---------------------------------------------------------------------------

void CCnvsMngr::DumpCnvs()
  {
  dbgpln("==================== Conversions ====================");
  int i=0;
  while (i<Cnvs.GetSize())
    {
    CDataCnv* p = Cnvs[i];
    while (p)
      {
      dbgpln("%3d:%12s  %12s %14.6g %14.6g %14.6g %14.6g", i, p->Fam(), p->pTxt, p->dScl, p->dOff*p->dOffMult, p->MinValue(), p->MaxValue());//, p->bIsDefault);
      p = p->m_pNxt;
      }
    i++;
    }
  dbgpln("=====================================================");
  }

//---------------------------------------------------------------------------

void CCnvsMngr::Clear()
  {
  //reset conversion "overwrite"...
  for (int i=0; i<Cnvs.GetSize(); i++)
    Cnvs[i]->SetOverTxt("");

  //delete all conversions that are project related...
  for (i=0; i<Cnvs.GetSize(); i++)
    {
    if (Cnvs[i]->PrjCnv())
      {
      delete Cnvs[i];
      Cnvs.RemoveAt(i);
      }
    else
      Cnvs[i]->Clear();
    }
  };

//---------------------------------------------------------------------------

void CCnvsMngr::Recover()
  {
  if (PrjIniFile())
    {
    CProfINIFile PF(PrjIniFile());
    for (int i=0; i<Cnvs.GetSize(); i++)
      if (Cnvs[i]->Fam() && strlen(Cnvs[i]->Fam())>0)
        {
        Strng O=PF.RdStr("Conversions", Cnvs[i]->Fam(), "");
        Cnvs[i]->SetOverTxt(O() ? O() : "");
        }
    }
  };

//---------------------------------------------------------------------------

void CCnvsMngr::Save()
  {
  if (PrjIniFile())
    {
    CProfINIFile PF(PrjIniFile());
    for (int i=0; i<Cnvs.GetSize(); i++)
      if (Cnvs[i]->Fam() && strlen(Cnvs[i]->Fam())>0)
        PF.WrStr("Conversions", Cnvs[i]->Fam(), Cnvs[i]->OverTxt());
//      PF.WrStr("Conversions", Cnvs[i]->Fam(), Cnvs[i]->OverTxt() ? Cnvs[i]->OverTxt() : "");
    }
  };

//===========================================================================
//
//
//
//===========================================================================

CArray<CDataCnv*, CDataCnv*> Cnvs;
CCnvsMngr gs_CnvsMngr;

//===========================================================================
void AppendCnvText(char *buff, char * CnvTxt)
  {
  if (CnvTxt && strlen(CnvTxt)>0)
    {
    strcat(buff, " (");
    strcat(buff, CnvTxt);
    strcat(buff, ")");
    }
  }
void AppendCnvText(Strng &buff, char * CnvTxt)
  {
  if (CnvTxt && strlen(CnvTxt)>0)
    {
    buff+=" (";
    buff+=CnvTxt;
    buff+=")";
//    buff.Set("%s (%s)", buff(), CnvTxt);
    }
  }
//===========================================================================
