//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992
//
//
//

#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#include "sc_defs.h"
#include "datacnvs.h"

#include "mg_sys14.h"
#include "debugnew.h"

const byte   idOt = 0;
const byte   idOp = 1;

long Drw_System14[] = { DD_Poly, -5,5, -5,-5, 5,-5, 5,5, -5,5,
                        DD_End};

IMPLEMENT_MODELUNIT(Sys14, "System14", "", Drw_System14, "S14", TOC_ALL|TOC_GRP_ENERGY|TOC_USER,
                    "System14",
                    "System14")

static IOAreaRec S14IOAreaList[] =
  {{"Ot",       "Ot"        , idOt,   LIO_Out,  nc_MLnk, 1, 1, 0, 0.0f, ""},
   {"Op",       "Op"        , idOp,   LIO_Out,  nc_MLnk, 1, 1, 0, 0.0f, ""},
   {NULL}};

/*
struct tFlowLineMap
  {
  int iWellNo;
  //int bHasCV;
  int iCVNo;
  };
static tFlowLineMap FlowLineMap[] =
//   1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18}; // Straight thru lines
 {{  1, 0    }, //  1
  {  2, 0    }, //  2
  {  3, 0    }, //  3
  {  4, 0    }, //  4
  {  4, 0    }, //  5
  {  6, 0    }, //  6
  {  7, 0    }, //  7
  {  8, 0    }, //  8
  {  9, 0    }, //  9
  { 10, 0    }, // 10
  { 11, 0    }, // 11
  { 12, 0    }, // 12
  { 17, 17   }, // 13
  { 14, 0    }, // 14
  { 10, 0    }, // 15
  { 11, 0    }, // 16
  { 17, 0    }, // 17
  { 18, 0    }  // 18
  };

static int WellDisp[] =
  {  4,  5,
    10, 15,
    11, 16,
    17, 13,
    -1, -1,
    -1, -1,
    -1, -1,
    -1, -1,
    -1, -1};
*/

struct tFlowLineMap
  {
  int iX;
  int iY;
  char * WellName;
  };
// This mapping was updated by Dave Bean on 16 January 1997
static tFlowLineMap FlowLineMap[] =
//   1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18}; // Straight thru lines
 {{ 17, 13, "FA01P(17&13)" }, // Pair FA01
  { 12, 03, "FA02P(12&03)" }, //
  { 15, 10, "FA03P(15&10)" },
  {  8,  0, "FA04P(08)" },
  {  5,  4, "FA05P(05&04)" },
  {  1,  0, "FA06P(01)" },
  {  6, 11, "FA07P(06&11)" },
  { 14,  7, "FA08P(14&07)" },
  { 16,  0, "FA09P(16***)" },
  {  0,  0, "" },
  };

//=========================================================================

//static IOAreaRec S14IOAreaList[] =
//  {{"Ot"    , 0, IO_Out,  nc_MLnk, 1, ""},
//   {"Op"    , 1, IO_Out,  nc_MLnk, 1, ""},
//   {NULL}};

