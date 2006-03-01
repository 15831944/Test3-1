//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdarg.h>
#include <sc_defs.h>

#define __FE_HYD_CPP

#include "fe_hyd.h"
#include "models.h"

//==========================================================================
//
//
//
//==========================================================================
/*#D:#T:Hydrate DP
#X:#n#h<General Description>#N#NThis determines if hydrates will form in
a pipe.  The calculations are based on the temperature and pressure in the pipe
and a curve supplied by the user.  This curve shows temperature vs pressure for
the hydrate formation zone of a given gas SG.  If the conditions lie within the
hydrate formation zone the model checks the concentration of MEG in the pipeline.
If the concentration is less than required to inhibit hydrate formation the line
will slowly become blocked.
If the conditions do not lie within he hydrate formation zone, or there is sufficient
MEG to prevent hydrates, the model calculates the pressure drop within the pipe
according to the Darcy flow model.  The Churchill equation has been used
to calculate the Darcy pressure drop.  Ref: Churchill, S.W., Friction factor
equation spans all flow regimes, Chem. Eng., Nov. 7, 1977.

#n#n#h<Variables to be supplied by the user>
#n#i<Roughn :>The average roughness of the pipe.  The default is 1 mm
#n#i<Diam :>The inner pipe diameter. The default is 100 mm.
#n#i<Act_Len :>The actual pipe length that is required.  The default is 1 m
#n#i<Fit_Len :>The additional length of pipe, that is equivalent to the
resistance caused by any bends or other modifications to the originally straight
pipe. The default is 1 m.
#n#i<Force :>Once the hydrates have completely blocked the pipe, no flow is possible
within the pipe.  Therefore, to break the blockage the user may set this function to
a value greater than 0.  This will 'force' a hole in the hydrates and if there is MEG
present the hydrates will slowly dissolve.
#n#i<HCurve :>The curve showing the relationship between pressure and temperature
required for hydrate formation.

#n#n#h<Variables calculated by the model>
#n#i<Hydrates :> This shows the precentage of the line which is blocked with hydrates.
Once this value reaches 100% the line will no longer flow.
#n#i<Regime :>The type of flow of the fluid within the pipe :
#n#i<Laminar>A flow in which each particle of the fluid follows a
smooth path and the paths do not cross over one another.
#n#i<Turbulent>A flow characterised by small whirlpool-like currents or
eddies which absorb a large amount of energy and increase the viscosity
( internal friction ) of the fluid.

#n#h<Other>#n
Default model prefix:P#n
Short name:FE_Hyd#n
Model type:Link
#G:Links

*/
//==========================================================================

SPECIEBLK_L(H2O,      "H2O",      false);
SPECIEBLK_L(MEG,      "MEG",      false);
SPECIEBLK_L(Methanol, "Methanol", false);

IMPLEMENT_FLWEQN(FE_Hyd, PipeGroup.Name(), "FE_Hyd", "", TOC_ALL|TOC_GRP_ENERGY|TOC_USER,
                 "Hydrate DP",
                 "Unit to simulate hydrate formation in a pipe");

