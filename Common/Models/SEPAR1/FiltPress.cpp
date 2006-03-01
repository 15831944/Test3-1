//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdio.h>

#include "sc_defs.h"
#define  __FILTPRSS_CPP
#include "filtprss.h"
#include "dbgmngr.h"

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

const byte idFeed     = 0;
const byte idWash     = 1;
const byte idFiltrate = 2;
const byte idCake     = 3;
const byte idWashings = 4;
const byte idVent     = 5;

static IOAreaRec FilterPressIOAreaList[] =
  {{"Filter Press Feed", "Feed"      , idFeed,     LIO_In0 ,    nc_MLnk, 1, 20},
   {"Wash Water",        "WashWater" , idWash,     LIO_In1,     nc_MLnk, 0, 1},
   {"Filtrate",          "Filtrate"  , idFiltrate, LIO_Out0,    nc_MLnk, 1, 1},
   {"Solids",            "Cake"      , idCake,     LIO_Out1,    nc_MLnk, 1, 1},
   {"Washings",          "Washings"  , idWashings, LIO_Out,    nc_MLnk, 0, 1},
   {"",                  "Vent"      , idVent,     LIO_Out,    nc_MLnk, 0, 1, IOPipeEntry, (float)1.0},
   {NULL}};

long Drw_FltPrss[] = { DD_Poly, -10,-5, -10,5, -8,5, -8,-5, -6,-5, -6,5, -4,5,
                     -4,-5, -2,-5, -2,5, 0,5, 0,-5, 2,-5, 2,5, 4,5, 4,-5, 6,-5,
                     6,5, 8,5, 8,-5, 10,-5, 10,5, -10,5, -10,-5, 10,-5, 0,-7.5,
                     -10,-5, DD_End };

//--------------------------------------------------------------------------

IMPLEMENT_MODELUNIT(FilterPress, "FiltPrss", "1", Drw_FltPrss, "FP", TOC_ALL|TOC_GRP_SEPAR|TOC_STD_KENWALT,
                    "FilterPress(1)",
                    "FilterPress")
FilterPress::FilterPress(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MdlNode(pClass_, TagIn, pAttach, eAttach),
  Cake("Cake", this, TOA_Embedded),
  ToCake("ToCake", this, TOA_Embedded),
  FromCake("FromCake", this, TOA_Embedded),
  Sd("Sd", this, TOA_Free),
  Wash("Wash", this, TOA_Free)
  {
  AttachIOAreas(FilterPressIOAreaList);
  FlowRatio      = 0.0;
  RqdFiltSolids  = 0.00001;
  RqdCakeMoist   = 0.02;
  WashEff        = 0.9;
  ActWashEff     = 0.9;
  FilterDP       = 0.0;
  MembDP         = 100;
  LiqVisc        = 0.001;
  SpCakeRes      = 2.0e11;
  FiltArea       = 10.0;
  fDischarge     = True;
  fFilt2Washings = False;
  bTrackStatus   = 1;

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

  //DDB.Text("");
  //GSM.Add_OnOff(DDB);

  DDB.Visibility(DYNMODE);
  DDB.Text   ("Datum");
  DDB.Double ("Datum",        "Z",    DC_L    , "m",    &dDatum,        this, isResult|0      | DYNMODE);
  DDB.Double ("Datum_Rqd",    "Z_Rqd",DC_L    , "m",    &dDatum_Rqd,    this, isParmStopped | DYNMODE|NAN_OK);
  DDB.Visibility();

  //if (!GSM.Enabled())
  //  {
    DDB.Text    (" ");
    DDB.Double  ("",   "RqdFiltSolids",       DC_Frac,      "%",        &RqdFiltSolids,  this, isParm);
    DDB.Double  ("",   "RqdCakeMoist",        DC_Frac,      "%",        &RqdCakeMoist,   this, isParm);
    DDB.Double  ("",   "RqdWashEff",          DC_Frac,      "%",        &WashEff,        this, isParm);
    DDB.Double  ("",   "ActWashEff",          DC_Frac,      "%",        &ActWashEff,     this, isResult|0);

    DDB.Text    (" ");
    DDB.CheckBox("TrackStatus", "",           DC_,          "",         &bTrackStatus,   this, isParm);

    DDB.Visibility(DYNMODE);
    DDB.Text    (" ");
    DDB.CheckBoxBtn("",   "Filtrate2Washing",    DC_,          "",         &fFilt2Washings, this, isParm, DDBYesNo);
    DDB.CheckBoxBtn("",   "CakeDisch",           DC_,          "",         &fDischarge,     this, isParm, DDBYesNo);
    DDB.Text    (" ");
    DDB.Double  ("",   "Press_Drop",          DC_DP,        "kPa",      &FilterDP,       this, isResult|0);
    DDB.Double  ("",   "MembraneRes",         DC_MembDP,    "kPa.s/m",  &MembDP,         this, isParm);
    DDB.Double  ("",   "Viscosity",           DC_Visc,      "cP",       &LiqVisc,        this, isParm);
    DDB.Double  ("",   "SpecCakeResistance",  DC_SpCakeRes, "m/kg",     &SpCakeRes,      this, isParm);
    DDB.Double  ("",   "Area",                DC_Area,      "m^2",      &FiltArea,       this, isParm);

    if (SolveMode()==DYNMODE)
      DDB.Object(&Cake, this, NULL, NULL, DDB_RqdPage);
    DDB.Visibility();
  //  }
  //else
  //  GSM.BuildDataDefn(DDB, NULL, NULL, DDB_OptPage);

  BuildDataDefnShowIOs(DDB);

  DDB.EndStruct();
  };

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
  if (NewMode==PBMODE)
    GSM.Open();

  return True;
  };

