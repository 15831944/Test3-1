//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992
 
#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdio.h>

#include "sc_defs.h"
#define  __SECREFORM_CPP
#include "SecReform.h"
                 
//==========================================================================
const byte ioid_HC            = 0;
const byte ioid_SO            = 1;
const byte ioid_Prod          = 2;
//const byte ioid_OverFlw       = 3;
//const byte ioid_GasVent       = 4;

static IOAreaRec SecRefIOAreaList[] =
  {{"Feed Hydrocarbons",          "FeedHC"     , ioid_HC  , LIO_InOut,  nc_MLnk, 0, 20, IOPipeEntry, 1.0f},
   {"Feed Stm Oxy",               "FeedSO"     , ioid_SO  , LIO_InOut,  nc_MLnk, 0, 20, IOPipeEntry, 1.0f},
   {"Product",                    "Product"    , ioid_Prod, LIO_InOut,  nc_MLnk, 0, 20, IOPipeEntry, 0.0f},
   //{"OverFlow",                   "OverFlw"  , ioid_OverFlw, LIO_Out,    nc_MLnk, 0, 10, IOPipeEntry, 1.0f},
   //{"GasVent",                    "GasVent"  , ioid_GasVent, LIO_Out,    nc_MLnk, 0, 10, IOPipeEntry|IOGasVent , 1.0f},
   //{"Heat Exchange Element",      "HX"       , IOId_HX,         LIO_InOut,  nc_MLnk, 2, 20, IOGRP(1) },
   //{"Heat Exchange Element In",   "HX_In"    , IOId_HX,         LIO_In,     nc_MLnk, 2, 20, IOGRP(1) },
   //{"Heat Exchange Element Out",  "HX_Out"   , IOId_HX,         LIO_Out,    nc_MLnk, 2, 20, IOGRP(1) },
   {NULL}}; //This lists the areas of the model where links can be attached.


static double Drw_SecReformer[] = { DD_Scale, 1500,
                           DD_Poly, -5,-5, -5,5, -2,10, -2,15, 2,15, 2,10, 5,5, 5,-5,
                           DD_Arc3, -5,-5, 0,-7, 5,-5,
                           DD_Poly, 2,-7, 2,-12, -2,-12, -2,-7, 
                           DD_End }; //This provides the default drawing of the SecReformer.

//--------------------------------------------------------------------------

IMPLEMENT_MODELUNIT(SecReformer, "SecReformer", "1", Drw_SecReformer, "MixerSplitter", "SR", 
                    TOC_DYNAMICFULL|TOC_USER, 
                    "Secondary Reformer(1)",   
                    "Secondary Reformer") 

