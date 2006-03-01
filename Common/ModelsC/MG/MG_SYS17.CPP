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

#include "mg_sys17.h"
#include "sc_defs.h"
#include "datacnvs.h"
//#include "netwrk.h"
#include "flwnode.h"
#include "debugnew.h" 


long Drw_System17[] = { 
                        DD_Poly, -5,5, -5,-5, 5,-5, 5,5, -5,5, 
                        DD_End 
                      }; 

IMPLEMENT_MODELUNIT(Sys17, "System17", "", Drw_System17, "S17", TOC_ALL|TOC_GRP_ENERGY|TOC_USER, 
                    "System17", 
                    "System17")  

Sys17::Sys17(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach):
  MdlNode(pClass_, TagIn, pAttach, eAttach)
  {
  AttachClassInfo(nc_Process, TwoIOAreaList);
  AllDataHere = 1;
  PK1702.Bst=520.0;
  PK1702.bAuto=0;
  PK1702.iOperate=0;
  PIA17002.P = 0.0;
  };

// -------------------------------------------------------------------------

void Sys17::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this); 
  DDB.Text(" ");                                                                           
  DDB.Double    ("PK1702.Bst",     "",   DC_P,  "kPag",     &PK1702.Bst           ,this  ,0);
  DDB.Bool      ("PK1702.Auto",    "",   DC_,       "",     &PK1702.bAuto         ,this  ,0);
  DDB.Long      ("PK1702.Operate", "",   DC_,       "",     &PK1702.iOperate      ,this  ,0);
  DDB.Double    ("PIA17002.P",     "",   DC_P,  "kPag",     &PIA17002.P           ,this  ,0);
  DDB.Text(" ");                                                                           
  DDB.EndStruct(); 
  }

// --------------------------------------------------------------------------

void Sys17::EvalDerivs()
  {
  };

// --------------------------------------------------------------------------

void Sys17::EvalDiscrete()
  {
  PIA17002.P = (PK1702.bAuto ? PK1702.Bst : Std_P);
      if (PK1702.iOperate < 0)
        {
        PK1702.bAuto = !PK1702.bAuto;
        PK1702.iOperate= PK1702.bAuto;
        }
//  dbgpln("System17 %i %g", PK1702.bAuto, PIA17002.P);
  };

