//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992 - 1996

#include "stdafx.h"
#include "sc_defs.h"
#define  __BOILER_CPP
#include "boiler.h"

//==========================================================================

const byte ioidFeedLiq  = 0;
const byte ioidSteamOut = 1;
const byte ioidBlowDown = 2;

static IOAreaRec BoilerIOAreaList[] =
  {{"",   "FeedLiquor", ioidFeedLiq,  LIO_In0 ,    nc_MLnk, 1, 10,  IOGRP(1)|IOPipeEntry|IOChgFracHgt|IOChgAperture, (float)0.1},
  {"",    "SteamOut",   ioidSteamOut, LIO_Out0,    nc_MLnk, 1,  1,  IOGRP(1)|IOPipeEntry|IOApertureHoriz|IOChgFracHgt|IOChgAperture, 1.0f},
  {"",    "BlowDown",   ioidBlowDown, LIO_Out ,    nc_MLnk, 0,  1,  IOGRP(1)|IOPipeEntry|IOChgFracHgt|IOChgAperture, (float)0.5},
  SPILL2AREA,
  VENT2AREA,
  {NULL}}; //This lists the areas of the model where links can be attached.


XID xid_DegSuperHeat  = MdlBsXID(4505);
XID xid_TotalHeatRqd  = MdlBsXID(4506);
XID xid_FuelEnergyRqd = MdlBsXID(4507);

static double Drw_Boiler[] =
  {
  DD_Poly, 5,10, 5,-10, -5,-10, -5,10, 5,10,
    DD_Arc, 0, 4, 3,
    DD_Arc, 0, -4, 3,
    DD_End
  };

//--------------------------------------------------------------------------

IMPLEMENT_MODELUNIT(Boiler, "Boiler", "", Drw_Boiler, "HeatTransfer", "BL", TOC_PROBAL|TOC_GRP_ENERGY|TOC_HEATEXTRA,
                    "Heat Transfer:Boiler",
                    "Simple Boiler." );

static MInitialiseTest InitTest(&BoilerClass);
SPECIEBLK_L(InitTest, H2OLiq, "H2O(l)", true);
SPECIEBLK_V(InitTest, H2OVap, "H2O(g)", true);

