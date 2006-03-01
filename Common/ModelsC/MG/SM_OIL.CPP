//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "sc_defs.h"

#define  __SM_OILFL_CPP
#include "sp_db.h"
#include "sp_cont.h"
#include "sm_oil.h"
#include "errorlog.h"
//#include "dbgcom.h"
#include "dbgmngr.h"

// #define dbgSM_Oil 0x1
#define dbgSM_Oil 01

#if dbgSM_Oil
static CDbgMngr dbgQPFlash    ("SM_Oil", "QPFlash");
static CDbgMngr dbgTPFlash    ("SM_Oil", "TPFlash");
static CDbgMngr dbgConvergence("SM_Oil", "Convergence");
static CDbgMngr dbgMemory     ("SM_Oil", "Memory");
static CDbgMngr dbgStats      ("SM_Oil", "Stats");
static CDbgMngr dbgKConverge  ("SM_Oil", "KConverge");
#endif

IMPLEMENT_SPMODEL(SM_Oil, "LHOil", "", TOC_ALL|TOC_GRP_ENERGY|TOC_USER, "Light/Heavy Oil", "");
IMPLEMENT_SPARES(SM_Oil, 100);

#ifdef ForceTests
ACCESS_SPECIE(H2o,       "H2O"     ); 
ACCESS_SPECIE(Methane,   "MethaneG");
ACCESS_SPECIE(Ethane,    "EthaneG"); 
ACCESS_SPECIE(Propane,   "PropaneG");
#endif

// ===========================================================================

flag SM_Oil::fIdealFlashOnly=1;

SM_Oil::SM_Oil(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach) :
  SpModel(pClass_, Tag_, pAttach, eAttach)
  {
  LoadZused = False;
  InitRqd   = True;
  Tsetting=-1.0; 

  ZLen=0;
  Zvap=1.0; 
  Zliq=1.0; 

  QPm.Temp=dNAN();
  
  H2OLiq=0.0;
  H2OVap=0.0;
  QTLast=293;
  QPLast=100;
  Qflashdone=False; 
  }

// ---------------------------------------------------------------------------

SM_Oil::~SM_Oil()
  {
  }

// --------------------------------------------------------------------------

void SM_Oil::BuildDataDefn_Vars(DataDefnBlk & DDB, SPM_View View)
  {
  SpModel::BuildDataDefn_Vars(DDB, View);
  DDB.Bool("IdealKs",   "",  DC_,  "",         &fIdealFlashOnly,  this,  isParm);
  }

// --------------------------------------------------------------------------

void SM_Oil::IdealGasKratio(double T, double P)
  {
  int i;
  double ZC,TR,PSAT;

  for (i = 0; i < ZLen; i++) 
    {
    ZC = 0.29 - 0.065 * CDB[i].ACent();
    TR = T/CDB[i].TCrit();

    if (T > CDB[i].TCrit()) 
      {
      double RLPSAT = (16.26-73.85*ZC+90.0*ZC*ZC) * (1.0-1.0/TR)-exp(log(10.0)*(-8.68*(TR-1.8+6.2*ZC)*(TR-1.8+6.2*ZC)));
      if ((RLPSAT <  -LnExpMax))
          RLPSAT =  -LnExpMax;
      if ((RLPSAT >  LnExpMax))
          RLPSAT =  LnExpMax;
      PSAT = exp(RLPSAT) * CDB[i].PCrit();  //kcg added *1000
      }
    else   
      {
      PSAT = exp((4.92 * CDB[i].ACent() + 5.81) * log(TR) - 0.0838 * (4.92 * CDB[i].ACent() + 2.06) * (36.0 / 
              TR - 35.0 - (TR * TR * TR * TR * TR * TR) + 42.0 * 
              log(TR))) * CDB[i].PCrit();    //kcg added *1000
      }
    K[i] = PSAT / P;
    }
  }

// --------------------------------------------------------------------------

void SM_Oil::SetXYfromK()
  {
  int i;

double KScl=1.;
  double Vf=Range( 1.0e-8, VFrac, 1.0 - 1.0e-8);
  //double sx = 0.0;
  //double sy = 0.0;

  for (int Iter=1;Iter;Iter--)
    {
    double ZmK=0.0;
    double ZdK=0.0;
    for (i = 0; i < ZLen; i++) 
      {
      K[i] = Range(1.0e-10, K[i]*KScl, 1.0e10);
      X[i] = Z[i] / (1.0 + (Vf * (K[i] - 1.0)));
      Y[i] = K[i] * X[i];
      ZmK+=Z[i]*K[i];
      ZdK+=Z[i]/K[i];
      //sx += X[i];
      //sy += Y[i];
      }
    //sx=GTZ(sx);
    //sy=GTZ(sy);
  
    double sk = GTZ(K.Sum());
    double amk = 0.0;
    double gmk = 1.0;
    double sz = GTZ(Z.Sum());
    double sx = GTZ(X.Sum());
    double sy = GTZ(Y.Sum());
    for (i = 0; i < ZLen; i++) 
      {
      amk+=K[i];
      gmk*=K[i];
      }
    amk/=ZLen;
    gmk=Pow(gmk, 1.0/ZLen);
    flag BubblePt=ZmK<1.0;
    flag DewPt=ZdK<1.0;
    #if dbgSM_Oil
    if (dbgKConverge())
      dbgpln("SXY %s", BubblePt?"All Liquid" : DewPt ? "All Vapour" : "");    
    #endif
    int xxx=0;
    if (fabs(sx-1.0)<0.01)
      break;

    /**/
    for (i = 0; i < ZLen; i++) 
      {
      X[i] /= sx;
      Y[i] /= sy;
      }
    /**/
    }
  int yyy=0;

  }

// --------------------------------------------------------------------------

double VFracSmall         = 1.0e-6;
double QPFlashTol         = 1.0e-4;
double ConvergeKratiosTol = QPFlashTol*1.0e-3;
double RachRice_SolveTol1 = ConvergeKratiosTol*1.0e-4;
double RachRice_SolveTol2 = 1.0e-10;
double GundersonRootTol   = 1.0e-6;

double SM_Oil::GundersonRoot(double A1,double A0,double StartZ)
  {
  #if SM_OIL_STATS
  Cnt.GundRootCall++;
  #endif

  int Iter = 0;
  double ERR,F,DF,ZN,Zv = StartZ,Zv2;

  do 
    { 
    #if SM_OIL_STATS
    Cnt.GundRootIter++;
    #endif
    Zv2 = Zv*Zv;
    F = (Zv * Zv2) - Zv2 + A1 * Zv - A0;
    DF = 3.0 * (Zv2) - 2.0 * Zv + A1;
    ZN = Zv - (F / DF);
    ERR = fabs((ZN - Zv) / ZN);
    Zv = ZN;
    Iter++;
    if( Iter > 45 )
      {
      LogError("SM_Oil", 0, "gundersonroot is running at more than 45 its");
      return(-100.0);
      }
    } 
  while (ERR > GundersonRootTol);
  return(Zv);
}

// --------------------------------------------------------------------------
// Refer to: Numerical aspects of the mplementation of cubic equations of state 
// in flash calculation routines/
// Computers and Chemical Engineering Vol 6, No 3, pp 245-255 1982

