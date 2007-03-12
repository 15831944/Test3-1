//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __TAGVDOC_H
#define __TAGVDOC_H

#ifndef __DOCROOT_H
#include "docroot.h"
#endif
#ifndef __EXECUTIV_H
#include "executiv.h"
#endif
#ifndef __DATAATTR_H
#include "dataattr.h"
#endif
#ifndef __PGM_E_H
#include "pgm_e.h"
#endif
#ifndef __PROJECT_H
#include "project.h"
#endif

#define WITHAUTOSCALE 1

//===========================================================================

const long  TGU_Toggle                = 0x00000001;
const long  TGU_LoadComplete          = 0x00000002;
const long  TGU_Start                 = 0x00000004;
const long  TGU_TagView               = 0x00000008;
const long  TGU_Stop                  = 0x00000010;
const long  TGU_UpdateAll             = 0x00000020;
const long  TGU_Colour                = 0x00000040;
const long  TGU_FixTimeBase           = 0x00000080;
const long  TGU_ClearTrendDisplay     = 0x00000100;
const long  TGU_LoadDataComplete      = 0x00000200;
const long  TGU_Repaint               = 0x00000400;
const long  TGU_NowActivated          = 0x00001000;
const long  TGU_NowDeactivated        = 0x00002000;
const long  TGU_UpdateMinMax          = 0x00004000;

const byte  TAS_Off                   = 0;
const byte  TAS_FitMin                = 1;
const byte  TAS_Fit1                  = 2;
const byte  TAS_Fit10                 = 3;
const byte  TAS_Fit50                 = 4;
const byte  TAS_FitZMin               = 5;
const byte  TAS_FitZ1                 = 6;
const byte  TAS_FitZ10                = 7;
const byte  TAS_FitZ50                = 8;
//const byte  TAS_FitZero               = 5;

//===========================================================================

_FWDDEF(CTagVwDoc)
_FWDDEF(CTagVwDocC)
_FWDDEF(CTagVwSlot)
_FWDDEF(CTagVwCmdBlk)
_FWDDEF(CTrendSlot)
_FWDDEF(CTrendPtBlkList)

//-------------------------------------------------------------------------

class CTagVwCmdBlk : public DocCmdBlk
  {
  DEFINE_MEMSTATS(CTagVwCmdBlk)
  public:
    CView*          pTrnd;
    CView*          pText;

  public:
    CTagVwCmdBlk(int ExtraCmds);
    ~CTagVwCmdBlk();
    virtual void   InitForCmd();
    virtual void   OnActivateDoc(flag bActivate);
    virtual void   BuildVNT();
    virtual flag   UpdateCmdStr(int nID, CCmdUI* pCmdUI);
    virtual flag   ProcessCmdStr(int nID, rStrng Str);

    void           DoActivate();
    void           DoScroll();
    
  public: // Document Methods
    virtual void   DumpErrors();
    virtual void   AttachDoc(pDocRoot pDoc_, CView* pTrnd_, CView* pText_);
    virtual BOOL   ReadDocument(const char *pszPathName, FILE* pFile);
    virtual BOOL   WriteDocument(const char *pszPathName, FILE* pFile);

  };


//=========================================================================

const int SlotsPerCtrl        = 4;
const int CtrlHeaderSlots     = 13;

const int MinTrendSlots       = 40;

//const int TrndSubsStartIndex  = 0+64;  //starting index for EO_ tag subscriptions for trend functions

// Trend Scaling
const int TrendLUperPT  = 1;
const int MaxTrendPts   = 4096;
const long TrendXMax    = TrendLUperPT*(MaxTrendPts-16)/4;  // 16 spare
const long TrendYMax    = TrendXMax; // to keep logical units approximately isotropic

const int ColourArrayLen=16;
typedef COLORREF ColourArray[ColourArrayLen];

const byte STT_Empty        = 0;
const byte STT_Funct        = 1;
const byte STT_Comment      = 2;
const byte STT_Data         = 3;
const byte STT_DataPending  = 4;

//-------------------------------------------------------------------------

const byte CT_CheckFirst  = 0x01;
const byte CT_GetRecorded = 0x02;

