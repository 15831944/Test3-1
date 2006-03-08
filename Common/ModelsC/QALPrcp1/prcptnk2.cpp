//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#include "stdafx.h"
#include "sc_defs.h"
#include "flwnode.h"
#define  __PRCPTNK2_CPP
#include "prcptnk2.h"
#include "alspmdlb.h"
//#include "qspmdl.h"
//#include "optoff.h"

typedef SMBayerBase SMBayerB;

//==========================================================================

DATACNV_PRI(DC_TCONST, "dC/s.C", 0.0, 1.0, "Therm Const", true);
DATACNV_SEC(DC_TCONST, "dC/h.C", 3600.0, 0.0, 1.0, 1000, true);

//==========================================================================

const byte ioid_Feed         = 0;
const byte ioid_Product      = 1;

static IOAreaRec QPrecipMain2IOAreaList[] =
  {{"Feed",     "Feed",      ioid_Feed    , LIO_In ,    nc_MLnk, 1, 20,     0, 1.0f},
   {"Overflow", "Overflow",  ioid_Product , LIO_Out,    nc_MLnk, 1,  1,     0, (float)0.5},
   {NULL}}; //This lists the areas of the model where links can be attached.


static double Drw_QPrecipMain2[] = {  DD_Poly, -5,14, -5,-14, 0,-16, 5,-14, 5,14, -5,14,
                                   DD_End }; 
//--------------------------------------------------------------------------

IMPLEMENT_MODELUNIT(QPrecipMain2, "QALPrcp", "2", Drw_QPrecipMain2, "Tank", "PRCP", TOC_ALL|TOC_GRP_ALUMINA|TOC_QALEXTRA,
                    "Process:Unit:QAL Precipitator(2)",
                    "The precipitator models a continuous gibbsite crystallisation "
                    "precipitation tank.")

