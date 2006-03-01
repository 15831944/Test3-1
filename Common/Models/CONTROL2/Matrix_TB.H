//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992,1994

#ifndef  __Matrix_TB_H
#define  __Matrix_TB_H

#ifndef __DATACNVS_H
#include "datacnvs.h"
#endif  
#ifndef __FIXEDIT_H
#include "fixedit.h"
#endif  
#ifndef __TAGOBJ_H
#include "tagobj.h"
#endif  
#ifndef __VECTORS_H
#include "vectors.h"
#endif
//#ifndef __BASEMDL_H
//#include "basemdl.h"
//#endif

//#ifndef Z_MAT_H
//#include "ZMat.h"
//#endif

#include "Optlib\OptLib.h"


#ifdef __Matrix_TB_CPP
  #define DllImportExport DllExport
#elif !defined(Control2)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ===========================================================================

_FWDDEF(CMatrixTB);
DEFINE_TAGOBJEDT(CMatrixTB);

//----------------------------------------------------------------------------

typedef CArray <Strng, Strng> StrngArray;

class DllImportExport CMatrixTB : public FxdEdtBookRef, public TaggedObject, public ZMat
  {
  protected:

    bool               m_bViewOnly; // Whether Matrix is Viewable Only or Editable
    StrngArray         RowNames;
    StrngArray         ColNames;

  public:
    CMatrixTB(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach);
    CMatrixTB( pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach, bool bviewonly = true );
//    CMatrixTB( pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach );
    CMatrixTB(CMatrixTB& Fn);
    virtual ~CMatrixTB();
    void Clear();
    CMatrixTB & operator=(CMatrixTB& Fn) { return *this; }; // Need work here to copy dynamic contents

  protected:
 
  public:

  protected:
 
  public:

    virtual void   BuildDataDefn(DataDefnBlk &DDB);
    virtual flag   DataXchg(DataChangeBlk &DCB);
    virtual void   Build();
    virtual void   Load(FxdEdtInfo &EI, Strng & Str);
    virtual long   Parse(FxdEdtInfo &EI, Strng & Str);
    virtual long   ButtonPushed(FxdEdtInfo &EI, Strng & Str);

    void SetPrefix(pchar rdesc, pchar cdesc );
    void SetRowPrefix(pchar rdesc  );
    void SetColPrefix(pchar cdesc  );
    void SetRowName(long row , pchar name );
    void SetColName(long col , pchar name );
    void SetDim(int RowCount,int ColCount);

  };

// ===========================================================================

#undef DllImportExport

#endif