void SM_Oil::Gunderson(double &Q,double &R,double &A,double &B,int IPH,double& Z )
  {
  if( Q > 1.0/3.0 )
    {
    double F3 = 1.0 / 27.0 - 1.0 / 9.0 + Q / 3.0 - R;
    if( F3 > 0.0 )
      {
      Z = GundersonRoot(Q,R,0.0);
      }
    else
      {
      Z = GundersonRoot(Q,R,3.0); // change from program to match gunderson paper
      }
    }
  else
    {
    if( (R > 1.0/27.0) && (IPH == 1) )
      {
      Z = GundersonRoot(Q,R,3.0); // change from program to match gunderson paper
      }
    else
      {
      double Z1 = (1.0 - sqrt(1.0 - (3.0 * Q))) / 3.0;
      double Z2 = (1.0 + sqrt(1.0 - (3.0 * Q))) / 3.0;
      double F1 = (Z1 * Z1 * Z1) - (Z1 * Z1) + (Q * Z1) - R;
      double F2 = (Z2 * Z2 * Z2) - (Z2 * Z2) + (Q * Z2) - R;
      if( IPH == 0 )
        {
        if( F1 > 0.0 )
          {
          Z = GundersonRoot(Q,R,0.0);
          }
        else
          {
          Z = Z1;
          B = B*(1 + F1/R);
          Q = A - B - B*B;
          R = A * B;
          }
        }
      else
        {
        if( F2 < 0.0 )
          {
          Z = GundersonRoot(Q,R,3.0); // change from program to match gunderson paper
          }
        else
          {
          Z = Z2;
          B = B*(1 + F2/R);
          Q = A - B - B*B;
          R = A * B;
          }
        }
      }
    }
  }

// --------------------------------------------------------------------------

double SM_Oil::FindZValue(flag Vapour, double B, double C, double D, CubeRoots Roots, int NRoots, double &AMix, double &BMix)
  {
  const flag FDebug=0;
  flag Imag1     = FALSE;
  flag Imag2     = FALSE;
  flag WrongRoot = FALSE;
  double ZComp=dNAN();

  switch (NRoots)
    {
    case 1: 
      //  there is only one root - first check if it corresponds
      //  to the required phase, using Gosset's method 
  
      {
      flag Liq = False;
      flag Vap = False;

      if (Roots[0] < (BMix / (3.0 * Cb)))
        Liq = True;
      else
        Vap = True;

      if (FDebug) 
        dbgpln("%s, %s Root Found", Liq?"Liquid":"      ", Vap ? "Vapour":"      ");

      if ((Vapour && Liq) || (!Vapour && Vap))
        //  it doesn't so get a pseudoroot 
        {
        double Z1 = 0.0;
        double Z2 = 0.0;
        double F1 = 0.0;
        double F2 = 0.0;

        if (C < (1./3.)) //                      { have turning points }
          {
          Z1 = (1. - Sqrt(1. - 3.*C)) / 3.;
          Z2 = (1. + Sqrt(1. - 3.*C)) / 3.;
          F1 = (Z1 - 1.) * Sqr(Z1) + C * Z1 + D;
          F2 = (Z2 - 1.) * Sqr(Z2) + C * Z2 + D;

          ZComp = Roots[0];
          if (Vapour && (F1 > 0.) && (F2 > 0.))
            {
            ZComp = Z2;
            Imag2 = True;
            }
          else if (!Vapour && (F1 < 0.) && (F2 < 0.))
            {
            ZComp = Z1;
            Imag1 = True;
            };

          // retest root found 

          Liq = False;
          Vap = False;
          if (ZComp < (BMix / (3.0 * Cb)))
            Liq = True;
          else
            Vap = True;
          if (FDebug) 
            dbgpln("%s, %s Root Found", Liq?"Liquid":"      ", Vap ? "Vapour":"      ");

          if (Vapour && Liq) 
            {
            WrongRoot = True;
            Imag1 = False;
            ZComp = 1.01 * BMix / (3.0 * Cb);
            F1 = (ZComp - 1.) * Sqr (ZComp) + C * ZComp + D;
            };

          if (!Vapour && Vap) 
            {
            WrongRoot = True;
            Imag2 = False;
            ZComp = 0.99 * BMix / (3.0 * Cb);
            F1 = (ZComp - 1.) * Sqr (ZComp) + C * ZComp + D;
            };

          }
        else 
          // no turning points
          {
          if (Vapour && Liq) 
            ZComp = 1.01 * BMix / (3.0 * Cb);
          if (!Vapour && Vap) 
            ZComp = 0.99 * BMix / (3.0 * Cb);
          F1 = (ZComp - 1.) * Sqr (ZComp) + C * ZComp + D;
          WrongRoot = True;
          };

        // modify BMix for imaginary roots
        if (WrongRoot)         BMix = BMix * (1 - F1/D);
        if (Vapour  && Imag2)  BMix = BMix * (1 - F2/D);
        if (!Vapour && Imag1)  BMix = BMix * (1 - F1/D);
        }
      else
        ZComp = Roots[0];
      break;
      };   
    case 2:
      {
      if (Vapour)
        ZComp = Roots[1];
      else
        ZComp = Roots[0];
      break;
      };
    case 3:
      {
      if (Vapour)
        ZComp = Roots[2];
      else
        ZComp = Roots[0];
      break;
      };
    };
  return ZComp;
  };

// --------------------------------------------------------------------------
                                                                           
double SM_Oil::RachRice_Value(double Vf, double &Deriv, double &LfNew, double &VfNew)
  {
  int i;
  Vf=Range(VFracSmall, Vf, 1.0-VFracSmall);
  double Func = 0.0;
  Deriv = 0.0;
  LfNew=0.0;
  if (K.GetLen()  < 1) 
    return 0.0;  //kcg new flash in pipe K empty
  for( i = 0; i < ZLen; i++ )
    {

    double t = K[i]-1.0;
    double a = (t*Vf + 1.0);
    double a2 = a*a;
    
    double MinAValue=MinZValue*1.0e-20;
    if( fabs(a ) < MinAValue)
       a  = MinAValue * Sign(a );
    if( fabs(a2) < MinAValue)
       a2 = MinAValue * Sign(a2);

    Func  += ( t * Z[i] ) / (a);
    Deriv += -( t * t * Z[i] ) / (a2);

    LfNew+=Z[i]/(1.0+Vf*(K[i]-1));
    }
  VfNew=1.0-LfNew;
  return Func;
  }

// --------------------------------------------------------------------------

int SM_Oil::RachRice_Solve()
  {
  int cnt = 0;
  int ier = 0;
  double F,F1,F2,d1,LfNew, VfNew;
  double VFrac_old = VFrac;

  #if SM_OIL_STATS
  Cnt.RacRicCall++;
  #endif

  F1 =  RachRice_Value(VFracSmall, d1, LfNew, VfNew); // evaluate at zero
  if( F1 < 1.0e-10)
    {
    VFrac = VFracSmall;
    //kcg VFrac = VFrac/2.0;
    return -1;
    }
  F2 =  RachRice_Value(1.0-VFracSmall, d1, LfNew, VfNew);
  if( F2 > 1.0e-6) // CNM 1.0e-6
    {
    VFrac = 1.0-VFracSmall;
    // VFrac = (VFrac + 1.0)/2.0;
    return -1;
    }
  
  VFrac = Range(0.0, VFrac, 1.0);

  for (cnt=1000; cnt; cnt--)
    {
    #if SM_OIL_STATS
    Cnt.RacRicIter++;
    #endif

    double oldVF = VFrac;
    F = RachRice_Value(VFrac, d1, LfNew, VfNew);
    double V = VFrac - (F/(d1));
    //VFrac = Range(0.000001, (V + VFrac)/2.0, 0.999999);
    VFrac = Range(0.0, (V + VFrac)/2.0, 1.0);

    if ((fabs(F) < RachRice_SolveTol1) || (fabs(VFrac-oldVF)<RachRice_SolveTol2))
      break;
    }
  if (cnt==0)
    LogError("SM_Oil", 0, "RachRice_Solve not Converged");

  F = RachRice_Value(VFrac, d1, LfNew, VfNew);

  return(0);
  }


