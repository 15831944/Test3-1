//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdarg.h>

#define __FE_VEL_CPP

#include "fe_Vel.h"
#include "models.h"

//==========================================================================
//
//
//
//==========================================================================
/*#D:#T:DP Prop to Velocity
#X:#n#h<General Description>#N#NThis is a linear flow equation, as it assumes
that the pressure drop within a pipe is proportional to the velocity within
the pipe.  The pressure drop calculated using this assumption will be higher
than the actual result for velocities below 8m/s.  Once the velocity increases
above this value the calculated pressure drop falls far below the actual number.
#nThis assumption was based on the flow of water through a pipe.  If the
fluid is more viscous than water, ie slurries, than the user should increase
the modifier to increase the pressure drop.  For gases the modifier should
have a value less than 1.0.

#n#n#h<Variables to be supplied by the user>
#n#i<Diam :>The internal diameter of the pipe. The default is 100mm.
#n#i<Length :>The physical length of the pipe.  The default is 10 m
#n#i<Modifier :>This variable is used to increase or decrease the calculated
pressure drop. The default is 1.0 (for water)

#n#h<Other>#n
Default model prefix:P#n
Short name:FE_Vel#n
Model type:Link
#G:Links

*/
//==========================================================================

IMPLEMENT_FLWEQN(FE_Hyp, PipeGroup.Name(), "FE_Vel", "", TOC_ALL|TOC_GRP_ENERGY|TOC_USER,
                  "DP Prop to Velocity",
                  " Pressure drop relationship dP proportional to velocity");

FE_Hyp::FE_Hyp(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  FlwEqn(pClass_, pTag, pAttach, eAttach)
  {
  Mod     = 1.0;
  Diam    = 0.1;
  Len_Rqd = 10.0;
  }

//--------------------------------------------------------------------------

FE_Hyp::~FE_Hyp()
  {
  }

//--------------------------------------------------------------------------

void FE_Hyp::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.Double ("Diameter",   "Diam",     DC_L,  "mm",    &Diam      ,this  ,isParm);
  DDB.Double ("Length",     "Length",   DC_L,  "m",     &Len_Rqd   ,this  ,isParmStopped);
  DDB.Double ("Modifier",   "Modifier", DC_,   "",      &Mod       ,this  ,isParm);
  };

//--------------------------------------------------------------------------

flag FE_Hyp::EvaluateFlwEqn(CSpPropInfo *pProps, FlwBlkBase & FE, double Regulation, FBPhysData *pPhD0, FBPhysData *pPhD1)
  {
  double radius, Rho, rhoArea, Vel1, Vel2, dQm, dPq1, dPq2;
  Rho = FE.MeanRho(pProps);
  Rho = Max(0.01, Rho);
  radius  = Diam/2.0;
  rhoArea = Max(Rho * PI * Sqr(radius),1.0e-6);

  FE.SetQmMeas(RangeFlow(FE.QmMeas(), 1.0e-6));
  Vel1    = FE.QmMeas()/rhoArea;
  FE.SetQmMeas(Vel1 * rhoArea);
  dQm     = FE.QmMeas() * 0.001;
  Vel2    = (FE.QmMeas() + dQm)/rhoArea;

  dPq1    = 6.75 * Vel1 * FE.TotLength() * Mod;
  dPq2    = 6.75 * Vel2 * FE.TotLength() * Mod;

  FE.SetVelocity(FE.GetQm()/rhoArea);
  FE.SetDPq(-FE.QmSign() * dPq1, -FE.QmSign() * (dPq2 - dPq1)/dQm);

  return True;
  };

//============================================================================
