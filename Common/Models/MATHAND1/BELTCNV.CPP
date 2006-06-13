//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#define  __BELTCNV_CPP
#include "beltcnv.h"
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

const int idFeed       = FlwIOId(0);
const int idProd       = FlwIOId(1);
const int EIOId_Supply = ElecIOId(0);

XID xidBeltLength       = ModelXID(1000);
XID xidBeltSpeed        = ModelXID(1001);
XID xidTotSpilt         = ModelXID(1002);
XID xidNWtMtrs          = ModelXID(1003);
XID xidNSections        = ModelXID(1004);
XID xidTotalMass        = ModelXID(1005);
XID xidAvgLoading       = ModelXID(1006);
XID xidShowProf         = ModelXID(1007);
XID xidProfPts          = ModelXID(1008);
XID xidProfDispType     = ModelXID(1009);
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

IMPLEMENT_MODELUNIT(BeltCnv, "BeltCnv", "", Drw_BeltCnv, "Conveyor", "BCV", TOC_ALL|TOC_DYNAMICFLOW|TOC_GRP_TRANSFER|TOC_STD_KENWALT,
                    "Material Transfer:Belt Conveyor",
                    "Belt Conveyor");

BeltCnv::BeltCnv(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MdlNode(pClass_, TagIn, pAttach, eAttach),
  m_Pwr("Supply", this, TOA_Embedded),
  m_BeltSB("PL_FwdRv", this, &m_Pwr),
  m_WtMtrPos(*(new CWtMtrPosArray)),
  m_Spill(eDIO_Spill, this, false, true, "Spill", IOId_Spill2Area, IOId_AreaSpillI),
  m_Vent(eDIO_Vent, this, false, true, "Vent", IOId_Vent2Area, IOId_AreaVentI),
  m_AccIn(eDIO_Accum, this, false, true, "Accumulation"),
  m_AccOut(eDIO_Deplete, this, false, true, "Depletion"),
  m_AccCalc(this, m_AccIn, m_AccOut),
  m_Q("Belt", this, TOA_Embedded, &m_Spill, &m_Vent, &m_AccIn, &m_AccOut)
  {
  m_Pwr.Initialise("525v_A/C");
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

  };

// -------------------------------------------------------------------------

BeltCnv::~BeltCnv()
  {
  delete &m_WtMtrPos;
  }

// -------------------------------------------------------------------------
/*This provides access to the variables of the model and calls the
BuildDataDefn for objects used by the model.*/
void BeltCnv::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);

  //DDB.Page   ("Parms");
  DDB.Text    ("");
  DDB.Visibility(SM_DynBoth|HM_All);
  BuildDataDefnElevation(DDB);
  DDB.Text("");
  DDB.Double  ("BeltSpeed",    "",                DC_Ldt,  "m/s",   xidBeltSpeed,       this, isResult);
  DDB.Visibility();
  DDB.Double  ("Length",       "L",               DC_L,    "m",     xidBeltLength,      this, isParm);
  DDB.Visibility(SM_DynBoth|HM_All);
  DDB.Double  ("MaxSpeed",     "",                DC_Ldt,  "m/s",   &m_MaxVelocity,     this, isParm);
  DDB.Double  ("MaxLoading",   "MaxSpillLoad",    DC_MpL,  "kg/m",  &m_MaxSpillLoading, this, isParm);
  DDB.Double  ("MinLoading",   "",                DC_MpL,  "kg/m",  &m_MinLoading,      this, isParm|InitHidden);
  DDB.Long    ("MaxTurndown",  "",                DC_,     "",      &m_MaxTurnDown,     this, isParm|InitHidden);

  DDB.Visibility(SM_DynBoth|HM_All);
  DDB.Double  ("",             "MaxStallLoad",    DC_M,    "kg",    &m_MaxStallLoading, this, isParm);
  DDB.CheckBox("",             "StartIfStalled",  DC_,     "",      &m_fStartIfStalled, this, isParm);
  DDB.Byte    ("",             "WasRunning",      DC_,     "",      &m_fWasRunning,     this, isResult|InitHidden);

  DDB.Double  ("TotalMass",    "",                DC_M,    "kg",    xidTotalMass,       this, isResult);
  DDB.Double  ("AvgLoading",   "",                DC_MpL,  "kg/m",  xidAvgLoading,      this, isResult);

  if (OrigPrjFileVerNo()<90 && DDB.ForFileSnpScn())
    DDB.String  ("SpillArea",    "",              DC_,     "",      &m_sAreaTag,        this, isParmStopped);
  DDB.Double  ("TotalSpilt",   "",                DC_M,    "kg",    xidTotSpilt,        this, isParm);

  DDB.Page("BeltDrive", DDB_RqdPage);
  DDB.Text    ("Power Draw");
  DDB.Double  ("NoLoadPower",  "",                DC_Pwr,  "kW",    &m_NLPower,         this, isParm);
  DDB.Double  ("FullLoadPower","",                DC_Pwr,  "kW",    &m_FLPower,         this, isParm);
  DDB.Double  ("Power",        "",                DC_Pwr,  "kW",    &m_Power,           this, isResult);

  DDB.Text("");
  m_Pwr.BuildDataDefn(DDB);
  m_BeltSB.BuildDataDefn(DDB, this, "Speed", 1);
  DDB.Visibility();

  DDB.Text("");
  DDB.Long    ("NoWtMtrs",     "",                DC_,     "",      xidNWtMtrs,         this, isParm);
  static DDBValueLst DDB1[] = {
    {QPT_None,        "None" },
    {QPT_FixedPts,    "Fixed Points"},
    {QPT_AllSections, "All Section Pts"},
    {QPT_MinSections, "Min Section Pts"},
    {0}};
  DDB.CheckBox("ShowProfile",  "",                DC_,     "",      xidShowProf,        this, isParm|SetOnChange);
  DDB.Visibility(SM_DynBoth|HM_All, m_fShowProfile);
  DDB.Long    ("ProfDispType", "",                DC_,     "",      xidProfDispType,    this, isParm, DDB1);
  //DDB.Visibility(SM_DynBoth|HM_All, m_fShowProfile && m_ProfDispType==QPT_FixedPts);
  //DDB.Long    ("ProfilePts",   "",                DC_,     "",      xidProfPts,         this, isParm);
  DDB.Visibility(SM_DynBoth|HM_All);
  DDB.Long    ("NSections",    "",                DC_,     "",      xidNSections,       this, isResult);
  if (DDB.BeginStruct(this, "MinSect", 0, DDB_NoPage))
    {
    DDB.Double  ("Length",     "",                DC_Frac, "%",     &m_Q.m_dMinFracSectLen,  this, isParm);
    DDB.Visibility(SM_DynBoth|HM_All, m_Q.m_dMinFracSectLen>0);
    DDB.Double  ("LoadTol",    "",                DC_Frac, "%",     &m_Q.m_dLoadTol,  this, isParm);
    DDB.Double  ("TempTol",    "",                DC_Frac, "%",     &m_Q.m_dTempTol,  this, isParm);
    DDB.Double  ("MFracTol",   "",                DC_Frac, "%",     &m_Q.m_dMFracTol,  this, isParm);
    }
  DDB.EndStruct();

  DDB.Text("");
  BuildDataDefnShowIOs(DDB);


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
        DDB.Double     ("Pos",      "",  DC_L , "m",      xidFeedPos0+i,      this, isParm);
        DDB.Visibility(SM_DynBoth|HM_All);
        DDB.CheckBoxBtn("Limited",  "",  DC_, "",         xidFeedLimited0+i,  this, isParm);
        DDB.Visibility(SM_DynBoth|HM_All, m_Q.FeedLimited(i));
        DDB.Double     ("Capacity", "",  DC_Frac, "%",    xidFeedCapFrac0+i,  this, isParm);
        DDB.Visibility();
        DDB.Double     ("Qm",       "",  DC_Qm , "kg/s",  xidFeedQm0+i,       this, 0);
        DDB.Text("");

        if (DoOld)
          {
          DDB.BeginElement(this, i);
          DDB.Double("Pos",     "",  DC_L , "m",    xidFeedPos0+i,       this, isParm);
          DDB.Double("Qm",     "",  DC_Qm , "kg/s",  xidFeedQm0+i,       this, 0);
          }
        }
  DDB.EndArray();

  DDB.Page("Products", DDB_RqdPage);
  if (DDB.BeginArray(this, "Pr", "BltCnvPr", nPr, 0))
    for (i=0; i<nPr; i++)
      if (DDB.BeginElement(this, Nd_Rmt(PrLst[i])->FullObjTag()))
        {
        DDB.Double("Pos",     "",   DC_L ,     "m",    xidProdPos0+i,       this, isParm);
        DDB.Double("Removal","",    DC_Frac , "%",     xidProdRemove0+i,    this, isParm);
        DDB.Double("Qm",    "",     DC_Qm ,   "kg/s",  xidProdQm0+i,        this, 0);
        DDB.Double("Loss",  "",     DC_Qm ,   "kg/s",  xidProdLoss0+i,      this, 0);

        DDB.Text("");

        if (DoOld)
          {
          DDB.BeginElement(this, i);
          //DDB.TagComment(Nd_Rmt(PrLst[i])->FullObjTag());
          //DDB.BeginElement(this, Nd_Rmt(PrLst[i])->FullObjTag());
          DDB.Double("Pos",     "",   DC_L ,     "m",    xidProdPos0+i,       this, isParm);
          DDB.Double("Removal","",    DC_Frac , "%",     xidProdRemove0+i,    this, isParm);
          DDB.Double("Qm",    "",     DC_Qm ,   "kg/s",  xidProdQm0+i,        this, 0);
          DDB.Double("Loss",  "",     DC_Qm ,   "kg/s",  xidProdLoss0+i,      this, 0);
          }
        }
  DDB.EndArray();

  if (m_fShowProfile)
    DDB.Object(&m_Q.Profile(), this);

  if (SolveDynamicMethod())
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

