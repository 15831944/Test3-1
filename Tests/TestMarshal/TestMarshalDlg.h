// TestMarshalDlg.h : header file
//

#if !defined(AFX_TESTMARSHALDLG_H__D866250D_2420_4B45_86C2_FA21D2F078A0__INCLUDED_)
#define AFX_TESTMARSHALDLG_H__D866250D_2420_4B45_86C2_FA21D2F078A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WITHSCD 1
#if WITHSCD 
#include "..\..\SysCADMarshal\SysCADMarshal_i.h"
#else
#include "D:\Temp\XXYY1\XXYY1_i.h"
#endif

class CTestMarshalDlgAutoProxy;

//*******************************************************************
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

//*******************************************************************
//  Server Interfaces
//*******************************************************************
#if WITHSCD 
//typedef ScdInterfaceWrap<IScdXObject, &IID_IScdXObject> IScdXObjectWrap;
typedef ScdInterfaceWrap<ISysCADMarshal, &DIID_ISysCADMarshal> ISysCADMarshalWrap;
typedef ScdInterfaceWrap<IScdMarshal, &IID_IScdMarshal> IScdMarshalWrap;
typedef ScdInterfaceWrap<IScdExecObject, &IID_IScdExecObject> IScdExecObjectWrap;
#else
typedef ScdInterfaceWrap<IXXXYYY1, &IID_IXXXYYY1> IXXXYYY1Wrap;
#endif




//class OPCServer :
//  public IOPCServerWrap
//  {
//public:
//  OPCServer(void) { }
//  HRESULT AddGroup
//          (
//            LPCWSTR   szName,
//            BOOL      bActive,
//            DWORD     dwRequestedUpdateRate,
//            OPCHANDLE hClientGroup,
//            LONG      *pTimeBias,
//            FLOAT     *pPercentDeadband,
//            DWORD     dwLCID,
//            OPCHANDLE *phServerGroup,
//            DWORD     *pRevisedUpdateRate,
//            REFIID    riid,
//            LPUNKNOWN *ppUnk
//          )
//    { //ASSERT( IsOk() );
//    if(!IsOk()) return E_FAIL;
//
//    HRESULT hr = m_pI->AddGroup
//      (
//        szName,
//        bActive,
//        dwRequestedUpdateRate,
//        hClientGroup,
//        pTimeBias,
//        pPercentDeadband,
//        dwLCID,
//        phServerGroup,
//        pRevisedUpdateRate,
//        riid,
//        ppUnk
//      );
//    if(FAILED(hr)) TRACE_MSG(_T("IOPCServer::AddGroup"), hr);
//    return hr;
//    }
//
//  HRESULT GetErrorString(HRESULT dwError, LCID dwLocale, LPWSTR *ppString)
//    { //ASSERT( IsOk() );
//    HRESULT hr = m_pI->GetErrorString(dwError, dwLocale, ppString);
//    if(FAILED(hr)) TRACE_MSG(_T("IOPCServer::GetErrorString"), hr);
//    return hr;
//    }
//
//  HRESULT GetGroupByName(LPCWSTR szName, REFIID riid, LPUNKNOWN *ppUnk)
//    { //ASSERT( IsOk() );
//    HRESULT hr = m_pI->GetGroupByName(szName, riid, ppUnk);
//    if(FAILED(hr)) TRACE_MSG(_T("IOPCServer::GetGroupByName"), hr);
//    return hr;
//    }
//
//  HRESULT GetStatus(OPCSERVERSTATUS **ppServerStatus)
//    { //ASSERT( IsOk() );
//    HRESULT hr = m_pI->GetStatus(ppServerStatus);
//    if(FAILED(hr)) TRACE_MSG(_T("IOPCServer::GetStatus"), hr);
//    return hr;
//    }
//
//  HRESULT RemoveGroup(OPCHANDLE hServerGroup, BOOL bForce)
//    { //ASSERT( IsOk() );
//    if(!IsOk()) return E_FAIL;
//
//    HRESULT hr = m_pI->RemoveGroup(hServerGroup, bForce);
//    if(FAILED(hr)) TRACE_MSG(_T("IOPCServer::RemoveGroup"), hr);
//    return hr;
//    }
//
//  HRESULT CreateGroupEnumerator(OPCENUMSCOPE dwScope, REFIID riid, LPUNKNOWN *ppUnk)
//    { //ASSERT( IsOk() );
//    HRESULT hr = m_pI->CreateGroupEnumerator(dwScope, riid, ppUnk);
//    if(FAILED(hr)) TRACE_MSG(_T("IOPCServer::CreateGroupEnumerator"), hr);
//    return hr;
//    }
//  };






