//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#include "sc_defs.h"
#include "datacnvs.h"
#include "flwnode.h"
#include "mg_bhpv.h"

#include "net_err.h"
/*
*****************************************************************************


This model only been made to compile so far - plenty fixes needed


******************************************************************************
*/
#include "debugnew.h" // must follow all IMPLEMENT_DYNCREATE & IMPLEMENT_SERIAL

long Drw_BHPValve[] = { DD_Poly, -5,5, -5,-5, 5,-5, 5,5, -5,5,
                     DD_End };

const byte idIn    = 0;

static IOAreaRec BHPValveIOAreaList[] =
  {{"I",       "I"          , idIn,   LIO_In ,  nc_MLnk, 1, 1, 0, 0.0f, ""},
   {NULL}};

IMPLEMENT_MODELUNIT(BHPValve, "BHPValve", "", Drw_BHPValve, "BHP", TOC_ALL|TOC_GRP_ENERGY|TOC_USER,
                    "BHPValve",
                    "BHPValve")

//--------------------------------------------------------------------------

BHPValve::BHPValve(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MN_SrcSnk(pClass_, TagIn, pAttach, eAttach)
  {
  AttachClassInfo(nc_Process, BHPValveIOAreaList);

  FlareTag="VG1601";

  const double QIni[NBHPVs] = {10.0,1.0,50.0,50.0};
  PSetP=Std_P+9310.0;
  PGapP=20.0;
  Flash=0;
  TempO=0.0;

  for (int i=0; i<NBHPVs; i++)
    {
    bAuto[i]=0;
    YRqd[i]=0.0;
    Res[i]=0.0;
    QPt[i]=QIni[i];
    PPt[i]=8000.0;
    }

  FlareDrum=NULL;

  ValidateDataBlk VDB;
  ValidateData(VDB);

  };

//--------------------------------------------------------------------------

void   BHPValve::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);

  DDB.Text("");
  DDB.Bool   ("",       "BDV_Auto",         DC_,        "",       &bAuto[0], this  ,isParm);
  DDB.Bool   ("",       "HCV_Auto",         DC_,        "",       &bAuto[1], this  ,isParm);
  DDB.Bool   ("",      "PSV1_Auto",         DC_,        "",       &bAuto[2], this  ,isParm);
  DDB.Bool   ("",      "PSV2_Auto",         DC_,        "",       &bAuto[3], this  ,isParm);

  DDB.Double ("",      "BDV_Y_Rqd",     DC_Frac,       "%",        &YRqd[0], this  ,isParm);
  DDB.Double ("",      "HCV_Y_Rqd",     DC_Frac,       "%",        &YRqd[1], this  ,isParm);
  DDB.Double ("",     "PSV1_Y_Rqd",     DC_Frac,       "%",        &YRqd[2], this  ,isParm);
  DDB.Double ("",     "PSV2_Y_Rqd",     DC_Frac,       "%",        &YRqd[3], this  ,isParm);

  DDB.Page("Page 2",DDB_RqdPage);
  DDB.String ("Flare",        "",           DC_,         "",     &FlareTag,   this,  isParm);
  DDB.Text("");
  DDB.Double ("",    "PSVSet",            DC_P,     "kPag",        &PSetP,   this  ,isParm);
  DDB.Double ("",    "PSVGap",           DC_DP,      "kPa",        &PGapP,    this  ,isParm);
  DDB.Text("");
  DDB.Double ("",    "BDVPPt",           DC_DP,      "kPa",       &PPt[0],   this  ,isParm);
  DDB.Double ("",    "BDVQPt",           DC_Qm,     "kg/s",       &QPt[0],   this  ,isParm);
  DDB.Double ("",    "BDVRes",             DC_,         "",       &Res[0],   this  ,isParm);
  DDB.Text("");
  DDB.Double ("",    "HCVPPt",           DC_DP,      "kPa",       &PPt[1],   this  ,isParm);
  DDB.Double ("",    "HCVQPt",           DC_Qm,     "kg/s",       &QPt[1],   this  ,isParm);
  DDB.Double ("",    "HCVRes",             DC_,         "",       &Res[1],   this  ,isParm);
  DDB.Text("");
  DDB.Double ("",    "PSVPPt",           DC_DP,      "kPa",       &PPt[2],   this  ,isParm);
  DDB.Double ("",    "PSVQPt",           DC_Qm,     "kg/s",       &QPt[2],   this  ,isParm);
  DDB.Double ("",    "PSVRes",             DC_,         "",       &Res[2],   this  ,isParm);
  DDB.Text("");
  DDB.Bool   ("",     "Flash",             DC_,         "",        &Flash,   this  ,isParm);
  DDB.Text("");

  DDB.EndStruct();

  };

//--------------------------------------------------------------------------

flag BHPValve::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=MN_SrcSnk::ValidateData(VDB);

  PPt[3]=PPt[2];
  QPt[3]=QPt[2];
  Res[3]=Res[2];

  for (int i=0; i<NBHPVs; i++)
    if (Valid(QPt[i]) && Valid(PPt[i]))
      Res[i]=fabs(PPt[i])/GTZ(fabs(QPt[i]));

  return OK;
  }

//--------------------------------------------------------------------------

void BHPValve::EvalJoinPressures()
  {
  MN_SrcSnk::EvalJoinPressures();
  }

//--------------------------------------------------------------------------

//flag BHPValve::EvalFlowEquations(CSpPropInfo *pProps, int iJoinId, int FE)
//  {
//  return False;
//  }

//--------------------------------------------------------------------------

flag BHPValve::EvalFlowEquations(CSpPropInfo *pProps, int IONo, int FE)
  {
  //pFlange pC = IOFlange(IONo);
  FlwBlk &FB  = *IOFB(IONo, FE);
  double QIn  = FB.QmMeas();
  double R_Fxd=0.0;
  if (IOId_Self(IONo)==0)
    {
    int N=0;
    for (int i=0; i<NBHPVs; i++)
      if (bAuto[i] && (YRqd[i] > MinValveOpening))
        {
        double R=Res[i]/YRqd[i];
        R_Fxd = N++==0 ? R : (R_Fxd*R)/GTZ(R_Fxd+R);
        }

    if (N>0)
      {
      FB.SetDPb(0.0, 0.0);
      FB.SetDPq(-QIn*R_Fxd, -Sign(QIn)*R_Fxd);
      FB.SetQmFree();
      return True;
      }
    }
  FB.SetQmReqd(0.0);
  FB.SetDPq(0.0, 0.0);
  return True;
  };

// --------------------------------------------------------------------------

void BHPValve::EvalDerivs()
  {
  if ((FlareDrum==NULL) && (FlareTag[0]!='~'))
    {
    FlareDrum = (pMdlNode)FamilyHead()->FindObj(FlareTag());
    if (FlareDrum==NULL)
      FlareTag[0]='~';
    }

  };

// --------------------------------------------------------------------------

void BHPValve::EvalCtrlActions()
  {
  if (NoFlwIOs()>=1)
    {
    YRqd[2] = 0.7*YRqd[2] + 0.3*Range(0.0, (IOP_Rmt(0)-PSetP)/Max(1.0, PGapP), 1.0);
    YRqd[3] = YRqd[2];
    }
  }

//==========================================================================
