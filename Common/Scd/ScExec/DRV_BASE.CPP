//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
#define  __DRV_BASE_CPP
#include "tagobj.h"
#include "execlib.h"
#include "drv_base.h"
#include "dbgmngr.h"

//#include "optoff.h"

#define dbgDriverMan     1

#if dbgDriverMan
static CDbgMngr dbgRandomNumbers ("DriverManager", "RandomNumbers");
static CDbgMngr dbgFeedBack      ("DriverManager", "FeedBack");
#endif

const char* OPC_DLL_Name = "scOPCsrv.dll";

#if WITHDRVDEBUG 
double CDriver::TimeFromStart()
  {
  static DWORD S=0;
  if (S==0) S=GetTickCount();
  return 0.001*(GetTickCount()-S);
  }

#endif

//===========================================================================

DrvHstCfg::DrvHstCfg()
  {
  iFilterAlg = 0;//HSFA_None;
  iWinFltCount = 0;
  dFilterTau = 0.0;
  dWinFltPeriod = 0.0;
  dFltDelta = 0.0;
  iBoxcarAlg = 0;//HSA_None;
  dDeltaLo      = 0.0;
  dDeltaHi      = 0.0;
  iNoRec        = 0;
  iNoNotRec     = 0;
  dDecrease     = 0.0;
  }

DrvArcCfg::DrvArcCfg()
  {
  m_iFn=ADBFn_Null;
  }

//===========================================================================
// If these are inline some drivers are not correctly Constructed
CDriver::CDriver(const char* Tag) 
  { 
  sTag = Tag;
  InitializeCriticalSection(&critical_section);
  hLib = NULL;
  bConfigIsBusy = 1;
  pDrvMngr = NULL;
  bTwoCfgPasses = false;
#if DBGDRIVERLOCKS
  dbgLockTime=0.0;
  SW.Clear();
  SW.Start();
#endif
  bLclTagSrvrOK=0;
  }

// --------------------------------------------------------------------------

CDriver::~CDriver() 
  {
  DeleteCriticalSection(&critical_section);
  }

// --------------------------------------------------------------------------

void CDriver::DrvError(CDrvCfgHelper * pCfg, DWORD Flags, char *Drv, char * fmt, ...)
  {
  char S[MsgBuffLen]; 
  va_list argptr;
  va_start(argptr,fmt);
  vsprintf(S, fmt, argptr);
  va_end(argptr);

  Strng Msg;
  Msg.Set("Item %i: '%s' %s", pCfg->lLineNumber, pCfg->sTag(), S);
  //LogError((char*)Drv, 0, S());
  LogLn(Flags, (char*)Drv, 0, Msg());
  pCfg->bErrorFlagged=true;
  };

//===========================================================================

IMPLEMENT_SPARES(CDelayItem, 5000);

CDelayItem::CDelayItem(CDrvSubsConn* pConSlot, PkDataUnion& Dat, DWORD DelayTime)
  {
  pCon = pConSlot;
  pNxt = NULL;
  pPrev = NULL;
  Data.Set(Dat);
  dwDelayTime = DelayTime;
  dwStartTime = GetTickCount();
  static int ANumber=0;  
  dwNumber = ANumber++;
  }

//===========================================================================

IMPLEMENT_SPARES(CDrvSltChange, 5000);

CDrvSltChange::CDrvSltChange(CDrvSlot * pSlot, long lDataIndex, double d, byte Typ, CCnvIndex Cnv, LPCTSTR CnvTxt)
  {
  m_pSlot = pSlot;
  m_pCdBlk=NULL;
  m_pCdBlkVar=NULL;
  DataIndex = lDataIndex;
  dVal = d;
  iTyp = Typ;
  m_iCnv=Cnv;
  m_sCnvTxt=CnvTxt;
  }

//===========================================================================

CDrvSltChange::CDrvSltChange(CDrvCodeBlk *pCdBlk, GCVar *pCdBlkVar, long lDataIndex, double d, byte Typ, CCnvIndex Cnv, LPCTSTR CnvTxt)
  {
  m_pSlot = NULL;
  m_pCdBlk = pCdBlk;
  m_pCdBlkVar = pCdBlkVar;
  DataIndex = lDataIndex;
  dVal = d;
  iTyp = Typ;
  m_iCnv=Cnv;
  m_sCnvTxt=CnvTxt;
  }

//===========================================================================

