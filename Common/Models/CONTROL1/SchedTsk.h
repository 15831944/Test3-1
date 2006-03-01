//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __SCHEDTSK_H
#define  __SCHEDTSK_H

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

#ifdef __SCHEDTSK_CPP
  #define DllImportExport 
#elif !defined(Control1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//============================================================================

class ScheduleTask
  {
  public:
    flag    bScrOn;
    Strng   sScrPath;
    Strng   sScrFile;
    Strng   sScrStatus;
    byte    iScrSchedType;
    long    iScrExecCnt;
    long    iScrIterCnt;
    double  dScrExecTime;
    double  dScrIterTime;
    HANDLE  hScrProcess;             //process handle for the editor
    DWORD   dwScrProcessId;          //process ID for the editor

    flag    bMcrOn;
    Strng   sMcrPath;
    Strng   sMcrFile;
    Strng   sMcrStatus;
    byte    iMcrSchedType;
    long    iMcrExecCnt;
    long    iMcrIterCnt;
    double  dMcrExecTime;
    double  dMcrIterTime;
    byte    iMcrType;
    Strng   sMcrName;

    flag    bRepOn;
    Strng   sRepPath;
    Strng   sRepFile;
    Strng   sRepStatus;
    byte    iRepSchedType;
    long    iRepExecCnt;
    long    iRepIterCnt;
    double  dRepExecTime;
    double  dRepIterTime;
    Strng   sRepTblName;
    Strng   sRepEndTime;
    long    iRepTagCnt;
    CSVector RepTags;
    long    iRepPtsCnt;
    double  dRepDuration;

    ScheduleTask();
    void   BuildDataDefn(DataDefnBlk & DDB, TaggedObject* pObj);
    flag   DataXchg(DataChangeBlk & DCB, TaggedObject* pObj);
    void   EvalDiscrete();
    int    FilesUsed(CFilesUsedArray & Files);
    int    ChangeTag(pchar pOldTag, pchar pNewTag);
    int    DeleteTag(pchar pDelTag, BOOL & FoundOne);
  private:
    void ExecScr();
    void ExecMcr();
    void ExecRep();
  };

//--------------------------------------------------------------------------

DEFINE_TAGOBJ(CScheduleTask);
class CScheduleTask : public FlwNode
  {
  public:
    flag           bAboutToStart:1;      //flag set True for first iteration when run is pressed
    ScheduleTask   ST;

    CScheduleTask(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CScheduleTask();
  public:
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   PreStartCheck();
    virtual void   EvalDiscrete();
    virtual int    FilesUsed(CFilesUsedArray & Files);
    virtual int    ChangeTag(pchar pOldTag, pchar pNewTag);
    virtual int    DeleteTag(pchar pDelTag);
    virtual dword  ModelStatus();
    DEFINE_CI(CScheduleTask, FlwNode, 4);

  };
  
//===========================================================================

#undef DllImportExport

#endif

