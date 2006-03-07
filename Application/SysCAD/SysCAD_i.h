

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0361 */
/* at Tue Mar 07 15:38:04 2006
 */
/* Compiler settings for .\SysCAD.odl:
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


#ifndef __SysCAD_i_h__
#define __SysCAD_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IScdApplication_FWD_DEFINED__
#define __IScdApplication_FWD_DEFINED__
typedef interface IScdApplication IScdApplication;
#endif 	/* __IScdApplication_FWD_DEFINED__ */


#ifndef __IScdGraphic_FWD_DEFINED__
#define __IScdGraphic_FWD_DEFINED__
typedef interface IScdGraphic IScdGraphic;
#endif 	/* __IScdGraphic_FWD_DEFINED__ */


#ifndef __IScdGraphics_FWD_DEFINED__
#define __IScdGraphics_FWD_DEFINED__
typedef interface IScdGraphics IScdGraphics;
#endif 	/* __IScdGraphics_FWD_DEFINED__ */


#ifndef __IScdHistorian_FWD_DEFINED__
#define __IScdHistorian_FWD_DEFINED__
typedef interface IScdHistorian IScdHistorian;
#endif 	/* __IScdHistorian_FWD_DEFINED__ */


#ifndef __IScdLicenseApp_FWD_DEFINED__
#define __IScdLicenseApp_FWD_DEFINED__
typedef interface IScdLicenseApp IScdLicenseApp;
#endif 	/* __IScdLicenseApp_FWD_DEFINED__ */


#ifndef __IScdOptions_FWD_DEFINED__
#define __IScdOptions_FWD_DEFINED__
typedef interface IScdOptions IScdOptions;
#endif 	/* __IScdOptions_FWD_DEFINED__ */


#ifndef __IScdProject_FWD_DEFINED__
#define __IScdProject_FWD_DEFINED__
typedef interface IScdProject IScdProject;
#endif 	/* __IScdProject_FWD_DEFINED__ */


#ifndef __IScdReplay_FWD_DEFINED__
#define __IScdReplay_FWD_DEFINED__
typedef interface IScdReplay IScdReplay;
#endif 	/* __IScdReplay_FWD_DEFINED__ */


#ifndef __IScdReport_FWD_DEFINED__
#define __IScdReport_FWD_DEFINED__
typedef interface IScdReport IScdReport;
#endif 	/* __IScdReport_FWD_DEFINED__ */


#ifndef __IScdReports_FWD_DEFINED__
#define __IScdReports_FWD_DEFINED__
typedef interface IScdReports IScdReports;
#endif 	/* __IScdReports_FWD_DEFINED__ */


#ifndef __IScdSnapshot_FWD_DEFINED__
#define __IScdSnapshot_FWD_DEFINED__
typedef interface IScdSnapshot IScdSnapshot;
#endif 	/* __IScdSnapshot_FWD_DEFINED__ */


#ifndef __IScdTrend_FWD_DEFINED__
#define __IScdTrend_FWD_DEFINED__
typedef interface IScdTrend IScdTrend;
#endif 	/* __IScdTrend_FWD_DEFINED__ */


#ifndef __IScdTrends_FWD_DEFINED__
#define __IScdTrends_FWD_DEFINED__
typedef interface IScdTrends IScdTrends;
#endif 	/* __IScdTrends_FWD_DEFINED__ */


#ifndef __IScdAppTag_FWD_DEFINED__
#define __IScdAppTag_FWD_DEFINED__
typedef interface IScdAppTag IScdAppTag;
#endif 	/* __IScdAppTag_FWD_DEFINED__ */


#ifndef __IScdAppTags_FWD_DEFINED__
#define __IScdAppTags_FWD_DEFINED__
typedef interface IScdAppTags IScdAppTags;
#endif 	/* __IScdAppTags_FWD_DEFINED__ */


#ifndef __IScdOPCServer_FWD_DEFINED__
#define __IScdOPCServer_FWD_DEFINED__
typedef interface IScdOPCServer IScdOPCServer;
#endif 	/* __IScdOPCServer_FWD_DEFINED__ */


#ifndef __IScdDDEServer_FWD_DEFINED__
#define __IScdDDEServer_FWD_DEFINED__
typedef interface IScdDDEServer IScdDDEServer;
#endif 	/* __IScdDDEServer_FWD_DEFINED__ */


#ifndef __IScdIOMarshal_FWD_DEFINED__
#define __IScdIOMarshal_FWD_DEFINED__
typedef interface IScdIOMarshal IScdIOMarshal;
#endif 	/* __IScdIOMarshal_FWD_DEFINED__ */


#ifndef __IScdTagFixup_FWD_DEFINED__
#define __IScdTagFixup_FWD_DEFINED__
typedef interface IScdTagFixup IScdTagFixup;
#endif 	/* __IScdTagFixup_FWD_DEFINED__ */


#ifndef ___IScdApplicationEvents_FWD_DEFINED__
#define ___IScdApplicationEvents_FWD_DEFINED__
typedef interface _IScdApplicationEvents _IScdApplicationEvents;
#endif 	/* ___IScdApplicationEvents_FWD_DEFINED__ */


#ifndef __ScdApplication_FWD_DEFINED__
#define __ScdApplication_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdApplication ScdApplication;
#else
typedef struct ScdApplication ScdApplication;
#endif /* __cplusplus */

#endif 	/* __ScdApplication_FWD_DEFINED__ */


#ifndef ___IScdGraphicsEvents_FWD_DEFINED__
#define ___IScdGraphicsEvents_FWD_DEFINED__
typedef interface _IScdGraphicsEvents _IScdGraphicsEvents;
#endif 	/* ___IScdGraphicsEvents_FWD_DEFINED__ */


#ifndef __ScdGraphics_FWD_DEFINED__
#define __ScdGraphics_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdGraphics ScdGraphics;
#else
typedef struct ScdGraphics ScdGraphics;
#endif /* __cplusplus */

#endif 	/* __ScdGraphics_FWD_DEFINED__ */


#ifndef ___IScdGraphicEvents_FWD_DEFINED__
#define ___IScdGraphicEvents_FWD_DEFINED__
typedef interface _IScdGraphicEvents _IScdGraphicEvents;
#endif 	/* ___IScdGraphicEvents_FWD_DEFINED__ */


#ifndef __ScdGraphic_FWD_DEFINED__
#define __ScdGraphic_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdGraphic ScdGraphic;
#else
typedef struct ScdGraphic ScdGraphic;
#endif /* __cplusplus */

#endif 	/* __ScdGraphic_FWD_DEFINED__ */


#ifndef ___IScdTrendsEvents_FWD_DEFINED__
#define ___IScdTrendsEvents_FWD_DEFINED__
typedef interface _IScdTrendsEvents _IScdTrendsEvents;
#endif 	/* ___IScdTrendsEvents_FWD_DEFINED__ */


#ifndef __ScdTrends_FWD_DEFINED__
#define __ScdTrends_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdTrends ScdTrends;
#else
typedef struct ScdTrends ScdTrends;
#endif /* __cplusplus */

#endif 	/* __ScdTrends_FWD_DEFINED__ */


#ifndef ___IScdTrendEvents_FWD_DEFINED__
#define ___IScdTrendEvents_FWD_DEFINED__
typedef interface _IScdTrendEvents _IScdTrendEvents;
#endif 	/* ___IScdTrendEvents_FWD_DEFINED__ */


#ifndef __ScdTrend_FWD_DEFINED__
#define __ScdTrend_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdTrend ScdTrend;
#else
typedef struct ScdTrend ScdTrend;
#endif /* __cplusplus */

#endif 	/* __ScdTrend_FWD_DEFINED__ */


#ifndef ___IScdReportsEvents_FWD_DEFINED__
#define ___IScdReportsEvents_FWD_DEFINED__
typedef interface _IScdReportsEvents _IScdReportsEvents;
#endif 	/* ___IScdReportsEvents_FWD_DEFINED__ */


#ifndef __ScdReports_FWD_DEFINED__
#define __ScdReports_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdReports ScdReports;
#else
typedef struct ScdReports ScdReports;
#endif /* __cplusplus */

#endif 	/* __ScdReports_FWD_DEFINED__ */


#ifndef ___IScdReportEvents_FWD_DEFINED__
#define ___IScdReportEvents_FWD_DEFINED__
typedef interface _IScdReportEvents _IScdReportEvents;
#endif 	/* ___IScdReportEvents_FWD_DEFINED__ */


#ifndef __ScdReport_FWD_DEFINED__
#define __ScdReport_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdReport ScdReport;
#else
typedef struct ScdReport ScdReport;
#endif /* __cplusplus */

#endif 	/* __ScdReport_FWD_DEFINED__ */


#ifndef ___IScdSnapshotEvents_FWD_DEFINED__
#define ___IScdSnapshotEvents_FWD_DEFINED__
typedef interface _IScdSnapshotEvents _IScdSnapshotEvents;
#endif 	/* ___IScdSnapshotEvents_FWD_DEFINED__ */


#ifndef __ScdSnapshot_FWD_DEFINED__
#define __ScdSnapshot_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdSnapshot ScdSnapshot;
#else
typedef struct ScdSnapshot ScdSnapshot;
#endif /* __cplusplus */

#endif 	/* __ScdSnapshot_FWD_DEFINED__ */


#ifndef ___IScdHistorianEvents_FWD_DEFINED__
#define ___IScdHistorianEvents_FWD_DEFINED__
typedef interface _IScdHistorianEvents _IScdHistorianEvents;
#endif 	/* ___IScdHistorianEvents_FWD_DEFINED__ */


#ifndef __ScdHistorian_FWD_DEFINED__
#define __ScdHistorian_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdHistorian ScdHistorian;
#else
typedef struct ScdHistorian ScdHistorian;
#endif /* __cplusplus */

#endif 	/* __ScdHistorian_FWD_DEFINED__ */


#ifndef ___IScdReplayEvents_FWD_DEFINED__
#define ___IScdReplayEvents_FWD_DEFINED__
typedef interface _IScdReplayEvents _IScdReplayEvents;
#endif 	/* ___IScdReplayEvents_FWD_DEFINED__ */


#ifndef __ScdReplay_FWD_DEFINED__
#define __ScdReplay_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdReplay ScdReplay;
#else
typedef struct ScdReplay ScdReplay;
#endif /* __cplusplus */

#endif 	/* __ScdReplay_FWD_DEFINED__ */


#ifndef ___IScdProjectEvents_FWD_DEFINED__
#define ___IScdProjectEvents_FWD_DEFINED__
typedef interface _IScdProjectEvents _IScdProjectEvents;
#endif 	/* ___IScdProjectEvents_FWD_DEFINED__ */


#ifndef __ScdProject_FWD_DEFINED__
#define __ScdProject_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdProject ScdProject;
#else
typedef struct ScdProject ScdProject;
#endif /* __cplusplus */

#endif 	/* __ScdProject_FWD_DEFINED__ */


