//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty)
 
#include "stdafx.h"
#include "sc_defs.h"
#define  __QCLASSIF_CPP
#include "classif.h"

//==========================================================================
/*#D:#T:Classifier
#X:#u<#l<#b<General Description>>>#n#n
The classifier mixes perfectly all the incoming feed streams and then, based on the user configuration, calculates the amount,
temperature and composition of material reporting to the underflow and overflow. The underflow and overflow liquids have the same composition.
#n#n
Reactions can be specified in a reaction file.
#n#n
The user specifies:#n
 - solids content of the underflow at 25C in g/L,#n
 - solids recovery to the underflow in %,#n
 - chemical reactions and their extents,#n
#n#n
The model converts the solids content at 25C to the solids content at the operating temperature to determine the composition of the underflow and overflow.
#n#n
#n#m<classif1.bmp>#n
#n#n
#n#u<#l<#b<Inputs and Outputs>>>#n#w
Label   Input/Output  Minimum number  Maximum number#n
feed       Input           1              20#n
overflow   Output          1              1#n
underflow  Output          1              1#n
#a
To function the settler must have at least one feed, the overflow and underflow specified.
Up to 20 feeds are allowed.
#n#n
#n#u<#l<#b<Model variables>>>#n#n
#i<Underflow solids at 25C> : #n
#i<Recovery solids to underflow> : #n
#i<Heat loss to atmosphere> : #n
#i<Reactions> : #n
#i<Total mass flow> : #n
#i<Total volume flow in> : #n
#i<Total volume flow out> : 
#n#n
#n#u<#l<#b<Physical Equations>>>#n#n
The density of the under or over flow streams is given by:
#n#m<settler2.bmp>#n
#n#u<#l<#b<Chemical Equations>>>#n#n
Chemical reactions can be specified in a reaction file.
#n#n
*/
//==========================================================================

const byte ioidFeed          = 0;
const byte ioidOFlw          = 1;
const byte ioidUFlw          = 2;
const byte ioidVent          = 3;

static IOAreaRec ClassifierIOAreaList[] =
  {
    {"Classifier Feed",      "Feed",      ioidFeed,         LIO_In0,     nc_MLnk, 1, 20,  IOPipeEntry, 1.0f},
    {"Classifier Overflow",  "Overflow",  ioidOFlw,         LIO_Out0,    nc_MLnk, 1,  1,  IOPipeEntry, (float)0.98},
    {"Classifier Underflow", "Underflow", ioidUFlw,         LIO_Out1,    nc_MLnk, 1,  1,  IOPipeEntry, 0.0f},
    {"",                     "Vent",      ioidVent,         LIO_Out,     nc_MLnk, 0,  1,  IOPipeEntry, 1.0f},
    SPILL2AREA,
    VENT2AREA(ioidVent),
    {NULL}
  }; //This lists the areas of the model where links can be attached.


/*static double Drw_Classifier[] = { DD_Poly, -10,10, -10,-5, 0,-10, 10,-5, 10,10, -10,10,
                                   DD_TagPos, 0, -13.5,
                                   DD_End }; */
static double Drw_Classifier[] = { DD_Poly, -6,1, -6,9, 6,9, 6,1, 2.7,-13, -2.7,-13, -6,1, 6,1,
                                   DD_Poly, -2,9, -2,13, 2,13, 2,9,
                                   DD_Poly, -6,6, -8,6, -8,4, -6,4,
                                   DD_TagPos, 0, -16.5,
                                   DD_End };


enum CLM_Methods { CLUF_SolRec, CLUF_OFConc, CLUF_UQVol };

//--------------------------------------------------------------------------

IMPLEMENT_MODELUNIT(Classifier, "Classif", "", Drw_Classifier, "WasherDecanter", "CL", TOC_PROBAL|TOC_GRP_SEPAR|TOC_STD_KENWALT,
                    "Separation:Classifier",
                    "The classifier mixes perfectly all the incoming feed streams and then, "
                    "based on the user configuration, calculates the amount, temperature and "
                    "composition of material reporting to the underflow and overflow. The "
                    "underflow and overflow liquids have the same composition."
                    " "
                    "The model converts the solids content at 25C to the solids content at the "
                    "operating temperature to determine the composition of the underflow and overflow.")

