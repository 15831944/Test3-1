//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#define  __MATHLIBTO_CPP
#include "sc_defs.h"
#include "tagobj.h"
#include "errorlog.h"

// ===========================================================================
//
//
//
// ===========================================================================

void DualDbl::BuildDataDefn(DataDefnBlk & DDB, pchar Nm, pchar Sym, CCnvIndex Cnv, pchar pCnv_, long lHandle, pTaggedObject pThis, 
                            char * ValStr, char * OvrStr, DDEF_Flags ValFlags, DDEF_Flags OvrFlags, bool ForceOldMethod)
  {
  if (PrjFileVerNo()<53 ||
      !DDB.ForFileSnpScn() ||
      (ForceOldMethod && DDB.ForFileSnpScn() && DDB.DoingPutData()))
    {
    DDB.Double(Nm, Sym, Cnv, pCnv_, lHandle, pThis, isParm|NAN_OK);
    DDB.TagComment(Valid(dOvr) ? OvrStr : ValStr);
    }
  if (!DDB.ForView() && !ForceOldMethod)
    {
    LPSTR Ext[2]    = {".Val", ".Ovr"};
    DDEF_Flags F[2] = { /*isParm|*/DDEF_DUALVALUE|InitHidden|ValFlags, isParm|DDEF_DUALVALUEOVR|InitHidden|NAN_OK|OvrFlags};
    LPSTR Com[2]    = { ValStr, OvrStr};
    for (int i=0; i<2; i++)
      {
      SCDFullTag Nmx, Symx;
      strcpy(Nmx, Nm);
      strcpy(Symx, Sym);
      if (strlen(Nm))
        strcat(Nmx, Ext[i]);
      if (strlen(Sym))
        strcat(Symx, Ext[i]);
      DDB.Double(Nmx, Symx, Cnv, pCnv_, lHandle, pThis, F[i]);
      DDB.TagComment(Com[i]);
      }
    }
  }

//---------------------------------------------------------------------------

flag DualDbl::DataXchg(DataChangeBlk & DCB, long lHandle, pTaggedObject pThis)
  {
  if (lHandle==DCB.lHandle)
    {
    if ((DCB.pDataInfo->iFlags&(DDEF_DUALVALUE|DDEF_DUALVALUEOVR))==0)
//    if (PrjFileVerNo()<52)
      {
      if (DCB.rD)
        {
        if (Valid(dOvr)!=Valid(*(DCB.rD)))
          pThis->StructureChanged(pThis);
        dOvr=*(DCB.rD);
        }
      if (DCB.ForFileSnpScn())
        DCB.D = dOvr;
      else
        DCB.D = (*this)();
      return 1;
      }
    else if (DCB.pDataInfo->iFlags&DDEF_DUALVALUEOVR)
      {
      if (DCB.rD)
        {
        if (Valid(dOvr)!=Valid(*(DCB.rD)))
          pThis->StructureChanged(pThis);
        dOvr=*DCB.rD;
        }
      DCB.D = dOvr;
      }
    else
      {
      //if (DCB.rD)
      //  dVal=*DCB.rD;
      DCB.D = dVal;
      }
    return 1;
    }
  return 0;
  }

// ===========================================================================
//
//
//
// ===========================================================================

CToleranceBlockArray CToleranceBlock::s_Array;

// ---------------------------------------------------------------------------

CToleranceBlock::CToleranceBlock(DWORD Use, LPCTSTR Name, double Abs, double Rel, long MaxIters, DWORD Flags, CCnvIndex AbsIndex, LPCTSTR AbsTxt)
  {
  m_pParent=NULL;
  m_sName=Name;
  m_dwFlags=Use|Flags;
  m_dMaxIters=MaxIters;
  m_dAbs=Max(3.0e-14, Abs);
  m_dRel=Rel;
  m_dAbsMult=1; 
  m_dRelMult=1;
  m_dErr=0;
  //if (m_dAbs==0.0)
  //  m_dwFlags=m_dwFlags&~TBF_UseAbs;
  //if (m_dRel==0.0)
  //  m_dwFlags=m_dwFlags&~TBF_UseRel;
  //m_dwFlags=m_dwFlags|Flags;
  m_AbsCnv.SetIndex(AbsIndex);
  m_AbsCnv.SetText((LPTSTR)AbsTxt);

  if ((m_dwFlags&TBF_LocalOnly)==0)
    {
    for (int i=0; i<s_Array.GetSize(); i++)
      if (_stricmp(Name, s_Array[i]->GetName())<=0)
        break;
    if (i<s_Array.GetSize())
      s_Array.InsertAt(i, this);
    else
      s_Array.Add(this);
    }

  #ifndef _RELEASE
  if (m_pParent==NULL && ((m_dwFlags&TBF_UseMaxParent) || (m_dwFlags&TBF_UseAbsParent) || (m_dwFlags&TBF_UseRelParent)))
    {
    ASSERT_ALWAYS(FALSE, "ToleranceBlock parent not defined!");
    //future calls to GetMaxIters(), GetAbs(), GetRel(), etc will crash!
    }
  #endif
  }

