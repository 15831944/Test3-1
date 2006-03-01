//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992,1994

#include "stdafx.h"
#include <math.h>

#define __CM_FLEXI_CPP

#include "cm_flexi.h"

#define dbgEst

#define Alfa 1.0
#define Beta .5
#define Gama 2.0

//==========================================================================
//
//
//
//==========================================================================

/*        ************ Program FLEXIPLEX ************

ProbNo   the problem type to be solved
NumParmsUsed total number of Independant variables
NC       total number of equality constraints
NIC      total number of Inequality constraints
Size     edge length of the Initial polyhedron
Conver   Convergence criterion for termination of the search
Alfa     the reflection coefficient
Beta     the contraction coefficient
Gama     the expansion coefficient
Solution(I) the assumed vector to Initiate the search
Fdifer   the tolerance criterion for constraint violation
Icont    a counter to record stage violations
Ncont    a counter to print Information every (NX+1) stage
Low      an Index to Identify Information related to the Lowest value
         of obj. function In most recent polyhedron
Lhigh    an Index to Identify Information related to largest value of
         obj. function In most polyhedron
Lsec     an Index to Identify Information related to the second
         largest value of obj. function In most recent polyhedron

                       ***************
*/
//==========================================================================
//
//
//
//==========================================================================

CMFFlexiPlex::CMFFlexiPlex(CCMinFinder * pFn_, double Tol/*=1.0e-6*/ , int ItMax_/*=100*/ )
  {
  pFn = pFn_;

  NumParms = NumParmsUsed = 0;
  DoParmsRange = 0;

  StepSize = 0.1;
  ConverTol = 1.0e-3;
  
  NC = NIC = 0;

  I = J = II = IIN = INF = Icont = IterCnt = Ncont = IterCost = 0;
  K1 = K2 = K3 = K4 = K5 = K6 = K7 = K8 = K9 = 0;
  L5 = L6 = L7 = L8 = L9 = Lhigh = Low = Lsec = 0;
  N1 = N2 = N3 = N4 = N5 = N6 = N7 = N8 = N9 = N = 0;


  ETA = Fdifer = FH = FL = Fold = FS = Fter = R1A = R2A = R3A = 0.0;
  Scale = Seql = Step = Step1 = Step2 = Sum2 = 0.0;

  Restart = AbortFit = TraceReqd = 0;
  CalcErr = 0;

  };


//==========================================================================

void CMFFlexiPlex::DoIO()
  {
  dbgpln("Iter %4i, FDifer %14.6g  ObjFn %14.6g", IterCnt, Fdifer, Residual[(K9)-1]);
  dbgp("           UserParms ");
  for (int i=0; i<NumParms; i++)
    dbgp(" %14.6g", UserParms[i]);
  dbgpln("");
  dbgp("           Resid ");
  for (i=0; i<NIC+NC; i++)
    dbgp(" %14.6g", Residual[i]);
  dbgpln("");


  };
//void Sumr() {};
//void Feasbl() {};
void ManualInt()
  {
  };
void UpdateCostCount()
  {
  };


//==========================================================================

void CMFFlexiPlex::Start()
  {
  int I, J, L;
  double Step1, Step2;
  CDMatrix A;
  A.SetSize(K1, NumParmsUsed);

  Step1 = Step / (NumParmsUsed * Sqrt(2.0)) *(Sqrt(NumParmsUsed + 1.0) + NumParmsUsed - 1.0);
  Step2 = Step / (NumParmsUsed * Sqrt(2.0)) *(Sqrt(NumParmsUsed + 1.0) - 1.0);
  for (J = 1; J <= NumParmsUsed; J++)
    A[ (1) - 1][ (J) - 1] = 0.0;
  for (I = 2; I <= K1; I++)
    {
    for (J = 1; J <= NumParmsUsed; J++)
      A[ (I) - 1][ (J) - 1] = Step2;
    L = I - 1;
    A[ (I) - 1][ (L) - 1] = Step1;
    }
  for (I = 1; I <= K1; I++)
    for (J = 1; J <= NumParmsUsed; J++)
      X1[ (I) - 1][ (J) - 1] = NormParms[ (J) - 1] + A[ (I) - 1][ (J) - 1];
  }

//==========================================================================
//  computes the sum of the square values of the violated constraints
//  In order to be compared with the tolerance criterion.

void CMFFlexiPlex::Sumr()
  {

  int J;
  Sum[ (IIN) - 1] = 0;
  DeNormalise_Parms(UserParms, NormParms);
  pFn->InEqualityConstraints(UserParms, Residual);
  Seql = 0;
  if (NIC != 0)
    for (J = K7; J <= K8; J++)
      if (Residual[ (J) - 1] < 0)
        Seql = Seql + Residual[ (J) - 1] * Residual[ (J) - 1];
  if (NC != 0)
    {
    pFn->EqualityConstraints(UserParms, Residual);
    for (J = 1; J <= NC; J++)
      Seql = Seql + Residual[ (J) - 1] * Residual[ (J) - 1];
    }
  Sum[ (IIN) - 1] = Seql;
  }

