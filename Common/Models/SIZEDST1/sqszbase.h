//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __SQSZBASE_H
#define  __SQSZBASE_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif  
#ifndef __2D_FN_H
  #include "2d_fn.h"
#endif  
#ifndef __SCDARRAY_H
  #include "scdarray.h"
#endif  
#ifndef __DATAATTR_H
  #include "dataattr.h"
#endif  
#ifndef __SP_CONT_H
  #include "sp_cont.h"
#endif  
#ifndef __SFE_INIT_H
  #include "sfe_init.h"
#endif
#ifdef __SQSZBASE_CPP
  #define DllImportExport DllExport
#elif !defined(SizeDst1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ==========================================================================
//
//
//
// ==========================================================================

class SQSzDist1;
class SQSzDist1Edt1;
class SzPartCrv1;
class SzSelBrk1;

// ==========================================================================

#if UseExtraColumns
#define UseAllColumns 0 //kga:if 1 include SumG, xxxG columns (doubt we will ever need these)
#else
#define UseAllColumns 0
#endif

const dword SzDSlct_Fp   = 0x00000001;
const dword SzDSlct_Qm   = 0x00000002; 
const dword SzDSlct_M    = 0x00000004;
const dword SzDSlct_NFp  = 0x00000008;
const dword SzDSlct_Qn   = 0x00000010;
const dword SzDSlct_N    = 0x00000020;
const dword SzDSlct_NpM  = 0x00000040;

const dword SzDSlct_Cum  = 0x00010000;
#if UseAllColumns
const dword SzDSlct_CumG = 0x00020000;
#endif

#if UseAllColumns
const int DI_MFp     = 0; // Mass Related 
const int DI_Qm      = 1;
const int DI_M       = 2;
const int DI_MFpCum  = 3;
const int DI_QmCum   = 4;
const int DI_MCum    = 5;
const int DI_MFpCumG = 6;
const int DI_QmCumG  = 7;
const int DI_MCumG   = 8;

const int DI_NFp     = 9; // Count Related
const int DI_Qn      = 10;
const int DI_N       = 11;
const int DI_NFpCum  = 12;
const int DI_QnCum   = 13;
const int DI_NCum    = 14;
const int DI_NFpCumG = 15;
const int DI_QnCumG  = 16;
const int DI_NCumG   = 17;

const int DI_NpM     = 18; // Specific Count
const int DI_NpMCum  = 19;
const int DI_NpMCumG = 20;

const int DI_Xtra    = 21;
#else
const int DI_MFp     = 0; // Mass Related 
const int DI_Qm      = 1;
const int DI_M       = 2;
const int DI_MFpCum  = 3;
const int DI_QmCum   = 4;
const int DI_MCum    = 5;

const int DI_NFp     = 6; // Count Related
const int DI_Qn      = 7;
const int DI_N       = 8;
const int DI_NFpCum  = 9;
const int DI_QnCum   = 10;
const int DI_NCum    = 11;

const int DI_NpM     = 12; // Specific Count
const int DI_NpMCum  = 13;

const int DI_Xtra    = 14;
#endif

// ===========================================================================
//
//
//
// ===========================================================================

enum SeriesDataType { e_Manual, e_Q , e_Sqrt , e_Ratio , e_Arithmetic };

class CSieveSeriesHelper
  {
  public:
    CSieveSeriesHelper() { pCnv=NULL; Precision=-1; m_SeriesDataType = e_Ratio; m_Data=sqrt(2.0); m_DataMin=0.1; m_DataMax=500.0; m_bQMidPt=1; };
    CSieveSeriesHelper(char* pName, char* pUnits) { m_Name=pName; Units=pUnits; pCnv=NULL; Precision=-1; m_SeriesDataType = e_Q; m_Data=0.0; m_DataMin=0.5; m_DataMax=490.0; m_bQMidPt=1; };
    void SortSizes();
    flag ChangeUnits(char* pNewCnvTxt);
    void CalcDisplayPrecision();
    void FormatSize(double d, CString& S);
    void CreateSeriesFromQ();
    void CreateSeriesSQRT();
    void CreateSeriesFromRatio();
    void CreateSeriesArithmetic();

    void CreateSeriesFromData(); // Calls one of the above depending on m_SeriesDataType

  public:
    Strng m_Name;
    Strng Units;

    SeriesDataType m_SeriesDataType;

    //double m_Q;
    //double m_QMin;
    //double m_QMax;
    //bool m_bQMidPt;
    double m_Data;     // Data has different meaning for differnet SeriesDataType
    double m_DataMin;
    double m_DataMax;
    bool m_bQMidPt;

    //CArray <double, double> Sizes;
    CArray <double, double> CalculatedSizes;
    CArray <double, double> ManualSizes;
    //CDVector Sizes;
    CDataCnv* pCnv;
    int Precision;
    double Q() { return m_Data; };   //   
    double Ratio() { return m_Data>0 ? pow(2.0, 1.0/(3.0*m_Data)) : -1; }; // 

  };

