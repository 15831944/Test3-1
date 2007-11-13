//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992
 
#include "stdafx.h"
#define  __SQSZDST1_CPP
#include "sc_defs.h"
#include "sp_db.h"
#include "mdlcfg.h"
#include "sqszbase.h"
#include "sqszdst1.h"
#include "executiv.h"
#include "dbgmngr.h"
//#include "optoff.h"

//TODO Manual size distribution
//set this to 1 to allow user to enter manual size distribution sieve size ranges
#define UseManualSizeDefn 0

#define WithSauterMeanD 0

#define  dbgSzDist   WITHDEBUG

#if dbgSzDist
static CDbgMngr dbgSzDistSplits("SzDist", "Splits");
#endif

//===========================================================================
//
//
//
//===========================================================================
#define MoveDistList 0

//static flag fSzAscend=false;//true;
//const MaxDSizes=15;
//const ActDSizes=5;
//struct dSizeInfo {double d; char * Name;};
//const dSizeInfo dSzInfo[ActDSizes] = 
//  {
//    {0.99, "d99"},
//    {0.80, "d80"},
//    {0.50, "d50"},
//    {0.30, "d30"},
//    {0.20, "d20"},
//  };

const int Id_CurShow          =  1;
const int Id_CurHide          =  2;
const int Id_RqdSauterl32     =  3; // KGAFIX - check position / use etc of this / should not always be setable
const int Id_TopSize          = 30;
const int Id_BotSize          = 31;
const int Id_IntervalCnt      = 32;
const int Id_SzAscend         = 33;
const int Id_GrWidth          = 34;
const int Id_GrHeight         = 35;
const int Id_GrOn             = 36;
const int Id_DispRetained     = 37;
const int Id_AutoScale        = 38;
const int Id_PartStepped      = 39;
const int Id_ApplyToDist      = 40;
const int Id_Cumulative       = 41;
const int Id_Fractional       = 42;
const int Id_XLog             = 43;
const int Id_XMin             = 44;
const int Id_XMax             = 45;
const int Id_XIntRng          = 46;
const int Id_XInt             = 47;
const int Id_PartLen          = 48;
const int Id_PartSizeDefn     = 49;
const int Id_DistOn           = 50;
const int Id_DistOff          = Id_DistOn+MaxDistributions;
const int Id_dSize            = Id_DistOff+MaxDistributions; 
const int Id_YMin             = Id_dSize+MaxCSDMeasurements*MaxColumns;
const int Id_YFMax            = Id_YMin +MaxColumns;            const int Id_XIntRange   = Id_YFMax;
const int Id_YQmTtl           = Id_YFMax+MaxColumns;            const int Id_YSelection  = Id_YQmTtl;
const int Id_YData            = Id_YQmTtl+MaxColumns;
const int Id_YMode            = Id_YData+MaxColumns;
const int Id_YState           = Id_YMode+MaxColumns;
const int Id_YTitle           = Id_YState+MaxColumns;
const int Id_YGrfOn           = Id_YTitle+MaxColumns;
const int Id_CopyTest         = Id_YGrfOn+MaxColumns;

#ifdef USE_PSD_TB
const int Id_TestDataChk      = Id_CopyTest+MaxColumns;
const int Id_Last             = Id_TestDataChk+MaxColumns; // New Ids can start from this point for CPSDTB
#endif

XID xidPartSizeDefn       = Qual0XID(0);
XID xidPartLength         = Qual0XID(1);
XID xidPartCurves         = Qual0XID(2);
XID xidApplyToDist        = Qual0XID(3);
XID xidPartCrvMode_F      = Qual0XID(4);
XID xidPartCrvMode_C      = Qual0XID(5);
XID xidPartCrvState_On    = Qual0XID(6);
XID xidPartCrvState_Off   = Qual0XID(7);
XID xidPartCrvMode        = Qual0XID(8);
XID xidPartCrvState       = xidPartCrvMode+(1*MaxColumns);
XID xidPartCrvEdit        = xidPartCrvMode+(2*MaxColumns);
XID xidPartCrvAutoCorrect = xidPartCrvMode+(3*MaxColumns);
XID xidPartCrvTitle       = xidPartCrvMode+(4*MaxColumns);
XID xidPartCrvCnvIndex    = xidPartCrvMode+(5*MaxColumns);
XID xidPartCrvCnvText     = xidPartCrvMode+(6*MaxColumns);
XID xidPartCrvRangeLo     = xidPartCrvMode+(7*MaxColumns);
XID xidPartCrvRangeHi     = xidPartCrvMode+(8*MaxColumns);
XID xidUsedClassID        = xidPartCrvMode+(9*MaxColumns);
                                
//XID xidSzCol                = Qual0XID(MaxColumns*20); see sizedst1.h
XID xidSzAllowSizeEdits   = xidSzCol+MaxDistributions*MaxIntervals*MaxColumns;
XID xidSzDistUsed         = xidSzAllowSizeEdits+1;
XID xidSzDistTopSize      = xidSzDistUsed+1;
XID xidSzDistBottomSize   = xidSzDistTopSize+1;
XID xidDistSzIntervalCnt  = xidSzDistBottomSize+1;
XID xidSzDistPresent      = xidDistSzIntervalCnt+1;
XID xidSzDistOnOff        = xidSzDistPresent   +MaxDistributions;
XID xidSzSeriesUsed       = xidSzDistOnOff     +MaxDistributions;
XID xidSzTopSize          = xidSzSeriesUsed    +MaxDistributions;
XID xidSzBotSize          = xidSzTopSize       +MaxDistributions;
XID xidSzQmTtl            = xidSzBotSize       +MaxDistributions;
XID xidSzMeas             = xidSzQmTtl         +(MaxDistributions*MaxColumns);
XID xidSzNext             = xidSzMeas          +(MaxDistributions*MaxCSDMeasurements*MaxColumns);

#ifdef USE_PSD_TB
XID xidSzTestData         = xidSzNext          +(MaxDistributions*MaxCSDMeasurements*MaxColumns); // Do not know how much space
                                                                                                  // xidSzNext needs??? Does not appear to be used

XID XID_Last              = xidSzTestData      + 1; // New XIDs can start from this point for CPSDTB
#endif

// ==========================================================================
//
// Dief'ed from Precip PSD Stuff  
//
// ==========================================================================

// **********************************************************************
//     Moment: Function to calculate moment of psd
// **********************************************************************

double moment_(double *dist, long j, long npts, double *x)
  {
  // System generated locals
  long     i__1;
  double  ret_val;

  // Builtin functions
  //double      pow_di(double *, long *);

  // Local variables
  long     i;

  // ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
  //
  //     Local Variables Used
  //     npts          number of points in discretization of psd
  //     x             midpoints of discretization
  //     Dist          particle size distribution
  //     i             counter in do loop
  //     j             index of moment
  // ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
  //
  // Parameter adjustments
  // Function Body
  ret_val = 0.;

  // initialize
  i__1 = npts;
  for(i = 0; i < i__1; ++i)
    {
    ret_val += dist[i] * pow(1e6*x[i], j); // note x is midpoint
    }

  return ret_val;
  } // moment_

// **********************************************************************
//     Number_to_Mass: Subroutine to convert number fraction psd to
//     mass (or volume) fraction psd
// **********************************************************************

// Subroutine
int number_to_mass__(double *dist, long npts, double *x)
  {
  // System generated locals
  long           i__1;
  double        d__1, d__2;

  // Local variables
  double        const_;
  long           ii;
  double        mom3;

  // ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
  //
  //     Local Variables Used
  //     npts          number of points in discretization of psd
  //     x             midpoints of discretization
  //     const         normalizing factor
  //     dist          particle size distribution
  //     ii            counter in do loop
  //     mom3          3rd moment of psd
  // ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
  //
  //    Functions Used
  //    Moment         Calculates moments from psd
  // ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
  //

  // Function Body
  mom3 = moment_(&dist[0], 3, npts, &x[0]);
  if(mom3 >= 1e-20)
    {
    const_ = 1. / mom3;
    }
  else
    {
    const_ = 0.;
    }

  i__1 = npts;
  for(ii = 0; ii < i__1; ++ii)
    {
    // Computing 3rd power
    d__1 = 1e6*x[ii], d__2 = d__1;
    dist[ii] = const_ * (d__2 * (d__1 * d__1)) * dist[ii];
    }

  return 0;
  } // number_to_mass__

int mass_to_number__(double *dist, long npts, double *x)
  {
  // System generated locals
  long           i__1;
  double        d__1;//, d__2;

  // Local variables
  long           ii;
  double        sum;
  double        scl;

  // ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
  //
  //     Local Variables Used
  //     npts          number of points in discretization of psd
  //     x             midpoints of discretization
  //     const         normalizing factor
  //     dist          particle size distribution
  //     ii            counter in do loop
  // ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
  //
  //    Functions Used
  //    Moment         Calculates moments from psd
  // ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
  //
  // Parameter adjustments
  
  sum=0;
  i__1 = npts;
  for(ii = 0; ii < i__1; ++ii)
    {
    // Computing 3rd power
    d__1 = 1e6*x[ii];
    dist[ii] = dist[ii] / pow(d__1, 3);
    sum+=dist[ii];
    }

  if (sum >= 1e-20)
    scl= 1/sum;
  else
    scl=0.0;

  for(ii = 0; ii < i__1; ++ii)
    dist[ii] *= scl;

  return 0;
  } // number_to_mass__

// **********************************************************************
//     Dist_Sauter: Subroutine calculates bin numbers for exponential
//     number psd given the Sauter mean diameter
// **********************************************************************

// Subroutine
int dist_sauter__(double *psd, double l32, long npts, double *x)
  {
  // System generated locals
  long           i__1;

  // Builtin functions
  //double            exp(double);

  // Local variables
  long           i;
  double        mom0;

  // cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
  //
  //     Local Variables Used
  //     npts          number of points in discretization of psd
  //     x             midpoints of discretization
  //     i            counter in do loop
  //     L32          Sauter mean diameter
  //     mom0         0th moment
  //     psd          particle size distribution
  // ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
  //
  //    Functions Used
  //    Moment         Calculates moments from psd
  // ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
  //
  // Parameter adjustments

  // Function Body
  i__1 = npts;
  for(i = 0; i < i__1; ++i)
    {
    psd[i] = 1e6*x[i] * exp(x[i] * -3. / l32); // note interval prop to x
    }

  mom0 = moment_(&psd[0], 0, npts, &x[0]);
  i__1 = npts;
  for(i = 0; i < i__1; ++i)
    {
    psd[i] /= mom0; // normalize
    }

  return 0;
  } // dist_sauter__

// ==========================================================================
//
//
//
// ==========================================================================

SQSzDistFractionInfo::SQSzDistFractionInfo(SzPartCrv1 & PartFn, int CurveNo):
  m_PartFn(PartFn)
  {
  m_CurveNo=CurveNo;
  m_ByePass2Fines=0;
  m_ByePass2Coarse=0;
  //m_SpSplit.SetSize(SDB.No());
  m_pSQFines=NULL;
  m_pSQCoarse=NULL;

  m_FineMass=dNAN;
  m_CoarseMass=dNAN;
  };

// ==========================================================================
//
//
//
// ==========================================================================
 
IMPLEMENT_SPARES(SQSzDist1, 400);
IMPLEMENT_TAGOBJEDTDERIVED(SQSzDist1, "SpQuality", "SzDist", "", "SZD", TOC_ALL|TOC_GRP_SIZEDST|TOC_SIZEDIST, SQSzDist1Edt, 
                 "Size Distribution",
                 "Specie Quality to describe the distribution of a specie with size",
                 SQSzDist1TagObjClass);

IMPLEMENT_BUILDDATADEFNGLOBAL(SQSzDist1);

SQSzDist1::SQSzDist1(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach) :
  SpQuality(pClass_, Tag_, pAttach, eAttach)//,
  {
  Distributions.SetSize(SD_Defn.NDistributions());
  fAllowSizeEdits=0;
  iRqdDistOff = -1;
  iRqdDistOn = -1;
  iDistUsed = 0;
#ifdef USE_PSD_TB
  bAllowTestData = false;
  m_PSDTB = new CPSDTB();
#endif
  };

// --------------------------------------------------------------------------

SQSzDist1::~SQSzDist1()
  {
  for (int d=0; d<NDistributions(); d++)
    FreeDist(d);
#ifdef USE_PSD_TB
  delete m_PSDTB;
#endif
  };

// --------------------------------------------------------------------------

/*#ifndef _RELEASE
SQSzDist1 & SQSzDist1::operator=(const SQSzDist1 & S)
  {
  ASSERT(0); // assignment prevent its use
  return *this;
  } 
#endif*/

// --------------------------------------------------------------------------

void SQSzDist1::AllocDist(int d)
  {
  ASSERT(d<NDistributions());
  ASSERT_RDB(iDistUsed==d, "Why not!?!?!", __FILE__, __LINE__);
  if (Distributions[d]==NULL)
    {
    Distributions[d]=new CSD_Distribution(SD_Defn.GetDist(d));
    }
 }
      
// --------------------------------------------------------------------------

void SQSzDist1::FreeDist(int d)
  {
  if (Distributions[d])
    {
    delete Distributions[d];
    Distributions[d]=NULL;
    }
  }

// --------------------------------------------------------------------------

void SQSzDist1::ChangeToDist(int d)
  {
  ASSERT(d<NDistributions());
  iDistUsed = d;
  if (Distributions[d]==NULL)
    {
    for (int i=0; i<NDistributions(); i++)
      if (Distributions[i])
        FreeDist(i);
    AllocDist(d);
    }
  }
          
// --------------------------------------------------------------------------

//Does Nothing: 
//void SQSzDist1::BuildDataDefn_Lcl(DataDefnBlk & DDB)
//  {
//  }

// --------------------------------------------------------------------------

flag SQSzDist1::BuildDataDefnGlobal(TagObjClass *pClass, TaggedObject * pOwner, DataDefnBlk & DDB)
  {                           
  //LPTSTR ClassId="Glbl_SQSzDist1Glbl";
  if (DDB.BeginObject(pOwner, "SQSzDist1", "SQSzDist1_Glbl"))
    {
    DDB.BeginStruct(pOwner, "VarSlct", NULL, DDB_OptPage, UIGlobalDX|pClass->IdNo());

    DDB.Text("Variable Selection");
    DDB.CheckBoxBtn("FracPass",       "",  DC_ ,    "", SzDSlct_Fp  , pOwner, isParm);
    DDB.CheckBoxBtn("MassFlow",       "",  DC_ ,    "", SzDSlct_Qm  , pOwner, isParm);
    DDB.CheckBoxBtn("Mass",           "",  DC_ ,    "", SzDSlct_M   , pOwner, isParm);
    DDB.CheckBoxBtn("CountFracPass",  "",  DC_ ,    "", SzDSlct_NFp , pOwner, isParm);
    DDB.CheckBoxBtn("CountFlow",      "",  DC_ ,    "", SzDSlct_Qn  , pOwner, isParm);
    DDB.CheckBoxBtn("Count",          "",  DC_ ,    "", SzDSlct_N   , pOwner, isParm);
    DDB.CheckBoxBtn("SpecCount",      "",  DC_ ,    "", SzDSlct_NpM , pOwner, isParm);
    DDB.CheckBoxBtn("Cumulative",     "",  DC_ ,    "", SzDSlct_Cum , pOwner, isParm);
#if UseAllColumns
    DDB.CheckBoxBtn("CumulativeInv",  "",  DC_ ,    "", SzDSlct_CumG, pOwner, isParm);
#endif
    DDB.EndStruct();
    }
  DDB.EndObject();

  return true; 
  };

// --------------------------------------------------------------------------

flag SQSzDist1::DataXchgGlobal(TagObjClass *pClass, TaggedObject * pOwner, DataChangeBlk & DCB)
  {
  if (DCB.dwUserInfo==(UIGlobalDX|pClass->IdNo()))
    {
    switch (DCB.lHandle)
      {
      case SzDSlct_Fp:
      case SzDSlct_Qm: 
      case SzDSlct_M:
      case SzDSlct_NFp:
      case SzDSlct_Qn:
      case SzDSlct_N:
      case SzDSlct_NpM:
      case SzDSlct_Cum:
#if UseAllColumns
      case SzDSlct_CumG:
#endif
        if (DCB.rB)
          CSD_Column::sm_VarSlctMask = *DCB.rB ? (CSD_Column::sm_VarSlctMask | DCB.lHandle) : (CSD_Column::sm_VarSlctMask & ~DCB.lHandle);
        DCB.B = (CSD_Column::sm_VarSlctMask & DCB.lHandle)!=0;
        return true;
      }
    };
  return false;
  }

// --------------------------------------------------------------------------

void SQSzDist1::BuildDataDefn(DataDefnBlk & DDB)
  {
  int d;
  Strng Nm;
  #if MoveDistList
  //if (DDB.ForView())
    {
    //DDEF_Flags Vis=DDB.GetVisibility();
    //DDB.Visibility();
    DDB.Text("Distributions");
    DDB.Text("-------------");
    for (d=0; d<NDistributions(); d++)
      {
      CSD_DistDefn &DD=*SD_Defn.GetDist(d);
      Nm.Set("Dist%02d", d);
      //DDB.CheckBoxBtn(Nm(),   "", DC_,  "", xidSzDistOnOff+d, this, isParm|DDEF_AFFECTSSTRUCT, DDBYesNo);
      //DDB.Bool(Nm(),   "", DC_,  "", xidSzDistOnOff+d, this, isParmStopped|/*DDEF_AFFECTSSTRUCT|*/noFileAtAll);
      DDB.Bool(Nm(),   "", DC_,  "", xidSzDistOnOff+d, this, isParmStopped/*DDEF_AFFECTSSTRUCT|*/);
      DDB.TagComment(DD.Name());
      }
    //DDB.Visibility(Vis);
    }
  DDB.Bool("AllowSizeEdits", "", DC_, "", xidSzAllowSizeEdits, this, isParm|InitHidden|noFileAtAll);
  #endif

  //find one & only active distribution...
  d=0;
  CSD_Distribution* pD = d<NDistributions() ? Distributions[d] : NULL;
  while (d<NDistributions() && pD==NULL)
    {
    d++;
    pD = d<NDistributions() ? Distributions[d] : NULL;
    }

  if (pD==NULL && NDistributions()>0)
    {
    d = 0;
    iDistUsed=d;
    AllocDist(d);
    pD = Distributions[d];
    }

  /*DDB.BeginObject(this, "Sz", "SzQual", NULL, DDB_OptPage);
  for (int d_=0; d_<NDistributions(); d_++)
    {
    //CSD_Distribution* pD_ = Distributions[d_];
    //CSD_DistDefn &DD = *SD_Defn.GetDist(d_);
    DDB.Bool("Present",     "", DC_,   "",  xidSzDistPresent+d_, this, isParm);
    DDB.Double("TopSize",   "", SD_Defn.TBCnv.Index(), SD_Defn.TBCnv.Text(),  xidSzTopSize+d, this, noFileAtAll);
    if (d==0)
      DDB.Bool("AllowSizeEdits", "", DC_, "", xidSzAllowSizeEdits, this, isParm|InitHidden|noFileAtAll);
    }
  DDB.EndObject();*/
  //DDB.Long("SizeDefn",   "",  DC_ ,   "", xidSzDistUsed, this, isParm);

  /*if (d==1)
    {
    CSD_DistDefn &DDefn=*SD_Defn.GetDist(0);
    DDB.BeginObject(this, DDefn.Name(), "xx", NULL, DDB_OptPage);
    DDB.Long("SizeDefn",   "",  DC_ ,   "", xidSzDistUsed, this, isParm);
    DDB.EndObject();
    }*/
  if (pD)
    {
    DDB.BeginObject(this, "Sz", SQSzDist1Class.ClassId(), NULL, DDB_OptPage);
    DDB.Long("SizeDefn",   "",  DC_ ,   "", xidSzDistUsed, this, isParm|AffectsStruct);
    DDB.Bool("AllowSizeEdits", "", DC_, "", xidSzAllowSizeEdits, this, isParm|InitHidden|noFileAtAll);
    DDB.Double("TopSize", "", DC_L, "um", xidSzDistTopSize, this, isResult|InitHidden|noFileAtAll);
    DDB.Double("BottomSize", "", DC_L, "um", xidSzDistBottomSize, this, isResult|InitHidden|noFileAtAll);
    DDB.Long("IntervalCnt", "", DC_, "", xidDistSzIntervalCnt, this, isResult|InitHidden|noFileAtAll);

    // Check Box whether to display PSD Test Data or not
#ifdef USE_PSD_TB
 //   if (fAllowSizeEdits)
      DDB.Bool("AllowTestData", "", DC_, "", xidSzTestData , this, isParm|SetOnChange);

    // Build the Data Definition for the PSD Test Data Entry
    // We do it here because if we do it at the end of the routine
    // the data is displayed independent of the EditView and we get Invalid Parameter
    // error messages - Needs further investigation.
    if (bAllowTestData)
    {
      m_PSDTB->SetIDOffsets( Id_Last , XID_Last );
      m_PSDTB->SetDistribution(&pD->rDefn); 
      m_PSDTB->BuildDataDefn(DDB,this);
    }
#endif

    /*for (int d_=0; d_<NDistributions(); d_++)
      {
      CSD_DistDefn &DD = *SD_Defn.GetDist(d_);
      Strng Tg;
      if (d_==0)
        {
        DDB.Long("SizeDefn",   "",  DC_ ,   "", xidSzDistUsed, this, isParm);
        DDB.Bool("AllowSizeEdits", "", DC_, "", xidSzAllowSizeEdits, this, isParm|InitHidden|noFileAtAll);
        }
      Tg.Set("%s_TopSize", DD.Name());
      DDB.Double(Tg(),   "", SD_Defn.TBCnv.Index(), SD_Defn.TBCnv.Text(),  xidSzTopSize+d_, this, noFileAtAll);
      //Tg.Set("%s_Present", DD.Name());
      //DDB.Bool(Tg(),     "", DC_,   "",  xidSzDistPresent+d_, this, isParm);
      }*/

    //find one & only active distribution...
    d=0;
    CSD_Distribution* pD = d<NDistributions() ? Distributions[d] : NULL;
    while (d<NDistributions() && pD==NULL)
      {
      d++;
      pD = d<NDistributions() ? Distributions[d] : NULL;
      }

    CSD_DistDefn &DDefn=*SD_Defn.GetDist(d);
    ASSERT_RDB(pD!=NULL, "What!!!", __FILE__, __LINE__);
    //pD = Distributions[d];
    if (pD && pModel->UseAsFlow())
      {
      CSD_Distribution &D=*pD;
      if (!DDB.ForFileSnpScn())
        {
        if (DDB.BeginObject(this, "QmTtl", "SzQmTtl", NULL, DDB_OptPage))
          {
          //CArray <flag, flag> Done;
          for (int c=0; c<DDefn.Columns.GetSize(); c++)
            if (DDefn.Columns[c].Avail(*pModel))
              {
              CSD_Column &C=DDefn.Columns[c];
          //    Done.SetSize(Max(C.iSpId+1, Done.GetSize()));
              if (C.iSpId>=0)// && !Done[C.iSpId])
                {
                //Done[C.iSpId]=true;
                //Nm.Set("%s.QmTtl", C.sSpName());
                Nm=C.sSpName();
                DDB.Double(Nm(),"", SD_Defn.YQmCnv.Index(), SD_Defn.YQmCnv.Text(), 
                           xidSzQmTtl + (d*MaxDistributions) + c, this, DDEF_WRITEPROTECT|noFileAtAll|DupHandlesOk);
                }
              }
          }
        DDB.EndObject();
        }
      }

    ASSERT_RDB(Distributions[d]!=NULL, "Double damn What!!!", __FILE__, __LINE__);
    pD = Distributions[d];
    if (pD)
      {
      CSD_Distribution &D=*pD;
      for (int i=0; i<D.NIntervals(); i++)
        {
        Nm.Set("I%i", i);
        DDB.BeginObject(this, Nm(), "SzInts", NULL, DDB_OptPage);
        DDB.Double("Size",    "", DC_L,   "um", &D.rIntervals[i], this, DDEF_WRITEPROTECT);
        DDB.Double("SzRange", "", DC_L,   "um", &D.rIntervals[i], this, DDEF_WRITEPROTECT|noFileAtAll);

        if (DDB.ForView() || DDB.ForFiling() || DDB.ForSnapShot())
          {
          for (int ci=0; ci<DDefn.FileColIndices.GetSize(); ci++)
            {
            int c=DDefn.FileColIndices[ci];
            if (DDefn.Columns[c].Avail(*pModel))
              {
              CSD_Column &C=DDefn.Columns[c];
              DDB.Double(C.sFullName(), (PrjFileVerNo()<23) ? C.sFullNameColon() : "", C.pCnv->Index(), C.pCnv->Text(), 
                         xidSzCol+d*MaxColumns*MaxIntervals + i*MaxColumns + c, this, isParm);//C.dwSaveFlags);
              }
            }
          }
        else
          {

          for (int c=0; c<DDefn.Columns.GetSize(); c++)
            if (DDefn.Columns[c].Avail(*pModel))
              {
              CSD_Column &C=DDefn.Columns[c];
              DDB.Double(C.sFullName(), (PrjFileVerNo()<23) ? C.sFullNameColon() : "",  C.pCnv->Index(), C.pCnv->Text(), 
                         xidSzCol+d*MaxColumns*MaxIntervals + i*MaxColumns + c, this, C.dwSaveFlags);
              }
          }
        DDB.EndObject();
        }

      if (!DDB.ForView() && !DDB.ForFiling() && !DDB.ForSnapShot())
      //if (!DDB.ForFiling() && !DDB.ForSnapShot())
        {
        const long NMeas=D.Defn().Measurements.GetSize();
        for (i=0; i<NMeas; i++)
          {
          CSD_Measurement &M=D.Defn().Measurements[i];
          //Nm.Set("%i", i);
          if (M.m_eType!=eSDMT_Text)
            {
            DDB.BeginObject(this, M.m_sName(), "SzInts", NULL, DDB_OptPage);
            for (int c=0; c<DDefn.Columns.GetSize(); c++)
              {
              if (DDefn.Columns[c].Avail(*pModel))
                {
                CSD_Column &C=DDefn.Columns[c];
                DDB.Double(C.sSpName(), "", M.Cnv.Index(), M.Cnv.Text(), 
                           xidSzMeas + d*MaxColumns*MaxCSDMeasurements + i*MaxColumns + c, this, 
                           DDEF_WRITEPROTECT/*C.dwSaveFlags*/|noFile|noSnap|DupHandlesOk);
                }
              }
            DDB.EndObject();
            }
          }
        }
      }
 

    if (DDB.ForView())
      {
      DDB.Object(pModel, this, NULL, NULL, DDB_RqdPage);
      }
    DDB.EndObject();
    }
  else if (NDistributions()>0)
    {
    CSD_DistDefn &DDefn=*SD_Defn.GetDist(0);
    DDB.BeginObject(this, DDefn.Name(), SQSzDist1Class.ClassId(), NULL, DDB_OptPage);
    DDB.Long("SizeDefn",   "",  DC_ ,   "", xidSzDistUsed, this, isParm);
    /*for (int d_=0; d_<NDistributions(); d_++)
      {
      CSD_DistDefn &DD = *SD_Defn.GetDist(d_);
      Strng Tg;
      //Tg.Set("%s_Present", DD.Name());
      //DDB.Bool(Tg(),     "", DC_,   "",  xidSzDistPresent+d_, this, isParm);
      if (d_==0)
        {
        DDB.Long("SizeDefn",   "",  DC_ ,   "", xidSzDistUsed, this, isParm);
        DDB.Bool("AllowSizeEdits", "", DC_, "", xidSzAllowSizeEdits, this, isParm|InitHidden|noFileAtAll);
        }
      Tg.Set("%s_TopSize", DD.Name());
      DDB.Double(Tg(),   "", SD_Defn.TBCnv.Index(), SD_Defn.TBCnv.Text(),  xidSzTopSize+d_, this, noFileAtAll);
      }*/
    DDB.EndObject();
    }
  /*if (d==0 && NDistributions()>1)
    {
    CSD_DistDefn &DDefn=*SD_Defn.GetDist(1);
    DDB.BeginObject(this, DDefn.Name(), "xx", NULL, DDB_OptPage);
    DDB.Long("SizeDefn",   "",  DC_ ,   "", xidSzDistUsed, this, isParm);
    DDB.EndObject();
    }*/
  }

// --------------------------------------------------------------------------

flag SQSzDist1::DataXchg(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
    case xidSzAllowSizeEdits: 
      if (DCB.rB && (pAttachedTo==NULL))
        fAllowSizeEdits=*DCB.rB;
      DCB.B=(pAttachedTo ? AllowSizeEdits() : fAllowSizeEdits);
      return 1;
    case xidSzDistUsed:
      if (DCB.rL)
        {
        iDistUsed = Range(0, (int)*DCB.rL, NDistributions()-1);
        ChangeToDist(iDistUsed);
        }
      DCB.L=iDistUsed;
      return 1;
    case xidSzDistTopSize: 
      DCB.D=SD_Defn.GetDist(iDistUsed)->TopSize();
      return 1;
    case xidSzDistBottomSize: 
      DCB.D=SD_Defn.GetDist(iDistUsed)->BottomSize();
      return 1;
    case xidDistSzIntervalCnt: 
      DCB.L=SD_Defn.GetDist(iDistUsed)->NIntervals();
      return 1;
#ifdef USE_PSD_TB
    case xidSzTestData:
      if (DCB.rB)
        bAllowTestData=(*DCB.rB!=0);
      DCB.B=bAllowTestData;
      return 1;
#endif
    }

  #if MoveDistList
  if (DCB.lHandle>=xidSzDistOnOff && DCB.lHandle<xidSzDistOnOff+MaxDistributions)
    {
    const int d=DCB.lHandle-xidSzDistOnOff;
    flag ff = DCB.ForFiling();
    flag fv = DCB.ForView();
    flag fe = (Distributions[d]!=NULL);
    if (DCB.rB)
      {
      if (*DCB.rB)
        {
        AllocDist(d);
        iRqdDistOn = d;
        }
      else
        {
        FreeDist(d);
        iRqdDistOff = d;
        }
      //View().DoRebuild();

      //perhaps post message to alloc/remove distribution?

      }
    DCB.B=(Distributions[d]!=NULL);
    return 1;
    }
  #endif
  if (DCB.lHandle>=xidSzDistPresent && DCB.lHandle<xidSzDistPresent+MaxDistributions)
    {
    const int d=DCB.lHandle-xidSzDistPresent;
    #if MoveDistList
    flag ff = DCB.ForFiling();
    flag fv = DCB.ForView();
    flag fe = (Distributions[d]!=NULL);
    #endif
    if (DCB.rB)
      {
      if (*DCB.rB)
        {
        //if (d!=iRqdDistOff)
          AllocDist(d);
        //else
        //  iRqdDistOff = -1;
        }
      else
        {
        //if (d!=iRqdDistOn)
        //  FreeDist(d);
        //else
        //  iRqdDistOn = -1;
        }
      }
    DCB.B=(Distributions[d]!=NULL);
    return 1;
    }
  if (DCB.lHandle>=xidSzTopSize && DCB.lHandle<xidSzTopSize+MaxDistributions)
    {
    const int d=DCB.lHandle-xidSzTopSize;
    if (DistExists(d))
      {
      //if (DCB.rD)
        //Distributions[d]->settopsize???
      DCB.D=Distributions[d]->TopSize();
      }
    return 1;
    }
  if (DCB.lHandle>=xidSzQmTtl && DCB.lHandle<xidSzQmTtl+(MaxDistributions*MaxColumns) && pModel->UseAsFlow())
    {
    const int i=DCB.lHandle-xidSzQmTtl;
    const int d=i/MaxDistributions;
    const int c=i%MaxDistributions;
    if (DistExists(d))
      {
      CSD_Column &C=SD_Defn.GetDist(d)->Columns[c];
      //if (DCB.rD)
        //Distributions[d]->settopsize???
      if (C.iSpId>=0)
        DCB.D=Distributions[d]->GetMass(&pModel->m_Ovr, pModel->MArray(), C.iSpId);
      }
    return 1;
    }
  if (DCB.lHandle>=xidSzCol && DCB.lHandle<xidSzCol+MaxDistributions*MaxIntervals*MaxColumns)
    {
    const int i=DCB.lHandle-xidSzCol;
    const int d=i/(MaxIntervals*MaxColumns);
    const int iSz=(i/MaxColumns)%MaxIntervals;
    const int c=i%MaxColumns;
    if (DistExists(d))
      {
      CSD_Column &C=SD_Defn.GetDist(d)->Columns[c];
      CSD_Distribution &D=*Distributions[d];
      if (C.iDataId==DI_MFp)   
        {
        if (C.iSpId>=0)
          {
          if (DCB.rD)
            {
            D.PriSp[C.iSpId]->FracPass[iSz]=*DCB.rD;
            }
          DCB.D=D.PriSp[C.iSpId]->FracPass[iSz];
          return 1;
          }
//        else
//          DCB.D=dNAN;
//        return 1;
        }
      D.CalculateResults(&pModel->m_Ovr, pModel->MArray(), C.iDataId, C.iSpId);
//      if (C.iSpId>=0)
        DCB.D=D.Results()[iSz];
//      else
//        DCB.D=dNAN;
      return 1;
      }
    }

  if (DCB.lHandle>=xidSzMeas && DCB.lHandle<xidSzMeas+MaxCSDMeasurements*MaxDistributions*MaxColumns)
    {
    int i=DCB.lHandle-xidSzMeas;
    int d=i/(MaxCSDMeasurements*MaxColumns);
    int iSz=(i/MaxColumns)%MaxCSDMeasurements;
    int c=i%MaxColumns;
    CSD_Measurement &M=SD_Defn.GetDist(d)->Measurements[iSz];
    DCB.D=dNAN;
    if (DistExists(d))
      {
      switch (M.m_eType)
        {
        case eSDMT_SizePassFrac:
          DCB.D=SizePassingFraction(d, SD_Defn.GetDist(d)->Columns[c].iSpId, M.m_dValue);
          break;
        case eSDMT_FracPassSize: 
          DCB.D=FractionPassingSize(d, SD_Defn.GetDist(d)->Columns[c].iSpId, M.m_dValue);
          break;
        case eSDMT_SAM:
          DCB.D=SurfaceAreaPerGram(d, SD_Defn.GetDist(d)->Columns[c].iSpId);
          break;
        case eSDMT_SAL:
          DCB.D=SurfaceAreaPerLitre(d, SD_Defn.GetDist(d)->Columns[c].iSpId);
          break;
        case eSDMT_PPG:
          DCB.D=ParticleCountPerMass(d, SD_Defn.GetDist(d)->Columns[c].iSpId);
          break;
        }
      }
    return 1;
    }

#ifdef USE_PSD_TB
 if (DCB.lHandle>=XID_Last)
  {
    int val = m_PSDTB->DataXchg(DCB);
    if (val != 0) return(1);
  }
