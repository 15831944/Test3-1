//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __ELECCB_H
#define  __ELECCB_H

#ifndef __SC_DEFS_H
#include "sc_defs.h"
#endif  
#ifndef __FLWNODE_H
#include "flwnode.h"
#endif
//#ifndef __M_BASE_H
//#include "m_base.h"
//#endif  
#include "models.h"

#ifdef __ELECCB_CPP
  #define DllImportExport 
#elif !defined(Power1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//============================================================================

const int MaxPGMWatch = 15;

DEFINE_TAGOBJ(CElecBlk);
class CElecBlk : public FlwNode//, GCXRefHelper
  {
  public:

    CElecBlk(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CElecBlk();
    virtual void   ResetData(flag Complete);

  private:
    void           SetMaxVarData(long NewSize);
    void           DDBAddWatchedVars(DataDefnBlk& DDB, char* pHdTag, pGCVar VarList, int Lvl, int &nPg, int &nOnPg, flag ForceNewPage, int MaxPerPage);
    void           CountWatchedVars(pGCVar VarList, int & Count);
    flag           LoadPGM(char* pPGMName, flag WithDebug);
  public:
    virtual LPTSTR GetPrimaryCfg();
    virtual flag   PutPrimaryCfg(LPCTSTR Cfg);

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual void   CollectElectrics(CNodeElectricsArray & TSCA);
    virtual void   ConnectElectrics();
    virtual flag   PreStartCheck();
    virtual void   EvalCtrlInitialise();
    virtual void   EvalCtrlActions(); 
    virtual void   EvalCtrlStrategy(); 
    virtual void   EvalCtrlTerminate();
            void   DoEvalCtrl(LPCTSTR TriggerName); 
    virtual void   SetState(eScdMdlStateActs RqdState);
    virtual int    ChangeTag(pchar pOldTag, pchar pNewTag);
    virtual int    DeleteTag(pchar pDelTag);
      
    virtual flag   GetOtherData(FilingControlBlock &FCB);
    virtual flag   PutOtherData(FilingControlBlock &FCB);
    virtual void   OnAppActivate(BOOL bActive);
    virtual int    FilesUsed(CFilesUsedArray & Files);

    // CNodeXRefMngr Overides
    virtual bool   IsXRefListActive();
    virtual int    UpdateXRefLists(CXRefBuildResults & Results);
    virtual void   UnlinkAllXRefs();

    virtual dword  ModelStatus();

    DEFINE_CI(CElecBlk, FlwNode, 6+MaxCondMsgs+MaxCondMsgs);

  public:
    Strng          sPGMName;             //name of pgm file
    Strng          sPGMFile;             //temp name of pgm file
    Strng          sPGMPath;             //temp path of pgm file
    GCInsMngr      PgmMngr;              //PGM manager

    GCVar**        m_VarData;              //pointer to array of data vars (external refrences)
    int            m_nVarData;             //number of data vars
    long           m_nMaxVarData;          //current max length of array of data vars
    CCriticalSection m_VarDataSect;

    HANDLE         hProcess;             //process handle for the editor
    DWORD          dwProcessId;          //process ID for the editor
    FILETIME       EditTime;             //time editor scheduled
    flag           bOn;                  //must the pgm be executed etc
    flag           bAutoReload;          // AutoReload ?
    flag           bReloadReqd;          //
    flag           bWithDBG;             //
    int            iIterCnt;             //iteration counter
    byte           m_bMustInit:1,        //flag set to indicate that Initialise must happen
                   m_bIterOne:1,         //flag set for first iteration when run is pressed
                   m_bMustTerm:1,        //flag set to indicate that Terminate must happen
                   bJustLoaded:1,        //flag set True just after PGM is loaded
                   bEmpty:1,             //flag set True for Empty SetState option
                   bPreset:1;            //flag set True for Preset SetState option

    CETermStripArray  m_PGMTermStrips;

  };
  
//===========================================================================

#undef DllImportExport

#endif

