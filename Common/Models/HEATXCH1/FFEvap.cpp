//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992 - 1996
 
#include "stdafx.h"
#include "sc_defs.h"
#define  __FFEVAP_CPP
#include "ffevap.h"

//==========================================================================
#if WithFFEvap

const byte ioid_TubeIn    = 0;
const byte ioid_TubeOut   = 1;
const byte ioid_TubeEvap  = 2;
const byte ioid_ShellIn   = 3;
const byte ioid_ShellOut  = 4;
const byte ioid_ShellVent = 5;

const IDMask ShellIOMask=Id_2_Mask(ioid_ShellIn)|
                        Id_2_Mask(ioid_ShellOut)|
                        Id_2_Mask(ioid_ShellVent);


static IOAreaRec FFEvapIOAreaList[] =
  {{"",              "Liquor_In",   ioid_TubeIn,    LIO_In0 ,  nc_MLnk, 1, 10, IOGRP(1)|IOPipeEntry|IOShwFracHgt|IOShwAperture|IOApertureHoriz, 1.0f, {0,0,0,0}, NULL, &PipeEntryGroup},//, "Side[iP].I"},
   {"",              "Liquor_Out",  ioid_TubeOut,   LIO_Out0,  nc_MLnk, 1,  1, IOGRP(1)|IOPipeEntry|IOShwFracHgt|IOShwAperture|IOApertureHoriz, 0.0f, {0,0,0,0}, NULL, &PipeEntryGroup},//, "Side[iP].O"},
   {"",              "Liquor_Evap", ioid_TubeEvap,  LIO_Out ,  nc_MLnk, 1,  1, IOGRP(1)|IOPipeEntry|IOShwFracHgt|IOShwAperture|IOApertureHoriz, 0.0f, {0,0,0,0}, NULL, &PipeEntryGroup},//, "Side[iP].O"},
   {"",              "Steam",       ioid_ShellIn,   LIO_In1 ,  nc_MLnk, 1,  1, IOGRP(2)|IOPipeEntry|IOShwFracHgt|IOShwAperture|IOApertureHoriz, 1.0f, {0,0,0,0}, NULL, &PipeEntryGroup},//, "Side[iS].I"},
   {"",              "Condensate",  ioid_ShellOut,  LIO_Out1,  nc_MLnk, 1,  1, IOGRP(2)|IOPipeEntry|IOShwFracHgt|IOShwAperture|IOApertureHoriz, 0.0f, {0,0,0,0}, NULL, &PipeEntryGroup},//, "Side[iS].O"},
   {"",              "Shell_Vent",  ioid_ShellVent, LIO_Out ,  nc_MLnk, 0,  1, IOGRP(2)|IOPipeEntry|IOShwFracHgt|IOShwAperture|IOApertureHoriz, 1.0f, {0,0,0,0}, NULL, &PipeEntryGroup},//, "Side[iS].O"},
   {NULL}};

static double Drw_FFEvap[] = { DD_Poly, -4,11, -4,-11,
                             DD_Poly, 4,11, 4,-11,
                             DD_Poly, -4,8, 4,8,
                             DD_Poly, -4,-5, 4,-5,
                             DD_Poly, -3,8, -3,-5,
                             DD_Poly, -2,8, -2,-5,
                             DD_Poly, -1,8, -1,-5,
                             DD_Poly, 1,8, 1,-5,
                             DD_Poly, 2,8, 2,-5,
                             DD_Poly, 3,8, 3,-5,
                             DD_Arc3, -4,11, 0,13, 4,11,
                             DD_Arc3, -4,-11, 0,-13, 4,-11,
                             DD_End };

enum  QST_CalcMode { QST_FTCActive, QST_Makup, QST_General };

//--------------------------------------------------------------------------

IMPLEMENT_MODELUNIT(FFEvap, "FFEvap", "", Drw_FFEvap, "HeatTransfer", "FFE", TOC_PROBAL|TOC_GRP_ENERGY|TOC_HEATEXTRA,
                    "Heat Transfer:Falling Film Evaporator",
                    "Falling Film Evaporator." )

static MInitialiseTest InitTest(&FFEvapClass);
SPECIEBLK_L(InitTest, H2OLiq, "H2O(l)", true);
SPECIEBLK_V(InitTest, H2OVap, "H2O(g)", true);

