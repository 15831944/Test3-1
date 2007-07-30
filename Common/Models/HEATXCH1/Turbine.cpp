//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992 - 1996
 
#include "stdafx.h"
#include "sc_defs.h"
#define  __TURBINE_CPP
#include "turbine.h"
//#include "optoff.h"

//==========================================================================

static double Drw_Turbine[] = { DD_Poly, -4,4, -4,-4, 4,-8, 4,8, -4,4, 
                                DD_TagPos, 0, -11.5,
                                DD_End };

//--------------------------------------------------------------------------

IMPLEMENT_MODELUNIT(CTurbine, "Turbine", "", Drw_Turbine, "HeatTransfer", "TRB", TOC_PROBAL|TOC_GRP_ENERGY|TOC_HEATEXTRA,
                    "Heat Transfer:Steam Turbine",
                    "Steam Turbine." )

static MInitialiseTest InitTest(&CTurbineClass);
SPECIEBLK_L(InitTest, H2OLiq, "H2O(l)", true);
SPECIEBLK_V(InitTest, H2OVap, "H2O(g)", true);

//BEGIN_TAGOBJSELECTABLE(CTurbine)              
//virtual bool Selectable() { return H2OLiq.Exists() && H2OVap.Exists(); }
//END_TAGOBJSELECTABLE()              

enum TEM_ExhaustMode { TEM_Off, TEM_Inoperative, TEM_SuperHeated, TEM_TwoPhaseSaturated };

XID xid_TurbinePower = MdlBsXID(4405);
XID xid_WorkPower = MdlBsXID(4406);
XID xid_TurbineWork = MdlBsXID(4407);
XID xid_MechWork = MdlBsXID(4408);

//--------------------------------------------------------------------------

