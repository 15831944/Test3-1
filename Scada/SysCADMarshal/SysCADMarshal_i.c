

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


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

MIDL_DEFINE_GUID(IID, IID_IScdMarshal,0x057E8131,0xC4D6,0x4E0F,0xBA,0xCD,0x76,0x15,0x74,0x37,0x04,0xB1);


MIDL_DEFINE_GUID(IID, IID_IScdExecObject,0x426AB2D6,0x83E3,0x47C3,0x8C,0x85,0xF6,0xDF,0xA7,0x04,0xC0,0x1D);


MIDL_DEFINE_GUID(IID, IID__IScdMarshalEvents,0xB2C38331,0x751E,0x47C1,0xB3,0x75,0x4C,0xBD,0xFD,0x0D,0x93,0x8C);


MIDL_DEFINE_GUID(IID, IID__IScdExecObjectEvents,0x4DD2D65E,0xE0F4,0x4B8E,0xAE,0x8D,0x67,0xF4,0x42,0xAF,0x98,0x69);


MIDL_DEFINE_GUID(IID, LIBID_SysCADMarshal,0x708DC249,0x6838,0x4822,0xA2,0xD5,0xE4,0x08,0xFD,0x82,0x9B,0x07);


MIDL_DEFINE_GUID(IID, DIID_ISysCADMarshal,0x8DD18AB7,0xD600,0x4085,0x8E,0x4F,0xB3,0x4E,0x51,0xC4,0x4D,0x26);


MIDL_DEFINE_GUID(CLSID, CLSID_Document,0x01E21D76,0xDF1D,0x43DD,0xA2,0xA9,0x6A,0x40,0xC1,0x47,0x65,0xB1);


MIDL_DEFINE_GUID(CLSID, CLSID_ScdMarshal,0x35725B10,0xC090,0x4C48,0xAE,0x31,0xF9,0xC6,0x14,0xC8,0x7C,0xD0);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

