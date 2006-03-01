//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992
 
#include "stdafx.h"
#define __2D_HARIS_CPP
#include "sc_defs.h"
#include "2d_fn.h"
#include "2d_haris.h"

// =========================================================================

IMPLEMENT_2D_MODEL(C2DHaris, "2D_Haris", "", TOC_SYSTEM, "Harris Distribution", "Harris Equation Size Distribution");

// =========================================================================
/*#D:#T:Harris Equation Distribution
#X:#n#u<#l<#b<General Description>>>#PThis fits the Harris Equation distribution
curve to data, for example when determining the particle size distribution
of granular materials. This makes use of the standard Rosin Rambler plot.
The Harris Equation is flexible and fits most uni-modal distributions.
F(X)=...
#P#u<#l<#b<Variables to be supplied by the user>>>#n
#p#b<MaxSize :>#NThe maximum size in the sample.
Cannot be zero.
#p#b<FSlope :>#NThis represents the slope of the Log-Ln plot in the fine-size region
#p#b<CSlope :>#NThis represents the slope of the Log-Ln plot in the coarse-size region
plot).*/

pchar C2DHaris::ParmDescs[HarisNoOfParms] =
  {"MaxSize", "FSlope", "CSlope", "Gain", "Offset" };

// =========================================================================
 
C2DHaris::C2DHaris(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  C2DModel(pClass_, pTag, pAttach, eAttach)
  {
  Clear();
  }
 
// -------------------------------------------------------------------------
 
C2DHaris::~C2DHaris()
  {
  }
 
//---------------------------------------------------------------------------
 
void C2DHaris::Clear()
  {
  CBaseMdl::Clear();
  SetNParms(HarisNoOfParms);
  Parms[0] = 1.0; //MaxSize
  Parms[1] = 0.0; //FSlope
  Parms[2] = 0.0; //CSlope
  Parms[3] = 1.0; //Gain
  Parms[4] = 0.0; //Offset
  }

// -------------------------------------------------------------------------
 
double C2DHaris::Yx(double Xi)
  {
  return (Pow(1-Pow(Xi / Parms[0], Parms[1]), Parms[2]) * Parms[3]) + Parms[4];
  }

//---------------------------------------------------------------------------
// =========================================================================
