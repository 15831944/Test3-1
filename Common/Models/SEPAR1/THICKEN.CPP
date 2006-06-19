//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#define  __THICKEN_CPP
#include "thicken.h"
//==========================================================================
/*#D:#T:Thickener
#X:#h<General Description>#N#NThe thickener concentrates the solids in 
the feed to the underflow stream.  In Dynamic mode the thickener is based on 
a set of user defined curves relating the percentage solids in the unit to the 
height of the solids bed in the thickener. The bed height is calculated using 
the mass of solids in the thickener and the area of the thickener.  
The curves can be obtained from plant data, if available, or from experimental data.
If no data is available, the default curves can be used.
#nIn Mass balancing mode the user defines the percentage solids in the under 
and over flows from the unit.

#n#n#h<Variables to be supplied by the user>#n
#n#i<% RqdUFSolids:>The required % solids in the underflow from the thickener. This
variable is only required in mass balancing mode.
#n#i<% RqdOFSolids:>The required % solids in the overflow from the thickener. This
variable is also only required in mass balancing mode.
#n#i<Datum required:>The relative elevation of the thickener with respect to the 
common datum of the system.

#n#n#h<Variables calculated by the model>#n
#n#i<Datum:>The height of the thickener above the system datum.
#n#i<UFSolids:>The actual % solids in the underflow.
#n#i<OFSolids:>The actual % solids in the overflow.

#n#h<Other>#n
Default model prefix:THK#n
Short name:Thicken#n
Model type:Unit#n
#G:Units
*/

//==========================================================================

const byte ioidFd = 0;
const byte ioidOF = 1;
const byte ioidUF = 2;
const byte ioidVent = 3;

static IOAreaRec ThickenerIOAreaList[] =
  {
    {"Thickener Feed",      "Feed",       ioidFd,           LIO_In0,  nc_MLnk, 1, 20, IOPipeEntry, 1.0f},
    {"Thickener Overflow",  "OverFlow",   ioidOF,           LIO_Out0, nc_MLnk, 1, 1,  IOPipeEntry, (float)0.98},
    {"Thickener Underflow", "UnderFlow",  ioidUF,           LIO_Out1, nc_MLnk, 1, 1,  IOPipeEntry, 0.0f},
    {"",                    "Vent",       ioidVent,         LIO_Out,  nc_MLnk, 0, 1,  IOPipeEntry, 1.0f},
    SPILL2AREA("Spills", IOId_Spill2Area),
    VENT2AREA("Vents",   IOId_Vent2Area),
    {NULL}
  };

static double Drw_Thicken[] = { DD_Poly, -25,0, -25,-3, -22,-3, -22,0, 22,0, 22,-3, 25,-3, 
											 25,0, 25,-3, 22,-3, 22,-4, 0,-13, -22,-4, -22,-3, 
											 -22,-4, 22,-4, DD_End };

enum OFM_OverFlowMethods { OFM_OFSolFrac, OFM_OFSolConc };

//--------------------------------------------------------------------------

IMPLEMENT_MODELUNIT(Thickener, "Thicken", "1", Drw_Thicken, "WasherDecanter", "THK", TOC_ALL|TOC_GRP_SEPAR|TOC_STD_KENWALT,
                    "Separation:Thickener", 
                    "Thickener based on graphs which "
                    "determine underflow and overflow "
                    "densities from the thickener bed height") 

