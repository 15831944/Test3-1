//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <float.h>

#include "sc_defs.h"
#include "datacnvs.h"
#include "flwnode.h"

#define  __HEATX_CPP
#include "heatx.h"

#include "dbgmngr.h"

#define dbgHeatX           0

#if dbgHeatX
static CDbgMngr  dbgHeater   ("M_HEATX",  "HeatXchg");
#endif

#define BOX(x,y,w,h) x,y, x,y+h, x+w,y+h, x+w,y, x,y

XID xidHXSd_dT = MdlBsXID(10004);

const byte idPIn  = 0;
const byte idPOut = 1;
const byte idSIn  = 2;
const byte idSOut = 3;

//===========================================================================
//
// Heat Exchanger
//
//===========================================================================
/*#D:#T:Heat Exchanger
#X:#h<General Description>#nGeneral purpose heat exchanger. ???
#n#n
#n#h<Inputs and Outputs>#w
Label    Input/Output  Minimum number  Maximum number#n
PriIn        Input          1              1#n
IP           Input          1              1#n
I            Input          1              1#n
PriOut       Output         1              1#n
OP           Output         1              1#n
O            Output         1              1#n
SecIn        Input          1              1#n
IS           Input          1              1#n
SecOut       Output         1              1#n
OS           Output         1              1#n
#a#n
#h<Variables to be supplied by the user>#n
#i<Z_Reqd> : Datum.#n
#i<HX.Model> : Select the heat exchange model to be used. A #x<heat exchange
block> will need to be configured.#n
#n
#n#n
#h<Associated variables>#n
#i<???> : #n
#n
#n#h<Other>#n
Default model prefix:HXCH#n
Short name:HeatXchg#n
Model type:Unit#n
#G:Units
*/

static IOAreaRec HeaterIOAreaList[] =
  {{"",              "PriIn" , idPIn , LIO_In0 ,  nc_MLnk, 1, 1, 1},//, "Side[iP].I"},
   {"",              "IP"    , idPIn , LIO_In  ,  nc_MLnk, 1, 1, 1},//, "Side[iP].I"},
   {"",              "I"     , idPIn , LIO_In  ,  nc_MLnk, 1, 1, 1},//, "Side[iP].I"},
   {"",              "PriOut", idPOut, LIO_Out0,  nc_MLnk, 1, 1, 1},//, "Side[iP].O"},
   {"",              "OP"    , idPOut, LIO_Out ,  nc_MLnk, 1, 1, 1},//, "Side[iP].O"},
   {"",              "O"     , idPOut, LIO_Out ,  nc_MLnk, 1, 1, 1},//, "Side[iP].O"},
   {"",              "SecIn" , idSIn , LIO_In1 ,  nc_MLnk, 1, 1, 2},//, "Side[iS].I"},
   {"",              "IS"    , idSIn , LIO_In  ,  nc_MLnk, 1, 1, 2},//, "Side[iS].I"},
   {"",              "SecOut", idSOut, LIO_Out1,  nc_MLnk, 1, 1, 2},//, "Side[iS].O"},
   {"",              "OS"    , idSOut, LIO_Out ,  nc_MLnk, 1, 1, 2},//, "Side[iS].O"},
   {NULL}};


static double Drw_Heater[] = {DD_Poly, -6,-6, -6,6, 6,6, 6,-6, -6,-6,
                            DD_Poly, -5,-6, -5,-3, -4,3, -3,-3, -2,3, -1,-3, 0,3,
                                     1,-3, 2,3, 3,-3, 4,3, 5,-3, 5,-6,
                            DD_End };

