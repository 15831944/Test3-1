//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __CCWASH_H
#define  __CCWASH_H

#ifndef __SC_DEFS_H
  #include <sc_defs.h>
#endif
#ifndef __M_SURGE_H
  #include <m_surge.h>
#endif
#ifndef __GSMODEL_H
  #include <gsmodel.h>
#endif

#ifdef __CCWASH_CPP
  #define DllImportExport DllExport
#else
  #define DllImportExport
#endif

// ==========================================================================

DEFINE_TAGOBJ(CCWasher);
class DllImportExport CCWasher : public MN_Surge
  {
  public:
		double				 MixEff;            // The degree of imperfect mixing in the washer
    double         ScandrettEff     ; // Scrandrett wash efficiency as a fraction
    double         RqdUFSolidsConc25; // Underflow solids at 25 dC in g/L
    double         RqdUFSolids;       // Underflow solids as % solids w/w
    double         ByPassGuess;
    double         WashByPassGuess;
    double         Reqd_UFlowSolidsCorr ;
		double				 UFSolids;//, OFSolids;
    double				 UFCaustic, OFCaustic;
    double         ActUFSolidsConc25;//, ActOFSolidsConc25;
    double				 ActScandrettEff;
    byte           SA;
    byte           iScanEffSpecie;
    byte           iEffMethod;

    SpConduit      Qm, Qw, Qt;

    flag           fOn;
    
    CCWasher(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CCWasher (){};
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual void   EvalSteadyState();
    virtual void   EvalJoinPressures(long JoinMask);
    virtual void   EvalJoinFlows(int JoinNo);
    virtual void   EvalProducts(long JoinMask);
    virtual void   SetState(eScdMdlStateActs RqdState);
    // ConditionBlk Override
    DEFINE_CI(CCWasher, MN_Surge, 8);
   private:
  };

//=========================================================================== 

#undef DllImportExport

#endif
