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
#include "mg_sys13.h"
#include "debugnew.h"

//=========================================================================

#define dbgsys13         0x0


SPECIEBLK_L(TEG,       "TEG"   , false  );
SPECIEBLK_L(H2O,       "H2O"   , false  );

const byte   idGr = 0;
const byte   idGl = 1;

long Drw_System13[] = {DD_Poly, -5,5, -5,-5, 5,-5, 5,5, -5,5, DD_End};

IMPLEMENT_MODELUNIT(Sys13, "System13", "", Drw_System13, "S13", TOC_ALL|TOC_GRP_ENERGY|TOC_USER,
                    "System13",
                    "System13")

static IOAreaRec S13IOAreaList[] =
  {{"Gr",       "Gr"        , idGr,   LIO_In ,  nc_MLnk, 1, 1, 0, 0.0f, ""},
   {"Gl",       "Gl"        , idGl,   LIO_Out,  nc_MLnk, 1, 1, 0, 0.0f, ""},
   {NULL}};

// -------------------------------------------------------------------------

Sys13::Sys13(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MdlNode(pClass_, TagIn, pAttach, eAttach),
  VH1301  ("VH1301", this, TOA_Embedded),
  VH1302  ("VH1302", this, TOA_Embedded),
  VGA1301 ("VGA1301", this, TOA_Embedded),
  VBA1301 ("VBA1301", this, TOA_Embedded),
  HBA1301 ("HBA1301", this, TOA_Embedded),
  TEGStore("TEGStore", this, TOA_Embedded),
  Q1("Q1", this, TOA_Embedded),
  Q2("Q2", this, TOA_Embedded),
  Q3("Q3", this, TOA_Embedded),
  Q4("Q4", this, TOA_Embedded),
  Q5("Q5", this, TOA_Embedded),
  Q6("Q6", this, TOA_Embedded),
  Q7("Q7", this, TOA_Embedded),
  Q8("Q8", this, TOA_Embedded),
  Q9("Q9", this, TOA_Embedded),
  Qa("Qa", this, TOA_Embedded),
  Qb("Qb", this, TOA_Embedded),
  Qc("Qc", this, TOA_Embedded)
  {
  AttachClassInfo(nc_Process, S13IOAreaList);
  AllDataHere = 1;

  VH1301.SetVolume(2.6);
  VH1302.SetVolume(2.6);
  VGA1301.SetVolume(1.0);
  VBA1301.SetVolume(26.0);
  HBA1301.SetVolume(2.0);
  TEGStore.SetVolume(2.0);

  VH1301.SetStateAction(IE_Integrate);
  VH1302.SetStateAction(IE_Integrate);
  VGA1301.SetStateAction(IE_Integrate);
  VBA1301.SetStateAction(IE_Integrate);
  HBA1301.SetStateAction(IE_Integrate);
  TEGStore.SetStateAction(IE_Integrate);

  Q1m=0.0;
  Q2m=0.0;
  Q3m=0.0;
  Q4m=0.0;
  Q5m=0.0;
  Q6m=0.0;
  Q7m=0.0;
  Q8m=0.0;
  Q9m=0.0;
  Qam=0.0;
  Qbm=0.0;
  Qcm=0.0;

  bLC13111=0;
  bLC13114=0;              bLC13117=0;
  bLC13120=0;
  bPKA1301B=0;
  bPKA1301A=0;
  bPKA1301B=0;
  bPK1302A=0;
  bPK1302B=0;
  bUHA1301A=0;
  bUHA1301B=0;
  VH1301_Lvl=0.5;
  VH1301_P=AtmosPress()+300.0;
  VH1302_Lvl=0.5;
  VH1302_P=AtmosPress()+300.0;
  VGA1301_LvlSet=0.55;
  VGA1301_Lvl=0.5;
  VGA1301_P=AtmosPress()+300.0;
  HBA1301_LvlSet=0.5;
  HBA1301_Lvl=0.5;
  HBA1301_P=AtmosPress()+300.0;
  VBA1301_Lvl=0.5;

  VGA1301_T=C_2_K(100.0);
  HBA1301_T=C_2_K(100);
  VBA1301_T=C_2_K(20.0);
  VH1301_T=C_2_K(20.0);
  VH1302_T=C_2_K(20.0);

  //VB1001_LvlSet=0.65; //  changed for external controller
  //VB1001_Lvl=0.7;     // mhm comeback - this level has to come from the glycol contacter
  //FEA13050_Qm=0.0;
  QmBoil=1.0;
  QmRich=0.0;
  QmLean=0.0;
  QmCirc=1.0;
  QmXfer=1.0;
  QmFill=1.0;
  QmSetl=10.0;

  VLVin_Posn = 1.0;


  TEGStore.ZeroMass();
  TEGStore.SetSpMass(TEG.li(), 1.0);
  TEGStore.SetTemp(C_2_K(20.0));

  };

//--------------------------------------------------------------------------

void Sys13::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);

  DDB.Page("Page 1");
  DDB.Bool      ("PKA1301A.Auto",    "",      DC_,      "",       &bPKA1301A     ,this  ,isParm);
  DDB.Bool      ("PKA1301B.Auto",    "",      DC_,      "",       &bPKA1301B     ,this  ,isParm);
  DDB.Bool      ("UHA1301A.Auto",    "",      DC_,      "",       &bUHA1301A     ,this  ,isParm);
  DDB.Bool      ("UHA1301B.Auto",    "",      DC_,      "",       &bUHA1301B     ,this  ,isParm);
  DDB.Text(" ");
  DDB.Double    ("VLVin_Posn",       "",      DC_Frac,  "%",      &VLVin_Posn    ,this  ,isParm);
  DDB.Text(" ");
  DDB.Double    ("VGA1301.LvlSet",   "",      DC_Frac,  "%",      &VGA1301_LvlSet,this  ,isParm);
  DDB.Double    ("VGA1301.Lvl",      "",      DC_Frac,  "%",      &VGA1301_Lvl   ,this  ,0);
  DDB.Double    ("VGA1301.P",        "",      DC_P,     "kPa",    &VGA1301_P     ,this  ,0);
  DDB.Double    ("VGA1301.T",        "",      DC_T,     "C",      &VGA1301_T     ,this  ,0);
  DDB.Text(" ");
  DDB.Double    ("HBA1301.LvlSet",   "",      DC_Frac,  "%",      &HBA1301_LvlSet,this  ,isParm);
  DDB.Double    ("HBA1301.Lvl",      "",      DC_Frac,  "%",      &HBA1301_Lvl   ,this  ,0);
  DDB.Double    ("HBA1301.P",        "",      DC_P,     "kPa",    &HBA1301_P     ,this  ,0);
  DDB.Double    ("HBA1301.T",        "",      DC_T,     "C",      &HBA1301_T     ,this  ,0);
  DDB.Text(" ");
  DDB.Double    ("VBA1301.Lvl",      "",      DC_Frac,  "%",      &VBA1301_Lvl   ,this  ,0);
  DDB.Double    ("VBA1301.T",        "",      DC_T,     "C",      &VBA1301_T     ,this  ,0);
  DDB.Text(" ");
  //DDB.Double    ("VB1001.LvlSet",    "",      DC_Frac,  "%",      &VB1001_LvlSet ,this  ,isParm);
  //DDB.Double    ("VB1001.Lvl",       "",      DC_Frac,  "%",      &VB1001_Lvl    ,this  ,0);
  //DDB.Double    ("FEA13050.Qm",      "",      DC_Qm,    "kg/s",   &FEA13050_Qm   ,this  ,0);
  DDB.Text(" ");
  DDB.Double    ("QmBoil",           "",      DC_Qm,    "kg/s",   &QmBoil        ,this  ,isParm);
  DDB.Double    ("QmCirc",           "",      DC_Qm,    "kg/s",   &QmCirc        ,this  ,isParm);
  DDB.Double    ("QmXfer",           "",      DC_Qm,    "kg/s",   &QmXfer        ,this  ,isParm);
  DDB.Double    ("QmFill",           "",      DC_Qm,    "kg/s",   &QmFill        ,this  ,isParm);
  DDB.Double    ("QmSetl",           "",      DC_Qm,    "kg/s",   &QmSetl        ,this  ,isParm);
  DDB.Double    ("QmRich",           "",      DC_Qm,    "kg/s",   &QmRich        ,this  ,0);
  DDB.Double    ("QmLean",           "",      DC_Qm,    "kg/s",   &QmLean        ,this  ,0);
  DDB.Text(" ");
  DDB.Double    ("Q1m",              "",      DC_Qm,    "kg/s",   &Q1m           ,this  ,0);
  DDB.Double    ("Q2m",              "",      DC_Qm,    "kg/s",   &Q2m           ,this  ,0);
  DDB.Double    ("Q3m",              "",      DC_Qm,    "kg/s",   &Q3m           ,this  ,0);
  DDB.Double    ("Q4m",              "",      DC_Qm,    "kg/s",   &Q4m           ,this  ,0);
  DDB.Double    ("Q5m",              "",      DC_Qm,    "kg/s",   &Q5m           ,this  ,0);
  DDB.Double    ("Q6m",              "",      DC_Qm,    "kg/s",   &Q6m           ,this  ,0);
  DDB.Double    ("Q7m",              "",      DC_Qm,    "kg/s",   &Q7m           ,this  ,0);
  DDB.Text(" ");
  DDB.Page("Page 2");
  DDB.Bool      ("LC13111.Auto",     "",      DC_,      "",       &bLC13111     ,this   ,isParm);
  DDB.Bool      ("LC13114.Auto",     "",      DC_,      "",       &bLC13114     ,this   ,isParm);
  DDB.Bool      ("LC13117.Auto",     "",      DC_,      "",       &bLC13117     ,this   ,isParm);
  DDB.Bool      ("LC13120.Auto",     "",      DC_,      "",       &bLC13120     ,this   ,isParm);
  DDB.Bool      ("PK1302A.Auto",     "",      DC_,      "",       &bPK1302A     ,this   ,isParm);
  DDB.Bool      ("PK1302B.Auto",     "",      DC_,      "",       &bPK1302B     ,this   ,isParm);
  DDB.Text(" ");
  DDB.Text(" ");
  DDB.Double    ("VH1301.Lvl",       "",      DC_Frac,  "%",      &VH1301_Lvl   ,this   ,0);
  DDB.Double    ("VH1301.T",         "",      DC_T,     "C",      &VH1301_T     ,this   ,0);
  DDB.Double    ("VH1302.Lvl",       "",      DC_Frac,  "%",      &VH1302_Lvl   ,this   ,0);
  DDB.Double    ("VH1302.T",         "",      DC_T,     "C",      &VH1302_T     ,this   ,0);
  DDB.Text(" ");
  DDB.Double    ("Q8m",              "",      DC_Qm,    "kg/s",   &Q8m          ,this   ,0);
  DDB.Double    ("Q9m",              "",      DC_Qm,    "kg/s",   &Q9m          ,this   ,0);
  DDB.Double    ("Qam",              "",      DC_Qm,    "kg/s",   &Qam          ,this   ,0);
  DDB.Double    ("Qbm",              "",      DC_Qm,    "kg/s",   &Qbm          ,this   ,0);
  DDB.Double    ("Qcm",              "",      DC_Qm,    "kg/s",   &Qcm          ,this   ,0);
  DDB.Text(" ");

