//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __CLFLWEQN_H
#define  __CLFLWEQN_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __DATACNVS_H
  #include "datacnvs.h"
#endif
#ifndef __TAGOBJ_H
  #include "tagobj.h"
#endif
#ifndef __ODESOLVE_H
  #include "odesolve.h"
#endif
#ifndef __SP_CONT_H
  #include "sp_cont.h"
#endif
#ifndef __SP_CONT_H
  #include "sp_cont.h"
#endif

//#pragma warning(3: 4244)

#ifdef __FLWEQN_CPP
  #define DllImportExport DllExport
#elif !defined(FLWLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ===========================================================================

#define dbgFlwEqns  WITHDEBUG
#define WithCHOKEDFLW 1

#define UseNewThermalBlock 10

// ===========================================================================

const int NxidFEIOs=250;

XID xidFlwEqnGrp            = FlwXID(10000);
XID xidFlwEqnNm             = FlwXID(10001);
XID xidFlwEqnDesc           = FlwXID(10002);
XID xidFlwEqnIdStr          = FlwXID(10003);
XID xidFlwEqnLocation       = FlwXID(10004);
XID xidFlwEqnDiam           = FlwXID(10005);
XID xidFlwEqnArea           = FlwXID(10006);           
XID xidFlwEqnRqdLen         = FlwXID(10007);           
XID xidFlwEqnActLen         = FlwXID(10008);           
XID xidFlwEqnFitLen         = FlwXID(10009);           
XID xidFlwEqnDZ             = FlwXID(10010);           
XID xidFlwEqnKFact          = FlwXID(10011);           
XID xidFlwEqnKFactOvr       = FlwXID(10012);           
XID xidFlwEqnDatumFlng      = FlwXID(10013);           
XID xidFlwEqnDatumNode      = FlwXID(10014);           

XID xidFlwEqnQmMode         = FlwXID(10021);
XID xidFlwEqnQmAsMass     = FlwXID(10022);
XID xidFlwEqnQmReqd         = FlwXID(10023);
XID xidFlwEqnQvReqd         = FlwXID(10024);
XID xidFlwEqnQmRqdCapacity  = FlwXID(10025);
XID xidFlwEqnQmMinCapacity  = FlwXID(10026);
XID xidFlwEqnQmMaxCapacity  = FlwXID(10027);
XID xidFlwEqnQvRqdCapacity  = FlwXID(10028);
XID xidFlwEqnQvMinCapacity  = FlwXID(10029);
XID xidFlwEqnQvMaxCapacity  = FlwXID(10030);

//XID xidFlwEqnPressOn        = FlwXID(10040);
XID xidFlwEqnNomDP          = FlwXID(10041);
XID xidFlwEqnNomQm          = FlwXID(10042);
XID xidFlwEqnNomQv          = FlwXID(10043);

XID xidFEArea           = FlwXID(20000+ 0*NxidFEIOs);
XID xidFELength         = FlwXID(20000+ 1*NxidFEIOs);
XID xidFEDZ             = FlwXID(20000+ 2*NxidFEIOs);
XID xidFEVelocity       = FlwXID(20000+ 3*NxidFEIOs);
XID xidFEDPb            = FlwXID(20000+ 4*NxidFEIOs);
XID xidFEDPbX           = FlwXID(20000+ 5*NxidFEIOs);
XID xidFEDPbdQ          = FlwXID(20000+ 7*NxidFEIOs);
XID xidFEDPq            = FlwXID(20000+ 8*NxidFEIOs);
XID xidFEDPqdQ          = FlwXID(20000+ 9*NxidFEIOs);
XID xidFEDPa            = FlwXID(20000+10*NxidFEIOs);
XID xidFEDPadQ          = FlwXID(20000+11*NxidFEIOs);
XID xidFEDPz            = FlwXID(20000+12*NxidFEIOs);
XID xidFEDPzdQ          = FlwXID(20000+13*NxidFEIOs);
XID xidFEDPmB           = FlwXID(20000+14*NxidFEIOs);
XID xidFEDPmQ           = FlwXID(20000+15*NxidFEIOs);
XID xidFEDPmdQ          = FlwXID(20000+16*NxidFEIOs);
XID xidFEQm             = FlwXID(20000+17*NxidFEIOs);

XID xidFEQmEst          = FlwXID(20000+18*NxidFEIOs);

XID xidFEQmPrvPB        = FlwXID(20000+19*NxidFEIOs);
XID xidFEQmAvail        = FlwXID(20000+20*NxidFEIOs);
XID xidFEQmSpace        = FlwXID(20000+21*NxidFEIOs);
//XID xidFEQmMemPB        = FlwXID(20000+24*NxidFEIOs);
XID xidFEBstDamping     = FlwXID(20000+25*NxidFEIOs);

// Max of 40
XID xidFELast           = FlwXID(20000+26*NxidFEIOs);
XID xidFEFirst          = xidFEArea;

const double Min_FE_Rho_Value=1.0e-3;
const double Max_FE_Rho_Value=20000.0;
inline double Min_FE_Rho(double Rho) { return Range(Min_FE_Rho_Value,Rho,Max_FE_Rho_Value); };

const double PBQmEst         = 1.0e100;
const double MaxXfCap        = 1.0e10;
const double MinValveOpening = 1.0e-8;
const double SmallPosFlow    = 2.0*ZeroLimit;
const double LargePosFlow    = 1.0e6*MaxXfCap;

inline bool FlwExists(double q) {return (fabs(q) > 0.9*SmallPosFlow);};
inline bool FlwZero(double q)   {return (fabs(q) < 0.9*SmallPosFlow);};
inline bool FlwLarge(double q)  {return (fabs(q) >= 1e-3*LargePosFlow);};
inline bool FlwPos(double q)    {return (q > 0.9*SmallPosFlow);};
inline bool FlwGEZ(double q)    {return (q >= 0.0);};
inline bool FlwNeg(double q)    {return (q < -0.9*SmallPosFlow);};
inline bool FlwLEZ(double q)    {return (q <= 0.0);};
inline sint FlwSign(double q)   { return (q > 0.9*SmallPosFlow) ? 1 : (q < -0.9*SmallPosFlow) ? -1 : 0;}

const double BigPressure           = 1.0e6;
const double LowPressure           = 1.0;  // Press limited to be >=
const double LowPressThreshold     = 50.0; // Below this press Converge Damp applied incrementally until @ LowPressure = LowPressDamp
const double LowPressDamp          = 2.0;
const double LowPressureTest       = 1.1;  // Error message if Press <

extern DllImportExport double MinAperture[2]; // { Dynamic, InLineIntegral}
extern DllImportExport double MinFracHgt[2];
extern DllImportExport double MaxFracHgt[2];  

class CFlwThermalBlk; // forward
class CClosureInfo; 
//============================================================================
//
//
//
//============================================================================