// ---------------------------------------------------------------------------

CToleranceBlock::CToleranceBlock(DWORD Use, CToleranceBlock * pParent, LPCTSTR Name, double AbsMult, double RelMult, long MaxIters, DWORD Flags)
  {
  m_pParent=pParent;
  m_sName=Name;
  m_dwFlags=Use|Flags;
  m_dMaxIters=MaxIters;
  m_dAbs=3.0e-14; // floating point precision (for double - should be larger for float)
  m_dRel=1.0e-9;
  m_dAbsMult=AbsMult;
  m_dRelMult=RelMult;
  m_dErr=0;
  //if (AbsMult==0.0)
  //  m_dwFlags=m_dwFlags&~TBF_UseAbsParent;
  //if (RelMult==0.0)
  //  m_dwFlags=m_dwFlags&~TBF_UseRelParent;
  //m_dwFlags=m_dwFlags|Flags;

  if ((m_dwFlags&TBF_LocalOnly)==0)
    {
    for (int i=0; i<s_Array.GetSize(); i++)
      if (_stricmp(Name, s_Array[i]->GetName())<=0)
        break;
    if (i<s_Array.GetSize())
      s_Array.InsertAt(i, this);
    else
      s_Array.Add(this);
    }

  #ifndef _RELEASE
  if (m_pParent==NULL && ((m_dwFlags&TBF_UseMaxParent) || (m_dwFlags&TBF_UseAbsParent) || (m_dwFlags&TBF_UseRelParent)))
    {
    ASSERT_ALWAYS(FALSE, "ToleranceBlock parent not defined!");
    //future calls to GetMaxIters(), GetAbs(), GetRel(), etc will crash!
    }
  #endif
  }

// ---------------------------------------------------------------------------

CToleranceBlock::~CToleranceBlock()
  {
  for (int i=0; i<s_Array.GetSize(); i++)
    if (s_Array[i]==this)
      break;
  if (i<s_Array.GetSize())
    s_Array.RemoveAt(i);
  }

void CToleranceBlock::SetMaxIters(double MaxIters) { m_dMaxIters.dOvr=MaxIters; };
void CToleranceBlock::SetAbs(double Abs)      { if (!Valid(Abs) || Abs>0) m_dAbs.dOvr=Abs; };
void CToleranceBlock::SetRel(double Rel)      { if (!Valid(Rel) || Rel>0) m_dRel.dOvr=Rel; };
void CToleranceBlock::SetAbsMult(double Abs)  { if (!Valid(Abs) || Abs>0) m_dAbsMult.dOvr=Abs; };
void CToleranceBlock::SetRelMult(double Rel)  { if (!Valid(Rel) || Rel>0) m_dRelMult.dOvr=Rel; };
void CToleranceBlock::SetFlags(DWORD Flags)   { m_dwFlags=Flags; };

//// ===========================================================================
////
////
////
//// ===========================================================================
//// Brents method of solution for roots of equation
//// see Pg 359..362 of Numerical recipes for C 2nd Edition
//
//===========================================================================
//
//
//
//===========================================================================
// Brents method of solution for roots of equation
// see Pg 359..362 of Numerical recipes for C 2nd Edition

const double EPS=3.0e-14; // floating point precision (for double - should be larger for float)

