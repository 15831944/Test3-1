//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdio.h>

#include "sc_defs.h"
#include "datacnvs.h"
#include "sp_cont.h"
#define  __MULTIFB_CPP
#include "multifb.h"
#include "m_base.h"

#include "errorlog.h"
#include "dbhelper.h"
#include "dbgmngr.h"

#define dbgMultiFB       WITHDEBUG

#if dbgMultiFB
static CDbgMngr dbgDoFlash      ("MultiFB",  "DoFlash");
static CDbgMngr dbgShowIter     ("MultiFB",  "ShowIter");
#endif

//==========================================================================
//
//
//
//==========================================================================

CFlwEqnGrp MFBPipeGroup("MFBPipe", "MFBPipe");
CFlwEqnGrp MFBJoinGroup("MFBJoin", "MFBJoin");
CFlwEqnGrp MFBEnExGroup("MFBEnEx", "MFBEnEx");

//==========================================================================
//
//
//
//==========================================================================
//DEFINE_MEMSTATS(CFlwEqn)
//  DEFINE_MEMSTATS(CFlwEqn)
//  protected:
//    double  dOverideRho, dOverideTemp, dOverideTau;

#if WITHMFBDEDT
IMPLEMENT_TAGOBJEDT(CMultiFlwBlk, "MultiFB", "FB_Grp", "", "", TOC_ALL|TOC_GRP_GENERAL|TOC_STD_KENWALT, CMultiFlwBlkEdt, "Multi flow block", "Multi flow block");
#else
IMPLEMENT_TAGOBJ(CMultiFlwBlk, "MultiFB", "FB_Grp", "", "", TOC_ALL|TOC_GRP_GENERAL|TOC_STD_KENWALT, "Multi flow block", "Multi flow block");
#endif

CMultiFlwBlk::CMultiFlwBlk(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  TaggedObject(pClass_, pTag, pAttach, eAttach),
  Common(NULL)
  {
  fOnRqd=fOn=false;
  iIoIn=-1;
  iIoOut=-1;
  FBCnt.Msk=0;
  }

//--------------------------------------------------------------------------

CMultiFlwBlk::CMultiFlwBlk(pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach, int IoIn, int IoOut, CVLEBase * VLE) :
  TaggedObject(&CMultiFlwBlkClass, pTag, pAttach, eAttach),
  Common(VLE)
  {
  fOnRqd=fOn=false;
  iIoIn=IoIn;
  iIoOut=IoOut;
  FBCnt.Msk=0;
  };

//--------------------------------------------------------------------------

CMultiFlwBlk::~CMultiFlwBlk()
  {
  FlwNode * pNd = (FlwNode *) pAttachedTo;
  if (pNd==NULL)
    {
    for (int i=0; i<FBA.GetSize(); i++)
      delete FBA[i].pFB;
    }
  };

//--------------------------------------------------------------------------

void CMultiFlwBlk::SetOn(flag On)
  {
  fOn=fOnRqd=On;
  if (fOn)
    GetFBInfo();
  };

//--------------------------------------------------------------------------

void CMultiFlwBlk::SetOnRqd(flag On)
  {
  fOnRqd=On;
  };


//--------------------------------------------------------------------------

void AssignFG(FlwBlk *FB, CFlwEqnGrp &FEG, CMultiFlwBlk * pThis)
  {
  FB->AssignFlwEqnGroup(FEG, FEG.Default(), pThis);
  if (FB->Eqn()==NULL)
    FB->CreateFlwEqn(FEG.Default());
  }

//--------------------------------------------------------------------------

void CMultiFlwBlk::AssignFlowGroups()
  {

  FlwNode * pNd = (FlwNode *) pAttachedTo;
  //ASSERT(pNd);
  if (pNd)
    {
    for (int i=0; i<pNd->NoFlwIOs(); i++)
      {
      // Remote Flw Blks
      int j=pNd->NIOFBs_Rmt(i)-1;
      if (pNd->IOPipeEntry_Rmt(i))
        pNd->IOFB_Rmt(i,j--)->AssignFlwEqnGroup(MFBEnExGroup, MFBEnExGroup.Default(), this);
      else if (pNd->IOPipeJoin_Rmt(i))
        pNd->IOFB_Rmt(i,j--)->AssignFlwEqnGroup(MFBJoinGroup, MFBJoinGroup.Default(), this);
      else // Do not modify the remote
        j=-1;
      while (j>=0)
        pNd->IOFB_Rmt(i,j--)->AssignFlwEqnGroup(MFBPipeGroup, MFBPipeGroup.Default(), this);

      // Local Flw Blks
      for (j=0; j<pNd->NIOFBs(i); j++)
        pNd->IOFB(i,j)->AssignFlwEqnGroup(MFBPipeGroup, MFBPipeGroup.Default(), this);
      }
    GetFBInfo();
    }
  else
    {
    GetFBInfo();


    int i0=0;
    if (FBCnt.Cnt[0])
      {
      i0=1;
      AssignFG(FBA[0].pFB, MFBEnExGroup, this);
      }
//    int i0=(FBCnt.Cnt[0] ? 1 : 0);
    int i1=FBA.GetSize()-1;
    if (FBCnt.Cnt[3])
      {
      i1=FBA.GetSize()-2;
      AssignFG(FBA[FBA.GetSize()-1].pFB, MFBEnExGroup, this);
      }
//    int i1=(FBCnt.Cnt[3] ? FBA.GetSize()-2 : FBA.GetSize()-1);
    for (int i=i0; i<=i1; i++)
      AssignFG(FBA[i].pFB, MFBPipeGroup, this);
//      {
//      FlwBlk &FB=*FBA[i].pFB;
//      FB.AssignFlwEqnGroup(MFBPipeGroup, MFBPipeGroup.Default(), this);
//      if (FB.Eqn()==NULL)
//        FB.CreateFlwEqn(MFBPipeGroup.Default());
//      }
    }
  for (int i=0; i<FBA.GetSize(); i++)
    {
    //FlwBlk &FB=*FBA[i].pFB;
    MFB_Eqn * pM = dynamic_cast<MFB_Eqn *>(FBA[i].pFB->Eqn());
    if (pM)
      pM->pMFCB=&Common;//this;
    }
  }

//--------------------------------------------------------------------------

void CMultiFlwBlk::GetFBInfo()
  {
  FlwNode * pNd = (FlwNode *) pAttachedTo;
  //ASSERT(pNd);
  if (pNd)
    {
    if (pNd->NoFlwIOs()<2)
      {
      FBCnt.Msk=0;
      return;
      }

    if (pNd->IOPipeEntry_Rmt(iIoIn) || pNd->IOPipeJoin_Rmt(iIoIn))
      FBCnt.Cnt[0]=pNd->NIOFBs_Rmt(iIoIn);
    else
      FBCnt.Cnt[0]=0;
    FBCnt.Cnt[1]=pNd->NIOFBs(iIoIn);
    FBCnt.Cnt[2]=pNd->NIOFBs(iIoOut);
    if (pNd->IOPipeEntry_Rmt(iIoOut) || pNd->IOPipeJoin_Rmt(iIoOut))
      FBCnt.Cnt[3]=pNd->NIOFBs_Rmt(iIoOut);
    else
      FBCnt.Cnt[3]=0;
    FBA.SetSize(FBCnt.Total());

    int i, j=0;
    for (i=FBCnt.Cnt[0]-1; i>=0 ; i--)
      FBA[j++].pFB = pNd->IOFB_Rmt(iIoIn, i);
    for (i=0; i<FBCnt.Cnt[1]; i++)
      FBA[j++].pFB= pNd->IOFB(iIoIn, i);
    for (i=FBCnt.Cnt[2]-1; i>=0; i--)
      FBA[j++].pFB= pNd->IOFB(iIoOut, i);
    for (i=0; i<FBCnt.Cnt[3]; i++)
      FBA[j++].pFB= pNd->IOFB_Rmt(iIoOut, i);
    }
  else
    {
    }

  int i, j=0;
  for (i=FBCnt.Cnt[0]-1; i>=0 ; i--)
    {
    FBA[j].iBlk=0;
    FBA[j].iFE =i;
    FBA[j].iSgn=-1;
    j++;
    }
  for (i=0; i<FBCnt.Cnt[1]; i++)
    {
    FBA[j].iBlk=1;
    FBA[j].iFE =i;
    FBA[j].iSgn=1;
    j++;
    }
  for (i=FBCnt.Cnt[2]-1; i>=0; i--)
    {
    FBA[j].iBlk=2;
    FBA[j].iFE =i;
    FBA[j].iSgn=-1;
    j++;
    }
  for (i=0; i<FBCnt.Cnt[3]; i++)
    {
    FBA[j].iBlk=3;
    FBA[j].iFE =i;
    FBA[j].iSgn=1;
    j++;
    }
  }

//--------------------------------------------------------------------------

void CMultiFlwBlk::SetFBs()
  {
  FlwNode * pNd = (FlwNode *) pAttachedTo;
  //ASSERT(pNd);
  if (pNd)
    {
    if (pNd->NoFlwIOs()>=2)
      {
      if (FBCnt.Cnt[0])
        pNd->SetNIOFBs_Rmt(iIoIn,  FBCnt.Cnt[0]);
      pNd->SetNIOFBs(iIoIn,      FBCnt.Cnt[1]);
      pNd->SetNIOFBs(iIoOut,     FBCnt.Cnt[2]);
      if (FBCnt.Cnt[3])
        pNd->SetNIOFBs_Rmt(iIoOut, FBCnt.Cnt[3]);
      }
    }
  else
    {
    // Already OK
    int n=FBA.GetSize();
    for (int i=FBCnt.Total(); i<n; i++)
      delete FBA[i].pFB;

    FBA.SetSize(FBCnt.Total());

    for (i=n; i<FBCnt.Total(); i++)
      FBA[i].pFB=new FlwBlk();
    }
  }

//--------------------------------------------------------------------------

void CMultiFlwBlk::ChgFBs(int Blk, int At, int Inc)
  {
  int & Cnt=FBCnt.Cnt[Blk];
  int iOff=0;
  for (int b=0; b<Blk; b++)
    iOff+=FBCnt.Cnt[b];

  if (Inc>0)
    {
    CFBPtr A;
    At=Range(1, iOff+At, (int)FBCnt.Total()-1);
    FBA.InsertAt(At, A, Inc);
    Cnt+=Inc;
    for (int i=0; i<Inc; i++)
      FBA[At+i].pFB=new FlwBlk();
    }
  else if (Inc<0)
    {
    int Dec=-Inc;
    Dec=Min(Cnt-1, Dec);
    if (Dec>0 && FBCnt.Cnt[b]>1)
      {
      At=Range(1, iOff+At, (int)FBCnt.Total()-2);
      FBA.RemoveAt(At, Dec);
      Cnt-=Dec;
      }
    }
  GetFBInfo();
  AssignFlowGroups();
  };

//--------------------------------------------------------------------------

static char * MFBDescsOld[4] = {"Entry", "Src", "Dst", "Exit", };
static char * MFBDescs[4]    = {"Src", "Ent", "Ext", "Dst", };

//--------------------------------------------------------------------------

void CMultiFlwBlk::BuildDataDefn(DataDefnBlk & DDB)
  {
  FlwNode * pNd = (FlwNode *) pAttachedTo;
  if (DDB.BeginStruct(this))
    {
    if (fOn || (pNd==NULL))
      {

      //DDB.Byte  ("dMagn_Fac",   "",  DC_,      "",      &Common.dMagn_Fac,    this,  isParm);
      DDB.Double("dMagn_Fac",   "",  DC_,      "",      &Common.dMagn_Fac,    this,  isParm);
      DDB.Double("dPowr_Fac",   "",  DC_,      "",      &Common.dPowr_Fac,    this,  isParm);
      DDB.Double("dAge_Fac",    "",  DC_,      "",      &Common.dAge_Fac,     this,  isParm);
      DDB.Double("dRoughness",  "",  DC_L,     "mm",    &Common.dRoughness,   this,  isParm);

//      Common.LVisc.BuildDataDefn(DDB, "LViscosity",  "",  DC_Visc,  "mPas", lHandle, this);
//      Common.GVisc.BuildDataDefn(DDB, "GViscosity",  "",  DC_Visc,  "mPas", lHandle, this);

      DDB.Double("LVisc",     "",  DC_Visc,  "mPas",    &Common.LVisc,  this,  isParm);
      //DDB.Double("LViscRqd",  "",  DC_Visc,  "mPas",    &Common.LVisc.dOvr,  this,  isParm|NAN_OK);
      DDB.Double("GVisc",     "",  DC_Visc,  "mPas",    &Common.GVisc,  this,  isParm);
      //DDB.Double("GViscRqd",  "",  DC_Visc,  "mPas",    &Common.GVisc.dOvr,  this,  isParm|NAN_OK);
      DDB.Double("LDens",     "",  DC_Rho,   "kg/m^3",  &Common.LDens,  this,  isParm);
      //DDB.Double("LDensRqd",  "",  DC_Rho,   "kg/m^3",  &Common.LDens.dOvr,  this,  isParm|NAN_OK);
      DDB.Double("GDens",     "",  DC_Rho,   "kg/m^3",  &Common.GDens,  this,  isParm);
      //DDB.Double("GDensRqd",  "",  DC_Rho,   "kg/m^3",  &Common.GDens.dOvr,  this,  isParm|NAN_OK);
      DDB.Double("STens",     "",  DC_SurfTens, "N/m",  &Common.STens,  this,  isParm|NAN_OK);
      //DDB.Double("STensRqd",  "",  DC_SurfTens, "N/m",  &Common.STens.dOvr,  this,  isParm|NAN_OK);

      DDB.DWord("FBCntMsk",   "", DC_, "",  xidMFBBuildIt,   this, isParm);

      if (DDB.BeginArray(this, "FB", (PrjFileVerNo()<22) ? "FlwBlk" : "FB_Data", FBCnt.Total()))
        {
        char ** pD=PrjFileVerNo()<22 ? MFBDescsOld: MFBDescs;
        for (int i=0; i<FBA.GetSize(); i++)
          {
          Strng Tg;
          Tg.Set("%s-%i", pD[FBA[i].iBlk], FBA[i].iFE);
          if (DDB.BeginElement(this, Tg(), NULL, 0x0100000|i))
            {
            DDB.SuppressNextBlock();
            FBA[i].pFB->BuildDataDefn(FBDDOpt_WithAll|FBDDOpt_WithXtraEqn, DDB, this, (PrjFileVerNo()<22) ? "FB" : "Eqn", -1, DDB_NoPage);
            }
          }
        }
      DDB.EndArray();
      }
    }
  DDB.EndStruct();
  };

//--------------------------------------------------------------------------

flag CMultiFlwBlk::DataXchg(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
    case xidMFBBuildIt :
      {
      if (DCB.rDW)
        {
        //CFBCount RqdFBCnt;
        //RqdFBCnt.Msk=*DCB.rDW;
        //
        //for (int b=0; b<4; b++)
        //  {
        //  while (FBCnt.Cnt[b]!=RqdFBCnt.Cnt[b])
        //    {
        //    if (RqdFBCnt.Cnt[b]>FBCnt.Cnt[b])
        //      ChgFBs(b, FBCnt.Cnt[b], +1);
        //    else
        //      ChgFBs(b, FBCnt.Cnt[b]-1, -1);
        //    }
        //  }

        FBCnt.Msk=*DCB.rDW;
        SetFBs();
        AssignFlowGroups();
        }
      //else
      GetFBInfo();
      DCB.DW=FBCnt.Msk;
      return 1;
      }
    }

  if (DCB.dwUserInfo & 0x0100000)
    {
    int i=DCB.dwUserInfo & 0xffff;
    if (FBA[i].pFB->DataXchg(DCB))
      return 1;
    }

  if (TaggedObject::DataXchg(DCB))
    return 1;

  return 0;
  };

//--------------------------------------------------------------------------

flag CMultiFlwBlk::ValidateData(ValidateDataBlk & VDB)
  {
  fOn=fOnRqd;
  flag OK=True;
  return OK;
  }

