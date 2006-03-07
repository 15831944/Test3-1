

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0361 */
/* at Tue Mar 07 15:29:48 2006
 */
/* Compiler settings for .\ScdVB.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __ScdVB_h__
#define __ScdVB_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IScdUserInfo_FWD_DEFINED__
#define __IScdUserInfo_FWD_DEFINED__
typedef interface IScdUserInfo IScdUserInfo;
#endif 	/* __IScdUserInfo_FWD_DEFINED__ */


#ifndef __IScdProfile_FWD_DEFINED__
#define __IScdProfile_FWD_DEFINED__
typedef interface IScdProfile IScdProfile;
#endif 	/* __IScdProfile_FWD_DEFINED__ */


#ifndef ___ScdUserInfoEvents_FWD_DEFINED__
#define ___ScdUserInfoEvents_FWD_DEFINED__
typedef interface _ScdUserInfoEvents _ScdUserInfoEvents;
#endif 	/* ___ScdUserInfoEvents_FWD_DEFINED__ */


#ifndef __IScdTimer_FWD_DEFINED__
#define __IScdTimer_FWD_DEFINED__
typedef interface IScdTimer IScdTimer;
#endif 	/* __IScdTimer_FWD_DEFINED__ */


#ifndef __ScdUserInfo_FWD_DEFINED__
#define __ScdUserInfo_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdUserInfo ScdUserInfo;
#else
typedef struct ScdUserInfo ScdUserInfo;
#endif /* __cplusplus */

#endif 	/* __ScdUserInfo_FWD_DEFINED__ */


#ifndef ___ScdTimerEvents_FWD_DEFINED__
#define ___ScdTimerEvents_FWD_DEFINED__
typedef interface _ScdTimerEvents _ScdTimerEvents;
#endif 	/* ___ScdTimerEvents_FWD_DEFINED__ */


#ifndef __IScdRegistry_FWD_DEFINED__
#define __IScdRegistry_FWD_DEFINED__
typedef interface IScdRegistry IScdRegistry;
#endif 	/* __IScdRegistry_FWD_DEFINED__ */


#ifndef __ScdTimer_FWD_DEFINED__
#define __ScdTimer_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdTimer ScdTimer;
#else
typedef struct ScdTimer ScdTimer;
#endif /* __cplusplus */

#endif 	/* __ScdTimer_FWD_DEFINED__ */


#ifndef ___ScdRegistryEvents_FWD_DEFINED__
#define ___ScdRegistryEvents_FWD_DEFINED__
typedef interface _ScdRegistryEvents _ScdRegistryEvents;
#endif 	/* ___ScdRegistryEvents_FWD_DEFINED__ */


#ifndef __IScdFileSystem_FWD_DEFINED__
#define __IScdFileSystem_FWD_DEFINED__
typedef interface IScdFileSystem IScdFileSystem;
#endif 	/* __IScdFileSystem_FWD_DEFINED__ */


#ifndef __ScdRegistry_FWD_DEFINED__
#define __ScdRegistry_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdRegistry ScdRegistry;
#else
typedef struct ScdRegistry ScdRegistry;
#endif /* __cplusplus */

#endif 	/* __ScdRegistry_FWD_DEFINED__ */


#ifndef ___ScdFileSystemEvents_FWD_DEFINED__
#define ___ScdFileSystemEvents_FWD_DEFINED__
typedef interface _ScdFileSystemEvents _ScdFileSystemEvents;
#endif 	/* ___ScdFileSystemEvents_FWD_DEFINED__ */


#ifndef __IScdEventRelay_FWD_DEFINED__
#define __IScdEventRelay_FWD_DEFINED__
typedef interface IScdEventRelay IScdEventRelay;
#endif 	/* __IScdEventRelay_FWD_DEFINED__ */


#ifndef __ScdFileSystem_FWD_DEFINED__
#define __ScdFileSystem_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdFileSystem ScdFileSystem;
#else
typedef struct ScdFileSystem ScdFileSystem;
#endif /* __cplusplus */

#endif 	/* __ScdFileSystem_FWD_DEFINED__ */


#ifndef ___ScdEventRelayEvents_FWD_DEFINED__
#define ___ScdEventRelayEvents_FWD_DEFINED__
typedef interface _ScdEventRelayEvents _ScdEventRelayEvents;
#endif 	/* ___ScdEventRelayEvents_FWD_DEFINED__ */


#ifndef __IScdStopWatch_FWD_DEFINED__
#define __IScdStopWatch_FWD_DEFINED__
typedef interface IScdStopWatch IScdStopWatch;
#endif 	/* __IScdStopWatch_FWD_DEFINED__ */


