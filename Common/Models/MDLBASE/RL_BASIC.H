//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __RL_BASIC_H
#define  __RL_BASIC_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
//#ifndef __FLWNODE_H
//  #include <flwnode.h>
//#endif
#ifndef __CTRL_IO_H
  #include "ctrl_io.h"
#endif

#ifdef __RL_BASIC_CPP
  #define DllImportExport DllExport
#elif !defined(MdlBase)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ===========================================================================
//
//
//
// ===========================================================================

DEFINE_TAGOBJ(RL_Basic)
class RL_Basic : public CIO_Logic
  {
  public:
    double         dOut, dMin, dMax;

    RL_Basic(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~RL_Basic();

    //virtual void   Initialise(pTaggedObject pAttach);
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   CopyFrom(CIO_Logic * pOther);
    virtual void   EvalDiscrete(FlwNode* pFNode) {};
    virtual void   EvalCtrlActions(FlwNode* pNd) {};
    virtual void   EvalCtrlStrategy(FlwNode* pNd) {};
    virtual void   SetOutput(double);
    virtual double Output();
    virtual void   SetRange(double Min, double Max);
    virtual double RangeMin();
    virtual double RangeMax();
  };

// ---------------------------------------------------------------------------

DEFINE_TAGOBJ(RL_Stroke)
class RL_Stroke: public RL_Basic
  {
  public:
    double         dOutRqd, dStrkUp, dStrkDn;

    RL_Stroke(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~RL_Stroke();

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual void   EvalDiscrete(FlwNode* pFNode) {};
    virtual void   EvalCtrlActions(FlwNode* pNd);
    virtual void   EvalCtrlStrategy(FlwNode* pNd) {};
    virtual void   SetOutput(double);
    virtual double Output();
    virtual void   SetRange(double Min, double Max);
    virtual double RangeMin();
    virtual double RangeMax();
  };

// ---------------------------------------------------------------------------
#define IMPLEMENT_REGLTNLOGIC(Md, Name, Version, Cat, ShortDesc, Desc) \
  IMPLEMENT_TAGOBJ(Md, "Regltn", Name, Version, "", "", Cat, ShortDesc, Desc);

// ---------------------------------------------------------------------------

_FWDDEF(CIO_RegltnBlk)
class DllImportExport CIO_RegulationBlk : public CIO_Blk
  {
  public:
    CIO_RegulationBlk(pchar Name, pTaggedObject pAttach, CPwrUser * pPwr):
      CIO_Blk("Regltn", Name, pAttach, pPwr){ };
    virtual ~CIO_RegulationBlk() {};

    void SetOutput(double O) { ((pRL_Basic)pLogic)->SetOutput(O) ;};
    double Output() { return ((pRL_Basic)pLogic)->Output() ;};
    void SetRange(double Min, double Max) { ((pRL_Basic)pLogic)->SetRange(Min, Max); };
    double RangeMin() { return ((pRL_Basic)pLogic)->RangeMin() ;};
    double RangeMax() { return ((pRL_Basic)pLogic)->RangeMax() ;};
  };


// ===========================================================================

#undef DllImportExport

#endif