IMPLEMENT_MODELUNITHIDDEN(HeatXchg, "HeatXchg", "", Drw_Heater, "HeatExchange", "HXCH", TOC_ALL|TOC_GRP_ENERGY|TOC_STD_KENWALT, "Heat Exchanger", "A Long Description")
HeatXchg::HeatXchg(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MdlNode(pClass_, TagIn, pAttach, eAttach),
  //EffectCurve("DataOnly", "Effective", this, TOA_Embedded)
  //HXB(&CHXBlockClass, "HX", this, TOA_Embedded)
  HX(this, &CHXBlockClass),
  m_HXJ(this, idPIn, idPOut, idSIn, idSOut),
  FBP(NULL, 0, 1.0),
  FBS(NULL, 0, 1.0)
  {
  AttachClassInfo(nc_Process, HeaterIOAreaList, &HeatXDPGroup);
  //HXB.SetPri(this, idPIn, idPOut);
  //HXB.SetSec(this, idSIn, idSOut);

  HX.Open(true);
  FBP.AssignFlwEqnGroup(HeatXDPGroup, HeatXDPGroup.Default(), this);
  FBS.AssignFlwEqnGroup(HeatXDPGroup, HeatXDPGroup.Default(), this);

  }


//--------------------------------------------------------------------------

//struct HXJoinInfo
//  {
//  int JoinId;
//  int ioIn[20], ioOut;
//  };
//
//
//flag PriIsJoin(pFlwNode p, HXJoinInfo & Info)
//  {
//  int j=0;
//  Info.ioOut=-1;
//  for (int i=0; i<p->NIOs; i++)
//    if ((p->IOId_Self(i)==idPIn)||(p->IOId_Self(i)==idPOut))
//      if (p->IO_In(i))
//        Info.ioIn[j++] = i;
//      else if (p->IO_Out(i))
//        Info.ioOut = i;
//  Info.ioIn[j++] = -1;
//  if (Info.ioIn[0]>=0 && Info.ioOut>=0)
//    {
//    Info.JoinId=0;
//    return True;
//    }
//  Info.JoinId=-1;
//  return False;
//  }
//
////--------------------------------------------------------------------------
//
//flag SecIsJoin(pFlwNode p, HXJoinInfo & Info)
//  {
//  int j=0;
//  Info.ioOut=-1;
//  for (int i=0; i<p->NIOs; i++)
//    if ((p->IOId_Self(i)==idSIn)||(p->IOId_Self(i)==idSOut))
//      if (p->IO_In(i))
//        Info.ioIn[j++] = i;
//      else if (p->IO_Out(i))
//        Info.ioOut = i;
//  Info.ioIn[j++] = -1;
//  if (Info.ioIn[0]>=0 && Info.ioOut>=0)
//    {
//    HXJoinInfo m_HXJ.P;
//    if (PriIsJoin(p, m_HXJ.P))
//      Info.JoinId=1;
//    else
//      Info.JoinId=0;
//    return True;
//    }
//  Info.JoinId=-1;
//  return False;
//  }
//
//--------------------------------------------------------------------------

void HeatXchg::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this, NULL, NULL, DDB_NoPage);

  BuildDataDefnElevation(DDB);

  DDB.Text(" ");
  BuildDataDefnShowIOs(DDB);
  BuildDataDefnIOOpts(DDB);
  DDB.Text(" ");

  if (DDB.BeginStruct(this, "SecDP", NULL, DDB_NoPage, 2))
    FBS.BuildDataDefn(FBDDOpt_WithEqn, DDB, this);
  DDB.EndStruct();

  if (DDB.BeginStruct(this, "PriDP", NULL, DDB_NoPage, 1))
    FBP.BuildDataDefn(FBDDOpt_WithEqn, DDB, this);
  DDB.EndStruct();

  DDB.Text(" ");
  HX.BuildDataDefn(DDB, NULL, NULL, DDB_OptPage, 0);
  //DDB.Object(&HXB, this, NULL, NULL, DDB_RqdPage);
  //HXB.BuildDataDefn(DDB, True, IOWithId_Self(idPIn)>=0 && IOWithId_Self(idPOut),
  //                       True, IOWithId_Self(idSIn)>=0 && IOWithId_Self(idSOut));


  DDB.EndStruct();
  }

//---------------------------------------------------------------------------

flag HeatXchg::DataXchg(DataChangeBlk & DCB)
  {
  if (MdlNode::DataXchg(DCB))
    return 1;
  if (HX.DataXchg(DCB))
    return 1;
  switch (DCB.dwUserInfo)
    {
    case 1 : FBP.DataXchg(DCB); return 1;
    case 2 : FBS.DataXchg(DCB); return 1;
    }

  return 0;
  }

//---------------------------------------------------------------------------

void HeatXchg::StartSolution()
  {
//  FBP.StartSolution();
//  FBS.StartSolution();
  }

//---------------------------------------------------------------------------

