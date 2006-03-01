//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
#define  __RTBLACKBOX_CPP
#include "crush1.h"
#include "osm.h"
#include "op_db.h"
#include "rtblackbox.h"

//====================================================================================

XID xidCrushModel   = MdlBsXID(10010);

//====================================================================================

//define Input/Output connections
const byte ioidFeed0  = 0;
const byte ioidFeed1  = 1;
const byte ioidFeed2  = 2;
const byte ioidProd0  = 3;
const byte ioidProd1  = 4;
const byte ioidProd2  = 5;
const byte ioidProd3  = 6;
const byte ioidProd4  = 7;

const int MaxFeedIn = 3;
const int MaxProdOut = 5;

static IOAreaRec RTBlackBoxIOAreaList[] = {
  {"",              "Feed0"    , ioidFeed0, LIO_In ,  nc_MLnk, 1, 10, dwIOIsBuffer|IOGRP(1), 0.0F},
  {"",              "Feed1"    , ioidFeed1, LIO_In ,  nc_MLnk, 0, 10, dwIOIsBuffer|IOGRP(1), 0.0F},
  {"",              "Feed2"    , ioidFeed2, LIO_In ,  nc_MLnk, 0, 10, dwIOIsBuffer|IOGRP(1), 0.0F},
  {"",              "Product0" , ioidProd0, LIO_Out,  nc_MLnk, 1,  1, dwIOIsBuffer|IOGRP(1), 0.0F},
  {"",              "Product1" , ioidProd1, LIO_Out,  nc_MLnk, 0,  1, dwIOIsBuffer|IOGRP(1), 0.0F},
  {"",              "Product2" , ioidProd2, LIO_Out,  nc_MLnk, 0,  1, dwIOIsBuffer|IOGRP(1), 0.0F},
  {"",              "Product3" , ioidProd3, LIO_Out,  nc_MLnk, 0,  1, dwIOIsBuffer|IOGRP(1), 0.0F},
  {"",              "Product4" , ioidProd4, LIO_Out,  nc_MLnk, 0,  1, dwIOIsBuffer|IOGRP(1), 0.0F},
  {NULL} };
           
//define default graphic symbol
long Drw_RTBlackBox[] = { DD_Poly, -10,-9, -10,9, 10,9, 10,-9, -10,-9,
                         DD_End };

//===========================================================================

IMPLEMENT_MODELUNIT(RTBlackBox, "RTBlackBox", "", Drw_RTBlackBox, "RTBB", TOC_ALL|TOC_GRP_SIZEDST|TOC_RTTS, 
                    "RTTS Black Box", 
                    "Rio Tinto Technical Services Black Box Models")

