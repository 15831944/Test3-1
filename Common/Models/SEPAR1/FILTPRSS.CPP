//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include "sc_defs.h"
#define  __FILTPRSS_CPP
#include "filtprss.h"
//#include "optoff.h"

//===========================================================================
//
//                                 FilterPress
//
//===========================================================================
/*#D:#T:FilterPress(1)
#X:#h<General Description>#nThe filter press is used to seperate the solids
from the liquids in the feed stream.  There is the facility to wash the solids using wash
water.  This water will displace a percentage of the liquid in the cake from the filter
press.  The filter press can have between 1 and 20 feed streams, but it must have at
least 1 link connected to the feed inlet.  Links must also be connected to the filtrate
and cake connections.  Wash water does not have to be connected to the filter.
#nThe user also has the ability to connect a stream to the filter press to remove the
washing stream separately to the filtrate stream.  This stream will contain the wash
water which is not contained in the filter cake, as well as the feed liquor which was
displaced from the cake by the wash water.  Obviously, if the user does not connect a wash
water stream to the unit, then there is no point in connecting a washings stream either.

#n#n#h<Variables to be supplied by the user>#n
#n#i<% RqdFiltSolids:> The required % solids from the feed reporting to the filtrate.
#n#i<RqdCakeMoist:> The required % moisture in the filter cake.  If wash water has been
added to the filter press, the moisture will consist mainly of wash water.
#n#i<RqdWashEff:> The required displacement efficiency of the wash water, given as a
number between 0 and 1.  1 represents 100% efficiency, in which case all of the liquid
remaining in the cake will consist of wash water.  A value of 0.9 means that 10% of the
liquid in the cake will consist of filtrate.
#nIn Dynamic mode the following parameters also need to be configured:
#n#i<Filtrate2Washing> This can be set on or off, allowing the liquor from the Filter Press
to report to a washings stream.
#n#i<Datum required:> The relative elevation of the filter press with respect to the
common datum of the system.

#n#n#h<Variables calculated by the model>#n
#n#i<ActWashEff:> The actual wash efficiency as calculated by the model.  If this number is
different from the required efficiency, a warning will be displayed in the messages window.
#n#i<Datum:> The height of the filter press above the system datum.

#n#n#h<Other>#n
Default model prefix:FP#n
Short name:FiltPrss#n
Model type:Unit#n
#G:Units
*/
//==========================================================================

DATACNV_PRI(DC_SpCakeRes,     "m/kg",              1, 1.0e12, "Specific Cake Resistance", false);
DATACNV_PRI(DC_MembDP,        "kPa.s/m",           1, 1000.0, "Membrane DP", false);

enum FSM_FiltrateMethods { FSM_SolToFiltFrac, FSM_FiltSolConc };

const byte ioidFeed     = 0;
const byte ioidWash     = 1;
const byte ioidFiltrate = 2;
const byte ioidCake     = 3;
const byte ioidWashings = 4;
const byte ioidVent     = 5;

static IOAreaRec FilterPressIOAreaList[] =
  {
    {"Filter Press Feed", "Feed",       ioidFeed,         LIO_In0,  nc_MLnk, 1, 20, IOPipeEntry, 1.0f},
    {"Wash Water",        "WashWater",  ioidWash,         LIO_In1,  nc_MLnk, 0, 5,  IOPipeEntry, 1.0f},
    {"Filtrate",          "Filtrate",   ioidFiltrate,     LIO_Out0, nc_MLnk, 1, 1,  IOPipeEntry, (float)0.98},
    {"Solids",            "Cake",       ioidCake,         LIO_Out1, nc_MLnk, 1, 1,  IOPipeEntry, 0.0f},
    {"Washings",          "Washings",   ioidWashings,     LIO_Out,  nc_MLnk, 0, 1,  IOPipeEntry, (float)0.98},
    {"",                  "Vent",       ioidVent,         LIO_Out,  nc_MLnk, 0, 1,  IOPipeEntry, 1.0f},
    //SPILL2AREA("Spills", IOId_Spill2Area),
    VENT2AREA("Vents",   IOId_Vent2Area),
    {NULL}
  };

static double Drw_FltPrss[] = { DD_Poly, -10,-5, -10,5, -8,5, -8,-5, -6,-5, -6,5, -4,5,
                     -4,-5, -2,-5, -2,5, 0,5, 0,-5, 2,-5, 2,5, 4,5, 4,-5, 6,-5,
                     6,5, 8,5, 8,-5, 10,-5, 10,5, -10,5, -10,-5, 10,-5, 0,-7,
                     -10,-5, DD_End };

