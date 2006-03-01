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

#include "OPCStuff.h"

#include "ConnectionPoint.h"

class COpcC_Item : public CObject
{
public:
   COpcC_Item() : quality(0), hServerHandle(0) {;}
   CString      name;
   COleVariant  value;
   WORD         quality;
   FILETIME     timestamp;
   OPCHANDLE    hServerHandle;
};

typedef CTypedPtrList<CObList, COpcC_Item*> ItemList;

//**************************************************************************
class COpcC_AdviseSink : public IAdviseSink
{
public:
   COpcC_AdviseSink();
   ~COpcC_AdviseSink(void);

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
class COpcC_Client //: public CDocument
{
protected: // create from serialization only
   COpcC_Client();

// Attributes
public:

// Operations
public:
   ItemList items;
   //void     Disconnect() {OnOpcDisconnect();}
   void     Disconnect() {DoOpcDisconnect();}
   void     AddItem(LPCTSTR itemID, LPCTSTR accessPath, VARTYPE type);
   CString  GetErrorString(LPCTSTR description, HRESULT error);
   void     ReportError(LPCTSTR description, HRESULT error);

// Implementation
public:
   virtual ~COpcC_Client();

    OPCServer       opcServer;      // the connected server
    OPCGroupStateMgt opcGroup;      // the single group on this server

protected:
    // OPC 1.0 IDataObject
    DWORD           dwConnection1;
    DWORD           dwConnection2;
    OPCHANDLE       groupHandle;
    COpcC_AdviseSink* testSink;

    // OPC 2.0 ConnectionPoint
    COpcC_Callback* callbackCP;
    COpcC_Shutdown* shutdownCP;
    DWORD           dwShutdownConnection;
    BOOL            usingCP;

    DWORD           transactionID;  // only used for asynchronous I/O
    CString         lastServer;
    CString         lastNode;

    void            DoOpcDisconnect();

};

/////////////////////////////////////////////////////////////////////////////