// -------------------------------------------------------------------------
Sys14::Sys14(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MdlNode(pClass_, TagIn, pAttach, eAttach),
  Contents("Contents", this, TOA_Embedded)
  {
  AttachClassInfo(nc_Process, S14IOAreaList);
  AllDataHere = 1;
  ChkChar=1.0;
  FlrNQv=0.0;
  FlrQvPt=0.0;
  ManR=0.0033;
  NomManP=Std_P+8230.0;
  InertiaTau=10.0;
  InertiaScl=100.0;
  DP6Tau=60.0;
  T_Rqd=C_2_K(90.0);

  bBDV14003=0;
  bBDV14004=0;
  bBDV14007=0;
  bBDV14008=0;
  bESDV14005=0;
  bESDV14006=0;

  TI201=dNAN();
  PI201=dNAN();
  TI202=dNAN();
  PI203=dNAN();

  for (word w=0; w<NoofWells; w++)
    {
    rAWell W=Wells[w];
    W.bSSV1=0;
    W.bMSV2=0;
    W.bWSV3=0;
    W.BPV1=0.0;
    //W.bBPV8=0;
    W.bHCV5=0;
    W.bHCV6=0;
    W.bHCV7=0;
    W.PI3=AtmosPress()+20000.0; // drops down to +- 14000
    W.DPI3=0.0;
    //W.DPI6=0.0;
    W.CV4=0.0;
    W.PI5=AtmosPress();
    W.HCV7=0.0;
    W.NQv=0.0;
    W.NQvPt=0.0;
    W.ChkOpnPt=0.0;
    W.ChkOpnSt=0.0;
    W.WellP=AtmosPress()+20000.0;
    W.WellR=3000.0/16.5;
    W.FQv=0.0;
    W.pNQv=0.0;
    W.InerP=0.0;
    }
  };

// -------------------------------------------------------------------------

void Sys14::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);

  DDB.Page      ("Variables");
  DDB.Double    ("FlrNQv",           "",      DC_NQv,   "Nm^3/h",  &FlrNQv             ,this  ,0);
  DDB.Double    ("FlrNQvPt",         "",      DC_NQv,   "Nm^3/h",  &FlrQvPt            ,this  ,isParm);
  DDB.Double    ("Temp_Rqd",         "T_Rqd", DC_T  ,   "C",       &T_Rqd              ,this  ,isParm);
  DDB.Double    ("ManR",             "",      DC_,      "",        &ManR               ,this  ,isParm);
  DDB.Double    ("NomManP",          "",      DC_P,     "kPag",    &NomManP            ,this  ,isParm);
  DDB.Double    ("InertiaTau",       "",      DC_Time,  "sec",     &InertiaTau         ,this  ,isParm);
  DDB.Double    ("InertiaScl",       "",      DC_,      "",        &InertiaScl         ,this  ,isParm);
  DDB.Double    ("DP6Tau",           "",      DC_Time,  "sec",     &DP6Tau             ,this  ,isParm);
  DDB.Double    ("TI14201",          "",      DC_T,     "C",       &TI201              ,this  ,0);
  DDB.Double    ("PI14201",          "",      DC_P,     "kPag",    &PI201              ,this  ,0);
  DDB.Double    ("TI14202",          "",      DC_T,     "C",       &TI202              ,this  ,0);
  DDB.Double    ("PI14203",          "",      DC_P,     "kPag",    &PI203              ,this  ,0);
  DDB.Bool      ("BDV14003.Auto",    "",      DC_,      "",        &bBDV14003          ,this  ,isParm);
  DDB.Bool      ("BDV14004.Auto",    "",      DC_,      "",        &bBDV14004          ,this  ,isParm);
  DDB.Bool      ("BDV14007.Auto",    "",      DC_,      "",        &bBDV14007          ,this  ,isParm);
  DDB.Bool      ("BDV14008.Auto",    "",      DC_,      "",        &bBDV14008          ,this  ,isParm);
  DDB.Bool      ("ESDV14005.Auto",   "",      DC_,      "",        &bESDV14005         ,this  ,isParm);
  DDB.Bool      ("ESDV14006.Auto",   "",      DC_,      "",        &bESDV14006         ,this  ,isParm);
  //DDB.Double    ("HCV14001",         "",      DC_P,     "kPag",    &HCV14001           ,this  ,0);
  //DDB.Double    ("HCV14002",         "",      DC_P,     "kPag",    &HCV14002           ,this  ,0);
  //DDB.Double    ("HCV14003",         "",      DC_P,     "kPag",    &HCV14003           ,this  ,0);
  //DDB.Double    ("HCV14004",         "",      DC_P,     "kPag",    &HCV14004           ,this  ,0);

  DDB.Text      (" ");
  if (SolveMode()==DYNMODE)
    {
    DDB.Object(&Contents, this, NULL, NULL, DDB_RqdPage);
//    DDB.Object(&PresetImg, this, NULL, NULL, DDB_RqdPage);
    }
  DDB.Text      (" ");

    //for (int i=0; i<NoofWells; i++)
  for (int i=0; FlowLineMap[i].iX>0;i++)
    {
    //int wn=WellDisp[i];
    //if (wn<0)
    //  continue;
    //int wi=wn-1;
    //pAWell pW=&Wells[wi];
    for (int j=0; j<2; j++)
      {
      int wn=0, cvno=0;
      if (j==0)
        {
        wn=FlowLineMap[i].iX;
        if (FlowLineMap[i].iY==0)
          cvno=FlowLineMap[i].iX;
        }
      else
        {
        wn=FlowLineMap[i].iY;
        if (FlowLineMap[i].iY>0)
          cvno=FlowLineMap[i].iX;
        }

      if (wn>0)
        {
        pAWell pW=&Wells[wn-1];

        Strng Pg;
        if (j==0)
          Pg.Set("%s/%i",FlowLineMap[i].WellName,wn);
        else
          Pg.Set("%i",wn);
        Strng SSV14xx1_Auto;  SSV14xx1_Auto.Set ("SSV14%02i1.Auto",    wn);
        Strng MSV14xx2_Auto;  MSV14xx2_Auto.Set ("MSV14%02i2.Auto",    wn);
        Strng WSV14xx3_Auto;  WSV14xx3_Auto.Set ("WSV14%02i3.Auto",    wn);
        Strng BPV14xx8_Auto;  BPV14xx8_Auto.Set ("BPV14%02i8.Auto",    wn);
        Strng HCV14xx5_Auto;  HCV14xx5_Auto.Set ("HCV14%02i5.Auto",    wn);
        Strng HCV14xx6_Auto;  HCV14xx6_Auto.Set ("HCV14%02i6.Auto",    wn);
        Strng HCV14xx7_Auto;  HCV14xx7_Auto.Set ("HCV14%02i7.Auto",    wn);
        Strng HCV14xx7_Y_Rqd; HCV14xx7_Y_Rqd.Set("HCV14%02i7.Y_Rqd",   wn);
        Strng BPV14xx1_Y_Rqd; BPV14xx1_Y_Rqd.Set("BPV14%02i1.Y_Rqd",   wn);
        Strng PI14xx3_P;      PI14xx3_P.Set     ("PI14%02i3.P",        wn);
        Strng PI14xx5_P;      PI14xx5_P.Set     ("PI14%02i5.P",        wn);
        Strng DPI14xx3_P;     DPI14xx3_P.Set    ("DPI14%02i3.P",       wn);
        Strng DPI14xx6_P;     DPI14xx6_P.Set    ("DPI14%02i6.P",       wn);
        Strng Wxx_NQv;        Wxx_NQv.Set       ("W%02i.NQv",          wn);
        Strng Wxx_FQv;        Wxx_FQv.Set       ("W%02i.FQv",          wn);
        Strng CV14xx4_Choke;  CV14xx4_Choke.Set ("CV14%02i4.Choke",    cvno);
        Strng Wxx_NQvPt;      Wxx_NQvPt.Set     ("W%02i.NQvPt",        wn);
        Strng Wxx_ChkOpnPt;   Wxx_ChkOpnPt.Set  ("W%02i.ChkOpnPt",     wn);
        Strng Wxx_ChkOpnSt;   Wxx_ChkOpnSt.Set  ("W%02i.ChkOpnSt",     wn);
        Strng Wxx_WellP;      Wxx_WellP.Set     ("W%02i.WellP",        wn);
        Strng Wxx_WellR;      Wxx_WellR.Set     ("W%02i.WellR",        wn);

        DDB.Page      (Pg(), DDB_RqdPage);
#ifndef UsingZTagObj
        DDB.VolatileTags(Pg(), Pg());
#endif
        DDB.Text(FlowLineMap[i].WellName);
        if (j==0)
          {
          DDB.Bool      (SSV14xx1_Auto(),    "",      DC_,      "",        &pW->bSSV1    ,this  ,isParm);
#ifndef UsingZTagObj
          DDB.VolatileTags(SSV14xx1_Auto(),    SSV14xx1_Auto());
#endif
          DDB.Bool      (MSV14xx2_Auto(),    "",      DC_,      "",        &pW->bMSV2    ,this  ,isParm);
#ifndef UsingZTagObj
          DDB.VolatileTags(MSV14xx2_Auto(),    MSV14xx2_Auto());
#endif
          DDB.Bool      (WSV14xx3_Auto(),    "",      DC_,      "",        &pW->bWSV3    ,this  ,isParm);
#ifndef UsingZTagObj
          DDB.VolatileTags(WSV14xx3_Auto(),    WSV14xx3_Auto());
#endif
          //DDB.Bool      (BPV14xx8_Auto(),    "",      DC_,      "",        &pW->bBPV8    ,this  ,isParm);
          //DDB.VolatileTags(BPV14xx8_Auto(),    BPV14xx8_Auto());
          }
        else
          {
          DDB.Text(" ");
          DDB.Text(" ");
          DDB.Text(" ");
          //DDB.Text(" ");
          }
        DDB.Bool      (HCV14xx5_Auto(),    "",      DC_,      "",        &pW->bHCV5    ,this  ,isParm);
#ifndef UsingZTagObj
        DDB.VolatileTags(HCV14xx5_Auto(),    HCV14xx5_Auto());
#endif
        DDB.Bool      (HCV14xx6_Auto(),    "",      DC_,      "",        &pW->bHCV6    ,this  ,isParm);
#ifndef UsingZTagObj
        DDB.VolatileTags(HCV14xx6_Auto(),    HCV14xx6_Auto());
#endif
        DDB.Bool      (HCV14xx7_Auto(),    "",      DC_,      "",        &pW->bHCV7    ,this  ,isParm);
#ifndef UsingZTagObj
        DDB.VolatileTags(HCV14xx7_Auto(),    HCV14xx7_Auto());
#endif
        DDB.Double    (HCV14xx7_Y_Rqd(),   "",      DC_Frac,  "%",       &pW->HCV7     ,this  ,isParm);
#ifndef UsingZTagObj
        DDB.VolatileTags(HCV14xx7_Y_Rqd(),   HCV14xx7_Y_Rqd());
#endif
        if (j==0)
          {
          DDB.Double    (BPV14xx1_Y_Rqd(),   "",      DC_Frac,  "%",       &pW->BPV1     ,this  ,isParm);
#ifndef UsingZTagObj
          DDB.VolatileTags(BPV14xx1_Y_Rqd(),   BPV14xx1_Y_Rqd());
#endif
          DDB.Double    (PI14xx3_P(),        "",      DC_P,     "kPag",    &pW->PI3      ,this  ,0);
#ifndef UsingZTagObj
          DDB.VolatileTags(PI14xx3_P(),        PI14xx3_P());
#endif
          }
        DDB.Double    (PI14xx5_P(),        "",      DC_P,     "kPag",    &pW->PI5      ,this  ,0);
#ifndef UsingZTagObj
        DDB.VolatileTags(PI14xx5_P(),        PI14xx5_P());
#endif
        DDB.Double    (DPI14xx3_P(),        "",     DC_DP,    "kPa",     &pW->DPI3      ,this  ,0);
#ifndef UsingZTagObj
        DDB.VolatileTags(DPI14xx3_P(),        DPI14xx3_P());
#endif
        //DDB.Double    (DPI14xx6_P(),        "",     DC_DP,    "kPa",     &pW->DPI6      ,this  ,0);
        //DDB.VolatileTags(DPI14xx6_P(),        DPI14xx6_P());
        DDB.Double    (Wxx_NQv(),          "",      DC_NQv,   "Nm^3/h",  &pW->NQv      ,this  ,0);
#ifndef UsingZTagObj
        DDB.VolatileTags(Wxx_NQv(),          Wxx_NQv());
#endif
        DDB.Double    (Wxx_FQv(),          "",      DC_NQv,   "Nm^3/h",  &pW->FQv      ,this  ,0);
#ifndef UsingZTagObj
        DDB.VolatileTags(Wxx_FQv(),          Wxx_FQv());
#endif
        if (cvno>0)
          {
          DDB.Double    (CV14xx4_Choke(),    "",      DC_Frac,  "%",       &pW->CV4      ,this  ,isParm);
#ifndef UsingZTagObj
          DDB.VolatileTags(CV14xx4_Choke(),    CV14xx4_Choke());
#endif
          }
        else
          DDB.Text(" ");
        DDB.Double    (Wxx_NQvPt(),        "",      DC_NQv,   "Nm^3/h",  &pW->NQvPt    ,this  ,isParm);
#ifndef UsingZTagObj
        DDB.VolatileTags(Wxx_NQvPt(),        Wxx_NQvPt());
#endif
        DDB.Double    (Wxx_ChkOpnPt(),     "",      DC_Frac,  "%",       &pW->ChkOpnPt ,this  ,isParm);
#ifndef UsingZTagObj
        DDB.VolatileTags(Wxx_ChkOpnPt(),     Wxx_ChkOpnPt());
#endif
        DDB.Double    (Wxx_ChkOpnSt(),     "",      DC_Frac,  "%",       &pW->ChkOpnSt ,this  ,isParm);
#ifndef UsingZTagObj
        DDB.VolatileTags(Wxx_ChkOpnSt(),     Wxx_ChkOpnSt());
#endif
        DDB.Double    (Wxx_WellP(),        "",      DC_P,     "kPag",    &pW->WellP    ,this  ,isParm);
#ifndef UsingZTagObj
        DDB.VolatileTags(Wxx_WellP(),        Wxx_WellP());
#endif
        DDB.Double    (Wxx_WellR(),        "",      DC_,      "",        &pW->WellR    ,this  ,isParm);
#ifndef UsingZTagObj
        DDB.VolatileTags(Wxx_WellR(),        Wxx_WellR());
#endif
        }
      }
    }

  DDB.EndStruct();
  }