RTBlackBox::RTBlackBox (pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MdlNode(pClass_, TagIn, pAttach, eAttach)
  {
  AttachClassInfo(nc_Process, RTBlackBoxIOAreaList);

  bOnLine = 1;
  m_pMethod = NULL;

  if (!TheVectorDef.bDoneInit)
    {
    MSpecie SD;
    TheVectorDef.Species.SetSize(SDB.No());
    for (int s=0; s<SDB.No(); s++)
      {
      SD.sTag = SDB[s].Tag();
      SD.iPhase = SDB[s].mOcc;
      SD.dMW = SDB[s].MoleWt();
      //SD.dRho = SDB[s].; TODO
      TheVectorDef.Species[s] = SD;
      }
    TheVectorDef.bDoneInit = 1;
    }

  if (!TheSieveDefList.bDoneInit)
    {
    TheSieveDefList.SieveDefs.SetSize(SD_Defn.NDistributions());
    for (int d=0; d<SD_Defn.NDistributions(); d++)
      {
      CSD_DistDefn &DD=*SD_Defn.GetDist(d);
      const int NumSizes = DD.NIntervals(); //number of sieve size intervals
      const int NumComps = DD.NPriIds(); //number of components with size data
      MSieveDef &SD=TheSieveDefList.SieveDefs[d];
      SD.Init(NumSizes);
      //convert sieve series from m to mm and from largest to smallest sizes...
      for (int i=0; i<NumSizes; i++)
        SD.pData[i] = DD.Intervals.GetAt(NumSizes-i-2) * 1000.0;
      //SD.pData[NumSizes-1] = 0.0; //ensure "smallest" size is zero
      SD.SpSzIndexes.SetSize(NumComps);
      for (i=0; i<NumComps; i++)
        SD.SpSzIndexes[i] = DD.PriSzId(i);
      }
    TheSieveDefList.bDoneInit = 1;
    }

  m_pMethod = MBaseMethodDef::First()->Construct();
  m_pMethod->Init(m_pMethod->MethodDef);
  m_pMethod->MethodDef.iInputTypesRqd = Range(1, m_pMethod->MethodDef.iInputTypesRqd, MaxFeedIn);
  m_pMethod->MethodDef.iOutputTypesRqd = Range(1, m_pMethod->MethodDef.iOutputTypesRqd, MaxProdOut);
  }

//--------------------------------------------------------------------------

RTBlackBox::~RTBlackBox()
  {
  }

//--------------------------------------------------------------------------

void RTBlackBox::BuildDataDefnForMethod(DataDefnBlk & DDB)
  {

  DDB.BeginObject(this, "MC", "MethCalc", NULL, DDB_RqdPage);
  //m_pMethod->InitialiseFields();
  m_pMethod->Fields.RemoveAll();
  m_pMethod->BuildDataFields();
  for (int i=0; i<m_pMethod->Fields.GetSize(); i++)
    {
    MDataField & DF = m_pMethod->Fields[i];
    DDEF_Flags Flgs = 0;
    if (DF.dwFlags & MF_RESULT)         Flgs |= DDEF_RESULT;
    if (DF.dwFlags & MF_PARAMETER)      Flgs |= DDEF_PARAM;
    if (DF.dwFlags & MF_PARAM_STOPPED)  Flgs |= (DDEF_PARAM|DDEF_PARAMSTOPPED);
    if (DF.dwFlags & MF_SET_ON_CHANGE)  Flgs |= DDEF_SETONCHANGE;
    if (DF.dwFlags & MF_NO_FILING)      Flgs |= (DDEF_NOFILE|DDEF_NOSNAPSHOT|DDEF_NOSCENARIO);
    if (DF.dwFlags & MF_NO_VIEW)        Flgs |= DDEF_NOVIEW;
    if (DF.dwFlags & MF_INIT_HIDDEN)    Flgs |= DDEF_HIDDEN;
    if (DF.dwFlags & MF_NAN_OK)         Flgs |= DDEF_NAN_OK;
    char* pNm = (char*)(LPCSTR)DF.Name;
    switch (DF.iFieldType)
      {
      case MT_BOOL:   DDB.CheckBox(pNm, "", DC_, "", (unsigned char*)DF.pData, this, Flgs); break;
      case MT_LONG:   DDB.Long(pNm, "", DC_, "", (long*)DF.pData, this, Flgs); break;
      case MT_DOUBLE: DDB.Double (pNm, "", DC_, "", (double*)DF.pData, this, Flgs); break;
      case MT_TEXT:   DDB.Text(pNm); break;
      }
    }
  DDB.EndObject();
  }

//--------------------------------------------------------------------------

void RTBlackBox::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);
  DDB.Text("");
  DDB.CheckBox("On",           "",    DC_,      "",    &bOnLine,        this, isParm|SetOnChange);
  //DDB.Byte    ("ModelMethod",   "",    DC_,      "",    xidModelMethod,   this, isParmStopped|SetOnChange, DDB0);

  DDB.Visibility(DYNMODE);
  DDB.Text   ("Datum");
  DDB.Double ("Datum",        "Z",    DC_L    , "m",    &dDatum,        this, isResult|0      | DYNMODE);
  DDB.Double ("Datum_Rqd",    "Z_Rqd",DC_L    , "m",    &dDatum_Rqd,    this, isParmStopped | DYNMODE|NAN_OK);
  DDB.Text("");
  BuildDataDefnShowIOs(DDB);

  DDB.Visibility();
  if (m_pMethod)
    BuildDataDefnForMethod(DDB);
  DDB.EndStruct();
  }

//--------------------------------------------------------------------------

flag RTBlackBox::DataXchg(DataChangeBlk & DCB)
  {
  /*switch (DCB.lHandle)
    {
    }*/

  if (MdlNode::DataXchg(DCB)) 
    return 1;

  return 0;
  }

//--------------------------------------------------------------------------

flag RTBlackBox::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK = MdlNode::ValidateData(VDB);
  return OK;
  }

//--------------------------------------------------------------------------

flag RTBlackBox::PreStartCheck()
  {
  flag Ok=1;
  /*flag Ok=(some test);
  if (!Ok)
    {
    LogError(FullObjTag(), 0, "Some error message");
    return Ok;
    }*/

  return Ok;
  };