class CTagVwSlot
  {
  DEFINE_MEMSTATS(CTagVwSlot)
  public:
    Strng          sTag;
    Strng          sValue;
    Strng          sMinVal;
    Strng          sMaxVal;
    Strng          sDesc;
    Strng_List     sStrList;
    Strng          sTagOnly;
    Strng          sCnvTxt;

    byte           bSlotType;
    byte           bScaleApplied;
    flag           bAutoScaleOn;

    long           nTrendNo;
    double         dValue;
    double         dValueLowest;
    double         dValueHighest;
    LOGPEN         lPen;
    DWORD          dwWidth;
    byte           bTagValid:1,
                   bValueValid:1,
                   bFunctValid:1,
                   bHasFunct:1,
                   bChgd:1,
                   bHex:1;
    byte           bRecorded:1,
                   m_bEditableStopped:1,
                   m_bEditableRunning:1,
                   bIsResult:1,
                   bDriverTag:1,
                   bArchiveTag:1,
                   bCtrlSlot:1;
    int            iNextChgdFld;
    char           cType;
    GCVar*         pFunctVar;
    CnvAttribute   Cnv;
    FmtAttribute   Fmt;
    CTagRefStatus  m_XRefStatus;

    CTagVwSlot();
    ~CTagVwSlot();
    void Clear(int iSlot);
    void ClearIt(pCTagVwDoc pDoc);
    byte TestSlotType();
    flag SetSlotAsFunct(pCTagVwDoc pDoc);
    flag SetSlotAsData(pCTagVwDoc pDoc, CPkDataItem & Item);
    void SetSlotAsEmpty(pCTagVwDoc pDoc);
    flag ConnectTag(pCTagVwDoc pDoc, byte Opts);// BOOL CheckFirst);
    flag SetTrending(int TrendNo, pCTagVwDoc pDoc);
    flag RecordTag(pCTagVwDoc pDoc, flag RecordingOn);
    flag ShowDriverTagInfo(pCTagVwDoc pDoc, WORD DrvOptMask);
    flag ShowArchiveTagInfo(pCTagVwDoc pDoc, WORD DrvOptMask);
    flag ChangeTag(char* pNewTag, pCTagVwDoc pDoc, COLORREF DefColour);
    flag ChangeCnv(char* pNewCnv);
    flag SetOutput(pCTagVwDoc pDoc, pchar s, flag PromptNeeded=False, flag PermissionNeeded=False);
    void LoadData(pCTagVwDoc pDoc);
    inline flag MeasChanged() { return bChgd; };
    inline void ClearChanged() { bChgd = 0; };
    inline flag TagValid() { return bTagValid; };
    inline flag ValueValid() { return bValueValid; };
    inline flag FunctValid() { return bFunctValid; };
    inline flag HasFunct() { return bHasFunct; };
    inline flag IsCtrlSlot() { return bCtrlSlot; };
    inline flag IsBlank() { return (/*sTag()==NULL || */sTag.Len()==0); };
    inline flag IsComment() { return (sTag.Len()>0 && sTag[0]=='\''); };
    inline pchar Tag() { return sTag(); };
    inline pchar SetTag(char* p) 
      {
      sTag=p;
      sTag.Trim(" \t\n\r\v\f");
      TaggedObject::SplitTagCnv(sTag(), sTagOnly, sCnvTxt);
      return sTag();
      };
    inline pchar SetTag(char* p, char* q, char*r=NULL)
      {
      sTag.Concat(p,q,r);
      sTag.Trim(" \t\n\r\v\f");
      TaggedObject::SplitTagCnv(sTag(), sTagOnly, sCnvTxt);
      return sTag();
      };

    inline pchar TagOnly() { return sTagOnly(); };
    inline pchar CnvTxt() { return sCnvTxt(); };

    int         NoSlots(pCTagVwDoc pDoc);
    int         NoTrends(pCTagVwDoc pDoc);
    CTagVwSlot& DS(pCTagVwDoc pDoc, int i);
    CTrendSlot& Trend(pCTagVwDoc pDoc, int i);
    void        DoFit(byte How, double &Mn, double &Mx, bool KeepMn=false, bool KeepMx=false);
    void        SetRange(double Mn, double Mx);
    void        UpdateYMnMx(pCTagVwDoc pDoc);

  };

//===========================================================================

