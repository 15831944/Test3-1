//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// generic Sfe DLL Interface

#ifndef __SFE_BASE_H
#define __SFE_BASE_H

#include "..\..\..\smdk\include\md_share2.h"
#include "executiv.h"
#include "nodedraw.h"
#include "scexec.h"
#include "sigflw.h"
#include "elecbase.h"
#include "afxwin.h"

//#pragma once

#ifdef __SFE_BASE_CPP
  #define DllImportExport DllExport
#elif !defined(SCEXEC)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

#define WithIOChgMonitor    1

// ========================================================================

const dword nc_FlwSolver  = 0x00000001; // General/Support Classes
const dword nc_Parent     = 0x00000002;
const dword nc_StandAlone = 0x00000004;
const dword nc_Audit      = 0x00000008;

const dword nc_Process    = eScdNodeType_Process;// 0x00000010; // Process Classes
const dword nc_MXfer      = eScdNodeType_MXfer  ;// 0x00000020;
const dword nc_MSrcSnk    = eScdNodeType_MSrcSnk;// 0x00000040;
const dword nc_MLnk       = eScdNodeType_MLnk   ;// 0x00000080;
const dword nc_MBst       = eScdNodeType_MBst   ;// 0x00000100;
                                                
const dword nc_Control    = eScdNodeType_Control;// 0x00001000; // Conrol Classes
const dword nc_CLnk       = eScdNodeType_CLnk   ;// 0x00002000;
                                                    
const dword nc_Elec       = eScdNodeType_Elec   ;// 0x00010000; // Power Transmission Classes
const dword nc_ELnk       = eScdNodeType_ELnk   ;// 0x00020000; 
const dword nc_HLnk       = eScdNodeType_HLnk   ;// 0x00040000;
const dword nc_ALnk       = eScdNodeType_ALnk   ;// 0x00080000;
const dword nc_Cbl        = eScdNodeType_Cbl    ;// 0x00100000;

#define FlwUnitGrp  "FlwUnit"
#define FlwLinkGrp  "FlwLink"
#define CtrlLinkGrp "CtrlLinkGrp"
#define ElecLinkGrp "ElecLinkGrp"
#define AirLinkGrp  "AirLinkGrp"

const int NErr_NotFound                = 2000;
const int NErr_NoSpace                 = 2001;
const int NErr_NoProblem               = 2002;
const int NErr_NotConverged            = 2003;
const int NErr_AlreadyConnected        = 2004;
const int NErr_Toomanyconnections      = 2005;
const int NErr_Incompatibleconnections = 2006;
const int NErr_AlreadyExists           = 2007;
const int NErr_SelfConnect             = 2008;

extern DllImportExport LPCTSTR NErr_String(int iErr);

// ========================================================================

class Node;

// ========================================================================

class DllImportExport MDLLInfo
  {
  public:
    int                        iPriority;
    CString                    sName;
    CArray <CString, LPCTSTR>  m_DependentDLLList;
    DWORD                      dwFlags; //not used yet
    DWORD                      dwUniqueID; //not used yet

    MDLLInfo()
      {
      iPriority = 99;
      dwFlags = 0;
      dwUniqueID = 0;
      };
    void AddDependentDLL(LPCTSTR pDLLName)
      {
      m_DependentDLLList.Add(CString(pDLLName));
      };
  };

class RequestModelInfoRec
  {
  public:
    Strng Class;
    Strng DefTag;
    Strng Desc;
    Strng ShortDesc;
    Strng MdlLibName;
    Strng Group;
    Strng DrwGroup;
    dword Category;
    bool IsSMDK;
    bool IsSelectable;

    RequestModelInfoRec()
      { Category=0; IsSMDK=0; IsSelectable=0; };
  };

class RequestModelInfoArray : public CArray <RequestModelInfoRec, RequestModelInfoRec& > {};

class RequestConnModelInfoRec
  {
  public:
    Strng SrcTag;
    //Strng SrcIO;
    Strng DstTag;
    //Strng DstIO;
    Strng ClassId;
  };

typedef struct
  {
  double m_Left,m_Top,m_Right,m_Bottom; 
  } CRct_Dbl;

typedef struct
  {
  int       iId;
  Strng     Name;
  Strng     Desc;
  flag      Input;
  flag      Output;
  DWORD     m_dwType;
  int       nIORqd;
  int       nIOMax;
  byte      Dirn;
  int       iGrp;
  flag      m_Hidden;
  Strng     m_sDefaultTS;
  CRct_Dbl  m_GrfTarget;
  } RequestModelIOInfoRec;

class RequestModelIOInfoArray : public CArray <RequestModelIOInfoRec, RequestModelIOInfoRec& > {};

typedef struct
  {
  int   iId;
  Strng Tag;
  Strng ClassName;
  Strng ClassId;
  Strng Group;
  int iGrp;
  int iDirn;
  flag fAutoConnect;
  flag fIsDirectConnect;
  } RequestModelIOConnRec;

typedef flag         (*fn_CallBack)(PVOID FS, int iCode, long lData, pvoid pData);

// ==========================================================================
//
//
//
// ==========================================================================

class DllImportExport CFlwTreeItem
  {
  public:
    CFlwTreeItem * pNext;
    CFlwTreeItem * pChild;
    Strng          sText;
    DWORD          dwFlags;
    int            iImage;

    CFlwTreeItem();
    CFlwTreeItem::CFlwTreeItem(char * Txt, DWORD Flags, int Image);
    ~CFlwTreeItem();

    CFlwTreeItem * AddSibling(char * Txt, DWORD Flags, int Image);
    CFlwTreeItem * AddChild(char * Txt, DWORD Flags, int Image);


  };

class DllImportExport CFlwTree : public CFlwTreeItem
  {
  public:

    CFlwTree();
    ~CFlwTree();
  };

// ==========================================================================
//
//
//
// ==========================================================================

class CModelStatusItem
  {
  public:
    Strng           m_Tag;
    dword           m_dwFlags;
    int             m_iColour;
    int             m_iAColour;

    CModelStatusItem()
      {
      m_dwFlags=0xFFFFFFFF;
      m_iColour=-1;
      m_iAColour=-1;
      };
  };

class CModelStatusArray : public CArray <CModelStatusItem, CModelStatusItem&> { };

