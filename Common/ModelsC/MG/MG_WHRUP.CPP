//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <float.h>
                                                                        
#include "sc_defs.h"
#include "datacnvs.h"
#include "mg_whrup.h"


long Drw_WHRUP_Package[] = { DD_Poly, -5,5, -5,-5, 5,-5, 5,5, -5,5, 
                     DD_End }; 

IMPLEMENT_MODELUNIT(WHRUP_Package, "WHRUPPck", "", Drw_WHRUP_Package, "WHR", TOC_ALL|TOC_GRP_ENERGY|TOC_USER, 
                    "WHRUP_Package", 
                    "WHRUP_Package")  
 

WHRUP_Package::WHRUP_Package(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) : 
  HeatXchg(pClass_, TagIn, pAttach, eAttach)
  {
  AttachClassInfo(nc_Process, TwoIOAreaList);
  DamperPos=0.5;
  };

//--------------------------------------------------------------------------

void WHRUP_Package::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this); 
  DDB.Double ("Damper",      "Damper",         DC_Frac,      "%",   &DamperPos,   this, 0); 
  HeatXchg::BuildDataDefn(DDB);
  DDB.EndStruct(); 
  }

// --------------------------------------------------------------------------

void WHRUP_Package::EvalDerivs()
  {
  Eff=Range(0.0, DamperPos*0.95, 1.0);
  HeatXchg::EvalDerivs();
  };









