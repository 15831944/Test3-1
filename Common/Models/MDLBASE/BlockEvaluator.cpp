#include "stdafx.h"

#include "sc_defs.h"
#include "flwnode.h"
#define __BLOCKEVALUATOR_CPP
#include "BlockEvaluator.h"
#include "m_link.h"
//#include "optoff.h"

#define dbgBlkEvalProd 0

#define DISABLESIMULTANEOUS   01

//=========================================================================
//
//
//
//=========================================================================

XID xidNMakeups       = EvalBlkXID(1);
XID xidNBleeds        = EvalBlkXID(2);
XID xidOldBlkSeq      = EvalBlkXID(3);

CBlockEvaluator::CBlockEvaluator(FlwNode * pNd,
                                 DWORD Options,
                                 CReactionBase * pRB,
                                 CHXBase *pHX,
                                 CEnvironHXBase * pEHX,
                                 CVLEBase * pVLE,
                                 CEvapBase * pEvap) :
  CBlockEvalStateSemantics(false)
  {
  m_pNd             = pNd;
  m_Options         = Options;
  //m_bAllowStateSemantics = false;
  
  Attach((Options & BEO_StateSemantics)!=0, pRB, pHX, pEHX, pVLE, pEvap);
  };

//-------------------------------------------------------------------------

CBlockEvaluator::~CBlockEvaluator(void)
  {
  RemBlk(m_pRB);
  RemBlk(m_pHX); 
  RemBlk(m_pEHX); 
  RemBlk(m_pVLE); 
  RemBlk(m_pEvap); 

  for (int a=0; a<m_pMakeups.GetSize(); a++)
    {
    RemBlk(m_pMakeups[a]);
    delete m_pMakeups[a];
    }
  for (int a=0; a<m_pBleeds.GetSize(); a++)
    {
    RemBlk(m_pBleeds[a]);
    delete m_pBleeds[a];
    }
  }

//-------------------------------------------------------------------------

bool CBlockEvaluator::StateSemanticsOn()
  {
  return m_bAllowStateSemantics && m_pNd->SolveBufferedMethod(); 
  };

//-------------------------------------------------------------------------

void CBlockEvaluator::Attach(bool AllowStateSemantics,
                             CReactionBase * pRB,
                             CHXBase *pHX,
                             CEnvironHXBase * pEHX,
                             CVLEBase * pVLE,
                             CEvapBase * pEvap)
  {
  m_nBlocks   = 0;
  m_bAllowStateSemantics = AllowStateSemantics;

  m_pRB   = pRB;
  m_pHX   = pHX;
  m_pEHX  = pEHX;
  m_pVLE  = pVLE;
  m_pEvap = pEvap;

  AddBlk(m_pRB, MaxNdMakeups+2);
  AddBlk(m_pHX, MaxNdMakeups+3); 
  AddBlk(m_pEHX, MaxNdMakeups+4); 
  AddBlk(m_pVLE, MaxNdMakeups+5); 
  AddBlk(m_pEvap, MaxNdMakeups+6); 

  m_nMaxNdMakeups  = MaxNdMakeups;

  SortBlocks();
  };

//-------------------------------------------------------------------------

void CBlockEvaluator::SetEnable(bool On)
  {
  for (int a=0; a<m_pMakeups.GetSize(); a++)
    m_pMakeups[a]->SetEnable(On);

  for (int a=0; a<m_pBleeds.GetSize(); a++)
    m_pBleeds[a]->SetEnable(On);
  };

//-------------------------------------------------------------------------

void CBlockEvaluator::AddBlk(CBlockEvalBase *p, int DefSeqNo)
  {
  if (p)
    {
    p->m_pBlkStateSemantics=this;
    p->SetOnOffValLst(p->StateSemanticsOn() ? &m_OnOffStateValLst : &m_OnOffValLst);
    p->SetDefBlkSeqNo(DefSeqNo);
    m_Blks[m_nBlocks++]=p;
    if (0)
      dbgpln("AddBlk %3i %s %s", m_nBlocks, p->StateSemanticsOn()?"On":"  ", m_pNd->FullObjTag());
    }
  };

//-------------------------------------------------------------------------

