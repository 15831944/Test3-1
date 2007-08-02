#pragma once

#include "vlebase.h"
#include "sp_react.h"
#include "m_hxbase.h"
#include "evapblock.h"
#include "makeupblock.h"
#include "bleedblock.h"

#ifdef __BLOCKEVALUATOR_CPP
  #define DllImportExport DllExport
#elif !defined(MdlBase)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

class DllImportExport CBlockEvaluator
  {
  public:

    //                                     RB, HX, EHX, VLE
    static const int MaxBEBlocks           =   1  +1   +1   +1   +MaxNdMakeups+MaxNdBleeds;

    CBlockEvaluator(FlwNode * pThis,
                    CReactionBase * pRB = NULL,
                    CHXBase *pHX = NULL,
                    CEnvironHXBase * pEHX = NULL,
                    CVLEBase * pVLE = NULL,
                    CEvapBase * pEvap = NULL);
    ~CBlockEvaluator(void);

    void Attach(CReactionBase * pRB = NULL,
                CHXBase *pHX = NULL,
                CEnvironHXBase * pEHX = NULL,
                CVLEBase * pVLE = NULL,
                CEvapBase * pEvap = NULL);

    void              SetEnable(bool On);

    void              AddBlk(CBlockEvalBase *p, int DefSeqNo);
    void              RemBlk(CBlockEvalBase *p);

    //CFlange         * GetFlange(int IOId);

    void              Add_OnOff(DataDefnBlk &DDB, DDBPages PageIs=DDB_NoPage);
    void              BuildDataDefn(DataDefnBlk &DDB, DDBPages PageIs=DDB_RqdPage);
    flag              DataXchg(DataChangeBlk & DCB);
    flag              ValidateData(ValidateDataBlk & VDB);
    //int               DefBlkSeqNo(byte SeqID);
    //int               BlkSeqNo(byte SeqID);
    //void              SetBlkSeqNo(byte SeqID, byte No);
    void              SortBlocks();

    void              EvalProducts(SpConduit & Fo, double Po, CFlwThermalBlk * pFTB, double FinalTEst=dNAN);
    void              EvalProductsInline(SpConduit & Fo, double Len, double Diam, double Po, CFlwThermalBlk * pFTB, double FinalTEst=dNAN);

    int               ChangeTag(char * pOldTag, char * pNewTag);
    int               DeleteTag(char * pDelTag);

    int               MakeupCount() { return m_pMakeups.GetCount(); };
    int               BleedCount()  { return m_pBleeds.GetCount(); };

    void              SetMakeupCount(int N);
    void              SetBleedCount(int N);

    bool              RBAttached()      { return m_pRB!=NULL; };
    bool              HXAttached()      { return m_pHX!=NULL; };
    bool              EHXAttached()     { return m_pEHX!=NULL; };
    bool              VLEAttached()     { return m_pVLE!=NULL; };
    bool              EvapAttached()    { return m_pEvap!=NULL; };

  protected:
    FlwNode         * m_pThis;
    int               m_nBlocks;
    //int               m_nBEInSeq;
    //byte              m_BEInSeq[MaxBEBlocks+1];
    CBlockEvalBase  * m_Blks[MaxBEBlocks+1];


    int               m_nMaxNdMakeups;
    CReactionBase   * m_pRB;
    CHXBase         * m_pHX;
    CEnvironHXBase  * m_pEHX;
    CVLEBase        * m_pVLE;
    CEvapBase       * m_pEvap;
    CArray <CMakeupBase*, CMakeupBase*> m_pMakeups;
    CArray <CBleedBase*, CBleedBase*> m_pBleeds;

    Strng             m_sBlkSeq;
    DDBValueLstMem    m_OnOffValLst;  
  };


#undef DllImportExport