//MRootFinderBase::MRootFinderBase(char *DescStr, double Tol, int ItMax_)
MRootFinderBase::MRootFinderBase(char *DescStr, CToleranceBlock & Tol): \
  m_Tol(Tol)
  { 
  m_Tol.SetFlags(m_Tol.GetFlags()|TBF_UseMax|/*TBF_UseAbs|*/TBF_UseRel);
  //m_Tol.GetAbsDD().dVal=EPS;
  pDescStr=DescStr;
  //strcpy(Tag, pTag1 ? pTag1 : "");
  //if (pTag2 && strlen(pTag2)>0)
  //  {
  //  strcat(Tag, ".");
  //  strcat(Tag, pTag2);
  //  }
 // tol=Tol; 
 // ItMax=ItMax_; 
  dTarget=0.0;
  dResult=0.0;
  m_FnVal=0.0;
  m_FnErr=0.0;
  bStarted=0; 
  bLimits=0;
  bConverged=0;
  bEstSet=0;
  bLogErrors=1;
  dEst=0.0;
  dEstSlp=0.0;
  iter=-1;
  pMRootFinderBusy=NULL;
  dMax=1.0e30;
  dMin=-1.0e30;
  iError=RF_NotStarted;
  x1=x2=m_Err1=m_Err2=m_Val1=m_Val2=xmin=xmax=0.0;
  };

// --------------------------------------------------------------------------

bool MRootFinderBase::Converged(double X1, double X2, double F1, double F2)
  { 
  return bConverged; 
  }; 

// --------------------------------------------------------------------------

int MRootFinderBase::Start(double X1, double X2, double F1/*=dNAN*/, double F2/*=dNAN*/)
  { 
  iError=RF_OK;
  x1=X1;
  x2=X2;
  m_Val1=F1;
  m_Val2=F2;

  if (pMRootFinderBusy)
    *pMRootFinderBusy=1;

  if (bLimits)
    {
    x1=Max(xmin, x1);
    x2=Min(xmax, x2);
    }

  iter=-1;
  if (bEstSet)
    {
    bEstSet=0;

    if ((bLimits && (dEst<xmin)) || (dEst<X1))
      return RF_EstimateLoLimit;
    if ((bLimits && (dEst>xmax)) || (dEst>X2))
      return RF_EstimateHiLimit;

    iter=-2;//testing estimate
    double fne=Function(dEst);
    double fe=fne-dTarget;
    iter=-1;//testing limits

    // >>>>>>>> kga 14/11/01
    //kga : if estimate is correct, exit imediatly as a solve is not required
    //double tol1=2.0*EPS*fabs(dEst)+0.5*tol;
    //if (fabs(0.5*(dEst-V0))<=tol1)
    //if (fabs(fe)<tol)
    if (fabs(fe)<m_Tol.GetRel()*5.0) //use relaxed tolerance?
      {
      iError=RF_EstimateOK;
      dResult=dEst;
      m_FnVal=fne;
      m_FnErr=fe;
      if (pMRootFinderBusy)
        *pMRootFinderBusy=0;
      return iError;
      }
    // <<<<<<<< kga 14/11/01
    if (dEstSlp==0)
      {
      F1 = (Finite(F1) ? F1 : Function(x1));
      F2 = (Finite(F2) ? F2 : Function(x2));
      if (F2>=F1)
        dEstSlp=1;
      else
        dEstSlp=-1;
      }

    if (fe*dEstSlp < 0.0)
      {
      m_Val2 = (Finite(F2) ? F2 : Function(x2));
      m_Err2 = m_Val2-dTarget;
      if (fe * m_Err2 > 0.0) // Not Bracketed
        {
        iError=RF_BadEstimate;
        m_Val1= (Finite(F1) ? F1 : Function(x1));
        m_Err1= m_Val1-dTarget;
      //return iError;
        }
      else
        {
        x1=dEst;
        m_Val1=fne;
        m_Err1=fe;
        }
      }
    else
      {
      m_Val1 = (Finite(F1) ? F1 : Function(x1));
      m_Err1 = m_Val1-dTarget;
      if (m_Err1 * fe > 0.0) // Not Bracketed
        {
        iError=RF_BadEstimate;
        m_Val2= (Finite(F2) ? F2 : Function(x2));
        m_Err2= m_Val2-dTarget;
        //return iError;
        }
      else
        {
        x2=dEst;
        m_Val2=fne;
        m_Err2=fe;
        }
      }
    if (iError==RF_BadEstimate)
      {
      iError=RF_OK;
      //m_Err1 = (Finite(F1) ? F1 : Function(x1))-dTarget;
      //m_Err2 = (Finite(F2) ? F2 : Function(x2))-dTarget;
      }
    }
  else
    {
    m_Val1 = (Finite(F1) ? F1 : Function(x1));
    m_Err1 = m_Val1-dTarget;
    m_Val2 = (Finite(F2) ? F2 : Function(x2));
    m_Err2 = m_Val2-dTarget;
    }

  bool Ok1=Finite(m_Err1);
  bool Ok2=Finite(m_Err2);

  if (!Ok1 || !Ok2)
    iError=RF_BadStartValues;
  else if (m_Tol.ConvergedVV(m_Val1, m_Val2)) 
    {
    iError=RF_Independant;
    }
  else if (m_Err1 * m_Err2 <= 0.0) // Bracketed
    {
    if (x1 > x2)
      {
      Exchange(x1, x2);
      Exchange(m_Err1, m_Err2);
      }
    bStarted=1;
    }
  else
    {
    // Go To Closest Limit
    dResult=(fabs(m_Err1)<fabs(m_Err2)) ? x1 : x2;
    //Function(dResult);
    m_FnVal=Function(dResult);
    m_FnErr=m_FnVal-dTarget;
    iError = (fabs(m_Err1)<fabs(m_Err2)) ? RF_LoLimit : RF_HiLimit;
    }

  if (pMRootFinderBusy)
    *pMRootFinderBusy=0;

  if (iError!=RF_OK)
    if (0 && bLogErrors)
      LogError(strlen(ObjTag())>0 ? ObjTag() : "MATHLIB", 0, "MRootFinderBase Not Started");

  return iError;
  };

