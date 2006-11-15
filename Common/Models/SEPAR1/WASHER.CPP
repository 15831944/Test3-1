//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#define  __WASHER_CPP
#include "Washer.h"
//#include "optoff.h"

//==========================================================================
/*#D:#T:Washer
#X:#n#h<General Description>#NThe washer acts as a thickener with imperfect
mixing.  The imperfect mixing is controlled by a user defined variable, Mixeff, which
defines the extent of the mixing between the feed stream and all other inputs.  If the
washer is perfectly mixed, then MixEff = 100%, anything less will result in imperfect
mixing.
#nIn dynamic mode, the washer is based on a set of user defined curves relating the
percentage solids to the height of the solids bed in the thickener. The bed height is
calculated using the mass of solids in the washer and the area of the washer.
#nThe curves can be obtained from plant data, if available, or from experimental data.
If no data is available, the default curves can be used.

#n#n#h<Inputs and Outputs>#w
Label    Input/Output  Minimum number  Maximum number#n
Feed          Input          1              4#n
Input         Input          0              20#n
OverFlow      Output         1              4#n
UnderFlow     Output         1              4#n
#a

#n#n#h<Variables to be supplied by the user>#n
#n#i<% MixEff:> The degree of mixing efficiency in the washer.  100% gives perfect mixing.
#n#i<% RqdUFSolids:> The required % solids in the underflow from the washer. This
variable is only required in mass balancing mode.
#n#i<% RqdOFSolids:> The required % solids in the overflow from the washer. This
variable is also only required in mass balancing mode.
#n#i<Datum required:> The relative elevation of the washer with respect to the
common datum of the system.
#n#n#h<Variables calculated by the model>#n
#n#i<Datum:> The height of the washer above the system datum.
#n#i<UFSolids:> The actual % solids in the underflow.
#n#i<OFSolids:> The actual % solids in the overflow.

#n#n#h<Other>#n
Default model prefix:WSH#n
Short name:Washer#n
Model type:Unit#n
#G:Units
*/

//==========================================================================

const byte ioidFd = 0;
const byte ioidIt = 1;
const byte ioidOF = 2;
const byte ioidUF = 3;
const byte ioidVent = 4;

static IOAreaRec WasherIOAreaList[] =
  {
    {"Washer Feed",      "Feed"      , ioidFd,   LIO_In0 , nc_MLnk, 1, 10,IOPipeEntry, 1.0f},
    {"Washer Input",     "Input"     , ioidIt,   LIO_In1,  nc_MLnk, 0, 20,IOPipeEntry, 1.0f}, //ie 'wash water'
    {"Washer Overflow",  "OverFlow"  , ioidOF,   LIO_Out0, nc_MLnk, 1, 1, IOPipeEntry, (float)0.98},
    {"Washer Underflow", "UnderFlow" , ioidUF,   LIO_Out1, nc_MLnk, 1, 1, IOPipeEntry, 0.0f},
    {"",                 "Vent"      , ioidVent, LIO_Out , nc_MLnk, 0, 1, IOPipeEntry, 1.0f},
    SPILL2AREA("Spills", IOId_Spill2Area),
    VENT2AREA("Vents",   IOId_Vent2Area),
    {NULL}
  };

static double Drw_Washer[] = { DD_Poly, -25,0, -25,-3, -22,-3, -22,0, 22,0, 22,-3, 25,-3,
											 25,0, 25,-3, 22,-3, 22,-4, 0,-13, -22,-4, -22,-3,
											 -22,-4, 22,-4, DD_End };

enum OFM_OverFlowMethods { OFM_OFSolFrac, OFM_OFSolConc };
enum IBM_InputBypassMethods { IBM_None, IBM_Overflow, IBM_Underflow };

//--------------------------------------------------------------------------

IMPLEMENT_MODELUNIT(Washer, "Washer", "1", Drw_Washer, "WasherDecanter", "WSH", TOC_ALL|TOC_GRP_SEPAR|TOC_STD_KENWALT,
                    "Separation:Washer(1)",
                    "Washer parameters and connections "
                    "determine underflow and overflow")

