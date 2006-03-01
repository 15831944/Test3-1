//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <float.h>

#define  __CONVEYOR_CPP

#include "conveyor.h"

long Drw_Conveyor[] = { DD_Poly, -20,2, -20,-2, -18,-2, -18,0, 18,0, 18,-2, 20,-2, 20,2, -20,2,
                        DD_End }; //This provides the default drawing of the Conveyor.
//--------------------------------------------------------------------------

IMPLEMENT_MODELUNITHIDDEN(Conveyor, "Conveyor", "1", Drw_Conveyor, "CV", TOC_ALL|TOC_GRP_TRANSFER|TOC_STD_KENWALT,
                    "Materials Handling:Conveyor",
                    "Conveyor");

Conveyor::Conveyor(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  BeltCnv(pClass_, TagIn, pAttach, eAttach)
  {
  };

// -------------------------------------------------------------------------

Conveyor::~Conveyor()
  {
  }

//--------------------------------------------------------------------------
