//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#define  __BELTCNV2_CPP
#include "beltcnv2.h"
//#include "optoff.h"

//==========================================================================
/*#D:#T:Belt Conveyor
#X:#h<General Description>#n
This model is used to simulate a Belt conveyor, with the associated time delays
and mass carrying limitations.  The conveyor may have more than one feed point and
more than one take-off.
#n#n
#n#h<Inputs and Outputs>#w
Label    Input/Output  Minimum number  Maximum number#n
I            Input          1              20#n
O            Output         1              20#n
#a#n
#h<Variables to be supplied by the user>#n
#i<BeltSpeed> : The actual speed of the unit.#n
#i<Datum_Rqd> : The datum of the conveyor with respect to the common datum of the
system.#n
#i<Length> : The length of the belt conveyor.#n
#i<MaxSpeed> : The maximum speed of the belt conveyor.#n
#i<MaxLoading> : The maximum mass loading of the belt conveyor before the material
is spilled.#n
#i<MaxStallLoad> : The maximum mass loading of the belt conveyor before the unit stalls.#n
#i<StartIfStalled> : The user may enable the belt to start after it has stalled.#n
#i<SpillArea> : The unit to which the spilled material should report.#n
#i<NoLoadPower> : The power drawn by the conveyor in the unloaded state.#n
#i<FullLoadPower> : The power drawn by the conveyor in the fully loaded state.#n
Each feed and removal point on the conveyor has the following variables:#n
#i<Pos> : The feed position along the conveyor.#n
#n#n
#h<Variables calculated by the Conveyor>#n
#i<Datum> : The actual datum of the belt conveyor.#n
#i<Power> : The power drawn by the belt conveyor.#n
#i<NSections> : The number of sections into which the belt conveyor is divided for the purpose
of the simulation.#n

#n#h<Other>#n
Default model prefix:BC#n
Short name:BeltCnv#n
Model type:Unit#n
#G:Units
*/
//==========================================================================

const byte idFeed       = FlwIOId(0);
const byte idProd       = FlwIOId(1);
const int EIOId_Supply  = ElecIOId(0);

XID xidBeltLength       = ModelXID(1000);
XID xidBeltSpeed        = ModelXID(1001);
XID xidBeltStopped      = ModelXID(1002);
XID xidTotSpilt         = ModelXID(1003);
XID xidNWtMtrs          = ModelXID(1004);
XID xidNSections        = ModelXID(1005);
XID xidTotalMass        = ModelXID(1006);
XID xidAvgLoading       = ModelXID(1007);
XID xidShowProf         = ModelXID(1008);
XID xidProfPts          = ModelXID(1009);
XID xidProfDispType     = ModelXID(1010);
XID xidFeedType0        = ModelXID(1100);
XID xidFeedIndex0       = ModelXID(1200);
XID xidFeedPos0         = ModelXID(1300);
XID xidFeedQm0          = ModelXID(1400);
XID xidFeedLimited0     = ModelXID(1500);
XID xidFeedCapFrac0     = ModelXID(1600);
XID xidProdType0        = ModelXID(1700);
XID xidProdIndex0       = ModelXID(1800);
XID xidProdPos0         = ModelXID(1900);
XID xidProdRemove0      = ModelXID(2000);
XID xidProdQm0          = ModelXID(2100);
XID xidProdLoss0        = ModelXID(2200);
XID xidProdBinWidth0    = ModelXID(2300);
XID xidProdSpillMargin0 = ModelXID(2400);
XID xidWtrMtrPos0       = ModelXID(3000);
XID xidWtrMtrLd0        = ModelXID(3100);
XID xidWtrMtrRate0      = ModelXID(3200);
XID xidProdDrvCount     = ModelXID(4000);
XID xidFeedDrvCount     = ModelXID(4001);
XID xidTripperOn        = ModelXID(4002);

XID xidBalance          = ModelXID(4003);

static IOAreaRec BeltCnvIOAreaList[] =
  {{"Feeds",        "Feed"     , idFeed, LIO_In0 ,    nc_MLnk, 0, 20, IOSetXfer|IOGRP(0)},
   {"Products",     "Prod"     , idProd, LIO_Out0,    nc_MLnk, 0, 20, IOSetXfer|IOGRP(0)},
   SPILL2AREA("Spills", IOId_Spill2Area),
   VENT2AREA("Vents",   IOId_Vent2Area),
   {"ElecSupply",    "ElecIn",      EIOId_Supply, LIO_In , nc_ELnk, 0,50},
   {NULL}}; //This lists the areas of the model where links can be attached.


static double Drw_BeltCnv[] = { DD_Poly, -20,2, -20,-2, -18,-2, -18,0, 18,0, 18,-2, 20,-2, 20,2, -20,2,
                              DD_End }; //This provides the default drawing of the BeltCnv.
//--------------------------------------------------------------------------

//==========================================================================
//
//
//
//==========================================================================

CBeltFdPrDrive::CBeltFdPrDrive(BeltCnv2 * pBelt) :
  m_pBelt(pBelt),
  m_Pwr("Supply", pBelt, TOA_Embedded),
  m_IOPtSB("PL_FwdRv", m_pBelt, &m_Pwr)
  {
  m_Pwr.Initialise("525v_A/C");
  m_lCtrl=BPC_None;
  m_dPosnReqd=dNAN;
  m_dPosnActual=0;
  m_dMaxSpeed=1;                               
  m_dPosnMin=0;
  m_dPosnMax=m_pBelt->m_Q.Length();
  };

// -------------------------------------------------------------------------

void CBeltFdPrDrive::Advance(double TimeStep)
  {
  m_dPosnStart=m_dPosnActual;
  const double SpdFrac = m_IOPtSB.Speed(m_pBelt);
  double DistMoved=m_dMaxSpeed*Range(-1.0, SpdFrac, 1.0)*TimeStep;
  
  if (m_lCtrl!=BPC_None)
    {
    double PosnReqd;
    if (Valid(m_dPosnReqd))
      {
      m_dPosnReqd = Range(m_dPosnMin, m_dPosnReqd, m_dPosnMax);
      PosnReqd = m_dPosnReqd;
      }
    else
      {
      PosnReqd = m_IOPtSB.Forward() ? m_dPosnMax : m_dPosnMin;
      }

    double DistApart=PosnReqd-m_dPosnStart;
    m_IOPtSB.SetDirection(DistApart>=0);

    if (m_IOPtSB.Running())
      {
      double StepFrac=Range(-1.0, DistApart/NZ(DistMoved), 1.0);
      DistMoved*=StepFrac;
      }
    else
      DistMoved=0;

    if (m_lCtrl==BPC_Full)
      {
      if (fabs(DistApart)<1.0e-3 && m_IOPtSB.Running())
        m_IOPtSB.Stop();
      else if (fabs(DistApart)>1.0e-3 && !m_IOPtSB.Running())
        m_IOPtSB.Start();
      }
    else if (m_lCtrl==BPC_FwdRev)
      {
      if (m_IOPtSB.Running())
        {
        //"bounce" of edges
        double NextPosn = m_dPosnActual+DistMoved;
        if (NextPosn<=m_dPosnMin)
          {
          m_IOPtSB.SetDirection(true);
          DistMoved = NextPosn-m_dPosnActual;
          }
        else if (NextPosn>=m_dPosnMax)
          {
          m_IOPtSB.SetDirection(false);
          DistMoved = NextPosn-m_dPosnActual;
          }
        }
      }
    }

  m_dPosnActual+=DistMoved;
  double P=Range(m_dPosnMin, m_dPosnActual, m_dPosnMax);
  if (P!=m_dPosnActual)
    {
    m_dPosnActual=P;
    m_IOPtSB.SetTrip();
    }
  
  
  };

