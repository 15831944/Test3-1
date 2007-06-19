//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992 - 1996
 
#include "stdafx.h"
#include "sc_defs.h"
#define  __DESUPHT_CPP
#include "desupht.h"
//#include "optoff.h"

//==========================================================================

enum  TS_TempSpecs { TS_TDrop, TS_FinalT, TS_AppSatT };
//enum  QST_CalcMode { QST_FTCActive, QST_Makup, QST_General };

//==========================================================================

const byte ioidSteamIn  = 0;
const byte ioidFeedLiq  = 1;
const byte ioidSteamOut = 2;

static IOAreaRec DeSuperHeaterIOAreaList[] =
  {{"",   "SteamIn",    ioidSteamIn,  LIO_In0,     nc_MLnk, 1,  1,  IOOptsHide|IOPipeJoin|IOGRP(1)}, //IOGRP(1)|IOPipeEntry|IOChgFracHgt|IOChgAperture, (float)0.1},
  {"",    "FeedLiquor", ioidFeedLiq,  LIO_In1,     nc_MLnk, 1,  1,  IOOptsHide|IOPipeJoin|IOGRP(1)}, //IOGRP(1)|IOPipeEntry|IOChgFracHgt|IOChgAperture, (float)0.5},
  {"",    "SteamOut",   ioidSteamOut, LIO_Out0,    nc_MLnk, 1,  1,  IOOptsHide|IOPipeJoin|IOGRP(1)}, //IOGRP(1)|IOPipeEntry|IOChgFracHgt|IOChgAperture|IOApertureHoriz, 1.0f},
  SPILL2AREA,
  VENT2AREA(-1),
  {NULL}}; //This lists the areas of the model where links can be attached.

XID xid_DegSuperHeat  = MdlBsXID(4505);

static double Drw_DeSuperHeater[] ={DD_Poly, 8,3, 2,1, 1,1, 1,2, -1,2, -1,1, -2,1, -8,3, -8,-3, -2,-1, 2,-1, 8,-3, 8,3,
                                 DD_End };

//--------------------------------------------------------------------------

IMPLEMENT_MODELUNIT(DeSuperHeater, "DesuperHeater", "", Drw_DeSuperHeater, "HeatTransfer", "DSH", TOC_PROBAL|TOC_GRP_ENERGY|TOC_HEATEXTRA,
                    "Heat Transfer:DesuperHeater",
                    "Simple Desuperheater." )
                    
static MInitialiseTest InitTest(&DeSuperHeaterClass);
SPECIEBLK_L(InitTest, H2OLiq, "H2O(l)", true);
SPECIEBLK_V(InitTest, H2OVap, "H2O(g)", true);

