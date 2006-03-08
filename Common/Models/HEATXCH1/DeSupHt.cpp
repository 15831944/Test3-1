//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992 - 1996
 
#include "stdafx.h"
#include "sc_defs.h"
#define  __DESUPHT_CPP
#include "desupht.h"

//==========================================================================

enum  TS_TempSpecs { TS_TDrop, TS_FinalT, TS_AppSatT };
//enum  QST_CalcMode { QST_FTCActive, QST_Makup, QST_General };

//==========================================================================

const byte ioidSteamIn  = 0;
const byte ioidFeedLiq  = 1;
const byte ioidSteamOut = 2;

static IOAreaRec DeSuperHeaterIOAreaList[] =
  {{"",   "SteamIn",    ioidSteamIn,  LIO_In0,     nc_MLnk, 1,  1,  IOGRP(1)|IOPipeEntry|IOShwFracHgt|IOShwAperture, (float)0.1},
  {"",    "FeedLiquor", ioidFeedLiq,  LIO_In1,     nc_MLnk, 1,  1,  IOGRP(1)|IOPipeEntry|IOShwFracHgt|IOShwAperture, (float)0.5},
  {"",    "SteamOut",   ioidSteamOut, LIO_Out0,    nc_MLnk, 1,  1,  IOGRP(1)|IOPipeEntry|IOShwFracHgt|IOShwAperture|IOApertureHoriz, 1.0f},
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
  MN_Surge(pClass_, TagIn, pAttach, eAttach),
  FTC(this)
  {
  AttachIOAreas(DeSuperHeaterIOAreaList, &PipeEntryGroup);
  Contents.SetClosed(False);

  bShowQFeed = 0;
  bTrackH2OFeed = 1;
  iTempSpec  = TS_AppSatT;
  dFinalTRqd = C_2_K(220.0);
  dAppTRqd   = 40.0;
  dTDropRqd  = 10.0;
  dFinalP    = Std_P;
  dFinalT    = Std_T;
  dSatTOut   = Std_T;
  dFlowRqd   = dNAN;
  dActualFlow= 0.0;

  VLE.Open(NULL, true);

  //RegisterMacroMdlNode(CMMFlashTrain::MMIOs, &DeSuperHeaterClass, ioidSI_Steam, mmio_MODEL, &typeid(CFT_Condenser));
  }

//--------------------------------------------------------------------------

DeSuperHeater::~DeSuperHeater()
  {
  }

//--------------------------------------------------------------------------

void DeSuperHeater::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);

  DDB.Visibility(SM_DynBoth|HM_All);
  BuildDataDefnElevation(DDB);
  DDB.Visibility();
  
  DDB.Text    ("");
  DDB.Text    ("Requirements");
  static DDBValueLst DDB0[]={
    {TS_TDrop,  "Temp Drop"},
    {TS_FinalT, "Final Temp"},
    {TS_AppSatT, "Approach SatT"},
    {0}};
  DDB.Byte    ("TempSpec",  "",  DC_,     "",      &iTempSpec,  this, isParm|AffectsStruct|SetOnChange, DDB0);
  DDB.Visibility(SHM_All, (iTempSpec == TS_FinalT));
  DDB.Double  ("FinalTRqd", "",  DC_T,    "C",     &dFinalTRqd, this, isParm);
  DDB.Visibility(SHM_All, (iTempSpec == TS_TDrop));
  DDB.Double  ("TDropRqd",  "",  DC_dT,   "C",     &dTDropRqd,  this, isParm);
  DDB.Visibility(SHM_All, (iTempSpec == TS_AppSatT));
  DDB.Double  ("ApproachSatT","",  DC_dT,   "C",     &dAppTRqd,  this, isParm);
  DDB.Visibility(SM_Direct|HM_All);
  DDB.CheckBox("ShowQFeed",         "",  DC_,     "",      &bShowQFeed,         this, isParm|SetOnChange);
  DDB.Visibility();
  DDB.CheckBox("TrackH2OFd",        "",  DC_,     "",      &bTrackH2OFeed,      this, isParm);
  //RB.Add_OnOff(DDB, False);
  //EHX.Add_OnOff(DDB, False);

  DDB.Text    ("");
  DDB.Text    ("Results");
  //DDB.Double  ("TRise",     "",  DC_dT,   "C",     &dTRise,     this, isResult|0);
  DDB.Double  ("FlowRqd",   "",  DC_Qm,   "kg/s",  &dFlowRqd,   this, isResult|NAN_OK);
  DDB.Double  ("ActualFlow","",  DC_Qm,   "kg/s",  &dActualFlow,this, isResult);
  DDB.Double  ("FinalP",    "",  DC_P,    "kPag",  &dFinalP,    this, isResult);
  DDB.Double  ("FinalT",    "",  DC_T,    "C",     &dFinalT,    this, isResult);
  DDB.Double  ("SatT",      "",  DC_T,    "C",     &dSatTOut,   this, isResult|noFile|noSnap);
  DDB.Double  ("DegSuperHeat","",DC_dT,   "C",     xid_DegSuperHeat,this, isResult|noFile|noSnap);
  
  DDB.Visibility(SM_DynBoth|HM_All);
  AddMdlClosed(DDB);
  AddMdlNetworked(DDB);

  DDB.Text    ("");
  BuildDataDefnShowIOs(DDB);
  BuildDataDefnIOOpts(DDB);

  //RB.BuildDataDefn(DDB);
  //EHX.BuildDataDefn(DDB);

  if (bShowQFeed && SolveDirectMethod())
    {
    QFeed(); // ensure exists
    if (QFeed.Exists())
      DDB.Object(&QFeed, this, NULL, NULL, DDB_RqdPage);
    }

  if (SolveDynamicMethod())
    {
    DDB.Object(&Contents, this, NULL, NULL, DDB_RqdPage);
    DDB.Object(&m_PresetImg, this, NULL, NULL, DDB_RqdPage);
    }

  DDB.EndStruct();
  }

