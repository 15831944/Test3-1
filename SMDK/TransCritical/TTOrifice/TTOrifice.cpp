//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
//   Time-stamp: <2007-02-25 01:15:34 Rod Stephenson Transcritical Pty Ltd>
// Copyright (C) 2005 by Transcritical Technologies Pty Ltd and KWA
//   CAR Specific extensions by Transcritical Technologies Pty Ltd
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "TTOrifice.h"
#include "..\ttcommon\utils.h"
//#pragma optimize("", off)

#define dbgModels 1

//====================================================================================
static MInitialiseTest InitTest("Digest");
static MSpeciePtr  spWater         (InitTest, "H2O(l)", false);
static MSpeciePtr  spWaterVapor    (InitTest, "H2O(g)", false);


//====================================================================================



//====================================================================================

const long idIn  = 0;
const long idOut  = 1;

class CFlashSolver;


enum SlipModels {SFM_Homo,  SFM_Lock,  SFM_Faus,  SFM_Thom,  SFM_Zivi,  SFM_Baro,  SFM_Mood,  SFM_Wall,  SFM_HNEM};
enum OpMode {OM_Simple, OM_Full};


static MDDValueLst DD0[]=
  {
    {SFM_Homo, "Homogeneous"},
    {SFM_Lock, "Lockheed Martinelli"},
    {SFM_Faus, "Fauske"},
    {SFM_Thom, "Thom"},
    {SFM_Zivi, "Zivi"},
    {SFM_Baro, "Baroczy"},
    {SFM_Mood, "Moody"},
    {SFM_Wall, "Wallis"},
    {SFM_HNEM, "Homogeneous NonEquil"},
    {NULL}
  };

static MDDValueLst DD1[]=
  {
    {OM_Simple, "Simple"},
    {OM_Full, "Choking"},

    {NULL}
  };


static MDDValueLst DD2[]={
  {1, "Simple"},
  {2, "Linear"},
  {3, "50%"}, 
  {5, "25%"}, 
  {10, "Angle"}, 
  {11, "10%"}, 
  {NULL}
};

  
  
  
  
    

static const double slipData[8][4] = 
    { 

	{1,1,1,0},
	{.28,.64,.36,.07},
	{1,1,.5,0},
	{1,1,.89,.18},
	{1,1,.67,0.},
	{1,.74,.65,.13},
	{1,1,.6666666,0},
	{1,.72,.4,0.08}
    };



// Utility class for slip flow calculations
class SlipFlow 
{

    double mBb;
    double mn1;
    double mn2;
    double mn3;
public:
    SlipFlow(double _mBb, double _mn1, double _mn2, double _mn3): 

	mBb(_mBb), mn1(_mn1), mn2(_mn2), mn3(_mn3) 
    { }

  SlipFlow(int i) 
  {
    
    mBb = slipData[i][0];
    mn1 = slipData[i][1];
    mn2 = slipData[i][2];
    mn3 = slipData[i][3];
  }

    
	     

  double voidFraction(const MStream &fs) {
    double vapor = fs.MassFrac(MP_Gas);
    double slurry = fs.MassFrac(MP_SL);
    if (vapor < 1.0e-8) return 0.0;
    return 1./(1+mBb*pow(slurry/vapor, mn1)*
	       pow(fs.Density(MP_Gas)/fs.Density(MP_SL), mn2)*
	       pow(fs.DynamicViscosity(MP_SL)/fs.DynamicViscosity(MP_Gas), mn3));
  }


  double SlipDensity(const MStream &fs) 
  {
    double epsilon = voidFraction(fs);
    if (epsilon<1.0e-6) return fs.Density(MP_SL); 
    double x = fs.MassFrac(MP_Gas);
    return 1./(Sqr(1-x)/fs.Density(MP_SL)/(1-epsilon) + Sqr(x)/fs.Density(MP_Gas)/epsilon);
  }
  


  double Phi2Phase(double x, double G, double Y, double n=0.25) 
  {
    double B;
    G = sqrt(G);
    if (Y<9.5) B = 55/G;
    else if (Y < 28) B = 520/(Y*G);
    else B = 15000/(Y*Y*G);
    return 1+(Y*Y-1)*B*pow(x, (2-n)/2.)*pow(1-x, (2-n)/2.)+pow(x, 2-n);
  }
  

};


