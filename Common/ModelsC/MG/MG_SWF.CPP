//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#include "sc_defs.h"
#include "datacnvs.h"
#include "flwnode.h"
#include "mg_swf.h"

#include "net_err.h"
//#include "dbgcom.h"
/*
*****************************************************************************


This model only been made to compile so far - plenty fixes needed


******************************************************************************
*/
#include "debugnew.h" // must follow all IMPLEMENT_DYNCREATE & IMPLEMENT_SERIAL

long Drw_SWF_Package[] = { DD_Poly, -5,5, -5,-5, 5,-5, 5,5, -5,5, 
                     DD_End }; 

IMPLEMENT_MODELUNIT(SWF_Package, "SWFPack", "", Drw_SWF_Package, "STR", TOC_ALL|TOC_GRP_ENERGY|TOC_USER, 
                    "SWF_Package", 
                    "SWF_Package")  

//--------------------------------------------------------------------------




SWF_Package::SWF_Package(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) : 
  MN_BstRes(pClass_, TagIn, pAttach, eAttach)
  {
  AttachClassInfo(nc_Process, TwoIOAreaList);
  };

//--------------------------------------------------------------------------
  
void   SWF_Package::BuildDataDefn(DataDefnBlk & DDB)
  { 
  DDB.BeginStruct(this); 

  MN_BstRes::BuildDataDefn(DDB);

  DDB.Text(""); 
  DDB.Double ("Datum",        "Z",      DC_L,     "m",      &dDatum,          this  ,isParm); 
  DDB.Text("");                                                        
  DDB.EndStruct(); 
  };




