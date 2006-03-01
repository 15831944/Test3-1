// AdviseSink.h: interface for the AdviseSink class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ADVISESINK_H__28CB1995_D9A1_4E4C_8A0E_54AB8A554AB9__INCLUDED_)
#define AFX_ADVISESINK_H__28CB1995_D9A1_4E4C_8A0E_54AB8A554AB9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class COPC_AdviseSink :
  public IAdviseSink
  {
public:
  COPC_AdviseSink(void);
  ~COPC_AdviseSink(void);

  STDMETHODIMP  QueryInterface(REFIID, void **);
  STDMETHODIMP_(ULONG)
  AddRef(void);
  STDMETHODIMP_(ULONG)
  Release(void);

  STDMETHODIMP_ (void)  OnDataChange(LPFORMATETC, LPSTGMEDIUM);
  STDMETHODIMP_ (void)  OnViewChange(DWORD, LONG);
  STDMETHODIMP_ (void)  OnRename(LPMONIKER);
  STDMETHODIMP_ (void)  OnSave(void);
  STDMETHODIMP_ (void)  OnClose(void);

  STDMETHODIMP_ (void)  OnWriteComplete(LPFORMATETC, LPSTGMEDIUM);
protected:
  ULONG m_cRef;
  };

#endif // !defined(AFX_ADVISESINK_H__28CB1995_D9A1_4E4C_8A0E_54AB8A554AB9__INCLUDED_)