_FWDDEF(TrndTimeBase)
class TrndTimeBase
  {
  DEFINE_MEMSTATS(TrndTimeBase)
  public:
    CTimeValue     EndTime;
    CTimeValue     StartTime;
    flag           TrackingTime;
    flag           AllowLatch;

    TrndTimeBase() { EndTime=CTimeValue(3600.0); StartTime=CTimeValue(0.0); TrackingTime=1; AllowLatch=0; };

    inline CTimeValue Duration() { return EndTime-StartTime; };
    flag operator ==(TrndTimeBase & TB) { return EndTime==TB.EndTime && StartTime==TB.StartTime && TrackingTime==TB.TrackingTime; };
    flag operator !=(TrndTimeBase & TB) { return EndTime!=TB.EndTime || StartTime!=TB.StartTime || TrackingTime!=TB.TrackingTime; };
  };

//=========================================================================

const int MaxTrendPtBlk = 256;

class CTrendSclBlk
  {
  public:
    double         YMx, YMn;
    
    CTrendSclBlk() {YMn=YMx=1.0;};
  };

//=========================================================================

const int  TrndIntInvalid=-32768;
inline int IntRound(long Lo, double X, long Hi) 
  { return (X > Hi ? Hi : (X < Lo ? Lo : (int)X));};
inline int TimePixel(CTimeValue Time, TrndTimeBase& TB) 
  { return IntRound(LONG_MIN, ((TrendXMax/NZ(TB.Duration().Seconds)*(Time-TB.StartTime).Seconds)/TrendLUperPT)*TrendLUperPT, LONG_MAX/*TrendXMax-1*/);};
inline int YPixel(double Val, CTrendSclBlk & Scl) 
  { return Valid(Val) ? IntRound(0, TrendYMax/NZ((double)(Scl.YMx-Scl.YMn))*(Val-Scl.YMn), TrendYMax-1) : TrndIntInvalid; };

//=========================================================================

_FWDDEF(CTrendPtBlk)
class CTrendPtBlk
  {
  DEFINE_SPARES(CTrendPtBlk)
  public:         
    CPoint             Pt[MaxTrendPtBlk];
    int                iNPts;
    int                iNewPts;

    CTrendPtBlk();
    ~CTrendPtBlk();
    flag AddPt(CTimeValue Time, double Val, byte Typ, int SltNo, int Fwd, CTrendSclBlk & Scl, CTagVwDoc *Doc);
    void ShiftPts(CTimeValue dTIme, CTrendSclBlk & Scl, CTagVwDoc *Doc);
    void UpdateYMnMx(CTrendSclBlk & OldScl, CTrendSclBlk & NewScl, CTagVwDoc *Doc);
    
    CPoint & operator[](int i) { ASSERT(i>=0 && i < MaxTrendPtBlk); return Pt[i];};
    int      NPts() { return iNPts;};
    void     Clear() {iNPts=0;};
    flag     AnyInRange();
    int      NewPoints() { return iNewPts; };
    void     ClearNewPoints() { iNewPts=0; };
    
  };

//-------------------------------------------------------------------------

class CTrendPtBlkList : public CList<pCTrendPtBlk, pCTrendPtBlk>
  {
  DEFINE_MEMSTATS(CTrendPtBlkList)
  };

//-------------------------------------------------------------------------

class CTrendSlot
  {
  DEFINE_MEMSTATS(CTrendSlot)

  friend class CTagVwDoc;
  friend class CTagVwTrend;
  friend class CTagVwSlot;
  public:
    pCTagVwSlot    pSlt;
    pCTagVwDoc     pDoc;
    int            iNo;
    CTrendSclBlk   Scl;
    CTrendPtBlkList PtBlks;//, FreeBlks;          

    CTrendPtBlk*   pStartOfTail;
    CTimeValue     dBuildStartTime;
    CTimeValue     LastMeasTime;
    double         LastMeasValue;

    CTrendSlot();
    ~CTrendSlot();
   
  protected:
    CTrendPtBlk*   GetPtBlk();
    void           Clear();
    void           ClearTail();
    void           AddStartPt(CTimeValue Tim, double Val);

    void           Connect(int No, /*pCTagVwTrend Vw_, */pCTagVwSlot Slt_, pCTagVwDoc Doc_); 
    flag           Connected() { return (pSlt!=NULL);};
    void           Draw(CDC* pDC, flag DoingAll, flag OnlyNew, flag fFromHeadOnly);
    void           AppendMeasured(CTimeValue TheTime);
    void           BuildFromEvent(CTimeValue Time, double Val);
    void           ShiftDrwPts(CTimeValue dTime);
    void           UpdateYMnMx(double NewMin, double NewMax);

    void           Start();
    void           Execute();
    void           Stop();

    CTagVwSlot&    DS(int i);
    CTrendSlot&    Trend(int i);

  public:
  
  };

