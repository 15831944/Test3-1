

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0361 */
/* at Tue Mar 07 15:31:21 2006
 */
/* Compiler settings for .\ScdCOM.idl:
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

#ifndef __ScdCOM_h__
#define __ScdCOM_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __ISysCADCmd_FWD_DEFINED__
#define __ISysCADCmd_FWD_DEFINED__
typedef interface ISysCADCmd ISysCADCmd;
#endif 	/* __ISysCADCmd_FWD_DEFINED__ */


#ifndef __ISysCADEdit_FWD_DEFINED__
#define __ISysCADEdit_FWD_DEFINED__
typedef interface ISysCADEdit ISysCADEdit;
#endif 	/* __ISysCADEdit_FWD_DEFINED__ */


#ifndef __IBackdoor_FWD_DEFINED__
#define __IBackdoor_FWD_DEFINED__
typedef interface IBackdoor IBackdoor;
#endif 	/* __IBackdoor_FWD_DEFINED__ */


#ifndef ___ISysCADCmdEvents_FWD_DEFINED__
#define ___ISysCADCmdEvents_FWD_DEFINED__
typedef interface _ISysCADCmdEvents _ISysCADCmdEvents;
#endif 	/* ___ISysCADCmdEvents_FWD_DEFINED__ */


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
    
    MIDL_INTERFACE("F0A8E28B-C450-4F11-B96B-7AA62B445D36")
    ISysCADCmd : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_UpNAbout( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WaitUpNAbout( 
            double TimeOut) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SyncCallsOn( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_SyncCallsOn( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SyncCallsTimeOut( 
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_SyncCallsTimeOut( 
            /* [in] */ double newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LoadProject( 
            /* [in] */ BSTR bsProjectName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SaveProject( 
            /* [in] */ BSTR bsProjectName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LoadSnapshot( 
            /* [in] */ BSTR bsSnapshotName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SaveSnapshot( 
            /* [in] */ BSTR bsSnapshotName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LoadScenario( 
            /* [in] */ BSTR bsScenarioName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SaveScenario( 
            /* [in] */ BSTR bsScenarioName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CloseProject( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetTag( 
            /* [in] */ BSTR Tag,
            /* [in] */ VARIANT *Value) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetTag( 
            /* [in] */ BSTR Tag,
            /* [out] */ VARIANT *Value) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE StartDynamicSolve( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE StartProbalSolve( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Start( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Step( 
            /* [defaultvalue][in] */ long IterMark = 0,
            /* [defaultvalue][in] */ double WaitForNext = 0) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Stop( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Idle( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Wait( 
            /* [in] */ double Seconds) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WaitCount( 
            /* [in] */ long Iterations) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WaitTillSteadyState( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WaitTillStop( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ResetWait( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GenerateTagReport( 
            /* [in] */ BSTR Filename,
            /* [in] */ BSTR Reportname) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GenerateTrendReport( 
            /* [in] */ BSTR Filename,
            /* [in] */ BSTR Reportname) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ProcessSetTags( 
            /* [in] */ BSTR Filename,
            /* [in] */ BSTR Reportname) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SaveAndCloseReport( 
            /* [in] */ BSTR Filename) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ExecuteMacro( 
            /* [in] */ BSTR Filename,
            /* [in] */ BSTR Macroname) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_CmpFilename( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_CmpFilename( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_CmpSort( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_CmpSort( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_CmpMaxCount( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_CmpMaxCount( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_CmpRelativeTolerance( 
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_CmpRelativeTolerance( 
            /* [in] */ double newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_CmpAbsoluteTolerance( 
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_CmpAbsoluteTolerance( 
            /* [in] */ double newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_CmpShowDifferentStrings( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_CmpShowDifferentStrings( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_CmpShowMissingTags( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_CmpShowMissingTags( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CompareReset( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CompareScenarios( 
            /* [in] */ BSTR Filename1,
            /* [in] */ BSTR Filename2) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CompareScenarioToCurrent( 
            /* [in] */ BSTR Filename) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE TestScenario( 
            /* [in] */ BSTR Filename) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RestartHistorian( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Sleep( 
            double Seconds) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Exit( void) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_EventsOn( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_EventsOn( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
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
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_RealTimeOn( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_RealTimeOn( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_RealTimeMultiplier( 
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_RealTimeMultiplier( 
            /* [in] */ double newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SequenceNo( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_SequenceNo( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetAppWndState( 
            long ReqdState) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LoadBacktrack( 
            /* [in] */ BSTR bsBacktrackName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SaveBacktrack( 
            /* [in] */ BSTR bsBacktrackName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE VariableSetReplayItem( 
            /* [in] */ long SequenceNo,
            /* [in] */ BSTR Tag,
            /* [in] */ VARIANT Value,
            /* [in] */ BSTR CnvTxt) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE VariableSetReplayCtrl( 
            /* [in] */ long Command,
            /* [defaultvalue][in] */ long Param = 0) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_TagValue( 
            /* [in] */ BSTR Tag,
            /* [retval][out] */ VARIANT *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_TagValue( 
            /* [in] */ BSTR Tag,
            /* [in] */ VARIANT newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_TagType( 
            /* [in] */ BSTR Tag,
            /* [retval][out] */ long *pVal) = 0;
        
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
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_UpNAbout )( 
            ISysCADCmd * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WaitUpNAbout )( 
            ISysCADCmd * This,
            double TimeOut);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SyncCallsOn )( 
            ISysCADCmd * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_SyncCallsOn )( 
            ISysCADCmd * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SyncCallsTimeOut )( 
            ISysCADCmd * This,
            /* [retval][out] */ double *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_SyncCallsTimeOut )( 
            ISysCADCmd * This,
            /* [in] */ double newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LoadProject )( 
            ISysCADCmd * This,
            /* [in] */ BSTR bsProjectName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SaveProject )( 
            ISysCADCmd * This,
            /* [in] */ BSTR bsProjectName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LoadSnapshot )( 
            ISysCADCmd * This,
            /* [in] */ BSTR bsSnapshotName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SaveSnapshot )( 
            ISysCADCmd * This,
            /* [in] */ BSTR bsSnapshotName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LoadScenario )( 
            ISysCADCmd * This,
            /* [in] */ BSTR bsScenarioName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SaveScenario )( 
            ISysCADCmd * This,
            /* [in] */ BSTR bsScenarioName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CloseProject )( 
            ISysCADCmd * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetTag )( 
            ISysCADCmd * This,
            /* [in] */ BSTR Tag,
            /* [in] */ VARIANT *Value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetTag )( 
            ISysCADCmd * This,
            /* [in] */ BSTR Tag,
            /* [out] */ VARIANT *Value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *StartDynamicSolve )( 
            ISysCADCmd * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *StartProbalSolve )( 
            ISysCADCmd * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Start )( 
            ISysCADCmd * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Step )( 
            ISysCADCmd * This,
            /* [defaultvalue][in] */ long IterMark,
            /* [defaultvalue][in] */ double WaitForNext);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Stop )( 
            ISysCADCmd * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Idle )( 
            ISysCADCmd * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Wait )( 
            ISysCADCmd * This,
            /* [in] */ double Seconds);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WaitCount )( 
            ISysCADCmd * This,
            /* [in] */ long Iterations);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WaitTillSteadyState )( 
            ISysCADCmd * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WaitTillStop )( 
            ISysCADCmd * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ResetWait )( 
            ISysCADCmd * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GenerateTagReport )( 
            ISysCADCmd * This,
            /* [in] */ BSTR Filename,
            /* [in] */ BSTR Reportname);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GenerateTrendReport )( 
            ISysCADCmd * This,
            /* [in] */ BSTR Filename,
            /* [in] */ BSTR Reportname);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ProcessSetTags )( 
            ISysCADCmd * This,
            /* [in] */ BSTR Filename,
            /* [in] */ BSTR Reportname);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SaveAndCloseReport )( 
            ISysCADCmd * This,
            /* [in] */ BSTR Filename);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ExecuteMacro )( 
            ISysCADCmd * This,
            /* [in] */ BSTR Filename,
            /* [in] */ BSTR Macroname);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_CmpFilename )( 
            ISysCADCmd * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_CmpFilename )( 
            ISysCADCmd * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_CmpSort )( 
            ISysCADCmd * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_CmpSort )( 
            ISysCADCmd * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_CmpMaxCount )( 
            ISysCADCmd * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_CmpMaxCount )( 
            ISysCADCmd * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_CmpRelativeTolerance )( 
            ISysCADCmd * This,
            /* [retval][out] */ double *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_CmpRelativeTolerance )( 
            ISysCADCmd * This,
            /* [in] */ double newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_CmpAbsoluteTolerance )( 
            ISysCADCmd * This,
            /* [retval][out] */ double *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_CmpAbsoluteTolerance )( 
            ISysCADCmd * This,
            /* [in] */ double newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_CmpShowDifferentStrings )( 
            ISysCADCmd * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_CmpShowDifferentStrings )( 
            ISysCADCmd * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_CmpShowMissingTags )( 
            ISysCADCmd * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_CmpShowMissingTags )( 
            ISysCADCmd * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CompareReset )( 
            ISysCADCmd * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CompareScenarios )( 
            ISysCADCmd * This,
            /* [in] */ BSTR Filename1,
            /* [in] */ BSTR Filename2);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CompareScenarioToCurrent )( 
            ISysCADCmd * This,
            /* [in] */ BSTR Filename);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *TestScenario )( 
            ISysCADCmd * This,
            /* [in] */ BSTR Filename);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RestartHistorian )( 
            ISysCADCmd * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Sleep )( 
            ISysCADCmd * This,
            double Seconds);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Exit )( 
            ISysCADCmd * This);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_EventsOn )( 
            ISysCADCmd * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_EventsOn )( 
            ISysCADCmd * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsStopped )( 
            ISysCADCmd * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsIdling )( 
            ISysCADCmd * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsRunning )( 
            ISysCADCmd * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_StepSize )( 
            ISysCADCmd * This,
            /* [retval][out] */ double *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_StepSize )( 
            ISysCADCmd * This,
            /* [in] */ double newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_RealTimeOn )( 
            ISysCADCmd * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_RealTimeOn )( 
            ISysCADCmd * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_RealTimeMultiplier )( 
            ISysCADCmd * This,
            /* [retval][out] */ double *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_RealTimeMultiplier )( 
            ISysCADCmd * This,
            /* [in] */ double newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SequenceNo )( 
            ISysCADCmd * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_SequenceNo )( 
            ISysCADCmd * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetAppWndState )( 
            ISysCADCmd * This,
            long ReqdState);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LoadBacktrack )( 
            ISysCADCmd * This,
            /* [in] */ BSTR bsBacktrackName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SaveBacktrack )( 
            ISysCADCmd * This,
            /* [in] */ BSTR bsBacktrackName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *VariableSetReplayItem )( 
            ISysCADCmd * This,
            /* [in] */ long SequenceNo,
            /* [in] */ BSTR Tag,
            /* [in] */ VARIANT Value,
            /* [in] */ BSTR CnvTxt);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *VariableSetReplayCtrl )( 
            ISysCADCmd * This,
            /* [in] */ long Command,
            /* [defaultvalue][in] */ long Param);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_TagValue )( 
            ISysCADCmd * This,
            /* [in] */ BSTR Tag,
            /* [retval][out] */ VARIANT *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_TagValue )( 
            ISysCADCmd * This,
            /* [in] */ BSTR Tag,
            /* [in] */ VARIANT newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_TagType )( 
            ISysCADCmd * This,
            /* [in] */ BSTR Tag,
            /* [retval][out] */ long *pVal);
        
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


#define ISysCADCmd_get_UpNAbout(This,pVal)	\
    (This)->lpVtbl -> get_UpNAbout(This,pVal)

#define ISysCADCmd_WaitUpNAbout(This,TimeOut)	\
    (This)->lpVtbl -> WaitUpNAbout(This,TimeOut)

#define ISysCADCmd_get_SyncCallsOn(This,pVal)	\
    (This)->lpVtbl -> get_SyncCallsOn(This,pVal)

#define ISysCADCmd_put_SyncCallsOn(This,newVal)	\
    (This)->lpVtbl -> put_SyncCallsOn(This,newVal)

#define ISysCADCmd_get_SyncCallsTimeOut(This,pVal)	\
    (This)->lpVtbl -> get_SyncCallsTimeOut(This,pVal)

#define ISysCADCmd_put_SyncCallsTimeOut(This,newVal)	\
    (This)->lpVtbl -> put_SyncCallsTimeOut(This,newVal)

#define ISysCADCmd_LoadProject(This,bsProjectName)	\
    (This)->lpVtbl -> LoadProject(This,bsProjectName)

#define ISysCADCmd_SaveProject(This,bsProjectName)	\
    (This)->lpVtbl -> SaveProject(This,bsProjectName)

#define ISysCADCmd_LoadSnapshot(This,bsSnapshotName)	\
    (This)->lpVtbl -> LoadSnapshot(This,bsSnapshotName)

#define ISysCADCmd_SaveSnapshot(This,bsSnapshotName)	\
    (This)->lpVtbl -> SaveSnapshot(This,bsSnapshotName)

#define ISysCADCmd_LoadScenario(This,bsScenarioName)	\
    (This)->lpVtbl -> LoadScenario(This,bsScenarioName)

#define ISysCADCmd_SaveScenario(This,bsScenarioName)	\
    (This)->lpVtbl -> SaveScenario(This,bsScenarioName)

#define ISysCADCmd_CloseProject(This)	\
    (This)->lpVtbl -> CloseProject(This)

#define ISysCADCmd_SetTag(This,Tag,Value)	\
    (This)->lpVtbl -> SetTag(This,Tag,Value)

#define ISysCADCmd_GetTag(This,Tag,Value)	\
    (This)->lpVtbl -> GetTag(This,Tag,Value)

#define ISysCADCmd_StartDynamicSolve(This)	\
    (This)->lpVtbl -> StartDynamicSolve(This)

#define ISysCADCmd_StartProbalSolve(This)	\
    (This)->lpVtbl -> StartProbalSolve(This)

#define ISysCADCmd_Start(This)	\
    (This)->lpVtbl -> Start(This)

#define ISysCADCmd_Step(This,IterMark,WaitForNext)	\
    (This)->lpVtbl -> Step(This,IterMark,WaitForNext)

#define ISysCADCmd_Stop(This)	\
    (This)->lpVtbl -> Stop(This)

#define ISysCADCmd_Idle(This)	\
    (This)->lpVtbl -> Idle(This)

#define ISysCADCmd_Wait(This,Seconds)	\
    (This)->lpVtbl -> Wait(This,Seconds)

#define ISysCADCmd_WaitCount(This,Iterations)	\
    (This)->lpVtbl -> WaitCount(This,Iterations)

#define ISysCADCmd_WaitTillSteadyState(This)	\
    (This)->lpVtbl -> WaitTillSteadyState(This)

#define ISysCADCmd_WaitTillStop(This)	\
    (This)->lpVtbl -> WaitTillStop(This)

#define ISysCADCmd_ResetWait(This)	\
    (This)->lpVtbl -> ResetWait(This)

#define ISysCADCmd_GenerateTagReport(This,Filename,Reportname)	\
    (This)->lpVtbl -> GenerateTagReport(This,Filename,Reportname)

#define ISysCADCmd_GenerateTrendReport(This,Filename,Reportname)	\
    (This)->lpVtbl -> GenerateTrendReport(This,Filename,Reportname)

#define ISysCADCmd_ProcessSetTags(This,Filename,Reportname)	\
    (This)->lpVtbl -> ProcessSetTags(This,Filename,Reportname)

#define ISysCADCmd_SaveAndCloseReport(This,Filename)	\
    (This)->lpVtbl -> SaveAndCloseReport(This,Filename)

#define ISysCADCmd_ExecuteMacro(This,Filename,Macroname)	\
    (This)->lpVtbl -> ExecuteMacro(This,Filename,Macroname)

#define ISysCADCmd_get_CmpFilename(This,pVal)	\
    (This)->lpVtbl -> get_CmpFilename(This,pVal)

#define ISysCADCmd_put_CmpFilename(This,newVal)	\
    (This)->lpVtbl -> put_CmpFilename(This,newVal)

#define ISysCADCmd_get_CmpSort(This,pVal)	\
    (This)->lpVtbl -> get_CmpSort(This,pVal)

#define ISysCADCmd_put_CmpSort(This,newVal)	\
    (This)->lpVtbl -> put_CmpSort(This,newVal)

#define ISysCADCmd_get_CmpMaxCount(This,pVal)	\
    (This)->lpVtbl -> get_CmpMaxCount(This,pVal)

#define ISysCADCmd_put_CmpMaxCount(This,newVal)	\
    (This)->lpVtbl -> put_CmpMaxCount(This,newVal)

#define ISysCADCmd_get_CmpRelativeTolerance(This,pVal)	\
    (This)->lpVtbl -> get_CmpRelativeTolerance(This,pVal)

#define ISysCADCmd_put_CmpRelativeTolerance(This,newVal)	\
    (This)->lpVtbl -> put_CmpRelativeTolerance(This,newVal)

#define ISysCADCmd_get_CmpAbsoluteTolerance(This,pVal)	\
    (This)->lpVtbl -> get_CmpAbsoluteTolerance(This,pVal)

#define ISysCADCmd_put_CmpAbsoluteTolerance(This,newVal)	\
    (This)->lpVtbl -> put_CmpAbsoluteTolerance(This,newVal)

#define ISysCADCmd_get_CmpShowDifferentStrings(This,pVal)	\
    (This)->lpVtbl -> get_CmpShowDifferentStrings(This,pVal)

#define ISysCADCmd_put_CmpShowDifferentStrings(This,newVal)	\
    (This)->lpVtbl -> put_CmpShowDifferentStrings(This,newVal)

#define ISysCADCmd_get_CmpShowMissingTags(This,pVal)	\
    (This)->lpVtbl -> get_CmpShowMissingTags(This,pVal)

#define ISysCADCmd_put_CmpShowMissingTags(This,newVal)	\
    (This)->lpVtbl -> put_CmpShowMissingTags(This,newVal)

#define ISysCADCmd_CompareReset(This)	\
    (This)->lpVtbl -> CompareReset(This)

#define ISysCADCmd_CompareScenarios(This,Filename1,Filename2)	\
    (This)->lpVtbl -> CompareScenarios(This,Filename1,Filename2)

#define ISysCADCmd_CompareScenarioToCurrent(This,Filename)	\
    (This)->lpVtbl -> CompareScenarioToCurrent(This,Filename)

#define ISysCADCmd_TestScenario(This,Filename)	\
    (This)->lpVtbl -> TestScenario(This,Filename)

#define ISysCADCmd_RestartHistorian(This)	\
    (This)->lpVtbl -> RestartHistorian(This)

#define ISysCADCmd_Sleep(This,Seconds)	\
    (This)->lpVtbl -> Sleep(This,Seconds)

#define ISysCADCmd_Exit(This)	\
    (This)->lpVtbl -> Exit(This)

#define ISysCADCmd_get_EventsOn(This,pVal)	\
    (This)->lpVtbl -> get_EventsOn(This,pVal)

#define ISysCADCmd_put_EventsOn(This,newVal)	\
    (This)->lpVtbl -> put_EventsOn(This,newVal)

#define ISysCADCmd_get_IsStopped(This,pVal)	\
    (This)->lpVtbl -> get_IsStopped(This,pVal)

#define ISysCADCmd_get_IsIdling(This,pVal)	\
    (This)->lpVtbl -> get_IsIdling(This,pVal)

#define ISysCADCmd_get_IsRunning(This,pVal)	\
    (This)->lpVtbl -> get_IsRunning(This,pVal)

#define ISysCADCmd_get_StepSize(This,pVal)	\
    (This)->lpVtbl -> get_StepSize(This,pVal)

#define ISysCADCmd_put_StepSize(This,newVal)	\
    (This)->lpVtbl -> put_StepSize(This,newVal)

#define ISysCADCmd_get_RealTimeOn(This,pVal)	\
    (This)->lpVtbl -> get_RealTimeOn(This,pVal)

#define ISysCADCmd_put_RealTimeOn(This,newVal)	\
    (This)->lpVtbl -> put_RealTimeOn(This,newVal)

#define ISysCADCmd_get_RealTimeMultiplier(This,pVal)	\
    (This)->lpVtbl -> get_RealTimeMultiplier(This,pVal)

#define ISysCADCmd_put_RealTimeMultiplier(This,newVal)	\
    (This)->lpVtbl -> put_RealTimeMultiplier(This,newVal)

#define ISysCADCmd_get_SequenceNo(This,pVal)	\
    (This)->lpVtbl -> get_SequenceNo(This,pVal)

#define ISysCADCmd_put_SequenceNo(This,newVal)	\
    (This)->lpVtbl -> put_SequenceNo(This,newVal)

#define ISysCADCmd_SetAppWndState(This,ReqdState)	\
    (This)->lpVtbl -> SetAppWndState(This,ReqdState)

#define ISysCADCmd_LoadBacktrack(This,bsBacktrackName)	\
    (This)->lpVtbl -> LoadBacktrack(This,bsBacktrackName)

#define ISysCADCmd_SaveBacktrack(This,bsBacktrackName)	\
    (This)->lpVtbl -> SaveBacktrack(This,bsBacktrackName)

#define ISysCADCmd_VariableSetReplayItem(This,SequenceNo,Tag,Value,CnvTxt)	\
    (This)->lpVtbl -> VariableSetReplayItem(This,SequenceNo,Tag,Value,CnvTxt)

#define ISysCADCmd_VariableSetReplayCtrl(This,Command,Param)	\
    (This)->lpVtbl -> VariableSetReplayCtrl(This,Command,Param)

#define ISysCADCmd_get_TagValue(This,Tag,pVal)	\
    (This)->lpVtbl -> get_TagValue(This,Tag,pVal)

#define ISysCADCmd_put_TagValue(This,Tag,newVal)	\
    (This)->lpVtbl -> put_TagValue(This,Tag,newVal)

#define ISysCADCmd_get_TagType(This,Tag,pVal)	\
    (This)->lpVtbl -> get_TagType(This,Tag,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_get_UpNAbout_Proxy( 
    ISysCADCmd * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB ISysCADCmd_get_UpNAbout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_WaitUpNAbout_Proxy( 
    ISysCADCmd * This,
    double TimeOut);


void __RPC_STUB ISysCADCmd_WaitUpNAbout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_get_SyncCallsOn_Proxy( 
    ISysCADCmd * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB ISysCADCmd_get_SyncCallsOn_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_put_SyncCallsOn_Proxy( 
    ISysCADCmd * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB ISysCADCmd_put_SyncCallsOn_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_get_SyncCallsTimeOut_Proxy( 
    ISysCADCmd * This,
    /* [retval][out] */ double *pVal);


void __RPC_STUB ISysCADCmd_get_SyncCallsTimeOut_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_put_SyncCallsTimeOut_Proxy( 
    ISysCADCmd * This,
    /* [in] */ double newVal);


void __RPC_STUB ISysCADCmd_put_SyncCallsTimeOut_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_LoadProject_Proxy( 
    ISysCADCmd * This,
    /* [in] */ BSTR bsProjectName);


void __RPC_STUB ISysCADCmd_LoadProject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_SaveProject_Proxy( 
    ISysCADCmd * This,
    /* [in] */ BSTR bsProjectName);


void __RPC_STUB ISysCADCmd_SaveProject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_LoadSnapshot_Proxy( 
    ISysCADCmd * This,
    /* [in] */ BSTR bsSnapshotName);


void __RPC_STUB ISysCADCmd_LoadSnapshot_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_SaveSnapshot_Proxy( 
    ISysCADCmd * This,
    /* [in] */ BSTR bsSnapshotName);


void __RPC_STUB ISysCADCmd_SaveSnapshot_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_LoadScenario_Proxy( 
    ISysCADCmd * This,
    /* [in] */ BSTR bsScenarioName);


void __RPC_STUB ISysCADCmd_LoadScenario_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_SaveScenario_Proxy( 
    ISysCADCmd * This,
    /* [in] */ BSTR bsScenarioName);


void __RPC_STUB ISysCADCmd_SaveScenario_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_CloseProject_Proxy( 
    ISysCADCmd * This);


void __RPC_STUB ISysCADCmd_CloseProject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_SetTag_Proxy( 
    ISysCADCmd * This,
    /* [in] */ BSTR Tag,
    /* [in] */ VARIANT *Value);


void __RPC_STUB ISysCADCmd_SetTag_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_GetTag_Proxy( 
    ISysCADCmd * This,
    /* [in] */ BSTR Tag,
    /* [out] */ VARIANT *Value);


void __RPC_STUB ISysCADCmd_GetTag_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_StartDynamicSolve_Proxy( 
    ISysCADCmd * This);


void __RPC_STUB ISysCADCmd_StartDynamicSolve_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_StartProbalSolve_Proxy( 
    ISysCADCmd * This);


void __RPC_STUB ISysCADCmd_StartProbalSolve_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_Start_Proxy( 
    ISysCADCmd * This);


void __RPC_STUB ISysCADCmd_Start_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_Step_Proxy( 
    ISysCADCmd * This,
    /* [defaultvalue][in] */ long IterMark,
    /* [defaultvalue][in] */ double WaitForNext);


void __RPC_STUB ISysCADCmd_Step_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_Stop_Proxy( 
    ISysCADCmd * This);


void __RPC_STUB ISysCADCmd_Stop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_Idle_Proxy( 
    ISysCADCmd * This);


void __RPC_STUB ISysCADCmd_Idle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_Wait_Proxy( 
    ISysCADCmd * This,
    /* [in] */ double Seconds);


void __RPC_STUB ISysCADCmd_Wait_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_WaitCount_Proxy( 
    ISysCADCmd * This,
    /* [in] */ long Iterations);


void __RPC_STUB ISysCADCmd_WaitCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_WaitTillSteadyState_Proxy( 
    ISysCADCmd * This);


void __RPC_STUB ISysCADCmd_WaitTillSteadyState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_WaitTillStop_Proxy( 
    ISysCADCmd * This);


void __RPC_STUB ISysCADCmd_WaitTillStop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_ResetWait_Proxy( 
    ISysCADCmd * This);


void __RPC_STUB ISysCADCmd_ResetWait_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_GenerateTagReport_Proxy( 
    ISysCADCmd * This,
    /* [in] */ BSTR Filename,
    /* [in] */ BSTR Reportname);


void __RPC_STUB ISysCADCmd_GenerateTagReport_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_GenerateTrendReport_Proxy( 
    ISysCADCmd * This,
    /* [in] */ BSTR Filename,
    /* [in] */ BSTR Reportname);


void __RPC_STUB ISysCADCmd_GenerateTrendReport_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_ProcessSetTags_Proxy( 
    ISysCADCmd * This,
    /* [in] */ BSTR Filename,
    /* [in] */ BSTR Reportname);


void __RPC_STUB ISysCADCmd_ProcessSetTags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_SaveAndCloseReport_Proxy( 
    ISysCADCmd * This,
    /* [in] */ BSTR Filename);


void __RPC_STUB ISysCADCmd_SaveAndCloseReport_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_ExecuteMacro_Proxy( 
    ISysCADCmd * This,
    /* [in] */ BSTR Filename,
    /* [in] */ BSTR Macroname);


void __RPC_STUB ISysCADCmd_ExecuteMacro_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_get_CmpFilename_Proxy( 
    ISysCADCmd * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB ISysCADCmd_get_CmpFilename_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_put_CmpFilename_Proxy( 
    ISysCADCmd * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB ISysCADCmd_put_CmpFilename_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_get_CmpSort_Proxy( 
    ISysCADCmd * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB ISysCADCmd_get_CmpSort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_put_CmpSort_Proxy( 
    ISysCADCmd * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB ISysCADCmd_put_CmpSort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_get_CmpMaxCount_Proxy( 
    ISysCADCmd * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB ISysCADCmd_get_CmpMaxCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_put_CmpMaxCount_Proxy( 
    ISysCADCmd * This,
    /* [in] */ long newVal);


void __RPC_STUB ISysCADCmd_put_CmpMaxCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_get_CmpRelativeTolerance_Proxy( 
    ISysCADCmd * This,
    /* [retval][out] */ double *pVal);


void __RPC_STUB ISysCADCmd_get_CmpRelativeTolerance_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_put_CmpRelativeTolerance_Proxy( 
    ISysCADCmd * This,
    /* [in] */ double newVal);


void __RPC_STUB ISysCADCmd_put_CmpRelativeTolerance_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_get_CmpAbsoluteTolerance_Proxy( 
    ISysCADCmd * This,
    /* [retval][out] */ double *pVal);


void __RPC_STUB ISysCADCmd_get_CmpAbsoluteTolerance_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_put_CmpAbsoluteTolerance_Proxy( 
    ISysCADCmd * This,
    /* [in] */ double newVal);


void __RPC_STUB ISysCADCmd_put_CmpAbsoluteTolerance_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_get_CmpShowDifferentStrings_Proxy( 
    ISysCADCmd * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB ISysCADCmd_get_CmpShowDifferentStrings_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_put_CmpShowDifferentStrings_Proxy( 
    ISysCADCmd * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB ISysCADCmd_put_CmpShowDifferentStrings_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_get_CmpShowMissingTags_Proxy( 
    ISysCADCmd * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB ISysCADCmd_get_CmpShowMissingTags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_put_CmpShowMissingTags_Proxy( 
    ISysCADCmd * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB ISysCADCmd_put_CmpShowMissingTags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_CompareReset_Proxy( 
    ISysCADCmd * This);


void __RPC_STUB ISysCADCmd_CompareReset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_CompareScenarios_Proxy( 
    ISysCADCmd * This,
    /* [in] */ BSTR Filename1,
    /* [in] */ BSTR Filename2);


void __RPC_STUB ISysCADCmd_CompareScenarios_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_CompareScenarioToCurrent_Proxy( 
    ISysCADCmd * This,
    /* [in] */ BSTR Filename);


void __RPC_STUB ISysCADCmd_CompareScenarioToCurrent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_TestScenario_Proxy( 
    ISysCADCmd * This,
    /* [in] */ BSTR Filename);


void __RPC_STUB ISysCADCmd_TestScenario_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_RestartHistorian_Proxy( 
    ISysCADCmd * This);


void __RPC_STUB ISysCADCmd_RestartHistorian_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_Sleep_Proxy( 
    ISysCADCmd * This,
    double Seconds);


void __RPC_STUB ISysCADCmd_Sleep_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_Exit_Proxy( 
    ISysCADCmd * This);


void __RPC_STUB ISysCADCmd_Exit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_get_EventsOn_Proxy( 
    ISysCADCmd * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB ISysCADCmd_get_EventsOn_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_put_EventsOn_Proxy( 
    ISysCADCmd * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB ISysCADCmd_put_EventsOn_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_get_IsStopped_Proxy( 
    ISysCADCmd * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB ISysCADCmd_get_IsStopped_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_get_IsIdling_Proxy( 
    ISysCADCmd * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB ISysCADCmd_get_IsIdling_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_get_IsRunning_Proxy( 
    ISysCADCmd * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB ISysCADCmd_get_IsRunning_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_get_StepSize_Proxy( 
    ISysCADCmd * This,
    /* [retval][out] */ double *pVal);


void __RPC_STUB ISysCADCmd_get_StepSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_put_StepSize_Proxy( 
    ISysCADCmd * This,
    /* [in] */ double newVal);


void __RPC_STUB ISysCADCmd_put_StepSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_get_RealTimeOn_Proxy( 
    ISysCADCmd * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB ISysCADCmd_get_RealTimeOn_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_put_RealTimeOn_Proxy( 
    ISysCADCmd * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB ISysCADCmd_put_RealTimeOn_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_get_RealTimeMultiplier_Proxy( 
    ISysCADCmd * This,
    /* [retval][out] */ double *pVal);


void __RPC_STUB ISysCADCmd_get_RealTimeMultiplier_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_put_RealTimeMultiplier_Proxy( 
    ISysCADCmd * This,
    /* [in] */ double newVal);


void __RPC_STUB ISysCADCmd_put_RealTimeMultiplier_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_get_SequenceNo_Proxy( 
    ISysCADCmd * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB ISysCADCmd_get_SequenceNo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_put_SequenceNo_Proxy( 
    ISysCADCmd * This,
    /* [in] */ long newVal);


void __RPC_STUB ISysCADCmd_put_SequenceNo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_SetAppWndState_Proxy( 
    ISysCADCmd * This,
    long ReqdState);


void __RPC_STUB ISysCADCmd_SetAppWndState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_LoadBacktrack_Proxy( 
    ISysCADCmd * This,
    /* [in] */ BSTR bsBacktrackName);


void __RPC_STUB ISysCADCmd_LoadBacktrack_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_SaveBacktrack_Proxy( 
    ISysCADCmd * This,
    /* [in] */ BSTR bsBacktrackName);


void __RPC_STUB ISysCADCmd_SaveBacktrack_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_VariableSetReplayItem_Proxy( 
    ISysCADCmd * This,
    /* [in] */ long SequenceNo,
    /* [in] */ BSTR Tag,
    /* [in] */ VARIANT Value,
    /* [in] */ BSTR CnvTxt);


void __RPC_STUB ISysCADCmd_VariableSetReplayItem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_VariableSetReplayCtrl_Proxy( 
    ISysCADCmd * This,
    /* [in] */ long Command,
    /* [defaultvalue][in] */ long Param);


void __RPC_STUB ISysCADCmd_VariableSetReplayCtrl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_get_TagValue_Proxy( 
    ISysCADCmd * This,
    /* [in] */ BSTR Tag,
    /* [retval][out] */ VARIANT *pVal);


void __RPC_STUB ISysCADCmd_get_TagValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_put_TagValue_Proxy( 
    ISysCADCmd * This,
    /* [in] */ BSTR Tag,
    /* [in] */ VARIANT newVal);


void __RPC_STUB ISysCADCmd_put_TagValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISysCADCmd_get_TagType_Proxy( 
    ISysCADCmd * This,
    /* [in] */ BSTR Tag,
    /* [retval][out] */ long *pVal);


void __RPC_STUB ISysCADCmd_get_TagType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISysCADCmd_INTERFACE_DEFINED__ */


#ifndef __ISysCADEdit_INTERFACE_DEFINED__
#define __ISysCADEdit_INTERFACE_DEFINED__

/* interface ISysCADEdit */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_ISysCADEdit;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("648529D8-8872-44e4-8A34-E59449D6832F")
    ISysCADEdit : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE InsertNode( 
            /* [in] */ BSTR Class,
            /* [in] */ BSTR Tag,
            /* [in] */ double X,
            /* [in] */ double Y,
            /* [in] */ double Z,
            /* [in] */ double XScale,
            /* [in] */ double YScale,
            /* [in] */ double Rotation) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE InsertLink( 
            /* [in] */ BSTR Class,
            /* [in] */ BSTR Tag,
            /* [in] */ BSTR SrcTag,
            /* [in] */ BSTR SrcIO,
            /* [in] */ BSTR DstTag,
            /* [in] */ BSTR DstIO,
            /* [in] */ double Xs,
            /* [in] */ double Ys,
            /* [in] */ double Zs,
            /* [in] */ double Xd,
            /* [in] */ double Yd,
            /* [in] */ double Zd) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_GrfWndIndex( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_GrfWndIndex( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ZoomWindowFull( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateUnit( 
            /* [in] */ BSTR Class,
            /* [in] */ BSTR Tag) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateUnitGrf( 
            /* [in] */ BSTR Tag,
            /* [in] */ double X,
            /* [in] */ double Y,
            /* [in] */ double Z,
            /* [in] */ BSTR Symbol,
            /* [in] */ double XScale,
            /* [in] */ double YScale,
            /* [in] */ double Rotation) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateLink( 
            /* [in] */ BSTR Class,
            /* [in] */ BSTR Tag,
            /* [in] */ BSTR SrcTag,
            /* [in] */ BSTR SrcIO,
            /* [in] */ BSTR DstTag,
            /* [in] */ BSTR DstIO) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateLinkGrf( 
            /* [in] */ BSTR Tag,
            /* [in] */ long DrawLineMethod) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateLinkGrfLine( 
            /* [in] */ BSTR Tag,
            /* [in] */ double Xs,
            /* [in] */ double Ys,
            /* [in] */ double Zs,
            /* [in] */ double Xd,
            /* [in] */ double Yd,
            /* [in] */ double Zd) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISysCADEditVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISysCADEdit * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISysCADEdit * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISysCADEdit * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISysCADEdit * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISysCADEdit * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISysCADEdit * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISysCADEdit * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *InsertNode )( 
            ISysCADEdit * This,
            /* [in] */ BSTR Class,
            /* [in] */ BSTR Tag,
            /* [in] */ double X,
            /* [in] */ double Y,
            /* [in] */ double Z,
            /* [in] */ double XScale,
            /* [in] */ double YScale,
            /* [in] */ double Rotation);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *InsertLink )( 
            ISysCADEdit * This,
            /* [in] */ BSTR Class,
            /* [in] */ BSTR Tag,
            /* [in] */ BSTR SrcTag,
            /* [in] */ BSTR SrcIO,
            /* [in] */ BSTR DstTag,
            /* [in] */ BSTR DstIO,
            /* [in] */ double Xs,
            /* [in] */ double Ys,
            /* [in] */ double Zs,
            /* [in] */ double Xd,
            /* [in] */ double Yd,
            /* [in] */ double Zd);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_GrfWndIndex )( 
            ISysCADEdit * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_GrfWndIndex )( 
            ISysCADEdit * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ZoomWindowFull )( 
            ISysCADEdit * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateUnit )( 
            ISysCADEdit * This,
            /* [in] */ BSTR Class,
            /* [in] */ BSTR Tag);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateUnitGrf )( 
            ISysCADEdit * This,
            /* [in] */ BSTR Tag,
            /* [in] */ double X,
            /* [in] */ double Y,
            /* [in] */ double Z,
            /* [in] */ BSTR Symbol,
            /* [in] */ double XScale,
            /* [in] */ double YScale,
            /* [in] */ double Rotation);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateLink )( 
            ISysCADEdit * This,
            /* [in] */ BSTR Class,
            /* [in] */ BSTR Tag,
            /* [in] */ BSTR SrcTag,
            /* [in] */ BSTR SrcIO,
            /* [in] */ BSTR DstTag,
            /* [in] */ BSTR DstIO);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateLinkGrf )( 
            ISysCADEdit * This,
            /* [in] */ BSTR Tag,
            /* [in] */ long DrawLineMethod);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateLinkGrfLine )( 
            ISysCADEdit * This,
            /* [in] */ BSTR Tag,
            /* [in] */ double Xs,
            /* [in] */ double Ys,
            /* [in] */ double Zs,
            /* [in] */ double Xd,
            /* [in] */ double Yd,
            /* [in] */ double Zd);
        
        END_INTERFACE
    } ISysCADEditVtbl;

    interface ISysCADEdit
    {
        CONST_VTBL struct ISysCADEditVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISysCADEdit_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISysCADEdit_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISysCADEdit_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISysCADEdit_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISysCADEdit_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISysCADEdit_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISysCADEdit_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISysCADEdit_InsertNode(This,Class,Tag,X,Y,Z,XScale,YScale,Rotation)	\
    (This)->lpVtbl -> InsertNode(This,Class,Tag,X,Y,Z,XScale,YScale,Rotation)

#define ISysCADEdit_InsertLink(This,Class,Tag,SrcTag,SrcIO,DstTag,DstIO,Xs,Ys,Zs,Xd,Yd,Zd)	\
    (This)->lpVtbl -> InsertLink(This,Class,Tag,SrcTag,SrcIO,DstTag,DstIO,Xs,Ys,Zs,Xd,Yd,Zd)

#define ISysCADEdit_get_GrfWndIndex(This,pVal)	\
    (This)->lpVtbl -> get_GrfWndIndex(This,pVal)

#define ISysCADEdit_put_GrfWndIndex(This,newVal)	\
    (This)->lpVtbl -> put_GrfWndIndex(This,newVal)

#define ISysCADEdit_ZoomWindowFull(This)	\
    (This)->lpVtbl -> ZoomWindowFull(This)

#define ISysCADEdit_CreateUnit(This,Class,Tag)	\
    (This)->lpVtbl -> CreateUnit(This,Class,Tag)

#define ISysCADEdit_CreateUnitGrf(This,Tag,X,Y,Z,Symbol,XScale,YScale,Rotation)	\
    (This)->lpVtbl -> CreateUnitGrf(This,Tag,X,Y,Z,Symbol,XScale,YScale,Rotation)

#define ISysCADEdit_CreateLink(This,Class,Tag,SrcTag,SrcIO,DstTag,DstIO)	\
    (This)->lpVtbl -> CreateLink(This,Class,Tag,SrcTag,SrcIO,DstTag,DstIO)

#define ISysCADEdit_CreateLinkGrf(This,Tag,DrawLineMethod)	\
    (This)->lpVtbl -> CreateLinkGrf(This,Tag,DrawLineMethod)

#define ISysCADEdit_CreateLinkGrfLine(This,Tag,Xs,Ys,Zs,Xd,Yd,Zd)	\
    (This)->lpVtbl -> CreateLinkGrfLine(This,Tag,Xs,Ys,Zs,Xd,Yd,Zd)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISysCADEdit_InsertNode_Proxy( 
    ISysCADEdit * This,
    /* [in] */ BSTR Class,
    /* [in] */ BSTR Tag,
    /* [in] */ double X,
    /* [in] */ double Y,
    /* [in] */ double Z,
    /* [in] */ double XScale,
    /* [in] */ double YScale,
    /* [in] */ double Rotation);


void __RPC_STUB ISysCADEdit_InsertNode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISysCADEdit_InsertLink_Proxy( 
    ISysCADEdit * This,
    /* [in] */ BSTR Class,
    /* [in] */ BSTR Tag,
    /* [in] */ BSTR SrcTag,
    /* [in] */ BSTR SrcIO,
    /* [in] */ BSTR DstTag,
    /* [in] */ BSTR DstIO,
    /* [in] */ double Xs,
    /* [in] */ double Ys,
    /* [in] */ double Zs,
    /* [in] */ double Xd,
    /* [in] */ double Yd,
    /* [in] */ double Zd);


void __RPC_STUB ISysCADEdit_InsertLink_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISysCADEdit_get_GrfWndIndex_Proxy( 
    ISysCADEdit * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB ISysCADEdit_get_GrfWndIndex_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISysCADEdit_put_GrfWndIndex_Proxy( 
    ISysCADEdit * This,
    /* [in] */ long newVal);


void __RPC_STUB ISysCADEdit_put_GrfWndIndex_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISysCADEdit_ZoomWindowFull_Proxy( 
    ISysCADEdit * This);


void __RPC_STUB ISysCADEdit_ZoomWindowFull_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISysCADEdit_CreateUnit_Proxy( 
    ISysCADEdit * This,
    /* [in] */ BSTR Class,
    /* [in] */ BSTR Tag);


void __RPC_STUB ISysCADEdit_CreateUnit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISysCADEdit_CreateUnitGrf_Proxy( 
    ISysCADEdit * This,
    /* [in] */ BSTR Tag,
    /* [in] */ double X,
    /* [in] */ double Y,
    /* [in] */ double Z,
    /* [in] */ BSTR Symbol,
    /* [in] */ double XScale,
    /* [in] */ double YScale,
    /* [in] */ double Rotation);


void __RPC_STUB ISysCADEdit_CreateUnitGrf_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISysCADEdit_CreateLink_Proxy( 
    ISysCADEdit * This,
    /* [in] */ BSTR Class,
    /* [in] */ BSTR Tag,
    /* [in] */ BSTR SrcTag,
    /* [in] */ BSTR SrcIO,
    /* [in] */ BSTR DstTag,
    /* [in] */ BSTR DstIO);


void __RPC_STUB ISysCADEdit_CreateLink_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISysCADEdit_CreateLinkGrf_Proxy( 
    ISysCADEdit * This,
    /* [in] */ BSTR Tag,
    /* [in] */ long DrawLineMethod);


void __RPC_STUB ISysCADEdit_CreateLinkGrf_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISysCADEdit_CreateLinkGrfLine_Proxy( 
    ISysCADEdit * This,
    /* [in] */ BSTR Tag,
    /* [in] */ double Xs,
    /* [in] */ double Ys,
    /* [in] */ double Zs,
    /* [in] */ double Xd,
    /* [in] */ double Yd,
    /* [in] */ double Zd);


void __RPC_STUB ISysCADEdit_CreateLinkGrfLine_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISysCADEdit_INTERFACE_DEFINED__ */


#ifndef __IBackdoor_INTERFACE_DEFINED__
#define __IBackdoor_INTERFACE_DEFINED__

/* interface IBackdoor */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IBackdoor;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("7837875f-63e2-4642-af7c-df1ba7ff10a4")
    IBackdoor : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Register( 
            /* [in] */ ULONG i_dwCastedCallback) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetResourceHInstance( 
            /* [ref][out] */ ULONG *hResInst) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetResourceCBRegID( 
            /* [ref][out] */ ULONG *ID) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IBackdoorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IBackdoor * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IBackdoor * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IBackdoor * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IBackdoor * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IBackdoor * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IBackdoor * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IBackdoor * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Register )( 
            IBackdoor * This,
            /* [in] */ ULONG i_dwCastedCallback);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetResourceHInstance )( 
            IBackdoor * This,
            /* [ref][out] */ ULONG *hResInst);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetResourceCBRegID )( 
            IBackdoor * This,
            /* [ref][out] */ ULONG *ID);
        
        END_INTERFACE
    } IBackdoorVtbl;

    interface IBackdoor
    {
        CONST_VTBL struct IBackdoorVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBackdoor_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IBackdoor_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IBackdoor_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IBackdoor_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IBackdoor_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IBackdoor_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IBackdoor_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IBackdoor_Register(This,i_dwCastedCallback)	\
    (This)->lpVtbl -> Register(This,i_dwCastedCallback)

#define IBackdoor_GetResourceHInstance(This,hResInst)	\
    (This)->lpVtbl -> GetResourceHInstance(This,hResInst)

#define IBackdoor_GetResourceCBRegID(This,ID)	\
    (This)->lpVtbl -> GetResourceCBRegID(This,ID)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IBackdoor_Register_Proxy( 
    IBackdoor * This,
    /* [in] */ ULONG i_dwCastedCallback);


void __RPC_STUB IBackdoor_Register_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IBackdoor_GetResourceHInstance_Proxy( 
    IBackdoor * This,
    /* [ref][out] */ ULONG *hResInst);


void __RPC_STUB IBackdoor_GetResourceHInstance_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IBackdoor_GetResourceCBRegID_Proxy( 
    IBackdoor * This,
    /* [ref][out] */ ULONG *ID);


void __RPC_STUB IBackdoor_GetResourceCBRegID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IBackdoor_INTERFACE_DEFINED__ */



#ifndef __SysCADLib_LIBRARY_DEFINED__
#define __SysCADLib_LIBRARY_DEFINED__

/* library SysCADLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_SysCADLib;

#ifndef ___ISysCADCmdEvents_DISPINTERFACE_DEFINED__
#define ___ISysCADCmdEvents_DISPINTERFACE_DEFINED__

/* dispinterface _ISysCADCmdEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__ISysCADCmdEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("43933726-54B7-4DCB-82B2-F4E630719423")
    _ISysCADCmdEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _ISysCADCmdEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _ISysCADCmdEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _ISysCADCmdEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _ISysCADCmdEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _ISysCADCmdEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _ISysCADCmdEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _ISysCADCmdEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _ISysCADCmdEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _ISysCADCmdEventsVtbl;

    interface _ISysCADCmdEvents
    {
        CONST_VTBL struct _ISysCADCmdEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _ISysCADCmdEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _ISysCADCmdEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _ISysCADCmdEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _ISysCADCmdEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _ISysCADCmdEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _ISysCADCmdEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _ISysCADCmdEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___ISysCADCmdEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_SysCADCmd;

#ifdef __cplusplus

class DECLSPEC_UUID("1E7E6A3F-9107-40E7-9616-65EEE2CCCE06")
SysCADCmd;
#endif
#endif /* __SysCADLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long *, unsigned long            , VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserMarshal(  unsigned long *, unsigned char *, VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserUnmarshal(unsigned long *, unsigned char *, VARIANT * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long *, VARIANT * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


