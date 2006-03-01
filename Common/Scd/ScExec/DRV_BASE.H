//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __DRV_BASE_H
#define  __DRV_BASE_H

#ifndef __EXECLIB_H
  #include "execlib.h"
#endif
#ifndef __DATACNVS_H
  #include "datacnvs.h"
#endif
#include "limits.h"
#ifndef __NOISE_H
  #include "noise.h"
#endif
#ifndef __PGM_E_H
  #include "pgm_e.h"
#endif


#ifdef __DRV_BASE_CPP
  #define DllImportExport DllExport
#elif !defined(SCEXEC)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

#define WITHDRVDEBUG 1

//------------------------------------------------------------------------

_FWDDEF(CDriver)
_FWDDEF(CDrvSubsConn)
_FWDDEF(CDrvSlot)
_FWDDEF(CDrvCfgHelper)
_FWDDEF(CDriverManagerBase);

typedef CDriver* (*fn_AddDrv)(char*);

extern DllImportExport const char* OPC_DLL_Name;

//---------------- DRIVER INPUT SPAN
_FWDDEF(DrvSpan)
class DllImportExport DrvSpan
  {
  public:
    long Span;//DWORD Span; changed KGA 23/5/96
    long Offset;//DWORD Offset; changed KGA 23/5/96
    long ClampLo; // CNM
    long ClampHi; // CNM
    DrvSpan() { Span = 0; Offset = 0; ClampLo=LONG_MIN; ClampHi=LONG_MAX; };
    DrvSpan(long span, long offset, long clamplo, long clamphi) 
      { Span = span; Offset = offset; ClampLo=clamplo; ClampHi=clamphi; };

    flag ClampValid() { return ClampLo!=LONG_MIN || ClampHi!=LONG_MAX; };
    void ClearClamp() { ClampLo=LONG_MIN; ClampHi=LONG_MAX; };
  };

//---------------- DRIVER OUTPUT RANGES
_FWDDEF(DrvRange)
class DllImportExport DrvRange
  {
  public:
    double RngHi;
    double RngLo;
    double Range;
    DrvRange() { RngHi = 0.0; RngLo = 0.0; Range = 0.0; };
    DrvRange(double dRngLo, double dRngHi) { RngHi = dRngHi; RngLo = dRngLo; Range = RngHi - RngLo; };
  };

//---------------- HISTORIAN SETTINGS
_FWDDEF(DrvHstCfg)
class DllImportExport DrvHstCfg
  {
  public:
    double dFilterTau;      
    double dWinFltPeriod;
    double dFltDelta;
    double dDeltaLo;
    double dDeltaHi;
    double dDecrease;
    int    iWinFltCount;
    int    iNoRec;
    int    iNoNotRec;
    int    iFilterAlg;
    int    iBoxcarAlg;
    DrvHstCfg();
    DrvHstCfg(const pDrvHstCfg pH) { *this = *pH; };
  };

//---------------- ARCHIVE SETTINGS
_FWDDEF(DrvArcCfg)
class DllImportExport DrvArcCfg
  {
  public:
	  Strng        m_sTable;
    
    byte         m_iFn;

    DrvArcCfg();
    DrvArcCfg(const pDrvArcCfg pH) { *this = *pH; };
  };

class DrvArcCfgArray : public CArray<DrvArcCfg*, DrvArcCfg*> {};

//------------------------------------------------------------------------

class CDrvCodeBlk
  {
  public:
    CDrvCodeBlk() : m_Code(/*NULL,*/ NULL) {};
    Strng          m_sTag;
    GCInsMngr      m_Code;    
    CArray <CDrvSubsConn*, CDrvSubsConn*> m_GetsFromMe; // Array of Connects which get from here
  };

//------------------------------------------------------------------------

const DWORD InfiniteDelay=0xffffffff;

_FWDDEF(CDelayItem)
class DllImportExport CDelayItem
  {
  public:
    CDrvSubsConn *pCon;
    CDelayItem  *pNxt;
    CDelayItem  *pPrev;
    DWORD        dwStartTime;
    DWORD        dwDelayTime;
    DWORD        dwNumber;
    PkDataUnion  Data;
    CDelayItem(CDrvSubsConn *pConSlot, PkDataUnion& Dat, DWORD DelayTime);
   
  DEFINE_SPARES(CDelayItem);
  };

