//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $ QAL Extensions by Transcritical Technologies Pty Ltd
//   Time-stamp: <2006-11-01 11:51:54 Rod Stephenson Transcritical Pty Ltd>
//===========================================================================

#include "stdafx.h"
#include "QALHeater.h"

#define dbgModels 1

//====================================================================================

static double LMTD(double TbTi, double TbTo, double ShTi, double ShTo)
  {
  double gttd = ShTo - TbTi;
  double lttd = ShTi - TbTo;
  return (gttd=lttd) ? gttd : (gttd-lttd)/log(GTZ(gttd)/(GTZ(lttd)));
  }

//====================================================================================

static double LMTDSaturation(double Ti, double To, double Ts)
  {
  if (Ti>Ts)
    {
    Ts=Min(Ts, Ti-0.01);
    To=Range(Ts+1.0e-6, To, Ti-1.0e-6);
    }
  else
    {
    Ts=Max(Ts, Ti+0.01);
    To=Range(Ti+1.0e-6, To, Ts-1.0e-6);
    }
  return (To-Ti)/NZ(Ln((Ts-Ti)/(Ts-To)));
  };

//====================================================================================

const long idTubeI  = 0;
const long idTubeO  = 1;
const long idShellI = 2;
const long idShellO = 3;
const long idVent = 4;


const long LUMPED = 0;
const long FULL =1;
const long SCALING=2;

const long FXD = 0;
const long KD =1;
const long DETAIL=2;

static MDDValueLst DDHxMode[]=
  {
    {0, "Area"},
    {1, "Tube Only"},
    {2, "Full"},
    {NULL}
  };

static MDDValueLst DDSSHxMode[]=
  {
    {0, "Lumped"},
    {1, "Full"},
    {NULL}
  };

static MDDValueLst DDTSHxMode[]=
  {
    {0, "Lumped"},
    {1, "Full"},
    {NULL}
  };


static MDDValueLst DDSSdPMode[]=
  {
    {0, "Fixed"},
    {1, "Kd"},
    {2, "Detail"},
    {NULL}
  };

static MDDValueLst DDTSdPMode[]=
  {
    {0, "Fixed"},
    {1, "Kd"},
    {2, "Detail"},
    {NULL}
  };







static MInOutDefStruct s_IODefs[]=
  {
  //  Desc;             Name;       PortId; Rqd; Max; CnId, FracHgt;  Options;
    { "Tube In",       "TubeI",     idTubeI,   1,  10,    0,    1.0f,  MIO_In |MIO_Material },
    { "Tube Out",      "TubeO",     idTubeO,   1,   1,    0,    1.0f,  MIO_Out|MIO_Material },
    { "Shell In",      "ShellI",    idShellI,  1,  10,    1,    1.0f,  MIO_In |MIO_Material },
    { "Shell Out",     "ShellO",    idShellO,  1,   1,    1,    1.0f,  MIO_Out|MIO_Material },
    { "Vent",          "Vent0",    idVent,    0,   1,    1,    1.0f,  MIO_Out|MIO_Material },
    { NULL },
  };

static double Drw_CQALHeater[] = 
  { 
  MDrw_Poly, -10,1, 10,1,
  MDrw_Poly, -10,-1, 10,-1,

  MDrw_Poly, -9,3, -9,1,
  MDrw_Poly, -9,-1, -9,-2, 8,-2, 8,-3,

  MDrw_Poly, 9,-3, 9,-1,
  MDrw_Poly, 9,1, 9,2, -8,2, -8,3,

  MDrw_End 
  };

//---------------------------------------------------------------------------
DEFINE_TRANSFER_UNIT(CQALHeater, "QALHeater", DLL_GroupName)

void CQALHeater_UnitDef::GetOptions()
  {
  SetDefaultTag("TR");
  SetDrawing("HeatExchange", Drw_CQALHeater);
  SetTreeDescription("QAL:Liquor Heater");
  SetDescription("TODO: Flash Train Heater Model");
  SetModelSolveMode(MSolveMode_Probal);
  SetModelGroup(MGroup_Energy);
  SetModelLicense(MLicense_HeatExchange);
  };

//---------------------------------------------------------------------------

CQALHeater::CQALHeater(MUnitDefBase *pUnitDef, TaggedObject * pNd) : 
MBaseMethod(pUnitDef, pNd),
m_VLE(this, VLEF_QPFlash, "VLE"),
m_FTC(this)
{
  //default values...
  m_dHTC        = 0.98;
  m_dHTArea       = 1;
  m_dUA         = 0;
  m_dLMTD       = 0;
  m_dDuty       = 0;
  m_dLMTDFactor = 1;
  m_lNTubes = 600;
  m_lNPasses = 6;
  m_dTubeLength = 6.0;
  m_dTubeID = 38e-3;
  m_dTubeOD = 43e-3;
  m_dHTEffArea = 1;
  }

