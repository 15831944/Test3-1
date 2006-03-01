

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0361 */
/* at Mon Feb 27 10:13:33 2006
 */
/* Compiler settings for .\ScdMdl.idl:
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


#ifndef __ScdMdl_h__
#define __ScdMdl_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IScdLicenseMdl_FWD_DEFINED__
#define __IScdLicenseMdl_FWD_DEFINED__
typedef interface IScdLicenseMdl IScdLicenseMdl;
#endif 	/* __IScdLicenseMdl_FWD_DEFINED__ */


#ifndef __IScdConversion_FWD_DEFINED__
#define __IScdConversion_FWD_DEFINED__
typedef interface IScdConversion IScdConversion;
#endif 	/* __IScdConversion_FWD_DEFINED__ */


#ifndef __IScdConversions_FWD_DEFINED__
#define __IScdConversions_FWD_DEFINED__
typedef interface IScdConversions IScdConversions;
#endif 	/* __IScdConversions_FWD_DEFINED__ */


#ifndef __IScdProperty_FWD_DEFINED__
#define __IScdProperty_FWD_DEFINED__
typedef interface IScdProperty IScdProperty;
#endif 	/* __IScdProperty_FWD_DEFINED__ */


#ifndef __IScdProperties_FWD_DEFINED__
#define __IScdProperties_FWD_DEFINED__
typedef interface IScdProperties IScdProperties;
#endif 	/* __IScdProperties_FWD_DEFINED__ */


#ifndef __IScdSpecieDefns_FWD_DEFINED__
#define __IScdSpecieDefns_FWD_DEFINED__
typedef interface IScdSpecieDefns IScdSpecieDefns;
#endif 	/* __IScdSpecieDefns_FWD_DEFINED__ */


#ifndef __IScdSpecieDefn_FWD_DEFINED__
#define __IScdSpecieDefn_FWD_DEFINED__
typedef interface IScdSpecieDefn IScdSpecieDefn;
#endif 	/* __IScdSpecieDefn_FWD_DEFINED__ */


#ifndef __IScdSpVectorDefn_FWD_DEFINED__
#define __IScdSpVectorDefn_FWD_DEFINED__
typedef interface IScdSpVectorDefn IScdSpVectorDefn;
#endif 	/* __IScdSpVectorDefn_FWD_DEFINED__ */


#ifndef __IScdSpVector_FWD_DEFINED__
#define __IScdSpVector_FWD_DEFINED__
typedef interface IScdSpVector IScdSpVector;
#endif 	/* __IScdSpVector_FWD_DEFINED__ */


#ifndef __IScdQuality_FWD_DEFINED__
#define __IScdQuality_FWD_DEFINED__
typedef interface IScdQuality IScdQuality;
#endif 	/* __IScdQuality_FWD_DEFINED__ */


#ifndef __IScdQualities_FWD_DEFINED__
#define __IScdQualities_FWD_DEFINED__
typedef interface IScdQualities IScdQualities;
#endif 	/* __IScdQualities_FWD_DEFINED__ */


#ifndef __ScdLicenseMdl_FWD_DEFINED__
#define __ScdLicenseMdl_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdLicenseMdl ScdLicenseMdl;
#else
typedef struct ScdLicenseMdl ScdLicenseMdl;
#endif /* __cplusplus */

#endif 	/* __ScdLicenseMdl_FWD_DEFINED__ */


#ifndef __ScdSpecieDefns_FWD_DEFINED__
#define __ScdSpecieDefns_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdSpecieDefns ScdSpecieDefns;
#else
typedef struct ScdSpecieDefns ScdSpecieDefns;
#endif /* __cplusplus */

#endif 	/* __ScdSpecieDefns_FWD_DEFINED__ */


#ifndef __ScdSpecieDefn_FWD_DEFINED__
#define __ScdSpecieDefn_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdSpecieDefn ScdSpecieDefn;
#else
typedef struct ScdSpecieDefn ScdSpecieDefn;
#endif /* __cplusplus */

#endif 	/* __ScdSpecieDefn_FWD_DEFINED__ */


#ifndef __ScdProperty_FWD_DEFINED__
#define __ScdProperty_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdProperty ScdProperty;
#else
typedef struct ScdProperty ScdProperty;
#endif /* __cplusplus */

#endif 	/* __ScdProperty_FWD_DEFINED__ */


#ifndef __ScdConversions_FWD_DEFINED__
#define __ScdConversions_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdConversions ScdConversions;
#else
typedef struct ScdConversions ScdConversions;
#endif /* __cplusplus */

#endif 	/* __ScdConversions_FWD_DEFINED__ */


#ifndef __ScdConversion_FWD_DEFINED__
#define __ScdConversion_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdConversion ScdConversion;
#else
typedef struct ScdConversion ScdConversion;
#endif /* __cplusplus */

#endif 	/* __ScdConversion_FWD_DEFINED__ */


#ifndef __ScdProperties_FWD_DEFINED__
#define __ScdProperties_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdProperties ScdProperties;
#else
typedef struct ScdProperties ScdProperties;
#endif /* __cplusplus */

#endif 	/* __ScdProperties_FWD_DEFINED__ */


#ifndef __ScdSpVector_FWD_DEFINED__
#define __ScdSpVector_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdSpVector ScdSpVector;
#else
typedef struct ScdSpVector ScdSpVector;
#endif /* __cplusplus */

#endif 	/* __ScdSpVector_FWD_DEFINED__ */


#ifndef __ScdQuality_FWD_DEFINED__
#define __ScdQuality_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdQuality ScdQuality;
#else
typedef struct ScdQuality ScdQuality;
#endif /* __cplusplus */

#endif 	/* __ScdQuality_FWD_DEFINED__ */


#ifndef __ScdQualities_FWD_DEFINED__
#define __ScdQualities_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdQualities ScdQualities;
#else
typedef struct ScdQualities ScdQualities;
#endif /* __cplusplus */

#endif 	/* __ScdQualities_FWD_DEFINED__ */


#ifndef __ScdSpVectorDefn_FWD_DEFINED__
#define __ScdSpVectorDefn_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdSpVectorDefn ScdSpVectorDefn;
#else
typedef struct ScdSpVectorDefn ScdSpVectorDefn;
#endif /* __cplusplus */

#endif 	/* __ScdSpVectorDefn_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"
#include "ScdIF.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 


#ifndef __ScdMdl_LIBRARY_DEFINED__
#define __ScdMdl_LIBRARY_DEFINED__

/* library ScdMdl */
/* [helpstring][version][uuid] */ 










EXTERN_C const IID LIBID_ScdMdl;

#ifndef __IScdLicenseMdl_INTERFACE_DEFINED__
#define __IScdLicenseMdl_INTERFACE_DEFINED__

