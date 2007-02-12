//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
//   Time-stamp: <2007-02-12 14:04:13 Rod Stephenson Transcritical Pty Ltd>
// Copyright (C) 2005 by Transcritical Technologies Pty Ltd and KWA
//   CAR Specific extensions by Transcritical Technologies Pty Ltd
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "TTOrifice.h"
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


//===========================================================================
//
//  Flash Solver
//
//===========================================================================



class CFlashSolver : public MRootFinder
  {
  public:
    CFlashSolver(MStream & _iStream, double _p, MVLEBlk &_VLE): 
      MRootFinder("FlashSolver" ,s_Tol), iStream(_iStream), m_VLE(_VLE), pRqd(_p) {};
    ~CFlashSolver() {};
    double Function(double x);
    double setP(double P) { pRqd = P;}
    
  protected:
    MStream & iStream;
    MVLEBlk      &m_VLE;
    double pRqd;
    

    static MToleranceBlock s_Tol;
  };



MToleranceBlock CFlashSolver::s_Tol(TBF_Both, "CFlashSolver:CondensFinder", 0.0, 1.0e-12);

double CFlashSolver::Function(double x) 
{
  m_VLE.SetFlashVapFrac(iStream, x, VLEF_QVFlash);
  double dFlashP = iStream.SaturationP(iStream.T);
  return pRqd - dFlashP;
}


/*
double PFlash(MStream & inStream, double P) 
{
  CFlashSolver fn(inStream, dFlashP, m_VLE);

      


  if (fn.FindRoot(0., 0., 0.99)==RF_OK) {
	//Log.Message(MMsg_Error, "Found Root");
    dxVapor = fn.Result();
    m_VLE.SetFlashVapFrac(inStream, dxVapor, VLEF_QVFlash);
    return dxVapor
  } else {
    Log.Message(MMsg_Error, "Couldnt solve for root");
    return -1.0;
  } 


  
}
*/



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

//---------------------------------------------------------------------------
DEFINE_TRANSFER_UNIT(CTTOrifice, "TTOrifice", DLL_GroupName)

void CTTOrifice_UnitDef::GetOptions()
  {
  SetDefaultTag("ORI");
  SetDrawing("HeatExchange", Drw_CTTOrifice);
  SetTreeDescription("TTechFT:Orifice");
  SetDescription("TODO: Two Phase Flow Orifice");
  SetModelSolveMode(MSolveMode_Probal);
  SetModelGroup(MGroup_Energy);
  SetModelLicense(MLicense_HeatExchange | 0x00010000);
  };

//---------------------------------------------------------------------------

CTTOrifice::CTTOrifice(MUnitDefBase *pUnitDef, TaggedObject * pNd) : 
MBaseMethod(pUnitDef, pNd),
m_VLE(this, VLEF_QPFlash, "VLE")
{
  //default values...
  
  
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
  //DD.CheckBox("On", "",  &bOnline, MF_PARAMETER|MF_SET_ON_CHANGE);

  
  DD.Long  ("SlipModel",    "",     &m_lSlipMode,   MF_PARAMETER|MF_SET_ON_CHANGE, DD0);
  DD.Double ("PipeD", "", &dPipe, MF_PARAMETER, MC_L("mm"));
  DD.Double ("OrificeDiameter", "", &dOut, MF_PARAMETER, MC_L("mm"));
  DD.Double ("OrificeInletD", "", &dIn, MF_PARAMETER, MC_L("mm"));
  DD.Double ("OrificeThickness", "", &dThick, MF_PARAMETER, MC_L("mm"));
  DD.Double("FlashPressure", "", &dFlashP, MF_PARAMETER, MC_P("kPa"));
  DD.Double("TemperatureDrop", "", &dFlashdT, MF_PARAMETER, MC_dT("C"));
  DD.Double("VaporFraction", "", &dxVapor, MF_PARAMETER, MC_Frac);
  DD.Double("SlipDensity", "", &dSlipDensity, MF_RESULT, MC_Rho);
  DD.Double("EntryK", "", &dEntryK, MF_PARAMETER, MC_None);


  DD.Double("InletPressure", "", &dPin, MF_RESULT, MC_P("kPa"));
  DD.Double("DownstreamPressure", "", &dPout, MF_PARAMETER, MC_P("kPa"));


  DD.Double("OrificeEntryPressure", "", &dPOrificein, MF_RESULT, MC_P("kPa"));
  DD.Double("MassFlow", "", &dMassFlow, MF_RESULT, MC_Qm("kg/s"));
  DD.Double("MassFlow1", "", &dMassFlow1, MF_RESULT, MC_Qm("kg/s"));
  

    
  
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
    //Joins[0].SetProbalP(dFlashP);
    return true;
  }