#endif

  return SpQuality::DataXchg(DCB);
  };

// --------------------------------------------------------------------------

flag SQSzDist1::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=true;
  for (int d=0; d<NDistributions(); d++)
    {
    if (Distributions[d])
      {
      Strng s;
      CSD_Distribution& D = *Distributions[d];
      OK = OK && D.ValidateData(VDB);
      double Err;
      for (int c=0; c<SD_Defn.GetDist(d)->Columns.GetSize(); c++)
        {
        CSD_Column& C = SD_Defn.GetDist(d)->Columns[c];
//        if (C.iDataId==DI_MFp && C.iSpId>=0 && !TaggedObject::GetXWritesBusy() && AllowSizeEdits() && D.GetMass(pModel->MArray(), C.iSpId)>1.0e-9)
        if (C.iDataId==DI_MFp && C.iSpId>=0 && AllowSizeEdits() && D.GetMass(&pModel->m_Ovr, pModel->MArray(), C.iSpId)>1.0e-9)
          {
          if (D.PriSp[C.iSpId]->FracPass.AdjustCumulativeEnd(1.0, 0.0, 1.0, Err))
            LogWarning(UsedByTag(), 0, "Sizes adjusted to ensure correct sum (%s column %d) Error=%g", UsedByFullTag(s), c, Err);
          }
        }
      }
    }

  if (OK)
    Equilibrate();

  return OK;
  }

// --------------------------------------------------------------------------

void SQSzDist1::ExchangeSpecies(SpQuality * Other)
  {
  OnDataChange();
  };

// --------------------------------------------------------------------------

void SQSzDist1::Equilibrate()
  {
  OnDataChange();
//  SMBayerBase * pB=dynamic_cast<SMBayerBase *>(pModel);
//  if (pB)
//    {
//    for (int d=0; d<NDistributions(); d++)
//      {
////TODO  the 1 in the line below needs to a configurable flag
//      // the config extras needs to be on a per PriSpecie and Distribution basis.
//      if (DistExists(d) && 1)
//        {
//        const double temp = pB->Temp();
//        const double dens = pB->THADens(temp); //why THA density?
//        const double psa = Distributions[d]->PartSurfaceArea(dens);
//        // Convert m^2/kg to m^2/g
//        pB->SetSAM(0.001*psa, true);
//        }
//      }
//    }
  };

// --------------------------------------------------------------------------

flag SQSzDist1::AllowSizeEdits()
  {
  return SQEditable();
  //return true if model allows size distribution fractions to be set
  //if (pAttachedTo && pAttachedTo->pAttachedTo)
  //  {
  //  if (_stricmp(pAttachedTo->pAttachedTo->Tag(), "Qo")==0 || //used in link
  //      _stricmp(pAttachedTo->pAttachedTo->Tag(), "Disch")==0 || //used in mill,crush1,rollscrusher
  //      _stricmp(pAttachedTo->pAttachedTo->Tag(), "Flows")==0) //used in feeder
  //    return false;
  //  }
  //return true;
  }

// --------------------------------------------------------------------------

char* SQSzDist1::UsedByTag()
  {
  return pAttachedTo ? pAttachedTo->BaseTag() : BaseTag();
  }

// --------------------------------------------------------------------------

char* SQSzDist1::UsedByFullTag(Strng& Str)
  {
  Str=(pAttachedTo ? pAttachedTo->FullObjTag() : FullObjTag());
  return Str();
  }

// --------------------------------------------------------------------------

void SQSzDist1::ZeroMass()
  {
  OnDataChange();
  for (int d=0; d<NDistributions(); d++)
    {
    if (Distributions[d])
      Distributions[d]->ZeroMass();
    //if (Distributions[d])
    //  FreeDist(d);
    }
  }

// --------------------------------------------------------------------------

void SQSzDist1::ZeroDeriv()
  {
  }

// --------------------------------------------------------------------------

void SQSzDist1::ScaleMass(PhMask PhaseM__, double Mult)
  {
  if (Mult < 1.0e-6)
    ZeroMass();
  }

// --------------------------------------------------------------------------

void SQSzDist1::ScaleMass(CIArray & SpIds, double Mult)
  {
  if (Mult < 1.0e-6)
    ZeroMass();
  }

// --------------------------------------------------------------------------

void SQSzDist1::ScaleMass(CSysVector  &Scl, double Mult)
  {
  if (Mult < 1.0e-6)
    ZeroMass();
  }


// --------------------------------------------------------------------------

void SQSzDist1::SetMassF(CSysVector &M1, SpQuality * pQual2)
  {
  OnDataChange();
  SQSzDist1 & P2=*(SQSzDist1*)pQual2;
  for (int d=0; d<P2.NDistributions(); d++)
    if (P2.Distributions[d])
      {
      iDistUsed=d;
      AllocDist(d);
      *Distributions[d]=*P2.Distributions[d];
      }
    else
      FreeDist(d);
  };

// --------------------------------------------------------------------------

void SQSzDist1::AddMassF(CSysVector &M1, SpQuality * pQual2, CSysVector &M2)
  {
  OnDataChange();
  if (pQual2)
    {
    SQSzDist1 & P2=*(SQSzDist1*)pQual2;
    /*for (int d=0; d<P2.NDistributions(); d++)
      if (Distributions[d])
        Distributions[d]->AddMassF(M1, 1.0, P2.Distributions[d], M2, 1.0);
      else if (P2.Distributions[d])
        {
        iDistUsed=d;
        AllocDist(d);
        Distributions[d]->AddMassF(M1, 1.0, P2.Distributions[d], M2, 1.0);
        }
    }*/
    const flag b1 = DistributionsExist();
    const flag b2 = P2.DistributionsExist();
    if (b1 || b2)
      {
      bool FixRqd=false;
      if (!b1)
        {
        //LogWarning(FullObjTag(), 0, "No Size data Create default!  [%s]", P2.FullObjTag());
        iDistUsed=P2.iDistUsed;
        AllocDist(iDistUsed);
        }
      else if (!b2)
        {
        //LogWarning(FullObjTag(), 0, "No Size data!  [%s]", P2.FullObjTag());
        }
      if (b2 && iDistUsed!=P2.iDistUsed)
        {
        LogWarning(FullObjTag(), 0, "Distributions don't match! [%s]", P2.FullObjTag());
        //todo convert!!!
        FixRqd=true;
        }
      Distributions[iDistUsed]->AddMassF(M1, 1.0, P2.Distributions[P2.iDistUsed], M2, 1.0);

      /*if (FixRqd)
        for (s=0; s<Distributions[iDistUsed]->NPriIds(); s++)
          Distributions[iDistUsed]->PriSp[s]->FracPass.AdjustCumulativeEnd(1.0, 0.0, 1.0);*/
      }
    }
  };

// --------------------------------------------------------------------------

void SQSzDist1::SubMassF(CSysVector &M1, SpQuality * pQual2, CSysVector &M2)
  {
  OnDataChange();
  };

// --------------------------------------------------------------------------

void SQSzDist1::Copy(pSpModel pMdl2, SpQuality * pQual2)
  {
  OnDataChange();
  SQSzDist1 & P2=*(SQSzDist1*)pQual2;
  for (int d=0; d<P2.NDistributions(); d++)
    if (P2.Distributions[d])
      {
      iDistUsed=d;
      AllocDist(d);
      *Distributions[d]=*P2.Distributions[d];
      }
    else
      FreeDist(d);
  };

// --------------------------------------------------------------------------

void SQSzDist1::AddDeriv(pSpModel pMdl2, SpQuality * pQual2, double Sgn_)
  {
  // There should not be any code here 
  // Qualities are added in Discrete
  };

// --------------------------------------------------------------------------

void SQSzDist1::AddDiscrete(pSpModel pMdl2, SpQuality * pQual2, double Sgn_)
  {
  OnDataChange();
  if (pQual2)// && pQ2->iOreId>=0)
    {
    SQSzDist1 & P2=*(SQSzDist1*)pQual2;
    for (int d=0; d<P2.NDistributions(); d++)
      if (Distributions[d])
        Distributions[d]->AddMassF(pModel->MArray(), 1.0, P2.Distributions[d], pMdl2->MArray(), ICGetTimeInc());
      else if (P2.Distributions[d])
        {
        iDistUsed=d;
        AllocDist(d);
        Distributions[d]->AddMassF(pModel->MArray(), 1.0, P2.Distributions[d], pMdl2->MArray(), ICGetTimeInc());
        }
    }
  };

// --------------------------------------------------------------------------

void SQSzDist1::ODEOperate(CODEDataBlock & ODB)
  {
  switch (ODB.m_Cmd)
    {
    case eStateAdvance:
    case eStateConverge:
    case eStateLoad:
    case eStateDiskLoad:
      OnDataChange();
      break;
    case eStateFixDV:
    case eStateTest:
    case eStateSave:
    case eStateDiskSave:
      break;
    }
  };

// --------------------------------------------------------------------------

flag SQSzDist1::ChangeDistribution(SpConduit &QFeed, SpConduit &QProd, int iDistRqd)
  {
  SpModel * pMdli = QFeed.Model();
  SQSzDist1 * pSzi = SQSzDist1::Ptr(pMdli, true);
  if (pSzi==NULL || !pSzi->DistributionsExist())
    return false;

  int indexi = pSzi->DistIndex();
  if (iDistRqd==indexi)
    return true; //already correct distribution

  SpModel * pMdlo = QProd.Model();
  SQSzDist1 * pSzo = SQSzDist1::Ptr(pMdlo, true);
  if (pSzo==NULL || !pSzo->DistributionsExist())
    return false;

  ASSERT_RDB(pMdli!=pMdlo, "Why are these pointing to same!!!", __FILE__, __LINE__);
  pSzo->ChangeToDist(iDistRqd);
  int indexo = pSzo->DistIndex();
  if (indexi==indexo)
    return false;

  CSD_Distribution &Di=pSzi->Dist(indexi);
  CSD_Distribution &Do=pSzo->Dist(indexo);
  const int leni = Di.Intervals().GetSize();
  const int leno = Do.Intervals().GetSize();
  const double xi_min = Di.Intervals()[0];
  const double xi_max = Di.Intervals()[leni-1];
  const double xi_bot = Di.BottomSize();
  const int Ns = Di.NPriIds();
  CDArray Cumi(Ns);
  CDArray Cumo(Ns);
  for (int s=0; s<Ns; s++)
    {
    Cumi[s] = 0.0;
    Cumo[s] = 0.0;
    }
  double xi, x2, xo;

  int i = 0;
  int o = 0;
  xo = Do.Intervals()[o];
  xi = xi_bot;
  x2 = xi_min;
  for (s=0; s<Ns; s++)
    {
    CDArray & Sizei = Di.PriSp[s]->FracPass;
    Cumi[s] += Sizei[i];
    }
  while (xo<=xi_max && o<leno && i<leni)
    {
    while (x2<xo && i<leni-1)
      {
      i++;
      xi = x2;
      x2 = Di.Intervals()[i];
      for (int s=0; s<Ns; s++)
        {
        CDArray & Sizei = Di.PriSp[s]->FracPass;
        Cumi[s] += Sizei[i];
        }
      }
          
    if (x2>=xo && i<leni)
      {
      //LogNote("xx", 0, "%2d: B %.4gmm between (%d and %d) %.4gmm and %.4gmm", o, xo, i-1, i, xi, x2);
      //Use cumulative. Convert to log scale (RR), then use linear interpolation on this scale
      const double xlog1 = log(x2);
      const double xlog2 = log(xi);
      const double xlogr = log(xo); //required size
      for (int s=0; s<Ns; s++)
        {
        CDArray & Sizei = Di.PriSp[s]->FracPass;
        CDArray & Sizeo = Do.PriSp[s]->FracPass;
        const double fp1 = Cumi[s];
        const double fp2 = Cumi[s]-Sizei[i];
        if (fabs(fp2-fp1)<1.0e-9)
          {
          //Sizeo[o] = 0.0;
          Sizeo[o] = Max(0.0, fp1-Cumo[s]);
          Cumo[s] += Sizeo[o];
          }
        else
          {
          const double loglog1 = log(-log(Min(fp1, 0.999999999)));
          const double loglog2 = log(-log(Max(fp2, 0.000000001)));
          const double m = (loglog2-loglog1) / (xlog2-xlog1);
          const double loglogr = loglog1 + m * (xlogr-xlog1);
          const double Prevcumo = Cumo[s];
          Cumo[s] = exp(-exp(loglogr));
          Sizeo[o] = Cumo[s]-Prevcumo;
          }
        }

      o++;
      if (o<leno)
        xo = Do.Intervals()[o];
      }
    }

  //top end
  if (o<leno)
    {
    for (int s=0; s<Ns; s++)
      {
      CDArray & Sizeo = Do.PriSp[s]->FracPass;
      Sizeo[o] = 1.0 - Cumo[s];
      }
    o++;
    if (o<leno)
      xo = Do.Intervals()[o];
    }
  while (o<leno)
    {
    //LogNote("xx", 0, "%2d: C %.4gmm", o, xo);
    for (int s=0; s<Do.NPriIds(); s++)
      {
      CDArray & Sizeo = Do.PriSp[s]->FracPass;
      Sizeo[o] = 0.0;
      }
    o++;
    if (o<leno)
      xo = Do.Intervals()[o];
    }

  //ensure math is exactly correct...
  for (s=0; s<Do.NPriIds(); s++)
    Do.PriSp[s]->FracPass.AdjustCumulativeEnd(1.0, 0.0, 1.0);

  return true;
  }

// --------------------------------------------------------------------------

class SPFFnd : public MRootFinderBase
  {
  public:
    SQSzDist1 *pSz;
    int iDistNo, iPriId;
    double dDX;
    static CToleranceBlock s_Tol;
    SPFFnd(double DX, SQSzDist1 *Sz, int DistNo, int PriId) : 
      MRootFinderBase("SPFFnd", s_Tol) 
      { dDX=DX; pSz=Sz; iDistNo=DistNo; iPriId=PriId;};
    LPCTSTR ObjTag() { return (LPCTSTR)pSz->FullObjTag(); };
    double Function(double x) { return pSz->FractionPassingSize(iDistNo, iPriId, x)-dDX; };
  };

CToleranceBlock SPFFnd::s_Tol(TBF_Both, "SizeDst:SPFFnd", 0, 1.0e-9); 

// --------------------------------------------------------------------------

double SQSzDist1::SizePassingFraction(int DistNo, int PriId, double dFrac)
  {
  int d0, dN;
  GetApplyToDists(DistNo, d0, dN);
  
  int GotOne=0;
  double BottomSize=1e10;
  double TopSize=0.0;
  for (int d=d0; d<dN; d++)
    if (DistExists(d))
      {
      GotOne=1;
      CSD_Distribution &D=Dist(d);
      BottomSize=Min(BottomSize, D.BottomSize());
      TopSize=Max(TopSize, D.TopSize());
      }

  if (GotOne)
    {
    double CutSize=Sqrt(BottomSize*TopSize);

    SPFFnd SPFF(dFrac, this, DistNo, PriId);
    SPFF.SetTarget(0.0);
    int iErr=SPFF.Start(BottomSize, TopSize);
    if (iErr==RF_OK)
      {
      if (SPFF.Solve_Brent()!=RF_OK)
        LogWarning(FullObjTag(), 0, "SQSzDist1::SPF() - Brent Not Converged");
      else 
        return SPFF.Result();
      }
    //else 
    //  LogWarning(FullObjTag(), 0, "SQSzDist1::SPF() - Brent Not Started");
 
    return dNAN;
    }

  return dNAN;
  }

// --------------------------------------------------------------------------

double SQSzDist1::FractionPassingSize(int DistNo, int PriId, double dSize)
  {
  if (!Valid(dSize))
    return dNAN;

  int d0, dN;
  GetApplyToDists(DistNo, d0, dN);
  int GotOne=0;
  
  double SPF = 0.0;
  double TotMass = 0.0;
  for (int d=d0; d<dN; d++)
    if (DistExists(d))
      {
      GotOne=true;
      CSD_Distribution &D=Dist(d);
      int s0=(PriId>=0) ? PriId : 0;
      int sN=(PriId>=0) ? PriId : D.NPriIds()-1;
      for (int s=s0; s<=sN; s++)
        {
        SpVector & M=pModel->m_M;
        double Mass=0.0;
        for (int l=0; l<D.NSecIds(s); l++)
          Mass+=M[D.SzId(s,l)];

        CDArray & Size = D.PriSp[s]->FracPass;
        if (Size.GetSize()>0)
          {
          static CDArray CumSize;
          CumSize = Size;
          CumSize.ToCumulative();

          double PF = CumSize.LinearInterpolate(dSize, D.Intervals(), false);
          SPF+=PF*Mass;
          }
        TotMass+=Mass;
        }
      }
  if (GotOne)
    return SPF/GTZ(TotMass);
  return dNAN;
  }

// --------------------------------------------------------------------------

double SQSzDist1::SurfaceAreaPerGram(int DistNo, int PriId)
  {
  int d0, dN;
  GetApplyToDists(DistNo, d0, dN);

  double Temp=pModel->Temp();
  double Press=pModel->Press();

  CDensityInfo C(pModel->Fidelity(), SMO_DensNone, Temp, Press, NULL, pModel->SVData());

  double SAM=0;
  double TotMass=0;
  for (int d=d0; d<dN; d++)
    {
    if (DistExists(d))
      {
      CSD_Distribution &D=Dist(d);
      int s0=(PriId>=0) ? PriId : 0;
      int sN=(PriId>=0) ? PriId : D.NPriIds()-1;
      for (int s=s0; s<=sN; s++)
        {
        long NIntervals = D.NIntervals();
        CDArray & Size = D.Intervals();
        CDArray & FracPass = D.PriSp[s]->FracPass;

        for (int l=0; l<D.NSecIds(s); l++)
          {
          double Dens=SDB[D.SzId(s,l)].DensityXZero(C);
          double Mass=C.Mass();
          TotMass+=Mass;
          for (long i=0; i<NIntervals; i++)
            {
            double DMean = (i==0) ? (Size[i] / 2) : ((Size[i] + Size[i-1]) / 2);
            double CntPerGram = 0.001 * FracPass[i] * Mass / (Dens * PI/6*pow(DMean, 3));
            double AreaPerGram = CntPerGram * (PI*DMean*DMean);
            SAM+=AreaPerGram;
            }
          }
        }
      }
    }
  return SAM/GTZ(TotMass);
  }

// --------------------------------------------------------------------------

double SQSzDist1::SurfaceAreaPerLitre(int DistNo, int PriId)
  {
  int d0, dN;
  GetApplyToDists(DistNo, d0, dN);

  double Temp=pModel->Temp();
  double Press=pModel->Press();

  CDensityInfo C(pModel->Fidelity(), SMO_DensNone, Temp, Press, NULL, pModel->SVData());

  double SAL=0;
  double TotMass=0;
  for (int d=d0; d<dN; d++)
    if (DistExists(d))
      {
      CSD_Distribution &D=Dist(d);
      int s0=(PriId>=0) ? PriId : 0;
      int sN=(PriId>=0) ? PriId : D.NPriIds()-1;
      for (int s=s0; s<=sN; s++)
        {
        long NIntervals = D.NIntervals();
        CDArray & Size = D.Intervals();
        CDArray & FracPass = D.PriSp[s]->FracPass;

        for (int l=0; l<D.NSecIds(s); l++)
          {
          double Dens=SDB[D.SzId(s,l)].DensityXZero(C);
          double Mass=C.Mass();
          TotMass+=Mass;
          for (long i=0; i<NIntervals; i++)
            {
            double DMean = (i==0) ? (Size[i] / 2) : ((Size[i] + Size[i-1]) / 2);
            double CntPerGram = 0.001 * FracPass[i] * Mass / (Dens * PI/6*pow(DMean, 3));
            double AreaPerGram = CntPerGram * (PI*DMean*DMean);
            SAL+=AreaPerGram*Dens;
            }
          }
        }
      }
  return SAL/GTZ(TotMass);
  }
// --------------------------------------------------------------------------

double SQSzDist1::ParticleCountPerMass(int DistNo, int PriId)
  {
  int d0, dN;
  GetApplyToDists(DistNo, d0, dN);

  if (1)//pModel->AllMassesValid())
    {
    double Temp=pModel->Temp();
    double Press=pModel->Press();

    CDensityInfo C(pModel->Fidelity(), SMO_DensNone, Temp, Press, NULL, pModel->SVData());

    double Cnt=0;
    double TotMass=0;
    for (int d=d0; d<dN; d++)
      if (DistExists(d))
        {
        CSD_Distribution &D=Dist(d);
        int s0=(PriId>=0) ? PriId : 0;
        int sN=(PriId>=0) ? PriId : D.NPriIds()-1;
        for (int s=s0; s<=sN; s++)
          {
          long NIntervals = D.NIntervals();
          CDArray & Size = D.Intervals();
          CDArray & FracPass = D.PriSp[s]->FracPass;

          for (int l=0; l<D.NSecIds(s); l++)
            {
            const double Dens=SDB[D.SzId(s,l)].DensityXZero(C);
            const double Mass=C.Mass();
            TotMass+=Mass;
            for (long i=0; i<NIntervals; i++)
              {
              const double DMean = (i==0) ? (Size[i] / 2) : ((Size[i] + Size[i-1]) / 2);
              double CntPerKg = FracPass[i] * Mass / (Dens * PI/6*pow(DMean, 3));
              Cnt+=CntPerKg;
              }
            }
          }
        }
    return Cnt/GTZ(TotMass);
    }
  return dNAN;
  }

// --------------------------------------------------------------------------

double SQSzDist1::GeometricMean(int DistNo, int PriId)
  {
  /*
  CDArray & X=SizeFn.Xs();
  CDArray & Y=SizeFn.Ys();
  double SumDiff=0.0;
  double SumLnDiff=0.0;
  double SumY=0.0;
  int n=SizeFn.Length();
  for (int i=0; i<n; i++)
    {
    double Diff, LnDiff;
    if (i==n-1)
      {
      Diff=0.0;
      LnDiff=0.0;
      }
    else
      {
      Diff=X[i+1]-X[i];
      LnDiff=log(Sqrt(X[i+1]*X[i]))*Diff;
      }
    SumDiff+=Diff;
    SumLnDiff+=LnDiff;
    SumY+=Y[i];
    if (i<4) // Fineest 4

    }
  double DLnK=log(X[0])*(1.0-SumY)
  double D_N=
  
  */
  return dNAN;
  }

// --------------------------------------------------------------------------

double SQSzDist1::TotalMass(int DistNo, int PriId)
  {
  int d0, dN;
  GetApplyToDists(DistNo, d0, dN);
  
  double TotMass = 0.0;
  for (int d=d0; d<dN; d++)
    if (DistExists(d))
      {
      CSD_Distribution &D=Dist(d);
      int s0=(PriId>=0) ? PriId : 0;
      int sN=(PriId>=0) ? PriId : D.NPriIds()-1;
      for (int s=s0; s<=sN; s++)
        {
        SpVector & M=pModel->m_M;
        for (int l=0; l<D.NSecIds(s); l++)
          TotMass+=M[D.SzId(s,l)];
        }
      }
  return TotMass;
  }

// --------------------------------------------------------------------------

CSD_Distribution * SQSzDist1::FindDistributionFor(int SpecieId)
  {
  for (int i=0; i<NDistributions(); i++)
    if (DistExists(i))
      {
      CSD_Distribution * D=Distributions[i];
      for (int j=0; j<D->NPriIds(); j++)
        if (D->SzId(j, 0)==SpecieId)
          return D;
      }

  return NULL;
  };

// --------------------------------------------------------------------------

CSD_SpDist * SQSzDist1::FindSpDistributionFor(int SpecieId)
  {
  for (int i=0; i<NDistributions(); i++)
    if (DistExists(i))
      {
      CSD_Distribution * D=Distributions[i];
      for (int j=0; j<D->NPriIds(); j++)
        if (D->SzId(j, 0)==SpecieId)
          return D->PriSp[j];
      }

  return NULL;
  };

// --------------------------------------------------------------------------

double SQSzDist1::DoFinesFraction(C2DFn & PartFn, double ByePass2Fines, double ByePass2Grits, flag bSetIt) 
  { 
  OnDataChange();
  double Sum0=0.0;
  double Sum1=0.0;
  return Sum1/GTZ(Sum0);
  };

// --------------------------------------------------------------------------

double SQSzDist1::DoGritsFraction(C2DFn & PartFn, double ByePass2Fines, double ByePass2Grits, flag bSetIt) 
  { 
  OnDataChange();
  double Sum0=0.0;
  double Sum1=0.0;
  return Sum1/GTZ(Sum0);
  }

// --------------------------------------------------------------------------

double SQSzDist1::DoFinesFraction(SQSzDistFractionInfo &Info, flag bSetIt) 
  { 
  OnDataChange();

  const int nCrv=Info.m_PartFn.Length();
  if (nCrv==0)// || ns==0)
    return 0.0;

  #if dbgSzDist
  if (dbgSzDistSplits())
    dbgpln("--------------------------------------------");
  #endif
  double Sum0=0.0;
  double Sum1=0.0;
  CDArray & PcX = Info.m_PartFn.SizePts();
  double GF = Range(0.0, 1.0 - Info.m_ByePass2Fines /*- ByePass2Grits*/, 1.0);
  Info.m_SpSplitToFines.SetSpcScalar(dNAN);
  int d0, dN;
  Info.m_PartFn.GetApplyToDists(d0, dN);

  #if WithIndividPartCrv
  if (Info.m_CurveNo<0)
    {

    // NB Info.m_pSQFines Info.m_pSQCoarse
    // Relies on Fines Dist being structurally the same 'this' after a copy

    for (int d=d0; d<dN; d++)
      if (DistExists(d))
        {
        CSD_Distribution &D=Dist(d);
        double TotMass=TotalMass(d, -1);

        const int nInt=D.NIntervals();
        CDArray & Size=D.Intervals();

        const int CrvCount = Info.m_PartFn.NCurves();
        const int PriIdsCount = D.NPriIds();
        if (CrvCount!=PriIdsCount)
          {
          int xx=0; //DO NOT EXPECT THIS
          }
        for (int crv=0; crv<CrvCount; crv++)
          {
          CDArray & PcY = Info.m_PartFn.Curve(crv);
  double p0=PcY[0];
  double p1=PcY[nCrv-1];

          if (Info.m_PartFn.CurveState(crv)==PC_Off)
            continue;

          const bool ToCoarse=(Info.m_PartFn.CurveMode(crv)==PC_Frac2Coarse);

          int iCrvX=0;
          double Sz0=0.0;
          double PcX0=0.0;
          double PcX1=PcX[iCrvX];
          double PcY0=ToCoarse ? 0.0 : 1.0;
          double PcY1=PcY[iCrvX];
          #if dbgSzDist
          if (dbgSzDistSplits())
            dbgpln("                ------Size-----  SumSplit       --------Span-------      ---------Pc---------");
          #endif
          for (long iInt=0; iInt<nInt; iInt++)
            {
            double Sz=Size[iInt];
            double SumSplit=0.0;
            double SzSpan=Sz-Sz0;
            for (;;)
              {
              if (SzSpan>0.0) //KGA 14/7/98
                {
                //...
                double PcSpan=Min(Sz,PcX1)-Max(Sz0, PcX0);
                if (Info.m_PartFn.PartStepped())
                  SumSplit+=PcY1*PcSpan/SzSpan;
                else
                  {
                  SumSplit+=PcY1*PcSpan/SzSpan; // ETC ETC
                  //TODO Smooth Partition Curve
                  }
                #if dbgSzDist
                if (dbgSzDistSplits())
                  dbgpln("%3i %3i %12.3f>%12.3f %7.3f %12.3f/%12.3f %12.3f>%12.3f",
                        iInt, iCrvX, Sz0*1e6, Sz*1e6, SumSplit, PcSpan*1e6 ,SzSpan*1e6, PcX0*1e6, PcX1*1e6);
                #endif
                }
              
              if (PcX1<=Sz)
                {
                if (iCrvX<nCrv)
                  {
                  PcX0=PcX1;
                  PcX1=PcX[iCrvX];
                  PcY1=PcY[iCrvX];
                  iCrvX++;
                  }
                else
                  {
                  iCrvX++;
                  PcX0=PcX1;
                  PcX1=1.1*Size[nInt-1]; // TopSize
                  PcY1=1.1*Size[nInt-1]; // TopSize
                  }
                }
              else// if (iCrvX>=nCrv)
                break;
              }
          
            //for (int s=0; s<D.NPriIds(); s++)
              {
              double &TotFrac=D.PriSp[crv]->FracPass[iInt];
              double TotMass=TotalMass(d, crv);
              double FineFrac, CoarseFrac;
              if (ToCoarse)
                {
                FineFrac=TotFrac * (Info.m_ByePass2Fines + GF * (1.0-SumSplit));
                CoarseFrac=TotFrac * (Info.m_ByePass2Fines + GF * SumSplit);
                }
              else
                {
                FineFrac=TotFrac * (Info.m_ByePass2Fines + GF * SumSplit);
                CoarseFrac=TotFrac * (Info.m_ByePass2Fines + GF * (1.0-SumSplit));
                }
              Sum0+=TotFrac*TotMass;
              Sum1+=FineFrac*TotMass;
              
              D.PriSp[crv]->WorkFrac[iInt]=FineFrac;
              if (Info.m_pSQFines)
                Info.m_pSQFines->Dist(d).PriSp[crv]->FracPass[iInt]=FineFrac;
              if (Info.m_pSQCoarse)
                Info.m_pSQCoarse->Dist(d).PriSp[crv]->FracPass[iInt]=CoarseFrac;
              }
            Sz0=Sz;
            }

          //for (int s=0; s<D.NPriIds(); s++)
            {
            if (Info.m_pSQFines)
              Info.m_pSQFines->Dist(d).PriSp[crv]->FracPass.Normalise();
            if (Info.m_pSQCoarse)
              Info.m_pSQCoarse->Dist(d).PriSp[crv]->FracPass.Normalise();
            }

  #if dbgSzDist
  if (dbgSzDistSplits())
            {
            dbgpln("------------------------");
            dbgpln("       FP    Split    FnFP    CsFP");
            //for (int s=0; s<D.NPriIds(); s++)
              for (int iInt=0; iInt<nInt; iInt++)
                {
                dbgp("%3i %7.3f %7.3f ",iInt, D.PriSp[crv]->FracPass[iInt], D.PriSp[crv]->WorkFrac[iInt]); 
                dbgp(Info.m_pSQFines ? "%7.3f ":"      *",Info.m_pSQFines->Dist(d).PriSp[crv]->FracPass[iInt]);
                dbgp(Info.m_pSQCoarse ? "%7.3f ":"      *",Info.m_pSQCoarse->Dist(d).PriSp[crv]->FracPass[iInt]);
                dbgpln(" %s", SDB[D.SzId(crv,0)].SymOrTag());
                }
            }
  #endif

          //for (int sp=0; sp<D.NPriIds(); sp++)
            {
            //Must also do the secondary species
            double SplitToFines=0.0;
            for (int iInt=0; iInt<nInt; iInt++)
              SplitToFines+=D.PriSp[crv]->WorkFrac[iInt];
            for (int sp1=0; sp1<D.NSecIds(crv); sp1++)
              {
              int sc=D.SzId(crv, sp1);
              Info.m_SpSplitToFines.VValue[sc]=SplitToFines;
              }

            #if dbgSzDist
            if (dbgSzDistSplits())
              dbgpln("Split:%7.3f, %s", Info.m_SpSplitToFines[crv], SDB[crv].SymOrTag());
            #endif
            }
          }
        }
    }
  #endif
  if (Info.m_CurveNo>=0)
    {
  // NB Info.m_pSQFines Info.m_pSQCoarse
  // Relies on Fines Dist being structurally the same 'this' after a copy

  for (int d=d0; d<dN; d++)
    if (DistExists(d))
      {
        CDArray & PcY = Info.m_PartFn.Curve(Info.m_CurveNo);
        double p0=PcY[0];
        double p1=PcY[nCrv-1];

      CSD_Distribution &D=Dist(d);
      double TotMass=TotalMass(d, -1);
  
      int nInt=D.NIntervals();
      CDArray & Size=D.Intervals();

      if (Info.m_PartFn.CurveState(Info.m_CurveNo)==PC_Off)
        continue;

      flag ToCoarse=(Info.m_PartFn.CurveMode(Info.m_CurveNo)==PC_Frac2Coarse);

      int iCrvX=0;
      double Sz0=0.0;
      double PcX0=0.0;
      double PcX1=PcX[iCrvX];
      double PcY0=ToCoarse ? 0.0 : 1.0;
      double PcY1=PcY[iCrvX];
      #if dbgSzDist
      if (dbgSzDistSplits())
        dbgpln("                ------Size-----  SumSplit       --------Span-------      ---------Pc---------");
      #endif
      for (long iInt=0; iInt<nInt; iInt++)
        {
        double Sz=Size[iInt];
        double SumSplit=0.0;
        double SzSpan=Sz-Sz0;
        for (;;)
          {
          if (SzSpan>0.0) //KGA 14/7/98
            {
            //...
            double PcSpan=Min(Sz,PcX1)-Max(Sz0, PcX0);
            if (Info.m_PartFn.PartStepped())
              SumSplit+=PcY1*PcSpan/SzSpan;
            else
              {
              SumSplit+=PcY1*PcSpan/SzSpan; // ETC ETC
              //TODO Smooth Partition Curve
              }
            #if dbgSzDist
            if (dbgSzDistSplits())
              dbgpln("%3i %3i %12.3f>%12.3f %7.3f %12.3f/%12.3f %12.3f>%12.3f",
                     iInt, iCrvX, Sz0*1e6, Sz*1e6, SumSplit, PcSpan*1e6 ,SzSpan*1e6, PcX0*1e6, PcX1*1e6);
            #endif
            }
          
          if (PcX1<=Sz)
            {
            if (iCrvX<nCrv)
              {
              PcX0=PcX1;
              PcX1=PcX[iCrvX];
              PcY1=PcY[iCrvX];
              iCrvX++;
              }
            else
              {
              iCrvX++;
              PcX0=PcX1;
              PcX1=1.1*Size[nInt-1]; // TopSize
              PcY1=1.1*Size[nInt-1]; // TopSize
              }
            }
          else// if (iCrvX>=nCrv)
            break;
          }
      
        for (int s=0; s<D.NPriIds(); s++)
          {
          double &TotFrac=D.PriSp[s]->FracPass[iInt];
          double TotMass=TotalMass(d, s);
          double FineFrac, CoarseFrac;
          if (ToCoarse)
            {
            FineFrac=TotFrac * (Info.m_ByePass2Fines + GF * (1.0-SumSplit));
            CoarseFrac=TotFrac * (Info.m_ByePass2Fines + GF * SumSplit);
            }
          else
            {
            FineFrac=TotFrac * (Info.m_ByePass2Fines + GF * SumSplit);
            CoarseFrac=TotFrac * (Info.m_ByePass2Fines + GF * (1.0-SumSplit));
            }
          Sum0+=TotFrac*TotMass;
          Sum1+=FineFrac*TotMass;
          
          D.PriSp[s]->WorkFrac[iInt]=FineFrac;
          if (Info.m_pSQFines)
            Info.m_pSQFines->Dist(d).PriSp[s]->FracPass[iInt]=FineFrac;
          if (Info.m_pSQCoarse)
            Info.m_pSQCoarse->Dist(d).PriSp[s]->FracPass[iInt]=CoarseFrac;
          }
        Sz0=Sz;
        }

      for (int s=0; s<D.NPriIds(); s++)
        {
        if (Info.m_pSQFines)
          Info.m_pSQFines->Dist(d).PriSp[s]->FracPass.Normalise();
        if (Info.m_pSQCoarse)
          Info.m_pSQCoarse->Dist(d).PriSp[s]->FracPass.Normalise();
        }

      #if dbgSzDist
      if (dbgSzDistSplits())
        {
        dbgpln("------------------------");
        dbgpln("       FP    Split    FnFP    CsFP");
        for (int s=0; s<D.NPriIds(); s++)
          for (int iInt=0; iInt<nInt; iInt++)
            {
            dbgp("%3i %7.3f %7.3f ",iInt, D.PriSp[s]->FracPass[iInt], D.PriSp[s]->WorkFrac[iInt]); 
            dbgp(Info.m_pSQFines ? "%7.3f ":"      *",Info.m_pSQFines->Dist(d).PriSp[s]->FracPass[iInt]);
            dbgp(Info.m_pSQCoarse ? "%7.3f ":"      *",Info.m_pSQCoarse->Dist(d).PriSp[s]->FracPass[iInt]);
            dbgpln(" %s",SDB[D.SzId(s,0)].SymOrTag());
            }
        }
      #endif

      for (int sp=0; sp<D.NPriIds(); sp++)
        {
        // CNM must also do the secondary species
        //int s=D.SzId(sp,0);
        //Info.m_SpSplitToFines[s]=0.0;
        //for (int iInt=0; iInt<nInt; iInt++)
        //  Info.m_SpSplitToFines[s]+=D.PriSp[sp]->WorkFrac[iInt];
        double SplitToFines=0.0;
        for (int iInt=0; iInt<nInt; iInt++)
          SplitToFines+=D.PriSp[sp]->WorkFrac[iInt];
        for (int sp1=0; sp1<D.NSecIds(sp); sp1++)
          {
          int sc=D.SzId(sp, sp1);
          Info.m_SpSplitToFines.VValue[sc]=SplitToFines;
          }

        #if dbgSzDist
        if (dbgSzDistSplits())
            dbgpln("Split:%7.3f, %s", Info.m_SpSplitToFines[sp], SDB[sp].SymOrTag());
        #endif
        }
      }

    }
  return Sum1/GTZ(Sum0);

  };

