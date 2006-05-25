//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __ARCHMAN_H
#define  __ARCHMAN_H

#ifndef __EXECUTIV_H
#include "executiv.h"
#endif

#include <afxtempl.h>

//------------------------------------------------------------------------

class CArchiveManager; // forward;

//------------------------------------------------------------------------

class CArcAddress
  {
  public:
    CString   m_sTag;
    long      m_lInputIndex;
    long      m_lFieldIndex;
    CArcAddress()
      {
      m_lInputIndex=-1;
      m_lFieldIndex=-1;
      };
    void SetTag(LPCTSTR Tag) 
      { 
      m_sTag=Tag; 
      };
    void SetIndex(CArchiveManager & AM);
    //bool IndexOK() { return m_lInputIndex>=0 || m_lFieldIndex; };
    double GetValue(CArchiveManager & AM);
  };

//------------------------------------------------------------------------

class CArchiveInput
  {
  public:
    CString     m_sScdMTag;
    byte        m_iType;
    CCnvIndex   m_iCnv;             //conversion index
    Strng       m_sCnvTxt;          //engineering units
    
    double      m_dValue;

    flag PutValue(CPkDataItem &PItem);
    double GetValue() { return m_dValue; };
  };

class CAInputArray : public CArray < CArchiveInput*, CArchiveInput* > {};


//------------------------------------------------------------------------

class CArchiveDBField
  {
  public:
                                  // By Example
    CArcAddress     m_Src;        // m_Src.m_sTag = SomeTag
    CString         m_sTable;     // ATable
    CString         m_sFldTag;    // SomeTag-AVG
    CString         m_sScdMTag;    // ATable.SomeTag.AVG
    byte            m_bWhat;
    //long            m_lInputIndex;
    long            m_lTableIndex;
    byte            m_iType;
    CCnvIndex       m_iCnv;             //conversion index
    Strng           m_sCnvTxt;          //engineering units
    CString         m_sDescription;   
                    
    ADOX::DataTypeEnum m_dbType;
    ADOX::ColumnAttributesEnum m_dbAttr;
                    
    bool            m_bReset;
    bool            m_bChanged;
    long            m_nSamples;
    double          m_dValue; // ...
 
  public:
    CArchiveDBField();
    ~CArchiveDBField();

    flag GetValue(CPkDataList &List, /*CPkDataItem * &pPItem,*/ flag Complete);
    double GetValue() { return m_dValue; };
    flag Execute(CXM_TimeControl &CB, CArchiveManager & DrvMan);
    void ClearChanged() { m_bChanged=false; };
    bool Changed() { return m_bChanged; };

  };

class CArchiveTrigger
  {
  public:
    CArcAddress     m_Src;
                    
    bool            m_bInitReqd;
    bool            m_bRise;
    bool            m_bFall;
    bool            m_bAny;
    double          m_dPrevAny;
    double          m_dPrevEdge;
    double          m_dDeltaRise;
    double          m_dDeltaFall;
    double          m_dDeltaAny;

    CArchiveTrigger();
    ~CArchiveTrigger();

    bool Parse(LPCTSTR Cfg);
    bool TestTrigger(CArchiveManager  & ArcMan);

  };

class CADBFieldArray : public CArray<CArchiveDBField*, CArchiveDBField*> {};
class CADBFieldMap : public CMap <LPCTSTR, LPCTSTR, long, long> {};
class CATriggerArray : public CArray <CArchiveTrigger*, CArchiveTrigger*> {};

