//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include <stdafx.h>
#include "sc_defs.h"
#define  __RTTSCRUSH_CPP
#include "crush1.h"
#include "osm.h"
#include "op_db.h"
#include "oreprops.h"
#include "rttscrush.h"
//#include "optoff.h"

//====================================================================================

const byte CM_General    = 0;
const byte CM_Model400   = 1;

XID xidCrushModel   = MdlBsXID(10010);
XID xidNumComps     = MdlBsXID(10011);
XID xidM400_Ecs     = MdlBsXID(10020);

//====================================================================================

Model400Helper::Model400Helper()
  {
  feedData = NULL;
  sizeSeries = NULL;
  PrevNumSizes = 0;
  iNumComps = 0;
  pT10 = NULL;
  //K1  = 77.03;
  //K2  = 106.27;
  //K3  = 2.3;
  //T10 = 14.33;
  K1  = 0.0;
  K2  = 50.0;
  K3  = 2.3;
  //T10 = 10.0;
  M0 =  1.4936;
  M1 =  0.9543;
  M2 = -0.3856;
  M3 = 23.7200;
  ECS = 0.1;
  }

//--------------------------------------------------------------------------

Model400Helper::~Model400Helper()
  {
  if (feedData)
    delete []feedData;
  if (sizeSeries)
    delete []sizeSeries;
  if (pT10)
    delete []pT10;
  }

//--------------------------------------------------------------------------

int Model400Helper::SetNumComps(int NewSize)
  {
  if (iNumComps!=NewSize)
    {
    double* pNewT10 = NULL;
    if (NewSize>0)
      {
      pNewT10 = new double[NewSize];
      for (int i=0; i<iNumComps && i<NewSize; i++)
        pNewT10[i] = pT10[i];
      for (; i<NewSize; i++)
        pNewT10[i] = 10.0;
      }
    if (pT10)
      delete []pT10;
    pT10 = pNewT10;
    iNumComps = NewSize;
    }
  return iNumComps;
  }

//--------------------------------------------------------------------------

void Model400Helper::BuildDataDefn(DataDefnBlk & DDB, pTaggedObject This, flag Vis)
  {
  DDEF_Flags Old = DDB.GetVisibility();
  DDB.Visibility(NSHM_All, Vis);
  DDB.Text(" ");
  DDB.Double("K_1",     "",   DC_,     "",       &K1,   This, isParm);
  DDB.Double("K_2",     "",   DC_,     "",       &K2,   This, isParm);
  DDB.Double("K_3",     "",   DC_,     "",       &K3,   This, isParm);
  DDB.Double("M_0",     "",   DC_,     "",       &M0,   This, isParm|InitHidden);
  DDB.Double("M_1",     "",   DC_,     "",       &M1,   This, isParm|InitHidden);
  DDB.Double("M_2",     "",   DC_,     "",       &M2,   This, isParm|InitHidden);
  DDB.Double("M_3",     "",   DC_,     "",       &M3,   This, isParm|InitHidden);
  DDB.Double("Ecs",     "",   DC_,     "", xidM400_Ecs, This, isParm);
  Strng Tg;
  SpecieIter I(som_Sol);
  for (int SpId=-1; I.Loop(SpId); )
    {
    if (ODB[SpId].OK())
      {
      //int i = SD_Defn.FindIndex(0, SpId);
      int i= SD_Defn.GetDist(0)->FindPriSzIdIndex(SpId);
      if (i>=0)
        {
        Tg.Set("%s_T10", ODB[SpId].Material());//SDB[SpId].CmpTag());
        DDB.Double(Tg(),    "",   DC_,     "",       &pT10[i],  This, isResult|noFile|noSnap);
        }
      }
    }
  DDB.SetVisibility(Old);
  }

//--------------------------------------------------------------------------

flag Model400Helper::DataXchg(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
    case xidM400_Ecs:
      if (DCB.rD)
        {
        ECS=*DCB.rD;
        RecalcT10();
        }
      DCB.D=ECS;
      return 1;
    }

  return 0;
  }

//--------------------------------------------------------------------------

