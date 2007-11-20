//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __MARSHAL_H
#define  __MARSHAL_H

#ifndef __EXECUTIV_H
#include "executiv.h"
#endif

#include "..\..\..\Scada\SysCADMarshal\SysCADMarshal_i.h"
#include "executiv.h"

// ========================================================================
//
//
//
// ========================================================================

template<class T, const IID * pIID>
class ScdInterfaceWrap
  {
public:
  ScdInterfaceWrap(void) :
  m_pI(NULL)
    {
    }

  ~ScdInterfaceWrap(void)
    {
    Detach();
    }
  operator T * ()
    {
    return m_pI;
    }
  operator IUnknown * ()
    {
    return (IUnknown *) m_pI;
    }
  T* operator->() // CNM
    {
    return (T*) m_pI;
    }
  operator IUnknown ** ()
    {
    return (IUnknown **) &m_pI;
    }
  operator LPVOID * ()
    {
    return (LPVOID *) &m_pI;
    }
  HRESULT Attach(IUnknown *pUnk)
    {
    ASSERT(pUnk != NULL);
    if(m_pI == pUnk) return S_OK;
    Detach();

    HRESULT hr = pUnk->QueryInterface(*pIID, (LPVOID *) &m_pI);
    //if(FAILED(hr)) TRACE_MSG(_T("QueryInterface"), hr);
    return hr;
    }

  HRESULT Attach(T *pI)
    {
    ASSERT(pI != NULL);
    if(m_pI == pI) return S_OK;
    Detach();
    m_pI = pI;
    m_pI->AddRef();
    return S_OK;
    }

  void  Detach(void)  { if(m_pI) m_pI->Release(); m_pI = NULL; }
  HRESULT QueryInterface(REFIID iid, void **ppvObject)
    {
    if(m_pI) return m_pI->QueryInterface(iid, ppvObject);
    return E_FAIL;
    }

  BOOL IsOk(void) const
    {
    return(m_pI != NULL);
    }

//protected:
  T *m_pI;
  };

typedef ScdInterfaceWrap<IScdMarshal, &IID_IScdMarshal> IScdMarshalWrap;
typedef ScdInterfaceWrap<IScdExecObject, &IID_IScdExecObject> IScdExecObjectWrap;

// ========================================================================
//
//
//
// ========================================================================

class CScdMarshalEventsBase :
  public _IScdMarshalEvents,
  public CComObjectRootEx<CComMultiThreadModel>
  {
  public:
    CScdMarshalEventsBase() {;}
    virtual  ~CScdMarshalEventsBase(){;}

BEGIN_COM_MAP(CScdMarshalEventsBase)
   COM_INTERFACE_ENTRY(_IScdMarshalEvents)
END_COM_MAP()

DECLARE_NOT_AGGREGATABLE(CScdMarshalEventsBase)

  };

  class CScdMarshalEvents : public CComObject<CScdMarshalEventsBase> {};

// ========================================================================
//
//
//
// ========================================================================

class CScdExecObjectEventsBase :
  public _IScdExecObjectEvents,
  public CComObjectRootEx<CComMultiThreadModel>
  {
  public:
    CScdExecObjectEventsBase() {;}
    virtual  ~CScdExecObjectEventsBase(){;}

BEGIN_COM_MAP(CScdExecObjectEventsBase)
   COM_INTERFACE_ENTRY(_IScdExecObjectEvents)
END_COM_MAP()

DECLARE_NOT_AGGREGATABLE(CScdExecObjectEventsBase) 
public :

   virtual HRESULT STDMETHODCALLTYPE ReadTaggedItems( 
           DWORD __RPC_FAR dwCount,
           SCD_TAGITEM __RPC_FAR **pTag,
           SCD_DATAITEM __RPC_FAR **pData,
           DWORD __RPC_FAR **pRetCds);

  };
typedef CComObject<CScdExecObjectEventsBase> CScdExecObjectEvents;

// ========================================================================
//
//
//
// ========================================================================

class CIOMarshalItem
  {
  public:
    bool          m_bOK;
    CString       m_sTag;
    //CString       m_sConnTag;
    VARTYPE       m_vtType;
    byte          m_iTyp;
    CCnvIndex     m_iCnv;
    CString       m_sCnvTxt; 

    DWORD			    m_dwItemIndex;
    DWORD			    m_dwHandle;
	  DWORD			    m_dwOptions;
		SCD_XIOAction	m_dwAction;

    CIOMarshalItem()
      {
      m_bOK=false;
      m_iTyp=0;
      m_iCnv=0;
      }
  };


class CIOMarshal : public CExecObj
  {
  public:

    CXM_Route       m_DrvManRoute;
    flag           m_GotDrvManRoute;

    void           Initialise();
    int            Configure(LPCSTR File, LPCSTR Node);//, flag IgnoresOn, flag ForcesOn, flag ReadAll, flag NoiseOn, byte DrvStartFBKCheck);
    void           Options(char* pGotoTag = NULL);
    void           CloseOptions();
    long           Open();
    long           Close(BOOL SaveState = TRUE);

    long           DoOpenAttach();
    long           DoCloseDetach();

    DWORD                 m_dwMarshalCookie;
    IScdMarshalWrap       m_Marshal;
    CScdMarshalEvents    *m_pMarshalEvents;

    DWORD                 m_dwExecObjectCookie;
    IScdExecObjectWrap    m_ExecObject;
    CScdExecObjectEvents *m_pExecObjectEvents;

    Strng                 m_FileName;
    Strng                 m_Node;
    
    DWORD                 m_dwMaxItemCount, m_dwOldMaxItemCount;
    DWORD                 m_dwItemCount;
    SCD_TAGITEM          *m_pTags,    *m_pOldTags;   
    SCD_DATAITEM         *m_pDatas,   *m_pOldDatas;  
    DWORD                *m_pDWords,  *m_pOldDWords; 
    //

    void AllocItemMemory(DWORD N);
    void MarkItemMemory();
    void FixItemMemory();

    //DWORD                 m_dwMaxDataCount;
    //DWORD                 m_dwDataCount;
    //SCD_DATAITEM         *m_pDataItems;

    CArray <CIOMarshalItem*, CIOMarshalItem*> m_Items;

  public:
    CIOMarshal();
    ~CIOMarshal();
    

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
    virtual flag   EO_EndConnect(FilingControlBlock &FCB);
    virtual flag   EO_EndLoad(FilingControlBlock &FCB);
    virtual int    EO_QueryChangeTag(pchar pOldTag, pchar pNewTag);
    virtual int    EO_ChangeTag(pchar pOldTag, pchar pNewTag);
    virtual int    EO_ChangeTagDone(pchar pOldTag, pchar pNewTag);
    virtual int    EO_QueryDeleteTags(Strng_List & Tags);
    virtual int    EO_DeleteTags(Strng_List & Tags);
    virtual int    EO_DeleteTagsDone(Strng_List & Tags);
    virtual void   EO_OnAppActivate(BOOL bActive);
    virtual flag   EO_RequestTagInfo(RequestTagInfoRec& Rqst, ReplyTagInfoRec& Info);
  };

// ========================================================================
//
//
//
// ========================================================================

extern CIOMarshal * gs_pIOMarshal; // one and only driver manager EO

// ========================================================================
//
//
//
// ========================================================================

#endif