CTurbine::CTurbine(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MN_Xfer(pClass_, TagIn, pAttach, eAttach)
  {
  AttachIOAreas(TwoIOAreaListWithSV, &PipeEntryGroup);
  //Contents.SetClosed(False);
  bOnLine = 1;
  dEfficiency = 0.8;
  dMechEff = 0.99;
  dPoutRqd = 750.0;
  bTrackSteamStatus = 1;
  dSteamTestFrac = 0.99;
  dPIn = Std_P;
  dPOut = Std_P;
  dTempIn = Std_T;
  dTempOut = Std_T;
  dSatTOut = Std_T;
  dSatPOut = Std_P;
  dVapFracIn = 1.0;
  dVapFracOut = 1.0;
  //dHeatFlow = 0.0;
  dIdealPower = 0.0;
  dIdealWork = 0.0;
  dQm = 0.0;
  //dCpIn = 0.0;
  //dCpOut = 0.0;
  dH20Frac = 1.0;
  dSgIn = 0.0;
  dSlIn = 0.0;
  dCalcT = dNAN;
  dCalcVF1 = dNAN;
  dCalcVF2 = dNAN;
  iExhaustMode = TEM_Inoperative;

  //EHX.Open(&CEHX_LossPerQmClass);
  m_VLE.Open(NULL, true);

  //RegisterMacroMdlNode(CMMFlashTrain::MMIOs, &CTurbineClass, ioid_ShellIn, mmio_MODEL, &typeid(CFT_Condenser));
  }

//--------------------------------------------------------------------------

CTurbine::~CTurbine()
  {
  }

//--------------------------------------------------------------------------

void CTurbine::BuildDataDefn(DataDefnBlk & DDB)
  {
  static DDBValueLst DDB1[]={
    {TEM_Off, "Off"},
    {TEM_Inoperative,         "Inoperative"},
    {TEM_SuperHeated,         "SuperHeated"},
    {TEM_TwoPhaseSaturated,   "Two Phase Saturated"},
    {0}};
  DDB.BeginStruct(this);
  DDB.Text    ("");
  DDB.Text    ("Requirements");
  DDB.CheckBox("On",              "",    DC_,     "",      &bOnLine,           this, isParm|SetOnChange);
  DDB.Double  ("P_Rqd",           "",    DC_P,    "kPag",  &dPoutRqd,          this, isParm);
  DDB.Double  ("Efficiency",      "",    DC_Frac, "%",     &dEfficiency,       this, isParm);
  DDB.Double  ("MechEfficiency",  "",    DC_Frac, "%",     &dMechEff,          this, isParm);
  DDB.CheckBox("TrackSteamFd",    "",    DC_,     "",      &bTrackSteamStatus, this, isParm);
  DDB.Visibility(NSHM_All, bTrackSteamStatus);
  DDB.Double  ("TrackSteamFrac",  "",    DC_Frac, "%",     &dSteamTestFrac,    this, isParm);
  DDB.Visibility();
  //RB.Add_OnOff(DDB, False);
  //EHX.Add_OnOff(DDB, False);

  DDB.Text    ("");
  DDB.Text    ("Results");
  DDB.Double  ("Mass_Flow",       "Qm",  DC_Qm,   "kg/s",  &dQm,               this, isResult|InitHidden);
  //DDB.Double  ("",                "Cp",  DC_CpMs, "kJ/kg.C",&dCpIn,              this, isResult|noFile|noSnap|InitHidden);
  DDB.Double  ("TemperatureIn",   "Ti",  DC_T,    "C",     &dTempIn,           this, isResult|noFile|noSnap);
  DDB.Double  ("TemperatureOut",  "To",  DC_T,    "C",     &dTempOut,          this, isResult);
  DDB.Double  ("PressIn",         "Pi",  DC_P,    "kPag",  &dPIn,              this, isResult|noFile|noSnap);
  DDB.Double  ("PressOut",        "Po",  DC_P,    "kPag",  &dPOut,             this, isResult);
  DDB.Double  ("VapourFracIn",    "Vfi", DC_Frac, "%",     &dVapFracIn,        this, isResult|noFile|noSnap);
  DDB.Double  ("VapourFracOut",   "Vfo", DC_Frac, "%",     &dVapFracOut,       this, isResult);
  DDB.Double  ("SatT",            "",    DC_T,    "C",     &dSatTOut,          this, isResult|noFile|noSnap);
  DDB.Double  ("SatP",            "",    DC_P,    "kPag",  &dSatPOut,          this, isResult|noFile|noSnap|InitHidden);
  DDB.Double  ("SteamEntropyIn",  "Sgi", DC_SMs,  "kJ/kg.K",&dSgIn,            this, isResult|noFile|noSnap|InitHidden);
  DDB.Double  ("WaterEntropyIn",  "Sli", DC_SMs,  "kJ/kg.K",&dSlIn,            this, isResult|noFile|noSnap|InitHidden);
  //DDB.Double  ("HeatFlow",        "",    DC_Pwr,  "kW",    &dHeatFlow,         this, isResult|InitHidden);
  DDB.Double  ("IdealWork",       "",    DC_HMs,  "kJ/kg", &dIdealWork,        this, isResult|InitHidden);
  DDB.Double  ("TurbineWork",     "",    DC_HMs,  "kJ/kg", xid_TurbineWork,    this, isResult|noFile|noSnap|InitHidden);
  DDB.Double  ("WorkOut",         "Work", DC_HMs, "kJ/kg", xid_MechWork,       this, isResult|noFile|noSnap);
  DDB.Double  ("IdealPower",      "",    DC_Pwr,  "kW",    &dIdealPower,       this, isResult);
  DDB.Double  ("TurbinePower",    "",    DC_Pwr,  "kW",    xid_TurbinePower,   this, isResult|noFile|noSnap);
  DDB.Double  ("PowerOut",        "Power", DC_Pwr, "kW",   xid_WorkPower,      this, isResult|noFile|noSnap);
  DDB.Double  ("H2O_Frac",        "",    DC_Frac, "%",     &dH20Frac,          this, isResult|InitHidden);
  DDB.Byte    ("ExhaustMode",     "",    DC_,     "",      &iExhaustMode,      this, isResult, DDB1);
  DDB.Text    ("");

  //dword VFlags=FTC.Active() ? 0 : noView; 
  //DDB.Double  ("QRqd",              "",  DC_Qm,   "kg/s",  &FTC.dQRqd,           this, isResult|VFlags);
  //DDB.Double  ("QCond",             "",  DC_Qm,   "kg/s",  &FTC.dQCond,          this, isResult|VFlags);
  //DDB.Double  ("SuctionP",          "",  DC_P,    "kPa",   &FTC.dSuctionP,       this, isResult|noView|VFlags);
  //DDB.Double  ("MinSatPress",       "",  DC_P,    "kPa",   &FTC.dMinSatPress,    this, isResult|VFlags);

  //if (FTC.Active())
  //  {
  //  DDB.String  ("FlashTrain",        "",  DC_,     "",      &FTC.pMNd->TagRef(), this, isResult|noFile|noSnap|VFlags);
  //  DDB.String  ("FlashTrainEqp",     "",  DC_,     "",      FTC.MMList(),        this, isResult|MultiLineStr|noFile|noSnap|VFlags);
  //  DDB.Text    ("");
  //  }

  //DDB.Visibility(NM_Dynamic|SM_All|HM_All);
  //AddMdlClosed(DDB);
  //AddMdlNetworked(DDB);
  //DDB.Text    ("");
  DDB.Visibility();

  DDB.Text    ("");
  BuildDataDefnShowIOs(DDB);
  BuildDataDefnIOOpts(DDB);

  //RB.BuildDataDefn(DDB);
  //EHX.BuildDataDefn(DDB);

  DDB.EndStruct();
  }

//--------------------------------------------------------------------------

flag CTurbine::DataXchg(DataChangeBlk & DCB)
  {
  if (MN_Xfer::DataXchg(DCB)) 
    return 1;
  switch (DCB.lHandle)
    {
    case xid_TurbinePower:
      DCB.D = dIdealPower*dEfficiency;
      return 1;
    case xid_WorkPower:
      DCB.D = dIdealPower*dEfficiency*dMechEff;
      return 1;
    case xid_TurbineWork:
      DCB.D = dIdealWork*dEfficiency;
      return 1;
    case xid_MechWork:
      DCB.D = dIdealWork*dEfficiency*dMechEff;
      return 1;
    }
  return 0;
  }

//---------------------------------------------------------------------------

flag CTurbine::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=MN_Xfer::ValidateData(VDB);
  //dTRiseRqd=ValidateRange(VDB, "TRiseRqd", -100.0, dTRiseRqd, 500.0);
  return OK;
  }

