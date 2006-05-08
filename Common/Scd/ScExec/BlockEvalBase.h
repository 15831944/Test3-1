#pragma once

#ifdef __BLOCKEVALBASE_CPP
  #define DllImportExport DllExport
#elif !defined(MdlBase)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

class DllImportExport CBlockEvalBase
  {
  public:
    CBlockEvalBase(void);
  public:
    ~CBlockEvalBase(void);

    byte BlockSeqNo()             { return m_iBlockSeqNo; };
    void BlockSeqNo(byte SeqNo)   { m_iBlockSeqNo=SeqNo; };
    //void OnOpen()                 { };
    //void OnClose()                { m_iBlockSeqNo=0; };

  protected:
    byte m_iBlockSeqNo;
  };

#undef DllImportExport
