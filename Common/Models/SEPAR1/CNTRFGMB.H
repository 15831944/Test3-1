//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __CNTRFGMB_H
#define  __CNTRFGMB_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif  
#ifndef __M_SURGE_H
  #include "m_surge.h"
#endif  

#ifdef __CNTRFGMB_CPP
  #define DllImportExport DllExport
#elif !defined(Separ1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//=========================================================================== 

DEFINE_TAGOBJ(CentrifugeMB);

class CentrifugeMB : public MN_Surge
  {
  public:
    double         RqdSolidsToFiltrate;//SolidLoss
    double         RqdCakeMoist;
    double         FlowRatio, WashEff;
    double         ActCakeLiq;
    double         ActFiltSolConc, ActFiltSolConcT;
    double         ActCakeSolids, ActFiltSolids;
    //byte           iScanEffSpecie; //eg caustic
    //double         ActScandrettEff;
    byte           iWashEffSpecie; //eg caustic
    double         dSpWashEff,dSpWashEff2;
    flag           Disch;
    SpConduit      QFd;

    CentrifugeMB(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CentrifugeMB (){};
    virtual void   BuildDataDefn(DataDefnBlk & DDB);

    virtual void   EvalJoinPressures(long JoinMask);
    virtual void   EvalProducts(CNodeEvalIndex & NEI);
  private:
  };

//=========================================================================== 

#undef DllImportExport

#endif