void HeatXchg::PostConnect(int IONo)
  {
  MdlNode::PostConnect(IONo);

  if (IOId_Self(IONo)>=idPIn && IOId_Self(IONo)<=idPOut)
    IOFB(IONo,0)->SetParentFlwEqn(&FBP);
  else if (IOId_Self(IONo)>=idSIn && IOId_Self(IONo)<=idSOut)
    IOFB(IONo,0)->SetParentFlwEqn(&FBS);
  }

//---------------------------------------------------------------------------

void HeatXchg::ConfigureJoins()
  {
  m_HXJ.SetJoinIds(0, True, True);
  if (m_HXJ.PriIsJoined())
    {
    for (int i=0; m_HXJ.P.IOJoin(i)>=0; i++)
      SetIO_Join(m_HXJ.P.IOJoin(i), m_HXJ.P.JoinId());
    }
  else
    {
    for (int i=0; m_HXJ.P.IOJoin(i)>=0; i++)
      SetIO_Closed(m_HXJ.P.IOJoin(i), m_HXJ.P.JoinId());
    }

  if (m_HXJ.SecIsJoined())
    {
    for (int i=0; m_HXJ.S.IOJoin(i)>=0; i++)
      SetIO_Join(m_HXJ.S.IOJoin(i), m_HXJ.S.JoinId());
    }
  else
    {
    for (int i=0; m_HXJ.S.IOJoin(i)>=0; i++)
      SetIO_Closed(m_HXJ.S.IOJoin(i), m_HXJ.S.JoinId());
    //if (m_HXJ.S.ioOut>=0) SetIO_Closed(m_HXJ.S.ioOut);
    }

  }

//--------------------------------------------------------------------------

flag HeatXchg::EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo)
  {
  if (IONo==m_HXJ.P.IOJoin(0))
    {
    IOFB(IONo,FE)->EvaluateFlwEqn(Task, pProps, true, false, 1.0, &IOFBFlng_Rmt(IONo)->PhD(), NULL);
    return true;
    }
  if (IONo==m_HXJ.S.IOJoin(0))
    {
    IOFB(IONo,FE)->EvaluateFlwEqn(Task, pProps, true, false, 1.0, &IOFBFlng_Rmt(IONo)->PhD(), NULL);
    return true;
    }
  return false;
  };

//--------------------------------------------------------------------------

void HeatXchg::EvalProducts(CNodeEvalIndex & NEI)
  {
  //HXJoinInfo m_HXJ.P, m_HXJ.S;
  if (m_HXJ.PriIsJoined())
    {
    //HXB.SdP.MeasureFeeds(this, *IOConduit(m_HXJ.P.ioOut), idPIn, idPOut);
    if (m_HXJ.P.IOFlwIn(0)>=0 && m_HXJ.P.IOFlwOut()>=0)
      {
      SpConduit & CdOut=*IOConduit(m_HXJ.P.IOFlwOut());
      double PriPIn=SigmaQInPMin(CdOut, som_ALL, Id_2_Mask(idPIn)|Id_2_Mask(idPOut));//Out);
      HX.SetPrimary(NULL, CdOut, PriPIn);
      }
    //if (HXB.SdP.FTC.Active())
    //  for (int i=0; i<NIOs; i++)
    //    if (IOId_Self(i)==idPIn)
    //      IOFlange(i)->SetPBFlwScl(HXB.Pri.FTC.dQRqd);
    }

  if (m_HXJ.SecIsJoined())
    {
    if (m_HXJ.S.IOFlwIn(0)>=0 && m_HXJ.S.IOFlwOut()>=0)
      {
      SpConduit & CdOut=*IOConduit(m_HXJ.S.IOFlwOut());
      double SecPIn=SigmaQInPMin(CdOut, som_ALL, Id_2_Mask(idSIn));//Out);
      HX.SetSecondary(NULL, CdOut, SecPIn);
      //HXB.SdS.MeasureFeeds(this, *IOConduit(m_HXJ.S.ioOut), idSIn, idSOut);
      }
    }

  //MeasureFeeds(pFlwNode pNd_, rSpConduit rCd, double PIn, CEnvironHX * EHX=NULL, CReactionBlock * RB=NULL);


  HX.EvalProducts();

  }

//===========================================================================
