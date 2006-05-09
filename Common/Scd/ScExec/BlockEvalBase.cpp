#include "stdafx.h"

#include "sc_defs.h"
#define __BLOCKEVALBASE_CPP
#include "BlockEvalBase.h"

static const LPTSTR SeqNames[] =
  {
  "First",
  "Second",
  "Third",
  "Fourth",
  "Fifth",
  "Sixth",
  };


CBlockEvalBase::CBlockEvalBase(void)
  {
  m_iBlockSeqNo=0;
  m_pOnOffValLst=NULL;
  }

CBlockEvalBase::~CBlockEvalBase(void)
  {
  }

void CBlockEvalBase::BuildOnOffValLst(DDBValueLstMem  * ValLst, int NInSequence)
  {
  ValLst->Empty();
  ValLst->Add(BlkEval_Off, "Off");

  for (int i=0; i<NInSequence; i++)
    ValLst->Add(BlkEval_First+i, SeqNames[i]);

  ValLst->Add(BlkEval_On, "On");

  };

void CBlockEvalBase::SetOnOffValLst(DDBValueLstMem  * ValLst)
  {
  m_pOnOffValLst = ValLst; 
  };

DDBValueLst * CBlockEvalBase::GetOnOffValLst()
  {
  //return &DDBOnOff[0]; 
  return m_pOnOffValLst ? m_pOnOffValLst->Item(0) : &DDBOnOff[0]; 
  };

void CBlockEvalBase::Open(long L)
  {
  //if (L==BlkEval_On)
  //  BlockSeqNo(0);
  //else if (L>=BlkEval_First)
  //  BlockSeqNo(L-BlkEval_First+1);
  if (L>BlkEval_Off)
    BlockSeqNo(L);
  };

long CBlockEvalBase::OpenStatus(flag Enabled)
  {
  if (Enabled)
    {
    return BlockSeqNo();
    //if (BlockSeqNo()==0)
    //  return BlkEval_On;
    //else
    //  return BlockSeqNo()+(BlkEval_First-1);
    }
  else
    return BlkEval_Off;
  };
