//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __TANK_H
#define  __TANK_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __M_SURGE_H
  #include "m_surge.h"
#endif

#ifdef __SECREFORM_CPP
  #define DllImportExport DllExport
#elif !defined(Petro1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//=========================================================================== 

const byte MaxSecRefTPoints=5;

DEFINE_TAGOBJ(SecReformer);
/*Basic Mixed Tank - Multiple Inputs and Outputs*/
class DllImportExport SecReformer: public MN_Surge
  {
  public:
    //double         FlashSplit;
    //double         SS_Lvl;
    byte           nAbove, nBelow;
    struct   
      {
      double       m_dTemp;
      double       m_dMult;
      }            m_TPoints[MaxSecRefTPoints];

    SecReformer(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~SecReformer(){};
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    //virtual void   StartSolution();
    //virtual void   EvalSteadyState();
    //virtual void   EvalJoinPressures(long JoinMask);
    //virtual void   EvalJoinFlows(int JoinNo);
    virtual void   EvalProducts(CNodeEvalIndex & NEI);
    virtual void   EvalDerivs(CNodeEvalIndex & NEI);
    virtual void   EvalDiscrete();

   private:
  };

//=========================================================================== 

#undef DllImportExport

#endif