class CModelStatusCriterion
  {
  public:
    CModelStatusCriterion(dword Opts)
      {
      m_dwMdlStatusOptions=Opts;
      }
    //Strng           m_sWhichTags;
  public:
    dword           m_dwMdlStatusOptions;
  };

// ==========================================================================
//
//
//
// ==========================================================================

class DllImportExport CNodeWiring
  {
  public:
    class CTerminal
      {
      public:
        CString m_sName;
        long    m_iTermStrip;
        CTerminal() { m_iTermStrip=-1;};
        CTerminal(LPCTSTR Name) : m_sName(Name) { m_iTermStrip=-1;};
        CTerminal(LPCTSTR Name, long TSNo) : m_sName(Name) { m_iTermStrip=TSNo;};
      };
    class CTermStrip
      {
      public:
        CString m_sName;
        CTermStrip() {};
        CTermStrip(LPCTSTR Name) : m_sName(Name) {};
      };
    class CConnect
      {
      public:
        long    m_iSrcTerm;
        long    m_iDstTerm;
        CConnect() { m_iSrcTerm=-1; m_iDstTerm=-1; }
      };
    class CCable
      {
      public:
        CString m_sName;
        CCable() {};
        CCable(LPCTSTR Name) : m_sName(Name) {};
      };

    class CCableArray :     public CArray<CCable,      CCable&>      {};
    class CTerminalArray :  public CArray<CTerminal,   CTerminal&>   {};
    class CTermStripArray : public CArray<CTermStrip,  CTermStrip&>  {};
    class CConnectArray :   public CArray<CConnect,    CConnect&>    {}; 


  public:
    CNodeWiring() : m_NodeTag("")
      {
      };
    CNodeWiring(LPCTSTR pTag) : m_NodeTag(pTag)
      {
      };

    void BuildWiring(LPCTSTR pTag, CNodeElectrics &WI);
    void AddCable(LPCTSTR Name);
    int FindTerminal(int iTermStrip, LPCTSTR Name);
    int TSTerminals(int iTermStrip, CLArray & Terminals);

    CString TsName(long iTs);
    CString TsTrmName(long iTerm);
    CString ConnectSrcName(long iConnect);
    CString ConnectDstName(long iConnect);

  public:
    CString   m_NodeTag;

    CCableArray       m_Cables;
    CTerminalArray    m_Terminals; 
    CTermStripArray   m_TermStrips;
    CConnectArray     m_Connects; 
    

  };

class DllImportExport CLinkWiring
  {
  public:

    class CTerm
      {
      public:
        CTerm(long Wire=-1, LPCTSTR TermStripName="", LPCTSTR TerminalName="") :
          m_iWire(Wire),
          m_sTermStripName(TermStripName),
          m_sTerminalName(TerminalName)
          {
          }
        CString  FullName() 
          {
          CString S(m_sTermStripName);
          S+=":";
          S+=m_sTerminalName;
          return S;
          }
        void FullName(CString & FullName) 
          {
          FullName=m_sTermStripName;
          FullName+=":";
          FullName+=m_sTerminalName;
          }

        long     m_iWire;
        CString  m_sTermStripName;
        CString  m_sTerminalName;
      };
    
    CString               m_LinkTag;
    CString               m_SrcNdTag;
    CString               m_DstNdTag;
    CArray<CTerm, CTerm&> m_SrcTerminals; 
    CArray<CTerm, CTerm&> m_DstTerminals; 

    bool                  m_bAutoWire;
    CString               m_SrcTS;
    CString               m_DstTS;
    
    CLinkWiring()         
      {  
      m_bAutoWire=false;
      };
    CLinkWiring(LPCTSTR pTag):m_LinkTag(pTag), m_bAutoWire(false)
      {
      };

    long WireCount() const { return m_SrcTerminals.GetCount(); }; 

  };

// ==========================================================================
//
//
//
// ==========================================================================

const int MaxChngTrcVals = 10;

enum eTraceValue
  {
  TV_None,
  TV_AbsChg,
  TV_RelChg,
  TV_Value,
  };


class DllImportExport CTraceItem
  {
  protected:
    // User Filled in
    Strng           m_sFullTag;
    Strng           m_sDstTag;
    Strng           m_sDescTag;
    Strng           m_sCnv;
    long            m_lId;

    // Calculated by CFlwSolver..
    double          m_dVals[MaxChngTrcVals];
    double          m_dRqds[MaxChngTrcVals];
    long            m_lIter[MaxChngTrcVals];
    long            m_nVals;

    static long     m_lIterCount;
  public:

    CTraceItem();
    ~CTraceItem();
    CTraceItem(const CTraceItem & Other);
    CTraceItem & operator=(const CTraceItem & Other);

    LPCTSTR         FullTag()    { return m_sFullTag(); };
    LPCTSTR         DstTag()     { return m_sDstTag(); };
    LPCTSTR         DescTag()    { return m_sDescTag(); };
    LPCTSTR         CnvStr()     { return m_sCnv(); };
    static LPCTSTR  Prefix()     { return "Chgs"; };
    static LPCTSTR  IdStr(long Id);

    void            Init(long Id, LPCTSTR DescTag, LPCTSTR Cnv, LPCTSTR NdTg, LPCTSTR DstNdTg);
    void            Set(double Val, double Rqd=dNAN);
    static void     BumpCount() { m_lIterCount++;};
    static long     CurrentIter() { return m_lIterCount;};
    
    long            Id()                  { return m_lId; };
    long            NVals()               { return m_nVals; };
    double          Val(int i=0)          { return m_dVals[i]; };
    double          Rqd(int i=0)          { return m_dRqds[i]; };
    long            IterNo(int i=0)       { return m_lIter[i]; };
    bool            ErrorDefined(int i=0);
    double          AbsErr(int i=0);
    double          AbsChg(int i=0);
    double          RelErr(int i=0);
    double          RelChg(int i=0);
    double          CalcValue(eTraceValue What, int i=0);
  };


class CTraceArray : public CArray <CTraceItem, CTraceItem&> 
  { 
  public:
    CTraceArray & operator=(const CTraceArray &X)
      {
      SetSize(X.GetSize());
      for (int i=0; i<X.GetSize(); i++)
        (*this)[i]=X[i];
      return *this;
      };
  };