//  if (PrjFileVerNo()<24)
//    {
    VGA1301.BuildDataDefn(DDB);
    HBA1301.BuildDataDefn(DDB);
    VBA1301.BuildDataDefn(DDB);
    VH1301.BuildDataDefn(DDB);
    VH1302.BuildDataDefn(DDB);
    TEGStore.BuildDataDefn(DDB);
//    }
//  else
//    {
//    DDB.Object(&VGA1301 , this, NULL, NULL, DDB_OptPage);
//    DDB.Object(&HBA1301 , this, NULL, NULL, DDB_OptPage);
//    DDB.Object(&VBA1301 , this, NULL, NULL, DDB_OptPage);
//    DDB.Object(&VH1301  , this, NULL, NULL, DDB_OptPage);
//    DDB.Object(&VH1302  , this, NULL, NULL, DDB_OptPage);
//    DDB.Object(&TEGStore, this, NULL, NULL, DDB_OptPage);
//    }

  DDB.EndStruct();
  };
//--------------------------------------------------------------------------

flag Sys13::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=MdlNode::ValidateData(VDB);
  VGA1301_T=(VGA1301_T < C_2_K(-20.0) || VGA1301_T > C_2_K(200.0)) ? C_2_K(20.0) : VGA1301_T;
  HBA1301_T=(HBA1301_T < C_2_K(-20.0) || HBA1301_T > C_2_K(200.0)) ? C_2_K(20.0) : HBA1301_T;
  VBA1301_T=(VBA1301_T < C_2_K(-20.0) || VBA1301_T > C_2_K(200.0)) ? C_2_K(20.0) : VBA1301_T;
  VH1301_T =(VH1301_T  < C_2_K(-20.0) || VH1301_T  > C_2_K(200.0)) ? C_2_K(20.0) : VH1301_T;
  VH1302_T =(VH1302_T  < C_2_K(-20.0) || VH1302_T  > C_2_K(200.0)) ? C_2_K(20.0) : VH1302_T;

  VGA1301.SetTemp(VGA1301_T);
  HBA1301.SetTemp(HBA1301_T);
  VBA1301.SetTemp(VBA1301_T);
  VH1301.SetTemp(VH1301_T);
  VH1302.SetTemp(VH1302_T);
  return OK;
  };

