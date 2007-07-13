//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992 - 1996
 
#include "stdafx.h"
#include "sc_defs.h"
#define  __CONTCTHT_CPP
#include "contctht.h"
//#include "optoff.h"

#define WithPressureOption 0

//==========================================================================
/*#D:#T:Direct Contact Heater(1)
#X:#h<General Description>#n
The steam injection heater is a direct contact heater.
The unit has separate liquor and steam inlets, a hot liquor  and a steam outlet.
#nThe model mixes the inlet streams, carries out any chemical reactions specified 
by the user before calculating the amount of steam which must condense to obtain 
the required temperature.  The hot liquor and the excess steam exit at the equilibrium 
pressure of the unit.
#nIf there is insufficient steam to obtain the required temperature, all of the 
available steam is condensed.
#nIf the direct contact heater is inserted as part of a flash train, then the unit will 
calculate the amount of steam required to obtain the user defined temperature.  In this 
case, there will be no steam exiting from the excess steam line.
#n#n
#n#u<#l<#b<Inputs and Outputs>>>#n#w
Label    Input/Output  Minimum number  Maximum number#n
liquor       Input          1              10#n
steam        Input          1              1#n
hot liquor   Output         1              1#n
excess steam Output         1              1#n
#a
To function the direct contact heater must have all of the above streams connected.

#n#n#h<Variables to be supplied by the user>#n
#i<TempSpec>:The method of calculating the final temperature, Temperature Rise or 
Final Temperature.#n
#i<FinalTRqd>:The required temperature of the exiting hot liquor.#n
#i<TRiseRqd>:The required temperature increase between the incoming liquor and the 
exiting hot liquor.

#n#n#h<Variables calculated by the model>#n
#i<TRise>:The calculated Temperature Rise between the entering and exiting liquor 
streams.#n
#i<FinalP>:The operating pressure in the unit.#n
#i<FinalT>:The temperature of the exiting streams.

#n#n#h<Assumptions & Limitations>#n
All the steam condenses when inserted as part of a flash train.

#n#n#h<Chemical Equations>#n
By default, no chemical reactions occur in the steam injector.
Chemical reactions can be specified in a reaction file.
#n#h<Heat Transfer>#n
Heat loss to atmosphere is specified and the model maintains a thermal balance.

#n#h<Other>#n
Default model prefix:SI#n
Short name:ContctHt#n
Model type:Unit#n
#G:Units
*/
//==========================================================================

const byte ioidSI_Liq         = 0;
const byte ioidSI_Steam       = 1;
const byte ioidSI_HotLiq      = 2;
const byte ioidSI_ESteam      = 3;

static IOAreaRec ContactHeaterIOAreaList[] =
  {{"Liquor",      "Liquor",     ioidSI_Liq ,   LIO_In0 ,    nc_MLnk, 1,  10, IOGRP(1)|IOPipeEntry|IOChgFracHgt|IOChgAperture|IOApertureHoriz, 1.0f},
   {"Steam",       "Steam"  ,    ioidSI_Steam,  LIO_In1 ,    nc_MLnk, 1,  1,  IOGRP(1)|IOPipeEntry|IOChgFracHgt|IOChgAperture, 0.5f},
   {"Hot Liquor",  "HotLiquor",  ioidSI_HotLiq, LIO_Out0,    nc_MLnk, 1,  1,  IOGRP(1)|IOPipeEntry|IOChgFracHgt|IOChgAperture, 0.5f},
   {"Excess Steam","ExcessSteam",ioidSI_ESteam, LIO_Out1,    nc_MLnk, 0,  1,  IOGRP(1)|IOPipeEntry|IOChgFracHgt|IOChgAperture, 0.98f},
   SPILL2AREA,
   VENT2AREA(-1),
   {NULL}}; //This lists the areas of the model where links can be attached.


static double Drw_ContactHeater[] ={DD_Poly, -5,7, -5,-7, 5,-7, 5,7, -5,7,
                                 DD_Arc3, -5,7, 0,10, 5,7,
                                 DD_Arc3, -5,-7, 0,-10, 5,-7,
                                 DD_End };