//-------------------------------------------------------------------------

enum  FCT_How {FCT_Radial, FCT_Vertical};

//-------------------------------------------------------------------------

class TrackTrend
  {
  DEFINE_MEMSTATS(TrackTrend)

  public: 
    CTrendPtBlk*     pBlk;
    int              No;
    int              Pt;
    long             Distance;
    POINT            XY;

    TrackTrend() {Clear();};
    void Clear() {pBlk=NULL;No=Pt=Distance=-1;XY.x=-1;};
  };

//=========================================================================

class CSlotArray : public CArray<CTagVwSlot, CTagVwSlot&>
  {
  public:
    void CheckSize(int n)
      {
      int i=GetSize();
      if (n>i) SetSize(n);
      for (int j=i;j<n;j++) ElementAt(j).Clear(j);
      };
    void ChangeSize(int n) 
      {
      int i=GetSize();
      SetSize(Max(0,GetSize()+n));
      int k=GetSize();
      for (int j=i;j<k;j++) ElementAt(j).Clear(j);
      };
  };
class CTrendArray : public CArray<CTrendSlot, CTrendSlot&> 
  {
  public:
    void CheckSize(int n) { if (n>GetSize()) SetSize(n); };
    void ChangeSize(int n) { SetSize(Max(0,GetSize()+n)); };
  };