Washer::Washer(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MN_Surge(pClass_, TagIn, pAttach, eAttach),
  UFSolidsFn("DataOnly", "UF_Solids", this, TOA_Embedded),
  OFSolidsFn("DataOnly", "OF_Solids", this, TOA_Embedded),
  Qw("Qw", this)
  {
  AttachIOAreas(WasherIOAreaList, &PipeEntryGroup);
  Contents.SetClosed(False);
  Contents.SetPreset(&m_Preset, &m_PresetImg);

  SetAllowedModes(true, NM_All|SM_Direct|/*SM_Inline|*/SM_Buffered|HM_All);//"Transfer", "Inline", "Buffered");

	MixEff          = 1.0;
  bMixEffLiqOnly  = false;
  iRqdOFMethod    = OFM_OFSolFrac;
  iRqdBypassMethod= IBM_Overflow;
  IntOFSolidsUsed = dNAN;
  IntUFSolidsUsed = dNAN;
  RqdUFSolids     = 0.4;
  RqdOFSolids     = 0.00001;
  RqdOFSolConc    = 0.1;
  UFSolids        = RqdUFSolids;
  OFSolids        = RqdOFSolids;
  BedHgt          = 0.0;
  ActUFSolidsConc25 = 0.0;
  ActOFSolidsConc25 = 0.0;
  iScanEffSpecie  = 0;
  ActScandrettEff = 0.0;
  BypassQm        = 0.0;
  bTrackStatus    = true;
  }

//--------------------------------------------------------------------------

Washer::~Washer()
  {
  }

//--------------------------------------------------------------------------
/*#F:This provides access to the variables of the model and calls the
BuildDataDefn for objects used by the model.*/
void Washer::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);

  DDB.Visibility(NM_Dynamic|SM_All|HM_All);
  BuildDataDefnElevation(DDB);
  DDB.Text    ("");
  DDB.Double  ("Bed_Height",              "BedHgt",          DC_L,    "m",      &BedHgt,             this, isResult|0);
  DDB.Double  ("Pressure",                "P",               DC_P,    "kPag",   xidPMean,            this, isResult|0);
  DDB.Double  ("Density",                 "Rho",             DC_Rho,  "kg/m^3", xidRho,              this, isResult|0);
  DDB.Double  ("Level",                   "Lvl",             DC_Frac, "%",      xidLevel,            this, isResult|0);
  DDB.Double  ("Mass_Accum",              "QmAcc",           DC_Qm,   "kg/s",   &m_QmAcc,            this, isResult|0);
  DDB.Double  ("Vol_Accum",               "QvAcc",           DC_Qv,   "L/s",    &m_QvAcc,            this, isResult|0);

  static DDBValueLst DDB0a[]=
    {
      {OFM_OFSolFrac, "OverFlowSolidsFraction"},
      {OFM_OFSolConc, "OverFlowSolidsConcentration"},
      {0}
    };
  static DDBValueLst DDB0b[]=
    {
      {OFM_OFSolFrac, "OverFlowSolidsFraction"},
      {OFM_OFSolConc, "OverFlowSolidsConcentration", MDD_Hidden },
      {0}
    };
  static DDBValueLst DDB1[]=
    {
      {IBM_None, "None"},
      {IBM_Overflow, "OverFlow"},
      {IBM_Underflow, "Underflow"},
      {0}
    };

  DDB.Visibility(NM_Probal|SM_All|SM_Inline|HM_All);
  DDB.Text    ("");
  DDB.Text    ("Requirements");
  DDB.Byte    ("Bypass_Option",           "BypassOpt",       DC_,     "",       &iRqdBypassMethod,   this, isParm|AffectsStruct, DDB1);
  DDB.Visibility(NM_Probal|SM_All|SM_Inline|HM_All, (iRqdBypassMethod != IBM_None));
  DDB.Double  ("Mixing_Efficiency",       "MixingEff",       DC_Frac, "%",      &MixEff,             this, isParm);
  DDB.CheckBox("Mixing_Eff_LiquidsOnly",  "MixEffLiqOnly",   DC_,     "",       &bMixEffLiqOnly,     this, isParm);
  DDB.Visibility(NM_Probal|SM_All|SM_Inline|HM_All);
  DDB.Double  ("Rqd_UnderFlow_Solids",    "RqdUFSolids",     DC_Frac, "%",      &RqdUFSolids,        this, isParm);
  DDB.Byte    ("OverFlow_Method",         "Method",          DC_,     "",       &iRqdOFMethod,       this, isParm|AffectsStruct, iRqdBypassMethod == IBM_Underflow ? DDB0b : DDB0a);
  DDB.Visibility(NM_Probal|SM_All|SM_Inline|HM_All, (iRqdOFMethod == OFM_OFSolFrac));
  DDB.Double  ("Rqd_OverFlow_Solids",     "RqdOFSolids",     DC_Frac, "%",      &RqdOFSolids,        this, isParm);
  DDB.Visibility(NM_Probal|SM_All|SM_Inline|HM_All, (iRqdOFMethod == OFM_OFSolConc));
  DDB.Double  ("Rqd_OverFlowSolidsConc25","RqdOFSolConc25",  DC_Conc, "g/L",    &RqdOFSolConc,       this, isParm);
  DDB.Visibility(NM_Probal|SM_All|SM_Inline|HM_All, (iRqdBypassMethod != IBM_Underflow));
  DDB.Double  ("Internal_OF_SolidsUsed",  "IntOFSolidsUsed", DC_Frac, "%",      &IntOFSolidsUsed,    this, isResult|InitHidden);//|noView);
  DDB.Visibility(NM_Probal|SM_All|SM_Inline|HM_All, (iRqdBypassMethod == IBM_Underflow));
  DDB.Double  ("Internal_UF_SolidsUsed",  "IntUFSolidsUsed", DC_Frac, "%",      &IntUFSolidsUsed,    this, isResult|InitHidden);//|noView);
  DDB.Visibility();
  DDB.Text    ("Results");
  DDB.Visibility(NM_Probal|SM_All|SM_Inline|HM_All, (iRqdBypassMethod != IBM_None));
  DDB.Double  ("Bypass_Flowrate",         "BypassQm",        DC_Qm,   "kg/s",   &BypassQm,           this, isResult);
  DDB.Visibility();
	DDB.Double  ("UnderFlow_Solids",        "UFSolids",        DC_Frac, "%",      &UFSolids,           this, isResult);
  DDB.Double  ("OverFlow_Solids",         "OFSolids",        DC_Frac, "%",      &OFSolids,           this, isResult);
  DDB.Double  ("UnderFlow_SolidsConc@25", "UFSolConc25",     DC_Conc, "g/L",    &ActUFSolidsConc25,  this, isResult|InitHidden);
  DDB.Double  ("OverFlow_SolidsConc@25",  "OFSolConc25",     DC_Conc, "g/L",    &ActOFSolidsConc25,  this, isResult|InitHidden);
  DDB.Byte    ("",                        "ScanEffSpecie",   DC_,     "",       &iScanEffSpecie,     this, isParm, SDB.DDBLiqSpList());
  DDB.Double  ("Scandrett_Wash_Eff",      "ScandrettEff",    DC_Frac, "%",      &ActScandrettEff,    this, isResult);//|noFile|noSnap);

  DDB.Text    (" ");
  RB.Add_OnOff(DDB);
  EHX.Add_OnOff(DDB);
  DDB.Visibility(NM_Probal|SM_All|SM_Inline|HM_All);
  DDB.CheckBox("",                        "TrackStatus",     DC_,     "",       &bTrackStatus,       this, isParm);
  DDB.Visibility();

  EHX.BuildDataDefn(DDB);

  DDB.Text    (" ");
	BuildDataDefnShowIOs(DDB);
  BuildDataDefnIOOpts(DDB);

  if (SolveSurgeMethod())
    {
    DDB.Object(&Contents, this, NULL, NULL, DDB_RqdPage);
    DDB.Object(&m_PresetImg, this, NULL, NULL, DDB_RqdPage);
    }

  RB.BuildDataDefn(DDB, NULL, NULL, DDB_RqdPage, 0);

  DDB.Visibility(NM_Dynamic|SM_All|HM_All);
	DDB.Object(&UFSolidsFn, this, NULL, NULL, DDB_RqdPage);
  DDB.Object(&OFSolidsFn, this, NULL, NULL, DDB_RqdPage);
  DDB.Visibility();

  DDB.EndStruct();
  }


