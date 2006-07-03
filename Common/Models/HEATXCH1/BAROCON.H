//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992 - 1998

#ifndef  __BAROCON_H
#define  __BAROCON_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#define BASIC1
#include "models.h"

#ifndef __FLSHTRN_H
  #include "flshtrn.h"
#endif


#ifdef __BAROCON_CPP
  #define DllImportExport DllExport
#else
  #define DllImportExport
#endif

// ==========================================================================

DEFINE_TAGOBJ(BarometricCondenser);
class DllImportExport BarometricCondenser : public MN_Surge
  {
  public:
    double         Rqd_Flash_Press;

    double         ApproachTemp;
    double         dFlashP, dFlashT;

    CFT_Condenser  m_FTC;
      
    BarometricCondenser(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~BarometricCondenser (){};
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    
    virtual flag   InitialiseSolution();
    virtual void   EvalSteadyState();
    virtual void   SetState(eScdMdlStateActs RqdState);
    virtual void   EvalJoinPressures(long JoinMask);
    virtual void   EvalJoinFlows(int JoinNo);
    virtual void   EvalProducts(CNodeEvalIndex & NEI);

    virtual void   ClosureInfo();

#if DOPBSMOOTHING  
    virtual void   EvalMsAGDatas();
#endif

    flag           MacroMdlValidNd(int iIONo) { return True; };
    void           MacroMdlEvaluate(eScdMacroMdlEvals Eval);
    CMacroMdlBase*   MacroMdlActivate() { return &m_FTC; };
    void           MacroMdlDeactivate() {};

   private:
  };

//=========================================================================== 

#undef DllImportExport

#endif