//--------------------------------------------------------------------------

void CMultiFlwBlk::SetPhysicalData()
  {
  for (int i=0; i<FBA.GetSize(); i++)
    FBA[i].pFB->SetActLength(FBA[i].pFB->RqdLength());
  FlwNode * pNd = (FlwNode *) pAttachedTo;
  if (pNd)
    {
    }
  };

//--------------------------------------------------------------------------

void CMultiFlwBlk::SetDatums(int Pass, CFlwNodeIndexList & List, int IOIn, double &RiseRqd, double &DatumChg)
  {
  Z_States xxx=true;

  MdlNode * pNd = dynamic_cast<MdlNode*>(pAttachedTo);
  if (pNd)
    {
    DatumChg=0.0;
    for (int j=0; j<pNd->NIOFBs(0); j++)
      DatumChg+=pNd->IOFB(0, j)->Rise();
    for (j=0; j<pNd->NIOFBs(1); j++)
      DatumChg+=-pNd->IOFB(1, j)->Rise();

    pNd->SetDatums_Link(Pass, List, IOIn, RiseRqd, DatumChg);

    }
  return;
  }

//--------------------------------------------------------------------------

void CMultiFlwBlk::EvalJoinPressures(flag DoXfer, flag AppRhoH)
  {
  Common.fDoXfer=DoXfer;
  Common.fAppRhoH=AppRhoH;
  Common.fFindRhoH=false;

//  fDoXfer=DoXfer;
  MdlNode * pNd = dynamic_cast<MdlNode*>(pAttachedTo);
  if (pNd)
    {
    for (int i=0; i<2; i++)
      {
      if (pNd->IOPipeEntry_Rmt(i) || pNd->IOPipeJoin_Rmt(i))
        {
        for (int j=0; j<pNd->NIOFBs_Rmt(i); j++)
          {
          FlwBlk & FB=*pNd->IOFB_Rmt(i,j);
          FB.SetDPz((DoXfer ? 0.0 : FB.StaticHead(pNd->IONetProps(i)/*, Common.fAppRhoH*/)),0.0);
          if (j!=pNd->NIOFBs_Rmt(i)-1)
            FB.SetDPzX(0.0, 0.0);
          }
        }
      for (int j=0; j<pNd->NIOFBs(i); j++)
        {
        FlwBlk & FB=*pNd->IOFB(i,j);
        FB.SetDPz((DoXfer ? 0.0 : FB.StaticHead(pNd->IONetProps(i)/*, Common.fAppRhoH*/)),0.0);
        FB.SetDPzX(0.0, 0.0);
        }
      }
    }
  };

//===========================================================================
//
//                                  CMultiFlwBlk
//
//===========================================================================

const int FldBase          = 20;

const int Id_ColShow       = 1;
const int Id_ColHide       = 2;
const int Id_Roughness     = 3;
const int Id_GVisc         = 4;
const int Id_GViscRqd      = 5;
const int Id_LVisc         = 6;
const int Id_LViscRqd      = 7;
const int Id_GDens         = 8;
const int Id_GDensRqd      = 9;
const int Id_LDens         = 10;
const int Id_LDensRqd      = 11;
const int Id_STens         = 12;
const int Id_STensRqd      = 13;


const int Id_First         = FldBase+0;

const int Id_Class         = FldBase+0;
const int Id_Desc          = FldBase+1;
const int Id_IdStr         = FldBase+2;
const int Id_Location      = FldBase+3;
const int Id_Diam          = FldBase+4;
const int Id_RqdLength     = FldBase+5;
const int Id_dZ            = FldBase+6;
const int Id_Z             = FldBase+7;
const int Id_ActLength     = FldBase+8;
const int Id_KFact         = FldBase+9;
const int Id_FricFact      = FldBase+10;
const int Id_Velocity      = FldBase+11;
const int Id_dPFric        = FldBase+12;
const int Id_dPAcc         = FldBase+13;
const int Id_dPStat        = FldBase+14;
const int Id_dP            = FldBase+15;
const int Id_Press         = FldBase+16;
const int Id_QmL           = FldBase+17;
const int Id_QmV           = FldBase+18;
const int Id_Vfrac         = FldBase+19;
const int Id_Slip          = FldBase+20;
const int Id_Void          = FldBase+21;
const int Id_OnePh         = FldBase+22;
const int Id_Temp          = FldBase+23;
//const int     = FldBase+22;

const int Id_Last          = FldBase+25;

//------------------------------------------------------------------------------

#if WITHMFBDEDT
CMultiFlwBlkEdt::CMultiFlwBlkEdt(pFxdEdtView pView_, pCMultiFlwBlk pMFB_) :
  FxdEdtBookRef(pView_),
  rMFB(*pMFB_),
  ViscCnv(DC_Visc,     "mPas"),
  ViscFmt("", 0, 3, 'f'),
  DensCnv(DC_Rho,     "kg/m^3"),
  DensFmt("", 0, 3, 'f'),
  STensCnv(DC_SurfTens,     "N/m"),
  STensFmt("", 0, 3, 'f'),
  RoughCnv(DC_L,     "mm"),
  RoughFmt("", 0, 3, 'f')
  {
  rMFB.SetOn(true);
  iPg1=0;

  CDs.SetAtGrow(Id_Class-FldBase,     MFBColDesc("Class",     Id_Class,     true,    7, 0, 0,      "",     1));
  CDs.SetAtGrow(Id_Desc-FldBase,      MFBColDesc("Desc",      Id_Desc,      true,   12, 0, 0,      "",     1));
  CDs.SetAtGrow(Id_Diam-FldBase,      MFBColDesc("Diam",      Id_Diam,      true,    8, 2, DC_L,   "mm",   1));
  CDs.SetAtGrow(Id_IdStr-FldBase,     MFBColDesc("IdStr",     Id_IdStr,        false,   8, 2, 0,      "",     1));
  CDs.SetAtGrow(Id_Location-FldBase,  MFBColDesc("Location",  Id_Location,  false,   8, 2, 0,      "",     1));
  //CDs.SetAtGrow(Id_Beta-FldBase,      MFBColDesc("Beta",      Id_Beta,      true,    8, 2, DC_,    "",     3));
  CDs.SetAtGrow(Id_RqdLength-FldBase, MFBColDesc("RqdLen",    Id_RqdLength, true,    8, 2, DC_L,   "m",    3/*2*/));
  CDs.SetAtGrow(Id_dZ-FldBase,        MFBColDesc("dZ",        Id_dZ,        true,    8, 2, DC_L,   "m",    3/*2*/));
  CDs.SetAtGrow(Id_Z-FldBase,         MFBColDesc("Z",         Id_Z,         false,   6, 2, DC_L,   "m",    1/*2*/));
  CDs.SetAtGrow(Id_ActLength-FldBase, MFBColDesc("ActLen",    Id_ActLength, false,   6, 2, DC_L,   "m",    3/*2*/));
  CDs.SetAtGrow(Id_KFact-FldBase,     MFBColDesc("K",         Id_KFact,     true,    8, 2, DC_,    "",     3));
  CDs.SetAtGrow(Id_FricFact-FldBase,  MFBColDesc("f",         Id_FricFact,  true,    8, 2, DC_,    "",     5));
  CDs.SetAtGrow(Id_Velocity-FldBase,  MFBColDesc("Vel",       Id_Velocity,  true,    8, 2, DC_Ldt, "m/s",  3/*2*/));
  CDs.SetAtGrow(Id_dPFric-FldBase,    MFBColDesc("dPFric",    Id_dPFric,    false,  10, 2, DC_DP,  "kPa",  3));
  CDs.SetAtGrow(Id_dPAcc-FldBase,     MFBColDesc("dPAcc",     Id_dPAcc,     false,  10, 2, DC_DP,  "kPa",  3));
  CDs.SetAtGrow(Id_dPStat-FldBase,    MFBColDesc("dPStat",    Id_dPStat,    false,  10, 2, DC_DP,  "kPa",  3));
//  CDs.SetAtGrow(Id_dPStatX-FldBase,   MFBColDesc("dPStatX",   Id_dPStatX,   false,  10, 2, DC_DP,  "kPa",  3));
  CDs.SetAtGrow(Id_dP-FldBase,        MFBColDesc("dP",        Id_dP,        true,   10, 2, DC_DP,  "kPa",  3));
  CDs.SetAtGrow(Id_Press-FldBase,     MFBColDesc("Press",     Id_Press,     true,   10, 2, DC_P,   "kPa",  2));
  CDs.SetAtGrow(Id_QmV-FldBase,       MFBColDesc("QmV",       Id_QmV,       false,  10, 2, DC_Qm,  "kg/s", 2));
  CDs.SetAtGrow(Id_QmL-FldBase,       MFBColDesc("QmL",       Id_QmL,       false,  10, 2, DC_Qm,  "kg/s", 2));
  CDs.SetAtGrow(Id_Vfrac-FldBase,     MFBColDesc("Vf",        Id_Vfrac,     false,   7, 2, DC_Frac,"%",  3));
  CDs.SetAtGrow(Id_Slip-FldBase,      MFBColDesc("Slip",      Id_Slip,      true,    7, 2, DC_,    "",   3));
  CDs.SetAtGrow(Id_Void-FldBase,      MFBColDesc("Void",      Id_Void,      false,   7, 2, DC_Frac,"%",   3));
  CDs.SetAtGrow(Id_OnePh-FldBase,     MFBColDesc("OnePh",     Id_OnePh,     true,    7, 2, DC_Frac,"%",  2));
  CDs.SetAtGrow(Id_Temp-FldBase,      MFBColDesc("T",         Id_Temp,      false,   7, 2, DC_T,   "C",  2));

  ObjectAttribute *pAttr=ObjAttributes.FindObject("MultiFB");
  CProfINIFile PF(PrjIniFile());
  if (pAttr)
    {
    pAttr->FieldFmtCnvs("Viscosity", ViscFmt,  ViscCnv);
    pAttr->FieldFmtCnvs("Density", DensFmt,  DensCnv);
    pAttr->FieldFmtCnvs("SurfaceTension", STensFmt,  STensCnv);
    pAttr->FieldFmtCnvs("Roughness", RoughFmt,  RoughCnv);
    }
  for (int i=0;i<CDs.GetSize(); i++)
    {
    CDs[i].bOn=PF.RdInt("MultiFB", CDs[i].sTag, CDs[i].bOn)!=0;
    if (pAttr)
      pAttr->FieldFmtCnvs(CDs[i].sTag, CDs[i].Fmt,  CDs[i].Cnv);
    }
  }

//---------------------------------------------------------------------------

CMultiFlwBlkEdt::~CMultiFlwBlkEdt()
  {
  ObjectAttribute *pAttr=ObjAttributes.FindObject("MultiFB");
  if (pAttr)
    {
    pAttr->SetFieldFmt ("Viscosity", ViscFmt);
    pAttr->SetFieldCnvs("Viscosity", ViscCnv);
    pAttr->SetFieldFmt ("Density", DensFmt);
    pAttr->SetFieldCnvs("Density", DensCnv);
    pAttr->SetFieldFmt ("SurfaceTension", STensFmt);
    pAttr->SetFieldCnvs("SurfaceTension", STensCnv);
    pAttr->SetFieldFmt ("Roughness", RoughFmt);
    pAttr->SetFieldCnvs("Roughness", RoughCnv);
    }
  CProfINIFile PF(PrjIniFile());
  for (int i=0;i<CDs.GetSize(); i++)
    {
    PF.WrInt("MultiFB", CDs[i].sTag, CDs[i].bOn);
    if (pAttr)
      {
      pAttr->SetFieldFmt(CDs[i].sTag, CDs[i].Fmt);
      pAttr->SetFieldCnvs(CDs[i].sTag, CDs[i].Cnv);
      }
    }
  }

//---------------------------------------------------------------------------

int CMultiFlwBlkEdt::DistFromPgNo(int Pg)
  {
  return -1;
  }

//---------------------------------------------------------------------------

void CMultiFlwBlkEdt::PutDataStart()
  {
  }

//---------------------------------------------------------------------------

void CMultiFlwBlkEdt::PutDataDone()
  {
  }

//---------------------------------------------------------------------------

void CMultiFlwBlkEdt::GetDataStart()
  {
  }

//---------------------------------------------------------------------------

void CMultiFlwBlkEdt::GetDataDone()
  {
  }

//---------------------------------------------------------------------------

void CMultiFlwBlkEdt::StartBuild()
  {
  }

//---------------------------------------------------------------------------

//#define FB0(b)      (((b)&1)==0 ? rMFB.FBCnt.Cnt[(b)]-1 : 0)
//#define FB1(b)      (((b)&1)==0 ? -1 : rMFB.FBCnt.Cnt[(b)])
//#define FBI(b)      (((b)&1)==0 ? -1 : 1)
//#define FBPtr(b,i)  (((b)&1)==0 ? rMFB.FBs[b][rMFB.FBCnt.Cnt[b]-(i)-1] : rMFB.FBs[b][i])
#define FBPtr(iOff,i)  rMFB.FBA[iOff+i].pFB;
#define FBSgn(b)    (((b)&1)==0 ? -1 : 1)

void CMultiFlwBlkEdt::Build()
  {
  Strng S;
  int L;
  iPg1=pView->Pages;

  StartPage("MFB");
  StartBlk(4, 0, NULL);
  L=0;
  SetSpace(L, 1);
  L++;
  //SetSpace(L, 1);
  SetDParm(L,"Show", 5, "", Id_ColShow, 15, 0, "  ");
  int GotOne=0;
  FldHasFixedStrValue(-1, " ");
  for (int i=1; i<CDs.GetSize(); i++)
    if (!CDs[i].bOn && CDs[i].sTag)
      {
      FldHasFixedStrValue(i, CDs[i].sTag);
      GotOne=1;
      }
//  if (!GotOne)
//    FldHasFixedStrValue(-1, " ");
//  L++;
  SetSpace(L, 2);
  SetDParm(L,"Hide", 5, "", Id_ColHide, 15, 0, "");
  GotOne=0;
  FldHasFixedStrValue(-1, " ");
  for (i=1; i<CDs.GetSize(); i++)
    if (CDs[i].bOn && CDs[i].sTag)
      {
      FldHasFixedStrValue(i, CDs[i].sTag);
      GotOne=1;
      }
//  if (!GotOne)
//    FldHasFixedStrValue(-1, " ");
  L++;
  L++;

  for (i=0; i<CDs.GetSize(); i++)
    if (CDs[i].bOn)
      {
      SetDesc(L, CDs[i].sTag,    -1, CDs[i].iWide,  2, "");
      if (CDs[i].iId==Id_Class)
        SetSpace(L, 3);
      }

  int iFB=0;
  for (int b=0; b<4; b++)
    {
    rMFB.FBEdtBlkId[b]=-1;
    if (rMFB.FBCnt.Cnt[b])
      {
      StartBlk(1+rMFB.FBCnt.Cnt[b], 1, NULL);
      rMFB.FBEdtBlkId[b]=View().CPage()->CurBlkId;
      L=0;

      char ** pD=PrjFileVerNo()<22 ? MFBDescsOld: MFBDescs;
      SetDesc(L, pD[b],  -1, CDs[0].iWide,  0, "");
      SetSpace(L, 3);
      if (iFB==0)
        {
        for (int i=1; i<CDs.GetSize(); i++)
          if (CDs[i].bOn)
            {
            switch (CDs[i].iId)
              {
              case Id_Press:
              case Id_Z:
                SetParm(L, " ",  CDs[i].iId, CDs[i].iWide,  CDs[i].iJust, "");
                break;
              default:
                SetSpace(L, CDs[i].iWide);
                break;
              }

            }
        }
      L++;

      for (int f=0; f<rMFB.FBCnt.Cnt[b]; f++)
        {
        for (int i=0; i<CDs.GetSize(); i++)
          if (CDs[i].bOn)
            {
            SetParm(L, " ",  CDs[i].iId, CDs[i].iWide,  CDs[i].iJust, "");
            DDBValueLstMem DDB0;
            if (CDs[i].iId==Id_Class)
              {
              FlwBlk * pFB=rMFB.FBA[iFB].pFB;
              int xx=0;
              TagObjClass::GetSDescValueLst(pFB->GroupName(), DDB0);//, NoEqnName);
              for (int j=0; j<DDB0.Length(); j++)
                FldHasFixedStrValue(j, DDB0.Item(j)->m_pStr);
              if (DDB0.Length()==0)
                FldHasFixedStrValue(-1, "-");
              SetSpace(L, 3);
              }

            }
        L++;
        iFB++;
        }
      }
    }
  StartPage("..");
  StartBlk(4, 0, NULL);
  L=0;
  SetSpace(L,2);
  L++;
  SetDesc(L, "Common Data",    -1, 20,  0, "");
  L++;
  SetDParm(L,"Gas Density",   16, "", Id_GDens, 10, 2, " ");
  SetDParm(L,"   Reqd",    7, "", Id_GDensRqd, 10, 2, " ");
  L++;
  SetDParm(L,"Liq Density",   16, "", Id_LDens, 10, 2, " ");
  SetDParm(L,"   Reqd",    7, "", Id_LDensRqd, 10, 2, " ");
  L++;
  SetDParm(L,"Gas Viscosity",   16, "", Id_GVisc, 10, 2, " ");
  SetDParm(L,"   Reqd",    7, "", Id_GViscRqd, 10, 2, " ");
  L++;
  SetDParm(L,"Liq Viscosity",   16, "", Id_LVisc, 10, 2, " ");
  SetDParm(L,"   Reqd",    7, "", Id_LViscRqd, 10, 2, " ");
  L++;
  SetDParm(L,"Surface Tension",   16, "", Id_STens, 10, 2, " ");
  SetDParm(L,"   Reqd",    7, "", Id_STensRqd, 10, 2, " ");
  L++;
  SetDParm(L,"Roughness",       16, "", Id_Roughness, 10, 2, " ");

  }