//--------------------------------------------------------------------------
// x is required "internal" overflow solids fraction
//
class CWasherOFFnd : public MRootFinderBase
  {
  public:
    SpConduit &Sd, &Ww, &Fd, &Qw, &Co, &Cu;
    double IntOFSolidsUsed;
    double IntUFSolidsUsed;
    flag bVentErr;
    double dBypassQm;
  protected:
    SpConduit* pVent;
    MN_Surge* pSurge;

    double POut;
    double RqdUFSolids;
    double MixEff;
    int    iRqdOFMethod;
    flag   BypassLiqOnly;
    int    iRqdBypassMethod;
    double RqdOFSolids;

    static CToleranceBlock s_Tol;

  public:
    CWasherOFFnd(SpConduit &Sd_, SpConduit &Ww_, SpConduit &Fd_, SpConduit &Qw_, SpConduit &Co_, SpConduit &Cu_, SpConduit* pVent_, MN_Surge* pSurge_,
      double POut_, double RqdUFSolids_, double MixEff_, int iRqdOFMethod_, flag BypassLiqOnly_, int iRqdBypassMethod_, double RqdOFSolids_);
    double Function(double x);
  };

CToleranceBlock CWasherOFFnd::s_Tol(TBF_Both, "Washer:Overflow", 0.0, 1.0e-8);

