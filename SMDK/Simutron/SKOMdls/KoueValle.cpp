//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "kouevalle.h"  
#pragma optimize("", off)
#include "math.h"


//====================================================================================

static MInitialiseTest InitTest("CSKOSMClass");

MSpeciePtr   Water              (InitTest, "H2O(l)",         false);
MSpeciePtr   Silane30           (InitTest, "SiH4-30(g)",     false);
MSpeciePtr   Silane29           (InitTest, "SiH4-29(g)",     false);
MSpeciePtr   Silane28           (InitTest, "SiH4-28(g)",     false); //organic
MSpeciePtr   Silane30l          (InitTest, "SiH4-30(l)",     false);
MSpeciePtr   Silane29l          (InitTest, "SiH4-29(l)",     false);
MSpeciePtr   Silane28l          (InitTest, "SiH4-28(l)",     false); //organic
MSpeciePtr   Oxygen             (InitTest, "O2(g)",          false); //organic
MSpeciePtr   Nitrogen           (InitTest, "N2(g)",          false);
MSpeciePtr   Hydrogen           (InitTest, "H2(g)",          false);
MSpeciePtr   Air                (InitTest, "Air(g)",         false);
MSpeciePtr   Steam              (InitTest, "H2O(g)",         false);


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

const long idGasIn  = 0;
const long idGasOut = 1;
const long idLiqOut = 2;
const long idCoolIn = 3;
const long idCoolOut = 4; 

static MInOutDefStruct s_IODefs[]=
  {
  //NOTE: Different CnId for tube side and shell side flows (material does not mix!)
  //  Desc;             Name;       PortId;   Rqd; Max; CnId, FracHgt;  Options;
    { "Gas In",       "GasIn",      idGasIn,    1,   1,    MCN_Join|0,    1.0f,  MIO_In |MIO_Material }, 
    { "Gas Out",      "GasOut",     idGasOut,   1,   1,    MCN_Join|0,    1.0f,  MIO_Out|MIO_Material },
    { "Liquid Out",   "LiqOut",     idLiqOut,   1,   1,    MCN_Join|0,    1.0f,  MIO_Out|MIO_Material },
    { "Coolant In",   "CoolI",      idCoolIn,   1,   1,    MCN_Join|1,    1.0f,  MIO_In |MIO_Material },
    { "Coolant Out",  "CoolO",      idCoolOut,  1,   1,    MCN_Join|1,    1.0f,  MIO_Out|MIO_Material },
    { NULL },
  };

static double Drw_CKoueValle[] = 
  { 
  MDrw_Poly, -5,-9, 5,-9, 5,9, -5,9, -5,-9, 
  MDrw_Poly, -5,9, -5,10, 5,10, 5,9,
  MDrw_Poly, -5,-9, -5,-10, 5,-10, 5,-9,
  MDrw_Arc,  5, 8, 1, 0, 90,
  MDrw_Arc, -5, 8, 1, 90, 180,
  MDrw_Arc, -5,-8, 1, 180, 270,
  MDrw_Arc,  5,-8, 1, 270, 360,
  MDrw_Poly, -6,-8, -6, 8,
  MDrw_Poly,  6,-8,  6, 8,
  MDrw_Poly,  -4,-9,  -4, 9,
  MDrw_Poly,  -3,-9,  -3, 9,
  MDrw_Poly,  -2,-9,  -2, 9,
  MDrw_Poly,  -1,-9,  -1, 9,
  MDrw_Poly,   0,-9,   0, 9,
  MDrw_Poly,   1,-9,   1, 9,
  MDrw_Poly,   2,-9,   2, 9,
  MDrw_Poly,   3,-9,   3, 9,
  MDrw_Poly,   4,-9,   4, 9,

  //MDrw_Poly, -5,1, -6,1, -6,0, -5,0, -5,1,
  //MDrw_Poly, 5,-1, 6,-1, 6,0, 5,0, 5,-1,
  MDrw_End
  };

//---------------------------------------------------------------------------
DEFINE_TRANSFER_UNIT(CKoueValle, "KoueValle", DLL_GroupName)

void CKoueValle_UnitDef::GetOptions()
  {
  SetDefaultTag("KV");
  SetDrawing("KoueValle", Drw_CKoueValle);
  SetTreeDescription("SKO:N2 Condensing cooling Carrier");
  SetDescription("TODO: A description");
  SetModelSolveMode(MSolveMode_DynamicFull);
  SetModelGroup(MGroup_Energy);
  SetModelLicense(MLicense_User);
  };

//---------------------------------------------------------------------------

CKoueValle::CKoueValle(MUnitDefBase * pUnitDef, TaggedObject * pNd) : \
MBaseMethod(pUnitDef, pNd)//,
//m_VLE(this, VLEF_QPFlash, "VLE"),
//m_FTC(this)
  {
  //default values...
  /*m_HTC           = 91.9;
  m_Area          = 0.5;*/
  m_SegCount      = 30;
  m_tubeno		= 43;

  //double          m_LMTD;
  m_Duty=0;
  m_Damping       = 0.8;
  m_Tolerance     = 0.01;
  m_ToleranceN2T  = 0.01;
  m_TN2OutEst     = dNAN;

  }

//---------------------------------------------------------------------------

CKoueValle::~CKoueValle()
  {
  }

//---------------------------------------------------------------------------

