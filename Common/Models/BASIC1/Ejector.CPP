//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdio.h>

#include "sc_defs.h"
#define  __EJECTOR_CPP
#include "ejector.h"
#include "models.h"
#include "flweqn.h"
//#include "optoff.h"

//==========================================================================
//
//
//
//==========================================================================

const byte IOId_InPres = FlwIOId(0);
const byte IOId_InSuct = FlwIOId(1);
const byte IOId_Output = FlwIOId(2);

static IOAreaRec X_EjectorIOAreaList[] =
  {{"InPres",      "InPres"   , IOId_InPres, LIO_In,  nc_MLnk, 1, 1, /*IOPipeJoin|*/IOGRP(1)},
   {"InSuct",      "InSuct"   , IOId_InSuct, LIO_In0,  nc_MLnk, 1, 1, /*IOPipeJoin|*/IOGRP(1)},
   {"Output",      "Output"   , IOId_Output, LIO_Out0, nc_MLnk, 1, 1, /*IOPipeJoin|*/IOGRP(1)},
   {NULL}};

static double Drw_X_Ejector[] = { //DD_Scale, 2000,
                         DD_Poly, -4,2, 0,2, 0,-2, -4,-2, -4,2,
                         DD_Poly, 0,1, 6,2, 6,-2, 0,-1,
                         DD_End };

//--------------------------------------------------------------------------

IMPLEMENT_MODELUNIT(Ejector, "Ejector", "1", Drw_X_Ejector, "MixerSplitter", "EJ", TOC_DYNAMICFULL|TOC_GRP_GENERAL|TOC_STD_KENWALT,
                    "Process:Unit:Ejector(1)",
                    "Ejector")

Ejector::Ejector(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MN_Xfer(pClass_, TagIn, pAttach, eAttach)//,
  {
  AttachIOAreas(X_EjectorIOAreaList);

  Joins.SetSize(1); // PreInit to Allow access to Leak Information


  dPSuct=0.0;
  daKFact=1.0;
  dbKFact=1.0;
  daNzDiam=0.03;
  dbNzDiam=0.05;
  d3NzDiam=0.05;
  dPsr=1;
  dPcr=1;

  dQa=0.0;
  dQb=0.0;

  dPa=Std_P;
  dPb=Std_P;
  dP3=Std_P;

  dAreaRatio=Sqr(d3NzDiam)/Sqr(daNzDiam);
  };

// -------------------------------------------------------------------------

void Ejector::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);

  DDB.Text   ("");
  DDB.Double  ("EntrainRatio",    "Wr",    DC_,     "",      &dWr,      this, isResult|0);
  DDB.Double  ("SuctionRatio",    "Psr",   DC_,     "",      &dPsr,     this, isResult|0);
  DDB.Double  ("CompRatio",       "Pcr",   DC_,     "",      &dPcr,     this, isResult|0);
  DDB.Double  ("MotiveFlow",      "Qa",    DC_Qm,   "kg/s",  &dQa,      this, isResult|0);
  DDB.Double  ("SuctionFlow",     "Qb",    DC_Qm,   "kg/s",  &dQb,      this, isResult|0);
  DDB.Double  ("PGain",           "PSuct", DC_DP,   "kPa",   &dPSuct,   this, isResult|0|InitHidden);
  DDB.Double  ("PMotive",         "Pa",    DC_P,    "kPa",   &dPa,      this, isResult|0);
  DDB.Double  ("PSuction",        "Pb",    DC_P,    "kPa",   &dPb,      this, isResult|0);
  DDB.Double  ("PProduct",        "P3",    DC_P,    "kPa",   &dP3,      this, isResult|0);
  DDB.Text("");

  DDB.Visibility(NM_Dynamic|SM_All|HM_All);
  BuildDataDefnElevation(DDB);
  DDB.Text("");
  DDB.Double  ("MotiveNozzle",    "",      DC_L,    "mm", &daNzDiam,      this, isParm|SM_DynBoth);
  DDB.Double  ("SuctionNozzle",   "",      DC_L,    "mm", &dbNzDiam,      this, isParm|SM_DynBoth);
  DDB.Double  ("DischargeNozzle", "",      DC_L,    "mm", &d3NzDiam,      this, isParm|SM_DynBoth);
  DDB.Double  ("KFactMotive",     "",      DC_,     "%",  &daKFact,       this, isParm|SM_DynBoth);
  DDB.Double  ("KFactSuction",    "",      DC_,     "%",  &dbKFact,       this, isParm|SM_DynBoth);
  DDB.Double  ("AreaRatio",       "",      DC_,     "%",  &dAreaRatio,    this, isResult|SM_DynBoth);

  BuildDataDefnShowIOs(DDB);

  DDB.EndStruct();
  };