class CTagVwDoc : public DocRoot, public CExecObj, CNodeXRefMngr
  {
  friend class CLabelWnd;
  protected: // create from serialization only
    CTagVwDoc();//int NSlots_=0);//MaxTagVwSlots);
    DECLARE_DYNCREATE(CTagVwDoc)
  public:
    static ColourArray PenColours;

    CSlotArray       DS;
    int              NoSlots() { return DS.GetSize(); };
    int              iFirstChgdFld;
    int              iExecCnt;
                    
    CTrendArray      Trend;
    int              NoTrends() { return Trend.GetSize(); };

    CTagVwCmdBlk     TCB;
    CView*           pTrnd;
    CView*           pText;

    int              ColumnWidths[4];

    COLORREF         BackGroundColour;
    COLORREF         BorderColour;
    COLORREF         GridColour;
    COLORREF         CTimeColour;
    COLORREF         DigColour;
    COLORREF         TextColourBusy;
    COLORREF         TextColourNotBusy;
    int              GridStyle;
    int              CTimeStyle;
    int              DigStyle;

    CRITICAL_SECTION LockSection;
    TrndTimeBase     TB;
    static TrndTimeBase GTB;
    static BOOL      fTimeBaseGlobal;
  
    double           ShiftRatio;

    long             iRqstNo;
    long             iLastRqstNo;
    int              iNXGridDivs;
    int              iNYGridDivs;
    flag             fFixTimeBaseReqd:1,
                     fFirstActivate:1,
                     bJustLoaded:1,
                     bFunctBuildRqd:1,
                     bBusyBuildingPGM:1;
    UINT             ST:1,SD:1,ET:1,ED:1,Dur:1,HS:1,Scroll:1; //flags for label format

    int              iSltHiLite;
    flag             fSltHiLiteOn;

    int               m_iFnctSubsStartIndex; 

  public:
    virtual ~CTagVwDoc();
    virtual void     ConnectAllTags();
    virtual void     GetAllRecordedTags();
    virtual void     DeleteContents();
    int              FreeTrendNo();
    flag             ToggleTrending(int iSltNo);
    flag             ToggleTrending(CIArray & iSltNo);
    flag             SetTrending(CIArray & iSltNo, flag SetOn);
    pCTagVwSlot      Slot(int i) { return &DS[i]; };
    virtual void     OnAttachDoc();
    virtual void     OnActivate(flag bActivate);
    virtual void     OpenVNT();
    virtual BOOL     ReadDocument(const char* pszPathName, FILE* pFile);
    virtual BOOL     WriteDocument(const char* pszPathName, FILE* pFile);

    void             ReConnectTrends();
    void             ClearAllTrends();
    void             LoadData(CTagVwSlot * pSlot, int UpdateOpts);
    void             InsertLine(int iLine, int N);
    void             DeleteLine(int iLine, int N);
    void             ClearLine(int iLine, int N);

    void             FixTimeBase();
    void             SetTimeGlobal(flag On);
    void             SetTimebase(CTimeValue Start, CTimeValue Duration, flag bGlobal);
    void             AdjustTimebase(double ShiftFrac, double Scale, flag TrackingOn, flag AllowLatch);
    void             UpdateTimebase(TrndTimeBase & TTB);
    static void      DoSetTimeGlobal(flag On);
    static flag      TimeBaseGlobal() { return fTimeBaseGlobal; };
    CTimeValue       TimeBaseStart(flag bGlobal=False) { return bGlobal ? GTB.StartTime : TB.StartTime; };
    CTimeValue       TimeBaseEnd(flag bGlobal=False) { return bGlobal ? GTB.EndTime : TB.EndTime; };
    CTimeValue       TimeBaseDuration(flag bGlobal=False) { return bGlobal ? GTB.Duration() : TB.Duration(); };
    flag             TimeBaseTracking(flag bGlobal=False) { return bGlobal ? GTB.TrackingTime : TB.TrackingTime; };

    TrndTimeBase &   GetCurrentTimeBase() { return TB; };

    void             TryScrollWindow();
    int              FindClosestTrendPt(POINT &Pt, int TrndNo, TrackTrend &Trk);//, int &TrndPt, long &Distance, POINT &XY);
    int              FindClosestTrendPtVert(POINT &Pt, int TrndNo, TrackTrend &Trk);//, int &TrndPt, long &Distance, POINT &XY);
    int              FindClosestTrend(FCT_How How, POINT &Pt, TrackTrend &Trk, int& LastTrndNo, int RqdTrendNo=-1);

    static void      AdjustTimebaseToEndAll();
    static void      RebuildAll();
    void             Rebuild();
    virtual void     PreloadSlots() {}; 

    flag             FindTrendTag(LPCSTR Tag, int &iLine);
    flag             FindTag(LPCSTR Tag, int &iLine, bool Activate);
    void             TrendSlotsDlg(char* pTag);
    void             TrendQueryDlg();
    void             TrendOptionsDlg();
    void             Lock() { EnterCriticalSection(&LockSection); };
    void             Free() { LeaveCriticalSection(&LockSection); };

    static void      CreateScaleMenu(CMenu &Menu, pCTagVwSlot pSlot);
    static void      CreateFitMenu(CMenu &Menu, flag ForAll, flag WithAutoOnOff);
    flag             ProcessScaleMenu(int RetCd, pCTagVwSlot pSlot, double Val);
    
    void             HiLiteTrend(int Slt2HiLite);
    void             BlinkTrend();

    #ifdef _DEBUG
    virtual void     AssertValid() const;
    virtual void     Dump(CDumpContext& dc) const;
    #endif
  
  public:
    CStdioFile*      pFile;          //file for query results, NULL if none
    flag             StartFile(pchar Filename);
  
  protected:
    virtual BOOL OnNewDocument();
    virtual BOOL OnOpenDocument(const char* pszPathName);
  public:
    //{{AFX_MSG(CTagVwDoc)
    afx_msg void OnTrendOptions();
    afx_msg void OnTrendGetquery();
    afx_msg void OnTrendSlots();
    afx_msg void OnTrendRefresh();
    afx_msg void OnTrendRefreshall();
    afx_msg void OnTrendRecord();
    afx_msg void OnTrendRecordall();
    afx_msg void OnTrendFitAll(UINT Id);
    afx_msg void OnUpdateTrendRecord(CCmdUI* pCmdUI);
    afx_msg void OnUpdateTrendRecordall(CCmdUI* pCmdUI);
    //}}AFX_MSG
  protected:
    DECLARE_MESSAGE_MAP()

    BOOL             NewReadDocument(FILE* pFile);

    flag             AddFunct(char* pName, CTagVwSlot* pSlt);
    void             BuildFunctions();
    void             EvaluateFunctions();
    
    // CNodeXRefMngr Overrides
    virtual int    ResolveNearXRef(CXRefItem * XRef, flag MustBeParm);
    virtual int    ResolveFarXRef(CXRefItem * XRef);
    virtual int    ResolveIOXRef(CXRefItem * XRef);
    virtual int    UpdateXRefLists(CXRefBuildResults & Results);
    virtual void   UnlinkAllXRefs();
    virtual char*  GetOwnerTag();
    virtual bool   GetOwnerDbgBrk() { return false; };

    // ExecObj Overrides 
    virtual flag   EO_RequestTagInfo(RequestTagInfoRec& Rqst, ReplyTagInfoRec& Info);
    virtual flag   EO_LoadComplete(FilingControlBlock &FCB);
    virtual DWORD  EO_Message(CXMsgLst &XM, CXM_Route &Route);
    virtual int    EO_QueryChangeTag(pchar pOldTag, pchar pNewTag);
    virtual int    EO_ChangeTag(pchar pOldTag, pchar pNewTag);
    virtual int    EO_QueryDeleteTag(pchar pDelTag);
    virtual int    EO_DeleteTag(pchar pDelTag);
    virtual flag   EO_QueryTime(CXM_TimeControl &CB, CTimeValue &TimeRqd, CTimeValue &dTimeRqd);
    virtual flag   EO_Start(CXM_TimeControl &CB);
    virtual void   EO_QuerySubsReqd(CXMsgLst &XM);
    virtual void   EO_QuerySubsAvail(CXMsgLst &XM, CXMsgLst &XMRet);
    virtual flag   EO_TagsNotAvail(CXMsgLst &XM);
    virtual flag   EO_ReadSubsData(CXMsgLst &XM);  
    virtual flag   EO_WriteSubsData(CXMsgLst &XM, flag FirstBlock, flag LastBlock);  
    virtual flag   EO_Execute(CXM_TimeControl &CB, CEOExecReturn &EORet);
    virtual flag   EO_Stop(CXM_TimeControl &CB);
    virtual flag   EO_Starting(flag fBeginStarting);
    virtual flag   EO_Stopping(flag fBeginStopping);
    virtual void   EO_GlblResultValidity(flag IsValid, flag IsAlwaysValid);
  };