// --------------------------------------------------------------------------
//  RacRic evaluates the Rachford and Rice objective function
double SM_Oil::RacRic()
  {
  double TempVal = 0.;
  for (int i = 0; i < ZLen; i++ )
    TempVal = TempVal + Z[i] * (K[i] - 1.) / (1. + VFrac * (K[i] - 1.));   
  return TempVal;
  };

// --------------------------------------------------------------------------
//  DRacRic evaluates the first derivative of the Rachford and Rice
//  objective function

double SM_Oil::DRacRic ()
  {
  double TempVal = 0.;
  for (int i = 0; i < ZLen; i++ )
    TempVal = TempVal - Z[i] * Sqr (K[i] - 1.) / Sqr (1. + VFrac * (K[i] - 1.));
  return TempVal;
  };

// --------------------------------------------------------------------------
// FindBounds determines the upper and lower bounds for the solution
// of the Rachford and Rice objective function

void SM_Oil::FindBounds(double &Left, double &Right)
  {
  double SmallestK =  1.0e20;
  double LargestK  = -1.0e20;
  for (int i = 0; i < ZLen; i++ )
    {
    LargestK  = Max(LargestK, K[i]);
    SmallestK = Min(SmallestK, K[i]);
    //if (K[i] > LargestK )
    //  LargestK  = K[i];
    //if (K[i] < SmallestK)
    //  SmallestK = K[i];
    };
  Left  = 1.0 / (1.0 - LargestK);
  Right = 1.0 / (1.0 - SmallestK);
  if (SmallestK > 1.)
    Right = 1.0e20;
  else if (LargestK < 1.)
    Left = -1.0e20;
  };

// --------------------------------------------------------------------------
// ConvergeVFrac solves the Rachford and Rice objective function        ³
// for VFrac using a Newton Raphson technique modified
// to ensure convergence quickly

void SM_Oil::ConvergeVFrac(flag NoGuess)
  {
  const double FuncErr  = 1.0E-4;
  const int    MaxIter  = 50;
  double LeftAsym, RightAsym;//, Func, DFunc, D2Func, VFrac1;
  //int    i;
  FindBounds (LeftAsym, RightAsym);
 
  //if (FDebug) 
  //  {
  //  RTL_Use;
  //  writeln (DBGFPtr^, 'LeftAsym, RightAsym  ', LeftAsym, RightAsym);
  //  RTL_Free;
  //  };

  if (LeftAsym < -0.9e20)
    VFrac = 0.0;
  else if (RightAsym > 0.9e20)
    VFrac = 1.0;
  else
    {
    if (NoGuess) 
      VFrac = 0.5 * (LeftAsym + RightAsym);
    else if ((VFrac < LeftAsym) || (VFrac > RightAsym))
      VFrac = 0.5 * (LeftAsym + RightAsym);

    //if (FDebug) 
    //  FOR I = 1 TO NComp DO
    //    {
    //    RTL_Use;
    //    writeln (DBGFPtr^, 'K ', I, K[I]);
    //    writeln (DBGFPtr^, 'Z ', I, Z[I]);
    //    RTL_Free;
    //    };

    //double VFrac1 = VFrac;
    for (int Iter=MaxIter; Iter; Iter--)
      {
      //VFrac  = VFrac1;
      double Func = RacRic ();
      if ((fabs(Func) > FuncErr) || (Iter == 0))
        {
        double DFunc  = DRacRic ();
        double VFrac1 = VFrac - Func / DFunc;
        //if (VFrac1 > RightAsym)
        if (VFrac1 > RightAsym)
          VFrac1 = 0.5 * (RightAsym + VFrac);
        if (VFrac1 < LeftAsym)
          VFrac1 = 0.5 * (LeftAsym + VFrac);
        //if (Func > RightAsym)
        //  VFrac1 = 0.5 * (1.0 + VFrac);
        //if (Func < LeftAsym)
        //  VFrac1 = 0.5 * (0.0 + VFrac);
        //if (VFrac1 > 1.0)
        //  VFrac1 = 0.5 * (1.0 + VFrac);
        //if (VFrac1 < 0.0)
        //  VFrac1 = 0.5 * (0.0 + VFrac);
        VFrac = Range(0.0, VFrac1, 1.0);
        }
      else 
        break;
      }
    if (Iter==0)
      LogError("SM_Oil", 0, "ConvergeVFrac not Converged");

    //if (FDebug) 
    //  {
    //  RTL_Use;
    //  writeln (DBGFPtr^, '        ', VFrac, Func);
    //  RTL_Free;
    //  };
    //VFrac = VFrac1;
    VFrac=Range(0.0, VFrac, 1.0);
    };
  };

// --------------------------------------------------------------------------

