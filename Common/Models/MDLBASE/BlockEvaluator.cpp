#include "stdafx.h"

#include "sc_defs.h"
#include "flwnode.h"
#define __BLOCKEVALUATOR_CPP
#include "BlockEvaluator.h"

#if WITHBLOCKEVALUATOR


#define DISABLESIMULTANEOUS   1

//=========================================================================
//
//
//
//=========================================================================

//XID xidEvalSeqRB   = EvalBlkXID(1);
//XID xidEvalSeqHX   = EvalBlkXID(2);
//XID xidEvalSeqEHX  = EvalBlkXID(3);
//XID xidEvalSeqVLE  = EvalBlkXID(4);
//XID xidEvalSeqEvap = EvalBlkXID(5);

static LPTSTR Names[] = 
  {
  "Null",
  "RB",
  "HX",
  "EHX",
  "VLE",
  "Evap",
  };

CBlockEvaluator::CBlockEvaluator(CReactionBase * pRB,
                                CHXBase *pHX,
                                CEnvironHXBase * pEHX,
                                CVLEBase * pVLE,
                                CEvapBase * pEvap)
  {
  m_nBlocks   = 0;
  m_nSequence = 0;

  m_pRB   = pRB;
  m_pHX   = pHX;
  m_pEHX  = pEHX;
  m_pVLE  = pVLE;
  m_pEvap = pEvap;

  m_RBDefSeq   = 2;  
  m_HXDefSeq   = 3; 
  m_EHXDefSeq  = 4; 
  m_VLEDefSeq  = 5; 
  m_EvapDefSeq = 6;

  SortBlocks();

  if (m_pRB) 
    {
    m_pRB->SetOnOffValLst(&m_OnOffValLst);
    m_nBlocks++;
    };
  if (m_pHX) 
    {
    m_pHX->SetOnOffValLst(&m_OnOffValLst);
    m_nBlocks++;
    };
  if (m_pEHX) 
    {
    m_pEHX->SetOnOffValLst(&m_OnOffValLst);
    m_nBlocks++;
    };
  if (m_pVLE) 
    {
    m_pVLE->SetOnOffValLst(&m_OnOffValLst);
    m_nBlocks++;
    };
  if (m_pEvap) 
    {
    m_pEvap->SetOnOffValLst(&m_OnOffValLst);
    m_nBlocks++;
    };
  };

//-------------------------------------------------------------------------

CBlockEvaluator::~CBlockEvaluator(void)
  {
  }

//-------------------------------------------------------------------------

void CBlockEvaluator::Add_OnOff(TaggedObject * pThis, DataDefnBlk &DDB, DDBPages PageIs)
  {
  if (1)
    {
    DDB.String("EvaluationSequence", "EvalSeq", DC_, "", &m_sBlkSeq, pThis, 0); 

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
    }

  DDB.Text("");
  };

//-------------------------------------------------------------------------

void CBlockEvaluator::BuildDataDefn(TaggedObject * pThis, DataDefnBlk &DDB, DDBPages PageIs)
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
  };

//-------------------------------------------------------------------------

flag CBlockEvaluator::DataXchg(DataChangeBlk & DCB)
  {
  if (m_pRB && /*m_pRB->Enabled() &&*/ m_pRB->DataXchg(DCB))
    return 1;
  if (m_pHX && /*m_pHX->Enabled() &&*/ m_pHX->DataXchg(DCB))
    return 1;
  if (m_pEHX && /*m_pEHX->Enabled() &&*/ m_pEHX->DataXchg(DCB))
    return 1;
  if (m_pVLE && /*m_pVLE->Enabled() &&*/ m_pVLE->DataXchg(DCB))
    return 1;
  if (m_pEvap && /*m_pEvap->Enabled() &&*/ m_pEvap->DataXchg(DCB))
    return 1;

  return 0;
  };

//-------------------------------------------------------------------------

flag CBlockEvaluator::ValidateData(ValidateDataBlk & VDB)
  {
  SortBlocks();

  return true;
  };

//-------------------------------------------------------------------------

