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
// Makeup Blocks
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

    inline CDirectFlwIO   & getIn();

  public:
    static const pchar GroupName;

    CMakeupBase     * m_pMakeupBase;

    _declspec(property(get=getIn))          CDirectFlwIO   & In;    

  };

DEFINE_MAKEUPBLOCK(CMakeupBlock);

// ===========================================================================
//
// Makeup Base
//
// ===========================================================================

class DllImportExport CMakeupBase : public CBlockEvalBase
  {
  friend class CMakeupBlock;
  friend class CBlockEvaluator;

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

  protected:
    flag              m_fFixed;
    flag              m_fEnabled;
    CMakeupBlock    * m_pMakeupB;
    TaggedObject    * m_pNd;

    CDirectFlwIO      m_In;

  public:

  };

// ===========================================================================
//
// 
//
// ===========================================================================

CDirectFlwIO & CMakeupBlock::getIn() { return m_pMakeupBase->m_In; };    

// ===========================================================================
//
// 
//
// ===========================================================================


#undef DllImportExport

#endif