//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __SQSZDST1_H
#define  __SQSZDST1_H

#ifndef __SQSZBASE_H
  #include "sqszbase.h"
#endif
#ifndef __SP_CONT_H
  #include "sp_cont.h"
#endif
#ifndef __FIXEDIT_H
  #include "fixedit.h"
#endif
#ifndef __GPWNDS_H
  #include "gpwnds.h"
#endif
#ifndef __SFE_INIT_H
  #include "sfe_init.h"
#endif
#ifndef __SCD_SW_H
  #include "scd_wm.h"
#endif

#define WithIndividPartCrv 1
#define USE_PSD_TB 1
#ifdef USE_PSD_TB

// PSD TB TO DO
// Make page visible only when required
// Include other functions in PSD class to generate PSD data

#ifndef __PSD_TB_H
#include "PSD_TB.h"
#endif
#endif

#ifdef __SQSZDST1_CPP
  #define DllImportExport DllExport
#elif !defined(SizeDst1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

#define WITHSELBRK    01

const int MaxDistributions  = 4;
//const int MaxIntervals      = 120;//64; // Increased for QAL PSD
//const int MaxColumns        = 120;//100; // Increased for RTTS
//const int MaxPartCurves     = 12;
const int MaxIntervals      = 128;//64; // Increased for QAL PSD
const int MaxColumns        = 128;//100; // Increased for RTTS
const int MaxPartCurves     = 15;

XID xidSzCol                = Qual0XID(MaxColumns*20);

// ==========================================================================
//
//
//
// ==========================================================================

class SQSzDist1;
class SzPartCrv1;
class SzSelBrk1;

// ===========================================================================
//
//
//
// ===========================================================================

class DllImportExport SQSzDistFractionInfo
  {
  public:
    // Configuration
    SzPartCrv1 & m_PartFn;
    int          m_CurveNo;
    double       m_ByePass2Fines;
    double       m_ByePass2Coarse;

    // Optional Configuration
    SQSzDist1 *  m_pSQFines;
    SQSzDist1 *  m_pSQCoarse;

    // Calculated
    double       m_FineMass;
    double       m_CoarseMass;
    SpMArray     m_SpSplitToFines;

    SQSzDistFractionInfo(SzPartCrv1 & PartFn, int CurveNo);
  };

// ===========================================================================
//
//
//
// ===========================================================================

DEFINE_TAGOBJEDTDERIVED(SQSzDist1, SQSzDist1TagObjClass);