Boiler::Boiler(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
MN_Surge(pClass_, TagIn, pAttach, eAttach),
FTC(this)
  {
  AttachIOAreas(BoilerIOAreaList, &PipeEntryGroup);
  Contents.SetClosed(False);

  bOnLine = 1;
  bShowQFeed = 0;
  dFinalTRqd = C_2_K(350.0);
  dFinalPRqd = 4000.0;
  dDrumPress = dFinalPRqd * 1.1;
  dBlowDownFrac = 0.05;
  dEffFrac   = 0.9;
  dDrumTemp = Std_T;
  dBlowDownQm = 0.0;
  dBlowDownHf = 0.0;
  dBlowDownHs = 0.0;
  dHeat1 = 0.0;
  dHeat2 = 0.0;
  dFinalP    = Std_P;
  dFinalT    = Std_T;
  dSatTOut   = Std_T;

  m_VLE.Open(NULL, true);

  //RegisterMacroMdlNode(CMMFlashTrain::MMIOs, &BoilerClass, ioidSI_Steam, mmio_MODEL, &typeid(CFT_Condenser));
  }

//--------------------------------------------------------------------------

Boiler::~Boiler()
  {
  }

//--------------------------------------------------------------------------

void Boiler::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);

  DDB.Visibility(NM_Dynamic|SM_All|HM_All);
  BuildDataDefnElevation(DDB);
  DDB.Visibility();
  DDB.Text    ("");
  DDB.Text    ("Requirements");
  DDB.CheckBox("On",        "",  DC_,     "",      &bOnLine,    this, isParm|SetOnChange);
  DDB.Double  ("FinalTRqd", "",  DC_T,    "C",     &dFinalTRqd, this, isParm);
  DDB.Double  ("FinalPRqd", "",  DC_P,    "kPag",  &dFinalPRqd, this, isParm);
  DDB.Double  ("DrumP",     "",  DC_P,    "kPag",  &dDrumPress, this, isParm);
  DDB.Double  ("BlowDown",  "",  DC_Frac, "%",     &dBlowDownFrac, this, isParm);
  DDB.Double  ("Efficiency","",  DC_Frac, "%",     &dEffFrac,   this, isParm);
  DDB.Visibility(NM_Probal|SM_All|HM_All);
  DDB.CheckBox("ShowQFeed",         "",  DC_,     "",      &bShowQFeed,         this, isParm|SetOnChange);
  //DDB.Visibility(NM_Probal|SM_All|HM_All, !bVentConnected);
  //DDB.CheckBox("VentExcessVapour",  "",  DC_,     "",      &bRemoveExcessVapour,this, isParm|SetOnChange);
  DDB.Visibility();
  //DDB.CheckBox("TrackSteamFd",      "",  DC_,     "",      &bTrackSteamStatus,  this, isParm|SetOnChange);
  //RB.Add_OnOff(DDB, False);
  //EHX.Add_OnOff(DDB, False);

  DDB.Text    ("");
  DDB.Text    ("Results");
  DDB.Double  ("DrumT",     "",  DC_T,    "C",     &dDrumTemp,  this, isResult);
  //DDB.Double  ("TRise",     "",  DC_dT,   "C",     &dTRise,     this, isResult|0);
  DDB.Double  ("HeatRqd1",  "",  DC_Pwr,  "kW",    &dHeat1,     this, isResult);
  DDB.Double  ("HeatRqd2",  "",  DC_Pwr,  "kW",    &dHeat2,     this, isResult);
  DDB.Double  ("TtlHeatRqd","",  DC_Pwr,  "kW",    xid_TotalHeatRqd, this, isResult|noFile|noSnap);
  DDB.Double  ("FuelEnergyRqd","",  DC_Pwr, "kW",  xid_FuelEnergyRqd, this, isResult|noFile|noSnap);
  DDB.Double  ("BlowDownQm","",  DC_Qm,   "kg/s",  &dBlowDownQm,this, isResult);
  DDB.Double  ("FinalP",    "",  DC_P,    "kPag",  &dFinalP,    this, isResult);
  DDB.Double  ("FinalT",    "",  DC_T,    "C",     &dFinalT,    this, isResult);
  DDB.Double  ("SatT",      "",  DC_T,    "C",     &dSatTOut,   this, isResult|noFile|noSnap);
  DDB.Double  ("DegSuperHeat","",DC_dT,   "C",     xid_DegSuperHeat,this, isResult|noFile|noSnap);

  DDB.Visibility(NM_Dynamic|SM_All|HM_All);
  AddMdlClosed(DDB);
  AddMdlNetworked(DDB);
  DDB.Text    ("");
  DDB.Visibility();

  DDB.Text    ("");
  BuildDataDefnShowIOs(DDB);
  BuildDataDefnIOOpts(DDB);

  //RB.BuildDataDefn(DDB);
  //EHX.BuildDataDefn(DDB);

  if (bShowQFeed && NetProbalMethod())
    DDB.Object(&m_QFeed, this, NULL, NULL, DDB_RqdPage);

  if (NetDynamicMethod())
    {
    DDB.Object(&Contents, this, NULL, NULL, DDB_RqdPage);
    DDB.Object(&m_PresetImg, this, NULL, NULL, DDB_RqdPage);
    }

  DDB.EndStruct();
  }

//--------------------------------------------------------------------------

