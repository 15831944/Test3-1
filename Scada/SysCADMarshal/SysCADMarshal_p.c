

/* this ALWAYS GENERATED file contains the proxy stub code */


 /* File created by MIDL compiler version 6.00.0361 */
/* at Mon Feb 27 10:22:46 2006
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

#if !defined(_M_IA64) && !defined(_M_AMD64)


#pragma warning( disable: 4049 )  /* more than 64k source lines */
#if _MSC_VER >= 1200
#pragma warning(push)
#endif
#pragma warning( disable: 4100 ) /* unreferenced arguments in x86 call */
#pragma warning( disable: 4211 )  /* redefine extent to static */
#pragma warning( disable: 4232 )  /* dllimport identity*/
#define USE_STUBLESS_PROXY


/* verify that the <rpcproxy.h> version is high enough to compile this file*/
#ifndef __REDQ_RPCPROXY_H_VERSION__
#define __REQUIRED_RPCPROXY_H_VERSION__ 440
#endif


#include "rpcproxy.h"
#ifndef __RPCPROXY_H_VERSION__
#error this stub requires an updated version of <rpcproxy.h>
#endif // __RPCPROXY_H_VERSION__


#include "SysCADMarshal_i.h"

#define TYPE_FORMAT_STRING_SIZE   1291                              
#define PROC_FORMAT_STRING_SIZE   533                               
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   1            

typedef struct _MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } MIDL_TYPE_FORMAT_STRING;

typedef struct _MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } MIDL_PROC_FORMAT_STRING;


static RPC_SYNTAX_IDENTIFIER  _RpcTransferSyntax = 
{{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}};


extern const MIDL_TYPE_FORMAT_STRING __MIDL_TypeFormatString;
extern const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IScdMarshal_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IScdMarshal_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IScdExecObject_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IScdExecObject_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO _IScdMarshalEvents_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO _IScdMarshalEvents_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO _IScdExecObjectEvents_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO _IScdExecObjectEvents_ProxyInfo;


extern const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ];

#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif

#if !(TARGET_IS_NT40_OR_LATER)
#error You need a Windows NT 4.0 or later to run this stub because it uses these features:
#error   -Oif or -Oicf, [wire_marshal] or [user_marshal] attribute.
#error However, your C/C++ compilation flags indicate you intend to run this app on earlier systems.
#error This app will die there with the RPC_X_WRONG_STUB_VERSION error.
#endif


static const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure Open */

			0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x3 ),	/* 3 */
/*  8 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 10 */	NdrFcShort( 0x0 ),	/* 0 */
/* 12 */	NdrFcShort( 0x24 ),	/* 36 */
/* 14 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter Filename */

/* 16 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 18 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 20 */	NdrFcShort( 0x4 ),	/* Type Offset=4 */

	/* Parameter Return */

/* 22 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 24 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 26 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 28 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 30 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 32 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Close */

/* 34 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 36 */	NdrFcLong( 0x0 ),	/* 0 */
/* 40 */	NdrFcShort( 0x4 ),	/* 4 */
/* 42 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 44 */	NdrFcShort( 0x6 ),	/* 6 */
/* 46 */	NdrFcShort( 0x24 ),	/* 36 */
/* 48 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter Save */

/* 50 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 52 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 54 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Parameter Return */

/* 56 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 58 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 60 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 62 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 64 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 66 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Start */

/* 68 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 70 */	NdrFcLong( 0x0 ),	/* 0 */
/* 74 */	NdrFcShort( 0x3 ),	/* 3 */
/* 76 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 78 */	NdrFcShort( 0x1c ),	/* 28 */
/* 80 */	NdrFcShort( 0x24 ),	/* 36 */
/* 82 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter pTmCB */

/* 84 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 86 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 88 */	NdrFcShort( 0xe ),	/* Type Offset=14 */

	/* Parameter pdwReturn */

/* 90 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 92 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 94 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 96 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 98 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 100 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Execute */

/* 102 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 104 */	NdrFcLong( 0x0 ),	/* 0 */
/* 108 */	NdrFcShort( 0x4 ),	/* 4 */
/* 110 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 112 */	NdrFcShort( 0x50 ),	/* 80 */
/* 114 */	NdrFcShort( 0x58 ),	/* 88 */
/* 116 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x4,		/* 4 */

	/* Parameter pTmCB */

/* 118 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 120 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 122 */	NdrFcShort( 0xe ),	/* Type Offset=14 */

	/* Parameter pEORet */

/* 124 */	NdrFcShort( 0x11a ),	/* Flags:  must free, in, out, simple ref, */
/* 126 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 128 */	NdrFcShort( 0x2e ),	/* Type Offset=46 */

	/* Parameter pdwReturn */

/* 130 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 132 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 134 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 136 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 138 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 140 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Stop */

/* 142 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 144 */	NdrFcLong( 0x0 ),	/* 0 */
/* 148 */	NdrFcShort( 0x5 ),	/* 5 */
/* 150 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 152 */	NdrFcShort( 0x1c ),	/* 28 */
/* 154 */	NdrFcShort( 0x24 ),	/* 36 */
/* 156 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter pTmCB */

/* 158 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 160 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 162 */	NdrFcShort( 0xe ),	/* Type Offset=14 */

	/* Parameter pdwReturn */

/* 164 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 166 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 168 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 170 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 172 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 174 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure QueryTime */

/* 176 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 178 */	NdrFcLong( 0x0 ),	/* 0 */
/* 182 */	NdrFcShort( 0x6 ),	/* 6 */
/* 184 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 186 */	NdrFcShort( 0x48 ),	/* 72 */
/* 188 */	NdrFcShort( 0x50 ),	/* 80 */
/* 190 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x4,		/* 4 */

	/* Parameter pTmCB */

/* 192 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 194 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 196 */	NdrFcShort( 0xe ),	/* Type Offset=14 */

	/* Parameter pdTimeReqd */

/* 198 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 200 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 202 */	0xc,		/* FC_DOUBLE */
			0x0,		/* 0 */

	/* Parameter pdDeltaTReqd */

/* 204 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 206 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 208 */	0xc,		/* FC_DOUBLE */
			0x0,		/* 0 */

	/* Return value */

/* 210 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 212 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 214 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure QuerySubscriptionTagsRequired */

