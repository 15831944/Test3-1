

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0361 */
/* at Mon Feb 27 10:04:01 2006
 */
/* Compiler settings for .\ScdIF.idl:
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


#ifndef __ScdIF_h__
#define __ScdIF_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IScdASyncEvents_FWD_DEFINED__
#define __IScdASyncEvents_FWD_DEFINED__
typedef interface IScdASyncEvents IScdASyncEvents;
#endif 	/* __IScdASyncEvents_FWD_DEFINED__ */


#ifndef __IScdTaggedObject_FWD_DEFINED__
#define __IScdTaggedObject_FWD_DEFINED__
typedef interface IScdTaggedObject IScdTaggedObject;
#endif 	/* __IScdTaggedObject_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

/* interface __MIDL_itf_ScdIF_0000 */
/* [local] */ 

typedef /* [v1_enum][helpstring][uuid] */  DECLSPEC_UUID("7178B88C-530E-4c75-8BB7-607E5C343C4F") 
enum eScdDirections
    {	eScdDirection_InOut	= 0x11,
	eScdDirection_Out	= 0x1,
	eScdDirection_Out0	= 0x3,
	eScdDirection_Out1	= 0x5,
	eScdDirection_Out2	= 0x9,
	eScdDirection_In	= 0x10,
	eScdDirection_In0	= 0x30,
	eScdDirection_In1	= 0x50,
	eScdDirection_In2	= 0x90,
	eScdDirection_In0Out	= eScdDirection_In0 | eScdDirection_Out,
	eScdDirection_InOut0	= eScdDirection_In | eScdDirection_Out0,
	eScdDirection_All	= 0xff
    } 	eScdDirections;

typedef /* [v1_enum][helpstring][uuid] */  DECLSPEC_UUID("DDFF69CF-8700-4130-B0EC-A5E109C708A6") 
enum eScdNodeTypes
    {	eScdNodeType_Process	= 0x10,
	eScdNodeType_MXfer	= 0x20,
	eScdNodeType_MSrcSnk	= 0x40,
	eScdNodeType_MLnk	= 0x80,
	eScdNodeType_MBst	= 0x100,
	eScdNodeType_Control	= 0x1000,
	eScdNodeType_CLnk	= 0x2000,
	eScdNodeType_Elec	= 0x10000,
	eScdNodeType_ELnk	= 0x20000,
	eScdNodeType_HLnk	= 0x40000,
	eScdNodeType_ALnk	= 0x80000,
	eScdNodeType_Cbl	= 0x100000,
	eScdNodeType_All	= 0xffffff0,
	eScdNodeType_Units	= eScdNodeType_Process | eScdNodeType_MXfer | eScdNodeType_MSrcSnk | eScdNodeType_MBst,
	eScdNodeType_Links	= eScdNodeType_MLnk | eScdNodeType_CLnk | eScdNodeType_ELnk | eScdNodeType_HLnk | eScdNodeType_ALnk | eScdNodeType_Cbl
    } 	eScdNodeTypes;

typedef /* [v1_enum][helpstring][uuid] */  DECLSPEC_UUID("C6EDC03C-968D-4e8b-9282-6B4998F78730") 
enum eScdTSTypes
    {	eScdElecType_Control	= 0x1,
	eScdElecType_Power	= 0x2,
	eScdElecType_All	= 0xffffffff
    } 	eScdElecTypes;

typedef /* [v1_enum][helpstring][uuid] */  DECLSPEC_UUID("CB7F8D4A-CF63-4d66-9C2A-4487F35D9BD9") 
enum eScdLinkTypes
    {	eScdLinkType_MLnk	= eScdNodeType_MLnk,
	eScdLinkType_CLnk	= eScdNodeType_CLnk,
	eScdLinkType_ELnk	= eScdNodeType_ELnk,
	eScdLinkType_HLnk	= eScdNodeType_HLnk,
	eScdLinkType_ALnk	= eScdNodeType_ALnk,
	eScdLinkType_Cbl	= eScdNodeType_Cbl,
	eScdLinkType_All	= eScdNodeType_All
    } 	eScdLinkTypes;