class CTraceList : public CList  <CTraceItem*, CTraceItem*> { };

// ==========================================================================
//
//
//
// ==========================================================================

enum eEOClass 
  {
  EOC_Null=0, 
  EOC_Proc=0x00010000, 
  EOC_Ctrl=0x00020000, 
  EOC_Elec=0x00040000, 
  EOC_Air =0x00080000, 
  EOC_XRef=0x00100000, 
  EOC_Mask=0xffff0000
  };

class DllImportExport CEvalOrderIOItem
  {
  public:

    CEvalOrderIOItem() 
      {
      m_bIn=false;
      m_bOut=false;
      m_bTear=false;
      m_bOwner=false;
      m_lRmtProcOrd=0;
      m_lRmtCtrlOrd=0;
      m_dMax=0;
      m_dwType=EOC_Null;
      };
    CEvalOrderIOItem(const CEvalOrderIOItem & X)
      {
      m_bIn         = X.m_bIn;
      m_bOut        = X.m_bOut;
      m_bTear       = X.m_bTear;
      m_bOwner      = X.m_bOwner;
      m_lRmtProcOrd = X.m_lRmtProcOrd;
      m_lRmtCtrlOrd = X.m_lRmtCtrlOrd;
      m_sRmtTag     = X.m_sRmtTag;
      m_sRmtDesc    = X.m_sRmtDesc;
      m_Trc         = X.m_Trc;
      m_dMax        = X.m_dMax;
      m_dwType      = X.m_dwType;
      };
    CEvalOrderIOItem&operator=(const CEvalOrderIOItem & X)
      {
      m_bIn         = X.m_bIn;
      m_bOut        = X.m_bOut;
      m_bTear       = X.m_bTear;
      m_bOwner      = X.m_bOwner;
      m_lRmtProcOrd = X.m_lRmtProcOrd;
      m_lRmtCtrlOrd = X.m_lRmtCtrlOrd;
      m_sRmtTag     = X.m_sRmtTag;
      m_sRmtDesc    = X.m_sRmtDesc;
      m_dMax        = X.m_dMax;
      m_dwType      = X.m_dwType;
      return *this;
      };

    bool            m_bIn;
    bool            m_bOut;
    bool            m_bTear;
    bool            m_bOwner;
    long            m_lRmtProcOrd;
    long            m_lRmtCtrlOrd;
    double          m_dMax;
    eEOClass        m_dwType;

    Strng     m_sRmtTag;
    Strng     m_sRmtDesc;
    CTraceArray m_Trc;
  };

class DllImportExport CEvalOrderItem
  {
  public:
    CEvalOrderItem() 
      {
      m_lProcOrd=0;
      m_lCtrlOrd=0;
      m_dMax=0;
      };
    CEvalOrderItem(const CEvalOrderItem & X)
      {
      m_sTag      = X.m_sTag;
      m_lProcOrd  = X.m_lProcOrd;
      m_lCtrlOrd  = X.m_lCtrlOrd;
      m_dMax      = X.m_dMax;
      m_FIOs.Copy(X.m_FIOs);
      m_CIOs.Copy(X.m_CIOs);
      m_XIOs.Copy(X.m_XIOs);
      };
    CEvalOrderItem&operator=(const CEvalOrderItem & X)
      {
      m_sTag      = X.m_sTag;
      m_lProcOrd  = X.m_lProcOrd;
      m_lCtrlOrd  = X.m_lCtrlOrd;
      m_dMax      = X.m_dMax;
      m_FIOs.Copy(X.m_FIOs);
      m_CIOs.Copy(X.m_CIOs);
      m_XIOs.Copy(X.m_XIOs);
      return *this;
      };

    Strng     m_sTag;
    long      m_lCluster;
    long      m_lProcOrd;
    long      m_lCtrlOrd;
    double    m_dMax;
    CArray <CEvalOrderIOItem, CEvalOrderIOItem&> m_FIOs, m_CIOs, m_XIOs;
  };
class DllImportExport CEvalOrderArray : public CArray <CEvalOrderItem, CEvalOrderItem&> {};

// ==========================================================================
//
//
//
// ==========================================================================

class CEvalStatsRec
  {
  public:
    long   m_lCallCount;
    double m_dTimeTot;
    double m_dTimeIter;

    CEvalStatsRec()
      {
      m_lCallCount=0;
      m_dTimeTot=0;
      m_dTimeIter=0;
      };

    CEvalStatsRec&operator=(CStopWatch & Sw)
      {
      m_lCallCount=Sw.Laps();
      m_dTimeTot=Sw.Secs();
      return *this;
      }
  
  };

class DllImportExport CEvalStatsItem
  {
  public:
    CEvalStatsItem() 
      {
      };
    CEvalStatsItem(const CEvalStatsItem & X)
      {
      m_sTag                = X.m_sTag;
      m_sClass              = X.m_sClass;
      m_Items.Copy(X.m_Items);
      };
    CEvalStatsItem&operator=(const CEvalStatsItem & X)
      {
      m_sTag                = X.m_sTag;
      m_sClass              = X.m_sClass;
      m_Items.Copy(X.m_Items);
      return *this;
      };

    Strng     m_sTag, m_sClass;
    CArray <CEvalStatsRec, CEvalStatsRec&> m_Items;
  };

class DllImportExport CEvalStatsArray : public CArray <CEvalStatsItem, CEvalStatsItem&> 
  {
  public:
    long     m_nIterCount;
    //double   m_nIterCount;
  };

// ==========================================================================
//
//
//
// ==========================================================================