Thickener::Thickener(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MN_Surge(pClass_, TagIn, pAttach, eAttach),
  UFSolidsFn("DataOnly", "UF_Solids", this, TOA_Embedded),
  OFSolidsFn("DataOnly", "OF_Solids", this, TOA_Embedded)
  {
  AttachIOAreas(ThickenerIOAreaList, &PipeEntryGroup);
  Contents.SetClosed(False);
  Contents.SetPreset(&m_Preset, &m_PresetImg);

  SetAllowedSolveModes(SM_Direct|/*SM_Inline|*/SM_Buffered);//"Transfer", "Inline", "Buffered");

  iRqdOFMethod  = OFM_OFSolFrac;
  IntOFSolidsUsed = dNAN;
  RqdUFSolids   = 0.4;
  RqdOFSolids   = 0.00001;
  RqdOFSolConc  = 0.1;
  UFSolids      = RqdUFSolids;
  OFSolids      = RqdOFSolids;
  BedHgt        = 0.0;
  ActUFSolidsConc25 = 0.0;
  ActOFSolidsConc25 = 0.0;
  ActUFSolRec   = 0.0;
  bTrackStatus  = true;
  }

//--------------------------------------------------------------------------

Thickener::~Thickener()
  {
  }

//--------------------------------------------------------------------------
/*#F:This provides access to the variables of the model and calls the
BuildDataDefn for objects used by the model.*/
void Thickener::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);

  DDB.Visibility(SM_DynBoth|HM_All);
  BuildDataDefnElevation(DDB);
  DDB.Text    ("");
  DDB.Double  ("Bed_Height",              "BedHgt",          DC_L,    "m",      &BedHgt,             this, isResult|0);
  DDB.Double  ("Pressure",                "P",               DC_P,    "kPag",   xidPMean,            this, isResult|0);
  DDB.Double  ("Density",                 "Rho",             DC_Rho,  "kg/m^3", xidRho,              this, isResult|0);
  DDB.Double  ("Level",                   "Lvl",             DC_Frac, "%",      xidLevel,            this, isResult|0);
  DDB.Double  ("Mass_Accum",              "QmAcc",           DC_Qm,   "kg/s",   &m_QmAcc,            this, isResult|0);
  DDB.Double  ("Vol_Accum",               "QvAcc",           DC_Qv,   "L/s",    &m_QvAcc,            this, isResult|0);

  static DDBValueLst DDB0[]={
    {OFM_OFSolFrac, "OverFlowSolidsFraction"},
    {OFM_OFSolConc, "OverFlowSolidsConcentration"},
    {0}};
  DDB.Visibility(SM_Direct|HM_All);
	DDB.Text    ("");
  DDB.Text    ("Requirements");
  DDB.Double  ("Rqd_UnderFlow_Solids",    "RqdUFSolids",     DC_Frac, "%",      &RqdUFSolids,        this, isParm);
  DDB.Byte    ("OverFlow_Method",         "Method",          DC_,     "",       &iRqdOFMethod,       this, isParm|AffectsStruct, DDB0);
  DDB.Visibility(SM_Direct|HM_All, (iRqdOFMethod == OFM_OFSolFrac));
  DDB.Double  ("Rqd_OverFlow_Solids",     "RqdOFSolids",     DC_Frac, "%",      &RqdOFSolids,        this, isParm);
  DDB.Visibility(SM_Direct|HM_All, (iRqdOFMethod == OFM_OFSolConc));
  DDB.Double  ("Rqd_OverFlowSolidsConc25","RqdOFSolConc25",  DC_Conc, "g/L",    &RqdOFSolConc,       this, isParm);
  DDB.Visibility(SM_Direct|HM_All);
  DDB.Double  ("Internal_OF_SolidsUsed",  "IntOFSolidsUsed", DC_Frac, "%",      &IntOFSolidsUsed,    this, isParmConstruct|isResult|InitHidden);//|noView);
  DDB.Visibility();
  DDB.Text    ("Results");
	DDB.Double  ("UnderFlow_Solids",        "UFSolids",        DC_Frac, "%",      &UFSolids,           this, isParmConstruct|isResult);
  DDB.Double  ("OverFlow_Solids",         "OFSolids",        DC_Frac, "%",      &OFSolids,           this, isParmConstruct|isResult);
  DDB.Double  ("UnderFlow_SolidsConc@25", "UFSolConc25",     DC_Conc, "g/L",    &ActUFSolidsConc25,  this, isParmConstruct|isResult|InitHidden);
  DDB.Double  ("OverFlow_SolidsConc@25",  "OFSolConc25",     DC_Conc, "g/L",    &ActOFSolidsConc25,  this, isParmConstruct|isResult|InitHidden);
  DDB.Double  ("UnderFlow_SolidsRecovery","UFSolRec",        DC_Frac, "%",      &ActUFSolRec,        this, isParmConstruct|isResult);

	DDB.Text    ("");
  DDB.Visibility(SM_Direct|HM_All);
  DDB.CheckBox("ShowQFeed",               "",                DC_,     "",       &bShowQFeed,         this, isParm|SetOnChange);
  DDB.Visibility();
  RB.Add_OnOff(DDB);
  EHX.Add_OnOff(DDB);
  DDB.Visibility(SM_Direct|HM_All);
  DDB.CheckBox("",                        "TrackStatus",     DC_,     "",       &bTrackStatus,       this, isParm);
  DDB.Visibility();

  DDB.Text    ("");
	BuildDataDefnShowIOs(DDB);
  BuildDataDefnIOOpts(DDB);

  RB.BuildDataDefn(DDB);
  EHX.BuildDataDefn(DDB);

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

  DDB.Visibility(SM_DynBoth|HM_All);
	DDB.Object(&UFSolidsFn, this, NULL, NULL, DDB_RqdPage);
  DDB.Object(&OFSolidsFn, this, NULL, NULL, DDB_RqdPage);
  DDB.Visibility();

  DDB.EndStruct();
  }