void CBlockEvaluator::RemBlk(CBlockEvalBase *p)
  {
  if (p)
    {
    for (int i=0; i<m_nBlocks; i++)
      {
      if (m_Blks[i]==p)
        {
        m_nBlocks--;
        for (int j=i; j<m_nBlocks; j++)
          m_Blks[j]=m_Blks[j+1];
        if (0)
          dbgpln("RemBlk %3i %s", m_nBlocks, m_pNd->FullObjTag());
        break;
        }
      }
    }
  }

//-------------------------------------------------------------------------

void CBlockEvaluator::Add_OnOff(DataDefnBlk &DDB, DDBPages PageIs)
  {
  BOOL DoIt=true;
  if (PrjFileVerNo()>=99)
    DoIt=DDB.BeginObject(m_pNd, "EB", "EB_Slct", 0, DDB_NoPage);
  
  if (DoIt)
    {
    DDB.String("", "EvalSeq.Feed", DC_, "", &m_sBlkSeqFeed, m_pNd, 0); 
    if (StateSemanticsOn())
      DDB.String("", "EvalSeq.Content", DC_, "", &m_sBlkSeqState, m_pNd, 0); 
    if (m_Options&BEO_WithMakeups)
      DDB.Long("Makeups", "", DC_, "", xidNMakeups, m_pNd, isParmStopped|SetOnChange); 
    if (m_Options&BEO_WithBleeds)
      DDB.Long("Bleeds",  "", DC_, "", xidNBleeds,  m_pNd, isParmStopped|SetOnChange); 

    //DDB.Text("");
    if (m_Options&BEO_WithMakeups)
      {
      for (int a=0; a<m_pMakeups.GetSize(); a++)
        m_pMakeups[a]->Add_OnOff(DDB, isParmStopped|SetOnChange, 100000+a*1000);
      }

    if (m_pRB)
      m_pRB->Add_OnOff(DDB, isParmStopped|SetOnChange);
    if (m_pHX)
      m_pHX->Add_OnOff(DDB, isParmStopped|SetOnChange);
    if (m_pEHX)
      m_pEHX->Add_OnOff(DDB, isParmStopped|SetOnChange);
    if (m_pVLE)
      m_pVLE->Add_OnOff(DDB, isParmStopped|SetOnChange);
    if (m_pEvap)
      m_pEvap->Add_OnOff(DDB, isParmStopped|SetOnChange);

    if (m_Options&BEO_WithBleeds)
      {
      for (int a=0; a<m_pBleeds.GetSize(); a++)
        m_pBleeds[a]->Add_OnOff(DDB, isParmStopped|SetOnChange, 200000+a*1000);
      }
    }

  if (PrjFileVerNo()>=99)
    DDB.EndObject();

  if (PrjFileVerNo()<99 && (DDB.DoingPutData() && DDB.ForFileSnpScn() || 0))
    {                      
    static DDBValueLst DDBBS[]={
      {0, "RB EHX VLE" },
      {1, "EHX RB VLE" },
      {0}};
    DDB.Byte("BlockActionSeq",         "",   DC_,  "",  xidOldBlkSeq, m_pNd, isParmStopped, DDBBS);
    }

  DDB.Text("");
  };

//-------------------------------------------------------------------------

void CBlockEvaluator::BuildDataDefn(DataDefnBlk &DDB, DDBPages PageIs)
  {

  if (m_pRB && m_pRB->Enabled())
    m_pRB->BuildDataDefn(DDB);
  if (m_pHX && m_pHX->Enabled())
    m_pHX->BuildDataDefn(DDB);
  if (m_pEHX && m_pEHX->Enabled())
    m_pEHX->BuildDataDefn(DDB);
  if (m_pVLE && m_pVLE->Enabled())
    m_pVLE->BuildDataDefn(DDB);
  if (m_pEvap && m_pEvap->Enabled())
    m_pEvap->BuildDataDefn(DDB);
  
  bool TheFirst=true;
  for (int a=0; a<m_pMakeups.GetSize(); a++)
    {
    if (m_pMakeups[a]->Enabled())
      {
      m_pMakeups[a]->BuildDataDefn(DDB, /*S()*/NULL, NULL, TheFirst||1?DDB_RqdPage:DDB_OptPage, 100000+a*1000);
      TheFirst=false;
      }
    }
  for (int a=0; a<m_pBleeds.GetSize(); a++)
    {
    if (m_pBleeds[a]->Enabled())
      {
      m_pBleeds[a]->BuildDataDefn(DDB, /*S()*/NULL, NULL, TheFirst||1?DDB_RqdPage:DDB_OptPage, 200000+a*1000);
      TheFirst=false;
      }
    }
  };

 //-------------------------------------------------------------------------

