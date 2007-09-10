//===========================================================================
//=== SysCAD SMDK - Hyprod Precipitator Model 2003 : Alcan, Kenwalt       ===
//===========================================================================

#include "stdafx.h" 
#define  __HYPRODCLASSIF_CPP
#include "hyprodclassif.h"
#include "math.h"

#if ForceOptimizeOff
#pragma optimize("", off)
#endif

//====================================================================================

const int idFeed=0;
const int idOverflow=1;
const int idUnderflow=2;

static MInOutDefStruct s_IODefs[]=
  {
  //  Desc;             Name;       Id; Rqd; Max; CnId, FracHgt;  Options;
    { "Feed",           "Feed",      idFeed,      1,  10,    0,    1.0f,  MIO_In |MIO_Material},
    { "Overflow",       "Overflow",  idOverflow,  1,   1,    0,    1.0f,  MIO_Out|MIO_Material},
    { "Underflow",      "Underflow", idUnderflow, 1,   1,    0,    0.0f,  MIO_Out|MIO_Material},
    { NULL },
  };

static double ClassifierDraw[] = { MDrw_Poly,  -6,6,  -6,2, 0,-8, 6,2, 6,6, -6,6,
                          MDrw_End};

//---------------------------------------------------------------------------

DEFINE_SURGE_UNIT(Classifier, "Classif", DLL_GroupName)
void Classifier_UnitDef::GetOptions()
  {
  SetDefaultTag("CL", true);
  SetDrawing("SizeSeparation", ClassifierDraw);
  SetTreeDescription("Process:Unit:Alcan Classifier");
  SetDescription("Alcan Classifier based on Hyprod model");
  SetModelSolveMode(MSolveMode_Probal);
  SetModelGroup(MGroup_Alumina);
  SetModelLicense(MLicense_HeatExchange|MLicense_Alumina|MLicense_Alcan);
  };

//---------------------------------------------------------------------------

Classifier::Classifier(MUnitDefBase * pUnitDef, TaggedObject * pNd) : MBaseMethod(pUnitDef, pNd)
  {
  //default values...
  m_bOn = 1;
  m_bPrecipOn = 1;
  m_bBrentSolverSelected =1;
  m_eModel = eMdl_Gravity;
  m_eInputMethod = eIM_CutSize,

  m_eSharpEqn = eSE_Lynch;
  m_dD50 = 0.00005;
  m_dSharpness = 4.0;
  m_dRqdUFSolidsConc = 700.0;
  m_dRqdRecoveryFrac = 0.5;
  m_dRqdUFVolFlow = 2.77777777777e-4;// 100 m3/h
  m_dRqdOFSolidsTons = 10.0/3.6; //kg/s
  m_dDiameter = 10.0;
  m_dHindSettlFac = 1.0;
  m_dRf = 0.25;
  m_dUFSolidsConc = 0.0;
  m_dUFVolFlow = 0.0;
  m_dOFSolidsConc = 0.0;
  m_dOFVolFlow = 0.0;
  m_dOFSolidsTons = 0.0;
  m_dCalcRecoveryFrac = m_dRqdRecoveryFrac;
  m_dRqdRecoveryFracUsed = m_dRqdRecoveryFrac;
  m_dD50Used = m_dD50;
  }

//---------------------------------------------------------------------------

void Classifier::Init()
  {
  SetIODefinition(s_IODefs);
  }

const int idDX_Version = 1;

//---------------------------------------------------------------------------