//--------------------------------------------------------------------------

void Thickener::EvalJoinPressures(long JoinMask)
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
      MN_Surge::EvalJoinPressures(JoinMask);
      break;
    }
  }

//--------------------------------------------------------------------------
// x is required "internal" overflow solids fraction
//
class CThickenerOFFnd : public MRootFinderBase
  {
  public:
    SpConduit &Sd, &Fd, &Co, &Cu;
    double IntOFSolidsUsed;
    flag bVentErr;
  protected:
    SpConduit* pVent;
    MN_Surge* pSurge;

    double POut;
    double RqdUFSolids;
    double iRqdOFMethod;
    static CToleranceBlock s_Tol;
  public:
    CThickenerOFFnd(SpConduit &Sd_, SpConduit &Fd_, SpConduit &Co_, SpConduit &Cu_, SpConduit* pVent_, MN_Surge* pSurge_,
      double POut_, double RqdUFSolids_, double iRqdOFMethod_);
    double Function(double x);
  };

CToleranceBlock CThickenerOFFnd::s_Tol(TBF_Both, "Thickener:Overflow", 0.0, 1.0e-8);

//--------------------------------------------------------------------------
    
CThickenerOFFnd::CThickenerOFFnd(SpConduit &Sd_, SpConduit &Fd_, SpConduit &Co_, SpConduit &Cu_, SpConduit* pVent_, MN_Surge* pSurge_, 
      double POut_, double RqdUFSolids_, double iRqdOFMethod_) :
  MRootFinderBase("Thicken", s_Tol),//1.0e-8), 
  Sd(Sd_), Fd(Fd_), Co(Co_), Cu(Cu_)
  {
  pVent = pVent_;
  pSurge = pSurge_;
  POut = POut_;
  RqdUFSolids = RqdUFSolids_;
  iRqdOFMethod = iRqdOFMethod_;

  bVentErr = false;
  IntOFSolidsUsed = 0.0;
  }

//---------------------------------------------

