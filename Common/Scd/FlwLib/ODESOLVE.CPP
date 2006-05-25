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
#define  __ODESOLVE_CPP
#include "tagobj.h"
#include "odesolve.h"
#include "errorlog.h"
#include "dbgmngr.h"
#include "executiv.h"

#define dbgODESolve          WITHDEBUG
#if dbgODESolve
static CDbgMngr dbgShowEvals      ("ODESolve", "ShowEvals");
static CDbgMngr dbgSteps          ("ODESolve", "Steps");
static CDbgMngr dbgStepsDetail    ("ODESolve", "StepsDetail");
static CDbgMngr dbgStepsNext      ("ODESolve", "StepsNext");
static CDbgMngr dbgStepsDerivs    ("ODESolve", "StepsDerivs");
static CDbgMngr dbgDumpDerivs     ("ODESolve", "DumpDerivs");
FILE *hDumpFile =NULL;
#endif

#define dbgIntegrator            0
#define dbgStepSizeDetailTest  0.1

#define dbgHistory               0
#define dbgLoHidV                0

#include "debugnew.h" // must follow all IMPLEMENT_DYNCREATE & IMPLEMENT_SERIAL

IMPLEMENT_MEMSTATS(CStateSave)
IMPLEMENT_MEMSTATS(CStateInt)
IMPLEMENT_MEMSTATS(CTimeStateBlock)
IMPLEMENT_MEMSTATS(ODESolverBase)
IMPLEMENT_MEMSTATS(CODESolver)

//===========================================================================

#define PGROW          -0.20
#define PSHRINK        -0.25
#define FCOR           0.066666666666666666667 /* 1.0/15.0*/
#define SAFETY         0.9
//#define SAFETY         0.8
#define ERRCON         6.0e-4
#define TINY           1.0e-30

#define FRACERRORLIMIT 0.0

//===========================================================================

long   CStateSave::m_Ids=0;
long   CStateSave::m_nTotal=0;
long   CStateInt::m_nTotal=0;

//---------------------------------------------------------------------------

CStateSave::CStateSave()
  {
  m_Id=++m_Ids;
  m_nTotal++;
  m_pV=NULL;
  m_Vi=0.0;
  m_Vs=0.0;
  }

//---------------------------------------------------------------------------

CStateSave::~CStateSave()
  {
  m_nTotal--;
  }

void CStateSave::Attach(double * V)
  {
  m_pV=V;
  };

//---------------------------------------------------------------------------

CStateInt::CStateInt()
  {
  m_nTotal++;

  m_pVTst=NULL;
  m_pDV=NULL;

  m_TolTestOn=True;
  m_Vi=0.0;
  m_Vt=0.0;

  m_Vs=0.0;
  m_DVs=0.0;
  m_DVi=0.0;
  m_DVm=0.0;
  m_DVt=0.0;
  }

//---------------------------------------------------------------------------

CStateInt::~CStateInt()
  {
  m_nTotal--;

//  for (int i=0;i<CODESolver::BadTolList.Len;i++)
//    if (CODESolver::BadTolList.List[i]==this)
//      {
//      if (i<CODESolver::BadTolList.Len)
//        memmove(&CODESolver::BadTolList.List[i], &CODESolver::BadTolList.List[i+1],
//                (CODESolver::BadTolList.Len-(i+1))*sizeof(CODESolver::BadTolList.List[0]));
//      CODESolver::BadTolList.Len--;
//      break;
//      }
//  for (i=0;i<CODESolver::m_BadLim.Len;i++)
//    if (CODESolver::m_BadLim.List[i]==this)
//      {
//      if (i<CODESolver::m_BadLim.Len)
//        memmove(&CODESolver::m_BadLim.List[i], &CODESolver::m_BadLim.List[i+1],
//                (CODESolver::m_BadLim.Len-(i+1))*sizeof(CODESolver::m_BadLim.List[0]));
//      CODESolver::m_BadLim.Len--;
//      break;
//      }
  }

//---------------------------------------------------------------------------

void CStateInt::Attach(double * V, double * dV, double * VTst)
  {
  CStateSave::Attach(V);
  m_pVTst=VTst;
  m_pDV=dV;
  };

//===========================================================================
//
//
//
//===========================================================================

long CODEDataBlock::SaveStart(CODEDataBlock & ODB, CStateArray & States) 
  { 
  for (int i=0; i<States.NInts(); i++)
    {
    CStateInt &I=States.Int(i);
    I.m_Vs   = *I.m_pV;
    I.m_DVs  = *I.m_pDV;
    }
  for (i=0; i<States.NSaves(); i++)
    {
    CStateSave &S=States.Save(i);
    S.m_Vs   = *S.m_pV;
    }
  return 0;
  };

long CODEDataBlock::LoadStart(CODEDataBlock & ODB, CStateArray & States) 
  { 
  for (int i=0; i<States.NInts(); i++)
    {
    CStateInt &I=States.Int(i);
    *I.m_pV    = I.m_Vs;
    *I.m_pDV   = I.m_DVs;
    }
  for (i=0; i<States.NSaves(); i++)
    {
    CStateSave &S=States.Save(i);
    *S.m_pV    = S.m_Vs;
    }
  return 0;
  };