DeSuperHeater::DeSuperHeater(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MN_Xfer(pClass_, TagIn, pAttach, eAttach),
  m_FTC(this)
  {
  AttachIOAreas(DeSuperHeaterIOAreaList, &PipeEntryGroup);
  //Contents.SetClosed(False);

  m_bShowQFeed = 0;
  m_bTrackH2OFeed = 1;
  m_iTempSpec  = TS_AppSatT;
  m_dFinalTRqd = C_2_K(220.0);
  m_dAppTRqd   = 40.0;
  m_dTDropRqd  = 10.0;
  m_dFinalP    = Std_P;
  m_dFinalT    = Std_T;
  m_dSatTOut   = Std_T;
  m_dFlowRqd   = dNAN;
  m_dActualFlow= 0.0;

  dPIn = Std_P;
  dTempIn = Std_T;
  dVapFracIn = 1.0;
  dVapFracOut = 1.0;

  m_VLE.Open(NULL, true);

  GSM.Open(); // required to Pass Avail/Reqd info about

  //RegisterMacroMdlNode(CMMFlashTrain::MMIOs, &DeSuperHeaterClass, ioidSI_Steam, mmio_MODEL, &typeid(CFT_Condenser));
  RegisterMacroMdlNode(CMMFlashTrain::MMIOs, &typeid(DeSuperHeater), ioidSteamIn, mmio_CONNECT, &typeid(CFT_Mixer));
  RegisterMacroMdlNode(CMMFlashTrain::MMIOs, &typeid(DeSuperHeater), ioidSteamOut, mmio_CONNECT, &typeid(CFT_Mixer));
  }

//--------------------------------------------------------------------------

DeSuperHeater::~DeSuperHeater()
  {
  }

//--------------------------------------------------------------------------

void DeSuperHeater::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);

  DDB.Visibility(NM_Dynamic|SM_All|HM_All);
  BuildDataDefnElevation(DDB);
  DDB.Visibility();
  
  DDB.Text    ("");
  DDB.Text    ("Requirements");
  static DDBValueLst DDB0[]={
    {TS_TDrop,  "Temp Drop"},
    {TS_FinalT, "Final Temp"},
    {TS_AppSatT, "Approach SatT"},
    {0}};
  DDB.Byte    ("TempSpec",  "",  DC_,     "",      &m_iTempSpec,  this, isParm|AffectsStruct|SetOnChange, DDB0);
  DDB.Visibility(NSHM_All, (m_iTempSpec == TS_FinalT));
  DDB.Double  ("FinalTRqd", "",  DC_T,    "C",     &m_dFinalTRqd, this, isParm);
  DDB.Visibility(NSHM_All, (m_iTempSpec == TS_TDrop));
  DDB.Double  ("TDropRqd",  "",  DC_dT,   "C",     &m_dTDropRqd,  this, isParm);
  DDB.Visibility(NSHM_All, (m_iTempSpec == TS_AppSatT));
  DDB.Double  ("ApproachSatT","",  DC_dT,   "C",     &m_dAppTRqd,  this, isParm);
  DDB.Visibility(NM_Probal|SM_All|HM_All);
  DDB.CheckBox("ShowQFeed",         "",  DC_,     "",      &m_bShowQFeed,         this, isParm|SetOnChange);
  DDB.Visibility();
  DDB.CheckBox("TrackH2OFd",        "",  DC_,     "",      &m_bTrackH2OFeed,      this, isParm);
  //RB.Add_OnOff(DDB, False);
  //m_EHX.Add_OnOff(DDB, False);

  //GSM.Add_OnOff(DDB);

  DDB.Text    ("");
  DDB.Text    ("Results");
  //DDB.Double  ("TRise",     "",  DC_dT,   "C",     &dTRise,     this, isResult|0);
  DDB.Double  ("FlowRqd",   "",  DC_Qm,   "kg/s",  &m_dFlowRqd,   this, isResult|NAN_OK);
  DDB.Double  ("ActualFlow","",  DC_Qm,   "kg/s",  &m_dActualFlow,this, isResult);

  DDB.Double  ("", "Steam.Ti",   DC_T,    "C",     &dTempIn,      this, isResult|noFile|noSnap);
  DDB.Double  ("FinalT", "To",   DC_T,    "C",     &m_dFinalT,    this, isResult);
  DDB.Double  ("", "Steam.Pi",   DC_P,    "kPag",  &dPIn,         this, isResult|noFile|noSnap|InitHidden);
  DDB.Double  ("FinalP", "Po",   DC_P,    "kPag",  &m_dFinalP,    this, isResult);
  DDB.Double  ("", "Steam.Vfi",  DC_Frac, "%",     &dVapFracIn,   this, isResult|noFile|noSnap|InitHidden);
  DDB.Double  ("VapourFracOut", "Vfo", DC_Frac, "%", &dVapFracOut, this, isResult);
  DDB.Double  ("SatT",      "",  DC_T,    "C",     &m_dSatTOut,   this, isResult|noFile|noSnap);
  DDB.Double  ("DegSuperHeat","",DC_dT,   "C",     xid_DegSuperHeat,this, isResult|noFile|noSnap);
  
  DDB.Visibility(NM_Dynamic|SM_All|HM_All);
  //AddMdlClosed(DDB);
  //AddMdlNetworked(DDB);

  DDB.Text    ("");
  BuildDataDefnShowIOs(DDB);
  BuildDataDefnIOOpts(DDB);

  //RB.BuildDataDefn(DDB);
  //m_EHX.BuildDataDefn(DDB);

  if (m_bShowQFeed && NetProbalMethod())
    DDB.Object(&m_QFeed, this, NULL, NULL, DDB_RqdPage);

  //if (NetDynamicMethod())
  //  {
  //  //DDB.Object(&Contents, this, NULL, NULL, DDB_RqdPage);
  //  DDB.Object(&m_PresetImg, this, NULL, NULL, DDB_RqdPage);
  //  }

  DDB.EndStruct();
  }