#ifndef __ScdEventRelay_FWD_DEFINED__
#define __ScdEventRelay_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdEventRelay ScdEventRelay;
#else
typedef struct ScdEventRelay ScdEventRelay;
#endif /* __cplusplus */

#endif 	/* __ScdEventRelay_FWD_DEFINED__ */


#ifndef __ScdStopWatch_FWD_DEFINED__
#define __ScdStopWatch_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdStopWatch ScdStopWatch;
#else
typedef struct ScdStopWatch ScdStopWatch;
#endif /* __cplusplus */

#endif 	/* __ScdStopWatch_FWD_DEFINED__ */


#ifndef __ScdProfile_FWD_DEFINED__
#define __ScdProfile_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdProfile ScdProfile;
#else
typedef struct ScdProfile ScdProfile;
#endif /* __cplusplus */

#endif 	/* __ScdProfile_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __IScdUserInfo_INTERFACE_DEFINED__
#define __IScdUserInfo_INTERFACE_DEFINED__

/* interface IScdUserInfo */
/* [unique][helpstring][dual][uuid][object] */ 

typedef /* [public][public][public] */ 
enum __MIDL_IScdUserInfo_0001
    {	ScdUser_None	= 0,
	ScdUser_Trainee	= 1,
	ScdUser_Instructor	= 2,
	ScdUser_Engineer	= 3,
	ScdUser_SystemManager	= 4
    } 	ScdUserType;