/* 216 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 218 */	NdrFcLong( 0x0 ),	/* 0 */
/* 222 */	NdrFcShort( 0x7 ),	/* 7 */
/* 224 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 226 */	NdrFcShort( 0x3e ),	/* 62 */
/* 228 */	NdrFcShort( 0x40 ),	/* 64 */
/* 230 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x5,		/* 5 */

	/* Parameter Start */

/* 232 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 234 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 236 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Parameter dwMaxCount */

/* 238 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 240 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 242 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwCount */

/* 244 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 246 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 248 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pTags */

/* 250 */	NdrFcShort( 0x201b ),	/* Flags:  must size, must free, in, out, srv alloc size=8 */
/* 252 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 254 */	NdrFcShort( 0x3a ),	/* Type Offset=58 */

	/* Return value */

/* 256 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 258 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 260 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure QuerySubscriptionTagsAvailable */

/* 262 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 264 */	NdrFcLong( 0x0 ),	/* 0 */
/* 268 */	NdrFcShort( 0x8 ),	/* 8 */
/* 270 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 272 */	NdrFcShort( 0x38 ),	/* 56 */
/* 274 */	NdrFcShort( 0x40 ),	/* 64 */
/* 276 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x4,		/* 4 */

	/* Parameter dwMaxCount */

/* 278 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 280 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 282 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwCount */

/* 284 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 286 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 288 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pTags */

/* 290 */	NdrFcShort( 0x201b ),	/* Flags:  must size, must free, in, out, srv alloc size=8 */
/* 292 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 294 */	NdrFcShort( 0x6e ),	/* Type Offset=110 */

	/* Return value */

/* 296 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 298 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 300 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ReadSubscriptionData */

/* 302 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 304 */	NdrFcLong( 0x0 ),	/* 0 */
/* 308 */	NdrFcShort( 0x9 ),	/* 9 */
/* 310 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 312 */	NdrFcShort( 0x44 ),	/* 68 */
/* 314 */	NdrFcShort( 0x40 ),	/* 64 */
/* 316 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x6,		/* 6 */

	/* Parameter Start */

/* 318 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 320 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 322 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Parameter ReadAll */

/* 324 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 326 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 328 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Parameter dwMaxCount */

/* 330 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 332 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 334 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwCount */

/* 336 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 338 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 340 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pData */

/* 342 */	NdrFcShort( 0x201b ),	/* Flags:  must size, must free, in, out, srv alloc size=8 */
/* 344 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 346 */	NdrFcShort( 0x88 ),	/* Type Offset=136 */

	/* Return value */

/* 348 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 350 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 352 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure WriteSubscriptionData */

/* 354 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 356 */	NdrFcLong( 0x0 ),	/* 0 */
/* 360 */	NdrFcShort( 0xa ),	/* 10 */
/* 362 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 364 */	NdrFcShort( 0x38 ),	/* 56 */
/* 366 */	NdrFcShort( 0x40 ),	/* 64 */
/* 368 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x4,		/* 4 */

	/* Parameter dwMaxCount */

/* 370 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 372 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 374 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwCount */

/* 376 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 378 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 380 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pData */

/* 382 */	NdrFcShort( 0x201b ),	/* Flags:  must size, must free, in, out, srv alloc size=8 */
/* 384 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 386 */	NdrFcShort( 0x49e ),	/* Type Offset=1182 */

	/* Return value */

/* 388 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 390 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 392 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ReadTaggedItems */

/* 394 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 396 */	NdrFcLong( 0x0 ),	/* 0 */
/* 400 */	NdrFcShort( 0xb ),	/* 11 */
/* 402 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 404 */	NdrFcShort( 0x8 ),	/* 8 */
/* 406 */	NdrFcShort( 0x8 ),	/* 8 */
/* 408 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x5,		/* 5 */

	/* Parameter dwCount */

/* 410 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 412 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 414 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pTags */

/* 416 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 418 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 420 */	NdrFcShort( 0x4bc ),	/* Type Offset=1212 */

	/* Parameter pData */

/* 422 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 424 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 426 */	NdrFcShort( 0x4d2 ),	/* Type Offset=1234 */

	/* Parameter pReturn */

/* 428 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 430 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 432 */	NdrFcShort( 0x4e8 ),	/* Type Offset=1256 */

	/* Return value */

/* 434 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 436 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 438 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure WriteTaggedItems */

/* 440 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 442 */	NdrFcLong( 0x0 ),	/* 0 */
/* 446 */	NdrFcShort( 0xc ),	/* 12 */
/* 448 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 450 */	NdrFcShort( 0x8 ),	/* 8 */
/* 452 */	NdrFcShort( 0x8 ),	/* 8 */
/* 454 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x5,		/* 5 */

	/* Parameter dwCount */

/* 456 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 458 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 460 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pTags */

/* 462 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 464 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 466 */	NdrFcShort( 0x4bc ),	/* Type Offset=1212 */

	/* Parameter pData */

/* 468 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
/* 470 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 472 */	NdrFcShort( 0x4d2 ),	/* Type Offset=1234 */

	/* Parameter pReturn */

/* 474 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 476 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 478 */	NdrFcShort( 0x4e8 ),	/* Type Offset=1256 */

	/* Return value */

/* 480 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 482 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 484 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ReadTaggedItems */

/* 486 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 488 */	NdrFcLong( 0x0 ),	/* 0 */
/* 492 */	NdrFcShort( 0x3 ),	/* 3 */
/* 494 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 496 */	NdrFcShort( 0x8 ),	/* 8 */
/* 498 */	NdrFcShort( 0x8 ),	/* 8 */
/* 500 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x5,		/* 5 */

	/* Parameter dwCount */

/* 502 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 504 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 506 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pTags */

/* 508 */	NdrFcShort( 0x200b ),	/* Flags:  must size, must free, in, srv alloc size=8 */
/* 510 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 512 */	NdrFcShort( 0x4f2 ),	/* Type Offset=1266 */

	/* Parameter pData */

/* 514 */	NdrFcShort( 0x201b ),	/* Flags:  must size, must free, in, out, srv alloc size=8 */
/* 516 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 518 */	NdrFcShort( 0x4fa ),	/* Type Offset=1274 */

	/* Parameter pReturn */