// -------------------------------------------------------------------------

flag Sys14::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=MdlNode::ValidateData(VDB);
  double Duty=0.0;
  Contents.Model()->TPFlash(T_Rqd, NomManP, Duty);

  return OK;
  };

//--------------------------------------------------------------------------

flag Sys14::InitialiseSolution()
  {
// mhm   Class &= ~nc_FlwCon;
// mhm   Class |=  nc_Surge;
  return 1;
  };

//--------------------------------------------------------------------------

flag Sys14::TerminateSolution()
  {
  return 1;
  };

//--------------------------------------------------------------------------

void Sys14::EvalWell(rAWell Wl)
  {
  double Opn=0.0;
  double Nr=Contents.NRho(som_ALL)*0.001;

  Wl.HCV7=Range(0.0, Wl.HCV7, 1.0);

  flag WellAuto = Wl.bSSV1 && Wl.bMSV2 && Wl.bWSV3;
  int  ManAuto  = (Wl.bHCV5 || Wl.bHCV6);
  int  FlrAuto  = (Wl.HCV7>1.0e-6 && Wl.bHCV7);
  double FlrY   = Range(0.0, Wl.HCV7,1.0);
  //double ChkY;
  //flag HasCV=(Wl.iCVNo>0)
  //if (HasCV)
  double ChkY=Pow(Range(0.0, (Wl.CV4-Wl.ChkOpnSt)/GTZ(Wl.ChkOpnPt-Wl.ChkOpnSt),1.0), ChkChar);
  //else
    //ChkY=1.0;

  Wl.NQv=0.0;
  Wl.FQv=0.0;
  if (WellAuto && ManAuto && FlrAuto)
    {
    Wl.NQv=ChkY*(Wl.NQvPt-Wl.HCV7*FlrQvPt);
    Wl.FQv=ChkY*(Wl.HCV7*FlrQvPt);
    }
  else if (WellAuto && ManAuto)
    {
    Wl.NQv=ChkY*Wl.NQvPt;
    }
  else if (WellAuto && FlrAuto)
    {
    Wl.FQv=ChkY*Wl.NQvPt;
    Wl.PI5=2.0*AtmosPress()+Wl.FQv*Contents.NRho(som_ALL)*ManR+Wl.InerP;
    }
  else if (ManAuto && FlrAuto)
    {
    Wl.PI3-=0.2*(Wl.PI3-AtmosPress());
    }
  else if (!ManAuto && !WellAuto && FlrAuto)
    {
    Opn = Range(0.0, Wl.HCV7,1.0);
    Wl.NQv=Opn*FlrQvPt;
    }
  FlrNQv+=Wl.FQv;

  Wl.PI3 =Wl.WellP-(Wl.NQv+Wl.FQv)*Nr*Wl.WellR;
  //if (!Wl.bBPV8)
  //  Wl.DPI6=Wl.WellP-NomManP;

  Wl.DPI3=Wl.WellP-NomManP;
  }