int SM_Oil::Fugacity (flag Vapour, SpMArray & PHI,double T,double P, CDVector& XorY, double& Zval)
  {
  int i;//,j;

  // Calculate AAM and BBM for the mixture
  /*
  double AAM = 0.0;
  double BBM = 0.0;
  for (i = 0; i < ZLen; i++) 
    {
    BBM += B[i] * XorY[i];
    for (j = 0; j < ZLen; j++)
      {
      AAM += ALA[i][j] * XorY[i] * XorY[j];
      }
    }

  // Estimate the Compressability
  double BIGA = AAM * P / ( RGas * RGas * T * T );
  double BIGB = BBM * P / ( RGas * T);
  double Q = BIGA - BIGB - (BIGB*BIGB);
  double RR = BIGA * BIGB;

  Gunderson(Q,RR,BIGA,BIGB,!LIQ,Zval);
  for (j = 0; j < ZLen; j++) 
    {
    double SUMXA = 0;
    for (i = 0; i < ZLen; i++)
      {
      SUMXA = SUMXA + XorY[i] * ALA[j][i];
      }
    double RLNPHI = 0.0;
    if ((((Zval - BIGB) > 0.0) && ((1.0 + BIGB / Zval) > 0.0))) 
      {
      RLNPHI = (B[j] / BBM) * (Zval - 1.0) - log(Zval - BIGB) - 
              BIGA / BIGB * (2.0 * SUMXA / AAM - B[j] / BBM) * log(1.0 + 
             (BIGB / Zval));
      }
    else  
      {
      RLNPHI = 0.0;
      //dbgpln("RLNPHI made zero"); 
      }  
    PHI[j] = RLNPHI;
    }
  */
  //------------------ACF's version
  /*
  for (i = 0; i < ZLen; i++) 
    {
    double TCrit = CDB[i].TCrit();
    double PCrit = CDB[i].PCrit();
    double ACent = CDB[i].ACent();

    A[i] = Ca * Sqr(RGas*TCrit) /(PCrit);   
    S[i] = 0.48 + 1.574 * ACent- 0.176 * Sqr(ACent);
    Alpha[i] = Sqr(1.0 + S[i] * (1.0 - Sqrt(T / TCrit)));
    //ALA[i][i] = A[i] * Alpha[i];
    B[i] = Cb * RGas * CDB[i].TCrit()/CDB[i].PCrit();
    }
  */
  /*
  double AsMix = 0.0;
  double BsMix = 0.0;
  for (i = 0; i < ZLen; i++) 
    {
    BsMix += B[i] * XorY[i];
    AsMix += XorY[i]*CDB[i].TCrit()*Sqrt(Alpha[i]/CDB[i].PCrit());
    
    //AsMix += ALA[i][j] * XorY[i] * XorY[j];
    }

  double AMix  = Ca * Sqr(AsMix) * P / Sqr(T);
  double BMix  = BsMix * P / (RGas * T);
  */
  double AMix = 0.0;
  double BMix = 0.0;
  for (i = 0; i < ZLen; i++) 
    {
    double TCrit = CDB[i].TCrit();
    double PCrit = CDB[i].PCrit();
    double ACent = CDB[i].ACent();

    S[i] = 0.48 + 1.574 * ACent- 0.176 * Sqr(ACent);
    Alpha[i] = Sqr(1.0 + S[i] * (1.0 - Sqrt(T / TCrit)));
    AMix += XorY[i] * TCrit*Sqrt(Alpha[i]/PCrit);
    BMix += XorY[i] * TCrit/PCrit;
    
    //AsMix += ALA[i][j] * XorY[i] * XorY[j];
    }

  AMix  = Ca * P / Sqr(T) * Sqr(AMix);
  BMix  = Cb * P / (T) * BMix;
  
  CubeRoots Roots;
  int NRoots;
  double b = -1;
  double c = AMix - BMix - Sqr(BMix);
  double d = -AMix * BMix;

  NRoots = FindCubeRoots (b, c, d, Roots);
  Zval=FindZValue(Vapour, b,c,d, Roots, NRoots, AMix, BMix);

  //double BigA = AsMix * P / Sqr(RGas * T);
  //double BigB = BsMix * P / (RGas * T);

  #if dbgSM_Oil
  if (dbgKConverge())
    dbgpln("Fug A,B,Z: %14.3g %14.3g %14.3g", AMix, BMix, Zval);
  #endif
  
  double SqrtA=0.0;
  double B1=0.0;
  for (i = 0; i < ZLen; i++) 
    {
    double TCrit = CDB[i].TCrit();
    double PCrit = CDB[i].PCrit();
    SqrtA+=XorY[i]*Sqrt(Alpha[i]*PCrit);
    B1+=XorY[i]*TCrit/PCrit;
    }
  
  double LclPhi[MaxSpecies];
  for (i = 0; i < ZLen; i++) 
    {
    //double SumXa = 0;
    //for (i = 0; i < ZLen; i++)
//      SumXa = SumXa + XorY[i] * ALA[i][i];
    //SumXa = XorY[i] * ALA[i][i];
    double TCrit = CDB[i].TCrit();
    double PCrit = CDB[i].PCrit();

    double SqrtAj=Sqrt(Alpha[i]/PCrit)*TCrit;
    double Bj=TCrit/PCrit;

    //Zval=Max(Zval, BMix+1.0e-6);
    //ZVal=Max(ZVal, -BMix+1.0e-6);
    //  if ((((Zval - BMix) > 0.0) && ((1.0 + BMix / Zval) > 0.0))) 
    if ((((Zval - BMix) > 0.0) && ((1.0 + BMix / Zval) > 0.0))) 
      LclPhi[i] = (Bj / BMix) * (Zval - 1.0) - 
               log(Zval - BMix) - 
               AMix / BMix * (2.0 * SqrtAj/SqrtA - Bj/B1) * 
               log(1.0 + (BMix / Zval));
    else
      LclPhi[i] = 0.0;
    PHI[i] = LclPhi[i];

    }

  int ACFs=1;
  if (ACFs)
    {
    // ACF's Version;
    double As[MaxSpecies];
    double Bs[MaxSpecies];
    double Sxa[MaxSpecies];
    double AsMix=0.0;
    double BsMix=0.0;
    for (int i=0; i<ZLen; i++)
      {
      double TCrit = CDB[i].TCrit();
      double PCrit = CDB[i].PCrit();
      double ACent = CDB[i].ACent();
      As[i]=Ca*Sqr(RGas*TCrit)/PCrit*Alpha[i];
      Bs[i]=Cb*RGas*TCrit/PCrit;
      }
    for (i=0; i<ZLen; i++)
      {
      Sxa[i]=0.0;
      for (int j=0; j<ZLen; j++)
        Sxa[i]+=XorY[j]*Sqrt(As[i]*As[j]);
      AsMix+=XorY[i]*Sxa[i];
      BsMix+=XorY[i]*Bs[i];
      }
    double AMix=AsMix*P/Sqr(RGas*T);
    double BMix=BsMix*P/(RGas*T);
    
    double ALclPhi[MaxSpecies];
    for (i=0; i<ZLen; i++)
      {
      if ((((Zval - BMix) > 0.0) && ((1.0 + BMix / Zval) > 0.0))) 
        ALclPhi[i] = (Bs[i] / BsMix) * (Zval - 1.0) - 
                 log(Zval - BMix) - 
                 AMix / BMix * (2.0 * Sxa[i]/AsMix - Bs[i]/BsMix) * 
                 log(1.0 + (BMix / Zval));
      else
        ALclPhi[i] = 0.0;
      PHI[i]=ALclPhi[i];
      }

    int xxx=0;
    int yyy=0;
    }


  
  /*
  double BigA = AsMix * P / Sqr(RGas * T);
  double BigB = BsMix * P / (RGas * T);

  //dbgpln("P,T,Z: %14.3g %14.3g %14.3g",P,T,Zval);
  for (j = 0; j < ZLen; j++) 
    {
    double SumXa = 0;
    //for (i = 0; i < ZLen; i++)
//      SumXa = SumXa + XorY[i] * ALA[j][i];
    SumXa = XorY[i] * ALA[i][i];
    double RLnPhi = 0.0;
    if ((((Zval - BigB) > 0.0) && ((1.0 + BigB / Zval) > 0.0))) 
      {
      RLnPhi = (B[j] / BsMix) * (Zval - 1.0) - log(Zval - BigB) - 
              BigA / BigB * (2.0 * SumXa / AsMix - B[j] / BsMix) * log(1.0 + 
             (BigB / Zval));
      }
    else  
      RLnPhi = 0.0;
    PHI[j] = RLnPhi;
    }
  */
  return(0);
  }

// --------------------------------------------------------------------------

double SM_Oil::FugacityKratio(double Temp_, double Pres_, double Damp)
  {
  SpMArray PHIL,PHIV;

  #if SM_OIL_STATS
  Cnt.FugKCall++;
  #endif

  //SetXYfromK();

  //Temp_=(100.-32.)/1.8+273.16;
  //Pres_=(250./14.7)*100.0;
  //Y.SetAll(0.0);
  //Y[Methane.ci()]=0.75;
  //Y[Ethane.ci()]=0.15;
  //Y[Propane.ci()]=0.10;
  
  if (Fugacity(False, PHIL, Temp_, Pres_, X, Zliq))
    LogError("SM_Oil", 0, "Liquid Fugacity returns error");
  if (Fugacity(True, PHIV, Temp_, Pres_, Y, Zvap))
    LogError("SM_Oil", 0, "Gas Fugacity returns error");

  #if dbgSM_Oil
  if (dbgKConverge())
    {
    int i;
    dbgp("  L:");
    dbgp(" %10.10s", "");
    for (i=0; i<CDB.No() ; i++)
      dbgp(" %10.7f", PHIL[i]);
    dbgpln("");
    dbgp("  V:");
    dbgp(" %10.10s", "");
    for (i=0; i<CDB.No() ; i++)
      dbgp(" %10.7f", PHIV[i]);
    dbgpln("");
    }
  #endif

  //Damp=0.0;
  double rms = 0.0;
  for (int i = 0; i < ZLen; i++) 
    {
    #if SM_OIL_STATS
    Cnt.FugKIter++;   
    #endif

    double NewK;
    if (0)
      NewK = exp(PHIL[i])*X[i]/NZ(exp(PHIV[i])*Y[i]);
    else 
      NewK = exp(Range(-50.0, PHIL[i] - PHIV[i],50.0));
      //NewK = exp(Range(-50.0, PHIV[i] - PHIL[i],50.0)); // Just fiddling
    
    rms = Max(fabs(NewK-K[i]),rms);
    
    K[i] = Damp*K[i] +(1.0-Damp)*NewK;
    //K[i] = Sqrt(K[i]*NewK);
    }

  return rms;
  }

// --------------------------------------------------------------------------