Classifier::Classifier(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MN_Surge(pClass_, TagIn, pAttach, eAttach)
  {
  AttachIOAreas(ClassifierIOAreaList);
  Contents.SetClosed(False);
//  Contents.SetAllowAdjust(FALSE);
  SetAllowedModes(true, NM_All|SM_Direct|/*SM_Inline|*//*SM_Buffered|*/HM_All);

  iUFCalc       = CLUF_SolRec;
  Reqd_UFConc   = 700.0;
  Reqd_OFConc   = 0.1;
  Reqd_UQVol    = 1.0;
  Reqd_UFSolRec = 0.9;
  UFSolids      = 0.0;
  OFSolids      = 0.0;
  ActUFSolidsConc25 = 0.0;
  ActOFSolidsConc25 = 0.0;
  ActUFSolRec   = 0.0;
  bTrackStatus  = true;
   
  //m_EHX.Open(&CEHX_LossPerQmClass);
  }

//--------------------------------------------------------------------------

Classifier::~Classifier()
  {
  }

//--------------------------------------------------------------------------

void Classifier::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);

  DDB.Visibility();

  DDBValueLst DDB0[]={
    {CLUF_SolRec, "UFSolRec"},
    {CLUF_OFConc, "OFConc"},
    {CLUF_UQVol,  "UFVolFlow"},
    {0}};
	DDB.Text    ("");
  DDB.Text    ("Requirements");
  DDB.Byte    ("",                        "Method",          DC_,     "",       &iUFCalc,            this, isParm, DDB0);
  DDB.Double  ("Rqd_SolidsToUF_Recovery", "RqdUFSolRec",     DC_Frac, "%",      &Reqd_UFSolRec,      this, isParm | (iUFCalc==CLUF_SolRec ? 0 : noView));
  DDB.Double  ("Rqd_UnderFlowSolidsConc25","RqdUFSolConc25", DC_Conc, "g/L",    &Reqd_UFConc,        this, isParm);
  DDB.Double  ("Rqd_OverFlowSolidsConc25","RqdOFSolConc25",  DC_Conc, "g/L",    &Reqd_OFConc,        this, isParm | (iUFCalc==CLUF_OFConc ? 0 : noView));
  DDB.Double  ("Rqd_UnderFlow_VolFlow25", "RqdUFQv25",       DC_Qv,   "m^3/s",  &Reqd_UQVol,         this, isParm | (iUFCalc==CLUF_UQVol  ? 0 : noView));
  DDB.Text    ("Results");
	DDB.Double  ("UnderFlow_Solids",        "UFSolids",        DC_Frac, "%",      &UFSolids,           this, isResult);
  DDB.Double  ("OverFlow_Solids",         "OFSolids",        DC_Frac, "%",      &OFSolids,           this, isResult);
  DDB.Double  ("UnderFlow_SolidsConc@25", "UFSolConc25",     DC_Conc, "g/L",    &ActUFSolidsConc25,  this, isResult);
  DDB.Double  ("OverFlow_SolidsConc@25",  "OFSolConc25",     DC_Conc, "g/L",    &ActOFSolidsConc25,  this, isResult);
  DDB.Double  ("UnderFlow_SolidsRecovery","UFSolRec",        DC_Frac, "%",      &ActUFSolRec,        this, isResult);
 
  DDB.Text    ("");
  m_RB.Add_OnOff(DDB);
  m_EHX.Add_OnOff(DDB);
  DDB.Visibility(NM_Probal|SM_All|HM_All);
  DDB.CheckBox("",                        "TrackStatus",     DC_,     "",       &bTrackStatus,       this, isParm);
  DDB.Visibility();

  DDB.Text    ("");
  BuildDataDefnShowIOs(DDB);
  
  m_RB.BuildDataDefn(DDB);
  m_EHX.BuildDataDefn(DDB);

  if (SolveSurgeMethod())
    {
    DDB.Object(&Contents, this, NULL, NULL, DDB_RqdPage);
    //DDB.Object(&m_PresetImg, this, NULL, NULL, DDB_RqdPage);
    }


//  if (Contents.SpillExists())
//    DDB.Object(&Contents.OverFlow(), this, NULL, NULL, DDB_RqdPage);
  DDB.EndStruct();
  }

//--------------------------------------------------------------------------

void Classifier::EvalJoinPressures(long JoinMask)
  {
  switch (NetMethod())
    {
    case NM_Probal:
      {
      for (int j=0; j<NoProcessJoins(); j++)
        {
        double P = PBPress();//AtmosPress(IODatum_Term(j));
        SetJoinPressure(j, P);
        }
      break;
      }
    case NM_Dynamic:
      MN_Surge::EvalJoinPressures(JoinMask);
      break;
    }
  }
 