void CBlockEvaluator::SetMakeupCount(int N, bool MakeConnects)
   {
   if (N!=m_pMakeups.GetSize())
     {
     N=Range(0, N, m_nMaxNdMakeups); 
     for (int a=N; a<m_pMakeups.GetSize(); a++)
       {
       //m_pMakeups[a]->SetEnable(false);
       m_pNd->DoDirectDisConnect(&m_pMakeups[a]->m_SrcIO);// m_pMakeups[a]->m_SrcIO.DoDisConnect();
       //m_pMakeups[a]->m_SrcIO.ManageNdDirectIOs(false);
       RemBlk(m_pMakeups[a]);
       delete m_pMakeups[a];
       }
     int NOld=m_pMakeups.GetSize();
     m_pMakeups.SetSize(N);
     for (int a=NOld; a<N; a++)
       {
       Strng Tg, Nm;
       Tg.Set("%s%i", MakeupIOTag, a+1);
       m_pMakeups[a] = new CMakeupBase(m_pNd, a, Tg());
       m_pMakeups[a]->Open(1);
       m_pMakeups[a]->SetEnable(true);
       if (MakeConnects)
         m_pNd->DoDirectConnect(&m_pMakeups[a]->m_SrcIO);// m_pMakeups[a]->m_SrcIO.DoDisConnect();
       AddBlk(m_pMakeups[a], 1+a);
       }
     SortBlocks();
     }
   }

//-------------------------------------------------------------------------

void CBlockEvaluator::SetBleedCount(int N, bool MakeConnects)
  {
  if (N!=m_pBleeds.GetSize())
    {
    N=Range(0, N, MaxNdBleeds); 
    for (int a=N; a<m_pBleeds.GetSize(); a++)
      {
      m_pNd->DoDirectDisConnect(&m_pBleeds[a]->m_SnkIO);// m_pMakeups[a]->m_SrcIO.DoDisConnect();
      RemBlk(m_pBleeds[a]);
      delete m_pBleeds[a];
      }
    int NOld=m_pBleeds.GetSize();
    m_pBleeds.SetSize(N);
    for (int a=NOld; a<N; a++)
      {
      Strng Tg, Nm;
      Tg.Set("%s%i", BleedIOTag, a+1);
      m_pBleeds[a] = new CBleedBase(m_pNd, a, Tg());
      m_pBleeds[a]->Open(1);
      m_pBleeds[a]->SetEnable(true);
      if (MakeConnects)
        m_pNd->DoDirectConnect(&m_pBleeds[a]->m_SnkIO);// m_pMakeups[a]->m_SrcIO.DoDisConnect();
      AddBlk(m_pBleeds[a], MaxNdMakeups*2+a);
      }
    SortBlocks();
    }
  }

//-------------------------------------------------------------------------

