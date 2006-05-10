#include "stdafx.h"

#include "sc_defs.h"
#define __BLOCKEVALBASE_CPP
#include "BlockEvalBase.h"

static const LPTSTR SeqNames[] =
  {
/*  "First",
  "Second",
  "Third",
  "Fourth",
  "Fifth",
  "Sixth",*/
  "On (Priority 1)",
  "On (Priority 2)",
  "On (Priority 3)",
  "On (Priority 4)",
  "On (Priority 5)",
  "On (Priority 6)",
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
  ValLst->Add(BlkEval_On, "On");
  for (int i=0; i<NInSequence; i++)
    ValLst->Add(BlkEval_First+i, SeqNames[i]);
  };

void CBlockEvalBase::SetOnOffValLst(DDBValueLstMem  * ValLst)
  {
  m_pOnOffValLst = ValLst; 
  };

DDBValueLst * CBlockEvalBase::GetOnOffValLst()
  {
  return m_pOnOffValLst ? m_pOnOffValLst->Item(0) : &DDBOnOff[0]; 
  };

void CBlockEvalBase::Open(byte L)
  {
  if (L>BlkEval_Off)
    BlockSeqNo(L);
  };

byte CBlockEvalBase::OpenStatus(flag Enabled)
  {
  if (Enabled)
    return BlockSeqNo();
  else
    return BlkEval_Off;
  };