class CNodeListItem
  {
  public:
    CNodeListItem() 
      {
      m_lSeqNo=-1;
      m_bIsLnk=false;
      m_lSrcIOInx=-1;
      m_lDstIOInx=-1;
      };
    CNodeListItem(const CNodeListItem & X)
      {
      *this=X;
      };
    CNodeListItem&operator=(const CNodeListItem & X)
      {
      m_sTag                = X.m_sTag;
      m_sClass              = X.m_sClass;
      m_sSubClass           = X.m_sSubClass;
      m_sPrimaryCfg         = X.m_sPrimaryCfg;
      m_sEqpDesc            = X.m_sEqpDesc;
      m_sEqpMemo            = X.m_sEqpMemo;
      m_sEqpIdStr           = X.m_sEqpIdStr;
      m_sEqpLocation        = X.m_sEqpLocation;
      m_sPlantArea          = X.m_sPlantArea;
      m_sEqpGUID            = X.m_sEqpGUID;
      m_lSeqNo              = X.m_lSeqNo;
      m_bIsLnk              = X.m_bIsLnk;
      m_sSrcNd              = X.m_sSrcNd;;
      m_sDstNd              = X.m_sDstNd;;
      m_sSrcIO              = X.m_sSrcIO;;
      m_sDstIO              = X.m_sDstIO;;
      m_lSrcIOInx           = X.m_lSrcIOInx;;
      m_lDstIOInx           = X.m_lDstIOInx;;
      return *this;
      };

    CString   m_sTag;
    CString   m_sClass;
    CString   m_sSubClass;
    CString   m_sPrimaryCfg;
    CString   m_sEqpDesc;
    CString   m_sEqpMemo;
    CString   m_sEqpIdStr;
    CString   m_sEqpLocation;
    CString   m_sPlantArea;
    CString   m_sEqpGUID;
    long      m_lSeqNo;
    bool      m_bIsLnk;
    CString   m_sSrcNd;
    CString   m_sDstNd;
    CString   m_sSrcIO;
    CString   m_sDstIO;
    long      m_lSrcIOInx;
    long      m_lDstIOInx;

  };

class DllImportExport CNodeList : public CList <CNodeListItem, CNodeListItem&> 
  {
  public:
  };

template<> inline BOOL AFXAPI CompareElements(const CNodeListItem * E1, const CNodeListItem * E2)
  { 
  return  (E1->m_sTag == E2->m_sTag) &&
          (E1->m_sClass == E2->m_sClass) &&
          (E1->m_sSubClass == E2->m_sSubClass) &&
          (E1->m_sPrimaryCfg == E2->m_sPrimaryCfg) &&
          (E1->m_lSeqNo == E2->m_lSeqNo) &&
          (E1->m_bIsLnk == E2->m_bIsLnk) &&
          (E1->m_sSrcNd == E2->m_sSrcNd) &&
          (E1->m_sDstNd == E2->m_sDstNd) &&
          (E1->m_sSrcIO == E2->m_sSrcIO) &&
          (E1->m_sDstIO == E2->m_sDstIO) &&
          (E1->m_lSrcIOInx == E2->m_lSrcIOInx) &&
          (E1->m_lDstIOInx == E2->m_lDstIOInx);
  };

// ==========================================================================
//
//
//
// ==========================================================================

class CNodeConfigItem
  {
  public:
    CNodeConfigItem() 
      {
      m_ttType      = tt_NULL;
      m_dwChgSeqNo  = 0;
      };
    CNodeConfigItem(const CNodeConfigItem & X)
      {
      *this=X;
      };
    CNodeConfigItem&operator=(const CNodeConfigItem & X)
      {
      m_ttType      = X.m_ttType;
      m_sNdTag      = X.m_sNdTag;  
      m_sFldTag     = X.m_sFldTag;  
      m_sCnvStr     = X.m_sCnvStr;
      m_sValue      = X.m_sValue;
      m_dwChgSeqNo  = X.m_dwChgSeqNo;
      return *this;
      };

    int       m_ttType;
    CString   m_sNdTag;
    CString   m_sFldTag;
    CString   m_sCnvStr;
    CString   m_sValue;
    DWORD     m_dwChgSeqNo;
  };

class DllImportExport CNodeConfigList : public CList<CNodeConfigItem, CNodeConfigItem&> 
  {
  public:
  };

template<> inline BOOL AFXAPI CompareElements(const CNodeConfigItem * E1, const CNodeConfigItem * E2)
  { 
  return 
    (E1->m_sFldTag == E2->m_sFldTag) && 
    //(E1->m_bIsDbl == E2->m_bIsDbl) && 
    //(E1->m_dValue == E2->m_dValue) && 
    (E1->m_sValue == E2->m_sValue);
  };

// ==========================================================================
//
//
//
// ==========================================================================

class DllImportExport CNodeBalanceIO 
  {
  public:
    CNodeBalanceIO ();
    CNodeBalanceIO (const CNodeBalanceIO  & X);
    CNodeBalanceIO & operator=(const CNodeBalanceIO  & X);

    //CNodeBalanceIO & operator+=(const CNodeBalanceIO  & X);
    void                  Clear(bool Full);
    void                  DoCalculations();

  public:
    CString               m_Tag;
    CString               m_IOPort;
    CString               m_SpModelName;
    int                   m_iJoinId;
    int                   m_iSgn;
    double                m_Qm;
    double                m_T;
    double                m_P;
    double                m_Rho;
    double                m_Sf;
    double                m_Lf;
    double                m_Vf;
    double                m_msCp0;
    double                m_msCpT;
    double                m_msHsT;
    double                m_msHzT;
    double                m_msHfT;
    double                m_totHsT;
    double                m_totHzT;
    double                m_totHfT;
  };

class DllImportExport CNodeBalanceIOArray : public CArray<CNodeBalanceIO, CNodeBalanceIO&> {};

// --------------------------------------------------------------------------

class DllImportExport CNodeBalanceJoin
  {
  public:
    CNodeBalanceJoin();
    CNodeBalanceJoin(const CNodeBalanceJoin& X);
    CNodeBalanceJoin& operator=(const CNodeBalanceJoin& X);
    CNodeBalanceJoin& operator+=(const CNodeBalanceJoin& X);
    void                  AddIO(const CNodeBalanceIO  & X);

    void                  Clear(bool Full);
    void                  DoCalculations();

  public:                              
    double                m_JoinMassFlwIn;
    double                m_JoinMassGain;
    double                m_JoinMassFlwOut;
    double                m_JoinFeedCpT;
    double                m_JoinProdCpT;
    double                m_JoinHfIn;
    double                m_JoinHfGain;
    double                m_JoinPowerIn;
    double                m_JoinHfOut;
    double                m_JoinHfChg;
    double                m_JoinHsIn;
    double                m_JoinHsGain;
    double                m_JoinHsOut;
    double                m_JoinHsChg;

    double                m_RCTMassGain;
    double                m_RCTHfGain;
    double                m_RCTHsGain;
    double                m_RCTHOR_Std0;
    double                m_RCTHOR_Mdl0;
    double                m_RCTHOR_Used0;
    double                m_RCTHOR_Diff0;
    double                m_RCTPowerIn;
    double                m_VLEHsGain;
    double                m_EHXPowerIn;

  };

