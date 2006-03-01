

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0361 */
/* at Thu Dec 16 10:17:58 2004
 */
/* Compiler settings for .\PLC5.idl:
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

#ifndef __PLC5_i_h__
#define __PLC5_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __ISysCADCmd_FWD_DEFINED__
#define __ISysCADCmd_FWD_DEFINED__
typedef interface ISysCADCmd ISysCADCmd;
#endif 	/* __ISysCADCmd_FWD_DEFINED__ */


#ifndef __SysCADCmd_FWD_DEFINED__
#define __SysCADCmd_FWD_DEFINED__

#ifdef __cplusplus
typedef class SysCADCmd SysCADCmd;
#else
typedef struct SysCADCmd SysCADCmd;
#endif /* __cplusplus */

#endif 	/* __SysCADCmd_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __ISysCADCmd_INTERFACE_DEFINED__
#define __ISysCADCmd_INTERFACE_DEFINED__

/* interface ISysCADCmd */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_ISysCADCmd;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("13EF82EF-63CD-48A6-BAE3-042E79385BAA")
    ISysCADCmd : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct ISysCADCmdVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISysCADCmd * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISysCADCmd * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISysCADCmd * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISysCADCmd * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISysCADCmd * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISysCADCmd * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISysCADCmd * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } ISysCADCmdVtbl;

    interface ISysCADCmd
    {
        CONST_VTBL struct ISysCADCmdVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISysCADCmd_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISysCADCmd_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISysCADCmd_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISysCADCmd_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISysCADCmd_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISysCADCmd_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISysCADCmd_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISysCADCmd_INTERFACE_DEFINED__ */



#ifndef __PLC5Lib_LIBRARY_DEFINED__
#define __PLC5Lib_LIBRARY_DEFINED__

/* library PLC5Lib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_PLC5Lib;

EXTERN_C const CLSID CLSID_SysCADCmd;

#ifdef __cplusplus

class DECLSPEC_UUID("09087B23-B280-4A17-9A7C-B5452E2C3EA7")
SysCADCmd;
#endif
#endif /* __PLC5Lib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