//---------------------------------------------------------------------------

CQALHeater::~CQALHeater()
  {
  }

//---------------------------------------------------------------------------

void CQALHeater::Init()
{
  SetIODefinition(s_IODefs);
  RegisterFlashTrainNode(idShellI, true, m_FTC.TypeInfo());
}

//---------------------------------------------------------------------------

const long OM_Condensing  = 1;

void CQALHeater::BuildDataFields()
{
  DD.CheckBox("Online", "",  &m_bOnline, MF_PARAMETER|MF_SET_ON_CHANGE);    
  DD.Long("Geometry",   "", &m_lHxMode , MF_PARAMETER|MF_SET_ON_CHANGE, DDHxMode); 
//   DD.Show(m_lHxMode==0);
//   DD.Double("HTC",       "",     &m_dHTC,           MF_PARAMETER, MC_HTC("kW/m^2.K"));
//   DD.Double("Area",      "",     &m_dHTArea,          MF_PARAMETER, MC_Area("m^2"));
  DD.Show((bool) m_lHxMode);
  DD.Text("Calculation Modes");
  DD.Long("SS.Hx.Calc",   "", &m_lSSHxMode , MF_PARAMETER|MF_SET_ON_CHANGE, DDSSHxMode); 
  DD.Long("TS.Hx.Calc",   "", &m_lTSHxMode , MF_PARAMETER|MF_SET_ON_CHANGE, DDTSHxMode); 
  DD.Long("SS.dP.Calc",   "", &m_lSSdPMode , MF_PARAMETER|MF_SET_ON_CHANGE, DDSSdPMode); 
  DD.Long("TS.dP.Calc",   "", &m_lTSdPMode , MF_PARAMETER|MF_SET_ON_CHANGE, DDTSdPMode);
  DD.CheckBox("TS.Scaling", "",  &m_bTSScaling, MF_PARAMETER|MF_SET_ON_CHANGE);
  DD.CheckBox("SS.Scaling", "",  &m_bSSScaling, MF_PARAMETER|MF_SET_ON_CHANGE);
  DD.Show();
  DD.Double("HTC",       "",     &m_dHTC,     m_lHxMode ? MF_RESULT : MF_PARAMETER, MC_HTC("kW/m^2.K"));
  DD.Double("Area",      "",     &m_dHTArea,  m_lHxMode ? MF_RESULT : MF_PARAMETER, MC_Area("m^2"));
  DD.Show((bool) m_lHxMode);

  DD.Double("TS.Velocity", "",   &m_dTSVel, MF_RESULT, MC_Ldt("m/s"));
  DD.Double("Area.TubeOD", "", &m_dHTExtArea, MF_RESULT|MF_NO_FILING, MC_Area);

  

  DD.Show();
  DD.Double("U*A",       "",     &m_dUA,            MF_RESULT, MC_UA("kW/K"));
  DD.Double("LMTD",      "",     &m_dLMTD,          MF_RESULT, MC_dT("C"));
  DD.Double("Duty",      "",     &m_dDuty,          MF_RESULT, MC_Pwr("kW"));
  DD.Double("LMTDFact",  "",     &m_dLMTDFactor,    MF_RESULT, MC_Frac("%"));
  DD.Double("Eff.Area", "", &m_dHTEffArea, MF_PARAMETER, MC_Frac("%"));
  DD.Double("QmVentRqd", "", &m_dQmVent, MF_PARAMETER, MC_Qm);
  DD.Text("");
  DD.Show((bool) m_lHxMode);
  DD.Page("TubeSide");
  DD.Long("Number.Tubes", "", &m_lNTubes, MF_PARAMETER);
  DD.Double("Tube.Length", "", &m_dTubeLength, MF_PARAMETER, MC_L("m")); 
  DD.Double("Tube.ID", "", &m_dTubeID, MF_PARAMETER, MC_L("mm")); 
  DD.Double("Tube.OD", "", &m_dTubeOD, MF_PARAMETER, MC_L("mm")); 
  DD.Double("Tube.Plate.Thk", "", &m_dTubePlateThk, MF_PARAMETER, MC_L("mm")); 
  DD.Long("Tubeside.Passes", "",  &m_lNPasses, MF_PARAMETER);
  DD.Double("Tube.Conductivity", "", &m_dTubeConductivity, MF_PARAMETER, MC_HCond);
  DD.Show(m_lHxMode==2);
  DD.Double("Tran.Pitch", "", &m_dtPitch, MF_PARAMETER, MC_L("mm")); 
  DD.Double("Long.Pitch", "", &m_dlPitch, MF_PARAMETER, MC_L("mm"));
  DD.Show(m_lTSHxMode);
  DD.Double("Liquor.k", "", &m_dThermalConductivity, MF_PARAMETER, MC_HCond);
  

  DD.Show(m_lHxMode && m_bTSScaling);
  DD.Double("Scale.Thick", "", &m_dScaleThickness, MF_PARAMETER, MC_L("mm")); 
  DD.Double("Scale.Cond", "", &m_dScaleConductivity, MF_PARAMETER, MC_HCond); 
  DD.Show(m_lHxMode && m_lTSdPMode==KD);
  DD.Double("TS.Kd.Factor","", &m_dTSKdFactor, MF_PARAMETER, MC_None);
  
    
  DD.Show( m_lHxMode);
  DD.Double("TS.HT.Coeff", "", &m_dTSHTC, m_lTSHxMode?MF_RESULT:MF_PARAMETER, MC_HTC);

  DD.Text("Results");
  DD.Double("TS.G", "", &m_dG,  MF_RESULT|MF_NO_FILING, MC_None);
  DD.Text("In");
  DD.Double("TSin.Re", "", &m_dReIn,  MF_RESULT|MF_NO_FILING, MC_None);
  DD.Double("TSin.ft", "", &m_dftIn,  MF_RESULT|MF_NO_FILING, MC_None);
  DD.Double("TSin.dPdL", "", &m_dPdLIn,  MF_RESULT|MF_NO_FILING, MC_P("kPa"));
  DD.Text("Out");
  DD.Double("TSout.Re", "", &m_dReOut,  MF_RESULT|MF_NO_FILING, MC_None);
  DD.Double("TSout.ft", "", &m_dftOut,  MF_RESULT|MF_NO_FILING, MC_None);
  DD.Double("TSout.dPdL", "", &m_dPdLOut,  MF_RESULT|MF_NO_FILING, MC_P("kPa"));
  DD.Text("Total");
  DD.Double("TS.dP", "",  &m_dP,  MF_RESULT|MF_NO_FILING, MC_P("kPa"));
#ifdef TTDEBUG
  DD.Double("tmp1", "",  &m_tmp1,  MF_RESULT|MF_NO_FILING, MC_P("kPa"));
  DD.Double("tmp2", "",  &m_tmp2,  MF_RESULT|MF_NO_FILING, MC_None);
  DD.Double("tmp3", "",  &m_tmp3,  MF_RESULT|MF_NO_FILING, MC_None);
  DD.Double("tmp4", "",  &m_tmp4,  MF_RESULT|MF_NO_FILING, MC_None);
  DD.Double("tmp5", "",  &m_tmp5,  MF_RESULT|MF_NO_FILING, MC_None);
#endif
  
  DD.Page("ShellSide");
  DD.Show(m_lHxMode==2);
  DD.Text("Geometry");
  DD.Double("Shell.ID", "", &m_dShellID, MF_PARAMETER, MC_L("mm")); 
  DD.Double("Shell.OTL", "", &m_dShellOTL, MF_PARAMETER, MC_L("mm")); 
  DD.Double("Baffle.Space", "", &m_dBaffleSpacing, MF_PARAMETER, MC_L("mm"));
  DD.Double("Baffle.Cut", "", &m_dShellID, MF_PARAMETER, MC_None);
  DD.Show(m_lHxMode);
  DD.Text("Data");
  DD.Double("SS.HT.Coeff", "", &m_dSSHTC, m_lSSHxMode?MF_RESULT:MF_PARAMETER, MC_HTC);
  DD.Double("m_Sec.Qm", "", &m_dSecQm, MF_RESULT, MC_Qm);
  DD.Show(m_lHxMode && m_bSSScaling);
  DD.Double("SS.Scale.Thick", "", &m_dSScaleThickness, MF_PARAMETER, MC_L("mm")); 
  DD.Double("SS.Scale.Cond", "", &m_dSScaleConductivity, MF_PARAMETER, MC_HCond); 
  DD.Show(m_lHxMode && m_lSSdPMode==KD);
  DD.Double("SS.Kd.Factor","", &m_dSSKdFactor, MF_PARAMETER, MC_None);
  
    
  DD.Show( m_lHxMode);


  DD.Page("VLE");
  DD.Show();
  m_FTC.BuildDataFields();

}