//--------------------------------------------------------------------------

void Classifier::EvalProducts(CNodeEvalIndex & NEI)
  {
  if (SolveSurgeMethod())
    {
    MN_Surge::EvalProducts(NEI);
    return;
    }

  StkSpConduit Qm("Qm", chLINEID(), this);

  const int ioUFlw = IOWithId_Self(ioidUFlw);
  const int ioOFlw = IOWithId_Self(ioidOFlw);
  const int ioVent = IOWithId_Self(ioidVent);
  SpConduit & Qu = *IOConduit(ioUFlw);
  SpConduit & Qo = *IOConduit(ioOFlw);
  SpConduit * pVent = (ioVent>=0 ? IOConduit(ioVent) : NULL);

  Qm().QZero();

  SigmaQInPMin(Qm(), som_ALL, Id_2_Mask(ioidFeed));
  flag HasFlw = (Qm().QMass(som_ALL)>UsableMass);

  m_RB.EvalProducts(Qm());
  m_EHX.EvalProducts(Qm());

  //put all vapours (if any) to vent (if present)...
  flag bVentErr = false;
  const double Qg = Qm().QMass(som_Gas);
  if (Qg>1e-12)
    {
    if (pVent)
      pVent->QSetM(Qm(), som_Gas, Qg, Std_P);
    else
      bVentErr = true;
    }
  else if (pVent)
    pVent->QZero();

  // Density Calc should be done @ 25dC
  double Ts=Qm().Temp();
  Qm().SetTemp(C_2_K(25.0));


  double RhoS=GTZ(Qm().Rho(som_Sol));
  double RhoL=GTZ(Qm().Rho(som_Liq));

  double Fs=Qm().MassFrac(som_Sol);
  double Fl=Qm().MassFrac(som_Liq);

  double Ms=Qm().QMass(som_Sol);
  double Ml=Qm().QMass(som_Liq);

  double ConcFd = Ms/GTZ(Qm().QVolume());
  // Underflow Concentration must be greater than the Feed
  double ConcUs = Max(Reqd_UFConc, ConcFd+0.1);
  SetCI(1, bTrackStatus && HasFlw && ConcUs != Reqd_UFConc);

  switch (iUFCalc)
    {
    case CLUF_UQVol:
      {
      // Underflow solids at 25 dC in g/L
      double FQm=Ms+Ml;

      // MassFrac in Over & Under
      double USFrac=Range(0.0, RhoS/GTZ(RhoS*RhoL/ConcUs+RhoS-RhoL), 1.0);
      double ULFrac=Range(0.0, 1.0-USFrac, 1.0);

      double URho=1.0/GTZ(USFrac/RhoS+ULFrac/RhoL);

      double UQm=Range(0.0, URho*Reqd_UQVol, FQm);
      double OQm=FQm-UQm;

      double S2U=Range(0.0, UQm*USFrac, Ms);
      double L2U=Range(0.0, UQm*ULFrac, Ml);

      double S2O=Range(0.0, Ms-S2U, Ms);
      double L2O=Range(0.0, Ml-L2U, Ml);

      Qu.QSetM(Qm(), som_Sol, S2U, Std_P);
      Qu.QAddM(Qm(), som_Liq, L2U);

      Qo.QSetM(Qm(), som_Sol, S2O, Std_P);
      Qo.QAddM(Qm(), som_Liq, L2O);
      break;
      }
    case CLUF_OFConc:
      {
      // Overflow Concentration must be less than the Feed
      double ConcOs = Min(Reqd_OFConc, ConcFd-0.1);
      SetCI(2, bTrackStatus && HasFlw && ConcOs != Reqd_OFConc);
      ConcOs = Max(0.0, ConcOs);

      if (ConcFd>=ConcUs)
        {
        // Too Dense
        Qu.QSetF(Qm(), som_ALL, 1.0, Std_P);
        Qo.QSetF(Qm(), som_ALL, 0.0, Std_P);
        }
      else if (ConcFd<=ConcOs)
        {
        // Too Fluid
        Qu.QSetF(Qm(), som_ALL, 0.0, Std_P);
        Qo.QSetF(Qm(), som_ALL, 1.0, Std_P);
        }
      else
        {
        // MassFrac in Over & Under
        double Os=RhoS*ConcOs/GTZ(RhoL*RhoS-RhoL*ConcOs+RhoS*ConcOs);
        double Us=RhoS*ConcUs/GTZ(RhoL*RhoS-RhoL*ConcUs+RhoS*ConcUs);

        Os=Range(0.0, Os, 1.0);
        Us=Range(0.0, Us, 1.0);

        double FQm=Ms+Ml;
        double UQm=FQm*Range(0.0, (Fs-Os)/GTZ(Us-Os), 1.0);
        double OQm=FQm-UQm;

        double Fs2U=Range(0.0, UQm*Us/GTZ(Ms), 1.0);
        double Fl2U=Range(0.0, UQm*(1.0-Us)/GTZ(Ml), 1.0);

        Qu.QSetF(Qm(), som_Sol, Fs2U, Std_P);
        Qu.QAddF(Qm(), som_Liq, Fl2U);

        Qo.QSetF(Qm(), som_Sol, 1.0-Fs2U, Std_P);
        Qo.QAddF(Qm(), som_Liq, 1.0-Fl2U);
        }
      break;
      }
    case CLUF_SolRec:
      {
      double URec = Range(0.0, Reqd_UFSolRec,  1.0);

      ConcUs=Min(ConcUs, RhoS);
      double Msu=URec*Ms;
      double Mlu=Range(0.0, RhoL*Msu*(1.0/ConcUs-1.0/RhoS), Ml);

      Qu.QSetM(Qm(), som_Sol, Msu, Std_P);
      Qu.QAddM(Qm(), som_Liq, Mlu);

      Qo.QSetM(Qm(), som_Sol, GEZ(Ms-Msu), Std_P);
      Qo.QAddM(Qm(), som_Liq, GEZ(Ml-Mlu));
      break;
      }
    }

  // Restore Temp
  Qu.SetTemp(Ts);
  Qo.SetTemp(Ts);

  //Reactions in underflow...
  //RB.EvalProducts(Qu);

  //results...
  Qu.SanityCheck();
  Qo.SanityCheck();
  UFSolids = Qu.MassFrac(som_Sol);
  OFSolids = Qo.MassFrac(som_Sol);
  ActUFSolidsConc25 = Qu.PhaseConc(C_2_K(25.0), som_Sol, som_ALL);
  ActOFSolidsConc25 = Qo.PhaseConc(C_2_K(25.0), som_Sol, som_ALL);
  const double UFSolidsQm = Qu.QMass(som_Sol);
  const double OFSolidsQm = Qo.QMass(som_Sol);
  ActUFSolRec = UFSolidsQm/GTZ(UFSolidsQm+OFSolidsQm);
  double ActUFQVol25 = Reqd_UQVol;//Qu.??? //todo!!! complete this!

  if (bTrackStatus && HasFlw)
    {
    double UFConcErr = fabs(ActUFSolidsConc25 - Reqd_UFConc);
    SetCI(3, UFConcErr > 1.0e-5);
    switch (iUFCalc)
      {
      case CLUF_UQVol:
        {
        ClrCI(4);
        double UFVolErr = fabs(ActUFQVol25 - Reqd_UQVol);
        SetCI(5, UFVolErr > 1.0e-6);
        break;
        }
      case CLUF_OFConc:
        {
        double OFConcErr = fabs(ActOFSolidsConc25 - Reqd_OFConc);
        SetCI(4, OFConcErr > 1.0e-5);
        ClrCI(5);
        break;
        }
      case CLUF_SolRec:
        ClrCI(4);
        ClrCI(5);
        break;
      }
    }
  else
    {
    ClrCI(3);
    ClrCI(4);
    ClrCI(5);
    }
  SetCI(6, HasFlw && bVentErr);
  }
   
//--------------------------------------------------------------------------

flag Classifier::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case 1: pS="W\tSpecified U/F Concentration Too Low"; return 1;
    case 2: pS="W\tSpecified O/F Concentration Too High"; return 1;
    case 3: pS="W\tU/F Solids Concentration Requirements not met."; return 1;
    case 4: pS="W\tO/F Solids Concentration Requirements not met."; return 1;
    case 5: pS="W\tU/F Volume Flow Requirements not met."; return 1;
    case 6: pS="W\tNo Vent Defined."; return 1;
    default:                                               
      return MN_Surge::CIStrng(No, pS);
    }
  }

//--------------------------------------------------------------------------
//==========================================================================
//
//
//
//==========================================================================