flag BeltCnv::DataXchg(DataChangeBlk & DCB)
  {
  if (MdlNode::DataXchg(DCB))
    return 1;
  if (m_BeltSB.DataXchg(DCB))
    return 1;
  if (m_Pwr.DataXchg(DCB))
    return 1;
  if (DCB.dwUserInfo==1 && m_Spill.DataXchg(DCB))
    return 1;
  if (DCB.dwUserInfo==2 && m_Vent.DataXchg(DCB))
    return 1;
  if (DCB.dwUserInfo==3 && m_AccIn.DataXchg(DCB))
    return 1;
  if (DCB.dwUserInfo==4 && m_AccOut.DataXchg(DCB))
    return 1;

  switch (DCB.lHandle)
    {
    case xidBeltLength:
      if (DCB.rD)
        m_Q.SetLength(*DCB.rD);
      DCB.D=m_Q.Length();
      return 1;
    case xidBeltSpeed:
      DCB.D=m_MaxVelocity*Range(-1.0, m_BeltSB.Speed(this), 1.0);
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
        m_ProfPts = Range(0L, *DCB.rL, 124L);
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
    default:
      {
      int i=DCB.lHandle-xidFeedPos0;
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

flag BeltCnv::ValidateData(ValidateDataBlk & VDB)
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
    if (m_Q.m_Prod[i].Position()<0.0 || m_Q.m_Prod[i].Position()>m_Q.Length())
      BadProdPosition=1;
  SetCI(2, BadProdPosition);

  flag BadMtrPosition=0;
  for (i=0; i<m_WtMtrPos.GetSize(); i++)
    if (m_WtMtrPos[i]<0.0 || m_WtMtrPos[i]>m_Q.Length())
      BadMtrPosition=1;
  SetCI(3, BadMtrPosition);

  //m_Q.SetSpill(Spills.Find(SpillName()));
  return OK;
  }

//--------------------------------------------------------------------------

void BeltCnv::CollectElectrics(CNodeElectricsArray & TSCA)
  {
  MdlNode::CollectElectrics(TSCA);

  CNodeElectrics &TSP=m_Electrics;
  if (m_Pwr.TermStrip())
    TSP.AddTermStrip(m_Pwr.TermStrip());
  };

//--------------------------------------------------------------------------

void BeltCnv::ConnectElectrics()
  {
  MdlNode::ConnectElectrics();
  if (m_Pwr.TermStrip())
    m_Pwr.ConnectPower();
  };

//---------------------------------------------------------------------------

flag BeltCnv::GetOtherData(FilingControlBlock &FCB)
  {
  return m_Q.GetOtherData(FCB);
  };

//---------------------------------------------------------------------------

flag BeltCnv::PutOtherData(FilingControlBlock &FCB)
  {
  return m_Q.PutOtherData(FCB);
  };

//--------------------------------------------------------------------------

CFlange * BeltCnv::GetFlange(int IOId)
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

void BeltCnv::PostConnect(int IONo)
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

void BeltCnv::PreDisConnect(int IONo)
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

void BeltCnv::SetState(eScdMdlStateActs RqdState)
  {
  MdlNode::SetState(RqdState);
  m_Q.SetState(RqdState);
  switch (RqdState)
    {
    case MSA_PBInit:
    case MSA_Empty:
    case MSA_PreSet:
      m_Q.Empty();
      m_Q.SetTotalSpilt(0.0, 0.0, 0.0, 0.0);
      break;
    case MSA_SteadyState:
      break;
    }
  };

//---------------------------------------------------------------------------

void BeltCnv::StartSolution()
  {
  };

//---------------------------------------------------------------------------

void BeltCnv::ConfigureJoins()
  {
  int i;
  switch (NetMethod())
    {
    case NM_Probal:
      for (i=0; (i<NoFlwIOs()); i++)
        SetIO_Join(i, 0);
      break;
    case NM_Dynamic:
      for (int i = 0; i < NoFlwIOs(); i++)
        SetIO_Open(i, 0, false, ESS_Denied);
      break;
    }
  };

//---------------------------------------------------------------------------

void BeltCnv::StartStep()
  {
  m_MaxSpillLoading = Max(0.01, m_MaxSpillLoading);
  m_Q.SetStepInfo(ICGetTimeInc(), m_MaxVelocity, m_MaxVelocity*Range(-1.0, m_BeltSB.Speed(this), 1.0), m_MaxTurnDown);
  m_Q.SetMaxSpillLoading(m_MaxSpillLoading);
  m_Q.SetMinLoading(m_MinLoading);
  m_AccCalc.Start(0, m_Q);
  };

//--------------------------------------------------------------------------

void BeltCnv::EvalJoinPressures(long JoinMask)
  {
  for (int i = 0; i < NoFlwIOs(); i++)
    Set_IOP_Self(i, AtmosPress(IODatum_Term(i)));
  };

//--------------------------------------------------------------------------

flag BeltCnv::EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo)
  {
  if (m_Spill.EvalFlowEquations(Task, pProps, IONo, FE, LnkNo))
    return true;
  if (m_Vent.EvalFlowEquations(Task, pProps, IONo, FE, LnkNo))
    return true;
  if (m_AccIn.EvalFlowEquations(Task, pProps, IONo, FE, LnkNo))
    return true;
  if (m_AccOut.EvalFlowEquations(Task, pProps, IONo, FE, LnkNo))
    return true;
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

void BeltCnv::EvalProducts(long JoinMask)
  {
  switch (SolveMethod())
    {
    case SM_Direct:
      if (NJoins()>=1)
        Xfer_EvalProducts(0, Joins[0].Pressure(), NULL, NULL, NULL, NULL, NULL);
      #pragma chFIXIT(Correct conveyor output split in ProBal needs implementing)
      break;
    case SM_Inline:
    case SM_Buffered:
      {
      int Prod=0;
      for (int i=0; i<NoFlwIOs(); i++)
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

void BeltCnv::EvalPowerRequired()
  {
  m_Pwr.Zero();
  if (Valid(m_Power))
    m_Pwr.SetPower(m_Power);
  }

//--------------------------------------------------------------------------

void BeltCnv::EvalCtrlActions(eScdCtrlTasks Tasks)
  {
  m_BeltSB.EvalCtrlActions(this);
  }

//--------------------------------------------------------------------------

/*This is called once, after the iteration process and allows the discreet
manipulation of the contents contained by the surge unit.*/

void BeltCnv::EvalDiscrete()
  {

  int Feed=0;
  for (int i=0; i<NoFlwIOs(); i++)
    if (IOId_Self(i)==idFeed)
      m_Q.SetFeed(Feed++, IOQm_In(i), IOConduit(i));

  flag Moved=(m_fWasRunning && (m_Q.TotalMass()<m_MaxStallLoading || m_fStartIfStalled));
  m_Q.DumpProfile(0, "Before", 0);
  m_Q.Advance(ICGetTimeInc(), Moved);//, m_MaxVelocity, m_MaxVelocity*Speed);

  m_fWasRunning = fabs(m_BeltSB.Speed(this))>0.0;

  m_Power=(m_NLPower+(m_FLPower-m_NLPower)* m_Q.AverageLoading()/m_MaxSpillLoading)*Range(-1.0, m_BeltSB.Speed(this), 1.0);

  m_Q.DumpProfile(1, "After", 0);
  m_BeltSB.EvalDiscrete(this);

  m_AccCalc.Complete(0, m_Q, ICGetTimeInc());
  }

//--------------------------------------------------------------------------

void BeltCnv::ClosureInfo()
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

dword BeltCnv::ModelStatus()
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

flag BeltCnv::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case  1: pS="E\tBad Feed Position";                      return 1;
    case  2: pS="E\tBad Prod Position";                      return 1;
    case  3: pS="E\tBad Weightometer Position";              return 1;
    default:
      return MdlNode::CIStrng(No, pS);
    }
  };

//==========================================================================
//
//
//
//==========================================================================