typedef /* [v1_enum][helpstring][uuid] */  DECLSPEC_UUID("15A56509-9915-4464-908C-20FD08E07B82") 
enum eScdMsg_Flags
    {	eScdMsg_NULL	= 0,
	eScdMsg_Condition	= 0x2,
	eScdMsg_Fatal	= 0x4,
	eScdMsg_Stop	= 0x8,
	eScdMsg_Error	= 0x10,
	eScdMsg_Warning	= 0x20,
	eScdMsg_Note	= 0x40,
	eScdMsg_Cmd	= 0x80,
	eScdMsg_Separator	= 0x100,
	eScdMsg_FromPGM	= 0x8000,
	eScdMsg_FromCOM	= 0x10000,
	eScdMsg_All	= 0xffffffff
    } 	eScdMsg_Flags;

typedef /* [v1_enum][helpstring][uuid] */  DECLSPEC_UUID("9AC9EDE2-D32F-4aff-8C76-98E00751A8A7") 
enum eScdMsg_Options
    {	eScdMsgOpt_NULL	= 0,
	eScdMsgOpt_COMCallsEntry	= 0x1,
	eScdMsgOpt_COMCallsExit	= 0x2,
	eScdMsgOpt_COMCalls	= 0x3,
	eScdMsgOpt_COMCallsInternal	= 0x4,
	eScdMsgOpt_All	= eScdMsgOpt_COMCalls,
	eScdMsgOpt_AllDebug	= eScdMsgOpt_COMCalls | eScdMsgOpt_COMCallsInternal
    } 	eScdMsg_Options;

typedef /* [v1_enum][helpstring][uuid] */  DECLSPEC_UUID("F640F6AB-548E-4540-9DEC-3C75D7A19F3C") 
enum eScdEvalSeq_Options
    {	eScdEvalSeq_Process	= 0x1,
	eScdEvalSeq_Control	= 0x2,
	eScdEvalSeq_FullDesc	= 0x4
    } 	eScdEvalSeq_Options;

typedef /* [v1_enum][helpstring][uuid] */  DECLSPEC_UUID("12928A51-4C0F-4ffe-817F-0C22F1C107AB") 
enum eScdEvalSeqIO_Flags
    {	eScdEvalSeqIOFlags_In	= 0x1,
	eScdEvalSeqIOFlags_Out	= 0x2,
	eScdEvalSeqIOFlags_Tear	= 0x4,
	eScdEvalSeqIOFlags_Owner	= 0x8
    } 	eScdEvalSeqIO_Flags;

typedef /* [v1_enum][helpstring][uuid] */  DECLSPEC_UUID("6639C444-6466-4646-B6B3-B5D14431A6AC") 
enum eScdEvalSeqIO_Class
    {	eScdEvalSeqIOClass_Process	= 0x10000,
	eScdEvalSeqIOClass_Control	= 0x20000,
	eScdEvalSeqIOClass_Electrical	= 0x40000,
	eScdEvalSeqIOClass_Air	= 0x80000,
	eScdEvalSeqIOClass_XRef	= 0x100000
    } 	eScdEvalSeqIO_Class;

typedef /* [v1_enum][helpstring][uuid] */  DECLSPEC_UUID("B1742116-60C6-40ff-BDBB-EC66FA4B6F7A") 
enum eScdSlvTagsCodes
    {	eScdSTC_OK	= 0
    } 	eScdSlvTagsCodes;

typedef /* [v1_enum][helpstring][uuid] */  DECLSPEC_UUID("48E53B23-E2AF-4c96-9B1F-F91A734F4F23") 
enum eScdSlvTagType
    {	eScdSTT_I2	= 0,
	eScdSTT_NullTag	= 0x100,
	eScdSTT_ZeroTag	= 0x200,
	eScdSTT_NoPrj	= 0x300
    } 	eScdSlvTagType;

