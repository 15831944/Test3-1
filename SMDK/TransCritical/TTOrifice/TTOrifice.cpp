//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
//   Time-stamp: <2007-03-05 02:04:31 Rod Stephenson Transcritical Pty Ltd>
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

  
  
  
  
    

static const double slipData[9][4] = 
    { 

	{1,1,1,0},            //"Homogeneous"	       
	{.28,.64,.36,.07},    //"Lockheed Martinelli" 
	{1,1,.5,0},	      //"Fauske"	       
	{1,1,.89,.18},	      //"Thom"		       
	{1,1,.67,0.},	      //"Zivi"		       
	{1,.74,.65,.13},      //"Baroczy"	       
	{1,1,.6666666,0},     //"Moody"	       
	{1,.72,.4,0.08},      //"Wallis"	       
	{1,1,1,0}, 	      //"Homogeneous NonEquil"
    };



// Utility class for slip flow calculations
class SlipFlow 
{

  double mBb;
  double mn1;
  double mn2;
  double mn3;
  bool bAmiraCorrection;

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
    bAmiraCorrection = true;
  }

    
	     

  double voidFraction(const MStream &fs) {
    double vapor = fs.MassFrac(MP_Gas);
    double slurry = fs.MassFrac(MP_SL);
    double dc = bAmiraCorrection ? dcf(K2C(fs.T)) : 1.0;
    if (vapor < 1.0e-8) return 0.0;
    return 1./(1+mBb*pow(slurry/vapor, mn1)*
	       pow(fs.Density(MP_Gas)/(fs.Density(MP_SL)/dc), mn2)*
	       pow(fs.DynamicViscosity(MP_SL)/fs.DynamicViscosity(MP_Gas), mn3));
  }


  double SlipDensity(const MStream &fs) 
  {
    double epsilon = voidFraction(fs);
    double dc = bAmiraCorrection ? dcf(K2C(fs.T)) : 1.0;
    if (epsilon<1.0e-6) return fs.Density(MP_SL)/dc; 
    double x = fs.MassFrac(MP_Gas);
    return 1./(Sqr(1-x)/(fs.Density(MP_SL)/dc)/(1-epsilon) + Sqr(x)/fs.Density(MP_Gas)/epsilon);
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
  dSinglePhaseDP = 0.0;
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

  /*#ifdef TTDEBUG
  DD.CheckBox("TTDBG", "",  &bTTDebug, MF_PARAMETER);
  #endif */
  DD.CheckBox("On", "",  &bOn, MF_PARAMETER|MF_SET_ON_CHANGE);
  DD.CheckBox("PassThru", "",  &bPassThru, MF_PARAMETER|MF_SET_ON_CHANGE);
  DD.CheckBox("LCValve", "",  &bControlValve, MF_PARAMETER|MF_SET_ON_CHANGE);
  
  DD.Long  ("SlipModel",    "",     &m_lSlipMode,   MF_PARAMETER|MF_SET_ON_CHANGE, DD0);
  DD.Long  ("OpMode",    "",     &m_lOpMode,   MF_PARAMETER|MF_SET_ON_CHANGE, DD1);
  DD.Double("FlashPressure", "", &dFlashP, MF_RESULT, MC_P("kPa"));
  DD.Double("BoilingPtElev", "", &dBPE, MF_RESULT, MC_dT("C"));
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
  
  DD.Double("POutActual", "", &dPOutActual, MF_RESULT|MF_INIT_HIDDEN, MC_P("kPa"));
  //Use MF_NO_VIEW if user will never see this but parameter must be saved and recovered

  DD.Double("DownstreamPressure", "", &dPout, MF_PARAMETER, MC_P("kPa"));
  DD.Double("VaporFraction", "", &dxVapor, MF_RESULT, MC_Frac("%"));
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
  DD.Double("SinglePhaseDP", "", &dSinglePhaseDP, MF_RESULT, MC_P("kPa"));

  DD.Double("ChokeVelocity", "", &dChokeVelocity, MF_RESULT, MC_Ldt);
  DD.Double("HomogChokeVelocity", "", &dHomogChokeVelocity, MF_RESULT, MC_Ldt);
  DD.Double("ChokeMassVel", "", &dChokeMassVelocity, MF_RESULT, MC_None);
  DD.Double("HomoChokeMassVel", "", &dHomogMassChokeVelocity, MF_RESULT, MC_None);



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
#ifdef TTDEBUG
  DD.Page("Flash Densities");
  DD.Double ("PMax", "", &dPMax, MF_RESULT, MC_P);
  DD.Double ("PMin", "", &dPMin, MF_RESULT, MC_P);
  DD.Double ("Beta", "", &dBeta, MF_RESULT, MC_None);
  DD.Double ("GIn", "", &dGIn, MF_RESULT, MC_None);
  DD.Double ("r_est", "", &d_rEst, MF_RESULT, MC_None);
  DD.Double ("PCriticalEst", "", &dPCritEst, MF_RESULT, MC_P);
  
  for (int i=0; i<30; i++) {
    Tg.Format("Density %3d", i);
    DD.Double((char*)(const char*)Tg, "", dRhoData+i, MF_RESULT, MC_Rho);
  }
#endif

  
  m_VLE.BuildDataFields();     
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
    bool bValveFlash = false;
    InStream.P = Pi;

    MStream & OutStream = FlwIOs[FlwIOs.First[idOut]].Stream;
    OutStream = InStream;
    dInletDensity = InStream.Density(MP_Liq);
    SlipFlow s1(m_lSlipMode);
    if (bOn) {
      //same parameter sanity checks
      dPipe = Max(dPipe, 1.0e-6);
      dEntryK = Max(dEntryK, 1.0e-9);
      dFlashP = InStream.SaturationP(InStream.T);
      dBPE = InStream.BoilingPtElevation();
      OutStream.P = dFlashP;

      dPin = Pi;
      double deltaP = GTZ(Pi - dPout);
      double den = InStream.Density();
      double area = CircleArea(dPipe);
      double oarea = CircleArea(dOut);
      dMassFlow =  area*sqrt(2*den*deltaP/dEntryK);
      dMassFlow1 = InStream.Mass();
      dPipeVelocity = InStream.Volume()/area;
      dGIn = dMassFlow1/area;

      double dP1 = .5*dEntryK*Sqr(dGIn)/den/1000.;
      if (bControlValve) {
	dValveCv = ValveCv();
	if (dValveCv > 1.0) {
	  dValveK = 200.*Sqr(area*3600/(.866*dValveCv));
	  dPValve = .5*dValveK*Sqr(dGIn)/den/1000.;
	} else {
	  bValveFlash = true;
	}
      } else dPValve=0.0;

      dPOrificeIn = Pi + (dLevel+dOrificeHead) * 9.81*den/1000.-dP1;
      if (dPOrificeIn - dPValve < dFlashP && bControlValve) {
	Log.SetCondition(true, 0, MMsg_Warning, "Flashing in Valve");
      }
	

      if (!bValveFlash) dPOrificeIn -= dPValve;

      dSatP = m_VLE.SaturationP(InStream, InStream.T);
      dTin = InStream.T;
      MStream mstmp;
      mstmp = InStream;


      dMassVelocity = massVelocity(mstmp, dPOrificeIn, dPout); 


      dSinglePhaseDP = 0.5*Sqr(dMassFlow1/CircleArea(dOut))/den/1000.;
      dMassFlow2 = dMassVelocity*CircleArea(dOut);// Density correction, based on bayer.exe AMIRA 
      dMassFlow3 = chokeMassVelocity(InStream, dPOrificeIn)*CircleArea(dOut);


      if (dPCritical>dPout)  {   // Choked Flow
        dMassFlow4 = dMassFlow3;

	dPOutActual = dPCritical;
      }
      else {
	  dMassFlow4 = dMassFlow2;
	dPOutActual = dPout;
      }
      if (!bPassThru) 
        m_VLE.PFlash(OutStream, dPOutActual);

      if (bPassThru)
	dPOutActual = dPin;

      dxVapor = OutStream.Mass(MP_Gas)/OutStream.Mass();
      dFlashdT = InStream.T - OutStream.T;
      dSlipDensity = s1.SlipDensity(OutStream);

      dChokeVelocity =       criticalVelocity(OutStream);
      dHomogChokeVelocity   = criticalVelocity(OutStream, SFM_Homo);
      dChokeMassVelocity = criticalFlow(OutStream);
      dHomogMassChokeVelocity = criticalFlow(OutStream, SFM_Homo);

#ifdef TTDEBUG   // Flash densities...
      double dPData[30];
      mstmp = InStream;
      SlipFlow s1(m_lSlipMode);
      dPMax = dFlashP;
      dPMin = dFlashP - dFlashP/2.*29/30.;
      for (int i=0; i<30; i++) {
	double p = dFlashP - dFlashP/2.*i/30.;
	dPData[i] = p;

	if (m_lSlipMode == SFM_HNEM) 
	  dRhoData[i] = HNEFlash(InStream, p);
	else {
	  m_VLE.PFlash(mstmp, p);
	  dRhoData[i] = s1.SlipDensity(mstmp);
	}
	
      }
      dBeta = betaCoeff(dPData,  dRhoData, 30);
      double dG2 = dGIn*dGIn + 2*den*(dPin-dFlashP)*1000.;
      d_rEst = pow((dBeta+1)/(2*dBeta)/(1.-(dBeta-1)*dG2/(2000.*dFlashP*den)), 1.0/(dBeta-1));
      dPCritEst = d_rEst*dFlashP;

	  
#endif


    }  else {    // !bOn... bypass altogether and dont do calcs.
      dMassFlow4 = dMassFlow1;
      dFlashdT = 0;
      dPOutActual = dPin;
      //SetCondition(0, "Off");
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
  double I1 = 2000.*(pIn-pSat)/den;
  double I2;
  double pMid = (pSat+pOut)/2;
  if (m_lSlipMode != SFM_HNEM) {
    m_VLE.PFlash(ms, pMid);
    den2 = s1.SlipDensity(ms);
    m_VLE.PFlash(ms, pOut);
    den3 = s1.SlipDensity(ms);
  // Log.Message(MMsg_Warning, "Pin %8.2f Pout %8.2f Psat %8.2f", pIn, pOut, pSat);
  // Log.Message(MMsg_Warning, "den %8.2f den2 %8.2f den3 %8.2f I1 %8.2f I2 %8.2f", den, den2, den3, I1, I2);
  } else {
    den2 = HNEFlash(ms, pMid);
    den3 = HNEFlash(ms, pOut);
  }  
  I2 = (2000.*(pSat-pOut)*(1./den+4./den2+1./den3)/6.);
  return den3*sqrt(Sqr(dGIn/den)+I1+I2);
  
 
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
    // Log.Message(MMsg_Error, "Mass Velocity %8.2f", mv);
    if (mv>gmax) {
      pcrit = p;
      gmax = mv;
    }
  }
  dPCritical = pcrit;
  return gmax;
}