//--------------------------------------------------------------------------

flag DeSuperHeater::DataXchg(DataChangeBlk & DCB)
  {
  if (MN_Xfer::DataXchg(DCB)) 
    return 1;

  switch (DCB.lHandle)
    {
    case xid_DegSuperHeat:
      DCB.D = m_dFinalT-m_dSatTOut;
      return 1;
    }
  return 0;
  }

//---------------------------------------------------------------------------

flag DeSuperHeater::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=MN_Xfer::ValidateData(VDB);
  m_dTDropRqd=ValidateRange(VDB, "TDropRqd", 0.0, m_dTDropRqd, 500.0);
  //dFinalTRqd=ValidateRange(VDB, "FinalTRqd", 20.0, dFinalTRqd, IF97_MaxSatT);
  return OK;
  }

//--------------------------------------------------------------------------

void DeSuperHeater::EvalPBMakeUpReqd(long JoinMask)
  {
  if (GSM.Enabled())
    GSM.MakeUpNodeTransferReqd(0);
  MakeUpNodeTransferReqd(0);

  if (Valid(m_dFlowRqd))
    {
    const int IOHotSteam  = IOWithId_Self(ioidSteamIn);
    const int IOCoolWater = IOWithId_Self(ioidFeedLiq);
    const int IOSteamOut  = IOWithId_Self(ioidSteamOut);
    
    CFlange &HotSteam=*IOFlange(IOHotSteam);
    CFlange &CoolWater=*IOFlange(IOCoolWater);
    CFlange &SteamOut=*IOFlange(IOSteamOut);
    CoolWater.SetMakeUpReqd(m_dFlowRqd);
    HotSteam.SetMakeUpReqd(SteamOut.MakeUpReqd()-m_dFlowRqd);
    }
  }

//--------------------------------------------------------------------------

void DeSuperHeater::EvalPBMakeUpAvail(long JoinMask)
  {
  if (GSM.Enabled())
    GSM.MakeUpNodeTransferAvail(0);
  MakeUpNodeTransferAvail(0);
  }

//--------------------------------------------------------------------------

void DeSuperHeater::ConfigureJoins()
  {
  MN_Xfer::ConfigureJoins();
  /*if (NetMethod()==NM_Probal)
    {
    for (int i=0; i<NoProcessIOs(); i++)
      SetIO_Join(i, 0); 
    }
  else
    {
    MN_Xfer::ConfigureJoins();
    }*/
  }

//--------------------------------------------------------------------------

void DeSuperHeater::EvalJoinPressures(long JoinMask)
  {
  if (NoFlwIOs()>0)
    switch (NetMethod())
      {
      case NM_Probal:
        {
        for (int j=0; j<NoProcessJoins(); j++)
          {
          //double Pj=MeasureJoinPressure(j);
          //const int IOHotSteam  = IOWithId_Self(ioidSteamIn);
          //double P = IOP_Flng(IOHotSteam);
          SetJoinPressure(j, m_dFinalP); //only set outlet pressures
          }
        }
        break;
      case NM_Dynamic:
        MN_Xfer::EvalJoinPressures(JoinMask);
        break;
      }
  }
 
//--------------------------------------------------------------------------