// --------------------------------------------------------------------------

double SQSzDist1::DoGritsFraction(SQSzDistFractionInfo &Info, flag bSetIt) 
  { 
  OnDataChange();
  double Sum0=0.0;
  double Sum1=0.0;
  return Sum1/GTZ(Sum0);
  }

// --------------------------------------------------------------------------

double SQSzDist1::DoSplitSolids(SQSzDistFractionInfo &Info, SpModel &QFine, SpModel &QCoarse)
  {
  SpModel &QFeed=*pModel;

  SQSzDist1 *FSz=SQSzDist1::Ptr(&QFine);
  SQSzDist1 *CSz=SQSzDist1::Ptr(&QCoarse);

//  QFine.SetMassM(*this, som_Sol, 0.0, Std_P);
//  QCoarse.SetMassM(*this, som_Sol, 0.0, Std_P);
  FSz->Copy(&QFeed, this);
  CSz->Copy(&QFeed, this);

  Info.m_pSQFines=FSz;
  Info.m_pSQCoarse=CSz;
  
  double FF=DoFinesFraction(Info, false);

  double TFn=0.0;
  double TCs=0.0;
  for (int s=0; s<SDB.Count(); s++)
    if (Valid(Info.m_SpSplitToFines[s]))
      {
      double MFn=QFeed.m_M[s]*Info.m_SpSplitToFines[s];
      double MCs=QFeed.m_M[s]-MFn;
      
      QFine.SetVMass(s, QFeed, MFn);
      QCoarse.SetVMass(s, QFeed, MCs);

      TFn+=MFn;
      TCs+=MCs;
      }

  Info.m_FineMass=TFn;
  Info.m_CoarseMass=TCs;

  return FF;
  };

// --------------------------------------------------------------------------

double SQSzDist1::Crush0(C2DFn & PartFn) 
  { 
  OnDataChange();
  for (int d=0; d<NDistributions(); d++)
    if (Distributions[d])
      {
      Distributions[d]->Crush0(PartFn);
      }

  return 0.0;
  }

// --------------------------------------------------------------------------

double SQSzDist1::Crush0(SzPartCrv1 & PartFn, int CurveNo) 
  { 
  OnDataChange();
  for (int d=0; d<NDistributions(); d++)
    if (Distributions[d])
      {
      Distributions[d]->Crush0(PartFn, CurveNo);
      }

  return 0.0;
  }

// --------------------------------------------------------------------------

double SQSzDist1::Break0(SzSelBrk1 & SB) 
  { 
  OnDataChange();
  for (int d=0; d<NDistributions(); d++)
    if (Distributions[d])
      {
      Distributions[d]->Break0(SB);
      }

  return 0.0;
  }

// --------------------------------------------------------------------------

double SQSzDist1::CrushExt0(SzPartCrv1 &Extents, SpConduit &QFeed, int CurveNo)
  {
  OnDataChange();
  SQSzDist1 & FdSz = *SQSzDist1::Ptr(QFeed.Model());
  for (int d=0; d<NDistributions(); d++)
    if (Distributions[d] && FdSz.Distributions[d])
      {
      Distributions[d]->CrushExt0(Extents, *(FdSz.Distributions[d]), CurveNo);
      }

  return 0.0;
  }

// --------------------------------------------------------------------------

double SQSzDist1::Crush1(int s, C2DFn & PartFn) 
  { 
  OnDataChange();
  for (int d=0; d<NDistributions(); d++)
    if (Distributions[d] && s<Distributions[d]->NPriIds())
      {
      Distributions[d]->Crush1(s, PartFn);
      }

  return 0.0;
  }
// --------------------------------------------------------------------------

double SQSzDist1::Crush1(int s, SzPartCrv1 & PartFn, int CurveNo) 
  { 
  OnDataChange();
  for (int d=0; d<NDistributions(); d++)
    if (Distributions[d] && s<Distributions[d]->NPriIds())
      {
      Distributions[d]->Crush1(s, PartFn, CurveNo);
      }

  return 0.0;
  }

// --------------------------------------------------------------------------

double SQSzDist1::CrushExt1(int s, SzPartCrv1 &Extents, SpConduit &QFeed, int CurveNo)
  {
  OnDataChange();
  SQSzDist1 & FdSz = *SQSzDist1::Ptr(QFeed.Model());
  for (int d=0; d<NDistributions(); d++)
    if (Distributions[d] && FdSz.Distributions[d] && s<Distributions[d]->NPriIds())
      {
      Distributions[d]->CrushExt1(s, Extents, *(FdSz.Distributions[d]), CurveNo);
      }

  return 0.0;
  }

// --------------------------------------------------------------------------

flag SQSzDist1::CheckDischIsValid1(int s, SpConduit &QFeed)
  {
  SQSzDist1 & FdSz = *SQSzDist1::Ptr(QFeed.Model());
  for (int d=0; d<NDistributions(); d++)
    if (Distributions[d] && FdSz.Distributions[d] && s<Distributions[d]->NPriIds())
      {
      //CSD_Distribution &D=*Distributions[d];

      CSD_SpDist & SFeed = *(FdSz.Distributions[d]->PriSp[s]);
      CSD_SpDist & SDisch = *(Distributions[d]->PriSp[s]);
      double SumFeed = 0.0;
      double SumDisch = 0.0;
      for (int j=Distributions[d]->NIntervals()-1; j>=0; j--)
        {
        SumFeed += SFeed.FracPass[j];
        SumDisch += SDisch.FracPass[j];
        if (SumDisch-SumFeed>1.0e-9)
          return false;
        }
      }
  return true;
  }

// --------------------------------------------------------------------------

double SQSzDist1::CorrectDisch1(int s, SpConduit &QFeed)
  {
  SQSzDist1 & FdSz = *SQSzDist1::Ptr(QFeed.Model());
  for (int d=0; d<NDistributions(); d++)
    if (Distributions[d] && FdSz.Distributions[d] && s<Distributions[d]->NPriIds())
      {
      //CSD_Distribution &D=*Distributions[d];

      CSD_SpDist & SFeed = *(FdSz.Distributions[d]->PriSp[s]);
      CSD_SpDist & SDisch = *(Distributions[d]->PriSp[s]);
      double SumFeed = 0.0;
      double SumDisch = 0.0;
      for (int j=Distributions[d]->NIntervals()-1; j>=0; j--)
        {
        SumFeed += SFeed.FracPass[j];
        double PrevSumDisch = SumDisch;
        SumDisch += SDisch.FracPass[j];
        if (SumDisch>SumFeed)
          {
          if (j==0)
            SDisch.FracPass[j] = 1.0 - PrevSumDisch;
          else
            {
            double Extra = SumDisch - SumFeed;
            SDisch.FracPass[j] = SumFeed - PrevSumDisch;
            double TempSumDisch = SumFeed;
            if (SumDisch>=1.0)
              {
              for (int i=j-1; i>0; i--)
                {
                double d = (1.0/j) * Extra;
                SDisch.FracPass[i] += d;
                TempSumDisch += SDisch.FracPass[i];
                }
              }
            else
              {
              for (int i=j-1; i>0; i--)
                {
                double d = (SDisch.FracPass[i]/(1.0-SumDisch)) * Extra;
                SDisch.FracPass[i] += d;
                TempSumDisch += SDisch.FracPass[i];
                }
              }
            SDisch.FracPass[0] = 1.0 - TempSumDisch;
            SumDisch = SumFeed;
            }
          }
        }
      }
  return 0.0;
  }

// ==========================================================================
//
//
//
// ==========================================================================

SQSzDist1TagObjClass::SQSzDist1TagObjClass(pchar pClassName, pchar pGroup_, pchar pClassId_, pchar SubClassId, pchar pVersion_, pchar pDrwGroup_, pchar pDefTag_, dword dwCat, pchar ShortDesc, pchar pDesc, DWORD SelectMask) : 
  SQTagObjClass(pClassName, pGroup_, pClassId_, SubClassId, pVersion_/*, pDrwGroup_*/, pDefTag_, dwCat, ShortDesc, pDesc, SelectMask) 
  {
  FCnv.Set(DC_Frac, "%");
  FFmt.Set("", 0, 2, 'f');
  }


/* NBNB: KGA 13/1/04 : Potential problem? Correlation between tear variables and number of distribution definitions (used or unused!) needs checking!!!
   More than one distribution may exist in a project (or project cfg file) but only one is ever actually used in a tear stream! */

//---------------------------------------------------------------------------

int SQSzDist1TagObjClass::NTearVariables()
  {
  int N=0;
  for (int d=0; d<SD_Defn.NDistributions(); d++)
    {
    CSD_DistDefn &D=*(SD_Defn.GetDist(d));
    N+=D.NPriIds()*D.NIntervals();
    }
  
  return N;
  };

//---------------------------------------------------------------------------

int SQSzDist1TagObjClass::DefineTearVariables(TearVarArray & TV, int n)
  {
  int N=0;
  for (int d=0; d<SD_Defn.NDistributions(); d++)
    {
    CSD_DistDefn &D=*(SD_Defn.GetDist(d));
    for (int id=0; id<D.NPriIds(); id++)
      {
      int s=D.SzId(id,0);
      for (int i=0; i<D.NIntervals(); i++)
        {
        TV[n].m_Tag.Set("D%i.I%i.%s", d, i, SDB[s].Tag());
        TV[n].m_Sym.Set("D%i.I%i.%s", d, i, SDB[s].SymOrTag());
        TV[n].m_pDiffCnv=&FCnv;
        TV[n].m_pDiffFmt=&FFmt;
        TV[n].m_pMeasCnv=&FCnv;
        TV[n].m_pMeasFmt=&FFmt;
        n++;
        N++;
        }
      }
    }

  ASSERT(N==NTearVariables());
  return N;
  };

//===========================================================================
//
//
//
//===========================================================================

void SQSzDist1::TearGetInputs(TearVarArray & TV, int n)
  {
  double TotMass=pModel->Mass();
  for (int d=0; d<SD_Defn.NDistributions(); d++)
    if (DistExists(d))
      {
      CSD_Distribution &D=Dist(d);
      for (int id=0; id<D.NPriIds(); id++)
        {
        double MF=pModel->VMass[D.SzId(id, 0)]/GTZ(TotMass);
        flag MFOK=MF>1.0e-8;
        for (int i=0; i<D.NIntervals(); i++)
          {
          TV[n].m_Y[0]=D.PriSp[id]->FracPass[i];
//          DoBreak();
          TV[n].m_bTestIsValid=MFOK;
          //TV[n].m_bHoldOutput=false;//!MFOK;
          n++;
          }
        }
      }
  };

//---------------------------------------------------------------------------

void SQSzDist1::TearGetOutputs(TearVarArray & TV, int n)
  {
  for (int d=0; d<SD_Defn.NDistributions(); d++)
    if (DistExists(d))
      {
      CSD_Distribution &D=Dist(d);
      for (int id=0; id<D.NPriIds(); id++)
        for (int i=0; i<D.NIntervals(); i++)
          TV[n++].m_X[0]=D.PriSp[id]->FracPass[i];
      }
  };

//---------------------------------------------------------------------------

void SQSzDist1::TearSetOutputs(TearVarArray & TV, int n)
  {
  for (int d=0; d<SD_Defn.NDistributions(); d++)
    if (DistExists(d))
      {
      CSD_Distribution &D=Dist(d);
      for (int id=0; id<D.NPriIds(); id++)
        {
        for (int i=0; i<D.NIntervals(); i++)
          D.PriSp[id]->FracPass[i]=TV[n++].m_X[0];
          //if (!TV[n].bHoldOutput)
          //  {
          //  D.PriSp[id]->FracPass[i]=TV[n].X[0];
          //  n++;
          //  }
        D.PriSp[id]->FracPass.Normalise();
        }
      }
  };

//---------------------------------------------------------------------------

void SQSzDist1::Dump()
  {
  for (int d=0; d<SD_Defn.NDistributions(); d++)
    if (DistExists(d))
      {
      dbgpln("Dist%02i", d);
      CSD_Distribution &D=Dist(d);
      for (int id=0; id<D.NPriIds(); id++)
        {
        dbgp("  ");
        for (int i=0; i<D.NIntervals(); i++)
          dbgp(" %12.8f", D.PriSp[id]->FracPass[i]);
        dbgpln("");
        }
      }
  };
    
//===========================================================================
//
//
//
//===========================================================================

const int SQSzDist1Edt::XPix=10000;
const int SQSzDist1Edt::YPix=10000;
//const int SQSzDist1Edt::CrvPts=256;
const int SQSzDist1Edt::CrvPts=(MaxIntervals+2)*2;
int SQSzDist1Edt::iGraphWidth=40;
int SQSzDist1Edt::iGraphHeight=15;
int SQSzDist1Edt::iGraphOn=1;
flag SQSzDist1Edt::fCrvStepped=true;

//---------------------------------------------------------------------------

struct MappingSave
  {
  POINT    VOrg;
  SIZE     VExt;
  POINT    WOrg;
  SIZE     WExt;
  };

//---------------------------------------------------------------------------

static void PushScaling(rGDIBlk GB, RECT &GraphR, MappingSave &MapSv)
  {
  SetMapMode(GB.DC().m_hDC, MM_ANISOTROPIC);
  SetViewportOrgEx(GB.DC().m_hDC, GraphR.left, GraphR.top, &MapSv.VOrg);
  SetViewportExtEx(GB.DC().m_hDC, GraphR.right-GraphR.left, GraphR.bottom-GraphR.top, &MapSv.VExt);

  int XPixFrac=SQSzDist1Edt::XPix/40;
  int YPixFrac=SQSzDist1Edt::YPix/40;
  
  SetWindowOrgEx(GB.DC().m_hDC, -XPixFrac, SQSzDist1Edt::YPix+YPixFrac, &MapSv.WOrg);
  SetWindowExtEx(GB.DC().m_hDC, SQSzDist1Edt::XPix+2*XPixFrac, -(SQSzDist1Edt::YPix+2*YPixFrac), &MapSv.WExt);
//  SetWindowOrgEx(GB.DC().m_hDC, 0, SQSzDist1Edt::YPix, &MapSv.WOrg);
//  SetWindowExtEx(GB.DC().m_hDC, SQSzDist1Edt::XPix, -SQSzDist1Edt::YPix, &MapSv.WExt);
  }

//---------------------------------------------------------------------------

static void PopScaling(rGDIBlk GB, MappingSave &MapSv)
  {
  SetMapMode(GB.DC().m_hDC, MM_TEXT);
  SetViewportOrgEx(GB.DC().m_hDC, MapSv.VOrg.x,  MapSv.VOrg.y, NULL);
  SetViewportExtEx(GB.DC().m_hDC, MapSv.VExt.cx, MapSv.VExt.cy, NULL);
  SetWindowOrgEx(GB.DC().m_hDC, MapSv.WOrg.x,  MapSv.WOrg.y, NULL);
  SetWindowExtEx(GB.DC().m_hDC, MapSv.WExt.cx, MapSv.WExt.cy, NULL);
  }

//---------------------------------------------------------------------------

SQSzDist1Edt::SQSzDist1Edt(FxdEdtView * pView_, SQSzDist1 * pSD_) :
  FxdEdtBookRef(pView_),
  rSD(*pSD_)
  {
  iDragPtNo = -1;
 
  XMin=0.0;
  XMax=1.0;
  XFactor=1.0;
  iNameWidth=4;
  iWorkDist=0;
  iPg1=0;

  fXtraPg=0;

  pLabels = new CLabelWnd;
  pLabels->CreateIt(pView_);
  pLabels->SetFont(pView_->ScrGB.pFont[0]);
  pLabels->SetLineCnt(1/*2*/);

//  pWrkCnv=NULL;
//  pWrkFmt=NULL;

  ObjectAttribute *pAttr=ObjAttributes.FindObject("SqSzDist1Edt");
  if (pAttr)
    {
    pAttr->FieldFmtCnvs("Size", SD_Defn.XFmt, SD_Defn.XCnv);
    pAttr->FieldFmtCnvs("Frac", SD_Defn.YFFmt, SD_Defn.YFCnv);
    pAttr->FieldFmtCnvs("Flow", SD_Defn.YQmFmt, SD_Defn.YQmCnv);
    pAttr->FieldFmtCnvs("Mass", SD_Defn.YMFmt, SD_Defn.YMCnv);
    pAttr->FieldFmtCnvs("PartFlow",  SD_Defn.YQnFmt, SD_Defn.YQnCnv);
    pAttr->FieldFmtCnvs("PartCount", SD_Defn.YNFmt, SD_Defn.YNCnv);
    pAttr->FieldFmtCnvs("PartCountFrac", SD_Defn.YNFFmt, SD_Defn.YNFCnv);
    pAttr->FieldFmtCnvs("SpPartCount", SD_Defn.YNpMFmt, SD_Defn.YNpMCnv);
    pAttr->FieldFmtCnvs("TopBotSize", SD_Defn.TBFmt, SD_Defn.TBCnv);
    }

  CProfINIFile PF(PrjIniFile());
  if (ValidDistribution())
    {
    Strng s;
    for (int i=0; i<NColumns(); i++)
      if (Columns(i).Avail(Model()))
        {
        s=Columns(i).sFullName();
        Columns(i).SetOn(PF.RdInt("SqSzDist1Edt", s(), Columns(i).On()));
        s+="_GrfOn";
        Columns(i).SetGrfOn(PF.RdInt("SqSzDist1Edt", s(), Columns(i).GrfOn()));
        }
    }

  SQSzDist1Edt::iGraphWidth=PF.RdInt("SqSzDist1Edt", "GraphWidth", 40);
  SQSzDist1Edt::iGraphHeight=PF.RdInt("SqSzDist1Edt", "GraphHeight", 15);
  SQSzDist1Edt::iGraphOn=PF.RdInt("SqSzDist1Edt", "GraphOn", true);
  SQSzDist1Edt::fCrvStepped=PF.RdInt("SqSzDist1Edt", "CrvStepped", true);

  fSzAscend=PF.RdInt("SzDistribution", "IntervalsAscending", fSzAscend);
  }

//---------------------------------------------------------------------------

SQSzDist1Edt::~SQSzDist1Edt()
  {
  CProfINIFile PF(PrjIniFile());
  if (ValidDistribution())
    {
    Strng s;
    for (int i=0; i<NColumns(); i++)
      if (Columns(i).Avail(Model()))
        {
        s=Columns(i).sFullName();
        PF.WrInt("SqSzDist1Edt", s(), Columns(i).On());
        s+="_GrfOn";
        PF.WrInt("SqSzDist1Edt", s(), Columns(i).GrfOn());
        }
    }

  PF.WrInt("SqSzDist1Edt", "GraphWidth",  SQSzDist1Edt::iGraphWidth);
  PF.WrInt("SqSzDist1Edt", "GraphHeight", SQSzDist1Edt::iGraphHeight);
  PF.WrInt("SqSzDist1Edt", "GraphOn",     SQSzDist1Edt::iGraphOn);
  PF.WrInt("SqSzDist1Edt", "CrvStepped",  SQSzDist1Edt::fCrvStepped);
  PF.WrInt("SzDistribution", "IntervalsAscending", fSzAscend);

  ObjectAttribute *pAttr=ObjAttributes.FindObject("SqSzDist1Edt");
  if (pAttr)
    {
    pAttr->SetFieldFmt("Size", SD_Defn.XFmt);
    pAttr->SetFieldCnvs("Size", SD_Defn.XCnv);
    pAttr->SetFieldFmt("Frac", SD_Defn.YFFmt);
    pAttr->SetFieldCnvs("Frac", SD_Defn.YFCnv);
    pAttr->SetFieldFmt("Flow", SD_Defn.YQmFmt);
    pAttr->SetFieldCnvs("Flow", SD_Defn.YQmCnv);
    pAttr->SetFieldFmt("Mass", SD_Defn.YMFmt);
    pAttr->SetFieldCnvs("Mass", SD_Defn.YMCnv);
    pAttr->SetFieldFmt("PartFlow", SD_Defn.YQnFmt);
    pAttr->SetFieldCnvs("PartFlow", SD_Defn.YQnCnv);
    pAttr->SetFieldFmt("PartCount", SD_Defn.YNFmt);
    pAttr->SetFieldCnvs("PartCount", SD_Defn.YNCnv);
    pAttr->SetFieldFmt("PartCountFrac", SD_Defn.YNFFmt);
    pAttr->SetFieldCnvs("PartCountFrac", SD_Defn.YNFCnv);
    pAttr->SetFieldFmt("SpPartCount", SD_Defn.YNpMFmt);
    pAttr->SetFieldCnvs("SpPartCount", SD_Defn.YNpMCnv);
    pAttr->SetFieldFmt("TopBotSize", SD_Defn.TBFmt);
    pAttr->SetFieldCnvs("TopBotSize", SD_Defn.TBCnv);
    }
  
  pLabels->DestroyWindow();
  
  //cnmdelete pLabels;
  }

//---------------------------------------------------------------------------

int SQSzDist1Edt::DistFromPgNo(int Pg)
  {
  //find one & only active distribution...
  int d=0;
  CSD_Distribution* pD = d<rSD.NDistributions() ? rSD.Distributions[d] : NULL;
  while (d<rSD.NDistributions() && pD==NULL)
    {
    d++;
    pD = d<rSD.NDistributions() ? rSD.Distributions[d] : NULL;
    }
  return (pD ? d : -1);
  /*for (int d=0; d<rSD.NDistributions(); d++)
    if (rSD.Distributions[d])
      {
      if (Pg--==0) 
        return d;
      }
  //DoBreak();
  return -1;*/
  }

//---------------------------------------------------------------------------

void SQSzDist1Edt::PutDataStart()
  {
  }

//---------------------------------------------------------------------------

void SQSzDist1Edt::PutDataDone()
  {
  }

//---------------------------------------------------------------------------

void SQSzDist1Edt::GetDataStart()
  {
  }

//---------------------------------------------------------------------------

void SQSzDist1Edt::GetDataDone()
  {
  }

//---------------------------------------------------------------------------

void SQSzDist1Edt::StartBuild()
  {
  }

//---------------------------------------------------------------------------

