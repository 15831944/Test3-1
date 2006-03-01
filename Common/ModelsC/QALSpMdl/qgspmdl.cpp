//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#include "stdafx.h"
#define  __QGSPMDL_CPP
#include "sp_db.h"
#include "qgspmdl.h"
//#include "errorlog.h"
//#include "optoff.h"

// ==========================================================================

#define dbgModels 01
#if dbgModels
#include "dbgmngr.h"
static CDbgMngr dbgSpecies ("QALSpMdl", "Species");
#endif

// ==========================================================================
//
//                         QAL Bauxite Specie Model
//
// ==========================================================================

IMPLEMENT_SPMODEL(QSMBauxite, "QBauxite", "", TOC_ALL|TOC_GRP_ALUMINA|TOC_QAL, "Bauxite", "QAL Bauxite Model");
IMPLEMENT_SPARES(QSMBauxite, 100);

// ==========================================================================
//
//                             Specie Access 
//
// ==========================================================================

static MInitialiseTest InitTest(&QSMBauxiteClass); 

SPECIEBLK_L(InitTest, Water,                 "H2O(l)",                                 false);
SPECIEBLK_S(InitTest, Anatase,               "TiO2(s)",                                false);
SPECIEBLK_S(InitTest, Boehmite,              "AlO.OH(s)",                              false);
SPECIEBLK_S(InitTest, Gibbsite,              "Al[OH]3(s)",                             false);
SPECIEBLK_S(InitTest, Heamatite,             "Fe2O3(s)",                               false);
SPECIEBLK_S(InitTest, InertAlumina,          "Al2O3(s)",                               false);
SPECIEBLK_S(InitTest, InertSolids,           "InertSolids(s)",                         false);
SPECIEBLK_S(InitTest, Kaolin,                "2SiO2.Al2O3.2H2O(s)",                    false);
SPECIEBLK_S(InitTest, OrganicCarbon,         "C(s)",                                   false);
SPECIEBLK_S(InitTest, PhosphorousPentoxide,  "P2O5(s)",                                false);
SPECIEBLK_S(InitTest, Quartz,                "SiO2(s)",                                false);

CArray <int, int> QSMBauxite::LclParm;

QSMBauxite::QSMBauxite(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach):
  SpModel(pClass_, Tag_, pAttach, eAttach)
  {
  fDoCalc  = false;
  dA_Moisture  = 0.0;
  dA_TCA       = 0.0;
  dA_TAA       = 0.0;
  dA_THA       = 0.0;
  dA_TotalSiO2 = 0.0;
  dA_Quartz    = 0.0;
  dA_Fe2O3     = 0.0;
  dA_TiO2      = 0.0;
  dA_P2O5      = 0.0;
  dA_TOOC      = 0.0;
  dA_LOI       = 0.0;
  }

// --------------------------------------------------------------------------

QSMBauxite::~QSMBauxite()
  {
  }

// --------------------------------------------------------------------------

DDEF_Flags QSMBauxite::FixSpEditFlags(DDEF_Flags Flags, int iSpid, flag AsParms)
  {
  if (fDoCalc && LclParm.GetSize()!=SDB.Count())
    {
    LclParm.SetSize(SDB.Count());
    for (int i=0; i<SDB.Count(); i++)
      LclParm[i]=false;

    //LclParm[Water.LiqPhInx()] = true;
    }

  return (Flags&~(isParm|isResult)) | ((AsParms && (!fDoCalc || LclParm[iSpid])) ? DDEF_PARAM: DDEF_RESULT); 
  }
// --------------------------------------------------------------------------

