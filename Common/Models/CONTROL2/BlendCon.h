//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __BLENDCON_H
#define  __BLENDCON_H

#ifndef __SC_DEFS_H
#include "sc_defs.h"
#endif  
#ifndef __FLWNODE_H
#include "flwnode.h"
#endif
#ifndef __MODELS_H
#include "xrefs.h"
#endif  
#ifndef __MODELS_H
#include "models.h"
#endif  
#include "..\mathand1\multistore.h"

#ifndef  __Matrix_TB_H
  #include "Matrix_TB.h"
#endif

#ifdef __BLENDCON_CPP
  #define DllImportExport 
#elif !defined(Control2)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

#define WithMethod1 0
#define WithTables 0

#define SKIPIT 0
#if SKIPIT 
#pragma message ("---------------------------------------SKIPPED")
#else

class CBlendCon; // Forward Definition

//--------------------------------------------------------------------------

class CBlendCriteriaHelper
  {
  public:
    CBlendCriteriaHelper();

    bool      m_bDDBValid;
    int       m_iMyIndex;
    CBlendCriteria* m_pBCrit;
    DDBValueLstMem  m_DDBAdd;
    DDBValueLstMem  m_DDBRemove;

    void      SetUpDDB();
    void BuildDataDefn(FlwNode* pTagObj, DataDefnBlk & DDB, int CriteriaIndex);

  };

typedef CArray <CBlendCriteriaHelper*, CBlendCriteriaHelper*> BlendCriteriaArray;

//--------------------------------------------------------------------------

class CBlendControlHelper : public CBlendData
  {
  public:
   	enum eBlendMethod { eBM_Manual, eBM_Auto1 };
    CBlendControlHelper();
    ~CBlendControlHelper();

    void      BuildDataDefn(FlwNode* pTagObj, DataDefnBlk & DDB);
    void      BuildCriteriaDataDefn(FlwNode* pTagObj, DataDefnBlk & DDB);
    flag      DataXchg(DataChangeBlk & DCB);
    void      SetCriteriaCount(short CriteriaCnt);
    int       CriteriaCount() { return m_iCriteriaCnt; };
    void      InitBlendControl(long TankCount, long ComponentCnt);
    void      InitTanks(long TankCnt, long ComponentCnt);
    void      InitCriteria(long CriteriaCount);
    void      Optimise(CBlendCon* pBlender, bool SetIt);
    void      Optimise0(CBlendCon* pBlender, bool SetIt);
    void      Optimise1(CBlendCon* pBlender, bool SetIt);
    void      Optimise2(CBlendCon* pBlender, bool SetIt);
    void      Calculate(CBlendCon* pBlender, bool SetIt);

    //int       m_iMyIndex;
    long      m_iMethod;
    short     m_iStreamIndex;
    short     m_iIOIndex;
    short     m_iOptimiseMethod;
    TankMin*  tankMin;
    TankBlendOptimiser* tankOpt;

	  BlendCriteriaArray m_BCH;

  };

//typedef CArray <CBlendControlHelper*, CBlendControlHelper*> BlendControlArray;

//--------------------------------------------------------------------------

DEFINE_TAGOBJ(CBlendCon);
class CBlendCon : public FlwNode , public CXRefStatus 
  {
  public:

    CTODirectPtr<CMultiStorage>  m_pStorage; 
    flag       bValid:1;
    Strng      sMSTag; //Tag of MultiStore model
    Strng      sPipeTag; //Tag of MultiStore outlet pipe affected


    flag           bOn;                  //
    flag           bLogOptimiseErrors;
    flag           bLogOptimiseNotes;
    flag           bLogSetOptimiseData;
    Strng          m_StateLine[3];       //status messages
    flag           bDoneExtRefs:1,       //
                   bAboutToStart:1;      //flag set True for first iteration when run is pressed

    CBlendCon(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CBlendCon();
    virtual void   ResetData(flag Complete);
    bool IsXRefActive() const    { return bValid; };
  private:
    void           ResetAllStats();
  public:
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual void   GlobalConnect();
    virtual void   GlobalDisConnect();
    virtual flag   PreStartCheck();
    virtual void   EvalCtrlStrategy(eScdCtrlTasks Tasks=CO_All);
    virtual int    ChangeTag(pchar pOldTag, pchar pNewTag);
    virtual int    DeleteTag(pchar pDelTag);

    // CNodeXRefMngr Overides
    virtual bool   TestXRefListActive();
    virtual int    UpdateXRefLists(CXRefBuildResults & Results);
    virtual void   UnlinkAllXRefs();

    virtual void   EvalProducts(CNodeEvalIndex & NEI);
    virtual dword  ModelStatus();

    DEFINE_CI(CBlendCon, FlwNode, 4);

  public:

  //protected:

    //
    // Assay Definitions are Global - CAssays gs_Assays;
    // We will create a data entry page in the blend controller to
    // keep everything together.
    //

    // Blend Criteria
    CBlendControlHelper m_BC;

    // For display purposes
    CMatrixTB mStockpileAssays;
    #if WithMethod1
    // Blend Optimsation Input/Output Data
    CMatrixTB mAssays;
    CMatrixTB mHardConstraints;
    CMatrixTB mSoftConstraints;
    CMatrixTB mSoftRatios;
    CMatrixTB mX;
    CMatrixTB mSoftConstraintCosts;
    CMatrixTB mSoftRatiosCosts;
    #endif
  };
  
//===========================================================================

#endif

#undef DllImportExport

#endif