// -------------------------------------------------------------------------

IMPLEMENT_MODELUNIT(BeltCnv2, "BeltCnv", "2", Drw_BeltCnv, "Conveyor", "BCV", TOC_ALL|TOC_DYNAMICFLOW|TOC_GRP_TRANSFER|TOC_STD_KENWALT,
                    "Material Transfer:Belt Conveyor Automation",
                    "Belt Conveyor with Automation");

BeltCnv2::BeltCnv2(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MdlNode(pClass_, TagIn, pAttach, eAttach),
  m_WtMtrPos(*(new CWtMtrPosArray)),
  m_BeltPwr("Supply", this, TOA_Embedded),
  m_BeltSB("PL_FwdRv", this, &m_BeltPwr),
  m_Spill(eDIO_Spill, this, false, true, "Spill", IOId_Spill2Area, IOId_AreaSpillI),
  m_Vent(eDIO_Vent, this, false, true, "Vent", IOId_Vent2Area, IOId_AreaVentI),
  m_AccIn(eDIO_Accum, this, false, true, "Accumulation"),
  m_AccOut(eDIO_Deplete, this, false, true, "Depletion"),
  m_AccCalc(this, m_AccIn, m_AccOut),
  m_Q("Belt", this, TOA_Embedded, &m_Spill, &m_Vent, &m_AccIn, &m_AccOut)
  {
  m_BeltPwr.Initialise("525v_A/C");
  m_TripperOn=false;

  AttachClassInfo(nc_Process, NULL);
  AttachIOAreas(BeltCnvIOAreaList);
  //Initialise();
  m_MaxVelocity=1.0;
  m_MaxSpillLoading=100.0;
  m_MaxStallLoading=1000.0;
  m_MaxTurnDown=20;
  m_ProfPts=10;
  m_ProfDispType=QPT_MinSections;
  m_fShowProfile=False;
  m_MinLoading=0.001;
  m_fStartIfStalled=True;
  m_fWasRunning=False;

  m_NLPower=1.0;
  m_FLPower=10.0;
  m_Power=0.0;

  m_LBtnDn=false;
  m_RBtnDn=false;
  m_MousePt.x=-1;
  };

// -------------------------------------------------------------------------

BeltCnv2::~BeltCnv2()
  {
  delete &m_WtMtrPos;
  for (int i=0; i<m_FeedDrives.GetSize(); i++)
    delete m_FeedDrives[i];
  for (i=0; i<m_ProdDrives.GetSize(); i++)
    delete m_ProdDrives[i];
  }

// -------------------------------------------------------------------------

static DDBValueLst DDBFdTypes[] = {
  {QIO_Fixed,       "Fixed" },
  {QIO_Moving,      "Moving"},
  {0}};
static DDBValueLst DDBPrTypesA[] = {
  {QIO_Fixed,       "Fixed" },
  {QIO_FixedEnd,    "FixedAtEnd" },
  {QIO_Moving,      "Moving"},
  //{QIO_Distributed, "Distributed"},
  {0}};
static DDBValueLst DDBPrTypesB[] = {
  {QIO_Fixed,       "Fixed" },
  {QIO_FixedEnd,    "FixedAtEnd" },
  {QIO_Moving,      "Moving"},
  {QIO_Distributed, "Tripper"},
  {0}};
static DDBValueLst DDBControl[] = {
  {CBeltFdPrDrive::BPC_None,        "None" },
  {CBeltFdPrDrive::BPC_FwdRev,      "FwdRev"},
  {CBeltFdPrDrive::BPC_Full,        "Full"},
  {0}};
static DDBValueLst DDB1[] = {
  {QPT_None,        "None" },
  {QPT_FixedPts,    "Fixed Points"},
  {QPT_AllSections, "All Section Pts"},
  {QPT_MinSections, "Min Section Pts"},
  {0}};

/*This provides access to the variables of the model and calls the
BuildDataDefn for objects used by the model.*/
void BeltCnv2::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);

  //DDB.Page   ("Parms");
  DDB.Text    ("");
  DDB.Visibility(NM_Dynamic|SM_All|HM_All);
  BuildDataDefnElevation(DDB);
  DDB.Text("");
  DDB.Double  ("BeltSpeed",    "",                DC_Ldt,  "m/s",   xidBeltSpeed,       this, isResult);
  DDB.CheckBox("BeltStopped",  "",                DC_,     "",      xidBeltStopped,     this, isResult);
  DDB.Visibility();
  DDB.Double  ("Length",       "L",               DC_L,    "m",     xidBeltLength,      this, isParm);
  DDB.Visibility(NM_Dynamic|SM_All|HM_All);
  DDB.Double  ("MaxSpeed",     "",                DC_Ldt,  "m/s",   &m_MaxVelocity,     this, isParm);
  DDB.Double  ("MaxLoading",   "MaxSpillLoad",    DC_MpL,  "kg/m",  &m_MaxSpillLoading, this, isParm);
  DDB.Double  ("MinLoading",   "",                DC_MpL,  "kg/m",  &m_MinLoading,      this, isParm|InitHidden);
  DDB.Long    ("MaxTurndown",  "",                DC_,     "",      &m_MaxTurnDown,     this, isParm|InitHidden);
  
 
  DDB.Visibility(NM_Dynamic|SM_All|HM_All);
  DDB.Double  ("",             "MaxStallLoad",    DC_M,    "kg",    &m_MaxStallLoading, this, isParm);
  DDB.CheckBox("",             "StartIfStalled",  DC_,     "",      &m_fStartIfStalled, this, isParm);
  DDB.Byte    ("",             "WasRunning",      DC_,     "",      &m_fWasRunning,     this, isResult|InitHidden);

  DDB.Double  ("TotalMass",    "",                DC_M,    "kg",    xidTotalMass,       this, isResult);
  DDB.Double  ("AvgLoading",   "",                DC_MpL,  "kg/m",  xidAvgLoading,      this, isResult);

  if (OrigPrjFileVerNo()<90 && DDB.ForFileSnpScn())
    DDB.String  ("SpillArea",    "",              DC_,     "",      &m_sAreaTag,        this, isParmStopped);
  DDB.Double  ("TotalSpilt",   "",                DC_M,    "kg",    xidTotSpilt,        this, isParm);
  DDB.Double  ("Balance",      "",                DC_M,    "kg",    xidBalance,         this, isResult);
  DDB.Double  ("PrevTotalMass","",                DC_M,    "kg",    &m_TotMassPrev,     this, isParmConstruct|InitHidden);
  DDB.Double  ("PrevSpillMass","",                DC_M,    "kg",    &m_SpillMassPrev,   this, isParmConstruct|InitHidden);

  DDB.Text("");
  DDB.Long    ("NoWtMtrs",     "",                DC_,     "",      xidNWtMtrs,         this, isParm);
  DDB.CheckBox("ShowProfile",  "",                DC_,     "",      xidShowProf,        this, isParm|SetOnChange);
  DDB.Visibility(NM_Dynamic|SM_All|HM_All, m_fShowProfile);
  DDB.Long    ("ProfDispType", "",                DC_,     "",      xidProfDispType,    this, isParm, DDB1);
  //DDB.Visibility(NM_Dynamic|SM_All|HM_All, m_fShowProfile && m_ProfDispType==QPT_FixedPts);
  //DDB.Long    ("ProfilePts",   "",                DC_,     "",      xidProfPts,         this, isParm);
  DDB.Visibility(NM_Dynamic|SM_All|HM_All);
  DDB.Long    ("NSections",    "",                DC_,     "",      xidNSections,       this, isResult);
 if (DDB.BeginStruct(this, "MinSect", 0, DDB_NoPage))
    {
    DDB.Double  ("Length",     "",                DC_Frac, "%",     &m_Q.m_dMinFracSectLen,  this, isParm);
    DDB.Visibility(NM_Dynamic|SM_All|HM_All, m_Q.m_dMinFracSectLen>0);
    DDB.Double  ("LoadTol",    "",                DC_Frac, "%",     &m_Q.m_dLoadTol,  this, isParm);
    DDB.Double  ("TempTol",    "",                DC_Frac, "%",     &m_Q.m_dTempTol,  this, isParm);
    DDB.Double  ("MFracTol",   "",                DC_Frac, "%",     &m_Q.m_dMFracTol,  this, isParm);
    }
  DDB.EndStruct();

  DDB.Text("");
  DDB.Long    ("NFeedDrives",  "",                DC_,     "",      xidFeedDrvCount,    this, isParmStopped);
  DDB.Long    ("NProdDrives",  "",                DC_,     "",      xidProdDrvCount,    this, isParmStopped);
  DDB.Text("");
  DDB.CheckBoxBtn("TripperOn",        "",                DC_,     "",      xidTripperOn,       this, isParmStopped);

  DDB.Text("");
  BuildDataDefnShowIOs(DDB);

  DDB.Page("BeltDrive", DDB_RqdPage);
  DDB.Text    ("Power Draw");
  DDB.Double  ("NoLoadPower",  "",                DC_Pwr,  "kW",    &m_NLPower,         this, isParm);
  DDB.Double  ("FullLoadPower","",                DC_Pwr,  "kW",    &m_FLPower,         this, isParm);
  DDB.Double  ("Power",        "",                DC_Pwr,  "kW",    &m_Power,           this, isResult);
  DDB.Text("");
  
  DDB.Text("");
  m_BeltPwr.BuildDataDefn(DDB, this, NULL, 1);
  m_BeltSB.BuildDataDefn(DDB, this, "Speed", 1);

  DDB.Visibility();

  int FdLst[MaxIOList], PrLst[MaxIOList];
  int nFd=0, nPr=0;
  for (int i=0; i<NoFlwIOs(); i++)
    if (IOId_Self(i)==idFeed)
      FdLst[nFd++]=i;
    else if (IOId_Self(i)==idProd)
      PrLst[nPr++]=i;

  int DoOld=((!DDB.ForFiling() && !DDB.ForSnapShot() && !DDB.ForScenario() && !DDB.ForView()) ||
            (DDB.ForFiling() && DDB.DoingPutData()));

  Strng Nm;
  DDB.Page("Feeds", DDB_RqdPage);
  if (DDB.BeginArray(this, "Fd", "BltCnvFd", nFd, 0))
    for (i=0; i<nFd; i++)
      if (DDB.BeginElement(this, Nd_Rmt(FdLst[i])->FullObjTag()))
        {
        DDB.Long       ("Type",     "",  DC_,   "",       xidFeedType0+i,     this, isParmStopped, DDBFdTypes);
        eQIOTypes Typ=m_Q.m_Feed[i].Type();
        if (Typ!=QIO_Fixed)
          DDB.Long     ("DriveNo",  "",  DC_,   "",       xidFeedIndex0+i,    this, isParmStopped);
        DDB.Double     ("Pos",      "",  DC_L , "m",      xidFeedPos0+i,      this, Typ!=QIO_Moving?isParm:0);
        DDB.Visibility(NM_Dynamic|SM_All|HM_All);
        DDB.CheckBoxBtn("Limited",  "",  DC_,   "",       xidFeedLimited0+i,  this, isParm);
        DDB.Visibility(NM_Dynamic|SM_All|HM_All, m_Q.FeedLimited(i));
        DDB.Double     ("Capacity", "",  DC_Frac, "%",    xidFeedCapFrac0+i,  this, isParm);
        DDB.Visibility();
        DDB.Double     ("Qm",       "",  DC_Qm , "kg/s",  xidFeedQm0+i,       this, isResult);
        DDB.Text("");

        if (DoOld)
          {
          DDB.BeginElement(this, i);
          DDB.Double("Pos",     "",  DC_L , "m",    xidFeedPos0+i,       this, isParm);
          DDB.Double("Qm",     "",  DC_Qm , "kg/s",  xidFeedQm0+i,       this, isResult);
          }
        }
  DDB.EndArray();

  DDB.Page("Products", DDB_RqdPage);
  if (DDB.BeginArray(this, "Pr", "BltCnvPr", nPr, 0))
    for (i=0; i<nPr; i++)
      if (DDB.BeginElement(this, Nd_Rmt(PrLst[i])->FullObjTag()))
        {
        DDB.Long  ("Type",    "",  DC_,      "",          xidProdType0+i,      this, m_TripperOn ? 0:isParmStopped, m_TripperOn ? DDBPrTypesB:DDBPrTypesA);
        eQIOTypes Typ=m_TripperOn ? QIO_Distributed : m_Q.m_Prod[i].Type();
        if (Typ!=QIO_Fixed && Typ!=QIO_FixedEnd)
          {
          if (Typ==QIO_Moving)
           DDB.Long     ("DriveNo",  "",  DC_,   "",       xidProdIndex0+i,     this, isParmStopped);

          if (m_TripperOn)
            {
            DDB.Double("BinCentre",   "",  DC_L ,  "m",   xidProdPos0+i,          this, isParm);
            DDB.Double("BinWidth",    "",  DC_L ,  "m",   xidProdBinWidth0+i,     this, isParm);
            DDB.Double("SpillMargin", "",  DC_L ,  "m",   xidProdSpillMargin0+i,  this, isParm);
            }
          }
        if (!m_TripperOn)
          {
          DDB.Double("Pos",     "",  DC_L ,    "m",         xidProdPos0+i,       this, Typ==QIO_Fixed?isParm:isResult);
          }
        DDB.Double("Removal", "",  DC_Frac , "%",         xidProdRemove0+i,    this, isParm);
        DDB.Double("Qm",      "",  DC_Qm ,   "kg/s",      xidProdQm0+i,        this, isResult);
        DDB.Double("Loss",    "",  DC_Qm ,   "kg/s",      xidProdLoss0+i,      this, isResult);

        DDB.Text("");

        if (DoOld)
          {
          DDB.BeginElement(this, i);
          //DDB.TagComment(Nd_Rmt(PrLst[i])->FullObjTag());
          //DDB.BeginElement(this, Nd_Rmt(PrLst[i])->FullObjTag());
          DDB.Double("Pos",     "",   DC_L ,     "m",    xidProdPos0+i,       this, isParm);
          DDB.Double("Removal","",    DC_Frac , "%",     xidProdRemove0+i,    this, isParm);
          DDB.Double("Qm",    "",     DC_Qm ,   "kg/s",  xidProdQm0+i,        this, isResult);
          DDB.Double("Loss",  "",     DC_Qm ,   "kg/s",  xidProdLoss0+i,      this, isResult);
          }
        }
  DDB.EndArray();

  if (m_FeedDrives.GetSize())
    {
    DDB.Page("FeedDrives", DDB_RqdPage);
    if (DDB.BeginArray(this, "FdDrv", "FdDrv", m_FeedDrives.GetSize(), 0))
      {
      for (int i=0; i<m_FeedDrives.GetSize(); i++)
        if (DDB.BeginElement(this, i, 0, -1, DDB_OptPage))
          {
          CBeltFdPrDrive &Fd=*m_FeedDrives[i];
          Fd.m_Pwr.BuildDataDefn(DDB);
          DDB.BeginStruct(this, "Posn", 0, DDB_NoPage);
          DDB.Long  ("Control","",   DC_ ,      "",      &Fd.m_lCtrl,        this, isParmStopped, DDBControl);
          DDB.Double("Reqd",   "",   DC_L ,     "m",     &Fd.m_dPosnReqd,    this, isParm|NAN_OK);
          DDB.Double("Actual", "",   DC_L ,     "m",     &Fd.m_dPosnActual,  this, isResult);
          DDB.Double("Min",    "",   DC_L ,     "m",     &Fd.m_dPosnMin,     this, isParm);
          DDB.Double("Max",    "",   DC_L ,     "m",     &Fd.m_dPosnMax,     this, isParm);
          DDB.EndStruct();
          DDB.Double("MaximumSpeed", "MaxSpeed",   DC_Ldt,    "m/s",   &Fd.m_dMaxSpeed,    this, isParm);
          DDB.Text("");
          Fd.m_IOPtSB.BuildDataDefn(DDB, this, "Motor", 100+i);
          DDB.Text("");
          }
      }
    DDB.EndArray();
    }
 
  if (m_ProdDrives.GetSize())
    {
    DDB.Page("ProdDrives", DDB_RqdPage);
    if (DDB.BeginArray(this, "PrDrv", "PrDrv", m_ProdDrives.GetSize(), 0))
      {
      for (int i=0; i<m_ProdDrives.GetSize(); i++)
        if (DDB.BeginElement(this, i, 0, -1, DDB_OptPage))
          {
          CBeltFdPrDrive &Pr=*m_ProdDrives[i];
          Pr.m_Pwr.BuildDataDefn(DDB);
          DDB.BeginStruct(this, "Posn", 0, DDB_NoPage);
          DDB.Long  ("Control","",   DC_ ,      "",      &Pr.m_lCtrl,        this, isParmStopped, DDBControl);
          DDB.Double("Reqd",   "",   DC_L ,     "m",     &Pr.m_dPosnReqd,    this, isParm|NAN_OK);
          DDB.Double("Actual", "",   DC_L ,     "m",     &Pr.m_dPosnActual,  this, isResult);
          DDB.Double("Min",    "",   DC_L ,     "m",     &Pr.m_dPosnMin,     this, isParm);
          DDB.Double("Max",    "",   DC_L ,     "m",     &Pr.m_dPosnMax,     this, isParm);
          DDB.EndStruct();
          DDB.Text("");
          DDB.Double("MaximumSpeed", "MaxSpeed",   DC_Ldt,    "m/s",   &Pr.m_dMaxSpeed,    this, isParm);
          Pr.m_IOPtSB.BuildDataDefn(DDB, this, "Motor", 200+i);
          DDB.Text("");
          }
      }
    DDB.EndArray();
    }

  if (m_WtMtrPos.GetSize())
    {
    DDB.Page("WtMtrs", DDB_RqdPage);
    if (DDB.BeginArray(this, "WtMtr", "WtMtr", m_WtMtrPos.GetSize(), 0))
      {
      for (int i=0; i<m_WtMtrPos.GetSize(); i++)
        if (DDB.BeginElement(this, i))
          {
          DDB.Double("Pos",      "",                  DC_L ,     "m",     xidWtrMtrPos0+i,      this, isParm);
          DDB.Double("Loading",  "",                  DC_MpL,    "kg/m",  xidWtrMtrLd0+i,       this, isResult|0);
          DDB.Double("Rate",     "",                  DC_Qm,     "kg/s",  xidWtrMtrRate0+i,     this, isResult|0);
          DDB.Text("");
          }
      }
    DDB.EndArray();
    }

  if (m_fShowProfile)
    DDB.Object(&m_Q.Profile(), this);

  if (NetDynamicMethod())
    {
    if (m_Spill.Enabled || m_Vent.Enabled || m_AccIn.Enabled || m_AccOut.Enabled)
      {
      DDB.Page("AreaIO", DDB_RqdPage);
      m_Spill.BuildDataDefn(DDB, NULL, DDB_NoPage, 1, DFIO_ShowQm);
      m_Vent.BuildDataDefn(DDB, NULL, DDB_NoPage, 2, DFIO_ShowQm);
      m_AccIn.BuildDataDefn(DDB, NULL, DDB_NoPage, 3, DFIO_ShowQm);
      m_AccOut.BuildDataDefn(DDB, NULL, DDB_NoPage, 4, DFIO_ShowQm);
      }
    }

  DDB.EndStruct();
  };