//--------------------------------------------------------------------------

void CTurbine::ConfigureJoins()
  {
  if (NetMethod()==NM_Probal)
    {
    for (int i=0; i<NoProcessIOs(); i++)
      SetIO_Join(i, 0); 
    }
  else
    {
    MN_Xfer::ConfigureJoins();
    }
  }

//--------------------------------------------------------------------------

void CTurbine::EvalJoinPressures(long JoinMask)
  {
  if (NoFlwIOs()>0)
    {
    if (NetMethod()==NM_Probal)
      {
      for (int j=0; j<NoJoins(); j++)
        {
        //double Pj=MeasureJoinPressure(j);
        SetJoinPressure(j, dPOut); //only set outlet pressures
        }
      }
    else
      MN_Xfer::EvalJoinPressures(JoinMask);
    }
  }
 
//--------------------------------------------------------------------------

void CTurbine::ClosureInfo()
  {
  MN_Xfer::ClosureInfo();
  if (m_Closure.DoFlows())
    {
    CClosureInfo &CI=m_Closure[0];
    CI.m_PowerIn-=(dIdealPower*dEfficiency);
    }
  }

//--------------------------------------------------------------------------

void   CTurbine::SetState(eScdMdlStateActs RqdState)
  {
  MN_Xfer::SetState(RqdState);
  switch (RqdState)
    {
    case MSA_PBInit:
      //FTC.SetState(RqdState);
      break;
    case MSA_ZeroFlows:
    case MSA_Empty:
    case MSA_PreSet:
      break;
    case MSA_SteadyState:
      LogNote(FullObjTag(), 0, "SteadyState Undefined");
      break;
    }
  }

//--------------------------------------------------------------------------

class TurbineTempFnd : public MRootFinderBase
  {
  public:
    static CToleranceBlock s_Tol;
    TurbineTempFnd(SpConduit &Cond, double RqdP_) : MRootFinderBase("TurbineTempFnd", s_Tol), C(Cond) { RqdP=RqdP_; };
    double  Function(double x);
  protected:
    SpConduit & C;
    double RqdP;
  };

