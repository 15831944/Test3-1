//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992 -1998
 
#include "stdafx.h"
#include "sc_defs.h"
#include "flwnode.h"
#define  __BAROCON_CPP
#include "barocon.h"

//#include "optoff.h"

//==========================================================================

#define dbgModels         WITHDEBUG
#if dbgModels   
#include "dbgmngr.h"
static CDbgMngr dbgFlashTrain ("HeatXch1", "FlashTrain");
#endif

//==========================================================================
/*#D:#T:Barometric Condenser
#X:#n#n#h<General Description>#n
The barometric condenser is a direct contact heater (or rather cooler).
The unit has separate water and steam inlets, and mixture and vapour outlets.
In most situations the vapour line is connected to a steam ejector.
#n
The model mixes the inlet streams, rejects the amount of heat lost to atmosphere 
before carrying out the condensation and calculating the temperature, composition 
and flow rate of the exit streams.
#nThe liquid phase mixture in the receiver is at atmospheric pressure.  All the 
liquid is pumped from the mixture outlet.  The non condensable vapours exist via 
the vent at flash pressure.
#n
If the vapour outlet is not connected all the steam is assumed to condense at the 
equilibrium temperature.
#n
If the vapour line is connected the model operates in one of two modes.#n
1. The user specifies only the heat loss to atmosphere from the condenser.
The model will calculate the flash pressure and temperature, and the flow rate of 
the vapour and liquor phases.  (This mode assumes that the links to and from the 
condenser are configured to give realistic pressure drops.)#n
2. The user specifies the heat loss and the flash pressure.
The model will calculate the flash temperature, and the flow rate of the vapour and 
liquor phases, and the necessary impedances in the outlet pipes to give the required 
pressure drops to obtain a pressure balance.
#n
#n#u<#l<#b<Inputs and Outputs>>>#n#w
Label    Input/Output  Minimum number  Maximum number#n
steam        Input          1              1#n
water        Input          1              1#n
vapour       Output         0              1#n
mixture      Output         1              1#n
#a
To function the barometric condenser must have at least the steam, water, and mixture 
streams connected.
#n#n
#n#n#h<Variables to be supplied by the user>#n
#n#i<Rqd_Flash_Press:> The required pressure in the unit.
#n#i<ApproachTemp:> This variable is only visible if the unit is part of a Flash Train.  It 
is a measure of the inefficiency of the unit.  At 100% efficiency, or an approach 
temperature of 0 deg C, the cooling water would condense the steam and the mixture would 
exit from the unit at the saturated temperature.  The larger the approach temperature, the 
more cooling water required to condense the steam.  The mixture will leave the unit below 
the saturation temperature.
#n#n#h<Variables calculated by the model>#n
#n#i<Cond_Press:> The pressure in the unit.
#n#i<Cond_Temp:> The temperature in the unit.
#n#i<QRqd:> The steam required by the unit, this is only visible if the unit is part of a 
Flash Train.
#n#i<QCond:> The amount of condensate produced in the unit.
#n#n
#n#u<#l<#b<Assumptions & Limitations>>>#n#n
If the stream vent (vapour) is not connected, then it is assumed that all steam entering 
the barometric condenser condenses and reports to the mixture receiver.
#n#n
The mixture in the receiver is at atmospheric pressure and is pumped away from the unit 
at pressures much greater than atmospheric pressure.
#n#n
#n#u<#l<#b<Heat Transfer>>>#n#n
The model maintains a thermal balance.

#n#n#h<Other>#n
Default model prefix:BC#n
Short name:BaroCond#n
Model type:Unit#n
#G:Units
*/
//==========================================================================

const byte ioidBC_Steam = 0;
const byte ioidBC_Water = 1;
const byte ioidBC_Mix   = 2;
const byte ioidBC_Vap   = 3;