// --------------------------------------------------------------------------

flag BeltCnv2::DataXchg(DataChangeBlk & DCB)
  {
  if (MdlNode::DataXchg(DCB))
    return 1;
  if (m_BeltPwr.DataXchg(DCB))
    return 1;
  if (m_BeltSB.DataXchg(DCB))
    return 1;
  if (DCB.dwUserInfo==1 && m_Spill.DataXchg(DCB))
    return 1;
  if (DCB.dwUserInfo==2 && m_Vent.DataXchg(DCB))
    return 1;
  if (DCB.dwUserInfo==3 && m_AccIn.DataXchg(DCB))
    return 1;
  if (DCB.dwUserInfo==4 && m_AccOut.DataXchg(DCB))
    return 1;

  if (DCB.dwUserInfo>=100 && DCB.dwUserInfo<=199)
    {                               
    if (m_FeedDrives[DCB.dwUserInfo-100]->m_Pwr.DataXchg(DCB))
      return 1;
    if (m_FeedDrives[DCB.dwUserInfo-100]->m_IOPtSB.DataXchg(DCB))
      return 1;
    }
  if (DCB.dwUserInfo>=200 && DCB.dwUserInfo<=299)
    {
    if (m_ProdDrives[DCB.dwUserInfo-200]->m_Pwr.DataXchg(DCB))
      return 1;
    if (m_ProdDrives[DCB.dwUserInfo-200]->m_IOPtSB.DataXchg(DCB))
      return 1;
    }

  switch (DCB.lHandle)
    {
    case xidBeltLength:
      if (DCB.rD)
        {
        m_Q.SetLength(*DCB.rD);
        for (int i=0; i<m_Q.NProds(); i++)
          {
          if (m_Q.m_Prod[i].Type()==QIO_FixedEnd)
            m_Q.m_Prod[i].SetPosition(m_Q.Length());
          }
        }
      DCB.D=m_Q.Length();
      return 1;
    case xidBeltSpeed:
      DCB.D=m_MaxVelocity*Range(-1.0, m_BeltSB.Speed(this), 1.0);
      return 1;
    case xidBeltStopped:
      DCB.B=(fabs(Range(-1.0, m_BeltSB.Speed(this), 1.0))<1.0e-5);
      return 1;
    case xidTotSpilt:
      if (DCB.rD)
        {
        double Scl=*DCB.rD/GTZ(m_Q.TotalSpiltMass());
        m_Q.SetTotalSpilt(*DCB.rD, Scl*m_Q.TotalSpiltHf(), Scl*m_Q.TotalSpiltHs(), Scl*m_Q.TotalSpiltHz());
        }
      DCB.D=m_Q.TotalSpiltMass();
      return 1;
    case xidNWtMtrs:
      if (DCB.rL)
        {
        m_WtMtrPos.SetSize(*DCB.rL);
        StructureChanged(this);
        }
      DCB.L=m_WtMtrPos.GetSize();
      return 1;
    case xidShowProf:
      if (DCB.rB)
        {
        m_fShowProfile=*DCB.rB;
        if (m_fShowProfile)
          m_Q.SetShowProfileType(m_ProfDispType);
        else
          m_Q.SetShowProfileType(QPT_None);
        StructureChanged(this);
        }
      DCB.B=m_fShowProfile;
      return 1;
    case xidProfDispType:
      if (DCB.rL)
        {
        m_ProfDispType=*DCB.rL;
        if (m_fShowProfile)
          {
          m_Q.SetShowProfileType(m_ProfDispType);
          if (m_ProfDispType==QPT_FixedPts)
            m_Q.SetProfileLen(m_ProfPts);
          StructureChanged(this);
          }
        }
      DCB.L=m_ProfDispType;
      return 1;
    case xidProfPts:
      if (DCB.rL)
        {
        m_ProfPts = Range(0L, *DCB.rL, 128L);
        m_Q.SetProfileLen(m_ProfPts);
        StructureChanged(this);
        }
      DCB.L=m_ProfPts;
      return 1;
    case xidNSections:
      DCB.L=m_Q.NSections();
      return 1;
    case xidTotalMass:
      DCB.D=m_Q.TotalMass();
      return 1;
    case xidAvgLoading:
      DCB.D=m_Q.AverageLoading();
      return 1;
    case xidFeedDrvCount:
      {
      if (DCB.rL)
        {
        int OldSize=m_FeedDrives.GetSize();
        int NewSize=*DCB.rL;
        for (int i=NewSize; i<OldSize; i++)
          delete m_FeedDrives[i];
        m_FeedDrives.SetSize(NewSize);
        for (i=OldSize; i<NewSize; i++)
          m_FeedDrives[i]=new CBeltFdPrDrive(this);
        }
      DCB.L=m_FeedDrives.GetSize();
      return 1;
      }
    case xidProdDrvCount:
      {
      if (DCB.rL)
        {
        int OldSize=m_ProdDrives.GetSize();
        int NewSize=*DCB.rL;
        for (int i=NewSize; i<OldSize; i++)
          delete m_ProdDrives[i];
        m_ProdDrives.SetSize(NewSize);
        for (i=OldSize; i<NewSize; i++)
          m_ProdDrives[i]=new CBeltFdPrDrive(this);
        }
      DCB.L=m_ProdDrives.GetSize();
      return 1;
      }
    case xidBalance:
      {
      double B=-(m_Q.TotalMass()-m_TotMassPrev)-(m_Q.TotalSpiltMass()-m_SpillMassPrev); 
      for (int i=0; i<NoFlwIOs(); i++)
        B+=IOQm_In(i)*ICGetTimeInc();
      DCB.D=B;
      return 1;
      }
      break;
    case xidTripperOn:
      {
      if (DCB.rB)
        {
        m_TripperOn=*DCB.rB;
        if (m_TripperOn)
          {
          int OldSize=m_ProdDrives.GetSize();
          int NewSize=1;
          for (int i=NewSize; i<OldSize; i++)
            delete m_ProdDrives[i];
          m_ProdDrives.SetSize(NewSize);
          for (i=OldSize; i<NewSize; i++)
            m_ProdDrives[i]=new CBeltFdPrDrive(this);
          }
        }
      DCB.B=m_TripperOn;
      return 1;
      }
    default:
      {
      int i=DCB.lHandle-xidFeedType0;
      if (i>=0 && i<m_Q.NFeeds())
        {
        if (DCB.rL)
          m_Q.m_Feed[i].SetType((eQIOTypes)*DCB.rL);
        DCB.L=m_Q.m_Feed[i].Type();
        return 1;
        }
      i=DCB.lHandle-xidFeedIndex0;
      if (i>=0 && i<m_Q.NFeeds())
        {
        if (DCB.rL)
          m_Q.m_Feed[i].SetIndex(*DCB.rL);
        DCB.L=m_Q.m_Feed[i].Index();
        return 1;
        }
      i=DCB.lHandle-xidFeedPos0;
      if (i>=0 && i<m_Q.NFeeds())
        {
        if (DCB.rD)
          m_Q.m_Feed[i].SetPosition(*DCB.rD);
        DCB.D=m_Q.m_Feed[i].Position();
        return 1;
        }
      i=DCB.lHandle-xidFeedQm0;
      if (i>=0 && i<m_Q.NFeeds())
        {
        DCB.D=m_Q.m_Feed[i].QmAct();
        return 1;
        }
      i=DCB.lHandle-xidFeedLimited0;
      if (i>=0 && i<m_Q.NFeeds())
        {
        if (DCB.rB)
          m_Q.m_Feed[i].SetFeedLimited(*DCB.rB);
        DCB.B=m_Q.m_Feed[i].FeedLimited();
        return 1;
        }
      i=DCB.lHandle-xidFeedCapFrac0;
      if (i>=0 && i<m_Q.NFeeds())
        {
        if (DCB.rD)
          m_Q.m_Feed[i].SetFeedCapFrac(*DCB.rD);
        DCB.D=m_Q.m_Feed[i].FeedCapFrac();
        return 1;
        }
      i=DCB.lHandle-xidProdType0;
      if (i>=0 && i<m_Q.NProds())
        {
        if (DCB.rL)
          m_Q.m_Prod[i].SetType((eQIOTypes)*DCB.rL);
        DCB.L=!m_TripperOn||DCB.ForFileSnpScn() ? m_Q.m_Prod[i].Type() : QIO_Distributed;
        return 1;
        }
      i=DCB.lHandle-xidProdIndex0;
      if (i>=0 && i<m_Q.NProds())
        {
        if (DCB.rL)
          m_Q.m_Prod[i].SetIndex(*DCB.rL);
        DCB.L=m_Q.m_Prod[i].Index();
        return 1;
        }
      i=DCB.lHandle-xidProdBinWidth0;
      if (i>=0 && i<m_Q.NProds())
        {
        if (DCB.rD)
          m_Q.m_Prod[i].SetBinWidth(*DCB.rD);
        DCB.D=m_Q.m_Prod[i].BinWidth();
        return 1;
        }
      i=DCB.lHandle-xidProdSpillMargin0;
      if (i>=0 && i<m_Q.NProds())
        {
        if (DCB.rD)
          m_Q.m_Prod[i].SetSpillMargin(*DCB.rD);
        DCB.D=m_Q.m_Prod[i].SpillMargin();
        return 1;
        }
      i=DCB.lHandle-xidProdPos0;
      if (i>=0 && i<m_Q.NProds())
        {
        if (DCB.rD)
          m_Q.m_Prod[i].SetPosition(*DCB.rD);
        DCB.D=m_Q.m_Prod[i].Position();
        return 1;
        }
      i=DCB.lHandle-xidProdRemove0;
      if (i>=0 && i<m_Q.NProds())
        {
        if (DCB.rD)
          m_Q.m_Prod[i].SetRemoval(*DCB.rD);
        DCB.D=m_Q.m_Prod[i].Removal();
        return 1;
        }
      i=DCB.lHandle-xidProdQm0;
      if (i>=0 && i<m_Q.NProds())
        {
        DCB.D=m_Q.m_Prod[i].QmAct();
        return 1;
        }
      i=DCB.lHandle-xidProdLoss0;
      if (i>=0 && i<m_Q.NProds())
        {
        DCB.D=m_Q.m_Prod[i].Loss();
        return 1;
        }
      i=DCB.lHandle-xidWtrMtrPos0;
      if (i>=0 && i<m_WtMtrPos.GetSize())
        {
        if (DCB.rD)
          m_WtMtrPos[i]=*DCB.rD;
        DCB.D=m_WtMtrPos[i];
        return 1;
        }
      i=DCB.lHandle-xidWtrMtrLd0;
      if (i>=0 && i<m_WtMtrPos.GetSize())
        {
        DCB.D=m_Q.Loading(m_WtMtrPos[i]);
        return 1;
        }
      i=DCB.lHandle-xidWtrMtrRate0;
      if (i>=0 && i<m_WtMtrPos.GetSize())
        {
        DCB.D=m_Q.Loading(m_WtMtrPos[i])*m_MaxVelocity*Range(-1.0, m_BeltSB.Speed(this), 1.0);
        return 1;
        }
      }
    }
  return 0;
  }

