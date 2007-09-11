//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
#define  __QSTEAM_CPP
#include "sp_db.h"
//#include "errorlog.h"
//#include "surfaces.h"
//#include "optoff.h"

#define  QALH2OName       "QALH2O"
                   
// ==========================================================================
//
//
//
// ===========================================================================

/* Old QAL LatentHeatVap not used
double SMSteam::msLatentHeatVap(double T_, SpMArray * pMA)
  {
  
  double t1=Range(0.0, (647.3-T_)/647.3, 1.0);
  double t2=0.779221*pow(t1, 1./3.) + 4.62668*pow(t1, 5./6.) - 1.07931*pow(t1, 7./8.);
  double t3=t1*(-3.87446+t1*(2.94553+t1*(-8.06395+t1*(11.5633-6.02884*t1))));
  return 2500.9*(t2+t3);
  };
*/
//--------------------------------------------------------------------------

inline double SatVT(double Press)
  {
  double LogP = log(Max(0.01, Press) / 1000.0);
  if (Press < 12330.0) 
    return 42.6776 - 3892.7/(LogP-9.48654);
  else
    return -387.592 - 12587.5/(LogP-15.2578);
  }

//--------------------------------------------------------------------------

H2OEQNBEGIN(QAL_WaterHeatCapacity, "QALWaterCp")
virtual double AtTP(double Temp, double Press) 
  { 
  //double Tc = Range(??, K_2_C(Temp), ??); // Kelvin to Centigrade
  double Tc = K_2_C(Temp); // Kelvin to Centigrade
  double CpLiq = 4.187229 + Tc*(7.402096e-4 
                          + Tc*(-3.719241e-5 + Tc*(5.18192e-7 
                          + Tc*(-2.3319045e-9 + Tc* 3.894306e-12))));
  return CpLiq;
  };
H2OEQNEND

//--------------------------------------------------------------------------

H2OEQNBEGIN(QAL_SteamHeatCapacity, "QALSteamCp")
virtual double AtTP(double Temp, double Press)
  { 
  //const double Tk = Range(273.16, Temp, 1073.15);
  const double Tk = Temp;
  const double tSat = SatVT(Press);  // Find saturation temperature
  const double Pm = Press/1000.0;  // Convert to MPa
  const double t1 = 1.610693 + 5.472051e-2 * Pm + 7.517537e-4 * Pm * Pm;
  const double t2 = 3.383117e-4 - 1.975736e-5 * Pm - 2.87409e-7 * Pm * Pm;
  const double t3 = 1707.82 + tSat * ( -16.99419 + tSat * (0.062746295
                    + tSat * ( -1.0284259e-4 + 6.4561298e-8 * tSat )));
  double CpVap = t1  + 2.* t2 * Tk - t3 * exp( ( tSat - Tk ) / 45.0 ) * ( -1.0/45.0);
  return CpVap;
  }
H2OEQNEND

//--------------------------------------------------------------------------

H2OEQNBEGIN(QAL_WaterEnthalpy, "QALWaterH")
virtual double AtTP(double Temp, double Press)
  {
  //double Tc = Range(??, K_2_C(Temp), ??); // Kelvin to Centigrade
  double Tc = K_2_C(Temp); // Kelvin to Centigrade

  double HLiq = 7.608868e-3 + Tc*(4.187229 + Tc*(3.701048e-4 
                            + Tc*(-1.239747e-5 + Tc*(1.295473e-7 
                            + Tc*(-4.663809e-10 + Tc * 6.49051e-13)))));
  return HLiq; 
  }
H2OEQNEND

//--------------------------------------------------------------------------

H2OEQNBEGIN(QAL_SteamEnthalpy, "QALSteamH")
virtual double AtTP(double Temp, double Press)
  {
  //const double Tk = Range(273.16, Temp, 1073.15);
  const double Tk = Temp;
  const double tSat = SatVT(Press);  // Find saturation temperature
  const double Pm = Press/1000.0;  // Convert to MPa
  const double t0 = 2041.21 - 40.40021 * Pm - 0.48095 * Pm * Pm;
  const double t1 = 1.610693 + 5.472051e-2 * Pm + 7.517537e-4 * Pm * Pm;
  const double t2 = 3.383117e-4 - 1.975736e-5 * Pm - 2.87409e-7 * Pm * Pm;
  const double t3 = 1707.82 + tSat * ( -16.99419 + tSat * (0.062746295
                    + tSat * ( -1.0284259e-4 + 6.4561298e-8 * tSat )));
  double HVap = t0 + t1 * Temp + t2 * Tk * Tk - t3 * exp( (tSat - Tk) / 45.0 );
  return HVap;
  }
