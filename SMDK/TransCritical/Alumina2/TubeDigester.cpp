//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
//   Time-stamp: <2006-11-13 11:09:09 Rod Stephenson Transcritical Pty Ltd>
// Copyright (C) 2005 by Transcritical Technologies Pty Ltd and KWA
//   CAR Specific extensions by Transcritical Technologies Pty Ltd
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "TubeDigester.h"


#define dbgModels 1

//====================================================================================
static MInitialiseTest InitTest("Digest");
static MSpeciePtr  spWater         (InitTest, "H2O(l)", false);
static MSpeciePtr  spWaterVapor    (InitTest, "H2O(g)", false);


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

static double LMTD(double TbTi, double TbTo, double ShTi, double ShTo)
{
  double gttd = ShTo - TbTi;
  double lttd = ShTi - TbTo;
  if (gttd==0 || lttd==0) return 0.0;
  const double r = gttd/lttd;
  if (r <= 0) return 0.0;
  return   (gttd==lttd) ? gttd : (gttd-lttd)/log(r);
}






//====================================================================================

const long idTubeI  = 0;
const long idTubeO  = 1;
const long idShellI = 2;
const long idShellO = 3;
const long idVentO  = 4;



enum AmbientLossMethod {ALM_None, ALM_LossPerQm, ALM_ProductTemp, ALM_TempDrop, ALM_LossToAmbient, ALM_LossToAmbient2,
   ALM_FixedHeatFlow};


static MDDValueLst DDALMethod[]=
  {
    {ALM_None, "None"},
    {ALM_LossPerQm, "LossPerQm"},
    {ALM_ProductTemp, "ProductTemp"},
    {ALM_TempDrop, "TempDrop"},
    {ALM_LossToAmbient, "LossToAmbient"},
    {ALM_LossToAmbient2, "LossToAmbient2"},
    {ALM_FixedHeatFlow, "FixedHeatFlow"},
    {NULL}
  };


static MDDValueLst DDHxMode[]=
  {
    {0, "Overall"},
    {1, "TubeDetail"},
    {NULL}
  };

static MDDValueLst DDSSHxMode[]=
  {
    {0, "Lumped"},
    {1, "Detail"},
    {NULL}
  };

static MDDValueLst DDTSHxMode[]=
  {
    {0, "Lumped"},
    {1, "Detail"},
    {NULL}
  };


static MDDValueLst DDSSdPMode[]=
  {
    {0, "Fixed"},
    //    {1, "Kd"},
    {2, "Full"},
    {NULL}
  };

static MDDValueLst DDTSdPMode[]=
  {
    {0, "Fixed"},
    // {1, "Kd"},
    {2, "Full"},
    {NULL}
  };


enum {OM_Simple, OM_Condensing, OM_LiveSteam
}
;


static MDDValueLst DDOpMode[]=
  {
    {OM_Simple,      "Simple"},
    {OM_Condensing,  "Condensing"},
    {OM_LiveSteam,  "Live Steam"},
    {0}
  };







static MInOutDefStruct s_IODefs[]=
  {
  //  Desc;        Name;    PortId; Rqd; Max; CnId, FracHgt;  Options;
    { "Tube In",  "TubeI",  idTubeI,   1,  10,    0,    1.0f,  MIO_In |MIO_Material },
    { "Tube Out", "TubeO",  idTubeO,   1,   1,    0,    1.0f,  MIO_Out|MIO_Material },
    { "Shell In", "ShellI", idShellI,  1,  10,    1,    1.0f,  MIO_In |MIO_Material },
    { "Shell Out","ShellO", idShellO,  1,   1,    1,    1.0f,  MIO_Out|MIO_Material },
    { "Vent",     "VentO",  idVentO,   0,   1,    1,    1.0f,  MIO_Out|MIO_Material },
    { NULL },
  };

double Drw_CCARTubeDigester[] = 
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
DEFINE_TRANSFER_UNIT_EX(CCARTubeDigester, "Alumina2*TubeDigester", DLL_GroupName)

void CCARTubeDigester_UnitDef::GetOptions()
  {
  SetDefaultTag("TR");
  SetDrawing("HeatExchange", Drw_CCARTubeDigester);
  SetTreeDescription("Heat Transfer:Shell & Tube(2)");
  SetDescription("TODO: Flash Train Digester");
  SetModelSolveMode(MSolveMode_Probal);
  SetModelGroup(MGroup_Energy);
  //SetModelLicense(MLicense_HeatExchange);
  };

//---------------------------------------------------------------------------

