// ScdEventRelay.cpp : Implementation of CScdEventRelay
#include "stdafx.h"
#include "ScdVB.h"
#include "ScdEventRelay.h"

/////////////////////////////////////////////////////////////////////////////
// CScdEventRelay

STDMETHODIMP CScdEventRelay::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IScdEventRelay
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

STDMETHODIMP CScdEventRelay::NotifyMe(long i_lCookie)
{
  try
    {
    m_qCookies.push( i_lCookie );
    ::SetEvent( m_evNotify );
    return S_OK;
    }
  catch(...)
    {
    return E_FAIL;
    }
}

DWORD CScdEventRelay::ThreadProc()
{
  try
    {
    HANDLE  ah[] = { m_evNotify , m_evStop };
    while( WAIT_OBJECT_0 + 1 != ::WaitForMultipleObjects( sizeof(ah)/sizeof(HANDLE) , ah , FALSE , INFINITE ) )
      {
      while( ! m_qCookies.empty() )
        {
          long lCookie = m_qCookies.back();
          m_qCookies.pop();
          Fire_YourNotifier( lCookie );
        }
      }
      return 0;
    }
  catch(...)
    {
    return -1;
    }
}

DWORD WINAPI CScdEventRelay::StaticThreadProc( LPVOID lpv)
  {
  return static_cast<CScdEventRelay*>( lpv)->ThreadProc();
  }

HRESULT CScdEventRelay::FinalConstruct()
{
  try
    {
    m_evStop = ::CreateEvent( 0 , FALSE , FALSE , 0 );
    m_evNotify = ::CreateEvent( 0 , FALSE , FALSE , 0 );

    m_hThreadProc = ::CreateThread( 0 , 0 , StaticThreadProc , this , 0 , 0 );
    return S_OK;
    }
  catch(...)
    {
    return E_FAIL;
    }
}

void CScdEventRelay::FinalRelease()
{
  try
    {
    ::SetEvent( m_evStop );
    if ( WAIT_TIMEOUT == ::WaitForSingleObject( m_hThreadProc , 1000 ) )
      {
      TerminateThread( m_hThreadProc , -1 );
      }
    ::CloseHandle( m_evNotify );
    ::CloseHandle( m_evStop);
    ::CloseHandle( m_hThreadProc );

    }
  catch(...)
    {
    }
}
