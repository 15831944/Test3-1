//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#ifndef  __SCDVER_H
#define  __SCDVER_H

//===========================================================================
//  Compile Options
//===========================================================================

// License Options moved to ScdVerLic.h

// --------------------------------------------------------------------------

#define USESCDEXPLORER        1
#define BLDDEPENDENTFILES     1
#define WITHCOMCMD            0
#define WITHMULTIFLWBLK       1
#define WITHTWOPHASE          0
#define WITH_H2O_FNS_ALWAYS   1

#define SYSCAD10              0
#define CREATENETSERVER       (1 && SYSCAD10)

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
#define USEREGISTRY           0

#define WITHBLOCKEVALUATOR    (1 && (_MSC_VER>=1400))

//perhaps developing new functionality...
#define WITHCHANGEOBJ         0 //too slow...???
#define WITHAUTOMATION        0
#define WITHRCTEDITDLG        0

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

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++ Used for changing SysCAD version numbers.           ++
// ++ See BuildVer.h for build numbers.                   ++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#include "..\..\..\SMDK\include\md_share1.h"

#if (NextVersion && SCD_VERINFO_V1==0)
#error "SCD_VERINFO_V1 needs to be bumped"
#endif

#define VERSTR(x)    #x
#define VERSTR2(x)  VERSTR(x)

#define SCD_VERINFO_VS_X(A,B,C,D,E) VERSTR2(A) "." VERSTR2(B) "." VERSTR2(C) " (Build " VERSTR2(D) ") " E "[.Net]\0"

#ifdef _DEBUG
#define SCD_VERINFO_VS            SCD_VERINFO_VS_X(SCD_VERINFO_V0, SCD_VERINFO_V1, SCD_VERINFO_V2, SCD_BUILDNO, "Full Debug")
#elif  _RELEASE
#define SCD_VERINFO_VS            SCD_VERINFO_VS_X(SCD_VERINFO_V0, SCD_VERINFO_V1, SCD_VERINFO_V2, SCD_BUILDNO, "")
#else
#define SCD_VERINFO_VS            SCD_VERINFO_VS_X(SCD_VERINFO_V0, SCD_VERINFO_V1, SCD_VERINFO_V2, SCD_BUILDNO, "Release Debug")
#endif

#define SCD_VERINFO_P0   SCD_VERINFO_V0
#define SCD_VERINFO_P1   SCD_VERINFO_V1
#define SCD_VERINFO_P2   SCD_VERINFO_V2
#define SCD_VERINFO_P3   SCD_VERINFO_V3

#define SCD_VERINFO_PS_X(A,B,C)   VERSTR2(A) "." VERSTR2(B) "." VERSTR2(C) "\0"
#define SCD_VERINFO_PS            SCD_VERINFO_PS_X(SCD_VERINFO_P0, SCD_VERINFO_P1, SCD_VERINFO_P2)

#define SCD_VERINFO_Company       "Kenwalt\0"
#define SCD_VERINFO_CompanyFull   "Kenwalt Pty Ltd\0"
#define SCD_VERINFO_Copyright     "Copyright © 1995-2006\0"
#define SCD_VERINFO_CopyrightFull "Copyright © Kenwalt 1995-2006\0"
#define SCD_VERINFO_Legal         "-\0"
#define SCD_VERINFO_Company2      "KWA Kenwalt\0"
#define SCD_VERINFO_Company2Full  "KWA Kenwalt Australia Pty Ltd\0"

#endif // __SCDVER_H

//===========================================================================
//===========================================================================
/*

Revision History moved to revhist.h

*/
//===========================================================================