//---------------------------------------------------------------------------

void CMultiFlwBlkEdt::Load(FxdEdtInfo &EI, Strng & Str)
  {
  if (CurrentBlk(EI))
    {
    switch (EI.PageNo-iPg1)
      {
      case 0:
        switch (EI.FieldId)
          {
          case Id_ColShow:
          case Id_ColHide:
            {
//            for (int i=1; i<CDs.GetSize(); i++)
//              if (CDs[i].bOn && EI.FieldId==Id_ColHide ||
//                  !CDs[i].bOn && EI.FieldId==Id_ColShow)
//                {
//                Str=CDs[i].sTag;
//                break;
//                }
            Str=" ";
            break;
            }
          }
        break;
      case 1:
        switch (EI.FieldId)
          {
          case Id_GVisc:
            ViscFmt.FormatFloat(ViscCnv.Human(rMFB.Common.GVisc), Str);
            EI.Fld->fEditable=false;
            break;
          //case Id_GViscRqd:
          //  ViscFmt.FormatFloat(ViscCnv.Human(rMFB.Common.GVisc.Ovr()), Str);
          //  EI.Fld->fEditable=true;
          //  break;
          case Id_LVisc:
            ViscFmt.FormatFloat(ViscCnv.Human(rMFB.Common.LVisc), Str);
            EI.Fld->fEditable=false;
            break;
          //case Id_LViscRqd:
          //  ViscFmt.FormatFloat(ViscCnv.Human(rMFB.Common.LVisc.Ovr()), Str);
          //  EI.Fld->fEditable=true;
          //  break;
          case Id_GDens:
            DensFmt.FormatFloat(DensCnv.Human(rMFB.Common.GDens), Str);
            EI.Fld->fEditable=false;
            break;
          //case Id_GDensRqd:
          //  DensFmt.FormatFloat(DensCnv.Human(rMFB.Common.GDens.Ovr()), Str);
          //  EI.Fld->fEditable=true;
          //  break;
          case Id_LDens:
            DensFmt.FormatFloat(DensCnv.Human(rMFB.Common.LDens), Str);
            EI.Fld->fEditable=false;
            break;
          //case Id_LDensRqd:
          //  DensFmt.FormatFloat(DensCnv.Human(rMFB.Common.LDens.Ovr()), Str);
          //  EI.Fld->fEditable=true;
          //  break;
          case Id_STens:
            STensFmt.FormatFloat(STensCnv.Human(rMFB.Common.STens), Str);
            EI.Fld->fEditable=false;
            break;
          //case Id_STensRqd:
          //  STensFmt.FormatFloat(STensCnv.Human(rMFB.Common.STens.Ovr()), Str);
          //  EI.Fld->fEditable=true;
          //  break;
          case Id_Roughness:
            RoughFmt.FormatFloat(RoughCnv.Human(rMFB.Common.dRoughness), Str);
            EI.Fld->fEditable=true;
            break;
          }
        break;
      }
    }

//  if (CurrentBlk(EI))
//    {
//    int pg=EI.PageNo;
//    int i=(int)(EI.BlkRowNo-EI.Index);
//
//    switch (EI.FieldId)
//      {
//      case Id_ColShow:
//      case Id_ColHide:
//        {
//        for (int i=1; i<CDs.GetSize(); i++)
//          if (CDs[i].bOn && EI.FieldId==Id_ColHide ||
//              !CDs[i].bOn && EI.FieldId==Id_ColShow)
//            {
//            Str=CDs[i].sTag;
//            break;
//            }
//        break;
//        }
//      }
//    }

#define DBL(D)    { CDs[Col].Fmt.FormatFloat(CDs[Col].Cnv.Human(D), Str); EI.Fld->fEditable=false; }
#define DBLEDT(D) { CDs[Col].Fmt.FormatFloat(CDs[Col].Cnv.Human(D), Str); EI.Fld->fEditable=true; }

  int iOff=0;
  for (int b=0; b<4; b++)
    if (rMFB.FBCnt.Cnt[b])
      {
      if (CurrentBlk(EI))
        {
        int pg=EI.PageNo-iPg1;
        int i=(int)(EI.BlkRowNo-EI.Index);

        int Col=EI.FieldId-FldBase;
        double D=0;
        if (i<0)
          {
          FlwBlk * pFB=FBPtr(iOff, 0);
          switch (EI.FieldId)
            {
            case Id_Press:      DBL   (pFB->m_pNdPB ? pFB->NodePress() : dNAN); break;
            case Id_Z:          DBL   (pFB->DatumNode());         break;
            default :           Str.Set(" ??? %i", EI.FieldId);   break;
            }
          }
        else
          {
          FlwBlk * pFB=FBPtr(iOff, i);
          switch (EI.FieldId)
            {
            case Id_Class:      Str=pFB->Eqn() ? pFB->Eqn()->ShortDesc() : "-"; EI.Fld->fEditable=true; break;
            case Id_Desc:       Str=pFB->Eqn() ? pFB->Eqn()->EqpDesc() : ""; EI.Fld->fEditable=true; break;
            case Id_IdStr:      Str=pFB->Eqn() ? pFB->Eqn()->EqpIdStr() : ""; EI.Fld->fEditable=false; break;
            case Id_Location:   Str=pFB->Eqn() ? pFB->Eqn()->EqpLocation() : ""; EI.Fld->fEditable=false; break;
            case Id_Diam:       DBLEDT(pFB->Diam());              break;
            //case Id_Beta:       DBLEDT(D);                        break;
            case Id_dZ:         DBLEDT(FBSgn(b)*pFB->Rise());     break;
            case Id_Z:          DBL   ((b&1)? pFB->DatumNode() : pFB->DatumFlng()); break;
            case Id_RqdLength:  DBLEDT(pFB->RqdLength());         break;
            case Id_ActLength:  DBL   (pFB->ActLength());         break;
            case Id_KFact:      DBLEDT(pFB->GetKFact());          break;
            case Id_FricFact:   DBL   (pFB->GetFricFact());       break;
            case Id_Velocity:   DBL   (FBSgn(b)*pFB->Velocity()); break;
            case Id_dPFric:     DBL   (FBSgn(b)*pFB->DPq());      break;
            case Id_dPAcc:      DBL   (FBSgn(b)*pFB->DPa());      break;
            case Id_dPStat:     DBL   (FBSgn(b)*pFB->DPz());      break;
            //case Id_dPStatX:    DBL   (FBSgn(b)*pFB->DPzX());     break;
            case Id_dP:         DBL   (FBSgn(b)*pFB->DPtot());    break;
            //case Id_Press:      DBL   (FBSgn(b)>0 ? pFB->NodePress() : pFB->PFlng ? pFB->FlngPress() : dNAN); break;
            case Id_Press:      DBL   (FBSgn(b)>0 ? (pFB->m_pNdPB ? pFB->NodePress() : dNAN) : pFB->m_pFlPB ? pFB->FlngPress() : dNAN); break;
            case Id_QmV:        DBL   (FBSgn(b)*pFB->GetQm()*pFB->LiqVolFrac(NULL)); break;
            case Id_QmL:        DBL   (FBSgn(b)*pFB->GetQm()*pFB->VapVolFrac(NULL)); break;
            case Id_Vfrac:      DBL   (pFB->VapVolFrac(NULL));    break;
            case Id_Slip:       DBL   (pFB->SlipRatio());         break;
            case Id_Void:       DBL   (pFB->VoidFraction());      break;
            case Id_OnePh:      DBL   (pFB->OnePhPart());         break;
            case Id_Temp:       DBL   (pFB->TempOut());           break;
            default :           Str.Set(" ??? %i", EI.FieldId);   break;
            }
          }
        }
      iOff+=rMFB.FBCnt.Cnt[b];
      }
#undef DBL
#undef DBLEDT
  }

//---------------------------------------------------------------------------

long CMultiFlwBlkEdt::Parse(FxdEdtInfo &EI, Strng & Str)
  {
  long Fix=0;
  if (CurrentBlk(EI))
    {
    switch (EI.PageNo-iPg1)
      {
      case 0:
        switch (EI.FieldId)
          {
          case Id_ColShow:
          case Id_ColHide:
            {
            for (int i=1; i<CDs.GetSize(); i++)
              if (Str.XStrICmp(CDs[i].sTag)==0)
                {
                CDs[i].bOn=!CDs[i].bOn;
                View().DoRebuild();
                Fix=1;
                break;
                }
            break;
            }
          }
        break;
      case 1:
        {
        switch (EI.FieldId)
          {
          //case Id_GViscRqd:
          //  rMFB.Common.GVisc.dOvr=ViscCnv.Normal(SafeAtoF(Str, 0.001));
          //  break;
          //case Id_LViscRqd:
          //  rMFB.Common.LVisc.dOvr=ViscCnv.Normal(SafeAtoF(Str, 0.001));
          //  break;
          //case Id_GDensRqd:
          //  rMFB.Common.GDens.dOvr=DensCnv.Normal(SafeAtoF(Str, 0.001));
          //  break;
          //case Id_LDensRqd:
          //  rMFB.Common.LDens.dOvr=DensCnv.Normal(SafeAtoF(Str, 0.001));
          //  break;
          //case Id_STensRqd:
          //  rMFB.Common.STens.dOvr=STensCnv.Normal(SafeAtoF(Str, 0.001));
          //  break;
          case Id_Roughness:
            rMFB.Common.dRoughness=RoughCnv.Normal(SafeAtoF(Str, 5.00E-05));
            break;
          }
        }
        break;
      }
    }

//  if (CurrentBlk(EI))
//    {
//    int p=EI.PageNo-iPg1;
//    int i=(int)(EI.BlkRowNo-EI.Index);
//    switch (EI.FieldId)
//      {
//      case Id_ColShow:
//      case Id_ColHide:
//        {
//        for (int i=1; i<CDs.GetSize(); i++)
//          if (Str.XStrICmp(CDs[i].sTag)==0)
//            {
//            CDs[i].bOn=!CDs[i].bOn;
//            View().DoRebuild();
//            Fix=1;
//            break;
//            }
//        break;
//        }
//      }
//    }

  int iOff=0;
  for (int b=0; b<4; b++)
    if (rMFB.FBCnt.Cnt[b])
      {
      if (CurrentBlk(EI) && (EI.PageNo-iPg1)==0)
        {
        int i=(int)(EI.BlkRowNo-EI.Index);

        FlwBlk * pFB=FBPtr(iOff, i);
        int Col=EI.FieldId-FldBase;
        CnvAttribute &Cnv=CDs[Col].Cnv;
        switch (EI.FieldId)
          {
          case Id_Class:
            {
            pTagObjClass pC=pFB->Eqn()->Class()->FindGrpShortDesc(Str());
            if (pC)
              pFB->ChangeFlwEqn(pC);
            break;
            }
          case Id_Desc:       if (pFB->Eqn()) pFB->Eqn()->SetEqpDesc(Str() ? Str() : ""); break;
          case Id_IdStr:      if (pFB->Eqn()) pFB->Eqn()->SetEqpIdStr(Str() ? Str() : ""); break;
          case Id_Location:   if (pFB->Eqn()) pFB->Eqn()->SetEqpLocation(Str() ? Str() : ""); break;
          case Id_Diam:       pFB->SetDiam(Cnv.Normal(SafeAtoF(Str, 0.01)));  break;
//          case Id_Beta:       break;
          case Id_dZ:         pFB->SetRise(FBSgn(b)*Cnv.Normal(SafeAtoF(Str, 1.0))); break;
          case Id_RqdLength:  pFB->SetRqdLength(Cnv.Normal(SafeAtoF(Str, 1.0)));    break;
          case Id_ActLength:  pFB->SetActLength(Cnv.Normal(SafeAtoF(Str, 1.0)));    break;
          case Id_KFact:      pFB->SetKFact(Cnv.Normal(SafeAtoF(Str, 1.0)));  break;
//          case Id_Velocity:   //break;
//          case Id_dP:         break;
//          case Id_dPFric:     D=FBSgn(b)*pFB->DPq();        goto DoDbl;
//          case Id_dPAcc:      break;
//          case Id_dPStat:     D=FBSgn(b)*pFB->DPz();        goto DoDbl;
//          case Id_Press:      Str.Set("%i", EI.FieldId); break;
          default : break;
          }
        }
      iOff+=rMFB.FBCnt.Cnt[b];
      }

  return Fix;
  }

//---------------------------------------------------------------------------

long CMultiFlwBlkEdt::ButtonPushed(FxdEdtInfo &EI, Strng & Str)
  {
  long Fix=0; //set Fix=1 to redraw graph
  if (CurrentBlk(EI))
    {

    }

//  if (CurrentBlk(EI))
//    {
//    }

  int iOff=0;
  for (int b=0; b<4; b++)
    if (rMFB.FBCnt.Cnt[b])
      {
      if (CurrentBlk(EI) && (EI.PageNo-iPg1)==0)
        {
        int i=(int)(EI.BlkRowNo-EI.Index);

        FlwBlk * pFB=FBPtr(iOff, i);
        int Col=EI.FieldId-FldBase;
//        switch (EI.FieldId)
//          {
//          case Id_Add:
//          case Id_Sub:
//            rMFB.ChgFBs(b, iOff, EI.FieldId==Id_Add ? +1 : -1);
//            View().DoRebuild();
//            break;
//          }
        }
      iOff+=rMFB.FBCnt.Cnt[b];
      }
  return Fix;
  }

//---------------------------------------------------------------------------