double CThickenerOFFnd::Function(double x) 
  {
  IntOFSolidsUsed = x;
  
  Sd.QCopy(Fd);
  
  pSurge->RB.EvalProducts(Sd);
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

  // *** NBNBNB A Big Kludge
  SpModel *pM=Sd.Model();
  for (int q=0; q<pM->nQuals; q++)
    if (pM->Quals[q].m_pQ)
      pM->Quals[q].m_pQ->Equilibrate();
  // *** Replace with Sd().EquilibrateQualities();
  
  //double HTot = Sd.totHf(); //what about vapours in Sd!!!???!!!
  //double FT = Sd.Temp();

  //after reaction...
  const double Qs = Sd.QMass(som_Sol);
  const double Ql = Sd.QMass(som_Liq);
  const double Us = RqdUFSolids;
  const double Os = IntOFSolidsUsed;

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

  //if (pSurge->DoDbgBrk())
  //  {
  //  dbgp("%18.7e %18.7e %18.7e %18.7e", Qs, Ql, Us, Os);
  //  dbgpln(" %18.7e %18.7e %18.7e %18.7e", So, Lo, Su, Lu);
  //  }

//Sd().DumpQualities();
  Cu.QSetM(Sd, som_Sol, Su, POut);
  Cu.QAddM(Sd, som_Liq, Lu);
//Cu.DumpQualities();    
  Co.QSetM(Sd, som_Sol, So, POut);
  Co.QAddM(Sd, som_Liq, Lo);
//Co.DumpQualities();

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
      H = Cu.totHf(som_ALL, FT, P)+Co.totHf(som_ALL, FT, P);
      }
    Cu.SetTemp(FT);
    Co.SetTemp(FT);
    }*/

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

