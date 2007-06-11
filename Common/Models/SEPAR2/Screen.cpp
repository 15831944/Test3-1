//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#define  __SCREEN_CPP
#include "Screen.h"
//#include "optoff.h"

#define dbgScreen    0

//==========================================================================

enum SBMethods { SBM_Karra, SBM_PartCrv_Individ, SBM_PartCrv_Overall, SBM_Whiten, SBM_WhitenBetaStar };
enum KMethods  { KM_Area, KM_d50 };
enum FMethods  { FM_None, FM_LiqFines, FM_FineEff };

XID xidMethod       = MdlBsXID(10050);
XID xidWhitenBeta   = MdlBsXID(10051);
XID xidWhitenRf     = MdlBsXID(10052);
XID xidWhitenC      = MdlBsXID(10053);

IMPLEMENT_TAGOBJ(ScreenBlk, "ScrnBlk", "ScrnBlk", "1", "", "SB", TOC_ALL|TOC_GRP_SIZEDST|TOC_SIZEDIST, "Screen Blk", "Screen Block");

ScreenBlk::ScreenBlk(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  TaggedObject(pClass_, TagIn, pAttach, eAttach),
  QTemp("QTemp", this, TOA_Embedded),
  QFine("QFine", this, TOA_Embedded),
  PartCrv("PartCrv", this, TOA_Embedded),
  PartCrv2("PrtCurve", this, TOA_Embedded)
  {
  fMode     = True;
  bKneeCorr = 0;
  iMethod   = SBM_Karra;
  iKM       = KM_d50;
  iFineMeth = FM_None;
  Cap      = 1000.0;
  TotCap   = 1000.0;
  MinFine  = 0.0;
  EWash    = 1.0;
  FdSol    = 0.0;
  OSSol    = 0.0;
  USSol    = 0.0;
  OSDens   = 0.0;
  USDens   = 0.0;
  SArea    = 10.0;
  Area     = 10.0;
  ht       = 5.0;
  hmm      = 5.0;
  m_d50Op      = 1.0;
  m_Alpha  = 4.0;
  m_AlphaOp = 4.0;
  m_AlphaKnee = dNAN;
  m_AlphaAtCap = 4.0;
  Beta     = 0.0;
  BetaStar = 1.0;
  m_Rf     = 0.0;
  m_RfOp   = 0.0;
  m_RfKnee = dNAN;
  m_RfAtCap = 0.0;
  MaxSize  = 1.0;
  ht_chk   = 1.0e100;
  m_OSMoist  = 0.01;
  m_OSMoistOp = 0;
  m_OSMoistKnee = dNAN;
  m_OSMoistAtCap = 0.1;
  OSSplit  = 0.1;
  QmSIn    = 0.0;
  QmLIn    = 0.0;
  A        = 1.0;
  B        = 1.0;
  C        = 1.0;
  D        = 1.0;
  E        = 1.0;
  F        = 1.0;
  G        = 1.0;
  m_CutSize     = 1.0e-3;
  m_CutSizeKnee =dNAN;
  m_CutSizeAtCap =1e-3;

  m_EfficiencyAtSize = 0.0;
  m_EfficiencySize   = m_CutSize;

  DeckFeed = 0.0;
  FinetoCoarse = 0.0;
  bTrackStatus = True;
  OTemp.SetSize(0, 32);
  Feed.SetSize(0, 32);
  UTemp.SetSize(0, 32);
  YTemp.SetSize(0, 32);
  Fine.SetSize(0, 32);
  ASSERT_VALID(&OTemp);
  ASSERT_VALID(&Feed);
  ASSERT_VALID(&UTemp);
  ASSERT_VALID(&YTemp);
  ASSERT_VALID(&Fine);

  PartCrv.SetNCurves(1);
  PartCrv.SetEditable(0, true);
  PartCrv.SetLength(3);
  PartCrv.SetModeVisibility(true, "Passing", "Retained");
  PartCrv.SetStateVisibility(false, "", "");

  PartCrv2.SetNCurves(Max(1, SD_Defn.NPriIds(0)));
  for (int c=0; c<PartCrv2.NCurves(); c++)
    PartCrv2.SetEditable(c, false);
  PartCrv2.SetLength(3);
  PartCrv2.SetModeVisibility(true, "Passing", "Retained");
  PartCrv2.SetStateVisibility(false, "", "");
  PartCrv2.SetCumulativeVisibility(false);
  };

//--------------------------------------------------------------------------

void ScreenBlk::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);

  //DDB.Visibility(NM_Probal|SM_All|HM_All|SSMODE);
  DDB.Text("");
  static DDBValueLst DDB2[]={
    {(int)True,  "Screen"},
    {(int)False, "Split" },
    {0}};
  DDB.Bool    ("Mode",            "",            DC_,     "",       &fMode,        this, isParm|SetOnChange, DDB2);
  DDB.Text(" ");

  DDB.Visibility(NSHM_All, !fMode);
  DDB.Double  ("",                "OSSolids",    DC_Frac, "%",      &OSSplit,       this, isParm);
  DDB.Visibility();
  DDB.Double  ("OSMoisture",      "OSMoist",     DC_Frac, "%",      &m_OSMoist,     this, isParm);
  DDB.Visibility(NSHM_All, fMode);
  DDB.CheckBox("RateCorrection",  "",            DC_,     "",       &bKneeCorr,     this, isParm|SetOnChange);
  DDB.Visibility(NSHM_All, bKneeCorr && fMode);
  DDB.Double  ("",                "OSMoist.Op",  DC_Frac, "%",      &m_OSMoistOp,   this, isResult);
  DDB.Double  ("",                "OSMoist.Knee",DC_Qm,   "kg/s",   &m_OSMoistKnee, this, isParm|NAN_OK);
  DDB.Double  ("",                "OSMoist.@Cap",DC_Frac, "%",      &m_OSMoistAtCap,this, isParm);
  DDB.Visibility();
  DDB.Double  ("",                "Capacity",    DC_Qm,   "kg/s",   &Cap,           this, isParm);
  DDB.Double  ("TotalCapacity",   "TotalCap",    DC_Qm,   "kg/s",   &TotCap,        this, isResult|0);
  DDB.Double  ("",                "ScreenFeed",  DC_Qm,   "kg/s",   &DeckFeed,      this, isResult|0);
  DDB.CheckBox("TrackStatus",      "",           DC_,     "",       &bTrackStatus,  this, isParm);

  DDB.Visibility(NSHM_All, fMode);
  DDB.Text(" ");
  static DDBValueLst DDB1[]={
    {SBM_Karra,     "Karra"},
    {SBM_PartCrv_Overall, "PartitionCrv"},
#if WithIndividPartCrv
    {SBM_PartCrv_Individ, "Individual_PartCrv" },
#endif
    //{SBM_PartCrv_Overall, "Overall_PartCrv"},
    {SBM_Whiten, "Whiten"},
    {SBM_WhitenBetaStar, "WhitenBeta"},
    {0}};
  DDB.Byte    ("SizingMethod",    "Method",      DC_,     "",       xidMethod,      this, isParm|SetOnChange, DDB1);

  DDB.Visibility(NSHM_All, iMethod==SBM_Karra && fMode);
  DDB.Text(" ");
  static DDBValueLst DDB4[]={
    {KM_Area,     "ScreenArea"},
    {KM_d50,      "Define_d50"},
    {0}};
  DDB.Byte    ("d50_Method",      "",            DC_,     "",       &iKM,           this, isParm|SetOnChange, DDB4);

  DDB.Visibility(NSHM_All, iMethod==SBM_Karra && iKM==KM_Area && fMode);
  DDB.Double  ("DDScreenArea",    "SArea",       DC_Area, "m^2",    &SArea,         this, isParm);
  DDB.Double  ("TotalScreenArea", "TotalArea",   DC_Area, "m^2",    &Area,          this, isResult|0);
  DDB.Double  ("ThruFall_Aper",   "CutAper",     DC_L,    "um",     &hmm,           this, isParm);
  DDB.Bool    ("Wet_Dry",         "Wet?",        DC_,     "",       &wet,           this, isParm, DDBYesNo);
  //DDB.Double  ("Eff_Wash",        "WashEff",     DC_Frac, "%",      &EWash,         this, isParm);

  DDB.Visibility(NSHM_All, ((iMethod==SBM_Karra && iKM==KM_d50) || (iMethod==SBM_Whiten) || (iMethod==SBM_WhitenBetaStar)) && fMode);
  DDB.Double  ("Cut_Size",     "d50",            DC_L,    "mm",     &m_CutSize,       this, isParm);
  DDB.Visibility(NSHM_All, (iMethod==SBM_Whiten || iMethod==SBM_WhitenBetaStar) && bKneeCorr && fMode);
  DDB.Double  ("",             "d50.Knee",       DC_Qm,   "kg/s",   &m_CutSizeKnee,   this, isParm|NAN_OK);
  DDB.Double  ("",             "d50.@Cap",       DC_L,    "mm",     &m_CutSizeAtCap,  this, isParm);
  DDB.Visibility(NSHM_All, (iMethod==SBM_Whiten || iMethod==SBM_WhitenBetaStar) && fMode);
  DDB.Double  ("",             "Alpha",          DC_,     "",       &m_Alpha,         this, isParm);
  DDB.Visibility(NSHM_All, (iMethod==SBM_Whiten || iMethod==SBM_WhitenBetaStar) && bKneeCorr && fMode);
  DDB.Double  ("",             "Alpha.Op",       DC_,     "",       &m_AlphaOp,   this, isResult);
  DDB.Double  ("",             "Alpha.Knee",     DC_Qm,   "kg/s",   &m_AlphaKnee, this, isParm|NAN_OK);
  DDB.Double  ("",             "Alpha.@Cap",     DC_,     "",       &m_AlphaAtCap,this, isParm);
  DDB.Visibility(NSHM_All, iMethod==SBM_WhitenBetaStar && fMode);
  DDB.Double  ("",             "Beta",           DC_,     "",       xidWhitenBeta,  this, isParm);
  DDB.Double  ("",             "Beta*",          DC_,     "",       &BetaStar,      this, isResult);
  DDB.Visibility(NSHM_All, (iMethod==SBM_Whiten || iMethod==SBM_WhitenBetaStar) && fMode);
  DDB.Double  ("",             "Rf",             DC_,     "",       xidWhitenRf,    this, isParm);
  DDB.Visibility(NSHM_All, (iMethod==SBM_Whiten || iMethod==SBM_WhitenBetaStar) && bKneeCorr && fMode);
  DDB.Double  ("",             "Rf.Op",          DC_,     "",       &m_RfOp,   this, isResult);
  DDB.Double  ("",             "Rf.Knee",        DC_Qm,   "kg/s",   &m_RfKnee, this, isParm|NAN_OK);
  DDB.Double  ("",             "Rf.@Cap",        DC_,     "",       &m_RfAtCap,this, isParm);
  DDB.Visibility(NSHM_All, (iMethod==SBM_Whiten || iMethod==SBM_WhitenBetaStar) && fMode);
  DDB.Double  ("",             "C",              DC_,     "",       xidWhitenC,     this, isParm|noFile|noSnap);
  DDB.TagComment("=1-Rf");

  DDB.Visibility(NSHM_All, iMethod==SBM_Karra && fMode);
  static DDBValueLst DDB3[]={
    {FM_None,     "None"},
    {FM_LiqFines, "Fines_in_Moist" },
    {FM_FineEff,  "Min_to_O/S" },
    {0}};
  DDB.Text(" ");
  DDB.Byte    ("",                 "FinesMethod",  DC_,     "",       &iFineMeth,    this, isParm|SetOnChange, DDB3);

  DDB.Visibility(NSHM_All, iMethod==SBM_Karra && iFineMeth==FM_FineEff && fMode);
  DDB.Double  ("Fine%toCoarse",    "MinFine",      DC_Frac, "%",      &MinFine,      this, isParm);

  DDB.Visibility();
  DDB.Text("");
  DDB.Visibility(NSHM_All, fMode);
  DDB.Double  ("Calculated_d50",    "Calc_d50",    DC_L,    "um",     &m_d50Op,      this, isResult|0);
  DDB.Double  ("Intersection",      "IntersectPt", DC_L,    "um",     &ICut,         this, isResult|0);
  DDB.Visibility();
  DDB.Double  ("",                  "SoltoO/S",    DC_Frac, "%",      &OSSol,        this, isResult|0);
  DDB.Double  ("",                  "LiqtoO/S",    DC_Frac, "%",      &OSLiq,        this, isResult|0);
  DDB.Double  ("OS_Density",        "OSDens",      DC_Rho,  "kg/m^3", &OSDens,       this, isResult|0);
  DDB.Double  ("US_Density",        "USDens",      DC_Rho,  "kg/m^3", &USDens,       this, isResult|0);
  DDB.Text("");
  DDB.Double  ("UF_Eff_Size",       "UF_Sz",       DC_L,    "mm", &m_EfficiencySize,       this, isParm);
  DDB.Double  ("UF_Efficiency",     "UF_Eff",      DC_Frac,  "%", &m_EfficiencyAtSize,this, isResult|0);

  if ((iMethod==SBM_PartCrv_Individ || iMethod==SBM_PartCrv_Overall) && fMode)
    {
    DDB.Visibility(NSHM_All, (iMethod==SBM_PartCrv_Individ || iMethod==SBM_PartCrv_Overall) && fMode);
    DDB.Object(&PartCrv, this, NULL, NULL, DDB_RqdPage);
    }
  else if ((iMethod==SBM_Karra || iMethod==SBM_Whiten || iMethod==SBM_WhitenBetaStar) && fMode)
    {
    DDB.Visibility(NSHM_All, (iMethod==SBM_Karra || iMethod==SBM_Whiten || iMethod==SBM_WhitenBetaStar) && fMode);
    DDB.Object(&PartCrv2, this, NULL, NULL, DDB_RqdPage);
    }

  DDB.EndStruct();
  }