flag CMultiFlwBlkEdt::DoLButtonDown(UINT nFlags, CPoint point)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag CMultiFlwBlkEdt::DoLButtonUp(UINT nFlags, CPoint point)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag CMultiFlwBlkEdt::DoLButtonDblClk(UINT nFlags, CPoint point)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag CMultiFlwBlkEdt::DoRButtonDown(UINT nFlags, CPoint point)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag CMultiFlwBlkEdt::DoRButtonUp(UINT nFlags, CPoint point)
  {
  FxdEdtView &Vw=View();
  flag ret=false;
  if (Vw.CPgNo>=0)
    {
    FxdEdtInfo EI;
    if (Vw.LocateFromCR((int)Vw.ChEditPos.x, (int)Vw.ChEditPos.y, EI))
      {
      ret=true;
      if (EI.FieldId>=Id_First && EI.FieldId<=Id_Last)
        {
        //flag TagOnly=false;
        int iOff=0;
        for (int b=0; b<4; b++)
          {
          if (EI.BlkId==rMFB.FBEdtBlkId[b])
            break;
          iOff+=rMFB.FBCnt.Cnt[b];
          }

        int i=(int)(EI.BlkRowNo-EI.Index);
//        FlwBlk * pFB=FBPtr(iOff, i);
//        ChgFBs(

        CRect WRect;
        Vw.GetWindowRect(&WRect);
        CPoint  RBPoint;
        RBPoint.x = WRect.left+point.x;
        RBPoint.y = WRect.top+point.y;

        CMenu Menu;
        Menu.CreatePopupMenu();
//        CMenu FormatMenu;
//        FormatMenu.CreatePopupMenu();
//        if (pWrkFmt)
//          pWrkFmt->AddToMenu(FormatMenu);//, "");
//        CMenu CnvMenu;
//        CnvMenu.CreatePopupMenu();
//        if (pWrkCnv)
//          pWrkCnv->AddToMenu(CnvMenu);//, d.iCnv, d.sCnvTxt());

//        Menu.AppendMenu(MF_POPUP, (unsigned int)CnvMenu.m_hMenu, "&Conversions");
//        if ((pWrkCnv && pWrkCnv->Index()<=0) || TagOnly)
//          Menu.EnableMenuItem(0, MF_BYPOSITION|MF_GRAYED);
//
//        Menu.AppendMenu(MF_POPUP, (unsigned int)FormatMenu.m_hMenu, "&Format");
//        if (TagOnly)
//          Menu.EnableMenuItem(1, MF_BYPOSITION|MF_GRAYED);

//        Menu.AppendMenu(MF_SEPARATOR);
//        Menu.AppendMenu(MF_STRING, pView->iCmdCopyTag, "Copy Full &Tag");
//        if (EI.Fld->Tag==NULL)
//          Menu.EnableMenuItem(pView->iCmdCopyTag, MF_BYCOMMAND|MF_GRAYED);
//        Menu.AppendMenu(MF_STRING, pView->iCmdCopyVal, "Copy &Value");

        Menu.AppendMenu(MF_STRING, 100, "&Insert Element");
        Menu.AppendMenu(MF_STRING, 101, "&Delete Element");
        Menu.AppendMenu(MF_STRING, 102, "&Select Element");
        Menu.AppendMenu(MF_SEPARATOR);
        Menu.AppendMenu(MF_STRING, 103, "&Dump PDS Result");

//        Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, RBPoint.x, RBPoint.y, &View());
//        Menu.DestroyMenu();

        int RetCd=Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_RETURNCMD,
            RBPoint.x, RBPoint.y, &View());
        Menu.DestroyMenu();
        switch (RetCd)
          {
          case 100:
          case 101:
            rMFB.ChgFBs(b, i, RetCd==100 ? +1 : -1);
            View().DoRebuild();
            break;
          case 102:
            {
            char * DataFile="$BaseCfgFiles\\PipeFits.MDB";
            char* Table="InLine";
            char* Columns[]={"A", "B", "C"};
            CStringArray RetValues;
            if (SelectValues(DataFile, Table, Columns, RetValues))
              {
              int xxx=0;
              }
            }
            break;
          case 103:
            {
            CSCDFileDialog Dlg(FALSE, NULL, "*.PDS.csv", OFN_HIDEREADONLY, "PDS Result File (*.csv)|*.csv||", &View());
            Dlg.m_ofn.lpstrInitialDir = PrjFiles();
            Dlg.m_ofn.lpstrTitle = "PDS Results";
            if (Dlg.DoModal()==IDOK)
              {
              Strng Fn;
              Fn = Dlg.GetPathName();
              DumpPDS(Fn());
              }
            }
            break;
          }

//        switch (EI.FieldId)
//          {
//          case Id_DisplayT:
//            pWrkFmt=&TFmt;
//            pWrkCnv=&TCnv;
//            break;
//          case Id_DisplayP:
//            pWrkFmt=&PFmt;
//            pWrkCnv=&PCnv;
//            break;
//          case Id_MolWt:
//            TagOnly=true;
//            break;
//          case Id_Hf:
//          case Id_H:
//            pWrkFmt=&HFmt;
//            pWrkCnv=&HCnv;
//            break;
//          case Id_Cp:
//            pWrkFmt=&CpFmt;
//            pWrkCnv=&CpCnv;
//            break;
//          case Id_Rho:
//            pWrkFmt=&RhoFmt;
//            pWrkCnv=&RhoCnv;
//            break;
//          case Id_Vp:
//            pWrkFmt=&VpFmt;
//            pWrkCnv=&VpCnv;
//            break;
//          default:
//            pWrkFmt=NULL;
//            pWrkCnv=NULL;
//          }
//
//        if (pWrkFmt || TagOnly)
//          {
//          CRect WRect;
//          Vw.GetWindowRect(&WRect);
//          CPoint  RBPoint;
//          RBPoint.x = WRect.left+point.x;
//          RBPoint.y = WRect.top+point.y;
//
//          CMenu Menu;
//          Menu.CreatePopupMenu();
//          CMenu FormatMenu;
//          FormatMenu.CreatePopupMenu();
//          if (pWrkFmt)
//            pWrkFmt->AddToMenu(FormatMenu);//, "");
//          CMenu CnvMenu;
//          CnvMenu.CreatePopupMenu();
//          if (pWrkCnv)
//            pWrkCnv->AddToMenu(CnvMenu);//, d.iCnv, d.sCnvTxt());
//
//          Menu.AppendMenu(MF_POPUP, (unsigned int)CnvMenu.m_hMenu, "&Conversions");
//          if ((pWrkCnv && pWrkCnv->Index()<=0) || TagOnly)
//            Menu.EnableMenuItem(0, MF_BYPOSITION|MF_GRAYED);
//
//          Menu.AppendMenu(MF_POPUP, (unsigned int)FormatMenu.m_hMenu, "&Format");
//          if (TagOnly)
//            Menu.EnableMenuItem(1, MF_BYPOSITION|MF_GRAYED);
//
//          Menu.AppendMenu(MF_SEPARATOR);
//          Menu.AppendMenu(MF_STRING, pView->iCmdCopyTag, "Copy Full &Tag");
//          if (EI.Fld->Tag==NULL)
//            Menu.EnableMenuItem(pView->iCmdCopyTag, MF_BYCOMMAND|MF_GRAYED);
//          Menu.AppendMenu(MF_STRING, pView->iCmdCopyVal, "Copy &Value");
//
//          Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, RBPoint.x, RBPoint.y, &View());
//          Menu.DestroyMenu();
//          }

        }
      }
    }
  return ret;
  }

//---------------------------------------------------------------------------

flag CMultiFlwBlkEdt::DoRButtonDblClk(UINT nFlags, CPoint point)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag CMultiFlwBlkEdt::DoAccCnv(UINT Id)
  {
//  pCDataCnv pC=Cnvs[pWrkCnv->Index()];
//  for (int i=Id; i>0; i--)
//    pC=pC->Next();
//
//  if (pC)
//    {
//    pWrkCnv->SetText(pC->Txt());
//    }
  View().DoRebuild();
  return true;
  };

//---------------------------------------------------------------------------

flag CMultiFlwBlkEdt::DoAccFmt(UINT Id)
  {
//  for (UINT i=0; i<(UINT)DefinedFmts.GetSize(); i++)
//    if (i==Id)
//      break;
//
//  if (i<(UINT)DefinedFmts.GetSize())
//    {
//    *pWrkFmt=DefinedFmts[i];
//    View().DoRebuild();
//    }
  return true;
  };

//---------------------------------------------------------------------------

flag CMultiFlwBlkEdt::DoAccRptTagLists()
  {
  return FxdEdtBookRef::DoAccRptTagLists();
  };

//---------------------------------------------------------------------------

flag CMultiFlwBlkEdt::SelectValues(char* DataFile, char* Table, char** Columns, CStringArray &RetValues)
  {
  flag Ok=false;

  Strng DF;
  DF.FnExpand(DataFile);
  CFileStatus State;
  if (CFile::GetStatus(DF(), State))
    {
    CDBHelper DB(DF(), Table);
    if (DB.Open(DBConnect_Default, Columns[0], DBH_HasHeadings|DBH_ReadOnly|DBH_ForwardOnly|DBH_TrimStrings))
      {
      if (DB.CheckFieldNames(Columns))
        {
        CStringArray Values;
        while (!DB.IsEOF())
          {
          DB.GetNextRow(Values);

//          Strng SymBuff;
//          const char* pTag = (const char*)Values[0];
//          const char* pSym = CheckSymbolName((const char*)Values[1], SymBuff);
//
          Ok=true;
          }
        }
      else
        LogError("MultiFB", 0, "Incorrect field names: Table '%s' in '%s'", Table, DataFile);
      DB.Close();
      }
    else
      LogError("MultiFB", 0, "Unable to open Table '%s' in '%s'", Table, DataFile);
    }
  else
    LogError("MultiFB", 0, "Unable to open Database File '%s'", DataFile);
  return Ok;
  }

#endif

#undef FBPtr
#undef FBSgn

//---------------------------------------------------------------------------

//#define FBPtr(i)  rMFB.FBA[i].pFB;

const int NumPDSVars  = 20;
const int NMaxColours = 16;

double GetAPDSVal(int What, int b, FlwBlk * pFB)
  {
  int FBSgn=(((b)&1)==0 ? -1 : 1);
  switch (What)
    {
    case  0: return pFB->Diam();
    case  1: return FBSgn*pFB->Rise();
    case  2: return (b&1)? pFB->DatumNode() : pFB->DatumFlng();
    case  3: return pFB->RqdLength();
    case  4: return pFB->ActLength();
    case  5: return pFB->GetKFact();
    case  6: return pFB->GetFricFact();
    case  7: return FBSgn*pFB->Velocity();
    case  8: return FBSgn*pFB->DPq();
    case  9: return FBSgn*pFB->DPa();
    case 10: return FBSgn*pFB->DPz();
    case 11: return FBSgn*pFB->DPtot();
    case 12: return FBSgn>0 ? pFB->NodePress() : pFB->FlngPress();
//    case 13: return FBSgn*pFB->GetQmV();
//    case 14: return FBSgn*pFB->GetQmL();
//    case 15: return pFB->VapFrac();
    case 16: return pFB->SlipRatio();
    case 17: return pFB->VoidFraction();
    case 18: return pFB->OnePhPart();
    case 19: return pFB->TempOut();
    }
  return dNAN;
  }

static const char* PDSName[NumPDSVars] =
  {
  //"Desc",
  //"Equip",
  "Diam",
  "Rise",
  "Datum",
  "RqdLen",
  "ActLen",
  "KFact",
  "FricFact",
  "Velocity",
  "DPq",
  "DPa",
  "DPz",
  "DPtot",
  "Press",
  "QmV",
  "QmL",
  "VapFrac",
  "SlipRatio",
  "VoidFraction",
  "OnePhPart",
  "TempOut",
  };

//---------------------------------------------------------------------------

flag CMultiFlwBlkEdt::DumpPDS(char * Fn)
  {
  FILE * h=fopen(Fn, "wt");
  if (h==NULL)
    {
    LogError("PDS Results", 0, "%s not opened", Fn);
    return false;
    }

  fprintf(h, "IdStr,Desc,Equip");
  for (int j=0; j<NumPDSVars; j++)
    fprintf(h, ",%s,%s.Col", PDSName[j], PDSName[j]);
  fprintf(h, "\n");

  int Cnt[NumPDSVars], Col[NumPDSVars];
  double Vn[NumPDSVars], Vx[NumPDSVars];
  Strng S[30];
  for (j=0; j<NumPDSVars; j++)
    {
    Cnt[j]=0;
    Vx[j]=1;
    Vn[j]=0;
    }
  int i=0;
  for (int b=0; b<4; b++)
    {
    for (int ii=0; ii<rMFB.FBCnt.Cnt[b]; ii++)
      {
      // Skip Entry Exit
      if (i>0 && i<rMFB.FBA.GetSize()-1)
        {
        FlwBlk * pFB=rMFB.FBA[i].pFB;
        for (int j=0; j<NumPDSVars; j++)
          {
          double D=GetAPDSVal(j, b, pFB);
          if (Valid(D))
            {
            if (Cnt[j]==0)
              {
              Vn[j]=D;
              Vx[j]=D;
              }
            else
              {
              Vn[j]=Min(Vn[j],D);
              Vx[j]=Max(Vx[j],D);
              }
            Cnt[j]++;
            }
          }
        }
      i++;
      }
    }
  i=0;
  for (b=0; b<4; b++)
    {
    for (int ii=0; ii<rMFB.FBCnt.Cnt[b]; ii++)
      {
      // Skip Entry Exit
      if (i>0 && i<rMFB.FBA.GetSize()-1)
        {
        FlwBlk * pFB=rMFB.FBA[i].pFB;
        for (int j=0; j<NumPDSVars; j++)
          {
          double D=GetAPDSVal(j, b, pFB);
          if (Valid(D))
            {
            S[j].Set("%.15g", D);
            if (Vx[j]<=0.0)
              Col[j]=(int)((D-Vx[j])/LTZ(Vn[j]-Vx[j])*NMaxColours);
            else
              Col[j]=(int)((D-Vn[j])/GTZ(Vx[j]-Vn[j])*NMaxColours);
            }
          else
            {
            S[j]="*";
            Col[j]=0;
            }
          }

        fprintf(h, "\"%s\",\"%s\",\"%s\"",
              pFB->Eqn() ? pFB->Eqn()->EqpIdStr() : "",
              pFB->Eqn() ? pFB->Eqn()->ShortDesc() : "-",
              pFB->Eqn() ? pFB->Eqn()->EqpDesc() : "" );
        for (j=0; j<NumPDSVars; j++)
          fprintf(h, ",%s,%i",S[j](),Col[j]);
        fprintf(h, "\n");
        }
      i++;
      }
    }
  fclose(h);
  return true;
  };

//#undef FBPtr
//#undef FBSgn

//==========================================================================
//
//
//
//==========================================================================
const int MaxBisections=15;

MFCommonBlk::MFCommonBlk(CVLEBase *VLE)
  {
  fDoXfer     = false;
  fAppRhoH    = true;
  fFindRhoH   = false;
  fPressOK    = true;
  dMagn_Fac   = 1.0;
  dPowr_Fac   = 1.0;
  dAge_Fac    = 0.0;
  dRoughness  = 5.00E-05; // Commercial Steel

  LVisc       = 0.001;    // Water = 1     cP = 0.001    Ns/m^s @
  GVisc       = 0.000012; // Steam = 0.012 cP = 0.000012 Ns/m^s
  LDens       = 1000.0;
  GDens       = 1.0;
  pVLE        = VLE;
  FLog        = 1.0/log(10.0);

  DPAccl1 = DPAccl2 = DPAccl = 0.0;
  DPStat1 = DPStat2 = DPStat = 0.0;
  DPFric1 = DPFric2 = DPFric = 0.0;

  DPScale       = 1.0;
  PIn=POut      = Std_P;
  VfIn=VfOut    = 0.0;
  VFracOK       = false;
  dOnePhDPQ     = 0.0;
  dOnePhDPZ     = 0.0;

  dIntDiamMult  = 10.0;

  iVoidMeth     = VM_Butterworth;
  iVMButterFact = VMB_Baroczy;
  iDensMultMeth = DM_Friedel;
  };

double MFCommonBlk::FrictionFact(  double Diam, double Vel, double Dens)
  {
  double Re = Reynolds(Diam, Vel, Dens);
  double A = Pow((-2.457 * log(Pow(7.0/Re,0.9) + 0.27 * dRoughness/Diam)), 16);
  double B = Pow((37530.0/Re), 16);
  // Crane Eqn 1-4
  double F_Fac = 8 * Pow((Pow((8/Re), 12) + 1.0/Pow((A + B), 1.5)),0.0833);
  return F_Fac;
  }