class DllImportExport CNodeBalanceJoinArray : public CArray<CNodeBalanceJoin, CNodeBalanceJoin&> {};

// --------------------------------------------------------------------------

class DllImportExport CNodeBalanceInfo 
  {
  public:
    CNodeBalanceInfo(LPCTSTR Tag="");
    void                  SetJoinCount(int N);
    void                  DoCalculations(bool CombinedJoins);
    void                  Combine(CNodeBalanceInfo & B);

  public:
    CString               m_NodeTag;
    CNodeBalanceIOArray   m_Feeds, m_Prods;
    CNodeBalanceJoinArray m_Joins;
  };

class DllImportExport CNodeAuditInfo 
  {
  public:
    CNodeAuditInfo(LPCTSTR Tag, dword Options);

  public:
    CString               m_NodeTag;
    dword                 m_Options;
    CString               m_Report;

  };

// ==========================================================================
//
//
//
// ==========================================================================

class CXRefItem; 
class CTagRef; 
class DllImportExport CTagRefInfoItem
  {
  public:
    Strng   m_sRefTag;
    Strng   m_sRmtTag;
    Strng   m_sRefID;
    Strng   m_sRmtID;
    Strng   m_sCnv;
    Strng   m_sLclVar;
    bool    m_fIsGetRef,
            m_fIsSetRef,
            m_fIsIO;
    int     m_iGetCnt;
    int     m_iSetCnt;

    void Copy (const CTagRef & X, bool Reverse);

  private:
    CTagRefInfoItem & operator=(const CTagRef & X)    // Hide this
      {
      INCOMPLETECODE();
      return *this;
      };
  };

typedef CSmartPtrAllocate<CTagRefInfoItem> CSPXRefInfoItem;
class DllImportExport CXRefInfoArray : public CArray<CSPXRefInfoItem, CSPXRefInfoItem&> {};

// ==========================================================================
//
//
//
// ==========================================================================

class CRctBlkInformation
  {
  public:
    class CRct
      {
      public:
        CRct()
          {
          m_iSeqNo = 0;
          m_iBasis = RctNull;
          }
        CRct(const CRct & Other)
          {
          *this=Other;
          }
        CRct & operator = (const CRct & Other)
          {
          m_iSeqNo    = Other.m_iSeqNo;
          m_iBasis    = Other.m_iBasis;
          m_sTag      = Other.m_sTag;
          m_sComment  = Other.m_sComment;
          m_sDefn     = Other.m_sDefn;
          }

      public:
        int                m_iSeqNo;
        eScdReactionBasis  m_iBasis;
        CString            m_sTag;
        CString            m_sComment;
        CString            m_sDefn;
      };

    class CRctBlk
      { 
      public:
        CRctBlk() 
          {
          }
        CRctBlk(const CRctBlk & Other)
          {
          *this=Other;
          }
        CRctBlk & operator=(const CRctBlk & Other)
          {
          m_sNdTag    = Other.m_sNdTag;
          m_sBlkTag   = Other.m_sBlkTag;
          m_sRctFile  = Other.m_sRctFile;
          m_sComment  = Other.m_sComment;
          m_Rcts.SetSize(Other.m_Rcts.GetSize());
          for (int i=0; i<Other.m_Rcts.GetSize(); i++)
            m_Rcts[i]=Other.m_Rcts[i];
          }

      public: 
        CString     m_sNdTag; 
        CString     m_sBlkTag; 
        CString     m_sRctFile; 
        CString     m_sComment; 
        CArray <CRct, CRct&> m_Rcts;
      };

    CArray <CRctBlk, CRctBlk&> m_RBs;
 
  }; 

// ==========================================================================
//
//
//
// ==========================================================================

enum FE_TagOps
  {
  FETOp_GetActive,
  FETOp_SetActive,
  FETOp_GetHold,
  FETOp_SetHold,
  FETOp_MarkFlwNets,
  FETOp_MarkPrsNets,
  FETOp_MrkCrit1,
  FETOp_MrkCrit2,
  FETOp_MrkCrit3,
  FETOp_ClrMarked,

  };

class CFlwSolver; // forward
class FlwNode; // forward
class CXRefInfoArray;
class CFlwNodeArray : public CArray<FlwNode*, FlwNode*> {};