//---------------------------------------------------------------------------

flag BeltCnv2::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=MdlNode::ValidateData(VDB);
  //m_Q.Spill.SetTarget(m_SpillName());

  flag BadFeedPosition=0;
  for (int i=0; i<m_Q.NFeeds(); i++)
    if (m_Q.m_Feed[i].Position()<0.0 || m_Q.m_Feed[i].Position()>m_Q.Length())
      BadFeedPosition=1;
  SetCI(1, BadFeedPosition);

  flag BadProdPosition=0;
  for (i=0; i<m_Q.NProds(); i++)
    {
    if (m_Q.m_Prod[i].Type()==QIO_FixedEnd)
      m_Q.m_Prod[i].SetPosition(m_Q.Length());
    if (m_Q.m_Prod[i].Position()<0.0 || m_Q.m_Prod[i].Position()>m_Q.Length())
      BadProdPosition=1;
    }
  SetCI(2, BadProdPosition);

  flag BadMtrPosition=0;
  for (i=0; i<m_WtMtrPos.GetSize(); i++)
    if (m_WtMtrPos[i]<0.0 || m_WtMtrPos[i]>m_Q.Length())
      BadMtrPosition=1;
  SetCI(3, BadMtrPosition);

  for (i=0; i<m_FeedDrives.GetSize(); i++)
    {
    CBeltFdPrDrive & Fd=*m_FeedDrives[i];
    Fd.m_dPosnActual=Range(Fd.m_dPosnMin, Fd.m_dPosnActual, Fd.m_dPosnMax);
    }  
  for (i=0; i<m_ProdDrives.GetSize(); i++)
    {
    CBeltFdPrDrive & Pr=*m_ProdDrives[i];
    Pr.m_dPosnActual=Range(Pr.m_dPosnMin, Pr.m_dPosnActual, Pr.m_dPosnMax);
    }  

  //m_Q.SetSpill(Spills.Find(SpillName()));
  return OK;
  }