CToleranceBlock TurbineTempFnd::s_Tol(TBF_Both, "Turbine:FinalTempFnd", 0.0, 1.0e-9);

double TurbineTempFnd::Function(double x)
  {
  C.SetTempPress(x, RqdP);
  double s = C.totSf();
  return s;
  }

//--------------------------------------------------------------------------

class TurbineVapFracFnd : public MRootFinderBase
  {
  public:
    static CToleranceBlock s_Tol;
    TurbineVapFracFnd(CTurbine & Turb_, SpConduit &Cond, double RqdT_, double RqdP_, bool UseEnthalpyCalc) 
      : MRootFinderBase("TurbineVapFracFnd", s_Tol), Turb(Turb_), C(Cond)
      { RqdP=RqdP_; RqdT=RqdT_; EnthalpyCalc=UseEnthalpyCalc; };
    double  Function(double x);
  protected:
    bool EnthalpyCalc;
    CTurbine & Turb;
    SpConduit & C;
    double RqdP;
    double RqdT;
  };

CToleranceBlock TurbineVapFracFnd::s_Tol(TBF_Both, "Turbine:VapFracFnd", 0.0, 1.0e-8);

double TurbineVapFracFnd::Function(double x)
  {
  Turb.m_VLE.SetFlashVapFrac(C, x, 0);
  C.SetTempPress(RqdT, RqdP);
  double d = EnthalpyCalc ? C.totHf() : C.totSf();
  return d;
  }

//--------------------------------------------------------------------------