// bool CQALHeater::ExchangeDataFields()
//   {
//   if (m_FTC.ExchangeDataFields())
//     return 1;
//   switch (DX.Handle)
//     {
//     case 1: 
//       if (DX.HasReqdValue)
//         m_lOpMode=DX.Long;
//       DX.Long=m_lOpMode;
//       return true;
//     }
//   return false;
//   }

//---------------------------------------------------------------------------

bool CQALHeater::ConfigureJoins()
  { 
  MBaseMethod::ConfigureJoins();
  //Joins.Count=2;
  return true; 
  }

//---------------------------------------------------------------------------

bool CQALHeater::EvalJoinPressures()
{
  if (1) {//set pressures at each join (pipes connected to unit)
    for (int j=0; j<Joins.Count; j++) {
      double Pj=Joins[j].GetProbalPIn();
      
      if (j==1 && m_FTC.Active) {
	m_FTC.SuctionP=Pj;
	Joins[j].SetProbalP(Pj, true, true);
      }
      if (j==0) {
	double pOut;
	if (Pj < m_dP)    // Pressure drop greater than inlet pressure
	  pOut = 101.325;   // Just set outlet to atmospheric for now...
	else
	  pOut = Pj-m_dP;
	Joins[j].SetProbalP(pOut, false, true);
      }
      
    }
    return true;
  }
  else {
    //INCOMPLETECODE() 
  }
  return false;
}