//--------------------------------------------------------------------------

void RTBlackBox::EvalProducts()
  {
  //if (NoFlwIOs()<2)
  //  return;
  const int ioProd0 = IOWithId_Self(ioidProd0);
  const int ioFeed0 = IOWithId_Self(ioidFeed0);

  if (ioProd0<0 || ioFeed0<0)
    return;

  ASSERT_RDB(m_pMethod, "!!!");
  
  if (!m_pMethod->DoneIOInit())
    m_pMethod->InitIO();

  ClrCI(1);
  ClrCI(2);
  ClrCI(3);
  ClrCI(4);

  if (!bOnLine)
    {
    StkSpConduit Mixture("Mixture", chLINEID(), this);
    SigmaQInPMin(Mixture(), som_ALL, Id_2_Mask(ioidFeed0)|Id_2_Mask(ioidFeed1)|Id_2_Mask(ioidFeed2));
    SpConduit & QPr = *IOConduit(ioProd0);
    QPr.QCopy(Mixture());
    return;
    }

  int i,s;
  StkSpConduit Mixture("Mixture", chLINEID(), this);

  for (i=0; i<MaxFeedIn; i++)
    {
    if (i>=m_pMethod->InputCnt() && IOWithId_Self(ioidFeed0+i)>=0)
      SetCI(1, true);
    if (i<m_pMethod->InputCnt())
      {
      SigmaQInPMin(Mixture(), som_ALL, Id_2_Mask(ioidFeed0+i));
      MVector & QI = m_pMethod->Input(i);
      for (int s=0; s<SDB.No(); s++)
        QI.pMasses[s] = Mixture().Qm(s);
      QI.T = Mixture().Temp();
      QI.P = Mixture().Press();
      if (i==0 && m_pMethod->MethodDef.bSizeDataRqd)
        {
        m_pMethod->pSieveDefUsed = NULL;
        SpModel * pMdl = Mixture().Model();
        SQSzDist1 * pSz = SQSzDist1::Ptr(pMdl, true);
        if (pSz==NULL || !pSz->DistributionsExist())
          {
          //if (QFd.QMass()>UsableMass)
          SetCI(4);
          }
        else
          {//
          ASSERT_RDB(pSz->iDistUsed>=0 && pSz->iDistUsed<TheSieveDefList.Count(), "Why not???");
          m_pMethod->pSieveDefUsed = TheSieveDefList.SieveDef(pSz->iDistUsed);
          }
        }
      }
    }

  MVector & QI0 = m_pMethod->Input(0);
  for (i=0; i<MaxProdOut; i++)
    {
    if (i>=m_pMethod->OutputCnt() && IOWithId_Self(ioidProd0+i)>=0)
      SetCI(2, true);
    if (i<m_pMethod->OutputCnt())
      {
      MVector & QO = m_pMethod->Output(i);
      if (i==0)
        QO=QI0;
      else
        QO.SetToZero();
      }
    }

  m_pMethod->EvalProducts();

  for (i=0; i<m_pMethod->OutputCnt(); i++)
    {
    MVector & QO = m_pMethod->Output(i);
    for (s=0; s<SDB.No(); s++)
      Mixture().SetQm(s, QO.pMasses[s]);
    Mixture().SetTempPress(QO.T, QO.P);
    const int ioP = IOWithId_Self(ioidProd0+i);
    if (ioP<0)
      SetCI(3, true);
    else
      {
      SpConduit & QPr = *IOConduit(ioP);
      QPr.QCopy(Mixture());
      }
    }

  }

//--------------------------------------------------------------------------

void RTBlackBox::EvalCtrlActions()
  {
  MdlNode::EvalCtrlActions();
  }

//---------------------------------------------------------------------------

void RTBlackBox::ConfigureJoins()
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
        SetIO_Open(i, 0, false, ESS_Denied);
      break;
    }
  }

//--------------------------------------------------------------------------

dword RTBlackBox::ModelStatus()
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

flag RTBlackBox::CIStrng(int No, pchar & pS)
  {
  switch (No-CBContext())
    {
    case  1: pS="E\tIncorrect feed stream connection!";  return 1;
    case  2: pS="E\tIncorrect product stream connection!";  return 1;
    case  3: pS="E\tRequired product stream not connected!";  return 1;
    case  4: pS="W\tNo Size distributions defined";  return 1;
    default:                                               
      return MdlNode::CIStrng(No, pS);
    }
  }

//===========================================================================