void Classifier::BuildDataFields()
  {
  static MDDValueLst DDModels[]=
    {
    {eMdl_Gravity,  "Gravity Classifier"},
    {eMdl_Cyclone,  "Cyclone"},
    {0}
    };
  static MDDValueLst DDInputMethods[]=
    {
    {eIM_CutSize,  "Cut Size"},
    {eIM_Recovery, "Recovery"},
    {eIM_Underflow,"Underflow"},
    {eIM_Overflow, "Overflow"},
    {eIM_Model,    "Model"},
    {0}
    };
  static MDDValueLst DDSharpEqn[]=
    {
    {eSE_Lynch, "Lynch"},
    {eSE_Plitt, "Plitt"},
    {0}
    };
   static MDDValueLst DDHeatBalance[]=
    {
    {eHBal_Normal, "Normal"},
    {eHBal_ImposedTemp, "Imposed Temperature"},
  //  {eHBal_OptimumTemp, "Optimum Temperature"},
    {0}
    };
  DD.Text     ("");
  DD.String("Version", "", idDX_Version, MF_RESULT);
  DD.CheckBox ("On", "",   &m_bOn,  MF_PARAMETER);
  DD.Long     ("Model", "", (long*)&m_eModel, MF_PARAMETER|MF_SET_ON_CHANGE, DDModels);
  DD.Show(m_eModel==eMdl_Gravity);
  DD.Text     ("");
  DD.Text     ("Precipitation...");
  DD.CheckBox ("Precip", "",   &m_bPrecipOn,  MF_PARAMETER|MF_SET_ON_CHANGE);
  DD.Show(m_eModel==eMdl_Gravity && m_bPrecipOn);
  DD.Double   ("NoOperating", "", &m_dNumberInService, MF_PARAMETER,  MC_);
  DD.Double   ("Volume", "",    &m_dVolume, MF_PARAMETER,  MC_Vol("m^3"));
  DD.Text     ("");
  DD.Long     ("ThermalBalance", "", (long*)&m_eHeatBalance, MF_PARAMETER|MF_SET_ON_CHANGE, DDHeatBalance);
  DD.Show(m_eModel==eMdl_Gravity && m_eHeatBalance == eHBal_Normal && m_bPrecipOn);
  DD.Double   ("Surface", "",   &m_dSurface, MF_PARAMETER,  MC_Area("m^2"));
  DD.Show( m_eHeatBalance == eHBal_ImposedTemp && m_bPrecipOn);
  DD.Double   ("TempImposed", "", &m_dTempImpos, MF_PARAMETER, MC_T("C"));
  DD.Show(true);
  DD.Text     ("");
  DD.Text     ("Separation...");
  DD.Long     ("InputMethod", "", (long*)&m_eInputMethod, MF_PARAMETER|MF_SET_ON_CHANGE, DDInputMethods);
  DD.Long     ("SharpEqn", "", (long*)&m_eSharpEqn, MF_PARAMETER, DDSharpEqn);
  DD.Show(m_eInputMethod==eIM_CutSize);
  DD.Double   ("CutSize", "", &m_dD50,     MF_PARAMETER, MC_L("um"));
  DD.Show(true);
  DD.Double   ("Sharpness", "", &m_dSharpness,     MF_PARAMETER ,MC_);
  DD.Show(m_eInputMethod!=eIM_Model );
  DD.Double   ("RqdUF_SolidsConc", "", &m_dRqdUFSolidsConc, MF_PARAMETER, MC_Conc("g/L"));
  DD.Show(m_eInputMethod==eIM_Recovery);
  DD.Double   ("RqdRecovery", "", &m_dRqdRecoveryFrac, MF_PARAMETER, MC_Frac("%"));
  //DD.Show(m_eInputMethod==eIM_Model || m_eInputMethod==eIM_Underflow);
  //DD.Show(m_eInputMethod!=eIM_CutSize && m_eInputMethod!=eIM_Recovery);
  DD.Show(true);
  DD.CheckBox ("Use Brent Solver method", "", &m_bBrentSolverSelected, MF_PARAMETER|MF_SET_ON_CHANGE);
  DD.Double   ("RqdUF_VolFlow", "", &m_dRqdUFVolFlow, (m_eInputMethod==eIM_Model || m_eInputMethod==eIM_Underflow) ? MF_PARAMETER : MF_RESULT, MC_Qv("m^3/h"));
  DD.Show(m_eInputMethod==eIM_Overflow);
  DD.Double   ("RqdOF_SolidsFlow", "", &m_dRqdOFSolidsTons, MF_PARAMETER, MC_Qm("t/h"));
  DD.Show(m_eInputMethod==eIM_Model);
  DD.Double   ("TankDiameter", "", &m_dDiameter, MF_PARAMETER, MC_L("m"));
  DD.Double   ("HinderSettlingFactor", "", &m_dHindSettlFac, MF_PARAMETER, MC_);
  DD.Show(true);
  DD.Text     ("");
  DD.Text     ("Results");
  DD.Show(m_eModel==eMdl_Gravity && m_bPrecipOn);
  DD.Double("Yield", "", &m_dYield, MF_RESULT, MC_Conc("g/L"));
  DD.Show(true);
  DD.Double("Ti", "", &m_dTempIn, MF_RESULT, MC_T("C"));
  DD.Double("To", "", &m_dTempOut, MF_RESULT, MC_T("C"));
  DD.Double("ACin", "", &m_dACIn, MF_RESULT, MC_);
  if (m_eModel==eMdl_Gravity && m_bPrecipOn)
    DD.Double("ACout", "", &m_dACOut, MF_RESULT, MC_);
  DD.Double("SolidConcIn", "", &m_dSolidConcIn, MF_RESULT, MC_Conc("g/L"));
  //DD.Double("SolidConcOut", "", &m_dSolidConcOut, MF_RESULT, MC_Conc("g/L"));
  DD.Double   ("Rf", "", &m_dRf, MF_RESULT, MC_Frac );
  DD.Double   ("Recovery", "", &m_dCalcRecoveryFrac, MF_RESULT, MC_Frac("%"));
  DD.Double   ("CutSizeUsed", "", &m_dD50Used, MF_RESULT, MC_L("um"));
  DD.Double   ("UF_SolidsConc", "", &m_dUFSolidsConc, MF_RESULT, MC_Conc("g/L"));
  DD.Double   ("UF_VolFlow", "", &m_dUFVolFlow, MF_RESULT, MC_Qv("m^3/h"));
  DD.Double   ("OF_SolidsConc", "", &m_dOFSolidsConc, MF_RESULT, MC_Conc("g/L"));
  DD.Double   ("OF_VolFlow", "", &m_dOFVolFlow, MF_RESULT, MC_Qv("m^3/h"));
  DD.Double   ("OF_SolidsFlow", "", &m_dOFSolidsTons, MF_RESULT, MC_Qm("t/h"));

  DD.Object(m_QProd, MDD_RqdPage);
  }