class CPressBlk
  {
  public:
    CPressBlk();
    void Init();


    double    getP()                { return m_P;       };
    double    getPEst()             { return m_PEst;    };
    double    getPMn()              { return m_PMn;     };
    double    getPGood()            { return m_PGood;   };
    float     getPMax()             { return m_PMax;    };
    float     getPMaxLim()          { return m_PMaxLim; };

    void      putP(double P)        { m_P=P;        };
    void      putPEst(double P)     
      { 
      //dbgpln("pb[%4i] PEst=%10.2f", m_iNo, m_PEst);
      m_PEst=P;     
      };
    void      putPMn(double P)      { m_PMn=P;      };
    void      putPGood(double P)    { m_PGood=P;    };
    void      putPMax(float P)      { m_PMax=P;     };
    void      putPMaxLim(float P)   { m_PMaxLim=P;  };

    _declspec(property(get=getP,put=putP))                double P; 
    _declspec(property(get=getPEst,put=putPEst))          double PEst; 
    _declspec(property(get=getPMn,put=putPMn))            double PMn; 
    _declspec(property(get=getPGood,put=putPGood))        double PGood; 
    _declspec(property(get=getPMax,put=putPMax))          float  PMax; 
    _declspec(property(get=getPMaxLim,put=putPMaxLim))    float  PMaxLim; 

    double  * PAdd()                { return &m_P;       };
    double  * PEstAdd()             { return &m_PEst;    };
    double  * PMnAdd()              { return &m_PMn;     };
    double  * PGoodAdd()            { return &m_PGood;   };
    float   * PMaxAdd()             { return &m_PMax;    };
    float   * PMaxLimAdd()          { return &m_PMaxLim; };

  protected:
    double    m_P;
    double    m_PEst;
    double    m_PMn;

    double    m_PGood;

    float     m_PMax;        // Used to Estimate Max Pressures in links in fixed mass mode
    float     m_PMaxLim;     // Used to Estimate Max Pressures in links in fixed mass mode

  public:
    static long sm_iNo;
    long      m_iNo;


  };

class CPressBlkArray : public CArray <CPressBlk, CPressBlk&>
  {
  public :
    CPressBlk & FirstElement() { return ElementAt(0); };
    CPressBlk & LastElement() { return ElementAt(GetUpperBound()); };
  };

// ===========================================================================

class CFBPhysData
  {
  public:

    CFBPhysData()
      {
      ResetData(true);
      };

    void ResetData(flag Complete)
      {
      if (Complete)
        {
        bIsPipe=false;
        dRqdLen=1.0;
        dActLen=1.0;
        dFitLen=0.0;
        SetDiam(0.1);//SetDiam(0.019);
        dRise=0.0;
        dDatumNode=0.0;
        dDatumFlng=0.0;
        KFact.dVal=1.0;
        KFact.dOvr=dNAN;
        }
      };

    bool            IsPipe()                { return bIsPipe; };
    double          TotLength()             { return dActLen+dFitLen; };
    double          RqdLength()             { return dRqdLen;  };
    double          ActLength()             { return dActLen;  };
    double          FitLength()             { return dFitLen; };
    double          Area()                  { return dArea;    };
    double          Diam()                  { return Sqrt(dArea*4.0/PI); };
    void            SetIsPipe(bool B)       { bIsPipe=B; };
    void            SetDiam(double D)       { dArea=PI*Sqr(D)/4.0; };
    void            SetArea(double A)       { dArea=A; };
    void            SetRqdLength(double A)  { dRqdLen=A; };
    void            SetActLength(double A)  { dActLen=A; };
    void            SetFitLength(double A)  { dFitLen=A; };

    void            SetRise(double A)       { dRise=A; };
    void            SetDatumFlng(double A)  { dDatumFlng=A; };
    void            SetDatumNode(double A)  { dDatumNode=A; };
    double          Rise()                  { return dRise; };
    double          DatumFlng()             { return dDatumFlng; };
    double          DatumNode()             { return dDatumNode; };

  public:
    bool            bIsPipe;
    double          dRqdLen,        // Required Length
                    dActLen,        // Actual Length
                    dFitLen,        // Fitting Length
                    dArea,          // Area
                    dRise,          // Change In Datum ( + Towards Node }
                    dDatumNode,     // Datum Node Side
                    dDatumFlng;     // Datum Flng Side
    DualDbl         KFact;

  };

// ===========================================================================

class DllImportExport CSpPropInfo
  {
  public:
    CSpPropInfo()
      {
      Reset();
      };
    void Reset()
      {
      m_bValid          = false;
      m_bLastSegment    = true;
      m_dRhoL           = 997.16;
      m_dRhoV           = AtmosDensity();
      m_dTemp           = Std_T;
      m_dPress          = AtmosPress();
      m_dVisc           = 0.001;
      m_dLVolFrac       = 0.0;
      m_dVVolFrac       = 1.0;
      m_dLVolFracBal    = 0.0;
      m_dLenFraction    = 1.0;
      };

    void SetPropInfoVL(SpModel & Md, double VVolFrac, double LVolFrac, SpMArray * pImage);
    void SetPropInfoX(SpConduit & Cd);
    
    double RhoL()             { return m_dRhoL; };
    double RhoV(double P)     { return m_dRhoV * (P/m_dPress) /*/ GTZ(m_dTemp/Norm_T)*/; };
    double Rho(double P, double Override)
                              {
                              if (Valid(Override))
                                return Override;
                              double Rv=RhoV(P);
                              double Rl=m_dRhoL;
                              double Ml=Rl*NettLVolFrac();
                              double Mv=Rv*NettVVolFrac();
                              return Min_FE_Rho((Mv+Ml)/(Ml/Rl+Mv/Rv));
                              };
    
    double Temp(double Override)              { return Valid(Override) ? Override : m_dTemp; };
    double DynamicViscosity(double Override)  { return Valid(Override) ? Override : m_dVisc; };       
    
    double VVolFrac()                         { return m_dVVolFrac; };
    double LVolFrac()                         { return m_dLVolFrac; };
    double NettLVolFrac();
    double NettVVolFrac();
    double AsVMassFrac(double VapVolFrac, double LiqVolFrac, double P);
    double AsLMassFrac(double VapVolFrac, double LiqVolFrac, double P);
   
    double msVolL()                           { return 1/Min_FE_Rho(m_dRhoL); };
    double msVolV(double P)                   { return 1/Min_FE_Rho(RhoV(P)); };
    double msVol(double P, double Override)   { return 1/Min_FE_Rho(Rho(P, Override)); };

  public:
    bool   m_bValid,         // Set if these values have been updated
           m_bLastSegment;   // set if this is the last in the sequence 
    double m_dRhoL,          // Density of Liquid/Slurry
           m_dRhoV,          // Density of Vapour @ T,P
           m_dTemp,          // Temperature of Flow
           m_dPress,         // Pressure of Flow
           m_dVisc,          // Viscosity
           m_dLVolFrac,      // Liquor Volume Fraction
           m_dVVolFrac,      // Vapour Volume Fraction
           m_dLVolFracBal,   // Liquor Volume Fraction Balancing
           m_dLenFraction;   // Fraction Of TotalLength

  };

class DllImportExport CSpPropInfoArray : public CArray<CSpPropInfo, CSpPropInfo&> {};

//============================================================================

class DllImportExport CFlwEqnFlwRqd
  {
  public:

    bool          m_AsMass;
    double        m_MeasDens;
    double        m_ReqdQm;  
    double        m_ReqdQv;  

    double        m_RqdQmCap;
    double        m_MinQmCap;
    double        m_MaxQmCap;

    double        m_RqdQvCap;
    double        m_MinQvCap;
    double        m_MaxQvCap;
    
    double        m_PressDP;
    double        m_PressQm;
    double        m_PressQv;

  };