//BEGIN_TAGOBJSELECTABLE(FFEvap)              
//virtual bool Selectable() { return H2OLiq.Exists() && H2OVap.Exists(); }
//END_TAGOBJSELECTABLE()              

//--------------------------------------------------------------------------

CHXSideHelper::CHXSideHelper(char* pName)
  {
  sName = pName;
  dTi=dTo=dSatT=Std_T;
  dPi=dPo=dSatP=Std_P;
  dHi=dHo=0.0;
  dVfi=dVfo=0.0;
  dQm=0.0;
  }

//--------------------------------------------------------------------------

void CHXSideHelper::BuildDataDefn(TaggedObject* pTagObj, DataDefnBlk & DDB)
  {
  if (DDB.BeginStruct(pTagObj, sName(), NULL, DDB_NoPage))
    {
    DDB.Double ("Mass_Flow",      "Qm",     DC_Qm,    "kg/s",    &dQm  , pTagObj, isResult);
    //DDB.Double ("",               "Cp",     DC_CpMs,  "kJ/kg.C", &dCp  , pTagObj, isResult|noFile|noSnap);
    DDB.Double ("TemperatureIn",  "Ti",     DC_T,     "C",       &dTi  , pTagObj, isResult|noFile|noSnap);
    DDB.Double ("TemperatureOut", "To",     DC_T,     "C",       &dTo  , pTagObj, isResult);
    DDB.Double ("PressIn",        "Pi",     DC_P,     "kPag",    &dPi  , pTagObj, isResult|noFile|noSnap);
    DDB.Double ("PressOut",       "Po",     DC_P,     "kPag",    &dPo  , pTagObj, isResult);
    DDB.Double ("VapourFracIn",   "Vfi",    DC_Frac,  "%",       &dVfi , pTagObj, isResult|noFile|noSnap);
    DDB.Double ("VapourFracOut",  "Vfo",    DC_Frac,  "%",       &dVfo , pTagObj, isResult);
    //DDB.Double ("",               "dT",     DC_dT,    "C",       ?, pTagObj, isResult|noFile|noSnap);
    DDB.Double ("SatT",           "",       DC_T,     "C",       &dSatT, pTagObj, isResult|noFile|noSnap);
    DDB.Double ("SatP",           "",       DC_P,     "kPag",    &dSatP, pTagObj, isResult|noFile|noSnap);
    //DDB.Double("Duty",            "",       DC_Pwr,   "kW",      &dDuty, pTagObj, isResult);
    DDB.Text("");
    }
  DDB.EndStruct();
  }

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