class CSizeDistHelper
  {
  public:
    CSizeDistHelper() {};
    CSizeDistHelper(char* pName, char* pSieveSeriesName) { Name=pName; SieveSeriesName=pSieveSeriesName; };
    void SetDefaultTopBottom(CSieveSeriesHelper* pSeries);

  public:
    Strng Name;
    Strng SieveSeriesName;
    double dTopSize;
    double dBottomSize;
    double dTopSizeDisplay;
    double dBottomSizeDisplay;
    Strng DefaultSpecie;
    CArray <Strng, Strng&> Species;
  };

_FWDDEF(CSD_Measurement)
class CDistMeasHelper
  {
  public:
    CDistMeasHelper() {};

  public:
 
    CArray <CSD_Measurement, CSD_Measurement> Measurements;

  };

typedef CSieveSeriesHelper* pCSieveSeriesHelper;
typedef CSizeDistHelper* pCSizeDistHelper;
typedef CDistMeasHelper* pCDistMeasHelper;

class CSD_CfgHelper
  {
  public:
    ~CSD_CfgHelper();
    CSieveSeriesHelper* FindSieveSeries(char* pSeriesName);
    CSizeDistHelper* FindDistribution(char* pDistName);
    void Load(CProfINIFile & CfgPF);
    void Save(CProfINIFile & CfgPF);
    int CreateDefaultSD();
    int CreateDefaultSS();
    int CreateDefaultMeasurement();
    void RemoveDistribution(int index);
    void RemoveSieveSeries(int index);

    CArray <pCSieveSeriesHelper, pCSieveSeriesHelper&> SSs;
    CArray <pCSizeDistHelper, pCSizeDistHelper&> SDs;
    CArray <pCDistMeasHelper, pCDistMeasHelper&> SDsMeas;
  };

// ===========================================================================

class CSD_Intervals;
class CSD_Distribution;
class CSD_DistDefn;

// ==========================================================================
#ifdef OLD
class DllImportExport CSD_Intervals : public CDArray
  {
  friend class SfeSD_Defn;
  protected:
    Strng  m_sName;
    double m_Q;  // From Aust CSIRO docs for QAL 3rd Page 

  public:
    CSD_Intervals() { m_Q=0; };
    // Copy Constructor.
    CSD_Intervals(CSD_Intervals & Copy)
      { 
      m_sName=Copy.m_sName;
      m_Q=Copy.m_Q;
      };
    char * Name() { return m_sName(); };
    int    NameLength() { return m_sName.Length(); };

    void SetQ(double d) { m_Q=d; };   //   
    double Q() { return m_Q; };       //   
    double Ratio() { return m_Q>0 ? pow(2.0, 1.0/(3.0*m_Q)) : -1; }; // 
  };
#endif

class DllImportExport CSD_Intervals : public CDArray
  {
  friend class SfeSD_Defn;
  protected:
    Strng  m_sName;
    //double m_Q;  // From Aust CSIRO docs for QAL 3rd Page 
    double m_Data; // m_Data replaces m_Q as can have different types
                   // of Series

    SeriesDataType m_SeriesDataType;

  public:
    CSD_Intervals() { m_SeriesDataType = e_Manual; m_Data=0; };
    // Copy Constructor.
    CSD_Intervals(CSD_Intervals & Copy)
      { 
      m_sName=Copy.m_sName;
      m_Data=Copy.m_Data;
      };
    char * Name() { return m_sName(); };
    int    NameLength() { return m_sName.Length(); };

    //
    // 
    //
    void SetSeriesType(SeriesDataType iSeriesDataType ) { m_SeriesDataType = iSeriesDataType; };
    SeriesDataType GetSeriesType(void) { return m_SeriesDataType; };

    //
    // Q Series Methods - Should we throw an exception if the
    // SeriesDataType is not a Q type?
    //
    void SetQ(double d) { m_Data=d; };   //   
    double Q() { return m_Data; };       //   
    double Ratio() { return m_Data>0 ? pow(2.0, 1.0/(3.0*m_Data)) : -1; }; //


  };

