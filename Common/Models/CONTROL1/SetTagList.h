//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __SETTAGLIST_H
#define  __SETTAGLIST_H

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

#ifdef __SETTAGLIST_CPP
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

class CTagListCon;

class TagListFileHelper
  {
  public:
    flag       bValid:1;
    flag       bLogSetTags;

    Strng      sStatus;
    Strng      sPath;
    Strng      sPathForView;
    Strng      sFile;
    HANDLE     hProcess;             //process handle for the editor
    DWORD      dwProcessId;          //process ID for the editor
    CTagListCon* pParent;

    TagListFileHelper(CTagListCon* Parent);
    virtual ~TagListFileHelper();
    bool LoadAndSetTags(bool TestParamStopped);
  };

//--------------------------------------------------------------------------

DEFINE_TAGOBJ(CTagListCon);
class CTagListCon : public FlwNode
  {
  public:
    flag           bOn;
    flag           bSetBeforeStart;
    flag           bSetAtStart;
    TagListFileHelper TLH;              //
    flag           bAboutToStart:1;  //flag set True for first iteration when run is pressed

    CTagListCon(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CTagListCon();
    virtual void   ResetData(flag Complete);
  private:

  public:
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual flag   PreStartCheck();
    virtual void   EvalCtrlInitialise();
    virtual void   EvalCtrlStrategy();
    virtual int    ChangeTag(pchar pOldTag, pchar pNewTag);
    virtual int    DeleteTag(pchar pDelTag);
    virtual int    FilesUsed(CFilesUsedArray & Files);

    virtual void   SetState(eScdMdlStateActs RqdState);
    virtual void   EvalProducts(long JoinMask);
    virtual dword  ModelStatus();

    DEFINE_CI(CTagListCon, FlwNode, 4);
  };
  
//===========================================================================

#endif

#undef DllImportExport

#endif

