// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __BELTFILT_H
#define  __BELTFILT_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif  

//#define SIZEDST1
#include "models.h"

#ifndef __2D_FN_H
  #include "2d_fn.h"
#endif  

#ifdef __BELTFILT_CPP
  #define DllImportExport DllExport
#elif !defined(Separ1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//=========================================================================== 

DEFINE_TAGOBJ(BeltFilt);

class BeltFilt : public MdlNode
  {
  public:
    long           Stage;    // Number of wash stages on the belt filter
    double         WR;       // Ratio of wash solution to liquor holdup in cake
    double         RqdCakeMoist; // Percentage of moisture in solid cake discharging from filter
    double         ActCakeSolids, ActFiltSolids;
    double         ActCakeSolConc, ActFiltSolConc;
    //double         QmSIn, QmLIn, QmWIn;
    flag           bTrackStatus;
    
    SpConduit      QFd;
    C2DFn          WashLossCurve;   // Washing loss curve for simple filtration washing 

    BeltFilt(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~BeltFilt();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);

    virtual void   ConfigureJoins();
    virtual void   EvalJoinPressures(long JoinMask);

    virtual void   EvalProducts(CNodeEvalIndex & NEI);
    virtual dword  ModelStatus();
    DEFINE_CI(BeltFilt, MdlNode, 4);
  
  private:
            void   Conc(CDVector& d, long temp, double CLiq, double FLiq, double QmWashLiq);
            void   filt(CDMatrix& w, CDVector& d, int j, long max);
            void   washcon(CDMatrix& w, CDMatrix& c, double Cin, int j, long temp);
  };

//=========================================================================== 

#undef DllImportExport

#endif
