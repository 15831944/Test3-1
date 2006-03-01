//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdio.h>

#include "sc_defs.h"
#define  __HYDROCYC_CPP
#include "hydrocyc.h"
#include "dbgmngr.h"

//#include "sqszdst1.h"

#define dbgModels  01
#if dbgModels
static CDbgMngr dbgHydroCyclone("Models", "HydroCyc");
#endif

//===========================================================================
//
//                                 HydroCyclone
//
//===========================================================================

const byte ioidFeed  = 0;
const byte ioidFines = 1;
const byte ioidGrits = 2;

static IOAreaRec HydroCycloneIOAreaList[] =
  {{"Cyclone Feed",      "Feed"      , ioidFeed , LIO_In0 ,  nc_MLnk, 1, 1,            0, 0.0F},
   {"",                  "I"         , ioidFeed , LIO_In ,  nc_MLnk, 1, 1,            0, 0.0F},
   {"Cyclone Overflow",  "Fines"     , ioidFines, LIO_Out0,  nc_MLnk, 1, 1,            0, 0.0F},
   {"Cyclone Underflow", "Grits"     , ioidGrits, LIO_Out1,  nc_MLnk, 1, 1, dwIOIsBuffer, 0.0F}, //Out.Out"},
   {NULL}};
           
long Drw_HydroCyclone[] = { DD_Poly, -8,4, -8,12, 8,12, 8,4, 4,-12, -4,-12, -8,4, 8,4,
                           DD_Poly, BOX(-4,12, 8,4),
                           DD_End };

//--------------------------------------------------------------------------

IMPLEMENT_MODELUNIT(HydroCyclone, "HydroCyc", "", Drw_HydroCyclone, "HC", TOC_ALL|TOC_GRP_SIZEDST|TOC_USER,
                    "Hydro Cyclone", "") 
HydroCyclone::HydroCyclone(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MdlNode(pClass_, TagIn, pAttach, eAttach),
  PartCrv("Partition", "PartCrv", this, TOA_Embedded),
  Feed("Feed", this, TOA_Embedded)//,
  {
  AttachClassInfo(nc_Process, HydroCycloneIOAreaList);
  Ore2Grit = 0.2;
  Liq2Grit = 0.5;
  ByePass2Grits = 0.0;
  GritFlowRatio = 0.0;
  };

//--------------------------------------------------------------------------

HydroCyclone::~HydroCyclone ()
  {
  };

//--------------------------------------------------------------------------

void HydroCyclone::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);
  DDB.Double("ByePass2Grits", "",       DC_Frac, "%",      &ByePass2Grits,  this,isParm);
  DDB.Double("OreToGrits",    "",       DC_Frac, "%",      &Ore2Grit,       this, DynMode() ? 0 : isParm);
  DDB.Double("LiqToGrits",    "",       DC_Frac, "%",      &Liq2Grit,       this, DynMode() ? 0 : isParm);

  DDB.Text("");
  GSM.Add_OnOff(DDB);
  GSM.BuildDataDefn(DDB, NULL, NULL, DDB_OptPage);

  DDB.Object(&PartCrv, this, NULL, NULL, DDB_RqdPage);


  DDB.EndStruct();
  };

// --------------------------------------------------------------------------

flag HydroCyclone::DataXchg(DataChangeBlk & DCB)
  {
  if (MdlNode::DataXchg(DCB))
    return 1;
  return 0;
  }

// --------------------------------------------------------------------------
// hss commented out to force a compile
/*flag HydroCyclone::ValidateData(ValidateDataBlk & VDB)
  {
  if (DynMode())
    //PartCrv.SetXPts(SQSzDist1::TylerSeries);
  return True;
  }
*/
// -------------------------------------------------------------------------

flag HydroCyclone::ChangeMode(dword NewMode, dword OldMode) 
  { 
  if (NewMode==PBMODE)
    GSM.Open();

  return True; 
  };

//---------------------------------------------------------------------------
// hss commented out to force a compile
/*flag HydroCyclone::InitialiseSolution()
  {
  if (DynMode())
    //PartCrv.SetXPts(SQSzDist1::TylerSeries);
  return True;
  }*/

//---------------------------------------------------------------------------

