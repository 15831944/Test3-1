//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#ifndef  __SCDVER_H
#define  __SCDVER_H

//===========================================================================
//  Compile Options
//===========================================================================

#define BLDDEPENDENTFILES     1
#define WITHCOMCMD            0
#define WITHMULTIFLWBLK       1
#define WITHTWOPHASE          0
#define WITH_H2O_FNS_ALWAYS   1

//Water model selection...
#define WithSatH2OMdl         0
#define WithIAFH2OMdl         0
#define WithNBSH2OMdl         0
#define WithIF97H2OMdl        1
#define WithHFC134a           1

//new stuff "always" enabled...
#define WITHZIP               1
#define WITHOPC               1
#define WITHXREFUPDATES       1
#define WITHCTRLOBJ           1
#define WITHCTRLEDIT          1
#define WITHORDEROBJ          1
#define WITHIOOBJ             1
#define WITHXMENUS            1
#define WITHDOUBLEGSMODEL     1
#define WITHSURGECTRLDOUBLE   1
#define WITHGRFDOCFRAME       1
#define WITHAUTOVLE           1
#define WITHGRFGROUP          1
#define WITHGRFGROUPCONFIG    1 
#define WITHTAGMONITOR        (WITHGRFGROUPCONFIG)
#define WithSimpleEvap        1
#define WithSimpleBleed       0  /*not ready for release*/
#define WITHNODETAGIO         1  /*currently cannot be switched off due to link errors*/
#define WITHNODEPROCS         10  /*not ready for release*/

#define USEREGISTRY           0

//perhaps developing new functionality...
#define WITHCHANGEOBJ         0 //too slow...???
#define WITHAUTOMATION        0
#define WITHRCTEDITDLG        0
#define WITHDOCUMENTATIONLINK 0  /*not implemented*/

//discontinued functionality...
#define WITHLOCALCOPY         0
#define WITHDEFAULTSPDB       0
#define WITHDDEREPORTS        0
#define WITHSCRCYCLES         0 // for Ken at machodorp
#define WITHSPECSHEETS        0
#define WITHNETSERVER         0
#define WITHVISIODOC          0
#define WITHIMPORTPDS         0
#define WITHANALYSE           0
#define WITHDRVMAN            0

//#include "..\..\..\SMDK\include\md_share1.h"

// ========================================================================

#ifdef _DEBUG
#define _MAKENAME "Debug"
#elif  _RELEASE
#define _MAKENAME "Release"
#else
#define _MAKENAME "Dbg_Release"
#endif

// --------------------------------------------------------------------------
//information when a Build is released
#define SCD_BUILD_SVN    1489                 /* SVN when Build is first released */
#define SCD_BUILD_NUM    123                  /* Build number*/
#define SCD_BUILD_DATE   "20 February 2007"   /* Date when Build is first released */

//compile information
#define SCD_COMPILE_DT   __DATE__
#define SCD_COMPILE_DTTM  __DATE__ " " __TIME__   
#define SCD_COMPILE_TS   __TIMESTAMP__        

//Version number
#define SCD_VERINFO_V0   9                    /* Major Version */
#define SCD_VERINFO_V1   1                    /* Minor Version */
#define SCD_VERINFO_V2   SCD_BUILD_NUM        /* Incompatible Version, ie check these numbers match in DLLs*/
#define SCD_VERINFO_V3   SCD_BUILD_SVN        /* Compatible Version - idealy this should change every time a version is issued, but for now use svn number when build is created*/

// --------------------------------------------------------------------------

//Version numbers used in rc2 files
#define SCD_VERINFO_P0   SCD_VERINFO_V0
#define SCD_VERINFO_P1   SCD_VERINFO_V1
#define SCD_VERINFO_P2   SCD_VERINFO_V2
#define SCD_VERINFO_P3   SCD_VERINFO_V3

//Version strings
#define VERSTR(x)    #x
#define VERSTR2(x)  VERSTR(x)

#define SCD_VERINFO_VS_X(A,B,C,D,E) VERSTR2(A) "." VERSTR2(B) " Build:" VERSTR2(C) " Rev:" VERSTR2(D) " " E "[.Net]\0"

#ifdef _DEBUG
#define SCD_VERINFO_VS            SCD_VERINFO_VS_X(SCD_VERINFO_V0, SCD_VERINFO_V1, SCD_VERINFO_V2, SCD_BUILDNO, "Full Debug")
#elif  _RELEASE
#define SCD_VERINFO_VS            SCD_VERINFO_VS_X(SCD_VERINFO_V0, SCD_VERINFO_V1, SCD_VERINFO_V2, SCD_BUILDNO, "")
#else
#define SCD_VERINFO_VS            SCD_VERINFO_VS_X(SCD_VERINFO_V0, SCD_VERINFO_V1, SCD_VERINFO_V2, SCD_BUILDNO, "Release Debug")
#endif

#define SCD_VERINFO_PS_X(A,B,C)   VERSTR2(A) "." VERSTR2(B) "." VERSTR2(C) "\0"
#define SCD_VERINFO_PS            SCD_VERINFO_PS_X(SCD_VERINFO_P0, SCD_VERINFO_P1, SCD_VERINFO_P2)

//More version information used in RC files, etc
#define SCD_VERINFO_Company       "Kenwalt\0"
#define SCD_VERINFO_CompanyFull   "Kenwalt Pty Ltd\0"
#define SCD_VERINFO_Copyright     "Copyright © 1995-2007\0"
#define SCD_VERINFO_CopyrightFull "Copyright © Kenwalt 1995-2007\0"
#define SCD_VERINFO_Legal         "-\0"
#define SCD_VERINFO_Company2      "KWA Kenwalt\0"
#define SCD_VERINFO_Company2Full  "KWA Kenwalt Australia Pty Ltd\0"


#if (NextVersion && SCD_VERINFO_V1==0)
#error "SCD_VERINFO_V1 needs to be bumped"
#endif

#endif // __SCDVER_H

//===========================================================================
//===========================================================================
/*

Revision History moved to revhist.h

*/
//===========================================================================



