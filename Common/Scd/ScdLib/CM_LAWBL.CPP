//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992,1994
 
#include "stdafx.h"
#include <math.h>

#define __CM_LAWBL_CPP

#include "cm_lawbl.h"

#define dbgEst

const double ar_Perturb = 1.0e-5;

//==========================================================================
//
//
//
//==========================================================================

CMFLawBailey::CMFLawBailey(CCMinFinder *pFn_, double Tol/*=1.0e-6*/, int ItMax_/*=100*/)
  {
  pFn=pFn_;
  dTol=Tol;
  iItMax=ItMax_;
  };


// -------------------------------------------------------------------------

double CMFLawBailey::SetRestrParms(double Alpha, CDVector & RestrParms, CDVector & DeltaParms)
  {
  double RetAlpha=Alpha;
  //DeNormalise_Parms(UserParms, RestrParms);

  for (int i=0; i<NumParmsUsed; i++)
    {
    RestrParms[i] = NormParms[i] + Alpha*DeltaParms[i];
    double a=0.0;
    if (RestrParms[i] > NParmMaxs[i])
      a=(NParmMaxs[i]-NormParms[i])/DeltaParms[i];
    else if (RestrParms[i] < NParmMins[i])
      a=(NParmMins[i]-NormParms[i])/DeltaParms[i];
    if (a>0.0 && a<RetAlpha)
      RetAlpha=a;
    }
  
  for (i=0; i<NumParmsUsed; i++)
    RestrParms[i] = NormParms[i] + RetAlpha*DeltaParms[i];

  //Normalise_Parms(UserParms, RestrParms);
  
  return RetAlpha;
  }

