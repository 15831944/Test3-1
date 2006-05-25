//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __EVENTSCON_H
#define  __EVENTSCON_H

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

#ifdef __EVENTSCON_CPP
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

//--------------------------------------------------------------------------

class CEventsCon;

class EventRowInfo
  {
  public:
    double     m_dTime; //seconds
    double     m_dOutputVal;
    Strng      m_sOutputTag;

    EventRowInfo();
    EventRowInfo(double Time, double Output, char* OutputTag);
  };

class EventConInfo
  {
  public:
    flag       bValid:1,
               bReloadRqd:1;
    flag       bWrapArround;  //must the events wrap arround after full time
    flag       bLogSetTags;

    Strng      sStatus;
    Strng      sPath;
    Strng      sPathForView;
    Strng      sFile;
    HANDLE     hProcess;             //process handle for the editor
    DWORD      dwProcessId;          //process ID for the editor
    long       iRowCnt;
    EventRowInfo ** RowData;
    double     dStartTime;
    double     dTimePassed;
    long       iCurIndex;
    CEventsCon* pParent;

    EventConInfo(CEventsCon* Parent);
    virtual ~EventConInfo();
    long SetRowCnt(long NewSize);
    flag DoLoad();
    void Sort();
    void LoadAndAdvance(double ICTime, double TimePassed);
    void Start(double ICTime);
    void ExecIns(double ICTime);
  };

//--------------------------------------------------------------------------

DEFINE_TAGOBJ(CEventsCon);
class CEventsCon : public FlwNode
  {
  public:
    flag           bOn;
    EventConInfo   ECI;              //
    flag           bAboutToStart:1;  //flag set True for first iteration when run is pressed

    CEventsCon(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CEventsCon();
    virtual void   ResetData(flag Complete);
  private:
    void           SetCount(long NewSize);

  public:
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual flag   PreStartCheck();
    virtual void   EvalCtrlStrategy(eScdCtrlTasks Tasks=CO_All);
    virtual int    ChangeTag(pchar pOldTag, pchar pNewTag);
    virtual int    DeleteTag(pchar pDelTag);
    virtual int    FilesUsed(CFilesUsedArray & Files);

    virtual void   SetState(eScdMdlStateActs RqdState);
    virtual void   EvalProducts(long JoinMask);
    virtual dword  ModelStatus();

    DEFINE_CI(CEventsCon, FlwNode, 4);
  };
  
//===========================================================================

#endif

#undef DllImportExport

#endif

