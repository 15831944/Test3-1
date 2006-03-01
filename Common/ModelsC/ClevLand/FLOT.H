//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __FLOT_H
#define  __FLOT_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#define BASIC1
#define SIZEDST1
#include "models.h"
#ifndef __M_SURGE_H
  #include "m_surge.h"
#endif
#ifndef __2D_FN_H
  #include "2d_fn.h"
#endif  

#ifdef __FLOT_CPP
  #define DllImportExport DllExport
#elif !defined(CLEVLAND)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//=========================================================================== 

DEFINE_TAGOBJ(Flot);
/*#C:
Flotation Cell - Multiple Inputs and Outputs
*/
class DllImportExport Flot : public MN_Surge
  {
  protected:
    SzPartCrv1     SizeRec;//1, SizeRec2, SizeRec3;

  public:
    double         SS_Lvl, Eff, Rec, Grd, Grade;
    byte           nAbove, nBelow;
	  long           iFlot, iSec;
		byte           iColl, RM, Fine;
		byte					 SecFlot;
    SpConduit      QFd;
    double         QmSIn, QmLIn, Factor;
    double         TlSol, TlLiq, FlSol, FlLiq;
		double				 TotRecover, Grade2;

    C2DFn          FlotFn;

    Flot(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~Flot (){};
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