//------------------------------------------------------------------------

_FWDDEF(CDrvSltChange)
class DllImportExport CDrvSltChange
  {
  public:
    CDrvSlot    *m_pSlot;
    CDrvCodeBlk *m_pCdBlk;    
    GCVar       *m_pCdBlkVar;
    long         DataIndex;
    double       dVal;
    byte         iTyp;
	  CCnvIndex    m_iCnv;
	  Strng        m_sCnvTxt;

    CDrvSltChange(CDrvSlot * pSlot, long lDataIndex, double d, byte Typ, CCnvIndex Cnv, LPCTSTR CnvTxt);
    CDrvSltChange(CDrvCodeBlk *pCdBlk, GCVar *pCdBlkVar, long lDataIndex, double d, byte Typ, CCnvIndex Cnv, LPCTSTR CnvTxt);
  
  DEFINE_SPARES(CDrvSltChange);
 
  };

//------------------------------------------------------------------------

const long ConIndexStart = 25000;

//------------------------------------------------------------------------

struct CDrvSubsConnPrfPt
  {
  float X,Y;
  };

class DllImportExport CDrvSubsConnPrf 
  {
  protected:
    Strng      m_sTag; //FileName etc
    enum { eNULL, eABS, eSCL, eSCLPERC, eCONTRONIC } m_Mode;
    bool m_bYReversed;
    CArray <CDrvSubsConnPrfPt, CDrvSubsConnPrfPt&> m_Points;

  public:
    CDrvSubsConnPrf ();
    long Parse(LPCSTR File);

    double X2Y(double X);
    double Y2X(double Y);
     
    LPCTSTR ErrorString(long RetCode);
  };

const byte DrvSubsConnMaxOps=8;

class DllImportExport CDrvSubsConnOp_Base
  {
  protected:

  public:
    virtual double Exec(double d, double DeltaTime)=0;
  };

class DllImportExport CDrvSubsConnOp_Rev : public CDrvSubsConnOp_Base
  {
  protected:
    double m_dOffVal;  
  public:
    CDrvSubsConnOp_Rev(double i_dOffVal)
      {
      m_dOffVal=i_dOffVal;
      };
    virtual double Exec(double d, double DeltaTime)
      {
      return m_dOffVal-d;
      };
  };

class DllImportExport CDrvSubsConnOp_Not : public CDrvSubsConnOp_Base
  {
  public:
    CDrvSubsConnOp_Not()
      {
      };
    virtual double Exec(double d, double DeltaTime)
      {
      return d == 0.0 ? 1.0 : 0.0;
      };
  };

class DllImportExport CDrvSubsConnOp_Abs : public CDrvSubsConnOp_Base
  {
  protected:
  
  public:
    CDrvSubsConnOp_Abs()
      {
      };
    virtual double Exec(double d, double DeltaTime)
      {
      return fabs(d);
      };
  };

class DllImportExport CDrvSubsConnOp_Cmp : public CDrvSubsConnOp_Base
  {
  protected:
    int m_iCmpTest;
    double m_dCmpVal;  
  public:
    CDrvSubsConnOp_Cmp(int iOp, double i_dCmpVal)
      {
      m_iCmpTest=iOp;
      m_dCmpVal=i_dCmpVal;
      };
    virtual double Exec(double d, double DeltaTime)
      {
      switch (m_iCmpTest)
        {
        case -2:return d<m_dCmpVal;
        case -1:return d<=m_dCmpVal;
        case  0:return d==m_dCmpVal;
        case  1:return d>=m_dCmpVal;
        case  2:return d>m_dCmpVal;
        default:return false;
        }
      };
  };

class DllImportExport CDrvSubsConnOp_Range : public CDrvSubsConnOp_Base
  {
  protected:
    double m_dMin;    
    double m_dMax;  

  public:
    CDrvSubsConnOp_Range(double i_dMin, double i_dMax)
      {
      m_dMin=i_dMin;
      m_dMax=i_dMax;
      };
    virtual double Exec(double d, double DeltaTime)
      {
      d=Range(m_dMin, d, m_dMax);
      return d;
      };
  };