// --------------------------------------------------------------------------

int MRootFinderBase::Solve_Brent()
  { 
  if (!bStarted)
    {
    iError=RF_NotStarted;
    return iError;
    }

  if (pMRootFinderBusy)
    *pMRootFinderBusy=1;

  iError=RF_OK;

  // The Function must be called at the end of the solution 
  // with the best estimate ie it must be called at least once.
  bool FnCalled=0; 
  double a=x1,b=x2,c=x2,d,e,min1,min2;
  double fa=m_Err1, fb=m_Err2;
  double fna=m_Val1, fnb=m_Val2;
  double fc, fnc,p,q,r,s,tol1,xm;
  fnc=fnb;
  fc=fb;
  int dbgiters=(m_Tol.GetMaxIters()*90)/100;
  for (iter=1; iter < m_Tol.GetMaxIters(); iter++)
    {
    if (iter>=dbgiters)
      {
      if (iter==dbgiters)
        dbgpln("Brent Not Converging x1:%g(err1%g) > x2:%g(err2%g)",x1,m_Err1,x2,m_Err2);
      dbgpln("                     x :%.20e(err %.20e)",b,fb);
      }
    
    bool ReDoFn=0;
    if ((fb>0.0 && fc>0.0) || (fb<0.0 && fc<0.0))
      {
      // Rename a,b,c and adjust bounding interval d
      c=a;
      fc=fa;
      fnc=fna;
      e=d=b-a;
      }
    if (fabs(fc)<fabs(fb))
      {
      a=b;
      b=c;
      c=a;
      fa=fb;
      fb=fc;
      fc=fa;
      fna=fnb;
      fnb=fnc;
      fnc=fna;
      ReDoFn=1;
      }
    // Convergence check
    tol1=2.0* /*m_Tol.GetAbs()*/ EPS*fabs(b)+0.5*m_Tol.GetRel();
    xm=0.5*(c-b);
    bConverged=(fabs(xm)<=tol1);
    if (Converged(b, c, fnb, fnc) || fb==0.0) 
    //if (fabs(xm)<=tol1 || fb==0.0) 
      {
      if (ReDoFn || !FnCalled)
        {
        fnb=Function(b);
        fb=fnb-dTarget;
        }
      dResult=b;
      m_FnVal=fnb;
      m_FnErr=fb;
      if (pMRootFinderBusy)
        *pMRootFinderBusy=0;
      return RF_OK;
      }
    if (fabs(e)>=tol1 && fabs(fa)>fabs(fb))
      {
      // Attempt inverse quadratic interpolation
      s=fb/fa;
      if (a==c)
        {
        p=2.0*xm*s;
        q=1.0-s;
        }
      else
        {
        q=fa/fc;
        r=fb/fc;
        p=s*(2.0*xm*q*(q-r)-(b-a)*(r-1.0));
        q=(q-1.0)*(r-1.0)*(s-1.0);
        }
      // check whether in bounds
      if (p>0.0)
        q=-q;
      p=fabs(p);
      min1=3.0*xm*q-fabs(tol1*q);
      min2=fabs(e*q);
      if (2.0*p < (min1 < min2 ? min1 : min2))
        {
        // accept interpolation
        e=d;
        d=p/q;
        }
      else
        {
        //Interpolation failed, use bisection 
        d=xm;
        e=d;
        }
      }
    else
      {
      // Bounds decreasing too slowly, use bisection
      d=xm;
      e=d;
      }
    // Move last best guess to a;
    a=b;
    fa=fb;
    fna=fnb;
    // Evaluate new trial root.
    //if (fGuessSet)
    //  {
    //  b=dGuess;
    //  fb=Function(b)-dTarget;
    //  fGuessSet=0;
    //  }
    //else
    if (fabs(d)>tol1)
      b+=d;
    else
      {
      double dx=Min(fabs(tol1), fabs(0.5*xm));
      b+= (xm > 0.0 ? dx: -dx);
      //b+= (xm > 0.0 ? fabs(tol1) : -fabs(tol1));
      }
    fnb=Function(b);
    fb=fnb-dTarget;
    FnCalled=1;
    }

  if (pMRootFinderBusy)
    *pMRootFinderBusy=0;
  
  if (bLogErrors)
    LogError(strlen(ObjTag())>0 ? ObjTag() : "MATHLIB", 0, "MRootFinderBase::SolveBrent Not Converged");

  iError=RF_NotConverged;
  return iError;
  };