/*static double Drw_ContactHeater[] ={DD_Poly, -5,10, -5,-5, 5,-5, 5,10, -5,10,
                                 DD_Arc3, -5,10, 0,13, 5,10,
                                 DD_Arc3, -5,-5, 0,-8, 5,-5,
                                 DD_End };*/

//--------------------------------------------------------------------------

IMPLEMENT_MODELUNIT(ContactHeater, "ContctHt", "1", Drw_ContactHeater, "HeatTransfer", "SI", TOC_ALL|TOC_GRP_ENERGY|TOC_HEATBAL,
//                    "HX - Direct Contact Heater(1)",
                    "Heat Transfer:Direct Contact Heater(1)",
                    "The direct contact heater model mixes the inlet streams, condenses "
                    "all the steam and carries out any chemical reactions specified before "
                    "rejecting the amount of heat lost and calculating the temperature, "
                    "composition and flow rate of the exit stream." )
                    
//static MInitialiseTest InitTest(&ContactHeaterClass);
//SPECIEBLK_L(InitTest, H2OLiq, "H2O(l)", true);
//SPECIEBLK_V(InitTest, H2OVap, "H2O(g)", true);

ContactHeater::ContactHeater(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MN_Surge(pClass_, TagIn, pAttach, eAttach),
  m_FTC(this)
  {
  AttachIOAreas(ContactHeaterIOAreaList, &PipeEntryGroup);
  Contents.SetClosed(False);

  iRqdCalcMode = QST_Free;
  iTempSpec  = QST_TRise;
  dFinalTRqd = C_2_K(220.0);
  dFinalPRqd = 2000.0;
  dTRiseRqd  = 10.0;
  dTRise     = 0.0;
  dFinalP    = Std_P;
  dFinalT    = Std_T;
  dSatPOut   = Std_P;
  dVentRqd   = 0.0;
  dQmVapVent = 0.0;
  SteamRqd   = dNAN;
  dPrevPress = dNAN;
//xx  dVapFracRqd = dNAN;
  iCalcMode  = QST_General;
  m_bShowQFeed = 0;
  bTrackSteamStatus = 1;
  bRemoveExcessVapour = 1;
  bVentConnected = 0;

  //m_EHX.Open(&CEHX_LossPerQmClass);
  m_VLE.Open(NULL, true);

  RegisterMacroMdlNode(CMMFlashTrain::MMIOs, &typeid(ContactHeater), ioidSI_Steam, mmio_MODEL, &typeid(CFT_Condenser));
  }

//--------------------------------------------------------------------------

ContactHeater::~ContactHeater()
  {
  }

//--------------------------------------------------------------------------

void ContactHeater::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);

  DDB.Visibility(NM_Dynamic|SM_All|HM_All);
  BuildDataDefnElevation(DDB);
  if (SolveSurgeMethod())
    {
    DDB.Visibility(NM_Dynamic|SM_DynBoth|HM_All);
    DDB.Double  ("Pressure",     "P",     DC_P,    "kPag",   xidPMean,       this, isResult|0);
    DDB.Double  ("Temperature",  "T",     DC_T,    "C",      xidTemp,        this, isResult|0);
    DDB.Double  ("Density",      "Rho",   DC_Rho,  "kg/m^3", xidRho,         this, isResult|0);
    DDB.Double  ("Level",        "Lvl",   DC_Frac, "%",      xidLevel,       this, isResult|0);
    DDB.Double  ("Mass_Accum",   "QmAcc", DC_Qm,  "kg/s",    &m_QmAcc,       this, isResult|0);
    DDB.Double  ("Vol_Accum",    "QvAcc", DC_Qv,  "L/s",     &m_QvAcc,       this, isResult|0);
    }
  DDB.Visibility();
  
  static DDBValueLst DDB0[]={
    {QST_Free,  "Free"},
    {QST_ForceDemand, "Steam Demand"},
    {QST_ForceGeneral, "Stand Alone"},
    {0}};
  static DDBValueLst DDB1[]={
    {QST_TRise,  "Temp Rise"},
    {QST_FinalT, "Final Temp"},
#if WithPressureOption
    {QST_FinalP, "Final Pressure"},
#endif
    {0}};
  static DDBValueLst DDB2[]={
    {QST_FTCActive, "Flash Train"},
    {QST_Makup,     "Steam Makeup"},
    {QST_General,   "General"},
    {0}};

  DDB.Text    ("");
  DDB.Text    ("Requirements");
  DDB.Byte    ("RqdCalcMode", "", DC_,    "",      &iRqdCalcMode, this, isParm|AffectsStruct, DDB0);
  DDB.Byte    ("TempSpec",  "",  DC_,     "",      &iTempSpec,  this, isParm|AffectsStruct, DDB1);
  DDB.Visibility(NSHM_All, (iTempSpec == QST_FinalT));
  DDB.Double  ("FinalTRqd", "",  DC_T,    "C",     &dFinalTRqd, this, isParm);
  DDB.Visibility(NSHM_All, (iTempSpec == QST_FinalP));
  DDB.Double  ("FinalPRqd", "",  DC_P,    "kPag",  &dFinalPRqd, this, isParm);
  DDB.Visibility(NSHM_All, (iTempSpec == QST_TRise));
  DDB.Double  ("TRiseRqd",  "",  DC_dT,   "C",     &dTRiseRqd,  this, isParm);
  DDB.Visibility();
  DDB.Double  ("SteamVentRqd", "", DC_Qm, "kg/s",  &dVentRqd,   this, isParm);
  RB.Add_OnOff(DDB);
  m_EHX.Add_OnOff(DDB);
  DDB.Visibility(NM_Probal|SM_All|HM_All);
  DDB.CheckBox("ShowQFeed",         "",  DC_,     "",      &m_bShowQFeed,         this, isParm|SetOnChange);
  DDB.Visibility(NM_Probal|SM_All|HM_All, !bVentConnected);
  DDB.CheckBox("VentExcessVapour",  "",  DC_,     "",      &bRemoveExcessVapour,this, isParm|SetOnChange);
  DDB.Visibility();
  DDB.CheckBox("TrackSteamFd",      "",  DC_,     "",      &bTrackSteamStatus,  this, isParm|SetOnChange);

  DDB.Visibility();
  DDB.Text    ("Results");
  DDB.Double  ("TRise",     "",  DC_dT,   "C",     &dTRise,     this, isResult|0);
  DDB.Double  ("FinalP",    "",  DC_P,    "kPag",  &dFinalP,    this, isResult|NAN_OK);
  DDB.Double  ("FinalT",    "",  DC_T,    "C",     &dFinalT,    this, isResult|NAN_OK);
  DDB.Double  ("SatP",      "",  DC_P,    "kPag",  &dSatPOut,   this, isResult|noFile|noSnap|InitHidden);
  DDB.Visibility(NM_Probal|SM_All|HM_All);
  DDB.Double  ("VapQmOut",  "",  DC_Qm,   "kg/s",  &dQmVapVent, this, isResult|noFile|noSnap|InitHidden);
  DDB.Byte    ("CalcMode",  "",  DC_,     "",      &iCalcMode,  this, isResult/*|InitHidden*/, DDB2);
  DDB.Visibility();
  DDB.Double  ("PrevP",             "",  DC_P,   "kPa",  &dPrevPress,            this, isResult|noView);