FE_Hyd::FE_Hyd(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  FlwEqn(pClass_, pTag, pAttach, eAttach),
  HCurve("DataOnly", "HCurve", this, TOA_Embedded, "Pressure", "Temperature")
  {
  Diam         = 0.1;//100 mm
  Res_Magn_Fac = 1.0;
  Res_Powr_Fac = 1.0;
  Age_Res_Fac  = 1.0;
  Rough        = 0.00001;// 0.01 mm
  Viscosity    = 0.001; // Water = 1cP = 0.001 Ns/m^s
  Len_Rqd      = 1.0;//1 m
  Len_Fit      = 0.0;// 1 m
  LoLimVel     = 0.1;
  Block        = 0.0;
  Force        = 0.0;
  Resistance   = 100.0;

  dFreezeRate  = 0.01;
  dMeltRate    = 0.01;

  }

//--------------------------------------------------------------------------

FE_Hyd::~FE_Hyd()
  {
  }

//--------------------------------------------------------------------------

#define RQDLEN (PrjFileVerNo()<36? "Act_Len" : "Rqd_Len")

void FE_Hyd::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.Double ("PipeRoughness","Roughn"    ,DC_L   , "mm"    , &Rough       ,this  ,isParm);
  DDB.Double ("Diameter"     ,"Diam"      ,DC_L   , "mm"    , &Diam        ,this  ,isParm);
  DDB.Double ("Length"       ,RQDLEN      ,DC_L   , "m"     , &Len_Rqd     ,this  ,isParmStopped);
  DDB.Double ("FittingLength","Fit_Len"   ,DC_L   , "m"     , &Len_Fit     ,this  ,isParmStopped);
  DDB.Double ("Viscosity"    ,"Viscosity" ,DC_Visc, "cP"    , &Viscosity   ,this  ,isParm);
  DDB.Text("");
  DDB.Double ("Resistance"  , "Resis"     ,DC_    , ""      , &Resistance  ,this  ,isParm);
  DDB.Double ("ForcingFunc"  ,"Force"     ,DC_Frac, "%"     , &Force       ,this  ,isParm);
  DDB.Double ("HydrateForm"  ,"Hydrates"  ,DC_Frac, "%"     , &Block       ,this  ,isParm);


  DDB.Double ("FreezeRate"   ,""          ,DC_Frac, "%"     , &dFreezeRate ,this  ,isParm);
  DDB.Double ("MeltRate"     ,""          ,DC_Frac, "%"     , &dMeltRate   ,this  ,isParm);

  DDB.Object(&HCurve,  this, NULL, NULL, DDB_RqdPage);
  };

//--------------------------------------------------------------------------

flag FE_Hyd::ValidateData(ValidateDataBlk & VDB)
  {
  HSpline.Clear();

  for (int i=0; i<HCurve.GetLen() ; i++)
    HSpline.AddPt(HCurve.Xp(i), HCurve.Yp(i));

  HSpline.SetSigma(1.0);
  HSpline.Fit();

//  if (!PhDOK())
//    return true;
  return True;
  }

//--------------------------------------------------------------------------

//void FE_Hyd::GetPhysData(FBPhysData & D, double DZ)
//  {
//  D.SetActLength(Max(fabs(DZ), Len_Rqd));
//  D.SetFitLength(0.0);
//  D.dArea=Sqr(Diam)*PI/4.0;
//  };
//double ::GetActualLength(double DZ)
//  { return Max(fabs(DZ), Len_Rqd); };
//double FE_Hyd::GetExtraLength()
//  { return 0.0; };
//double FE_Hyd::GetActualArea()
//  { return Sqr(Diam)*PI/4.0; };
//
//--------------------------------------------------------------------------


/*void FE_Hyd::StartSolution(rFlwEqnBlk FE)
  {
  FE.SetLength(Max(Max(fabs(FE.DZ()), Len_Rqd)+Len_Fit, 0.1*Diam));
  };*/


//--------------------------------------------------------------------------

double FE_Hyd::dP_Vel_Fn(double Vel, double Rho, double Length)
  {
  double Re, SG, Age_Fac, pwrFac;
  Re      = (Diam*Vel*Rho)/Viscosity;        //Reynold's number
  SG      = Rho / 1000.0;
  Age_Fac = 1.0 + 0.03 * Age_Res_Fac;
  pwrFac  = 2.0 * Res_Powr_Fac;

  // Churchill equation Ref: Churchill, S.W., Friction factor equation spans all flow regimes,
  // Chem. Eng., Nov. 7, 1977.
  double A, B, F_Fac, const_num;
  A         = Pow((-2.457 * log(Pow(7.0/Re,0.9) + 0.27 * Rough/Diam)), 16);
  B         = Pow((37530.0/Re), 16);
  F_Fac     = 8 * Pow((Pow((8/Re), 12) + 1.0/Pow((A + B), 1.5)),0.0833);
  const_num = 2.0 * F_Fac * SG * Age_Fac * Length * Pow(Vel,pwrFac) / Diam;

  return const_num;
  }

//--------------------------------------------------------------------------
/* The following equation has been used to calculate the concentration of MEG
   required to inhibit hydrate formation.  Ref : ?? Gas Conditioning and Processing -
   The Basic Principles  */