//--------------------------------------------------------------------------

CWasherOFFnd::CWasherOFFnd(SpConduit &Sd_, SpConduit &Ww_, SpConduit &Fd_, SpConduit &Qw_, SpConduit &Co_, SpConduit &Cu_, SpConduit* pVent_, MN_Surge* pSurge_,
      double POut_, double RqdUFSolids_, double MixEff_, int iRqdOFMethod_, flag BypassLiqOnly_, int iRqdBypassMethod_, double RqdOFSolids_) :
  MRootFinderBase("Thicken", s_Tol),//1.0e-8),
  Sd(Sd_), Ww(Ww_), Fd(Fd_), Qw(Qw_), Co(Co_), Cu(Cu_)
  {
  pVent = pVent_;
  pSurge = pSurge_;
  POut = POut_;
  RqdUFSolids = RqdUFSolids_;
  MixEff = MixEff_;
  iRqdOFMethod = iRqdOFMethod_;
  BypassLiqOnly = BypassLiqOnly_;
  iRqdBypassMethod = iRqdBypassMethod_;
  RqdOFSolids = RqdOFSolids_;

  bVentErr = false;
  IntOFSolidsUsed = 0.0;
  IntUFSolidsUsed = 0.0;
  dBypassQm = 0.0;
  }

//---------------------------------------------

