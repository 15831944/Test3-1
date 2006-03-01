//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992
 
#include "stdafx.h"
#define __2D_ROSIN_CPP
#include "sc_defs.h"
#include "2d_fn.h"
#include "2d_rosin.h"

// =========================================================================

IMPLEMENT_2D_MODEL(C2DRosin, "2D_Rosin", "", TOC_SYSTEM, "Rosin-Rambler Distribution", "Rosin-Rambler Size Distribution");

// =========================================================================
/*#D:#T:Log-Normal Distribution
#X:#n#u<#l<#b<General Description>>>#PThis fits the Rosin-Rambler distribution
curve to data, for example when determining the particle size distribution
of granular materials, according tho the following formula :
F(X)=...
#P#u<#l<#b<Variables to be supplied by the user>>>#n
#p#b<XRange :>#NThis gives an indication of the range of the 'Xe'.
Cannot be zero.
#p#b<Charctr :>#NA constant which describes the characteristics of the
material.*/

pchar C2DRosin::ParmDescs[RosinNoOfParms] =
  {"XRange", "Charctr", "Gain", "Offset" };

// =========================================================================
 
C2DRosin::C2DRosin(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  C2DModel(pClass_, pTag, pAttach, eAttach)
  {
  Clear();
  }
 
// -------------------------------------------------------------------------
 
C2DRosin::~C2DRosin()
  {
  }
 
//---------------------------------------------------------------------------
 
void C2DRosin::Clear()
  {
  CBaseMdl::Clear();
  SetNParms(RosinNoOfParms);
  Parms[0] = 1.0;  //XRange
  Parms[1] = 0.0;  //Charctr
  Parms[2] = 1.0;  //Gain
  Parms[3] = 0.0;  //Offset
  }

// -------------------------------------------------------------------------
 
double C2DRosin::Yx(double Xi)
  {
  return (exp(- Pow(Xi / Parms[0], Parms[1])) * Parms[2]) + Parms[3];
  }

//---------------------------------------------------------------------------
// =========================================================================