//---------------------------------------------------------------------------

static MInOutDefStruct s_IODefs[]=
  {
  //  Desc;        Name;    PortId; Rqd; Max; CnId, FracHgt;  Options;
    { "Inlet",  "Inlet",  idIn,     1,  1,     0,    1.0f,  MIO_In |MIO_Material },
    { "Outlet", "Outlet",  idOut,   1,   1,    0,    1.0f,  MIO_Out|MIO_Material },
    { NULL },
  };

double Drw_CTTOrifice[] = 
  { 
  MDrw_Poly, -5,-8, -5,8,
  MDrw_Poly, 0,-8, 0,-3,
  MDrw_Poly, 0,3, 0, 8,
  MDrw_Poly, 5,-8, 5,8,
  MDrw_End 
  };


double Drw_CTTOrifice1[] = {
  MDrw_Poly, -10, 0, -5, 0,
  MDrw_Poly, -5, -3, -5, 3, 5, -3, 5, 3, -5, -3,
  MDrw_Poly, 0, 0, 0, 5, -3, 5, -2, 7, 0,8, 2, 7, 3, 5, 0,5,
  MDrw_Poly, 5, 0, 10, 0,
  MDrw_Poly, 10, -5, 10, 5,
  MDrw_Poly, 12, -5, 12, -3,
  MDrw_Poly, 12, 5, 12, 3,
  MDrw_Poly, 14, -5, 14, 5,
  MDrw_Poly, 14, 0, 18, 0,

  MDrw_End
};


//---------------------------------------------------------------------------
DEFINE_TRANSFER_UNIT(CTTOrifice, "TTOrifice", DLL_GroupName)

void CTTOrifice_UnitDef::GetOptions()
  {
  SetDefaultTag("ORI");
  SetDrawing("HeatExchange", Drw_CTTOrifice1);
  SetTreeDescription("TTechFT:Orifice");
  SetDescription("TODO: Two Phase Flow Orifice");
  SetModelSolveMode(MSolveMode_All);
  SetModelGroup(MGroup_Energy);
  SetModelLicense(MLicense_HeatExchange | 0x00010000);
  };

//---------------------------------------------------------------------------

CTTOrifice::CTTOrifice(MUnitDefBase *pUnitDef, TaggedObject * pNd) : 
MBaseMethod(pUnitDef, pNd),
m_VLE(this, VLEF_QPFlash, "VLE")
{
  //set defaults for ALL values...
  dEntryK = 1.0;
  dPipe = 0.4;
  dOut = 0.1;
  dLevel = 5.0;
  dOrificeHead = 3.0;
  bOn  = true;
  bPassThru = false;
  m_lOpMode = OM_Full;
  dValvePosition = 0.0;
  bControlValve = true;
  //todo: set some reasonable defaults....
  m_lSlipMode = SFM_Homo;
  dIn = dOut;
  dThick = 0.01;
  dFlashdT=dTin=C2K(25);
  dxVapor = 0.0;
  dSlipDensity=dInletDensity=1000.0;
  dDensityCorrection=0.0;
  dFlashP=dPin=dPout=dPOrificeIn=dPCritical=dSatP=dPOutActual=StdP;
  dMassFlow=dMassFlow1=dMassFlow2=dMassFlow3=dMassFlow4=0.0;
  dMassVelocity=0.0;
  dPValve = 0.0;
  dValveData[0] = 300;
  dValveData[1] = 6000;
  dPipeVelocity = 0.0;
  lValveDataPts = 2;
  for (int i=2; i<=10; i++) dValveData[i]=0.0;
}


//---------------------------------------------------------------------------

CTTOrifice::~CTTOrifice()
  {
  }

//---------------------------------------------------------------------------

void CTTOrifice::Init()
{
  SetIODefinition(s_IODefs);
}

//---------------------------------------------------------------------------

