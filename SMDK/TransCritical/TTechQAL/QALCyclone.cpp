//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __QALCYCLONE_CPP
#include "qalcyclone.h"
#include "math.h"
//#pragma optimize("", off)

//====================================================================================

CSeparWhitenPSD::CSeparWhitenPSD()
  {
  dD50 = 0.001; //m
  dAlpha = 4.0;
  }

//---------------------------------------------------------------------------

void CSeparWhitenPSD::BuildDataFieldsConfig(MBaseMethod & Mdl)
  {
  Mdl.DD.ObjectBegin("Separ_Whiten", "Whiten" );
  Mdl.DD.Text("Requirements...");
  Mdl.DD.Double("d50", "", &dD50, MF_PARAMETER, MC_L("mm"));
  Mdl.DD.Double("Alpha", "", &dAlpha, MF_PARAMETER, MC_None);
  //Mdl.DD.Double("Beta", "", &dBeta, MF_PARAMETER, MC_None);
  Mdl.DD.Text("");
  Mdl.DD.ObjectEnd();
  }

//---------------------------------------------------------------------------

void CSeparWhitenPSD::BuildDataFieldsResult(MBaseMethod & Mdl)
  {
  Mdl.DD.Page("PartCurve");
  CString Tg;
  for (int i=0; i<Eu.GetSize(); i++)
    {
    Tg.Format("Part%02d", i);
    Mdl.DD.Double((char*)(const char*)Tg, "", &Eu[i], MF_RESULT|MF_NO_FILING, MC_Frac);
    }
  }

//---------------------------------------------------------------------------

int CSeparWhitenPSD::CalcSeparation(MBaseMethod & Mdl, MStream & QI, MStream & Qof, MStream & Quf)
  {
  // Sanity Check d50
  if (dD50<1.0e-12)
    dD50 = 1.0e-12;

  MIPSD & PSD = QI.IF<MIPSD>(false);
  MIPSD & PSDof = Qof.IF<MIPSD>(false);
  MIPSD & PSDuf = Quf.IF<MIPSD>(false);

  const long l_SizeCount = PSD.getSizeCount();
  const long l_PSDVectorCount = PSD.getPSDVectorCount();

  //Build probability passing from specified d50, Alpha parameters and PSD size interval data
  Eu.SetSize(l_SizeCount);
  Eu[0] = 1.0;
  for (int lSzIndex=1; lSzIndex<l_SizeCount; lSzIndex++)
    {
    const double NominalSize = PSD.getDefn().getGeometricMean(lSzIndex); //use geometrical mean
    const double d = dAlpha*NominalSize/dD50;
    if (d>230.0)//limit of exp()
      Eu[lSzIndex] = 1.0;
    else
      {
      Eu[lSzIndex] = (exp(d)-1.0) / (exp(d) + exp(dAlpha) - 2.0);
      if (Eu[lSzIndex]>1.0)
        Eu[lSzIndex] = 1.0;
      }
    }

  // Calculate the Split Between Undersize and Oversize for each species
  // that has size data.
  for (int lPSDVec=0; lPSDVec<l_PSDVectorCount; lPSDVec++)
    {
    // Example of getting the species Index in the stream related to this PSD
    // long lSpeciesIndex = getSpecieIndex(lPSDVec);

    // Create some memory for the input and output PSD mass data
    double* M = new double[l_SizeCount];
    double* M_os = new double[l_SizeCount];
    double* M_us = new double[l_SizeCount];

    // Populate the input mass vector
    // The underlying implementation only stores
    // fractions. Hence we cannot get access to the individual
    // masses for each size interval directly.
    // This method works out the actual mass
    // for each size interval and populates the specified vector
    PSD.ExtractMassVector(M, lPSDVec);

    // Calculate the split
    double l_OF = 0.0;
    double l_US = 0.0;

    for (int lSzIndex=0; lSzIndex<l_SizeCount; lSzIndex++)
      {
      // Get the calculated split for the Size
      const double Split = Eu[lSzIndex];

      // The overflow
      l_OF = M[lSzIndex]*Split;
      M_os[lSzIndex] = l_OF;

      // The underflow
      l_US = M[lSzIndex] - l_OF;
      M_us[lSzIndex] = l_US;
      }

    // Set the output Masses
    // The underlying implementation only stores the total mass
    // and the PSD fractions. This method re-calculates the total
    // mass and underlying PSD fractions from the supplied mass vector
    PSDof.ReplaceMassVector(M_os, lPSDVec );
    PSDuf.ReplaceMassVector(M_us, lPSDVec );

    // Cleanup
    delete [] M;
    delete [] M_os;
    delete [] M_us;
    }


  // Handle species without size data (send all to Quf)...
  for (int l_SpeciesIndex=0; l_SpeciesIndex<QI.Count(); l_SpeciesIndex++)
    {
    if (QI.HasSizeData(l_SpeciesIndex)==false)
      {
      // Species has no Size Data
      Qof.putM(l_SpeciesIndex, 0.0);
      Quf.putM(l_SpeciesIndex, QI.getM(l_SpeciesIndex) );
      }
    }
  return 0;
  }

