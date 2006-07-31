#ifndef  __KWAVER_H
#define  __KWAVER_H

#define CK_USE6134       1

#if CK_USE6134
#define KWA_VERINFO_V0   2         /* Major Version */
#define KWA_VERINFO_V1   0        /* Minor Version */
#define KWA_VERINFO_V2   0         
#define KWA_VERINFO_V3   0         
#define KWA_VERINFO_VS   "1.0\0"
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
#define KWA_VERINFO_Company       "Kenwalt Pty Ltd\0"
#define KWA_VERINFO_CompanyFull   "Kenwalt Pty Ltd\0"
#define KWA_VERINFO_Copyright     "Copyright © 1997-2006\0"
#define KWA_VERINFO_CopyrightFull "Copyright © Kenwalt Pty Ltd 1997-2006\0"
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


*/