//--------------------------------------------------------------------------

IMPLEMENT_MODELUNIT(FilterPress, "FiltPrss", "1", Drw_FltPrss, "Filter", "FP", TOC_ALL|TOC_GRP_SEPAR|TOC_STD_KENWALT,
                    "Separation:Filter Press(1)",
                    "FilterPress")
FilterPress::FilterPress(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MdlNode(pClass_, TagIn, pAttach, eAttach),
  Cake("Cake", this, TOA_Embedded),
  ToCake("ToCake", this, TOA_Embedded),
  FromCake("FromCake", this, TOA_Embedded),
  //m_Spill(eDIO_Spill, this, false, true, "Spill", IOId_Spill2Area, IOId_AreaSpillI),
  m_Vent(eDIO_Vent, this, false, true, "Vent", IOId_Vent2Area, IOId_AreaVentI),
  //Wash("Wash", this, TOA_Free),
  Fd("Fd", this, TOA_Free)
  {
  AttachIOAreas(FilterPressIOAreaList);

  SetAllowedModes(true, NM_All|SM_Direct|/*SM_Inline|*/SM_Buffered|HM_All);//"Transfer", "Inline", "Buffered");

  iRqdFiltMethod  = FSM_SolToFiltFrac;
  RqdFiltSolConc  = 1.0;
  RqdSolidsToFiltrate = 0.0001;
  RqdCakeMoist    = 0.02;
  RqdWashEff      = 0.9;
  ActCakeSolConc  = 0.0;
  ActFiltSolConc  = RqdFiltSolConc;
  //ActFiltSolConcT = RqdFiltSolConc;
  ActWashEff      = RqdWashEff;
  SolidsToFiltUsed= dNAN;
  ActCakeSolids   = 0.0;
  ActFiltSolids   = 0.0;
  //iScanEffSpecie  = 0;
  //ActScandrettEff = 0.0;
  iWashEffSpecie  = 0;
  dSpWashEff      = 0.0;

  FilterDP        = 0.0;
  MembDP          = 100;
  LiqVisc         = 0.001;
  SpCakeRes       = 2.0e11;
  FiltArea        = 10.0;
  fDischarge      = true;
  fFilt2Washings  = false;

  bTrackStatus    = 1;

  //m_Spill.SetLclIOId(ioid_OverFlw);
  m_Vent.SetLclIOId(ioidVent);

  Cake.SetStateAction(IE_Integrate);
  };

// -------------------------------------------------------------------------

FilterPress::~FilterPress ()
  {
  };

//--------------------------------------------------------------------------

void FilterPress::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);

  DDB.Visibility(SM_Buffered|HM_All);
  BuildDataDefnElevation(DDB);
  DDB.Visibility();

  DDB.Text    ("");
  static DDBValueLst DDB0[]={
    {FSM_SolToFiltFrac, "SolidsToFiltrateFraction"},
    {FSM_FiltSolConc, "FiltrateConcentration"},
    {0}};
  DDB.Text    ("Requirements");
  DDB.Byte    ("Filtrate_Method",         "Method",          DC_,     "",       &iRqdFiltMethod,     this, isParm|AffectsStruct, DDB0);
  DDB.Visibility(NSHM_All, (iRqdFiltMethod == FSM_SolToFiltFrac));
  if (PrjFileVerNo()<48)
    DDB.Double  ("",                        "RqdFiltSolids",   DC_Frac, "%",      &RqdSolidsToFiltrate,this, isParm);
  else
    DDB.Double  ("Rqd_Solids_To_Filtrate",  "RqdSolToFilt",    DC_Frac, "%",      &RqdSolidsToFiltrate,this, isParm);//SolidLoss
  DDB.Visibility(NSHM_All, (iRqdFiltMethod == FSM_FiltSolConc));
  DDB.Double  ("Rqd_FiltrateSolidsConc25","RqdFiltSolConc25",DC_Conc, "g/L",    &RqdFiltSolConc,     this, isParm);
  DDB.Visibility();
  DDB.Double  ("Solids_To_FiltrateUsed",  "SolToFiltUsed",   DC_Frac, "%",      &SolidsToFiltUsed,   this, isResult|InitHidden);//|noView);
  DDB.Double  ("Rqd_Cake_Moisture",       "RqdCakeMoist",    DC_Frac, "%",      &RqdCakeMoist,       this, isParm);
  DDB.Double  ("Rqd_Wash_Efficiency",     "RqdWashEff",      DC_Frac, "%",      &RqdWashEff,         this, isParm);
  DDB.Text    ("Results");
  DDB.Double  ("Wash_Efficiency",         "WashEff",         DC_Frac, "%",      &ActWashEff,         this, isResult);//|noFile|noSnap);
  DDB.Double  ("Cake_Solids",             "CakeSolids",      DC_Frac, "%",      &ActCakeSolids,      this, isResult);//|noFile|noSnap);
  DDB.Double  ("Filtrate_Solids",         "FiltSolids",      DC_Frac, "%",      &ActFiltSolids,      this, isResult);//|noFile|noSnap);
  DDB.Double  ("CakeSolidsConc@25",       "CakeSolConc25",   DC_Conc, "g/L",    &ActCakeSolConc,     this, isResult);//|noFile|noSnap);
  DDB.Double  ("FiltrateSolidsConc@25",   "FiltSolConc25",   DC_Conc, "g/L",    &ActFiltSolConc,     this, isResult);//|noFile|noSnap);
  //DDB.Double  ("FiltrateSolidsConc@T",    "FiltSolConc@T",   DC_Conc, "g/L",    &ActFiltSolConcT,    this, isResult);//|noFile|noSnap|InitHidden);
  //DDB.Byte    ("",                        "ScanEffSpecie",   DC_,     "",       &iScanEffSpecie,     this, isParm, SDB.DDBLiqSpList());
  //DDB.Double  ("Scandrett_Wash_Eff",      "ScandrettEff",    DC_Frac, "%",      &ActScandrettEff,    this, isResult);//|noFile|noSnap);
  DDB.Byte    ("",                        "WashComp",        DC_,     "",       &iWashEffSpecie,     this, isParm, SDB.DDBLiqSpList());
  DDB.Double  ("",                        "CompWashEff",     DC_Frac, "%",      &dSpWashEff,         this, isResult);//|noFile|noSnap);

  DDB.Text    (" ");
  DDB.CheckBox("TrackStatus", "",           DC_,          "",         &bTrackStatus,     this, isParm);

  DDB.Visibility(SM_Buffered|HM_All);
  DDB.Text    (" ");
  DDB.CheckBoxBtn("",   "Filtrate2Washing",    DC_,          "",      &fFilt2Washings,   this, isParm, DDBYesNo);
  DDB.CheckBoxBtn("",   "CakeDisch",           DC_,          "",      &fDischarge,       this, isParm, DDBYesNo);
  DDB.Text    (" ");
  DDB.Double  ("",   "Press_Drop",          DC_DP,        "kPa",      &FilterDP,         this, isResult|0);
  DDB.Double  ("",   "MembraneRes",         DC_MembDP,    "kPa.s/m",  &MembDP,           this, isParm);
  DDB.Double  ("",   "Viscosity",           DC_Visc,      "cP",       &LiqVisc,          this, isParm);
  DDB.Double  ("",   "SpecCakeResistance",  DC_SpCakeRes, "m/kg",     &SpCakeRes,        this, isParm);
  DDB.Double  ("",   "Area",                DC_Area,      "m^2",      &FiltArea,         this, isParm);

  if (SolveDynamicMethod())
    DDB.Object(&Cake, this, NULL, NULL, DDB_RqdPage);
  DDB.Visibility();

  DDB.Text    ("");
  BuildDataDefnShowIOs(DDB);

  BuildDataDefnAreaIOs(DDB);

  DDB.EndStruct();
  }

