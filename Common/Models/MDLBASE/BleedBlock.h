#pragma once

#include "flwnode.h"
#include "blockevalbase.h"

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

class CBleedBase;

// ==========================================================================
//
// Bleed Blocks
//
// ==========================================================================

#define DEFINE_BLEEDBLOCK(SM) DEFINE_TAGOBJ(SM)
#define IMPLEMENT_BLEEDBLOCK(Obj, ModelId, Version, Cat, SDesc, LDesc) \
  IMPLEMENT_TAGOBJ(Obj, CBleedBlock::GroupName, ModelId, Version, "", "", Cat, SDesc, LDesc)

class DllImportExport CBleedBlock : public TaggedObject
  {
  public:
    CBleedBlock(TagObjClass* pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach);
    virtual ~CBleedBlock();

    virtual flag    DoesSomething() { return false; };

    virtual void    BuildDataDefn(DataDefnBlk& DDB);
    virtual flag    DataXchg(DataChangeBlk & DCB) { return 0; };

    virtual flag    ValidateData(ValidateDataBlk & VDB) { return 1; };
    virtual void    EvalProducts(SpConduit & QPrd, double Po, double FinalTEst=dNAN);
    virtual void    EvalProductsPipe(SpConduit & QPrd, double Len, double Diam, double Po, double FinalTEst=dNAN);

    inline CDirectFlwIO   & getSnkIO();

  public:
    static const pchar GroupName;

    CBleedBase     * m_pBleedBase;

    _declspec(property(get=getSnkIO))          CDirectFlwIO   & SnkIO;    
  };

DEFINE_BLEEDBLOCK(CBleedBlock);

// ===========================================================================
//
// Bleed Base
//
// ===========================================================================

class DllImportExport CBleedBase : public CBlockEvalBase
  {
  friend class CBleedBlock;
  friend class CBlockEvaluator;
  public:

    CBleedBase(TaggedObject * pAttach, int Index, LPTSTR Name);
    ~CBleedBase();

    //virtual Strng  Name();

    flag           Open(TagObjClass * pEvapClass=NULL, flag Fixed=False);
    flag           Open(byte OnOffSeq, TagObjClass * pEvapClass=NULL, flag Fixed=False) { CBlockEvalBase::Open(OnOffSeq); return Open(pEvapClass, Fixed); }
    byte           OpenStatus() { return CBlockEvalBase::OpenStatus(Enabled()); }
    void           Close();

    CBleedBlock *   operator->() { return m_pBleedB; };
    flag           Enabled() { return m_fEnabled && (m_pBleedB!=NULL); };
    void           Enable() { m_fEnabled = true; };
    void           Disable() { m_fEnabled = false; };
    void           Add_OnOff(DataDefnBlk &DDB, dword Flags=isParmStopped, int UserInfo=0);
    void           BuildDataDefn(DataDefnBlk &DDB, char* pTag="Adj", char* pTagComment=NULL, DDBPages PageIs=DDB_RqdPage, dword UserInfo=0);
    flag           DataXchg(DataChangeBlk & DCB);

    flag           ValidateData(ValidateDataBlk & VDB)
      { return Enabled() ? m_pBleedB->ValidateData(VDB) : 0; };
    void           EvalProducts(SpConduit & QPrd, double Po, double FinalTEst=dNAN)
      { if (Enabled()) m_pBleedB->EvalProducts(QPrd, Po, FinalTEst); };
    void           EvalProductsPipe(SpConduit & QPrd, double Len, double Diam, double Po, double FinalTEst=dNAN)
      { if (Enabled()) m_pBleedB->EvalProductsPipe(QPrd, Len, Diam, Po, FinalTEst); };

  protected:
    flag              m_fFixed;
    flag              m_fEnabled;
    CBleedBlock     * m_pBleedB;
    TaggedObject    * m_pNd;

    CDirectFlwIO      m_SnkIO;

  public:

  };

// ===========================================================================
//
// 
//
// ===========================================================================

CDirectFlwIO & CBleedBlock::getSnkIO() { return m_pBleedBase->m_SnkIO; };    

// ===========================================================================
//
// 
//
// ===========================================================================

#undef DllImportExport