// -------------------------------------------------------------------------
/*
flag CMFLawBailey::ConstructLBMatrix(CDVector &NParms, CDVector &ParmTols, CDMatrix &Z, CDVector &C)
  {
  static CDVector Derivs;

  DeNormalise_Parms(UserParms, NParms);

  Derivs.SetSize(NumParmsUsed);
  Derivs.SetAll(0.0);
  C.SetSize(NumParmsUsed);
  C.SetAll(0.0);
  Z.SetSize(NumParmsUsed, NumParmsUsed);
  Z.SetAll(0.0);
  
  pFn->CostFn(UserParms); //??
  //Find the residuals
  for (int i=0; i<Targets.GetSize(); i++)
    {
    //Get all derivs at data point i
    for (long k=0; k<NumParmsUsed; k++)
      Derivs[k] = EvalDeriv(UserParms, ParmTols, i, k);
 
    Derivs.dbgDump("Derivs");
    // Calc Targets At Parms
    SetParams(Parms);
    SolveProblem();
    GetTargets();
    for (k=0; k<NumParmsUsed; k++)
      C[k] += -(Targets[i].Exp()-Targets[i].Val()) * (fabs(Derivs[k]) / StdD[i]);
    //Construct the matrix
    for (long NRow=0; NRow<NumParmsUsed; NRow++)
      for (long NCol=0; NCol<NumParmsUsed; NCol++)
        Z[NRow][NCol] += (Derivs[NRow] * Derivs[NCol]);
    }

  return True;
  };
*/
// -------------------------------------------------------------------------
// LawAndBailey is the Law and Bailey regression routine.                      
// On entry, B contains initial guesses for the parameters, BTol contains the  
// required tolerances, F is the equation to be used, S is the sum of squares  
// routine to be used, NParm is the number of parameters to estimate, Np is the
// number of data points in the data set.                                      
// On exit, B contains the estimated parameter values, Sa contains the sum of  
// squares at the minimu, and Iter contains the number of iterations performed 
int CMFLawBailey::Solve()//CDVector &DeltaParms, double &Sa) 
  {
  //CRegStuff R;
  InitParameters();

  DeltaParms.SetSize(NumParmsUsed);
  NParmMins.SetSize(NumParmsUsed);
  NParmMaxs.SetSize(NumParmsUsed);

  for (int t=0; t<NumParms; t++)
    {
    ParmTols[t]=Max(1.0e-6, UserParms[t]*1.0e-4);
    }

  DeNormalise_Parms(ParmMins, NParmMins);
  DeNormalise_Parms(ParmMaxs, NParmMaxs);

  CDVector RestrParms(NumParmsUsed);
  CDMatrix Z(NumParmsUsed, NumParmsUsed);
  CDVector C(NumParmsUsed);
  double SaPrv1 = 1.0e+30;
  double SaPrv2 = 1.0e+30;
  double SaPrv3 = 1.0e+30;
  double SaInit = 0.0;
  //DeltaB.SetSize(NParm, 0.0);
  flag Converged = False;
  CLVector Index(NumParmsUsed);
  int D;
  iIter = 0;
  #ifdef dbgEst
  dbgpln("------------** START LBEST **---------------------------------------------");
  //dbgDump();
  UserParms.dbgDump("Parms");
  ParmMins.dbgDump("ParmMins");
  ParmMaxs.dbgDump("ParmMaxs");
  ParmTols.dbgDump("ParmTols");
  //StdD.dbgDump("StdD    ");
  dbgpln("------------");
  #endif
  DeNormalise_Parms(UserParms, NormParms);
  SaInit = pFn->CostFn(UserParms);
  #ifdef dbgEst
  UserParms.dbgDump("Parms");
  dbgpln("CostFn       %14.6g",SaInit);
  #endif

  Sa=SaInit; // CNM
  double SaTol =1.0e-3;
  double Alpha = 1.0;
  double Beta = 0.25;

  RestrParms=NormParms;
  do
    {
    if (Sa < SaTol)
      {
      Converged = True;
      #ifdef dbgEst
      UserParms.dbgDump("Parms");
      dbgpln("Converged Sa:%10g -----------", Sa);
      #endif
      break;
      }

    dbgpln("-+- %d -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+", iIter);
    #ifdef dbgEst
    UserParms.dbgDump("Parms");
    dbgpln("Sa:%10g", Sa);
    dbgpln("Alpha:%10g", Alpha);
    #endif
    DeNormalise_Parms(UserParms, NormParms);
    pFn->ConstructLBMatrix(UserParms, ParmTols, Z, C);
    #ifdef dbgEst
    Z.dbgDump("Z:");
    C.dbgDump("C:");

    #endif
    DeltaParms = C;
    if (Z.LUDecompose(Index, D))
      Z.LUBackSub(Index, DeltaParms);
    else 
      return False; //probably singular !!!
    //Invert, solution was returned in DeltaParms
    iIter++;
    double Dt = 0.0;
    for (long i=0; i<NumParmsUsed; i++)
      Dt += (C[i] * DeltaParms[i]);
    if (Dt<0.0)
      {
      Dt = -Dt;
      DeltaParms.Mult(-1.0);
      }

    #ifdef dbgEst
    DeltaParms.dbgDump("DeltaParms");
    dbgpln("Dt:%10g", Dt);
    #endif

    //Be optimistic !
    Converged = True;

    Alpha=SetRestrParms(Alpha, RestrParms, DeltaParms);
    DeNormalise_Parms(UserParms, RestrParms);
    double SaA = pFn->CostFn(UserParms);
    #ifdef dbgEst
    RestrParms.dbgDump("RestrParms");
    dbgpln("CostFn       %14.6g",SaA);
    #endif

    SetRestrParms(Alpha*0.999, RestrParms, DeltaParms);
    DeNormalise_Parms(UserParms, RestrParms);
    double SaA1 = pFn->CostFn(RestrParms);
    #ifdef dbgEst
    dbgpln("CostFn       %14.6g",SaA1);
    #endif
    
    if (SaA1 < SaA) // ie past minimum
      {
      do 
        {
        Sa=SaA;
        Alpha=Alpha*0.5;
        Alpha=SetRestrParms(Alpha, RestrParms, DeltaParms);
        DeNormalise_Parms(UserParms, RestrParms);
        SaA = pFn->CostFn(UserParms);
        #ifdef dbgEst
        dbgpln("Alpha/2  %10g Sa:%10g", Alpha, SaA);
        #endif
        }
      while (SaA<Sa && Alpha > 1.0e-6);
      Alpha=Alpha*2.0;
      Alpha=SetRestrParms(Alpha, RestrParms, DeltaParms);
      }
    else
      {
      do 
        {
        Sa=SaA;
        Alpha=Alpha*2.0;
        Alpha=SetRestrParms(Alpha, RestrParms, DeltaParms);
        DeNormalise_Parms(UserParms, RestrParms);
        SaA = pFn->CostFn(UserParms);
        #ifdef dbgEst
        dbgpln("Alpha*2  %10g Sa:%10g", Alpha, SaA);
        #endif
        }
      while (SaA<Sa);
      Alpha=Alpha/2.0;
      Alpha=SetRestrParms(Alpha, RestrParms, DeltaParms);
      }
    NormParms=RestrParms;
    }
  while (iIter <= iItMax);
  DeNormalise_Parms(UserParms, NormParms);
  Sa = pFn->CostFn(UserParms); // Restore State of Model
  
  #ifdef dbgEst
  dbgpln("------------");
  dbgpln("Converged:%d", Converged);
  //dbgDump();
  dbgpln("------------** END LBEST **---------------------------------------------");
  #endif
  return True;
  }

//==========================================================================