class CArchiveDBTable
  {
  public:
    CString         m_sName;
    CString         m_sCfgSection; // Starts @ 1
    long            m_lPeriod;
    long            m_lOffset;
    bool            m_bInitReqd;
    CTimeValue      m_dNextSaveTime;
    bool            m_bPeriodOn;
                    
    CATriggerArray  m_Triggers;
    CADBFieldArray  m_Fields;

    CArchiveManager  &m_rAM;
    ADODB::_RecordsetPtr m_pRst;
    ADOX::_TablePtr m_pDefs;

    long              TriggerCount() { return m_Triggers.GetSize(); };
    long              FieldCount() { return m_Fields.GetSize(); };
    CArchiveTrigger   &GetTrigger(long Index) { return *m_Triggers[Index]; };
    CArchiveDBField   &GetField(long Index) { return *m_Fields[Index]; };

  public:
    CArchiveDBTable(CArchiveManager *pADB);
    ~CArchiveDBTable();
    bool AddPeriod(long Period, long Offset);
    bool AddTrigger(LPCTSTR Cfg);
    bool Execute(CXM_TimeControl &CB, CArchiveManager & DrvMan);
    bool CreateDBTable();
    bool CreateDBFields(ADOX::_TablePtr & pTbl);
    bool CreateDBField(ADOX::_TablePtr & pTbl, bool SysFld, LPCTSTR FldName, ADOX::DataTypeEnum FldType, ADOX::ColumnAttributesEnum FldAttr, bool MakeAutoInc=false);
    bool CreateDBRecordset();
    bool CloseDBTable();
    bool AddDBRecord(CXM_TimeControl &CB);
  };

class CADBTableArray : public CArray<CArchiveDBTable*, CArchiveDBTable*> {};

//========================================================================
//========================================================================
//========================================================================

//------------------------------------------------------------------------

