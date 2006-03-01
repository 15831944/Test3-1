#ifndef _SCDVBCP_H_
#define _SCDVBCP_H_

template <class T>
class CProxy_IScdUserInfoEvents : public IConnectionPointImpl<T, &DIID__ScdUserInfoEvents, CComDynamicUnkArray>
{
	//Warning this class may be recreated by the wizard.
public:
};



template <class T>
class CProxy_IScdTimerEvents : public IConnectionPointImpl<T, &DIID__ScdTimerEvents, CComDynamicUnkArray>
{
	//Warning this class may be recreated by the wizard.
public:
	HRESULT Fire_Expired()
	{
		CComVariant varResult;
		T* pT = static_cast<T*>(this);
		int nConnectionIndex;
		int nConnections = m_vec.GetSize();
		
		for (nConnectionIndex = 0; nConnectionIndex < nConnections; nConnectionIndex++)
		{
			pT->Lock();
			CComPtr<IUnknown> sp = m_vec.GetAt(nConnectionIndex);
			pT->Unlock();
			IDispatch* pDispatch = reinterpret_cast<IDispatch*>(sp.p);
			if (pDispatch != NULL)
			{
				VariantClear(&varResult);
				DISPPARAMS disp = { NULL, NULL, 0, 0 };
				pDispatch->Invoke(0x1, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &disp, &varResult, NULL, NULL);
			}
		}
		return varResult.scode;
	
	}
};

template <class T>
class CProxy_IScdRegistryEvents : public IConnectionPointImpl<T, &DIID__ScdRegistryEvents, CComDynamicUnkArray>
{
	//Warning this class may be recreated by the wizard.
public:
};


template <class T>
class CProxy_IScdFileSystemEvents : public IConnectionPointImpl<T, &DIID__ScdFileSystemEvents, CComDynamicUnkArray>
{
	//Warning this class may be recreated by the wizard.
public:
};

template <class T>
class CProxy_IScdEventRelayEvents : public IConnectionPointImpl<T, &DIID__ScdEventRelayEvents, CComDynamicUnkArray>
{
	//Warning this class may be recreated by the wizard.
public:
	HRESULT Fire_YourNotifier(LONG i_lCookie)
	{
		CComVariant varResult;
		T* pT = static_cast<T*>(this);
		int nConnectionIndex;
		CComVariant* pvars = new CComVariant[1];
		int nConnections = m_vec.GetSize();
		
		for (nConnectionIndex = 0; nConnectionIndex < nConnections; nConnectionIndex++)
		{
			pT->Lock();
			CComPtr<IUnknown> sp = m_vec.GetAt(nConnectionIndex);
			pT->Unlock();
			IDispatch* pDispatch = reinterpret_cast<IDispatch*>(sp.p);
			if (pDispatch != NULL)
			{
				VariantClear(&varResult);
				pvars[0] = i_lCookie;
				DISPPARAMS disp = { pvars, NULL, 1, 0 };
				pDispatch->Invoke(0x1, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &disp, &varResult, NULL, NULL);
			}
		}
		delete[] pvars;
		return varResult.scode;
	
	}
};
#endif