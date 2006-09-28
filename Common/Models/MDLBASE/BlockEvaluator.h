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

    static const int MaxMakeupBlocks       =  5;
    static const int MaxBleedBlocks        =  5;
    //                                     RB, HX, EHX, VLE
    static const int MaxBEBlocks           =   1  +1   +1   +1   +MaxMakeupBlocks+MaxBleedBlocks;

    CBlockEvaluator(FlwNode * pThis,
                    CReactionBase * pRB = NULL,
                    CHXBase *pHX = NULL,
                    CEnvironHXBase * pEHX = NULL,
                    CVLEBase * pVLE = NULL,
                    CEvapBase * pEvap = NULL);
    ~CBlockEvaluator(void);

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
    void              EvalProductsPipe(SpConduit & Fo, double Len, double Diam, double Po, CFlwThermalBlk * pFTB, double FinalTEst=dNAN);

    int               ChangeTag(char * pOldTag, char * pNewTag);
    int               DeleteTag(char * pDelTag);

  protected:
    FlwNode         * m_pThis;
    int               m_nBlocks;
    //int               m_nBEInSeq;
    //byte              m_BEInSeq[MaxBEBlocks+1];
    CBlockEvalBase  * m_Blks[MaxBEBlocks+1];


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