flag Boiler::DataXchg(DataChangeBlk & DCB)
  {
  if (MN_Surge::DataXchg(DCB))
    return 1;

  switch (DCB.lHandle)
    {
    case xid_DegSuperHeat:
      DCB.D = dFinalT-dSatTOut;
      return 1;
    case xid_TotalHeatRqd:
      DCB.D=dHeat1+dHeat2;
      return 1;
    case xid_FuelEnergyRqd:
      DCB.D=(dHeat1+dHeat2)/GTZ(dEffFrac);
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

flag Boiler::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=MN_Surge::ValidateData(VDB);
  //dFinalTRqd=ValidateRange(VDB, "FinalTRqd", 20.0, dFinalTRqd, IF97_MaxSatT);
  return OK;
  }

//---------------------------------------------------------------------------

void Boiler::ConfigureJoins()
  {
  for (int i=0; i<NoProcessIOs(); i++)
    SetIO_Join(i, IOId_Self(i)==ioidBlowDown ? 1 : 0);
    //SetIO_Join(i, 0);
  }

//--------------------------------------------------------------------------

void Boiler::EvalJoinPressures(long JoinMask)
  {
  if (NoFlwIOs()>0)
    {
    switch (NetMethod())
      {
      case NM_Probal:
        {
        for (int j=0; j<NoProcessJoins(); j++)
          {
          SetJoinPressure(j, (j==0 ? dFinalP : dDrumPress));
          }
        break;
        }
      case NM_Dynamic:
        MdlNode::EvalJoinPressures(JoinMask);
        break;
      }
    }
  }

//--------------------------------------------------------------------------

void   Boiler::SetState(eScdMdlStateActs RqdState)
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

void Boiler::EvalProducts(CNodeEvalIndex & NEI)
  {
  if (NoFlwIOs()==0)
    return;
  switch (SolveMethod())
    {
    case SM_Direct:
      {
      dBlowDownFrac = Range(0.0, dBlowDownFrac, 1.0);
      dEffFrac = Max(0.01, dEffFrac);

      const int IOFeedLiq  = IOWithId_Self(ioidFeedLiq);
      const int IOBlowDown = IOWithId_Self(ioidBlowDown);
      const int IOSteamOut = IOWithId_Self(ioidSteamOut);

      StkSpConduit Mixture("Mixture", chLINEID(), this);

      SpConduit & SO = *(IOConduit(IOSteamOut));
      SpConduit * pBD = (IOBlowDown>=0 ? IOConduit(IOBlowDown) : NULL);

      double Pi = SigmaQInPMin(Mixture(), som_ALL, Id_2_Mask(ioidFeedLiq));

      //VLE.SetHfInAtZero(Mixture());

      if (bShowQFeed)
        m_QFeed.QCopy(Mixture());

      const int si = H2OVap();
      const int wi = H2OLiq();
      double FeedLiqQ = Mixture().QMass();

      if (FeedLiqQ>UsableMass && bOnLine)
        {
        const double hi = Mixture().totHf();
        dDrumTemp = Mixture().SaturationT(dDrumPress);
        Mixture().SetPress(dDrumPress);
        Mixture().Set_totHf(hi);
        double dDrumTemp_ = Mixture().SaturationT(dDrumPress);
        Mixture().SetTemp(dDrumTemp);
        const double h1 = Mixture().totHf();
        dHeat1 = (h1-hi);

        if (1)
          {
          if (pBD)
            {
            SpConduit & BD = *(pBD);
            const double FeedSlurryQ = Mixture().QMass(som_SL);
            const double FeedWaterQ = Mixture().VMass[wi];
            double WaterSplit = Max(0.0, (FeedSlurryQ*dBlowDownFrac - (FeedSlurryQ-FeedWaterQ))/GTZ(FeedWaterQ));

            BD.QSetF(Mixture(), som_ALL, 0.5, dDrumPress); //IMPORTANT: copy something to get qualities, etc correct
            SO.QSetF(Mixture(), som_ALL, 0.5, dDrumPress); //IMPORTANT: copy something to get qualities, etc correct
            for (int i=0; i<SDB.Count(); i++)
              {
              if (SDB[i].IsGas())
                {
                BD.SetVMass(i, Mixture(), 0.0);
                SO.SetVMass(i, Mixture(), Mixture().VMass[i]);
                }
              else if (SDB[i].IsSol())
                {
                BD.SetVMass(i, Mixture(), Mixture().VMass[i]);
                SO.SetVMass(i, Mixture(), 0.0);
                }
              else
                {
                if (i==wi)
                  {
                  BD.SetVMass(i, Mixture(), (Mixture().VMass[i]*WaterSplit));
                  SO.SetVMass(i, Mixture(), Mixture().VMass[i]*(1.0-WaterSplit));
                  }
                else
                  {
                  BD.SetVMass(i, Mixture(), Mixture().VMass[i]);
                  SO.SetVMass(i, Mixture(), 0.0);
                  }
                }
              }
            BD.SetTempPress(dDrumTemp, dDrumPress);
            SO.SetTempPress(dDrumTemp, dDrumPress);
            const double h1_ = SO.totHf();
            /*const double h1_bd = BD.totHf();
            const double htot = h1_+h1_bd;
            const double err = htot-h1;*/
            SO.SetPress(dFinalPRqd);
            m_VLE.SetHfInAtZero(SO);
            m_VLE.SetFlashVapFrac(SO, 1.0, 0);
            m_VLE.AddHfOutAtZero(SO);
            SO.SetTemp(dFinalTRqd);
            const double h2 = SO.totHf();
            dHeat2 = (h2-h1_);
            }
          else
            {//blowdown not present
            SO.QSetF(Mixture(), som_ALL, 1.0, dDrumPress); //IMPORTANT: copy something to get qualities, etc correct
            SO.SetTempPress(dDrumTemp, dDrumPress);
            const double h1_ = SO.totHf();
            SO.SetPress(dFinalPRqd);
            m_VLE.SetHfInAtZero(SO);
            m_VLE.SetFlashVapFrac(SO, 1.0, 0);
            m_VLE.AddHfOutAtZero(SO);
            SO.SetTemp(dFinalTRqd);
            const double h2 = SO.totHf();
            dHeat2 = (h2-h1_);
            }
          }
        //else
        //  {
        //  dBlowDownQm = FeedLiqQ*dBlowDownFrac;
        //  BD.QSetF(Mixture(), som_ALL, dBlowDownFrac, dDrumPress);
        //  SO.QSetF(Mixture(), som_ALL, 1.0-dBlowDownFrac, dFinalPRqd);
        //  VLE.SetHfInAtZero(SO);
        //  VLE.SetFlashVapFrac(SO, 1.0, 0);
        //  VLE.AddHfOutAtZero(SO);
        //  SO.SetTemp(dFinalTRqd);
        //  const double h2 = SO.totHf();
        //  dHeat2 = (h2-h1*(1.0-dBlowDownFrac));
        //  }

        }
      else
        {
        dHeat1 = 0.0;
        dHeat2 = 0.0;
        if (FeedLiqQ>UsableMass)
          {
          dDrumPress = Mixture().Press();
          if (pBD)
            {
            pBD->QSetF(Mixture(), som_SL, 1.0, dDrumPress);
            SO.QSetF(Mixture(), som_Gas, 1.0, dDrumPress);
            }
          else
            SO.QSetF(Mixture(), som_ALL, 1.0, dDrumPress);
          }
        else
          {
          SO.QCopy(Mixture());
          if (pBD)
            pBD->QZero();
          }
        }

      dBlowDownQm = (pBD ? pBD->QMass() : 0.0);
      dBlowDownHf = (pBD ? pBD->totHf() : 0.0);
      dBlowDownHs = (pBD ? pBD->totHs() : 0.0);
      dFinalT    = SO.Temp();
      dFinalP    = SO.Press();
      dSatTOut   = SO.SaturationT(dFinalP);
      const double BlowDownWaterQm = (pBD ? pBD->VMass[wi] : 0.0);
      SetCI(1, bOnLine && dBlowDownQm>1.0e-12 && BlowDownWaterQm<1.0e-12);

      }
    break;
    default:
      MN_Surge::EvalProducts(NEI);
    }
  }

//--------------------------------------------------------------------------

void Boiler::ClosureInfo()
  {
  MN_Surge::ClosureInfo();
  if (m_Closure.DoFlows())
    {
    CClosureInfo &CI0=m_Closure[0];
    CI0.m_PowerIn+=dHeat1+dHeat2;
    CI0.m_HfGainAtZero=m_VLE.HfGainAtZero();
    CI0.m_MassLoss+=dBlowDownQm;
    CI0.m_HfLoss+=dBlowDownHf;
    CI0.m_HsLoss+=dBlowDownHs;
    if (m_Closure.GetCount()>1)
      {
      CClosureInfo &CI1=m_Closure[1];
      CI1.m_MassGain+=dBlowDownQm;
      CI1.m_HfGain+=dBlowDownHf;
      CI1.m_HsGain+=dBlowDownHs;
      }
    }
  }

//--------------------------------------------------------------------------

flag Boiler::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case 1: pS="W\tNo water in blowdown";                                return 1;
    case 2: pS="W\tNo Steam Supplied";                                   return 1;
    case 3: pS="W\tNo Vent Defined";                                     return 1;
    case 4: pS="W\tTemperature Required too high based on feed streams"; return 1;
    case 5: pS="W\tExpect pure steam in steam line";                     return 1;
    default:
      return MN_Surge::CIStrng(No, pS);
    }
  }

//==========================================================================

