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

CBlockEvaluator::CBlockEvaluator(CReactionBase * pRB,   byte iRB, 
                                 CHXBase *pHX,          byte iHX, 
                                 CEnvironHXBase * pEHX, byte iEHX, 
                                 CVLEBase * pVLE,       byte iVLE, 
                                 CEvapBase * pEvap,     byte iEvap)
  {
  m_nBlocks   = 0;
  m_nSequence = 0;

  m_pRB   = pRB;
  m_pHX   = pHX;
  m_pEHX  = pEHX;
  m_pVLE  = pVLE;
  m_pEvap = pEvap;

  SetDefSequence(iRB, iHX, iEHX, iVLE, iEvap);

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

  
  //m_Sequence[0] = m_pRB   ? BES_RB    : BES_Null;
  //m_Sequence[1] = m_pHX   ? BES_HX    : BES_Null;
  //m_Sequence[2] = m_pEHX  ? BES_EHX   : BES_Null;
  //m_Sequence[3] = m_pVLE  ? BES_VLE   : BES_Null;
  //m_Sequence[4] = m_pEvap ? BES_Evap  : BES_Null;
  //m_Sequence[5] = BES_Null;


  //ASSERT(m_Sequence[MaxBEBlocks]==BES_Null);

  //for (int i=0; i<MaxBEBlocks; i++)
  //  {
  //  if (m_Sequence[i]==BES_Null)
  //    {
  //    for (int j=i; j<MaxBEBlocks; j++)
  //      m_Sequence[j]=m_Sequence[j+1];
  //    }
  //  }

  //m_nSequence=0;
  //for (int i=0; i<MaxBEBlocks; i++)
  //  if (m_Sequence[i]!=BES_Null)
  //    m_nSequence=i+1;

  //Strng S;
  //m_SeqValLst.Add(0, "Default");
  //for (int i=1; i<m_nSequence; i++)
  //  m_SeqValLst.Add(i, S.Set("%i", i));

  };

//-------------------------------------------------------------------------

CBlockEvaluator::~CBlockEvaluator(void)
  {
  }

//-------------------------------------------------------------------------

void CBlockEvaluator::SetDefSequence(byte iRB, 
                                     byte iHX, 
                                     byte iEHX, 
                                     byte iVLE, 
                                     byte iEvap)
  {
  m_RBDefSeq   = iRB;  
  m_HXDefSeq   = iHX; 
  m_EHXDefSeq  = iEHX; 
  m_VLEDefSeq  = iVLE; 
  m_EvapDefSeq = iEvap;

  ASSERT_ALWAYS(m_pRB==NULL || m_RBDefSeq>0, "Bad EvalBlock DefSeq");
  ASSERT_ALWAYS(m_pHX==NULL || m_HXDefSeq>0, "Bad EvalBlock DefSeq"); 
  ASSERT_ALWAYS(m_pEHX==NULL || m_EHXDefSeq>0, "Bad EvalBlock DefSeq"); 
  ASSERT_ALWAYS(m_pVLE==NULL || m_VLEDefSeq>0, "Bad EvalBlock DefSeq"); 
  ASSERT_ALWAYS(m_pEvap==NULL || m_EvapDefSeq>0, "Bad EvalBlock DefSeq");

  SortBlocks();
  };

//-------------------------------------------------------------------------