//--------------------------------------------------------------------------

void Sys14::EvalWellPair(rAWell X, rAWell Y)
  {
  double Opn=0.0;
  double Nr=Contents.NRho(som_ALL)*0.001;

  X.HCV7=Range(0.0, X.HCV7, 1.0);
  Y.HCV7=Range(0.0, Y.HCV7, 1.0);

  flag WellAuto = X.bSSV1 && X.bMSV2 && X.bWSV3;
  int  XManAuto  = (X.bHCV5 || X.bHCV6);
  int  YManAuto  = (Y.bHCV5 || Y.bHCV6);
  int  XFlrAuto  = (X.HCV7>1.0e-6 && X.bHCV7);
  int  YFlrAuto  = (Y.HCV7>1.0e-6 && Y.bHCV7);
  double XFlrY   = Range(0.0, X.HCV7,1.0);
  double YFlrY   = Range(0.0, Y.HCV7,1.0);
  double XChkY=1.0;
  double YChkY=Pow(Range(0.0, (Y.CV4-Y.ChkOpnSt)/GTZ(Y.ChkOpnPt-Y.ChkOpnSt),1.0), ChkChar);
  flag XbChkY=1;
  flag YbChkY=(YChkY>Y.ChkOpnSt);

  X.NQv=0.0;
  X.FQv=0.0;
  Y.NQv=0.0;
  Y.FQv=0.0;
  if (WellAuto)
    {
    //if (XFlrAuto)
    //  {
    //  //X.NQv=XChkY*(X.NQvPt-X.HCV7*FlrQvPt);
    //  X.FQv=1.0;//XChkY*(1.0X.HCV7*FlrQvPt);
    //  }
    //else
    if (XManAuto)
      X.NQv=XChkY;//*X.NQvPt;
    //else if (XFlrAuto)
    //  {
    //  X.FQv=XChkY*X.NQvPt;
    //  X.PI5=2.0*AtmosPress()+X.FQv*Contents.NRho(som_ALL)*ManR+X.InerP;
    //  }
    //if (YManAuto)
    //  {
    //  Y.NQv=YChkY;//*(Y.NQvPtY.HCV7*FlrQvPt);
    // / //Y.FQv=YChkY*(Y.HCV7*FlrQvPt);
    //  }
    //else
    if (YFlrAuto)
      Y.FQv=1.0;//XChkY*(1.0X.HCV7*FlrQvPt);
    else if (YManAuto)
      Y.NQv=YChkY;//*Y.NQvPt;
    //else if (YFlrAuto)
    // / {
    //  Y.FQv=YChkY*Y.NQvPt;
    //  Y.PI5=2.0*AtmosPress()+Y.FQv*Contents.NRho(som_ALL)*ManR+Y.InerP;
    //  }
    }
  else
    {
    if (XManAuto && XFlrAuto)
      X.PI3-=0.2*(X.PI3-AtmosPress());
    else if (!XManAuto && XFlrAuto)
      {
      Opn = Range(0.0, X.HCV7,1.0);
      X.NQv=Opn*FlrQvPt;
      }
    }

  if (Y.FQv>1.0e-3)
    {
    Y.FQv*=X.NQvPt;
    FlrNQv+=Y.FQv;
    X.NQv=0.0;
    Y.NQv=0.0;
    }
  else
    {
    double SumFrac=X.NQv+Y.NQv;
    X.NQv*=X.NQvPt/Max(1.0, SumFrac);
    Y.NQv*=X.NQvPt/Max(1.0, SumFrac);
    }

  //X.PI3 =X.WellP-(X.NQv+X.FQv)*Nr*X.WellR;
  //Y.PI3 =X.WellP-(Y.NQv+Y.FQv)*Nr*Y.WellR;
  X.PI3 =X.WellP-(X.NQv+X.FQv+Y.NQv+Y.FQv)*Nr*X.WellR;

  //if (!X.bBPV8)
  //  X.DPI6=X.WellP-NomManP;
  //if (!Y.bBPV8)
  //  Y.DPI6=Y.WellP-NomManP;

  //X.DPI3=X.WellP-NomManP;

  if (WellAuto)
    {
    if (!X.bHCV6 && !Y.bHCV6)
      {
      X.PI5 = X.WellP;
      Y.PI5 = X.WellP;
      }
    else
      {
      X.PI5 = NomManP+0.02*(X.PI3-NomManP)*XChkY;
      Y.PI5 = NomManP+0.02*(X.PI3-NomManP)*YChkY;
      }
    }
  else
    {
    if ((X.BPV1>0.0001) && !X.bHCV6 && !Y.bHCV6)
      {
      X.PI5 += (X.PI3-X.PI5)*Range(0.0, X.BPV1, 1.0)/30.0;
      if (YbChkY)
        Y.PI5 = X.PI5;
      }
    else
      {
      if (X.bHCV6)
        {
        X.PI5 = NomManP;
        if (YbChkY && !Y.bHCV6)
          Y.PI5 = X.PI5;
        }
      if (Y.bHCV6)
        {
        Y.PI5 = NomManP; //
        if (YbChkY && !X.bHCV6)
          X.PI5 = Y.PI5;
        }
//      if (Y.bHCV6)
//        Y.PI5 = NomManP; //
//      else if (XbChkY && !X.bHCV6)
//        X.PI5 = NomManP;
      }

    //else if (FriendHasCV4 && FriendCV4Open)
    //  Mn.PI5 = Friend.PI5; //
    }
  X.DPI3=X.PI3-X.PI5;

  };

