#pragma once

#include "datatype.h"

#ifdef __BLOCKEVALBASE_CPP
  #define DllImportExport DllExport
#elif !defined(MdlBase)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

const int BlkEval_Off   = 0;
const int BlkEval_On    = 10;
const int BlkEval_First = 1;


class DllImportExport CBlockEvalBase
  {
  public:
    CBlockEvalBase(void);
  public:
    ~CBlockEvalBase(void);

    static void       BuildOnOffValLst(DDBValueLstMem  * ValLst, int NInSequence);

    byte              BlockSeqNo()             { return m_iBlockSeqNo; };
    void              BlockSeqNo(byte SeqNo)   { m_iBlockSeqNo=SeqNo; };

    void              SetOnOffValLst(DDBValueLstMem  * ValLst);
    DDBValueLst     * GetOnOffValLst();

    void              Open(long L);
    long              OpenStatus(flag Enabled);

  protected:
    byte              m_iBlockSeqNo;
    DDBValueLstMem  * m_pOnOffValLst;
  };

#undef DllImportExport
