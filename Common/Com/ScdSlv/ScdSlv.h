

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0361 */
/* at Wed Mar 08 09:24:24 2006
 */
/* Compiler settings for .\ScdSlv.idl:
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


#ifndef __ScdSlv_h__
#define __ScdSlv_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IScdDebug_FWD_DEFINED__
#define __IScdDebug_FWD_DEFINED__
typedef interface IScdDebug IScdDebug;
#endif 	/* __IScdDebug_FWD_DEFINED__ */


#ifndef __IScdDynamic_FWD_DEFINED__
#define __IScdDynamic_FWD_DEFINED__
typedef interface IScdDynamic IScdDynamic;
#endif 	/* __IScdDynamic_FWD_DEFINED__ */


#ifndef __IScdMessage_FWD_DEFINED__
#define __IScdMessage_FWD_DEFINED__
typedef interface IScdMessage IScdMessage;
#endif 	/* __IScdMessage_FWD_DEFINED__ */


#ifndef __IScdMessages_FWD_DEFINED__
#define __IScdMessages_FWD_DEFINED__
typedef interface IScdMessages IScdMessages;
#endif 	/* __IScdMessages_FWD_DEFINED__ */


#ifndef __IScdNode_FWD_DEFINED__
#define __IScdNode_FWD_DEFINED__
typedef interface IScdNode IScdNode;
#endif 	/* __IScdNode_FWD_DEFINED__ */


#ifndef __IScdNodes_FWD_DEFINED__
#define __IScdNodes_FWD_DEFINED__
typedef interface IScdNodes IScdNodes;
#endif 	/* __IScdNodes_FWD_DEFINED__ */


#ifndef __IScdNodeClass_FWD_DEFINED__
#define __IScdNodeClass_FWD_DEFINED__
typedef interface IScdNodeClass IScdNodeClass;
#endif 	/* __IScdNodeClass_FWD_DEFINED__ */


#ifndef __IScdConnect_FWD_DEFINED__
#define __IScdConnect_FWD_DEFINED__
typedef interface IScdConnect IScdConnect;
#endif 	/* __IScdConnect_FWD_DEFINED__ */


#ifndef __IScdConnects_FWD_DEFINED__
#define __IScdConnects_FWD_DEFINED__
typedef interface IScdConnects IScdConnects;
#endif 	/* __IScdConnects_FWD_DEFINED__ */


#ifndef __IScdConnectPoint_FWD_DEFINED__
#define __IScdConnectPoint_FWD_DEFINED__
typedef interface IScdConnectPoint IScdConnectPoint;
#endif 	/* __IScdConnectPoint_FWD_DEFINED__ */


#ifndef __IScdConnectPoints_FWD_DEFINED__
#define __IScdConnectPoints_FWD_DEFINED__
typedef interface IScdConnectPoints IScdConnectPoints;
#endif 	/* __IScdConnectPoints_FWD_DEFINED__ */


#ifndef __IScdProbal_FWD_DEFINED__
#define __IScdProbal_FWD_DEFINED__
typedef interface IScdProbal IScdProbal;
#endif 	/* __IScdProbal_FWD_DEFINED__ */


#ifndef __IScdSolver_FWD_DEFINED__
#define __IScdSolver_FWD_DEFINED__
typedef interface IScdSolver IScdSolver;
#endif 	/* __IScdSolver_FWD_DEFINED__ */


#ifndef __IScdSlvTag_FWD_DEFINED__
#define __IScdSlvTag_FWD_DEFINED__
typedef interface IScdSlvTag IScdSlvTag;
#endif 	/* __IScdSlvTag_FWD_DEFINED__ */


#ifndef __IScdSlvTags_FWD_DEFINED__
#define __IScdSlvTags_FWD_DEFINED__
typedef interface IScdSlvTags IScdSlvTags;
#endif 	/* __IScdSlvTags_FWD_DEFINED__ */


#ifndef __IScdTest_FWD_DEFINED__
#define __IScdTest_FWD_DEFINED__
typedef interface IScdTest IScdTest;
#endif 	/* __IScdTest_FWD_DEFINED__ */


#ifndef __IScdWire_FWD_DEFINED__
#define __IScdWire_FWD_DEFINED__
typedef interface IScdWire IScdWire;
#endif 	/* __IScdWire_FWD_DEFINED__ */


#ifndef __IScdWires_FWD_DEFINED__
#define __IScdWires_FWD_DEFINED__
typedef interface IScdWires IScdWires;
#endif 	/* __IScdWires_FWD_DEFINED__ */


#ifndef __IScdTerminal_FWD_DEFINED__
#define __IScdTerminal_FWD_DEFINED__
typedef interface IScdTerminal IScdTerminal;
#endif 	/* __IScdTerminal_FWD_DEFINED__ */


#ifndef __IScdTerminals_FWD_DEFINED__
#define __IScdTerminals_FWD_DEFINED__
typedef interface IScdTerminals IScdTerminals;
#endif 	/* __IScdTerminals_FWD_DEFINED__ */


#ifndef __IScdTermStrip_FWD_DEFINED__
#define __IScdTermStrip_FWD_DEFINED__
typedef interface IScdTermStrip IScdTermStrip;
#endif 	/* __IScdTermStrip_FWD_DEFINED__ */


#ifndef __IScdTermStrips_FWD_DEFINED__
#define __IScdTermStrips_FWD_DEFINED__
typedef interface IScdTermStrips IScdTermStrips;
#endif 	/* __IScdTermStrips_FWD_DEFINED__ */


#ifndef __IScdElectrics_FWD_DEFINED__
#define __IScdElectrics_FWD_DEFINED__
typedef interface IScdElectrics IScdElectrics;
#endif 	/* __IScdElectrics_FWD_DEFINED__ */


#ifndef __IScdComponent_FWD_DEFINED__
#define __IScdComponent_FWD_DEFINED__
typedef interface IScdComponent IScdComponent;
#endif 	/* __IScdComponent_FWD_DEFINED__ */


#ifndef __IScdComponents_FWD_DEFINED__
#define __IScdComponents_FWD_DEFINED__
typedef interface IScdComponents IScdComponents;
#endif 	/* __IScdComponents_FWD_DEFINED__ */


#ifndef __IScdComplex_FWD_DEFINED__
#define __IScdComplex_FWD_DEFINED__
typedef interface IScdComplex IScdComplex;
#endif 	/* __IScdComplex_FWD_DEFINED__ */


#ifndef __ScdMessage_FWD_DEFINED__
#define __ScdMessage_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdMessage ScdMessage;
#else
typedef struct ScdMessage ScdMessage;
#endif /* __cplusplus */

#endif 	/* __ScdMessage_FWD_DEFINED__ */


#ifndef __ScdMessages_FWD_DEFINED__
#define __ScdMessages_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdMessages ScdMessages;
#else
typedef struct ScdMessages ScdMessages;
#endif /* __cplusplus */

#endif 	/* __ScdMessages_FWD_DEFINED__ */


#ifndef __ScdProbal_FWD_DEFINED__
#define __ScdProbal_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdProbal ScdProbal;
#else
typedef struct ScdProbal ScdProbal;
#endif /* __cplusplus */

#endif 	/* __ScdProbal_FWD_DEFINED__ */


#ifndef __ScdSolver_FWD_DEFINED__
#define __ScdSolver_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdSolver ScdSolver;
#else
typedef struct ScdSolver ScdSolver;
#endif /* __cplusplus */

#endif 	/* __ScdSolver_FWD_DEFINED__ */


#ifndef __ScdDebug_FWD_DEFINED__
#define __ScdDebug_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdDebug ScdDebug;
#else
typedef struct ScdDebug ScdDebug;
#endif /* __cplusplus */

#endif 	/* __ScdDebug_FWD_DEFINED__ */


#ifndef __ScdTest_FWD_DEFINED__
#define __ScdTest_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdTest ScdTest;
#else
typedef struct ScdTest ScdTest;
#endif /* __cplusplus */

#endif 	/* __ScdTest_FWD_DEFINED__ */


#ifndef __ScdSlvTags_FWD_DEFINED__
#define __ScdSlvTags_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdSlvTags ScdSlvTags;
#else
typedef struct ScdSlvTags ScdSlvTags;
#endif /* __cplusplus */

#endif 	/* __ScdSlvTags_FWD_DEFINED__ */


#ifndef __ScdSlvTag_FWD_DEFINED__
#define __ScdSlvTag_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdSlvTag ScdSlvTag;
#else
typedef struct ScdSlvTag ScdSlvTag;
#endif /* __cplusplus */

#endif 	/* __ScdSlvTag_FWD_DEFINED__ */


#ifndef __ScdDynamic_FWD_DEFINED__
#define __ScdDynamic_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdDynamic ScdDynamic;
#else
typedef struct ScdDynamic ScdDynamic;
#endif /* __cplusplus */

#endif 	/* __ScdDynamic_FWD_DEFINED__ */


#ifndef __ScdNodes_FWD_DEFINED__
#define __ScdNodes_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdNodes ScdNodes;
#else
typedef struct ScdNodes ScdNodes;
#endif /* __cplusplus */

#endif 	/* __ScdNodes_FWD_DEFINED__ */


#ifndef __ScdNode_FWD_DEFINED__
#define __ScdNode_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdNode ScdNode;
#else
typedef struct ScdNode ScdNode;
#endif /* __cplusplus */

#endif 	/* __ScdNode_FWD_DEFINED__ */


#ifndef __ScdConnect_FWD_DEFINED__
#define __ScdConnect_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdConnect ScdConnect;
#else
typedef struct ScdConnect ScdConnect;
#endif /* __cplusplus */

#endif 	/* __ScdConnect_FWD_DEFINED__ */


#ifndef __ScdConnectPoint_FWD_DEFINED__
#define __ScdConnectPoint_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdConnectPoint ScdConnectPoint;
#else
typedef struct ScdConnectPoint ScdConnectPoint;
#endif /* __cplusplus */

#endif 	/* __ScdConnectPoint_FWD_DEFINED__ */


#ifndef __ScdConnects_FWD_DEFINED__
#define __ScdConnects_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdConnects ScdConnects;
#else
typedef struct ScdConnects ScdConnects;
#endif /* __cplusplus */

#endif 	/* __ScdConnects_FWD_DEFINED__ */


#ifndef __ScdConnectPoints_FWD_DEFINED__
#define __ScdConnectPoints_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdConnectPoints ScdConnectPoints;
#else
typedef struct ScdConnectPoints ScdConnectPoints;
#endif /* __cplusplus */

#endif 	/* __ScdConnectPoints_FWD_DEFINED__ */


#ifndef __ScdNodeClass_FWD_DEFINED__
#define __ScdNodeClass_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdNodeClass ScdNodeClass;
#else
typedef struct ScdNodeClass ScdNodeClass;
#endif /* __cplusplus */

#endif 	/* __ScdNodeClass_FWD_DEFINED__ */


#ifndef __ScdWire_FWD_DEFINED__
#define __ScdWire_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdWire ScdWire;
#else
typedef struct ScdWire ScdWire;
#endif /* __cplusplus */

#endif 	/* __ScdWire_FWD_DEFINED__ */


#ifndef __ScdWires_FWD_DEFINED__
#define __ScdWires_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdWires ScdWires;
#else
typedef struct ScdWires ScdWires;
#endif /* __cplusplus */

#endif 	/* __ScdWires_FWD_DEFINED__ */


#ifndef __ScdTerminal_FWD_DEFINED__
#define __ScdTerminal_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdTerminal ScdTerminal;
#else
typedef struct ScdTerminal ScdTerminal;
#endif /* __cplusplus */

#endif 	/* __ScdTerminal_FWD_DEFINED__ */


#ifndef __ScdTerminals_FWD_DEFINED__
#define __ScdTerminals_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdTerminals ScdTerminals;
#else
typedef struct ScdTerminals ScdTerminals;
#endif /* __cplusplus */

#endif 	/* __ScdTerminals_FWD_DEFINED__ */


#ifndef __ScdTermStrip_FWD_DEFINED__
#define __ScdTermStrip_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdTermStrip ScdTermStrip;
#else
typedef struct ScdTermStrip ScdTermStrip;
#endif /* __cplusplus */

#endif 	/* __ScdTermStrip_FWD_DEFINED__ */


#ifndef __ScdTermStrips_FWD_DEFINED__
#define __ScdTermStrips_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdTermStrips ScdTermStrips;
#else
typedef struct ScdTermStrips ScdTermStrips;
#endif /* __cplusplus */

#endif 	/* __ScdTermStrips_FWD_DEFINED__ */


#ifndef __ScdElectrics_FWD_DEFINED__
#define __ScdElectrics_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdElectrics ScdElectrics;
#else
typedef struct ScdElectrics ScdElectrics;
#endif /* __cplusplus */

#endif 	/* __ScdElectrics_FWD_DEFINED__ */


#ifndef __ScdComplex_FWD_DEFINED__
#define __ScdComplex_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdComplex ScdComplex;
#else
typedef struct ScdComplex ScdComplex;
#endif /* __cplusplus */

#endif 	/* __ScdComplex_FWD_DEFINED__ */


#ifndef __ScdComponent_FWD_DEFINED__
#define __ScdComponent_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdComponent ScdComponent;
#else
typedef struct ScdComponent ScdComponent;
#endif /* __cplusplus */

#endif 	/* __ScdComponent_FWD_DEFINED__ */


#ifndef __ScdComponents_FWD_DEFINED__
#define __ScdComponents_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdComponents ScdComponents;
#else
typedef struct ScdComponents ScdComponents;
#endif /* __cplusplus */

#endif 	/* __ScdComponents_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"
#include "ScdIF.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 


#ifndef __ScdSlv_LIBRARY_DEFINED__
#define __ScdSlv_LIBRARY_DEFINED__

/* library ScdSlv */
/* [helpstring][version][uuid] */ 


























typedef /* [v1_enum][helpstring][uuid] */  DECLSPEC_UUID("8E4ECA64-1459-4172-8098-8C9371D640C9") 
enum eScdNodeCat_Flags
    {	eScdNodeCat_DYNAMIC	= 0x1,
	eScdNodeCat_PROBAL	= 0x2,
	eScdNodeCat_SOLVE_MASK	= 0xf,
	eScdNodeCat_STD_KENWALT	= 0x10,
	eScdNodeCat_SIZEDIST	= 0x20,
	eScdNodeCat_ALUMINA	= 0x40,
	eScdNodeCat_USER	= 0x80,
	eScdNodeCat_KAISER	= 0x100,
	eScdNodeCat_QAL	= 0x200,
	eScdNodeCat_QALEXTRA	= 0x400,
	eScdNodeCat_ALCAN	= 0x800,
	eScdNodeCat_IRITRON	= 0x1000,
	eScdNodeCat_RTTS	= 0x2000,
	eScdNodeCat_HEATBAL	= 0x4000,
	eScdNodeCat_BLACKBOX	= 0x8000,
	eScdNodeCat_POWERDIST	= 0x10000,
	eScdNodeCat_MDL_UNUSED3	= 0x20000,
	eScdNodeCat_MDL_UNUSED2	= 0x40000,
	eScdNodeCat_MDL_UNUSED1	= 0x80000,
	eScdNodeCat_MDL_MASK	= 0xffff0,
	eScdNodeCat_GRP_GENERAL	= 0x80000000,
	eScdNodeCat_GRP_ENERGY	= 0x40000000,
	eScdNodeCat_GRP_SEPAR	= 0x20000000,
	eScdNodeCat_GRP_SIZEDST	= 0x10000000,
	eScdNodeCat_GRP_TRANSFER	= 0x8000000,
	eScdNodeCat_GRP_ALUMINA	= 0x4000000,
	eScdNodeCat_GRP_POWERDIST	= 0x2000000,
	eScdNodeCat_GRP_UNUSED5	= 0x1000000,
	eScdNodeCat_GRP_UNUSED4	= 0x800000,
	eScdNodeCat_GRP_UNUSED3	= 0x400000,
	eScdNodeCat_GRP_UNUSED2	= 0x200000,
	eScdNodeCat_GRP_UNUSED1	= 0x100000,
	eScdNodeCat_GRP_MASK	= 0xfff00000
    } 	eScdNodeCat_Flags;


EXTERN_C const IID LIBID_ScdSlv;

#ifndef __IScdDebug_INTERFACE_DEFINED__
#define __IScdDebug_INTERFACE_DEFINED__