//--------------------------------------------------------------------------

flag ScreenBlk::DataXchg(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
    case xidMethod:
      if (DCB.rB)
        {
        byte PrevMethod = iMethod;
        iMethod=*DCB.rB;
#if WithIndividPartCrv
        if (iMethod==SBM_PartCrv_Individ || iMethod==SBM_PartCrv_Overall)
          {
          const int DistIndex = (PartCrv.GetSizeDefn()>=0 ? PartCrv.GetSizeDefn() : 0);
          const int PriIdCount = SD_Defn.NPriIds(DistIndex);
          const int RqdNCurves = (iMethod==SBM_PartCrv_Overall ? 1 : Max(1, PriIdCount));
          if (PartCrv.NCurves()!=RqdNCurves)
            {
            PartCrv.SetNCurves(RqdNCurves);
            for (int c=0; c<PartCrv.NCurves(); c++)
              PartCrv.SetEditable(c, true);
            }
          }
#endif
        }
      DCB.B=iMethod;
      return 1;
    case xidWhitenBeta:
      if (DCB.rD)
        {
        Beta=*DCB.rD;
        CalcBetaStar();
        }
      DCB.D=Beta;
      return 1;
    case xidWhitenRf:
      if (DCB.rD)
        m_Rf=*DCB.rD;
      DCB.D=m_Rf;
      return 1;
    case xidWhitenC:
      if (DCB.rD)
        m_Rf=1.0-(*DCB.rD);
      DCB.D=1.0-m_Rf;
      return 1;
    }
  return 0;
  }

//--------------------------------------------------------------------------

flag ScreenBlk::ValidateData(ValidateDataBlk & VDB)
  {
  ASSERT_VALID(&OTemp);
  ASSERT_VALID(&Feed);
  ASSERT_VALID(&UTemp);
  ASSERT_VALID(&YTemp);
  ASSERT_VALID(&Fine);
  flag OK=1;
  return OK;
  }

//--------------------------------------------------------------------------

void ScreenBlk::Setup_Constants()
  {
  if( fabs(ht - ht_chk) < 1.0e-5)
    return;
  double htt = ht / 1000.0;
  if (htt < 50.8)        // Modifying factor for Screen opening
     A = 12.1286 * Pow(htt,0.3162) + 10.2991;
  else
     A = 0.3388 * htt + 14.4122;
  if (wet)
     {
     double T = 1.26 * htt;  // Modifying factor for wet DDScreening
     if (T < 1.0)
        E = 1.0;
     if ((T >= 1.0) && (T <= 2.0))
        E = T;
     if ((T > 2.0) && (T < 4.0))
        E = 1.5 + 0.25 * T;
     if ((T >= 4.0) && (T <= 6.0))
        E = 2.5;
     if ((T > 6.0) && (T <= 10.0))
        E = 3.25 - 0.125 * T;
     if ((T > 10.0) && (T < 12.0))
        E = 4.5 - 0.25 * T;
     if ((T >= 12.0) && (T <= 16.0))
        E = 2.1 - 0.05 * T;
     if ((T > 16.0) && (T < 24.0))
        E = 1.5 - 0.0125 * T;
     if ((T >= 24.0) && (T <= 32.0))
        E = 1.35 - 0.00625 * T;
     if (T > 32.0)
        E = 1.15;
     }
  else
     E = 1.0;

  ht_chk = ht;

  OSSplit=Range(0.0, OSSplit, 1.0);
  m_OSMoist=Range(0.0, m_OSMoist, 1.0);
  }

// -------------------------------------------------------------------------

double ScreenBlk::Distribution(double Topsize, double Lowersize, SpConduit &QFd)
  {
  SQSzDist1 &Sz=*(SQSzDist1::Ptr(QFd.Model()));

  double MassRange = 0.0;
  for (int d=0; d<Sz.NDistributions(); d++)
    if (Sz.DistExists(d))
      {
      CSD_Distribution &D=Sz.Dist(d);
      for (int s=0; s<D.NPriIds(); s++)
        {
        CDArray & Size = D.PriSp[s]->FracPass;
        long len = Size.GetSize();

        if (len >0.0)
          {
          static CDArray CumSize;
          CumSize = Size;
          CumSize.ToCumulative();

          MaxSize=D.TopSize();
          Topsize = Range(D.BottomSize(), Topsize, D.TopSize());
          Lowersize = Range(D.BottomSize(), Lowersize, D.TopSize());

          double LoMass = CumSize.LinearInterpolate(Lowersize, D.Intervals(), false);
          double HiMass = CumSize.LinearInterpolate(Topsize, D.Intervals(), false);

          MassRange += (HiMass - LoMass) * 100.0;
          }
        }
      }

  return MassRange;
  }

// -------------------------------------------------------------------------

double ScreenBlk::OverSize(SpConduit &QFd)
  {
  double Q = Distribution(1e100,ht, QFd); // %Oversize in feed to Screen
  if (Q <= 87)
    return 1.6 - 0.012 * Q;
  else
    return 4.275 + 0.0425 * Q;
  }