/* 520 */	NdrFcShort( 0x201b ),	/* Flags:  must size, must free, in, out, srv alloc size=8 */
/* 522 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 524 */	NdrFcShort( 0x502 ),	/* Type Offset=1282 */

	/* Return value */

/* 526 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 528 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 530 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

			0x0
        }
    };

static const MIDL_TYPE_FORMAT_STRING __MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */
/*  2 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/*  4 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/*  6 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/*  8 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 10 */	
			0x11, 0x0,	/* FC_RP */
/* 12 */	NdrFcShort( 0x2 ),	/* Offset= 2 (14) */
/* 14 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x7,		/* 7 */
/* 16 */	NdrFcShort( 0x58 ),	/* 88 */
/* 18 */	NdrFcShort( 0x0 ),	/* 0 */
/* 20 */	NdrFcShort( 0x0 ),	/* Offset= 0 (20) */
/* 22 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 24 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 26 */	0xc,		/* FC_DOUBLE */
			0xc,		/* FC_DOUBLE */
/* 28 */	0xc,		/* FC_DOUBLE */
			0xc,		/* FC_DOUBLE */
/* 30 */	0xc,		/* FC_DOUBLE */
			0xc,		/* FC_DOUBLE */
/* 32 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 34 */	0xd,		/* FC_ENUM16 */
			0x40,		/* FC_STRUCTPAD4 */
/* 36 */	0xc,		/* FC_DOUBLE */
			0x5b,		/* FC_END */
/* 38 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 40 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 42 */	
			0x11, 0x0,	/* FC_RP */
/* 44 */	NdrFcShort( 0x2 ),	/* Offset= 2 (46) */
/* 46 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 48 */	NdrFcShort( 0x10 ),	/* 16 */
/* 50 */	0xc,		/* FC_DOUBLE */
			0xc,		/* FC_DOUBLE */
/* 52 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 54 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 56 */	0xc,		/* FC_DOUBLE */
			0x5c,		/* FC_PAD */
/* 58 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 60 */	NdrFcShort( 0x2 ),	/* Offset= 2 (62) */
/* 62 */	
			0x13, 0x0,	/* FC_OP */
/* 64 */	NdrFcShort( 0x1c ),	/* Offset= 28 (92) */
/* 66 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 68 */	NdrFcShort( 0x1c ),	/* 28 */
/* 70 */	NdrFcShort( 0x0 ),	/* 0 */
/* 72 */	NdrFcShort( 0xc ),	/* Offset= 12 (84) */
/* 74 */	0x36,		/* FC_POINTER */
			0x36,		/* FC_POINTER */
/* 76 */	0x6,		/* FC_SHORT */
			0x3e,		/* FC_STRUCTPAD2 */
/* 78 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 80 */	0x8,		/* FC_LONG */
			0xd,		/* FC_ENUM16 */
/* 82 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 84 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 86 */	
			0x22,		/* FC_C_CSTRING */
			0x5c,		/* FC_PAD */
/* 88 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 90 */	
			0x22,		/* FC_C_CSTRING */
			0x5c,		/* FC_PAD */
/* 92 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 94 */	NdrFcShort( 0x0 ),	/* 0 */
/* 96 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 98 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 100 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 102 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 104 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 106 */	NdrFcShort( 0xffd8 ),	/* Offset= -40 (66) */
/* 108 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 110 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 112 */	NdrFcShort( 0x2 ),	/* Offset= 2 (114) */
/* 114 */	
			0x13, 0x0,	/* FC_OP */
/* 116 */	NdrFcShort( 0x2 ),	/* Offset= 2 (118) */
/* 118 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 120 */	NdrFcShort( 0x0 ),	/* 0 */
/* 122 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 124 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 126 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 128 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 130 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 132 */	NdrFcShort( 0xffbe ),	/* Offset= -66 (66) */
/* 134 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 136 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 138 */	NdrFcShort( 0x2 ),	/* Offset= 2 (140) */
/* 140 */	
			0x13, 0x0,	/* FC_OP */
/* 142 */	NdrFcShort( 0x3fe ),	/* Offset= 1022 (1164) */
/* 144 */	
			0x13, 0x0,	/* FC_OP */