void CTTOrifice::BuildDataFields()
{

#ifdef TTDEBUG
  DD.CheckBox("TTDBG", "",  &bTTDebug, MF_PARAMETER);
#endif
  DD.CheckBox("On", "",  &bOn, MF_PARAMETER|MF_SET_ON_CHANGE);
  DD.CheckBox("PassThru", "",  &bPassThru, MF_PARAMETER|MF_SET_ON_CHANGE);
  DD.CheckBox("LCValve", "",  &bControlValve, MF_PARAMETER|MF_SET_ON_CHANGE);
  
  DD.Long  ("SlipModel",    "",     &m_lSlipMode,   MF_PARAMETER|MF_SET_ON_CHANGE, DD0);
  DD.Long  ("OpMode",    "",     &m_lOpMode,   MF_PARAMETER|MF_SET_ON_CHANGE, DD1);
  DD.Double("FlashPressure", "", &dFlashP, MF_RESULT, MC_P("kPa"));
  DD.Double("TemperatureDrop", "", &dFlashdT, MF_RESULT, MC_dT("C"));
  DD.Double("VaporFraction", "", &dxVapor, MF_RESULT, MC_Frac("%"));
  DD.Double("SlipDensity", "", &dSlipDensity, MF_RESULT, MC_Rho);
  DD.Text("Valve and Entry Characteristics");

  DD.Double ("PipeD", "", &dPipe, MF_PARAMETER, MC_L("mm"));
  DD.Double ("PipeVelocity", "", &dPipeVelocity, MF_RESULT, MC_Ldt("m/s"));
  DD.Double ("OrificeDiameter", "", &dOut, MF_PARAMETER, MC_L("mm"));
  DD.Double ("OrificeInletD", "", &dIn, MF_PARAMETER|MF_INIT_HIDDEN, MC_L("mm"));
  DD.Double ("OrificeThickness", "", &dThick, MF_PARAMETER|MF_INIT_HIDDEN, MC_L("mm"));
  DD.Double("InletLevel", "", &dLevel, MF_PARAMETER, MC_L("m"));  
  DD.Double("OrificeHead", "", &dOrificeHead, MF_PARAMETER, MC_L("m"));
  DD.Double("EntryK", "", &dEntryK, MF_PARAMETER, MC_None);
  DD.Show(bControlValve);
  DD.Double("ValvePosition", "", &dValvePosition, MF_PARAMETER, MC_Frac("%"));
  DD.Double("ValveK", "", &dValveK, MF_RESULT, MC_None);
  DD.Double("ValvePressureDrop", "", &dPValve, MF_RESULT, MC_P("kPa"));
  DD.Show();
  DD.Double("OrificeEntryPressure", "", &dPOrificeIn, MF_RESULT, MC_P("kPa"));


  DD.Text("Stream Data");
  DD.Double("InletPressure", "", &dPin, MF_RESULT, MC_P("kPa"));
  DD.Double("InletDensity", "", &dInletDensity, MF_RESULT, MC_Rho);
  DD.Double("DensityCorrection", "", &dDensityCorrection, MF_RESULT, MC_None);
  
  DD.Double("POutActual", "", &dPOutActual, MF_RESULT|MF_NO_VIEW, MC_P("kPa")); //Use MF_NO_VIEW if user will never see this but parameter must be saved and recovered

  DD.Double("DownstreamPressure", "", &dPout, MF_PARAMETER, MC_P("kPa"));
  DD.Double("SaturationPressure", "", &dSatP, MF_RESULT, MC_P("kPa"));
  DD.Double("TemperatureIn", "", &dTin, MF_RESULT, MC_T("C"));  
  DD.Double("TemperatureDrop", "", &dFlashdT, MF_RESULT, MC_dT("C"));
  DD.Text("Flow and Pressure Results");

  DD.Double("MassFlow", "", &dMassFlow, MF_RESULT, MC_Qm("kg/s"));
  DD.Double("MassFlow1", "", &dMassFlow1, MF_RESULT, MC_Qm("kg/s"));
  DD.Double("MassFlow2", "", &dMassFlow2, MF_RESULT, MC_Qm("kg/s"));
  DD.Double("MassFlow3", "", &dMassFlow3, MF_RESULT, MC_Qm("kg/s"));
  DD.Double("MassFlow4", "", &dMassFlow4, MF_RESULT, MC_Qm("kg/s"));
  DD.Double("MassVelocity", "", &dMassVelocity, MF_RESULT|MF_INIT_HIDDEN, MC_None);
  DD.Double("PCritical", "", &dPCritical, MF_RESULT, MC_P("kPa"));
  DD.Show(bControlValve);
  DD.Page("Valve Cv");
  DD.Long  ("ValveDataPts",    "",     &lValveDataPts,   MF_PARAMETER|MF_SET_ON_CHANGE, DD2);
  DD.CheckBox("LinearInterpolate", "", &bValveLinear,  MF_PARAMETER|MF_SET_ON_CHANGE);
  CString Tg;
  for (int i=0; i<lValveDataPts; i++) {
    Tg.Format("Valve Cv %2d", i);
    DD.Double((char*)(const char*)Tg, "", dValveData+i, MF_PARAMETER, MC_None);
  }
  DD.Double("ValveCv@posn", "", &dValveCv, MF_RESULT, MC_None);
  DD.Show();
}

