// ScdFileSystem.cpp : Implementation of CScdFileSystem
#include "stdafx.h"
#include "ScdVB.h"
#include "ScdFileSystem.h"
#include "comdef.h"

/////////////////////////////////////////////////////////////////////////////
// CScdFileSystem

STDMETHODIMP CScdFileSystem::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IScdFileSystem
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

STDMETHODIMP CScdFileSystem::get_FindFirstFile(BSTR Mask, BSTR *pFileName, ScdFileAttributes *pAttr, long *pVal)
{
	try
	{
    if (m_hFileFind!=INVALID_HANDLE_VALUE)
      ::FindClose(m_hFileFind);
    m_hFileFind=::FindFirstFile(_bstr_t(Mask), &m_FindData);
    if (m_hFileFind==INVALID_HANDLE_VALUE)
      {
      *pVal=-1;
      *pFileName=NULL;
      *pAttr=ScdFile_NONE;
      }
    else
      {
      *pVal=0;
      _bstr_t s(m_FindData.cFileName);
      *pFileName=s.copy();
      *pAttr=ScdFileAttributes(m_FindData.dwFileAttributes);
      }
    return S_OK;
	}
	catch(...)
	{
		return E_FAIL;
	}
}

STDMETHODIMP CScdFileSystem::get_FindNextFile(BSTR *pFileName, ScdFileAttributes *pAttr, long *pVal)
{
	try
	{
    if (m_hFileFind!=INVALID_HANDLE_VALUE)
      if (!FindNextFile(m_hFileFind, &m_FindData))
        {
        ::FindClose(m_hFileFind);
        m_hFileFind=INVALID_HANDLE_VALUE;
        }
    if (m_hFileFind==INVALID_HANDLE_VALUE)
      {
      *pVal=-1;
      *pFileName=NULL;
      *pAttr=ScdFile_NONE;
      }
    else
      {
      *pVal=0;
      _bstr_t s(m_FindData.cFileName);
      *pFileName=s.copy();
      *pAttr=ScdFileAttributes(m_FindData.dwFileAttributes);
      }
    return S_OK;
	}
	catch(...)
	{
		return E_FAIL;
	}
}