/* interface IScdDebug */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdDebug;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4249F08F-233A-4EEC-9017-641B42D121F6")
    IScdDebug : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FileOpen( 
            /* [in] */ BSTR FileName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FileClose( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScdDebugVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdDebug * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdDebug * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdDebug * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdDebug * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdDebug * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdDebug * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdDebug * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FileOpen )( 
            IScdDebug * This,
            /* [in] */ BSTR FileName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FileClose )( 
            IScdDebug * This);
        
        END_INTERFACE
    } IScdDebugVtbl;

    interface IScdDebug
    {
        CONST_VTBL struct IScdDebugVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdDebug_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdDebug_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdDebug_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdDebug_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdDebug_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdDebug_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdDebug_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IScdDebug_FileOpen(This,FileName)	\
    (This)->lpVtbl -> FileOpen(This,FileName)

#define IScdDebug_FileClose(This)	\
    (This)->lpVtbl -> FileClose(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdDebug_FileOpen_Proxy( 
    IScdDebug * This,
    /* [in] */ BSTR FileName);


void __RPC_STUB IScdDebug_FileOpen_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdDebug_FileClose_Proxy( 
    IScdDebug * This);


void __RPC_STUB IScdDebug_FileClose_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScdDebug_INTERFACE_DEFINED__ */


#ifndef __IScdDynamic_INTERFACE_DEFINED__
#define __IScdDynamic_INTERFACE_DEFINED__

/* interface IScdDynamic */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdDynamic;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("747D22A6-D343-4CA6-91CE-376EED67C3DB")
    IScdDynamic : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_IsStopped( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_IsIdling( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_IsRunning( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_StepSize( 
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_StepSize( 
            /* [in] */ double newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_RealTime( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_RealTime( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_RealTimeMultiplier( 
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_RealTimeMultiplier( 
            /* [in] */ double newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Start( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Step( 
            /* [defaultvalue][in] */ long IterMark = 0,
            /* [defaultvalue][in] */ double WaitForNext = 0) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Stop( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Idle( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScdDynamicVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdDynamic * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdDynamic * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdDynamic * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdDynamic * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdDynamic * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdDynamic * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdDynamic * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsStopped )( 
            IScdDynamic * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsIdling )( 
            IScdDynamic * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsRunning )( 
            IScdDynamic * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_StepSize )( 
            IScdDynamic * This,
            /* [retval][out] */ double *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_StepSize )( 
            IScdDynamic * This,
            /* [in] */ double newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_RealTime )( 
            IScdDynamic * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_RealTime )( 
            IScdDynamic * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_RealTimeMultiplier )( 
            IScdDynamic * This,
            /* [retval][out] */ double *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_RealTimeMultiplier )( 
            IScdDynamic * This,
            /* [in] */ double newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Start )( 
            IScdDynamic * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Step )( 
            IScdDynamic * This,
            /* [defaultvalue][in] */ long IterMark,
            /* [defaultvalue][in] */ double WaitForNext);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Stop )( 
            IScdDynamic * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Idle )( 
            IScdDynamic * This);
        
        END_INTERFACE
    } IScdDynamicVtbl;

    interface IScdDynamic
    {
        CONST_VTBL struct IScdDynamicVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdDynamic_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdDynamic_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdDynamic_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdDynamic_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdDynamic_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdDynamic_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdDynamic_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IScdDynamic_get_IsStopped(This,pVal)	\
    (This)->lpVtbl -> get_IsStopped(This,pVal)

#define IScdDynamic_get_IsIdling(This,pVal)	\
    (This)->lpVtbl -> get_IsIdling(This,pVal)

#define IScdDynamic_get_IsRunning(This,pVal)	\
    (This)->lpVtbl -> get_IsRunning(This,pVal)

#define IScdDynamic_get_StepSize(This,pVal)	\
    (This)->lpVtbl -> get_StepSize(This,pVal)

#define IScdDynamic_put_StepSize(This,newVal)	\
    (This)->lpVtbl -> put_StepSize(This,newVal)

#define IScdDynamic_get_RealTime(This,pVal)	\
    (This)->lpVtbl -> get_RealTime(This,pVal)

#define IScdDynamic_put_RealTime(This,newVal)	\
    (This)->lpVtbl -> put_RealTime(This,newVal)

#define IScdDynamic_get_RealTimeMultiplier(This,pVal)	\
    (This)->lpVtbl -> get_RealTimeMultiplier(This,pVal)

#define IScdDynamic_put_RealTimeMultiplier(This,newVal)	\
    (This)->lpVtbl -> put_RealTimeMultiplier(This,newVal)

#define IScdDynamic_Start(This)	\
    (This)->lpVtbl -> Start(This)

#define IScdDynamic_Step(This,IterMark,WaitForNext)	\
    (This)->lpVtbl -> Step(This,IterMark,WaitForNext)

#define IScdDynamic_Stop(This)	\
    (This)->lpVtbl -> Stop(This)

#define IScdDynamic_Idle(This)	\
    (This)->lpVtbl -> Idle(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdDynamic_get_IsStopped_Proxy( 
    IScdDynamic * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB IScdDynamic_get_IsStopped_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdDynamic_get_IsIdling_Proxy( 
    IScdDynamic * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB IScdDynamic_get_IsIdling_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdDynamic_get_IsRunning_Proxy( 
    IScdDynamic * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB IScdDynamic_get_IsRunning_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdDynamic_get_StepSize_Proxy( 
    IScdDynamic * This,
    /* [retval][out] */ double *pVal);


void __RPC_STUB IScdDynamic_get_StepSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IScdDynamic_put_StepSize_Proxy( 
    IScdDynamic * This,
    /* [in] */ double newVal);


void __RPC_STUB IScdDynamic_put_StepSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdDynamic_get_RealTime_Proxy( 
    IScdDynamic * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB IScdDynamic_get_RealTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IScdDynamic_put_RealTime_Proxy( 
    IScdDynamic * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB IScdDynamic_put_RealTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdDynamic_get_RealTimeMultiplier_Proxy( 
    IScdDynamic * This,
    /* [retval][out] */ double *pVal);


void __RPC_STUB IScdDynamic_get_RealTimeMultiplier_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IScdDynamic_put_RealTimeMultiplier_Proxy( 
    IScdDynamic * This,
    /* [in] */ double newVal);


void __RPC_STUB IScdDynamic_put_RealTimeMultiplier_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdDynamic_Start_Proxy( 
    IScdDynamic * This);


void __RPC_STUB IScdDynamic_Start_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdDynamic_Step_Proxy( 
    IScdDynamic * This,
    /* [defaultvalue][in] */ long IterMark,
    /* [defaultvalue][in] */ double WaitForNext);


void __RPC_STUB IScdDynamic_Step_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdDynamic_Stop_Proxy( 
    IScdDynamic * This);


void __RPC_STUB IScdDynamic_Stop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdDynamic_Idle_Proxy( 
    IScdDynamic * This);


void __RPC_STUB IScdDynamic_Idle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScdDynamic_INTERFACE_DEFINED__ */


#ifndef __IScdMessage_INTERFACE_DEFINED__
#define __IScdMessage_INTERFACE_DEFINED__

/* interface IScdMessage */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdMessage;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("93A8521F-945A-4226-BD45-52B58D34F73A")
    IScdMessage : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Flags( 
            /* [retval][out] */ eScdMsg_Flags *pVal) = 0;
        
        virtual /* [helpstring][restricted][id][propput] */ HRESULT STDMETHODCALLTYPE put_Flags( 
            /* [in] */ eScdMsg_Flags newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_IDNo( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][restricted][id][propput] */ HRESULT STDMETHODCALLTYPE put_IDNo( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_IterationNo( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][restricted][id][propput] */ HRESULT STDMETHODCALLTYPE put_IterationNo( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SequenceNo( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][restricted][id][propput] */ HRESULT STDMETHODCALLTYPE put_SequenceNo( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_CallNo( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][restricted][id][propput] */ HRESULT STDMETHODCALLTYPE put_CallNo( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Description( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][restricted][id][propput] */ HRESULT STDMETHODCALLTYPE put_Description( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Source( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][restricted][id][propput] */ HRESULT STDMETHODCALLTYPE put_Source( 
            /* [in] */ BSTR newVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScdMessageVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdMessage * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdMessage * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdMessage * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdMessage * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdMessage * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdMessage * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdMessage * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Flags )( 
            IScdMessage * This,
            /* [retval][out] */ eScdMsg_Flags *pVal);
        
        /* [helpstring][restricted][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Flags )( 
            IScdMessage * This,
            /* [in] */ eScdMsg_Flags newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IDNo )( 
            IScdMessage * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][restricted][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_IDNo )( 
            IScdMessage * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IterationNo )( 
            IScdMessage * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][restricted][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_IterationNo )( 
            IScdMessage * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SequenceNo )( 
            IScdMessage * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][restricted][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_SequenceNo )( 
            IScdMessage * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_CallNo )( 
            IScdMessage * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][restricted][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_CallNo )( 
            IScdMessage * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Description )( 
            IScdMessage * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][restricted][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Description )( 
            IScdMessage * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Source )( 
            IScdMessage * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][restricted][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Source )( 
            IScdMessage * This,
            /* [in] */ BSTR newVal);
        
        END_INTERFACE
    } IScdMessageVtbl;

    interface IScdMessage
    {
        CONST_VTBL struct IScdMessageVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdMessage_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdMessage_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdMessage_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdMessage_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdMessage_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdMessage_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdMessage_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IScdMessage_get_Flags(This,pVal)	\
    (This)->lpVtbl -> get_Flags(This,pVal)

#define IScdMessage_put_Flags(This,newVal)	\
    (This)->lpVtbl -> put_Flags(This,newVal)

#define IScdMessage_get_IDNo(This,pVal)	\
    (This)->lpVtbl -> get_IDNo(This,pVal)

#define IScdMessage_put_IDNo(This,newVal)	\
    (This)->lpVtbl -> put_IDNo(This,newVal)

#define IScdMessage_get_IterationNo(This,pVal)	\
    (This)->lpVtbl -> get_IterationNo(This,pVal)

#define IScdMessage_put_IterationNo(This,newVal)	\
    (This)->lpVtbl -> put_IterationNo(This,newVal)

#define IScdMessage_get_SequenceNo(This,pVal)	\
    (This)->lpVtbl -> get_SequenceNo(This,pVal)

#define IScdMessage_put_SequenceNo(This,newVal)	\
    (This)->lpVtbl -> put_SequenceNo(This,newVal)

#define IScdMessage_get_CallNo(This,pVal)	\
    (This)->lpVtbl -> get_CallNo(This,pVal)

#define IScdMessage_put_CallNo(This,newVal)	\
    (This)->lpVtbl -> put_CallNo(This,newVal)

#define IScdMessage_get_Description(This,pVal)	\
    (This)->lpVtbl -> get_Description(This,pVal)

#define IScdMessage_put_Description(This,newVal)	\
    (This)->lpVtbl -> put_Description(This,newVal)

#define IScdMessage_get_Source(This,pVal)	\
    (This)->lpVtbl -> get_Source(This,pVal)

#define IScdMessage_put_Source(This,newVal)	\
    (This)->lpVtbl -> put_Source(This,newVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdMessage_get_Flags_Proxy( 
    IScdMessage * This,
    /* [retval][out] */ eScdMsg_Flags *pVal);


void __RPC_STUB IScdMessage_get_Flags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][restricted][id][propput] */ HRESULT STDMETHODCALLTYPE IScdMessage_put_Flags_Proxy( 
    IScdMessage * This,
    /* [in] */ eScdMsg_Flags newVal);


void __RPC_STUB IScdMessage_put_Flags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdMessage_get_IDNo_Proxy( 
    IScdMessage * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IScdMessage_get_IDNo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][restricted][id][propput] */ HRESULT STDMETHODCALLTYPE IScdMessage_put_IDNo_Proxy( 
    IScdMessage * This,
    /* [in] */ long newVal);


void __RPC_STUB IScdMessage_put_IDNo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdMessage_get_IterationNo_Proxy( 
    IScdMessage * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IScdMessage_get_IterationNo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][restricted][id][propput] */ HRESULT STDMETHODCALLTYPE IScdMessage_put_IterationNo_Proxy( 
    IScdMessage * This,
    /* [in] */ long newVal);


void __RPC_STUB IScdMessage_put_IterationNo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdMessage_get_SequenceNo_Proxy( 
    IScdMessage * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IScdMessage_get_SequenceNo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][restricted][id][propput] */ HRESULT STDMETHODCALLTYPE IScdMessage_put_SequenceNo_Proxy( 
    IScdMessage * This,
    /* [in] */ long newVal);


void __RPC_STUB IScdMessage_put_SequenceNo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdMessage_get_CallNo_Proxy( 
    IScdMessage * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IScdMessage_get_CallNo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][restricted][id][propput] */ HRESULT STDMETHODCALLTYPE IScdMessage_put_CallNo_Proxy( 
    IScdMessage * This,
    /* [in] */ long newVal);


void __RPC_STUB IScdMessage_put_CallNo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdMessage_get_Description_Proxy( 
    IScdMessage * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IScdMessage_get_Description_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][restricted][id][propput] */ HRESULT STDMETHODCALLTYPE IScdMessage_put_Description_Proxy( 
    IScdMessage * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IScdMessage_put_Description_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdMessage_get_Source_Proxy( 
    IScdMessage * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IScdMessage_get_Source_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][restricted][id][propput] */ HRESULT STDMETHODCALLTYPE IScdMessage_put_Source_Proxy( 
    IScdMessage * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IScdMessage_put_Source_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScdMessage_INTERFACE_DEFINED__ */


#ifndef __IScdMessages_INTERFACE_DEFINED__
#define __IScdMessages_INTERFACE_DEFINED__

/* interface IScdMessages */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdMessages;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("ED9A26C3-3527-4d90-82AD-D8F4741C242A")
    IScdMessages : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Item( 
            /* [in] */ long index,
            /* [retval][out] */ IScdMessage **pItem) = 0;
        
        virtual /* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ LPUNKNOWN *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ViewLimit( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ViewLimit( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Refresh( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Flush( 
            /* [defaultvalue][in] */ long NoToKeep = 0) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Options( 
            /* [retval][out] */ eScdMsg_Options *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Options( 
            /* [in] */ eScdMsg_Options newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Select( 
            /* [defaultvalue][in] */ eScdMsg_Flags Selection = eScdMsg_All) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LogFileOpen( 
            /* [in] */ BSTR FileName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LogFileClose( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScdMessagesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdMessages * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdMessages * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdMessages * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdMessages * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdMessages * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdMessages * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdMessages * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Item )( 
            IScdMessages * This,
            /* [in] */ long index,
            /* [retval][out] */ IScdMessage **pItem);
        
        /* [id][restricted][propget] */ HRESULT ( STDMETHODCALLTYPE *get__NewEnum )( 
            IScdMessages * This,
            /* [retval][out] */ LPUNKNOWN *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            IScdMessages * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ViewLimit )( 
            IScdMessages * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ViewLimit )( 
            IScdMessages * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Refresh )( 
            IScdMessages * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Flush )( 
            IScdMessages * This,
            /* [defaultvalue][in] */ long NoToKeep);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Options )( 
            IScdMessages * This,
            /* [retval][out] */ eScdMsg_Options *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Options )( 
            IScdMessages * This,
            /* [in] */ eScdMsg_Options newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Select )( 
            IScdMessages * This,
            /* [defaultvalue][in] */ eScdMsg_Flags Selection);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LogFileOpen )( 
            IScdMessages * This,
            /* [in] */ BSTR FileName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LogFileClose )( 
            IScdMessages * This);
        
        END_INTERFACE
    } IScdMessagesVtbl;

    interface IScdMessages
    {
        CONST_VTBL struct IScdMessagesVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdMessages_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdMessages_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdMessages_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdMessages_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdMessages_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdMessages_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdMessages_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IScdMessages_Item(This,index,pItem)	\
    (This)->lpVtbl -> Item(This,index,pItem)

#define IScdMessages_get__NewEnum(This,pVal)	\
    (This)->lpVtbl -> get__NewEnum(This,pVal)

#define IScdMessages_get_Count(This,pVal)	\
    (This)->lpVtbl -> get_Count(This,pVal)

#define IScdMessages_get_ViewLimit(This,pVal)	\
    (This)->lpVtbl -> get_ViewLimit(This,pVal)

#define IScdMessages_put_ViewLimit(This,newVal)	\
    (This)->lpVtbl -> put_ViewLimit(This,newVal)

#define IScdMessages_Refresh(This)	\
    (This)->lpVtbl -> Refresh(This)

#define IScdMessages_Flush(This,NoToKeep)	\
    (This)->lpVtbl -> Flush(This,NoToKeep)

#define IScdMessages_get_Options(This,pVal)	\
    (This)->lpVtbl -> get_Options(This,pVal)

#define IScdMessages_put_Options(This,newVal)	\
    (This)->lpVtbl -> put_Options(This,newVal)

#define IScdMessages_Select(This,Selection)	\
    (This)->lpVtbl -> Select(This,Selection)

#define IScdMessages_LogFileOpen(This,FileName)	\
    (This)->lpVtbl -> LogFileOpen(This,FileName)

#define IScdMessages_LogFileClose(This)	\
    (This)->lpVtbl -> LogFileClose(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdMessages_Item_Proxy( 
    IScdMessages * This,
    /* [in] */ long index,
    /* [retval][out] */ IScdMessage **pItem);


void __RPC_STUB IScdMessages_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE IScdMessages_get__NewEnum_Proxy( 
    IScdMessages * This,
    /* [retval][out] */ LPUNKNOWN *pVal);


void __RPC_STUB IScdMessages_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdMessages_get_Count_Proxy( 
    IScdMessages * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IScdMessages_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdMessages_get_ViewLimit_Proxy( 
    IScdMessages * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IScdMessages_get_ViewLimit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IScdMessages_put_ViewLimit_Proxy( 
    IScdMessages * This,
    /* [in] */ long newVal);


void __RPC_STUB IScdMessages_put_ViewLimit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdMessages_Refresh_Proxy( 
    IScdMessages * This);


void __RPC_STUB IScdMessages_Refresh_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdMessages_Flush_Proxy( 
    IScdMessages * This,
    /* [defaultvalue][in] */ long NoToKeep);


void __RPC_STUB IScdMessages_Flush_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdMessages_get_Options_Proxy( 
    IScdMessages * This,
    /* [retval][out] */ eScdMsg_Options *pVal);


void __RPC_STUB IScdMessages_get_Options_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IScdMessages_put_Options_Proxy( 
    IScdMessages * This,
    /* [in] */ eScdMsg_Options newVal);


void __RPC_STUB IScdMessages_put_Options_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdMessages_Select_Proxy( 
    IScdMessages * This,
    /* [defaultvalue][in] */ eScdMsg_Flags Selection);


void __RPC_STUB IScdMessages_Select_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdMessages_LogFileOpen_Proxy( 
    IScdMessages * This,
    /* [in] */ BSTR FileName);


void __RPC_STUB IScdMessages_LogFileOpen_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdMessages_LogFileClose_Proxy( 
    IScdMessages * This);


void __RPC_STUB IScdMessages_LogFileClose_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScdMessages_INTERFACE_DEFINED__ */


#ifndef __IScdNode_INTERFACE_DEFINED__
#define __IScdNode_INTERFACE_DEFINED__

/* interface IScdNode */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdNode;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("50EB2EAE-1D92-4BF5-BBAE-D92EA5E54B6E")
    IScdNode : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Tag( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Tag( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Type( 
            /* [retval][out] */ eScdNodeTypes *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Class( 
            /* [retval][out] */ IScdNodeClass **pClass) = 0;
        
        virtual /* [helpstring][restricted][id] */ HRESULT STDMETHODCALLTYPE SetObjectInfo( 
            /* [in] */ DWORD newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_TypeString( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Connects( 
            /* [defaultvalue][in] */ eScdDirections Direction,
            /* [defaultvalue][in] */ eScdNodeTypes Types,
            /* [retval][out] */ IScdConnects **pConnects) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ConnectPoints( 
            /* [defaultvalue][in] */ eScdDirections Direction,
            /* [defaultvalue][in] */ eScdNodeTypes Types,
            /* [retval][out] */ IScdConnectPoints **pConnectPoints) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SrcNode( 
            /* [retval][out] */ IScdNode **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_DstNode( 
            /* [retval][out] */ IScdNode **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_RmtNodes( 
            /* [in] */ eScdDirections Dirns,
            /* [defaultvalue][in] */ eScdNodeTypes Types,
            /* [retval][out] */ IScdNodes **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Electrics( 
            /* [retval][out] */ IScdElectrics **pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScdNodeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdNode * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdNode * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdNode * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdNode * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdNode * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdNode * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdNode * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Tag )( 
            IScdNode * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Tag )( 
            IScdNode * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Type )( 
            IScdNode * This,
            /* [retval][out] */ eScdNodeTypes *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Class )( 
            IScdNode * This,
            /* [retval][out] */ IScdNodeClass **pClass);
        
        /* [helpstring][restricted][id] */ HRESULT ( STDMETHODCALLTYPE *SetObjectInfo )( 
            IScdNode * This,
            /* [in] */ DWORD newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_TypeString )( 
            IScdNode * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Connects )( 
            IScdNode * This,
            /* [defaultvalue][in] */ eScdDirections Direction,
            /* [defaultvalue][in] */ eScdNodeTypes Types,
            /* [retval][out] */ IScdConnects **pConnects);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ConnectPoints )( 
            IScdNode * This,
            /* [defaultvalue][in] */ eScdDirections Direction,
            /* [defaultvalue][in] */ eScdNodeTypes Types,
            /* [retval][out] */ IScdConnectPoints **pConnectPoints);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SrcNode )( 
            IScdNode * This,
            /* [retval][out] */ IScdNode **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DstNode )( 
            IScdNode * This,
            /* [retval][out] */ IScdNode **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_RmtNodes )( 
            IScdNode * This,
            /* [in] */ eScdDirections Dirns,
            /* [defaultvalue][in] */ eScdNodeTypes Types,
            /* [retval][out] */ IScdNodes **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Electrics )( 
            IScdNode * This,
            /* [retval][out] */ IScdElectrics **pVal);
        
        END_INTERFACE
    } IScdNodeVtbl;

    interface IScdNode
    {
        CONST_VTBL struct IScdNodeVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdNode_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdNode_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdNode_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdNode_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdNode_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdNode_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdNode_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IScdNode_get_Tag(This,pVal)	\
    (This)->lpVtbl -> get_Tag(This,pVal)

#define IScdNode_put_Tag(This,newVal)	\
    (This)->lpVtbl -> put_Tag(This,newVal)

#define IScdNode_get_Type(This,pVal)	\
    (This)->lpVtbl -> get_Type(This,pVal)

#define IScdNode_get_Class(This,pClass)	\
    (This)->lpVtbl -> get_Class(This,pClass)

#define IScdNode_SetObjectInfo(This,newVal)	\
    (This)->lpVtbl -> SetObjectInfo(This,newVal)

#define IScdNode_get_TypeString(This,pVal)	\
    (This)->lpVtbl -> get_TypeString(This,pVal)

#define IScdNode_get_Connects(This,Direction,Types,pConnects)	\
    (This)->lpVtbl -> get_Connects(This,Direction,Types,pConnects)

#define IScdNode_get_ConnectPoints(This,Direction,Types,pConnectPoints)	\
    (This)->lpVtbl -> get_ConnectPoints(This,Direction,Types,pConnectPoints)

#define IScdNode_get_SrcNode(This,pVal)	\
    (This)->lpVtbl -> get_SrcNode(This,pVal)

#define IScdNode_get_DstNode(This,pVal)	\
    (This)->lpVtbl -> get_DstNode(This,pVal)

#define IScdNode_get_RmtNodes(This,Dirns,Types,pVal)	\
    (This)->lpVtbl -> get_RmtNodes(This,Dirns,Types,pVal)

#define IScdNode_get_Electrics(This,pVal)	\
    (This)->lpVtbl -> get_Electrics(This,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdNode_get_Tag_Proxy( 
    IScdNode * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IScdNode_get_Tag_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IScdNode_put_Tag_Proxy( 
    IScdNode * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IScdNode_put_Tag_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdNode_get_Type_Proxy( 
    IScdNode * This,
    /* [retval][out] */ eScdNodeTypes *pVal);


void __RPC_STUB IScdNode_get_Type_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdNode_get_Class_Proxy( 
    IScdNode * This,
    /* [retval][out] */ IScdNodeClass **pClass);


void __RPC_STUB IScdNode_get_Class_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][restricted][id] */ HRESULT STDMETHODCALLTYPE IScdNode_SetObjectInfo_Proxy( 
    IScdNode * This,
    /* [in] */ DWORD newVal);


void __RPC_STUB IScdNode_SetObjectInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdNode_get_TypeString_Proxy( 
    IScdNode * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IScdNode_get_TypeString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdNode_get_Connects_Proxy( 
    IScdNode * This,
    /* [defaultvalue][in] */ eScdDirections Direction,
    /* [defaultvalue][in] */ eScdNodeTypes Types,
    /* [retval][out] */ IScdConnects **pConnects);


void __RPC_STUB IScdNode_get_Connects_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdNode_get_ConnectPoints_Proxy( 
    IScdNode * This,
    /* [defaultvalue][in] */ eScdDirections Direction,
    /* [defaultvalue][in] */ eScdNodeTypes Types,
    /* [retval][out] */ IScdConnectPoints **pConnectPoints);


void __RPC_STUB IScdNode_get_ConnectPoints_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdNode_get_SrcNode_Proxy( 
    IScdNode * This,
    /* [retval][out] */ IScdNode **pVal);


void __RPC_STUB IScdNode_get_SrcNode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdNode_get_DstNode_Proxy( 
    IScdNode * This,
    /* [retval][out] */ IScdNode **pVal);


void __RPC_STUB IScdNode_get_DstNode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdNode_get_RmtNodes_Proxy( 
    IScdNode * This,
    /* [in] */ eScdDirections Dirns,
    /* [defaultvalue][in] */ eScdNodeTypes Types,
    /* [retval][out] */ IScdNodes **pVal);


void __RPC_STUB IScdNode_get_RmtNodes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdNode_get_Electrics_Proxy( 
    IScdNode * This,
    /* [retval][out] */ IScdElectrics **pVal);


void __RPC_STUB IScdNode_get_Electrics_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScdNode_INTERFACE_DEFINED__ */


#ifndef __IScdNodes_INTERFACE_DEFINED__
#define __IScdNodes_INTERFACE_DEFINED__

/* interface IScdNodes */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdNodes;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("530CE54B-054D-44E9-90D7-A2DABF4E57D1")
    IScdNodes : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Node( 
            /* [in] */ VARIANT WhichOne,
            /* [retval][out] */ IScdNode **pItem) = 0;
        
        virtual /* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ LPUNKNOWN *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ReturnCode( 
            /* [retval][out] */ eScdNodeCodes *pVal) = 0;
        
        virtual /* [helpstring][restricted][id] */ HRESULT STDMETHODCALLTYPE SetObjectInfo( 
            /* [in] */ DWORD What,
            /* [in] */ DWORD Node,
            /* [in] */ DWORD Types,
            /* [in] */ DWORD Direction) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Select( 
            /* [defaultvalue][in] */ eScdNodeTypes eType = eScdNodeType_All) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddUnit( 
            /* [in] */ BSTR Class,
            /* [in] */ BSTR Tag,
            /* [retval][out] */ IScdNode **pUnit) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddLink( 
            /* [in] */ BSTR Class,
            /* [in] */ BSTR Tag,
            /* [in] */ BSTR SrcTag,
            /* [in] */ BSTR SrcIO,
            /* [in] */ BSTR DstTag,
            /* [in] */ BSTR DstIO,
            /* [retval][out] */ IScdNode **pLink) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddCable( 
            /* [in] */ BSTR Class,
            /* [in] */ BSTR Tag,
            /* [in] */ BSTR SrcTag,
            /* [in] */ BSTR SrcIO,
            /* [in] */ BSTR SrcTS,
            /* [in] */ BSTR DstTag,
            /* [in] */ BSTR DstIO,
            /* [in] */ BSTR DstTS,
            /* [retval][out] */ IScdNode **pLink) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScdNodesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdNodes * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdNodes * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdNodes * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdNodes * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdNodes * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdNodes * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdNodes * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Node )( 
            IScdNodes * This,
            /* [in] */ VARIANT WhichOne,
            /* [retval][out] */ IScdNode **pItem);
        
        /* [id][restricted][propget] */ HRESULT ( STDMETHODCALLTYPE *get__NewEnum )( 
            IScdNodes * This,
            /* [retval][out] */ LPUNKNOWN *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            IScdNodes * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ReturnCode )( 
            IScdNodes * This,
            /* [retval][out] */ eScdNodeCodes *pVal);
        
        /* [helpstring][restricted][id] */ HRESULT ( STDMETHODCALLTYPE *SetObjectInfo )( 
            IScdNodes * This,
            /* [in] */ DWORD What,
            /* [in] */ DWORD Node,
            /* [in] */ DWORD Types,
            /* [in] */ DWORD Direction);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Select )( 
            IScdNodes * This,
            /* [defaultvalue][in] */ eScdNodeTypes eType);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AddUnit )( 
            IScdNodes * This,
            /* [in] */ BSTR Class,
            /* [in] */ BSTR Tag,
            /* [retval][out] */ IScdNode **pUnit);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AddLink )( 
            IScdNodes * This,
            /* [in] */ BSTR Class,
            /* [in] */ BSTR Tag,
            /* [in] */ BSTR SrcTag,
            /* [in] */ BSTR SrcIO,
            /* [in] */ BSTR DstTag,
            /* [in] */ BSTR DstIO,
            /* [retval][out] */ IScdNode **pLink);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AddCable )( 
            IScdNodes * This,
            /* [in] */ BSTR Class,
            /* [in] */ BSTR Tag,
            /* [in] */ BSTR SrcTag,
            /* [in] */ BSTR SrcIO,
            /* [in] */ BSTR SrcTS,
            /* [in] */ BSTR DstTag,
            /* [in] */ BSTR DstIO,
            /* [in] */ BSTR DstTS,
            /* [retval][out] */ IScdNode **pLink);
        
        END_INTERFACE
    } IScdNodesVtbl;

    interface IScdNodes
    {
        CONST_VTBL struct IScdNodesVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdNodes_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdNodes_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdNodes_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdNodes_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdNodes_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdNodes_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdNodes_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IScdNodes_Node(This,WhichOne,pItem)	\
    (This)->lpVtbl -> Node(This,WhichOne,pItem)

#define IScdNodes_get__NewEnum(This,pVal)	\
    (This)->lpVtbl -> get__NewEnum(This,pVal)

#define IScdNodes_get_Count(This,pVal)	\
    (This)->lpVtbl -> get_Count(This,pVal)

#define IScdNodes_get_ReturnCode(This,pVal)	\
    (This)->lpVtbl -> get_ReturnCode(This,pVal)

#define IScdNodes_SetObjectInfo(This,What,Node,Types,Direction)	\
    (This)->lpVtbl -> SetObjectInfo(This,What,Node,Types,Direction)

#define IScdNodes_Select(This,eType)	\
    (This)->lpVtbl -> Select(This,eType)

#define IScdNodes_AddUnit(This,Class,Tag,pUnit)	\
    (This)->lpVtbl -> AddUnit(This,Class,Tag,pUnit)

#define IScdNodes_AddLink(This,Class,Tag,SrcTag,SrcIO,DstTag,DstIO,pLink)	\
    (This)->lpVtbl -> AddLink(This,Class,Tag,SrcTag,SrcIO,DstTag,DstIO,pLink)

#define IScdNodes_AddCable(This,Class,Tag,SrcTag,SrcIO,SrcTS,DstTag,DstIO,DstTS,pLink)	\
    (This)->lpVtbl -> AddCable(This,Class,Tag,SrcTag,SrcIO,SrcTS,DstTag,DstIO,DstTS,pLink)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdNodes_Node_Proxy( 
    IScdNodes * This,
    /* [in] */ VARIANT WhichOne,
    /* [retval][out] */ IScdNode **pItem);


void __RPC_STUB IScdNodes_Node_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE IScdNodes_get__NewEnum_Proxy( 
    IScdNodes * This,
    /* [retval][out] */ LPUNKNOWN *pVal);


void __RPC_STUB IScdNodes_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdNodes_get_Count_Proxy( 
    IScdNodes * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IScdNodes_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdNodes_get_ReturnCode_Proxy( 
    IScdNodes * This,
    /* [retval][out] */ eScdNodeCodes *pVal);


void __RPC_STUB IScdNodes_get_ReturnCode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][restricted][id] */ HRESULT STDMETHODCALLTYPE IScdNodes_SetObjectInfo_Proxy( 
    IScdNodes * This,
    /* [in] */ DWORD What,
    /* [in] */ DWORD Node,
    /* [in] */ DWORD Types,
    /* [in] */ DWORD Direction);


void __RPC_STUB IScdNodes_SetObjectInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdNodes_Select_Proxy( 
    IScdNodes * This,
    /* [defaultvalue][in] */ eScdNodeTypes eType);


void __RPC_STUB IScdNodes_Select_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdNodes_AddUnit_Proxy( 
    IScdNodes * This,
    /* [in] */ BSTR Class,
    /* [in] */ BSTR Tag,
    /* [retval][out] */ IScdNode **pUnit);


void __RPC_STUB IScdNodes_AddUnit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdNodes_AddLink_Proxy( 
    IScdNodes * This,
    /* [in] */ BSTR Class,
    /* [in] */ BSTR Tag,
    /* [in] */ BSTR SrcTag,
    /* [in] */ BSTR SrcIO,
    /* [in] */ BSTR DstTag,
    /* [in] */ BSTR DstIO,
    /* [retval][out] */ IScdNode **pLink);


void __RPC_STUB IScdNodes_AddLink_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdNodes_AddCable_Proxy( 
    IScdNodes * This,
    /* [in] */ BSTR Class,
    /* [in] */ BSTR Tag,
    /* [in] */ BSTR SrcTag,
    /* [in] */ BSTR SrcIO,
    /* [in] */ BSTR SrcTS,
    /* [in] */ BSTR DstTag,
    /* [in] */ BSTR DstIO,
    /* [in] */ BSTR DstTS,
    /* [retval][out] */ IScdNode **pLink);


void __RPC_STUB IScdNodes_AddCable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScdNodes_INTERFACE_DEFINED__ */


#ifndef __IScdNodeClass_INTERFACE_DEFINED__
#define __IScdNodeClass_INTERFACE_DEFINED__

/* interface IScdNodeClass */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdNodeClass;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3E92067C-548A-493E-B4C6-D4D9CAB640C0")
    IScdNodeClass : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Description( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ShortDescription( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Library( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_DefaultTag( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Category( 
            /* [retval][out] */ eScdNodeCat_Flags *pVal) = 0;
        
        virtual /* [helpstring][restricted][id] */ HRESULT STDMETHODCALLTYPE SetObjectInfo( 
            /* [in] */ DWORD newVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScdNodeClassVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdNodeClass * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdNodeClass * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdNodeClass * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdNodeClass * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdNodeClass * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdNodeClass * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdNodeClass * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Description )( 
            IScdNodeClass * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ShortDescription )( 
            IScdNodeClass * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Library )( 
            IScdNodeClass * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DefaultTag )( 
            IScdNodeClass * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            IScdNodeClass * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Category )( 
            IScdNodeClass * This,
            /* [retval][out] */ eScdNodeCat_Flags *pVal);
        
        /* [helpstring][restricted][id] */ HRESULT ( STDMETHODCALLTYPE *SetObjectInfo )( 
            IScdNodeClass * This,
            /* [in] */ DWORD newVal);
        
        END_INTERFACE
    } IScdNodeClassVtbl;

    interface IScdNodeClass
    {
        CONST_VTBL struct IScdNodeClassVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdNodeClass_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdNodeClass_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdNodeClass_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdNodeClass_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdNodeClass_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdNodeClass_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdNodeClass_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IScdNodeClass_get_Description(This,pVal)	\
    (This)->lpVtbl -> get_Description(This,pVal)

#define IScdNodeClass_get_ShortDescription(This,pVal)	\
    (This)->lpVtbl -> get_ShortDescription(This,pVal)

#define IScdNodeClass_get_Library(This,pVal)	\
    (This)->lpVtbl -> get_Library(This,pVal)

#define IScdNodeClass_get_DefaultTag(This,pVal)	\
    (This)->lpVtbl -> get_DefaultTag(This,pVal)

#define IScdNodeClass_get_Name(This,pVal)	\
    (This)->lpVtbl -> get_Name(This,pVal)

#define IScdNodeClass_get_Category(This,pVal)	\
    (This)->lpVtbl -> get_Category(This,pVal)

#define IScdNodeClass_SetObjectInfo(This,newVal)	\
    (This)->lpVtbl -> SetObjectInfo(This,newVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdNodeClass_get_Description_Proxy( 
    IScdNodeClass * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IScdNodeClass_get_Description_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdNodeClass_get_ShortDescription_Proxy( 
    IScdNodeClass * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IScdNodeClass_get_ShortDescription_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdNodeClass_get_Library_Proxy( 
    IScdNodeClass * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IScdNodeClass_get_Library_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdNodeClass_get_DefaultTag_Proxy( 
    IScdNodeClass * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IScdNodeClass_get_DefaultTag_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdNodeClass_get_Name_Proxy( 
    IScdNodeClass * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IScdNodeClass_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdNodeClass_get_Category_Proxy( 
    IScdNodeClass * This,
    /* [retval][out] */ eScdNodeCat_Flags *pVal);


void __RPC_STUB IScdNodeClass_get_Category_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][restricted][id] */ HRESULT STDMETHODCALLTYPE IScdNodeClass_SetObjectInfo_Proxy( 
    IScdNodeClass * This,
    /* [in] */ DWORD newVal);


void __RPC_STUB IScdNodeClass_SetObjectInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScdNodeClass_INTERFACE_DEFINED__ */


#ifndef __IScdConnect_INTERFACE_DEFINED__
#define __IScdConnect_INTERFACE_DEFINED__

/* interface IScdConnect */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdConnect;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1CD389F3-67DD-4E46-85C9-C40A6E49F8D9")
    IScdConnect : public IDispatch
    {
    public:
        virtual /* [helpstring][restricted][id] */ HRESULT STDMETHODCALLTYPE SetObjectInfo( 
            /* [in] */ DWORD newVal,
            /* [in] */ long Index) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Index( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_RmtIndex( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_RmtNode( 
            /* [retval][out] */ IScdNode **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ConnectString( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_RmtConnectString( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ConnectPoint( 
            /* [retval][out] */ IScdConnectPoint **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_RmtConnectPoint( 
            /* [retval][out] */ IScdConnectPoint **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Node( 
            /* [retval][out] */ IScdNode **pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScdConnectVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdConnect * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdConnect * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdConnect * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdConnect * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdConnect * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdConnect * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdConnect * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][restricted][id] */ HRESULT ( STDMETHODCALLTYPE *SetObjectInfo )( 
            IScdConnect * This,
            /* [in] */ DWORD newVal,
            /* [in] */ long Index);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Index )( 
            IScdConnect * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_RmtIndex )( 
            IScdConnect * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_RmtNode )( 
            IScdConnect * This,
            /* [retval][out] */ IScdNode **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ConnectString )( 
            IScdConnect * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_RmtConnectString )( 
            IScdConnect * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ConnectPoint )( 
            IScdConnect * This,
            /* [retval][out] */ IScdConnectPoint **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_RmtConnectPoint )( 
            IScdConnect * This,
            /* [retval][out] */ IScdConnectPoint **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Node )( 
            IScdConnect * This,
            /* [retval][out] */ IScdNode **pVal);
        
        END_INTERFACE
    } IScdConnectVtbl;

    interface IScdConnect
    {
        CONST_VTBL struct IScdConnectVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdConnect_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdConnect_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdConnect_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdConnect_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdConnect_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdConnect_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdConnect_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IScdConnect_SetObjectInfo(This,newVal,Index)	\
    (This)->lpVtbl -> SetObjectInfo(This,newVal,Index)

#define IScdConnect_get_Index(This,pVal)	\
    (This)->lpVtbl -> get_Index(This,pVal)

#define IScdConnect_get_RmtIndex(This,pVal)	\
    (This)->lpVtbl -> get_RmtIndex(This,pVal)

#define IScdConnect_get_RmtNode(This,pVal)	\
    (This)->lpVtbl -> get_RmtNode(This,pVal)

#define IScdConnect_get_ConnectString(This,pVal)	\
    (This)->lpVtbl -> get_ConnectString(This,pVal)

#define IScdConnect_get_RmtConnectString(This,pVal)	\
    (This)->lpVtbl -> get_RmtConnectString(This,pVal)

#define IScdConnect_get_ConnectPoint(This,pVal)	\
    (This)->lpVtbl -> get_ConnectPoint(This,pVal)

#define IScdConnect_get_RmtConnectPoint(This,pVal)	\
    (This)->lpVtbl -> get_RmtConnectPoint(This,pVal)

#define IScdConnect_get_Node(This,pVal)	\
    (This)->lpVtbl -> get_Node(This,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][restricted][id] */ HRESULT STDMETHODCALLTYPE IScdConnect_SetObjectInfo_Proxy( 
    IScdConnect * This,
    /* [in] */ DWORD newVal,
    /* [in] */ long Index);


void __RPC_STUB IScdConnect_SetObjectInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdConnect_get_Index_Proxy( 
    IScdConnect * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IScdConnect_get_Index_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdConnect_get_RmtIndex_Proxy( 
    IScdConnect * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IScdConnect_get_RmtIndex_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdConnect_get_RmtNode_Proxy( 
    IScdConnect * This,
    /* [retval][out] */ IScdNode **pVal);


void __RPC_STUB IScdConnect_get_RmtNode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdConnect_get_ConnectString_Proxy( 
    IScdConnect * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IScdConnect_get_ConnectString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdConnect_get_RmtConnectString_Proxy( 
    IScdConnect * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IScdConnect_get_RmtConnectString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdConnect_get_ConnectPoint_Proxy( 
    IScdConnect * This,
    /* [retval][out] */ IScdConnectPoint **pVal);


void __RPC_STUB IScdConnect_get_ConnectPoint_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdConnect_get_RmtConnectPoint_Proxy( 
    IScdConnect * This,
    /* [retval][out] */ IScdConnectPoint **pVal);


void __RPC_STUB IScdConnect_get_RmtConnectPoint_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdConnect_get_Node_Proxy( 
    IScdConnect * This,
    /* [retval][out] */ IScdNode **pVal);


void __RPC_STUB IScdConnect_get_Node_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScdConnect_INTERFACE_DEFINED__ */


#ifndef __IScdConnects_INTERFACE_DEFINED__
#define __IScdConnects_INTERFACE_DEFINED__

/* interface IScdConnects */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdConnects;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("630D2D53-76B3-41E1-A5EF-01501E42FFC8")
    IScdConnects : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Connect( 
            /* [in] */ VARIANT WhichOne,
            /* [retval][out] */ IScdConnect **pItem) = 0;
        
        virtual /* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ LPUNKNOWN *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ReturnCode( 
            /* [retval][out] */ eScdNodeCodes *pVal) = 0;
        
        virtual /* [helpstring][restricted][id] */ HRESULT STDMETHODCALLTYPE SetObjectInfo( 
            /* [in] */ DWORD newVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScdConnectsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdConnects * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdConnects * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdConnects * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdConnects * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdConnects * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdConnects * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdConnects * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Connect )( 
            IScdConnects * This,
            /* [in] */ VARIANT WhichOne,
            /* [retval][out] */ IScdConnect **pItem);
        
        /* [id][restricted][propget] */ HRESULT ( STDMETHODCALLTYPE *get__NewEnum )( 
            IScdConnects * This,
            /* [retval][out] */ LPUNKNOWN *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            IScdConnects * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ReturnCode )( 
            IScdConnects * This,
            /* [retval][out] */ eScdNodeCodes *pVal);
        
        /* [helpstring][restricted][id] */ HRESULT ( STDMETHODCALLTYPE *SetObjectInfo )( 
            IScdConnects * This,
            /* [in] */ DWORD newVal);
        
        END_INTERFACE
    } IScdConnectsVtbl;

    interface IScdConnects
    {
        CONST_VTBL struct IScdConnectsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdConnects_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdConnects_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdConnects_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdConnects_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdConnects_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdConnects_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdConnects_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IScdConnects_Connect(This,WhichOne,pItem)	\
    (This)->lpVtbl -> Connect(This,WhichOne,pItem)

#define IScdConnects_get__NewEnum(This,pVal)	\
    (This)->lpVtbl -> get__NewEnum(This,pVal)

#define IScdConnects_get_Count(This,pVal)	\
    (This)->lpVtbl -> get_Count(This,pVal)

#define IScdConnects_get_ReturnCode(This,pVal)	\
    (This)->lpVtbl -> get_ReturnCode(This,pVal)

#define IScdConnects_SetObjectInfo(This,newVal)	\
    (This)->lpVtbl -> SetObjectInfo(This,newVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdConnects_Connect_Proxy( 
    IScdConnects * This,
    /* [in] */ VARIANT WhichOne,
    /* [retval][out] */ IScdConnect **pItem);


void __RPC_STUB IScdConnects_Connect_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE IScdConnects_get__NewEnum_Proxy( 
    IScdConnects * This,
    /* [retval][out] */ LPUNKNOWN *pVal);


void __RPC_STUB IScdConnects_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdConnects_get_Count_Proxy( 
    IScdConnects * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IScdConnects_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdConnects_get_ReturnCode_Proxy( 
    IScdConnects * This,
    /* [retval][out] */ eScdNodeCodes *pVal);


void __RPC_STUB IScdConnects_get_ReturnCode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][restricted][id] */ HRESULT STDMETHODCALLTYPE IScdConnects_SetObjectInfo_Proxy( 
    IScdConnects * This,
    /* [in] */ DWORD newVal);


void __RPC_STUB IScdConnects_SetObjectInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScdConnects_INTERFACE_DEFINED__ */


#ifndef __IScdConnectPoint_INTERFACE_DEFINED__
#define __IScdConnectPoint_INTERFACE_DEFINED__

/* interface IScdConnectPoint */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdConnectPoint;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("128122C7-F846-4730-88E0-3FF8844153A0")
    IScdConnectPoint : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Description( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Input( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Output( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Type( 
            /* [retval][out] */ eScdNodeTypes *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_TypeString( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ConnectCountRqd( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ConnectCountMax( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][restricted][id] */ HRESULT STDMETHODCALLTYPE SetObjectInfo( 
            /* [in] */ DWORD newVal,
            /* [in] */ long Index) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Index( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ID( 
            /* [retval][out] */ long *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScdConnectPointVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdConnectPoint * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdConnectPoint * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdConnectPoint * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdConnectPoint * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdConnectPoint * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdConnectPoint * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdConnectPoint * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            IScdConnectPoint * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Description )( 
            IScdConnectPoint * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Input )( 
            IScdConnectPoint * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Output )( 
            IScdConnectPoint * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Type )( 
            IScdConnectPoint * This,
            /* [retval][out] */ eScdNodeTypes *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_TypeString )( 
            IScdConnectPoint * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ConnectCountRqd )( 
            IScdConnectPoint * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ConnectCountMax )( 
            IScdConnectPoint * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][restricted][id] */ HRESULT ( STDMETHODCALLTYPE *SetObjectInfo )( 
            IScdConnectPoint * This,
            /* [in] */ DWORD newVal,
            /* [in] */ long Index);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Index )( 
            IScdConnectPoint * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ID )( 
            IScdConnectPoint * This,
            /* [retval][out] */ long *pVal);
        
        END_INTERFACE
    } IScdConnectPointVtbl;

    interface IScdConnectPoint
    {
        CONST_VTBL struct IScdConnectPointVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdConnectPoint_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdConnectPoint_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdConnectPoint_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdConnectPoint_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdConnectPoint_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdConnectPoint_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdConnectPoint_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IScdConnectPoint_get_Name(This,pVal)	\
    (This)->lpVtbl -> get_Name(This,pVal)

#define IScdConnectPoint_get_Description(This,pVal)	\
    (This)->lpVtbl -> get_Description(This,pVal)

#define IScdConnectPoint_get_Input(This,pVal)	\
    (This)->lpVtbl -> get_Input(This,pVal)

#define IScdConnectPoint_get_Output(This,pVal)	\
    (This)->lpVtbl -> get_Output(This,pVal)

#define IScdConnectPoint_get_Type(This,pVal)	\
    (This)->lpVtbl -> get_Type(This,pVal)

#define IScdConnectPoint_get_TypeString(This,pVal)	\
    (This)->lpVtbl -> get_TypeString(This,pVal)

#define IScdConnectPoint_get_ConnectCountRqd(This,pVal)	\
    (This)->lpVtbl -> get_ConnectCountRqd(This,pVal)

#define IScdConnectPoint_get_ConnectCountMax(This,pVal)	\
    (This)->lpVtbl -> get_ConnectCountMax(This,pVal)

#define IScdConnectPoint_SetObjectInfo(This,newVal,Index)	\
    (This)->lpVtbl -> SetObjectInfo(This,newVal,Index)

#define IScdConnectPoint_get_Index(This,pVal)	\
    (This)->lpVtbl -> get_Index(This,pVal)

#define IScdConnectPoint_get_ID(This,pVal)	\
    (This)->lpVtbl -> get_ID(This,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdConnectPoint_get_Name_Proxy( 
    IScdConnectPoint * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IScdConnectPoint_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdConnectPoint_get_Description_Proxy( 
    IScdConnectPoint * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IScdConnectPoint_get_Description_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdConnectPoint_get_Input_Proxy( 
    IScdConnectPoint * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB IScdConnectPoint_get_Input_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdConnectPoint_get_Output_Proxy( 
    IScdConnectPoint * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB IScdConnectPoint_get_Output_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdConnectPoint_get_Type_Proxy( 
    IScdConnectPoint * This,
    /* [retval][out] */ eScdNodeTypes *pVal);


void __RPC_STUB IScdConnectPoint_get_Type_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdConnectPoint_get_TypeString_Proxy( 
    IScdConnectPoint * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IScdConnectPoint_get_TypeString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdConnectPoint_get_ConnectCountRqd_Proxy( 
    IScdConnectPoint * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IScdConnectPoint_get_ConnectCountRqd_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdConnectPoint_get_ConnectCountMax_Proxy( 
    IScdConnectPoint * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IScdConnectPoint_get_ConnectCountMax_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][restricted][id] */ HRESULT STDMETHODCALLTYPE IScdConnectPoint_SetObjectInfo_Proxy( 
    IScdConnectPoint * This,
    /* [in] */ DWORD newVal,
    /* [in] */ long Index);


void __RPC_STUB IScdConnectPoint_SetObjectInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdConnectPoint_get_Index_Proxy( 
    IScdConnectPoint * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IScdConnectPoint_get_Index_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdConnectPoint_get_ID_Proxy( 
    IScdConnectPoint * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IScdConnectPoint_get_ID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScdConnectPoint_INTERFACE_DEFINED__ */


#ifndef __IScdConnectPoints_INTERFACE_DEFINED__
#define __IScdConnectPoints_INTERFACE_DEFINED__

/* interface IScdConnectPoints */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdConnectPoints;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("045C8C54-8CCF-4AB7-B56E-9D26CB725D19")
    IScdConnectPoints : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ConnectPoint( 
            /* [in] */ VARIANT WhichOne,
            /* [retval][out] */ IScdConnectPoint **pItem) = 0;
        
        virtual /* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ LPUNKNOWN *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ReturnCode( 
            /* [retval][out] */ eScdNodeCodes *pVal) = 0;
        
        virtual /* [helpstring][restricted][id] */ HRESULT STDMETHODCALLTYPE SetObjectInfo( 
            /* [in] */ DWORD newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_InputPathName( 
            /* [in] */ eScdLinkTypes Type,
            /* [defaultvalue][in] */ long PathNo,
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_OutputPathName( 
            /* [in] */ eScdLinkTypes Type,
            /* [defaultvalue][in] */ long PathNo,
            /* [retval][out] */ BSTR *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScdConnectPointsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdConnectPoints * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdConnectPoints * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdConnectPoints * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdConnectPoints * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdConnectPoints * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdConnectPoints * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdConnectPoints * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ConnectPoint )( 
            IScdConnectPoints * This,
            /* [in] */ VARIANT WhichOne,
            /* [retval][out] */ IScdConnectPoint **pItem);
        
        /* [id][restricted][propget] */ HRESULT ( STDMETHODCALLTYPE *get__NewEnum )( 
            IScdConnectPoints * This,
            /* [retval][out] */ LPUNKNOWN *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            IScdConnectPoints * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ReturnCode )( 
            IScdConnectPoints * This,
            /* [retval][out] */ eScdNodeCodes *pVal);
        
        /* [helpstring][restricted][id] */ HRESULT ( STDMETHODCALLTYPE *SetObjectInfo )( 
            IScdConnectPoints * This,
            /* [in] */ DWORD newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_InputPathName )( 
            IScdConnectPoints * This,
            /* [in] */ eScdLinkTypes Type,
            /* [defaultvalue][in] */ long PathNo,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_OutputPathName )( 
            IScdConnectPoints * This,
            /* [in] */ eScdLinkTypes Type,
            /* [defaultvalue][in] */ long PathNo,
            /* [retval][out] */ BSTR *pVal);
        
        END_INTERFACE
    } IScdConnectPointsVtbl;

    interface IScdConnectPoints
    {
        CONST_VTBL struct IScdConnectPointsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdConnectPoints_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdConnectPoints_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdConnectPoints_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdConnectPoints_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdConnectPoints_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdConnectPoints_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdConnectPoints_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IScdConnectPoints_ConnectPoint(This,WhichOne,pItem)	\
    (This)->lpVtbl -> ConnectPoint(This,WhichOne,pItem)

#define IScdConnectPoints_get__NewEnum(This,pVal)	\
    (This)->lpVtbl -> get__NewEnum(This,pVal)

#define IScdConnectPoints_get_Count(This,pVal)	\
    (This)->lpVtbl -> get_Count(This,pVal)

#define IScdConnectPoints_get_ReturnCode(This,pVal)	\
    (This)->lpVtbl -> get_ReturnCode(This,pVal)

#define IScdConnectPoints_SetObjectInfo(This,newVal)	\
    (This)->lpVtbl -> SetObjectInfo(This,newVal)

#define IScdConnectPoints_get_InputPathName(This,Type,PathNo,pVal)	\
    (This)->lpVtbl -> get_InputPathName(This,Type,PathNo,pVal)

#define IScdConnectPoints_get_OutputPathName(This,Type,PathNo,pVal)	\
    (This)->lpVtbl -> get_OutputPathName(This,Type,PathNo,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdConnectPoints_ConnectPoint_Proxy( 
    IScdConnectPoints * This,
    /* [in] */ VARIANT WhichOne,
    /* [retval][out] */ IScdConnectPoint **pItem);


void __RPC_STUB IScdConnectPoints_ConnectPoint_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE IScdConnectPoints_get__NewEnum_Proxy( 
    IScdConnectPoints * This,
    /* [retval][out] */ LPUNKNOWN *pVal);


void __RPC_STUB IScdConnectPoints_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdConnectPoints_get_Count_Proxy( 
    IScdConnectPoints * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IScdConnectPoints_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdConnectPoints_get_ReturnCode_Proxy( 
    IScdConnectPoints * This,
    /* [retval][out] */ eScdNodeCodes *pVal);


void __RPC_STUB IScdConnectPoints_get_ReturnCode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][restricted][id] */ HRESULT STDMETHODCALLTYPE IScdConnectPoints_SetObjectInfo_Proxy( 
    IScdConnectPoints * This,
    /* [in] */ DWORD newVal);


void __RPC_STUB IScdConnectPoints_SetObjectInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdConnectPoints_get_InputPathName_Proxy( 
    IScdConnectPoints * This,
    /* [in] */ eScdLinkTypes Type,
    /* [defaultvalue][in] */ long PathNo,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IScdConnectPoints_get_InputPathName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdConnectPoints_get_OutputPathName_Proxy( 
    IScdConnectPoints * This,
    /* [in] */ eScdLinkTypes Type,
    /* [defaultvalue][in] */ long PathNo,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IScdConnectPoints_get_OutputPathName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScdConnectPoints_INTERFACE_DEFINED__ */


#ifndef __IScdProbal_INTERFACE_DEFINED__
#define __IScdProbal_INTERFACE_DEFINED__

/* interface IScdProbal */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdProbal;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A36FFC98-0C3B-4B56-BF39-0125C974AF80")
    IScdProbal : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_IsStopped( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_IsIdling( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_IsRunning( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Start( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Step( 
            /* [defaultvalue][in] */ long IterMark = 0,
            /* [defaultvalue][in] */ double WaitForNext = 0) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Stop( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Idle( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScdProbalVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdProbal * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdProbal * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdProbal * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdProbal * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdProbal * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdProbal * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdProbal * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsStopped )( 
            IScdProbal * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsIdling )( 
            IScdProbal * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsRunning )( 
            IScdProbal * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Start )( 
            IScdProbal * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Step )( 
            IScdProbal * This,
            /* [defaultvalue][in] */ long IterMark,
            /* [defaultvalue][in] */ double WaitForNext);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Stop )( 
            IScdProbal * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Idle )( 
            IScdProbal * This);
        
        END_INTERFACE
    } IScdProbalVtbl;

    interface IScdProbal
    {
        CONST_VTBL struct IScdProbalVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdProbal_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdProbal_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdProbal_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdProbal_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdProbal_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdProbal_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdProbal_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IScdProbal_get_IsStopped(This,pVal)	\
    (This)->lpVtbl -> get_IsStopped(This,pVal)

#define IScdProbal_get_IsIdling(This,pVal)	\
    (This)->lpVtbl -> get_IsIdling(This,pVal)

#define IScdProbal_get_IsRunning(This,pVal)	\
    (This)->lpVtbl -> get_IsRunning(This,pVal)

#define IScdProbal_Start(This)	\
    (This)->lpVtbl -> Start(This)

#define IScdProbal_Step(This,IterMark,WaitForNext)	\
    (This)->lpVtbl -> Step(This,IterMark,WaitForNext)

#define IScdProbal_Stop(This)	\
    (This)->lpVtbl -> Stop(This)

#define IScdProbal_Idle(This)	\
    (This)->lpVtbl -> Idle(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdProbal_get_IsStopped_Proxy( 
    IScdProbal * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB IScdProbal_get_IsStopped_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdProbal_get_IsIdling_Proxy( 
    IScdProbal * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB IScdProbal_get_IsIdling_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdProbal_get_IsRunning_Proxy( 
    IScdProbal * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB IScdProbal_get_IsRunning_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdProbal_Start_Proxy( 
    IScdProbal * This);


void __RPC_STUB IScdProbal_Start_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdProbal_Step_Proxy( 
    IScdProbal * This,
    /* [defaultvalue][in] */ long IterMark,
    /* [defaultvalue][in] */ double WaitForNext);


void __RPC_STUB IScdProbal_Step_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdProbal_Stop_Proxy( 
    IScdProbal * This);


void __RPC_STUB IScdProbal_Stop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdProbal_Idle_Proxy( 
    IScdProbal * This);


void __RPC_STUB IScdProbal_Idle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScdProbal_INTERFACE_DEFINED__ */


#ifndef __IScdSolver_INTERFACE_DEFINED__
#define __IScdSolver_INTERFACE_DEFINED__

/* interface IScdSolver */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdSolver;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("851C57EA-F1E5-4ff8-9518-4B91B7CB7830")
    IScdSolver : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE InitialiseCfg( 
            /* [in] */ BSTR CfgFileName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE InitialiseDB( 
            /* [in] */ BSTR CfgFileName,
            /* [in] */ SAFEARRAY * parrSpecies) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Messages( 
            /* [retval][out] */ IScdMessages **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SpecieDefns( 
            /* [retval][out] */ /* external definition not present */ IScdSpecieDefns **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Tags( 
            /* [retval][out] */ IScdSlvTags **pTags) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Debug( 
            /* [retval][out] */ IScdDebug **pDebug) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Test( 
            /* [retval][out] */ IScdTest **pTest) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Nodes( 
            /* [defaultvalue][in] */ eScdNodeTypes eType,
            /* [retval][out] */ IScdNodes **pNodes) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Probal( 
            /* [retval][out] */ IScdProbal **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Dynamic( 
            /* [retval][out] */ IScdDynamic **pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateSpModel( 
            /* [in] */ BSTR ProgID,
            /* [retval][out] */ IScdTaggedObject **pModel) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CollectEvalSequenceInfo( 
            /* [in] */ eScdEvalSeq_Options eReqdSeq,
            /* [out] */ long *ItemCount) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetEvalSequenceItem( 
            /* [in] */ long Index,
            /* [out] */ BSTR *Tag,
            /* [out] */ BSTR *Class,
            /* [out] */ long *IOCount) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetEvalSequenceIOItem( 
            /* [in] */ long Index,
            /* [in] */ long IOItem,
            /* [out] */ eScdEvalSeqIO_Class *eClass,
            /* [out] */ eScdEvalSeqIO_Flags *eFlags,
            /* [out] */ long *RemoteIndex,
            /* [out] */ BSTR *RemoteDesc) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_RunMode( 
            /* [retval][out] */ eScdMode *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_RunMode( 
            /* [in] */ eScdMode newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Conversions( 
            /* [retval][out] */ /* external definition not present */ IScdConversion **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Time( 
            /* [defaultvalue][in] */ eScdTimeFormat Fmt,
            /* [retval][out] */ VARIANT *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Time( 
            /* [defaultvalue][in] */ eScdTimeFormat Fmt,
            /* [in] */ VARIANT newVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScdSolverVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdSolver * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdSolver * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdSolver * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdSolver * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdSolver * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdSolver * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdSolver * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *InitialiseCfg )( 
            IScdSolver * This,
            /* [in] */ BSTR CfgFileName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *InitialiseDB )( 
            IScdSolver * This,
            /* [in] */ BSTR CfgFileName,
            /* [in] */ SAFEARRAY * parrSpecies);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Messages )( 
            IScdSolver * This,
            /* [retval][out] */ IScdMessages **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SpecieDefns )( 
            IScdSolver * This,
            /* [retval][out] */ /* external definition not present */ IScdSpecieDefns **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Tags )( 
            IScdSolver * This,
            /* [retval][out] */ IScdSlvTags **pTags);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Debug )( 
            IScdSolver * This,
            /* [retval][out] */ IScdDebug **pDebug);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Test )( 
            IScdSolver * This,
            /* [retval][out] */ IScdTest **pTest);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Nodes )( 
            IScdSolver * This,
            /* [defaultvalue][in] */ eScdNodeTypes eType,
            /* [retval][out] */ IScdNodes **pNodes);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Probal )( 
            IScdSolver * This,
            /* [retval][out] */ IScdProbal **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Dynamic )( 
            IScdSolver * This,
            /* [retval][out] */ IScdDynamic **pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateSpModel )( 
            IScdSolver * This,
            /* [in] */ BSTR ProgID,
            /* [retval][out] */ IScdTaggedObject **pModel);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CollectEvalSequenceInfo )( 
            IScdSolver * This,
            /* [in] */ eScdEvalSeq_Options eReqdSeq,
            /* [out] */ long *ItemCount);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetEvalSequenceItem )( 
            IScdSolver * This,
            /* [in] */ long Index,
            /* [out] */ BSTR *Tag,
            /* [out] */ BSTR *Class,
            /* [out] */ long *IOCount);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetEvalSequenceIOItem )( 
            IScdSolver * This,
            /* [in] */ long Index,
            /* [in] */ long IOItem,
            /* [out] */ eScdEvalSeqIO_Class *eClass,
            /* [out] */ eScdEvalSeqIO_Flags *eFlags,
            /* [out] */ long *RemoteIndex,
            /* [out] */ BSTR *RemoteDesc);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_RunMode )( 
            IScdSolver * This,
            /* [retval][out] */ eScdMode *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_RunMode )( 
            IScdSolver * This,
            /* [in] */ eScdMode newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Conversions )( 
            IScdSolver * This,
            /* [retval][out] */ /* external definition not present */ IScdConversion **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Time )( 
            IScdSolver * This,
            /* [defaultvalue][in] */ eScdTimeFormat Fmt,
            /* [retval][out] */ VARIANT *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Time )( 
            IScdSolver * This,
            /* [defaultvalue][in] */ eScdTimeFormat Fmt,
            /* [in] */ VARIANT newVal);
        
        END_INTERFACE
    } IScdSolverVtbl;

    interface IScdSolver
    {
        CONST_VTBL struct IScdSolverVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdSolver_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdSolver_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdSolver_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdSolver_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdSolver_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdSolver_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdSolver_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IScdSolver_InitialiseCfg(This,CfgFileName)	\
    (This)->lpVtbl -> InitialiseCfg(This,CfgFileName)

#define IScdSolver_InitialiseDB(This,CfgFileName,parrSpecies)	\
    (This)->lpVtbl -> InitialiseDB(This,CfgFileName,parrSpecies)

#define IScdSolver_get_Messages(This,pVal)	\
    (This)->lpVtbl -> get_Messages(This,pVal)

#define IScdSolver_get_SpecieDefns(This,pVal)	\
    (This)->lpVtbl -> get_SpecieDefns(This,pVal)

#define IScdSolver_get_Tags(This,pTags)	\
    (This)->lpVtbl -> get_Tags(This,pTags)

#define IScdSolver_get_Debug(This,pDebug)	\
    (This)->lpVtbl -> get_Debug(This,pDebug)

#define IScdSolver_get_Test(This,pTest)	\
    (This)->lpVtbl -> get_Test(This,pTest)

#define IScdSolver_get_Nodes(This,eType,pNodes)	\
    (This)->lpVtbl -> get_Nodes(This,eType,pNodes)

#define IScdSolver_get_Probal(This,pVal)	\
    (This)->lpVtbl -> get_Probal(This,pVal)

#define IScdSolver_get_Dynamic(This,pVal)	\
    (This)->lpVtbl -> get_Dynamic(This,pVal)

#define IScdSolver_CreateSpModel(This,ProgID,pModel)	\
    (This)->lpVtbl -> CreateSpModel(This,ProgID,pModel)

#define IScdSolver_CollectEvalSequenceInfo(This,eReqdSeq,ItemCount)	\
    (This)->lpVtbl -> CollectEvalSequenceInfo(This,eReqdSeq,ItemCount)

#define IScdSolver_GetEvalSequenceItem(This,Index,Tag,Class,IOCount)	\
    (This)->lpVtbl -> GetEvalSequenceItem(This,Index,Tag,Class,IOCount)

#define IScdSolver_GetEvalSequenceIOItem(This,Index,IOItem,eClass,eFlags,RemoteIndex,RemoteDesc)	\
    (This)->lpVtbl -> GetEvalSequenceIOItem(This,Index,IOItem,eClass,eFlags,RemoteIndex,RemoteDesc)

#define IScdSolver_get_RunMode(This,pVal)	\
    (This)->lpVtbl -> get_RunMode(This,pVal)

#define IScdSolver_put_RunMode(This,newVal)	\
    (This)->lpVtbl -> put_RunMode(This,newVal)

#define IScdSolver_get_Conversions(This,pVal)	\
    (This)->lpVtbl -> get_Conversions(This,pVal)

#define IScdSolver_get_Time(This,Fmt,pVal)	\
    (This)->lpVtbl -> get_Time(This,Fmt,pVal)

#define IScdSolver_put_Time(This,Fmt,newVal)	\
    (This)->lpVtbl -> put_Time(This,Fmt,newVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdSolver_InitialiseCfg_Proxy( 
    IScdSolver * This,
    /* [in] */ BSTR CfgFileName);


void __RPC_STUB IScdSolver_InitialiseCfg_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdSolver_InitialiseDB_Proxy( 
    IScdSolver * This,
    /* [in] */ BSTR CfgFileName,
    /* [in] */ SAFEARRAY * parrSpecies);


void __RPC_STUB IScdSolver_InitialiseDB_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSolver_get_Messages_Proxy( 
    IScdSolver * This,
    /* [retval][out] */ IScdMessages **pVal);


void __RPC_STUB IScdSolver_get_Messages_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSolver_get_SpecieDefns_Proxy( 
    IScdSolver * This,
    /* [retval][out] */ /* external definition not present */ IScdSpecieDefns **pVal);


void __RPC_STUB IScdSolver_get_SpecieDefns_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSolver_get_Tags_Proxy( 
    IScdSolver * This,
    /* [retval][out] */ IScdSlvTags **pTags);


void __RPC_STUB IScdSolver_get_Tags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSolver_get_Debug_Proxy( 
    IScdSolver * This,
    /* [retval][out] */ IScdDebug **pDebug);


void __RPC_STUB IScdSolver_get_Debug_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSolver_get_Test_Proxy( 
    IScdSolver * This,
    /* [retval][out] */ IScdTest **pTest);


void __RPC_STUB IScdSolver_get_Test_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSolver_get_Nodes_Proxy( 
    IScdSolver * This,
    /* [defaultvalue][in] */ eScdNodeTypes eType,
    /* [retval][out] */ IScdNodes **pNodes);


void __RPC_STUB IScdSolver_get_Nodes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSolver_get_Probal_Proxy( 
    IScdSolver * This,
    /* [retval][out] */ IScdProbal **pVal);


void __RPC_STUB IScdSolver_get_Probal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSolver_get_Dynamic_Proxy( 
    IScdSolver * This,
    /* [retval][out] */ IScdDynamic **pVal);


void __RPC_STUB IScdSolver_get_Dynamic_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdSolver_CreateSpModel_Proxy( 
    IScdSolver * This,
    /* [in] */ BSTR ProgID,
    /* [retval][out] */ IScdTaggedObject **pModel);


void __RPC_STUB IScdSolver_CreateSpModel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdSolver_CollectEvalSequenceInfo_Proxy( 
    IScdSolver * This,
    /* [in] */ eScdEvalSeq_Options eReqdSeq,
    /* [out] */ long *ItemCount);


void __RPC_STUB IScdSolver_CollectEvalSequenceInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdSolver_GetEvalSequenceItem_Proxy( 
    IScdSolver * This,
    /* [in] */ long Index,
    /* [out] */ BSTR *Tag,
    /* [out] */ BSTR *Class,
    /* [out] */ long *IOCount);


void __RPC_STUB IScdSolver_GetEvalSequenceItem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdSolver_GetEvalSequenceIOItem_Proxy( 
    IScdSolver * This,
    /* [in] */ long Index,
    /* [in] */ long IOItem,
    /* [out] */ eScdEvalSeqIO_Class *eClass,
    /* [out] */ eScdEvalSeqIO_Flags *eFlags,
    /* [out] */ long *RemoteIndex,
    /* [out] */ BSTR *RemoteDesc);


void __RPC_STUB IScdSolver_GetEvalSequenceIOItem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSolver_get_RunMode_Proxy( 
    IScdSolver * This,
    /* [retval][out] */ eScdMode *pVal);


void __RPC_STUB IScdSolver_get_RunMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IScdSolver_put_RunMode_Proxy( 
    IScdSolver * This,
    /* [in] */ eScdMode newVal);


void __RPC_STUB IScdSolver_put_RunMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSolver_get_Conversions_Proxy( 
    IScdSolver * This,
    /* [retval][out] */ /* external definition not present */ IScdConversion **pVal);


void __RPC_STUB IScdSolver_get_Conversions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSolver_get_Time_Proxy( 
    IScdSolver * This,
    /* [defaultvalue][in] */ eScdTimeFormat Fmt,
    /* [retval][out] */ VARIANT *pVal);


void __RPC_STUB IScdSolver_get_Time_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IScdSolver_put_Time_Proxy( 
    IScdSolver * This,
    /* [defaultvalue][in] */ eScdTimeFormat Fmt,
    /* [in] */ VARIANT newVal);


void __RPC_STUB IScdSolver_put_Time_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScdSolver_INTERFACE_DEFINED__ */


#ifndef __IScdSlvTag_INTERFACE_DEFINED__
#define __IScdSlvTag_INTERFACE_DEFINED__

/* interface IScdSlvTag */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdSlvTag;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("8EA3D98A-AAC9-4104-8E2C-EEF897FBE06B")
    IScdSlvTag : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IScdSlvTagVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdSlvTag * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdSlvTag * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdSlvTag * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdSlvTag * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdSlvTag * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdSlvTag * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdSlvTag * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IScdSlvTagVtbl;

    interface IScdSlvTag
    {
        CONST_VTBL struct IScdSlvTagVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdSlvTag_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdSlvTag_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdSlvTag_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdSlvTag_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdSlvTag_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdSlvTag_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdSlvTag_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IScdSlvTag_INTERFACE_DEFINED__ */


#ifndef __IScdSlvTags_INTERFACE_DEFINED__
#define __IScdSlvTags_INTERFACE_DEFINED__

/* interface IScdSlvTags */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdSlvTags;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("51501760-320C-4230-8B48-908F3A7113B5")
    IScdSlvTags : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Tag( 
            /* [in] */ VARIANT WhichOne,
            /* [retval][out] */ IScdSlvTag **pItem) = 0;
        
        virtual /* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ LPUNKNOWN *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ BSTR Tag) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Remove( 
            /* [in] */ VARIANT WhichOne) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ReturnCode( 
            /* [retval][out] */ eScdSlvTagsCodes *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_TagValue( 
            /* [in] */ BSTR TheTag,
            /* [retval][out] */ VARIANT *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_TagValue( 
            /* [in] */ BSTR TheTag,
            /* [in] */ VARIANT newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_TagType( 
            /* [in] */ BSTR TheTag,
            /* [retval][out] */ eScdSlvTagType *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScdSlvTagsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdSlvTags * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdSlvTags * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdSlvTags * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdSlvTags * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdSlvTags * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdSlvTags * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdSlvTags * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Tag )( 
            IScdSlvTags * This,
            /* [in] */ VARIANT WhichOne,
            /* [retval][out] */ IScdSlvTag **pItem);
        
        /* [id][restricted][propget] */ HRESULT ( STDMETHODCALLTYPE *get__NewEnum )( 
            IScdSlvTags * This,
            /* [retval][out] */ LPUNKNOWN *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            IScdSlvTags * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Add )( 
            IScdSlvTags * This,
            /* [in] */ BSTR Tag);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Remove )( 
            IScdSlvTags * This,
            /* [in] */ VARIANT WhichOne);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ReturnCode )( 
            IScdSlvTags * This,
            /* [retval][out] */ eScdSlvTagsCodes *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_TagValue )( 
            IScdSlvTags * This,
            /* [in] */ BSTR TheTag,
            /* [retval][out] */ VARIANT *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_TagValue )( 
            IScdSlvTags * This,
            /* [in] */ BSTR TheTag,
            /* [in] */ VARIANT newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_TagType )( 
            IScdSlvTags * This,
            /* [in] */ BSTR TheTag,
            /* [retval][out] */ eScdSlvTagType *pVal);
        
        END_INTERFACE
    } IScdSlvTagsVtbl;

    interface IScdSlvTags
    {
        CONST_VTBL struct IScdSlvTagsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdSlvTags_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdSlvTags_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdSlvTags_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdSlvTags_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdSlvTags_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdSlvTags_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdSlvTags_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IScdSlvTags_Tag(This,WhichOne,pItem)	\
    (This)->lpVtbl -> Tag(This,WhichOne,pItem)

#define IScdSlvTags_get__NewEnum(This,pVal)	\
    (This)->lpVtbl -> get__NewEnum(This,pVal)

#define IScdSlvTags_get_Count(This,pVal)	\
    (This)->lpVtbl -> get_Count(This,pVal)

#define IScdSlvTags_Add(This,Tag)	\
    (This)->lpVtbl -> Add(This,Tag)

#define IScdSlvTags_Remove(This,WhichOne)	\
    (This)->lpVtbl -> Remove(This,WhichOne)

#define IScdSlvTags_get_ReturnCode(This,pVal)	\
    (This)->lpVtbl -> get_ReturnCode(This,pVal)

#define IScdSlvTags_get_TagValue(This,TheTag,pVal)	\
    (This)->lpVtbl -> get_TagValue(This,TheTag,pVal)

#define IScdSlvTags_put_TagValue(This,TheTag,newVal)	\
    (This)->lpVtbl -> put_TagValue(This,TheTag,newVal)

#define IScdSlvTags_get_TagType(This,TheTag,pVal)	\
    (This)->lpVtbl -> get_TagType(This,TheTag,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdSlvTags_Tag_Proxy( 
    IScdSlvTags * This,
    /* [in] */ VARIANT WhichOne,
    /* [retval][out] */ IScdSlvTag **pItem);


void __RPC_STUB IScdSlvTags_Tag_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE IScdSlvTags_get__NewEnum_Proxy( 
    IScdSlvTags * This,
    /* [retval][out] */ LPUNKNOWN *pVal);


void __RPC_STUB IScdSlvTags_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSlvTags_get_Count_Proxy( 
    IScdSlvTags * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IScdSlvTags_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdSlvTags_Add_Proxy( 
    IScdSlvTags * This,
    /* [in] */ BSTR Tag);


void __RPC_STUB IScdSlvTags_Add_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdSlvTags_Remove_Proxy( 
    IScdSlvTags * This,
    /* [in] */ VARIANT WhichOne);


void __RPC_STUB IScdSlvTags_Remove_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSlvTags_get_ReturnCode_Proxy( 
    IScdSlvTags * This,
    /* [retval][out] */ eScdSlvTagsCodes *pVal);


void __RPC_STUB IScdSlvTags_get_ReturnCode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSlvTags_get_TagValue_Proxy( 
    IScdSlvTags * This,
    /* [in] */ BSTR TheTag,
    /* [retval][out] */ VARIANT *pVal);


void __RPC_STUB IScdSlvTags_get_TagValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IScdSlvTags_put_TagValue_Proxy( 
    IScdSlvTags * This,
    /* [in] */ BSTR TheTag,
    /* [in] */ VARIANT newVal);


void __RPC_STUB IScdSlvTags_put_TagValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdSlvTags_get_TagType_Proxy( 
    IScdSlvTags * This,
    /* [in] */ BSTR TheTag,
    /* [retval][out] */ eScdSlvTagType *pVal);


void __RPC_STUB IScdSlvTags_get_TagType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScdSlvTags_INTERFACE_DEFINED__ */


#ifndef __IScdTest_INTERFACE_DEFINED__
#define __IScdTest_INTERFACE_DEFINED__

/* interface IScdTest */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdTest;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("19E9AC58-F99E-4296-BF44-E3E994EA0934")
    IScdTest : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReturnHResult( 
            /* [in] */ long Sev,
            /* [in] */ long Code) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ThrowException( 
            /* [in] */ long Sev,
            /* [in] */ long Code) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScdTestVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdTest * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdTest * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdTest * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdTest * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdTest * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdTest * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdTest * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ReturnHResult )( 
            IScdTest * This,
            /* [in] */ long Sev,
            /* [in] */ long Code);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ThrowException )( 
            IScdTest * This,
            /* [in] */ long Sev,
            /* [in] */ long Code);
        
        END_INTERFACE
    } IScdTestVtbl;

    interface IScdTest
    {
        CONST_VTBL struct IScdTestVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdTest_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdTest_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdTest_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdTest_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdTest_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdTest_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdTest_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IScdTest_ReturnHResult(This,Sev,Code)	\
    (This)->lpVtbl -> ReturnHResult(This,Sev,Code)

#define IScdTest_ThrowException(This,Sev,Code)	\
    (This)->lpVtbl -> ThrowException(This,Sev,Code)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdTest_ReturnHResult_Proxy( 
    IScdTest * This,
    /* [in] */ long Sev,
    /* [in] */ long Code);


void __RPC_STUB IScdTest_ReturnHResult_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdTest_ThrowException_Proxy( 
    IScdTest * This,
    /* [in] */ long Sev,
    /* [in] */ long Code);


void __RPC_STUB IScdTest_ThrowException_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScdTest_INTERFACE_DEFINED__ */


#ifndef __IScdWire_INTERFACE_DEFINED__
#define __IScdWire_INTERFACE_DEFINED__

/* interface IScdWire */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdWire;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("37C130B3-29B7-491A-A8A1-E8AA54542C1C")
    IScdWire : public IDispatch
    {
    public:
        virtual /* [helpstring][restricted][id] */ HRESULT STDMETHODCALLTYPE SetObjectInfo( 
            /* [in] */ DWORD Node,
            /* [in] */ DWORD Wire) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Tag( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Node( 
            /* [retval][out] */ IScdNode **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SrcNode( 
            /* [retval][out] */ IScdNode **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_DstNode( 
            /* [retval][out] */ IScdNode **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SrcTermStrip( 
            /* [retval][out] */ IScdTermStrip **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_DstTermStrip( 
            /* [retval][out] */ IScdTermStrip **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SrcTerminal( 
            /* [retval][out] */ IScdTerminal **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_DstTerminal( 
            /* [retval][out] */ IScdTerminal **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Area( 
            /* [retval][out] */ DOUBLE *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Area( 
            /* [in] */ DOUBLE newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Length( 
            /* [retval][out] */ DOUBLE *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Length( 
            /* [in] */ DOUBLE newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Component( 
            /* [retval][out] */ IScdComponent **pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScdWireVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdWire * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdWire * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdWire * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdWire * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdWire * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdWire * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdWire * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][restricted][id] */ HRESULT ( STDMETHODCALLTYPE *SetObjectInfo )( 
            IScdWire * This,
            /* [in] */ DWORD Node,
            /* [in] */ DWORD Wire);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Tag )( 
            IScdWire * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Node )( 
            IScdWire * This,
            /* [retval][out] */ IScdNode **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SrcNode )( 
            IScdWire * This,
            /* [retval][out] */ IScdNode **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DstNode )( 
            IScdWire * This,
            /* [retval][out] */ IScdNode **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SrcTermStrip )( 
            IScdWire * This,
            /* [retval][out] */ IScdTermStrip **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DstTermStrip )( 
            IScdWire * This,
            /* [retval][out] */ IScdTermStrip **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SrcTerminal )( 
            IScdWire * This,
            /* [retval][out] */ IScdTerminal **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DstTerminal )( 
            IScdWire * This,
            /* [retval][out] */ IScdTerminal **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Area )( 
            IScdWire * This,
            /* [retval][out] */ DOUBLE *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Area )( 
            IScdWire * This,
            /* [in] */ DOUBLE newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Length )( 
            IScdWire * This,
            /* [retval][out] */ DOUBLE *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Length )( 
            IScdWire * This,
            /* [in] */ DOUBLE newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Component )( 
            IScdWire * This,
            /* [retval][out] */ IScdComponent **pVal);
        
        END_INTERFACE
    } IScdWireVtbl;

    interface IScdWire
    {
        CONST_VTBL struct IScdWireVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdWire_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdWire_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdWire_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdWire_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdWire_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdWire_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdWire_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IScdWire_SetObjectInfo(This,Node,Wire)	\
    (This)->lpVtbl -> SetObjectInfo(This,Node,Wire)

#define IScdWire_get_Tag(This,pVal)	\
    (This)->lpVtbl -> get_Tag(This,pVal)

#define IScdWire_get_Node(This,pVal)	\
    (This)->lpVtbl -> get_Node(This,pVal)

#define IScdWire_get_SrcNode(This,pVal)	\
    (This)->lpVtbl -> get_SrcNode(This,pVal)

#define IScdWire_get_DstNode(This,pVal)	\
    (This)->lpVtbl -> get_DstNode(This,pVal)

#define IScdWire_get_SrcTermStrip(This,pVal)	\
    (This)->lpVtbl -> get_SrcTermStrip(This,pVal)

#define IScdWire_get_DstTermStrip(This,pVal)	\
    (This)->lpVtbl -> get_DstTermStrip(This,pVal)

#define IScdWire_get_SrcTerminal(This,pVal)	\
    (This)->lpVtbl -> get_SrcTerminal(This,pVal)

#define IScdWire_get_DstTerminal(This,pVal)	\
    (This)->lpVtbl -> get_DstTerminal(This,pVal)

#define IScdWire_get_Area(This,pVal)	\
    (This)->lpVtbl -> get_Area(This,pVal)

#define IScdWire_put_Area(This,newVal)	\
    (This)->lpVtbl -> put_Area(This,newVal)

#define IScdWire_get_Length(This,pVal)	\
    (This)->lpVtbl -> get_Length(This,pVal)

#define IScdWire_put_Length(This,newVal)	\
    (This)->lpVtbl -> put_Length(This,newVal)

#define IScdWire_get_Component(This,pVal)	\
    (This)->lpVtbl -> get_Component(This,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][restricted][id] */ HRESULT STDMETHODCALLTYPE IScdWire_SetObjectInfo_Proxy( 
    IScdWire * This,
    /* [in] */ DWORD Node,
    /* [in] */ DWORD Wire);


void __RPC_STUB IScdWire_SetObjectInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdWire_get_Tag_Proxy( 
    IScdWire * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IScdWire_get_Tag_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdWire_get_Node_Proxy( 
    IScdWire * This,
    /* [retval][out] */ IScdNode **pVal);


void __RPC_STUB IScdWire_get_Node_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdWire_get_SrcNode_Proxy( 
    IScdWire * This,
    /* [retval][out] */ IScdNode **pVal);


void __RPC_STUB IScdWire_get_SrcNode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdWire_get_DstNode_Proxy( 
    IScdWire * This,
    /* [retval][out] */ IScdNode **pVal);


void __RPC_STUB IScdWire_get_DstNode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdWire_get_SrcTermStrip_Proxy( 
    IScdWire * This,
    /* [retval][out] */ IScdTermStrip **pVal);


void __RPC_STUB IScdWire_get_SrcTermStrip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdWire_get_DstTermStrip_Proxy( 
    IScdWire * This,
    /* [retval][out] */ IScdTermStrip **pVal);


void __RPC_STUB IScdWire_get_DstTermStrip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdWire_get_SrcTerminal_Proxy( 
    IScdWire * This,
    /* [retval][out] */ IScdTerminal **pVal);


void __RPC_STUB IScdWire_get_SrcTerminal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdWire_get_DstTerminal_Proxy( 
    IScdWire * This,
    /* [retval][out] */ IScdTerminal **pVal);


void __RPC_STUB IScdWire_get_DstTerminal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdWire_get_Area_Proxy( 
    IScdWire * This,
    /* [retval][out] */ DOUBLE *pVal);


void __RPC_STUB IScdWire_get_Area_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IScdWire_put_Area_Proxy( 
    IScdWire * This,
    /* [in] */ DOUBLE newVal);


void __RPC_STUB IScdWire_put_Area_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdWire_get_Length_Proxy( 
    IScdWire * This,
    /* [retval][out] */ DOUBLE *pVal);


void __RPC_STUB IScdWire_get_Length_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IScdWire_put_Length_Proxy( 
    IScdWire * This,
    /* [in] */ DOUBLE newVal);


void __RPC_STUB IScdWire_put_Length_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdWire_get_Component_Proxy( 
    IScdWire * This,
    /* [retval][out] */ IScdComponent **pVal);


void __RPC_STUB IScdWire_get_Component_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScdWire_INTERFACE_DEFINED__ */


#ifndef __IScdWires_INTERFACE_DEFINED__
#define __IScdWires_INTERFACE_DEFINED__

/* interface IScdWires */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdWires;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("386A7C55-991B-4D50-B164-5BAD1C980D7D")
    IScdWires : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Wire( 
            /* [in] */ VARIANT WhichOne,
            /* [retval][out] */ IScdWire **pItem) = 0;
        
        virtual /* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ LPUNKNOWN *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ReturnCode( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][restricted][id] */ HRESULT STDMETHODCALLTYPE SetObjectInfo( 
            /* [in] */ DWORD Node,
            /* [in] */ DWORD Types,
            /* [in] */ DWORD Direction) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Select( 
            /* [defaultvalue][in] */ eScdElecTypes Types = eScdElecType_All,
            /* [defaultvalue][in] */ eScdDirections Direction = eScdDirection_InOut) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetWire( 
            /* [in] */ LONG No,
            /* [in] */ BSTR Tag,
            /* [in] */ BSTR SrcTermStrip,
            /* [in] */ BSTR SrcTerminal,
            /* [in] */ BSTR DstTermStrip,
            /* [in] */ BSTR DstTerminal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AutoWire( 
            /* [in] */ BSTR SrcTermStrip,
            /* [in] */ BSTR DstTermStrip) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScdWiresVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdWires * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdWires * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdWires * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdWires * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdWires * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdWires * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdWires * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Wire )( 
            IScdWires * This,
            /* [in] */ VARIANT WhichOne,
            /* [retval][out] */ IScdWire **pItem);
        
        /* [id][restricted][propget] */ HRESULT ( STDMETHODCALLTYPE *get__NewEnum )( 
            IScdWires * This,
            /* [retval][out] */ LPUNKNOWN *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            IScdWires * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ReturnCode )( 
            IScdWires * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][restricted][id] */ HRESULT ( STDMETHODCALLTYPE *SetObjectInfo )( 
            IScdWires * This,
            /* [in] */ DWORD Node,
            /* [in] */ DWORD Types,
            /* [in] */ DWORD Direction);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Select )( 
            IScdWires * This,
            /* [defaultvalue][in] */ eScdElecTypes Types,
            /* [defaultvalue][in] */ eScdDirections Direction);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetWire )( 
            IScdWires * This,
            /* [in] */ LONG No,
            /* [in] */ BSTR Tag,
            /* [in] */ BSTR SrcTermStrip,
            /* [in] */ BSTR SrcTerminal,
            /* [in] */ BSTR DstTermStrip,
            /* [in] */ BSTR DstTerminal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AutoWire )( 
            IScdWires * This,
            /* [in] */ BSTR SrcTermStrip,
            /* [in] */ BSTR DstTermStrip);
        
        END_INTERFACE
    } IScdWiresVtbl;

    interface IScdWires
    {
        CONST_VTBL struct IScdWiresVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdWires_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdWires_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdWires_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdWires_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdWires_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdWires_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdWires_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IScdWires_Wire(This,WhichOne,pItem)	\
    (This)->lpVtbl -> Wire(This,WhichOne,pItem)

#define IScdWires_get__NewEnum(This,pVal)	\
    (This)->lpVtbl -> get__NewEnum(This,pVal)

#define IScdWires_get_Count(This,pVal)	\
    (This)->lpVtbl -> get_Count(This,pVal)

#define IScdWires_get_ReturnCode(This,pVal)	\
    (This)->lpVtbl -> get_ReturnCode(This,pVal)

#define IScdWires_SetObjectInfo(This,Node,Types,Direction)	\
    (This)->lpVtbl -> SetObjectInfo(This,Node,Types,Direction)

#define IScdWires_Select(This,Types,Direction)	\
    (This)->lpVtbl -> Select(This,Types,Direction)

#define IScdWires_SetWire(This,No,Tag,SrcTermStrip,SrcTerminal,DstTermStrip,DstTerminal)	\
    (This)->lpVtbl -> SetWire(This,No,Tag,SrcTermStrip,SrcTerminal,DstTermStrip,DstTerminal)

#define IScdWires_AutoWire(This,SrcTermStrip,DstTermStrip)	\
    (This)->lpVtbl -> AutoWire(This,SrcTermStrip,DstTermStrip)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdWires_Wire_Proxy( 
    IScdWires * This,
    /* [in] */ VARIANT WhichOne,
    /* [retval][out] */ IScdWire **pItem);


void __RPC_STUB IScdWires_Wire_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE IScdWires_get__NewEnum_Proxy( 
    IScdWires * This,
    /* [retval][out] */ LPUNKNOWN *pVal);


void __RPC_STUB IScdWires_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdWires_get_Count_Proxy( 
    IScdWires * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IScdWires_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdWires_get_ReturnCode_Proxy( 
    IScdWires * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IScdWires_get_ReturnCode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][restricted][id] */ HRESULT STDMETHODCALLTYPE IScdWires_SetObjectInfo_Proxy( 
    IScdWires * This,
    /* [in] */ DWORD Node,
    /* [in] */ DWORD Types,
    /* [in] */ DWORD Direction);


void __RPC_STUB IScdWires_SetObjectInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdWires_Select_Proxy( 
    IScdWires * This,
    /* [defaultvalue][in] */ eScdElecTypes Types,
    /* [defaultvalue][in] */ eScdDirections Direction);


void __RPC_STUB IScdWires_Select_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdWires_SetWire_Proxy( 
    IScdWires * This,
    /* [in] */ LONG No,
    /* [in] */ BSTR Tag,
    /* [in] */ BSTR SrcTermStrip,
    /* [in] */ BSTR SrcTerminal,
    /* [in] */ BSTR DstTermStrip,
    /* [in] */ BSTR DstTerminal);


void __RPC_STUB IScdWires_SetWire_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdWires_AutoWire_Proxy( 
    IScdWires * This,
    /* [in] */ BSTR SrcTermStrip,
    /* [in] */ BSTR DstTermStrip);


void __RPC_STUB IScdWires_AutoWire_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScdWires_INTERFACE_DEFINED__ */


#ifndef __IScdTerminal_INTERFACE_DEFINED__
#define __IScdTerminal_INTERFACE_DEFINED__

/* interface IScdTerminal */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdTerminal;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("8423BE53-50DF-4FEA-B3D8-1FB5252A2CBA")
    IScdTerminal : public IDispatch
    {
    public:
        virtual /* [helpstring][restricted][id] */ HRESULT STDMETHODCALLTYPE SetObjectInfo( 
            /* [in] */ DWORD Term) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Tag( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Node( 
            /* [retval][out] */ IScdNode **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_TermStrip( 
            /* [retval][out] */ IScdTermStrip **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Voltage( 
            /* [retval][out] */ IScdComplex **pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScdTerminalVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdTerminal * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdTerminal * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdTerminal * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdTerminal * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdTerminal * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdTerminal * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdTerminal * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][restricted][id] */ HRESULT ( STDMETHODCALLTYPE *SetObjectInfo )( 
            IScdTerminal * This,
            /* [in] */ DWORD Term);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Tag )( 
            IScdTerminal * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Node )( 
            IScdTerminal * This,
            /* [retval][out] */ IScdNode **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_TermStrip )( 
            IScdTerminal * This,
            /* [retval][out] */ IScdTermStrip **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Voltage )( 
            IScdTerminal * This,
            /* [retval][out] */ IScdComplex **pVal);
        
        END_INTERFACE
    } IScdTerminalVtbl;

    interface IScdTerminal
    {
        CONST_VTBL struct IScdTerminalVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdTerminal_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdTerminal_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdTerminal_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdTerminal_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdTerminal_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdTerminal_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdTerminal_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IScdTerminal_SetObjectInfo(This,Term)	\
    (This)->lpVtbl -> SetObjectInfo(This,Term)

#define IScdTerminal_get_Tag(This,pVal)	\
    (This)->lpVtbl -> get_Tag(This,pVal)

#define IScdTerminal_get_Node(This,pVal)	\
    (This)->lpVtbl -> get_Node(This,pVal)

#define IScdTerminal_get_TermStrip(This,pVal)	\
    (This)->lpVtbl -> get_TermStrip(This,pVal)

#define IScdTerminal_get_Voltage(This,pVal)	\
    (This)->lpVtbl -> get_Voltage(This,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][restricted][id] */ HRESULT STDMETHODCALLTYPE IScdTerminal_SetObjectInfo_Proxy( 
    IScdTerminal * This,
    /* [in] */ DWORD Term);


void __RPC_STUB IScdTerminal_SetObjectInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdTerminal_get_Tag_Proxy( 
    IScdTerminal * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IScdTerminal_get_Tag_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdTerminal_get_Node_Proxy( 
    IScdTerminal * This,
    /* [retval][out] */ IScdNode **pVal);


void __RPC_STUB IScdTerminal_get_Node_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdTerminal_get_TermStrip_Proxy( 
    IScdTerminal * This,
    /* [retval][out] */ IScdTermStrip **pVal);


void __RPC_STUB IScdTerminal_get_TermStrip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdTerminal_get_Voltage_Proxy( 
    IScdTerminal * This,
    /* [retval][out] */ IScdComplex **pVal);


void __RPC_STUB IScdTerminal_get_Voltage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScdTerminal_INTERFACE_DEFINED__ */


#ifndef __IScdTerminals_INTERFACE_DEFINED__
#define __IScdTerminals_INTERFACE_DEFINED__

/* interface IScdTerminals */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdTerminals;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("57EFCD3E-7581-47A3-8FA8-F08FCE6AA56B")
    IScdTerminals : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Terminal( 
            /* [in] */ VARIANT WhichOne,
            /* [retval][out] */ IScdTerminal **pItem) = 0;
        
        virtual /* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ LPUNKNOWN *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ReturnCode( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][restricted][id] */ HRESULT STDMETHODCALLTYPE SetObjectInfo( 
            /* [in] */ DWORD Node,
            /* [in] */ DWORD Types,
            /* [in] */ DWORD Direction) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Select( 
            /* [defaultvalue][in] */ eScdElecTypes Types = eScdElecType_All,
            /* [defaultvalue][in] */ eScdDirections Direction = eScdDirection_InOut) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScdTerminalsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdTerminals * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdTerminals * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdTerminals * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdTerminals * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdTerminals * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdTerminals * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdTerminals * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Terminal )( 
            IScdTerminals * This,
            /* [in] */ VARIANT WhichOne,
            /* [retval][out] */ IScdTerminal **pItem);
        
        /* [id][restricted][propget] */ HRESULT ( STDMETHODCALLTYPE *get__NewEnum )( 
            IScdTerminals * This,
            /* [retval][out] */ LPUNKNOWN *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            IScdTerminals * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ReturnCode )( 
            IScdTerminals * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][restricted][id] */ HRESULT ( STDMETHODCALLTYPE *SetObjectInfo )( 
            IScdTerminals * This,
            /* [in] */ DWORD Node,
            /* [in] */ DWORD Types,
            /* [in] */ DWORD Direction);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Select )( 
            IScdTerminals * This,
            /* [defaultvalue][in] */ eScdElecTypes Types,
            /* [defaultvalue][in] */ eScdDirections Direction);
        
        END_INTERFACE
    } IScdTerminalsVtbl;

    interface IScdTerminals
    {
        CONST_VTBL struct IScdTerminalsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdTerminals_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdTerminals_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdTerminals_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdTerminals_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdTerminals_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdTerminals_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdTerminals_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IScdTerminals_Terminal(This,WhichOne,pItem)	\
    (This)->lpVtbl -> Terminal(This,WhichOne,pItem)

#define IScdTerminals_get__NewEnum(This,pVal)	\
    (This)->lpVtbl -> get__NewEnum(This,pVal)

#define IScdTerminals_get_Count(This,pVal)	\
    (This)->lpVtbl -> get_Count(This,pVal)

#define IScdTerminals_get_ReturnCode(This,pVal)	\
    (This)->lpVtbl -> get_ReturnCode(This,pVal)

#define IScdTerminals_SetObjectInfo(This,Node,Types,Direction)	\
    (This)->lpVtbl -> SetObjectInfo(This,Node,Types,Direction)

#define IScdTerminals_Select(This,Types,Direction)	\
    (This)->lpVtbl -> Select(This,Types,Direction)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdTerminals_Terminal_Proxy( 
    IScdTerminals * This,
    /* [in] */ VARIANT WhichOne,
    /* [retval][out] */ IScdTerminal **pItem);


void __RPC_STUB IScdTerminals_Terminal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE IScdTerminals_get__NewEnum_Proxy( 
    IScdTerminals * This,
    /* [retval][out] */ LPUNKNOWN *pVal);


void __RPC_STUB IScdTerminals_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdTerminals_get_Count_Proxy( 
    IScdTerminals * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IScdTerminals_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdTerminals_get_ReturnCode_Proxy( 
    IScdTerminals * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IScdTerminals_get_ReturnCode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][restricted][id] */ HRESULT STDMETHODCALLTYPE IScdTerminals_SetObjectInfo_Proxy( 
    IScdTerminals * This,
    /* [in] */ DWORD Node,
    /* [in] */ DWORD Types,
    /* [in] */ DWORD Direction);


void __RPC_STUB IScdTerminals_SetObjectInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdTerminals_Select_Proxy( 
    IScdTerminals * This,
    /* [defaultvalue][in] */ eScdElecTypes Types,
    /* [defaultvalue][in] */ eScdDirections Direction);


void __RPC_STUB IScdTerminals_Select_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScdTerminals_INTERFACE_DEFINED__ */


#ifndef __IScdTermStrip_INTERFACE_DEFINED__
#define __IScdTermStrip_INTERFACE_DEFINED__

/* interface IScdTermStrip */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdTermStrip;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("68A08D45-05DF-4463-B2EF-2DC28328E97F")
    IScdTermStrip : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Terminal( 
            /* [in] */ VARIANT WhichOne,
            /* [retval][out] */ IScdTerminal **pItem) = 0;
        
        virtual /* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ LPUNKNOWN *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ReturnCode( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][restricted][id] */ HRESULT STDMETHODCALLTYPE SetObjectInfo( 
            /* [in] */ DWORD TermStrip,
            /* [in] */ DWORD Types,
            /* [in] */ DWORD Direction) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Select( 
            /* [defaultvalue][in] */ eScdElecTypes Types = eScdElecType_All,
            /* [defaultvalue][in] */ eScdDirections Direction = eScdDirection_InOut) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Tag( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Node( 
            /* [retval][out] */ IScdNode **pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScdTermStripVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdTermStrip * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdTermStrip * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdTermStrip * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdTermStrip * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdTermStrip * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdTermStrip * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdTermStrip * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Terminal )( 
            IScdTermStrip * This,
            /* [in] */ VARIANT WhichOne,
            /* [retval][out] */ IScdTerminal **pItem);
        
        /* [id][restricted][propget] */ HRESULT ( STDMETHODCALLTYPE *get__NewEnum )( 
            IScdTermStrip * This,
            /* [retval][out] */ LPUNKNOWN *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            IScdTermStrip * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ReturnCode )( 
            IScdTermStrip * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][restricted][id] */ HRESULT ( STDMETHODCALLTYPE *SetObjectInfo )( 
            IScdTermStrip * This,
            /* [in] */ DWORD TermStrip,
            /* [in] */ DWORD Types,
            /* [in] */ DWORD Direction);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Select )( 
            IScdTermStrip * This,
            /* [defaultvalue][in] */ eScdElecTypes Types,
            /* [defaultvalue][in] */ eScdDirections Direction);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Tag )( 
            IScdTermStrip * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Node )( 
            IScdTermStrip * This,
            /* [retval][out] */ IScdNode **pVal);
        
        END_INTERFACE
    } IScdTermStripVtbl;

    interface IScdTermStrip
    {
        CONST_VTBL struct IScdTermStripVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdTermStrip_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdTermStrip_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdTermStrip_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdTermStrip_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdTermStrip_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdTermStrip_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdTermStrip_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IScdTermStrip_Terminal(This,WhichOne,pItem)	\
    (This)->lpVtbl -> Terminal(This,WhichOne,pItem)

#define IScdTermStrip_get__NewEnum(This,pVal)	\
    (This)->lpVtbl -> get__NewEnum(This,pVal)

#define IScdTermStrip_get_Count(This,pVal)	\
    (This)->lpVtbl -> get_Count(This,pVal)

#define IScdTermStrip_get_ReturnCode(This,pVal)	\
    (This)->lpVtbl -> get_ReturnCode(This,pVal)

#define IScdTermStrip_SetObjectInfo(This,TermStrip,Types,Direction)	\
    (This)->lpVtbl -> SetObjectInfo(This,TermStrip,Types,Direction)

#define IScdTermStrip_Select(This,Types,Direction)	\
    (This)->lpVtbl -> Select(This,Types,Direction)

#define IScdTermStrip_get_Tag(This,pVal)	\
    (This)->lpVtbl -> get_Tag(This,pVal)

#define IScdTermStrip_get_Node(This,pVal)	\
    (This)->lpVtbl -> get_Node(This,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdTermStrip_Terminal_Proxy( 
    IScdTermStrip * This,
    /* [in] */ VARIANT WhichOne,
    /* [retval][out] */ IScdTerminal **pItem);


void __RPC_STUB IScdTermStrip_Terminal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE IScdTermStrip_get__NewEnum_Proxy( 
    IScdTermStrip * This,
    /* [retval][out] */ LPUNKNOWN *pVal);


void __RPC_STUB IScdTermStrip_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdTermStrip_get_Count_Proxy( 
    IScdTermStrip * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IScdTermStrip_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdTermStrip_get_ReturnCode_Proxy( 
    IScdTermStrip * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IScdTermStrip_get_ReturnCode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][restricted][id] */ HRESULT STDMETHODCALLTYPE IScdTermStrip_SetObjectInfo_Proxy( 
    IScdTermStrip * This,
    /* [in] */ DWORD TermStrip,
    /* [in] */ DWORD Types,
    /* [in] */ DWORD Direction);


void __RPC_STUB IScdTermStrip_SetObjectInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdTermStrip_Select_Proxy( 
    IScdTermStrip * This,
    /* [defaultvalue][in] */ eScdElecTypes Types,
    /* [defaultvalue][in] */ eScdDirections Direction);


void __RPC_STUB IScdTermStrip_Select_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdTermStrip_get_Tag_Proxy( 
    IScdTermStrip * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IScdTermStrip_get_Tag_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdTermStrip_get_Node_Proxy( 
    IScdTermStrip * This,
    /* [retval][out] */ IScdNode **pVal);


void __RPC_STUB IScdTermStrip_get_Node_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScdTermStrip_INTERFACE_DEFINED__ */


#ifndef __IScdTermStrips_INTERFACE_DEFINED__
#define __IScdTermStrips_INTERFACE_DEFINED__

/* interface IScdTermStrips */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdTermStrips;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("231165C8-AF0E-41F7-93DD-8E42FFEB6365")
    IScdTermStrips : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE TermStrip( 
            /* [in] */ VARIANT WhichOne,
            /* [retval][out] */ IScdTermStrip **pItem) = 0;
        
        virtual /* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ LPUNKNOWN *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ReturnCode( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][restricted][id] */ HRESULT STDMETHODCALLTYPE SetObjectInfo( 
            /* [in] */ DWORD Node,
            /* [in] */ DWORD Types,
            /* [in] */ DWORD Direction) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Select( 
            /* [defaultvalue][in] */ eScdElecTypes Types = eScdElecType_All,
            /* [defaultvalue][in] */ eScdDirections Direction = eScdDirection_InOut) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScdTermStripsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdTermStrips * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdTermStrips * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdTermStrips * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdTermStrips * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdTermStrips * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdTermStrips * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdTermStrips * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *TermStrip )( 
            IScdTermStrips * This,
            /* [in] */ VARIANT WhichOne,
            /* [retval][out] */ IScdTermStrip **pItem);
        
        /* [id][restricted][propget] */ HRESULT ( STDMETHODCALLTYPE *get__NewEnum )( 
            IScdTermStrips * This,
            /* [retval][out] */ LPUNKNOWN *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            IScdTermStrips * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ReturnCode )( 
            IScdTermStrips * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][restricted][id] */ HRESULT ( STDMETHODCALLTYPE *SetObjectInfo )( 
            IScdTermStrips * This,
            /* [in] */ DWORD Node,
            /* [in] */ DWORD Types,
            /* [in] */ DWORD Direction);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Select )( 
            IScdTermStrips * This,
            /* [defaultvalue][in] */ eScdElecTypes Types,
            /* [defaultvalue][in] */ eScdDirections Direction);
        
        END_INTERFACE
    } IScdTermStripsVtbl;

    interface IScdTermStrips
    {
        CONST_VTBL struct IScdTermStripsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdTermStrips_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdTermStrips_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdTermStrips_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdTermStrips_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdTermStrips_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdTermStrips_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdTermStrips_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IScdTermStrips_TermStrip(This,WhichOne,pItem)	\
    (This)->lpVtbl -> TermStrip(This,WhichOne,pItem)

#define IScdTermStrips_get__NewEnum(This,pVal)	\
    (This)->lpVtbl -> get__NewEnum(This,pVal)

#define IScdTermStrips_get_Count(This,pVal)	\
    (This)->lpVtbl -> get_Count(This,pVal)

#define IScdTermStrips_get_ReturnCode(This,pVal)	\
    (This)->lpVtbl -> get_ReturnCode(This,pVal)

#define IScdTermStrips_SetObjectInfo(This,Node,Types,Direction)	\
    (This)->lpVtbl -> SetObjectInfo(This,Node,Types,Direction)

#define IScdTermStrips_Select(This,Types,Direction)	\
    (This)->lpVtbl -> Select(This,Types,Direction)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdTermStrips_TermStrip_Proxy( 
    IScdTermStrips * This,
    /* [in] */ VARIANT WhichOne,
    /* [retval][out] */ IScdTermStrip **pItem);


void __RPC_STUB IScdTermStrips_TermStrip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE IScdTermStrips_get__NewEnum_Proxy( 
    IScdTermStrips * This,
    /* [retval][out] */ LPUNKNOWN *pVal);


void __RPC_STUB IScdTermStrips_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdTermStrips_get_Count_Proxy( 
    IScdTermStrips * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IScdTermStrips_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdTermStrips_get_ReturnCode_Proxy( 
    IScdTermStrips * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IScdTermStrips_get_ReturnCode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][restricted][id] */ HRESULT STDMETHODCALLTYPE IScdTermStrips_SetObjectInfo_Proxy( 
    IScdTermStrips * This,
    /* [in] */ DWORD Node,
    /* [in] */ DWORD Types,
    /* [in] */ DWORD Direction);


void __RPC_STUB IScdTermStrips_SetObjectInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdTermStrips_Select_Proxy( 
    IScdTermStrips * This,
    /* [defaultvalue][in] */ eScdElecTypes Types,
    /* [defaultvalue][in] */ eScdDirections Direction);


void __RPC_STUB IScdTermStrips_Select_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScdTermStrips_INTERFACE_DEFINED__ */


#ifndef __IScdElectrics_INTERFACE_DEFINED__
#define __IScdElectrics_INTERFACE_DEFINED__

/* interface IScdElectrics */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdElectrics;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("8A7205B3-3838-45D2-A3D8-5A05F7D7C314")
    IScdElectrics : public IDispatch
    {
    public:
        virtual /* [helpstring][restricted][id] */ HRESULT STDMETHODCALLTYPE SetObjectInfo( 
            /* [in] */ DWORD newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_TermStrips( 
            /* [defaultvalue][in] */ eScdElecTypes Types,
            /* [defaultvalue][in] */ eScdDirections Direction,
            /* [retval][out] */ IScdTermStrips **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Terminals( 
            /* [defaultvalue][in] */ eScdElecTypes Types,
            /* [retval][out] */ IScdTerminals **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Wires( 
            /* [defaultvalue][in] */ eScdElecTypes Types,
            /* [retval][out] */ IScdWires **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Components( 
            /* [defaultvalue][in] */ eScdElecTypes Types,
            /* [retval][out] */ IScdComponents **pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScdElectricsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdElectrics * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdElectrics * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdElectrics * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdElectrics * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdElectrics * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdElectrics * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdElectrics * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][restricted][id] */ HRESULT ( STDMETHODCALLTYPE *SetObjectInfo )( 
            IScdElectrics * This,
            /* [in] */ DWORD newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_TermStrips )( 
            IScdElectrics * This,
            /* [defaultvalue][in] */ eScdElecTypes Types,
            /* [defaultvalue][in] */ eScdDirections Direction,
            /* [retval][out] */ IScdTermStrips **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Terminals )( 
            IScdElectrics * This,
            /* [defaultvalue][in] */ eScdElecTypes Types,
            /* [retval][out] */ IScdTerminals **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Wires )( 
            IScdElectrics * This,
            /* [defaultvalue][in] */ eScdElecTypes Types,
            /* [retval][out] */ IScdWires **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Components )( 
            IScdElectrics * This,
            /* [defaultvalue][in] */ eScdElecTypes Types,
            /* [retval][out] */ IScdComponents **pVal);
        
        END_INTERFACE
    } IScdElectricsVtbl;

    interface IScdElectrics
    {
        CONST_VTBL struct IScdElectricsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdElectrics_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdElectrics_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdElectrics_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdElectrics_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdElectrics_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdElectrics_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdElectrics_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IScdElectrics_SetObjectInfo(This,newVal)	\
    (This)->lpVtbl -> SetObjectInfo(This,newVal)

#define IScdElectrics_get_TermStrips(This,Types,Direction,pVal)	\
    (This)->lpVtbl -> get_TermStrips(This,Types,Direction,pVal)

#define IScdElectrics_get_Terminals(This,Types,pVal)	\
    (This)->lpVtbl -> get_Terminals(This,Types,pVal)

#define IScdElectrics_get_Wires(This,Types,pVal)	\
    (This)->lpVtbl -> get_Wires(This,Types,pVal)

#define IScdElectrics_get_Components(This,Types,pVal)	\
    (This)->lpVtbl -> get_Components(This,Types,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][restricted][id] */ HRESULT STDMETHODCALLTYPE IScdElectrics_SetObjectInfo_Proxy( 
    IScdElectrics * This,
    /* [in] */ DWORD newVal);


void __RPC_STUB IScdElectrics_SetObjectInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdElectrics_get_TermStrips_Proxy( 
    IScdElectrics * This,
    /* [defaultvalue][in] */ eScdElecTypes Types,
    /* [defaultvalue][in] */ eScdDirections Direction,
    /* [retval][out] */ IScdTermStrips **pVal);


void __RPC_STUB IScdElectrics_get_TermStrips_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdElectrics_get_Terminals_Proxy( 
    IScdElectrics * This,
    /* [defaultvalue][in] */ eScdElecTypes Types,
    /* [retval][out] */ IScdTerminals **pVal);


void __RPC_STUB IScdElectrics_get_Terminals_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdElectrics_get_Wires_Proxy( 
    IScdElectrics * This,
    /* [defaultvalue][in] */ eScdElecTypes Types,
    /* [retval][out] */ IScdWires **pVal);


void __RPC_STUB IScdElectrics_get_Wires_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdElectrics_get_Components_Proxy( 
    IScdElectrics * This,
    /* [defaultvalue][in] */ eScdElecTypes Types,
    /* [retval][out] */ IScdComponents **pVal);


void __RPC_STUB IScdElectrics_get_Components_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScdElectrics_INTERFACE_DEFINED__ */


#ifndef __IScdComponent_INTERFACE_DEFINED__
#define __IScdComponent_INTERFACE_DEFINED__

/* interface IScdComponent */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdComponent;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A081A069-91AA-4C5F-B524-80B440CD138C")
    IScdComponent : public IDispatch
    {
    public:
        virtual /* [helpstring][restricted][id] */ HRESULT STDMETHODCALLTYPE SetObjectInfo( 
            /* [in] */ DWORD Node,
            /* [in] */ DWORD Component) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SrcTerminal( 
            /* [retval][out] */ IScdTerminal **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_DstTerminal( 
            /* [retval][out] */ IScdTerminal **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Impedance( 
            /* [retval][out] */ IScdComplex **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Source( 
            /* [retval][out] */ IScdComplex **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Current( 
            /* [retval][out] */ IScdComplex **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Applied( 
            /* [retval][out] */ IScdComplex **pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScdComponentVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdComponent * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdComponent * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdComponent * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdComponent * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdComponent * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdComponent * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdComponent * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][restricted][id] */ HRESULT ( STDMETHODCALLTYPE *SetObjectInfo )( 
            IScdComponent * This,
            /* [in] */ DWORD Node,
            /* [in] */ DWORD Component);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SrcTerminal )( 
            IScdComponent * This,
            /* [retval][out] */ IScdTerminal **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DstTerminal )( 
            IScdComponent * This,
            /* [retval][out] */ IScdTerminal **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Impedance )( 
            IScdComponent * This,
            /* [retval][out] */ IScdComplex **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Source )( 
            IScdComponent * This,
            /* [retval][out] */ IScdComplex **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )( 
            IScdComponent * This,
            /* [retval][out] */ IScdComplex **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Applied )( 
            IScdComponent * This,
            /* [retval][out] */ IScdComplex **pVal);
        
        END_INTERFACE
    } IScdComponentVtbl;

    interface IScdComponent
    {
        CONST_VTBL struct IScdComponentVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdComponent_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdComponent_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdComponent_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdComponent_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdComponent_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdComponent_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdComponent_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IScdComponent_SetObjectInfo(This,Node,Component)	\
    (This)->lpVtbl -> SetObjectInfo(This,Node,Component)

#define IScdComponent_get_SrcTerminal(This,pVal)	\
    (This)->lpVtbl -> get_SrcTerminal(This,pVal)

#define IScdComponent_get_DstTerminal(This,pVal)	\
    (This)->lpVtbl -> get_DstTerminal(This,pVal)

#define IScdComponent_get_Impedance(This,pVal)	\
    (This)->lpVtbl -> get_Impedance(This,pVal)

#define IScdComponent_get_Source(This,pVal)	\
    (This)->lpVtbl -> get_Source(This,pVal)

#define IScdComponent_get_Current(This,pVal)	\
    (This)->lpVtbl -> get_Current(This,pVal)

#define IScdComponent_get_Applied(This,pVal)	\
    (This)->lpVtbl -> get_Applied(This,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][restricted][id] */ HRESULT STDMETHODCALLTYPE IScdComponent_SetObjectInfo_Proxy( 
    IScdComponent * This,
    /* [in] */ DWORD Node,
    /* [in] */ DWORD Component);


void __RPC_STUB IScdComponent_SetObjectInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdComponent_get_SrcTerminal_Proxy( 
    IScdComponent * This,
    /* [retval][out] */ IScdTerminal **pVal);


void __RPC_STUB IScdComponent_get_SrcTerminal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdComponent_get_DstTerminal_Proxy( 
    IScdComponent * This,
    /* [retval][out] */ IScdTerminal **pVal);


void __RPC_STUB IScdComponent_get_DstTerminal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdComponent_get_Impedance_Proxy( 
    IScdComponent * This,
    /* [retval][out] */ IScdComplex **pVal);


void __RPC_STUB IScdComponent_get_Impedance_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdComponent_get_Source_Proxy( 
    IScdComponent * This,
    /* [retval][out] */ IScdComplex **pVal);


void __RPC_STUB IScdComponent_get_Source_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdComponent_get_Current_Proxy( 
    IScdComponent * This,
    /* [retval][out] */ IScdComplex **pVal);


void __RPC_STUB IScdComponent_get_Current_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdComponent_get_Applied_Proxy( 
    IScdComponent * This,
    /* [retval][out] */ IScdComplex **pVal);


void __RPC_STUB IScdComponent_get_Applied_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScdComponent_INTERFACE_DEFINED__ */


#ifndef __IScdComponents_INTERFACE_DEFINED__
#define __IScdComponents_INTERFACE_DEFINED__

/* interface IScdComponents */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdComponents;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("37E46C83-1438-464B-B27C-2BECFBE4D1B5")
    IScdComponents : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Component( 
            /* [in] */ VARIANT WhichOne,
            /* [retval][out] */ IScdComponent **pItem) = 0;
        
        virtual /* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ LPUNKNOWN *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ReturnCode( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][restricted][id] */ HRESULT STDMETHODCALLTYPE SetObjectInfo( 
            /* [in] */ DWORD Node,
            /* [in] */ DWORD Types) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Select( 
            /* [defaultvalue][in] */ eScdElecTypes Types = eScdElecType_All) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScdComponentsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdComponents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdComponents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdComponents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdComponents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdComponents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdComponents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdComponents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Component )( 
            IScdComponents * This,
            /* [in] */ VARIANT WhichOne,
            /* [retval][out] */ IScdComponent **pItem);
        
        /* [id][restricted][propget] */ HRESULT ( STDMETHODCALLTYPE *get__NewEnum )( 
            IScdComponents * This,
            /* [retval][out] */ LPUNKNOWN *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            IScdComponents * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ReturnCode )( 
            IScdComponents * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][restricted][id] */ HRESULT ( STDMETHODCALLTYPE *SetObjectInfo )( 
            IScdComponents * This,
            /* [in] */ DWORD Node,
            /* [in] */ DWORD Types);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Select )( 
            IScdComponents * This,
            /* [defaultvalue][in] */ eScdElecTypes Types);
        
        END_INTERFACE
    } IScdComponentsVtbl;

    interface IScdComponents
    {
        CONST_VTBL struct IScdComponentsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdComponents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdComponents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdComponents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdComponents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdComponents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdComponents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdComponents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IScdComponents_Component(This,WhichOne,pItem)	\
    (This)->lpVtbl -> Component(This,WhichOne,pItem)

#define IScdComponents_get__NewEnum(This,pVal)	\
    (This)->lpVtbl -> get__NewEnum(This,pVal)

#define IScdComponents_get_Count(This,pVal)	\
    (This)->lpVtbl -> get_Count(This,pVal)

#define IScdComponents_get_ReturnCode(This,pVal)	\
    (This)->lpVtbl -> get_ReturnCode(This,pVal)

#define IScdComponents_SetObjectInfo(This,Node,Types)	\
    (This)->lpVtbl -> SetObjectInfo(This,Node,Types)

#define IScdComponents_Select(This,Types)	\
    (This)->lpVtbl -> Select(This,Types)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdComponents_Component_Proxy( 
    IScdComponents * This,
    /* [in] */ VARIANT WhichOne,
    /* [retval][out] */ IScdComponent **pItem);


void __RPC_STUB IScdComponents_Component_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE IScdComponents_get__NewEnum_Proxy( 
    IScdComponents * This,
    /* [retval][out] */ LPUNKNOWN *pVal);


void __RPC_STUB IScdComponents_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdComponents_get_Count_Proxy( 
    IScdComponents * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IScdComponents_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdComponents_get_ReturnCode_Proxy( 
    IScdComponents * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IScdComponents_get_ReturnCode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][restricted][id] */ HRESULT STDMETHODCALLTYPE IScdComponents_SetObjectInfo_Proxy( 
    IScdComponents * This,
    /* [in] */ DWORD Node,
    /* [in] */ DWORD Types);


void __RPC_STUB IScdComponents_SetObjectInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdComponents_Select_Proxy( 
    IScdComponents * This,
    /* [defaultvalue][in] */ eScdElecTypes Types);


void __RPC_STUB IScdComponents_Select_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScdComponents_INTERFACE_DEFINED__ */


#ifndef __IScdComplex_INTERFACE_DEFINED__
#define __IScdComplex_INTERFACE_DEFINED__

/* interface IScdComplex */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdComplex;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E66B691F-32DF-4B51-8D0B-6D5D18068472")
    IScdComplex : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Real( 
            /* [retval][out] */ DOUBLE *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Real( 
            /* [in] */ DOUBLE newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Imag( 
            /* [retval][out] */ DOUBLE *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Imag( 
            /* [in] */ DOUBLE newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Magnitude( 
            /* [retval][out] */ DOUBLE *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Magnitude( 
            /* [in] */ DOUBLE newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Phase( 
            /* [retval][out] */ DOUBLE *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Phase( 
            /* [in] */ DOUBLE newVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScdComplexVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdComplex * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdComplex * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdComplex * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdComplex * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdComplex * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdComplex * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdComplex * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Real )( 
            IScdComplex * This,
            /* [retval][out] */ DOUBLE *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Real )( 
            IScdComplex * This,
            /* [in] */ DOUBLE newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Imag )( 
            IScdComplex * This,
            /* [retval][out] */ DOUBLE *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Imag )( 
            IScdComplex * This,
            /* [in] */ DOUBLE newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Magnitude )( 
            IScdComplex * This,
            /* [retval][out] */ DOUBLE *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Magnitude )( 
            IScdComplex * This,
            /* [in] */ DOUBLE newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Phase )( 
            IScdComplex * This,
            /* [retval][out] */ DOUBLE *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Phase )( 
            IScdComplex * This,
            /* [in] */ DOUBLE newVal);
        
        END_INTERFACE
    } IScdComplexVtbl;

    interface IScdComplex
    {
        CONST_VTBL struct IScdComplexVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdComplex_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdComplex_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdComplex_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdComplex_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdComplex_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdComplex_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdComplex_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IScdComplex_get_Real(This,pVal)	\
    (This)->lpVtbl -> get_Real(This,pVal)

#define IScdComplex_put_Real(This,newVal)	\
    (This)->lpVtbl -> put_Real(This,newVal)

#define IScdComplex_get_Imag(This,pVal)	\
    (This)->lpVtbl -> get_Imag(This,pVal)

#define IScdComplex_put_Imag(This,newVal)	\
    (This)->lpVtbl -> put_Imag(This,newVal)

#define IScdComplex_get_Magnitude(This,pVal)	\
    (This)->lpVtbl -> get_Magnitude(This,pVal)

#define IScdComplex_put_Magnitude(This,newVal)	\
    (This)->lpVtbl -> put_Magnitude(This,newVal)

#define IScdComplex_get_Phase(This,pVal)	\
    (This)->lpVtbl -> get_Phase(This,pVal)

#define IScdComplex_put_Phase(This,newVal)	\
    (This)->lpVtbl -> put_Phase(This,newVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdComplex_get_Real_Proxy( 
    IScdComplex * This,
    /* [retval][out] */ DOUBLE *pVal);


void __RPC_STUB IScdComplex_get_Real_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IScdComplex_put_Real_Proxy( 
    IScdComplex * This,
    /* [in] */ DOUBLE newVal);


void __RPC_STUB IScdComplex_put_Real_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdComplex_get_Imag_Proxy( 
    IScdComplex * This,
    /* [retval][out] */ DOUBLE *pVal);


void __RPC_STUB IScdComplex_get_Imag_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IScdComplex_put_Imag_Proxy( 
    IScdComplex * This,
    /* [in] */ DOUBLE newVal);


void __RPC_STUB IScdComplex_put_Imag_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdComplex_get_Magnitude_Proxy( 
    IScdComplex * This,
    /* [retval][out] */ DOUBLE *pVal);


void __RPC_STUB IScdComplex_get_Magnitude_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IScdComplex_put_Magnitude_Proxy( 
    IScdComplex * This,
    /* [in] */ DOUBLE newVal);


void __RPC_STUB IScdComplex_put_Magnitude_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdComplex_get_Phase_Proxy( 
    IScdComplex * This,
    /* [retval][out] */ DOUBLE *pVal);


void __RPC_STUB IScdComplex_get_Phase_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IScdComplex_put_Phase_Proxy( 
    IScdComplex * This,
    /* [in] */ DOUBLE newVal);


void __RPC_STUB IScdComplex_put_Phase_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScdComplex_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_ScdMessage;

#ifdef __cplusplus

class DECLSPEC_UUID("95A4F4C8-D911-4CC5-A241-116BAEAC9892")
ScdMessage;
#endif

EXTERN_C const CLSID CLSID_ScdMessages;

#ifdef __cplusplus

class DECLSPEC_UUID("7D916AF7-E05D-4D62-A19A-078752183055")
ScdMessages;
#endif

EXTERN_C const CLSID CLSID_ScdProbal;

#ifdef __cplusplus

class DECLSPEC_UUID("CEBC2245-9DDC-4B20-8F79-83656AD1D066")
ScdProbal;
#endif

EXTERN_C const CLSID CLSID_ScdSolver;

#ifdef __cplusplus

class DECLSPEC_UUID("6761C370-442A-4bb3-AF4C-07BB7353F23B")
ScdSolver;
#endif

EXTERN_C const CLSID CLSID_ScdDebug;

#ifdef __cplusplus

class DECLSPEC_UUID("1591C55C-A339-42CE-BEAF-E286147842C1")
ScdDebug;
#endif

EXTERN_C const CLSID CLSID_ScdTest;

#ifdef __cplusplus

class DECLSPEC_UUID("3784FBD8-103D-4141-91C4-613785F70F1F")
ScdTest;
#endif

EXTERN_C const CLSID CLSID_ScdSlvTags;

#ifdef __cplusplus

class DECLSPEC_UUID("15366A57-3C50-498E-97E5-C731706C3861")
ScdSlvTags;
#endif

EXTERN_C const CLSID CLSID_ScdSlvTag;

#ifdef __cplusplus

class DECLSPEC_UUID("64AA6F46-2439-4C48-B03E-25EBE42128A3")
ScdSlvTag;
#endif

EXTERN_C const CLSID CLSID_ScdDynamic;

#ifdef __cplusplus

class DECLSPEC_UUID("0F708E57-EEED-47D5-BC95-BFB97D24CC15")
ScdDynamic;
#endif

EXTERN_C const CLSID CLSID_ScdNodes;

#ifdef __cplusplus

class DECLSPEC_UUID("C4FCD11A-5909-46D7-A7FC-70BDFFC45635")
ScdNodes;
#endif

EXTERN_C const CLSID CLSID_ScdNode;

#ifdef __cplusplus

class DECLSPEC_UUID("2C170CD9-3C2A-49D6-B5E9-3F6AA803D480")
ScdNode;
#endif

EXTERN_C const CLSID CLSID_ScdConnect;

#ifdef __cplusplus

class DECLSPEC_UUID("9BF2CD13-ADB0-4D4D-960A-73268FC7A58F")
ScdConnect;
#endif

EXTERN_C const CLSID CLSID_ScdConnectPoint;

#ifdef __cplusplus

class DECLSPEC_UUID("4DFDA6A2-C7A3-4087-A43C-CBCC65C72650")
ScdConnectPoint;
#endif

EXTERN_C const CLSID CLSID_ScdConnects;

#ifdef __cplusplus

class DECLSPEC_UUID("A6FDC0DE-3269-4B86-AC79-493E251818D2")
ScdConnects;
#endif

EXTERN_C const CLSID CLSID_ScdConnectPoints;

#ifdef __cplusplus

class DECLSPEC_UUID("E1CD13B4-F7D2-46CB-9E5F-D173D2C0BAF9")
ScdConnectPoints;
#endif

EXTERN_C const CLSID CLSID_ScdNodeClass;

#ifdef __cplusplus

class DECLSPEC_UUID("1ECC6CD5-189B-4A02-8AFE-75502855977E")
ScdNodeClass;
#endif

EXTERN_C const CLSID CLSID_ScdWire;

#ifdef __cplusplus

class DECLSPEC_UUID("5B9DCE12-D825-4CBC-BE39-C9A98F544877")
ScdWire;
#endif

EXTERN_C const CLSID CLSID_ScdWires;

#ifdef __cplusplus

class DECLSPEC_UUID("954689B2-7310-416C-B8CB-3B2D3F90F305")
ScdWires;
#endif

EXTERN_C const CLSID CLSID_ScdTerminal;

#ifdef __cplusplus

class DECLSPEC_UUID("7821B605-F73F-45F5-A65A-7A07F422FF66")
ScdTerminal;
#endif

EXTERN_C const CLSID CLSID_ScdTerminals;

#ifdef __cplusplus

class DECLSPEC_UUID("68E11902-B946-4EF3-8816-212320DAFC3F")
ScdTerminals;
#endif

EXTERN_C const CLSID CLSID_ScdTermStrip;

#ifdef __cplusplus

class DECLSPEC_UUID("BF38097F-D4C9-472D-B05A-354B3C9AF164")
ScdTermStrip;
#endif

EXTERN_C const CLSID CLSID_ScdTermStrips;

#ifdef __cplusplus

class DECLSPEC_UUID("9DD5CBD4-8C0F-4C42-82A9-5CB3C38D3958")
ScdTermStrips;
#endif

EXTERN_C const CLSID CLSID_ScdElectrics;

#ifdef __cplusplus

class DECLSPEC_UUID("98B8EE21-5BB7-421F-9E36-5B7DE01C07BB")
ScdElectrics;
#endif

EXTERN_C const CLSID CLSID_ScdComplex;

#ifdef __cplusplus

class DECLSPEC_UUID("C3AAC7AF-9E3E-427E-A6FC-84F055850271")
ScdComplex;
#endif

EXTERN_C const CLSID CLSID_ScdComponent;

#ifdef __cplusplus

class DECLSPEC_UUID("1448A094-B17A-45A6-97F3-F49171B18531")
ScdComponent;
#endif

EXTERN_C const CLSID CLSID_ScdComponents;

#ifdef __cplusplus

class DECLSPEC_UUID("75B7A003-8AE1-4E70-8D39-E35C37513D58")
ScdComponents;
#endif
#endif /* __ScdSlv_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