// ==========================================================================

struct ColumnInitInfo
  {
  char *         Name;
  flag           fFileIt;
  flag           fOn;
  flag           fForFlow;
  flag           fForMass;
  flag           fForCum;
  dword          m_dwVarSlct;
  int            iDataId;
  CnvAttribute   *pCnv;
  FmtAttribute   *pFmt;
  flag           fEditable;
  flag           fGrfOn;
  dword          dwSaveFlags;
  flag           fDone;
  };

class DllImportExport CSD_Column
  {
  public:
    //const int MaxSelectCols=4;

    CSD_Column();
    CSD_Column(ColumnInitInfo & CI, char * SpName, int Xid, int SpId, flag SimpleNames);

//   char* DispName() { return (fSimpleNames ? sName() : sFullName()); };
    char* DispName() { return (fSimpleNames ? sName() : sFullNameColon()); }; // CNM
    void SetOn(flag On) { fOn=On; };
    flag On() { return fOn; };
    void SetForFlow(flag ForFlow) { fForFlow=ForFlow; };
    flag ForFlow() { return fForFlow; };
    void SetForMass(flag ForMass) { fForMass=ForMass; };
    flag ForMass() { return fForMass; };
    flag ForCum() { return fForCum; };
    flag Editable() { return fEditable; };
    flag SimpleNames() { return fSimpleNames; };
    void SetGrfOn(flag GrfOn) { fGrfOn=GrfOn; };
    flag GrfOn() { return fGrfOn; };

    flag Avail(SpModel &p) { return ((sm_VarSlctMask & m_dwVarSlct)==m_dwVarSlct) && (fForFlow==p.UseAsFlow() || fForMass==!p.UseAsFlow()) ; };
    flag Reqd(SpModel &p) { return fOn && Avail(p); };

  public:
    Strng         sFullName, sFullNameColon, sName, sSpName;
    int           iSpId, iDataId, iXid;
    double        dDispMin, dFDispMax; 
    CnvAttribute *pCnv;
    FmtAttribute *pFmt;
    dword         dwSaveFlags;
    int           iColWidth;
    dword         m_dwVarSlct;
    static dword  sm_VarSlctMask;

  protected:
    flag    fOn;

    flag    fForFlow:1,
            fForMass:1,
            fEditable:1,
            fForCum:1,
            fSimpleNames:1,
            fGrfOn:1;
  };

class DllImportExport CSD_ColumnArray : public CArray <CSD_Column, CSD_Column&> {};

// ==========================================================================

class DllImportExport CSD_ColumnOpt
  {
  public:
    Strng  sName;
    int    iOpt;
  };

class DllImportExport CSD_ColumnOptArray : public CArray <CSD_ColumnOpt, CSD_ColumnOpt&> {};

// ==========================================================================

const int MaxCSDMeasurements=40;

enum eSDMeasTypes 
  {
  eSDMT_Null, 
  eSDMT_SizePassFrac, 
  eSDMT_FracPassSize, 
  eSDMT_PPG, 
  eSDMT_SAM, 
  eSDMT_SAL, 
  eSDMT_Text
  };

class DllImportExport CSD_Measurement
  {
  public:
    eSDMeasTypes m_eType;  // Measurement Type
    double m_dValue;       // Value for measurment if required
    Strng  m_sName;        // Measurement Name
    CnvAttribute   ValCnv; // Value Conversion Attribute
    CnvAttribute   Cnv;    // Result Conversion Attribute
    FmtAttribute   Fmt;

    CSD_Measurement()
      {
      m_eType  = eSDMT_Null;
      m_dValue = 0;
      };

    CSD_Measurement& operator=(const CSD_Measurement &M)
      {
      m_eType   = M.m_eType;
      m_dValue  = M.m_dValue;
      m_sName   = M.m_sName;
      Cnv       = M.Cnv;
      ValCnv    = M.ValCnv;
      Fmt       = M.Fmt;
      return *this;
      }
    
    //
//    short  m_iCnv;
//    Strng  m_sCnv; 
  };

