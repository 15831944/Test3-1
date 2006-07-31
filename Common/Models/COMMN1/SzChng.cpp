//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include "sc_defs.h"
#define  __SZCHNG_CPP
#include "szchng.h"

//===========================================================================

//XID xidMethod           = MdlBsXID(10021);
//XID xidEnterExtents     = MdlBsXID(10022);

//===========================================================================
//
//                                SzChng
//
//===========================================================================


const byte ioidFeed   = 0;
const byte ioidProd   = 1;

static IOAreaRec SzChngIOAreaList[] =
  {{"",              "Feed"      , ioidFeed, LIO_In0 ,  nc_MLnk, 1,  1, IOSetXfer|IOGRP(1), 0.0F},
   {"",              "Product"   , ioidProd, LIO_Out0,  nc_MLnk, 1,  1, IOSetXfer|IOGRP(1), 0.0F},
   {NULL}};
           
static double Drw_SzChng[] = { DD_Poly, -4,-2, -4,2, 4,2, 4,-2, -4,-2,
                      DD_Poly, -4,0, 4,0,
                      DD_End };

//===========================================================================

IMPLEMENT_MODELUNIT(SzChng, "SzChng", "", Drw_SzChng, "SizeAlteration", "SZCH", TOC_ALL|TOC_DYNAMICFLOW|TOC_GRP_SIZEDST|TOC_SIZEDIST, 
                    "Size Alteration:Change Sieve Series", "")

SzChng::SzChng (pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MdlNode(pClass_, TagIn, pAttach, eAttach)
  {
  AttachClassInfo(nc_Process, SzChngIOAreaList);

  bOnLine=1;
  iDistRqd = 0;
  }

//--------------------------------------------------------------------------

SzChng::~SzChng()
  {
  }

//--------------------------------------------------------------------------

void SzChng::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);

  DDB.Text("");
  DDBValueLstMem DDB0;
  DDB0.Empty();
  for (int i=0; i<SD_Defn.NDistributions(); i++)
    DDB0.Add(i, SD_Defn.GetDist(i)->Name());
  DDB.CheckBox("On",           "",    DC_,      "",    &bOnLine,        this, isParm|SetOnChange);
  DDB.Long    ("SizeDefn",     "",    DC_,      "",    &iDistRqd,       this, isParm, DDB0());
  DDB.Text("");
  BuildDataDefnShowIOs(DDB);
  
  DDB.EndStruct();
  }

//--------------------------------------------------------------------------

flag SzChng::DataXchg(DataChangeBlk & DCB)
  {
  if (MdlNode::DataXchg(DCB))
    return 1;

  return 0;
  }

//--------------------------------------------------------------------------

flag SzChng::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK = MdlNode::ValidateData(VDB);
  //OK = OK && CB.ValidateData(VDB);
  return OK;
  }

//-------------------------------------------------------------------------- 

void SzChng::SetDatums(int Pass, CFlwNodeIndexList & List, int IOIn)
  {
  SetDatums_Node(Pass, List, IOIn, NULL);
  }
 
//-------------------------------------------------------------------------- 

flag SzChng::Set_Sizes()
  {
  return True;
  }

//---------------------------------------------------------------------------

void SzChng::ConfigureJoins()
  {
  Init_NoJoins(1);
  int i;
  switch (NetMethod())
    {
    case NM_Probal:
      for (i=0; (i<NoProcessIOs()); i++)
        SetIO_Join(i, 0);
      break;
    case NM_Dynamic:
      for (i=0; (i<NoProcessIOs()); i++)
        SetIO_Open(i, 0, false, ESS_Denied);
      break;
    }
  };

//--------------------------------------------------------------------------

flag SzChng::InitialiseSolution()
  {
  return True;
  };

//--------------------------------------------------------------------------

void SzChng::EvalProducts(CNodeEvalIndex & NEI)
  {
  const int ioProd = IOWithId_Self(ioidProd);
  const int ioFeed = IOWithId_Self(ioidFeed);
  if (ioProd>=0 && ioFeed>=0)
    {
    SpConduit Disch("Disch", this, TOA_Free); //working copy
    SigmaQInPMin(Disch, som_ALL, Id_2_Mask(ioidFeed));
    IOConduit(ioProd)->QCopy(Disch);
    if (bOnLine)
      {
      flag b = SQSzDist1::ChangeDistribution(/**IOConduit(ioFeed)*/Disch, *IOConduit(ioProd), iDistRqd);
      }
    }
  }

//--------------------------------------------------------------------------

dword SzChng::ModelStatus()
  {
  dword Status=MdlNode::ModelStatus();
  if (NoFlwIOs()>1)
    {
    const int iIn  = (IOQmEst_In(0)>0.0 ? 0 : 1);
    Status |= (IOConduit(iIn)->QMass()>UsableMass ? FNS_UFlw : FNS_UNoFlw);
    }
  else if (NoFlwIOs()==1)
    Status |= FNS_Error;
  return Status;
  }

//--------------------------------------------------------------------------
//===========================================================================
