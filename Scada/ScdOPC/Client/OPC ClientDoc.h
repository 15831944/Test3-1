//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  OPC ClientDoc.h
//  $Author    :  Jim Hansen
//
//  Description:  The OPC actions are mostly handled by this Doc.
//                class Item represents items in a group
//                class CAdviseSink is the interface that gets advise callbacks.
//
//**************************************************************************
#include "ConnectionPoint.h"

class Item : public CObject
{
public:
   Item() : quality(0), hServerHandle(0) {;}
   CString      name;
   COleVariant  value;
   WORD         quality;
   FILETIME     timestamp;
   OPCHANDLE    hServerHandle;
};

typedef CTypedPtrList<CObList, Item*> ItemList;

//**************************************************************************
class CAdviseSink : public IAdviseSink
{
public:
   CAdviseSink();
   ~CAdviseSink(void);

   STDMETHODIMP QueryInterface(REFIID, void**);
   STDMETHODIMP_(ULONG) AddRef(void);
   STDMETHODIMP_(ULONG) Release(void);

   STDMETHODIMP_(void)  OnDataChange(LPFORMATETC, LPSTGMEDIUM);
   STDMETHODIMP_(void)  OnViewChange(DWORD, LONG);
   STDMETHODIMP_(void)  OnRename(LPMONIKER);
   STDMETHODIMP_(void)  OnSave(void);
   STDMETHODIMP_(void)  OnClose(void);

   STDMETHODIMP_(void)  OnWriteComplete(LPFORMATETC, LPSTGMEDIUM);
protected:
   ULONG               m_cRef;
};

//**************************************************************************
class OPCClientDoc : public CDocument
{
protected: // create from serialization only
   OPCClientDoc();
   DECLARE_DYNCREATE(OPCClientDoc)

// Attributes
public:

// Operations
public:
   ItemList items;
   void     Disconnect() {OnOpcDisconnect();}
   void     AddItem(LPCTSTR itemID, LPCTSTR accessPath, VARTYPE type);
   CString  GetErrorString(LPCTSTR description, HRESULT error);
   void     ReportError(LPCTSTR description, HRESULT error);

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(OPCClientDoc)
   public:
   virtual BOOL OnNewDocument();
   virtual void Serialize(CArchive& ar);
   //}}AFX_VIRTUAL

// Implementation
public:
   virtual ~OPCClientDoc();
#ifdef _DEBUG
   virtual void AssertValid() const;
   virtual void Dump(CDumpContext& dc) const;
#endif

    Item*           pCurrentItem;   // selected item in view
    HWND            hView;          // for updates
    OPCServer       opcServer;      // the connected server
    OPCGroupStateMgt opcGroup;      // the single group on this server

protected:
    // OPC 1.0 IDataObject
    DWORD           dwConnection1;
    DWORD           dwConnection2;
    OPCHANDLE       groupHandle;
    CAdviseSink*    testSink;

    // OPC 2.0 ConnectionPoint
    OPCCallback*    callbackCP;
    OPCShutdown*    shutdownCP;
    DWORD           dwShutdownConnection;
    BOOL            usingCP;

    DWORD           transactionID;  // only used for asynchronous I/O
    CString         lastServer;
    CString         lastNode;

// Generated message map functions
protected:
   //{{AFX_MSG(OPCClientDoc)
   afx_msg void OnOpcConnect();
   afx_msg void OnUpdateOpcConnect(CCmdUI* pCmdUI);
   afx_msg void OnOpcDisconnect();
   afx_msg void OnUpdateOpcDisconnect(CCmdUI* pCmdUI);
   afx_msg void OnOpcServerstatus();
   afx_msg void OnUpdateOpcServerstatus(CCmdUI* pCmdUI);
   afx_msg void OnOpcGroupparameters();
   afx_msg void OnUpdateOpcGroupparameters(CCmdUI* pCmdUI);
   afx_msg void OnOpcAdditem();
   afx_msg void OnUpdateOpcAdditem(CCmdUI* pCmdUI);
   afx_msg void OnOpcItemProperties();
   afx_msg void OnUpdateOpcItemProperties(CCmdUI* pCmdUI);
   afx_msg void OnOpcItemattributes();
   afx_msg void OnUpdateOpcItemattributes(CCmdUI* pCmdUI);
   afx_msg void OnOpcWritevaluetoitem();
   afx_msg void OnUpdateOpcWritevaluetoitem(CCmdUI* pCmdUI);
   afx_msg void OnOpcReaditem();
   afx_msg void OnUpdateOpcReaditem(CCmdUI* pCmdUI);
   afx_msg void OnOpcRefresh();
   afx_msg void OnUpdateOpcRefresh(CCmdUI* pCmdUI);
   afx_msg void OnOpcRemoveitem();
   afx_msg void OnUpdateOpcRemoveitem(CCmdUI* pCmdUI);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