class DllImportExport CSD_MeasurementArray : public CArray <CSD_Measurement, CSD_Measurement&> {};

// ==========================================================================

class DllImportExport CSD_DistDefn
  {
  friend class CSD_Distribution;
  friend class SfeSD_Defn;
  friend class SQSzDist1;
  friend class SQSzDist1Edt;
  friend class CSzSSA;

  protected:
    Strng            sName;
    int              iDefaultDist; // Set to index where all unrefd solids are to be
    CSMatrx          sSzNames;
    CIMatrx          iSzIds;
    // Grade
    CSArray          GrdNames;
    CLVector         GrdIds;

    double           dBottomSize, dTopSize;
    double           dBottomSizeDisp, dTopSizeDisp;

    double           dSzDispMin;
    double           dSzDispMax;

    flag             fAutoScale:1,
                     fDispRetained:1,
                     fXLog:1,
                     fYLog:1;
                     //fViewIsMass:1;

  public:
    CSD_Intervals    Intervals;   // Actual Intervals;
    //CDArray          BondWIInit;  // N+1 Individual Bond Work Indices.
    CSD_ColumnOptArray ColumnOpts;
    CSD_MeasurementArray Measurements;

    CSD_ColumnArray    Columns;
    CArray <int, int>  FileColIndices;
    CArray <int, int>  DispColIndices;


  public:
    CSD_DistDefn()
      {
      iDefaultDist=-1;

      dBottomSize=1.0e-8;
      dBottomSizeDisp=1.0e-8;
      dTopSize=10.0;
      dTopSizeDisp=10.0;
      fAutoScale=true;
      //fCumulativeOn=true;
      //fFractionalOn=true;
      fDispRetained=true;

      fXLog=true;
      fYLog=false;
  
      dSzDispMin=0.0001;
      dSzDispMax=1;
      }

    char *           Name() { return sName(); };
    int              NameLength() { return sName.Length(); };

    double           BottomSize() { return dBottomSize; };
    double           TopSize() { return dTopSize; };
    double           BottomSizeDisp() { return dBottomSizeDisp; };
    double           TopSizeDisp() { return dTopSizeDisp; };

    int              NIntervals() { return Intervals.GetSize();};
    int              NGrades()    { return GrdNames.GetSize();};
  
    double           GeometricMean(int iInt)
                        { return iInt==0 ? sqrt(Intervals[iInt] * dBottomSize) : sqrt(Intervals[iInt] * Intervals[iInt-1]); };
//                        { return iInt==0 ? 0.0 : sqrt(Intervals[iInt] * Intervals[iInt-1]); };
//                        { return iInt==0 ? sqrt(Intervals[iInt]) : sqrt(Intervals[iInt] * Intervals[iInt-1]); };
    double           MeanPartDiam(int iInt) 
                        {
                        if (iInt==0) 
                          return Intervals[iInt]*0.5; 
                        if (iInt==NIntervals()-1) 
                          return ((Intervals[iInt-1]+Intervals[iInt])*0.5)*Intervals[iInt]/Intervals[iInt-1]; 
                        return (Intervals[iInt-1]+Intervals[iInt])*0.5; 
                        };
//                        { return iInt==0 ? (Intervals[iInt]+dBottomSize)*0.5 : (Intervals[iInt-1]+Intervals[iInt])*0.5; };

    int              NPriIds()    { return iSzIds.GetSize(); };
    int              NSecIds(int i) { return iSzIds[i].GetSize(); };
    int              SzId(int i, int j) { return iSzIds[i][j]; }; //return SpecieId
    int              PriSzId(int i) { return iSzIds[i][0]; }; //return Pri SpecieId
    int              FindPriSzIdIndex(int SpecieId) 
      { for (int i=0; i<iSzIds.GetSize(); i++)
          if (iSzIds[i].GetSize()>0 && iSzIds[i][0]==SpecieId)
            return i;
        return -1; };

    int NColumnOpts() { return ColumnOpts.GetSize(); };
    int NColumns() { return Columns.GetSize(); };

  };