FFEvap::FFEvap(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MN_Surge(pClass_, TagIn, pAttach, eAttach),
  Shell("Shell"), 
  Tubes("Tube"),
  VLE_Tubes(this, VLEF_QVFlash)
  {
  AttachIOAreas(FFEvapIOAreaList, &PipeEntryGroup);
  Contents.SetClosed(False);
  dShellHeatLoss = 0.0;
  dTRiseRqd   = 0.0;
  dTubeMinPout = 20.0;
  dDuty       = 0.0;
  iCalcMode   = QST_General;
  dPrvVapFrac = 0.0;
  dQmEvap     = 0.0;
  dQmVapVent  = 0.0;
  bOnLine = 1;
  bAllowEvap = 1;
  bTrackSteamStatus = 1;
  bRemoveExcessVapour = 1;
  bShellVentConnected = 0;

  //EHX.Open(&CEHX_LossPerQmClass);
  VLE_Tubes.Open(NULL, true);
  VLE.Open(NULL, true); //shell

  //RegisterMacroMdlNode(CMMFlashTrain::MMIOs, &FFEvapClass, ioid_ShellIn, mmio_MODEL, &typeid(CFT_Condenser));
  }

//--------------------------------------------------------------------------

FFEvap::~FFEvap()
  {
  }

//--------------------------------------------------------------------------

void FFEvap::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);
  //DDB.Text    ("");
  //DDB.Text    ("Results");

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

  //DDB.Visibility(SM_DynBoth|HM_All);
  //AddMdlClosed(DDB);
  //AddMdlNetworked(DDB);
  //DDB.Text    ("");
  DDB.Visibility();

  //RB.Add_OnOff(DDB, False);

  DDB.Text    ("");
  BuildDataDefnShowIOs(DDB);
  BuildDataDefnIOOpts(DDB);

  DDB.Page("HX", DDB_RqdPage);
  DDB.Text    ("");
  DDB.CheckBox("On",        "", DC_,     "",         &bOnLine,        this, isParm|SetOnChange);
  DDB.Double  ("Duty",      "", DC_Pwr,  "kW",       &dDuty,          this, isResult);
  DDB.Text    ("");
  DDB.Text    ("Tube Side (Liquor)");
  DDB.CheckBox("AllowEvap", "", DC_,     "",         &bAllowEvap,     this, isParm|SetOnChange);
  DDB.Visibility(SM_Direct|HM_All, bAllowEvap);
  DDB.Double  ("TRiseRqd",  "", DC_dT,   "C",        &dTRiseRqd,      this, isParm);
  DDB.Double  ("MinPressOut", "MinPout", DC_P, "kPag", &dTubeMinPout, this, isParm);
  DDB.Visibility();
  DDB.Double  ("EvapQm",    "", DC_Qm,   "kg/s",     &dQmEvap,        this, isResult);
  DDB.Text    ("Shell Side");
  //EHX.Add_OnOff(DDB, False);
  //EHX.BuildDataDefn(DDB);
  DDB.Double  ("HeatLoss",  "", DC_Pwr,  "kW",       &dShellHeatLoss, this, isParm);
  DDB.Double  ("VapQmOut",  "", DC_Qm,   "kg/s",     &dQmVapVent,     this, isResult|InitHidden);
  DDB.Visibility(SM_Direct|HM_All, !bShellVentConnected);
  DDB.CheckBox("VentExcessShellVap", "", DC_, "",    &bRemoveExcessVapour,this, isParm);
  DDB.Visibility();
  DDB.CheckBox("TrackSteamFd",       "", DC_, "",    &bTrackSteamStatus,  this, isParm);
  DDB.Text    ("");
  Tubes.BuildDataDefn(this, DDB);
  Shell.BuildDataDefn(this, DDB);
  
  //RB.BuildDataDefn(DDB);

  DDB.EndStruct();
  }

//--------------------------------------------------------------------------

flag FFEvap::DataXchg(DataChangeBlk & DCB)
  {
  if (MN_Surge::DataXchg(DCB)) 
    return 1;
  return 0;
  }

//---------------------------------------------------------------------------

flag FFEvap::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=MN_Surge::ValidateData(VDB);
  dTRiseRqd=ValidateRange(VDB, "TRiseRqd", -100.0, dTRiseRqd, 500.0);
  return OK;
  }

//--------------------------------------------------------------------------

void FFEvap::ConfigureJoins()
  {
  switch (NetMethod())
    {
    case NM_Probal:
      {
      Set_NoJoins(2);
      for (int i=0; i<NoProcessIOs(); i++)
        if (IOId_Self(i)>=ioid_ShellIn)
          SetIO_Join(i, 0);
        else
          SetIO_Join(i, 1);

      //todo SetTearPriority
//IOFlange(i)->SetTearPriority((1 && (IOId_Self(i)==ioidSIn)) ? TP_Last : TP_Normal);
      break;
      }
    case NM_Dynamic:
      {
      Set_NoJoins(2);
      //todo
      break;
      }
    }
  }

//--------------------------------------------------------------------------

void FFEvap::EvalJoinPressures(long JoinMask)
  {
  if (NoFlwIOs()>0)
    {
    switch (NetMethod())
      {
      case NM_Probal:
        {
        for (int j=0; j<NoProcessJoins(); j++)
          {
          double Pj=GetPBInputPressure(j);

          //if (j==1 && m_HX.m_FTC.Active())
          //  m_FTC.SetSuctionP(Pj);
          //SetPBJoinPressure(j, Pj, true, true);

          if (j==0)
            SetPBJoinPressure(j, Shell.dPo, true, true);
          else
            SetPBJoinPressure(j, Tubes.dPo, true, true);
          }
        }
        break;
      case NM_Dynamic:
        MdlNode::EvalJoinPressures(JoinMask);
        break;
      }
    }
  }
 
//--------------------------------------------------------------------------