class DllImportExport CDrvSubsConnOp_Sqr : public CDrvSubsConnOp_Base
  {
  protected:
    double m_dMult;   
    double m_dMin1;  
    double m_dMin2;  
    double m_dMax1;  
    double m_dMax2;  

  public:
    CDrvSubsConnOp_Sqr(double i_dMult, double i_dMin1, double i_dMax1, double i_dMin2, double i_dMax2)
      {
      m_dMult=i_dMult;
      m_dMin1=i_dMin1;
      m_dMax1=i_dMax1;
      m_dMin2=i_dMin2;
      m_dMax2=i_dMax2;
      };
    virtual double Exec(double d, double DeltaTime)
      {
      if (m_dMax1!=m_dMin1)
        d=(d-m_dMin1)/(m_dMax1-m_dMin1);
      d = Sign(d)*Sqr(d*m_dMult);
      if (m_dMax2!=m_dMin2)
        d=m_dMin2 + d*(m_dMax2-m_dMin2);
      return d;
      };
  };

class DllImportExport CDrvSubsConnOp_Sqrt : public CDrvSubsConnOp_Base
  {
  protected:
    double m_dMult;   
    double m_dMin1;  
    double m_dMin2;  
    double m_dMax1;  
    double m_dMax2;  

  public:
    CDrvSubsConnOp_Sqrt(double i_dMult, double i_dMin1, double i_dMax1, double i_dMin2, double i_dMax2)
      {
      m_dMult=i_dMult;
      m_dMin1=i_dMin1;
      m_dMax1=i_dMax1;
      m_dMin2=i_dMin2;
      m_dMax2=i_dMax2;
      };
    virtual double Exec(double d, double DeltaTime)
      {
      if (m_dMax1!=m_dMin1)
        d=(d-m_dMin1)/(m_dMax1-m_dMin1);
      d = Sign(d)*Sqrt(fabs(d))*m_dMult;
      if (m_dMax2!=m_dMin2)
        d=m_dMin2 + d*(m_dMax2-m_dMin2);
      return d;
      };
  };

class DllImportExport CDrvSubsConnOp_Profile : public CDrvSubsConnOp_Base
  {
  protected:
    bool m_bReverse;
    CDrvSubsConnPrf * m_pXY;

  public:
    CDrvSubsConnOp_Profile(bool i_bReverse, CDrvSubsConnPrf * i_pXY)
      {
      m_bReverse=i_bReverse;
      m_pXY=i_pXY;
      };
    virtual ~CDrvSubsConnOp_Profile()
      {
      delete m_pXY;
      };
    virtual double Exec(double d, double DeltaTime)
      {
      if (m_bReverse)
        d = m_pXY->Y2X(d); 
      else
        d = m_pXY->X2Y(d); 
      return d;
      };
  };

class DllImportExport CDrvSubsConnOp_Filter : public CDrvSubsConnOp_Base
  {
  protected:
    byte m_iType;
    double m_dTau;         
    double m_dPrev;         

  public:
    CDrvSubsConnOp_Filter(byte i_iType, double i_dTau)      
      {
      m_iType=i_iType;
      m_dTau=Max(0.001, i_dTau);
      m_dPrev=fNAN;
      };
    virtual double Exec(double d, double DeltaTime)
      {
      double Alpha = 1.0-exp(-DeltaTime/m_dTau);
      if (Valid(m_dPrev))
        d = d * Alpha + (1-Alpha)*m_dPrev; 
      m_dPrev=d; 
      return d;
      };
  };

class DllImportExport CDrvSubsConnOp_Noise : public CDrvSubsConnOp_Base , protected CNoise
  {
  protected:
    //CNoise   * m_pFn;                       
    bool m_Relative;

  public:
    CDrvSubsConnOp_Noise(bool i_Relative, NoiseTypes i_iType, double i_dStdDev, double i_dMean=0.0)
      {
      SetType(NoiseTypes(i_iType));
      SetGlobal(false);
      SetSeed(rand());
      Init(i_dMean, i_dStdDev); 
      m_Relative=i_Relative;
      };
    virtual double Exec(double d, double DeltaTime)
      {
      double N=GetVal();
      d += (m_Relative ? d * N : N);
      return d;
      };
  };

