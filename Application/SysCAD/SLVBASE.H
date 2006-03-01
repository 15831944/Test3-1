//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992 - 1995

#ifndef __SLVBASE_H
#define __SLVBASE_H
//
#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __SCEXEC_H
  #include "scexec.h"
#endif
//#ifndef __EXECUTIV_H
//  #include "executiv.h"
//#endif
#ifndef __EQNSOLVE__H
  #include "eqnsolve.h"
#endif
#ifndef __PGM_E__H
  #include "pgm_e.h"
#endif
#ifndef __AFXTEMPL_H__
  #include <afxtempl.h> // "xafxtempl.h"
#endif
#ifndef __SCDTEMPL_H
  #include "scdtempl.h"
#endif
#ifndef __CMINIMUM_H
  #include "cminimum.h"
#endif

//===========================================================================
//
//
//
//===========================================================================

class CSDExp
  {
  public:
    Strng          sPGM;
    Strng          sExp;
    Strng          sCurVal;
    pGCVar         pVar;

    double operator()() { return pVar->getD() ;};
  //   double &operator=(double D) { pVar->set(D); /*double d=pVar->getD();*/ return D; };
    double operator=(double D) { pVar->set(D); /*double d=pVar->getD();*/ return D; };

    CSDExp()
      {
      //dVal=1.0; 
      pVar=NULL;
      };
  };

//===========================================================================
//
//
//
//===========================================================================

class CTarget
  {
  public:
    CSDExp         Exp;
    CSDExp         Val;
    //CSDExp         CurVal;
    //Strng          sPGM[2]; // Value & Target
    //Strng          sExp[2];
    //double         dVal[2]; 
    //pGCVar         pVar[2];

    CTarget() {};// dVal[0]=dVal[1]=0.0; };
  };

typedef CArray<CTarget, CTarget&> CTargetArray;

//===========================================================================
//
//
//
//===========================================================================

class CParam
  {
  public:
    CSDExp         Exp;
    CSDExp         Min;
    CSDExp         Max;
    //CSDExp         CurVal;
    //Strng          sPGM[3]; // Value & Min & Max
    //Strng          sExp[3];
    //double         dVal[3]; 
    //pGCVar         pVar[3];

    CParam() {};// dVal[0]=dVal[1]=dVal[2]=0.0; };
  };
typedef CArray<CParam, CParam&> CParamArray;

//===========================================================================
//
//
//
//===========================================================================

typedef CSCDList <CXM_Route *, CXM_Route *> CXM_RouteList;
typedef CSCDPtrListIter <CXM_RouteList, CXM_Route *> CXM_RouteIter;

typedef CArray <CXM_Route, CXM_Route&> CXM_RouteArray;

//===========================================================================
//
//
//
//===========================================================================

class SolveToolCB
  {
  public:
    CStopWatch      RunSW;

    // Optimiser
    CDVector       StdD;
    CTargetArray   Targets;
    CParamArray    Params;

    GCInsMngr      Pgm;
    //CCMinFinder    MinFinder;
    //EqnSlvCtrlBlk  CB;
  };

//===========================================================================
//
//
//
//===========================================================================

#endif