// -------------------------------------------------------------------------

flag Ejector::DataXchg(DataChangeBlk & DCB)
  {
  if (MN_Xfer::DataXchg(DCB))
    return 1;

  return 0;
  }

// -------------------------------------------------------------------------

flag Ejector::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=MN_Xfer::ValidateData(VDB);

  daNzDiam=Max(daNzDiam,0.001);
  dbNzDiam=Max(dbNzDiam,0.001);
  d3NzDiam=Max(d3NzDiam,0.001);

  dAreaRatio=Sqr(d3NzDiam)/Sqr(daNzDiam);
  return OK;
  }

//---------------------------------------------------------------------------

void Ejector::ConfigureJoins()
  {
  SetIO_Join(IOId_InPres, 0);
  SetIO_Join(IOId_InSuct, 0);
  SetIO_Join(IOId_Output, 0);

  SetJoin_HasVolume(0, false, true);
  };

//---------------------------------------------------------------------------
// See Spreadsheet Ejector.XLS
//---------------------------------------------------------------------------

double PsrFn(double Wr, double Ar)
  {
  double Z=log(Ar);
  double A=-0.00064*Ar+0.08416;
  double B=0.00098*Ar+0.80955;
  double C=-1.07264*Z+0.43544;
  double Y=log(Wr);
  return exp(A*Y*Y+B*Y+C);
  }

//---------------------------------------------------------------------------

double PcrFn(double Psr, double Ar)
  {
  double Z=log(Ar);
  double A=-0.00151*Ar+0.16223;
  double B=-0.00581*Ar+0.29474;
  double C=-0.53444*Z+1.77853;
  double Y=log(Psr);
  return exp(A*Y*Y+B*Y+C);
  }

//---------------------------------------------------------------------------

const double RngFlwLim = 0.001;

double BstFn(double QMotive, double QSuct, double AreaRatio, double P3)
  {
  QMotive=CFlwBlkBase::RangeFlow(QMotive, RngFlwLim);
  QSuct=CFlwBlkBase::RangeFlow(QSuct, RngFlwLim);

  double Wr=Range(0.0002, QSuct/QMotive, 30.0);
  double Psr=PsrFn(Wr, AreaRatio);
  double Pcr=PcrFn(Psr, AreaRatio);
  double PBRqd=Max(P3/Pcr, 5.0);;
  return GEZ(P3-PBRqd);
  };

//---------------------------------------------------------------------------

void DPQ(CSpPropInfo *pProps, FlwBlk & FB, double K, double Area, double &Vel, double &DP, double &DPDQ)
  {
  double Rho=Max(0.001, FB.MeanRho(pProps));
  double DqScl=1.001;
  double Q0=CFlwBlkBase::RangeFlow(FB.QmMeas(), RngFlwLim);
  double Q1=CFlwBlkBase::RangeFlow(FB.QmMeas()*DqScl, RngFlwLim);
  double Vel0=Q0/(Rho*Area);
  double Vel1=Q1/(Rho*Area);
  Vel=Vel0;

  DP=PressDropKFact(Vel0, Rho, K);
  double DP1=PressDropKFact(Vel1, Rho, K);
  DPDQ=(DP1-DP)/GTZ(Q1-Q0);
  }

//---------------------------------------------------------------------------