CCARTubeDigester::CCARTubeDigester(MUnitDefBase *pUnitDef, TaggedObject * pNd) : 
MBaseMethod(pUnitDef, pNd),
m_RB(this, false, "RB"),
m_VLE(this, VLEF_QPFlash, "VLE"),
m_FTC(this)
{
  //default values...
  bOnline = true;
  m_dHTC        = 0.98;
  m_dHTArea       = 100.0;
  m_dUA         = 0;
  m_dLMTD       = 0;
  m_dDuty       = 0;
  m_dLMTDFactor = 1;
  m_lNTubes = 600;
  m_lNPasses = 6;
  m_dTubeLength = 6.0;
  m_dTubeID = 38e-3;
  m_dTubeOD = 43e-3;
  m_dEntranceC = 1.0;
  m_dExitC = 0.34;
  m_bMTDCalcMode = false;

  m_bEnvironHX = false;
  m_lEnvHxMode = 0;
  m_dEnvHTC = .2;
  m_dLossPerQm = 10.;
  m_dEnvHeatLoss = 100;
  
  m_lHxMode = 0;
  m_lSSHxMode = 0;  
  m_lTSHxMode =0;
  m_lSSdPMode =0;
  m_lTSdPMode =0;
  m_bTSScaling = false;
  m_bSSScaling = false;
  m_dSScaleThickness = 1.0e-3;
  m_dScaleThickness = 1.0e-3;
  m_dScaleConductivity=.2;
  
  m_dTubeConductivity = 20;
  


  m_dSSHTC = 1.0;
  m_dTSHTC = 1.0;
  m_dQmVentRqd = 0.0;
  m_dP = 0.0;


  m_dReactionDamping = 0.0;
  m_dDeltaTOld = 0.0;
  
  
}


//---------------------------------------------------------------------------

CCARTubeDigester::~CCARTubeDigester()
  {
  }

//---------------------------------------------------------------------------

void CCARTubeDigester::Init()
{
  SetIODefinition(s_IODefs);
  RegisterFlashTrainNode(idShellI, true, m_FTC.TypeInfo());
}

//---------------------------------------------------------------------------