void HydroCyclone::ConfigureJoins()
  {
  Init_NJoins(1);
  int i;
  switch (SolveMode())
    {
    case PBMODE:
    case SSMODE:
      for (i=0; (i<NoFlwIOs()); i++)
        SetIO_Join(i, 0, 0);
      break;
    case DYNMODE:
      for (i=0; (i<NoFlwIOs()); i++)
        if (IOId_Self(i)==ioidGrits)
          SetIO_Open(i, 1, false, ESS_Denied);
//          SetIO_Open(i);
        else
          {
          SetIO_Join(i, 0, 0);
          if (IOId_Self(i)==ioidFines)
            SetJoinQm_QmRatio (0, i);
          }

      break;
    }
  };

// --------------------------------------------------------------------------

void HydroCyclone::EvalJoinFlows(int JoinNo)
  {
  double QmVIn = 0.0, QmSIn = 0.0;
  for (int i=0; i<NoFlwIOs() && IOId_Self(i)==ioidFeed ; i++)
    if (IO_In(i))
      {
      pSpConduit p=IOConduit(i);
      QmVIn += p->QMass(som_Liq);
      QmSIn += p->QMass(som_Sol);
      }

dbgpln("Hydrocyclone to be sorted out Liquids not done");

  long Failed = 0;
  double Passing=0;
  int nIn1=IOWithId_Self(ioidFeed);
  if (nIn1 >= 0)
    {
    pSQSzDist1 pSz;
    pSz=SQSzDist1::Ptr(IOConduit(nIn1)->Model());
    Ore2Grit=pSz->GritsFraction(PartCrv, 0.0, ByePass2Grits);
    GritFlowRatio=(Ore2Grit*QmSIn)/GTZ(QmVIn + (1.0-Ore2Grit)*QmSIn);
    Joins[0].SetQm_QmRatio(Max(1.0e-6, GritFlowRatio));
    }
  };

//--------------------------------------------------------------------------

void HydroCyclone::EvalProducts()
  {
  if (GSM.Enabled())
    {
    if (NJoins()>0)
      switch (SolveMode())
        {
        case PBMODE:
        case SSMODE:
          if (NJoins()>=1)
            Xfer_EvalProducts(0, Joins[0].Pressure(), NULL, NULL, NULL, GSM(), NULL);
          break;
        case DYNMODE:
          for (int j=0; j<NJoins(); j++)
            Xfer_EvalProducts(j, Joins[j].Pressure(), NULL, NULL, NULL, GSM(), NULL);
          break;
        }
    }
  else
    {
    Feed.QZero();
    Feed.SetPress(Std_P);
    double  Liq_in=0.0, Solids_in=0.0;
    for (int i = 0; i < NoFlwIOs(); i++)
      if (IO_In(i))
        {
        Solids_in+=IOConduit(i)->QMass(som_Sol);
        Liq_in+=IOConduit(i)->QMass(som_Liq);
        Feed.QAddF(*IOConduit(i), som_ALL, 1.0);
        }

    int ioFines=IOWithId_Self(ioidFines);
    int ioGrits=IOWithId_Self(ioidGrits);

    if (ioFines>=0 && IO_Out(ioFines))// && ioGrits>=0 && IO_Out(ioGrits))
      {
      double QmOre2Grit=Solids_in*Ore2Grit;
      double QmOre2Fine=Solids_in-QmOre2Grit;
      double QmLiq2Grit=Liq_in*Liq2Grit;
      double QmLiq2Fine=Liq_in-QmLiq2Grit;

      SpConduit & Fines = *IOConduit(ioFines);
      SpConduit & Grits = *IOBuffer(ioGrits);

      Fines.QSetM(Feed, som_Liq, QmLiq2Fine, IOP_Self(ioFines));
      Fines.QAddM(Feed, som_Sol, QmOre2Fine);
      Grits.QSetM(Feed, som_Liq, QmLiq2Grit, IOP_Self(ioGrits));
      Grits.QAddM(Feed, som_Sol, QmOre2Grit);

      if (SQSzDist1::Ptr(Feed.Model(), False))
        {
        SQSzDist1 &SzFeed=*SQSzDist1::Ptr(Feed.Model());
        SQSzDist1 &SzFine=*SQSzDist1::Ptr(Fines.Model());
        SQSzDist1 &SzGrit=*SQSzDist1::Ptr(Grits.Model());

        SzFine.SetFinesFraction(PartCrv, 0.0, ByePass2Grits);
        SzGrit.SetGritsFraction(PartCrv, 0.0, ByePass2Grits);
        }
      }
    }
  }

//===========================================================================