void SQSzDist1Edt::Build()
  {
  int d;
  Strng Tg;
  Strng S;

  fXtraPg=0;
  #if !MoveDistList
  /*for (d=0; d<rSD.NDistributions(); d++)
    if (!rSD.Distributions[d])
      fXtraPg=1;
  //
  fXtraPg=1;
  if (fXtraPg)
    {
    int L=0;
    StartPage("Sizes");
    StartBlk(rSD.NDistributions()+3, 0, NULL);
    SetDesc(L, "Distributions",  -1, 14,  2, " ");
    L++;
    SetDesc(L, "-------------",  -1, 14,  2, " ");
    L++;
    
    for (int d=0; d<rSD.NDistributions(); d++)
      //if (!rSD.Distributions[d])
      {
      SetSpace(L, 3);
      if (rSD.Distributions[d])
      //if (Dist(d))
        {
        Tg.Set(SD_Defn.GetDist(d)->Name());
        Tg+=" On>Off";
        SetButton(L, Tg(),  Id_DistOff+d, Max(12, Tg.Len()+2), 0, "");
        }
      else
        {
        Tg.Set(SD_Defn.GetDist(d)->Name());
        Tg+=" Off>On";
        SetButton(L, Tg(),  Id_DistOn+d, Max(12, Tg.Len()+2), 0, "");
        }
      L++;
      }
    }*/
  #endif

  //find one & only active distribution...
  d=0;
  CSD_Distribution* pD = d<rSD.NDistributions() ? rSD.Distributions[d] : NULL;
  while (d<rSD.NDistributions() && pD==NULL)
    {
    d++;
    pD = d<rSD.NDistributions() ? rSD.Distributions[d] : NULL;
    }

  //const int NColmns = rSD.NDistributions() ? NColumns() : 0;
  SpModel* p = (SpModel*)rSD.pModel;
  flag AsFlow = (rSD.pModel->UseAsFlow());
  iPg1=pView->Pages;
  //StartPage(pD ? pD->Name() : "Sz", true);
  StartPage("Sz", true);
  if (1) // Header 1 Blk
    {
    StartBlk(1+rSD.NDistributions(), 0, NULL);
    int L=(pD && GraphOn()) ? 0 : 1;
    for (int d_=0; d_<rSD.NDistributions(); d_++)
      {
      CSD_Distribution* pD_ = rSD.Distributions[d_];
      SetSpace(L, 3);
      if (pD_)
        {
        Tg.Set(SD_Defn.GetDist(d_)->Name());
        Tg+=" On>Off";
        SetButton(L, Tg(),  Id_DistOff+d_, Max(12, Tg.Len()+2), 0, "");
        }
      else
        {
        Tg.Set(SD_Defn.GetDist(d_)->Name());
        Tg+=" Off>On";
        SetButton(L, Tg(),  Id_DistOn+d_, Max(12, Tg.Len()+2), 0, "");
        }
      L++;
      }
    }

  if (pD)
    {
    SetWorkDist(d);
    }

  const int NColmns = pD ? NColumns() : 0;
  int ColWidth=10;
  for (int c=0; c<NColmns; c++)
    if (Columns(c).Reqd(Model()))
      ColWidth=Max(ColWidth, Columns(c).iColWidth);

  if (pD)
    {
    CSD_DistDefn &DDefn=pD->Defn();
 
    //StartPage(DD().Name(), true);//(strlen(p)>0) ? p : "Fn");
    if (1) // Header 2 Blk
      {
      int HedLen = (GraphOn() ? 5 : 3);
      #if WithSauterMeanD
      HedLen++;
      #endif
      StartBlk(HedLen, 0, NULL);
      int L=0;
      if (GraphOn())
        {
        SetSpace(L, 1);
        SetParm(L, "", Id_XMin, 10, 2, "");
        SetSpace(L, Max(2,GraphWidth()-20-2));
        SetParm(L, "", Id_XMax, 10, 2, "");
        SetSpace(L, 1);
        SetDesc(L, "Graph:", -1, 6, 2, " ");
        Strng Tg(GraphOn() ? " On > Off " : " Off > On ");
        SetButton(L, Tg(),  Id_GrOn, 10, 0, "");

        L++;
        SetSpace(L, 1);
        if (XLog())
          SetButton(L,"Log > Lin", Id_XLog, 11, 0, "");
        else
          SetButton(L,"Lin > Log", Id_XLog, 11, 0, "");
        SetSpace(L, 1);
        if (AutoScale())
          SetButton(L, "Auto > Fixed Scale", Id_AutoScale, 20, 0, "");
        else
          SetButton(L, "Fixed > Auto Scale", Id_AutoScale, 20, 0, "");
        SetSpace(L, 1);
        if (fCrvStepped)
          SetButton(L,"Stepped > Smooth", Id_PartStepped, 18, 0, " ");
        else
          SetButton(L,"Smooth > Stepped", Id_PartStepped, 18, 0, " ");
        SetSpace(L, 1);
        L++;
        }
      SetSpace(L, 1);
      L++;
    
      SetSpace(L, 1);
      SetDParm(L, "Show", 5, "", Id_CurShow, 25, 0, "  ");
      int GotOne=0;
      for (int i=0; i<NColmns; i++)
        if (!Columns(i).On() && Columns(i).Avail(Model()))
          {
          FldHasFixedStrValue(i, Columns(i).DispName());
          GotOne=1;
          }
      if (!GotOne)
        FldHasFixedStrValue(-1, " ");
      //L++;
      SetSpace(L, 1);
      SetDParm(L, "Hide", 5, "", Id_CurHide, 25, 0, "");
      GotOne=0;
      for (i=0; i<NColmns; i++)
        if (Columns(i).On() && Columns(i).Avail(Model()))
        //if (Columns(i).Avail(Model()))
          {
          FldHasFixedStrValue(i, Columns(i).DispName());
          GotOne=1;
          }
      if (!GotOne)
        FldHasFixedStrValue(-1, " ");

#if WithSauterMeanD
      L++;
      SetSpace(L, 1);
      SetDParm(L, "SauterMeanD", 15, "", Id_RqdSauterl32, 32, 0, "");
#endif
      }

    flag DoneGap=false;
    flag DoneCumGap=false;
    const flag MultOre = (NColmns && !Columns(0).SimpleNames());
    if (0) // Measurements Blk
      {
      int HedLen = (MultOre ? 2 : 1);
      StartBlk(DDefn.Measurements.GetSize()+HedLen, HedLen, NULL);
      int L=0;
      //...
      if (MultOre)
        {
        DoneGap=false;
        DoneCumGap=false;
        SetSpace(L, 4);
        for (int ci=0; ci<NColmns; ci++)
          {
          int c=DDefn.DispColIndices[ci];
          if (Columns(c).Reqd(Model()))
            {
            if (!Columns(c).ForCum() && !DoneCumGap && !DoneGap)
              {
              DoneGap=true;
              SetSpace(L, 10+1);
              }
            if (Columns(c).ForCum() && !DoneCumGap)
              {
              DoneCumGap = true;
              SetSpace(L, 2+10+1);
              }
            S=Columns(c).sName();
            S+=':';
            S.SetLength(Min(S.Length(), ColWidth));
            SetDesc(L, S(), -1, ColWidth, 1, "");
            }
          }
        L++;
        }

      //...
      DoneGap=false;
      DoneCumGap=false;
      SetDesc(L, "Measurements", -1, 14, 0, " ");
      for (int ci=0; ci<NColmns; ci++)
        {
        int c=DDefn.DispColIndices[ci];
        if (Columns(c).Reqd(Model()))
          {
          if (!Columns(c).ForCum() && !DoneCumGap && !DoneGap)
            {
            DoneGap=true;
            SetSpace(L, 1);
            }
          if (Columns(c).ForCum() && !DoneCumGap)
            {
            DoneCumGap = true;
            SetSpace(L, (DoneGap ? 2+10+1 : 2+1));
            }
          S=(MultOre ? Columns(c).sSpName() : Columns(c).sName());
          S.SetLength(Min(S.Length(), ColWidth));
          SetDesc(L, S(), -1, ColWidth, 1, "");
          }
        }
      L++;
      for (int iSz=0; iSz<DDefn.Measurements.GetSize(); iSz++)
        {
        DoneGap=false;
        DoneCumGap=false;
        CSD_Measurement &M=DDefn.Measurements[iSz];
        SetDesc(L, M.m_sName(),  -1, 14,  2, "");
        if (M.m_eType!=eSDMT_Text)
          {
          for (ci=0; ci<NColmns; ci++)
            {
            int c=DDefn.DispColIndices[ci];
            if (Columns(c).Reqd(Model()))
              {
              if (!Columns(c).ForCum() && !DoneCumGap && !DoneGap)
                {
                DoneGap=true;
                SetSpace(L, 1);
                }
              if (Columns(c).ForCum() && !DoneCumGap)
                {
                DoneCumGap = true;
                if (DoneGap)
                  {
                  SetSpace(L, 1);
                  SetDesc(L, M.Cnv.Text()/*SD_Defn.XCnv.Text()*/, 0, 12, 0, "");
                  }
                else
                  SetSpace(L, 2+1);
                }
              SetParm(L, "", Id_dSize+iSz+c*MaxCSDMeasurements, ColWidth, 2, "");
              Tg.Set("%s.%s.%s", FullObjTag(), M.m_sName(), Columns(c).sSpName());
              SetTag(Tg(), M.Cnv.Text());//SD_Defn.XCnv.Text());
              }
            }
          }
        SetSpace(L, 1);
        SetDesc(L, M.Cnv.Text()/*SD_Defn.XCnv.Text()*/, 0, 16, 0, "");
        L++;
        }
      }
    
    if (1) // Data Blk
      {
      int L=0;
      int HedLen = 2;
#ifdef USE_PSD_TB
      if (rSD.fAllowSizeEdits)
        {
        HedLen = 3;
        if (rSD.bAllowTestData)
          HedLen++;
        }
#endif
      if (MultOre)
        HedLen++;
      if (rSD.pModel->UseAsFlow())
        HedLen++;
      int BlkLen = D().NIntervals() + HedLen + 1;
      if (GraphOn())
        BlkLen+=2;
      StartBlk(BlkLen, HedLen, NULL);
      L=0;

      if (MultOre && ColWidth>14)
        ColWidth=14;

#ifdef USE_PSD_TB
      if (rSD.fAllowSizeEdits)
        {
        SetSpace(L, 1);
        SetDesc(L, "Test Data", -1, 10, 1, " ");
        SetCheckBox(L, "TstDat" , Id_TestDataChk , 2, 2 , "", true);
        L++;

        if (rSD.bAllowTestData)
          {
          SetSpace(L, 14);
          for (int ci=0; ci<NColmns; ci++)
            {
            int c=DDefn.DispColIndices[ci];
            if ((Columns(c).Editable())&&(Columns(c).iSpId >=0))
              {
              SetButton(L, "Set", Id_CopyTest+ci , ColWidth , 0, "");
              }
            else
              {
              SetSpace(L, ColWidth);
              }
            }
          L++;
          }
        }
#endif

      //heading...
      if (MultOre)
        {
        DoneGap=false;
        SetSpace(L, 4);
        for (int ci=0; ci<NColmns; ci++)
          {
          int c=DDefn.DispColIndices[ci];
          if (Columns(c).Reqd(Model()))
            {
            if (!Columns(c).ForCum() && !DoneGap)
              {
              DoneGap=true;
              SetDesc(L, "Size", -1, 10, 1, " ");
              }
            if (!Columns(c).ForCum())
              {
              S=Columns(c).sName();
              S+=':';
              S.SetLength(Min(S.Length(), ColWidth));
              SetDesc(L, S(), -1, ColWidth, 1, "");
              }
            }
          }
        L++;

        DoneGap=false;
        SetDesc(L, "Int",  -1, 4,  1, "");
        for (int ci=0; ci<NColmns; ci++)
          {
          int c=DDefn.DispColIndices[ci];
          if (Columns(c).Reqd(Model()))
            {
            if (!Columns(c).ForCum() && !DoneGap)
              {
              DoneGap=true;
              SetDesc(L, "Range", -1, 10, 1, " ");
              }
            if (!Columns(c).ForCum())
              {
              S=Columns(c).sSpName();
              S.SetLength(Min(S.Length(), ColWidth));
              SetDesc(L, S(), -1, ColWidth, 1, "");
              }
            }
          }
        L++;
        }
      else
        {
        DoneGap=false;
        DoneCumGap=false;
        SetDesc(L, "Int",  -1, 4,  1, "");
        for (int ci=0; ci<NColmns; ci++)
          {
          int c=DDefn.DispColIndices[ci];
          if (Columns(c).Reqd(Model()))
            {
            if (!Columns(c).ForCum() && !DoneCumGap && !DoneGap)
              {
              DoneGap=true;
              SetDesc(L, MultOre ? "Range" : "Size Range", -1, 10, 1, " ");
              }
            if (Columns(c).ForCum() && !DoneCumGap)
              {
              DoneCumGap = true;
              SetSpace(L, 2);
              S="Size";
              S.SetLength(Min(S.Length(), 10));
              SetDesc(L, S(), -1, 10, 1, " ");
              }
            S=(MultOre ? Columns(c).sSpName() : Columns(c).sName());
            S.SetLength(Min(S.Length(), ColWidth));
            SetDesc(L, S(), -1, ColWidth, 1, "");
            }
          }
        L++;
        }

      if (rSD.pModel->UseAsFlow())
        {
        //...
        if (MultOre)
          {
          DoneGap=false;
          S.Set("Qm Total(%s)", SD_Defn.YQmCnv.Text());
          SetDesc(L, S(), -1, 14, 2, " ");
          for (int ci=0; ci<NColmns; ci++)
            {
            int c=DDefn.DispColIndices[ci];
            if (Columns(c).Reqd(Model()))
              {
              if (!Columns(c).ForCum() && !DoneGap)
                DoneGap = true;
              if (!Columns(c).ForCum())
                {
                if (Columns(c).iSpId>=0)
                  {
                  FxdEdtFld* fef = SetParm(L, "", Id_YQmTtl+c, ColWidth, 2, "");
                  Tg.Set("%s.QmTtl.%s", FullObjTag(), Columns(c).sSpName());
                  //pView->SetTag(Tg(), SD_Defn.YQmCnv.Text());
                  SetTag(Tg(), SD_Defn.YQmCnv.Text());
                  }
                else
                  SetSpace(L, ColWidth);
                }
              }
            }
          SetSpace(L, 1);
          SetDesc(L, SD_Defn.YQmCnv.Text(), 0, 16, 0, "");
          L++;
          }
        else
          {
          DoneGap=false;
          DoneCumGap=false;
          S.Set("Qm Total(%s)", SD_Defn.YQmCnv.Text());
          SetDesc(L, S(), -1, 14, 2, " ");
          for (int ci=0; ci<NColmns; ci++)
            {
            int c=DDefn.DispColIndices[ci];
            if (Columns(c).Reqd(Model()))
              {
              if (!Columns(c).ForCum() && !DoneCumGap && !DoneGap)
                DoneGap = true;
              if (Columns(c).ForCum() && !DoneCumGap)
                {
                DoneCumGap = true;
                if (DoneGap)
                  {
                  SetSpace(L, 1);
                  SetDesc(L, SD_Defn.YQmCnv.Text(), 0, 12, 0, "");
                  //SetSpace(L, 2+10+1);
                  }
                }
              if (Columns(c).iSpId>=0)
                {
                FxdEdtFld* fef = SetParm(L, "", Id_YQmTtl+c, ColWidth, 2, "");
                Tg.Set("%s.QmTtl.%s", FullObjTag(), Columns(c).sSpName());
                //pView->SetTag(Tg(), SD_Defn.YQmCnv.Text());
                SetTag(Tg(), SD_Defn.YQmCnv.Text());
                }
              else
                SetSpace(L, ColWidth);
              }
            }
          SetSpace(L, 1);
          SetDesc(L, SD_Defn.YQmCnv.Text(), 0, 16, 0, "");
          L++;
          }
        }

      //..
      if (MultOre)
        {
        DoneGap=false;
        SetSpace(L, 4);
        for (int ci=0; ci<NColmns; ci++)
          {
          int c=DDefn.DispColIndices[ci];
          if (Columns(c).Reqd(Model()))
            {
            if (!Columns(c).ForCum() && !DoneGap)
              {
              DoneGap = true;
              SetDesc(L, SD_Defn.XCnv.Text(),  -1, 10,  2, " ");
              }
            if (!Columns(c).ForCum())
              {
              SetDesc(L, Columns(c).pCnv->Text(),  -1, ColWidth,  2, "");
              }
            }
          }
        }
      else
        {
        DoneGap=false;
        DoneCumGap=false;
        SetSpace(L, 4);
        for (int ci=0; ci<NColmns; ci++)
          {
          int c=DDefn.DispColIndices[ci];
          if (Columns(c).Reqd(Model()))
            {
            if (!Columns(c).ForCum() && !DoneCumGap && !DoneGap)
              {
              DoneGap = true;
              SetDesc(L, SD_Defn.XCnv.Text(),  -1, 10,  2, " ");
              }
            if (Columns(c).ForCum() && !DoneCumGap)
              {
              DoneCumGap = true;
              SetSpace(L, 2);
              SetDesc(L, SD_Defn.XCnv.Text(),  -1, 10,  2, " ");
              }
            SetDesc(L, Columns(c).pCnv->Text(),  -1, ColWidth,  2, "");
            }
          }
        }

      //..
      if (MultOre)
        {
        const int Intr = D().NIntervals();
        int iSz=fSzAscend ? 0 : Intr-1;
        int inc=fSzAscend ? 1 : -1;
        for ( ; iSz>=0 && iSz<Intr; iSz+=inc)
          {
          L++;
          S.Set("I%i", iSz);
          SetDesc(L, S(), -1 , 4, 3, "");
          DoneGap=false;
          for (int ci=0; ci<NColmns; ci++)
            {
            int c=DDefn.DispColIndices[ci];
            if (Columns(c).Reqd(Model()))
              {
              if (!Columns(c).ForCum() && !DoneGap)
                {
                DoneGap = true;
                SetParm(L, "",  Id_XIntRng, 10, 2, " ");
                Tg.Set("%s.%s.%s", FullObjTag(), S(), "SzRange");
                SetTag(Tg(), SD_Defn.XCnv.Text());
                }
              if (!Columns(c).ForCum())
                {
                SetParm(L, "",  Id_YData+c, ColWidth, 2, "");
                Tg.Set("%s.%s.%s", FullObjTag(), S(), Columns(c).sFullName());
                SetTag(Tg(), Columns(c).pCnv->Text());
                }
              }
            }
          }
        if (GraphOn())
          {
          //...
          L++;
          DoneGap=false;
          SetDesc(L, "Grf On",  -1, 14,  1, " ");
          const int FirstPen=3;
          int iPen=FirstPen;
          for (int ci=0; ci<NColmns; ci++)
            {
            int c=DDefn.DispColIndices[ci];
            if (Columns(c).Reqd(Model()))
              {
              if (!Columns(c).ForCum() && !DoneGap)
                DoneGap = true;
              if (!Columns(c).ForCum())
                {
                //SetButton(L, Columns(c).GrfOn() ? " On>Off " : " Off>On ", Id_YGrfOn+c, ColWidth-2, 0, "  ");
                SetButton(L, Columns(c).GrfOn() ? "  On>Off  " : "  Off>On  ", Id_YGrfOn+c, ColWidth, 0, "");
                iPen++;
                }
              }
            }

          //...
          L++;
          DoneGap=false;
          SetDesc(L, "Disp_Max",  -1, 14,  1, " ");
          for (ci=0; ci<NColmns; ci++)
            {
            int c=DDefn.DispColIndices[ci];
            if (Columns(c).Reqd(Model()))
              {
              if (!Columns(c).ForCum() && !DoneGap)
                DoneGap = true;
              if (!Columns(c).ForCum())
                {
                SetParm(L, "", Id_YFMax+c, ColWidth, 2, "");
                }
              }
            }
          }
        }
      else
        {
        const int Intr = D().NIntervals();
        int iSz=fSzAscend ? 0 : Intr-1;
        int inc=fSzAscend ? 1 : -1;
        for ( ; iSz>=0 && iSz<Intr; iSz+=inc)
          {
          L++;
          S.Set("I%i", iSz);
          SetDesc(L, S(), -1 , 4, 3, "");
          DoneGap=false;
          DoneCumGap=false;
          for (int ci=0; ci<NColmns; ci++)
            {
            int c=DDefn.DispColIndices[ci];
            if (Columns(c).Reqd(Model()))
              {
              if (!Columns(c).ForCum() && !DoneCumGap && !DoneGap)
                {
                DoneGap = true;
                SetParm(L, "",  Id_XIntRng, 10, 2, " ");
                Tg.Set("%s.%s.%s", FullObjTag(), S(), "SzRange");
                SetTag(Tg(), SD_Defn.XCnv.Text());
                }
              if (Columns(c).ForCum() && !DoneCumGap)
                {
                DoneCumGap = true;
                SetSpace(L, 2);
                SetParm(L, "",  Id_XInt, 10, 2, " ");
                Tg.Set("%s.%s.%s", FullObjTag(), S(), "Size");
                SetTag(Tg(), SD_Defn.XCnv.Text());
                }
              SetParm(L, "",  Id_YData+c, ColWidth, 2, "");
              Tg.Set("%s.%s.%s", FullObjTag(), S(), Columns(c).sFullName());
              SetTag(Tg(), Columns(c).pCnv->Text());
              }
            }
          }
        if (GraphOn())
          {
          //...
          L++;
          DoneGap=false;
          DoneCumGap=false;
          SetDesc(L, "Grf On",  -1, 14,  1, " ");
          const int FirstPen=3;
          int iPen=FirstPen;
          for (int ci=0; ci<NColmns; ci++)
            {
            int c=DDefn.DispColIndices[ci];
            if (Columns(c).Reqd(Model()))
              {
              if (!Columns(c).ForCum() && !DoneCumGap && !DoneGap)
                DoneGap = true;
              if (Columns(c).ForCum() && !DoneCumGap)
                {
                DoneCumGap = true;
                if (DoneGap)
                  SetSpace(L, 2+10+1);
                }
              //SetButton(L, Columns(c).GrfOn() ? " On>Off " : " Off>On ", Id_YGrfOn+c, ColWidth-2, 0, "  ");
              SetButton(L, Columns(c).GrfOn() ? "  On>Off  " : "  Off>On  ", Id_YGrfOn+c, ColWidth, 0, "");
              iPen++;
              }
            }

          //...
          L++;
          DoneGap=false;
          DoneCumGap=false;
          SetDesc(L, "Disp_Max",  -1, 14,  1, " ");
          for (ci=0; ci<NColmns; ci++)
            {
            int c=DDefn.DispColIndices[ci];
            if (Columns(c).Reqd(Model()))
              {
              if (!Columns(c).ForCum() && !DoneCumGap && !DoneGap)
                DoneGap = true;
              if (Columns(c).ForCum() && !DoneCumGap)
                {
                DoneCumGap = true;
                if (DoneGap)
                  SetSpace(L, 2+10+1);
                }
              SetParm(L, "", Id_YFMax+c, ColWidth, 2, "");
              }
            }
          }
        }

      }
      
    //Cumulative data...
    if (MultOre)
    if (1) // Cum Data Blk
      {
      int L=0;
      int HedLen = 3;
      if (rSD.pModel->UseAsFlow())
        HedLen++;
      int BlkLen = D().NIntervals() + HedLen + 1;
      if (GraphOn())
        BlkLen+=2;
      StartBlk(BlkLen, HedLen, NULL);
      L=0;

      if (MultOre)
        {
        //heading...
        DoneGap=false;
        SetSpace(L, 4);
        for (int ci=0; ci<NColmns; ci++)
          {
          int c=DDefn.DispColIndices[ci];
          if (Columns(c).Reqd(Model()))
            {
            if (Columns(c).ForCum() && !DoneGap)
              {
              DoneGap=true;
              SetDesc(L, "Size", -1, 10, 1, " ");
              }
            if (Columns(c).ForCum())
              {
              S=Columns(c).sName();
              S+=':';
              S.SetLength(Min(S.Length(), ColWidth));
              SetDesc(L, S(), -1, ColWidth, 1, "");
              }
            }
          }
        L++;

        DoneGap=false;
        SetDesc(L, "Int",  -1, 4,  1, "");
        for (int ci=0; ci<NColmns; ci++)
          {
          int c=DDefn.DispColIndices[ci];
          if (Columns(c).Reqd(Model()))
            {
            if (Columns(c).ForCum() && !DoneGap)
              {
              DoneGap=true;
              SetDesc(L, "", -1, 10, 1, " ");
              }
            if (Columns(c).ForCum())
              {
              S=Columns(c).sSpName();
              S.SetLength(Min(S.Length(), ColWidth));
              SetDesc(L, S(), -1, ColWidth, 1, "");
              }
            }
          }
        L++;

        if (rSD.pModel->UseAsFlow())
          {
          //...
          DoneGap=false;
          S.Set("Qm Total(%s)", SD_Defn.YQmCnv.Text());
          SetDesc(L, S(), -1, 14, 2, " ");
          for (int ci=0; ci<NColmns; ci++)
            {
            int c=DDefn.DispColIndices[ci];
            if (Columns(c).Reqd(Model()))
              {
              if (Columns(c).ForCum() && !DoneGap)
                DoneGap = true;
              if (Columns(c).ForCum())
                {
                if (Columns(c).iSpId>=0)
                  {
                  FxdEdtFld* fef = SetParm(L, "", Id_YQmTtl+c, ColWidth, 2, "");
                  Tg.Set("%s.QmTtl.%s", FullObjTag(), Columns(c).sSpName());
                  //pView->SetTag(Tg(), SD_Defn.YQmCnv.Text());
                  SetTag(Tg(), SD_Defn.YQmCnv.Text());
                  }
                else
                  SetSpace(L, ColWidth);
                }
              }
            }
          SetSpace(L, 1);
          SetDesc(L, SD_Defn.YQmCnv.Text(), 0, 16, 0, "");
          L++;
          }

        //..
        DoneGap=false;
        SetSpace(L, 4);
        for (int ci=0; ci<NColmns; ci++)
          {
          int c=DDefn.DispColIndices[ci];
          if (Columns(c).Reqd(Model()))
            {
            if (Columns(c).ForCum() && !DoneGap)
              {
              DoneGap = true;
              SetDesc(L, SD_Defn.XCnv.Text(),  -1, 10,  2, " ");
              }
            if (Columns(c).ForCum())
              {
              SetDesc(L, Columns(c).pCnv->Text(),  -1, ColWidth,  2, "");
              }
            }
          }

        //..
        const int Intr = D().NIntervals();
        int iSz=fSzAscend ? 0 : Intr-1;
        int inc=fSzAscend ? 1 : -1;
        for ( ; iSz>=0 && iSz<Intr; iSz+=inc)
          {
          L++;
          S.Set("I%i", iSz);
          SetDesc(L, S(), -1 , 4, 3, "");
          DoneGap=false;
          for (int ci=0; ci<NColmns; ci++)
            {
            int c=DDefn.DispColIndices[ci];
            if (Columns(c).Reqd(Model()))
              {
              if (Columns(c).ForCum() && !DoneGap)
                {
                DoneGap = true;
                SetParm(L, "",  Id_XInt, 10, 2, " ");
                Tg.Set("%s.%s.%s", FullObjTag(), S(), "Size");
                SetTag(Tg(), SD_Defn.XCnv.Text());
                }
              if (Columns(c).ForCum())
                {
                SetParm(L, "",  Id_YData+c, ColWidth, 2, "");
                Tg.Set("%s.%s.%s", FullObjTag(), S(), Columns(c).sFullName());
                SetTag(Tg(), Columns(c).pCnv->Text());
                }
              }
            }
          }
        if (GraphOn())
          {
          //...
          L++;
          DoneGap=false;
          SetDesc(L, "Grf On",  -1, 14,  1, " ");
          const int FirstPen=3;
          int iPen=FirstPen;
          for (int ci=0; ci<NColmns; ci++)
            {
            int c=DDefn.DispColIndices[ci];
            if (Columns(c).Reqd(Model()))
              {
              if (Columns(c).ForCum() && !DoneGap)
                DoneGap = true;
              if (Columns(c).ForCum())
                {
                //SetButton(L, Columns(c).GrfOn() ? " On>Off " : " Off>On ", Id_YGrfOn+c, ColWidth-2, 0, "  ");
                SetButton(L, Columns(c).GrfOn() ? "  On>Off  " : "  Off>On  ", Id_YGrfOn+c, ColWidth, 0, "");
                iPen++;
                }
              }
            }

          //...
          L++;
          DoneGap=false;
          SetDesc(L, "Disp_Max",  -1, 14,  1, " ");
          for (ci=0; ci<NColmns; ci++)
            {
            int c=DDefn.DispColIndices[ci];
            if (Columns(c).Reqd(Model()))
              {
              if (Columns(c).ForCum() && !DoneGap)
                DoneGap = true;
              if (Columns(c).ForCum())
                {
                SetParm(L, "", Id_YFMax+c, ColWidth, 2, "");
                }
              }
            }
          }
        }

      }
    
    if (1) // Base Blk
      {
      StartBlk(6, 0, NULL);
      int L=0;
      SetDesc(L, "Size Intervals :",  -1, 16,  2, " ");
      Strng SzTg(fSzAscend ? " > Descending " : " > Ascending ");
      SetButton(L, SzTg(),  Id_SzAscend, 14, 0, "");
      L++;
      SetDesc(L, "Graph:", -1, 6, 2, " ");
      Strng Tg(GraphOn() ? " On>Off " : " Off>On ");
      SetButton(L, Tg(),  Id_GrOn, 10, 0, "");
      if (GraphOn())
        {
        SetSpace(L, 1);
        SetDParm(L, "Width", 6, "", Id_GrWidth, 2, 2, "");
        SetSpace(L, 1);
        SetDParm(L, "Height", 7, "", Id_GrHeight, 2, 2, "");
        }
      L++;
      SetDParm(L, "Top Size", 12, "", Id_TopSize, 12, 2, " ");
      Tg.Set("%s.%s", FullObjTag(), "TopSize");
      SetTag(Tg(), SD_Defn.TBCnv.Text());
      SetDesc(L, SD_Defn.TBCnv.Text(),  -1, 10, 0, " ");
      L++;
      SetDParm(L, "Bottom Size", 12, "", Id_BotSize, 12, 2, " ");
      Tg.Set("%s.%s", FullObjTag(), "BottomSize");
      SetTag(Tg(), SD_Defn.TBCnv.Text());
      SetDesc(L, SD_Defn.TBCnv.Text(),  -1, 10, 0, " ");
      L++;
      SetDParm(L, "Interval Count", 16, "", Id_IntervalCnt, 8, 0, " ");
      Tg.Set("%s.%s", FullObjTag(), "IntervalCnt");
      SetTag(Tg());
      L++;
      }
    }

  StartPage("MSz", true);

  if (pD)
    {
    CSD_DistDefn &DDefn=pD->Defn();

    flag DoneGap=false;
    flag DoneCumGap=false;
    const flag MultOre = (NColmns && !Columns(0).SimpleNames());
    if (1) // Measurements Blk
      {
      int HedLen = (MultOre ? 2 : 1);
      StartBlk(DDefn.Measurements.GetSize()+HedLen, HedLen, NULL);
      int L=0;
      //...
      if (MultOre)
        {
        DoneGap=false;
        DoneCumGap=false;
        SetSpace(L, 4);
        for (int ci=0; ci<NColmns; ci++)
          {
          int c=DDefn.DispColIndices[ci];
          if (Columns(c).Reqd(Model()))
            {
            if (!Columns(c).ForCum() && !DoneCumGap && !DoneGap)
              {
              DoneGap=true;
              SetSpace(L, 10+1);
              }
            if (Columns(c).ForCum() && !DoneCumGap)
              {
              DoneCumGap = true;
              SetSpace(L, 2+10+1);
              }
            S=Columns(c).sName();
            S+=':';
            S.SetLength(Min(S.Length(), ColWidth));
            SetDesc(L, S(), -1, ColWidth, 1, "");
            }
          }
        L++;
        }

      //...
      DoneGap=false;
      DoneCumGap=false;
      SetDesc(L, "Measurements", -1, 14, 0, " ");
      for (int ci=0; ci<NColmns; ci++)
        {
        int c=DDefn.DispColIndices[ci];
        if (Columns(c).Reqd(Model()))
          {
          if (!Columns(c).ForCum() && !DoneCumGap && !DoneGap)
            {
            DoneGap=true;
            SetSpace(L, 1);
            }
          if (Columns(c).ForCum() && !DoneCumGap)
            {
            DoneCumGap = true;
            SetSpace(L, (DoneGap ? 2+10+1 : 2+1));
            }
          S=(MultOre ? Columns(c).sSpName() : Columns(c).sName());
          S.SetLength(Min(S.Length(), ColWidth));
          SetDesc(L, S(), -1, ColWidth, 1, "");
          }
        }
      L++;
      for (int iSz=0; iSz<DDefn.Measurements.GetSize(); iSz++)
        {
        DoneGap=false;
        DoneCumGap=false;
        CSD_Measurement &M=DDefn.Measurements[iSz];
        SetDesc(L, M.m_sName(),  -1, 14,  2, "");
        if (M.m_eType!=eSDMT_Text)
          {
          for (ci=0; ci<NColmns; ci++)
            {
            int c=DDefn.DispColIndices[ci];
            //dbgpln("BBB:%3i %3i %3i %-20s %-20s", iSz, ci, c, M.m_sName(), Columns(c).sSpName());
            if (Columns(c).Reqd(Model()))
              {
              if (!Columns(c).ForCum() && !DoneCumGap && !DoneGap)
                {
                DoneGap=true;
                SetSpace(L, 1);
                }
              if (Columns(c).ForCum() && !DoneCumGap)
                {
                DoneCumGap = true;
                if (DoneGap)
                  {
                  SetSpace(L, 1);
                  SetDesc(L, M.Cnv.Text()/*SD_Defn.XCnv.Text()*/, 0, 12, 0, "");
                  }
                else
                  SetSpace(L, 2+1);
                }
              SetParm(L, "", Id_dSize+iSz+c*MaxCSDMeasurements, ColWidth, 2, "");
              Tg.Set("%s.%s.%s", FullObjTag(), M.m_sName(), Columns(c).sSpName());
              SetTag(Tg(), M.Cnv.Text());//SD_Defn.XCnv.Text());
              }
            }
          }
        SetSpace(L, 1);
        SetDesc(L, M.Cnv.Text()/*SD_Defn.XCnv.Text()*/, 0, 16, 0, "");
        L++;
        }
      }
    }

#ifdef USE_PSD_TB
    // Build the EditView for the PSDTB Pages

    if (rSD.bAllowTestData)
    {
      rSD.m_PSDTB->SetIDOffsets( Id_Last , XID_Last );
      rSD.m_PSDTB->Build(this);
    }
#endif

  }

//---------------------------------------------------------------------------

void SQSzDist1Edt::ChangeLayout(rFxdEdtPage Pg, int TotalRows, int TotalCols)
  {
  int d=DistFromPgNo(Pg.No-iPg1);
  if (GraphOn() && (d>=0))//(Pg.No-iPg1>=0))
    {
    Pg.MoveRows(-32000);
    Pg.MoveRows(GraphHeight());
    }
  }

// --------------------------------------------------------------------------

char* SQSzDist1Edt::GetRangeLbl(int i, Strng& Str, flag Range)
  {
  if (Range)
    {
    if (fSzAscend)
      {
      if (i>=D().rIntervals.GetUpperBound())  
        {
        Strng S;
        SD_Defn.XFmt.FormatFloat(SD_Defn.XCnv.Human(D().rIntervals[i-1]), S);
        Str="+";
        Str+=S;
        }
      else
        {
        Strng S;
        SD_Defn.XFmt.FormatFloat(SD_Defn.XCnv.Human(D().rIntervals[i]), S);
        Str="-";
        Str+=S;
        }
      }
    else
      {
      if (i<D().rIntervals.GetUpperBound())  
        {
        i=D().rIntervals.GetUpperBound()-i;
        Strng S;
        SD_Defn.XFmt.FormatFloat(SD_Defn.XCnv.Human(D().rIntervals[i-1]), S);
        Str="+";
        Str+=S;
        }
      else
        {
        Strng S;
        SD_Defn.XFmt.FormatFloat(SD_Defn.XCnv.Human(D().rIntervals[0]), S);
        Str="-";
        Str+=S;
        }
      }
    }
  else
    {
    if (fSzAscend)
      {
      SD_Defn.XFmt.FormatFloat(SD_Defn.XCnv.Human(D().rIntervals[i]), Str);
      }
    else
      {
      i=D().rIntervals.GetUpperBound()-i;
      SD_Defn.XFmt.FormatFloat(SD_Defn.XCnv.Human(D().rIntervals[i]), Str);
      }
    }
  return Str();
  }

//---------------------------------------------------------------------------

inline double SQSzDist1Edt::GetYData(CSD_Column &C, int iInt)
  {
  return GetYData(C)[iInt];
  }

//---------------------------------------------------------------------------

inline CDArray & SQSzDist1Edt::GetYData(CSD_Column &C)
  {
  CSD_Distribution & D_ = D();
  double x0 = D_.PriSp[0/*C.iSpId*/]->FracPass[0];
  double x1 = D_.PriSp[0/*C.iSpId*/]->FracPass[1];
  if (D().CalculateResults(&rSD.pModel->m_Ovr, rSD.pModel->MArray(),/*iWorkDist,*/ C.iDataId, C.iSpId))
    return D().Results();
  return *((CDArray*)NULL);
  }

//---------------------------------------------------------------------------

void SQSzDist1Edt::SetYData(CSD_Column &C, int iInt, double Val)
  {
  switch (C.iDataId)
    {
    case DI_MFp: 
      if (C.iSpId>=0)
        D().PriSp[C.iSpId]->FracPass[iInt]=Val;
      D().PriSp[C.iSpId]->FracPass.AdjustCumulativeEnd(1.0, 0.0, 1.0);
      break;
    #if UseExtraColumns
    case DI_NFp: 
      D().SetCountFrac(&rSD.pModel->m_Ovr, rSD.pModel->MArray(), C.iSpId, iInt, Val);//, rSD.WorkSpace); 
      break;
    case DI_NpM: 
      D().SetSpCount(&rSD.pModel->m_Ovr, rSD.pModel->MArray(), C.iSpId, iInt, Val);//, rSD.WorkSpace); 
      break;
    case DI_N : 
    case DI_Qn: 
      if (D().CalculateResults(&rSD.pModel->m_Ovr, rSD.pModel->MArray(),/*iWorkDist,*/ C.iDataId, C.iSpId))
        {
        D().Results()[iInt]=Val;
        if (C.iSpId>=0)
          D().PriSp[C.iSpId]->FracPass[iInt]=Val;
        D().PriSp[C.iSpId]->FracPass.AdjustCumulativeEnd(1.0, 0.0, 1.0);
        }
      break;
    #endif
    }
  }

//---------------------------------------------------------------------------

void SQSzDist1Edt::Load(FxdEdtInfo &EI, Strng & Str)
  {
  //dbgpln("...: %3i %3i %3i %3i %3i ", EI.BlkId, EI.BlkRowNo, EI.FieldId, EI.Index, EI.PageNo);
  if (CurrentBlk(EI))
    {//header 1
    int i=(int)(EI.BlkRowNo-EI.Index);
    }
  int d=DistFromPgNo(EI.PageNo-iPg1);
  if (d<0)
    {
    if (CurrentBlk(EI))
      {//header 2
      }
    }
  else  
    {
    SetWorkDist(d);
    if (CurrentBlk(EI))
      {//header 2
      int p=EI.PageNo;
      int i=(int)(EI.BlkRowNo-EI.Index);
      switch (EI.FieldId)
        {
        case Id_XMin :  
          {
          SD_Defn.XFmt.FormatFloat(SD_Defn.XCnv.Human(XDispMin()), Str);
          }
          break;
        case Id_XMax :  
          {
          SD_Defn.XFmt.FormatFloat(SD_Defn.XCnv.Human(XDispMax()), Str);
          }
          break;
        case Id_CurShow:  
          {
          for (int i=0; i<NColumns(); i++)
            if (!Columns(i).On() && Columns(i).Avail(Model()))
              {
              Str=Columns(i).DispName();
              break;
              }
          break;
          }
        case Id_CurHide:  
          {
          for (int i=0; i<NColumns(); i++)
            if (Columns(i).On() && Columns(i).Avail(Model()))
              {
              Str=Columns(i).DispName();
              break;
              }
          break;
          }
        #if WithSauterMeanD
        case Id_RqdSauterl32:
          {
          //KGAFIX
          Str="Set";
          break;
          }
        #endif
        }
      }

    // Measurements 
    if (0)//CurrentBlk(EI))
      {
      int p=EI.PageNo;
      int i=(int)(EI.BlkRowNo-EI.Index);
      if (EI.FieldId>=Id_dSize && EI.FieldId<Id_dSize+MaxCSDMeasurements*(1+NColumns()))
        {
        int c=(EI.FieldId-Id_dSize) / MaxCSDMeasurements;
        int i=(EI.FieldId-Id_dSize) % MaxCSDMeasurements;
        int s=Columns(c).iSpId;
        CSD_Measurement &M=SD_Defn.GetDist(d)->Measurements[i];

        double Meas=dNAN;
        switch (M.m_eType)
          {
          case eSDMT_SizePassFrac: 
            Meas=rSD.SizePassingFraction(d, s, M.m_dValue); 
            break;
          case eSDMT_FracPassSize:
            Meas=rSD.FractionPassingSize(d, s, M.m_dValue);
            break;
          case eSDMT_SAM:
            Meas=rSD.SurfaceAreaPerGram(d, s);
            break;
          case eSDMT_SAL:
            Meas=rSD.SurfaceAreaPerLitre(d, s);
            break;
          case eSDMT_PPG:
            Meas=rSD.ParticleCountPerMass(d, s);
            break;
          }
        //SD_Defn.XFmt.FormatFloat(SD_Defn.XCnv.Human(Meas), Str);
        M.Fmt.FormatFloat(M.Cnv.Human(Meas), Str);
        EI.Fld->fEditable=false;
        }
      }
    
    if (CurrentBlk(EI))
      {//data
      int p=EI.PageNo;
      int i=(int)(EI.BlkRowNo-EI.Index);
      switch (EI.FieldId)
        {
#ifdef USE_PSD_TB
       case Id_TestDataChk: // Testing
            Str.Set("%i", rSD.bAllowTestData);
            break;
#endif

        case Id_XIntRng:
        case Id_XInt:
          GetRangeLbl(i, Str, EI.FieldId==Id_XIntRng);
          EI.Fld->fEditable=false;
          break;
        default:
          if (EI.FieldId>=Id_YData && EI.FieldId<Id_YData+MaxColumns)
            {
            if (!fSzAscend)
              i = D().rIntervals.GetUpperBound()-i;
            const int c = EI.FieldId-Id_YData;
            CSD_Column &C = Columns(c);
            const double dd = GetYData(C, i);
            C.pFmt->FormatFloat(C.pCnv->Human(dd), Str);
            EI.Fld->fEditable = (C.Editable() && C.iSpId>=0 && rSD.fAllowSizeEdits);
            }
          else if (EI.FieldId>=Id_YQmTtl && EI.FieldId<Id_YQmTtl+NColumns())
            {
            CSD_Column &C=Columns(EI.FieldId-Id_YQmTtl);
            SD_Defn.YQmFmt.FormatFloat(SD_Defn.YQmCnv.Human(D().GetMass(&rSD.pModel->m_Ovr, rSD.pModel->MArray(), C.iSpId)), Str);
            EI.Fld->fEditable = false;
			      }
          else if (EI.FieldId>=Id_YMin && EI.FieldId<Id_YMin+NColumns())
            {
            CSD_Column &C=Columns(EI.FieldId-Id_YMin);
            C.pFmt->FormatFloat(C.pCnv->Human(C.dDispMin), Str);
			      }
          else if (EI.FieldId>=Id_YFMax && EI.FieldId<Id_YFMax+NColumns())
            {
            CSD_Column &C=Columns(EI.FieldId-Id_YFMax);
            C.pFmt->FormatFloat(C.pCnv->Human(C.dFDispMax), Str);
            EI.Fld->fEditable = (!AutoScale());
			      }
        }
      }
    const flag MultOre = (NColumns() && !Columns(0).SimpleNames());
    if (MultOre)
    if (CurrentBlk(EI))
      {//cum data
      int p=EI.PageNo;
      int i=(int)(EI.BlkRowNo-EI.Index);
      switch (EI.FieldId)
        {
        case Id_XIntRng:
        case Id_XInt:
          GetRangeLbl(i, Str, EI.FieldId==Id_XIntRng);
          EI.Fld->fEditable=false;
          break;
        default:
          if (EI.FieldId>=Id_YData && EI.FieldId<Id_YData+MaxColumns)
            {
            if (!fSzAscend)
              i = D().rIntervals.GetUpperBound()-i;
            const int c = EI.FieldId-Id_YData;
            CSD_Column &C = Columns(c);
            const double dd = GetYData(C, i);
            C.pFmt->FormatFloat(C.pCnv->Human(dd), Str);
            EI.Fld->fEditable = (C.Editable() && C.iSpId>=0 && rSD.fAllowSizeEdits);
            }
          else if (EI.FieldId>=Id_YQmTtl && EI.FieldId<Id_YQmTtl+NColumns())
            {
            CSD_Column &C=Columns(EI.FieldId-Id_YQmTtl);
            SD_Defn.YQmFmt.FormatFloat(SD_Defn.YQmCnv.Human(D().GetMass(&rSD.pModel->m_Ovr, rSD.pModel->MArray(), C.iSpId)), Str);
            EI.Fld->fEditable = false;
			      }
          else if (EI.FieldId>=Id_YMin && EI.FieldId<Id_YMin+NColumns())
            {
            CSD_Column &C=Columns(EI.FieldId-Id_YMin);
            C.pFmt->FormatFloat(C.pCnv->Human(C.dDispMin), Str);
			      }
          else if (EI.FieldId>=Id_YFMax && EI.FieldId<Id_YFMax+NColumns())
            {
            CSD_Column &C=Columns(EI.FieldId-Id_YFMax);
            C.pFmt->FormatFloat(C.pCnv->Human(C.dFDispMax), Str);
            EI.Fld->fEditable = (!AutoScale());
			      }
        }
      }
    if (CurrentBlk(EI))
      {//other
      switch (EI.FieldId)
        {
        case Id_SzAscend:  Str.Set("%i", fSzAscend); break;
        case Id_GrWidth:   Str.Set("%i", iGraphWidth); break;
        case Id_GrHeight:  Str.Set("%i", iGraphHeight); break;
        case Id_GrOn    :  Str.Set("%i", iGraphOn); break;
        case Id_TopSize :  
          SD_Defn.TBFmt.FormatFloat(SD_Defn.TBCnv.Human(D().TopSize()), Str);
          EI.Fld->fEditable=false;
          break;
        case Id_BotSize :
          SD_Defn.TBFmt.FormatFloat(SD_Defn.TBCnv.Human(D().BottomSize()), Str);
          EI.Fld->fEditable=false;
          break;
        case Id_IntervalCnt : 
          Str.Set("%i", D().NIntervals());//SD_Defn.GetDist(d)->NIntervals());
          EI.Fld->fEditable=false; 
          break;
        }
      }

    // Measurements 
    if (1)//CurrentBlk(EI))
      {
      int p=EI.PageNo;
      int i=(int)(EI.BlkRowNo-EI.Index);
      if (EI.FieldId>=Id_dSize && EI.FieldId<Id_dSize+MaxCSDMeasurements*(1+NColumns()))
        {
        int c=(EI.FieldId-Id_dSize) / MaxCSDMeasurements;
        int i=(EI.FieldId-Id_dSize) % MaxCSDMeasurements;
        int s=Columns(c).iSpId;
        //dbgpln("---: %3i %3i %3i ", c,i,s);

        const long NMeas=SD_Defn.GetDist(d)->Measurements.GetSize();
        if (i<NMeas)
          {
          CSD_Measurement &M=SD_Defn.GetDist(d)->Measurements[i];
          double Meas=dNAN;
          switch (M.m_eType)
            {
            case eSDMT_SizePassFrac: 
              Meas=rSD.SizePassingFraction(d, s, M.m_dValue); 
              break;
            case eSDMT_FracPassSize:
              Meas=rSD.FractionPassingSize(d, s, M.m_dValue);
              break;
            case eSDMT_SAM:
              Meas=rSD.SurfaceAreaPerGram(d, s);
              break;
            case eSDMT_SAL:
              Meas=rSD.SurfaceAreaPerLitre(d, s);
              break;
            case eSDMT_PPG:
              Meas=rSD.ParticleCountPerMass(d, s);
              break;
            }
          //SD_Defn.XFmt.FormatFloat(SD_Defn.XCnv.Human(Meas), Str);
          M.Fmt.FormatFloat(M.Cnv.Human(Meas), Str);
          }
        EI.Fld->fEditable=false;
        }
      }
    }

#ifdef USE_PSD_TB
   // Load PSDBTB Data
//    if (CurrentBlk(EI))
//    {

      //if (rSD.bAllowTestData)
      //{

         rSD.m_PSDTB->SetIDOffsets( Id_Last , XID_Last );
         rSD.m_PSDTB->Load(this,EI,Str);
      //}
//    }
#endif

  }

