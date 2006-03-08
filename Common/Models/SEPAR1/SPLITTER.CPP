//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#define  __SPLITTER_CPP
#include "splitter.h"

//==========================================================================
/*#D:#T:Splitter                       
#X:#h<General Description>#N#N
The splitter model is used to split the feed streams into two user-defined 
streams.  All of the feed streams to the Splitter will be mixed and the 
resulting homogenous product will be split between the under and over size 
streams.  If the user connects more than one pipe to either the over or 
under size stream, then the over or under size product will be split equally 
between the streams.

#nThe splitter is used to emulate a screen on a very simple basis.  If the 
user has size distribution information, then the actual screen model will 
split the feed stream based on well-defined screening theory.

#nThe splitter can operate in any one of three modes:
#na) The user defines individual specie splits to the oversize.  A maximum 
of two species may be specified.
#nb) The user defines the percentage of solids and liquids in the feed stream/s 
that must report to the oversize stream.
#nc) The user defines the percentage of solids in the feed that must report to 
the oversize, as well as the percent solids in the oversize stream.  The model 
will calculate the liquid split to give the required solids percentage.

#n#h<Inputs and Outputs>#w
Label    Input/Output  Minimum number  Maximum number#n
Feed            Input          1              20#n
UnderSize       Output         1              4#n
OverSize        Output         1              4#n
#a#n

#n#n#h<Variables to be supplied by the user>#n
#n#i<Method:>The user is required to define the split method for the unit.  Three 
choices exist:
#n1.Specie - in which case the unit will remove a user defined fraction of the 
chosen specie(s) to the oversize stream.
#n2.Feed_Sol/Liq - the unit separates the feed solids and liquids in a set proportion.
#n3.OS_Sol/Liq - the unit sends a user defined percentage of the solids to the oversize 
stream, at set percentage solids.
#n#i<Specie:>This is required if the 'Specie' split method is chosen.  The user chooses 
which specie(s) will be split out from the feed stream.
#n#i<Split:>This is required if the 'Specie' split method is chosen.  The user inputs the 
percentage of the specie(s) which are sent to the overflow stream.
#n#i<OSSol:>This is required if the 'Feed_Sol/Liq' or 'OS_Sol/Liq' split method is chosen.  
The user is required to input the percentage of the feed solids which report to the 
oversize stream.
#n#i<OSLiq:>This is also only required if the 'Feed_Sol/Liq' split method is chosen.  
The user is required to input the percentage of the feed liquids which report to the 
oversize stream.
#n#i<%Solid:>This is also only required if the 'OS_Sol/Liq' split method is chosen.  The 
user defines the percentage solids in the oversize stream.
#n#i<Z_Rqd:>The relative elevation of the splitter with respect to the common datum 
of the system. Only required in dynamic mode.

#n#n#h<Variables calculated by the model>#n
#n#i<OSDens:>The density of the oversize stream. 
#n#i<USDens:>The density of the undersize stream.  

#n#h<Other>#n
Default model prefix:SPL#n
Short name:Splitter#n
Model type:Unit#n
#G:Units
*/

//==========================================================================

//ACCESS_SPECIE(H2O,"H2O");

const byte ioidFd = 0;
const byte ioidUS = 1;
const byte ioidOS = 2;

static IOAreaRec SplitterIOAreaList[] =
  {{"Feed or wash water",         "Feed",       ioidFd, LIO_In0 ,   nc_MLnk, 1, 20, 0},
   {"Liquid/under size material", "UnderSize",  ioidUS, LIO_Out0,   nc_MLnk, 1,  1, IOSetXfer|IOGRP(0)},
   {"Over size material/solids",  "OverSize",   ioidOS, LIO_Out1,   nc_MLnk, 1,  1, IOSetXfer|IOGRP(0)},
   {NULL}};

static double Drw_Splitter[] = { DD_Poly, -1,5, 1,5, 3,-5, -3,-5, -1,5,
                        DD_End };

enum SPM_FiltrateMethods { SPM_Specie, SPM_FeedSolLiq, SPM_OSSolLiq };

//--------------------------------------------------------------------------