//====================================================================================

CSeparPlittPSD::CSeparPlittPSD()
  {
  m_NoOfCyclones = 1.0;
  m_SIDc = 6.0   * 0.0254;
  m_SIDo = 1.0   * 0.0254;
  m_SIDu = 0.707 * 0.0254;
  m_SIDi = 1.09  * 0.0254;
  m_SIh = 17.0  * 0.0254;
  m_RefTemp = StdT;
  m_Factor1_d50 = 1.0;
  m_Factor2_Sharp = 1.0;
  m_Factor3_DP = 1.0;
  m_Factor4_S = 1.0;
  m_Factor5_OFLiq = 1.0;
  m_FdLiqVisc = 0.0;
  m_ViscOption = 0;
  m_UserVisc = 0.89e-3;
  m_RfMem = 0.0;
  m_Alpha = 0.0;
  m_d50 = 0.0;
  m_S = 0.0;
  m_H = 0.0;
  m_m = 0.0;
  m_Rv = 0.1;
  m_Rf = 0.1;
  }

//---------------------------------------------------------------------------
enum eViscOpts { VO_None, VO_Model, VO_User };

void CSeparPlittPSD::BuildDataFieldsConfig(MBaseMethod & Mdl)
  {
  static MDDValueLst  DDViscOpt[] = 
    { {VO_None, "None", 0},
      {VO_Model, "Feed", 0},
      {VO_User, "User", 0},
      NULL };

  Mdl.DD.ObjectBegin("Separ_Plitt", "Plitt" );
  Mdl.DD.Text("Requirements...");
  Mdl.DD.Double("Cyclone_Diameter",    "Dc",    &m_SIDc,           MF_PARAMETER, MC_L("mm"));
  Mdl.DD.Double("Inlet_Diameter",      "Di",    &m_SIDi,           MF_PARAMETER, MC_L("mm"));
  Mdl.DD.Double("Vortex_Diameter",     "Do",    &m_SIDo,           MF_PARAMETER, MC_L("mm"));
  Mdl.DD.Double("Spigot_Diameter",     "Du",    &m_SIDi,           MF_PARAMETER, MC_L("mm"));
  Mdl.DD.Double("Height",              "h",     &m_SIh,            MF_PARAMETER, MC_L("mm"));
  Mdl.DD.Long("ViscosityOption",       "ViscOpt", &m_ViscOption,   MF_PARAMETER|MF_SET_ON_CHANGE, DDViscOpt);
  Mdl.DD.Show(m_ViscOption==VO_User);
  Mdl.DD.Double("LiqViscOverride",     "ViscOvr", &m_UserVisc,     MF_PARAMETER, MC_Visc("cP"));
  Mdl.DD.Show();
  Mdl.DD.Double("RefTemperature",      "RefT",  &m_RefTemp,        MF_PARAMETER, MC_T("C"));
  
  //DDB.Double("Scale",              "",             DC_Frac, "",       &m_Scale,  m_pCyc, Flags|isParm|NAN_OK);
  Mdl.DD.Double("d50_Factor",          "F1",    &m_Factor1_d50,    MF_PARAMETER, MC_None);
  Mdl.DD.Double("Sharp_Factor",        "F2",    &m_Factor2_Sharp,  MF_PARAMETER, MC_None);
  Mdl.DD.Double("Pressure_Factor",     "F3",    &m_Factor3_DP,     MF_PARAMETER, MC_None);
  Mdl.DD.Double("S_Factor",            "F4",    &m_Factor4_S,      MF_PARAMETER, MC_None);
  Mdl.DD.Double("OFLiq_Factor",        "F5",    &m_Factor5_OFLiq,  MF_PARAMETER, MC_None);
  Mdl.DD.Text("");
  Mdl.DD.Text("Results");
  Mdl.DD.Double("FeedLiqVisc", "",      &m_FdLiqVisc, MF_RESULT|MF_NO_FILING, MC_Visc("cP"));
  Mdl.DD.Double("",     "QvPerCyclone", &m_Qvc,       MF_RESULT, MC_Qv("m^3/s"));
  Mdl.DD.Double("",     "VolFrac",      &m_Phi,       MF_RESULT, MC_Frac("%"));
  Mdl.DD.Double("",     "d50",          &m_d50,       MF_RESULT, MC_L("mm"));
  Mdl.DD.Double("",     "PressDrop",    &m_DP,        MF_RESULT, MC_DP("kPa"));
  Mdl.DD.Double("",     "S",            &m_S,         MF_RESULT, MC_None);
  Mdl.DD.Double("",     "Rv",           &m_Rv,        MF_RESULT, MC_Frac("%"));
  Mdl.DD.Double("Sharpness", "m",       &m_m,         MF_RESULT, MC_None);
  Mdl.DD.Double("",     "Alpha",        &m_Alpha,     MF_RESULT, MC_None);
  Mdl.DD.Double("",     "Rs",           &m_Rs,        MF_RESULT, MC_Frac("%"));
  Mdl.DD.Double("",     "Rf",           &m_Rf,        MF_RESULT, MC_Frac("%"));
  Mdl.DD.Text("");
  Mdl.DD.ObjectEnd();
  }