int CBlockEvaluator::DefBlkSeqNo(byte SeqID)
  {
  int Seq=255;

  switch (SeqID)
    {
    case BES_RB:    if (m_pRB->Enabled()) Seq=m_RBDefSeq; break;
    case BES_HX:    if (m_pHX->Enabled()) Seq=m_HXDefSeq; break;
    case BES_EHX:   if (m_pEHX->Enabled()) Seq=m_EHXDefSeq; break;
    case BES_VLE:   if (m_pVLE->Enabled()) Seq=m_VLEDefSeq; break;
    case BES_Evap:  if (m_pEvap->Enabled()) Seq=m_EvapDefSeq; break;
    }
  
  return Seq;
  }

//-------------------------------------------------------------------------

int CBlockEvaluator::BlkSeqNo(byte SeqID)
  {
  int Seq=255;
  
  switch (SeqID)
    {
    case BES_RB:    if (m_pRB->Enabled()) Seq=m_pRB->BlockSeqNo(); break;
    case BES_HX:    if (m_pHX->Enabled()) Seq=m_pHX->BlockSeqNo(); break;
    case BES_EHX:   if (m_pEHX->Enabled()) Seq=m_pEHX->BlockSeqNo(); break;
    case BES_VLE:   if (m_pVLE->Enabled()) Seq=m_pVLE->BlockSeqNo(); break;
    case BES_Evap:  if (m_pEvap->Enabled()) Seq=m_pEvap->BlockSeqNo(); break;
    }

  if (Seq==BlkEval_On) 
    Seq=254;

  return Seq;
  }

//-------------------------------------------------------------------------

void CBlockEvaluator::SetBlkSeqNo(byte SeqID, byte No)
  {
  switch (SeqID)
    {
    case BES_RB:    if (m_pRB->Enabled()) m_pRB->BlockSeqNo(No); break;
    case BES_HX:    if (m_pHX->Enabled()) m_pHX->BlockSeqNo(No); break;
    case BES_EHX:   if (m_pEHX->Enabled()) m_pEHX->BlockSeqNo(No); break;
    case BES_VLE:   if (m_pVLE->Enabled()) m_pVLE->BlockSeqNo(No); break;
    case BES_Evap:  if (m_pEvap->Enabled()) m_pEvap->BlockSeqNo(No); break;
    }
  }
//-------------------------------------------------------------------------

void CBlockEvaluator::SortBlocks()
  {
  int i;

  m_Sequence[0] = m_pRB   && m_pRB->Enabled()   ? BES_RB    : BES_Null;
  m_Sequence[1] = m_pHX   && m_pHX->Enabled()   ? BES_HX    : BES_Null;
  m_Sequence[2] = m_pEHX  && m_pEHX->Enabled()  ? BES_EHX   : BES_Null;
  m_Sequence[3] = m_pVLE  && m_pVLE->Enabled()  ? BES_VLE   : BES_Null;
  m_Sequence[4] = m_pEvap && m_pEvap->Enabled() ? BES_Evap  : BES_Null;
  m_Sequence[5] = BES_Null;

  ASSERT(m_Sequence[MaxBEBlocks]==BES_Null);

  for (i=0; i<MaxBEBlocks; i++)
    {
    if (m_Sequence[i]==BES_Null)
      {
      for (int j=i; j<MaxBEBlocks; j++)
        m_Sequence[j]=m_Sequence[j+1];
      }
    }

  m_nSequence=0;
  for (i=0; i<MaxBEBlocks; i++)
    if (m_Sequence[i]!=BES_Null)
      m_nSequence=i+1;

  CBlockEvalBase::BuildOnOffValLst(&m_OnOffValLst, m_nBlocks);

  //int i;
  for (i=1 ; i<m_nSequence; )
    {
    if (BlkSeqNo(m_Sequence[i-1])>BlkSeqNo(m_Sequence[i]) ||
        (BlkSeqNo(m_Sequence[i-1])==BlkSeqNo(m_Sequence[i]) && DefBlkSeqNo(m_Sequence[i-1])>DefBlkSeqNo(m_Sequence[i])))
      {
      // swap
      Exchange(m_Sequence[i-1], m_Sequence[i]);
      if (i>1)
        i--;
      }
    else
      i++;
    };

  if (0)
    {
    dbgpln("SortBlks ===============");
    for (i=0 ; i<m_nSequence; i++)
      dbgpln("  Seq: %5i %5i", m_Sequence[i], BlkSeqNo(m_Sequence[i]));
    }

  m_sBlkSeq="";
  i=0;
  for (; i<m_nSequence && BlkSeqNo(m_Sequence[i])<BlkEval_On; )
    {
    Strng S(Names[m_Sequence[i]]);
    int N=1;
    int iSeq=BlkSeqNo(m_Sequence[i]);
    i++;

#if DISABLESIMULTANEOUS
    if (i<m_nSequence && BlkSeqNo(m_Sequence[i])==iSeq)
      {
      for (int j=i; j<m_nSequence; j++)
        {
        SetBlkSeqNo(m_Sequence[j], BlkSeqNo(m_Sequence[j])+1);
        if (j<m_nSequence-1 && BlkSeqNo(m_Sequence[j])<BlkSeqNo(m_Sequence[j+1])) 
          break;
        }
      }
#endif

    for (  ; i<m_nSequence && BlkSeqNo(m_Sequence[i])==iSeq; i++)
      {
      N++;
      S+=", ";
      S+=Names[m_Sequence[i]];
      }
    if (N>1)
      {
      if (m_sBlkSeq.GetLength()>0)
        m_sBlkSeq+=" > ";
      m_sBlkSeq+="[";
      m_sBlkSeq+=S;
      m_sBlkSeq+="]";
      }
    else
      {
      if (m_sBlkSeq.GetLength()>0)
        m_sBlkSeq+=" > ";
      m_sBlkSeq+=S;
      }
    }

  for (  ; i<m_nSequence /*&& BlkSeqNo(m_Sequence[i])==0*/ && BlkSeqNo(m_Sequence[i])<255; i++)
    {
    if (m_sBlkSeq.GetLength()>0)
      m_sBlkSeq+=" > ";
    m_sBlkSeq+=Names[m_Sequence[i]];
    }

  int xxx=0;
  }

