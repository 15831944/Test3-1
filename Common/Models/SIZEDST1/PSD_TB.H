//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992,1994

#ifndef  __PSD_TB_H
#define  __PSD_TB_H

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
#ifndef  __PSD_H
#include "psd.h"
#endif

#include "SP_DB.h"
#include "sqszbase.h"

#ifdef __PSD_TB_CPP
  #define DllImportExport DllExport
#elif !defined(SizeDst1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ===========================================================================

_FWDDEF(CPSDTB);
//DEFINE_TAGOBJEDT(CPSDTB);

//----------------------------------------------------------------------------

typedef CArray <Strng, Strng> StrngArray;

class DllImportExport CPSDTB //: /*public FxdEdtBookRef,*/ public TaggedObject
  {
  //protected:
  public:

    CPSDTestDataArray m_PSDTestData; // Array of Test Data for each species
    CSD_DistDefn*     m_pD;          // Reference to the Distribution Data

    int               Id_Offset;     // Offset from which Edit Object IDs are added
    int               XID_Offset;    // Offset from which Build Data Defn/ Data XChng IDs are added

  public:
    CPSDTB(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach  );
    CPSDTB( pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach  );
    CPSDTB(CPSDTB& Fn);
    CPSDTB();
    virtual ~CPSDTB();
    void Clear();
    CPSDTB & operator=(CPSDTB& Fn) { return *this; };

  protected:
 
  public:

  protected:
 
  public:

    virtual void   BuildDataDefn(DataDefnBlk &DDB, TaggedObject* TagObj);
    virtual flag   DataXchg(DataChangeBlk &DCB);
    virtual void   Build(FxdEdtBookRef* pFxdEdt);
    virtual void   Load (FxdEdtBookRef* pFxdEdt,  FxdEdtInfo &EI, Strng & Str);
    virtual long   Parse(FxdEdtBookRef* pFxdEdt,  FxdEdtInfo &EI, Strng & Str);
    virtual long   ButtonPushed(FxdEdtBookRef* pFxdEdt, FxdEdtInfo &EI, Strng & Str);

    CnvAttribute*  GetCnv( long lFieldID );
    void           SetIDOffsets(int Id, int XID );
    void           SetSpecies(long N, const Strng *Names);
    void           SetDistribution(CSD_DistDefn   *m_pD);

  };


// ===========================================================================

#undef DllImportExport

#endif