// --------------------------------------------------------------------------

flag FilterPress::DataXchg(DataChangeBlk & DCB)
  {
  if (MdlNode::DataXchg(DCB))
    return 1;
  return 0;
  }

// -------------------------------------------------------------------------

flag FilterPress::ChangeMode(dword NewMode, dword OldMode)
  {
  if (NewMode==SM_Direct)
    GSM.Open();

  return True;
  }

//--------------------------------------------------------------------------

void FilterPress::SetState(eScdMdlStateActs RqdState)
  {
  MdlNode::SetState(RqdState);
  switch (RqdState)
    {
    case MSA_PBInit:
      break;
    case MSA_Empty:
    case MSA_PreSet:
      Cake.ZeroMass();
      Cake.ZeroDeriv();
      break;
    case MSA_SteadyState:
      LogNote(FullObjTag(), 0, "SteadyState Undefined");
      break;
    }
  }

//---------------------------------------------------------------------------

//CFlange * FilterPress::GetFlange(int IOId)
//  {
//  switch (IOId)
//    {
//    //case IOId_Spill2Area:   return &m_Spill.Flange;
//    case IOId_Vent2Area:    return &m_Vent.Flange;
//    //case IOId_AccIn2Area:   return &m_AccIn.Flange;
//    //case IOId_AccOut2Area:  return &m_AccOut.Flange;
//    }
//  return NULL;
//  };
//
//---------------------------------------------------------------------------