class DllImportExport CSfeSrvrBase
  {
  friend class CSysCADLicense;
  protected:
    fn_CallBack    SfeCallBack;
    CFlwSolver     *SfeFS;

  public:
    CSfeSrvrBase();
    virtual ~CSfeSrvrBase();
    virtual flag         Init(pchar ModuleName, pchar FlwLibTag, HINSTANCE hInst, CExecObj *pObj, flag DbgOn, flag FlushDbg, fn_CallBack CallBack) { return false; };
    //virtual void         InitLic(dword LicWord) { };
    virtual flag         Term() { return false; };
    virtual pNodeGrfInfo Drawings() { return 0; };

    virtual int          FE_DoInsert(char * Class, char *SubClass, char *PrimaryCfgIn, char *NTag, char *SrcTag, char *DstTag, FlwNode **pNd=NULL) { return 0; };
    virtual int          FE_DoInsertRoot(char * RootName) { return 0; };
    virtual int          FE_DoDelete(char * Tag) { return 0; };
    virtual int          FE_DoConnect(char * SrcTag, char *DstTag) { return 0; };
    virtual int          FE_DoDisConnect(char * SrcTag, char *DstTag) { return 0; };
    virtual int          FE_SetCommon(char * Tag, char * EqpDesc, char * EqpMemo, char * EqpIdStr, char * EqpLocation, char * PlantArea, char * EqpGUID) { return 0; };
    virtual int          FE_GetCommon(char * Tag, Strng & EqpDesc, Strng & EqpMemo, Strng & EqpIdStr, Strng & EqpLocation, Strng & PlantArea, Strng & EqpGUID) { return 0; };
//    virtual int          FE_DoSolveNet() { return 0; };
    virtual int          FE_TagOperation(FE_TagOps Act, Strng_List & TagList) { return 0; };
    virtual int          FE_SetHoldGlobalLinks(bool On) { return 0; };
    virtual byte         FE_FlwMomentumOn() { return 0; };
    virtual void         FE_SetFlwMomentumOn(byte On) { };

    virtual byte         FE_ODE_Method() { return 0; };
    virtual byte         FE_ODE_StepSizeControl() { return 0; };
    virtual void         FE_ODE_SetMethod(byte Method_) { };
    virtual void         FE_ODE_SetStepSizeControl(byte StepSizeControl_) { };
    virtual double       FE_ODE_GetMagEPSAbs() { return 0; };
    virtual double       FE_ODE_GetMagEPSRel() { return 0; };
    virtual void         FE_ODE_SetMagEPSAbs(double Eps) { };
    virtual void         FE_ODE_SetMagEPSRel(double Eps) { };
    virtual double       FE_ODE_GetTimeInc() { return 0; };
    virtual void         FE_ODE_SetTimeInc(double T) { };
    virtual double       FE_ODE_GetTimeIncMxRqd() { return 0; };
    virtual void         FE_ODE_SetTimeIncMxRqd(double T) { };
    virtual double       FE_ODE_GetTimeIncMnRqd() { return 0; };
    virtual void         FE_ODE_SetTimeIncMnRqd(double T) { };

    virtual double       FE_Net_GetLnkEPSAbs() { return 0; };
    virtual double       FE_Net_GetLnkEPSRel() { return 0; };
    virtual void         FE_Net_SetLnkEPSAbs(double Eps) { };
    virtual void         FE_Net_SetLnkEPSRel(double Eps) { };
    virtual double       FE_Net_GetNetDeRate() { return 0; };
    virtual void         FE_Net_SetNetDeRate(double Mult) { };
    virtual char *       FE_Net_GetSpillArea() { return 0; };
    virtual void         FE_Net_SetSpillArea(char * Spill) { };

    virtual int          FE_AD_GetID      (int Index, Strng & Class, Strng & Name) { return 0; };
    virtual int          FE_AD_Select     (int Index, char * Tag) { return 0; };
    virtual int          FE_AD_SetSelectedTags(Strng_List &SelectedTags) { return 0; };
    virtual int          FE_AD_GetTagTree (CFlwTree & Tree) { return 0; };
    virtual int          FE_AD_GetStatus  (Strng & Status) { return 0; };
    virtual int          FE_AD_Go         () { return 0; };

    virtual int          FE_IOPnt_Add     (LPCTSTR Tag, bool Input, LPCTSTR Desc, byte Type, LPCTSTR ValueStr, LPCTSTR ValueCnvs) { return 0; };
    virtual int          FE_IOPnt_Remove  (LPCTSTR Tag) { return 0; };

    virtual int          FE_GetEvalOrder(bool GetCtrl, bool GetFullDescI, bool GetFullDescO, eTraceValue TraceWhat, CEvalOrderArray &Info) { return 0; };
    virtual int          FE_GetEvalStats(CEvalStatsArray &Info) { return 0; };
    virtual int          FE_GetNodeList(CNodeList&List) { return 0; };
    virtual int          FE_GetNodeConfiguration(BOOL AllParms, BOOL AllState, LPCTSTR NdTag, CNodeConfigList & List) { return 0; };
    virtual int          FE_GetNodeBalanceInfo(CNodeBalanceInfo & Balance, CNodeAuditInfo & Audit) { return 0; };

    virtual flag         RequestModelInfoByGroupIndex(pchar pGroup, int iNo, RequestModelInfoRec & Info, bool IgnoreSelFn=false);

    virtual flag         RequestModelInfoByClassId(pchar pClass, RequestModelInfoRec & Info);
    virtual flag         RequestModelClassId(pchar pTag, Strng & ClassId) { return 0; };
    virtual flag         RequestModelConnInfo(pchar pTag, RequestConnModelInfoRec & Info) { return 0; };
    virtual flag         RequestModelIOInfoByClassId(pchar pClass, RequestModelIOInfoArray & Info) { return 0; };

    virtual flag         RequestModelIOInfoByIndex(pchar pTag, int iIndex, RequestModelIOInfoRec & Info) { return 0; };
    virtual flag         RequestModelIOInfoById(pchar pTag, int iId, RequestModelIOInfoRec & Info) { return 0; };
    virtual flag         RequestModelIOConn(pchar pTag, int iNo, RequestModelIOConnRec & Info) { return 0; };
    virtual int          RequestModelStatus(CModelStatusCriterion &Criteria, CModelStatusArray &Status) { return 0; };
    virtual int          RequestModelStatusChgCnt(CModelStatusCriterion &Criteria, int MaxTest) { return 0; };
    virtual int          RequestTagRefInfo(LPCTSTR Tag, CXRefInfoArray &Refs) { return 0; };
    //virtual int          RequestXRefInfoChg() { return 0; };

    virtual int          FE_GetRctInfo(CRctBlkInformation & RctInfo) { return 0; };
    virtual int          FE_SetRctInfo(LPCTSTR NodeTag)    { return 0; };

    virtual int          FE_GetNodeWiring(CNodeWiring &NodeWiring) { return -1; };
    virtual int          FE_GetLinkWiring(CLinkWiring &LinkWiring) { return -1; };  // -1 implies not a cable
    virtual int          FE_SetLinkWiring(CLinkWiring &LinkWiring) { return -1; };

    virtual long         FE_GetTopologyCount() { return 0; };
    virtual long         FE_GetNodes(DWORD What, CFlwNodeArray & Nodes) { return 0; };
    virtual FlwNode     *FE_FindNode(LPCTSTR Tag) { return NULL; };

    //virtual void         FE_ElecComponentsChanged() { };
    virtual void         FE_UnDoGlobalLinks()                       { };
    virtual long         FE_SpModelFixupStrategy()                  { return 0; }
    virtual void         FE_SetSpModelFixupStrategy(long Strategy)  { }


    virtual flag         FE_TestModelTagUnique(pchar pTag)           { return 0; };

    virtual void         EO_SetModelState(eScdMdlStateActs RqdState, Strng_List &RqdTags) { };
    virtual flag         EO_GetModelAction(Strng & Tag, MdlActionArray & Acts) { return 0; };
    virtual flag         EO_SetModelAction(Strng & Tag, MdlAction & Act) { return 0; };

    //virtual flag         EO_SolutionReqd() { return 0; };
    //virtual int          EO_InitSolution(EqnSlvCtrlBlk & EQCtrl) { return 0; };
    //virtual int          EO_StepSolution(EqnSlvCtrlBlk & EQCtrl) { return 0; };
    //virtual int          EO_TermSolution(EqnSlvCtrlBlk & EQCtrl) { return 0; };
    virtual int          EO_GetSolutionError(EqnSlvCtrlBlk & EQCtrl, Strng & RptLine) { return 0; };

    virtual void         EO_OnAppActivate(BOOL bActive) { };
    virtual flag         EO_RequestTagInfo(RequestTagInfoRec& Rqst, ReplyTagInfoRec& Info) { return 0; };
    virtual int          EO_QueryChangeTag(pchar pOldTag, pchar pNewTag) { return 0; };
    virtual int          EO_ChangeTag(pchar pOldTag, pchar pNewTag) { return 0; };
    virtual int          EO_QueryDeleteTag(pchar pDelTag) { return 0; };
    virtual int          EO_DeleteTag(pchar pDelTag) { return 0; };
    virtual int          EO_CanClose(Strng_List & Problems) { return 0; };

    virtual void         EO_SetSolveMode(long RunMds, long RunMdsMsk) { };
    virtual flag         EO_Starting(flag fBeginStarting) { return 0; };
    virtual flag         EO_SetTime(CTimeValue TimeRqd, bool Rewind) { return true;};
    virtual flag         EO_QueryTime(CXM_TimeControl &CB, CTimeValue &TimeRqd, CTimeValue &dTimeRqd) { return 0; };
    virtual flag         EO_GotoRunStart(CXM_TimeControl &CB)                        { return 0; }
    virtual flag         EO_GotoRunEnd(CXM_TimeControl &CB, bool TimeChanged)        { return 0; }
    virtual flag         EO_PreStart(CXM_TimeControl &CB)                             { return 1; }
    virtual flag         EO_Start(CXM_TimeControl &CB)                                { return 0; };
    virtual void         EO_QuerySubsReqd(CXMsgLst &XM)                               { };
    virtual void         EO_QuerySubsAvail(CXMsgLst &XM, CXMsgLst &XMRet)             { };
    virtual flag         EO_ReadSubsData(CXMsgLst &XM)                                { return 0; };
    virtual flag         EO_WriteSubsData(CXMsgLst &XM, flag FirstBlock, flag LastBlock) { return 0; };
    virtual DWORD        EO_ReadTaggedItem(CXM_ObjectTag & ObjTag, CXM_ObjectData &ObjData, CXM_Route &Route) { return 0; };
    virtual int          EO_WriteTaggedItem(CXM_ObjectData &ObjData, CXM_Route &Route) { return 0; };
    virtual flag         EO_Execute(CXM_TimeControl &CB, CEOExecReturn &EORet) { return 0; };
    virtual flag         EO_StepDone() { return 0; };
    virtual flag         EO_QueryStop(CXM_TimeControl &CB, bool Starting) { return 0; };
    virtual flag         EO_Stop(CXM_TimeControl &CB) { return 0; };
    virtual flag         EO_Stopping(flag BeginStopping) { return 0; };
    virtual flag         EO_BeginPause(CXM_TimeControl &CB) { return 0; };
    virtual flag         EO_EndPause(CXM_TimeControl &CB) { return 0; };
    virtual flag         EO_BeginSave(FilingControlBlock &FCB) { return 0; };
    virtual flag         EO_SaveDefinedData(FilingControlBlock &FCB, Strng &Tag, CXMsgLst &XM) { return 0; };
    virtual flag         EO_SaveOtherData(FilingControlBlock &FCB) { return 0; };
    virtual flag         EO_SaveConn(FilingControlBlock &FCB, CXMsgLst &XM) { return 0; };
    virtual flag         EO_EndSave(FilingControlBlock &FCB) { return 0; };
    virtual flag         EO_BeginLoad(FilingControlBlock &FCB) { return 0; };
    virtual flag         EO_LoadDefinedData(FilingControlBlock &FCB, CXMsgLst &XM) { return 0; };
    virtual flag         EO_LoadOtherData(FilingControlBlock &FCB) { return 0; };
    virtual flag         EO_LoadConn(FilingControlBlock &FCB, CXMsgLst &XM) { return 0; };
    virtual flag         EO_EndLoad(FilingControlBlock &FCB) { return 0; };
    virtual void         EO_BeginBulkChange() {};
    virtual void         EO_EndBulkChange() {};

  };