//--------------------------------------------------------------------------

void   FilterPress::SetState(EOMdlStates RqdState)
  {
  MdlNode::SetState(RqdState);
  switch (RqdState)
    {
    case EOMS_PBInit:
      break;
    case EOMS_Empty:
    case EOMS_PreSet:
      Cake.ZeroMass();
      Cake.ZeroDeriv();
      break;
    case EOMS_SteadyState:
      LogNote(FullObjTag(), 0, "SteadyState Undefined");
      break;
    }
  return 0.0;
  };

//---------------------------------------------------------------------------

void FilterPress::ConfigureJoins()
  {
  Init_NJoins(1);
  int iWashings = IOWithId_Self(idWashings);
  if (iWashings < 0)
    fFilt2Washings=False;
  int i;
  switch (SolveMode())
    {
    case PBMODE:
    case SSMODE:
      for (i=0; (i<NoFlwIOs()); i++)
        SetIO_Join(i, 0, 0);
      break;
    case DYNMODE:
      for (i=0; (i<NoFlwIOs()); i++)
        {
        switch (IOId_Self(i))
          {
          case idFiltrate:
            if (!fFilt2Washings && !fDischarge)
              SetIO_Join(i, 0, 0);//&FB);
            else
              SetIO_Closed(i, 0);
            break;
          case idWashings:
            if (fFilt2Washings && !fDischarge)
              SetIO_Join(i, 0, 0);//&FB);
            else
              SetIO_Closed(i, 0);
            break;
          case idCake:
            if (fDischarge)
              SetIO_Join(i, 0, 0);//&FB);
            else
              SetIO_Closed(i, 0);
            break;
          default:
            SetIO_Join(i, 0, 0);//&FB);
            break;
          }
        }
      break;
    }
  };

// --------------------------------------------------------------------------

void FilterPress::EvalJoinFlows(int JoinNo)
  {
  MdlNode::EvalJoinFlows(JoinNo);
  };

//--------------------------------------------------------------------------

void FilterPress::EvalJoinPressures()
  {
  MdlNode::EvalJoinPressures();

  //int iCake     = IOWithId_Self(idCake);
  //int iWashings = IOWithId_Self(idWashings);
  };

//--------------------------------------------------------------------------

//flag FilterPress::EvalFlowEquations(CSpPropInfo *pProps, int iJoinId, int FE)
//  {
//
//  return True;
//  };

//--------------------------------------------------------------------------

flag FilterPress::EvalFlowEquations(CSpPropInfo *pProps, int IONo, int FE)
  {

  if ((IOId_Self(IONo)==idFiltrate && !fFilt2Washings) ||
      (IOId_Self(IONo)==idWashings && fFilt2Washings))
    {
    SigmaQInPMin(Sd, som_ALL, Id_2_Mask(idFeed)|Id_2_Mask(idWash));

    double Mass=Cake.Mass();
    double TMass=Sd.QMass();
    double LVol =Sd.QVolume(som_Liq);

    double QIn=IOFB(IONo,FE)->QmMeas();
    double EstVolFlow = fabs(QIn) / GTZ(TMass) *  LVol;
    double DP=MembDP*EstVolFlow/GTZ(FiltArea)+LiqVisc*SpCakeRes*Mass*EstVolFlow/Sqr(FiltArea);

    IOFB(IONo,FE)->SetDPq(Sign(QIn)*DP, Sign(QIn)*DP/NZ(fabs(QIn)));
    FilterDP = IOP_Self(IONo)-IOP_Rmt(IONo);
    }
  else if (IOId_Self(IONo)==idCake && fDischarge)
    {
    IOFB(IONo,FE)->SetDPq(0.0, 0.0);
    }

  return True;
  };