//==========================================================================
//   subroutine feasbl minimizes the sum of the square values of the
//   violated constraints.  It Is called every time the combined value
//   of the violated constraints exceeds the value of the tolerance
//   criterion for the current stage.

void CMFFlexiPlex::Feasbl()
  {
  int Ichek, Index, Kount, I, J; //, K, L;
  double Alfa1, Beta1, Difer, Factor, Fint, Ratio, Sums, Suml, Sumh;
  double Flg[10];
  CDVector R1, R2, R3, H;

  H.SetSize(NumParmsUsed);

  IterCnt = IterCnt + 1;
  Icont = 0;
  Ichek = 0;
L310 :
  Start();
  for (I = 1; I <= K1; I++)
    {
    for (J = 1; J <= NumParmsUsed; J++)
      NormParms[ (J) - 1] = X1[ (I) - 1][ (J) - 1];
    IIN = I;
    Sumr();
    }
  // select largest value of sum[i] In simplex
L320 :
  Sumh = Sum[ (1) - 1];
  Index = 1;
  for (I = 2; I <= K1; I++)
    if (Sum[ (I) - 1] > Sumh)
      {
      Sumh = Sum[ (I) - 1];
      Index = I;
      }

  // select minimum value of sum[i] In simplex
  Suml = Sum[ (1) - 1];
  Kount = 1;
  for (I = 2; I <= K1; I++)
    if (Sum[ (I) - 1] < Suml)
      {
      Suml = Sum[ (I) - 1];
      Kount = I;
      }
  // find centroid of points with I different than Index
  for (J = 1; J <= NumParmsUsed; J++)
    {
    Sum2 = 0.0;
    for (I = 1; I <= K1; I++)
      Sum2 = Sum2 + X1[ (I) - 1][ (J) - 1];
    X1[ (K2) - 1][ (J) - 1] = 1.0 / NumParmsUsed * (Sum2 - X1[ (Index) - 1][ (J) - 1]);
    X1[ (K3) - 1][ (J) - 1] = 2.0 * X1[ (K2) - 1][ (J) - 1] - X1[ (Index) - 1][ (J) - 1];
    NormParms[ (J) - 1] = X1[ (K3) - 1][ (J) - 1];
    }
  IIN = K3;
  Sumr();
  if (Sum[ (K3) - 1] < Suml)
    goto L330;

  // select second largest value In simplex
  if (Index == 1)
    Sums = Sum[ (2) - 1];
  else
    Sums = Sum[ (1) - 1];
  for (I = 1; I <= K1; I++)
    if ((Index - I) != 0)
      if (Sum[ (I) - 1] > Sums)
        Sums = Sum[ (I) - 1];
  if (Sum[ (K3) - 1] > Sums)
    goto L340;
  else
    goto L390;

//  form expansion of new minimum If reflection has produced one minimum
L330 :
  for (J = 1; J <= NumParmsUsed; J++)
    {
    X1[ (K4) - 1][ (J) - 1] = X1[ (K2) - 1][ (J) - 1] + 2.0 * (X1[ (K3) - 1][ (J) - 1] - X1[ (K2) - 1][ (J) - 1]);
    NormParms[ (J) - 1] = X1[ (K4) - 1][ (J) - 1];
    }
  IIN = K4;
  Sumr();
  if (Sum[ (K4) - 1] < Suml)
    goto L380;
  else
    goto L390;
L340 :
  if (Sum[ (K3) - 1] > Sumh)
    goto L350;
  for (J = 1; J <= NumParmsUsed; J++)
    X1[ (Index) - 1][ (J) - 1] = X1[ (K3) - 1][ (J) - 1];
L350 :
  for (J = 1; J <= NumParmsUsed; J++)
    {
    X1[ (K4) - 1][ (J) - 1] = 0.5 * X1[ (Index) - 1][ (J) - 1] + 0.5 * X1[ (K2) - 1][ (J) - 1];
    NormParms[ (J) - 1] = X1[ (K4) - 1][ (J) - 1];
    }
  IIN = K4;
  Sumr();
  if (Sumh > Sum[ (K4) - 1])
    goto L370;

  // reduce simplex by half If reflection happens 
  // to produce a larger value than the maximum 

  for (J = 1; J <= NumParmsUsed; J++)
    for (I = 1; I <= K1; I++)
      X1[ (I) - 1][ (J) - 1] = 0.5 * (X1[ (I) - 1][ (J) - 1] +
        X1[ (Kount) - 1][ (J) - 1]);
  for (I = 1; I <= K1; I++)
    {
    for (J = 1; J <= NumParmsUsed; J++)
      NormParms[ (J) - 1] = X1[ (I) - 1][ (J) - 1];
    IIN = I;
    Sumr();
    }
L360 :
  Suml = Sum[ (1) - 1];
  Kount = 1;
  for (I = 2; I <= K1; I++)
    if (Suml >= Sum[ (I) - 1])
      {
      Suml = Sum[ (I) - 1];
      Kount = I;
      }
  
  //writedbg(RStr(Sum[Kount],6,-1)+' ');

  SR[ (INF) - 1] = Sqrt(Max(1.0e-50, Sum[ (Kount) - 1]));
  for (J = 1; J <= NumParmsUsed; J++)
    NormParms[ (J) - 1] = X1[ (Kount) - 1][ (J) - 1];
  goto L400;
L370 :
  for (J = 1; J <= NumParmsUsed; J++)
    X1[ (Index) - 1][ (J) - 1] = X1[ (K4) - 1][ (J) - 1];
  Sum[ (Index) - 1] = Sum[ (K4) - 1];
  goto L360;
L380 :
  for (J = 1; J <= NumParmsUsed; J++)
    {
    X1[ (Index) - 1][ (J) - 1] = X1[ (K4) - 1][ (J) - 1];
    NormParms[ (J) - 1] = X1[ (Index) - 1][ (J) - 1];
    }
  Sum[ (Index) - 1] = Sum[ (K4) - 1];
  SR[ (INF) - 1] = Sqrt(Max(1.0e-50, Sum[ (K4) - 1]));
  goto L400;
L390 :
  for (J = 1; J <= NumParmsUsed; J++)
    {
    X1[ (Index) - 1][ (J) - 1] = X1[ (K3) - 1][ (J) - 1];
    NormParms[ (J) - 1] = X1[ (Index) - 1][ (J) - 1];
    }
  Sum[ (Index) - 1] = Sum[ (K3) - 1];
  SR[ (INF) - 1] = Sqrt(Max(1.0e-50, Sum[ (K3) - 1]));
L400 :
  Icont = Icont + 1;
  for (J = 1; J <= NumParmsUsed; J++)
    X2[ (INF) - 1][ (J) - 1] = NormParms[ (J) - 1];
  if (Icont < 2 * K1)
    goto L500;
  Icont = 0;
  for (J = 1; J <= NumParmsUsed; J++)
    NormParms[ (J) - 1] = X1[ (K2) - 1][ (J) - 1];
  IIN = K2;
  Sumr();
  Difer = 0;
  for (I = 1; I <= K1; I++)
    Difer = Difer + Sqr(Sum[ (I) - 1] - Sum[ (K2) - 1]);
  Difer = 1.0 / (K7 * NumParmsUsed) * Sqrt(Max(1.0e-50, Difer));

  if (Difer > 1.0E-14)
    goto L500;

  // If flexible simplex method failed to satisfy the constraints
  // within the tolerance criterion for the current stage,  the search
  // Is perturbed from the position where the x vector Is stuck and
  // then Feasbl Is repeated once more from the beginning

  IIN = K1;
  Step = 20.0 * Fdifer;
  Sumr();

  SR[ (INF) - 1] = Sqrt(Max(1.0e-50, Seql));
  for (J = 1; J <= NumParmsUsed; J++)
    X1[ (K1) - 1][ (J) - 1] = NormParms[ (J) - 1];
  for (J = 1; J <= NumParmsUsed; J++)
    {
    Factor = 1.0;
    NormParms[ (J) - 1] = X1[ (K1) - 1][ (J) - 1] + Factor * Step;
    X1[ (L9) - 1][ (J) - 1] = NormParms[ (J) - 1];
    IIN = L9;
    Sumr();
    NormParms[ (J) - 1] = X1[ (K1) - 1][ (J) - 1] - Factor * Step;
    X1[ (L5) - 1][ (J) - 1] = NormParms[ (J) - 1];
    IIN = L5;
    Sumr();
L410 :
    if (Sum[ (L9) - 1] < Sum[ (K1) - 1])
      goto L420;
    if (Sum[ (L5) - 1] < Sum[ (K1) - 1])
      goto L430;
    goto L440;
L420 :
    X1[ (L5) - 1][ (J) - 1] = X1[ (K1) - 1][ (J) - 1];
    Sum[ (L5) - 1] = Sum[ (K1) - 1];
    X1[ (K1) - 1][ (J) - 1] = X1[ (L9) - 1][ (J) - 1];
    Sum[ (K1) - 1] = Sum[ (L9) - 1];
    Factor = Factor + 1.0;
    NormParms[ (J) - 1] = X1[ (K1) - 1][ (J) - 1] + Factor * Step;
    IIN = L9;
    Sumr();
    goto L410;
L430 :
    X1[ (L9) - 1][ (J) - 1] = X1[ (K1) - 1][ (J) - 1];
    Sum[ (L9) - 1] = Sum[ (K1) - 1];
    X1[ (K1) - 1][ (J) - 1] = X1[ (L5) - 1][ (J) - 1];
    Sum[ (K1) - 1] = Sum[ (L5) - 1];
    Factor = Factor + 1.0;
    NormParms[ (J) - 1] = X1[ (K1) - 1][ (J) - 1] - Factor * Step;
    IIN = L5;
    Sumr();
    goto L410;

    // one dimensional search by golden section along each coordinate 
L440 :
    //kcg


    H[ (J) - 1] = X1[ (L9) - 1][ (J) - 1] - X1[ (L5) - 1][ (J) - 1];
    X1[ (L6) - 1][ (J) - 1] = X1[ (L5) - 1][ (J) - 1] + H[ (J) - 1] * R1A;
    NormParms[ (J) - 1] = X1[ (L6) - 1][ (J) - 1];
    IIN = L6;
    Sumr();
    X1[ (L7) - 1][ (J) - 1] = X1[ (L5) - 1][ (J) - 1] + H[ (J) - 1] * R2A;
    NormParms[ (J) - 1] = X1[ (L7) - 1][ (J) - 1];
    IIN = L7;
    Sumr();
    if (Sum[ (L6) - 1] > Sum[ (L7) - 1])
      goto L460;
    X1[ (L8) - 1][ (J) - 1] = X1[ (L5) - 1][ (J) - 1] + (1.0 - R3A) *
    H[ (J) - 1];
    X1[ (L5) - 1][ (J) - 1] = X1[ (L7) - 1][ (J) - 1];
    NormParms[ (J) - 1] = X1[ (L8) - 1][ (J) - 1];
    IIN = L8;
    Sumr();
    if (Sum[ (L8) - 1] > Sum[ (L6) - 1])
      goto L450;
    X1[ (L5) - 1][ (J) - 1] = X1[ (L6) - 1][ (J) - 1];
    Sum[ (L5) - 1] = Sum[ (L6) - 1];
    goto L480;
L450 :
    X1[ (L9) - 1][ (J) - 1] = X1[ (L8) - 1][ (J) - 1];
    Sum[ (L9) - 1] = Sum[ (L8) - 1];
    goto L480;
L460 :
    X1[ (L9) - 1][ (J) - 1] = X1[ (L6) - 1][ (J) - 1];
    X1[ (L8) - 1][ (J) - 1] = X1[ (L5) - 1][ (J) - 1] + R3A * H[ (J) - 1];
    NormParms[ (J) - 1] = X1[ (L8) - 1][ (J) - 1];
    IIN = L8;
    Sumr();
    Step = StepSize;
    Sum[ (L9) - 1] = Sum[ (L6) - 1];
    if (Sum[ (L7) - 1] > Sum[ (L8) - 1])
      goto L470;
    X1[ (L5) - 1][ (J) - 1] = X1[ (L8) - 1][ (J) - 1];
    Sum[ (L5) - 1] = Sum[ (L8) - 1];
    goto L480;
L470 :
    X1[ (L9) - 1][ (J) - 1] = X1[ (L7) - 1][ (J) - 1];
    Sum[ (L9) - 1] = Sum[ (L7) - 1];
L480 :
    if (fabs(X1[ (L9) - 1][ (J) - 1] - X1[ (L5) - 1][ (J) - 1]) > (0.01 * Fdifer))
      goto L440;
    X1[ (K1) - 1][ (J) - 1] = X1[ (L7) - 1][ (J) - 1];
    NormParms[ (J) - 1] = X1[ (L7) - 1][ (J) - 1];
    Sum[ (K1) - 1] = Sum[ (L5) - 1];

    SR[ (INF) - 1] = Sqrt(Max(1.0e-50, Sum[ (K1) - 1]));
    if (SR[ (INF) - 1] < Fdifer)
      goto L490;
    }
  Ichek = Ichek + 1;
  Step = Fdifer;
  if (Ichek <= 2)
    goto L310;
  Fold = 1.0E-12;
  if (CalcErr == 0)
    CalcErr = 11;
  if (TraceReqd)
    {
    /*

    WriteDbgLn('');
    WriteDbgLn('A constraint cannot be satisfied !');
    ShowMsg('A constraint cannot be satisfied. Choose another starting vector and repeat.', InfoMsg, NIL);
    FOR  J := 1 to NumParmsUsed  DO
      Begin
     NewDbgLn;
      WriteDbg('X('+IStr(J,2)+') '+ RStr(NormParms[J],14,7));   { ... Unsatisfiable Vectors  }
      Col := Col + 1;
      End;
    PopWarningMsg(@Wnd_User,'Press any key to continue');
    exit;
    */
    }
  goto L520;
L490 :
  for (J = 1; J <= NumParmsUsed; J++)
    {
    X2[ (INF) - 1][ (J) - 1] = X1[ (K1) - 1][ (J) - 1];
    NormParms[ (J) - 1] = X1[ (K1) - 1][ (J) - 1];
    }
L500 :
  if (SR[ (INF) - 1] > Fdifer)
    goto L320;

  //  modified lagrange Interpolation for tight Inequalities
  
  if (SR[ (INF) - 1] > 0)
    goto L600;
  UpdateCostCount();
  DeNormalise_Parms(UserParms, NormParms);
  Residual[ (K9) - 1] = pFn->CostFn(UserParms);
  DoIO();
  Fint = Residual[ (K9) - 1];
  for (J = 1; J <= NumParmsUsed; J++)
    NormParms[ (J) - 1] = X2[ (INF) - 1][ (J) - 1];
  DeNormalise_Parms(UserParms, NormParms);
  pFn->InEqualityConstraints(UserParms, Residual);
  for (J = K7; J <= K8; J++)
    R1[ (J) - 1] = Residual[ (J) - 1];
  for (J = 1; J <= NumParmsUsed; J++)
    NormParms[ (J) - 1] = X1[ (Kount) - 1][ (J) - 1];
  DeNormalise_Parms(UserParms, NormParms);
  pFn->InEqualityConstraints(UserParms, Residual);
  for (J = K7; J <= K8; J++)
    R3[ (J) - 1] = Residual[ (J) - 1];
  for (J = 1; J <= NumParmsUsed; J++)
    {
    H[ (J) - 1] = X1[ (Kount) - 1][ (J) - 1] - X2[ (INF) - 1][ (J) - 1];
    NormParms[ (J) - 1] = X2[ (INF) - 1][ (J) - 1] + 0.5 *
    H[ (J) - 1];
    }

  DeNormalise_Parms(UserParms, NormParms);
  pFn->InEqualityConstraints(UserParms, Residual);
  Flg[ (1) - 1] = 0.0;
  Flg[ (2) - 1] = 0.0;
  Flg[ (3) - 1] = 0.0;
  for (J = K7; J <= K8; J++)
    if (R3[ (J) - 1] < 0)
      {
      Flg[ (1) - 1] = Flg[ (1) - 1] + R1[ (J) - 1] * R1[ (J) - 1];
      Flg[ (2) - 1] = Flg[ (2) - 1] + Residual[ (J) - 1] *
      Residual[ (J) - 1];
      Flg[ (3) - 1] = Flg[ (3) - 1] + R3[ (J) - 1] * R3[ (J) - 1];
      }

  SR[ (INF) - 1] = Sqrt(Max(1.0e-50, Flg[ (1) - 1]));
  if (SR[ (INF) - 1] < Fdifer)
    goto L600;
  Alfa1 = Flg[ (1) - 1] - 2.0 * Flg[ (2) - 1] + Flg[ (3) - 1];
  Beta1 = 3.0 * Flg[ (1) - 1] - 4.0 * Flg[ (2) - 1] + Flg[ (3) - 1];
  if (Beta1 == 0)
    Ratio = 0;
  else if (Alfa1 == 0)
    Ratio = 1.0e+06;
  else
    Ratio = Beta1 / (4.0 * Alfa1);
  for (J = 1; J <= NumParmsUsed; J++)
    NormParms[ (J) - 1] = X2[ (INF) - 1][ (J) - 1] + H[ (J) - 1] *
    Ratio;
  IIN = INF;
  Sumr();

  SR[ (INF) - 1] = Sqrt(Max(1.0e-50, Seql));
  if (SR[ (INF) - 1] < Fdifer)
    goto L510;
  for (I = 1; I <= 20; I++)
    {
    for (J = 1; J <= NumParmsUsed; J++)
      NormParms[ (J) - 1] = NormParms[ (J) - 1] - 0.05 *
      H[ (J) - 1];
    Sumr();

    SR[ (INF) - 1] = Sqrt(Max(1.0e-50, Seql));
    if (SR[ (INF) - 1] < Fdifer)
      goto L510;
    }
L510 :
  UpdateCostCount();
  DeNormalise_Parms(UserParms, NormParms);
  Residual[ (K9) - 1] = pFn->CostFn(UserParms);
  DoIO();
  if (Fint > Residual[ (K9) - 1])
    goto L520;
  SR[ (INF) - 1] = 0;
  goto L600;
L520 :
  for (J = 1; J <= NumParmsUsed; J++)
    X2[ (INF) - 1][ (J) - 1] = NormParms[ (J) - 1];
L600 :
  for (J = 1; J <= NumParmsUsed; J++)
    NormParms[ (J) - 1] = X2[ (INF) - 1][ (J) - 1];
  }

