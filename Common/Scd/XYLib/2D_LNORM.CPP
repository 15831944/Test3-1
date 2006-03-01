//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992
 
#include "stdafx.h"
#define __2D_LNORM_CPP
#include "sc_defs.h"
#include "2d_fn.h"
#include "2d_lnorm.h"

// =========================================================================

IMPLEMENT_2D_MODEL(C2DLNorm, "2D_LNorm", "", TOC_SYSTEM, "Log-Normal Distribution", "Log-Normal Size Distribution");

// =========================================================================
/*#D:#T:Log-Normal Distribution
#X:#n#u<#l<#b<General Description>>>#PThis fits the log-normal distribution
curve to data, for example when determining the particle size distribution
of granular materials, according tho the following formula :
dF/d(lnX)=XdF/dX= 1/ln(...
#P#u<#l<#b<Variables to be supplied by the user>>>#n
#p#b<Gm Std Dev :>#NThe geometrical standard deviation of dF/dX, equivalent to
X84% / X50% ie the particle size fraction at 84 % divided by the particle
size fraction at 50%. This cannot be zero.
#p#b<Gm Mean :>#NThe geometrical mean of dF/dX.
*/

double C2DLNorm::Const1 = 1.0 / (sqrt(2 * PI));

pchar C2DLNorm::ParmDescs[LNormNoOfParms] =
  {"Gm_Std_Dev", "Gm_Mean", "Gain", "Offset" };

// =========================================================================
 
C2DLNorm::C2DLNorm(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  C2DModel(pClass_, pTag, pAttach, eAttach)
  {
  Clear();
  }
 
// -------------------------------------------------------------------------
 
C2DLNorm::~C2DLNorm()
  {
  }
 
//---------------------------------------------------------------------------
 
void C2DLNorm::Clear()
  {
  CBaseMdl::Clear();
  SetNParms(LNormNoOfParms);
  Parms[0] = 1.0; //GStdDev
  Parms[1] = 0.5; //GMean
  Parms[2] = 1.0; //Gain
  Parms[3] = 0.0; //Offset
  }

// -------------------------------------------------------------------------
 
double C2DLNorm::Yx(double Xi)
  {
  Parms[0]=Max(Parms[0], 1.0e-10);
  Parms[1]=Max(Parms[1], 1.0e-10);
  double a = log(Parms[0]);
  double b = log(Parms[1]);
  return (Const1 / GTZ(a) * Exps(- Sqr((log(GTZ(Xi))-b) / GTZ(2. * a * a))) * Parms[2]) + Parms[3];
  }

//---------------------------------------------------------------------------
// =========================================================================