//---------------------------------------------------------------------------

bool CTTOrifice::ConfigureJoins()
  { 
  MBaseMethod::ConfigureJoins();
  //Joins.Count=2;
  return true; 
  }

//---------------------------------------------------------------------------

bool CTTOrifice::EvalJoinPressures()
  {
  Joins[0].SetProbalP(dPOutActual);
  return true;
  }

//===========================================================================
//
//    Choke and slip calculations for stream
//
//===========================================================================
//---------------------------------------------------------------------------



double CTTOrifice::ValveCv() 
{
  switch (lValveDataPts) {
  case 1:
    return dValveData[0]*dValvePosition;
  case 2: 
    return dValveData[0] + (dValveData[1]-dValveData[0])*dValvePosition;
  case 3:
    if (bValveLinear)
      return linn(dValvePosition, dValveData, 3);
    else
      return poly3(dValvePosition, dValveData);
  default:
    if (bValveLinear)
      return linn(dValvePosition, dValveData, lValveDataPts);
    else
      return polyn(dValvePosition, dValveData, lValveDataPts);
  }
}





void CTTOrifice::EvalProducts()
{
  try           
    {
    MStream InStream;
    FlwIOs.AddMixtureIn_Id(InStream, idIn);

    dDensityCorrection = dcf(K2C(InStream.T));

    const double Pi = FlwIOs[FlwIOs.First[idIn]].Stream.getP(); //get pressure of input stream 
    InStream.P = Pi;

    MStream & OutStream = FlwIOs[FlwIOs.First[idOut]].Stream;
    OutStream = InStream;
    dInletDensity = InStream.Density(MP_Liq);
    SlipFlow s1(m_lSlipMode);
    if (bOn) 
      {
      //same parameter sanity checks
      dPipe = Max(dPipe, 1.0e-6);
      dEntryK = Max(dEntryK, 1.0e-9);
      dFlashP = InStream.SaturationP(InStream.T);

      if (!bPassThru) 
        m_VLE.PFlash(OutStream, dPout);
      OutStream.P = dFlashP;
      dPOutActual = bPassThru ? dPin : dFlashP;

      dPin = Pi;
      double deltaP = GTZ(Pi - dPout);
      double den = InStream.Density();
      double area = CircleArea(dPipe);
      dMassFlow =  area*sqrt(2*den*deltaP/dEntryK);
      dMassFlow1 = InStream.Mass();
      dPipeVelocity = InStream.Volume()/area;
      dxVapor = OutStream.Mass(MP_Gas)/OutStream.Mass();
      double dGIn = dMassFlow1/area;
      double dP1 = .5*dEntryK*Sqr(dGIn)/den/1000.;
      if (bControlValve) {
	dValveCv = ValveCv();
	dValveK = 200.*Sqr(area*3600/(.866*dValveCv));
	dPValve = .5*dValveK*Sqr(dGIn)/den/1000.;
      } else dPValve=0.0;

      dPOrificeIn = Pi + (dLevel+dOrificeHead) * 9.81*den/1000.-dP1-dPValve;

      dSatP = m_VLE.SaturationP(InStream, InStream.T);
      dTin = InStream.T;
      dFlashdT = InStream.T - OutStream.T;
      dSlipDensity = s1.SlipDensity(OutStream);
      MStream mstmp;
      mstmp = InStream;

      dMassVelocity = massVelocity(mstmp, dPOrificeIn, dPout);  CString Tg;

      dMassFlow2 = dMassVelocity*CircleArea(dOut);// Density correction, based on bayer.exe AMIRA 
      dMassFlow3 = chokeMassVelocity(InStream, dPOrificeIn)*CircleArea(dOut);
      if (dPCritical>dPout) 
        {   // Choked Flow
        dMassFlow4 = dMassFlow3;
        }
      else 
        {
        dMassFlow4 = dMassFlow2;
        }
      } 
    else 
      {
      dMassFlow4 = dMassFlow1;
      dFlashdT = 0;
      dPOutActual = dPin;
      }
    }
  catch (MMdlException &e)  
    {
      Log.Message(MMsg_Error, e.Description);
    }
  catch (MFPPException &e)
    {
      e.ClearFPP();
      Log.Message(MMsg_Error, e.Description);
    }
  catch (MSysException &e)
    {
      Log.Message(MMsg_Error, e.Description);
    }
  catch (...)
    {
      Log.Message(MMsg_Error, "Some Unknown Exception occured");
    }
}