flag Ejector::EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo)
  {
  if (fDoDbgBrk)
    { int ccc=0; }

  switch (Task)
    {
    case FET_GetMode:
      break;
    case FET_SetQm:
      break;
    case FET_CalcDP:
      {
      // all ios should be connected - enforced by IOGRP
      int ioPres=IOWithId_Self(IOId_InPres);
      int ioSuct=IOWithId_Self(IOId_InSuct);
      int ioOut=IOWithId_Self(IOId_Output);

      switch (IOId_Self(IONo))
        {
        case IOId_InPres:
          {
          FlwBlk & FB=*IOFB(IONo,0);

          double AreaNz=Diam2Area(daNzDiam);
          double Vel, DP, DPDQ;
          DPQ(pProps, FB, daKFact, AreaNz, Vel, DP, DPDQ);

          FB.SetVelocity(FB.QmSign()*Vel);
          FB.SetDPq(-FB.QmSign()*DP, -FB.QmSign()*DPDQ);
          FB.SetDPb(0.0, 0.0);
          FB.SetDerivsBad(false);

          if (fDoDbgBrk)
            { int ccc=0; }
          }
        break;
        case IOId_InSuct:
          {
          FlwBlk & FB=*IOFB(IONo,0);
          double AreaSc=Diam2Area(daNzDiam)*dAreaRatio;
          double dP3=IOFB(ioOut,0)->FlngPress();
          double q1=CFlwBlkBase::RangeFlow(FB.QmMeas(), RngFlwLim);
          double q2=CFlwBlkBase::RangeFlow(FB.QmMeas()*1.001, RngFlwLim);

          double AreaNz=Diam2Area(dbNzDiam);
          double Vel, DP, DPDQ;
          DPQ(pProps, FB, dbKFact, AreaNz, Vel, DP, DPDQ);

          double Rho=Max(0.001, FB.MeanRho(pProps));
          //double Vel=q1/(Rho*AreaSc);

          double P1=BstFn(dQa,  q1, dAreaRatio, dP3);
          double P2=BstFn(dQa,  q2, dAreaRatio, dP3);
          double P0=BstFn(dQa, 0.0, dAreaRatio, dP3);

          FB.SetVelocity(FB.QmSign()*Vel);
          FB.SetDPq(-(P0-P1)-FB.QmSign()*DP, -(P2-P1)/NZ(q2-q1) -FB.QmSign()*DPDQ);
          FB.SetDPb(P0, 0.0);
          FB.SetDerivsBad(false);

          if (fDoDbgBrk)
            { int ccc=0; }
          break;
          }
        case IOId_Output:
          {
          IOFB(IONo,0)->SetDPq(0.0, 0.0);
          IOFB(IONo,0)->SetDPb(0.0, 0.0);
          if (fDoDbgBrk)
            { int ccc=0; }
          break;
          }
        }
      break;
      }
    }
  return True;
  };

//---------------------------------------------------------------------------

void  Ejector::EvalJoinFlows(int JoinNo)
  {
  };

//---------------------------------------------------------------------------

void Ejector::EvalDiscrete()
  {
  // all ios should be connected - enforced by IOGRP
  int ioPres=IOWithId_Self(IOId_InPres);
  int ioSuct=IOWithId_Self(IOId_InSuct);
  int ioOut=IOWithId_Self(IOId_Output);

  dPa=IOFB(ioPres,0)->FlngPress();
  dPb=IOFB(ioSuct,0)->FlngPress();
  dP3=IOFB(ioOut,0)->FlngPress();

  dQa=GTZ(IOFB(ioPres,0)->m_QmEst);
  dQb=GTZ(IOFB(ioSuct,0)->m_QmEst);
  double Qa=CFlwBlkBase::RangeFlow(dQa, RngFlwLim);
  double Qb=CFlwBlkBase::RangeFlow(dQb, RngFlwLim);

  dWr=Range(0.0002, Qb/Qa, 30.0);
  dPsr=PsrFn(dWr, dAreaRatio);
  dPcr=PcrFn(dPsr, dAreaRatio);
  //double PBRqd=Max(dP3/dPcr, 5.0);;
  //dPSuct=dPSuctDmp*dPSuct+(1.0-dPSuctDmp)*GEZ(dP3-PBRqd);
  };

//---------------------------------------------------------------------------

dword  Ejector::ModelStatus()
  {
  dword Status=MN_Xfer::ModelStatus();

  return Status;
  };

//==========================================================================