//--------------------------------------------------------------------------

void FilterPress::EvalProducts()
  {
  int iFiltrate = IOWithId_Self(idFiltrate);
  int iCake     = IOWithId_Self(idCake);
  int iWashings = IOWithId_Self(idWashings);
  int iWash     = IOWithId_Self(idWash);

  rSpConduit Filtrate = *IOConduit(iFiltrate);
  rSpConduit CakeOut  = *IOConduit(iCake);

  double Mwash, MSol, MLiq, MSol2Filt, MLiq2Cake,
         MLiq2Filt, Filt2Cake, Wash2Cake;
  SigmaQInPMin(Sd, som_ALL, Id_2_Mask(idFeed));

  RqdFiltSolids = Range(1.0e-8, RqdFiltSolids, 0.2);
  RqdCakeMoist  = Range(1.0e-8, RqdCakeMoist, 0.99);

  double solwash = 0.0;
  double washSF = 0.0;

  if (DynMode())
    {
    for (int i=0; i<NoFlwIOs(); i++)
      if (IOId_Self(i)==idWash)
        Sd.QAddF(*IOConduit(i), som_ALL, 1.0);

    MSol = Sd.QMass(som_Sol);
    MLiq = Sd.QMass(som_Liq);
    MSol2Filt = MSol * RqdFiltSolids;
    MLiq2Cake = (RqdCakeMoist * (MSol - MSol2Filt))/GTZ(1 - RqdCakeMoist);
    MLiq2Filt = MLiq - MLiq2Cake;

    if (fDischarge)
      {
      ToCake.QZero();
      FromCake.QSetF(Cake, som_ALL, Cake.Mass() * 0.8/ICGetdTime(), IOP_Self(iFiltrate));
      CakeOut.QSetF(Sd, som_ALL, 1.0, IOP_Self(iFiltrate));
      CakeOut.QAddF(FromCake, som_ALL, 1.0);
      Filtrate.QZero();
      if (iWashings>=0)
        IOConduit(iWashings)->QZero();
      }
    else
      {
      ToCake.QSetM(Sd, som_Sol, GEZ(MSol - MSol2Filt), IOP_Self(iCake));
      ToCake.QAddM(Sd, som_Liq, GEZ(MLiq2Cake));
      CakeOut.QZero();
      FromCake.QZero();

      if (fFilt2Washings && (iWashings>=0))
        {
        rSpConduit Washings = *IOConduit(iWashings);
        Washings.QSetM(Sd, som_Sol, GEZ(MSol2Filt), IOP_Self(iFiltrate));
        Washings.QAddM(Sd, som_Liq, GEZ(MLiq2Filt));
        }
      else
        {
        Filtrate.QSetM(Sd, som_Sol, GEZ(MSol2Filt), IOP_Self(iFiltrate));
        Filtrate.QAddM(Sd, som_Liq, GEZ(MLiq2Filt));
        }
      }
    }
  else
    {
    MSol = Sd.QMass(som_Sol);
    MLiq = Sd.QMass(som_Liq);
    MSol2Filt = MSol * RqdFiltSolids;

    CakeOut.QSetM(Sd, som_Sol, GEZ(MSol - MSol2Filt), IOP_Self(iCake));

    if (iWash >= 0)
      {//has wash water stream...
      rSpConduit Wash = *IOConduit(iWash);
      Mwash = Wash.QMass(som_Liq);
      solwash = Wash.QMass(som_Sol);
      washSF = solwash / GTZ(Mwash + solwash);

      MLiq2Cake = (RqdCakeMoist * (MSol + solwash - MSol2Filt))/GTZ(1.0 - RqdCakeMoist);
      Filt2Cake = MLiq2Cake * (1.0 - WashEff);
      Wash2Cake = MLiq2Cake - Filt2Cake;
      MLiq2Filt = MLiq - Filt2Cake;

      if (Mwash >= Wash2Cake)
        {
        CakeOut.QAddM(Wash, som_Sol, GEZ(solwash)); //all solids in washwater to report to cake
        CakeOut.QAddM(Wash, som_Liq, GEZ(Wash2Cake));
        CakeOut.QAddM(Sd, som_Liq, GEZ(Filt2Cake));
        ActWashEff = Wash2Cake/Max(MLiq2Cake, 1e-6);
        if (iWashings >= 1 && IOConduit(iWashings)->QMass()>UsableMass)
          {
          rSpConduit Washings = *IOConduit(iWashings);
          Filtrate.QSetM(Sd, som_Liq, GEZ(MLiq - MLiq2Cake), IOP_Self(iFiltrate));
          Washings.QSetM(Wash, som_Liq, GEZ(Mwash - Wash2Cake), IOP_Self(iWashings));
          Washings.QAddM(Sd, som_Liq, GEZ(MLiq2Cake - Filt2Cake));
          double totout, liqfilt, liqwash;
          liqfilt = Filtrate.QMass(som_Liq);
          liqwash = Washings.QMass(som_Liq);
          totout  = liqfilt + liqwash;
          if (totout > 0.0)
            {
            liqfilt = liqfilt / totout;
            liqwash = liqwash / totout;
            Filtrate.QAddM(Sd, som_Sol, GEZ(MSol2Filt * liqfilt));
            Washings.QAddM(Sd, som_Sol, GEZ(MSol2Filt * liqwash));
            }
          else
            {
            Filtrate.QAddM(Sd, som_Sol, 0.0);
            Washings.QAddM(Sd, som_Sol, 0.0);
            }
          }
        else
          {
          Filtrate.QSetM(Sd, som_Liq, GEZ(MLiq2Filt), IOP_Self(iFiltrate));
          Filtrate.QAddM(Wash, som_Liq, GEZ(Mwash - Wash2Cake));
          Filtrate.QAddM(Sd, som_Sol, GEZ(MSol2Filt));
          if (iWashings >= 0)
            IOConduit(iWashings)->QCopy(Wash);
          }
        }
      else
        {
        CakeOut.QAddM(Wash, som_Liq, GEZ(Mwash));
        CakeOut.QAddM(Sd, som_Liq, GEZ(MLiq2Cake - Mwash));
        Filtrate.QSetM(Sd, som_Liq, GEZ(MLiq + Mwash - MLiq2Cake), IOP_Self(iFiltrate));
        Filtrate.QAddM(Sd, som_Sol, GEZ(MSol2Filt));
        ActWashEff = Mwash/Max(MLiq2Cake, 1e-6);
        if (iWashings >= 0)
          {
          rSpConduit Washings = *IOConduit(iWashings);
          Washings.QSetM(Wash, som_Liq, 0.0, IOP_Self(iWashings));
          }
        }
      }
    else
      {
      MLiq2Cake = (RqdCakeMoist * (MSol - MSol2Filt))/GTZ(1.0 - RqdCakeMoist);
      MLiq2Filt = MLiq - MLiq2Cake;
      ActWashEff = 0.0/Max(MLiq2Cake, 1e-6);

      Filtrate.QSetM(Sd, som_Sol, GEZ(MSol2Filt), IOP_Self(iFiltrate));
      Filtrate.QAddM(Sd, som_Liq, GEZ(MLiq2Filt));

      if (fDischarge || PBMode())
        {
        CakeOut.QSetM(Sd, som_Sol, GEZ(MSol - MSol2Filt), IOP_Self(iCake));
        CakeOut.QAddM(Sd, som_Liq, GEZ(MLiq2Cake));
        }
      else
        CakeOut.QZero();
      }
    }

  //put all vapours (if any) to vent (if present)
  int iVent = IOWithId_Self(idVent);
  if (iVent>=0)
    {
    double Qg = Sd.QMass(som_Vap);
    rSpConduit Cvent = *IOConduit(iVent);
    Cvent.QSetM(Sd, som_Vap, Qg, Std_P);
    if (iWash >= 0)
      {
      rSpConduit Wash = *IOConduit(iWash);
      Qg = Wash.QMass(som_Vap);
      Cvent.QAddM(Wash, som_Vap, Qg);
      }
    }

  double WashErr = fabs(WashEff - ActWashEff);
  SetCI(1, bTrackStatus && (WashErr > 1.0e-6));
  SetCI(2, bTrackStatus && (washSF > 0.01));
  //SetCI(3, bTrackStatus && (solwash > 1.0e-6));
  }

//===========================================================================

void FilterPress::EvalDerivs()
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
    case  1: pS="W\tWash Requirements not met"; return 1;
    case  2: pS="W\tLarge amount of solids in Wash Water"; return 1;
    //case  3: pS="N\tSolids detected in Wash Water report to cake"; return 1;
    default:
      return MdlNode::CIStrng(No, pS);
    }
  }

//===========================================================================