void Model400Helper::RecalcT10()
  {
  for (int i=0; i<iNumComps && i<SD_Defn.NPriIds(0); i++)
    {
    int SpId = SD_Defn.GetDist(0)->PriSzId(i);
    ASSERT(SpId>=0);
    RT_OreProps* OreProps = (RT_OreProps*)GetGlobalOther("RioTintoOreProps");
    if (ODB[SpId].OK() && OreProps)
      {
      const int AIndex = OreProps->FindOtherProp("A");
      const int bIndex = OreProps->FindOtherProp("b");
      if (AIndex>=0 && bIndex>=0)
        {
        RT_OreProp& Prop = OreProps->OrePropByIndex(SpId);
        const double A = Prop.OtherProp(AIndex);
        const double b = Prop.OtherProp(bIndex);
        //const double t10 = A * Exps(1.0 - b*ECS); kga 03/2006 this is wrong! changed to equation below
        const double t10 = A * (1.0 - Exps(-b * ECS));
        if (t10<1.0e-6)
          {
          LogWarning("Model400", 0, "T10<1e-6 due to bad ore properties A and b parameters, assume T10 = 10");
          pT10[i] = 10.0;
          }
        else
          pT10[i] = t10;
        }
      else
        {
        LogWarning("Model400", 0, "Ore properties A and b parameters not found, assume T10 = 10");
        pT10[i] = 10.0;
        }
      }
      //pT10[i] = ODB[SpId].T10(ECS);
      //pT10[i] = ODB[SpId].A() * Exps(1.0 - ODB[SpId].b()*Ecs);
    else
      {
      LogWarning("Model400", 0, "Ore properties A and b parameters not found, assume T10 = 10");
      pT10[i] = 10.0;
      }
    }
  }

//--------------------------------------------------------------------------

void Model400Helper::DemoTest()
  {
  #ifndef _RELEASE
  //DEMO code from RTD...

  // Demo Data Set

  const int NUM_COMPS = 1;            // Single ore component
  const int NUM_SIZES = 33;

  double sizeSeries[NUM_SIZES] = {
    1200.000,
    848.528,
    600.000,
    424.264,
    300.000,
    212.132,
    150.000,
    106.066,
    75.000,
    45.000,
    37.000,
    25.400,
    19.000,
    16.000,
    13.200,
    9.500,
    6.700,
    4.750,
    3.350,
    2.360,
    1.720,
    1.180,
    0.850,
    0.600,
    0.425,
    0.300,
    0.212,
    0.150,
    0.106,
    0.075,
    0.053,
    0.038,
    0.000
  };

  double feedData[NUM_SIZES] = {
    0.000,
    0.000,
    0.000,
    0.000,
    0.000,
    0.000,
    0.000,
    203.118,
    207.678,
    255.051,
    97.604,
    72.853,
    65.205,
    37.043,
    28.013,
    37.578,
    39.187,
    33.278,
    28.385,
    24.455,
    19.653,
    19.294,
    15.720,
    14.290,
    12.797,
    11.588,
    10.447,
    9.435,
    8.601,
    7.806,
    7.135,
    6.253,
    52.533
  };

  const double _K1  = 77.03;
  const double _K2  = 106.27;
  const double _K3  = 2.3;
  const double _T10 = 14.33;

  //-- Build a feed Stream ------------------------------------------------
  OSM_Type feedType;
  feedType.setSizes( NUM_SIZES, (double*) sizeSeries );
  feedType.setComponents( NUM_COMPS );

  //-- Setup a feed stream ------------------------------------------------

  OSM_Stream feed( feedType, feedData );  // nominate its type and data

  //-- Build and calculate a crusher model --------------------------------

  OSM_Model400 crush1( feedType );             // dimension according to feedType

  crush1.setFeed( feed );                      // crusher connected to feed

  // configure crusher parameters
  crush1.setParams(_K1, _K2, _K3);
  crush1.setT10(_T10);
  //crush1.setAppearance(M0, M1, M2, M3);

  crush1.calculate( );                        // calculate crusher

  const OSM_Stream& product = crush1.product();   // access product Stream
  const double Feed_tph = feed.sum();
  const double Product_tph = product.sum();

  //-- Output results -----------------------------------------------------
  dbgpln(" ++++++++++ RTD Model400 Crusher test ++++++++++ ");
  dbgpln("K1:%.5f  K2:%.5f  K3:%.5f", _K1, _K2, _K3);
  dbgpln("T10:%.5f", _T10);
  dbgpln("");
  dbgpln("  #%10s%10s%10s : %11s%10s", "retained", "feed", "product", "feed%%", "product%");

  for( int i=0; i<feed.nSize(); i++ )
    {
    dbgpln("%3i%10.3f%10.3f%10.3f : %10.4f%10.4f", i, feed.sieve()[i], feed[0][i], crush1.product()[0][i], feed[0][i]/Feed_tph*100.0, crush1.product()[0][i]/Product_tph*100.0);
    }

  dbgpln("");
  dbgpln("   %10s%10.3f%10.3f", "tph", Feed_tph, Product_tph);
  dbgpln(" ++++++++++ RTD Model400 Crusher test DONE ++++++++++ ");
  #endif
  }