void CTurbine::EvalProducts(CNodeEvalIndex & NEI)
  {
  iExhaustMode = TEM_Inoperative;
  if (NoFlwIOs()<2)
    return;
  const int In  = (IO_In(0) ? 0 : 1);
  const int Out = ((In+1) % 2);
  SpConduit & FiTmp = *IOConduit(In);
  SpConduit & Fo = *IOConduit(Out);
  ClrCI(1);
  ClrCI(2);
  ClrCI(3);

  StkSpConduit Fi("Fi", chLINEID(), this);
  Fi->QCopy(FiTmp);

  dQm = Fi->QMass();

  bool Error=false;
  if (IO_Zero(0))
    {
    Fi->QSetTraceMass();//QZero();
    Fo.QSetTraceMass();//QZero();
    IOFB(0,0)->SetQm(0.0);
    Error=true;
    }
  if (dQm<UsableMass)
    {
    Fo.QCopy(Fi());
    Error=true;
    }
  if (Error)
    {
    dIdealWork = 0.0;
    dIdealPower = 0.0;
    //dHeatFlow = 0.0;
    return;
    }

  if (NetDynamicMethod())
    {
    MN_Xfer::EvalProducts(NEI); //todo implement dynamic version.....
    return;
    }

  if (!bOnLine)
    {
    iExhaustMode = TEM_Off;
    Error=true;
    }

  const int H2Og_i = H2OVap();
  const int H2Ol_i = H2OLiq();
  dPIn       = Fi->Press();
  dTempIn    = Fi->Temp();
  dVapFracIn = Fi->MassFrac(som_Gas);
  dSgIn      = SDB[H2Og_i].msSf(Fi->Fidelity(), dTempIn, dPIn, NULL, NULL)-(SDB[H2Og_i].m_FData[Fi->Fidelity()].m_dCSf[0]/SDB[H2Og_i].MoleWt()); //entropy
  dSlIn      = SDB[H2Ol_i].msSf(Fi->Fidelity(), dTempIn, dPIn, NULL, NULL)-(SDB[H2Ol_i].m_FData[Fi->Fidelity()].m_dCSf[0]/SDB[H2Ol_i].MoleWt()); //entropy
  //dCpIn      = Fi->msCp();
  const double SteamQ = Fi->VMass[H2Og_i];
  const double WaterQ = Fi->VMass[H2Ol_i];
  dH20Frac = (SteamQ+WaterQ)/dQm;
  const double WaterFracIn = WaterQ/GTZ(SteamQ+WaterQ);
  SetCI(1, bTrackSteamStatus && (SteamQ/dQm<dSteamTestFrac));

  if (!Error && WaterFracIn>0.0)
    {
    const double SatTi = Fi->SaturationT(dPIn);
    if (dTempIn>SatTi)
      {//invalid steam feed should NOT contain water!!!
      //change feed to superheated steam...
      const double hi = Fi->totHf();
      double VF = 1.0;
      m_VLE.SetFlashVapFrac(Fi(), VF, 0);
      Fi->Set_totHf(hi);
      //temperature and entropy will now be different...
      }
    }

  Fo.QCopy(Fi());

  if (!Error)
    {
    if (dPoutRqd>=dPIn)
      {
      Error=true;
      SetCI(2, true);
      }
    }
    
  if (!Error)
    {
    ClrCI(2);
    const double hi = Fi->totHf();
    const double si = Fi->totSf();
    //const double PureSatTo = Fi->PureSaturationT(dPoutRqd);
    const double SatTo = Fi->SaturationT(dPoutRqd);
    bool SuperHeatedExhaust = false;
    double T = 0.0;
    int iRet;

    
    //TODO: Require a function to return temperature for a specified entropy and pressure for steam.
    //Use Brent instead...
    TurbineTempFnd Fnd1(Fo, dPoutRqd);
    Fnd1.SetTarget(si);
    if (Valid(dCalcT))
      {
      Fnd1.SetEstimate(dCalcT, -1.0);
      dCalcT = dNAN;
      }
    const double MnX = max(SatTo-250.0, ZeroCinK);
    const double MxX = max(dTempIn, SatTo)*1.2;
    iRet = Fnd1.Start(MnX, MxX);
    if (iRet==RF_EstimateOK) //estimate is good, solve not required
      {
      dCalcT = Fnd1.Result();
      }
    else
      {
      if (iRet==RF_BadEstimate)
        iRet = Fnd1.Start(MnX, MxX); // Restart
      if (iRet==RF_OK)
        if (Fnd1.Solve_Brent()==RF_OK)
          {
          dCalcT = Fnd1.Result();
          }
      if (iRet==RF_LoLimit)
        {
        dCalcT = MnX;
        }
      else if (iRet==RF_HiLimit)
        {
        dCalcT = MxX;
        }
      }
    if (Valid(dCalcT))
      {
      T = dCalcT;
      if (T>SatTo)
        SuperHeatedExhaust=true;
      }
    else
      {//PROBLEM!! No Solution???
      Error=true;
      #ifndef _RELEASE
      DoBreak();
      #endif
      SetCI(3, true);
      }

    if (SuperHeatedExhaust)
      {
      iExhaustMode = TEM_SuperHeated;
      Fo.SetTempPress(T, dPoutRqd);
      dCalcVF1 = dNAN;
      }
    else if (!Error)
      {
      //TODO: Require a function to adjust vapour fraction(flash) for a specified entropy, pressure and temparature.
      //Use Brent instead...
      TurbineVapFracFnd Fnd2(*this, Fo, SatTo, dPoutRqd, false);
      Fnd2.SetTarget(si);
      if (Valid(dCalcVF1))
        {
        Fnd2.SetEstimate(dCalcVF1, -1.0);
        dCalcVF1 = dNAN;
        }
      iRet = Fnd2.Start(0.01, 1.0);
      if (iRet==RF_EstimateOK) //estimate is good, solve not required
        {
        dCalcVF1 = Fnd2.Result();
        }
      else
        {
        if (iRet==RF_BadEstimate)
          iRet = Fnd2.Start(0.01, 1.0); // Restart
        if (iRet==RF_OK)
          if (Fnd2.Solve_Brent()==RF_OK)
            {
            dCalcVF1 = Fnd2.Result();
            }
        }
      if (!Valid(dCalcVF1))
        {//PROBLEM!! No Solution???
        Error=true;
        #ifndef _RELEASE
        DoBreak();
        #endif
        Fnd2.Function(Fnd2.Result());//use limit
        SetCI(3, true);
        }
      }

    if (!Error)
      {
      double vfo = Fo.MassFrac(som_Gas);
      double to = Fo.Temp();//should be equal to SatTo
      //double so = Fo.totSf();
  
      double ideal_ho = Fo.totHf();
      dIdealPower = hi-ideal_ho;
      dIdealWork = dIdealPower/dQm; //work per unit mass of steam kJ/kg

      //double Power = dIdealPower*dEfficiency*dMechEff; //this WAS WRONG should not include MechEff!
      double Power = dIdealPower*dEfficiency;
      double ho = hi - Power;
      //double hsat = Fo.totHf(som_ALL, SatTo, dPoutRqd);//???
      double hsat = SDB[H2Og_i].msHf(Fi->Fidelity(), SatTo, dPoutRqd, NULL, NULL)*dQm; //enthalpy at saturation if pure steam

      Fo.Set_totHf(ho);

      if (!SuperHeatedExhaust)
        {
        if (ho<hsat)
          {//Two phase saturated exhaust...
          //flash some more water to steam to satisfy saturation conditions...
          iExhaustMode = TEM_TwoPhaseSaturated;
          //TODO: Require a function to adjust vapour fraction(flash) for a specified enthalpy, pressure and temparature.
          double VF = 0.0;
          TurbineVapFracFnd Fnd3(*this, Fo, SatTo, dPoutRqd, true);
          Fnd3.SetTarget(ho);
          const double LoX = vfo*0.99; 
          if (Valid(dCalcVF2) && dCalcVF2>LoX)
            {
            Fnd3.SetEstimate(dCalcVF2, -1.0);
            dCalcVF2 = dNAN;
            }
          iRet = Fnd3.Start(LoX, 1.0);
          if (iRet==RF_EstimateOK) //estimate is good, solve not required
            {
            dCalcVF2 = Fnd3.Result();
            }
          else
            {
            if (iRet==RF_BadEstimate)
              iRet = Fnd3.Start(LoX, 1.0); // Restart
            if (iRet==RF_OK)
              if (Fnd3.Solve_Brent()==RF_OK)
                {
                dCalcVF2 = Fnd3.Result();
                }
            }

          if (Valid(dCalcVF2))
            {
            VF = dCalcVF2;
            m_VLE.SetFlashVapFrac(Fo, VF, 0);
            Fo.SetTempPress(SatTo, dPoutRqd);
            Fo.Set_totHf(ho);//do this again for numerical accuaracy conserving exact energy?
            }
          else
            {//Is this a problem, perhaps not???
            //Assume result is actually SuperHeated!?!
            SuperHeatedExhaust=true;
            iExhaustMode = TEM_SuperHeated;
            double VF = 1.0;
            m_VLE.SetFlashVapFrac(Fo, VF, 0);
            Fo.Set_totHf(ho);
            //Error=true;
            #ifndef _RELEASE
            DoBreak();
            #endif
            //SetCI(3, true);
            }
          }
        else
          {
          //superheated exhaust
          SuperHeatedExhaust=true;
          iExhaustMode = TEM_SuperHeated;
          double VF = 1.0;
          m_VLE.SetFlashVapFrac(Fo, VF, 0);
          Fo.Set_totHf(ho);
          dCalcVF2 = dNAN;
          }
        }
      else
        {
        dCalcVF2 = dNAN;
        }

      }
    }

  if (Error)
    {
    dIdealWork = 0.0;
    dIdealPower = 0.0;
    }

  //dHeatFlow   = -dPower;
  dPOut       = Fo.Press();
  dTempOut    = Fo.Temp();
  dVapFracOut = Fo.MassFrac(som_Gas);
  dSatTOut    = Fo.SaturationT(dPOut);
  dSatPOut    = Fo.SaturationP(dTempOut);
  //dCpOut      = Fo.msCp();
  }
  
//--------------------------------------------------------------------------

flag CTurbine::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case 1: pS="N\tExpect pure steam in feed";                           return 1;
    case 2: pS="W\tCannot Achieve Pressure Requirements based on feed";  return 1;
    case 3: pS="E\tError converging";                                    return 1;
    default:                                               
      return MN_Xfer::CIStrng(No, pS);
    }
  }

//==========================================================================