//--------------------------------------------------------------------------

void BeltCnv2::CollectElectrics(CNodeElectricsArray & TSCA)
  {
  MdlNode::CollectElectrics(TSCA);

  CNodeElectrics &TSP=m_Electrics;
  if (m_BeltPwr.TermStrip())
    TSP.AddTermStrip(m_BeltPwr.TermStrip());
  for (int i=0; i<m_FeedDrives.GetSize(); i++)
    {
    CBeltFdPrDrive & Fd=*m_FeedDrives[i];
    if (Fd.m_Pwr.TermStrip())
      TSP.AddTermStrip(Fd.m_Pwr.TermStrip());
    }  
  for (i=0; i<m_ProdDrives.GetSize(); i++)
    {
    CBeltFdPrDrive & Pr=*m_ProdDrives[i];
    if (Pr.m_Pwr.TermStrip())
      TSP.AddTermStrip(Pr.m_Pwr.TermStrip());
    }  
  };

//--------------------------------------------------------------------------

void BeltCnv2::ConnectElectrics()
  {
  MdlNode::ConnectElectrics();
  if (m_BeltPwr.TermStrip())
    m_BeltPwr.ConnectPower();
  };

//---------------------------------------------------------------------------

flag BeltCnv2::GetOtherData(FilingControlBlock &FCB)
  {
  return m_Q.GetOtherData(FCB);
  };