//--------------------------------------------------------------------------

// Press Drop in kPa
double MFCommonBlk::PressDropLength(double Diam, double Vel, double Dens, double Length)
  {
  // VelocityHead     = Pow(Vel,2.0*dPowr_Fac)/(2.0 * g);
  // HeadLoss mLiquid = (F_Fac * Length / Diam) * VelocityHead * dMagn_Fac * AFac;
  // DP kPa           =  HeadLoss * Dens * g;

  Vel=Max(1.0e-3, fabs(Vel));
  double F_Fac=FrictionFact(Diam, Vel, Dens);
  double AFac = 1.0 + 0.03 * dAge_Fac;
  // Crane Eqn 1-4
  double DP = 0.001 * (F_Fac * Length / Diam) * Pow(Vel,2.0*dPowr_Fac)/2.0 * Dens * dMagn_Fac * AFac;
  return DP;
  }

//--------------------------------------------------------------------------

double MFCommonBlk::PressDropKFact(double Vel, double Dens, double KFact)
  {
  // VelocityHead      = Pow(Vel,2.0)/(2.0 * g);
  // HeadLoss mLiquid  = K * VelocityHead;
  // DP (Pa)           = HeadLoss * Dens * g;

  //double F_Fac=FrictionFactTurbulent(Diam, Dens);
  double K = KFact;// * F_Fac);
  double DP = K * Pow(Vel,2.0*dPowr_Fac)/2.0 * Dens;
  return DP * 0.001; // kPa
  }

//--------------------------------------------------------------------------

void MFCommonBlk::SetPhysInfo(double DI, double DE, double DO, double KMax, double Len, double Rise, double Scale)
  {
  DiamI=DI;
  DiamE=DE;
  DiamO=DO;
  //DiamMean=0.5*(Diam1+Diam2);
  AreaI=PI*Sqr(DiamI)/4.0;
  AreaE=PI*Sqr(DiamE)/4.0;
  AreaO=PI*Sqr(DiamO)/4.0;
  AreaMean=0.5*(AreaI+AreaO);
  DiamMean=Sqrt(4*AreaMean/PI);
  DZ=Rise;//*Scale;
  KFact=KMax;//*Scale;
  Length=Len;//*Scale;
  DPScale=Scale;
  };

//--------------------------------------------------------------------------

const double MinMFBVapFrac=0.0000000001;

//??? What Press
flag MFCommonBlk::SetFlowInfo(SpConduit & Cd, double RqdPress)
  {
  double LclX=Cd.MassFrac(som_Gas);
  X=Range(MinMFBVapFrac, LclX, 1.0-MinMFBVapFrac);
  VFracOK=(X==LclX);
  if (!VFracOK)
    X=X>0.5 ? 1.0 : 0.0;
//    return false;
  VFrac     = X;
  LFrac     = 1-X;//Cd.MassFrac(som_SL);
  //X         = LclX;
  Tk        = Cd.Temp();
  MassFlow  = Cd.QMass();
  GDens     = Max(1.0e-6, Cd.Rho(som_Gas));//, Tk, RqdPress);
  LDens     = Max(1.0e-3, Cd.Rho(som_SL));//, Tk, RqdPress);
  G         = MassFlow/AreaMean;
  VelMix    = Cd.QVolume()/AreaE;
  GVisc     = Cd.DynamicViscosity(som_Gas);
  LVisc     = Cd.DynamicViscosity(som_SL);
  STens     = Cd.SurfaceTension(som_Liq);

  /*
  double Tc = K_2_C(Tk);
  // ********** Vapour viscosity in mPa.s = cP
  if (Tc < 250)
    GVisc.dVal = 0.00003 * Tc + 0.0089;
  else if (Tc >= 250 && Tc < 310)
    GVisc.dVal = 0.00004 * Tc + 0.0062;
  else if (Tc >= 310)
    GVisc.dVal = 0.000003 * Sqr(Tc) - 0.0017 * Tc + 0.2823;
  GVisc.dVal *= 0.001;  // >> Pa.s

  // ********** Water viscosity in mPa.s = cP
  if (Tc > 0.01 && Tc < 100)
    LVisc.dVal = 1.4851 * Exps(-0.018 * Tc);
  else if (Tc >= 100 && Tc < 200)
    LVisc.dVal = 0.5629 * Exps(-0.0073 * Tc);
  else if (Tc >= 200 && Tc < 370)
    LVisc.dVal = -0.0004 * Tc + 0.2188;
  LVisc.dVal = 0.11; // Hard Wired;
  LVisc.dVal *= 0.001; // >> Pa.s

  // ********** SurfaceTension tension of water in N/m
  //KELV = Tc + 273.15
  double C10 = 0.2358 * Pow((647.15 - Tk) / 647.15, 1.256);
  double C11 = 1 - 0.625 * (647.15 - Tk) / 647.15;
  STens.dVal = C10 * C11;
*/
  WeLiq = Sqr(G) * DiamMean / (LDens * STens);
  ReLiq = G * DiamMean / LVisc;
  //FLC = Sqr(FLog * log(K / (3700 * DiamMean) + 5.74 / Pow(ReLiq, 0.9)));
  //LFricFact = 4 * 0.0625 / FLC;
  ReGas = G * DiamMean / GVisc;

  SetVoidFrac();

  TPDens = (1 - VoidFrac)*LDens + VoidFrac*GDens;
//  Slip = Max(1.0, X * LDens * (1 - VoidFrac) / (VoidFrac * (1 - X) * GDens));
  if (X>1.0e-10)
    Slip = Max(1.0, X * LDens * (1 - VoidFrac) / (VoidFrac * (1 - X) * GDens));
  else
    Slip = 1.0;

  //dbgpln("Slip:%10.5f %15.6e %15.6f %15.6f %15.6e %10.3f", Slip, X, LDens/1000, GDens/1000, VoidFrac, RqdPress);
  // Critical Mass Flux
  if (X>1.0e-10)
    {
    double Gamma=1.3;
    double Chi=Pow(2/(Gamma+1), Gamma/(Gamma-1));
    double VolCR=(1-X)/LDens+X*Pow(Chi, -1/Gamma)/GDens;
    double Flux1=(2*X*RqdPress/(0.001*GDens))*(Gamma/(Gamma-1));
    double Flux2=1-2/(Gamma+1);
    double FluxCr=(1/VolCR)*Sqrt(fabs(Flux1*Flux2));
    double VapVel=316.2*Sqrt(Gamma*RqdPress/(100*GDens));
    double TPVel=GDens*VapVel/(TPDens*Sqrt(X*Gamma));

    double Flux=MassFlow/AreaO;
    if (VelMix>TPVel && Flux>1.3*FluxCr)
      {
      dbgpln("Choked VelR:%10.2f FluxR:%10.2f", VelMix/GTZ(TPVel), Flux/GTZ(1.3*FluxCr));
      int yyy=0;
      }
    int xxx=0 ;

    }

  return VFracOK;
  };

//--------------------------------------------------------------------------

void MFCommonBlk::SetFricFact()
  {
  double FLC = Sqr(FLog * log(dRoughness / (3.700 * DiamMean) + 5.74 / Pow(ReLiq, 0.9)));
  double FGC = Sqr(FLog * log(dRoughness / (3.700 * DiamMean) + 5.74 / Pow(ReGas, 0.9)));
  LFricFact = 4 * 0.0625 / FLC;
  GFricFact = 4 * 0.0625 / FGC;
  };

//--------------------------------------------------------------------------

void MFCommonBlk::SetVoidFrac()
  {
  if (!VFracOK)
    {
    VoidFrac = 0.0;
    Slip     = 1;
    return;
    }

  switch (iVoidMeth)
    {
    case VM_Butterworth:
      {
      // B U T T E R W O R T H (1975)   C O R R E L A T I O N
      struct Lcl { double BB, N1, N2, N3; };
      static Lcl Factors[8]=
        {
          {   1,    1,   1./2,    0},  // ..Fauske (1962)
          {   1,    1,   0.67,    0},  // ..Zivi (1964)
          {   1, 0.74,   0.65, 0.13},  // ..Baroczy (1965)
          {   1, 0.72,    0.4, 0.08},  // ..Wallis (1965)
          {0.28, 0.64,   0.36, 0.07},  // ..Lockhart & Martinelli (1949)
          {   1,    1,   0.89, 0.18},  // ..Thom (1964)
          {   1,    1,   2./3,    0},  // ..Moody (1965)
          {   1,    1,      1,    0}   // ..Homogeneous
        };
      Lcl &Fct =Factors[iVMButterFact];

      //double ZETA = Pow((1 - X) / X, Fct.N1) * Pow(GDens / LDens, Fct.N2) * Pow(LVisc / GVisc, Fct.N3);
      //VoidFrac = 1 / (1 + Fct.BB * ZETA);
      double f1 = Pow((1 - X) / X, Fct.N1);
      double f2 = Pow(GDens / LDens, Fct.N2);
      double f3 = Pow(LVisc / GVisc, Fct.N3);
      VoidFrac = 1 / (1 + Fct.BB * f1*f2*f3);

      //Debug.Print "Butterworth VoidFrac"; VoidFrac
      break;
      }
    case VM_Premoli:
      {
      // P R E M O L I   E T   A L (1971)   C O R R E L A T I O N
      double BETA = X * LDens / (X * LDens + (1 - X) * GDens);
      double YFf = BETA / (1 - BETA);
      double E1 = 1.578 * Pow(ReLiq, -0.19) * Pow(LDens / GDens, 0.22);
      double E2 = 0.0273 * WeLiq * Pow(ReLiq, -0.51) * Pow(LDens / GDens, -0.08);
      Slip = 1 + E1 * Pow(fabs(YFf / (1 + YFf * E2) - YFf * E2), 0.5);
      VoidFrac = X / (X + Slip * (1 - X) * GDens / LDens);
      //Debug.Print "Premoli VoidFrac"; VoidFrac; " Slip"; Slip
      }
    }

  //double TPDensx=(1-VoidFrac)*LDens+VoidFrac*GDens;
  //int xxx=0;
  }

//--------------------------------------------------------------------------

void MFCommonBlk::SetDensityMult()
  {
  if (!VFracOK)
    {
    Phi=1.0;
    return;
    }

  switch (iDensMultMeth)
    {
    case DM_Friedel:
      {
      // ====================================================================
      //               T W O - P H A S E   M U L T I P L I E R S
      //          FRIEDEL (1979) USED A DATA BASE OF 25,000 POINTS TO
      //        DEVELOP THE FOLLOWING CORRELETION WITHIN +/- 22% ERROR.
      // ====================================================================

      double TPDens = 1 / (X / GDens + (1 - X) / LDens);
      double CF1 = Sqr(1 - X) + Sqr(X) * (LDens / GDens) * (GFricFact / LFricFact);
      double CF2 = Pow(X, 0.78) * Pow(1 - X, 0.24) * Pow(LDens / GDens, 0.91);
      double CF3 = Pow(GVisc / LVisc, 0.19) * Pow(1 - GVisc / LVisc, 0.7);
      double FRTP = Sqr(G) / (9.81 * DiamMean * Sqr(TPDens));
      double WETP = Sqr(G) * DiamMean / (TPDens * STens);
      Phi = CF1 + 2.025 * 3.24 * CF2 * CF3 / (Pow(FRTP, 0.045) * Pow(WETP, 0.035));
      break;
      }
    case DM_Beattie:
      {
      // ====================================================================
      //               T W O - P H A S E   M U L T I P L I E R S
      //               BASED ON THE FLOW REGIME MAP BEATTIE (1973)
      //          PROPOSED DIFFERENT VALUES OF THE TWO-PHASE MULTIPLIER.
      // ====================================================================
      double MULT1 = Pow(1 + X * (LDens / GDens - 1), 0.8);
      double MULT2 = (3.5 * GVisc + 2 * LVisc) * LDens / ((GVisc + LVisc) * GDens) - 1;
      double MULT3 = Pow((1 + X * (3.5 * LDens / GDens - 1)), 0.2);
      double MULT4 = Pow((1 + X * (GVisc * LDens / (LVisc * GDens) - 1)), 0.2);
      double MULT5 = Pow((1 + X * (LDens / GDens - 1)), 1.8);
      double MULT6 = Pow((GVisc / LVisc), 0.2);
      double MULT7 = Pow((GDens / LDens), 0.8);
      if (VoidFrac < 0.3)
        Phi = MULT1 * Pow(1 + X * MULT2, 0.2);
      else if (VoidFrac < 0.8)
        Phi = MULT1 * MULT3;
      else if (VoidFrac < 0.95)
        Phi = MULT1 * MULT4;
      else // (VoidFrac >= 0.95)
        Phi = MULT7 * MULT6 * MULT5;
      break;
      }
    }
  }

//--------------------------------------------------------------------------

void MFCommonBlk::SetProperties(SpConduit & Cd, double P, double MaxT, flag DoDbgHdr)
  {
  /*
  if (0)
    {
    for (int yy=0; yy<3; yy++)
      {
      SpConduit &W=*GetWrkConduit("SPW");//, L, S;
      W.QCopy(Cd);
      switch (yy)
        {
        case 0: W.ChangeModel("Steam/Water"); break;
        case 1: W.ChangeModel("Kaiser");      break;
        case 2: W.ChangeModel("Worsley");     break;
        case 3: W.ChangeModel("QAL");         break;
        }
      double t = W.Temp();
      double p = W.SaturationP(t);


      dbgpln("Mdl:%s", W.Model()->ClassId());
      dbgpln("X(%)\tT\tPsat\tMass SL\tMass \tH SL\tH Vap\tH Tot");
      for (double x=0.0; x<1.0; x+=(x<0.1) ? 0.01 : 0.05)
        {
        pVLE->SetFlashVapFrac(*(W.Model()), t, p, x, 0);

        double h1 = W.totHf(som_SL, t, p, W.pMArray());
        double h2 = W.totHf(som_Gas, t, p, W.pMArray());
        double h3 = W.totHf(som_ALL, t, p, W.pMArray());
        dbgpln("%16.6f\t%16.6f\t%16.6f\t%16.6f\t%16.6f\t%16.6f\t%16.6f\t%16.6f",
                 x*100,K_2_C(t),p,W.QMass(som_SL),W.QMass(som_Gas),h1/1000,h2/1000,h3/1000);
        }
      }
    }
  */
  int dodbg=0;
  pVLE->PFlash(Cd, P, 0.0, VLEF_SaturationLimit);

  if (MaxT>0 && Cd.Temp()>MaxT)
    Cd.SetTemp(MaxT);

  flag OK=SetFlowInfo(Cd, P);

  SetFricFact();
  SetDensityMult();

  if (VoidFrac>0.005 && TPDens > 1000)
    {
    int xxx=0;
    }

  if (dodbg)
    {
    if (DoDbgHdr)
      dbgpln("%+30s : %+12s %+12s %+12s %+12s %+12s %+12s %+12s %+12s %+12s %+12s %+12s %+12s %+12s %+12s %+12s %+12s %+12s %+12s %+12s %+12s", "Props",
           "P", "Tk", "MassFlow", "G", "X", "GDens", "LDens", "TPDens", "SurfTens",
           "GVisc", "LVisc", "ReLiq", "ReGas", "WeLiq", "K", "LFricFact", "GFricFact", "Phi", "Slip", "Void");
    dbgpln("%+30s : %12.5g %12.5g %12.5g %12.5g %12.5g %12.5g %12.5g %12.5g %12.5g %12.5g %12.5g %12.5g %12.5g %12.5g %12.5g %12.5g %12.5g %12.5g %12.5g %12.5g", " ",
         P, Tk, MassFlow, G, X, GDens, LDens, TPDens, STens,
         GVisc, LVisc, ReLiq, ReGas, WeLiq, KFact, LFricFact, GFricFact, Phi, Slip, VoidFrac);
    }
  }

//--------------------------------------------------------------------------