//--------------------------------------------------------------------------

void Sys14::EvalJoinPressures()
  {
  double Duty=0.0;
  Contents.Model()->TPFlash(T_Rqd, NomManP, Duty);

  FlrNQv=0.0;
  for (int i=0; FlowLineMap[i].iX>0;i++)
    if (FlowLineMap[i].iY>0)
      EvalWellPair(Wells[FlowLineMap[i].iX-1], Wells[FlowLineMap[i].iY-1]);
    else
      EvalWell(Wells[FlowLineMap[i].iX-1]);

  /**
  double Nr=Contents.NRho(som_ALL)*0.001;

  for (word iw=0; iw<NoofWells; iw++)
    {
    int wn=WellDisp[iw];
    if (wn<0)
      continue;
    int wi=wn-1;
    rAWell Mn=Wells[wi];
    rAWell Wl=Wells[FlowLineMap[wi].iWellNo-1];
    double Opn=0.0;

    Mn.HCV7=Range(0.0, Mn.HCV7, 1.0);

    flag WellAuto = Wl.bSSV1 && Wl.bMSV2 && Wl.bWSV3;
    int  ManAuto  = (Mn.bHCV5 || Mn.bHCV6);
    int  FlrAuto  = (Mn.HCV7>1.0e-6 && Mn.bHCV7);
    double FlrY   = Range(0.0, Mn.HCV7,1.0);
    double ChkY;
    flag HasCV=(FlowLineMap[wi].iCVNo>0);
    if (HasCV)
      ChkY=Pow(Range(0.0, (Mn.CV4-Mn.ChkOpnSt)/GTZ(Mn.ChkOpnPt-Mn.ChkOpnSt),1.0), ChkChar);
    else
      ChkY=1.0;

    Mn.NQv=0.0;
    Mn.FQv=0.0;
    if (WellAuto && ManAuto && FlrAuto)
      {
      Mn.NQv=ChkY*(Mn.NQvPt-Mn.HCV7*FlrQvPt);
      Mn.FQv=ChkY*(Mn.HCV7*FlrQvPt);
      }
    else if (WellAuto && ManAuto)
      {
      Mn.NQv=ChkY*Mn.NQvPt;
      }
    else if (WellAuto && FlrAuto)
      {
      Mn.FQv=ChkY*Mn.NQvPt;
      Mn.PI5=2.0*AtmosPress()+Mn.FQv*Contents.NRho(som_ALL)*ManR+Mn.InerP;
      }
    else if (ManAuto && FlrAuto)
      {
      Mn.PI3-=0.2*(Mn.PI3-AtmosPress());
      }
    else if (!ManAuto && !WellAuto && FlrAuto)
      {
      Opn = Range(0.0, Mn.HCV7,1.0);
      Mn.NQv=Opn*FlrQvPt;
      }
    FlrNQv+=Mn.FQv;

    Mn.PI3 =Wl.WellP-(Mn.NQv+Mn.FQv)*Nr*Wl.WellR;
    if (!Wl.bBPV8)
      Wl.DPI6=Wl.WellP-NomManP;



    Wl.DPI3=Wl.WellP-NomManP;

    if (!WellAuto)
      {
      if (Mn.bHCV6)
        Mn.PI5 = NomManP;
      else if (HasCV4 && CV4Open)
        Mn.PI5 = Friend.PI5; //
      else if (FriendHasCV4 && FriendCV4Open)
        Mn.PI5 = Friend.PI5; //
      }
    }
  **/
  double Pm = NomManP;//20000.0 + AtmosPress();
  for (i = 0; i < NoFlwIOs(); i++)
      Set_IOP_Self(i,Pm);
  };

