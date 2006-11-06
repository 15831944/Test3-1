//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define  __FLWNETS_CPP
#include "sp_cont.h"
#include ".\sparseslv\sparseslv.h"
#include "flwnets.h"

#include "errorlog.h"
#include "dbgmngr.h"
#include "scd_wm.h"
#include "nrecipe.h"

//#include "optoff.h"

const double MinFxdQmDiff          = 1.0e-10;
const double ArrayReAnalyseRatio   = 100.0;
const double MinimumResistance     = 1.0e-40;
const double NetMaxResRatioInit    = 1.0e10;

const int    MaxBuildMatrixPasses  = 4;
const int    MaxNetSolvePasses     = 10;

const double MaxdQMult             = 4.0;
const int    ConvergeModulo        = 3;
const double SignificanceRatio     = 1.0e-12; // 12 digits

static int WithRhoHSens=0;


#define dbgFlwNets  (1 || WITHDEBUG)

#if (dbgFlwNets)

#define dbgSubNetsLong                0     

static CDbgMngr dbgAdjustRes                ("FlwNets", "AdjustRes");
static CDbgMngr dbgApplyDQ                  ("FlwNets", "ApplyDQ");
static CDbgMngr dbgAutoDbgBrkOnCvgFail      ("FlwNets", "AutoDbgBrkOnCvgFail");
static CDbgMngr dbgCalcDQ                   ("FlwNets", "CalcDQ");
static CDbgMngr dbgCnvFilter                ("FlwNets", "CnvFilter");
static CDbgMngr dbgConvergeNoDerivs         ("FlwNets", "ConvergeNoDerivs");
static CDbgMngr dbgDisableUDFs              ("FlwNets", "DisableUDFs");
static CDbgMngr dbgDumpFlowChgs             ("FlwNets", "Dump-FlowChgs");
static CDbgMngr dbgDumpPressBlks            ("FlwNets", "Dump-PressBlks");
static CDbgMngr dbgFindConns                ("FlwNets", "Find.Conns");
static CDbgMngr dbgFindGFBs                 ("FlwNets", "Find.GFBs");
static CDbgMngr dbgFindFBs                  ("FlwNets", "Find.FBs");
static CDbgMngr dbgGFBSummary               ("FlwNets", "GFBSummary");
static CDbgMngr dbgGFmt                     ("FlwNets", "GFmt");
static CDbgMngr dbgJoinConnects             ("FlwNets", "JoinConnects");
static CDbgMngr dbgMtxCalcIndices           ("FlwNets", "Mtx.CalcIndices");
static CDbgMngr dbgMtxDbgBrk                ("FlwNets", "Mtx.DbgBrk");
static CDbgMngr dbgMtxFastConverge          ("FlwNets", "Mtx.FastConverge");
static CDbgMngr dbgMtxForceBuild            ("FlwNets", "Mtx.ForceBuild");
static CDbgMngr dbgMtxGetSoln               ("FlwNets", "Mtx.GetSoln");
static CDbgMngr dbgMtxGetSolnChg            ("FlwNets", "Mtx.GetSolnChg");
static CDbgMngr dbgMtxInsert                ("FlwNets", "Mtx.Insert");
static CDbgMngr dbgMtxRowPrint              ("FlwNets", "Mtx.RowPrint");
static CDbgMngr dbgNetConverge              ("FlwNets", "Net.Converge");
static CDbgMngr dbgNetSolve                 ("FlwNets", "Net.Solve");
static CDbgMngr dbgPartialStaticHead        ("FlwNets", "PartialStaticHead");
static CDbgMngr dbgProdFracs                ("FlwNets", "ProdFracs");
static CDbgMngr dbgRegulators               ("FlwNets", "Regulators");
static CDbgMngr dbgSetEstP                  ("FlwNets", "SetEstP");
static CDbgMngr dbgShowQmSet                ("FlwNets", "ShowQmSet");
static CDbgMngr dbgShowBad                  ("FlwNets", "ShowBad");
static CDbgMngr dbgSolveFxdPs               ("FlwNets", "Solve.Fxd.Ps");
static CDbgMngr dbgSolveFxdPsXtra           ("FlwNets", "Solve.Fxd.PsXtra");
static CDbgMngr dbgSolveFxdQs               ("FlwNets", "Solve.Fxd.Qs");
static CDbgMngr dbgSolveFxdQsTrv            ("FlwNets", "Solve.Fxd.QsTrv");
static CDbgMngr dbgSolveIsoNode             ("FlwNets", "Solve.IsoNode");
static CDbgMngr dbgSubNets                  ("FlwNets", "SubNets");
static CDbgMngr dbgSubNetsSolve             ("FlwNets", "SubNetsSolve");
//static CDbgMngr dbgThreads                  ("FlwNets", "Threads");
static CDbgMngr dbgThreadsJob               ("FlwNets", "Threads.Job");
static CDbgMngr dbgThreadsSleep             ("FlwNets", "Threads.Sleep");
static CDbgMngr dbgTopologyChg              ("FlwNets", "TopologyChg");
static CDbgMngr dbgTraverseAll              ("FlwNets", "Traverse.All");
static CDbgMngr dbgTraverseDPa              ("FlwNets", "Traverse.DPa");
static CDbgMngr dbgTraverseDPb              ("FlwNets", "Traverse.DPb");
static CDbgMngr dbgTraverseDPdQ             ("FlwNets", "Traverse.DPdQ");
static CDbgMngr dbgTraverseDPz              ("FlwNets", "Traverse.DPz");
static CDbgMngr dbgTraverseDbgBrk           ("FlwNets", "Traverse.DbgBrk");
static CDbgMngr dbgTraverseDbgBrkAllInSubNet("FlwNets", "Traverse.DbgBrkAllInSubNet");
static CDbgMngr dbgTraverseDistError        ("FlwNets", "Traverse.DistError");
static CDbgMngr dbgTraverseDeltas           ("FlwNets", "Traverse.Deltas");
static CDbgMngr dbgTraverseFBNos            ("FlwNets", "Traverse.FBNos");
static CDbgMngr dbgTraverseFxdOrder         ("FlwNets", "Traverse.FxdOrder");
static CDbgMngr dbgTraverseGetMode          ("FlwNets", "Traverse.GetMode");
static CDbgMngr dbgTraverseP                ("FlwNets", "Traverse.P");
static CDbgMngr dbgTraversePBNos            ("FlwNets", "Traverse.PBNos");
static CDbgMngr dbgTraversePBlks            ("FlwNets", "Traverse.PBlks");
static CDbgMngr dbgTraversePFxd             ("FlwNets", "Traverse.PFxd");
static CDbgMngr dbgTraversePhys             ("FlwNets", "Traverse.Phys");
static CDbgMngr dbgTraverseProps            ("FlwNets", "Traverse.Props");
static CDbgMngr dbgTraverseQ                ("FlwNets", "Traverse.Q");
static CDbgMngr dbgTraverseResult           ("FlwNets", "Traverse.Result");
static CDbgMngr dbgTraverseSeq              ("FlwNets", "Traverse.Seq");
static CDbgMngr dbgTraverseShowFlags        ("FlwNets", "Traverse.ShowFlags");

static CDbgMngr dbgWhere                    ("FlwNets", "Where");
static CDbgMngr dbgWideFmt                  ("FlwNets", "WideFmt");

static CDbgMngr dbgRunStats                 ("FlwSolve", "RunStats");

//===========================================================================

#if (dbgFlwNets)
static CDbgMngr dbgThreadsJobTiming      ("FlwSolve", "Threads-JobTiming");
#endif

#define dbgrowprint                   0
#define dbgConvergeSubNet             0
#define dbgChokePt                    0
#define dbgSSolve                     0

static double dbgMtxGetSolnChgTol=1.0e-4;

#endif

static int FindConnsDeep=0;

//==========================================================================

#define DoArrayDamping 0
#if DoArrayDamping
const double ArrayDamping          = 0.0;
#endif

const byte NLineCnvCnt             = 5;
const byte LineCnvCnt[NLineCnvCnt] = {32,32,64,64,96};

//==========================================================================
//
//
//
//==========================================================================
#if (dbgFlwNets)

#define DBG_TraverseQ            0x00000001
#define DBG_TraverseP            0x00000002
#define DBG_TraversePFxd         0x00000004
#define DBG_CalcdQ               0x00000008
#define DBG_TrvErrors            0x00000010
#define DBG_ApplydQ              0x00000020
#define DBG_Derivs               0x00000040
#define DBG_TrvResult            0x00000080
#define DBG_Deltas               0x00000100
#define DBG_Props                0x00000200
#define DBG_Phys                 0x00000400
#define DBG_DPa                  0x00000800
#define DBG_DPb                  0x00001000
#define DBG_DPz                  0x00002000
#define DBG_DPdQ                 0x00004000
#define DBG_SetEstP              0x00008000
#define DBG_Regulator            0x00010000
#define DBG_TrvGetMode           0x00040000
#define DBG_TraverseFBNos        0x00080000
#define DBG_TraversePBNos        0x00100000
#define DBG_TraverseFxdOrder     0x00200000

#define DBG_Traverse             (DBG_TraverseP | DBG_TraverseQ | DBG_TrvGetMode)

static dword DbgFlags  = 0;

static int WithGFmt   = 0;
#endif

static int QWide      = 11;
static int QPrec      = 4;
static int PWide      = 16;
static int PPrec      = 7;
static int RWide      = 16;
static int RPrec      = 7;
static int MWide      = 16;
static int MPrec      = 7;
static int DWide      = 16;
static int DPrec      = 7;

static void SolveDbgFlags(flag HasDoDbgBrk)
  {
#if (dbgFlwNets)
  DbgFlags = 0;
  if (!dbgtestfileopen())
    return;

  WithGFmt = dbgGFmt();

  if (dbgWideFmt())
    {
    QWide = 17;
    QPrec = 9;
    PWide = 16;
    PPrec = 8;
    RWide = 16;
    RPrec = 8;
    MWide = 16;
    MPrec = 8;
    DWide = 16;
    DPrec = 8;
    }
  else
    {
    QWide = 12;
    QPrec = 4;
    PWide = 12;
    PPrec = 4;
    RWide = 12;
    RPrec = 4;
    MWide = 12;
    MPrec = 4;
    DWide = 12;
    DPrec = 4;
    }

  if (dbgTraverseDbgBrk() && !HasDoDbgBrk)
    return;

  if (dbgCalcDQ())
    DbgFlags |= DBG_CalcdQ;
  if (dbgApplyDQ())
    DbgFlags |= DBG_CalcdQ | DBG_ApplydQ;
  if (dbgTraverseGetMode())
    DbgFlags |= DBG_TrvGetMode;
  if (dbgTraverseResult())
    DbgFlags |= DBG_TrvResult;
  if (dbgTraverseAll())
    DbgFlags |= DBG_TrvErrors;
  if (dbgTraverseP())
    DbgFlags |= DBG_TraverseP;
  if (dbgTraverseFBNos())
    DbgFlags |= DBG_TraverseFBNos;
  if (dbgTraverseFxdOrder())
    DbgFlags |= DBG_TraverseFxdOrder;
  if (dbgTraversePBNos())
    DbgFlags |= DBG_TraversePBNos;
  if (dbgTraversePFxd())
    DbgFlags |= DBG_TraversePFxd;
  if (dbgTraverseQ())
    DbgFlags |= DBG_TraverseQ;
  if (dbgTraverseDeltas())
    DbgFlags |= DBG_Deltas;
  if (dbgTraverseProps())
    DbgFlags |= DBG_Props;
  if (dbgTraversePhys())
    DbgFlags |= DBG_Phys;
  if (dbgRegulators())
    DbgFlags |= DBG_Regulator;
  if (dbgTraverseDPa())
    DbgFlags |= DBG_DPa;
  if (dbgTraverseDPb())
    DbgFlags |= DBG_DPb;
  if (dbgTraverseDPz())
    DbgFlags |= DBG_DPz;
  if (dbgTraverseDPdQ())
    DbgFlags |= DBG_DPdQ;
  if (dbgSetEstP())
    DbgFlags |= DBG_SetEstP;
#endif

  }

//==========================================================================

inline int TstChg(double A, double B, double Tol, double &Frac)
  {
  Frac=(A-B)/NZ(Max(fabs(A), fabs(B)));
  return fabs(Frac) > Tol;
  }

//==========================================================================
//
// Class to Handle Unidirectional Flow in pipes
//
//==========================================================================

int UDFRec::MaxActionCount  = 2;
double UDFRec::RestrictMult = 10.0;

//==========================================================================
//
//
//
//==========================================================================

double FlwBlk::RMin=1.0e-3;//110;

FlwBlk::FlwBlk()
  {
  m_fOvrSpecd=false;
  m_fWasOvrSpecd=false;
  m_fFixedPrev=false;
  m_fXferPrev=false;
  m_Qm=0.0;
  m_QmEst=0.0;

  FlwBlk::ResetData(true);
  };

//--------------------------------------------------------------------------

FlwBlk::~FlwBlk()
  {
  };

//--------------------------------------------------------------------------

void FlwBlk::ResetData(flag Complete)
  {
  CFlwBlkBase::ResetData(Complete);

  m_fFixedPrev    = false;
  m_fXferPrev     = false;
  m_QmRqd         = dNAN;
  m_QmReg         = 1.0;
  m_fFixedVlv     = false;
  m_fFixed        = false;
  m_fIsDegFree    = false;
  m_QmXfCap       = dNAN;
  m_fXfer         = false;
  m_fInXferNet    = False;
  m_iRegulator    = FBReg_Off;

  m_pQmEstSrc    = NULL;

  m_iFxdPOrder    = -1;
  PrevDirn        = 0;
  m_pGFb          = NULL;
  pChoke          = NULL;
  QmRatio.Mult    = 1.0;
  QmRatio.MFb     = NULL;

  pNetTopoChg     = NULL;
  pSubNetTopoChg  = NULL;
  pChoke          = NULL;

  UDF.OpenP       = 1.0;
  UDF.CloseP      = 0.0;
  UDF.DirnAllowed = UDFD_Both;
  UDF.Action      = UDFA_None;
  UDF.Restrict    = 0.0;
  UDF.ActionCount = 0;
  UDF.fInList     = 0;
  UDF.IterCount   = 0;
#ifdef _DEBUG
  nMaxTraverse    =  0;
  nAvgTraverse    =  0.0f;
#endif
  m_DPEstFxdQm    = 0.0;
#if (0||dbgTrackQmModes)
  dbgpln("%-20s fb%04i", "ResetData", m_iNo);
#endif
  }

//-------------------------------------------------------------------------

void FlwBlk::GetFlags(CGrpFlwBlk * pGFB, int FBDirn)
  {
  CGrpFlwBlk &GFb=*pGFB;

  pGFB->m_iPropsSelected = Max(pGFB->m_iPropsSelected, m_PropSel.m_iReqd); 

  GFb.m_fDerivsBad     = GFb.m_fDerivsBad     || m_fDerivsBad;
  GFb.m_fFunctOfPress  = GFb.m_fFunctOfPress  || m_fFunctOfPress;
  GFb.m_fMomentumValid = GFb.m_fMomentumValid || m_fMomentumValid;
  GFb.m_fHoldFastConverge = GFb.m_fHoldFastConverge || m_fHoldFastConverge;

  if (FBDirn>0)
    {
    GFb.m_fEvalFwdOnly   = GFb.m_fEvalFwdOnly || m_fEvalFwdOnly;
    GFb.m_fEvalRevOnly   = GFb.m_fEvalRevOnly || m_fEvalRevOnly;
    }
  else
    {
    GFb.m_fEvalFwdOnly   = GFb.m_fEvalFwdOnly || m_fEvalRevOnly;
    GFb.m_fEvalRevOnly   = GFb.m_fEvalRevOnly || m_fEvalFwdOnly;
    }
  GFb.m_fSetCI13=false;
  if (GFb.m_fEvalFwdOnly && GFb.m_fEvalRevOnly)
    {
    SetQmReqd(0.0);
    GFb.m_fSetCI13=true;
    }
  }


static LPCTSTR dbgIODesc(FlwNode * pNd, int IONo, int FE, int LnkNo)
  {
  static CString S;
  if (LnkNo>=0)
    S.Format("%20s[Ln %i  ]", pNd->FullObjTag(), LnkNo);
  else if (IONo>=0)
    S.Format("%20s[Io %i.%i]", pNd->FullObjTag(), IONo, FE);
  else
    S.Format("%20s[??    ]", pNd->FullObjTag());
  return S;
  };

//-------------------------------------------------------------------------


void FlwBlk::EvalFlow(eScdFlwEqnTasks Task, CGrpFlwBlk * pGFB, int FBDirn, int PassDirn, CFlwBlkIndex & FBI, CSpPropInfo * pNetProps, double DestP, LPCTSTR dbgChar)
  {
  CGrpFlwBlk &GFb=*pGFB;

  m_iDirnRel2Traverse=(sint)FBDirn;

  flag MeasPAtEnd=false;

  switch (Task)
    {
    case FET_GetMode:
      {
      if (UDF.ActionCount>0)
        {
        m_dPq   += -fabs(m_Qm)*UDF.Restrict;
        m_dPqdQ += -m_sQMeas*UDF.Restrict;
        if (UDF.ActionCount==UDF.MaxActionCount)
          {
          GFb.m_fUDFShut=true;
          SetQmReqd(0.0);
          }
        else
          SetQmFree();
        }

      if (m_fFixedPrev!=m_fFixed)
        {
        if (GFb.m_fHasDbgBrk)
          { int xxx=0; }
        m_fFixedPrev=m_fFixed;
        GFb.m_fTopologyChanged=true;
        }
      if (m_fXferPrev!=m_fXfer)
        {
        m_fXferPrev=m_fXfer;
        GFb.m_fTopologyChanged=true;
        }

      GetFlags(pGFB, FBDirn);

      m_fOvrSpecd=false;
      if (m_fFixed) // Includes fXfer
        {
        GFb.m_fHasDegFree = GFb.m_fHasDegFree || m_fIsDegFree;
        double &GXf_QmCap=GFb.m_QmXfCap;
        double &GFb_QmRqd=GFb.m_QmRqd;
        double &GFb_QmReg=GFb.m_QmReg;
        double Lnk_QmRqd=dNAN;
        flag ThisFBLimiting=false;
        if (m_fXfer || GFb.m_fInXferNet || GFb.m_fFixedXfer)
          {
          if (Valid(m_QmXfCap))
            {
            Lnk_QmRqd=FBDirn*m_QmXfCap;//*PassDirn;
            if (Valid(GXf_QmCap))
              {
              double GXf_QmCap_Mem=GXf_QmCap;
              if (GXf_QmCap*Lnk_QmRqd<0.0)
                m_fOvrSpecd=true;
              if (Lnk_QmRqd>=0.0)
                GXf_QmCap=Range(0.0, GXf_QmCap, Lnk_QmRqd);
              else
                GXf_QmCap=Range(Lnk_QmRqd, GXf_QmCap, 0.0);
              ThisFBLimiting=(GXf_QmCap_Mem!=GXf_QmCap);
              }
            else
              {
              GXf_QmCap=Range(-MaxXfCap ,Lnk_QmRqd, MaxXfCap);
              ThisFBLimiting=true;
              }
            }
          if (m_fOvrSpecd != m_fWasOvrSpecd)
            GFb.m_pNet->nOverSpecChgs++;
          if (m_fOvrSpecd)
            {
            GXf_QmCap=0.0;
            ThisFBLimiting=true;
            }
          GFb.m_fFixedXfer=true;
          }
        else
          {
          if (m_fFixedVlv)
            {
            Lnk_QmRqd=FBDirn*m_QmRqd;
            GFb_QmReg*=m_QmReg;
            GFb.m_nFixedVlv++;
            }
          else
            {
            Lnk_QmRqd=FBDirn*m_QmRqd;
            if (GFb.m_fFixed && Valid(GFb_QmRqd))
              {
              if (fabs(Lnk_QmRqd)>MinFxdQmDiff && fabs(GFb_QmRqd)>MinFxdQmDiff)
                {
                if ((GFb.m_nFixedRqd>0) && (Lnk_QmRqd*GFb_QmRqd<0)) // diffent directions
                  m_fOvrSpecd=true;
                }
              }                
            GFb.m_nFixedRqd++;
            }

          if (m_fOvrSpecd != m_fWasOvrSpecd)
            GFb.m_pNet->nOverSpecChgs++;
          if (m_fOvrSpecd)
            Lnk_QmRqd=0.0;
          if (!Valid(GFb_QmRqd))
            {
            GFb_QmRqd=Lnk_QmRqd;
            ThisFBLimiting=true;
            }
          else if (fabs(Lnk_QmRqd)<fabs(GFb_QmRqd))
            {
            if (m_fFixedVlv)
              GFb_QmRqd=Sign(GFb_QmRqd)*fabs(Lnk_QmRqd);
            else 
              GFb_QmRqd=Lnk_QmRqd;
            ThisFBLimiting=true;
            }
          else
            { // get sign correct
            if (!m_fFixedVlv)
              GFb_QmRqd=Sign(Lnk_QmRqd)*fabs(GFb_QmRqd);
            }
          if (!Valid(GFb.m_QmRqd))
            LogError("Solver", 0, "Internal Error - FlwBlk::EvalFlow - Bad dQmReqd (404)");
          }

        GFb.m_fFixed=true;
        m_fWasOvrSpecd=m_fOvrSpecd;

        if (m_fIsDegFree && Valid(Lnk_QmRqd) && ThisFBLimiting)
          {
          GFb.m_XBst.m_pFB=this;
          GFb.m_XBst.m_dFB=FBDirn;
          }
        else //if (GFb.XBst.pFB!=this)
          {
          m_dPbX = 0.0;
          // dbgpln("1m_dPbX = 0  %s [%s>%s]", AttachedTo()->Tag(), pGFB->m_Src.m_pNd->Tag(), pGFB->m_Dst.m_pNd->Tag());
          }
        }

#if dbgFlwNets
      if (DbgFlags & DBG_TrvGetMode)
        {
        LPSTR EqnNm=Eqn() && Eqn()->FlwEqnName() ? Eqn()->FlwEqnName() : "";
        if (_stricmp(EqnNm, "None")==0)
          EqnNm=" ";

        dbglock();
        Strng FS;
        FS.Set(m_iFxdPOrder>=0? "%4i ":"        ", m_iFxdPOrder);
        dbgp("%s%s%s%s%s%s %s%s %-2.2s %3.3s",
          SgnStr(m_iDirnRel2Traverse),
          SgnStr(m_iDirnRel2Connect),
          (m_fFixed ? "F" : " "), 
          (DbgFlags & DBG_TraverseFxdOrder ? FS():""),
          m_fDerivsBad?"x":"d",
          FlowMode()==FBMode_Xfer?"X":FlowMode()==FBMode_Simple?"S":FlowMode()==FBMode_Linear?"L":FlowMode()==FBMode_Full?"F":"?",
          UDF.ActionCount==0?" ":UDF.ActionCount==1?"r":"C",
          " ", dbgChar, EqnNm);

        dbgp(fabs(m_Qm)<0.1 && WithGFmt?" %*.*g":" %*.*f", QWide,QPrec,Range(-99999.9999, m_Qm, 99999.9999));


        if (1)//DbgFlags & DBG_TraverseQ)
          {
          Strng S0, S1, S2, S3;
          S0.Set(Valid(GFb.m_QmXfCap) ? "%#*.*g" : "*",QWide,QPrec,GFb.m_QmXfCap);
          S2.Set(Valid(m_QmXfCap) ? "%#*.*g" : "*",QWide,QPrec,m_QmXfCap);
          S1.Set(Valid(GFb.m_QmRqd) ? "%#*.*g" : "*",QWide,QPrec,GFb.m_QmRqd);
          S3.Set(Valid(m_QmRqd) ? "%#*.*g" : "*",QWide,QPrec,m_QmRqd);
          dbgp(" %16s %16s %16s %16s %5.3f %5.3f", 
            S0(), S1(), S2(), S3(), GFb.m_QmReg, m_QmReg);
          }


        dbgpln(" %s", dbgIODesc(FBI.m_pNd, FBI.m_iIONo, FBI.m_iFENo, FBI.m_iLnkNo));
        dbgunlock();
        }
#endif
      break;
      }       
    case FET_SetQm:
    case FET_CalcDP:
      {
      if (Task==FET_SetQm)
        {
        m_dPbX   = 0.0;
        m_QmEst  = m_Qm;
        m_ResT   = 0;
        m_dPq    = 0;
        m_dPmQ   = 0;

        m_SimpleRes  = 0.0;
        }
      else
        {
        if (m_fFixed)
          {
          }
        else // Not Fixed Flow
          {
          if (QmRatio.MFb==NULL)
            {
            m_dPbX=0.0;
            // dbgpln("3m_dPbX = 0  %s [%s>%s]", AttachedTo()->Tag(), pGFB->m_Src.m_pNd->Tag(), pGFB->m_Dst.m_pNd->Tag());
            }
          }

        m_QmEst=m_Qm;
        double R1=0;
        double R2;
        double B;
        double DP;

        if (m_fInXferNet)
          R1  = 0.0;
        else if (FlowMode()==FBMode_Full)
          R1  = Max(MinimumResistance, fabs(m_dPq)/QmMeas());
        else if (FlowMode()==FBMode_Simple || FlowMode()==FBMode_Linear)
          {
          R1  = m_SimpleRes;
          m_dPqdQ  = m_SimpleRes; // this is needed for convergence
          }
        else
          ASSERT("Should Not Get Here - Bad R1");

        if (FlowMode()==FBMode_Full || FlowMode()==FBMode_Linear)
          {
          R2      = fabs(m_dPmQ)/GTZ(fabs(QmMeas()));
          }
        else
          {
          R2      = 0.0;
          m_dPa   = 0.0;
          m_dPz   = 0.0;
          m_dPzI  = 0.0;
          }

        m_ResTCalc = R1+R2;

        const int iStart=m_pGFb->m_pFNB->m_lResRunAvgStart;
        const int iLen=m_pGFb->m_pFNB->m_lResRunAvgCount;

        int iLoop=m_pGFb->m_pSubNet->iNetCnvLoop;
        
        if (iLoop<m_iNetLoop)
          {
          m_nResTHist=0;
          m_iResTIndex=-1;
          }
        else if (iLoop>=iStart && iLoop>m_iNetLoop)
          {
          m_nResTHist++;
          m_iResTIndex=(m_iResTIndex+1)%iLen;
          m_ResTHist.SetSize(iLen);
          }
        m_iNetLoop=iLoop;

        if (m_nResTHist>0)
          {
          m_ResTHist[m_iResTIndex]=m_ResTCalc;
          m_ResT  = 0.0;
          int n=Min(m_nResTHist, iLen);
          for (int j=0; j<n; j++)
            m_ResT += m_ResTHist[j];
          m_ResT /= j;

          if (0 && m_ResT>0.0)
            {
            dbgp("%3i %3i %12.4f %12.4f [", iLoop, m_nResTHist, m_ResT, m_ResTCalc);
            int n=Min(m_nResTHist, iLen);
            int jj=m_iResTIndex;
            for (int j=0; j<n; j++)
              {
              dbgp(" %12.4f", m_ResTHist[jj--]);
              if (jj<0)
                jj=iLen-1;
              }
            dbgpln("]");
            }
          int xxx=0;
          }
        else
          m_ResT  = m_ResTCalc;


        // Reconstruct dPq & dPmQ
        m_dPq   = -R1*m_Qm;
        m_dPmQ  = -R2*m_Qm;

        B       = m_dPa + m_dPb + m_dPbX + m_dPz + m_dPzI + m_dPmB;
        DP      = B + (m_dPq/*+dPqRes*/+m_dPmQ);

#ifndef _RELEASE
        static bool DoCheck = 0;
        if (DoCheck)
          ASSERT_ALWAYS(fabs(DP)<1.0e25, "Why is DP Wrong!"); //change DoCheck to 0 to stop assert
#endif
        if (fabs(DP)>1.0e25)
          LogNote(FBI.m_pNd->FullObjTag(),0, "Very high DP %.4g",DP); //change DoCheck to 0 to stop assert
        //kga : 2/5/2002 : added line below to try fix crash (caused by swap from probal to dynamic)
        DP = Range(-1.0e25, DP, 1.0e25);

        GFb.m_P_Here    += FBDirn*DP;
        GFb.m_P_HereLnk += FBDirn*DP;

        GFb.m_P_HereSafe = Max(LowPressure, GFb.m_P_Here);
        GFb.m_P_Lowest   = Min(GFb.m_P_Lowest, GFb.m_P_Here);

        GFb.m_Sum_dP     += FBDirn*DP;
        GFb.m_Sum_B      += FBDirn*B;
        GFb.m_Sum_ResT   += fabs(m_ResT);

        // Rate of Change is not a Function of Link Direction
        if (FlowMode()==FBMode_Full || FlowMode()==FBMode_Linear)
          {
          GFb.m_Sum_dPbdQ  += (m_dPadQ + m_dPbdQ + m_dPzdQ + /*dPzXdQ +*/ m_dPmdQ);
          GFb.m_Sum_adPqdQ += fabs(m_dPqdQ)/*+fabs(dPqResdQ)*/+fabs(m_dPmdQ);
          }
        else
          {
          GFb.m_Sum_dPbdQ  += (m_dPbdQ + /*dPzXdQ +*/ m_dPmdQ);
          GFb.m_Sum_adPqdQ += fabs(m_dPqdQ)/*+fabs(dPqResdQ)*/+fabs(m_dPmdQ);
          }

        if (m_iRegulator!=FBReg_Off && FlowMode()==FBMode_Full)
          {
          CGrpFlwRegBlk &R=GFb.m_Regulator;
          if (R.m_iWhat!=m_iRegulator)
            {
            if (GFb.m_pSubNet->m_RegGrps.Find(m_pGFb)==NULL)
              GFb.m_pSubNet->m_RegGrps.AddTail(m_pGFb);
            };

          R.m_iWhat=m_iRegulator;
          R.m_pNd = FBI.m_pNd;    
          R.m_pFB = this;    
          R.m_pFRB = FBI.m_pNd->GetFlwRegulator();    
          }

        ///GetFlags(pGFB, FBDirn);

        GFb.m_BstDamping=Max(GFb.m_BstDamping, dBstDamping);
        }

      if (0) // Do Checks
        {
        if (!Valid(m_Qm) || !Valid(m_ResTCalc) || !Valid(m_dPa) || !Valid(m_dPb) || !Valid(m_dPbX) || !Valid(m_dPz) || !Valid(m_dPzI) || !Valid(m_dPmB))
          LogError(dbgIODesc(FBI.m_pNd, FBI.m_iIONo, FBI.m_iFENo, FBI.m_iLnkNo), 0, 
        
          "Bad Field m_Qm:%.2f m_ResTCalc:%.2f m_dPa:%.2f m_dPb:%.2f m_dPbX:%.2f m_dPz:%.2f m_dPzI:%.2f m_dPmB:%.2f"
          , m_Qm, m_ResTCalc, m_dPa, m_dPb, m_dPbX, m_dPz, m_dPzI, m_dPmB);

        }
#if dbgFlwNets
      if (/*!GFb.m_fFixed &&*/ DbgFlags & DBG_TraverseP)
        {
        LPSTR EqnNm=Eqn() && Eqn()->FlwEqnName() ? Eqn()->FlwEqnName() : "";
        if (_stricmp(EqnNm, "None")==0)
          EqnNm=" ";
        dbglock();
        Strng FS;
        FS.Set(m_iFxdPOrder>=0? "%4i ":"        ", m_iFxdPOrder);
        if (DbgFlags & DBG_TraverseFBNos)
          dbgp("fb[%4i] ", m_iNo); 
        if (DbgFlags & DBG_TraversePBNos)
          dbgp("sp[%4i] dp[%4i] ", (FBDirn>0?SrcPB():DstPB()).m_iNo,(FBDirn<0?SrcPB():DstPB()).m_iNo); 
        dbgp("%s%s%s%s%s%s%s %s%s %-2.2s %3.3s",
          SgnStr(m_iDirnRel2Traverse),
          SgnStr(m_iDirnRel2Connect),
          (m_fFixed ? "F":" "),(m_fIsDegFree ? "f" : " "), 
          (DbgFlags & DBG_TraverseFxdOrder ? FS():""),
          m_fDerivsBad?"x":"d",
          FlowMode()==FBMode_Xfer?"X":FlowMode()==FBMode_Simple?"S":FlowMode()==FBMode_Linear?"L":FlowMode()==FBMode_Full?"F":"?",
          UDF.ActionCount==0?" ":UDF.ActionCount==1?"r":"C",
          " ", dbgChar, EqnNm);
        if (DbgFlags & DBG_Regulator)
          {
          if (m_iRegulator!=FBReg_Off && FlowMode()==FBMode_Full)
            dbgp(" %2s[%6.3f]", 
            m_iRegulator==FBReg_Pi? "Pi" :m_iRegulator==FBReg_Po ? "Po" : "Qm", 
            GFb.m_Regulator.m_dCtrlCur);
          else
            dbgp(" %2s %6s ", "", "");
          }

        dbgp(fabs(m_Qm)<0.1 && WithGFmt?" %*.*g":" %*.*f", QWide,QPrec,Range(-99999.9999, m_Qm, 99999.9999));
        dbgp(" %#*.*f",PWide,PPrec,Range(-9999999.9999, GFb.m_P_Here, 9999999.9999));

        if (0)
          dbgp(" %#*.*f %#*.*f",
          PWide,PPrec,Range(-9999999.9999, GFb.m_P_HereSafe, 9999999.9999),
          PWide,PPrec,Range(-9999999.9999, GFb.m_P_HereLnk, 9999999.9999)
          );
        if (0)
          dbgp(" %#11.2f %#11.2f",
          Range(-99999.99, FBDirn>0 ? FlngPB().P : NodePB().P, 99999.99),
          Range(-99999.99, FBDirn<0 ? FlngPB().P : NodePB().P, 99999.99)
          );

        if (DbgFlags & DBG_TraversePFxd)
          {
          dbgp(" %#*.*f",
            PWide,PPrec,Range(-99999.99, FBDirn>0 ? FlngPB().PGood : NodePB().PGood, 99999.99)
            );
          dbgp(" %#*.*f",
            PWide,PPrec,Range(-99999.99, FBDirn<0 ? FlngPB().PGood : NodePB().PGood, 99999.99)
            );
          }

        //    dbgp(fabs(dPq)<0.1 && WithGFmt?" +(%#*.*g":" +(%#*.*f%#*.*f", PWide,PPrec,FBDirn * dPq, PWide,PPrec,FBDirn * dPqRes);
        dbgp(fabs(m_dPq)<0.1 && WithGFmt?" +(%#*.*g":" +(%#*.*f", PWide,PPrec,Range(-999999.9, FBDirn * m_dPq, 999999.9));
        dbgp(fabs(m_ResT)<1e-3 && WithGFmt?"%#*.*g ":"%#*.*f ", RWide,RPrec,Range(-999999.9, m_ResT, 999999.9));
        if (DbgFlags&DBG_DPa)
          dbgp(fabs(m_dPa)<0.1 && WithGFmt?" %#*.*g":" %#*.*f", PWide,PPrec,FBDirn * m_dPa);
        if (DbgFlags&(DBG_DPb|DBG_DPz))
          dbgp(fabs(m_dPb + m_dPbX + m_dPz)<0.1 && WithGFmt?" %#*.*g":" %#*.*f",PWide,PPrec,FBDirn * (m_dPb + m_dPbX + m_dPz));
        if (DbgFlags&(DBG_DPdQ))
          dbgp(m_dPbdQ + m_dPqdQ<0.1 /*&& WithGFmt*/?" %#*.*g":" %#*.*f",PWide,PPrec,FBDirn * (m_dPbdQ + m_dPqdQ));
        dbgp("[");
        if (DbgFlags&DBG_DPb)
          dbgp(fabs(m_dPb)<0.1 && WithGFmt?"%#*.*g":"%#*.*f",PWide,PPrec,FBDirn * m_dPb);
        if (DbgFlags&DBG_DPb)
          dbgp(fabs(m_dPbX)<0.1 && WithGFmt?" %#*.*g":" %#*.*f",PWide,PPrec,FBDirn * m_dPbX);
        if (DbgFlags&DBG_DPz)
          dbgp(fabs(m_dPz)<0.1 && WithGFmt?" %#*.*g":" %#*.*f",PWide,PPrec,FBDirn * m_dPz);
        if (DbgFlags&DBG_DPz)
          dbgp(fabs(0.0)<0.1 && WithGFmt?" %#*.*g":" %#*.*f",PWide,PPrec,0.0);
        if (DbgFlags&DBG_DPz)
          dbgp(fabs(m_dPzI)<0.1 && WithGFmt?" %#*.*g":" %#*.*f", PWide,PPrec,FBDirn * m_dPzI);
        if (DbgFlags&DBG_DPdQ)
          dbgp(fabs(m_dPbdQ)<0.1 && WithGFmt?" %#*.*g":" %#*.*f", PWide,PPrec,FBDirn * m_dPbdQ);
        if (DbgFlags&DBG_DPdQ)
          dbgp(fabs(m_dPqdQ)<0.1 && WithGFmt?" %#*.*g":" %#*.*f", PWide,PPrec,FBDirn * m_dPqdQ);
        dbgp("])");

        if (DbgFlags&DBG_Props)
          {
          dbgp("%*.*f %*.*f %*.*g %*.*f %*.*f ", 
            DWide, DPrec, Properties(pNetProps)->m_dRhoL, 
            DWide, DPrec, Properties(pNetProps)->m_dRhoV, 
            DWide, DPrec, Properties(pNetProps)->m_dVisc, 
            DWide, DPrec, Properties(pNetProps)->m_dLVolFrac, 
            DWide, DPrec, Properties(pNetProps)->m_dVVolFrac);
          }

        if (fTwoPhase)//DoMultiPhase)
          {
          dbgp(Valid(SlipRatio()) ? "%#7.3f ":"        ", SlipRatio());
          dbgp(Valid(VoidFraction()) ? "%#7.3f ":"        ", VoidFraction());
          }

        if (DbgFlags&DBG_Phys)
          {
          if (Valid(Velocity()))
            dbgp(fabs(Velocity())<0.01?"%12.2g ":"%12.4f ", Velocity());
          else
            dbgp("%12s ", " ");
          dbgp("%8.2f ", PhD().TotLength());
          dbgp("%8.2f ", PhD().Diam());
          dbgp("%10.4f ", PhD().KFact());
          }

        dbgpln(" %s", dbgIODesc(FBI.m_pNd, FBI.m_iIONo, FBI.m_iFENo, FBI.m_iLnkNo));
        dbgunlock();
        }
      else if (GFb.m_fFixed && DbgFlags & DBG_TraverseQ)
        {
        Strng S0, S1, S2, S3;
        S0.Set(Valid(GFb.m_QmXfCap) ? "%#*.*g" : "*",QWide,QPrec,GFb.m_QmXfCap);
        S2.Set(Valid(m_QmXfCap) ? "%#*.*g" : "*",QWide,QPrec,m_QmXfCap);
        S1.Set(Valid(GFb.m_QmRqd) ? "%#*.*g" : "*",QWide,QPrec,GFb.m_QmRqd);
        S3.Set(Valid(m_QmRqd) ? "%#*.*g" : "*",QWide,QPrec,m_QmRqd);

        LPSTR EqnNm=Eqn() && Eqn()->FlwEqnName() ? Eqn()->FlwEqnName() : "";
        if (_stricmp(EqnNm, "None")==0)
          EqnNm=" ";
        Strng FS;
        FS.Set(m_iFxdPOrder>=0? "%4i ":"        ", m_iFxdPOrder);

        if (DbgFlags & DBG_TraverseFBNos)
          dbgp("fb[%4i] ", m_iNo); 
        if (DbgFlags & DBG_TraversePBNos)
          dbgp("sp[%4i] dp[%4i] ", (FBDirn>0?SrcPB():DstPB()).m_iNo,(FBDirn<0?SrcPB():DstPB()).m_iNo); 
        dbgp("%s%s%s%s%s  %s%s %-2.2s %3.3s",
          SgnStr(FBDirn),
          (m_fFixed ? m_fIsDegFree ? "F" : "f" : " "),
          (DbgFlags & DBG_TraverseFxdOrder ? FS():""), " ",
          " ", " ", " ", dbgChar, EqnNm);
        if (DbgFlags & DBG_Regulator)
          dbgp(" %2s %6s ", "", "");
        dbgpln(" %*.*f %*.*f %16s %16s %16s %16s %5.3f %5.3f %s",
          QWide,QPrec,Range(-99999.9999, m_Qm, 99999.9999),
          PWide,PPrec,Range(-99999.9999, GFb.m_P_Here, 99999.9999), S0(), S1(), S2(), S3(),
          GFb.m_QmReg, m_QmReg,
          dbgIODesc(FBI.m_pNd, FBI.m_iIONo, FBI.m_iFENo, FBI.m_iLnkNo)
          );
        }
#endif
      break;
      }
    }
  }

//-------------------------------------------------------------------------

void FlwBlk::EvalFlowX(eScdFlwEqnTasks Task, CGrpFlwBlk * pGFB, int FBDirn, int PassDirn, CFlwBlkIndex & FBI, CSpPropInfo * pNetProps, LPCTSTR dbgChar)
  {
  CGrpFlwBlk &GFb=*pGFB;

  m_iDirnRel2Traverse=(sint)FBDirn;

  switch (Task)
    {
    case FET_GetMode:
      {
#if dbgFlwNets
      if (DbgFlags & DBG_TrvGetMode)
        {
        Strng FS;
        FS.Set(m_iFxdPOrder>=0 ? "%4i ":"     ", m_iFxdPOrder);
        dbglock();
        dbgp("%s%s%s%s%s%s %s%s %-2.2s %3.3s",
          SgnStr(m_iDirnRel2Traverse),
          SgnStr(m_iDirnRel2Connect),
          (m_fFixed ? m_fIsDegFree ? "F" : "f" : " "), 
          (DbgFlags & DBG_TraverseFxdOrder ? FS():""),
          m_fDerivsBad?"x":"d",
          FlowMode()==FBMode_Xfer?"X":FlowMode()==FBMode_Simple?"S":FlowMode()==FBMode_Linear?"L":FlowMode()==FBMode_Full?"F":"?",
          UDF.ActionCount==0?" ":UDF.ActionCount==1?"r":"C",
          " ", dbgChar, "RhH");
        dbgp(fabs(m_Qm)<0.1 && WithGFmt?" %*.*g":" %*.*f", QWide,QPrec,Range(-99999.9999, m_Qm, 99999.9999));
        if (1)//DbgFlags & DBG_TraverseQ)
          dbgp(" %16s %16s %16s %16s %5s %5s", "", "", "", "","","");
        dbgpln(" %s", dbgIODesc(FBI.m_pNd, FBI.m_iIONo, FBI.m_iFENo, FBI.m_iLnkNo));

        dbgunlock();
        }
#endif
      break;
      }
    case FET_CalcDP:
      {
      m_ResT=0.0;
      m_SimpleRes=0.0;

      double B      = m_dPzX;
      double DP     = B;

      GFb.m_P_Here    += FBDirn*DP;
      GFb.m_P_HereLnk += FBDirn*DP;

      GFb.m_P_HereSafe = Max(LowPressure, GFb.m_P_Here);
      GFb.m_P_Lowest   = Min(GFb.m_P_Lowest, GFb.m_P_Here);

      GFb.m_Sum_dP     += FBDirn*DP;
      GFb.m_Sum_B      += FBDirn*B;

      // Rate of Change is not a Function of Link Direction
      GFb.m_Sum_dPbdQ  += (m_dPzXdQ);
      GFb.m_Sum_adPqdQ += 0.0;
      GFb.m_Sum_ResT   += fabs(m_ResT);
      //  GFb.Sum_ResQ   += fabs(ResQ);

      GetFlags(pGFB, FBDirn);

      GFb.m_BstDamping=Max(GFb.m_BstDamping, dBstDamping);

#if dbgFlwNets
      if (DbgFlags & DBG_TraverseP)
        {
        Strng FS;
        FS.Set(m_iFxdPOrder>=0 ? "%4i ":"     ", m_iFxdPOrder);
        dbglock();
        if (DbgFlags & DBG_TraverseFBNos)
          dbgp("fb[%4i] ", m_iNo); 
        if (DbgFlags & DBG_TraversePBNos)
          dbgp("sp[%4i] dp[%4i] ", (FBDirn>0?SrcPB():DstPB()).m_iNo,(FBDirn<0?SrcPB():DstPB()).m_iNo); 
        dbgp("%s%s%s%s%s%s%s %s%s %-2.2s %3.3s",
          SgnStr(m_iDirnRel2Traverse),
          SgnStr(m_iDirnRel2Connect),
          (m_fFixed ? "F":" "),(m_fIsDegFree ? "f" : " "), 
          (DbgFlags & DBG_TraverseFxdOrder ? FS():""),
          m_fDerivsBad?"x":"d",
          FlowMode()==FBMode_Xfer?"X":FlowMode()==FBMode_Simple?"S":FlowMode()==FBMode_Linear?"L":FlowMode()==FBMode_Full?"F":"?",
          UDF.ActionCount==0?" ":UDF.ActionCount==1?"r":"C",
          " ", dbgChar, "RhH");
        if (DbgFlags & DBG_Regulator)
          dbgp(" %2s %6s ", "", "");
        dbgp(fabs(m_Qm)<0.1 && WithGFmt?" %*.*g":" %*.*f", QWide,QPrec,Range(-99999.9999, m_Qm, 99999.9999));
        dbgp(" %#*.*f", PWide,PPrec,Range(-9999999.9999, GFb.m_P_Here, 9999999.9999));
        if (DbgFlags & DBG_TraversePFxd)
          dbgp(" %#*.*f",
          PWide,PPrec,Range(-99999.99, FBDirn>0 ? FlngPB().PGood : NodePB().PGood, 99999.99)
          );

        //dbgp(" +(%#*.*f%#*.*f", PWide,PPrec,0., PWide,PPrec,0.);
        dbgp(" +(%#*.*f", PWide,PPrec,0.);
        dbgp("%#*s ", RWide, "");
        if (DbgFlags&DBG_DPa)
          dbgp(" %#*.*f", PWide,PPrec,0.);
        if (DbgFlags&(DBG_DPb|DBG_DPz))
          dbgp(" %#*.*f", PWide,PPrec,0.);
        if (DbgFlags&(DBG_DPdQ))
          dbgp(" %#*.*f", PWide,PPrec,0.);

        dbgp("[");
        if (DbgFlags&DBG_DPb)
          dbgp("%#*.*f",PWide,PPrec, 0.0);
        if (DbgFlags&DBG_DPb)
          dbgp(" %#*.*f",PWide,PPrec, 0.0);
        if (DbgFlags&DBG_DPz)
          dbgp(" %#*.*f",PWide,PPrec, 0.0);
        if (DbgFlags&DBG_DPz)
          dbgp(fabs(m_dPzX)<0.1 && WithGFmt?" %#*.*g":" %#*.*f",PWide, PPrec, FBDirn * m_dPzX);
        if (DbgFlags&DBG_DPz)
          dbgp(" %#*.*f",PWide,PPrec, 0.0);
        if (DbgFlags&DBG_DPdQ)
          dbgp(" %#*.*f",PWide,PPrec, 0.0);
        if (DbgFlags&DBG_DPdQ)
          dbgp(" %#*.*f",PWide,PPrec, 0.0);
        dbgp("])");


        if (DbgFlags&DBG_Props)
          {
          dbgp("%*.*f %*.*f %*.*g %*.*f %*.*f ", 
            DWide, DPrec, Properties(pNetProps)->m_dRhoL, 
            DWide, DPrec, Properties(pNetProps)->m_dRhoV, 
            DWide, DPrec, Properties(pNetProps)->m_dVisc, 
            DWide, DPrec, Properties(pNetProps)->m_dLVolFrac, 
            DWide, DPrec, Properties(pNetProps)->m_dVVolFrac);
          }

        if (DbgFlags&DBG_Phys)
          {
          dbgp("%12s ", " ");
          dbgp("%8s ", " ");
          dbgp("%8s ", " ");
          dbgp("%10s ", " ");
          }
        dbgpln(" %s", dbgIODesc(FBI.m_pNd, FBI.m_iIONo, FBI.m_iFENo, FBI.m_iLnkNo));

        dbgunlock();
        }
      else if (DbgFlags & DBG_TraverseQ)
        {
        LPSTR EqnNm=Eqn() && Eqn()->FlwEqnName() ? Eqn()->FlwEqnName() : "";
        if (_stricmp(EqnNm, "None")==0)
          EqnNm=" ";
        dbgpln("%s%s%s%s %-2.2s %3.3s %11.4f %11.4f %16s %16s %16s %16s %s",
          SgnStr(FBDirn), (m_fFixed ? m_fIsDegFree ? "F" : "f" : " "), " ",
          " ", dbgChar, EqnNm,
          Range(-99999.9999, m_Qm, 99999.9999),
          Range(-99999.9999, GFb.m_P_Here, 99999.9999), "", "", "", "",
          dbgIODesc(FBI.m_pNd, FBI.m_iIONo, FBI.m_iFENo, FBI.m_iLnkNo));

        }
#endif
      break;
      }
    }
  }

//--------------------------------------------------------------------------

void FlwBlk::SetChokedQm(double Qm_, double Dp_)
  {
  if (pChoke && (Qm_<pChoke->m_Qm))
    {
    pChoke->m_At=this;
    pChoke->m_Qm=fabs(Qm_);
    pChoke->m_Dp=fabs(Dp_);
    }
  }

FlwNode * FlwBlk::GetQmEstSrc()
  {
  return dynamic_cast<FlwNode*>(m_pQmEstSrc);
  };

//--------------------------------------------------------------------------
//
// Act==0                 : Valve Fully Open
// Act==UDF.MaxActionCount : Valve Fully Closed


byte FlwBlk::UDFReqdAction(double &DP)
  {
  ASSERT(UDF.DirnAllowed!=UDFD_Both);
  DP=0.0;

  switch (UDF.DirnAllowed)
    {
    case UDFD_Fwd:
    case UDFD_Rev:
      {
      int Sgn=(UDF.DirnAllowed==UDFD_Fwd) ? 1 : -1;

      UDF.SetAction(UDFA_None);
      if (m_Qm*Sgn<0.0)
        {
        UDF.SetAction(UDFA_Close);
        DP=fabs(m_ResT*m_Qm);
        }
      // SysCAD puts the boost reqd to balance the link into dPbX.
      // if the link is shut. If this value is negetive wrt the
      // allowed direction of flow it implies that the
      // UDF should be flowing.
      else if (UDF.ActionCount==UDF.MaxActionCount)
        {
        // Fully Closed
        if (-Sgn*(m_dPbX+m_dPb)>UDF.OpenP)
          {
          UDF.SetAction(UDFA_Open);
          DP=fabs(m_dPbX+m_dPb);
          }
        }
      else
        {
        // Partially Closed / Open
        if (Sgn*(m_Qm*m_ResT)>UDF.OpenP)
          {
          UDF.SetAction(UDFA_Open);
          DP=fabs(m_ResT*m_Qm);
          }
        }

      //if ((UDF.Action==UDFA_Close) && (UDF.ActionCount==UDF.MaxActionCount))
      //  UDF.Action=UDFA_None;
      //else if ((UDF.Action==UDFA_Open) && (UDF.ActionCount==0))
      //  UDF.Action=UDFA_None;
      break;
      }

    }

  return UDF.Action;
  };

//--------------------------------------------------------------------------

void FlwBlk::UDFActivate()
  {
  double DP;
  byte Act= UDFReqdAction(DP);
  CGrpFlwBlk &GFb=*m_pGFb;
  switch (Act)
    {
    case UDFA_Open:
    case UDFA_OpenF:
      ASSERT(UDF.ActionCount>0);
      UDF.ActionCount--;
      UDF.IterCount++;
#if dbgFlwNets
      dbgpln("UDF Open  %i %14.6g : %s",UDF.ActionCount,DP,GFb.m_Lnks.GetHead()->Nd()->FullObjTag());
#endif
      break;
    case UDFA_Close:
    case UDFA_CloseF:
      ASSERT(UDF.ActionCount<UDF.MaxActionCount);
      UDF.ActionCount++;
      UDF.IterCount++;
#if dbgFlwNets
      dbgpln("UDF Close %i %14.6g : %s",UDF.ActionCount,m_Qm,GFb.m_Lnks.GetHead()->Nd()->FullObjTag());
#endif
      break;
    default:
      ASSERT(0);
    }

  if (UDF.ActionCount==0)
    UDF.Restrict=0.0;
  else
    UDF.Restrict= m_ResT * Pow(UDF.RestrictMult, UDF.ActionCount);

  };

//==========================================================================
//
//
//
//==========================================================================

CGrpFlwBlk::CGrpFlwBlk()
  {
  m_pTwoPhCd=NULL;
  m_Sum_BSmth=dNAN;
  m_Sum_RSmth=dNAN;
  m_Sum_ROpSmth=dNAN;
  m_Sum_BOpSmth=dNAN;
  Clear(NULL, NULL);
  };

//--------------------------------------------------------------------------

CGrpFlwBlk::~CGrpFlwBlk()
  {

  for (int i=0; i<m_FBSeq.GetCount(); i++)
    delete m_FBSeq[i];
  m_FBSeq.SetSize(0);

  for (int i=0; i<m_FBs.GetCount(); i++)
    m_FBs[i]->m_pGFb=NULL;

  delete m_pTwoPhCd;
  };

//-------------------------------------------------------------------------

void CGrpFlwBlk::Clear(CNetBlk* pNet_, CFullNetBlk *pFNB_)
  {
  m_pFNB                 = pFNB_;
  m_pSubNet              = NULL;
  m_pNet                 = pNet_;

  m_nFixedVlv            = 0;
  m_nFixedRqd            = 0;

  m_fDerivsBad           = false;
  m_fFunctOfPress        = false;
  m_fEvalFwdOnly         = false;
  m_fEvalRevOnly         = false;
  m_fMomentumValid       = false;
  m_fFixed               = false;
  m_fHasDegFree          = false;
  m_fFixedExternal       = false;
  m_fUnDefined           = false;

  //dbgpln("    m_fFixedExternal %#010x = FALSE", this);

  m_fFixedXfer           = false;
  m_fInXferNet           = false;
  m_fSelfLoop            = false;
  m_fUDFShut             = false;
  m_fChoked              = false;
  m_fTopologyChanged     = true;
  m_fTraverseFwd         = true;
  //m_fVFlowAllowed        = false;
  //m_fLFlowAllowed        = false;
  m_iFlwPh_Act           = FPh_UnKnown;

  m_fIsBad               = false;
  m_fSrcIsEdge           = false;
  m_fDstIsEdge           = false;
  m_fFastConvergeOK      = false;
  m_fHoldFastConverge    = false;

  m_fSetCI5              = false;
  m_fSetCI6              = false;
  m_fSetCI7              = false;
  m_fSetCI8              = false;
  m_fSetCI13             = false;
  m_fSetCI14             = false;
  m_fSetCI21             = false;
  m_fSetCI22             = false;

  m_Regulator.m_iWhat       = FBReg_Off;
  m_Regulator.m_pNd         = NULL;    
  m_Regulator.m_pFB         = NULL;    
  m_Regulator.m_dMeasValue  = 0;
  m_Regulator.m_dCtrlCur    = 1.0;//dNAN;
  m_Regulator.m_fSetCI18    = false;
  m_Regulator.m_nHistory    =0;

  m_fGFTolsOK            = false;
  m_fGFNetTolsOK         = false;
  m_fGFRegTolOK          = false;
  m_fGFHoldRegCvg        = false;

  m_fChoked              = false;
  m_ChokeInfo.Clear();

  m_CnvFilter            = 1.0;

  m_QmRatio.MFb          = NULL;
  m_QmRatio.SFb          = NULL;

  m_Src.m_pNd            = NULL;
  m_Dst.m_pNd            = NULL;
  m_Src.m_iIONo          = -1;
  m_Dst.m_iIONo          = -1;
  m_Src.m_iLnkNo         = -1;
  m_Dst.m_iLnkNo         = -1;
  m_Src.m_pFBJn          = NULL;
  m_Dst.m_pFBJn          = NULL;
  m_Src.m_pJoin          = NULL;
  m_Dst.m_pJoin          = NULL;

  m_QmIn                 = 0.0;
  m_QmEstIn              = 0.0;
  m_QmEstInMem           = 0.0;
  m_QmRqd                = 0.0;
  m_QmReg                = 1.0;
  m_QmXfCap              = dNAN;//MaxXfCap;

  //fSetVolFrac          = false;
  m_pBalProps            = NULL;
  //dVVolFrac            = 0.0;
  //dLVolFracBal         = 0.0;

  m_Src_P                = 0.0;
  m_Dst_P                = 0.0;
  m_End_P                = 0.0;
  m_P_Applied            = 0.0;
  m_P_Mean               = 0.0;
  m_P_Here               = 0.0;
  m_P_HereLnk            = 0.0;
  m_P_Lowest             = 0.0;
  m_P_Err                = 0.0;
  m_Sum_dP               = 0.0;
  m_Sum_adPqdQ           = 0.0;
  m_Sum_dPbdQ            = 0.0;
  m_Sum_ResT             = 0.0;
  //Sum_ResQ             = 0.0;
  m_Sum_B                = 0.0;
  m_Sum_ROp              = 0.0;
  m_Sum_BOp              = 0.0;

  m_Sum_RPrev[0]         = 0.0;
  m_Sum_RPrev[1]         = 0.0;
  m_Sum_BPrev[0]         = 0.0;
  m_Sum_BPrev[1]         = 0.0;

  m_PressDir             = 0;
  m_iInitialFlwDirn      = 0;
  m_fFindStaticHead      = 0;


  m_HiImp.fLimit         = false;

  m_fTwoPhase=false;
  if (m_pTwoPhCd)
    m_pTwoPhCd->QZero();

#ifdef _DEBUG
  m_dwIterNo             = 0xffffffff;
  m_nTraverse            = 0;
  m_nMaxTraverse         = 0;
  m_nAvgTraverse         = 0.0f;
#endif

  m_Lnks.RemoveAllToFreeList();

  //?? Wrong Place -> FB
  //m_dVPhaseFrac          = 0.0;
  m_iPFndInx             = -1;
  m_NdInx                = -1;
  m_SigPLInEqn           = -1;
  m_SigPVInEqn           = -1;

  m_dVResAdj             = 1.0;
  m_dLResAdj             = 1.0;
  m_dVPrevQm             = 0.0;
  m_dLPrevQm             = 0.0;
  m_dVResMtx             = 1.0;
  m_dLResMtx             = 1.0;

  m_NdInx                = -3;
  m_LnkInx.Clear();

  }

//--------------------------------------------------------------------------

void CGrpFlwBlk::AddJoin(CJoinRec * pJ, flag AtHead)
  {
  if (pJ->pGFb)
    return;
#if dbgFlwNets
  if (dbgFindGFBs())
    dbgpln("  GFB:AddJoin          j:%04i %s", pJ->m_iNo, pJ->Nd()->FullObjTag());
#endif
  if (AtHead)
    m_Lnks.AddHead(pJ);
  else
    m_Lnks.AddTail(pJ);
  pJ->pGFb=this;
  }

//--------------------------------------------------------------------------

void CGrpFlwBlk::SetNet(CNetBlk *Net, CSubNetBlk * SubNet)
  {
  m_pNet=Net;
  m_pSubNet=SubNet;

  m_FTag="";
  m_RTag="";

  m_QmIn=GetLinkQm();
  m_QmEstIn=GetLinkQmEst();
  }

//-------------------------------------------------------------------------

void CGrpFlwBlk::SetSrc(CJoinRec * pJ, int ConnNo)
  {
#if dbgFlwNets
  if (dbgFindGFBs())
    dbgpln("  GFB:SetSrc           j:%04i %i %s",pJ->m_iNo,ConnNo, pJ->pNd->FullObjTag());
#endif

  m_Src.m_pNd=pJ->pNd;
  m_Src.m_iIONo=pJ->IONo(ConnNo);
  m_Src.m_iLnkNo=pJ->LinkNo(ConnNo);
  if (m_Src.IsIO())
    m_Src.m_pFBJn=pJ->IOFBJoin(ConnNo);
  else
    m_Src.m_pFBJn=pJ->LinkFB(ConnNo);
  m_Src.m_pJoin=pJ;
  }

//-------------------------------------------------------------------------

void CGrpFlwBlk::SetDst(CJoinRec * pJ, int ConnNo)
  {
#if dbgFlwNets
  if (dbgFindGFBs())
    dbgpln("  GFB:SetDst           j:%04i %i %s",pJ->m_iNo,ConnNo, pJ->pNd->FullObjTag());
#endif
  m_Dst.m_pNd=pJ->pNd;
  m_Dst.m_iIONo=pJ->IONo(ConnNo);
  m_Dst.m_iLnkNo=pJ->LinkNo(ConnNo);
  if (m_Dst.IsIO())
    m_Dst.m_pFBJn=pJ->IOFBJoin(ConnNo);
  else
    m_Dst.m_pFBJn=pJ->LinkFB(ConnNo);
  m_Dst.m_pJoin=pJ;
  }

//--------------------------------------------------------------------------

char *CGrpFlwBlk::FwdTag()
  {
  if (m_FTag.Length()==0)
    {
    m_FTag=m_Src.m_pNd->FullObjTag();
    m_FTag+=".";
    m_FTag+=m_Src.m_pNd->IODesc_Self(m_Src.m_iIONo)->pName;
    
    m_FTag+=" > ";
    if (m_Lnks.GetCount()>0)
      {
      m_FTag+=m_Lnks.GetHead()->pNd->FullObjTag();
      if (m_Lnks.GetCount()>1)
        m_FTag+="..";
      m_FTag+=" > ";
      }
    m_FTag+=m_Dst.m_pNd->FullObjTag();
    m_FTag+=".";
    m_FTag+=m_Dst.m_pNd->IODesc_Self(m_Dst.m_iIONo)->pName;
    }
  return m_FTag();
  }

//--------------------------------------------------------------------------

char *CGrpFlwBlk::RevTag()
  {
  if (m_RTag.Length()==0)
    {
    m_RTag=m_Dst.m_pNd->FullObjTag();
    m_RTag+=".";
    m_RTag+=m_Dst.m_pNd->IODesc_Self(m_Dst.m_iIONo)->pName;
    m_RTag+=" > ";
    if (m_Lnks.GetCount()>0)
      {
      m_RTag+=m_Lnks.GetTail()->pNd->FullObjTag();
      if (m_Lnks.GetCount()>1)
        m_RTag+="..";
      m_RTag+=" > ";
      }
    m_RTag+=m_Src.m_pNd->FullObjTag();
    m_RTag+=".";
    m_RTag+=m_Src.m_pNd->IODesc_Self(m_Src.m_iIONo)->pName;
    }
  return m_RTag();
  }

//-------------------------------------------------------------------------

void CGrpFlwBlk::AttachFB(FlwBlk *pFB, CNetBlk & NI, CSubNetBlk & SNI)
  {
#if dbgFlwNets
  if (dbgFindFBs())
    dbgpln("  GFB:AttachFB         fb[%4i]",pFB->m_iNo);
#endif

  if (pFB->m_iDirnRel2Connect==0)
    {
    int xxx=0;
    }

  pFB->m_pGFb=this;
  pFB->m_bAttached2GFB=true;
  pFB->pChoke=&m_ChokeAt;
  pFB->pNetTopoChg=&NI.fTopologyChanged;
  pFB->pSubNetTopoChg=&SNI.fTopologyChanged;

  m_fFunctOfPress  = m_fFunctOfPress  || pFB->m_fFunctOfPress;
  m_fEvalFwdOnly   = m_fEvalFwdOnly   || pFB->m_fEvalFwdOnly;
  m_fEvalRevOnly   = m_fEvalRevOnly   || pFB->m_fEvalRevOnly;
  m_fMomentumValid = m_fMomentumValid || pFB->m_fMomentumValid;
  m_fTwoPhase      = m_fTwoPhase      || pFB->fTwoPhase;

  if ((pFB->UDF.DirnAllowed!=UDFD_Both) && !pFB->UDF.fInList)
    {
    pFB->UDF.fInList=true;
    SNI.m_UDFFBs.AddTail(pFB);
    }

  //dbgpln("Attach FB %08x %08x %s[%s] > %s[%s]", pFB, pFB->pGFb, Src.pNd->FullObjTag(), Src.pNd->ClassId(), Dst.pNd->FullObjTag(), Dst.pNd->ClassId()) ;
  }

//-------------------------------------------------------------------------

static LPCTSTR FPAStr(byte iFPA)
  {
  switch (iFPA)
    {
    case FPh_Gasses:   return "VapOnly";
    case FPh_Liquid:   return "LiqOnly";
    case FPh_Both:     return "Both";
    case FPh_One:      return "One";
    case FPh_None:     return "";
    case FPh_UnKnown:  return "Unknown";
    case FPh_Default:  return "Default";
    default:           return "??";
    }
  }

//-------------------------------------------------------------------------

void CGrpFlwBlk::InitFlowPhases()
  {
  m_iFlwPh_Act=FPh_Both;

  if (m_Src.IsIO())
    {
    for (int fe=m_Src.NIOFBs()-1; fe>=0; fe--)
      m_Src.IOFB(fe)->m_iFlwPh_Act=m_Src.IOFB(fe)->m_iFlwPh_Rqd;
    }
  else
    m_Src.LinkFB()->m_iFlwPh_Act=m_Src.LinkFB()->m_iFlwPh_Rqd;

  JoinIter L(m_Lnks);
  //FlwBlk * pSrcFB=m_Src.IOFBFlng();
  for (CJoinRec * pJ = L.First(); (pJ); pJ = L.Next())
    {
    int io=pJ->Dirn>0 ? 0 : 1;
    for (int fe=0; fe<pJ->NIOFBs(io); fe++)
      pJ->IOFB(io,fe)->m_iFlwPh_Act=pJ->IOFB(io,fe)->m_iFlwPh_Rqd;
    //TODO
    //_asm int 3;

    io=OtherEnd(io);
    for (fe=pJ->NIOFBs(io)-1; fe>=0; fe--)
      pJ->IOFB(io,fe)->m_iFlwPh_Act=pJ->IOFB(io,fe)->m_iFlwPh_Rqd;
    }

  if (m_Dst.IsIO())
    {
    for (int fe=0; fe<m_Dst.NIOFBs(); fe++)
      m_Dst.IOFB(fe)->m_iFlwPh_Act=m_Dst.IOFB(fe)->m_iFlwPh_Rqd;
    }
  else
    m_Dst.LinkFB()->m_iFlwPh_Act=m_Dst.LinkFB()->m_iFlwPh_Rqd;
  }

//-------------------------------------------------------------------------

void CJoinRec::InitFlowPhases()
  {
  m_iFlwPh_Act=m_iFlwPh_Rqd;
  };

inline CString DbgVLStr(byte New, bool Lo=true)
  {
  CString S;
  S.Format("%s%s", New&FPh_Gasses?(Lo?"v":"V"):" ", New&FPh_Liquid?(Lo?"l":"L"):" ");
  return S;
  }

//-------------------------------------------------------------------------

bool CGrpFlwBlk::TestSetFlowPhases(bool FullFlow)
  {
  byte New=m_iFlwPh_Act;
  bool Chg=false;

  if (0 && CFlwBlkData::sm_iFlwPh_Force==FPh_Both)
    {
    New = FPh_Both;
    }
  else
    {
    if (m_Src.IsIO())
      {
      for (int fe=m_Src.NIOFBs()-1; fe>=0; fe--)
        {
        byte Act=m_Src.IOFB(fe)->m_iFlwPh_Act;
        Chg = Chg || (New != Act);
        New &= Act;
        //dbgpln("b     %s %s              %s", DbgVLStr(New), DbgVLStr(Act), m_Src.Nd()->Tag());
        }
      }
    else
      {
      byte Act=m_Src.LinkFB()->m_iFlwPh_Act;
      Chg = Chg || (New != Act);
      New &= Act;
      //dbgpln("b     %s %s              %s", DbgVLStr(New), DbgVLStr(Act), m_Src.Nd()->Tag());
      }

    JoinIter L(m_Lnks);
    for (CJoinRec * pJ = L.First(); (pJ); pJ = L.Next())
      {
      int io=pJ->Dirn>0 ? 0 : 1;
      for (int fe=0; fe<pJ->NIOFBs(io); fe++)
        {
        byte Act=pJ->IOFB(io,fe)->m_iFlwPh_Act;
        Chg = Chg || (New != Act);
        New &= Act;
        //dbgpln("b     %s %s              %s", DbgVLStr(New), DbgVLStr(Act), pJ->Nd()->Tag());
        }

      //TODO 
      //_asm int 3;

      io=OtherEnd(io);
      for (int fe=pJ->NIOFBs(io)-1; fe>=0; fe--)
        {
        byte Act=pJ->IOFB(io,fe)->m_iFlwPh_Act;
        Chg = Chg || (New != Act);
        New &= Act;
        //dbgpln("b     %s %s              %s", DbgVLStr(New), DbgVLStr(Act), pJ->Nd()->Tag());
        }
      }

    if (m_Dst.IsIO())
      {
      for (int fe=0; fe<m_Dst.NIOFBs(); fe++)
        {
        byte Act=m_Dst.IOFB(fe)->m_iFlwPh_Act;
        Chg = Chg || (New != Act);
        New &= Act;
        //dbgpln("b     %s %s              %s", DbgVLStr(New), DbgVLStr(Act), m_Dst.Nd()->Tag());
        }
      }
    else
      {
      byte Act=m_Dst.LinkFB()->m_iFlwPh_Act;
      Chg = Chg || (New != Act);
      New &= Act;
      //dbgpln("b     %s %s              %s", DbgVLStr(New), DbgVLStr(Act), m_Dst.Nd()->Tag());
      }

    New = (New & ~CFlwBlkData::sm_iFlwPh_Block) | CFlwBlkData::sm_iFlwPh_Force;
    }

  //dbgpln("g %s %s%s %s", (Chg)?"Chg":"   ", DbgVLStr(New), FwdTag());

  if (Chg)
    {
    SetFlowPhases(New);
    return true;
    }

  return false;  
  }

//-------------------------------------------------------------------------

void CGrpFlwBlk::SetFlowPhases(byte New)
  {
  m_iFlwPh_Act = New;   
  if (m_Src.IsIO())
    {
    for (int fe=m_Src.NIOFBs()-1; fe>=0; fe--)
      m_Src.IOFB(fe)->m_iFlwPh_Act=New;
    }
  else
    {
    m_Src.LinkFB()->m_iFlwPh_Act=New;
    }

  //TODO 
  //  _asm int 3;

  JoinIter L(m_Lnks);
  //FlwBlk * pSrcFB=m_Src.IOFBFlng();
  for (CJoinRec * pJ = L.First(); (pJ); pJ = L.Next())
    {
    int io=pJ->Dirn>0 ? 0 : 1;
    for (int fe=0; fe<pJ->NIOFBs(io); fe++)
      pJ->IOFB(io,fe)->m_iFlwPh_Act=New;

    io=OtherEnd(io);
    for (int fe=pJ->NIOFBs(io)-1; fe>=0; fe--)
      pJ->IOFB(io,fe)->m_iFlwPh_Act=New;
    }

  if (m_Dst.IsIO())
    {
    for (int fe=0; fe<m_Dst.NIOFBs(); fe++)
      m_Dst.IOFB(fe)->m_iFlwPh_Act=New;
    }
  else
    {
    m_Dst.LinkFB()->m_iFlwPh_Act=New;
    //?
    }

  }

//-------------------------------------------------------------------------

bool CJoinRec::TestSetFlowPhases(bool FullFlow)
  {
  int nVFlowOKAllowed = 0;
  int nLFlowOKAllowed = 0;

  for (int c=0; c<NConns(); c++)
    {
    if ((m_iFlwPh_Act & FPh_Gasses) && GFB(c)->VFlowAllowed(FullFlow))
      nVFlowOKAllowed++;
    if ((m_iFlwPh_Act & FPh_Liquid) && GFB(c)->LFlowAllowed(FullFlow))
      nLFlowOKAllowed++;
    }

  byte Old=m_iFlwPh_Act;
  byte New=m_iFlwPh_Act;

  New = (nVFlowOKAllowed>=2) ? (New | FPh_Gasses) : (New & ~FPh_Gasses);
  New = (nLFlowOKAllowed>=2) ? (New | FPh_Liquid) : (New & ~FPh_Liquid);

  New = (New & ~CFlwBlkData::sm_iFlwPh_Block) | CFlwBlkData::sm_iFlwPh_Force;

  //dbgpln("g %s %s%s %s", (New != Old)?"Chg":"   ", DbgVLStr(New), pNd->Tag());

  if (New!=Old)
    {
    m_iFlwPh_Act=New;
    for (int c=0; c<NConns(); c++)
      GFB(c)->SetFlowPhases(New);
    return true;
    }
  return false;
  };

//-------------------------------------------------------------------------

void CGrpFlwBlk::InitFBSequence()
  {
  for (int i=0; i<m_FBSeq.GetCount(); i++)
    delete m_FBSeq[i];
  m_FBSeq.SetSize(0);

  if (m_Src.IsIO())
    {
    int iFE0=m_Src.m_pNd->NIOFBs(m_Src.m_iIONo)-1;
    for (int iFE=iFE0; iFE>=0; iFE--)
      {
      FlwBlk *pFB=m_Src.m_pNd->IOFB(m_Src.m_iIONo, iFE);
      m_FBSeq.Add(new CGrpFlwIndex(GFI_IO, m_Src.m_pNd, -1, m_Src.m_iIONo, iFE, -1, m_Src.m_pJoin, pFB, &pFB->NodePB(), &pFB->FlngPB()));
      }
    }
  else
    {
    CLinkRec &L=m_Src.Link();
    if (L.m_iJoinId0==m_Src.m_pJoin->m_iJoinId)
      {
      FlwBlk *pFB=m_Src.LinkFB();
      m_FBSeq.Add(new CGrpFlwIndex(GFI_Lnk, m_Src.m_pNd, 1, -1, 0, m_Src.m_iLnkNo, m_Src.m_pJoin, pFB, &pFB->SrcPB(), &pFB->DstPB()));
      }
    }

  JoinIter L(m_Lnks);
  // --- Intermediate Nodes
  CJoinRec * pJnt = L.First();
  while (pJnt)
    {
    int LnkDir = pJnt->Dirn;
    FlwNode * pNd=pJnt->Nd();

    // Which end First
    int SCn = (LnkDir>0 ? 0 : 1);

    if (pJnt->IsIO(SCn))
      {
      int SIo = pJnt->IONo(SCn);
      for (int iFE=0; iFE<pNd->NIOFBs(SIo); iFE++)
        {
        FlwBlk *pFB=pNd->IOFB(SIo,iFE);
        m_FBSeq.Add(new CGrpFlwIndex(GFI_IO, pNd, 1, SIo, iFE, -1, pJnt, pFB, &pFB->FlngPB(), &pFB->NodePB()));
        }
      }
    else
      {
      CLinkRec & L=*pJnt->Link(SCn);
      if (L.m_iJoinId0==pJnt->m_iJoinId)
        {
        FlwBlk *pFB=pJnt->LinkFB(SCn);
        m_FBSeq.Add(new CGrpFlwIndex(GFI_Lnk, pNd, -1, -1, 0, pJnt->LinkNo(SCn), pJnt, pFB, &pFB->DstPB(), &pFB->SrcPB()));
        }
      }

    m_FBSeq.Add(new CGrpFlwIndex(GFI_Join, pJnt->pNd, 1, -1, 0, -1, pJnt, NULL, &pJnt->m_PB, &pJnt->m_PB));

    // Other End
    int DCn = OtherEnd(SCn);
    if (pJnt->IsIO(DCn))
      {
      int DIo = pJnt->IONo(DCn);
      for (int iFE=pNd->NIOFBs(DIo)-1; iFE>=0; iFE--)
        {
        FlwBlk *pFB=pNd->IOFB(DIo,iFE);
        m_FBSeq.Add(new CGrpFlwIndex(GFI_IO, pNd, -1, DIo, iFE, -1, pJnt, pFB, &pFB->NodePB(), &pFB->FlngPB()));
        }
      }
    else
      {
      CLinkRec & L=*pJnt->Link(DCn);
      if (L.m_iJoinId0==pJnt->m_iJoinId)
        {
        FlwBlk *pFB=pJnt->LinkFB(DCn);
        m_FBSeq.Add(new CGrpFlwIndex(GFI_Lnk, pNd, 1, -1, 0, pJnt->LinkNo(DCn), pJnt, pFB, &pFB->SrcPB(), &pFB->DstPB()));
        }
      }

    pJnt = L.Next();
    }

  // --- Destination Node
  if (m_Dst.IsIO())
    {
    int iFE0=m_Dst.m_pNd->NIOFBs(m_Dst.m_iIONo);
    for (int iFE=0; iFE<iFE0; iFE++)
      {
      FlwBlk *pFB=m_Dst.m_pNd->IOFB(m_Dst.m_iIONo,iFE);
      m_FBSeq.Add(new CGrpFlwIndex(GFI_IO, m_Dst.m_pNd, 1, m_Dst.m_iIONo, iFE, -1, m_Dst.m_pJoin, pFB, &pFB->FlngPB(), &pFB->NodePB()));
      }
    }
  else
    {
    CLinkRec &L=m_Dst.Link();
    if (L.m_iJoinId0==m_Dst.m_pJoin->m_iJoinId)
      {
      FlwBlk *pFB=m_Dst.LinkFB();
      m_FBSeq.Add(new CGrpFlwIndex(GFI_Lnk, m_Dst.m_pNd, -1, -1, 0, m_Dst.m_iLnkNo, m_Dst.m_pJoin, pFB, &pFB->DstPB(), &pFB->SrcPB()));
      }
    }

  m_FBSeq[0]->m_bAtStart=true;
  m_FBSeq[m_FBSeq.GetUpperBound()]->m_bAtEnd=true;
  }

//-------------------------------------------------------------------------


void CGrpFlwBlk::DbgDumpPBlks(flag GoFwd, bool WithPress)
  {

  dbgpln("--------------------------------------------------  %s", GoFwd?FwdTag():RevTag());
  // --- Intermediate Nodes
  int Fbs0=GoFwd ? 0 : m_FBSeq.GetUpperBound();
  int FbsN=GoFwd ? m_FBSeq.GetCount() : -1;
  int FbsI=GoFwd ? 1 : -1;
  for (int i=Fbs0; i!=FbsN; i+=FbsI)
    {
    CGrpFlwIndex *I = m_FBSeq[i];
    switch (I->m_eType)
      {
      case GFI_IO:
      case GFI_Lnk:
      case GFI_Join:
        dbglock();
        dbgp("PB %-3s %s %s%s", 
          I->IsIO()?"IO":I->IsLnk()?"Lnk":"Jnt",
          I->m_iSgn>0?"+":I->m_iSgn<0?"-":".", I->AtStart(GoFwd)?"S":" ", I->AtEnd(GoFwd)?"E":" ");
        dbgp(" {");
        dbgp(I->m_pFB ? "fb[%4i,%4i]:":"f            :",I->m_pFB ? I->FB.m_iNo:-1,I->m_pFB ? I->FB.m_iFxdPOrder:-1);
        dbgp(I->m_pFB ? "PBs%4i>%-4i ":"PBs%4i|%-4i ", I->SrcPB(GoFwd).m_iNo, I->DstPB(GoFwd).m_iNo);
        dbgp(I->m_pFB ? "Ord%4i>%-4i ":"Ord%4i|%-4i ", I->SrcPB(GoFwd).m_iFxdPOrder, I->DstPB(GoFwd).m_iFxdPOrder);
        dbgp("}");
        if (WithPress)
          {
          dbgp(" n:%10.4f  f:%10.4f", I->SrcPB(GoFwd).P, I->DstPB(GoFwd).P);
          dbgp(" Estn:%10.4f  f:%10.4f", I->SrcPB(GoFwd).PEst, I->DstPB(GoFwd).PEst);
          dbgp(" Mn=n:%10.4f  f:%10.4f", I->SrcPB(GoFwd).PMn, I->DstPB(GoFwd).PMn);
          }
        dbgpln(" %s", I->Nd.Tag());
        dbgunlock();
        break;
      };
    };
  dbgpln("--------------------------------------------------");
  }

//-------------------------------------------------------------------------

void CGrpFlwBlk::TraverseGetMode(flag GoFwd, CGrpFlwBlkSD & Src, CGrpFlwBlkSD & Dst, CSpPropInfo *pNetProps, pchar dbgwhere)
  {
  FlwBlk  * pPrevFB=NULL;
#if dbgFlwNets
  if (DbgFlags & DBG_TrvGetMode)
    {
    dbglock();

    if (DbgFlags & DBG_TraverseFBNos)
      dbgp("FBNos   "); 
    if (DbgFlags & DBG_TraversePBNos)
      dbgp("PBNos             "); 
    dbgp("%s      Rgm", "      ");
    if (DbgFlags & DBG_Regulator)
      dbgp(" %2s %6s ", "", "");
    dbgp(" %*s", QWide,"Qm");
    dbgp(" %16s", "GFb.XfCap");
    dbgp(" %16s", "XfCap");
    dbgp(" %16s", "GFb.QmRqd");
    dbgp(" %16s", "QmRqd");
    dbgp(" %5s", "GQmRg");
    dbgp(" %5s", "QmReg");

    dbgpln("");
    dbgunlock();
    }
#endif

  if (_stricmp(Src.Nd()->FullObjTag(), PlantAreaTag)==0 ||
    _stricmp(Dst.Nd()->FullObjTag(), PlantAreaTag)==0)
    {
    int xxx=0;

    }

  int Fbs0=GoFwd ? 0 : m_FBSeq.GetUpperBound();
  int FbsN=GoFwd ? m_FBSeq.GetCount() : -1;
  int FbsI=GoFwd ? 1 : -1;
  for (int i=Fbs0; i!=FbsN; i+=FbsI)
    {
    CGrpFlwIndex *I = m_FBSeq[i];
    switch (I->m_eType)
      {
      case GFI_IO:
      case GFI_Lnk:
        {
        CPressBlk & SrcPB=I->SrcPB(GoFwd);
        CPressBlk & DstPB=I->DstPB(GoFwd);

        //I->FB.SetVLFracs(dTheVVolFrac, dTheLVolFrac);
        I->FB.SetPropSelect(m_iPropsSelected);
        I->FB.SetTwoPhCd(m_pTwoPhCd);
        I->FB.SetSrcFB(pPrevFB);
        if (pPrevFB)
          pPrevFB->SetDstFB(&I->FB);

        if (I->IsIO() && I->AtStart(GoFwd) && I->Nd.IO_Open_Self(I->m_iIONo) && !I->Nd.IO_Direct_Self(I->m_iIONo))
          I->FB.EvalFlowX(FET_GetMode, this, I->m_iSgn*m_PassDirn, m_PassDirn, *I, pNetProps, "g0");

        NDSTARTSW(&I->Nd, FSW_EvalFlowEquations);
        flag fHasEqnsS=I->Nd.EvalFlowEquations(FET_GetMode, pNetProps, I->m_iIONo, I->m_iFENo, I->m_iLnkNo);
        NDSTOPSW(&I->Nd, FSW_EvalFlowEquations);
        I->FB.EvalFlow(FET_GetMode, this, I->m_iSgn*m_PassDirn, m_PassDirn, *I, pNetProps, DstPB.P, "g1");


        if (I->IsIO() && I->AtEnd(GoFwd) && I->Nd.IO_Open_Self(I->m_iIONo) && !I->Nd.IO_Direct_Self(I->m_iIONo))
          I->FB.EvalFlowX(FET_GetMode, this, I->m_iSgn*m_PassDirn, m_PassDirn, *I, pNetProps, "g2");

        pPrevFB=&I->FB;
        }
      break;
      case GFI_Join:
        //I->Join.PB.P=m_P_HereSafe;
        break;
      };
    };

  }

//-------------------------------------------------------------------------

#define ForceEvalFlow 1

//-------------------------------------------------------------------------

void CGrpFlwBlk::TraversePress(eScdFlwEqnTasks Task, double Qm, flag GoFwd, CGrpFlwBlkSD & Src, CGrpFlwBlkSD & Dst, CSpPropInfo *pNetProps, pchar dbgwhere)
  {
#if dbgFlwNets
  TraverseDbgHeader(pNetProps);
#endif

  double PrevError=0;

#if DISTERRORINTRAVERSE  
  //for (int iii=10; iii>=0; iii--)
  //  {
#endif

    m_Sum_dP=0.0;
    m_Sum_adPqdQ=0.0;
    m_Sum_dPbdQ=0.0;
    m_Sum_B=0.0;
    m_Sum_ResT=0.0;
    m_BstDamping=0.0;

    if (Src.IsIO())
      m_Src_P  = Src.m_pNd->IOP_Est_Self(Src.m_iIONo);
    else
      m_Src_P = Src.m_pJoin->m_PB.PEst;

    m_P_Here = m_Src_P;
    m_P_HereLnk = m_P_Here;
    m_P_HereSafe = Max(LowPressure, m_P_Here);
    m_P_Lowest = m_P_Here;

    if (Src.IsIO())
      Src.m_pNd->Set_IOP_Self(Src.m_iIONo, m_P_HereSafe);

    double PEst = m_Src_P;

    //    pSrcFB->SetNodePress(m_Src_P);
#if dbgFlwNets
    if (DbgFlags & DBG_TraverseP)
      {
      if (DbgFlags & DBG_TraverseFBNos)
        dbgp("         "); 
      if (DbgFlags & DBG_TraversePBNos)
        dbgp("pb[%4i]          ", Src.m_pJoin->m_PB.m_iNo); 
      dbgp("Src Nd   %s       ", (DbgFlags & DBG_TraverseFxdOrder ? "     ":""));
      if (DbgFlags & DBG_Regulator)       
        dbgp(" %2s %6s ", "", "");
      dbgp(" %*s %#*.*f", QWide,"",PWide,PPrec,m_Src_P);

      if (DbgFlags & DBG_TraversePFxd)
        dbgp(" %*s", PWide,"");
      if (0) // PLnk
        dbgp("LnkPress         ");
      dbgp("   %*s",PWide,"");
      dbgp("%*s ", RWide, "");
      if (DbgFlags&DBG_DPa)
        dbgp(" %*s",PWide,"");

      if (DbgFlags&(DBG_DPb|DBG_DPz))
        dbgp(" %*s",PWide,"");
      if (DbgFlags&(DBG_DPdQ))
        dbgp(" %*s",PWide,"");
      dbgp("[");
      if (DbgFlags&DBG_DPb)
        dbgp("%*s",PWide,"");
      if (DbgFlags&DBG_DPb)
        dbgp(" %*s",PWide,"");
      if (DbgFlags&DBG_DPz)
        dbgp(" %*s",PWide,"");
      if (DbgFlags&DBG_DPz)
        dbgp(" %*s",PWide,"");
      if (DbgFlags&DBG_DPz)
        dbgp(" %*s",PWide,"");
      if (DbgFlags&(DBG_DPdQ))
        {
        dbgp(" %*s",PWide,"");
        dbgp(" %*s",PWide,"");
        }
      dbgp("])");
      if (DbgFlags&DBG_Props)
        dbgp("%*s %*s %*s %*s %*s ", DWide, "", DWide, "", DWide, "", DWide, "", DWide, "");
      if (m_fTwoPhase)//DoMultiPhase)
        {
        dbgp("%7s ", "");
        dbgp("%7s ", "");
        }

      if (DbgFlags&DBG_Phys)
        {
        dbgp("%12s ", "");
        dbgp("%8s ", "");
        dbgp("%8s ", "");
        dbgp("%10s ", "");
        }
      dbgpln(" %20s[%i]", Src.m_pNd->FullObjTag(), Src.m_pJoin->m_iJoinId);
      }
#endif

    ////TODO Improve Line convergence under variable press conditions
#if DISTERRORINTRAVERSE  
    double SumAbsDPEst=0;
    int iWrkDP=0;
#endif

    int Fbs0=GoFwd ? 0 : m_FBSeq.GetUpperBound();
    int FbsN=GoFwd ? m_FBSeq.GetCount() : -1;
    int FbsI=GoFwd ? 1 : -1;
    for (int i=Fbs0; i!=FbsN; i+=FbsI)
      {
      CGrpFlwIndex *I = m_FBSeq[i];
      switch (I->m_eType)
        {
        case GFI_IO:
          {
          FlwBlk & FB=I->FB;
          CPressBlk & SrcPB=I->SrcPB(GoFwd);
          CPressBlk & DstPB=I->DstPB(GoFwd);

          SrcPB.P=m_P_HereSafe;
          SrcPB.PEst=m_P_HereLnk;

          FB.SetQm(I->m_iSgn*Qm);

          if (I->AtStart(GoFwd) && I->Nd.IO_Open_Self(I->m_iIONo) && !I->Nd.IO_Direct_Self(I->m_iIONo))
            FB.EvalFlowX(FET_CalcDP, this, I->m_iSgn*m_PassDirn, m_PassDirn, *I, pNetProps, "p0");

          NDSTARTSW(&I->Nd, FSW_EvalFlowEquations);
          flag HasEqns=I->Nd.EvalFlowEquations(FET_CalcDP, pNetProps, I->m_iIONo, I->m_iFENo, -1);
          NDSTOPSW(&I->Nd, FSW_EvalFlowEquations);
          if (ForceEvalFlow || HasEqns)
            FB.EvalFlow(FET_CalcDP, this, I->m_iSgn*m_PassDirn, m_PassDirn, *I, pNetProps, DstPB.P, "p1");

          if (I->AtEnd(GoFwd) && I->Nd.IO_Open_Self(I->m_iIONo) && !I->Nd.IO_Direct_Self(I->m_iIONo))
            FB.EvalFlowX(FET_CalcDP, this, I->m_iSgn*m_PassDirn, m_PassDirn, *I, pNetProps, "p2");

          FB.m_DPEstFxdQm=(m_P_HereLnk-SrcPB.PEst)*(GoFwd?1:-1);
          if (!I->AtEnd(GoFwd))
            {
            DstPB.P=m_P_HereSafe;
            DstPB.PEst=m_P_HereLnk;
            PEst=m_P_HereLnk;
            }
#if DISTERRORINTRAVERSE  
          double ADP = fabs(m_P_HereLnk-SrcPB.PEst);
          SumAbsDPEst+=ADP;
          m_WrkDP.SetAtGrow(iWrkDP++, ADP);
#if dbgFlwNets
          if (dbgTraverseDistError())
            dbgpln("sb[%4i] dp[%4i] ----------       %12.4f %12.4f", SrcPB.m_iNo, DstPB.m_iNo, SrcPB.PEst, m_P_HereLnk);
#endif
#endif
          }
          break;
        case GFI_Lnk:
          {
          FlwBlk & FB=I->FB;
          CPressBlk & SrcPB=I->SrcPB(GoFwd);
          CPressBlk & DstPB=I->DstPB(GoFwd);

          SrcPB.P=m_P_HereSafe;
          SrcPB.PEst=m_P_HereLnk;

          FB.SetQm(I->m_iSgn*Qm);

          NDSTARTSW(&I->Nd, FSW_EvalFlowEquations);
          flag HasEqns=I->Nd.EvalFlowEquations(FET_CalcDP, pNetProps, -1, 0, I->m_iLnkNo);
          NDSTOPSW(&I->Nd, FSW_EvalFlowEquations);
          if (ForceEvalFlow || HasEqns)
            FB.EvalFlow(FET_CalcDP, this, I->m_iSgn*m_PassDirn, m_PassDirn, *I, pNetProps, DstPB.P, "pl");

          FB.m_DPEstFxdQm=(m_P_HereLnk-SrcPB.PEst)*(GoFwd?1:-1);
          if (!I->AtEnd(GoFwd))
            {
            DstPB.P=m_P_HereSafe;
            DstPB.PEst=m_P_HereLnk;
            PEst=m_P_HereLnk;
            }
#if DISTERRORINTRAVERSE  
          double ADP = fabs(m_P_HereLnk-SrcPB.PEst);
          SumAbsDPEst+=ADP;
          m_WrkDP.SetAtGrow(iWrkDP++, ADP);
#if dbgFlwNets
          if (dbgTraverseDistError())
            dbgpln("sb[%4i] dp[%4i] ----------       %12.4f %12.4f", SrcPB.m_iNo, DstPB.m_iNo, SrcPB.PEst, m_P_HereLnk);
#endif
#endif
          }
          break;
        case GFI_Join:
          I->Join.m_PB.P=m_P_HereSafe;
          I->Join.m_PB.PEst=m_P_Here;
#if dbgFlwNets
          if (DbgFlags & DBG_TraverseP)
            {
            if (DbgFlags & DBG_TraverseFBNos)
              dbgp("         "); 
            if (DbgFlags & DBG_TraversePBNos)
              dbgp("pb[%4i]          ", I->Join.m_PB.m_iNo); 
            dbgp("%s  %s        ", "      ", (DbgFlags & DBG_TraverseFxdOrder ? "     ":""));

            if (DbgFlags & DBG_Regulator)       
              dbgp(" %2s %6s ", "", "");
            dbgp(" %*s %#*.*f", QWide,"",PWide,PPrec,I->Join.m_PB.PEst);

            if (DbgFlags & DBG_TraversePFxd)
              dbgp(" %*s", PWide,"");
            if (0) // PLnk
              dbgp("LnkPress         ");
            dbgp("   %*s",PWide,"");
            dbgp("%*s ", RWide, "");
            if (DbgFlags&DBG_DPa)
              dbgp(" %*s",PWide,"");

            if (DbgFlags&(DBG_DPb|DBG_DPz))
              dbgp(" %*s",PWide,"");
            if (DbgFlags&(DBG_DPdQ))
              dbgp(" %*s",PWide,"");
            dbgp("[");
            if (DbgFlags&DBG_DPb)
              dbgp("%*s",PWide,"");
            if (DbgFlags&DBG_DPb)
              dbgp(" %*s",PWide,"");
            if (DbgFlags&DBG_DPz)
              dbgp(" %*s",PWide,"");
            if (DbgFlags&DBG_DPz)
              dbgp(" %*s",PWide,"");
            if (DbgFlags&DBG_DPz)
              dbgp(" %*s",PWide,"");
            if (DbgFlags&(DBG_DPdQ))
              {
              dbgp(" %*s",PWide,"");
              dbgp(" %*s",PWide,"");
              }
            dbgp("])");
            if (DbgFlags&DBG_Props)
              dbgp("%*s %*s %*s %*s %*s ", DWide, "", DWide, "", DWide, "", DWide, "", DWide, "");
            if (m_fTwoPhase)//DoMultiPhase)
              {
              dbgp("%7s ", "");
              dbgp("%7s ", "");
              }

            if (DbgFlags&DBG_Phys)
              {
              dbgp("%12s ", "");
              dbgp("%8s ", "");
              dbgp("%8s ", "");
              dbgp("%10s ", "");
              }
            dbgpln(" %20s[Jn %i]", I->Join.Nd()->Tag(), I->Join.m_iJoinId);
            }
#endif
          break;
        };
      };

    if (m_fTopologyChanged)
      {
      m_pNet->fTopologyChanged=true;
      m_pSubNet->fTopologyChanged=true;
      }

    if (Dst.IsIO())
      {
      m_Dst_P = Dst.m_pNd->IOP_Est_Self(Dst.m_iIONo);
      Dst.m_pNd->Set_IOP_Self(Dst.m_iIONo, Max(LowPressure, m_Dst_P));
      }
    else
      m_Dst_P = Dst.m_pJoin->m_PB.PEst;

    m_End_P = (GoFwd ? m_Dst_P : m_Src_P);

    // --- End Of Link
    m_P_Err     = (m_P_Here - m_Dst_P)*m_PassDirn;
    m_P_Applied = (m_Src_P-m_Dst_P)*m_PassDirn;
    m_P_Mean    = (m_Src_P+m_Dst_P)*0.5;
    m_PressDir  = ((m_Src_P-m_Dst_P+m_Sum_B)*m_PassDirn >=0.0 ? 1 : -1);

    m_Sum_dP      *= m_PassDirn;
    m_Sum_B       *= m_PassDirn;

  if (0) // Do Checks
    {
    if (!Valid(m_P_Err) || !Valid(m_P_Applied) || !Valid(m_P_Mean) || !Valid(m_Sum_dP))
      LogError("Bad Press", 0, "Bad Field m_P_Err:%.2f m_P_Applied:%.2f m_P_Mean:%.2f m_Sum_dP:%.2f %s->%s"
      , m_P_Err, m_P_Applied, m_P_Mean, m_Sum_dP, Src.Nd()->FullObjTag(), Dst.Nd()->FullObjTag());

    }


#if dbgFlwNets
    if (DbgFlags & DBG_TraverseP)
      {
      if (DbgFlags & DBG_TraverseFBNos)
        dbgp("         "); 
      if (DbgFlags & DBG_TraversePBNos)
        dbgp("pb[%4i]          ", Dst.m_pJoin->m_PB.m_iNo); 
      dbgp("Dst Nd   %s       ", (DbgFlags & DBG_TraverseFxdOrder ? "     ":""));
      if (DbgFlags & DBG_Regulator)
        dbgp(" %2s %6s ", "", "");
      //dbgp(" %*s %#*.*f Est:%#*.*f ", QWide,"",PWide,PPrec,m_Dst_P, PWide,PPrec,m_Dst_P);
      dbgp(" %*s %#*.*f", QWide,"",PWide,PPrec,m_Dst_P);

      if (DbgFlags & DBG_TraversePFxd)
        dbgp(" %*s", PWide,"");
      if (0) // PLnk
        dbgp("LnkPress         ");
      dbgp("   %*s",PWide,"");
      dbgp("%*s ", RWide, "");
      if (DbgFlags&DBG_DPa)
        dbgp(" %*s",PWide,"");

      if (DbgFlags&(DBG_DPb|DBG_DPz))
        dbgp(" %*s",PWide,"");
      if (DbgFlags&(DBG_DPdQ))
        dbgp(" %*s",PWide,"");
      dbgp("[");
      if (DbgFlags&DBG_DPb)
        dbgp("%*s",PWide,"");
      if (DbgFlags&DBG_DPb)
        dbgp(" %*s",PWide,"");
      if (DbgFlags&DBG_DPz)
        dbgp(" %*s",PWide,"");
      if (DbgFlags&DBG_DPz)
        dbgp(" %*s",PWide,"");
      if (DbgFlags&DBG_DPz)
        dbgp("%*s",PWide,"");
      if (DbgFlags&(DBG_DPdQ))
        {
        dbgp(" %*s",PWide,"");
        dbgp(" %*s",PWide,"");
        }
      dbgp("])");
      if (DbgFlags&DBG_Props)
        dbgp("%*s %*s %*s %*s %*s ", DWide, "", DWide, "", DWide, "", DWide, "", DWide, "");
      if (m_fTwoPhase)//DoMultiPhase)
        {
        dbgp("%7s ", "");
        dbgp("%7s ", "");
        }

      if (DbgFlags&DBG_Phys)
        {
        dbgp("%12s ", "");
        dbgp("%8s ", "");
        dbgp("%8s ", "");
        dbgp("%10s ", "");
        }
      dbgpln(" %20s[%i]", Dst.m_pNd->FullObjTag(), Dst.m_pJoin->m_iJoinId);

      dbgp(fabs(m_P_Err)<0.01?"Diff: %#*.*g":"Diff:%#*.*f", PWide,PPrec,m_P_Err*m_PassDirn);
      bool LnkCnvgd=m_pFNB->m_LnkEPS.ConvergedDV(m_P_Err, m_P_Mean);
      dbgpln(fabs(Qm)<0.01||fabs(Qm)>=1.0e5?" @%*.*g %s":" @%*.*f %s",QWide,QPrec,Qm, LnkCnvgd?"":"<<<<<<<<<<<<<<< Not Converged");

      dbgpln("%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
        m_fGFTolsOK         ? "GFTolsOK ":"",
        m_fGFNetTolsOK      ? "GFNetTolsOK ":"",
        m_fGFRegTolOK       ? "GFRegTolOK ":"",
        m_fGFHoldRegCvg     ? "GFHoldRegCvg ":"",
        m_fDerivsBad        ? "DerivsBad ":"",
        m_fFunctOfPress     ? "FunctOfPress ":"",
        m_fEvalFwdOnly      ? "EvalFwdOnly ":"",
        m_fEvalRevOnly      ? "EvalRevOnly ":"",
        m_fMomentumValid    ? "MomentumValid ":"",
        m_fFixedXfer        ? "FixedXfer ":"",
        m_fFixedExternal    ? "FixedExternal ":"",
        m_fInXferNet        ? "InXferNet ":"",
        m_fUnDefined        ? "UnDefined ":"",
        m_fTopologyChanged  ? "TopologyChanged ":"",
        m_fChoked           ? "Choked ":"",
        m_fSelfLoop         ? "SelfLoop ":"",
        m_fUDFShut          ? "UDFShut ":"",
        m_fTraverseFwd      ? "TraverseFwd ":"",
        m_fIsBad            ? "IsBad ":"",
        m_fSrcIsEdge        ? "SrcIsEdge ":"",
        m_fDstIsEdge        ? "DstIsEdge ":"");

      dbgpln("QmRqd: %-4s %-4s %s", m_fFixed?"Fxd":"   ", m_fHasDegFree?"DegF":"    ", DbgFltString(m_QmRqd,QPrec,QPrec,QWide));
      dbgpln("QmReg: %-4s %-4s %s", "", "", DbgFltString(m_QmReg,QPrec,QPrec,QWide));
      dbgpln("XfCap: %-4s %-4s %s", m_fFixedXfer?"Xfr":"   ", "", DbgFltString(m_QmXfCap,QPrec,QPrec,QWide));

      if (dbgTraversePBlks())                                           
        DbgDumpPBlks(GoFwd, true);
      }
#endif

    //break; // <<<<<<<<<<

#if DISTERRORINTRAVERSE  

    //if (fabs(PrevError-m_P_Err)<1e-6)
    //  break;
    //PrevError=m_P_Err;

#if dbgFlwNets
    if (dbgTraverseDistError())
      dbgpln("============= %12.4f", SumAbsDPEst);
#endif

    SumAbsDPEst=GTZ(SumAbsDPEst);
    iWrkDP=0;
    double P_HereLnk=m_Src_P;
    for (int i=Fbs0; i!=FbsN; i+=FbsI)
      {
      CGrpFlwIndex *I = m_FBSeq[i];
      switch (I->m_eType)
        {
        case GFI_IO:
          {
          FlwBlk & FB=I->FB;
          CPressBlk & SrcPB=I->SrcPB(GoFwd);
          CPressBlk & DstPB=I->DstPB(GoFwd);

          SrcPB.P    = P_HereLnk;
          SrcPB.PEst = P_HereLnk;
          SrcPB.PMn  = P_HereLnk;
          P_HereLnk += FB.m_DPEstFxdQm*(GoFwd?1:-1);

          if (!I->AtEnd(GoFwd))
            {
            //P_HereLnk +=  m_P_Err*m_WrkDP[iWrkDP++]/SumAbsDPEst;
            DstPB.P    = P_HereLnk;
            DstPB.PEst = P_HereLnk;
            DstPB.PMn  = P_HereLnk;
            //DstPB.PEst = SrcPB.PEst + m_P_Err*m_WrkDP[iWrkDP++]/SumAbsDPEst;
            //DstPB.PMn = DstPB.PEst;
            }
          //SumAbsDPEst+=fabs(DstPB.PEst-SrcPB.PEst);
#if dbgFlwNets
          if (dbgTraverseDistError())
            dbgpln("sb[%4i] dp[%4i] ++++++++++       %12.4f %12.4f", SrcPB.m_iNo, DstPB.m_iNo, SrcPB.PEst, DstPB.PEst);
#endif
          }
          break;
        case GFI_Lnk:
          {
          FlwBlk & FB=I->FB;
          CPressBlk & SrcPB=I->SrcPB(GoFwd);
          CPressBlk & DstPB=I->DstPB(GoFwd);

          SrcPB.P    = P_HereLnk;
          SrcPB.PEst = P_HereLnk;
          SrcPB.PMn  = P_HereLnk;
          P_HereLnk += FB.m_DPEstFxdQm*(GoFwd?1:-1);

          if (!I->AtEnd(GoFwd))
            {
            //P_HereLnk +=  m_P_Err*m_WrkDP[iWrkDP++]/SumAbsDPEst;
            DstPB.P    = P_HereLnk;
            DstPB.PEst = P_HereLnk;
            DstPB.PMn  = P_HereLnk;
            //DstPB.PEst = SrcPB.PEst + m_P_Err*m_WrkDP[iWrkDP++]/SumAbsDPEst;
            //DstPB.PMn = DstPB.PEst;
            }
          //SumAbsDPEst+=fabs(DstPB.PEst-SrcPB.PEst);
#if dbgFlwNets
          if (dbgTraverseDistError())
            dbgpln("sb[%4i] dp[%4i] ++++++++++       %12.4f %12.4f", SrcPB.m_iNo, DstPB.m_iNo, SrcPB.PEst, DstPB.PEst);
#endif
          //dbgpln("+++++++++++++ %12.4f %12.4f", SrcPB.PEst, DstPB.PEst);
          }
          break;
        case GFI_Join:
          I->Join.m_PB.P    = P_HereLnk;
          I->Join.m_PB.PEst = P_HereLnk;
          I->Join.m_PB.PMn  = P_HereLnk;
          break;
        };
      };
  //  }

  //if (iii<0)
  //  LogWarning("FlwNets", 0, "RedistError Loop not Converged");

#endif


  if (Valid(m_QmXfCap)) m_QmXfCap *= m_PassDirn;
  if (Valid(m_QmRqd)) m_QmRqd *= m_PassDirn;
  };

//-------------------------------------------------------------------------

void CGrpFlwBlk::TraverseDbgHeader(CSpPropInfo *pNetProps)
  {
#if dbgFlwNets
  if (DbgFlags & DBG_TraverseP)
    {
    dbglock();

    if (pNetProps)
      dbgpln("Vol V/L%8.4f/%8.4f+%8.4f Rho nV/L %12.5f/%12.5f   Vi%8.3f T %8.3f",
      pNetProps->m_dVVolFrac*100, 
      pNetProps->m_dLVolFrac*100, pNetProps->m_dLVolFracBal*100,
      pNetProps->m_dRhoV, pNetProps->m_dRhoL, 
      pNetProps->m_dVisc, K2C(pNetProps->m_dTemp));

    //dbgp("%s     Rgm %*s %*s", SolnStr(SolutionType()), QWide,"Qm",PWide,"Press");
    if (DbgFlags & DBG_TraverseFBNos)
      dbgp("FBNos   "); 
    if (DbgFlags & DBG_TraversePBNos)
      dbgp("PBNos             "); 
    dbgp("%s      Rgm", "       ");
    if (DbgFlags & DBG_Regulator)
      dbgp(" %2s %6s ", "", "");
    dbgp(" %*s %*s", QWide,"Qm",PWide,"Press");

    if (DbgFlags & DBG_TraversePFxd)
      dbgp(" %*s", PWide,"PGood");
    if (0) // PLnk
      dbgp("LnkPress         ");
    dbgp("   %*s",PWide,"ResDrop");
    dbgp("%*s ", RWide, "Res");
    //dbgp(" %*s",PWide,"RstDrop");
    if (DbgFlags&DBG_DPa)
      dbgp(" %*s",PWide,"Accel");

    if (DbgFlags&(DBG_DPb|DBG_DPz))
      dbgp(" %*s",PWide,"Boost");
    if (DbgFlags&(DBG_DPdQ))
      dbgp(" %*s",PWide,"dQ");
    dbgp("[");
    if (DbgFlags&DBG_DPb)
      dbgp("%*s",PWide,"dPb");
    if (DbgFlags&DBG_DPb)
      dbgp(" %*s",PWide,"dPbX");
    if (DbgFlags&DBG_DPz)
      dbgp(" %*s",PWide,"dPz");
    if (DbgFlags&DBG_DPz)
      dbgp(" %*s",PWide,"dPzX/F");
    if (DbgFlags&DBG_DPz)
      dbgp("%*s",PWide,"dPzI");
    if (DbgFlags&(DBG_DPdQ))
      {
      dbgp(" %*s",PWide,"PbdQ");
      dbgp(" %*s",PWide,"PqdQ");
      }
    dbgp("])");

    if (DbgFlags&DBG_Props)
      {
      dbgp("%*s %*s %*s %*s %*s ", DWide, "RhoL", DWide, "NRhoV", DWide, "Visc", DWide, "LVolFr", DWide, "VVolFr");
      }
    if (m_fTwoPhase)//DoMultiPhase)
      {
      dbgp("%7s ", "Slip");
      dbgp("%7s ", "VoidFrc");
      }

    if (DbgFlags&DBG_Phys)
      {
      dbgp("%12s ", "Vel");
      dbgp("%8s ", "Len");
      dbgp("%8s ", "Diam");
      dbgp("%10s ", "KFact");
      }

    dbgpln("");
    dbgunlock();
    }
  else if (DbgFlags & DBG_TraverseQ)
    {
    dbgpln("%s      Rgm           Qm       Press    GFb.dQm_XfCap        dQm_XfCap        GFb.dQm_Rqd        dQm_Rqd", "      ");//SolnStr(SolutionType()));
    }
#endif
  }

//-------------------------------------------------------------------------

void CGrpFlwBlk::Traverse(pchar dbgwhere)
  {
  SCDTRY
    {

    double       dPErrRatio=0, P_ErrPrev;
    int          FBDirn[2]={1,-1};
    //int          SIo, DIo, SCn, DCn, iFE;
    //FlwNode     *Src.m_pNd, *Dst.m_pNd;
    //int         Src.m_iIONo, Dst.m_iIONo;

    JoinIter L(m_Lnks);

    SolveDbgFlags(m_fHasDbgBrk);

    if (!FlwExists(m_QmIn))
      m_QmIn=0.0;
    int FlwDirn=(int)Sign(NZ(m_QmIn));
    m_QmIn = Range((double)-1.0e20, m_QmIn, 1.0e20); //temp kga 26/4/2001 to try prevent crash
    double Qm=m_QmIn;

    P_ErrPrev=1e6;
    m_P_Err=0.0;

    if (m_fEvalFwdOnly)
      m_PassDirn = 1;
    else if (m_fEvalRevOnly)
      m_PassDirn = -1;
    else
      m_PassDirn = (sint)FlwDirn;
    flag GoFwd = m_PassDirn > 0;

    if (m_fEvalFwdOnly || m_fEvalRevOnly)
      BreakPoint();

    CGrpFlwBlkSD & Src = (GoFwd ? m_Src:m_Dst);
    CGrpFlwBlkSD & Dst = (GoFwd ? m_Dst:m_Src);

    m_iPropsSelected=FBPS_Default;

    m_fDerivsBad=false;
    m_fFunctOfPress=false;
    m_fEvalFwdOnly=false;
    m_fEvalRevOnly=false;


    int Fbs0=GoFwd ? 0 : m_FBSeq.GetUpperBound();
    int FbsN=GoFwd ? m_FBSeq.GetCount() : -1;
    int FbsI=GoFwd ? 1 : -1;
    for (int i=Fbs0; i!=FbsN; i+=FbsI)
      {
      CGrpFlwIndex *I = m_FBSeq[i];
      switch (I->m_eType)
        {
        case GFI_IO:
        case GFI_Lnk:
          I->FB.GetFlags(this, I->m_iSgn);
          break;
        case GFI_Join:
          break;
        };
      };

    m_XBst.m_pFB=NULL;
    m_XBst.m_dFB=0;
    int FixedLoopCnt=0;
FixdPBx:

    for (int PErrIter=0; ; PErrIter++) // loop in case of pressure dependencies in line
      {
      m_fDerivsBad            = false;
      m_fFunctOfPress         = false;
      m_fEvalFwdOnly          = false;
      m_fEvalRevOnly          = false;
      m_fMomentumValid        = false;
      m_fHoldFastConverge     = false;

      Qm=m_QmIn;
      SolveDbgFlags(m_fHasDbgBrk);

#ifdef _DEBUG
      flag IterStart=(m_dwIterNo!=m_pFNB->dwIterNo);
      if (IterStart)
        {
        m_nMaxTraverse=Max(m_nMaxTraverse, m_nTraverse);
        m_nAvgTraverse=m_nAvgTraverse*0.9f+m_nTraverse*0.1f;
        m_nTraverse=0;
        }
      m_nTraverse++;
      m_dwIterNo=m_pFNB->dwIterNo;
#endif

      m_fGFTolsOK=false;
      m_fGFNetTolsOK=false;
      m_fGFRegTolOK=false;

#if dbgFlwNets
      if (DbgFlags & DBG_Traverse)
        dbgpln("------------------------%s %s %s------------------------ %s",
        m_Src.m_pNd->FullObjTag(),GoFwd?"-->":"<--",m_Dst.m_pNd->FullObjTag(), dbgwhere);
#endif

      m_Sum_dP=0.0;
      m_Sum_adPqdQ=0.0;
      m_Sum_dPbdQ=0.0;
      m_Sum_B=0.0;
      m_Sum_ResT=0.0;
      //Sum_ResQ=0.0;
      //m_Sum_ROp=0.0;
      //m_Sum_BOp=0.0;

      m_ChokeAt.m_At=NULL;
      m_ChokeAt.m_Qm=1.0e200;

      m_fUDFShut=0;

      flag Fixed_Mem  = m_fFixed;
      flag Xfer_Mem   = m_fFixedXfer;
      if (m_fFixedExternal)
        {
        m_fFixed     = true;
        m_fFixedXfer = m_fInXferNet;
        }
      else
        {
        m_fFixed     = false;
        m_fFixedXfer = false;
        }
      m_QmRqd      = dNAN;
      m_QmReg      = 1.0;
      m_QmXfCap    = dNAN;
      m_nFixedVlv  = 0;
      m_nFixedRqd  = 0;

      m_Regulator.m_iWhat = FBReg_Off;
      m_Regulator.m_pNd   = NULL;    
      m_Regulator.m_pFB   = NULL;    

      m_fHasDegFree       = false;
      m_fTopologyChanged  = false;
      m_QmRatio.MFb       = NULL;
      m_QmRatio.SFb       = NULL;

      //CSpPropInfo LclNetProps;
      CSpPropInfo *pNetProps=NULL;
      //if (Src.IsIO())
      Src.m_pNd->IOGetNetProps(Src.m_iIONo, Qm);
      //else
      //  Src.m_pNd->LinkGetNetProps(Src.m_iLnkNo, Qm);
      if (pNetProps)
        {
        if (m_fFindStaticHead)
          {
          m_pBalProps=pNetProps;
          m_fDerivsBad=true;
          }
        }
      else
        {
        if (m_fFindStaticHead)
          {
          int iFE0=Src.m_pNd->NIOFBs(Src.m_iIONo)-1;
          FlwBlk & FB=*Src.m_pNd->IOFB(Src.m_iIONo, iFE0);
          m_pBalProps=FB.Properties(NULL);//&FB.m_FlwProps;
          }
        }


#if WITHTWOPHASE
      if (fTwoPhase)
        {
        if (!pTwoPhCd)
          pTwoPhCd=new SpConduit("TwoPhCd");
        pTwoPhCd->QSetM(*(Src.m_pNd->IOConduit(Src.m_iIONo)), som_ALL, Max(2.0*UsableMass, fabs(m_QmIn)), m_Src_P);
#if dbgFlwNets
        if (DbgFlags & DBG_TraverseP)
          dbgpln("TwoPhCd    T:%#16.7f  P:%#16.7f  SrcT:%#16.7f  SrcP:%#16.7f",
          pTwoPhCd->Temp(), pTwoPhCd->Press(),
          Src.m_pNd->IOConduit(Src.m_iIONo)->Temp(), Src.m_pNd->IOConduit(Src.m_iIONo)->Press());

#endif
        }
#else
      m_pTwoPhCd=NULL;
#endif

      if (0)
        {
        Strng S;
        S.Set("%5s %9s %9s %13s %14s", m_fFixed?"Fixed":"", m_fFixedXfer?"FixedXfer":"", m_fInXferNet ?"InXferNet":"", m_fFixedExternal ?"FixedExternal":"", m_fFastConvergeOK ?"FastConvergeOK":"");
        dbgpln("Flags A %-50s %s", S(), m_FTag());
        }

      // InitPtrs in Direction of Flow and adjust fill if neccessary
      TraverseGetMode(GoFwd, Src, Dst, pNetProps, dbgwhere);

      if (Valid(m_QmRqd))
        m_QmRqd*=m_QmReg;

      if (m_fTopologyChanged)
        {
        m_pNet->fTopologyChanged=true;
        m_pSubNet->fTopologyChanged=true;
        }


      if (0)
        {
        Strng S;
        S.Set("%5s %9s %9s %13s %14s", m_fFixed?"Fixed":"", m_fFixedXfer?"FixedXfer":"", m_fInXferNet ?"InXferNet":"", m_fFixedExternal ?"FixedExternal":"", m_fFastConvergeOK ?"FastConvergeOK":"");
        dbgpln("Flags B %-50s %s", S(), m_FTag());
        }

      if (m_fFixedXfer)
        // This will Set Qm but assume that all impedances are ZERO
        TraversePress(FET_SetQm, Qm, GoFwd, Src, Dst, pNetProps, dbgwhere);
      else
        // Called for all Non-XFer links (Fixed, FreeFlow,..)  
        // This will Set Qm and calculate all impedances
        TraversePress(FET_CalcDP, Qm, GoFwd, Src, Dst, pNetProps, dbgwhere);

      //dbgpln("Flags1 %s %s %s",m_fFixed ? "m_fFixed":"        ", m_fFixedXfer?"m_fFixedXfer":"            ", FwdTag());

      // Test Press Converged
      if (!m_fFunctOfPress || m_fFixed || m_fFixedXfer)
        {
        m_fSetCI5=false;
        goto PressLoopDone;
        }
      else
        {
        //double dPErrChgRatio;
        double P_ErrChg=m_P_Err-P_ErrPrev;
        if (m_pFNB->m_LnkEPS.ConvergedDV(P_ErrChg, m_P_Err /*1.0e-12, >Lnk_Eps_R, dPErrChgRatio*/))
          {
          m_fSetCI5=false;
          goto PressLoopDone;
          }
        if (PErrIter>100)
          {
          m_fSetCI5=true;
          goto PressLoopDone;
          }
        P_ErrPrev=m_P_Err;
        }
      } // Press Iter Loop

PressLoopDone:

    Qm=m_QmIn;

    if (m_fFixed || m_fFixedXfer)
      {
      if (m_fHasDegFree)
        {
        //if (FixedLoopCnt++==0 && fabs(m_P_Err)>1.0e-12 && m_XBst.m_pFB!=NULL)
        if (FixedLoopCnt<5 && (m_fFixed && fabs(m_P_Err)>1.0e-12 && m_XBst.m_pFB!=NULL))
          {
          //dbgp(" %2i m_dPbX %12.3f > ", FixedLoopCnt, m_XBst.m_pFB->m_dPbX);

          m_XBst.m_pFB->m_dPbX-=m_XBst.m_dFB*m_P_Err*(FixedLoopCnt==0 ? 1.0 : 0.5)*m_PassDirn;

          //dbgpln(" %12.3f  %s>%s", m_XBst.m_pFB->m_dPbX, m_Src.m_pNd->Tag(), m_Dst.m_pNd->Tag());
          FixedLoopCnt++;
          goto FixdPBx;
          }
        }

      if (!Valid(m_P_Err))
        {
        _asm int 3;
        BreakPoint()
        }

      if (fabs(m_P_Err)>1.0e-12)
        BreakPoint();

      if (Valid(m_QmRqd))
        m_fGFTolsOK = (fabs(m_QmIn-m_QmRqd)<=fabs(1e-8*m_QmRqd));//10);
      else if (Valid(m_QmXfCap))
        m_fGFTolsOK = (fabs(m_QmIn-m_QmXfCap)<=fabs(1e-8*m_QmXfCap));//-10);
      else
        m_fGFTolsOK = true;
      m_fGFRegTolOK = m_fGFTolsOK;
      m_fGFNetTolsOK = m_fGFTolsOK;

#if dbgFlwNets
      if (dbgTraverseShowFlags())
        {
        CString S;
        if (Valid(m_QmRqd))
          S.Format("QmRqd:%10.3f %10.3f %10.6e", m_QmIn, m_QmRqd, m_QmIn-m_QmRqd);
        else if (Valid(m_QmXfCap))
          S.Format("QmCap:%10.3f %10.3f %10.6e", m_QmIn, m_QmXfCap, m_QmIn-m_QmXfCap);
        else
          S="                                         ";
        dbgpln("TolsA {%s}: %s %s %s %s", S, m_fGFTolsOK ?"GFTolsOK":"", m_fGFRegTolOK  ?"RegTolOK":"", m_fGFNetTolsOK  ?"NetTolsOK":"", FwdTag());
        }
#endif
      }
    else
      {
      m_fGFRegTolOK=true;
      m_Regulator.m_fSetCI18=false;
      if (m_Regulator.m_iWhat!=FBReg_Off && m_Regulator.m_pFB->FlowMode()==FBMode_Full)
        {
        m_Regulator.m_pFRB->m_pFB=m_Regulator.m_pFB;

        FlwBlk &FB=*m_Regulator.m_pFB;

        if (!FB.m_MeasDone)
          FB.m_MeasRhoStep=FB.m_MeasRho;

        double MV=m_Regulator.GetMeasValue();

        if (!FB.m_MeasDone)
          m_Regulator.m_pFRB->SetMeasStart(MV);

        FB.m_MeasDone=true;

        //dbgpln("RegMeasValue: %15.6f %15.6f %15.6f", MV, m_Regulator.m_pFRB->StepTarget(), m_Regulator.m_pFRB->FinalTarget());

        //m_Regulator.GetMeasValue();
        //m_Regulator.GetMeasValue();
        //switch (m_Regulator.m_iWhat)
        //  {
        //  case FBReg_Qm     : m_Regulator.m_dMeasValue = FB.m_Qm;  break;
        //  case FBReg_Qv     : m_Regulator.m_dMeasValue = FB.m_Qm/FB.m_MeasRho;  break;
        //  case FBReg_QvStep : m_Regulator.m_dMeasValue = FB.m_Qm/FB.m_MeasRhoStep;  break;
        //  case FBReg_Pi     : m_Regulator.m_dMeasValue = FB.SrcPB().PEst;  break;
        //  case FBReg_Po     : m_Regulator.m_dMeasValue = FB.DstPB().PEst;  break;
        //  }
        m_Regulator.m_pFRB->SetMeas(m_Regulator.m_dMeasValue);
        m_fGFRegTolOK=m_pFNB->m_RegSolveEPS.ConvergedVV(m_Regulator.m_dMeasValue , m_Regulator.m_pFRB->StepTarget());
        }

      if (!m_HiImp.fLimit)
        {
        m_fGFTolsOK = m_pFNB->m_LnkEPS.ConvergedDV(m_P_Err, m_P_Mean);
        dPErrRatio = m_pFNB->m_LnkEPS.GetNormalError();
        m_fGFNetTolsOK = (dPErrRatio / m_pFNB->m_NetDeRating< 1.0);
        }
      else
        {
        m_fGFTolsOK = true;
        m_fGFNetTolsOK = true;
        }
#if dbgFlwNets
      if (!m_fGFNetTolsOK)
        if (dbgNetConverge())
          dbgpln("  !NetSolved Qm:%#16.9g Tol:%7.1f R:%#16.9g P_A:%#16.9g B:%#16.9g P_M:%#16.9g %s ",
          Qm, Range(0.0, dPErrRatio / m_pFNB->m_NetDeRating, 9999.9),m_Sum_ResT, m_P_Applied,
          m_P_Applied+m_Sum_B, m_P_Mean, FwdTag());
      if (dbgTraverseShowFlags())
        {
        CString S("                                         ");
        dbgpln("TolsB  %s : %s %s %s %s", S, m_fGFTolsOK ?"GFTolsOK":"", m_fGFRegTolOK  ?"RegTolOK":"", m_fGFNetTolsOK  ?"NetTolsOK":"", FwdTag());
        }
#endif
      }

    if (m_ChokeAt.m_At==NULL)
      {
      m_fChoked=0;
      m_ChokeInfo.m_At=NULL;
      m_ChokeInfo.m_PX=0.0;
      }

    FlwNode * pQmEstSrc=m_XBst.m_pFB ? dynamic_cast<FlwNode*>(m_XBst.m_pFB->AttachedTo()) : NULL;
    for (int i=0; i<m_FBSeq.GetCount(); i++)
      {
      CGrpFlwIndex *I = m_FBSeq[i];
      switch (I->m_eType)
        {
        case GFI_IO:
        case GFI_Lnk:
          I->FB.m_pQmEstSrc=pQmEstSrc;
          break;
        case GFI_Join:
          break;
        };
      }
    //if (pLimitNode)
    //  dbgpln("QmEstSrc: %s", pQmEstSrc->FullObjTag());

#if dbgFlwNets
    if (DbgFlags & DBG_TrvResult)
      {
      if (m_fFixedXfer || m_HiImp.fLimit)
        dPErrRatio=-1.0;
      char ErrV[256], NErrV[256], Qr[256];
      sprintf(ErrV, "%2i", (int)Min(dPErrRatio,99.0));
      sprintf(NErrV, "%2i", (int)Min(dPErrRatio/m_pFNB->m_NetDeRating,99.0));
      if (m_fFixed)
        sprintf(Qr, "%11.4g", m_fFixedXfer ? m_QmXfCap : m_QmRqd);
      else
        sprintf(Qr, "%-11.11s", "*");
      dbglock();
      //    dbgp("T[%-8.8s] L%s N%s A:%*.*g B:%*.*g Q:%*.*g Rt:%*.*g Rq:%*.*g dP:%*.*g e:%*.*g Qr:%s",
      dbgp("T[%-8.8s] L%s N%s R%s %s %s S:%*.*g D:%*.*g A:%*.*g B:%*.*g Q:%*.*g Rt:%*.*g dP:%*.*g e:%*.*g Qr:%s",
        dbgwhere, 
        (m_fGFTolsOK ? " ." : ErrV),
        (m_fGFNetTolsOK ? " ." : NErrV),
        (m_fGFRegTolOK? "." : "*"),
        m_fDerivsBad?"x":"d",
        m_fFixedXfer?"X":m_fFixed?"F":" ",
        PWide,PPrec,m_Src_P, 
        PWide,PPrec,m_Dst_P, 
        PWide,PPrec,m_P_Applied, 
        PWide,PPrec,m_Sum_B, 
        QWide,QPrec,Qm, 
        RWide,RPrec,m_Sum_ResT, 
        //      RWide,RPrec,Sum_ResQ, 
        PWide,PPrec,m_Sum_dP, 
        PWide,PPrec,m_P_Err, Qr);
      if (DbgFlags&DBG_Deltas)
        dbgp(" dQ:%13.5g dB:%13.5g", m_Sum_adPqdQ, m_Sum_dPbdQ);

      if (DbgFlags & DBG_Regulator)
        {
        if (m_Regulator.m_iWhat!=FBReg_Off)
          dbgp("[C:%7.4f M:%8.2f T:%8.2f]", m_Regulator.m_dCtrlCur, m_Regulator.m_dMeasValue, m_Regulator.m_pFRB->StepTarget());
        else
          dbgp("  %14s %15s ", "", "");
        }

      dbgp(" %s", FwdTag());
      dbgpln("");
      dbgunlock();
      }
    if (dbgTopologyChg())
      if (m_fTopologyChanged)
        dbgpln("  ----> Topology Change FxdQm %s Q:%#16.9g %s ",
        (m_fFixed ? (m_fFixedXfer ? "TRANSFER":"FIXED"):m_fUnDefined?"UNDEFINED":"FREE "),
        m_fFixedXfer ? m_QmXfCap : m_QmRqd, FwdTag());
    if (dbgSolveFxdQsTrv())
      if(m_fFixedXfer)
        dbgpln("  Lnk dQm_XfCap:%12.4g P:%12.4g > %12.4g %s", m_QmXfCap,m_Src_P,m_Dst_P,FwdTag());
      else if(m_fFixed)
        dbgpln("  Lnk dQm_Rqd:%12.4g   P:%12.4g > %12.4g %s", m_QmRqd,m_Src_P,m_Dst_P,FwdTag());
#endif

    }
  SCDCATCH("CGrpFlwBlk::Traverse", FwdTag());
  }

//-------------------------------------------------------------------------

void CGrpFlwBlk::SetLinkQmEstAndQm(double Qm)
  {
#if (dbgFlwNets)
  if (dbgShowQmSet() && (!dbgTraverseDbgBrk() || m_fHasDbgBrk))
    {
    char buff1[256];
    char buff2[256];
    sprintf(buff1, fabs(m_QmIn)<0.1 && WithGFmt?"%*.*g":"%*.*f", QWide,QPrec,Range(-99999.9999, m_QmIn, 99999.9999));
    sprintf(buff2, fabs(Qm)<0.1 && WithGFmt?"%*.*g":"%*.*f", QWide,QPrec,Range(-99999.9999, Qm, 99999.9999));
    dbgpln("SetLinkQmEstAndQm %s > %s %s", buff1, buff2, FwdTag());
    }
#endif

  if (!Valid(Qm))
    LogError("Solver", 0, "Internal Error - CGrpFlwBlk::Set_Qm - Bad Qm (1078)");
  m_QmIn=Qm;
  m_QmEstIn=Qm;

  for (int i=0; i!=m_FBSeq.GetCount(); i++)
    {
    CGrpFlwIndex *I = m_FBSeq[i];
    switch (I->m_eType)
      {
      case GFI_IO:
      case GFI_Lnk:
        I->FB.SetQm(I->m_iSgn*Qm);
        I->FB.SetQmEst(I->m_iSgn*Qm);
        //dbgpln("SetQm f:%04i", I->FB.m_iNo, I->m_iSgn*Qm);
        break;
      }
    }
  }

//-------------------------------------------------------------------------

void CGrpFlwBlk::SetLinkQmEst(double Qm)
  {
#if (dbgFlwNets)
  if (dbgShowQmSet() && (!dbgTraverseDbgBrk() || m_fHasDbgBrk))
    {
    char buff1[256];
    char buff2[256];
    sprintf(buff1, fabs(m_QmIn)<0.1 && WithGFmt?"%*.*g":"%*.*f", QWide,QPrec,Range(-99999.9999, m_QmIn, 99999.9999));
    sprintf(buff2, fabs(Qm)<0.1 && WithGFmt?"%*.*g":"%*.*f", QWide,QPrec,Range(-99999.9999, Qm, 99999.9999));
    dbgpln("SetLinkQmEst      %s > %s %s", buff1, buff2, FwdTag());
    }
#endif
  m_QmEstIn=Qm;

  for (int i=0; i!=m_FBSeq.GetCount(); i++)
    {
    CGrpFlwIndex *I = m_FBSeq[i];
    switch (I->m_eType)
      {
      case GFI_IO:
      case GFI_Lnk:
        //I->FB.SetQm(I->m_iSgn*Qm);
        I->FB.SetQmEst(I->m_iSgn*Qm);
        break;
      }
    }
  }

//-------------------------------------------------------------------------

double CGrpFlwBlk::GetLinkQm()
  {
  if (m_Src.IsIO())
    return m_Src.m_pNd->IOQm_Out(m_Src.m_iIONo);

  FlwBlk & FB=*m_Src.LinkFB();
  return FB.m_Qm*FB.m_sQMeas;
  }

//-------------------------------------------------------------------------

double CGrpFlwBlk::GetLinkQmEst()
  {
  if (m_Src.IsIO())
    return m_Src.m_pNd->IOQmEst_Out(m_Src.m_iIONo);

  FlwBlk & FB=*m_Src.LinkFB();
  return FB.m_QmEst*FB.m_sQMeas;
  }

//-------------------------------------------------------------------------

void CGrpFlwBlk::ConvergeChokePt(int MaxLoops, flag LastOuterLoop)
  {
  /**/
  double Filt=1.0;
  double ld=0.0,d;
  flag Solved=0;
  flag CountExceeded = 0;
  flag WasChoked=m_fChoked;
  m_fChoked=0;
  m_ChokeInfo.m_At=NULL;
  m_ChokeInfo.m_PX=0.0;
  double ChokeQm=m_ChokeAt.m_Qm;
  for (int Loop=0; !Solved && !CountExceeded; Loop++)
    {
    d=m_ChokeAt.m_Qm-ChokeQm;
    if (d*ld < 0.0)
      Filt*=0.5;
    ld=d;
    ChokeQm+=d*Filt;

    //    Set_Qm(FNB, PressDir*ChokeQm, true, DbgFlags);
    Traverse("ChokeCnv");
    Solved = (fabs(ChokeQm-m_ChokeAt.m_Qm) < 1.0e-6*ChokeQm+1.0e-6);

#if (dbgFlwNets)
    if (dbgChokePt)
      dbgpln("ChokeHi  %#11g %#11g %s",
      m_ChokeAt.m_Qm, m_ChokeAt.m_Dp, m_Lnks.GetHead()->Nd()->FullObjTag());
#endif

    CountExceeded = (Loop >= MaxLoops);

    //CNM temp out for MG Compressor
    //if (CountExceeded && LastOuterLoop)
    //  pStart->Debug |= ( 0 ? DBG_Traverse : DBG_TrvResult) | DBG_CalcdQ | DBG_TrvErrors | DBG_ApplydQ;

    if (CountExceeded)
      dbgpln("------ Choke Converge Count[%i] Exceeded:%s ------", Loop, m_Lnks.GetHead()->Nd()->FullObjTag());
    }
  m_fChoked=(m_PressDir*m_P_Err > 0.0);
  m_ChokeInfo.m_QmHi=m_ChokeAt.m_Qm;
  m_ChokeInfo.m_DpHi=m_ChokeAt.m_Dp;
  double PErr1=m_P_Err;

  //  m_ChokeInfo.m_QmLo=Choke.Qm*0.99;
  //  Set_Qm(PressDir*m_ChokeInfo.m_QmLo, Net, pStart, DbgFlags);
  //  Traverse(pStart, DbgFlags,"ChokeLo");
  //  double PErr2=m_P_Err;
  //  m_ChokeInfo.m_DpLo=m_ChokeInfo.m_DpHi-fabs(PErr1-PErr2);

#if (dbgFlwNets)
  if (dbgChokePt)
    {
    //    dbgpln("ChokeLo  %#11g %#11g %s",
    //           m_ChokeInfo.m_QmLo, m_ChokeInfo.m_DpLo, pStart->FullObjTag());
    dbgpln("%s  Err:%+#11g App:%#11g B:%#11g %s", m_fChoked?"fChoked  ":"FreeFlow",
      m_P_Err*m_PressDir, m_P_Applied, m_Sum_B, m_Lnks.GetHead()->Nd()->FullObjTag());
    }
#endif
  if (m_fChoked)
    {
    m_ChokeInfo.m_At=m_ChokeAt.m_At;
    m_ChokeInfo.m_PX=fabs(m_P_Err);
    Traverse("ChokeSet");
    }
  /**/
  }

//-------------------------------------------------------------------------

class QmFnd : public MRootFinderBase
  {
  protected:
    CFullNetBlk&     FNB;
    CGrpFlwBlk *     pGrp;
  public:
    QmFnd(CFullNetBlk &FNB_, CGrpFlwBlk * pGrp_, CToleranceBlock & Tol) : //double Tol) :
        FNB(FNB_),
          MRootFinderBase("QfFnd", Tol)
          { pGrp=pGrp_; bLogErrors=0; };
        LPCTSTR ObjTag() { return (LPCTSTR)pGrp->FwdTag(); };
        double Function(double x)
          {
          pGrp->SetLinkQmEstAndQm(x);
          pGrp->Traverse("CnvgFun");
#if dbgFlwNets
          if (dbgConvergeNoDerivs())
            dbgpln("   Cnv %20.12g %20.12g", x, pGrp->m_P_Err);
#endif
          return pGrp->m_P_Err;
          };
        bool Converged(double X1, double X2, double F1, double F2) { return pGrp->m_fGFTolsOK!=0; };

  };

//-------------------------------------------------------------------------

void CGrpFlwBlk::ConvergeQm( int MaxLoops, flag FirstOuterLoop, flag LastOuterLoop)
  {
  SCDTRY
    {
    if (m_fHasDbgBrk)
      BreakPoint();

#if (dbgFlwNets)
    bool DoDbgIndent= (DbgFlags & (DBG_TrvResult|DBG_TraverseP|DBG_TraverseQ))!=0;
    if (DoDbgIndent)
      dbgindent(2);
#endif
    bool DerivConvergeFailed=false;
    if (m_fFindStaticHead && TestAndSolvePartialStaticHead(m_iInitialFlwDirn))
      {
      m_fGFTolsOK=true;
      }
    else if (!m_fDerivsBad)
      {
      double aQm, pdQ=0.0, dQ=0.0;
      bool   DropOut = false;
      bool   CountExceeded = false;
      int    Reversals=0;

      m_fGFTolsOK=false;
      m_CnvFilter=Min(m_CnvFilter*1.01, 1.0);
      flag   fQmPrevGood=false;
      double dQmPrev=0.0;
      double QmInPrev=m_QmIn;

      const int MaxQmHist=4;
      int nQmHist=0;
      double QmHist[MaxQmHist];

      for (int Loop=0; !m_fGFTolsOK && !DropOut && !CountExceeded; Loop++)
        {
        aQm=fabs(m_QmIn);
#if (dbgFlwNets)
        if ((Loop >= MaxLoops-10) && LastOuterLoop && dbgAutoDbgBrkOnCvgFail())
          {
          m_fHasDbgBrk=true;
          DbgFlags |= DBG_CalcdQ;
          }

        if (dbgCnvFilter())
          {
          if (pdQ * dQ < -1.0e-150)
            {
            Reversals++;
            if (Reversals > 3)
              {
              m_CnvFilter *= 0.8;
              m_CnvFilter = Max(0.01, m_CnvFilter);
              Reversals=0;
              if (true || (DbgFlags & DBG_CalcdQ))
                dbgpln("CnvFilter %#11g : %s", m_CnvFilter, FwdTag());
              }
            }
          }
#endif
        pdQ=dQ;

        m_fChoked=((!m_fChoked && (aQm > m_ChokeAt.m_Qm)) ||
          (m_fChoked && (m_ChokeInfo.m_PX+m_PressDir*m_P_Err > -1.0)));

        if (m_fChoked)
          {
          dQ = Sign(m_QmIn)*m_ChokeAt.m_Qm-m_QmIn;
          m_QmIn += dQ*m_CnvFilter;
          SetLinkQmEstAndQm(m_QmIn);
#if (dbgFlwNets)
          if (DbgFlags & DBG_CalcdQ)
            dbgpln("Choke Q   %#11g %#11g %#11g %s", m_QmIn, m_ChokeAt.m_Dp, m_ChokeInfo.m_PX, FwdTag());
#endif
          m_ChokeInfo.m_At=m_ChokeAt.m_At;
          m_ChokeInfo.m_PX+=(m_PressDir*m_P_Err);
          Traverse("CvgChok");
          }
        else
          {
          m_ChokeInfo.m_PX=0.0;
          double QmInMem=m_QmIn;
          bool LowestPOK=(m_P_Lowest >= 0.5);

          double Sum_dPdQ = m_Sum_adPqdQ - m_Sum_dPbdQ;

          // Damp Sigma if low pressure < Threshold
          double SigScale=1.0+(LowPressDamp-1.0)*GEZ(LowPressThreshold-Max(LowPressure, m_P_Lowest))/LowPressThreshold;
          double dQUnLim=(fabs(Sum_dPdQ) > 0.1*SmallPosFlow ? (m_Sum_dP + m_P_Applied) / (SigScale*Sum_dPdQ) : 0.0);

          double dQMax=Max(SmallPosFlow*0.1, aQm * MaxdQMult);
          double dQLim=Sign(dQUnLim)*Min(fabs(dQUnLim), dQMax);
          dQ= (FlwExists(aQm) ? dQLim : 1.0);//dQUnLim);
          double QMax=m_ChokeAt.m_Qm*0.999999;

          fQmPrevGood=true;
          dQmPrev=dQ*m_CnvFilter;
          QmInPrev=m_QmIn;

          m_QmIn+=dQ*m_CnvFilter;
          m_QmIn=Range(-QMax, m_QmIn, QMax);

          if (m_fFindStaticHead)
            {
            nQmHist=Min(nQmHist+1, MaxQmHist);
            for (int i=nQmHist-1; i>0; i--)
              QmHist[i]=QmHist[i-1];
            QmHist[0]=m_QmIn;
            if (nQmHist>=2)
              {
              // test for oscillation
              if (QmHist[0]*QmHist[1]<0)// && QmHist[1]*QmHist[2]<0)
                {
                if (TestAndSolvePartialStaticHead(m_iInitialFlwDirn))
                  {
                  m_fGFTolsOK=true;
                  };
                }
              }
            }

#if (dbgFlwNets)
          if (DbgFlags & DBG_CalcdQ)
            {
            dbgpln("Calc  Q %s %#11g>%#11g %+#11g -((%#11g-%#11g)/(%5.2f*%#11g)) %s",
              (dQ != dQUnLim ? "*" : " "),
              QmInMem, m_QmIn, dQ,
              m_Sum_dP, m_P_Applied,
              SigScale, Sum_dPdQ,
              FwdTag());
            }
#endif

          if (Loop>0 && fabs(dQ)/GTZ(aQm) < SignificanceRatio) // CNM
            {
            m_fGFTolsOK = true;
#if (dbgFlwNets)
            if (DbgFlags & DBG_ApplydQ)// || true)
              dbgpln("InSignificant dQ  %#16.9e / %#16.9g %s", dQ, m_QmIn, FwdTag());
#endif
            }
          else
            {
            SetLinkQmEstAndQm(m_QmIn);
            Traverse("CvgFree");
            if (!m_fGFTolsOK && FirstOuterLoop)
              DropOut=fabs(dQ)/GTZ(fabs(QmInMem)) < 0.1; // if less than 10% error drop out of loop now
            }
          }

        CountExceeded = (Loop >= MaxLoops);                                          
        /** CNM temp out for MG Compressor
        if (CountExceeded && LastOuterLoop)
        pHead->Debug |= ( 1 ? DBG_Traverse : DBG_TrvResult) | DBG_CalcdQ | DBG_TrvErrors | DBG_ApplydQ;
        **/
        if (CountExceeded && LastOuterLoop)
          dbgpln("------ Line Converge Count[%i : Q:%16.9g, pdQ:%16.9g, dQ:%16.9g %5.2f%%] Exceeded:%s ------", Loop, m_QmIn, pdQ, dQ, 100.0*dQ/NZ(pdQ), FwdTag());
        }
      DerivConvergeFailed=CountExceeded;
      }
    if (m_fDerivsBad || DerivConvergeFailed)
      {
      m_fGFTolsOK=0;

      double QmStart=m_QmIn;
      double aQm=fabs(m_QmIn);
      int FlwDir=0;
      double Qm1, Qm0;
      double Err1, Err0;

#if dbgFlwNets
      if (dbgConvergeNoDerivs())
        dbgpln("====================");
#endif

      flag GotStraddle=0;
      if (aQm>100.0*SmallPosFlow)
        {
        // Try at +-1% then +-10% .
        double Diff=0.01;
        for (int i=0; i<=2 && !GotStraddle; i++, Diff*=10.0)
          {
          Qm0=Sign(QmStart)*Max(SmallPosFlow, (1.0-Diff)*fabs(QmStart));
          SetLinkQmEstAndQm(Qm0);
          Traverse("CnvgSt1");
          Err0=m_P_Err;
#if dbgFlwNets
          if (dbgConvergeNoDerivs())
            dbgpln("Lo    %14.6g %14.6g", Qm0, m_P_Err);
#endif

          Qm1=(1.0+Diff)*QmStart;
          SetLinkQmEstAndQm(Qm1);
          Traverse("CnvgSt2");
          Err1=m_P_Err;
#if dbgFlwNets
          if (dbgConvergeNoDerivs())
            dbgpln("Hi    %14.6g %14.6g", Qm1, m_P_Err);
#endif
          GotStraddle= (Err0*Err1<0.0);
          }
        }

      if (!GotStraddle)
        {
        SetLinkQmEstAndQm(SmallPosFlow);
        Traverse("CnvgSt3");
        if (m_Sum_B+(m_P_Applied) >=0.0)
          FlwDir=1;
        else
          {
          SetLinkQmEstAndQm(-SmallPosFlow);
          Traverse("CnvgSt4");
          if (m_Sum_B+(m_P_Applied) <=0.0)
            FlwDir=-1;
          }

#if dbgFlwNets
        if (dbgConvergeNoDerivs())
          dbgpln("FlwDir %i", FlwDir);
#endif
        if (FlwDir != 0)
          {
          QmStart=fabs(QmStart);
          if (QmStart <= SmallPosFlow)
            QmStart=100.0;

          Qm0=SmallPosFlow*FlwDir;
          Err0=m_P_Err;
#if dbgFlwNets
          if (dbgConvergeNoDerivs())
            dbgpln("Zero   %14.6g %14.6g", SmallPosFlow*FlwDir, m_P_Err);
#endif
          Qm1=QmStart*FlwDir;

          for (;;)
            {
            SetLinkQmEstAndQm(Qm1);
            Traverse("CnvgSt5");
            Err1=m_P_Err;
#if dbgFlwNets
            if (dbgConvergeNoDerivs())
              dbgpln("Srch   %14.6g %14.6g", Qm1, m_P_Err);
#endif
            if (Err1*Err0 <0.0)
              {
              GotStraddle=true;
              break;
              }
            if (Err1*Err0 ==0.0)
              {
              //GotStraddle=true;
              break;
              }

            Qm0=Qm1;
            Err0=Err1;

            Qm1*=2.0;

            if (fabs(Qm1)>1.0e10)
              break;
            }
          }
        }


      if (GotStraddle)
        {
        QmFnd QF(*m_pFNB, this, m_pFNB->m_QmFndEPS);//0.1*pFNB->Lnk_Eps_A);
        QF.SetTarget(0.0);

        if (QF.Start(Qm0, Qm1, Err0, Err1)==RF_OK)
          {
          if (QF.Solve_Brent()==RF_OK)
            {
            m_fGFTolsOK=true;
            m_fSetCI6=false;
            m_fSetCI7=false;
            }
          else
            {
            m_fSetCI6=true;
            dbgpln("SetCI6 %s", FwdTag());
            }
          }
        else
          {
          m_fSetCI7=true;
          dbgpln("SetCI7 %s", FwdTag());
          ASSERT_ALWAYS(0, "Could not Start LineConvergence"); // could not Start - Should never occur
          }
        m_fSetCI8=false;
        }
      else if (Err0==0)
        {
        SetLinkQmEstAndQm(Qm0);
        Traverse("CnvgSetQm0");
        //Err0=m_P_Err;
        }
      else if (Err1==0)
        {
        SetLinkQmEstAndQm(Qm1);
        Traverse("CnvgSetQm1");
        //Err1=m_P_Err;
        }
      else

        {
        //      if (Pass==0)
        //        {
        //        JoinIter L(m_Lnks);
        //        for (CJoinRec * p = L.First(); p; p = L.Next())
        //          {
        //          p->SetQmVAccum(0.0);
        //          p->SetQmLAccum(0.0);
        //          }
        //        goto ReStart;
        //        }
        //      else

        m_fSetCI8=true;
        dbgpln("SetCI8 %s", FwdTag());
        }
      }
#if (dbgFlwNets)
    if (DoDbgIndent)
      dbgindent(-2);
#endif
    }
  SCDCATCH("CGrpFlwBlk::Converge", FwdTag());
  }

//#else
//
//
//#endif
//-------------------------------------------------------------------------




//-------------------------------------------------------------------------

class VVolFinder : public MRootFinderBase
  {
  protected:
    CFullNetBlk&     FNB;
    CGrpFlwBlk *     pGrp;
    int             Dirn;
  public:
    VVolFinder(CFullNetBlk &FNB_, CGrpFlwBlk * pGrp_, int Dirn_) : //double Tol) :
        FNB(FNB_),
          MRootFinderBase("RhoHFinder", FNB_.m_VVolFndEPS)
          {
          pGrp=pGrp_;
          Dirn=Dirn_;
          bLogErrors=0;
          };
        LPCTSTR ObjTag() { return (LPCTSTR)pGrp->FwdTag(); };
        double Function(double x)
          {
          pGrp->m_pBalProps->m_dLVolFracBal=x;
          pGrp->SetLinkQmEstAndQm(Dirn*SmallPosFlow);
          pGrp->Traverse("AdjRhoH");

          //dbgpln("RhoHFinder %8.6f %16.8f %16.8f %16.8f", x, pGrp->m_P_Applied, pGrp->m_Sum_dP, pGrp->m_P_Applied+pGrp->m_Sum_dP);
          return (pGrp->m_P_Applied+pGrp->m_Sum_dP);
          };
  };

//-------------------------------------------------------------------------

flag CGrpFlwBlk::TestAndSolvePartialStaticHead(int StartDirn)
  {
  double A=m_P_Applied;

  double MemQmIn=m_QmIn;
  double MemQmEstIn=m_QmEstIn;
  double MemLVolFracBal=m_pBalProps->m_dLVolFracBal;
  m_pBalProps->m_dLVolFracBal=0;

  SetLinkQmEstAndQm(-StartDirn*SmallPosFlow);
  Traverse("TstSmal1");
  double DPPos=m_Sum_dP;

  SetLinkQmEstAndQm(+StartDirn*SmallPosFlow);
  Traverse("TstSmal2");
  double DPNeg=m_Sum_dP;

  m_pBalProps->m_dLVolFracBal=MemLVolFracBal;

  double ErrP=(A+DPPos);
  double ErrN=(A+DPNeg);

  flag FindRhoH=(ErrP*ErrN<0);

#if (dbgFlwNets)
  if (dbgPartialStaticHead() && (!dbgTraverseDbgBrk() || m_fHasDbgBrk))
    dbgpln("--> PartialStaticHead: %s A:%10.4f, DpP:%10.4f DpN:%10.4f ErrP:%10.4f ErrN:%10.4f %s",
    FindRhoH?"FIND":"   ", A, DPPos, DPNeg, ErrP, ErrN, FwdTag());
#endif

  // if the errors are different signs adjust rhoH at low flow
  bool RetOK=false;
  int C21OK=true;
  int C22OK=true;
  if (FindRhoH)
    {
    VVolFinder VF(*m_pFNB, this, StartDirn);//, 0.001);
    VF.SetTarget(0.0);

    if (VF.Start(-m_pBalProps->m_dLVolFrac, 1-m_pBalProps->m_dLVolFrac)==RF_OK)
      {
      if (VF.Solve_Brent()==RF_OK)
        RetOK=true;
      else
        {
        C21OK=false;
        m_pBalProps->m_dLVolFracBal=0;
        }
      }
    else
      {
      C22OK=false;
      m_pBalProps->m_dLVolFracBal=0;
      }
    }
  else
    {
    SetLinkQmEstAndQm(MemQmEstIn);
    Traverse("Restore");
    }
  m_fSetCI21=!C21OK;
  m_fSetCI22=!C22OK;
  return RetOK;
  };

//--------------------------------------------------------------------------

double CFlwRegBlk::FinalTarget()
  {
  double Rqd=0;
  switch (m_iWhat)
    {
    case FBReg_Pi     : Rqd = m_dPiReqd; break;
    case FBReg_Po     : Rqd = m_dPoReqd; break;
    case FBReg_Qm     : Rqd = m_dQmReqd; break;
    case FBReg_Qv     : Rqd = m_dQvReqd; break;
    case FBReg_QvStep : Rqd = m_dQvReqd; break;
    case FBReg_Off    : Rqd = 0.0; break;
    }
  return Rqd;
  };

double CFlwRegBlk::StepTarget()
  {
  double Rqd=0, Start=0;
  switch (m_iWhat)
    {
    case FBReg_Pi     : Rqd = m_dPiReqd; Start = m_dPiStart; break;
    case FBReg_Po     : Rqd = m_dPoReqd; Start = m_dPoStart; break;
    case FBReg_Qm     : Rqd = m_dQmReqd; Start = m_dQmStart; break;
    case FBReg_Qv     : Rqd = m_dQvReqd; Start = m_dQvStart; break;
    case FBReg_QvStep : Rqd = m_dQvReqd; Start = m_dQvStart; break;
    case FBReg_Off    : Rqd = 0.0; Start = 0.0; break;
    }

  //double X=1.0-exp(-m_dTau/ICGetTimeInc());
  double X=1.0-Range(0.0, ICGetTimeInc()/m_dTau, 1.0);
  return Rqd+X*(Start-Rqd);
  };

void CFlwRegBlk::SetMeas(double M)
  {
  switch (m_iWhat)
    {
    case FBReg_Pi     : m_dPiMeas=(float)M; break;
    case FBReg_Po     : m_dPoMeas=(float)M; break;
    case FBReg_Qm     : m_dQmMeas=(float)M; break;
    case FBReg_Qv     : m_dQvMeas=(float)M; break;
    case FBReg_QvStep : m_dQvMeas=(float)M; break;
    case FBReg_Off    :
    default:       ;
    }
  };

void CFlwRegBlk::SetMeasStart(double M)
  {
  switch (m_iWhat)
    {
    case FBReg_Pi     : m_dPiStart=(float)M; break;
    case FBReg_Po     : m_dPoStart=(float)M; break;
    case FBReg_Qm     : m_dQmStart=(float)M; break;
    case FBReg_Qv     : m_dQvStart=(float)M; break;
    case FBReg_QvStep : m_dQvStart=(float)M; break;
    case FBReg_Off    :
    default:       ;
    }
  };

//--------------------------------------------------------------------------

double CGrpFlwRegBlk::GetMeasValue()
  {
  FlwBlk &FB=*m_pFB;
  switch (m_iWhat)
    {
    case FBReg_Qm     : m_dMeasValue = FB.m_Qm;  break;
    case FBReg_Qv     : m_dMeasValue = FB.m_Qm/FB.m_MeasRho;  break;
    case FBReg_QvStep : m_dMeasValue = FB.m_Qm/FB.m_MeasRhoStep;  break;
    case FBReg_Pi     : m_dMeasValue = FB.SrcPB().PEst;  break;
    case FBReg_Po     : m_dMeasValue = FB.DstPB().PEst;  break;
    }
  return m_dMeasValue;
  };

void CGrpFlwRegBlk:: SetFlwRegulator(double R)
  {
  m_dCtrlCur=R;
  m_pFRB->m_dCtrlCur=Range(m_pFRB->m_dCtrlMin, (float)R, m_pFRB->m_dCtrlMax);
  }

//--------------------------------------------------------------------------

class CRegSolver: public MRootFinderBase
  {
  protected:
    CFullNetBlk &    rFNB; 
    CGrpFlwBlk *     pGrp;
    CFlwRegBlk *    pFRB;
    FlwNode   *     pNd;
    int iMaxLoops;
    flag fFirstOuterLoop;
    flag fLastOuterLoop;

  public:
    CRegSolver(CFullNetBlk &FNB, CGrpFlwBlk * Grp, CFlwRegBlk *FRB, FlwNode * Nd, int MaxLoops, flag FirstOuterLoop, flag LastOuterLoop) :
        rFNB(FNB), 
          MRootFinderBase("RegulatorSolver", FNB.m_RegSolveEPS)
          {
          pGrp=Grp;
          pFRB=FRB;
          pNd=Nd;
          iMaxLoops =       MaxLoops;       
          fFirstOuterLoop = FirstOuterLoop; 
          fLastOuterLoop =  LastOuterLoop;  
          bLogErrors=0;
          };
        LPCTSTR ObjTag() { return (LPCTSTR)pGrp->FwdTag(); };
        double Function(double x)
          {
#if dbgFlwNets
          if (0 && dbgRegulators())
            dbgpln(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Regulator Ctrl = %15.6f", x);
#endif
          //dbgpln("-----Ctrl   %15.6f", x);
          pGrp->m_Regulator.SetFlwRegulator(x);
          pGrp->ConvergeQm(iMaxLoops, fFirstOuterLoop, fLastOuterLoop);

#if dbgFlwNets
          if (dbgRegulators())
            dbgpln("Cvg Ctrl:%20.11f Target:%17.6f Meas:%17.6f %17.6f %14.8f", x, dTarget, pGrp->m_Regulator.m_dMeasValue, pGrp->m_QmEstIn, pFRB->m_pFB?pFRB->m_pFB->m_MeasRho:0.0);
#endif

          return (pGrp->m_Regulator.m_dMeasValue);
          };
  };

void CGrpFlwBlk::ConvergeRegulator(int MaxLoops, flag FirstOuterLoop, flag LastOuterLoop)
  {
  byte How=m_Regulator.m_iWhat;
  if (How==FBReg_Off || m_Regulator.m_pFB==NULL || (m_Regulator.m_pFB->FlowMode()!=FBMode_Full))
    {
    return;
    }

  FlwNode &Nd     = *m_Regulator.m_pNd;
  CFlwRegBlk &FRB = *m_Regulator.m_pFRB;
  FlwBlk &FB      = *m_Regulator.m_pFB;

  if (FRB.m_bHoldCvg)
    return;

  FirstOuterLoop=false;
#if dbgFlwNets
  if (dbgRegulators())
    {
    dbgpln(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Regulator %4i %4i %4i", MaxLoops, FirstOuterLoop, LastOuterLoop);
    dbgindent(2);
    }
#endif

  flag OK=false;
  if (0) // faster - not reliable
    {
    double Q=m_QmIn;
    double Ctrl=m_Regulator.m_dCtrlCur;
    Ctrl=Range(FRB.CtrlMin(), Ctrl, FRB.CtrlMax());
    //ConvergeQm(MaxLoops, FirstOuterLoop, LastOuterLoop);
    flag IgnoreRegTest=false;
    const long MaxIters=100;
    for (long iIter=0; iIter<MaxIters; iIter++)
      {
      SetLinkQmEstAndQm(Q);
      m_Regulator.SetFlwRegulator(Ctrl);
      Traverse("Reg1");
      double DErr=m_P_Err;
      double RErr=m_Regulator.m_dMeasValue-FRB.StepTarget();
      if (m_fGFTolsOK && (m_fGFRegTolOK || IgnoreRegTest))
        break;

      double DCtrl=(Ctrl>0.5 ? -1:1)*0.001;

      SetLinkQmEstAndQm(Q);
      m_Regulator.SetFlwRegulator(Ctrl+DCtrl);
      Traverse("Reg2");
      double DErr1=m_P_Err;
      double RErr1=m_Regulator.m_dMeasValue-FRB.StepTarget();

      double DQ=NZ(Q)*0.001;
      SetLinkQmEstAndQm(Q+DQ);
      m_Regulator.SetFlwRegulator(Ctrl);
      Traverse("Reg3");
      double DErr2=m_P_Err;
      double RErr2=m_Regulator.m_dMeasValue-FRB.StepTarget();

      double dDdC=(DErr1-DErr)/DCtrl;
      double dRdC=(RErr1-RErr)/DCtrl;
      double dDdQ=(DErr2-DErr)/DQ;
      double dRdQ=(RErr2-RErr)/DQ;

      double C=dRdC/NZ(dRdC*dDdQ-dRdQ*dDdC);
      double D=dDdC/NZ(dDdC*dRdQ-dDdQ*dRdC);
      double A=dRdQ/NZ(dRdQ*dDdC-dRdC*dDdQ);
      double B=dDdQ/NZ(dDdQ*dRdC-dDdC*dRdQ);

      double Scl=1.0;
      double ReqdQChg=-Scl*(DErr*C+RErr*D);
      double ReqdCChg=-Scl*(DErr*A+RErr*B);

      double CDiff=ReqdCChg;
      bool OutOfRange=false;
      if (ReqdCChg<0.0)
        {
        if (Ctrl==FRB.CtrlMin())
          OutOfRange=true;
        else
          ReqdCChg=Max(FRB.CtrlMin()-Ctrl, ReqdCChg);
        }
      else if (ReqdCChg>0.0)
        {
        if (Ctrl==FRB.CtrlMax())
          OutOfRange=true;
        else
          ReqdCChg=Min(ReqdCChg, FRB.CtrlMax()-Ctrl);
        }
      //else
      //  ReqdCChg=Range(FRB.CtrlMin()-Ctrl, ReqdCChg, FRB.CtrlMax()-Ctrl)-ReqdCChg;

      CDiff=ReqdCChg-CDiff;

      dbgpln("ChgCon DQ:%14.6e  DC:%14.6e  CDiff:%14.6e ", ReqdQChg, ReqdCChg, CDiff);

      if (OutOfRange)
        {
        SetLinkQmEstAndQm(Q);
        m_Regulator.SetFlwRegulator(Ctrl);
        break;
        }
      else if (fabs(CDiff)>1.0e-30)
        {
        if (fabs(dRdQ)<1e-30)
          {
          ReqdQChg=-Scl*DErr/NZ(dDdQ);
          //ReqdCChg=0;
          }
        else
          {
          ReqdQChg+=-Scl*(CDiff-RErr*B)*C/NZ(A)+RErr*D;
          //ReqdCChg+=CDiff;
          }
        if (fabs(ReqdCChg)<1.0e-10)
          {
          IgnoreRegTest=true;
          ReqdQChg=-Scl*(DErr/NZ(dDdQ));
          if (fabs(ReqdQChg)>1000)
            { 
            int xxx=0;
            }
          }
        }
      else
        {
        int xxx=00;
        }

      //      double CChgNew=Range(FRB.CtrlMin()-Ctrl, ReqdCChg, FRB.CtrlMax()-Ctrl);//-ReqdCChg;
      //      if (fabs(CChgNew-ReqdCChg)>1.0e-30)
      //        {
      //        double Ratio=fabs(CChgNew)/GTZ(fabs(ReqdCChg));
      //        RErr*=Ratio;
      //
      //        ReqdQChg=-Scl*(DErr*C+RErr*D);
      //        ReqdCChg=-Scl*(DErr*A+RErr*B);
      //
      ////        ReqdQChg+=-Scl*(CErr-RErr*B)*C/NZ(A)+RErr*D;
      ////        ReqdCChg+=CErr;
      //        if (fabs(ReqdCChg)<1.0e-10)
      //          {
      //          IgnoreRegTest=true;
      //          //ReqdQChg=-Scl*(DErr/NZ(dDdQ));
      //          if (fabs(ReqdQChg)>100)
      //            { int xxx=0;}
      //          }
      //        }
      //
      Q+=ReqdQChg;
      Ctrl+=ReqdCChg;
      if (fabs(Q)>1.0e6)
        { int XXX=0; }
#if (dbgFlwNets)
      if (iIter>=MaxIters-10)
        { 
        dbgTraverseResult.bOn=1;
        //dbgTraverseP.bOn=1;
        int xxx=0; 
        }
#endif
      }
    if (iIter>=MaxIters)
      {
      LogError("Solver", 0, "Too Many ConvergeRegulator Iterations");
      }
    }
  else
    {
    CRegSolver RS(*m_pFNB, this, m_Regulator.m_pFRB, m_Regulator.m_pNd, MaxLoops, FirstOuterLoop, LastOuterLoop);
    RS.SetTarget(FRB.StepTarget());
    RS.SetLimits(FRB.CtrlMin(), FRB.CtrlMax());
    if (/*false &&*/ Valid(m_Regulator.m_pFRB->m_dCtrlCur)) // ForKCG - maybe set true to false
      {
      int Sgn=1;
      switch (FRB.What())
        {
        case FBReg_Pi : Sgn=-1;/**Sign(FB.Qm)*/; break;
        case FBReg_Po : Sgn=+1;/**Sign(FB.Qm)*/; break;
        case FBReg_Qm :                        
        case FBReg_Qv :                        
        case FBReg_QvStep :                        
        default:        Sgn=1; break;
        }
      if (RS.SolveFromEst(m_Regulator.m_pFRB->m_dCtrlCur, Sgn, 0.10, 0.001)==RF_OK)
        {
        OK=true;
        }
      }
    if (!OK)
      {
      if (RS.Start(FRB.CtrlMin(), FRB.CtrlMax())==RF_OK)
        {
        if (RS.Solve_Brent()==RF_OK)
          {
          OK=true;
          }          
        else
          {
#if dbgFlwNets
          if (dbgRegulators())
            dbgpln("Regulator not Solved %s", FwdTag());
#endif       
          }
        }
      else
        {
#if dbgFlwNets
        if (dbgRegulators())
          dbgpln("Out Of Range %s", FwdTag());
#endif
        }
      }
    }

  m_Regulator.m_fSetCI18=!OK && ((m_Regulator.m_pFRB->m_iShowWhat & FBRShow_Msg)!=0);

#if dbgFlwNets
  if (dbgRegulators())
    {
    dbgindent(-2);
    dbgpln("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
    }
#endif
  };

//==========================================================================
//
//
//
//==========================================================================

CGlblStopWatch gs_swTotalExecTicks     (gs_swFlwSolve, "Net", "TotalExecTicks",   0);
CGlblStopWatch gs_swMeasureTicks       (gs_swFlwSolve, "Net", "Measure",          1, (CGlblStopWatch::eShowDbg|CGlblStopWatch::eShowAccess|CGlblStopWatch::eShowTotal));
CGlblStopWatch gs_swConvergeTicks      (gs_swFlwSolve, "Net", "Converge",         2);
CGlblStopWatch gs_swSolveTicks         (gs_swFlwSolve, "Net", "Solve",            3);
CGlblStopWatch gs_swLoadSurgeTicks     (gs_swFlwSolve, "Net", "LoadSurge",        4);
CGlblStopWatch gs_swCalcIndicesTicks   (gs_swFlwSolve, "Net", "CalcIndices",      5);
CGlblStopWatch gs_swMtxBuildTicks      (gs_swFlwSolve, "Net", "MtxBuild",         6);
CGlblStopWatch gs_swMtxSolveTicks      (gs_swFlwSolve, "Net", "MtxSolve",         7);
CGlblStopWatch gs_swMtxExtractTicks    (gs_swFlwSolve, "Net", "MtxExtract",       8);
CGlblStopWatch gs_swChkJoinsTicks      (gs_swFlwSolve, "Net", "ChkJoins",         9);
CGlblStopWatch gs_swEvalJoinQTicks     (gs_swFlwSolve, "Net", "EvalJoinQ",       10);
CGlblStopWatch gs_swEvalJoinPTicks     (gs_swFlwSolve, "Net", "EvalJoinP",       11);
CGlblStopWatch gs_swSolveFxdFlwQTicks  (gs_swFlwSolve, "Net", "SolveFxdFlwQ",    12);
CGlblStopWatch gs_swSolveFxdFlwPTicks  (gs_swFlwSolve, "Net", "SolveFxdFlwP",    13);
CGlblStopWatch gs_swFixTopoTicks       (gs_swFlwSolve, "Net", "FixTopology",     14);
CGlblStopWatch gs_swOverHeadTicks      (gs_swFlwSolve, "Net", "OverHead",        15);
CGlblStopWatch gs_swTotalDerivative    (gs_swFlwSolve, "Net", "TotalDerivative", 16);

CSubNetBlk::CSubNetBlk(): m_SS(0)
  {
  m_Lp0=0; 
  m_LpN=2; 
  fClearSS=true;
  Clear(1, NULL, NULL);
  }

//--------------------------------------------------------------------------

CSubNetBlk::~CSubNetBlk()
  {
  Clear(-1, NULL, NULL);
  }

//--------------------------------------------------------------------------

void CSubNetBlk::Clear(int SubNetNo_, CNetBlk* pNet_, CFullNetBlk *pFNB_)
  {
  m_pFirstJoin=NULL;
  pNet = pNet_;
  pFNB=pFNB_;
  m_SubNetNo=SubNetNo_;
  SetFullFlow(true); 

  GrpFlwIter G(Grps);
  for (CGrpFlwBlk * g=G.First(); g; g=G.Next())
    {
    g->m_fIsBad=false;
    pNet->FreeGFB(g);
    }

  Grps.RemoveAllToFreeList();
  m_RegGrps.RemoveAllToFreeList();
  Ties.RemoveAllToFreeList();
  Joins.RemoveAllToFreeList();

  m_FxdPList.RemoveAllToFreeList();
  m_FxdQList.RemoveAllToFreeList();
  m_UDFFBs.RemoveAllToFreeList();

  iMtxOrder             = 0;
  fClearSS              = true;
  fMustAnalyse          = false;
  fSolved               = false;
  fNetTolsOK            = false;
  fLonely               = false;
  fUnDefined            = false;
  fIsolatedNode         = false;
  fIsolatedNet          = false;
  fFixed                = false;
  fXfer                 = false;
  fXferTies             = false;
  fNetWrk               = false;

  fTopologyChanged      = false;

  fIndexCalculationReqd = true;
  fMomentumValid        = false;
  }

//-------------------------------------------------------------------------

inline char * FxdStrng(flag fXfer) { return fXfer ? "Xfer " : "Fixed"; };
inline char * FxdStr(flag fXfer) { return fXfer ? "Xfr" : "Fxd"; };

static CString DbgSrcDstPB(CPressBlk &SrcPB, CPressBlk &DstPB)
  {
  CString X;
  X.Format("sp[%4i,%4i] [%10s %10s %10s] dp[%4i,%4i] [%10s %10s %10s] ",
    SrcPB.m_iNo,SrcPB.m_iFxdPOrder, 
    DbgFltString(SrcPB.P,-1,3),
    DbgFltString(SrcPB.PEst,-1,3),
    DbgFltString(SrcPB.PMax,-1,3),
    DstPB.m_iNo,DstPB.m_iFxdPOrder,
    DbgFltString(DstPB.P,-1,3),
    DbgFltString(DstPB.PEst,-1,3),
    DbgFltString(DstPB.PMax,-1,3));
  return X;
  }

static CString DbgOnePB(CPressBlk &PB, bool All)
  {
  CString X;
  X.Format("pb[%4i,%4i] [%10s %10s %10s]",
    PB.m_iNo,PB.m_iFxdPOrder,
    true||All ? DbgFltString(PB.P,-1,3):"",
    All ? DbgFltString(PB.PEst,-1,3):"",
    DbgFltString(PB.PMax,-1,3));
  return X;
  }
static CString DbgOnePBHd()
  {
  CString X;
  X.Format("pb[%4s,%4s] [%10s %10s %10s]", "Id", "POrd", "P", "PEst", "PMax");
  return X;
  }

static CString DbgFwdRev(flag Fwd)
  {
  return Fwd ? "Fwd":"Rev";
  }

//-------------------------------------------------------------------------

void CSubNetBlk::FindFxdFBPEstRqd( CJoinGFBPtr & JG)
  {
  FlwBlk &FB = *JG.FB();
  //flag GoFwd=JG.Forward();
  flag GoFwd=FB.SrcPB().m_iFxdPOrder<=FB.DstPB().m_iFxdPOrder;
  if (FB.GetFixed())
    {
    CPressBlk & SrcPB=FB.SrcPB(GoFwd);
    CPressBlk & DstPB=FB.DstPB(GoFwd);

    SrcPB.PMax=Valid(SrcPB.PMaxLim) ? SrcPB.PMaxLim: fNAN;
    DstPB.PMax=Valid(DstPB.PMaxLim) ? DstPB.PMaxLim: fNAN;

#if dbgFlwNets
    if (dbgSolveFxdPs())
      {
      Strng S;
      dbgpln("  %-3s fb[%4i,%4i] %-3s %15s %10s %5i %s %s", 
        "Fxd", FB.m_iNo, FB.m_iFxdPOrder, DbgFwdRev(GoFwd), DbgFltString(FB.RequiredQm(),3,2), "---", 
        FB.m_iFxdPOrder, DbgOnePB(SrcPB, false), JG.Tag(S));
      dbgpln("                                                         %s ", 
        DbgOnePB(DstPB, false));

      }
#endif
    }
  else
    {
    CPressBlk & SrcPB=FB.SrcPB(GoFwd);
    CPressBlk & DstPB=FB.DstPB(GoFwd);

    DstPB.PMax=Valid(SrcPB.PMaxLim)?SrcPB.PMaxLim:SrcPB.PMax;
    if (Valid(DstPB.PMaxLim))
      DstPB.PMax=Valid(DstPB.PMax)?Min(DstPB.PMaxLim, DstPB.PMax):DstPB.PMax;

#if dbgFlwNets
    if (dbgSolveFxdPs())
      {
      dbgpln("  %-3s fb[%4i,%4i] %-3s %15s %10.3f %5i %s %s",
        "FB", FB.m_iNo, FB.m_iFxdPOrder, DbgFwdRev(GoFwd), DbgFltString(FB.RequiredQm(),3,2), FB.m_DPEstFxdQm, 
        FB.m_iFxdPOrder, DbgOnePB(SrcPB, false), JG.Nd()->Tag());
      dbgpln("                                                         %s ",DbgOnePB(DstPB, false));
      }
#endif
    }
  }

//-------------------------------------------------------------------------

void CSubNetBlk::FindFxdJoinPEstRqd(CJoinGFBPtr & JG)
  {
  CJoinRec & Jn= *JG.Join();
  FlwNode &Nd = *Jn.Nd();
  int iSrcConn= -1;
  int HiOrd   = -1;
  int iSrcIO  = -1;
  for (int c=0 ; c<Jn.m_nConns; c++)     
    {
    if (Jn.IsIO(c))
      {
      for (int Fe=0; Fe<Jn.NIOFBs(c); Fe++)
        {
        FlwBlk &FB=*Jn.IOFB(c, Fe);
        if (FB.m_iFxdPOrder>HiOrd)
          {
          HiOrd=FB.m_iFxdPOrder;
          iSrcConn=c;
          }
        }
      iSrcIO=Jn.IONo(iSrcConn);
      }
    else
      {
      FlwBlk & FB=*Jn.LinkFB(c);
      if (FB.m_iFxdPOrder>HiOrd)
        {
        HiOrd=FB.m_iFxdPOrder;
        iSrcConn=c;
        }
      }
    }

  double PMin = -1.0;
  for (c=0 ; c<Jn.m_nConns; c++)
    {
    if (c!=iSrcConn)
      {
      CPressBlk & PB=*Jn.PB_Self(c);
      double Px=PB.PMax;//Nd.IOP_Max_Self(c);
      if (Valid(Px))
        PMin = Min(PMin, Px);
#if dbgFlwNets
      if (dbgSolveFxdPs())
        {
        dbgpln("  %-3s %13s %-3s %15s %10s %5s %s %s < %s", 
          "Cn<", "", "", "", "", "", DbgOnePB(PB,false), JG.Nd()->Tag(), JG.Nd()->Nd_Rmt(c)->Tag());
        }
#endif
      }
    }

  Jn.PB()->PMax=(PMin>=0.0 ? (float)PMin: fNAN);
  Jn.PB_Self(iSrcConn)->PMax=Jn.PB()->PMax;

#if dbgFlwNets
  if (dbgSolveFxdPs())
    {
    dbgpln("  %-3s %13s %-3s %15s %10s %5s %s %s", 
      "Jn", "", "", "", "", "", DbgOnePB(*Jn.PB(), false), JG.Nd()->Tag());
    }
#endif
  }

//-------------------------------------------------------------------------

void CSubNetBlk::SolveFxdFBP(CJoinGFBPtr & JG)
  {
  FlwBlk &FB = *JG.FB();
  //flag GoFwd=!JG.Forward();
  flag GoFwd=FB.SrcPB().m_iFxdPOrder>FB.DstPB().m_iFxdPOrder;

  CPressBlk & SrcPB=FB.SrcPB(GoFwd);
  CPressBlk & DstPB=FB.DstPB(GoFwd);

  if (FB.GetFixed())
    {
    if (Valid(SrcPB.PMax))
      SrcPB.PEst=Min(SrcPB.PEst, (double)SrcPB.PMax);
    if (Valid(DstPB.PMax))
      DstPB.PEst=Min(DstPB.PEst, (double)DstPB.PMax);
    }
  else
    {
    if (Valid(SrcPB.PMax))
      SrcPB.PEst=Min(SrcPB.PEst, (double)SrcPB.PMax);
    DstPB.PEst=SrcPB.PEst+(FB.m_DPEstFxdQm*(GoFwd?-1:1));
    if (Valid(DstPB.PMax))
      DstPB.PEst=Min(DstPB.PEst, (double)DstPB.PMax);
    }

  SrcPB.P    = Max(LowPressure, SrcPB.PEst);
  DstPB.P    = Max(LowPressure, DstPB.PEst);

#if dbgFlwNets
  if (dbgSolveFxdPs())
    {
    dbgpln("  %-3s fb[%4i,%4i] %-3s %15s %10.3f %5i %5s %s %s", 
      FB.GetFixed()?"Fxd":"FB", FB.m_iNo, FB.m_iFxdPOrder,
      DbgFwdRev(GoFwd),
      DbgFltString(FB.RequiredQm(),3,2), 
      FB.m_DPEstFxdQm, FB.m_iFxdPOrder, "", DbgOnePB(SrcPB, true), JG.Nd()->Tag());
    dbgpln("                                                               %s ", DbgOnePB(DstPB, true));
    }
#endif
  }

//-------------------------------------------------------------------------
//#endif
//-------------------------------------------------------------------------

void CSubNetBlk::SolveFxdJoinP( CJoinGFBPtr & JG)
  {
  CJoinRec & Jn= *JG.Join();
  FlwNode &Nd = *Jn.Nd();
  int iSrcConn=-1;
  int HiOrd=-1;
  for (int c=0 ; c<Jn.m_nConns; c++)
    {
    for (int fe=0; fe<Jn.NFBs(c); fe++)
      {
      if (Jn.FB(c, fe)->m_iFxdPOrder>HiOrd)
        {
        HiOrd=Jn.FB(c, fe)->m_iFxdPOrder;
        iSrcConn=c;
        }
      }
    }
  double P = Jn.PB_Self(iSrcConn)->PEst;
  Jn.m_PB.P=Max(LowPressure, P);
  Jn.m_PB.PEst=P;

#if dbgFlwNets
  if (dbgSolveFxdPs())
    {
    dbgpln("  %-3s %13s %-3s %15s %10s %5s %5i %s %s", 
      "Jn", "", "", "", "", "", iSrcConn, DbgOnePB(*Jn.PB(), true), JG.Nd()->Tag());
    }
#endif

  for (c=0 ; c<Jn.m_nConns; c++)
    {
    if (c!=iSrcConn)
      {
      CPressBlk &PB = *Jn.PB_Self(c);
      PB.P    = Max(LowPressure, P);
      PB.PEst = P;
#if dbgFlwNets
      if (dbgSolveFxdPs())
        {
        dbgpln("  %-3s %13s %-3s %15s %10s %5s %5s %s %s > %s",
        "Cn>", "", "", "", "", "", "", DbgOnePB(PB, true), JG.Nd()->Tag(), JG.Nd()->Nd_Rmt(c)->Tag());
        }
#endif
      }
    }
  }

//-------------------------------------------------------------------------

void CSubNetBlk::DumpFxdConnPs()
  {
#if dbgFlwNets
  if (dbgDumpPressBlks())
    {
    char * SepLn="---------------------------------------------------------------------------------------------------------";
    dbgpln(SepLn);
    dbgpln("Dump Fxd Ps");
    dbgpln("           %s", DbgOnePBHd());

    CJoinGFBIter Fx(m_FxdPList);
    for (CJoinGFBPtr * pc = Fx.First(); pc ; pc=Fx.Next())
      {
      Strng S0, S1;
      if (pc->IsJoin())
        {
        CJoinRec & Join = *pc->Join();
        for (int c=0; c<Join.NConns(); c++)
          {
          if (Join.IsIO(c))
            {
            dbgpln("Jn IO  %2i  %s %s{%i}[%i]", c,
              DbgSrcDstPB(*Join.IOPB_Self(c), *Join.IOPB_Flng(c)),
              Join.Nd()->Tag(),Join.m_iJoinId, c);
            }
          else
            {
            CLinkRec &L=*Join.Link(c);
            dbgpln("Jn Lnk %2i  %s %s{%i} > %s{%i}", c,
              DbgSrcDstPB(L.FB.SrcPB(), L.FB.DstPB()),
              Join.Nd()->Tag(),pc->Forward()?L.m_iJoinId0:L.m_iJoinId1,
              Join.Nd()->Tag(),pc->Forward()?L.m_iJoinId1:L.m_iJoinId0);
            }
          }
        }
      else
        {
        FlwBlk &FB = *pc->FB();
        Strng Tmp;
        dbgpln("FB         %s %s", DbgOnePB(FB.SrcPB(pc->Forward()), true), pc->Tag(Tmp));
        dbgpln("           %s",    DbgOnePB(FB.DstPB(pc->Forward()), true));
        }
      }
    dbgpln(SepLn);
    }
#endif
  }

//-------------------------------------------------------------------------

void CSubNetBlk::SolveFxdPs()
  {
  CStopWatchLap SWLapSolveFxdFlwPTicks(gs_swSolveFxdFlwPTicks);

#if dbgFlwNets
  DumpFxdConnPs();
  char * SepLn="=========================================================================================================";
  if (dbgSolveFxdPs())
    {
    dbgpln(SepLn);
    dbgpln("FindFxdPs");
    dbgpln("  Type                            QmRqd      EstDP FxOrd %s", DbgOnePBHd());
    }
#endif

  // Solve In Forward then Reverse Order
  CJoinGFBIter Fx(m_FxdPList);
  for (CJoinGFBPtr * pc = Fx.First(); pc ; pc=Fx.Next())
    if (pc->IsJoin())
      FindFxdJoinPEstRqd(*pc);
    else
      FindFxdFBPEstRqd(*pc);

#if dbgFlwNets
  //DumpFxdConnPs();
  if (dbgSolveFxdPs())
    {
    dbgpln(SepLn);
    dbgpln("SolveFxdPs");
    dbgpln("  Type                            QmRqd      EstDP FxOrd       %s", DbgOnePBHd());
    };
#endif

  for (pc = Fx.Last(); pc ; pc=Fx.Prev())
    if (pc->IsJoin())
      SolveFxdJoinP(*pc);
    else
      SolveFxdFBP(*pc);

#if dbgFlwNets
  if (dbgSolveFxdPs())
    dbgpln(SepLn);
  DumpFxdConnPs();
#endif

  Traverse(); // Get all internal Pressures correct

  }

//-------------------------------------------------------------------------

void CSubNetBlk::SolveFxdGFBQ( CJoinGFBPtr & JG)//, CGrpFlwBlk * & pPrevGFB)
  {
  char * pDbgStr=fXfer ? "SolveXfrLnkQ" : "SolveFxdLnkQ";
  CGrpFlwBlk &GFB= *JG.GFB();
  if (GFB.m_fFixedExternal)
    {
#if dbgFlwNets
    if (dbgSolveFxdQs())
      dbgpln("%s LnkQ Is    %#16.9g %18s ---- %s", FxdStr(fXfer), GFB.m_QmIn,"",GFB.FwdTag());
#endif
    // Actual flow should be set by SolveFxdTieQ
    GFB.Traverse(pDbgStr);
    double QRqd=GFB.RequiredQm();
    if (Valid(QRqd))
      {
      // Reset Actual Flow
      if (QRqd>=0.0)
        GFB.SetLinkQmEstAndQm(Range(0.0, GFB.m_QmIn, QRqd));
      else
        GFB.SetLinkQmEstAndQm(Range(QRqd, GFB.m_QmIn, 0.0));
      // Actual flow should be set by SolveFxdTieQ
      GFB.Traverse(pDbgStr);
      }
    }
  else
    {
    GFB.Traverse(pDbgStr);
    double QRqd=GFB.RequiredQm();
#if dbgFlwNets
    if (dbgSolveFxdQs())
      dbgpln("%s LnkQ Set   %#16.9g %18s ---- %s", FxdStr(fXfer), QRqd,"",GFB.FwdTag());
#endif
    GFB.SetLinkQmEstAndQm(Valid(QRqd) ? QRqd : GFB.m_QmIn);
    }
  }

//-------------------------------------------------------------------------

void CSubNetBlk::SolveFxdJoinQ( CJoinGFBPtr & JG)
  {
  CJoinRec &Jn=*JG.Join();
  ASSERT(Jn.fIsTie);
#if dbgFlwNets
  if (dbgSolveFxdQs())
    dbgpln("%s JoinQ                       %18s ---- %s",FxdStr(fXfer),"",Jn.Nd()->FullObjTag());
#endif

  // Sum Inputs
  double Flw=0.0;
  int nToSet=0;
  for (int c=0 ; c<Jn.m_nConns; c++)
    {
    CJoinRec * pR=Jn.Join_Rmt(c);
    if (pR->iFxdQOrder < Jn.iFxdQOrder)
      {
      double Qm=Jn.FB_Self(c)->m_Qm*Jn.FB_SelfSgn(c);
#if dbgFlwNets
      if (dbgSolveFxdQs())
        dbgpln("               %+#16.9g %18s      %s",Qm,"",pR->Nd()->FullObjTag());
#endif
      Qm=ChkZero(Qm);
      Flw+=Qm;
      }
    else if (++nToSet==1)
      Jn.iQmToSet=c;
    }

  if (Jn.pNd->fDoDbgBrk)
    BreakPoint();
  if (nToSet!=1)
    return;

#if dbgFlwNets
  if (dbgSolveFxdQs())
    dbgpln("             = %+#16.9g %18s      %s",Flw,"",Jn.Nd_Rmt(Jn.iQmToSet)->FullObjTag());
#endif
  double QRqd=-Flw*Jn.QSgn(Jn.iQmToSet);
  Jn.GFB(Jn.iQmToSet)->SetLinkQmEstAndQm(QRqd);
  }

//-------------------------------------------------------------------------

void CSubNetBlk::LimitFxdGFBQ( CJoinGFBPtr & JG, long & Changes)
  {
  char * pDbgStr=fXfer ? "LimitXfrLnkQ" : "LimitFxdLnkQ";
  CGrpFlwBlk &GFB= *JG.GFB();
  if (GFB.m_fFixedExternal)
    {
#if dbgFlwNets
    if (dbgSolveFxdQs())
      dbgpln("%s LnkQ Is    %#16.9g %18s ---- %s", FxdStr(fXfer), GFB.m_QmIn, "",GFB.FwdTag());
#endif
    // Actual flow should be set by SolveFxdTieQ
    double QRqd=GFB.RequiredQm();
    // Closer to Zero
    bool DoLimit=false;
    if (Valid(QRqd))
      if (GFB.m_QmIn>0 && QRqd<GFB.m_QmIn)
        {
        DoLimit=true;
        QRqd=GEZ(QRqd);
        }
      else if (GFB.m_QmIn<0 && QRqd>GFB.m_QmIn)
        {
        DoLimit=true;
        QRqd=LEZ(QRqd);
        }

      if (DoLimit)
        {
#if dbgFlwNets
        if (dbgSolveFxdQs())
          dbgpln("%s LnkQ Limit %#16.9g > %#16.9g ---- %s", FxdStr(fXfer), GFB.m_QmIn, QRqd, GFB.FwdTag());
#endif
        if (fabs(GFB.m_QmIn-QRqd)>1e-6)
          Changes++;
        GFB.SetLinkQmEstAndQm(QRqd);
        }
      // Update Pressures
      GFB.Traverse(pDbgStr);
    }
  else
    {
    // Update pressures
    GFB.Traverse(pDbgStr);
    }
  }

//-------------------------------------------------------------------------

void CSubNetBlk::LimitFxdJoinQ( CJoinGFBPtr & JG, long & Changes)
  {
  CJoinRec &Jn=*JG.Join();
  char * pDbgStr=fXfer ? "LimitXfrJoinQ" : "LimitFxdJoinQ";
  ASSERT(Jn.fIsTie);
#if dbgFlwNets
  if (dbgSolveFxdQs())
    dbgpln("%s JoinQLimit                  %18s ---- %s",FxdStr(fXfer),"",Jn.Nd()->FullObjTag());
#endif

  // Sum Outputs
  int i=Jn.iQmToSet;
#if dbgFlwNets
  if (dbgSolveFxdQs())
    dbgpln("               %#16.9g %18s      %s",Jn.Nd()->IOQm_In(Jn.IONo(i)),"",Jn.Nd_Rmt(i)->FullObjTag());
#endif

  double FlwIn=0.0;
  double FlwOut=0.0;
  for (int c=0 ; c<Jn.m_nConns; c++)
    {
    CJoinRec * pR=Jn.Join_Rmt(c);
    double Qm=ChkZero(Jn.FBQm_In(c));
    if (Qm>0.0)
      FlwIn+=Qm;
    else
      FlwOut-=Qm;
    }

  if (Jn.pNd->fDoDbgBrk)
    BreakPoint();

#if dbgFlwNets
  if (dbgSolveFxdQs())
    dbgpln("             I %+#16.9g O %+#16.9g",FlwIn,FlwOut);
#endif

  if (FlwIn > FlwOut)
    {
    double Frac=FlwOut/GTZ(FlwIn);
    for (c=0 ; c<Jn.m_nConns; c++)
      {
      double Qm=ChkZero(Jn.FBQm_In(c));
      if (Qm>0.0)
        {
        CGrpFlwBlk &GFB= *Jn.GFB_Rmt(c);

        double QmWas = GFB.m_QmEstIn;
        Qm*=Jn.QSgn(c);
        GFB.SetLinkQmEstAndQm(Qm*Frac);
        GFB.Traverse(pDbgStr);
        if (fabs(Qm*Frac-QmWas)>1e-6)
          Changes++;
#if dbgFlwNets
        if (dbgSolveFxdQs())
          dbgpln("       In Now  %+#16.9g  [%+#16.9g]     %s",Jn.FBQm_In(c),QmWas,Jn.Nd_Rmt(c)->FullObjTag());
#endif
        }
      }
    }
  else
    {
    double Frac=FlwIn/GTZ(FlwOut);
    for (c=0 ; c<Jn.m_nConns; c++)
      {
      double Qm=ChkZero(Jn.FBQm_In(c));
      if (Qm<0.0)
        {
        CGrpFlwBlk &GFB= *Jn.GFB_Rmt(c);

        double QmWas = GFB.m_QmEstIn;
        Qm*=Jn.QSgn(c);
        GFB.SetLinkQmEstAndQm(Qm*Frac);
        GFB.Traverse(pDbgStr);
        if (fabs(Qm*Frac-QmWas)>1e-6)
          Changes++;
#if dbgFlwNets
        if (dbgSolveFxdQs())
          dbgpln("      Out Now  %+#16.9g  [%+#16.9g]     %s",Jn.FBQm_In(c),QmWas,Jn.Nd_Rmt(c)->FullObjTag());
#endif
        }
      }
    }
  }

//-------------------------------------------------------------------------

void CSubNetBlk::SumFxdLnkQ( CJoinGFBPtr & JG)//CJoinGFBPtr * pC, CGrpFlwBlk * & pPrevGFB)
  {
  CGrpFlwBlk &GFB = *JG.GFB();
  CJoinRec * SJ=GFB.m_Src.m_pJoin;
  if (SJ && SJ->fIsTie && SJ->Typ!=JTyp_FxdTie)
    {
    double VPhFrac=GFB.m_Src.m_pFBJn->PhaseFrac(NULL, 0);
    double LPhFrac=GFB.m_Src.m_pFBJn->PhaseFrac(NULL, 1);
    SJ->dSigmaQmV-=GFB.m_QmIn*GTZ(VPhFrac);
    SJ->dSigmaQmL-=GFB.m_QmIn*GTZ(LPhFrac);
    if (SJ->pNd->fDoDbgBrk)
      BreakPoint();
    }
  CJoinRec * DJ=GFB.m_Dst.m_pJoin;
  if (DJ && DJ->fIsTie && DJ->Typ!=JTyp_FxdTie)
    {
    double VPhFrac=GFB.m_Dst.m_pFBJn->PhaseFrac(NULL, 0);
    double LPhFrac=GFB.m_Dst.m_pFBJn->PhaseFrac(NULL, 1);
    DJ->dSigmaQmV+=GFB.m_QmIn*GTZ(VPhFrac);
    DJ->dSigmaQmL+=GFB.m_QmIn*GTZ(LPhFrac);
    if (DJ->pNd->fDoDbgBrk)
      BreakPoint();
    }
  }

//-------------------------------------------------------------------------

void CSubNetBlk::SolveFxdQs()
  {
  CStopWatchLap SWLapSolveFxdFlwQTicks(gs_swSolveFxdFlwQTicks);

  CJoinGFBIter Fx(m_FxdQList);
  for (CJoinGFBPtr * pc = Fx.First(); pc ; pc=Fx.Next())
    if (pc->IsJoin())
      SolveFxdJoinQ(*pc);
    else
      SolveFxdGFBQ(*pc);

  const int MaxLoops=10;
  for (int Loop=0; Loop<MaxLoops; Loop++)
    {
#if dbgFlwNets
    if (dbgSolveFxdQs())
      {
      dbgpln("LimitFxdFlows Loop %2i ------------------", Loop);
      dbgindent(+2);
      }
#endif
    long Changes=0;
    for (pc = Fx.Last(); pc ; pc=Fx.Prev())
      if (pc->IsJoin())
        LimitFxdJoinQ(*pc, Changes);
      else
        LimitFxdGFBQ(*pc, Changes);
#if dbgFlwNets
    if (dbgSolveFxdQs())
      {
      dbgindent(-2);
      dbgpln("LimitFxdFlows Loop %2i Changes %i ", Loop, Changes);
      }
#endif
    if (Changes==0)
      break;
    }

  if (Loop>=MaxLoops)
    {
    LogError("Solver", 0, "Max Limit Fxd Flow Loops"); 
    }

  for (pc = Fx.First(); pc ; pc=Fx.Next())
    if (!pc->IsJoin())
      SumFxdLnkQ(*pc);

  }

//-------------------------------------------------------------------------

void CSubNetBlk::SolveXferGFBQ( CJoinGFBPtr & JG)//, CGrpFlwBlk * & pPrevGFB)
  {
  Strng S;
  char * pDbgStr=fXfer ? "SolveXfrLnkQ" : "SolveFxdLnkQ";
  CGrpFlwBlk &GFB= *JG.GFB();
  if (GFB.m_fFixedExternal)
    {

    GFB.Traverse(pDbgStr);

    double Qm;
    if (Valid(GFB.m_QmXfCap))
      Qm=GFB.m_QmXfCap;
    else
      {
      JoinIter L(GFB.m_Lnks);
      CJoinRec * pJnt = L.First();
      Qm=MaxXfCap * pJnt->Dirn;
      }

#if dbgFlwNets
    if (dbgSolveFxdQs())
      dbgpln("%s LnkQ Is    %#16.9g %18s ---- %s", FxdStr(fXfer), Qm,"",GFB.FwdTag());
#endif
    GFB.SetLinkQmEstAndQm(Qm);
    }
  else
    {
    GFB.Traverse(pDbgStr);
    double QRqd=GFB.RequiredQm();
#if dbgFlwNets
    if (dbgSolveFxdQs())
      dbgpln("%s LnkQ Set   %#16.9g %18s ---- %s", FxdStr(fXfer), QRqd,"",GFB.FwdTag());
#endif
    GFB.SetLinkQmEstAndQm(Valid(QRqd) ? QRqd : GFB.m_QmIn);
    }
  }

//-------------------------------------------------------------------------

void CSubNetBlk::SolveXferQs()
  {
  CStopWatchLap SWLapSolveFxdFlwQTicks(gs_swSolveFxdFlwQTicks);

  CJoinGFBIter Fx(m_FxdQList);
  //CGrpFlwBlk *pPrevGFB=NULL;
  for (CJoinGFBPtr * pc = Fx.First(); pc ; pc=Fx.Next())
    //if (pc->IsJoin())
    //  pPrevGFB=NULL;
    //else
    if (!pc->IsJoin())
      SolveXferGFBQ(*pc);//, pPrevGFB);
  }

//-------------------------------------------------------------------------

//flag CSubNetBlk::LoadTiePress()
//  {
//  return 1;
//  }

//---------------------------------------------------------------------------

void CSubNetBlk::FITGrpMeasure(CLkGrpFlwIter & Iter)
  {
  CGrpFlwBlkPtr g;
  SCDTRY
    {
    while (Iter.GetNext(g))
      {
      g->Traverse("Measure");
      if (g->m_fMomentumValid)
        fMomentumValid=true;

      if (!g->m_fGFTolsOK)
        {
        g->m_fIsBad=true;
  #if (dbgFlwNets)
        if (dbgShowBad())
          dbgpln(" Flw Is Bad P - %s", g->FwdTag());
  #endif
        }
      //udf if (g->fUDFShut)
      //udf   g->Set_Qm( SmallPosFlow, true);
      if (!g->m_fGFNetTolsOK)
        {
        fNetTolsOK = false;
  #if (dbgFlwNets)
        if (DbgFlags & DBG_TraverseP)
          dbgpln(" Net not converged P - %s", g->FwdTag());
  #endif
        }
      }
    }
  SCDCATCH("CSubNetBlk::FITGrpMeasure", g ? g->FwdTag():NULL)
  }

//-------------------------------------------------------------------------


flag CSubNetBlk::Measure(int NetCnvLoop)
  {
  iNetCnvLoop=NetCnvLoop;
  CStopWatchLap SWLapMeasureTicks(gs_swMeasureTicks);
  GrpFlwIter G(Grps);
  for (CGrpFlwBlk * g=G.First(); g; g=G.Next())
    g->m_fIsBad=false;

  fNetTolsOK = true;
  fMomentumValid=false;

  SCDTRY
    {
    if (FSTC.NThreads())
      {
      CLkGrpFlwIter TCAll(&Grps);
      FSTC.DoJob(eFITGrpMeasure, this, &TCAll);
      }
    else
      {
      GrpFlwIter G(Grps);
      for (CGrpFlwBlk * g=G.First(); g; g=G.Next())
        {
        g->Traverse("Measure");
        if (g->m_fMomentumValid)
          fMomentumValid=true;

        if (!g->m_fGFTolsOK)
          {
          g->m_fIsBad=true;
#if (dbgFlwNets)
          if (dbgShowBad())
            dbgpln(" Flw Is Bad P - %s", g->FwdTag());
#endif
          }
        //udf if (g->fUDFShut)
        //udf   g->Set_Qm( SmallPosFlow, true);
        if (!g->m_fGFNetTolsOK)
          {
          fNetTolsOK = false;
#if (dbgFlwNets)
          if (DbgFlags & DBG_TraverseP)
            dbgpln(" Net not converged P - %s", g->FwdTag());
#endif
          }
        }
      }


    if (m_RegGrps.GetCount()>0)
      {
#if (dbgFlwNets)
      flag DoDbg=dbgRegulators()!=0;
      if (0 && DoDbg)
        dbgpln(" Regulators - Measure");
#endif
      GrpFlwIter RG(m_RegGrps);
      for (CGrpFlwBlk * rg=RG.First(); rg; rg=RG.Next())
        {
        CGrpFlwRegBlk &GFRB = rg->m_Regulator;
        FlwNode &Nd         = *GFRB.m_pNd;
        CFlwRegBlk &FRB     = *GFRB.m_pFRB;
        FlwBlk &FB          = *GFRB.m_pFB;


#if (dbgFlwNets)
        if (DoDbg)
          {
          dbglock();
          dbgp("\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\"
            "\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\");
          dbgpln(" Reg: What:%2i %s", FRB.m_iWhat, rg->FwdTag());
          }
#endif
        if (FRB.m_iWhat>0)
          {

          //int MaxLoops = LineCnvCnt[Min(NetCnvLoop,NLineCnvCnt-1)];
          //flag FirstOuterLoop = NetCnvLoop==0;
          //flag LastOuterLoop = NetCnvLoop==NLineCnvCnt-1;
          //rg->ConvergeQm(MaxLoops, FirstOuterLoop, LastOuterLoop);

          if (NetCnvLoop>0)
            GFRB.m_nHistory=Min(GFRB.m_nHistory+1L, MaxRegHistory);
          else
            GFRB.m_nHistory=1;

          for (int i=GFRB.m_nHistory-1; i>0; i--)
            {
            GFRB.m_daError[i]=GFRB.m_daError[i-1];
            GFRB.m_daCtrl[i]=GFRB.m_daCtrl[i-1];
            GFRB.m_daGain[i]=GFRB.m_daGain[i-1];
            }

          GFRB.m_daError[0]=GFRB.m_dMeasValue-FRB.StepTarget();
          GFRB.m_daCtrl[0]=FRB.m_dCtrlCur;
          if (GFRB.m_nHistory>1)
            {
            double ChgError=GFRB.m_daError[0]-GFRB.m_daError[1];
            double ChgCtrl=GFRB.m_daCtrl[0]-GFRB.m_daCtrl[1];

            GFRB.m_daGain[0]=ChgCtrl/NZ(ChgError);
            }

#if (dbgFlwNets)
          if (DoDbg)
            {
            dbgpln("n:%2i C:%8.4f M:%10.2f T:%10.2f", 
              GFRB.m_nHistory, FRB.m_dCtrlCur, GFRB.m_dMeasValue , FRB.StepTarget());
            for (int i=GFRB.m_nHistory-1; i>=0; i--) 
              {
              dbgp("  t-%i, E:%10.2f C:%8.4f", i, GFRB.m_daError[i], GFRB.m_daCtrl[i]);
              if (i<GFRB.m_nHistory-1)
                dbgp(" Gn:%8.4f ", GFRB.m_daGain[i]);
              if (i>0)
                dbgpln("");
              }
            }
#endif

          if (GFRB.m_nHistory>=2 && 
            (fabs(GFRB.m_daGain[0]-GFRB.m_daGain[1]) < 0.1*fabs(GFRB.m_daGain[0])) && // small change
            (GFRB.m_daGain[0]*GFRB.m_daGain[1]>=0)  // same sign
            )
            {
            double RqdCtrlChg=-0.8*(0.5*(GFRB.m_daGain[0]+GFRB.m_daGain[1]))*GFRB.m_daError[0];

            double DChg=0.2*(FRB.CtrlMax()-FRB.CtrlMin());
            RqdCtrlChg=Range(-DChg, RqdCtrlChg, DChg);

            double RqdCtrl=FRB.m_dCtrlCur+RqdCtrlChg;

            RqdCtrl=Range(FRB.CtrlMin(), RqdCtrl, FRB.CtrlMax());

            rg->m_Regulator.SetFlwRegulator(RqdCtrl);
            FRB.m_bHoldCvg=true;
            GFRB.m_nHistory=0;

#if (dbgFlwNets)
            if (DoDbg)
              dbgpln(" Ctrl:%+8.5f >>:%8.5f", RqdCtrlChg, RqdCtrl);
#endif
            }
          else
            {
            FRB.m_bHoldCvg=false;

#if (dbgFlwNets)
            if (DoDbg)
              dbgpln("");
#endif
            }
          }

#if (dbgFlwNets)
        if (DoDbg)
          {
          dbgpln("//////////////////////////////////////////////////////////////////////////");
          dbgunlock();
          }
#endif
        }
      }

#if dbgFlwNets
    if (dbgWhere())
      dbgpln("Measure  SubNets  %s ", fNetTolsOK ? "OK" : "NOT CONVERGED");
#endif
    }
  SCDCATCH("CSubNetBlk::Measure", NULL)
  return fNetTolsOK;
  }

//---------------------------------------------------------------------------

void CSubNetBlk::FITGrpTraverse(CLkGrpFlwIter & Iter)
  {
  CGrpFlwBlkPtr g;
  SCDTRY
    {
    while (Iter.GetNext(g))
      {
      g->Traverse("Test");
      if (g->m_fMomentumValid)
        fMomentumValid=true;
      }
    }
  SCDCATCH("CSubNetBlk::FITGrpTraverse", g ? g->FwdTag():NULL)
  }

//-------------------------------------------------------------------------

void CSubNetBlk::Traverse()
  {
  CStopWatchLap SWLapConvergeTicks(gs_swConvergeTicks);

  SCDTRY
    {
    fMomentumValid=false;
  if (FSTC.NThreads())
    {
    CLkGrpFlwIter TCAll(&Grps);
    FSTC.DoJob(eFITGrpTraverse, this, &TCAll);
    }
  else
    {
    GrpFlwIter G(Grps);
    for (CGrpFlwBlk * g=G.First(); g; g=G.Next())
      {
      g->Traverse("Test");
      if (g->m_fMomentumValid)
        fMomentumValid=true;
      }
    }
#if dbgFlwNets
  DumpFxdConnPs();
#endif
    }
  SCDCATCH("CSubNetBlk::Traverse", NULL)
  }

//---------------------------------------------------------------------------

void CSubNetBlk::FITGrpConverge(CLkGrpFlwIter & Iter)
  {
  CGrpFlwBlkPtr g=NULL;
  SCDTRY
    {
    while (Iter.GetNext(g))
      {
      if (g->m_fFixed)
        {
        g->SetLinkQmEstAndQm(g->RequiredQm());
        g->Traverse("TestFxd1");
        }
      else // CNM if (!g->Solved)
        {
        g->Traverse("Init");
        if (g->m_fGFTolsOK && (g->m_fGFRegTolOK || g->m_fGFHoldRegCvg))
          {
          }
        else if (g->m_fFixed)
          {
          g->SetLinkQmEstAndQm(g->RequiredQm());
          g->Traverse("TestFxd2");
          }
        else 
          {
          int MaxLoops = LineCnvCnt[Min(iNetCnvLoop,NLineCnvCnt-1)];
          flag FirstOuterLoop = iNetCnvLoop==0;
          flag LastOuterLoop = iNetCnvLoop==NLineCnvCnt-1;
          g->ConvergeQm(MaxLoops, FirstOuterLoop, LastOuterLoop);
          g->ConvergeRegulator(MaxLoops, FirstOuterLoop, LastOuterLoop);
          }
        if (!g->m_fGFNetTolsOK)
          fNetTolsOK=0;
        }
      }
    }
  SCDCATCH("CSubNetBlk::FITGrpConverge", g ? g->FwdTag():NULL)
  }

//-------------------------------------------------------------------------

flag CSubNetBlk::Converge(int NetCnvLoop)
  {
  iNetCnvLoop=NetCnvLoop;
  CStopWatchLap SWLapConvergeTicks(gs_swConvergeTicks);

#if dbgConvergeSubNet
  dbgpln("ConvergeSubNet [%3i.%3i]------ %s", Net,SubNet,Nets[Net].SubNet[SubNet].Start->Tag());
#endif

  fNetTolsOK = true;
  if (FSTC.NThreads())
    {
    CLkGrpFlwIter TCAll(&Grps);
    FSTC.DoJob(eFITGrpConverge, this, &TCAll);
    }
  else
    {
    GrpFlwIter G(Grps);
    for (CGrpFlwBlk * g=G.First(); g; g=G.Next())
      {
      if (g->m_fFixed)
        {
        g->SetLinkQmEstAndQm(g->RequiredQm());
        g->Traverse("TestFxd");
        }
      else // CNM if (!g->Solved)
        {
        if (g->m_fHasDbgBrk)
          {
          static int DoDmp=0;
          if (DoDmp)
            {
            double QMem=g->m_QmIn;
            long NSteps=100;
            long hNSteps=NSteps/2;
            double QHi=10;
            double QLo=0.000001;
            double LnQHi=log(QHi);
            double LnQLo=log(QLo);
            double dLnQ=(LnQHi-LnQLo)/hNSteps;
            for (int i=0; i<=NSteps; i++)
              {
              double Q=0;
              if (i<hNSteps)
                Q=-exp(LnQLo+(hNSteps-i)*dLnQ);
              else if (i>hNSteps)
                Q=exp(LnQLo+(i-hNSteps)*dLnQ);
              g->SetLinkQmEstAndQm(Q);
              g->Traverse("Crv");
              }
            g->SetLinkQmEstAndQm(QMem);
            }
          }
        g->m_fSetCI6=false;
        g->m_fSetCI7=false;
        g->m_fSetCI8=false;
        g->Traverse("Init");
        if (g->m_fGFTolsOK && (g->m_fGFRegTolOK || g->m_fGFHoldRegCvg))
          {
          }
        else if (g->m_fFixed)
          {
          g->SetLinkQmEstAndQm(g->RequiredQm());
          g->Traverse("TestFxd");
          }
        else // CNM if (!g->Solved)
          {
          int MaxLoops = LineCnvCnt[Min(iNetCnvLoop,NLineCnvCnt-1)];
          flag FirstOuterLoop = iNetCnvLoop==0;
          flag LastOuterLoop = iNetCnvLoop==NLineCnvCnt-1;
          g->ConvergeQm(MaxLoops, FirstOuterLoop, LastOuterLoop);
          g->ConvergeRegulator(MaxLoops, FirstOuterLoop, LastOuterLoop);
          }
        if (!g->m_fGFNetTolsOK)
          fNetTolsOK=0;
        }
      }
    }

#if dbgFlwNets
  if (dbgWhere())
    dbgpln("Converge SubNets  %s ", fNetTolsOK ? "OK" : "NOT CONVERGED");
#endif

  return fNetTolsOK;
  }

//-------------------------------------------------------------------------

flag CSubNetBlk::SolveUnDefined()
  {
  //iNetCnvLoop=NetCnvLoop;
  CStopWatchLap SWLapConvergeTicks(gs_swConvergeTicks);

#if dbgConvergeSubNet
  dbgpln("SolveUndefinedSubNet [%3i.%3i]------ %s", Net,SubNet,Nets[Net].SubNet[SubNet].Start->Tag());
#endif

  fNetTolsOK = true;
  GrpFlwIter G(Grps);
  for (CGrpFlwBlk * g=G.First(); g; g=G.Next())
    {
    g->m_fUnDefined=true;
    g->m_fSetCI6=false;
    g->m_fSetCI7=false;
    g->m_fSetCI8=false;
    g->Traverse("Init");

    if (g->m_fFixed)
      {
      g->SetLinkQmEstAndQm(g->RequiredQm());
      g->Traverse("TestFxd");
      }
    else // CNM if (!g->Solved)
      {
      g->SetLinkQmEstAndQm(0.0);//g->RequiredQm());
      g->Traverse("SetZero");
      }
    }

#if dbgFlwNets
  if (dbgWhere())
    dbgpln("SolveUndefined SubNets  %s ", fNetTolsOK ? "OK" : "NOT CONVERGED");
#endif

  return fNetTolsOK;
  }

//-------------------------------------------------------------------------

//inline flag DoJoinFlow(CJoinRec & J, int i)
//  { return (i==0) && J.VFlowAllowed() || (i==1) && J.LFlowAllowed(); };
//
//inline flag DoJoinFlow(CGrpFlwBlk & G, int i)
//  { return (i==0) && G.VFlowAllowed() || (i==1) && G.LFlowAllowed(); };
//
//inline flag DoJoinOneFlow(CGrpFlwBlk & G)
//  { return !G.VFlowAllowed() || !G.LFlowAllowed(); };

// --------------------------------------------------------------------------

char * CSubNetBlk::TIndicesStr(char * Buff, CJoinRec * pJ)
  {
  char P[33], V[50], Va[33], L[50], La[33], X[33], IsoV[33], IsoL[33], V2L[33];

  sprintf(P, "%s", pJ->PInx() >=0 ? ltoa(pJ->PInx(), X, 10) : ".");

  sprintf(V2L, "%s", (pJ->TQV2LInx() >=0 ? ltoa(pJ->TQV2LInx(), X, 10) : "."));
  int ii=pJ->TQIsoInx();
  sprintf(IsoV, "%s", (ii>=0 && pJ->VFlowAllowed(m_bFullFlow) ? ltoa(ii++, X, 10) : "."));
  sprintf(IsoL, "%s", (ii>=0 && pJ->LFlowAllowed(m_bFullFlow) ? ltoa(ii, X, 10) : "."));

  sprintf(V, "%s", (pJ->TQVAccInx() >=0 ? ltoa(pJ->TQVAccInx(), Va, 10) : " "));
  for (int c=0; c<pJ->NConns(); c++)
    {
    int i=pJ->GFB(c)->LQVInx();
    if (i>=0)
      {
      sprintf(Va, "%s%s", SgnStr(pJ->QSgn(c)), ltoa(i, X, 10));
      strcat(V, Va);
      }
    }
  if (pJ->TQV2LInx() >=0)
    {
    strcat(V,"-");
    strcat(V,V2L);
    }

  sprintf(L, "%s", (pJ->TQLAccInx() >=0 ? ltoa(pJ->TQLAccInx(), La, 10) : " "));
  for (c=0; c<pJ->NConns(); c++)
    {
    int i=pJ->GFB(c)->LQLInx();
    if (i>=0)
      {
      sprintf(La, "%s%s", SgnStr(pJ->QSgn(c)), ltoa(i, X, 10));
      strcat(L, La);
      }
    }
  if (pJ->TQV2LInx() >=0)
    {
    strcat(L,"+");
    strcat(L,V2L);
    }

  sprintf(Buff, "%-7.7s %-15.15s %-15.15s %3s %3s/%3s",P,V,L,V2L,IsoV,IsoL);
  return Buff;
  }

// --------------------------------------------------------------------------

char * CSubNetBlk::LIndicesStr(char * Buff, CGrpFlwBlk * pG)
  {
  char P[33], X[50], Y[33];
  char V[50], L[50];
  sprintf(P, "%s,%s",
    (pG->PSrcInx() >=0 ? ltoa(pG->PSrcInx(), X, 10) : "-"),
    (pG->PDstInx() >=0 ? ltoa(pG->PDstInx(), Y, 10) : "-"));
  sprintf(Buff, "%-7.7s %-15.15s %-15.15s %3s %3s %3s",
    P,
    (pG->LQVInx() >=0 ? ltoa(pG->LQVInx(), V, 10) : ""),
    (pG->LQLInx() >=0 ? ltoa(pG->LQLInx(), L, 10) : ""),
    "","","");
  return Buff;
  }

//-------------------------------------------------------------------------

void CSubNetBlk::SolveIsoNode()
  {
  // An IsoNode Has all flws in/out of it defined
  if (Ties.GetCount()!=1 || Grps.GetCount()!=0)
    DoBreak();

  CJoinRec & T =*(Ties.GetHead());

#if dbgFlwNets
  flag DoDbg=(flag)dbgSolveIsoNode();
  if (DoDbg)
    dbgp("Iso ");
#endif

  if (T.pNd->fDoDbgBrk)
    BreakPoint();

  /*
  if (T.fIsImpNd || T.fIsAccLnk)
  {
  SigPEqn=Eqn++;
  if (T.VFlowAllowed())
  SigQmVEqn=Eqn++;
  if (T.LFlowAllowed())
  SigQmLEqn=Eqn++;
  if (T.TQV2LInx()>0) // From Vapour to Liquid
  V2LQmEqn=Eqn++;
  }
  else
  {
  #pragma chNOTE(To be improved)
  SigQmVEqn=Eqn;
  SigQmLEqn=Eqn;
  Eqn++;
  }

  if (T.fIsoNetRef)
  IsoRefEqn=Eqn++;

  for (int Lp=m_Lp0; Lp<m_LpN; Lp++)
  if (DoJoinFlow(T, Lp))
  {
  int Eqn=(Lp==0) ? SigQmVEqn : SigQmLEqn;
  for (int c=0; c<T.NConns(); c++)
  if (ConnSameSubNet(&T, c))
  {
  CGrpFlwBlk &G=*T.IOFB(c)->pGFb;
  if (G.LQInx(Lp)>=0)
  {
  int iQInx=G.LQInx(Lp);

  m_SS.SetValue(Eqn, iQInx, T.QSgn(c));
  #if dbgFlwNets
  Sigs[nSigs++]= T.QSgn(c)*iQInx;
  #endif
  }
  }

  double Q1=-T.SigmaQm(Lp);
  if (T.pNd->fDoDbgBrk)
  BreakPoint();
  if (FlwLarge(Q1))
  {
  Q1=0.0;
  T.Nd()->SetCI(15);
  }
  else
  T.Nd()->ClrCI(15);
  Q1+=-T.GetQmSink(Lp);
  //          Q1-=((Lp==0) ? -1 : 1) * T.dV2LQmReqd;
  Q1=ChkZero(Q1);
  double Q2=0.0;

  if (T.fHasQmAux)
  {
  JoinQmAux &QA = *T.pQmAux;
  if (QA.HasRqdQm())
  Q2= -ChkZero(-QA.dRqdQm);
  }

  double VecVal=Q1+Q2;
  if ((SigQmVEqn==SigQmLEqn) && (Lp>0))
  {
  VecVal+=m_SS.GetVector(Eqn);
  }
  m_SS.SetVector(Eqn, VecVal);

  #if dbgFlwNets
  if (DoDbg)
  {
  if ((SigQmVEqn==SigQmLEqn) && (Lp>0))
  dbgp(" SigQ# %#16.7g",VecVal);
  else
  dbgp(" SigQ  %#16.7g",VecVal);
  //            if (T.fHasQmAux && T.pQmAux->HasRqdQm())
  //              dbgp(" L %#16.7g ",Q2);
  }
  #endif
  }


  if (T.fIsImpNd || T.fIsAccLnk)
  {
  // Connect Delta Q
  double P=T.pNd->JoinP_Good(T.iJoinId);
  #if dbgFlwNets
  if (DoDbg)
  dbgp(" ImpNd P:%#16.7g",P);
  #endif
  for (int Lp=m_Lp0; Lp<m_LpN; Lp++)
  if (DoJoinFlow(T, Lp))
  {
  int QAccEqn=(Lp==0) ? SigQmVEqn : SigQmLEqn;
  if (SigQmVEqn==SigQmLEqn)
  DoBreak();

  m_SS.SetValue(QAccEqn, T.TQAccInx(Lp), -1);      // Sigma Q = 0.0

  double Sens= Lp ? T.pNd->JoinP_SensL(T.iJoinId) : T.pNd->JoinP_SensV(T.iJoinId);
  if (Sens==0.0) // ? Not Set
  Sens=1.0e6;
  Sens*=ICGetTimeInc();

  m_SS.SetValue(SigPEqn, T.TQAccInx(Lp), Sens);
  m_SS.SetValue(SigPEqn, T.PInx(), -1);      // Final P In Node
  m_SS.SetVector(SigPEqn, -Max(P, 1.0));     // Init P In Node
  #if dbgFlwNets
  if (DoDbg)
  dbgp(" %sS:%#16.7g ",(Lp?"L":"V"),Sens);
  #endif

  // Static Head sensitivities
  if (WithRhoHSens)
  for (int c=0; c<T.NConns(); c++)
  {
  CGrpFlwBlk &G=*T.IOFB(c)->pGFb;
  double S=(Lp==0?T.pNd->IOB_SensV(T.IONo(c)):T.pNd->IOB_SensL(T.IONo(c)));
  S*=ICGetTimeInc();

  if (fabs(S)>1.0e-6)//LowestRes)
  {
  int SigPEqn=(Lp==0?G.SigPVInEqn:G.SigPLInEqn);
  // Only applies to Liquid Acc
  m_SS.SetValue(SigPEqn, T.TQAccInx(1), S*T.QSgn(c));
  }
  }
  }

  if (T.TQV2LInx()>0) // From Vapour to Liquid
  {
  // Other Sigma Q's
  m_SS.SetValue(SigQmVEqn, T.TQV2LInx(), -1);
  m_SS.SetValue(SigQmLEqn, T.TQV2LInx(), 1);

  m_SS.SetVector(V2LQmEqn, -T.dV2LQmReqd);
  #if dbgFlwNets
  if (DoDbg)
  dbgp("\n   V2L:%#16.7g ",-T.dV2LQmReqd);
  #endif

  // Qx=Sig Q ...
  m_SS.SetValue(V2LQmEqn, T.TQV2LInx(), -1);
  #if dbgFlwNets
  if (DoDbg)
  dbgp(" VFct:%#16.7g LFct:%#16.7g ",T.dV2LQmVFact, T.dV2LQmLFact );
  #endif
  for (int c=0; c<T.NConns(); c++)
  {
  CGrpFlwBlk &G=*T.IOFB(c)->pGFb;
  for (int Lp=m_Lp0; Lp<m_LpN; Lp++)
  {
  if (G.LQInx(Lp)>=0)
  {
  int iQInx=G.LQInx(Lp);
  double Fact=(Lp==0 ? T.dV2LQmVFact : T.dV2LQmLFact);
  if (fabs(Fact) > V2LLimit)
  m_SS.SetValue(V2LQmEqn, iQInx, Fact*T.QSgn(c));
  }
  }
  }
  }
  }

  if (T.fIsoNetRef)
  {
  //TODO Must fix IsoNetRef
  DoBreak();
  const int QInx=T.TQIsoInx();//
  const int SInx=T.PInx();
  // Resistance Value is not very important Flow should be zero
  m_SS.SetValue(IsoRefEqn,   QInx, 1.230);
  m_SS.SetValue(IsoRefEqn,   SInx, -1.0);
  m_SS.SetVector(IsoRefEqn, -Max(T.PB.P, Std_P));
  // Equation : Sigma Q=0 for source Node
  m_SS.SetValue(SInx,   QInx, 1.0);
  }

  #if dbgFlwNets
  if (DoDbg)
  {
  dbgp(" %s.j%i Conns:",T.pNd->FullObjTag(), T.iJoinArrayInx);
  for (int x=0; x<nSigs; x++)
  dbgp("%+i ", Sigs[x]);
  dbgpln("");
  }
  #endif
  }

  */




  };

//-------------------------------------------------------------------------

inline double AdjustR(double R, double & ResAdj, double NetMaxRes)
  {
  double Ra=Range(MinimumResistance, R, NetMaxRes);
  ResAdj=Ra/R;
  return Ra;
  }

//-------------------------------------------------------------------------

void CSubNetBlk::CalcIndicesFull1(CJoinRec * pSrc, CJoinRec * pJ, CGrpFlwBlk * pG)
  {
  if (pJ)
    {
    if (pJ->fIsTie)
      {
      pJ->NdInx = -1;
      pJ->fIsolatedNet = fIsolatedNet;
      pJ->fIsolatedNode = fIsolatedNode;
      m_CalcIndexSeq.AddJoin(pJ);
      for (int c = 0; c < pJ->m_nConns; c++)
        {
        CGrpFlwBlk * pGFB = pJ->GFB(c);
        if ((pGFB->m_NdInx == -2) && (pGFB->m_pSubNet == this))
          CalcIndicesFull1(pJ, NULL, pJ->GFB(c));//->pGFb);
        }
      }
    else if (pJ->fSelfLoop)//udf || pJ->fUDFShut)
      {
      pJ->NdInx = -1;
      pJ->fIsolatedNet = fIsolatedNet;
      pJ->fIsolatedNode = fIsolatedNode;
      m_CalcIndexSeq.AddJoin(pJ);
      }
    }
  else if (pG)
    {
    pG->m_NdInx = -1;
    JoinIter L(pG->m_Lnks);
    for (CJoinRec * pJ = L.First(); (pJ); pJ = L.Next())
      {
      pJ->NdInx = -1;
      pJ->fIsolatedNet = fIsolatedNet;
      pJ->fIsolatedNode = fIsolatedNode;
      }
    m_CalcIndexSeq.AddGrp(pG);

    if (pG->m_Src.m_pJoin==pSrc && pG->m_Dst.m_pJoin->NdInx == -2)
      CalcIndicesFull1(pSrc, pG->m_Dst.m_pJoin, NULL);
    else if (pG->m_Dst.m_pJoin==pSrc && pG->m_Src.m_pJoin->NdInx == -2)
      CalcIndicesFull1(pSrc, pG->m_Src.m_pJoin, NULL);
    }
  }

//-------------------------------------------------------------------------

void CSubNetBlk::CalcIndicesFull2(int &NdInx, CJoinRec * pSrc, CJoinRec * pJ, CGrpFlwBlk * pG,
                                  BldNetInx & PQInx, int &EqnOne)
  {
  if (pJ)
    {
    if (pJ->fIsTie)
      {
      int & PQI=PQInx[pJ->Typ];
      pJ->EqnOne()=EqnOne++;
      pJ->PInx() = PQI++;

      if (pJ->fIsoNetRef)
        {
        pJ->TQIsoInx()=PQI;//++;
        if (pJ->VFlowAllowed(m_bFullFlow))
          {
          PQI++; // Space for Delta Q
          //pJ->TQVAccInx() = PQI++; // Space for Delta Q
          EqnOne++;
          };
        if (pJ->LFlowAllowed(m_bFullFlow))
          {
          PQI++; // Space for Delta Q
          //pJ->TQLAccInx() = PQI++; // Space for Delta Q
          EqnOne++;
          };
        }

      if (pJ->fIsImpNd || pJ->fIsAccLnk)    // Implicit Soln
        {
        if (pJ->VFlowAllowed(m_bFullFlow))
          {
          pJ->TQVAccInx() = PQI++; // Space for Delta Q
          EqnOne++;
          };
        if (pJ->LFlowAllowed(m_bFullFlow))
          {
          pJ->TQLAccInx() = PQI++; // Space for Delta Q
          EqnOne++;
          };
        if (pJ->VFlowAllowed(m_bFullFlow) && pJ->LFlowAllowed(m_bFullFlow) && pJ->V2LReqd(m_bFullFlow))
          {
          pJ->TQV2LInx() = PQI++;  // Space for Xfer Q
          EqnOne++;
          };
        }
      pJ->EqnCnt()=EqnOne-pJ->EqnOne();
      pJ->NdInx = NdInx++;
      for (int c = 0; c < pJ->m_nConns; c++)
        {
        CGrpFlwBlk * pGFB = pJ->GFB(c);
        if ((pGFB->m_NdInx == -1) && (pGFB->m_pSubNet == this))
          CalcIndicesFull2(NdInx, pJ, NULL, pJ->GFB(c), PQInx, EqnOne);
        }
      }
    else if (pJ->fSelfLoop)//udf || pJ->fUDFShut)
      {
      //PQInxThis=-1;
      }
    }
  else if (pG)
    {
    if (!pG->m_fSelfLoop)
      {
      int & PQI=PQInx[pG->m_LnkInx.Typ];
      pG->EqnOne()=EqnOne;

      if (pG->VFlowAllowed(m_bFullFlow))
        {
        pG->LQVInx()  = PQI++;   // Unknown = QmV
        EqnOne++;
        }
      if (pG->LFlowAllowed(m_bFullFlow))
        {
        pG->LQLInx()  = PQI++;   // Unknown = QmL
        EqnOne++;
        }

      pG->EqnCnt()=EqnOne-pG->EqnOne();
      JoinIter L(pG->m_Lnks);
      for (CJoinRec * pJ = L.First(); (pJ); pJ = L.Next())
        pJ->NdInx = NdInx;
      pG->m_NdInx=NdInx++;

      if (pG->m_Src.m_pJoin==pSrc && pG->m_Dst.m_pJoin->NdInx == -1)
        CalcIndicesFull2(NdInx, pSrc, pG->m_Dst.m_pJoin, NULL, PQInx, EqnOne);
      else if (pG->m_Dst.m_pJoin==pSrc && pG->m_Src.m_pJoin->NdInx == -1)
        CalcIndicesFull2(NdInx, pSrc, pG->m_Src.m_pJoin, NULL, PQInx, EqnOne);
      }
    }
  }

//-------------------------------------------------------------------------

void CSubNetBlk::CalcIndicesFull()
  {
  if (!fIndexCalculationReqd)
    return;

  CStopWatchLap SWLapCalcIndicesTicks(gs_swCalcIndicesTicks);

  fIndexCalculationReqd=0;
  fClearSS=true;

  char i;
  BldNetInx  PQInx, InxCnt;

  int EqnCnt=1;
  for (i=0; i< JTyp_NoOf ; i++)
    {
    InxCnt[i] = 0;
    PQInx[i]  = 1;
    }

  GrpFlwIter G(Grps);
  for (CGrpFlwBlk * g = G.First(); (g); g = G.Next())
    {
    if (g->m_Src.m_pJoin==g->m_Dst.m_pJoin)
      {
      g->m_fSelfLoop=1;
      int &Cnt=InxCnt[g->m_LnkInx.Typ];
#if dbgFlwNets
      if (dbgMtxCalcIndices())
        dbgpln("Cnts %-12s %3i : %3i SelfLoop %s",JoinTypeStr(g->m_LnkInx.Typ),Cnt,0, g->FwdTag());
#endif
      }
    else
      {
      g->m_fSelfLoop=0;
      //udf if (!g->fUDFShut)
      int &Cnt=InxCnt[g->m_LnkInx.Typ];
#if dbgFlwNets
      int Cnt0=Cnt;
#endif
      if (g->VFlowAllowed(m_bFullFlow))
        Cnt+=1;          // Unknown = QmV
      if (g->LFlowAllowed(m_bFullFlow))
        Cnt+=1;          // Unknown = QmL
#if dbgFlwNets
      if (dbgMtxCalcIndices())
        dbgpln("Cnts %-12s %3i : %3i          %s",JoinTypeStr(g->m_LnkInx.Typ),Cnt,Cnt-Cnt0, g->FwdTag());
#endif
      }
    g->m_NdInx=-2;
    JoinIter L(g->m_Lnks);
    for (CJoinRec * pL=L.First(); pL; pL=L.Next())
      {
      pL->NdInx=-2;
      pL->fSelfLoop=g->m_fSelfLoop;
      pL->fUDFShut=g->m_fUDFShut;
      }
    }

  JoinIter T(Ties);
  for (CJoinRec * pT = T.First(); (pT); pT = T.Next())
    {
    pT->NdInx=-2;
    pT->fSelfLoop=0;
    pT->fUDFShut=0;
    pT->fIsoNetRef=0;
    int & Cnt=InxCnt[pT->Typ];
#if dbgFlwNets
    int Cnt0=Cnt;
#endif
    Cnt++;               // Unknown = P
    if (pT->fIsImpNd || pT->fIsAccLnk)
      {
      if (pT->VFlowAllowed(m_bFullFlow))
        Cnt++;           // Unknown = QmV
      if (pT->LFlowAllowed(m_bFullFlow))
        Cnt++;           // Unknown = QmL
      if (pT->VFlowAllowed(m_bFullFlow) && pT->LFlowAllowed(m_bFullFlow) && pT->V2LReqd(m_bFullFlow))
        Cnt++;           // Unknown = Xfer Q
      }
#if dbgFlwNets
    if (dbgMtxCalcIndices())
      dbgpln("Cnts %-12s %3i : %3i          %s",JoinTypeStr(pT->Typ),Cnt,Cnt-Cnt0, pT->Nd()->Tag());
#endif
    }


  fNetWrk = 1;
  fIsolatedNet  = ((InxCnt[JTyp_Edge] == 0) &&
    (InxCnt[JTyp_Link] != 0) &&
    (InxCnt[JTyp_Tie] > 0) &&
    (InxCnt[JTyp_ImpNd] == 0) &&
    (InxCnt[JTyp_AccLnk] == 0));
  fIsolatedNode = ((InxCnt[JTyp_Edge] == 0) &&
    (InxCnt[JTyp_Link] == 0));

  if (fIsolatedNet)
    {
    // Must add an Edge to connect the network to a reference Pressure
    // Find Tie with lowest Pressure
    CJoinRec * pTLow = NULL;
    double PLow = 1.0e100;
    for (CJoinRec * pT = T.First(); (pT); pT = T.Next())
      if (pT->m_PB.P < PLow)
        {
        PLow=pT->m_PB.P;
        pTLow=pT;
        }
      pTLow->fIsoNetRef=1;
      ASSERT(InxCnt[JTyp_Edge]==0);
#if dbgFlwNets
      int Cnt0=InxCnt[JTyp_Tie];
#endif
      if (pTLow->VFlowAllowed(m_bFullFlow))
        InxCnt[JTyp_Tie]++; // Make Space for a pseudo Edge Vap
      if (pTLow->LFlowAllowed(m_bFullFlow))
        InxCnt[JTyp_Tie]++; // Make Space for a pseudo Edge Liq
#if dbgFlwNets
      if (dbgMtxCalcIndices())
        dbgpln("Cnts %-12s %3i : %3i %s",JoinTypeStr(JTyp_Tie),InxCnt[JTyp_Tie],InxCnt[JTyp_Tie]-Cnt0, "ISOLATED_NET");
#endif
    }


  iMtxOrder=0;
  for (int nt=JTyp_Edge; nt<=JTyp_AccLnk; nt=nt+1)
    {
    PQInx[nt+1] = PQInx[nt] + InxCnt[nt];
    iMtxOrder = Max(iMtxOrder, PQInx[nt+1]-1);
    }


#if dbgFlwNets
  if (dbgMtxCalcIndices())
    {
    dbgpln("%-12s Cnt Inx", "CalcIndices");
    for (int nt=JTyp_Unknown; nt<JTyp_NoOf; nt++)
      dbgpln("%-12s %3i %3i ", JoinTypeStr(nt), InxCnt[nt], PQInx[nt]);
    dbgpln("Matrix Order=%i", iMtxOrder);
    }
#endif

  m_CalcIndexSeq.RemoveAllToFreeList();
  CalcIndicesFull1(NULL, m_pFirstJoin, NULL);

  int NdInx=0;
  int EqnOne=1;
  CalcIndicesFull2(NdInx, NULL, m_pFirstJoin, NULL, PQInx, EqnOne);

  if (fIsolatedNet)
    {
    //   JTyp_Tie

    }

#if dbgFlwNets
  if (dbgMtxCalcIndices())
    {
    char Buff[256];
    dbgpln("Start CalcIndices");
    dbgpln("%-10s [ Nd] io Eqn1  Cnt  P       V...            L...            V2L Iso/Iso", "");

    CJoinGrpIter Seq(m_CalcIndexSeq);
    for (CJoinGrpPtr * p=Seq.First() ; p; p=Seq.Next())
      {
      if (!p->IsGrp())
        {
        CJoinRec * pJ=p->Join();
        dbgpln("%-10s [%3i]    %4i %4i  %s  %s[%i]", JoinTypeStr(pJ->Typ),
          pJ->NdInx, pJ->EqnOne(), pJ->EqnCnt(),
          TIndicesStr(Buff, pJ), pJ->Nd()->Tag(),pJ->iJoinArrayInx);
        }
      else
        {
        CGrpFlwBlk * pG=p->Grp();
        if (pG->m_fSelfLoop)
          dbgpln("%-10s [   ]    %4s %4s  %s  %s", JoinTypeStr(pG->m_LnkInx.Typ),
          "Self", "Loop", LIndicesStr(Buff, pG), pG->FwdTag());
        else
          dbgpln("%-10s [   ]    %4i %4i  %s  %s", JoinTypeStr(pG->m_LnkInx.Typ),
          pG->EqnOne(), pG->EqnCnt(), LIndicesStr(Buff, pG), pG->FwdTag());
        }
      }
    dbgpln("End CalcIndices");
    }
#endif


#if dbgFlwNets
  if (dbgSubNets())
    ShowSubNet(false);
#endif
  }

//-------------------------------------------------------------------------

flag CSubNetBlk::BuildMatrixFull(long NetCnvLoop)
  {
  CStopWatchLap SWLapSolveTicks(gs_swSolveTicks);
  CStopWatchLap SWLapMtxBuildTicks(gs_swMtxBuildTicks);

  flag OK=false;

#if dbgFlwNets
  if (dbgMtxForceBuild())
    fClearSS=true;
#endif

  for (int Pass=0; Pass<MaxBuildMatrixPasses; Pass++)
    {
    if (fClearSS)
      {
      m_SS.Clear(iMtxOrder, ArrayReAnalyseRatio);
      fClearSS=false;
      fMustAnalyse = true;
      }

#if dbgFlwNets
    flag DoDbg=dbgMtxInsert() || dbgSubNets() && pNet->dbgSubNetStruct;
    DoDbg=DoDbg&&(fHasDbgBrk || !dbgMtxDbgBrk());
#endif

    JoinIter Ti(Ties);
    CJoinRec * pT;

    flag FCAllOK=pFNB->m_fAllowFastConverge && (NetCnvLoop>=6);
    if (FCAllOK)
      {
#if dbgFlwNets
      if (dbgMtxFastConverge())
        dbgpln("FastConverge:");
#endif
      for (pT=Ti.First(); pT; pT=Ti.Next())
        {
        double PMin=1e10;
        double PMax=-1e10;
        CJoinRec &T=*pT;
        for (int c=0; c<T.NConns(); c++)
          if (ConnSameSubNet(&T, c))
            {
            CGrpFlwBlk &G=*T.GFB(c);
            double PRmt;
            if (G.m_Src.Join()!=pT)
              {
              PRmt=G.m_Src.IOPBFlng()->PEst+G.m_Sum_B;//.Join()->PB.PEst+G.m_Sum_B;
#if dbgFlwNets
              if (dbgMtxFastConverge())
                dbgpln("    AtDst %10.3f = %10.3f +%10.3f (m_Src_P:%10.3f)%s", 
                PRmt, G.m_Src.IOPBFlng()->PEst, G.m_Sum_B, G.m_Src_P, G.m_Src.Nd()->FullObjTag());
#endif
              }
            else
              {
              PRmt=G.m_Dst.IOPBFlng()->PEst-G.m_Sum_B;//.Join()->PB.PEst-G.m_Sum_B;
#if dbgFlwNets
              if (dbgMtxFastConverge())
                dbgpln("    AtSrc %10.3f = %10.3f -%10.3f (m_Dst_P:%10.3f)%s", 
                PRmt, G.m_Dst.IOPBFlng()->PEst, G.m_Sum_B, G.m_Dst_P, G.m_Dst.Nd()->FullObjTag());
#endif
              }
            PMin=Min(PMin, PRmt);
            PMax=Max(PMax, PRmt);
            }
          pT->fFastConvergeOK=PMin<=pT->m_PB.PEst && PMax>=pT->m_PB.PEst;
          FCAllOK=FCAllOK && pT->fFastConvergeOK;

#if dbgFlwNets
          if (dbgMtxFastConverge())
            dbgpln("  %s PMin:%10.3f PNd:%10.3f PMax:%10.3f %s", 
            pT->fFastConvergeOK?"Yes":"No ", PMin, pT->m_PB.PEst, PMax, pT->Nd()->FullObjTag());
#endif
        }
      }

    if (FCAllOK)
      {
      GrpFlwIter G(Grps);
      for (CGrpFlwBlk * g=G.First(); g; g=G.Next())
        {
        if (!g->m_fSelfLoop)
          {
          g->m_fFastConvergeOK=(!g->m_fHoldFastConverge &&
            g->m_Src.Join()->fFastConvergeOK && 
            g->m_Dst.Join()->fFastConvergeOK);

          if (g->m_fFastConvergeOK)
            {
            double Q=g->m_QmIn;
            double DQ=0.001*Q;
            double SumB=g->m_Sum_B;
            double SumR=g->m_Sum_ResT;
            double SumDP=g->m_Sum_dP;

            g->SetLinkQmEstAndQm(Q-DQ);
            g->Traverse("FastCvg1");
            double SumB1=g->m_Sum_B;
            double SumR1=g->m_Sum_ResT;
            double SumDP1=g->m_Sum_dP;

            g->SetLinkQmEstAndQm(Q+DQ);
            g->Traverse("FastCvg2");
            double DSumB=g->m_Sum_B-SumB1;
            double DSumR=g->m_Sum_ResT-SumR1;
            double DSumDP=g->m_Sum_dP-SumDP1;

            if (fabs(DSumDP)<1.0e-6*Max(fabs(g->m_P_Applied), fabs(g->m_Sum_B)))
              g->m_fFastConvergeOK=false;
            else if (!FlwExists(Q))
              g->m_fFastConvergeOK=false;
            else
              {
              g->m_Sum_ROp=fabs(DSumDP/NZ(2*DQ));
              g->m_Sum_BOp=-(g->m_P_Applied-g->m_Sum_ROp*Q);
              }

            //// restore
            //g->m_Sum_B=SumB;
            //g->Sum_R=SumR;
            //g->m_Sum_dP=SumDP;
            //
            //if (0)
            //  {
            //  g->SetLinkQmEstAndQm(Q);
            //  g->Traverse("Restore");
            //  }
#if dbgFlwNets
            if (dbgMtxFastConverge())
              dbgpln("FastCvg Q:%*.*f B:%*.*f R:%*.*f Rop:%*.*f BOp:%*.*f Q*ROp:%*.*f SumDP:%*.*f %s", 
              QWide,QPrec,Q, 
              PWide,PPrec,SumB, 
              RWide,RPrec,SumR, 
              RWide,RPrec,g->m_Sum_ROp, 
              PWide,PPrec,g->m_Sum_BOp, 
              PWide,PPrec,Q*g->m_Sum_ROp, 
              PWide,PPrec,SumDP, 
              g->FwdTag());
#endif
            }
          }
        }
      }
    else
      {
      GrpFlwIter G(Grps);
      for (CGrpFlwBlk * g=G.First(); g; g=G.Next())
        g->m_fFastConvergeOK=false;
      }

    //#if NETMAXRESRATIO
    //double LowestRes=1e100;
    //CGrpFlwBlk * pLoGrp=NULL;
    //#endif
    GrpFlwIter G(Grps);
    for (CGrpFlwBlk * g=G.First(); g; g=G.Next())
      if (!g->m_fSelfLoop)
        {

        g->m_BstDamping=Range(0.0, g->m_BstDamping, 1.0);

        // Alternate Damping
        //        if (0)
        //          {
        //          double GrpDampFactor=0.0;
        //          g->RDamp.MeasAdvance(g->Sum_R, g->BstDamping, GrpDampFactor);
        //          g->BDamp.MeasAdvance(g->m_Sum_B, g->BstDamping, GrpDampFactor);
        //          g->m_Sum_RSmth=g->RDamp.SmoothIt(GrpDampFactor);
        //          g->m_Sum_BSmth=g->BDamp.SmoothIt(GrpDampFactor);
        //          //dbgpln("RB %8.3f  %14.6g > %14.6g   %14.6g > %14.6g  %s", GrpDampFactor, g->m_Sum_RSmth, g->Sum_R, g->m_Sum_BSmth, g->m_Sum_B, g->Tag());
        //          }
        //        else
          {
          ////TODO REMOVE Dampin
          //g->BstDamping=Max(0.4, g->BstDamping);

          if (Valid(g->m_Sum_BSmth))
            {
            if (g->m_BstDamping>0.0)
              { int xxx=0; }
            g->m_Sum_BSmth=g->m_BstDamping*g->m_Sum_BSmth + (1.0-g->m_BstDamping)*g->m_Sum_B;
            g->m_Sum_RSmth=g->m_BstDamping*g->m_Sum_RSmth + (1.0-g->m_BstDamping)*g->m_Sum_ResT;
            g->m_Sum_ROpSmth=g->m_Sum_ROp;
            g->m_Sum_BOpSmth=g->m_Sum_BOp;
            }
          else
            {
            g->m_Sum_BSmth=g->m_Sum_B;
            g->m_Sum_RSmth=g->m_Sum_ResT;
            g->m_Sum_ROpSmth=g->m_Sum_ROp;
            g->m_Sum_BOpSmth=g->m_Sum_BOp;
            }
          }

        OK=true;
        //#if NETMAXRESRATIO
        //if (g->Sum_R<LowestRes)
        //  {
        //  LowestRes=g->m_Sum_RSmth;
        //  if (LowestRes==0.0)
        //    BreakPoint();
        //  pLoGrp=g;
        //  }
        //#endif
        }

      //#if NETMAXRESRATIO
      //    // NBNB Choice of NetMaxResRatio is very important
      //    double NetMaxRes=Max(MinimumResistance, LowestRes)*CFullNetBlk::NetMaxResRatio;
      //
      //#if dbgFlwNets
      //    if (fDoDebug)
      //      dbgpln("NetRes  %14.5g > %14.5g ", LowestRes, NetMaxRes);//, G.First()->RootNd()->FullObjTag());
      //#endif
      //#endif

      for (g=G.First(); g; g=G.Next())
        if (!g->m_fSelfLoop)
          {
          if (g->m_fHasDbgBrk)
            BreakPoint()
            OK=true;
          g->m_HiImp.fLimit=false;
          //if (g->fFastConvergeOK && g->m_Sum_ROpSmth<1.0e-10)
          double LoRes=1e100;
          for (int Lp=m_Lp0; Lp<m_LpN; Lp++)
            {
            if (DoJoinFlow(*g, Lp))
              {
              double PhFrac=DoJoinOneFlow(*g) ? 1.0 : GTZ(g->m_Src.m_pFBJn->PhaseFrac(NULL, Lp));
              switch (Lp)
                {
                case 0:
                  PhFrac*=GTZ(1.0-g->m_Src.m_pFBJn->VThrottle());
                  //#if NETMAXRESRATIO
                  //g->dVResMtx=AdjustR(g->m_Sum_RSmth/PhFrac, g->dVResAdj, NetMaxRes);
                  //#else
                  g->m_dVResMtx=(g->m_fFastConvergeOK?g->m_Sum_ROpSmth:g->m_Sum_RSmth)/PhFrac;
                  g->m_dVResAdj=1.0;
                  LoRes=Min(LoRes, g->m_dVResMtx);
                  //#endif
                  break;
                case 1:
                  PhFrac*=GTZ(1.0-g->m_Src.m_pFBJn->LThrottle());
                  //#if NETMAXRESRATIO
                  //g->dLResMtx=AdjustR(g->m_Sum_RSmth/PhFrac, g->dLResAdj, NetMaxRes);
                  //#else
                  g->m_dLResMtx=(g->m_fFastConvergeOK?g->m_Sum_ROpSmth:g->m_Sum_RSmth)/PhFrac;
                  g->m_dLResAdj=1.0;
                  LoRes=Min(LoRes, g->m_dLResMtx);
                  //#endif
                  break;
                }

              //dbgpln("%2i %12.4g %12.4g %12.4g %12.4g %s", Lp, g->m_dLResMtx, g->m_Sum_ROpSmth, g->m_Sum_RSmth, PhFrac, g->FwdTag());
              }
            }
          }


        for (g=G.First(); g; g=G.Next())
          if (!g->m_fSelfLoop)
            {

            OK=true;
            //        #if NETMAXRESRATIO
            //        g->HiImp.fLimit=false;
            //        if (g->Sum_R>NetMaxRes)
            ////          {
            //          g->HiImp.fLimit=true;
            ////          g->HiImp.pLoGrp=pLoGrp;
            ////          }
            //       #endif

            double Sens=1e30;
            g->m_SigPVInEqn  = UnUsedIndex;
            g->m_SigPLInEqn  = UnUsedIndex;
            int PFndEqn    = UnUsedIndex;
            int i=g->EqnOne();
            if (g->VFlowAllowed(m_bFullFlow))
              g->m_SigPVInEqn=i++;
            if (g->LFlowAllowed(m_bFullFlow))
              g->m_SigPLInEqn=i++;
            int SInx   = g->PSrcInx();
            int DInx   = g->PDstInx();
            for (int Lp=m_Lp0; Lp<m_LpN; Lp++)
              {
              if (DoJoinFlow(*g, Lp))
                {
                double R;
                int QInx, SigPInEqn;
                switch (Lp)
                  {
                  case 0:
                    QInx = g->LQVInx();
                    SigPInEqn = g->m_SigPVInEqn;
                    R=g->m_dVResMtx;
                    break;
                  case 1:
                    QInx = g->LQLInx();
                    SigPInEqn = g->m_SigPLInEqn;
                    R=g->m_dLResMtx;
                    break;
                  }
                // Equation : Sigma DP = 0;
                m_SS.SetValue(SigPInEqn, QInx, R);
                m_SS.SetValue(SigPInEqn, SInx, -1);
                double P=0;
                switch (g->m_LnkInx.Typ)
                  {
                  case JTyp_Link:
                    P=(g->m_fFastConvergeOK?g->m_Sum_BOpSmth:g->m_Sum_BSmth);
                    m_SS.SetValue(SigPInEqn, DInx, 1);
                    m_SS.SetVector(SigPInEqn, P);
                    break;
                  case JTyp_Edge:
                    P=((g->m_fFastConvergeOK?g->m_Sum_BOpSmth:g->m_Sum_BSmth)-g->m_End_P);
                    m_SS.SetVector(SigPInEqn, P);
                    break;
                  }

#if dbgFlwNets
                if (DoDbg)
                  {
                  double RFracChg, BFracChg;
                  TstChg(R, g->m_Sum_RPrev[Lp], 1.0, RFracChg);
                  TstChg(P, g->m_Sum_BPrev[Lp], 1.0, BFracChg);

                  dbgp("%s Ins%s:{%3i,.} ",Lp?"L":"V",JoinTypeStr(g->m_LnkInx.Typ),SigPInEqn);
                  dbgp("%s ",  g->m_fFastConvergeOK?"Y":"n");
                  dbgp("%#16.9g[%+#11.6f%%]",R, 100*RFracChg);
                  //              dbgp(" {.,%3i} %2i",SInx,-1);
                  switch (g->m_LnkInx.Typ)
                    {
                    case JTyp_Link:
                      //                  dbgp(" [%3i,.] 1",DInx);
                      dbgp(" B: %#16.9g[%+#11.6f%%]", P,100*BFracChg);
                      break;
                    case JTyp_Edge:
                      //                  dbgp("          ");
                      //                  P=g->m_Sum_BSmth-g->m_End_P;
                      dbgp(" BP:%#16.9g[%+#11.6f%%]", P,100*BFracChg);
                      break;
                    default:
                      dbgp("               ");
                    }
                  dbgpln(" Q:%#16.9g  %s ",g->m_QmEstIn,g->FwdTag());
                  if (g->m_QmRatio.MFb!=NULL)
                    rGQmRatioRec QmR = g->m_QmRatio;
                  }
#endif
                g->m_Sum_RPrev[Lp]=R;
                g->m_Sum_BPrev[Lp]=P;
                }
              }

            }

          //JoinIter Ti(Ties);

          for (pT=Ti.First(); pT; pT=Ti.Next())
            {
            CJoinRec &T=*pT;

#if dbgFlwNets
            int nSigs=0;
            int Sigs[256];
            if (DoDbg)
              dbgp("Ins%s:[%3i,.] ", JoinTypeStr(T.Typ),T.PInx());
#endif

            if (T.pNd->fDoDbgBrk)
              BreakPoint();
            // Equation : Sigma Q=0;
            int IsoRefEqn=UnUsedIndex;
            int SigQmVEqn=UnUsedIndex;
            int SigQmLEqn=UnUsedIndex;
            int SigPEqn  =UnUsedIndex;
            int V2LQmEqn =UnUsedIndex;
            int Eqn=T.EqnOne();


            if (T.fIsImpNd || T.fIsAccLnk)
              {
              OK=true;
              SigPEqn=Eqn++;
              if (T.VFlowAllowed(m_bFullFlow))
                SigQmVEqn=Eqn++;
              if (T.LFlowAllowed(m_bFullFlow))
                SigQmLEqn=Eqn++;
              if (T.TQV2LInx()>0) // From Vapour to Liquid
                V2LQmEqn=Eqn++;
              }
            else
              {
              //#pragma chNOTE(To be improved)
              SigQmVEqn=Eqn;
              SigQmLEqn=Eqn;
              Eqn++;
              }

            if (T.fIsoNetRef)
              IsoRefEqn=Eqn++;

            for (int Lp=m_Lp0; Lp<m_LpN; Lp++)
              if (DoJoinFlow(T, Lp))
                {
                int Eqn=(Lp==0) ? SigQmVEqn : SigQmLEqn;
                for (int c=0; c<T.NConns(); c++)
                  if (ConnSameSubNet(&T, c))
                    {
                    CGrpFlwBlk &G=*T.GFB(c);
                    if (G.LQInx(Lp)>=0)
                      {
                      int iQInx=G.LQInx(Lp);

                      m_SS.SetValue(Eqn, iQInx, T.QSgn(c));
#if dbgFlwNets
                      Sigs[nSigs++]= T.QSgn(c)*iQInx;
#endif
                      }
                    }

                  double Q1=-T.SigmaQm(Lp);
                  if (T.pNd->fDoDbgBrk)
                    BreakPoint();
                  if (FlwLarge(Q1))
                    {
                    Q1=0.0;
                    T.Nd()->SetCI(15);
                    }
                  else
                    T.Nd()->ClrCI(15);
                  Q1+=-T.GetQmSink(Lp);
                  //          Q1-=((Lp==0) ? -1 : 1) * T.dV2LQmReqd;
                  Q1=ChkZero(Q1);
                  double Q2=0.0;

                  if (T.fHasQmAux)
                    {
                    JoinQmAux &QA = *T.pQmAux;
                    if (QA.HasRqdQm())
                      Q2= -ChkZero(-QA.dRqdQm);
                    }

                  double VecVal=Q1+Q2;
                  if ((SigQmVEqn==SigQmLEqn) && (Lp>0))
                    {
                    VecVal+=m_SS.GetVector(Eqn);
                    }
                  m_SS.SetVector(Eqn, VecVal);

#if dbgFlwNets
                  if (DoDbg)
                    {
                    if ((SigQmVEqn==SigQmLEqn) && (Lp>0))
                      dbgp(" SigQ# %#16.7g",VecVal);
                    else
                      dbgp(" SigQ  %#16.7g",VecVal);
                    //            if (T.fHasQmAux && T.pQmAux->HasRqdQm())
                    //              dbgp(" L %#16.7g ",Q2);
                    }
#endif
                }


              if (T.fIsImpNd || T.fIsAccLnk)
                {
                // Connect Delta Q
                double P=T.pNd->JoinP_Good(T.m_iJoinId);
#if dbgFlwNets
                if (DoDbg)
                  dbgp(" ImpNd P:%#16.7g",P);
#endif
                for (int Lp=m_Lp0; Lp<m_LpN; Lp++)
                  if (DoJoinFlow(T, Lp))
                    {
                    int QAccEqn=(Lp==0) ? SigQmVEqn : SigQmLEqn;
                    if (SigQmVEqn==SigQmLEqn)
                      DoBreak();

                    m_SS.SetValue(QAccEqn, T.TQAccInx(Lp), -1);      // Sigma Q = 0.0

                    int N=0;
                    for (int c=0; c<T.NConns(); c++)
                      {
                      CGrpFlwBlk &G=*T.GFB(c);
                      if (G.LQInx(Lp)>0)
                        {
                        double Sens=Lp ? T.pNd->IO_PSensL(T.IONo(c)) : T.pNd->IO_PSensV(T.IONo(c));
                        if (Sens==0.0) // ? Not Set
                          Sens=1.0e6;
                        Sens*=ICGetTimeInc();
                        m_SS.SetValue(SigPEqn, G.LQInx(Lp), Sens*T.QSgn(c));
                        N++;
                        }
                      }

                    //            m_SS.SetValue(SigPEqn, T.PInx(), -2*N);      // Final P In Node
                    //            m_SS.SetVector(SigPEqn, -Max(2*N*P, 1.0));     // Init P In Node
                    m_SS.SetValue(SigPEqn, T.PInx(), -1);      // Final P In Node
                    m_SS.SetVector(SigPEqn, -Max(P, 1.0));     // Init P In Node
                    //            #if dbgFlwNets
                    //            if (fDoDebug)
                    //              dbgp(" %sS:%#16.7g ",(Lp?"L":"V"),Sens);
                    //            #endif

                    // Static Head sensitivities
                    if (WithRhoHSens)
                      for (int c=0; c<T.NConns(); c++)
                        {
                        CGrpFlwBlk &G=*T.GFB(c);
                        double S=(Lp==0?T.pNd->IO_RhoHSensV(T.IONo(c)):T.pNd->IO_RhoHSensL(T.IONo(c)));
                        S*=ICGetTimeInc();

                        if (fabs(S)>1.0e-6)//LowestRes)
                          {
                          int SigPEqn=(Lp==0?G.m_SigPVInEqn:G.m_SigPLInEqn);
                          // Only applies to Liquid Acc
                          m_SS.SetValue(SigPEqn, T.TQAccInx(1), S*T.QSgn(c));
                          }
                        }
                    }

                  if (T.TQV2LInx()>0) // From Vapour to Liquid
                    {
                    // Other Sigma Q's
                    m_SS.SetValue(SigQmVEqn, T.TQV2LInx(), -1);
                    m_SS.SetValue(SigQmLEqn, T.TQV2LInx(), 1);

                    m_SS.SetVector(V2LQmEqn, -T.dV2LQmReqd);
#if dbgFlwNets
                    if (DoDbg)
                      dbgp("\n   V2L:%#16.7g ",-T.dV2LQmReqd);
#endif

                    // Qx=Sig Q ...
                    m_SS.SetValue(V2LQmEqn, T.TQV2LInx(), -1);

                    for (int c=0; c<T.NConns(); c++)
                      {
#if dbgFlwNets
                      if (DoDbg)
                        dbgp("\n                         (%i)VFct:%#16.7g LFct:%#16.7g ",c,T.IO_Self(c)->dV2LQmVFact, T.IO_Self(c)->dV2LQmLFact );
#endif
                      CGrpFlwBlk &G=*T.GFB(c);
                      for (int Lp=m_Lp0; Lp<m_LpN; Lp++)
                        {
                        if (G.LQInx(Lp)>=0)
                          {
                          int iQInx=G.LQInx(Lp);
                          double Fact=(Lp==0 ? T.IO_Self(c)->dV2LQmVFact : T.IO_Self(c)->dV2LQmLFact);
                          if (fabs(Fact) > V2LLimit)
                            m_SS.SetValue(V2LQmEqn, iQInx, Fact*T.QSgn(c));
                          }
                        }
                      }
                    }
                }

              if (T.fIsoNetRef)
                {
                int QInx=T.TQIsoInx();//
                int SInx=T.PInx();
                // Resistance Value is not very important Flow should be zero
                if (T.VFlowAllowed(m_bFullFlow))
                  {
                  m_SS.SetValue(IsoRefEqn,   QInx, 1.230);
                  m_SS.SetValue(IsoRefEqn,   SInx, -1.0);
                  m_SS.SetVector(IsoRefEqn, -Max(T.m_PB.P, Std_P));
                  // Equation : Sigma Q=0 for source Node
                  m_SS.SetValue(SigQmVEqn,   QInx, 1.0);
                  IsoRefEqn++;
                  QInx++;
                  }
                if (T.LFlowAllowed(m_bFullFlow))
                  {
                  m_SS.SetValue(IsoRefEqn,   QInx, 1.230);
                  m_SS.SetValue(IsoRefEqn,   SInx, -1.0);
                  m_SS.SetVector(IsoRefEqn, -Max(T.m_PB.P, Std_P));
                  // Equation : Sigma Q=0 for source Node
                  m_SS.SetValue(SigQmLEqn,   QInx, 1.0);
                  }
                }

#if dbgFlwNets
              if (DoDbg)
                {
                dbgp(" %s[%i] Conns:",T.pNd->FullObjTag(), T.iJoinArrayInx);
                for (int x=0; x<nSigs; x++)
                  dbgp("%+i ", Sigs[x]);
                dbgpln("");
                }
#endif
            }
          if (!m_SS.ReAnalysisReqd())
            break;

          //CBldSeqItem &ele=m_SS.ProblemElement();
          //dbgpln("ReAnalyse Matrix #%i  %i,%i  = %g [%g]", m_SS.Changes(), ele.row, ele.col, ele.value, m_SS.BigRatio());

          fClearSS=true;
          OK=false;
    }
  if (Pass>=MaxBuildMatrixPasses)
    LogError("FlowSolver", 0, "Matrix BuildError - Too many Passes");

  return OK;
  }

//-------------------------------------------------------------------------

flag CSubNetBlk::SolveMatrixFull(long NetCnvLoop)
  {
  CStopWatchLap SWLapSolveTicks(gs_swSolveTicks);
  CStopWatchLap SWLapMtxSolveTicks(gs_swMtxSolveTicks);

  bool DoDirect=true;
  int Err=m_SS.Initialise(DoDirect);
  if (fMustAnalyse)
    {
#if dbgFlwNets
    if ((fHasDbgBrk || !dbgMtxDbgBrk()) && dbgMtxRowPrint())
      {
      m_SS.RowPrint((iMtxOrder > 0), MWide, MPrec);
      dbgfileflush(1);
      dbgfileflush(0);
      }
#endif

    int Err=m_SS.Analyse();

#if dbgFlwNets
    if (!DoDirect && (fHasDbgBrk || !dbgMtxDbgBrk()) && dbgMtxRowPrint())
      {
      m_SS.RowPrint((iMtxOrder > 0), MWide, MPrec);
      dbgfileflush(1);
      dbgfileflush(0);
      }
#endif

#if dbgFlwNets
    if ((fHasDbgBrk || !dbgMtxDbgBrk()) && dbgMtxRowPrint())
      {
      Mtx_Info info;
      m_SS.Report_Stats(&info);

      dbgpln("Elements NonZero/ FillIns  %7i  / %7i", info.nonzerocount, info.num_fill_ins);
      dbgpln("Sparcity Before / After    %7.3f -> %7.3f%%", info.sparcity_before, info.sparcity_after);
      dbgpln("-----------------------");

      }
#endif
    fMustAnalyse = false;
    fAnalyseOK = (Err==SMTX_NONE);
    if (!fAnalyseOK)
      {
      Strng Tag("???");
      if (Grps.GetCount()>0)
        Tag=Grps.GetHead()->FwdTag();
      LogError("Solver", 0, "Matrix Analysis Error '%s' %s",CSparseSolver::ErrorString(Err), Tag());

      m_SS.Clear(iMtxOrder, ArrayReAnalyseRatio);
      fClearSS=false;
      fMustAnalyse = true;
      //fSubNetSolved = false;

#if (dbgFlwNets)
      long Old=dbgMtxInsert.PgmSet(1);
#endif
      BuildMatrixFull(NetCnvLoop);
#if (dbgFlwNets)
      dbgMtxInsert.PgmSet(Old);
#endif
      }
    }
  else
    {
    int xxx=0;
    }

  if (fAnalyseOK)
    {
    if (m_SS.Solve()!=SMTX_NONE)
      fAnalyseOK=0; // Prevent Solution
    }

  //dbgpln("MtxSolve:%12.6f",pFNB->SolveTicks.Lap());

  //dbgpln("MtxSolve:%12.6f",pFNB->SolveTicks.Lap());
  return fAnalyseOK;
  }

//-------------------------------------------------------------------------

flag CSubNetBlk::ExtractMatrixFull(long NetCnvLoop)
  {
  CStopWatchLap SWLapSolveTicks(gs_swSolveTicks);
  CStopWatchLap SWLapMtxExtractTicks(gs_swMtxExtractTicks);

  if (fAnalyseOK)
    {

    GrpFlwIter G(Grps);
#if dbgFlwNets
    long DoDbg1=(fHasDbgBrk || !dbgMtxDbgBrk()) && dbgMtxGetSoln();
    long DoDbg2=(fHasDbgBrk || !dbgMtxDbgBrk()) && dbgMtxGetSolnChg();
    if (DoDbg1 || DoDbg2)
      {
      for (int Lp=m_Lp0; Lp<m_LpN; Lp++)
        {
        for (CGrpFlwBlk * g=G.First(); g; g=G.Next())
          {
          if ((g->m_LnkInx.Typ==JTyp_Link || g->m_LnkInx.Typ==JTyp_Edge) && !g->m_fSelfLoop)//udf  && !g->fUDFShut)
            {
            if (DoJoinFlow(*g, Lp))
              {
              double FracErrQm;
              int QInx = g->LQInx(Lp);
              double Qm = m_SS.Solution(QInx);
              double PrevQm=g->PrevQm(Lp);
              DoDbg2=TstChg(Qm, PrevQm, dbgMtxGetSolnChgTol, FracErrQm);
              if (DoDbg1 || DoDbg2)
                {
                dbgp("%s %s %s:[%3i]   Flow  %#16.9g[%+#11.6f%%] Adj:%10.3e",Lp?"L":"V",
                  "Soln","   ",QInx,Qm,100.0*FracErrQm,g->ResAdj(Lp));
                //dbgp("%s %s %s:[%3i]   Flow  %#16.9g[%+#11.6f%%] Adj:%10.3e",Lp?"L":"V",
                //  "Soln", SolnStr(SolutionType()),QInx,Qm,100.0*FracErrQm,g->ResAdj(Lp));
                dbgp(" %s", g->FwdTag());
                dbgpln("");
                }
              g->SetPrevQm(Lp, Qm);
              }
            }
          }
        }
      }
#endif

    for (CGrpFlwBlk * g=G.First(); g; g=G.Next())
      {
      if ((g->m_LnkInx.Typ==JTyp_Link || g->m_LnkInx.Typ==JTyp_Edge) && !g->m_fSelfLoop)//udf  && !g->fUDFShut)
        {
        // Flow In to GFb
        double Qm=0.0;
        FlwBlk & FB = *g->m_Src.m_pFBJn;
        double Fill=1.0-FB.VapVolFrac(NULL);
        if (g->LQVInx()>=0)
          {
          double PhFrac=GTZ((1.0-Fill)*(1.0-FB.VThrottle()));
          Qm+=m_SS.Solution(g->LQVInx())*g->VResAdj();
          }
        if (g->LQLInx()>=0)
          {
          double PhFrac=GTZ(Fill*(1.0-FB.LThrottle()));
          Qm+=m_SS.Solution(g->LQLInx())*g->LResAdj();
          }

        g->SetLinkQmEstAndQm(Qm);
        }
      }

    JoinIter Ti(Ties);
    for (CJoinRec * pTie=Ti.First(); pTie; pTie=Ti.Next())
      {
      CJoinRec &T=*pTie;
      ASSERT(T.fIsTie && (T.Typ==JTyp_Tie || T.Typ==JTyp_ImpNd || T.Typ==JTyp_AccLnk));
      int PInx = T.PInx();
      double PEst = m_SS.Solution(PInx);
      double PSafe = Max(LowPressure, PEst);

      if (T.fIsImpNd || T.fIsAccLnk)
        {
        //        QAccInx=PInx+1;
        for (int Lp=m_Lp0; Lp<m_LpN; Lp++)
          if (DoJoinFlow(T, Lp))
            {
            double QmAcc=m_SS.Solution(T.TQAccInx(Lp));
            T.SetQmAccum(Lp, QmAcc);
            if (WithRhoHSens && (Lp==1)) // Only Liquid
              for (int c=0; c<T.NConns(); c++)
                {
                //CGrpFlwBlk &G=*T.GFB(c);
                int IONo=T.IONo(c);
                double S=T.pNd->IO_RhoHSensL(IONo);
                if (fabs(S)>1.0e-6)//LowestRes
                  T.pNd->IOFB(IONo,0)->SetDPzI(-QmAcc*S*ICGetTimeInc());
                else
                  T.pNd->IOFB(IONo,0)->SetDPzI(0.0);
                }
            }
          else
            T.SetQmAccum(Lp, 0.0);

        if (T.TQV2LInx()>0) // From Vapour to Liquid
          T.SetQmV2L(m_SS.Solution(T.TQV2LInx()));
        }
      else
        {
        T.SetQmAccum(0, 0.0);
        T.SetQmAccum(1, 0.0);
        }

#if dbgFlwNets
      long DoDbg1=(fHasDbgBrk || !dbgMtxDbgBrk()) && dbgMtxGetSoln(T.Nd()->Tag());
      long DoDbg2=(fHasDbgBrk || !dbgMtxDbgBrk()) && dbgMtxGetSolnChg(T.Nd()->Tag());
      if (DoDbg1 || DoDbg2)
        {
        double FracErr;
        DoDbg2=TstChg(PEst, T.m_PB.PEst, dbgMtxGetSolnChgTol, FracErr);
        if (DoDbg1 || DoDbg2)
          {
          //dbgp("  Soln %s:[%3i]   Press %#16.9g [%+#11.6f%%] ", SolnStr(SolutionType()),PInx,P,100.0*FracErr);
          dbgp("  Soln %s:[%3i]   Press %#16.9g %s [%+#11.6f%%] ", "   ",
            PInx,PEst,PEst!=PSafe?">>1":"   ",100.0*FracErr);
          if (T.fIsImpNd || T.fIsAccLnk)
            {
            for (int Lp=m_Lp0; Lp<m_LpN; Lp++)
              if (DoJoinFlow(T, Lp))
                dbgp(" %sQmAcc %#16.9g ",Lp?"L":"V", m_SS.Solution(T.TQAccInx(Lp)));
            if (T.TQV2LInx()>0) // From Vapour to Liquid
              dbgp(" Xfer %#16.7g",m_SS.Solution(T.TQV2LInx()));
            }
          dbgpln(" %s", T.Nd()->FullObjTag());
          }
        }
#endif

      if (0)
        {
        PEst = 0.5 * (T.m_PB.PEst + PEst);
        PSafe = Max(LowPressure, PEst);
        }
      T.m_PB.P = PSafe;
      T.m_PB.PEst = PEst;
      FlwNode &Nd=*T.Nd();
      for (int k = 0; k<T.m_nConns; k++)
        {
        if (T.IsIO(k))
          {
          int io=T.IONo(k);
          Nd.Set_IOP_Self(io, PSafe);
          Nd.Set_IOP_Est_Self(io, PEst);
          }
        }
      Nd.Set_JoinP(T.m_iJoinId, PSafe);
      Nd.Set_JoinP_Est(T.m_iJoinId, PEst);
      }
    }

  return fAnalyseOK;
  }

//-------------------------------------------------------------------------

void CSubNetBlk::CheckJoinFlows( CJoinRec * pTie)
  {
  CStopWatchLap SWLapChkJoinsTicks(gs_swChkJoinsTicks);

  CJoinRec & J=*pTie;
  //   if (J.nProdFracs>=2)
  if (J.fMatrixChanged)
    for (int c=0; c<J.NConns(); c++)
      {
      for (int FE=0; FE<J.NIOFBs(c); FE++)
        J.IOFB(c, FE)->m_dPb=0.0;
      J.GFB(c)->Traverse("ChkJnFlw");
      }
  }

//-------------------------------------------------------------------------

void CSubNetBlk::SolveTiePressures()
  {
  JoinIter Ti(Ties);
  for (CJoinRec * pTie=Ti.First(); pTie; pTie=Ti.Next())
    {
    double PMin=1e10;
    double PMax=-1e10;
    double SumInvR=0;
    double SumPInvR=0;
    CJoinRec &T=*pTie;
    for (int c=0; c<T.NConns(); c++)
      if (1)//ConnSameSubNet(&T, c))
        {
        CGrpFlwBlk &G=*T.GFB(c);
        double PRmt, R;
        if (G.m_Src.Join()!=pTie)
          {
          PRmt=G.m_Src.IOPBFlng()->PEst+G.m_Sum_B;//.Join()->PB.PEst+G.m_Sum_B;

          //#if dbgFlwNets
          //if (dbgMtxFastConverge())
          dbgpln("    AtDst %10.3f = %10.3f +%10.3f (m_Src_P:%10.3f) Sum_REst:%10.3e %s", 
            PRmt, G.m_Src.IOPBFlng()->PEst, G.m_Sum_B, G.m_Src_P, G.m_Sum_ResT, G.m_Src.Nd()->FullObjTag());
          //#endif
          }
        else
          {
          PRmt=G.m_Dst.IOPBFlng()->PEst-G.m_Sum_B;//.Join()->PB.PEst-G.m_Sum_B;
          //#if dbgFlwNets
          //if (dbgMtxFastConverge())
          dbgpln("    AtSrc %10.3f = %10.3f -%10.3f (m_Dst_P:%10.3f) Sum_REst:%10.3e %s", 
            PRmt, G.m_Dst.IOPBFlng()->PEst, G.m_Sum_B, G.m_Dst_P, G.m_Sum_ResT, G.m_Dst.Nd()->FullObjTag());
          //#endif
          }
        R=GTZ(G.m_Sum_ResT);
        SumInvR+=1.0/R;
        SumPInvR+=PRmt/R;

        PMin=Min(PMin, PRmt);
        PMax=Max(PMax, PRmt);
        }

      double NewP=SumPInvR/SumInvR;

      dbgpln("TiePress %10.4f %10.4f > %10.4f %10.4f %s", PMin, T.m_PB.PEst, NewP, PMax, T.Nd()->Tag());
      //pT->fFastConvergeOK=PMin<=pT->PB.PEst && PMax>=pT->PB.PEst;
      //FCAllOK=FCAllOK && pT->fFastConvergeOK;

      //#if dbgFlwNets
      //if (dbgMtxFastConverge())
      //  dbgpln("  %s PMin:%10.3f PNd:%10.3f PMax:%10.3f %s", 
      //  pT->fFastConvergeOK?"Yes":"No ", PMin, pT->PB.PEst, PMax, pT->Nd()->FullObjTag());
      //#endif
    }
  }

//-------------------------------------------------------------------------

const bool DoDirectSubs=false;

flag CSubNetBlk::SolveNetFlows()
  {
  if (fIsolatedNode)
    BreakPoint();

  flag CountExceeded = false;
  flag SubNetSolved = false;
#if dbgFlwNets
  if (dbgWhere())
    dbgpln("NetCnvLoop %i %i %i %s >>>> ", pNet->NetNo, m_SubNetNo, 0, SubNetSolved?"Solved":"");
#endif
  for (int NetCnvLoop = 0; ; NetCnvLoop++)
    {
    //    LoadTiePress();
#if dbgFlwNets
    DbgFlags |= (NetCnvLoop > pFNB->m_lNetCnvCnt-2 ? DBG_CalcdQ : 0);
    if (NetCnvLoop >= pFNB->m_lNetCnvCnt-2 && dbgAutoDbgBrkOnCvgFail())
      {
      GrpFlwIter G(Grps);
      for (CGrpFlwBlk * g=G.First(); g; g=G.Next())
        {
        JoinIter L(g->m_Lnks);
        for (CJoinRec * p = L.First(); p; p = L.Next())
          {
          p->pNd->fDoDbgBrk=true;
          break;
          }
        }
      SetDBGFlags();
      }
#endif

    if (NetCnvLoop==0)
      // Must go thru Net solve at least once - leave SubNetSolved==false
      Converge(NetCnvLoop);
    // Traverse();
    else if (DoDirectSubs)
      SubNetSolved = Converge(NetCnvLoop);
    else if ((((NetCnvLoop % ConvergeModulo) != (ConvergeModulo-1)) || !SubNetTopologyOK()))
      SubNetSolved = Converge(NetCnvLoop);
    else if (pFNB->m_fAllowFastConverge)
      SubNetSolved = Converge(NetCnvLoop);

    // NBNB Expand Converge Tests to test for lack of convergence in ties etc
    bool FlowsCanChange=(!fMomentumValid || ((ICStepStart())==0) || ODE().GetOrder()<2);
    if (FlowsCanChange)
      {
      if (SubNetTopologyOK())
        {
        if (1)
          {
          CStopWatchLap SWLapEvalJoinQTicks(gs_swEvalJoinQTicks);

          //FlwNode::fMatrixChanged=0;
          JoinIter T(Ties);
          for (CJoinRec * pTie=T.First(); pTie; pTie=T.Next())
            {
            int OldnProdFracs=pTie->nProdFracs;
            pTie->nProdFracs=0;
            pTie->Nd()->EvalJoinFlows(pTie->iJoinArrayInx);
            if (OldnProdFracs!=pTie->nProdFracs)
              pTie->fMatrixChanged=1;

            CheckJoinFlows(pTie);

            if (pTie->fMatrixChanged)
              {
              pTie->fMatrixChanged=false;
              pTie->fProdFracsChgd=false;
              SubNetSolved=false;
              fIndexCalculationReqd=true;
#if dbgFlwNets
              if (dbgProdFracs())
                dbgpln("PF:MatrixChgd %s",pTie->Nd()->FullObjTag());
#endif
              }
            else if (pTie->fProdFracsChgd)
              {
              pTie->fProdFracsChgd=false;
              SubNetSolved=false;
              fIndexCalculationReqd=true;
#if dbgFlwNets
              if (dbgProdFracs())
                dbgpln("PF:fProdFracsChgd %s",pTie->Nd()->FullObjTag());
#endif
              }
            else if (pTie->fHasQmAux)
              {
              JoinQmAux &QA = *pTie->pQmAux;
              if (QA.fChanged)
                {
                QA.fChanged=false;
                SubNetSolved=false;
                }
              }
            }
          }

        if (SubNetTopologyOK())
          {
          if (DoDirectSubs) // Direct Subs
            {
            SolveTiePressures();
            }
          else
            {
            CalcIndicesFull();
            if (BuildMatrixFull(NetCnvLoop))
              {
              //dbgpln("Solve Matrix %3i %3i %3i ----------------------", pNet->NetNo, SubNetNo, NetCnvLoop);
              if (SolveMatrixFull(NetCnvLoop))
                {
                ExtractMatrixFull(NetCnvLoop);
#if dbgFlwNets
                DbgFlags|=(NetCnvLoop > pFNB->m_lNetCnvCnt-3 ? DBG_CalcdQ : 0);
#endif
                SubNetSolved = Measure(NetCnvLoop);
                }
              else
                SubNetSolved=false;
              }
            else
              SubNetSolved=false;
            }
          }
        else
          SubNetSolved=false;
        }
      else
        SubNetSolved = false;
      }
    else
      SubNetSolved=true;  // Momentum In Action ... change no flows

    CountExceeded = (NetCnvLoop > pFNB->m_lNetCnvCnt);

    if (SubNetSolved || CountExceeded)
      {
#if dbgFlwNets
      if (dbgWhere())
        dbgpln("NetCnvLoop %i %i %i %s >>>> ", pNet->NetNo, m_SubNetNo, NetCnvLoop, SubNetSolved?"Solved":"");
#endif
      break;
      }
    // Must do at least one (but no more) pass if the network
    // is going to have to be rebuild
    // if(!SubNetTopologyOK())
    if(!pNet->NetTopologyOK())
      break;
    }

  GrpFlwIter G(Grps);
  int Cnt=0;
  for (CGrpFlwBlk * g=G.First(); g; g=G.Next())
    g->m_fSetCI14=(g->m_fIsBad && CountExceeded);

#if dbgFlwNets
  if (dbgWhere() && !SubNetTopologyOK())
    dbgpln("------ Sub Net Topology Change ------");
#endif

  return SubNetSolved;
  }

//==========================================================================

flag CSubNetBlk::Solve()
  {
  //  XferDbgFlags();
  fSolved = 0;
  //fSubNetSolved = false;

  InitUDFs();

  // remember initial direction of flow
  GrpFlwIter G(Grps);
  for (CGrpFlwBlk * g=G.First(); g; g=G.Next())
    {
    int InitDirn=0;
    if (!g->m_Src.IsIO())
      InitDirn=g->m_Src.LinkFB()->QmSign();
    else if (g->m_Src.NIOFBs()>0)
      InitDirn=-g->m_Src.IOFB(0)->QmSign();
    else if (g->m_Dst.IsIO() && g->m_Dst.NIOFBs()>0)
      InitDirn=g->m_Dst.IOFB(0)->QmSign();
    else
      {
      BreakPoint();
      }
    g->m_iInitialFlwDirn=InitDirn;

    g->m_fFindStaticHead=false;
    for (int f=0; f<g->m_FBs.GetSize(); f++)
      {
      if (g->m_FBs[f]->FindStaticHead())
        {
        g->m_fFindStaticHead=true;
        break;
        };
      };
    }

  for (flag UDFsOK=0; !UDFsOK; )
    {
    int FillPass=0;
    for (flag FillsOK=0; !FillsOK; FillPass++)
      {
      FillsOK=true;
      if (fXferTies)
        {
#if dbgFlwNets
        if (dbgWhere())
          { 
          dbgpln("SolveSubNet:[%i.%i] XferTies>>>>>>>>>>>>>>>>>> ",pNet->NetNo,m_SubNetNo); 
          dbgindent(+2); 
          }
#endif
        // Do Nothing
        fSolved = 1;
        }
      else if (fFixed)
        {
#if dbgFlwNets
        if (dbgWhere())
          {
          dbgpln("SolveSubNet:[%i.%i] Fixed   >>>>>>>>>>>>>>>>>> ",pNet->NetNo,m_SubNetNo); 
          dbgindent(+2); 
          }
#endif
        if (fXfer)
          SolveXferQs();
        else
          SolveFxdQs();
        fSolved = 1;
        }
      else if (fLonely)
        {
#if dbgFlwNets
        if (dbgWhere())
          { 
          dbgpln("SolveSubNet:[%i.%i] Lonely  >>>>>>>>>>>>>>>>>> ",pNet->NetNo,m_SubNetNo); 
          dbgindent(+2); 
          }
#endif
        fSolved = Converge(NLineCnvCnt);
        }
      else if (fUnDefined)
        {
#if dbgFlwNets
        if (dbgWhere())
          {
          dbgpln("SolveSubNet:[%i.%i] Undefined >>>>>>>>>>>>>>>> ",pNet->NetNo,m_SubNetNo);
          dbgindent(+2); 
          }
#endif

        fSolved = SolveUnDefined();
        }
      else
        {
#if dbgFlwNets
        if (dbgWhere())
          {
          dbgpln("SolveSubNet:[%i.%i] NetWork >>>>>>>>>>>>>>>>>> ",pNet->NetNo,m_SubNetNo); 
          dbgindent(+2);
          }
#endif
        fSolved = SolveNetFlows();
        }
#if dbgFlwNets
      if (dbgWhere())
        dbgindent(-2); 
#endif

      if (!SubNetTopologyOK())
        {
#if dbgFlwNets
        if (dbgWhere())
          dbgpln("------ Sub Net Topology Change Break ------");
#endif
        fSolved = 0;
        goto TheExit;
        }
      }

    int UDFChg=TestUDFsChange();
    if (UDFChg<0)
      {
      LogError("Solver", 0, "Too Many UDF Iterations");
      goto TheExit;
      }
    UDFsOK=(!UDFChg);
    }

TheExit:

  GrpFlwIter RG(m_RegGrps);
  for (CGrpFlwBlk * rg=RG.First(); rg; rg=RG.Next())
    rg->m_Regulator.m_pFRB->m_bHoldCvg=false;

#if (dbgFlwNets)
  if (dbgDumpFlowChgs())
    {
    for (CGrpFlwBlk * g=G.First(); g; g=G.Next())
      {
      dbglock();
      FlwBlk &I=*g->m_Src.IOFB(0);
      FlwBlk &O=*g->m_Dst.IOFB(0);
      dbgp("QmChg");
      dbgp("  In:%12.4f > %12.4f %9.4f%%", I.m_QmEstGood, I.m_QmEst, (I.m_QmEst-I.m_QmEstGood)*100/
        GTZ(Max(fabs(I.m_QmEstGood), fabs(I.m_QmEst))));
      dbgpln(" %s", g->FwdTag());
      dbgunlock();
      }
    }
#endif

  return fSolved;
  }


//==========================================================================

void CSubNetBlk::InitUDFs()
  {
  FlwBlkIter FBs(m_UDFFBs);
  for (FlwBlk *p=FBs.First(); p; p=FBs.Next())
    p->UDF.IterCount=0;
  }

//==========================================================================

int CSubNetBlk::TestUDFsChange()
  {
  int UDFsChange=0;
  if (m_UDFFBs.GetCount()>0)
    {
    double WrkDP=-1.0e30;
    FlwBlk *WrkFB=NULL;
    FlwBlkIter FBs(m_UDFFBs);
    for (FlwBlk *p=FBs.First(); p; p=FBs.Next())
      {
      double DP;
      byte Act=p->UDFReqdAction(DP);
      if (Act != UDFA_None)
        {
        UDFsChange=1;
        if (DP>=WrkDP)
          {
          WrkDP=DP;
          WrkFB=p;
          }
        }
      }
    if (UDFsChange)
      {
#if dbgFlwNets
      if (dbgDisableUDFs())
        {
        LogError("FlwNet DEBUG",0,"UDFS Disabled");
        UDFsChange=0;
        }
      else
#endif
        {
        if (WrkFB->UDF.IterCount<pFNB->m_lMaxUDFIters)
          WrkFB->UDFActivate();
        else
          return -1;
        }
      }
    }
  return UDFsChange;
  }

//==========================================================================

flag CSubNetBlk::CheckForErrors()
  {
  GrpFlwIter G(Grps);
  FlwNode * pLoInNet=NULL;
  double LowestPressNet=LowPressureTest;

  for (CGrpFlwBlk * g=G.First(); g; g=G.Next())
    {
    //    if (g->fSetCI14)
    //      LogError(g->FwdTag(), 0, "Link not converged");      

    FlwNode * pLoInGrp=NULL;
    double LowestPressGrp=fLonely||fUnDefined ? LowPressureTest : LowestPressNet;

    if (g->m_Regulator.m_pNd)
      g->m_Regulator.m_pNd->SetCI(18, g->m_Regulator.m_fSetCI18 && g->m_Regulator.m_iWhat!=FBReg_Off);

    JoinIter L(g->m_Lnks);
    for (CJoinRec * p = L.First(); p; p = L.Next())
      {
      FlwNode * pNd=p->pNd;
      pNd->SetCINoLog(4, g->m_HiImp.fLimit!=0);
      pNd->SetCINoLog(5,  g->m_fSetCI5 );
      pNd->SetCINoLog(6,  g->m_fSetCI6 );
      pNd->SetCINoLog(7,  g->m_fSetCI7 );
      pNd->SetCINoLog(8,  g->m_fSetCI8 );
      pNd->SetCI     (13, g->m_fSetCI13);
      pNd->SetCINoLog(14, g->m_fSetCI14);
      pNd->SetCINoLog(21, g->m_fSetCI21);
      pNd->SetCINoLog(22, g->m_fSetCI22);

      if (p->IsIO(0) && p->IOP_Self(0)<LowestPressGrp)
        {
        LowestPressGrp=p->IOP_Self(0);
        pLoInGrp=pNd;
        }
      //INCOMPLETECODE1("FlwNets 4 Leaks");
      if (p->IsIO(1) && p->IOP_Self(1)<LowestPressGrp)
        {
        LowestPressGrp=p->IOP_Self(1);
        pLoInGrp=pNd;
        }
      }

    if (pLoInGrp)
      {
      if (fLonely || fUnDefined)
        pLoInGrp->fNegPress=true;
      else if (LowestPressGrp<LowestPressNet)
        {
        pLoInNet=pLoInGrp;
        LowestPressNet=LowestPressGrp;
        }
      }
    }

  if (pLoInNet && !(fLonely||fUnDefined))
    pLoInNet->fNegPress=true;

  return true;
  };

//==========================================================================
//
//
//
//==========================================================================

void CJoinGFBList::AddFB(FlwBlk * pFB, FlwNode * pNd, flag Forward, flag StartTree, int iSrcPBOrder, int iFBOrder, int iDstPBOrder)
  {
  CJoinGFBPtr JCP;
  JCP.m_pNd         = pNd;
  JCP.m_pJoin       = NULL;
  JCP.m_pFB         = pFB;
  JCP.m_pGFB        = NULL;
  JCP.m_iFxd        = -1;
  JCP.m_fForward    = Forward;
  JCP.m_fStartTree  = StartTree;

  if (iFBOrder>=0)
    {
    pFB->SrcPB().m_iFxdPOrder=iSrcPBOrder;
    pFB->m_iFxdPOrder = iFBOrder;
    pFB->DstPB().m_iFxdPOrder=iDstPBOrder;
    }

  AddTail(JCP);

#if dbgFlwNets
  if (dbgFindFBs())
    {
    dbgpln("  JnGFBList:AddFB       %s %s %i", 
      StartTree ? "@":" ", Forward ?"Fwd":"Rev",pFB->m_iNo);//Forward ? pGFB->FwdTag() : pGFB->RevTag() );
    }
#endif
  };

void CJoinGFBList::AddGFB(CGrpFlwBlk * pGFB, flag Forward, flag StartTree)
  {
  CJoinGFBPtr JCP;
  JCP.m_pNd         = NULL;
  JCP.m_pJoin       = NULL;
  JCP.m_pFB         = NULL;
  JCP.m_pGFB        = pGFB;
  JCP.m_iFxd        = -1;
  JCP.m_fForward    = Forward;
  JCP.m_fStartTree  = StartTree;
  AddTail(JCP);

#if dbgFlwNets
  if (dbgFindFBs())
    {
    dbgpln("  JnGFBList:AddGFB       %s  GFB %s", 
      StartTree ? "@":" ",Forward ? pGFB->FwdTag() : pGFB->RevTag() );
    }
#endif
  };

void CJoinGFBList::AddJoin(CJoinRec * pJ, int iPBOrder)
  {
  CJoinGFBPtr JCP;
  JCP.m_pNd         = pJ->Nd();
  JCP.m_pJoin       = pJ;
  JCP.m_pFB         = NULL;
  JCP.m_pGFB        = NULL;
  JCP.m_iFxd        = -1;
  JCP.m_fForward    = false;
  JCP.m_fStartTree  = false;

  if (iPBOrder>=0)
    pJ->PB()->m_iFxdPOrder = iPBOrder;
    
  AddTail(JCP);

#if dbgFlwNets
  if (dbgFindFBs())
    dbgpln("  JnGFBList:AddFBJn          Jn  %s", pJ->Nd()->Tag());
#endif
  };

//==========================================================================
//
//
//
//==========================================================================

CNetBlk::CNetBlk()
  {
  nJobsToDo=0;
  bFixBusy=false;
  NSubNets=0;
  Clear(-1, NULL);
  }

//--------------------------------------------------------------------------

CNetBlk::~CNetBlk()
  {
  Clear(-1, NULL);

  }

//-------------------------------------------------------------------------

void CNetBlk::Clear(int NetNo_, CFullNetBlk *pFNB_)
  {
  pFNB=pFNB_;
  for (int s=0; s<NSubNets; s++)
    SubNet[s]->Clear(-1, this, pFNB);

  NetNo=NetNo_;
  fSolved=0;
  fTopologyChanged=0;
  dbgSubNetStruct=0;
  NSubNets=0;
  nOverSpecChgs=0;
  }

//-------------------------------------------------------------------------

flag CNetBlk::AddConn2GFB(ACPosition Pos, JoinTypes Typ, CGrpFlwBlkPtr & pGFB, CJoinRec * pSrcJoin, int SrcConnNo, int *FxdOrder)
  {
  CJoinRec * pDstJoin=pSrcJoin->Join_Rmt(SrcConnNo);
  int       DstConnNo=pSrcJoin->ConnNo_Rmt(SrcConnNo);

#if dbgFlwNets
  if (dbgFindConns())
    {
    dbgpln("      NBk:AddConn2GFB      %30s > %s", pSrcJoin->Nd()->Tag(), pDstJoin->Nd()->Tag());
    }
#endif
  int Ret=false;
  if (!pGFB)
    {
    Ret=true;

    pGFB=AllocGFB();
    pGFB->m_LnkInx.Typ=Typ;
    switch (Pos)
      {
      case ACP_Tail :
        pGFB->SetSrc(pSrcJoin, SrcConnNo);
        pGFB->SetDst(pDstJoin, DstConnNo);
        break;
      case ACP_Head :
        pGFB->SetDst(pSrcJoin, SrcConnNo);
        pGFB->SetSrc(pDstJoin, DstConnNo);
        break;
      }
    }
  else
    {
    switch (Pos)
      {
      case ACP_Head : pGFB->SetSrc(pDstJoin, DstConnNo); break;
      case ACP_Tail : pGFB->SetDst(pDstJoin, DstConnNo); break;
      }
    }

  for (int fe=pSrcJoin->NIOFBs(SrcConnNo)-1; fe>=0; fe--)
    {
    pSrcJoin->IOFB(SrcConnNo, fe)->m_pGFb=pGFB;
    pSrcJoin->IOFB(SrcConnNo, fe)->m_iFxdPOrder=FxdOrder ? (*FxdOrder)++ : -1;
    //#if dbgFlwNets
    //if (dbgFindConns())
    //  dbgpln("  NBk:FB Src   %3i FBOrd:%3i PBOrd:%3i", 
    //            pSrcJoin->IOFB(SrcConnNo, fe)->m_iNo, pSrcJoin->IOFB(SrcConnNo, fe)->m_iFxdPOrder, 
    //            pSrcJoin->IOPB(SrcConnNo, fe)->m_iNo, pSrcJoin->IOPB(SrcConnNo, fe)->m_iFxdPOrder);
    //#endif
    }
  for (fe=0; fe<pDstJoin->NIOFBs(DstConnNo); fe++)
    {
    pDstJoin->IOFB(DstConnNo, fe)->m_pGFb=pGFB;
    pDstJoin->IOFB(DstConnNo, fe)->m_iFxdPOrder=FxdOrder ? (*FxdOrder)++ : -1;
    //#if dbgFlwNets
    //if (dbgFindConns())
    //  dbgpln("  NBk:FB Dst   %3i FBOrd:%3i PBOrd:%3i", 
    //            pDstJoin->IOFB(DstConnNo, fe)->m_iNo, pDstJoin->IOFB(DstConnNo, fe)->m_iFxdPOrder, 
    //            pDstJoin->IOPB(DstConnNo, fe)->m_iNo, pDstJoin->IOPB(DstConnNo, fe)->m_iFxdPOrder);
    //#endif
    }

  //#if dbgFlwNets
  //if (dbgFindConns())
  //  dbgpln(" %s > %s", pSrcJoin->Nd()->Tag(), pDstJoin->Nd()->Tag());
  //#endif
  return Ret!=0;
  }

//-------------------------------------------------------------------------

int FreeConns(CJoinRec * p, int &No)
  {
  int nFree=0;
  int iCFree[MaxIOList];
  for (int c = 0; c < p->NConns(); c++)
    if (!ConnHasGFB(p, c))
      iCFree[nFree++] = c;
  No=iCFree[0];
  return nFree;
  }

//-------------------------------------------------------------------------

void CNetBlk::FindFixedConnsFromJoin(flag ForXfer, bool Fwd, CSubNetBlk &SNI, CJoinRec * p, int ConnNo, int &FxdOrder, CGrpFlwBlk *pGFB)
  {
  for (int c = 0; c < p->m_nConns; c++)
    if (c!=ConnNo && p->Connected(c) && !ConnHasGFB(p, c))
      {
      CJoinRec * pRmt     = p->Join_Rmt(c);
      int       iConnRmt = p->ConnNo_Rmt(c);
      FindFixedConns(ForXfer, Fwd, SNI, p, c, pRmt, iConnRmt, pGFB, LFP_Elsewhere, FxdOrder);
      }
  }

void CNetBlk::FindFixedConns(flag ForXfer, bool Fwd, CSubNetBlk &SNI, CJoinRec * pSrc, int SrcConnNo, CJoinRec * pDst, int DstConnNo, CGrpFlwBlk * pGFB, LFxdPos Where, int &FxdOrder)
  {
  if (AddConn2GFB(Fwd?ACP_Tail:ACP_Head, JTyp_FxdLnk, pGFB, pSrc, SrcConnNo, NULL))//&FxdOrder))
    {
    SNI.Grps.AddTail(pGFB);
    SNI.fFixed = true;
    SNI.fXfer  = ForXfer;

    pGFB->m_fFixedExternal=(Where==LFP_Elsewhere);
    //dbgpln("    m_fFixedExternal %#010x %i %s", pGFB, Where, pGFB->m_fFixedExternal?"Fixed:External":"Fixed:Here");

    SNI.m_FxdQList.AddGFB(pGFB, Fwd, Where == LFP_FixedHere);
    }

  if (pSrc->Conn(SrcConnNo).IsLnk())
    {
    FlwBlk *pFB=pSrc->FB(SrcConnNo,0);
    //SNI.m_FxdPList.AddFB(pFB, pSrc->Nd(), SrcConnNo<DstConnNo, Where == LFP_FixedHere);
    if (pSrc->m_iJoinId<pDst->m_iJoinId)
      SNI.m_FxdPList.AddFB(pFB, pSrc->Nd(), true, Where == LFP_FixedHere, FxdOrder, FxdOrder+1, FxdOrder+2);
    else
      SNI.m_FxdPList.AddFB(pFB, pSrc->Nd(), false, Where == LFP_FixedHere, FxdOrder+2, FxdOrder+1, FxdOrder);
    FxdOrder+=3;
    }
  else if (pSrc->FB(SrcConnNo,0)->GetFixed())
    {
    SNI.m_FxdPList.AddFB(pSrc->FB(SrcConnNo,0), pSrc->Nd(), false, Where == LFP_FixedHere, FxdOrder+1, FxdOrder, FxdOrder+1);
    FxdOrder+=2;
    SNI.m_FxdPList.AddFB(pDst->FB(DstConnNo,0), pDst->Nd(), true, false, FxdOrder, FxdOrder+1, FxdOrder+2);
    FxdOrder+=3;
    }
  else 
    {
    SNI.m_FxdPList.AddFB(pSrc->FB(SrcConnNo,0), pSrc->Nd(), false, Where == LFP_FixedHere, FxdOrder, FxdOrder+1, FxdOrder+2);
    FxdOrder+=3;
    SNI.m_FxdPList.AddFB(pDst->FB(DstConnNo,0), pDst->Nd(), true, false, FxdOrder, FxdOrder+1, FxdOrder+2);
    FxdOrder+=3;
    }

  FindConnsDeep++;

  for (int Pass=0; Pass<2; Pass++)
    {
    CJoinRec * p;
    int        ConnNo;
    ACPosition Pos;

    if (Pass==0)
      {
      p       = pSrc;
      ConnNo  = SrcConnNo;
      Pos     = Fwd?ACP_Head:ACP_Tail;
      }
    else
      {
      p       = pDst;
      ConnNo  = DstConnNo;
      Pos     = Fwd?ACP_Tail:ACP_Head;
      }

    if (p->Typ!=JTyp_Unknown)
      continue;

    // Part of a Link ?
    if (p->fIs2IO)
      {

      p->Typ = JTyp_FxdLnk;
      pGFB->AddJoin(p, Pos==ACP_Head);
      SNI.Joins.AddTail(p);

      SNI.m_FxdPList.AddJoin(p, FxdOrder++);

      ////if (Where==LFP_FixedHere)
      //  pGFB->m_fFixedExternal=false;//(Where==LFP_Elsewhere);
      //dbgpln("pGFB->m_fFixedExternal %#010x %i %s", pGFB, Where, pGFB->m_fFixedExternal?"Fixed:External":"Fixed:Here");

      p->pSubNet = &SNI;
      p->iFxdQOrder = ++FxdOrder;
      p->fInFixedNet = true;
      p->fIsFixedExtern=(Where==LFP_Elsewhere);
      p->fIsFixedXfer=p->IsXferLnk();
      p->fInXferNet=ForXfer;
      p->SetQmVAccum(0.0);
      p->SetQmLAccum(0.0);

      FindFixedConnsFromJoin(ForXfer, Pos==ACP_Tail, SNI, p, ConnNo, FxdOrder, pGFB);
      }
    else if (!ForXfer && MassConserveTie(p))
      {
      int FreeConn;
      if (FreeConns(p, FreeConn)==1)
        {
        SNI.Ties.AddTail(p);
        SNI.Joins.AddTail(p);
        SNI.m_FxdPList.AddJoin(p, FxdOrder++);
        SNI.m_FxdQList.AddJoin(p,-1);

        p->Typ             = JTyp_FxdTie;
        p->pSubNet         = &SNI;
        p->iFxdQOrder      = ++FxdOrder;
        p->fInFixedNet     = true;
        p->fIsFixedXfer    = false;
        p->fInXferNet      = ForXfer;
        p->fIsFixedExtern  = true;
        p->SetQmVAccum(0.0);
        p->SetQmLAccum(0.0);

        if (!ConnHasGFB(p, FreeConn))
          {
          CJoinRec * pRmt     = p->Join_Rmt(FreeConn);
          int       iConnRmt = p->ConnNo_Rmt(FreeConn);
          FindFixedConns(ForXfer, true, SNI, p, FreeConn, pRmt, iConnRmt, NULL, LFP_Elsewhere, FxdOrder);
          }
        }
      }
    else if (ForXfer)
      //  else if (0 && ForXfer)
      {
      SNI.Ties.AddTail(p);
      SNI.Joins.AddTail(p);
      SNI.m_FxdPList.AddJoin(p, FxdOrder++);
      SNI.m_FxdQList.AddJoin(p, -1);

      p->Typ             = JTyp_XferTie;//JTyp_FxdTie;
      p->pSubNet         = &SNI;
      p->iFxdQOrder      = ++FxdOrder;
      p->fInFixedNet     = true;
      p->fIsFixedXfer    = false;
      p->fInXferNet      = true;
      p->fIsFixedExtern  = true;
      p->SetQmVAccum(0.0);
      p->SetQmLAccum(0.0);

      FindFixedConnsFromJoin(ForXfer, true, SNI, p, ConnNo, FxdOrder, NULL);
      }
    }

  FindConnsDeep--;
  }

//-------------------------------------------------------------------------

void CNetBlk::FindFxdQs(flag ForXfer, CSubNetBlk &SNI, CJoinRec * pJ, int Conn, int &FxdOrder)
  {
  FindConnsDeep=0;
  //  flag Xfr=ForXfer && ConnIsXfer(pJ, Conn);
  //  flag Fxd=!ForXfer && ConnIsFixed(pJ, Conn);
  flag Xfr=ForXfer && FBIsXfer(pJ, Conn);
  flag Fxd=!ForXfer && FBIsFixed(pJ, Conn);

  if (0)
    dbgpln("FindFxdQs  %s %s %i %s", 
    FBIsXfer(pJ, Conn)?"Xfr":"  ", FBIsFixed(pJ, Conn)?"Fxd":"   ", Conn, pJ->Nd()->FullObjTag());

  if (!ConnHasGFB(pJ, Conn) && (Xfr || Fxd))
    {
#if dbgFlwNets
    if (dbgFindConns())
      dbgpln("%s  : ------- [%s] %s",
      ForXfer ? "Xfer Conns " : "Fixed Conns", pJ->Nd()->FullObjTag(), "");
#endif
    CJoinRec * pNxt=pJ->Join_Rmt(Conn);
    int NxtConn=pJ->ConnNo_Rmt(Conn);
    FindFixedConns(ForXfer, true, SNI, pJ, Conn, pNxt, NxtConn, NULL, LFP_FixedHere, FxdOrder);
    }
  }

//-------------------------------------------------------------------------

flag CNetBlk::FindXferTies(CSubNetBlk &SNI, CJoinRec * & pJ)
  {
#if dbgFlwNets
  if (dbgFindConns())
    {
    dbgpln("Xfer Ties : ------- [%s] %s", pJ->Nd()->FullObjTag(), "");
    }
#endif

  if (pJ->Typ!=JTyp_Unknown)
    return false;

  // Look for Any Non Xfer
  for (int c = 0; c < pJ->m_nConns; c++)
    if (pJ->Connected(c))
      {
      CJoinRec * pRmt = pJ->Join_Rmt(c);
      if (!pRmt || !pRmt->fInXferNet)
        return false;
      }

    SNI.Ties.AddTail(pJ);
    SNI.Joins.AddTail(pJ);
    SNI.fFixed    = true;
    SNI.fXfer     = true;
    SNI.fXferTies = true;

    pJ->Typ             = JTyp_XferTie;
    pJ->pSubNet         = &SNI;
    //pJ->iFxdQOrder      = ++FxdOrder;
    pJ->fInFixedNet     = true;
    pJ->fIsFixedXfer    = false;
    pJ->fInXferNet      = true;
    pJ->fIsFixedExtern  = true;
    pJ->SetQmVAccum(0.0);
    pJ->SetQmLAccum(0.0);

    return true;
  }

//-------------------------------------------------------------------------

void CNetBlk::FindMatrixConns(CSubNetBlk &SNI, CJoinRec * pSrc, CJoinRec * p, int ConnNo, CGrpFlwBlk * pGFB)
  {
  FindConnsDeep++;

  p->pSubNet = &SNI;
  p->fHasQRatio=0;
  SNI.Joins.AddTail(p);
  if (p->pNd->fDoDbgBrk)
    BreakPoint();

  //dbgpln("XXXX %s",p->Nd()->FullObjTag());
  if (p->fIsTie)
    {
    p->Typ = (p->fIsImpNd ? JTyp_ImpNd : p->fIsAccLnk ? JTyp_AccLnk : JTyp_Tie);
    p->pSubNet = &SNI;
    SNI.Ties.AddTail(p);
    pGFB=NULL;
    }
  else if (p->fIs2IO)
    {
    p->Typ = JTyp_Link;
    p->pSubNet = &SNI;
    pGFB->AddJoin(p);
    }

  for (int c = 0; c < p->m_nConns; c++)
    {
    if (p->Connected(c) && !p->GFB(c))
      {
      if (p->fIsTie)
        pGFB=NULL;
      CJoinRec * pR = p->Join_Rmt(c);
      if (AddConn2GFB(ACP_Tail, JTyp_Link, pGFB, p, c, /*NULL,*/ NULL))
        SNI.Grps.AddTail(pGFB);
      if (pR->Typ==JTyp_Unknown)
        FindMatrixConns(SNI, p, pR, p->ConnNo_Rmt(c), pGFB);
      }
    }

  FindConnsDeep--;
  }

//-------------------------------------------------------------------------

flag CNetBlk::FindNetLinks(CSubNetBlk &SNI, CJoinRec * & pJ)
  {
#if dbgFlwNets
  if (dbgFindConns())
    {
    dbgpln("Matrix Conns : ------- [%s] %s", pJ->Nd()->FullObjTag(), "");
    }
#endif

  // Look for FreeConnection
  for (int c = 0; c < pJ->m_nConns; c++)
    if (pJ->Connected(c) && !pJ->GFB(c))
      {
      FindConnsDeep=0;
      flag HasQRatio=0;
      SNI.m_pFirstJoin=pJ;
      ASSERT(SNI.m_pFirstJoin->fIsTie); // First Join in SubNet Must be a Tie
      CGrpFlwBlk * pGFB=NULL;
      FindMatrixConns(SNI, NULL, pJ, -1, pGFB);//, pJ->fIsTie, false, HasQRatio);

      SNI.fIndexCalculationReqd=1;
      SNI.fNetWrk=1;
      return true;
      }

    return false;
  }

//-------------------------------------------------------------------------

void CNetBlk::FindLonelyConns(ACPosition Pos, CSubNetBlk &SNI, CJoinRec * p, int ConnNo, CGrpFlwBlk * pGFB)
  {
  FindConnsDeep++;

  // Part of a Link ?
  CJoinRec*pJ;
  int iC;
  // Go to other side and try
  CJoinRec * pJoinNxt=p->Join_Rmt(ConnNo);
  int iConnNxt=OtherEnd(p->ConnNo_Rmt(ConnNo));
  //dbgpln("___N [%i]%s",iConnNxt,pJoinNxt->pNd->FullObjTag());
  while (pJoinNxt->fIs2IO)
    if (!ConnHasGFB(pJoinNxt, iConnNxt))
      {
      AddConn2GFB(ACP_Tail, JTyp_Lone, pGFB, pJoinNxt, iConnNxt, NULL);
      pGFB->AddJoin(pJoinNxt, false);
      pJoinNxt->Dirn = ((iConnNxt == 1) ? 1 : -1);
      pJoinNxt->pSubNet = &SNI;
      pJ=pJoinNxt->Join_Rmt(iConnNxt);
      iC=OtherEnd(pJoinNxt->ConnNo_Rmt(iConnNxt));
      pJoinNxt=pJ;
      iConnNxt=iC;
      //dbgpln("...N [%i]%s",iConnNxt,pJoinNxt->pNd->FullObjTag());
      }
    else
      break;

  // Go to other way and try
  CJoinRec * pJoinPrv=p;
  int iConnPrv=OtherEnd(ConnNo);
  //dbgpln("___P [%i]%s",iConnPrv,pJoinPrv->pNd->FullObjTag());
  while (pJoinPrv->fIs2IO)
    if (!ConnHasGFB(pJoinPrv, iConnPrv))
      {
      AddConn2GFB(ACP_Head, JTyp_Lone, pGFB, pJoinPrv, iConnPrv, NULL);
      pGFB->AddJoin(pJoinPrv, true);
      pJoinPrv->pSubNet = &SNI;
      pJ=pJoinPrv->Join_Rmt(iConnPrv);
      iC=OtherEnd(pJoinPrv->ConnNo_Rmt(iConnPrv));
      pJoinPrv=pJ;
      iConnPrv=iC;
      //dbgpln("...P [%i]%s",iConnPrv,pJoinPrv->pNd->FullObjTag());
      }
    else
      break;

  FindConnsDeep--;
  }

//-------------------------------------------------------------------------

flag CNetBlk::FindLonelyConns(CSubNetBlk &SNI, CJoinRec * pJ, int Conn)
  {
  FindConnsDeep=0;
  if (!ConnHasGFB(pJ, Conn))
    {
#if dbgFlwNets
    if (dbgFindConns())
      dbgpln("Lonely Conns  : ------- [%s] %s", pJ->Nd()->FullObjTag(), "");
#endif
    // ? Part of a Link;
    CGrpFlwBlk * pGFB=NULL;

    CJoinRec * pLnk= pJ->fIs2IO ? pJ : pJ->Join_Rmt(Conn);
    int iConn=pJ->fIs2IO ? Conn : pJ->ConnNo_Rmt(Conn);

    if (AddConn2GFB(ACP_Tail, JTyp_Lone, pGFB, pLnk,iConn, /*NULL,*/ NULL))//, p->Join_Rmt(Conn), p->ConnNo_Rmt(Conn)))
      {
      SNI.Grps.AddTail(pGFB);
      SNI.fLonely = 1;
      }
    //dbgpln("PLnk %s", pLnk->Nd()->FullObjTag());
    FindLonelyConns(ACP_Tail, SNI, pLnk, iConn, pGFB);
    return true;
    }
  return false;
  }

//-------------------------------------------------------------------------

void CNetBlk::FindUnDefConns(ACPosition Pos, CSubNetBlk &SNI, CJoinRec * p, int ConnNo, CGrpFlwBlk * pGFB)
  {
  FindConnsDeep++;

  // Part of a Link ?
  CJoinRec*pJ;
  int iC;
  // Go to other side and try
  CJoinRec * pJoinNxt=p->Join_Rmt(ConnNo);
  int iConnNxt=OtherEnd(p->ConnNo_Rmt(ConnNo));
  //dbgpln("___N [%i]%s",iConnNxt,pJoinNxt->pNd->FullObjTag());
  while (pJoinNxt->fIs2IO)
    if (!ConnHasGFB(pJoinNxt, iConnNxt))
      {
      AddConn2GFB(ACP_Tail, JTyp_Lone, pGFB, pJoinNxt, iConnNxt, NULL);
      pGFB->AddJoin(pJoinNxt, false);
      pJoinNxt->Dirn = ((iConnNxt == 1) ? 1 : -1);
      pJoinNxt->pSubNet = &SNI;
      pJ=pJoinNxt->Join_Rmt(iConnNxt);
      iC=OtherEnd(pJoinNxt->ConnNo_Rmt(iConnNxt));
      pJoinNxt=pJ;
      iConnNxt=iC;
      //dbgpln("...N [%i]%s",iConnNxt,pJoinNxt->pNd->FullObjTag());
      }
    else
      break;

  // Go to other way and try
  CJoinRec * pJoinPrv=p;
  int iConnPrv=OtherEnd(ConnNo);
  //dbgpln("___P [%i]%s",iConnPrv,pJoinPrv->pNd->FullObjTag());
  while (pJoinPrv->fIs2IO)
    if (!ConnHasGFB(pJoinPrv, iConnPrv))
      {
      AddConn2GFB(ACP_Head, JTyp_Lone, pGFB, pJoinPrv, iConnPrv, NULL);
      pGFB->AddJoin(pJoinPrv, true);
      pJoinPrv->pSubNet = &SNI;
      pJ=pJoinPrv->Join_Rmt(iConnPrv);
      iC=OtherEnd(pJoinPrv->ConnNo_Rmt(iConnPrv));
      pJoinPrv=pJ;
      iConnPrv=iC;
      //dbgpln("...P [%i]%s",iConnPrv,pJoinPrv->pNd->FullObjTag());
      }
    else
      break;

  FindConnsDeep--;
  }

//-------------------------------------------------------------------------

flag CNetBlk::FindUnDefConns(CSubNetBlk &SNI, CJoinRec * pJ, int Conn)
  {
  FindConnsDeep=0;
  if (!ConnHasGFB(pJ, Conn))
    {
#if dbgFlwNets
    if (dbgFindConns())
      dbgpln("UnDefined Conns  : ------- [%s] %s", pJ->Nd()->FullObjTag(), "");
#endif
    // ? Part of a Link;
    CGrpFlwBlk * pGFB=NULL;

    CJoinRec * pLnk= pJ->fIs2IO ? pJ : pJ->Join_Rmt(Conn);
    int iConn=pJ->fIs2IO ? Conn : pJ->ConnNo_Rmt(Conn);

    if (AddConn2GFB(ACP_Tail, JTyp_UnDef, pGFB, pLnk,iConn, /*NULL,*/ NULL))//, p->Join_Rmt(Conn), p->ConnNo_Rmt(Conn)))
      {
      SNI.Grps.AddTail(pGFB);
      SNI.fUnDefined = 1;
      }
    //dbgpln("PLnk %s", pLnk->Nd()->FullObjTag());
    FindUnDefConns(ACP_Tail, SNI, pLnk, iConn, pGFB);
    return true;
    }
  return false;
  }

//-------------------------------------------------------------------------

void CNetBlk::PrepareSubNet(int SubNetNo)
  {
  SubNet.SetSize(Max(SubNetNo+1, SubNet.GetSize()));
  SubNet[SubNetNo]->Clear(SubNetNo, this, pFNB);
  };


//-------------------------------------------------------------------------

CGrpFlwBlk * CNetBlk::AllocGFB()
  {
  CGrpFlwBlk * p = new CGrpFlwBlk;
  p->Clear(this, pFNB);
  return p;
  };

//-------------------------------------------------------------------------

void CNetBlk::FreeGFB(CGrpFlwBlk * p)
  {
  delete p;
  };

//--------------------------------------------------------------------------

void CNetBlk::DumpJoinList()
  {
#if dbgFlwNets
  if (dbgJoinConnects())
    {
    dbgpln("Joins:=================================================");
    CFlwNodeIter N(Nds);
    for (FlwNode * p=N.First(); p; p=N.Next())
      {

      dbgpln("%-28s -----------------------------------", p->FullObjTag());
      for (int j=0; j<p->Joins.GetCount(); j++)
        {
        CJoinRec &J=p->Joins[j];
        for (int i=0; i<J.NConns(); i++)
          {
          if (i==0)
            {
            Strng S;
            if (J.fIs2IO)    S+="Link ";
            if (J.fIsTie)    S+="Tie ";
            if (J.fIsOpen)   S+="Open ";
            if (J.fDamped)   S+="Damped ";
            if (J.fIsImpNd)  S+="ImpNd ";
            if (J.fIsPumpNd) S+="PumpNd ";
            if (J.fIsAccLnk) S+="AccLnk ";
            if (J.fIsMkup)   S+="Makeup ";
            if (J.fIsAudit)  S+="Audit ";
            dbgp("  %2i] %-20s ",j,S());
            }
          else
            dbgp("      %20s ","");
          dbgpln("[%3i] %-6s %-6s %-7s %-8s %-9s %20s[J:%i C:%i]", i,
            ConnHasGFB(&J, i)     ? "HasGFB":"",    
            ConnIsXfer(&J, i)     ? "IsXfer":"",    
            ConnIsFixed(&J, i)    ? "IsFixed":"",    
            //ConnSameSubNet(&J, i) ? "SameSubNet":"",    
            FBIsXfer(&J, i)       ? "FBIsXfer":"",    
            FBIsFixed(&J, i)      ? "FBIsFixed":"",
            J.Nd_Rmt(i)->FullObjTag(),
            J.m_Conns[i].m_iRmtJoin,J.m_Conns[i].m_iRmtConn);

          }
        }
      //      dbgpln(" ");
      }
    dbgpln("=======================================================");
    }
#endif
  }

//-------------------------------------------------------------------------
//#include "OptOff.h"

void CNetBlk::BldSubNets()
  {
  UnBldSubNets();

  SCDTRY
    {
    CFlwNodeIter N(Nds);
    for (FlwNode *p=N.First(); p; p=N.Next())
      {
      // Clear Flow related Conditions
      for (int i=0; i<=p->CICount(); i++)
        {
        switch (i)
          {
          case 2: // Low/Neg Press
          case 4: // Impedance range
            break;
          default:
            p->ClrCI(i);
            break;
          }
        }
      p->BuildJoinList();
      }

    for (p=N.First(); p; p=N.Next())
      p->ConnectJoinList();

    int SeqNo=0;

    for (p=N.First(); p; p=N.Next())
      p->PostConnectJoinList();

    for (p=N.First(); p; p=N.Next())
      for (int j=0; j<p->NoJoins(); j++)
        Joins.AddTail(&p->Joins[j]);

    JoinIter J(Joins);
    for (CJoinRec * pJ = J.First(); (pJ); pJ = J.Next())
      {
      pJ->Prepare();
      if (pJ->fIsOpen)
        pJ->Typ=JTyp_Open;
      }

    DumpJoinList();

    int FxdOrder= 0;
    int SubNetNo = 0;

    PrepareSubNet(SubNetNo);
    // Look for Xfer Lnks
    for (pJ = J.First(); (pJ); pJ = J.Next())
      {
      for (int c=0; c<pJ->NConns(); c++)
        {
        if (!ConnHasGFB(pJ, c) && ConnIsXfer(pJ, c))
          FindFxdQs(true, *SubNet[SubNetNo], pJ, c, FxdOrder);
        }
      }
    if (SubNet[SubNetNo]->m_FxdPList.GetCount()>0)
      PrepareSubNet(++SubNetNo);

    // Look for Xfer Nodes
    for (pJ = J.First(); (pJ); pJ = J.Next())
      {
      if ((pJ->Typ == JTyp_Unknown) && (pJ->fIsTie))
        FindXferTies(*SubNet[SubNetNo], pJ);
      }
    if (SubNet[SubNetNo]->Ties.GetCount()>0)
      PrepareSubNet(++SubNetNo);

    // Look for Fixed Lnks
    // Start at ends
    for (pJ = J.First(); (pJ); pJ = J.Next())
      {
      for (int c=0; c<pJ->NConns(); c++)
        {
        //dbgpln("FindFxdQs %2i %2i %s[%i][%i]", ConnHasGFB(pJ, c), ConnIsFixed(pJ, c), pJ->Nd()->FullObjTag(), pJ->m_iJoinId, c);
        if (!ConnHasGFB(pJ, c) && ConnIsFixed(pJ, c))
          FindFxdQs(false, *SubNet[SubNetNo], pJ, c, FxdOrder);
        }
      }
    if (SubNet[SubNetNo]->m_FxdPList.GetCount()>0)
      PrepareSubNet(++SubNetNo);

    if (1)
      {
      for (pJ = J.First(); (pJ); pJ = J.Next())
        {
        if ((pJ->Typ == JTyp_Unknown) && (pJ->fIsTie))
          {
          if (FindNetLinks(*SubNet[SubNetNo], pJ))
            PrepareSubNet(++SubNetNo);
          }
        }

      flag Found=0;
      for (pJ = J.First(); (pJ); pJ = J.Next())
        {
        for (int c=0; c<pJ->NConns(); c++)
          {
          if (/*(pJ->fIs2IO || pJ->fIsTie) &&*/ !ConnHasGFB(pJ, c))
            {
            if (FindLonelyConns(*SubNet[SubNetNo], pJ, c))
              Found=1;
            }
          }
        }
      if (Found)
        SubNetNo++;
      }
    //else
    //  {
    //  flag Found=0;
    //  for (pJ = J.First(); (pJ); pJ = J.Next())
    //    {
    //    for (int c=0; c<pJ->NConns(); c++)
    //      {
    //      if (!ConnHasGFB(pJ, c))
    //        {
    //        if (FindUnDefConns(*SubNet[SubNetNo], pJ, c))
    //          Found=1;
    //        }
    //      }
    //    }
    //  if (Found)
    //    SubNetNo++;
    //  }

    NSubNets = SubNetNo;

    fTopologyChanged=0;

    if (dbgtestfileopen())
      for (pJ = J.First(); (pJ); pJ = J.Next())
        pJ->sNetInfo="";

    int iJn=0;
    for (int Sub=0; Sub<NSubNets; Sub++)
      {
      flag IsFullFlow=false;
      CSubNetBlk &SNI=*SubNet[Sub];


      SNI.fTopologyChanged=0;

      GrpFlwIter G(SNI.Grps);
      for (CGrpFlwBlk * g=G.First(); g; g=G.Next())
        {
        ////if (Where==LFP_FixedHere)
        //  pGFB->m_fFixedExternal=false;//(Where==LFP_Elsewhere);
        //dbgpln("pGFB->m_fFixedExternal %#010x %i %s", pGFB, Where, pGFB->m_fFixedExternal?"Fixed:External":"Fixed:Here");


#if dbgFlwNets
        if (dbgGFBSummary())
          {
          dbgpln("GFB >> j:%4i i:%4i l:%4i id:%5i %s   %s %s %s %s", 
            g->m_Src.m_pJoin->m_iNo, g->m_Src.m_iIONo, g->m_Src.m_iLnkNo, 
            g->m_Src.m_pNd->IOId_Self(g->m_Src.m_iIONo), 
            g->m_Src.m_pNd->FullObjTag(), 
            g->m_fFixed          ?"Fixed":"",
            g->m_fFixedExternal  ?"FixedExternal":"",
            g->m_fFixedXfer      ?"FixedXfer":"",
            g->m_fInXferNet      ?"InXferNet":"");

          JoinIter L(g->m_Lnks);
          for (CJoinRec * pJ = L.First(); (pJ); pJ = L.Next())
            dbgpln("    .. j:%4i   %4s   %4s    %5s %s", pJ->m_iNo, "", "", "", pJ->pNd->FullObjTag());
          dbgpln("    << j:%4i i:%4i l:%4i id:%5i %s",
            g->m_Dst.m_pJoin->m_iNo, g->m_Dst.m_iIONo, g->m_Dst.m_iLnkNo, 
            g->m_Dst.m_pNd->IOId_Self(g->m_Dst.m_iIONo), 
            g->m_Dst.m_pNd->FullObjTag());
          }
#endif

        g->m_LnkInx.Typ=(g->m_Src.m_pJoin->fIsOpen || g->m_Dst.m_pJoin->fIsOpen ? JTyp_Edge : JTyp_Link);

        g->m_fSrcIsEdge=(g->m_LnkInx.Typ==JTyp_Edge) && (g->m_Src.m_pJoin->fIsOpen || g->m_Src.m_pJoin->fIsImpNd);
        g->m_fDstIsEdge=(g->m_LnkInx.Typ==JTyp_Edge) && (g->m_Dst.m_pJoin->fIsOpen || g->m_Dst.m_pJoin->fIsImpNd);

        g->m_FBs.SetSize(0, 4);

        FlwBlk * pSrcFB=NULL;
        if (g->m_Src.IsIO())
          {
          for (int fe=g->m_Src.NIOFBs()-1; fe>=0; fe--)
            {
            FlwBlk * FB=g->m_Src.IOFB(fe);
            FB->SetDisabled(SNI.fXfer);
            FB->SetInXferNet(SNI.fXfer);
            if (FB->FlowMode()==FBMode_Full)
              IsFullFlow=true;
            //dbgpln("Src[%2i] fb[%4i] %s", fe, FB->m_iNo, g->m_Src.Nd()->Tag());
            ASSERT(!FB->m_bAttached2GFB);
            g->AttachFB(FB, *this, SNI);
            g->m_FBs.Add(FB);
            }
          pSrcFB=g->m_Src.IOFBFlng();
          }
        else
          {
          CLinkRec & L=g->m_Src.Link();
          if (L.m_iJoinId0==g->m_Src.m_pJoin->m_iJoinId)
            {
            FlwBlk * FB=g->m_Src.LinkFB();
            FB->SetDisabled(SNI.fXfer);
            FB->SetInXferNet(SNI.fXfer);
            if (FB->FlowMode()==FBMode_Full)
              IsFullFlow=true;

            //dbgpln("Src %2i fb[%4i] %s", L.m_iSgn, FB->m_iNo, g->m_Src.m_pNd->Tag());

            ASSERT(!FB->m_bAttached2GFB);
            g->AttachFB(FB, *this, SNI);
            g->m_FBs.Add(FB);

            CLinkRec & L=g->m_Src.Link();
            L.m_iSgn=1;

            pSrcFB=g->m_Src.LinkFB();
            }
          }

        JoinIter L(g->m_Lnks);
        for (CJoinRec * pJ = L.First(); (pJ); pJ = L.Next())
          {
          ASSERT(pJ->fIs2IO);
          ASSERT(pJ->NConns()==2);

          pJ->Typ=g->m_LnkInx.Typ;
          pJ->Dirn=(pJ->IOFBFlng_Rmt(0)==pSrcFB ? 1 : -1);
          pJ->pGFb=g;
          pJ->Nd()->pNetTopoChg=&fTopologyChanged;
          pJ->Nd()->pSubNetTopoChg=&SNI.fTopologyChanged;
          pJ->m_Conns[0].m_iQSgn=1;
          pJ->m_Conns[1].m_iQSgn=-1;
          pJ->SetQmVAccum(0.0);
          pJ->SetQmLAccum(0.0);

          int cn=pJ->Dirn>0 ? 0 : 1;
          if (pJ->IsIO(cn))
            {
            for (int fe=0; fe<pJ->NIOFBs(cn); fe++)
              {
              FlwBlk * FB=pJ->IOFB(cn,fe);
              FB->SetInXferNet(SNI.fXfer);
              FB->SetDisabled(SNI.fXfer);
              //dbgpln("JSrc[%i] fb[%4i] %s", fe, FB->m_iNo, pJ->Nd()->Tag());
              ASSERT(!FB->m_bAttached2GFB);
              g->AttachFB(FB, *this, SNI);
              g->m_FBs.Add(FB);
              }
            }
          else
            {
            CLinkRec & L=*pJ->Link(cn);
            if (L.m_iJoinId0==pJ->m_iJoinId)
              {
              FlwBlk * FB=pJ->LinkFB(cn);
              FB->SetInXferNet(SNI.fXfer);
              FB->SetDisabled(SNI.fXfer);
              if (FB->FlowMode()==FBMode_Full)
                IsFullFlow=true;
              //dbgpln("JSrc %2i fb[%4i] %s", L.m_iSgn, FB->m_iNo, pJ->Nd()->Tag());

              ASSERT(!FB->m_bAttached2GFB);
              g->AttachFB(FB, *this, SNI);
              g->m_FBs.Add(FB);

              L.m_iSgn=-1;//(L.m_iJoinConnNo1==pJ->m_iJoinId ? 1 :-1);
              }
            }

          cn=OtherEnd(cn);
          if (pJ->IsIO(cn))
            {
            for (int fe=pJ->NIOFBs(cn)-1; fe>=0; fe--)
              {
              FlwBlk * FB=pJ->IOFB(cn,fe);
              FB->SetInXferNet(SNI.fXfer);
              FB->SetDisabled(SNI.fXfer);
              if (FB->FlowMode()==FBMode_Full)
                IsFullFlow=true;
              //dbgpln("JDst[%2i] fb[%4i] %4i %s", fe, FB->m_iNo, pJ->Nd()->Tag());
              ASSERT(!FB->m_bAttached2GFB);
              g->AttachFB(FB, *this, SNI);
              g->m_FBs.Add(FB);
              }
            }
          else
            {
            CLinkRec & L=*pJ->Link(cn);
            if (L.m_iJoinId0==pJ->m_iJoinId)
              {
              FlwBlk * FB=pJ->LinkFB(cn);
              FB->SetInXferNet(SNI.fXfer);
              FB->SetDisabled(SNI.fXfer);
              if (FB->FlowMode()==FBMode_Full)
                IsFullFlow=true;
              ASSERT(!FB->m_bAttached2GFB);
              g->AttachFB(FB, *this, SNI);
              g->m_FBs.Add(FB);

              CLinkRec & L=*pJ->Link(cn);
              L.m_iSgn=1;
              //dbgpln("JDst %2i fb[%4i] %s", L.m_iSgn, FB->m_iNo, pJ->Nd()->Tag());
              }
            }

          pSrcFB= pJ->IOFBFlng(cn);

          pJ->m_NetNo=NetNo;
          pJ->m_SubNetNo=Sub;

          if (dbgtestfileopen())
            {
            char * What="?";
            if (SNI.fFixed)
              What="Fixed";
            else if (SNI.fXfer)
              What="Xfer";
            else if (SNI.fXferTies)
              What="XferTies";
            else if (SNI.fNetWrk)
              What="Net";
            else if (SNI.fLonely)
              What="Lonely";
            else if (SNI.fUnDefined)
              What="Lonely";
            else if (SNI.fIsolatedNet)
              What="IsoNet";
            else if(SNI.fIsolatedNode)
              What="IsoNode";

            pJ->sNetInfo.Set("Lnk Net(%i)Sub(%i) %s",NetNo,Sub,What);
            }
          }

        if (g->m_Dst.IsIO())
          {
          for (int fe=0; fe<g->m_Dst.NIOFBs(); fe++)
            {
            FlwBlk * FB=g->m_Dst.IOFB(fe);
            FB->SetDisabled(SNI.fXfer);
            FB->SetInXferNet(SNI.fXfer);
            if (FB->FlowMode()==FBMode_Full)
              IsFullFlow=true;
            //dbgpln("Dst[%2i] fb[%4i] %s", fe, FB->m_iNo, g->m_Dst.m_pNd->Tag());
            ASSERT(!FB->m_bAttached2GFB);
            g->AttachFB(FB, *this, SNI);
            g->m_FBs.Add(FB);
            }
          }
        else
          {
          CLinkRec & L=g->m_Dst.Link();
          if (L.m_iJoinId0==g->m_Dst.m_pJoin->m_iJoinId)
            {
            FlwBlk * FB=g->m_Dst.LinkFB();
            FB->SetDisabled(SNI.fXfer);
            FB->SetInXferNet(SNI.fXfer);
            if (FB->FlowMode()==FBMode_Full)
              IsFullFlow=true;
            ASSERT(!FB->m_bAttached2GFB);
            g->AttachFB(FB, *this, SNI);
            g->m_FBs.Add(FB);

            CLinkRec & L=g->m_Dst.Link();
            L.m_iSgn=-1;//(L.m_iJoinConnNo1==g->m_Dst.m_pJoin->m_iJoinId) ? 1:-1;
            //dbgpln("Dst %2i fb[%4i] %s", L.m_iSgn, FB->m_iNo, g->m_Src.m_pNd->Tag());
            }
          }

        g->SetNet(this, SubNet[Sub]);

        if ((g->m_Regulator.What()!=FBReg_Off) &&
          (g->m_Regulator.m_pFB!=NULL) && 
          (g->m_Regulator.m_pFB->FlowMode()==FBMode_Full))
          SNI.m_RegGrps.AddTail(g);

        g->InitFBSequence();
#if dbgFlwNets
        if (dbgTraverseSeq())
          g->DbgDumpPBlks(true, false);
#endif
        }

      SNI.SetFullFlow(IsFullFlow);

      JoinIter T(SNI.Ties);
      for (CJoinRec * pT=T.First(); pT; pT=T.Next())
        {
        pT->pGFb=NULL;
        pT->TieInx.Typ=pT->Typ;
        pT->Nd()->pNetTopoChg=&fTopologyChanged;
        pT->Nd()->pSubNetTopoChg=&SNI.fTopologyChanged;

        for (int c=0; c<pT->NConns(); c++)
          pT->m_Conns[c].m_iQSgn=(pT==pT->GFB(c)->m_Src.m_pJoin ? -1 : 1);

        pT->m_NetNo=NetNo;
        pT->m_SubNetNo=Sub;

        if (dbgtestfileopen())
          {
          char * What="?";
          if (SNI.fFixed)
            What="Fixed";
          else if (SNI.fXfer)
            What="Xfer";
          else if (SNI.fXferTies)
            What="XferTies";
          else if (SNI.fNetWrk)
            What="Net";
          else if (SNI.fLonely)
            What="Lonely";
          else if (SNI.fUnDefined)
            What="Lonely";
          else if (SNI.fIsolatedNet)
            What="IsoNet";
          else if(SNI.fIsolatedNode)
            What="IsoNode";

          //pT->sNetInfo.Set("Tie N(%i)S(%i)J(%i) %s",NetNo,Sub,iJn++,What);
          pT->sNetInfo.Set("Tie Net(%i)Sub(%i) %s",NetNo,Sub,What);
          }
        }

      for (CGrpFlwBlk * g=G.First(); g; g=G.Next())
        g->InitFlowPhases();
      for (CJoinRec * pT=T.First(); pT; pT=T.Next())
        pT->InitFlowPhases();

      //dbgpln("FlwPh Force     %s%s %s", CFlwBlkData::sm_iFlwPh_Force&FPh_Gasses?"V":" ", CFlwBlkData::sm_iFlwPh_Force&FPh_Liquid?"L":" ","------------------------");
      //dbgpln("      Block     %s%s ", CFlwBlkData::sm_iFlwPh_Block&FPh_Gasses?"V":" ", CFlwBlkData::sm_iFlwPh_Block&FPh_Liquid?"L":" ");
      int Iter=0;
      for (bool Busy=true; Busy; )
        {
        Busy=false;
        for (CGrpFlwBlk * g=G.First(); g; g=G.Next())
          Busy |= g->TestSetFlowPhases(SNI.m_bFullFlow);
        for (CJoinRec * pT=T.First(); pT; pT=T.Next())
          Busy |= pT->TestSetFlowPhases(SNI.m_bFullFlow);
        //dbgpln("Done %i ----", !Busy);
        }
      }

#if dbgFlwNets
    if (dbgSubNets())
      {
      for (Sub=0; Sub<NSubNets; Sub++)
        {
        dbgpln("============================= SubNet [%i]", Sub);
        CSubNetBlk &SNI=*SubNet[Sub];
        GrpFlwIter G(SNI.Grps);
        for (CGrpFlwBlk * g=G.First(); g; g=G.Next())
          {
          Strng S0;
          S0.Set(Valid(g->m_QmEstIn) ? "%#*.*g" : "*",QWide,QPrec,g->m_QmEstIn);
          if (g->m_QmEstIn>=0.0)
            {
            dbgp("GFb : %s %s", S0(), g->m_Src.m_pNd->FullObjTag());
            JoinIter L(g->m_Lnks);
            for (CJoinRec * pJ = L.First(); (pJ); pJ = L.Next())
              dbgp(" -> %s", pJ->Nd()->FullObjTag());
            dbgpln(" -> %s [%s]", g->m_Dst.m_pNd->FullObjTag(), g->FwdTag());
            }
          else
            {
            dbgp("GFb : %s %s", S0(), g->m_Dst.m_pNd->FullObjTag());
            JoinIter L(g->m_Lnks);
            for (CJoinRec * pJ = L.Last(); (pJ); pJ = L.Prev())
              dbgp(" -> %s", pJ->Nd()->FullObjTag());
            dbgpln(" -> %s [%s]", g->m_Src.m_pNd->FullObjTag(), g->RevTag());
            }
          }
        Strng S;
        dbgpln("Fixed PList -----------------");
        CJoinGFBIter PC(SNI.m_FxdPList);
        for (CJoinGFBPtr *pc = PC.First(); (pc); pc = PC.Next())
          if (pc->IsJoin())
            dbgpln("Jn  %s %s", ".", pc->Tag(S));
          else  if (pc->IsFB())
            {
            FlwBlk &FB=*pc->FB();
            dbgpln("FB  %s fb[%4i] sp[%4i] dp[%4i] ", pc->IsFixed()?"F":" ", FB.m_iNo, FB.SrcPB(pc->Forward()).m_iNo, FB.DstPB(pc->Forward()).m_iNo);
            }
          else
            ASSERT_ALWAYS(FALSE, "GFB in FxdPList ?");

          dbgpln("Fixed QList -----------------");
          CJoinGFBIter QC(SNI.m_FxdQList);
          for (CJoinGFBPtr *pc = QC.First(); (pc); pc = QC.Next())
            if (pc->IsJoin())
              dbgpln("Jn  %s %s", ".", pc->Tag(S));
            else  if (pc->IsGFB())
              dbgpln("GFB %s %s", " ", pc->Tag(S));
            else  
              ASSERT_ALWAYS(FALSE, "FB in FxdQList ?");

            dbgpln("=============================");
        }
      ShowSubNets(-1, false);
      }
#endif

#if (dbgFlwNets)
    dbgSubNetStruct=dbgSubNetsSolve()!=0;
#else 
    dbgSubNetStruct=0;
#endif
    }
  SCDCATCH("CNetBlk::BldSubNets", NULL);

  };

//-------------------------------------------------------------------------

void CNetBlk::UnBldSubNets()
  {
  JoinIter J(Joins);
  for (CJoinRec * pJ = J.First(); (pJ); pJ = J.Next())
    {
    for (int i = 0; i < pJ->m_nConns; i++)
      for (int fe=0; fe<pJ->NIOFBs(i); fe++)
        {
        FlwBlk *pFb=pJ->IOFB(i, fe);
        pFb->pNetTopoChg=NULL;
        pFb->pSubNetTopoChg=NULL;
        }
    }

  Joins.RemoveAllToFreeList();

  for (int Sub = 0; Sub < NSubNets; Sub++)
    {
#if dbgSSolve
    dbgpln("UnBldSubNets [%i,%i]",NetNo, Sub);
#endif
    CSubNetBlk &SNI = *SubNet[Sub];
    SNI.Clear(-1, this, pFNB);
    }
  }

//#include "OptOn.h"

//-------------------------------------------------------------------------

void CNetBlk::FixNetTopology()
  {
  CStopWatchLap SWLapFixTopoTicks(gs_swFixTopoTicks);
  if (fTopologyChanged)
    {
#if dbgFlwNets
    if (dbgSubNets())
      dbgpln("TopologyChanged [%i] - Rebuild Nets",NetNo);
#endif
    BldSubNets();
    }

  fTopologyChanged = 0;
  for (int Sub = 0; Sub < NSubNets; Sub++)
    SubNet[Sub]->fTopologyChanged = 0;
  }

//-------------------------------------------------------------------------

void CSubNetBlk::SetDBGFlags()
  {
  fHasDbgBrk=false;
  GrpFlwIter G(Grps);
  for (CGrpFlwBlk * g=G.First(); g; g=G.Next())
    {
    g->m_fHasDbgBrk=false;
    JoinIter L(g->m_Lnks);
    for (CJoinRec * pJ = L.First(); (pJ); pJ = L.Next())
      if (pJ->pNd->fDoDbgBrk)
        {
        pJ->fHasDbgBrk=true;
        g->m_fHasDbgBrk=true;
        fHasDbgBrk=true;
        }
    }
#if dbgFlwNets
  if (fHasDbgBrk && dbgTraverseDbgBrkAllInSubNet())
    for (g=G.First(); g; g=G.Next())
      g->m_fHasDbgBrk=true;
#endif
  };
//-------------------------------------------------------------------------

void CNetBlk::SetDBGFlags()
  {
  JoinIter J(Joins);
  for (CJoinRec * pJ = J.First(); (pJ); pJ = J.Next())
    pJ->fHasDbgBrk=false;

  for (int Sub=0; Sub<NSubNets; Sub++)
    SubNet[Sub]->SetDBGFlags();
  };

//-------------------------------------------------------------------------

void CNetBlk::SetQmEstAndQm()//int Soln)
  {
  for (int Sub=0; Sub<NSubNets; Sub++)
    {
    GrpFlwIter G(SubNet[Sub]->Grps);
    for (CGrpFlwBlk * g=G.First(); g; g=G.Next())
      g->SetLinkQmEstAndQm(g->m_QmEstIn);
    }
  }

//-------------------------------------------------------------------------

void CNetBlk::SaveQmEst()
  {
  for (int Sub=0; Sub<NSubNets; Sub++)
    {
    GrpFlwIter G(SubNet[Sub]->Grps);
    for (CGrpFlwBlk * g=G.First(); g; g=G.Next())
      g->m_QmEstInMem=g->m_QmEstIn;
    }
  }

//-------------------------------------------------------------------------

void CNetBlk::RestoreQmEst()
  {
  for (int Sub=0; Sub<NSubNets; Sub++)
    {
    GrpFlwIter G(SubNet[Sub]->Grps);
    for (CGrpFlwBlk * g=G.First(); g; g=G.Next())
      g->m_QmEstIn=g->m_QmEstInMem;
    }
  }

//-------------------------------------------------------------------------

void CNetBlk::SaveQmGood()
  {
  for (int Sub=0; Sub<NSubNets; Sub++)
    {
    GrpFlwIter G(SubNet[Sub]->Grps);
    for (CGrpFlwBlk * g=G.First(); g; g=G.Next())
      g->m_QmGood=g->m_QmIn;
    }
  }

//-------------------------------------------------------------------------

void CNetBlk::SavePGood()
  {
  CFlwNodeIter Nd(Nds);
  for (FlwNode *p = Nd.First(); (p); p = Nd.Next())
    {
    for (int j=0; j<p->Joins.GetSize(); j++)
      p->Set_JoinP_Good(j, p->JoinP(j));
    for (int i=0; i<p->NoFlwIOs(); i++)
      {
      CPressBlkArray & PB=p->IOPBs(i);
      for (int f=0; f<PB.GetSize(); f++)
        PB[f].PGood=PB[f].P;
      //        Set_IOP_Good_Self(i, p->IOP_Self(i));
      //p->Set_IOP_Good_Self(i, p->IOP_Self(i));
      p->Set_IOP_Good_Flng(i, p->IOP_Flng(i));
      }
    //TODO - LinkPB
    }
  }

//-------------------------------------------------------------------------

void CNetBlk::RestorePGood()
  {
  CFlwNodeIter Nd(Nds);
  for (FlwNode *p = Nd.First(); (p); p = Nd.Next())
    {
    for (int j=0; j<p->Joins.GetSize(); j++)
      p->Set_JoinP(j, p->JoinP_Good(j));
    for (int i=0; i<p->NoFlwIOs(); i++)
      {
      p->Set_IOP_Self(i, p->IOP_Good_Self(i));
      p->Set_IOP_Flng(i, p->IOP_Good_Flng(i));
      }
    //TODO - LinkPB
    }
  }

//-------------------------------------------------------------------------

void CNetBlk::Solve()
  {
  SCDTRY
    {
    fSolved=0;

    for (int Pass=0; Pass<MaxNetSolvePasses; Pass++)
      {
#if dbgFlwNets
      if (dbgWhere())
        dbgpln("NetSolvePass:[%i]=%i ===================================================================",
        NetNo, Pass);
#endif
      // Fix network topology if neccesary
      FixNetTopology();

      SetDBGFlags();

      fSolved = 1;
      JoinIter J(Joins);
      for (CJoinRec * pJ=J.First(); pJ ; pJ=J.Next())
        {
        SolveDbgFlags(pJ->fHasDbgBrk);
        if (pJ->fIsOpen)
          {
          for (int c=0; c<pJ->NConns(); c++)
            {
#if dbgFlwNets
            if (DbgFlags & DBG_SetEstP)
              dbgpln("SetEstP %#*.*f %s[%i]", PWide, PPrec, pJ->IOP_Self(c), pJ->pNd->FullObjTag(), c);
#endif
            pJ->Set_IOP_Est_Self(c, pJ->IOP_Self(c));
            }
          }
        pJ->dSigmaQmV=0.0;
        pJ->dSigmaQmL=0.0;
        }

      // Try to Solve All SubNets
      for (int Sub = 0; (Sub < NSubNets); Sub++)
        if (!SubNet[Sub]->Solve()) // force all Subnets to be called
          fSolved = 0;

      // Update all Fixed flow link Pressures Based on Current Best Estimates
      for (Sub = 0; (Sub < NSubNets); Sub++)
        if (SubNet[Sub]->fFixed)
          SubNet[Sub]->SolveFxdPs();

      for (Sub = 0; (Sub < NSubNets); Sub++)
        {
        SubNet[Sub]->InitUDFs();
        if (SubNet[Sub]->TestUDFsChange())
          {
          fSolved=0;
          fTopologyChanged=1;
          }
        }

      dbgSubNetStruct=0;

      if (NetTopologyOK() && (nOverSpecChgs>0) && (Pass<MaxNetSolvePasses-1))
        fTopologyChanged=1;

      // if no Changes to the overall network topology then we are done
      if (NetTopologyOK())
        {
        nOverSpecChgs=0;
        break;
        }

      nOverSpecChgs=0;
      }

    for (int Sub = 0; (Sub < NSubNets); Sub++)
      {
      if (!SubNet[Sub]->fSolved)
        {
        CSubNetBlk &SNI=*SubNet[Sub];
        LPTSTR What;
        if (SNI.fFixed)
          What="Fixed Group";
        else if (SNI.fXfer)
          What="Transfer Link Group";
        else if (SNI.fXferTies)
          What="Transfer Ties Group";
        else if (SNI.fNetWrk)
          What="Network";
        else if (SNI.fLonely)
          What="Isolated Link Group";
        else if (SNI.fUnDefined)
          What="Indefined Link Group";
        else if (SNI.fIsolatedNet)
          What="Isolated Network";
        else if(SNI.fIsolatedNode)
          What="Isolated Node";
        else
          What="??";
        Strng S, D(What);
        D += " not converged ";
        if (SNI.Ties.GetCount()>0)
          {
          D+=SNI.Ties.GetHead()->Nd()->FullObjTag();
          if (SNI.Ties.GetCount()>1)
            D+="...";
          }
        else if (SNI.Grps.GetCount()>0)
          {
          D+=SNI.Grps.GetHead()->FwdTag();
          if (SNI.Grps.GetCount()>1)
            D+="...";
          }
        S.Set("Net[%i.%i]", SNI.pNet->NetNo, SNI.m_SubNetNo);
        LogWarning(S(), 0, "%s", D());

        GrpFlwIter G(SNI.Grps);
        for (CGrpFlwBlk * g=G.First(); g; g=G.Next())
          {
          if (!g->m_fGFTolsOK)
            LogNote(g->FwdTag(), 0, "Not converged PErr=%.4f kPa", g->m_P_Err);
          }
        }
      }
    if (Pass>=MaxNetSolvePasses)
      LogError("Solver", 0, "Too Many Solve Passes");

    }
  SCDCATCH("CNetBlk::Solve", NULL);
    //  return fSolved;
  }

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

void CNetBlk::SolveStart()
  {
  fSolved=0;

  iSolvePass=0;

  pFNB->JobListIter.AddJob(Job_FixNet, NetNo, 0);
  }

//-------------------------------------------------------------------------

void CNetBlk::JobFixNet()
  {
  // Fix network topology if neccesary
  if (bFixBusy)
    DoBreak();
  bFixBusy=true;
  FixNetTopology();

  bFixBusy=false;

  SetDBGFlags();

  fSolved = 1;
  JoinIter J(Joins);
  for (CJoinRec * pJ=J.First(); pJ ; pJ=J.Next())
    {
    pJ->dSigmaQmV=0.0;
    pJ->dSigmaQmL=0.0;
    }

  nJobsToDo=0;
  for (int Sub = 0; (Sub < NSubNets); Sub++)
    if (SubNet[Sub]->fFixed)
      nJobsToDo++;
  for (Sub = 0; (Sub < NSubNets); Sub++)
    if (SubNet[Sub]->fFixed)
      JobListIter().AddJob(Job_SolveFlwsFxd, NetNo, Sub);

  if (nJobsToDo==0)
    JobListIter().AddJob(Job_SolveFlwsFxdDone, NetNo, 0);
  }

//-------------------------------------------------------------------------

void CNetBlk::JobSolveFlwsFxd(int Sub)
  {
  // Try to Solve All SubNets
  if (!SubNet[Sub]->Solve()) // force all Subnets to be called
    fSolved = 0;

  if (InterlockedDecrement(&nJobsToDo)==0)
    JobListIter().AddJob(Job_SolveFlwsFxdDone, NetNo, 0);
  }

//-------------------------------------------------------------------------

void CNetBlk::JobSolveFlwsFxdDone()
  {
  nJobsToDo=0;
  for (int Sub = 0; (Sub < NSubNets); Sub++)
    if (!SubNet[Sub]->fFixed)
      nJobsToDo++;
  for (Sub = 0; (Sub < NSubNets); Sub++)
    if (!SubNet[Sub]->fFixed)
      JobListIter().AddJob(Job_SolveFlws, NetNo, Sub);
  if (nJobsToDo==0)
    JobListIter().AddJob(Job_SolveFlwsDone, NetNo, 0);
  }

//-------------------------------------------------------------------------

void CNetBlk::JobSolveFlws(int Sub)
  {
  // Try to Solve All SubNets
  if (!SubNet[Sub]->Solve()) // force all Subnets to be called
    fSolved = 0;

  if (InterlockedDecrement(&nJobsToDo)==0)
    JobListIter().AddJob(Job_SolveFlwsDone, NetNo, 0);
  }

//-------------------------------------------------------------------------

void CNetBlk::JobSolveFlwsDone()
  {
  nJobsToDo=NSubNets;
  for (int Sub = 0; (Sub < NSubNets); Sub++)
    JobListIter().AddJob(Job_SolvePs, NetNo, Sub);
  }

//-------------------------------------------------------------------------

void CNetBlk::JobSolvePs(int Sub)
  {
  // Update all Fixed flow link Pressures Based on Current Best Estimates
  if (SubNet[Sub]->fFixed)
    SubNet[Sub]->SolveFxdPs();

  if (InterlockedDecrement(&nJobsToDo)==0)
    JobListIter().AddJob(Job_CheckDone, NetNo, 0);
  };

//-------------------------------------------------------------------------

void CNetBlk::JobCheckDone()
  {
  dbgSubNetStruct=0;

  if (NetTopologyOK() && (nOverSpecChgs>0) && (iSolvePass<MaxNetSolvePasses-1))
    fTopologyChanged=1;

  // if no Changes to the overall network topology then we are done
  if (NetTopologyOK())
    {
    nOverSpecChgs=0;
    return;
    }
  nOverSpecChgs=0;
  iSolvePass++;

  if (iSolvePass>=MaxNetSolvePasses)
    {
    LogError("Solver", 0, "Too Many Solve Passes");
    //    JobList().AddDone();
    }
  else
    {
    JobListIter().AddJob(Job_FixNet, NetNo, 0);
    }
  }

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

flag CNetBlk::CheckForErrors()
  {
  CStopWatchLap SWLapOverHeadTicks(gs_swOverHeadTicks);

  int OK= true;
  for (int Sub = 0; Sub < NSubNets; Sub++)
    if (!SubNet[Sub]->CheckForErrors())
      OK=false;

  return OK!=0;
  }

// =========================================================================
//
//
//
//==========================================================================

//double CFullNetBlk::NetMaxResRatio=0.0;

CFullNetBlk::CFullNetBlk(CFlwNodeList& AllNdsIn) : \
AllNds(AllNdsIn),
m_LnkEPS(TBF_DynSys, "FlwNets:LnkEPS", 1.0e-6, 1.0e-8),
m_QmFndEPS(TBF_DynSys, &m_LnkEPS, "FlwNets:QmFndEPS", 1.0, 0.1),
m_VVolFndEPS(TBF_DynSys, "FlwNets:VVolFndEPS", 0, 0.001),  
m_RegSolveEPS(TBF_DynSys, "FlwNets:RegSolveEPS", 0, 1e-8)  
  {
  MaxSolveCnt = 2500;
  TotalSolveCnt = 0;

  m_dNetMaxResRatio=NetMaxResRatioInit;
  m_NetDeRating=10;
  m_lNetCnvCnt=20;
  m_lMaxUDFIters=30;
  m_fAllowFastConverge=false;
  m_lResRunAvgStart=5; 
  m_lResRunAvgCount=5;

  //fStopReqd=0;

  NNets=0;
  };

//-------------------------------------------------------------------------

CFullNetBlk::~CFullNetBlk()
  {
  ClearSolutionBlks();
  Nets.SetSize(0);
  };

//-------------------------------------------------------------------------

flag CFullNetBlk::Initialise(/*double Lnk_Eps_A_, double Lnk_Eps_R_, double Net_DeRating_*/)
  {
  //fStopReqd=0;

  MakeNetNodeList();

  //Lnk_Eps_A = Lnk_Eps_A_;
  //Lnk_Eps_R = Lnk_Eps_R_;
  m_NetDeRating=Max(1.0, m_NetDeRating);

  int LastNet = -1;
  CFlwNodeIter It(Nds);
  for (FlwNode *p = It.First(); (p); p = It.Next())
    LastNet = Max(LastNet, p->NetNo);
  Nets.SetSize(Max(Nets.GetSize(), LastNet+1));
  NNets=LastNet+1;
  for (int i=0; i<NNets; i++)
    Nets[i].Clear(i, this);

  MaxSolveCnt = 2500;
  TotalSolveCnt = 0;

  if (!BuildProblem())
    NNets=0;

  return (Nds.GetCount()==0) || ProblemOK();
  };

//--------------------------------------------------------------------------

void CFullNetBlk::CreateSolutionBlks(int NRqdThreads)
  {
  };

//--------------------------------------------------------------------------

void CFullNetBlk::ClearSolutionBlks()
  {
  for (int Net = 0; Net < NNets; Net++)
    Nets[Net].UnBldSubNets();
  NNets=0;
  Nds.RemoveAllToFreeList();
  };

//--------------------------------------------------------------------------

void CFullNetBlk::FollowIOs(FlwNode *p, int NetIn, FlwNode *&Start, FlwNode *&End)
  {
  if (!p->Done && !p->Busy)
    {
    p->Done = 1;
    p->Busy = 1;
    if ((p->m_dwType & (nc_Process|nc_MSrcSnk|nc_MXfer|nc_MLnk|nc_MBst))!=0)
      {
      p->Done = 1;

      if (!Start)
        Start = p;
      End = p;
      p->NetNo = NetIn;

      Nds.AddTail(p);
      for (int i = 0; i < p->NoFlwIOs(); i++)
        FollowIOs(p->Nd_Rmt(i), NetIn, Start, End);
      }
    p->Busy = 0;
    }
  };

//--------------------------------------------------------------------------

flag CFullNetBlk::MakeNetNodeList()
  {
  int   Net;
  FlwNode *p, *Head, *Start, *End, *LastEnd;

  Net = 0;
  Head  = NULL;
  Start = NULL;
  End   = NULL;
  CFlwNodeIter All(AllNds);
  for (p=All.First(); (p); p=All.Next())
    p->Busy=p->Done=p->Break=0;

  for (p=All.First(); (p); p=All.Next())
    {
    LastEnd = End;
    FollowIOs(p, Net, Start, End);
    if (LastEnd != End)
      {
      Net++;
      if (!Head)
        Head = Start;
      }
    };

  return Nds.GetCount()>0;
  };

//-------------------------------------------------------------------------

flag CFullNetBlk::BuildProblem()
  {
  int        Net;
  FlwNode     *p;

  Net = -1;
  for (Net=0; Net<NNets; Net++)
    {
    Nets[Net].Nds.RemoveAllToFreeList();
    Nets[Net].Joins.RemoveAllToFreeList();
    Nets[Net].fSolved = 0;
    }

  CFlwNodeIter N(Nds);
  for (p = N.First(); (p); p = N.Next())
    {
    if (p->NetNo>=0)
      Nets[p->NetNo].Nds.AddTail(p);
    }

  for (Net = 0; Net < NNets; Net++)
    Nets[Net].BldSubNets();

  return 1;
  }

//-------------------------------------------------------------------------

void CFullNetBlk::SetQmEstAndQm()//int Soln)
  {
  for (int Net=0; Net<NNets; Net++)
    Nets[Net].SetQmEstAndQm();//Soln);
  }

//-------------------------------------------------------------------------

void CFullNetBlk::SaveQmEst()//int Soln)
  {
  for (int Net=0; Net<NNets; Net++)
    Nets[Net].SaveQmEst();//Soln);
  }

//-------------------------------------------------------------------------

void CFullNetBlk::RestoreQmEst()//int Soln)
  {
  for (int Net=0; Net<NNets; Net++)
    Nets[Net].RestoreQmEst();//Soln);
  }

//-------------------------------------------------------------------------

void CFullNetBlk::SaveQmGood()//int Soln)
  {
  for (int Net=0; Net<NNets; Net++)
    Nets[Net].SaveQmGood();//Soln);
  }

//-------------------------------------------------------------------------

void CFullNetBlk::SavePGood()
  {
  for (int Net=0; Net<NNets; Net++)
    Nets[Net].SavePGood();
  }

//-------------------------------------------------------------------------

void CFullNetBlk::RestorePGood()
  {
  for (int Net=0; Net<NNets; Net++)
    Nets[Net].RestorePGood();
  }

//-------------------------------------------------------------------------

void CFullNetBlk::Solve(dword IterNo)
  {
  //ClearSWS();

  int Net, TimeIter = 0;
#ifdef _DEBUG
  dwIterNo=IterNo;
#endif

#if dbgFlwNets
  if (dbgNetSolve())
    {
    dbgpln("=============================================");
    dbgpln("No Of Nets:[%3i]", NNets);
    }
  if (dbgWhere())
    dbgpln("EvalJoinPressures");
#endif

  TimeIter++;

  if (FSTC.NThreads())
    {
    JobListIter.Initialise(&xJobList, FSTC.NThreads());
    for (Net = 0; Net < NNets; Net++)
      Nets[Net].SolveStart();
    FSTC.DoJob(FST_FlwSolve, this, NULL);//, &SlvJobList);
    }
  else
    {
    for (Net = 0; (Net < NNets); Net++)
      Nets[Net].Solve();
    }

#if dbgFlwNets
  if (dbgNetSolve())
    dbgpln("====================================");
#endif

  //  return 0;
  };

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

char* Job_CmdStr[Job_CmdLast+1] =
  {
  "FixNet",
    "SolveFlwsFxd",
    "SolveFlwsFxdDone",
    "SolveFlws",
    "SolvePs",
    "CheckDone",
    //  "Done",
    "CmdLast"
  };


void CFullNetBlk::FSTFlwSolve()
  {
  CJobItem Job;
#if (dbgFlwSolveTiming)
  CStopWatch SWIdle;
  CStopWatch SWExec;
  SWIdle.Start();
#endif
  while (JobListIter.RemoveNext(Job))
    {
#if dbgFlwNets
    if (dbgThreadsJob())
      dbgpln("  Job %4i %-20.20s  %3i %3i Start \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\",
      GetCurrentThreadId(), Job_CmdStr[Job.iWhat], Job.iNetNo, Job.iSubNetNo);
    if (dbgThreadsSleep())
      Sleep(0);
#endif

    ASSERT(_CrtCheckMemory());

#if (dbgFlwSolveTiming)
    SWIdle.Stop();
    SWExec.Clear();
    SWExec.Start();
#endif

    switch (Job.iWhat)
      {
      case Job_FixNet:
        Nets[Job.iNetNo].JobFixNet();
        break;
      case Job_SolveFlwsFxd:
        Nets[Job.iNetNo].JobSolveFlwsFxd(Job.iSubNetNo);
        break;
      case Job_SolveFlwsFxdDone:
        Nets[Job.iNetNo].JobSolveFlwsFxdDone();
        break;
      case Job_SolveFlws:
        Nets[Job.iNetNo].JobSolveFlws(Job.iSubNetNo);
        break;
      case Job_SolveFlwsDone:
        Nets[Job.iNetNo].JobSolveFlwsDone();
        break;
      case Job_SolvePs:
        Nets[Job.iNetNo].JobSolvePs(Job.iSubNetNo);
        break;
      case Job_CheckDone:
        Nets[Job.iNetNo].JobCheckDone();
        break;
      }
#if dbgFlwSolveTiming 
    if (dbgThreadsJobTiming())                                           
      {                                                                 
      SWExec.Stop();                                                  
      dbgpln("Job[%4i]%-20.20s Time:%10.3fms Idle:%10.3fms Exec:%10.3fms %5.1f%% ------", 
        GetCurrentThreadId(), Job_CmdStr[Job.iWhat],                    
        1e3*gs_ExecIterTime.Secs(),
        1e3*SWIdle.Secs(),                                              
        1e3*SWExec.Secs(),                                              
        100*SWExec.Secs()/GTZ(SWExec.Secs()+SWIdle.Secs()));            
      SWIdle.Clear();                                                 
      SWIdle.Start();                                                 
      }
#endif

#if dbgFlwNets
    if (dbgThreadsJob())
      dbgpln("  Job %4i %-20.20s  %3i %3i Done  ////////////////////////////////////////////////////",
      GetCurrentThreadId(), Job_CmdStr[Job.iWhat], Job.iNetNo, Job.iSubNetNo);
    if (dbgThreadsSleep())
      Sleep(0);
#endif
    }
  }

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

int CFullNetBlk::CheckForErrors()
  {
  CFlwNodeIter N(Nds);
  for (FlwNode *pNd = N.First(); (pNd); pNd = N.Next())
    {
    flag OvrSpecd=false;
    for (int i=0; i<pNd->NoFlwIOs(); i++)
      for (int fe=0; fe<pNd->NIOFBs(i); fe++)
        OvrSpecd = OvrSpecd || pNd->IOFB(i, fe)->m_fOvrSpecd;
    pNd->SetCI(10, OvrSpecd);
    pNd->fNegPress=false;
    }

  int OK=true;
  for (int Net = 0; (Net < NNets); Net++)
    {
    if (!Nets[Net].CheckForErrors())
      OK=false;
    }

  for (pNd = N.First(); (pNd); pNd = N.Next())
    {
    flag CI3Clr=true;
    for (int j=0; j<pNd->Joins.GetSize(); j++)
      {
      CJoinRec & J=pNd->Joins[j];
      if (!J.fInXferNet && !J.fIsOpen)
        {
        double SigFlw=J.GetQmSink()-J.GetQmAccum();
        //      double SigFlw=-J.GetQmAccum();
        double ASigFlw=fabs(SigFlw);
        double SigFlwI=SigFlw>0.0 ? SigFlw : 0;
        double SigFlwO=SigFlw<0.0 ? -SigFlw : 0;
        for (int i=0 ; i<J.m_nConns; i++)
          {
          double Flw=0.0;
          if (J.IsIO(i))
            Flw=pNd->IOQmEst_In(J.IONo(i));
          else
            Flw=J.LinkFB(i)->m_QmEst*(J.Link(i)->m_iJoinId1==j?1:-1);

          SigFlw+=Flw;
          ASigFlw+=fabs(Flw);
          if (Flw>0.0)
            SigFlwI+=Flw;
          else
            SigFlwO-=Flw;
          }
        if (J.pQmAux)
          {
          double QmA=J.pQmAux->dQm;
          SigFlw+=QmA;
          ASigFlw+=fabs(QmA);
          if (QmA>0.0)
            SigFlwI+=QmA;
          else
            SigFlwO-=QmA;
          }
        if (fabs(SigFlw)/(GTZ(ASigFlw)+1.0e-6)> 5.0e-3)
          {
          CI3Clr=false;
          pNd->SetCI(3, "W\tEst Flows Not Converged %8.4f %% In:%g Out:%g kg/s",100.0*SigFlw/NZ(ASigFlw),SigFlwI, SigFlwO);
          dbgpln("=================================");
          dbgpln("Est Flows Not Converged %8.4f %% %s",100.0*SigFlw/NZ(ASigFlw), J.Nd()->FullObjTag());
          dbgpln("In: %14.6g %14s", SigFlwI, "");
          dbgpln("Out:%14.6g %14s", SigFlwO, "");
          dbgpln("---------------------------------");
          for (int i=0 ; i<J.m_nConns; i++)     
            {
            if (J.IsIO(i))
              dbgpln("    %14.6g %14.6g : %s",pNd->IOQmEst_In(J.IONo(i)), pNd->IOQm_In(J.IONo(i)), pNd->Nd_Rmt(J.IONo(i))->FullObjTag());
            }
          if (J.pQmAux)
            dbgpln("    %14.6g %14s : %s", J.pQmAux->dQm, "", "Aux...");
          dbgpln("---------------------------------");
          }
        }
      }
    pNd->SetCI(2, pNd->fNegPress);
    if (CI3Clr)
      pNd->ClrCI(3);
    }

  return OK;
  }

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

long CFullNetBlk::GetNumNets()
  {
  return NNets;
  }

//-------------------------------------------------------------------------

long CFullNetBlk::GetNumSubNets(long iNet)
  {
  long i=(iNet>=0) ? iNet : 0;
  long n=(iNet>=0) ? iNet+1 : NNets;
  long N=0;
  for ( ; (i<n); i++)
    N+=Nets[i].NSubNets;
  return N;
  }

//-------------------------------------------------------------------------

long CFullNetBlk::GetNumNodes(long iNet)
  {
  long i=(iNet>=0) ? iNet : 0;
  long n=(iNet>=0) ? iNet+1 : NNets;
  long N=0;
  for ( ; (i<n); i++)
    {
    CNetBlk &NB=Nets[i];
    for (long j=0 ; (j<NB.NSubNets); j++)
      N+=NB.SubNet[j]->Ties.GetCount();
    }
  return N;
  }

//-------------------------------------------------------------------------

long CFullNetBlk::GetNumFxdLnks(long iNet)
  {
  long i=(iNet>=0) ? iNet : 0;
  long n=(iNet>=0) ? iNet+1 : NNets;
  long N=0;
  for ( ; (i<n); i++)
    {
    CNetBlk &NB=Nets[i];
    for (long j=0 ; (j<NB.NSubNets); j++)
      if (NB.SubNet[j]->fFixed)
        N+=NB.SubNet[j]->Grps.GetCount();
    }
  return N;
  }

//-------------------------------------------------------------------------

long CFullNetBlk::GetNumLoneLnks(long iNet)
  {
  long i=(iNet>=0) ? iNet : 0;
  long n=(iNet>=0) ? iNet+1 : NNets;
  long N=0;
  for ( ; (i<n); i++)
    {
    CNetBlk &NB=Nets[i];
    for (long j=0 ; (j<NB.NSubNets); j++)
      if (NB.SubNet[j]->fLonely)
        N+=NB.SubNet[j]->Grps.GetCount();
    }
  return N;
  }

//-------------------------------------------------------------------------

long CFullNetBlk::GetNumNetLnks(long iNet)
  {
  long i=(iNet>=0) ? iNet : 0;
  long n=(iNet>=0) ? iNet+1 : NNets;
  long N=0;
  for ( ; (i<n); i++)
    {
    CNetBlk &NB=Nets[i];
    for (long j=0 ; (j<NB.NSubNets); j++)
      if (NB.SubNet[j]->fNetWrk)
        N+=NB.SubNet[j]->Grps.GetCount();
    }
  return N;
  }

//-------------------------------------------------------------------------

//void CFullNetBlk::ClearSWS()
//  {
//  FixTopoTicks.Clear();
//  OverHeadTicks.Clear();
//  for (long i=0; (i<NNets); i++)
//    {
//    CNetBlk &NB=Nets[i];
//    for (long j=0 ; (j<NB.NSubNets); j++)
//      {
//      CSubNetBlk &SNB=*NB.SubNet[j];
//      SNB.m_swConvergeTicks.Clear();
//      SNB.MeasureTicks.Clear();
//      SNB.SolveTicks.Clear();
//      SNB.LoadSurgeTicks.Clear();
//      SNB.CalcIndicesTicks.Clear();
//      SNB.MtxBuildTicks.Clear();
//      SNB.MtxSolveTicks.Clear();
//      SNB.MtxExtractTicks.Clear();
//      SNB.ChkJoinsTicks.Clear();
//      SNB.EvalJoinQTicks.Clear();
//      SNB.EvalJoinPTicks.Clear();
//      SNB.SolveFxdFlwQTicks.Clear();
//      SNB.SolveFxdFlwPTicks.Clear();
//      }
//    }
//  }

//-------------------------------------------------------------------------

//double CFullNetBlk::GetConvergeSecs()
//  {
//  double N=0;
//  for (long i=0; (i<NNets); i++)
//    {
//    CNetBlk &NB=Nets[i];
//    for (long j=0 ; (j<NB.NSubNets); j++)
//      N+=NB.SubNet[j]->ConvergeTicks.Secs();
//    }
//  return N;
//  }
//
////-------------------------------------------------------------------------
//
//double CFullNetBlk::GetMeasureSecs()
//  {
//  double N=0;
//  for (long i=0; (i<NNets); i++)
//    {
//    CNetBlk &NB=Nets[i];
//    for (long j=0 ; (j<NB.NSubNets); j++)
//      N+=NB.SubNet[j]->MeasureTicks.Secs();
//    }
//  return N;
//  }
//
////-------------------------------------------------------------------------
//
//double CFullNetBlk::GetSolveSecs()
//  {
//  double N=0;
//  for (long i=0; (i<NNets); i++)
//    {
//    CNetBlk &NB=Nets[i];
//    for (long j=0 ; (j<NB.NSubNets); j++)
//      N+=NB.SubNet[j]->SolveTicks.Secs();
//    }
//  return N;
//  }
//
////-------------------------------------------------------------------------
//
//double CFullNetBlk::GetLoadSurgeSecs()
//  {
//  double N=0;
//  for (long i=0; (i<NNets); i++)
//    {
//    CNetBlk &NB=Nets[i];
//    for (long j=0 ; (j<NB.NSubNets); j++)
//      N+=NB.SubNet[j]->LoadSurgeTicks.Secs();
//    }
//  return N;
//  }
//
////-------------------------------------------------------------------------
//
//double CFullNetBlk::GetCalcIndicesSecs()
//  {
//  double N=0;
//  for (long i=0; (i<NNets); i++)
//    {
//    CNetBlk &NB=Nets[i];
//    for (long j=0 ; (j<NB.NSubNets); j++)
//      N+=NB.SubNet[j]->CalcIndicesTicks.Secs();
//    }
//  return N;
//  }
//
////-------------------------------------------------------------------------
//
//double CFullNetBlk::GetMtxBuildSecs()
//  {
//  double N=0;
//  for (long i=0; (i<NNets); i++)
//    {
//    CNetBlk &NB=Nets[i];
//    for (long j=0 ; (j<NB.NSubNets); j++)
//      N+=NB.SubNet[j]->MtxBuildTicks.Secs();
//    }
//  return N;
//  }
//
////-------------------------------------------------------------------------
//
//double CFullNetBlk::GetMtxSolveSecs()
//  {
//  double N=0;
//  for (long i=0; (i<NNets); i++)
//    {
//    CNetBlk &NB=Nets[i];
//    for (long j=0 ; (j<NB.NSubNets); j++)
//      N+=NB.SubNet[j]->MtxSolveTicks.Secs();
//    }
//  return N;
//  }
//
////-------------------------------------------------------------------------
//
//double CFullNetBlk::GetMtxExtractSecs()
//  {
//  double N=0;
//  for (long i=0; (i<NNets); i++)
//    {
//    CNetBlk &NB=Nets[i];
//    for (long j=0 ; (j<NB.NSubNets); j++)
//      N+=NB.SubNet[j]->MtxExtractTicks.Secs();
//    }
//  return N;
//  }
//
////-------------------------------------------------------------------------
//
//double CFullNetBlk::GetChkJoinsSecs()
//  {
//  double N=0;
//  for (long i=0; (i<NNets); i++)
//    {
//    CNetBlk &NB=Nets[i];
//    for (long j=0 ; (j<NB.NSubNets); j++)
//      N+=NB.SubNet[j]->ChkJoinsTicks.Secs();
//    }
//  return N;
//  }
//
////-------------------------------------------------------------------------
//
//double CFullNetBlk::GetEvalJoinQSecs()
//  {
//  double N=0;
//  for (long i=0; (i<NNets); i++)
//    {
//    CNetBlk &NB=Nets[i];
//    for (long j=0 ; (j<NB.NSubNets); j++)
//      N+=NB.SubNet[j]->EvalJoinQTicks.Secs();
//    }
//  return N;
//  }
//
////-------------------------------------------------------------------------
//
//double CFullNetBlk::GetEvalJoinPSecs()
//  {
//  double N=0;
//  for (long i=0; (i<NNets); i++)
//    {
//    CNetBlk &NB=Nets[i];
//    for (long j=0 ; (j<NB.NSubNets); j++)
//      N+=NB.SubNet[j]->EvalJoinPTicks.Secs();
//    }
//  return N;
//  }
//
////-------------------------------------------------------------------------
//
//double CFullNetBlk::GetSolveFxdFlwQSecs()
//  {
//  double N=0;
//  for (long i=0; (i<NNets); i++)
//    {
//    CNetBlk &NB=Nets[i];
//    for (long j=0 ; (j<NB.NSubNets); j++)
//      N+=NB.SubNet[j]->SolveFxdFlwQTicks.Secs();
//    }
//  return N;
//  }
//
////-------------------------------------------------------------------------
//
//double CFullNetBlk::GetSolveFxdFlwPSecs()
//  {
//  double N=0;
//  for (long i=0; (i<NNets); i++)
//    {
//    CNetBlk &NB=Nets[i];
//    for (long j=0 ; (j<NB.NSubNets); j++)
//      N+=NB.SubNet[j]->SolveFxdFlwPTicks.Secs();
//    }
//  return N;
//  }

//===========================================================================

IMPLEMENT_SPARES(CGrpFlwIndex, 5000);
IMPLEMENT_SPARES(CGrpFlwBlk, 5000);
IMPLEMENT_SPARES(CSubNetBlk, 100);
IMPLEMENT_SPARES(CNetBlk, 100);
IMPLEMENT_SPARES(CFullNetBlk, 100);

//===========================================================================
//
//
//
//===========================================================================
