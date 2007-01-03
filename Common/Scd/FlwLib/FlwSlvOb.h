//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __FLWSLVOB_H
#define  __FLWSLVOB_H

#ifndef __AFXTEMPL_H__
  #include <afxtempl.h> // "xafxtempl.h"
#endif
#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __STDIO_H
  #include <stdio.h>
#endif
#ifndef __FLWNETS_H
  #include "FlwNets.h"
#endif
#ifndef __ODESOLVE_H
  #include "odesolve.h"
#endif
#ifndef __EXECUTIV_H
  #include "executiv.h"
#endif
#ifndef __FIXEDIT_H
  #include "fixedit.h"
#endif  
//#ifndef __GPWNDS_H
//  #include "gpwnds.h"
//#endif  
#ifndef __DATAATTR_H
  #include "dataattr.h"
#endif  
#ifndef __CTRLSEQ_H
  #include "ctrlseq.h"
#endif  
#ifndef __TAGDB_H
  #include "tagdb.h"
#endif  

#ifdef __FLWSLVOB_CPP
  #define DllImportExport DllExport
#elif !defined(FLWLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

#include "scdver.h"  // WITHCTRLOBJ/EDIT

// ==========================================================================
//
//
//
// ==========================================================================

XID xidATearTag          = FlwXID(10000);

// ==========================================================================
//
//
//
// ==========================================================================

