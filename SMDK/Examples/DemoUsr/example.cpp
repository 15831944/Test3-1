//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __EXAMPLE_CPP
#include "example.h"
//#include "md_headers.h"

//====================================================================================

IMPLEMENT_MODEL_METHOD(Example, "Example1", "SMDK_Ex1", "Data", MG_NODE)

Example::Example()
  {
  //default values...
  dFeedQm = 0.0;
  dRqdTemp = 30.0+273.16;
  dFeedT = 273.16;
  dDoubleParm1 = 1.1;
  dDoubleParm2 = 2.2;
  dDoubleParm3 = 3.3;
  dDoubleParm4 = 4.4;
  dDoubleParm4 = 5.5;
  iLongParm1 = 0;
  iShortParm1 = 0;
  bBoolParm1 = 0;

  dMixT   = 0.0;
  dMixP   = 0.0;
  dMixQm  = 0.0;
  dMixQv  = 0.0;
  dMixRho = 0.0;
  dMixCp  = 0.0;
  }

//---------------------------------------------------------------------------

void Example::Init()
  {
  MInOutDef OutputDef;
  MethodDef.GetOutputType(0, OutputDef);
  OutputDef.bSizeDataRqd = 1;
  MethodDef.SetOutputType(0, OutputDef);
  }

//---------------------------------------------------------------------------

void Example::BuildDataFields()
  {
  DD.Text("Example Fields...");
  DD.Dble("RqdT",   &dRqdTemp,     MC_T,    MF_PARAMETER);
  DD.Dble("FeedT",  &dFeedT,       MC_T,    MF_RESULT);
  DD.Dble("FeedQm", &dFeedQm,      MC_Qm,   MF_RESULT|MF_NO_FILING);
  DD.Dble("DParm1", &dDoubleParm1, MC_None, MF_PARAMETER);
  DD.Dble("DParm2", &dDoubleParm2, MC_None, MF_PARAMETER|MF_INIT_HIDDEN);
  DD.Long("LParm1", &iLongParm1,            MF_PARAM_STOPPED);
  DD.Shrt("SParm1", &iShortParm1,           MF_PARAMETER);
  DD.Bool("BParm1", &bBoolParm1,            MF_PARAMETER);
  DD.Text("");
  DD.Text("Some more...");
  DD.Dble("DParm3", &dDoubleParm3, MC_None, MF_RESULT|MF_NO_FILING);
  DD.Dble("DParm4", &dDoubleParm4, MC_None, MF_RESULT);
  DD.Dble("DParm5", &dDoubleParm5, MC_None, MF_RESULT);
  DD.Text("");
  DD.Text("Input Mixture:");
  DD.Dble("T",      &dMixT,        MC_T,    MF_RESULT|MF_NO_FILING);
  DD.Dble("P",      &dMixP,        MC_P,    MF_RESULT|MF_NO_FILING);
  DD.Dble("Qm",     &dMixQm,       MC_Qm,   MF_RESULT|MF_NO_FILING);
  DD.Dble("Qv",     &dMixQv,       MC_Qv,   MF_RESULT|MF_NO_FILING);
  DD.Dble("Density",&dMixRho,      MC_Rho,  MF_RESULT|MF_NO_FILING);
  DD.Dble("Cp",     &dMixCp,       MC_CpMs, MF_RESULT|MF_NO_FILING);
  }

//---------------------------------------------------------------------------

void Example::EvalProducts()
  {
  MStream & QI = *StreamIn[0];
  MStream & QO = *StreamOut[0];

  QO = QI; //set output=input
  dFeedQm = QI.MassFlow;
  dFeedT = QI.T;
  CString S;
  S = gs_VectorDef.SpecieTag(0); //get tag of first specie
  dDoubleParm3 = QI.M[0]; //get mass flow of first specie
  QO.T = dRqdTemp;
  if (QI.SieveDef)
    {
    SetCondition(0, "N\tSize data present");
    ClearCondition(0);
    dDoubleParm4 = QI.Size[0][2];
    dDoubleParm5 = QI.SieveDef->Size(2);
    }
  else
    {
    SetCondition(0, "W\tSize data NOT present");
    //SetCondition(1, "N\tSome message");
    //SetCondition(1, "E\tSome message");
    dDoubleParm4 = -1.0;
    dDoubleParm5 = -1.0;
    }

  MStream Q;
  //= InputMixture();
  AddMixtureInId(Q);

  dMixT  = Q.T;
  dMixP  = Q.P;
//  dMixQm = Q.Qm();//dQm;
//  dMixQv = Q.dQv;
//  dMixRho= Q.dDensity;
//  dMixCp = Q.dCp;
  }

//====================================================================================
