//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992
 
#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdio.h>

#include "sc_defs.h"
#define  __AREA_CPP
#include "area.h"
#include "flshtrn.h"
//#include "optoff.h"

//==========================================================================
//
//
//
//==========================================================================

static IOAreaRec CAreaIOAreaList[] =
  {
    {"SpillIO",   "SpillIO",  IOId_AreaSpillI,  LIO_In,     nc_MLnk, 0, MaxIOList, IOPipeEntry, 0, {0,0,0,0}, NULL, &Xfer2AreaGroup},
    {"VentIO",    "VentIO",   IOId_AreaVentI,   LIO_In,     nc_MLnk, 0, MaxIOList, IOPipeEntry, 0, {0,0,0,0}, NULL, &Xfer2AreaGroup},
    {"LeakIO",    "LeakIO",   IOId_AreaLeakI,   LIO_InOut,  nc_MLnk, 0, MaxIOList, IOPipeEntry, 0, {0,0,0,0}, NULL, &NullFlwGroup},
    {"MakeupIO",  "MakeupIO", IOId_AreaMakeupO, LIO_Out,    nc_MLnk, 0, MaxIOList, IOPipeEntry, 0, {0,0,0,0}, NULL, &NullFlwGroup},
    {"BleedIO",   "BleedIO",  IOId_AreaBleedI,  LIO_In,     nc_MLnk, 0, MaxIOList, IOPipeEntry, 0, {0,0,0,0}, NULL, &NullFlwGroup},
    {NULL}
  };

static double Drw_CArea[] = 
  { 
  DD_Poly, 6,6, 9,4, 7,2, 11,0, 9,-2, 10,-4, 8,-6,
           -8,-6, -11,-4, -9,-2, -8,0, -10,2, -11,4, -8,6,
           6,6, DD_End 
  };

//--------------------------------------------------------------------------

IMPLEMENT_MODELUNIT(CArea, "Area", "1", Drw_CArea, "Area", "Area", TOC_ALL|TOC_GRP_GENERAL|TOC_STD_KENWALT,
                    "Process:Unit:Area", 
                    "Simple Source and/or Sink of Material as Leaks") 