//--------------------------------------------------------------------------

double ScreenBlk::NearSize(SpConduit &QFd)
  {
  double Xn = Distribution(ht*1.25,ht*0.75, QFd); // % Nearsize in feed to Screen
  double t = 1.0 - Xn/100.0;
  return 0.844 * Pow(t,3.453);
  }

//--------------------------------------------------------------------------

double ScreenBlk::HalfSize(SpConduit &QFd)
  {
  double R = Distribution(ht/2.0,0.0, QFd); // % Halfsize in feed to Screen
  double C=1.0;
  if (R <= 30.0)
    C = 0.012 * R + 0.7;
  if ((R > 30.0) && (R < 55.0))
    C = 0.1528 * Pow(R,0.564);
  if ((R >= 55.0) && (R < 80.0))
    C = 0.0061 * Pow(R,1.37);
  if (R >= 80.0)
    C = 0.05 * R - 1.5;
  return C;
  }

// -------------------------------------------------------------------------

double ScreenBlk::CalcBetaStar()
  {
  BetaStar = 1.0 - Beta;
  double PN = (1.0 + Beta*BetaStar) * (Exps(m_AlphaOp) - 1.0) / (Exps(m_AlphaOp*BetaStar) + Exps(m_AlphaOp) - 2.0); //Partition Number
  int iter = 0;
  const int MaxIter = 100;
  const double Tol = 1.0e-4;
  while (iter++<MaxIter)
    {
    BetaStar += (BetaStar*(PN-0.5)/10.0); //update estimate of BetaStar
    PN = (1.0 + Beta*BetaStar) * (Exps(m_AlphaOp) - 1.0) / (Exps(m_AlphaOp*BetaStar) + Exps(m_AlphaOp) - 2.0); //Recalc Partition Number
    if (fabs(PN-0.5)<Tol)
      break;
    }
  if (iter>=MaxIter)
    LogError(Tag(), 0, "Unable to calculate BetaStar!");
  return BetaStar;
  }

// -------------------------------------------------------------------------

//inline double ToUM(double d) { return d*1e6;};
//inline double ToM(double d) { return d*1e-6;};