//===========================================================================
//
//    Choke and slip calculations for stream
//
//===========================================================================



double ChokeMassVelocity(MStream & s) 
{
  
  return 0;
  


  
}

double SlipDensity(MStream &s, long SFM) 
{
  
  return 0;
  


  
}






//---------------------------------------------------------------------------

void CTTOrifice::EvalProducts()
{
  try           
    {
      MStream InStream;
      FlwIOs.AddMixtureIn_Id(InStream, idIn);
      double p = InStream.getP();
      Log.Message(MMsg_Error, "Pressure %8.3f", p);

      double MaxP = AtmosPress();
      long index = FlwIOs.First[idIn];
      while (index>=0)
        {
        MStream & QI = FlwIOs[index].Stream; //get reference to the actual input stream
        p = QI.getP();
        MaxP = Max(MaxP, p);
        Log.Message(MMsg_Note, "Pressure %8.3f", p);
        index = FlwIOs.Next_In[index][idIn]; //is there another stream connected to this port
        }

      InStream.P = MaxP;
      //const double Ti = InStream.T;
      //InStream.SetTP(Ti, MaxP);
      p = InStream.getP();
      Log.Message(MMsg_Note, "After set Pressure %8.3f", p);

      //so....
      //probably want code below...
      //MStream InStream;
      //FlwIOs.AddMixtureIn_Id(InStream, idIn);
      //const double Pi = FlwIOs[FlwIOs.First[idIn]].Stream.getP() //get pressure of input stream (one and only connection)
      //InStream.P = Pi;

      MStream & OutStream = FlwIOs[FlwIOs.First[idOut]].Stream;
      SlipFlow s1(m_lSlipMode);
      OutStream = InStream;

      //same parameter sanity checks
      dPipe = Max(dPipe, 1.0e-6);
      dEntryK = Max(dEntryK, 1.0e-9);

      /* 
      //m_VLE.TPFlash(OutStream, OutStream.T-dFlashdT, dFlashP, VLEF_QVFlash);
      //m_VLE.SetFlashVapFrac(OutStream, dxVapor, VLEF_QVFlash);
      //dFlashP = OutStream.SaturationP(OutStream.T);
      
      CFlashSolver fn(OutStream, dFlashP, m_VLE);

      


      if (fn.FindRoot(0., 0., 0.99)==RF_OK) {
	//Log.Message(MMsg_Error, "Found Root");
	dxVapor = fn.Result();
	m_VLE.SetFlashVapFrac(OutStream, dxVapor, VLEF_QVFlash);
	dFlashdT = InStream.T - OutStream.T;
	dSlipDensity = s1.SlipDensity(OutStream);
      } else {
	Log.Message(MMsg_Error, "Couldnt solve for root");
	} */
      dPin = p;
      double deltaP = GTZ(p - dPout);
      double den = InStream.Density();
      double area = CircleArea(dPipe);
      dMassFlow =  area*sqrt(2*den*deltaP/dEntryK);
      dMassFlow1 = InStream.Mass();
      

    
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