//---------------------------------------------------------------------------

bool Classifier::ExchangeDataFields()
  {
  switch (DX.Handle)
    {
    case idDX_Version: 
      DX.String = VersionDescription;
      return true;
    }
  return false;
  }


//---------------------------------------------------------------------------

bool Classifier::ValidateDataFields()
  {//ensure parameters are within expected ranges
   if (m_eModel==eMdl_Cyclone && m_eInputMethod==eIM_Model)
			m_eInputMethod = eIM_Recovery;
   m_dD50      = DV.ValidateRange("", 0.000001,  m_dD50 , 0.0005);
   m_dD50Used  = DV.ValidateRange("", 0.000001,  m_dD50 , 0.0005);
   m_dRqdRecoveryFrac = DV.ValidateRange("", 0.01,  m_dRqdRecoveryFrac , 0.995);


//!!!! NEED TO PUT A LOT OF SAFETY CHECKS !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


  return true;
  }

//---------------------------------------------------------------------------

bool Classifier::CalcPrecipition(MVector & Feed, MVector & Prod)
  {
  try
    {
    int ErrCode = PrecipSS(Feed, Prod, m_QProd);
    switch (ErrCode)
      {
      case 0:
        HeatBalance(Feed, Prod, Prod.T);
        break;
      case -1: Log.SetCondition(6, MMsg_Error, "PrecipSS Failed - No Solids"); return false;
      case -2: Log.SetCondition(6, MMsg_Error, "PrecipSS Failed - No Caustic"); return false;
      case -3: Log.Message(MMsg_Note, "Pas Precipitation"); return false;
      case -4: Log.SetCondition(6, MMsg_Error, "PrecipSS Failed - Infinite solve loop"); return false;
      }
    }
  catch(MMdlException & e)
    {
    Log.SetCondition(5, MMsg_Error, "Exception Occurred in CalcPrecipition %s", e.Description);
    SetIdleRequired("Phone Denis");
    return false;
    }
  catch(...)
    {
    Log.SetCondition(5, MMsg_Error, "Exception Occurred in CalcPrecipition");
    SetIdleRequired("Phone Denis");
    return false;
    }
  return true;
  }

//---------------------------------------------------------------------------