/* 146 */	NdrFcShort( 0x3ca ),	/* Offset= 970 (1116) */
/* 148 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 150 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 152 */	NdrFcShort( 0xfff8 ),	/* -8 */
/* 154 */	NdrFcShort( 0x2 ),	/* Offset= 2 (156) */
/* 156 */	NdrFcShort( 0x10 ),	/* 16 */
/* 158 */	NdrFcShort( 0x2f ),	/* 47 */
/* 160 */	NdrFcLong( 0x14 ),	/* 20 */
/* 164 */	NdrFcShort( 0x800b ),	/* Simple arm type: FC_HYPER */
/* 166 */	NdrFcLong( 0x3 ),	/* 3 */
/* 170 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 172 */	NdrFcLong( 0x11 ),	/* 17 */
/* 176 */	NdrFcShort( 0x8001 ),	/* Simple arm type: FC_BYTE */
/* 178 */	NdrFcLong( 0x2 ),	/* 2 */
/* 182 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 184 */	NdrFcLong( 0x4 ),	/* 4 */
/* 188 */	NdrFcShort( 0x800a ),	/* Simple arm type: FC_FLOAT */
/* 190 */	NdrFcLong( 0x5 ),	/* 5 */
/* 194 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 196 */	NdrFcLong( 0xb ),	/* 11 */
/* 200 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 202 */	NdrFcLong( 0xa ),	/* 10 */
/* 206 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 208 */	NdrFcLong( 0x6 ),	/* 6 */
/* 212 */	NdrFcShort( 0xe8 ),	/* Offset= 232 (444) */
/* 214 */	NdrFcLong( 0x7 ),	/* 7 */
/* 218 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 220 */	NdrFcLong( 0x8 ),	/* 8 */
/* 224 */	NdrFcShort( 0xe2 ),	/* Offset= 226 (450) */
/* 226 */	NdrFcLong( 0xd ),	/* 13 */
/* 230 */	NdrFcShort( 0xf4 ),	/* Offset= 244 (474) */
/* 232 */	NdrFcLong( 0x9 ),	/* 9 */
/* 236 */	NdrFcShort( 0x100 ),	/* Offset= 256 (492) */
/* 238 */	NdrFcLong( 0x2000 ),	/* 8192 */
/* 242 */	NdrFcShort( 0x10c ),	/* Offset= 268 (510) */
/* 244 */	NdrFcLong( 0x24 ),	/* 36 */
/* 248 */	NdrFcShort( 0x31a ),	/* Offset= 794 (1042) */
/* 250 */	NdrFcLong( 0x4024 ),	/* 16420 */
/* 254 */	NdrFcShort( 0x314 ),	/* Offset= 788 (1042) */
/* 256 */	NdrFcLong( 0x4011 ),	/* 16401 */
/* 260 */	NdrFcShort( 0x312 ),	/* Offset= 786 (1046) */
/* 262 */	NdrFcLong( 0x4002 ),	/* 16386 */
/* 266 */	NdrFcShort( 0x310 ),	/* Offset= 784 (1050) */
/* 268 */	NdrFcLong( 0x4003 ),	/* 16387 */
/* 272 */	NdrFcShort( 0x30e ),	/* Offset= 782 (1054) */
/* 274 */	NdrFcLong( 0x4014 ),	/* 16404 */
/* 278 */	NdrFcShort( 0x30c ),	/* Offset= 780 (1058) */
/* 280 */	NdrFcLong( 0x4004 ),	/* 16388 */
/* 284 */	NdrFcShort( 0x30a ),	/* Offset= 778 (1062) */
/* 286 */	NdrFcLong( 0x4005 ),	/* 16389 */
/* 290 */	NdrFcShort( 0x308 ),	/* Offset= 776 (1066) */
/* 292 */	NdrFcLong( 0x400b ),	/* 16395 */
/* 296 */	NdrFcShort( 0x2f2 ),	/* Offset= 754 (1050) */
/* 298 */	NdrFcLong( 0x400a ),	/* 16394 */
/* 302 */	NdrFcShort( 0x2f0 ),	/* Offset= 752 (1054) */
/* 304 */	NdrFcLong( 0x4006 ),	/* 16390 */
/* 308 */	NdrFcShort( 0x2fa ),	/* Offset= 762 (1070) */
/* 310 */	NdrFcLong( 0x4007 ),	/* 16391 */
/* 314 */	NdrFcShort( 0x2f0 ),	/* Offset= 752 (1066) */
/* 316 */	NdrFcLong( 0x4008 ),	/* 16392 */
/* 320 */	NdrFcShort( 0x2f2 ),	/* Offset= 754 (1074) */
/* 322 */	NdrFcLong( 0x400d ),	/* 16397 */
/* 326 */	NdrFcShort( 0x2f0 ),	/* Offset= 752 (1078) */
/* 328 */	NdrFcLong( 0x4009 ),	/* 16393 */
/* 332 */	NdrFcShort( 0x2ee ),	/* Offset= 750 (1082) */
/* 334 */	NdrFcLong( 0x6000 ),	/* 24576 */
/* 338 */	NdrFcShort( 0x2ec ),	/* Offset= 748 (1086) */
/* 340 */	NdrFcLong( 0x400c ),	/* 16396 */
/* 344 */	NdrFcShort( 0x2ea ),	/* Offset= 746 (1090) */
/* 346 */	NdrFcLong( 0x10 ),	/* 16 */
/* 350 */	NdrFcShort( 0x8002 ),	/* Simple arm type: FC_CHAR */
/* 352 */	NdrFcLong( 0x12 ),	/* 18 */
/* 356 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 358 */	NdrFcLong( 0x13 ),	/* 19 */
/* 362 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 364 */	NdrFcLong( 0x15 ),	/* 21 */
/* 368 */	NdrFcShort( 0x800b ),	/* Simple arm type: FC_HYPER */
/* 370 */	NdrFcLong( 0x16 ),	/* 22 */
/* 374 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 376 */	NdrFcLong( 0x17 ),	/* 23 */
/* 380 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 382 */	NdrFcLong( 0xe ),	/* 14 */
/* 386 */	NdrFcShort( 0x2c8 ),	/* Offset= 712 (1098) */
/* 388 */	NdrFcLong( 0x400e ),	/* 16398 */
/* 392 */	NdrFcShort( 0x2cc ),	/* Offset= 716 (1108) */
/* 394 */	NdrFcLong( 0x4010 ),	/* 16400 */
/* 398 */	NdrFcShort( 0x2ca ),	/* Offset= 714 (1112) */
/* 400 */	NdrFcLong( 0x4012 ),	/* 16402 */
/* 404 */	NdrFcShort( 0x286 ),	/* Offset= 646 (1050) */
/* 406 */	NdrFcLong( 0x4013 ),	/* 16403 */
/* 410 */	NdrFcShort( 0x284 ),	/* Offset= 644 (1054) */
/* 412 */	NdrFcLong( 0x4015 ),	/* 16405 */
/* 416 */	NdrFcShort( 0x282 ),	/* Offset= 642 (1058) */
/* 418 */	NdrFcLong( 0x4016 ),	/* 16406 */
/* 422 */	NdrFcShort( 0x278 ),	/* Offset= 632 (1054) */
/* 424 */	NdrFcLong( 0x4017 ),	/* 16407 */
/* 428 */	NdrFcShort( 0x272 ),	/* Offset= 626 (1054) */
/* 430 */	NdrFcLong( 0x0 ),	/* 0 */
/* 434 */	NdrFcShort( 0x0 ),	/* Offset= 0 (434) */
/* 436 */	NdrFcLong( 0x1 ),	/* 1 */
/* 440 */	NdrFcShort( 0x0 ),	/* Offset= 0 (440) */
/* 442 */	NdrFcShort( 0xffff ),	/* Offset= -1 (441) */
/* 444 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 446 */	NdrFcShort( 0x8 ),	/* 8 */
/* 448 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 450 */	
			0x13, 0x0,	/* FC_OP */