//============================================================================
//
//
//
//============================================================================

const byte FEOF_Null             = 0x00;
const byte FEOF_WithAutoComp     = 0x01;
const byte FEOF_WithPVConst      = 0x02;
const byte FEOF_ShowPVConst      = 0x04;

class DllImportExport CFlwEqn  : public TaggedObject
  {
  friend class CFlwEqnPtr;
  friend class CFlwBlkBase;
  public:
    CFlwEqn(TagObjClass * pClass_, pchar pTag, TaggedObject * pAttach, TagObjAttachment eAttach);

    virtual ~CFlwEqn() {};

    virtual void    BuildDataDefn(DataDefnBlk & DDB)=0;
    virtual void    BuildDataDefnOveride(DataDefnBlk & DDB, TaggedObject * pThis=NULL);
    virtual flag    DataXchg(DataChangeBlk & DCB);
    virtual flag    ValidateData(ValidateDataBlk & VDB);

    void            SetOverideTauRqd(double Value) { m_dOverideTauRqd=Value; }; 
    void            SetOverideRhoLRqd(double Value) { m_dOverideRhoLRqd=Value; }; 
    void            SetOverideRhoVRqd(double Value) { m_dOverideRhoVRqd=Value; }; 
    void            SetOverideTempRqd(double Value) { m_dOverideTempRqd=Value; };
    void            SetOverideViscRqd(double Value) { m_dOverideViscRqd=Value; };

    void            SetOveride(double RhoLRqd, double RhoVRqd, double TempRqd, double ViscRqd)
                      {
                      m_dOverideRhoLRqd=RhoLRqd;  
                      m_dOverideRhoVRqd=RhoVRqd;  
                      m_dOverideTempRqd=TempRqd; 
                      m_dOverideViscRqd=ViscRqd;
                      };

    void            ClrOverideTauRqd()  { m_dOverideTauRqd=dNAN; }; 
    void            ClrOverideRhoLRqd() { m_dOverideRhoLRqd=dNAN; }; 
    void            ClrOverideRhoVRqd() { m_dOverideRhoVRqd=dNAN; }; 
    void            ClrOverideTempRqd() { m_dOverideTempRqd=dNAN; };
    void            ClrOverideViscRqd() { m_dOverideViscRqd=dNAN; };
    void            ClrOveride()
                      {
                      m_dOverideRhoLRqd=dNAN;  
                      m_dOverideRhoVRqd=dNAN;  
                      m_dOverideTempRqd=dNAN; 
                      m_dOverideViscRqd=dNAN; 
                      };

    void            SetLinearised(bool On, double DP, double Qm, double Regulation)
                      {
                      m_bLinearOn=On;
                      m_dLinearDP=DP;
                      m_dLinearQm=Qm;
                      m_dLinearReg=Regulation;
                      }
    void            ClrLinearised()
                      {
                      m_bLinearOn=false;
                      }
    
    virtual void    InitFlowInfo(CFlwBlkBase & FE) {};
    virtual void    StepFlowInfo(CFlwBlkBase & FE) {};
    virtual flag    EvalFlowInfoStart(CFlwBlkBase & FE) { return False; };
    virtual void    EvalFlowInfoEnd(CFlwBlkBase & FE) { };
    virtual flag    TestFlowInfo(CFlwBlkBase & FE) { return True; };

    virtual void    SetPhysData(double DZ);
    virtual void    SetKFact(double K) { m_PhD.KFact.SetVal(K, this); };
    virtual double  GetKFact() { return m_PhD.KFact.Val(); };
    virtual double  GetFricFact() { return dNAN; };
    virtual flag    EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)=0;
    virtual pchar   FlwEqnName();
    //static  double  RangeFlow(double Flow, double LoLimit);
    virtual void    EvalProducts(CFlwBlkBase & FE, double Pi, double Po, CFlwThermalBlk & FTB);
    virtual void    EvalDiscrete(CFlwBlkBase & FE);

    static int      DoDumpTest;
    static int      TestLvl;
    void            DumpTest(char * Fn, double Lo, double Hi, double Delta,
                            CFlwBlkBase & FE, double Regulation, double P);

    double          TotLength()             { return m_PhD.TotLength(); };
    double          RqdLength()             { return m_PhD.RqdLength(); };
    double          ActLength()             { return m_PhD.ActLength(); };
    double          FitLength()             { return m_PhD.FitLength(); };
    double          Area()                  { return m_PhD.Area();      };
    double          Diam()                  { return m_PhD.Diam();      };
    void            SetDiam(double D)       { m_PhD.SetDiam(D); };
    void            SetArea(double A)       { m_PhD.SetArea(A); };
    void            SetRqdLength(double A)  { m_PhD.SetRqdLength(A); };
    void            SetActLength(double A)  { m_PhD.SetActLength(A); };
    void            SetFitLength(double A)  { m_PhD.SetFitLength(A); };
    void            SetRise(double A)       { m_PhD.SetRise(A); };
    void            SetDatumFlng(double A)  { m_PhD.SetDatumFlng(A); };
    void            SetDatumNode(double A)  { m_PhD.SetDatumNode(A); };
    double          Rise()                  { return m_PhD.Rise(); };
    double          DatumFlng()             { return m_PhD.DatumFlng(); };
    double          DatumNode()             { return m_PhD.DatumNode(); };
    DualDbl     &   KFact()                 { return m_PhD.KFact; };

    double          LScale();
    double          ZScale();

    virtual double  ShaftPower()            { return dNAN; };
    
    flag            Linearised()            { return m_bLinearOn; };
    double          LinearisedDP(double Qm, double Regulation);

    CFlwEqnPtr     * EqnPtr()                { return m_pEqnPtr; };
    //CVLEBase      * VLE();

    protected:
    DEFINE_MEMSTATS(CFlwEqn)

  public: // Temporary
    CFBPhysData      m_PhD;
    long            m_nEvals;

  protected:
    double          m_dOverideRhoLRqd;
    double          m_dOverideRhoVRqd;
    double          m_dOverideTempRqd;
    double          m_dOverideTauRqd;
    double          m_dOverideViscRqd;
    
    flag            m_bLinearOn;
    double          m_dLinearDP;
    double          m_dLinearQm;
    double          m_dLinearReg;
    // used by all
    double         m_dViscMeas;
    double         m_dDensMeas;
    double         m_dVelMeas;
    double         m_dVelSonicMeas;
    double         m_dDP;
    //double         m_dQm;

    CFlwEqnPtr     * m_pEqnPtr;

    CFlwEqnFlwRqd  m_FlwRqd;
  };

// ===========================================================================

class DllImportExport CFlwEqnGrp
  {
  friend class CFlwEqnPtr;
  //public
  protected:
    char*     pGroupName;
    char*     pDefaultInstance;

  public:
    CFlwEqnGrp() { pGroupName=pDefaultInstance=NULL; };
    CFlwEqnGrp(char* GroupName, char* DefaultInstance) { pGroupName=GroupName; pDefaultInstance=DefaultInstance; };

    char*     Name() { return pGroupName; };
    char*     Default() { return pDefaultInstance; };
    void      SetDefault(char * Def) { pDefaultInstance=Def; };
  };