typedef /* [v1_enum][helpstring][uuid] */  DECLSPEC_UUID("29134EE7-24B5-4d4e-9106-C2ECDA8FC679") 
enum eScdNodeCodes
    {	eScdUC_OK	= 0,
	eScdUC_TagExists	= 1,
	eScdUC_UnknownClass	= 2,
	eScdUC_BadLicense	= 3,
	eScdUC_Failed	= 4
    } 	eScdNodeCodes;

typedef /* [v1_enum][helpstring][uuid] */  DECLSPEC_UUID("DB31CF58-EB0E-4667-844D-5670FD2CFFFF") 
enum eScdTimeFormat
    {	eScdTimeFmt_Seconds	= 0,
	eScdTimeFmt_Secs1970	= 1,
	eScdTimeFmt_Date1900	= 2
    } 	eScdTimeFormat;

typedef /* [v1_enum][helpstring][uuid] */  DECLSPEC_UUID("6582BF6B-3C74-4303-A2A1-DC465EBD9B05") 
enum ComCmdReturns
    {	ComCmdRet_OK	= 0,
	ComCmdRet_InvalidTag	= 1,
	ComCmdRet_InvalidConversion	= 2,
	ComCmdRet_Fail	= 3
    } 	ComCmdReturns;

typedef /* [v1_enum][helpstring][uuid] */  DECLSPEC_UUID("FB7CFFCC-4032-412e-83AE-08E17F28E484") 
enum ePropertyTypes
    {	ePT_Null	= 0,
	ePT_Bool	= 1,
	ePT_Long	= 2,
	ePT_Double	= 3,
	ePT_String	= 4
    } 	ePropertyTypes;

typedef /* [v1_enum][helpstring][uuid] */  DECLSPEC_UUID("4758D29B-F0D8-47f2-8925-0751CD8B5451") 
enum ePropertyPageTypes
    {	ePPg_Null	= 0,
	ePPg_Optional	= 1,
	ePPg_Required	= 2
    } 	ePropertyPageTypes;

typedef /* [v1_enum][helpstring][uuid] */  DECLSPEC_UUID("E4BF2980-62F0-470c-A962-3B7F68F26936") 
enum ePropertyVisibility
    {	ePVis_Probal	= 0x1,
	ePVis_DynFlow	= 0x2,
	ePVis_DynFull	= 0x4,
	ePVis_View	= 0x10,
	ePVis_File	= 0x20,
	ePVis_Scenario	= 0x40,
	ePVis_Snapshot	= 0x80,
	ePVis_AllPB	= 0xf1,
	ePVis_AllDyn	= 0xf6,
	ePVis_All	= 0xf7
    } 	ePropertyVisibility;

typedef /* [v1_enum][helpstring][uuid] */  DECLSPEC_UUID("C1A42C61-3E8D-486e-B416-840431D32370") 
enum eScdGasMixingRule
    {	eScdGMR_Default	= 0,
	eScdGMR_Simple	= 1
    } 	eScdGasMixingRule;

typedef /* [v1_enum][helpstring][uuid] */  DECLSPEC_UUID("C51C2B41-E9E4-4e7f-9F34-34D8FFFF9A0E") 
enum eScdEqnOfState
    {	eScdEOS_Default	= 0,
	eScdEOS_SRK	= 1
    } 	eScdEqnOfState;

typedef /* [v1_enum][helpstring][uuid] */  DECLSPEC_UUID("98CADC27-0EB9-47fc-9120-2EF211A5F85C") 
enum eScdVLEOptions
    {	VLEF_Null	= 0,
	VLEF_VfFlash	= 0x1,
	VLEF_TPFlash	= 0x2,
	VLEF_QPFlash	= 0x4,
	VLEF_QVFlash	= 0x8,
	VLEF_SaturationLimit	= 0x10,
	VLEF_QMVapFlash	= 0x20,
	VLEF_WithDebug	= 0x10000000
    } 	eScdVLEOptions;