/* 452 */	NdrFcShort( 0xc ),	/* Offset= 12 (464) */
/* 454 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 456 */	NdrFcShort( 0x2 ),	/* 2 */
/* 458 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 460 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 462 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 464 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 466 */	NdrFcShort( 0x8 ),	/* 8 */
/* 468 */	NdrFcShort( 0xfff2 ),	/* Offset= -14 (454) */
/* 470 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 472 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 474 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 476 */	NdrFcLong( 0x0 ),	/* 0 */
/* 480 */	NdrFcShort( 0x0 ),	/* 0 */
/* 482 */	NdrFcShort( 0x0 ),	/* 0 */
/* 484 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 486 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 488 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 490 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 492 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 494 */	NdrFcLong( 0x20400 ),	/* 132096 */
/* 498 */	NdrFcShort( 0x0 ),	/* 0 */
/* 500 */	NdrFcShort( 0x0 ),	/* 0 */
/* 502 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 504 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 506 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 508 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 510 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 512 */	NdrFcShort( 0x2 ),	/* Offset= 2 (514) */
/* 514 */	
			0x13, 0x0,	/* FC_OP */
/* 516 */	NdrFcShort( 0x1fc ),	/* Offset= 508 (1024) */
/* 518 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x49,		/* 73 */
/* 520 */	NdrFcShort( 0x18 ),	/* 24 */
/* 522 */	NdrFcShort( 0xa ),	/* 10 */
/* 524 */	NdrFcLong( 0x8 ),	/* 8 */
/* 528 */	NdrFcShort( 0x58 ),	/* Offset= 88 (616) */
/* 530 */	NdrFcLong( 0xd ),	/* 13 */
/* 534 */	NdrFcShort( 0x78 ),	/* Offset= 120 (654) */
/* 536 */	NdrFcLong( 0x9 ),	/* 9 */
/* 540 */	NdrFcShort( 0x94 ),	/* Offset= 148 (688) */
/* 542 */	NdrFcLong( 0xc ),	/* 12 */
/* 546 */	NdrFcShort( 0xbc ),	/* Offset= 188 (734) */
/* 548 */	NdrFcLong( 0x24 ),	/* 36 */
/* 552 */	NdrFcShort( 0x114 ),	/* Offset= 276 (828) */
/* 554 */	NdrFcLong( 0x800d ),	/* 32781 */
/* 558 */	NdrFcShort( 0x130 ),	/* Offset= 304 (862) */
/* 560 */	NdrFcLong( 0x10 ),	/* 16 */
/* 564 */	NdrFcShort( 0x148 ),	/* Offset= 328 (892) */
/* 566 */	NdrFcLong( 0x2 ),	/* 2 */
/* 570 */	NdrFcShort( 0x160 ),	/* Offset= 352 (922) */
/* 572 */	NdrFcLong( 0x3 ),	/* 3 */
/* 576 */	NdrFcShort( 0x178 ),	/* Offset= 376 (952) */
/* 578 */	NdrFcLong( 0x14 ),	/* 20 */
/* 582 */	NdrFcShort( 0x190 ),	/* Offset= 400 (982) */
/* 584 */	NdrFcShort( 0xffff ),	/* Offset= -1 (583) */
/* 586 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 588 */	NdrFcShort( 0x4 ),	/* 4 */
/* 590 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 592 */	NdrFcShort( 0x0 ),	/* 0 */
/* 594 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 596 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 598 */	NdrFcShort( 0x4 ),	/* 4 */
/* 600 */	NdrFcShort( 0x0 ),	/* 0 */
/* 602 */	NdrFcShort( 0x1 ),	/* 1 */
/* 604 */	NdrFcShort( 0x0 ),	/* 0 */
/* 606 */	NdrFcShort( 0x0 ),	/* 0 */
/* 608 */	0x13, 0x0,	/* FC_OP */
/* 610 */	NdrFcShort( 0xff6e ),	/* Offset= -146 (464) */
/* 612 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 614 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 616 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 618 */	NdrFcShort( 0x8 ),	/* 8 */
/* 620 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 622 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 624 */	NdrFcShort( 0x4 ),	/* 4 */
/* 626 */	NdrFcShort( 0x4 ),	/* 4 */
/* 628 */	0x11, 0x0,	/* FC_RP */
/* 630 */	NdrFcShort( 0xffd4 ),	/* Offset= -44 (586) */
/* 632 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 634 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 636 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 638 */	NdrFcShort( 0x0 ),	/* 0 */
/* 640 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 642 */	NdrFcShort( 0x0 ),	/* 0 */
/* 644 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 648 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 650 */	NdrFcShort( 0xff50 ),	/* Offset= -176 (474) */
/* 652 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 654 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 656 */	NdrFcShort( 0x8 ),	/* 8 */
/* 658 */	NdrFcShort( 0x0 ),	/* 0 */
/* 660 */	NdrFcShort( 0x6 ),	/* Offset= 6 (666) */
/* 662 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 664 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 666 */	
			0x11, 0x0,	/* FC_RP */
/* 668 */	NdrFcShort( 0xffe0 ),	/* Offset= -32 (636) */
/* 670 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 672 */	NdrFcShort( 0x0 ),	/* 0 */
/* 674 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 676 */	NdrFcShort( 0x0 ),	/* 0 */
/* 678 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 682 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 684 */	NdrFcShort( 0xff40 ),	/* Offset= -192 (492) */
/* 686 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 688 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 690 */	NdrFcShort( 0x8 ),	/* 8 */
/* 692 */	NdrFcShort( 0x0 ),	/* 0 */
/* 694 */	NdrFcShort( 0x6 ),	/* Offset= 6 (700) */
/* 696 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 698 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 700 */	
			0x11, 0x0,	/* FC_RP */
/* 702 */	NdrFcShort( 0xffe0 ),	/* Offset= -32 (670) */
/* 704 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 706 */	NdrFcShort( 0x4 ),	/* 4 */
/* 708 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 710 */	NdrFcShort( 0x0 ),	/* 0 */
/* 712 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 714 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 716 */	NdrFcShort( 0x4 ),	/* 4 */
/* 718 */	NdrFcShort( 0x0 ),	/* 0 */
/* 720 */	NdrFcShort( 0x1 ),	/* 1 */
/* 722 */	NdrFcShort( 0x0 ),	/* 0 */
/* 724 */	NdrFcShort( 0x0 ),	/* 0 */
/* 726 */	0x13, 0x0,	/* FC_OP */
/* 728 */	NdrFcShort( 0x184 ),	/* Offset= 388 (1116) */
/* 730 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 732 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 734 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 736 */	NdrFcShort( 0x8 ),	/* 8 */
/* 738 */	NdrFcShort( 0x0 ),	/* 0 */
/* 740 */	NdrFcShort( 0x6 ),	/* Offset= 6 (746) */
/* 742 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 744 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 746 */	
			0x11, 0x0,	/* FC_RP */