//---------------------------------------------------------------------------

long SQSzDist1Edt::Parse(FxdEdtInfo &EI, Strng & Str)
  {
  long Fix=0; //set Fix=1 to redraw graph
  
  if (CurrentBlk(EI))
    {//header 1
    int i=(int)(EI.BlkRowNo-EI.Index);
    }

  int d=DistFromPgNo(EI.PageNo-iPg1);
  if (d<0)
    {
    if (CurrentBlk(EI))
      {//header 2
      }
    }
  else
    {
    SetWorkDist(d);
    if (CurrentBlk(EI))
      {//header 2
      //bObjModified=1;
      int p=EI.PageNo;
      int i=(int)(EI.BlkRowNo-EI.Index);
      switch (EI.FieldId)
        {
        case Id_XMin :  
          SetXDispMin(SD_Defn.XCnv.Normal(SafeAtoF(Str, 0.0))); 
          View().DoReload();
          break;
        case Id_XMax :  
          SetXDispMax(SD_Defn.XCnv.Normal(SafeAtoF(Str, 1.0)));  
          View().DoReload();
          break;
        case Id_CurShow:    
        case Id_CurHide:    
          {
          for (int i=0; i<NColumns(); i++)
            if (Str.XStrICmp(Columns(i).DispName())==0)
              {
              Columns(i).SetOn(!Columns(i).On());
              View().DoRebuild();
              Fix=1;
              break;
              }
          break;

          }
        #if WithSauterMeanD
        case Id_RqdSauterl32:
          {
//          D().rIntervals.GetUpperBound()-i;
//          if (EI.FieldId>=Id_YData && EI.FieldId<Id_YData+NColumns())
//            {
//            CSD_Column &C=Columns(EI.FieldId-Id_YData);
//            SetYData(C, i, C.pCnv->Normal(SafeAtoF(Str, 0.0))); 
//KGAFIX
// this does not auto update on entry ???
          double psd[1024]={0};
          double *x=&D().rIntervals[0];
          double l32=SafeAtoF(Str, dNAN);
          long lNInts=D().rIntervals.GetSize();
          if (Valid(l32) && l32>0.0)
            {
            dist_sauter__(psd, l32, lNInts, x);
            number_to_mass__(psd, lNInts, x);
            for (int i=0; i<lNInts; i++)
              D().PriSp[0/*C.iSpId*/]->FracPass[i]=psd[i];
            }
          //??
          break;
          }
        #endif
        }
      }
    
    // Measurements
    if (0)//CurrentBlk(EI))
      {
      }

    if (CurrentBlk(EI))
      {//data
      int p=EI.PageNo;
      int i=(int)(EI.BlkRowNo-EI.Index);
      switch (EI.FieldId)
        {
        case Id_XIntRng :
          /*sprintf(Str, "%g", D().rIntervals[i]); */ 
          break;
        case Id_XInt :
          /*sprintf(Str, "%g", D().rIntervals[i]); */ 
          break;
        default:
          if (!fSzAscend)
            i=D().rIntervals.GetUpperBound()-i;
          if (EI.FieldId>=Id_YData && EI.FieldId<Id_YData+NColumns())
            {
            const int c = EI.FieldId-Id_YData;
            CSD_Column &C=Columns(c);
            const double dd = C.pCnv->Normal(SafeAtoF(Str, 0.0));
            SetYData(C, i, dd); 
            View().DoReload();
            }
          else if (EI.FieldId>=Id_YQmTtl && EI.FieldId<Id_YQmTtl+NColumns())
            {
            //CSD_Column &C=Columns(EI.FieldId-Id_YQmTtl);
            //C.dFDispMax=C.pCnv->Normal(SafeAtoF(Str, 1.0));
            //View().DoReload();
            }
          else if (EI.FieldId>=Id_YMin && EI.FieldId<Id_YMin+NColumns())
            {
				    CSD_Column &C=Columns(EI.FieldId-Id_YMin);
            C.dDispMin=C.pCnv->Normal(SafeAtoF(Str, 0.0));
            View().DoReload();
            }
          else if (EI.FieldId>=Id_YFMax && EI.FieldId<Id_YFMax+NColumns())
            {
            CSD_Column &C=Columns(EI.FieldId-Id_YFMax);
            C.dFDispMax=C.pCnv->Normal(SafeAtoF(Str, 1.0));
            View().DoReload();
            }
        }
      }
    const flag MultOre = (NColumns() && !Columns(0).SimpleNames());
    if (MultOre)
    if (CurrentBlk(EI))
      {//cum data
      int p=EI.PageNo;
      int i=(int)(EI.BlkRowNo-EI.Index);
      switch (EI.FieldId)
        {
        case Id_XIntRng :
          /*sprintf(Str, "%g", D().rIntervals[i]); */ 
          break;
        case Id_XInt :
          /*sprintf(Str, "%g", D().rIntervals[i]); */ 
          break;
        default:
          if (!fSzAscend)
            i=D().rIntervals.GetUpperBound()-i;
          if (EI.FieldId>=Id_YData && EI.FieldId<Id_YData+NColumns())
            {
            const int c = EI.FieldId-Id_YData;
            CSD_Column &C=Columns(c);
            const double dd = C.pCnv->Normal(SafeAtoF(Str, 0.0));
            SetYData(C, i, dd); 
            View().DoReload();
            }
          else if (EI.FieldId>=Id_YQmTtl && EI.FieldId<Id_YQmTtl+NColumns())
            {
            //CSD_Column &C=Columns(EI.FieldId-Id_YQmTtl);
            //C.dFDispMax=C.pCnv->Normal(SafeAtoF(Str, 1.0));
            //View().DoReload();
            }
          else if (EI.FieldId>=Id_YMin && EI.FieldId<Id_YMin+NColumns())
            {
				    CSD_Column &C=Columns(EI.FieldId-Id_YMin);
            C.dDispMin=C.pCnv->Normal(SafeAtoF(Str, 0.0));
            View().DoReload();
            }
          else if (EI.FieldId>=Id_YFMax && EI.FieldId<Id_YFMax+NColumns())
            {
            CSD_Column &C=Columns(EI.FieldId-Id_YFMax);
            C.dFDispMax=C.pCnv->Normal(SafeAtoF(Str, 1.0));
            View().DoReload();
            }
        }
      }

    if (CurrentBlk(EI))
      {//other
      switch (EI.FieldId)
        {
        case Id_SzAscend:
          fSzAscend = Str.SafeAtoL()!=0;
          View().DoRebuild();
          break;
        case Id_GrWidth:
          iGraphWidth = Range(10L, Str.SafeAtoL(), 60L);
          View().DoRebuild();
          break;
        case Id_GrHeight:  
          iGraphHeight = Range(5L, Str.SafeAtoL(), 30L);
          View().DoRebuild();
          break;
        case Id_GrOn:  
          iGraphOn= Range(0L, Str.SafeAtoL(), 1L);
          View().DoRebuild();
          break;
        case Id_TopSize:
          break;
        case Id_BotSize:
          break;
        case Id_IntervalCnt:
          break;
        }
      }
    }


#ifdef USE_PSD_TB
//    if (CurrentBlk(EI))
//      {//PSD

      // Parse PSDBTB Data
      //if (rSD.bAllowTestData)
      //  {

 
        rSD.m_PSDTB->SetIDOffsets( Id_Last , XID_Last );
        rSD.m_PSDTB->Parse(this,EI,Str);
//        }
//      }
#endif

  return Fix;
  }

//---------------------------------------------------------------------------

long SQSzDist1Edt::ButtonPushed(FxdEdtInfo &EI, Strng & Str)
  {
  long Fix=0; //set Fix=1 to redraw graph
  if (CurrentBlk(EI))
    {//header 1
    int i=(int)(EI.BlkRowNo-EI.Index);
    if (EI.FieldId>=Id_DistOn && EI.FieldId<Id_DistOn+MaxDistributions)
      {
      int d=EI.FieldId-Id_DistOn;
      rSD.iDistUsed=d;
      if (!rSD.Distributions[d])
        {
        ChangeToDist(d);
        View().DoRebuild();
        }
      }
    if (EI.FieldId>=Id_DistOff && EI.FieldId<Id_DistOff+MaxDistributions)
      {
      int d=EI.FieldId-Id_DistOff;
      if (rSD.Distributions[d])
        {//NOT ALLOWED!!
        //FreeDist(d);
        //View().DoRebuild();
        }
      }
    }
  int d=DistFromPgNo(EI.PageNo-iPg1);
  #if !MoveDistList
  if (d<0)
    {
    if (CurrentBlk(EI))
      {//header 2
      /*if (EI.FieldId>=Id_DistOn && EI.FieldId<Id_DistOn+MaxDistributions)
        {
        int d=EI.FieldId-Id_DistOn;
        if (!rSD.Distributions[d])
          {
          AllocDist(d);
          View().DoRebuild();
          }
        }
      if (EI.FieldId>=Id_DistOff && EI.FieldId<Id_DistOff+MaxDistributions)
        {
        int d=EI.FieldId-Id_DistOff;
        if (rSD.Distributions[d])
          {
          FreeDist(d);
          View().DoRebuild();
          }
        }*/
      }
    }
  else
  #endif
    {
    SetWorkDist(d);
    if (CurrentBlk(EI))
      {//header 2
      int p=EI.PageNo;
      int i=(int)(EI.BlkRowNo-EI.Index);
      switch (EI.FieldId)
        {
        case Id_DispRetained:
          SetDispRetained(!DispRetained());
          View().DoRebuild();
          Fix=1;
          break;
        case Id_AutoScale:
          SetAutoScale(!AutoScale());
          SetRanges();
          View().DoRebuild();
          Fix=1;
          break;
        case Id_GrOn    :  
          iGraphOn=!iGraphOn; 
          View().DoRebuild();
          Fix=1;
          break;
        case Id_PartStepped:
          fCrvStepped = !fCrvStepped;
          SetRanges();
          View().DoRebuild();
          Fix=1;
          break;
        case Id_XLog:
          SetXLog(!XLog());
          SetRanges();
          View().DoRebuild();
          Fix=1;
          break;
        }
      }

    // Measurements 
    if (0)//CurrentBlk(EI))
      {
      }

    if (CurrentBlk(EI))
      {//data
      int p=EI.PageNo;
      int i=(int)(EI.BlkRowNo-EI.Index);
#ifdef USE_PSD_TB
      if (EI.FieldId>=Id_CopyTest && EI.FieldId<Id_CopyTest+MaxColumns)
        {

        // Button Pressed to generated data from test PSD data
        int offset = EI.FieldId - Id_CopyTest;

        int count = 0;
        for (int i=0; i<NColumns(); i++)
          {

          if (Columns(i).Editable()&&(Columns(i).iSpId>=0))
            {
            if ( i == offset )
              {

              //
              // Set Button was pressed for this column
              // so xfer the data from the PSD tab page
              // 

              // Get the Size Intervals
              CSD_Intervals    Ints = D().rIntervals;

              // Number of Size Intervals
              int NInts = D().rIntervals.GetSize();


              double ypassi = 0.0;   // Fraction Passing at Size Interval i
              double ypassim1 = 0.0; // Fraction Passing at Size Interval i-1
              double yret = 0.0;     // Fraction Retained at Size Inreval i-1
              double x = 0.0;        // Size Interval size


              for ( int lint = 0 ; lint < NInts ; lint++ )
                {
                x = D().rIntervals[lint];

                // Calculate passing value based on test data entered on
                // PSD tab page for this size
                //ypassi = rSD.m_PSDTB->m_PSDTestData[count].PercentPassingRR(x);
                ypassi = rSD.m_PSDTB->m_PSDTestData[count].PercentPassing(x);

                // Calculate the equivalent retained data
                yret = ypassi - ypassim1;
                ypassim1 = ypassi;

                // Set the columns retained data
                CSD_Column &lCol = Columns(i);
                SetYData( lCol , lint , max(0.0,yret) ); 

                }

              }
            count++;
            }
          }

          View().DoRebuild();
        }

      if (EI.FieldId==Id_TestDataChk)
        {
        rSD.bAllowTestData=!rSD.bAllowTestData; 
        //View().DoRebuild();
        Fix=1;
        }
#endif

      if (EI.FieldId>=Id_YGrfOn && EI.FieldId<Id_YGrfOn+NColumns())
        {
        CSD_Column &C=Columns(EI.FieldId-Id_YGrfOn);
        C.SetGrfOn(!C.GrfOn());
        //View().DoReload();
        View().DoRebuild();
        Fix=1;
        }
      }

    const flag MultOre = (NColumns() && !Columns(0).SimpleNames());
    if (MultOre)
    if (CurrentBlk(EI))
      {//data cum
      int p=EI.PageNo;
      int i=(int)(EI.BlkRowNo-EI.Index);
      if (EI.FieldId>=Id_YGrfOn && EI.FieldId<Id_YGrfOn+NColumns())
        {
        CSD_Column &C=Columns(EI.FieldId-Id_YGrfOn);
        C.SetGrfOn(!C.GrfOn());
        //View().DoReload();
        View().DoRebuild();
        Fix=1;
        }
      }

    // Base Blk
    if (CurrentBlk(EI))
      {
      int p=EI.PageNo;
      int i=(int)(EI.BlkRowNo-EI.Index);
      switch (EI.FieldId)
        {
        case Id_SzAscend:  
          fSzAscend=!fSzAscend; 
          View().DoRebuild();
          Fix=1;
          break;
        case Id_GrOn    :  
          iGraphOn=!iGraphOn; 
          View().DoRebuild();
          Fix=1;
          break;
        }
      }

    }

  return Fix;
  }

//---------------------------------------------------------------------------

void SQSzDist1Edt::SetRanges()
  {
  const int len = Length();
  const flag IsXLog = XLog();
  const flag IsYLog = YLog();
  if (AutoScale())
    {
    if (len==0)
      {
      SetXDispMin(0.01);
      SetXDispMax(1.0);
      for (int c=0; c<NColumns(); c++)
        {
        SetYDispMin(c, 0.01);
//        SetYCDispMax(c, 1.0);
        SetYFDispMax(c, 1.0);
        }
      }
    else
      {
      SetXDispMin(Max(D().rIntervals.ScanMin(), DD().dBottomSizeDisp));
      SetXDispMax(Min(D().rIntervals.ScanMax(), DD().dTopSizeDisp));
      if (IsXLog)
        {
        SetXDispMin(XDispMin()*0.5);
        SetXDispMax(XDispMax()*1.1);
        }
      else
        {
        double xBorder = (XDispMax()-XDispMin())/40.0;
        SetXDispMin(XDispMin()-xBorder);
        SetXDispMax(XDispMax()+xBorder);
        }
      }
    }
  SetXDispMax(Max(XDispMax(), XDispMin()+1.0e-6));
  if (IsXLog)
    {//set ranges to nearest decade
    SetXDispMin(Max(XDispMin(), MinLogVal));
    SetXDispMin(pow(10.0, floor(Log10(XDispMin()))));
    SetXDispMax(pow(10.0, ceil(Log10(XDispMax()))));
    }

  if (AutoScale() && len>0)
    {
    for (int c=0; c<NColumns(); c++)
      if (Columns(c).Reqd(Model()))
        {
        SetYDispMin(c, 0.0);
        SetYFDispMax(c, GetYData(c).ScanMax());
        }
    }
  for (int c=0; c<NColumns(); c++)
    if (Columns(c).Reqd(Model()))
      {
      SetYFDispMax(c, Max(YFDispMax(c), YDispMin(c)+1.0e-6));
      if (IsYLog)
        {
        SetYDispMin(c, Max(YDispMin(c), MinLogVal));
        SetYDispMin(c, pow(10.0, floor(Log10(YDispMin(c)))));
        SetYFDispMax(c, pow(10.0, ceil(Log10(YFDispMax(c)))));
        }
      }
  }

//---------------------------------------------------------------------------

void SQSzDist1Edt::OnDrawBack(rGDIBlk GB,int PgNo, CRgn &ClipRgn)
  {
  ASSERT(pView->CPgNo==PgNo);
  if (GraphOn() && (pView->CPgNo-iPg1>=0) && ValidDistribution() && DistFromPgNo(pView->CPgNo-iPg1)>=0)
    {
    SetWorkDist(DistFromPgNo(pView->CPgNo-iPg1));

    CDC &DC = GB.DC();
    CDCResChk ResChk(DC);

    SetPosition(GB);
    flag DoGraph = DC.RectVisible(&GraphR);

    CGdiObject* OldBrush = (CGdiObject*)DC.SelectObject(GB.pBrushGrfBack);

    if (DoGraph)
      {//draw the curve...
      int Err = ExtSelectClipRgn(DC.m_hDC, (HRGN)ClipRgn.m_hObject, RGN_AND);

      COLORREF OldBkColor = DC.SetBkColor(GB.crGrfBack);
      CPen* OldPen = DC.SelectObject(GB.pPenTxtBord);

      Err = ExtSelectClipRgn(DC.m_hDC, (HRGN)ClipRgn.m_hObject, RGN_AND);
    
      DC.DrawEdge(&GraphR, EDGE_SUNKEN, BF_RECT);
      int EdgeX=GetSystemMetrics(SM_CXEDGE);
      int EdgeY=GetSystemMetrics(SM_CYEDGE);
      GraphR.InflateRect(-EdgeX*2, -EdgeY*2);

      DC.Rectangle(&GraphR);
      MappingSave MapSv;
      PushScaling(GB, GraphR, MapSv);

      CRgn ClipRgnGrf;
      CRgn CombinedRgn;
      ClipRgnGrf.CreateRectRgnIndirect(&GraphR);
      //
      Err = ExtSelectClipRgn(DC.m_hDC, (HRGN)ClipRgnGrf.m_hObject, RGN_AND);

      CPoint Org = DC.GetWindowOrg();
      CSize Ext = DC.GetWindowExt();

  	  const int len = Length();
      const flag IsXLog = XLog();
      const flag IsYLog = YLog();
      //
      SetRanges();
      //
      //get normalised values...
      if (IsXLog)
        {
        XMin = Log10(XDispMin());
        XMax = Log10(XDispMax());
        }
      else
        {
        XMin = XDispMin();
        XMax = XDispMax();
        }
      const double dX = GTZ(XMax-XMin);
      XFactor = XPix/dX;

      YMin.SetSize(NColumns());
      YFMax.SetSize(NColumns());
      YFFactor.SetSize(NColumns());
      for (int c=0; c<NColumns(); c++)
        {
        YMin[c] = YDispMin(c);
        YFMax[c] = YFDispMax(c);
        const double dFY = GTZ(YFMax[c]-YMin[c]);
        YFFactor[c] = YPix/dFY;
        }

      //draw axis...
      ::SelectObject(DC.m_hDC, GB.hLinePenDim[0]);
      int x1,y1;
      if (!IsXLog)
        {
        x1 = (int)Range((double)-INT_MAX, (0.0-XMin)*XFactor, (double)INT_MAX);
        DC.MoveTo(x1,Org.y);
        DC.LineTo(x1,Org.y+Ext.cy);
        }
      if (!IsYLog)
        {
        for (int c=0; c<NColumns(); c++)
          { // Fix This ??? Make Axes Coincide
          y1 = (int)Range((double)-INT_MAX, (0.0-YMin[c])*YFFactor[c], (double)INT_MAX);
          DC.MoveTo(Org.x,y1);
          DC.LineTo(Org.x+Ext.cx,y1);
          }
        }
      //draw X log decades...
      if (IsXLog)
        {
        int Decades = (int)(Log10(XDispMax()) - Log10(XDispMin()));
        double Xv = XDispMin();
        for (int j=0; j<Decades; j++)
          {
          ::SelectObject(DC.m_hDC, GB.hLinePenDim[1]);
          for (int i=1; i<10; i++)
            {
            x1 = (int)Range((double)-INT_MAX, (Log10(Xv*i)-XMin)*XFactor, (double)INT_MAX);
            DC.MoveTo(x1,Org.y);
            DC.LineTo(x1,Org.y+Ext.cy);
            if (i==1)
              ::SelectObject(DC.m_hDC, GB.hLinePenDim[0]);
            if (Decades>4)
              i++;
            }
          Xv *= 10.0;
          }
        }
   
      ::SelectObject(DC.m_hDC, GB.hLinePen[0]);

      CDArray & Xs=GetXData();
      const int dCrossX = XPix/150;
      const int dCrossY = YPix/150;

      const int FirstPen=3;
      if (0 && (len>0))
        {
        //draw crosses at each point...
        int iPen=FirstPen;
        for (int c=0; c<NColumns(); c++)
          if (Columns(c).Reqd(Model()))
            {
            ::SelectObject(DC.m_hDC, GB.hLinePen[iPen++]);
            double YSum=0.0;
            for (int j=0; j<len; j++)
              {
              CDArray &Ys=GetYData(Columns(c));
              x1 = IsXLog ? CalcXLogPix(Xs[j]) : CalcXPix(Xs[j]);
              y1 = IsYLog ? CalcYFLogPix(c, Ys[j]) : CalcYFPix(c, Ys[j]);
              DC.MoveTo(x1-dCrossX,y1-dCrossY);
              DC.LineTo(x1+dCrossX,y1+dCrossY);
              DC.MoveTo(x1-dCrossX,y1+dCrossY);
              DC.LineTo(x1+dCrossX,y1-dCrossY);
              YSum+=Ys[j];
              }
            }
        }
    
      //draw the curves...
      int iPen=FirstPen;
      int XYLen=Xs.GetSize();
      for (c=0; c<NColumns(); c++)
        if (Columns(c).Reqd(Model()))
          {
          if (Columns(c).GrfOn())
            {
            ::SelectObject(DC.m_hDC, GB.hLinePen[iPen]);
            CDArray &Ys=GetYData(Columns(c));
            POINT XY[CrvPts];
            XYLen=0;
            for (int j=0; j<Xs.GetSize()-1; j++)
              {
              const double xval = Xs[j];
              if (XYLen<CrvPts-2)//too many points?!?
                {
              if (xval>DD().dBottomSizeDisp && xval<DD().dTopSizeDisp)
                {//only add points in display range
                int x=IsXLog ? CalcXLogPix(xval) : CalcXPix(xval);
                int y=IsYLog ? CalcYFLogPix(c, Ys[j]) : CalcYFPix(c, Ys[j]);
                if (fCrvStepped)
                  {
                  if (XYLen>0)
                    XY[XYLen].x = XY[XYLen-1].x;
                  else
                    XY[XYLen].x = IsXLog ? CalcXLogPix(DD().dBottomSizeDisp) : CalcXPix(DD().dBottomSizeDisp);//-XPix;
                  XY[XYLen].y = y;//XY[XYLen-1].y;
                  XYLen++;
                  }
                XY[XYLen].x = x;
                XY[XYLen].y = y;
                XYLen++;
                }
              }
              }
            XY[XYLen].x = IsXLog ? CalcXLogPix(DD().dTopSizeDisp) : CalcXPix(DD().dTopSizeDisp);
            XY[XYLen].y = XY[XYLen-1].y;
            XYLen++;

            DC.Polyline(XY, XYLen);
            }
          iPen++;
          }
    
      // Restore State
      PopScaling(GB, MapSv);
      DC.SelectObject(OldPen);
      DC.SetBkColor(OldBkColor);
      DC.SelectClipRgn(NULL);
      }
    DC.SelectObject(OldBrush);
    }
  }

//---------------------------------------------------------------------------

void SQSzDist1Edt::SetPosition(rGDIBlk GB)
  {
  int CW = GB.ColWdt();
  int RH = GB.RowHgt();
  GraphR = GB.LuDataRect();
  GraphR.top -= 1;
  GraphR.right = GraphR.left+GraphWidth()*CW;
  GraphR.bottom = GraphR.top+GraphHeight()*RH;

  GraphR.top+=View().CPg->FixedRows*RH;
  GraphR.bottom+=View().CPg->FixedRows*RH;

  int EdgeX=GetSystemMetrics(SM_CXEDGE);
  int EdgeY=GetSystemMetrics(SM_CYEDGE);
  GraphR.InflateRect(-EdgeX*2, -EdgeY*2);
  
  //iNameWidth = Max(rSD.XNameLen(), rSD.YNameLen());
  pLabels->SetPosition(GraphR.left, GraphR.bottom+1, iNameWidth + 18);
  
  }

//---------------------------------------------------------------------------

void SQSzDist1Edt::PointtoLP(POINT &Pt)
  {
  CDC* pDC = &(View().ScrGB.DC());
  CDCResChk ResChk(pDC);

  MappingSave MapSv;
  PushScaling(View().ScrGB, GraphR, MapSv);
  pDC->DPtoLP(&Pt);
  PopScaling(View().ScrGB, MapSv);
  }

//---------------------------------------------------------------------------

void SQSzDist1Edt::PointtoWP(POINT Pt, double& x, double& y)
  {
  rGDIBlk rGB = View().ScrGB;
  CDC& rDC = rGB.DC();
  CDCResChk ResChk(rDC);
  MappingSave MapSv;
  PushScaling(View().ScrGB, GraphR, MapSv);
  rDC.DPtoLP(&Pt);
  PopScaling(View().ScrGB, MapSv);
  
  if (XLog())
    x = CalcXLogVal(Pt.x);
  else
    x = CalcXVal(Pt.x);
  }

//---------------------------------------------------------------------------

int SQSzDist1Edt::ClosestPt(CPoint point)
  {
  double x,y;
  PointtoWP(point, x, y);
  int PtNo = -1;
  double dist = DBL_MAX;
  return PtNo;
  }

//---------------------------------------------------------------------------

void SQSzDist1Edt::ToggleDigCursor(POINT &Pt)
  {
  CClientDC dc(&Wnd(pView));
  CDCResChk ResChk(dc);

  MappingSave MapSv;
  PushScaling(View().ScrGB, GraphR, MapSv);
  CPen APen(PS_SOLID,1,RGB(0xFF, 0xFF, 0xFF));//dc.GetNearestColor(Doc().DigColour));
  CGdiObject* OldPen=(CGdiObject*)dc.SelectObject(&APen);
  int OldMode=dc.SetROP2(R2_XORPEN);
  dc.MoveTo(Pt.x,0);
  dc.LineTo(Pt.x,YPix);
  dc.MoveTo(0, Pt.y);
  dc.LineTo(XPix, Pt.y);
  dc.SetROP2(OldMode);
  PopScaling(View().ScrGB, MapSv);
  }

//---------------------------------------------------------------------------

flag SQSzDist1Edt::DoLButtonDown(UINT nFlags, CPoint point)
  {
  if (pView->CPgNo-iPg1>=0 && ValidDistribution() && PtInRect(&GraphR, point))
    {
    if (nFlags & MK_SHIFT)
      iDragPtNo = ClosestPt(point);
    return 1;
    }
  return 0;
  }

//---------------------------------------------------------------------------

flag SQSzDist1Edt::DoLButtonUp(UINT nFlags, CPoint point)
  {
  if (pView->CPgNo-iPg1>=0 && ValidDistribution() && PtInRect(&GraphR, point))
    {
    if (iDragPtNo>=0)
      {
      View().DoReload();
      iDragPtNo = -1;
      }
    return 1;
    }
  return 0;
  }

//---------------------------------------------------------------------------

flag SQSzDist1Edt::DoLButtonDblClk(UINT nFlags, CPoint point)
  {
  if (pView->CPgNo-iPg1>=0 && PtInRect(&GraphR, point))
    {
    return 1;
    }
  return 0;
  }

//---------------------------------------------------------------------------

flag SQSzDist1Edt::DoRButtonDown(UINT nFlags, CPoint point)
  {
  if (pView->CPgNo-iPg1>=0 && PtInRect(&GraphR, point))
    {
    return 1;
    }
  return 0;
  }

//---------------------------------------------------------------------------