SM_OilStateSet * SM_Oil::FindBestSet(double Temp_, double Pres_)
  {
  int is=-1;
  double D=SMBigDistance/2.0;
  for (int i=0; i<MaxSMOilSets; i++)
    {
    Sets[i].Age++;
    double d=Sets[i].Distance(Temp_, Pres_);
    if (d<D)
      {
      is=i;
      D=d;
      }
    }
  if (is>=0)
    {
    Sets[is].Age=0;
    #if dbgSM_Oil
    if(dbgMemory())
      dbgpln("BestSet %2i D:%12.3e  T:%12.3e  D:%12.3e", is, D, Sets[is].Ts-Temp_, Sets[is].Ps-Pres_);
    #endif
    return &Sets[is];
    }
  return NULL;
  }

// --------------------------------------------------------------------------

SM_OilStateSet * SM_Oil::FindOldSet(double Temp_, double Pres_)
  {
  int is=-1, Age=MaxSMOilSets/2;
  double D=0.0;
  for (int i=0; i<MaxSMOilSets; i++)
    {
    if (!Sets[i].Initialised())
      return &Sets[i];
    double d=Sets[i].Distance(Temp_, Pres_);
    if ((d>D) && (Sets[i].Age>Age))
      {
      is=i;
      D=d;
      Age=Sets[i].Age;
      }
    //if (Sets[i].Age>Age)
    //  {
    //  is=i;
    //  Age=Sets[i].Age;
    //  }
    }
  if (is>=0)
    {
    Sets[is].Age=0;
    return &Sets[is];
    }
  return NULL;
  }

// --------------------------------------------------------------------------

// Attempt to combine the AFS & MJM approach by KCG -----------------------------

flag SM_Oil::ConvergeKratios(pchar Tag, double Temp_, double Pres_)// , SM_OilStateSet * SS)
  {
  int    cnt = 0;
  double deltavf = 1.0e100;

  #if SM_OIL_STATS
  Cnt.CnvKCall++;
  #endif

  Temp_ = Range(110.0,Temp_,425.0);  // kcg Check Root finder NBNB
  Pres_ = Range(1.0,Pres_,1000000.0);
  Pres_ = Range(20.0,Pres_,15000.0); // CNM kcg its starts going mog after this
  //Pres_ = Range(80.0,Pres_,9000.0); // CNM kcg its starts going mog after this

  VFrac = Range(0.0, VFrac, 1.0);

  // Initialise the solution
  SetConstantTcalcs(Temp_);
  InitRqd = 1;

  //double Damp;
  //int Iter;
  
  //Damp=0.0;
  //Iter=0;

  //#if dbgSM_Oil
  //if(dbgConvergence())
  //  dbglock();
  //#endif
  if  (fIdealFlashOnly || (K.GetLen()  < 1))
    {
    VFrac=0.93;
    IdealGasKratio(Temp_,Pres_);

    // CNM This is here to get the solution to started
    // Under Some Situations Ks should maybe be reduced
    K.Mult(1.2);

    #if dbgSM_Oil
    if(dbgConvergence())
      dbgp("%18.9f %18.9f Ideal ", Temp_, Pres_);
    #endif
    }
  else if (1)
    {
    SM_OilStateSet * p=FindBestSet(Temp_, Pres_);
    if (p)
      LoadFrom(*p);
    else
      {
      VFrac=0.93;
      IdealGasKratio(Temp_,Pres_);
      }
    #if dbgSM_Oil
    if(dbgConvergence())
      if (p)
        dbgp("%18.9f %18.9f %14.9f %14.9f ", Temp_, Pres_, p->Ts-Temp_, p->Ps-Pres_);
      else
        dbgp("%18.9f %18.9f %14s %14s ", Temp_, Pres_, "---", "---");
    #endif
    }
  
  // Improve VFrac Estimate
  RachRice_Solve();
  //ConvergeVFrac(False);
  if (fIdealFlashOnly)
    {
    SetXYfromK();
    }
  else
    {
    for(int k=100; k ; k--)
      {
      SetXYfromK();
      #if dbgSM_Oil
      if (dbgKConverge())
        {
        int i;
        double d1,LfNew,VfNew;
        dbgpln(" XX:%10.7f %10.3g", VFrac, RachRice_Value(VFrac, d1, LfNew, VfNew));
        dbgp("    ");
        dbgp(" %10.10s", "Sigma");
        for (i=0; i<CDB.No() ; i++)
          dbgp(" %10.10s", CDB[i].SymOrTag());
        dbgpln("");
        dbgp("  K:");
        dbgp(" %10.10s", "");
        for (i=0; i<CDB.No() ; i++)
          dbgp(" %10.7f", K[i]);
        dbgpln("");
        dbgp("  Z:");
        dbgp(" %10.7f", Z.Sum());
        for (i=0; i<CDB.No() ; i++)
          dbgp(" %10.7f", Z[i]);
        dbgpln("");
        dbgp("  X:");
        dbgp(" %10.7f", X.Sum());
        for (i=0; i<CDB.No() ; i++)
          dbgp(" %10.7f", X[i]);
        dbgpln("");
        dbgp("  Y:");
        dbgp(" %10.7f", Y.Sum());
        for (i=0; i<CDB.No() ; i++)
          dbgp(" %10.7f", Y[i]);
        dbgpln("");
        }
      #endif
    
      double vfr_prv = VFrac;
      double DampV = 0.0;
      double DampF = 0.0;

      // Estimate Ks.
      FugacityKratio(Temp_,Pres_,DampF); 
   
      double XSum=X.Sum();
      int xxx=0;
      // Estimate VFrac
      RachRice_Solve(); 
      //ConvergeVFrac(False);
  
      if ((fabs(vfr_prv-VFrac) < ConvergeKratiosTol))
        break;
    
      VFrac = vfr_prv*DampV + VFrac*(1.0 - DampV);
      } 

    if (k==0)
      LogError("SM_Oil", 0, "ConvergeKRatios not Converged");
    else
      {
      SM_OilStateSet * p=FindOldSet(Temp_, Pres_);
      if (p)
        {
        p->LoadFrom(*this);
        p->SetTP(Temp_, Pres_);
        }
      }
    }

  SetMassAndTemp(Temp_);
  //  if (SO)
//    SO->vs.Save(this);
  #if dbgSM_Oil
  if(dbgConvergence())
    {
    double CurE = msHf(som_ALL,Temp_, Pres_,pMArray());
    dbgpln("H:%12.4f Vf:%12.8f %s", CurE, VFrac*100.0, Tag);
  //  dbgunlock();
    }
  #endif
  

  return True;
  }


// --------------------------------------------------------------------------

/*
These routines have be developed from Fortran in Daubert - Chemical Engineering Thermodynamics
The names have been changed to permit two different sets of code to coexist and not to conceal 
the source
*/

// --------------------------------------------------------------------------