long CODEDataBlock::SaveIntermediate(CODEDataBlock & ODB, CStateArray & States) 
  { 
  for (int i=0; i<States.NInts(); i++)
    {
    CStateInt &I=States.Int(i);
    I.m_Vi   = *I.m_pV;
    I.m_DVi  = *I.m_pDV;
    }
  for (i=0; i<States.NSaves(); i++)
    {
    CStateSave &S=States.Save(i);
    S.m_Vi   = *S.m_pV;
    }
  return 0;
  };

long CODEDataBlock::SaveEP(CODEDataBlock & ODB, CStateArray & States) 
  { 
  for (int i=0; i<States.NInts(); i++)
    {
    CStateInt &I=States.Int(i);
    I.m_VEP   = *I.m_pV;
    }
  for (i=0; i<States.NSaves(); i++)
    {
    CStateSave &S=States.Save(i);
    S.m_VEP   = *S.m_pV;
    }
  return 0;
  };

long CODEDataBlock::LoadEP(CODEDataBlock & ODB, CStateArray & States) 
  { 
  for (int i=0; i<States.NInts(); i++)
    {
    CStateInt &I=States.Int(i);
    *I.m_pV   = I.m_VEP;
    }
  for (i=0; i<States.NSaves(); i++)
    {
    CStateSave &S=States.Save(i);
    *S.m_pV   = S.m_VEP;
    }
  return 0;
  };

long CODEDataBlock::CalcErrors(CODEDataBlock & ODB, CStateArray & States) 
  { 
  CBadStateList & BadTolList=ODB.BadTolList();

  for (int i=0; i<States.NInts(); i++)
    {
    CStateInt &I=States.Int(i);
      //for (pIntegrator I=It.First(); (I) ; I=It.Next())

    I.m_TolTestFail=0;
    switch (ODB.StepSizeControl())
      {
      case ODE_SSC_Var_1:
        I.m_Vt  = *I.m_pV - I.m_Vi;
        break;
      case ODE_SSC_Var_2:
        I.m_Vt  = *I.m_pV - (I.m_Vs + 0.5*I.m_DVm*ODB.m_TimeInc);
        break;
      }
    if (I.m_TolTestOn)
      {
      //double temp=fabs(I.m_V_t)/(fabs(I.m_V) * m_Mag_Eps_R + m_Mag_Eps_A);
      // below is a slightly less rigorous tolerance when V is approaching Zero.
      //double FracErr=fabs(I.m_V_t)/(Max(fabs(I.m_V), fabs(I.m_V_s)) * m_Mag_Eps_R + m_Mag_Eps_A);
      I.m_dFracErr=fabs(I.m_Vt)/(fabs(*I.m_pVTst) * ODB.Mag_Eps_R() + ODB.Mag_Eps_A());
      //if (!Valid(FracErr))
      //  dbgpln("NAN Mag %14.6g %s", I.m_Vt, I.m_sTag());
  
      if (Valid(I.m_dFracErr))
        {
        I.m_TolTestFail=(I.m_dFracErr>=1.0);
  
        if (ODB.ErrMax() < I.m_dFracErr)
          ODB.SetErrMax(I.m_dFracErr);
        if (I.m_dFracErr>=FRACERRORLIMIT)
          {
          int i;
          for (i=BadTolList.Len; i>0; i-- )
            if (BadTolList.List[i-1]->m_dFracErr >= I.m_dFracErr)
              break;
          if (i<BadListMax)// && BadTolList.Len<BadListMax)
            {
            if (i<BadListMax-1)
              memmove(&BadTolList.List[i+1], &BadTolList.List[i], sizeof(BadTolList.List[0])*(BadListMax-(i+1)));
            BadTolList.List[i]=&I;
            BadTolList.Len=Min(BadTolList.Len+1, BadListMax);
            
            }
          }
        }
      }
    I.m_TolTestOn=I.m_TolTestRqd;
    }

  //#if dbgODESolve
  //if (dbgSteps())
  //  {
  //  int i;
  //  char buff[256];
  //  dbgpln(3,"CODESolver::StepSize %s m_Time:%s  m_TimeInc:%7.3f  error:%10.4g ",
  //        (ErrMax <= 1.0) ? "  OK   ":"TOO BIG", SecstoHMS(Time_s, buff), rIC.m_TimeInc, ErrMax);
  //  if (dbgStepsDetail())
  //    for (i=0 ; i < BadTolList.Len;i++)
  //      if ((!Valid(BadTolList.List[i]->dFracErr) || BadTolList.List[i]->dFracErr >= dbgStepSizeDetailTest))
  //        {
  //        dbgpln(3,"  Mag %7.2f %14.6g > %14.6g %s [%14.6g]", BadTolList.List[i]->dFracErr, BadTolList.List[i]->V_s, BadTolList.List[i]->V, BadTolList.List[i]->sTag(), BadTolList.List[i]->V_t);
  //        if (dbgStepsDerivs())
  //          dbgpln(2,"            s:%14.6g m:%14.6g e:%14.6g ", BadTolList.List[i]->dV_s,BadTolList.List[i]->dV_m*0.5,BadTolList.List[i]->dV);
  //        }
  //  }
  //#endif
  return 0;
  };