//---------------------------------------------------------------------------

flag BeltCnv2::PutOtherData(FilingControlBlock &FCB)
  {
  return m_Q.PutOtherData(FCB);
  };

//--------------------------------------------------------------------------

CFlange * BeltCnv2::GetFlange(int IOId)
  {
  switch (IOId)
    {
    case IOId_Spill2Area:   return &m_Spill.Flange;
    case IOId_Vent2Area:    return &m_Vent.Flange;
    //case IOId_AccIn2Area:   return &m_AccIn.Flange;
    //case IOId_AccOut2Area:  return &m_AccOut.Flange;
    }
  return NULL;
  };

//---------------------------------------------------------------------------

void BeltCnv2::PostConnect(int IONo)
  {
  if (!PostConnectDirect(IONo))
    {
    MdlNode::PostConnect(IONo);
    if (IOId_Self(IONo)==idFeed)
      {
      m_Q.AddFeed();
      StructureChanged(this);
      }
    else if (IOId_Self(IONo)==idProd)
      {
      m_Q.AddProd();
      StructureChanged(this);
      }
    }
  };

//---------------------------------------------------------------------------

void BeltCnv2::PreDisConnect(int IONo)
  {
  if (!PreDisConnectDirect(IONo))
    {
    if (IOId_Self(IONo)==idFeed)
      {
      for (int i=0, n=-1; i<=IONo; i++)
        if (IOId_Self(i)==idFeed)
          n++;
      if (n>=0)
        m_Q.DeleteFeed(n);
      StructureChanged(this);
      }
    else if (IOId_Self(IONo)==idProd)
      {
      for (int i=0, n=-1; i<=IONo; i++)
        if (IOId_Self(i)==idProd)
          n++;
      if (n>=0)
        m_Q.DeleteProd(n);
      StructureChanged(this);
      }
    MdlNode::PreDisConnect(IONo);
    }
  };

//--------------------------------------------------------------------------

void BeltCnv2::SetState(eScdMdlStateActs RqdState)
  {
  MdlNode::SetState(RqdState);
  m_Q.SetState(RqdState);
  switch (RqdState)
    {
    case MSA_PBInit:
    case MSA_Empty:
    case MSA_PreSet:
      m_Q.Empty();
      m_TotMassPrev=0.0;
      m_Q.SetTotalSpilt(0.0, 0.0, 0.0, 0.0);
      m_SpillMassPrev=0.0;
      break;
    case MSA_SteadyState:
      break;
    case MSA_EmptySpillTargets:
      m_Q.SetTotalSpilt(0.0, 0.0, 0.0, 0.0);
      m_SpillMassPrev=0.0;
      break;
    case MSA_DynStatsRunInit:
      //if (pStats)
      //  pStats->ResetStats();
      //dInitMt = m_Q.TotalMass();
      m_Q.SetTotalSpilt(0.0, 0.0, 0.0, 0.0);
      m_SpillMassPrev=0.0;
      break;
    }
  };

//---------------------------------------------------------------------------

void BeltCnv2::StartSolution()
  {
  };

//---------------------------------------------------------------------------

void BeltCnv2::ConfigureJoins()
  {
  int i;
  switch (NetMethod())
    {
    case NM_Probal:
      for (i=0; (i<NoProcessIOs()); i++)
        SetIO_Join(i, 0);
      break;
    case NM_Dynamic:
      for (int i = 0; i < NoProcessIOs(); i++)
        SetIO_Open(i, 0, false, ESS_Denied);
      break;
    }
  };

//---------------------------------------------------------------------------