//--------------------------------------------------------------------------

//flag Sys14::EvalFlowEquations(CSpPropInfo *pProps, int iJoinId, int FE)
//  {
//  return False;
//  };

// --------------------------------------------------------------------------

void Sys14::ConfigureJoins()
  {
  for (int i=0; i<NoFlwIOs(); i++)
    SetIO_Open(i, 0, false, ESS_Denied);
  };

//--------------------------------------------------------------------------

flag Sys14::EvalFlowEquations(CSpPropInfo *pProps, int IONo, int FE)
  {
  double Qm=0.0;
  double Nr=Contents.NRho(som_ALL);
  for (word w=0; w<NoofWells; w++)
    {
    rAWell W=Wells[w];
    if ((IONo==0 ? W.bHCV5 : W.bHCV6)!=0)
      {
      Qm+=W.NQv;     // NB 1 Nm^3/sec approx= 1 kg/s

      //W.PI5=IOP_Rmt(IONo)+(W.NQv+W.FQv)*Nr*ManR+W.InerP;
      //if (W.NQv > 1.0e-6)
      //  dbgpln("Well %2i : Qv:%12.4g P3:%12.4g P5:%12.4g IP:%12.4g",w,W.NQv,W.PI3,W.PI5,W.InerP);
//      NomManP=IOP_Flng(IONo);
      }
    }

  pFlwBlk pC = IOFB(IONo, FE);
  pC->SetQmReqd(-Qm);

  return True;
  }