// ===========================================================================

const byte FBB_ShowAs_Off       = 0;
const byte FBB_ShowAs_Note      = 1;
const byte FBB_ShowAs_Warn      = 2;
const byte FBB_ShowAs_Error     = 3;

const byte FBPS_Default         = 0x00;
const byte FBPS_FwdOnly         = 0x01;
const byte FBPS_RevOnly         = 0x02;
const byte FBPS_Inherit         = 0x10;

const byte FBMode_Xfer            = 0x01;
const byte FBMode_Simple          = 0x02;
const byte FBMode_Linear          = 0x03;
const byte FBMode_Full            = 0x04;

const byte FBOpt_AsValve          = 0x01;
const byte FBOpt_HasSimpleRes     = 0x02;

const long FBDDOpt_WithData       = 0x0100;
const long FBDDOpt_WithEqn        = 0x0200;
const long FBDDOpt_WithXtraEqn    = 0x0400;
const long FBDDOpt_HideEqnStruct  = 0x0800;
const long FBDDOpt_WithQmSpec     = 0x1000;
const long FBDDOpt_WithDPSpec     = 0x2000;
const long FBDDOpt_WithGroup      = 0x4000;
const long FBDDOpt_WithAll        = FBDDOpt_WithData|FBDDOpt_WithEqn;

struct CFBPropSelector
  {
  byte           m_iReqd;  
  byte           m_iInUse;
  CFBPropSelector()
    {
    m_iReqd  = FBPS_Default; 
    m_iInUse = FBPS_Default;
    }
  };

const byte FPh_None     = 0x00;
const byte FPh_Liquid   = 0x01; 
const byte FPh_Gasses   = 0x02; 
const byte FPh_Both     = 0x03; 
const byte FPh_One      = 0x04; 
const byte FPh_UnKnown  = 0x08;
const byte FPh_Default  = 0x10;

class DllImportExport CFlwBlkData
  {
  friend class FlwNode;
  friend class FlwSlvDyn;
  protected:

  public:
    CFlwBlkData();
    virtual ~CFlwBlkData();

    void           ResetData(flag Complete);

    CPressBlk     &FlngPB()             { return *m_pFlPB; };
    CPressBlk     &NodePB()             { return *m_pNdPB; };
    CPressBlk     &SrcPB(flag Fwd=true) { return *(Fwd ? m_pFlPB:m_pNdPB); };
    CPressBlk     &DstPB(flag Fwd=true) { return *(Fwd ? m_pNdPB:m_pFlPB); };


  public:       // <<<<<<<<<< TO BE REMOVED <<<<<<<<<<<
    
    CFBPropSelector m_PropSel;   // 

    flag           m_bClosed;       // Set to indicate that SetIO_Closed had been called
    flag           m_fDerivsBad;     // Set if derivatives can be relied on
    flag           m_fFunctOfPress;  // Set if values in this flwblk are pressure dependant
    flag           m_fEvalFwdOnly;   // Set if values in this flwblk are to be eval only forward
    flag           m_fEvalRevOnly;   // Set if values in this flwblk are to be eval only forward
    flag           m_fHoldFastConverge;// Set if Fast Converge is NOT allowed

    sint           m_sQMeas,            // sign of flow
                   m_iDirnRel2Traverse, // Dirn Relative to Traverse
                   m_iDirnRel2Connect;  // Dirn Relative to Connection to be set by models;
    

    double         m_Qm,             // MassFlow    ( + Towards Node }
                   m_QmEst,          // Netwrk Solution Qm;
                   m_QmEstGood,      // Qm at Start of Iteration
                   m_QmAvail,        //
                   m_QmSpace,        //
                   m_QmPrvPB;        // Previous Iterations Qm - Only used in ProBal;

    byte           m_MeasType;       // 
    bool           m_MeasDone;       // Cleared in StepFlowInfo - beginning of each step
    double         m_QmMeas,         // Qm @ LowerLimit
                   m_MeasRho,        // Measured Density - each call
                   m_MeasRhoStep,    // Measured Density - @ Start of each Step
                   m_MeasArea,       // 'Area' 
                   m_MeasLoLimit,    // QmLimit

                   m_dPb,            // Boost_Head
                   m_dPbX,           // Boost_Head To Achiev Required Flow
                   m_dPbdQ,          // 1st Derivative of Boost_Head wrt Q
                   m_dPq,            // FlowDrop (for display only)
                   m_dPqdQ,          // 1st Derivative of FlowDrop wrt Q
                   m_dPa,            // Acceleration DP
                   m_dPadQ,          // 1st Derivative of FlowDrop wrt Q
                   m_dPz,            // Static_Head (RhoH)
                   m_dPzI,           // Static_Head (RhoH) additional due to Implicit solution
                   m_dPzdQ,          // 1st Derivative of Static_Head wrt Q
                   m_dPzX,           // Static_Head (RhoH) External - In Surge units
                   m_dPzXdQ,         // 1st Derivative of External Static_Head wrt Q
                   m_dPmB,           // Momentum_Head Boost Component
                   m_dPmQ,           // Momentum_Head Resist Component
                   m_dPmdQ,          // 1st Derivative of Resist Component
                   m_ResT;           // Resistance (Total)

    double         m_SimpleRes;      // Simple Mode Resistance = 

    double         m_MomentScl,      // Momentum Scaling
                   m_Vel,            // Velocity
                   m_VelSonic;       // Sonic Velocity

    double         m_Regulation;     // Current Regulation Setting
    double       * m_pUserValues;    // Current 'Other Data - to pass data from model to eqn'
    long           m_nUserValues;    // Current 'Other Data - to pass data from model to eqn'

    CPressBlk    * m_pFlPB;          // Pressure at Flange Side
    CPressBlk    * m_pNdPB;          // Pressure at Node Side

    double         m_dOverideTau;
    double         m_dOverideRhoL;
    double         m_dOverideRhoV;
    double         m_dOverideTemp;
    double         m_dOverideVisc;

    byte           m_iFlwPh_Rqd;
    byte           m_iFlwPh_Act;

  public:
    static byte    sm_iFlwPh_Force;
    static byte    sm_iFlwPh_Block;
    
    static byte    sm_iShowVelLimitAs;
    static double  sm_dVelLimitSL;
    static double  sm_dVelLimitV;
    
    static byte    sm_iShowQmDiffAs;
    static CToleranceBlock m_QmMatchTol;
    static long    sm_iNo;
    long           m_iNo;

  protected:
    double         dVThrottle,
                   dLThrottle;

    double         dSlip,
                   dVoidFrac,
                   dOnePhPart,
                   dTempOut;

    SpConduit    * m_pCd;
    SpConduit    * pTwoPhCd;
    flag           fTwoPhase;

    CFlwBlkBase   * pSrcFB;
    CFlwBlkBase   * pDstFB;

    double         dBstDamping;

    flag           m_fMomentumValid;
    flag           m_fApplyStaticHead;
    flag           m_fFindStaticHead;

  };

// ===========================================================================