void CCARTubeDigester::BuildDataFields()
{

#ifdef TTDEBUG
  DD.CheckBox("TTDBG", "",  &bTTDebug, MF_PARAMETER);
#endif
  DD.CheckBox("On", "",  &bOnline, MF_PARAMETER|MF_SET_ON_CHANGE);
  DD.Long  ("OpMode",    "",     &m_lOpMode,   MF_PARAMETER|MF_SET_ON_CHANGE, DDOpMode);

  m_RB.OnOffCheckBox();
  DD.Show(m_RB.Enabled());
  DD.CheckBox("Use.RMTD", "", &m_bMTDCalcMode,  MF_PARAMETER|MF_SET_ON_CHANGE);
  //  DD.Double("Reaction.Damping", "",   &m_dReactionDamping, MF_PARAMETER, MC_Frac("%"));
  DD.Double("Reaction.Ht",  "",     &dHf, MF_RESULT , MC_Pwr("kW"));


  DD.Show();
  DD.CheckBox("EnvironHX", "", &m_bEnvironHX,  MF_PARAMETER|MF_SET_ON_CHANGE);

  DD.Show(m_bEnvironHX);
  DD.Long("Model",   "", &m_lEnvHxMode , MF_PARAMETER|MF_SET_ON_CHANGE, DDALMethod);
  DD.Show(m_lEnvHxMode==ALM_LossPerQm);
  DD.Double("LossPerQm", "", &m_dLossPerQm, MF_PARAMETER,  MC_HMs);
  DD.Show(m_lEnvHxMode==ALM_LossToAmbient);
  DD.Double("Loss.To.Ambient", "", &m_dEnvHTC, MF_PARAMETER,  MC_UA);
  
  DD.Show(m_bEnvironHX);
  DD.Double("Env.Heat.Loss",  "",     &m_dEnvHeatLoss, 
	    m_lEnvHxMode==6 ?  MF_PARAMETER:MF_RESULT , MC_Pwr("kW"));
  
  DD.Show();
  DD.Long("Layout",   "", &m_lHxMode , MF_PARAMETER|MF_SET_ON_CHANGE, DDHxMode); 
  DD.Double("HTC",       "", &m_dHTC, m_lHxMode ? MF_RESULT : MF_PARAMETER, MC_HTC("kW/m^2.K"));
  DD.Double("Area",      "", &m_dHTArea, m_lHxMode ? MF_RESULT : MF_PARAMETER, MC_Area("m^2"));
  DD.Show(m_lHxMode!=0);

  DD.Text("Calculation Modes");
  DD.Long("SSHxCalc",   "", &m_lSSHxMode , MF_PARAMETER|MF_SET_ON_CHANGE, DDSSHxMode); 
  DD.Long("TSHxCalc",   "", &m_lTSHxMode , MF_PARAMETER|MF_SET_ON_CHANGE, DDTSHxMode); 
  DD.Long("SSdPCalc",   "", &m_lSSdPMode , MF_PARAMETER|MF_SET_ON_CHANGE, DDSSdPMode); 
  DD.Long("TSdPCalc",   "", &m_lTSdPMode , MF_PARAMETER|MF_SET_ON_CHANGE, DDTSdPMode); 
  DD.CheckBox("TS.Scaling", "",  &m_bTSScaling, MF_PARAMETER|MF_SET_ON_CHANGE);
  DD.CheckBox("SS.Scaling", "",  &m_bSSScaling, MF_PARAMETER|MF_SET_ON_CHANGE);

  DD.Show(m_lHxMode!=0);

  DD.Show();
  DD.Double("U*A",       "",     &m_dUA,            MF_RESULT, MC_UA("kW/K"));
  DD.Double("LMTD",      "",     &m_dLMTD,          MF_RESULT, MC_dT("C"));
  DD.Double("RMTD",      "",     &m_dRMTD,          MF_RESULT, MC_dT("C"));

  DD.Double("Duty",      "",     &m_dDuty,          MF_RESULT, MC_Pwr("kW"));
  DD.Show(m_lOpMode!=0);
  DD.Double("Tot.Cond.Duty", "", &m_dTotDuty,  MF_RESULT, MC_Pwr("kW"));
  DD.Show();
  DD.Double("LMTDFact",  "",     &m_dLMTDFactor,    MF_RESULT, MC_Frac("%"));
  DD.Show(m_lOpMode!=0);
  DD.Double("QmVentRqd", "",     &m_dQmVentRqd,     MF_PARAMETER, MC_Qm("kg/s"));
  DD.Show();
  DD.Text("");
  DD.Double("Tube.In.T",      "",     &m_dTSTin,          MF_RESULT, MC_T("C"));
  DD.Double("Tube.Out.T",      "",     &m_dTSTout,          MF_RESULT, MC_T("C"));
  DD.Double("Shell.In.T",      "",     &m_dSSTin,          MF_RESULT, MC_T("C"));
  DD.Double("Shell.Out.T",      "",     &m_dSSTout,          MF_RESULT, MC_T("C"));

  DD.Text("");
  DD.Double("Tube.In.P",      "",     &m_dTSPin,          MF_RESULT, MC_P("kPa"));
  DD.Double("Tube.Out.P",      "",     &m_dTSPout,          MF_RESULT, MC_P("kPa"));
  DD.Double("Shell.In.P",      "",     &m_dSSPin,          MF_RESULT, MC_P("kPa"));
  DD.Double("Shell.Out.P",      "",     &m_dSSPout,          MF_RESULT, MC_P("kPa"));

  DD.Double("Tube.Qm", "",     &m_dQmTS,     MF_RESULT, MC_Qm("kg/s"));
  DD.Double("Shell.Qm", "",    &m_dQmSS,     MF_RESULT, MC_Qm("kg/s"));



  m_RB.BuildDataFields();

#ifdef TTDEBUG
  DD.Double("tmp1", "",  &m_tmp1,  MF_RESULT|MF_NO_FILING, MC_None);
  DD.Double("tmp2", "",  &m_tmp2,  MF_RESULT|MF_NO_FILING, MC_None);
  DD.Double("tmp3", "",  &m_tmp3,  MF_RESULT|MF_NO_FILING, MC_None);
  DD.Double("tmp4", "",  &m_tmp4,  MF_RESULT|MF_NO_FILING, MC_None);
  DD.Double("tmp5", "",  &m_tmp5,  MF_RESULT|MF_NO_FILING, MC_None);
  DD.Double("tmp6", "",  &m_tmp6,  MF_RESULT|MF_NO_FILING, MC_None);
  DD.Double("tmp7", "",  &m_tmp7,  MF_RESULT|MF_NO_FILING, MC_None);
  DD.Double("tmp8", "",  &m_tmp8,  MF_RESULT|MF_NO_FILING, MC_None);
#endif

  
  DD.Show(m_lHxMode!=0);

  DD.Page("ShellSide");         // For detailed models, all shellside stuff here
  DD.Text("Mode: Condensing");
  DD.Double("ShellSideHTC", "", &m_dSSHTC, MF_PARAMETER,  MC_HTC("kW/m^2.K"));
  DD.Show(m_lHxMode && m_bSSScaling);
  DD.Double("SS.Scale.Thick", "", &m_dSScaleThickness, MF_PARAMETER, MC_L("mm")); 
  DD.Double("SS.Scale.Cond", "", &m_dScaleConductivity, MF_PARAMETER, MC_HCond); 
  DD.Show(m_lHxMode!=0);
  DD.Text("Results");
  DD.Double("Qm", "", &m_dCondFlow, MF_RESULT|MF_NO_FILING, MC_Qm);
  DD.Double("Sat.T", "", &m_dSatT, MF_RESULT|MF_NO_FILING, MC_T);
  DD.Double("Cond.T", "", &m_dSST, MF_RESULT|MF_NO_FILING, MC_T);
  DD.Double("SatP", "", &m_dSatP, MF_RESULT|MF_NO_FILING, MC_P);


  DD.Page("TubeSide");    // All the tubeside stuff here
  DD.Text("Mode: Sensible + Reaction");
  DD.Text("Data");
  DD.Double("Tube.Conductivity", "", &m_dTubeConductivity, MF_PARAMETER, MC_HCond);
  DD.Long("NumberTubes", "", &m_lNTubes, MF_PARAMETER);
  DD.Double("TubeLength", "", &m_dTubeLength, MF_PARAMETER, MC_L("m")); 
  DD.Double("TubeID", "", &m_dTubeID, MF_PARAMETER, MC_L("mm")); 
  DD.Double("TubeOD", "", &m_dTubeOD, MF_PARAMETER, MC_L("mm")); 
  DD.Long("TubeSidePasses", "",  &m_lNPasses, MF_PARAMETER);
  DD.Double("EntranceCc", "", &m_dEntranceC, MF_PARAMETER | MF_INIT_HIDDEN, MC_None);
  DD.Double("ExitCc", "", &m_dExitC, MF_PARAMETER | MF_INIT_HIDDEN, MC_None);
  
  DD.Text("HeatTransfer");
  DD.Double("TubeSideHTC", "", &m_dTSHTC, MF_PARAMETER,  MC_HTC("kW/m^2.K"));

  DD.Show(m_lHxMode && m_bTSScaling);  // Scaling active
  DD.Double("Scale.Thick", "", &m_dScaleThickness, MF_PARAMETER, MC_L("mm")); 
  DD.Double("Scale.Cond", "", &m_dScaleConductivity, MF_PARAMETER, MC_HCond);
  DD.Show(m_lHxMode!=0);
 
  DD.Text("Results");
  DD.Double("TS.Vel", "", &m_dTSVel, MF_RESULT, MC_Ldt("m/s"));
  DD.Double("cp", "", &m_dTSCp, MF_RESULT|MF_NO_FILING, MC_CpMs);

  DD.Double("TS.G", "", &m_dG,  MF_RESULT|MF_NO_FILING, MC_None);
  DD.Double("TS.Rht", "", &m_dRHt,  MF_RESULT|MF_NO_FILING, MC_HMs("kJ/kg"));
  DD.Text("In");
  DD.Double("Tin",      "",     &m_dTSTin,          MF_RESULT, MC_T("C"));
  DD.Double("TSin.Re", "", &m_dReIn,  MF_RESULT|MF_NO_FILING | MF_INIT_HIDDEN, MC_None);
  DD.Double("TSin.ft", "", &m_dftIn,  MF_RESULT|MF_NO_FILING | MF_INIT_HIDDEN, MC_None);
  DD.Double("TSin.dPdL", "", &m_dPdLIn,  MF_RESULT|MF_NO_FILING | MF_INIT_HIDDEN, MC_P("kPa"));
  DD.Text("Out");
  DD.Double("Tout",      "",     &m_dTSTout,          MF_RESULT, MC_T("C"));
  DD.Double("TSout.Re", "", &m_dReOut,  MF_RESULT|MF_NO_FILING | MF_INIT_HIDDEN, MC_None);
  DD.Double("TSout.ft", "", &m_dftOut,  MF_RESULT|MF_NO_FILING | MF_INIT_HIDDEN, MC_None);
  DD.Double("TSout.dPdL", "", &m_dPdLOut,  MF_RESULT|MF_NO_FILING | MF_INIT_HIDDEN, MC_P("kPa"));
  DD.Text("Total");
  DD.Double("TS.dPIO", "",  &m_dPEntrance,  MF_RESULT|MF_NO_FILING, MC_P("kPa"));
  DD.Double("TS.dP", "",  &m_dP,  m_lTSdPMode ? MF_RESULT|MF_NO_FILING : MF_PARAMETER, MC_P("kPa"));


  DD.Show(m_lOpMode!=0);
  DD.Page("VLE");
  DD.Show();
  m_FTC.BuildDataFields();

}

