//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __NOISECON_H
#define  __NOISECON_H

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

#ifdef __NOISECON_CPP
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

_FWDDEF(NoiseConInfo)
class NoiseConInfo : public CXRefStatus 
  {
  public:
    CTgFnIoVar m_OutputVar;
    flag       bOn:1,
               bValid:1;
    long       iPriority;
    long       iExecCnt;
    long       iIterCnt;

    Strng      sTagSuffix;
    CNoise     Noise;

    NoiseConInfo();
    virtual ~NoiseConInfo();
    void Init(int iNo);
    double ExecIns();
    void ResetStats();

    //CXRefStatus Override
    bool IsXRefActive() const    { return bValid && bOn;  };
  };

//--------------------------------------------------------------------------

DEFINE_TAGOBJ(CNoiseCon);
class CNoiseCon : public FlwNode
  {
  public:
    static flag    bWithCnvComment;
    NoiseConInfo** DataBlk;              //array of noise blocks
    long           iCount;               //number
    flag           bOn;                  //
    Strng          m_StateLine[3];       //status messages
    flag           bDoneExtRefs:1,       //
                   bAboutToStart:1;      //flag set True for first iteration when run is pressed

    CNoiseCon(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CNoiseCon();
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
    virtual void   EvalCtrlStrategy(eScdCtrlTasks Tasks=CO_All);
    virtual int    ChangeTag(pchar pOldTag, pchar pNewTag);
    virtual int    DeleteTag(pchar pDelTag);

    // CNodeXRefMngr Overides
    virtual bool   IsXRefListActive();
    virtual int    UpdateXRefLists(CXRefBuildResults & Results);
    virtual void   UnlinkAllXRefs();

    virtual void   SetState(eScdMdlStateActs RqdState);
    virtual void   EvalProducts(long JoinMask);
    virtual dword  ModelStatus();

    DEFINE_CI(CNoiseCon, FlwNode, 4);

  };
  
//===========================================================================

#endif

#undef DllImportExport

#endif