void ScreenBlk::Separate(SpConduit &QFd, SpConduit &QOs, SpConduit &QUs, int s, double SNum)
  {
  ASSERT_VALID(&OTemp);
  ASSERT_VALID(&Feed);
  ASSERT_VALID(&UTemp);
  ASSERT_VALID(&YTemp);
  ASSERT_VALID(&Fine);
  m_d50Op = Max(1.0e-6, m_d50Op);
  m_CutSize = Max(1.0e-6, m_CutSize);
  m_Alpha = Max(1.0e-6, m_Alpha);

  switch (SolveMethod())
    {
    case SM_Direct:
    case SM_Inline:
    case SM_Buffered:
      {
      if (!fMode)
        {
        QmSIn = QFd.QMass(som_Sol);
        QmLIn = QFd.QMass(som_Liq);
        OSSol = OSSplit;
        USSol = 1.0-OSSplit;
        if (QmLIn > 0.0)
          {
          if (m_OSMoist<0.999999)
            {
            OSLiq = OSSol * QmSIn * m_OSMoist / (1.0-m_OSMoist) / QmLIn;
            OSLiq = Range(0.0, OSLiq, 1.0);
            }
          else
            OSLiq = 1.0;

          USLiq = 1.0 - OSLiq;
          }
        else
          {
          OSLiq = 0.0;
          USLiq = 0.0;
          }
        QOs.QSetF(QFd, SetMass_Frac, OSSol, OSLiq, 0.0, Std_P);
        QUs.QSetF(QFd, SetMass_Frac, USSol, USLiq, 0.0, Std_P);
        }
      else
        {
        TotCap   = Cap * SNum;
        DeckFeed = QFd.QMass(som_Sol);
        if (bTrackStatus && iMethod==SBM_Karra && iKM==KM_Area) //model is feed sensative
          SetCI(s, (DeckFeed - TotCap)>0.0); //Set warning tag

        if (1)
          {//check for valid feed...
          const double QmFd = QFd.QMass();
          SpModel * pMdl = QFd.Model();
          //SQSzDist1 * pSz = SQSzDist1::Ptr(pMdl, false);
          SQSzDist1 * pSz = SQSzDist1::Ptr(pMdl, true);
          if (pSz==NULL || !pSz->DistributionsExist())
            {
            QUs.QZero(); //first do QZero to ensure existing SzQual deleted
            QUs.QCopy(QFd);
            QOs.QZero();
            if (QmFd>UsableMass)
              SetCI(3);
            else
              ClrCI(3);
            return;
            }
          ClrCI(3);

          if (QmFd <= UsableMass)
            {
            QUs.QCopy(QFd);
            QOs.QZero();
            return;
            }

          if (PartCrv.GetSizeDefn()!=pSz->DistIndex())
            PartCrv.SetSizeDefn(pSz->DistIndex());//change to correct sizedistribution defn
          if (PartCrv2.GetSizeDefn()!=pSz->DistIndex())
            PartCrv2.SetSizeDefn(pSz->DistIndex());//change to correct sizedistribution defn
          }

        if (iMethod==SBM_PartCrv_Individ || iMethod==SBM_PartCrv_Overall)  // Partition Curve
          {
          if (SQSzDist1::Ptr(QFd.Model(), false))
            {
            QUs.QSetM(QFd, som_ALL, 0.0, Std_P);
            QOs.QSetM(QFd, som_ALL, 0.0, Std_P);

#if WithIndividPartCrv
            SQSzDistFractionInfo Info(PartCrv, (iMethod==SBM_PartCrv_Individ ? -1 : 0));
#else
            if (iMethod==SBM_PartCrv_Individ)
              iMethod=SBM_PartCrv_Overall; //todo screen each ore with different partition curve

            SQSzDistFractionInfo Info(PartCrv, 0);
#endif
            SQSzDist1::SplitSolids(Info, QFd, QUs, QOs);

            OSSol=Info.m_CoarseMass;
            QmSIn = QFd.QMass(som_Sol);
            QmLIn = QFd.QMass(som_Liq);
            if (QmLIn > 1.0e-6)
              {
              m_OSMoistOp = m_OSMoist;
              if (bKneeCorr && Valid(m_OSMoistKnee))
                {
                m_OSMoistKnee=Range(0.0, m_OSMoistKnee, Cap*0.99);
                if (DeckFeed>m_OSMoistKnee)
                  {
                  m_OSMoistOp = m_OSMoist+(m_OSMoistAtCap-m_OSMoist)*(DeckFeed-m_OSMoistKnee*SNum)/GTZ(TotCap-m_OSMoistKnee*SNum);
                  m_d50Op = Max(m_d50Op, 1e-9);
                  }
                }

              if (m_OSMoistOp>1.0e-12)
                {
                m_OSMoistOp = Range(0.0, m_OSMoistOp, 0.999);
                OSLiq = OSSol / (1.0 - m_OSMoistOp) - OSSol;
                OSLiq = Range(0.0, OSLiq, QmLIn * 0.85);
                }
              else
                OSLiq = 0.0;

              USLiq = QmLIn - OSLiq;
              }
            else
              {
              OSLiq = 0.0;
              USLiq = 0.0;
              }

            QUs.QAddM(QFd, som_Liq, USLiq);
            QOs.QAddM(QFd, som_Liq, GEZ(QmLIn-USLiq));

            QOs.SetTemp(QFd.Temp());
            QUs.SetTemp(QFd.Temp());
            }
          }
        else if (iMethod==SBM_Whiten || iMethod==SBM_WhitenBetaStar)
          {
          SQSzDist1 &Sz    =*SQSzDist1::Ptr(QFd.Model());
          SQSzDist1 &SzOs  =*SQSzDist1::Ptr(QOs.Model());
          SQSzDist1 &SzUs  =*SQSzDist1::Ptr(QUs.Model());
          SQSzDist1 &SzTemp=*SQSzDist1::Ptr(QTemp.Model());
          SQSzDist1 &SzFine=*SQSzDist1::Ptr(QFine.Model());

          const double TonsLimit = 1e-8;
          double TonsTotal=0.0;
          for (int d=0; d<Sz.NDistributions(); d++)
            {
            if (Sz.DistExists(d))
              {
              CSD_Distribution &D=Sz.Dist(d);
              for (int s=0; s<D.NPriIds(); s++)
                for (int s1=0; s1<D.NSecIds(s); s1++)
                  TonsTotal += QFd.VMass[D.SzId(s,s1)];
              }
            }

          if (TonsTotal <= TonsLimit)
            {
            QUs.QCopy(QFd);
            QOs.QZero();
            SetCI(4);
            return;
            }
          ClrCI(4);

          m_d50Op = m_CutSize;  // Do separation based on user defined m_d50Op
          FdSol = TonsTotal;
          if (bKneeCorr && Valid(m_CutSizeKnee))
            {
            m_CutSizeKnee=Range(0.0, m_CutSizeKnee, Cap*0.99);
            if (TonsTotal>m_CutSizeKnee)
              {
              m_d50Op = m_d50Op+(m_CutSizeAtCap-m_CutSize)*(TonsTotal-m_CutSizeKnee*SNum)/GTZ(TotCap-m_CutSizeKnee*SNum);
              m_d50Op = Max(m_d50Op, 1e-9);
              }
            }

          m_AlphaOp=m_Alpha;
          if (bKneeCorr && Valid(m_AlphaKnee))
            {
            m_AlphaKnee=Range(0.0, m_AlphaKnee, Cap*0.99);
            if (TonsTotal>m_AlphaKnee)
              {
              m_AlphaOp= m_AlphaOp+(m_AlphaAtCap-m_Alpha)*(TonsTotal-m_AlphaKnee*SNum)/GTZ(TotCap-m_AlphaKnee*SNum);
              m_AlphaOp= Max(m_AlphaOp, 1e-6);
              }
            }

          m_RfOp=m_Rf;
          if (bKneeCorr && Valid(m_RfKnee))
            {
            m_RfKnee=Range(0.0, m_RfKnee, Cap*0.99);
            if (TonsTotal>m_RfKnee)
              {
              m_RfOp = m_RfOp+(m_RfAtCap-m_Rf)*(TonsTotal-m_RfKnee*SNum)/GTZ(TotCap-m_RfKnee*SNum);
              m_RfOp = Max(m_RfOp, 1e-9);
              }
            }

          if (iMethod==SBM_WhitenBetaStar)
            CalcBetaStar(); //recalc to be sure!

          //=========================================================================
          // Transfer All Qualities / SOlids to US and Liquids 50/50
          QOs.QSetF(QFd, SetMass_Frac, 0.0, 0.5, 0.0, Std_P);
          QUs.QSetF(QFd, SetMass_Frac, 1.0, 0.5, 0.0, Std_P);

          OSSol = 0.0;
          USSol = 0.0;
          flag NoSize=True;
          for (d=0; d<Sz.NDistributions(); d++)
            if (Sz.DistExists(d))
              {
              SzUs.AllocDist(d);
              SzOs.AllocDist(d);
              CSD_Distribution &D=Sz.Dist(d);
              CSD_Distribution &DO=SzOs.Dist(d);
              CSD_Distribution &DU=SzUs.Dist(d);
              //PartCrv2.SetNCurves(D.NPriIds());
              //PartCrv2.SetNCurves(Max(1, SD_Defn.NPriIds(0)));
              for (int s=0; s<D.NPriIds(); s++)
                {
                CDArray & Size = D.PriSp[s]->FracPass;

                const int len=Size.GetSize();
                if (len>0)
                  {
                  NoSize=false;
                  CDArray & USize=DU.PriSp[s]->FracPass;
                  CDArray & OSize=DO.PriSp[s]->FracPass;
                  OTemp.SetSize(len);
                  Feed.SetSize(len);
                  UTemp.SetSize(len);
                  YTemp.SetSize(len);

                  double Fractions, x1, y1, tmpF;
                  Fractions = 0.0;
                  for(long i=0 ; i<len ; i++)
                    Fractions += Size[i];
                  if (Fractions <= 0.0)
                    Fractions = 1.0;

                  for(i=0 ; i<len ; i++)
                    YTemp[i] = Size[i] / Fractions;

                  double TonsS=0.0;
                  for (int s1=0; s1<D.NSecIds(s); s1++)
                    TonsS+=QFd.VMass[D.SzId(s,s1)];
                  for(i=0 ; i<len ; i++)
                    {
                    x1      = D.Intervals()[i];
                    y1      = YTemp[i];
                    tmpF    = y1 * TonsS;
                    Feed[i] = tmpF;

                    #if dbgScreen
                      dbgpln("%2i, %5.2f, %5.2f, %6.3f", i,x1,y1,tmpF);
                    #endif
                    }

                  double SpcOSSol = 0.0;
                  double SpcUSSol = 0.0;
                  const double BetaStarUsed = (iMethod==SBM_Whiten ? 1.0 : BetaStar);
                  const double BS_C = 1.0-m_RfOp;
                  //OSize[i] this is badly named it is Eu or "probabilty passing"!!!
                  for (i=0; i<len; i++)
                    {
                    const double LowSz = (i==0 ? D.BottomSize() : D.Intervals()[i-1]);
                    //const double NominalSize = D.Intervals()[i]; //use topsize of range
                    //const double NominalSize = LowSz + ((D.Intervals()[i] - LowSz)/2.0); //use arithmetic mean
                    const double NominalSize = sqrt(LowSz * D.Intervals()[i]); //use geometrical mean
                    const double d0 = NominalSize/m_d50Op;
                    const double dddd = m_AlphaOp*BetaStarUsed*d0;
                    if (dddd>230.0)//limit of exp()
                      OSize[i] = 1.0;
                    else if (iMethod==SBM_Whiten)
                      {//Simplified Whiten where Beta=0.0 and BetaStar=1.0 in SBM_WhitenBetaStar equation
                      const double dd = exp(dddd);
                      OSize[i] = (dd-1.0)/(dd + exp(m_AlphaOp) - 2.0);
                      OSize[i] = Range(0.0, OSize[i] + (m_RfOp * (1.0 - OSize[i])), 1.0);
                      }
                    else if (iMethod==SBM_WhitenBetaStar)
                      {//Whiten with Beta
                      const double dd = exp(m_AlphaOp);
                      OSize[i] = 1.0 - BS_C*(1.0 + Beta*BetaStarUsed*d0)*(dd-1.0)/(exp(dddd) + dd - 2.0);
                      OSize[i] = Range(0.0, OSize[i], 1.0);
                      }

                    OTemp[i] = OSize[i] * Feed[i];
                    SpcOSSol+= OTemp[i];
                    UTemp[i] = Feed[i] - OTemp[i];
                    SpcUSSol+= UTemp[i];
                    }

                  if (SpcOSSol < TonsLimit)
                    {
                    SpcUSSol=GTZ(SpcUSSol);
                    for (i=0; i<len; i++)
                      {
                      OSize[i] = 0.0;
                      USize[i] = (UTemp[i] / SpcUSSol);
                      }
                    OSize[len-1] = 1.0;
                    }
                  if (SpcUSSol < TonsLimit)
                    {
                    SpcOSSol=GTZ(SpcOSSol);
                    for (i=0; i<len; i++)
                      {
                      USize[i] = 0.0;
                      OSize[i] = (OTemp[i] / SpcOSSol);
                      }
                    USize[0] = 1.0;
                    }
                  if ((SpcOSSol > TonsLimit) && (SpcUSSol > TonsLimit))
                    {
                    for (i=0; i<len; i++)
                      {
                      OSize[i] = (OTemp[i] / SpcOSSol);
                      USize[i] = (UTemp[i] / SpcUSSol);
                      }
                    }

                  OSize.AdjustCumulativeEnd(1.0, 0.0, 1.0);//kga 28/4/99
                  USize.AdjustCumulativeEnd(1.0, 0.0, 1.0);//kga 28/4/99

                  double TotSol=GTZ(SpcOSSol+SpcUSSol);
                  double OSSolFrac=SpcOSSol/TotSol;
                  double USSolFrac=SpcUSSol/TotSol;

                  for (s1=0; s1<D.NSecIds(s); s1++)
                    {
                    int Id=D.SzId(s,s1);
                    QOs.SetVMass(Id, QFd, QFd.VMass[Id]*OSSolFrac);
                    QUs.SetVMass(Id, QFd, QFd.VMass[Id]*USSolFrac);
                    }

                  OSSol += SpcOSSol;
                  USSol += SpcUSSol;

                  //partition curve for display...
                  if (s<PartCrv2.NCurves())
                    {
                    CDArray& Crv = PartCrv2.Curve(s);
                    for (i=0; i<len; i++)
                      {
                      if (Feed[i]<1.0e-18)
                        Crv[i] = (i==0 ? 0.0 : Crv[i-1]);
                      else
                        Crv[i] = OTemp[i] / Max(Feed[i], 1e-18);
                      }
                    if (PartCrv2.CurveMode(s) == PC_Frac2Fine)
                      {
                      for (i=0; i<len; i++)
                        Crv[i] = 1.0 - Crv[i];
                      }
                    }
                  }
                }
              }

          QmSIn = QFd.QMass(som_Sol);
          QmLIn = QFd.QMass(som_Liq);
          if (QmLIn > 1.0e-6)
            {
            m_OSMoistOp = m_OSMoist;
            if (bKneeCorr && Valid(m_OSMoistKnee))
              {
              m_OSMoistKnee=Range(0.0, m_OSMoistKnee, Cap*0.99);
              if (DeckFeed>m_OSMoistKnee)
                {
                m_OSMoistOp = m_OSMoist+(m_OSMoistAtCap-m_OSMoist)*(DeckFeed-m_OSMoistKnee*SNum)/GTZ(TotCap-m_OSMoistKnee*SNum);
                m_d50Op = Max(m_d50Op, 1e-9);
                }
              }
            if (m_OSMoistOp>1.0e-12)
              {
              m_OSMoistOp = Range(0.0, m_OSMoistOp, 0.999);
              OSLiq = OSSol / (1.0 - m_OSMoistOp) - OSSol;
              OSLiq = Range(0.0, OSLiq, QmLIn * 0.85);
              }
            else
              OSLiq = 0.0;

            USLiq = QmLIn - OSLiq;

            // Convert Liquids to % of feed
            OSLiq = OSLiq / QmLIn;
            USLiq = USLiq / QmLIn;
            }
          else
            {
            OSLiq = 0.0;
            USLiq = 0.0;
            }

          //==========================================================================

          FineOS = OSLiq;
          FinetoCoarse = FineOS * FineTons;

          // Convert Solids to % of feed
          OSSol = OSSol / TonsTotal;
          USSol = USSol / TonsTotal;

          for (int s=0; s<SDB.Count(); s++)
            if (SDB[s].IsLiq())
              {
              QOs.SetVMass(s, QFd, QFd.VMass[s]*OSLiq);
              QUs.SetVMass(s, QFd, QFd.VMass[s]*USLiq);
              }

          QOs.SetTemp(QFd.Temp());
          QUs.SetTemp(QFd.Temp());

          if (NoSize)
            {
            LogNote(Tag(), 0, "No Screen feed size curve");
            QUs.QCopy(QFd);
            QOs.QZero();
            }

          //===========================================================================

          }
        else
          {

          SQSzDist1 &Sz=*SQSzDist1::Ptr(QFd.Model());
          SQSzDist1 &SzOs=*SQSzDist1::Ptr(QOs.Model());
          SQSzDist1 &SzUs=*SQSzDist1::Ptr(QUs.Model());
          SQSzDist1 &SzTemp =*SQSzDist1::Ptr(QTemp.Model());
          SQSzDist1 &SzFine =*SQSzDist1::Ptr(QFine.Model());

          const double TonsLimit = 1e-8;
          double TonsTotal=0.0;
          for (int d=0; d<Sz.NDistributions(); d++)
            {
            if (Sz.DistExists(d))
              {
              CSD_Distribution &D=Sz.Dist(d);
              for (int s=0; s<D.NPriIds(); s++)
                for (int s1=0; s1<D.NSecIds(s); s1++)
                  TonsTotal+=QFd.VMass[D.SzId(s,s1)];
              }
            }

          if (TonsTotal <= TonsLimit)
            {
            QUs.QCopy(QFd);
            QOs.QZero();
            SetCI(4);
            return;
            }
          ClrCI(4);

          double Dem, Tus;
          FdSol = TonsTotal;

          if (iKM==KM_Area)  // Calculate m_d50Op based on screen area and feed characteristics
            {
            ht = hmm;

            Setup_Constants();
            B    = OverSize(QFd);
            C    = HalfSize(QFd);
            D    = 1.1 - 0.1*s;
            G    = NearSize(QFd);
            F    = QFd.Rho(som_Sol) / 1602.0;
            Dem  = A*B*C*D*E*F*G;
            Tus  = Distribution(ht,0.0, QFd); //Theoretical undersize flow thru Screen tph
            Area = SArea * SNum;  // Total area of all screens
            Tus  = (Tus / 100.0) * TonsTotal / Area;
            // calculate m_d50Op in um
            if ((Dem > 1.0e-5) && (Tus>1.0e-20)) //If not, should generate error message!
              {
              double Teff = Pow((Tus / Dem),-0.148);
              m_d50Op = Range(1.0e-6, ht * Teff, MaxSize);
              }
            else
              m_d50Op = 1.0e-6;
            /*if (wet)
              {
              //if (curve)
              //  EWash = WashEffCurve.Yx(QmWIn);
              EWash = Max(1e-3, EWash);
              m_d50Op = m_d50Op / EWash;
              }*/
            }
          else
            m_d50Op = m_CutSize;  // Do separation based on user defined m_d50Op

          //=========================================================================
          // Transfer All Qualities / SOlids to US and Liquids 50/50
          QOs.QSetF(QFd, SetMass_Frac, 0.0, 0.5, 0.0, Std_P);
          QUs.QSetF(QFd, SetMass_Frac, 1.0, 0.5, 0.0, Std_P);

          OSSol = 0.0;
          USSol = 0.0;
          flag NoSize=True;
          for (d=0; d<Sz.NDistributions(); d++)
            if (Sz.DistExists(d))
              {
              SzUs.AllocDist(d);
              SzOs.AllocDist(d);
              CSD_Distribution &D=Sz.Dist(d);
              CSD_Distribution &DO=SzOs.Dist(d);
              CSD_Distribution &DU=SzUs.Dist(d);
              //PartCrv2.SetNCurves(D.NPriIds());
              //PartCrv2.SetNCurves(Max(1, SD_Defn.NPriIds(0)));
              for (int s=0; s<D.NPriIds(); s++)
                {
                CDArray & Size = D.PriSp[s]->FracPass;

                int len=Size.GetSize();
                if (len>0)
                  {
                  NoSize=false;
                  CDArray & USize=DU.PriSp[s]->FracPass;
                  CDArray & OSize=DO.PriSp[s]->FracPass;
                  OTemp.SetSize(len);
                  Feed.SetSize(len);
                  UTemp.SetSize(len);
                  YTemp.SetSize(len);

                  double Fractions, x1, y1, tmpF;
                  Fractions = 0.0;
                  for(long i=0 ; i<len ; i++)
                    Fractions += Size[i];
                  if (Fractions <= 0.0)
                    Fractions = 1.0;

                  for(i=0 ; i<len ; i++)
                    YTemp[i] = Size[i] / Fractions;

                  double TonsS=0.0;
                  for (int s1=0; s1<D.NSecIds(s); s1++)
                    TonsS+=QFd.VMass[D.SzId(s,s1)];
                  for(i=0 ; i<len ; i++)
                    {
                    x1      = D.Intervals()[i];
                    y1      = YTemp[i];
                    tmpF    = y1 * TonsS;
                    Feed[i] = tmpF;

                    #if dbgScreen
                      dbgpln("%2i, %5.2f, %5.2f, %6.3f", i,x1,y1,tmpF);
                    #endif
                    }

                  double SpcOSSol = 0.0;
                  double SpcUSSol = 0.0;
                  for (i=0; i<len; i++)
                    {
                    const double LowSz = (i==0 ? D.BottomSize() : D.Intervals()[i-1]);
                    //const double NominalSize = D.Intervals()[i]; //use topsize of range
                    //const double NominalSize = LowSz + ((D.Intervals()[i] - LowSz)/2.0); //use arithmetic mean
                    const double NominalSize = sqrt(LowSz * D.Intervals()[i]); //use geometrical mean
                    OSize[i] = (1.0 - exp((0.0 - 0.693) * Pow(NominalSize/m_d50Op, 5.846))); //Karra

                    // If the user has specified a minimum percentage of material reporting
                    // to oversize, do this now.
                    if (iFineMeth==FM_FineEff)
                      {
                      MinFine  = Range(0.0, MinFine, 1.0);
                      OSize[i] = Max(OSize[i], MinFine);
                      }

                    OTemp[i] = OSize[i] * Feed[i];
                    SpcOSSol+= OTemp[i];
                    UTemp[i] = Feed[i] - OTemp[i];
                    SpcUSSol+= UTemp[i];
                    }

                  if (SpcOSSol < TonsLimit)
                    {
                    SpcUSSol=GTZ(SpcUSSol);
                    for (i=0; i<len; i++)
                      {
                      OSize[i] = 0.0;
                      USize[i] = (UTemp[i] / SpcUSSol);
                      }
                    OSize[len-1] = 1.0;
                    }
                  if (SpcUSSol < TonsLimit)
                    {
                    SpcOSSol=GTZ(SpcOSSol);
                    for (i=0; i<len; i++)
                      {
                      USize[i] = 0.0;
                      OSize[i] = (OTemp[i] / SpcOSSol);
                      }
                    USize[0] = 1.0;
                    }
                  if ((SpcOSSol > TonsLimit) && (SpcUSSol > TonsLimit))
                    {
                    for (i=0; i<len; i++)
                      {
                      OSize[i] = (OTemp[i] / SpcOSSol);
                      USize[i] = (UTemp[i] / SpcUSSol);
                      }
                    }

                  OSize.AdjustCumulativeEnd(1.0, 0.0, 1.0);//kga 28/4/99
                  USize.AdjustCumulativeEnd(1.0, 0.0, 1.0);//kga 28/4/99

                  double TotSol=GTZ(SpcOSSol+SpcUSSol);
                  double OSSolFrac=SpcOSSol/TotSol;
                  double USSolFrac=SpcUSSol/TotSol;

                  for (s1=0; s1<D.NSecIds(s); s1++)
                    {
                    int Id=D.SzId(s,s1);
                    QOs.SetVMass(Id, QFd, QFd.VMass[Id]*OSSolFrac);
                    QUs.SetVMass(Id, QFd, QFd.VMass[Id]*USSolFrac);
                    }

                  OSSol += SpcOSSol;
                  USSol += SpcUSSol;

                  if (iFineMeth==FM_FineEff || iFineMeth==FM_None)
                    {
                    if (s<PartCrv2.NCurves())
                      {
                      CDArray& Crv = PartCrv2.Curve(s);
                      for (i=0; i<len; i++)
                        {
                        if (Feed[i]<1.0e-18)
                          Crv[i] = (i==0 ? 0.0 : Crv[i-1]);
                        else
                          Crv[i] = OTemp[i] / Max(Feed[i], 1e-18);
                        }
                      if (PartCrv2.CurveMode(s) == PC_Frac2Fine)
                        {
                        for (i=0; i<len; i++)
                          Crv[i] = 1.0 - Crv[i];
                        }
                      }
                    else
                      {
                      int xx=1;//?????
                      }
                    }
                  }
                }
              }

          QmSIn = QFd.QMass(som_Sol);
          QmLIn = QFd.QMass(som_Liq);
          if (QmLIn > 1.0e-6)
            {
            m_OSMoistOp = m_OSMoist;
            if (bKneeCorr && Valid(m_OSMoistKnee))
              {
              m_OSMoistKnee=Range(0.0, m_OSMoistKnee, Cap*0.99);
              if (DeckFeed>m_OSMoistKnee)
                {
                m_OSMoistOp = m_OSMoist+(m_OSMoistAtCap-m_OSMoist)*(DeckFeed-m_OSMoistKnee*SNum)/GTZ(TotCap-m_OSMoistKnee*SNum);
                m_d50Op = Max(m_d50Op, 1e-9);
                }
              }
            if (m_OSMoistOp>1.0e-12)
              {
              m_OSMoistOp = Range(0.0, m_OSMoistOp, 0.999);
              OSLiq = OSSol / (1.0 - m_OSMoistOp) - OSSol;
              OSLiq = Range(0.0, OSLiq, QmLIn * 0.85);
              }
            else
              OSLiq = 0.0;

            USLiq = QmLIn - OSLiq;

            // Convert Liquids to % of feed
            OSLiq = OSLiq / QmLIn;
            USLiq = USLiq / QmLIn;
            }
          else
            {
            OSLiq = 0.0;
            USLiq = 0.0;
            }

            //==========================================================================

          if (iFineMeth==FM_LiqFines)
            {
            QOs.QSetF(QFd, SetMass_Frac, 0.0, 0.5, 0.0, Std_P);
            QUs.QSetF(QFd, SetMass_Frac, 1.0, 0.5, 0.0, Std_P);

            OSSol = 0.0;
            USSol = 0.0;
            NoSize=True;
            for (d=0; d<Sz.NDistributions(); d++)
              if (Sz.DistExists(d))
                {
                SzUs.AllocDist(d);
                SzOs.AllocDist(d);
                CSD_Distribution &D=Sz.Dist(d);
                CSD_Distribution &DO=SzOs.Dist(d);
                CSD_Distribution &DU=SzUs.Dist(d);
                //PartCrv2.SetNCurves(D.NPriIds());
                //PartCrv2.SetNCurves(Max(1, SD_Defn.NPriIds(0)));
                for (int s=0; s<D.NPriIds(); s++)
                  {
                  CDArray & Size = D.PriSp[s]->FracPass;

                  int len=Size.GetSize();
                  if (len>0)
                    {
                    NoSize=false;
                    CDArray & USize=DU.PriSp[s]->FracPass;
                    CDArray & OSize=DO.PriSp[s]->FracPass;
                    OTemp.SetSize(len);
                    Feed.SetSize(len);
                    UTemp.SetSize(len);
                    YTemp.SetSize(len);

                    double Fractions, x1, y1, tmpF;
                    Fractions = 0.0;
                    for(long i=0 ; i<len ; i++)
                      Fractions += Size[i];
                    if (Fractions <= 0.0)
                      Fractions = 1.0;

                    for(i=0 ; i<len ; i++)
                      YTemp[i] = Size[i] / Fractions;

                    double TonsS=0.0;
                    for (int s1=0; s1<D.NSecIds(s); s1++)
                      TonsS+=QFd.VMass[D.SzId(s,s1)];
                    for(i=0 ; i<len ; i++)
                      {
                      x1      = D.Intervals()[i];
                      y1      = YTemp[i];
                      tmpF    = y1 * TonsS;
                      Feed[i] = tmpF;

                      #if dbgScreen
                        dbgpln("%2i, %5.2f, %5.2f, %6.3f", i,x1,y1,tmpF);
                      #endif
                      }

                    double SpcOSSol = 0.0;
                    double SpcUSSol = 0.0;
                    for (i=0; i<len; i++)
                      {
                      const double LowSz = (i==0 ? D.BottomSize() : D.Intervals()[i-1]);
                      //const double NominalSize = D.Intervals()[i]; //use topsize of range
                      //const double NominalSize = LowSz + ((D.Intervals()[i] - LowSz)/2.0); //use arithmetic mean
                      const double NominalSize = sqrt(LowSz * D.Intervals()[i]); //use geometrical mean
                      OSize[i] = (1.0 - exp((0.0 - 0.693) * Pow(NominalSize/m_d50Op, 5.846))); //Karra
                      OSize[i] = Range(0.0, OSize[i] + OSLiq * (1.0 - OSize[i]), 1.0);
                      OTemp[i] = OSize[i] * Feed[i];
                      SpcOSSol+= OTemp[i];
                      UTemp[i] = Feed[i] - OTemp[i];
                      SpcUSSol+= UTemp[i];
                      }

                    if (SpcOSSol < TonsLimit)
                      {
                      SpcUSSol=GTZ(SpcUSSol);
                      for (i=0; i<len; i++)
                        {
                        OSize[i] = 0.0;
                        USize[i] = (UTemp[i] / SpcUSSol);
                        }
                      OSize[len-1] = 1.0;
                      }
                    if (SpcUSSol < TonsLimit)
                      {
                      SpcOSSol=GTZ(SpcOSSol);
                      for (i=0; i<len; i++)
                        {
                        USize[i] = 0.0;
                        OSize[i] = (OTemp[i] / SpcOSSol);
                        }
                      USize[0] = 1.0;
                      }
                    if ((SpcOSSol > TonsLimit) && (SpcUSSol > TonsLimit))
                      {
                      for (i=0; i<len; i++)
                        {
                        OSize[i] = (OTemp[i] / SpcOSSol);
                        USize[i] = (UTemp[i] / SpcUSSol);
                        }
                      }

                    OSize.AdjustCumulativeEnd(1.0, 0.0, 1.0);//kga 28/4/99
                    USize.AdjustCumulativeEnd(1.0, 0.0, 1.0);//kga 28/4/99

                    double TotSol=GTZ(SpcOSSol+SpcUSSol);
                    double OSSolFrac=SpcOSSol/TotSol;
                    double USSolFrac=SpcUSSol/TotSol;

                    for (s1=0; s1<D.NSecIds(s); s1++)
                      {
                      int Id=D.SzId(s,s1);
                      QOs.SetVMass(Id, QFd, QFd.VMass[Id]*OSSolFrac);
                      QUs.SetVMass(Id, QFd, QFd.VMass[Id]*USSolFrac);
                      }

                    OSSol += SpcOSSol;
                    USSol += SpcUSSol;

                    CDArray& Crv = PartCrv2.Curve(s);
                    for (i=0; i<len; i++)
                      {
                      if (Feed[i]<1.0e-18)
                        Crv[i] = (i==0 ? 0.0 : Crv[i-1]);
                      else
                        Crv[i] = OTemp[i] / Max(Feed[i], 1e-18);
                      }
                    if (PartCrv2.CurveMode(s) == PC_Frac2Fine)
                      {
                      for (i=0; i<len; i++)
                        Crv[i] = 1.0 - Crv[i];
                      }
                    }
                  }
                }

            QmSIn = QFd.QMass(som_Sol);
            QmLIn = QFd.QMass(som_Liq);
            if (QmLIn > 1.0e-6)
              {
              m_OSMoistOp = m_OSMoist;
              if (bKneeCorr && Valid(m_OSMoistKnee))
                {
                m_OSMoistKnee=Range(0.0, m_OSMoistKnee, Cap*0.99);
                if (DeckFeed>m_OSMoistKnee)
                  {
                  m_OSMoistOp = m_OSMoist+(m_OSMoistAtCap-m_OSMoist)*(DeckFeed-m_OSMoistKnee*SNum)/GTZ(TotCap-m_OSMoistKnee*SNum);
                  m_d50Op = Max(m_d50Op, 1e-9);
                  }
                }
              if (m_OSMoistOp>1.0e-12)
                {
                m_OSMoistOp = Range(0.0, m_OSMoistOp, 0.999);
                OSLiq = OSSol / (1.0 - m_OSMoistOp) - OSSol;
                OSLiq = Range(0.0, OSLiq, QmLIn);
                }
              else
                OSLiq = 0.0;
              USLiq = QmLIn - OSLiq;

              // Convert Liquids to % of feed
              OSLiq = OSLiq / QmLIn;
              USLiq = USLiq / QmLIn;
              }
            else
              {
              OSLiq = 0.0;
              USLiq = 0.0;
              }
            } //hss2

          FineOS = OSLiq;
          FinetoCoarse = FineOS * FineTons;

          // Convert Solids to % of feed
          OSSol = OSSol / TonsTotal;
          USSol = USSol / TonsTotal;

          for (int s=0; s<SDB.Count(); s++)
            if (SDB[s].IsLiq())
              {
              QOs.SetVMass(s, QFd, QFd.VMass[s]*OSLiq);
              QUs.SetVMass(s, QFd, QFd.VMass[s]*USLiq);
              }

          QOs.SetTemp(QFd.Temp());
          QUs.SetTemp(QFd.Temp());

          if (NoSize)
            {
            LogNote(Tag(), 0, "No Screen feed size curve");
            QUs.QCopy(QFd);
            QOs.QZero();
            }

          //===========================================================================

          }
        }
      break;
      }
    } //end switch (SlvMd)


    // Calculate Efficiency to Underflow of Deck at Specified Size
    // m_EfficiencyAtSize = % Mass in Underflow at Size / % Mass in Feed at Size

    SQSzDist1 &Sz    =*SQSzDist1::Ptr(QFd.Model());
    SQSzDist1 &SzOs  =*SQSzDist1::Ptr(QOs.Model());
    SQSzDist1 &SzUs  =*SQSzDist1::Ptr(QUs.Model());

    for (int d=0; d<Sz.NDistributions(); d++)
    {
      if (Sz.DistExists(d))
      {

      double lFeed = 0.0;
      double lUnderflow = 0.0;

      CSD_Distribution &D=Sz.Dist(d);
      for (int s=0; s<D.NPriIds(); s++)
        {
        double lfeed_frac;
        lfeed_frac = Sz.FractionPassingSize(d, s , m_EfficiencySize );
        lFeed += lfeed_frac*Sz.TotalMass(d,s);
        double lunderflow_frac;
        lunderflow_frac = SzUs.FractionPassingSize(d, s , m_EfficiencySize  );
        lUnderflow += lunderflow_frac*SzUs.TotalMass(d,s);

        }

        m_EfficiencyAtSize = lUnderflow/GTZ(lFeed);

      }
    }

  OSDens = QOs.Rho();
  USDens = QUs.Rho();
  ICut = Intersection(QUs, QOs);
  }