//xx  DDB.Double  ("PrevVapFrac",       "",  DC_Frac, "",    &dVapFracRqd,           this, isResult|noView);
  DDB.Text    ("");
  
  //NB: during a project load or save m_FTC.Active() is ALWAYS false!!!
  //dword VFlags=m_FTC.Active() ? 0 : noView|noFile|noSnap; 

  m_FTC.BuildDataDefn(DDB);
  //dword VFlags=m_FTC.Active() ? 0 : noView; 
  //DDB.Double  ("QRqd",              "",  DC_Qm,   "kg/s",  &m_FTC.dQRqd,           this, isResult|VFlags);
  //DDB.Double  ("QCond",             "",  DC_Qm,   "kg/s",  &m_FTC.dQCond,          this, isResult|VFlags);
  //DDB.Double  ("SuctionP",          "",  DC_P,    "kPa",   &m_FTC.dSuctionP,       this, isResult|noView|VFlags);
  //DDB.Double  ("MinSatPress",       "",  DC_P,    "kPa",   &m_FTC.dMinSatPress,    this, isResult|VFlags);

  //if (m_FTC.Active())
  //  {
  //  DDB.String  ("FlashTrain",        "",  DC_,     "",      &m_FTC.pMNd->TagRef(), this, isResult|noFile|noSnap|VFlags);
  //  DDB.String  ("FlashTrainEqp",     "",  DC_,     "",      m_FTC.MMList(),        this, isResult|MultiLineStr|noFile|noSnap|VFlags);
  //  DDB.Text    ("");
  //  }

  DDB.Visibility(NM_Dynamic|SM_All|HM_All);
  AddMdlClosed(DDB);
  AddMdlNetworked(DDB);
  DDB.Visibility();

  DDB.Text    ("");
  BuildDataDefnShowIOs(DDB);
  BuildDataDefnIOOpts(DDB);

  RB.BuildDataDefn(DDB);
  m_EHX.BuildDataDefn(DDB);

  if (m_bShowQFeed && NetProbalMethod())
    DDB.Object(&m_QFeed, this, NULL, NULL, DDB_RqdPage);

  if (NetDynamicMethod())
    {
    DDB.Object(&Contents, this, NULL, NULL, DDB_RqdPage);
    DDB.Object(&m_PresetImg, this, NULL, NULL, DDB_RqdPage);
    }

  DDB.EndStruct();
  }

