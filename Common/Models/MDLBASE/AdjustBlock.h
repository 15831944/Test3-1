#pragma once

#include "flwnode.h"
#include "blockevalbase.h"

#if WITHBLOCKEVALUATOR

#ifdef __XBLOCKBLOCK_CPP
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

class CAdjustBase;

// ==========================================================================
//
// Evaporation Blocks
//
// ==========================================================================

#define DEFINE_XBLOCK(SM) DEFINE_TAGOBJ(SM)
#define IMPLEMENT_XBLOCK(Obj, ModelId, Version, Cat, SDesc, LDesc) \
  IMPLEMENT_TAGOBJ(Obj, CXBlock::GroupName, ModelId, Version, "", "", Cat, SDesc, LDesc)

class DllImportExport CXBlock : public TaggedObject
  {
  public:
    CXBlock(TagObjClass* pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach);
    virtual ~CXBlock();

    virtual void    BuildDataDefn(DataDefnBlk& DDB);
    virtual flag    DataXchg(DataChangeBlk & DCB) { return 0; };

    virtual flag    ValidateData(ValidateDataBlk & VDB) { return 1; };
    virtual void    EvalProducts(SpConduit & Fo, double Po, double FinalTEst=dNAN);
    virtual void    EvalProductsPipe(SpConduit & Fo, double Len, double Diam, double Po, double FinalTEst=dNAN);

  public:
    static const pchar GroupName;

    CAdjustBase     * m_pAdjustBase;
  };

DEFINE_XBLOCK(CXBlock);

// ===========================================================================

class DllImportExport CXBlk_Makeup: public CXBlock
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

    //class CAdjustSpce
    //  {
    //  public:
    //    long      m_CIndex;
    //    byte      m_Dest;
    //    double    m_Value;
    //    double    m_;
    //  };

    //CArray <CAdjustComp, CAdjustComp&> m_Components;

  };

DEFINE_XBLOCK(CXBlk_Makeup);

// ===========================================================================

class DllImportExport CXBlk_Bleed: public CXBlock
  {
  public:
    CXBlk_Bleed(TagObjClass* pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach);
    virtual ~CXBlk_Bleed();

    virtual void   BuildDataDefn(DataDefnBlk& DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    virtual void   EvalProducts(SpConduit & Fo, double Po, double FinalTEst=dNAN);
    virtual void   EvalProductsPipe(SpConduit & Fo, double Len, double Diam, double Po, double FinalTEst=dNAN);

  public:

    enum eType      { Type_TotQm, Type_TotFrac, };

    eType           m_Type;
    double          m_QmBleed;
    double          m_Frac;

    //class CAdjustSpce
    //  {
    //  public:
    //    long      m_CIndex;
    //    byte      m_Dest;
    //    double    m_Value;
    //    double    m_;
    //  };

    //CArray <CAdjustComp, CAdjustComp&> m_Components;

  };

DEFINE_XBLOCK(CXBlk_Bleed);

// ===========================================================================
//
// Evaporation Base
//
// ===========================================================================

class DllImportExport CAdjustBase : public CBlockEvalBase
  {
  public:

    CAdjustBase(TaggedObject * pAttach, int Index);
    ~CAdjustBase();

    virtual Strng  Name();

    flag           Open(TagObjClass * pEvapClass=NULL, flag Fixed=False);
    flag           Open(byte OnOffSeq, TagObjClass * pEvapClass=NULL, flag Fixed=False) { CBlockEvalBase::Open(OnOffSeq); return Open(pEvapClass, Fixed); }
    byte           OpenStatus() { return CBlockEvalBase::OpenStatus(Enabled()); }
    void           Close();

    CXBlock *   operator->() { return m_pAdjustB; };
    flag           Enabled() { return m_fEnabled && (m_pAdjustB!=NULL); };
    void           Enable() { m_fEnabled = true; };
    void           Disable() { m_fEnabled = false; };
    void           Add_OnOff(DataDefnBlk &DDB, dword Flags=isParmStopped, int UserInfo=0);
    void           BuildDataDefn(DataDefnBlk &DDB, char* pTag="Adj", char* pTagComment=NULL, DDBPages PageIs=DDB_RqdPage, dword UserInfo=0);
    flag           DataXchg(DataChangeBlk & DCB);

    flag           ValidateData(ValidateDataBlk & VDB)
      { return Enabled() ? m_pAdjustB->ValidateData(VDB) : 0; };
    void           EvalProducts(SpConduit & Fo, double Po, double FinalTEst=dNAN)
      { if (Enabled()) m_pAdjustB->EvalProducts(Fo, Po, FinalTEst); };
    void           EvalProductsPipe(SpConduit & Fo, double Len, double Diam, double Po, double FinalTEst=dNAN)
      { if (Enabled()) m_pAdjustB->EvalProductsPipe(Fo, Len, Diam, Po, FinalTEst); };

    SpConduit    & DiscardCd() { return m_DiscardCd; };

  protected:
    flag              m_fFixed;
    flag              m_fEnabled;
    CXBlock    * m_pAdjustB;
    TaggedObject    * m_pNd;
    int               m_Index;
    //Strng             m_sTag;

    SpConduit         m_DiscardCd;

  public:

  };

#undef DllImportExport

#endif