//--------------------------------------------------------------------------

double ScreenBlk::Intersection(SpConduit &QUs, SpConduit &QOs)
  {
  SQSzDist1 &USz=*(SQSzDist1::Ptr(QUs.Model()));
  SQSzDist1 &OSz=*(SQSzDist1::Ptr(QOs.Model()));

  double MassRange = 0.0, Cut;
  for (int d=0; d<USz.NDistributions(); d++)
    if (USz.DistExists(d) && OSz.DistExists(d))
      {
      CSD_Distribution &D1 = USz.Dist(d);
      CSD_Distribution &D2 = OSz.Dist(d);
      CDArray Tons, CutPt;
      int n = D1.NPriIds();
      Tons.SetSize(n);
      CutPt.SetSize(n);
      for (int s=0; s<D1.NPriIds(); s++)
        {
        CDArray & Size1 = D1.PriSp[s]->FracPass;
        CDArray & Size2 = D2.PriSp[s]->FracPass;
        long len = Size1.GetSize();
        CDArray YTemp1, YTemp2, USize, OSize, RUSize;
        YTemp1.SetSize(len);
        YTemp2.SetSize(len);
        USize.SetSize(len);
        OSize.SetSize(len);
        RUSize.SetSize(len);

        if (len >0.0)
          {
          double Fr1 = 0.0, Fr2 = 0.0;
          int top;

          for(long i=0 ; i<len ; i++)
            {
            Fr1 += ChkZero(Size1[i]);
            Fr2 += ChkZero(Size2[i]);
            }
          if (Fr1 <= 0.0)
            Fr1 = 1.0;
          if (Fr2 <= 0.0)
            Fr2 = 1.0;

          Tons[s] = 0.0;
          for (int s1=0; s1<D1.NSecIds(s); s1++)
            Tons[s]+= QOs.VMass[D1.SzId(s,s1)] + QUs.VMass[D1.SzId(s,s1)];

          for(i=0 ; i<len ; i++)
            {
            YTemp1[i] = ChkZero(Size1[i]) / Fr1;
            YTemp2[i] = ChkZero(Size2[i]) / Fr2;
            }

          USize[1] = YTemp1[0];
          OSize[1] = YTemp2[0];
          RUSize[1] = 1.0 - USize[1];
          for (i=2; i<len; i++)
            {
            USize[i] = USize[i-1] + YTemp1[i-1];
            OSize[i] = OSize[i-1] + YTemp2[i-1];
            RUSize[i] = 1.0 - USize[i];
            }

          for (i=2; i<len; i++)
            {
            const double U1 = RUSize[i];
            const double O1 = OSize[i];
            if (O1 >= U1)
              {
              top = i;
              i = len;
              }
            }

          if ((top >= 2) && (top < len))
            {
            // First determine linear equation for under size
            double x1, x2, y1, y2, Um, Uc, Om, Oc;
            x1 = D1.Intervals()[top-1];
            x2 = D1.Intervals()[top];
            y1 = RUSize[top-1];
            y2 = RUSize[top];

            Um = (y2 - y1)/GTZ(x2 - x1);
            Uc = y2 - Um * x2;

            // Linear equation for over size
            x1 = D2.Intervals()[top-1];
            x2 = D2.Intervals()[top];
            y1 = OSize[top-1];
            y2 = OSize[top];

            Om = (y2 - y1)/GTZ(x2 - x1);
            Oc = y2 - Om * x2;

            // Now find intersection of the lines
            if (Um == Om)
              Cut = 0.0;
            else
              Cut = (Oc - Uc)/(Um - Om);
            }
          else
            Cut = 0.0;
          CutPt[s] = Cut;
          }
        }
      // Calculate weighted avarage cut-point
      double TotalTons = 0.0;
      Cut = 0.0;
      for (s=0; s<D1.NPriIds(); s++)
        {
        Cut+= CutPt[s] * Tons[s];
        TotalTons+= Tons[s];
        }
      if (TotalTons > 1e-12)
        Cut = Cut/TotalTons;
      else
        Cut = 0.0;
      }
    return Cut;
  }

