//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992
 
#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdio.h>

#define  __AD_OTHER_CPP

#include "sc_defs.h"
#include "gpfuncs.h"
#include "ad_other.h"
#include "sp_cont.h"
#include "flwnode.h"
 
#include "errorlog.h"
#include "dbgmngr.h"
 
//#define dbgADBase       01
//#if dbgADBase 
//static CDbgMngr dbgEvalDerivs      ("ADObject",  "EvalDerivs");
//
//#endif

//==========================================================================
//
//
//
//==========================================================================

IMPLEMENT_ADOBJECT(CADThickener, "ADThick", "", "AD", "SC", TOC_SYSTEM, "Unit:Thickener", "Thickener") 
IMPLEMENT_SPARES(CADThickener, 10);
 
CADThickener::CADThickener(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CADObject(pClass_, TagIn, pAttach, eAttach)
  {
  };

//--------------------------------------------------------------------------

CADThickener::~CADThickener()
  {
  };

//==========================================================================
//
//
//
//==========================================================================

IMPLEMENT_ADOBJECT(CADSysCurve, "ADSysCrv", "", "AD", "SC", TOC_SYSTEM, "Network:System Curve", "System Curve") 
IMPLEMENT_SPARES(CADSysCurve, 10);
 
CADSysCurve::CADSysCurve(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CADObject(pClass_, TagIn, pAttach, eAttach)
  {
  };

//--------------------------------------------------------------------------

CADSysCurve::~CADSysCurve()
  {
  };

//==========================================================================
//
//
//
//==========================================================================

IMPLEMENT_ADOBJECT(CADNetFlash, "ADNtFlsh", "", "AD", "SC", TOC_SYSTEM, "NetWork:Flash Position", "NetWork Flash") 
IMPLEMENT_SPARES(CADNetFlash, 10);
 
CADNetFlash::CADNetFlash(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CADObject(pClass_, TagIn, pAttach, eAttach)
  {
  };

//--------------------------------------------------------------------------

CADNetFlash::~CADNetFlash()
  {
  };

//==========================================================================
//
//
//
//==========================================================================

IMPLEMENT_ADOBJECT(CADPipeVelocity, "ADPpVelo", "", "AD", "SC", TOC_SYSTEM, "Pipe:Velocity", "Pipe Velocity") 
IMPLEMENT_SPARES(CADPipeVelocity, 10);
 
CADPipeVelocity::CADPipeVelocity(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CADObject(pClass_, TagIn, pAttach, eAttach)
  {
  };

//--------------------------------------------------------------------------

CADPipeVelocity::~CADPipeVelocity()
  {
  };

//==========================================================================
//
//
//
//==========================================================================
