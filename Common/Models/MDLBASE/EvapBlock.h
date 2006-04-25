#pragma once

#include "flwnode.h"

#ifdef __EVAPBLOCK_CPP
  #define DllImportExport DllExport
#elif !defined(MdlBase)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ==========================================================================
//
// Environmental Heat xchg
//
// ==========================================================================

#define DEFINE_EHX(SM) DEFINE_TAGOBJ(SM)
#define IMPLEMENT_EVAP(Obj, ModelId, Version, Cat, SDesc, LDesc) \
        IMPLEMENT_TAGOBJ(Obj, CEvapBlock::GroupName, ModelId, Version, "", "", Cat, SDesc, LDesc)

class DllImportExport CEvapBlock : public TaggedObject
  {
  public:
    static const pchar GroupName;

    //double       dHeatFlow;
    //double       TempKFeed;
    //double       TempKProd;

    CEvapBlock(TagObjClass* pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach);
    virtual ~CEvapBlock();

    virtual void   BuildDataDefn(DataDefnBlk& DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB) { return 0; };

    virtual flag   ValidateData(ValidateDataBlk & VDB) { return 1; };
    //virtual void   EvalProducts(SpConduit & Qf, double FinalTEst=dNAN);
    //virtual void   EvalProductsPipe(SpConduit & Qf, double Len, double Diam, double FinalTEst=dNAN);
    //virtual double HeatFlow() { return dHeatFlow; };
    virtual void   EvalProducts(SpConduit & Fo, double Po, double FinalTEst=dNAN);
    virtual void   EvalProductsPipe(SpConduit & Fo, double Len, double Diam, double Po, double FinalTEst=dNAN);
  
  };

DEFINE_EHX(CEvapBlock);

// ===========================================================================

class DllImportExport CEvBlk_Percentage: public CEvapBlock
  {
  public:
    //double         dHperQm;

    CEvBlk_Percentage(TagObjClass* pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach);
    virtual ~CEvBlk_Percentage();

    virtual void   BuildDataDefn(DataDefnBlk& DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);

    virtual void   EvalProducts(SpConduit & Fo, double Po, double FinalTEst=dNAN);
    virtual void   EvalProductsPipe(SpConduit & Fo, double Len, double Diam, double Po, double FinalTEst=dNAN);
  };

DEFINE_EHX(CEvBlk_Percentage);

//class DllImportExport CEvapBlock
//  {
//  public:
//    CEvapBlock(void);
//  public:
//    ~CEvapBlock(void);
//  };

// ===========================================================================
//
//
//
// ===========================================================================

class DllImportExport CEvapBase
  {
  public:

    CEvapBase(TaggedObject * pAttach) { m_pEvapB=NULL; m_pNd=pAttach; m_fEnabled=m_fFixed=False; };
    virtual ~CEvapBase()              { delete m_pEvapB; };

    flag           Open(TagObjClass * pEvapClass=NULL, flag Fixed=False);
    void           Close();

    //int TPFlashReqd() { return (dwSelectMask&VLEF_TPFlash)!=0; };
    //int QPFlashReqd() { return (dwSelectMask&VLEF_QPFlash)!=0; };
    //int QVFlashReqd() { return (dwSelectMask&VLEF_QVFlash)!=0; };
    //int QMVapFlashReqd() { return (dwSelectMask&VLEF_QMVapFlash)!=0; };

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

  protected:
    flag            m_fFixed;
    flag            m_fEnabled;
    CEvapBlock    * m_pEvapB;
    pTaggedObject   m_pNd;
    //dword           dwSelectMask;

    //double          m_SensHtIn;

  public:

  };

#undef DllImportExport