class DllImportExport CDrvSubsConnOp_Scale : public CDrvSubsConnOp_Base
  {
  protected:
    double     m_dMult;            
    double     m_dOffset;            

  public:
    CDrvSubsConnOp_Scale(double i_dMult, double i_dOffset)            
      {
      m_dMult=i_dMult;
      m_dOffset=i_dOffset;
      };
    virtual double Exec(double d, double DeltaTime)
      {
      return d * m_dMult + m_dOffset;
      };
  };

class DllImportExport CDrvSubsConnOp_Mask: public CDrvSubsConnOp_Base
  {
  protected:
    long m_lMask;
  public:

    CDrvSubsConnOp_Mask(bool AsBitNo, long Val)
      {
      m_lMask=AsBitNo ? (1<<Val) : Val;
      };
    virtual double Exec(double d, double DeltaTime)
      {
      return ((long)Range((double)LONG_MAX, d, (double)LONG_MIN) & m_lMask);
      };
  };


class DllImportExport CDrvSubsConn
  {
  public:
    Strng      m_sTag;       //tag for set or get
    Strng      m_sCnvTxt;    //conversion text of Src/Dest
    CCnvIndex  m_iCnv;       //conversion index of Src/Dest
    byte       m_Typ;        //tt_XXX type of the tag for the set or get
    byte       m_nOps;
    CDrvSubsConnOp_Base *m_Op[DrvSubsConnMaxOps];

    WORD       m_bGet:1,     //is this a get
               bInternal:1,  //is this connection internal to the driver
               bInv:1,       //perform a logical not before set or get
               bDelayLock:1, //allready busy with a delay
               bUseDelay2:1, //must the second delay time be used for bit=1
               bEdge:1;      //bit slot, reacting to rising or falling edge (one delay time = INF)

    WORD       iSubsIndex;   //index into array of SubsItems (valid for non internal connections)
    DWORD      dwDelayTime;  //time in mSecs for delay for sets
    DWORD      dwDelayTime2; //time in mSecs for delay for sets for bit slot

    CDrvSlot        *m_pParentSlot; //pointer to parent slot
    
    CDrvSlot        *m_pSlot;       //pointer to the internal slot
    CDrvCodeBlk     *m_pCdBlk;      //pointer to CodeBlock (either this or m_pSlot)
    GCVar           *m_pCdBlkVar;   //pointer to CodeBlock variable

    CDrvSubsConn    *m_pNxt;        //next ConSlot for the driver slot
    CDrvSubsConn    *m_pNxtSubs;    //next ConSlot for the same tag

    CDrvSubsConn(pchar pTag, pchar pCnvTxt, flag Get, flag Inv);
    ~CDrvSubsConn(); 
    void SetDelayTimes(DWORD DelayTime, DWORD DelayTime2, flag UseDelay2);
    static CDrvSubsConn * AddGetConnect(pCDrvSubsConn& pFirst, pchar pTag, flag Inv);
    static CDrvSubsConn * AddSetConnect(pCDrvSubsConn& pFirst, pchar pTag, flag Inv, DWORD DelayTime, DWORD DelayTime2, flag UseDelay2);
    void  AddOp(CDrvSubsConnOp_Base* i_pOp) { m_Op[m_nOps++]=i_pOp; };

    bool PutSubsValue2Slot(CDriverManagerBase * pMngr, CPkDataItem *pPItem, bool ForcesOn, double DeltaTime, bool AllowNoise);
  };

//------------------------------------------------------------------------

class DllImportExport CProcessConnItem
  {
  public:
    CProcessConnItem * m_pNext;
    flag            m_bDouble;
    double          m_rVal;
    long            m_lVal;
    flag            m_bDoingForceSet; 
    CDrvSubsConn  * m_pCon; 
    CDrvSlot      * m_pSlt; 
    CDriver       * m_pDrv;
    long            m_lLevel;

    DEFINE_SPARES(CProcessConnItem);
  };