CDrvSubsConn::CDrvSubsConn(pchar pTag, pchar pCnvTxt, flag Get, flag Inv)
  {
  m_sTag = pTag;
  m_sCnvTxt = pCnvTxt;
  m_iCnv = 0;
  m_bGet = Get;
  m_Typ = 0;
  bInv = Inv;
  dwDelayTime = 0;
  dwDelayTime2 = 0;
  m_nOps=0;
  for (int i=0; i<DrvSubsConnMaxOps; i++)
    m_Op[i]=NULL;//CCS_Null;
  bEdge = 0;
  bUseDelay2 = 0;
  bInternal = 0;
  bDelayLock = 0;

  iSubsIndex = 0xffff;
  m_pParentSlot = NULL;
  m_pSlot = NULL;
  m_pCdBlk =  NULL;
  m_pCdBlkVar =  NULL;
  m_pNxt = NULL;
  m_pNxtSubs = NULL;
  }

// --------------------------------------------------------------------------

CDrvSubsConn::~CDrvSubsConn()
  {
  for (int i=0; i<DrvSubsConnMaxOps; i++)
    delete m_Op[i];
  };

// --------------------------------------------------------------------------

void CDrvSubsConn::SetDelayTimes(DWORD DelayTime, DWORD DelayTime2, flag UseDelay2)
  {
  dwDelayTime = DelayTime;
  dwDelayTime2 = DelayTime2;
  bUseDelay2 = UseDelay2;
  bEdge = (dwDelayTime==InfiniteDelay || (bUseDelay2 && dwDelayTime2==InfiniteDelay));
  }

// --------------------------------------------------------------------------

CDrvSubsConn * CDrvSubsConn::AddGetConnect(CDrvSubsConn*& pFirst, pchar pTag, flag Inv)//, byte Op, double LclConst)
  {
  if (pTag && strlen(pTag)>0)
    {
    Strng WrkTag,WrkCnvTxt;
    TaggedObject::SplitTagCnv(pTag, WrkTag, WrkCnvTxt);
    TaggedObject::ValidateTag(WrkTag);
    if (WrkTag.Length()>0)
      {
      CDrvSubsConn* pNew = new CDrvSubsConn(WrkTag(), WrkCnvTxt(), True, Inv);
      //pNew->m_Op[pNew->m_nOps++]=Op;
      //pNew->m_dLclConst = (float)LclConst;
      if (pFirst)
        pNew->m_pNxt = pFirst;
      pFirst = pNew;
      return pNew;
      }
    }
  return NULL;
  }

// --------------------------------------------------------------------------

CDrvSubsConn * CDrvSubsConn::AddSetConnect(CDrvSubsConn*& pFirst, pchar pTag, flag Inv, DWORD DelayTime, DWORD DelayTime2, flag UseDelay2)//, byte Op, double LclConst)
  {
  if (pTag && strlen(pTag)>0)
    {
    Strng WrkTag,WrkCnvTxt;
    TaggedObject::SplitTagCnv(pTag, WrkTag, WrkCnvTxt);
    TaggedObject::ValidateTag(WrkTag);
    if (WrkTag.Length()>0)
      {
      CDrvSubsConn* pNew = new CDrvSubsConn(WrkTag(), WrkCnvTxt(), False, Inv);
      pNew->SetDelayTimes(DelayTime, DelayTime2, UseDelay2);
      if (pFirst)
        pNew->m_pNxt = pFirst;
      pFirst = pNew;
      return pNew;
      }
    }
  return NULL;
  }

//---------------------------------------------------------------------------

bool CDrvSubsConn::PutSubsValue2Slot(CDriverManagerBase * pMngr, CPkDataItem *pPItem, bool ForcesOn, double DeltaTime, bool AllowNoise)
  {
//  m_Typ = pPItem->Type();
//  if (pPItem->Contains(PDI_CnvInfo))
//    {
//    m_iCnv=pPItem->CnvIndex();
//    long i=(m_iCnv);
//    if (i!=0 && Cnvs[i]->Find(m_sCnvTxt())==NULL)
//      LogError("DrvMngr", 0, "Cnv %s not found for %s", m_sCnvTxt(), m_sTag());
//    }

  if (m_bGet)
    {
    //value has changed, it must be sent (for trends etc), add it to linked list...
    double d = pPItem->Value()->GetDouble(m_iCnv, m_sCnvTxt());
    for (int op=0; op<m_nOps; op++)
      {
      if (AllowNoise || dynamic_cast<CDrvSubsConnOp_Noise*>(m_Op[op])==NULL)
        d=m_Op[op]->Exec(d, DeltaTime);
      if (!Valid(d))
        { int xxx=0;}
      }
    
    if (ForcesOn && m_pParentSlot->bEnableForces)
      {
      PkDataUnion Data(m_pParentSlot->dForceVal);
      m_pParentSlot->PutTagValue(Data); //send force value to driver...
      }
    else
      {
      PkDataUnion Data(d);
      m_pParentSlot->PutTagValue(Data); //send to driver...
      // pParentSlot->PutTagValue(*(pPItem->Value())); //send to driver...
      }
    long DataIndex = m_pParentSlot->SlotNo;
    pMngr->AddSltChange(DataIndex, d, m_pParentSlot); //send to SysCAD EO objects...
    return true;
    }
  return false;
  }

