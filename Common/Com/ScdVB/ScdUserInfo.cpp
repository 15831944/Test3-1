// ScdUserInfo.cpp : Implementation of CScdUserInfo
#include "stdafx.h"
#include "ScdVB.h"
#include "ScdUserInfo.h"
#include "lmcons.h"
#include "comdef.h"

/////////////////////////////////////////////////////////////////////////////
// CScdUserInfo

STDMETHODIMP CScdUserInfo::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IScdUserInfo
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

STDMETHODIMP CScdUserInfo::get_IsTrainee(BOOL *pVal)
{
	try
	{
		*pVal = m_eUserType >=ScdUser_Trainee;
		return S_OK;
	}
	catch(...)
	{
		return E_FAIL;
	}
}

STDMETHODIMP CScdUserInfo::get_IsEngineer(BOOL *pVal)
{
	try
	{
		*pVal = m_eUserType >=ScdUser_Engineer;
		return S_OK;
	}
	catch(...)
	{
		return E_FAIL;
	}
}

STDMETHODIMP CScdUserInfo::get_IsInstructor(BOOL *pVal)
{
	try
	{
		*pVal = m_eUserType >=ScdUser_Instructor;
		return S_OK;
	}
	catch(...)
	{
		return E_FAIL;
	}
}

STDMETHODIMP CScdUserInfo::get_IsSystemManager(BOOL *pVal)
{
	try
	{
		*pVal = m_eUserType >=ScdUser_SystemManager;
		return S_OK;
	}
	catch(...)
	{
		return E_FAIL;
	}
}

STDMETHODIMP CScdUserInfo::get_UserName(BSTR *pVal)
{
	try
	{
  char Buff[UNLEN+1];
  DWORD Len=sizeof(Buff);
  if (::GetUserName(Buff, &Len))
    {
    _bstr_t s(Buff);
    *pVal=s.copy();
    return S_OK;
    }
  else
    return E_FAIL;

	}
	catch(...)
	{
		return E_FAIL;
	}
}

STDMETHODIMP CScdUserInfo::get_UserProfile(BSTR *pVal)
{
	try
	{
  char Buff1[1024], Buff2[UNLEN+1];
  DWORD Len1=sizeof(Buff1);
  DWORD Len2=sizeof(Buff2);
  if (1) //::GetProfilesDirectory(Buff1, &Len1))
    {
    // temp
    strcpy(Buff1, "C:\\Documents and Settings");
    //
    _bstr_t s(Buff1);
    s+="\\";
    if (::GetUserName(Buff2, &Len2))
      {
      s+=Buff2;
      *pVal=s.copy();
      return S_OK;
      }
    }
  return E_FAIL;

	}
	catch(...)
	{
		return E_FAIL;
	}
}

STDMETHODIMP CScdUserInfo::get_AllUsersProfile(BSTR *pVal)
{
	try
	{
  char Buff1[1024];
  DWORD Len1=sizeof(Buff1);
  if (1) //::GetAllUsersProfileDirectory(Buff1, &Len1))
    {
    // temp
    strcpy(Buff1, "C:\\Documents and Settings\\All Users");
    //
    _bstr_t s(Buff1);
    *pVal=s.copy();
    return S_OK;
    }
  return E_FAIL;

	}
	catch(...)
	{
		return E_FAIL;
	}
}

STDMETHODIMP CScdUserInfo::get_UserType(ScdUserType *pVal)
{
	try
	{
    *pVal=m_eUserType;
    return S_OK;
	}
	catch(...)
	{
		return E_FAIL;
	}
}

STDMETHODIMP CScdUserInfo::put_UserType(ScdUserType newVal)
{
	try
	{
    m_eUserType = newVal;
    return S_OK;
	}
	catch(...)
	{
		return E_FAIL;
	}
}
