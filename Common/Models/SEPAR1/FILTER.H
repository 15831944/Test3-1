// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __FILTER_H
#define  __FILTER_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif  
//#define SIZEDST1
#include "models.h"
#ifndef __2D_FN_H
  #include "2d_fn.h"
#endif  

#ifdef __FILTER_CPP
  #define DllImportExport DllExport
#elif !defined(Separ1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//=========================================================================== 

DEFINE_TAGOBJ(Filter);

class Filter : public MdlNode
  {
  public:
    long           Stage;    // Number of wash stages on the belt filter
    double         N;        // Ratio of wash solution to liquor holdup in cake
    double         Moist;    // Percentage of moisture in solid cake discharging from filter
    //double         QmSIn, QmLIn, QmWIn;
    
    //FlangeBuffer   OnFilter;
    //FlangeBuffer   ThruFilter;
    SpConduit      QFd, QSo, QFt, QWS;
    C2DFn          WashLossCurve;   // Washing loss curve for simple filtration washing 

    Filter(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~Filter();
    //virtual void   Initialise();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);

    virtual void   EvalJoinPressures(long JoinMask);
    virtual void   EvalProducts(long JoinMask);
    virtual void   EvalDerivs(long JoinMask);
  
  private:
            void   Conc(CDVector& d, long temp, double CLiq, double FLiq);
            void   filt(CDMatrix& w, CDVector& d, int j, long max);
            void   washcon(CDMatrix& w, CDMatrix& c, double Cin, int j, long temp);
  };

//=========================================================================== 
#undef DllImportExport

#endif
