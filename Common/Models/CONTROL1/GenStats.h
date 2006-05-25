//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __GENSTATS_H
#define  __GENSTATS_H

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

#ifdef __GENSTATS_CPP
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

_FWDDEF(GenStatInfo)
class GenStatInfo : public CXRefStatus
  {
  public:
    CTgFnIoVar m_MeasVar;
    flag       bValid:1;
    long       iPriority;

    Strng      sTagSuffix;
    double     dMeas;
    double     dTtlTime;
    double     dZeroTime;
    double     dTotal;
    double     dMax;
    double     dMin;

    GenStatInfo();
    virtual ~GenStatInfo();
    void Init(int iNo);
    void ExecIns(double dT);
    void ResetStats();

    //CXRefStatus Override
    bool IsXRefActive() const    { return bValid;  };
  };

//--------------------------------------------------------------------------

DEFINE_TAGOBJ(CGenStats);
class CGenStats : public FlwNode
  {
  friend class CTgFnIoVar;
  public:
    static flag    bWithCnvComment;
    GenStatInfo**  DataBlk;              //array of statistics blocks
    long           iCount;               //number
    flag           bOn;                  //
    flag           bResetOnStart;
    flag           bResetOnInit;
    flag           bResetOnEmpty;
    flag           bResetOnPreSet;
    Strng          m_StateLine[3];       //status messages
    flag           bDoneExtRefs:1,       //
                   bAboutToStart:1;      //flag set True for first iteration when run is pressed

    CGenStats(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CGenStats();
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
    virtual void   EvalProducts(long JoinMask);
    virtual dword  ModelStatus();

    DEFINE_CI(CGenStats, FlwNode, 4);

  };
  
//===========================================================================

#endif

#undef DllImportExport

#endif