//--------------------------------------------------------------------------

void Sys13::EvalJoinPressures()
  {
  double Pm = AtmosPress();
  for (int i = 0; i < NoFlwIOs(); i++)
      Set_IOP_Self(i,Pm);
  //TRACE("MHM CHECK WITH CNM");
  }


//--------------------------------------------------------------------------

void Sys13::ConfigureJoins()
  {
  for (int i=0; i<NoFlwIOs(); i++)
    SetIO_Open(i, 0, false, ESS_Denied);
  };

// --------------------------------------------------------------------------

flag Sys13::EvalFlowEquations(CSpPropInfo *pProps, int IONo, int FE)
  {
  double Qm=0.0;
  pFlwBlk pC = IOFB(IONo, FE);
  if (IOId_Self(IONo) == idGr )      // Rich Glycol In
    {
    //mhm change to enable a controller to be used
    //Qm = Max(0.0, VB1001_Lvl-VB1001_LvlSet)*QmSetl*VLVin_Posn;
    Qm = Max(0.0,QmSetl*VLVin_Posn);
    QmRich=Qm;
    pC->SetQmReqd(Qm); // ie In
    }
  else if (IOId_Self(IONo) == idGl ) // Lean Glycol Out
    {
    Qm = (bPKA1301A ? QmCirc : 0.0) + (bPKA1301B ? QmCirc: 0.0);
    QmLean=Qm;
    pC->SetQmReqd(-Qm); // ie Out
    }
  pC->SetDPq(0.0,0.0);
  return True;
  };