QPrecipMain2::QPrecipMain2(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MN_Surge(pClass_, TagIn, pAttach, eAttach)
  {
  AttachIOAreas(QPrecipMain2IOAreaList);
  Contents.SetClosed(false);
  bOnLine = 1;
  }

static DDBValueLst DDBDensCorr[]=
  {
    {eDensCorrCRC          ,  "CRC"},
    {eDensCorrQAL1999      ,  "QAL-1999"},
    {0}
  };

static DDBValueLst DDBSolCorr[]=
  {
    {eSolCorrMisra1970    ,  "Misra-1970"},
    {eSolCorrRodda1989    ,  "Rodda-1989"},
    {eSolCorrRodda1990    ,  "Rodda-1990"},
    {eSolCorrRodda1991    ,  "Rodda-1991"},
    {eSolCorrRosenberg1996,  "Rosenberg-1996"},
    {eSolCorrYoung1997    ,  "Young-1997"},
    {eSolCorrQAL1999      ,  "QAL-1999"},
    {0}
  };

static DDBValueLst DDBGrowCorr[]=
  {
    {eGrowCorrWhite1988,  "White-1988"},
    {eGrowCorrCRC1998  ,  "CRC-1998"},
    {eGrowCorrQAL2001  ,  "QAL-2001"},
    //{eGrowCorrQAL2001a ,  "QAL-2001a"},
    {0}
  };

static DDBValueLst DDBNucCorr[]=
  {
    {eNucCorrMisra1970,  "Misra-1970"},
    {0}
  };
static DDBValueLst DDBAgglCorr[]=
  {
    {eAgglCorrIlievski1991,  "Ilievski-1991"},
    {0}
  };
static DDBValueLst DDBSodaCorr[]=
  {
    {eSodaCorrSang1988  ,  "Sang-1988"},
    {eSodaCorrOhkawa1985,  "Ohkawa-1985"},
    {eSodaCorrQAL1988   ,  "QAL-1988"},
    {eSodaCorrCRC1993   ,  "CRC-1993"},
    {eSodaCorrQAL1999   ,  "QAL-1999"},
    {0}
  };

const word idmDoTestBtn     = 1000;

#ifdef _RELEASE
void TgCmt(DataDefnBlk & DDB, char* Comment) {};
#else
void TgCmt(DataDefnBlk & DDB, char* Comment) { DDB.TagComment(Comment); };
#endif


void QPrecipMain2::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);

  DDB.Visibility();
  DDB.Text("");
  DDB.CheckBoxBtn("OnLine",    "", DC_,  "",   &bOnLine,       this, isParm, DDBYesNo);
  #ifndef _RELEASE
  DDB.CheckBoxBtn("TransferOnly", "", DC_, "",  &PSD.m_bXFerOnly, this, isParm|noFile|noSnap|noScenario, DDBYesNo);
  #endif

  DDB.Visibility();
  BuildDataDefnShowIOs(DDB);

  DDB.Page("Config", DDB_RqdPage);
  if (DDB.BeginStruct(this, "Corrs"))
    {
    DDB.Long  ("Density",         "", DC_,     "",      &PSD.m_Corr.m_DensCorr, this, isParm|SetOnChange, DDBDensCorr);
    DDB.Long  ("Solubility",      "", DC_,     "",      &PSD.m_Corr.m_SolCorr,  this, isParm|SetOnChange, DDBSolCorr);
    DDB.Long  ("Growth",          "", DC_,     "",      &PSD.m_Corr.m_GrowCorr, this, isParm|SetOnChange, DDBGrowCorr);
    DDB.Long  ("Nucleation",      "", DC_,     "",      &PSD.m_Corr.m_NucCorr,  this, isParm|SetOnChange, DDBNucCorr);
    DDB.Long  ("Agglomeration",   "", DC_,     "",      &PSD.m_Corr.m_AgglCorr, this, isParm|SetOnChange, DDBAgglCorr);
    DDB.Long  ("Soda",            "", DC_,     "",      &PSD.m_Corr.m_SodaCorr, this, isParm|SetOnChange, DDBSodaCorr);
    }                                             
  DDB.EndStruct();                                

  DDB.Visibility();
  
  if (DDB.BeginStruct(this, "Factors"))           
    {                                             
    DDB.Double("OverallRate",     "", DC_,     "",      &PSD.m_GlobalVars.m_OverallRF,          this, isParm);
    DDB.Double("GrowthRate",      "", DC_,     "",      &PSD.m_GlobalVars.m_GrowthRF,           this, isParm);
    DDB.Double("NucleationRate",  "", DC_,     "",      &PSD.m_GlobalVars.m_NucleationRF,       this, isParm);
    DDB.Double("AgglomRate",      "", DC_,     "",      &PSD.m_GlobalVars.m_AgglomRF,           this, isParm);
    DDB.Text("Parameters");
    DDB.Double("GrowthActivationEnergy", "", DC_T, "K", &PSD.m_GlobalVars.m_G_ActivationEnergy, this, isParm);
    DDB.Double("GrowthK0",        "", DC_,     "",      &PSD.m_GlobalVars.m_G_K0,               this, isParm);
    DDB.Double("BoundSodaK1",     "", DC_,     "",      &PSD.m_GlobalVars.m_K1_BoundSoda,       this, isParm);
    DDB.Double("AgglomLinDiam",   "", DC_,     "",      &PSD.m_GlobalVars.m_AggKernalF[0],      this, isParm);
    DDB.Double("AgglowQuadDiam",  "", DC_,     "",      &PSD.m_GlobalVars.m_AggKernalF[1],      this, isParm);
    DDB.Double("AgglomMaxDiam",   "", DC_L,  "um",      &PSD.m_GlobalVars.m_AggKernalF[2],      this, isParm);
    }                                 
  DDB.EndStruct();                    
                                      
  DDB.Double  ("EquilTime",        "",DC_Time, "h",     &PSD.m_EquilTime,                       this, isParm);
  DDB.Double  ("TankVol",          "",DC_Vol,  "kL",     &PSD.m_GlobalVars.m_TankVol,           this, isParm);
  DDB.Double  ("ThermalConst",     "",DC_TCONST, "dC/h.C",  &PSD.m_DerivsPars.m_dThermalConst,  this,	isParm);
  DDB.Double  ("AmbientTemp",      "",DC_T,    "C",       &PSD.m_DerivsPars.m_dAmbientTemp,     this,	isParm);
  DDB.Bool    ("Add_ReactionHeat", "",DC_,     "",        &PSD.m_DerivsPars.m_bWithHOR,         this, isParm, DDBYesNo);
  DDB.Text("Results");
  DDB.Double  ("ACin",              "",         DC_,     "",       &PSD.m_dFdAC,                this, isResult);//|noFile|noSnap|noScenario);
  DDB.Double  ("ACeq",              "",         DC_,     "",       &PSD.m_dACeq,                this, isResult);//|noFile|noSnap|noScenario);
  DDB.Double  ("ACout",             "",         DC_,     "",       &PSD.m_dCnAC,                this, isResult);//|noFile|noSnap|noScenario);
  DDB.Double  ("IonicStrength",     "",         DC_,     "",       &PSD.m_dIonicStrength,       this, isResult|noFile|noSnap|noScenario|InitHidden);
  DDB.Double  ("Aeq",               "",         DC_Conc, "kg/m^3", &PSD.m_dAeq,                 this, isResult|noFile|noSnap|noScenario|InitHidden);
  DDB.Double  ("Yield",             "",         DC_Conc, "kg/m^3", &PSD.m_dYield,               this, isResult);//|noFile|noSnap|noScenario);
  //DDB.Double  ("TankSlurryDensity", "TankRho",  DC_Rho,  "kg/m^3", &PSD.rhos,                   this, isResult|noFile|noSnap|noScenario);
  DDB.Double  ("TankSlurryMass",    "TankMass", DC_M,    "kg",     &PSD.m_DerivsPars.m_Mass,    this, isResult);//|noFile|noSnap|noScenario); //mass of slurry in tank
  DDB.Double  ("ResidenceTime",     "ResTime",  DC_Time, "h",      &PSD.m_dResTime,             this, isResult);//|noFile|noSnap|noScenario); //Residence time
  DDB.Double  ("GrowthRate",        "G",        DC_,     "",       &PSD.m_dGrowthRate,          this, isResult);//|noFile|noSnap|noScenario); //growth rate in microns/hr
  DDB.Double  ("NucleationRate",    "Nuc",      DC_,     "",       &PSD.m_DerivsPars.m_Nuc,     this, isResult);//|noFile|noSnap|noScenario); //nucleation rate
  DDB.Double  ("AgglomRate",        "Agg",      DC_,     "",       &PSD.m_DerivsPars.m_Agg,     this, isResult);//|noFile|noSnap|noScenario); //agglomeration rate
  DDB.Double  ("SodaInclusion",     "SodaR",    DC_,     "",       &PSD.m_DerivsPars.m_SodaR,   this, isResult);//|noFile|noSnap|noScenario); //Soda Inclusion
  DDB.Double  ("",                  "SodaC",    DC_Frac, "%",      &PSD.m_DerivsPars.m_SodaC,   this, isResult);//|noFile|noSnap|noScenario); //Soda Inclusion
  //DDB.Visibility(SHM_All, PSD.m_Corr.m_GrowCorr==eGrowCorrQAL2001a);
  DDB.Visibility(SHM_All, PSD.m_Corr.m_GrowCorr==eGrowCorrQAL2001);
  DDB.Double  ("Sol",               "",         DC_Conc,  "g/L",       &PSD.m_DerivsPars.m_SolConc, this, isResult);
  DDB.Double  ("SSA",               "",         DC_SurfAreaM, "m^2/g", &PSD.m_DerivsPars.m_SSA, this, isResult);

  DDB.Double("ThermalLoss",      "",DC_Pwr,    "kW",      &PSD.m_DerivsPars.m_dThermalLoss,     this, 0);
  DDB.Double("ReactionHeat",     "",DC_Pwr,    "kW",      &PSD.m_DerivsPars.m_dReactionHeat,    this, 0);
  DDB.Text("");