void   FFEvap::SetState(eScdMdlStateActs RqdState)
  {
  MN_Surge::SetState(RqdState);
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

class FFE_FinalTempFnd : public MRootFinderBase
  {
  public:
    static CToleranceBlock s_Tol;
    
    FFE_FinalTempFnd(FFEvap &FFE_, SpConduit &Cond);
    double  Function(double x);
  protected:
    FFEvap &FFE;
    SpConduit &C;
    double H;
  };

CToleranceBlock FFE_FinalTempFnd::s_Tol(TBF_Both, "FFEvap:FinalTempFnd", 0.0, 1.0e-6);

//--------------------------------------------------------------------------

FFE_FinalTempFnd::FFE_FinalTempFnd(FFEvap &FFE_, SpConduit &Cond):
  MRootFinderBase("FFEvapTempFnd", s_Tol),
  FFE(FFE_),
  C(Cond)
  {
  H = C.totHf();
  }

//--------------------------------------------------------------------------

double FFE_FinalTempFnd::Function(double x)
  {
  FFE.VLE_Tubes.SetFlashVapFrac(C, x, 0);
  const double sp = C.SaturationP(dTarget);
  C.SetPress(sp);
  C.Set_totHf(H);
  double dFinalT = C.Temp();
  return dFinalT;
  }

//--------------------------------------------------------------------------

void FFEvap::EvalProducts(CNodeEvalIndex & NEI)
  {
  if (NoFlwIOs()==0)
    return;
  switch (SolveMethod())
    {
    case SM_Direct:
      {
      dTubeMinPout = Max(1.0, dTubeMinPout);
      //dVentRqd = Max(0.0, dVentRqd);
      const int IOSteam   = IOWithId_Self(ioid_ShellIn);   //Steam feed
      const int IOExVap   = IOWithId_Self(ioid_ShellVent); //Excess Steam/vapour vent
      const int IOConden  = IOWithId_Self(ioid_ShellOut);  //Condensate
      const int IOLiqFeed = IOWithId_Self(ioid_TubeIn);    //weak liquor feed
      const int IOHotLiq  = IOWithId_Self(ioid_TubeOut);   //conc liquor outlet
      const int IOHotEvap = IOWithId_Self(ioid_TubeEvap);  //liquor outlet evap steam
      StkSpConduit Cshell("Shell", chLINEID(), this);
      StkSpConduit Ctubes("Tube", chLINEID(), this);
      
      SigmaQInPMin(Ctubes(), som_ALL, Id_2_Mask(ioid_TubeIn));
      SigmaQInPMin(Cshell(), som_ALL, Id_2_Mask(ioid_ShellIn));

      SpConduit & ShellSteamIn  = *(IOConduit(IOSteam));
      SpConduit & TubesHotLiqIn = *(IOConduit(IOLiqFeed));
      
      bShellVentConnected = (IOExVap>=0);
      const int H2Og_i    = H2OVap();
      const int H2Ol_i    = H2OLiq();

      //shell steam in...
      if (bOnLine)
        Cshell().Set_totHf(Cshell().totHf()-dShellHeatLoss);//apply heat loss
      Shell.dQm = Cshell().QMass();
      Shell.dHi = Cshell().totHf();
      Shell.dPi = ShellSteamIn.Press();//get from first pipe in
      Shell.dTi = ShellSteamIn.Temp();//get from first pipe in
      Shell.dVfi = Cshell().MassFrac(som_Gas);
      double PureSteamQm = Cshell().VMass[H2Og_i];

      VLE.SetHfInAtZero(Cshell()); 
      VLE_Tubes.SetHfInAtZero(Ctubes()); 

      //tubes liquor in...
      Tubes.dQm = Ctubes().QMass();
      Tubes.dHi = Ctubes().totHf();
      Tubes.dPi = TubesHotLiqIn.Press();//get from first pipe in
      Tubes.dTi = TubesHotLiqIn.Temp();//get from first pipe in
      Tubes.dVfi = Ctubes().MassFrac(som_Gas);


      flag HasSteam  = (Shell.dQm>1.0e-6);
      flag HasLiquor = (Tubes.dQm>1.0e-6);

      SetCI(4, bOnLine && bTrackSteamStatus && HasSteam && (PureSteamQm/Shell.dQm)<0.9999); //expect 100% steam in steam line

      //Flange & StmFl = *IOFlange(IOSteam);
      //if (FTC.Active() || StmFl.IsMakeUpAvail())
      //  {
      //  }
      //else
        {
        //iCalcMode = QST_General;
        //RB.EvalProducts(Mixture());

        bool ZeroEvapLimit = false;
        if (bOnLine && HasSteam)
          {

          Shell.dTo = Cshell().SaturationT(Shell.dPi);
          Shell.dPo = Cshell().SaturationP(Shell.dTo);
          Cshell().SetPress(Shell.dPo); //change pressure
          Cshell().Set_totHf(Shell.dHi); //ensure enthalpy is same
          VLE.SetFlashVapFrac(Cshell(), 0.0, 0); //condense all steam
          Cshell().SetTemp(Shell.dTo); //change to required sat temp
          Shell.dHo = Cshell().totHf(); //get new enthalpy
          dDuty = Shell.dHi - Shell.dHo; //calc duty
          Tubes.dPo = Tubes.dPi; //maintain tube liquor pressure
          Ctubes().SetPress(Tubes.dPo); //change pressure
          Ctubes().Set_totHf(Tubes.dHi+dDuty); //apply duty
          if (bAllowEvap)
            {
            Tubes.dTo = Tubes.dTi + dTRiseRqd; //required temperature
            Tubes.dPo = Ctubes().SaturationP(Tubes.dTo); //estimate required pressure
            if (Tubes.dPo<dTubeMinPout)
              {
              VLE_Tubes.SetFlashVapFrac(Ctubes(), 0.0, 0); //condense all steam
              Ctubes().Set_totHf(Tubes.dHi+dDuty); //ensure enthalpy is same
              Tubes.dPo = Ctubes().SaturationP(Tubes.dTo);
              if (Tubes.dPo<dTubeMinPout)//check again
                {
                Tubes.dPo = dTubeMinPout;
                VLE_Tubes.QPFlash(Ctubes(), Tubes.dPo, 0.0, 0); //flash to required temperature, if BPE=0.0 then press=PureSaturationP
                double vf = Ctubes().MassFrac(som_Gas);
                if (vf<0.00000001)
                  {
                  ZeroEvapLimit = true;
                  Ctubes().SetPress(Tubes.dPo); //set to min pressure
                  Ctubes().Set_totHf(Tubes.dHi+dDuty); //ensure enthalpy is same
                  Tubes.dTo = Ctubes().Temp();
                  }
                else
                  Tubes.dTo = Ctubes().SaturationT(Tubes.dPo);
                }
              }
            if (!ZeroEvapLimit)
              {
              VLE_Tubes.QPFlash(Ctubes(), Tubes.dPo, 0.0, 0); //flash to required temperature, if BPE=0.0 then press=PureSaturationP
              //require covergance to exact reqd temp...
              const double sv = Ctubes().VMass[H2Og_i];
              const double dEst = sv/GTZ(sv+Ctubes().VMass[H2Ol_i]);
              if (fabs(dEst-dPrvVapFrac)>0.05)
                dPrvVapFrac = dEst;
              FFE_FinalTempFnd Fnd(*this, Ctubes());
              Fnd.SetTarget(Tubes.dTo);
              Fnd.SetLimits(Max(0.0, dPrvVapFrac-0.1), Min(1.0, dPrvVapFrac+0.1));
              Fnd.SetEstimate(dPrvVapFrac, 1.0);
              int iRet=Fnd.Start(Max(0.0, dPrvVapFrac-0.1), Min(1.0, dPrvVapFrac+0.1));
              if (iRet==RF_EstimateOK) //estimate is good, solve not required
                {
                dPrvVapFrac = Fnd.Result();
                }
              else
                {
                if (Fnd.Solve_Brent()==RF_OK)
                  {
                  dPrvVapFrac = Fnd.Result();
                  }
                }
              }
            }
          else
            {
            }
          dQmVapVent = Cshell().QMass(som_Gas);

          //VLE.SensibleHeatOut(Cshell()); //steam in
          }
        else
          {
          Cshell().SetTempPress(Shell.dTi, Shell.dPi); //set to same as input
          Cshell().Set_totHf(Shell.dHi); //ensure enthalpy is same
          Ctubes().SetTempPress(Tubes.dTi, Tubes.dPi); //set to same as input
          Ctubes().Set_totHf(Tubes.dHi); //ensure enthalpy is same
          dDuty = 0.0;
          dQmVapVent = 0.0;
          }

        Tubes.dTo   = Ctubes().Temp(); //get actual outlet temp
        Tubes.dPo   = Ctubes().Press(); //get actual outlet pressure
        Tubes.dHo   = Ctubes().totHf();
        Tubes.dVfo  = Ctubes().MassFrac(som_Gas);
        Tubes.dSatT = Ctubes().SaturationT(Tubes.dPo);
        Tubes.dSatP = Ctubes().SaturationP(Tubes.dTo);
        /*Tubes.dHEnv*/double eT = Tubes.dHo-Tubes.dHi;

        Shell.dTo   = Cshell().Temp(); //get actual outlet temp
        Shell.dPo   = Cshell().Press(); //get actual outlet pressure
        Shell.dHo   = Cshell().totHf();
        Shell.dVfo  = Cshell().MassFrac(som_Gas);
        Shell.dSatT = Cshell().SaturationT(Shell.dPo);
        Shell.dSatP = Cshell().SaturationP(Shell.dTo);
        /*Shell.dHEnv*/double eS = Shell.dHo-Shell.dHi;

        if (bOnLine)
          {
          SetCI(1, ZeroEvapLimit);
          SetCI(2, bAllowEvap && !ZeroEvapLimit && fabs(Tubes.dTi + dTRiseRqd - Tubes.dTo)>1.0e-4);
          IOConduit(IOConden)->QSetF(Cshell(), som_SL, 1.0, Shell.dPo);
          IOConduit(IOHotLiq)->QSetF(Ctubes(), som_SL, 1.0, Tubes.dPo);
          IOConduit(IOHotEvap)->QSetF(Ctubes(), som_Gas, 1.0, Tubes.dPo);
          if (bShellVentConnected)
            {
            ClrCI(3);
            SpConduit & CVent = *(IOConduit(IOExVap));
            CVent.QSetF(Cshell(), som_Gas, 1.0, Shell.dPo);
            }
          else
            {
            SetCI(3, fabs(dQmVapVent)>UsableMass);
            if (!bRemoveExcessVapour)
              IOConduit(IOConden)->QAddF(Cshell(), som_Gas, 1.0);
            }
          }
        else
          {
          ClrCI(1);
          ClrCI(2);
          ClrCI(3);
          IOConduit(IOConden)->QCopy(Cshell());
          IOConduit(IOHotLiq)->QCopy(Ctubes());
          IOConduit(IOHotEvap)->QZero();
          if (bShellVentConnected)
            IOConduit(IOExVap)->QZero();
          }
        dQmEvap = IOConduit(IOHotEvap)->QMass();
        }
      
      //VLE.AddHfOSensibleHeatOut(Cshell()); //steam out
      VLE.AddHfOutAtZero(Cshell()); 
      VLE_Tubes.AddHfOutAtZero(Ctubes()); 

      //dTRise  = dFinalT - Liquor().Temp();
      //SetCI(1, HasLiquor && fabs(dFinalT-RqdProdTemp)/RqdProdTemp>1.0e-4);

      //FTC.dMinSatPress = 0.0; // ToFix
      }
      break;
    default:
      MN_Surge::EvalProducts(NEI);
    }
  }
  
//--------------------------------------------------------------------------

void FFEvap::ClosureInfo()
  {
  if (m_Closure.DoFlows())
    {
    CClosureInfo &CI0=m_Closure[0];
    CClosureInfo &CI1=m_Closure[1];
    //if (EHX.Enabled())
    //  CI.m_PowerIn+=EHX.HeatFlow();

    CI0.m_PowerIn+=-dDuty;
    CI1.m_PowerIn+=+dDuty;

    CI0.m_HfGainAtZero+=VLE.HfGainAtZero();
    CI1.m_HfGainAtZero+=VLE_Tubes.HfGainAtZero();
    }
  };

//--------------------------------------------------------------------------

flag FFEvap::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case 1: pS="W\tNo evaporation (Liquor temperature/pressure conditions too low)"; return 1;
    case 2: pS="W\tLiquor temperature rise required before evaporation occurs."; return 1;
    case 3: pS="W\tNo Shell Vent Defined";                               return 1;
    case 4: pS="W\tExpect pure steam in steam line";                     return 1;
    default:                                               
      return MN_Surge::CIStrng(No, pS);
    }
  }

//==========================================================================
#endif