//--------------------------------------------------------------------------

void CTTOrifice::ClosureInfo(MClosureInfo & CI)
{
  if (CI.DoFlows())
    {
      //CI.AddPowerIn(0, -m_dActualDuty);
    }
}
//====================================================================================


double CTTOrifice::massVelocity(MStream  ms, double pIn, double pOut) 
{

  if (pOut > pIn) return 0.0;
  double pSat = ms.SaturationP(ms.T);
  if (pOut > pSat) {
    double den = ms.Density();
    return sqrt(2000.*(pIn-pOut)*den);
  }
  // Main Cases

  SlipFlow s1(m_lSlipMode);
  double den1, den2, den3;
  // Initial pressure below saturation
  if (pIn < pSat) {
    double pMid = (pIn+pOut)/2;
    m_VLE.PFlash(ms, pIn);
    den1 = s1.SlipDensity(ms);
    m_VLE.PFlash(ms, pMid);
    den2 = s1.SlipDensity(ms);
    m_VLE.PFlash(ms, pOut);     

    den3 = s1.SlipDensity(ms);
    return den3*sqrt(2000.*(pIn-pOut)*(1./den1+4./den2+1./den3)/6.);
  }
  // Default Case, Initial Pressure above saturation
  double den = ms.Density();   // Slurry (nonflashed) Density
  double I1 = sqrt(2000.*(pIn-pSat)/den);
  double pMid = (pSat+pOut)/2;
  m_VLE.PFlash(ms, pMid);
  den2 = s1.SlipDensity(ms);
  m_VLE.PFlash(ms, pOut);
  den3 = s1.SlipDensity(ms);
  double I2 = sqrt(2000.*(pSat-pOut)*(1./den+4./den2+1./den3)/6.);
  return den3*(I1+I2);
  
 
}



double CTTOrifice::chokeMassVelocity(MStream  ms, double pIn) 
{
  double pSat = ms.SaturationP(ms.T);
  double pMin = pSat/2.;
  double dP = pSat-pMin;
  double gmax = -1000.;
  double pcrit;
  for (int i=0; i<=20; i++) {
    MStream mtmp;
    mtmp = ms;
    double p = pSat - dP*i/20.;
    double mv = massVelocity(mtmp, pIn, p);
    if (mv>gmax) {
      pcrit = p;
      gmax = mv;
    }
  }
  dPCritical = pcrit;
  return gmax;
}


// Density correction, based on bayer.exe AMIRA 
double dcf(double tc) {
  if (tc<144.2467) return 1.0;
  /// Dont apply correction for low temperatures; this just happens that
  // dcf(144.2467) === 1.00000, so no discontinuity!
  else return 1.1737e-5*tc*tc -2.3329e-3*tc+1.0923;
}