//---------------------------------------------------------------------------

void CSeparPlittPSD::BuildDataFieldsResult(MBaseMethod & Mdl)
  {
  Mdl.DD.Page("PartCurve");
  CString Tg;
  for (int i=0; i<Eu.GetSize(); i++)
    {
    Tg.Format("Part%02d", i);
    Mdl.DD.Double((char*)(const char*)Tg, "", &Eu[i], MF_RESULT|MF_NO_FILING, MC_Frac("%"));
    }
  }

//---------------------------------------------------------------------------

double CSeparPlittPSD::CalcDP(double Qvc, double Phi)
  {
  const double Num = 1.88*Pow(Qvc, 1.78)*Exps(0.0055*Phi*100.0);
  const double Den = Pow(m_Dc, 0.37)*Pow(m_Di, 0.94)*Pow(m_h, 0.28)*Pow(m_Du*m_Du + m_Do*m_Do, 0.87);
  return m_Factor3_DP*Num/GTZ(Den);
  }

//---------------------------------------------------------------------------

int CSeparPlittPSD::CalcSeparation(MBaseMethod & Mdl, MStream & QI, MStream & Qof, MStream & Quf)
  {
  //Sanity Checks
  m_NoOfCyclones = Max(m_NoOfCyclones, 0.001);

  MIPSD & PSD = QI.IF<MIPSD>(false);
  MIPSD & PSDof = Qof.IF<MIPSD>(false);
  MIPSD & PSDuf = Quf.IF<MIPSD>(false);

  const long l_SizeCount = PSD.getSizeCount();
  const long l_PSDVectorCount = PSD.getPSDVectorCount();

  Eu.SetSize(l_SizeCount);
  Eu[0] = 0.0;

  // Create some memory for the input and output PSD mass data
  double* M = new double[l_SizeCount];
  double* M_of = new double[l_SizeCount];
  double* M_uf = new double[l_SizeCount];
 
  //Initialisation calcs
  m_Dc= 100.0*m_SIDc;
  m_Do= 100.0*m_SIDo;
  m_Du= 100.0*m_SIDu;
  m_Di= 100.0*m_SIDi;
  m_h=  100.0*m_SIh;
  m_FdLiqVisc = QI.DynamicViscosity(MP_Liq);
  const double QmTot = QI.MassFlow();
  const double Rho = QI.Density();
  double Q = QmTot;
  Q = GTZ(Q)/GTZ(Rho)*1000.0*60.0;     // l/min
  const double RefPress = StdP;        // Reference pressure
  m_Phi = QI.VolFrac(MP_Sol, m_RefTemp, RefPress);
  m_Qvc = Q/m_NoOfCyclones;
  const double RhoS = 0.001*QI.Density(MP_Sol);     // g/cc
  const double RhoL = 0.001*QI.Density(MP_Liq);     // g/cc
  const double C1 = 50.5;
  const double K2 = 0.5;
  double ViscTerm = 1.0;
  if (m_ViscOption==VO_Model)
    ViscTerm = Pow(m_FdLiqVisc*1000.0, 0.5);
  else if (m_ViscOption==VO_User)
    ViscTerm = Pow(m_UserVisc*1000.0, 0.5);
  const double d50Num = C1*Pow(m_Dc, 0.46)*Pow(m_Di, 0.6)*Pow(m_Do, 1.21)*ViscTerm*Exps(0.063*m_Phi*100.0);
  const double d50Den = Pow(m_Du, 0.71)*Pow(m_h, 0.38)*Pow(m_Qvc, 0.45)*Pow((RhoS-RhoL), K2);

  m_d50 = m_Factor1_d50*1.0e-6*d50Num/GTZ(d50Den);
  m_DP = CalcDP(m_Qvc, m_Phi);
  const double SG = Rho/1000.0;
  m_H = m_DP/(9.80707 * SG);
  // const double SNum = 1.9*Pow(m_Du/m_Do, 3.31)*Pow(m_h, 0.54)*Pow(m_Du*m_Du+m_Do*m_Do, 0.36)*Exps(0.0054*m_Phi*100.0);
  const double SNum = 3.29*Pow(m_Du/m_Do, 3.31)*Pow(m_h, 0.54)*Pow(m_Du*m_Du+m_Do*m_Do, 0.36)*Exps(0.0054*m_Phi*100.0)*pow(RhoS*1000, 0.24);
  const double SDen = Pow(m_H, 0.24)*Pow(m_Dc, 1.11);
  m_S = m_Factor4_S*SNum/GTZ(SDen);
  m_Rv = m_S/(m_S+1.0);
  m_m = m_Factor2_Sharp*1.94*Exps((-1.58*m_Rv)*Pow(m_Dc*m_Dc*m_h/Q, 0.15));
  //m_Rf=m_Rv; // Initial Guess
  m_Alpha = 1.54*m_m - 0.47;

  const int MaxPasses = 20;
  for (int Pass=0; Pass<MaxPasses; Pass++)
    {
    double SpcOfSol = 0.0;
    double SpcUfSol = 0.0;

    // Calculate the Split Between Undersize and Oversize for each species
    // that has size data.
    for (int lPSDVec=0; lPSDVec<l_PSDVectorCount; lPSDVec++)
      {
      // Example of getting the species Index in the stream related to this PSD
      // long lSpeciesIndex = getSpecieIndex(lPSDVec);

      PSD.ExtractMassVector(M, lPSDVec);

      // Calculate the PSD split
      for (int lSzIndex=1; lSzIndex<l_SizeCount; lSzIndex++)
        {
        // Get the calculated split for the Size
        const double NominalSize = PSD.getDefn().getGeometricMean(lSzIndex); //use geometrical mean
        //const double NominalSize = PSD.getSize(lSzIndex);
        //Lynch
        const double d = m_Alpha*NominalSize/m_d50;
        double YP;
        if (d>230.0)//limit of exp()
          YP = 1.0;
        else
          {
          const double A = exp(d);
          YP = (A-1.0) / (A+exp(m_Alpha)-2.0);
          }
        Eu[lSzIndex] = YP + m_Rf*(1.0-YP);
        if (Eu[lSzIndex]>1.0)
          Eu[lSzIndex] = 1.0;
        }

      for (int lSzIndex=0; lSzIndex<l_SizeCount; lSzIndex++)
        {
        const double Split = (1.0 - Eu[lSzIndex]); //Get the calculated split for the Size
        const double OF_ = M[lSzIndex]*Split; // The overflow
        M_of[lSzIndex] = OF_;
        SpcOfSol += OF_;
        const double UF_ = M[lSzIndex] - OF_; // The underflow
        M_uf[lSzIndex] = UF_;
        SpcUfSol += UF_;
        }

      // Set the output Masses
      PSDof.ReplaceMassVector(M_of, lPSDVec );
      PSDuf.ReplaceMassVector(M_uf, lPSDVec );
      }

    const double TotSol    = GTZ(SpcOfSol+SpcUfSol); //total solids with PSD data
    const double OfSolFrac = SpcOfSol/TotSol; //average solid fraction to OF
    const double UfSolFrac = SpcUfSol/TotSol; //average solid fraction to UF

    double UFSol = SpcOfSol; //total solids to OF
    double OFSol = SpcUfSol; //total solids to UF

    // Handle solids without size data (use average split)...
    for (int l_SpeciesIndex=0; l_SpeciesIndex<QI.Count(); l_SpeciesIndex++)
      {
      if (QI.IsSolid(l_SpeciesIndex) && QI.HasSizeData(l_SpeciesIndex)==false)
        {// Solid Species has no Size Data
        const double SolSpcM = QI.getM(l_SpeciesIndex);
        const double OFSolSpcM = SolSpcM*OfSolFrac;
        Qof.putM(l_SpeciesIndex, OFSolSpcM);
        OFSol += OFSolSpcM;
        const double UFSolSpcM = SolSpcM-OFSolSpcM;
        Quf.putM(l_SpeciesIndex, UFSolSpcM);
        UFSol += UFSolSpcM;
        }
      }

    //next separate liquids...
    double UFLiq; //UF liquid split fraction
    double OFLiq; //OF liquid split fraction
    m_Rs = OFSol/GTZ(UFSol+OFSol);
    if ((1.0-m_Phi) > 1e-6)
      {
      const double m_RfRqd = (m_Rv-m_Rs*m_Phi)/(1.0-m_Phi);
      m_Rf = Range(0.001, m_RfRqd, 0.999);
      UFLiq = m_Rf/(1.0+m_Rf);
      OFLiq = 1.0-UFLiq;
      OFLiq = Range(0.0, OFLiq*m_Factor5_OFLiq, 1.0);
      UFLiq = 1.0 - OFLiq;
      }
    else
      {
      UFLiq = 0.0;
      OFLiq = 0.0;
      m_Rf = 0.0;
      }

    // Handle liquids...
    for (int l_SpeciesIndex=0; l_SpeciesIndex<QI.Count(); l_SpeciesIndex++)
      {
      if (QI.IsLiquid(l_SpeciesIndex))
        {
        const double LiqSpcM = QI.getM(l_SpeciesIndex);
        const double OFLiqSpcM = LiqSpcM*OFLiq;
        Qof.putM(l_SpeciesIndex, OFLiqSpcM);
        Quf.putM(l_SpeciesIndex, LiqSpcM-OFLiqSpcM);
        }
      }

    bool Converged = (fabs(m_Rf-m_RfMem)<1.0e-6);
    m_RfMem = m_Rf;
    if (Converged)
      break; //exit loop
    }

  // Cleanup
  delete [] M;
  delete [] M_of;
  delete [] M_uf;
  return 0;
  }

