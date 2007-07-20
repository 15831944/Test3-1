//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __HEATX_H
#define  __HEATX_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __2D_FN_H
  #include "2d_fn.h"
#endif
#ifndef __M_HXBASE_H
  #include "m_hxbase.h"
#endif
//#ifndef __M_BASE_H
//  #include "m_base.h"
//#endif
#ifndef __MODELS_H
  #include "models.h"
#endif

#ifdef __HEATX_CPP
  #define DllImportExport DllExport
#elif !defined(MdlBase)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif


// ==========================================================================
//
//  Heat Exchange Model
//
// ==========================================================================

DEFINE_TAGOBJ(HeatXchg);
class DllImportExport HeatXchg : public MdlNode
{
  public:

    CHXBase        HX;
    CFlwEqnPtr     FBP, FBS;
    CHXJoinInfo    m_HXJ;


    HeatXchg(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~HeatXchg(){};

    void           BuildDataDefn(DataDefnBlk & DDB);
    flag           DataXchg(DataChangeBlk & DCB);
    void           StartSolution();
    void           PostConnect(int IONo);

    void           ConfigureJoins();
    flag           EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo);
    void           EvalProducts(CNodeEvalIndex & NEI);

  private:
};

// ===========================================================================

#undef DllImportExport


#endif


