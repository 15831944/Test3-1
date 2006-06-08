#include "stdafx.h"

#include "sc_defs.h"
#include "flwnode.h"
#define __BLOCKEVALUATOR_CPP
#include "BlockEvaluator.h"

#if WITHBLOCKEVALUATOR


#define DISABLESIMULTANEOUS   01

//=========================================================================
//
//
//
//=========================================================================

XID xidNMakeups       = EvalBlkXID(1);
XID xidNBleeds        = EvalBlkXID(2);

CBlockEvaluator::CBlockEvaluator(FlwNode * pThis,
                                 CReactionBase * pRB,
                                 CHXBase *pHX,
                                 CEnvironHXBase * pEHX,
                                 CVLEBase * pVLE,
                                 CEvapBase * pEvap)
  {
  m_pThis     = pThis;
  m_nBlocks   = 0;

  m_pRB   = pRB;
  m_pHX   = pHX;
  m_pEHX  = pEHX;
  m_pVLE  = pVLE;
  m_pEvap = pEvap;

  AddBlk(m_pRB, 2);
  AddBlk(m_pHX, 4); 
  AddBlk(m_pEHX, 6); 
  AddBlk(m_pVLE, 8); 
  AddBlk(m_pEvap, 10); 

  SortBlocks();
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

void CBlockEvaluator::AddBlk(CBlockEvalBase *p, int DefSeqNo)
  {
  if (p)
    {
    dbgpln("AddBlk %3i", m_nBlocks);
    p->SetOnOffValLst(&m_OnOffValLst);
    p->SetDefBlkSeqNo(DefSeqNo);
    m_Blks[m_nBlocks++]=p;
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
        dbgpln("RemBlk %3i", m_nBlocks);
        break;
        }
      }
    }
  }

//-------------------------------------------------------------------------

void CBlockEvaluator::Add_OnOff(DataDefnBlk &DDB, DDBPages PageIs)
  {
  BOOL DoIt=true;
  if (PrjFileVerNo()>=98)
    DoIt=DDB.BeginObject(m_pThis, "EB", "EB_Slct", 0, DDB_NoPage);
  
  if (DoIt)
    {
    DDB.String("EvaluationSequence", "EvalSeq", DC_, "", &m_sBlkSeq, m_pThis, 0); 
    DDB.Long("Makeups", "", DC_, "", xidNMakeups, m_pThis, isParmStopped|SetOnChange); 
    DDB.Long("Bleeds",  "", DC_, "", xidNBleeds,  m_pThis, isParmStopped|SetOnChange); 

    Strng S;
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

    //DDB.Text("");
    for (int a=0; a<m_pMakeups.GetSize(); a++)
      m_pMakeups[a]->Add_OnOff(DDB, isParmStopped|SetOnChange, 100000+a*1000);
    for (int a=0; a<m_pBleeds.GetSize(); a++)
      m_pBleeds[a]->Add_OnOff(DDB, isParmStopped|SetOnChange, 200000+a*1000);
    }

  if (PrjFileVerNo()>=98)
    DDB.EndObject();

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
      m_pMakeups[a]->BuildDataDefn(DDB, /*S()*/NULL, NULL, TheFirst?DDB_RqdPage:DDB_OptPage, 100000+a*1000);
      TheFirst=false;
      }
    }
  for (int a=0; a<m_pBleeds.GetSize(); a++)
    {
    if (m_pBleeds[a]->Enabled())
      {
      m_pBleeds[a]->BuildDataDefn(DDB, /*S()*/NULL, NULL, TheFirst?DDB_RqdPage:DDB_OptPage, 200000+a*1000);
      TheFirst=false;
      }
    }
  };

//-------------------------------------------------------------------------