IMPLEMENT_MODELUNIT(Splitter, "Splitter", "1", Drw_Splitter, "MixerSplitter", "SPL", TOC_ALL|TOC_DYNAMICFLOW|TOC_GRP_SEPAR|TOC_STD_KENWALT,
                    "Separation:Splitter",
                    "General Purpose Splitter") 

Splitter::Splitter(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MdlNode(pClass_, TagIn, pAttach, eAttach),
  QFeed("QFeed", this, TOA_Embedded)//
  {
  AttachIOAreas(SplitterIOAreaList);
  iSplitMethod  = SPM_Specie;
  //bShowQFeed    = 0;
  OverS         = 0.1;
  RqdSolidsToOS = 0.5;
  RqdLiqToOS    = 0.05;
  RqdOSSolids   = 0.45;
  iSep1         = -1;
  iSep2         = -1;
  iSep3         = -1;
  OSDens        = 0.0;
  USDens        = 0.0;
	OSSolids      = 0.0;
  USSolids      = 0.0;
  ActOSSolidsConc25 = 0.0;
  ActUSSolidsConc25 = 0.0;
  bTrackStatus  = true;
  }

// -------------------------------------------------------------------------
/*This provides access to the variables of the model and calls the
BuildDataDefn for objects used by the model.*/
void Splitter::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);

  DDB.Visibility(SM_DynBoth|HM_All);
  BuildDataDefnElevation(DDB);
  static DDBValueLst DDB1[]={
    {(int)SPM_Specie, "Specie"},
    {(int)SPM_FeedSolLiq, "Feed_Sol/Liq"},
    {(int)SPM_OSSolLiq, "OS_Sol/Liq"},
    {0}};
  DDB.Visibility();
	DDB.Text    ("");
  DDB.Text    ("Requirements");
  if (PrjFileVerNo()<49)
    {
    DDB.Byte    ("Split_Type",      "Method",   DC_,     "" ,      &iSplitMethod,  this, isParm, DDB1);
	  if (iSplitMethod == SPM_Specie)
		  {
	    DDB.Short   ("",              "Specie1",  DC_    , "" ,      &iSep1,         this, isParm, SDB.DDBSpListDash());
		  DDB.Short   ("",              "Specie2",  DC_    , "" ,      &iSep2,         this, isParm, SDB.DDBSpListDash());
		  DDB.Short   ("",              "Specie3",  DC_    , "" ,      &iSep3,         this, isParm, SDB.DDBSpListDash());
	    DDB.Double  ("OverSizeSplit", "OS_Split", DC_Frac, "%",      &OverS,         this, isParm);
		  }
	  else if (iSplitMethod == SPM_FeedSolLiq)
		  {
		  DDB.Double  ("OS_Solids",     "OSSol",    DC_Frac, "%",      &RqdSolidsToOS,      this, isParm);
		  DDB.Double  ("OS_Liquid",     "OSLiq",    DC_Frac, "%",      &RqdLiqToOS,         this, isParm);
		  }
	  else
		  {
		  DDB.Double  ("OS_Solids",     "OSSol",    DC_Frac, "%",      &RqdSolidsToOS,      this, isParm);
		  DDB.Double  ("Percent_Solids","%Solid",   DC_Frac, "%",      &RqdOSSolids,        this, isParm);
		  }
    DDB.Text    ("Results");
    DDB.Double  ("OS_Density",              "OSDens",          DC_Rho,  "kg/m^3", &OSDens,             this, isResult|0);
    DDB.Double  ("US_Density",              "USDens",          DC_Rho,  "kg/m^3", &USDens,             this, isResult|0);
    }
  else
    {
    DDB.Byte    ("Split_Method",            "Method",          DC_,     "" ,      &iSplitMethod,       this, isParm|SetOnChange, DDB1);
    DDB.Visibility(SHM_All, (iSplitMethod == SPM_Specie));
    DDB.Short   ("",                        "Specie1",         DC_    , "" ,      &iSep1,              this, isParm, SDB.DDBSpListDash());
    DDB.Short   ("",                        "Specie2",         DC_    , "" ,      &iSep2,              this, isParm, SDB.DDBSpListDash());
    DDB.Short   ("",                        "Specie3",         DC_    , "" ,      &iSep3,              this, isParm, SDB.DDBSpListDash());
    DDB.Double  ("OverSizeSplit",           "OS_Split",        DC_Frac, "%",      &OverS,              this, isParm);
    DDB.Visibility(SHM_All, (iSplitMethod == SPM_FeedSolLiq || iSplitMethod == SPM_OSSolLiq));
    DDB.Double  ("Rqd_Solids_To_OverSize",  "RqdSolToOS",      DC_Frac, "%",      &RqdSolidsToOS,      this, isParm);
    DDB.Visibility(SHM_All, (iSplitMethod == SPM_FeedSolLiq));
    DDB.Double  ("Rqd_Liquid_To_OverSize",  "RqdLiqToOS",      DC_Frac, "%",      &RqdLiqToOS,         this, isParm);
    DDB.Visibility(SHM_All, (iSplitMethod == SPM_OSSolLiq));
    DDB.Double  ("Rqd_OverSize_Solids",     "RqdOSSolids",     DC_Frac, "%",      &RqdOSSolids,        this, isParm);
    DDB.Visibility();
    DDB.Text    ("Results");
    DDB.Double  ("OverSize_Density",        "OSRho",           DC_Rho,  "kg/m^3", &OSDens,             this, isResult|0);
    DDB.Double  ("UnderSize_Density",       "USRho",           DC_Rho,  "kg/m^3", &USDens,             this, isResult|0);
	  DDB.Double  ("OverSize_Solids",         "OSSolids",        DC_Frac, "%",      &OSSolids,           this, isResult|0);
    DDB.Double  ("UnderSize_Solids",        "USSolids",        DC_Frac, "%",      &USSolids,           this, isResult|InitHidden);
    DDB.Double  ("OverSize_SolidsConc@25",  "OSSolConc25",     DC_Conc, "g/L",    &ActOSSolidsConc25,  this, isResult|InitHidden);
    DDB.Double  ("UnderSize_SolidsConc@25", "USSolConc25",     DC_Conc, "g/L",    &ActUSSolidsConc25,  this, isResult|InitHidden);
    }

  DDB.Visibility(SM_Direct|HM_All);
	DDB.Text    ("");
  //DDB.CheckBox("ShowQFeed",         "",  DC_,     "",      &bShowQFeed,         this, isParm|SetOnChange);
  DDB.CheckBox("",                        "TrackStatus",     DC_,     "",       &bTrackStatus,       this, isParm);
  DDB.Visibility();
	DDB.Text    ("");
  BuildDataDefnShowIOs(DDB);

  /*//if (SolveDirectMethod())
  //  DDB.Object(&QFeed, this, NULL, NULL, DDB_RqdPage, -1, bShowQFeed ? 0 : noView);
  if (bShowQFeed && SolveDirectMethod())
    DDB.Object(&QFeed, this, NULL, NULL, DDB_RqdPage, -1);*/

  DDB.EndStruct();
  }

