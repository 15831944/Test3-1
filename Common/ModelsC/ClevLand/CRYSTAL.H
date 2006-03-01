//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __Crystal_H
#define  __Crystal_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#define BASIC1
#define SIZEDST1
#include "models.h"
#ifndef __2D_FN_H
  #include "2d_fn.h"
#endif
#ifndef __M_SURGE_H
  #include "m_surge.h"
#endif  

#ifdef __Crystal_CPP
  #define DllImportExport DllExport
#elif !defined(CLEVLAND)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif
//=========================================================================== 

DEFINE_TAGOBJ(Crystal);

class DllImportExport Crystal : public MN_Surge
  {
  protected:
    SzPartCrv1     DischCrystal;

  public:
    double         SS_Lvl, PRqd;
    byte           CR, nAbove, nBelow;
    C2DFn          Soluble;
    SpConduit      Disch;


    Crystal(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~Crystal (){};
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual void   StartSolution();
    virtual void   EvalSteadyState();
    virtual void   EvalJoinPressures();
    virtual void   EvalJoinFlows(int JoinNo);
    virtual void   EvalProducts();

   private:
  };

//=========================================================================== 

#undef DllImportExport

#endif


