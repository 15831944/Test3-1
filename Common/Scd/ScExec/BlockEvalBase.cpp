#include "stdafx.h"

#include "sc_defs.h"
#define __BLOCKEVALBASE_CPP
#include "BlockEvalBase.h"

#include "optoff.h"

CBlockEvalBase::CBlockEvalBase(byte BEId, int Index, LPTSTR Name, bool DefinesStateSemantics) : \
  m_sName(Name) 
  {
  m_BEId                    = BEId;
  m_Index                   = Index;
  m_iBlkSeqNo               = 0;
  m_iDefBlkSeqNo            = 255;      
  m_bDefinesStateSemantics  = DefinesStateSemantics;
  m_pBlkStateSemantics      = NULL;
  m_pOnOffValLst            = NULL;
  }

CBlockEvalBase::~CBlockEvalBase(void)
  {
  }

bool CBlockEvalBase::StateSemanticsOn()
  { 
  bool On = m_bDefinesStateSemantics && (BlkSeqNo()>=BlkEval_State); 
  if (On && m_pBlkStateSemantics)
    return m_pBlkStateSemantics->StateSemanticsOn();
  return false;
  };

void CBlockEvalBase::SetOnOffValLst(DDBValueLstMem  * ValLst/*, DDBValueLstMem  * StateValLst*/)
  {
  m_pOnOffValLst = ValLst; 
  };

DDBValueLst * CBlockEvalBase::GetOnOffValLst(/*bool WithState*/)
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

  if (ForSort)
    {    
    if (Seq==BlkEval_State)
      Seq=200;
    if (Seq==BlkEval_On)
      Seq=100;
    }

  //dbgpln("BlkSeqNo %08x %3i = %3i", this, m_BEId, Seq);
  return Seq;
  };

LPTSTR CBlockEvalBase::Name()
  {
  return m_sName();
  }


