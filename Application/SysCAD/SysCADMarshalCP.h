#ifndef _SYSCADMARSHALCP_H_
#define _SYSCADMARSHALCP_H_

#import "D:\SysCAD82\_Src\SysCADMarshal\SysCADMarshal.tlb" raw_interfaces_only, raw_native_types, no_namespace, named_guids	//"Import typelib"
template <class T>
class CProxy_IScdExecObjectEvents : public IConnectionPointImpl<T, &IID__IScdExecObjectEvents, CComDynamicUnkArray>
{
	//Warning this class may be recreated by the wizard.
public:
	HRESULT Fire_ReadTaggedItem(tagSCD_TAGITEM * Tag, tagSCD_DATAITEM * Data, ULONG * Return)
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
				ret = p_IScdExecObjectEvents->ReadTaggedItem(Tag, Data, Return);
		}	return ret;
	
	}
};
#endif