class DllImportExport CDrvSlot
  {  
  public:                  
    Strng            sTag;             //slot tag
    byte             iTyp;             //tt_XXX SysCAD data type
    DWORD            bChanged:1,       //has the slot value changed
                     bSetReqd:1,       //
                     m_bReqdByExec:1,          //does the EO manager require this slot; ie does SysCAD's subscription lists need to access this slot in any way
                     bHstCfgd:1,       //
                     bArcCfgd:1,       //
                     bValInited:1,     //
                     bValueWritten:1,  //
                     bRecordIt:1,      //must the historian record this slot
                     bIgnoreWrites:1,  //if True, no values must be written to the driver
                     bIgnoreReads:1,   //if True, no values must be read from the driver
                     bLocal:1,         //local var, must be retained
                     bUseInitVal:1,    //must the local var be initialised
                     m_bHasSet:1,      //has a connect that must be set
                     m_bHasSetAsGet:1, //has a connect that must be set by getting remotely *(CodeBlks)
                     bHoldOn:1,        //
                     bEnableForces:1,  //if True, the value of dForceVal is used for that slot regardless
                     bHasFarCon:1,     //true if some other driver slot gets or sets this slot
                     bHasGetConn:1,    // true if one of the conns is a get
                     bSltDbgOn:1;
    DWORD            SlotNo  :21,      //slot index in CDrvMngr array of slots
                     chgcount: 7,
                     iStatus : 4;
    double           dInitVal;         //initial value for a local variable
    double           dForceVal;        //force value if forces is on
    XIOAction        iAction;          //IO type
    CCnvIndex        m_iCnv;             //conversion index
    Strng            m_sCnvTxt;          //engeneering units
    DDBValueLstMem * m_pValLst;
    int              iCnt;
    Strng            sDescription;     //tag description
    Strng            sConnection;      //tag connection ( for display)
    CDriver         *pDrv;             //pointer to driver  
    DrvHstCfg       *pHstCfg;          //historian configuration
    DrvArcCfgArray   ArcCfg;           //archive configuration
    DrvSpan         *pSpan;            //span, offset configuration
    DrvRange        *pRange;           //range configuration
    CDrvSubsConn    *m_pConnects;       //linked list of related slots
    short           m_bProcessConnectsBusy;

    #if WITHDRVDEBUG 
    double           dbgSrcReadTime;
    #endif

    CDrvSlot(pchar pTag);
    virtual ~CDrvSlot();
    virtual flag     GetValue(PkDataUnion& Data) = 0;
    virtual flag     SetValue(PkDataUnion& Data) = 0;
    virtual flag     dbgGetValue(PkDataUnion& Data) = 0;
    virtual void     GetDrvDesc(CString& s);
    virtual void     GetLongDrvDesc(CString& s);
    virtual void     GetTagOrAddrDesc(CString& s);
    virtual long     GetRawValueLong() { return 0; };
    virtual double   GetRawValueDble() { return GetRawValueLong(); };

    flag             GetTagValue(CPkDataList &List, CPkDataItem * &pPItem, flag Complete);
    flag             PutTagValue(PkDataUnion& Data);
    inline flag      Changed() { return (flag)bChanged; };
    inline void      ClearChanged() { bChanged = 0; };
    inline flag      SetReqd() { return (flag)bSetReqd; };    

    void             ProcessConnects(flag Chng, flag WriteThru, flag FromOutput, flag ForceSet, long Level=0);
    void             ProcessConnects(CProcessConnItem * &pItemListHead, CProcessConnItem *&pItemList, flag Chng, flag WriteThru, flag FromOutput, flag ForceSet, long Level=0);
    virtual void     ClearFBKChange() = 0;
    virtual flag     TestVal(double v, flag IsForceSet) = 0;
    virtual flag     TestVal(long v, flag IsForceSet) = 0;
    virtual flag     SetVal(double v, flag IsForceSet) = 0;
    virtual flag     SetVal(long v, flag IsForceSet) = 0;
    virtual void     GetTheVal(PkDataUnion& Data, flag FromOutput) = 0;
    virtual CDelayItem * GetDelayItemsForSlot() = 0;
    virtual void     InsertDelayItem(CDelayItem * pDelay);

    flag AddProcessConnItem(CProcessConnItem * &pItemListHead, CProcessConnItem *&pItemList, 
                        flag AsDouble,
                        double rVal, long lVal, flag DoingForceSet, CDrvSubsConn* pCon, CDrvSlot * pSlt, CDriver * pDrv, long Level);
    void ApplyProcessConnItems(CProcessConnItem * pItemListHead);

  };