class DllImportExport CFlwEqnPtr //: public CFBPhysData
  {
  friend class CFlwEqn;
  friend class CFlwBlkBase;
  public:
    CFlwEqnPtr(CFlwBlkData *pData, long Options, double DPFactor);
    virtual ~CFlwEqnPtr();

    CFlwEqnPtr & operator=(const CFlwEqnPtr & A);

    CFlwEqnPtr     & Eqn2Use() { return m_pParentFEP ? *m_pParentFEP : *this; };

    void            SetOptions(long Options, double DPFactor);
    virtual void    ResetData(flag Complete);
    flag            Disabled()    { return Eqn2Use().m_fDisabled; };
    void            SetDisabled(flag On) { Eqn2Use().m_fDisabled=On; };
    flag            GroupOK()     { return Eqn2Use().m_sGroup()!=NULL; };
    CFlwEqnGrp    & Group()       { return *(Eqn2Use().m_pGroup); };
    char *          GroupName()   { return Eqn2Use().m_sGroup(); };
    TaggedObject  * AttachedTo()  { return Eqn2Use().m_pAttachedTo; };

    byte            FlowMode()              { return m_FlowMode; };
    void            SetFlowMode(byte Md);
    CFlwEqn        * Eqn()         { return Eqn2Use().m_pEqn; };
    CFBPhysData    & PhD()         { return Eqn2Use().m_pEqn->m_PhD; };
    CFlwEqnFlwRqd  & FlwRqd()      { return Eqn2Use().m_pEqn->m_FlwRqd; };
    bool            FlwRqdActive()         
      {
      if (Eqn2Use().FlowMode()!=FBMode_Xfer)
        {
        CFlwEqnFlwRqd &FR=FlwRqd();

        if (Valid(FR.m_ReqdQm) || Valid(FR.m_ReqdQv))
          return true;
        }
      return false;
      }

    CFlwEqn        * CreateFlwEqn(pchar Name_);
    CFlwEqn        * ChangeFlwEqn(pchar Name_, flag ChgDefault);
    CFlwEqn        * ChangeFlwEqn(pTagObjClass pRqdEqnClass_);

    void            SetParentFlwEqn(CFlwEqnPtr *RqdFEP);

    CFlwEqn       * AssignFlwEqnGroup(CFlwEqnGrp &Group, pchar Rqd, pTaggedObject pAttach);
    virtual void    BuildDataDefn(long DDOptions, DataDefnBlk & DDB, pTaggedObject pOwnObj, char * FBTag=NULL, DWORD UserInfo=-1, DDBPages PgOpt=DDB_NoPage);
    virtual flag    DataXchg(DataChangeBlk & DCB);
    virtual flag    ValidateData(ValidateDataBlk & VDB);

    pchar           FlwEqnName() { return Eqn()->FlwEqnName();};

    void            SetPhysData(double DZ)  { Eqn()->SetPhysData(DZ); };
    void            SetFBScales(double LScl, double ZScl)  { m_dLScl=LScl; m_dZScl=ZScl; };

    void            SetKFact(double K)      { Eqn()->SetKFact(K); };
    double          GetKFact()              { return Eqn()->GetKFact(); };
    double          GetFricFact()           { return Eqn()->GetFricFact(); };

    bool            IsPipe()                { return Eqn()->m_PhD.bIsPipe; };
    void            SetIsPipe(bool B)       { Eqn()->m_PhD.bIsPipe=B; };

    long            Options()               { return Eqn2Use().m_Options;};

    double          LScale()                { return m_dLScl; };
    double          ZScale()                { return m_dZScl; };
    double          TotLength()             { return m_dLScl*Eqn()->m_PhD.TotLength(); };
    double          RqdLength()             { return m_dLScl*Eqn()->m_PhD.RqdLength(); };
    double          ActLength()             { return m_dLScl*Eqn()->m_PhD.ActLength();};
    double          FitLength()             { return m_dLScl*Eqn()->m_PhD.FitLength();};
    double          Area()                  { return Eqn()->m_PhD.Area(); };
    double          Diam()                  { return Eqn()->m_PhD.Diam(); };
    void            SetDiam(double D)       { Eqn()->m_PhD.SetDiam(D); };
    void            SetArea(double A)       { Eqn()->m_PhD.SetArea(A); };
    void            SetRqdLength(double A)  { Eqn()->m_PhD.SetRqdLength(A/m_dLScl); };
    void            SetActLength(double A)  { Eqn()->m_PhD.SetActLength(A/m_dLScl); };
    void            SetFitLength(double A)  { Eqn()->m_PhD.SetFitLength(A/m_dLScl); };
    void            SetRise(double A)       { Eqn()->m_PhD.SetRise(A/m_dZScl); };
    void            SetDatumFlng(double A)  { Eqn()->m_PhD.SetDatumFlng(A); };
    void            SetDatumNode(double A)  { Eqn()->m_PhD.SetDatumNode(A); };
    double          Rise()                  { return m_dZScl*Eqn()->m_PhD.Rise(); };
    double          DatumFlng()             { return Eqn()->m_PhD.DatumFlng(); };
    double          DatumNode()             { return Eqn()->m_PhD.DatumNode(); };
    DualDbl     &   KFact()                 { return Eqn()->m_PhD.KFact; };

    //double          FR_Resistance(double Density) { return m_dLScl*Eqn()->m_FlwRqd.Resistance(Density);};
    double          FR_Resistance(double Density, double Regulation);

    double          ShaftPower();

    void            SetOveride(double RhoLRqd, double RhoVRqd, double TempRqd, double ViscRqd) { Eqn()->SetOveride(RhoLRqd, RhoVRqd, TempRqd, ViscRqd); }
    void            ClrOveride()             { Eqn()->ClrOveride(); };
                    
    flag            Linearised()             { return Eqn()->Linearised(); };
    double          LinearisedDP(double Qm, double Regulation) { return m_dLScl*Eqn()->LinearisedDP(Qm, Regulation); };
    void            SetLinearised(bool On, double DP, double Qm, double Regulation) { Eqn()->SetLinearised(On, DP/m_dLScl, Qm, Regulation); };
    void            ClrLinearised() { Eqn()->ClrLinearised(); };

  private:
    byte            m_FlowMode;
    CFlwBlkData   * m_pData;
    CFlwEqn       * m_pEqn;
    CFlwEqnPtr    * m_pParentFEP;
    CList<CFlwEqnPtr*, CFlwEqnPtr*> m_Children;
    TaggedObject * m_pAttachedTo;
    //CVLEBase      *m_pVLE;
    CFlwEqnGrp   * m_pGroup;
    Strng          m_sGroup;
    flag           m_fDisabled;
    double         m_dLScl;
    double         m_dZScl;
    long           m_Options;

  };

// ===========================================================================

