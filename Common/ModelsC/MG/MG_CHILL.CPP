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
#include "mg_chill.h"

#define DllImportExport 

long Drw_GasChiller[] = { DD_Poly, -5,5, -5,-5, 5,-5, 5,5, -5,5, 
                     DD_End }; 

IMPLEMENT_MODELUNIT(GasChill, "GasChill", "", Drw_GasChiller, "CHL", TOC_ALL|TOC_GRP_ENERGY|TOC_USER, 
                    "GasChiller", 
                    "GasChiller")  
 
GasChill::GasChill(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) : 
  HeatXchg1(pClass_, TagIn, pAttach, eAttach)
  {
  };

//--------------------------------------------------------------------------

void GasChill::BuildDataDefn(DataDefnBlk & DDB)
  {
  HeatXchg1::BuildDataDefn(DDB);
  }


#undef DllImportExport 