//--------------------------------------------------------------------------

flag ScreenBlk::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case  1: pS="Top Deck Capacity Exceeded";   return 1;
    case  2: pS="Bottom Deck Capacity Exceeded";   return 1;
    case  3: pS="W\tNo feed Size distributions defined"; return 1;
    case  4: pS="N\tInsufficient Solids in Feed";    return 1;
    default:
      return TaggedObject::CIStrng(No, pS);
    }
  }


//==========================================================================
/*#D:#T:Screen
#X:#h<General Description>#nThe screen is used to seperate solids
and liquids into two, or three product streams.  The separation can be based on
size distribution curves and a m_d50Op of the screen, or it can be used as a simple
splitter.
#nWhen the screen is used to perform a proper screening function the feed to the
screen must have a size distribution. The screen will then manipulate the curve
to produce two products, an oversize fraction and an undersize fraction, from
each screen deck.

#n#nTwo screening options exist :
#n1) Use the Karra method to determine the screen cut point.  This can be
calculated in one of two ways:
#n  a) Calculate the m_d50Op of the screen based on screen area, screen aperture,
percentage near size, half size and oversize material, wet or dry screening,
ore density and washing effiency, if wet screening is used.
#n  b) Define the m_d50Op of the screen.
#nUsing the m_d50Op (either calculated or defined), the fraction of material in
each size fraction in the feed size distribution curve which will report to
oversize is computed.  The difference reports to the undersize.
#nThe amount of solids reporting to the oversize fraction is also influenced by
the amount of liquid which flows over the stream.  Some solids are entrained with
the liquid, and hence the separation is not perfect.

#n#n2)In the case of the partition curve, the model will distribute the feed
material based on the user defined partition curve.

#n#nThe percentage moisture of the oversize is defined by the user.  The balance
of the feed liquid flows through the screen.

#n#nThe alogorithms used in this model were obtained from the following paper :
#nCIM Mineral Engineering Bulletin, April 1979.
#nDevelopment of a model for predicting the screening performance of a vibrating
screen.  V.K.Karra
#n#n
#n#h<Inputs and Outputs>#w
Label    Input/Output  Minimum number  Maximum number#n
Feed          Input          1               5#n
WashWater     Input          0               5#n
UnderSize     Output         1               1#n
MidSize       Output         0               1#n
OverSize      Output         0               1#n
#a#n
#h<Variables to be supplied by the user>#n
#i<Mode> : This variable is only visible in mass balancing mode.  The user
may choose either screen or split mode.  Screen mode will perform a full
screening calculation, whereas split will separate the solids and liquids
based on user defined fractions.#n
#i<OSSolids> : If the user chooses 'Split' this variable becomes visible.  The
user must specify the fraction of solids reporting to the oversize stream.#n
#i<OSMoist> : The moisture content of the oversize material.  The balance
of the liquids will report to the undersize stream.#n
#i<Method> : The method of obtaining the m_d50Op, either
#n'ScreenArea' which will calculate the m_d50Op based on the parameters below, or
#n'd50Method' in which case the user must insert the required m_d50Op.#n
#i<m_CutSize> : The user defined m_d50Op.#n
#i<Screening Area> : The screen area.#n
#i<Throughfall Aperture> : The actual screen aperture size seen by the
particles.  This is calculated using the following formula :
#nHt = (h + d)#*cosA - d  where
#nd - wire diameter
#nh - aperture of screen
#nA - the inclination angle to the horizontal of the screen#n
#i<Wet or Dry screening> : The user must select which method is required.
#i<Curve?> : This parameter is only necessary if wet screening has been
selected.  The user must select yes or no.  It the user selects yes, this
means that the washing efficiency will be found from a curve.  In this case,
the data points must be inputted on the wash curve page.  The points can
either be read in from a spread sheet, saved as a comma delimited file (CSV),
or typed in directly in the space required, (see help for curves).
The amount of wash water used is shown on the x axis.  The corresponding wash
efficiency, 0 to 1, is given on the y axis.  If the user selects no, a number
for the washing efficiency, from 0 to 100%, must be supplied in the space
provided at WashEff.#n
#i<Datum required> : The relative elevation of the screen with respect to
the common datum of the system.#n
#n#n
#h<Associated variables>#n
#i<Datum> : The height of the screen above the system datum.#n
#i<m_d50Op> : The particle size, at which 50% of the material will report to
the oversize, and 50% to the undersize.#n
#i<IntersectPt> : The size at which the cummulative percentage oversize of
the fine product (the under size) is equal to the cummulative percentage
undersize of the coarse product (the over flow).#n
#i<SoltoOS> : The percentage of solids in the feed which reports to the
screen over size product.#n
#i<LiqtoOS> : The percentage of liquids in the feed which reports to the
screen over size product.#n
#i<OSDens> : The density of the material, solids and liquids, which
reports to the screen oversize.#n
#i<USDens> : The density of the material, solids and liquids, which
reports to the screen undersize.#n
#n
#n#h<Other>#n
Default model prefix:SCR#n
Short name:Screen#n
Model type:Unit#n
#G:Units
*/
//==========================================================================