// ==========================================================================

class DllImportExport CSD_SpDist
  {
  friend class SfeSD_Defn;
  friend class SQSzDist1;
  friend class SQSzDist1Edt;
  friend class CSD_Distribution;
  friend class CSD_SpDistArray;
  friend class CSzSSA;

  public:
    CDArray          FracPass;    // Ni Fractions Passing Interval size 
    //CDArray          BondWI;      // Ni Individual Bond Work Indices.
    //CDMatrx          Grade;       // Ng by Ni Grades of M Species

    CDArray          WorkFrac;    // Ni Fractions Passing Interval size 

  public:
    CSD_SpDist();
    CSD_SpDist(const CSD_SpDist & Other);
    CSD_SpDist & operator=(const CSD_SpDist & Other);

    virtual ~CSD_SpDist();

    DEFINE_SPARES(CSD_SpDist)

  };

// ==========================================================================

typedef CSmartPtrAllocate<CSD_SpDist> CSPASD_SpDist;
class DllImportExport CSD_SpDistArray : public CArray <CSPASD_SpDist, CSPASD_SpDist&> {};

// ==========================================================================
extern DllImportExport double gs_CountFactor;

class DllImportExport CSD_Distribution
  {
  friend class SfeSD_Defn;
  friend class SQSzDist1;
  friend class SQSzDist1Edt;
  friend class CSzSSA;

  protected:
    CSD_DistDefn  &  rDefn;
    CSD_Intervals &  rIntervals;  // Reference to Actual Intervals;

    int              iCurrentDataId;
    int              iCurrentSpId;
    CDArray          SpMass;
    CDArray          SpDensity;
    CDArray          WorkSpace;
    CDArray          WorkSpace1;
    //CDArray          TempMass;
    CDArray         *pResults;

  public:
    CSD_SpDistArray  PriSp;

    int              NIntervals() { return rIntervals.GetSize();};
    int              NGrades()    { return rDefn.GrdNames.GetSize();};
  
    double           BottomSize() { return rDefn.dBottomSize; };
    double           TopSize()    { return rDefn.dTopSize; };
    double           BottomSizeDisp() { return rDefn.dBottomSizeDisp; };
    double           TopSizeDisp()    { return rDefn.dTopSizeDisp; };

    CSD_Intervals &  Intervals()  { return rIntervals;};

    int              NPriIds()    { return rDefn.iSzIds.GetSize(); };
    int              NSecIds(int i) { return rDefn.iSzIds[i].GetSize(); };
    int              SzId(int i, int j) { return rDefn.iSzIds[i][j]; };
    int              PriSzId(int i) { return rDefn.iSzIds[i][0]; };
    int              FindPriSzIdIndex(int SpecieId) { return rDefn.FindPriSzIdIndex(SpecieId); };
    double           GeometricMean(int iInt) { return rDefn.GeometricMean(iInt); };
    double           MeanPartDiam(int iInt) { return rDefn.MeanPartDiam(iInt); };
    double           MeanPartMass(int iInt, double MeanDens) 
                       { 
                       //double R=MeanPartDiam(iInt)/2; 
                       //return 4/3*PI*(R*R*R)*MeanDens;
                       //return 4.0/3.0*PI*(R*R*R)*MeanDens;
                       const double D = MeanPartDiam(iInt); 
                       return PI*(D*D*D)*MeanDens/6.0; 
                       };
    double           PartCount(int iInt, double Mass, double MeanDens) 
                       { 
                       return Mass/GTZ(MeanPartMass(iInt, MeanDens))*gs_CountFactor; 
                       };
    double           PartDensity(int iInt, double MeanDens) { return 1.0/GTZ(MeanPartMass(iInt, MeanDens)); };
    double           SpecificSurfaceAreaM(double T, double P, SpPropOveride * pOvr, double *M) 
                        { 
                        double TotalF=0.0;
                        double TotalA=0.0;
                        double TotalM=0.0;
                        for (int s=0; s<NPriIds(); s++)
                          TotalM+=M[PriSzId(s)];
                        TotalM=GTZ(TotalM);
                        for (int s=0; s<NPriIds(); s++)
                          {
                          double SolDens=SDB[PriSzId(s)].Density(SpModel::Fidelity(), T, P, pOvr, M);
                          CSD_SpDist & S=*PriSp[s];
                          double Mult=M[PriSzId(s)]/TotalM;
                          for (int i=0; i<NIntervals(); i++)
                            {
                            const double D=MeanPartDiam(i); 
                            const double Y=3.0/GTZ(0.5*SolDens*D);  // m^2/kg
                            TotalF+=Mult*S.FracPass[i];
                            TotalA+=Mult*Y*S.FracPass[i];
                            }
                          }
                        return 0.001*TotalA/GTZ(TotalF);
                        }
    double           SpecificSurfaceAreaV(double T, double P, SpPropOveride * pOvr, double *M, double LiqVol) 
                        { 
                        double TotalF=0.0;
                        double TotalA=0.0;
                        for (int s=0; s<NPriIds(); s++)
                          {
                          double SolDens=SDB[PriSzId(s)].Density(SpModel::Fidelity(), T, P, pOvr, M);
                          CSD_SpDist & S=*PriSp[s];
                          for (int i=0; i<NIntervals(); i++)
                            {
                            const double D=MeanPartDiam(i); 
                            const double Y=3.0/GTZ(0.5*SolDens*D);  // m^2/kg
                            TotalA+=M[PriSzId(s)]*Y*S.FracPass[i];
                            }
                          }
                        return TotalA/GTZ(LiqVol);
                        }
    double SolidsMass(double T, double P, SpPropOveride * pOvr, double *M) 
                        { 
                        double TotalM=0.0;
                        for (int s=0; s<NPriIds(); s++)
                          TotalM+=M[PriSzId(s)];
                        return TotalM;
                        }
    /*double SolidsDensity(double T, double P, SpPropOveride * pOvr, double *M) 
                        { 
                        //double TotalM=0;
                        //double TotalV=0;
                        //for (int s=0; s<NPriIds(); s++)
                        //  TotalM+=M[PriSzId(s)];
                        //TotalM=GTZ(TotalM);
                        //for (int s=0; s<NPriIds(); s++)
                        //  {
                        //  CSD_SpDist & S=*PriSp[s];
                        //  //double Mult=M[PriSzId(s)]/TotalM;
                        //  //double Volume=M[PriSzId(s)]/
                        //  for (int i=0; i<NIntervals(); i++)
                        //    {
                        //    double D=MeanPartDiam(i); 
                        //    double Y=3.0/GTZ(0.5*MeanDens*D);  // m^2/kg
                        //    TotalF+=Mult*S.FracPass[i];
                        //    TotalA+=Mult*Y*S.FracPass[i];
                        //    };
                        //  };
                        //return TotalA/GTZ(TotalF);

                        return 2420;

                      //  return TotalM;
                        }*/
    
    CSD_DistDefn  &  Defn() { return rDefn; };

    char*     Name() { return rDefn.sName(); };
    int       NameLength() { return rDefn.sName.Length(); };

    CSD_Distribution(CSD_DistDefn* pDefn);
    CSD_Distribution(const CSD_Distribution & Other);
    CSD_Distribution& operator=(const CSD_Distribution & Other);
    virtual   ~CSD_Distribution();

    flag      ValidateData(ValidateDataBlk & VDB);

    void      ZeroMass();
    void      AddMassF(CSysVector &M1, double M1Mult, CSD_Distribution *pD2, CSysVector &M2, double M2Mult);
    
    double    Crush0(C2DFn & PartFn);
    double    Crush1(int s, C2DFn & PartFn);

    double    Crush0(SzPartCrv1 & PartFn, int CurveNo);
    double    Crush1(int s, SzPartCrv1 & PartFn, int CurveNo);

    double    Break0(SzSelBrk1 & PartFn);

    double    CrushExt0(SzPartCrv1 &Extents, CSD_Distribution &FdD, int CurveNo);
    double    CrushExt1(int s, SzPartCrv1 &Extents, CSD_Distribution &FdD, int CurveNo);

  protected:
    void      ClearWorkSpace();
    void      ClearWorkSpace1();
    
    CDArray & Results() { return *pResults; };
    void      SetResults(CDArray & Results) { pResults=&Results; };
    
    void      DistTotals(SpPropOveride *Ovr, CSysVector &M1, int iSpPriId, double &Mass, double &Volume);
    void      CalcDensity(SpPropOveride *Ovr, CSysVector &M1, int iSpId);

    double    GetMass(SpPropOveride *Ovr, CSysVector &M1, int iSpId);
    double    GetMassFrac(SpPropOveride *Ovr, CSysVector &M1, int iSpId);
    void      GetSpCount(SpPropOveride *Ovr, CSysVector &M1, int iSpId);
    void      GetCount(SpPropOveride *Ovr, CSysVector &M1, int iSpId);
    void      GetCountFrac(SpPropOveride *Ovr, CSysVector &M1, int iSpId);
    void      SetCountFrac(SpPropOveride *Ovr, CSysVector &M1, int iSpId, int iInt, double Val);
    void      SetSpCount(SpPropOveride *Ovr, CSysVector &M1, int iSpId, int iInt, double Val);
    flag      CalculateResults(SpPropOveride *Ovr, CSysVector &M1, int DataId, int SpId);

    DEFINE_SPARES(CSD_Distribution)

  };