void SM_Oil::SetConstantTcalcs(double T)
  {
  int i;//,j;

  if( fabs(Tsetting - T) > 0.001 )
    {
    Tsetting = T;
    for (i = 0; i < ZLen; i++) 
      {
      if( CDB[i].TCrit() > 1000.0  || CDB[i].TCrit() < 1.0 )
        {
        LogWarning("Flashing",0,"Bad Tcrit value in Specie Database for %s",CDB[i].SymOrTag());
        }
      if( CDB[i].PCrit() > 100000.0 || CDB[i].PCrit() < 1.0  )
        {
        LogWarning("Flashing",0,"Bad Pcrit value in Specie Database for %s",CDB[i].SymOrTag());
        }
      if( CDB[i].ACent() > 2.0 || CDB[i].ACent() < -2.0  )
        {
        LogWarning("Flashing",0,"Bad Acentricity value in Specie Database for %s",CDB[i].SymOrTag());
        }
      }


    for (i = 0; i < ZLen; i++) 
      {
      double TCrit = CDB[i].TCrit();
      double PCrit = CDB[i].PCrit();
      double ACent = CDB[i].ACent();

      A[i] = Ca * Sqr(RGas*TCrit) /(PCrit);   
      S[i] = 0.48 + 1.574 * ACent- 0.176 * Sqr(ACent);
      Alpha[i] = Sqr(1.0 + S[i] * (1.0 - Sqrt(T / TCrit)));
      //ALA[i][i] = A[i] * Alpha[i];
      B[i] = Cb * RGas *  CDB[i].TCrit()/CDB[i].PCrit();
      }
    /*** To Go Back to handle interaction parms
    for(i = 0; i < ZLen; i++)
      {
      for (j = 0; j < ZLen; j++) 
        {
        //  estact to be fleshed page 256 - daubert
        //double VCI = (*VC)[(I) - 1] * 1.0e-6;
        //double VCJ = (*VC)[(J) - 1] * 1.0e-6;
        //ACTION[I][J] = (exp(ln(exp(ln(VCI * VCJ) / 3.0) / 
        //        ((exp(ln(VCI) / 3.0) + exp(ln(VCJ) / 3.0)) / 2.0)) * 3.0));
        ALA[i][j] = sqrt(ALA[i][i] * ALA[j][j]) * (1.0 - CDB.BIP[i][j]);
        }
      }
    **/
    }
  }  

// --------------------------------------------------------------------------
/*
double SM_Oil::Enthalpy_Departure(pchar dbgTag,CDVector& C,double Temp_, double Pres_, flag Vapour)
  {
  double dAsdT = 0.0;
  
  int i,j;
  double Zv    = Vapour ? Zvap : Zliq;
  double HDep  = 0.0;

  double AAM = 0.0;
  double BBM = 0.0;
  for (i = 0; i < ZLen; i++) 
    {
    BBM += B[i] * C[i];
    for (j = 0; j < ZLen; j++)
      {
      AAM += ALA[i][j] * C[i] * C[j];
      }
    }

  for (i = 0; i < ZLen; i++)
    {
    // dasidt[i] =  - sqrt(As[i]* * Fc[i] / (Temp_ * CDB[i].TCrit())) * S[i];
    double fc = A[i];
    f[i] = fc*Alpha[i];
    D[i] = -sqrt(f[i])*sqrt(fc)*S[i]/(CDB[i].TCrit()*sqrt(Temp_/CDB[i].TCrit()));
    }
  dAsdT = 0.0;
  for (i = 0; i < CDB.No(); i++)
    for (j = 0; j < CDB.No(); j++)
      dAsdT = dAsdT + C[i]* C[j]*(1.0 - CDB.BIP(i,j))*(sqrt(f[i]/f[j])*D[j] + sqrt(f[j]/f[i])*D[i]);
  dAsdT = dAsdT / 2.0;
  HDep = ( - AAM + Temp_ * dAsdT) / BBM * log(1.0 + BBM * Pres_ /(Zv*RGas*Temp_))+RGas*Temp_*(Zv-1.0);
  return HDep;
  return 0.0;
  }
*/
// --------------------------------------------------------------------------
/*
double SM_Oil::msEnthalpyZR(PhMask Phase, double T_, double P_, SpMArray * pMA)
  {
  return SpModel::msEnthalpyZR(Phase, T_, P_, pMA);
  }


// --------------------------------------------------------------------------

double SM_Oil::totCp(PhMask Phase, double T_, double P_, SpMArray * pMA)
  {
  SpMArray &MA = (pMA==NULL ? MArray() : *pMA);
  return msCp(Phase, T_, P_, &MA) * MA.Sum(Phase);
  }

// --------------------------------------------------------------------------

double SM_Oil::totEnthalpyZR(PhMask Phase, double T_, double P_, SpMArray * pMA)
  {
  SpMArray &MA = (pMA==NULL ? MArray() : *pMA);
  return msEnthalpyZR(Phase, T_, P_, &MA) * MA.Sum(Phase);
  }

// ---------------------------------------------------------------------------
// SpecHeat calculates the specified phase specific heat in J/mol.K
double SM_Oil::msCp(PhMask Phase, double T_, double P_, SpMArray * pMA)
  {
  return SpModel::msCp(Phase, T_, P_, pMA);
  }
*/
// ---------------------------------------------------------------------------

void SM_Oil::LoadZ()
  {
  LoadZused = True;
  if( CDB.No() != ZLen)
    {
    ValidFlags |= 0x2;
    X.SetSize(CDB.No());
    Y.SetSize(CDB.No());
    Z.SetSize(CDB.No());
    K.SetSize(CDB.No(),0.5);
    A.SetSize(CDB.No());
    B.SetSize(CDB.No());
    S.SetSize(CDB.No());
    Alpha.SetSize(CDB.No());
    ZLen=CDB.No();
    }

  Z.SetAll(MinZValue);
  for (int s=0; s < SDB.No(); s++)
    Z[SDB[s].CId()] += SpMoles(s);
  Z.Normalise();
  }

// ---------------------------------------------------------------------------
// Test Functions

#if SM_OIL_WITHFLASHTEST

flag SM_Oil::DoFlashTest=0;

void SM_Oil::FlashTestGundersonValues()
  {
  typedef struct {  double Ptest;
                    double Ttest;
                    double Result;
                 } Test_Flash;
               
               
  Test_Flash tf[] = 
       {
       {  1.0 * 101.0,       100.0,          0.0     } ,
       {  1.0 * 101.0,       180.0,          0.7725  } ,
       {  1.0 * 101.0,       260.0,          0.8864  } ,
       {  1.0 * 101.0,       340.0,          1.0     } ,
     
       {  8.0 * 101.0,       140.0,          0.0     } ,
       {  8.0 * 101.0,       220.0,          0.7566  } ,
       {  8.0 * 101.0,       300.0,          0.8693  } ,
       {  8.0 * 101.0,       380.0,          1.0     } ,

       { 32.0 * 101.0,       180.0,          0.0     } ,
       { 32.0 * 101.0,       260.0,          0.7251  } ,
       { 32.0 * 101.0,       340.0,          0.8618  } ,
       { 32.0 * 101.0,       400.0,          1.0     } ,

       {128.0 * 101.0,       250.0,          0.0     } ,
       {128.0 * 101.0,       320.0,          0.6178  } ,
       {128.0 * 101.0,       400.0,          1.0     } ,

       {160.0 * 101.0,       280.0,          0.0     } ,
       {160.0 * 101.0,       320.0,          0.4806  } ,
       {160.0 * 101.0,       360.0,          0.7739  } ,
       {160.0 * 101.0,       370.0,          0.8934  } ,

       {172.0 * 101.0,       300.0,          0.0     } ,
       {172.0 * 101.0,       320.0,          0.3500  } ,
       {172.0 * 101.0,       340.0,          0.5722  } ,
       {172.0 * 101.0,       360.0,          1.0     } 
       };

  LoadZ();
  int i;
  for( i = 0; i < CDB.No(); i++ )
    {
    pchar ppp = CDB[i].SymOrTag();
    if( CDB[i].SymOrTag() && strcmp(CDB[i].SymOrTag(),    "N2"            ) == 0 )  Z[i] = 0.0054;
    else
    if( CDB[i].SymOrTag() && strcmp(CDB[i].SymOrTag(),    "MethaneG"      ) == 0 )  Z[i] = 0.7280;
    else
    if( CDB[i].SymOrTag() && strcmp(CDB[i].SymOrTag(),    "EthaneG"       ) == 0 )  Z[i] = 0.0546;
    else
    if( CDB[i].SymOrTag() && strcmp(CDB[i].SymOrTag(),    "PropaneG"      ) == 0 )  Z[i] = 0.0302;
    else
    if( CDB[i].SymOrTag() && strcmp(CDB[i].SymOrTag(),    "NorButaneG"    ) == 0 )  Z[i] = 0.0307;
    else
    if( CDB[i].SymOrTag() && strcmp(CDB[i].SymOrTag(),    "NorPentaneG"   ) == 0 )  Z[i] = 0.0688;
    else
    if( CDB[i].SymOrTag() && strcmp(CDB[i].SymOrTag(),    "PC6"           ) == 0 )  Z[i] = 0.0438;
    else
    if( CDB[i].SymOrTag() && strcmp(CDB[i].SymOrTag(),    "PC8"           ) == 0 )  Z[i] = 0.0375;
    else
    Z[i] = MinZValue;
    }

  for( i = 0; i < CDB.No(); i++ )
    {
    dbgpln("Data to converge   %12s  %12.6f",CDB[i].SymOrTag(),Z[i]);
    }

  char *s1 = "Temp";       
  char *s2 = "Pressure";
  char *s3 = "GundersonVf";
  char *s4 = "Vf";
  char *s41 = "VfDiff%";
  char *s5 = "Zliquid";
  char *s6 = "Zvapour";

  for(i = 0 ; i < 23; i++ )
    {
    //ConvergeKratios("TestFlash",tf[i].Ttest,tf[i].Ptest);
    ConvergeKratios("TestFlash",tf[i].Ttest,tf[i].Ptest);
    dbgpln(" GundersonTest %15.5f , %15.5f , %15.5f , %15.5f , %15.5f , %15.5f , %15.5f",tf[i].Ttest,tf[i].Ptest,tf[i].Result,VFrac,(tf[i].Result-VFrac)*100.0/GTZ(Max(fabs(tf[i].Result), fabs(VFrac))), Zliq,Zvap);
    }
  };