#ifndef ___IScdLicenseAppEvents_FWD_DEFINED__
#define ___IScdLicenseAppEvents_FWD_DEFINED__
typedef interface _IScdLicenseAppEvents _IScdLicenseAppEvents;
#endif 	/* ___IScdLicenseAppEvents_FWD_DEFINED__ */


#ifndef __ScdLicenseApp_FWD_DEFINED__
#define __ScdLicenseApp_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdLicenseApp ScdLicenseApp;
#else
typedef struct ScdLicenseApp ScdLicenseApp;
#endif /* __cplusplus */

#endif 	/* __ScdLicenseApp_FWD_DEFINED__ */


#ifndef ___IScdOptionsEvents_FWD_DEFINED__
#define ___IScdOptionsEvents_FWD_DEFINED__
typedef interface _IScdOptionsEvents _IScdOptionsEvents;
#endif 	/* ___IScdOptionsEvents_FWD_DEFINED__ */


#ifndef __ScdOptions_FWD_DEFINED__
#define __ScdOptions_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdOptions ScdOptions;
#else
typedef struct ScdOptions ScdOptions;
#endif /* __cplusplus */

#endif 	/* __ScdOptions_FWD_DEFINED__ */


#ifndef ___IScdAppTagsEvents_FWD_DEFINED__
#define ___IScdAppTagsEvents_FWD_DEFINED__
typedef interface _IScdAppTagsEvents _IScdAppTagsEvents;
#endif 	/* ___IScdAppTagsEvents_FWD_DEFINED__ */


#ifndef __ScdAppTags_FWD_DEFINED__
#define __ScdAppTags_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdAppTags ScdAppTags;
#else
typedef struct ScdAppTags ScdAppTags;
#endif /* __cplusplus */

#endif 	/* __ScdAppTags_FWD_DEFINED__ */


#ifndef ___IScdAppTagEvents_FWD_DEFINED__
#define ___IScdAppTagEvents_FWD_DEFINED__
typedef interface _IScdAppTagEvents _IScdAppTagEvents;
#endif 	/* ___IScdAppTagEvents_FWD_DEFINED__ */


#ifndef __ScdAppTag_FWD_DEFINED__
#define __ScdAppTag_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdAppTag ScdAppTag;
#else
typedef struct ScdAppTag ScdAppTag;
#endif /* __cplusplus */

#endif 	/* __ScdAppTag_FWD_DEFINED__ */


#ifndef ___IScdOPCServerEvents_FWD_DEFINED__
#define ___IScdOPCServerEvents_FWD_DEFINED__
typedef interface _IScdOPCServerEvents _IScdOPCServerEvents;
#endif 	/* ___IScdOPCServerEvents_FWD_DEFINED__ */


#ifndef __ScdOPCServer_FWD_DEFINED__
#define __ScdOPCServer_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdOPCServer ScdOPCServer;
#else
typedef struct ScdOPCServer ScdOPCServer;
#endif /* __cplusplus */

#endif 	/* __ScdOPCServer_FWD_DEFINED__ */


#ifndef ___IScdDDEServerEvents_FWD_DEFINED__
#define ___IScdDDEServerEvents_FWD_DEFINED__
typedef interface _IScdDDEServerEvents _IScdDDEServerEvents;
#endif 	/* ___IScdDDEServerEvents_FWD_DEFINED__ */


#ifndef __ScdDDEServer_FWD_DEFINED__
#define __ScdDDEServer_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdDDEServer ScdDDEServer;
#else
typedef struct ScdDDEServer ScdDDEServer;
#endif /* __cplusplus */

#endif 	/* __ScdDDEServer_FWD_DEFINED__ */


#ifndef ___IScdIOMarshalEvents_FWD_DEFINED__
#define ___IScdIOMarshalEvents_FWD_DEFINED__
typedef interface _IScdIOMarshalEvents _IScdIOMarshalEvents;
#endif 	/* ___IScdIOMarshalEvents_FWD_DEFINED__ */


#ifndef __ScdIOMarshal_FWD_DEFINED__
#define __ScdIOMarshal_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdIOMarshal ScdIOMarshal;
#else
typedef struct ScdIOMarshal ScdIOMarshal;
#endif /* __cplusplus */

#endif 	/* __ScdIOMarshal_FWD_DEFINED__ */


#ifndef __ScdTagFixup_FWD_DEFINED__
#define __ScdTagFixup_FWD_DEFINED__

#ifdef __cplusplus
typedef class ScdTagFixup ScdTagFixup;
#else
typedef struct ScdTagFixup ScdTagFixup;
#endif /* __cplusplus */

#endif 	/* __ScdTagFixup_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ScdIF.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 


#ifndef __ScdApp_LIBRARY_DEFINED__
#define __ScdApp_LIBRARY_DEFINED__

/* library ScdApp */
/* [helpstring][version][uuid] */ 





















EXTERN_C const IID LIBID_ScdApp;

#ifndef __IScdApplication_INTERFACE_DEFINED__
#define __IScdApplication_INTERFACE_DEFINED__