static IOAreaRec BarometricCondenserIOAreaList[] =
  {{"Steam",   "Steam"  , ioidBC_Steam , LIO_In0 ,   nc_MLnk, 1,  1, IOPipeEntry|IOChgFracHgt|IOChgAperture, 1.0f},
   {"Water",   "Water"  , ioidBC_Water , LIO_In1,    nc_MLnk, 1,  1, IOPipeEntry|IOChgFracHgt|IOChgAperture, (float)0.5},
   {"Mixture", "Mixture", ioidBC_Mix   , LIO_Out0,   nc_MLnk, 1,  1, IOPipeEntry|IOApertureHoriz|IOChgFracHgt|IOChgAperture, 0.0f},
   {"Vapour",  "Vapour",  ioidBC_Vap   , LIO_Out1,   nc_MLnk, 0,  1, IOPipeEntry|IOApertureHoriz|IOChgFracHgt|IOChgAperture, 1.0f},
   SPILL2AREA,
   VENT2AREA(-1),
   {NULL}}; //This lists the areas of the model where links can be attached.


static double Drw_BarometricCondenser[] = { 
                                 DD_Poly, -7,-4, -7,-11, 7,-11, 7,-4,
                                 DD_Poly, -1.4,-7.5, -1.4,3, -2.8,4.4, -2.8,10, -2.1,10.7, 2.1,10.7, 2.8,10, 2.8,4.4, 1.4,3, 1.4,-7.5,  
                                 DD_Poly, -7,-5.4, -1.4,-5.4,
                                 DD_Poly, 7,-5.4, 1.4,-5.4,
                                 DD_End }; 
/*static double Drw_BarometricCondenser[] = { 
                                 DD_Scale, 750,
                                 DD_Poly, -10,10, -10,0, 10,0, 10,10,
                                 DD_Poly, -2, 5, -2,20, -4,22, -4,30, -3,31, 3,31, 4,30, 4,22, 
                                           2,20, 2,5,  
                                 DD_Poly, -10,8, -2,8,
                                 DD_Poly,   2,8, 10,8,
                                 DD_End };*/
 //--------------------------------------------------------------------------

IMPLEMENT_MODELUNIT(BarometricCondenser, "BaroCond", "", Drw_BarometricCondenser, "HeatTransfer", "BC", TOC_ALL|TOC_GRP_ENERGY|TOC_HEATBAL,
//                    "HX - Barometric Condenser",
                    "Heat Transfer:Barometric Condenser",
                    "The barometric condenser is a direct contact heater " 
                    "The unit has separate water and steam inlets, and mixture "
                    "and vapour outlets. ")