typedef /* [v1_enum][helpstring][uuid] */  DECLSPEC_UUID("1085B615-96FE-4475-AC73-FC40DEEA79C4") 
enum eScdMdlStateActs
    {	MSA_Null	= 0,
	MSA_ZeroFlows	= MSA_Null + 1,
	MSA_Empty	= MSA_ZeroFlows + 1,
	MSA_PreSet	= MSA_Empty + 1,
	MSA_SteadyState	= MSA_PreSet + 1,
	MSA_PBInit	= MSA_SteadyState + 1,
	MSA_DynStatsRunInit	= MSA_PBInit + 1,
	MSA_EmptySpillTargets	= MSA_DynStatsRunInit + 1
    } 	eScdMdlStateActs;

typedef /* [v1_enum][helpstring][uuid] */  DECLSPEC_UUID("25CDB7E1-D2E1-4b72-9218-97830D2255A3") 
enum eScdMacroMdlEvals
    {	MME_Flash2P	= 0,
	MME_Flash2Qv	= MME_Flash2P + 1,
	MME_Flash2QvSens	= MME_Flash2Qv + 1
    } 	eScdMacroMdlEvals;

typedef /* [v1_enum][helpstring][uuid] */  DECLSPEC_UUID("FBDE39A0-94B9-4681-9B52-47C68CC3C52B") 
enum eScdProbalPCtrls
    {	PBPC_Null	= 0,
	PBPC_Atmos	= 0x1,
	PBPC_MinFd	= 0x2,
	PBPC_MaxFd	= 0x4,
	PBPC_Reqd	= 0x8,
	PBPC_SatP	= 0x10,
	PBPC_ReqdSatT	= 0x20,
	PBPC_BubblePt	= 0x40,
	PBPC_DewPt	= 0x80,
	PBPC_SubCooled	= 0x100,
	PBPC_SuperHt	= 0x200,
	PBPC_Basic	= 0x1f,
	PBPC_All	= 0x3ff
    } 	eScdProbalPCtrls;

typedef /* [v1_enum][helpstring][uuid] */  DECLSPEC_UUID("E1FE860C-62C7-4cb5-B463-13F4E890ED9A") 
enum eScdTagFixupRules
    {	eFixup_NoChange	= 0,
	eFixup_Prefix	= 1,
	eFixup_Suffix	= 2,
	eFixup_PrefixSuffix	= 3,
	eFixup_RegExpression	= 4,
	eFixup_ExchangeTags	= 5,
	eFixup_ReplaceTag	= 6
    } 	eScdTagFixupRules;

typedef /* [v1_enum][helpstring][uuid] */  DECLSPEC_UUID("408F9E20-F82E-4076-AB47-53B03F87CCB0") 
enum eScdNDBOptions
    {	eNDB_BlankTables	= 0x1,
	eNDB_CommonTables	= 0x2,
	eNDB_Sequence	= 0x4,
	eNDB_Graphics	= 0x8,
	eNDB_Models	= 0x10,
	eNDB_Configuration	= 0x20,
	eNDB_Parameters	= 0x40,
	eNDB_State	= 0x80,
	eNDB_CommonNames	= 0x100,
	eNDB_GroupInserts	= 0x200,
	eNDB_StdExport	= eNDB_CommonTables | eNDB_Sequence | eNDB_Graphics | eNDB_Models | eNDB_Configuration,
	eNDB_StdImport	= eNDB_Sequence | eNDB_Graphics | eNDB_Models | eNDB_Configuration
    } 	eScdNDBOptions;