class DllImportExport CFlwBlkBase : public CFlwBlkData,  public CFlwEqnPtr
  {
  public:
    CFlwBlkBase();//CVLEBase * pVLE=NULL);
    virtual ~CFlwBlkBase();

    virtual void   BuildDataDefn(long DDOptions, DataDefnBlk & DDB, pTaggedObject pOwnObj, char * FBTag=NULL, DWORD UserInfo=-1, DDBPages PgOpt=DDB_NoPage);
    virtual flag   DataXchg(DataChangeBlk & DCB);

    void           ResetData(flag Complete);
    virtual void   InitFlowInfo(SpConduit * pCd);
    virtual void   StepFlowInfo();
    virtual flag   EvalFlowInfoStart();
    virtual void   EvalFlowInfoEnd();
    virtual flag   TestFlowInfo();

    flag           EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);//, double P1, double P2);
    flag           SolveFlwEqn(CSpPropInfo *pProps, double Regulation, double P1_, double P2_, double & ReqdFlw);
    void           EvalProducts(double Pi, double Po, SpConduit & C, CFlwThermalBlk & FTB);
    void           EvalDiscrete();

    void           EvalGetModeRqd(long FlowMode, bool On, bool IsDegFreedom, CSpPropInfo *pProps, double Regulation);

    virtual flag   InXferNet()=0;
    virtual void   SetXferMode()=0;
    virtual void   SetXferCapacity(double QmCap)=0;
    virtual void   SetQmReqd(double QmReqd, flag IsDegFree=true, float MaxNode=fNAN, float MaxFlng=fNAN)=0;
    virtual void   SetQmReqdVlv(double QmReqd, double Reg, flag IsDegFree=true, float MaxNode=fNAN, float MaxFlng=fNAN)=0;

    void           SetDerivsBad(flag On=True)         { m_fDerivsBad=On; };
    void           ClrDerivsBad()                     { m_fDerivsBad=false; };
    void           SetFunctOfPress(flag On=True)      { m_fFunctOfPress=On; };
    void           ClrFunctOfPress()                  { m_fFunctOfPress=false; };

    void           SetEvalFwdOnly(flag On=True)       { m_fEvalFwdOnly=On; };
    void           ClrEvalFwdOnly()                   { m_fEvalFwdOnly=false; };
    void           SetEvalRevOnly(flag On=True)       { m_fEvalRevOnly=On; };
    void           ClrEvalRevOnly()                   { m_fEvalRevOnly=false; };
    void           SetHoldFastConverge(flag On=True)  { m_fHoldFastConverge=On; };
    void           ClrHoldFastConverge()              { m_fHoldFastConverge=false; };
    
    virtual void   SetQmFree()=0;

    void           SetCd(SpConduit *Cd)     { m_pCd=Cd; };
    SpConduit    & Cd()                    { ASSERT(m_pCd); return *m_pCd; };

    void           SetTwoPhase(flag On)     { fTwoPhase=On; };
    flag           TwoPhase()               { return fTwoPhase; };
    void           SetTwoPhCd(SpConduit *Cd){ pTwoPhCd=Cd; };
    SpConduit    & TwoPhCd()                { ASSERT(pTwoPhCd); return *pTwoPhCd; };
    SpConduit    * TwoPhCdPtr()             { return pTwoPhCd; };

    void           SetSrcFB(CFlwBlkBase *FB) { pSrcFB=FB; };
    flag           SrcFBOK()                { return pSrcFB!=NULL; };
    CFlwBlkBase   & SrcFB()                  { return *pSrcFB; };
    void           SetDstFB(CFlwBlkBase *FB) { pDstFB=FB; };
    flag           DstFBOK()                { return pDstFB!=NULL; };
    CFlwBlkBase   & DstFB()                  { return *pDstFB; };

    void           SetFlngPress(double P)   { FlngPB().P=P; };
    void           SetNodePress(double P)   { NodePB().P=P; };
    double         FlngPress()              { return FlngPB().P; };
    double         NodePress()              { return NodePB().P; };

    double         MeanPress(sint FD)       { return (FD>0) ? FlngPB().PMn : (FD<0) ? NodePB().PMn : 0.5*(FlngPB().PMn+NodePB().PMn); };
    double         MeanPress()              { return MeanPress(m_sQMeas); };
    double         MeanFlngPress()          { return FlngPB().PMn; };
    double         MeanNodePress()          { return NodePB().PMn; };
    double         MeanFBPress()            { return 0.5*(FlngPB().PMn+NodePB().PMn); };

    void           SetPropSelect(byte iProps)
      {
      switch (iProps)
        {
        case FBPS_Default:
          m_PropSel.m_iInUse = iProps;
          break;
        case FBPS_FwdOnly: 
          m_PropSel.m_iInUse = m_iDirnRel2Connect>0 ? FBPS_FwdOnly : FBPS_RevOnly;
          break;
        case FBPS_RevOnly: 
          m_PropSel.m_iInUse = m_iDirnRel2Connect<0 ? FBPS_FwdOnly : FBPS_RevOnly;
          break;
        }
      }

    CSpPropInfo *  Properties(CSpPropInfo * pProps);
    double         MeanTemp(CSpPropInfo * pProps, flag AllowOveride=true);
    double         MeanRho(CSpPropInfo * pProps, double P, flag AllowOveride=true);
    double         MeanRho(CSpPropInfo * pProps) { return MeanRho(pProps, MeanPress()); };
    double         MeanRhoL(CSpPropInfo * pProps, flag AllowOveride=true);
    double         MeanRhoV(CSpPropInfo * pProps, double P, flag AllowOveride=true);
    double         MeanViscosity(CSpPropInfo * pProps, flag AllowOveride=true);

    double         MeasRhoA() { return m_MeasRho*m_MeasArea; };

    double         NettLVolFrac(CSpPropInfo * pProps);
    double         NettVVolFrac(CSpPropInfo * pProps);

    double         StaticHead(CSpPropInfo * pProps);
    flag           ApplyStaticHead()           { return m_fApplyStaticHead; };
    void           SetApplyStaticHead(flag On) { m_fApplyStaticHead=On; };
    flag           FindStaticHead()           { return m_fFindStaticHead; };
    void           SetFindStaticHead(flag On) { m_fFindStaticHead=On; };

    double         VThrottle()              { return dVThrottle; };
    double         LThrottle()              { return dLThrottle; };
    void           VThrottle(double T)      { dVThrottle=T; };
    void           LThrottle(double T)      { dLThrottle=T; };

    void           SetVelocity(double V, double SonicV=dNAN)    { m_Vel=V; m_VelSonic=SonicV; };
    double         Velocity()               { return m_Vel; };
                                               
    void           SetRegulation(double A)  { m_Regulation=A; };
    double         Regulation()             { return m_Regulation; };
    void           SetUserValues(double * V, int N)  { m_pUserValues = (N>0?V:NULL); m_nUserValues = N; };
    double         UserValue(int i)         { if (i<0 || i>=m_nUserValues) throw new CInvalidArgException;  return m_pUserValues[i]; };

    int            QmSign()                 { return m_sQMeas; };
    double         QmMeas(double Scl=1.0);
    double         QvMeas(double Scl=1.0);
    double         VelMeas(double Scl=1.0);
    double         DQmMeas(double Scl=1.0);
    static double  RangeFlow(double Qm, double LoLimit);
    double         SetQmMeasRange(double Rho, double LoQm); 
    double         SetQvMeasRange(double Rho, double LoVol);
    double         SetVelMeasRange(double Rho, double Area, double LoVel);

    void           SetDPb(double A)         { m_dPb=A; };
    double         DPb()                    { return m_dPb; };
    void           SetDPbX(double A)        { m_dPbX=A; };
    double         DPbX()                   { return m_dPbX; };
    void           SetDPbdQ(double A)       { m_dPbdQ=A; };
    double         DPbdQ()                  { return m_dPbdQ; };
    void           SetDPq(double A)         { m_dPq=A; };
    double         DPq()                    { return m_dPq; };
    void           SetDPqdQ(double A)       { m_dPqdQ=A; };
    double         DPqdQ()                  { return m_dPqdQ; };
    void           SetDPa(double A)         { m_dPa=A; };
    double         DPa()                    { return m_dPa; };
    void           SetDPadQ(double A)       { m_dPadQ=A; };
    double         DPadQ()                  { return m_dPadQ; };
    void           SetDPz(double A)         { m_dPz=A; };
    double         DPz()                    { return m_dPz; };
    void           SetDPzI(double A)        { m_dPzI=A; };
    double         DPzI()                   { return m_dPzI; };
    void           SetDPzdQ(double A)       { m_dPzdQ=A; };
    double         DPzdQ()                  { return m_dPzdQ; };
    void           SetDPzX(double A)        { m_dPzX=A; };
    double         DPzX()                   { return m_dPzX; };
    void           SetDPzXdQ(double A)      { m_dPzXdQ=A; };
    double         DPzXdQ()                 { return m_dPzXdQ; };
    void           SetDPmB(double A)        { m_dPmB=A; };
    double         DPmB()                   { return m_dPmB; };
    void           SetDPmQ(double A)        { m_dPmQ=A; };
    double         DPmQ()                   { return m_dPmQ; };
    void           SetDPmdQ(double A)       { m_dPmdQ=A; };
    double         DPmdQ()                  { return m_dPmdQ; };
    double         DPtot()                  { return m_dPb+m_dPbX+m_dPq+m_dPa+m_dPz/*+dPzI*/+m_dPmB+m_dPmQ; };

    void           SetDPa(double dPa_, double dPadQ_)   { m_dPa = dPa_; m_dPadQ = dPadQ_; };
    void           SetDPb(double dPb_, double dPbdQ_)   { m_dPb = dPb_; m_dPbdQ = dPbdQ_; }; 
    void           SetDPz(double dPz_, double dPzdQ_)   { m_dPz = dPz_; m_dPzdQ = dPzdQ_; }; 
    void           SetDPzX(double dPz_, double dPzdQ_)  { m_dPzX = dPz_; m_dPzXdQ = dPzdQ_; }; 
    void           SetDPq(double dPq_, double dPqdQ_)   
      {
      if (fabs(dPq_)<1e6) // a ridiculous pressure drop
        {
        m_dPq = dPq_; 
        m_dPqdQ = dPqdQ_; 
        }
      else
        {
        double Scl=fabs(dPq_)/1e6;
        m_dPq = dPq_/Scl; 
        m_dPqdQ = dPqdQ_/Scl; 
        }
      }; 
    void           AddDPa(double dPa_, double dPadQ_)   { m_dPa += dPa_; m_dPadQ += dPadQ_; };
    void           AddDPb(double dPb_, double dPbdQ_)   { m_dPb += dPb_; m_dPbdQ += dPbdQ_; }; 
    void           AddDPz(double dPz_, double dPzdQ_)   { m_dPz += dPz_; m_dPzdQ += dPzdQ_; }; 
    void           AddDPzX(double dPz_, double dPzdQ_)  { m_dPzX += dPz_; m_dPzXdQ += dPzdQ_; }; 
    void           AddDPq(double dPq_, double dPqdQ_)   { m_dPq += dPq_; m_dPqdQ += dPqdQ_; }; 
    void           ClrDPb() { SetDPb(0,0); };
    void           ClrDPq() { SetDPq(0,0); };
    void           ClrDPz() { SetDPz(0,0); };

    void           SetMomentum(CSpPropInfo * pProps, double P);
    void           AddMomentum(CSpPropInfo * pProps, double P);
    void           ClrMomentum();

    double         PAtFromEntry(double Frac);

    void           SetQm(double A)          { m_Qm=A; A=NZ(A); m_QmMeas = fabs(A); m_sQMeas = (sint)FlwSign(A); };
    void           SetVLFracs(CSpPropInfo * pProps, double V, double L);

    // help with MFB futures
    double         VapVolFrac(CSpPropInfo * pProps) { return Properties(pProps)->NettVVolFrac(); };
    double         LiqVolFrac(CSpPropInfo * pProps) { return Properties(pProps)->NettLVolFrac(); };
    double         PhaseFrac(CSpPropInfo * pProps, int Lp) { return Lp==0 ? VapVolFrac(pProps): LiqVolFrac(pProps); };

    double         GetQm()                  { return m_Qm; };
    void           SetQmEst(double A)       { m_QmEst=A;};
    double         GetQmEst()               { return m_QmEst; };
    void           SetQmPrvPB(double A)     { m_QmPrvPB=A;};
    double         GetQmPrvPB()             { return m_QmPrvPB; };
    void           SetQmAvail(double A)     { m_QmAvail=A;};
    double         GetQmAvail()             { return m_QmAvail; };
    void           SetQmSpace(double A)     { m_QmSpace=A;};
    double         GetQmSpace()             { return m_QmSpace; };

    //void           SetQmMemPB(double A)     { QmMemPB=A;};
    //double         GetQmMemPB()             { return QmMemPB; };

    void           SetBstDamping(double A)  { dBstDamping=A; };
    double         BstDamping()             { return dBstDamping; };

    void           SetOverideTau(double A)  { m_dOverideTau=A; };

    void           SetSlipRatio(double D=dNAN)    { dSlip=D; };
    double         SlipRatio()                      { return dSlip; };
    void           SetVoidFraction(double D=dNAN) { dVoidFrac=D; };
    double         VoidFraction()                   { return dVoidFrac; };
    void           SetOnePhPart(double D=dNAN)    { dOnePhPart=D; };
    double         OnePhPart()                      { return dOnePhPart; };
    void           SetTempOut(double D=dNAN)      { dTempOut=D; };
    double         TempOut()                        { return dTempOut; };

  public:
    static byte    fMomentumOn;
    static double  dHasFlowLimit;
    typedef enum ePropIndices {TheFwdProps=0, TheRevProps=1};
  public:
    CSpPropInfo    m_FlwProps[2];    // Props last Flowed in Fwd Dirn
    //CSpPropInfo   &m_FwdFlwProps;    // Props last Flowed in Fwd Dirn
    //CSpPropInfo   &m_RevFlwProps;    // Props last Flowed in Fwd Dirn 
    byte           m_iFwdFlwProps;   // index for actual use based on validity flags
    byte           m_iRevFlwProps;   // index for actual use based on validity flags
    bool           m_TopoChgRqd;

  };

