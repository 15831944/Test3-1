#include "stdafx.h"

#include "sc_defs.h"
#include "flwnode.h"
#define __BLOCKEVALUATOR_CPP
#include "BlockEvaluator.h"

//#if WITHBLOCKEVALUATOR

//=========================================================================
//
//
//
//=========================================================================

XID xidEvalSeq0 = EvalBlkXID(1);

static LPTSTR Names[] = 
  {
  "Null",
  "RB",
  "HX",
  "EHX",
  "VLE",
  "Evap",
  };

CBlockEvaluator::CBlockEvaluator(CReactionBase * pRB, CHXBase *pHX, CEnvironHXBase * pEHX, CVLEBase * pVLE, CEvapBase * pEvap)
  {
  m_pRB   = pRB;
  m_pHX   = pHX;
  m_pEHX  = pEHX;
  m_pVLE  = pVLE;
  m_pEvap = pEvap;

  m_Sequence[0] = m_pRB   ? BES_RB    : BES_Null;
  m_Sequence[1] = m_pHX   ? BES_HX    : BES_Null;
  m_Sequence[2] = m_pEHX  ? BES_EHX   : BES_Null;
  m_Sequence[3] = m_pVLE  ? BES_VLE   : BES_Null;
  m_Sequence[4] = m_pEvap ? BES_Evap  : BES_Null;
  m_Sequence[5] = BES_Null;

  ASSERT(m_Sequence[MaxBEBlocks]==BES_Null);

  Strng S;
  for (int i=0; i<MaxBEBlocks; i++)
    {
    if (m_Sequence[i]==BES_Null)
      {
      for (int j=i; j<MaxBEBlocks; j++)
        m_Sequence[j]=m_Sequence[j+1];
      }
    }

  for (int i=0; m_Sequence[i]>BES_Null; i++)
    m_ValLst.Add(m_Sequence[i]|BES_On, Names[m_Sequence[i]&BES_Blocks]);

  for (int i=0; m_Sequence[i]>BES_Null; i++)
    {
    S.Set("%s - Off", Names[m_Sequence[i]&BES_Blocks]);
    m_ValLst.Add(m_Sequence[i], S());
    }

  if (0) // set this to 1 to switch on simultaneous soln - still to be coded
    {
    for (int i=0; m_Sequence[i]>BES_Null; i++)
      {
      S.Set("%s - Sim", Names[m_Sequence[i]&BES_Blocks]);
      m_ValLst.Add(m_Sequence[i]|BES_On|BES_Simultaneous, S());
      }
    }
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
    for (int i=0; m_Sequence[i]>BES_Null; i++)
      {
      flag On=0;
      switch (m_Sequence[i]&BES_Blocks)
        {
        case BES_RB:    On=m_pRB->Enabled(); break;
        case BES_HX:    On=m_pHX->Enabled(); break;
        case BES_EHX:   On=m_pEHX->Enabled(); break;
        case BES_VLE:   On=m_pVLE->Enabled(); break;
        case BES_Evap:  On=m_pEvap->Enabled(); break;
        }
      if (On)
        m_Sequence[i]|=BES_On; 
      else
        m_Sequence[i]&=~BES_On; 
      }
    SortBlocks();
    }

  if (DDB.BeginStruct(pThis, "Blocks", NULL, PageIs))
    {
    DDB.Byte("Seq.1", "", DC_, "", xidEvalSeq0, pThis, isParm|SetOnChange, &m_ValLst);
    Strng S;
    for (int i=1; m_Sequence[i]>BES_Null; i++)
      {
      S.Set("Seq.%i", i+1);
      DDB.Byte(S(), "", DC_, "", xidEvalSeq0+i, pThis, isParm|SetOnChange, &m_ValLst);
      }
    }
  //for (int i=0; m_Sequence[i]>BES_Null; i++)
  //  {
  //  switch (m_Sequence[i]&BES_Blocks)
  //    {
  //    case BES_RB:    m_pRB->Add_OnOff(DDB); break;
  //    case BES_HX:    m_pHX->Add_OnOff(DDB); break;
  //    case BES_EHX:   m_pEHX->Add_OnOff(DDB); break;
  //    case BES_VLE:   m_pVLE->Add_OnOff(DDB); break;
  //    case BES_Evap:  m_pEvap->Add_OnOff(DDB); break;
  //    }
  //  }
  //DDB.Text("");
  DDB.EndStruct();
  };

