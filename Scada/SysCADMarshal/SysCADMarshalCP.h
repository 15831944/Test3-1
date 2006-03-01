#ifndef _SYSCADMARSHALCP_H_
#define _SYSCADMARSHALCP_H_







template <class T>
class CProxy_IScdMarshalEvents : public IConnectionPointImpl<T, &IID__IScdMarshalEvents, CComDynamicUnkArray>
{
	//Warning this class may be recreated by the wizard.
public:
};







          




template <class T>
class CProxy_IScdExecObjectEvents : public IConnectionPointImpl<T, &IID__IScdExecObjectEvents, CComDynamicUnkArray>
{
	//Warning this class may be recreated by the wizard.
public:
	HRESULT Fire_ReadTaggedItems(ULONG dwCount, tagSCD_TAGITEM * * pTags, tagSCD_DATAITEM * * pData, ULONG * * pReturn)
	{
		HRESULT ret;
		T* pT = static_cast<T*>(this);
		int nConnectionIndex;
		int nConnections = m_vec.GetSize();
		
		for (nConnectionIndex = 0; nConnectionIndex < nConnections; nConnectionIndex++)
		{
			pT->Lock();
			CComPtr<IUnknown> sp = m_vec.GetAt(nConnectionIndex);
			pT->Unlock();
			_IScdExecObjectEvents* p_IScdExecObjectEvents = reinterpret_cast<_IScdExecObjectEvents*>(sp.p);
			if (p_IScdExecObjectEvents != NULL)
				ret = p_IScdExecObjectEvents->ReadTaggedItems(dwCount, pTags, pData, pReturn);
		}	return ret;
	
	}
};
#endif