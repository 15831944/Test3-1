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
#include "mg_strn.h"

#include "net_err.h"
//#include "dbgcom.h"
/*
*****************************************************************************


This model only been made to compile so far - plenty fixes needed


******************************************************************************
*/
#include "debugnew.h" // must follow all IMPLEMENT_DYNCREATE & IMPLEMENT_SERIAL

long Drw_Strainer[] = { DD_Poly, -5,5, -5,-5, 5,-5, 5,5, -5,5,
                     DD_End };

IMPLEMENT_MODELUNIT(Strainer, "Strainer", "", Drw_Strainer, "STR", TOC_ALL|TOC_GRP_ENERGY|TOC_USER,
                    "Strainer",
                    "Strainer")

//--------------------------------------------------------------------------




Strainer::Strainer(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MN_BstRes(pClass_, TagIn, pAttach, eAttach)
  {
  AttachClassInfo(nc_Process, TwoIOAreaList);
  R_Fxd = 1.0;
  };

//--------------------------------------------------------------------------

void Strainer::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);
  DDB.Double ("Resistance",      "Resistance",         DC_,      "",   &R_Fxd,   this, isParm);

  DDB.Text   ("");
  DDB.Double("Mass_Flow",    "Qm",     DC_Qm  , "t/h",    xidQm         ,this, 0);
  DDB.Double("Vol_Flow",     "Qv",     DC_Qv  , "m^3/h",  xidQv         ,this, 0);
  DDB.Double("NVol_Flow",    "NQv",    DC_NQv , "Nm^3/h", xidNQv        ,this, 0);

  DDB.Double("Temperature",  "T",      DC_T   , "C",      xidTemp       ,this, 0);
  DDB.Double("Density" ,     "Rho",    DC_Rho , "kg/m^3", xidRho        ,this, 0);

  DDB.Text   ("");
  DDB.String ("SrcIO",         "",      DC_,     "",       xidSrcIOTag   ,this, 0);
  DDB.String ("DstIO",         "",      DC_,     "",       xidDstIOTag   ,this, 0);

  DDB.EndStruct();
  };

//--------------------------------------------------------------------------
/*
flag Strainer::EvalFlowEquations(CSpPropInfo *pProps, int iJoinId, int FE)
  {
  FB.SetDPb(0.0, 0.0);
  FB.SetDPq(-R_Fxd*FB.GetQm(), -R_Fxd);
  return True;
  };*/

//--------------------------------------------------------------------------

flag Strainer::EvalFlowEquations(CSpPropInfo *pProps, int IONo, int FE)
  {
  FlwBlk &FB = *IOFB(IONo, FE);
  FB.SetDPb(0.0, 0.0);
  FB.SetDPq(-R_Fxd*FB.GetQm(), -R_Fxd);
  return True;
  };




