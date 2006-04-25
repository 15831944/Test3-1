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

//#if WITHBLOCKEVALUATOR

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
    static const byte BES_Blocks       = 0x0f;
    static const byte BES_On           = 0x40;
    static const byte BES_Simultaneous = 0x80;

    CBlockEvaluator(CReactionBase * pRB = NULL, 
                    CHXBase *pHX = NULL, 
                    CEnvironHXBase * pEHX = NULL, 
                    CVLEBase * pVLE = NULL, 
                    CEvapBase * pEvap = NULL);
    ~CBlockEvaluator(void);

    void              Add_OnOff(TaggedObject * pThis, DataDefnBlk &DDB, DDBPages PageIs=DDB_NoPage);
    void              BuildDataDefn(TaggedObject * pThis, DataDefnBlk &DDB, DDBPages PageIs=DDB_RqdPage);
    flag              DataXchg(DataChangeBlk & DCB);
    flag              ValidateData(ValidateDataBlk & VDB);
    void              SortBlocks();

    void              EvalProducts(CFlwThermalBlk & FTB, SpConduit & Fo, double Po, double FinalTEst=dNAN);
    void              EvalProductsPipe(CFlwThermalBlk & FTB, SpConduit & Fo, double Len, double Diam, double Po, double FinalTEst=dNAN);

  protected:
    byte              m_Sequence[MaxBEBlocks+1];

    CReactionBase   * m_pRB;
    CHXBase         * m_pHX;
    CEnvironHXBase  * m_pEHX;
    CVLEBase        * m_pVLE;
    CEvapBase       * m_pEvap;

    DDBValueLstMem    m_ValLst;  
  };

//#endif

#undef DllImportExport