//===========================================================================

CDrvSubsConnPrf::CDrvSubsConnPrf()
  {
  m_Mode=eNULL;
  };

//---------------------------------------------------------------------------

long CDrvSubsConnPrf::Parse(LPCSTR File)
  {
  if (!FileExists((LPTSTR)File))
    return 1;
  
  m_sTag.FnName((LPTSTR)File);
  m_bYReversed=false;
  
  long RetCode=0;
  Strng Ext;
  Ext.FnExt((LPTSTR)File);
  if (Ext.XStrICmp(".txt")==0 || Ext.XStrICmp(".csv")==0)
    {
    FILE *h=fopen(File, "rt");
    if (h)
      {
      char Buff[4096];
      CSVColArray c;
      int Quote;
      int nFlds = 0;
      Buff[0]=0;
      while (strlen(Buff)==0 && fgets(Buff, sizeof(Buff), h))
        XStrLTrim(Buff, " \t\n");
       
      nFlds = ParseCSVTokens(Buff, c, Quote);
      
      if (nFlds>0)
        {
        if (_stricmp(Buff, "ABS")==0)
          m_Mode=eABS;
        else if (_stricmp(Buff, "SCL")==0)
          m_Mode=eSCL;
        else if (_stricmp(Buff, "SCL%")==0)
          m_Mode=eSCLPERC;
        else if (_stricmp(Buff, "CONTRONIC")==0)
          m_Mode=eCONTRONIC;
        else 
          {
          return 4;
          goto Leave;
          }
        
        if (m_Mode==eCONTRONIC)
          {
          while (fgets(Buff, sizeof(Buff), h))
            {
            int nFlds = ParseTokenList(Buff, c, "=:");
            if (nFlds>=3)
              {
              CDrvSubsConnPrfPt Pt;
              Pt.X=(float)SafeAtoF(c[1]);
              Pt.Y=(float)SafeAtoF(c[2]);
              m_Points.Add(Pt);
              }
            else if (m_Points.GetSize()>0)
              break;
            }
          }
        else
          {
          while (fgets(Buff, sizeof(Buff), h))
            {
            int nFlds = ParseCSVTokens(Buff, c, Quote);
            if (nFlds>=2)
              {
              CDrvSubsConnPrfPt Pt;
              Pt.X=(float)SafeAtoF(c[0]);
              Pt.Y=(float)SafeAtoF(c[1]);
              m_Points.Add(Pt);
              }
            else if (m_Points.GetSize()>0)
              break;
            }
          }
        if (m_Points.GetSize()<2)
          {
          RetCode=5;
          goto Leave;
          }
        m_bYReversed=m_Points[0].Y> m_Points[m_Points.GetUpperBound()].Y;
        fclose(h);
        }
      else
        RetCode=4;
      }
    else
      RetCode=2;
Leave:
    if (h)
      fclose(h);
    return RetCode;
    }
  else
    return 3;
  };

// CONTRONIC FORMAT
//NO. 1=    .0 %:        .0 I/P VALUE  :
//NO. 2=   6.2 %:     123.6 I/P VALUE  :
//NO. 3=  12.5 %:     221.1 I/P VALUE  :
//NO. 4=  18.7 %:     308.1 I/P VALUE  :
//---------------------------------------------------------------------------

LPCTSTR CDrvSubsConnPrf::ErrorString(long RetCode)
  {
  switch (RetCode)
    {
    case 0: return "No Error";
    case 1: return "File missing";
    case 2: return "File not opened";
    case 3: return "Must be txt or csv";
    case 4: return "First Line must contain ABS,SCL SCL% or CONTRONIC";
    case 5: return "Must have 2 or more points";
    default:
      return "Unknown error";
    }
  };