/* interface IScdLicenseMdl */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdLicenseMdl;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("FAB87D34-0F72-4801-AA82-CBF156E1EB16")
    IScdLicenseMdl : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IScdLicenseMdlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdLicenseMdl * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdLicenseMdl * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdLicenseMdl * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdLicenseMdl * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdLicenseMdl * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdLicenseMdl * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdLicenseMdl * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IScdLicenseMdlVtbl;

    interface IScdLicenseMdl
    {
        CONST_VTBL struct IScdLicenseMdlVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdLicenseMdl_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdLicenseMdl_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdLicenseMdl_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdLicenseMdl_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdLicenseMdl_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdLicenseMdl_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdLicenseMdl_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IScdLicenseMdl_INTERFACE_DEFINED__ */


#ifndef __IScdConversion_INTERFACE_DEFINED__
#define __IScdConversion_INTERFACE_DEFINED__

/* interface IScdConversion */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdConversion;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("64BE6593-613C-4574-AE5B-F7031EDCC710")
    IScdConversion : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Family( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Description( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Scale( 
            /* [retval][out] */ DOUBLE *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Offset( 
            /* [retval][out] */ DOUBLE *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_IsSI( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [restricted][helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown **pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Item( 
            /* [in] */ VARIANT WhichOne,
            /* [retval][out] */ IScdConversion **pItem) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Index( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetObjectInfo( 
            /* [in] */ LONG iCnv,
            /* [in] */ LONG iIndex) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Formula( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScdConversionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdConversion * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdConversion * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdConversion * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdConversion * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdConversion * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdConversion * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdConversion * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Family )( 
            IScdConversion * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            IScdConversion * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Description )( 
            IScdConversion * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Scale )( 
            IScdConversion * This,
            /* [retval][out] */ DOUBLE *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Offset )( 
            IScdConversion * This,
            /* [retval][out] */ DOUBLE *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsSI )( 
            IScdConversion * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            IScdConversion * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [restricted][helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get__NewEnum )( 
            IScdConversion * This,
            /* [retval][out] */ IUnknown **pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Item )( 
            IScdConversion * This,
            /* [in] */ VARIANT WhichOne,
            /* [retval][out] */ IScdConversion **pItem);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Index )( 
            IScdConversion * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetObjectInfo )( 
            IScdConversion * This,
            /* [in] */ LONG iCnv,
            /* [in] */ LONG iIndex);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Formula )( 
            IScdConversion * This,
            /* [retval][out] */ BSTR *pVal);
        
        END_INTERFACE
    } IScdConversionVtbl;

    interface IScdConversion
    {
        CONST_VTBL struct IScdConversionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdConversion_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdConversion_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdConversion_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdConversion_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdConversion_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdConversion_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdConversion_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IScdConversion_get_Family(This,pVal)	\
    (This)->lpVtbl -> get_Family(This,pVal)

#define IScdConversion_get_Name(This,pVal)	\
    (This)->lpVtbl -> get_Name(This,pVal)

#define IScdConversion_get_Description(This,pVal)	\
    (This)->lpVtbl -> get_Description(This,pVal)

#define IScdConversion_get_Scale(This,pVal)	\
    (This)->lpVtbl -> get_Scale(This,pVal)

#define IScdConversion_get_Offset(This,pVal)	\
    (This)->lpVtbl -> get_Offset(This,pVal)

#define IScdConversion_get_IsSI(This,pVal)	\
    (This)->lpVtbl -> get_IsSI(This,pVal)

#define IScdConversion_get_Count(This,pVal)	\
    (This)->lpVtbl -> get_Count(This,pVal)

#define IScdConversion_get__NewEnum(This,pVal)	\
    (This)->lpVtbl -> get__NewEnum(This,pVal)

#define IScdConversion_Item(This,WhichOne,pItem)	\
    (This)->lpVtbl -> Item(This,WhichOne,pItem)

#define IScdConversion_get_Index(This,pVal)	\
    (This)->lpVtbl -> get_Index(This,pVal)

#define IScdConversion_SetObjectInfo(This,iCnv,iIndex)	\
    (This)->lpVtbl -> SetObjectInfo(This,iCnv,iIndex)

#define IScdConversion_get_Formula(This,pVal)	\
    (This)->lpVtbl -> get_Formula(This,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdConversion_get_Family_Proxy( 
    IScdConversion * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IScdConversion_get_Family_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdConversion_get_Name_Proxy( 
    IScdConversion * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IScdConversion_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdConversion_get_Description_Proxy( 
    IScdConversion * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IScdConversion_get_Description_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdConversion_get_Scale_Proxy( 
    IScdConversion * This,
    /* [retval][out] */ DOUBLE *pVal);


void __RPC_STUB IScdConversion_get_Scale_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdConversion_get_Offset_Proxy( 
    IScdConversion * This,
    /* [retval][out] */ DOUBLE *pVal);


void __RPC_STUB IScdConversion_get_Offset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdConversion_get_IsSI_Proxy( 
    IScdConversion * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB IScdConversion_get_IsSI_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdConversion_get_Count_Proxy( 
    IScdConversion * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB IScdConversion_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [restricted][helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdConversion_get__NewEnum_Proxy( 
    IScdConversion * This,
    /* [retval][out] */ IUnknown **pVal);


void __RPC_STUB IScdConversion_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdConversion_Item_Proxy( 
    IScdConversion * This,
    /* [in] */ VARIANT WhichOne,
    /* [retval][out] */ IScdConversion **pItem);


void __RPC_STUB IScdConversion_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdConversion_get_Index_Proxy( 
    IScdConversion * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB IScdConversion_get_Index_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdConversion_SetObjectInfo_Proxy( 
    IScdConversion * This,
    /* [in] */ LONG iCnv,
    /* [in] */ LONG iIndex);


void __RPC_STUB IScdConversion_SetObjectInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdConversion_get_Formula_Proxy( 
    IScdConversion * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IScdConversion_get_Formula_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScdConversion_INTERFACE_DEFINED__ */


#ifndef __IScdConversions_INTERFACE_DEFINED__
#define __IScdConversions_INTERFACE_DEFINED__

/* interface IScdConversions */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdConversions;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("CE1F0452-571C-4B26-AADF-6F1D6FF60E6B")
    IScdConversions : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [restricted][helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown **pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Item( 
            /* [in] */ VARIANT WhichOne,
            /* [retval][out] */ IScdConversion **pItem) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScdConversionsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdConversions * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdConversions * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdConversions * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdConversions * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdConversions * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdConversions * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdConversions * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            IScdConversions * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [restricted][helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get__NewEnum )( 
            IScdConversions * This,
            /* [retval][out] */ IUnknown **pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Item )( 
            IScdConversions * This,
            /* [in] */ VARIANT WhichOne,
            /* [retval][out] */ IScdConversion **pItem);
        
        END_INTERFACE
    } IScdConversionsVtbl;

    interface IScdConversions
    {
        CONST_VTBL struct IScdConversionsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdConversions_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdConversions_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdConversions_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdConversions_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdConversions_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdConversions_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdConversions_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IScdConversions_get_Count(This,pVal)	\
    (This)->lpVtbl -> get_Count(This,pVal)

#define IScdConversions_get__NewEnum(This,pVal)	\
    (This)->lpVtbl -> get__NewEnum(This,pVal)

#define IScdConversions_Item(This,WhichOne,pItem)	\
    (This)->lpVtbl -> Item(This,WhichOne,pItem)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdConversions_get_Count_Proxy( 
    IScdConversions * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB IScdConversions_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [restricted][helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdConversions_get__NewEnum_Proxy( 
    IScdConversions * This,
    /* [retval][out] */ IUnknown **pVal);


void __RPC_STUB IScdConversions_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdConversions_Item_Proxy( 
    IScdConversions * This,
    /* [in] */ VARIANT WhichOne,
    /* [retval][out] */ IScdConversion **pItem);


void __RPC_STUB IScdConversions_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScdConversions_INTERFACE_DEFINED__ */


#ifndef __IScdProperty_INTERFACE_DEFINED__
#define __IScdProperty_INTERFACE_DEFINED__

/* interface IScdProperty */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdProperty;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B5AD2C79-D2B7-4D88-8DD6-A26AE9A18438")
    IScdProperty : public IDispatch
    {
    public:
        virtual /* [helpstring][restricted][id] */ HRESULT STDMETHODCALLTYPE SetObjectInfo( 
            /* [in] */ ULONG What,
            /* [in] */ ULONG Mdl,
            /* [in] */ ULONG PropNo) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Value( 
            /* [defaultvalue][in] */ long Phases,
            /* [defaultvalue][in] */ double TdK,
            /* [defaultvalue][in] */ double PkPa,
            /* [retval][out] */ VARIANT *pValue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetValue( 
            /* [in] */ VARIANT *pValue) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Tag( 
            /* [retval][out] */ BSTR *pName) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Symbol( 
            /* [retval][out] */ BSTR *pName) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Description( 
            /* [retval][out] */ BSTR *pDesc) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Cnv( 
            /* [retval][out] */ IScdConversion **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Index( 
            /* [retval][out] */ LONG *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScdPropertyVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdProperty * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdProperty * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdProperty * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdProperty * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdProperty * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdProperty * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdProperty * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][restricted][id] */ HRESULT ( STDMETHODCALLTYPE *SetObjectInfo )( 
            IScdProperty * This,
            /* [in] */ ULONG What,
            /* [in] */ ULONG Mdl,
            /* [in] */ ULONG PropNo);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Value )( 
            IScdProperty * This,
            /* [defaultvalue][in] */ long Phases,
            /* [defaultvalue][in] */ double TdK,
            /* [defaultvalue][in] */ double PkPa,
            /* [retval][out] */ VARIANT *pValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetValue )( 
            IScdProperty * This,
            /* [in] */ VARIANT *pValue);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Tag )( 
            IScdProperty * This,
            /* [retval][out] */ BSTR *pName);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Symbol )( 
            IScdProperty * This,
            /* [retval][out] */ BSTR *pName);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Description )( 
            IScdProperty * This,
            /* [retval][out] */ BSTR *pDesc);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Cnv )( 
            IScdProperty * This,
            /* [retval][out] */ IScdConversion **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Index )( 
            IScdProperty * This,
            /* [retval][out] */ LONG *pVal);
        
        END_INTERFACE
    } IScdPropertyVtbl;

    interface IScdProperty
    {
        CONST_VTBL struct IScdPropertyVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdProperty_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdProperty_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdProperty_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdProperty_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdProperty_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdProperty_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdProperty_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IScdProperty_SetObjectInfo(This,What,Mdl,PropNo)	\
    (This)->lpVtbl -> SetObjectInfo(This,What,Mdl,PropNo)

#define IScdProperty_get_Value(This,Phases,TdK,PkPa,pValue)	\
    (This)->lpVtbl -> get_Value(This,Phases,TdK,PkPa,pValue)

#define IScdProperty_SetValue(This,pValue)	\
    (This)->lpVtbl -> SetValue(This,pValue)

#define IScdProperty_get_Tag(This,pName)	\
    (This)->lpVtbl -> get_Tag(This,pName)

#define IScdProperty_get_Symbol(This,pName)	\
    (This)->lpVtbl -> get_Symbol(This,pName)

#define IScdProperty_get_Description(This,pDesc)	\
    (This)->lpVtbl -> get_Description(This,pDesc)

#define IScdProperty_get_Cnv(This,pVal)	\
    (This)->lpVtbl -> get_Cnv(This,pVal)

#define IScdProperty_get_Index(This,pVal)	\
    (This)->lpVtbl -> get_Index(This,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][restricted][id] */ HRESULT STDMETHODCALLTYPE IScdProperty_SetObjectInfo_Proxy( 
    IScdProperty * This,
    /* [in] */ ULONG What,
    /* [in] */ ULONG Mdl,
    /* [in] */ ULONG PropNo);


void __RPC_STUB IScdProperty_SetObjectInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdProperty_get_Value_Proxy( 
    IScdProperty * This,
    /* [defaultvalue][in] */ long Phases,
    /* [defaultvalue][in] */ double TdK,
    /* [defaultvalue][in] */ double PkPa,
    /* [retval][out] */ VARIANT *pValue);


void __RPC_STUB IScdProperty_get_Value_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdProperty_SetValue_Proxy( 
    IScdProperty * This,
    /* [in] */ VARIANT *pValue);


void __RPC_STUB IScdProperty_SetValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdProperty_get_Tag_Proxy( 
    IScdProperty * This,
    /* [retval][out] */ BSTR *pName);


void __RPC_STUB IScdProperty_get_Tag_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdProperty_get_Symbol_Proxy( 
    IScdProperty * This,
    /* [retval][out] */ BSTR *pName);


void __RPC_STUB IScdProperty_get_Symbol_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdProperty_get_Description_Proxy( 
    IScdProperty * This,
    /* [retval][out] */ BSTR *pDesc);


void __RPC_STUB IScdProperty_get_Description_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdProperty_get_Cnv_Proxy( 
    IScdProperty * This,
    /* [retval][out] */ IScdConversion **pVal);


void __RPC_STUB IScdProperty_get_Cnv_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdProperty_get_Index_Proxy( 
    IScdProperty * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB IScdProperty_get_Index_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScdProperty_INTERFACE_DEFINED__ */


#ifndef __IScdProperties_INTERFACE_DEFINED__
#define __IScdProperties_INTERFACE_DEFINED__

/* interface IScdProperties */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdProperties;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B31AE0F2-09DF-4823-BEDB-1DCB5AA7E123")
    IScdProperties : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Property( 
            /* [in] */ VARIANT WhichOne,
            /* [retval][out] */ IScdProperty **pItem) = 0;
        
        virtual /* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ LPUNKNOWN *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetObjectInfo( 
            /* [in] */ ULONG What,
            /* [in] */ ULONG Which) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScdPropertiesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdProperties * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdProperties * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdProperties * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdProperties * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdProperties * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdProperties * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdProperties * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Property )( 
            IScdProperties * This,
            /* [in] */ VARIANT WhichOne,
            /* [retval][out] */ IScdProperty **pItem);
        
        /* [id][restricted][propget] */ HRESULT ( STDMETHODCALLTYPE *get__NewEnum )( 
            IScdProperties * This,
            /* [retval][out] */ LPUNKNOWN *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            IScdProperties * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetObjectInfo )( 
            IScdProperties * This,
            /* [in] */ ULONG What,
            /* [in] */ ULONG Which);
        
        END_INTERFACE
    } IScdPropertiesVtbl;

    interface IScdProperties
    {
        CONST_VTBL struct IScdPropertiesVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdProperties_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdProperties_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdProperties_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdProperties_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdProperties_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdProperties_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdProperties_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IScdProperties_Property(This,WhichOne,pItem)	\
    (This)->lpVtbl -> Property(This,WhichOne,pItem)

#define IScdProperties_get__NewEnum(This,pVal)	\
    (This)->lpVtbl -> get__NewEnum(This,pVal)

#define IScdProperties_get_Count(This,pVal)	\
    (This)->lpVtbl -> get_Count(This,pVal)

#define IScdProperties_SetObjectInfo(This,What,Which)	\
    (This)->lpVtbl -> SetObjectInfo(This,What,Which)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdProperties_Property_Proxy( 
    IScdProperties * This,
    /* [in] */ VARIANT WhichOne,
    /* [retval][out] */ IScdProperty **pItem);


void __RPC_STUB IScdProperties_Property_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE IScdProperties_get__NewEnum_Proxy( 
    IScdProperties * This,
    /* [retval][out] */ LPUNKNOWN *pVal);


void __RPC_STUB IScdProperties_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdProperties_get_Count_Proxy( 
    IScdProperties * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IScdProperties_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdProperties_SetObjectInfo_Proxy( 
    IScdProperties * This,
    /* [in] */ ULONG What,
    /* [in] */ ULONG Which);


void __RPC_STUB IScdProperties_SetObjectInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScdProperties_INTERFACE_DEFINED__ */


#ifndef __IScdSpecieDefns_INTERFACE_DEFINED__
#define __IScdSpecieDefns_INTERFACE_DEFINED__

/* interface IScdSpecieDefns */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdSpecieDefns;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D26883EF-03F7-443a-BBA3-424AF01A740D")
    IScdSpecieDefns : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Specie( 
            /* [in] */ VARIANT WhichOne,
            /* [retval][out] */ IScdSpecieDefn **pItem) = 0;
        
        virtual /* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ LPUNKNOWN *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SomAll( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SomVap( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SomLiq( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SomSol( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SomSL( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Phases( 
            /* [in] */ LONG ReqdPhases,
            /* [retval][out] */ SAFEARRAY * *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MolecularWeights( 
            /* [retval][out] */ SAFEARRAY * *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScdSpecieDefnsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdSpecieDefns * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdSpecieDefns * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdSpecieDefns * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdSpecieDefns * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdSpecieDefns * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdSpecieDefns * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdSpecieDefns * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Specie )( 
            IScdSpecieDefns * This,
            /* [in] */ VARIANT WhichOne,
            /* [retval][out] */ IScdSpecieDefn **pItem);
        
        /* [id][restricted][propget] */ HRESULT ( STDMETHODCALLTYPE *get__NewEnum )( 
            IScdSpecieDefns * This,
            /* [retval][out] */ LPUNKNOWN *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            IScdSpecieDefns * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SomAll )( 
            IScdSpecieDefns * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SomVap )( 
            IScdSpecieDefns * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SomLiq )( 
            IScdSpecieDefns * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SomSol )( 
            IScdSpecieDefns * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SomSL )( 
            IScdSpecieDefns * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Phases )( 
            IScdSpecieDefns * This,
            /* [in] */ LONG ReqdPhases,
            /* [retval][out] */ SAFEARRAY * *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_MolecularWeights )( 
            IScdSpecieDefns * This,
            /* [retval][out] */ SAFEARRAY * *pVal);
        
        END_INTERFACE
    } IScdSpecieDefnsVtbl;

    interface IScdSpecieDefns
    {
        CONST_VTBL struct IScdSpecieDefnsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdSpecieDefns_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdSpecieDefns_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdSpecieDefns_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdSpecieDefns_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdSpecieDefns_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdSpecieDefns_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdSpecieDefns_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IScdSpecieDefns_Specie(This,WhichOne,pItem)	\
    (This)->lpVtbl -> Specie(This,WhichOne,pItem)

#define IScdSpecieDefns_get__NewEnum(This,pVal)	\
    (This)->lpVtbl -> get__NewEnum(This,pVal)

#define IScdSpecieDefns_get_Count(This,pVal)	\
    (This)->lpVtbl -> get_Count(This,pVal)

#define IScdSpecieDefns_get_SomAll(This,pVal)	\
    (This)->lpVtbl -> get_SomAll(This,pVal)

#define IScdSpecieDefns_get_SomVap(This,pVal)	\
    (This)->lpVtbl -> get_SomVap(This,pVal)

#define IScdSpecieDefns_get_SomLiq(This,pVal)	\
    (This)->lpVtbl -> get_SomLiq(This,pVal)

#define IScdSpecieDefns_get_SomSol(This,pVal)	\
    (This)->lpVtbl -> get_SomSol(This,pVal)

#define IScdSpecieDefns_get_SomSL(This,pVal)	\
    (This)->lpVtbl -> get_SomSL(This,pVal)

#define IScdSpecieDefns_get_Phases(This,ReqdPhases,pVal)	\
    (This)->lpVtbl -> get_Phases(This,ReqdPhases,pVal)

#define IScdSpecieDefns_get_MolecularWeights(This,pVal)	\
    (This)->lpVtbl -> get_MolecularWeights(This,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdSpecieDefns_Specie_Proxy( 
    IScdSpecieDefns * This,
    /* [in] */ VARIANT WhichOne,
    /* [retval][out] */ IScdSpecieDefn **pItem);


void __RPC_STUB IScdSpecieDefns_Specie_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE IScdSpecieDefns_get__NewEnum_Proxy( 
    IScdSpecieDefns * This,
    /* [retval][out] */ LPUNKNOWN *pVal);


void __RPC_STUB IScdSpecieDefns_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSpecieDefns_get_Count_Proxy( 
    IScdSpecieDefns * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB IScdSpecieDefns_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSpecieDefns_get_SomAll_Proxy( 
    IScdSpecieDefns * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB IScdSpecieDefns_get_SomAll_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSpecieDefns_get_SomVap_Proxy( 
    IScdSpecieDefns * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB IScdSpecieDefns_get_SomVap_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSpecieDefns_get_SomLiq_Proxy( 
    IScdSpecieDefns * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB IScdSpecieDefns_get_SomLiq_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSpecieDefns_get_SomSol_Proxy( 
    IScdSpecieDefns * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB IScdSpecieDefns_get_SomSol_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSpecieDefns_get_SomSL_Proxy( 
    IScdSpecieDefns * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB IScdSpecieDefns_get_SomSL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSpecieDefns_get_Phases_Proxy( 
    IScdSpecieDefns * This,
    /* [in] */ LONG ReqdPhases,
    /* [retval][out] */ SAFEARRAY * *pVal);


void __RPC_STUB IScdSpecieDefns_get_Phases_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSpecieDefns_get_MolecularWeights_Proxy( 
    IScdSpecieDefns * This,
    /* [retval][out] */ SAFEARRAY * *pVal);


void __RPC_STUB IScdSpecieDefns_get_MolecularWeights_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScdSpecieDefns_INTERFACE_DEFINED__ */


#ifndef __IScdSpecieDefn_INTERFACE_DEFINED__
#define __IScdSpecieDefn_INTERFACE_DEFINED__

/* interface IScdSpecieDefn */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdSpecieDefn;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("80D292B3-AB2B-4c23-9C0E-691D450D3827")
    IScdSpecieDefn : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MolecularWt( 
            /* [retval][out] */ DOUBLE *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Density( 
            /* [defaultvalue][in] */ DOUBLE TdK,
            /* [defaultvalue][in] */ DOUBLE PkPa,
            /* [retval][out] */ DOUBLE *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_msEnthalpy( 
            /* [defaultvalue][in] */ DOUBLE TdK,
            /* [defaultvalue][in] */ DOUBLE PkPa,
            /* [retval][out] */ DOUBLE *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_msEntropy( 
            /* [defaultvalue][in] */ DOUBLE TdK,
            /* [defaultvalue][in] */ DOUBLE PkPa,
            /* [retval][out] */ DOUBLE *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_msCp( 
            /* [defaultvalue][in] */ DOUBLE TdK,
            /* [defaultvalue][in] */ DOUBLE PkPa,
            /* [retval][out] */ DOUBLE *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_DynamicViscosity( 
            /* [defaultvalue][in] */ DOUBLE TdK,
            /* [defaultvalue][in] */ DOUBLE PkPa,
            /* [retval][out] */ DOUBLE *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ThermalConductivity( 
            /* [defaultvalue][in] */ DOUBLE TdK,
            /* [defaultvalue][in] */ DOUBLE PkPa,
            /* [retval][out] */ DOUBLE *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_VapourP( 
            /* [defaultvalue][in] */ DOUBLE TdK,
            /* [retval][out] */ DOUBLE *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_VapourT( 
            /* [defaultvalue][in] */ DOUBLE PkPa,
            /* [retval][out] */ DOUBLE *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Tag( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Symbol( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Index( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][restricted][id][propput] */ HRESULT STDMETHODCALLTYPE put_Index( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Fidelity( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Fidelity( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Properties( 
            /* [retval][out] */ IScdProperties **pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScdSpecieDefnVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdSpecieDefn * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdSpecieDefn * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdSpecieDefn * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdSpecieDefn * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdSpecieDefn * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdSpecieDefn * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdSpecieDefn * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_MolecularWt )( 
            IScdSpecieDefn * This,
            /* [retval][out] */ DOUBLE *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Density )( 
            IScdSpecieDefn * This,
            /* [defaultvalue][in] */ DOUBLE TdK,
            /* [defaultvalue][in] */ DOUBLE PkPa,
            /* [retval][out] */ DOUBLE *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_msEnthalpy )( 
            IScdSpecieDefn * This,
            /* [defaultvalue][in] */ DOUBLE TdK,
            /* [defaultvalue][in] */ DOUBLE PkPa,
            /* [retval][out] */ DOUBLE *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_msEntropy )( 
            IScdSpecieDefn * This,
            /* [defaultvalue][in] */ DOUBLE TdK,
            /* [defaultvalue][in] */ DOUBLE PkPa,
            /* [retval][out] */ DOUBLE *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_msCp )( 
            IScdSpecieDefn * This,
            /* [defaultvalue][in] */ DOUBLE TdK,
            /* [defaultvalue][in] */ DOUBLE PkPa,
            /* [retval][out] */ DOUBLE *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DynamicViscosity )( 
            IScdSpecieDefn * This,
            /* [defaultvalue][in] */ DOUBLE TdK,
            /* [defaultvalue][in] */ DOUBLE PkPa,
            /* [retval][out] */ DOUBLE *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ThermalConductivity )( 
            IScdSpecieDefn * This,
            /* [defaultvalue][in] */ DOUBLE TdK,
            /* [defaultvalue][in] */ DOUBLE PkPa,
            /* [retval][out] */ DOUBLE *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_VapourP )( 
            IScdSpecieDefn * This,
            /* [defaultvalue][in] */ DOUBLE TdK,
            /* [retval][out] */ DOUBLE *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_VapourT )( 
            IScdSpecieDefn * This,
            /* [defaultvalue][in] */ DOUBLE PkPa,
            /* [retval][out] */ DOUBLE *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Tag )( 
            IScdSpecieDefn * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Symbol )( 
            IScdSpecieDefn * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Index )( 
            IScdSpecieDefn * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][restricted][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Index )( 
            IScdSpecieDefn * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Fidelity )( 
            IScdSpecieDefn * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Fidelity )( 
            IScdSpecieDefn * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Properties )( 
            IScdSpecieDefn * This,
            /* [retval][out] */ IScdProperties **pVal);
        
        END_INTERFACE
    } IScdSpecieDefnVtbl;

    interface IScdSpecieDefn
    {
        CONST_VTBL struct IScdSpecieDefnVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdSpecieDefn_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdSpecieDefn_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdSpecieDefn_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdSpecieDefn_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdSpecieDefn_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdSpecieDefn_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdSpecieDefn_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IScdSpecieDefn_get_MolecularWt(This,pVal)	\
    (This)->lpVtbl -> get_MolecularWt(This,pVal)

#define IScdSpecieDefn_get_Density(This,TdK,PkPa,pVal)	\
    (This)->lpVtbl -> get_Density(This,TdK,PkPa,pVal)

#define IScdSpecieDefn_get_msEnthalpy(This,TdK,PkPa,pVal)	\
    (This)->lpVtbl -> get_msEnthalpy(This,TdK,PkPa,pVal)

#define IScdSpecieDefn_get_msEntropy(This,TdK,PkPa,pVal)	\
    (This)->lpVtbl -> get_msEntropy(This,TdK,PkPa,pVal)

#define IScdSpecieDefn_get_msCp(This,TdK,PkPa,pVal)	\
    (This)->lpVtbl -> get_msCp(This,TdK,PkPa,pVal)

#define IScdSpecieDefn_get_DynamicViscosity(This,TdK,PkPa,pVal)	\
    (This)->lpVtbl -> get_DynamicViscosity(This,TdK,PkPa,pVal)

#define IScdSpecieDefn_get_ThermalConductivity(This,TdK,PkPa,pVal)	\
    (This)->lpVtbl -> get_ThermalConductivity(This,TdK,PkPa,pVal)

#define IScdSpecieDefn_get_VapourP(This,TdK,pVal)	\
    (This)->lpVtbl -> get_VapourP(This,TdK,pVal)

#define IScdSpecieDefn_get_VapourT(This,PkPa,pVal)	\
    (This)->lpVtbl -> get_VapourT(This,PkPa,pVal)

#define IScdSpecieDefn_get_Tag(This,pVal)	\
    (This)->lpVtbl -> get_Tag(This,pVal)

#define IScdSpecieDefn_get_Symbol(This,pVal)	\
    (This)->lpVtbl -> get_Symbol(This,pVal)

#define IScdSpecieDefn_get_Index(This,pVal)	\
    (This)->lpVtbl -> get_Index(This,pVal)

#define IScdSpecieDefn_put_Index(This,newVal)	\
    (This)->lpVtbl -> put_Index(This,newVal)

#define IScdSpecieDefn_get_Fidelity(This,pVal)	\
    (This)->lpVtbl -> get_Fidelity(This,pVal)

#define IScdSpecieDefn_put_Fidelity(This,newVal)	\
    (This)->lpVtbl -> put_Fidelity(This,newVal)

#define IScdSpecieDefn_get_Properties(This,pVal)	\
    (This)->lpVtbl -> get_Properties(This,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSpecieDefn_get_MolecularWt_Proxy( 
    IScdSpecieDefn * This,
    /* [retval][out] */ DOUBLE *pVal);


void __RPC_STUB IScdSpecieDefn_get_MolecularWt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSpecieDefn_get_Density_Proxy( 
    IScdSpecieDefn * This,
    /* [defaultvalue][in] */ DOUBLE TdK,
    /* [defaultvalue][in] */ DOUBLE PkPa,
    /* [retval][out] */ DOUBLE *pVal);


void __RPC_STUB IScdSpecieDefn_get_Density_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSpecieDefn_get_msEnthalpy_Proxy( 
    IScdSpecieDefn * This,
    /* [defaultvalue][in] */ DOUBLE TdK,
    /* [defaultvalue][in] */ DOUBLE PkPa,
    /* [retval][out] */ DOUBLE *pVal);


void __RPC_STUB IScdSpecieDefn_get_msEnthalpy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSpecieDefn_get_msEntropy_Proxy( 
    IScdSpecieDefn * This,
    /* [defaultvalue][in] */ DOUBLE TdK,
    /* [defaultvalue][in] */ DOUBLE PkPa,
    /* [retval][out] */ DOUBLE *pVal);


void __RPC_STUB IScdSpecieDefn_get_msEntropy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSpecieDefn_get_msCp_Proxy( 
    IScdSpecieDefn * This,
    /* [defaultvalue][in] */ DOUBLE TdK,
    /* [defaultvalue][in] */ DOUBLE PkPa,
    /* [retval][out] */ DOUBLE *pVal);


void __RPC_STUB IScdSpecieDefn_get_msCp_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSpecieDefn_get_DynamicViscosity_Proxy( 
    IScdSpecieDefn * This,
    /* [defaultvalue][in] */ DOUBLE TdK,
    /* [defaultvalue][in] */ DOUBLE PkPa,
    /* [retval][out] */ DOUBLE *pVal);


void __RPC_STUB IScdSpecieDefn_get_DynamicViscosity_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSpecieDefn_get_ThermalConductivity_Proxy( 
    IScdSpecieDefn * This,
    /* [defaultvalue][in] */ DOUBLE TdK,
    /* [defaultvalue][in] */ DOUBLE PkPa,
    /* [retval][out] */ DOUBLE *pVal);


void __RPC_STUB IScdSpecieDefn_get_ThermalConductivity_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSpecieDefn_get_VapourP_Proxy( 
    IScdSpecieDefn * This,
    /* [defaultvalue][in] */ DOUBLE TdK,
    /* [retval][out] */ DOUBLE *pVal);


void __RPC_STUB IScdSpecieDefn_get_VapourP_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSpecieDefn_get_VapourT_Proxy( 
    IScdSpecieDefn * This,
    /* [defaultvalue][in] */ DOUBLE PkPa,
    /* [retval][out] */ DOUBLE *pVal);


void __RPC_STUB IScdSpecieDefn_get_VapourT_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSpecieDefn_get_Tag_Proxy( 
    IScdSpecieDefn * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IScdSpecieDefn_get_Tag_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSpecieDefn_get_Symbol_Proxy( 
    IScdSpecieDefn * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IScdSpecieDefn_get_Symbol_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSpecieDefn_get_Index_Proxy( 
    IScdSpecieDefn * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IScdSpecieDefn_get_Index_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][restricted][id][propput] */ HRESULT STDMETHODCALLTYPE IScdSpecieDefn_put_Index_Proxy( 
    IScdSpecieDefn * This,
    /* [in] */ long newVal);


void __RPC_STUB IScdSpecieDefn_put_Index_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSpecieDefn_get_Fidelity_Proxy( 
    IScdSpecieDefn * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IScdSpecieDefn_get_Fidelity_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IScdSpecieDefn_put_Fidelity_Proxy( 
    IScdSpecieDefn * This,
    /* [in] */ long newVal);


void __RPC_STUB IScdSpecieDefn_put_Fidelity_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSpecieDefn_get_Properties_Proxy( 
    IScdSpecieDefn * This,
    /* [retval][out] */ IScdProperties **pVal);


void __RPC_STUB IScdSpecieDefn_get_Properties_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScdSpecieDefn_INTERFACE_DEFINED__ */


#ifndef __IScdSpVectorDefn_INTERFACE_DEFINED__
#define __IScdSpVectorDefn_INTERFACE_DEFINED__

/* interface IScdSpVectorDefn */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdSpVectorDefn;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4CB82A2C-01E0-4F0C-93E7-AA5A975B2819")
    IScdSpVectorDefn : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Tag( 
            /* [in] */ LONG Index,
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Symbol( 
            /* [in] */ LONG Index,
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Cnv( 
            /* [in] */ VARIANT Index,
            /* [retval][out] */ IScdConversion **pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScdSpVectorDefnVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdSpVectorDefn * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdSpVectorDefn * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdSpVectorDefn * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdSpVectorDefn * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdSpVectorDefn * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdSpVectorDefn * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdSpVectorDefn * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            IScdSpVectorDefn * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Tag )( 
            IScdSpVectorDefn * This,
            /* [in] */ LONG Index,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Symbol )( 
            IScdSpVectorDefn * This,
            /* [in] */ LONG Index,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Cnv )( 
            IScdSpVectorDefn * This,
            /* [in] */ VARIANT Index,
            /* [retval][out] */ IScdConversion **pVal);
        
        END_INTERFACE
    } IScdSpVectorDefnVtbl;

    interface IScdSpVectorDefn
    {
        CONST_VTBL struct IScdSpVectorDefnVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdSpVectorDefn_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdSpVectorDefn_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdSpVectorDefn_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdSpVectorDefn_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdSpVectorDefn_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdSpVectorDefn_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdSpVectorDefn_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IScdSpVectorDefn_get_Count(This,pVal)	\
    (This)->lpVtbl -> get_Count(This,pVal)

#define IScdSpVectorDefn_get_Tag(This,Index,pVal)	\
    (This)->lpVtbl -> get_Tag(This,Index,pVal)

#define IScdSpVectorDefn_get_Symbol(This,Index,pVal)	\
    (This)->lpVtbl -> get_Symbol(This,Index,pVal)

#define IScdSpVectorDefn_get_Cnv(This,Index,pVal)	\
    (This)->lpVtbl -> get_Cnv(This,Index,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSpVectorDefn_get_Count_Proxy( 
    IScdSpVectorDefn * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB IScdSpVectorDefn_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSpVectorDefn_get_Tag_Proxy( 
    IScdSpVectorDefn * This,
    /* [in] */ LONG Index,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IScdSpVectorDefn_get_Tag_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSpVectorDefn_get_Symbol_Proxy( 
    IScdSpVectorDefn * This,
    /* [in] */ LONG Index,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IScdSpVectorDefn_get_Symbol_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSpVectorDefn_get_Cnv_Proxy( 
    IScdSpVectorDefn * This,
    /* [in] */ VARIANT Index,
    /* [retval][out] */ IScdConversion **pVal);


void __RPC_STUB IScdSpVectorDefn_get_Cnv_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScdSpVectorDefn_INTERFACE_DEFINED__ */


#ifndef __IScdSpVector_INTERFACE_DEFINED__
#define __IScdSpVector_INTERFACE_DEFINED__

/* interface IScdSpVector */
/* [unique][helpstring][dual][oleautomation][uuid][object] */ 


EXTERN_C const IID IID_IScdSpVector;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C978AD3B-4AD3-4290-9C42-EAB344B06786")
    IScdSpVector : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Mass( 
            /* [in] */ LONG Phases,
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Masses( 
            /* [retval][out] */ SAFEARRAY * *SpcMasses) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Masses( 
            /* [in] */ VARIANT ReqdMasses) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SpecieMass( 
            /* [in] */ LONG SpecieIndex,
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_SpecieMass( 
            /* [in] */ LONG SpecieIndex,
            /* [in] */ double newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Temperature( 
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Temperature( 
            /* [in] */ double newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Pressure( 
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Pressure( 
            /* [in] */ double newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Enthalpy( 
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Enthalpy( 
            /* [in] */ double newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Vector( 
            /* [defaultvalue][in] */ VARIANT_BOOL EnthalpyBasis,
            /* [retval][out] */ SAFEARRAY * *Data) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Vector( 
            /* [defaultvalue][in] */ VARIANT_BOOL EnthalpyBasis,
            /* [in] */ VARIANT ReqdData) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Density( 
            /* [in] */ LONG Phases,
            /* [optional][in] */ VARIANT TdK,
            /* [optional][in] */ VARIANT PkPa,
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Volume( 
            /* [in] */ LONG Phases,
            /* [optional][in] */ VARIANT TdK,
            /* [optional][in] */ VARIANT PkPa,
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SaturationP( 
            /* [optional][in] */ VARIANT TdK,
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SaturationT( 
            /* [optional][in] */ VARIANT PkPa,
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Concentration( 
            /* [in] */ LONG SpecieIndex,
            /* [in] */ LONG Phases,
            /* [optional][in] */ VARIANT TdK,
            /* [optional][in] */ VARIANT PkPa,
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_msEnthalpy( 
            /* [defaultvalue][in] */ double RefT,
            /* [defaultvalue][in] */ LONG Phases,
            /* [optional][in] */ VARIANT TdK,
            /* [optional][in] */ VARIANT PkPa,
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_msEntropy( 
            /* [defaultvalue][in] */ double RefT,
            /* [defaultvalue][in] */ LONG Phases,
            /* [optional][in] */ VARIANT TdK,
            /* [optional][in] */ VARIANT PkPa,
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_msCp( 
            /* [defaultvalue][in] */ double RefT,
            /* [defaultvalue][in] */ LONG Phases,
            /* [optional][in] */ VARIANT TdK,
            /* [optional][in] */ VARIANT PkPa,
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Properties( 
            /* [retval][out] */ IScdProperties **pVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Model( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Model( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_SpecieDefns( 
            /* [retval][out] */ IScdSpecieDefns **pVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Defn( 
            /* [retval][out] */ IScdSpVectorDefn **pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScdSpVectorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdSpVector * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdSpVector * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdSpVector * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdSpVector * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdSpVector * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdSpVector * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdSpVector * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Mass )( 
            IScdSpVector * This,
            /* [in] */ LONG Phases,
            /* [retval][out] */ double *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Masses )( 
            IScdSpVector * This,
            /* [retval][out] */ SAFEARRAY * *SpcMasses);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Masses )( 
            IScdSpVector * This,
            /* [in] */ VARIANT ReqdMasses);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SpecieMass )( 
            IScdSpVector * This,
            /* [in] */ LONG SpecieIndex,
            /* [retval][out] */ double *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_SpecieMass )( 
            IScdSpVector * This,
            /* [in] */ LONG SpecieIndex,
            /* [in] */ double newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Temperature )( 
            IScdSpVector * This,
            /* [retval][out] */ double *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Temperature )( 
            IScdSpVector * This,
            /* [in] */ double newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Pressure )( 
            IScdSpVector * This,
            /* [retval][out] */ double *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Pressure )( 
            IScdSpVector * This,
            /* [in] */ double newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Enthalpy )( 
            IScdSpVector * This,
            /* [retval][out] */ double *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Enthalpy )( 
            IScdSpVector * This,
            /* [in] */ double newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Vector )( 
            IScdSpVector * This,
            /* [defaultvalue][in] */ VARIANT_BOOL EnthalpyBasis,
            /* [retval][out] */ SAFEARRAY * *Data);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Vector )( 
            IScdSpVector * This,
            /* [defaultvalue][in] */ VARIANT_BOOL EnthalpyBasis,
            /* [in] */ VARIANT ReqdData);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Density )( 
            IScdSpVector * This,
            /* [in] */ LONG Phases,
            /* [optional][in] */ VARIANT TdK,
            /* [optional][in] */ VARIANT PkPa,
            /* [retval][out] */ double *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Volume )( 
            IScdSpVector * This,
            /* [in] */ LONG Phases,
            /* [optional][in] */ VARIANT TdK,
            /* [optional][in] */ VARIANT PkPa,
            /* [retval][out] */ double *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SaturationP )( 
            IScdSpVector * This,
            /* [optional][in] */ VARIANT TdK,
            /* [retval][out] */ double *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SaturationT )( 
            IScdSpVector * This,
            /* [optional][in] */ VARIANT PkPa,
            /* [retval][out] */ double *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Concentration )( 
            IScdSpVector * This,
            /* [in] */ LONG SpecieIndex,
            /* [in] */ LONG Phases,
            /* [optional][in] */ VARIANT TdK,
            /* [optional][in] */ VARIANT PkPa,
            /* [retval][out] */ double *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_msEnthalpy )( 
            IScdSpVector * This,
            /* [defaultvalue][in] */ double RefT,
            /* [defaultvalue][in] */ LONG Phases,
            /* [optional][in] */ VARIANT TdK,
            /* [optional][in] */ VARIANT PkPa,
            /* [retval][out] */ double *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_msEntropy )( 
            IScdSpVector * This,
            /* [defaultvalue][in] */ double RefT,
            /* [defaultvalue][in] */ LONG Phases,
            /* [optional][in] */ VARIANT TdK,
            /* [optional][in] */ VARIANT PkPa,
            /* [retval][out] */ double *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_msCp )( 
            IScdSpVector * This,
            /* [defaultvalue][in] */ double RefT,
            /* [defaultvalue][in] */ LONG Phases,
            /* [optional][in] */ VARIANT TdK,
            /* [optional][in] */ VARIANT PkPa,
            /* [retval][out] */ double *pVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Properties )( 
            IScdSpVector * This,
            /* [retval][out] */ IScdProperties **pVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Model )( 
            IScdSpVector * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Model )( 
            IScdSpVector * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SpecieDefns )( 
            IScdSpVector * This,
            /* [retval][out] */ IScdSpecieDefns **pVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Defn )( 
            IScdSpVector * This,
            /* [retval][out] */ IScdSpVectorDefn **pVal);
        
        END_INTERFACE
    } IScdSpVectorVtbl;

    interface IScdSpVector
    {
        CONST_VTBL struct IScdSpVectorVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdSpVector_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdSpVector_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdSpVector_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdSpVector_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdSpVector_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdSpVector_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdSpVector_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IScdSpVector_get_Mass(This,Phases,pVal)	\
    (This)->lpVtbl -> get_Mass(This,Phases,pVal)

#define IScdSpVector_get_Masses(This,SpcMasses)	\
    (This)->lpVtbl -> get_Masses(This,SpcMasses)

#define IScdSpVector_put_Masses(This,ReqdMasses)	\
    (This)->lpVtbl -> put_Masses(This,ReqdMasses)

#define IScdSpVector_get_SpecieMass(This,SpecieIndex,pVal)	\
    (This)->lpVtbl -> get_SpecieMass(This,SpecieIndex,pVal)

#define IScdSpVector_put_SpecieMass(This,SpecieIndex,newVal)	\
    (This)->lpVtbl -> put_SpecieMass(This,SpecieIndex,newVal)

#define IScdSpVector_get_Temperature(This,pVal)	\
    (This)->lpVtbl -> get_Temperature(This,pVal)

#define IScdSpVector_put_Temperature(This,newVal)	\
    (This)->lpVtbl -> put_Temperature(This,newVal)

#define IScdSpVector_get_Pressure(This,pVal)	\
    (This)->lpVtbl -> get_Pressure(This,pVal)

#define IScdSpVector_put_Pressure(This,newVal)	\
    (This)->lpVtbl -> put_Pressure(This,newVal)

#define IScdSpVector_get_Enthalpy(This,pVal)	\
    (This)->lpVtbl -> get_Enthalpy(This,pVal)

#define IScdSpVector_put_Enthalpy(This,newVal)	\
    (This)->lpVtbl -> put_Enthalpy(This,newVal)

#define IScdSpVector_get_Vector(This,EnthalpyBasis,Data)	\
    (This)->lpVtbl -> get_Vector(This,EnthalpyBasis,Data)

#define IScdSpVector_put_Vector(This,EnthalpyBasis,ReqdData)	\
    (This)->lpVtbl -> put_Vector(This,EnthalpyBasis,ReqdData)

#define IScdSpVector_get_Density(This,Phases,TdK,PkPa,pVal)	\
    (This)->lpVtbl -> get_Density(This,Phases,TdK,PkPa,pVal)

#define IScdSpVector_get_Volume(This,Phases,TdK,PkPa,pVal)	\
    (This)->lpVtbl -> get_Volume(This,Phases,TdK,PkPa,pVal)

#define IScdSpVector_get_SaturationP(This,TdK,pVal)	\
    (This)->lpVtbl -> get_SaturationP(This,TdK,pVal)

#define IScdSpVector_get_SaturationT(This,PkPa,pVal)	\
    (This)->lpVtbl -> get_SaturationT(This,PkPa,pVal)

#define IScdSpVector_get_Concentration(This,SpecieIndex,Phases,TdK,PkPa,pVal)	\
    (This)->lpVtbl -> get_Concentration(This,SpecieIndex,Phases,TdK,PkPa,pVal)

#define IScdSpVector_get_msEnthalpy(This,RefT,Phases,TdK,PkPa,pVal)	\
    (This)->lpVtbl -> get_msEnthalpy(This,RefT,Phases,TdK,PkPa,pVal)

#define IScdSpVector_get_msEntropy(This,RefT,Phases,TdK,PkPa,pVal)	\
    (This)->lpVtbl -> get_msEntropy(This,RefT,Phases,TdK,PkPa,pVal)

#define IScdSpVector_get_msCp(This,RefT,Phases,TdK,PkPa,pVal)	\
    (This)->lpVtbl -> get_msCp(This,RefT,Phases,TdK,PkPa,pVal)

#define IScdSpVector_get_Properties(This,pVal)	\
    (This)->lpVtbl -> get_Properties(This,pVal)

#define IScdSpVector_get_Model(This,pVal)	\
    (This)->lpVtbl -> get_Model(This,pVal)

#define IScdSpVector_put_Model(This,newVal)	\
    (This)->lpVtbl -> put_Model(This,newVal)

#define IScdSpVector_get_SpecieDefns(This,pVal)	\
    (This)->lpVtbl -> get_SpecieDefns(This,pVal)

#define IScdSpVector_get_Defn(This,pVal)	\
    (This)->lpVtbl -> get_Defn(This,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSpVector_get_Mass_Proxy( 
    IScdSpVector * This,
    /* [in] */ LONG Phases,
    /* [retval][out] */ double *pVal);


void __RPC_STUB IScdSpVector_get_Mass_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSpVector_get_Masses_Proxy( 
    IScdSpVector * This,
    /* [retval][out] */ SAFEARRAY * *SpcMasses);


void __RPC_STUB IScdSpVector_get_Masses_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IScdSpVector_put_Masses_Proxy( 
    IScdSpVector * This,
    /* [in] */ VARIANT ReqdMasses);


void __RPC_STUB IScdSpVector_put_Masses_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSpVector_get_SpecieMass_Proxy( 
    IScdSpVector * This,
    /* [in] */ LONG SpecieIndex,
    /* [retval][out] */ double *pVal);


void __RPC_STUB IScdSpVector_get_SpecieMass_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IScdSpVector_put_SpecieMass_Proxy( 
    IScdSpVector * This,
    /* [in] */ LONG SpecieIndex,
    /* [in] */ double newVal);


void __RPC_STUB IScdSpVector_put_SpecieMass_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSpVector_get_Temperature_Proxy( 
    IScdSpVector * This,
    /* [retval][out] */ double *pVal);


void __RPC_STUB IScdSpVector_get_Temperature_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IScdSpVector_put_Temperature_Proxy( 
    IScdSpVector * This,
    /* [in] */ double newVal);


void __RPC_STUB IScdSpVector_put_Temperature_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSpVector_get_Pressure_Proxy( 
    IScdSpVector * This,
    /* [retval][out] */ double *pVal);


void __RPC_STUB IScdSpVector_get_Pressure_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IScdSpVector_put_Pressure_Proxy( 
    IScdSpVector * This,
    /* [in] */ double newVal);


void __RPC_STUB IScdSpVector_put_Pressure_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSpVector_get_Enthalpy_Proxy( 
    IScdSpVector * This,
    /* [retval][out] */ double *pVal);


void __RPC_STUB IScdSpVector_get_Enthalpy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IScdSpVector_put_Enthalpy_Proxy( 
    IScdSpVector * This,
    /* [in] */ double newVal);


void __RPC_STUB IScdSpVector_put_Enthalpy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSpVector_get_Vector_Proxy( 
    IScdSpVector * This,
    /* [defaultvalue][in] */ VARIANT_BOOL EnthalpyBasis,
    /* [retval][out] */ SAFEARRAY * *Data);


void __RPC_STUB IScdSpVector_get_Vector_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IScdSpVector_put_Vector_Proxy( 
    IScdSpVector * This,
    /* [defaultvalue][in] */ VARIANT_BOOL EnthalpyBasis,
    /* [in] */ VARIANT ReqdData);


void __RPC_STUB IScdSpVector_put_Vector_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSpVector_get_Density_Proxy( 
    IScdSpVector * This,
    /* [in] */ LONG Phases,
    /* [optional][in] */ VARIANT TdK,
    /* [optional][in] */ VARIANT PkPa,
    /* [retval][out] */ double *pVal);


void __RPC_STUB IScdSpVector_get_Density_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSpVector_get_Volume_Proxy( 
    IScdSpVector * This,
    /* [in] */ LONG Phases,
    /* [optional][in] */ VARIANT TdK,
    /* [optional][in] */ VARIANT PkPa,
    /* [retval][out] */ double *pVal);


void __RPC_STUB IScdSpVector_get_Volume_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSpVector_get_SaturationP_Proxy( 
    IScdSpVector * This,
    /* [optional][in] */ VARIANT TdK,
    /* [retval][out] */ double *pVal);


void __RPC_STUB IScdSpVector_get_SaturationP_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSpVector_get_SaturationT_Proxy( 
    IScdSpVector * This,
    /* [optional][in] */ VARIANT PkPa,
    /* [retval][out] */ double *pVal);


void __RPC_STUB IScdSpVector_get_SaturationT_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSpVector_get_Concentration_Proxy( 
    IScdSpVector * This,
    /* [in] */ LONG SpecieIndex,
    /* [in] */ LONG Phases,
    /* [optional][in] */ VARIANT TdK,
    /* [optional][in] */ VARIANT PkPa,
    /* [retval][out] */ double *pVal);


void __RPC_STUB IScdSpVector_get_Concentration_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSpVector_get_msEnthalpy_Proxy( 
    IScdSpVector * This,
    /* [defaultvalue][in] */ double RefT,
    /* [defaultvalue][in] */ LONG Phases,
    /* [optional][in] */ VARIANT TdK,
    /* [optional][in] */ VARIANT PkPa,
    /* [retval][out] */ double *pVal);


void __RPC_STUB IScdSpVector_get_msEnthalpy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSpVector_get_msEntropy_Proxy( 
    IScdSpVector * This,
    /* [defaultvalue][in] */ double RefT,
    /* [defaultvalue][in] */ LONG Phases,
    /* [optional][in] */ VARIANT TdK,
    /* [optional][in] */ VARIANT PkPa,
    /* [retval][out] */ double *pVal);


void __RPC_STUB IScdSpVector_get_msEntropy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSpVector_get_msCp_Proxy( 
    IScdSpVector * This,
    /* [defaultvalue][in] */ double RefT,
    /* [defaultvalue][in] */ LONG Phases,
    /* [optional][in] */ VARIANT TdK,
    /* [optional][in] */ VARIANT PkPa,
    /* [retval][out] */ double *pVal);


void __RPC_STUB IScdSpVector_get_msCp_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IScdSpVector_get_Properties_Proxy( 
    IScdSpVector * This,
    /* [retval][out] */ IScdProperties **pVal);


void __RPC_STUB IScdSpVector_get_Properties_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IScdSpVector_get_Model_Proxy( 
    IScdSpVector * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IScdSpVector_get_Model_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IScdSpVector_put_Model_Proxy( 
    IScdSpVector * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IScdSpVector_put_Model_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IScdSpVector_get_SpecieDefns_Proxy( 
    IScdSpVector * This,
    /* [retval][out] */ IScdSpecieDefns **pVal);


void __RPC_STUB IScdSpVector_get_SpecieDefns_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IScdSpVector_get_Defn_Proxy( 
    IScdSpVector * This,
    /* [retval][out] */ IScdSpVectorDefn **pVal);


void __RPC_STUB IScdSpVector_get_Defn_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScdSpVector_INTERFACE_DEFINED__ */


#ifndef __IScdQuality_INTERFACE_DEFINED__
#define __IScdQuality_INTERFACE_DEFINED__

/* interface IScdQuality */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdQuality;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("8314E7E3-14C4-4B24-8596-5D1590C8753E")
    IScdQuality : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IScdQualityVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdQuality * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdQuality * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdQuality * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdQuality * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdQuality * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdQuality * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdQuality * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IScdQualityVtbl;

    interface IScdQuality
    {
        CONST_VTBL struct IScdQualityVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdQuality_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdQuality_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdQuality_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdQuality_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdQuality_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdQuality_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdQuality_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IScdQuality_INTERFACE_DEFINED__ */


#ifndef __IScdQualities_INTERFACE_DEFINED__
#define __IScdQualities_INTERFACE_DEFINED__

/* interface IScdQualities */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdQualities;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("8D211EFF-095B-483D-86F3-3C17526CED9B")
    IScdQualities : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IScdQualitiesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdQualities * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdQualities * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdQualities * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdQualities * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdQualities * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdQualities * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdQualities * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IScdQualitiesVtbl;

    interface IScdQualities
    {
        CONST_VTBL struct IScdQualitiesVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdQualities_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdQualities_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdQualities_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdQualities_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdQualities_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdQualities_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdQualities_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IScdQualities_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_ScdLicenseMdl;

#ifdef __cplusplus

class DECLSPEC_UUID("DBC6EB45-6281-43AA-9B8F-39980BF00359")
ScdLicenseMdl;
#endif

EXTERN_C const CLSID CLSID_ScdSpecieDefns;

#ifdef __cplusplus

class DECLSPEC_UUID("D78E12EA-92A6-4FB6-944C-6B55CE98BC14")
ScdSpecieDefns;
#endif

EXTERN_C const CLSID CLSID_ScdSpecieDefn;

#ifdef __cplusplus

class DECLSPEC_UUID("59E7E070-1CE9-49DE-B079-52715F766313")
ScdSpecieDefn;
#endif

EXTERN_C const CLSID CLSID_ScdProperty;

#ifdef __cplusplus

class DECLSPEC_UUID("2BD292B6-D61B-4274-9BAE-2D6BFC6D9580")
ScdProperty;
#endif

EXTERN_C const CLSID CLSID_ScdConversions;

#ifdef __cplusplus

class DECLSPEC_UUID("081CAA3C-36DF-45D7-9D06-B1A36B435811")
ScdConversions;
#endif

EXTERN_C const CLSID CLSID_ScdConversion;

#ifdef __cplusplus

class DECLSPEC_UUID("F37EFC34-1557-4189-A284-457526C92E38")
ScdConversion;
#endif

EXTERN_C const CLSID CLSID_ScdProperties;

#ifdef __cplusplus

class DECLSPEC_UUID("1EAE4D40-5796-45A8-B785-AFD306F84041")
ScdProperties;
#endif

EXTERN_C const CLSID CLSID_ScdSpVector;

#ifdef __cplusplus

class DECLSPEC_UUID("A5AB3C07-C1FB-4C5B-A469-DBD2659E0BBE")
ScdSpVector;
#endif

EXTERN_C const CLSID CLSID_ScdQuality;

#ifdef __cplusplus

class DECLSPEC_UUID("BE8AB028-3F02-465D-8691-7C1CC68511FF")
ScdQuality;
#endif

EXTERN_C const CLSID CLSID_ScdQualities;

#ifdef __cplusplus

class DECLSPEC_UUID("8BA848DE-9A9D-4456-A724-8966B05E2A8C")
ScdQualities;
#endif

EXTERN_C const CLSID CLSID_ScdSpVectorDefn;

#ifdef __cplusplus

class DECLSPEC_UUID("148EE6E0-4040-4324-8EE4-151FC305225F")
ScdSpVectorDefn;
#endif
#endif /* __ScdMdl_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