// ---------------------------------------------------------------------------
  
void SM_Oil::FlashTest()
  {
  double PP1=50.0;
  double PP2=15000.0;
  double TT1=C_2_K(-150.0);
  double TT2=C_2_K(250.0);
  double PP,TT;
  static int CSVNo=0;
  Strng Nm;
  Nm.Set("%sFlash%03i.CSV", PrjFiles(),CSVNo++);
  dbgp(" ======================== Dump TO CSV %s ======================== ", Nm());
  FILE *f=fopen(Nm(), "wt");
  if (f)
    {
    fprintf(f, " ");
    if (1)
      {
      for(TT = TT1; TT <= TT2; TT +=  10.0 )
        fprintf(f, ",%.5f ",TT);
      fprintf(f, "\n");
      for(PP = PP1;   PP <= PP2; PP += 200.0 )
        {
        fprintf(f, "%.5f ",PP);
        for(TT = TT1; TT <= TT2; TT +=  10.0 )
          {
          ConvergeKratios("TestFlash",TT,PP);

          //dbgp(",%.5f(%6.4f) ",VFrac,RachRice_Fv);
          fprintf(f, ",%.5f ",VFrac);
          }
        fprintf(f, "\n");
        }
      }
    else
      {
      for(PP = PP1; PP <= PP2; PP +=  200.0 )
        fprintf(f, ",%.5f ",PP);
      fprintf(f, "\n");
      for(TT = TT1;   TT <= TT2; TT += 10.0 )
        {
        fprintf(f, "%.5f ",TT);
        for(PP = PP1; PP <= PP2; PP +=  200.0 )
          {
          ConvergeKratios("TestFlash",TT,PP);

          //dbgp(",%.5f(%6.4f) ",VFrac,RachRice_Fv);
          fprintf(f, ",%.5f ",VFrac);
          }
        fprintf(f, "\n");
        }
      }
    fclose(f);
    }
  }

// --------------------------------------------------------------------------

#endif

// --------------------------------------------------------------------------

void SM_Oil::DbgDump(pchar dbgTag)
  {
  dbgp("T:%7.2f P:%8.2f H:%8.2f ", Temp(), Press(), SpModel::msHf());
  dbgp("F:%8.5f ", VFrac);
  dbgp("K:"); for (int i=0; i<CDB.No(); i++) dbgp("%8.5f ", K[i]); 
  //dbgp("X:"); for (i=0; i<CDB.No(); i++) dbgp("%8.5f ", m.X[i]); 
  //dbgp("Y:"); for (i=0; i<CDB.No(); i++) dbgp("%8.5f ", m.Y[i]); 
  //dbgp("Z:"); for (i=0; i<CDB.No(); i++) dbgp("%8.5f ", Z[i]); 
  dbgpln("%s",dbgTag);
  /*
  int i;
  dbgpln("                      Tag = %s",Tag);
  dbgpln("                 Pressure = %.1fkPa",Pres_);
  dbgpln("              Temperature = %.1fK",Temp_);
  dbgpln(" Calls to Converge Kratio = %d",kvaluscall);
  dbgpln(" Calls to Converge RacRic = %d",racriccall);
  dbgpln("       Max Iter in Kvalus = %d",kvalusiter);
  dbgpln("       Max Iter in RacRic = %d",racriciter);
  dbgpln("          Vapour Fraction = %.5f",VFrac);
  dbgpln("               Liquid 'Z' = %.5f",Zliq);
  dbgpln("               Vapour 'Z' = %.5f",Zvap);
//  dbgpln("     Total Enthalpy = %.5f",Enthalpy);

  dbgpln("%15s, %15s, %15s, %15s, %15s, %15s, %15s",
                              "Species",
                              "Liquid",
                              "Vapour",
                              "X",
                              "Y",
                              "Z",
                              "K");
  
  
  double l=0.0,v=0.0,x=0.0,y=0.0,z=0.0;
  for ( i = 0; i < ZLen; i++ )
    {
    double liq = (1.0 - VFrac)*Z[i];
    double vap = (VFrac      )*Z[i];
    dbgpln("%15s, %15.5f, %15.5f, %15.5f, %15.5f, %15.5f, %15.5f",CDB[i].SymOrTag(),liq,vap,X[i],Y[i],Z[i],K[i]); 
    l += liq;
    v += vap;
    x += X[i];
    y += Y[i];
    z += Z[i];
    }
    dbgpln("%15s, %15.5f, %15.5f, %15.5f, %15.5f, %15.5f","Totals",l,v,x,y,z); 
  */
  };

//--------------------------------------------------------------------------

class QPFnd : public MRootFinderBase
  {
  public:
    SM_Oil    &Om;                                           
    double    P1;

    QPFnd(SM_Oil * pM, double P1_, double RqdTol) : MRootFinderBase("QPFnd", RqdTol), Om(*pM)
      { P1=P1_; }
    char * ObjTag() { return Om.FullObjTag(); };
    double Function(double x)
      { 
      // x is the Temp
      Om.ConvergeKratios("QPFnd",x,P1);
      //Om.SetMassAndTemp(x);
      double H=Om.msHf(som_ALL,x,P1,Om.pMArray());
      return H;
      };
  };

//--------------------------------------------------------------------------

QPF_Modes SM_Oil::QPFlash(pSpModel pVap, double P1, double Duty, flag SaturationLimit, double *pOverDuty) 
  { DoBreak(); return QPF_Error;};