void BeltCnv2::StartStep()
  {
  m_MaxSpillLoading = Max(0.01, m_MaxSpillLoading);
  m_Q.SetStepInfo(ICGetTimeInc(), m_MaxVelocity, m_MaxVelocity*Range(-1.0, m_BeltSB.Speed(this), 1.0), m_MaxTurnDown);
  m_TotMassPrev=m_Q.TotalMass();
  m_SpillMassPrev=m_Q.TotalSpiltMass();
  m_Q.SetMaxSpillLoading(m_MaxSpillLoading);
  m_Q.SetMinLoading(m_MinLoading);
  m_AccCalc.Start(0, m_Q);

  switch (NetMethod())
    {
    case NM_Dynamic:
      {
      if (m_TripperOn)
        {
        CBeltFdPrDrive &TrDrv=*m_ProdDrives[0];
        m_Q.SetTripper(true, TrDrv.Speed(this), TrDrv.m_dPosnActual, TrDrv.m_dPosnMin, TrDrv.m_dPosnMax);
        }
      else
        m_Q.SetTripper(false);

      int i;
      bool CI4=false;
      for (i=0; i<m_Q.NFeeds(); i++)
        {
        SpQIO &Fd=m_Q.m_Feed[i];
        switch (Fd.Type())
          {
          case QIO_Moving:
            if (Fd.Index()>=0 && Fd.Index()<m_FeedDrives.GetSize())
              {
              CBeltFdPrDrive &FDrv=*m_FeedDrives[Fd.Index()];
              Fd.SetIOMoving(true, FDrv.m_dPosnActual, FDrv.m_dPosnReqd, FDrv.Speed(this), ICGetTimeInc(), FDrv.m_dPosnMin, FDrv.m_dPosnMax);
              }
            else
              CI4=true;
            break;
          default:
            Fd.SetIOMoving(false);
            break;
          }
        }
      for (i=0; i<m_Q.NProds(); i++)
        {
        SpQIO &Pr=m_Q.m_Prod[i];
        switch (!m_TripperOn ? Pr.Type() : QIO_Distributed)
          {
          case QIO_Moving:
            if (Pr.Index()>=0 && Pr.Index()<m_ProdDrives.GetSize())
              {
              CBeltFdPrDrive &PDrv=*m_ProdDrives[Pr.Index()];
              Pr.SetIOMoving(true, PDrv.m_dPosnActual, PDrv.m_dPosnReqd, PDrv.Speed(this), ICGetTimeInc(), PDrv.m_dPosnMin, PDrv.m_dPosnMax);
              Pr.SetUseBinSizes(false);
              }
            else
              CI4=true;
            break;
          case QIO_Distributed:
            if (0<m_ProdDrives.GetSize())
              {
              CBeltFdPrDrive &PDrv=*m_ProdDrives[Pr.Index()];
              Pr.SetUseBinSizes(true);//, Lo, Hi);
              }
            else
              CI4=true;
            break;
          default:
            Pr.SetIOMoving(false);
            Pr.SetUseBinSizes(false);
            break;
          }
        }
      SetCI(4, CI4);
      break;
      }
    }
  };

//--------------------------------------------------------------------------

void BeltCnv2::EvalJoinPressures(long JoinMask)
  {
  for (int i = 0; i < NoFlwIOs(); i++)
    Set_IOP_Self(i, AtmosPress(IODatum_Term(i)));
  };

//--------------------------------------------------------------------------

flag BeltCnv2::EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo)
  {
  switch (Task)
    {
    case FET_GetMode:
      {
      int Feed=0;
      for (int i=0; i<IONo; i++)
        if (IOId_Self(i)==idFeed)
          Feed++;
      if (IOId_Self(IONo)==idFeed)
        {
        if (m_Q.FeedLimited(Feed))
          {
          IOFB(IONo,FE)->SetXferMode();
          IOFB(IONo,FE)->SetXferCapacity(m_Q.FeedCapacity(Feed));
          }
        else
          IOFB(IONo,FE)->SetXferMode();
        return True;
        }

      IOFB(IONo,FE)->SetQmFree();
      break;
      }
    case FET_SetQm:
      IOFB(IONo, FE)->SetDPb(0.0, 0.0);
      IOFB(IONo, FE)->SetDPbX(0.0);
      IOFB(IONo, FE)->SetDPq(0.0, 0.0);
      break;
    case FET_CalcDP:
      break;
    }
  return True;
  };

//--------------------------------------------------------------------------

void BeltCnv2::EvalProducts(CNodeEvalIndex & NEI)
  {
  switch (SolveMethod())
    {
    case SM_Direct:
      if (NoProcessJoins()>=1)
        Xfer_EvalProducts(0, Joins[0].Pressure(), NULL, NULL, NULL, NULL, NULL);
      #pragma chFIXIT(Correct conveyor output split in ProBal needs implementing)
      break;
    case SM_Inline:
    case SM_Buffered:
      {
      int i, Feed=0;
      for (i=0; i<NoFlwIOs(); i++)
        if (IOId_Self(i)==idFeed)
          m_Q.SetFeed(Feed++, FlwPos(IOQmEst_In(i)) ? IOQm_In(i):0.0, IOConduit(i));
      int Prod=0;
      for (i=0; i<NoFlwIOs(); i++)
        if (IOId_Self(i)==idProd)
          m_Q.SetProductQmEst(Prod++, IOQmEst_Out(i));
      m_Q.EvalProducts();
      Prod=0;
      for (i=0; i<NoFlwIOs(); i++)
        if (IOId_Self(i)==idProd)
          m_Q.GetProduct(Prod++, IOConduit(i));
      }
      break;
    }
  }

//---------------------------------------------------------------------------

void BeltCnv2::EvalPowerRequired()
  {
  m_BeltPwr.Zero();
  if (Valid(m_Power))
    m_BeltPwr.SetPower(m_Power);

  for (int i=0; i<m_FeedDrives.GetSize(); i++)
    {
    CBeltFdPrDrive & Fd=*m_FeedDrives[i];
    Fd.m_Pwr.Zero();
    double Power=0;
    if (Valid(Power))
      Fd.m_Pwr.SetPower(Power);
    }
  for (i=0; i<m_ProdDrives.GetSize(); i++)
    {
    CBeltFdPrDrive & Pr=*m_ProdDrives[i];
    Pr.m_Pwr.Zero();
    double Power=0;
    if (Valid(Power))
      Pr.m_Pwr.SetPower(Power);
    }
  }

//--------------------------------------------------------------------------

void BeltCnv2::EvalCtrlActions(eScdCtrlTasks Tasks)
  {
  m_BeltSB.EvalCtrlActions(this);
  for (int i=0; i<m_FeedDrives.GetSize(); i++)
    m_FeedDrives[i]->m_IOPtSB.EvalCtrlActions(this);
  for (i=0; i<m_ProdDrives.GetSize(); i++)
    m_ProdDrives[i]->m_IOPtSB.EvalCtrlActions(this);
  }

//--------------------------------------------------------------------------

/*This is called once, after the iteration process and allows the discreet
manipulation of the contents contained by the surge unit.*/

void BeltCnv2::EvalDiscrete()
  {
  m_Spill.Sum.ZeroFlows();

  for (int i=0; i<m_FeedDrives.GetSize(); i++)
    m_FeedDrives[i]->Advance(ICGetTimeInc());
  
  for (i=0; i<m_ProdDrives.GetSize(); i++)
    m_ProdDrives[i]->Advance(ICGetTimeInc());

  int Feed=0;
  for (i=0; i<NoFlwIOs(); i++)
    if (IOId_Self(i)==idFeed)
      m_Q.SetFeed(Feed++, IOQm_In(i), IOConduit(i));

  flag Moved=(m_fWasRunning && (m_Q.TotalMass()<m_MaxStallLoading || m_fStartIfStalled));

  m_Q.Advance(ICGetTimeInc(), Moved);

  m_fWasRunning = fabs(m_BeltSB.Speed(this))>0.0;
  m_Power=(m_NLPower+(m_FLPower-m_NLPower)* m_Q.AverageLoading()/m_MaxSpillLoading)*Range(-1.0, m_BeltSB.Speed(this), 1.0);

  m_BeltSB.EvalDiscrete(this);
  for (i=0; i<m_FeedDrives.GetSize(); i++)
    m_FeedDrives[i]->m_IOPtSB.EvalDiscrete(this);
  for (i=0; i<m_ProdDrives.GetSize(); i++)
    m_ProdDrives[i]->m_IOPtSB.EvalDiscrete(this);

  m_AccCalc.Complete(0, m_Q, ICGetTimeInc());
  }

//--------------------------------------------------------------------------