void FilterPress::ConfigureJoins()
  {
  Init_NoJoins(1);
  int iWashings = IOWithId_Self(ioidWashings);
  if (iWashings < 0)
    fFilt2Washings=False;
  int i;
  if (NetMethod()==NM_Probal || SolveMethod()!=SM_Buffered)
    {
    for (i=0; (i<NoProcessIOs()); i++)
      SetIO_Join(i, 0);
    }
  else
    {
    for (i=0; (i<NoProcessIOs()); i++)
      {
      switch (IOId_Self(i))
        {
        case ioidFiltrate:
          if (!fFilt2Washings && !fDischarge)
            SetIO_Join(i, 0);
          else
            SetIO_Closed(i, 0);
          break;
        case ioidWashings:
          if (fFilt2Washings && !fDischarge)
            SetIO_Join(i, 0);
          else
            SetIO_Closed(i, 0);
          break;
        case ioidCake:
          if (fDischarge)
            SetIO_Join(i, 0);
          else
            SetIO_Closed(i, 0);
          break;
        default:
          SetIO_Join(i, 0);
          break;
        }
      }
    }
  }

//--------------------------------------------------------------------------

flag FilterPress::EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo)
  {
  switch (Task)
    {
    case FET_GetMode:
      break;
    case FET_SetQm:
      IOFB(IONo, FE)->SetDPb(0.0, 0.0);
      IOFB(IONo, FE)->SetDPbX(0.0);
      IOFB(IONo, FE)->SetDPq(0.0, 0.0);
      break;
    case FET_CalcDP:
      {
      if ((IOId_Self(IONo)==ioidFiltrate && !fFilt2Washings) ||
        (IOId_Self(IONo)==ioidWashings && fFilt2Washings))
        {
        SigmaQInPMin(Sd, som_ALL, Id_2_Mask(ioidFeed)|Id_2_Mask(ioidWash));

        double Mass=Cake.Mass();
        double TMass=Sd.QMass();
        double LVol =Sd.QVolume(som_Liq);

        double QIn=IOFB(IONo,FE)->QmMeas();
        double EstVolFlow = fabs(QIn) / GTZ(TMass) *  LVol;
        double DP=MembDP*EstVolFlow/GTZ(FiltArea)+LiqVisc*SpCakeRes*Mass*EstVolFlow/Sqr(FiltArea);

        IOFB(IONo,FE)->SetDPq(Sign(QIn)*DP, Sign(QIn)*DP/NZ(fabs(QIn)));
        FilterDP = IOP_Self(IONo)-IOP_Rmt(IONo);
        }
      else if (IOId_Self(IONo)==ioidCake && fDischarge)
        {
        IOFB(IONo,FE)->SetDPq(0.0, 0.0);
        }
      break;
      }
    }
  return true;
  }

//--------------------------------------------------------------------------
// x is required feed solids fraction to report to filtrate
//
class CSolidsFracFnd : public MRootFinderBase
  {
  public:
    SpConduit &Fd, &Filtrate, &CakeOut;
    double washSF;
    double ActWashEff;
    double SolidsToFiltUsed;
  protected:
    SpConduit * pWash;
    SpConduit * pWashings;
    flag UseWashings;
    double MSol, MLiq;
    double POut;
    double RqdCakeMoist, RqdWashEff;
    double HTot;
    double FT;
    static CToleranceBlock s_Tol;

  public:
    CSolidsFracFnd(SpConduit &Fd_, SpConduit * pWash_, SpConduit &Filtrate_, SpConduit &CakeOut_, SpConduit * pWashings_,
      flag UseWashings_, double POut_, double RqdCakeMoist_, double RqdWashEff_, double HTot_, double FT_);
    double Function(double x);
  };

CToleranceBlock CSolidsFracFnd::s_Tol(TBF_Both, "FilterPress:SolidsFrac", 0.0, 1.0e-8);

//--------------------------------------------------------------------------
    