//  RB.Add_OnOff(DDB, False);
  
  DDB.Page("Fd", DDB_RqdPage);
  DDB.CheckBoxBtn("InputOverride",    "", DC_,     "",      &PSD.m_bUseFdConcOvr,               this, isParm, DDBYesNo);
  //DDB.Visibility(SHM_All, PSD.m_bUseFdConcOvr);
  if (PSD.m_bUseFdConcOvr || !DDB.ForView())
    {
    if (DDB.BeginStruct(this, "InputOvr"))
      {
      //DDB.Double("Temperature",    "", DC_T,    "C",     &PSD.m_FdConcOvr.m_Temp,   this, isParm|NAN_OK);
      DDB.Double("BndSoda",        "", DC_Frac, "%",     &PSD.m_FdConcOvr.m_BndSoda,            this, isParm|NAN_OK);
      DDB.Double("A/C",            "", DC_,     "",      &PSD.m_FdConcOvr.acs,                  this, isParm|NAN_OK);
      DDB.Double("Caustic",        "", DC_Conc, "g/L",   &PSD.m_FdConcOvr.cs,                   this, isParm|NAN_OK);
      DDB.Double("SolidsConc",     "", DC_Conc, "g/L",   &PSD.m_FdConcOvr.gpl,                  this, isParm|NAN_OK);
      DDB.Double("NaCl",           "", DC_Conc, "g/L",   &PSD.m_FdConcOvr.cl,                   this, isParm|NAN_OK);
      DDB.Double("NaF",            "", DC_Conc, "g/L",   &PSD.m_FdConcOvr.f,                    this, isParm|NAN_OK);
      DDB.Double("Na2CO3",         "", DC_Conc, "g/L",   &PSD.m_FdConcOvr.co3,                  this, isParm|NAN_OK);
      DDB.Double("Na2SO4",         "", DC_Conc, "g/L",   &PSD.m_FdConcOvr.so4,                  this, isParm|NAN_OK);
      DDB.Double("SiO2",           "", DC_Conc, "g/L",   &PSD.m_FdConcOvr.si,                   this, isParm|NAN_OK);
      DDB.Double("Fe2O3",          "", DC_Conc, "g/L",   &PSD.m_FdConcOvr.fe,                   this, isParm|NAN_OK);
      DDB.Double("Na2C2O4",        "", DC_Conc, "g/L",   &PSD.m_FdConcOvr.ox,                   this, isParm|NAN_OK);
      DDB.Double("TOC",            "", DC_Conc, "g/L",   &PSD.m_FdConcOvr.toc,                  this, isParm|NAN_OK);
      }                                 
    DDB.EndStruct();       
    }
  DDB.Visibility();
  DDB.Text("");
  if (DDB.BeginStruct(this, "Input"))
    {
    //DDB.Double("Temperature",    "",          DC_T,    "C",      &PSD.m_FdConc.m_Temp,   this, isResult);
    DDB.Double("A/C",            "",          DC_,     "",       &PSD.m_FdConc.acs,             this, isResult); TgCmt(DDB, "acs");
    DDB.Double("CausticConc",    "Caustic",   DC_Conc, "g/L",    &PSD.m_FdConc.cs,              this, isResult); TgCmt(DDB, "cs");
    DDB.Double("SolidsConc",     "Solids",    DC_Conc, "g/L",    &PSD.m_FdConc.gpl,             this, isResult); TgCmt(DDB, "gpl");
    DDB.Double("NaClConc",       "NaCl",      DC_Conc, "g/L",    &PSD.m_FdConc.cl,              this, isResult); TgCmt(DDB, "cl");
    DDB.Double("NaFConc",        "NaF",       DC_Conc, "g/L",    &PSD.m_FdConc.f,               this, isResult); TgCmt(DDB, "f");
    DDB.Double("Na2CO3Conc",     "Na2CO3",    DC_Conc, "g/L",    &PSD.m_FdConc.co3,             this, isResult); TgCmt(DDB, "co3");
    DDB.Double("Na2SO4Conc",     "Na2SO4",    DC_Conc, "g/L",    &PSD.m_FdConc.so4,             this, isResult); TgCmt(DDB, "so4");
    DDB.Double("SiO2Conc",       "SiO2",      DC_Conc, "g/L",    &PSD.m_FdConc.si,              this, isResult); TgCmt(DDB, "si");
    DDB.Double("Fe2O3Conc",      "Fe2O3",     DC_Conc, "g/L",    &PSD.m_FdConc.fe,              this, isResult); TgCmt(DDB, "fe");
    DDB.Double("Na2C2O4Conc",    "Na2C2O4",   DC_Conc, "g/L",    &PSD.m_FdConc.ox,              this, isResult); TgCmt(DDB, "ox");
    DDB.Double("TOCConc",        "TOC",       DC_Conc, "g/L",    &PSD.m_FdConc.toc,             this, isResult); TgCmt(DDB, "toc");
    DDB.Text("");                                                                               
    DDB.Double("Al2O3Conc",      "A",         DC_Conc, "g/L",    &PSD.m_FdConc.a,               this, isResult); TgCmt(DDB, "a");
    DDB.Double("Na2OConc",       "Na2O",      DC_Conc, "g/L",    &PSD.m_FdConc.c,               this, isResult); TgCmt(DDB, "c");
    DDB.Double("BndSoda",        "",          DC_Frac, "%",      &PSD.m_FdConc.m_BndSoda,       this, isResult); TgCmt(DDB, "m_BndSoda");
    DDB.Double("OrgSoda",        "",          DC_Frac, "%",      &PSD.m_FdConc.m_BndSodaAsOrg,  this, isResult); TgCmt(DDB, "m_BndSodaAsOrg");
    DDB.Double("L10",            "",          DC_,     "",       &PSD.m_FdConc.m_L10,           this, isResult);
    DDB.Double("L21",            "",          DC_,     "",       &PSD.m_FdConc.m_L21,           this, isResult);
    DDB.Double("L32",            "",          DC_,     "",       &PSD.m_FdConc.m_L32,           this, isResult);
    DDB.Double("Density",        "Rho",       DC_Rho,  "kg/m^3", &PSD.m_FdConc.m_Dens,          this, isResult);
    DDB.Double("SeedSurfAreaM",  "SAM",       DC_SurfAreaM, "m^2/g",&PSD.m_FdConc.m_SAM,        this, isResult);
    }                                 
  DDB.EndStruct();       
  DDB.Text("");

  DDB.Page("Cn", DDB_RqdPage);
  DDB.CheckBoxBtn("ContentOverride",    "", DC_,  "",  &PSD.m_bUseCnConcOvr,       this, isParm, DDBYesNo);
  //DDB.Visibility(SHM_All, PSD.m_bUseCnConcOvr);
  if (PSD.m_bUseCnConcOvr || !DDB.ForView())
    {
    if (DDB.BeginStruct(this, "ContOvr"))
      {
      //DDB.Double("Temperature",    "", DC_T,    "C",     &PSD.m_CnConcOvr.m_Temp,   this, isParm|NAN_OK);
      DDB.Double("BndSoda",        "", DC_Frac, "%",     &PSD.m_CnConcOvr.m_BndSoda,            this, isParm|NAN_OK);
      DDB.Double("A/C",            "", DC_,     "",      &PSD.m_CnConcOvr.acs,                  this, isParm|NAN_OK);
      DDB.Double("Caustic",        "", DC_Conc, "g/L",   &PSD.m_CnConcOvr.cs,                   this, isParm|NAN_OK);
      DDB.Double("SolidsConc",     "", DC_Conc, "g/L",   &PSD.m_CnConcOvr.gpl,                  this, isParm|NAN_OK);
      DDB.Double("NaCl",           "", DC_Conc, "g/L",   &PSD.m_CnConcOvr.cl,                   this, isParm|NAN_OK);
      DDB.Double("NaF",            "", DC_Conc, "g/L",   &PSD.m_CnConcOvr.f,                    this, isParm|NAN_OK);
      DDB.Double("Na2CO3",         "", DC_Conc, "g/L",   &PSD.m_CnConcOvr.co3,                  this, isParm|NAN_OK);
      DDB.Double("Na2SO4",         "", DC_Conc, "g/L",   &PSD.m_CnConcOvr.so4,                  this, isParm|NAN_OK);
      DDB.Double("SiO2",           "", DC_Conc, "g/L",   &PSD.m_CnConcOvr.si,                   this, isParm|NAN_OK);
      DDB.Double("Fe2O3",          "", DC_Conc, "g/L",   &PSD.m_CnConcOvr.fe,                   this, isParm|NAN_OK);
      DDB.Double("Na2C2O4",        "", DC_Conc, "g/L",   &PSD.m_CnConcOvr.ox,                   this, isParm|NAN_OK);
      DDB.Double("TOC",            "", DC_Conc, "g/L",   &PSD.m_CnConcOvr.toc,                  this, isParm|NAN_OK);
      }                                 
    DDB.EndStruct();       
    }
  DDB.Visibility();
  DDB.Text("");
  if (DDB.BeginStruct(this, "Cont"))
    {
    //DDB.Double("Slurry",         "", DC_,     "",      &PSD.CnVolSlurry,       this, isResult);
    //DDB.Double("Temperature",    "",          DC_T,    "C",      &PSD.m_CnConc.m_Temp,   this, isResult);
    DDB.Double("A/C",            "",          DC_,     "",       &PSD.m_CnConc.acs,             this, isResult); TgCmt(DDB, "acs");
    DDB.Double("CausticConc",    "Caustic",   DC_Conc, "g/L",    &PSD.m_CnConc.cs,              this, isResult); TgCmt(DDB, "cs");
    DDB.Double("SolidsConc",     "Solids",    DC_Conc, "g/L",    &PSD.m_CnConc.gpl,             this, isResult); TgCmt(DDB, "gpl");
    DDB.Double("NaClConc",       "NaCl",      DC_Conc, "g/L",    &PSD.m_CnConc.cl,              this, isResult); TgCmt(DDB, "cl");
    DDB.Double("NaFConc",        "NaF",       DC_Conc, "g/L",    &PSD.m_CnConc.f,               this, isResult); TgCmt(DDB, "f");
    DDB.Double("Na2CO3Conc",     "Na2CO3",    DC_Conc, "g/L",    &PSD.m_CnConc.co3,             this, isResult); TgCmt(DDB, "co3");
    DDB.Double("Na2SO4Conc",     "Na2SO4",    DC_Conc, "g/L",    &PSD.m_CnConc.so4,             this, isResult); TgCmt(DDB, "so4");
    DDB.Double("SiO2Conc",       "SiO2",      DC_Conc, "g/L",    &PSD.m_CnConc.si,              this, isResult); TgCmt(DDB, "si");
    DDB.Double("Fe2O3Conc",      "Fe2O3",     DC_Conc, "g/L",    &PSD.m_CnConc.fe,              this, isResult); TgCmt(DDB, "fe");
    DDB.Double("Na2C2O4Conc",    "Na2C2O4",   DC_Conc, "g/L",    &PSD.m_CnConc.ox,              this, isResult); TgCmt(DDB, "ox");
    DDB.Double("TOCConc",        "TOC",       DC_Conc, "g/L",    &PSD.m_CnConc.toc,             this, isResult); TgCmt(DDB, "toc");
    DDB.Text("");                                                                               
    DDB.Double("Al2O3Conc",      "A",         DC_Conc, "g/L",    &PSD.m_CnConc.a,               this, isResult); TgCmt(DDB, "a");
    DDB.Double("Na2OConc",       "Na2O",      DC_Conc, "g/L",    &PSD.m_CnConc.c,               this, isResult); TgCmt(DDB, "c");
    DDB.Double("BndSoda",        "",          DC_Frac, "%",      &PSD.m_CnConc.m_BndSoda,       this, isResult);
    DDB.Double("OrgSoda",        "",          DC_Frac, "%",      &PSD.m_CnConc.m_BndSodaAsOrg,  this, isResult); TgCmt(DDB, "orgsoda");
    DDB.Double("L10",            "",          DC_,     "",       &PSD.m_CnConc.m_L10,           this, isResult);
    DDB.Double("L21",            "",          DC_,     "",       &PSD.m_CnConc.m_L21,           this, isResult);
    DDB.Double("L32",            "",          DC_,     "",       &PSD.m_CnConc.m_L32,           this, isResult);
    DDB.Double("Density",        "Rho",       DC_Rho,  "kg/m^3", &PSD.m_CnConc.m_Dens,          this, isResult);
    DDB.Double("SeedSurfAreaM",  "SAM",       DC_SurfAreaM, "m^2/g",&PSD.m_CnConc.m_SAM,        this, isResult);
    }                                 
  DDB.EndStruct();       
  DDB.Text("");

  RB.BuildDataDefn(DDB, NULL, NULL, DDB_RqdPage);
  DDB.Object(&Contents, this, NULL, NULL, DDB_RqdPage);
  DDB.EndStruct();
  }

