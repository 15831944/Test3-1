//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __DRV_MNGR_H
#define  __DRV_MNGR_H

#ifndef __EXECUTIV_H
#include "executiv.h"
#endif
#ifndef __DRV_BASE_H
#include "drv_base.h"
#endif
#include "pgm_e.h"

//------------------------------------------------------------------------

#if WITHDRVMAN

_FWDDEF(CConnSubsItem)
class CConnSubsItem
  {
  public:
    CDrvSubsConn*  m_pCon;        //pointer to first connection with a unique non driver tag
    XIOAction      m_iAction;     //IO type

    CConnSubsItem(CDrvSubsConn* pConSlot);
    void AddConn(CDrvSubsConn* pConSlot);
  };

//------------------------------------------------------------------------

typedef CArray < DrvSpan*, DrvSpan* > CDrvSpanArray;
typedef CArray < DrvRange*, DrvRange* > CDrvRangeArray;
typedef CArray < DrvHstCfg*, DrvHstCfg* > CDrvHstCfgArray;
typedef CArray < DrvArcCfg*, DrvArcCfg* > CDrvArcCfgArray;
typedef CArray < CConnSubsItem*, CConnSubsItem* > CConnSubsItemArray;
typedef CArray < CDrvSubsConn*, CDrvSubsConn* > CDrvSubsConnArray;
typedef CList  < CDrvSltChange*, CDrvSltChange* > CDrvSltChangeList;

typedef CSmartPtr <CDrvSlot> CSPDrvSlot;
typedef CSmartPtr <CDrvCodeBlk> CSPDrvCodeBlk;
typedef CArray < CSPDrvSlot, CSPDrvSlot& > CDrvSlotArray;

typedef CArray < CDrvCodeBlk*, CDrvCodeBlk*> CDrvCodeBlkArray;

//------------------------------------------------------------------------

class CDriverManager;
class CDriverArray
  {
  public :
    CDriverArray(CDriverManager *pMngr);
    ~CDriverArray();
    void              Alloc(int Maximum);
    pCDriver          Find(const char* pTag);             
    pCDriver          Add(const char *sPath, const char *pTag);
    inline int        InUse() { return Used; };
    inline pCDriver   At(int i) { return *(pItem+i); };
    inline int        Index(pCDriver pDrv)
     { for (int i=0; i<Used; i++) 
         if (*(pItem+i) == pDrv) return i;
       return -1;
     }
  private :
    pCDriver*   pItem;
    int         Used;
    int         Maximum;
    CDriverManager *m_pMngr;
  };

//------------------------------------------------------------------------

typedef CMap <char*, char*, long, long> CDrvSlotMap;
typedef CMap <char*, char*, long, long> CCodeBlkMap;

const byte FBKCheckNever=0;
const byte FBKCheckOnce=1;
const byte FBKCheckSkipOnce=2;
const byte FBKCheckAlways=3;

