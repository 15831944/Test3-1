//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __TAGDB_H
#define  __TAGDB_H

#ifndef __DATACNVS_H
  #include "datacnvs.h"
#endif

#ifndef __EXECLIB_H
  #include "execlib.h"
#endif

#ifndef __DBHELPER_H
  #include "dbhelper.h"
#endif

#ifdef __TAGDB_CPP
  #define DllImportExport DllExport
#elif !defined(SCEXEC)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

#define MDBName         "Scd_Mdls.Mdb"
#define DefaultCfgLabel "Default"

//extern DllImportExport const char const * PlantModelTag;

//const DWORD DBO_Fmts            = 0x0000ffff;
//const DWORD DBO_Find            = 0x00008000;

const DWORD DBO_ForRead         = 0x00000001;
const DWORD DBO_ForWrite        = 0x00000002;
                                          
const DWORD DBO_DoDBData        = 0x00000010;
const DWORD DBO_DoOtherData     = 0x00000020;
const DWORD DBO_DoAllData       = (DBO_DoDBData|DBO_DoOtherData);
                                    
const DWORD DBO_CopyOldDBFiles  = 0x00000100;
const DWORD DBO_RemovePrevRevs  = 0x00000200;
const DWORD DBO_CompressDB      = 0x00000400;

const DWORD DBRev_MostRecent    = 0x00000001;

// --------------------------------------------------------------------------

/*This class represents the tagged database.*/
class CTagDBRt
  {
  protected:
    DWORD            m_OpenOpts; 

  public:
    CTagDBRt() { m_OpenOpts=0;};
    virtual ~CTagDBRt() {};
    virtual flag     IsOpen()=0;
    virtual int      Open(long DBFmt, DWORD DBOpts, pchar pRoot, pchar pRootOld, pchar pOldName)=0;
    virtual int      Flush(flag bOnlyChanged)=0;
    virtual int      CloseIt()=0;
    virtual void     SetChanged(flag Changed)=0;

    virtual long     FindRevisionNo(DWORD Options)=0;
    virtual long     BumpRevisionNo()=0;
    virtual long     GetRevisionNo()=0;
    virtual long     SetRevisionNo(long RevNo)=0;
    
    virtual void     StartTransaction()=0;
    virtual void     StartSaveSequence(pchar pObjType)=0;
    virtual int      SaveObject(pchar pObjType, CXM_ObjectData *ObjData)=0;
    virtual void     EndTransaction(BOOL Commit)=0;
    virtual void     SetHoldLocalTransactions(BOOL Hold)=0;

    virtual void     StartLoadSequence(pchar pObjType)=0;
    virtual int      LoadObject(CXM_ObjectData *pObjData, flag CreateOnly=False)=0;

    virtual int      LoadObject(pchar Tag, CXM_ObjectData *pObjData)=0;

    virtual void     RemoveRevisions(int From, int To)=0;

  };

// --------------------------------------------------------------------------

class DllImportExport CTagDBase
  {
  public:
    CTagDBRt      *pDB;
    long          m_nObjectsSaved;
    static Strng   sOldFlwLibLocation;
    DWORD          m_OpenOpts; 

  public:
    CTagDBase();
    ~CTagDBase();
    flag           IsOpen();
    int            Open(long DBFmt, DWORD DBOpts, pchar pRoot, pchar pRootOld, pchar pOldName);
    int            Flush(flag bOnlyChanged);
    int            Close();
    void           SetChanged(flag Changed);

    long           FindRevisionNo(DWORD Options);
    long           BumpRevisionNo();

    void           StartSaveSequence(pchar pObjType);
    int            SaveObject(pchar pObjType, CXM_ObjectData *ObjData);

    void           StartLoadSequence(pchar pObjType);
    int            LoadObject(CXM_ObjectData *pObjData, flag CreateOnly=False);

    int            LoadObject(pchar Tag, CXM_ObjectData *pObjData);

    static void    EscapeFlds(Strng &Tag);
    static void    UnEscapeFlds(Strng &Tag);
    static void    EscapeStructs(Strng &Tag);
    static void    UnEscapeStructs(Strng &Tag);
  };

// ==========================================================================
//
//
//
// ==========================================================================

#undef DllImportExport

#endif
