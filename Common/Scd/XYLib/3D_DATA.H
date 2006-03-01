//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992,1994

#ifndef  __3D_DATA_H
#define  __3D_DATA_H

#ifndef __TRINET_H
#include "trinet.h"
#endif

#ifdef __3D_DATA_CPP
  #define DllImportExport DllExport
#elif !defined(XYLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//============================================================================

DEFINE_TAGOBJ(C3DDataOnly);
class DllImportExport C3DDataOnly : public C3DModel
  {
  DEFINE_SPARES(C3DDataOnly);
  public:
    flag     AllContours;
    TriNet   YTrinet;
    flag     bDoYReFit;

    CCriticalSection m_TheLock;

    C3DDataOnly(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~C3DDataOnly();
    virtual void   CopyModel(pC3DDataOnly pMd);
    virtual void   Clear();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual double Zxy(double Xi, double Yi);
    virtual double Yxz(double Xi, double Zi);
    virtual flag   ReFit();
    virtual void   Build();
    virtual void   Load(FxdEdtInfo &EI, Strng & Str);
    virtual long   Parse(FxdEdtInfo &EI, Strng & Str);
    virtual long   ButtonPushed(FxdEdtInfo &EI, Strng & Str);
  };

// ===========================================================================

#undef DllImportExport

#endif