//===========================================================================
//
//  Condensate Finder
//
//===========================================================================

class CCondensateFinder : public MRootFinder
  {
  public:
    CCondensateFinder(CQALHeater * pTH, MStream &TubeI, MStream &ShellI, MStream &TubeO, MStream &ShellO);
    ~CCondensateFinder();
    double Function(double x);

  protected:
    MStream & m_TubeI;
    MStream & m_ShellI;
    MStream & m_TubeO;
    MStream & m_ShellO;

    CQALHeater &m_TR;
    MVLEBlk      &m_VLE;

    static MToleranceBlock s_Tol;
  };

MToleranceBlock CCondensateFinder::s_Tol(TBF_Both, "CQALHeater:CondensFinder", 0.0, 1.0e-12);

//---------------------------------------------------------------------------

CCondensateFinder::CCondensateFinder(CQALHeater * pTH, MStream &TubeI, MStream &ShellI, MStream &TubeO, MStream &ShellO) : 
MRootFinder("A Desc" ,s_Tol),
m_TR(*pTH), 
m_TubeI(TubeI),
m_ShellI(ShellI),
m_TubeO(TubeO),
m_ShellO(ShellO),
m_VLE(pTH->m_VLE)
  {
  m_TubeO.SuppressErrors=false;
  m_ShellO.SuppressErrors=false;
  };

//---------------------------------------------------------------------------

CCondensateFinder::~CCondensateFinder()
  {
  m_TubeO.SuppressErrors=false;
  m_ShellO.SuppressErrors=false;
  };

//---------------------------------------------------------------------------

double CCondensateFinder::Function(double Amount)
  {
  m_ShellO.SetM(m_ShellI, MP_All, Amount, m_ShellI.P);
  double ShtotHf=m_ShellO.totHf();

  //double FTemp=Max(m_TubeO.T, m_ShellI.SaturationT());
  double FTemp=m_ShellI.SaturationT();
  double FPress=m_ShellI.SaturationP();
  m_VLE.SetSatPVapFrac(m_ShellO, FTemp, FPress, 0.0, VLEF_Null);
  m_ShellO.SetTP(FTemp, FPress);//m_ShellO.m_pCd->Temp();
  m_TR.m_dDuty=-(m_ShellO.totHf()-ShtotHf);

  m_TubeO.Set_totHf(m_TubeI.totHf()+/*m_TubeO.Sgn**/m_TR.m_dDuty/*+m_TubeO.dHEnv+m_ShellO.dHEnv*/);

  double TDuty;
  if (m_TR.m_dUA>1.0e-6) {
      m_TR.m_dLMTD = LMTDSaturation(m_TubeI.T, m_TubeO.T, m_ShellO.T);
      TDuty = m_TR.m_dUA * m_TR.m_dLMTD * m_TR.m_dLMTDFactor;
  }
  else {
    m_TR.m_dLMTD=0.0;
    TDuty = 0.0;
  }

  double Diff=m_TR.m_dDuty-TDuty;
  return Diff;
  };