typedef CSfeSrvrBase* (*fn_AddSfeSrvrBase)();
extern DllImportExport CSfeSrvrBase * gs_pSfeSrvr;

// ==========================================================================
//
//
//
// ==========================================================================

class DllImportExport CGrfTagGroups
  {
  public:
    CGrfTagGroups() {};
    int Count() { return Groups.GetSize(); };
    void Reset(int Size = 0);
    int Find(char* pGrpName);
    char* GetName(int index);
    bool IsActive(int index);
    void Rebuild(bool Force = false);
    void SetActive(int index, bool Active);

    CArray <Strng, Strng&> Groups;
    CArray <int, int> Active;
  };

extern DllImportExport CGrfTagGroups gs_GrfTagGroups;

// ==========================================================================
//
//
//
//===========================================================================

class DllImportExport CPrjSaveAsDlg : public CDialog
  {
  public:
    CPrjSaveAsDlg(char* pRootDir, BOOL IsNew, CWnd* pParent = NULL);
    //{{AFX_DATA(CPrjSaveAsDlg)
    enum { IDD = IDD_PRJSAVEASDLG };
    BOOL m_CopyHist;
    int m_DbFmt;
    CString m_VerStr;
    BOOL m_RemovePrevRevs;
    BOOL m_CopyDBFromOld;
    //}}AFX_DATA
    BOOL m_Copy[6];
    int m_CopyCnt[6];
    BOOL m_DBOn;
    BOOL m_CopyOn;
    BOOL m_CopyDBOn;

    //{{AFX_VIRTUAL(CPrjSaveAsDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

  protected:
    Strng sRoot;
    BOOL bIsNew;
    //Strng sTitle;
    //{{AFX_MSG(CPrjSaveAsDlg)
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  public:
    CComboBox m_cbFmts;
    afx_msg void OnBnClickedOk();
  };