//--------------------------------------------------------------------------

flag QPrecipMain2::DataXchg(DataChangeBlk & DCB)
  {
  if (MN_Surge::DataXchg(DCB)) 
    return 1;
  switch (DCB.lHandle)
    {
    case idmDoTestBtn:
      DCB.B=0;
      return 1;
    }
  return 0;
  }

//---------------------------------------------------------------------------

flag QPrecipMain2::ValidateData(ValidateDataBlk & VDB)
  {
  return MN_Surge::ValidateData(VDB);
  }

//--------------------------------------------------------------------------

void QPrecipMain2::EvalJoinPressures(long JoinMask)
  {
  switch (NetMethod())
    {
    case NM_Probal:
      {
      for (int j=0; j<NJoins(); j++)
        SetPBJoinPressure(j, Std_P, true, true);
      break;
      }
    case NM_Dynamic:
      MdlNode::EvalJoinPressures(JoinMask);
      break;
    }
  };
 
//--------------------------------------------------------------------------

void QPrecipMain2::EvalJoinFlows(int JoinNo)
  {
  switch (NetMethod())
    {
    case NM_Probal:
    case NM_Dynamic:
      break;
    }
  }
 
//--------------------------------------------------------------------------

void QPrecipMain2::EvalProducts(long JoinMask)
  {
  switch (SolveMethod())
    {
    case SM_Direct:
      {
      const int ioProd = IOWithId_Self(ioid_Product);
      ASSERT(ioProd>=0);
      SpConduit & Qp=*IOConduit(ioProd);
      Qp.QZero();
      bool MustCopy=true;
      QFeed().QZero();
      SigmaQInPMin(QFeed(), som_ALL, Id_2_Mask(ioid_Feed));//, &QSMBayerClass);
      if (bOnLine && QFeed().QMass()>UsableMass)
        {
        RB.EvalProducts(QFeed());
        //EHX.EvalProducts(Qp);

        SMBayerB * pBm = dynamic_cast<SMBayerB*>(QFeed().Model());
        SQSzDist1 * pFdSz = SQSzDist1::FindQual(QFeed().Model());
        SetCI(1, pBm==NULL);
        SetCI(2, pFdSz==NULL || !pFdSz->DistributionsExist());
        if (pBm && pFdSz && pFdSz->DistributionsExist())
          {
          SMBayerB & Bm=*pBm;

          //if (0)
          //  Contents.SetTempPress(C2K(66), AtmosPress());

          Contents.SetVolume(PSD.m_GlobalVars.m_TankVol);
          if (Contents.Mass()<1.0e-6*QFeed().QMass())
            Contents.SetF(QFeed(), som_ALL, 1.0, AtmosPress()); //if contents empty, set to "same" as feed
          double L=Contents.Level();
          Contents.ScaleMass(som_SL, 1.0/GTZ(L));

          if (Qp.QMass()<1.0e-6*QFeed().QMass())
            Qp.QSetF(QFeed(), som_ALL, 1.0);
          MustCopy=false;

          PSD.SolveIt(QFeed(), Contents, Qp, true);
          //if (0)
          //  {
          //  double msH=QFeed().msHf();
          //  Contents.Set_msHf(msH);
          //  double t=Contents.Temp();
          //  int xxx=0;
          //  }

          Qp.QSetM(Contents, som_ALL, QFeed().QMass());
          }
        }
      
      if (MustCopy)
        Qp.QSetF(QFeed(), som_ALL, 1.0);
      break;
      }
    default:
      MN_Surge::EvalProducts(JoinMask);
    }
  }
    
//--------------------------------------------------------------------------

void QPrecipMain2::ClosureInfo() 
  { 
  if (NoFlwIOs()>0)
    {
    MN_Surge::ClosureInfo();
    if (m_Closure.DoFlows())
      {
      CClosureInfo &CI=m_Closure[0];
      CI.m_PowerIn+=-PSD.m_DerivsPars.m_dThermalLoss;

      double HOR=PSD.m_DerivsPars.m_bWithHOR ? PSD.m_DerivsPars.m_dReactionHeat : 0.0;
      CI.m_RCTHOR_Used0+=HOR;
      //CI.m_RCTHOR_UsedT+=HOR;
      }
    }
  };

//--------------------------------------------------------------------------

flag QPrecipMain2::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  int xxx=(No-CBContext());
  switch (No-CBContext())
    {
    case 1: pS="W\tBad Feed Stream - Not Bayer Model"; return 1;
    case 2: pS="W\tBad Feed Stream - No valid size data"; return 1;
    default:                                               
      return MN_Surge::CIStrng(No, pS);
    }
  };

//==========================================================================
//
//
//
//==========================================================================