long CODEDataBlock::Correct1(CODEDataBlock & ODB, CStateArray & States) 
  { 
  for (int i=0; i<States.NInts(); i++)
    {
    CStateInt &I=States.Int(i);
    *I.m_pV += I.m_Vt*FCOR;
    }
  return 0;
  };

long CODEDataBlock::Correct2(CODEDataBlock & ODB, CStateArray & States) 
  { 
  for (int i=0; i<States.NInts(); i++)
    {
    CStateInt &I=States.Int(i);
    // Null
    }
  return 0;
  };

long CODEDataBlock::Converge(CODEDataBlock & ODB, CStateArray & States) 
  { 
  for (int i=0; i<States.NInts(); i++)
    {
    CStateInt &I=States.Int(i);
    // Null
    }
  return 0;
  };

// ----
long CODEDataBlock::EulerAdvance(CODEDataBlock & ODB, CStateArray & States)
  {   
  for (int i=0; i<States.NInts(); i++)
    {
    CStateInt &I=States.Int(i);
    *I.m_pV = I.m_Vs + *I.m_pDV*ODB.m_TimeInc;
    }
  return 0;
  };

long CODEDataBlock::EulerFixDV(CODEDataBlock & ODB, CStateArray & States)
  {   
  for (int i=0; i<States.NInts(); i++)
    {
    CStateInt &I=States.Int(i);
    *I.m_pDV   = (*I.m_pV-I.m_Vs)/ODB.m_TimeInc;
    I.m_DVt = *I.m_pDV;
    }
  return 0;
  };

// ----
long CODEDataBlock::RK2AdvanceA(CODEDataBlock & ODB, CStateArray & States)
  {  
  double HalfDT=0.5*ODB.m_TimeInc;
  for (int i=0; i<States.NInts(); i++)
    {
    CStateInt &I=States.Int(i);
    *I.m_pV = I.m_Vs + *I.m_pDV*HalfDT;
    }
  return 0;
  };

long CODEDataBlock::RK2FixDVA(CODEDataBlock & ODB, CStateArray & States)
  {   
  double HalfDT=0.5*ODB.m_TimeInc;
  for (int i=0; i<States.NInts(); i++)
    {
    CStateInt &I=States.Int(i);
    *I.m_pDV   = (*I.m_pV-I.m_Vt)/HalfDT;
    I.m_DVt   = *I.m_pDV;
    }
  return 0;
  }

long CODEDataBlock::RK2AdvanceB(CODEDataBlock & ODB, CStateArray & States)
  {   
  for (int i=0; i<States.NInts(); i++)
    {
    CStateInt &I=States.Int(i);
    *I.m_pV = I.m_Vs + *I.m_pDV*ODB.m_TimeInc;

    }
  return 0;
  };

long CODEDataBlock::RK2FixDVB(CODEDataBlock & ODB, CStateArray & States)
  {   
  for (int i=0; i<States.NInts(); i++)
    {
    CStateInt &I=States.Int(i);
    *I.m_pDV   = (*I.m_pV-I.m_Vt)/ODB.m_TimeInc;
    }
  return 0;
  }

// ----
long CODEDataBlock::RK4AdvanceA(CODEDataBlock & ODB, CStateArray & States)
  {   
  double HalfDT=0.5*ODB.m_TimeInc;
  for (int i=0; i<States.NInts(); i++)
    {
    CStateInt &I=States.Int(i);
    I.m_Vt      = *I.m_pV;
    I.m_DVt     = *I.m_pDV;
    *I.m_pV     = I.m_Vt + *I.m_pDV*HalfDT;
    }
  return 0;
  };

long CODEDataBlock::RK4FixDVA(CODEDataBlock & ODB, CStateArray & States)
  {   
  double HalfDT=0.5*ODB.m_TimeInc;
  for (int i=0; i<States.NInts(); i++)
    {
    CStateInt &I=States.Int(i);
    *I.m_pDV   = (*I.m_pV-I.m_Vt)/HalfDT;
    I.m_DVt = *I.m_pDV;
    }
  return 0;
  }

long CODEDataBlock::RK4AdvanceB(CODEDataBlock & ODB, CStateArray & States)
  {   
  double HalfDT=0.5*ODB.m_TimeInc;
  for (int i=0; i<States.NInts(); i++)
    {
    CStateInt &I=States.Int(i);
    I.m_DVm     = *I.m_pDV;
    *I.m_pV        = I.m_Vt + *I.m_pDV*HalfDT;
    }
  return 0;
  };

long CODEDataBlock::RK4FixDVB(CODEDataBlock & ODB, CStateArray & States)
  {   
  double HalfDT=0.5*ODB.m_TimeInc;
  for (int i=0; i<States.NInts(); i++)
    {
    CStateInt &I=States.Int(i);
    *I.m_pDV   = (*I.m_pV-I.m_Vt)/HalfDT;
    I.m_DVm = *I.m_pDV;
    }
  return 0;
  }

long CODEDataBlock::RK4AdvanceC(CODEDataBlock & ODB, CStateArray & States)
  {   
  for (int i=0; i<States.NInts(); i++)
    {
    CStateInt &I=States.Int(i);
    *I.m_pV   = I.m_Vt + *I.m_pDV*ODB.m_TimeInc;
    }
  return 0;
  };