void CKoueValle::Init()
  {
  SetIODefinition(s_IODefs);
  //RegisterFlashTrainNode(idCoolIn, true, m_FTC.TypeInfo()); //register as potential part of flashtrain and which connection
  }

//---------------------------------------------------------------------------
const int idDX_UA = 1;

void CKoueValle::BuildDataFields()
  {
  DD.Text(""); 
  DD.Text("Configuration");
  //DD.CheckBox("On",          "", DC_,     "",         &m_fOn,            this, isParm|SetOnChange);
  /*DD.Double("HTC",       "",     &m_HTC,           MF_PARAMETER, MC_HTC("kW/m^2.K"));
  DD.Double("TubeArea",  "",     &m_Area,          MF_PARAMETER, MC_Area("m^2"));*/
  DD.Double("TubeNo",  "",     &m_tubeno,		MF_PARAMETER, MC_Area(""));


  DD.Text("");
  DD.Text("Results..");
  DD.Double("U",       "",     &m_HTC,           MF_RESULT,  MC_HTC("kW/m^2.K"));
  DD.Double("A",       "",     &m_Area,           MF_RESULT,  MC_Area ("m^2"));


  DD.Double("U*A",       "",     idDX_UA,           MF_RESULT|MF_NO_FILING, MC_UA("kW/K"));
  //DD.Double("LMTD",      "",     &m_dLMTD,          MF_RESULT, MC_dT("C"));
  //DD.Double("Duty",      "",     &m_Duty,          MF_RESULT, MC_Pwr("kW"));
  DD.Double("Duty",      "",     &m_Duty,          MF_RESULT, MC_Pwr("kW"));

  DD.Text("");

  //m_FTC.BuildDataFields();
  }

//---------------------------------------------------------------------------

bool CKoueValle::ExchangeDataFields()
  {
  switch (DX.Handle)
    {
    case idDX_UA: 
      //do not need to set anything this is simply a calculated result
      DX.Double=m_HTC*m_Area;
      return true;
    }

  //if (m_FTC.ExchangeDataFields())
  //  return 1;
  return false;
  }

//---------------------------------------------------------------------------

bool CKoueValle::EvalJoinPressures()
  {
  if (IsNetProbal)
    {//set pressures at each join (pipes connected to unit)
    return MBaseMethod::EvalJoinPressures();
    //for (int j=0; j<Joins.Count; j++) 
    //  {
    //  Joins[j].SetProbalP(dPout, false, true); //only set pressure at outlet
    //  }
    //return true;
    }
  else
    { 
    //INCOMPLETECODE(); 
    return MBaseMethod::EvalJoinPressures();
    }
  return false;
  }

//---------------------------------------------------------------------------
//===========================================================================
//---------------------------------------------------------------------------

class CST_CondensateFinder : public MRootFinder
  {
  public:
    CST_CondensateFinder(CKoueValle * pUnit, MStream &TubeI, MStream &ShellI, MStream &TubeO, MStream &ShellO);
    ~CST_CondensateFinder();
    double Function(double x);

  protected:
    MStream & m_TubeI;
    MStream & m_ShellI;
    MStream & m_TubeO;
    MStream & m_ShellO;

    CKoueValle &m_Unit;
    MVLEBlk      &m_VLE;

    static MToleranceBlock s_Tol;
  };

MToleranceBlock CST_CondensateFinder::s_Tol(TBF_Both, "DemoShellTube:CondensFinder", 0.0, 1.0e-12);

//---------------------------------------------------------------------------

//CST_CondensateFinder::CST_CondensateFinder(CKoueValle * pUnit, MStream &TubeI, MStream &ShellI, MStream &TubeO, MStream &ShellO) : 
//MRootFinder("A Desc" ,s_Tol),
//m_Unit(*pUnit), 
//m_TubeI(TubeI),
//m_ShellI(ShellI),
//m_TubeO(TubeO),
//m_ShellO(ShellO),
//m_VLE(pUnit->m_VLE)
//  {
//  m_TubeO.SuppressErrors=true;
//  m_ShellO.SuppressErrors=true;
//  };
//
////---------------------------------------------------------------------------
//
//CST_CondensateFinder::~CST_CondensateFinder()
//  {
//  m_TubeO.SuppressErrors=false;
//  m_ShellO.SuppressErrors=false;
//  };
//
////---------------------------------------------------------------------------
//
//double CST_CondensateFinder::Function(double Amount)
//  {
//  m_ShellO.SetM(m_ShellI, MP_All, Amount, m_ShellI.P);
//  double ShtotHf=m_ShellO.totHf();
//
//  double FTemp=m_ShellI.SaturationT();
//  double FPress=m_ShellI.SaturationP();
//  m_VLE.SetFlashVapFrac(m_ShellO, FTemp, FPress, 0.0, VLEF_Null);
//  m_ShellO.SetTP(FTemp, FPress);
//  m_Unit.m_dDuty=-(m_ShellO.totHf()-ShtotHf);
//
//  m_TubeO.Set_totHf(m_TubeI.totHf()+m_Unit.m_dDuty);
//
//  double TDuty;
//  const double UA = m_Unit.m_dHTC*m_Unit.m_dArea;
//  if (UA>1.0e-6)
//    {
//    m_Unit.m_dLMTD = LMTDSaturation(m_TubeI.T, m_TubeO.T, m_ShellO.T);
//    TDuty = UA * m_Unit.m_dLMTD;
//    }
//  else
//    {
//    m_Unit.m_dLMTD = 0.0;
//    TDuty = 0.0;
//    } 
//
//  double Diff = m_Unit.m_dDuty-TDuty;
//  return Diff;
//  };