QPF_Modes SM_Oil::QPFlash(double P1, double Duty, flag SaturationLimit, double *pOverDuty)
  {
  LoadZ();

  #if SM_OIL_WITHFLASHTEST
  if (DoFlashTest)
    {
    FlashTest();
    DoFlashTest=0;
    }
  #endif

  #if SM_OIL_STATS
  Cnt.Reset();
  #endif

  double T0=Temp();
  double P0=Press();

  #if dbgSM_Oil
  if (dbgQPFlash())
    dbgp(" QPflash Input Vector T0:%-12.5f P0:%-12.5f P1:%-12.5f %s",T0,P0,P1, FullObjTag());
  #endif

  #if dbgSM_Oil
  if (dbgQPFlash())
    dbgpln(" Output Vector T0:%-12.5f P0:%-12.5f P1:%-12.5f",T0,P0,P1);
  #endif

  // Converge Ks @ Initial conditions
  //if (QPm.Init.ValidState())
  //  LoadFrom(QPm.Init);
  ConvergeKratios("InitCond",T0,P0);
  //SetMassAndTemp(T0);
  double DstE = msHf(som_ALL,T0,P0,pMArray()) + Duty;
  double T1=T0, CurE;
  //SaveTo(QPm.Init);


  // Converge Ks & Temp @ Final conditions such that Enthalpy is conserved
  SetPress(P1);

  if (Valid(QPm.Temp))
    {
    T1=QPm.Temp;
    QPm.Temp=dNAN();
    //if (QPm.Final.ValidState())
    //  LoadFrom(QPm.Final);
    }

  ConvergeKratios("CurCond",T1,P1);
  //SetMassAndTemp(T1);
  CurE = msHf(som_ALL,T1,P1,pMArray());

  const double BigTStep=25.0;
  const double TStepMult=4.0;
  double deltaT = BigTStep/(TStepMult*TStepMult*TStepMult);
  double tl, tr, fl, fr;
  tl=T1;
  tr=T1;
  fl=CurE;
  fr=CurE;

  // look for straddle point
  if( DstE > CurE )
    {
    while( DstE > CurE )
      {
      tl=T1;
      fl=CurE;
      T1 = T1 + deltaT;
      tr=T1;
      ConvergeKratios("FindStraddleUP",T1,P1);
      //SetMassAndTemp(T1);
      CurE = msHf(som_ALL,T1,P1,pMArray());
      fr=CurE;
      deltaT=Min(deltaT*TStepMult, BigTStep);
      }
    }
  else
    {
    while( DstE < CurE && tl>C_2_K(-100.0))
      {
      tr=T1;
      fr=CurE;
      T1 = T1 - deltaT;
      tl=T1;
      ConvergeKratios("FindStraddleDN",T1,P1);
      ///SetMassAndTemp(T1);
      CurE = msHf(som_ALL,T1,P1,pMArray());
      fl=CurE;
      deltaT=Min(deltaT*TStepMult, BigTStep);
      }
    }
  
  // solve using Brents algorithm
  QPFnd QPF(this, P1, QPFlashTol*fabs(fl-fr));
  QPF.SetTarget(DstE);
  if (QPF.Start(tl, tr, fl, fr)==RF_OK)
    {
    if (QPF.Solve_Brent()==RF_OK)
      {
      QPm.Temp=QPF.Result();
      //SaveTo(QPm.Final);
      }
    else
      LogError("SM_Oil", 0, "QPF Not Solved");
    }
  else
    LogError("SM_Oil", 0, "QPF Not Started");

  #if dbgSM_Oil && SM_OIL_STATS
  if (dbgStats())
    Cnt.Write(Temp(), Press());
  #endif

  return QPF_Sensible;
  }

// ---------------------------------------------------------------------------
/**
QPF_Modes SM_Oil::QFlash(double SpaceVolume, double Duty)
  {
  LoadZ();
  return QPF_Sensible;

  #if SM_OIL_WITHFLASHTEST
  if (DoFlashTest)
    {
    FlashTest();
    DoFlashTest=0;
    }
  #endif

  
  double T=QTLast;
  double P=QPLast, P1;
  double H0=SpModel::msHf(), H1;
  if(!Qflashdone )
    {
    T=Temp();
    P=Press();
    }
dbgpln("=========");
  int Dir=0;
  double XX=0.8;
  P1=P;
  while (1)//fabs(T-QTLast)>1.0e-3)
    {
    //tTemp(T);
    while (1)//for (int i=0; i<5; i++)
      {
      P+=0.01*(P1-P);
      //if ()
      ConvergeKratios("SM_Oil", T,P);
      SetMassAndTemp(T);
      P1=Press();
dbgpln("  %8.2f  %8.3f  %8.3f",T,P,P1);
      if (fabs(P-P1)<0.01)
        break;
      }
    
    H1=SpModel::msHf();
    if (fabs(H1-H0)<1.0e-3)
      break;
    if (Dir!=Sign(H1-H0))
      XX=1.0-(1.0-XX)*0.5;
    Dir=Sign(H1-H0);
    if (H1>H0)
      T*=XX;
    else
      T/=XX;
    }

  QTLast=T;
  QPLast=P;
  Qflashdone = True;
  
  return QPF_Sensible;
  }
*/
// ---------------------------------------------------------------------------

double SM_Oil::TPFlash(double T1, double P1, double &Duty)
  {
  LoadZ();
  
  #if SM_OIL_WITHFLASHTEST
  if (DoFlashTest)
    {
    FlashTest();
    DoFlashTest=0;
    }
  #endif

  ConvergeKratios(FullObjTag(),T1,P1);
  #if dbgSM_Oil
  if(dbgTPFlash())
    DbgDump(FullObjTag());
  #endif
  //SetMassAndTemp(T1);

  return SpModel::totHf(); 
  }


// ===========================================================================
// Debug
/*
void SM_Oil::WriteDebug (pchar Tag, int Iter, double ErrK, double VFrac_, double Temp_, double Pres_, double Duty_)
  {
  double XP, YP, ZP;
  char xxx[50];
  strcpy(xxx,"                        ");
  strset(xxx, (Iter>=0) ? '-' : '=');
  dbgpln("%s %s %s", xxx,Tag,xxx);

  dbgnln();
         
  dbgpln("Temperature     : %7.2f K",Temp_);
  dbgpln("Pressure        : %7.2f kPa",Pres_);
  dbgpln("Vapour Fraction :%8.4f",VFrac_);
  dbgpln("Duty            :%8.1f",Duty_);
  if (Iter >=0)
    {
    dbgpln("Iteration       :%3i",Iter);
    dbgpln("Error           :%14g",ErrK);
    }
  dbgpln("");

  dbgpln("      Feed    Vapour    Liquid           K Component");
  for (int I=0; I<CDB.No(); I++)
    {
    XP = 100 * X[I];
    if (XP < 0.0005) XP = 0;
    YP = 100 * Y[I];
    if (YP < 0.0005) YP = 0;
    ZP = 100 * Z[I];

    dbgp("%10.2f%10.3f%10.3f",ZP,YP,XP);

    if ((K[I] > 1.0E-4) && (K[I] < 1.0E4))
      dbgp("%12.5f",K[I]);
    else   
      dbgp("%14g",K[I]);
    dbgpln(" %s",CDB[I].Name);
    }

  dbgpln("%s %s %s", xxx,Tag,xxx);
  };
*/
// ===========================================================================

void SM_Oil::SetMassAndTemp(double Temp_)
  {
  // NB Must enforce mass closure

  PhMask Msk=som_Vap|som_Liq;
  double Mo_t=Moles(Msk);
  for (int c=0; c < CDB.No(); c++)
    {
    rComponentD Cd=CDB[c];
    if (Cd.vi()>=0 && Cd.li()>=0)
      {
      double m=M[Cd.vi()]+M[Cd.li()];
      M[Cd.vi()]=Mo_t*VFrac*Y[c]*SDB[Cd.vi()].MoleWt();
      M[Cd.li()]=Max(0.0, m-M[Cd.vi()]);
      }
    }

  //M[H2o.vi()]=H2OVap;
  //M[H2o.li()]=H2OLiq;
  
  ClrMStatesOK();
  SetTemp(Temp_);
  }

// ===========================================================================