H2OEQNEND

//--------------------------------------------------------------------------

H2OEQNBEGIN(QAL_WaterDensity, "QALWaterRho")
virtual double AtTP(double Temp, double Press)
  {
  return 1.0;
  }
H2OEQNEND

//--------------------------------------------------------------------------

H2OEQNBEGIN(QAL_SteamDensity, "QALSteamRho")
virtual double AtTP(double Temp, double Press)
  {
  double Rho=0.8037; //0.8037 from old QAL specie database
  return Rho*Press/Std_P*Std_T/GTZ(Temp); //todo : check!
  //return Rho*Press/Std_P*ZeroCinK/GTZ(Temp);  //should be this???
  //return 0.8037; 
  }
H2OEQNEND

//--------------------------------------------------------------------------

H2OEQNBEGIN(QAL_WaterVapourPress, "QALWaterVp")
virtual double AtT(double Temp) 
  {
  double P;
  if ( Temp < 50.0)    // Low Temperature
    P=1000.0 * exp(-3892.7 / (50.0 - 42.6776) + 9.48654); 
  else if ( Temp < 600.0)
    P=1000.0 * exp(-3892.7 / (Temp - 42.6776) + 9.48654);
  else
    P=1000.0 * exp(-12587.5 / (Temp + 387.592) + 15.2578);
  return P;
  }
H2OEQNEND

// ==========================================================================
//
//
//
// ===========================================================================

#define DllImportExport

DEFINE_TAGOBJ(QALH2OProps);
class QALH2OProps : public H2OPropertiesBlock
  {
  public:

    QALH2OProps(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);
    
    virtual ~QALH2OProps();
    virtual void   BuildDataDefn(DataDefnBlk & DDB)     { };
    virtual flag   DataXchg(DataChangeBlk & DCB)        { return 0; };
    virtual flag   ValidateData(ValidateDataBlk & VDB)  { return 0; };

  };

//---------------------------------------------------------------------------

IMPLEMENT_TAGOBJ(QALH2OProps, H2OPropsGroup, QALH2OName, "", "",  "", TOC_DYNAMICFULL | TOC_PROBAL | TOC_QAL | TOC_GRP_GENERAL, 
                 "QAL H2O Props",
                 "Properties for Water & Steam");

//---------------------------------------------------------------------------

QALH2OProps::QALH2OProps(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  H2OPropertiesBlock(pClass_, pTag, pAttach, eAttach)
  {
  for (int i=0; i<2; i++)
    {
    m_Liq[i].m_Rho.SetEqn(ET_Std, new QAL_WaterDensity(NULL, NULL),        273.16, 623.16);
    m_Liq[i].m_Cp.SetEqn(ET_Std,  new QAL_WaterHeatCapacity(NULL, NULL),   273.16, 623.16);
    m_Liq[i].m_H.SetEqn(ET_Std,   new QAL_WaterEnthalpy(NULL, NULL),       273.16, 623.16);
    m_Vap[i].m_Rho.SetEqn(ET_Std, new QAL_SteamDensity(NULL, NULL),        273.16, 1073.16);
    m_Vap[i].m_Cp.SetEqn(ET_Std,  new QAL_SteamHeatCapacity(NULL, NULL),   273.16, 1073.16);
    m_Vap[i].m_H.SetEqn(ET_Std,   new QAL_SteamEnthalpy(NULL, NULL),       273.16, 1073.16);

    m_Common[i].m_Vp.SetEqn(ET_Std, new QAL_WaterVapourPress(NULL, NULL),  273.16, 623.16);
    }
  }  
    
//--------------------------------------------------------------------------

QALH2OProps::~QALH2OProps()
  {
  for (int i=0; i<2; i++)
    {
    m_Liq[i].m_Rho.Clear();
    m_Liq[i].m_Cp.Clear();
    m_Liq[i].m_H.Clear();
    m_Vap[i].m_Rho.Clear();
    m_Vap[i].m_Cp.Clear();
    m_Vap[i].m_H.Clear();

    m_Common[i].m_Vp.Clear();
    }
  }

//--------------------------------------------------------------------------
// ===========================================================================
