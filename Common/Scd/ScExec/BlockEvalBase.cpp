#include "stdafx.h"

#include "sc_defs.h"
#define __BLOCKEVALBASE_CPP
#include "BlockEvalBase.h"

CBlockEvalBase::CBlockEvalBase(byte BEId, int Index, LPTSTR Name, bool HasStateSemantics) : \
  m_sName(Name) 
  {
  m_BEId=BEId;
  m_Index=Index;
  m_iBlkSeqNo=0;
  m_iDefBlkSeqNo=255;      
  m_bHasStateSemantics=HasStateSemantics;
  m_pOnOffValLst=NULL;
  }

CBlockEvalBase::~CBlockEvalBase(void)
  {
  }

//void CBlockEvalBase::BuildOnOffValLst(DDBValueLstMem  * ValLst, int NInSequence, LPCSTR StateName)
//  {
//  ValLst->Empty();
//  ValLst->Add(BlkEval_Off, "Off");
//  ValLst->Add(BlkEval_On, "On");
//  for (int i=0; i<NInSequence; i++)
//    ValLst->Add(BlkEval_First+i, SeqNames[i]);
//  if (StateName && m_bHasStateSemantics)
//    ValLst->Add(BlkEval_State, (LPSTR)StateName);
//  };

void CBlockEvalBase::SetOnOffValLst(DDBValueLstMem  * ValLst/*, DDBValueLstMem  * StateValLst*/)
  {
  m_pOnOffValLst = ValLst; 
  /*m_pOnOffStateValLst = StateValLst;*/
  };

DDBValueLst * CBlockEvalBase::GetOnOffValLst(/*bool WithState*/)
  {
  //DDBValueLstMem * Lst = WithState ? m_pOnOffStateValLst : m_pOnOffValLst;
  //return Lst ? Lst->Item(0) : &DDBOnOff[0]; 
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
  //return Strng(Names[m_BEId]);
  }


