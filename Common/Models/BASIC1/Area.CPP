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

  static const BDDSpecies_Blk sbImage[]=
    {
      {SVV_AsMassFrac,    SVV_AsMassFrac,  true,  true , VAMsk_SpMass },
      {SVV_AsMoleFrac,    SVV_AsMassFrac,  true,  false, VAMsk_SpMole },
      {SVV_AsVolFrac,     SVV_AsMassFrac,  true,  false, VAMsk_SpVol  },
      {SVV_AsNVolFrac,    SVV_AsMassFrac,  true,  false, VAMsk_SpNVol },
    };                      

  m_Content.SetViewBlk(sbImage, sizeof(sbImage));

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