// -------------------------------------------------------------------------

int CMFFlexiPlex::Solve()
  {
  InitParameters();

  if (! pFn->InitConvergence(StepSize, ConverTol))
    goto L200;


  NC = pFn->NEqualityConstraints();
  NIC = pFn->NInEqualityConstraints();

  if (NumParmsUsed == 0)
    goto L200;
L10 :
  Restart = FALSE;
  AbortFit = FALSE;
  CalcErr = 0;
  IterCnt = 1;
  IterCost = 0;
  //PlotIterCnt = 0;
  //LastPlotCost = 0;
  //LastPlotCnt = 0;
  Step = StepSize;
  K1 = NumParmsUsed + 1;
  K2 = NumParmsUsed + 2;
  K3 = NumParmsUsed + 3;
  K4 = NumParmsUsed + 4;
  K5 = NumParmsUsed + 5;
  K6 = NC + NIC;
  K7 = NC + 1;
  K8 = NC + NIC;
  K9 = K8 + 1;
  N = NumParmsUsed - NC;
  N1 = N + 1;
  if (N1 < 3)
    {
    N1 = 3;
    N = 2;
    }
  N2 = N + 2;
  N3 = N + 3;
  N4 = N + 4;
  N5 = N + 5;
  N6 = N + 6;
  N7 = N + 7;
  N8 = N + 8;
  R1A = 0.5 * (Sqrt(5.0) - 1.0);
  R2A = R1A * R1A;
  R3A = R2A * R1A;
  L5 = NumParmsUsed + 5;
  L6 = NumParmsUsed + 6;
  L7 = NumParmsUsed + 7;
  L8 = NumParmsUsed + 8;
  L9 = NumParmsUsed + 9;
  Icont = 1;
  Ncont = 1;
  Fdifer = 2.0 * (NC + 1) * Step;
  Fold = Fdifer;

  Residual.SetSize(K9);
  F.SetSize(N8); //?
  SR.SetSize(N8); //?
  Sum.SetSize(L9); //?
  F.SetAll(0.0);
  SR.SetAll(0.0);
  Sum.SetAll(0.0);
  X1.SetSize(L9, NumParmsUsed); //?
  X2.SetSize(N4, NumParmsUsed); //?
  X1.SetAll(0.0);
  X2.SetAll(0.0);

  /*WriteTraceWindow(1,18,'<<FindParam>Init1.(Fold=Fdifer) '+RStr(Fold,10,2)+'>>');*/

  IIN = N1;
  Sumr();

  SR[ (N1) - 1] = Sqrt(Max(1.0e-50, Seql));
  INF = N1;
  DoIO();
  if (SR[ (N1) - 1] < Fdifer)
    goto L20;
  Step = 0.05 * Fdifer;
  Feasbl();
  if (TraceReqd)
    {
    //  FOR  J := 1 to NumParmsUsed  DO
    //    WriteTraceWindow(1+(23*(Col-1)),Line,'X('+IStr(J,2)+') '+
    //      RStr(X2[INF,J],14,7));   { ... Unsatisfiable Vectors  }
    }
  if (TraceReqd)
    {
        //WriteTraceWindow(1,22,'The sum of violated constraints is '+RStr(SR[INF],12,5));*/
    }
  if (Fold < 1.0E-09)
    goto L190;
L20 :
  DoIO();
  if (Restart)
    goto L10;
  if (AbortFit)
    goto L200;
  Fter = Residual[ (K9) - 1];

  // compute centroid of all verticies of Initial polyhedron

  Step1 = Step * (Sqrt(NumParmsUsed + 1.0) +  NumParmsUsed - 1.0) / (NumParmsUsed * Sqrt(2.0));
  Step2 = Step * (Sqrt(NumParmsUsed + 1.0) - 1.0) / (NumParmsUsed * Sqrt(2.0));
  ETA = (Step1 + (NumParmsUsed - 1.0) * Step2) / (NumParmsUsed + 1.0);
  for (J = 1; J <= NumParmsUsed; J++)
    NormParms[ (J) - 1] = NormParms[ (J) - 1] - ETA;
  Start();
  for (I = 1; I <= N1; I++)
    for (J = 1; J <= NumParmsUsed; J++)
      X2[ (I) - 1][ (J) - 1] = X1[ (I) - 1][ (J) - 1];
  for (I = 1; I <= N1; I++)
    {
    IIN = I;
    for (J = 1; J <= NumParmsUsed; J++)
      NormParms[ (J) - 1] = X2[ (I) - 1][ (J) - 1];
    Sumr();

    SR[ (I) - 1] = Sqrt(Max(1.0e-50, Seql));
    if (SR[ (I) - 1] < Fdifer)
      goto L30;
    Feasbl();
    if (Fold < 1.0E-09)
      goto L190;
L30 :
    UpdateCostCount();
    DeNormalise_Parms(UserParms, NormParms);
    Residual[ (K9) - 1] = pFn->CostFn(UserParms);
    DoIO();
    if (Restart)
      goto L10;
    if (AbortFit)
      goto L200;
    F[ (I) - 1] = Residual[ (K9) - 1];
    }
L40 :
  ManualInt();
  if (Restart)
    goto L10;
  if (AbortFit)
    goto L200;
  Step = 0.05 * Fdifer;
  Icont = Icont + 1;

  // select largest value of objective function from polyhedron vertices 
  FH = F[ (1) - 1];
  Lhigh = 1;
  for (I = 2; I <= N1; I++)
    if (F[ (I) - 1] >= FH)
      {
      FH = F[ (I) - 1];
      Lhigh = I;
      }

  // select minimum value of objective function from polyhedron vertices 
L50 :
  FL = F[ (1) - 1];
  Low = 1;
  for (I = 2; I <= N1; I++)
    if (FL >= F[ (I) - 1])
      {
      FL = F[ (I) - 1];
      Low = I;
      }
  for (J = 1; J <= NumParmsUsed; J++)
    NormParms[ (J) - 1] = X2[ (Low) - 1][ (J) - 1];
  IIN = Low;
  Sumr();

  SR[ (Low) - 1] = Sqrt(Max(1.0e-50, Seql));
  if (SR[ (Low) - 1] < Fdifer)
    goto L60;
  INF = Low;
  Feasbl();
  if (Fold < 1.0E-09)
    goto L190;
  UpdateCostCount();
  DeNormalise_Parms(UserParms, NormParms);
  Residual[ (K9) - 1] = pFn->CostFn(UserParms);
  DoIO();
  if (Restart)
    goto L10;
  if (AbortFit)
    goto L200;
  F[ (Low) - 1] = Residual[ (K9) - 1];
  goto L50;

  // find centroid of points with I different than Lhigh
L60 :
  for (J = 1; J <= NumParmsUsed; J++)
    {
    Sum2 = 0;
    for (I = 1; I <= N1; I++)
      Sum2 = Sum2 + X2[ (I) - 1][ (J) - 1];
    X2[ (N2) - 1][ (J) - 1] = 1.0 / N * (Sum2 - X2[ (Lhigh) - 1][ (J) - 1]);
    }
  Sum2 = 0;
  for (I = 1; I <= N1; I++)
    for (J = 1; J <= NumParmsUsed; J++)
      Sum2 = Sum2 + Sqr(X2[ (I) - 1][ (J) - 1] - X2[ (N2) - 1][ (J) - 1]);

  Fdifer = (NC + 1.0) / N1 * Sqrt(Max(1.0e-50, Sum2));
  if (Fdifer < Fold)
    Fold = Fdifer;
  else
    Fdifer = Fold;
  Fter = F[ (Low) - 1];
  Ncont = Ncont + 1;
  if (Ncont < 4 * N1)
    goto L80;
  if (Icont < 1500)
    goto L70;
  Fold = 0.5 * Fold;
L70 :
  Ncont = 0;
  DoIO();
L80 :
  if (Fdifer < ConverTol)
    goto L180;

  //  select second largest value of objective function
  if (Lhigh == 1)
    {
    FS = F[ (2) - 1];
    Lsec = 2;
    }
  else
    {
    FS = F[ (1) - 1];
    Lsec = 1;
    }
  for (I = 1; I <= N1; I++)
    if ((Lhigh != I) && (F[ (I) - 1] >= FS))
      {
      FS = F[ (I) - 1];
      Lsec = I;
      }
  
  // reflect high point through centroid
  for (J = 1; J <= NumParmsUsed; J++)
    {
    X2[ (N3) - 1][ (J) - 1] = X2[ (N2) - 1][ (J) - 1] + Alfa * (X2[ (N2) - 1][ (J) - 1] - X2[ (Lhigh) - 1][ (J) - 1]);
    NormParms[ (J) - 1] = X2[ (N3) - 1][ (J) - 1];
    }
  IIN = N3;
  Sumr();

  SR[ (N3) - 1] = Sqrt(Max(1.0e-50, Seql));
  if (SR[ (N3) - 1] < Fdifer)
    goto L90;
  INF = N3;
  Feasbl();
  if (Fold < 1.0E-09)
    goto L190;
L90 :
  UpdateCostCount();
  DeNormalise_Parms(UserParms, NormParms);
  Residual[ (K9) - 1] = pFn->CostFn(UserParms);
  DoIO();
  if (Restart)
    goto L10;
  if (AbortFit)
    goto L200;
  F[ (N3) - 1] = Residual[ (K9) - 1];
  if (F[ (N3) - 1] < F[ (Low) - 1])
    goto L110;
  if (F[ (N3) - 1] < F[ (Lsec) - 1])
    goto L100;
  goto L130;
L100 :
  for (J = 1; J <= NumParmsUsed; J++)
    X2[ (Lhigh) - 1][ (J) - 1] = X2[ (N3) - 1][ (J) - 1];
  SR[ (Lhigh) - 1] = SR[ (N3) - 1];
  F[ (Lhigh) - 1] = F[ (N3) - 1];
  goto L40;

  // expand vector of search along direction through centroid and reflected vector

L110 :
  for (J = 1; J <= NumParmsUsed; J++)
    {
    X2[ (N4) - 1][ (J) - 1] = X2[ (N3) - 1][ (J) - 1] + Gama * (X2[ (N3) - 1][ (J) - 1] - X2[ (N2) - 1][ (J) - 1]);
    NormParms[ (J) - 1] = X2[ (N4) - 1][ (J) - 1];
    }
  IIN = N4;
  Sumr();

  SR[ (N4) - 1] = Sqrt(Max(1.0e-50, Seql));
  if (SR[ (N4) - 1] < Fdifer)
    goto L120;
  INF = N4;
  Feasbl();
  if (Fold < 1.0E-09)
    goto L190;
L120 :
  UpdateCostCount();
  DeNormalise_Parms(UserParms, NormParms);
  Residual[ (K9) - 1] = pFn->CostFn(UserParms);
  DoIO();
  if (Restart)
    goto L10;
  if (AbortFit)
    goto L200;
  F[ (N4) - 1] = Residual[ (K9) - 1];
  if (F[ (Low) - 1] < F[ (N4) - 1])
    goto L100;
  for (J = 1; J <= NumParmsUsed; J++)
    X2[ (Lhigh) - 1][ (J) - 1] = X2[ (N4) - 1][ (J) - 1];
  F[ (Lhigh) - 1] = F[ (N4) - 1];
  SR[ (Lhigh) - 1] = SR[ (N4) - 1];
  goto L40;
L130 :
  if (F[ (N3) - 1] > F[ (Lhigh) - 1])
    goto L140;
  for (J = 1; J <= NumParmsUsed; J++)
    X2[ (Lhigh) - 1][ (J) - 1] = X2[ (N3) - 1][ (J) - 1];
L140 :
  for (J = 1; J <= NumParmsUsed; J++)
    {
    X2[ (N4) - 1][ (J) - 1] = Beta * X2[ (Lhigh) - 1][ (J) - 1] + (1.0 - Beta) * X2[ (N2) - 1][ (J) - 1];
    NormParms[ (J) - 1] = X2[ (N4) - 1][ (J) - 1];
    }
  IIN = N4;
  Sumr();

  SR[ (N4) - 1] = Sqrt(Max(1.0e-50, Seql));
  if (SR[ (N4) - 1] < Fdifer)
    goto L150;
  INF = N4;
  Feasbl();
  if (Fold < 1.0E-09)
    goto L190;
L150 :
  UpdateCostCount();
  DeNormalise_Parms(UserParms, NormParms);
  Residual[ (K9) - 1] = pFn->CostFn(UserParms);
  DoIO();
  if (Restart)
    goto L10;
  if (AbortFit)
    goto L200;
  F[ (N4) - 1] = Residual[ (K9) - 1];
  if (F[ (Lhigh) - 1] > F[ (N4) - 1])
    goto L170;
  for (J = 1; J <= NumParmsUsed; J++)
    for (I = 1; I <= N1; I++)
      X2[ (I) - 1][ (J) - 1] = 0.5 * (X2[ (I) - 1][ (J) - 1] + X2[ (Low) - 1][ (J) - 1]);
  for (I = 1; I <= N1; I++)
    {
    for (J = 1; J <= NumParmsUsed; J++)
      NormParms[ (J) - 1] = X2[ (I) - 1][ (J) - 1];
    IIN = I;
    Sumr();

    SR[ (I) - 1] = Sqrt(Max(1.0e-50, Seql));
    if (SR[ (I) - 1] < Fdifer)
      goto L160;
    INF = I;
    Feasbl();
    if (Fold < 1.0E-09)
      goto L190;
L160 :
    UpdateCostCount();
    DeNormalise_Parms(UserParms, NormParms);
    Residual[ (K9) - 1] = pFn->CostFn(UserParms);
    DoIO();
    F[ (I) - 1] = Residual[ (K9) - 1];
    }
  goto L40;
L170 :
  for (J = 1; J <= NumParmsUsed; J++)
    X2[ (Lhigh) - 1][ (J) - 1] = X2[ (N4) - 1][ (J) - 1];
  SR[ (Lhigh) - 1] = SR[ (N4) - 1];
  F[ (Lhigh) - 1] = F[ (N4) - 1];
  goto L40;
L180 :
  DoIO();
  goto L200;
L190 :
  if (TraceReqd)
    DoIO();
  //if (SmallTraceReqd)
  //  {
  //  if (CalcErr == 0)
  //    CalcErr = 10;
  //
  //  //ShowMsg('Not Converging ', InfoMsg, NIL);
  //  //WriteDbgLn('');
  //  //WriteDbgLn('Not Converging !');
  //  }
L200 :
  DeNormalise_Parms(UserParms, NormParms);
  /*
  for (I = 1; I <= cblk->FitData.NumPoints; I++)
    {
      for (II = 1; II <= cblk->FitData.NIndData; II++)
          cblk->IVal[(II) - 1] = (*withp1->IndData[(II) - 1])[(I) - 1];

      FlexiFit_Func(CalcDep,*cblk,cblk->FitQ,cblk->Eqn.NumParms,
              cblk->Eqn.ParmsQ,cblk->Eqn.UParmsQ,
              cblk->FitData.Residual)
      (*withp1->YFit)[(I) - 1] = cblk->DVal;
    }
  */
  return CalcErr;
  }

//==========================================================================
//
//
//
//==========================================================================