//--------------------------------------------------------------------------

BOOL Model400Helper::CrushProduct(SpConduit & QFd, SpConduit & Disch)
  {
  if (0) //set this to 1 to get debug file for test case
    DemoTest();

  //This can easily be made significantly more efficient, current
  //implementation uses files provided as close to "as-is" as possible.

  //--- get SysCAD info into format to be used by RTD JK type crusher model------
  //feed stream...
  SQSzDist1 & FdSz = *SQSzDist1::Ptr(QFd.Model());
  CSD_Distribution & FdDst = FdSz.Dist(FdSz.iDistUsed);

  //product/discharge stream...
  SQSzDist1 & DsSz = *SQSzDist1::Ptr(Disch.Model());
  CSD_Distribution & DsDst = DsSz.Dist(DsSz.iDistUsed);
  ASSERT_RDB(FdSz.iDistUsed==DsSz.iDistUsed, "Should use same sieve series!", __FILE__, __LINE__);

  //Other data...
  const double QmSolTtl = QFd.QMass(som_Sol) / 1000.0 * 3600.0; // total mass flow converted from kg/s to t/h
  const int num_sizes = FdDst.NIntervals(); //number of sieve size intervals
  const int NumComps = FdDst.NPriIds(); //number of components with size data

  //setup data structures...
  if (feedData==NULL || sizeSeries==NULL || PrevNumSizes!=num_sizes)
    {
    if (feedData)
      {
      delete []feedData;
      feedData = NULL;
      }
    if (sizeSeries)
      {
      delete []sizeSeries;
      sizeSeries = NULL;
      }
    PrevNumSizes = num_sizes;
    if (PrevNumSizes>0)
      {
      feedData = new double[PrevNumSizes];
      sizeSeries = new double[PrevNumSizes];
      }
    }

  //convert sieve series from m to mm and from largest to smallest sizes...
  for (int i=0; i<num_sizes-1; i++)
    sizeSeries[i] = FdDst.Intervals().GetAt(num_sizes-i-2) * 1000.0;
  sizeSeries[num_sizes-1] = 0.0; //ensure "smallest" size is zero

  //now, for each component...
  for (int c=0; c<NumComps; c++)
    {
    CSD_SpDist & FdS = *(FdDst.PriSp[c]); //feed stream for this component
    CSD_SpDist & DsS = *(DsDst.PriSp[c]); //discharge stream for this component
    const int NumSecComps = FdDst.NSecIds(c); //number of other components sharing this size data
    //for now, assume NumSecComps==1
    const int SpId = FdDst.SzId(c, 0);
    const double QmSp = QFd.VMass[SpId] / 1000.0 * 3600.0; // component mass flow converted from kg/s to t/h

    if (QmSp>1.0e-9)
      {
      //convert size data from fractions to t/h and from largest to smallest sizes...
      for (i=0; i<num_sizes; i++)
        feedData[i] = (FdS.FracPass[num_sizes-i-1] * QmSp);

      //-- Setup the flowsheet stream material type ---------------------------
      const int NUM_COMPS = 1; // Single ore component (ie one at a time)
      OSM_Type feedType;
      feedType.setSizes(num_sizes, sizeSeries);
      feedType.setComponents(NUM_COMPS);

      //-- Setup a feed stream ------------------------------------------------
      OSM_Stream feed(feedType, feedData);  // nominate its type and data

      //-- Build and calculate a crusher model --------------------------------
      OSM_Model400 crush1(feedType);            // dimension according to feedType
      crush1.setFeed(feed);                     // crusher connected to feed

      // set crusher parameters
      crush1.setParams(K1, K2, K3);
      crush1.setT10(pT10[c]);
      crush1.setAppearance(M0, M1, M2, M3);

      crush1.calculate();                       // calculate crusher

      const OSM_Stream& product = crush1.product();   // access product Stream

      //--- get product info into SysCAD format--------------------------------
      //convert product size data from t/h to fractions and from smallest to largest sizes...
      for (i=0; i<num_sizes; i++)
        DsS.FracPass[num_sizes-i-1] = (crush1.product()[0][i] / QmSp);
      }
    else
      {
      for (i=0; i<num_sizes; i++)
        DsS.FracPass[i] = FdS.FracPass[i];
      }

    //OreProp & OP = ODB[SpId];
    //if (OP.OK())
    //  LogNote(OP.Material(), 0, "%i) %s: %.2gt/h Fe:%g  %s", c, SDB[SpId].Tag(), QmSp, OP.TheorFe(), OP.Description());
    //else
    //  LogWarning(SDB[SpId].Tag(), 0, "NOT OK %i  SpId:%i", c, SpId, OP.Tag());
    }

  return true;
  }

