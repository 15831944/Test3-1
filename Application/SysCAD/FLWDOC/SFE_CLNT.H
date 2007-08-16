//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __SFE_CLNT_H
#define  __SFE_CLNT_H
#ifndef __EXECUTIV_H
  #include "executiv.h"
#endif
#ifndef __SFE_BASE_H
  #include "sfe_base.h"
#endif  
#ifndef __ENGSTATE_H
  #include "engstate.h"
#endif
#include "mdlrunmngr.h"
#include "sfe_clnt.h"

// ==========================================================================

_FWDDEF(SFEFlwLib)
class SFEFlwLib : public SFEMdlLib, public CExecObj
  {
  public:
    SFEFlwLib();
    virtual ~SFEFlwLib();

    virtual flag   Initialise(pchar ModelCfg, pchar FlwLibTag, pchar EO_LocationTag);
    virtual flag   Terminate();
    //virtual void   InitLic(dword dwLic);

    int            AddAttachedToMenu(CMenu& Menu, char* pTag);

    inline pNodeGrfInfo   NodeDrawings() { return SFENodeGrfInfo; };
    virtual flag   Loaded() { return pSfe!=NULL; };
    //inline void           Lock() { if (pSfe) pSfe->Lock(); };
    //inline void           Free() { if (pSfe) pSfe->Free(); };

#if WITHSFEWND 
    inline CMDIChildWnd*  GetSFEFrame() { return pSFEFrame; };
#endif
    
    void           Enter();
    void           Leave();
//    int            EnterLeave(int def, int fn) {if !pSfe return def; Enter(); int i=fn};
//    double         EnterLeave(double def, int fn) {};

    int            FE_DoInsert(char * Class, char *SubClass, char *PrimaryCfgIn, char *NTag, char *SrcTag, char *DstTag, FlwNode **pNd=NULL, LPSTR pGuidStr=NULL);
    int            FE_DoInsertRoot(char * RootName);
    int            FE_DoDelete(char * Tag);
    flag           FE_GetNodeGuid(pchar Tag, Strng & Guid);
    int            FE_ChangeNodeGuid(pchar Tag, pchar Guid);
    flag           FE_FindNodeInfoFromGuid(pchar Guid, Strng &Tag, flag & IsLink);
    int            FE_DoConnect(char * SrcTag, char *DstTag);
    int            FE_DoDisConnect(char * SrcTag, char *DstTag);
    int            FE_SetCommon(char * Tag, char * EqpDesc, char * EqpMemo, char * EqpIdStr, char * EqpLocation, char * PlantArea, char * EqpGUID);
    int            FE_GetCommon(char * Tag, Strng & EqpDesc, Strng & EqpMemo, Strng & EqpIdStr, Strng & EqpLocation, Strng & PlantArea, Strng & EqpGUID);
//    int            FE_DoSolveNet();
    int            FE_TagOperation(FE_TagOps Act, Strng_List & TagList);
    int            FE_SetHoldGlobalLinks(bool On);

    byte           FE_FlwMomentumOn();
    void           FE_SetFlwMomentumOn(byte On);

    byte           FE_ODE_Method();
    byte           FE_ODE_StepSizeControl();
    void           FE_ODE_SetMethod(byte Method_);
    void           FE_ODE_SetStepSizeControl(byte StepSizeControl_);
    double         FE_ODE_GetMagEPSAbs();
    double         FE_ODE_GetMagEPSRel();
    void           FE_ODE_SetMagEPSAbs(double Eps);
    void           FE_ODE_SetMagEPSRel(double Eps);
    double         FE_ODE_GetTimeInc();
    void           FE_ODE_SetTimeInc(double T);
    double         FE_ODE_GetTimeIncMxRqd();
    void           FE_ODE_SetTimeIncMxRqd(double T);
    double         FE_ODE_GetTimeIncMnRqd();
    void           FE_ODE_SetTimeIncMnRqd(double T);
    
    double         FE_Net_GetLnkEPSAbs();
    double         FE_Net_GetLnkEPSRel();
    void           FE_Net_SetLnkEPSAbs(double Eps);
    void           FE_Net_SetLnkEPSRel(double Eps);
    double         FE_Net_GetNetDeRate();
    void           FE_Net_SetNetDeRate(double Mult);
    pchar          FE_Net_GetSpillArea();
    void           FE_Net_SetSpillArea(pchar Spill);

    int            FE_AD_GetID      (int Index, Strng & Class, Strng & Name);
    int            FE_AD_Select     (int Index, char * Tag);
    int            FE_AD_SetSelectedTags(Strng_List &SelectedTags);
    int            FE_AD_GetTagTree (CFlwTree & Tree);
    int            FE_AD_GetStatus  (Strng & Status);
    int            FE_AD_Go         ();

    int            FE_IOPnt_Add     (LPCTSTR Tag, bool Input, LPCTSTR Desc, byte Type, LPCTSTR ValueStr, LPCTSTR ValueCnvs);
    int            FE_IOPnt_Remove  (LPCTSTR Tag);

    int            FE_GetEvalOrder(bool GetCtrl, bool GetFullDescI, bool GetFullDescO, eTraceValue TraceWhat, CEvalOrderArray &Info);
    int            FE_GetEvalStats(CEvalStatsArray &Info);
    int            FE_GetNodeList(CNodeList&List);
    int            FE_GetNodeConfiguration(BOOL AllParms, BOOL AllState, LPCTSTR NdTag, CNodeConfigList&List);
    int            FE_GetNodeBalanceInfo(CNodeBalanceInfo & Balance, CNodeAuditInfo & Audit);

    int            FE_SpModelFixupStrategy();
    void           FE_SetSpModelFixupStrategy(long Strategy);

    pchar          RequestModelDLLPath();
    pchar          RequestModelDLLTag();
    flag           RequestModelInfoByGroupIndex(pchar pGroup, int iIndex, RequestModelInfoRec & Info);
    flag           RequestModelInfoByClassId(pchar pClass, RequestModelInfoRec & Info);
    flag           RequestModelClassId(pchar pTag, Strng & ClassId);
    flag           RequestModelConnInfo(pchar pTag, RequestConnModelInfoRec & Info);
    flag           RequestModelIOInfoByClassId(pchar pClass, RequestModelIOInfoArray & Info);
    flag           RequestModelIOInfoByIndex(pchar pTag, int iIndex, RequestModelIOInfoRec & Info);
    flag           RequestModelIOInfoById(pchar pTag, int iId, RequestModelIOInfoRec & Info);
    flag           RequestModelIOConn(pchar pTag, int iNo, RequestModelIOConnRec & Info);

    int            RequestModelStatus(CModelStatusArray &Status, flag & TagsChanged);
    flag           TestLoadModelStatus();

    flag           RequestModelAssocGrfs(LPCTSTR Tag, CMdlAssocGrfList & Grf);
    flag           TestLoadModelAssocGrfs();

    int            RequestTagRefInfo(LPCTSTR Tag, CXRefInfoArray &Refs);
//    int            RequestXRefInfoChg();
    flag           RequestModelAssocGrfsMasks(bool SetClass, bool SetNode,LPCSTR NdTag, CModelAssocGrfMasks & Msks);

    int            FE_GetRctInfo(CRctBlkInformation & RctInfo);
    int            FE_SetRctInfo(LPCTSTR NodeTag);

    int            FE_GetNodeWiring(CNodeWiring &NodeWiring);
    int            FE_GetLinkWiring(CLinkWiring &LinkWiring);
    int            FE_SetLinkWiring(CLinkWiring &LinkWiring);

    void           FE_SetModelStatusOptions(dword Options);
    flag           FE_TestModelTagUnique(pchar pTag);
    int            FE_LoadAllModelStatus();
    dword          FE_GetModelStatus(pchar pTag);
    CModelStatusArray & FE_MdlStatus() { return m_Status.m_Status; };
 
    void           LockStatus() { m_Status.m_Sect.Lock(); };
    void           FreeStatus() { m_Status.m_Sect.Unlock(); };
    void           SetLoadAllModelStatus(bool SetOptions=false, dword ReqdOptions=0)
      { 
      LockStatus(); 
      m_Status.m_LoadReqd=true; 
      if (SetOptions) 
        m_Status.m_dwOptions=ReqdOptions; 
      FreeStatus();
      };
    dword          GetStatusLoadSeqNo() 
      { 
      LockStatus();
      dword dw=m_Status.m_nLoadSeqNo; 
      FreeStatus(); 
      return dw;
      };
    
    int            FE_LoadAllModelAssocGrfs();
    int            FE_PrepareAllModelAssocGrfs(dword & LoadSeqNo, CMdlAssocGrfList *&pGrfs, CMdlAssocGrfMap *&pMap);

    void           LockAssocGrfs() { m_AssocGrfs.m_Sect.Lock(); };
    void           FreeAssocGrfs() { m_AssocGrfs.m_Sect.Unlock(); };
    void           SetLoadAllModelAssocGrfs() 
      {
      LockAssocGrfs();
      m_AssocGrfs.m_LoadReqd=true; 
      FreeAssocGrfs();
      };
    long           GetAssocGrfLoadSeqNo() 
      { 
      LockAssocGrfs(); 
      dword dw=m_AssocGrfs.m_nLoadSeqNo; 
      LockAssocGrfs(); 
      return dw;
      };

    void           EO_SetModelState(eScdMdlStateActs RqdState, Strng_List &RqdTags) ;
    flag           EO_GetModelAction(LPCTSTR Tag, CMdlActionArray & Acts);
    flag           EO_SetModelAction(LPCTSTR Tag, CMdlAction & Acts);

    flag           EO_GetModelGraphic(LPCTSTR Tag, CMdlGraphicArray & Grf);
    flag           EO_OperateModelGraphic(LPCTSTR Tag, CMdlGraphicWnd & Wnd, CMdlGraphic & Grf);

    int            EO_GetSolutionError(EqnSlvCtrlBlk & EQCtrl, Strng & RptLine);

    // CExecObj Overrides
    void           EO_OnAppActivate(BOOL bActive);
    flag           EO_RequestTagInfo(RequestTagInfoRec& Rqst, ReplyTagInfoRec& Info);
    int            EO_QueryChangeTag(pchar pOldTag, pchar pNewTag);
    int            EO_ChangeTag(pchar pOldTag, pchar pNewTag);
    int            EO_ChangeTagDone(pchar pOldTag, pchar pNewTag);
    int            EO_QueryDeleteTag(pchar pDelTag);
    int            EO_DeleteTag(pchar pDelTag);
    int            EO_DeleteTagDone(pchar pDelTag);
    int            EO_CanClose(Strng_List & Problems);

    void           EO_SetSolveMode();
    flag           EO_Starting(flag fBeginStarting);
    flag           EO_SetTime(CTimeValue TimeRqd, bool Rewind);
    flag           EO_QueryTime(CXM_TimeControl &CB, CTimeValue &TimeRqd, CTimeValue &dTimeRqd);
    flag           EO_GotoRunStart(CXM_TimeControl &CB);
    flag           EO_GotoRunEnd(CXM_TimeControl &CB, bool TimeChanged);
    flag           EO_PreStart(CXM_TimeControl &CB);
    flag           EO_Start(CXM_TimeControl &CB);
    void           EO_QuerySubsReqd(CXMsgLst &XM);
    void           EO_QuerySubsAvail(CXMsgLst &XM, CXMsgLst &XMRet);
    flag           EO_ReadSubsData(CXMsgLst &XM);
    flag           EO_WriteSubsData(CXMsgLst &XM, flag FirstBlock, flag LastBlock);
    DWORD          EO_ReadTaggedItem(CXM_ObjectTag & ObjTag, CXM_ObjectData &ObjData, CXM_Route &Route);
    int            EO_WriteTaggedItem(CXM_ObjectData &ObjData, CXM_Route &Route);
    flag           EO_Execute(CXM_TimeControl &CB, CEOExecReturn &EORet);
    flag           EO_StepDone(CXM_TimeControl &CB, flag StepExecuted);
    flag           EO_QueryStop(CXM_TimeControl &CB, bool Starting);
    flag           EO_Stop(CXM_TimeControl &CB);
    flag           EO_Stopping(flag BeginStopping);
    flag           EO_BeginPause(CXM_TimeControl &CB);
    flag           EO_EndPause(CXM_TimeControl &CB);
                   
    flag           EO_BeginSave(FilingControlBlock &FCB);
    flag           EO_SaveDefinedData(FilingControlBlock &FCB, Strng &Tag, CXMsgLst &XM);
    flag           EO_SaveOtherData(FilingControlBlock &FCB);
    flag           EO_SaveConn(FilingControlBlock &FCB, CXMsgLst &XM);
    flag           EO_EndSave(FilingControlBlock &FCB);
    flag           EO_BeginLoad(FilingControlBlock &FCB);
    flag           EO_LoadDefinedData(FilingControlBlock &FCB, CXMsgLst &XM);
    flag           EO_LoadOtherData(FilingControlBlock &FCB);
    flag           EO_LoadConn(FilingControlBlock &FCB, CXMsgLst &XM);
    flag           EO_EndConnect(FilingControlBlock &FCB);
    flag           EO_EndLoad(FilingControlBlock &FCB);
    void           EO_BeginBulkChange(); 
    void           EO_EndBulkChange(); 

  protected:
    Strng             sFlwLibTag;

#if WITHSFEWND 
    CMDIChildWnd    * pSFEFrame;
    EngStateWnd     * pSFEWnd;
#endif

    CSfeSrvrBase    * pSfe;
    NodeGrfInfo     * SFENodeGrfInfo;
    
    int               FPP_State;   // FPP State

    struct CMdlStatus
      {
      
      CModelStatusArray m_Status;
      CModelStatusMap   m_StatusMap;

      dword             m_dwOptions;
      flag              m_LoadReqd;
      dword             m_nLoadSeqNo;
      CCriticalSection  m_Sect;
      }               m_Status;

    struct CAssocGrfs
      {
      CMdlAssocGrfList  m_Grfs;
      CMdlAssocGrfMap   m_Map;

      flag              m_LoadReqd;
      dword             m_nLoadSeqNo;
      CCriticalSection  m_Sect;
      }               m_AssocGrfs;
  };

extern SFEFlwLib      * gs_pTheSFELib;
extern CAppRunManager * gs_pAppRunMngr;

// ==========================================================================

#endif




