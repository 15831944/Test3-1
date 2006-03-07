

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0361 */
/* at Tue Mar 07 11:45:33 2006
 */
/* Compiler settings for .\SysCADMarshal.odl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
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

#ifndef __SysCADMarshal_i_h__
#define __SysCADMarshal_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IScdMarshal_FWD_DEFINED__
#define __IScdMarshal_FWD_DEFINED__
typedef interface IScdMarshal IScdMarshal;
#endif 	/* __IScdMarshal_FWD_DEFINED__ */


#ifndef __IScdExecObject_FWD_DEFINED__
#define __IScdExecObject_FWD_DEFINED__
typedef interface IScdExecObject IScdExecObject;
#endif 	/* __IScdExecObject_FWD_DEFINED__ */


#ifndef ___IScdMarshalEvents_FWD_DEFINED__
#define ___IScdMarshalEvents_FWD_DEFINED__
typedef interface _IScdMarshalEvents _IScdMarshalEvents;
#endif 	/* ___IScdMarshalEvents_FWD_DEFINED__ */


#ifndef ___IScdExecObjectEvents_FWD_DEFINED__
#define ___IScdExecObjectEvents_FWD_DEFINED__
typedef interface _IScdExecObjectEvents _IScdExecObjectEvents;
#endif 	/* ___IScdExecObjectEvents_FWD_DEFINED__ */


#ifndef __ISysCADMarshal_FWD_DEFINED__
#define __ISysCADMarshal_FWD_DEFINED__
typedef interface ISysCADMarshal ISysCADMarshal;
#endif 	/* __ISysCADMarshal_FWD_DEFINED__ */


#ifndef __Document_FWD_DEFINED__
#define __Document_FWD_DEFINED__

#ifdef __cplusplus
typedef class Document Document;
#else
typedef struct Document Document;
#endif /* __cplusplus */

#endif 	/* __Document_FWD_DEFINED__ */


#ifndef __ScdMarshal_FWD_DEFINED__
#define __ScdMarshal_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdMarshal ScdMarshal;
#else
typedef struct ScdMarshal ScdMarshal;
#endif /* __cplusplus */

#endif 	/* __ScdMarshal_FWD_DEFINED__ */


#ifndef ___IScdMarshalEvents_FWD_DEFINED__
#define ___IScdMarshalEvents_FWD_DEFINED__
typedef interface _IScdMarshalEvents _IScdMarshalEvents;
#endif 	/* ___IScdMarshalEvents_FWD_DEFINED__ */


#ifndef ___IScdExecObjectEvents_FWD_DEFINED__
#define ___IScdExecObjectEvents_FWD_DEFINED__
typedef interface _IScdExecObjectEvents _IScdExecObjectEvents;
#endif 	/* ___IScdExecObjectEvents_FWD_DEFINED__ */


#ifndef __IScdMarshal_FWD_DEFINED__
#define __IScdMarshal_FWD_DEFINED__
typedef interface IScdMarshal IScdMarshal;
#endif 	/* __IScdMarshal_FWD_DEFINED__ */


#ifndef __IScdExecObject_FWD_DEFINED__
#define __IScdExecObject_FWD_DEFINED__
typedef interface IScdExecObject IScdExecObject;
#endif 	/* __IScdExecObject_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

/* interface __MIDL_itf_SysCADMarshal_0000 */
/* [local] */ 

typedef 
enum tagScd_RunTerms
    {	Scd_RTRM_None	= 0,
	Scd_RTRM_For	= Scd_RTRM_None + 1,
	Scd_RTRM_Until	= Scd_RTRM_For + 1,
	Scd_RTRM_SteadyState	= Scd_RTRM_Until + 1,
	Scd_RTRM_ForIters	= Scd_RTRM_SteadyState + 1
    } 	Scd_RunTerms;

typedef 
enum tagSCD_XIOAction
    {	Scd_XIO_Null	= 0,
	Scd_XIO_In	= Scd_XIO_Null + 1,
	Scd_XIO_Out	= Scd_XIO_In + 1,
	Scd_XIO_InOut	= Scd_XIO_Out + 1
    } 	SCD_XIOAction;