//--------------------------------------------------------------------------

BOOL Model400Helper::CrushProductOne(SpConduit & QFd, SpConduit & Disch)
  {
  if (0) //set this to 1 to get debug file for test case
    DemoTest();

  //Assumes only one solid specie with size distribution info!
  const int NUM_COMPS = 1;            // Single ore component

  //This can easily be made significantly more efficient, current
  //implementation uses files provided as close to "as-is" as possible.

  //--- get SysCAD info into format to be used by RTD JK type crusher model------
  //feed stream...
  SQSzDist1 & FdSz = *SQSzDist1::Ptr(QFd.Model());
  CSD_Distribution & FdDst = FdSz.Dist(0);
  CSD_SpDist & FdS = *(FdDst.PriSp[0]);
  double QmSolTtl = QFd.QMass(som_Sol) / 1000.0 * 3600.0; // total mass flow converted from kg/s to t/h

  //product/discharge stream...
  SQSzDist1 & DsSz = *SQSzDist1::Ptr(Disch.Model());
  CSD_Distribution & DsDst = DsSz.Dist(0);
  CSD_SpDist & DsS = *(DsDst.PriSp[0]);

  const int num_sizes = FdDst.NIntervals(); //number of sieve size intervals

  if (feedData==NULL || sizeSeries==NULL || PrevNumSizes!=num_sizes)
    {
    if (feedData)
      {
      delete []feedData;
      feedData = NULL;
      }
    if (sizeSeries)
      {
      delete []sizeSeries;
      sizeSeries = NULL;
      }
    PrevNumSizes = num_sizes;
    if (PrevNumSizes>0)
      {
      feedData = new double[PrevNumSizes];
      sizeSeries = new double[PrevNumSizes];
      }
    }

  //convert sieve series from m to mm and from largest to smallest sizes...
  for (int i=0; i<num_sizes-1; i++)
    sizeSeries[i] = FdDst.Intervals().GetAt(num_sizes-i-2) * 1000.0;
  sizeSeries[num_sizes-1] = 0.0; //ensure "smallest" size is zero
  //convert size data from fractions to t/h and from largest to smallest sizes...
  for (i=0; i<num_sizes; i++)
    feedData[i] = (FdS.FracPass[num_sizes-i-1] * QmSolTtl);

  //-- Setup the flowsheet stream material type ---------------------------
  OSM_Type feedType;
  feedType.setSizes(num_sizes, sizeSeries);
  feedType.setComponents(NUM_COMPS);

  //-- Setup a feed stream ------------------------------------------------
  OSM_Stream feed(feedType, feedData);  // nominate its type and data

  //-- Build and calculate a crusher model --------------------------------
  OSM_Model400 crush1(feedType);            // dimension according to feedType
  crush1.setFeed(feed);                     // crusher connected to feed

  // set crusher parameters
  crush1.setParams(K1, K2, K3);
  crush1.setT10(pT10[0]);
  crush1.setAppearance(M0, M1, M2, M3);

  crush1.calculate();                       // calculate crusher

  const OSM_Stream& product = crush1.product();   // access product Stream

  //--- get product info into SysCAD format--------------------------------
  //convert product size data from t/h to fractions and from smallest to largest sizes...
  for (i=0; i<num_sizes; i++)
    DsS.FracPass[num_sizes-i-1] = (crush1.product()[0][i] / QmSolTtl);

  return true;
  }