flag CBlockEvaluator::DataXchg(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
    case xidNMakeups:
      if (DCB.rL)
        {
        int N=*DCB.rL;
        N=Range(0, N, MaxMakeupBlocks); 
        for (int a=N; a<m_pMakeups.GetSize(); a++)
          {
          RemBlk(m_pMakeups[a]);
          delete m_pMakeups[a];
          }
        int NOld=m_pMakeups.GetSize();
        m_pMakeups.SetSize(N);
        for (int a=NOld; a<N; a++)
          {
          m_pMakeups[a] = new CMakeupBase(m_pThis, a);
          m_pMakeups[a]->Enable();
          AddBlk(m_pMakeups[a], 1+a*2);
          }
        }
      DCB.L=m_pMakeups.GetSize();
      return 1;
    case xidNBleeds:
      if (DCB.rL)
        {
        int N=*DCB.rL;
        N=Range(0, N, MaxBleedBlocks); 
        for (int a=N; a<m_pBleeds.GetSize(); a++)
          {
          RemBlk(m_pBleeds[a]);
          delete m_pBleeds[a];
          }
        int NOld=m_pBleeds.GetSize();
        m_pBleeds.SetSize(N);
        for (int a=NOld; a<N; a++)
          {
          m_pBleeds[a] = new CBleedBase(m_pThis, a);
          m_pBleeds[a]->Enable();
          AddBlk(m_pBleeds[a], 1+a*2);
          }
        }
      DCB.L=m_pBleeds.GetSize();
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

flag CBlockEvaluator::ValidateData(ValidateDataBlk & VDB)
  {
  SortBlocks();

  return true;
  };

//-------------------------------------------------------------------------

CFlange * CBlockEvaluator::GetFlange(int IOId)
  {
  CFlange * pFlng=NULL;
  if (IOId>=IOId_Bleed2Area && IOId<IOId_Bleed2Area+CBlockEvaluator::MaxBleedBlocks)
    pFlng = &(m_pBleeds[IOId-IOId_Bleed2Area]->m_Out.Flange);
  if (IOId>=IOId_Makeup2Area && IOId<IOId_Makeup2Area+CBlockEvaluator::MaxMakeupBlocks)
    pFlng = &(m_pMakeups[IOId-IOId_Makeup2Area]->m_In.Flange);
  dbgpln("CBlockEvaluator::GetFlange %3i %08x", IOId, pFlng);
  return pFlng;
  }

//-------------------------------------------------------------------------

void CBlockEvaluator::SortBlocks()
  {
  CBlockEvalBase::BuildOnOffValLst(&m_OnOffValLst, m_nBlocks);

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
    dbgpln("SortBlks ===============");
    for (i=0 ; i<m_nBlocks; i++)
      dbgpln("  Seq: %-6s %5i", m_Blks[i]->Name(), m_Blks[i]->BlkSeqNo(true));
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

  m_sBlkSeq="";
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
      if (m_sBlkSeq.GetLength()>0)
        m_sBlkSeq+=".";
      m_sBlkSeq+="[";
      m_sBlkSeq+=S;
      m_sBlkSeq+="]";
      }
    else
      {
      if (m_sBlkSeq.GetLength()>0)
        m_sBlkSeq+=".";
      m_sBlkSeq+=S;
      }
    }

  for (  ; i<m_nBlocks && m_Blks[i]->BlkSeqNo()<255; i++)
    {
    if (m_sBlkSeq.GetLength()>0)
      m_sBlkSeq+=".";
    m_sBlkSeq+=m_Blks[i]->Name();
    }

  int xxx=0;
  }

//-------------------------------------------------------------------------

void CBlockEvaluator::EvalProducts(SpConduit & Fo, double Po, CFlwThermalBlk * pFTB, double FinalTEst)
  {
  for (int i=0; i<m_nBlocks ; i++)
    {
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
        m_pVLE->QPFlash(Fo, Po, 0.0, VLEF_Null);
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
      }
    //      }
    }
  };

//-------------------------------------------------------------------------

void CBlockEvaluator::EvalProductsPipe(SpConduit & Fo, double Len, double Diam, double Po, CFlwThermalBlk * pFTB, double FinalTEst)
  {
  for (int i=0; i<m_nBlocks; i++)
    {
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
        m_pEHX->EvalProductsPipe(Fo, Len, Diam);
        if (pFTB)
          pFTB->AddEHXEnd();
        break;

      case BEId_VLE:   
        if (pFTB)
          pFTB->AddVLEBegin();
        m_pVLE->QPFlash(Fo, Po, 0.0, VLEF_Null);
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
        //if (pFTB)
        //  pFTB->AddEvapBegin();
        m_pMakeups[m_Blks[i]->Index()]->EvalProducts(Fo, Po); 
        //if (pFTB)
        //  pFTB->AddEvapEnd();
        break;
      case BEId_Bleed:  
        //if (pFTB)
        //  pFTB->AddEvapBegin();
        m_pBleeds[m_Blks[i]->Index()]->EvalProducts(Fo, Po); 
        //m_Blks[i]->EvalProducts(Fo, Po); 
        //if (pFTB)
        //  pFTB->AddEvapEnd();
        break;

      }
    //}
    }
  };

//=========================================================================
//
//
//
//=========================================================================

#endif