//---------------------------------------------------------------------------

void CKoueValle::DoCondensingHeater(MStream & GasI, MStream & CoolI, MStream & GasO, MStream & LiqO, MStream & CoolO)
  {

  double A, NTU;
  double CpN2, CpGas, WN2, WGasIn, Ccold, Chot, MGasIn, molfrSiInGas, molGasIn;
  double Cmin, Cmax, Cr, WLiqIn;
  double TGasIn,	TGasOut, TN2In, TN2Out;
  double E, qmax, q, qsensible; 
  double Tsat, PGasIn, Ppartial, lamda, Wcondensed, WSiIn;
  double CpLiq,qsubcool, Tliq;
  double massfrSiIn, massfrSiOut, molfrSiOutGas, WSiOut; 
  double PGasOut, WLiqOut, WGasOut;
  double error, TN2error;
  double WH2, WSiGasOut, MGasOut;
  double TGasInGiven, TN2InGiven, qtotal, TN2OutTotal, q_inc, Uave;
  double Nu_gas,Nu_N2,C_gas, Re_gas, Re_liq, Re_N2; 
  double Pr_gas, Pr_liq, Pr_N2, rho_gas, rho_liq, rho_N2, u_gas, u_liq, u_N2;
  double de, mu_gas, mu_liq, mu_N2, k_gas, k_liq, k_N2, kw;
  double h_gas, h_liq, hi, tubelength, di, d0, Di;
  double h0, h0d,hid,Tw, Ashell, jh;
  double pt, parameter, bracket;
  double myexp, Uerror, Utotal, Utolerance = 0.00001;
  double h_liqcheck, parameter1, CondFrac;
  // Choose number of increments: m_SegCount increments imply that A = m_Area/m_SegCount

  di = 0.0045; // in meter
  d0 = 0.00635;
  Di = 0.026;
  tubelength = 0.7; 
  m_HTC = 0.05;
  m_Area = PI*d0*tubelength*m_tubeno; 

  Uerror = 1;

  while (Uerror > Utolerance)
    {	

    TGasInGiven = GasI.T;
    TN2InGiven = CoolI.T;
    TGasIn = TGasInGiven;  //K
    TN2In = TN2InGiven;
    WLiqIn = GasI.MassFlow(MP_SL);; 
    WGasIn = GasI.MassFlow(MP_Gas); 
    MGasIn = GasI.MoleWt();
    PGasIn = GasI.P;;
    A = m_Area;
    WN2 = CoolI.MassFlow(); 
    CpN2 = 1046; //J/kg.K
    CpLiq = 1900; // in J/kgK vir silaan

    molfrSiInGas = (GasI.Ml[Silane28]+GasI.Ml[Silane29]+GasI.Ml[Silane30])/GasI.Moles(); //      (MGasIn - 2)/30;
    WSiIn = GasI.M[Silane28]+GasI.M[Silane29]+GasI.M[Silane30]; //kg/h pure silane
    massfrSiIn = (GasI.M[Silane28]+GasI.M[Silane29]+GasI.M[Silane30])/GasI.Mass(); //      (MGasIn - 2)/30;
    CpGas = massfrSiIn*(1.8347*TGasIn + 828.79) + (1-massfrSiIn)*(13.859+0.001696*TGasIn)*1000; //J/kgK silaan
    CpGas = GTZ(CpGas);
    Chot = GTZ(WGasIn*CpGas);
    Ccold = GTZ(WN2*CpN2);

    if (Ccold <= Chot)
      {
      Cmin = Ccold;
      Cmax = Chot;
      }
    else 
      {	
      Cmin = Chot;
      Cmax = Ccold;
      }
    Cr = Cmin / Cmax;
    NTU = m_HTC*1000*A/Cmin;
    E = (1 - exp(-1*NTU*(1-Cr)))/(1 - Cr*exp(-1*NTU*(1-Cr)));
    qmax = Cmin*(TGasIn - TN2In); 
    q = E*qmax;
    TN2OutTotal = q/WN2/CpN2 + TN2InGiven;
    qtotal = q;

    m_TN2OutEst = TN2OutTotal;


    if (molfrSiInGas > 0)
      {
      Ppartial = Range(0.000001,molfrSiInGas*PGasIn,PGasIn);	
      Tsat = 62.5213382197203*pow((Ppartial*1000),0.0796290365852);
      lamda =  -5.3536246*pow(Tsat,2) + 453.1153095*Tsat + 456194.636731;
      qsensible = WGasIn*CpGas*(TGasIn - Tsat); 

      //Loop to calculate whether condensation takes place at all

      if (abs(qsensible) >= abs(q))
        {	
        TGasOut = (q - WLiqIn*CpLiq*Tliq - WGasIn*CpGas*TGasIn)/(-WLiqIn*CpLiq - WGasIn*CpGas);
        Tliq = TGasOut;				
        Wcondensed = 0;

        }
      else 
        {
        TGasOut = Tsat;
        Tliq = TGasOut;
        Wcondensed = (q - qsensible)/lamda;
        Dbg.PrintLn("Condensed %10.4f", Wcondensed);
        }

      //Loop to calculate whether subcooling takes place

      if (Wcondensed > WSiIn) 
        {
        Wcondensed = WSiIn;
        qsubcool = q - (qsensible + Wcondensed*lamda);
        Tliq = (qsubcool - Wcondensed*CpLiq*Tsat - (WGasIn - Wcondensed)*CpGas*Tsat)/(-Wcondensed*CpLiq - (WGasIn - Wcondensed)*CpGas);
        TGasOut = Tliq;
        Dbg.PrintLn("SubCooled %10.4f", Wcondensed);
        }

      }
    else
      {
      Tliq = (q-WLiqIn*CpLiq*Tliq - WGasIn*CpGas*TGasIn)/(-WLiqIn*CpLiq-WGasIn*CpGas);
      TGasOut = Tliq;
      Wcondensed = 0;

      }


    WH2 = WGasIn - WSiIn;  //kg/h 
    WSiGasOut = WSiIn - Wcondensed;
    PGasOut = PGasIn;
    WLiqOut = WLiqIn + Wcondensed;
    WGasOut = WH2 + WSiGasOut;
    CondFrac =  Wcondensed/GTZ(WSiIn);

    LiqO.M[Silane28l]+=GasO.M[Silane28]*CondFrac;
    LiqO.M[Silane29l]+=GasO.M[Silane29]*CondFrac;
    LiqO.M[Silane30l]+=GasO.M[Silane30]*CondFrac;
    GasO.M[Silane28]=GasO.M[Silane28]*(1.0-CondFrac);
    GasO.M[Silane29]=GasO.M[Silane29]*(1.0-CondFrac);
    GasO.M[Silane30]=GasO.M[Silane30]*(1.0-CondFrac);

    h0d = 10000;
    hid = 10000;

    rho_gas = GasI.Density(MP_Gas,(GTZ(TGasIn)),PGasIn);
    rho_liq = -0.0036*pow(Tliq,2) - 0.757*Tliq + 797.7;
    rho_N2 = CoolI.Density(MP_Gas,(GTZ(TN2In)),CoolI.P);
    mu_gas = GasI.DynamicViscosity(MP_Gas,(GTZ(TGasIn)),PGasIn);
    mu_liq = 0.0026*exp(-0.0177*Tliq);
    mu_N2 = CoolI.DynamicViscosity(MP_Gas,(GTZ(TN2In)),CoolI.P);

    Tw = GTZ(TGasOut);

    myexp = -1.4087 + 1.3982*log10(Tw)+ 0.2543*pow((log10(Tw)),2) + -0.6260*pow((log10(Tw)),3) + 0.2334*pow((log10(Tw)),4) + 0.4256*pow((log10(Tw)),5) + -0.4658*pow((log10(Tw)),6) + 0.1650*pow((log10(Tw)),7) + -0.0199*pow((log10(Tw)),8);
    kw = GTZ(pow(10.0,myexp));  // kry van Internet:  vergelyking vir stainless 316 se k

    //tubeside
    de = di;
    //k_gas =0.2; // kry later beter waarde

    //==============================================================================
    //better k_gas calc
    //==============================================================================
    double Tb1, Tb2, S1, S2, S11, S21, S12, S22,musilane, muH2, k_H2, k_silane;
    double A11, A12, A21, A22, k_lowpressure, A, B, C, Vc, V_specific, Pc, Tc, Zc, rho_red;
    double phi1, phi2, Tpc, Ppc;

    Tb1 = 20.45;
    Tb2 = 161;
    S1 = 1.5*Tb1;
    S2 = 1.5*Tb2;
    S11 = S1;
    S12 = pow(S1*S2, 0.5);
    S21 = pow(S2*S1,0.5);
    S22 = S2;

    musilane = (3.5117*(TGasIn - 273.15) + 1065.1)*pow(10.0, -8.0);
    muH2 =(2.0008*(TGasIn - 273.15) + 841.14)*pow(10.0,-8.0);
    k_H2 = 0.0004*TGasIn + 0.0655;
    k_silane =0.0001*TGasIn - 0.0102;

    A12 = 0.25*pow(1+pow(muH2/musilane*pow(16.0,0.75)*(TGasIn + S1)/(TGasIn + S2),0.5),2)*(TGasIn+S12)/(TGasIn+S1);
    A11 = 1;
    A22 = 1;
    A21 = 0.25*pow(1+pow(musilane/muH2*pow((1/16.0),0.75)*(TGasIn + S2)/(TGasIn + S1),0.5),2)*(TGasIn+S21)/(TGasIn+S2);
    k_lowpressure = (1-molfrSiInGas)*k_H2/((1-molfrSiInGas)*A11+molfrSiInGas*A12)+molfrSiInGas*k_silane/((1-molfrSiInGas)*A21+molfrSiInGas*A22);

    phi1 = (1-molfrSiInGas)*pow(0.06145,2/3)/((1-molfrSiInGas)*pow(0.06415,2/3)+molfrSiInGas*pow(0.1327,2/3));
    phi2 = molfrSiInGas*pow(0.1327,2/3)/((1-molfrSiInGas)*pow(0.06415,2/3)+molfrSiInGas*pow(0.1327,2/3));
    Vc = phi1*0.06415+phi2*0.1327+2*phi1*phi2*(-0.03498);   //m3/kmol of l/mol
    V_specific= MGasIn/rho_gas;
    rho_red =Vc/V_specific; 

    Tc = (molfrSiInGas*0.1327*269.7+(1-molfrSiInGas)*0.06415*33.19)/(molfrSiInGas*0.1327+(1-molfrSiInGas)*0.06415);  //K

    Tpc = molfrSiInGas*269.7+(1-molfrSiInGas)*33.19;
    Ppc = molfrSiInGas*4.843+(1-molfrSiInGas)*1.315;
    Pc = Ppc+Ppc*(5.808+4.93*((1-molfrSiInGas)*(-0.214)+molfrSiInGas*0.0222))*((Tc-Tpc)/Tpc);  //MPa
    Zc = PGasIn*MGasIn/rho_gas/8.314/TGasIn;

    if (rho_red < 0.5)
      {
      A = 0.0002702;
      B = 0.535;
      C = -1.0;
      }
    else if (rho_red < 2.0)
      {
      A = 0.0002528;
      B = 0.67;
      C = -1.069;
      }
    else 
      {
      A = 0.0000574;
      B = 1.155;
      C = 2.016;
      }
    k_gas = k_lowpressure +A*(exp(B*rho_red)+C)/(pow(Tc,1/6)*pow(MGasIn, 0.5)/pow(Pc,2/3)*pow(Zc,5));
    //==============================================================================
    //better k_gas calc end
    //==============================================================================


    u_gas = (WGasIn/m_tubeno)/rho_gas*4/PI/pow(de,2);
    Re_gas = rho_gas*u_gas*de/mu_gas;
    Pr_gas = CpGas*mu_gas/k_gas;
    C_gas = 0.021;
    if (Re_gas > 2000)
      Nu_gas = C_gas*pow(Re_gas,0.8)*pow(Pr_gas,0.33);
    else 
      Nu_gas =1.86*pow(Re_gas*Pr_gas,0.33)*pow(de/tubelength,0.33);

    h_gas = GTZ(Nu_gas*k_gas/di);
    hi = h_gas; 

    if (WLiqOut > 0)
      {
      // doen vir vloeistof
      k_liq = -0.0007901941563*Tliq + 0.2588834861487;  // W/mK
      u_liq = (WLiqOut/m_tubeno)/rho_liq *4/PI/pow(de,2);
      Re_liq  = rho_liq *u_liq *de/mu_liq ;
      Pr_liq  = CpLiq*mu_liq /k_liq ;

      if (Re_liq < 2000)
        parameter = 1.5405*pow(Re_liq,-0.3457);
      else
        parameter = 1;
      bracket = pow(mu_liq,2)/(rho_liq*(rho_liq - rho_gas)*9.81);
      h_liq = GTZ(parameter*k_liq/(pow(bracket,0.333)));
      parameter1 = 0.333*log(bracket);
      h_liqcheck = GTZ(parameter*k_liq/(exp(parameter1)));

      hi = GTZ(1/(1/h_liq +qsensible/q/h_gas ));
      }

    //shellside 

    k_N2 = 0.024;	
    pt = 1.25*d0;
    Ashell = (pt - d0)*Di*0.14/pt;
    de = 1.1/d0*(pow(pt,2) - 0.917*pow(d0,2));
    u_N2 = WN2/rho_N2/Ashell;
    Re_N2 = rho_N2*u_N2*de/mu_N2;
    Pr_N2 = CpN2*mu_N2/k_N2;
    if (Re_N2 <= 100000)
      jh =  0.3831*pow(Re_N2,-0.4798);
    else
      jh = 33.279*pow(Re_N2,-0.4685);
    Nu_N2 = jh*Re_N2*pow(Pr_N2,(1/3));
    h0 = GTZ(Nu_N2*k_N2/di);

    Utotal = (1/(1/h0 +1/h0d + d0*log(d0/di)/2/kw +d0/di/hid + d0/di/hi))/1000;

    Uerror = abs(m_HTC - Utotal);
    m_HTC = Utotal;
    }

  for (int j = 1; j<=100; j++)
    {

    GasO  = GasI;      //copy Gas input to Gas output
    LiqO.ZeroMass();  //Zero Liquid output
    CoolO = CoolI;     //copy Coolant input to output
    Uave = 0;
    q_inc = 0;
    A = m_Area/m_SegCount;
    TN2Out = m_TN2OutEst;
    TGasIn = TGasInGiven;  //K
    WLiqIn = GasI.MassFlow(MP_SL);//0; 
    WGasIn = GasI.MassFlow(MP_Gas);// 0.75;  //kg/s
    MGasIn = GasI.MoleWt();//4.5;
    PGasIn = GasI.P;//. 300;  //kPa
    molGasIn = WGasIn/MGasIn;  //kmol/h

    molfrSiInGas = (GasI.Ml[Silane28]+GasI.Ml[Silane29]+GasI.Ml[Silane30])/GasI.Moles(); //      (MGasIn - 2)/30;
    WSiIn = GasI.M[Silane28]+GasI.M[Silane29]+GasI.M[Silane30]; //kg/h pure silane
    massfrSiIn = (GasI.M[Silane28]+GasI.M[Silane29]+GasI.M[Silane30])/GasI.Mass(); //      (MGasIn - 2)/30;

    CpGas = massfrSiIn*(1.82347*TGasIn + 828.79) + (1-massfrSiIn)*(13.859+0.001696*TGasIn)*1000; //J/kgK  Verander na integraal waarde Cp = f(T)
    Chot = GTZ(WGasIn*CpGas);
    Ccold = GTZ(WN2*CpN2);

    Dbg.PrintLn("                   %2s %10s %10s %10s %10s %10s %10s %10s %10s %10s Iter = %i", 
      "", "WGasIn", "WLiqIn", "TGasIn", "WN2", "TN2Out", "Ccold", "Chot", "CpGas", "massfrSiIn", j);


    for (int k = 1; k<=m_SegCount; k++ ) 
      {	
      Dbg.PrintLn("                   %2i %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f", 
        k, WGasIn, WLiqIn, TGasIn, WN2, TN2Out, Ccold, Chot, CpGas, massfrSiIn);

      if (Ccold <= Chot)
        {
        Cmin = Ccold;
        Cmax = Chot;
        }
      else 
        {	
        Cmin = Chot;
        Cmax = Ccold;
        }
      Cr = Cmin / Cmax;

      NTU = m_HTC*1000*A/Cmin;
      E = (1 - exp(-1*NTU*(1-Cr)))/(1 - Cr*exp(-1*NTU*(1-Cr)));

      //Guess TN2 in
      double TN2prev, TN2prev2, f, fprev, fprev2;
      bool nosegments = false;
      int iEscape=1000;
      error = 1;
      while (error > m_Tolerance && iEscape-->0)
        {

        if (iEscape==999)
          {

          q = qtotal/m_SegCount;
          TN2In = TN2Out - q/WN2/CpN2;
          qmax = Cmin*(TGasIn - TN2In); 
          q = Range(0.0, E*qmax, qmax);
          fprev2 = TN2Out - (q/Ccold + TN2In);
          TN2prev2 = TN2In;
          }

        if (iEscape==998)
          {

          TN2In = TN2prev2 + fprev2;
          qmax = Cmin*(TGasIn - TN2In); 
          q = Range(0.0, E*qmax, qmax);
          fprev = TN2Out - (q/Ccold + TN2In);
          TN2prev = TN2In;	

          }

        if (iEscape<=997)
          {
          TN2In = TN2prev - fprev*(TN2prev - TN2prev2)/(fprev - fprev2);
          //TN2In = Range(0.0 , TN2In, TN2Out);
          qmax = Cmin*(TGasIn - TN2In); 
          q = Range(0.0, E*qmax, qmax);
          f = TN2Out - (q/Ccold + TN2In);
          TN2prev2 = TN2prev;
          fprev2 = fprev;
          TN2prev = TN2In;
          fprev = f;
          error = abs(TN2Out - (q/Ccold + TN2In));
          }

        }

      if (j ==100) 

        {
        TN2In = TN2InGiven;
        m_TN2OutEst = TN2OutTotal;
        q = qtotal;
        nosegments = true;
        }

      if (molfrSiInGas > 0)
        {
        Ppartial = Range(0.000001,molfrSiInGas*PGasIn,PGasIn);	
        Tsat = 62.5213382197203*pow((Ppartial*1000),0.0796290365852);
        lamda =  -5.3536246*pow(Tsat,2) + 453.1153095*Tsat + 456194.636731;
        qsensible = WLiqIn*CpLiq*(Tliq - Tsat) + WGasIn*CpGas*(TGasIn - Tsat) ;

        //Loop to calculate whether condensation takes place at all


        if (abs(qsensible) >= abs(q))
          {	

          TGasOut = (q - WLiqIn*CpLiq*Tliq - WGasIn*CpGas*TGasIn)/(-WLiqIn*CpLiq - WGasIn*CpGas);
          Tliq = TGasOut;
          Wcondensed = 0;
          // Dbg.PrintLn("Condensed Zero");
          }
        else 
          {
          TGasOut = Tsat;
          Tliq = TGasOut;
          Wcondensed = (q - qsensible)/lamda;
          Dbg.PrintLn("Condensed %10.4f", Wcondensed);
          }

        //Loop to calculate whether subcooling takes place

        if (Wcondensed > WSiIn)
          {
          Wcondensed = WSiIn;
          qsubcool = q - (qsensible + Wcondensed*lamda);
          Tliq = (qsubcool - Wcondensed*CpLiq*Tsat - (WGasIn - Wcondensed)*CpGas*Tsat)/(-Wcondensed*CpLiq - (WGasIn - Wcondensed)*CpGas);
          TGasOut = Tliq;
          Dbg.PrintLn("SubCooled %10.4f", Wcondensed);
          }

        }
      else
        {
        Tliq = (q-WLiqIn*CpLiq*Tliq - WGasIn*CpGas*TGasIn)/(-WLiqIn*CpLiq-WGasIn*CpGas);
        TGasOut = Tliq;
        Wcondensed = 0;

        }

      WH2 = WGasIn - WSiIn;  //kg/h 
      WSiGasOut = WSiIn - Wcondensed;
      PGasOut = PGasIn;
      WLiqOut = WLiqIn + Wcondensed;
      WGasOut = WH2 + WSiGasOut;
      CondFrac =  Wcondensed/GTZ(WSiIn);

      LiqO.M[Silane28l]+=GasO.M[Silane28]*CondFrac;
      LiqO.M[Silane29l]+=GasO.M[Silane29]*CondFrac;
      LiqO.M[Silane30l]+=GasO.M[Silane30]*CondFrac;
      GasO.M[Silane28]=GasO.M[Silane28]*(1.0-CondFrac);
      GasO.M[Silane29]=GasO.M[Silane29]*(1.0-CondFrac);
      GasO.M[Silane30]=GasO.M[Silane30]*(1.0-CondFrac);

      MGasOut = GasO.MoleWt();
      molfrSiOutGas = (GasO.Ml[Silane28]+GasO.Ml[Silane29]+GasO.Ml[Silane30])/GasO.Moles(); //      (MGasIn - 2)/30;
      WSiOut= GasO.M[Silane28]+GasO.M[Silane29]+GasO.M[Silane30]; //kg/h pure silane
      massfrSiOut = (GasO.M[Silane28]+GasO.M[Silane29]+GasO.M[Silane30])/GasO.Mass(); //      (MGasIn - 2)/30;


      //============== sit roetine in om U te bereken=====================================
      //if ((TGasOut < 50) || (TN2In < 50))
      //	m_HTC = Utotal;
      //else
      //{
      //								
      //rho_gas = GasI.Density(MP_Gas,(GTZ(TGasIn)),PGasIn);
      //rho_liq = LiqO.Density(MP_Liq,(GTZ(TGasOut)),PGasIn);
      //rho_N2 = CoolI.Density(MP_Gas,(GTZ(TN2In)),CoolI.P);
      //mu_gas = GasI.DynamicViscosity(MP_Gas,(GTZ(TGasIn)),PGasIn);
      //mu_liq = LiqO.DynamicViscosity(MP_Liq,(GTZ(TGasOut)),PGasIn);
      //mu_N2 = CoolI.DynamicViscosity(MP_Gas,(GTZ(TN2In)),CoolI.P);

      //Tw = TGasOut;
      //
      //if (TGasOut < 0)
      //Tw = 90;

      //double myexp;
      //myexp = -1.4087 + 1.3982*log10(Tw)+ 0.2543*pow((log10(Tw)),2) + -0.6260*pow((log10(Tw)),3) + 0.2334*pow((log10(Tw)),4) + 0.4256*pow((log10(Tw)),5) + -0.4658*pow((log10(Tw)),6) + 0.1650*pow((log10(Tw)),7) + -0.0199*pow((log10(Tw)),8);
      //		
      //kw = GTZ(pow(10.0,myexp));  // kry van Internet:  vergelyking vir stainless 316 se k

      //di = 0.0045; // in meter
      //d0 = 0.006;
      //Di = 0.026;

      //m_tubeno = 43;
      //tubelength = 0.7; 
      //
      ////tubeside
      //de = di;
      //k_gas = 0.0004*TGasIn + 0.0655; // kry later beter waarde
      //u_gas = (WGasIn/m_tubeno)/rho_gas*4/PI/pow(de,2);
      //Re_gas = rho_gas*u_gas*de/mu_gas;
      //Pr_gas = CpGas*mu_gas/k_gas;
      //C_gas = 0.021;
      //if (Re_gas > 2000)
      //	Nu_gas = C_gas*pow(Re_gas,0.8)*pow(Pr_gas,0.33);
      //else 
      //	Nu_gas =1.86*pow(Re_gas*Pr_gas,0.33)*pow(de/(tubelength/m_SegCount),0.33);

      //h_gas = GTZ(Nu_gas*k_gas/di);
      //hi = h_gas;

      //if (WLiqOut > 0)
      //{ 
      //	// doen vir vloeistof
      //	k_liq = -0.0007901941563*GTZ(Tliq) + 0.2588834861487;  // W/mK
      //	//k_liq = 0.0828; // kry later beter waarde
      //	u_liq = (WLiqOut/m_tubeno)/rho_liq *4/PI/pow(de,2);
      //	Re_liq  = rho_liq *u_liq *de/mu_liq ;
      //	Pr_liq  = CpLiq*mu_liq /k_liq ;
      //	/*C_liq = 0.023;
      //	if (Re_liq > 2000)
      //		Nu_liq  = C_liq *pow(Re_liq ,0.8)*pow(Pr_liq ,0.33);
      //	else Nu_liq  =1.86*pow(Re_liq *Pr_liq ,0.33)*pow(de/tubelength,0.33);
      //	h_liq  = GTZ(Nu_liq *k_liq /di);*/

      //	if (Re_liq <= 2000)
      //		parameter = 1.5405*pow(Re_liq,-0.3457);
      //	else
      //	parameter = 1;
      //	bracket = pow(mu_liq,2)/(rho_liq*(rho_liq - rho_gas)*9.81);
      //	h_liq = GTZ(parameter*k_liq/(pow(bracket,0.333)));

      //	hi = GTZ(1/(1/h_liq +qsensible/q/h_gas ));
      //}

      ////shellside 

      //k_N2 = 0.024;
      //Ashell = (pt - d0)*Di*0.14/pt;
      //de = 1.1/d0*(pow(pt,2) - 0.917*pow(d0,2));
      //u_N2 = WN2/rho_N2/Ashell;
      //Re_N2 = rho_N2*u_N2*de/mu_N2;
      //Pr_N2 = CpN2*mu_N2/k_N2;
      ////kry funksie jh vir baffle cut 45 %
      //if (Re_N2 <= 100000)
      //jh =  0.3831*pow(Re_N2,-0.4798);
      //else
      //jh = 33.279*pow(Re_N2,-0.4685);
      //Nu_N2 = jh*Re_N2*pow(Pr_N2,(1/3));
      //h0 = GTZ(Nu_N2*k_N2/di);

      //U = 1/(1/h0 +1/h0d + d0*log(d0/di)/2/kw +d0/di/hid + d0/di/hi);
      //m_HTC = U/1000;
      //Uave += U;
      //                     
      //}

      //====================einde van U roetine=====================================================


      if (nosegments == true)
        // take values as calculated for HX as a whole (without segments)
        break;

      // Allocate endpoint values of previous iteration to starting points for next iteration
      TGasIn = TGasOut;  //K
      TN2Out = TN2In;
      WLiqIn = WLiqOut; 
      WGasIn = WGasOut;  //kg/h
      MGasIn = MGasOut;
      PGasIn = PGasOut;  //kPa
      molfrSiInGas = molfrSiOutGas;

      WSiIn = WSiOut;
      massfrSiIn =massfrSiOut;
      q_inc += q;


      CpGas = massfrSiIn*(1.82347*TGasIn + 828.79) + (1-massfrSiIn)*(13.859+0.001696*TGasIn)*1000; //J/kgK  Verander na integraal waarde Cp = f(T)
      Ccold = WN2*CpN2;
      Chot = WGasIn*CpGas;


      }
    //Look at difference between TN2In calculated for bottom of exchanger, and TN2InGiven.  
    // Gues better Tn2out for next loop

    Dbg.PrintLn("                   %2s %10.4f %10.4f %10.4f %10.4f %10.4f", "==", WGasIn, WLiqIn, TGasIn, WN2, TN2Out);

    double TN2OutPrev, TN2OutPrev2, f, fprev, fprev2;
    //If Tn2In not equal to TN2InGiven, guess new TN2Out

    TN2error = abs(TN2In - TN2InGiven);	
    if (TN2error > m_ToleranceN2T)
      {

      if (j == 1)
        {
        TN2OutPrev2 = m_TN2OutEst;
        fprev2 =TN2In - TN2InGiven; 
        m_TN2OutEst =m_TN2OutEst - (TN2In - TN2InGiven);

        }
      if (j == 2)
        {

        TN2OutPrev = m_TN2OutEst;
        fprev = TN2In - TN2InGiven;
        m_TN2OutEst = TN2OutPrev - fprev*(TN2OutPrev - TN2OutPrev2)/(fprev - fprev2);
        }
      if (j>=3)
        {			
        f = TN2In - TN2InGiven;
        TN2OutPrev2 = TN2OutPrev;
        fprev2 = fprev;
        TN2OutPrev = m_TN2OutEst;
        fprev = f;
        m_TN2OutEst = TN2OutPrev - fprev*(TN2OutPrev - TN2OutPrev2)/(fprev - fprev2);

        }
      }


    if (TN2error <= m_ToleranceN2T)
      break;

    if (TGasInGiven > TN2InGiven)
      m_TN2OutEst =Range(TN2InGiven, m_TN2OutEst, TGasInGiven); 
    else
      m_TN2OutEst =Range(TGasInGiven, m_TN2OutEst, TN2InGiven); 
    TN2Out = m_TN2OutEst;
    }

  //  Wwerk uit wat moet temp aan einde wees
  if (TGasOut == Tsat)//maw kondenseer sonder subcooling
    {
    Ppartial = Range(0.000001,molfrSiOutGas*PGasIn,PGasIn);	
    Tsat = 62.5213382197203*pow((Ppartial*1000),0.0796290365852);
    TGasOut = Tsat;
    Tliq = TGasOut;
    }

  GasO.T  = Range(TN2InGiven, TGasOut, TGasInGiven); 
  LiqO.T  = Range(TN2InGiven, Tliq, TGasInGiven); 
  CoolO.T = Range(TN2InGiven, m_TN2OutEst, TGasInGiven);
  m_Duty = q_inc/1000;
  };