//-------------------------------------------------------------------------

void CBlockEvaluator::EvalProducts(SpConduit & Fo, double Po, CFlwThermalBlk * pFTB, double FinalTEst)
  {
  for (int i=0; i<m_nSequence; i++)
    {
    switch (m_Sequence[i])
      {
      case BES_RB:    
        if (pFTB)
          pFTB->AddRBBegin();
        m_pRB->EvalProducts(Fo);
        if (pFTB)
          pFTB->AddRBEnd();
        break;

      case BES_HX:    
        _asm int 3; // this needs thought
        //FTB.AddHXBegin();
        m_pHX->EvalProducts(); 
        //FTB.AddHXEnd();
        break;

      case BES_EHX:  
        if (pFTB)
          pFTB->AddEHXBegin();
        m_pEHX->EvalProducts(Fo);
        if (pFTB)
          pFTB->AddEHXEnd();
        break;

      case BES_VLE:   
        if (pFTB)
          pFTB->AddVLEBegin();
        m_pVLE->QPFlash(Fo, Po, 0.0, VLEF_Null);
        if (pFTB)
          pFTB->AddVLEEnd();
        break;

      case BES_Evap:  
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
  for (int i=0; i<m_nSequence; i++)
    {
    switch (m_Sequence[i])
      {
      case BES_RB:    
        if (pFTB)
          pFTB->AddRBBegin();
        m_pRB->EvalProducts(Fo);
        if (pFTB)
          pFTB->AddRBEnd();
        break;

      case BES_HX:    
        _asm int 3; // this needs thought
        //FTB.AddHXBegin();
        m_pHX->EvalProducts(); 
        //FTB.AddHXEnd();
        break;

      case BES_EHX:  
        if (pFTB)
          pFTB->AddEHXBegin();
        m_pEHX->EvalProductsPipe(Fo, Len, Diam);
        if (pFTB)
          pFTB->AddEHXEnd();
        break;

      case BES_VLE:   
        if (pFTB)
          pFTB->AddVLEBegin();
        m_pVLE->QPFlash(Fo, Po, 0.0, VLEF_Null);
        if (pFTB)
          pFTB->AddVLEEnd();
        break;

      case BES_Evap:  
        if (pFTB)
          pFTB->AddEvapBegin();
        m_pEvap->EvalProducts(Fo, Po); 
        if (pFTB)
          pFTB->AddEvapEnd();
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