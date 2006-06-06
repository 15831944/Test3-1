#include "stdafx.h"

#include "sc_defs.h"
#define __BLOCKEVALBASE_CPP
#include "BlockEvalBase.h"

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


CBlockEvalBase::CBlockEvalBase(byte BEId)
  {
  m_BEId=BEId;
  m_iBlkSeqNo=0;
  m_iDefBlkSeqNo=255;
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
    SetBlkSeqNo(L);
  };

byte CBlockEvalBase::OpenStatus(flag Enabled)
  {
  if (Enabled)
    return BlkSeqNo();
  else
    return BlkEval_Off;
  };

byte CBlockEvalBase::BlkSeqNo(bool ForSort)
  {
  int Seq=255;
  if (Enabled()) 
    Seq=m_iBlkSeqNo; 

  if (ForSort && (Seq==BlkEval_On))
    Seq=254;

  //dbgpln("BlkSeqNo %08x %3i = %3i", this, m_BEId, Seq);
  return Seq;
  };

Strng CBlockEvalBase::Name()
  {
  static LPTSTR Names[] =  {"Null", "RB", "HX", "EHX", "VLE", "Evap", "X", };

  return Strng(Names[m_BEId]);
  }