void MFCommonBlk::SetTwoPhDPs1(SpConduit & Cd, double P1)
  {
  SetProperties(Cd, P1, -1, true);

  double G1    = MassFlow/AreaI;
  //CHECK Which form to use ? Sum or Individual
  //DPFricLiq1=(KFact+LFricFact*Length/Diam1)*Sqr(G1)/(2*LDens);
  DPFricLiq1=0.0;
  if (KFact>0.0)
    DPFricLiq1=(KFact)*Sqr(G1)/(2*LDens);
  else
    DPFricLiq1=(LFricFact*Length/DiamI)*Sqr(G1)/(2*LDens);
  DPFric1=-0.001*Phi*DPFricLiq1;

  ASSERT_ALWAYS(Valid(DPFric1) && _finite(DPFric1), "Bad Friction Value", __FILE__, __LINE__);

  DPStat1=-0.001*9.81*DZ*(VoidFrac*GDens+(1-VoidFrac)*LDens); // ?? What about sin(Theta)

  double Beta1 = 0.0;
  if (VoidFrac<1.0)
    Beta1+=Sqr(1-X)/(LDens*(1-VoidFrac));
  if (VoidFrac>0.0)
    Beta1+=Sqr(X)/(GDens*VoidFrac);
  DPAccl1 =-0.001*Sqr(G1)*Beta1;
  };

//--------------------------------------------------------------------------

void MFCommonBlk::SetTwoPhDPs2(SpConduit & Cd, double P2, double MaxTemp)
  {
  SetProperties(Cd, P2, MaxTemp, false);//true);

  double G1    = MassFlow/AreaI;
  double G2    = MassFlow/AreaO;
  DPFric2=-0.001*Phi*DPFricLiq1;
  DPStat2=-0.001*9.81*DZ*(VoidFrac*GDens+(1-VoidFrac)*LDens); // ?? What about sin(Theta)

  ASSERT_ALWAYS(Valid(DPFric1) && _finite(DPFric1), "Bad Friction Value", __FILE__, __LINE__);

  double Beta2 = 0.0;
  if (VoidFrac<1.0)
    Beta2+=Sqr(1-X)/(LDens*(1-VoidFrac));
  if (VoidFrac>0.0)
    Beta2+=Sqr(X)/(GDens*VoidFrac);

  //CHECK Which Flux to Use
  DPAccl2=-0.001*Sqr(G2)*Beta2;
//  DPAccl2=-0.001*Sqr(G1)*Beta2;
  };

//--------------------------------------------------------------------------

void MFCommonBlk::SetTwoPhDPs()
  {
//#pragma chCHECKIT(Option or what)
//  switch (0)
//    {
//    case 0:
//      // Mean Values
//      DPFric=0.5*(DPFric1+DPFric2);
//      DPStat=0.5*(DPStat1+DPStat2);
//      break;
//    case 1:
//      // Inputs Only
//      DPFric=DPFric1;
//      DPStat=DPStat1;
//      break;
//    case 2:
//      // Average
//      DPFric=DPFric2;
//      DPStat=DPStat2;
//      break;
//    }

//  DPFric*=DPScale;
//  DPStat*=DPScale;
//  DPAccl=(DPAccl2-DPAccl1);
  };

//--------------------------------------------------------------------------

flag MFCommonBlk::TestPressureOK(double &P0, double &P1)
  {
  const double MinMFBFlashP=10.0;
  if (P1<MinMFBFlashP)
    {
    //DPFric=-fabs(MassFlow);
    //DPAccl=0;
    //DPStat=0;
    //P1=P0+(DPFric+DPAccl+DPStat);
    fPressOK=false;
    }
  return fPressOK;
  }

//--------------------------------------------------------------------------

flag MFCommonBlk::DoFlashLiqAtStart(flag AtEntry, CFlwBlkBase & FE,
                                    double Regulation, SpConduit &Cd,
                                    double K, double DI, double DE, double DO)//, double D2)
  {
  // find flash start by bisection
  //double DE=FE.Diam();
  //double DI=FE.SrcFBOK() ? FE.SrcFB().Diam() : DE;
  //double DO=FE.DstFBOK() ? FE.DstFB().Diam() : DE;

  double OnePhPart0=0.0;
  double OnePhPart1=1.0;
  double PBubble=POut;
  double PNoBubble=POut;
  for (int iter=0; iter<MaxBisections; iter++)
    {
    dOnePhPart=0.5*(OnePhPart0+OnePhPart1);
    double P=FE.PAtFromEntry(dOnePhPart);
    Cd.Set_totHf(HIn);
    pVLE->PFlash(Cd, P, 0.0, VLEF_SaturationLimit);
    double Vf=Cd.MassFrac(som_Gas);
    double T=Cd.Temp();
    if (Vf>MinMFBVapFrac)
      {
      OnePhPart1=dOnePhPart;
      PBubble=P;
      }
    else
      {
      OnePhPart0=dOnePhPart;
      PNoBubble=P;
      }

    #if dbgMultiFB
    if (dbgDoFlash() && dbgShowIter())
      dbgpln(" %6.2f%%: %10.2f   %10.2f   %10.6f %14.6f ", dOnePhPart*100, P, K_2_C(T), Vf*100, Cd.totHf());
    #endif
    }

  // Ensure that Vapour Exists
  dOnePhPart=GTZ(OnePhPart1);
  double TwoPhPart=GTZ(1-dOnePhPart);
  double OnePhDP=PBubble-PIn;

  double IntegrateLen=Min(DI,DE)*dIntDiamMult;

  // Integrate down length of Pipe
  double TwoPhLen=FE.TotLength()*(1.0-dOnePhPart);
  double TwoPhRise=FE.Rise()*(1.0-dOnePhPart);
  int NSects=int(TwoPhLen/IntegrateLen)+1;
  double DiamDiff=(DO-DI);

  double DPFricTot=0.0;
  double DPAcclTot=0.0;
  double DPStatTot=0.0;
  double P0=PBubble;
  double P1=PBubble;

  double SectK=K*TwoPhPart/NSects;
  double SectLen=FE.TotLength()*TwoPhPart/NSects;
  double SectRise=FE.Rise()*TwoPhPart/NSects;

////#pragma chCHECKIT(???????????)
//  if (dOnePhPart<0.9999999999)
  for (int Sect=0; Sect<NSects && fPressOK; Sect++)
    {
    double SectD1=DI+DiamDiff*(dOnePhPart+(TwoPhPart)*Sect/NSects);
    double SectD2=DI+DiamDiff*(dOnePhPart+(TwoPhPart)*(Sect+1)/NSects);

    SetPhysInfo(SectD1, DE, SectD2, SectK, SectLen, FE.QmSign()*SectRise, 1.0);
    SetTwoPhDPs1(Cd, P0);
    DPFric=DPFric1*DPScale;
    DPStat=DPStat1*DPScale;
    int IterStart=500;
    double DPTot=(DPFric+DPStat);
    for (int Iter=IterStart;Iter;Iter--)
      {
      P1=P0+DPTot;
//      if (Iter==IterStart-1) // CNM
//        break;
      if (!TestPressureOK(P0, P1))
        break;

      SetTwoPhDPs2(Cd, 0.5*(P0+P1), -1);
      DPFric=DPFric2*DPScale;
      DPStat=DPStat2*DPScale;

      SetTwoPhDPs2(Cd, P1, -1);
      DPAccl=(DPAccl2-DPAccl1);

      double NewDPTot=DPFric+DPStat+DPAccl;

      flag OK=(fabs(NewDPTot-DPTot)<1.0e-4);
      #if dbgMultiFB
      if (dbgDoFlash() && (dbgShowIter()))
        dbgpln("%10s%5s %10.3f>%10.3f P:%10.3f(%10.5f) SatP:%10.3f(%10.5f) F:%10.3f S:%10.3f A:%10.3f(%10.3f-%10.3f) Phi:%10.3f Slip:%10.3f Void:%10.3f Vel:%10.3f G:%10.3f X:%10.3e GDe:%10.3f LDe:%10.3f TPDe:%10.3f ", " ",
          "", P0, P1,Cd.Press(),K_2_C(Cd.Temp()),
          Cd.SaturationP(Cd.Temp()),K_2_C(Cd.SaturationT(Cd.Press())),
          DPFric,DPStat,DPAccl,DPAccl2,DPAccl1,Phi,Slip,VoidFrac,VelMix, G,X, GDens, LDens, TPDens);
      #endif
      if (OK)
        break;
    //  DPTot=0.5*(NewDPTot+DPTot);
      DPTot=NewDPTot;
      }
    if (Iter==0)
      dbgpln("Iter Limit #################################");

    #if dbgMultiFB
    if (dbgDoFlash())
      {
      dbgpln("%5s %3i %5s %10.3f>%10.3f P:%10.3f(%10.5f) SatP:%10.3f(%10.5f) F:%10.3f S:%10.3f A:%10.3f(%10.3f-%10.3f) Phi:%10.3f Slip:%10.3f Void:%10.3f Vel:%10.3f G:%10.3f X:%10.3e GDe:%10.3f LDe:%10.3f TPDe:%10.3f ", " ",
        Sect, "Flash", P0, P1,Cd.Press(),K_2_C(Cd.Temp()),
        Cd.SaturationP(Cd.Temp()),K_2_C(Cd.SaturationT(Cd.Press())),
        DPFric,DPStat,DPAccl,DPAccl2,DPAccl1,Phi,Slip,VoidFrac,VelMix, G,X, GDens, LDens, TPDens);
      }
    #endif

    DPFricTot+=DPFric;
    DPAcclTot+=DPAccl;
    DPStatTot+=DPStat;
    P0+=(DPFric+DPAccl+DPStat);
    }

  POut=Range(10.0,P0, PBubble);
  #if dbgMultiFB
  if (dbgDoFlash())
    {
    double PSat=Cd.SaturationP(Cd.Temp());
    dbgpln("%10s%-10s %10.3f%% F:%10.3f S:%10.3f Full F:%10.3f S:%10.3f", " ",
           "OnePhase", dOnePhPart*100, dOnePhPart*FE.QmSign()*FE.DPq(),
           dOnePhPart*FE.QmSign()*FE.DPz(), FE.QmSign()*FE.DPq(),FE.QmSign()*FE.DPz());
    dbgpln("%10s%s", " ",
      "Flash          ----------------------------------------------------------------");
    dbgpln("%10s%-10s@ %10.3f F:%10.3f S:%10.3f Full F:%10.3f S:%10.3f", " ",
      "TwoPhase", POut, DPFricTot,DPStatTot,DPFricTot/TwoPhPart,DPStatTot/TwoPhPart);
    }
  #endif

  FE.SetVelocity(FE.QmSign()*MixVel());
  FE.SetDPq(FE.DPq()*dOnePhPart+FE.QmSign()*DPFricTot, 0.0);

  ASSERT_ALWAYS(Valid(FE.DPq()) && _finite(FE.DPq()), "Bad Friction Value", __FILE__, __LINE__);

  FE.SetDPa(FE.QmSign()*DPAcclTot, 0.0);
  if (fAppRhoH)
    FE.SetDPz(FE.DPz()*dOnePhPart+FE.QmSign()*DPStatTot);
  FE.SetDerivsBad(true);
  FE.SetSlipRatio(SlipRatio());
  FE.SetVoidFraction(VoidFraction());
  FE.SetOnePhPart(dOnePhPart);
  FE.SetTempOut(Cd.Temp());

  return true;
  }

//--------------------------------------------------------------------------

flag MFCommonBlk::DoFlashVapAtStart(flag AtEntry, CFlwBlkBase & FE,
                                    double Regulation, SpConduit &Cd,
                                    double K, double DI, double DE, double DO)//, double D2)
  {
  //double DE=FE.Diam();
  //double DI=FE.SrcFBOK() ? FE.SrcFB().Diam() : DE;
  //double DO=FE.DstFBOK() ? FE.DstFB().Diam() : DE;

  double IntegrateLen=Min(DI,DE)*dIntDiamMult;

  // Integrate down length of Pipe
  int NSects=int(FE.TotLength()/IntegrateLen)+1;
  double DiamDiff=(DO-DI)/NSects;

  double DPFricTot=0.0;
  double DPAcclTot=0.0;
  double DPStatTot=0.0;
  double P0=PIn;
  double P1=PIn;

  double SectK=K/NSects;
  double SectLen=FE.TotLength()/NSects;
  double SectRise=FE.Rise()/NSects;

  double TwoPhPart=1.0;
  double DPNBFric=0.0;
  double DPNBAccl=0.0;
  double DPNBStat=0.0;

  flag TwoPh=true;
  flag Condensing;
  for (int Sect=0; (Sect<NSects) && TwoPh && fPressOK; Sect++)
    {
    double SectD1=DI+DiamDiff*Sect;
    double SectD2=DI+DiamDiff*(Sect+1);

    double SectTwoPhPart=1.0;
    double SectTwoPhPart0=0.0;
    double SectTwoPhPart1=1.0;
    double PBubble=P0;
    double PNoBubble=P0;

    for (int PartIter=0; PartIter<MaxBisections; PartIter++)
      {

      SetPhysInfo(SectD1, DE, SectD2, SectK, SectLen, FE.QmSign()*SectRise, SectTwoPhPart);

      SetTwoPhDPs1(Cd, P0);
      DPFric=DPFric1*DPScale;
      DPStat=DPStat1*DPScale;

      double SectInTemp=Cd.Temp();
      // Test to see what is going to happen - Compression or Expansion
      Condensing=(DPStat+DPFric>0.0);
      if (Condensing)
        {
        // Implies that the Press will go up and gas is going to Condense
        // Entropy will be lost and temp will hold constant
        }
      else // (DPStat < DPFric)
        {
        // Implies that the Press will go down and Liquid will Flash
        }

      double DPTot=(DPFric+DPStat);
      int IterStart=500;
      for (int Iter=IterStart;Iter;Iter--)
        {
        P1=P0+DPTot;
        if (!TestPressureOK(P0, P1))
          break;

        SetTwoPhDPs2(Cd, 0.5*(P0+P1), -1);
        DPFric=DPFric2*DPScale;
        DPStat=DPStat2*DPScale;

        SetTwoPhDPs2(Cd, P1, -1);
        DPAccl=(DPAccl2-DPAccl1);

        double NewDPTot=DPFric+DPStat+DPAccl;

        flag OK=(fabs(NewDPTot-DPTot)<1.0e-4);
        #if dbgMultiFB
        if (dbgDoFlash() && (dbgShowIter()))
          dbgpln("%10s%5s %10.3f>%10.3f P:%10.3f(%10.5f) SatP:%10.3f(%10.5f) F:%10.3f S:%10.3f A:%10.3f(%10.3f-%10.3f) Phi:%10.3f Slip:%10.3f Void:%10.3f Vel:%10.3f G:%10.3f X:%10.3e GDe:%10.3f LDe:%10.3f TPDe:%10.3f ",
                 " ",(Condensing ? "Cond ":"Flash"),
                P0, P1,Cd.Press(),K_2_C(Cd.Temp()),
                Cd.SaturationP(Cd.Temp()),K_2_C(Cd.SaturationT(Cd.Press())),
                DPFric,DPStat,DPAccl,DPAccl2,DPAccl1,Phi,Slip,VoidFrac,VelMix, G,X, GDens, LDens, TPDens);
        #endif
        //if (fabs(NewDPTot)<fabs(DPTot))
        //  DPTot=0.25*NewDPTot+0.75*DPTot;
        //else
        //DPTot=NewDPTot;
        DPTot=0.5*(NewDPTot+DPTot);
        if (OK && Iter<IterStart-2)
          break;
        }
      if (Iter==0)
        dbgpln("Iter Limit #################################");

      if ((VFracOK && PartIter==0) || !fPressOK)
        break;

      if (VFracOK)
        {
        SectTwoPhPart0=SectTwoPhPart;
        PBubble=P1;
        }
      else
        {
        SectTwoPhPart1=SectTwoPhPart;
        PNoBubble=P1;
        DPNBFric=DPFric;
        DPNBAccl=DPAccl;
        DPNBStat=DPStat;
        }
      #if dbgMultiFB
      if (dbgDoFlash() && dbgShowIter())
        dbgpln(" %6.2f%%: %10.2f   %10.2f   %10.6f %14.6f ", SectTwoPhPart*100, POut, K_2_C(Cd.Temp()), X*100, Cd.totHf());
      #endif

      SectTwoPhPart=0.5*(SectTwoPhPart0+SectTwoPhPart1);
      }


    TwoPhPart=(Sect+SectTwoPhPart)/NSects;
    dOnePhPart=1.0-TwoPhPart;

    if (PartIter && DPNBFric!=0.0)
      {
      P1=PNoBubble;
      SetTwoPhDPs2(Cd, P1, -1);
      DPFric=DPNBFric;
      DPAccl=DPNBAccl;
      DPStat=DPNBStat;
      TwoPh=false;
      }
    else
      P1=P0+(DPFric+DPAccl+DPStat);
    TestPressureOK(P0, P1);

    DPFricTot+=DPFric;
    DPAcclTot+=DPAccl;
    DPStatTot+=DPStat;

    #if dbgMultiFB
    if (dbgDoFlash())
      dbgpln("%5s %3i %5s %10.3f>%10.3f P:%10.3f(%10.5f) SatP:%10.3f(%10.5f) F:%10.3f S:%10.3f A:%10.3f(%10.3f-%10.3f) Phi:%10.3f Slip:%10.3f Void:%10.3f Vel:%10.3f G:%10.3f X:%10.3e GDe:%10.3f LDe:%10.3f TPDe:%10.3f ",
             " ",Sect,(Condensing ? "Cond ":"Flash"),
              P0, P1,Cd.Press(),K_2_C(Cd.Temp()),
              Cd.SaturationP(Cd.Temp()),K_2_C(Cd.SaturationT(Cd.Press())),
              DPFric,DPStat,DPAccl,DPAccl2,DPAccl1,Phi,Slip,VoidFrac,VelMix, G,X, GDens, LDens, TPDens);
    #endif
    P0=P1;
    }

  POut=P0;

  #if dbgMultiFB
  if (dbgDoFlash())
    {
    dbgpln("%10s%-10s@ %10.3f F:%10.3f S:%10.3f Full F:%10.3f S:%10.3f", " ",
      "TwoPhase", POut, DPFricTot,DPStatTot,DPFricTot/TwoPhPart,DPStatTot/TwoPhPart);
    if (dOnePhPart>1.0e-20)
      {
      dbgpln("%10s%s", " ",
        "Condense       ----------------------------------------------------------------");
      dbgpln("%10s%-10s %10.3f%% F:%10.3f S:%10.3f Full F:%10.3f S:%10.3f", " ",
             "OnePhase", dOnePhPart*100, dOnePhPart*dOnePhDPQ,
             dOnePhPart*dOnePhDPZ, dOnePhDPQ,dOnePhDPZ);
      }
    }
  #endif

  FE.SetVelocity(FE.QmSign()*MixVel());
  FE.SetDPq(dOnePhDPQ*dOnePhPart+FE.QmSign()*DPFricTot, 0.0);

  ASSERT_ALWAYS(Valid(FE.DPq()) && _finite(FE.DPq()), "Bad Friction Value", __FILE__, __LINE__);

  FE.SetDPa(FE.QmSign()*DPAcclTot, 0.0);
  if (fAppRhoH)
    FE.SetDPz(dOnePhDPZ*dOnePhPart+FE.QmSign()*DPStatTot);
  FE.SetDerivsBad(true);
  FE.SetSlipRatio(SlipRatio());
  FE.SetVoidFraction(VoidFraction());
  FE.SetOnePhPart(dOnePhPart);
  FE.SetTempOut(Cd.Temp());
  return true;
  }