long CODEDataBlock::RK4FixDVC(CODEDataBlock & ODB, CStateArray & States)
  {   
  for (int i=0; i<States.NInts(); i++)
    {
    CStateInt &I=States.Int(i);
    *I.m_pDV   = (*I.m_pV-I.m_Vt)/ODB.m_TimeInc;
    I.m_DVm += *I.m_pDV;
    }
  return 0;
  }

long CODEDataBlock::RK4AdvanceD(CODEDataBlock & ODB, CStateArray & States)
  {   
  for (int i=0; i<States.NInts(); i++)
    {
    CStateInt &I=States.Int(i);
    *I.m_pV   = I.m_Vt + *I.m_pDV*ODB.m_TimeInc;
    }
  return 0;
  };

long CODEDataBlock::RK4FixDVD(CODEDataBlock & ODB, CStateArray & States)
  {   
  for (int i=0; i<States.NInts(); i++)
    {
    CStateInt &I=States.Int(i);
    *I.m_pDV     = (*I.m_pV-I.m_Vt)/ODB.m_TimeInc;
    }
  return 0;
  }

long CODEDataBlock::RK4AdvanceE(CODEDataBlock & ODB, CStateArray & States)
  {   
  double h6=ODB.m_TimeInc/6;
  for (int i=0; i<States.NInts(); i++)
    {
    CStateInt &I=States.Int(i);
    *I.m_pV        = I.m_Vt + h6*(I.m_DVt+2.0*I.m_DVm+*I.m_pDV);
    }
  return 0;
  };

//===========================================================================
//
//
//
//===========================================================================

CTimeStateBlock::CTimeStateBlock(double Time_, double dTime_)
  {
  m_Time       = Time_;
  m_TimeInc     = dTime_;
  m_TimeIncNext = dNAN;
  m_TimeIncInit = 1.0;
  m_Flags      = 0;
  m_Desc       = NULL;
  //bHoldAdv   = false;

  m_TimeIncMxRqd = 1.0;
  m_TimeIncMx    = 1.0;
  m_TimeIncMnRqd = 0.001;
  m_TimeIncMn    = 0.001;
  m_DerivEstInc = 0.00001;
  m_SettleTime  = 0.0;
  };

//---------------------------------------------------------------------------

void CTimeStateBlock::AdvanceTime(double Advance)
  {
  m_Time+=Advance;
  };

//---------------------------------------------------------------------------

void CTimeStateBlock::SetTime(CTimeValue Time_)
  {
  //__debugbreak(); // how often does this get called ????????
  m_Time=Time_.Seconds;
  };
void CTimeStateBlock::SetTimeInc(CTimeValue dTime_)
  {
  m_TimeInc=GTZ(dTime_.Seconds);
  m_TimeIncNext=dNAN;
  }
void CTimeStateBlock::SetTimeIncMxRqd(CTimeValue dTimeMxRqd_)
  {
  m_TimeIncMxRqd=Max(1.0e-6, dTimeMxRqd_.Seconds);
  };
void CTimeStateBlock::SetTimeIncMnRqd(CTimeValue dTimeMnRqd_)
  {
  m_TimeIncMnRqd=Max(1.0e-9, dTimeMnRqd_.Seconds);
  };
void CTimeStateBlock::SetDerivEstInc(double DerivEstInc)
  {
  m_DerivEstInc=DerivEstInc;
  };
void CTimeStateBlock::SetSettleTime(double SettleTime)
  {
  m_SettleTime=SettleTime;
  };

//===========================================================================

CODESolver::CODESolver(CTimeStateBlock*pIC, double dTime_, double dTimeMax_, int MaxIters, ODE_Methods Method_, ODE_SSC_Type StepSizeControl_) :
  rIC(*pIC),
  m_MagEps(TBF_DynSys, "ODESolve:MagEPS", 1.0e-4,1.0e-6, 100, TBF_UseAbs|TBF_UseRel)
  {
  pODEBase=NULL;

  rIC.m_TimeIncMn=rIC.m_TimeInc;
  rIC.m_TimeIncMnRqd=rIC.m_TimeInc;
  dTimeWrk=rIC.m_TimeInc;
  Time_Stop=0.0;
  m_nGoodIters=0;
  m_nBadIters=0;
  m_nIters4Step=0;
  SetMethod(Method_);
  SetStepSizeControl(StepSizeControl_);
  rIC.m_TimeIncMx=dTimeMax_;
  rIC.m_TimeIncMxRqd=dTimeMax_;
  m_nMaxIters=MaxIters;
  SafetyScale=1.0;

//  m_Mag_Eps_R=dNAN;
//  m_Mag_Eps_A=dNAN;

  SetMagConvergence(1.0e-4,1.0e-6);

  GrowLimit=0.5;
  Grow=1.1;

  OKShrinkLimit=0.8;
  OKShrink=0.95;

  BADShrink=0.8;

  dEstMaxDT=0.0;
  fStepTooLarge=False;

  };

//---------------------------------------------------------------------------

CODESolver::~CODESolver()
  {
  };

//---------------------------------------------------------------------------

void CODESolver::SetMethod(byte Method_)
  {
  m_iMethod=Method_;
  // remove old implicit method
  if (m_iMethod>ODE_RK4)
    m_iMethod=ODE_Euler;
  }