//====================================================================================

//define Input/Output connections
const byte ioidFeed   = 0;
const byte ioidProd   = 1;

static IOAreaRec RTCrusherIOAreaList[] =
  {{"",              "Feed"      , ioidFeed,  LIO_In ,  nc_MLnk, 1, 20, IOIsBuffer|IOGRP(1), 0.0F},
   {"",              "Product"   , ioidProd,  LIO_Out,  nc_MLnk, 1,  1, IOIsBuffer|IOGRP(1), 0.0F},
   {NULL}};

//define default graphic symbol
static double Drw_RTCrusher[] = { DD_Poly, -10,-8, -10,8, 10,8, 10,-8, -10,-8,
                         DD_Poly, -8,8, -4,-8,
                         DD_Poly, -6,8, -2,-8,
                         DD_Poly,  6,8,  2,-8,
                         DD_Poly,  8,8,  4,-8,
                         DD_End };

//===========================================================================

IMPLEMENT_MODELUNIT(RTCrusher, "RTCrusher", "", Drw_RTCrusher, "SizeAlteration", "CR", TOC_PROBAL|TOC_DYNAMICFLOW|TOC_GRP_SIZEDST|TOC_RTTS,
                    "Size Alteration:RTTS Crusher",
                    "Rio Tinto Technical Services Crusher Models")

RTCrusher::RTCrusher (pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MdlNode(pClass_, TagIn, pAttach, eAttach),
  CB(&CrushBlkClass, "CB", this, TOA_Embedded),
  Disch("Disch", this, TOA_Embedded)
  {
  AttachClassInfo(nc_Process, RTCrusherIOAreaList);

  bOnLine = 1;
  iCrushModel = CM_General;
  iNumComps = 0;
  }


//--------------------------------------------------------------------------

RTCrusher::~RTCrusher()
  {
  }

//--------------------------------------------------------------------------

int RTCrusher::SetNumComps(int NewSize)
  {
  iNumComps = NewSize;
  M400.SetNumComps(NewSize);
  return iNumComps;
  }

//--------------------------------------------------------------------------

void RTCrusher::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);
  DDB.Text("");
  static DDBValueLst DDB0[]={
    {(int)CM_General, "General"},
    {(int)CM_Model400, "Model400" },
    {0}};
  DDB.CheckBox("On",           "",    DC_,      "",    &bOnLine,        this, isParm|SetOnChange);
  DDB.Byte    ("CrushModel",   "",    DC_,      "",    xidCrushModel,   this, isParmStopped|SetOnChange, DDB0);
  DDB.Long    ("NumComps",     "",    DC_,      "",    xidNumComps,     this, 0|InitHidden);

  M400.BuildDataDefn(DDB, this, (iCrushModel==CM_Model400));
  DDB.Visibility();

  DDB.Text("");
  DDB.Double("Density",        "Rho",       DC_Rho,   "kg/m^3", xidRho,      this, isResult|0);
	DDB.Visibility(NM_Dynamic|SM_All|HM_All);
  BuildDataDefnElevation(DDB);
  CB.Add_StandardDataDefn(DDB, (iCrushModel==CM_General));
  DDB.Visibility();
  DDB.Text("");
  BuildDataDefnShowIOs(DDB);

  CB.Add_ObjectDataDefn(DDB, (iCrushModel==CM_General));

  DDB.Object(&Disch,     this, NULL, NULL, DDB_RqdPage);

  DDB.EndStruct();
  }

//--------------------------------------------------------------------------

flag RTCrusher::DataXchg(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
    case xidCrushModel:
      if (DCB.rB)
        iCrushModel=*DCB.rB;
      DCB.B=iCrushModel;
      return 1;
    case xidNumComps:
      if (DCB.rL)
        {
        if (*DCB.rL!=iNumComps)
          StructureChanged(this);
        SetNumComps(Max(0L, *DCB.rL));
        }
      DCB.L = iNumComps;
      return True;
    }

  if (CB.DataXchg(DCB))
    return 1;

  if (M400.DataXchg(DCB))
    return 1;

  if (MdlNode::DataXchg(DCB))
    return 1;

  return 0;
  }

//--------------------------------------------------------------------------