CArea::CArea(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MN_Area(pClass_, TagIn, pAttach, eAttach)
  {
  AttachIOAreas(CAreaIOAreaList);
  m_Content.SetView(SVV_AsMassFrac);  
  //m_SrcsI.SetView(SVV_AsMassFlow);
  //m_SnksO.SetView(SVV_AsMassFlow);
  //m_MakeupsI.SetView(SVV_AsMassFlow);
  //m_BleedsO.SetView(SVV_AsMassFlow);
  //m_SpillsO.SetView(SVV_AsMassFlow);
  //m_VentsO.SetView(SVV_AsMassFlow);
  //m_LeaksI.SetView(SVV_AsMassFlow);
  //m_OvResults.m_Sum[AR_LeaksO].SetView(SVV_AsMassFlow);
  //m_AccI.SetView(SVV_AsMassFlow);
  //m_AccO.SetView(SVV_AsMassFlow);
  }

//--------------------------------------------------------------------------

void CArea::PrepareDefinedData()
  {
#if WITHSPECSHEETS
  if (sSpecSheet.XStrCmp("-")!=0)
    sSpecSheet= Contents.Model()->ShortDesc();
#endif
  //Contents.SetView(SVV_AsMassFrac);
  //Flows.SetView(SVV_AsMassFlow);

  }

//--------------------------------------------------------------------------

void CArea::BuildDataDefn(DataDefnBlk & DDB)
  {
  MN_Area::BuildDataDefn(DDB, BDDAO_Flows|BDDAO_Totals/*|BDDAO_Contents*/|BDDAO_Copy);

  //Strng S;
  //DDB.BeginStruct(this, NULL, NULL, DDB_NoPage);

  //DDB.Text  ("");
  //DDB.CheckBoxBtn("AuditOn",   "",    DC_,    "",  &m_bDoAudit,       this, isParm);
  //DDB.Text  ("");

  //DDB.Double ("Temperature",  "T",    DC_T,    "C",      xidTemp,       this, isResult|noFile|noSnap);
  //DDB.Double ("Density",      "Rho",  DC_Rho,  "kg/m^3", xidRho,        this, isResult|noFile|noSnap);
  //DDB.Double ("Pressure",     "P",    DC_P,    "kPag",   xidPMean,      this, isResult|noFile|noSnap);

  //DDB.Visibility(NSHM_All);
  //DDB.Text  ("");
  //DDB.Double("P_Rqd",         "",    DC_P   , "kPag",   xidEnvPRqd,     this, isParm|NAN_OK);

  //DDB.Visibility(NSHM_All);
  //DDB.Double("T_Rqd",         "",    DC_T   , "C",      xidEnvTRqd,     this, isParm|NAN_OK);

  //BuildDataDefnElevation(DDB);

  ////CDirectFlwIO::sm_
  //if (m_bDoAudit && gs_EvalAudits)
  //  {
  //  if (DDB.BeginStruct(this, "Flows", NULL, DDB_RqdPage))
  //    {
  //    if (DDB.BeginStruct(this, "Mass", NULL, DDB_NoPage))
  //      {
  //      DDB.Double ("",   "Spills",       DC_Qm,   "kg/s",  xidSpillsInQm,   this, isResult|noFile|noSnap); 
  //      DDB.Double ("",   "Vents",        DC_Qm,   "kg/s",  xidVentsInQm,    this, isResult|noFile|noSnap); 
  //      DDB.Double ("",   "LeaksIn",      DC_Qm,   "kg/s",  xidLeaksInQm,    this, isResult|noFile|noSnap); 
  //      DDB.Double ("",   "LeaksOut",     DC_Qm,   "kg/s",  xidLeaksOutQm,   this, isResult|noFile|noSnap); 
  //      DDB.Double ("",   "Srcs",         DC_Qm,   "kg/s",  xidSrcsInQm,     this, isResult|noFile|noSnap); 
  //      DDB.Double ("",   "Snks",         DC_Qm,   "kg/s",  xidSnksInQm,     this, isResult|noFile|noSnap); 
  //      DDB.Double ("",   "Makeups",      DC_Qm,   "kg/s",  xidMakeupsInQm,  this, isResult|noFile|noSnap); 
  //      DDB.Double ("",   "Bleeds",       DC_Qm,   "kg/s",  xidBleedsInQm,   this, isResult|noFile|noSnap); 
  //      DDB.Double ("",   "Accumulation", DC_Qm,   "kg/s",  xidAccumsInQm,   this, isResult|noFile|noSnap); 
  //      DDB.Double ("",   "Depletion",    DC_Qm,   "kg/s",  xidAccumsOutQm,  this, isResult|noFile|noSnap); 
  //      DDB.Double ("",   "Balance",      DC_Qm,   "kg/s",  xidNettInQm,     this, isResult|noFile|noSnap); 
  //      DDB.Double ("",   "Balance.Rel",  DC_Frac, "%",     xidNettInQmRel,  this, isResult|noFile|noSnap); 
  //      }
  //    DDB.EndStruct();

  //    DDB.Text(" ");
  //    if (DDB.BeginStruct(this, "Heat", NULL, DDB_NoPage))
  //      {
  //      DDB.Double ("",   "Spills",       DC_Pwr,   "kJ/s", xidSpillsInHz,    this, isResult|noFile|noSnap);
  //      DDB.Double ("",   "Vents",        DC_Pwr,   "kJ/s", xidVentsInHz,     this, isResult|noFile|noSnap);
  //      DDB.Double ("",   "LeaksIn",      DC_Pwr,   "kJ/s", xidLeaksInHz,     this, isResult|noFile|noSnap);
  //      DDB.Double ("",   "LeaksOut",     DC_Pwr,   "kJ/s", xidLeaksOutHz,    this, isResult|noFile|noSnap);
  //      DDB.Double ("",   "Srcs",         DC_Pwr,   "kJ/s", xidSrcsInHz,      this, isResult|noFile|noSnap);
  //      DDB.Double ("",   "Snks",         DC_Pwr,   "kJ/s", xidSnksInHz,      this, isResult|noFile|noSnap);
  //      DDB.Double ("",   "Makeups",      DC_Pwr,   "kJ/s", xidMakeupsInHz,   this, isResult|noFile|noSnap); 
  //      DDB.Double ("",   "Bleeds",       DC_Pwr,   "kJ/s", xidBleedsInHz,    this, isResult|noFile|noSnap); 
  //      DDB.Double ("",   "Accumulation", DC_Pwr,   "kJ/s", xidAccumsInHz,    this, isResult|noFile|noSnap);
  //      DDB.Double ("",   "Depletion",    DC_Pwr,   "kJ/s", xidAccumsOutHz,   this, isResult|noFile|noSnap);
  //      DDB.Double ("",   "Balance",      DC_Pwr,   "kJ/s", xidNettInHz,      this, isResult|noFile|noSnap);
  //      DDB.Double ("",   "Balance.Rel",  DC_Frac,  "%",    xidNettInHzRel,   this, isResult|noFile|noSnap);
  //      }
  //    DDB.EndStruct();
  //    }
  //  DDB.EndStruct();

  //  DDB.Text(" ");
  //  if (DDB.BeginStruct(this, "Totals", NULL, DDB_RqdPage))
  //    {
  //    if (DDB.BeginStruct(this, "Mass", NULL, DDB_NoPage))
  //      {
  //      DDB.Double ("",   "Spills",       DC_M,     "kg", xidSpillsInQmSum,   this, isResult|noFile|noSnap); 
  //      DDB.Double ("",   "Vents",        DC_M,     "kg", xidVentsInQmSum,    this, isResult|noFile|noSnap); 
  //      DDB.Double ("",   "LeaksIn",      DC_M,     "kg", xidLeaksInQmSum,    this, isResult|noFile|noSnap); 
  //      DDB.Double ("",   "LeaksOut",     DC_M,     "kg", xidLeaksOutQmSum,   this, isResult|noFile|noSnap); 
  //      DDB.Double ("",   "Srcs",         DC_M,     "kg", xidSrcsInQmSum,     this, isResult|noFile|noSnap); 
  //      DDB.Double ("",   "Snks",         DC_M,     "kg", xidSnksInQmSum,     this, isResult|noFile|noSnap); 
  //      DDB.Double ("",   "Makeups",      DC_M,     "kg", xidMakeupsInQmSum,  this, isResult|noFile|noSnap); 
  //      DDB.Double ("",   "Bleeds",       DC_M,     "kg", xidBleedsInQmSum,   this, isResult|noFile|noSnap); 
  //      DDB.Double ("",   "Accumulation", DC_M,     "kg", xidAccumsInQmSum,   this, isResult|noFile|noSnap); 
  //      DDB.Double ("",   "Depletion",    DC_M,     "kg", xidAccumsOutQmSum,  this, isResult|noFile|noSnap); 
  //      DDB.Double ("",   "Balance",      DC_M,     "kg", xidNettInQmSum,     this, isResult|noFile|noSnap); 
  //      DDB.Double ("",   "Balance.Rel",  DC_Frac,  "%",  xidNettInQmSumRel,  this, isResult|noFile|noSnap); 
  //      }
  //    DDB.EndStruct();

  //    DDB.Text(" ");
  //    if (DDB.BeginStruct(this, "Heat", NULL, DDB_NoPage))
  //      {
  //      DDB.Double ("",   "Spills",       DC_E,     "kJ", xidSpillsInHzSum,   this, isResult|noFile|noSnap);
  //      DDB.Double ("",   "Vents",        DC_E,     "kJ", xidVentsInHzSum,    this, isResult|noFile|noSnap);
  //      DDB.Double ("",   "LeaksIn",      DC_E,     "kJ", xidLeaksInHzSum,    this, isResult|noFile|noSnap);
  //      DDB.Double ("",   "LeaksOut",     DC_E,     "kJ", xidLeaksOutHzSum,   this, isResult|noFile|noSnap);
  //      DDB.Double ("",   "Srcs",         DC_E,     "kJ", xidSrcsInHzSum,     this, isResult|noFile|noSnap);
  //      DDB.Double ("",   "Snks",         DC_E,     "kJ", xidSnksInHzSum,     this, isResult|noFile|noSnap);
  //      DDB.Double ("",   "Makeups",      DC_E,     "kJ", xidMakeupsInHzSum,  this, isResult|noFile|noSnap); 
  //      DDB.Double ("",   "Bleeds",       DC_E,     "kJ", xidBleedsInHzSum,   this, isResult|noFile|noSnap); 
  //      DDB.Double ("",   "Accumulation", DC_E,     "kJ", xidAccumsInHzSum,   this, isResult|noFile|noSnap);
  //      DDB.Double ("",   "Depletion",    DC_E,     "kJ", xidAccumsOutHzSum,  this, isResult|noFile|noSnap);
  //      DDB.Double ("",   "Balance",      DC_E,     "kJ", xidNettInHzSum,     this, isResult|noFile|noSnap);
  //      DDB.Double ("",   "Balance.Rel",  DC_Frac,  "%",  xidNettInHzSumRel,  this, isResult|noFile|noSnap);
  //      }
  //    DDB.EndStruct();
  //    }
  //  DDB.EndStruct();
  //  }

  //DDB.Text(" ");
  //if (DDB.BeginStruct(this, "Counts", NULL, DDB_RqdPage))
  //  {
  //  DDB.Long   ("",   "Spills",   DC_,     "",       xidSpillsInCnt,  this, isResult|noFile|noSnap); 
  //  DDB.Long   ("",   "Vents",    DC_,     "",       xidVentsInCnt,   this, isResult|noFile|noSnap); 
  //  DDB.Long   ("",   "LeaksIn",  DC_,     "",       xidLeaksInCnt,   this, isResult|noFile|noSnap); 
  //  DDB.Long   ("",   "LeaksOut", DC_,     "",       xidLeaksOutCnt,  this, isResult|noFile|noSnap); 
  //  DDB.Long   ("",   "Srcs",     DC_,     "",       xidSrcsInCnt,    this, isResult|noFile|noSnap); 
  //  DDB.Long   ("",   "Snks",     DC_,     "",       xidSnksInCnt,    this, isResult|noFile|noSnap); 
  //  DDB.Long   ("",   "Makeups",  DC_,     "",       xidMakeupsInCnt, this, isResult|noFile|noSnap); 
  //  DDB.Long   ("",   "Bleeds",   DC_,     "",       xidBleedsInCnt,  this, isResult|noFile|noSnap); 
  //  DDB.Long   ("",   "Accums",   DC_,     "",       xidAccumsInCnt,  this, isResult|noFile|noSnap); 
  //  //DDB.Long   ("",   "Nett",     DC_,     "",       xidNettInCnt,    this, isResult|noFile|noSnap); 
  //  }
  //DDB.EndStruct();

  //DDB.Text(" ");
  //DDB.Text("Audit Clipboard options:");
  //DDB.Button("Copy_All",       "", DC_, "", xidIO_CopyAdtAll, this, isParm);
  //DDB.Button("Copy_Mass",      "", DC_, "", xidIO_CopyAdtMass, this, isParm);
  //DDB.Button("Copy_MassTotal", "", DC_, "", xidIO_CopyAdtMassTotal, this, isParm);
  //DDB.Button("Copy_Heat",      "", DC_, "", xidIO_CopyAdtHeat, this, isParm);
  //DDB.Button("Copy_HeatTotal", "", DC_, "", xidIO_CopyAdtHeatTotal, this, isParm);

  //DDB.Visibility();
  //DDB.Object(&m_Content,  this, NULL, NULL, DDB_RqdPage);
  //if (0)
  //  {
  //  //DDB.Object(&m_SrcsI,   this, NULL, NULL, DDB_RqdPage, -1, isResult);
  //  //DDB.Object(&m_SnksO,   this, NULL, NULL, DDB_RqdPage, -1, isResult);
  //  //DDB.Object(&m_MakeupsI,this, NULL, NULL, DDB_RqdPage, -1, isResult);
  //  //DDB.Object(&m_BleedsO, this, NULL, NULL, DDB_RqdPage, -1, isResult);
  //  //DDB.Object(&m_SpillsO, this, NULL, NULL, DDB_RqdPage, -1, isResult);
  //  //DDB.Object(&m_VentsO,  this, NULL, NULL, DDB_RqdPage, -1, isResult);
  //  //DDB.Object(&m_LeaksI,  this, NULL, NULL, DDB_RqdPage, -1, isResult);
  //  //DDB.Object(&m_OvResults.m_Sum[AR_LeaksO],  this, NULL, NULL, DDB_RqdPage, -1, isResult);
  //  //DDB.Object(&m_AccI,    this, NULL, NULL, DDB_RqdPage, -1, isResult);
  //  //DDB.Object(&m_AccO,    this, NULL, NULL, DDB_RqdPage, -1, isResult);
  //  }

  //DDB.Text("");
  //DDB.EndStruct();
  };  

//--------------------------------------------------------------------------

flag CArea::DataXchg(DataChangeBlk & DCB)
  {
  if (MN_Area::DataXchg(DCB)) 
    return 1;

  return 0;
  }

//==========================================================================
//
//
//
//==========================================================================

static IOAreaRec MakeupSrcIOAreaList[] =
  {
    {"MakeupIO",  "MakeupIO", IOId_AreaMakeupO, LIO_Out,    nc_MLnk, 0, MaxIOList, IOPipeEntry, 0, {0,0,0,0}, NULL, &NullFlwGroup},
    //{"BleedIO",   "BleedIO",  IOId_AreaBleedI,  LIO_In,     nc_MLnk, 0, MaxIOList, IOPipeEntry, 0, {0,0,0,0}, NULL, &NullFlwGroup},
    //{"SpillIO",   "SpillIO",  IOId_AreaSpillI,  LIO_In,     nc_MLnk, 0, MaxIOList, IOPipeEntry, 0, {0,0,0,0}, NULL, &Xfer2AreaGroup},
    //{"VentIO",    "VentIO",   IOId_AreaVentI,   LIO_In,     nc_MLnk, 0, MaxIOList, IOPipeEntry, 0, {0,0,0,0}, NULL, &Xfer2AreaGroup},
    //{"LeakIO",    "LeakIO",   IOId_AreaLeakI,   LIO_InOut,  nc_MLnk, 0, MaxIOList, IOPipeEntry, 0, {0,0,0,0}, NULL, &NullFlwGroup},
    {NULL}
  };

static double Drw_MakeupSrc[] = 
  { 
  DD_Poly, -2,3, 3,3, 5,0, 3,-3, -2,-3, 
  DD_Arc3, -2,-3, -5,0, -2,3,
  DD_End 
  };

//--------------------------------------------------------------------------

IMPLEMENT_MODELUNIT(MakeupSrc, "MakeupSrc", "1", Drw_MakeupSrc, "MakeupSrc", "MakeupSrc", TOC_ALL|TOC_GRP_GENERAL|TOC_STD_KENWALT,
                    "Process:Unit:Makeup Source", 
                    "Simple Source Material for Makeups") 

MakeupSrc::MakeupSrc(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MN_Area(pClass_, TagIn, pAttach, eAttach)
  {
  AttachIOAreas(MakeupSrcIOAreaList);
  m_Content.SetView(SVV_AsMassFrac);  
  m_bForceAudit=true;
  }

//--------------------------------------------------------------------------

void MakeupSrc::PrepareDefinedData()
  {
#if WITHSPECSHEETS
  if (sSpecSheet.XStrCmp("-")!=0)
    sSpecSheet= Contents.Model()->ShortDesc();
#endif
  }

//--------------------------------------------------------------------------

void MakeupSrc::BuildDataDefn(DataDefnBlk & DDB)
  {
  MN_Area::BuildDataDefn(DDB, BDDAO_Flows|BDDAO_Totals|BDDAO_Contents|BDDAO_SrcSnk|BDDAO_MULinks);
  };  

//--------------------------------------------------------------------------

flag MakeupSrc::DataXchg(DataChangeBlk & DCB)
  {
  if (MN_Area::DataXchg(DCB)) 
    return 1;

  return 0;
  }

//==========================================================================
//
//
//
//==========================================================================

static IOAreaRec BleedSnkIOAreaList[] =
  {
    //{"MakeupIO",  "MakeupIO", IOId_AreaMakeupO, LIO_Out,    nc_MLnk, 0, MaxIOList, IOPipeEntry, 0, {0,0,0,0}, NULL, &NullFlwGroup},
    {"BleedIO",   "BleedIO",  IOId_AreaBleedI,  LIO_In,     nc_MLnk, 0, MaxIOList, IOPipeEntry, 0, {0,0,0,0}, NULL, &NullFlwGroup},
    //{"SpillIO",   "SpillIO",  IOId_AreaSpillI,  LIO_In,     nc_MLnk, 0, MaxIOList, IOPipeEntry, 0, {0,0,0,0}, NULL, &Xfer2AreaGroup},
    //{"VentIO",    "VentIO",   IOId_AreaVentI,   LIO_In,     nc_MLnk, 0, MaxIOList, IOPipeEntry, 0, {0,0,0,0}, NULL, &Xfer2AreaGroup},
    //{"LeakIO",    "LeakIO",   IOId_AreaLeakI,   LIO_InOut,  nc_MLnk, 0, MaxIOList, IOPipeEntry, 0, {0,0,0,0}, NULL, &NullFlwGroup},
    {NULL}
  };

static double Drw_BleedSnk[] = 
  { 
  DD_Poly, -5,3, 2,3, 
  DD_Arc3, 2,3, 5,0, 2,-3,
  DD_Poly,  2,-3, -5,-3, -3,0, -5,3, DD_End 
  };

//--------------------------------------------------------------------------

IMPLEMENT_MODELUNIT(BleedSnk, "BleedSnk", "1", Drw_BleedSnk, "BleedSnk", "BleedSnk", TOC_ALL|TOC_GRP_GENERAL|TOC_STD_KENWALT,
                    "Process:Unit:Bleed Sink", 
                    "Simple Sink for Bleeds") 

BleedSnk::BleedSnk(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MN_Area(pClass_, TagIn, pAttach, eAttach)
  {
  AttachIOAreas(BleedSnkIOAreaList);
  m_Content.SetView(SVV_AsMassFrac);  
  m_bForceAudit=true;
  }

//--------------------------------------------------------------------------

void BleedSnk::PrepareDefinedData()
  {
#if WITHSPECSHEETS
  if (sSpecSheet.XStrCmp("-")!=0)
    sSpecSheet= Contents.Model()->ShortDesc();
#endif
  }

//--------------------------------------------------------------------------

void BleedSnk::BuildDataDefn(DataDefnBlk & DDB)
  {
  MN_Area::BuildDataDefn(DDB, BDDAO_Flows|BDDAO_Totals|BDDAO_SrcSnk);
  };  

//--------------------------------------------------------------------------

flag BleedSnk::DataXchg(DataChangeBlk & DCB)
  {
  if (MN_Area::DataXchg(DCB)) 
    return 1;

  return 0;
  }

//==========================================================================
//
//
//
//==========================================================================