// Calculate critical velocity for flashed flow
double CTTOrifice::criticalVelocity(MStream ms, int slipMode) 
{
  if (slipMode == SFM_Default) slipMode = m_lSlipMode;
  MStream mtmp;
  mtmp = ms;
  SlipFlow s1(slipMode);
  double den1 = s1.SlipDensity(ms);
  m_VLE.PFlash(mtmp, ms.P-1.0);
  double den2 = s1.SlipDensity(mtmp);
  if (den1==den2) return 0.0;
  return Sqrt(1000./(den1-den2));
}

// Calculate critical flow for flashed flow
double CTTOrifice::criticalFlow(MStream ms, int slipMode) 
{
  if (slipMode == SFM_Default) slipMode = m_lSlipMode;
  MStream mtmp;
  mtmp = ms;
  SlipFlow s1(slipMode);
  double den1 = s1.SlipDensity(ms);
  m_VLE.PFlash(mtmp, ms.P-1.0);
  double den2 = s1.SlipDensity(mtmp);
  if (den1==den2) return 0.0;
  return den1*Sqrt(1000./(den1-den2));
}


    




// Orifice dP for incoming saturated two phase flow


double CTTOrifice::orificeDeltaP(MStream ms) {

  MStream mtmp;
  mtmp = ms;
  double pIn  = ms.P;
  SlipFlow s1(m_lSlipMode);
  double den1;
  den1 = s1.SlipDensity(mtmp);
  double Gin = ms.Mass()/CircleArea(dOut);
  double dp1 = .5*Sqr(Gin)/den1;
  double dp1old = dp1;
  for (int i=0; i<20; i++) {
    m_VLE.PFlash(mtmp, pIn - dp1);
    den1 = s1.SlipDensity(mtmp);
    dp1 = .5*Sqr(Gin)/den1;
    if (fabs(dp1old-dp1)<1.0e-3) return dp1;
  }
  return 0.0;

}