//--------------------------------------------------------------------------

flag MFCommonBlk::DoFlash(flag AtEntry, CFlwBlkBase & FE,
                          double Regulation, SpConduit &Cd,
                          double K, double DI, double DE, double DO,//double D2,
                          double OnePhDPQ, double OnePhDPZ)
  {
//  double DE=FE.Diam();
//  double DI=FE.SrcFBOK() ? FE.SrcFB().Diam() : DE;
//  double DO=FE.DstFBOK() ? FE.DstFB().Diam() : DE;

  dOnePhDPQ=OnePhDPQ;
  dOnePhDPZ=OnePhDPZ;

  flag Done=false;
  if (AtEntry)
    {
    //dPrevDiam=D2;
    fPressOK=true;
    }
  else if (!fPressOK)
    return Done;

  ASSERT(pVLE);
  VfIn=Cd.MassFrac(som_Gas);
  TIn=Cd.Temp();
  HIn=Cd.totHf();
  PIn =FE.QmSign() > 0 ?  FE.FlngPress() : FE.NodePress();
  POut=FE.PAtFromEntry(1.0);
  #if dbgMultiFB
  double PSat, TSat;//, PSat1;
  if (dbgDoFlash())
    {
    PSat=Cd.SaturationP(Cd.Temp());
    TSat=Cd.SaturationT(POut);
    //PSat1=Cd.SaturationP(TSat);
    //double DPTot=POut-PIn; // Initial Est
    }
  #endif
  pVLE->PFlash(Cd, POut, 0.0, VLEF_SaturationLimit);

  VfOut=Cd.MassFrac(som_Gas);

  FE.SetOnePhPart(1.0);
  FE.SetTempOut(Cd.Temp());

  #if dbgMultiFB
  if (dbgDoFlash())
    {
    double TOut=Cd.Temp();
    double HOut=Cd.totHf();
    dbgpln("------------------------------------------------------");
    dbgpln("  DI:%10.2f  DE:%10.2f  DO:%10.2f  Rise:%10.2f  Len:%10.2f K:%10.3f ", DI,DE,DO, FE.Rise(), FE.TotLength(), K);
    dbgpln("        : %+10s   %+10s   %+10s   Sat P/T %10.2f   %10.2f ", "P", "T", "Vf", PSat, K_2_C(TSat));
    dbgpln("  In    : %10.2f   %10.2f   %10.6f %14.6f ", PIn,  K_2_C(TIn),  VfIn*100, HIn);
    }
  #endif

  if (VfIn>MinMFBVapFrac || VfOut>MinMFBVapFrac)
    {
    if (VfIn<MinMFBVapFrac && VfOut>MinMFBVapFrac)
      Done=DoFlashLiqAtStart(/*How,*/ AtEntry, FE, Regulation, Cd, K, DI, DE, DO);//, D2);
    else
      Done=DoFlashVapAtStart(/*How,*/ AtEntry, FE, Regulation, Cd, K, DI, DE, DO);//, D2);
    //CHECK Flash End

    //pVLE->PFlash(Cd, POut, 0.0, VLEF_SaturationLimit);
    }
  else
    {
    #if dbgMultiFB
    if (dbgDoFlash())
      {
      //dbgpln("#############  I/O   P:%10.2f >> %10.2f   T:%10.2f %10.2f ", PIn, POut, TIn, TOut);
      }
    #endif
    }

  Cd.SetPress(POut);

  #if dbgMultiFB
  if (dbgDoFlash())
    {
    dbgpln("  Out   : %10.2f   %10.2f   %10.6f %14.6f ", POut, K_2_C(Cd.Temp()), Cd.MassFrac(som_Gas)*100, Cd.totHf());
    dbgpln("------------------------------------------------------");
    }
  #endif

  //dPrevDiam=D2;
  return Done;
  }

//==========================================================================
//
//
//
//==========================================================================

void MFB_Eqn::BuildDataDefn(DataDefnBlk & DDB)
  {
  //TaggedObject::BuildBaseDefn(DDB);
  // Abstract CFlwEqn::BuildDataDefn(DataDefnBlk & DDB)
  //DDB.String ("IdStr",       "",         DC_L   , "m",     &m_sIdStr, this  ,isParm|InitHidden);
//  DDB.Double ("ActLen",       "",         DC_L   , "m",     xidFlwEqnActLen     ,this  ,isParmStopped);
//  DDB.Double ("Diam",         "",         DC_L   , "mm",    xidFlwEqnDiam       ,this  ,isParm);
////  DDB.Double ("dPz",         "",         DC_DP   , "kPa",   &dPz, this  ,0);
////  DDB.Double ("dPq",         "",         DC_DP   , "kPa",   &dPq, this  ,0);
//int xx=0;
  };

//--------------------------------------------------------------------------

flag MFB_Eqn::DataXchg(DataChangeBlk & DCB)
  {
  if (CFlwEqn::DataXchg(DCB))
    return true;

  return false;
  };

//===========================================================================
//
//
//
//===========================================================================

// Must be defined for DEFINE_TAGOBJ
#define DllImportExport

DEFINE_TAGOBJ(MFB_Pipe);
class MFB_Pipe : public MFB_Eqn
  {
  public:
    MFB_Pipe(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~MFB_Pipe();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, bool On, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);
  };



IMPLEMENT_FLWEQN(MFB_Pipe, MFBPipeGroup.Name(), "MFBPipe", "", TOC_SYSTEM,
                  "Pipe",
                  "Pipe");

MFB_Pipe::MFB_Pipe(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MFB_Eqn(pClass_, pTag, pAttach, eAttach)
  {
  }

//--------------------------------------------------------------------------

MFB_Pipe::~MFB_Pipe()
  {
  }

//--------------------------------------------------------------------------

void MFB_Pipe::BuildDataDefn(DataDefnBlk & DDB)
  {
  MFB_Eqn::BuildDataDefn(DDB);
//  DDB.Double ("Resistance",   "Resistn",  DC_    , "",      &R_Fxd        ,this  ,isParm);


  BuildDataDefnOveride(DDB);
  };

//--------------------------------------------------------------------------

flag MFB_Pipe::DataXchg(DataChangeBlk & DCB)
  {
  return MFB_Eqn::DataXchg(DCB);
  };

//--------------------------------------------------------------------------

flag MFB_Pipe::ValidateData(ValidateDataBlk & VDB)
  {
  return MFB_Eqn::ValidateData(VDB);
  };

//--------------------------------------------------------------------------

//void MFB_Pipe::SetPhysData(double DZ)
//  {
////  D.dLength=Max(fabs(DZ), Len_Rqd);
////  D.dXLength=0.0;
////  D.dArea=Area;
//  };

//--------------------------------------------------------------------------

flag MFB_Pipe::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, bool On, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  double Rho=Max(0.001, FE.TwoPhCdPtr() ? FE.TwoPhCd().Rho(som_SL) : FE.MeanRho(pProps));

  double DqScl=1.001;
  double Vel=FE.SetVelMeasRange(Rho, FE.Area(), 0.001);

  //double FricFact=pMFCB->FrictionFact(FE.Diam(), Vel, Rho);
  double DP=pMFCB->PressDropLength(FE.Diam(), FE.VelMeas(), Rho, FE.TotLength());
  double DP1=pMFCB->PressDropLength(FE.Diam(), FE.VelMeas(DqScl), Rho, FE.TotLength());

  double OnePhDPQ=-FE.QmSign()*DP;
  double OnePhDPZ=/*FE.QmSign()**/-0.001*9.81*FE.Rise()*Rho;
  FE.SetVelocity(FE.QmSign()*Vel);
  FE.SetDPq(OnePhDPQ, -FE.QmSign()*(DP1-DP)/FE.DQmMeas(DqScl));
  FE.SetDPa(0.0, 0.0);
  FE.SetSlipRatio();
  FE.SetVoidFraction();
  FE.SetOnePhPart();

  FE.SetDPz(OnePhDPZ); // ?? What about sin(Theta)
  FE.SetDerivsBad(true);

  if (FE.TwoPhCdPtr())
    {
    double DE=FE.Diam();
    double DI=DE;
    double DO=DE;

    if (pMFCB->DoFlash(false, FE, 1.0, FE.TwoPhCd(), 0.0, DI, DE, DO, OnePhDPQ, OnePhDPZ))
      {
      };
    FE.SetVLFracs(NULL, FE.TwoPhCd().MassFrac(som_Gas), FE.TwoPhCd().MassFrac(som_Liq));
    FE.SetTempOut(FE.TwoPhCd().Temp());
    }

  ASSERT_ALWAYS(Valid(FE.DPq()) && _finite(FE.DPq()), "Bad Friction Value", __FILE__, __LINE__);
//  FE.SetMomentum(FE.MeanPress());
  return True;
  };

//===========================================================================
//
//
//
//===========================================================================

DEFINE_TAGOBJ(MFB_InLine);
class MFB_InLine : public MFB_Eqn
  {
  friend class CMultiFlwBlkEdt;
  friend class CMultiFlwBlk;
  protected:
    double  dK;
  public:
    MFB_InLine(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~MFB_InLine();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, bool On, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);

    virtual void   SetPhysData(double DZ) {};
    virtual void   SetKFact(double K)     { dK=K; };
    virtual double GetKFact()             { return dK; };

  };



IMPLEMENT_FLWEQN(MFB_InLine, MFBPipeGroup.Name(), "MFBInLn", "", TOC_SYSTEM,
                  "InLine",
                  "In Line");

MFB_InLine::MFB_InLine(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MFB_Eqn(pClass_, pTag, pAttach, eAttach)
  {
  dK=1.0;
  }

//--------------------------------------------------------------------------

MFB_InLine::~MFB_InLine()
  {
  }

//--------------------------------------------------------------------------

void MFB_InLine::BuildDataDefn(DataDefnBlk & DDB)
  {
  MFB_Eqn::BuildDataDefn(DDB);

  DDB.Double ("",   "K",  DC_    , "",      &dK,this  ,isParm);

  BuildDataDefnOveride(DDB);
  };

//--------------------------------------------------------------------------

flag MFB_InLine::DataXchg(DataChangeBlk & DCB)
  {
  return MFB_Eqn::DataXchg(DCB);
  };

//--------------------------------------------------------------------------

flag MFB_InLine::ValidateData(ValidateDataBlk & VDB)
  {
  return MFB_Eqn::ValidateData(VDB);
  };

//--------------------------------------------------------------------------

flag MFB_InLine::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, bool On, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  double Rho=Max(0.001, FE.TwoPhCdPtr() ? FE.TwoPhCd().Rho(som_SL) : FE.MeanRho(pProps));
  double DqScl=1.001;

//TODO A Big Kludge

  //double A=FE.Area();
  //if (A<0.03)
  //  A=Sqr(0.2477)*3.14159/4;
  //double Vel=FE.QmMeas()/(Rho*A);

  double Vel=FE.SetVelMeasRange(Rho, FE.Area(), 0.001);

  double DP=pMFCB->PressDropKFact(FE.VelMeas(), Rho, dK);
  double DP1=pMFCB->PressDropKFact(FE.VelMeas(DqScl), Rho, dK);

  double OnePhDPQ=-FE.QmSign()*DP;
  double OnePhDPZ=/*FE.QmSign()**/-0.001*9.81*FE.Rise()*Rho;
  FE.SetVelocity(FE.QmSign()*Vel);
  FE.SetDPq(OnePhDPQ, -FE.QmSign()*(DP1-DP)/(FE.DQmMeas(DqScl)));
  FE.SetDPa(0.0, 0.0);
  FE.SetSlipRatio();
  FE.SetVoidFraction();
  FE.SetOnePhPart();
  FE.SetTempOut(1);

  FE.SetDPz(OnePhDPZ); // ?? What about sin(Theta)
  FE.SetDerivsBad(true);

  if (FE.TwoPhCdPtr())
    {
    double DE=FE.Diam();
    double DI=FE.SrcFBOK() ? FE.SrcFB().Diam() : DE;
    double DO=FE.DstFBOK() ? FE.DstFB().Diam() : DE;

    if (pMFCB->DoFlash(false, FE, 1.0, FE.TwoPhCd(), dK, DI, DE, DO, OnePhDPQ, OnePhDPZ))
      {
      };
    FE.SetVLFracs(NULL, FE.TwoPhCd().MassFrac(som_Gas), FE.TwoPhCd().MassFrac(som_Liq));
    FE.SetTempOut(FE.TwoPhCd().Temp());
    }

  ASSERT_ALWAYS(Valid(FE.DPq()) && _finite(FE.DPq()), "Bad Friction Value", __FILE__, __LINE__);

//  FE.SetMomentum(FE.MeanPress());
  return True;
  };