void DeSuperHeater::SetState(eScdMdlStateActs RqdState)
  {
  MN_Xfer::SetState(RqdState);
  switch (RqdState)
    {
    case MSA_PBInit:
      m_FTC.SetState(RqdState);
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

void DeSuperHeater::EvalJoinFlows(int JoinNo)
  {
  if (NoFlwIOs()>0)
    switch (NetMethod())
      {
      case NM_Probal:
      case NM_Dynamic:
        break;
      }
  }
 
//--------------------------------------------------------------------------

class DSH_FinalTempFnd : public MRootFinderBase
  {
  public:
    SpConduit &HS, &CW, &Mx;
    DeSuperHeater &DSH;
    double    dRqdP;
    double    dRqdT;
    static CToleranceBlock s_Tol;
    
    DSH_FinalTempFnd(SpConduit &HotSteam, SpConduit &CoolWater, SpConduit &Mixture, DeSuperHeater &DesuperHt, double RqdP, double RqdT);
    double  Function(double x);
  };

CToleranceBlock DSH_FinalTempFnd::s_Tol(TBF_Both, "DesuperHt:FinalTempFnd", 0.0, 1.0e-9);

//--------------------------------------------------------------------------

DSH_FinalTempFnd::DSH_FinalTempFnd(SpConduit &HotSteam, SpConduit &CoolWater, SpConduit &Mixture, DeSuperHeater &DesuperHt, double RqdP, double RqdT) :
  MRootFinderBase("Desuperheater", s_Tol),
  HS(HotSteam),
  CW(CoolWater),
  Mx(Mixture),
  DSH(DesuperHt)
  {
  dRqdP   = RqdP;
  dRqdT   = RqdT;
  }

//--------------------------------------------------------------------------

double DSH_FinalTempFnd::Function(double x)
  {
  Mx.QSetF(HS, som_ALL, 1.0, dRqdP);
  Mx.QAddM(CW, som_ALL, x);
  DSH.m_VLE.SetFlashVapFrac(Mx, 1.0, 0);
  double dFinalT = Mx.Temp();
  return dFinalT;
  }

//--------------------------------------------------------------------------
  
void DeSuperHeater::EvalProducts(CNodeEvalIndex & NEI)
  {
  if (NoFlwIOs()==0)
    return;
  switch (SolveMethod())
    {
    case SM_Direct:
      {
      const int IOHotSteam  = IOWithId_Self(ioidSteamIn);
      const int IOCoolWater = IOWithId_Self(ioidFeedLiq);
      const int IOSteamOut  = IOWithId_Self(ioidSteamOut);

      StkSpConduit Mixture("Mixture", chLINEID(), this);
      
      SpConduit & Fo = *(IOConduit(IOSteamOut));
      
      double Pi = SigmaQInPMin(Mixture(), som_ALL, Id_2_Mask(ioidSteamIn));//|Id_2_Mask(ioidFeedLiq));
      Mixture().QAddF(*IOConduit(IOCoolWater), som_ALL, 1.0);

      //SetCI(4, IOP_Flng(IOCoolWater)<IOP_Flng(IOHotSteam)); do not need this test!?; ignore cool water pressure

      m_VLE.SetHfInAtZero(Mixture());

      if (m_bShowQFeed)
        m_QFeed().QCopy(Mixture());

      const int si = H2OVap();
      const int wi = H2OLiq();
      //double SteamInT = SteamIn().Temp();//WRONG 
      //double SteamInP = SteamIn().Press();//WRONG
      SpConduit & QSteamIn = *IOConduit(IOHotSteam);
      const double SteamInT = QSteamIn.Temp();
      const double SteamInP = QSteamIn.Press();
      const double SteamInQ = QSteamIn.QMass();
      dPIn       = SteamInP;
      dTempIn    = SteamInT;
      dVapFracIn = QSteamIn.MassFrac(som_Gas);
      SpConduit & QFeedLiq = *IOConduit(IOCoolWater);
      const double FeedLiqT = QFeedLiq.Temp();
      const double FeedLiqP = QFeedLiq.Press();
      const double FeedLiqQ = QFeedLiq.QMass();
      m_dActualFlow = FeedLiqQ;
      m_dFinalP = SteamInP;

      ClrCI(1);
      ClrCI(2);
      ClrCI(3);
      if (SteamInQ>UsableMass && FeedLiqQ>UsableMass)
        {
        StkSpConduit SteamIn("Steam", chLINEID(), this);
        StkSpConduit WaterIn("Water", chLINEID(), this);
        SigmaQInPMin(SteamIn(), som_ALL, Id_2_Mask(ioidSteamIn));
        SigmaQInPMin(WaterIn(), som_ALL, Id_2_Mask(ioidFeedLiq));
        const double PureSteamQ = SteamIn().VMass[si];
        SetCI(2, m_bTrackH2OFeed && (SteamIn().QMass(som_SL)>1.0e-9) || PureSteamQ/GTZ(SteamIn().QMass(som_Gas))<0.98);
        const double PureWaterQ = WaterIn().VMass[wi];
        SetCI(3, m_bTrackH2OFeed && (PureWaterQ/FeedLiqQ<0.999));

        m_dSatTOut = Mixture().SaturationT(m_dFinalP);
        double RqdProdTemp = (m_iTempSpec==TS_AppSatT) ? m_dSatTOut+m_dAppTRqd : ((m_iTempSpec==TS_TDrop) ? SteamInT-m_dTDropRqd : m_dFinalTRqd);
        //SetCI(1, RqdProdTemp<dSatTOut-1.0e-12);
        //RqdProdTemp = Max(RqdProdTemp, dSatTOut+1.0e-9);
        bool Ok = false;
        DSH_FinalTempFnd FTF(SteamIn(), WaterIn(), Mixture(), *this, SteamInP, RqdProdTemp);
        FTF.SetTarget(RqdProdTemp);
        const double MaxQWater = Max(FeedLiqQ,1.0)*100.0;
        int iRet=FTF.Start(0.001, MaxQWater);
        if (Valid(m_dFlowRqd))
          {
          FTF.SetEstimate(m_dFlowRqd, 1.0);
          m_dFlowRqd = dNAN;
          }
        if (iRet==RF_EstimateOK) //estimate is good, solve not required
          {
          m_dFlowRqd = FTF.Result();
          Ok = true;
          }
        else
          {
          if (iRet==RF_BadEstimate)
            iRet = FTF.Start(0.0, MaxQWater); // Restart
          if (iRet==RF_OK)
            switch (FTF.Solve_Brent())
              {
              case RF_OK: m_dFlowRqd = FTF.Result(); Ok = true; break;
              case RF_HiLimit: m_dFlowRqd = MaxQWater; Ok = true; break;
              case RF_LoLimit: m_dFlowRqd = 0.1; Ok = true; break;
              }
          }
        CFlange &CoolWater=*IOFlange(IOCoolWater);
        if (CoolWater.IsMakeUpAvail())
          CoolWater.SetMakeUpReqd(m_dFlowRqd);

        //flash ALL water to steam
        SigmaQInPMin(Mixture(), som_ALL, Id_2_Mask(ioidSteamIn)|Id_2_Mask(ioidFeedLiq));
        const double hi = Mixture().totHf();
        Mixture().SetPress(SteamInP);
        double VF = 1.0;
        m_VLE.SetFlashVapFrac(Mixture(), VF, 0);
        Mixture().Set_totHf(hi);
        Fo.QCopy(Mixture());
        }
      else if (SteamInQ<UsableMass)
        {
        Fo.QCopy(QFeedLiq);
        }
      else
        {
        CFlange &CoolWater=*IOFlange(IOCoolWater);
        if (CoolWater.IsMakeUpAvail())
          CoolWater.SetMakeUpReqd(1.0);//demand something
        Fo.QCopy(QSteamIn);
        }
      
      m_dFinalT    = Fo.Temp();
      m_dFinalP    = Fo.Press();
      m_dSatTOut   = Fo.SaturationT(m_dFinalP);
      dVapFracOut  = Fo.MassFrac(som_Gas);

      m_VLE.AddHfOutAtZero(Fo);
      }
      break;
    default:
      {
      MN_Xfer::EvalProducts(NEI);
      }
    }
  }

//--------------------------------------------------------------------------

void DeSuperHeater::ClosureInfo()
  {
  if (m_Closure.DoFlows())
    {
    CClosureInfo &CI=m_Closure[0];
    if (m_EHX.Enabled())
      CI.m_EHXPowerIn+=m_EHX.HeatFlow();
    if (1)
      CI.m_HfGainAtZero+=m_VLE.HfGainAtZero();
    }
  }

//--------------------------------------------------------------------------

flag DeSuperHeater::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case 1: pS="E\tOutlet temperature cannot be below Saturation temperature"; return 1;
    case 2: pS="W\tExpect pure steam in steam feed"; return 1;
    case 3: pS="W\tExpect pure water in feed liquor feed"; return 1;
    case 4: pS="W\tCooling water - Low Pressure"; return 1;
    default:                                               
      return MN_Xfer::CIStrng(No, pS);
    }
  }

//==========================================================================

