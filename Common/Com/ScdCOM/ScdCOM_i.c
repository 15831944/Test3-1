

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 6.00.0361 */
/* at Wed Mar 08 09:24:04 2006
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

#if !defined(_M_IA64) && !defined(_M_AMD64)


#pragma warning( disable: 4049 )  /* more than 64k source lines */


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_ISysCADCmd,0xF0A8E28B,0xC450,0x4F11,0xB9,0x6B,0x7A,0xA6,0x2B,0x44,0x5D,0x36);


MIDL_DEFINE_GUID(IID, IID_ISysCADEdit,0x648529D8,0x8872,0x44e4,0x8A,0x34,0xE5,0x94,0x49,0xD6,0x83,0x2F);


MIDL_DEFINE_GUID(IID, IID_IBackdoor,0x7837875f,0x63e2,0x4642,0xaf,0x7c,0xdf,0x1b,0xa7,0xff,0x10,0xa4);


MIDL_DEFINE_GUID(IID, LIBID_SysCADLib,0x700C6795,0x4F08,0x48D6,0x96,0x0A,0x88,0x77,0x3A,0x56,0xE8,0x77);


MIDL_DEFINE_GUID(IID, DIID__ISysCADCmdEvents,0x43933726,0x54B7,0x4DCB,0x82,0xB2,0xF4,0xE6,0x30,0x71,0x94,0x23);


MIDL_DEFINE_GUID(CLSID, CLSID_SysCADCmd,0x1E7E6A3F,0x9107,0x40E7,0x96,0x16,0x65,0xEE,0xE2,0xCC,0xCE,0x06);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