//------------------------------------------------------------------------

class DllImportExport CDrvCfgHelper
  {
  public:
    Strng             sTag;
    XIOAction         iAction;
    byte              iTyp;
    Strng             sDesc;
    Strng             sConn;
    int               m_iCnv;
    Strng             m_sCnvUnits;
    DDBValueLstMem  * m_pValLst;
    Strng             sDrvCfg;
    CDriver         * pDrv;
    CDrvSubsConn    * pConSlot;
    DrvSpan           nS;
    DrvRange          nR;
    DrvHstCfg         nH;
    DrvArcCfgArray    nA;
    double            dInitVal;
    double            dForceVal;
    word              bRecordIt:1,
                      bUseRange:1,
                      bUseSpan:1,
                      bLocal:1,
                      bUseInitVal:1,
                      bIgnoreWrites:1,
                      bIgnoreReads:1,
                      bEnableForces:1,
                      bSltDbgOn:1;
                      
    word              bErrorFlagged:1; // CNM
    long              lLineNumber;    // CNM
    long              lSlotIndex;     // CNM

    CDrvCfgHelper()
      {
      iTyp = tt_NULL;
      m_iCnv = 0;
      m_pValLst=NULL;
      pDrv = NULL;
      pConSlot = NULL;
      bRecordIt = 0;
      bUseRange = 0;
      bUseSpan = 0;
      bLocal = 0;
      bUseInitVal = 0;
      bErrorFlagged=0;
      lLineNumber=-1;
      lSlotIndex=-1;
      };
  };

//------------------------------------------------------------------------

enum DriverStates { DS_Inactive, DS_Active, DS_Error };

//------------------------------------------------------------------------
//Pure virtual base class from which the driver manager is derived. This 
//allows individual drivers to call a virtual method to access the driver
//manager object indirectly. Individual drivers do NOT have direct access 
//to the driver manager.
class DllImportExport CDriverManagerBase
  {
  public:
    CDriverManagerBase();
    virtual void   AddSltChange(long DataIndex, double dVal, CDrvSubsConn * pCon, long Level=0) = 0;
    virtual void   AddSltChange(long DataIndex, double dVal, CDrvSlot * pSlt, long Level=0) = 0;
  };

//------------------------------------------------------------------------

class CDrvTimeout
  {
  public:
    DWORD m_dwStartTicks;
    DWORD m_dwTOMult;
    bool m_bLocalTO;

    CDrvTimeout(bool LocalTO, DWORD Mult)
      {
      m_dwStartTicks=GetTickCount();
      m_dwTOMult=Mult;
      m_bLocalTO=LocalTO;
      }
    bool TimedOut(DWORD mSecs)
      {
      DWORD Ticks=GetTickCount();
      if (Ticks<m_dwStartTicks)
        m_dwStartTicks=GetTickCount();
      return (Ticks-m_dwStartTicks)>mSecs*m_dwTOMult;
      }
    bool LocalTO() { return m_bLocalTO; };
  };

//------------------------------------------------------------------------
//Pure virtual base class from which drivers in other DLL's are derived

#define DBGDRIVERLOCKS 0