//---------------------------------------------------------------------------

double CDrvSubsConnPrf::X2Y(double X)
  {
  double Y=0;
  long i0, i1, inc;
  i0=0;
  i1=m_Points.GetUpperBound();
  inc=1;

  for (int i=i0; i0!=i1; i+=inc)
    if (X<m_Points[i].X)
      break;

  CDrvSubsConnPrfPt & Pt0=m_Points[i];
  CDrvSubsConnPrfPt & Pt1=m_Points[i+1];
   
  Y=Pt0.Y+(Pt1.Y-Pt0.Y)*(X-Pt0.X)/(Pt1.X-Pt0.X);

  return Y;
  };

//---------------------------------------------------------------------------

double CDrvSubsConnPrf::Y2X(double Y)
  {
  double X=0;
  long i0, i1, inc;
  if (m_bYReversed)
    {
    i0=m_Points.GetUpperBound();
    i1=0+1;
    inc=-1;
    }
  else
    {
    i0=0;
    i1=m_Points.GetUpperBound()-1;
    inc=1;
    }
  for (int i=i0; i!=i1; i+=inc)
    if (Y<m_Points[i].Y)
      break;

  CDrvSubsConnPrfPt & Pt0=m_Points[i];
  CDrvSubsConnPrfPt & Pt1=m_Points[i+inc];
   
  X=Pt0.X+(Pt1.X-Pt0.X)*(Y-Pt0.Y)/NZ(Pt1.Y-Pt0.Y);

  return X;
  };


//===========================================================================

IMPLEMENT_SPARES(CProcessConnItem,2000);

//===========================================================================

CDrvSlot::CDrvSlot(pchar pTag)
  {
  sTag = pTag;
  iTyp = 0;
  bChanged = 0;
  bSetReqd = 0;
  m_bReqdByExec = 0;
  bHstCfgd = 0;
  bArcCfgd = 0;
  bValInited = 0;
  bValueWritten = 0;
  bRecordIt = 0;
  bIgnoreWrites = 0;
  bIgnoreReads = 0;
  bEnableForces = 0;
  bLocal = 0;
  m_bHasSet = 0;
  m_bHasSetAsGet = 0;
  bHoldOn = 0;
  bUseInitVal = 0;
  bHasFarCon = 0;
  bHasGetConn = 0;
  dInitVal = 0.0;
  dForceVal = 0.0;
  SlotNo = 0;
  iStatus = 0;
  chgcount = 0;
  //iAction = XIO_None;
  m_iCnv = 0; 
  iCnt = 0;
  pDrv = NULL;
  pHstCfg = NULL;
  pSpan = NULL;
  pRange = NULL;
  m_pConnects = NULL;
  m_pValLst=NULL;

  m_bProcessConnectsBusy=0;
  }

// --------------------------------------------------------------------------

CDrvSlot::~CDrvSlot() 
  {
  delete m_pValLst;
  }                               

// --------------------------------------------------------------------------

flag CDrvSlot::GetTagValue(CPkDataList &List, CPkDataItem * &pPItem, flag Complete)
  {
  flag GotIt = False;
  PkDataUnion PData;
  #if dbgDriverMan
  if (dbgRandomNumbers())
    GotIt = dbgGetValue(PData); //call virtual method
  #endif

  if (!GotIt)
    GotIt = GetValue(PData); //call virtual method
  
  if (Complete)
    {
    //supply ranges in SI units !!!
    DataUnion uMin;
    DataUnion uMax;
    if (pRange)
      {
      uMin.Set(Cnvs[m_iCnv]->Normal(pRange->RngLo, m_sCnvTxt()));
      uMax.Set(Cnvs[m_iCnv]->Normal(pRange->RngHi, m_sCnvTxt()));
      }
    else
      {
      uMin.Set(Cnvs[m_iCnv]->Normal(0.0, m_sCnvTxt()));
      uMax.Set(Cnvs[m_iCnv]->Normal(1.0, m_sCnvTxt()));
      }
    List.SetDataValueAll(/*pPItem,*/ sTag(), "", PData, isDriverTag | ((iAction & XIO_In) ? DDEF_PARAM : 0),
                         uMin, uMax, m_iCnv, m_sCnvTxt(), ""/*sCnvFam()*/, NULL, False, sDescription());
    }
  else
    List.SetDataValue(/*pPItem,*/ NULL, PData);
  return True;
  }