//---------------------------------------------------------------------------

void CODESolver::SetStepSizeControl(byte StepSizeControl_)
  {
  m_iStepSizeControl = StepSizeControl_;
  }

//---------------------------------------------------------------------------

void CODESolver::SetMagConvergence(double Eps_R__, double Eps_A__)
  {
  m_MagEps.GetAbsDD().dVal=Eps_R__;
  m_MagEps.GetAbsDD().dVal=Eps_A__;
  };


double CODESolver::GetMagEPSAbs() { return m_MagEps.GetAbs(); };
double CODESolver::GetMagEPSRel() { return m_MagEps.GetRel(); };
void CODESolver::SetMagEPSAbs(double Eps)
  {
  m_MagEps.GetAbsDD().dVal=Eps;
  }

void CODESolver::SetMagEPSRel(double Eps)
  {
  m_MagEps.GetRelDD().dVal=Eps;
  }

//---------------------------------------------------------------------------

void CODESolver::SaveStart()
  {
  Time_s=rIC.m_Time;
  
  CODEDataBlock ODB(this);
  ODB.Set(eStateSave, -1, CODEDataBlock::SaveStart, pODEBase);
  }

//---------------------------------------------------------------------------

void CODESolver::SetLimits()
  {
  }

//---------------------------------------------------------------------------

void CODESolver::SaveIntermediate()
  {
  Time_i=rIC.m_Time;

  CODEDataBlock ODB(this);
  ODB.Set(eStateSave, -1, CODEDataBlock::SaveIntermediate, pODEBase);
  }

//---------------------------------------------------------------------------

void CODESolver::ReStart()
  {
  rIC.m_Time=Time_s;   

  CODEDataBlock ODB(this);
  ODB.Set(eStateLoad, -1, CODEDataBlock::LoadStart, pODEBase);
  }

//---------------------------------------------------------------------------

flag CODESolver::CalculateErrors()
  {
  m_ErrMax=0.0;

  for (int i=0; i<m_BadTolList.Len; i++)
    m_BadTolCopy.List[i]=m_BadTolList.List[i];
  m_BadTolCopy.Len=m_BadTolList.Len;
  
  m_BadTolList.Len=0;

  CODEDataBlock ODB(this);
  ODB.Set(eStateTest, -1, CODEDataBlock::CalcErrors, pODEBase);

  return 0;
  }

//---------------------------------------------------------------------------

flag CODESolver::StepSizeOK_1()
  {
  if (m_ErrMax <= 1.0)
    {
    if (rIC.m_TimeInc < 0.9*rIC.m_TimeIncInit) // cnm
      rIC.m_TimeIncNext = rIC.m_TimeInc;
    else
      rIC.m_TimeIncNext = (m_ErrMax > ERRCON ? SafetyScale*SAFETY*rIC.m_TimeInc*exp(PGROW*log(m_ErrMax)):2.0*rIC.m_TimeInc);
    rIC.m_TimeIncNext = Min(rIC.m_TimeIncNext, rIC.m_TimeIncMx);

    CODEDataBlock ODB(this);
    ODB.Set(eStateAdvance, -1, CODEDataBlock::Correct1, pODEBase);
    return 1;
    }

  rIC.m_TimeIncRestart = Range(rIC.m_TimeInc*0.1, SafetyScale*SAFETY*rIC.m_TimeInc*exp(PSHRINK*log(m_ErrMax)), rIC.m_TimeIncMx);
  fStepSizeTooSmall = (rIC.m_TimeInc <= 1.0001*rIC.m_TimeIncMn);
  return 0;
  }

//---------------------------------------------------------------------------

flag CODESolver::StepSizeOK_2()
  {
  if (m_ErrMax <= 1.0)
    {
    if (m_ErrMax < GrowLimit)
      {
//      rIC.m_TimeIncNext = rIC.m_TimeInc*Grow;
//      rIC.m_TimeIncNext = rIC.m_TimeInc*Grow *Min(1.0+0.25*log(GrowLimit/GTZ(ErrMax)), 2.0);
//      rIC.m_TimeIncNext = rIC.m_TimeInc*Grow*Min(1.0+0.10*log(GrowLimit/GTZ(ErrMax)), 2.0);
      rIC.m_TimeIncNext = rIC.m_TimeInc*(1.0+0.10*log(GrowLimit/GTZ(m_ErrMax)));
      rIC.m_TimeIncNext = Min(rIC.m_TimeIncNext, Min(rIC.m_TimeInc*2.0, rIC.m_TimeIncMx));
      #if dbgODESolve
      if (dbgStepsNext())
        dbgpln("NextStep:Grow   Err:%11.5g dT:%8.5f > %8.5f",m_ErrMax,rIC.m_TimeInc,rIC.m_TimeIncNext);
      #endif
      }
    else if (m_ErrMax > OKShrinkLimit)
      {
//      rIC.m_TimeIncNext = rIC.m_TimeInc*OKShrink;
      rIC.m_TimeIncNext = rIC.m_TimeInc*(1.0+(1.0-OKShrink)*(m_ErrMax - OKShrinkLimit)/(1.0 - OKShrinkLimit));
      #if dbgODESolve
      if (dbgStepsNext())
        dbgpln("NextStep:Shrink Err:%11.5g dT:%8.5f < %8.5f",m_ErrMax,rIC.m_TimeInc,rIC.m_TimeIncNext);
      #endif
      }
    else
      {
      rIC.m_TimeIncNext = rIC.m_TimeInc;
      #if dbgODESolve
      if (dbgStepsNext())
        dbgpln("NextStep:Hold   Err:%11.5g dT:           %8.5f",m_ErrMax,rIC.m_TimeInc);
      #endif
      }

    CODEDataBlock ODB(this);
    ODB.Set(eStateAdvance, -1, CODEDataBlock::Correct2, pODEBase);

    return 1;
    }

  double ndt = rIC.m_TimeInc*BADShrink /(1.0+Min(0.25*log(m_ErrMax), 1.0));
  #if dbgODESolve
  if (dbgStepsNext())
    dbgpln("NextStep:Redo   Err:%11.5g dT:%8.5f @ %8.5f",m_ErrMax,rIC.m_TimeInc,ndt);
  #endif
  rIC.m_TimeIncRestart = ndt;
  fStepSizeTooSmall = (rIC.m_TimeInc < rIC.m_TimeIncMn);//MINSTEPSIZE);
  return 0;
  }