const byte ioidFd = 0;
const byte ioidWW = 1;
const byte ioidUS = 2;
const byte ioidMS = 3;
const byte ioidOS = 4;

static IOAreaRec ScreenIOAreaList[] =
  {{"Feed",                       "Feed",       ioidFd, LIO_In0 ,   nc_MLnk, 1, 5},//, IOGRP(1)},
   {"Wash water",                 "WashWater",  ioidWW, LIO_In1 ,   nc_MLnk, 0, 5},
   {"Under size material",        "UnderSize",  ioidUS, LIO_Out0,   nc_MLnk, 1, 1},//, IOGRP(1)},
   {"Mid size material",          "MidSize",    ioidMS, LIO_Out1,   nc_MLnk, 0, 1},
   {"Over size material/solids",  "OverSize",   ioidOS, LIO_Out2,   nc_MLnk, 0, 1},
   {NULL}};

static double Drw_Screen[] = { DD_Poly, -10,2, 10,0, 10,-2, 0,-5, -10,0, -10,2,
                        DD_Poly, -10,3, 10,1,
                        DD_Poly, -10,4, 10,2,
                        DD_End };

//--------------------------------------------------------------------------

const byte OpenScreen =0;
const byte ClosedScreen =1;

IMPLEMENT_MODELUNIT(Screen, "Screen", "", Drw_Screen, "SizeSeparation", "SCR", TOC_ALL|TOC_DYNAMICFLOW|TOC_GRP_SIZEDST|TOC_SIZEDIST,
                    "Size Separation:Screen (Single/Double Deck)",
                    "Sizing Screen")