//--------------------------------------------------------------------------

Splitter::~Splitter()
  {
  }

//--------------------------------------------------------------------------

flag Splitter::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=MdlNode::ValidateData(VDB);
  if ((iSplitMethod == SPM_Specie) && 
      ((iSep1<0 || iSep1>=SDB.Count()) && (iSep2<0 || iSep2>=SDB.Count()) && (iSep3<0 || iSep3>=SDB.Count())) )
    OK=False;
  return OK;
  }

//---------------------------------------------------------------------------

void Splitter::ConfigureJoins()
  {
  for (int i=0; i<NoFlwIOs(); i++)
    SetIO_Open(i, 0, false, ESS_Denied);
  }

//--------------------------------------------------------------------------
/*
void Splitter::EvalJoinPressures(long JoinMask)
  {
  for (int i = 0; i < NoFlwIOs(); i++)
    Set_IOP_Self(i, AtmosPress(IODatum_Term(i)));
  }
*/
void Splitter::EvalJoinPressures(long JoinMask)
  {
  switch (NetMethod())
    {
    case NM_Probal:
      {
      for (int j=0; j<NJoins(); j++)
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
/*This determines what material should leave through each outlet,
and rate of change of the contents resulting from the flow, of material, out of each outlet
of the surge unit.*/
void Splitter::EvalProducts(long JoinMask)
  {
  SpConduit & Qo = *IOConduit(IOWithId_Self(ioidUS));
  SpConduit & Qu = *IOConduit(IOWithId_Self(ioidOS));

  OverS = Range(0.0, OverS, 1.0);
  RqdSolidsToOS = Range(0.0, RqdSolidsToOS, 1.0);
  RqdLiqToOS = Range(0.0, RqdLiqToOS, 1.0);

  SigmaQInPMin(QFeed, som_ALL, Id_2_Mask(ioidFd));
  flag HasFlw = (QFeed.QMass(som_ALL)>UsableMass);

  double T=QFeed.Temp();

  if (iSplitMethod == SPM_Specie)
    {
    Qo.QSetF(QFeed, SetMass_Frac, 1.0, 1.0, 1.0, Std_P);
    Qu.QSetF(QFeed, SetMass_Frac, 0.0, 0.0, 0.0, Std_P);
    if (iSep1>=0)
      {
      Qo.SetVMass(iSep1, QFeed, QFeed.VMass[iSep1] * (1.0 - OverS));
      Qu.SetVMass(iSep1, QFeed, QFeed.VMass[iSep1] * OverS);
      }
    if (iSep2>=0)
      {
      Qo.SetVMass(iSep2, QFeed, QFeed.VMass[iSep2] * (1.0 - OverS));
      Qu.SetVMass(iSep2, QFeed, QFeed.VMass[iSep2] * OverS);
      }
    if (iSep3>=0)
      {
      Qo.SetVMass(iSep3, QFeed, QFeed.VMass[iSep3] * (1.0 - OverS));
      Qu.SetVMass(iSep3, QFeed, QFeed.VMass[iSep3] * OverS);
      }
		}
	else if (iSplitMethod == SPM_FeedSolLiq)
		{
    Qu.QSetF(QFeed, SetMass_Frac, RqdSolidsToOS, RqdLiqToOS, 1.0, Std_P);
    Qo.QSetF(QFeed, SetMass_Frac, 1.0-RqdSolidsToOS, 1.0-RqdLiqToOS, 0.0, Std_P);
    }
	else
		{
    RqdOSSolids = Range(0.01, RqdOSSolids, 0.99);
    double Qs = QFeed.QMass(som_Sol);
    double Ql = QFeed.QMass(som_Liq);
    double Us = RqdOSSolids;
    
    double Su = Qs * RqdSolidsToOS;
    double Lu = Range(0.0, Su*(1.0 - Us)/Us, Ql);

    double So = Qs - Su;
    double Lo = Ql - Lu;

    Qu.QSetM(QFeed, som_Sol, Su, Std_P);
    Qu.QAddM(QFeed, som_Liq, Lu);
  
    Qo.QSetM(QFeed, som_Sol, So, Std_P);
    Qo.QAddM(QFeed, som_Liq, Lo);
		}

  Qo.SetTemp(T);
  Qu.SetTemp(T);
  
  //results...
  Qu.SanityCheck();
  Qo.SanityCheck();
  OSDens = Qu.Rho(som_ALL);
  USDens = Qo.Rho(som_ALL);
  OSSolids = Qu.MassFrac(som_Sol);
  USSolids = Qo.MassFrac(som_Sol);
  ActOSSolidsConc25 = Qu.PhaseConc(C_2_K(25.0), som_Sol, som_ALL);
  ActUSSolidsConc25 = Qo.PhaseConc(C_2_K(25.0), som_Sol, som_ALL);

  double OSErr = fabs(OSSolids - RqdOSSolids);
  SetCI(1, bTrackStatus && HasFlw && (iSplitMethod == SPM_OSSolLiq) && OSErr > 1.0e-6);
  }

//--------------------------------------------------------------------------

dword Splitter::ModelStatus()
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

//===========================================================================

flag Splitter::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case 1: pS="W\tRqdSolToOS not achieved";            return 1;
    default:
      return MdlNode::CIStrng(No, pS);
    }
  };

//==========================================================================