// --------------------------------------------------------------------------

flag CDrvSlot::PutTagValue(PkDataUnion& Data)
  {
  if ((iAction & XIO_In)!=0)
    {
    if (SetValue(Data))
      {
      bValueWritten=1;
      return True;
      }
    }
  return False;
  }

// --------------------------------------------------------------------------

void CDrvSlot::GetDrvDesc(CString& s) 
  {
  s = pDrv->sTag();
  }

// --------------------------------------------------------------------------

void CDrvSlot::GetLongDrvDesc(CString& s)
  {
  s = pDrv->sTag();
  }

// --------------------------------------------------------------------------

void CDrvSlot::GetTagOrAddrDesc(CString& s)
  {
  s = pDrv->sTag();
  }

// --------------------------------------------------------------------------

void TryAddSltChange(CDrvSubsConn* pCon, CDrvSlot * pSetSlt, CDriver * pDrv, double TheVal, long Level)
  {
  if (pCon->bInternal)
    {
    if (pSetSlt)
      {
      if (pSetSlt->m_bReqdByExec)
        {//only need to send it back if somebody wants it
        const long DataIndex = pSetSlt->SlotNo;
        #if dbgDriverMan
        if (dbgFeedBack()) dbgpln("%*sFBK AddSltChg Intern[%5i] %s > %s",(Level*8)+4,"", DataIndex, pCon->m_sTag(), pSetSlt->sTag());
        #endif
        pDrv->pDrvMngr->AddSltChange(DataIndex, TheVal, pCon, Level);
        }
      }
    }
  else
    {
    const long DataIndex = ConIndexStart + pCon->iSubsIndex;
    #if dbgDriverMan
    if (dbgFeedBack()) dbgpln("%*sFBK AddSltChg       [%5i] %s",(Level*8)+4,"", DataIndex, pCon->m_sTag());
    #endif
    pDrv->pDrvMngr->AddSltChange(DataIndex, TheVal, pCon, Level);
    }
  };

// --------------------------------------------------------------------------

flag CDrvSlot::AddProcessConnItem(CProcessConnItem * &pItemListHead, CProcessConnItem *&pItemList, 
                                  flag AsDouble,
                                  double rVal, long lVal, flag DoingForceSet, CDrvSubsConn* pCon, CDrvSlot * pSlt, CDriver * pDrv, long Level)
  {
  CProcessConnItem * p= new CProcessConnItem;
  if (pItemListHead==NULL)
    pItemListHead=p;
  if (pItemList)
    {
    p->m_pNext=p;
    pItemList=p;
    }
  else
    p->m_pNext=NULL;

  p->m_bDouble=AsDouble;
  p->m_rVal=rVal;
  p->m_lVal=lVal;
  p->m_bDoingForceSet=DoingForceSet; 
  p->m_pCon=pCon; 
  p->m_pSlt=pSlt; 
  p->m_pDrv=pDrv;
  p->m_lLevel=Level;
  
  return AsDouble ? TestVal(rVal, DoingForceSet) : TestVal(lVal, DoingForceSet);
  }

// --------------------------------------------------------------------------

void CDrvSlot::ApplyProcessConnItems(CProcessConnItem * pItemListHead)
  {
  CProcessConnItem * p=pItemListHead;
  while (pItemListHead)
    {
    CProcessConnItem * p=pItemListHead;
    
    if (p->m_bDouble)
      {
      p->m_pSlt->SetVal(p->m_rVal, p->m_bDoingForceSet);
      TryAddSltChange(p->m_pCon, p->m_pSlt, p->m_pDrv, p->m_rVal, p->m_lLevel);
      }
    else
      {
      p->m_pSlt->SetVal(p->m_lVal, p->m_bDoingForceSet);
      TryAddSltChange(p->m_pCon, p->m_pSlt, p->m_pDrv, p->m_lVal, p->m_lLevel);
      }
    pItemListHead=pItemListHead->m_pNext;
    delete p;
    }
  }

// --------------------------------------------------------------------------

void CDrvSlot::ProcessConnects(flag Chng, flag WriteThru, flag FromOutput, flag ForceSet, long Level)
  {
  CProcessConnItem * pItemListHead=NULL;
  CProcessConnItem * pItemList=NULL;
  ProcessConnects(pItemListHead, pItemList, Chng, WriteThru, FromOutput, ForceSet, Level);
  };