typedef /* [v1_enum][helpstring][uuid] */  DECLSPEC_UUID("AB6ECD2C-CC85-494a-8459-F4A8A4EBFC55") 
enum eScdBalanceValues
    {	eBV_JoinMassFlwIn	= 0,
	eBV_JoinMassGain	= eBV_JoinMassFlwIn + 1,
	eBV_JoinMassFlwOut	= eBV_JoinMassGain + 1,
	eBV_JoinFeedCpT	= eBV_JoinMassFlwOut + 1,
	eBV_JoinProdCpT	= eBV_JoinFeedCpT + 1,
	eBV_JoinHfIn	= eBV_JoinProdCpT + 1,
	eBV_JoinHfGain	= eBV_JoinHfIn + 1,
	eBV_JoinHsGain	= eBV_JoinHfGain + 1,
	eBV_JoinHfOut	= eBV_JoinHsGain + 1,
	eBV_JoinHfChg	= eBV_JoinHfOut + 1,
	eBV_JoinHsIn	= eBV_JoinHfChg + 1,
	eBV_JoinHsOut	= eBV_JoinHsIn + 1,
	eBV_JoinHsChg	= eBV_JoinHsOut + 1,
	eBV_JoinPowerIn	= eBV_JoinHsChg + 1,
	eBV_RCTMassGain	= eBV_JoinPowerIn + 1,
	eBV_RCTHfGain	= eBV_RCTMassGain + 1,
	eBV_RCTHsGain	= eBV_RCTHfGain + 1,
	eBV_RCTPowerIn	= eBV_RCTHsGain + 1,
	eBV_RCTHOR_Std0	= eBV_RCTPowerIn + 1,
	eBV_RCTHOR_Mdl0	= eBV_RCTHOR_Std0 + 1,
	eBV_RCTHOR_Used0	= eBV_RCTHOR_Mdl0 + 1,
	eBV_RCTHOR_Diff0	= eBV_RCTHOR_Used0 + 1,
	eBV_VLEHsGain	= eBV_RCTHOR_Diff0 + 1,
	eBV_EHXPowerIn	= eBV_VLEHsGain + 1
    } 	eScdBalanceValues;

typedef /* [public][v1_enum][helpstring][uuid] */  DECLSPEC_UUID("29D639B1-11C1-4482-91FB-80F02CC86BC9") 
enum eScdWndStateCmds
    {	eScdForeground	= 1,
	eScdNormalWnd	= 2,
	eScdMaximisedWnd	= 3,
	eScdMinimisedWnd	= 4,
	eScdActvateBtns	= 5,
	eScdDeactivateBtns	= 6
    } 	eScdWndStateCmds;

typedef /* [v1_enum][helpstring][uuid] */  DECLSPEC_UUID("4FFCEE9E-4C9E-4865-97D7-E5B56A7B509B") 
enum eScdGraphicsCodes
    {	eScdGC_OK	= 0
    } 	eScdGraphicsCodes;

typedef /* [v1_enum][helpstring][uuid] */  DECLSPEC_UUID("07FA8E2D-A0AD-4b62-85E5-E5D54CA7F517") 
enum eScdGraphicCodes
    {	eScdGraphicCode_OK	= 0,
	eScdGraphicCode_TagNotFound	= 1,
	eScdGraphicCode_GrfNotCreated	= 2
    } 	eScdGraphicCodes;

typedef /* [v1_enum][helpstring][uuid] */  DECLSPEC_UUID("BCCBC058-AD75-482e-B293-C8595F3E4C00") 
enum eScdZooms
    {	eScdZoom_Full	= 0
    } 	eScdZooms;

typedef /* [v1_enum][helpstring][uuid] */  DECLSPEC_UUID("8CEA74CD-AEFA-4cd3-936C-00B12E0B7992") 
enum eScdReportsCodes
    {	eScdRC_OK	= 0
    } 	eScdReportsCodes;

typedef /* [v1_enum][helpstring][uuid] */  DECLSPEC_UUID("FB366243-8BDB-4747-985E-34A394D72716") 
enum eScdReportTypes
    {	eScdRT_GetValues	= 0,
	eScdRT_SetValues	= 1,
	eScdRT_Trend	= 2
    } 	eScdReportTypes;

