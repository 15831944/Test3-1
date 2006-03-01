//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __CTRL_IO_H
#define  __CTRL_IO_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __FLWNODE_H
  #include "flwnode.h"
#endif
#ifndef __ELECTRICS_H
  #include "electrics.h"
#endif

#ifdef __CTRL_IO_CPP
  #define DllImportExport DllExport
#elif !defined(MdlBase)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ===========================================================================

XID xidLogicNm = MdlBsXID(3000);

// ===========================================================================

_FWDDEF(CIO_Logic)
/*#c:Class Used to implement Logic for a generic Digital IO Point in a model */
class DllImportExport CIO_Logic : public TaggedObject
  {
  friend class CIO_Blk;
  protected:
    CPwrUser       *m_pPwr;
  public:
    CIO_Logic(pTagObjClass pClass_, LPTSTR pTag, TaggedObject * pAttach, TagObjAttachment eAttach):
     TaggedObject(pClass_, pTag, pAttach, eAttach){ m_pPwr=NULL;};
    virtual ~CIO_Logic() {};

    virtual void   BuildDataDefn(DataDefnBlk & DDB)=0;
    virtual flag   DataXchg(DataChangeBlk & DCB) {return False;};
    virtual flag   CopyFrom(CIO_Logic * pOther)=0;
    virtual void   EvalDiscrete(FlwNode * pFNode)=0;
    virtual void   EvalCtrlActions(FlwNode * pFNode)=0;
    virtual void   EvalCtrlStrategy(FlwNode * pFNode)=0;
    virtual LPTSTR LogicName();// { return ClassId(); };
    virtual bool   PwrOn() { return ProbalMode() || (!m_pPwr || m_pPwr->On()); };

  protected:  // Helper Funstions
  };

// ---------------------------------------------------------------------------

_FWDDEF(CIO_Blk)
/*#c:Class Used to implement a generic Digital IO Point in a model */
class DllImportExport CIO_Blk 
  {
  protected:
    CIO_Logic     *pLogic;
    LPTSTR         pGroup;
    CPwrUser      *m_pPwr;

  public:
    CIO_Blk(LPTSTR Group, LPTSTR Name, TaggedObject * pAttach, CPwrUser * pPwr);
    ~CIO_Blk();       

  protected:
    CIO_Logic*     CreateLogic(LPTSTR Name_, TaggedObject * pAttach);
    void           ChangeLogic(LPTSTR Name_);
    
  public:
    void           BuildDataDefn(DataDefnBlk & DDB, TaggedObject * pOwnObj, LPTSTR pName, dword dwUserInfo, LPTSTR pComment=NULL, int HandleOffset=0);
    flag           DataXchg(DataChangeBlk & DCB, int HandleOffset=0);
    void           EvalDiscrete(FlwNode * pFNode) 
                     {if (pLogic) pLogic->EvalDiscrete(pFNode); };
    void           EvalCtrlActions(FlwNode * pFNode) 
                     {if (pLogic) pLogic->EvalCtrlActions(pFNode); };
    void           EvalCtrlStrategy(FlwNode * pFNode) 
                     {if (pLogic) pLogic->EvalCtrlStrategy(pFNode); };

    LPTSTR         LogicName() 
                     { return pLogic ? pLogic->LogicName() : NULL; };

  private:
  };

// ===========================================================================
// ===========================================================================

_FWDDEF(CIO_DLogic)
/*#c:Class Used to implement Logic for a generic Digital IO Point in a model */

class DllImportExport CIO_DLogic 
  {
  public:
    flag  *        pbSet;

    CIO_DLogic();

    void           AttachValue(pflag pSet);
    virtual void   Set(flag Set);

    virtual void   BuildDataDefn(TaggedObject * pObj, LPTSTR pTag, DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual void   EvalDiscrete(FlwNode * pNd);
    virtual void   EvalCtrlActions(FlwNode * pNd);
    virtual void   EvalCtrlStrategy(FlwNode * pNd);
  };

// ===========================================================================

_FWDDEF(CIO_Digital)
/*#c:Class Used to implement a generic Digital IO Point in a model */
class DllImportExport CIO_Digital 
  {
  public:
    flag           bSet;
    CIO_DLogic *   pLogic;
    
    CIO_Digital();       

    void           AttachLogic(CIO_DLogic * Logic);
    CIO_DLogic *   DetachLogic();
    flag           Set(flag SetIt);
    flag           On() { return bSet; };
    flag           Off() { return !bSet; };
    flag operator()() { return bSet; };

    void           BuildDataDefn(TaggedObject * pObj, LPTSTR pTag, DataDefnBlk & DDB);
    flag           DataXchg(DataChangeBlk & DCB);
    void           EvalDiscrete(FlwNode * pNd);
    void           EvalCtrlActions(FlwNode * pNd);
    void           EvalCtrlStrategy(FlwNode * pNd);

  private:
  };

// ===========================================================================

_FWDDEF(CIO_ALogic)
/*#c:Class Used to implement Logic for a generic Analog IO Point in a model */
class DllImportExport CIO_ALogic 
  {
  public:
    pdouble        pdVal;

    CIO_ALogic();

    void           AttachValue(pdouble pVal);
    virtual void   Set(double Val);

    virtual void   BuildDataDefn(TaggedObject * pObj, LPTSTR pTag, DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual void   EvalDiscrete(FlwNode * pNd);
    virtual void   EvalCtrlActions(FlwNode * pNd);
    virtual void   EvalCtrlStrategy(FlwNode * pNd);
  };

// ===========================================================================

_FWDDEF(CIO_Analog)
/*#c:Class Used to implement a generic Analog IO Point in a model */
class DllImportExport CIO_Analog 
  {
  public:
    double         dVal;
    CIO_ALogic*    pLogic;
    
    CIO_Analog();       

    void           AttachLogic(CIO_ALogic* Logic);
    CIO_ALogic*    DetachLogic();
    double         Set(double SetIt);
    double         Value() { return dVal; };
    double operator()() { return dVal; };

    void           BuildDataDefn(TaggedObject * pObj, LPTSTR pTag, DataDefnBlk & DDB);
    flag           DataXchg(DataChangeBlk & DCB);
    void           EvalDiscrete(FlwNode * pNd);
    void           EvalCtrlActions(FlwNode * pNd);
    void           EvalCtrlStrategy(FlwNode * pNd);

  private:
  };

// ===========================================================================
// ===========================================================================

#undef DllImportExport 

#endif