//===========================================================================
//
//
//
//===========================================================================

DEFINE_TAGOBJ(MFB_Valve);
class MFB_Valve : public MFB_Eqn
  {
  friend class CMultiFlwBlkEdt;
  friend class CMultiFlwBlk;
  protected:
    double  dK;
  public:
    MFB_Valve(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~MFB_Valve();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, bool On, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);

    virtual void   SetPhysData(double DZ)   {};
    virtual void   SetKFact(double K)       { dK=K; };
    virtual double GetKFact()               { return dK; };
  };



IMPLEMENT_FLWEQN(MFB_Valve, MFBPipeGroup.Name(), "MFBVlv", "", TOC_SYSTEM,
                  "Valve",
                  "Valve");

MFB_Valve::MFB_Valve(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MFB_Eqn(pClass_, pTag, pAttach, eAttach)
  {
  dK=1.0;
  }

//--------------------------------------------------------------------------

MFB_Valve::~MFB_Valve()
  {
  }

//--------------------------------------------------------------------------

void MFB_Valve::BuildDataDefn(DataDefnBlk & DDB)
  {
  MFB_Eqn::BuildDataDefn(DDB);

  DDB.Double ("",   "K",  DC_    , "",      &dK,this  ,isParm);

  BuildDataDefnOveride(DDB);
  };

//--------------------------------------------------------------------------

flag MFB_Valve::DataXchg(DataChangeBlk & DCB)
  {
  return MFB_Eqn::DataXchg(DCB);
  };

//--------------------------------------------------------------------------

flag MFB_Valve::ValidateData(ValidateDataBlk & VDB)
  {
  return MFB_Eqn::ValidateData(VDB);
  };

//--------------------------------------------------------------------------

flag MFB_Valve::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, bool On, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  double Rho=Max(0.001, FE.TwoPhCdPtr() ? FE.TwoPhCd().Rho(som_SL) : FE.MeanRho(pProps));
  //double Rho=Max(0.001, FE.MeanRho(pProps));

  // Inital Estimate ???????????
  double DqScl=1.001;
  double Vel=FE.SetVelMeasRange(Rho, FE.Area(), 0.001);

  double DP=pMFCB->PressDropKFact(FE.VelMeas(), Rho, dK);
  double DP1=pMFCB->PressDropKFact(FE.VelMeas(DqScl), Rho, dK);

  double OnePhDPQ=-FE.QmSign()*DP;
  double OnePhDPZ=/*FE.QmSign()**/-0.001*9.81*FE.Rise()*Rho;
  FE.SetVelocity(FE.QmSign()*Vel);
  FE.SetDPq(OnePhDPQ, -FE.QmSign()*(DP1-DP)/(FE.DQmMeas(DqScl)));
  FE.SetDPa(0.0, 0.0);
  FE.SetSlipRatio();
  FE.SetVoidFraction();
  FE.SetOnePhPart();
  FE.SetTempOut(2);

  FE.SetDPz(OnePhDPZ); // ?? What about sin(Theta)
  FE.SetDerivsBad(true);

  if (FE.TwoPhCdPtr())
    {
    double DE=FE.Diam();
//    double DI=FE.SrcFBOK() ? FE.SrcFB().Diam() : DE;
//    double DO=FE.DstFBOK() ? FE.DstFB().Diam() : DE;
    double DI=DE;
    double DO=DE;
    if (pMFCB->DoFlash(false, FE, Regulation, FE.TwoPhCd(), dK, DI, DE, DO, OnePhDPQ, OnePhDPZ))
      {
      };
    FE.SetVLFracs(NULL, FE.TwoPhCd().MassFrac(som_Gas), FE.TwoPhCd().MassFrac(som_Liq));
    FE.SetTempOut(FE.TwoPhCd().Temp());
    }

  return True;
  };

//===========================================================================
//
//
//
//===========================================================================

DEFINE_TAGOBJ(MFB_ExpCon);
class MFB_ExpCon : public MFB_Eqn
  {
  friend class CMultiFlwBlkEdt;
  friend class CMultiFlwBlk;
  protected:
    double  dK;
  public:
    MFB_ExpCon(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~MFB_ExpCon();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, bool On, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);

    virtual void   SetPhysData(double DZ)   {};
    virtual void   SetKFact(double K)       { dK=K; };
    virtual double GetKFact()               { return dK; };

  };



IMPLEMENT_FLWEQN(MFB_ExpCon, MFBPipeGroup.Name(), "MFBExCn", "", TOC_SYSTEM,
                  "ExpCon",
                  "Expansion/Contraction");

MFB_ExpCon::MFB_ExpCon(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MFB_Eqn(pClass_, pTag, pAttach, eAttach)
  {
  }

//--------------------------------------------------------------------------

MFB_ExpCon::~MFB_ExpCon()
  {
  }

//--------------------------------------------------------------------------

void MFB_ExpCon::BuildDataDefn(DataDefnBlk & DDB)
  {
  MFB_Eqn::BuildDataDefn(DDB);

  DDB.Double ("",   "K",  DC_    , "",      &dK,this  ,isParm);

  BuildDataDefnOveride(DDB);
  };

//--------------------------------------------------------------------------

flag MFB_ExpCon::DataXchg(DataChangeBlk & DCB)
  {
  return MFB_Eqn::DataXchg(DCB);
  };

//--------------------------------------------------------------------------

flag MFB_ExpCon::ValidateData(ValidateDataBlk & VDB)
  {
  return MFB_Eqn::ValidateData(VDB);
  };

//--------------------------------------------------------------------------

flag MFB_ExpCon::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, bool On, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  double Rho=Max(0.001, FE.TwoPhCdPtr() ? FE.TwoPhCd().Rho(som_SL) : FE.MeanRho(pProps));
  //double Rho=Max(0.001, FE.MeanRho(pProps));

  double DqScl=1.001;
  double Vel=FE.SetVelMeasRange(Rho, FE.Area(), 0.001);

  double DP=pMFCB->PressDropKFact(FE.VelMeas(Vel), Rho, dK);
  double DP1=pMFCB->PressDropKFact(FE.VelMeas(DqScl), Rho, dK);

  double OnePhDPQ=-FE.QmSign()*DP;
  double OnePhDPZ=/*FE.QmSign()**/-0.001*9.81*FE.Rise()*Rho;
  FE.SetVelocity(FE.QmSign()*Vel);
  FE.SetDPq(OnePhDPQ, -FE.QmSign()*(DP1-DP)/(FE.DQmMeas(DqScl)));
  FE.SetDPa(0.0, 0.0);
  FE.SetSlipRatio();
  FE.SetVoidFraction();
  FE.SetOnePhPart();
  FE.SetTempOut(3);

  FE.SetDPz(OnePhDPZ); // ?? What about sin(Theta)
  FE.SetDerivsBad(true);

  if (FE.TwoPhCdPtr())
    {
    double DE=FE.Diam();
    double DI=FE.SrcFBOK() ? FE.SrcFB().Diam() : DE;
    double DO=FE.DstFBOK() ? FE.DstFB().Diam() : DE;
    if (pMFCB->DoFlash(false, FE, 1.0, FE.TwoPhCd(), dK, DI, DE, DO, OnePhDPQ, OnePhDPZ))
      {
      };
    FE.SetVLFracs(NULL, FE.TwoPhCd().MassFrac(som_Gas), FE.TwoPhCd().MassFrac(som_Liq));
    FE.SetTempOut(FE.TwoPhCd().Temp());
    }

  return True;
  };

//===========================================================================
//
//
//
//===========================================================================

DEFINE_TAGOBJ(MFB_EnEx);
class MFB_EnEx : public MFB_Eqn
  {
  friend class CMultiFlwBlkEdt;
  friend class CMultiFlwBlk;
  protected:
    double  dK;
  public:
    MFB_EnEx(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~MFB_EnEx();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
//    virtual void   SetPhysData(double DZ);
    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, bool On, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);
    virtual void   SetPhysData(double DZ)   {};
    virtual void   SetKFact(double K)       { dK=K; };
    virtual double GetKFact()               { return dK; };
  };



IMPLEMENT_FLWEQN(MFB_EnEx, MFBEnExGroup.Name(), "MFBEnEx", "", TOC_SYSTEM,
                  "EntExt",
                  "Crane Entry Exit");

MFB_EnEx::MFB_EnEx(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MFB_Eqn(pClass_, pTag, pAttach, eAttach)
  {
  dK=1.0;
  }

//--------------------------------------------------------------------------

MFB_EnEx::~MFB_EnEx()
  {
  }

//--------------------------------------------------------------------------

void MFB_EnEx::BuildDataDefn(DataDefnBlk & DDB)
  {
  MFB_Eqn::BuildDataDefn(DDB);

  DDB.Double ("",   "K",  DC_    , "",      &dK,this  ,isParm);

  BuildDataDefnOveride(DDB);
  };

//--------------------------------------------------------------------------

flag MFB_EnEx::DataXchg(DataChangeBlk & DCB)
  {
  return MFB_Eqn::DataXchg(DCB);
  };

//--------------------------------------------------------------------------

flag MFB_EnEx::ValidateData(ValidateDataBlk & VDB)
  {
  //if (!PhDOK())
  //  return true;
  SetRqdLength(0.0);
  SetActLength(0.0);
  return MFB_Eqn::ValidateData(VDB);
  };

//--------------------------------------------------------------------------

//void MFB_EnEx::SetPhysData(double DZ)
//  {
////  D.dLength=0.0;//Max(fabs(DZ), Len_Rqd));
////  D.dXLength=0.0;
////  D.dArea=Area;
//  };

//--------------------------------------------------------------------------

flag MFB_EnEx::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, bool On, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  double Rho=Max(0.001, FE.TwoPhCdPtr() ? FE.TwoPhCd().Rho(som_SL) : FE.MeanRho(pProps));
  //double Rho=Max(0.001, FE.MeanRho(pProps));


  double DqScl=1.001;
  double Vel=FE.SetVelMeasRange(Rho, FE.Area(), 0.001);

  double DP=pMFCB->PressDropKFact(FE.VelMeas(), Rho, dK);
  double DP1=pMFCB->PressDropKFact(FE.VelMeas(DqScl), Rho, dK);

//dbgpln("@@@@@@@@ Rho %10.4f  Vel %10.4f", Rho, Vel);

  double OnePhDPQ=-FE.QmSign()*DP;
  double OnePhDPZ=/*FE.QmSign()**/-0.001*9.81*FE.Rise()*Rho;
  FE.SetVelocity(FE.QmSign()*Vel);
  FE.SetDPq(OnePhDPQ, -FE.QmSign()*(DP1-DP)/(FE.DQmMeas(DqScl)));
  FE.SetDPa(0.0, 0.0);
  FE.SetSlipRatio();
  FE.SetVoidFraction();
  FE.SetOnePhPart();
  FE.SetTempOut(4);

  FE.SetDPz(OnePhDPZ); // ?? What about sin(Theta)
  FE.SetDerivsBad(true);

  if (FE.TwoPhCdPtr())
    {
    //if (FE.QmSign()<0)
    //  pMFCB->SetPrevDiam(FE.Diam());
    double DE=FE.Diam();
    double DI=FE.SrcFBOK() ? FE.SrcFB().Diam() : DE;
    double DO=FE.DstFBOK() ? FE.DstFB().Diam() : DE;
    if (pMFCB->DoFlash(FE.QmSign()<0, FE, Regulation, FE.TwoPhCd(), dK, DI, DE, DO, OnePhDPQ, OnePhDPZ))
      {
      };
    FE.SetVLFracs(NULL, FE.TwoPhCd().MassFrac(som_Gas), FE.TwoPhCd().MassFrac(som_Liq));
    FE.SetTempOut(FE.TwoPhCd().Temp());
    }

  ASSERT_ALWAYS(Valid(FE.DPq()) && _finite(FE.DPq()), "Bad Friction Value", __FILE__, __LINE__);

  return True;
  };

//===========================================================================
//
//
//
//===========================================================================

DEFINE_TAGOBJ(MFB_Join);
class MFB_Join : public MFB_Eqn
  {
  friend class CMultiFlwBlkEdt;
  friend class CMultiFlwBlk;
  protected:
    double  dK;
  public:
    MFB_Join(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~MFB_Join();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual void   SetPhysData(double DZ);
    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, bool On, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);
  };



IMPLEMENT_FLWEQN(MFB_Join, MFBJoinGroup.Name(), "MFBJoin", "", TOC_SYSTEM,
                  "Join",
                  "Crane Join");

MFB_Join::MFB_Join(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MFB_Eqn(pClass_, pTag, pAttach, eAttach)
  {
  }

//--------------------------------------------------------------------------

MFB_Join::~MFB_Join()
  {
  }

//--------------------------------------------------------------------------

void MFB_Join::BuildDataDefn(DataDefnBlk & DDB)
  {
  MFB_Eqn::BuildDataDefn(DDB);

  DDB.Double ("",   "K",  DC_    , "",      &dK,this  ,isParm);

  BuildDataDefnOveride(DDB);
  };

//--------------------------------------------------------------------------

flag MFB_Join::DataXchg(DataChangeBlk & DCB)
  {
  return MFB_Eqn::DataXchg(DCB);
  };

//--------------------------------------------------------------------------

flag MFB_Join::ValidateData(ValidateDataBlk & VDB)
  {
//  if (!PhDOK())
//    return true;
  SetRqdLength(0.0);
  SetActLength(0.0);
  return MFB_Eqn::ValidateData(VDB);
  };

//--------------------------------------------------------------------------

void MFB_Join::SetPhysData(double DZ)
  {
//  D.dLength=0.0;
//  D.dXLength=0.0;
//  D.dArea=Area;
  };

//--------------------------------------------------------------------------

flag MFB_Join::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, bool On, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  double Rho=Max(0.001, FE.TwoPhCdPtr() ? FE.TwoPhCd().Rho(som_SL) : FE.MeanRho(pProps));
  //double Rho=Max(0.001, FE.MeanRho(pProps));

  double DqScl=1.001;
  double Vel=FE.SetVelMeasRange(Rho, FE.Area(), 0.001);

  double DP=pMFCB->PressDropKFact(FE.VelMeas(), Rho, dK);
  double DP1=pMFCB->PressDropKFact(FE.VelMeas(DqScl), Rho, dK);

  double OnePhDPQ=-FE.QmSign()*DP;
  double OnePhDPZ=/*FE.QmSign()**/-0.001*9.81*FE.Rise()*Rho;
  FE.SetVelocity(FE.QmSign()*Vel);
  FE.SetDPq(OnePhDPQ, -FE.QmSign()*(DP1-DP)/(FE.DQmMeas(DqScl)));
  FE.SetDPa(0.0, 0.0);
  FE.SetSlipRatio();
  FE.SetVoidFraction();
  FE.SetOnePhPart();
  FE.SetTempOut(5);

  FE.SetDPz(OnePhDPZ); // ?? What about sin(Theta)
  FE.SetDerivsBad(true);

  if (FE.TwoPhCdPtr())
    {
    double DE=FE.Diam();
    double DI=FE.SrcFBOK() ? FE.SrcFB().Diam() : DE;
    double DO=FE.DstFBOK() ? FE.DstFB().Diam() : DE;
    if (pMFCB->DoFlash(false, FE, 1.0, FE.TwoPhCd(), dK, DI, DE, DO, OnePhDPQ, OnePhDPZ))
      {
      };
    FE.SetVLFracs(NULL, FE.TwoPhCd().MassFrac(som_Gas), FE.TwoPhCd().MassFrac(som_Liq));
    FE.SetTempOut(FE.TwoPhCd().Temp());
    }
  return True;
  };

//===========================================================================
//
//
//
//===========================================================================


