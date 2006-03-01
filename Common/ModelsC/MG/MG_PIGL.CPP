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
#include "mg_pigl.h"




long Drw_PigLaunch[] = { DD_Poly, -5,5, -5,-5, 5,-5, 5,5, -5,5, 
                     DD_End }; 

IMPLEMENT_MODELUNIT(PigLaunch, "PigLnch", "", Drw_PigLaunch, "PIG", TOC_ALL|TOC_GRP_ENERGY|TOC_USER, 
                    "PigLauncher", 
                    "PigLauncher")  
 


PigLaunch::PigLaunch(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) : 
  MN_Xfer(pClass_, TagIn, pAttach, eAttach)
  {
  };

//--------------------------------------------------------------------------

void PigLaunch::BuildDataDefn(DataDefnBlk & DDB)
  {
  MN_Xfer::BuildDataDefn(DDB);
  }