bool Classifier::CalcSeparationNoPSD(MVector & Feed, MVector & OF, MVector & UF)
  {
  double FeedSolMass = Feed.Mass(MP_Sol);
  double FeedLiqVolFlow = Feed.Volume(MP_Liq);
  double TtlUFSolids;
  double UFVolFlow;
  if  (m_eInputMethod==eIM_Recovery) 
    {
    TtlUFSolids = m_dRqdRecoveryFrac * FeedSolMass;
    UFVolFlow = TtlUFSolids/m_dRqdUFSolidsConc; // m^3/s
    }
  else 
    {
    TtlUFSolids = m_dRqdRecoveryFrac * FeedSolMass;//todo
    UFVolFlow = m_dRqdUFVolFlow;
    }

  double UFSolVolFlow = TtlUFSolids/UF.Density(MP_Sol);
  double UFLiqVolFlow = UFVolFlow - UFSolVolFlow;
  double LiquidToOF_Frac = GEZ(1.0 - (UFLiqVolFlow/GTZ(FeedLiqVolFlow)));
  double SolidsToOF_Frac = GEZ(1.0 - (TtlUFSolids/GTZ(FeedSolMass)));

  for (int i=0; i<gs_MVDefn.Count(); i++)
    {
    if (gs_MVDefn[i].IsSolid())
      {
      OF.M[i] = Feed.M[i]*SolidsToOF_Frac;
      }
    else if (gs_MVDefn[i].IsLiquid())
      {
      OF.M[i] = Feed.M[i]*LiquidToOF_Frac;
      }
    else if (gs_MVDefn[i].IsGas())
      {
      OF.M[i] = Feed.M[i];
      }
    UF.M[i] = Feed.M[i]-OF.M[i];
    }
  m_dCalcRecoveryFrac = UF.Mass(MP_Sol)/FeedSolMass;
  return true;
  }

//---------------------------------------------------------------------------

bool Classifier::ComputeEfficiency(MVector & Feed, MVector & OF, MVector & UF, double Beta)
  {
  MIPSD   &FdPSD = *Feed.GetIF<MIPSD>(); //actual PSD data
  MIPSD   &OFPSD = *OF.GetIF<MIPSD>(); //actual PSD data
  MIPSD   &UFPSD = *UF.GetIF<MIPSD>(); //actual PSD data
  MPSDDefn &SD = FdPSD.getDefn(); //sieve series definition
	const int NumComps = SD.getPSDVectorCount(); // Number of components in the stream usisng size data
  Eu.SetSize(NIntervals);

  double SolidsToOF_Frac = 0.0;

  //todo what if NumComps>1 !!!!!!!
//if (sm_bCompletePopulation)
 
  for (int c=0; c<NumComps; c++)
    {
    const int SpId = SD.SpecieIndex[c];
    OF.M[SpId] = 0.0;
    UF.M[SpId] = 0.0;
    double FdMassFlow = Feed.M[SpId];
    double* pDataFd = FdPSD.getFracVector(c);
    double* pDataOF = OFPSD.getFracVector(c);
    double* pDataUF = UFPSD.getFracVector(c);
    for (int mm=0; mm<NIntervals; mm++)
      {
      const double d = SizeClass[mm]/(m_dD50Used*1.0e6);
  	  if (m_eSharpEqn == eSE_Lynch)
        {
        const double tmp1 = exp(m_dSharpness);
        const double tmp2 = exp(m_dSharpness*SizeClass[mm]/(m_dD50Used*1.0e6));
        const double tmp = (tmp1 - 1.0)/(tmp1 + tmp2 - 2.0);
        Eu[mm] = 1.0 + (1.0 - tmp)*Beta - Beta ;
        }
      else
        {
        Eu[mm] = 1.0 - Beta * exp(-0.693147*pow(d, m_dSharpness));
        }
      pDataUF[mm] = (pDataFd[mm] * FdMassFlow) * Eu[mm];
      UF.M[SpId] += pDataUF[mm];
      pDataOF[mm] = Max(0.0, (pDataFd[mm] * FdMassFlow) - pDataUF[mm]);
      OF.M[SpId] += pDataOF[mm];
      }

    for (mm=0; mm<NIntervals; mm++)
      {
      pDataOF[mm] /= GTZ(OF.M[SpId]);
      pDataUF[mm] /= GTZ(UF.M[SpId]);
      }
    }

  double FdMassFlow = 0;
  for (int c=0; c<NumComps; c++)
    {
    const int SpId = SD.SpecieIndex[c];
    SolidsToOF_Frac += OF.M[SpId];
    FdMassFlow += Feed.M[SpId];
    }
  SolidsToOF_Frac /= GTZ(FdMassFlow);
  SolidsToOF_Frac = Max(0.0, SolidsToOF_Frac);
  
  //solids split first
  double TtlUFSolids = 0.0; // kg/s
  for (int i=0; i<gs_MVDefn.Count(); i++)
    {
    if (gs_MVDefn[i].IsSolid())
      {
      OF.M[i] = Feed.M[i]*SolidsToOF_Frac;
      UF.M[i] = Feed.M[i]-OF.M[i];
      TtlUFSolids += UF.M[i];
      }
    }

  //now calculate liquid split...
  double UFVolFlow;
  if  (m_eInputMethod==eIM_CutSize || m_eInputMethod==eIM_Recovery) 
    {
    UFVolFlow = TtlUFSolids/m_dRqdUFSolidsConc; // m^3/s
    }
  else 
    {
    UFVolFlow = m_dRqdUFVolFlow;
    }
  double UFSolVolFlow = TtlUFSolids/UF.Density(MP_Sol);
  double UFLiqVolFlow = UFVolFlow - UFSolVolFlow;
  double FeedLiqVolFlow = Feed.Volume(MP_Liq);
  double LiquidToOF_Frac = 1.0 - (UFLiqVolFlow/GTZ(FeedLiqVolFlow));
  LiquidToOF_Frac = Max(0.0, LiquidToOF_Frac);

  for (i=0; i<gs_MVDefn.Count(); i++)
    {
    if (gs_MVDefn[i].IsLiquid())
      {
      OF.M[i] = Feed.M[i]*LiquidToOF_Frac;
      UF.M[i] = Feed.M[i]-OF.M[i];
      }
    else if (gs_MVDefn[i].IsGas())
      {
      OF.M[i] = Feed.M[i];
      UF.M[i] = 0.0;
      }
    }
  return true;
  }

