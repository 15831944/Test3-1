//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <float.h>

#define __MG_ESDV_CPP

#include "sc_defs.h"
#include "datacnvs.h"
#include "mg_esdv.h"


long Drw_ESDValve[] = { DD_Poly, -5,5, -5,-5, 5,-5, 5,5, -5,5, 
                     DD_End }; 

IMPLEMENT_MODELUNIT(ESDValve, "ESDValve", "", Drw_ESDValve, "ESD", TOC_ALL|TOC_GRP_ENERGY|TOC_USER, 
                    "ESDValve", 
                    "ESDValve")  
 
ESDValve::ESDValve(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) : 
  Valve_GP(pClass_, TagIn, pAttach, eAttach)
  {
  };

//--------------------------------------------------------------------------

void ESDValve::BuildDataDefn(DataDefnBlk & DDB)
  {
  Valve_GP::BuildDataDefn(DDB);
  }