//====================================================================================

const int idFeed      = 0;
const int idOF        = 1;
const int idUF        = 2;

static MInOutDefStruct s_IODefs[]=
  {
  //  Desc;             Name;       Id; Rqd; Max; CnId, FracHgt;  Options;
    { "Feed",           "Feed",      idFeed,  1,  10,    0,    1.0f,  MIO_In |MIO_Material|MIO_Transfer},
    { "OverFlow",       "OverFlow",  idOF,    1,   1,    0,    1.0f,  MIO_Out|MIO_Material|MIO_Transfer},
    { "UnderFlow",      "UnderFlow", idUF,    1,   1,    0,    1.0f,  MIO_Out|MIO_Material|MIO_Transfer},
    { NULL },
  };

static double Drw_QALCyclone[] = { 
  MDrw_Poly,  -6.,5., -6.,11., 6.,11., 6.,5., 3.,-11., 3.,-13, -3.,-13., -3.,-11., -6.,5., 6.,5.,
  MDrw_Poly,  -3.,11., -3.,15., 3.,15., 3.,11., -3.,11.,
  MDrw_End };

//---------------------------------------------------------------------------

DEFINE_TRANSFER_UNIT(QALCyclone, "QALCyclone", DLL_GroupName)
void QALCyclone_UnitDef::GetOptions()
  {
  SetDefaultTag("CY");
  SetDrawing("SizeSeparation", Drw_QALCyclone);
  SetTreeDescription("QAL:QAL Cyclone");
  SetDescription("TODO: A description");
  SetModelSolveMode(MSolveMode_Probal|MSolveMode_DynamicFlow);
  SetModelGroup(MGroup_PSD);
  SetModelLicense(/*MLicense_HeatExchange|MLicense_Alumina|*/MLicense_PSD);
  };

