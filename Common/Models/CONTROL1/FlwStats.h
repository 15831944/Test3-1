//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __FLWSTATS_H
#define  __FLWSTATS_H

#ifndef __SC_DEFS_H
#include "sc_defs.h"
#endif  
#ifndef __FLWNODE_H
#include "flwnode.h"
#endif
#ifndef __MODELS_H
#include "xrefs.h"
#endif  
#ifndef __MODELS_H
#include "models.h"
#endif  

#ifdef __FLWSTATS_CPP
  #define DllImportExport 
#elif !defined(Control1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

#define SKIPIT 0
#if SKIPIT 
#pragma message ("---------------------------------------SKIPPED")
#else

//============================================================================

_FWDDEF(FlwStatInfo)
class FlwStatInfo : public CXRefStatus
  {
  public:
    CTgFnIoVar m_MeasVar;
    CTgFnIoVar m_QvVar;
    CTgFnIoVar m_QmSVar;
    CTgFnIoVar m_QmLVar;
    flag       bValid:1;
    long       iPriority;
    byte       iType;

    Strng      sTagSuffix;
    Strng      sDesc;
    double     dMeasQm;
    double     dTtlTime;
    double     dZeroTime;
    double     dTotalMass;
    double     dMaxQm;
    double     dMinQm;

    double     dMeasQv;
    double     dTotalVol;
    double     dMeasQmS;
    double     dTotalMassS;
    double     dMeasQmL;
    double     dTotalMassL;

    FlwStatInfo();
    virtual ~FlwStatInfo();
    void Init(int iNo);
    void ResetStats();
    flag DoSetVars(char* Tag);
    void ExecIns(double dT);

    //CXRefStatus Override
    bool IsXRefActive() const    { return bValid;  };

  };

//--------------------------------------------------------------------------

DEFINE_TAGOBJ(CFlowStats);
class CFlowStats : public FlwNode
  {
  friend class CTgFnIoVar;
  public:
    FlwStatInfo**  DataBlk;              //array of flow statistic blocks
    long           iCount;               //number
    flag           bOn;                  //
    flag           bResetOnStart;
    flag           bResetOnInit;
    flag           bResetOnEmpty;
    flag           bResetOnPreSet;
    Strng          m_StateLine[3];       //status messages
    flag           bDoneExtRefs:1,       //
                   bAboutToStart:1;      //flag set True for first iteration when run is pressed

    CFlowStats(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CFlowStats();
    virtual void   ResetData(flag Complete);
  private:
    void           SetCount(long NewSize);
    flag           SortRqd();
    void           Sort();
    void           ResetAllStats();
  public:
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual flag   PreStartCheck();
    virtual void   EvalStatistics(eScdCtrlTasks Tasks=CO_All);
    virtual int    ChangeTag(pchar pOldTag, pchar pNewTag);
    virtual int    DeleteTag(pchar pDelTag);

    // CNodeXRefMngr Overides
    virtual bool   IsXRefListActive();
    virtual int    UpdateXRefLists(CXRefBuildResults & Results);
    virtual void   UnlinkAllXRefs();

    virtual void   SetState(eScdMdlStateActs RqdState);
    virtual void   EvalDiscrete();
    virtual dword  ModelStatus();

    DEFINE_CI(CFlowStats, FlwNode, 4);

  };
  
//===========================================================================

#endif

#undef DllImportExport

#endif