void CBlockEvaluator::Add_OnOff(TaggedObject * pThis, DataDefnBlk &DDB, DDBPages PageIs)
  {
  if (1)
    {
    DDB.String("BlkSeq", "", DC_, "", &m_sBlkSeq, pThis, 0); 

    Strng S;
    if (m_pRB)
      m_pRB->Add_OnOff(DDB);
    if (m_pHX)
      m_pHX->Add_OnOff(DDB);
    if (m_pEHX)
      m_pEHX->Add_OnOff(DDB);
    if (m_pVLE)
      m_pVLE->Add_OnOff(DDB);
    if (m_pEvap)
      m_pEvap->Add_OnOff(DDB);

    //if (m_pRB && m_pRB->Enabled())
    //  {
    //  DDB.Byte("RB.Seq", "", DC_, "", xidEvalSeqRB, pThis, isParm);
    //  if (m_pRB->BlockSeqNo()==0)
    //    DDB.TagComment(S.Set("Def=%i", m_RBDefSeq));
    //  }
    //if (m_pHX && m_pHX->Enabled())
    //  {
    //  DDB.Byte("HX.Seq", "", DC_, "", xidEvalSeqHX, pThis, isParm);
    //  if (m_pHX->BlockSeqNo()==0)
    //    DDB.TagComment(S.Set("Def=%i", m_HXDefSeq));
    //  }
    //if (m_pEHX && m_pEHX->Enabled())
    //  {
    //  DDB.Byte("EHX.Seq", "", DC_, "", xidEvalSeqEHX, pThis, isParm);
    //  if (m_pEHX->BlockSeqNo()==0)
    //    DDB.TagComment(S.Set("Def=%i", m_EHXDefSeq));
    //  }
    //if (m_pVLE && m_pVLE->Enabled())
    //  {
    //  DDB.Byte("VLE.Seq", "", DC_, "", xidEvalSeqVLE, pThis, isParm);
    //  if (m_pVLE->BlockSeqNo()==0)
    //    DDB.TagComment(S.Set("Def=%i", m_VLEDefSeq));
    //  }
    //if (m_pEvap && m_pEvap->Enabled())
    //  {
    //  DDB.Byte("Evap.Seq", "", DC_, "", xidEvalSeqEvap, pThis, isParm);
    //  if (m_pEvap->BlockSeqNo()==0)
    //    DDB.TagComment(S.Set("Def=%i", m_EvapDefSeq));
    //  }

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
  //switch (DCB.lHandle)
  //  {
  //  case xidEvalSeqRB:
  //    if (DCB.rB)
  //      m_pRB->BlockSeqNo(*DCB.rB);
  //    DCB.B=m_pRB->BlockSeqNo();
  //    break;
  //  case xidEvalSeqHX:
  //    if (DCB.rB)
  //      m_pHX->BlockSeqNo(*DCB.rB);
  //    DCB.B=m_pHX->BlockSeqNo();
  //    break;
  //  case xidEvalSeqEHX:
  //    if (DCB.rB)
  //      m_pEHX->BlockSeqNo(*DCB.rB);
  //    DCB.B=m_pEHX->BlockSeqNo();
  //    break;
  //  case xidEvalSeqVLE:
  //    if (DCB.rB)
  //      m_pVLE->BlockSeqNo(*DCB.rB);
  //    DCB.B=m_pVLE->BlockSeqNo();
  //    break;
  //  case xidEvalSeqEvap:
  //    if (DCB.rB)
  //      m_pEvap->BlockSeqNo(*DCB.rB);
  //    DCB.B=m_pEvap->BlockSeqNo();
  //    break;
  //  }

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
  switch (SeqID)
    {
    case BES_RB:    return m_pRB->Enabled() ? m_RBDefSeq : 255;
    case BES_HX:    return m_pHX->Enabled() ? m_HXDefSeq : 255;
    case BES_EHX:   return m_pEHX->Enabled() ? m_EHXDefSeq : 255;
    case BES_VLE:   return m_pVLE->Enabled() ? m_VLEDefSeq : 255;
    case BES_Evap:  return m_pEvap->Enabled() ? m_EvapDefSeq : 255;
    }
  return 255;
  }

//-------------------------------------------------------------------------

int CBlockEvaluator::BlkSeqNo(byte SeqID)
  {
  switch (SeqID)
    {
    case BES_RB:    return m_pRB->Enabled() ? m_pRB->BlockSeqNo() : 255;
    case BES_HX:    return m_pHX->Enabled() ? m_pHX->BlockSeqNo() : 255;
    case BES_EHX:   return m_pEHX->Enabled() ? m_pEHX->BlockSeqNo() : 255;
    case BES_VLE:   return m_pVLE->Enabled() ? m_pVLE->BlockSeqNo() : 255;
    case BES_Evap:  return m_pEvap->Enabled() ? m_pEvap->BlockSeqNo() : 255;
    }
  return 255;
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

  if (1)
    {
    dbgpln("===============");
    for (i=0 ; i<m_nSequence; i++)
      dbgpln("Seq: %5i %5i", m_Sequence[i], BlkSeqNo(m_Sequence[i]));
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

void CBlockEvaluator::EvalProducts(CFlwThermalBlk & FTB, SpConduit & Fo, double Po, double FinalTEst)
  {
  for (int i=0; i<m_nSequence; i++)
    {
    switch (m_Sequence[i])
      {
      case BES_RB:    
        FTB.AddRBBegin();
        m_pRB->EvalProducts(Fo);
        FTB.AddRBEnd();
        break;

      case BES_HX:    
        _asm int 3; // this needs thought
        //FTB.AddHXBegin();
        m_pHX->EvalProducts(); 
        //FTB.AddHXEnd();
        break;

      case BES_EHX:  
        FTB.AddEHXBegin();
        m_pEHX->EvalProducts(Fo);
        FTB.AddEHXEnd();
        break;

      case BES_VLE:   
        FTB.AddVLEBegin();
        m_pVLE->QPFlash(Fo, Po, 0.0, VLEF_Null);
        FTB.AddVLEEnd();
        break;

      case BES_Evap:  
        _asm int 3;
        m_pEvap->EvalProducts(Fo, Po); 
        break;
      }
    //      }
    }
  };

//-------------------------------------------------------------------------

void CBlockEvaluator::EvalProductsPipe(CFlwThermalBlk & FTB, SpConduit & Fo, double Len, double Diam, double Po, double FinalTEst)
  {
  for (int i=0; i<m_nSequence; i++)
    {
    switch (m_Sequence[i])
      {
      case BES_RB:    
        FTB.AddRBBegin();
        m_pRB->EvalProducts(Fo);
        FTB.AddRBEnd();
        break;

      case BES_HX:    
        _asm int 3; // this needs thought
        //FTB.AddHXBegin();
        m_pHX->EvalProducts(); 
        //FTB.AddHXEnd();
        break;

      case BES_EHX:  
        FTB.AddEHXBegin();
        m_pEHX->EvalProductsPipe(Fo, Len, Diam);
        FTB.AddEHXEnd();
        break;

      case BES_VLE:   
        FTB.AddVLEBegin();
        m_pVLE->QPFlash(Fo, Po, 0.0, VLEF_Null);
        FTB.AddVLEEnd();
        break;

      case BES_Evap:  
        _asm int 3;
        m_pEvap->EvalProducts(Fo, Po); 
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