class DllImportExport SQSzDist1 : public SpQuality
  {
  friend class SQSzDist1Edt;
  friend class CSD_Distribution;
  friend class CSzSSA;

  DEFINE_SPARES(SQSzDist1)

  public:
    CArray <CSD_Distribution*, CSD_Distribution* > Distributions;
    flag           fAllowSizeEdits;
    int            iRqdDistOff,iRqdDistOn;
    int            iDistUsed; //index of distribution used

#ifdef USE_PSD_TB

    bool           bAllowTestData; // Whether to use test data page or not
    CPSDTB*        m_PSDTB;        // PSD Test Data Entry
#endif

    SQSzDist1(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~SQSzDist1();

    DEFINE_QUALITYPTR(SQSzDist1, "SzDist");

    void           AllocDist(int d);
    void           FreeDist(int d);
    void           ChangeToDist(int d);

    static flag    BuildDataDefnGlobal(TagObjClass *pClass, TaggedObject * pOwner, DataDefnBlk & DDB);
    static flag    DataXchgGlobal(TagObjClass *pClass, TaggedObject * pOwner, DataChangeBlk & DCB);
    //Does Nothing: virtual void   BuildDataDefn_Lcl(DataDefnBlk & DDB);
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    //flag           CalculateResults(int d, int DataId, int SpId);

    void           OnDataChange()
      {
      for (int d=0; d<NDistributions(); d++)
        if (Distributions[d])
          {
          Distributions[d]->iCurrentDataId=-1;
          Distributions[d]->iCurrentSpId=-1;
          }
      };

    virtual void   ExchangeSpecies(SpQuality * Other);
    virtual void   Equilibrate();
    virtual void   ZeroMass();
    virtual void   ZeroDeriv();
    virtual void   ScaleMass(PhMask PhaseM, double Mult);
    virtual void   ScaleMass(CIArray & SpIds, double Mult);
    virtual void   ScaleMass(CSysVector  &Scl, double Mult);
    virtual void   SetMassF(CSysVector &M1, SpQuality * pQual2);
    virtual void   AddMassF(CSysVector &M1, SpQuality * pQual2, CSysVector &M2);
    virtual void   SubMassF(CSysVector &M1, SpQuality * pQual2, CSysVector &M2);
    virtual void   Copy(pSpModel pMdl2, SpQuality * pQual2);
    virtual void   AddDeriv(pSpModel pMdl2, SpQuality * pQual2, double Sgn_);
    virtual void   AddDiscrete(pSpModel pMdl2, SpQuality * pQual2, double Sgn_);
    virtual void   ODEOperate(CODEDataBlock & ODB);


    double         SizePassingFraction(int iDist, int iPriId, double dFrac);
    double         FractionPassingSize(int iDist, int iPriId, double dSize);
    double         SurfaceAreaPerGram(int DistNo, int PriId);
    double         SurfaceAreaPerLitre(int DistNo, int PriId);
    double         ParticleCountPerMass(int DistNo, int PriId);
    double         GeometricMean(int iDist, int iPriId);
    double         TotalMass(int iDist, int iPriId);

    double         FinesFraction(C2DFn & PartFn, double ByePass2Fines, double ByePass2Grits);
    double         SetFinesFraction(C2DFn & PartFn, double ByePass2Fines, double ByePass2Grits);
    double         GritsFraction(C2DFn & PartFn, double ByePass2Fines, double ByePass2Grits);
    double         SetGritsFraction(C2DFn & PartFn, double ByePass2Fines, double ByePass2Grits);
    double         Crush0(C2DFn & PartFn);
    double         Crush1(int s, C2DFn & PartFn);

    double         FinesFraction(SQSzDistFractionInfo &Info);
    double         SetFinesFraction(SQSzDistFractionInfo &Info);
    double         GritsFraction(SQSzDistFractionInfo &Info);
    double         SetGritsFraction(SQSzDistFractionInfo &Info);
    double         Crush0(SzPartCrv1 & PartFn, int CurveNo);
    double         Crush1(int s, SzPartCrv1 & PartFn, int CurveNo);
    double         Break0(SzSelBrk1 & SB);
    double         CrushExt0(SzPartCrv1 &Extents, SpConduit &QFeed, int CurveNo);
    double         CrushExt1(int s, SzPartCrv1 &Extents, SpConduit &QFeed, int CurveNo);

    flag           CheckDischIsValid1(int s, SpConduit &QFeed);
    double         CorrectDisch1(int s, SpConduit &QFeed);

    static double  SplitSolids(SQSzDistFractionInfo &Info, SpConduit &QFeed, SpConduit &QFine, SpConduit &QCoarse);
    static flag    ChangeDistribution(SpConduit &QFeed, SpConduit &QProd, int iDistRqd);

    flag           AllowSizeEdits();
    char*          UsedByTag();
    char*          UsedByFullTag(Strng& Str);

    int            NDistributions() { return Distributions.GetSize();};
    flag           DistExists(int i) { return (i<Distributions.GetSize() && Distributions[i]!=NULL); };
    CSD_Distribution& Dist(int i) { return *(Distributions[i]); };
    flag           DistributionsExist()
                     {
                     for (int d=0; d<Distributions.GetSize(); d++)
                       if (Distributions[d])
                         return true;
                     return false;
                     };
    int            DistIndex()
                     {
                     for (int d=0; d<Distributions.GetSize(); d++)
                       if (Distributions[d])
                         return d;
                     return -1;
                     };

    int            GetApplyToDists(int DistNo, int &d0, int &dN)
                     {
                     d0=(DistNo>=0) ? DistNo : 0;
                     dN=(DistNo>=0) ? Min(DistNo+1, NDistributions()) : NDistributions();
                     return dN-d0;
                     };

    CSD_Distribution *FindDistributionFor(int SpecieId);
    CSD_SpDist     *FindSpDistributionFor(int SpecieId);

  protected:
    double         DoFinesFraction(C2DFn & PartFn, double ByePass2Fines, double ByePass2Grits, flag bSetIt);
    double         DoGritsFraction(C2DFn & PartFn, double ByePass2Fines, double ByePass2Grits, flag bSetIt);

    double         DoFinesFraction(SQSzDistFractionInfo &Info, flag bSetIt);
    double         DoGritsFraction(SQSzDistFractionInfo &Info, flag bSetIt);

    double         DoSplitSolids(SQSzDistFractionInfo &Info, SpModel &QFine, SpModel &QCoarse);

  public:
    // Extras for Tear Management;
    virtual void   TearGetInputs(TearVarArray & TV, int n);
    virtual void   TearGetOutputs(TearVarArray & TV, int n);
    virtual void   TearSetOutputs(TearVarArray & TV, int n);

    // Debug Stuff
    virtual void   Dump();
/*#ifndef _RELEASE
  private:
    SQSzDist1 &     operator=(const SQSzDist1 & S);
#endif*/
  };

// ===========================================================================

inline double SQSzDist1::FinesFraction(C2DFn & PartFn, double ByePass2Fines, double ByePass2Grits)
  { return DoFinesFraction(PartFn, ByePass2Fines, ByePass2Grits, False); };
inline double SQSzDist1::SetFinesFraction(C2DFn & PartFn, double ByePass2Fines, double ByePass2Grits)
  { return DoFinesFraction(PartFn, ByePass2Fines, ByePass2Grits, True); };
inline double SQSzDist1::GritsFraction(C2DFn & PartFn, double ByePass2Fines, double ByePass2Grits)
  { return DoGritsFraction(PartFn, ByePass2Fines, ByePass2Grits, False); };
inline double SQSzDist1::SetGritsFraction(C2DFn & PartFn, double ByePass2Fines, double ByePass2Grits)
  { return DoGritsFraction(PartFn, ByePass2Fines, ByePass2Grits, True); };

inline double SQSzDist1::FinesFraction(SQSzDistFractionInfo &Info)
  { return DoFinesFraction(Info, False); };
inline double SQSzDist1::SetFinesFraction(SQSzDistFractionInfo &Info)
  { return DoFinesFraction(Info, True); };
inline double SQSzDist1::GritsFraction(SQSzDistFractionInfo &Info)
  { return DoGritsFraction(Info, False); };
inline double SQSzDist1::SetGritsFraction(SQSzDistFractionInfo &Info)
  { return DoGritsFraction(Info, True); };


inline double SQSzDist1::SplitSolids(SQSzDistFractionInfo &Info, SpConduit &QFeed, SpConduit &QFine, SpConduit &QCoarse)
  {
  SQSzDist1 &QFeedSz=*SQSzDist1::Ptr(QFeed.Model());
  return QFeedSz.DoSplitSolids(Info, *QFine.Model(), *QCoarse.Model());
  };

// ===========================================================================
//
//
//
// ===========================================================================

_FWDDEF(SQSzDist1Edt);
class DllImportExport SQSzDist1Edt : public FxdEdtBookRef
  {
  public :
    SQSzDist1 &    rSD;
    CRect          GraphR;
    double         XMin;       //normalised x min (lhs of graph)
    double         XMax;       //normalised x max (rhs of graph)
    double         XFactor;    // = XPix/(XMax-XMin)
    CDArray        YMin;       //normalised y min (bottom of graph)
//    CDArray        YCMax;      //normalised y max (top of graph)
    CDArray        YFMax;      //normalised y max (top of graph)
//    CDArray        YCFactor;   // = YPix/(YMax-YMin)
    CDArray        YFFactor;   // = YPix/(YMax-YMin)
    int            iDragPtNo;  //the index of the point being moved (-1 if none)
    int            iNameWidth; //
    int            iWorkDist;
    int            iPg1;

    //TagInfoBlk     WrkIB;
    //CnvAttribute   *pWrkCnv;
    //FmtAttribute   *pWrkFmt;

    CLabelWnd*     pLabels;    //
    static const int XPix;     //number of x pixels
    static const int YPix;     //number of x pixels
    static const int CrvPts;
    static int     iGraphWidth;
    static int     iGraphHeight;
    static int     iGraphOn;
    static flag    fCrvStepped;

    flag           fXtraPg;

    //CDArray        WorkSpace;
    //CDArray        WorkSpace1;

    void SetWorkDist(int i) { iWorkDist=i; };
    int GraphWidth()  { return iGraphWidth; };
    int GraphHeight() { return iGraphHeight; };
    int GraphOn() { return iGraphOn; };

    flag ValidDistribution() { return (iWorkDist<rSD.NDistributions()) && (rSD.Distributions[iWorkDist]!=NULL); };
    CSD_Distribution & D() { return *rSD.Distributions[iWorkDist]; };
    CSD_DistDefn & DD() { return D().rDefn; };
    SpModel & Model() { return *rSD.pModel; };
    int NColumns() { return D().rDefn.Columns.GetSize(); };
    CSD_Column &Columns(int i) { return D().rDefn.Columns[i]; };

    int Length() { return D().NIntervals(); };
    int DistFromPgNo(int Pg);

    flag   AutoScale() { return DD().fAutoScale; };
    void   SetAutoScale(flag On) { DD().fAutoScale=On; };

    //flag   CumulativeOn() { return DD().fCumulativeOn; };
    //void   SetCumulativeOn(flag On) { DD().fCumulativeOn=On; };
    //
    //flag   FractionalOn() { return DD().fFractionalOn; };
    //void   SetFractionalOn(flag On) { DD().fFractionalOn=On; };

    flag   DispRetained() { return DD().fDispRetained; };
    void   SetDispRetained(flag On) { DD().fDispRetained=On; };

    flag   XLog() { return DD().fXLog; };
    flag   YLog() { return DD().fYLog; };
    void   SetXLog(flag On) { DD().fXLog=On; };
    void   SetYLog(flag On) { DD().fYLog=On; };

    void   AllocDist(int d) { rSD.AllocDist(d); };
    void   FreeDist(int d) { rSD.FreeDist(d); };
    void   ChangeToDist(int d) { rSD.ChangeToDist(d); };
    //int    CurView() { return rSD.iCurView; };
    //void   SetCurView(int iV) { rSD.iCurView=iV; };

    double XDispMin() { return DD().dSzDispMin; };
    double XDispMax() { return DD().dSzDispMax; };
    double YDispMin(int c) { return DD().Columns[c].dDispMin; };
    //double YCDispMax(int c) { return DD().Columns[c].dCDispMax; };
    double YFDispMax(int c) { return DD().Columns[c].dFDispMax; };
    void SetXDispMin(double m) { DD().dSzDispMin=m; };
    void SetXDispMax(double m) { DD().dSzDispMax=m; };
    void SetYDispMin(int c, double m) { DD().Columns[c].dDispMin=m; };
    //void SetYCDispMax(int c, double m) { DD().Columns[c].dCDispMax=m; };
    void SetYFDispMax(int c, double m) { DD().Columns[c].dFDispMax=m; };

    double GetXData(int iInt) { return D().rIntervals[iInt]; };
    CDArray & GetXData() { return D().rIntervals; };
    double GetYData(CSD_Column &C, int iInt);
    CDArray & GetYData(CSD_Column &C);
    double GetYData(int c, int iInt) { return GetYData(Columns(c), iInt); };
    CDArray & GetYData(int c) { return GetYData(Columns(c)); };
    void SetYData(CSD_Column &C, int iInt, double Val);
    //void SetYData(CSD_Column &C);
    void SetYData(int c, int iInt, double Val) { SetYData(Columns(c), iInt, Val); };
    //void SetYData(int c) { SetYData(Columns(c)); };

    int CalcXPix(double X) { return (int)Range((double)-XPix, ((X)-XMin)*XFactor, (double)XPix*2);};
//    int CalcYCPix(int c, double Y) { return (int)Range((double)-YPix, ((Y)-YMin[c])*(YCFactor[c]*950)/1000+(YPix*25)/1000, (double)YPix*2);};
//    int CalcYFPix(int c, double Y) { return (int)Range((double)-YPix, ((Y)-YMin[c])*(YFFactor[c]*950)/1000+(YPix*25)/1000, (double)YPix*2);};
    int CalcYFPix(int c, double Y) { return (int)Range((double)-YPix, ((Y)-YMin[c])*YFFactor[c], (double)YPix*2);};

    int CalcXLogPix(double X) { return (int)Range((double)-XPix, (Log10(GTZ(X))-XMin)*XFactor, (double)XPix*2); };
//    int CalcYCLogPix(int c, double Y) { return (int)Range((double)-YPix, (Log10(GTZ(Y))-YMin[c])*(YCFactor[c]*950)/1000+(YPix*25)/1000, (double)YPix*2); };
//    int CalcYFLogPix(int c, double Y) { return (int)Range((double)-YPix, (Log10(GTZ(Y))-YMin[c])*(YFFactor[c]*950)/1000+(YPix*25)/1000, (double)YPix*2); };
    int CalcYFLogPix(int c, double Y) { return (int)Range((double)-YPix, (Log10(GTZ(Y))-YMin[c])*YFFactor[c], (double)YPix*2); };

    double CalcXVal(int X) { return (float)X/XFactor+XMin;};

    double CalcXLogVal(int X) { return pow(10.0, (float)X/XFactor+XMin); };

    SQSzDist1Edt(FxdEdtView * pView_, SQSzDist1 * pSD_);//, rStrng Tag, rStrng Name);
    virtual ~SQSzDist1Edt();

    virtual void PutDataStart();
    virtual void PutDataDone();
    virtual void GetDataStart();
    virtual void GetDataDone();

    virtual void StartBuild();
    virtual void Build();
    virtual void ChangeLayout(rFxdEdtPage Pg, int TotalRows, int TotalCols);
    virtual void Load(FxdEdtInfo &EI, Strng & Str);
    virtual long ButtonPushed(FxdEdtInfo &EI, Strng & Str);
    virtual long Parse(FxdEdtInfo &EI, Strng & Str);

    virtual void OnDrawBack(rGDIBlk GB,int PgNo, CRgn &ClipRgn);
    virtual flag DoLButtonDown(UINT nFlags, CPoint point);
    virtual flag DoLButtonUp(UINT nFlags, CPoint point);
    virtual flag DoLButtonDblClk(UINT nFlags, CPoint point);
    virtual flag DoRButtonDown(UINT nFlags, CPoint point);
    virtual flag DoRButtonUp(UINT nFlags, CPoint point);
    virtual flag DoRButtonDblClk(UINT nFlags, CPoint point);
    virtual flag DoMouseMove(UINT nFlags, CPoint point);

    virtual flag DoAccCnv(UINT Id);
    virtual flag DoAccFmt(UINT Id);
    virtual flag DoAccRptTagLists();

    virtual void SetPosition(rGDIBlk GB);//, CRect *CRReqd);
  private:
    char*        GetRangeLbl(int i, Strng& Str, flag Range);
    void         SetRanges();//CDArray & YPts, CDArray & XPts);
    void         PointtoLP(POINT &Pt);
    void         PointtoWP(POINT Pt, double& x, double& y);
    int          ClosestPt(CPoint point);
    void         ToggleDigCursor(POINT &Pt);

  };

// ===========================================================================
//
//
//
// ===========================================================================

class DllImportExport SzPartCrvGrf1
  {
  friend class SzPartCrv1;
  friend class SzPartCrv1Edt;

  protected:
    flag   fAutoScale;
    flag   fXLog, fYLog;

    double dSzDispMin;
    double dSzDispMax;

    double dDispMin, dFDispMax;

    CnvAttribute   XCnv, YCnv;
    FmtAttribute   XFmt, YFmt;

  public:
    SzPartCrvGrf1() :
      XCnv(DC_L, "um"),
      YCnv(DC_Frac, "%"),
      XFmt("", 0, 1, 'f'),
      YFmt("", 0, 2, 'f')
      {
      fAutoScale=True;

      fXLog=True;
      fYLog=False;

      dSzDispMin=0.0001;
      dSzDispMax=0.01;
      }
  };

// ==========================================================================

const byte MaxPCModes     = 2;
const byte PC_Frac2Fine   = 0;
const byte PC_Frac2Coarse = 1;

const byte MaxPCStates    = 2;
const byte PC_On          = 0;
const byte PC_Off         = 1;

class PartCrvInfo
  {
  public:
    byte Mode;
    byte State;
    byte Edit;
    byte AutoCorrect;
    Strng Title;
    CnvAttribute DataCnv;
    double dLoRange, dHiRange;

  public:
    PartCrvInfo() : DataCnv(DC_Frac, "%")
      {
      SetDefaultValues();
      };
    void SetDefaultValues()
      {
      Mode=PC_Frac2Fine;
      State=PC_On;
      Edit=1;
      AutoCorrect=0;
      dLoRange=0.0;
      dHiRange=1.0;
      };

  };

DEFINE_TAGOBJEDT(SzPartCrv1);
class DllImportExport SzPartCrv1 : public TaggedObject
  {
  friend class SzPartCrv1Edt;
  friend class SQSzDist1;

  DEFINE_SPARES(SzPartCrv1)
  protected:
    CArray <PartCrvInfo, PartCrvInfo&> CrvInfo;

    CDArray         LclSz; //size distribution used
    CDMatrx         Crvs;
    SzPartCrvGrf1   PCG;

    Strng           sPCModeStrings[MaxPCModes];
    Strng           sPCStateStrings[MaxPCStates];
    Strng           sUsedClassID;
    flag            fPCMode_Shown;
    flag            fPCState_Shown;
    flag            fPCGraph_Shown; //Graph functionality visible?
    //flag            fPCCumOrFrac_Shown;
    flag            fPCCumShown; //Show calculated cummulitave columns
    flag            fPCSizeRangeRows; //show row titles as size range (true) or size (false)
    flag            fFracViewActive;
    flag            fPartStepped;
    int             iColSortGroup; //0 for none
    int             iSizeDefn;
    int             iRngWidth;
    int             iApplyToDist;
    int             iCrvCount;
    double          dBottomSize, dTopSize;
    double          dBottomSizeDisp, dTopSizeDisp;

  public:
    SzPartCrv1(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach);
    SzPartCrv1(pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~SzPartCrv1();

    //general display options/properties...
    void           SetModeVisibility(flag Shown, char* FineTag, char* CoarseTag);
    void           SetStateVisibility(flag Shown, char* OnTag, char* OffTag);
    void           SetGraphVisibility(flag Shown) { fPCGraph_Shown=Shown; };
    //void           SetCumOrFracVisibility(flag Shown) { fPCCumOrFrac_Shown=Shown; };
    void           SetCumulativeVisibility(flag Shown);
    void           SetSizeRangeRows(flag SizeRangeRows) { fPCSizeRangeRows=SizeRangeRows; };
    void           SetFracViewActive(flag FracViewActive) { fFracViewActive=FracViewActive; };
    flag           FracViewActive() { return fFracViewActive; };

    //curve specific display options/properties...
    int            CurveMode(int CurveNo) { return CrvInfo[CurveNo].Mode; };
    void           SetCurveMode(int CurveNo, int Md) { CrvInfo[CurveNo].Mode=(byte)Md; };
    flag           IsCurveOn(int CurveNo) { return (CrvInfo[CurveNo].State!=PC_Off); };
    int            CurveState(int CurveNo) { return CrvInfo[CurveNo].State; };
    void           SetCurveState(int CurveNo, int St) { CrvInfo[CurveNo].State=(byte)St; };
    int            Editable(int CurveNo) { return CrvInfo[CurveNo].Edit; };
    void           SetEditable(int CurveNo, int Editable) { CrvInfo[CurveNo].Edit=(byte)Editable; };
    char*          Title(int CurveNo) { return CrvInfo[CurveNo].Title(); };
    void           SetTitle(int CurveNo, char* s) { CrvInfo[CurveNo].Title=s; };
    void           SetAutoCorrect(int CurveNo, flag On) { CrvInfo[CurveNo].AutoCorrect=On; };
    void           SetCnv(int CurveNo, CCnvIndex CnvIndex, char* CnvTxt) { CrvInfo[CurveNo].DataCnv.Set(CnvIndex, CnvTxt); };
    void           SetRange(int CurveNo, double Lo, double Hi) { CrvInfo[CurveNo].dLoRange=Lo; CrvInfo[CurveNo].dHiRange=Hi; };

    //Other...
    void           SetSizeDefn(int iDef);
    int            GetSizeDefn()           { return iSizeDefn; };

    int            Length()                { return SizePts().GetSize(); };
    void           SetLength(int N);

    int            NCurves()               { return iCrvCount; };//(fPCCumShown ? Crvs.GetSize()/2 : Crvs.GetSize()); };
    void           SetNCurves(int N);
    CDArray &      Curve(int i)            { return Crvs[i]; };

    double         BottomSize() { return dBottomSize; };
    double         TopSize() { return dTopSize; };
    double         BottomSizeDisp() { return dBottomSizeDisp; };
    double         TopSizeDisp() { return dTopSizeDisp; };

    double         SizePassingFraction(int CurveNo, double dFrac);
    double         FractionPassingSize(int CurveNo, double dSize);

  protected:
    void           CommonConstruct();
    char*          GetRangeLbl(int i, Strng& Str, flag Ascend, flag Range);
    void           CalcCumValues();

    //Does Nothing: virtual void   BuildDataDefn_Lcl(DataDefnBlk & DDB);
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    flag           UserSizePts()           { return iSizeDefn<0; };
    CDArray &      SizePts()               { return LclSz; };

    int            NDists()                { return SD_Defn.NDistributions(); };
    flag           DistExists(int i)       { return SD_Defn.DistExists(i); };

    int            ApplyToDist()           { return iApplyToDist; };
    void           SetApplyToDist(int i)   { iApplyToDist=i; SetSizeDefn(iSizeDefn); };

    flag           PartStepped()           { return fPartStepped; };
    void           SetPartStepped(flag On) { fPartStepped=On; };

    int            ColSortGroup()          { return iColSortGroup; };
    void           SetColSortGroup(int i)  { iColSortGroup = i; };

    flag           ApplyToAllDists()       { return iApplyToDist<0; };

    int            GetApplyToDists(int &d0, int &dN)
                     {
                     d0=(iApplyToDist>=0) ? iApplyToDist : 0;
                     dN=(iApplyToDist>=0) ? Min(iApplyToDist+1, NDists()) : NDists();
                     return dN-d0;
                     };


  };

// ===========================================================================
//
//
//
// ===========================================================================

_FWDDEF(SzPartCrv1Edt);
class DllImportExport SzPartCrv1Edt : public FxdEdtBookRef
  {
  public :
    SzPartCrvGrf1 &PCG;
    SzPartCrv1  &PC;
    CRect          GraphR;
    double         XMin;       //normalised x min (lhs of graph)
    double         XMax;       //normalised x max (rhs of graph)
    double         XFactor;    // = XPix/(XMax-XMin)
    CDArray        YMin;       //normalised y min (bottom of graph)
    CDArray        YFMax;      //normalised y max (top of graph)
    CDArray        YFFactor;   // = YPix/(YMax-YMin)
    int            iDragPtNo;  //the index of the point being moved (-1 if none)
    int            iNameWidth; //
    int            iWorkDist;
    int            iPg1;

    TagInfoBlk     WrkIB;
    //CnvAttribute   *pWrkCnv;
    //FmtAttribute   *pWrkFmt;

    CLabelWnd*     pLabels;    //
    static const int XPix;     //number of x pixels
    static const int YPix;     //number of x pixels
    static const int CrvPts;
    static int iGraphWidth;
    static int iGraphHeight;
    static int iGraphOn;

    int GraphWidth()  { return iGraphWidth; };
    int GraphHeight() { return iGraphHeight; };
    int GraphOn() { return (iGraphOn && PC.fPCGraph_Shown); };

    int NCurves() { return PC.NCurves(); };
    int Length() { return PC.Crvs.GetSize()>0 ? PC.Crvs[0].GetSize() : 0; };

    flag   AutoScale() { return PCG.fAutoScale; };
    void   SetAutoScale(flag On) { PCG.fAutoScale=On; };

    int    ApplyToDist() { return PC.iApplyToDist; };
    void   SetApplyToDist(int i) { PC.iApplyToDist=i; };

    flag   PartStepped() { return PC.fPartStepped; };
    void   SetPartStepped(flag On) { PC.fPartStepped=On; };

    flag   XLog() { return PCG.fXLog; };
    flag   YLog() { return PCG.fYLog; };
    void   SetXLog(flag On) { PCG.fXLog=On; };
    void   SetYLog(flag On) { PCG.fYLog=On; };

    double XDispMin() { return PCG.dSzDispMin; };
    double XDispMax() { return PCG.dSzDispMax; };
    double YDispMin(int c) { return PCG.dDispMin; };
    double YFDispMax(int c) { return PCG.dFDispMax; };
    void SetXDispMin(double m) { PCG.dSzDispMin=m; };
    void SetXDispMax(double m) { PCG.dSzDispMax=m; };
    void SetYDispMin(int c, double m) { PCG.dDispMin=m; };
    void SetYFDispMax(int c, double m) { PCG.dFDispMax=m; };

    int CalcXPix(double X) { return (int)Range((double)-XPix, ((X)-XMin)*XFactor, (double)XPix*2);};
    int CalcYFPix(int c, double Y) { return (int)Range((double)-YPix, ((Y)-YMin[c])*YFFactor[c], (double)YPix*2);};

    int CalcXLogPix(double X) { return (int)Range((double)-XPix, (Log10(GTZ(X))-XMin)*XFactor, (double)XPix*2); };
    int CalcYFLogPix(int c, double Y) { return (int)Range((double)-YPix, (Log10(GTZ(Y))-YMin[c])*YFFactor[c], (double)YPix*2); };

    double CalcXVal(int X) { return (float)X/XFactor+XMin;};

    double CalcXLogVal(int X) { return pow(10.0, (float)X/XFactor+XMin); };

    SzPartCrv1Edt(pFxdEdtView pView_, pSzPartCrv1 pSD_);
    virtual ~SzPartCrv1Edt();

    virtual void PutDataStart();
    virtual void PutDataDone();
    virtual void GetDataStart();
    virtual void GetDataDone();

    virtual void StartBuild();
    virtual void Build();
    virtual void ChangeLayout(rFxdEdtPage Pg, int TotalRows, int TotalCols);
    virtual void Load(FxdEdtInfo &EI, Strng & Str);
    virtual long ButtonPushed(FxdEdtInfo &EI, Strng & Str);
    virtual long Parse(FxdEdtInfo &EI, Strng & Str);

    virtual void OnDrawBack(rGDIBlk GB,int PgNo, CRgn &ClipRgn);
    virtual flag DoLButtonDown(UINT nFlags, CPoint point);
    virtual flag DoLButtonUp(UINT nFlags, CPoint point);
    virtual flag DoLButtonDblClk(UINT nFlags, CPoint point);
    virtual flag DoRButtonDown(UINT nFlags, CPoint point);
    virtual flag DoRButtonUp(UINT nFlags, CPoint point);
    virtual flag DoRButtonDblClk(UINT nFlags, CPoint point);
    virtual flag DoMouseMove(UINT nFlags, CPoint point);

    virtual flag DoAccCnv(UINT Id);
    virtual flag DoAccFmt(UINT Id);
    virtual flag DoAccRptTagLists();

    virtual void SetPosition(rGDIBlk GB);
  private:
    void SetRanges();
    void PointtoLP(POINT &Pt);
    void PointtoWP(POINT Pt, double& x, double& y);
    int ClosestPt(CPoint point);
    void ToggleDigCursor(POINT &Pt);

  };

// ===========================================================================
//
//
//
// ===========================================================================

#if WITHSELBRK

// ==========================================================================

class DllImportExport SzSelBrkGrf1
  {
  friend class SzSelBrk1;
  friend class SzSelBrk1Edt;

  protected:
    CnvAttribute   XCnv, YCnv;
    FmtAttribute   XFmt, YFmt;

  public:
    SzSelBrkGrf1() :
      XCnv(DC_L, "um"),
      YCnv(DC_Frac, "%"),
      XFmt("", 0, 1, 'f'),
      YFmt("", 0, 2, 'f')
      {
      }
  };

// ==========================================================================

DEFINE_TAGOBJEDT(SzSelBrk1);
class DllImportExport SzSelBrk1 : public TaggedObject
  {
  friend class SzSelBrk1Edt;

  DEFINE_SPARES(SzSelBrk1)
  protected:

    CDArray         LclSz;
    CDMatrx         SB;
    SzSelBrkGrf1    PCG;

    int             iSizeDefn;
    int             iApplyToDist;

  public:
    SzSelBrk1(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach);
    SzSelBrk1(pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~SzSelBrk1();

    //Does Nothing: virtual void   BuildDataDefn_Lcl(DataDefnBlk & DDB);
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    double         Selection(int s) { return SB[s][s]; };
    void           SetSelection(int s, double Frac) { SB[s][s] = Frac; };
    double         Breakage(int s, int d) { return SB[s][d]; };
    void           SetBreakage(int s, int d, double Frac) { SB[s][d] = Frac; };

    void           SetSizeDefn(int iDef);
    int            GetSizeDefn()           { return iSizeDefn; };
    void           CheckSB();

    int            Length()                { return SizePts().GetSize(); };
    void           SetLength(int N);

    flag           UserSizePts()           { return iSizeDefn<0; };
    CDArray       &SizePts()               { return LclSz; };

    int            NDists()                { return SD_Defn.NDistributions(); };
    flag           DistExists(int i)       { return SD_Defn.DistExists(i); };

    int            ApplyToDist()           { return iApplyToDist; };
    void           SetApplyToDist(int i)   { iApplyToDist=i; };

    flag           ApplyToAllDists()       { return iApplyToDist<0; };

    int            GetApplyToDists(int &d0, int &dN)
                     {
                     d0=(iApplyToDist>=0) ? iApplyToDist : 0;
                     dN=(iApplyToDist>=0) ? Min(iApplyToDist+1, NDists()) : NDists();
                     return dN-d0;
                     };

  protected:
    char*          GetRangeLbl(int i, Strng& Str, flag Ascend, flag Range);
  };

// ===========================================================================
//
//
//
// ===========================================================================

_FWDDEF(SzSelBrk1Edt);
class DllImportExport SzSelBrk1Edt : public FxdEdtBookRef
  {
  public :
    SzSelBrkGrf1 &PCG;
    SzSelBrk1     &PC;
    int            iNameWidth; //
    int            iWorkDist;
    int            iPg1;

    TagInfoBlk     WrkIB;
    //CnvAttribute   *pWrkCnv;
    //FmtAttribute   *pWrkFmt;

    int Length() { return PC.SB.GetSize(); };

    int    ApplyToDist() { return PC.iApplyToDist; };
    void   SetApplyToDist(int i) { PC.iApplyToDist=i; };

    SzSelBrk1Edt(pFxdEdtView pView_, pSzSelBrk1 pSD_);
    virtual ~SzSelBrk1Edt();

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
    virtual flag DoMouseMove(UINT nFlags, CPoint point);

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

#undef DllImportExport

#endif