typedef /* [v1_enum][helpstring][uuid] */  DECLSPEC_UUID("00741A6E-64DE-4c78-8484-DF448047B7F9") 
enum eScdHistDataExtractOptions
    {	eScdHDEO_CSV	= 0
    } 	eScdHistDataExtractOptions;

typedef /* [v1_enum][helpstring][uuid] */  DECLSPEC_UUID("03E07251-40A2-4b7e-8FA4-8378965E8546") 
enum eScdAppTagsCodes
    {	eScdATC_OK	= 0,
	eScdATC_TagMissing	= 0x1
    } 	eScdAppTagsCodes;

typedef /* [v1_enum][helpstring][uuid] */  DECLSPEC_UUID("F89EC9B6-262C-4886-A4A7-1BDC298D9DC3") 
enum eScdAppTagType
    {	eScdATT_NULL	= 0,
	eScdATT_Bool	= 1,
	eScdATT_Bit	= 2,
	eScdATT_Byte	= 3,
	eScdATT_Word	= 4,
	eScdATT_DWord	= 5,
	eScdATT_Char	= 6,
	eScdATT_Int	= 7,
	eScdATT_Short	= 8,
	eScdATT_Long	= 9,
	eScdATT_Flt16	= 10,
	eScdATT_Float	= 11,
	eScdATT_Double	= 12,
	eScdATT_Generic	= 13,
	eScdATT_pChar	= 14,
	eScdATT_ppChar	= 15,
	eScdATT_Strng	= 16,
	eScdATT_RqdPage	= 17,
	eScdATT_OptPage	= 18,
	eScdATT_Column	= 19,
	eScdATT_Text	= 20,
	eScdATT_Struct	= 21,
	eScdATT_Element	= 22,
	eScdATT_Object	= 23,
	eScdATT_Array	= 24,
	eScdATT_NullTag	= 0x100,
	eScdATT_ZeroTag	= 0x200,
	eScdATT_NoPrj	= 0x300,
	eScdATT_Error	= 0x400
    } 	eScdAppTagType;

typedef /* [public][v1_enum][helpstring][uuid] */  DECLSPEC_UUID("107F21EA-D128-4cf2-B575-096FF0EAB693") 
enum __MIDL___MIDL_itf_ScdIF_0000_0001
    {	eScdTRS_HistExists	= 1,
	eScdTRS_HistRecording	= 2,
	eScdTRS_HistDriverTag	= 4
    } 	eScdTagRecState;

typedef /* [v1_enum][helpstring][uuid] */  DECLSPEC_UUID("A15B39BF-605D-4010-B30B-B582CFDA63E7") 
enum eScdLinkDrawMethods
    {	eScdLinkDrawMethod_Direct	= 0
    } 	eScdLinkDrawMethods;