/* interface IScdApplication */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdApplication;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D7E8F427-C1AF-41D3-9920-6D92C012D936")
    IScdApplication : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Debug( 
            /* [retval][out] */ /* external definition not present */ IScdDebug **pDebug) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Test( 
            /* [retval][out] */ /* external definition not present */ IScdTest **pTest) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Messages( 
            /* [retval][out] */ /* external definition not present */ IScdMessages **pMessages) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OpenProject( 
            /* [in] */ BSTR PrjFolder,
            /* [retval][out] */ IScdProject **pProject) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateProject( 
            /* [in] */ BSTR CfgFolder,
            /* [in] */ BSTR PrjFolder,
            /* [defaultvalue][in] */ VARIANT_BOOL WithVersion,
            /* [defaultvalue][in] */ VARIANT_BOOL RemoveOld,
            /* [retval][out] */ IScdProject **pProject) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_License( 
            /* [retval][out] */ IScdLicenseApp **pLicense) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Options( 
            /* [retval][out] */ IScdOptions **pOptions) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CloseProject( 
            /* [defaultvalue][in] */ VARIANT_BOOL SaveIt = FALSE) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetWindowState( 
            /* [in] */ eScdWndStateCmds ReqdState) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Project( 
            /* [retval][out] */ IScdProject **pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScdApplicationVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdApplication * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdApplication * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdApplication * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdApplication * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdApplication * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdApplication * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdApplication * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Debug )( 
            IScdApplication * This,
            /* [retval][out] */ /* external definition not present */ IScdDebug **pDebug);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Test )( 
            IScdApplication * This,
            /* [retval][out] */ /* external definition not present */ IScdTest **pTest);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Messages )( 
            IScdApplication * This,
            /* [retval][out] */ /* external definition not present */ IScdMessages **pMessages);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OpenProject )( 
            IScdApplication * This,
            /* [in] */ BSTR PrjFolder,
            /* [retval][out] */ IScdProject **pProject);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateProject )( 
            IScdApplication * This,
            /* [in] */ BSTR CfgFolder,
            /* [in] */ BSTR PrjFolder,
            /* [defaultvalue][in] */ VARIANT_BOOL WithVersion,
            /* [defaultvalue][in] */ VARIANT_BOOL RemoveOld,
            /* [retval][out] */ IScdProject **pProject);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_License )( 
            IScdApplication * This,
            /* [retval][out] */ IScdLicenseApp **pLicense);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Options )( 
            IScdApplication * This,
            /* [retval][out] */ IScdOptions **pOptions);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CloseProject )( 
            IScdApplication * This,
            /* [defaultvalue][in] */ VARIANT_BOOL SaveIt);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetWindowState )( 
            IScdApplication * This,
            /* [in] */ eScdWndStateCmds ReqdState);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Project )( 
            IScdApplication * This,
            /* [retval][out] */ IScdProject **pVal);
        
        END_INTERFACE
    } IScdApplicationVtbl;

    interface IScdApplication
    {
        CONST_VTBL struct IScdApplicationVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdApplication_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdApplication_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdApplication_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdApplication_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdApplication_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdApplication_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdApplication_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IScdApplication_get_Debug(This,pDebug)	\
    (This)->lpVtbl -> get_Debug(This,pDebug)

#define IScdApplication_get_Test(This,pTest)	\
    (This)->lpVtbl -> get_Test(This,pTest)

#define IScdApplication_get_Messages(This,pMessages)	\
    (This)->lpVtbl -> get_Messages(This,pMessages)

#define IScdApplication_OpenProject(This,PrjFolder,pProject)	\
    (This)->lpVtbl -> OpenProject(This,PrjFolder,pProject)

#define IScdApplication_CreateProject(This,CfgFolder,PrjFolder,WithVersion,RemoveOld,pProject)	\
    (This)->lpVtbl -> CreateProject(This,CfgFolder,PrjFolder,WithVersion,RemoveOld,pProject)

#define IScdApplication_get_License(This,pLicense)	\
    (This)->lpVtbl -> get_License(This,pLicense)

#define IScdApplication_get_Options(This,pOptions)	\
    (This)->lpVtbl -> get_Options(This,pOptions)

#define IScdApplication_CloseProject(This,SaveIt)	\
    (This)->lpVtbl -> CloseProject(This,SaveIt)

#define IScdApplication_SetWindowState(This,ReqdState)	\
    (This)->lpVtbl -> SetWindowState(This,ReqdState)

#define IScdApplication_get_Project(This,pVal)	\
    (This)->lpVtbl -> get_Project(This,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdApplication_get_Debug_Proxy( 
    IScdApplication * This,
    /* [retval][out] */ /* external definition not present */ IScdDebug **pDebug);


void __RPC_STUB IScdApplication_get_Debug_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdApplication_get_Test_Proxy( 
    IScdApplication * This,
    /* [retval][out] */ /* external definition not present */ IScdTest **pTest);


void __RPC_STUB IScdApplication_get_Test_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdApplication_get_Messages_Proxy( 
    IScdApplication * This,
    /* [retval][out] */ /* external definition not present */ IScdMessages **pMessages);


void __RPC_STUB IScdApplication_get_Messages_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdApplication_OpenProject_Proxy( 
    IScdApplication * This,
    /* [in] */ BSTR PrjFolder,
    /* [retval][out] */ IScdProject **pProject);


void __RPC_STUB IScdApplication_OpenProject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdApplication_CreateProject_Proxy( 
    IScdApplication * This,
    /* [in] */ BSTR CfgFolder,
    /* [in] */ BSTR PrjFolder,
    /* [defaultvalue][in] */ VARIANT_BOOL WithVersion,
    /* [defaultvalue][in] */ VARIANT_BOOL RemoveOld,
    /* [retval][out] */ IScdProject **pProject);


void __RPC_STUB IScdApplication_CreateProject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdApplication_get_License_Proxy( 
    IScdApplication * This,
    /* [retval][out] */ IScdLicenseApp **pLicense);


void __RPC_STUB IScdApplication_get_License_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdApplication_get_Options_Proxy( 
    IScdApplication * This,
    /* [retval][out] */ IScdOptions **pOptions);


void __RPC_STUB IScdApplication_get_Options_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdApplication_CloseProject_Proxy( 
    IScdApplication * This,
    /* [defaultvalue][in] */ VARIANT_BOOL SaveIt);


void __RPC_STUB IScdApplication_CloseProject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdApplication_SetWindowState_Proxy( 
    IScdApplication * This,
    /* [in] */ eScdWndStateCmds ReqdState);


void __RPC_STUB IScdApplication_SetWindowState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdApplication_get_Project_Proxy( 
    IScdApplication * This,
    /* [retval][out] */ IScdProject **pVal);


void __RPC_STUB IScdApplication_get_Project_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScdApplication_INTERFACE_DEFINED__ */


#ifndef __IScdGraphic_INTERFACE_DEFINED__
#define __IScdGraphic_INTERFACE_DEFINED__

/* interface IScdGraphic */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdGraphic;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5C215D42-05B4-4755-AE96-677D6B2EAFC3")
    IScdGraphic : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Name( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddUnit( 
            /* [in] */ BSTR Tag,
            /* [in] */ double X,
            /* [in] */ double Y,
            /* [in] */ double Z,
            /* [in] */ BSTR Symbol,
            /* [in] */ double XScale,
            /* [in] */ double YScale,
            /* [in] */ double Rotation) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Zoom( 
            /* [defaultvalue][in] */ eScdZooms zoom = eScdZoom_Full) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddLinkA( 
            /* [in] */ BSTR Tag,
            /* [in] */ VARIANT Points3D) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddLinkM( 
            /* [in] */ BSTR Tag,
            /* [in] */ eScdLinkDrawMethods Method) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE InsertGroup( 
            /* [in] */ BSTR Library,
            /* [in] */ BSTR Group,
            /* [in] */ DOUBLE X,
            /* [in] */ DOUBLE Y,
            /* [in] */ DOUBLE Z,
            /* [in] */ DOUBLE XScale,
            /* [in] */ DOUBLE YScale,
            /* [in] */ DOUBLE ZScale,
            /* [in] */ IScdTagFixup *TagFixups,
            /* [defaultvalue][in] */ eScdNDBOptions Options = eNDB_StdImport) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScdGraphicVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdGraphic * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdGraphic * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdGraphic * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdGraphic * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdGraphic * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdGraphic * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdGraphic * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            IScdGraphic * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Name )( 
            IScdGraphic * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AddUnit )( 
            IScdGraphic * This,
            /* [in] */ BSTR Tag,
            /* [in] */ double X,
            /* [in] */ double Y,
            /* [in] */ double Z,
            /* [in] */ BSTR Symbol,
            /* [in] */ double XScale,
            /* [in] */ double YScale,
            /* [in] */ double Rotation);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Zoom )( 
            IScdGraphic * This,
            /* [defaultvalue][in] */ eScdZooms zoom);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AddLinkA )( 
            IScdGraphic * This,
            /* [in] */ BSTR Tag,
            /* [in] */ VARIANT Points3D);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AddLinkM )( 
            IScdGraphic * This,
            /* [in] */ BSTR Tag,
            /* [in] */ eScdLinkDrawMethods Method);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *InsertGroup )( 
            IScdGraphic * This,
            /* [in] */ BSTR Library,
            /* [in] */ BSTR Group,
            /* [in] */ DOUBLE X,
            /* [in] */ DOUBLE Y,
            /* [in] */ DOUBLE Z,
            /* [in] */ DOUBLE XScale,
            /* [in] */ DOUBLE YScale,
            /* [in] */ DOUBLE ZScale,
            /* [in] */ IScdTagFixup *TagFixups,
            /* [defaultvalue][in] */ eScdNDBOptions Options);
        
        END_INTERFACE
    } IScdGraphicVtbl;

    interface IScdGraphic
    {
        CONST_VTBL struct IScdGraphicVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdGraphic_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdGraphic_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdGraphic_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdGraphic_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdGraphic_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdGraphic_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdGraphic_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IScdGraphic_get_Name(This,pVal)	\
    (This)->lpVtbl -> get_Name(This,pVal)

#define IScdGraphic_put_Name(This,newVal)	\
    (This)->lpVtbl -> put_Name(This,newVal)

#define IScdGraphic_AddUnit(This,Tag,X,Y,Z,Symbol,XScale,YScale,Rotation)	\
    (This)->lpVtbl -> AddUnit(This,Tag,X,Y,Z,Symbol,XScale,YScale,Rotation)

#define IScdGraphic_Zoom(This,zoom)	\
    (This)->lpVtbl -> Zoom(This,zoom)

#define IScdGraphic_AddLinkA(This,Tag,Points3D)	\
    (This)->lpVtbl -> AddLinkA(This,Tag,Points3D)

#define IScdGraphic_AddLinkM(This,Tag,Method)	\
    (This)->lpVtbl -> AddLinkM(This,Tag,Method)

#define IScdGraphic_InsertGroup(This,Library,Group,X,Y,Z,XScale,YScale,ZScale,TagFixups,Options)	\
    (This)->lpVtbl -> InsertGroup(This,Library,Group,X,Y,Z,XScale,YScale,ZScale,TagFixups,Options)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdGraphic_get_Name_Proxy( 
    IScdGraphic * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IScdGraphic_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IScdGraphic_put_Name_Proxy( 
    IScdGraphic * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IScdGraphic_put_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdGraphic_AddUnit_Proxy( 
    IScdGraphic * This,
    /* [in] */ BSTR Tag,
    /* [in] */ double X,
    /* [in] */ double Y,
    /* [in] */ double Z,
    /* [in] */ BSTR Symbol,
    /* [in] */ double XScale,
    /* [in] */ double YScale,
    /* [in] */ double Rotation);


void __RPC_STUB IScdGraphic_AddUnit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdGraphic_Zoom_Proxy( 
    IScdGraphic * This,
    /* [defaultvalue][in] */ eScdZooms zoom);


void __RPC_STUB IScdGraphic_Zoom_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdGraphic_AddLinkA_Proxy( 
    IScdGraphic * This,
    /* [in] */ BSTR Tag,
    /* [in] */ VARIANT Points3D);


void __RPC_STUB IScdGraphic_AddLinkA_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdGraphic_AddLinkM_Proxy( 
    IScdGraphic * This,
    /* [in] */ BSTR Tag,
    /* [in] */ eScdLinkDrawMethods Method);


void __RPC_STUB IScdGraphic_AddLinkM_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdGraphic_InsertGroup_Proxy( 
    IScdGraphic * This,
    /* [in] */ BSTR Library,
    /* [in] */ BSTR Group,
    /* [in] */ DOUBLE X,
    /* [in] */ DOUBLE Y,
    /* [in] */ DOUBLE Z,
    /* [in] */ DOUBLE XScale,
    /* [in] */ DOUBLE YScale,
    /* [in] */ DOUBLE ZScale,
    /* [in] */ IScdTagFixup *TagFixups,
    /* [defaultvalue][in] */ eScdNDBOptions Options);


void __RPC_STUB IScdGraphic_InsertGroup_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScdGraphic_INTERFACE_DEFINED__ */


#ifndef __IScdGraphics_INTERFACE_DEFINED__
#define __IScdGraphics_INTERFACE_DEFINED__

/* interface IScdGraphics */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdGraphics;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("F3E7D068-8CFE-470F-802D-173DBE3DEB89")
    IScdGraphics : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Page( 
            /* [in] */ VARIANT WhichOne,
            /* [retval][out] */ IScdGraphic **pItem) = 0;
        
        virtual /* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ LPUNKNOWN *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ BSTR Name,
            /* [retval][out] */ IScdGraphic **pItem) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Remove( 
            /* [in] */ VARIANT WhichOne) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ReturnCode( 
            /* [retval][out] */ eScdGraphicsCodes *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScdGraphicsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdGraphics * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdGraphics * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdGraphics * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdGraphics * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdGraphics * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdGraphics * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdGraphics * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Page )( 
            IScdGraphics * This,
            /* [in] */ VARIANT WhichOne,
            /* [retval][out] */ IScdGraphic **pItem);
        
        /* [id][restricted][propget] */ HRESULT ( STDMETHODCALLTYPE *get__NewEnum )( 
            IScdGraphics * This,
            /* [retval][out] */ LPUNKNOWN *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            IScdGraphics * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Add )( 
            IScdGraphics * This,
            /* [in] */ BSTR Name,
            /* [retval][out] */ IScdGraphic **pItem);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Remove )( 
            IScdGraphics * This,
            /* [in] */ VARIANT WhichOne);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ReturnCode )( 
            IScdGraphics * This,
            /* [retval][out] */ eScdGraphicsCodes *pVal);
        
        END_INTERFACE
    } IScdGraphicsVtbl;

    interface IScdGraphics
    {
        CONST_VTBL struct IScdGraphicsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdGraphics_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdGraphics_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdGraphics_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdGraphics_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdGraphics_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdGraphics_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdGraphics_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IScdGraphics_Page(This,WhichOne,pItem)	\
    (This)->lpVtbl -> Page(This,WhichOne,pItem)

#define IScdGraphics_get__NewEnum(This,pVal)	\
    (This)->lpVtbl -> get__NewEnum(This,pVal)

#define IScdGraphics_get_Count(This,pVal)	\
    (This)->lpVtbl -> get_Count(This,pVal)

#define IScdGraphics_Add(This,Name,pItem)	\
    (This)->lpVtbl -> Add(This,Name,pItem)

#define IScdGraphics_Remove(This,WhichOne)	\
    (This)->lpVtbl -> Remove(This,WhichOne)

#define IScdGraphics_get_ReturnCode(This,pVal)	\
    (This)->lpVtbl -> get_ReturnCode(This,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdGraphics_Page_Proxy( 
    IScdGraphics * This,
    /* [in] */ VARIANT WhichOne,
    /* [retval][out] */ IScdGraphic **pItem);


void __RPC_STUB IScdGraphics_Page_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE IScdGraphics_get__NewEnum_Proxy( 
    IScdGraphics * This,
    /* [retval][out] */ LPUNKNOWN *pVal);


void __RPC_STUB IScdGraphics_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdGraphics_get_Count_Proxy( 
    IScdGraphics * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IScdGraphics_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdGraphics_Add_Proxy( 
    IScdGraphics * This,
    /* [in] */ BSTR Name,
    /* [retval][out] */ IScdGraphic **pItem);


void __RPC_STUB IScdGraphics_Add_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdGraphics_Remove_Proxy( 
    IScdGraphics * This,
    /* [in] */ VARIANT WhichOne);


void __RPC_STUB IScdGraphics_Remove_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdGraphics_get_ReturnCode_Proxy( 
    IScdGraphics * This,
    /* [retval][out] */ eScdGraphicsCodes *pVal);


void __RPC_STUB IScdGraphics_get_ReturnCode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScdGraphics_INTERFACE_DEFINED__ */


#ifndef __IScdHistorian_INTERFACE_DEFINED__
#define __IScdHistorian_INTERFACE_DEFINED__

/* interface IScdHistorian */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdHistorian;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("38A910D0-F4E2-4D6F-AB87-D74908A2CED0")
    IScdHistorian : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Delete( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RecordTags( 
            /* [in] */ BSTR TagFileName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DeleteData( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ExtractData( 
            /* [in] */ BSTR FileName,
            /* [in] */ eScdTimeFormat Fmt,
            /* [in] */ VARIANT StartTime,
            /* [in] */ DOUBLE Duration,
            /* [in] */ DOUBLE Interval,
            /* [defaultvalue][in] */ eScdHistDataExtractOptions Options = eScdHDEO_CSV) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScdHistorianVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdHistorian * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdHistorian * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdHistorian * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdHistorian * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdHistorian * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdHistorian * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdHistorian * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Delete )( 
            IScdHistorian * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RecordTags )( 
            IScdHistorian * This,
            /* [in] */ BSTR TagFileName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DeleteData )( 
            IScdHistorian * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ExtractData )( 
            IScdHistorian * This,
            /* [in] */ BSTR FileName,
            /* [in] */ eScdTimeFormat Fmt,
            /* [in] */ VARIANT StartTime,
            /* [in] */ DOUBLE Duration,
            /* [in] */ DOUBLE Interval,
            /* [defaultvalue][in] */ eScdHistDataExtractOptions Options);
        
        END_INTERFACE
    } IScdHistorianVtbl;

    interface IScdHistorian
    {
        CONST_VTBL struct IScdHistorianVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdHistorian_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdHistorian_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdHistorian_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdHistorian_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdHistorian_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdHistorian_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdHistorian_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IScdHistorian_Delete(This)	\
    (This)->lpVtbl -> Delete(This)

#define IScdHistorian_RecordTags(This,TagFileName)	\
    (This)->lpVtbl -> RecordTags(This,TagFileName)

#define IScdHistorian_DeleteData(This)	\
    (This)->lpVtbl -> DeleteData(This)

#define IScdHistorian_ExtractData(This,FileName,Fmt,StartTime,Duration,Interval,Options)	\
    (This)->lpVtbl -> ExtractData(This,FileName,Fmt,StartTime,Duration,Interval,Options)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdHistorian_Delete_Proxy( 
    IScdHistorian * This);


void __RPC_STUB IScdHistorian_Delete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdHistorian_RecordTags_Proxy( 
    IScdHistorian * This,
    /* [in] */ BSTR TagFileName);


void __RPC_STUB IScdHistorian_RecordTags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdHistorian_DeleteData_Proxy( 
    IScdHistorian * This);


void __RPC_STUB IScdHistorian_DeleteData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdHistorian_ExtractData_Proxy( 
    IScdHistorian * This,
    /* [in] */ BSTR FileName,
    /* [in] */ eScdTimeFormat Fmt,
    /* [in] */ VARIANT StartTime,
    /* [in] */ DOUBLE Duration,
    /* [in] */ DOUBLE Interval,
    /* [defaultvalue][in] */ eScdHistDataExtractOptions Options);


void __RPC_STUB IScdHistorian_ExtractData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScdHistorian_INTERFACE_DEFINED__ */


#ifndef __IScdLicenseApp_INTERFACE_DEFINED__
#define __IScdLicenseApp_INTERFACE_DEFINED__

/* interface IScdLicenseApp */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdLicenseApp;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A9817A29-F032-4FFF-9964-017002D6AF7E")
    IScdLicenseApp : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IScdLicenseAppVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdLicenseApp * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdLicenseApp * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdLicenseApp * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdLicenseApp * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdLicenseApp * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdLicenseApp * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdLicenseApp * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IScdLicenseAppVtbl;

    interface IScdLicenseApp
    {
        CONST_VTBL struct IScdLicenseAppVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdLicenseApp_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdLicenseApp_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdLicenseApp_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdLicenseApp_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdLicenseApp_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdLicenseApp_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdLicenseApp_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IScdLicenseApp_INTERFACE_DEFINED__ */


#ifndef __IScdOptions_INTERFACE_DEFINED__
#define __IScdOptions_INTERFACE_DEFINED__

/* interface IScdOptions */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdOptions;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B354722B-A835-4B64-81EA-3974FDC36CAA")
    IScdOptions : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IScdOptionsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdOptions * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdOptions * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdOptions * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdOptions * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdOptions * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdOptions * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdOptions * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IScdOptionsVtbl;

    interface IScdOptions
    {
        CONST_VTBL struct IScdOptionsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdOptions_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdOptions_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdOptions_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdOptions_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdOptions_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdOptions_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdOptions_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IScdOptions_INTERFACE_DEFINED__ */


#ifndef __IScdProject_INTERFACE_DEFINED__
#define __IScdProject_INTERFACE_DEFINED__

/* interface IScdProject */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdProject;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C1DDBC81-7788-456A-A1EF-D4D0CDE6B8A5")
    IScdProject : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Graphics( 
            /* [retval][out] */ IScdGraphics **pGraphics) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Trends( 
            /* [retval][out] */ IScdTrends **pTrends) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Historian( 
            /* [retval][out] */ IScdHistorian **pHistorian) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Reports( 
            /* [retval][out] */ IScdReports **pReports) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Snapshot( 
            /* [retval][out] */ IScdSnapshot **pSnapshot) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Solver( 
            /* [retval][out] */ /* external definition not present */ IScdSolver **pSolver) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SpecieDefns( 
            /* [retval][out] */ /* external definition not present */ IScdSpecieDefns **pSpecieDefns) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Tags( 
            /* [retval][out] */ IScdAppTags **pTags) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Save( 
            /* [defaultvalue][in] */ BSTR Filename = L"",
            /* [defaultvalue][in] */ VARIANT_BOOL NewVersion = FALSE) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_OPCServer( 
            /* [retval][out] */ IScdOPCServer **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_IOMarshal( 
            /* [retval][out] */ IScdIOMarshal **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_DDEServer( 
            /* [retval][out] */ IScdDDEServer **pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_CfgFolder( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_PrjFolder( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ExportNeutralDB( 
            /* [in] */ eScdNDBOptions Options,
            /* [in] */ BSTR GraphicsDatabase,
            /* [in] */ BSTR ModelDatabase) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ImportNeutralDB( 
            /* [in] */ eScdNDBOptions Options,
            /* [in] */ BSTR GraphicsDatabase,
            /* [in] */ BSTR ModelDatabase,
            /* [in] */ IScdTagFixup *TagFixups) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScdProjectVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdProject * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdProject * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdProject * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdProject * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdProject * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdProject * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdProject * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Graphics )( 
            IScdProject * This,
            /* [retval][out] */ IScdGraphics **pGraphics);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Trends )( 
            IScdProject * This,
            /* [retval][out] */ IScdTrends **pTrends);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Historian )( 
            IScdProject * This,
            /* [retval][out] */ IScdHistorian **pHistorian);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Reports )( 
            IScdProject * This,
            /* [retval][out] */ IScdReports **pReports);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Snapshot )( 
            IScdProject * This,
            /* [retval][out] */ IScdSnapshot **pSnapshot);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Solver )( 
            IScdProject * This,
            /* [retval][out] */ /* external definition not present */ IScdSolver **pSolver);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SpecieDefns )( 
            IScdProject * This,
            /* [retval][out] */ /* external definition not present */ IScdSpecieDefns **pSpecieDefns);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Tags )( 
            IScdProject * This,
            /* [retval][out] */ IScdAppTags **pTags);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Save )( 
            IScdProject * This,
            /* [defaultvalue][in] */ BSTR Filename,
            /* [defaultvalue][in] */ VARIANT_BOOL NewVersion);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_OPCServer )( 
            IScdProject * This,
            /* [retval][out] */ IScdOPCServer **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IOMarshal )( 
            IScdProject * This,
            /* [retval][out] */ IScdIOMarshal **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DDEServer )( 
            IScdProject * This,
            /* [retval][out] */ IScdDDEServer **pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_CfgFolder )( 
            IScdProject * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_PrjFolder )( 
            IScdProject * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ExportNeutralDB )( 
            IScdProject * This,
            /* [in] */ eScdNDBOptions Options,
            /* [in] */ BSTR GraphicsDatabase,
            /* [in] */ BSTR ModelDatabase);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ImportNeutralDB )( 
            IScdProject * This,
            /* [in] */ eScdNDBOptions Options,
            /* [in] */ BSTR GraphicsDatabase,
            /* [in] */ BSTR ModelDatabase,
            /* [in] */ IScdTagFixup *TagFixups);
        
        END_INTERFACE
    } IScdProjectVtbl;

    interface IScdProject
    {
        CONST_VTBL struct IScdProjectVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdProject_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdProject_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdProject_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdProject_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdProject_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdProject_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdProject_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IScdProject_get_Graphics(This,pGraphics)	\
    (This)->lpVtbl -> get_Graphics(This,pGraphics)

#define IScdProject_get_Trends(This,pTrends)	\
    (This)->lpVtbl -> get_Trends(This,pTrends)

#define IScdProject_get_Historian(This,pHistorian)	\
    (This)->lpVtbl -> get_Historian(This,pHistorian)

#define IScdProject_get_Reports(This,pReports)	\
    (This)->lpVtbl -> get_Reports(This,pReports)

#define IScdProject_get_Snapshot(This,pSnapshot)	\
    (This)->lpVtbl -> get_Snapshot(This,pSnapshot)

#define IScdProject_get_Solver(This,pSolver)	\
    (This)->lpVtbl -> get_Solver(This,pSolver)

#define IScdProject_get_SpecieDefns(This,pSpecieDefns)	\
    (This)->lpVtbl -> get_SpecieDefns(This,pSpecieDefns)

#define IScdProject_get_Tags(This,pTags)	\
    (This)->lpVtbl -> get_Tags(This,pTags)

#define IScdProject_Save(This,Filename,NewVersion)	\
    (This)->lpVtbl -> Save(This,Filename,NewVersion)

#define IScdProject_get_OPCServer(This,pVal)	\
    (This)->lpVtbl -> get_OPCServer(This,pVal)

#define IScdProject_get_IOMarshal(This,pVal)	\
    (This)->lpVtbl -> get_IOMarshal(This,pVal)

#define IScdProject_get_DDEServer(This,pVal)	\
    (This)->lpVtbl -> get_DDEServer(This,pVal)

#define IScdProject_get_CfgFolder(This,pVal)	\
    (This)->lpVtbl -> get_CfgFolder(This,pVal)

#define IScdProject_get_PrjFolder(This,pVal)	\
    (This)->lpVtbl -> get_PrjFolder(This,pVal)

#define IScdProject_ExportNeutralDB(This,Options,GraphicsDatabase,ModelDatabase)	\
    (This)->lpVtbl -> ExportNeutralDB(This,Options,GraphicsDatabase,ModelDatabase)

#define IScdProject_ImportNeutralDB(This,Options,GraphicsDatabase,ModelDatabase,TagFixups)	\
    (This)->lpVtbl -> ImportNeutralDB(This,Options,GraphicsDatabase,ModelDatabase,TagFixups)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdProject_get_Graphics_Proxy( 
    IScdProject * This,
    /* [retval][out] */ IScdGraphics **pGraphics);


void __RPC_STUB IScdProject_get_Graphics_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdProject_get_Trends_Proxy( 
    IScdProject * This,
    /* [retval][out] */ IScdTrends **pTrends);


void __RPC_STUB IScdProject_get_Trends_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdProject_get_Historian_Proxy( 
    IScdProject * This,
    /* [retval][out] */ IScdHistorian **pHistorian);


void __RPC_STUB IScdProject_get_Historian_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdProject_get_Reports_Proxy( 
    IScdProject * This,
    /* [retval][out] */ IScdReports **pReports);


void __RPC_STUB IScdProject_get_Reports_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdProject_get_Snapshot_Proxy( 
    IScdProject * This,
    /* [retval][out] */ IScdSnapshot **pSnapshot);


void __RPC_STUB IScdProject_get_Snapshot_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdProject_get_Solver_Proxy( 
    IScdProject * This,
    /* [retval][out] */ /* external definition not present */ IScdSolver **pSolver);


void __RPC_STUB IScdProject_get_Solver_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdProject_get_SpecieDefns_Proxy( 
    IScdProject * This,
    /* [retval][out] */ /* external definition not present */ IScdSpecieDefns **pSpecieDefns);


void __RPC_STUB IScdProject_get_SpecieDefns_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdProject_get_Tags_Proxy( 
    IScdProject * This,
    /* [retval][out] */ IScdAppTags **pTags);


void __RPC_STUB IScdProject_get_Tags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdProject_Save_Proxy( 
    IScdProject * This,
    /* [defaultvalue][in] */ BSTR Filename,
    /* [defaultvalue][in] */ VARIANT_BOOL NewVersion);


void __RPC_STUB IScdProject_Save_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdProject_get_OPCServer_Proxy( 
    IScdProject * This,
    /* [retval][out] */ IScdOPCServer **pVal);


void __RPC_STUB IScdProject_get_OPCServer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdProject_get_IOMarshal_Proxy( 
    IScdProject * This,
    /* [retval][out] */ IScdIOMarshal **pVal);


void __RPC_STUB IScdProject_get_IOMarshal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdProject_get_DDEServer_Proxy( 
    IScdProject * This,
    /* [retval][out] */ IScdDDEServer **pVal);


void __RPC_STUB IScdProject_get_DDEServer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdProject_get_CfgFolder_Proxy( 
    IScdProject * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IScdProject_get_CfgFolder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdProject_get_PrjFolder_Proxy( 
    IScdProject * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IScdProject_get_PrjFolder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdProject_ExportNeutralDB_Proxy( 
    IScdProject * This,
    /* [in] */ eScdNDBOptions Options,
    /* [in] */ BSTR GraphicsDatabase,
    /* [in] */ BSTR ModelDatabase);


void __RPC_STUB IScdProject_ExportNeutralDB_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdProject_ImportNeutralDB_Proxy( 
    IScdProject * This,
    /* [in] */ eScdNDBOptions Options,
    /* [in] */ BSTR GraphicsDatabase,
    /* [in] */ BSTR ModelDatabase,
    /* [in] */ IScdTagFixup *TagFixups);


void __RPC_STUB IScdProject_ImportNeutralDB_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScdProject_INTERFACE_DEFINED__ */


#ifndef __IScdReplay_INTERFACE_DEFINED__
#define __IScdReplay_INTERFACE_DEFINED__

/* interface IScdReplay */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdReplay;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0F869A01-707B-4E55-BD70-F7133264D53C")
    IScdReplay : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IScdReplayVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdReplay * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdReplay * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdReplay * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdReplay * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdReplay * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdReplay * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdReplay * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IScdReplayVtbl;

    interface IScdReplay
    {
        CONST_VTBL struct IScdReplayVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdReplay_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdReplay_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdReplay_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdReplay_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdReplay_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdReplay_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdReplay_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IScdReplay_INTERFACE_DEFINED__ */


#ifndef __IScdReport_INTERFACE_DEFINED__
#define __IScdReport_INTERFACE_DEFINED__

/* interface IScdReport */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdReport;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("9E5DEDD9-2F39-46C0-9F0B-E643DEAB7E41")
    IScdReport : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IScdReportVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdReport * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdReport * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdReport * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdReport * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdReport * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdReport * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdReport * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IScdReportVtbl;

    interface IScdReport
    {
        CONST_VTBL struct IScdReportVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdReport_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdReport_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdReport_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdReport_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdReport_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdReport_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdReport_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IScdReport_INTERFACE_DEFINED__ */


#ifndef __IScdReports_INTERFACE_DEFINED__
#define __IScdReports_INTERFACE_DEFINED__

/* interface IScdReports */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdReports;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("9BBB6577-1912-4A57-B3A3-695D2E1B9C47")
    IScdReports : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Report( 
            /* [in] */ VARIANT WhichOne,
            /* [retval][out] */ IScdReport **pItem) = 0;
        
        virtual /* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ LPUNKNOWN *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ BSTR IDStr,
            /* [retval][out] */ IScdReport **pItem) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Remove( 
            /* [in] */ VARIANT WhichOne) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Process( 
            /* [in] */ eScdReportTypes Type,
            /* [in] */ BSTR Filename,
            /* [in] */ BSTR Reportname,
            /* [defaultvalue][in] */ VARIANT_BOOL SaveAndClose = TRUE) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScdReportsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdReports * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdReports * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdReports * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdReports * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdReports * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdReports * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdReports * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Report )( 
            IScdReports * This,
            /* [in] */ VARIANT WhichOne,
            /* [retval][out] */ IScdReport **pItem);
        
        /* [id][restricted][propget] */ HRESULT ( STDMETHODCALLTYPE *get__NewEnum )( 
            IScdReports * This,
            /* [retval][out] */ LPUNKNOWN *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            IScdReports * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Add )( 
            IScdReports * This,
            /* [in] */ BSTR IDStr,
            /* [retval][out] */ IScdReport **pItem);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Remove )( 
            IScdReports * This,
            /* [in] */ VARIANT WhichOne);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Process )( 
            IScdReports * This,
            /* [in] */ eScdReportTypes Type,
            /* [in] */ BSTR Filename,
            /* [in] */ BSTR Reportname,
            /* [defaultvalue][in] */ VARIANT_BOOL SaveAndClose);
        
        END_INTERFACE
    } IScdReportsVtbl;

    interface IScdReports
    {
        CONST_VTBL struct IScdReportsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdReports_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdReports_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdReports_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdReports_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdReports_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdReports_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdReports_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IScdReports_Report(This,WhichOne,pItem)	\
    (This)->lpVtbl -> Report(This,WhichOne,pItem)

#define IScdReports_get__NewEnum(This,pVal)	\
    (This)->lpVtbl -> get__NewEnum(This,pVal)

#define IScdReports_get_Count(This,pVal)	\
    (This)->lpVtbl -> get_Count(This,pVal)

#define IScdReports_Add(This,IDStr,pItem)	\
    (This)->lpVtbl -> Add(This,IDStr,pItem)

#define IScdReports_Remove(This,WhichOne)	\
    (This)->lpVtbl -> Remove(This,WhichOne)

#define IScdReports_Process(This,Type,Filename,Reportname,SaveAndClose)	\
    (This)->lpVtbl -> Process(This,Type,Filename,Reportname,SaveAndClose)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdReports_Report_Proxy( 
    IScdReports * This,
    /* [in] */ VARIANT WhichOne,
    /* [retval][out] */ IScdReport **pItem);


void __RPC_STUB IScdReports_Report_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE IScdReports_get__NewEnum_Proxy( 
    IScdReports * This,
    /* [retval][out] */ LPUNKNOWN *pVal);


void __RPC_STUB IScdReports_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdReports_get_Count_Proxy( 
    IScdReports * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IScdReports_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdReports_Add_Proxy( 
    IScdReports * This,
    /* [in] */ BSTR IDStr,
    /* [retval][out] */ IScdReport **pItem);


void __RPC_STUB IScdReports_Add_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdReports_Remove_Proxy( 
    IScdReports * This,
    /* [in] */ VARIANT WhichOne);


void __RPC_STUB IScdReports_Remove_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdReports_Process_Proxy( 
    IScdReports * This,
    /* [in] */ eScdReportTypes Type,
    /* [in] */ BSTR Filename,
    /* [in] */ BSTR Reportname,
    /* [defaultvalue][in] */ VARIANT_BOOL SaveAndClose);


void __RPC_STUB IScdReports_Process_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScdReports_INTERFACE_DEFINED__ */


#ifndef __IScdSnapshot_INTERFACE_DEFINED__
#define __IScdSnapshot_INTERFACE_DEFINED__

/* interface IScdSnapshot */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdSnapshot;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("AD006B0A-7A2E-41FA-9E4A-A21CF142F95A")
    IScdSnapshot : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IScdSnapshotVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdSnapshot * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdSnapshot * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdSnapshot * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdSnapshot * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdSnapshot * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdSnapshot * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdSnapshot * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IScdSnapshotVtbl;

    interface IScdSnapshot
    {
        CONST_VTBL struct IScdSnapshotVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdSnapshot_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdSnapshot_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdSnapshot_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdSnapshot_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdSnapshot_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdSnapshot_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdSnapshot_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IScdSnapshot_INTERFACE_DEFINED__ */


#ifndef __IScdTrend_INTERFACE_DEFINED__
#define __IScdTrend_INTERFACE_DEFINED__

/* interface IScdTrend */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdTrend;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D7DD5FC3-96D0-40BA-BDAB-C5A4D30E6273")
    IScdTrend : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IScdTrendVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdTrend * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdTrend * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdTrend * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdTrend * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdTrend * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdTrend * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdTrend * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IScdTrendVtbl;

    interface IScdTrend
    {
        CONST_VTBL struct IScdTrendVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdTrend_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdTrend_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdTrend_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdTrend_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdTrend_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdTrend_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdTrend_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IScdTrend_INTERFACE_DEFINED__ */


#ifndef __IScdTrends_INTERFACE_DEFINED__
#define __IScdTrends_INTERFACE_DEFINED__

/* interface IScdTrends */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdTrends;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("AE13108F-98AA-41A6-9419-AF1B4330F406")
    IScdTrends : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IScdTrendsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdTrends * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdTrends * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdTrends * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdTrends * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdTrends * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdTrends * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdTrends * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IScdTrendsVtbl;

    interface IScdTrends
    {
        CONST_VTBL struct IScdTrendsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdTrends_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdTrends_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdTrends_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdTrends_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdTrends_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdTrends_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdTrends_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IScdTrends_INTERFACE_DEFINED__ */


#ifndef __IScdAppTag_INTERFACE_DEFINED__
#define __IScdAppTag_INTERFACE_DEFINED__

/* interface IScdAppTag */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdAppTag;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1CD2CBB0-B29A-4C87-A263-3A96C442B26C")
    IScdAppTag : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IScdAppTagVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdAppTag * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdAppTag * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdAppTag * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdAppTag * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdAppTag * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdAppTag * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdAppTag * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IScdAppTagVtbl;

    interface IScdAppTag
    {
        CONST_VTBL struct IScdAppTagVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdAppTag_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdAppTag_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdAppTag_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdAppTag_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdAppTag_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdAppTag_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdAppTag_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IScdAppTag_INTERFACE_DEFINED__ */


#ifndef __IScdAppTags_INTERFACE_DEFINED__
#define __IScdAppTags_INTERFACE_DEFINED__

/* interface IScdAppTags */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdAppTags;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("7DBCC5D8-BFE8-4989-9C78-BECB5EAA0119")
    IScdAppTags : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Tag( 
            /* [in] */ VARIANT WhichOne,
            /* [retval][out] */ IScdAppTag **pItem) = 0;
        
        virtual /* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ LPUNKNOWN *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ BSTR Tag) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Remove( 
            /* [in] */ VARIANT WhichOne) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ReturnCode( 
            /* [retval][out] */ eScdAppTagsCodes *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_TagValue( 
            /* [in] */ BSTR TheTag,
            /* [retval][out] */ VARIANT *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_TagValue( 
            /* [in] */ BSTR TheTag,
            /* [in] */ VARIANT newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_TagType( 
            /* [in] */ BSTR TheTag,
            /* [retval][out] */ eScdAppTagType *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_TagRecording( 
            /* [in] */ BSTR Tag,
            /* [retval][out] */ eScdTagRecState *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_TagRecording( 
            /* [in] */ BSTR Tag,
            /* [in] */ eScdTagRecState newVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScdAppTagsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdAppTags * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdAppTags * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdAppTags * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdAppTags * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdAppTags * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdAppTags * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdAppTags * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Tag )( 
            IScdAppTags * This,
            /* [in] */ VARIANT WhichOne,
            /* [retval][out] */ IScdAppTag **pItem);
        
        /* [id][restricted][propget] */ HRESULT ( STDMETHODCALLTYPE *get__NewEnum )( 
            IScdAppTags * This,
            /* [retval][out] */ LPUNKNOWN *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            IScdAppTags * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Add )( 
            IScdAppTags * This,
            /* [in] */ BSTR Tag);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Remove )( 
            IScdAppTags * This,
            /* [in] */ VARIANT WhichOne);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ReturnCode )( 
            IScdAppTags * This,
            /* [retval][out] */ eScdAppTagsCodes *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_TagValue )( 
            IScdAppTags * This,
            /* [in] */ BSTR TheTag,
            /* [retval][out] */ VARIANT *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_TagValue )( 
            IScdAppTags * This,
            /* [in] */ BSTR TheTag,
            /* [in] */ VARIANT newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_TagType )( 
            IScdAppTags * This,
            /* [in] */ BSTR TheTag,
            /* [retval][out] */ eScdAppTagType *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_TagRecording )( 
            IScdAppTags * This,
            /* [in] */ BSTR Tag,
            /* [retval][out] */ eScdTagRecState *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_TagRecording )( 
            IScdAppTags * This,
            /* [in] */ BSTR Tag,
            /* [in] */ eScdTagRecState newVal);
        
        END_INTERFACE
    } IScdAppTagsVtbl;

    interface IScdAppTags
    {
        CONST_VTBL struct IScdAppTagsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdAppTags_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdAppTags_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdAppTags_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdAppTags_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdAppTags_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdAppTags_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdAppTags_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IScdAppTags_Tag(This,WhichOne,pItem)	\
    (This)->lpVtbl -> Tag(This,WhichOne,pItem)

#define IScdAppTags_get__NewEnum(This,pVal)	\
    (This)->lpVtbl -> get__NewEnum(This,pVal)

#define IScdAppTags_get_Count(This,pVal)	\
    (This)->lpVtbl -> get_Count(This,pVal)

#define IScdAppTags_Add(This,Tag)	\
    (This)->lpVtbl -> Add(This,Tag)

#define IScdAppTags_Remove(This,WhichOne)	\
    (This)->lpVtbl -> Remove(This,WhichOne)

#define IScdAppTags_get_ReturnCode(This,pVal)	\
    (This)->lpVtbl -> get_ReturnCode(This,pVal)

#define IScdAppTags_get_TagValue(This,TheTag,pVal)	\
    (This)->lpVtbl -> get_TagValue(This,TheTag,pVal)

#define IScdAppTags_put_TagValue(This,TheTag,newVal)	\
    (This)->lpVtbl -> put_TagValue(This,TheTag,newVal)

#define IScdAppTags_get_TagType(This,TheTag,pVal)	\
    (This)->lpVtbl -> get_TagType(This,TheTag,pVal)

#define IScdAppTags_get_TagRecording(This,Tag,pVal)	\
    (This)->lpVtbl -> get_TagRecording(This,Tag,pVal)

#define IScdAppTags_put_TagRecording(This,Tag,newVal)	\
    (This)->lpVtbl -> put_TagRecording(This,Tag,newVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdAppTags_Tag_Proxy( 
    IScdAppTags * This,
    /* [in] */ VARIANT WhichOne,
    /* [retval][out] */ IScdAppTag **pItem);


void __RPC_STUB IScdAppTags_Tag_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE IScdAppTags_get__NewEnum_Proxy( 
    IScdAppTags * This,
    /* [retval][out] */ LPUNKNOWN *pVal);


void __RPC_STUB IScdAppTags_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdAppTags_get_Count_Proxy( 
    IScdAppTags * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IScdAppTags_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdAppTags_Add_Proxy( 
    IScdAppTags * This,
    /* [in] */ BSTR Tag);


void __RPC_STUB IScdAppTags_Add_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IScdAppTags_Remove_Proxy( 
    IScdAppTags * This,
    /* [in] */ VARIANT WhichOne);


void __RPC_STUB IScdAppTags_Remove_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdAppTags_get_ReturnCode_Proxy( 
    IScdAppTags * This,
    /* [retval][out] */ eScdAppTagsCodes *pVal);


void __RPC_STUB IScdAppTags_get_ReturnCode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdAppTags_get_TagValue_Proxy( 
    IScdAppTags * This,
    /* [in] */ BSTR TheTag,
    /* [retval][out] */ VARIANT *pVal);


void __RPC_STUB IScdAppTags_get_TagValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IScdAppTags_put_TagValue_Proxy( 
    IScdAppTags * This,
    /* [in] */ BSTR TheTag,
    /* [in] */ VARIANT newVal);


void __RPC_STUB IScdAppTags_put_TagValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdAppTags_get_TagType_Proxy( 
    IScdAppTags * This,
    /* [in] */ BSTR TheTag,
    /* [retval][out] */ eScdAppTagType *pVal);


void __RPC_STUB IScdAppTags_get_TagType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdAppTags_get_TagRecording_Proxy( 
    IScdAppTags * This,
    /* [in] */ BSTR Tag,
    /* [retval][out] */ eScdTagRecState *pVal);


void __RPC_STUB IScdAppTags_get_TagRecording_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IScdAppTags_put_TagRecording_Proxy( 
    IScdAppTags * This,
    /* [in] */ BSTR Tag,
    /* [in] */ eScdTagRecState newVal);


void __RPC_STUB IScdAppTags_put_TagRecording_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScdAppTags_INTERFACE_DEFINED__ */


#ifndef __IScdOPCServer_INTERFACE_DEFINED__
#define __IScdOPCServer_INTERFACE_DEFINED__

/* interface IScdOPCServer */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdOPCServer;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6B25D976-06AD-4555-839B-FE4397C8EA87")
    IScdOPCServer : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_On( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_On( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScdOPCServerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdOPCServer * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdOPCServer * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdOPCServer * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdOPCServer * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdOPCServer * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdOPCServer * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdOPCServer * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_On )( 
            IScdOPCServer * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_On )( 
            IScdOPCServer * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        END_INTERFACE
    } IScdOPCServerVtbl;

    interface IScdOPCServer
    {
        CONST_VTBL struct IScdOPCServerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdOPCServer_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdOPCServer_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdOPCServer_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdOPCServer_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdOPCServer_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdOPCServer_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdOPCServer_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IScdOPCServer_get_On(This,pVal)	\
    (This)->lpVtbl -> get_On(This,pVal)

#define IScdOPCServer_put_On(This,newVal)	\
    (This)->lpVtbl -> put_On(This,newVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdOPCServer_get_On_Proxy( 
    IScdOPCServer * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB IScdOPCServer_get_On_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IScdOPCServer_put_On_Proxy( 
    IScdOPCServer * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB IScdOPCServer_put_On_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScdOPCServer_INTERFACE_DEFINED__ */


#ifndef __IScdDDEServer_INTERFACE_DEFINED__
#define __IScdDDEServer_INTERFACE_DEFINED__

/* interface IScdDDEServer */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdDDEServer;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("F07CDDED-C83B-4102-9ABF-67E2CD538770")
    IScdDDEServer : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_On( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_On( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScdDDEServerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdDDEServer * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdDDEServer * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdDDEServer * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdDDEServer * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdDDEServer * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdDDEServer * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdDDEServer * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_On )( 
            IScdDDEServer * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_On )( 
            IScdDDEServer * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        END_INTERFACE
    } IScdDDEServerVtbl;

    interface IScdDDEServer
    {
        CONST_VTBL struct IScdDDEServerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdDDEServer_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdDDEServer_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdDDEServer_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdDDEServer_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdDDEServer_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdDDEServer_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdDDEServer_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IScdDDEServer_get_On(This,pVal)	\
    (This)->lpVtbl -> get_On(This,pVal)

#define IScdDDEServer_put_On(This,newVal)	\
    (This)->lpVtbl -> put_On(This,newVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdDDEServer_get_On_Proxy( 
    IScdDDEServer * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB IScdDDEServer_get_On_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IScdDDEServer_put_On_Proxy( 
    IScdDDEServer * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB IScdDDEServer_put_On_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScdDDEServer_INTERFACE_DEFINED__ */


#ifndef __IScdIOMarshal_INTERFACE_DEFINED__
#define __IScdIOMarshal_INTERFACE_DEFINED__

/* interface IScdIOMarshal */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdIOMarshal;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D0547ADB-FC63-409F-95C7-9A8883FE8980")
    IScdIOMarshal : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_On( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_On( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScdIOMarshalVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdIOMarshal * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdIOMarshal * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdIOMarshal * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdIOMarshal * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdIOMarshal * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdIOMarshal * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdIOMarshal * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_On )( 
            IScdIOMarshal * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_On )( 
            IScdIOMarshal * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        END_INTERFACE
    } IScdIOMarshalVtbl;

    interface IScdIOMarshal
    {
        CONST_VTBL struct IScdIOMarshalVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdIOMarshal_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdIOMarshal_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdIOMarshal_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdIOMarshal_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdIOMarshal_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdIOMarshal_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdIOMarshal_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IScdIOMarshal_get_On(This,pVal)	\
    (This)->lpVtbl -> get_On(This,pVal)

#define IScdIOMarshal_put_On(This,newVal)	\
    (This)->lpVtbl -> put_On(This,newVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdIOMarshal_get_On_Proxy( 
    IScdIOMarshal * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB IScdIOMarshal_get_On_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IScdIOMarshal_put_On_Proxy( 
    IScdIOMarshal * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB IScdIOMarshal_put_On_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScdIOMarshal_INTERFACE_DEFINED__ */


#ifndef __IScdTagFixup_INTERFACE_DEFINED__
#define __IScdTagFixup_INTERFACE_DEFINED__

/* interface IScdTagFixup */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdTagFixup;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("F50E3A4A-5F95-4B5D-9901-22D5522BFD58")
    IScdTagFixup : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Rule( 
            /* [retval][out] */ eScdTagFixupRules *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Rule( 
            /* [in] */ eScdTagFixupRules newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_String1( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_String1( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_String2( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_String2( 
            /* [in] */ BSTR newVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScdTagFixupVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdTagFixup * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdTagFixup * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdTagFixup * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdTagFixup * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdTagFixup * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdTagFixup * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdTagFixup * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Rule )( 
            IScdTagFixup * This,
            /* [retval][out] */ eScdTagFixupRules *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Rule )( 
            IScdTagFixup * This,
            /* [in] */ eScdTagFixupRules newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_String1 )( 
            IScdTagFixup * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_String1 )( 
            IScdTagFixup * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_String2 )( 
            IScdTagFixup * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_String2 )( 
            IScdTagFixup * This,
            /* [in] */ BSTR newVal);
        
        END_INTERFACE
    } IScdTagFixupVtbl;

    interface IScdTagFixup
    {
        CONST_VTBL struct IScdTagFixupVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdTagFixup_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdTagFixup_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdTagFixup_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdTagFixup_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdTagFixup_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdTagFixup_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdTagFixup_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IScdTagFixup_get_Rule(This,pVal)	\
    (This)->lpVtbl -> get_Rule(This,pVal)

#define IScdTagFixup_put_Rule(This,newVal)	\
    (This)->lpVtbl -> put_Rule(This,newVal)

#define IScdTagFixup_get_String1(This,pVal)	\
    (This)->lpVtbl -> get_String1(This,pVal)

#define IScdTagFixup_put_String1(This,newVal)	\
    (This)->lpVtbl -> put_String1(This,newVal)

#define IScdTagFixup_get_String2(This,pVal)	\
    (This)->lpVtbl -> get_String2(This,pVal)

#define IScdTagFixup_put_String2(This,newVal)	\
    (This)->lpVtbl -> put_String2(This,newVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdTagFixup_get_Rule_Proxy( 
    IScdTagFixup * This,
    /* [retval][out] */ eScdTagFixupRules *pVal);


void __RPC_STUB IScdTagFixup_get_Rule_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IScdTagFixup_put_Rule_Proxy( 
    IScdTagFixup * This,
    /* [in] */ eScdTagFixupRules newVal);


void __RPC_STUB IScdTagFixup_put_Rule_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdTagFixup_get_String1_Proxy( 
    IScdTagFixup * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IScdTagFixup_get_String1_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IScdTagFixup_put_String1_Proxy( 
    IScdTagFixup * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IScdTagFixup_put_String1_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IScdTagFixup_get_String2_Proxy( 
    IScdTagFixup * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IScdTagFixup_get_String2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IScdTagFixup_put_String2_Proxy( 
    IScdTagFixup * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IScdTagFixup_put_String2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScdTagFixup_INTERFACE_DEFINED__ */


#ifndef ___IScdApplicationEvents_DISPINTERFACE_DEFINED__
#define ___IScdApplicationEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IScdApplicationEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IScdApplicationEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("6564604D-8169-4528-9E50-43E3513E76C3")
    _IScdApplicationEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IScdApplicationEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IScdApplicationEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IScdApplicationEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IScdApplicationEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _IScdApplicationEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _IScdApplicationEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _IScdApplicationEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _IScdApplicationEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _IScdApplicationEventsVtbl;

    interface _IScdApplicationEvents
    {
        CONST_VTBL struct _IScdApplicationEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IScdApplicationEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _IScdApplicationEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _IScdApplicationEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _IScdApplicationEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _IScdApplicationEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _IScdApplicationEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _IScdApplicationEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IScdApplicationEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_ScdApplication;

#ifdef __cplusplus

class DECLSPEC_UUID("82B5AE15-C7F2-455A-886F-E0AB235A756D")
ScdApplication;
#endif

#ifndef ___IScdGraphicsEvents_DISPINTERFACE_DEFINED__
#define ___IScdGraphicsEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IScdGraphicsEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IScdGraphicsEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("4B358262-C451-41F6-AF27-19468F7BDEEE")
    _IScdGraphicsEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IScdGraphicsEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IScdGraphicsEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IScdGraphicsEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IScdGraphicsEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _IScdGraphicsEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _IScdGraphicsEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _IScdGraphicsEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _IScdGraphicsEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _IScdGraphicsEventsVtbl;

    interface _IScdGraphicsEvents
    {
        CONST_VTBL struct _IScdGraphicsEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IScdGraphicsEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _IScdGraphicsEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _IScdGraphicsEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _IScdGraphicsEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _IScdGraphicsEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _IScdGraphicsEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _IScdGraphicsEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IScdGraphicsEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_ScdGraphics;

#ifdef __cplusplus

class DECLSPEC_UUID("89FEEA68-73FC-4C68-AA13-BAE135290B8E")
ScdGraphics;
#endif

#ifndef ___IScdGraphicEvents_DISPINTERFACE_DEFINED__
#define ___IScdGraphicEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IScdGraphicEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IScdGraphicEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("7CBB5553-BBF6-4339-975A-047EF8F01578")
    _IScdGraphicEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IScdGraphicEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IScdGraphicEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IScdGraphicEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IScdGraphicEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _IScdGraphicEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _IScdGraphicEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _IScdGraphicEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _IScdGraphicEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _IScdGraphicEventsVtbl;

    interface _IScdGraphicEvents
    {
        CONST_VTBL struct _IScdGraphicEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IScdGraphicEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _IScdGraphicEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _IScdGraphicEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _IScdGraphicEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _IScdGraphicEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _IScdGraphicEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _IScdGraphicEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IScdGraphicEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_ScdGraphic;

#ifdef __cplusplus

class DECLSPEC_UUID("77DD141B-BE74-4E97-B13C-12C186E6E191")
ScdGraphic;
#endif

#ifndef ___IScdTrendsEvents_DISPINTERFACE_DEFINED__
#define ___IScdTrendsEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IScdTrendsEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IScdTrendsEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("EB3851B3-5C67-49ED-91DA-28A7C5774211")
    _IScdTrendsEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IScdTrendsEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IScdTrendsEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IScdTrendsEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IScdTrendsEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _IScdTrendsEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _IScdTrendsEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _IScdTrendsEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _IScdTrendsEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _IScdTrendsEventsVtbl;

    interface _IScdTrendsEvents
    {
        CONST_VTBL struct _IScdTrendsEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IScdTrendsEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _IScdTrendsEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _IScdTrendsEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _IScdTrendsEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _IScdTrendsEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _IScdTrendsEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _IScdTrendsEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IScdTrendsEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_ScdTrends;

#ifdef __cplusplus

class DECLSPEC_UUID("70AAA9DF-7546-4E4C-8484-6A7BBD2AF87B")
ScdTrends;
#endif

#ifndef ___IScdTrendEvents_DISPINTERFACE_DEFINED__
#define ___IScdTrendEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IScdTrendEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IScdTrendEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("BCB87010-3989-4B3A-ABD9-D90B612B1532")
    _IScdTrendEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IScdTrendEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IScdTrendEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IScdTrendEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IScdTrendEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _IScdTrendEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _IScdTrendEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _IScdTrendEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _IScdTrendEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _IScdTrendEventsVtbl;

    interface _IScdTrendEvents
    {
        CONST_VTBL struct _IScdTrendEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IScdTrendEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _IScdTrendEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _IScdTrendEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _IScdTrendEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _IScdTrendEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _IScdTrendEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _IScdTrendEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IScdTrendEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_ScdTrend;

#ifdef __cplusplus

class DECLSPEC_UUID("7FBB4DF3-4500-491C-BFB6-49B567E42146")
ScdTrend;
#endif

#ifndef ___IScdReportsEvents_DISPINTERFACE_DEFINED__
#define ___IScdReportsEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IScdReportsEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IScdReportsEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("D67C9B32-1CFD-4DCF-93BA-66DA10E292FC")
    _IScdReportsEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IScdReportsEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IScdReportsEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IScdReportsEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IScdReportsEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _IScdReportsEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _IScdReportsEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _IScdReportsEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _IScdReportsEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _IScdReportsEventsVtbl;

    interface _IScdReportsEvents
    {
        CONST_VTBL struct _IScdReportsEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IScdReportsEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _IScdReportsEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _IScdReportsEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _IScdReportsEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _IScdReportsEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _IScdReportsEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _IScdReportsEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IScdReportsEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_ScdReports;

#ifdef __cplusplus

class DECLSPEC_UUID("4E37ADB8-51BA-4AB1-A813-E7EB88DB1F59")
ScdReports;
#endif

#ifndef ___IScdReportEvents_DISPINTERFACE_DEFINED__
#define ___IScdReportEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IScdReportEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IScdReportEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("3E4DB624-6241-4AA4-A705-6C3D938DB29D")
    _IScdReportEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IScdReportEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IScdReportEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IScdReportEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IScdReportEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _IScdReportEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _IScdReportEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _IScdReportEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _IScdReportEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _IScdReportEventsVtbl;

    interface _IScdReportEvents
    {
        CONST_VTBL struct _IScdReportEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IScdReportEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _IScdReportEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _IScdReportEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _IScdReportEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _IScdReportEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _IScdReportEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _IScdReportEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IScdReportEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_ScdReport;

#ifdef __cplusplus

class DECLSPEC_UUID("C0C764DD-7B4A-4D40-A6B5-BD461C6B25CD")
ScdReport;
#endif

#ifndef ___IScdSnapshotEvents_DISPINTERFACE_DEFINED__
#define ___IScdSnapshotEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IScdSnapshotEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IScdSnapshotEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("347D49D9-6D03-4CCA-8331-80771CA09F38")
    _IScdSnapshotEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IScdSnapshotEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IScdSnapshotEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IScdSnapshotEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IScdSnapshotEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _IScdSnapshotEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _IScdSnapshotEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _IScdSnapshotEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _IScdSnapshotEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _IScdSnapshotEventsVtbl;

    interface _IScdSnapshotEvents
    {
        CONST_VTBL struct _IScdSnapshotEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IScdSnapshotEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _IScdSnapshotEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _IScdSnapshotEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _IScdSnapshotEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _IScdSnapshotEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _IScdSnapshotEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _IScdSnapshotEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IScdSnapshotEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_ScdSnapshot;

#ifdef __cplusplus

class DECLSPEC_UUID("9125AC56-6485-4E8C-9AE8-DB7B71687139")
ScdSnapshot;
#endif

#ifndef ___IScdHistorianEvents_DISPINTERFACE_DEFINED__
#define ___IScdHistorianEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IScdHistorianEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IScdHistorianEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("1405D2A2-A891-4153-BE9F-26CBC58F5918")
    _IScdHistorianEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IScdHistorianEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IScdHistorianEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IScdHistorianEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IScdHistorianEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _IScdHistorianEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _IScdHistorianEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _IScdHistorianEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _IScdHistorianEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _IScdHistorianEventsVtbl;

    interface _IScdHistorianEvents
    {
        CONST_VTBL struct _IScdHistorianEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IScdHistorianEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _IScdHistorianEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _IScdHistorianEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _IScdHistorianEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _IScdHistorianEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _IScdHistorianEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _IScdHistorianEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IScdHistorianEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_ScdHistorian;

#ifdef __cplusplus

class DECLSPEC_UUID("DF7B7439-A22A-4FD9-AF21-502E884BDB13")
ScdHistorian;
#endif

#ifndef ___IScdReplayEvents_DISPINTERFACE_DEFINED__
#define ___IScdReplayEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IScdReplayEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IScdReplayEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("0B832283-B646-4982-922D-922E0BA8191B")
    _IScdReplayEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IScdReplayEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IScdReplayEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IScdReplayEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IScdReplayEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _IScdReplayEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _IScdReplayEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _IScdReplayEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _IScdReplayEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _IScdReplayEventsVtbl;

    interface _IScdReplayEvents
    {
        CONST_VTBL struct _IScdReplayEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IScdReplayEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _IScdReplayEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _IScdReplayEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _IScdReplayEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _IScdReplayEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _IScdReplayEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _IScdReplayEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IScdReplayEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_ScdReplay;

#ifdef __cplusplus

class DECLSPEC_UUID("6CEC1C73-B7BB-4766-B352-9B465D280598")
ScdReplay;
#endif

#ifndef ___IScdProjectEvents_DISPINTERFACE_DEFINED__
#define ___IScdProjectEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IScdProjectEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IScdProjectEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("FE1BD893-D00F-4071-99C9-EF4145712FE4")
    _IScdProjectEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IScdProjectEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IScdProjectEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IScdProjectEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IScdProjectEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _IScdProjectEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _IScdProjectEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _IScdProjectEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _IScdProjectEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _IScdProjectEventsVtbl;

    interface _IScdProjectEvents
    {
        CONST_VTBL struct _IScdProjectEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IScdProjectEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _IScdProjectEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _IScdProjectEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _IScdProjectEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _IScdProjectEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _IScdProjectEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _IScdProjectEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IScdProjectEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_ScdProject;

#ifdef __cplusplus

class DECLSPEC_UUID("7209213F-9142-42F8-B379-2BD483FD19A7")
ScdProject;
#endif

#ifndef ___IScdLicenseAppEvents_DISPINTERFACE_DEFINED__
#define ___IScdLicenseAppEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IScdLicenseAppEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IScdLicenseAppEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("F92493F6-A68C-47C7-8603-63EED1703E0E")
    _IScdLicenseAppEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IScdLicenseAppEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IScdLicenseAppEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IScdLicenseAppEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IScdLicenseAppEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _IScdLicenseAppEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _IScdLicenseAppEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _IScdLicenseAppEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _IScdLicenseAppEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _IScdLicenseAppEventsVtbl;

    interface _IScdLicenseAppEvents
    {
        CONST_VTBL struct _IScdLicenseAppEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IScdLicenseAppEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _IScdLicenseAppEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _IScdLicenseAppEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _IScdLicenseAppEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _IScdLicenseAppEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _IScdLicenseAppEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _IScdLicenseAppEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IScdLicenseAppEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_ScdLicenseApp;

#ifdef __cplusplus

class DECLSPEC_UUID("AEEF651D-000C-476D-8D8A-0EFCE598BE2D")
ScdLicenseApp;
#endif

#ifndef ___IScdOptionsEvents_DISPINTERFACE_DEFINED__
#define ___IScdOptionsEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IScdOptionsEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IScdOptionsEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("B3736DA8-B7D6-4BED-86E0-D9405B9D3BAC")
    _IScdOptionsEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IScdOptionsEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IScdOptionsEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IScdOptionsEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IScdOptionsEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _IScdOptionsEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _IScdOptionsEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _IScdOptionsEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _IScdOptionsEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _IScdOptionsEventsVtbl;

    interface _IScdOptionsEvents
    {
        CONST_VTBL struct _IScdOptionsEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IScdOptionsEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _IScdOptionsEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _IScdOptionsEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _IScdOptionsEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _IScdOptionsEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _IScdOptionsEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _IScdOptionsEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IScdOptionsEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_ScdOptions;

#ifdef __cplusplus

class DECLSPEC_UUID("48640704-A5F3-4810-92FE-5AD0D72B330D")
ScdOptions;
#endif

#ifndef ___IScdAppTagsEvents_DISPINTERFACE_DEFINED__
#define ___IScdAppTagsEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IScdAppTagsEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IScdAppTagsEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("40658753-59D6-43EC-86B3-3ACF66203249")
    _IScdAppTagsEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IScdAppTagsEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IScdAppTagsEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IScdAppTagsEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IScdAppTagsEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _IScdAppTagsEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _IScdAppTagsEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _IScdAppTagsEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _IScdAppTagsEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _IScdAppTagsEventsVtbl;

    interface _IScdAppTagsEvents
    {
        CONST_VTBL struct _IScdAppTagsEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IScdAppTagsEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _IScdAppTagsEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _IScdAppTagsEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _IScdAppTagsEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _IScdAppTagsEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _IScdAppTagsEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _IScdAppTagsEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IScdAppTagsEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_ScdAppTags;

#ifdef __cplusplus

class DECLSPEC_UUID("9FE87A55-F087-40DC-BD9A-0AB66855C6A5")
ScdAppTags;
#endif

#ifndef ___IScdAppTagEvents_DISPINTERFACE_DEFINED__
#define ___IScdAppTagEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IScdAppTagEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IScdAppTagEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("27E1F8F1-3A3B-4A47-BDD6-2F84184BE74F")
    _IScdAppTagEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IScdAppTagEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IScdAppTagEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IScdAppTagEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IScdAppTagEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _IScdAppTagEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _IScdAppTagEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _IScdAppTagEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _IScdAppTagEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _IScdAppTagEventsVtbl;

    interface _IScdAppTagEvents
    {
        CONST_VTBL struct _IScdAppTagEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IScdAppTagEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _IScdAppTagEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _IScdAppTagEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _IScdAppTagEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _IScdAppTagEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _IScdAppTagEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _IScdAppTagEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IScdAppTagEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_ScdAppTag;

#ifdef __cplusplus

class DECLSPEC_UUID("EDEB289E-6CA0-44FF-9BCC-E0A79C961999")
ScdAppTag;
#endif

#ifndef ___IScdOPCServerEvents_DISPINTERFACE_DEFINED__
#define ___IScdOPCServerEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IScdOPCServerEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IScdOPCServerEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("CF9D72BD-DE3A-4B93-8C04-77D6DE2D66CD")
    _IScdOPCServerEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IScdOPCServerEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IScdOPCServerEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IScdOPCServerEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IScdOPCServerEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _IScdOPCServerEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _IScdOPCServerEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _IScdOPCServerEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _IScdOPCServerEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _IScdOPCServerEventsVtbl;

    interface _IScdOPCServerEvents
    {
        CONST_VTBL struct _IScdOPCServerEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IScdOPCServerEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _IScdOPCServerEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _IScdOPCServerEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _IScdOPCServerEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _IScdOPCServerEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _IScdOPCServerEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _IScdOPCServerEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IScdOPCServerEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_ScdOPCServer;

#ifdef __cplusplus

class DECLSPEC_UUID("FB41D0A4-B3F8-495E-ACE7-98E6B5182A21")
ScdOPCServer;
#endif

#ifndef ___IScdDDEServerEvents_DISPINTERFACE_DEFINED__
#define ___IScdDDEServerEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IScdDDEServerEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IScdDDEServerEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("5F996E78-B0C7-44A9-B4E2-28DDBE2CDEE5")
    _IScdDDEServerEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IScdDDEServerEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IScdDDEServerEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IScdDDEServerEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IScdDDEServerEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _IScdDDEServerEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _IScdDDEServerEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _IScdDDEServerEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _IScdDDEServerEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _IScdDDEServerEventsVtbl;

    interface _IScdDDEServerEvents
    {
        CONST_VTBL struct _IScdDDEServerEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IScdDDEServerEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _IScdDDEServerEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _IScdDDEServerEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _IScdDDEServerEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _IScdDDEServerEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _IScdDDEServerEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _IScdDDEServerEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IScdDDEServerEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_ScdDDEServer;

#ifdef __cplusplus

class DECLSPEC_UUID("D958F87F-BA78-4ECE-880B-30C1DEA8823C")
ScdDDEServer;
#endif

#ifndef ___IScdIOMarshalEvents_DISPINTERFACE_DEFINED__
#define ___IScdIOMarshalEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IScdIOMarshalEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IScdIOMarshalEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("EFAA1F8F-87B5-4A25-882C-E649367CF554")
    _IScdIOMarshalEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IScdIOMarshalEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IScdIOMarshalEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IScdIOMarshalEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IScdIOMarshalEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _IScdIOMarshalEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _IScdIOMarshalEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _IScdIOMarshalEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _IScdIOMarshalEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _IScdIOMarshalEventsVtbl;

    interface _IScdIOMarshalEvents
    {
        CONST_VTBL struct _IScdIOMarshalEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IScdIOMarshalEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _IScdIOMarshalEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _IScdIOMarshalEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _IScdIOMarshalEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _IScdIOMarshalEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _IScdIOMarshalEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _IScdIOMarshalEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IScdIOMarshalEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_ScdIOMarshal;

#ifdef __cplusplus

class DECLSPEC_UUID("2E2D7DE0-A3A3-4EE4-A91D-38DD20631F5F")
ScdIOMarshal;
#endif

EXTERN_C const CLSID CLSID_ScdTagFixup;

#ifdef __cplusplus

class DECLSPEC_UUID("D664BC94-4AB5-4B3E-929A-90FADA43CAB5")
ScdTagFixup;
#endif
#endif /* __ScdApp_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


