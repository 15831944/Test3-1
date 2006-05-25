//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __QUEUECON_H
#define  __QUEUECON_H

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

#ifdef __QUEUECON_CPP
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

_FWDDEF(QueueColInfo)
class QueueColInfo
  {
  public:
    Strng      m_sColTagName;
    //int        m_iLen;
    CTgFnIoVar m_OutputVar;
    double     m_dOutput;

    QueueColInfo();
  };

_FWDDEF(QueueConInfo)
class QueueConInfo : public CXRefStatus
  {
  public:
    flag       bOn:1,
               bValid:1,
               bReloadRqd:1,
               bLoaded:1,
               bUseHeadingRow:1;//must the tags be obtained from the first row
    flag       bWrapArround;  //must the queue wrap arround after all values used
    short      iStartIndex; //index for reset stats/start
    short      iCurIndex;

    Strng      sTagSuffix;
    Strng      sStatus;
    Strng      sPath;
    Strng      sPathForView;
    Strng      sFile;
    HANDLE     hProcess;             //process handle for the editor
    DWORD      dwProcessId;          //process ID for the editor
    short      iColCnt;
    QueueColInfo** ColData;
    CDMatrix   Data;      //data containing the values


    QueueConInfo();
    virtual ~QueueConInfo();
    short SetColCnt(short NewSize);
    int DoLoad();
    void ExecIns(double ICTime);
    void SetIndex(short RqdIndex);
    void AdvanceIndex();

    //CXRefStatus Override
    bool IsXRefActive() const    { return bValid;  };
  };

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

DEFINE_TAGOBJ(CQueueCon);
class CQueueCon : public FlwNode
  {
  public:
    static flag    bWithCnvComment;
    QueueConInfo   QCI;              //
    flag           bDoneExtRefs:1,       //
                   bAboutToStart:1;      //flag set True for first iteration when run is pressed

    CQueueCon(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CQueueCon();
    virtual void   ResetData(flag Complete);
  private:
    void           DoLoad();

  public:
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual flag   PreStartCheck();
    virtual void   EvalCtrlStrategy(eScdCtrlTasks Tasks=CO_All);
    virtual int    ChangeTag(pchar pOldTag, pchar pNewTag);
    virtual int    DeleteTag(pchar pDelTag);
    virtual int    FilesUsed(CFilesUsedArray & Files);

    // CNodeXRefMngr Overides
    virtual bool   IsXRefListActive();
    virtual int    UpdateXRefLists(CXRefBuildResults & Results);
    virtual void   UnlinkAllXRefs();

    virtual void   SetState(eScdMdlStateActs RqdState);
    virtual void   EvalProducts(long JoinMask);
    virtual dword  ModelStatus();

    DEFINE_CI(CQueueCon, FlwNode, 4);

  };
  
//===========================================================================

#endif

#undef DllImportExport

#endif