flag CBlockEvaluator::DataXchg(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
    case xidNMakeups:
      if (DCB.rL)
        SetMakeupCount(*DCB.rL, !DCB.ForFileSnpScn());
      DCB.L=m_pMakeups.GetSize();
      return 1;
    case xidNBleeds:
      if (DCB.rL)
        SetBleedCount(*DCB.rL, !DCB.ForFileSnpScn());
      DCB.L=m_pBleeds.GetSize();
      return 1;

    case xidOldBlkSeq:
      if (DCB.rB)
        {
        switch (*DCB.rB)
          {
          case 0:// "RB EHX VLE"
            m_pRB->SetBlkSeqNo(2);
            m_pEHX->SetBlkSeqNo(3);
            m_pVLE->SetBlkSeqNo(4);
            break;
          case 1:// "EHX RB VLE"
            m_pEHX->SetBlkSeqNo(2);
            m_pRB->SetBlkSeqNo(3);
            m_pVLE->SetBlkSeqNo(4);
            break;
          }
        }
      DCB.pC="????";
      return 1;
    }
  if (m_pRB && m_pRB->DataXchg(DCB))
    return 1;
  if (m_pHX && m_pHX->DataXchg(DCB))
    return 1;
  if (m_pEHX && m_pEHX->DataXchg(DCB))
    return 1;
  if (m_pVLE && m_pVLE->DataXchg(DCB))
    return 1;
  if (m_pEvap && m_pEvap->DataXchg(DCB))
    return 1;
  for (int a=0; a<m_pMakeups.GetSize(); a++)
    {
    if (DCB.dwUserInfo>=(dword)100000+a*1000 && DCB.dwUserInfo<(dword)100000+a*1000+1000)
      {
      if (m_pMakeups[a]->DataXchg(DCB))
        return 1;
      }
    }
  for (int a=0; a<m_pBleeds.GetSize(); a++)
    {
    if (DCB.dwUserInfo>=(dword)200000+a*1000 && DCB.dwUserInfo<(dword)200000+a*1000+1000)
      {
      if (m_pBleeds[a]->DataXchg(DCB))
        return 1;
      }
    }
  return 0;
  };

//-------------------------------------------------------------------------

flag CBlockEvaluator::ValidateData(ValidateDataBlk & VDB, bool AllowStateSemantics)
  {
  m_bAllowStateSemantics = AllowStateSemantics;
  SortBlocks();

  flag OK=1;
  for (int a=0; a<m_pMakeups.GetSize(); a++)
    {
    if (!m_pMakeups[a]->ValidateData(VDB))
      OK=0;
    }
  for (int a=0; a<m_pBleeds.GetSize(); a++)
    {
    if (!m_pBleeds[a]->ValidateData(VDB))
      OK=0;
    }

  return OK;
  };

//-------------------------------------------------------------------------

static const LPTSTR SeqNames[] =
  {
  //"On (Priority 1)",
  //"On (Priority 2)",
  //"On (Priority 3)",
  //"On (Priority 4)",
  //"On (Priority 5)",
  //"On (Priority 6)",
  "On-1",
  "On-2",
  "On-3",
  "On-4",
  "On-5",
  "On-6",
  "On-7",
  "On-8",
  "On-9",
  "On-10",
  "On-11",
  "On-12",
  };


void CBlockEvaluator::BuildOnOffValLst(DDBValueLstMem  * ValLst, int NInSequence, LPCSTR StateName)
  {
  ValLst->Empty();
  ValLst->Add(BlkEval_Off, "Off");
  ValLst->Add(BlkEval_On, "On");
  for (int i=0; i<NInSequence; i++)
    ValLst->Add(BlkEval_First+i, SeqNames[i]);
  if (StateName && StateSemanticsOn())
    ValLst->Add(BlkEval_State, Strng("On-", StateName)());
  };

//-------------------------------------------------------------------------

//CFlange * CBlockEvaluator::GetFlange(int IOId)
//  {
//  CFlange * pFlng=NULL;
//  if (IOId>=IOId_Bleed2Area && IOId<IOId_Bleed2Area+CBlockEvaluator::MaxNdBleeds)
//    pFlng = &(m_pBleeds[IOId-IOId_Bleed2Area]->m_SnkIO.Flange);
//  if (IOId>=IOId_Makeup2Area && IOId<IOId_Makeup2Area+CBlockEvaluator::MaxNdMakeups)
//    pFlng = &(m_pMakeups[IOId-IOId_Makeup2Area]->m_SrcIO.Flange);
//  if (0)
//    dbgpln("CBlockEvaluator::GetFlange %3i %08x", IOId, pFlng);
//  return pFlng;
//  }

//-------------------------------------------------------------------------