double CWasherOFFnd::Function(double x)
  {
  if (iRqdBypassMethod==IBM_Underflow)
    {
    IntUFSolidsUsed = x;
    //x : this is the "internal" underflow solids frac required BEFORE adding any bypass back to the underflow,
    }
  else
    {
    IntOFSolidsUsed = x;
    //x : this is the "internal" overflow solids frac required BEFORE adding any bypass back to the overflow,
    //if the mixeff = 100% then the Bypass is 0% then the "internal_OFSolFrac" = "final_OFSolFrac".
    }

  dBypassQm = 0.0;
  const double MixEffUsed = (iRqdBypassMethod==IBM_None ? 1.0 : MixEff);
  Ww.QSetF(Qw, som_ALL, MixEffUsed, POut);
  if (MixEffUsed<1.0)
    {
    Ww.QAddF(Qw, som_Gas, 1.0 - MixEffUsed);
    if (BypassLiqOnly)
      Ww.QAddF(Qw, som_Sol, 1.0 - MixEffUsed);
    }
  //Sd.QCopy(Fd);
  Sd.QSetF(Fd, som_ALL, 1.0, POut);
  Sd.QAddF(Ww, som_ALL, 1.0);

  pSurge->RB.EvalProducts(Sd); //React mixture
  pSurge->EHX.EvalProducts(Sd);

  //put all vapours (if any) to vent (if present)...
  const double Qg = Sd.QMass(som_Gas);
  if (Qg>1e-12)
    {
    if (pVent)
      pVent->QSetM(Sd, som_Gas, Qg, POut);
    else
      bVentErr = true;
    }
  else if (pVent)
    pVent->QZero();

  //double HTot = Sd.totHf(); //what about vapours in Sd!!!???!!!
  //double FT = Sd.Temp();

  //after reaction...
  const double Qs = Sd.QMass(som_Sol);
  const double Ql = Sd.QMass(som_Liq);
  const double Us = iRqdBypassMethod==IBM_Underflow ? IntUFSolidsUsed : RqdUFSolids;
  const double Os = iRqdBypassMethod==IBM_Underflow ? RqdOFSolids : IntOFSolidsUsed;

  double Su;
  double So;
  if (Os<1.0e-9)
    {
    Su = Qs;
    So = 0.0;
    }
  else
    {
    Su = Range(0.0, Us*(Os * Ql - Qs * (1.0 - Os))/LTZ(Os - Us), Qs);
    So = Qs - Su;
    }
  const double Lu = Range(0.0, Su*(1.0 - Us)/GTZ(Us), Ql);
  const double Lo = Ql - Lu;

  Cu.QSetM(Sd, som_Sol, Su, POut);
  Cu.QAddM(Sd, som_Liq, Lu);

  Co.QSetM(Sd, som_Sol, So, POut);
	Co.QAddM(Sd, som_Liq, Lo);

  /*
  Cu.SanityCheck();
  Co.SanityCheck();
  if (1)
    {// Correct Enthalpy...
    Cu.SetTemp(FT);
    Co.SetTemp(FT);
    double P = POut;
    double H = Cu.totHf()+Co.totHf();
    double dT = 0.0, H0, T0;
    for (int Hiter=0; Hiter<10; Hiter++)
      {
      if (ConvergedVV(HTot, H, 1.0e-12, 1.0e-12))
        break;
      if (dT!=0.0)
        dT = (HTot-H)*(FT-T0)/NZ(H-H0);
      else
        dT = 0.1;
      T0 = FT;
      H0 = H;
      FT += dT;
      H = Cu.totHf(som_ALL, FT, P)+Co.totHf(som_ALL, FT, P);
      }
    Cu.SetTemp(FT);
    Co.SetTemp(FT);
    }*/

  if (MixEffUsed<1.0)
    {//add bypass back...
    double BeforeQm, AfterQm;
    if (iRqdBypassMethod==IBM_Overflow)
      {//bypass to overflow...
      BeforeQm = Co.QMass(som_ALL);
      if (!BypassLiqOnly)
        Co.QAddF(Qw, som_Sol, 1.0 - MixEffUsed);
	    Co.QAddF(Qw, som_Liq, 1.0 - MixEffUsed);
      AfterQm = Co.QMass(som_ALL);
      }
    else
      {//bypass to underflow...
      BeforeQm = Cu.QMass(som_ALL);
      if (!BypassLiqOnly)
        Cu.QAddF(Qw, som_Sol, 1.0 - MixEffUsed);
	    Cu.QAddF(Qw, som_Liq, 1.0 - MixEffUsed);
      AfterQm = Cu.QMass(som_ALL);
      }
    dBypassQm = AfterQm-BeforeQm;
    }

  if (iRqdBypassMethod==IBM_Underflow)
    {
    double UFSolFrac = Cu.MassFrac(som_Sol);
    return UFSolFrac;
    }
  else
    {
    if (iRqdOFMethod == OFM_OFSolFrac)
      {
      double OFSolFrac = Co.MassFrac(som_Sol);
      return OFSolFrac;
      }
    else// if (iRqdOFMethod == OFM_OFSolConc)
      {
      double Conc25 = Co.PhaseConc(C_2_K(25.0), som_Sol, som_ALL);
      return Conc25;
      }
    }
  }

//--------------------------------------------------------------------------

void Washer::EvalJoinPressures(long JoinMask)
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
      MN_Surge::EvalJoinPressures(JoinMask);
      break;
    }
  }