//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  ConnectionPoint.h
//  $Author    :  Jim Hansen
//
//  Description:  Callback class for OPC Data Access 2.0
//
//
//**************************************************************************
//extern CComModule _Module;
//#include <atlcom.h>
//
//class OPCCallback :
//   public IOPCDataCallback,
//   public CComObjectRootEx<CComMultiThreadModel>
//{
//public:
//   OPCCallback() {;}
//   virtual  ~OPCCallback(){;}
//
//BEGIN_COM_MAP(OPCCallback)
//   COM_INTERFACE_ENTRY(IOPCDataCallback)
//END_COM_MAP()
//
//DECLARE_NOT_AGGREGATABLE(OPCCallback)
//
//   // The OPC Callback functions
//   STDMETHODIMP OnDataChange(
//    DWORD       Transid,
//    OPCHANDLE   grphandle,
//    HRESULT     masterquality,
//    HRESULT     mastererror,
//    DWORD       count,
//    OPCHANDLE * clienthandles,
//    VARIANT   * values,
//    WORD      * quality,
//    FILETIME  * time,
//    HRESULT   * errors);
//
//   STDMETHODIMP OnReadComplete(
//    DWORD       Transid,
//    OPCHANDLE   grphandle,
//    HRESULT     masterquality,
//    HRESULT     mastererror,
//    DWORD       count,
//    OPCHANDLE * clienthandles,
//    VARIANT   * values,
//    WORD      * quality,
//    FILETIME  * time,
//    HRESULT   * errors);
//
//   STDMETHODIMP OnWriteComplete(
//    DWORD       Transid,
//    OPCHANDLE   grphandle,
//    HRESULT     mastererr,
//    DWORD       count,
//    OPCHANDLE * clienthandles,
//    HRESULT   * errors);
//
//   STDMETHODIMP OnCancelComplete(
//    DWORD       transid,
//    OPCHANDLE   grphandle);
//};
//
//typedef CComObject<OPCCallback> OPCCallbackObject;
//
////**************************************************************************
//class CScd :
//   public IOPCShutdown,
//   public CComObjectRootEx<CComMultiThreadModel>
//{
//public:
//   OPCShutdown() {;}
//   virtual  ~OPCShutdown(){;}
//
//BEGIN_COM_MAP(OPCShutdown)
//   COM_INTERFACE_ENTRY(IOPCShutdown)
//END_COM_MAP()
//
//DECLARE_NOT_AGGREGATABLE(OPCShutdown)
//
//   // The OPC Callback function
//   STDMETHODIMP ShutdownRequest(LPCWSTR szReason);
//
//   CString reason;
//};
//
//typedef CComObject<OPCShutdown> OPCShutdownObject;
//
//
//
//
//

class CScdMarshalEvents :
   public _IScdMarshalEvents,
   public CComObjectRootEx<CComMultiThreadModel>
{
public:
   CScdMarshalEvents() {;}
   virtual  ~CScdMarshalEvents(){;}

BEGIN_COM_MAP(CScdMarshalEvents)
   COM_INTERFACE_ENTRY(_IScdMarshalEvents)
END_COM_MAP()

DECLARE_NOT_AGGREGATABLE(CScdMarshalEvents)

     };
typedef CComObject<CScdMarshalEvents> CScdMarshalEventsObject;

class CScdExecObjectEvents :
   public _IScdExecObjectEvents,
   public CComObjectRootEx<CComMultiThreadModel>
{
public:
   CScdExecObjectEvents() {;}
   virtual  ~CScdExecObjectEvents(){;}

BEGIN_COM_MAP(CScdExecObjectEvents)
   COM_INTERFACE_ENTRY(_IScdExecObjectEvents)
END_COM_MAP()

DECLARE_NOT_AGGREGATABLE(CScdExecObjectEvents)

     };
typedef CComObject<CScdExecObjectEvents> CScdExecObjectEventsObject;


/////////////////////////////////////////////////////////////////////////////
// CTestMarshalDlg dialog

class CTestMarshalDlg : public CDialog
{
	DECLARE_DYNAMIC(CTestMarshalDlg);
	friend class CTestMarshalDlgAutoProxy;

// Construction
public:
	CTestMarshalDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CTestMarshalDlg();

// Dialog Data
	//{{AFX_DATA(CTestMarshalDlg)
	enum { IDD = IDD_TESTMARSHAL_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestMarshalDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CTestMarshalDlgAutoProxy* m_pAutoProxy;
	HICON m_hIcon;

#if WITHSCD 
  IScdMarshalWrap m_Marshal;
  IScdExecObjectWrap m_ExecObject;
#else
  IXXXYYY1Wrap m_XXXYYY;
#endif

	BOOL CanExit();

	// Generated message map functions
	//{{AFX_MSG(CTestMarshalDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnConnect();
	afx_msg void OnDisconnect();
	afx_msg void OnQuerytime();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTMARSHALDLG_H__D866250D_2420_4B45_86C2_FA21D2F078A0__INCLUDED_)