void CBlockEvaluator::SortBlocks()
  {
  BuildOnOffValLst(&m_OnOffValLst, m_nBlocks, NULL);
  BuildOnOffValLst(&m_OnOffStateValLst, m_nBlocks, "Content");
  
  for (int b=0 ; b<m_nBlocks; b++)
    {
    m_Blks[b]->SetOnOffValLst(m_Blks[b]->StateSemanticsOn() ? &m_OnOffStateValLst : &m_OnOffValLst);
    if (0)
      {
      dbgpln("SetOnOff =============== %i %s %s", b, m_Blks[b]->StateSemanticsOn()?"On":"  ", m_pNd->FullObjTag());
      }
    }

  int i;
  for (i=1 ; i<m_nBlocks; )
    {
    if (m_Blks[i-1]->BlkSeqNo(true)>m_Blks[i]->BlkSeqNo(true) ||
        (m_Blks[i-1]->BlkSeqNo(true)==m_Blks[i]->BlkSeqNo(true) && m_Blks[i-1]->DefBlkSeqNo()>m_Blks[i]->DefBlkSeqNo()))
      {
      // swap
      Exchange(m_Blks[i-1], m_Blks[i]);
      if (i>1)
        i--;
      }
    else
      i++;
    };

  if (0)
    {
    dbgpln("SortBlks =============== %s", m_pNd->FullObjTag());
    for (i=0 ; i<m_nBlocks; i++)
      dbgpln("  Seq: %-6s %i %s %5i", m_Blks[i]->Name(), m_Blks[i]->OpenStatus(m_Blks[i]->Enabled()), m_Blks[i]->Enabled()?"En":"  ", m_Blks[i]->BlkSeqNo(true));
    }

#if DISABLESIMULTANEOUS
  if (1)
    {
    i=0;
    for (; i<m_nBlocks && m_Blks[i]->BlkSeqNo()<BlkEval_On; )
      {
      int N=1;
      int iSeq=m_Blks[i]->BlkSeqNo();
      i++;

      if (i<m_nBlocks && m_Blks[i]->BlkSeqNo()==iSeq)
        {
        for (int j=i; j<m_nBlocks; j++)
          {
          m_Blks[j]->SetBlkSeqNo(m_Blks[j]->BlkSeqNo()+1);
          if (j<m_nBlocks-1 && m_Blks[j]->BlkSeqNo()<m_Blks[j+1]->BlkSeqNo()) 
            break;
          }
        }
      }
    }
#endif

  if (0)
    {
    dbgpln("         ===============");
    for (i=0 ; i<m_nBlocks; i++)
      dbgpln("  Seq: %-6s %5i", m_Blks[i]->Name(), m_Blks[i]->BlkSeqNo(true));
    dbgpln("         ===============");
    }

  bool AddCommaFeed=false;
  bool AddCommaState=false;
  m_sBlkSeqFeed="";
  m_sBlkSeqState="";
  i=0;
  for (; i<m_nBlocks&& m_Blks[i]->BlkSeqNo(true)<BlkEval_On; )
    {
    Strng S(m_Blks[i]->Name());
    int N=1;
    int iSeq=m_Blks[i]->BlkSeqNo(true);
    i++;

    for (  ; i<m_nBlocks && m_Blks[i]->BlkSeqNo(true)==iSeq; i++)
      {
      N++;
      S+=",";
      S+=m_Blks[i]->Name();
      }
    if (N>1)
      {
      if (m_sBlkSeqFeed.GetLength()>0)
        m_sBlkSeqFeed+=".";
      m_sBlkSeqFeed+="[";
      m_sBlkSeqFeed+=S;
      m_sBlkSeqFeed+="]";
      }
    else
      {
      if (AddCommaFeed)
        m_sBlkSeqFeed+=".";
      m_sBlkSeqFeed+=S;
      AddCommaFeed=true;
      }
    }

  for (  ; i<m_nBlocks && m_Blks[i]->BlkSeqNo()<255; i++)
    {
    if (m_Blks[i]->BlkSeqNo()>=BlkEval_State)
      {
      if (AddCommaState)
        m_sBlkSeqState+=".";
      m_sBlkSeqState+=m_Blks[i]->Name();
      AddCommaState=true;
      }
    else
      {
      if (AddCommaFeed)
        m_sBlkSeqFeed+=".";
      m_sBlkSeqFeed+=m_Blks[i]->Name();
      AddCommaFeed=true;
      }
    }
  }

//-------------------------------------------------------------------------

