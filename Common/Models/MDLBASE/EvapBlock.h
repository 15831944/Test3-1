#pragma once

#include "flwnode.h"
#include "blockevalbase.h"

#ifdef __EVAPBLOCK_CPP
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

class CEvapBase;

// ==========================================================================
//
// Evaporation Blocks
//
// ==========================================================================

#define DEFINE_EVAL(SM) DEFINE_TAGOBJ(SM)
#define IMPLEMENT_EVAP(Obj, ModelId, Version, Cat, SDesc, LDesc) \
  IMPLEMENT_TAGOBJ(Obj, CEvapBlock::GroupName, ModelId, Version, "", "", Cat, SDesc, LDesc)

class DllImportExport CEvapBlock : public TaggedObject
  {
  public:
    CEvapBlock(TagObjClass* pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach);
    virtual ~CEvapBlock();

    virtual void    BuildDataDefn(DataDefnBlk& DDB);
    virtual flag    DataXchg(DataChangeBlk & DCB) { return 0; };

    virtual flag    ValidateData(ValidateDataBlk & VDB) { return 1; };
    virtual void    EvalProducts(SpConduit & Fo, double Po, double FinalTEst=dNAN);
    virtual void    EvalProductsPipe(SpConduit & Fo, double Len, double Diam, double Po, double FinalTEst=dNAN);

  public:
    static const pchar GroupName;

    CEvapBase     * m_pEvapBase;
  };

DEFINE_EVAL(CEvapBlock);

// ===========================================================================

class DllImportExport CEvBlk_Percentage: public CEvapBlock
  {
  public:
    CEvBlk_Percentage(TagObjClass* pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach);
    virtual ~CEvBlk_Percentage();

    virtual void   BuildDataDefn(DataDefnBlk& DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    virtual void   EvalProducts(SpConduit & Fo, double Po, double FinalTEst=dNAN);
    virtual void   EvalProductsPipe(SpConduit & Fo, double Len, double Diam, double Po, double FinalTEst=dNAN);

  public:

    class CEvapComp
      {
      public:
        long      m_CIndex;
        byte      m_Dest;
        double    m_Fraction;
      };

    CArray <CEvapComp, CEvapComp&> m_Components;

  };

DEFINE_EVAL(CEvBlk_Percentage);

// ===========================================================================
//
// Evaporation Base
//
// ===========================================================================

class DllImportExport CEvapBase : public CBlockEvalBase
  {
  public:

    CEvapBase(TaggedObject * pAttach, LPTSTR Name="Evap") : CBlockEvalBase(BEId_Evap, 0, Name) { m_pEvapB=NULL; m_pNd=pAttach; m_fEnabled=m_fFixed=False; };
    virtual ~CEvapBase()              { delete m_pEvapB; };

    flag           Open(TagObjClass * pEvapClass=NULL, flag Fixed=False);
    flag           Open(byte OnOffSeq, TagObjClass * pEvapClass=NULL, flag Fixed=False) { CBlockEvalBase::Open(OnOffSeq); return Open(pEvapClass, Fixed); }
    byte           OpenStatus() { return CBlockEvalBase::OpenStatus(Enabled()); }
    void           Close();

    CEvapBlock *   operator->() { return m_pEvapB; };
    flag           Enabled() { return m_fEnabled && (m_pEvapB!=NULL); };
    void           Enable() { m_fEnabled = true; };
    void           Disable() { m_fEnabled = false; };
    void           Add_OnOff(DataDefnBlk &DDB, dword Flags=isParmStopped);
    void           BuildDataDefn(DataDefnBlk &DDB, char* pTag="Evap", char* pTagComment=NULL, DDBPages PageIs=DDB_RqdPage, dword UserInfo=0);
    flag           DataXchg(DataChangeBlk & DCB);

    flag           ValidateData(ValidateDataBlk & VDB)
      { return Enabled() ? m_pEvapB->ValidateData(VDB) : 0; };
    void           EvalProducts(SpConduit & Fo, double Po, double FinalTEst=dNAN)
      { if (Enabled()) m_pEvapB->EvalProducts(Fo, Po, FinalTEst); };
    void           EvalProductsPipe(SpConduit & Fo, double Len, double Diam, double Po, double FinalTEst=dNAN)
      { if (Enabled()) m_pEvapB->EvalProductsPipe(Fo, Len, Diam, Po, FinalTEst); };

    SpConduit    & DiscardCd() { return m_DiscardCd; };

  protected:
    flag            m_fFixed;
    flag            m_fEnabled;
    CEvapBlock    * m_pEvapB;
    pTaggedObject   m_pNd;

    SpConduit       m_DiscardCd;

  public:

  };

#undef DllImportExport