// --------------------------------------------------------------------------

void Sys14::EvalProducts()
  {
  if (Valid(T_Rqd))
    Contents.SetTemp(T_Rqd);
  for (int i = 0; i < NoFlwIOs(); i++)
    {
    rSpConduit C = *IOConduit(i);
    double Qo = IOQm_In(i);
    if (FlwZero(Qo))
      Qo=0.0;
    if (Qo < 0.0)
      C.QSetM(Contents, /*False,*/ som_ALL, -Qo, IOP_Self(i));
    else
      C.QZero();

    if (0)//Debug)
      dbgpln("Flows Sys14 Out    : %14.6g, %14.6g | %14.6g         %s", (*IOConduit(i)).QMass(som_SL), (*IOConduit(i)).QMass(som_Vap), K_2_C(C.Temp()), FullTag());
    }
  };

// --------------------------------------------------------------------------

void Sys14::EvalDerivs()
  {
  if (Valid(T_Rqd))
    Contents.SetTemp(T_Rqd);
  //Contents.ZeroDeriv();

  int ti = IOWithId_Self(0);
  if (ti>=0)
    {
    TI201 = IOConduit(ti)->Temp();
    PI201 = IOP_Rmt(ti);
    }
  int pi = IOWithId_Self(1);
  if (pi>=0)
    {
    TI202 = IOConduit(pi)->Temp();
    PI203 = IOP_Rmt(pi);
    }
  };

// --------------------------------------------------------------------------

void Sys14::EvalDiscrete()
  {
  double Nr=Contents.NRho(som_ALL);
  for (word w=0; w<NoofWells; w++)
    {
    rAWell W=Wells[w];
    double NQv = W.NQv+W.FQv;
    double d   = NQv-W.pNQv;
    W.InerP *= exp(-Min(ICGetTimeInc()/Max(0.01, InertiaTau),78.0));
    W.InerP += d*Nr*InertiaScl;
    W.pNQv=NQv;
    //W.DPI6 *=  exp(-Min(ICGetTimeInc()/Max(0.01, DP6Tau),78.0));
    }

  };

//==========================================================================