//---------------------------------------------------------------------------

class CRecoveryFn: public MRootFinder
  {
  public:
	  static MToleranceBlock s_Tol;
  private:
  	Classifier & TheClassifier;
    MVector & Feed;
    MVector & OF;
    MVector & UF;
	  double TtlVolFlow;
    double FdSolMass;

  public: 
    CRecoveryFn(Classifier & Classifier_, MVector & Feed_, MVector & OF_, MVector & UF_) : MRootFinder("ClassifierRecoveryFn", s_Tol),
		  Feed(Feed_), OF(OF_), UF(UF_), TheClassifier(Classifier_)
      {
      TtlVolFlow = Feed.Volume();
      FdSolMass = Feed.Mass(MP_Sol);
	    }
	double Function(double x) 
    {
    TheClassifier.m_dD50Used = x;
    //const int MaxLoop = ((TheClassifier.m_eInputMethod!=eIM_Underflow) ? 5 : 2);
    const int MaxLoop = 50;
    double UFvol = UF.Volume();
    if (TheClassifier.m_eInputMethod!=eIM_Underflow)
      TheClassifier.m_dRf = UFvol/TtlVolFlow;//using previous UF.Volume can be a problem!?!
    double beta = 1.0 - TheClassifier.m_dRf;
    double PrevRf = TheClassifier.m_dRf;
    //require a second internal convergence because change in UF.Volume changes Rf (ie beta)
    for (int i=0; i<MaxLoop; i++)
      {
      TheClassifier.ComputeEfficiency(Feed, OF, UF, beta);
      UFvol = UF.Volume();
      if (TheClassifier.m_eInputMethod!=eIM_Underflow)
        TheClassifier.m_dRf = UFvol/TtlVolFlow;
      beta = 1.0 - TheClassifier.m_dRf;
      if (fabs(TheClassifier.m_dRf-PrevRf)<1.0e-5)
        break;
      PrevRf = TheClassifier.m_dRf;
      }
    if (i==MaxLoop)
      {//Report error when this doesn't converge!!??
      int xxx=0;
      }
    const double UFmass = UF.Mass(MP_Sol);
    const double recovery = UFmass/FdSolMass;
      int xxx=0;
    return recovery;
    }		 
  };
MToleranceBlock CRecoveryFn::s_Tol(TBF_Both, "AlcanClassifier:RecoveryFn", 1.0-6, 0.0, 100);

//---------------------------------------------------------------------------