//-------------------------------------------------------------------------

void CBlockEvaluator::BuildDataDefn(TaggedObject * pThis, DataDefnBlk &DDB, DDBPages PageIs)
  {
  for (int i=0; m_Sequence[i]>BES_Null; i++)
    {
    if (1)//m_Sequence[i]&BES_On)
      {
      switch (m_Sequence[i]&BES_Blocks)
        {
        case BES_RB:    m_pRB->BuildDataDefn(DDB); break;
        case BES_HX:    m_pHX->BuildDataDefn(DDB); break;
        case BES_EHX:   m_pEHX->BuildDataDefn(DDB); break;
        case BES_VLE:   m_pVLE->BuildDataDefn(DDB); break;
        case BES_Evap:  m_pEvap->BuildDataDefn(DDB); break;
        }
      }
    }
  };

//-------------------------------------------------------------------------

flag CBlockEvaluator::DataXchg(DataChangeBlk & DCB)
  {
  if (DCB.lHandle>=xidEvalSeq0 && DCB.lHandle<xidEvalSeq0+MaxBEBlocks)
    {
    int iSeq=DCB.lHandle-xidEvalSeq0;
    if (DCB.rB)
      {
      byte Reqd=*DCB.rB; 
      int iReqd=iSeq;

      // remove from current position
      int i=0;
      for ( ; i<MaxBEBlocks; i++)
        if ((m_Sequence[i] & BES_Blocks)==(Reqd & BES_Blocks))
          break;
      for ( ; i<MaxBEBlocks; i++)
        m_Sequence[i]=m_Sequence[i+1];

      // insert it again in reqd posn;
      for (i=MaxBEBlocks; i>iReqd; i--)
        m_Sequence[i]=m_Sequence[i-1];
      m_Sequence[iReqd]=Reqd;
      if (Reqd&BES_On)
        {
        switch (Reqd&BES_Blocks)
          {
          case BES_RB:    m_pRB->Open(); break;
          case BES_HX:    m_pHX->Open(); break;
          case BES_EHX:   m_pEHX->Open(); break;
          case BES_VLE:   m_pVLE->Open(); break;
          case BES_Evap:  m_pEvap->Open(); break;
          }
        }
      else
        {
        switch (Reqd&BES_Blocks)
          {
          case BES_RB:    m_pRB->Close(); break;
          case BES_HX:    m_pHX->Close(); break;
          case BES_EHX:   m_pEHX->Close(); break;
          case BES_VLE:   m_pVLE->Close(); break;
          case BES_Evap:  m_pEvap->Close(); break;
          }
        }
      SortBlocks();
      }
    DCB.B=m_Sequence[iSeq];
    return 1;
    }

  return 0;
  };

//-------------------------------------------------------------------------

flag CBlockEvaluator::ValidateData(ValidateDataBlk & VDB)
  {
  return 0;
  };

//-------------------------------------------------------------------------

void CBlockEvaluator::SortBlocks()
  {
  // Move 'Off' blocks to end
  for (int i=1 ; i<MaxBEBlocks; i++)
    {
    if ((m_Sequence[i-1] & BES_On)==0 && (m_Sequence[i] & BES_On)!=0 && (m_Sequence[i]&BES_Blocks)!=0)
      {
      // swap
      Exchange(m_Sequence[i-1], m_Sequence[i]);
      if (i>1)
        i+=-2;
      }
    };
  }

//-------------------------------------------------------------------------

void CBlockEvaluator::EvalProducts(CFlwThermalBlk & FTB, SpConduit & Fo, double Po, double FinalTEst)
  {
  for (int i=0; m_Sequence[i]>BES_Null; i++)
    {
    if (m_Sequence[i]&BES_On)
      {
      switch (m_Sequence[i]&BES_Blocks)
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
      }
    }
  };

//-------------------------------------------------------------------------

void CBlockEvaluator::EvalProductsPipe(CFlwThermalBlk & FTB, SpConduit & Fo, double Len, double Diam, double Po, double FinalTEst)
  {
  for (int i=0; m_Sequence[i]>BES_Null; i++)
    {
    if (m_Sequence[i]&BES_On)
      {
      switch (m_Sequence[i]&BES_Blocks)
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
      }
    }
  };

//=========================================================================
//
//
//
//=========================================================================

//#endif