EXTERN_C const IID IID_IScdUserInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("232C83AA-965A-4CEC-93FF-9D70438BD35F")
    IScdUserInfo : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_IsTrainee( 
            /* [retval][out] */ BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_IsEngineer( 
            /* [retval][out] */ BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_IsInstructor( 
            /* [retval][out] */ BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_IsSystemManager( 
            /* [retval][out] */ BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_UserName( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_UserProfile( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_AllUsersProfile( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_UserType( 
            /* [retval][out] */ ScdUserType *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_UserType( 
            /* [in] */ ScdUserType newVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScdUserInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdUserInfo * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdUserInfo * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdUserInfo * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdUserInfo * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdUserInfo * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdUserInfo * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdUserInfo * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsTrainee )( 
            IScdUserInfo * This,
            /* [retval][out] */ BOOL *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsEngineer )( 
            IScdUserInfo * This,
            /* [retval][out] */ BOOL *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsInstructor )( 
            IScdUserInfo * This,
            /* [retval][out] */ BOOL *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsSystemManager )( 
            IScdUserInfo * This,
            /* [retval][out] */ BOOL *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_UserName )( 
            IScdUserInfo * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_UserProfile )( 
            IScdUserInfo * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_AllUsersProfile )( 
            IScdUserInfo * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_UserType )( 
            IScdUserInfo * This,
            /* [retval][out] */ ScdUserType *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_UserType )( 
            IScdUserInfo * This,
            /* [in] */ ScdUserType newVal);
        
        END_INTERFACE
    } IScdUserInfoVtbl;

    interface IScdUserInfo
    {
        CONST_VTBL struct IScdUserInfoVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdUserInfo_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdUserInfo_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdUserInfo_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdUserInfo_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdUserInfo_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdUserInfo_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdUserInfo_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IScdUserInfo_get_IsTrainee(This,pVal)	\
    (This)->lpVtbl -> get_IsTrainee(This,pVal)

#define IScdUserInfo_get_IsEngineer(This,pVal)	\
    (This)->lpVtbl -> get_IsEngineer(This,pVal)

#define IScdUserInfo_get_IsInstructor(This,pVal)	\
    (This)->lpVtbl -> get_IsInstructor(This,pVal)

#define IScdUserInfo_get_IsSystemManager(This,pVal)	\
    (This)->lpVtbl -> get_IsSystemManager(This,pVal)

#define IScdUserInfo_get_UserName(This,pVal)	\
    (This)->lpVtbl -> get_UserName(This,pVal)

#define IScdUserInfo_get_UserProfile(This,pVal)	\
    (This)->lpVtbl -> get_UserProfile(This,pVal)

#define IScdUserInfo_get_AllUsersProfile(This,pVal)	\
    (This)->lpVtbl -> get_AllUsersProfile(This,pVal)

#define IScdUserInfo_get_UserType(This,pVal)	\
    (This)->lpVtbl -> get_UserType(This,pVal)

#define IScdUserInfo_put_UserType(This,newVal)	\
    (This)->lpVtbl -> put_UserType(This,newVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdUserInfo_get_IsTrainee_Proxy( 
    IScdUserInfo * This,
    /* [retval][out] */ BOOL *pVal);


void __RPC_STUB IScdUserInfo_get_IsTrainee_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdUserInfo_get_IsEngineer_Proxy( 
    IScdUserInfo * This,
    /* [retval][out] */ BOOL *pVal);


void __RPC_STUB IScdUserInfo_get_IsEngineer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdUserInfo_get_IsInstructor_Proxy( 
    IScdUserInfo * This,
    /* [retval][out] */ BOOL *pVal);


void __RPC_STUB IScdUserInfo_get_IsInstructor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdUserInfo_get_IsSystemManager_Proxy( 
    IScdUserInfo * This,
    /* [retval][out] */ BOOL *pVal);


void __RPC_STUB IScdUserInfo_get_IsSystemManager_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdUserInfo_get_UserName_Proxy( 
    IScdUserInfo * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IScdUserInfo_get_UserName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdUserInfo_get_UserProfile_Proxy( 
    IScdUserInfo * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IScdUserInfo_get_UserProfile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdUserInfo_get_AllUsersProfile_Proxy( 
    IScdUserInfo * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IScdUserInfo_get_AllUsersProfile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdUserInfo_get_UserType_Proxy( 
    IScdUserInfo * This,
    /* [retval][out] */ ScdUserType *pVal);


void __RPC_STUB IScdUserInfo_get_UserType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IScdUserInfo_put_UserType_Proxy( 
    IScdUserInfo * This,
    /* [in] */ ScdUserType newVal);


void __RPC_STUB IScdUserInfo_put_UserType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScdUserInfo_INTERFACE_DEFINED__ */


#ifndef __IScdProfile_INTERFACE_DEFINED__
#define __IScdProfile_INTERFACE_DEFINED__

/* interface IScdProfile */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdProfile;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0572B86A-699B-4B35-A55D-B62505D126F2")
    IScdProfile : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReadString( 
            /* [in] */ BSTR File,
            /* [in] */ BSTR Section,
            /* [in] */ BSTR Entry,
            /* [in] */ BSTR Default,
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WriteString( 
            /* [in] */ BSTR File,
            /* [in] */ BSTR Section,
            /* [in] */ BSTR Entry,
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReadLong( 
            /* [in] */ BSTR File,
            /* [in] */ BSTR Section,
            /* [in] */ BSTR Entry,
            /* [in] */ long Default,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WriteLong( 
            /* [in] */ BSTR File,
            /* [in] */ BSTR Section,
            /* [in] */ BSTR Entry,
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReadDouble( 
            /* [in] */ BSTR File,
            /* [in] */ BSTR Section,
            /* [in] */ BSTR Entry,
            /* [in] */ double Default,
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WriteDouble( 
            /* [in] */ BSTR File,
            /* [in] */ BSTR Section,
            /* [in] */ BSTR Entry,
            /* [in] */ double newVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScdProfileVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdProfile * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdProfile * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdProfile * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdProfile * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdProfile * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdProfile * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdProfile * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ReadString )( 
            IScdProfile * This,
            /* [in] */ BSTR File,
            /* [in] */ BSTR Section,
            /* [in] */ BSTR Entry,
            /* [in] */ BSTR Default,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WriteString )( 
            IScdProfile * This,
            /* [in] */ BSTR File,
            /* [in] */ BSTR Section,
            /* [in] */ BSTR Entry,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ReadLong )( 
            IScdProfile * This,
            /* [in] */ BSTR File,
            /* [in] */ BSTR Section,
            /* [in] */ BSTR Entry,
            /* [in] */ long Default,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WriteLong )( 
            IScdProfile * This,
            /* [in] */ BSTR File,
            /* [in] */ BSTR Section,
            /* [in] */ BSTR Entry,
            /* [in] */ long newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ReadDouble )( 
            IScdProfile * This,
            /* [in] */ BSTR File,
            /* [in] */ BSTR Section,
            /* [in] */ BSTR Entry,
            /* [in] */ double Default,
            /* [retval][out] */ double *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WriteDouble )( 
            IScdProfile * This,
            /* [in] */ BSTR File,
            /* [in] */ BSTR Section,
            /* [in] */ BSTR Entry,
            /* [in] */ double newVal);
        
        END_INTERFACE
    } IScdProfileVtbl;

    interface IScdProfile
    {
        CONST_VTBL struct IScdProfileVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdProfile_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdProfile_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdProfile_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdProfile_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdProfile_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdProfile_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdProfile_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IScdProfile_ReadString(This,File,Section,Entry,Default,pVal)	\
    (This)->lpVtbl -> ReadString(This,File,Section,Entry,Default,pVal)

#define IScdProfile_WriteString(This,File,Section,Entry,newVal)	\
    (This)->lpVtbl -> WriteString(This,File,Section,Entry,newVal)

#define IScdProfile_ReadLong(This,File,Section,Entry,Default,pVal)	\
    (This)->lpVtbl -> ReadLong(This,File,Section,Entry,Default,pVal)

#define IScdProfile_WriteLong(This,File,Section,Entry,newVal)	\
    (This)->lpVtbl -> WriteLong(This,File,Section,Entry,newVal)

#define IScdProfile_ReadDouble(This,File,Section,Entry,Default,pVal)	\
    (This)->lpVtbl -> ReadDouble(This,File,Section,Entry,Default,pVal)

#define IScdProfile_WriteDouble(This,File,Section,Entry,newVal)	\
    (This)->lpVtbl -> WriteDouble(This,File,Section,Entry,newVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdProfile_ReadString_Proxy( 
    IScdProfile * This,
    /* [in] */ BSTR File,
    /* [in] */ BSTR Section,
    /* [in] */ BSTR Entry,
    /* [in] */ BSTR Default,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IScdProfile_ReadString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdProfile_WriteString_Proxy( 
    IScdProfile * This,
    /* [in] */ BSTR File,
    /* [in] */ BSTR Section,
    /* [in] */ BSTR Entry,
    /* [in] */ BSTR newVal);


void __RPC_STUB IScdProfile_WriteString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdProfile_ReadLong_Proxy( 
    IScdProfile * This,
    /* [in] */ BSTR File,
    /* [in] */ BSTR Section,
    /* [in] */ BSTR Entry,
    /* [in] */ long Default,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IScdProfile_ReadLong_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdProfile_WriteLong_Proxy( 
    IScdProfile * This,
    /* [in] */ BSTR File,
    /* [in] */ BSTR Section,
    /* [in] */ BSTR Entry,
    /* [in] */ long newVal);


void __RPC_STUB IScdProfile_WriteLong_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdProfile_ReadDouble_Proxy( 
    IScdProfile * This,
    /* [in] */ BSTR File,
    /* [in] */ BSTR Section,
    /* [in] */ BSTR Entry,
    /* [in] */ double Default,
    /* [retval][out] */ double *pVal);


void __RPC_STUB IScdProfile_ReadDouble_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdProfile_WriteDouble_Proxy( 
    IScdProfile * This,
    /* [in] */ BSTR File,
    /* [in] */ BSTR Section,
    /* [in] */ BSTR Entry,
    /* [in] */ double newVal);


void __RPC_STUB IScdProfile_WriteDouble_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScdProfile_INTERFACE_DEFINED__ */



#ifndef __ScdVBLib_LIBRARY_DEFINED__
#define __ScdVBLib_LIBRARY_DEFINED__

/* library ScdVBLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_ScdVBLib;

#ifndef ___ScdUserInfoEvents_DISPINTERFACE_DEFINED__
#define ___ScdUserInfoEvents_DISPINTERFACE_DEFINED__

/* dispinterface _ScdUserInfoEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__ScdUserInfoEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("5AD09BF1-9696-46BA-92C7-FA5B48AB40A0")
    _ScdUserInfoEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _ScdUserInfoEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _ScdUserInfoEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _ScdUserInfoEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _ScdUserInfoEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _ScdUserInfoEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _ScdUserInfoEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _ScdUserInfoEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _ScdUserInfoEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _ScdUserInfoEventsVtbl;

    interface _ScdUserInfoEvents
    {
        CONST_VTBL struct _ScdUserInfoEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _ScdUserInfoEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _ScdUserInfoEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _ScdUserInfoEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _ScdUserInfoEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _ScdUserInfoEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _ScdUserInfoEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _ScdUserInfoEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___ScdUserInfoEvents_DISPINTERFACE_DEFINED__ */


#ifndef __IScdTimer_INTERFACE_DEFINED__
#define __IScdTimer_INTERFACE_DEFINED__

/* interface IScdTimer */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdTimer;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("F90C014A-9329-4FA1-A117-040B7E04BD8D")
    IScdTimer : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Start( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Stop( void) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Interval( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Interval( 
            /* [in] */ long newVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScdTimerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdTimer * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdTimer * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdTimer * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdTimer * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdTimer * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdTimer * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdTimer * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Start )( 
            IScdTimer * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Stop )( 
            IScdTimer * This);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Interval )( 
            IScdTimer * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Interval )( 
            IScdTimer * This,
            /* [in] */ long newVal);
        
        END_INTERFACE
    } IScdTimerVtbl;

    interface IScdTimer
    {
        CONST_VTBL struct IScdTimerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdTimer_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdTimer_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdTimer_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdTimer_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdTimer_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdTimer_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdTimer_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IScdTimer_Start(This)	\
    (This)->lpVtbl -> Start(This)

#define IScdTimer_Stop(This)	\
    (This)->lpVtbl -> Stop(This)

#define IScdTimer_get_Interval(This,pVal)	\
    (This)->lpVtbl -> get_Interval(This,pVal)

#define IScdTimer_put_Interval(This,newVal)	\
    (This)->lpVtbl -> put_Interval(This,newVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdTimer_Start_Proxy( 
    IScdTimer * This);


void __RPC_STUB IScdTimer_Start_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdTimer_Stop_Proxy( 
    IScdTimer * This);


void __RPC_STUB IScdTimer_Stop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdTimer_get_Interval_Proxy( 
    IScdTimer * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IScdTimer_get_Interval_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IScdTimer_put_Interval_Proxy( 
    IScdTimer * This,
    /* [in] */ long newVal);


void __RPC_STUB IScdTimer_put_Interval_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScdTimer_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_ScdUserInfo;

#ifdef __cplusplus

class DECLSPEC_UUID("03E0B7B1-100F-459B-B5DD-80A9CA890485")
ScdUserInfo;
#endif

#ifndef ___ScdTimerEvents_DISPINTERFACE_DEFINED__
#define ___ScdTimerEvents_DISPINTERFACE_DEFINED__

/* dispinterface _ScdTimerEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__ScdTimerEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("DC37B94D-4834-456E-9423-966554E5364E")
    _ScdTimerEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _ScdTimerEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _ScdTimerEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _ScdTimerEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _ScdTimerEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _ScdTimerEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _ScdTimerEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _ScdTimerEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _ScdTimerEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _ScdTimerEventsVtbl;

    interface _ScdTimerEvents
    {
        CONST_VTBL struct _ScdTimerEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _ScdTimerEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _ScdTimerEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _ScdTimerEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _ScdTimerEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _ScdTimerEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _ScdTimerEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _ScdTimerEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___ScdTimerEvents_DISPINTERFACE_DEFINED__ */


#ifndef __IScdRegistry_INTERFACE_DEFINED__
#define __IScdRegistry_INTERFACE_DEFINED__

/* interface IScdRegistry */
/* [unique][helpstring][dual][uuid][object] */ 

typedef /* [public][public][public] */ 
enum __MIDL_IScdRegistry_0001
    {	ScdHKEY_CLASSES_ROOT	= 0x80000000,
	ScdHKEY_CURRENT_USER	= 0x80000001,
	ScdHKEY_LOCAL_MACHINE	= 0x80000002,
	ScdHKEY_USERS	= 0x80000003
    } 	ScdHKEY;

typedef /* [public][public][public] */ 
enum __MIDL_IScdRegistry_0002
    {	ScdKey_NONE	= 0,
	ScdKey_SZ	= 1,
	ScdKey_DWORD	= 4
    } 	ScdKeyTypes;


EXTERN_C const IID IID_IScdRegistry;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2EC4B69C-050A-418E-A4CE-575D5575A540")
    IScdRegistry : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetKey( 
            /* [in] */ ScdKeyTypes Type,
            /* [in] */ ScdHKEY HKEYRoot,
            /* [in] */ BSTR KeyPath,
            /* [in] */ BSTR KeyName,
            /* [out][in] */ VARIANT *pValue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE PutKey( 
            /* [in] */ ScdKeyTypes Type,
            /* [in] */ ScdHKEY HKEYRoot,
            /* [in] */ BSTR KeyPath,
            /* [in] */ BSTR KeyName,
            /* [in] */ VARIANT Value) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScdRegistryVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdRegistry * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdRegistry * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdRegistry * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdRegistry * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdRegistry * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdRegistry * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdRegistry * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetKey )( 
            IScdRegistry * This,
            /* [in] */ ScdKeyTypes Type,
            /* [in] */ ScdHKEY HKEYRoot,
            /* [in] */ BSTR KeyPath,
            /* [in] */ BSTR KeyName,
            /* [out][in] */ VARIANT *pValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *PutKey )( 
            IScdRegistry * This,
            /* [in] */ ScdKeyTypes Type,
            /* [in] */ ScdHKEY HKEYRoot,
            /* [in] */ BSTR KeyPath,
            /* [in] */ BSTR KeyName,
            /* [in] */ VARIANT Value);
        
        END_INTERFACE
    } IScdRegistryVtbl;

    interface IScdRegistry
    {
        CONST_VTBL struct IScdRegistryVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdRegistry_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdRegistry_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdRegistry_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdRegistry_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdRegistry_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdRegistry_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdRegistry_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IScdRegistry_GetKey(This,Type,HKEYRoot,KeyPath,KeyName,pValue)	\
    (This)->lpVtbl -> GetKey(This,Type,HKEYRoot,KeyPath,KeyName,pValue)

#define IScdRegistry_PutKey(This,Type,HKEYRoot,KeyPath,KeyName,Value)	\
    (This)->lpVtbl -> PutKey(This,Type,HKEYRoot,KeyPath,KeyName,Value)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdRegistry_GetKey_Proxy( 
    IScdRegistry * This,
    /* [in] */ ScdKeyTypes Type,
    /* [in] */ ScdHKEY HKEYRoot,
    /* [in] */ BSTR KeyPath,
    /* [in] */ BSTR KeyName,
    /* [out][in] */ VARIANT *pValue);


void __RPC_STUB IScdRegistry_GetKey_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdRegistry_PutKey_Proxy( 
    IScdRegistry * This,
    /* [in] */ ScdKeyTypes Type,
    /* [in] */ ScdHKEY HKEYRoot,
    /* [in] */ BSTR KeyPath,
    /* [in] */ BSTR KeyName,
    /* [in] */ VARIANT Value);


void __RPC_STUB IScdRegistry_PutKey_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScdRegistry_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_ScdTimer;

#ifdef __cplusplus

class DECLSPEC_UUID("4B8BDF4B-681C-4201-989E-7916166D50D4")
ScdTimer;
#endif

#ifndef ___ScdRegistryEvents_DISPINTERFACE_DEFINED__
#define ___ScdRegistryEvents_DISPINTERFACE_DEFINED__

/* dispinterface _ScdRegistryEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__ScdRegistryEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("FEDD2DC2-B6F6-41B8-8230-33FA47D8A8F3")
    _ScdRegistryEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _ScdRegistryEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _ScdRegistryEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _ScdRegistryEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _ScdRegistryEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _ScdRegistryEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _ScdRegistryEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _ScdRegistryEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _ScdRegistryEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _ScdRegistryEventsVtbl;

    interface _ScdRegistryEvents
    {
        CONST_VTBL struct _ScdRegistryEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _ScdRegistryEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _ScdRegistryEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _ScdRegistryEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _ScdRegistryEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _ScdRegistryEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _ScdRegistryEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _ScdRegistryEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___ScdRegistryEvents_DISPINTERFACE_DEFINED__ */


#ifndef __IScdFileSystem_INTERFACE_DEFINED__
#define __IScdFileSystem_INTERFACE_DEFINED__

/* interface IScdFileSystem */
/* [unique][helpstring][dual][uuid][object] */ 

typedef /* [public][public][public] */ 
enum __MIDL_IScdFileSystem_0001
    {	ScdFile_NONE	= 0,
	ScdFile_ARCHIVE	= 0x20,
	ScdFile_DIRECTORY	= 0x10,
	ScdFile_HIDDEN	= 0x2,
	ScdFile_NORMAL	= 0x80,
	ScdFile_READONLY	= 0x1,
	ScdFile_SYSTEM	= 0x4,
	ScdFile_TEMPORARY	= 0x100
    } 	ScdFileAttributes;


EXTERN_C const IID IID_IScdFileSystem;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("FB5EC254-EFFA-414F-BA73-AB56A94801A7")
    IScdFileSystem : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_FindFirstFile( 
            /* [in] */ BSTR Mask,
            /* [out] */ BSTR *pFileName,
            /* [out] */ ScdFileAttributes *pAttr,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_FindNextFile( 
            /* [out] */ BSTR *pFileName,
            /* [out] */ ScdFileAttributes *pAttr,
            /* [retval][out] */ long *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScdFileSystemVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdFileSystem * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdFileSystem * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdFileSystem * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdFileSystem * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdFileSystem * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdFileSystem * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdFileSystem * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_FindFirstFile )( 
            IScdFileSystem * This,
            /* [in] */ BSTR Mask,
            /* [out] */ BSTR *pFileName,
            /* [out] */ ScdFileAttributes *pAttr,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_FindNextFile )( 
            IScdFileSystem * This,
            /* [out] */ BSTR *pFileName,
            /* [out] */ ScdFileAttributes *pAttr,
            /* [retval][out] */ long *pVal);
        
        END_INTERFACE
    } IScdFileSystemVtbl;

    interface IScdFileSystem
    {
        CONST_VTBL struct IScdFileSystemVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdFileSystem_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdFileSystem_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdFileSystem_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdFileSystem_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdFileSystem_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdFileSystem_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdFileSystem_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IScdFileSystem_get_FindFirstFile(This,Mask,pFileName,pAttr,pVal)	\
    (This)->lpVtbl -> get_FindFirstFile(This,Mask,pFileName,pAttr,pVal)

#define IScdFileSystem_get_FindNextFile(This,pFileName,pAttr,pVal)	\
    (This)->lpVtbl -> get_FindNextFile(This,pFileName,pAttr,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdFileSystem_get_FindFirstFile_Proxy( 
    IScdFileSystem * This,
    /* [in] */ BSTR Mask,
    /* [out] */ BSTR *pFileName,
    /* [out] */ ScdFileAttributes *pAttr,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IScdFileSystem_get_FindFirstFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdFileSystem_get_FindNextFile_Proxy( 
    IScdFileSystem * This,
    /* [out] */ BSTR *pFileName,
    /* [out] */ ScdFileAttributes *pAttr,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IScdFileSystem_get_FindNextFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScdFileSystem_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_ScdRegistry;

#ifdef __cplusplus

class DECLSPEC_UUID("0B076583-EB56-4F40-A825-BBA4870FE7A7")
ScdRegistry;
#endif

#ifndef ___ScdFileSystemEvents_DISPINTERFACE_DEFINED__
#define ___ScdFileSystemEvents_DISPINTERFACE_DEFINED__

/* dispinterface _ScdFileSystemEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__ScdFileSystemEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("8600F1E3-2E84-4893-8966-977EFA66ABAB")
    _ScdFileSystemEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _ScdFileSystemEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _ScdFileSystemEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _ScdFileSystemEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _ScdFileSystemEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _ScdFileSystemEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _ScdFileSystemEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _ScdFileSystemEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _ScdFileSystemEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _ScdFileSystemEventsVtbl;

    interface _ScdFileSystemEvents
    {
        CONST_VTBL struct _ScdFileSystemEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _ScdFileSystemEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _ScdFileSystemEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _ScdFileSystemEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _ScdFileSystemEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _ScdFileSystemEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _ScdFileSystemEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _ScdFileSystemEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___ScdFileSystemEvents_DISPINTERFACE_DEFINED__ */


#ifndef __IScdEventRelay_INTERFACE_DEFINED__
#define __IScdEventRelay_INTERFACE_DEFINED__

/* interface IScdEventRelay */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdEventRelay;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("50B624A4-2BA3-4FA3-B5C2-6B2D7DBA5BA9")
    IScdEventRelay : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE NotifyMe( 
            /* [in] */ long i_lCookie) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScdEventRelayVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdEventRelay * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdEventRelay * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdEventRelay * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdEventRelay * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdEventRelay * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdEventRelay * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdEventRelay * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *NotifyMe )( 
            IScdEventRelay * This,
            /* [in] */ long i_lCookie);
        
        END_INTERFACE
    } IScdEventRelayVtbl;

    interface IScdEventRelay
    {
        CONST_VTBL struct IScdEventRelayVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdEventRelay_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdEventRelay_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdEventRelay_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdEventRelay_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdEventRelay_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdEventRelay_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdEventRelay_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IScdEventRelay_NotifyMe(This,i_lCookie)	\
    (This)->lpVtbl -> NotifyMe(This,i_lCookie)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdEventRelay_NotifyMe_Proxy( 
    IScdEventRelay * This,
    /* [in] */ long i_lCookie);


void __RPC_STUB IScdEventRelay_NotifyMe_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScdEventRelay_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_ScdFileSystem;

#ifdef __cplusplus

class DECLSPEC_UUID("A1D8288B-081E-4828-A938-05F6804CD8CF")
ScdFileSystem;
#endif

#ifndef ___ScdEventRelayEvents_DISPINTERFACE_DEFINED__
#define ___ScdEventRelayEvents_DISPINTERFACE_DEFINED__

/* dispinterface _ScdEventRelayEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__ScdEventRelayEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("D671768F-8F32-48FE-8C5C-A69834E24770")
    _ScdEventRelayEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _ScdEventRelayEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _ScdEventRelayEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _ScdEventRelayEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _ScdEventRelayEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _ScdEventRelayEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _ScdEventRelayEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _ScdEventRelayEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _ScdEventRelayEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _ScdEventRelayEventsVtbl;

    interface _ScdEventRelayEvents
    {
        CONST_VTBL struct _ScdEventRelayEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _ScdEventRelayEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _ScdEventRelayEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _ScdEventRelayEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _ScdEventRelayEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _ScdEventRelayEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _ScdEventRelayEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _ScdEventRelayEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___ScdEventRelayEvents_DISPINTERFACE_DEFINED__ */


#ifndef __IScdStopWatch_INTERFACE_DEFINED__
#define __IScdStopWatch_INTERFACE_DEFINED__

/* interface IScdStopWatch */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdStopWatch;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2EFCB979-2075-48A0-B504-3361C7EEF788")
    IScdStopWatch : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Start( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Stop( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReStart( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Clear( void) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Time( 
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Elapsed( 
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Lap( 
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Laps( 
            /* [retval][out] */ DWORD *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScdStopWatchVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdStopWatch * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdStopWatch * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdStopWatch * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdStopWatch * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdStopWatch * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdStopWatch * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdStopWatch * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Start )( 
            IScdStopWatch * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Stop )( 
            IScdStopWatch * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ReStart )( 
            IScdStopWatch * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Clear )( 
            IScdStopWatch * This);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Time )( 
            IScdStopWatch * This,
            /* [retval][out] */ double *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Elapsed )( 
            IScdStopWatch * This,
            /* [retval][out] */ double *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Lap )( 
            IScdStopWatch * This,
            /* [retval][out] */ double *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Laps )( 
            IScdStopWatch * This,
            /* [retval][out] */ DWORD *pVal);
        
        END_INTERFACE
    } IScdStopWatchVtbl;

    interface IScdStopWatch
    {
        CONST_VTBL struct IScdStopWatchVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdStopWatch_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdStopWatch_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdStopWatch_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdStopWatch_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdStopWatch_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdStopWatch_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdStopWatch_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IScdStopWatch_Start(This)	\
    (This)->lpVtbl -> Start(This)

#define IScdStopWatch_Stop(This)	\
    (This)->lpVtbl -> Stop(This)

#define IScdStopWatch_ReStart(This)	\
    (This)->lpVtbl -> ReStart(This)

#define IScdStopWatch_Clear(This)	\
    (This)->lpVtbl -> Clear(This)

#define IScdStopWatch_get_Time(This,pVal)	\
    (This)->lpVtbl -> get_Time(This,pVal)

#define IScdStopWatch_get_Elapsed(This,pVal)	\
    (This)->lpVtbl -> get_Elapsed(This,pVal)

#define IScdStopWatch_get_Lap(This,pVal)	\
    (This)->lpVtbl -> get_Lap(This,pVal)

#define IScdStopWatch_get_Laps(This,pVal)	\
    (This)->lpVtbl -> get_Laps(This,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdStopWatch_Start_Proxy( 
    IScdStopWatch * This);


void __RPC_STUB IScdStopWatch_Start_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdStopWatch_Stop_Proxy( 
    IScdStopWatch * This);


void __RPC_STUB IScdStopWatch_Stop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdStopWatch_ReStart_Proxy( 
    IScdStopWatch * This);


void __RPC_STUB IScdStopWatch_ReStart_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdStopWatch_Clear_Proxy( 
    IScdStopWatch * This);


void __RPC_STUB IScdStopWatch_Clear_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdStopWatch_get_Time_Proxy( 
    IScdStopWatch * This,
    /* [retval][out] */ double *pVal);


void __RPC_STUB IScdStopWatch_get_Time_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdStopWatch_get_Elapsed_Proxy( 
    IScdStopWatch * This,
    /* [retval][out] */ double *pVal);


void __RPC_STUB IScdStopWatch_get_Elapsed_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdStopWatch_get_Lap_Proxy( 
    IScdStopWatch * This,
    /* [retval][out] */ double *pVal);


void __RPC_STUB IScdStopWatch_get_Lap_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdStopWatch_get_Laps_Proxy( 
    IScdStopWatch * This,
    /* [retval][out] */ DWORD *pVal);


void __RPC_STUB IScdStopWatch_get_Laps_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScdStopWatch_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_ScdEventRelay;

#ifdef __cplusplus

class DECLSPEC_UUID("61A60CB3-DDD9-4904-8680-D6D6727D5462")
ScdEventRelay;
#endif

EXTERN_C const CLSID CLSID_ScdStopWatch;

#ifdef __cplusplus

class DECLSPEC_UUID("92520B1B-1386-4671-85A9-7BA9AA73A0C6")
ScdStopWatch;
#endif

EXTERN_C const CLSID CLSID_ScdProfile;

#ifdef __cplusplus

class DECLSPEC_UUID("CCE6D425-88A9-4A4A-BF62-16B7F9540FC5")
ScdProfile;
#endif
#endif /* __ScdVBLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