double betaCoeff(double *pdata, double* rhodata, int n)
{
  double sx(0), sy(0), sxy(0), sxx(0);
  for (int i=0; i<n; i++) {
    double lp = log(pdata[i]);
    double lr = log(rhodata[i]);

    sx+=lp;
    sy+=lr;
    sxx+=lp*lp;
    sxy+=lp*lr;
  }
  double delta = n*sxx-sx*sx;
  return (n*sxy-sx*sy)/delta;
}


// Density correction, based on bayer.exe AMIRA 
double dcf(double tc) {
  if (tc<144.2467) return 1.0;
  /// Dont apply correction for low temperatures; this just happens that
  // dcf(144.2467) === 1.00000, so no discontinuity!
  else return 1.1737e-5*tc*tc -2.3329e-3*tc+1.0923;
}







double CTTOrifice::HNEFlash(MStream ms, double p, double alpha, double flashmax)
{
  MStream mtmp;
  mtmp = ms;
  m_VLE.PFlash(mtmp, p);
  double x1;   // Non equilibrium vapor fraction
  double x = mtmp.MassFrac(MP_Gas);  // Equilibrium vapor fraction
  if (x>flashmax) 
    x1 = x;
  else
    x1 = pow(x, alpha);    
  m_VLE.SetFlashVapFrac(mtmp, x1, 0);
  //Log.Message(MMsg_Error, "T %8.2f P %8.2f x, %10.6f, x1 %10.6f", mtmp.T, mtmp.P, x, x1);

  SlipFlow s1(SFM_HNEM);
  return s1.SlipDensity(mtmp);
}
