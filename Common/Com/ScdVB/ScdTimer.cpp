// ScdTimer.cpp : Implementation of CScdTimer
#include "stdafx.h"
#include "ScdVB.h"
#include "ScdTimer.h"

/////////////////////////////////////////////////////////////////////////////
// CScdTimer

STDMETHODIMP CScdTimer::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IScdTimer
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

STDMETHODIMP CScdTimer::Start()
{
	m_bActive = true;
	return S_OK;
}

STDMETHODIMP CScdTimer::Stop()
{
	m_bActive = false;
	return S_OK;
}

STDMETHODIMP CScdTimer::get_Interval(long *pVal)
{
	try
	{
		*pVal = m_lInterval;
		return S_OK;
	}
	catch(...)
	{
		return E_FAIL;
	}
}

STDMETHODIMP CScdTimer::put_Interval(long newVal)
{
	try
	{
		m_lInterval = (newVal>=MIN_TIMEOUT)?newVal:MIN_TIMEOUT;
		return S_OK;
	}
	catch(...)
	{
		return E_FAIL;
	}
}

HRESULT CScdTimer::FinalConstruct()
{
	try
	{
		m_hTerminate = ::CreateEvent( 0 , TRUE , FALSE , 0 );
		if ( NULL == m_hTerminate )
			throw E_FAILED_TO_CREATE_EVENT;

		m_hThreadHandle = ::CreateThread( 0 , 0 , StaticThreadProc , this , 0 , &m_dwThreadID );
		if ( NULL == m_hThreadHandle ) 
			throw E_UNABLE_TO_CREATE_WORKER_THREAD;

		return S_OK;
	}
	catch( unsigned int iError )
	{
		return AtlReportError( CLSID_ScdTimer , iError , IID_IScdTimer , E_FAIL , 0);
	}
	catch(...)
	{
		return AtlReportError( CLSID_ScdTimer , E_GENERAL_ERROR , IID_IScdTimer , E_FAIL , 0);
	}
}

void CScdTimer::FinalRelease()
{
	::SetEvent( m_hTerminate );
	if ( WAIT_TIMEOUT == ::WaitForSingleObject( m_hThreadHandle , 5000 ) )
		::TerminateThread( m_hThreadHandle , 0 );
}

DWORD CScdTimer::ThreadProc()
{
	try
	{
		while( WAIT_TIMEOUT == ::WaitForSingleObject( m_hTerminate , m_lInterval) )
		{
			if ( m_bActive )
				Fire_Expired();
		}
	return 0;
	}
	catch(...)
	{
		return -1;
	}
}

DWORD WINAPI CScdTimer::StaticThreadProc( LPVOID lpvoid)
{
	return static_cast<CScdTimer*>(lpvoid)->ThreadProc();
}