//---------------------------------------------------------------------------

QALCyclone::QALCyclone(MUnitDefBase * pUnitDef, TaggedObject * pNd) : MBaseMethod(pUnitDef, pNd)
  {
  //default values...
  dFeedQm = 0.0;
  dProdUfQm = 0.0;
  dProdOfQm = 0.0;
  }

//---------------------------------------------------------------------------

void QALCyclone::Init()
  {
  SetIODefinition(s_IODefs);
  }

//---------------------------------------------------------------------------

void QALCyclone::BuildDataFields()
  {
  //Whiten.BuildDataFieldsConfig(*this);
  Plitt.BuildDataFieldsConfig(*this);
  DD.Double("FeedQm", "", &dFeedQm, MF_RESULT|MF_NO_FILING, MC_Qm);
  DD.Double("UnderflowQm", "", &dProdUfQm, MF_RESULT|MF_NO_FILING, MC_Qm);
  DD.Double("OverflowQm", "", &dProdOfQm, MF_RESULT|MF_NO_FILING, MC_Qm);
  //Whiten.BuildDataFieldsResult(*this);
  Plitt.BuildDataFieldsResult(*this);
  }

//---------------------------------------------------------------------------

void QALCyclone::EvalProducts()
  {
  // Make a copy of our input stream so we do not corrupt it
  MStream QI;
  FlwIOs.AddMixtureIn_Id(QI, idFeed);

  // Get References to our Output Streams using the FlwIOs helper class and IO ids
  MStream & Qof = FlwIOs[FlwIOs.First[idOF]].Stream;
  MStream & Quf = FlwIOs[FlwIOs.First[idUF]].Stream;

  // Summary Mass Flow In
  dFeedQm = QI.MassFlow();

  // Always initialise the outputs as a copy of the inputs.
  // This ensures all "qualities" are copied and initialised (eg PSD, SpecieModel, etc)
  Qof = QI;
  Quf = QI;
  Quf.SetF(Quf, MP_All, 0);

  try
    {
    // Determine if the input stream has size information present
    MIPSD & PSD = QI.IF<MIPSD>(false);
    if (IsNothing(PSD)) {
      Log.Message(MMsg_Warning, "No PSD Properties present... cannot continue");
      return;
    }

    double MassPSD = 0.0;
    for (int s=0; s<QI.Count(); s++)
      {
      if (QI.HasSizeData(s))
        MassPSD += QI.getM(s);
      }

    if (dFeedQm>UsableMass && MassPSD>1.0e-8 && IsNothing(PSD)==false)
      {
      // Input Stream has flow and a PSD
      // which implies that some of the input stream species have associated size data

      //Whiten.CalcSeparation(*this, QI, Qof, Quf);
      Plitt.CalcSeparation(*this, QI, Qof, Quf);
      }
    else
      {
      if (IsNothing(PSD))
        Log.Message(MMsg_Warning, "No PSD Properties present in feed, report to overflow");
      Qof = QI;
      Quf.ZeroMass();
      }

    // Summary Mass Flow Out
    dProdOfQm = Qof.MassFlow();
    dProdUfQm = Quf.MassFlow();
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

//====================================================================================