flag SQSzDist1Edt::DoRButtonUp(UINT nFlags, CPoint point)
  {
  if (pView->CPgNo-iPg1>=0 && ValidDistribution() && PtInRect(&GraphR, point))
    {
    return 1;
    }
  FxdEdtView &Vw=View();
  flag ret=false;//FxdEdtView::DoRButtonUp(nFlags, point);
  if (Vw.CPgNo>=0)
    {
    FxdEdtInfo EI;
    if (Vw.LocateFromCR((int)Vw.ChEditPos.x, (int)Vw.ChEditPos.y, EI))
      {
      ret=true;
      //if (EI.FieldId<0 && EI.Fld)
      if (EI.Fld)
        {
        int nCols=(ValidDistribution() ? NColumns() : 0);
        if (EI.FieldId>=0)
/*        if (EI.FieldId==Id_XInt ||
            EI.FieldId==Id_XIntRng ||
            EI.FieldId==Id_XMin ||
            EI.FieldId==Id_XMax ||
            EI.FieldId==Id_TopSize ||
            EI.FieldId==Id_BotSize ||
            EI.FieldId>=Id_dSize && EI.FieldId<Id_dSize+MaxCSDMeasurements*MaxColumns||
            EI.FieldId>=Id_YMin && EI.FieldId<Id_YMin+nCols ||
//            EI.FieldId>=Id_YCMax && EI.FieldId<Id_YCMax+nCols ||
            EI.FieldId>=Id_YFMax && EI.FieldId<Id_YFMax+nCols ||
            EI.FieldId>=Id_YQmTtl && EI.FieldId<Id_YQmTtl+nCols ||
            EI.FieldId>=Id_YData && EI.FieldId<Id_YData+nCols)*/
          {

          CRect WRect;
          Vw.GetWindowRect(&WRect);
          CPoint  RBPoint;
          RBPoint.x = WRect.left+point.x;
          RBPoint.y = WRect.top+point.y;
          if (EI.FieldId==Id_XInt ||
              EI.FieldId==Id_XIntRng ||
              EI.FieldId==Id_XMin ||
              EI.FieldId==Id_XMax)
            {
            WrkIB.Set(EI.Fld->Tag, &SD_Defn.XCnv, &SD_Defn.XFmt);
            }
          else if (EI.FieldId>=Id_dSize && EI.FieldId<Id_dSize+MaxCSDMeasurements*(1+NColumns()))
            {
            int d=DistFromPgNo(EI.PageNo-iPg1);
            int c=(EI.FieldId-Id_dSize) / MaxCSDMeasurements;
            int i=(EI.FieldId-Id_dSize) % MaxCSDMeasurements;
            const long NMeas=SD_Defn.GetDist(d)->Measurements.GetSize();
            if (i<NMeas)
              {
              CSD_Measurement &M=SD_Defn.GetDist(d)->Measurements[i];
              WrkIB.Set(EI.Fld->Tag, &M.Cnv, &M.Fmt);
              }
            else
              return(ret);
            }
          else if (EI.FieldId==Id_TopSize ||
                   EI.FieldId==Id_BotSize)
            {
            WrkIB.Set(EI.Fld->Tag, &SD_Defn.TBCnv, &SD_Defn.TBFmt);
            }
          else if (EI.FieldId>=Id_YQmTtl && EI.FieldId<Id_YQmTtl+nCols)
            {
            WrkIB.Set(EI.Fld->Tag, &SD_Defn.YQmCnv, &SD_Defn.YQmFmt);
            }
#ifdef USE_PSD_TB
          else if (EI.FieldId >= Id_TestDataChk && EI.FieldId <= Id_Last)
            {
            int c=(EI.FieldId-Id_YMin)%MaxColumns;
            CSD_Column &C=Columns(c);
            WrkIB.Set(EI.Fld->Tag, C.pCnv, C.pFmt);
            }
          else
            {
            // Must be a right click on the PSD data page
            // 
            // Need to get Format to use for PSD Tab Data
            static CnvAttribute lcnv;
            static FmtAttribute fmt;
            CnvAttribute* cnvref = &lcnv;
            cnvref = rSD.m_PSDTB->GetCnv( EI.FieldId );
            if (cnvref)
              WrkIB.Set(EI.Fld->Tag, cnvref , &fmt );
            else
              WrkIB.Set(EI.Fld->Tag, &lcnv , &fmt );              
            /*Need more work here on conversions and formats*/
            //WrkIB.Set(EI.Fld->Tag, &SD_Defn.XCnv, &SD_Defn.XFmt);
#else
          else
            {
            return(ret);
#endif
            }

          CMenu Menu;
          Menu.CreatePopupMenu();
          CMenu FormatMenu;
          FormatMenu.CreatePopupMenu();
          WrkIB.Fmt().AddToMenu(FormatMenu);
          CMenu CnvMenu;
          CnvMenu.CreatePopupMenu();
          WrkIB.Cnv().AddToMenu(CnvMenu);

          Menu.AppendMenu(MF_POPUP, (unsigned int)CnvMenu.m_hMenu, "&Conversions");
          if (WrkIB.Cnv().Index()<=0)// || !IsFloatData(d.iType))
            Menu.EnableMenuItem(0, MF_BYPOSITION|MF_GRAYED);

          Menu.AppendMenu(MF_POPUP, (unsigned int)FormatMenu.m_hMenu, "&Format");

          Menu.AppendMenu(MF_STRING, pView->iSendToTrend, "Send To &Trend");
          Menu.AppendMenu(MF_STRING, pView->iSendToQuickView, "Send To &QuickView");
          Menu.AppendMenu(MF_STRING|MF_GRAYED, pView->iSendToQuickView, "Send To &Custom");

//      rAccNdData d=f();
//      RptTagListsBlk * QB = new QuickViewBlk(ObjClassId(), d.sRefTag(), &d.Cnv, &d.Fmt);
//      ScdMainWnd()->PostMessage(WMU_ADDTOQUICKVIEW, 0, (long)(QB));

          Menu.AppendMenu(MF_SEPARATOR);
          Menu.AppendMenu(MF_STRING, pView->iCmdCopyTag, "Copy Full &Tag");
          if (EI.Fld->Tag==NULL)
            Menu.EnableMenuItem(pView->iCmdCopyTag, MF_BYCOMMAND|MF_GRAYED);
          Menu.AppendMenu(MF_STRING, pView->iCmdCopyVal, "Copy &Value");
          
          Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, RBPoint.x, RBPoint.y, /*(CWnd*)*/&View());
          Menu.DestroyMenu();                                           
          }
        }
      }
    }
  return ret;
  }

//---------------------------------------------------------------------------

flag SQSzDist1Edt::DoRButtonDblClk(UINT nFlags, CPoint point)
  {
  if (pView->CPgNo-iPg1>=0 && PtInRect(&GraphR, point))
    {
    return 1;
    }
  return 0;
  }

//---------------------------------------------------------------------------

flag SQSzDist1Edt::DoMouseMove(UINT nFlags, CPoint point)
  {
  if (GraphOn() && pView->CPgNo-iPg1>=0 && ValidDistribution() && PtInRect(&GraphR, point))
    {
    double x,y;
    PointtoWP(point, x, y);
    char Buff[256], Buff2[256];
    iNameWidth=4;
    SD_Defn.XFmt.FormatFloat(SD_Defn.XCnv.Human(x), Buff2, sizeof(Buff2));
    sprintf(Buff, "%*s:%s", iNameWidth, "Size", Buff2);
    pLabels->SetText(0, Buff);
    pLabels->Show();
    pLabels->Invalidate();
    if (nFlags & MK_LBUTTON)
      {
      if (nFlags & MK_SHIFT)
        {
        if (iDragPtNo>=0)
          {
          }
        }
      else
        iDragPtNo = -1;
      Wnd(pView).InvalidateRect(&GraphR);
      }
    return 1;
    }
  else
    {
    pLabels->Hide();
    }
  return 0;
  }

//---------------------------------------------------------------------------

flag SQSzDist1Edt::DoAccCnv(UINT Id) 
  {
  pCDataCnv pC=Cnvs[WrkIB.Cnv().Index()];
  for (int i=Id; i>0; i--)
    pC=pC->Next();
    
  if (pC)
    {
    WrkIB.Cnv().SetText(pC->Txt());
    }
  View().DoRebuild();
  return true;
  }

//---------------------------------------------------------------------------

flag SQSzDist1Edt::DoAccFmt(UINT Id) 
  {
  for (UINT i=0; i<(UINT)DefinedFmts.GetSize(); i++) 
    if (i==Id) 
      break;

  if (i<(UINT)DefinedFmts.GetSize())
    {
    WrkIB.Fmt()=DefinedFmts[i];
    View().DoRebuild();
    }
  return true;
  }

//---------------------------------------------------------------------------

flag SQSzDist1Edt::DoAccRptTagLists()
  {
  return FxdEdtBookRef::DoAccRptTagLists();
  //TagInfoBlk * IB = new TagInfoBlk(WrkIB.ObjClassId(), WrkIB.RefTag(), 
  //                                 WrkIB.CnvOK() ? &WrkIB.Cnv() : NULL, 
  //                                 WrkIB.FmtOK() ? &WrkIB.Fmt() : NULL);
  //ScdMainWnd()->PostMessage(WMU_ADDTOQUICKVIEW, 0, (long)(IB));
  //
  //return true;
  };

//===========================================================================
//
//
//
//===========================================================================

IMPLEMENT_SPARES(SzPartCrv1, 200);
IMPLEMENT_TAGOBJEDT(SzPartCrv1, "SpPartition", "SzPart", "", "SZP", TOC_ALL|TOC_GRP_SIZEDST|TOC_SIZEDIST, SzPartCrv1Edt, 
                 "Size Partition Curve",
                 "Size Partition Curve");
SzPartCrv1::SzPartCrv1(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach) :
  TaggedObject(pClass_, Tag_, pAttach, eAttach)//,
  {
  CommonConstruct();
  }

// --------------------------------------------------------------------------

SzPartCrv1::SzPartCrv1(pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach) :
  TaggedObject(&SzPartCrv1Class, Tag_, pAttach, eAttach)//,
  {
  CommonConstruct();
  }

// --------------------------------------------------------------------------

void SzPartCrv1::CommonConstruct()
  {
  fPartStepped=true;
  fFracViewActive=true;
  iApplyToDist=-1;
  dBottomSize=1.0e-8;
  dBottomSizeDisp=1.0e-8;
  dTopSize=10.0;
  dTopSizeDisp=10.0;
  SetSizeDefn(0);
  sPCStateStrings[PC_On]="On";
  sPCStateStrings[PC_Off]="Off";
  sPCModeStrings[PC_Frac2Fine]="Fine", 
  sPCModeStrings[PC_Frac2Coarse]="Coarse", 
  fPCMode_Shown=true;
  fPCState_Shown=false;
  fPCGraph_Shown=true;
  //fPCCumOrFrac_Shown=true;
  fPCCumShown=false;
  fPCSizeRangeRows=true;
  iRngWidth=8;
  iCrvCount=0;
  iColSortGroup=1;

  }

// --------------------------------------------------------------------------

SzPartCrv1::~SzPartCrv1()
  {
  }

// --------------------------------------------------------------------------

void SzPartCrv1::SetModeVisibility(flag Shown, char* FineTag, char* CoarseTag)
  {
  fPCMode_Shown=Shown;
  if (FineTag && strlen(FineTag)>0)
    sPCModeStrings[PC_Frac2Fine]=FineTag;
  if (CoarseTag && strlen(CoarseTag)>0)
    sPCModeStrings[PC_Frac2Coarse]=CoarseTag;
  }

// --------------------------------------------------------------------------

void SzPartCrv1::SetStateVisibility(flag Shown, char* OnTag, char* OffTag)
  {
  fPCState_Shown=Shown;
  if (OnTag && strlen(OnTag)>0)
    sPCStateStrings[PC_On]=OnTag;
  if (OffTag && strlen(OffTag)>0)
    sPCStateStrings[PC_Off]=OffTag;
  }

// --------------------------------------------------------------------------

char* SzPartCrv1::GetRangeLbl(int i, Strng& Str, flag Ascend, flag Range)
  {
  if (Range)
    {
    if (Ascend)
      {
      if (i>=SizePts().GetUpperBound())  
        {
        Strng S;
        PCG.XFmt.FormatFloat(PCG.XCnv.Human(SizePts()[i-1]), S);
        Str="+";
        Str+=S;
        }
      else
        {
        Strng S;
        PCG.XFmt.FormatFloat(PCG.XCnv.Human(SizePts()[i]), S);
        Str="-";
        Str+=S;
        }
      //iRngWidth=Min(32, Max(iRngWidth, Str.Len()));
      Str.LPad(iRngWidth);
      }
    else
      {
      if (i<SizePts().GetUpperBound())  
        {
        i=SizePts().GetUpperBound()-i;
        Str="";
        Strng S;
        if (SD_Defn.LabelStyle()==LS_Full)
          {
          Strng S;
          PCG.XFmt.FormatFloat(PCG.XCnv.Human(SizePts()[i]), S);
          Str+="-";
          Str+=S;
          }
        PCG.XFmt.FormatFloat(PCG.XCnv.Human(SizePts()[i-1]), S);
        Str+="+";
        Str+=S;
        }
      else
        {
        Strng S;
        PCG.XFmt.FormatFloat(PCG.XCnv.Human(SizePts()[0]), S);
        Str="-";
        Str+=S;
        if (SD_Defn.LabelStyle()==LS_Full)
          {
          PCG.XFmt.FormatFloat(PCG.XCnv.Human(BottomSize()), S);
          Str+="+";
          Str+=S;
          }
        }
      }
    }
  else
    {
    if (!Ascend)
      i=SizePts().GetUpperBound()-i;
    PCG.XFmt.FormatFloat(PCG.XCnv.Human(SizePts()[i]), Str);
    }
  return Str();
  }

// --------------------------------------------------------------------------

void SzPartCrv1::CalcCumValues()
  {
  if (fPCCumShown)
    {
    if (Crvs.GetSize()<=iCrvCount)
      SetNCurves(iCrvCount);
    for (int c=0; c<NCurves(); c++)
      {
      Crvs[c+NCurves()]=Crvs[c];
      Crvs[c+NCurves()].ToCumulative();
      }
    }
  }

// --------------------------------------------------------------------------

void SzPartCrv1::SetSizeDefn(int iDef)
  {
  iSizeDefn=iDef;//MAX_INT;
  #if !UseManualSizeDefn
  if (iSizeDefn<0)
    iSizeDefn=0;
  #endif

  int L=Length();
  if (iSizeDefn>=0 && iSizeDefn<SD_Defn.NDistributions())
    {
    CSD_DistDefn &D=*SD_Defn.GetDist(iSizeDefn);
    dBottomSize=D.BottomSize();
    dTopSize=D.TopSize();
    dBottomSizeDisp=D.BottomSizeDisp();
    dTopSizeDisp=D.TopSizeDisp();

    LclSz.SetSize(0);
    CSD_Intervals &Intervals=D.Intervals; //use intervals from specified distribution
    const int icnt = Intervals.GetSize();
    for (int i=0; i<icnt; i++)
      if ((Intervals[i]>=dBottomSize) && (Intervals[i]<=dTopSize))
        {
        const double d = Intervals[i];
        LclSz.Add(d);
        }
    if ((icnt==0) || (Intervals[icnt-1]<dTopSize))
      LclSz.Add(dTopSize); // Oversize Interval
    //double OvrInt=dTopSizeDisp;//BigSizeInterval;
    //LclSz.Add(OvrInt); // Oversize Interval
    }

  //todo: if changing from one iSizeDefn to another the perhaps data should be changed intelligently!?
  SetLength(Length());
  }

// --------------------------------------------------------------------------

void SzPartCrv1::SetLength(int N)  
  { 
  if (!UserSizePts())
    N=SizePts().GetSize(); 
  else
    {
    N=Max(2, N); 
    SizePts().SetSize(N); 
    }
  Crvs.SetSizes(Crvs.GetSize(), N); 
  }

// --------------------------------------------------------------------------

void SzPartCrv1::SetNCurves(int N) 
  { 
  N=Min(N, MaxPartCurves);
  iCrvCount = N;
  const int F=(fPCCumShown ? 2 : 1);
  const int N0=CrvInfo.GetSize()/F;
  CrvInfo.SetSize(N*F);
  Crvs.SetSize(N*F);
  const int nPts = SizePts().GetSize();
  SetLength(nPts);
  for (int i=N0; i<N; i++)
    {
    if (nPts>0)
      Crvs[i][nPts-1] = 1.0;
    CrvInfo[i].SetDefaultValues();
    CrvInfo[i].Title.Set("Frac%d", i);
    }
  if (fPCCumShown)
    {
    for (int i=N; i<N*2; i++)
      {
      CrvInfo[i].SetDefaultValues();
      CrvInfo[i].Title.Set("Cum%d", i-N);
      CrvInfo[i].Edit=false;
      }
    }
  }

// --------------------------------------------------------------------------

void SzPartCrv1::SetCumulativeVisibility(flag Shown)
  {
  int N=NCurves();
  fPCCumShown=Shown;
  SetNCurves(N);
  }

// --------------------------------------------------------------------------

//Does Nothing: 
//void SzPartCrv1::BuildDataDefn_Lcl(DataDefnBlk & DDB)
//  {
//  }


// --------------------------------------------------------------------------

void SzPartCrv1::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginObject(this, Tag(), SzPartCrv1Class.ClassId(), NULL, DDB_OptPage);

  DDBValueLstMem DDB0;
  DDB0.Empty();
  for (int i=0; i<SD_Defn.NIntervals(); i++)
    DDB0.Add(i, SD_Defn.GetIntervals(i)->Name());
#if UseManualSizeDefn
  DDB0.Add(-1, "Manual");
#endif
  
  DDB.Long("SizeDefn",   "",  DC_ ,   "", xidPartSizeDefn,             this, isParm, DDB0());
  DDB.Long("Length",     "",  DC_ ,   "", xidPartLength,               this, isParm);
  DDB.Long("Curves",     "",  DC_ ,   "", xidPartCurves,               this, isParm);

  DDB0.Empty();
  for (i=0; i<SD_Defn.NDistributions(); i++)
    DDB0.Add(i, SD_Defn.GetDist(i)->Name());
  DDB0.Add(-1, "All");
  
#if UseManualSizeDefn
  DDB.Long("ApplyTo",       "", DC_, "", xidApplyToDist,       this, isParm, DDB0());
#endif
      
  static DDBValueLst DDB1[]={
    {0, "Smooth"},
    {1, "Stepped"},
    {0}};
  DDB.Bool("Shape",        "", DC_, "", &fPartStepped,        this, isParm, DDB1);
    
  DDB.Double("XDispMin",   "", DC_, "", &PCG.dSzDispMin,      this, isParm);
  DDB.Double("XDispMax",   "", DC_, "", &PCG.dSzDispMax,      this, isParm);

  DDB.Bool  ("PCState_Shown",   "",  DC_, "", &fPCState_Shown,    this, isParm|InitHidden|noFileAtAll);
  DDB.Bool  ("PCMode_Shown",    "",  DC_, "", &fPCMode_Shown,     this, isParm|InitHidden|noFileAtAll);
  DDB.Bool  ("PCGraph_Shown",   "",  DC_, "", &fPCGraph_Shown,    this, isParm|InitHidden|noFileAtAll);
  DDB.Bool  ("PCCum_Shown",     "",  DC_, "", &fPCCumShown,       this, isParm|InitHidden|noFileAtAll);
  DDB.Bool  ("PCSizeRangeRows", "",  DC_, "", &fPCSizeRangeRows,  this, isParm|InitHidden|noFileAtAll);
  
  DDB.String("UsedClassID",     "",  DC_, "", xidUsedClassID,     this, isParm|InitHidden|noFileAtAll);

  DDB.String("PCMode_F",       "",  DC_, "", xidPartCrvMode_F,    this, isParm|InitHidden|noFileAtAll);
  DDB.String("PCMode_C",       "",  DC_, "", xidPartCrvMode_C,    this, isParm|InitHidden|noFileAtAll);
  DDB.String("PCState_On",     "",  DC_, "", xidPartCrvState_On,  this, isParm|InitHidden|noFileAtAll);
  DDB.String("PCState_Off",    "",  DC_, "", xidPartCrvState_Off, this, isParm|InitHidden|noFileAtAll);

  if (fPCCumShown && Crvs.GetSize()<=iCrvCount)
    CalcCumValues();

  Strng Nm;
  for (i=0; i<NCurves(); i++)
    {
    Nm.Set("Mode%i", i);
    static DDBValueLst DDBMode[]= {
      {PC_Frac2Fine,   sPCModeStrings[PC_Frac2Fine]()},
      {PC_Frac2Coarse, sPCModeStrings[PC_Frac2Coarse]()},
      {0}};
    DDB.Byte(Nm(),  "",  DC_, "",  xidPartCrvMode+i, this, isParm, DDBMode);
    Nm.Set("State%i", i);
    static DDBValueLst DDBState[]={
      {PC_On,          sPCStateStrings[PC_On]()},
      {PC_Off,         sPCStateStrings[PC_Off]()},
      {0}};
    DDB.Byte(Nm(), "", DC_, "",  xidPartCrvState+i, this, isParm, DDBState);
    Nm.Set("Edit%i", i);
    DDB.Byte(Nm(), "", DC_, "",  xidPartCrvEdit+i, this, isParm|noFileAtAll);
    Nm.Set("AutoCorrect%i", i);
    DDB.Byte(Nm(), "", DC_, "",  xidPartCrvAutoCorrect+i, this, isParm|noFileAtAll);
    Nm.Set("Title%i", i);
    DDB.String(Nm(), "", DC_, "",  xidPartCrvTitle+i, this, isParm|noFileAtAll);
    Nm.Set("CnvIndex%i", i);
    DDB.Short(Nm(), "", DC_, "",  xidPartCrvCnvIndex+i, this, isParm|noFileAtAll);
    Nm.Set("CnvText%i", i);
    DDB.String(Nm(), "", DC_, "",  xidPartCrvCnvText+i, this, isParm|noFileAtAll);
    Nm.Set("RangeLo%i", i);
    DDB.Double(Nm(), "", DC_, "",  xidPartCrvRangeLo+i, this, isParm|noFileAtAll);
    Nm.Set("RangeHi%i", i);
    DDB.Double(Nm(), "", DC_, "",  xidPartCrvRangeHi+i, this, isParm|noFileAtAll);
    }

  if (DDB.BeginArray(this, "SZPrtCrv", "SZPrtCrv", Length()))
    for (int d=0; d<Length(); d++)
      if (DDB.BeginElement(this, d))
        {
        if (DDB.ForView())
          {
          //DDB.Double("Size",    "", DC_L, "um", &SizePts()[d], this, isResult|noFileAtAll);
          //DDB.Double("SzRange", "", DC_L, "um", &SizePts()[d], this, isResult|noFileAtAll);
          }
        else
          {
          DDB.Double("Size",    "", DC_L, "um", &SizePts()[d], this, iSizeDefn < 0 ? isParm : DDEF_WRITEPROTECT);
          DDB.Double("SzRange", "", DC_L, "um", &SizePts()[d], this, DDEF_WRITEPROTECT|noFileAtAll);
          }
        for (int c=0; c<NCurves(); c++)
          {
          //DDB.Double(CrvInfo[c].Title(),  "",  DC_Frac, "%",  &Crvs[c][d], this, isParm);
          DDB.Double(CrvInfo[c].Title(),  "",  CrvInfo[c].DataCnv.Index(), CrvInfo[c].DataCnv.Text(), &Crvs[c][d], this, isParm);
          //dbgpln("B:%s %3i %3i = %12.6f ", DDB.DoingGetData()?"Get":"Put", c,d, Crvs[c][d]);
          }
        if (fPCCumShown)
          {
          for (c=NCurves(); c<NCurves()*2; c++)
            DDB.Double(CrvInfo[c].Title(),  "",  CrvInfo[c].DataCnv.Index(), CrvInfo[c].DataCnv.Text(), &Crvs[c][d], this, noFileAtAll);//|DDEF_WRITEPROTECT);//isParm);
          }
        }
  DDB.EndArray();
  DDB.EndObject();
  }

// --------------------------------------------------------------------------

flag SzPartCrv1::DataXchg(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
    case xidPartSizeDefn: 
      if (DCB.rL)
        //SetSizeDefn(Range(-1L, *DCB.rL, SD_Defn.NIntervals()-1L));
        SetSizeDefn(Range(-1L, *DCB.rL, SD_Defn.NDistributions()-1L));
      DCB.L=iSizeDefn;
      return 1;
#if UseManualSizeDefn
    case xidApplyToDist:
      if (DCB.rL)
        {
        iApplyToDist=Range(-1L, *DCB.rL, SD_Defn.NDistributions()-1L);
        SetSizeDefn(iSizeDefn);
        }
      DCB.L=iApplyToDist;
      return 1;
#endif
    case xidPartLength: 
      if (DCB.rL)
        SetLength(*DCB.rL);
      DCB.L=Length();
      return 1;
    case xidPartCurves:
      // AttachedTo must set the Number of curves
      if (DCB.rL && (pAttachedTo==NULL))
        SetNCurves(*DCB.rL);
      DCB.L=NCurves();
      return 1;
    case xidUsedClassID:
      if (DCB.rpC && strlen(DCB.rpC)>0)
        sUsedClassID=DCB.rpC;
      DCB.pC=(pAttachedTo ? pAttachedTo->ClassId() : sUsedClassID());
      return 1;
    case xidPartCrvMode_F:
      if (DCB.rpC)
        sPCModeStrings[PC_Frac2Fine]=DCB.rpC;
      DCB.pC=sPCModeStrings[PC_Frac2Fine]();
      return 1;
    case xidPartCrvMode_C:
      if (DCB.rpC)
        sPCModeStrings[PC_Frac2Coarse]=DCB.rpC;
      DCB.pC=sPCModeStrings[PC_Frac2Coarse]();
      return 1;
    case xidPartCrvState_On:
      if (DCB.rpC)
        sPCStateStrings[PC_On]=DCB.rpC;
      DCB.pC=sPCStateStrings[PC_On]();
      return 1;
    case xidPartCrvState_Off:
      if (DCB.rpC)
        sPCStateStrings[PC_Off]=DCB.rpC;
      DCB.pC=sPCStateStrings[PC_Off]();
      return 1;
    default:
      if (DCB.lHandle>=xidPartCrvMode && DCB.lHandle<xidPartCrvMode+MaxColumns)
        {
        if (DCB.rB)
          SetCurveMode(DCB.lHandle-xidPartCrvMode, *DCB.rB);
        DCB.B=CurveMode(DCB.lHandle-xidPartCrvMode);
        return 1;
        }
      else if (DCB.lHandle>=xidPartCrvState && DCB.lHandle<xidPartCrvState+MaxColumns)
        {
        if (DCB.rB)
          SetCurveState(DCB.lHandle-xidPartCrvState, *DCB.rB);
        DCB.B=CurveState(DCB.lHandle-xidPartCrvState);
        return 1;
        }
      else if (DCB.lHandle>=xidPartCrvEdit && DCB.lHandle<xidPartCrvEdit+MaxColumns)
        {
        if (DCB.rB)
          SetEditable(DCB.lHandle-xidPartCrvEdit, *DCB.rB);
        DCB.B=Editable(DCB.lHandle-xidPartCrvEdit);
        return 1;
        }
      else if (DCB.lHandle>=xidPartCrvAutoCorrect && DCB.lHandle<xidPartCrvAutoCorrect+MaxColumns)
        {
        if (DCB.rB)
          CrvInfo[DCB.lHandle-xidPartCrvAutoCorrect].AutoCorrect=*DCB.rB;
        DCB.B=CrvInfo[DCB.lHandle-xidPartCrvAutoCorrect].AutoCorrect;
        return 1;
        }
      else if (DCB.lHandle>=xidPartCrvTitle && DCB.lHandle<xidPartCrvTitle+MaxColumns)
        {
        if (DCB.rpC)
          SetTitle(DCB.lHandle-xidPartCrvTitle, DCB.rpC);
        DCB.pC=Title(DCB.lHandle-xidPartCrvTitle);
        return 1;
        }
      else if (DCB.lHandle>=xidPartCrvCnvIndex && DCB.lHandle<xidPartCrvCnvIndex+MaxColumns)
        {
        if (DCB.rS)
          CrvInfo[DCB.lHandle-xidPartCrvCnvIndex].DataCnv.SetIndex(*DCB.rS);
        DCB.S=CrvInfo[DCB.lHandle-xidPartCrvCnvIndex].DataCnv.Index();
        return 1;
        }
      else if (DCB.lHandle>=xidPartCrvCnvText && DCB.lHandle<xidPartCrvCnvText+MaxColumns)
        {
        if (DCB.rpC)
          CrvInfo[DCB.lHandle-xidPartCrvCnvText].DataCnv.SetText(DCB.rpC);
        DCB.pC=CrvInfo[DCB.lHandle-xidPartCrvCnvText].DataCnv.Text();
        return 1;
        }
      else if (DCB.lHandle>=xidPartCrvRangeLo && DCB.lHandle<xidPartCrvRangeLo+MaxColumns)
        {
        if (DCB.rD)
          CrvInfo[DCB.lHandle-xidPartCrvRangeLo].dLoRange=*DCB.rD;
        DCB.D=CrvInfo[DCB.lHandle-xidPartCrvRangeLo].dLoRange;
        return 1;
        }
      else if (DCB.lHandle>=xidPartCrvRangeHi && DCB.lHandle<xidPartCrvRangeHi+MaxColumns)
        {
        if (DCB.rD)
          CrvInfo[DCB.lHandle-xidPartCrvRangeHi].dHiRange=*DCB.rD;
        DCB.D=CrvInfo[DCB.lHandle-xidPartCrvRangeHi].dHiRange;
        return 1;
        }

    }
  return TaggedObject::DataXchg(DCB);
  }

// --------------------------------------------------------------------------

flag SzPartCrv1::ValidateData(ValidateDataBlk & VDB)
  {
  for (int c=0; c<Crvs.GetSize(); c++)
    for (int j=0; j<Crvs[c].GetSize(); j++)
      {
      Crvs[c][j] = Range(CrvInfo[c].dLoRange, Crvs[c][j], CrvInfo[c].dHiRange);
      //dbgpln("A:%3i %3i = %12.6f %12.6f %12.6f ", c,j, CrvInfo[c].dLoRange, Crvs[c][j], CrvInfo[c].dHiRange);
      }
  for (c=0; c<NCurves(); c++)
    {
//    if (CrvInfo[c].AutoCorrect && !TaggedObject::GetXWritesBusy())
    if (CrvInfo[c].AutoCorrect)
      {
      double Err=0.0;
      if (Crvs[c].AdjustCumulativeEnd(CrvInfo[c].dHiRange, CrvInfo[c].dLoRange, CrvInfo[c].dHiRange, Err))
        if (pAttachedTo)
          LogWarning(pAttachedTo->Tag(), 0, "Sizes adjusted to ensure correct sum (%s.%s column %d) Error=%g", pAttachedTo->Tag(), Tag(), c, Err);
        else
          LogWarning("Tag?", 0, "Sizes adjusted to ensure correct sum (%s.%s column %d) Error=%g", "Tag?", Tag(), c, Err);
      }
    }
  if (fPCCumShown)
    CalcCumValues();
  return true;
  }

// --------------------------------------------------------------------------

double SzPartCrv1::SizePassingFraction(int CurveNo, double dFrac)
  {
  return dNAN;
  }

// --------------------------------------------------------------------------

double SzPartCrv1::FractionPassingSize(int CurveNo, double dSize)
  {
  double Frac=0.0;
  const long len = Length();
  switch (len)
    {
    case 0 : Frac=0.0; break;
    case 1 : Frac=Crvs[CurveNo][0]; break;
    default :
      {
      CDArray & X = SizePts();
      CDArray & Y = Crvs[CurveNo];
      for (long i=0; (i<len-2) && (dSize>=X[i+1]); i++) 
        { };
        //if (fabs(X[i+1] - X[i]) > 1.0e-12)
        //  break;
      int xx=0;
      Frac=Y[i]+(Y[i+1] - Y[i]) * (dSize - X[i]) / GTZ(X[i+1] - X[i]);
      break;
      }
    }

  return Range(0.0, Frac, 1.0);
  }

//===========================================================================
//
//
//
//===========================================================================

const int SzPartCrv1Edt::XPix=10000;
const int SzPartCrv1Edt::YPix=10000;
const int SzPartCrv1Edt::CrvPts=(MaxIntervals+2)*2;
int SzPartCrv1Edt::iGraphWidth=40;
int SzPartCrv1Edt::iGraphHeight=15;
int SzPartCrv1Edt::iGraphOn=1;

//------------------------------------------------------------------------------

SzPartCrv1Edt::SzPartCrv1Edt(pFxdEdtView pView_, pSzPartCrv1 pSD_) :
  FxdEdtBookRef(pView_),
  PC(*pSD_),
  PCG(pSD_->PCG)
  {
  iDragPtNo = -1;
 
  XMin=0.0;
  XMax=1.0;
  XFactor=1.0;
  iNameWidth=4;
  iWorkDist=0;
  iPg1=0;

//  fXtraPg=0;
  pLabels = new CLabelWnd;
  pLabels->CreateIt(pView_);
  pLabels->SetFont(pView_->ScrGB.pFont[0]);
  pLabels->SetLineCnt(1/*2*/);

  //pWrkCnv=NULL;
  //pWrkFmt=NULL;

  Strng Section;
  Section.Set("%s_%s", PC.Tag(), PC.sUsedClassID()/*PC.pAttachedTo->ClassId()*/);

  ObjectAttribute *pAttr=ObjAttributes.FindObject(Section());
  if (pAttr)
    {
    pAttr->FieldFmtCnvs("Size", PCG.XFmt, PCG.XCnv);
    pAttr->FieldFmtCnvs("Frac", PCG.YFmt, PCG.YCnv);
    }

  CProfINIFile PF(PrjIniFile());

  SzPartCrv1Edt::iGraphWidth=PF.RdInt(Section(), "GraphWidth", 40);
  SzPartCrv1Edt::iGraphHeight=PF.RdInt(Section(), "GraphHeight", 15);
  SzPartCrv1Edt::iGraphOn=PF.RdInt(Section(), "GraphOn", true);

  fSzAscend=PF.RdInt("SzDistribution", "IntervalsAscending", fSzAscend);
  }

//---------------------------------------------------------------------------

SzPartCrv1Edt::~SzPartCrv1Edt()
  {
  CProfINIFile PF(PrjIniFile());
  Strng Section;
  Section.Set("%s_%s", PC.Tag(), PC.sUsedClassID()/*PC.pAttachedTo->ClassId()*/);

  PF.WrInt(Section(), "GraphWidth",  SzPartCrv1Edt::iGraphWidth);
  PF.WrInt(Section(), "GraphHeight", SzPartCrv1Edt::iGraphHeight);
  PF.WrInt(Section(), "GraphOn",     SzPartCrv1Edt::iGraphOn);

  PF.WrInt("SzDistribution", "IntervalsAscending", fSzAscend);

  ObjectAttribute *pAttr=ObjAttributes.FindObject(Section());
  if (pAttr)
    {
    pAttr->SetFieldFmt("Size",  PCG.XFmt);
    pAttr->SetFieldCnvs("Size", PCG.XCnv);
    pAttr->SetFieldFmt("Frac",  PCG.YFmt);
    pAttr->SetFieldCnvs("Frac", PCG.YCnv);
    }
  
  pLabels->DestroyWindow();
  
  //cnmdelete pLabels;
  }

//---------------------------------------------------------------------------

//int SzPartCrv1Edt::DistFromPgNo(int Pg)
//  {
//  return 0;
////  for (int d=0; d<NCurves(); d++)
////    if (rSD.Distributions[d])
////      if (Pg--==0) return d;
//  //DoBreak();
//  return -1;
//  }

//---------------------------------------------------------------------------

void SzPartCrv1Edt::PutDataStart()
  {
  }

//---------------------------------------------------------------------------

void SzPartCrv1Edt::PutDataDone()
  {
  }

//---------------------------------------------------------------------------

void SzPartCrv1Edt::GetDataStart()
  {
  }

//---------------------------------------------------------------------------

void SzPartCrv1Edt::GetDataDone()
  {
  }

//---------------------------------------------------------------------------

void SzPartCrv1Edt::StartBuild()
  {
  }

//---------------------------------------------------------------------------