// --------------------------------------------------------------------------

#define NewSolveFromEst 0

int MRootFinderBase::SolveFromEst(double Est, double Sign, double dChgMax, double dChgInit) 
  {
  if (pMRootFinderBusy)
    *pMRootFinderBusy=1;

  iError=RF_OK;
  double PosV=1.0e10, PosF=1.0e10;
  double NegV=-1.0e10, NegF=-1.0e10;
  bool PosOk=0, NegOk=0, DoBrent=0;

  if (!Valid(dChgInit))
    dChgInit=dChgMax;

  iter=-2;//testing estimate
  double EstV=Est;
  double EstF=Function(Est); 

  double V0=EstV;
  double F0=EstF;

  const int MaxIt=100;
  int It=MaxIt;

  if (/*Sign**/(EstF-dTarget)>0.0)
    {
    PosF=EstF;
    PosV=EstV;
    PosOk=1;
    EstV-=Sign*dChgInit;//*0.001; // EstV too High
    #if NewSolveFromEst
    if (V0<=dMax && EstV>=dMax)
      EstV=V0-dChgInit;
    else if (V0>=dMin && EstV<=dMin)
      EstV=V0+dChgInit;
    #endif
    }
  else if (/*Sign**/(EstF-dTarget)<0.0)
    {
    NegF=EstF;
    NegV=EstV;
    NegOk=1;
    EstV+=Sign*dChgInit;//*0.001; // EstV too Low
    #if NewSolveFromEst
    if (V0<=dMax && EstV>=dMax)
      EstV=V0-dChgInit;
    else if (V0>=dMin && EstV<=dMin)
      EstV=V0+dChgInit;
    #endif
    }
  else
    {
    #if NewSolveFromEst
    dResult=EstV;
    m_FnVal=EstF;
    m_FnErr=m_FnVal-dTarget;
    goto Done;
    #endif
    }

  for (; It; It--)
    {
    EstV=Range(dMin, EstV, dMax);
    EstF=Function(EstV); 
    
    double tol1=2.0*EPS*fabs(EstV)+0.5*m_Tol.GetRel();
    bConverged=(fabs(0.5*(EstV-V0))<=tol1);
    // Test for no change in measurement
    if (fabs(EstF-F0)<1.0e-14+1.0e-14*fabs(EstF+F0))
      {
      dResult=EstV;
      m_FnVal=EstF;
      m_FnErr=m_FnVal-dTarget;
      if (!bConverged)
        iError=RF_NotConverged;
      #if NewSolveFromEst
      if (dResult>=dMax)
        iError=RF_HiLimit;
      else if (dResult<=dMin)
        iError=RF_LoLimit;
      else
        iError=RF_NotConverged;
      #endif
      goto Done;
      }

    if (bConverged) // Test for convergence
      {
      dResult=EstV;
      m_FnVal=EstF;
      m_FnErr=m_FnVal-dTarget;
      goto Done;
      }


    if (/*Sign**/(EstF-dTarget)>0.0)
      {
      PosF=Min(PosF, EstF);
      PosV=Min(PosV, EstV);
      PosOk=1;
      }
    else if (/*Sign**/(EstF-dTarget)<0.0)
      {
      NegF=Max(NegF, EstF);
      NegV=Max(NegV, EstV);
      NegOk=1;
      }
    if (PosOk && NegOk)
      goto Brent;

    double V=EstV+(dTarget-EstF)*(V0-EstV)/NZ(F0-EstF);
    V=Range(Max(dMin, EstV-dChgMax), V, Min(dMax, EstV+dChgMax));
    V0=EstV;
    F0=EstF;
    EstV=V;
    }

  // Allows large exploratory Duty without getting error messages
  if (bLogErrors && (It==0))
    LogWarning(ObjTag(), 0, "%s - Not Converged %g != %g", pDescStr, EstF, dTarget);
  goto Done;

Brent:
  // Allow Initialisation of Local Variables
  Start(NegV, PosV, NegF, PosF);
  if (Error()==RF_OK)
    {
    if (Solve_Brent()!=RF_OK)
      {
      if (bLogErrors)
        LogWarning(ObjTag(), 0, "%s - Brent Not Converged %g != %g", pDescStr, EstF, dTarget);
      }
    else
      {
      EstV=Result();
      }
    }
  else
    {
    if (bLogErrors)
      LogWarning(ObjTag(), 0, "%s - Brent Not Started", pDescStr);
    }
  
Done:;
  if (pMRootFinderBusy)
    *pMRootFinderBusy=0;

  return Error();
  };  