//--------------------------------------------------------------------------

flag ContactHeater::DataXchg(DataChangeBlk & DCB)
  {
  if (MN_Surge::DataXchg(DCB)) 
    return 1;

  switch (DCB.lHandle)
    {
    case xidClosed:
      if (DCB.rB)
        Contents.SetClosed(*DCB.rB, DCB.ForView()); 
      DCB.B=Contents.Closed(); 
      return 1;
    }
  return 0;
  }

//---------------------------------------------------------------------------

flag ContactHeater::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=MN_Surge::ValidateData(VDB);
  dTRiseRqd=ValidateRange(VDB, "TRiseRqd", 0.0, dTRiseRqd, 500.0);
  dFinalTRqd=ValidateRange(VDB, "FinalTRqd", 20.0, dFinalTRqd, IF97_MaxSatT);
  dFinalPRqd=ValidateRange(VDB, "FinalPRqd", 2.0, dFinalPRqd, IF97_MaxSatP);
  return OK;
  }

//--------------------------------------------------------------------------

void ContactHeater::EvalJoinPressures(long JoinMask)
  {
  if (NoFlwIOs()>0)
    switch (NetMethod())
      {
      case NM_Probal:
        {
        //if (!m_FTC.Active())
        //  m_FTC.SetSuctionP(IOP_Rmt(IOWithId_Self(ioidSI_HotLiq)));what does this do for us?
        if (NoProcessJoins()>0)
          {
          //m_FTC.SetSuctionP(Min(dFinalP, MeasureJoinPressure(0)));
          m_FTC.SetSuctionP(dFinalP);
//          m_FTC.SetSuctionP(MeasureJoinPressure(0));
          SetJoinPressure(0, m_FTC.SuctionP());
          }
        }
        break;
      case NM_Dynamic:
        MdlNode::EvalJoinPressures(JoinMask);
        break;
      }
  }
 
//--------------------------------------------------------------------------