/* 748 */	NdrFcShort( 0xffd4 ),	/* Offset= -44 (704) */
/* 750 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 752 */	NdrFcLong( 0x2f ),	/* 47 */
/* 756 */	NdrFcShort( 0x0 ),	/* 0 */
/* 758 */	NdrFcShort( 0x0 ),	/* 0 */
/* 760 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 762 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 764 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 766 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 768 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 770 */	NdrFcShort( 0x1 ),	/* 1 */
/* 772 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 774 */	NdrFcShort( 0x4 ),	/* 4 */
/* 776 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 778 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 780 */	NdrFcShort( 0x10 ),	/* 16 */
/* 782 */	NdrFcShort( 0x0 ),	/* 0 */
/* 784 */	NdrFcShort( 0xa ),	/* Offset= 10 (794) */
/* 786 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 788 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 790 */	NdrFcShort( 0xffd8 ),	/* Offset= -40 (750) */
/* 792 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 794 */	
			0x13, 0x0,	/* FC_OP */
/* 796 */	NdrFcShort( 0xffe4 ),	/* Offset= -28 (768) */
/* 798 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 800 */	NdrFcShort( 0x4 ),	/* 4 */
/* 802 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 804 */	NdrFcShort( 0x0 ),	/* 0 */
/* 806 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 808 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 810 */	NdrFcShort( 0x4 ),	/* 4 */
/* 812 */	NdrFcShort( 0x0 ),	/* 0 */
/* 814 */	NdrFcShort( 0x1 ),	/* 1 */
/* 816 */	NdrFcShort( 0x0 ),	/* 0 */
/* 818 */	NdrFcShort( 0x0 ),	/* 0 */
/* 820 */	0x13, 0x0,	/* FC_OP */
/* 822 */	NdrFcShort( 0xffd4 ),	/* Offset= -44 (778) */
/* 824 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 826 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 828 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 830 */	NdrFcShort( 0x8 ),	/* 8 */
/* 832 */	NdrFcShort( 0x0 ),	/* 0 */
/* 834 */	NdrFcShort( 0x6 ),	/* Offset= 6 (840) */
/* 836 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 838 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 840 */	
			0x11, 0x0,	/* FC_RP */
/* 842 */	NdrFcShort( 0xffd4 ),	/* Offset= -44 (798) */
/* 844 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 846 */	NdrFcShort( 0x8 ),	/* 8 */
/* 848 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 850 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 852 */	NdrFcShort( 0x10 ),	/* 16 */
/* 854 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 856 */	0x6,		/* FC_SHORT */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 858 */	0x0,		/* 0 */
			NdrFcShort( 0xfff1 ),	/* Offset= -15 (844) */
			0x5b,		/* FC_END */
/* 862 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 864 */	NdrFcShort( 0x18 ),	/* 24 */
/* 866 */	NdrFcShort( 0x0 ),	/* 0 */
/* 868 */	NdrFcShort( 0xa ),	/* Offset= 10 (878) */
/* 870 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 872 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 874 */	NdrFcShort( 0xffe8 ),	/* Offset= -24 (850) */
/* 876 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 878 */	
			0x11, 0x0,	/* FC_RP */
/* 880 */	NdrFcShort( 0xff0c ),	/* Offset= -244 (636) */
/* 882 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 884 */	NdrFcShort( 0x1 ),	/* 1 */
/* 886 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 888 */	NdrFcShort( 0x0 ),	/* 0 */
/* 890 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 892 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 894 */	NdrFcShort( 0x8 ),	/* 8 */
/* 896 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 898 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 900 */	NdrFcShort( 0x4 ),	/* 4 */
/* 902 */	NdrFcShort( 0x4 ),	/* 4 */
/* 904 */	0x13, 0x0,	/* FC_OP */
/* 906 */	NdrFcShort( 0xffe8 ),	/* Offset= -24 (882) */
/* 908 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 910 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 912 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 914 */	NdrFcShort( 0x2 ),	/* 2 */
/* 916 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 918 */	NdrFcShort( 0x0 ),	/* 0 */
/* 920 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 922 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 924 */	NdrFcShort( 0x8 ),	/* 8 */
/* 926 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 928 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 930 */	NdrFcShort( 0x4 ),	/* 4 */
/* 932 */	NdrFcShort( 0x4 ),	/* 4 */
/* 934 */	0x13, 0x0,	/* FC_OP */
/* 936 */	NdrFcShort( 0xffe8 ),	/* Offset= -24 (912) */
/* 938 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 940 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 942 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 944 */	NdrFcShort( 0x4 ),	/* 4 */
/* 946 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 948 */	NdrFcShort( 0x0 ),	/* 0 */
/* 950 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 952 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 954 */	NdrFcShort( 0x8 ),	/* 8 */
/* 956 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 958 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 960 */	NdrFcShort( 0x4 ),	/* 4 */
/* 962 */	NdrFcShort( 0x4 ),	/* 4 */
/* 964 */	0x13, 0x0,	/* FC_OP */
/* 966 */	NdrFcShort( 0xffe8 ),	/* Offset= -24 (942) */
/* 968 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 970 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 972 */	
			0x1b,		/* FC_CARRAY */
			0x7,		/* 7 */
/* 974 */	NdrFcShort( 0x8 ),	/* 8 */
/* 976 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 978 */	NdrFcShort( 0x0 ),	/* 0 */
/* 980 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 982 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 984 */	NdrFcShort( 0x8 ),	/* 8 */
/* 986 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 988 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 990 */	NdrFcShort( 0x4 ),	/* 4 */
/* 992 */	NdrFcShort( 0x4 ),	/* 4 */
/* 994 */	0x13, 0x0,	/* FC_OP */
/* 996 */	NdrFcShort( 0xffe8 ),	/* Offset= -24 (972) */
/* 998 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1000 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1002 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 1004 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1006 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1008 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1010 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1012 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1014 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 1016 */	NdrFcShort( 0xffd8 ),	/* -40 */
/* 1018 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1020 */	NdrFcShort( 0xffee ),	/* Offset= -18 (1002) */
/* 1022 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1024 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1026 */	NdrFcShort( 0x28 ),	/* 40 */
/* 1028 */	NdrFcShort( 0xffee ),	/* Offset= -18 (1010) */
/* 1030 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1030) */
/* 1032 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1034 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1036 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1038 */	NdrFcShort( 0xfdf8 ),	/* Offset= -520 (518) */
/* 1040 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1042 */	
			0x13, 0x0,	/* FC_OP */
