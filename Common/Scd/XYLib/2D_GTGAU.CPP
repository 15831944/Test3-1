//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992
 
#include "stdafx.h"
#define __2D_GTGAU_CPP
#include "sc_defs.h"
#include "2d_fn.h"
#include "2d_gtgau.h"

// =========================================================================

IMPLEMENT_2D_MODEL(C2DGtGau, "2D_GtGau", "", TOC_SYSTEM, "Gat-Gau Distribution", "Gates-Gaudin Schuhmann Size Distribution");

// =========================================================================
/*#D:#T:Gates-Gaudin Schuhmann Distribution
#X:#n#u<#l<#b<General Description>>>#PThis fits the Gates-Gaudin
Distribution curve to data, for example when determining the particle
size distribion of granular materials. This makes use of the standard log-log plots,
to calculate its parameters.
F(X)=...
#P#u<#l<#b<Variables to be supplied by the user>>>#n
#p#b<XSize :>#NThe size modulus intercept on particle size axis at f(x)=1.
Cannot be zero.
#p#b<Slope :>#NA constant distribution modulus ( the slope of the log-log
plot).*/

pchar C2DGtGau::ParmDescs[GtGauNoOfParms] =
  {"XSize", "Slope", "Gain", "Offset" };

// =========================================================================
 
C2DGtGau::C2DGtGau(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  C2DModel(pClass_, pTag, pAttach, eAttach)
  {
  Clear();
  }
 
// -------------------------------------------------------------------------

C2DGtGau::~C2DGtGau()
  {
  }
 
//---------------------------------------------------------------------------
 
void C2DGtGau::Clear()
  {
  CBaseMdl::Clear();
  SetNParms(GtGauNoOfParms);
  Parms[0] = 1.0;  //XSize
  Parms[1] = 0.0;  //Slope
  Parms[2] = 1.0;  //Gain
  Parms[3] = 0.0;  //Offset
  }

// -------------------------------------------------------------------------
 
double C2DGtGau::Yx(double Xi)
  {
  return (Pow(Xi / Parms[0], Parms[1]) * Parms[2]) + Parms[3];
  }

//---------------------------------------------------------------------------
// =========================================================================