//---------------------------------------------------------------------------

void CKoueValle::EvalProducts()
  {
  try           
    {
    MStream GasI;
    MStream CoolI;
    MStream & GasO = FlwIOs[FlwIOs.First[idGasOut]].Stream; //get reference to the actual output stream
    MStream & LiqO = FlwIOs[FlwIOs.First[idLiqOut]].Stream; //get reference to the actual output stream
    MStream & CoolO = FlwIOs[FlwIOs.First[idCoolOut]].Stream; //get reference to the actual output stream

    FlwIOs.AddMixtureIn_Id(GasI, idGasIn);
    FlwIOs.AddMixtureIn_Id(CoolI, idCoolIn);
    if (GasI.MassFlow()>1e-10 && CoolI.MassFlow()>0)
      DoCondensingHeater(GasI, CoolI, GasO, LiqO, CoolO);
    else
      {
      GasO=GasI;
      LiqO.ZeroMass();
      CoolO=CoolI;
      }

    //double tt=TubeO.T;
    //double st=ShellO.T;

    //double ShellPi=ShellI.P;
    //double ShellPo=ShellO.P;
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

void CKoueValle::ClosureInfo(MClosureInfo & CI)
  {
  if (CI.DoFlows())
    {
    //  HtIn += dActualDuty; //ensure heat balance
    }
  }

//====================================================================================