void CBlockEvaluator::CheckFlowsConsistent(int iJoinNo, bool ForMakeup)
  {
  if (iJoinNo>=0)
    {
    CJoinRec & J = m_pNd->Joins[iJoinNo];

    for (int c=0; c<J.NConns(); c++)
      {
      int io = J.Conn(c).m_iIONo;
      int ioIdRmt = m_pNd->IOId_Rmt(io);
      FlwNode * pRmt = m_pNd->Nd_Rmt(io);
      // check that the remote 'node' is a link
      if (dynamic_cast<MN_Lnk*>(pRmt)!=NULL)
        {
        bool LinkDrawnToHere = (ioIdRmt>0);
        double QmIn = m_pNd->IOQm_In(io);
        //if ((LinkDrawnToHere && (QmIn < SmallPosFlow)) || (!LinkDrawnToHere && (QmIn > SmallPosFlow)))
        //this gives an error message when the flow is exactly 0.0!!!
        if ((LinkDrawnToHere && (QmIn < (SmallPosFlow*-1.0))) || (!LinkDrawnToHere && (QmIn > SmallPosFlow)))
          LogError(m_pNd->FullObjTag(), 0, "%s: Bad flow direction @ %s", ForMakeup?"MakeUp":"Bleed", pRmt->FullObjTag());
        }
      }
    }
  }

//-------------------------------------------------------------------------

double CBlockEvaluator::Duty()
  {
  double D=0.0;
  for (int i=0; i<m_nBlocks ; i++)
    {
    switch (m_Blks[i]->BEId())
      {
      case BEId_RB:    
      case BEId_HX:    
      case BEId_EHX:  
      case BEId_VLE:   
      case BEId_Evap:  
        break;

      case BEId_Makeup:  
        D+=m_pMakeups[m_Blks[i]->Index()]->Duty(); 
        break;
      case BEId_Bleed:  
        //D+=m_pBleeds[m_Blks[i]->Index()]->Duty(); 
        break;

      }
    //      }
    }
  return D;
  }

//-------------------------------------------------------------------------

int CBlockEvaluator::EnabledCount()
  {
  int Cnt=0;
  for (int i=0; i<m_nBlocks ; i++)
    {
    int iSeq=m_Blks[i]->BlkSeqNo();
    if (iSeq>=BlkEval_State)
      continue;
    switch (m_Blks[i]->BEId())
      {
      case BEId_RB:    
        if (m_pRB->Enabled())
          Cnt++;
        break;
      case BEId_HX:    
        if (m_pHX->Enabled())
          Cnt++;
        break;
      case BEId_EHX:  
        if (m_pEHX->Enabled())
          Cnt++;
        break;
      case BEId_VLE:   
        if (m_pVLE->Enabled())
          Cnt++;
        break;
      case BEId_Evap:  
        if (m_pEvap->Enabled())
          Cnt++;
        break;
      case BEId_Makeup:  
        if (m_pMakeups[m_Blks[i]->Index()]->Enabled())
          Cnt++;
        break;
      case BEId_Bleed:  
        if (m_pBleeds[m_Blks[i]->Index()]->Enabled())
          Cnt++;
        break;
      }
    }
  return Cnt;
  };

//-------------------------------------------------------------------------

void CBlockEvaluator::EvalProducts(int iJoinNo, SpConduit & Fo, double Po, CFlwThermalBlk * pFTB, double FinalTEst)
  {
  if (dbgBlkEvalProd && m_nBlocks>0)
    dbgpln("CBlockEvaluator::EvalProducts >> Qm:%10.3f %s", Fo.QMass(), m_pNd->Tag());
  for (int i=0; i<m_nBlocks ; i++)
    {
    int iSeq=m_Blks[i]->BlkSeqNo();
    if (iSeq>=BlkEval_State)
      continue;
    switch (m_Blks[i]->BEId())
      {
      case BEId_RB:    
        if (pFTB)
          pFTB->AddRBBegin();
        m_pRB->EvalProducts(Fo);
        if (pFTB)
          pFTB->AddRBEnd();
        break;

      case BEId_HX:    
        __debugbreak(); // this needs thought
        //FTB.AddHXBegin();
        m_pHX->EvalProducts(); 
        //FTB.AddHXEnd();
        break;

      case BEId_EHX:  
        if (pFTB)
          pFTB->AddEHXBegin();
        m_pEHX->EvalProducts(Fo);
        if (pFTB)
          pFTB->AddEHXEnd();
        break;

      case BEId_VLE:   
        if (pFTB)
          pFTB->AddVLEBegin();
        Fo.SetVLEBlk(m_pVLE);
        m_pVLE->PFlash(Fo, Po, 0.0, VLEF_Null);
        if (pFTB)
          pFTB->AddVLEEnd();
        break;

      case BEId_Evap:  
        if (pFTB)
          pFTB->AddEvapBegin();
        m_pEvap->EvalProducts(Fo, Po); 
        if (pFTB) 
          pFTB->AddEvapEnd();
        break;

      case BEId_Makeup:  
        CheckFlowsConsistent(iJoinNo, true);
        m_pMakeups[m_Blks[i]->Index()]->EvalProducts(Fo, Po); 
        break;

      case BEId_Bleed:  
        CheckFlowsConsistent(iJoinNo, false);
        m_pBleeds[m_Blks[i]->Index()]->EvalProducts(Fo, Po); 
        break;
      }
    }
  if (dbgBlkEvalProd && m_nBlocks>0)
    dbgpln("                              << Qm:%10.3f", Fo.QMass());
  };