CSolidsFracFnd::CSolidsFracFnd(SpConduit &Fd_, SpConduit * pWash_, SpConduit &Filtrate_, SpConduit &CakeOut_, SpConduit * pWashings_,
      flag UseWashings_, double POut_, double RqdCakeMoist_, double RqdWashEff_, double HTot_, double FT_) :
  MRootFinderBase("FiltPress", s_Tol),//1.0e-8), 
  Fd(Fd_), Filtrate(Filtrate_), CakeOut(CakeOut_)
  {
  pWash = pWash_;
  pWashings = pWashings_;
  UseWashings = UseWashings_;
  POut = POut_;
  RqdCakeMoist = RqdCakeMoist_;
  RqdWashEff = RqdWashEff_;
  HTot = HTot_;
  FT = FT_;

  MSol = Fd.QMass(som_Sol);
  MLiq = Fd.QMass(som_Liq);
  washSF = 0.0;
  ActWashEff = 0.0;
  SolidsToFiltUsed = 0.0;
  }

//---------------------------------------------

double CSolidsFracFnd::Function(double x) 
  {
  SolidsToFiltUsed = x;
  double MSol2Filt = MSol * SolidsToFiltUsed;
  if (pWash)
    {//has wash water stream...
    SpConduit & Wash = *pWash;

    CakeOut.QSetM(Fd, som_Sol, GEZ(MSol - MSol2Filt), POut);

    double Mwash = Wash.QMass(som_Liq);
    double solwash = Wash.QMass(som_Sol);
    washSF = solwash / GTZ(Mwash + solwash);

    double MLiq2Cake = (RqdCakeMoist * (MSol + solwash - MSol2Filt))/GTZ(1.0 - RqdCakeMoist);
    double Filt2Cake = MLiq2Cake * (1.0 - RqdWashEff);
    double Wash2Cake = MLiq2Cake - Filt2Cake;
    double MLiq2Filt = MLiq - Filt2Cake;

    if (Mwash >= Wash2Cake)
      {
      CakeOut.QAddM(Wash, som_Sol, GEZ(solwash)); //all solids in washwater to report to cake
      CakeOut.QAddM(Wash, som_Liq, GEZ(Wash2Cake));
      CakeOut.QAddM(Fd, som_Liq, GEZ(Filt2Cake));
      ActWashEff = Wash2Cake/Max(MLiq2Cake, 1e-6);
      if (UseWashings)
        {
        SpConduit & Washings = *pWashings;
        Filtrate.QSetM(Fd, som_Liq, GEZ(MLiq - MLiq2Cake), POut);
        Washings.QSetM(Wash, som_Liq, GEZ(Mwash - Wash2Cake), POut);
        Washings.QAddM(Fd, som_Liq, GEZ(MLiq2Cake - Filt2Cake));
        double totout, liqfilt, liqwash;
        liqfilt = Filtrate.QMass(som_Liq);
        liqwash = Washings.QMass(som_Liq);
        totout  = liqfilt + liqwash;
        if (totout > 0.0)
          {
          liqfilt = liqfilt / totout;
          liqwash = liqwash / totout;
          Filtrate.QAddM(Fd, som_Sol, GEZ(MSol2Filt * liqfilt));
          Washings.QAddM(Fd, som_Sol, GEZ(MSol2Filt * liqwash));
          }
        else
          {
          Filtrate.QAddM(Fd, som_Sol, 0.0);
          Washings.QAddM(Fd, som_Sol, 0.0);
          }
        }
      else
        {
        Filtrate.QSetM(Fd, som_Liq, GEZ(MLiq2Filt), POut);
        Filtrate.QAddM(Wash, som_Liq, GEZ(Mwash - Wash2Cake));
        Filtrate.QAddM(Fd, som_Sol, GEZ(MSol2Filt));
        if (pWashings)
          pWashings->QCopy(Wash);
        }
      }
    else
      {
      CakeOut.QAddM(Wash, som_Liq, GEZ(Mwash));
      CakeOut.QAddM(Fd, som_Liq, GEZ(MLiq2Cake - Mwash));
      Filtrate.QSetM(Fd, som_Liq, GEZ(MLiq + Mwash - MLiq2Cake), POut);
      Filtrate.QAddM(Fd, som_Sol, GEZ(MSol2Filt));
      ActWashEff = Mwash/Max(MLiq2Cake, 1e-6);
      if (pWashings)
        {
        SpConduit & Washings = *pWashings;
        Washings.QSetM(Wash, som_Liq, 0.0, POut);
        }
      }
    }
  else
    {//no wash water stream
    double MLiq2Cake = (RqdCakeMoist * (MSol - MSol2Filt))/GTZ(1.0 - RqdCakeMoist);
    double MLiq2Filt = MLiq - MLiq2Cake;
    ActWashEff = 0.0/Max(MLiq2Cake, 1e-6);

    Filtrate.QSetM(Fd, som_Sol, GEZ(MSol2Filt), POut);
    Filtrate.QAddM(Fd, som_Liq, GEZ(MLiq2Filt));

    CakeOut.QSetM(Fd, som_Sol, GEZ(MSol - MSol2Filt), POut);
    CakeOut.QAddM(Fd, som_Liq, GEZ(MLiq2Cake));
    }

  CakeOut.SanityCheck();
  Filtrate.SanityCheck();
  if (1)
    {// Correct Enthalpy...
    CakeOut.SetTemp(FT);
    Filtrate.SetTemp(FT);
    if (pWashings)
      pWashings->SetTemp(FT);
    double P = POut;
  
    double H = CakeOut.totHf()+Filtrate.totHf();
    if (pWashings)
      H += pWashings->totHf();
    double dT = 0.0, H0, T0;
    for (int Hiter=0; Hiter<10; Hiter++)
      {
      double Err;
      if (ConvergedVV(HTot, H, 1.0e-12, 1.0e-12, Err))
        break;
      if (dT!=0.0)
        dT = (HTot-H)*(FT-T0)/NZ(H-H0);
      else
        dT = 0.1;
      T0 = FT;
      H0 = H;
      FT += dT;
      H = CakeOut.totHf(som_ALL, FT, P)+Filtrate.totHf(som_ALL, FT, P);
      if (pWashings)
        H += pWashings->totHf(som_ALL, FT, P);
      }
    CakeOut.SetTemp(FT);
    Filtrate.SetTemp(FT);
    if (pWashings)
      pWashings->SetTemp(FT);
    }

  double Conc25 = Filtrate.PhaseConc(C_2_K(25.0), som_Sol, som_ALL);
  return Conc25;
  }