//============================================================================
//
//
//
// ===========================================================================

class DllImportExport CFlwThermalBlk
  {
  public: 
    CFlwThermalBlk(FlwNode * Nd, CFlwEqnPtr * FEP);

    void          BuildDataDefn(DataDefnBlk & DDB, char* Tag, char* TagComment, DDBPages PageIs, byte Flags);
    flag          DataXchg(DataChangeBlk & DCB);
    flag          ValidateData(ValidateDataBlk & VDB);    

    void          BeginEvalProducts(SpConduit & Cd);
    void          AddWorkPower(double Power, double Press);
    void          AddHeatPower(double Power, double Press);
    void          AddVLEBegin();
    void          AddVLEEnd();
    void          AddEHXBegin();
    void          AddEHXEnd();
    void          AddRBBegin();
    void          AddRBEnd();
    void          AddACPower(double Power, double Press);
    void          EndEvalProducts();

    flag          AutoCompOn()  { return m_AutoCompOn;  };
    double      & TRiseRqd()    { return m_TRiseRqd;    };
    double      & PVConst()     { return m_PVConst; };

    double        TRiseMech()   { return m_TRiseMech;   };
    double        TRiseVLE()    { return m_TRiseVLE;    };
    double        TRiseEHX()    { return m_TRiseEHX;    };
    double        TRiseRB()     { return m_TRiseRB;     };
    double        TRiseAC()     { return m_TRiseAC;     };

    double        PowerMech()   { return m_PowerMech;   };
    double        PowerEHX()    { return m_PowerEHX;    };
    double        PowerRB()     { return m_PowerRB;     };
    double        PowerAC()     { return m_PowerAC;     };

    void          GetClosureInfo(CClosureInfo &CI, bool DoRB, bool DoEHX);

  protected:
    FlwNode     * m_pNd;
    CFlwEqnPtr   * m_pFEP;
    SpConduit   * m_pCd;

    enum          eTRSpec { TR_None, TR_TChange, TR_TFinal };
    byte          m_TRiseSpec;
    double        m_TRiseRqd;
    double        m_TFinalRqd;
    double        m_PVConst;
    flag          m_AutoCompOn;

    double        m_TRiseMech;     // Temp Rise due to Power Input - Resistance-  etc
    double        m_TRiseHeat;     // Temp Rise due to Heat Exchange
    double        m_TRiseVLE;      // Temp Rise due to flashing
    double        m_TRiseEHX;      // Temp Rise due to Environment
    double        m_TRiseRB;       // Temp Rise due to Reactions
    double        m_TRiseAC;       // Temp Rise due to Auto Compression

    double        m_PowerMech;     // Pwr due to Power Input - Resistance-  etc
    double        m_PowerHeat;     // Pwr due to Heat Exchange
    double        m_PowerEHX;      // Pwr due to Environment
    double        m_PowerRB;       // Pwr due to Reactions
    double        m_PowerAC;       // Pwr due to Auto Compression

    double        m_HfIn;           
    double        m_HfOut;          

    double        m_TIn;           
    double        m_TOut;          
    double        m_TRiseCorr;     // Temp Rise Total
    double        m_TRiseTot;      // Temp Rise Total
    double        m_PowerTot;      // 
    double        m_PowerCorr;     // 

    #if dbgFlwEqns  
    double        m_TRiseMechTmp;     
    double        m_TRiseHeatTmp;     
    double        m_TRiseVLETmp;     
    double        m_TRiseEHXTmp;     
    double        m_TRiseRBTmp;      
    double        m_TRiseACTmp;      
    #endif
    
  };