void QSMBauxite::BuildDataDefn_Vars(DataDefnBlk & DDB)
  {
  DDB.CheckBoxBtn("DefineSolids", "",  DC_,       "",       &fDoCalc,        this,  isParm);
  DDEF_Flags Old = DDB.GetVisibility();
  DDB.Visibility(SHM_All, fDoCalc);                                                         
  DDB.Double  ("",   "Moisture",  DC_Frac, "%", &dA_Moisture  ,    this, isParm);
  DDB.Double  ("",   "TCA",       DC_Frac, "%", &dA_TCA       ,    this, isParm);
  DDB.Double  ("",   "TAA",       DC_Frac, "%", &dA_TAA       ,    this, isParm);
  DDB.Double  ("",   "THA",       DC_Frac, "%", &dA_THA       ,    this, isParm);
  DDB.Double  ("",   "TotalSiO2", DC_Frac, "%", &dA_TotalSiO2 ,    this, isParm);
  DDB.Double  ("",   "Quartz",    DC_Frac, "%", &dA_Quartz    ,    this, isParm);
  DDB.Double  ("",   "Fe2O3",     DC_Frac, "%", &dA_Fe2O3     ,    this, isParm);
  DDB.Double  ("",   "TiO2",      DC_Frac, "%", &dA_TiO2      ,    this, isParm);
  DDB.Double  ("",   "P2O5",      DC_Frac, "%", &dA_P2O5      ,    this, isParm);
  DDB.Double  ("",   "TOOC",      DC_Frac, "%", &dA_TOOC      ,    this, isParm);
  DDB.Double  ("",   "LOI",       DC_Frac, "%", &dA_LOI       ,    this, isParm);
  DDB.SetVisibility(Old);
  DDB.Text("");

  SpModel::BuildDataDefn_Vars(DDB);
  }

// --------------------------------------------------------------------------