/* 1044 */	NdrFcShort( 0xfef6 ),	/* Offset= -266 (778) */
/* 1046 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1048 */	0x1,		/* FC_BYTE */
			0x5c,		/* FC_PAD */
/* 1050 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1052 */	0x6,		/* FC_SHORT */
			0x5c,		/* FC_PAD */
/* 1054 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1056 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 1058 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1060 */	0xb,		/* FC_HYPER */
			0x5c,		/* FC_PAD */
/* 1062 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1064 */	0xa,		/* FC_FLOAT */
			0x5c,		/* FC_PAD */
/* 1066 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1068 */	0xc,		/* FC_DOUBLE */
			0x5c,		/* FC_PAD */
/* 1070 */	
			0x13, 0x0,	/* FC_OP */
/* 1072 */	NdrFcShort( 0xfd8c ),	/* Offset= -628 (444) */
/* 1074 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1076 */	NdrFcShort( 0xfd8e ),	/* Offset= -626 (450) */
/* 1078 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1080 */	NdrFcShort( 0xfda2 ),	/* Offset= -606 (474) */
/* 1082 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1084 */	NdrFcShort( 0xfdb0 ),	/* Offset= -592 (492) */
/* 1086 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1088 */	NdrFcShort( 0xfdbe ),	/* Offset= -578 (510) */
/* 1090 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1092 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1094) */
/* 1094 */	
			0x13, 0x0,	/* FC_OP */
/* 1096 */	NdrFcShort( 0x14 ),	/* Offset= 20 (1116) */
/* 1098 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 1100 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1102 */	0x6,		/* FC_SHORT */
			0x1,		/* FC_BYTE */
/* 1104 */	0x1,		/* FC_BYTE */
			0x8,		/* FC_LONG */
/* 1106 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 1108 */	
			0x13, 0x0,	/* FC_OP */
/* 1110 */	NdrFcShort( 0xfff4 ),	/* Offset= -12 (1098) */
/* 1112 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1114 */	0x2,		/* FC_CHAR */
			0x5c,		/* FC_PAD */
/* 1116 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x7,		/* 7 */
/* 1118 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1120 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1122 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1122) */
/* 1124 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1126 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1128 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1130 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1132 */	NdrFcShort( 0xfc28 ),	/* Offset= -984 (148) */
/* 1134 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1136 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 1138 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1140 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1142 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1144 */	NdrFcShort( 0xfc18 ),	/* Offset= -1000 (144) */
/* 1146 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1148 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1150 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1152 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1152) */
/* 1154 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1156 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1158 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1160 */	NdrFcShort( 0xffe8 ),	/* Offset= -24 (1136) */
/* 1162 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1164 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 1166 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1168 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 1170 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1172 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 1174 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1176 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1178 */	NdrFcShort( 0xffe0 ),	/* Offset= -32 (1146) */
/* 1180 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1182 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 1184 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1186) */
/* 1186 */	
			0x13, 0x0,	/* FC_OP */
/* 1188 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1190) */
/* 1190 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 1192 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1194 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 1196 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1198 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 1200 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1202 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1204 */	NdrFcShort( 0xffc6 ),	/* Offset= -58 (1146) */
/* 1206 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1208 */	
			0x11, 0x0,	/* FC_RP */
/* 1210 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1212) */
/* 1212 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 1214 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1216 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1218 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1220 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 1224 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1226 */	NdrFcShort( 0xfb78 ),	/* Offset= -1160 (66) */
/* 1228 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1230 */	
			0x11, 0x0,	/* FC_RP */
/* 1232 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1234) */
/* 1234 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 1236 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1238 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1240 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1242 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 1246 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1248 */	NdrFcShort( 0xff9a ),	/* Offset= -102 (1146) */
/* 1250 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1252 */	
			0x11, 0x0,	/* FC_RP */
/* 1254 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1256) */
/* 1256 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1258 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1260 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1262 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1264 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1266 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 1268 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1270) */
/* 1270 */	
			0x12, 0x0,	/* FC_UP */
/* 1272 */	NdrFcShort( 0xffc4 ),	/* Offset= -60 (1212) */
/* 1274 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 1276 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1278) */
/* 1278 */	
			0x13, 0x0,	/* FC_OP */
/* 1280 */	NdrFcShort( 0xffd2 ),	/* Offset= -46 (1234) */
/* 1282 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 1284 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1286) */
/* 1286 */	
			0x13, 0x0,	/* FC_OP */
/* 1288 */	NdrFcShort( 0xffe0 ),	/* Offset= -32 (1256) */

			0x0
        }
    };

static const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ] = 
        {
            
            {
            VARIANT_UserSize
            ,VARIANT_UserMarshal
            ,VARIANT_UserUnmarshal
            ,VARIANT_UserFree
            }

        };



/* Standard interface: __MIDL_itf_SysCADMarshal_0000, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IScdMarshal, ver. 0.0,
   GUID={0x057E8131,0xC4D6,0x4E0F,{0xBA,0xCD,0x76,0x15,0x74,0x37,0x04,0xB1}} */

#pragma code_seg(".orpc")
static const unsigned short IScdMarshal_FormatStringOffsetTable[] =
    {
    0,
    34
    };

static const MIDL_STUBLESS_PROXY_INFO IScdMarshal_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IScdMarshal_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IScdMarshal_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IScdMarshal_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(5) _IScdMarshalProxyVtbl = 
{
    &IScdMarshal_ProxyInfo,
    &IID_IScdMarshal,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IScdMarshal::Open */ ,
    (void *) (INT_PTR) -1 /* IScdMarshal::Close */
};