//--------------------------------------------------------------------------

void FilterPress::EvalJoinPressures(long JoinMask)
  {
  switch (NetMethod())
    {
    case NM_Probal:
      {
      for (int j=0; j<NoProcessJoins(); j++)
        {
        double P = PBPress();//AtmosPress(IODatum_Term(j));
        SetPBJoinPressure(j, P, true, true);
        }
      break;
      }
    case NM_Dynamic:
      MdlNode::EvalJoinPressures(JoinMask);
      break;
    }
  }
 
//--------------------------------------------------------------------------

void FilterPress::EvalProducts(CNodeEvalIndex & NEI)
  {
  const int iFiltrate = IOWithId_Self(ioidFiltrate);
  const int iCake     = IOWithId_Self(ioidCake);
  const int iWashings = IOWithId_Self(ioidWashings);
  const int iWash     = IOWithId_Self(ioidWash);

  SpConduit & Filtrate = *IOConduit(iFiltrate);
  SpConduit & CakeOut  = *IOConduit(iCake);
  SpConduit * pWashings = (iWashings>=0 ? IOConduit(iWashings) : NULL);

  SigmaQInPMin(Sd, som_ALL, Id_2_Mask(ioidWash));
  SigmaQInPMin(Fd, som_ALL, Id_2_Mask(ioidFeed));
  //double CFeed = Fd.SpecieConc(Fd.Temp(), iScanEffSpecie, som_Liq);
  double CFeed = Fd.SpecieConc(Fd.Temp(), iWashEffSpecie, som_Liq);
  double CWash = Sd.SpecieConc(Sd.Temp(), iWashEffSpecie, som_Liq);

  SigmaQInPMin(Sd, som_SL/*ALL*/, Id_2_Mask(ioidFeed)|Id_2_Mask(ioidWash));
  flag HasFlw = (Sd.QMass(som_ALL)>UsableMass);
  const double MinFiltSolidsFrac = 1.0e-8;
  const double MaxFiltSolidsFrac = 0.3;
  RqdFiltSolConc = Range(0.0, RqdFiltSolConc, 5000.0);
  RqdSolidsToFiltrate  = Range(0.0/*MinFiltSolidsFrac*/, RqdSolidsToFiltrate, MaxFiltSolidsFrac);
  RqdCakeMoist   = Range(1.0e-8, RqdCakeMoist, 0.99);

  double washSF = 0.0;

  if (SolveBufferedMethod())
    {
    SolidsToFiltUsed = RqdSolidsToFiltrate;
    iRqdFiltMethod = FSM_SolToFiltFrac;

    for (int i=0; i<NoFlwIOs(); i++)
      if (IOId_Self(i)==ioidWash)
        Fd.QAddF(*IOConduit(i), som_ALL, 1.0);

    double MSol = Fd.QMass(som_Sol);
    double MLiq = Fd.QMass(som_Liq);
    double MSol2Filt = MSol * SolidsToFiltUsed;
    double MLiq2Cake = (RqdCakeMoist * (MSol - MSol2Filt))/GTZ(1.0 - RqdCakeMoist);
    double MLiq2Filt = MLiq - MLiq2Cake;

    if (fDischarge)
      {
      ToCake.QZero();
      FromCake.QSetF(Cake, som_ALL, Cake.Mass() * 0.8/ICGetTimeInc(), IOP_Self(iFiltrate));
      CakeOut.QSetF(Fd, som_ALL, 1.0, IOP_Self(iFiltrate));
      CakeOut.QAddF(FromCake, som_ALL, 1.0);
      Filtrate.QZero();
      if (iWashings>=0)
        IOConduit(iWashings)->QZero();
      }
    else
      {
      ToCake.QSetM(Fd, som_Sol, GEZ(MSol - MSol2Filt), IOP_Self(iCake));
      ToCake.QAddM(Fd, som_Liq, GEZ(MLiq2Cake));
      CakeOut.QZero();
      FromCake.QZero();

      if (fFilt2Washings && (iWashings>=0))
        {
        SpConduit & Washings = *IOConduit(iWashings);
        Washings.QSetM(Fd, som_Sol, GEZ(MSol2Filt), IOP_Self(iFiltrate));
        Washings.QAddM(Fd, som_Liq, GEZ(MLiq2Filt));
        }
      else
        {
        Filtrate.QSetM(Fd, som_Sol, GEZ(MSol2Filt), IOP_Self(iFiltrate));
        Filtrate.QAddM(Fd, som_Liq, GEZ(MLiq2Filt));
        }
      }

    //put all vapours (if any) to vent (if present)
    const int iVent = IOWithId_Self(ioidVent);
    SpConduit & Cvent = iVent>=0 ? *IOConduit(iVent) : m_Vent.Cd;
    double Qg = Fd.QMass(som_Gas);
    Cvent.QSetM(Fd, som_Gas, Qg, Std_P);
    if (iWash >= 0)
      {
      SpConduit & Wash = *IOConduit(iWash);
      Qg = Wash.QMass(som_Gas);
      Cvent.QAddM(Wash, som_Gas, Qg);
      }
    }
  else
    {//ProBal; DynXFer...
    SpConduit * pWash = (iWash>=0 ? IOConduit(iWash) : NULL);
    flag Ok = true;
    //double POut = AtmosPress(); //force outlet to Atmos P
    double POut = Std_P; //force outlet to Std_P
    flag UseWashings = (iWashings >= 1 && pWashings->QMass()>UsableMass);
    double HTot = Sd.totHf();
    double FT = Sd.Temp();
    CSolidsFracFnd Fnd(Fd, pWash, Filtrate, CakeOut, pWashings, UseWashings, POut, RqdCakeMoist, RqdWashEff, HTot, FT);
    if (iRqdFiltMethod == FSM_SolToFiltFrac)
      {
      double dd = Fnd.Function(RqdSolidsToFiltrate);
      }
    else if (iRqdFiltMethod == FSM_FiltSolConc)
      {
      SolidsToFiltUsed = Valid(SolidsToFiltUsed) ? SolidsToFiltUsed : 0.0001;
      if (RqdFiltSolConc<1.0e-9)
        {
        double dd = Fnd.Function(0.0);
        }
      else
        {
        Fnd.SetTarget(RqdFiltSolConc);
        Fnd.SetEstimate(SolidsToFiltUsed, 1.0);
        Ok = false;
        int iRet = Fnd.Start(MinFiltSolidsFrac, MaxFiltSolidsFrac);
        if (iRet==RF_EstimateOK) //estimate is good, solve not required
          {
          double dd = Fnd.Result();
          Ok = true;
          }
        else
          {
          if (iRet==RF_BadEstimate)
            iRet = Fnd.Start(MinFiltSolidsFrac, MaxFiltSolidsFrac); // Restart
          if (iRet==RF_OK)
            if (Fnd.Solve_Brent()==RF_OK)
              {
              double dd = Fnd.Result();
              //SolidsToFiltUsed = Fnd.SolidsToFiltUsed;
              Ok = true;
              }
          }
        }
      }
    if (!Ok)
      {
      //SetCI(???);
      }
    //use result anyway...
    SolidsToFiltUsed = Fnd.SolidsToFiltUsed; 
    washSF = Fnd.washSF;
    ActWashEff = Fnd.ActWashEff;

    //put all vapours (if any) to vent (if present)
    const int iVent = IOWithId_Self(ioidVent);
    if (iVent>=0)
      {
      double Qg = Fd.QMass(som_Gas);
      SpConduit & Cvent = *IOConduit(iVent);
      Cvent.QSetM(Fd, som_Gas, Qg, POut);
      if (iWash >= 0)
        {
        SpConduit & Wash = *IOConduit(iWash);
        Qg = Wash.QMass(som_Gas);
        Cvent.QAddM(Wash, som_Gas, Qg);
        }
      }
    }

  ActCakeSolids    = CakeOut.MassFrac(som_Sol);
  ActFiltSolids    = Filtrate.MassFrac(som_Sol);
  ActCakeSolConc   = CakeOut.PhaseConc(C_2_K(25.0), som_Sol, som_ALL);
  ActFiltSolConc   = Filtrate.PhaseConc(C_2_K(25.0), som_Sol, som_ALL);
  //ActFiltSolConcT  = Filtrate.PhaseConc(Filtrate.Temp(), som_Sol, som_ALL);

  //double CCake = CakeOut.SpecieConc(CakeOut.Temp(), iScanEffSpecie, som_Liq);
  double CCake = CakeOut.SpecieConc(CakeOut.Temp(), iWashEffSpecie, som_Liq);
  /*double CFilt;
  if (pWashings)
    {
    SigmaQOut(Sd, som_ALL, Id_2_Mask(ioidFiltrate)|Id_2_Mask(ioidWashings)); //use Sd for sum of filtrate and washings
    CFilt = Sd.SpecieConc(Sd.Temp(), iScanEffSpecie, som_Liq);
    }
  else
    {
    CFilt = Filtrate.SpecieConc(Filtrate.Temp(), iScanEffSpecie, som_Liq);
    }
  ActScandrettEff = (CFeed - CCake)/NZ(CFeed - CFilt);*/
  dSpWashEff = (CFeed - CCake)/NZ(CFeed - CWash);
  
  double WashErr   = fabs(RqdWashEff - ActWashEff);
  SetCI(1, bTrackStatus && HasFlw && (WashErr > 1.0e-6));
  SetCI(2, bTrackStatus && HasFlw && (washSF > 0.01));
  //SetCI(3, bTrackStatus && (solwash > 1.0e-6));
  if (SolveBufferedMethod())
    {
    ClrCI(4);
    ClrCI(5);
    }
  else
    {
    if (iRqdFiltMethod == FSM_SolToFiltFrac)
      {
      ClrCI(4);
      ClrCI(5);
      }
    else
      {
      SetCI(4, bTrackStatus && HasFlw && (fabs(ActFiltSolConc-RqdFiltSolConc) > 1.0e-5));
      SetCI(5, bTrackStatus && HasFlw && (SolidsToFiltUsed>1.0e-6) && (SolidsToFiltUsed<MinFiltSolidsFrac*1.001 || SolidsToFiltUsed>MaxFiltSolidsFrac*0.99999));
      }
    }
  }

