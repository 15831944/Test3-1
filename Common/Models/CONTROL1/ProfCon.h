//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __PROFCON_H
#define  __PROFCON_H

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

#ifdef __PROFCON_CPP
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

_FWDDEF(ProfColInfo)
class ProfColInfo
  {
  public:
    CTgFnIoVar        m_OutputVar;
    double            m_dOutput;
    double            m_dGain;
    double            m_dOffset;

    ProfColInfo();
  };

_FWDDEF(ProfConInfo)
class ProfConInfo : public CXRefStatus
  {
  public:
    flag       bOn:1,
               bValid:1,
               bReloadRqd:1,
               bLinear:1,       //must linear interpulation be applied between points
               bWrapArround:1,  //must the profile wrap arround after full time
               bUseHeadingRow:1,//must the tags be obtained from the first row
               bUseGainOffset:1;//can a gain and offset be used with the data
    long       iPriority;

    Strng      sTagSuffix;
    Strng      sStatus;
    Strng      sPath;
    Strng      sPathForView;
    Strng      sFile;
    HANDLE     hProcess;             //process handle for the editor
    DWORD      dwProcessId;          //process ID for the editor
    byte       iColCnt;
    ProfColInfo**   ColData;
    CSimpleProfiler Prof;


    ProfConInfo();
    virtual ~ProfConInfo();
    void Init(int iNo);
    byte SetColCnt(byte NewSize, byte MaxAllowed);
    flag DoLoad();
    void ExecIns(double ICTime);

    //CXRefStatus Override
    bool IsXRefActive() const    { return bValid;  };

  };

//--------------------------------------------------------------------------

DEFINE_TAGOBJ(CProfileCon);
class CProfileCon : public FlwNode
  {
  public:
    static flag    bWithCnvComment;
    ProfConInfo**  DataBlk;               //array of profiler blocks
    long           iCount;               //number
    flag           bOn;                  //
    Strng          m_StateLine[3];       //status messages
    flag           bDoneExtRefs:1,       //
                   bAboutToStart:1;      //flag set True for first iteration when run is pressed

    CProfileCon(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CProfileCon();
    virtual void   ResetData(flag Complete);
  private:
    void           SetCount(long NewSize);
    flag           SortRqd();
    void           Sort();
    void           RestartAll();

  public:
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual flag   PreStartCheck();
    virtual void   EvalCtrlInitialise(eScdCtrlTasks Tasks=CO_All);
    virtual void   EvalCtrlStrategy(eScdCtrlTasks Tasks=CO_All);
    virtual int    ChangeTag(pchar pOldTag, pchar pNewTag);
    virtual int    DeleteTag(pchar pDelTag);
    virtual int    FilesUsed(CFilesUsedArray & Files);

    // CNodeXRefMngr Overides
    virtual bool   TestXRefListActive();
    virtual int    UpdateXRefLists(CXRefBuildResults & Results);
    virtual void   UnlinkAllXRefs();

    virtual void   SetState(eScdMdlStateActs RqdState);
    virtual void   EvalProducts(CNodeEvalIndex & NEI);
    virtual dword  ModelStatus();

    DEFINE_CI(CProfileCon, FlwNode, 4);

  };
  
//===========================================================================

#endif

#undef DllImportExport

#endif

