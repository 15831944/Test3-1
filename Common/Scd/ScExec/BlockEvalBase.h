#pragma once

#include "datatype.h"
#include "executiv.h"

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
const int BlkEval_State = 100;

const byte BEId_Null         = 0;
const byte BEId_RB           = 1;
const byte BEId_HX           = 2;
const byte BEId_EHX          = 3;
const byte BEId_VLE          = 4;
const byte BEId_Evap         = 5;
const byte BEId_Makeup       = 6;
const byte BEId_Bleed        = 7;

class DllImportExport CBlockEvalBase
  {
  public:
    CBlockEvalBase(byte BEId, int Index, LPTSTR Name, bool HasStateSemantics);
  public:
    virtual ~CBlockEvalBase(void);

    //static void       BuildOnOffValLst(DDBValueLstMem  * ValLst, int NInSequence, LPCSTR StateName);

    byte              BEId()                       { return m_BEId; };
    int               Index()                      { return m_Index; };

    byte              BlkSeqNo(bool ForSort=false);
    void              SetBlkSeqNo(byte SeqNo)      { m_iBlkSeqNo=SeqNo; };

    byte              DefBlkSeqNo()                { return m_iDefBlkSeqNo; };
    void              SetDefBlkSeqNo(byte SeqNo)   { m_iDefBlkSeqNo=SeqNo; };


    void              SetOnOffValLst(DDBValueLstMem  * ValLst/*, DDBValueLstMem * StateValLst*/);
    DDBValueLst     * GetOnOffValLst(/*bool WithState=false*/);

    bool              HasStateSemantics()          { return m_bHasStateSemantics; };
    bool              StateSemanticsSelected()     { return m_bHasStateSemantics && (BlkSeqNo()==BlkEval_State); };

    void              Open(byte L);
    byte              OpenStatus(flag Enabled);

    virtual LPTSTR    Name();
    virtual flag      Enabled() { ASSERT(FALSE); return false; };

    virtual int       ChangeTag(char * pOldTag, char * pNewTag) { return EOCT_DONE; };
    virtual int       DeleteTag(char * pDelTag)                 { return EOCT_DONE; };


  protected:
    byte              m_iBlkSeqNo;
    byte              m_iDefBlkSeqNo;
    byte              m_BEId;
    int               m_Index;
    bool              m_bHasStateSemantics;
    DDBValueLstMem  * m_pOnOffValLst;
    //DDBValueLstMem  * m_pOnOffStateValLst;
    Strng             m_sName;

  };

#undef DllImportExport