class DllImportExport CDriver
  {  
  protected:
    CRITICAL_SECTION  critical_section;   //used for locks
  public:                  
    HINSTANCE        hLib;                //handle to driver DLL
    Strng            sTag;                //name of driver
    CDriverManagerBase* pDrvMngr;         //indirect access to driver manager through base class
    byte             bDoneSave:1,         //has save been started for this driver
                     bConfigIsBusy:1,     //Flag to indicate config is finished
                     bTwoCfgPasses:1,
                     bDoWriteThru:1,      // Moved from derived classes
                     bLclTagSrvrOK:1;
    
    Strng            CfgFile;             //keep this
    Strng            DllFile;             //keep this
#if DBGDRIVERLOCKS
    double      dbgLockTime;
    CStopWatch   SW;
    double AccLockTime() { double T=dbgLockTime; dbgLockTime=0.0; return T; };
#endif

    CDriver(const char* Tag);
    virtual ~CDriver();

    inline  void xLock()
      { 
#if DBGDRIVERLOCKS
      double X=SW.Time();
      EnterCriticalSection(&critical_section); 
      dbgLockTime+=SW.Time()-X;
#else
      EnterCriticalSection(&critical_section); 
#endif
      }
    inline  void xUnLock()
      {
      LeaveCriticalSection(&critical_section);
      }


    virtual void      BitBase(int &Base, int &WordWidth)=0;
    virtual flag      Configure(const char * Cfg) { return True; };
    virtual void      Open() {};
    virtual void      Close() {};
    virtual flag      Start() = 0;        //called by CDrvMngr::EO_Start
    virtual flag      Stop() = 0;         //called by CDrvMngr::EO_Stop
    virtual flag      ReadSubsData() = 0; //called by CDrvMngr::EO_ReadSubsData
    virtual flag      ReadTaggedItem(pchar pTag, CDrvCfgHelper * pCfg) = 0; //called by CDrvMngr::EO_ReadTaggedItem
    virtual flag      Execute(const CXM_TimeControl &CB) = 0;  //called by CDrvMngr::EO_Execute
    virtual pCDrvSlot AddSlot(CDrvCfgHelper * pCfg) = 0;
    virtual flag      CfgSlot(CDrvCfgHelper * pCfg, pCDrvSlot pSlot = NULL) = 0;
    virtual int       InitSlot(pCDrvSlot pSlot, CDrvTimeout * pTO) { return 0; }; // -1=Busy 0=Done 1=Fail
    virtual int       RefreshSlotData(pCDrvSlot pSlot, flag ConnOnly, flag DoInputs, flag DoOutputs, CDrvTimeout * pTO) { return 0; }; // -1=Busy 0=Done 1=Fail
    virtual void      ClearChanges(void) = 0;
    virtual pCDrvSlot GetNextChange(void) = 0;
    virtual DWORD     NumberOfChanges(void) = 0;
    virtual void      ClearConfigBusy(void) { bConfigIsBusy=0; }; // Flag to indicate config is finished
    virtual flag      IsConfigComplete() { return True; }; //= 0;
    virtual int       AddPages(CPropertyPage**Pages) { return 0; };
    virtual flag      HasLoadSave() = 0;
    virtual flag      BeginSave(FilingControlBlock &FCB) = 0; //called by CDrvMngr::EO_BeginSave
    virtual flag      SaveDefinedData(FilingControlBlock &FCB, Strng &Tag, CXMsgLst &XM) { return False; }; //called by CDrvMngr::EO_SaveDefinedData
    virtual flag      SaveOtherData(FilingControlBlock &FCB) { return False; }; //called by CDrvMngr::EO_SaveOtherData
    virtual flag      BeginLoad(FilingControlBlock &FCB) = 0; //called by CDrvMngr::EO_BeginLoad
    virtual flag      LoadDefinedData(FilingControlBlock &FCB, CXMsgLst &XM) { return False; }; //called by CDrvMngr::EO_LoadDefinedData
    virtual flag      LoadOtherData(FilingControlBlock &FCB) { return False; }; //called by CDrvMngr::EO_LoadOtherData
    virtual void      DumpIOMap() {};
    virtual DriverStates State() { return DS_Inactive; };
    virtual LRESULT   ProcessMessage(WPARAM wParam, LPARAM lParam) { return FALSE; };

    virtual void      DrvError(CDrvCfgHelper * pCfg, DWORD Flags, char *Drv, char * fmt, ...);
    virtual void      InsertDelayItem(CDelayItem * pDelay) = 0;
    #if WITHDRVDEBUG 
    static double     TimeFromStart();
    #endif
  };

//------------------------------------------------------------------------

inline void CDrvSlot::InsertDelayItem(CDelayItem * pDelay)
  {
  pDrv->InsertDelayItem(pDelay);
  }

#undef DllImportExport 

#endif
