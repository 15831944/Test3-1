#ifndef  __KWAVER_H
#define  __KWAVER_H

#define CK_USE6134       0
#define CK_USE6525       0
#define CK_USE7103       01

#if (CK_USE7103)
#define KWA_VERINFO_V0   2        /* Major Version */
#define KWA_VERINFO_V1   2        /* Minor Version */
#define KWA_VERINFO_V2   0         
#define KWA_VERINFO_V3   0         
#define KWA_VERINFO_VS   "2.2\0"
#elif (CK_USE6134 || CK_USE6525)
#define KWA_VERINFO_V0   2        /* Major Version */
#define KWA_VERINFO_V1   1        /* Minor Version */
#define KWA_VERINFO_V2   0         
#define KWA_VERINFO_V3   0         
#define KWA_VERINFO_VS   "2.1\0"
#else
#define KWA_VERINFO_V0   1         /* Major Version */
#define KWA_VERINFO_V1   0        /* Minor Version */
#define KWA_VERINFO_V2   0         
#define KWA_VERINFO_V3   0         
#define KWA_VERINFO_VS   "1.0\0"
#endif
#define KWA_VERINFO_P0   KWA_VERINFO_V0
#define KWA_VERINFO_P1   KWA_VERINFO_V1
#define KWA_VERINFO_P2   KWA_VERINFO_V2
#define KWA_VERINFO_P3   KWA_VERINFO_V3
#define KWA_VERINFO_PS   KWA_VERINFO_VS
#define KWA_VERINFO_Company       "KWA Kenwalt Australia Pty Ltd\0"
#define KWA_VERINFO_CompanyFull   "KWA Kenwalt Australia Pty Ltd\0"
#define KWA_VERINFO_Copyright     "Copyright © 1997-2008\0"
#define KWA_VERINFO_CopyrightFull "Copyright © SysCAD 1997-2008\0"
#define KWA_VERINFO_Legal         "-\0"

#endif // __KWAVER_H

//===========================================================================
/*

Revision History
================

1.0 : 11 May 2006
--------------------------
1) First release of syscad.exe license utility for SysCAD9.1
2) Using CrypKey 6.1

2.1 : ???
--------------------------
1) Upgrade to CrypKey 6.5

2.2 : 30 January 2008
--------------------------
1) Upgrade to CrypKey 7.1
*/