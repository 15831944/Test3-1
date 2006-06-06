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
const int BlkEval_On    = 1;
const int BlkEval_First = 2;

const byte BEId_Null         = 0;
const byte BEId_RB           = 1;
const byte BEId_HX           = 2;
const byte BEId_EHX          = 3;
const byte BEId_VLE          = 4;
const byte BEId_Evap         = 5;
const byte BEId_Adj          = 6;

class DllImportExport CBlockEvalBase
  {
  public:
    CBlockEvalBase(byte BEId);
  public:
    virtual ~CBlockEvalBase(void);

    static void       BuildOnOffValLst(DDBValueLstMem  * ValLst, int NInSequence);

    byte              BEId()                       { return m_BEId; };

    byte              BlkSeqNo(bool ForSort=false);
    void              SetBlkSeqNo(byte SeqNo)      { m_iBlkSeqNo=SeqNo; };

    byte              DefBlkSeqNo()                { return m_iDefBlkSeqNo; };
    void              SetDefBlkSeqNo(byte SeqNo)   { m_iDefBlkSeqNo=SeqNo; };


    void              SetOnOffValLst(DDBValueLstMem  * ValLst);
    DDBValueLst     * GetOnOffValLst();

    void              Open(byte L);
    byte              OpenStatus(flag Enabled);

    virtual Strng     Name();
    virtual flag      Enabled() { ASSERT(FALSE); return false; };

  protected:
    byte              m_iBlkSeqNo;
    byte              m_iDefBlkSeqNo;
    byte              m_BEId;
    DDBValueLstMem  * m_pOnOffValLst;
  };

#undef DllImportExport