//===========================================================================

class DllImportExport CNewPrjDlg : public CDialog
  {
  public:
    CNewPrjDlg(char * PrjFile, BOOL ForNewPrj, BOOL ForEdit, DWORD LicCategory, CWnd* pParent = NULL);
    virtual ~CNewPrjDlg();
    //{{AFX_DATA(CNewPrjDlg)
    enum { IDD = IDD_NEWPRJ_DLG };
    CListCtrl	m_MdlListC;
    int   m_NewGraph;
    int   m_NewTrend;
    CString	m_DllPath;
    //}}AFX_DATA

    CImageList    ImgList;

    DWORD         m_dwLicCategory;
    Strng         m_PrjFile;
    Strng         m_BrowseFolder;
    BOOL          m_bForNewPrj;
    BOOL          m_bForEdit;
    int           m_iModelCfg;

    CStringArray  m_FileArray;
    CStringArray  m_NameArray;
    CStringArray  m_DescArray;
    CStringArray  m_NameAndDescArray;
    CWordArray    m_ExistsArray;

    int           ModelCfg() { return m_iModelCfg;};

    int           AddACfg(Strng & ACfg, flag AllowConvert, flag DoDllLoad);
    int           RemoveACfg(int iCfg);

    char *        File()       { return m_iModelCfg>=0 ? (char*)(const char*)m_FileArray       [m_iModelCfg] : NULL; }
    char *        Name()       { return m_iModelCfg>=0 ? (char*)(const char*)m_NameArray       [m_iModelCfg] : NULL; }
    char *        Desc()       { return m_iModelCfg>=0 ? (char*)(const char*)m_DescArray       [m_iModelCfg] : NULL; }
    char *        NameAndDesc(){ return m_iModelCfg>=0 ? (char*)(const char*)m_NameAndDescArray[m_iModelCfg] : NULL; }

    //{{AFX_VIRTUAL(CNewPrjDlg)
    public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
  protected:
    int TryAddUpdateCfg(char* pCfgFile);
    int FindOrigIndex(CString& Cfg, CString& File);
    void SelectItem(int Pos);
    void DoNewCfg(char* pFileName);
    //{{AFX_MSG(CNewPrjDlg)
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    afx_msg void OnDynamic();
    afx_msg void OnProbal();
    afx_msg void OnBrowse();
    afx_msg void OnDblclkMdllist2(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnRemove();
    afx_msg void OnEditCfg();
    afx_msg void OnNewCfg();
    afx_msg void OnCopyCfg();
    //}}AFX_MSG
    afx_msg BOOL OnToolTipNotify(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnUpdateButton(CCmdUI* pCmdUi);
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================

class DllImportExport CModelInfo
  {
  public:
    Strng m_sCfgFile;
    Strng m_sName;
    Strng m_sDesc;
    Strng m_sCfgFiles;
    Strng m_sCfgHome;
    //Strng m_sCfgFolder;
    Strng m_sDLLList;
    flag  fNewTrend;
    flag  fNewGraph;

    struct CModelInfoSaved
      {
      Strng m_sCfgFiles;
      Strng m_sCfgHome;
      } Saved;

    CModelInfo()
      {
      fNewTrend=fNewGraph=true;
      }

    static int  CfgInfoFmt(char * FileName);
    byte        GetCfgInfo(char * FileName, flag DoDllLoad, flag Whinge);
    //byte        GetCfgInfoOld(char * FileName, flag Whinge);
    flag        GetSetName(char * PrjFile, flag ForNew, int OldPrjFileVer, DWORD LicCategory, LPCTSTR RqdCfgFile=NULL);
    void        FixCfgHome();
    flag        EditCfgFile(char *FileName, DWORD LicCategory);
    flag        EnsureCfgIsInList();
    static void MakeDefaultNewCfgFile(char* pCfgFile, char* pCfgFiles, char* pCfgHome, char * Desc);
  };

class DllImportExport CModelInfoUpgrade
  {
  public:
    Strng     m_OldFilePath;


    //flag      UpgradeCfgFile(char * FileName, CUpGrdCompletion & UGC);

    int       CheckCfgFileFormat(char *Filename, flag SetFlags);
    flag      CheckDBFileFormat(char *Filename);
  };


//===========================================================================

extern DllImportExport int DLLVersionOK(char * FullName);

// ==========================================================================

_FWDDEF(SFEMdlLib)
class DllImportExport SFEMdlLib  
  {
  protected:
    HINSTANCE hSFELib;
  
  public:
    Strng     sMdlDLLPath;
    Strng     sMdlDLLName;

    SFEMdlLib();
    virtual ~SFEMdlLib();

    flag Load(pchar MdlDLL);
    flag UnLoad();
    flag InitCfg();

    virtual flag   Initialise(pchar ModelCfg, pchar FlwLibTag, pchar EO_LocationTag);
    virtual flag   Terminate();
    virtual flag   Loaded() { return hSFELib!=NULL;};
  };

// ==========================================================================

class DllImportExport SFEMdlLibArray : public CArray <SFEMdlLib*, SFEMdlLib*>
  {
  public:
    flag           LoadFlwDLLs(Strng &DLLList, pchar FlwLibTag, pchar EO_LocationTag, SFEMdlLib * pTheFlwLib);
    flag           UnLoadFlwDLLs(flag DoTerminate=True);
  };

//===========================================================================

extern DllImportExport flag CheckForDuplicateClassIds();
extern DllImportExport void InitClassLicenses(dword dwLic);

//===========================================================================

extern DllImportExport void DumpAdtBalance(DWORD Options, CWnd *pWnd, CNodeAuditInfo & Audit);
extern DllImportExport void DumpClsBalance(bool DoCompact, bool DoSummary, CWnd *pWnd, CNodeBalanceInfo & Balance);

//===========================================================================

#undef DllImportExport

#endif
