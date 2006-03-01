#pragma once
//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __BLENDER_H
#define  __BLENDER_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __M_BASE_H
  #include "m_base.h"
#endif
#ifndef __MTRLOGIC_H
  #include "mtrlogic.h"
#endif
#ifndef __SP_QUEUE_H
  #include "sp_queue.h"
#endif

#define WithBlender 1

//===========================================================================
#if WithBlender

//--------------------------------------------------------------------------

class CAssaySum
  {
  public:
 
    CAssaySum();
    ~CAssaySum();
    void InitIndexList1(long ListCnt);
    void InitIndexList2(long ListCnt);
    long List1Cnt() { return m_iList1Cnt; };
    long List2Cnt() { return m_iList2Cnt; };
    bool InsertIndexList1(int Index);
    bool RemoveIndexList1(int Index);

    Strng       m_sName;
    //double      m_dResult;        //result
    int *       m_pIndexList1;    //array of index numbers into species
    int *       m_pIndexList2;    //array of index numbers into species
    long        m_iList1Cnt;      //length of index array 1
    long        m_iList2Cnt;      //length of index array 2
	

    void Destroy();
  };

//===========================================================================

class CAssaySumHelper
  {
  public:
    CAssaySumHelper();

    bool      m_bDDBValid;
    int       m_iMyIndex;
    CAssaySum* m_pBCrit;
    DDBValueLstMem  m_DDBAdd;
    DDBValueLstMem  m_DDBRemove;

    void      SetUpDDB();
    void BuildDataDefn(FlwNode* pTagObj, DataDefnBlk & DDB, int AssayIndex);

  };

//===========================================================================

typedef CArray <CAssaySumHelper*, CAssaySumHelper*> AssaySumArray;

//--------------------------------------------------------------------------

typedef double* pDbl;
typedef CAssaySum* pCAssaySum;


//===========================================================================

class CAssays
  {
  public:
    AssaySumArray m_Assays;
    long          m_iAssaySumCnt;
    pCAssaySum   *m_pAssaySum;

    CAssays() { m_iAssaySumCnt = 0; m_pAssaySum = 0; };
    //~CAssays();


	  void             InitAssaySum(long AssaySumCount);
    void             SetAssaySumCount(short AssaySumCnt);
    int              GetAssaySumCount() { return m_iAssaySumCnt; };
    CAssaySum*       GetAssaySum(int index) { return m_pAssaySum[index]; };

  };

//===========================================================================


class CBlendCriteria
  {
  public:
	enum eConstraintType { eCT_Soft, eCT_Ratio };
	enum eCompType       { eCT_Greater, eCT_Lesser };

    CBlendCriteria();
    ~CBlendCriteria();

    Strng       m_sName;

    eConstraintType   m_eConstraintType; //Contstraint Type Soft or Ratio
    eCompType         m_eCompType;       //Comparison Type > or <

    double      m_dWeighting;     //weighting of this criteria - larger number means more important
    double      m_dRequirement;   //requirement objective (sum fraction; ratio)
    double      m_dResult;        //resultant optimised "achieved" criteria value
	double      m_dCost;          //Cost of achieved criteria value

	long        m_AssayIndex1;    // Assay Array Index for constraint
	long        m_AssayIndex2;    // Assay Array Index for Constraint

    void Destroy();
  };

//===========================================================================

typedef CBlendCriteria* pCBlendCriteria;

//===========================================================================

class CBlendData
  {
  public:
    CBlendData();
    ~CBlendData();

    void Init(long TankCnt, long ComponentCnt, long CriteriaCnt, long AssayCnt);

    long TankCount() { return m_iTankCnt; };
    long ComponentCount() { return m_iComponentCnt; };
    long CriteriaCount() { return m_iCriteriaCnt; };
    //long AssaySumCount() { return m_iAssaySumCnt; };

    double ** GetContents() { return m_pContent; };
    double * GetTankContents(int index) { return m_pContent[index]; };
    double* GetTotal() { return m_pTotal; };
    double* GetMin() { return m_pMin; };
    double* GetMax() { return m_pMax; };
    double* GetResultSplits() { return m_pResultSplits; };
    CBlendCriteria* GetCriteria(int index) { return m_pCriteria[index]; };
	  //CAssaySum*      GetAssaySum(int index) { return gs_pAssaySum[index]; };//{return m_pAssaySum[index]; };

  protected:
    long        m_iTankCnt;       //number of tanks
    long        m_iComponentCnt;  //number of components in each tank
    double      m_dTotalRqd;      //total output amount required
    pDbl *      m_pContent;       //for each tank, fractions of each component (add up to 1.0)
    double *    m_pTotal;         //total amount for each tank
    double *    m_pMin;           //as a fraction minimum output allowed for each tank
    double *    m_pMax;           //as a fraction maximum output allowed for each tank
    double *    m_pResultSplits;  //calculated fraction from each tank making up total output (add up to 1.0)

    long        m_iCriteriaCnt;   //number of criteria to be tested
	  pCBlendCriteria * m_pCriteria;//array of pointers to the criteria information

    void DestroyContent();
    void DestroyCriteria();
  };

#endif //WithBlender
//===========================================================================
//
//
//
//===========================================================================

#endif
