//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992,1994,1995

//Used by PMC optimization equations...

#include "stdafx.h"
#include "sc_defs.h"
#include "vectors.h"

#define __OPTIMIZE_CPP

#include "optimize.h"

//===========================================================================
#ifdef PMC

int COptimizer::MdlCnt = 7;
CModelDesc COptimizer::Eqns[] = 
  { {"Linear", 1, 1},
    {"Quadratic", 1, 2},
    {"Poly", 1, 1},
    {"Scurve", 1, 2},
    {"CircLoad", 3, 2},
    {"PartSize", 3, 7},
    {"AveSize", 6, 0} };

// -------------------------------------------------------------------------

COptimizer::COptimizer()
  {
  Initialise();
  }

// -------------------------------------------------------------------------

void COptimizer::Initialise()
  {
  NoSamples = 10;
  SampleDelay = 0;
  Scale = 1.0;
  Offset = 0.0;
  MinYValid = 0.0;
  MaxYValid = 100.0;
  AdjSO = 0;
  UseYMeas = 0;
  Output = 0.0;
  ChangeModel("Linear");
  }

// -------------------------------------------------------------------------

flag COptimizer::ChangeModel(pchar EqnName)
  {
  for (int i=0; i<MdlCnt; i++)
    if (stricmp(EqnName, Eqns[i].Nm)==0)
      break;
  if (i>=MdlCnt)
    return False;
  Model = (MdlTypes)i;
  NoSamples = Max(NoSamples, 1L);
  nXMeas = Eqns[Model].nXMeas;
  nParms = Eqns[Model].nParms;
  xMeas.SetSize(nXMeas);
  Parm.SetSize(nParms);
  XMeas.SetSize(NoSamples, nXMeas);
  YMeas.SetSize(NoSamples);
  YCalc.SetSize(NoSamples);
  //SumXMeas.SetSize(nXMeas);
  SampleYPos = 0;
  SampleXPos = 0;
  SampleCnt = 0;
  SumXCnt = 0;
  SumYCnt = 0;
  AdjCnt = 0;
  return True;
  }

// -------------------------------------------------------------------------

double COptimizer::Equation(CDVector &XDat)
  {
  int i;
  double function = 0.0;
  switch (Model)
    {
    case Linear:
      for (i=0; i<nParms; i++)
        function += (Parm[i]*XDat[i]);
      break;
    case Quadratic:
      function = (Parm[0]*XDat[0]*XDat[0]) + Parm[1];
      break;
    case  Poly:
      {
      function = Parm[0];
      double Coeff = 1.0;
      for (i=1; i<nParms; i++)
        {
        function += (Parm[i]*Coeff*XDat[0]);
        Coeff *= XDat[0];
        }
      break;
      }
    case  SCurve:
      if (fabs(Parm[0]) > 1e-6)
        function = (exp(Parm[1]*XDat[0]/Parm[0]) + 1)/(exp(Parm[1]*XDat[0]/Parm[0]) + exp(Parm[1]) + 2);
      else 
        function = 2 / (3+exp(Parm[1]));
      break;
    case CircLoad:
      {
      double LRho1 = 5;        // Magnitite
      double LRho2 = 2.5;      // ore
      double LRho3 = 1.0;      // water
      double step2 = 0.0;
      double step1 = ((XDat[1]/100)*LRho1) + (1-(XDat[1]/100)) * LRho2;
      if (step1 > 0)
        step2 = (LRho3-(XDat[2]/LRho3))/((XDat[2]/step1)-(XDat[2]/LRho3));
      function = (Parm[0]*XDat[0]*XDat[2]*step2) + (Parm[1]*XDat[0]*XDat[2]*(1.0-step2));
      }
      break;
    case PartSize:
      {
      /*function= (0.0-(Parm[0]*pow(XDat[0],Parm[1]) + 
                 Parm[2]*pow(XDat[1],Parm[3]) -
                 Parm[4]*pow(XDat[2],Parm[5]))) * Parm[6];
      function = (Parm[6] * (Parm[0]*pow(XDat[0],Parm[1]) + Parm[2]*pow(XDat[1],Parm[3]) -
                           Parm[4]*pow(XDat[2],Parm[5]))); */

      double a = Parm[4]-(XDat[2]/2.5);
      double d = Parm[4];
      a /= d;
      if (XDat[0]<1.0e-6)
        XDat[0] = 1.0e-6;
      if (XDat[1]<1.0e-6)
        XDat[1] = 1.0e-6;
      double b = Parm[0]*pow(XDat[0],Parm[1]);
      double c = Parm[2] * pow(XDat[1],Parm[3]);
      function = Parm[6] * (b + c) * pow(a,Parm[5]);
      //function = (Parm[6] * (Parm[0]*pow(XDat[0],Parm[1]) + Parm[2]*pow(XDat[1],Parm[3]))*
      //                     pow(((Parm[4]-(XDat[2]/2.5))/Parm[4]),Parm[5]));
      }
      break;
    case AveSize:
      {
      double A = (XDat[0]*XDat[2]);
      double B = (XDat[1]*XDat[3]);
	    double Tote = A + B;
	    if (Tote <= 0.0) 
	      Tote = 0.001;
      double Mass1 = A / Tote;
	    double Mass2 = B / Tote;
      function = ((XDat[4]*Mass1) + (XDat[5]*Mass2)) / (Mass1+Mass2);
      break;
      }
    default:
      ASSERT(FALSE); //unsuported equation type
      break;
    }
  return function;
  }