bool Classifier::CalcSeparation(MVector & Feed, MVector & OF, MVector & UF)
  {
  const double FdSolMass = Feed.Mass(MP_Sol); 

  Log.SetCondition(FdSolMass<1.0e-9, 12, MMsg_Warning, "No solids in the feed - liquids report to UF");
  Log.ClearCondition(4);
  if (FdSolMass<1.0e-9)
    {
    UF = Feed;
    OF.ZeroMass();
    m_dCalcRecoveryFrac = 0.0;
    return true;
    }

  const double Min_d50 = 1.0e-8;
  InitSizeData(Feed);
  //MIBayer & UFB = *UF.GetIF<MIBayer>();
  MIBayer & FeedB = *Feed.GetIF<MIBayer>();
  const double T = Feed.T;
  const double TtlVolFlow = Feed.Volume();
  m_dD50 = Valid(m_dD50) ? Max(Min_d50, m_dD50) : Min_d50*10.0;


  switch (m_eInputMethod)
    {
    case eIM_CutSize:
    case eIM_Model:
      {
      const bool UseBrentSolver = false;
      if (UseBrentSolver)
        {
        //TODO: Rather use Brent solver!?!
        }
      else
        {
        //double Rf = 0.25;      // Assumed starting value
        double beta = 1.0 - m_dRf;
        //const double Visc = Feed.DynamicViscosity(MP_Liq);
        const double Visc = 0.002; //fixed value for Bayer typical liquor in kg /m.s (perhaps make this a parameter that the user can change?)
        const double Densliq = Feed.Density(MP_Liq);
        const double gpl = FeedB.SolidsConc(T);
        m_dD50Used = m_dD50;
        ComputeEfficiency(Feed, OF, UF, beta);
        int reply=0;
        double prev_Rf = m_dRf;
        m_dRf = UF.Volume() / TtlVolFlow;
        while ((fabs(prev_Rf - m_dRf ) > 0.0001) && (reply <= 100))
          {
          m_dRf = UF.Volume() / TtlVolFlow;
          beta = 1.0 - m_dRf; // This line SHOULD NOT be commented .. Except to simulate HYPROD results
          if (m_eInputMethod == eIM_Model)
 	          {  
            double speed = ( TtlVolFlow - UF.Volume() ) / ( PI * Sqr( m_dDiameter)/4); // OVERFLOW FLOW in m/s
            double K1 = Pow( (1.0 - gpl/THADens), 4.65); //effect of solids concentration Ref. Perry 
            m_dD50Used = Sqrt(speed * Visc/(1.08888* K1* m_dHindSettlFac * (THADens-Densliq)));// in microns
            if (m_dD50Used<0.0)
              {//Potential error!!!???
              m_dD50Used = Min_d50;
              }
            }
          ComputeEfficiency(Feed, OF, UF, beta);
          prev_Rf = m_dRf;
          reply++;
          }
        //m_dRqdUFVolFlow = UF.Volume();// do not want to override this!!!?!
        //m_dRqdRecoveryFrac = UF.Mass(MP_Sol)/FdSolMass; do not want to override this!!!?!
        Log.SetCondition(reply>99, 4, MMsg_Error, "Probleme de convergence de densité");
        }
      break;
      }
   
    case eIM_Overflow:
    case eIM_Underflow:
    case eIM_Recovery:
      {
      if (m_eInputMethod==eIM_Overflow)
	      {
		    double UFmass = Max(0.0, FdSolMass - m_dRqdOFSolidsTons);
		    m_dRqdUFVolFlow = UFmass/m_dRqdUFSolidsConc;
			m_dRqdRecoveryFracUsed =  UFmass/FdSolMass;
			m_dRf = (TtlVolFlow - m_dRqdUFVolFlow) / TtlVolFlow;
	      }
      if (m_eInputMethod==eIM_Underflow)
        {
        //m_dRqdRecoveryFrac = m_dRqdUFVolFlow * m_dRqdUFSolidsConc / FdSolMass; do not want to override this!!!?!
        m_dRqdRecoveryFracUsed = m_dRqdUFVolFlow * m_dRqdUFSolidsConc / FdSolMass;
        m_dRf = m_dRqdUFVolFlow/TtlVolFlow;
        }
      if (m_eInputMethod==eIM_Recovery)
        {
        m_dRqdRecoveryFracUsed = m_dRqdRecoveryFrac;
		double UFmass = Max(0.0, FdSolMass * m_dRqdRecoveryFracUsed);
		m_dRqdUFVolFlow = UFmass/m_dRqdUFSolidsConc;
		m_dRf = (TtlVolFlow - m_dRqdUFVolFlow) / TtlVolFlow;
        }


if ( (m_dRqdRecoveryFracUsed >= 1) || ( (m_dRf < 0) && (m_dRf > 1)) )
			{
			SetStopRequired("Insufficient Feed : revise flowsheet!");
			Log.SetCondition( true , 12, MMsg_Error, "Insufficient Feed to Classifier!use Recovery Mode");
			break;
			}


const bool UseBrentSolver = m_bBrentSolverSelected;

      if (UseBrentSolver)
        {
        //Solve to determine new d50 to achieve required recovery...
        const double Target = m_dRqdRecoveryFracUsed;
        double d50_Est;
        if (m_dD50 > 0 )
          d50_Est = m_dD50; 
        else
          d50_Est = 80;
        CRecoveryFn Fn(*this, Feed, OF, UF);
        if (Fn.FindRoot(Target, d50_Est/2.0, d50_Est*2, m_dD50Used)!=RF_OK)
          {//try again with less strict range...
          //switch (Fn.FindRoot(Target, Min_d50, SizeClass[NIntervals-1]*1.0e-3, m_dD50Used)) //todo: improve min/max range
          switch (Fn.FindRoot(Target, d50_Est/10.0, d50_Est*10.0, m_dD50Used))
            {
            case RF_OK:
              break;
            case RF_LoLimit:
              Log.SetCondition(true, 4, MMsg_Error, "Recovery cutSize RootFinder at LOW limit");
              break;
            case RF_HiLimit:
              Log.SetCondition(true, 4, MMsg_Error, "Recovery cutSize RootFinder at HIGH limit");
              break;
            default: 
              Log.SetCondition(true, 4, MMsg_Error, "Recovery cutSize RootFinder:%s", Fn.ResultString(Fn.Error())); 
              break;
            }
          }
        //m_dRqdUFVolFlow = UF.Volume();// DO WE WANT THIS???
        }
      else
        {     // ---------    SECANT  Root Finding Method   -----------------
		double beta = 1.0 - m_dRf;
        m_dD50Used = m_dD50;
        ComputeEfficiency(Feed, OF, UF, beta);
        m_dRf = UF.Volume() / TtlVolFlow;
		beta = 1.0 - m_dRf;
        ComputeEfficiency(Feed, OF, UF, beta);
		double NewFdSolMass = Feed.Mass(MP_Sol);
        double y0 = FdSolMass * m_dRqdRecoveryFracUsed - UF.Mass(MP_Sol);
        double Cut0 = m_dD50Used *1.0e6 ;
        
		m_dD50Used = m_dD50Used *1.1;
        ComputeEfficiency(Feed, OF, UF, beta);
        double y1 = FdSolMass * m_dRqdRecoveryFracUsed - UF.Mass(MP_Sol);
        double Cut1 = m_dD50Used *1.0e6 ;
     
		int reply=0;
        while ((fabs(y1) > 0.00001) && (reply <= 100))
          {
		  if ( (y0 >0.0)&& (y1 > y0)) // the secant goes the wrong way so we have to invert the pair of points
		   { double y_temp = y1;
			 double Cut_temp = Cut1;
			 y1	  = y0;
			 Cut1 = Cut0;
			 y0	  = y_temp;
			 Cut0 = Cut_temp;
			}
          m_dRf = UF.Volume() / TtlVolFlow;
          beta = 1.0 - m_dRf; // This line SHOULD NOT be commented .. Except to simulate HYPROD results
          m_dD50Used = ( Cut0 - y0* (Cut1-Cut0)/(y1 - y0) ) / 1.0e6;
          if (m_dD50Used<0.0)
            {//Trouble!!!???
            int xx=0;
            m_dD50Used = Min_d50;
            }
          y0 = y1;
          Cut0 = Cut1 ;
          ComputeEfficiency(Feed, OF, UF, beta);
          y1 = FdSolMass * m_dRqdRecoveryFracUsed - UF.Mass(MP_Sol);
          Cut1 = m_dD50Used *1.0e6;
          reply++;
          }
        m_dRqdUFVolFlow = UF.Volume();
        Log.SetCondition(reply>99, 4, MMsg_Error, "Probleme de convergence de densité");
        }
      break;
      }
    }
  m_dCalcRecoveryFrac = UF.Mass(MP_Sol)/FdSolMass;
  return true;
  }

