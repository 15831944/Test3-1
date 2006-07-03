//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __MILL1_H
#define  __MILL1_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif  
#ifndef __CRUSH1_H
  #include "crush1.h"
#endif  
//#define BASIC1
//#define COMMN1
#define SIZEDST1
#include "models.h"

#ifndef __M_SURGE_H
  #include "m_surge.h"
#endif  

#ifdef __MILL1_CPP
  #define DllImportExport DllExport
#elif !defined(Commn1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ===========================================================================

DEFINE_TAGOBJ(Mill1);
class Mill1 : public MN_Surge
  {
  public:
    SpConduit      Disch;
    CIO_MtrSpdBlk  MSB;
    CPwrUser       m_Pwr;
    CrushBlk       CB;

    double         DischOnSpeed;
    double         Power;
    double         BWI;

		double				 SS_Lvl;
    double         Heat;
    double         Eff;
    double         Motor;

    Mill1 (pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~Mill1();

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    virtual void   StartSolution();
    virtual void   EvalSteadyState();
    virtual void   EvalJoinPressures(long JoinMask);
    virtual void   EvalJoinFlows(int JoinNo);
    virtual void   EvalProducts(CNodeEvalIndex & NEI);

    virtual void   Ctrl_ConnIDStr(int i, Strng & ID, Strng & Tg);
    virtual void   EvalCtrlActions(eScdCtrlTasks Tasks=CO_All);
    virtual void   EvalPowerRequired();
    virtual void   ClosureInfo();
    
  private:
    //DEFINE_CI(Mill1, MN_Surge, 3);
  };

// ===========================================================================

#undef DllImportExport

#endif
