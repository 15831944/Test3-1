//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992 - 1995

#ifndef __SLVTOOL_H
#define __SLVTOOL_H
//
//#ifndef __SLVTBASE_H
//  #include "slvtbase.h"
//#endif
//#include "syscad.h"
#ifndef __EXECUTIV_H
  #include "executiv.h"
#endif

#define OPT_FILE "Scd_Opt.ini"

////////////////////////////////////////////////////////////////////////////
// CSolveTool dialog

class CSolveTool : public SolveToolBase 
  {
  // Construction
  public:

  
  public:
    // Public Creation
    //CStopWatch      RunSW;
  //
  //    // Optimiser
  //    CDVector       StdD;
  //    CTargetArray   Targets;
  //    CParamArray    Params;
  //
  //    GCInsMngr      Pgm;
  //    //CCMinFinder    MinFinder;
  //    //EqnSlvCtrlBlk  CB;

    CProfINIFile   ProfFn;

    flag           Open();
    flag           Close();

    flag           InitProblem(/*CExecutive & Exec, EqnSlvCtrlBlk & EqnCB*/);
    flag           SolveProblem(/*CExecutive & Exec, EqnSlvCtrlBlk & EqnCB*/);
    flag           StepProblemBegin(/*CExecutive & Exec, EqnSlvCtrlBlk & EqnCB*/);
    flag           StepProblemEnd(/*CExecutive & Exec, EqnSlvCtrlBlk & EqnCB*/);
    flag           TermProblem(/*CExecutive & Exec, EqnSlvCtrlBlk & EqnCB*/);
    flag           BeginPause(/*CExecutive & Exec, EqnSlvCtrlBlk & EqnCB*/);
    flag           EndPause(/*CExecutive & Exec, EqnSlvCtrlBlk & EqnCB*/);
  
  };

/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
extern CSolveTool SolveTool;//() { return *CSolveTool::pSolveTool; };

#endif