void MRootFinderBase::SetErrorHandling(bool LogErrors, bool * MRootFinderBusy)
  {
  bLogErrors=LogErrors;
  pMRootFinderBusy=MRootFinderBusy;
  if (pMRootFinderBusy)
    *pMRootFinderBusy=0;
  }

LPCTSTR MRootFinderBase::ResultString(int ErrorRet)
  {
  const LPCTSTR Strs[] = {
    "OK",
    "LoLimit",
    "HiLimit",
    "BadEstimate",
    "BadStartValues",
    "NotStarted",
    "NotConverged",
    "EstimateOK",
    "EstimateLoLimit",
    "EstimateHiLimit",
    };
  return Strs[ErrorRet];
  };

// ===========================================================================
//
//
//
// ===========================================================================

int MRootFinder::FindRoot(double FnTarget, double XLoBnd, double XHiBnd, double XEst, double XEstSlope)
  {
  SetTarget(FnTarget);
  if (Valid(XEst))
    SetEstimate(XEst, XEstSlope);
  int iRet=Start(XLoBnd, XHiBnd);
  if (iRet==RF_EstimateOK) //estimate is good, solve not required
    return RF_OK;
  if (iRet==RF_BadEstimate)
    iRet = Start(XLoBnd, XHiBnd); // Restart
  if (iRet==RF_OK) 
    return Solve_Brent();
  return iRet;
  }

// ===========================================================================

int MRootFinder::FindRootEst(double FnTarget, double XLoBnd, double XHiBnd, double XEst, double XEstSlope, double ChgMax, double ChgInit)
  {
  SetTarget(FnTarget);
  SetLimits(XLoBnd, XHiBnd);

  if (XEstSlope == 0.0)
    {
    if (Function(XHiBnd)>Function(XLoBnd))
      XEstSlope = 1.0;
    else
      XEstSlope = -1.0;
    }

  if (ChgMax==0.0)
    ChgMax=0.2*(XHiBnd-XLoBnd);

  if (ChgInit==0.0)
    ChgInit=0.01*(XHiBnd-XLoBnd);

  if (SolveFromEst(XEst, XEstSlope, ChgMax, ChgInit)==RF_OK)
    return RF_OK;

  return FindRoot(FnTarget, XLoBnd, XHiBnd, XEst, XEstSlope);
  }

// ===========================================================================
//
//
//
// ===========================================================================