void BeltCnv2::ClosureInfo()
  {
  //bool InRange=true;
  //for (int i=0; i<NoFlwIOs(); i++)
  //  InRange = InRange && IOConduit(i)->SMFnsInRange();
  //InRange = InRange && Contents.SMFnsInRange();
  //FlwNode::SetCI(29, !InRange);

  if (m_Closure.DoFlows())
    {
    }
  else
    { 
    if (m_Closure.GetSize()>0)
      {
      CClosureInfo &CI=m_Closure[0];
      CI.m_pRefMdl      = m_Q.Model();
      CI.m_ContentMass  = m_Q.TotalMass()+m_Q.TotalSpiltMass();
      CI.m_ContentHeat  = m_Q.TotalHf()+m_Q.TotalSpiltHf();
      }
    }        
  };

//--------------------------------------------------------------------------

dword BeltCnv2::ModelStatus()
  {
  dword Status=MdlNode::ModelStatus();
  if (NoFlwIOs())
    {
    int HasFlw=0;
    double TFlw=0.0;
    for (int i=0; i<NoFlwIOs(); i++)
      {
      TFlw+=IOQm_In(i);
      if (IOConduit(i)->QMass()>UsableMass)
        HasFlw=1;
      }
    Status |= (HasFlw ? FNS_UFlw : FNS_UNoFlw);
    if (TFlw>1.0e-6)
      Status |= FNS_NettQmP;
    else if (TFlw<-1.0e-6)
      Status |= FNS_NettQmN;
    }
  Status |= (m_BeltSB.On() && fabs(m_BeltSB.Speed(this))>1.0e-6 ? FNS_On : FNS_Off);
  return Status;
  }

//--------------------------------------------------------------------------

flag BeltCnv2::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case  1: pS="E\tBad Feed Position";                      return 1;
    case  2: pS="E\tBad Prod Position";                      return 1;
    case  3: pS="E\tBad Weightometer Position";              return 1;
    case  4: pS="E\tIncorrecly connected Drive";             return 1;
    default:
      return MdlNode::CIStrng(No, pS);
    }
  };

//--------------------------------------------------------------------------

flag BeltCnv2::GetModelAction(CMdlActionArray & Acts)
  {
  //CMdlAction {MAT_NULL, MAT_State, MAT_Value};
  CMdlAction M0(0, MAT_State, !m_BeltSB.On(), "Start", 1);
  CMdlAction M1(1, MAT_State, m_BeltSB.On(),  "Stop", 0);
  CMdlAction M2(2, MAT_Value, m_BeltSB.On(),  "Speed (%)", true,
                  m_BeltSB.ManualSpeed(this)*100, 0.0, 100.0,
                  m_BeltSB.Speed(this)*100);

  Acts.SetSize(0);
  Acts.SetAtGrow(0, M0);
  Acts.SetAtGrow(1, M1);
  Acts.SetAtGrow(2, M2);
  Acts.SetAtGrow(3, CMdlAction(3, MAT_Switch));
  return True;
  }

//--------------------------------------------------------------------------

flag BeltCnv2::SetModelAction(CMdlAction & Act)
  {
  switch (Act.iIndex)
    {
    case 0:
      m_BeltSB.SetOn(Act.iValue);
      break;
    case 1:
      m_BeltSB.SetOn(Act.iValue);
      break;
    case 2:
      m_BeltSB.SetManualSpeed(Act.dValue*0.01);
      break;
    case 3:
      m_BeltSB.SetOn(!m_BeltSB.On());
      break;
    }
  return true;
  }

//--------------------------------------------------------------------------

flag BeltCnv2::GetModelGraphic(CMdlGraphicArray & Grfs)
  {
  CMdlGraphic G0(0, MGT_Simple, true, "Show Profile", 1);

  Grfs.SetSize(0);
  Grfs.SetAtGrow(0, G0);
  return true; 
  };

//--------------------------------------------------------------------------

flag BeltCnv2::OperateModelGraphic(CMdlGraphicWnd & Wnd, CMdlGraphic & Grf)
  {
  const COLORREF White = COLORREF(RGB(255,255,255));
  const COLORREF Black = COLORREF(RGB(0,0,0));
  const COLORREF Cyan  = COLORREF(RGB(0,255,255));
  const COLORREF Red   = COLORREF(RGB(255,0,0));
  const COLORREF Green = COLORREF(RGB(0,255,0));

  switch (Wnd.m_eTask)
    {
    case MGT_Create:
      break;

    case MGT_EraseBkgnd:
        // Use Wnd.m_pDC;
       Wnd.m_bReturn = 0;// 1 if erased else 0
      break;

    case MGT_Paint:
      {
      double CursX=-1;
      double CursLd=0;
      if (/*m_LBtnDn &&*/ m_MousePt.x>=0 && m_MousePt.x<Wnd.m_ClientRect.right)
        CursX=(double)m_MousePt.x/Wnd.m_ClientRect.right*m_Q.Length();

      int y0 = Wnd.m_TextSize.y+1;
      int xlast=0;
      POSITION Pos=m_Q.m_Sections.GetHeadPosition();
      while (Pos)
        {
        SpQSection & S = *m_Q.m_Sections.GetNext(Pos);
        int x1 = (int)(S.Start()/m_Q.Length()*Wnd.m_ClientRect.right);
        int x2 = (int)(S.End()/m_Q.Length()*Wnd.m_ClientRect.right);
        int y1 = y0+(int)((1.0-S.Loading()/m_MaxSpillLoading)*(Wnd.m_ClientRect.bottom-y0));
        int y2 = Wnd.m_ClientRect.bottom;
        if (x1>xlast)
          Wnd.m_pPaintDC->FillSolidRect(CRect(xlast,0,x1,y2), Black);
        Wnd.m_pPaintDC->FillSolidRect(CRect(x1,0,x2,y1), Black);
        Wnd.m_pPaintDC->FillSolidRect(CRect(x1,y1,x2,y2), Cyan);
        xlast=x2;

        if (S.Start()<=CursX && CursX<=S.End())
          CursLd=S.Loading();
        }

      for (int f=0; f<m_Q.m_Feed.GetCount(); f++)
        {
        SpQIO & Fd=m_Q.m_Feed[f];
        if (Fd.Spill.Cd.QMass()>1e-6)
          {
          int x=(int)(Fd.Position()/m_Q.Length()*Wnd.m_ClientRect.right);
          Wnd.m_pPaintDC->FillSolidRect(CRect(x,y0,x+1,Wnd.m_ClientRect.bottom), COLORREF(RGB(255,0,0)));
          }
        }
      
      if (/*m_LBtnDn &&*/ m_MousePt.x>=0 && m_MousePt.x<Wnd.m_ClientRect.right)
        {
        int x=m_MousePt.x;
        Wnd.m_pPaintDC->FillSolidRect(CRect(x,y0,x+1,Wnd.m_ClientRect.bottom), COLORREF(RGB(0,255,0)));
        Wnd.m_pPaintDC->SetTextColor(Green);
        CString S;
        S.Format("%.1f kg/m", CursLd);
        Wnd.m_pPaintDC->TextOut(0,0,S);
        }

      break;
      }
    case MGT_MouseMove:
      m_LBtnDn=(Wnd.m_MouseFlags & MK_LBUTTON);
      if (m_LBtnDn)
        {
        m_MousePt=Wnd.m_MousePt;
        Wnd.m_pWnd->RedrawWindow(0, 0, RDW_INVALIDATE|RDW_UPDATENOW);
        }
      break;
    case MGT_LButtonDown:
      m_LBtnDn=true;
      break;
    case MGT_LButtonUp:
      m_LBtnDn=false;
      break;
    case MGT_RButtonDown:
      m_RBtnDn=true;
      break;
    case MGT_RButtonUp:
      m_RBtnDn=false;
      break;
    }
  return true; 
  };

//==========================================================================
//
//
//
//==========================================================================