const DWORD SCD_NOCOMPARE           = 0x00000040;
const DWORD SCD_NAN_OK              = 0x00000080;
const DWORD SCD_NOFILE              = 0x00000100;
const DWORD SCD_NOVIEW              = 0x00000200;
const DWORD SCD_NOSNAPSHOT          = 0x00000400;
const DWORD SCD_NOSCENARIO          = 0x00000800;
const DWORD SCD_PARAM               = 0x00001000;
const DWORD SCD_WRITEPROTECT        = 0x00002000;
const DWORD SCD_HIDDEN              = 0x00004000;
const DWORD SCD_AFFECTSSTRUCT       = 0x00008000;
const DWORD SCD_PARAMCONSTRUCT      = 0x00010000;
const DWORD SCD_KEEPHISTORY         = 0x00020000;
const DWORD SCD_TREENODE            = 0x00040000;
const DWORD SCD_TREECLOSED          = 0x00080000;
const DWORD SCD_BUTTON              = 0x00100000;
const DWORD SCD_DUPHANDLES_OK       = 0x00200000;
const DWORD SCD_MULTILINESTR        = 0x00400000;
const DWORD SCD_PARAMSTOPPED        = 0x00800000;
const DWORD SCD_INERROR             = 0x01000000;
const DWORD SCD_DRIVERTAG           = 0x02000000;
const DWORD SCD_CHECKBOX            = 0x04000000;
const DWORD SCD_TAGPARM             = 0x08000000;
const DWORD SCD_FUNCTPARM           = 0x10000000;
const DWORD SCD_SETONCHANGE         = 0x20000000;
const DWORD SCD_ARCHIVETAG          = 0x40000000;
const DWORD SCD_NOPUTSHORTCUT       = 0x80000000;
typedef struct tagSCD_EXECUTIONCONTROL
    {
    BOOL m_bRealTime;
    BOOL m_bSyncWithClock;
    BOOL m_bHoldAdv;
    double m_dTime;
    double m_dStepSize;
    double m_dStepSizeMax;
    double m_dStepSizeNxt;
    double m_dStopTime;
    double m_dRealTimeMult;
    long m_nSteps;
    long m_nMaxSteps;
    Scd_RunTerms m_iRunTerm;
    double m_dTimeUntilStop;
    } 	SCD_EXECUTIONCONTROL;

typedef struct tagSCD_EXECRETURN
    {
    double m_Time;
    double m_StepSizeNxt;
    } 	SCD_EXECRETURN;

typedef struct tagSCD_TAGITEM
    {
    /* [string] */ LPSTR m_szTag;
    /* [string] */ LPSTR m_szCnv;
    VARTYPE m_vtType;
    DWORD m_dwItemIndex;
    DWORD m_dwHandle;
    DWORD m_dwOptions;
    SCD_XIOAction m_dwAction;
    } 	SCD_TAGITEM;

typedef struct tagSCD_DATAITEM
    {
    DWORD m_dwItemIndex;
    DWORD m_dwHandle;
    DWORD m_dwOptions;
    VARIANT m_vValue;
    } 	SCD_DATAITEM;



extern RPC_IF_HANDLE __MIDL_itf_SysCADMarshal_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_SysCADMarshal_0000_v0_0_s_ifspec;

#ifndef __IScdMarshal_INTERFACE_DEFINED__
#define __IScdMarshal_INTERFACE_DEFINED__