flag RTCrusher::ValidateData(ValidateDataBlk & VDB)
  {
  if (SD_Defn.DistExists(0) && SD_Defn.NPriIds(0)>0 && iNumComps!=SD_Defn.NPriIds(0))
    {
    SetNumComps(SD_Defn.NPriIds(0));
    M400.RecalcT10();
    StructureChanged(this);
    }
  flag OK = MdlNode::ValidateData(VDB);
  return OK;
  }

//--------------------------------------------------------------------------

flag RTCrusher::PreStartCheck()
  {
  flag Ok=1;
  /*flag Ok=(some test);
  if (!Ok)
    {
    LogError(FullObjTag(), 0, "Some error message");
    return Ok;
    }*/

  M400.RecalcT10();
  return Ok;
  };

//--------------------------------------------------------------------------

void RTCrusher::EvalProducts(CNodeEvalIndex & NEI)
  {
  //if (NIOs<2)
  //  return;
  const int ioProd = IOWithId_Self(ioidProd);
  const int ioFeed = IOWithId_Self(ioidFeed);

  if (ioProd<0 || ioFeed<0)
    return;

  //SpConduit & QFd = *IOConduit(ioFeed);
  SpConduit & QPr = *IOConduit(ioProd);

  QPr.QZero();
  SigmaQInPMin(Disch, som_ALL, Id_2_Mask(ioidFeed)); //sum of all input streams
  BOOL ValidData = (Disch.QMass(som_Sol)>1.0e-6);
  ClrCI(1);
  if (bOnLine)
    {
    SpModel * pMdl = Disch.Model();
    //SQSzDist1 * pSz = SQSzDist1::Ptr(pMdl, false);
    SQSzDist1 * pSz = SQSzDist1::Ptr(pMdl, true);
    if (pSz==NULL || !pSz->DistributionsExist())
      {
      if (Disch.QMass()>UsableMass)
        SetCI(1);
      ValidData = false;
      }
    }
  else
    {
    ValidData = false;
    }
  if (ValidData)
    {
    SpConduit Sd("Sd", this, TOA_Free); //working copy
    Sd.QCopy(Disch);
    ValidData = CB.IsValidData(Sd);
    if (ValidData)
      {
      switch (iCrushModel)
        {
        case CM_General  : CB.CrushProduct(Sd, Disch); break;
        case CM_Model400 : M400.CrushProduct(Sd, Disch); break;
        }
      }
    }
  QPr.QCopy(Disch);
  }

//--------------------------------------------------------------------------

void RTCrusher::EvalCtrlActions(eScdCtrlTasks Tasks)
  {
  MdlNode::EvalCtrlActions(Tasks);
  }

//---------------------------------------------------------------------------

void RTCrusher::ConfigureJoins()
  {
  Init_NoJoins(1);
  int i;
  switch (NetMethod())
    {
    case NM_Probal:
      for (i=0; (i<NoFlwIOs()); i++)
        SetIO_Join(i, 0);
      break;
    case NM_Dynamic:
      for (i=0; (i<NoFlwIOs()); i++)
        SetIO_Open(i, 0, false, ESS_Denied);
      break;
    }
  }

//--------------------------------------------------------------------------

dword RTCrusher::ModelStatus()
  {
  dword Status=MdlNode::ModelStatus();
  const int NumFlows = NoFlwIOs();
  if (NumFlows>1)
    {
    int HasFlw=0;
    for (int i=0; i<NumFlows; i++)
      {
      if (IOConduit(i)->QMass()>DisplayZeroFlow)
        HasFlw=1;
      }
    Status |= (HasFlw ? FNS_UFlw : FNS_UNoFlw);
    //const int iIn  = (IOQmEst_In(0)>0.0 ? 0 : 1);
    //Status |= (IOConduit(iIn)->QMass()>UsableMass ? FNS_UFlw : FNS_UNoFlw);
    }
  else if (NumFlows==1)
    Status |= FNS_Error;
  return Status;
  }

//--------------------------------------------------------------------------

flag RTCrusher::CIStrng(int No, pchar & pS)
  {
  switch (No-CBContext())
    {
    case  1: pS="W\tNo Size distributions defined";  return 1;
    case  2: pS="N\tForced larger sizes";            return 1;
    default:
      return MdlNode::CIStrng(No, pS);
    }
  }

//===========================================================================