//-------------------------------------------------------------------------

class CTagVwCtrlInfo
  {
  public:
    Strng m_PIDTag;
    Strng m_SptTag;
    Strng m_MeasTag;
    Strng m_OutTag;

    flag  m_InUse, m_InLst;
  };

class CTagVwDocC : public CTagVwDoc
  {
  protected: // create from serialization only
    CTagVwDocC();//int NSlots_=0);//MaxTagVwSlots);
    DECLARE_DYNCREATE(CTagVwDocC)
  public:

    virtual void     ConnectAllTags();

    CArray <CTagVwCtrlInfo, CTagVwCtrlInfo&> m_Loops;
    int NoLoops() { return m_Loops.GetSize(); };

    int     m_iLoopSlotOne;
    int     m_nLoopSlotCount;
    int     m_iViewControl;
    int     LoopSlotOne() { return m_iLoopSlotOne; };
    int     LoopSlotCount() { return m_nLoopSlotCount; };
    int     ViewControl() { return m_iViewControl; };
    void    SetViewControl(int i) { m_iViewControl=i; };

    virtual void    PreloadSlots(); 

    Strng   m_RootTag;

    //{{AFX_MSG(CTagVwDocC)
    //}}AFX_MSG

  protected:
    DECLARE_MESSAGE_MAP()
  };

//-------------------------------------------------------------------------

union CFloatLParamUnion
  {
  float   f;
  long    L;
  };

//-------------------------------------------------------------------------

inline CTagVwSlot& CTrendSlot::DS(int i) { return pDoc->DS[i]; };
inline CTrendSlot& CTrendSlot::Trend(int i) { return pDoc->Trend[i]; };

inline int CTagVwSlot::NoSlots(pCTagVwDoc pDoc) { return pDoc->NoSlots(); };
inline int CTagVwSlot::NoTrends(pCTagVwDoc pDoc) { return pDoc->NoTrends(); };
inline CTagVwSlot& CTagVwSlot::DS(pCTagVwDoc pDoc, int i) { return pDoc->DS[i]; };
inline CTrendSlot& CTagVwSlot::Trend(pCTagVwDoc pDoc, int i) { return pDoc->Trend[i]; };

#endif
