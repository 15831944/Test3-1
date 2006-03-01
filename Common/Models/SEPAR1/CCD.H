//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __CCD_H
#define  __CCD_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif  
#ifndef __M_SURGE_H
  #include "m_surge.h"
#endif  
#ifndef __2D_FN_H
  #include "2d_fn.h"
#endif  

#ifdef __CCD_CPP
  #define DllImportExport DllExport
#elif !defined(Separ1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//=========================================================================== 

DEFINE_TAGOBJ(Ccd);

class Ccd : public MN_Surge
  {
  public:
		long					 Stage;								 // Number of stages in CCD circuit 
    double         UFSolids, OFSolids;   // Achieved Underflow & Overflow Densities
    double         WR;									 // wash ratio
    double         ActUFSolidsConc25, ActOFSolidsConc25;

    Ccd(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~Ccd (){};
    virtual void   BuildDataDefn(DataDefnBlk & DDB);

    virtual void   ConfigureJoins();
    virtual void   EvalJoinPressures(long JoinMask);
    virtual void   EvalProducts(long JoinMask);
  private:
            void   ConcBal(CSysVector & Fd, CDVector& c, CDVector& mass, double UVol, double max, int water);
  };

//=========================================================================== 

#undef DllImportExport

#endif