// --------------------------------------------------------------------------

void Sys13::EvalProducts()
  {

  Q6.QSetM(VBA1301,/*False,*/som_ALL, QmLean, VBA1301.Press());
  Q7.QCopy(Q6, VBA1301.Press());
  if ((NoFlwIOs() > 1) && (IOId_Self(1)==idGl))      // Lean Glycol Out // mhm to check the numbers here
     IOConduit(idGl)->QCopy(Q7, VBA1301.Press());
//mhm     IOFlange(1)->Q.QCopy(Q7, VBA1301.P()); // mhm
  };

// --------------------------------------------------------------------------

void Sys13::EvalDerivs()
  {
  VGA1301_Lvl=VGA1301.Level();
  HBA1301_Lvl=HBA1301.Level();
  VBA1301_Lvl=VBA1301.Level();
  VH1301_Lvl=VH1301.Level();
  VH1302_Lvl=VH1302.Level();
  VGA1301_T=VGA1301.Temp();
  HBA1301_T=HBA1301.Temp();
  VBA1301_T=VBA1301.Temp();
  VH1301_T=VH1301.Temp();
  VH1302_T=VH1302.Temp();

  double Qm;
  double P = Std_P;

  if ((NoFlwIOs() > 0) && (IOId_Self(0)==0))  // Rich Glycol In
    {
    Q1.QCopy(*IOConduit(idGr), P);

//mhm    Q1.QCopy(IOFlange(0)->Q, P);
    }
  else
    {
    Q1.QZero();
    }
  Q2.QCopy(Q1, P);
  Q3.QSetM(VGA1301,/*False, */som_ALL, Max(0.0, VGA1301_Lvl-VGA1301_LvlSet)*QmSetl, P);
  Q4.QCopy(Q3, P);
  Q5.QSetM(HBA1301,/*False, */som_ALL, Max(0.0, HBA1301_Lvl-HBA1301_LvlSet)*QmSetl, P);
  Q8.QSetM(TEGStore,/*False, */som_ALL, bLC13111 ? QmFill : 0.0, P);
  Q9.QSetM(TEGStore,/*False, */som_ALL, bLC13117 ? QmFill : 0.0, P);
  Qm=(bPK1302A ? QmXfer : 0.0) + (bPK1302B ? QmXfer : 0.0);
  if (bLC13114 && bLC13120)
    Qm/=2.0;
  Qa.QSetM(VH1301,/*False, */som_ALL, (bLC13114 ? Qm : 0.0), P);
  Qb.QSetM(VH1302,/*False, */som_ALL, (bLC13120 ? Qm : 0.0), P);
  Qc.QCopy(Qa, P);
  Qc.QAddF(Qb, som_ALL, 1.0);

  Q1m=Q1.QMass(som_ALL);
  Q2m=Q2.QMass(som_ALL);
  Q3m=Q3.QMass(som_ALL);
  Q4m=Q4.QMass(som_ALL);
  Q5m=Q5.QMass(som_ALL);
  Q6m=Q6.QMass(som_ALL);
  Q7m=Q7.QMass(som_ALL);
  Q8m=Q8.QMass(som_ALL);
  Q9m=Q9.QMass(som_ALL);
  Qam=Qa.QMass(som_ALL);
  Qbm=Qb.QMass(som_ALL);
  Qcm=Qc.QMass(som_ALL);

  #if dbgsys13
    dbgpln("System 13 - VGA1301_Lvl : %8.2f",VGA1301_Lvl);
    dbgpln("System 13 - HBA1301_Lvl : %8.2f",HBA1301_Lvl);
    dbgpln("System 13 - VBA1301_Lvl : %8.2f",VBA1301_Lvl);
    dbgpln("System 13 - VH1301_Lvl  : %8.2f",VH1301_Lvl );
    dbgpln("System 13 - VH1302_Lvl  : %8.2f",VH1302_Lvl );
    dbgpln("System 13 - Q1    : %10.2f",Q1m);
    dbgpln("System 13 - Q2    : %10.2f",Q2m);
    dbgpln("System 13 - Q3    : %10.2f",Q3m);
    dbgpln("System 13 - Q4    : %10.2f",Q4m);
    dbgpln("System 13 - Q5    : %10.2f",Q5m);
    dbgpln("System 13 - Q6    : %10.2f",Q6m);
    dbgpln("System 13 - Q7    : %10.2f",Q7m);
    dbgpln("System 13 - Q8    : %10.2f",Q8m);
    dbgpln("System 13 - Q9    : %10.2f",Q9m);
    dbgpln("System 13 - Qa    : %10.2f",Qam);
    dbgpln("System 13 - Qb    : %10.2f",Qbm);
    dbgpln("System 13 - Qc    : %10.2f",Qcm);
  #endif

  SpConduit QBoil("QBoil",NULL,TOA_Free);
  double qb=((bUHA1301A ? 1 : 0) + (bUHA1301B ? 1 : 0))*QmBoil;
  QBoil.SetQm(H2O.li(),Min(qb, HBA1301.SpMass(H2O.li())/10.0));
  //mhm QBoil.Qm(H2O.li())=Min(qb, HBA1301.SpM(H2O.li())/10.0);
  QBoil.SetTemp(HBA1301.Temp());

  VGA1301.ZeroDeriv();
  VGA1301.AddDeriv(Q2, 1);
  VGA1301.AddDeriv(Q3, -1);

  HBA1301.ZeroDeriv();
  HBA1301.AddDeriv(Q4, 1);
//  HBA1301.AddDeriv(Qc, 1, "+Qc");
// Qc is the sum of the glycol out of the VH1301 and VH1302 and according to the flowsheet should be added to VBA1301



  HBA1301.AddDeriv(Q5,    -1);
  HBA1301.AddDeriv(QBoil, -1);

  VBA1301.ZeroDeriv();
  VBA1301.AddDeriv(Q5,  1);
  VBA1301.AddDeriv(Q6, -1);
  VBA1301.AddDeriv(Qc,  1);

  VH1301.ZeroDeriv();
  VH1301.AddDeriv(Q8, 1);
  VH1301.AddDeriv(Qa, -1);

  VH1302.ZeroDeriv();
  VH1302.AddDeriv(Q9, 1);
  VH1302.AddDeriv(Qb, -1);

  };

// --------------------------------------------------------------------------

void Sys13::EvalDiscrete()
  {
  //VB1001_Lvl = 0.7; // mhm hardwireed for now
//mhm  VB1001_Lvl = Range((double)0.0, ContactLvl.GetFloat()*0.01, (double)1.0);
//mhm  commented out due to RemoteData

  };

// --------------------------------------------------------------------------

void Sys13::ApplyDerivs(double dTime, flag DoDbg)
  {
  VGA1301.ApplyDerivs(dTime, true);
  HBA1301.ApplyDerivs(dTime, true);
  VBA1301.ApplyDerivs(dTime, true);
  VH1301.ApplyDerivs(dTime, true);
  VH1302.ApplyDerivs(dTime, true);

  }

//=========================================================================