//--------------------------------------------------------------------------

flag DeSuperHeater::DataXchg(DataChangeBlk & DCB)
  {
  if (MN_Surge::DataXchg(DCB)) 
    return 1;

  switch (DCB.lHandle)
    {
    case xid_DegSuperHeat:
      DCB.D = dFinalT-dSatTOut;
      return 1;
    case xidClosed:
      if (DCB.rB)
        Contents.SetClosed(*DCB.rB, DCB.ForView()); 
      DCB.B=Contents.Closed(); 
      return 1;
    }
  return 0;
  }

//---------------------------------------------------------------------------

flag DeSuperHeater::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=MN_Surge::ValidateData(VDB);
  dTDropRqd=ValidateRange(VDB, "TDropRqd", 0.0, dTDropRqd, 500.0);
  //dFinalTRqd=ValidateRange(VDB, "FinalTRqd", 20.0, dFinalTRqd, IF97_MaxSatT);
  return OK;
  }

//--------------------------------------------------------------------------

void DeSuperHeater::EvalJoinPressures(long JoinMask)
  {
  if (NoFlwIOs()>0)
    switch (NetMethod())
      {
      case SM_Direct:
        {
        for (int j=0; j<NJoins(); j++)
          {
          double Pj=GetPBInputPressure(j);
          SetPBJoinPressure(j, dFinalP, true, true);
          }
        }
        break;
      case SM_Inline:
      case SM_Buffered:
        MdlNode::EvalJoinPressures(JoinMask);
        break;
      }
  }
 
//--------------------------------------------------------------------------

