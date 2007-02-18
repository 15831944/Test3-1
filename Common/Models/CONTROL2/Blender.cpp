//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "blender.h"
//#include "optoff.h"

//==========================================================================
//
//
//
//==========================================================================

// Global Assay Information used by Blend Controllers
CAssays gs_Assays;

void CAssays::InitAssaySum(long AssaySumCount)
  {
	  // This is called by SetAssaySumCount
	  // It create the pointers to the array of AssaySums
	  // The AssaySumHelper array contents are then pointed to
	  // these pointers after the new AssaySum have been created
	  // and the size adjusted - We really should integrate the two routines
	  // to save confusion

  if (AssaySumCount!=m_iAssaySumCnt)
    {

    long PrevCount = m_iAssaySumCnt;
    pCAssaySum * pAssay = NULL;
    if (AssaySumCount>0)
      {
      pAssay = new pCAssaySum[AssaySumCount];
      for (int i=0; i<min(PrevCount, AssaySumCount); i++)
        {
        pAssay[i] = m_pAssaySum[i];
        }
      for (int i=PrevCount; i<AssaySumCount; i++)
        {
        pAssay[i] = new CAssaySum;
        }
      }
    for (int i=AssaySumCount; i<PrevCount; i++)
      delete (m_pAssaySum[i]);

    m_iAssaySumCnt = AssaySumCount;
    m_pAssaySum = pAssay;
    }

  }

void             CAssays::SetAssaySumCount(short AssaySumCnt)

  {
  InitAssaySum(AssaySumCnt);

 
  int PrevCnt = m_Assays.GetCount();
  if (AssaySumCnt<m_Assays.GetCount())
    {
    for (int i=AssaySumCnt; i<PrevCnt; i++)
      delete m_Assays[i];
    }
  m_Assays.SetSize(AssaySumCnt);
  for (int i=PrevCnt; i<m_Assays.GetCount(); i++)
    m_Assays[i] = new CAssaySumHelper;

  for (int i=0; i<m_Assays.GetCount(); i++)
    {
    m_Assays[i]->m_iMyIndex = i;
    m_Assays[i]->m_pBCrit = GetAssaySum(i);
    m_Assays[i]->m_bDDBValid = false;
    }
  }

//--------------------------------------------------------------------------

CAssaySum::CAssaySum()
  {
//  m_dResult = 0.0;
  m_pIndexList1 = NULL;
  m_pIndexList2 = NULL;
  m_iList1Cnt = 0;
  m_iList2Cnt = 0;
  }

//--------------------------------------------------------------------------

CAssaySum::~CAssaySum()
  {
  Destroy();
  }

//--------------------------------------------------------------------------

void CAssaySum::InitIndexList1(long ListCnt)
  {
  if (ListCnt!=m_iList1Cnt)
    {
    if (m_pIndexList1)
      delete []m_pIndexList1;
    m_iList1Cnt = ListCnt;
    if (m_iList1Cnt==0)
      m_pIndexList1 = NULL;
    else
      {
      m_pIndexList1 = new int[m_iList1Cnt];
      for (int i=0; i<m_iList1Cnt; i++)
        m_pIndexList1[i] = -1;
      }
    }
  }

void CAssaySum::InitIndexList2(long ListCnt)
  {
  if (ListCnt!=m_iList2Cnt)
    {
    if (m_pIndexList2)
      delete []m_pIndexList2;
    m_iList2Cnt = ListCnt;
    if (m_iList2Cnt==0)
      m_pIndexList2 = NULL;
    else
      {
      m_pIndexList2 = new int[m_iList2Cnt];
      for (int i=0; i<m_iList2Cnt; i++)
        m_pIndexList2[i] = -1;
      }
    }
  }

//--------------------------------------------------------------------------

void CAssaySum::Destroy()
  {
  if (m_pIndexList1)
    delete []m_pIndexList1;
  if (m_pIndexList2)
    delete []m_pIndexList2;
  m_pIndexList1 = NULL;
  m_pIndexList2 = NULL;
  m_iList1Cnt = 0;
  m_iList2Cnt = 0;
  }

//--------------------------------------------------------------------------

bool CAssaySum::InsertIndexList1(int Index)
  {
  if (m_iList1Cnt==0)
    {
    InitIndexList1(1);
    m_pIndexList1[0] = Index;
    return true;
    }

  bool Found = false;
  for (int i=0; i<m_iList1Cnt; i++)
    if (m_pIndexList1[i]==Index)
      {
      Found = true;
      break;
      }

  if (Found)
    return false;

  int * newIndexList = new int[m_iList1Cnt+1];
  i = 0;
  while (m_pIndexList1[i]<Index && i<m_iList1Cnt)
    {
    newIndexList[i] = m_pIndexList1[i];
    i++;
    }
  newIndexList[i] = Index;
  i++;
  while (i<=m_iList1Cnt)
    {
    newIndexList[i] = m_pIndexList1[i-1];
    i++;
    }
  m_iList1Cnt++;
  delete []m_pIndexList1;
  m_pIndexList1 = newIndexList;
  return true;
  }