BarometricCondenser::BarometricCondenser(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MN_Surge(pClass_, TagIn, pAttach, eAttach),
  m_FTC(this)
  {
  AttachIOAreas(BarometricCondenserIOAreaList, &PipeEntryGroup);
  Contents.SetClosed(true);
  Contents.SetPreset(&m_Preset, &m_PresetImg);

  ApproachTemp    = 10.0;
  Rqd_Flash_Press = dNAN;
  dFlashP         = Std_P;
  dFlashT         = Std_T;

  m_EHX.Open(&CEHX_LossPerQmClass);
  m_VLE.Open(NULL, true);

  RegisterMacroMdlNode(CMMFlashTrain::MMIOs, &typeid(BarometricCondenser), ioidBC_Steam, mmio_MODEL, &typeid(CFT_Condenser));
  };

// -------------------------------------------------------------------------

void BarometricCondenser::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);

  DDB.Visibility(NM_Dynamic|SM_All|HM_All);
  DDB.Text    ("");
  BuildDataDefnElevation(DDB);
  DDB.Double  ("Pressure",     "P",     DC_P,    "kPag",   xidPMean,       this, isResult|0);
  DDB.Double  ("Temperature",  "T",     DC_T,    "C",      xidTemp,        this, isResult|0);
  DDB.Double  ("Density",      "Rho",   DC_Rho,  "kg/m^3", xidRho,         this, isResult|0);
  DDB.Double  ("Level",        "Lvl",   DC_Frac, "%",      xidLevel,       this, isResult|0);
  DDB.Double  ("Mass_Accum",   "QmAcc", DC_Qm,  "kg/s",    &m_QmAcc,       this, isResult|0);
  DDB.Double  ("Vol_Accum",    "QvAcc", DC_Qv,  "L/s",     &m_QvAcc,       this, isResult|0);
  DDB.Visibility();

  DDB.Text    ("");
  DDB.Text    ("Requirements");
  DDB.Visibility(NM_Probal|SM_All|SM_Inline|HM_All);
  dword VFlags1=(m_FTC.Active() ? noView : 0);
	DDB.Double("Rqd_Flash_Press",   "",  DC_P,    "kPag",  &Rqd_Flash_Press,  this, isParm|NAN_OK|VFlags1);
  dword VFlags2=(m_FTC.Active() ? 0 : noView); 
  DDB.Double  ("ApproachTemp",      "",  DC_dT,   "C",     &ApproachTemp,     this, isParm|VFlags2);
  DDB.Visibility();
  RB.Add_OnOff(DDB);
  m_EHX.Add_OnOff(DDB);
  DDB.Visibility(SM_Buffered|HM_All);
  m_VLE.Add_OnOff(DDB);
  DDB.Visibility();

  DDB.Text    ("Results");
  DDB.Visibility(NM_Probal|SM_All|SM_Inline|HM_All);
  DDB.Double  ("Cond_Press",       "",  DC_P,    "kPag",  &dFlashP, this, isResult|0);
  DDB.Double  ("Cond_Temp",        "",  DC_T,    "C",     &dFlashT, this, isResult|0);
  m_FTC.BuildDataDefn(DDB);
  //DDB.Double  ("QRqd",              "",  DC_Qm,   "kg/s",  &m_FTC.dQRqd,        this, isResult|VFlags);
  //DDB.Double  ("QCond",             "",  DC_Qm,   "kg/s",  &m_FTC.dQCond,       this, isResult|VFlags);
  //DDB.Double  ("SuctionP",          "",  DC_P,    "kPa",   &m_FTC.dSuctionP,    this, isResult|VFlags);
  //DDB.Double  ("MinSatPress",       "",  DC_P,    "kPa",   &m_FTC.dMinSatPress, this, isResult|VFlags);
  //DDB.Text  (" ");
  //if (m_FTC.Active())
  //  {
  //  DDB.String("FlashTrain",        "",  DC_,     "",      &m_FTC.pMNd->TagRef(), this, isResult|noFile|noSnap|VFlags);
  //  DDB.String("FlashTrainEqp",     "",  DC_,     "",      m_FTC.MMList(),        this, isResult|MultiLineStr|noFile|noSnap|VFlags);
  //  }

  DDB.Visibility(NM_Dynamic|SM_All|HM_All);
  AddMdlNetworked(DDB);
  DDB.Visibility();

  BuildDataDefnShowIOs(DDB);
  BuildDataDefnIOOpts(DDB);
  
  //if (NetProbalMethod())
  //  DDB.Object(&QFd, this, NULL, NULL, DDB_RqdPage);

  RB.BuildDataDefn(DDB);
  m_EHX.BuildDataDefn(DDB);
  m_VLE.BuildDataDefn(DDB);

  if (SolveBufferedMethod())
    {
    DDB.Object(&Contents, this, NULL, NULL, DDB_RqdPage);
    DDB.Object(&m_PresetImg, this, NULL, NULL, DDB_RqdPage);
    }

  //if (Contents.SpillExists())
  //  DDB.Object(&Contents.OverFlow(), this, NULL, NULL, DDB_RqdPage);
  DDB.EndStruct();
 
  };

//--------------------------------------------------------------------------

flag BarometricCondenser::DataXchg(DataChangeBlk & DCB)
  {
  if (MN_Surge::DataXchg(DCB)) 
    return 1;

  return 0;
  }

//---------------------------------------------------------------------------

flag BarometricCondenser::ValidateData(ValidateDataBlk & VDB)
  {
  return MN_Surge::ValidateData(VDB);
  }