double FE_Hyd::Inhibitor(double dT)
  {
  double MEGconc, MW;
  MW = SDB[MEG.li()].MoleWt();
  MEGconc = dT * MW/(2220.0 + dT * MW) * 100.0;

  return MEGconc;
  }
//--------------------------------------------------------------------------

flag FE_Hyd::EvaluateFlwEqn(CSpPropInfo *pProps, FlwBlkBase & FE, double Regulation, FBPhysData *pPhD0, FBPhysData *pPhD1)
  {
  /* Calculate hydrate forming conditions.  If in the hydrate zone, then close pipe,
     otherwise use Darcy to calculate pressure drop.*/
//SDB[i].IsLiq()

  double Rho = FE.MeanRho(pProps);
  Rho = Max(0.01, Rho);
  double ActTemp, PPress, Water;
  Hydrate = False;
  ActTemp = FE.MeanTemp(pProps);
  PPress  = FE.MeanFBPress();
  Water   = FE.Cd().Qm(H2O.li());
  if (Force > 0.0)
    {
    Block = Min(Force, 1.0);
    Force = 0.0;
    }
  if ((Water > 1e-6) || (Block > 0.0))
    {
    // Sufficient H2O, or blocked already
    double HydFormTemp = HSpline.Yx(PPress);

    double MethReqd  = 0.001;
    double MethActual = FE.Cd().SpecieConc(ActTemp, Methanol.li(), som_Liq);
    if (MethActual > MethReqd)
      {
      // Sufficient Methanol
      dChangeRate = -dMeltRate; //Melting - Regardless
      }
    else if (ActTemp < HydFormTemp)
      {
      // Bad Temp
    // Increase Hydrate if MEG too low, else leave Hydrate.
      double dT        = HydFormTemp - ActTemp + 4.0;
      double MEGReqd   = Inhibitor(dT);
      double MEGActual = FE.Cd().SpecieConc(ActTemp, MEG.li(), som_Liq);
      if ((MEGActual < MEGReqd) && (Water > 1e-6))
        {
        // Insufficient MEG
        dChangeRate = dFreezeRate; //Freezing up, pipe blocking
        }
      else
        {
        dChangeRate = 0.0; //MEG preventing further hydration, but temp not high enough to melt hydrate
        }
      }
    else
      {
      dChangeRate = -dMeltRate; //Temp above hydrate point - melts hydrate;
      }

    if ((Block >= 1.0))
      {
      Block = 1.0;
      FE.SetQmReqd(0.0);
      return True;
      }
    else if (Block <=0)
      {
      Block = 0.0;
      }
    else
      {
      // Block  = Max(Block, 0.01);
      FE.SetQmMeas(RangeFlow(FE.QmMeas(), 1.0e-6));
      FE.SetQmFree();
      FE.SetDPb(0.0, 0.0);
      FE.SetDPq(-FE.QmSign() * Resistance * FE.QmMeas() * Block, -FE.QmSign() * Resistance * Block);
      return True;
      }

    }

  // Not Blocked

  Block = 0.0;


  FE.SetQmFree();
  double radius  = Diam/2.0;
  double rhoArea = Rho * PI * Sqr(radius);
  double Vel1    = RangeFlow(FE.QmMeas()/rhoArea, LoLimVel);
  FE.SetQmMeas(Vel1 * rhoArea);
  double dQm  = FE.QmMeas() * 0.001;
  double Vel2 = (FE.QmMeas() + dQm)/rhoArea;

  double dPq1 = dP_Vel_Fn(Vel1, Rho, FE.TotLength());
  double dPq2 = dP_Vel_Fn(Vel2, Rho, FE.TotLength());

  FE.SetVelocity(FE.GetQm()/rhoArea);
  FE.SetDPq(-FE.QmSign() * dPq1, -FE.QmSign() * (dPq2 - dPq1)/dQm);


  return True;
  };

//==========================================================================

void FE_Hyd::StepFlowInfo(FlwBlkBase & FE)
  {
  FlwEqn::StepFlowInfo(FE);
  Block  = Block + ICGetTimeInc() * dChangeRate;
  Block  = Range(0.0, Block, 1.0);
  };

//==========================================================================