// ==========================================================================
//
//
//
// ==========================================================================

enum SzLabelStyles { LS_Full, LS_Short };

class DllImportExport SfeSD_Defn : public SfeInitClass, public CMdlGeneralInfo
  {
  public:
    SfeSD_Defn();
    virtual ~SfeSD_Defn();
    virtual void    Clear();
    virtual flag    LoadConfiguration(CProfINIFile & CfgPF);
    virtual flag    Initialise();
    virtual flag    Terminate();

    SzLabelStyles   LabelStyle() { return LblStyle; };
    int             NIntervals() { return Intervals.GetSize(); }; //return number of defined sieve series
    CSD_Intervals * GetIntervals(int i) { return Intervals[i]; };
    int             NDistributions() { return Distributions.GetSize(); }; //return number of defined distributions
    flag            DistExists(int i) { return (i<Distributions.GetSize() && Distributions[i]!=NULL); };
    CSD_DistDefn *  GetDist(int i) { return Distributions[i]; };
    int             NPriIds(int i) { return (DistExists(i) ? Distributions[i]->NPriIds() : 0); }; //return number of species used in this distribution
    int             FindIndex(int i, int SpecieId) { return (DistExists(i) ? Distributions[i]->FindPriSzIdIndex(SpecieId) : -1); };

    CSD_DistDefn *  FindDistributionFor(int SpecieId);

    virtual bool    GetInfo();

    CnvAttribute    XCnv, YFCnv, YQmCnv, YMCnv, YNFCnv, YQnCnv, YNCnv, YNpMCnv;
    FmtAttribute    XFmt, YFFmt, YQmFmt, YMFmt, YNFFmt, YQnFmt, YNFmt, YNpMFmt;
    CnvAttribute    TBCnv; //top/bottom size
    FmtAttribute    TBFmt; //top/bottom size

  protected:
    CArray <CSD_Intervals*, CSD_Intervals*> Intervals;
    CArray <CSD_DistDefn*, CSD_DistDefn*> Distributions;

    SzLabelStyles   LblStyle;
 
    void ParseSeries(Strng & CurTkn, CTokenFile &Tkns);
    void ParseTyler(Strng & CurTkn, CTokenFile &Tkns);
    void ParseDistribution(Strng & CurTkn, CTokenFile &Tkns);
  };

// ==========================================================================

extern DllImportExport SfeSD_Defn SD_Defn; //global pointer for size distribution management

extern flag DllImportExport fSzAscend;

// ==========================================================================
//
//
//
// ==========================================================================

class DllImportExport SQSzDist1TagObjClass : public SQTagObjClass
  {
  protected:
    CnvAttribute FCnv;
    FmtAttribute FFmt;

  public:
    SQSzDist1TagObjClass(pchar pClassName, pchar pGroup_, pchar pClassId_, pchar SubClassId, pchar pVersion_, pchar pDrwGroup_, pchar pDefTag_, dword dwCat, pchar ShortDesc, pchar pDesc, DWORD SelectMask);

    virtual int NTearVariables();
    virtual int DefineTearVariables(TearVarArray & TV, int n);
  };

// ==========================================================================
//
//
//
// ==========================================================================

#undef DllImportExport 

#endif
