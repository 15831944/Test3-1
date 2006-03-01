//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992,1994,1995

//Used by PMC optimization equations...
#ifdef PMC

#ifndef  __OPTIMIZE_H
#define  __OPTIMIZE_H

#ifdef __OPTIMIZE_CPP
  #define DllImportExport DllExport
#elif !defined(SCEXEC)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif


// ===========================================================================

enum MdlTypes { Linear, Quadratic, Poly, SCurve, CircLoad, PartSize, AveSize };

class CModelDesc
  {
  public:
    pchar     Nm;       //model name
    byte      nXMeas;   //number of measured (x) independent variables in equation
    byte      nParms;   //number of parameters in equation
  };

//----------------------------------------------------------------------------


_FWDDEF(COptimizer)
/*#C:This class is used by GenCon (PGM) for optimizing/estimating (done for PMC). 
See PGM Language help.*/
class DllImportExport COptimizer
  {
  public:
    MdlTypes       Model;       //currently selected model equation
    long           NoSamples;   //number of samples to record
    long           SampleDelay; //number of samples back
    CDVector       xMeas;       //array of current measured 'x' values
    double         yMeas;       //current measured 'y' value
    CDVector       Parm;        //array of parameters for the equation
    CDVector       YCalc;       //history of calculated 'y' values
    CDVector       YMeas;       //history of measured 'y' values
    CDMatrix       XMeas;       //history of measured 'x' values
    CDVector       SumXMeas;    //sum of all the xMeas samples
    double         SumYMeas;    //sum of all the yMeas samples
    double         SumYCalc;    //sum of all the yCalc samples
    long           SampleXPos;  //position of current 'x' value in history
    long           SampleYPos;  //position of current 'y' value in history
    double         MinYValid;   //for range checking yMeas
    double         MaxYValid;   //for range checking yMeas
    long           SampleCnt;   //counter
    long           SumXCnt;     //counter
    long           SumYCnt;     //counter
    long           AdjCnt;      //counter
    long           AdjSO;       //number of samples after which the offset must be adjusted
    double         Scale;       //gain equation is multiplied by
    double         Offset;      //offset added to equation
    double         Output;      //last result from estimate

    flag           UseYMeas;    //True if y measurements are obtained
    byte           nXMeas;      //number of measured (x) independent variables in equation
    byte           nParms;      //number of parameters in equation

    COptimizer();
    flag ChangeModel(pchar EqnName);
    double Estimate();
    void SetXMeas(double meas, int i)
      { xMeas[i] = meas; };
    void SetYMeas(double meas)
      { yMeas = meas; };
  protected:
    void Initialise();
    double Equation(CDVector &XDat);

    static int MdlCnt;
    static CModelDesc Eqns[];
  };

//----------------------------------------------------------------------------

#undef DllImportExport 

#endif

#endif //PMC
