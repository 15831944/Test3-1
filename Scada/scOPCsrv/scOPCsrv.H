//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __SCOPCSRV_H
#define  __SCOPCSRV_H

//=========================================================================

_FWDDEF(COPCSlot)
_FWDDEF(COPCDriver)

#include "SCDTEMPL.h"
#include "Drv_Base.h"

class OPCServerInfo
{
public:
    OPCServerInfo( LPOLESTR ProgID, LPOLESTR Description, CLSID& clsid )
        : m_ProgID(ProgID), m_Description(Description), m_clsid(clsid) {}
    CString     m_ProgID;
    CString     m_Description;
    CLSID       m_clsid;
};

typedef CTypedPtrList<CPtrList, OPCServerInfo*> OPCServerList;

BOOL GetOPCServers(LPTSTR Node, CLSID Catid, OPCServerList &Servers);

//=========================================================================

#endif