SecReformer::SecReformer(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MN_Surge(pClass_, TagIn, pAttach, eAttach)
  {
  AttachIOAreas(SecRefIOAreaList, &PipeEntryGroup);
  Contents.SetClosed(true);
  Contents.SetPreset(&m_Preset, &m_PresetImg);
  Contents.SetSQFlags(SQF_Editable, true);
//  FlashSplit=0.1;
//  SS_Lvl=0.25;

  for (int t=0; t<MaxSecRefTPoints; t++)
    {
    m_TPoints[t].m_dTemp=Std_T;
    m_TPoints[t].m_dMult=1;
    }
  };

//--------------------------------------------------------------------------
/*This provides access to the variables of the model and calls the
BuildDataDefn for objects used by the model.*/
#ifdef _RELEASE
#pragma optimize("", off)
#endif

void SecReformer::BuildDataDefn(DataDefnBlk & DDB)
  {
  Contents.SetSQFlags(SQF_Editable, true);//why does setting this flag above in constructor not work?
  DDB.BeginStruct(this);

  DDB.Visibility(SM_DynBoth|HM_All);
  BuildDataDefnElevation(DDB);
  DDB.Text    ("");
  DDB.Double  ("Pressure",     "P",     DC_P,    "kPag",   xidPMean,       this, isResult|noFile|noSnap);
  DDB.Double  ("Temperature",  "T",     DC_T,    "C",      xidTemp,        this, isResult|noFile|noSnap);
  DDB.Double  ("Density",      "Rho",   DC_Rho,  "kg/m^3", xidRho,         this, isResult|noFile|noSnap);
  //DDB.Double  ("Level",        "Lvl",   DC_Frac, "%",      xidLevel,       this, isResult|noFile|noSnap);
  DDB.Double  ("Mass_Accum",   "QmAcc", DC_Qm,  "kg/s",    &m_QmAcc,       this, isResult|noFile|noSnap);
  DDB.Double  ("Vol_Accum",    "QvAcc", DC_Qv,  "L/s",     &m_QvAcc,       this, isResult|noFile|noSnap);

  DDB.Text    ("");
  DDB.CheckBoxBtn("Closed",    "",      DC_     , "",      xidClosed,      this, 
    isParmStopped|(!gs_Environment.HasGas() ? noView|noFile|noSnap|noScenario:0) , DDBYesNo);
  DDB.CheckBoxBtn("Networked", "", DC_     , "",       xidNetPressDamp,       this, 
    isParmStopped|(!gs_Environment.HasGas() || !Contents.Closed() ? noView|noFile|noSnap|noScenario:0) , DDBYesNo);

  for (int t=0; t<MaxSecRefTPoints; t++)
    {
    Strng Tg;
    Tg.Set("TPoint-%i", t);
    if (DDB.BeginStruct(this, Tg()))
      {
      DDB.Double ("Mult", "", DC_, "", &m_TPoints[t].m_dMult, this, isParm); 
      DDB.Double ("T",    "", DC_T, "C", &m_TPoints[t].m_dTemp, this, isParm); 
      }
    DDB.EndStruct();
    }

  //DDB.Visibility(SM_Direct|HM_All);
  //PBPCtrl0.BuildDataDefn(DDB, this);
  //GSM.Add_OnOff(DDB);
  DDB.Visibility(SM_DynBoth|HM_All);
  DDB.Visibility();
  VLE.Add_OnOff(DDB);
  HX.Add_OnOff(DDB);
  RB.Add_OnOff(DDB);

  DDB.Visibility();
 
  BuildDataDefnShowIOs(DDB);
  BuildDataDefnIOOpts(DDB);

  //if (SolveDirectMethod())
  //  {
  //  QFeed(); // Ensure that exist so that they recover
  //  QProd();
  //  if (QFeed.Exists())
  //    DDB.Object(&QFeed, this, NULL, NULL, DDB_RqdPage);
  //  if (QProd.Exists())
  //    DDB.Object(&QProd, this, NULL, NULL, DDB_RqdPage);
  //  }
  m_EqThermals.BuildDataDefn(DDB, "Body");

  HX.BuildDataDefn(DDB);
  RB.BuildDataDefn(DDB);
  VLE.BuildDataDefn(DDB);

  if (SolveDynamicMethod())
    {
    //dword IOMsk=Id_2_Mask(ioid_Top)|Id_2_Mask(ioid_Side)|Id_2_Mask(ioid_Base)|Id_2_Mask(ioid_OverFlw);
    }
  //else
  //  GSM.BuildDataDefn(DDB);

  if (SolveDynamicMethod())
    {
    DDB.Object(&Contents, this, NULL, NULL, DDB_RqdPage);
    DDB.Object(&m_PresetImg, this, NULL, NULL, DDB_RqdPage);
    }

  DDB.EndStruct();
  }

#ifdef _RELEASE
#pragma optimize("", on)
#endif
//--------------------------------------------------------------------------

flag SecReformer::DataXchg(DataChangeBlk & DCB)
  {
  if (MN_Surge::DataXchg(DCB)) 
    return 1;
  if (VLE.DataXchg(DCB))
    return 1;
  if (HX.DataXchg(DCB))
    return 1;
  if (m_EqThermals.DataXchg(DCB))
    return 1;  

  switch (DCB.lHandle)
    {
    case xidClosed:
      if (DCB.rB)
        Contents.SetClosed(*DCB.rB); 
      DCB.B=Contents.Closed(); 
      return 1;
    }
  return 0;
  }

//---------------------------------------------------------------------------

flag SecReformer::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=MN_Surge::ValidateData(VDB);
  return OK;
  }

//--------------------------------------------------------------------------

void SecReformer::EvalProducts(CNodeEvalIndex & NEI)
  {
  switch (SolveMethod())
    {
    case SM_Direct:
    default:
      MN_Surge::EvalProducts(NEI);
    }
  }

//--------------------------------------------------------------------------
    
void SecReformer::EvalDerivs(CNodeEvalIndex & NEI)
  {
  MN_Surge::EvalDerivs(NEI);
  }

//--------------------------------------------------------------------------

void SecReformer::EvalDiscrete()
  {
  double T=Contents.Temp();
  for (int t=0; t<MaxSecRefTPoints; t++)
    m_TPoints[t].m_dTemp=T+(T-Std_T)*m_TPoints[t].m_dMult;
  }

// ===========================================================================