const CInterfaceStubVtbl _IScdMarshalStubVtbl =
{
    &IID_IScdMarshal,
    &IScdMarshal_ServerInfo,
    5,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IScdExecObject, ver. 0.0,
   GUID={0x426AB2D6,0x83E3,0x47C3,{0x8C,0x85,0xF6,0xDF,0xA7,0x04,0xC0,0x1D}} */

#pragma code_seg(".orpc")
static const unsigned short IScdExecObject_FormatStringOffsetTable[] =
    {
    68,
    102,
    142,
    176,
    216,
    262,
    302,
    354,
    394,
    440
    };

static const MIDL_STUBLESS_PROXY_INFO IScdExecObject_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IScdExecObject_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IScdExecObject_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IScdExecObject_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(13) _IScdExecObjectProxyVtbl = 
{
    &IScdExecObject_ProxyInfo,
    &IID_IScdExecObject,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IScdExecObject::Start */ ,
    (void *) (INT_PTR) -1 /* IScdExecObject::Execute */ ,
    (void *) (INT_PTR) -1 /* IScdExecObject::Stop */ ,
    (void *) (INT_PTR) -1 /* IScdExecObject::QueryTime */ ,
    (void *) (INT_PTR) -1 /* IScdExecObject::QuerySubscriptionTagsRequired */ ,
    (void *) (INT_PTR) -1 /* IScdExecObject::QuerySubscriptionTagsAvailable */ ,
    (void *) (INT_PTR) -1 /* IScdExecObject::ReadSubscriptionData */ ,
    (void *) (INT_PTR) -1 /* IScdExecObject::WriteSubscriptionData */ ,
    (void *) (INT_PTR) -1 /* IScdExecObject::ReadTaggedItems */ ,
    (void *) (INT_PTR) -1 /* IScdExecObject::WriteTaggedItems */
};

const CInterfaceStubVtbl _IScdExecObjectStubVtbl =
{
    &IID_IScdExecObject,
    &IScdExecObject_ServerInfo,
    13,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: _IScdMarshalEvents, ver. 0.0,
   GUID={0xB2C38331,0x751E,0x47C1,{0xB3,0x75,0x4C,0xBD,0xFD,0x0D,0x93,0x8C}} */

#pragma code_seg(".orpc")
static const unsigned short _IScdMarshalEvents_FormatStringOffsetTable[] =
    {
    0
    };

static const MIDL_STUBLESS_PROXY_INFO _IScdMarshalEvents_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &_IScdMarshalEvents_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO _IScdMarshalEvents_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &_IScdMarshalEvents_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(3) __IScdMarshalEventsProxyVtbl = 
{
    0,
    &IID__IScdMarshalEvents,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy
};

const CInterfaceStubVtbl __IScdMarshalEventsStubVtbl =
{
    &IID__IScdMarshalEvents,
    &_IScdMarshalEvents_ServerInfo,
    3,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Standard interface: __MIDL_itf_SysCADMarshal_0117, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: _IScdExecObjectEvents, ver. 0.0,
   GUID={0x4DD2D65E,0xE0F4,0x4B8E,{0xAE,0x8D,0x67,0xF4,0x42,0xAF,0x98,0x69}} */

#pragma code_seg(".orpc")
static const unsigned short _IScdExecObjectEvents_FormatStringOffsetTable[] =
    {
    486
    };

static const MIDL_STUBLESS_PROXY_INFO _IScdExecObjectEvents_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &_IScdExecObjectEvents_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO _IScdExecObjectEvents_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &_IScdExecObjectEvents_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) __IScdExecObjectEventsProxyVtbl = 
{
    &_IScdExecObjectEvents_ProxyInfo,
    &IID__IScdExecObjectEvents,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* _IScdExecObjectEvents::ReadTaggedItems */
};

const CInterfaceStubVtbl __IScdExecObjectEventsStubVtbl =
{
    &IID__IScdExecObjectEvents,
    &_IScdExecObjectEvents_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};

static const MIDL_STUB_DESC Object_StubDesc = 
    {
    0,
    NdrOleAllocate,
    NdrOleFree,
    0,
    0,
    0,
    0,
    0,
    __MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x20000, /* Ndr library version */
    0,
    0x6000169, /* MIDL Version 6.0.361 */
    0,
    UserMarshalRoutines,
    0,  /* notify & notify_flag routine table */
    0x1, /* MIDL flag */
    0, /* cs routines */
    0,   /* proxy/server info */
    0   /* Reserved5 */
    };

const CInterfaceProxyVtbl * _SysCADMarshal_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_IScdMarshalProxyVtbl,
    ( CInterfaceProxyVtbl *) &__IScdMarshalEventsProxyVtbl,
    ( CInterfaceProxyVtbl *) &__IScdExecObjectEventsProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IScdExecObjectProxyVtbl,
    0
};

const CInterfaceStubVtbl * _SysCADMarshal_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_IScdMarshalStubVtbl,
    ( CInterfaceStubVtbl *) &__IScdMarshalEventsStubVtbl,
    ( CInterfaceStubVtbl *) &__IScdExecObjectEventsStubVtbl,
    ( CInterfaceStubVtbl *) &_IScdExecObjectStubVtbl,
    0
};

PCInterfaceName const _SysCADMarshal_InterfaceNamesList[] = 
{
    "IScdMarshal",
    "_IScdMarshalEvents",
    "_IScdExecObjectEvents",
    "IScdExecObject",
    0
};


#define _SysCADMarshal_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _SysCADMarshal, pIID, n)

int __stdcall _SysCADMarshal_IID_Lookup( const IID * pIID, int * pIndex )
{
    IID_BS_LOOKUP_SETUP

    IID_BS_LOOKUP_INITIAL_TEST( _SysCADMarshal, 4, 2 )
    IID_BS_LOOKUP_NEXT_TEST( _SysCADMarshal, 1 )
    IID_BS_LOOKUP_RETURN_RESULT( _SysCADMarshal, 4, *pIndex )
    
}

const ExtendedProxyFileInfo SysCADMarshal_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _SysCADMarshal_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _SysCADMarshal_StubVtblList,
    (const PCInterfaceName * ) & _SysCADMarshal_InterfaceNamesList,
    0, // no delegation
    & _SysCADMarshal_IID_Lookup, 
    4,
    2,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};
#if _MSC_VER >= 1200
#pragma warning(pop)
#endif


#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