//--------------------------------------------------------------------------

flag BarometricCondenser::InitialiseSolution() 
  {
  MN_Surge::InitialiseSolution();
  return True;
  }

//--------------------------------------------------------------------------

void BarometricCondenser::EvalJoinPressures(long JoinMask)
  {
  switch (NetMethod())
    {
    case NM_Probal:
      {
      if (!m_FTC.Active())
        {
        const double SuctP = IOP_Flng(IOWithId_Self(ioidBC_Mix));
        m_FTC.SetSuctionP(SuctP);
        }
      if (NoProcessJoins()>0)
        {
        const double SuctP = MeasureJoinPressure(0);
        m_FTC.SetSuctionP(SuctP);
        SetJoinPressure(0, m_FTC.SuctionP());
        }
      }
      break;
    case NM_Dynamic:
      MN_Surge::EvalJoinPressures(JoinMask);
      break;
    }
  };
 
//--------------------------------------------------------------------------

void BarometricCondenser::SetState(eScdMdlStateActs RqdState)
  {
  MN_Surge::SetState(RqdState);
  switch (RqdState)
    {
    case MSA_PBInit:
      m_FTC.SetState(RqdState);
      break;
    case MSA_ZeroFlows:
    case MSA_Empty:
      break;
    case MSA_SteadyState:
      LogNote(FullObjTag(), 0, "SteadyState Undefined");
      break;
    }
  };

//--------------------------------------------------------------------------

void BarometricCondenser::EvalJoinFlows(int JoinNo)
  {
  switch (NetMethod())
    {
    case NM_Probal:
    case NM_Dynamic:
      break;
    }
  };
 
//--------------------------------------------------------------------------

void BarometricCondenser::EvalSteadyState()
  {
  };

//---------------------------------------------------------------------------

class BCSteamFinder : public MRootFinderBase
  {
  protected:
    BarometricCondenser &BC;

    double TSat, PFlash, TSteam, TApproach;
    SpConduit &Cd, &CdOther, &CdSteam;
    static CToleranceBlock s_Tol;

  public:
    BCSteamFinder(BarometricCondenser &BC_, SpConduit &Cd_, SpConduit &CdOther_, SpConduit &CdSteam_, double TSat_, double PFlash_, double TApp) :
      BC(BC_),
//      IC(IC_),
      Cd(Cd_),
      CdOther(CdOther_), 
      CdSteam(CdSteam_),
      MRootFinderBase("SteamFnd", s_Tol)//1.0e-8) 
      { 
      TSat=TSat_;
      PFlash=PFlash_;
      TApproach=TApp;
      TSteam=CdSteam.Temp();
      };
    LPCTSTR ObjTag() { return (LPCTSTR)BC.FullObjTag(); };      
    double Function(double x) 
      { 
      Cd.QSetF(CdOther, som_ALL, 1.0, PFlash);//Cd.Press());
      Cd.QAddM(CdSteam, som_ALL, x);
      
      BC.RB.EvalProducts(Cd);
      BC.m_EHX.EvalProducts(Cd);

      BC.m_VLE.SetFlashVapFrac(*Cd.Model(), 0.0, 0);
      double tt = Cd.Temp();
      return tt;
      };
  };

CToleranceBlock BCSteamFinder::s_Tol(TBF_Both, "BaroCon:SteamFinder", 0.0, 1.0e-8);

//--------------------------------------------------------------------------