/* interface IScdMarshal */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IScdMarshal;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("057E8131-C4D6-4E0F-BACD-7615743704B1")
    IScdMarshal : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Open( 
            /* [string][in] */ LPCWSTR Filename,
            /* [out] */ long *Return) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Close( 
            /* [in] */ VARIANT_BOOL Save,
            /* [out] */ long *Return) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScdMarshalVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdMarshal * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdMarshal * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdMarshal * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Open )( 
            IScdMarshal * This,
            /* [string][in] */ LPCWSTR Filename,
            /* [out] */ long *Return);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Close )( 
            IScdMarshal * This,
            /* [in] */ VARIANT_BOOL Save,
            /* [out] */ long *Return);
        
        END_INTERFACE
    } IScdMarshalVtbl;

    interface IScdMarshal
    {
        CONST_VTBL struct IScdMarshalVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdMarshal_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdMarshal_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdMarshal_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdMarshal_Open(This,Filename,Return)	\
    (This)->lpVtbl -> Open(This,Filename,Return)

#define IScdMarshal_Close(This,Save,Return)	\
    (This)->lpVtbl -> Close(This,Save,Return)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IScdMarshal_Open_Proxy( 
    IScdMarshal * This,
    /* [string][in] */ LPCWSTR Filename,
    /* [out] */ long *Return);


void __RPC_STUB IScdMarshal_Open_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IScdMarshal_Close_Proxy( 
    IScdMarshal * This,
    /* [in] */ VARIANT_BOOL Save,
    /* [out] */ long *Return);


void __RPC_STUB IScdMarshal_Close_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScdMarshal_INTERFACE_DEFINED__ */


#ifndef __IScdExecObject_INTERFACE_DEFINED__
#define __IScdExecObject_INTERFACE_DEFINED__

/* interface IScdExecObject */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IScdExecObject;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("426AB2D6-83E3-47C3-8C85-F6DFA704C01D")
    IScdExecObject : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Start( 
            /* [in] */ SCD_EXECUTIONCONTROL *pTmCB,
            /* [out][in] */ DWORD *pdwReturn) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Execute( 
            /* [in] */ SCD_EXECUTIONCONTROL *pTmCB,
            /* [out][in] */ SCD_EXECRETURN *pEORet,
            /* [out][in] */ DWORD *pdwReturn) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Stop( 
            /* [in] */ SCD_EXECUTIONCONTROL *pTmCB,
            /* [out][in] */ DWORD *pdwReturn) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE QueryTime( 
            /* [in] */ SCD_EXECUTIONCONTROL *pTmCB,
            /* [out][in] */ double *pdTimeReqd,
            /* [out][in] */ double *pdDeltaTReqd) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE QuerySubscriptionTagsRequired( 
            /* [in] */ VARIANT_BOOL Start,
            /* [out][in] */ DWORD *dwMaxCount,
            /* [out][in] */ DWORD *dwCount,
            /* [length_is][length_is][size_is][size_is][out][in] */ SCD_TAGITEM **pTags) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE QuerySubscriptionTagsAvailable( 
            /* [out][in] */ DWORD *dwMaxCount,
            /* [out][in] */ DWORD *dwCount,
            /* [length_is][length_is][size_is][size_is][out][in] */ SCD_TAGITEM **pTags) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ReadSubscriptionData( 
            /* [in] */ VARIANT_BOOL Start,
            /* [in] */ VARIANT_BOOL ReadAll,
            /* [out][in] */ DWORD *dwMaxCount,
            /* [out][in] */ DWORD *dwCount,
            /* [length_is][length_is][size_is][size_is][out][in] */ SCD_DATAITEM **pData) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE WriteSubscriptionData( 
            /* [out][in] */ DWORD *dwMaxCount,
            /* [out][in] */ DWORD *dwCount,
            /* [length_is][length_is][size_is][size_is][out][in] */ SCD_DATAITEM **pData) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ReadTaggedItems( 
            /* [in] */ DWORD dwCount,
            /* [size_is][in] */ SCD_TAGITEM *pTags,
            /* [size_is][out] */ SCD_DATAITEM *pData,
            /* [size_is][out] */ DWORD *pReturn) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE WriteTaggedItems( 
            /* [in] */ DWORD dwCount,
            /* [size_is][in] */ SCD_TAGITEM *pTags,
            /* [size_is][out][in] */ SCD_DATAITEM *pData,
            /* [size_is][out] */ DWORD *pReturn) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScdExecObjectVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdExecObject * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdExecObject * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdExecObject * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Start )( 
            IScdExecObject * This,
            /* [in] */ SCD_EXECUTIONCONTROL *pTmCB,
            /* [out][in] */ DWORD *pdwReturn);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Execute )( 
            IScdExecObject * This,
            /* [in] */ SCD_EXECUTIONCONTROL *pTmCB,
            /* [out][in] */ SCD_EXECRETURN *pEORet,
            /* [out][in] */ DWORD *pdwReturn);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Stop )( 
            IScdExecObject * This,
            /* [in] */ SCD_EXECUTIONCONTROL *pTmCB,
            /* [out][in] */ DWORD *pdwReturn);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *QueryTime )( 
            IScdExecObject * This,
            /* [in] */ SCD_EXECUTIONCONTROL *pTmCB,
            /* [out][in] */ double *pdTimeReqd,
            /* [out][in] */ double *pdDeltaTReqd);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *QuerySubscriptionTagsRequired )( 
            IScdExecObject * This,
            /* [in] */ VARIANT_BOOL Start,
            /* [out][in] */ DWORD *dwMaxCount,
            /* [out][in] */ DWORD *dwCount,
            /* [length_is][length_is][size_is][size_is][out][in] */ SCD_TAGITEM **pTags);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *QuerySubscriptionTagsAvailable )( 
            IScdExecObject * This,
            /* [out][in] */ DWORD *dwMaxCount,
            /* [out][in] */ DWORD *dwCount,
            /* [length_is][length_is][size_is][size_is][out][in] */ SCD_TAGITEM **pTags);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *ReadSubscriptionData )( 
            IScdExecObject * This,
            /* [in] */ VARIANT_BOOL Start,
            /* [in] */ VARIANT_BOOL ReadAll,
            /* [out][in] */ DWORD *dwMaxCount,
            /* [out][in] */ DWORD *dwCount,
            /* [length_is][length_is][size_is][size_is][out][in] */ SCD_DATAITEM **pData);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *WriteSubscriptionData )( 
            IScdExecObject * This,
            /* [out][in] */ DWORD *dwMaxCount,
            /* [out][in] */ DWORD *dwCount,
            /* [length_is][length_is][size_is][size_is][out][in] */ SCD_DATAITEM **pData);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *ReadTaggedItems )( 
            IScdExecObject * This,
            /* [in] */ DWORD dwCount,
            /* [size_is][in] */ SCD_TAGITEM *pTags,
            /* [size_is][out] */ SCD_DATAITEM *pData,
            /* [size_is][out] */ DWORD *pReturn);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *WriteTaggedItems )( 
            IScdExecObject * This,
            /* [in] */ DWORD dwCount,
            /* [size_is][in] */ SCD_TAGITEM *pTags,
            /* [size_is][out][in] */ SCD_DATAITEM *pData,
            /* [size_is][out] */ DWORD *pReturn);
        
        END_INTERFACE
    } IScdExecObjectVtbl;

    interface IScdExecObject
    {
        CONST_VTBL struct IScdExecObjectVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdExecObject_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdExecObject_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdExecObject_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdExecObject_Start(This,pTmCB,pdwReturn)	\
    (This)->lpVtbl -> Start(This,pTmCB,pdwReturn)

#define IScdExecObject_Execute(This,pTmCB,pEORet,pdwReturn)	\
    (This)->lpVtbl -> Execute(This,pTmCB,pEORet,pdwReturn)

#define IScdExecObject_Stop(This,pTmCB,pdwReturn)	\
    (This)->lpVtbl -> Stop(This,pTmCB,pdwReturn)

#define IScdExecObject_QueryTime(This,pTmCB,pdTimeReqd,pdDeltaTReqd)	\
    (This)->lpVtbl -> QueryTime(This,pTmCB,pdTimeReqd,pdDeltaTReqd)

#define IScdExecObject_QuerySubscriptionTagsRequired(This,Start,dwMaxCount,dwCount,pTags)	\
    (This)->lpVtbl -> QuerySubscriptionTagsRequired(This,Start,dwMaxCount,dwCount,pTags)

#define IScdExecObject_QuerySubscriptionTagsAvailable(This,dwMaxCount,dwCount,pTags)	\
    (This)->lpVtbl -> QuerySubscriptionTagsAvailable(This,dwMaxCount,dwCount,pTags)

#define IScdExecObject_ReadSubscriptionData(This,Start,ReadAll,dwMaxCount,dwCount,pData)	\
    (This)->lpVtbl -> ReadSubscriptionData(This,Start,ReadAll,dwMaxCount,dwCount,pData)

#define IScdExecObject_WriteSubscriptionData(This,dwMaxCount,dwCount,pData)	\
    (This)->lpVtbl -> WriteSubscriptionData(This,dwMaxCount,dwCount,pData)

#define IScdExecObject_ReadTaggedItems(This,dwCount,pTags,pData,pReturn)	\
    (This)->lpVtbl -> ReadTaggedItems(This,dwCount,pTags,pData,pReturn)

#define IScdExecObject_WriteTaggedItems(This,dwCount,pTags,pData,pReturn)	\
    (This)->lpVtbl -> WriteTaggedItems(This,dwCount,pTags,pData,pReturn)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IScdExecObject_Start_Proxy( 
    IScdExecObject * This,
    /* [in] */ SCD_EXECUTIONCONTROL *pTmCB,
    /* [out][in] */ DWORD *pdwReturn);


void __RPC_STUB IScdExecObject_Start_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IScdExecObject_Execute_Proxy( 
    IScdExecObject * This,
    /* [in] */ SCD_EXECUTIONCONTROL *pTmCB,
    /* [out][in] */ SCD_EXECRETURN *pEORet,
    /* [out][in] */ DWORD *pdwReturn);


void __RPC_STUB IScdExecObject_Execute_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IScdExecObject_Stop_Proxy( 
    IScdExecObject * This,
    /* [in] */ SCD_EXECUTIONCONTROL *pTmCB,
    /* [out][in] */ DWORD *pdwReturn);


void __RPC_STUB IScdExecObject_Stop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IScdExecObject_QueryTime_Proxy( 
    IScdExecObject * This,
    /* [in] */ SCD_EXECUTIONCONTROL *pTmCB,
    /* [out][in] */ double *pdTimeReqd,
    /* [out][in] */ double *pdDeltaTReqd);


void __RPC_STUB IScdExecObject_QueryTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IScdExecObject_QuerySubscriptionTagsRequired_Proxy( 
    IScdExecObject * This,
    /* [in] */ VARIANT_BOOL Start,
    /* [out][in] */ DWORD *dwMaxCount,
    /* [out][in] */ DWORD *dwCount,
    /* [length_is][length_is][size_is][size_is][out][in] */ SCD_TAGITEM **pTags);


void __RPC_STUB IScdExecObject_QuerySubscriptionTagsRequired_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IScdExecObject_QuerySubscriptionTagsAvailable_Proxy( 
    IScdExecObject * This,
    /* [out][in] */ DWORD *dwMaxCount,
    /* [out][in] */ DWORD *dwCount,
    /* [length_is][length_is][size_is][size_is][out][in] */ SCD_TAGITEM **pTags);


void __RPC_STUB IScdExecObject_QuerySubscriptionTagsAvailable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IScdExecObject_ReadSubscriptionData_Proxy( 
    IScdExecObject * This,
    /* [in] */ VARIANT_BOOL Start,
    /* [in] */ VARIANT_BOOL ReadAll,
    /* [out][in] */ DWORD *dwMaxCount,
    /* [out][in] */ DWORD *dwCount,
    /* [length_is][length_is][size_is][size_is][out][in] */ SCD_DATAITEM **pData);


void __RPC_STUB IScdExecObject_ReadSubscriptionData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IScdExecObject_WriteSubscriptionData_Proxy( 
    IScdExecObject * This,
    /* [out][in] */ DWORD *dwMaxCount,
    /* [out][in] */ DWORD *dwCount,
    /* [length_is][length_is][size_is][size_is][out][in] */ SCD_DATAITEM **pData);


void __RPC_STUB IScdExecObject_WriteSubscriptionData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IScdExecObject_ReadTaggedItems_Proxy( 
    IScdExecObject * This,
    /* [in] */ DWORD dwCount,
    /* [size_is][in] */ SCD_TAGITEM *pTags,
    /* [size_is][out] */ SCD_DATAITEM *pData,
    /* [size_is][out] */ DWORD *pReturn);


void __RPC_STUB IScdExecObject_ReadTaggedItems_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IScdExecObject_WriteTaggedItems_Proxy( 
    IScdExecObject * This,
    /* [in] */ DWORD dwCount,
    /* [size_is][in] */ SCD_TAGITEM *pTags,
    /* [size_is][out][in] */ SCD_DATAITEM *pData,
    /* [size_is][out] */ DWORD *pReturn);


void __RPC_STUB IScdExecObject_WriteTaggedItems_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScdExecObject_INTERFACE_DEFINED__ */


#ifndef ___IScdMarshalEvents_INTERFACE_DEFINED__
#define ___IScdMarshalEvents_INTERFACE_DEFINED__

/* interface _IScdMarshalEvents */
/* [helpstring][uuid][object] */ 


EXTERN_C const IID IID__IScdMarshalEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B2C38331-751E-47C1-B375-4CBDFD0D938C")
    _IScdMarshalEvents : public IUnknown
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct _IScdMarshalEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IScdMarshalEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IScdMarshalEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IScdMarshalEvents * This);
        
        END_INTERFACE
    } _IScdMarshalEventsVtbl;

    interface _IScdMarshalEvents
    {
        CONST_VTBL struct _IScdMarshalEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IScdMarshalEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _IScdMarshalEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _IScdMarshalEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* ___IScdMarshalEvents_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_SysCADMarshal_0117 */
/* [local] */ 

#define SCDTAG_Exists        0x0001
#define SCDTAG_DriverTag     0x0002
#define SCDTAG_SolverTag     0x0004
#define SCDTAG_ArchiveTag    0x0008
#define SCDTAG_NumData       0x0010
#define SCDTAG_StrData       0x0020
#define SCDTAG_IsParm        0x0040
#define SCDTAG_BadCnvs       0x0080


extern RPC_IF_HANDLE __MIDL_itf_SysCADMarshal_0117_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_SysCADMarshal_0117_v0_0_s_ifspec;

#ifndef ___IScdExecObjectEvents_INTERFACE_DEFINED__
#define ___IScdExecObjectEvents_INTERFACE_DEFINED__

/* interface _IScdExecObjectEvents */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID__IScdExecObjectEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4DD2D65E-E0F4-4B8E-AE8D-67F442AF9869")
    _IScdExecObjectEvents : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ReadTaggedItems( 
            /* [in] */ DWORD dwCount,
            /* [size_is][size_is][in] */ SCD_TAGITEM **pTags,
            /* [size_is][size_is][out][in] */ SCD_DATAITEM **pData,
            /* [size_is][size_is][out][in] */ DWORD **pReturn) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct _IScdExecObjectEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IScdExecObjectEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IScdExecObjectEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IScdExecObjectEvents * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *ReadTaggedItems )( 
            _IScdExecObjectEvents * This,
            /* [in] */ DWORD dwCount,
            /* [size_is][size_is][in] */ SCD_TAGITEM **pTags,
            /* [size_is][size_is][out][in] */ SCD_DATAITEM **pData,
            /* [size_is][size_is][out][in] */ DWORD **pReturn);
        
        END_INTERFACE
    } _IScdExecObjectEventsVtbl;

    interface _IScdExecObjectEvents
    {
        CONST_VTBL struct _IScdExecObjectEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IScdExecObjectEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _IScdExecObjectEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _IScdExecObjectEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _IScdExecObjectEvents_ReadTaggedItems(This,dwCount,pTags,pData,pReturn)	\
    (This)->lpVtbl -> ReadTaggedItems(This,dwCount,pTags,pData,pReturn)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE _IScdExecObjectEvents_ReadTaggedItems_Proxy( 
    _IScdExecObjectEvents * This,
    /* [in] */ DWORD dwCount,
    /* [size_is][size_is][in] */ SCD_TAGITEM **pTags,
    /* [size_is][size_is][out][in] */ SCD_DATAITEM **pData,
    /* [size_is][size_is][out][in] */ DWORD **pReturn);


void __RPC_STUB _IScdExecObjectEvents_ReadTaggedItems_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* ___IScdExecObjectEvents_INTERFACE_DEFINED__ */



#ifndef __SysCADMarshal_LIBRARY_DEFINED__
#define __SysCADMarshal_LIBRARY_DEFINED__

/* library SysCADMarshal */
/* [version][uuid] */ 






EXTERN_C const IID LIBID_SysCADMarshal;

#ifndef __ISysCADMarshal_DISPINTERFACE_DEFINED__
#define __ISysCADMarshal_DISPINTERFACE_DEFINED__

/* dispinterface ISysCADMarshal */
/* [uuid] */ 


EXTERN_C const IID DIID_ISysCADMarshal;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("8DD18AB7-D600-4085-8E4F-B34E51C44D26")
    ISysCADMarshal : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct ISysCADMarshalVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISysCADMarshal * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISysCADMarshal * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISysCADMarshal * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISysCADMarshal * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISysCADMarshal * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISysCADMarshal * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISysCADMarshal * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } ISysCADMarshalVtbl;

    interface ISysCADMarshal
    {
        CONST_VTBL struct ISysCADMarshalVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISysCADMarshal_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISysCADMarshal_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISysCADMarshal_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISysCADMarshal_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISysCADMarshal_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISysCADMarshal_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISysCADMarshal_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __ISysCADMarshal_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_Document;

#ifdef __cplusplus

class DECLSPEC_UUID("01E21D76-DF1D-43DD-A2A9-6A40C14765B1")
Document;
#endif

EXTERN_C const CLSID CLSID_ScdMarshal;

#ifdef __cplusplus

class DECLSPEC_UUID("35725B10-C090-4C48-AE31-F9C614C87CD0")
ScdMarshal;
#endif
#endif /* __SysCADMarshal_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long *, unsigned long            , VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserMarshal(  unsigned long *, unsigned char *, VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserUnmarshal(unsigned long *, unsigned char *, VARIANT * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long *, VARIANT * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