// --------------------------------------------------------------------------

#define DELAYPROCCONNSETS 1

void CDrvSlot::ProcessConnects(CProcessConnItem * &pItemListHead, CProcessConnItem *&pItemList, flag Chng, flag WriteThru, flag FromOutput, flag ForceSet, long Level)
  {
  //CProcessConnItem * pItemListHead=NULL;
  if (m_bProcessConnectsBusy)
    {
    LogError("DrvMngr", 0, "Circular Reference to %s", sTag());
    }
  else
    {
    m_bProcessConnectsBusy++;
    #if dbgDriverMan
    if (dbgFeedBack()) dbgpln("%*sProcessConnects  %s %s %s",(Level*8),"", m_bHasSet?"Set":"   ", m_bHasSetAsGet?"SetG":"    ", sTag());
    #endif
    if (m_bHasSet || m_bHasSetAsGet)
      {
      CDrvSubsConn* pCon = m_pConnects;
      while (pCon)
        {
        if (!pCon->m_bGet)
          {
          #if dbgDriverMan
          LPTSTR dbgConnTag=pCon->m_sTag();
          #endif

          CDrvSlot *pSetSlt=pCon->m_pSlot;
          CDrvCodeBlk *pSetCB=pCon->m_pCdBlk;

          flag FeedBack = 0;
          flag Done = 0;
          PkDataUnion Data;
          #if WITHDRVDEBUG 
          dbgSrcReadTime=CDriver::TimeFromStart();
          #endif
          GetTheVal(Data, FromOutput);
          flag DoingForceSet = ForceSet;//(!Chng && ForceSet && !pCon->bDelayLock);
          flag DoingWriteThru = (!Chng && (pDrv->bDoWriteThru||WriteThru) && !pCon->bDelayLock);
          flag SetDelayZero  = (!Chng);
          if (Chng || DoingForceSet || DoingWriteThru)
            {
            ClearFBKChange();
            //#if dbgDriverMan
            //if (dbgTimes())
            //  dbgpln("AB: chang: %10s %.3f  %d", pV->pSlot->sTag(), SW.Secs(), pV->Content);
            //#endif
            FeedBack = 1;
            if (pCon->bInternal)
              {
              //internal driver tag must be set, do it now ...
              dword DlyTime = (SetDelayZero ? 0 : pCon->dwDelayTime);
              if (pCon->dwDelayTime==InfiniteDelay)
                DlyTime = pCon->dwDelayTime;
              if (IsFloatData(iTyp))
                {
                if (DlyTime==0)
                  {
                  double d = Data.GetDouble();
                  if (pCon->bInv)
                    d = !d;
                  if (pSetCB)
                    {
                    #if dbgDriverMan
                    if (dbgFeedBack()) dbgpln("%*sFBK SetGC %g %s > %s", (Level*8)+2,"", d, sTag(), pCon->m_sTag());
                    #endif
                    // Set the Value
                    pCon->m_pCdBlkVar->set(d);

                    // Execute pgm
                    CGExecContext ECtx(NULL);
                    pSetCB->m_Code.Execute(ECtx);

                    // Update Destination Tags
                    // Update Destination Tags
                    for (int i=0; i<pSetCB->m_GetsFromMe.GetSize(); i++)
                      {
                      CDrvSubsConn *pGetCon=pSetCB->m_GetsFromMe[i];
                      ASSERT(pGetCon->m_pCdBlk==pSetCB);
                      if (pGetCon->m_pCdBlkVar->WhatAmI()==VarDouble)
                        {
                        double res=pGetCon->m_pCdBlkVar->getD();
                        CDrvSlot * pSetSlt=pGetCon->m_pParentSlot;
                        #if dbgDriverMan
                        if (dbgFeedBack()) dbgpln("%*sFBK GetGC %g %s > %s", (Level*8)+2,"", res, pGetCon->m_sTag(), pSetSlt->sTag());
                        #endif
#if DELAYPROCCONNSETS
                        flag Chng=AddProcessConnItem(pItemListHead, pItemList, true, res, 0, DoingForceSet, pGetCon, pSetSlt, pDrv, Level);
#else
                        flag Chng=pSetSlt->SetVal(res, DoingForceSet);
                        TryAddSltChange(pGetCon, pSetSlt, pDrv, res, Level);
#endif
                        pSetSlt->ProcessConnects(pItemListHead, pItemList, Chng, WriteThru, FromOutput, ForceSet, Level+1);
                        }
                      else
                        {
                        long res=pGetCon->m_pCdBlkVar->getL();
                        CDrvSlot * pSetSlt=pGetCon->m_pParentSlot;
                        #if dbgDriverMan
                        if (dbgFeedBack()) dbgpln("%*sFBK GetGC %i %s > %s", (Level*8)+2,"", res, pGetCon->m_sTag(), pSetSlt->sTag());
                        #endif
#if DELAYPROCCONNSETS
                        flag Chng=AddProcessConnItem(pItemListHead, pItemList, false, 0, res, DoingForceSet, pGetCon, pSetSlt, pDrv, Level);
#else
                        flag Chng=pSetSlt->SetVal(res, DoingForceSet);
                        TryAddSltChange(pGetCon, pSetSlt, pDrv, res, Level);
#endif
                        pSetSlt->ProcessConnects(pItemListHead, pItemList, Chng, WriteThru, FromOutput, ForceSet, Level+1);
                        }
                      int xxx=0;
                      }
                    int xxx=0;
                    }
                  else
                    {
                    #if dbgDriverMan
                    if (dbgFeedBack()) dbgpln("%*sFBK Set   %g %s > %s", (Level*8)+2,"", d, sTag(), dbgConnTag);
                    #endif
#if DELAYPROCCONNSETS
                    flag Chng=AddProcessConnItem(pItemListHead, pItemList, true, d, 0, DoingForceSet, pCon, pSetSlt, pDrv, Level);
#else
                    pSetSlt->SetVal(d, DoingForceSet);
                    TryAddSltChange(pCon, pSetSlt, pDrv, d, Level);
#endif
                    pSetSlt->ProcessConnects(pItemListHead, pItemList, Chng, WriteThru, FromOutput, ForceSet, Level+1);
                    }

                  Done = 1;
                  }
                }
              else
                {
                long l = Data.GetLong();
                if (pCon->bUseDelay2 && l && (!SetDelayZero || pCon->dwDelayTime2==InfiniteDelay))
                  DlyTime = pCon->dwDelayTime2;
                if (DlyTime==0)
                  {
                  if (pCon->bInv)
                    l = !l;
                  if (pSetCB)
                    {
                    #if dbgDriverMan
                    if (dbgFeedBack()) dbgpln("%*sFBK SetGC %i %s > %s", (Level*8)+2,"", l, sTag(), pCon->m_sTag);
                    #endif
                    // Set the Value
                    pCon->m_pCdBlkVar->set(l);
                  
                    #if dbgDriverMan
                    if (dbgFeedBack()) dbgpln("%*sFBK Exec %s [%s]", (Level*8)+2,"", pSetCB->m_sTag(), pSetCB->m_Code.m_DbgMngr.sPgmName());
                    #endif
                    // Execute pgm
                    CGExecContext ECtx(NULL);
                    pSetCB->m_Code.Execute(ECtx);
                  
                    // Update Destination Tags
                    for (int i=0; i<pSetCB->m_GetsFromMe.GetSize(); i++)
                      {
                      CDrvSubsConn *pGetCon=pSetCB->m_GetsFromMe[i];
                      ASSERT(pGetCon->m_pCdBlk==pSetCB);
                      if (pGetCon->m_pCdBlkVar->WhatAmI()==VarDouble)
                        {
                        double res=pGetCon->m_pCdBlkVar->getD();
                        CDrvSlot * pSetSlt=pGetCon->m_pParentSlot;
                        #if dbgDriverMan
                        if (dbgFeedBack()) dbgpln("%*sFBK GetGC %g %s > %s", (Level*8)+2,"", res, pGetCon->m_sTag(), pSetSlt->sTag());
                        #endif
#if DELAYPROCCONNSETS
                        flag Chng=AddProcessConnItem(pItemListHead, pItemList, true, res, 0, DoingForceSet, pGetCon, pSetSlt, pDrv, Level);
#else
                        flag Chng=pSetSlt->SetVal(res, DoingForceSet);
                        TryAddSltChange(pGetCon, pSetSlt, pDrv, res, Level);
#endif
                        pSetSlt->ProcessConnects(pItemListHead, pItemList, Chng, WriteThru, FromOutput, ForceSet, Level+1);
                        }
                      else
                        {
                        long res=pGetCon->m_pCdBlkVar->getL();
                        CDrvSlot * pSetSlt=pGetCon->m_pParentSlot;
                        #if dbgDriverMan
                        if (dbgFeedBack()) dbgpln("%*sFBK GetGC %i %s > %s", (Level*8)+2,"", res, pGetCon->m_sTag(), pSetSlt->sTag());
                        #endif
#if DELAYPROCCONNSETS
                        flag Chng=AddProcessConnItem(pItemListHead, pItemList, false, 0, res, DoingForceSet, pGetCon, pSetSlt, pDrv, Level);
#else
                        flag Chng=pSetSlt->SetVal(res, DoingForceSet);
                        TryAddSltChange(pGetCon, pSetSlt, pDrv, res, Level);
#endif
                        pSetSlt->ProcessConnects(pItemListHead, pItemList, Chng, WriteThru, FromOutput, ForceSet, Level+1);
                        }
                      int xxx=0;
                      }
                    int xxx=0;
                    }
                  else
                    {
                    #if dbgDriverMan
                    if (dbgFeedBack()) dbgpln("%*sFBK Set   %i %s > %s", (Level*8)+2,"", l, sTag(), dbgConnTag);
                    #endif
#if DELAYPROCCONNSETS
                    flag Chng=AddProcessConnItem(pItemListHead, pItemList, false, 0, l, DoingForceSet, pCon, pSetSlt, pDrv, Level);
#else
                    flag Chng=pSetSlt->SetVal(l, DoingForceSet);
                    TryAddSltChange(pCon, pSetSlt, pDrv, l, Level);
#endif
                    pSetSlt->ProcessConnects(pItemListHead, pItemList, Chng, WriteThru, FromOutput, ForceSet, Level+1);
                    }
                  Done = 1;
                  }
                }


              if (Done)
                {
                if (pSetCB)
                  {
                  //ASSERT_ALWAYS(TRUE, "Delays for CodeBlks not yet implemented")
                  }
                else
                  FeedBack = ((flag)(pSetSlt->m_bReqdByExec) && DlyTime!=InfiniteDelay);
                }
              else
                {
                FeedBack = 0;
                if (DlyTime!=InfiniteDelay)
                  {
                  if (pSetCB)
                    {
                    ASSERT_ALWAYS(TRUE, "Delays for CodeBlks not yet implemented")
                    }
                
                  // pDrv->Lock();
                  if (pCon->bDelayLock)
                    {//update delay item in linked list...

                    pCDelayItem pDelayItem = pSetSlt->GetDelayItemsForSlot();
                    while (pDelayItem)
                      {
                      if (pDelayItem->pCon==pCon)
                        {
                        //pDelayItem->dwDelayTime = DlyTime;
                        pDelayItem->Data.Set(Data);
                        break;
                        }
                      pDelayItem = pDelayItem->pNxt;
                      }
                    }
                  else
                    {//add delay item to linked list...
                    #if dbgDriverMan
                    if (dbgFeedBack()) dbgpln("%*sFBK SetDelay %s", (Level*8)+2,"", dbgConnTag);
                    #endif
                    pCon->bDelayLock = 1;
                    pSetSlt->InsertDelayItem(new CDelayItem(pCon, Data, DlyTime));
                    }
  //                pDrv->UnLock();
                  }
                }
              }
            else
              {//check if the external flowsheet tag must be set ...
              double d = Data.GetDouble();
              if (!IsFloatData(iTyp))
                {
                DWORD DlyTime = (SetDelayZero ? 0 : pCon->dwDelayTime);
                if (pCon->dwDelayTime==InfiniteDelay)
                  DlyTime = pCon->dwDelayTime;
                long l = Data.GetLong();
                if (pCon->bUseDelay2 && l && (!SetDelayZero || pCon->dwDelayTime2==InfiniteDelay))
                  DlyTime = pCon->dwDelayTime2;
                if (DlyTime!=InfiniteDelay)
                  TryAddSltChange(pCon, pSetSlt, pDrv, d, Level);
                }
              else
                TryAddSltChange(pCon, pSetSlt, pDrv, d, Level);
              }
            }
          }
        pCon = pCon->m_pNxt;
        }
      }
    m_bProcessConnectsBusy--;
    }
#if DELAYPROCCONNSETS
  if (Level==0)
    ApplyProcessConnItems(pItemListHead);
#endif
  }

//===========================================================================

CDriverManagerBase::CDriverManagerBase()
  {
  }

//===========================================================================