void BarometricCondenser::EvalProducts(CNodeEvalIndex & NEI)
  {
  switch (SolveMethod())
    {
    case SM_Direct:
    case SM_Inline:
      {
      StkSpConduit Cd("Cd", chLINEID(), this);
      StkSpConduit CdOther("CdOther", chLINEID(), this);
      StkSpConduit CdSteam("CdSteam", chLINEID(), this);
      CdOther().QZero();
      CdSteam().QZero();

      double PFlash;
      int i;

      PFlash = !m_FTC.Active() && Valid(Rqd_Flash_Press) ? Rqd_Flash_Press : m_FTC.SuctionP();
      //PFlash = Max(10.0, PFlash);
      PFlash = Max(5.0, PFlash);

//TODO WHAT TO DO ABOUT SPECIE MODEL ie MassWtMean / Steam / ...

      SigmaQInPMin(CdOther(), som_ALL, Id_2_Mask(ioidBC_Water));
//      CdOther().SetModel(&SMSteamClass);
//      CdOther().SetPress(PFlash);
//      for (i=0; i<NoFlwIOs(); i++)
//        if (IO_In(i) && IOId_Self(i)!=ioidBC_Steam)
//          CdOther().QAddF(*IOConduit(i), som_ALL, 1.0);

      SigmaQInPMin(CdSteam(), som_ALL, Id_2_Mask(ioidBC_Steam));
//      CdSteam().SetModel(&SMSteamClass);
//      CdSteam().SetPress(PFlash);

      flag HasSteam = 0;
      for (i=0; i<NoFlwIOs(); i++)
        if (IO_In(i) && IOId_Self(i)==ioidBC_Steam)
//          {
//          CdSteam().QAddF(*IOConduit(i), som_ALL, 1.0);
          HasSteam = 1;
//          }
      m_FTC.m_dQCond = CdSteam().QMass();
      m_FTC.m_dQCool = CdOther().QMass();
      double QSteam=m_FTC.m_dQCond;
      if (m_FTC.Active())
        {
        if (HasSteam)
          {
          double QSteam  = CdSteam().QMass();
          double CpSteam = QSteam>1.0e-6 ? CdSteam().msCp() : 1.5;
          double TSteam  = QSteam>1.0e-6 ? CdSteam().Temp() : C_2_K(100.0);

          double QOther  = CdOther().QMass();
          double CpOther = CdOther().msCp();
          double TOther  = CdOther().Temp();

          // ApproachTemp is the degree of subcooling achieved 
          // by the excess water relative to the saturated temperature 
          // of the incoming steam.
          double TSat = CdSteam().SaturationT(PFlash);
          double TRqd = Min(TSteam - ApproachTemp, TSat - ApproachTemp); //hss 6/8/98
          
          Cd().QSetF(CdSteam(), som_ALL, 1.0, PFlash);//CdSteam().Press());
          m_VLE.SetFlashVapFrac(Cd(), 0.0, 0);
          double Psat = Cd().SaturationP(TRqd);

          double LHVap   = Cd().msLatentHeatVap(TSat, Psat);
          double CpWater = Cd().msCp(som_ALL, TSat, Psat);
          
          double QRqdEst = QOther * (TRqd - TOther) * CpOther/
                           ((TSteam - TSat) * CpSteam + LHVap + (TSat - TRqd) * CpWater);
          if (QRqdEst> 1.0e-6)
            {
            BCSteamFinder SteamFnd(*this, Cd(), CdOther(), CdSteam(), TSat, PFlash, ApproachTemp);
            SteamFnd.SetTarget(TRqd);

            m_FTC.m_dQRqd = QRqdEst;
            if (SteamFnd.Start(0.5*QRqdEst, 1.5*QRqdEst)==RF_OK)
              if (SteamFnd.SolveFromEst(QRqdEst, 1, 0.1*QRqdEst)==RF_OK) 
              //if (SteamFnd.Solve_Brent()==RF_OK)
                {
                m_FTC.m_dQRqd = SteamFnd.Result();
                QSteam = m_FTC.m_dQRqd;//+m_FTC.dQCond);
                }
            }
          else
            m_FTC.m_dQRqd = 0.0;
          #if dbgModels   
          if (dbgFlashTrain())
            dbgpln("BC: Est:%10.4f MxFl:%10.4f", m_FTC.m_dQRqd, QRqdEst);    
          #endif
          }
        else
          {
          m_FTC.m_dQRqd = 0.0;
          }

        m_FTC.m_dQRqd = Max(1.0e-6, m_FTC.m_dQRqd);
        }

//double T0=Cd().Temp(), P0=Cd().Temp();
      Cd().QSetF(CdOther(), som_ALL, 1.0, PFlash);//Cd().Press());
//double T1=Cd().Temp(), P1=Cd().Temp();
//      Cd().QAddF(CdSteam(), som_ALL, 1.0);
      Cd().QAddM(CdSteam(), som_ALL, QSteam);
//double T2=Cd().Temp(), P2=Cd().Temp();

      RB.EvalProducts(Cd());          
      m_EHX.EvalProducts(Cd());
//double T3=Cd().Temp(), P3=Cd().Temp();
      m_VLE.SetHfInAtZero(Cd());

      m_VLE.SetFlashVapFrac(Cd(), 0.0, 0);
      //double T4=Cd().Temp(), P4=Cd().Temp();
      dFlashP = Cd().Press();
      dFlashT = Cd().Temp();

      IOConduit(IOWithId_Self(ioidBC_Mix))->QSetF(Cd(), som_SL, 1.0, PFlash);
			
      int idVap=IOWithId_Self(ioidBC_Vap); // hss 12/3/98.
      if (idVap>=0)
				IOConduit(idVap)->QSetF(Cd(), som_Gas, 1.0, PFlash);

      m_FTC.m_dMinSatPress = 0.0;
      
      if (m_FTC.Active())
        {
        double R = m_FTC.m_dQRqd;
        double A = 0;
        int i;
        for (i=0; i<NoFlwIOs(); i++)
          if (IOId_Self(i)==ioidBC_Steam)
            if (IO_In(i) && IOFlange(i)->IsMakeUpAvail())
              A+= IOFlange(i)->MakeUpAvail();
        A = GTZ(A);
        for (i=0; i<NoFlwIOs(); i++)
          if (IOId_Self(i)==ioidBC_Steam)
            if (IO_In(i) && IOFlange(i)->IsMakeUpAvail())
              // Distribute according to MakeUpAvail
              IOFlange(i)->SetMakeUpReqd(R*Max(1.0e-9, IOFlange(i)->MakeUpAvail()/A));
        }
      else
        {
        //for (i=0; i<NoFlwIOs(); i++)
        //  if (IOId_Self(i)==ioidBC_Steam)
        //    IOFlange(i)->ClrMakeUpReqd();
        }
      m_VLE.AddHfOutAtZero(Cd());
      }
      break;
    default:
      MN_Surge::EvalProducts(NEI);
    }
  }
    