// ===========================================================================

#define IMPLEMENT_FLWEQN(Md, Group, Name, Version, Cat, ShortDesc, Desc) \
  IMPLEMENT_TAGOBJ(Md, Group, Name, Version, "", "", Cat, ShortDesc, Desc);
#define IMPLEMENT_FLWEQNEDT(Md, Group, Name, Version, Cat, Edt, ShortDesc, Desc) \
  IMPLEMENT_TAGOBJEDT(Md, Group, Name, Version, "", "", Cat, Edt, ShortDesc, Desc);
#define IMPLEMENT_FLWEQNHIDDEN(Md, Group, Name, Version, Cat, ShortDesc, Desc) \
  IMPLEMENT_TAGOBJSLCT(Md, Group, Name, Version, "", "", Cat, ShortDesc, Desc, 0x00000000);

// ===========================================================================

extern DllImportExport CFlwEqnGrp NullFlwGroup;
extern DllImportExport CFlwEqnGrp Xfer2AreaGroup;

//==========================================================================
//
//
//
//==========================================================================

//// Press Drop in kPa
//double PressDropLength(double Diam, double Vel, double Dens, double Length, double dAge_Fac, double dPowr_Fac)
//  {
//  // VelocityHead     = Pow(Vel,2.0*dPowr_Fac)/(2.0 * g);
//  // HeadLoss mLiquid = (F_Fac * Length / Diam) * VelocityHead * dMagn_Fac * AFac;
//  // DP kPa           =  HeadLoss * Dens * g;
//
//  Vel=Max(1.0e-3, fabs(Vel));
//  double F_Fac=FrictionFact(Diam, Vel, Dens);
//  double AFac = 1.0 + 0.03 * dAge_Fac;
//  // Crane Eqn 1-4
//  return DP = 0.001 * (F_Fac * Length / Diam) * Pow(Vel,2.0*dPowr_Fac)/2.0 * Dens * dMagn_Fac * AFac;
//  }

// ===========================================================================

inline double Area2Diam(double A) { return Sqrt(A*4.0/PI); };
inline double Diam2Area(double D) { return D*D*PI/4.0; };

inline double  Reynolds(double Diam, double Vel, double Dens, double LVisc) { return Diam*Vel*Dens/LVisc; };
//static double  RelRough(double Diam) { return Roughness/Diam; };
//static double  FrictionFact(double Diam, double Vel, double Dens);

template <class TYPE> void Swap(TYPE &A, TYPE &B)
  {
  TYPE T(A);
  A=B;
  B=T;
  }

class DllImportExport CFrictionFactor
  {
  protected:
    int     m_Cnt;
    double  m_ReLo, m_ReHi;
    double  m_FacLo, m_FacHi;

  public:

    CFrictionFactor()
      {
      m_Cnt=0;
      m_ReLo=0;
      m_ReHi=0;
      m_FacLo=0;
      m_FacHi=0;
      };

//    inline double Value(double Diam, double Roughness, double Vel, double Dens, double LVisc)
    double Value(double Diam, double Roughness, double Vel, double Dens, double LVisc);
    inline double Turbulent(double Diam, double Roughness, double Dens, double LVisc)
      {
      return Value(Diam, Roughness, 1e3, Dens, LVisc);
      };
  };

// will be reunitialised on reload of FlwLib
extern DllImportExport double PressDropKFactMult;
inline double PressDropKFact(double Vel, double Dens, double KFact, double dPowr_Fac=1.0)
  {
  // VelocityHead      = Pow(Vel,2.0)/(2.0 * g);
  // HeadLoss mLiquid  = K * VelocityHead;
  // DP (Pa)           = HeadLoss * Dens * g;

  return PressDropKFactMult*0.0005*KFact * Pow(Vel,2.0*dPowr_Fac) * Dens;
  }

// ===========================================================================

//inline CVLEBase * CFlwEqn::VLE()  { return m_pEqnPtr->m_pVLE; };
inline double CFlwEqn::LScale()  { return m_pEqnPtr->LScale(); };
inline double CFlwEqn::ZScale()  { return m_pEqnPtr->ZScale(); };

// ===========================================================================

#undef DllImportExport

#endif
