#include "stdafx.h"

#include "sc_defs.h"
#include "flwnode.h"
#define __BLOCKEVALUATOR_CPP
#include "BlockEvaluator.h"

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

CBlockEvaluator::CBlockEvaluator(FlwNode * pThis,
                                 CReactionBase * pRB,
                                 CHXBase *pHX,
                                 CEnvironHXBase * pEHX,
                                 CVLEBase * pVLE,
                                 CEvapBase * pEvap)
  {
  m_pThis     = pThis;
  
  Attach(pRB, pHX, pEHX, pVLE, pEvap);
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

void CBlockEvaluator::Attach(CReactionBase * pRB,
                CHXBase *pHX,
                CEnvironHXBase * pEHX,
                CVLEBase * pVLE,
                CEvapBase * pEvap)
  {
  m_nBlocks   = 0;

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

  //if (m_pRB  )
  //  m_nMaxNdMakeups++;
  //if (m_pHX  ) 
  //  m_nMaxNdMakeups++;
  //if (m_pEHX ) 
  //  m_nMaxNdMakeups++;
  //if (m_pVLE ) 
  //  m_nMaxNdMakeups++;
  //if (m_pEvap) 
  //  m_nMaxNdMakeups++;

  SortBlocks();
  };

//-------------------------------------------------------------------------

void CBlockEvaluator::SetEnable(bool On)
  {
  for (int a=0; a<m_pMakeups.GetSize(); a++)
    m_pMakeups[a]->SetEnable(On);

  //if (m_pRB)
  //  m_pRB->Add_OnOff(DDB, isParmStopped|SetOnChange);
  //if (m_pHX)
  //  m_pHX->Add_OnOff(DDB, isParmStopped|SetOnChange);
  //if (m_pEHX)
  //  m_pEHX->Add_OnOff(DDB, isParmStopped|SetOnChange);
  //if (m_pVLE)
  //  m_pVLE->Add_OnOff(DDB, isParmStopped|SetOnChange);
  //if (m_pEvap)
  //  m_pEvap->Add_OnOff(DDB, isParmStopped|SetOnChange);

  for (int a=0; a<m_pBleeds.GetSize(); a++)
    m_pBleeds[a]->SetEnable(On);
  };

//-------------------------------------------------------------------------

void CBlockEvaluator::AddBlk(CBlockEvalBase *p, int DefSeqNo)
  {
  if (p)
    {
    if (0)
      dbgpln("AddBlk %3i %s", m_nBlocks, m_pThis->FullObjTag());
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
        if (0)
          dbgpln("RemBlk %3i %s", m_nBlocks, m_pThis->FullObjTag());
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
    DoIt=DDB.BeginObject(m_pThis, "EB", "EB_Slct", 0, DDB_NoPage);
  
  if (DoIt)
    {
    DDB.String("EvaluationSequence", "EvalSeq", DC_, "", &m_sBlkSeq, m_pThis, 0); 
    DDB.Long("Makeups", "", DC_, "", xidNMakeups, m_pThis, isParmStopped|SetOnChange); 
    DDB.Long("Bleeds",  "", DC_, "", xidNBleeds,  m_pThis, isParmStopped|SetOnChange); 

    //DDB.Text("");
    for (int a=0; a<m_pMakeups.GetSize(); a++)
      m_pMakeups[a]->Add_OnOff(DDB, isParmStopped|SetOnChange, 100000+a*1000);

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

    for (int a=0; a<m_pBleeds.GetSize(); a++)
      m_pBleeds[a]->Add_OnOff(DDB, isParmStopped|SetOnChange, 200000+a*1000);
    }

  if (PrjFileVerNo()>=99)
    DDB.EndObject();

  if (PrjFileVerNo()<99 && (DDB.DoingPutData() && DDB.ForFileSnpScn() || 0))
    {                      
    static DDBValueLst DDBBS[]={
      {0, "RB EHX VLE" },
      {1, "EHX RB VLE" },
      {0}};
    DDB.Byte("BlockActionSeq",         "",   DC_,  "",  xidOldBlkSeq, m_pThis, isParmStopped, DDBBS);
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

void CBlockEvaluator::SetMakeupCount(int N)
   {
   N=Range(0, N, m_nMaxNdMakeups); 
   for (int a=N; a<m_pMakeups.GetSize(); a++)
     {
     RemBlk(m_pMakeups[a]);
     delete m_pMakeups[a];
     }
   int NOld=m_pMakeups.GetSize();
   m_pMakeups.SetSize(N);
   for (int a=NOld; a<N; a++)
     {
     Strng Tg, Nm;
     Tg.Set("%s%i", MakeupIOTag, a+1);
     m_pMakeups[a] = new CMakeupBase(m_pThis, a, Tg());
     m_pMakeups[a]->Open(1);
     m_pMakeups[a]->SetEnable(true);
     AddBlk(m_pMakeups[a], 1+a);
     }
   }

//-------------------------------------------------------------------------

void CBlockEvaluator::SetBleedCount(int N)
  {
  N=Range(0, N, MaxNdBleeds); 
  for (int a=N; a<m_pBleeds.GetSize(); a++)
    {
    RemBlk(m_pBleeds[a]);
    delete m_pBleeds[a];
    }
  int NOld=m_pBleeds.GetSize();
  m_pBleeds.SetSize(N);
  for (int a=NOld; a<N; a++)
    {
    Strng Tg, Nm;
    Tg.Set("%s%i", BleedIOTag, a+1);
    m_pBleeds[a] = new CBleedBase(m_pThis, a, Tg());
    m_pBleeds[a]->Open(1);
    m_pBleeds[a]->SetEnable(true);
    AddBlk(m_pBleeds[a], MaxNdMakeups*2+a);
    }
  }

//-------------------------------------------------------------------------

flag CBlockEvaluator::DataXchg(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
    case xidNMakeups:
      if (DCB.rL)
        SetMakeupCount(*DCB.rL);
      DCB.L=m_pMakeups.GetSize();
      return 1;
    case xidNBleeds:
      if (DCB.rL)
        SetBleedCount(*DCB.rL);
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

flag CBlockEvaluator::ValidateData(ValidateDataBlk & VDB)
  {
  SortBlocks();

  return true;
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
    dbgpln("SortBlks =============== %s", m_pThis->FullObjTag());
    for (i=0 ; i<m_nBlocks; i++)
      dbgpln("  Seq: %-6s %i %s %5i", m_Blks[i]->Name(), m_Blks[i]->OpenStatus(m_Blks[i]->Enabled()), m_Blks[i]->Enabled()?"En":"Da", m_Blks[i]->BlkSeqNo(true));
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
  if (dbgBlkEvalProd && m_nBlocks>0)
    dbgpln("CBlockEvaluator::EvalProducts >> Qm:%10.3f %s", Fo.QMass(), m_pThis->Tag());
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
    //      }
    }
  if (dbgBlkEvalProd && m_nBlocks>0)
    dbgpln("                              << Qm:%10.3f", Fo.QMass());
  };

//-------------------------------------------------------------------------

void CBlockEvaluator::EvalProductsInline(SpConduit & Fo, double Len, double Diam, double Po, CFlwThermalBlk * pFTB, double FinalTEst)
  {
  if (dbgBlkEvalProd && m_nBlocks>0)
    dbgpln("CBlockEvaluator::EvalProductsInline >> Qm:%10.3f %s", Fo.QMass(), m_pThis->Tag());
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