//---------------------------------------------------------------------------

void CQALHeater::DoCondensingHeater(MStream & ShellI, MStream & TubeI, 
				    MStream & ShellO, MStream & TubeO)
{

  if (TubeI.MassFlow()>0 && ShellI.MassFlow()>0) {
    double TTi=TubeI.T;
    double STi=ShellI.T;

    m_dUA=m_dHTArea*m_dHTC*m_dHTEffArea;


    m_FTC.VapourFlow=ShellI.Mass();

    if (m_FTC.Active)
      {
	double R=m_FTC.VapourFlowReqd;//+LastVent.Mass;
	double A=0;
	for (int i=0; i<FlwIOs.Count; i++)
	  {
	    MFlow F=FlwIOs[i];
	    if (F.Id==idShellI && F.IsFlwIn && F.IsMakeUpAvail)
	      A+=F.MakeUpAvail;
	  }
	A=GTZ(A);
	// Distribute according to MakeUpAvail
	for (int i=0; i<FlwIOs.Count; i++)
	  {
	    MFlow F=FlwIOs[i];
	    if (F.Id==idShellI && F.IsFlwIn && F.IsMakeUpAvail)
	      F.MakeUpReqd=R*Max(1.0e-9, F.MakeUpAvail/A);
	  }
      }

    //operating as part of a flash train macro model, must calculate steam demand...
    double MaxVapFlow=100;
    
    m_FTC.VapourFlow = ShellI.MassFlow();
    
    CCondensateFinder Fn(this, TubeI, ShellI, TubeO, ShellO);
    // Now Solve
    switch (Fn.FindRoot(0, 0, MaxVapFlow))//MnTbOutT, MxTbOutT))
      {
      case RF_OK:      m_FTC.VapourFlowReqd = Fn.Result();       break;
      case RF_LoLimit: m_FTC.VapourFlowReqd = MaxVapFlow*1e-6;   break;
      case RF_HiLimit: m_FTC.VapourFlowReqd = MaxVapFlow;        break;
      default: 
	Log.Message(MMsg_Error, "Condesate Flw not found - RootFinder:%s", 
		    Fn.ResultString(Fn.Error())); 
	break;
      }
    
    
  }
};


//---------------------------------------------------------------------------

void CQALHeater::EvalProducts()
{
  try           
    {
    MStreamI TubeI;
    MStreamI ShellI;
    MStream & TubeO = FlwIOs[FlwIOs.First[idTubeO]].Stream;
    MStream & ShellO = FlwIOs[FlwIOs.First[idShellO]].Stream;

    FlwIOs.AddMixtureIn_Id(TubeI, idTubeI);
    FlwIOs.AddMixtureIn_Id(ShellI, idShellI);

    TubeO = TubeI;
    ShellO = ShellI;

    if (m_lHxMode) {// Need to calculate overall HTC from various contributions
      doGeometry();

      double drat = m_dTubeOD/m_dTubeID;
      double wallResistance = log(drat)*m_dTubeID/(2*m_dTubeConductivity);
      double scaleResistance = 0.0;
      double sscaleResistance = 0.0;
      if (m_bTSScaling && m_dScaleThickness > 0.0) {
	double id = m_dTubeID-2*m_dScaleThickness;  // actual ID with scale
	scaleResistance = log(m_dTubeID/id)*id/(2*m_dScaleConductivity);
      }
      if (m_bSSScaling && m_dSScaleThickness > 0.0) {
	double od = m_dTubeOD+2*m_dSScaleThickness;  // actual ID with scale
	sscaleResistance = log(od/m_dTubeOD)*m_dTubeOD/(2*m_dSScaleConductivity);
      }

      m_dHTC = 1/(1/m_dTSHTC + wallResistance+scaleResistance + sscaleResistance 
		  + 1/m_dSSHTC/drat);
      if (m_bTSScaling && m_dScaleThickness > 0.0)
	m_dHTC *= (1.-2*m_dScaleThickness/m_dTubeID);
    }
    
    if (m_bOnline)
      DoCondensingHeater(ShellI, TubeI, ShellO, TubeO);
    
    if (m_lHxMode && m_lTSdPMode) // Do we calculate dP from first principals? 
      flowRE(TubeI, TubeO);
    

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

void CQALHeater::ClosureInfo(MClosureInfo & CI)
{
  if (CI.DoFlows())
    {
    //  HtIn += dActualDuty; //ensure heat balance
    }
}

//====================================================================================