class CDriverManager : public CExecObj, public CDriverManagerBase, public CNodeXRefMngr//GCXRefHelper
  {

  friend class CDriverArray;
  friend class CDriverPage;
  friend class CSlotsPage;
  public:
    CTimeValue      dCurrentTime;
    int             UsedSlots; //number of slots
    int             UsedConns; //number of connections
    int             UsedCodeBlks;
    CConnSubsItemArray  ConnSubsItems; //for each unique tag in connect column requiring data subscription
    CDrvSubsConnArray   ConSlots;  //for each tag in the connect column
    CDrvSlotArray   Slots;     //for each logged looptag (flt,wrd,bit)
    CDrvSpanArray   Spans;     //digital span,offset on inputs
    CDrvRangeArray  Ranges;    //range in engineering units of 
    CDrvHstCfgArray HstCfg;    //settings for historian logging of tagged data
    CDrvArcCfgArray ArcCfg;    //settings for historian logging of tagged data
    CDriverArray    Drivers;   //array of drivers
    DWORD           EO_SubsRead_Busy;
    int             BusyDriver;
    DWORD           BusyChanges;
    CDrvSltChangeList m_SltChanges; // list of changes that must be forwarded to EO_manager subscription lists
  
    byte            bDrvStartFBKCheck;

    CDrvCodeBlkArray m_CodeBlks;
    CCodeBlkMap     m_CodeBlkMap;

    //CDrvSltChange * pChgHead;  //linked list of changes that must be forwarded to EO_manager subscription lists
    //CDrvSltChange * pChgTail;  //end of linked list of changes that must be forwarded to EO_manager subscription lists

  protected:
    CDrvSlotMap     SlotMap;   //map of tags and index to array of slots
    byte            bIsOpen:1,        //is the driver manager open
                    bDoneLocalLoadSave:1,
                    bIgnoresOn:1,     //allow all slots with ignore read/writes to be ignored
                    bForcesOn:1,      //allow all slots with forces on to read write the force value
                    bRecordHist:1,    //is at least one slot recorded in the historian
                    bRecordHistDone:1,//has the historian been notified about recording slots
                    bReadAll:1,       //Set if all Slots to be read all the time
                    bNoiseOn:1;       //Set if Noise must be generated
    Strng           sPath;            //path to driver manager
    Strng           sName;            //scf name
    Strng           sPrevTempFile;    //file last used for saving driver state
    short           iLoadSaveCnt;     //number of driver currently being saved
    short           iRollingRdCntRqd; //after required count iterations force a read (use 0 for none)
    short           iRollingRdCnt;    //iteration counter to determine when next read occurs
    short           iRollingRdIndex;  //index in slot array for next forced read
    short           iRollingWrCnt;    //after required count iterations force a write (use 0 for none)
    short           iRollingWrCntRqd; //iteration counter to determine when next write occurs
    short           iRollingWrIndex;  //index in slot array for next forced write


    CXM_Route        m_DrvManRoute;
    flag            m_GotDrvManRoute;
    int             m_enArchiveDB;
    int             m_enTODO;
    
  public:
    CDriverManager();
    ~CDriverManager();
    
    void           Initialise();
    int            Configure(const char* Path, const char* Name, flag IgnoresOn, flag ForcesOn, flag ReadAll, flag NoiseOn, byte DrvStartFBKCheck);
    void           Options(char* pGotoTag = NULL);
    void           CloseOptions();
    int            Open();
    flag           Close(BOOL SaveState = TRUE);
    int            DoOpen();
    flag           DoClose(BOOL SaveState = TRUE);

    int            ConfigCodeBlocks();
    
    int            NoDrivers(); 
    DriverStates   DriverState(int i);

    virtual void   AddSltChange(long DataIndex, double dVal, CDrvSubsConn * pCon, long Level=0);
    virtual void   AddSltChange(long DataIndex, double dVal, CDrvSlot * pSlt, long Level=0);
    void           SetReadAll(flag State) { bReadAll=State; };

    void           RefreshAllSlotData(bool DoInputs, bool DoOutputs);
    void           RefreshConnections();

    LRESULT        ProcessMessage(WPARAM wParam, LPARAM lParam);
  
  private :
    void           ConfigurationDump(void);

    int            Slt_CfgOptions(CDrvCfgHelper &Cfg, char *pOpts, int line_number);
    int            Slt_CfgConnection(CDrvCfgHelper &Cfg, char *pConn, int line_number);
    int            Slt_CfgDriver(CDrvCfgHelper &Cfg, char *pDrv, int line_number);
    int            Slt_CfgArchiveDB(CDrvCfgHelper &Cfg, char *pDB, int line_number);
    int            Slt_CfgModifier(CDrvCfgHelper &Cfg, char *pMod, int line_number);
    int            Slt_CfgRange(CDrvCfgHelper &Cfg, char *pRng, int line_number);
    int            Slt_CfgFilter(CDrvCfgHelper &Cfg, char *pFlt, int line_number);
    int            Slt_CfgCompression(CDrvCfgHelper &Cfg, char *pCmp, int line_number);
    int            Slt_CfgCnvs(CDrvCfgHelper &Cfg, char *pCnv, int line_number);
    int            Slt_CfgTODO(CDrvCfgHelper &Cfg, char *pIOPnt, int line_number);    
    int            Slt_Configure(DWORD line_number, int nFlds, char **c);//, char *line);
    int            Slt_Configure2(DWORD SlotNo, CDrvTimeout * pTO);
    int            CB_Configure(DWORD line_number, int nFlds, char **c);//, char *line);

    bool           ParseConnOperator(CDrvSubsConn *pC, CDrvCfgHelper &Cfg, char *pMod, int line_number);
    CDrvSubsConn * ParseConnFunction(CDrvCfgHelper &Cfg, char* pConn, int line_number);

    int            FindChanForTag(const char* pTag);
    int            FindCodeBlkForTag(const char* pTag, CDrvCodeBlk *&pCdBlk, GCVar * &pVar);
    int            AddSlot(CDrvCfgHelper * pCfg);
    flag           ForceSlot(CDrvSlot& S);
    void           RecordSlots();
    CDrvSltChange *FindSltChange(long DataIndex);
    void           SaveTags(FilingControlBlock &FCB, BOOL SaveAll);
    void           LoadTags(FilingControlBlock &FCB);

    pDrvSpan       FindSpan(long span, long offset, long clamplo, long clamphi);
    pDrvSpan       AddSpan(long span, long offset, long clamplo, long clamphi);
    pDrvRange      FindRange(double RngLo, double RngHi);
    pDrvRange      AddRange(double RngLo, double RngHi);
    pDrvHstCfg     FindHstCfg(const pDrvHstCfg pH);
    pDrvHstCfg     AddHstCfg(const pDrvHstCfg pH);
    pDrvArcCfg     FindArcCfg(const pDrvArcCfg pH);
    pDrvArcCfg     AddArcCfg(const pDrvArcCfg pH);
    int            FindSubsItem(char* pTag);
    
    // ExecObj Overrides
    virtual DWORD  EO_Message(CXMsgLst &XM, CXM_Route &Route);
    virtual flag   EO_QueryTime(CXM_TimeControl &CB, CTimeValue &TimeRqd, CTimeValue &dTimeRqd);
    virtual flag   EO_Start(CXM_TimeControl &CB);
    virtual void   EO_QueryHistoryCfg(CXMsgLst &XM); 
    virtual void   EO_QueryArchiveCfg(CXMsgLst &XM); 
    virtual void   EO_QuerySubsReqd(CXMsgLst &XM);
    virtual void   EO_QuerySubsAvail(CXMsgLst &XM, CXMsgLst &XMRet);
    virtual flag   EO_ReadSubsData(CXMsgLst &XM);  
    virtual flag   EO_WriteSubsData(CXMsgLst &XM, flag FirstBlock, flag LastBlock);  
    virtual DWORD  EO_ReadTaggedItem(CXM_ObjectTag & ObjTag, CXM_ObjectData &ObjData, CXM_Route &Route);
    virtual int    EO_WriteTaggedItem(CXM_ObjectData &ObjData, CXM_Route &Route);
    virtual flag   EO_Execute(CXM_TimeControl &CB, CEOExecReturn &EORet);
    virtual flag   EO_Stop(CXM_TimeControl &CB);
    virtual flag   EO_TagsNotAvail(CXMsgLst &XM);
    virtual flag   EO_BeginSave(FilingControlBlock &FCB);
    virtual flag   EO_SaveDefinedData(FilingControlBlock &FCB, Strng &Tag, CXMsgLst &XM);
    virtual flag   EO_SaveOtherData(FilingControlBlock &FCB);
    virtual flag   EO_BeginLoad(FilingControlBlock &FCB);
    virtual flag   EO_LoadDefinedData(FilingControlBlock &FCB, CXMsgLst &XM);
    virtual flag   EO_LoadOtherData(FilingControlBlock &FCB);
    virtual flag   EO_EndLoad(FilingControlBlock &FCB);
    virtual int    EO_QueryChangeTag(pchar pOldTag, pchar pNewTag);
    virtual int    EO_ChangeTag(pchar pOldTag, pchar pNewTag);
    virtual int    EO_ChangeTagDone(pchar pOldTag, pchar pNewTag);
    virtual int    EO_QueryDeleteTags(Strng_List & Tags) ;
    virtual int    EO_DeleteTags(Strng_List & Tags) ;
    virtual void   EO_OnAppActivate(BOOL bActive);
    virtual flag   EO_RequestTagInfo(RequestTagInfoRec& Rqst, ReplyTagInfoRec& Info);
  };

//-------------------------------------------------------------------------

extern CDriverManager* gs_pDrvMan; //pointer to one and only driver manager EO

#endif

//-------------------------------------------------------------------------

#endif

