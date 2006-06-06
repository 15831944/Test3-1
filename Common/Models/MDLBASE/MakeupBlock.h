#pragma once

#include "flwnode.h"
#include "blockevalbase.h"

#if WITHBLOCKEVALUATOR

#ifdef __MAKEUPBLOCK_CPP
#define DllImportExport DllExport
#elif !defined(MdlBase)
#define DllImportExport DllImport
#else
#define DllImportExport
#endif


// ==========================================================================
//
//
//
// ==========================================================================

class CMakeupBase;

// ==========================================================================
//
// Evaporation Blocks
//
// ==========================================================================

#define DEFINE_MAKEUPBLOCK(SM) DEFINE_TAGOBJ(SM)
#define IMPLEMENT_MAKEUPBLOCK(Obj, ModelId, Version, Cat, SDesc, LDesc) \
  IMPLEMENT_TAGOBJ(Obj, CMakeupBlock::GroupName, ModelId, Version, "", "", Cat, SDesc, LDesc)

class DllImportExport CMakeupBlock : public TaggedObject
  {
  public:
    CMakeupBlock(TagObjClass* pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach);
    virtual ~CMakeupBlock();

    virtual void    BuildDataDefn(DataDefnBlk& DDB);
    virtual flag    DataXchg(DataChangeBlk & DCB) { return 0; };

    virtual flag    ValidateData(ValidateDataBlk & VDB) { return 1; };
    virtual void    EvalProducts(SpConduit & Fo, double Po, double FinalTEst=dNAN);
    virtual void    EvalProductsPipe(SpConduit & Fo, double Len, double Diam, double Po, double FinalTEst=dNAN);

  public:
    static const pchar GroupName;

    CMakeupBase     * m_pMakeupBase;
  };

DEFINE_MAKEUPBLOCK(CMakeupBlock);

// ===========================================================================

class DllImportExport CXBlk_Makeup: public CMakeupBlock
  {
  public:
    CXBlk_Makeup(TagObjClass* pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach);
    virtual ~CXBlk_Makeup();

    virtual void   BuildDataDefn(DataDefnBlk& DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    virtual void   EvalProducts(SpConduit & Fo, double Po, double FinalTEst=dNAN);
    virtual void   EvalProductsPipe(SpConduit & Fo, double Len, double Diam, double Po, double FinalTEst=dNAN);

  public:
    enum eType     {Type_Qm, };

    eType           m_Type;
    double          m_QmRqd;

    //class CMakeupSpce
    //  {
    //  public:
    //    long      m_CIndex;
    //    byte      m_Dest;
    //    double    m_Value;
    //    double    m_;
    //  };

    //CArray <CMakeupComp, CMakeupComp&> m_Components;

  };

DEFINE_MAKEUPBLOCK(CXBlk_Makeup);

// ===========================================================================
//
// Evaporation Base
//
// ===========================================================================

class DllImportExport CMakeupBase : public CBlockEvalBase
  {
  public:

    CMakeupBase(TaggedObject * pAttach, int Index);
    ~CMakeupBase();

    virtual Strng  Name();

    flag           Open(TagObjClass * pEvapClass=NULL, flag Fixed=False);
    flag           Open(byte OnOffSeq, TagObjClass * pEvapClass=NULL, flag Fixed=False) { CBlockEvalBase::Open(OnOffSeq); return Open(pEvapClass, Fixed); }
    byte           OpenStatus() { return CBlockEvalBase::OpenStatus(Enabled()); }
    void           Close();

    CMakeupBlock *   operator->() { return m_pMakeupB; };
    flag           Enabled() { return m_fEnabled && (m_pMakeupB!=NULL); };
    void           Enable() { m_fEnabled = true; };
    void           Disable() { m_fEnabled = false; };
    void           Add_OnOff(DataDefnBlk &DDB, dword Flags=isParmStopped, int UserInfo=0);
    void           BuildDataDefn(DataDefnBlk &DDB, char* pTag="Adj", char* pTagComment=NULL, DDBPages PageIs=DDB_RqdPage, dword UserInfo=0);
    flag           DataXchg(DataChangeBlk & DCB);

    flag           ValidateData(ValidateDataBlk & VDB)
      { return Enabled() ? m_pMakeupB->ValidateData(VDB) : 0; };
    void           EvalProducts(SpConduit & Fo, double Po, double FinalTEst=dNAN)
      { if (Enabled()) m_pMakeupB->EvalProducts(Fo, Po, FinalTEst); };
    void           EvalProductsPipe(SpConduit & Fo, double Len, double Diam, double Po, double FinalTEst=dNAN)
      { if (Enabled()) m_pMakeupB->EvalProductsPipe(Fo, Len, Diam, Po, FinalTEst); };

    SpConduit    & DiscardCd() { return m_DiscardCd; };

  protected:
    flag              m_fFixed;
    flag              m_fEnabled;
    CMakeupBlock    * m_pMakeupB;
    TaggedObject    * m_pNd;
    int               m_Index;
    //Strng             m_sTag;

    SpConduit         m_DiscardCd;

  public:

  };

#undef DllImportExport

#endif