//--------------------------------------------------------------------------
/*#F:
This determines what material should leave through each outlet,
and rate of change of the contents resulting from the flow, of material, out of each outlet
of the surge unit.
*/
void Thickener::EvalProducts(long JoinMask)
  {
  StkSpConduit Sd("Sd", chLINEID(), this);
  StkSpConduit Fd("Fd", chLINEID(), this);

  RqdOFSolConc = Range(0.0, RqdOFSolConc, 5000.0);
  RqdOFSolids = Range(0.0, RqdOFSolids, 0.99);
  RqdUFSolids = Range(1.0e-8, RqdUFSolids, 1.0);

  switch (SolveMethod())
    {
    case SM_Direct:
      {
      const int iCo = IOWithId_Self(ioidOF);
      const int iCu = IOWithId_Self(ioidUF);
      const int iVent = IOWithId_Self(ioidVent);
      SpConduit & Co = *IOConduit(iCo);
      SpConduit & Cu = *IOConduit(iCu);
      SpConduit * pVent = (iVent>=0 ? IOConduit(iVent) : NULL);

      SigmaQInPMin(Fd(), som_ALL, Id_2_Mask(ioidFd));
      flag HasFlw = (Fd().QMass(som_ALL)>UsableMass);

      if (bShowQFeed && !QFeed.Exists())
        QFeed();//forces create
      if (QFeed.Exists())
        QFeed().QCopy(Fd());

      //double POut = AtmosPress(); //force outlet to Atmos P
      double POut = Std_P; //force outlet to Std_P
      CThickenerOFFnd Fnd(Sd(), Fd(), Co, Cu, pVent, this, POut, RqdUFSolids, iRqdOFMethod);
      IntOFSolidsUsed = Valid(IntOFSolidsUsed) ? IntOFSolidsUsed : RqdOFSolids;
      Fnd.SetEstimate(IntOFSolidsUsed, 1.0);
      if (iRqdOFMethod == OFM_OFSolFrac)
        Fnd.SetTarget(RqdOFSolids);
      else //if (iRqdOFMethod == OFM_OFSolConc)
        Fnd.SetTarget(RqdOFSolConc);
      flag Ok = false;
      //if (DoDbgBrk())
      //  dbgpln("-----------Start");
      int iRet = Fnd.Start(0.0, 0.999);
      if (iRet==RF_EstimateOK) //estimate is good, solve not required
        {
        double dd = Fnd.Result();
        Ok = true;
        }
      else
        {
        if (iRet==RF_BadEstimate)
          {
          //if (DoDbgBrk())
          //  dbgpln("-----------Start Again");
          iRet = Fnd.Start(0.0, 0.999); // Restart
          }
        if (iRet==RF_OK)
          {
          //if (DoDbgBrk())
          //  dbgpln("-----------SolveBrent");
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
      IntOFSolidsUsed = Fnd.IntOFSolidsUsed; 

      // Calculate Required Area
      if (OFSolidsFn.Length()>=2 && UFSolidsFn.Length()>=2)
        {
        }

      //if (DoDbgBrk())
      //  dbgpln("-----------Done");

      //results...
      Cu.SanityCheck();
      Co.SanityCheck();
      UFSolids = Cu.MassFrac(som_Sol);
      OFSolids = Co.MassFrac(som_Sol);
      ActUFSolidsConc25 = Cu.PhaseConc(C_2_K(25.0), som_Sol, som_ALL);
      ActOFSolidsConc25 = Co.PhaseConc(C_2_K(25.0), som_Sol, som_ALL);
      const double UFSolidsQm = Cu.QMass(som_Sol);
      const double OFSolidsQm = Co.QMass(som_Sol);
      ActUFSolRec = UFSolidsQm/GTZ(UFSolidsQm+OFSolidsQm);

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
    case SM_Inline:
    case SM_Buffered:
      {
      //Contents.ZeroDeriv();
      RB.EvalProducts(Sd());
      EHX.EvalProducts(Sd());

      double SolMass = Contents.Mass(som_Sol);
      double LiqMass = Contents.Mass(som_Liq);
      double Sol_Rho = Contents.Rho(som_Sol);
      double Liq_Rho = Contents.Rho(som_Liq);
      double Area    = Contents.ShapeVolume() / Contents.ShapeHeight();

      if (1 || (SolMass>1.0e-12 && LiqMass>1.0e-12))
        {
        SolMass=GTZ(SolMass);
        LiqMass=GTZ(LiqMass);
        BedHgt = (SolMass / (Area * Sol_Rho)) * (1 + Sol_Rho / Liq_Rho * (SolMass / LiqMass));

        int OFnLen=OFSolidsFn.GetLen();
        double ReqdOFSolids = Range(0.0, OFnLen>=2 ? OFSolidsFn.Yx(BedHgt) : BedHgt * 0.005, 1.0);
        double ReqdOFSl     = ReqdOFSolids / GTZ(1.0 - ReqdOFSolids);

        int UFnLen=UFSolidsFn.GetLen();
        double ReqdUFSolids = Range(0.0, UFnLen>=2 ? UFSolidsFn.Yx(BedHgt) : BedHgt * 0.5, 1.0);
        double ReqdUFSl     = ReqdUFSolids / GTZ(1.0 - ReqdUFSolids);

        SetProdMakeup(PMU_IOId | PMU_SLRatio, ioidOF, Contents, ReqdOFSl);
        SetProdMakeup(PMU_IOId | PMU_SLRatio, ioidUF, Contents, ReqdUFSl);
        }
      else
        {
        BedHgt = 0.0;
        // Just Operate as a Tank
        }

      //put all vapours (if any) to vent (if present)
      const int iVent = IOWithId_Self(ioidVent);
      if (iVent>=0)
        {
        double Qg = Sd().QMass(som_Gas);
        SpConduit & Cvent = *IOConduit(iVent);
        Cvent.QSetM(Sd(), som_Gas, Qg, Std_P);
        }

      SigmaQInPMin(QFeed(), som_ALL, First64IOIds);
      EvalProducts_SurgeLevel(ContentHgtOrd, &QFeed());

      //if (NJoins()>=1)
      //  Xfer_EvalProducts(0, Joins[0].Pressure(), &QSigmaOut, NULL, NULL, NULL);
      
      UFSolids = IOConduit(IOWithId_Self(ioidUF))->MassFrac(som_Sol);
      OFSolids = IOConduit(IOWithId_Self(ioidOF))->MassFrac(som_Sol);
      ActUFSolidsConc25 = IOConduit(IOWithId_Self(ioidUF))->PhaseConc(C_2_K(25.0), som_Sol, som_ALL);
      ActOFSolidsConc25 = IOConduit(IOWithId_Self(ioidOF))->PhaseConc(C_2_K(25.0), som_Sol, som_ALL);

      break;
      }
    }
  }

//--------------------------------------------------------------------------

flag Thickener::CIStrng(int No, pchar & pS)
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

//===========================================================================