void SzPartCrv1Edt::Build()
  {
  Strng Tg, S, St;

  /*PCG.sNames.SetSize(NCurves());
  for (int i=0; i<NCurves(); i++)
    PCG.sNames[i].Set("Frac%i", i);*/
  if (PC.fPCCumShown)// && PC.Crvs.GetSize()<=PC.iCrvCount)
    PC.CalcCumValues();

  iPg1=pView->Pages;
  StartPage(PC.Tag());
  if (1) // Head Blk
    {
#if UseManualSizeDefn
    int HedLen=(SD_Defn.NDistributions()>1 ? 3 : 2);//PC.UserSizePts() ? 4 : 3;
#else
    int HedLen=0;
#endif
    HedLen+=(GraphOn() ? 2 : 0);
    StartBlk(HedLen, 0, NULL);
    int L=0;
    if (GraphOn())
      {
      SetSpace(L, 1);
      SetParm(L, "", Id_XMin, 10, 2, "");
      SetSpace(L, Max(2,GraphWidth()-20-2));
      SetParm(L, "", Id_XMax, 10, 2, "");
      SetSpace(L, 1);
      SetDesc(L, "Graph:", -1, 6, 2, " ");
      Strng Tg(GraphOn() ? " On > Off " : " Off > On ");
      SetButton(L, Tg(),  Id_GrOn, 10, 0, "");

      L++;
      //SetSpace(L, 1);
      if (XLog())
        SetButton(L,"Log > Lin", Id_XLog, 10, 0, "");
      else
        SetButton(L,"Lin > Log", Id_XLog, 10, 0, "");
      SetSpace(L, 1);
      if (AutoScale())
        SetButton(L,"Auto > Fixed Scale", Id_AutoScale, 20, 0, "");
      else
        SetButton(L,"Fixed > Auto Scale", Id_AutoScale, 20, 0, "");
      SetSpace(L, 1);
      //SetDesc(L, "Type",  -1, 5,  0, " ");
      if (PartStepped())
        SetButton(L, "Stepped > Smooth", Id_PartStepped, 18, 0, " ");
      else
        SetButton(L, "Smooth > Stepped", Id_PartStepped, 18, 0, " ");
      L++;
      }

#if UseManualSizeDefn
    L++;
    int FLen=8;
    for (int i=0; i<SD_Defn.Intervals.GetSize(); i++)
      FLen=Max(FLen, SD_Defn.Intervals[i]->NameLength());
    SetDParm(L, "SizeDefn", 10, "", Id_PartSizeDefn, FLen, 0, "");
    for (i=0; i<SD_Defn.Intervals.GetSize(); i++)
      FldHasFixedStrValue(i, SD_Defn.Intervals[i]->Name());
    FldHasFixedStrValue(-1, "Manual");
    SetSpace(L, 5);
    SetDParm(L, "Length", 8, "", Id_PartLen, 3, 0, "");

    L++;
    if (SD_Defn.NDistributions()>1)
      {
      FLen=8;
      for (i=0; i<SD_Defn.NDistributions(); i++)
        FLen=Max(FLen, SD_Defn.Distributions[i]->NameLength());
      SetDParm(L, "Apply To", 10, "", Id_ApplyToDist, FLen, 0, "");
      for (i=0; i<SD_Defn.NDistributions(); i++)
        FldHasFixedStrValue(i, SD_Defn.Distributions[i]->Name());
      FldHasFixedStrValue(-1, "All");
      L++;
      }
#endif
    }

  if (1) // Stats Blk
    {
    }
  
  int SzRngLen = 12;
  if (1) // Data Blk
    {
    int HedLen = 2;
    if (PC.fPCMode_Shown)
      HedLen++;
    if (PC.fPCState_Shown)
      HedLen++;
    StartBlk(Length()+HedLen+1, HedLen, NULL);
    int L=0;

    SetDesc(L, " ",  -1, 4,  1, "");
    SetDesc(L, PC.fPCSizeRangeRows ? "Size Range" : "Size", -1, SzRngLen, 1, " ");
    for (int c=0; c<NCurves(); c++)
      {
      S=PC.CrvInfo[c].Title();
      S.SetLength(Min(S.Length(), 10));
      SetDesc(L, S(), -1, 10, 1, "");
      }
    if (PC.fPCCumShown)
      {
      SetSpace(L, 2);
      for (c=NCurves(); c<NCurves()*2; c++)
        {
        S=PC.CrvInfo[c].Title();
        S.SetLength(Min(S.Length(), 10));
        SetDesc(L, S(), -1, 10, 1, "");
        }
      }
    //...
    if (PC.fPCMode_Shown)
      {
      L++;
      SetDesc(L, "Mode",  -1, SzRngLen+4,  2, " ");
      for (c=0; c<NCurves(); c++)
        {
        SetParm(L, "", Id_YMode+c, 8, 0, "  ");
        for (int i=0; i<MaxPCModes; i++)
          FldHasFixedStrValue(i, PC.sPCModeStrings[i]());
        Tg.Set("%s.Mode%i", FullObjTag(), c);
        SetTag(Tg());
        }
      }
    //...
    if (PC.fPCState_Shown)
      {
      L++;
      SetDesc(L, "State",  -1, SzRngLen+4,  2, " ");
      for (c=0; c<NCurves(); c++)
        {
        SetParm(L, "", Id_YState+c, 8, 0, "  ");
        for (int i=0; i<MaxPCStates; i++)
          FldHasFixedStrValue(i, PC.sPCStateStrings[i]());
        Tg.Set("%s.State%i", FullObjTag(), c);
        SetTag(Tg());
        }
      }
      
    //...
    L++;
    SetDesc(L, PCG.XCnv.Text(),  -1, SzRngLen+4,  2, " ");
    for (c=0; c<NCurves(); c++)
      //SetDesc(L, PCG.YCnv.Text(),  -1, 10,  2, "");
      SetDesc(L, PC.CrvInfo[c].DataCnv.Text(),  -1, 10,  2, "");
    if (PC.fPCCumShown)
      {
      SetSpace(L, 2);
      SetDesc(L, "Size", -1, 10, 1, " ");
      for (c=NCurves(); c<NCurves()*2; c++)
        SetDesc(L, PC.CrvInfo[c].DataCnv.Text(),  -1, 10,  2, "");
      }
    //..
    int iSz=fSzAscend ? 0 : Length()-1;
    int inc=fSzAscend ? 1 : -1;
    for ( ; iSz>=0 && iSz<Length(); iSz+=inc)
//    for (int iSz=0; iSz<Length(); iSz++)
      {
      L++;
      S.Set("%i", iSz);
      St.Set("SZPrtCrv.[%i]", iSz);
      SetDesc(L, S(), -1 , 4, 3, "");
      SetParm(L, "",  PC.fPCSizeRangeRows ? Id_XIntRng : Id_XInt, SzRngLen, 2, " ");
      Tg.Set("%s.%s.%s", FullObjTag(), St(), PC.fPCSizeRangeRows ? "SzRange" : "Size");
      SetTag(Tg(), PCG.XCnv.Text());
      for (c=0; c<NCurves(); c++)
        {
        SetParm(L, "",  Id_YData+c, 10, 2, "");
        Tg.Set("%s.%s.%s", FullObjTag(), St(), PC.CrvInfo[c].Title());
        //SetTag(Tg(), PCG.YCnv.Text());
        SetTag(Tg(), PC.CrvInfo[c].DataCnv.Text());
        }
      if (PC.fPCCumShown)
        {
        SetSpace(L, 2);
      //S.Set("%i", iSz);
        St.Set("SZPrtCrv.[%i]", iSz);
        SetParm(L, "",  Id_XInt, 10, 2, " ");
        Tg.Set("%s.%s.%s", FullObjTag(), St(), "Size");
        SetTag(Tg(), PCG.XCnv.Text());
        for (c=NCurves(); c<NCurves()*2; c++)
          {
          SetParm(L, "", Id_YData+c, 10, 2, "");
          Tg.Set("%s.%s.%s", FullObjTag(), St(), PC.CrvInfo[c].Title());
          //SetTag(Tg(), PCG.YCnv.Text());
          SetTag(Tg(), PC.CrvInfo[c].DataCnv.Text());
          }
        }
      }
    }
  
  if (1) // Base Blk
    {
    StartBlk(PC.fPCGraph_Shown ? 3 : 2, 0, NULL);
    int L=0;
    SetDesc(L, "Size Intervals :",  -1, 16,  2, " ");
    Strng SzTg(fSzAscend ? " > Descending " : " > Ascending ");
    SetButton(L, SzTg(),  Id_SzAscend, 14, 0, "");
    L++;
    if (PC.fPCGraph_Shown)
      {
      SetDesc(L, "Graph:", -1, 6, 2, " ");
      Strng Tg(GraphOn() ? " On > Off " : " Off > On ");
      SetButton(L, Tg(),  Id_GrOn, 10, 0, "");
      if (GraphOn())
        {
        SetSpace(L, 1);
        SetDParm(L, "Width", 6, "", Id_GrWidth, 2, 2, "");
        SetSpace(L, 1);
        SetDParm(L, "Height", 7, "", Id_GrHeight, 2, 2, "");
        }
      L++;
      }
    }
  }

//---------------------------------------------------------------------------

void SzPartCrv1Edt::ChangeLayout(rFxdEdtPage Pg, int TotalRows, int TotalCols)
  {
  if (GraphOn() && (Pg.No-iPg1>=0))
    {
    Pg.MoveRows(-32000);
    Pg.MoveRows(GraphHeight());
    }
  }

//---------------------------------------------------------------------------

void SzPartCrv1Edt::Load(FxdEdtInfo &EI, Strng & Str)
  {
  if (CurrentBlk(EI))
    {//header
    int p=EI.PageNo;
    int i=(int)(EI.BlkRowNo-EI.Index);
    switch (EI.FieldId)
      {
      case Id_XMin :  
        PCG.XFmt.FormatFloat(PCG.XCnv.Human(XDispMin()), Str);
        break;
      case Id_XMax :  
        PCG.XFmt.FormatFloat(PCG.XCnv.Human(XDispMax()), Str);
        break;
      case Id_PartLen:
        Str.Set("%i", Length());
        EI.Fld->fEditable=PC.UserSizePts();
        break;
#if UseManualSizeDefn
      case Id_PartSizeDefn:
        if (!PC.UserSizePts())//iSizeDefn<SD_Defn.Intervals.GetSize())
          Str=SD_Defn.GetIntervals(PC.iSizeDefn)->Name();
        else
          Str="Manual";
        break;
      case Id_ApplyToDist:
        if (PC.iApplyToDist>=0)//iSizeDefn<SD_Defn.Intervals.GetSize())
          Str=SD_Defn.GetDist(PC.iApplyToDist)->Name();
        else
          Str="All";
        break;
#endif
      }
    }

  if (CurrentBlk(EI))
    {//data
    int p=EI.PageNo;
    int i=(int)(EI.BlkRowNo-EI.Index);
    switch (EI.FieldId)
      {
      case Id_XInt:
      case Id_XIntRng:
        PC.GetRangeLbl(i, Str, fSzAscend, EI.FieldId==Id_XIntRng);
        EI.Fld->fEditable=PC.UserSizePts();
        break;
      default:
        if (EI.FieldId>=Id_YData && EI.FieldId<Id_YData+(NCurves()*2))
          {
          if (!fSzAscend)
            i=PC.SizePts().GetUpperBound()-i;  
          //PCG.YFmt.FormatFloat(PCG.YCnv.Human(PC.Crvs[EI.FieldId-Id_YData][i]), Str);
          PCG.YFmt.FormatFloat(PC.CrvInfo[EI.FieldId-Id_YData].DataCnv.Human(PC.Crvs[EI.FieldId-Id_YData][i]), Str);
          EI.Fld->fEditable = PC.Editable(EI.FieldId-Id_YData);
          }
        else if (EI.FieldId>=Id_YMode && EI.FieldId<Id_YMode+(NCurves()*2))
          {
          Str=PC.sPCModeStrings[PC.CrvInfo[EI.FieldId-Id_YMode].Mode]();
          }
        else if (EI.FieldId>=Id_YState && EI.FieldId<Id_YState+(NCurves()*2))
          {
          Str=PC.sPCStateStrings[PC.CrvInfo[EI.FieldId-Id_YState].State]();
          }
      }
    }
  if (CurrentBlk(EI))
    {//other
    switch (EI.FieldId)
      {
      case Id_SzAscend:  Str.Set("%i", fSzAscend); break;
      case Id_GrWidth:   Str.Set("%i", iGraphWidth); break;
      case Id_GrHeight:  Str.Set("%i", iGraphHeight); break;
      case Id_GrOn    :  Str.Set("%i", iGraphOn); break;
      }
    }
  }

//---------------------------------------------------------------------------

long SzPartCrv1Edt::Parse(FxdEdtInfo &EI, Strng & Str)
  {
  long Fix=0; //set Fix=1 to redraw graph
  
  if (CurrentBlk(EI))
    {//header
    //bObjModified=1;
    int p=EI.PageNo;
    int i=(int)(EI.BlkRowNo-EI.Index);
    switch (EI.FieldId)
      {
      case Id_XMin :  
        SetXDispMin(PCG.XCnv.Normal(SafeAtoF(Str, 0.0))); 
        View().DoReload();
        break;
      case Id_XMax :  
        SetXDispMax(PCG.XCnv.Normal(SafeAtoF(Str, 1.0)));  
        View().DoReload();
        break;
      case Id_PartLen:
        PC.SetLength(Range(3L,SafeAtoL(Str,5),100L));
        View().DoRebuild();
        break;
#if UseManualSizeDefn
      case Id_PartSizeDefn:
        {
        int i;
        for (i=SD_Defn.NIntervals()-1; i>=0; i--)
          if (Str.XStrICmp(SD_Defn.GetIntervals(i)->Name())==0)
            break;
        PC.SetSizeDefn(i); // if not found will become Manual (-1)
        View().DoRebuild();
        }
        break;
      case Id_ApplyToDist:
        {
        int i;
        for (i=SD_Defn.NDistributions()-1; i>=0; i--)
          if (Str.XStrICmp(SD_Defn.GetDist(i)->Name())==0)
            break;
        PC.SetApplyToDist(i);
        View().DoRebuild();
        }
        break;
#endif
      }
    }
  
  if (CurrentBlk(EI))
    {//data
    int p=EI.PageNo;
    int i=(int)(EI.BlkRowNo-EI.Index);
    switch (EI.FieldId)
      {
      case Id_XInt:  
      case Id_XIntRng:
        if (!fSzAscend)
          i=PC.SizePts().GetUpperBound()-i;  
        PC.SizePts()[i]=PCG.XCnv.Normal(SafeAtoF(Str,0.0)); 
        break;
      default:
        if (!fSzAscend)
          i=PC.SizePts().GetUpperBound()-i;  
        if (EI.FieldId>=Id_YData && EI.FieldId<Id_YData+(NCurves()*2))
          {
          const int c = EI.FieldId-Id_YData;
          //PC.Crvs[c][i]=Range(0.0, PCG.YCnv.Normal(SafeAtoF(Str,0.0)),1.0); 
          PC.Crvs[c][i]=Range(PC.CrvInfo[c].dLoRange, PC.CrvInfo[c].DataCnv.Normal(SafeAtoF(Str,0.0)), PC.CrvInfo[c].dHiRange);
          if (PC.CrvInfo[c].AutoCorrect && c<NCurves())
            PC.Crvs[c].AdjustCumulativeEnd(PC.CrvInfo[c].dHiRange, PC.CrvInfo[c].dLoRange, PC.CrvInfo[c].dHiRange);
          PC.CalcCumValues();
          View().DoReload();
          }
        else if (EI.FieldId>=Id_YMode && EI.FieldId<Id_YMode+(NCurves()*2))
          {
          int i;
          for (i=0; i<MaxPCModes; i++)
            if (Str.XStrICmp(PC.sPCModeStrings[i]())==0)
              break;
          if (i>=MaxPCModes)
            i=PC_Frac2Fine;
          PC.SetCurveMode(EI.FieldId-Id_YMode, i);
          }
        else if (EI.FieldId>=Id_YState && EI.FieldId<Id_YState+(NCurves()*2))
          {
          int i;
          for (i=0; i<MaxPCStates; i++)
            if (Str.XStrICmp(PC.sPCStateStrings[i]())==0)
              break;
          if (i>=MaxPCStates)
            i=PC_On;
          PC.SetCurveState(EI.FieldId-Id_YState, i);
          }
      }
    }

  if (CurrentBlk(EI))
    {//other
    switch (EI.FieldId)
      {
      case Id_SzAscend:
        fSzAscend = Str.SafeAtoL()!=0;
        View().DoRebuild();
        break;
      case Id_GrWidth:
        iGraphWidth = Range(10L, Str.SafeAtoL(), 60L);
        View().DoRebuild();
        break;
      case Id_GrHeight:  
        iGraphHeight = Range(5L, Str.SafeAtoL(), 30L);
        View().DoRebuild();
        break;
      case Id_GrOn:  
        iGraphOn= Range(0L, Str.SafeAtoL(), 1L);
        View().DoRebuild();
        break;
      }
    }
  return Fix;
  }

//---------------------------------------------------------------------------

long SzPartCrv1Edt::ButtonPushed(FxdEdtInfo &EI, Strng & Str)
  {
  long Fix=0; //set Fix=1 to redraw graph
  if (CurrentBlk(EI))
    {//header
    int p=EI.PageNo;
    int i=(int)(EI.BlkRowNo-EI.Index);
    switch (EI.FieldId)
      {
      case Id_AutoScale:
        SetAutoScale(!AutoScale());
        SetRanges();
        View().DoRebuild();
        Fix=1;
        break;
      case Id_XLog:
        SetXLog(!XLog());
        SetRanges();
        View().DoRebuild();
        Fix=1;
        break;
      case Id_PartStepped:
        SetPartStepped(!PartStepped());
        SetRanges();
        //bShowOther = !bShowOther;
        View().DoRebuild();
        Fix=1;
        break;
      case Id_GrOn    :  
        iGraphOn=!iGraphOn; 
        View().DoRebuild();
        Fix=1;
        break;
      }
    }

  if (CurrentBlk(EI))
    {//data
    /*int p=EI.PageNo;
    int i=(int)(EI.BlkRowNo-EI.Index);
    switch (EI.FieldId)
      {
      }*/
    }
  // Base Blk
  if (CurrentBlk(EI))
    {
    int p=EI.PageNo;
    int i=(int)(EI.BlkRowNo-EI.Index);
    switch (EI.FieldId)
      {
      case Id_SzAscend:  
        fSzAscend=!fSzAscend; 
        View().DoRebuild();
        Fix=1;
        break;
      case Id_GrOn    :  
        iGraphOn=!iGraphOn; 
        View().DoRebuild();
        Fix=1;
        break;
      }
    }
  return Fix;
  }

//---------------------------------------------------------------------------

void SzPartCrv1Edt::SetRanges()
  {
  const int len = Length();
  const flag IsXLog = XLog();
  const flag IsYLog = YLog();
  if (AutoScale())
    {
    if (len==0)
      {
      SetXDispMin(0.01);
      SetXDispMax(1.0);
      for (int c=0; c<NCurves(); c++)
        {
        SetYDispMin(c, 0.00);
        SetYFDispMax(c, 1.0);
        }
      }
    else
      {
      SetXDispMin(PC.SizePts().ScanMin());
      SetXDispMax(PC.SizePts().ScanMax());
      if (IsXLog)
        {
        SetXDispMin(XDispMin()*0.5);
        SetXDispMax(XDispMax()*1.1);
        }
      else
        {
        double xBorder = (XDispMax()-XDispMin())/40.0;
        SetXDispMin(XDispMin()-xBorder);
        SetXDispMax(XDispMax()+xBorder);
        }
      }
    }
  SetXDispMax(Max(XDispMax(), XDispMin()+1.0e-6));
  if (IsXLog)
    {//set ranges to nearest decade
    SetXDispMin(Max(XDispMin(), MinLogVal));
    SetXDispMin(pow(10.0, floor(Log10(XDispMin()))));
    SetXDispMax(pow(10.0, ceil(Log10(XDispMax()))));
    }

  if (AutoScale() && len>0)
    {
    for (int c=0; c<NCurves(); c++)
      {
      SetYDispMin(c, 0.0);
      SetYFDispMax(c, PC.Crvs[c].ScanMax());
      }
    }
  for (int c=0; c<NCurves(); c++)
    {
    SetYFDispMax(c, Max(YFDispMax(c), YDispMin(c)+1.0e-6));
    if (IsYLog)
      {
      SetYDispMin(c, Max(YDispMin(c), MinLogVal));
      SetYDispMin(c, pow(10.0, floor(Log10(YDispMin(c)))));
      SetYFDispMax(c, pow(10.0, ceil(Log10(YFDispMax(c)))));
      }
    }
  }

//---------------------------------------------------------------------------

void SzPartCrv1Edt::OnDrawBack(rGDIBlk GB,int PgNo, CRgn &ClipRgn)
  {
  
  if (GraphOn() && (PgNo-iPg1>=0))
    {
//    SetWorkDist(DistFromPgNo(PgNo-iPg1));

    CDC &DC = GB.DC();
    CDCResChk ResChk(DC);
    SetPosition(GB); //calulate size of GraphR
    flag DoGraph = DC.RectVisible(&GraphR);

    CGdiObject* OldBrush = (CGdiObject*)DC.SelectObject(GB.pBrushGrfBack);

    if (DoGraph)
      {//draw the curve...
      int Err = ExtSelectClipRgn(DC.m_hDC, (HRGN)ClipRgn.m_hObject, RGN_AND);

      COLORREF OldBkColor = DC.SetBkColor(GB.crGrfBack);
      CPen* OldPen = DC.SelectObject(GB.pPenTxtBord);

      //CombinedRgn.CombineRgn(&ClipRgnGrf, &ClipRgn, RGN_AND);

      //int Err = ExtSelectClipRgn(DC.m_hDC, (HRGN)CombinedRgn.m_hObject, RGN_AND);
      Err = ExtSelectClipRgn(DC.m_hDC, (HRGN)ClipRgn.m_hObject, RGN_AND);
    
      DC.DrawEdge(&GraphR, EDGE_SUNKEN, BF_RECT);
      int EdgeX=GetSystemMetrics(SM_CXEDGE);
      int EdgeY=GetSystemMetrics(SM_CYEDGE);
      GraphR.InflateRect(-EdgeX*2, -EdgeY*2);

      DC.Rectangle(&GraphR);
      MappingSave MapSv;
      PushScaling(GB, GraphR, MapSv);

      CRgn ClipRgnGrf;
      CRgn CombinedRgn;
      ClipRgnGrf.CreateRectRgnIndirect(&GraphR);
      //
      //CombinedRgn.CombineRgn(&ClipRgnGrf, &ClipRgn, RGN_AND);

      //int Err = ExtSelectClipRgn(DC.m_hDC, (HRGN)CombinedRgn.m_hObject, RGN_AND);
      Err = ExtSelectClipRgn(DC.m_hDC, (HRGN)ClipRgnGrf.m_hObject, RGN_AND);

      CPoint Org = DC.GetWindowOrg();
      CSize Ext = DC.GetWindowExt();

  	  const int len = Length();
      const flag IsXLog = XLog();
      const flag IsYLog = YLog();
      //
      SetRanges();
      //
      //get normalised values...
      if (IsXLog)
        {
        XMin = Log10(XDispMin());
        XMax = Log10(XDispMax());
        }
      else
        {
        XMin = XDispMin();
        XMax = XDispMax();
        }
      const double dX = GTZ(XMax-XMin);
      XFactor = XPix/dX;

      YMin.SetSize(NCurves());
//      YCMax.SetSize(NCurves());
      YFMax.SetSize(NCurves());
//      YCFactor.SetSize(NCurves());
      YFFactor.SetSize(NCurves());
      for (int c=0; c<NCurves(); c++)
        {
        YMin[c] = YDispMin(c);
//        YCMax[c] = YCDispMax(c);
        YFMax[c] = YFDispMax(c);
//        const double dCY = GTZ(YCMax[c]-YMin[c]);
        const double dFY = GTZ(YFMax[c]-YMin[c]);
//        YCFactor[c] = YPix/dCY;
        YFFactor[c] = YPix/dFY;
        }

      //draw axis...
      ::SelectObject(DC.m_hDC, GB.hLinePenDim[0]);
      int x1,y1;
      if (!IsXLog)
        {
        x1 = (int)Range((double)-INT_MAX, (0.0-XMin)*XFactor, (double)INT_MAX);
        //x1 = (int)Range((double)-INT_MAX, (rSD.GetNormX(0.0)-XMin)*XFactor, (double)INT_MAX);
        DC.MoveTo(x1,Org.y);
        DC.LineTo(x1,Org.y+Ext.cy);
        }
      if (!IsYLog)
        {
        for (int c=0; c<NCurves(); c++)
          { // Fix This ??? Make Axes Coincide
          y1 = (int)Range((double)-INT_MAX, (0.0-YMin[c])*YFFactor[c], (double)INT_MAX);
          //y1 = (int)Range((double)-INT_MAX, (rSD.GetNormY(0.0)-YMin)*YFactor, (double)INT_MAX);
          DC.MoveTo(Org.x,y1);
          DC.LineTo(Org.x+Ext.cx,y1);
          }
        }
      //draw X log decades...
      if (IsXLog)
        {
        int Decades = (int)(Log10(XDispMax()) - Log10(XDispMin()));
        double Xv = XDispMin();
        for (int j=0; j<Decades; j++)
          {
          ::SelectObject(DC.m_hDC, GB.hLinePenDim[1]);
          for (int i=1; i<10; i++)
            {
            x1 = (int)Range((double)-INT_MAX, (Log10(Xv*i)-XMin)*XFactor, (double)INT_MAX);
            DC.MoveTo(x1,Org.y);
            DC.LineTo(x1,Org.y+Ext.cy);
            if (i==1)
              ::SelectObject(DC.m_hDC, GB.hLinePenDim[0]);
            if (Decades>4)
              i++;
            }
          Xv *= 10.0;
          }
        }
   
      ::SelectObject(DC.m_hDC, GB.hLinePen[0]);

      CDArray & Xs=PC.SizePts();//GetXData();
      const int dCrossX = XPix/150;
      const int dCrossY = YPix/150;

      const int FirstPen=3;
    
      //draw the curves...
      int iPen=FirstPen;
      int XYLen;
      if (Xs.GetSize()>0)
        for (c=0; c<NCurves(); c++)
          {
          ::SelectObject(DC.m_hDC, GB.hLinePen[iPen++]);
          CDArray &Ys=PC.Crvs[c];
          POINT XY[CrvPts];
          XYLen=0;
          for (int j=0; j<Xs.GetSize()/*-1*/; j++)
            {
            int x=IsXLog ? CalcXLogPix(Xs[j]) : CalcXPix(Xs[j]);
            int y=IsYLog ? CalcYFLogPix(c, Ys[j]) : CalcYFPix(c, Ys[j]);
            if (PartStepped())
              {
              if (XYLen>0)
                XY[XYLen].x = XY[XYLen-1].x;
              else
                XY[XYLen].x = IsXLog ? CalcXLogPix(PC.BottomSizeDisp()) : CalcXPix(PC.BottomSizeDisp());//-XPix;
              XY[XYLen].y = y;//XY[XYLen-1].y;
              XYLen++;
              }
            XY[XYLen].x = x;
            XY[XYLen].y = y;
            XYLen++;
            }
          XY[XYLen].x = IsXLog ? CalcXLogPix(PC.TopSizeDisp()) : CalcXPix(PC.TopSizeDisp());
          XY[XYLen].y = XY[XYLen-1].y;
          XYLen++;

          DC.Polyline(XY, XYLen);
          }
          //}
    
      // Restore State
      PopScaling(GB, MapSv);
      DC.SelectObject(OldPen);
      DC.SetBkColor(OldBkColor);
      DC.SelectClipRgn(NULL);
      }
    DC.SelectObject(OldBrush);
    }
  }

//---------------------------------------------------------------------------

void SzPartCrv1Edt::SetPosition(rGDIBlk GB)
  {
  int CW = GB.ColWdt();
  int RH = GB.RowHgt();
  GraphR = GB.LuDataRect();
  GraphR.top -= 1;
  GraphR.right = GraphR.left+GraphWidth()*CW;
  GraphR.bottom = GraphR.top+GraphHeight()*RH;

  GraphR.top+=View().CPg->FixedRows*RH;
  GraphR.bottom+=View().CPg->FixedRows*RH;

  int EdgeX=GetSystemMetrics(SM_CXEDGE);
  int EdgeY=GetSystemMetrics(SM_CYEDGE);
  GraphR.InflateRect(-EdgeX*2, -EdgeY*2);
  
  //iNameWidth = Max(rSD.XNameLen(), rSD.YNameLen());
  pLabels->SetPosition(GraphR.left, GraphR.bottom+1, iNameWidth + 18);
  
  }

//---------------------------------------------------------------------------

void SzPartCrv1Edt::PointtoLP(POINT &Pt)
  {
  CDC* pDC = &(View().ScrGB.DC());
  CDCResChk ResChk(pDC);
  MappingSave MapSv;
  PushScaling(View().ScrGB, GraphR, MapSv);
  pDC->DPtoLP(&Pt);
  PopScaling(View().ScrGB, MapSv);
  }

//---------------------------------------------------------------------------

void SzPartCrv1Edt::PointtoWP(POINT Pt, double& x, double& y)
  {
  rGDIBlk rGB = View().ScrGB;
  CDC& rDC = rGB.DC();
  CDCResChk ResChk(rDC);
  MappingSave MapSv;
  PushScaling(View().ScrGB, GraphR, MapSv);
  rDC.DPtoLP(&Pt);
  PopScaling(View().ScrGB, MapSv);
  
  if (XLog())
    x = CalcXLogVal(Pt.x);
  else
    x = CalcXVal(Pt.x);
  }

//---------------------------------------------------------------------------

int SzPartCrv1Edt::ClosestPt(CPoint point)
  {
  double x,y;
  PointtoWP(point, x, y);
  int PtNo = -1;
  double dist = DBL_MAX;
  return PtNo;
  }

//---------------------------------------------------------------------------

void SzPartCrv1Edt::ToggleDigCursor(POINT &Pt)
  {
  CClientDC dc(&Wnd(pView));
  CDCResChk ResChk(dc);

  MappingSave MapSv;
  PushScaling(View().ScrGB, GraphR, MapSv);
  CPen APen(PS_SOLID,1,RGB(0xFF, 0xFF, 0xFF));//dc.GetNearestColor(Doc().DigColour));
  CGdiObject* OldPen=(CGdiObject*)dc.SelectObject(&APen);
  int OldMode=dc.SetROP2(R2_XORPEN);
  dc.MoveTo(Pt.x,0);
  dc.LineTo(Pt.x,YPix);
  dc.MoveTo(0, Pt.y);
  dc.LineTo(XPix, Pt.y);
  dc.SetROP2(OldMode);
  PopScaling(View().ScrGB, MapSv);
  }

//---------------------------------------------------------------------------

flag SzPartCrv1Edt::DoLButtonDown(UINT nFlags, CPoint point)
  {
  if (PtInRect(&GraphR, point))
    {
    if (nFlags & MK_SHIFT)
      iDragPtNo = ClosestPt(point);
    return 1;
    }
  return 0;
  }

//---------------------------------------------------------------------------

flag SzPartCrv1Edt::DoLButtonUp(UINT nFlags, CPoint point)
  {
  if (PtInRect(&GraphR, point))
    {
    if (iDragPtNo>=0)
      {
      View().DoReload();
      iDragPtNo = -1;
      }
    return 1;
    }
  return 0;
  }

//---------------------------------------------------------------------------

flag SzPartCrv1Edt::DoLButtonDblClk(UINT nFlags, CPoint point)
  {
  if (PtInRect(&GraphR, point))
    {
    return 1;
    }
  return 0;
  }

//---------------------------------------------------------------------------

flag SzPartCrv1Edt::DoRButtonDown(UINT nFlags, CPoint point)
  {
  if (pView->CPgNo-iPg1>=0 && PtInRect(&GraphR, point))
    {
    return 1;
    }
  return 0;
  }

//---------------------------------------------------------------------------

flag SzPartCrv1Edt::DoRButtonUp(UINT nFlags, CPoint point)
  {
  if (pView->CPgNo-iPg1>=0 && PtInRect(&GraphR, point))
    {
    return 1;
    }
  FxdEdtView &Vw=View();
  flag ret=false;//FxdEdtView::DoRButtonUp(nFlags, point);
  if (Vw.CPgNo>=0)
    {
    FxdEdtInfo EI;
    if (Vw.LocateFromCR((int)Vw.ChEditPos.x, (int)Vw.ChEditPos.y, EI))
      {
      ret=true;
      //if (EI.FieldId<0 && EI.Fld)
      if (EI.Fld)
        {
        int nCols=MaxColumns;//NCurves();
        if (EI.FieldId==Id_XInt ||
            EI.FieldId==Id_XIntRng ||
            EI.FieldId==Id_XMin ||
            EI.FieldId==Id_XMax ||
            (EI.FieldId>=Id_YMode && EI.FieldId<Id_YMode+nCols) ||
            (EI.FieldId>=Id_YState && EI.FieldId<Id_YState+nCols) ||
            (EI.FieldId>=Id_YTitle && EI.FieldId<Id_YTitle+nCols) ||
            (EI.FieldId>=Id_dSize && EI.FieldId<Id_dSize+MaxCSDMeasurements*MaxColumns) ||
            (EI.FieldId>=Id_YMin && EI.FieldId<Id_YMin+nCols) ||
//            EI.FieldId>=Id_YCMax && EI.FieldId<Id_YCMax+nCols ||
            (EI.FieldId>=Id_YFMax && EI.FieldId<Id_YFMax+nCols) ||
            (EI.FieldId>=Id_YData && EI.FieldId<Id_YData+nCols) )
          {
          CRect WRect;
          Vw.GetWindowRect(&WRect);
          CPoint  RBPoint;
          RBPoint.x = WRect.left+point.x;
          RBPoint.y = WRect.top+point.y;
          if ( (EI.FieldId>=Id_YMode && EI.FieldId<Id_YMode+nCols) ||
               (EI.FieldId>=Id_YState && EI.FieldId<Id_YState+nCols) ||
               (EI.FieldId>=Id_YTitle && EI.FieldId<Id_YTitle+nCols) )
            {
            //pWrkFmt=NULL;
            //pWrkCnv=NULL;
            WrkIB.Set(EI.Fld->Tag);
            }
          else if (EI.FieldId==Id_XInt ||
              EI.FieldId==Id_XIntRng ||
              EI.FieldId==Id_XMin ||
              EI.FieldId==Id_XMax ||
              EI.FieldId>=Id_dSize && EI.FieldId<Id_dSize+MaxCSDMeasurements*MaxColumns)
            {
            WrkIB.Set(EI.Fld->Tag, &PCG.XCnv, &PCG.XFmt);
            //pWrkFmt=&PCG.XFmt;
            //pWrkCnv=&PCG.XCnv;
            }
          else
            {
            int c=(EI.FieldId-Id_YMin)%MaxColumns;
            //pWrkFmt=&PCG.YFmt;
            //pWrkCnv=&PCG.YCnv;
            //pWrkCnv=&(PC.CrvInfo[c].DataCnv);
            WrkIB.Set(EI.Fld->Tag, &(PC.CrvInfo[c].DataCnv), &PCG.YFmt);
            }

          CMenu Menu;
          Menu.CreatePopupMenu();
          CMenu FormatMenu;
          FormatMenu.CreatePopupMenu();
          if (WrkIB.FmtOK())
            {
            WrkIB.Fmt().AddToMenu(FormatMenu);
            CMenu CnvMenu;
            CnvMenu.CreatePopupMenu();
            WrkIB.Cnv().AddToMenu(CnvMenu);

            Menu.AppendMenu(MF_POPUP, (unsigned int)CnvMenu.m_hMenu, "&Conversions");
            if (WrkIB.Cnv().Index()<=0)// || !IsFloatData(d.iType))
              Menu.EnableMenuItem(0, MF_BYPOSITION|MF_GRAYED);

            Menu.AppendMenu(MF_POPUP, (unsigned int)FormatMenu.m_hMenu, "&Format");
            Menu.AppendMenu(MF_STRING, pView->iSendToTrend, "Send To &Trend");
            Menu.AppendMenu(MF_STRING, pView->iSendToQuickView, "Send To &QuickView");
            Menu.AppendMenu(MF_STRING|MF_GRAYED, pView->iSendToQuickView, "Send To &Custom");

            Menu.AppendMenu(MF_SEPARATOR);
            }
          
          Menu.AppendMenu(MF_STRING, pView->iCmdCopyTag, "Copy Full &Tag");
          if (EI.Fld->Tag==NULL)
            Menu.EnableMenuItem(pView->iCmdCopyTag, MF_BYCOMMAND|MF_GRAYED);
          Menu.AppendMenu(MF_STRING, pView->iCmdCopyVal, "Copy &Value");
          

          Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, RBPoint.x, RBPoint.y, /*(CWnd*)*/&View());
          Menu.DestroyMenu();                                           
          }
        }
      }
    }
  return ret;
  //return 0;
  }