void   DeSuperHeater::SetState(eScdMdlStateActs RqdState)
  {
  MN_Surge::SetState(RqdState);
  switch (RqdState)
    {
    case MSA_PBInit:
      FTC.SetState(RqdState);
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
      case SM_Inline:
      case SM_Buffered:
      case SM_Direct:
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
  DSH.VLE.SetFlashVapFrac(Mx, 1.0, 0);
  double dFinalT = Mx.Temp();
  return dFinalT;
  }

//--------------------------------------------------------------------------
  
void DeSuperHeater::EvalProducts(long JoinMask)
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
      
      double Pi = SigmaQInPMin(Mixture(), som_ALL, Id_2_Mask(ioidSteamIn)|Id_2_Mask(ioidFeedLiq));

      VLE.SetHfInAtZero(Mixture());

      if (bShowQFeed && !QFeed.Exists())
        QFeed();//forces create
      if (QFeed.Exists())
        QFeed().QCopy(Mixture());

      const int si = H2OVap();
      const int wi = H2OLiq();
      //double SteamInT = SteamIn().Temp();//WRONG 
      //double SteamInP = SteamIn().Press();//WRONG
      SpConduit & QSteamIn = *IOConduit(IOHotSteam);
      const double SteamInT = QSteamIn.Temp();
      const double SteamInP = QSteamIn.Press();
      const double SteamInQ = QSteamIn.QMass();
      SpConduit & QFeedLiq = *IOConduit(IOCoolWater);
      const double FeedLiqT = QFeedLiq.Temp();
      const double FeedLiqP = QFeedLiq.Press();
      const double FeedLiqQ = QFeedLiq.QMass();
      dActualFlow = FeedLiqQ;
      dFinalP = SteamInP;

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
        SetCI(2, bTrackH2OFeed && (SteamIn().QMass(som_SL)>1.0e-9) || PureSteamQ/GTZ(SteamIn().QMass(som_Gas))<0.98);
        const double PureWaterQ = WaterIn().VMass[wi];
        SetCI(3, bTrackH2OFeed && (PureWaterQ/FeedLiqQ<0.999));

        dSatTOut = Mixture().SaturationT(dFinalP);
        double RqdProdTemp = (iTempSpec==TS_AppSatT) ? dSatTOut+dAppTRqd : ((iTempSpec==TS_TDrop) ? SteamInT-dTDropRqd : dFinalTRqd);
        SetCI(1, RqdProdTemp<dSatTOut-1.0e-12);
        RqdProdTemp = Max(RqdProdTemp, dSatTOut+1.0e-9);
        bool Ok = false;
        DSH_FinalTempFnd FTF(SteamIn(), WaterIn(), Mixture(), *this, SteamInP, RqdProdTemp);
        FTF.SetTarget(RqdProdTemp);
        const double MaxQWater = Max(FeedLiqQ,1.0)*100.0;
        int iRet=FTF.Start(0.001, MaxQWater);
        if (Valid(dFlowRqd))
          {
          FTF.SetEstimate(dFlowRqd, 1.0);
          dFlowRqd = dNAN;
          }
        if (iRet==RF_EstimateOK) //estimate is good, solve not required
          {
          dFlowRqd = FTF.Result();
          Ok = true;
          }
        else
          {
          if (iRet==RF_BadEstimate)
            iRet = FTF.Start(0.0, MaxQWater); // Restart
          if (iRet==RF_OK)
            switch (FTF.Solve_Brent())
              {
              case RF_OK: dFlowRqd = FTF.Result(); Ok = true; break;
              case RF_HiLimit: dFlowRqd = MaxQWater; Ok = true; break;
              case RF_LoLimit: dFlowRqd = 0.1; Ok = true; break;
              }
          }
        CFlange &FeedFl=*IOFlange(IOCoolWater);
        if (FeedFl.IsMakeUpAvail())
          FeedFl.SetMakeUpReqd(dFlowRqd);

        //flash ALL water to steam
        SigmaQInPMin(Mixture(), som_ALL, Id_2_Mask(ioidSteamIn)|Id_2_Mask(ioidFeedLiq));
        const double hi = Mixture().totHf();
        Mixture().SetPress(SteamInP);
        double VF = 1.0;
        VLE.SetFlashVapFrac(Mixture(), VF, 0);
        Mixture().Set_totHf(hi);
        Fo.QCopy(Mixture());
        }
      else if (SteamInQ<UsableMass)
        {
        Fo.QCopy(QFeedLiq);
        }
      else
        {
        CFlange &FeedFl=*IOFlange(IOCoolWater);
        if (FeedFl.IsMakeUpAvail())
          FeedFl.SetMakeUpReqd(1.0);//demand something
        Fo.QCopy(QSteamIn);
        }
      
      dFinalT    = Fo.Temp();
      dFinalP    = Fo.Press();
      dSatTOut   = Fo.SaturationT(dFinalP);

      VLE.AddHfOutAtZero(Fo);
      }
      break;
    default:
      MN_Surge::EvalProducts(JoinMask);
    }
  }

//--------------------------------------------------------------------------

void DeSuperHeater::ClosureInfo()
  {
  if (m_Closure.DoFlows())
    {
    CClosureInfo &CI=m_Closure[0];
    if (EHX.Enabled())
      CI.m_EHXPowerIn+=EHX.HeatFlow();
    if (1)
      CI.m_HfGainAtZero+=VLE.HfGainAtZero();
    }
  };

//--------------------------------------------------------------------------

flag DeSuperHeater::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case 1: pS="E\tOutlet temperature cannot be below Saturation temperature"; return 1;
    case 2: pS="W\tExpect pure steam in steam feed"; return 1;
    case 3: pS="W\tExpect pure water in feed liquor feed"; return 1;
    default:                                               
      return MN_Surge::CIStrng(No, pS);
    }
  }

//==========================================================================

