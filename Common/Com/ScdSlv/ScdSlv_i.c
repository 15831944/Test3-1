

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 6.00.0361 */
/* at Tue Mar 07 11:46:01 2006
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

MIDL_DEFINE_GUID(IID, LIBID_ScdSlv,0x9BFC155B,0x7E7A,0x4A3A,0x8C,0x58,0x14,0x20,0x3C,0x0F,0x1E,0x01);


MIDL_DEFINE_GUID(IID, IID_IScdDebug,0x4249F08F,0x233A,0x4EEC,0x90,0x17,0x64,0x1B,0x42,0xD1,0x21,0xF6);


MIDL_DEFINE_GUID(IID, IID_IScdDynamic,0x747D22A6,0xD343,0x4CA6,0x91,0xCE,0x37,0x6E,0xED,0x67,0xC3,0xDB);


MIDL_DEFINE_GUID(IID, IID_IScdMessage,0x93A8521F,0x945A,0x4226,0xBD,0x45,0x52,0xB5,0x8D,0x34,0xF7,0x3A);


MIDL_DEFINE_GUID(IID, IID_IScdMessages,0xED9A26C3,0x3527,0x4d90,0x82,0xAD,0xD8,0xF4,0x74,0x1C,0x24,0x2A);


MIDL_DEFINE_GUID(IID, IID_IScdNode,0x50EB2EAE,0x1D92,0x4BF5,0xBB,0xAE,0xD9,0x2E,0xA5,0xE5,0x4B,0x6E);


MIDL_DEFINE_GUID(IID, IID_IScdNodes,0x530CE54B,0x054D,0x44E9,0x90,0xD7,0xA2,0xDA,0xBF,0x4E,0x57,0xD1);


MIDL_DEFINE_GUID(IID, IID_IScdNodeClass,0x3E92067C,0x548A,0x493E,0xB4,0xC6,0xD4,0xD9,0xCA,0xB6,0x40,0xC0);


MIDL_DEFINE_GUID(IID, IID_IScdConnect,0x1CD389F3,0x67DD,0x4E46,0x85,0xC9,0xC4,0x0A,0x6E,0x49,0xF8,0xD9);


MIDL_DEFINE_GUID(IID, IID_IScdConnects,0x630D2D53,0x76B3,0x41E1,0xA5,0xEF,0x01,0x50,0x1E,0x42,0xFF,0xC8);


MIDL_DEFINE_GUID(IID, IID_IScdConnectPoint,0x128122C7,0xF846,0x4730,0x88,0xE0,0x3F,0xF8,0x84,0x41,0x53,0xA0);


MIDL_DEFINE_GUID(IID, IID_IScdConnectPoints,0x045C8C54,0x8CCF,0x4AB7,0xB5,0x6E,0x9D,0x26,0xCB,0x72,0x5D,0x19);


MIDL_DEFINE_GUID(IID, IID_IScdProbal,0xA36FFC98,0x0C3B,0x4B56,0xBF,0x39,0x01,0x25,0xC9,0x74,0xAF,0x80);


MIDL_DEFINE_GUID(IID, IID_IScdSolver,0x851C57EA,0xF1E5,0x4ff8,0x95,0x18,0x4B,0x91,0xB7,0xCB,0x78,0x30);


MIDL_DEFINE_GUID(IID, IID_IScdSlvTag,0x8EA3D98A,0xAAC9,0x4104,0x8E,0x2C,0xEE,0xF8,0x97,0xFB,0xE0,0x6B);


MIDL_DEFINE_GUID(IID, IID_IScdSlvTags,0x51501760,0x320C,0x4230,0x8B,0x48,0x90,0x8F,0x3A,0x71,0x13,0xB5);


MIDL_DEFINE_GUID(IID, IID_IScdTest,0x19E9AC58,0xF99E,0x4296,0xBF,0x44,0xE3,0xE9,0x94,0xEA,0x09,0x34);


MIDL_DEFINE_GUID(IID, IID_IScdWire,0x37C130B3,0x29B7,0x491A,0xA8,0xA1,0xE8,0xAA,0x54,0x54,0x2C,0x1C);


MIDL_DEFINE_GUID(IID, IID_IScdWires,0x386A7C55,0x991B,0x4D50,0xB1,0x64,0x5B,0xAD,0x1C,0x98,0x0D,0x7D);


MIDL_DEFINE_GUID(IID, IID_IScdTerminal,0x8423BE53,0x50DF,0x4FEA,0xB3,0xD8,0x1F,0xB5,0x25,0x2A,0x2C,0xBA);


MIDL_DEFINE_GUID(IID, IID_IScdTerminals,0x57EFCD3E,0x7581,0x47A3,0x8F,0xA8,0xF0,0x8F,0xCE,0x6A,0xA5,0x6B);


MIDL_DEFINE_GUID(IID, IID_IScdTermStrip,0x68A08D45,0x05DF,0x4463,0xB2,0xEF,0x2D,0xC2,0x83,0x28,0xE9,0x7F);


MIDL_DEFINE_GUID(IID, IID_IScdTermStrips,0x231165C8,0xAF0E,0x41F7,0x93,0xDD,0x8E,0x42,0xFF,0xEB,0x63,0x65);


MIDL_DEFINE_GUID(IID, IID_IScdElectrics,0x8A7205B3,0x3838,0x45D2,0xA3,0xD8,0x5A,0x05,0xF7,0xD7,0xC3,0x14);


MIDL_DEFINE_GUID(IID, IID_IScdComponent,0xA081A069,0x91AA,0x4C5F,0xB5,0x24,0x80,0xB4,0x40,0xCD,0x13,0x8C);


MIDL_DEFINE_GUID(IID, IID_IScdComponents,0x37E46C83,0x1438,0x464B,0xB2,0x7C,0x2B,0xEC,0xFB,0xE4,0xD1,0xB5);


MIDL_DEFINE_GUID(IID, IID_IScdComplex,0xE66B691F,0x32DF,0x4B51,0x8D,0x0B,0x6D,0x5D,0x18,0x06,0x84,0x72);


MIDL_DEFINE_GUID(CLSID, CLSID_ScdMessage,0x95A4F4C8,0xD911,0x4CC5,0xA2,0x41,0x11,0x6B,0xAE,0xAC,0x98,0x92);


MIDL_DEFINE_GUID(CLSID, CLSID_ScdMessages,0x7D916AF7,0xE05D,0x4D62,0xA1,0x9A,0x07,0x87,0x52,0x18,0x30,0x55);


MIDL_DEFINE_GUID(CLSID, CLSID_ScdProbal,0xCEBC2245,0x9DDC,0x4B20,0x8F,0x79,0x83,0x65,0x6A,0xD1,0xD0,0x66);


MIDL_DEFINE_GUID(CLSID, CLSID_ScdSolver,0x6761C370,0x442A,0x4bb3,0xAF,0x4C,0x07,0xBB,0x73,0x53,0xF2,0x3B);


MIDL_DEFINE_GUID(CLSID, CLSID_ScdDebug,0x1591C55C,0xA339,0x42CE,0xBE,0xAF,0xE2,0x86,0x14,0x78,0x42,0xC1);


MIDL_DEFINE_GUID(CLSID, CLSID_ScdTest,0x3784FBD8,0x103D,0x4141,0x91,0xC4,0x61,0x37,0x85,0xF7,0x0F,0x1F);


MIDL_DEFINE_GUID(CLSID, CLSID_ScdSlvTags,0x15366A57,0x3C50,0x498E,0x97,0xE5,0xC7,0x31,0x70,0x6C,0x38,0x61);


MIDL_DEFINE_GUID(CLSID, CLSID_ScdSlvTag,0x64AA6F46,0x2439,0x4C48,0xB0,0x3E,0x25,0xEB,0xE4,0x21,0x28,0xA3);


MIDL_DEFINE_GUID(CLSID, CLSID_ScdDynamic,0x0F708E57,0xEEED,0x47D5,0xBC,0x95,0xBF,0xB9,0x7D,0x24,0xCC,0x15);


MIDL_DEFINE_GUID(CLSID, CLSID_ScdNodes,0xC4FCD11A,0x5909,0x46D7,0xA7,0xFC,0x70,0xBD,0xFF,0xC4,0x56,0x35);


MIDL_DEFINE_GUID(CLSID, CLSID_ScdNode,0x2C170CD9,0x3C2A,0x49D6,0xB5,0xE9,0x3F,0x6A,0xA8,0x03,0xD4,0x80);


MIDL_DEFINE_GUID(CLSID, CLSID_ScdConnect,0x9BF2CD13,0xADB0,0x4D4D,0x96,0x0A,0x73,0x26,0x8F,0xC7,0xA5,0x8F);


MIDL_DEFINE_GUID(CLSID, CLSID_ScdConnectPoint,0x4DFDA6A2,0xC7A3,0x4087,0xA4,0x3C,0xCB,0xCC,0x65,0xC7,0x26,0x50);


MIDL_DEFINE_GUID(CLSID, CLSID_ScdConnects,0xA6FDC0DE,0x3269,0x4B86,0xAC,0x79,0x49,0x3E,0x25,0x18,0x18,0xD2);


MIDL_DEFINE_GUID(CLSID, CLSID_ScdConnectPoints,0xE1CD13B4,0xF7D2,0x46CB,0x9E,0x5F,0xD1,0x73,0xD2,0xC0,0xBA,0xF9);


MIDL_DEFINE_GUID(CLSID, CLSID_ScdNodeClass,0x1ECC6CD5,0x189B,0x4A02,0x8A,0xFE,0x75,0x50,0x28,0x55,0x97,0x7E);


MIDL_DEFINE_GUID(CLSID, CLSID_ScdWire,0x5B9DCE12,0xD825,0x4CBC,0xBE,0x39,0xC9,0xA9,0x8F,0x54,0x48,0x77);


MIDL_DEFINE_GUID(CLSID, CLSID_ScdWires,0x954689B2,0x7310,0x416C,0xB8,0xCB,0x3B,0x2D,0x3F,0x90,0xF3,0x05);


MIDL_DEFINE_GUID(CLSID, CLSID_ScdTerminal,0x7821B605,0xF73F,0x45F5,0xA6,0x5A,0x7A,0x07,0xF4,0x22,0xFF,0x66);


MIDL_DEFINE_GUID(CLSID, CLSID_ScdTerminals,0x68E11902,0xB946,0x4EF3,0x88,0x16,0x21,0x23,0x20,0xDA,0xFC,0x3F);


MIDL_DEFINE_GUID(CLSID, CLSID_ScdTermStrip,0xBF38097F,0xD4C9,0x472D,0xB0,0x5A,0x35,0x4B,0x3C,0x9A,0xF1,0x64);


MIDL_DEFINE_GUID(CLSID, CLSID_ScdTermStrips,0x9DD5CBD4,0x8C0F,0x4C42,0x82,0xA9,0x5C,0xB3,0xC3,0x8D,0x39,0x58);


MIDL_DEFINE_GUID(CLSID, CLSID_ScdElectrics,0x98B8EE21,0x5BB7,0x421F,0x9E,0x36,0x5B,0x7D,0xE0,0x1C,0x07,0xBB);


MIDL_DEFINE_GUID(CLSID, CLSID_ScdComplex,0xC3AAC7AF,0x9E3E,0x427E,0xA6,0xFC,0x84,0xF0,0x55,0x85,0x02,0x71);


MIDL_DEFINE_GUID(CLSID, CLSID_ScdComponent,0x1448A094,0xB17A,0x45A6,0x97,0xF3,0xF4,0x91,0x71,0xB1,0x85,0x31);


MIDL_DEFINE_GUID(CLSID, CLSID_ScdComponents,0x75B7A003,0x8AE1,0x4E70,0x8D,0x39,0xE3,0x5C,0x37,0x51,0x3D,0x58);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