// bool CCARTubeDigester::ExchangeDataFields()
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

bool CCARTubeDigester::ConfigureJoins()
  { 
  MBaseMethod::ConfigureJoins();
  //Joins.Count=2;
  return true; 
  }

//---------------------------------------------------------------------------

bool CCARTubeDigester::EvalJoinPressures()
  {
  if (1)
    {//set pressures at each join (pipes connected to unit)
    for (int j=0; j<Joins.Count; j++)
      {
      double Pj=Joins[j].GetProbalPIn();

      if (j==1 && m_FTC.Active) {
        m_FTC.SuctionP=Pj;
	
	Joins[j].SetProbalP(Pj, true, true);
      } else {
	Joins[j].SetProbalP(Pj, false, true);
	
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
  else
    {
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
    CCondensateFinder(CCARTubeDigester * pTH, MStream &TubeI, MStream &ShellI, MStream &TubeO, MStream &ShellO);
    ~CCondensateFinder();
    double Function(double x);

  protected:
    MStream & m_TubeI;
    MStream & m_ShellI;
    MStream & m_TubeO;
    MStream & m_ShellO;

    CCARTubeDigester &m_TD;
    MVLEBlk      &m_VLE;

    static MToleranceBlock s_Tol;
  };






MToleranceBlock CCondensateFinder::s_Tol(TBF_Both, "ShellTube2:CondensFinder", 0.0, 1.0e-12);
//---------------------------------------------------------------------------

CCondensateFinder::CCondensateFinder(CCARTubeDigester * pTH, MStream &TubeI, MStream &ShellI, MStream &TubeO, MStream &ShellO) : 
MRootFinder("A Desc" ,s_Tol),
m_TD(*pTH), 
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
  double ShtotHz=m_ShellO.totHz();

  //double FTemp=Max(m_TubeO.T, m_ShellI.SaturationT());
  double FTemp=m_ShellI.SaturationT();
  double FPress=m_ShellI.SaturationP();

  m_VLE.SetFlashVapFrac(m_ShellO, FTemp, FPress, 0.0, VLEF_Null); // Set the shell out stream to all liq
  
  // CNM This line creates a problem if the Steam is superheated ???
  // JSM/RAS to check??
  //m_ShellO.SetTP(FTemp, FPress);                                  // at the flash temperature and pressure
  m_ShellO.SetTP(FTemp, m_ShellI.P);                                  // at the flash temperature and pressure
  
  
  m_TD.m_dTotDuty = ShtotHz-m_ShellO.totHz();         // This is the total duty incl env loss for Amount
  m_TD.m_dDuty = m_TD.m_dTotDuty;                     // This is Hx duty: tot - env

  if (m_TD.m_bEnvironHX && m_TD.m_lEnvHxMode) m_TD.m_dDuty -= m_TD.m_dEnvHeatLoss;
  m_TubeO.Set_totHz(m_TubeI.totHz()+/*m_TubeO.Sgn**/m_TD.m_dDuty-m_TD.dHf);



  double EMTD;  // Effective Mean Temperature Difference
  double TDuty;  // Heat transferred to Tubeside
  bool bRB = m_TD.m_RB.Enabled();
  if (m_TD.m_dUA>1.0e-6) {
    m_TD.m_dLMTD = LMTDSaturation(m_TubeI.T, m_TubeO.T, FTemp);

    if (m_TD.m_bMTDCalcMode && bRB) {  
      try {
	
	// double dHf=m_TD.m_RB.HfSumRct();    // Heat of Reaction
	
	double tsmf = m_TubeI.MassFlow();
	// double q = m_TubeO.msHz()-m_TubeI.msHz();  //
	m_TD.m_dTSCp = (m_TubeI.msCp() + m_TubeO.msCp())/2;
	double avgcp = m_TD.m_dTSCp;
	double lambda = -m_TD.m_dUA/(tsmf*avgcp);
	double R = m_TD.dHf/tsmf;
	m_TD.m_dRHt = R;
	double Rcp = R/avgcp;
	
	double dt1 = FTemp - m_TubeI.T;
	double dt2 = FTemp - m_TubeO.T;
	
	if (dt1==dt2) 
	  m_TD.m_dRMTD = dt1;
	else
	  m_TD.m_dRMTD = (dt2 - dt1 - Rcp)/Ln((dt2 + Rcp/lambda)/(dt1 + Rcp/lambda));

#ifdef TTDEBUG
	m_TD.m_tmp1 = m_TD.m_dRMTD;
	m_TD.m_tmp3 = dt2;
	m_TD.m_tmp4 = dt1;
	m_TD.m_tmp2 = lambda;
	m_TD.m_tmp5 = Rcp;
	m_TD.m_tmp5 = R;

	
#endif
	
 	EMTD = m_TD.m_dRMTD;
 	if (fabs(EMTD - m_TD.m_dLMTD)>25.)
 	  EMTD = m_TD.m_dLMTD;
      }
      catch (MFPPException &e) {
	e.ClearFPP();
	m_TD.Log.Message(MMsg_Error, "FP error in RMTD");
	EMTD = m_TD.m_dLMTD;
      }
    }
    

    else {
      EMTD = m_TD.m_dLMTD;
    }
  
    
    
    TDuty = m_TD.m_dUA * EMTD * m_TD.m_dLMTDFactor;
    
  }
  else {
    m_TD.m_dLMTD=0.0;
    TDuty = 0.0;
  }

  double Diff=m_TD.m_dDuty-TDuty;
  return Diff;
  };

//===================//
// Condensing Heater //
//===================//



void CCARTubeDigester::DoCondensingHeater(MStream & ShellI, MStream & TubeI, 
				    MStream & ShellO, MStream & TubeO)
{

  if (TubeI.MassFlow()>0 && ShellI.MassFlow()>0) {


    double TTi=TubeI.T;
    double STi=ShellI.T;

    m_dUA=m_dHTArea*m_dHTC;
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

    // m_RB.EvalProducts(TubeO);    // Now moved to here... so that reaction heat is calculated right
    
    CCondensateFinder Fn(this, TubeI, ShellI, TubeO, ShellO);
    // Now Solve
    switch (Fn.FindRoot(0, 0, MaxVapFlow))//MnTbOutT, MxTbOutT))
      {
      case RF_OK:      m_FTC.VapourFlowReqd = Fn.Result()+m_dQmVentRqd;
	m_dCondFlow = m_FTC.VapourFlowReqd - m_dQmVentRqd;
	break;
      case RF_LoLimit: m_FTC.VapourFlowReqd = MaxVapFlow*1e-6;   break;
      case RF_HiLimit: m_FTC.VapourFlowReqd = MaxVapFlow;        break;
      default: 
	Log.Message(MMsg_Error, "Condensate Flw not found - RootFinder:%s", 
 		    Fn.ResultString(Fn.Error())); 
	break;
      }

    const double SatP = TubeI.SaturationP(TubeI.T);

    m_FTC.MinSaturationP = SatP; //set the minimum allowable pressure for the steam source flash
    
    m_RB.EvalProducts(TubeO);   // <--------------- Previously called *AFTER* the hx calcs
    TubeO.Set_totHz(TubeI.totHz()+m_dDuty-dHf);

    dHf=m_RB.HfSumRct();
    
    //todo add error message for steam demand not met. example: 
    //if (fabs(m_FTC.VapourFlowReqd-m_FTC.VapourFlow)>1.0e-4)
  	//	Log.SetCondition(2,MMsg_Warning,"Flash train steam demand not met");
    
  }
};


//---------------------------------------------------------------------------



class CSimpleSolverFn : public MRootFinder
  {
  public:
    CSimpleSolverFn(CCARTubeDigester * pTH, MStream &TubeI, MStream &ShellI, MStream &TubeO, MStream &ShellO) : 
      MRootFinder("A Desc" ,s_Tol),
      m_TR(*pTH), m_TubeI(TubeI), m_ShellI(ShellI), m_TubeO(TubeO), m_ShellO(ShellO) 
      {
      }
    double Function(double TubeTOut) 
      {           

      double q = m_TubeI.totHz(MP_All, TubeTOut)-m_TubeI.totHz(MP_All, m_TubeI.T);
      m_TubeO.Set_totHz(m_TubeI.totHz()+q);
      m_ShellO.Set_totHz(m_ShellI.totHz()-q);
      double lmtd=LMTD(m_TubeI.T, TubeTOut, m_ShellI.T , m_ShellO.T);
      double qp = m_TR.m_dUA*lmtd;

      m_TR.m_dDuty=qp;
      m_TR.m_dLMTD=lmtd;

      return q-qp;
      };

  public:
    MStream &m_TubeI;
    MStream &m_ShellI;
    MStream &m_TubeO;
    MStream &m_ShellO;
    //  double Duty;

    CCARTubeDigester &m_TR;
    static MToleranceBlock s_Tol;
  };

MToleranceBlock CSimpleSolverFn::s_Tol(TBF_Both, "ShellTube2:Simple", 0.00005, 0, 100);

void CCARTubeDigester::DoSimpleHeater(MStream & ShellI, MStream & TubeI, MStream & ShellO, MStream & TubeO) {
  TubeO = TubeI;
  ShellO = ShellI;

  if (TubeI.MassFlow()>0 && ShellI.MassFlow()>0) {
    double TTi=TubeI.T;
    double STi=ShellI.T;
    m_dUA=m_dHTArea*m_dHTC;

    CSimpleSolverFn Fn(this, TubeI, ShellI, TubeO, ShellO);

    double TubeOutT;
    double MnTbOutT=TubeI.T;// No Transfer
    double MxTbOutT=ShellI.T-0.001;

    double qShell=ShellI.totHz()-ShellO.totHz(MP_All, TubeI.T, ShellI.P);
    double qTube=TubeI.totHz(MP_All, ShellI.T, TubeI.P)-TubeI.totHz();
        
    if (fabs(qShell)<fabs(qTube)) { // Limited By Shell - Tube TOut Limited
      //MStream  ms(TubeI);
      //MStream ms = TubeI;
      MStream ms;
      ms = TubeI;
      ms.Set_totHz(TubeI.totHz()+qShell);
      MxTbOutT = ms.T;
      Log.Message(MMsg_Note, "Limiting TubeOutT %g TubeI %g %d %d", MxTbOutT-273.15, TubeI.T-273.15, &TubeI, &ms); 
      } 

//     if (MnTbOutT > MxTbOutT) {
//       const double tmp = MnTbOutT;
//       MnTbOutT = MxTbOutT;
//       MxTbOutT = tmp;
//     }

    
    switch (Fn.FindRoot(0, MnTbOutT, MxTbOutT)) {
    case RF_OK:       TubeOutT = Fn.Result();   break;
    case RF_LoLimit:  {
      TubeOutT = MnTbOutT;      
      Log.Message(MMsg_Error, "Low Limit TubeOutT"); 
      break;
    } 
    case RF_HiLimit:  {
      TubeO.T = MxTbOutT;
      Log.Message(MMsg_Error, "Hi Limit TubeOutT"); 
      break;
    }
    default: 
      Log.Message(MMsg_Error, "TubeOutT not found - RootFinder:%s", Fn.ResultString(Fn.Error())); 
      TubeOutT=Fn.Result();
      break;

      m_RB.EvalProducts(TubeO);    
      dHf=m_RB.HfSumRct();
      m_dRMTD = m_dLMTD;
    }
  }
};




// Live steam heater has a fixed mass flow input that is generally controlled externally. So the duty is 
// actually fixed 
void CCARTubeDigester::DoLiveSteamHeater(MStream & ShellI, MStream & TubeI, MStream & ShellO, MStream & TubeO, MStream & VentO)
{
  TubeO = TubeI;
  ShellO = ShellI;
  m_dUA=m_dHTArea*m_dHTC;
  double FTemp=ShellI.SaturationT();
  if (TubeI.MassFlow()>0 && ShellI.MassFlow()>0 && ShellI.T > TubeI.T) {
    ShellO.SetM(ShellI, MP_All, 0);
    double &Vapor = ShellO.MassVector[spWaterVapor];
    double &Cond = ShellO.MassVector[spWater];
    Vapor = 0.0;
    Cond = ShellI.MassFlow();
    if (m_dQmVentRqd < ShellI.MassFlow())  { // Assumes all steam, should check for water vapor
	Cond -= m_dQmVentRqd;
	VentO.SetM(ShellI, MP_Gas, m_dQmVentRqd);
    } else 
      Log.Message(MMsg_Error, "Excess Vent Flow called for");
		  
    ShellO.T = FTemp;
    double duty = ShellI.totHz()-ShellO.totHz() - VentO.totHz();
    m_dTotDuty = duty;         // This is the total duty incl env loss for Amount
    if (m_bEnvironHX && m_lEnvHxMode) duty -= m_dEnvHeatLoss;
    m_dDuty = duty;           // This is Hx duty: tot - env


    m_RB.EvalProducts(TubeO);    
    dHf=m_RB.HfSumRct();
    TubeO.Set_totHz(TubeO.totHz()+duty);
    if (TubeO.T > ShellI.T) TubeO.T = ShellI.T;
    m_dRMTD = m_dLMTD = LMTD(TubeI.T, TubeO.T, ShellI.T, ShellO.T);
    
  }
}






//---------------------------------------------------------------------------

void CCARTubeDigester::EvalProducts()
{
  try           
    {
      MStream TubeI;
      MStream ShellI;
      MStream & TubeO = FlwIOs[FlwIOs.First[idTubeO]].Stream;
      MStream & ShellO = FlwIOs[FlwIOs.First[idShellO]].Stream;
      MStream & VentO = FlwIOs[FlwIOs.First[idVentO]].Stream;

      FlwIOs.AddMixtureIn_Id(TubeI, idTubeI);
      FlwIOs.AddMixtureIn_Id(ShellI, idShellI);

      TubeO = TubeI;
      ShellO = ShellI;
      VentO = ShellI;
      VentO.SetM(ShellI, MP_All, 0);   // Set default values in case of error
      if (bOnline) {
      
	if (m_lHxMode) { 
	  doGeometry();  // Calculate area etc from layout info
	  double drat = m_dTubeOD/m_dTubeID;     // Calculate overall HTC...
	  double wallResistance = log(drat)*m_dTubeID/(2*m_dTubeConductivity);
	  double scaleResistance = 0.0;
	  if (m_bTSScaling && m_dScaleThickness > 0.0) {
	    double id = m_dTubeID-2*m_dScaleThickness;  // actual ID with scale
	    scaleResistance = log(m_dTubeID/id)*id/(2*m_dScaleConductivity);
	  }

	  m_dHTC = 1/(1/m_dTSHTC + wallResistance+scaleResistance 
		      + 1/m_dSSHTC/drat);
	  if (m_bTSScaling && m_dScaleThickness > 0.0)      // correction for reduction in HT area
	    m_dHTC *= (1.-2*m_dScaleThickness/m_dTubeID);   // due to inside scaling


	}
    

	switch (m_lOpMode) {
	case OM_Simple:       DoSimpleHeater(ShellI, TubeI, ShellO, TubeO); break;
	case OM_Condensing:  
	  DoCondensingHeater(ShellI, TubeI, ShellO, TubeO);       
	  VentO.SetM(ShellI, MP_Gas, m_dQmVentRqd);
	  break;
	case OM_LiveSteam:    
	  DoLiveSteamHeater(ShellI, TubeI, ShellO, TubeO, VentO); break;
	}    
	m_dP = 0.0;
	if (m_lHxMode)
	  flowRE(TubeI, TubeO);

	m_dSatT = ShellI.SaturationT();
	m_dSatP = ShellI.SaturationP();
	m_dSST = ShellI.T;

	m_dTSTin =  TubeI.T;
	m_dTSTout = TubeO.T;
	m_dSSTin =  ShellI.T;
	m_dSSTout = ShellO.T;

	m_dTSPin =  TubeI.P;
	m_dTSPout = TubeO.P;
	m_dSSPin =  ShellI.P;
	m_dSSPout = ShellO.P;

	m_dQmTS = TubeO.Mass();
	m_dQmSS = ShellO.Mass();

    
	m_dActualDuty = m_lEnvHxMode ? m_dEnvHeatLoss : 0;
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

void CCARTubeDigester::ClosureInfo(MClosureInfo & CI)
{
  if (CI.DoFlows())
    {
      CI.AddPowerIn(0, -m_dActualDuty);
    }
}
//====================================================================================





void CCARTubeDigester::flowRE(MStream & sIn, MStream & sOut) 
{
  double mdot = sIn.MassFlow();

  double visc = sIn.DynamicViscosity();
  double den = sIn.Density();
  double Pr;
     
  
   
  m_dG = mdot/m_dTSFlowArea;
  m_dTSVel = m_dG/den;
  m_dReIn = m_dG*m_dTubeID/visc;
  m_dftIn = ft(m_dReIn);
  m_dPdLIn = 0.5*m_dG*m_dG/den/m_dTubeID*m_dftIn/1000.;  //kPa/m
  
  visc = sOut.DynamicViscosity();
  den = sOut.Density();

  m_dReOut = m_dG*m_dTubeID/visc;
  m_dftOut = ft(m_dReOut);
  m_dPdLOut = 0.5*m_dG*m_dG/den/m_dTubeID*m_dftOut/1000.;  //kPa/m
  // Entrance and exit effects to tubes...
  m_dPEntrance = m_dTSVel*m_dTSVel*den*(m_dEntranceC+m_dExitC)*m_lNPasses/1000.;  // kPa
  if (m_lTSdPMode)
    m_dP = .5*(m_dPdLIn+m_dPdLOut)*m_lNPasses*m_dTubeLength + m_dPEntrance;
  
  
  if (m_lTSHxMode) {    // Preliminary implementatio of tubside HT calculation...
    Pr = sIn.msCp()*sIn.DynamicViscosity()/m_dThermalConductivity;
    double dNuIn = NuG(m_dReIn, Pr) ;
    Pr = sOut.msCp()*sOut.DynamicViscosity()/m_dThermalConductivity;
    double dNuOut = NuG(m_dReIn, Pr);
    m_dTSHTC = (dNuIn+dNuOut)/2*m_dThermalConductivity/m_dTubeID;
  }
}



double  ft(double Re) 
{
  // Friction Factor for Tubeside Flow - Mills'''
  double z = (0.79 * log(Re) - 1.64);
  return 1./(z*z);
}




double NuG(double Re, double Pr) {

  // Gnielinskis formula for Nussalt number//
  double f = ft(Re);
  return (f/8) * (Re-1000)*Pr/(1+12.7*sqrt(f/8)*(pow(Pr, (2. / 3.)) - 1));
}