DEFINE_TAGOBJEDT(SDBObject)
class SDBObject : public CTNode
  {
  public:
    double m_dDisplayT;
    double m_dDisplayP;
    flag   m_bShowMs;
    flag   m_bHiFidelity;

    //long   m_iFidelity;

    SDBObject(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~SDBObject();

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual pchar  TagOfParent() { return PlantModelTag; };
  };

// ===========================================================================

_FWDDEF(SDBObjectEdt);
class DllImportExport SDBObjectEdt : public FxdEdtBookRef
  {
  public :
    SDBObject    & rSDBO;
    int            iNameWidth;
    int            iPg1;
    int            iPgExtraProps;
    int            iSpBlkCnt;
    int            iSpBlkLen[MaxSpecies+1];
    CnvAttribute   TCnv, PCnv, CpCnvMl, HCnvMl, SCnvMl, CpCnvMs, HCnvMs, SCnvMs, RhoCnv, VpCnv, VtCnv;
    FmtAttribute   MlFmt, TFmt, PFmt, CpFmt, HFmt, SFmt, RhoFmt, VpFmt, VtFmt;
    FmtAttribute   ElFmt;//shared Elemental MoleWt Fmt
    FmtAttribute   EPFmt;//shared ExtraProps Fmt

    int DistFromPgNo(int Pg);

    SDBObjectEdt(pFxdEdtView pView_, pSDBObject pSDBO_);//, rStrng Tag, rStrng Name);
    virtual ~SDBObjectEdt();

    virtual void PutDataStart();
    virtual void PutDataDone();
    virtual void GetDataStart();
    virtual void GetDataDone();

    virtual void StartBuild();
    virtual void Build();
    virtual void Load(FxdEdtInfo &EI, Strng & Str);
    virtual long ButtonPushed(FxdEdtInfo &EI, Strng & Str);
    virtual long Parse(FxdEdtInfo &EI, Strng & Str);

    virtual flag DoLButtonDown(UINT nFlags, CPoint point);
    virtual flag DoLButtonUp(UINT nFlags, CPoint point);
    virtual flag DoLButtonDblClk(UINT nFlags, CPoint point);
    virtual flag DoRButtonDown(UINT nFlags, CPoint point);
    virtual flag DoRButtonUp(UINT nFlags, CPoint point);
    virtual flag DoRButtonDblClk(UINT nFlags, CPoint point);
    virtual flag DoAccCnv(UINT Id);
    virtual flag DoAccFmt(UINT Id);
    virtual flag DoAccRptTagLists();
  };

// ==========================================================================
//
//
//
// ==========================================================================

DEFINE_TAGOBJEDT(TearObject)
//DEFINE_TAGOBJ(TearObject)
class TearObject : public CTNode
  {
  public:
    TearObject(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~TearObject();

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual pchar  TagOfParent() { return PlantModelTag; };
  };

// ===========================================================================

_FWDDEF(TearObjectEdt);
class DllImportExport TearObjectEdt : public FxdEdtBookRef
  {
  protected:
    static int     m_iSlct;

    TearObject &   rTearO;
    CnvAttribute   TCnv, DCnv;
    FmtAttribute   TFmt, DFmt;
    
    CArray <TearVarBlk*, TearVarBlk*> m_Blks;

    int            m_iSlctPg1;
    TearVarBlk    *m_pSlctTVB;

    int DistFromPgNo(int Pg);

  public:
    TearObjectEdt(pFxdEdtView pView_, pTearObject pChgO_);
    virtual ~TearObjectEdt();

    virtual void PutDataStart();
    virtual void PutDataDone();
    virtual void GetDataStart();
    virtual void GetDataDone();

    virtual void StartBuild();
    virtual void Build();
    virtual void Load(FxdEdtInfo &EI, Strng & Str);
    virtual long ButtonPushed(FxdEdtInfo &EI, Strng & Str);
    virtual long Parse(FxdEdtInfo &EI, Strng & Str);

    virtual flag DoLButtonDown(UINT nFlags, CPoint point);
    virtual flag DoLButtonUp(UINT nFlags, CPoint point);
    virtual flag DoLButtonDblClk(UINT nFlags, CPoint point);
    virtual flag DoRButtonDown(UINT nFlags, CPoint point);
    virtual flag DoRButtonUp(UINT nFlags, CPoint point);
    virtual flag DoRButtonDblClk(UINT nFlags, CPoint point);
    virtual flag DoAccCnv(UINT Id);
    virtual flag DoAccFmt(UINT Id);
    virtual flag DoAccRptTagLists();
  };

// ==========================================================================
//
//
//
// ==========================================================================

#if WITHCHANGEOBJ
const int MaxChgStats = 50;

DEFINE_TAGOBJEDT(ChgObject)
class ChgObject : public CTNode
  {
  friend class CFlwSolver;
  friend class ChgObjectEdt;

  protected:
    long            nNumStats;
    flag            fBlocked;
    flag            fAllowCollection;
    double          dShowTol;
    double          dSmoothing;

    static CSCDList <CTraceArray*, CTraceArray*> ChgStatsArrayList;
    static CArray <CTraceItem*, CTraceItem*> TheStats;
    static CArray <CTraceItem*, CTraceItem*> SortedErrStats;
    static CArray <CTraceItem*, CTraceItem*> SortedChgStats;
    static CArray <CTraceItem*, CTraceItem*> SortedOscStats;
    
    static long iTheStats;
  
  public:
    ChgObject(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~ChgObject();

    virtual void    BuildDataDefn(DataDefnBlk & DDB);
    virtual flag    DataXchg(DataChangeBlk & DCB);
    virtual flag    ValidateData(ValidateDataBlk & VDB);
    virtual pchar   TagOfParent() { return PlantModelTag; };

    flag            AllowCollection() { return fAllowCollection; };
    void            StartAddition();
    int             DoAddition(dword Flags, CTraceArray * pSA);
    void            DoSort();
  };

// ===========================================================================

_FWDDEF(ChgObjectEdt);
class DllImportExport ChgObjectEdt : public FxdEdtBookRef
  {
  public :
    ChgObject    & rChgO;
    int            iPg1;
    
    int DistFromPgNo(int Pg);

    ChgObjectEdt(pFxdEdtView pView_, pChgObject pChgO_);//, rStrng Tag, rStrng Name);
    virtual ~ChgObjectEdt();

    virtual void PutDataStart();
    virtual void PutDataDone();
    virtual void GetDataStart();
    virtual void GetDataDone();

    virtual void StartBuild();
    virtual void Build();
    virtual void Load(FxdEdtInfo &EI, Strng & Str);
    virtual long ButtonPushed(FxdEdtInfo &EI, Strng & Str);
    virtual long Parse(FxdEdtInfo &EI, Strng & Str);

    virtual flag DoLButtonDown(UINT nFlags, CPoint point);
    virtual flag DoLButtonUp(UINT nFlags, CPoint point);
    virtual flag DoLButtonDblClk(UINT nFlags, CPoint point);
    virtual flag DoRButtonDown(UINT nFlags, CPoint point);
    virtual flag DoRButtonUp(UINT nFlags, CPoint point);
    virtual flag DoRButtonDblClk(UINT nFlags, CPoint point);
    virtual flag DoAccCnv(UINT Id);
    virtual flag DoAccFmt(UINT Id);
    virtual flag DoAccRptTagLists();
  };
#endif

// ==========================================================================
//
//
//
// ==========================================================================

DEFINE_TAGOBJEDT(TolObject)
class TolObject : public CTNode
  {
  friend class CFlwSolver;
  friend class TolObjectEdt;

  protected:
  public:
    TolObject(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~TolObject();

    virtual void    BuildDataDefn(DataDefnBlk & DDB);
    virtual flag    DataXchg(DataChangeBlk & DCB);
    virtual flag    ValidateData(ValidateDataBlk & VDB);
    virtual pchar   TagOfParent() { return PlantModelTag; };

  };

// ===========================================================================

_FWDDEF(TolObjectEdt);
class DllImportExport TolObjectEdt : public FxdEdtBookRef
  {
  public :
    TolObject    & rTolO;
    int            iPg1;
    CnvAttribute   TCnv, MCnv;
    FmtAttribute   TFmt, MFmt;
    CArray <CToleranceBlock*, CToleranceBlock*> m_Blks;

    int DistFromPgNo(int Pg);

    TolObjectEdt(pFxdEdtView pView_, pTolObject pChgO_);
    virtual ~TolObjectEdt();

    virtual void PutDataStart();
    virtual void PutDataDone();
    virtual void GetDataStart();
    virtual void GetDataDone();

    virtual void StartBuild();
    virtual void Build();
    virtual void Load(FxdEdtInfo &EI, Strng & Str);
    virtual long ButtonPushed(FxdEdtInfo &EI, Strng & Str);
    virtual long Parse(FxdEdtInfo &EI, Strng & Str);

    virtual flag DoLButtonDown(UINT nFlags, CPoint point);
    virtual flag DoLButtonUp(UINT nFlags, CPoint point);
    virtual flag DoLButtonDblClk(UINT nFlags, CPoint point);
    virtual flag DoRButtonDown(UINT nFlags, CPoint point);
    virtual flag DoRButtonUp(UINT nFlags, CPoint point);
    virtual flag DoRButtonDblClk(UINT nFlags, CPoint point);
    virtual flag DoAccCnv(UINT Id);
    virtual flag DoAccFmt(UINT Id);
    virtual flag DoAccRptTagLists();
  };

// ==========================================================================
//
//
//
// ==========================================================================

#if WITHCTRLOBJ

#if WITHCTRLEDIT
DEFINE_TAGOBJEDT(CtrlObject)
#else
DEFINE_TAGOBJ(CtrlObject)
#endif
class CtrlObject : public CTNode
  {
  friend class CFlwSolver;
  friend class CtrlObjectEdt;

  protected:

  public:
    CtrlObject(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CtrlObject();

    virtual void    PrepareDefinedData(); // Called Before a GetDefined Data - for the root object
    virtual void    BuildDataDefn(DataDefnBlk & DDB);
    virtual flag    DataXchg(DataChangeBlk & DCB);
    virtual flag    ValidateData(ValidateDataBlk & VDB);
    virtual pchar   TagOfParent() { return PlantModelTag; };

  };

// ===========================================================================

#if WITHCTRLEDIT

_FWDDEF(CtrlObjectEdt);
class DllImportExport CtrlObjectEdt : public FxdEdtBookRef
  {
  public :
    CtrlObject    & rChgO;
    static long     m_iView;
    int             iPg1;
    
    //CtrlSeqBlkPtrArray m_Ctrls;

    int DistFromPgNo(int Pg);

    CtrlObjectEdt(pFxdEdtView pView_, pCtrlObject pChgO_);
    virtual ~CtrlObjectEdt();

    virtual void PutDataStart();
    virtual void PutDataDone();
    virtual void GetDataStart();
    virtual void GetDataDone();

    virtual void StartBuild();
    virtual void Build();
    virtual void Load(FxdEdtInfo &EI, Strng & Str);
    virtual long ButtonPushed(FxdEdtInfo &EI, Strng & Str);
    virtual long Parse(FxdEdtInfo &EI, Strng & Str);

    virtual flag DoLButtonDown(UINT nFlags, CPoint point);
    virtual flag DoLButtonUp(UINT nFlags, CPoint point);
    virtual flag DoLButtonDblClk(UINT nFlags, CPoint point);
    virtual flag DoRButtonDown(UINT nFlags, CPoint point);
    virtual flag DoRButtonUp(UINT nFlags, CPoint point);
    virtual flag DoRButtonDblClk(UINT nFlags, CPoint point);
    virtual flag DoAccCnv(UINT Id);
    virtual flag DoAccFmt(UINT Id);
    virtual flag DoAccRptTagLists();
  };

#endif
#endif

// ==========================================================================
//
//
//
// ==========================================================================

#if WITHORDEROBJ

DEFINE_TAGOBJEDT(OrderObject)
class OrderObject : public CTNode
  {
  friend class CFlwSolver;
  friend class OrderObjectEdt;

  protected:
  
  public:
    OrderObject(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~OrderObject();

    virtual void    BuildDataDefn(DataDefnBlk & DDB);
    virtual flag    DataXchg(DataChangeBlk & DCB);
    virtual flag    ValidateData(ValidateDataBlk & VDB);
    virtual pchar   TagOfParent() { return PlantModelTag; };
  };

// ===========================================================================

_FWDDEF(OrderObjectEdt);
class DllImportExport OrderObjectEdt : public FxdEdtBookRef
  {
  public :
    OrderObject  & rOrdO;
    int            iPg1;
    
    int DistFromPgNo(int Pg);

    OrderObjectEdt(pFxdEdtView pView_, pOrderObject pChgO_);//, rStrng Tag, rStrng Name);
    virtual ~OrderObjectEdt();

    virtual void PutDataStart();
    virtual void PutDataDone();
    virtual void GetDataStart();
    virtual void GetDataDone();

    virtual void StartBuild();
    virtual void Build();
    virtual void Load(FxdEdtInfo &EI, Strng & Str);
    virtual long ButtonPushed(FxdEdtInfo &EI, Strng & Str);
    virtual long Parse(FxdEdtInfo &EI, Strng & Str);
    virtual void OnOverDraw(rGDIBlk GB,int PgNo, CRgn &ClipRgn);

    virtual flag DoLButtonDown(UINT nFlags, CPoint point);
    virtual flag DoLButtonUp(UINT nFlags, CPoint point);
    virtual flag DoLButtonDblClk(UINT nFlags, CPoint point);
    virtual flag DoRButtonDown(UINT nFlags, CPoint point);
    virtual flag DoRButtonUp(UINT nFlags, CPoint point);
    virtual flag DoRButtonDblClk(UINT nFlags, CPoint point);
//    virtual flag DoAccCnv(UINT Id);
//    virtual flag DoAccFmt(UINT Id);
//    virtual flag DoAccRptTagLists();
  };

#endif

// ==========================================================================
//
//
//
// ==========================================================================

DEFINE_TAGOBJEDT(PwrObject)
class PwrObject : public CTNode
  {
  public:
    //double dDisplayT;
    //double dDisplayP;
    //flag   bShowMs;

    PwrObject(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~PwrObject();

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual pchar  TagOfParent() { return PlantModelTag; };
  };

// ===========================================================================

_FWDDEF(PwrObjectEdt);
class DllImportExport PwrObjectEdt : public FxdEdtBookRef
  {
  public :
    PwrObject    & rPwrO;
    int            iNameWidth; 
    int            iPg1;

    CnvAttribute   VoltCnv, AmpCnv, PwrCnv;
    FmtAttribute   VoltFmt, AmpFmt, PwrFmt;

    int DistFromPgNo(int Pg);

    PwrObjectEdt(pFxdEdtView pView_, pPwrObject pPwrO_);
    virtual ~PwrObjectEdt();

    virtual void PutDataStart();
    virtual void PutDataDone();
    virtual void GetDataStart();
    virtual void GetDataDone();

    virtual void StartBuild();
    virtual void Build();
    virtual void Load(FxdEdtInfo &EI, Strng & Str);
    virtual long ButtonPushed(FxdEdtInfo &EI, Strng & Str);
    virtual long Parse(FxdEdtInfo &EI, Strng & Str);

    virtual flag DoLButtonDown(UINT nFlags, CPoint point);
    virtual flag DoLButtonUp(UINT nFlags, CPoint point);
    virtual flag DoLButtonDblClk(UINT nFlags, CPoint point);
    virtual flag DoRButtonDown(UINT nFlags, CPoint point);
    virtual flag DoRButtonUp(UINT nFlags, CPoint point);
    virtual flag DoRButtonDblClk(UINT nFlags, CPoint point);
    virtual flag DoAccCnv(UINT Id);
    virtual flag DoAccFmt(UINT Id);
    virtual flag DoAccRptTagLists();
  };

// ==========================================================================
//
//
//
// ==========================================================================

#if WITHIOOBJ

class IOObjectItem
  {
  public:
    Strng         m_sTag;
    bool          m_bInput;
    Strng         m_sDesc;
    DataUnion     m_Value;
    bool          m_bRmtCfg;
    DDBValueLstMem m_ValueList;
 
    IOObjectItem()
      {
      m_bInput=true;
      m_Value.iType=tt_Bool;
      m_bRmtCfg=false;
      } 
  };

DEFINE_TAGOBJEDT(IOObject)
class IOObject : public CTNode
  {
  public:
    //double dDisplayT;
    //double dDisplayP;
    //flag   bShowMs;
    //long      m_nIOs;
    CArray <IOObjectItem, IOObjectItem&> m_IO;

    IOObject(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~IOObject();

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual pchar  TagOfParent() { return PlantModelTag; };

    long    NoIOs()                         { return m_IO.GetSize(); };
    long    SetNoIOs(long n)                { m_IO.SetSize( n ); return m_IO.GetSize(); };
    LPCTSTR GetIOTag(long i)                { return m_IO[i].m_sTag(); };
    LPCTSTR SetIOTag(long i, LPCTSTR Tg)    { m_IO[i].m_sTag=Tg; return m_IO[i].m_sTag(); };
    LPCTSTR GetIODesc(long i)               { return m_IO[i].m_sDesc(); };
    LPCTSTR SetIODesc(long i, LPCTSTR Desc) { m_IO[i].m_sDesc=Desc; return m_IO[i].m_sDesc(); };
    byte    GetIOType(long i)               { return m_IO[i].m_Value.iType; };
    DataUnion & IOValue(long i)             { return m_IO[i].m_Value; };

    int     AddTag(LPCTSTR Tag, bool Input, LPCTSTR Desc, byte Type, LPCTSTR ValueStr, LPCTSTR ValueCnvs);
    int     RemoveTag(LPCTSTR Tag);
    int     FindTag(LPCTSTR Tag);

  };

// ===========================================================================

_FWDDEF(IOObjectEdt);
class DllImportExport IOObjectEdt : public FxdEdtBookRef
  {
  public :
    IOObject     & rIOO;
    int            iNameWidth; //
    int            iPg1;
    int            iPtsPerPg;
    
    int DistFromPgNo(int Pg);

    IOObjectEdt(pFxdEdtView pView_, pIOObject pPwrO_);//, rStrng Tag, rStrng Name);
    virtual ~IOObjectEdt();

    virtual void PutDataStart();
    virtual void PutDataDone();
    virtual void GetDataStart();
    virtual void GetDataDone();

    virtual void StartBuild();
    virtual void Build();
    virtual void Load(FxdEdtInfo &EI, Strng & Str);
    virtual long ButtonPushed(FxdEdtInfo &EI, Strng & Str);
    virtual long Parse(FxdEdtInfo &EI, Strng & Str);

    virtual flag DoLButtonDown(UINT nFlags, CPoint point);
    virtual flag DoLButtonUp(UINT nFlags, CPoint point);
    virtual flag DoLButtonDblClk(UINT nFlags, CPoint point);
    virtual flag DoRButtonDown(UINT nFlags, CPoint point);
    virtual flag DoRButtonUp(UINT nFlags, CPoint point);
    virtual flag DoRButtonDblClk(UINT nFlags, CPoint point);
    virtual flag DoAccCnv(UINT Id);
    virtual flag DoAccFmt(UINT Id);
    virtual flag DoAccRptTagLists();

    long    NoIOs()                       { return rIOO.NoIOs(); };
    long    SetNoIOs(long n)              { rIOO.SetNoIOs( n ); return rIOO.NoIOs(); };
    LPCTSTR GetIOTag(long i)              { return rIOO.m_IO[i].m_sTag(); };
    LPCTSTR SetIOTag(long i, LPCTSTR Tg)  { rIOO.SetIOTag(i, Tg); return rIOO.GetIOTag(i); };
    LPCTSTR GetIODesc(long i)             { return rIOO.GetIODesc(i); };
    LPCTSTR SetIODesc(long i, LPCTSTR Tg) { rIOO.SetIODesc(i, Tg); return rIOO.GetIODesc(i); };
    byte    GetIOType(long i)             { return rIOO.m_IO[i].m_Value.iType; };
    DataUnion & IOValue(long i)           { return rIOO.m_IO[i].m_Value; };
  
  };

#endif

// ==========================================================================
//
//
//
// ==========================================================================

#undef DllImportExport
#endif