//--------------------------------------------------------------------------

void BarometricCondenser::ClosureInfo()
  {
  if (m_Closure.DoFlows())
    {
    CClosureInfo &CI=m_Closure[0];
    if (m_EHX.Enabled())
      CI.m_EHXPowerIn+=m_EHX.HeatFlow();
    if (1)
      CI.m_HfGainAtZero+=m_VLE.HfGainAtZero();
    }
  };

//--------------------------------------------------------------------------

#if DOPBSMOOTHING  
void BarometricCondenser::EvalMsAGDatas()
  {
  MN_Surge::EvalMsAGDatas();
  };
#endif

// --------------------------------------------------------------------------

void BarometricCondenser::MacroMdlEvaluate(eScdMacroMdlEvals Eval)
  {
  if (m_FTC.Active())
    {
    switch (Eval)
      {
      case MME_Flash2P:
        {       
        #if DOPBSMOOTHING  
        int n=0;
        for (int i=0; i<NoFlwIOs(); i++)
          if (IOId_Self(i)==ioidBC_Water)
            {
            CMSIO C;
            C.SetSrc(this, i);
            IOMSIndex(i)->SetMustExist(true);
            m_FTC.CoolLnks.SetAtGrow(n++, C);
            m_FTC.dVapLoss=0;
            }
        m_FTC.CoolLnks.SetSize(n);
        #endif
        }
        break;
      }
    }
  };

//==========================================================================
//
//
//
//==========================================================================