//---------------------------------------------------------------------------

flag SzPartCrv1Edt::DoRButtonDblClk(UINT nFlags, CPoint point)
  {
  if (pView->CPgNo-iPg1>=0 && PtInRect(&GraphR, point))
    {
    return 1;
    }
  return 0;
  }

//---------------------------------------------------------------------------

flag SzPartCrv1Edt::DoMouseMove(UINT nFlags, CPoint point)
  {
  if (GraphOn() && PtInRect(&GraphR, point))
    {
    double x,y;
    PointtoWP(point, x, y);
    char Buff[256], Buff2[256];
    iNameWidth=4;
    PCG.XFmt.FormatFloat(PCG.XCnv.Human(x), Buff2, sizeof(Buff2));
    sprintf(Buff, "%*s:%s", iNameWidth, "Size", Buff2);
    pLabels->SetText(0, Buff);
    pLabels->Show();
    pLabels->Invalidate();
    if (nFlags & MK_LBUTTON)
      {
      if (nFlags & MK_SHIFT)
        {
        if (iDragPtNo>=0)
          {
          //rSD.MovePt(iDragPtNo, x, y);
          //rSD.bObjModified = 1;
          }
        }
      else
        iDragPtNo = -1;
      Wnd(pView).InvalidateRect(&GraphR);
      }
    return 1;
    }
  else
    {
    pLabels->Hide();
    }
  return 0;
  }

//---------------------------------------------------------------------------

flag SzPartCrv1Edt::DoAccCnv(UINT Id) 
  {
  pCDataCnv pC=Cnvs[WrkIB.Cnv().Index()];
  for (int i=Id; i>0; i--)
    pC=pC->Next();
    
  if (pC)
    {
    WrkIB.Cnv().SetText(pC->Txt());
    }
  View().DoRebuild();
  return true;
  };

//---------------------------------------------------------------------------

flag SzPartCrv1Edt::DoAccFmt(UINT Id) 
  {
  for (UINT i=0; i<(UINT)DefinedFmts.GetSize(); i++) 
    if (i==Id) 
      break;

  if (i<(UINT)DefinedFmts.GetSize())
    {
    WrkIB.Fmt()=DefinedFmts[i];
    View().DoRebuild();
    }
  return true;
  };

//---------------------------------------------------------------------------

flag SzPartCrv1Edt::DoAccRptTagLists()
  {
  return FxdEdtBookRef::DoAccRptTagLists();
  //TagInfoBlk * IB = new TagInfoBlk(WrkIB.ObjClassId(), WrkIB.RefTag(), 
  //                                 WrkIB.CnvOK() ? &WrkIB.Cnv() : NULL, 
  //                                 WrkIB.FmtOK() ? &WrkIB.Fmt() : NULL);
  //ScdMainWnd()->PostMessage(WMU_ADDTOQUICKVIEW, 0, (long)(IB));
  //
  //return true;
  };

//===========================================================================
//
//
//
//===========================================================================

#if WITHSELBRK

IMPLEMENT_SPARES(SzSelBrk1, 100);
IMPLEMENT_TAGOBJEDT(SzSelBrk1, "SpSelBreak", "SzSelBrk", "", "SZSB", TOC_ALL|TOC_GRP_SIZEDST|TOC_SIZEDIST, SzSelBrk1Edt, 
                 "Size Selection & Breakage",
                 "Size Selection & Breakage");
SzSelBrk1::SzSelBrk1(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach) :
  TaggedObject(pClass_, Tag_, pAttach, eAttach)//,
  {
  iApplyToDist=0;
  SetSizeDefn(0);
  }

// --------------------------------------------------------------------------

SzSelBrk1::SzSelBrk1(pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach) :
  TaggedObject(&SzSelBrk1Class, Tag_, pAttach, eAttach)//,
  {
  iApplyToDist=0;
  SetSizeDefn(0);
  }

// --------------------------------------------------------------------------

SzSelBrk1::~SzSelBrk1()
  {
  }

// --------------------------------------------------------------------------

void SzSelBrk1::SetSizeDefn(int iDef)
  {
  iSizeDefn=iDef;//MAX_INT;
  #if !UseManualSizeDefn
  if (iSizeDefn<0)
    iSizeDefn=0;
  #endif

  int L=Length();
  if (iSizeDefn>=0 && SD_Defn.NIntervals()>iSizeDefn)
    {
    double dBottomSize=1.0e6;
    double dTopSize=0.0;
    if (0)
      {//old code...
      double dBottomSizeDisp=1.0e6;
      double dTopSizeDisp=0.0;
      int d0, dN;
      GetApplyToDists(d0, dN);
      for (int d=d0; d<dN; d++)
        if (DistExists(d))
          {
          CSD_DistDefn &D=*SD_Defn.GetDist(d);
          dBottomSize=Min(dBottomSize, D.BottomSize());
          dTopSize=Max(dTopSize, D.TopSize());
          dBottomSizeDisp=Min(dBottomSizeDisp, D.BottomSizeDisp());
          dTopSizeDisp=Max(dTopSizeDisp, D.TopSizeDisp());
          }
      }
    else
      {//new code March 2003
      CSD_DistDefn &D=*SD_Defn.GetDist(iSizeDefn);
      dBottomSize=D.BottomSize();
      dTopSize=D.TopSize();
      }


    LclSz.SetSize(0);
    CSD_Intervals &Intervals=*SD_Defn.GetIntervals(iSizeDefn);
    const int icnt = Intervals.GetSize();
    for (int i=0; i<icnt; i++)
      {
      if ((Intervals[i]>=dBottomSize) &&
          ((Intervals.GetSize()==0) || (Intervals[i]<=dTopSize)))
        LclSz.Add(Intervals[i]);
      }
    if (icnt==0 || Intervals[icnt-1]<dTopSize)
      LclSz.Add(dTopSize); // Oversize Interval
    //double OvrInt=dTopSizeDisp;//BigSizeInterval;
    //LclSz.Add(OvrInt); // Oversize Interval
    }

  L=SB.GetSize();
  SetLength(Length());
  if (Length()>L)
    {
    for (int s=L; s<Length(); s++)
      {
      SB[s][s]=0.1;
      for (int d=L; d<s; d++)
        SB[s][d]=0;
      }
    SB[0][0]=0.0;
    }
  CheckSB();
  }

// --------------------------------------------------------------------------

void SzSelBrk1::CheckSB()  
  { 
  for (int s=0; s<Length(); s++)
    {
    SB[s][s]=Range(0.0, SB[s][s], 1.0);
    double t=0;
    for (int d=0; d<s; d++)
      t+=SB[s][d];
    double Err=t-1.0;
    for (d=0; d<s; d++)
      {
      double Rqd=Range(0.0, SB[s][d]-Err, 1.0);
      Err+=Rqd-SB[s][d];
      SB[s][d]=Rqd;
      }
    }
  SB[0][0]=0.0;
  }

// --------------------------------------------------------------------------

void SzSelBrk1::SetLength(int N)  
  { 
  SB.SetSizes(N, N); 
  }

// --------------------------------------------------------------------------

//Does Nothing: 
//void SzSelBrk1::BuildDataDefn_Lcl(DataDefnBlk & DDB)
//  {
//  }

// --------------------------------------------------------------------------

void SzSelBrk1::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginObject(this, Tag(), SzSelBrk1Class.ClassId(), NULL, DDB_OptPage);

  DDBValueLstMem DDB0;
  DDB0.Empty();
  for (int i=0; i<SD_Defn.NIntervals(); i++)
    DDB0.Add(i, SD_Defn.GetIntervals(i)->Name());
#if UseManualSizeDefn
  DDB0.Add(-1, "Manual");
#endif
  
  DDB.Long("SizeDefn",   "",  DC_ ,   "", xidPartSizeDefn,             this, isParm, DDB0());
  DDB.Long("Length",     "",  DC_ ,   "", xidPartLength,               this, isParm);

  DDB0.Empty();
  for (i=0; i<SD_Defn.NDistributions(); i++)
    DDB0.Add(i, SD_Defn.GetDist(i)->Name());
  DDB0.Add(-1, "All");
  
  DDB.Long("ApplyTo",    "",  DC_ ,   "", xidApplyToDist,             this, isParm, DDB0());
      
  Strng Nm;
  if (DDB.BeginArray(this, "SZSB", "SZSB", Length()))
    for (int s=0; s<Length(); s++)
      if (DDB.BeginElement(this, s))
        {
        //DDB.Double("Size",    "", DC_L, "um", &SizePts()[s], this, isParm);
        DDB.Double("SzRange", "", DC_L, "um", &SizePts()[s], this, DDEF_WRITEPROTECT|noFileAtAll);
        DDB.Double("Selection", "", DC_Frac, "%", &SB[s][s], this, isParm);
        for (int d=0; d<s; d++)
          {
          Nm.Set("Brk%i", d);    
          DDB.Double(Nm(),    "",  DC_Frac, "%",  &SB[s][d], this, isParm);
          }
        }
  DDB.EndArray();
  DDB.EndObject();
  }

// --------------------------------------------------------------------------

flag SzSelBrk1::DataXchg(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
    case xidPartSizeDefn: 
      if (DCB.rL)
        SetSizeDefn(Range(-1L, *DCB.rL, SD_Defn.NIntervals()-1L));
      DCB.L=iSizeDefn;
      return 1;
    case xidApplyToDist:
      if (DCB.rL)
        {
        iApplyToDist=Range(-1L, *DCB.rL, SD_Defn.NDistributions()-1L);
        SetSizeDefn(iSizeDefn);
        }
      DCB.L=iApplyToDist;
      return 1;
    case xidPartLength: 
      if (DCB.rL)
        SetLength(*DCB.rL);
      DCB.L=Length();
      return 1;
    }
  return TaggedObject::DataXchg(DCB);
  }

// --------------------------------------------------------------------------

flag SzSelBrk1::ValidateData(ValidateDataBlk & VDB)
  {
  //flag OK=true;
  return true;
  }

// --------------------------------------------------------------------------

char* SzSelBrk1::GetRangeLbl(int i, Strng& Str, flag Ascend, flag Range)
  {
  if (Range)
    {
    if (fSzAscend)
      {
      if (i>=SizePts().GetUpperBound())
        {
        Strng S;
        PCG.XFmt.FormatFloat(PCG.XCnv.Human(SizePts()[i-1]), S);
        Str="+";
        Str+=S;
        }
      else
        {
        Strng S;
        PCG.XFmt.FormatFloat(PCG.XCnv.Human(SizePts()[i]), S);
        Str="-";
        Str+=S;
        }
      }
    else
      {
      if (i>0)  
        {
        Str="";
        Strng S;
        PCG.XFmt.FormatFloat(PCG.XCnv.Human(SizePts()[i-1]), S);
        Str+="+";
        Str+=S;
        }
      else
        {
        Strng S;
        PCG.XFmt.FormatFloat(PCG.XCnv.Human(SizePts()[0]), S);
        Str="-";
        Str+=S;
        }
      }
    }
  else
    {
    PCG.XFmt.FormatFloat(PCG.XCnv.Human(SizePts()[i]), Str);
    }
  return Str();
  }


//===========================================================================
//
//
//
//===========================================================================

SzSelBrk1Edt::SzSelBrk1Edt(pFxdEdtView pView_, pSzSelBrk1 pSD_) :
  FxdEdtBookRef(pView_),
  PC(*pSD_),
  PCG(pSD_->PCG)
  {
  iNameWidth=4;
  iWorkDist=0;
  iPg1=0;

  //pWrkCnv=NULL;
  //pWrkFmt=NULL;

  ObjectAttribute *pAttr=ObjAttributes.FindObject("SzSelBrk1Edt");
  if (pAttr)
    {
    pAttr->FieldFmtCnvs("Size", PCG.XFmt, PCG.XCnv);
    pAttr->FieldFmtCnvs("Frac", PCG.YFmt, PCG.YCnv);
    }

  }

//---------------------------------------------------------------------------

SzSelBrk1Edt::~SzSelBrk1Edt()
  {
  ObjectAttribute *pAttr=ObjAttributes.FindObject("SzSelBrk1Edt");
  if (pAttr)
    {
    pAttr->SetFieldFmt("Size",  PCG.XFmt);
    pAttr->SetFieldCnvs("Size", PCG.XCnv);
    pAttr->SetFieldFmt("Frac",  PCG.YFmt);
    pAttr->SetFieldCnvs("Frac", PCG.YCnv);
    }
  
  }

//---------------------------------------------------------------------------

void SzSelBrk1Edt::PutDataStart()
  {
  }

//---------------------------------------------------------------------------

void SzSelBrk1Edt::PutDataDone()
  {
  }

//---------------------------------------------------------------------------

void SzSelBrk1Edt::GetDataStart()
  {
  }

//---------------------------------------------------------------------------

void SzSelBrk1Edt::GetDataDone()
  {
  }

//---------------------------------------------------------------------------

void SzSelBrk1Edt::StartBuild()
  {
  }

//---------------------------------------------------------------------------

void SzSelBrk1Edt::Build()
  {

  Strng Tg;
  Strng S;

  iPg1=pView->Pages;
  StartPage("SelBrk");
  if (1) // Head Blk
    {
#if UseManualSizeDefn
    int HedLen=(SD_Defn.NDistributions()>1 ? 3 : 2);
#else
    int HedLen=0;
#endif
    StartBlk(HedLen, 0, NULL);
    int L=0;
#if UseManualSizeDefn
    int FLen=8;
    for (int i=0; i<SD_Defn.Intervals.GetSize(); i++)
      FLen=Max(FLen, SD_Defn.Intervals[i]->NameLength());
    SetDParm(L, "SizeDefn", 10, "", Id_PartSizeDefn, FLen, 0, "");
    for (i=0; i<SD_Defn.Intervals.GetSize(); i++)
      FldHasFixedStrValue(i, SD_Defn.Intervals[i]->Name());
    FldHasFixedStrValue(-1, "Manual");
    SetSpace(L, 5);
    SetDParm(L, "Length", 10, "", Id_PartLen, 3, 0, "");

    L++;
    if (SD_Defn.NDistributions()>1)
      {
      FLen=8;
      for (i=0; i<SD_Defn.NDistributions(); i++)
        FLen=Max(FLen, SD_Defn.Distributions[i]->NameLength());
      SetDParm(L, "Apply To", 10, "", Id_ApplyToDist, FLen, 0, "");
      for (i=0; i<SD_Defn.NDistributions(); i++)
        FldHasFixedStrValue(i, SD_Defn.Distributions[i]->Name());
      FldHasFixedStrValue(-1, "All");
      L++;
      }
#endif
    }

  int SzRngLen = 10;
  if (1) // Data Blk
    {
    int HedLen=4;
    StartBlk(Length()+HedLen+2, HedLen, NULL);
    int L=0;

    //..
    L++;
    Tg.Set("SzRange(%s)", PCG.XCnv.Text());
    SetDesc(L, Tg(),  -1, SzRngLen+4,  2, " ");
    for (int iSz=0; iSz<Length()-1; iSz++)
      {
      SetParm(L, "", Id_XIntRange+iSz, 8, 2, "");
      Tg.Set("%s.SZSB.[%i].%s", FullObjTag(), Length()-1-iSz, "SzRange");
      SetTag(Tg(), PCG.XCnv.Text());
      }
    //..
    L++;
    Tg.Set("Selection (%s)", PCG.YCnv.Text());
    SetDesc(L, Tg(), -1, SzRngLen+4,  2, " ");
    for (iSz=0; iSz<Length()-1; iSz++)
      {
      SetParm(L, "", Id_YSelection+iSz, 8, 2, "");
      Tg.Set("%s.SZSB.[%i].Selection", FullObjTag(), Length()-1-iSz);
      SetTag(Tg(), PCG.YCnv.Text());
      }
    //..
    L++;
    for (iSz=0; iSz<Length(); iSz++)
      {
      L++;
      S.Set("%2i", Length()-1-iSz);
      SetDesc(L, S(), -1 , 4, 3, "");
      SetParm(L, "", Id_XIntRng, SzRngLen, 2, " ");
      Tg.Set("%s.SZSB.[%i].%s", FullObjTag(), Length()-1-iSz, "SzRange");
      SetTag(Tg(), PCG.XCnv.Text());
      if (iSz==0)
        {
        Tg.Set("Breakage (%s)", PCG.YCnv.Text());
        SetDesc(L, Tg(), -1, 20,  0, " ");
        }
      for (int c=0; c<iSz; c++)
        {
        SetParm(L, "", Id_YData+c, 8, 2, "");
        Tg.Set("%s.SZSB.[%i].Brk%i", FullObjTag(), Length()-1-c, Length()-1-iSz);
        SetTag(Tg(), PCG.YCnv.Text());
        }
      }
    //..
    L++;
    Tg.Set("SzRange(%s)", PCG.XCnv.Text());
    SetDesc(L, Tg(),  -1, SzRngLen+4,  2, " ");
    for (iSz=0; iSz<Length()-1; iSz++)
      {
      SetParm(L, "", Id_XIntRange+iSz, 8, 2, "");
      Tg.Set("%s.SZSB.[%i].%s", FullObjTag(), Length()-1-iSz, "SzRange");
      SetTag(Tg(), PCG.XCnv.Text());
      }

/*    //..
    Tg.Set("SzRange(%s)", PCG.XCnv.Text());
    SetDesc(L, Tg(),  -1, SzRngLen+4,  2, " ");
    
    SetParm(L, "", Id_YData+1, 8, 2, "");
    Tg.Set("%s.[%i].%s", FullObjTag(), Length()-1, "SzRange");
    SetTag(Tg(), PCG.XCnv.Text());
    
    SetSpace(L, 10);
    Tg.Set("Select / Breakage(%s)", PCG.YCnv.Text());
    SetDesc(L, Tg(), -1, 20,  0, " ");
    //..
    for (int iSz=0; iSz<Length(); iSz++)
      {
      L++;
      S.Set("%2i", Length()-1-iSz);
      SetDesc(L, S(), -1 , 4, 3, "");
      SetParm(L, "", Id_XIntRng, SzRngLen, 2, " ");
      Tg.Set("%s.SZSB.[%i].%s", FullObjTag(), Length()-1-iSz, "SzRange");
      SetTag(Tg(), PCG.XCnv.Text());
      for (int c=0; c<=iSz; c++)
        {
        SetParm(L, "", Id_YData+c, 8, 2, "");
        if (c==iSz)
          Tg.Set("%s.SZSB.[%i].Selection", FullObjTag(), Length()-1-iSz);
        else
          Tg.Set("%s.SZSB.[%i].Brk%i", FullObjTag(), Length()-1-c, Length()-1-iSz);
        SetTag(Tg(), PCG.YCnv.Text());
        }
      if (iSz<Length()-1)
        {
        SetParm(L, "", Id_YData+iSz+1, 8, 2, "");
        S.Set("%2i", Length()-2-iSz);
        Tg.Set("%s.%s.%s", FullObjTag(), S(), "SzRange");
        SetTag(Tg(), PCG.XCnv.Text());
        }
      }*/

    }
  }

//---------------------------------------------------------------------------

void SzSelBrk1Edt::Load(FxdEdtInfo &EI, Strng & Str)
  {
  if (CurrentBlk(EI))
    {//header
    int p=EI.PageNo;
    int i=(int)(EI.BlkRowNo-EI.Index);
    switch (EI.FieldId)
      {
      case Id_PartLen:
        Str.Set("%i", Length());
        EI.Fld->fEditable=PC.UserSizePts();
        break;
      case Id_PartSizeDefn:
        if (!PC.UserSizePts())//iSizeDefn<SD_Defn.Intervals.GetSize())
          Str=SD_Defn.GetIntervals(PC.iSizeDefn)->Name();
        else
          Str="Manual";
        break;
      case Id_ApplyToDist:
        if (PC.iApplyToDist>=0)//iSizeDefn<SD_Defn.Intervals.GetSize())
          Str=SD_Defn.GetDist(PC.iApplyToDist)->Name();
        else
          Str="All";
        break;
      }
    }

  if (CurrentBlk(EI))
    {//data
    int p=EI.PageNo;
    int i=(int)(EI.BlkRowNo-EI.Index);
    switch (EI.FieldId)
      {
      case Id_XInt :
      case Id_XIntRng :
        {
        if (i<0)
          {
          }
        else
          {
          int d=Length()-1-i;
          PC.GetRangeLbl(d, Str, fSzAscend, EI.FieldId==Id_XIntRng);
          }
        EI.Fld->fEditable=PC.UserSizePts();
        break;
        }
      default:
        if (EI.FieldId>=Id_YSelection && EI.FieldId<Id_YSelection+MaxColumns)
          {
          const int s=Length()-1-(EI.FieldId-Id_YSelection);
          PCG.YFmt.FormatFloat(PCG.YCnv.Human(PC.SB[s][s]), Str);
          }
        else if (EI.FieldId>=Id_XIntRange && EI.FieldId<Id_XIntRange+MaxColumns)
          {
          const int d=Length()-1-(EI.FieldId-Id_XIntRange);
          PC.GetRangeLbl(d, Str, fSzAscend, true);
          EI.Fld->fEditable=false;
          }
        else if (EI.FieldId>=Id_YData && EI.FieldId<Id_YData+Length())
          {
          const int d=Length()-1-i;
          const int s=Length()-1-(EI.FieldId-Id_YData);
          PCG.YFmt.FormatFloat(PCG.YCnv.Human(PC.SB[s][d]), Str);
          }
      }
    }
  }

//---------------------------------------------------------------------------

long SzSelBrk1Edt::Parse(FxdEdtInfo &EI, Strng & Str)
  {
  long Fix=0; //set Fix=1 to redraw graph
  
  if (CurrentBlk(EI))
    {//header
    //bObjModified=1;
    int p=EI.PageNo;
    int i=(int)(EI.BlkRowNo-EI.Index);
    switch (EI.FieldId)
      {
      case Id_PartLen:
        PC.SetLength(Range(3L,SafeAtoL(Str,5),100L));
        View().DoRebuild();
        break;
      case Id_PartSizeDefn:
        {
        int i;
        for (i=SD_Defn.NIntervals()-1; i>=0; i--)
          if (Str.XStrICmp(SD_Defn.GetIntervals(i)->Name())==0)
            break;
        PC.SetSizeDefn(i); // if not found will become Manual (-1)
        View().DoRebuild();
        }
        break;
      case Id_ApplyToDist:
        {
        int i;
        for (i=SD_Defn.NDistributions()-1; i>=0; i--)
          if (Str.XStrICmp(SD_Defn.GetDist(i)->Name())==0)
            break;
        PC.SetApplyToDist(i);
        View().DoRebuild();
        }
        break;
      }
    }
  
  if (CurrentBlk(EI))
    {//data
    int p=EI.PageNo;
    int i=(int)(EI.BlkRowNo-EI.Index);
    switch (EI.FieldId)
      {
      case Id_XInt :  
        PC.SizePts()[i]=PCG.XCnv.Normal(SafeAtoF(Str, 0.0)); 
        break;
      case Id_XIntRng :  
        PC.SizePts()[i]=PCG.XCnv.Normal(SafeAtoF(Str, 0.0));
        break;
      default:
        if (EI.FieldId>=Id_YData && EI.FieldId<Id_YData+Length())
          {
          int d=Length()-1-i;
          int s=Length()-1-(EI.FieldId-Id_YData);//int s=EI.FieldId-Id_YData;
          PC.SB[s][d]=Range(0.0, PCG.YCnv.Normal(SafeAtoF(Str, 0.0)), 1.0);
          PC.CheckSB();
          View().DoReload();
          }
        else if (EI.FieldId>=Id_YSelection && EI.FieldId<Id_YSelection+Length())
          {
          int s=Length()-1-(EI.FieldId-Id_YSelection);
          PC.SB[s][s]=Range(0.0, PCG.YCnv.Normal(SafeAtoF(Str, 0.0)), 1.0);
          //PC.CheckSB();
          //View().DoReload();
          }

      }
    }

  return Fix;
  }

//---------------------------------------------------------------------------

long SzSelBrk1Edt::ButtonPushed(FxdEdtInfo &EI, Strng & Str)
  {
  long Fix=0; //set Fix=1 to redraw graph
  if (CurrentBlk(EI))
    {//header
    int p=EI.PageNo;
    int i=(int)(EI.BlkRowNo-EI.Index);
//    switch (EI.FieldId)
//      {
//      //case Id_DispRetained:
//      //  SetDispRetained(!DispRetained());
//      //  View().DoRebuild();
//      //  Fix=1;
//      //  break;
//      case Id_AutoScale:
//        SetAutoScale(!AutoScale());
//        SetRanges();
//        View().DoRebuild();
//        Fix=1;
//        break;
//      case Id_XLog:
//        SetXLog(!XLog());
//        SetRanges();
//        View().DoRebuild();
//        Fix=1;
//        break;
//      }
    }

  if (CurrentBlk(EI))
    {//data
    int p=EI.PageNo;
    int i=(int)(EI.BlkRowNo-EI.Index);
//    switch (EI.FieldId)
//      {
//      case Id_PartStepped:
//        SetPartStepped(!PartStepped());
//        SetRanges();
//        //bShowOther = !bShowOther;
//        View().DoRebuild();
//        Fix=1;
//        break;
//      case Id_Cumulative:
//        SetCumulativeOn(!CumulativeOn());
//        SetRanges();
//        //bShowOther = !bShowOther;
//        View().DoRebuild();
//        Fix=1;
//        break;
//      case Id_Fractional:
//        SetFractionalOn(!FractionalOn());
//        SetRanges();
//        //bShowOther = !bShowOther;
//        View().DoRebuild();
//        Fix=1;
//        break;
//      }
    }
//  // Base Blk
//  if (CurrentBlk(EI))
//    {
//    int p=EI.PageNo;
//    int i=(int)(EI.BlkRowNo-EI.Index);
//    switch (EI.FieldId)
//      {
//      case Id_GrOn    :  
//        iGraphOn=!iGraphOn; 
//        View().DoRebuild();
//        Fix=1;
//        break;
//      }
//    }
  return Fix;
  }

//---------------------------------------------------------------------------

flag SzSelBrk1Edt::DoLButtonDown(UINT nFlags, CPoint point)
  {
//  if (PtInRect(&GraphR, point))
//    {
//    if (nFlags & MK_SHIFT)
//      iDragPtNo = ClosestPt(point);
//    return 1;
//    }
  return 0;
  }

//---------------------------------------------------------------------------

flag SzSelBrk1Edt::DoLButtonUp(UINT nFlags, CPoint point)
  {
//  if (PtInRect(&GraphR, point))
//    {
//    if (iDragPtNo>=0)
//      {
//      View().DoReload();
//      iDragPtNo = -1;
//      }
//    return 1;
//    }
  return 0;
  }

//---------------------------------------------------------------------------

flag SzSelBrk1Edt::DoLButtonDblClk(UINT nFlags, CPoint point)
  {
//  if (PtInRect(&GraphR, point))
//    {
//    return 1;
//    }
  return 0;
  }

//---------------------------------------------------------------------------

flag SzSelBrk1Edt::DoRButtonDown(UINT nFlags, CPoint point)
  {
//  if (pView->CPgNo-iPg1>=0 && PtInRect(&GraphR, point))
//    {
//    return 1;
//    }
  return 0;
  }

//---------------------------------------------------------------------------

flag SzSelBrk1Edt::DoRButtonUp(UINT nFlags, CPoint point)
  {
//  if (pView->CPgNo-iPg1>=0 && PtInRect(&GraphR, point))
//    {
//    return 1;
//    }
  FxdEdtView &Vw=View();
  flag ret=false;//FxdEdtView::DoRButtonUp(nFlags, point);
  if (Vw.CPgNo>=0)
    {
    FxdEdtInfo EI;
    if (Vw.LocateFromCR((int)Vw.ChEditPos.x, (int)Vw.ChEditPos.y, EI))
      {
      ret=true;
      //if (EI.FieldId<0 && EI.Fld)
      if (EI.Fld)
        {
        int nCols=Length();
        if (EI.FieldId==Id_XInt ||
            EI.FieldId==Id_XIntRng ||
            EI.FieldId>=Id_XIntRange && EI.FieldId<Id_XIntRange+nCols ||
            EI.FieldId>=Id_YData && EI.FieldId<Id_YData+nCols ||
            EI.FieldId>=Id_YSelection && EI.FieldId<Id_YSelection+nCols)
          {
          CRect WRect;
          Vw.GetWindowRect(&WRect);
          CPoint  RBPoint;
          RBPoint.x = WRect.left+point.x;
          RBPoint.y = WRect.top+point.y;
          if ( (EI.FieldId>=Id_YMode && EI.FieldId<Id_YMode+nCols) )
            {
            //pWrkFmt=NULL;
            //pWrkCnv=NULL;
            WrkIB.Set(EI.Fld->Tag);
            }
          else if (EI.FieldId==Id_XInt ||
              EI.FieldId==Id_XIntRng ||
              EI.FieldId>=Id_XIntRange && EI.FieldId<Id_XIntRange+nCols)
            {
            //pWrkFmt=&PCG.XFmt;
            //pWrkCnv=&PCG.XCnv;
            WrkIB.Set(EI.Fld->Tag, &PCG.XCnv, &PCG.XFmt);
            }
          else
            {
            //int c=(EI.FieldId-Id_YMin)%MaxColumns;
            //CSD_Column &C=PC.Crvs[c];
            //pWrkFmt=C.pFmt;
            //pWrkCnv=C.pCnv;
            //pWrkFmt=&PCG.YFmt;
            //pWrkCnv=&PCG.YCnv;
            WrkIB.Set(EI.Fld->Tag, &PCG.YCnv, &PCG.YFmt);
            }

          CMenu Menu;
          Menu.CreatePopupMenu();
          CMenu FormatMenu;
          FormatMenu.CreatePopupMenu();
          if (WrkIB.FmtOK())
            {
            WrkIB.Fmt().AddToMenu(FormatMenu);
            CMenu CnvMenu;
            CnvMenu.CreatePopupMenu();
            WrkIB.Cnv().AddToMenu(CnvMenu);

            Menu.AppendMenu(MF_POPUP, (unsigned int)CnvMenu.m_hMenu, "&Conversions");
            if (WrkIB.Cnv().Index()<=0)// || !IsFloatData(d.iType))
              Menu.EnableMenuItem(0, MF_BYPOSITION|MF_GRAYED);

            Menu.AppendMenu(MF_POPUP, (unsigned int)FormatMenu.m_hMenu, "&Format");
            Menu.AppendMenu(MF_STRING, pView->iSendToTrend, "Send To &Trend");
            Menu.AppendMenu(MF_STRING, pView->iSendToQuickView, "Send To &QuickView");
            Menu.AppendMenu(MF_STRING|MF_GRAYED, pView->iSendToQuickView, "Send To &Custom");

            Menu.AppendMenu(MF_SEPARATOR);
            }
          
          Menu.AppendMenu(MF_STRING, pView->iCmdCopyTag, "Copy Full &Tag");
          if (EI.Fld->Tag==NULL)
            Menu.EnableMenuItem(pView->iCmdCopyTag, MF_BYCOMMAND|MF_GRAYED);
          Menu.AppendMenu(MF_STRING, pView->iCmdCopyVal, "Copy &Value");
          

          Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, RBPoint.x, RBPoint.y, /*(CWnd*)*/&View());
          Menu.DestroyMenu();                                           
          }
        }
      }
    }
  return ret;
  //return 0;
  }

//---------------------------------------------------------------------------

flag SzSelBrk1Edt::DoRButtonDblClk(UINT nFlags, CPoint point)
  {
//  if (pView->CPgNo-iPg1>=0 && PtInRect(&GraphR, point))
//    {
//    return 1;
//    }
  return 0;
  }

//---------------------------------------------------------------------------

flag SzSelBrk1Edt::DoMouseMove(UINT nFlags, CPoint point)
  {
//  if (GraphOn() && PtInRect(&GraphR, point))
//    {
//    double x,y;
//    PointtoWP(point, x, y);
//    char Buff[256], Buff2[256];
//    iNameWidth=4;
//    PCG.XFmt.FormatFloat(PCG.XCnv.Human(x), Buff2, sizeof(Buff2));
//    sprintf(Buff, "%*s:%s", iNameWidth, "Size", Buff2);
//    pLabels->SetText(0, Buff);
//    pLabels->Show();
//    pLabels->Invalidate();
//    if (nFlags & MK_LBUTTON)
//      {
//      if (nFlags & MK_SHIFT)
//        {
//        if (iDragPtNo>=0)
//          {
//          //rSD.MovePt(iDragPtNo, x, y);
//          //rSD.bObjModified = 1;
//          }
//        }
//      else
//        iDragPtNo = -1;
//      Wnd(pView).InvalidateRect(&GraphR);
//      }
//    return 1;
//    }
//  else
//    {
//    pLabels->Hide();
//    }
  return 0;
  }

//---------------------------------------------------------------------------

flag SzSelBrk1Edt::DoAccCnv(UINT Id) 
  {
  pCDataCnv pC=Cnvs[WrkIB.Cnv().Index()];
  for (int i=Id; i>0; i--)
    pC=pC->Next();
    
  if (pC)
    {
    WrkIB.Cnv().SetText(pC->Txt());
    }
  View().DoRebuild();
  return true;
  };

//---------------------------------------------------------------------------

flag SzSelBrk1Edt::DoAccFmt(UINT Id) 
  {
  for (UINT i=0; i<(UINT)DefinedFmts.GetSize(); i++) 
    if (i==Id) 
      break;

  if (i<(UINT)DefinedFmts.GetSize())
    {
    WrkIB.Fmt()=DefinedFmts[i];
    View().DoRebuild();
    }
  return true;
  };

//---------------------------------------------------------------------------

flag SzSelBrk1Edt::DoAccRptTagLists()
  {
  return FxdEdtBookRef::DoAccRptTagLists();
  //TagInfoBlk * IB = new TagInfoBlk(WrkIB.ObjClassId(), WrkIB.RefTag(), 
  //                                 WrkIB.CnvOK() ? &WrkIB.Cnv() : NULL, 
  //                                 WrkIB.FmtOK() ? &WrkIB.Fmt() : NULL);
  //ScdMainWnd()->PostMessage(WMU_ADDTOQUICKVIEW, 0, (long)(IB));
  //
  //return true;
  };

#endif