typedef /* [v1_enum][helpstring][uuid] */  DECLSPEC_UUID("F4019F64-0BF8-4ff2-BAED-FD62EB03A629") 
enum SV_View
    {	SVV_NULL	= 0,
	SVV_AsRawFrac	= SVV_NULL + 1,
	SVV_AsRawMass	= SVV_AsRawFrac + 1,
	SVV_AsRawMassFlow	= SVV_AsRawMass + 1,
	SVV_AsMass	= SVV_AsRawMassFlow + 1,
	SVV_AsMassFlow	= SVV_AsMass + 1,
	SVV_AsMassFrac	= SVV_AsMassFlow + 1,
	SVV_AsMole	= SVV_AsMassFrac + 1,
	SVV_AsMoleFlow	= SVV_AsMole + 1,
	SVV_AsMoleFrac	= SVV_AsMoleFlow + 1,
	SVV_AsVol	= SVV_AsMoleFrac + 1,
	SVV_AsVolFlow	= SVV_AsVol + 1,
	SVV_AsVolFrac	= SVV_AsVolFlow + 1,
	SVV_AsPartialP	= SVV_AsVolFrac + 1,
	SVV_AsPartialPFrac	= SVV_AsPartialP + 1,
	SVV_AsNVol	= SVV_AsPartialPFrac + 1,
	SVV_AsNVolFlow	= SVV_AsNVol + 1,
	SVV_AsNVolFrac	= SVV_AsNVolFlow + 1,
	SVV_AsElem	= SVV_AsNVolFrac + 1,
	SVV_AsElemFlow	= SVV_AsElem + 1,
	SVV_AsElemFrac	= SVV_AsElemFlow + 1,
	SVV_AsElemConc	= SVV_AsElemFrac + 1,
	SVV_AsElemConc25	= SVV_AsElemConc + 1,
	SVV_AsComp	= SVV_AsElemConc25 + 1,
	SVV_AsCompFlow	= SVV_AsComp + 1,
	SVV_AsCompFrac	= SVV_AsCompFlow + 1,
	SVV_AsCompConc	= SVV_AsCompFrac + 1,
	SVV_AsCompConc25	= SVV_AsCompConc + 1
    } 	SV_View;

typedef /* [v1_enum][helpstring][uuid] */  DECLSPEC_UUID("13CB03A9-6729-4052-BFA6-70238A8B3AB2") 
enum SV_ViewBasis
    {	SVVB_All	= 0,
	SVVB_Start	= SVVB_All,
	SVVB_Sol	= SVVB_Start + 1,
	SVVB_Liq	= SVVB_Sol + 1,
	SVVB_Gas	= SVVB_Liq + 1,
	SVVB_Occ	= SVVB_Gas + 1,
	SVVB_Phase	= SVVB_Occ + 1,
	SVVB_End	= SVVB_Phase + 1
    } 	SV_ViewBasis;

typedef /* [v1_enum][helpstring][uuid] */  DECLSPEC_UUID("B7CDB8DC-C343-4cda-8130-D62FF2631339") 
enum eScdFlwEqnTasks
    {	FET_GetMode	= 0,
	FET_SetQm	= FET_GetMode + 1,
	FET_CalcDP	= FET_SetQm + 1
    } 	eScdFlwEqnTasks;

typedef /* [v1_enum][helpstring][uuid] */  DECLSPEC_UUID("7D3A357B-988E-43fe-984B-0B705BA4F7F4") 
enum eScdPropagateNetTasks
    {	eScdSetFlowMode	= 0
    } 	eScdPropagateNetTasks;

typedef /* [v1_enum][helpstring][uuid] */  DECLSPEC_UUID("1646D290-F237-477b-BCC8-D8BE756C262E") 
enum eScdReactionBasis
    {	RctNull	= 0,
	RctEquilibrium	= RctNull + 1,
	RctRate	= RctEquilibrium + 1,
	RctExtent	= RctRate + 1,
	RctSource	= RctExtent + 1,
	RctSink	= RctSource + 1,
	RctHX1	= RctSink + 1,
	RctHX2	= RctHX1 + 1,
	RctPwr	= RctHX2 + 1
    } 	eScdReactionBasis;



extern RPC_IF_HANDLE __MIDL_itf_ScdIF_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_ScdIF_0000_v0_0_s_ifspec;


#ifndef __ScdIF_LIBRARY_DEFINED__
#define __ScdIF_LIBRARY_DEFINED__

/* library ScdIF */
/* [hidden][helpstring][version][uuid] */ 











































typedef /* [v1_enum][helpstring][uuid] */  DECLSPEC_UUID("FC07E4B7-C73D-40ba-9C32-129B904E6ACD") 
enum eScdMode
    {	eScdSolve_Probal	= 0x1,
	eScdSolve_DynamicXfer	= 0x2,
	eScdSolve_DynamicFull	= 0x4,
	eScdHeat_None	= 0x10,
	eScdHeat_Simple	= 0x20,
	eScdHeat_Full	= 0x40,
	eScdUser_Method_1	= 0x100,
	eScdUser_Method_2	= 0x200,
	eScdUser_Method_3	= 0x400,
	eScdUser_Method_4	= 0x800,
	eScdUser_Method_5	= 0x1000,
	eScdUser_Method_6	= 0x2000,
	eScdUser_Method_7	= 0x4000,
	eScdUser_Method_8	= 0x8000
    } 	eScdMode;