void   ContactHeater::SetState(eScdMdlStateActs RqdState)
  {
  MN_Surge::SetState(RqdState);
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

void ContactHeater::EvalJoinFlows(int JoinNo)
  {
  if (NoFlwIOs()>0)
    switch (NetMethod())
      {
      case NM_Probal:
      case NM_Dynamic:
        break;
      }
  };
 
//--------------------------------------------------------------------------

class FinalTempFnd : public MRootFinderBase
  {
  public:
    SpConduit &HL, &Liq, &Vp;
    double    LP,VentRqd;
    double    VQm;
    ContactHeater &CH;
    static CToleranceBlock s_Tol;
    
    FinalTempFnd(SpConduit &HotLiquor, SpConduit &Vapour, SpConduit &Liquor, double LiquorP, double VentQmRqd, ContactHeater &ContactH);
    double  Function(double x);
  };

CToleranceBlock FinalTempFnd::s_Tol(TBF_Both, "ContactHt:FinalTempFnd", 0.0, 1.0e-9);

//--------------------------------------------------------------------------

FinalTempFnd::FinalTempFnd(SpConduit &HotLiquor, SpConduit &Vapour, SpConduit &Liquor, double LiquorP, double VentQmRqd, ContactHeater &ContactH):
  MRootFinderBase("Heater", s_Tol),//1e-9),
  HL(HotLiquor),
  Vp(Vapour),
  Liq(Liquor),
  CH(ContactH)
  {
  LP      = LiquorP;
  VentRqd = VentQmRqd;
  VQm     = Vp.QMass();
  }

//--------------------------------------------------------------------------

double FinalTempFnd::Function(double x)
  {
  HL.QSetF(Liq, som_ALL, 1.0, LP);
  HL.QAddM(Vp, som_ALL, Max(VQm, x));
  CH.RB.EvalProducts(HL);
  CH.m_EHX.EvalProducts(HL);
  const double RqdFrac = (VentRqd>UsableMass ? VentRqd/GTZ(HL.VMass[CH.m_VLE.FlashVapIndex()/*H2OVap()*/]+HL.VMass[CH.m_VLE.FlashLiqIndex()/*H2OLiq()*/]) : 0.0);
  const double f = Max(RqdFrac, 1.0-Min(1.0, x/GTZ(VQm)));
  CH.m_VLE.SetFlashVapFrac(HL, f, 0);
  double dFinalT = HL.Temp();
  #ifndef _RELEASE
  double P = HL.Press();
  double SP = HL.SaturationP(dTarget);
  #endif
  return dFinalT;
  }

//--------------------------------------------------------------------------
  
void ContactHeater::EvalProducts(CNodeEvalIndex & NEI)
  {
  if (NoFlwIOs()==0)
    return;
  switch (SolveMethod())
    {
    case SM_Direct:
      {
      dVentRqd = Max(0.0, dVentRqd);
      const int IOSteam = IOWithId_Self(ioidSI_Steam);//Steam feed
      const int IOESteam = IOWithId_Self(ioidSI_ESteam);//Excess Steam vent
      const int IOLiqIn = IOWithId_Self(ioidSI_Liq);//Liquor feed

      StkSpConduit Mixture("Mixture", chLINEID(), this);
      StkSpConduit Liquor("Liquor", chLINEID(), this);
      StkSpConduit Vapour("Vapour", chLINEID(), this);
      
      SpConduit & HotLiquor = *(IOConduit(IOWithId_Self(ioidSI_HotLiq)));
      
      double Pi = SigmaQInPMin(Mixture(), som_ALL, Id_2_Mask(ioidSI_Liq)|Id_2_Mask(ioidSI_Steam));
      SigmaQInPMin(Liquor(), som_ALL, Id_2_Mask(ioidSI_Liq));
      SigmaQInPMin(Vapour(), som_ALL, Id_2_Mask(ioidSI_Steam));

      if (m_bShowQFeed)
        m_QFeed.QCopy(Mixture());

      bVentConnected = (IOESteam>=0);
      const int si = m_VLE.FlashVapIndex()/*H2OVap()*/;
      double LiquorQ      = Liquor().QMass();
      double LiquorQVap   = Liquor().QMass(som_Gas); //vapour in liquor feed
      //double LiquorP      = Liquor().Press();//this is "WRONG" because of PMin used!
      double LiquorT      = Liquor().Temp();
      double VentQ        = (bVentConnected ? IOConduit(IOESteam)->QMass() : 0.0);
      double VentSteamQ   = (bVentConnected ? IOConduit(IOESteam)->VMass[si] : 0.0);
      double VapourQ      = Vapour().QMass();
      double VapourQVap   = Vapour().QMass(som_Gas);
      double VapourSteamQ = Vapour().VMass[si];
      //double VapourP   = Vapour().Press();//this is "WRONG" because of PMin used!
      //double VapourT   = Vapour().Temp();//this is "WRONG" because of PMin used!
      ASSERT(IOSteam>=0);
      SpConduit & QSteamIn = *IOConduit(IOSteam);
      double VapourT = QSteamIn.Temp();
      double VapourP = QSteamIn.Press();
      SpConduit & QLiqIn = *IOConduit(IOLiqIn);
      double LiquorP = QLiqIn.Press();
      double LiquorT_ = QLiqIn.Temp();

      //const flag HasSteam  = (Vapour().Qm(si)>1.0e-6);
      const flag HasSteam  = (VapourQ>1.0e-6);
      const flag HasFeed   = (Mixture().QMass()>1.0e-6);
      const flag HasLiquor = (LiquorQ>1.0e-6);
      SetCI(2, HasLiquor && !HasSteam);

      SetCI(5, bTrackSteamStatus && HasSteam && (VapourSteamQ/VapourQ)<0.9999); //expect 100% steam in steam line

      dQmVapVent = 0.0;
      double RqdProdTemp;
      CFlange &StmFl=*IOFlange(IOSteam);
      bool GeneralCalc = (iRqdCalcMode==QST_ForceGeneral);
      if (iRqdCalcMode==QST_Free)
        {
        GeneralCalc = !(m_FTC.Active() || StmFl.IsMakeUpAvail());
        }
      const bool DemandErr = (!GeneralCalc && !m_FTC.Active() && !StmFl.IsMakeUpAvail());
      SetCI(6, DemandErr);
      if (DemandErr)
        GeneralCalc = true; //Force general calc

      if (!GeneralCalc)
        {
        iCalcMode = (byte)(m_FTC.Active() ? QST_FTCActive : QST_Makup);
        SetCI(2, HasLiquor && (VapourSteamQ<1.0e-6));

        m_FTC.m_dQCond = VapourQ;//VapourQVap;//VapourSteamQ

        RqdProdTemp = (iTempSpec==QST_TRise) ? Min(LiquorT+dTRiseRqd, IF97_MaxSatT) : dFinalTRqd;

        if (HasSteam || HasFeed)
          {
          double RqdTemp = RqdProdTemp;

          // Assume m_FTC.m_dQRqd is approximately correct
          m_FTC.m_dQRqd=Max(0.0, m_FTC.m_dQRqd);

          // Required Temp cannot go higher than Steam Saturated Temperature
          const double SatT = Vapour().SaturationT(VapourP);
          const double RqdT = Range(ZeroCinK, RqdTemp, max(SatT, LiquorT)); //ensure the target temp is **reasonable**
          const double LiqSatP = Liquor().SaturationP(RqdT);
          const double VapSatP = Vapour().SaturationP(RqdT);
          const double P_ = Vapour().SaturationP(LiquorT);
          dFinalP = LiqSatP; //a "reasonable" guess
          //dFinalP = Max(dFinalP, P_); //ensure the target pressure is **reasonable**
          //dFinalP = Max(2.0/*LiquorP*//*dFinalP*/, P_); //ensure the target pressure is **reasonable**
          double VentRqd = dVentRqd;//(bVentConnected ? dVentRqd : 0.0);
          SetCI(4, RqdT<RqdTemp);

          double EstFinalP = Valid(dPrevPress) ? Range(dFinalP-20.0, dPrevPress, dFinalP+20.0) : dFinalP;
          //double EstFinalP = dFinalP;
          FinalTempFnd FTF(Mixture(), Vapour(), Liquor(), EstFinalP, VentRqd, *this);

          FTF.SetTarget(RqdT);
          if (Valid(SteamRqd))
            {
            FTF.SetEstimate(SteamRqd, 1.0);
            SteamRqd = dNAN;
            }
          flag Ok = false;
          double MaxQStm=Max(1.1*(LiquorQ+VapourQ-VentQ), 0.1);
          int iRet=FTF.Start(0.0, MaxQStm);
          if (iRet==RF_EstimateOK) //estimate is good, solve not required
            {
            SteamRqd = FTF.Result();
            Ok = true;
            }
          else
            {
            if (iRet==RF_BadEstimate)
              iRet = FTF.Start(0.0, MaxQStm); // Restart
            if (iRet==RF_OK)
              if (FTF.Solve_Brent()==RF_OK)
                {
                SteamRqd = FTF.Result();
                Ok = true;
                }
            }
          if (Ok)
            {//now call function with ACTUAL VQm
            //doing this causes BIG stability errors, in some cases (eg Nickel demo project)...
            //double T = FTF.Function(VapourQ);
            dFinalP = Mixture().Press(); //determine ACTUAL finalP
            }
          double ExtraVapourQ = Mixture().QMass(som_Gas);
          //m_FTC.dQCond = Max(0.0, VapourQ+LiquorQVap-ExtraVapourQ); //assumes no vapour consumed/generated in RB !!!
          if (Ok)
            {//If required, adjust SteamRqd to get correct steam vent...
            //double ExcessVap = Mixture().QMass(som_Gas);
            const double exsteam = Mixture().VMass[si];
            const double diff = exsteam-VentRqd;
            if (diff>1.0e-12)
              {
              if (iCalcMode==QST_Makup)
                SteamRqd = Max(SteamRqd - (diff * 0.95), 0.0);
              else
                SteamRqd = Max(SteamRqd - (diff * 0.80), 0.0);
              }
            m_FTC.m_dQRqd = SteamRqd;
            }
          else
            {
            // if a problem just add
            m_FTC.m_dQRqd = 0.1;
            }

          //TODO TODO: Improve Contact Heater convergance...
          //1) Fix for bad mass balance after first solved!")
          //   eg change SteamVentRequired significantly, then press solve, solves with balance error 
          //   (ie m_FTC.dQRqd delay/missmatch!?!?!) is OK after pressing solve again!
          //2) Perhaps should be "loop within loop" to meet temperature and vent requirement efeciently?!?

          //=========================================================
  
          #if dbgModels   
          if (dbgFlashTrain())
            dbgpln("BC: Est:%10.4f MxFl:%10.4f", m_FTC.dQRqd, QRqdEst);    
          #endif
          }
        else
          {//no steam suplied
          dFinalP=LiquorP;
          m_FTC.m_dQRqd=0.0;
          Mixture().QSetF(Liquor(), som_ALL, 1.0, dFinalP);
          }
        m_FTC.m_dQRqd=Max(0.0, m_FTC.m_dQRqd);
        // Mixture now flashed

        //if (!m_FTC.Active())
          StmFl.SetMakeUpReqd(m_FTC.m_dQRqd);
        
        dQmVapVent = Mixture().QMass(som_Gas);
        if (bVentConnected)
          {
          HotLiquor.QSetF(Mixture(), som_SL, 1.0, dFinalP);
          SpConduit& ESteam = *(IOConduit(IOESteam));
          ESteam.QSetF(Mixture(), som_Gas, 1.0, dFinalP);
          ClrCI(3);
          }
        else
          {
          //Where should Excess Steam go?
          if (m_FTC.Active())
            HotLiquor.QSetF(Mixture(), som_SL, 1.0, dFinalP); //throw excess steam (and vapours) away
          else
            HotLiquor.QSetF(Mixture(), bRemoveExcessVapour ? som_SL : som_ALL, 1.0, dFinalP);
          if (dVentRqd>0.0)
            SetCI(3, true);
          else if (!m_FTC.Active())
            SetCI(3, m_FTC.m_dQRqd<0.999999*VapourQ);
          }
        }
      else
        {
        iCalcMode = QST_General;
        //SetCI(2, (Mixture().Qm(si)<1.0e-6));???
        RB.EvalProducts(Mixture());
        m_EHX.EvalProducts(Mixture());

        double TPress = LiquorP;
        RqdProdTemp = (iTempSpec==QST_TRise) ? LiquorT+dTRiseRqd : dFinalTRqd;

        if (HasSteam)
          {
          double RqdTemp = RqdProdTemp;

          TPress = Mixture().SaturationP(RqdTemp);
          m_VLE.PFlash(Mixture(), TPress, 0.0, 0);
          double Tmp = Mixture().Temp();
          TPress = Mixture().SaturationP(RqdTemp); //new SaturationP after Flash
          double exsteam = Mixture().VMass[si];

          if (Tmp > RqdTemp || exsteam > MeasTolerance)  // If there is enough steam, find the equilibrium
            {
            double RqdPure = Mixture().PureSaturationP(RqdTemp);
            //TPress should be less than RqdPure, if not then probably an error in SaturationP at RqdTemp for current specie model
            ASSERT(TPress<=RqdPure); //something may be wrong...
            //TPress = (TPress + RqdPure)/2.0; //guess
            if (Valid(dPrevPress))
              TPress = Range(TPress-20.0, dPrevPress, RqdPure); //use previous value?
            const int MxIter=20;
            for (int iIter=0; iIter<MxIter; iIter++)
              {
              m_VLE.PFlash(Mixture(), TPress, 0.0, 0);
              Tmp = Mixture().Temp();
              double TmpDiff = RqdTemp - Tmp;
              if (fabs(TmpDiff) < 1.0e-5)
                {
                TPress = Mixture().SaturationP(RqdTemp);
                iIter = MxIter;
                }
              else
                {
                exsteam = Mixture().VMass[si];
                if (Tmp > RqdTemp || exsteam > MeasTolerance)
                  {//try again...
                  TPress = Mixture().SaturationP(RqdTemp);
                  //TPress -= (TPress * 1.0e-5); //reduce pressure to converge faster???
                  TPress += TmpDiff; //reduce pressure to converge faster???
                  }
                else
                  {//no more steam left to condense to increase temperature...
                  TPress = Mixture().SaturationP(Tmp);
                  iIter = MxIter;
                  }
                }
              }
            }
          }

        double exvap = Mixture().QMass(som_Gas);
        
        HotLiquor.QSetF(Mixture(), som_SL, 1.0, TPress);
        dQmVapVent = Mixture().QMass(som_Gas);

        if (bVentConnected)
          {
          ClrCI(3);
          SpConduit & ESteam = *(IOConduit(IOESteam));
          ESteam.QSetF(Mixture(), som_Gas, 1.0, TPress);
          }
        else
          {
          SetCI(3, fabs(exvap)>UsableMass);
          if (!bRemoveExcessVapour)
            HotLiquor.QAddF(Mixture(), som_Gas, 1.0);
          }
        }
      
      dFinalT    = HotLiquor.Temp();
      dFinalP    = HotLiquor.Press();//HotLiquor.SaturationP(dFinalT);
      dSatPOut   = HotLiquor.SaturationP(dFinalT);
      dPrevPress = dSatPOut;
      dTRise     = dFinalT - LiquorT;
      SetCI(1, HasLiquor && !GeneralCalc && fabs(dFinalT-RqdProdTemp)/RqdProdTemp>1.0e-4);

      m_FTC.m_dMinSatPress = 1.0; // ToFix
      }
      break;
    default:
      MN_Surge::EvalProducts(NEI);
    }
  }

//--------------------------------------------------------------------------

flag ContactHeater::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case 1: pS="W\tCannot Achieve Temperature Requirements";             return 1;
    case 2: pS="W\tNo Steam Supplied";                                   return 1;
    case 3: pS="W\tNo Vent Defined";                                     return 1;
    case 4: pS="W\tTemperature Required too high based on feed streams"; return 1;
    case 5: pS="W\tExpect pure steam in steam line";                     return 1;
    case 6: pS="W\tExpect flash-train or makeup steam line arrangement"; return 1;
    default:                                               
      return MN_Surge::CIStrng(No, pS);
    }
  }

//==========================================================================