//--------------------------------------------------------------------------

void FilterPress::EvalDerivs(CNodeEvalIndex & NEI)
  {
  Cake.ZeroDeriv();
  if (!GetActiveHold())
    {
    Cake.AddDeriv(ToCake, 1.0);
    Cake.AddDeriv(FromCake, -1.0);
    }
  }

//--------------------------------------------------------------------------

dword FilterPress::ModelStatus()
  {
  dword Status=MdlNode::ModelStatus();
  if (NoFlwIOs())
    {
    int HasFlw=0;
    for (int i=0; i<NoFlwIOs(); i++)
      {
      if (IOConduit(i)->QMass()>UsableMass)
        HasFlw=1;
      }
    Status |= (HasFlw ? FNS_UFlw : FNS_UNoFlw);
    }
  return Status;
  }

//--------------------------------------------------------------------------

flag FilterPress::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case  1: pS="W\tWash requirements not met"; return 1;
    case  2: pS="W\tLarge amount of solids in Wash Water"; return 1;
    //case  3: pS="N\tSolids detected in Wash Water report to cake"; return 1;
    case  4: pS="W\tCannot achieve required filtrate solids concentration"; return 1;
    case  5: pS="N\tFeed solids to filtrate fraction used, is at limit"; return 1;
    //case  ?: pS="W\tCannot achieve required feed solids to filtrate fraction requirement"; return 1;
    default:
      return MdlNode::CIStrng(No, pS);
    }
  }

//--------------------------------------------------------------------------
//===========================================================================
