#pragma once

#include "vlebase.h"
#include "sp_react.h"
#include "m_hxbase.h"
#include "evapblock.h"

#ifdef __BLOCKEVALUATOR_CPP
  #define DllImportExport DllExport
#elif !defined(MdlBase)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//#undef    WITHBLOCKEVALUATOR
//#define   WITHBLOCKEVALUATOR        0

#if WITHBLOCKEVALUATOR

class DllImportExport CBlockEvaluator
  {
  public:
    static const int MaxBEBlocks = 5;

    static const byte BES_Null         = 0x00;
    static const byte BES_RB           = 0x01;
    static const byte BES_HX           = 0x02;
    static const byte BES_EHX          = 0x03;
    static const byte BES_VLE          = 0x04;
    static const byte BES_Evap         = 0x05;

    CBlockEvaluator(CReactionBase * pRB = NULL,   byte iRB = 0, 
                    CHXBase *pHX = NULL,          byte iHX = 0, 
                    CEnvironHXBase * pEHX = NULL, byte iEHX = 0, 
                    CVLEBase * pVLE = NULL,       byte iVLE = 0, 
                    CEvapBase * pEvap = NULL,     byte iEvap = 0);
    ~CBlockEvaluator(void);

    void              SetDefSequence(byte iRB = 0, 
                                     byte iHX = 0, 
                                     byte iEHX = 0, 
                                     byte iVLE = 0, 
                                     byte iEvap = 0);

    void              Add_OnOff(TaggedObject * pThis, DataDefnBlk &DDB, DDBPages PageIs=DDB_NoPage);
    void              BuildDataDefn(TaggedObject * pThis, DataDefnBlk &DDB, DDBPages PageIs=DDB_RqdPage);
    flag              DataXchg(DataChangeBlk & DCB);
    flag              ValidateData(ValidateDataBlk & VDB);
    int               DefBlkSeqNo(byte SeqID);
    int               BlkSeqNo(byte SeqID);
    void              SetBlkSeqNo(byte SeqID, byte No);
    void              SortBlocks();

    void              EvalProducts(CFlwThermalBlk & FTB, SpConduit & Fo, double Po, double FinalTEst=dNAN);
    void              EvalProductsPipe(CFlwThermalBlk & FTB, SpConduit & Fo, double Len, double Diam, double Po, double FinalTEst=dNAN);

  protected:
    int               m_nBlocks;
    int               m_nSequence;
    byte              m_Sequence[MaxBEBlocks+1];

    CReactionBase   * m_pRB;
    CHXBase         * m_pHX;
    CEnvironHXBase  * m_pEHX;
    CVLEBase        * m_pVLE;
    CEvapBase       * m_pEvap;

    byte              m_RBDefSeq; 
    byte              m_HXDefSeq; 
    byte              m_EHXDefSeq; 
    byte              m_VLEDefSeq; 
    byte              m_EvapDefSeq;

    Strng             m_sBlkSeq;
    DDBValueLstMem    m_OnOffValLst;  
  };

#endif

#undef DllImportExport