//---------------------------------------------------------------------------

void CODESolver::Initialise()
  {
  pODEBase=NULL;
  m_nGoodIters=0;
  m_nBadIters=0;
  m_nIters4Step=0;
  }

//---------------------------------------------------------------------------

void CODESolver::StartUp(ODESolverBase *ODEBase)
  {
  pODEBase=ODEBase;
  /*double dTMem=rIC.GetTimeInc();
  rIC.SetTimeInc(0.0);
  //pODEBase->EvalAllDiscrete();
  rIC.SetTimeInc(dTMem);*/


  LoadIC(IC_StepStart | IC_StepPreStart, "====");
  // Must Converge States Before Starting
  CODEDataBlock ODB(this);
  ODB.Set(eStateConverge, -1, CODEDataBlock::Converge, pODEBase);

  pODEBase=NULL;
  }


#if dbgODESolve
#define DUMPINT(s)                                           \
  if (dbgDumpDerivs())                                       \
    DumpIntegrators(hDumpFile, rIC.m_Time, rIC.m_TimeInc, s);
#else
#define DUMPINT(s)
#endif


int CODESolver::IntOneStep()
  {
  //m_SettleTime=SettleTime;

//  pIntegrator I;
  double Half_dT=dTimeWrk*0.5;
  double h6=dTimeWrk/6.0;
//  CIntegrateIter It(CStateSave::IList);

  LoadIC(0, "_");
  CODEDataBlock ODB(this);

  switch (m_iMethod)
    {
    case ODE_Euler :
      //m_BadLim.Len=0;
      ODB.Set(eStateAdvance, dTimeWrk, CODEDataBlock::EulerAdvance, pODEBase);

      rIC.AdvanceTime(dTimeWrk);

      ODB.Set(eStateFixDV, dTimeWrk, CODEDataBlock::EulerFixDV, pODEBase);
      break;
    case ODE_RK2 :
      //m_BadLim.Len=0;
      ODB.Set(eStateAdvance, dTimeWrk, CODEDataBlock::RK2AdvanceA, pODEBase);

      rIC.AdvanceTime(dTimeWrk);
      if (fStepTooLarge)
        break;

      ODB.Set(eStateFixDV, dTimeWrk, CODEDataBlock::RK2FixDVA, pODEBase);

      LoadIC(0, "+H");
      pODEBase->ODEDerivs();
      if (fStepTooLarge)
        break;
      //m_BadLim.Len=0;
      ODB.Set(eStateAdvance, dTimeWrk, CODEDataBlock::RK2AdvanceB, pODEBase);

      if (fStepTooLarge)
        break;
      ODB.Set(eStateFixDV, dTimeWrk, CODEDataBlock::RK2FixDVB, pODEBase);

      break;
    case ODE_RK4 :
      ODB.Set(eStateAdvance, dTimeWrk, CODEDataBlock::RK4AdvanceA, pODEBase);

      rIC.AdvanceTime(Half_dT);
      if (fStepTooLarge)
        break;
      //m_BadLim.Len=0;
      ODB.Set(eStateFixDV, dTimeWrk, CODEDataBlock::RK4FixDVA, pODEBase);

      LoadIC(0, "+H/2");
      pODEBase->ODEDerivs();
      if (fStepTooLarge)
        break;
      ODB.Set(eStateAdvance, dTimeWrk, CODEDataBlock::RK4AdvanceB, pODEBase);

      if (fStepTooLarge)
        break;
      //m_BadLim.Len=0;
      ODB.Set(eStateFixDV, dTimeWrk, CODEDataBlock::RK4FixDVB, pODEBase);

      LoadIC(0,"+H/2");
      pODEBase->ODEDerivs();
      if (fStepTooLarge)
        break;
      ODB.Set(eStateAdvance, dTimeWrk, CODEDataBlock::RK4AdvanceC, pODEBase);

      rIC.AdvanceTime(Half_dT);
      if (fStepTooLarge)
        break;
      //m_BadLim.Len=0;
      ODB.Set(eStateFixDV, dTimeWrk, CODEDataBlock::RK4FixDVC, pODEBase);

      LoadIC(0, "+H");
      pODEBase->ODEDerivs();
      if (fStepTooLarge)
        break;
      ODB.Set(eStateAdvance, dTimeWrk, CODEDataBlock::RK4AdvanceD, pODEBase);

      if (fStepTooLarge)
        break;
      //m_BadLim.Len=0;
      ODB.Set(eStateFixDV, dTimeWrk, CODEDataBlock::RK4FixDVD, pODEBase);

      //m_BadLim.Len=0;
      ODB.Set(eStateAdvance, dTimeWrk, CODEDataBlock::RK4AdvanceE, pODEBase);

      break;
    }
  return 0;
  };