EXTERN_C const IID LIBID_ScdIF;

#ifndef __IScdASyncEvents_INTERFACE_DEFINED__
#define __IScdASyncEvents_INTERFACE_DEFINED__

/* interface IScdASyncEvents */
/* [restricted][unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdASyncEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("EB0A0841-8896-432b-B501-6BBA1FCB38F0")
    IScdASyncEvents : public IDispatch
    {
    public:
        virtual /* [helpstring][id][restricted] */ HRESULT STDMETHODCALLTYPE DoEventMsg( 
            /* [in] */ long Evt,
            /* [in] */ long Data) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScdASyncEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdASyncEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdASyncEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdASyncEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdASyncEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdASyncEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdASyncEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdASyncEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][restricted] */ HRESULT ( STDMETHODCALLTYPE *DoEventMsg )( 
            IScdASyncEvents * This,
            /* [in] */ long Evt,
            /* [in] */ long Data);
        
        END_INTERFACE
    } IScdASyncEventsVtbl;

    interface IScdASyncEvents
    {
        CONST_VTBL struct IScdASyncEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdASyncEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdASyncEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdASyncEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdASyncEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdASyncEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdASyncEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdASyncEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IScdASyncEvents_DoEventMsg(This,Evt,Data)	\
    (This)->lpVtbl -> DoEventMsg(This,Evt,Data)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][restricted] */ HRESULT STDMETHODCALLTYPE IScdASyncEvents_DoEventMsg_Proxy( 
    IScdASyncEvents * This,
    /* [in] */ long Evt,
    /* [in] */ long Data);


void __RPC_STUB IScdASyncEvents_DoEventMsg_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScdASyncEvents_INTERFACE_DEFINED__ */


#ifndef __IScdTaggedObject_INTERFACE_DEFINED__
#define __IScdTaggedObject_INTERFACE_DEFINED__

/* interface IScdTaggedObject */
/* [restricted][unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IScdTaggedObject;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("46138AB0-6E9B-42ab-A50A-0F42B3654009")
    IScdTaggedObject : public IDispatch
    {
    public:
        virtual /* [helpstring][id][restricted][propget] */ HRESULT STDMETHODCALLTYPE get_RawModelPtr( 
            /* [retval][out] */ long *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScdTaggedObjectVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScdTaggedObject * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScdTaggedObject * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScdTaggedObject * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScdTaggedObject * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScdTaggedObject * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScdTaggedObject * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScdTaggedObject * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][restricted][propget] */ HRESULT ( STDMETHODCALLTYPE *get_RawModelPtr )( 
            IScdTaggedObject * This,
            /* [retval][out] */ long *pVal);
        
        END_INTERFACE
    } IScdTaggedObjectVtbl;

    interface IScdTaggedObject
    {
        CONST_VTBL struct IScdTaggedObjectVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScdTaggedObject_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScdTaggedObject_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScdTaggedObject_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScdTaggedObject_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScdTaggedObject_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScdTaggedObject_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScdTaggedObject_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IScdTaggedObject_get_RawModelPtr(This,pVal)	\
    (This)->lpVtbl -> get_RawModelPtr(This,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][restricted][propget] */ HRESULT STDMETHODCALLTYPE IScdTaggedObject_get_RawModelPtr_Proxy( 
    IScdTaggedObject * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IScdTaggedObject_get_RawModelPtr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScdTaggedObject_INTERFACE_DEFINED__ */

#endif /* __ScdIF_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


