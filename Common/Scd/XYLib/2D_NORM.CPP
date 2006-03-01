//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992
 
#include "stdafx.h"
#define __2D_NORM_CPP
#include "sc_defs.h"
#include "2d_fn.h"
#include "2d_norm.h"

// =========================================================================

IMPLEMENT_2D_MODEL(C2DNorm, "2D_Norm", "", TOC_SYSTEM, "Normal Distribution", "Normal Size Distribution");

// =========================================================================
/*#D:#T:Normal Distribution
#X:#n#u<#l<#b<General Description>>>#PThis fits the normal distribution
curve to data, for example when determining the particle size distribution
of granular materials, according tho the following formula :
dF/dX)= ...
#P#u<#l<#b<Variables to be supplied by the user>>>#n
#p#b<Std Dev :>#NThe standard deviation of dF/dX. This cannot be zero.
#p#b<Mean :>#NThe mean of dF/dX.
*/

double C2DNorm::Const1 = 1.0 / (sqrt(2 * PI));

pchar C2DNorm::ParmDescs[NormNoOfParms] =
  {"Std_Dev", "Mean", "Gain", "Offset" };

// =========================================================================
 
C2DNorm::C2DNorm(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  C2DModel(pClass_, pTag, pAttach, eAttach)
  {
  Clear();
  }
 
// -------------------------------------------------------------------------
 
C2DNorm::~C2DNorm()
  {
  }
 
//---------------------------------------------------------------------------
 
void C2DNorm::Clear()
  {
  CBaseMdl::Clear();
  SetNParms(NormNoOfParms);
  Parms[0] = 1.0;  //StdDev
  Parms[1] = 0.0;  //Mean
  Parms[2] = 1.0;  //Gain
  Parms[3] = 0.0;  //Offset
  }

// -------------------------------------------------------------------------

flag C2DNorm::InitParms(flag DoIt)
  {
  if (DoIt)
    {
    double MaxY = 0.0;
    const long Pts = NPts();
    for (long i=0; i<Pts; i++)
      {
      if (pFn->Yp(i) > MaxY)
        MaxY = pFn->Yp(i); 
      }
    double Variance = 0.0;
    for (i=0; i<Pts; i++)
      Variance = Variance + Sqr(MaxY - pFn->Yp(i));
    Parms[0] = sqrt(Variance);
    Parms[1] = MaxY;
    Parms[2] = 1.0;
    Parms[3] = 0.0;
    }
  return True;
  }

// -------------------------------------------------------------------------
 
double C2DNorm::Yx(double Xi)
  {
  return (Const1 / Parms[0] * exp(- Sqr((Xi-Parms[1]) / (2 * Parms[0] * Parms[0]))) * Parms[2]) + Parms[3];
  }

//---------------------------------------------------------------------------
// =========================================================================