class CArchiveManager : public CExecObj//, public CArchiveManagerBase
  {
  friend class CArchiveDBTable;
  friend class CAddToArchive;

  protected:
    bool            m_bIsOpen,
                    m_bDBOK,
                    m_bRegistered,
                    m_bOpenDBOnRun,
                    m_bDoneLocalLoadSave;

    CString         m_sCfgFile;
    CString         m_sDataFile;
    CADBTableArray  m_Tables;
    CADBFieldArray  m_Fields;
    CADBFieldMap    m_FieldMap;
    CAInputArray    m_Inputs;

    ADODB::_ConnectionPtr m_pCnn;
    ADOX::_CatalogPtr     m_pCat;

    bool            m_bDBExclusive;
    bool            m_bDBReadOnly;
    CString         m_sDBConnect;

    long            m_lChgSrchStart;
    long            m_iLoadSaveCnt;    
    CEvent          m_evDone;

    CString         m_sPrevTempFile;    //file last used for saving driver state

    long            m_nFieldsOpn;
    long            m_nTablesOpn;

    long            m_lInputOffset;
    long            m_lBusyChanges;
    bool            m_bSubsRead_Busy;

    CString         m_sSeparator;

  public:
    CArchiveManager();
    ~CArchiveManager();

    bool            ReadCfg();
    bool            CompleteOpen();
                    
    flag            Execute(CXM_TimeControl &CB);
                    
    long            AddField(LPCTSTR Table, LPCTSTR CfgString);
    long            AddField(LPCTSTR Tag, LPCTSTR Table, byte iFn, byte iType, CCnvIndex iCnv, LPCTSTR CnvTxt, LPCTSTR Description);
    long            AppendField(LPCTSTR Tag, LPCTSTR Table, byte iFn, byte iType, CCnvIndex iCnv, LPCTSTR CnvTxt, LPCTSTR Description);
    long            AddInput(LPCTSTR Tag);
    long            FindInputIndex(LPCTSTR Tag);
    long            FindFieldIndex(LPCTSTR Tag);
    long            FindTable(LPCTSTR TbName);
    long            FindField(LPCTSTR TbName, LPCTSTR FldName);
    long            FindField(long iTb, LPCTSTR FldName);
    byte            FieldType(long Index);

    flag            GetCalcValue(long Index, CPkDataList &List, /*CPkDataItem * &pPItem,*/ flag Complete);
    long            NoOfChanges(); 
    long            GetNextChange();

    long            TableCount() { return m_Tables.GetSize(); };
    long            FieldCount() { return m_Fields.GetSize(); };
    long            InputCount() { return m_Inputs.GetSize(); };
    CArchiveDBTable & GetTable(long Index);
    CArchiveInput & GetInput(long ItemIndex);
    CArchiveDBField & GetField(long ItemIndex);

    bool            OpenTheDB();
    bool            OpenDB();
    bool            CreateDBTables();
    bool            CreateDBRecordsets();
    bool            CloseDB();
    bool            Start();
    bool            Stop();
                    
    void            DisplayException(_com_error & e, DWORD LogOpts=0, LPCTSTR strMsg1="", LPCTSTR strMsg2="");
                    
    bool            SendStart();
    bool            SendExec();
    bool            SendStop();
    void            SetEvent() { ::SetEvent(m_evDone); };
                    
    void            Initialise();
    int             Configure(LPCTSTR CfgFile, bool OpenDBOnRun);
    void            Options(char* pGotoTag = NULL);
    void            CloseOptions();
    int             Open();
    flag            Close(BOOL SaveState = TRUE);
                    
    void            SaveTags(FilingControlBlock &FCB, BOOL SaveAll);
    void            LoadTags(FilingControlBlock &FCB);

    void            AddFieldDlg(TagInfoBlk &IB);

  private :
   
    // ExecObj Overrides
    virtual DWORD   EO_Message(CXMsgLst &XM, CXM_Route &Route);
    virtual flag    EO_QueryTime(CXM_TimeControl &CB, CTimeValue &TimeRqd, CTimeValue &dTimeRqd);
    virtual flag    EO_Start(CXM_TimeControl &CB);
    virtual void    EO_QuerySubsReqd(CXMsgLst &XM);
    virtual void    EO_QuerySubsAvail(CXMsgLst &XM, CXMsgLst &XMRet);
    virtual flag    EO_ReadSubsData(CXMsgLst &XM);  
    virtual flag    EO_WriteSubsData(CXMsgLst &XM, flag FirstBlock, flag LastBlock);  
    virtual DWORD   EO_ReadTaggedItem(CXM_ObjectTag & ObjTag, CXM_ObjectData &ObjData, CXM_Route &Route);
    virtual int     EO_WriteTaggedItem(CXM_ObjectData &ObjData, CXM_Route &Route);
    virtual flag    EO_Execute(CXM_TimeControl &CB, CEOExecReturn &EORet);
    virtual flag    EO_Stop(CXM_TimeControl &CB);
    virtual flag    EO_TagsNotAvail(CXMsgLst &XM);
    virtual flag    EO_BeginSave(FilingControlBlock &FCB);
    virtual flag    EO_SaveDefinedData(FilingControlBlock &FCB, Strng &Tag, CXMsgLst &XM);
    virtual flag    EO_SaveOtherData(FilingControlBlock &FCB);
    virtual flag    EO_BeginLoad(FilingControlBlock &FCB);
    virtual flag    EO_LoadDefinedData(FilingControlBlock &FCB, CXMsgLst &XM);
    virtual flag    EO_LoadOtherData(FilingControlBlock &FCB);
    virtual flag    EO_EndLoad(FilingControlBlock &FCB);
    virtual int     EO_QueryChangeTag(pchar pOldTag, pchar pNewTag);
    virtual int     EO_ChangeTag(pchar pOldTag, pchar pNewTag);
    virtual int     EO_QueryDeleteTag(pchar pDelTag);
    virtual int     EO_DeleteTag(pchar pDelTag);
    virtual void    EO_OnAppActivate(BOOL bActive);
    virtual flag    EO_RequestTagInfo(RequestTagInfoRec& Rqst, ReplyTagInfoRec& Info);
  };

inline byte CArchiveManager::FieldType(long Index) { return m_Fields[Index]->m_iType; };
inline CArchiveDBTable & CArchiveManager::GetTable(long ItemIndex) { return *m_Tables[ItemIndex]; };
inline CArchiveInput & CArchiveManager::GetInput(long ItemIndex) { return *m_Inputs[ItemIndex]; };
inline CArchiveDBField & CArchiveManager::GetField(long ItemIndex) { return *m_Fields[ItemIndex]; };
inline flag CArchiveManager::GetCalcValue(long Index, CPkDataList &List, /*CPkDataItem * &pPItem,*/ flag Complete)
 { return m_Fields[Index]->GetValue(List, /*pPItem,*/ Complete); };

//-------------------------------------------------------------------------

extern CArchiveManager* gs_pArcMan; //pointer to one and only driver manager EO

//-------------------------------------------------------------------------

#endif