Screen::Screen(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MdlNode(pClass_, TagIn, pAttach, eAttach),
  TopDeck(&ScreenBlkClass, "TopDeck", this, TOA_Embedded),
  BottomDeck(&ScreenBlkClass, "BottomDeck", this, TOA_Embedded),
  QFd("QFd", this, TOA_Embedded),
  QMs("QMs", this, TOA_Embedded),
  WashEffCurve("DataOnly", "WashCrv", this, TOA_Embedded)
  {
  AttachIOAreas(ScreenIOAreaList);
  iType=OpenScreen;
  QmWIn  = 0.0;
  nDecks = 0;
  NumS   = 1.0;
  };

// -------------------------------------------------------------------------
/*#F:This provides access to the variables of the model and calls the
BuildDataDefn for objects used by the model.*/
void Screen::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);

  DDB.Text    ("");
  DDB.Double  ("NumberOfScreens",  "NumScreen",  DC_,  "",    &NumS,         this, isParm);

  DDB.Visibility(NM_Dynamic|SM_All|HM_All);
  DDB.Text    ("");
  static DDBValueLst DDB0[]={
    {0,  "Open"},
    {1,  "Closed"},
    {0}};
  DDB.Byte    ("",          "Scrn_Type",   DC_,  "",  &iType,      this, isParm|SetOnChange, DDB0);

  BuildDataDefnElevation(DDB);
  DDB.Visibility();

  DDB.Text    ("");
  BuildDataDefnShowIOs(DDB);

  if (nDecks>0)
    DDB.Object(&TopDeck, this, NULL, NULL, DDB_RqdPage);
  if (nDecks>1)
    DDB.Object(&BottomDeck, this, NULL, NULL, DDB_RqdPage);

  //DDB.Object(&WashEffCurve, this, NULL, NULL, DDB_RqdPage);

  DDB.EndStruct();
  }

//--------------------------------------------------------------------------

flag Screen::DataXchg(DataChangeBlk & DCB)
  {
  if (MdlNode::DataXchg(DCB))
    return 1;
  if (nDecks>0)
    if (TopDeck.DataXchg(DCB))
      return 1;
  if (nDecks>1)
    if (BottomDeck.DataXchg(DCB))
      return 1;
  return 0;
  }

//--------------------------------------------------------------------------

flag Screen::ValidateData(ValidateDataBlk & VDB)
  {

  return MdlNode::ValidateData(VDB);
  }

//---------------------------------------------------------------------------

void Screen::ConnectionsChanged()
  {
  nDecks=0;
  if (IOWithId_Self(ioidMS)>=0)
    nDecks++;
  if (IOWithId_Self(ioidOS)>=0)
    nDecks++;
  TopDeck.SetTag("TopDeck");
  StructureChanged(this);
  };

//---------------------------------------------------------------------------

void Screen::ConfigureJoins()
  {
  int i;
  switch (NetMethod())
    {
    case NM_Probal:
      for (i=0; (i<NoProcessIOs()); i++)
        SetIO_Join(i, 0);
      break;
    case NM_Dynamic:
      if (iType==OpenScreen)
        for (i=0; i<NoProcessIOs(); i++)
          SetIO_Open(i, 0, false, ESS_Denied);
      else
        for (i=0; i<NoProcessIOs(); i++)
          SetIO_Join(i, 0);
      break;
    }
  };

//--------------------------------------------------------------------------

void Screen::EvalJoinPressures(long JoinMask)
  {
  for (int i = 0; i < NoFlwIOs(); i++)
    Set_IOP_Self(i, AtmosPress(IODatum_Term(i)));
  };

// --------------------------------------------------------------------------
/*
flag Screen::EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo)
  {

  if (IOId_Self(IONo)==idFeed)
    {
    pFlwBlk pFB = IOFB(IONo);
    SpConduit & QFd = *IOConduit(IONo);

    double Q=GEZ(pFB->GetQm());
    double DQ=Max(1.0e-3, 1.0e-2*Q);

    double FlowLoss0=Meth[iMethod]->DP(Q, QFd);
    double FlowLoss1=Meth[iMethod]->DP(Q+DQ, QFd);
    double DFlowLoss=(FlowLoss1-FlowLoss0)/DQ;

    pFB->SetQmFree();
    pFB->ClrDerivsBad();
    pFB->SetDPb(0.0, 0.0); // In is Positive
    pFB->SetDPq(FlowLoss0, DFlowLoss);
    return true;
    }
  return false;
  };*/

//--------------------------------------------------------------------------
/*#F:
This determines what material should leave through each outlet,
and rate of change of the contents resulting from the flow, of material, out of each outlet
of the surge unit.
*/
void Screen::EvalProducts(CNodeEvalIndex & NEI)
  {
  if (NoFlwIOs()==0)
    return;

 const int ioFd=IOWithId_Self(ioidFd);
 const int ioUS=IOWithId_Self(ioidUS);
 const int ioMS=IOWithId_Self(ioidMS);
 const int ioOS=IOWithId_Self(ioidOS);

  switch (SolveMethod())
    {
    case SM_Direct:
    case SM_Inline:
    case SM_Buffered:

      SigmaQInPMin(QFd, som_ALL, Id_2_Mask(ioidFd) | Id_2_Mask(ioidWW));

      QmWIn=0.0;
      for (int i = 0; i<NoFlwIOs(); i++)
        {
        if (IO_In(i) && (IOId_Self(i)==ioidWW))
          QmWIn += IOConduit(i)->QMass(som_Liq);
        }

      NumS = Max(0.1, NumS);

      //SpConduit QOs_("QOs", this, TOA_Free); //working copy
      //SpConduit QUs_("QUs", this, TOA_Free); //working copy

      switch (nDecks)
        {
        case 0:
          IOConduit(ioUS)->QCopy(QFd);
          break;
        case 1:
          TopDeck.Separate(QFd, *IOConduit(ioOS>=0 ? ioOS : ioMS), *IOConduit(ioUS), 1, NumS);
          //TopDeck.Separate(QFd, QOs_, QUs_, 1, NumS);
          //IOConduit(ioOS>=0 ? ioOS : ioMS)->QZero();
          //IOConduit(ioOS>=0 ? ioOS : ioMS)->QCopy(QOs_);
          //IOConduit(ioUS)->QZero();
          //IOConduit(ioUS)->QCopy(QUs_);
          break;
        case 2:
          TopDeck.Separate(QFd, *IOConduit(ioOS), QMs, 1, NumS);
          BottomDeck.Separate(QMs, *IOConduit(ioMS), *IOConduit(ioUS), 2, NumS);
          //TopDeck.Separate(QFd, QOs_, QMs, 1, NumS);
          //IOConduit(ioOS)->QZero();
          //IOConduit(ioOS)->QCopy(QOs_);
          //BottomDeck.Separate(QMs, QOs_, QUs_, 2, NumS);
          //IOConduit(ioMS)->QZero();
          //IOConduit(ioMS)->QCopy(QOs_);
          //IOConduit(ioUS)->QZero();
          //IOConduit(ioUS)->QCopy(QUs_);
          break;
        }
      break;
    }



  }

//--------------------------------------------------------------------------

dword Screen::ModelStatus()
  {
  dword Status=MdlNode::ModelStatus();
  if (NoFlwIOs())
    {
    int HasFlw=0;
    for (int i=0; i<NoFlwIOs(); i++)
      {
      if (IOConduit(i)->QMass()>DisplayZeroFlow)
        HasFlw=1;
      }
    Status |= (HasFlw ? FNS_UFlw : FNS_UNoFlw);
    }
  return Status;
  }

//===========================================================================
//===========================================================================
