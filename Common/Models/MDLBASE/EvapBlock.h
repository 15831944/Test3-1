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

enum EvapBlockTypes { EBT_None, EBT_Frac, EBT_Flow, EBT_TDiff };

#define DEFINE_EVAP(SM) DEFINE_TAGOBJ(SM)
#define IMPLEMENT_EVAP(Obj, ModelId, Version, Cat, SDesc, LDesc) \
  IMPLEMENT_TAGOBJ(Obj, CEvapBlock::GroupName, ModelId, Version, "", "", Cat, SDesc, LDesc)

class DllImportExport CEvapBlock : public TaggedObject
  {
  public:
    CEvapBlock(TagObjClass* pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach);
    virtual ~CEvapBlock();

    virtual void    BuildDataDefn(DataDefnBlk& DDB);
    void            DoBuildDataDefn(DataDefnBlk& DDB);
    virtual flag    DataXchg(DataChangeBlk & DCB); 
    virtual flag    ValidateData(ValidateDataBlk & VDB);

    virtual void    EvalProducts(SpConduit & Fo, double Po, double FinalTEst=dNAN);
    virtual void    EvalProductsInline(SpConduit & Fo, double Len, double Diam, double Po, double FinalTEst=dNAN);

  public:
    static const pchar GroupName;

    EvapBlockTypes  m_Type;
    CEvapBase     * m_pEvapBase;

  public:
    byte           m_Dest;
    double         m_EvapFrac;
    double         m_QmEvap;
    double         m_QmBleed;
    double         m_QmFeed;
    double         m_QmProd;
    double         m_HeatFlow;
    double         m_TempFeed;
    double         m_TempProd;
    double         m_PrevUsage;


    DualDbl        m_AmbientT;

    class CEvapComp
      {
      public:
        long      m_CIndex;
        double    m_QmFrac;
        double    m_QmRqd;
        double    m_OpTDiff;
        double    m_OpQm;
        double    m_PwrLaw;
      };

    CArray <CEvapComp, CEvapComp&> m_Components;

    DEFINE_CI(CEvapBlock, TaggedObject, 8);
  };

DEFINE_EVAP(CEvapBlock);

// ===========================================================================
//
// Evaporation 
//
// ===========================================================================

class DllImportExport CEvBlk_Fraction: public CEvapBlock
  {
  public:
    CEvBlk_Fraction(TagObjClass* pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach);
    virtual ~CEvBlk_Fraction();

    virtual void   BuildDataDefn(DataDefnBlk& DDB);
  };

// ===========================================================================
//
// Evaporation 
//
// ===========================================================================

DEFINE_EVAP(CEvBlk_Fraction);

class DllImportExport CEvBlk_Flow: public CEvapBlock
  {
  public:
    CEvBlk_Flow(TagObjClass* pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach);
    virtual ~CEvBlk_Flow();

    virtual void   BuildDataDefn(DataDefnBlk& DDB);
  };

DEFINE_EVAP(CEvBlk_Flow);

// ===========================================================================
//
// Evaporation 
//
// ===========================================================================

class DllImportExport CEvBlk_TDiff: public CEvapBlock
  {
  public:
    CEvBlk_TDiff(TagObjClass* pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach);
    virtual ~CEvBlk_TDiff();

    virtual void   BuildDataDefn(DataDefnBlk& DDB);

  };

DEFINE_EVAP(CEvBlk_TDiff);

// ===========================================================================
//
// Evaporation Base
//
// ===========================================================================

class DllImportExport CEvapBase : public CBlockEvalBase
  {
  public:

    CEvapBase(TaggedObject * pAttach, bool StateSemantics, LPTSTR Name="Evap") : CBlockEvalBase(BEId_Evap, 0, Name, StateSemantics) { m_pEvapB=NULL; m_pNd=pAttach; m_fEnabled=m_fFixed=False; };
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
    void           EvalProductsInline(SpConduit & Fo, double Len, double Diam, double Po, double FinalTEst=dNAN)
      { if (Enabled()) m_pEvapB->EvalProductsInline(Fo, Len, Diam, Po, FinalTEst); };

    SpConduit    & DiscardCd()  { return m_DiscardCd; };
    TaggedObject & Nd()         { return *m_pNd; };

  protected:
    flag            m_fFixed;
    flag            m_fEnabled;
    CEvapBlock    * m_pEvapB;
    pTaggedObject   m_pNd;

    SpConduit       m_DiscardCd;

  public:

  };

#undef DllImportExport