//---------------------------------------------------------------------------

void Classifier::EvalProducts()
  {
  if (!IsSolveDirect)//(!IsProbal)
    return;
  MStreamI Prod;
  bool SplitErr = false;
  try
    {
    MStreamI Feed;
    FlwIOs.AddMixtureIn_Id(Feed, idFeed);
    MStream & OF = FlwIOs[FlwIOs.First[idOverflow]].Stream;
    MStream & UF = FlwIOs[FlwIOs.First[idUnderflow]].Stream;

    Prod = Feed;
    m_dThermalLoss = 0.0;
    m_dYield = 0.0;
    m_dTempIn = Feed.T;
    MIBayer & FeedB=*Feed.FindIF<MIBayer>();
    if (!IsNothing(FeedB))
      {
      m_dACIn = FeedB.AtoC();
      m_dSolidConcIn = FeedB.SolidsConc(C2K(25.0));
      }
    else
      {
      m_dACIn = 0.0;
      m_dSolidConcIn = 0.0;
      }

    if (m_bOn && Feed.Mass()>UsableMass)
      {
      //first precipitation...
      if (m_bPrecipOn && m_eModel==eMdl_Gravity)
        {
        MIBayer & ProdB=*Prod.FindIF<MIBayer>();
        MISSA & FeedSSA=*Feed.FindIF<MISSA>();
        Log.SetCondition(IsNothing(FeedB), 1, MMsg_Warning, "Bad Feed Stream - Not Bayer Model"); //expect stream to have bayer properties
        Log.SetCondition(IsNothing(FeedSSA), 2, MMsg_Warning, "Bad Feed Stream - SSA must be present"); //expect stream to have bayer properties
        if (!IsNothing(FeedB) && !IsNothing(FeedSSA))
          {
          if (!CalcPrecipition(Feed, Prod))
            {//error occured
            if (fabs(Feed.Mass()-Prod.Mass())>10.e-6)
              {//serious problem, set prod = feed
              Prod = Feed;
              }
            m_dACOut = ProdB.AtoC();
            }
          else
            {
            double H0 = Feed.totHf(MP_All, Feed.T, Feed.P);
            m_dThermalLoss = H0-Prod.totHf(MP_All, Prod.T, Prod.P);
            m_dACOut = ProdB.AtoC();
            double Cout = ProdB.CausticConc(Prod.T);
            m_dYield = Cout*(m_dACIn-m_dACOut);
            //if (sm_bUsePrevPSD)
            //  {
            //  SetPSDfromPrev(Prod);
            //  }
			}
          }
        }
      m_QProd = Prod; //the stream after precipitation but before separation
        
      //now split...
      SplitErr = true;
      OF = Prod;
      UF = Prod;
      if (!sm_bCompletePopulation)
        {
        CalcSeparationNoPSD(Prod, OF, UF);
        }
      else
        {
        MIPSD & ProdSz = *Prod.FindIF<MIPSD>();
        Log.SetCondition(IsNothing(ProdSz), 3, MMsg_Warning, "Bad Feed Stream - No PSD present");
        if (!IsNothing(ProdSz))
          {
          CalcSeparation(Prod, OF, UF);
          }
        else
          {
          OF = Prod;
          UF = Prod;
          UF.ZeroMass();
          }
        }
      SplitErr = false;
      }
    else
      {
      OF = Prod;
      UF = Prod;
      UF.ZeroMass();
      }
    
    //results...
    m_dTempOut = Prod.T;
    MIBayer & UFB=*UF.FindIF<MIBayer>();
    MIBayer & OFB=*OF.FindIF<MIBayer>();
    if (!IsNothing(UFB) && !IsNothing(OFB))
      {
      m_dUFSolidsConc = UFB.SolidsConc(m_dTempOut);
      m_dOFSolidsConc = OFB.SolidsConc(m_dTempOut);
      }
    else
      {
      m_dUFSolidsConc = 0.0;
      m_dOFSolidsConc = 0.0;
      }
    m_dUFVolFlow = UF.Volume();
    m_dOFVolFlow = OF.Volume();
    m_dOFSolidsTons = OF.Mass(MP_Sol);
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

  if (SplitErr)
    {//Something is wrong!!!  Bug needs fixing!!! 
    //So lets set OF=feed and UF=0....
    MStream & OF = FlwIOs[FlwIOs.First[idOverflow]].Stream;
    MStream & UF = FlwIOs[FlwIOs.First[idUnderflow]].Stream;
    OF = Prod;
    UF = Prod;
    UF.ZeroMass();
    SetStopRequired("Phone Denis");
    }
  Log.SetCondition(SplitErr, 11, MMsg_Error, "CalcSeparation Error needs fixing!!!");
  }

//--------------------------------------------------------------------------

void Classifier::ClosureInfo(MClosureInfo & CI)
  {
  if (CI.DoFlows())
    {
    CI.AddPowerIn(0, -m_dThermalLoss);
    }
  }
//--------------------------------------------------------------------------

bool Classifier::PreStartCheck()
  {
  if (!sm_bCompletePopulation )
   {
	m_eInputMethod = eIM_Recovery; // force the method to be recovery 
	if ( m_dCalcRecoveryFrac<= 0)
	  {
	  m_sErrorMsg = "Invalid Method"; 
	  return false;
	  }
	else
	 {
	m_dRqdRecoveryFrac = m_dCalcRecoveryFrac;
     }
   }
  return true; 
  }

//====================================================================================