//---------------------------------------------------------------------------

void CODESolver::Integrate(ODESolverBase *ODEBase, double Stop_Time, double Max_dTime, flag OneIteration, flag HoldAdvance)
  {
  pODEBase=ODEBase;
  Time_Stop=Stop_Time;
  if (m_iMethod != ODE_RK4)
    m_iStepSizeControl=ODE_SSC_Fixed;

  rIC.m_TimeIncMx=Min(rIC.m_TimeIncMxRqd, Max_dTime);
  rIC.m_TimeIncMn=rIC.m_TimeIncMnRqd;

  dEstMaxDT=0.0;
  fStepTooLarge=False;
  fStepSizeTooSmall=0;
  //nBadVarRange=0;

  rIC.m_TimeIncMx=Max(1.0e-5, rIC.m_TimeIncMx);
  rIC.m_TimeIncMn=Max(1.0e-6, rIC.m_TimeIncMn);

  if (m_iStepSizeControl==ODE_SSC_Fixed)
    rIC.m_TimeIncNext=rIC.m_TimeIncMx;

  flag ShortStep=0;
  rIC.m_TimeIncNext=Range(rIC.m_TimeIncMn, Valid(rIC.m_TimeIncNext) ? rIC.m_TimeIncNext : rIC.m_TimeInc, rIC.m_TimeIncMx);
  while (rIC.m_Time < Time_Stop && !fStepSizeTooSmall)
    {
    double dT_Reqd;

    rIC.m_TimeInc=rIC.m_TimeIncNext;
    rIC.m_TimeIncInit=rIC.m_TimeInc;
    dT_Reqd = Min(Max_dTime, Min(rIC.m_TimeInc, (Time_Stop-rIC.m_Time)));
    ShortStep = (dT_Reqd < rIC.m_TimeInc * 0.99);
    rIC.m_TimeInc = dT_Reqd;

    LoadIC(IC_StepStart | IC_StepPreStart, "====");

    // Clear the last Iterations Errors
    m_BadTolList.Len=0;
    m_BadLim.Len=0;

    // Must Converge States Before Starting
    CODEDataBlock ODB(this);
    ODB.Set(eStateConverge, -1, CODEDataBlock::Converge, pODEBase);

    pODEBase->ODEStartStep();

    LoadIC(IC_StepStart , "====");
    pODEBase->ODEDerivs();
    SaveStart();
    #if dbgODESolve
    if (dbgDumpDerivs())
      DumpIntegrators(hDumpFile, rIC.m_Time, rIC.m_TimeInc, "Strt");
    #endif
    SetLimits();

    int IntLoop=0;
    m_BadTolList.Len=0;
    m_BadTolCopy.Len=0;

    for (flag OK=0; (!OK ) ; )
      {
      m_BadLim.Len=0;
      IntLoop++;
      if (IntLoop > 1)
        {
        LoadIC(IC_StepReStart ,"=..=");
        pODEBase->ODEStartStep();
        pODEBase->ODEDerivs();
        }

      flag IsBad=0;
      OK=1;
      dTimeWrk = rIC.m_TimeInc;
      IntOneStep();
      if (fStepTooLarge)
        {
        IsBad=True;
        }
      else
        switch (m_iStepSizeControl)
          {
          case ODE_SSC_Var_1:
            SaveIntermediate();
            ReStart();
            dTimeWrk *= 0.5;
            LoadIC(0 , "  ==");
            // Derivatives @ Start still the Same
            IntOneStep();
            if (!fStepTooLarge)
              {
              LoadIC(0 , "  --");
              pODEBase->ODEDerivs();
              IntOneStep();
              dTimeWrk *= 2.0;
              CalculateErrors();
              IsBad = !StepSizeOK_1();
              }
            break;

          case ODE_SSC_Var_2 :
            CalculateErrors();
            IsBad=!StepSizeOK_2();
            break;

          default:
            IsBad=0;
            break;
          }

      // FindOut what action to take
      // If bad then restart
      if (IsBad)
        {
        if (fStepTooLarge)
          {
          rIC.m_TimeInc=dEstMaxDT;
          ReStart();
          OK=0;
          m_nBadIters++;
          fStepTooLarge=False;
          }
        else if (fStepSizeTooSmall)
          {
          rIC.m_TimeIncNext=rIC.m_TimeIncMn;
          m_nGoodIters++;
          m_nIters4Step++;
          }
        else //if (!fStepSizeTooSmall)
          {
          rIC.m_TimeInc=rIC.m_TimeIncRestart;
          ReStart();
          OK=0;
          m_nBadIters++;
          m_nIters4Step++;
          }
        }
      else
        {
        m_nGoodIters++;
        m_nIters4Step++;
        }
      }

    // Must be called independently
    // EvalAllDiscrete();
    #if dbgODESolve
    if (dbgDumpDerivs())
      DumpIntegrators(hDumpFile, rIC.m_Time, rIC.m_TimeInc, "Done");
    #endif

    if (ShortStep && !fStepSizeTooSmall)
      {
      if (rIC.m_TimeIncNext >= rIC.m_TimeInc)
        // Test Passed Easily but step was artificially short - use prev dT
        rIC.m_TimeIncNext=rIC.m_TimeIncInit;
      break;
      }
    
    rIC.m_TimeIncNext=Min(Max_dTime, rIC.m_TimeIncNext);
    rIC.m_TimeIncNext=Min(Max_dTime, rIC.m_TimeIncNext);

    if (OneIteration)
      break;
    }

  pODEBase=NULL;
  };