//-------------------------------------------------------------------------

void CBlockEvaluator::EvalProductsInline(int iJoinNo, SpConduit & Fo, double Len, double Diam, double Po, CFlwThermalBlk * pFTB, double FinalTEst)
  {
  if (dbgBlkEvalProd && m_nBlocks>0)
    dbgpln("CBlockEvaluator::EvalProductsInline >> Qm:%10.3f %s", Fo.QMass(), m_pNd->Tag());
  for (int i=0; i<m_nBlocks; i++)
    {
    int iSeq=m_Blks[i]->BlkSeqNo();
    if (iSeq>=BlkEval_State)
      continue;
    switch (m_Blks[i]->BEId())
      {
      case BEId_RB:    
        if (pFTB)
          pFTB->AddRBBegin();
        m_pRB->EvalProducts(Fo);
        if (pFTB)
          pFTB->AddRBEnd();
        break;

      case BEId_HX:    
        __debugbreak(); // this needs thought
        //FTB.AddHXBegin();
        m_pHX->EvalProducts(); 
        //FTB.AddHXEnd();
        break;

      case BEId_EHX:  
        if (pFTB)
          pFTB->AddEHXBegin();
        m_pEHX->EvalProductsInline(Fo, Len, Diam);
        if (pFTB)
          pFTB->AddEHXEnd();
        break;

      case BEId_VLE:   
        if (pFTB)
          pFTB->AddVLEBegin();
        Fo.SetVLEBlk(m_pVLE);
        m_pVLE->PFlash(Fo, Po, 0.0, VLEF_Null);
        if (pFTB)
          pFTB->AddVLEEnd();
        break;

      case BEId_Evap:  
        if (pFTB)
          pFTB->AddEvapBegin();
        m_pEvap->EvalProducts(Fo, Po); 
        if (pFTB)
          pFTB->AddEvapEnd();
        break;

      case BEId_Makeup:  
        CheckFlowsConsistent(iJoinNo, true);
        m_pMakeups[m_Blks[i]->Index()]->EvalProducts(Fo, Po); 
        break;

      case BEId_Bleed:  
        CheckFlowsConsistent(iJoinNo, false);
        m_pBleeds[m_Blks[i]->Index()]->EvalProducts(Fo, Po); 
        break;
      }
    }
  if (dbgBlkEvalProd && m_nBlocks>0)
    dbgpln("                                  << Qm:%10.3f", Fo.QMass());
  };

//-------------------------------------------------------------------------

int CBlockEvaluator::ChangeTag(char * pOldTag, char * pNewTag)
  { 
  for (int i=0; i<m_nBlocks; i++)
    m_Blks[i]->ChangeTag(pOldTag, pNewTag);
  return EOCT_DONE; 
  };

//-------------------------------------------------------------------------

int CBlockEvaluator::DeleteTag(char * pDelTag)
  {
  for (int i=0; i<m_nBlocks; i++)
    m_Blks[i]->DeleteTag(pDelTag);
  return EODT_DONE;
  };

//=========================================================================
//
//
//
//=========================================================================