// -------------------------------------------------------------------------

double COptimizer::Estimate()
  {
  NoSamples = Max((long)1,NoSamples);
  SampleDelay = Max(0L, Min(SampleDelay, (long)(NoSamples-1)));
  XMeas.SetSize(NoSamples, nXMeas);
  YMeas.SetSize(NoSamples);
  YCalc.SetSize(NoSamples);

  flag SampleOK = (UseYMeas) ? (yMeas>=MinYValid && yMeas<=MaxYValid) : True;
  if (!SampleOK || SampleCnt==0)
    {
    SampleCnt=0;
    SampleXPos = 0;
    for (int j=0; j<NoSamples; j++)
      {
      for (int i=0; i<nXMeas; i++)
        XMeas[j][i] = 0.0;
      YMeas[j] = 0.0;
      YCalc[j] = 0.0;
      }
    /*SumXCnt=0;
    for(int i=0; i<nXMeas ; i++)
      SumXMeas[i] = 0.0;*/
    SumYCnt = 0;
    SumYMeas = 0.0;
    SumYCalc = 0.0;
    AdjCnt = 0;
    }

  SampleYPos = SampleXPos - SampleDelay;
  if ((SampleYPos < 0) && (SampleYPos+NoSamples < SampleCnt))
    SampleYPos += NoSamples;
  SampleYPos = SampleYPos%NoSamples; //prevent reduction in NoSamples causing crash
  SampleXPos = SampleXPos%NoSamples;

  SampleCnt = Min((long)(SampleCnt+1), NoSamples);

  for (int i=0; i<nXMeas; i++)
    {
    //SumXMeas[i] -= XMeas[SampleXPos][i];
    XMeas[SampleXPos][i] = xMeas[i];
    //SumXMeas[i] += XMeas[SampleXPos][i];
    }
  /*if (SampleXPos == (NoSamples-1))
    {//fix rounding errors...
    for (int i=0; i<nXMeas; i++)
      SumXMeas[i] = 0.0;
    for (int j=0; j<NoSamples; j++)
      for (int i=0; i<nXMeas ; i++)
        SumXMeas[i] += XMeas[j][i];
    }
  SumXCnt = Min((long)(SumXCnt+1), NoSamples);*/
  
  /*CDVector XMean;
  XMean.SetSize(nXMeas);
  for (i=0; i<nXMeas ; i++)
    XMean[i]=SumXMeas[i]/SumXCnt;*/

  if (SampleOK && SampleYPos >= 0)
    {
    if (UseYMeas)
      {
      SumYMeas -= YMeas[SampleYPos];
      SumYCalc -= YCalc[SampleYPos];
      YMeas[SampleYPos] = yMeas;
      }
    }
      
  long DelayIndex = Max((long)0, SampleYPos);
  CDVector XData;
  XMeas.GetRow(DelayIndex, XData);
  Output = Equation(XData);

  if (SampleOK)
    {
    if (SampleYPos >= 0)
      {
      YCalc[SampleYPos] = Output;
      if (UseYMeas)
        {
        SumYMeas += YMeas[SampleYPos];
        SumYCalc += YCalc[SampleYPos];
        SumYCnt = Min((long)(SumYCnt+1), NoSamples);
        if (SampleYPos==0)
          {//fix rounding errors...
          SumYMeas = 0.0;
          SumYCalc = 0.0;
          for (int i=0; i<SumYCnt; i++)
            {
            SumYMeas += YMeas[i];
            SumYCalc += YCalc[i];
            }
          }

        if (AdjSO>0)
          if (++AdjCnt>=AdjSO)
            {
            Offset = (SumYMeas-SumYCalc) / SumYCnt;
            AdjCnt = 0;
            }
        }
      }
    SampleYPos = (SampleYPos+1)%NoSamples;
    SampleXPos = (SampleXPos+1)%NoSamples;
    }

  Output = Scale * Output + Offset;

  return Output;
  }

// -------------------------------------------------------------------------
#endif //PMC