//--------------------------------------------------------------------------
/*#F:
This determines what material should leave through each outlet,
and rate of change of the contents resulting from the flow, of material, out of each outlet
of the surge unit.
*/
void Washer::EvalProducts(CNodeEvalIndex & NEI)
  {
  StkSpConduit Sd("Sd", chLINEID(), this);
  StkSpConduit Fd("Fd", chLINEID(), this);
  StkSpConduit Ww("Ww", chLINEID(), this);

  MixEff = Range(0.0, MixEff, 1.0);
  RqdOFSolConc = Range(0.0, RqdOFSolConc, 5000.0);
  RqdOFSolids = Range(0.0, RqdOFSolids, 0.99);
  RqdUFSolids = Range(1.0e-8, RqdUFSolids, 1.0);
  if (iRqdBypassMethod==IBM_Underflow && iRqdOFMethod == OFM_OFSolConc)
    {
    iRqdOFMethod = OFM_OFSolFrac;
    LogWarning(FullObjTag(), 0, "Because BypassOption is Underflow, Method has been changed to OverFlowSolidsFraction");
    }

  switch (SolveMethod())
    {
    case SM_Direct:
    case SM_Inline:
      {
      const int iCo = IOWithId_Self(ioidOF);
      const int iCu = IOWithId_Self(ioidUF);
      const int iVent = IOWithId_Self(ioidVent);
      SpConduit & Co = *IOConduit(iCo);
      SpConduit & Cu = *IOConduit(iCu);
      SpConduit * pVent = (iVent>=0 ? IOConduit(iVent) : NULL);

      SigmaQInPMin(Fd(), som_ALL, Id_2_Mask(ioidFd));
      SigmaQInPMin(Qw, som_ALL, Id_2_Mask(ioidIt));
      double CFeed = Fd().SpecieConc(Fd().Temp(), iScanEffSpecie, som_Liq);
      flag HasFlw = (Fd().QMass(som_ALL)+Qw.QMass(som_ALL)>UsableMass);

      //double POut = AtmosPress(); //force outlet to Atmos P
      double POut = Std_P; //force outlet to Std_P
      CWasherOFFnd Fnd(Sd(), Ww(), Fd(), Qw, Co, Cu, pVent, this, POut, RqdUFSolids, MixEff, iRqdOFMethod, bMixEffLiqOnly, iRqdBypassMethod, RqdOFSolids);
      if (iRqdBypassMethod==IBM_Underflow)
        {
        IntUFSolidsUsed = Valid(IntUFSolidsUsed) ? IntUFSolidsUsed : RqdUFSolids;
        Fnd.SetEstimate(IntUFSolidsUsed, 1.0);
        Fnd.SetTarget(RqdUFSolids);
        }
      else
        {
        IntOFSolidsUsed = Valid(IntOFSolidsUsed) ? IntOFSolidsUsed : RqdOFSolids;
        Fnd.SetEstimate(IntOFSolidsUsed, 1.0);
        if (iRqdOFMethod == OFM_OFSolFrac)
          Fnd.SetTarget(RqdOFSolids);
        else //if (iRqdOFMethod == OFM_OFSolConc)
          Fnd.SetTarget(RqdOFSolConc);
        }
      flag Ok = false;
      int iRet = Fnd.Start(0.0, 0.999);
      if (iRet==RF_EstimateOK) //estimate is good, solve not required
        {
        double dd = Fnd.Result();
        Ok = true;
        }
      else
        {
        if (iRet==RF_BadEstimate)
          iRet = Fnd.Start(0.0, 0.999); // Restart
        if (iRet==RF_OK)
          {
          if (Fnd.Solve_Brent()==RF_OK)
            {
            double dd = Fnd.Result();
            //IntOFSolidsUsed = Fnd.IntOFSolidsUsed;
            Ok = true;
            }
          }
        }

      if (!Ok)
        {
        //SetCI(???);
        }
      //use result anyway...
      BypassQm = Fnd.dBypassQm;
      if (iRqdBypassMethod==IBM_Underflow)
        {
        IntUFSolidsUsed = Fnd.IntUFSolidsUsed;
        IntOFSolidsUsed = dNAN;
        }
      else
        {
        IntOFSolidsUsed = Fnd.IntOFSolidsUsed;
        IntUFSolidsUsed = dNAN;
        }

      // Calculate Required Area
      if (OFSolidsFn.Length()>=2 && UFSolidsFn.Length()>=2)
        {
        }

      //results...
      Cu.SanityCheck();
      Co.SanityCheck();
      UFSolids = Cu.MassFrac(som_Sol);
      OFSolids = Co.MassFrac(som_Sol);
      ActUFSolidsConc25 = Cu.PhaseConc(C_2_K(25.0), som_Sol, som_ALL);
      ActOFSolidsConc25 = Co.PhaseConc(C_2_K(25.0), som_Sol, som_ALL);
      double Cuf = Cu.SpecieConc(Cu.Temp(), iScanEffSpecie, som_Liq);
      double Cof = Co.SpecieConc(Co.Temp(), iScanEffSpecie, som_Liq);
      ActScandrettEff = (CFeed - Cuf)/NZ(CFeed - Cof);

			double UFErr = fabs(UFSolids - RqdUFSolids);
		  SetCI(1, bTrackStatus && HasFlw && UFErr > 1.0e-6);
      if (iRqdOFMethod == OFM_OFSolFrac)
        {
        double OFErr = fabs(OFSolids - RqdOFSolids);
		    SetCI(2, bTrackStatus && HasFlw && OFErr > 1.0e-6);
        ClrCI(3);
        }
      else// if (iRqdOFMethod == OFM_OFSolConc)
        {
        ClrCI(2);
        double OFErr = fabs(ActOFSolidsConc25-RqdOFSolConc);
        SetCI(3, bTrackStatus && HasFlw && (OFErr > 1.0e-5));
        }
      SetCI(4, HasFlw && Fnd.bVentErr);

      break;
      }
    case SM_Buffered:
      {
      Contents.ZeroDeriv();
      RB.EvalProducts(Sd());
      EHX.EvalProducts(Sd());

      double SolMass = Contents.Mass(som_Sol);
      double LiqMass = Contents.Mass(som_Liq);
      double Sol_Rho = Contents.Rho(som_Sol) ;
      double Liq_Rho = Contents.Rho(som_Liq) ;
      double Area    = Contents.ShapeVolume() / Contents.ShapeHeight();

      if (1 || (SolMass>1.0e-12 && LiqMass>1.0e-12))
        {
        SolMass=GTZ(SolMass);
        LiqMass=GTZ(LiqMass);
        BedHgt = (SolMass / (Area * Sol_Rho)) * (1 + Sol_Rho / Liq_Rho * (SolMass / LiqMass));

        double ReqdOFSolids = Range(0.0, OFSolidsFn.Yx(BedHgt), 1.0);
        double ReqdOFSl     = ReqdOFSolids / GTZ(1.0 - ReqdOFSolids);

        double ReqdUFSolids = Range(0.0, UFSolidsFn.Yx(BedHgt), 1.0);
        double ReqdUFSl     = ReqdUFSolids / GTZ(1.0 - ReqdUFSolids);

        SetProdMakeup(PMU_IOId | PMU_SLRatio, ioidOF, Contents, ReqdOFSl);
        SetProdMakeup(PMU_IOId | PMU_SLRatio, ioidUF, Contents, ReqdUFSl);
        }
      else
        {
        BedHgt = 0.0;
        // Just Operate as a Tank
        }

      SigmaQInPMin(QFeed(), som_ALL, First64IOIds);
      EvalProducts_SurgeLevel(ContentHgtOrd, &QFeed());

      if (NoProcessJoins()>=1)
        Xfer_EvalProducts(0, Joins[0].Pressure(), NULL, &QProd, NULL, NULL, NULL);

      UFSolids = IOConduit(IOWithId_Self(ioidUF))->MassFrac(som_Sol);
      OFSolids = IOConduit(IOWithId_Self(ioidOF))->MassFrac(som_Sol);
      ActUFSolidsConc25 = IOConduit(IOWithId_Self(ioidUF))->PhaseConc(C_2_K(25.0), som_Sol, som_ALL);
      ActOFSolidsConc25 = IOConduit(IOWithId_Self(ioidOF))->PhaseConc(C_2_K(25.0), som_Sol, som_ALL);

      break;
      }
    }
  }

//--------------------------------------------------------------------------

flag Washer::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case  1: pS="W\tU/F Solids Requirements not met.";    return 1;
    case  2: pS="W\tO/F Solids Requirements not met.";    return 1;
    case  3: pS="W\tO/F Solids Concentration Requirements not met."; return 1;
    case  4: pS="W\tNo Vent Defined.";                    return 1;
    default:
      return MN_Surge::CIStrng(No, pS);
    }
  }

// ===========================================================================*/