flag QSMBauxite::ValidateData(ValidateDataBlk & VDB)
  {
  // Calculate the makeup given the Analysis
  // Assumptions:
  //  No Goethite present.
  //  Total Silica = Kaolin + Quartz.
  //  Fe2O3 and TiO2 represents Heamatite and Anatase.
  flag OK=SpModel::ValidateData(VDB);

  if (UnValidatedData() && fDoCalc)
      /*(
      dA_Moisture  > 1.0e-6 ||
      dA_TCA       > 1.0e-6 ||
      dA_TAA       > 1.0e-6 ||
      dA_THA       > 1.0e-6 ||
      dA_TotalSiO2 > 1.0e-6 ||
      dA_Quartz    > 1.0e-6 ||
      dA_Fe2O3     > 1.0e-6 ||
      dA_TiO2      > 1.0e-6 ||
      dA_P2O5      > 1.0e-6 ||
      dA_TOOC      > 1.0e-6 ||
      dA_LOI       > 1.0e-6
      )
      )*/
    {
    dA_Moisture = Range(0.0, dA_Moisture, 1.0);
    //dA_TCA = Range(0.0, dA_TCA, 1.0);
    //etc
    double InertAluminaFrac = (dA_TCA - dA_TAA - (dA_TotalSiO2-dA_Quartz)*InertAlumina.MW/(2.*Quartz.MW))/
                                      GTZ(dA_TCA);
    double WaterFrac     = dA_Moisture;
    double SolidsFrac    = 1.0 - WaterFrac;
    double LOICorrected  = dA_LOI - 2.0 * dA_TOOC;
    double KaolinFrac    = (dA_TotalSiO2 - dA_Quartz) * Kaolin.MW / Quartz.MW / 2.0;
    double GibbsiteFrac  = LOICorrected * (2.0 * Gibbsite.MW) / (2.0 * Water.MW)
                         - dA_TCA * (2.0 * Gibbsite.MW) / (2.0 * InertAlumina.MW)
                         - KaolinFrac * (2.0 * Gibbsite.MW) / (2.0 * Kaolin.MW)
                         + InertAluminaFrac * dA_TCA * (2.0 * Gibbsite.MW) / (2.0 * InertAlumina.MW);
    double BoehmiteFrac  = - 0.5  *  LOICorrected * (2.0 * Boehmite.MW) / Water.MW
                           + 1.5  *  dA_TCA * (2.0 * Boehmite.MW) / InertAlumina.MW
                           - 0.5  *  KaolinFrac * (2.0 * Boehmite.MW) / Kaolin.MW
                           - 1.5  *  InertAluminaFrac * dA_TCA * (2.0 * Boehmite.MW) / InertAlumina.MW;

    double OrganicCarbonFr        = Max(0.0, SolidsFrac * dA_TOOC);
    double KaolinFr               = Max(0.0, SolidsFrac * KaolinFrac);
    double QuartzFr               = Max(0.0, SolidsFrac * dA_Quartz);
    double HeamatiteFr            = Max(0.0, SolidsFrac * dA_Fe2O3);
    double AnataseFr              = Max(0.0, SolidsFrac * dA_TiO2);
    double GibbsiteFr             = Max(0.0, SolidsFrac * GibbsiteFrac);
    double BoehmiteFr             = Max(0.0, SolidsFrac * BoehmiteFrac);
    double PhosphorousPentoxideFr = Max(0.0, SolidsFrac * dA_P2O5);
    double InertAluminaFr         = Max(0.0, SolidsFrac * dA_TCA * InertAluminaFrac);
    // InertSolids make up the remainder
    double InertSolidsFr          = SolidsFrac - (OrganicCarbonFr + KaolinFr + QuartzFr +
                                                  HeamatiteFr + AnataseFr + GibbsiteFr +
                                                  BoehmiteFr + PhosphorousPentoxideFr +
                                                  InertAluminaFr);
    SetCI(1, InertSolidsFr<0.0);
    InertSolidsFr = Max(0.0, InertSolidsFr);

    // Get Original Temperature & Mass
    double T1=Temp();
    double M1=Mass();
    if (M1<1.0e-6)
      M1=1.0;

    ClrStatesOK();
    // Set New Masses
    m_M.Zero();
    m_M.VValue[Water.LiqPhInx()]                = WaterFrac * M1;
    m_M.VValue[OrganicCarbon.SolPhInx()]        = OrganicCarbonFr * M1;
    m_M.VValue[Kaolin.SolPhInx()]               = KaolinFr * M1;
    m_M.VValue[Quartz.SolPhInx()]               = QuartzFr * M1;
    m_M.VValue[Heamatite.SolPhInx()]            = HeamatiteFr * M1;
    m_M.VValue[Anatase.SolPhInx()]              = AnataseFr * M1;
    m_M.VValue[Gibbsite.SolPhInx()]             = GibbsiteFr * M1;
    m_M.VValue[Boehmite.SolPhInx()]             = BoehmiteFr * M1;
    m_M.VValue[PhosphorousPentoxide.SolPhInx()] = PhosphorousPentoxideFr * M1;
    m_M.VValue[InertAlumina.SolPhInx()]         = InertAluminaFr * M1;
    m_M.VValue[InertSolids.SolPhInx()]          = InertSolidsFr * M1;
    // Set Temperature
    SetTemp(T1);
    }

  return OK;
  }

//--------------------------------------------------------------------------

flag QSMBauxite::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case 1: pS="W\tCannot meet requirements specified for Bauxite makeup"; return 1;
    default:                                               
      return SpModel::CIStrng(No, pS);
    }
  }

// ==========================================================================
//
//                        QAL Limestone Specie Model
//
// ==========================================================================
// This model is primarily used for Display Manipulation

IMPLEMENT_SPMODEL(QSMLime, "QLime", "", TOC_ALL|TOC_GRP_ALUMINA|TOC_QAL, "Limestone", "QAL Limestone Model");
IMPLEMENT_SPARES(QSMLime, 100);

QSMLime::QSMLime(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach):
  SpModel(pClass_, Tag_, pAttach, eAttach)
  {
  }

// --------------------------------------------------------------------------

QSMLime::~QSMLime()
  {
  }

// ==========================================================================