//--------------------------------------------------------------------------

bool CAssaySum::RemoveIndexList1(int Index)
  {
  if (m_iList1Cnt==0)
    return false;

  if (m_iList1Cnt==1)
    {
    if (m_pIndexList1[0]==Index)
      {
      delete []m_pIndexList1;
      m_pIndexList1 = NULL;
      m_iList1Cnt = 0;
      return true;
      }
    return false;
    }

  int Found = -1;
  for (int i=0; i<m_iList1Cnt; i++)
    if (m_pIndexList1[i]==Index)
      {
      Found = i;
      break;
      }

  if (Found==-1)
    return false;

  int * newIndexList = new int[m_iList1Cnt-1];
  i = 0;
  while (i<Found)
    {
    newIndexList[i] = m_pIndexList1[i];
    i++;
    }
  i++;
  while (i<m_iList1Cnt)
    {
    newIndexList[i-1] = m_pIndexList1[i];
    i++;
    }
  m_iList1Cnt--;
  delete []m_pIndexList1;
  m_pIndexList1 = newIndexList;
  return true;
  }

//--------------------------------------------------------------------------

CBlendCriteria::CBlendCriteria()
  {
  m_eConstraintType = eCT_Soft;
  m_eCompType       = eCT_Greater;
  m_dRequirement = 0.0;
  m_dResult = 0.0;
  m_dCost = 0.0;
  m_dWeighting = 1.0;
  m_AssayIndex1 = 0;
  m_AssayIndex2 = 0;
  }

//--------------------------------------------------------------------------

CBlendCriteria::~CBlendCriteria()
  {
  Destroy();
  }

//--------------------------------------------------------------------------

void CBlendCriteria::Destroy()
  {

  }

//==========================================================================

CBlendData::CBlendData()
  {
  m_dTotalRqd = 0.0;
  m_pContent = NULL;
  m_pTotal = NULL;
  m_pMin = NULL;
  m_pMax = NULL;
  m_pRqdSplits = NULL;
  m_pResultSplits = NULL;
  m_iTankCnt = 0;
  m_iComponentCnt = 0;
  m_pCriteria = NULL;
  //m_pAssaySum = NULL; // Make a static global
  m_iCriteriaCnt = 0;
  //m_iAssaySumCnt = 0;
  }

//--------------------------------------------------------------------------

CBlendData::~CBlendData()
  {
  DestroyContent();
  DestroyCriteria();
  }

//--------------------------------------------------------------------------

void CBlendData::Init(long TankCnt, long ComponentCnt, long CriteriaCnt, long AssayCnt)
  {
  DestroyContent();
  DestroyCriteria();
  if (TankCnt>0 && ComponentCnt>0)
    {
    m_pContent = new pDbl[TankCnt];
    for (int i=0; i<TankCnt; i++)
      m_pContent[i] = new double[ComponentCnt];
    m_pTotal = new double[TankCnt];
    m_pMin = new double[TankCnt];
    m_pMax = new double[TankCnt];
    m_pRqdSplits = new double[TankCnt];
    m_pResultSplits = new double[TankCnt];
    m_iTankCnt = TankCnt;
    m_iComponentCnt = ComponentCnt;
    }
  if (CriteriaCnt>0)
    {
    m_pCriteria = new pCBlendCriteria[CriteriaCnt];
    for (int i=0; i<CriteriaCnt; i++)
      {
      m_pCriteria[i] = new CBlendCriteria;
      }
    m_iCriteriaCnt = CriteriaCnt;
    }
  }

//--------------------------------------------------------------------------

void CBlendData::DestroyContent()
  {

  if (m_pContent)
    {
    for (int i=0; i<m_iTankCnt; i++)
      delete [](m_pContent[i]);
    delete []m_pContent;
    delete []m_pTotal;
    delete []m_pMin;
    delete []m_pMax;
    delete []m_pRqdSplits;
    delete []m_pResultSplits;
    }
  m_pContent = NULL;
  m_pTotal = NULL;
  m_pMin = NULL;
  m_pMax = NULL;
  m_pRqdSplits = NULL;
  m_pResultSplits = NULL;
  m_iTankCnt = 0;
  m_iComponentCnt = 0;
  }

//--------------------------------------------------------------------------

void CBlendData::DestroyCriteria()
  {
  if (m_pCriteria)
    {
    for (int i=0; i<m_iCriteriaCnt; i++)
      delete m_pCriteria[i];
    delete []m_pCriteria;

    }
  m_pCriteria = NULL;
  m_iCriteriaCnt = 0;

  }

//==========================================================================
//
//
//
//==========================================================================