//---------------------------------------------------------------------------

void CODESolver::LoadIC(word Flags_, pchar Desc_)
  {
  rIC.m_Flags=Flags_;
  rIC.m_Desc=Desc_;
  };

//---------------------------------------------------------------------------

void CODESolver::SaveEvalProducts(ODESolverBase *ODEBase)
  {
  pODEBase=ODEBase;
  CODEDataBlock ODB(this);
  ODB.Set(eStateSave, -1, CODEDataBlock::SaveEP, pODEBase);
  };

//---------------------------------------------------------------------------

void CODESolver::RestoreEvalProducts(ODESolverBase *ODEBase)
  {
  pODEBase=ODEBase;
  CODEDataBlock ODB(this);
  ODB.Set(eStateLoad, -1, CODEDataBlock::LoadEP, pODEBase);
  };

//---------------------------------------------------------------------------

void CODESolver::DumpIntegrators(FILE * f, double Time_, double dTime_, pchar Desc)
  {
//  static pchar Method_Names[3] = {"Eul","RK2","RK4"};
//  flag WithIds=(strcmpi(Desc,"strt")==0);
//  if (WithIds)
//    {
//    CIntegrateIter It(CStateSave::IList);
//    for (pIntegrator I=It.First(); (I) ; I=It.Next())
//      I->DbgOn=fabs(I->V)>1.0e-30 || fabs(I->dV)>1.0e-30;
//    }
//
//  if (WithIds)
//    {
//    dbgp("    :Meth ,    m_Time,  m_TimeInc");
//    CIntegrateIter It(CStateSave::IList);
//    for (pIntegrator I=It.First(); (I) ; I=It.Next())
//      if (I->DbgOn)
//        dbgp("%25.25s|",I->sTag());
//    dbgpln("");
//    }
//  dbgp("%-4.4s:%s_%i,%8.2f,%7.3f",Desc, Method_Names[m_iMethod], m_iStepSizeControl, rIC.m_Time, rIC.m_TimeInc);
//  CIntegrateIter It(CStateSave::IList);
//  for (pIntegrator I=It.First(); (I) ; I=It.Next())
//    if (I->DbgOn)
//      dbgp("%14.6g %14.6g|",I->V,I->dV);
//  dbgpln("");
  };

//---------------------------------------------------------------------------

int CODESolver::BadTolInfo(int i, Strng &Tag, double &Tol)
  {
  pODEBase=NULL;
//  if (m_BadLim.List[i] && i<m_BadLim.Len)
//    {
//    Tag=m_BadLim.List[i]->sTag;
//    Tol=m_BadLim.List[i]->V;
//    return m_BadLim.List[i]->fAtLoLim ? 3 : 4;
//    }
//  i=i-m_BadLim.Len;
//  if (m_BadTolCopy.List[i] && i<m_BadTolCopy.Len)
//    {
//    Tag=m_BadTolCopy.List[i]->sTag;
//    Tol=m_BadTolCopy.List[i]->dFracErr;
//    return 1;
//    }
//  i=i-m_BadTolCopy.Len;
//  if (BadTolList.List[i] && i<BadTolList.Len)
//    {
//    Tag=BadTolList.List[i]->sTag;
//    Tol=BadTolList.List[i]->dFracErr;
//    return 2;
//    }
  Tag="";
  Tol=0.0;
  return 0;
  }

//---------------------------------------------------------------------------

void CODESolver::SetStepTooLarge(double EstMaxDT, char *Tag)
  {
  if (!fStepTooLarge || (EstMaxDT < dEstMaxDT))
    {
    dEstMaxDT=EstMaxDT;
    fStepTooLarge=True;
    sTooLargeTag=Tag;
    }
  };

//===========================================================================
//
//
//
//===========================================================================

CODESolver * g_pODE=NULL;
CTimeStateBlock * g_pPBIntCB=NULL;
CTimeStateBlock * g_pDynIntCB=NULL;

CODESolver & ODE() { return *g_pODE; }
CTimeStateBlock & IntCB() { return XDoingPB() ? *g_pPBIntCB : *g_pDynIntCB; }; 


//===========================================================================
//
//
//
//===========================================================================

