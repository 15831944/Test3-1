//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __MG_TWOPH_H
#define  __MG_TWOPH_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#define BASIC1
#ifndef __MODELS_H
  #include "models.h"
#endif

#ifndef __SM_OIL_H
  #include "sm_oil.h"
#endif

#ifndef __SP_CONT_H
  #include "sp_cont.h"
#endif

#ifdef __MG_TWOPH_CPP
  #define DllImportExport DllExport
#else
  #define DllImportExport
#endif

#define FixSeg0   0

const int MaxTPPSections = 100;

const word xidNSections       = 5000;
const word xidNGSegments      = 5001;
const word xidNLSegments      = 5002;
const word xidTPPSetLineP     = 5005;
const word xidTPPSetLineSegs  = 5006;
const word xidJoinStart       = 5100;
const word xidJoinEnd         = 5200;
const word xidJoinPos         = 5300;
const word xidTPPTemp         = 6000;
const word xidLSegFillFrac    = 6500;
const word xidLSegLen         = 7000;
const word xidGSegLen         = 7500;

// --------------------------------------------------------------------------

const byte TPR_Stratified = 0;
const byte TPR_Transition = 1;
const byte TPR_Slug       = 2;
const byte TPR_Bubble     = 3;
const byte TPR_Spray      = 4;
const byte TPR_Annular    = 5;
const byte TPR_Wave       = 6;
const byte TPR_Plug       = 7;
const byte TPR_Dispersed  = 8;

const byte HFR_Segregated=0;
const byte HFR_Transition=1;
const byte HFR_Intermittent=2;
const byte HFR_Distributed=3;

const double SegSplitFactor = 1.5;
const double SegMergeFactor = 0.5;
const double VelDampC = 0.75;
const double VelDampE = 0.5*(1.0-VelDampC);

DEFINE_TAGOBJ(TwoPhasePipe);

// --------------------------------------------------------------------------

class TPPSection;
class TPPJoin;
class TPPGSegment;
class TPPLSegment;


template<class TYPE, class ARGTYPE>
class CSCDPtrArray : public CTypedPtrArray <CPtrArray, ARGTYPE>
  {
  public:
    VOID SetSize(int nNewSize, int nGrowBy = -1)
      {
      int s=GetSize();
      for ( ; s>nNewSize; s--)
        delete GetAt(s-1);
      CPtrArray::SetSize(nNewSize, nGrowBy);
      for ( ; s<nNewSize; s++)
        SetAt(s, new TYPE);
      }
    ~CSCDPtrArray()
      {
      SetSize(0);
      }
 };

//typedef CSCDPtrArray <TPPGSegment, TPPGSegment*>  TPPGSegArray;

typedef CArray       <TPPSection, TPPSection&>    TPPSectArray;
typedef CSCDPtrArray <TPPJoin, TPPJoin*>          TPPJoinArray;
typedef CSCDPtrArray <TPPGSegment, TPPGSegment*>  TPPGSegArray;
typedef CSCDPtrArray <TPPLSegment, TPPLSegment*>  TPPLSegArray;

// --------------------------------------------------------------------------

const double DefLiqDens=700.0;
const double DefGasDens=100.0;
const double DefNormGasDens=1.0;

// --------------------------------------------------------------------------

class TPPGasProps
  {
  public:
    double dGasDens;
    double dNormGasDens;
    double dGasCp;
    double dGasSum;

    TPPGasProps();
    void InitSum(double Gas=0.0);
    void Sum(TPPGasProps & Other, double OtherGMass);
  };

// --------------------------------------------------------------------------

class TPPLiqProps
  {
  public:
    double dLiqDens;
    double dLiqCp;
    double dLiqSum;

    TPPLiqProps();
    void InitSum(double Liq=0.0);
    void Sum(TPPLiqProps & Other, double OtherLMass);
  };

// --------------------------------------------------------------------------

class TPPProps : public TPPGasProps, public TPPLiqProps
  {
  public:
    TPPProps();
    void InitSum(double Gas=0.0, double Liq=0.0);
    void Sum(TPPProps & Other, double OtherGMass, double OtherLMass);
    TPPProps & operator=(TPPGasProps & Other);
    TPPProps & operator=(TPPLiqProps & Other);
    TPPGasProps & GP() { return *this; };
    TPPLiqProps & LP() { return *this; };
  };

// --------------------------------------------------------------------------

class TPPSection
  {
  public:
    int    iMyIndex;

    double dStart;
    double dEnd;
    double dAngle;

    double dRelDatum;         // Relative Datum

    long   nSlugs;
    double dTotLiqHoldup;

    TPPSection();
    ~TPPSection();

  };

inline void AFXAPI ConstructElements(TPPSection *pInfo, int nCount)
  { for ( int i = 0; i < nCount; i++, pInfo++ ) (pInfo)->TPPSection::TPPSection(); };
inline void AFXAPI DestructElements(TPPSection *pInfo, int nCount)
  { for ( int i = 0; i < nCount; i++, pInfo++ ) (pInfo)->TPPSection::~TPPSection(); };
inline void AFXAPI DumpElements(CDumpContext& dc, TPPSection *pElements, int nCount) 
  { };

// --------------------------------------------------------------------------

class TPPJoin
  {
  public:
    double dMeasP;
    double dMeasT;
    double dMeasGasVel;
    double dMeasLiqVel;
    byte   bMeasHorFlowRegime;
    byte   bMeasFlowRegime;

    TPPJoin();
    TPPJoin(TPPJoin & Other);
    TPPJoin & operator=(TPPJoin & Other);
    ~TPPJoin();
  };

inline void AFXAPI ConstructElements(TPPJoin **pInfo, int nCount)
  { for ( int i = 0; i < nCount; i++, pInfo++ ) *pInfo= new TPPJoin; };
inline void AFXAPI DestructElements(TPPJoin **pInfo, int nCount)
  { for ( int i = 0; i < nCount; i++, pInfo++ ) delete *pInfo; };
inline void AFXAPI DumpElements(CDumpContext& dc, TPPJoin **pElements, int nCount) { };

// --------------------------------------------------------------------------

class TPPFlowStuff
  {
  public:
    byte            bHorFlowRegime;    // DP Calc FLow regime
    byte            bFlowRegime;       // Flow Regime
    double          dQmEst;            // Calc Flow due to DP
    double          dQmG;              // Calc Flow due to DP
    double          dQmL;              // Calc Flow due to DP
    double          dQvG;              // Gas Vol Flow due to DP
    double          dQvL;              // Liq Vol FLow due to DP
    double          dVapFrac;          // Mean vapour fraction
    double          sLiqVolFrac;       // Slip Liq Volume Fraction

    double          dDarcyCorrection;

    double          dSupGasVel;        // Superficial Gas Velocity
    double          dSupLiqVel;        // Superficial Liquid Velocity
    double          dTotalVel;         // Total Velocity

    double          dDPq, dDPqDq, dDPmB, dDPmQ, dDPmDq, dDPApplied, dDPz;
    double          dVelStart;         //

    double          dGasVelo;
    double          dLiqVelo;

    TPPFlowStuff();
    TPPFlowStuff(double Position, double Velocity, double Mass, double Temperature, TPPProps &Properties);
    TPPFlowStuff(TPPFlowStuff & Other);
    TPPFlowStuff & operator=(TPPFlowStuff & Other);
    ~TPPFlowStuff();

    void            BeggsAndBrill(TwoPhasePipe & Pipe, TPPProps &Props, double Len, double Angle, double Q, double &DPq, double &DPz, flag StartOfStep);
    void            Coker(TwoPhasePipe & Pipe, TPPProps &Props, double Len, double Angle, double Q, double &DPq, double &DPz, flag StartOfStep);
    void            EstimateDP1(TwoPhasePipe & Pipe, TPPProps &Props, double Len, double Angle, double Mass, double Q, double &DPq, double &DPz, double &DPmB, double &DPmQ, flag StartOfStep);
    void            EstimateDP(TwoPhasePipe & Pipe, TPPProps &Props, double Len, double Angle, double Mass, double QIn);//, double dTime);
    void            EstimateFlows(TwoPhasePipe & Pipe, TPPProps &Props, double Len, double Angle, double Mass, double DP);//, double dTime);
    void            SetFlwInfo(TwoPhasePipe & Pipe, TPPProps &Props, double Q);

    void            StartStep();

  private:

    inline double   sHorLiqFlowFrac(double a, double b, double c, double nVolFrac, double Nfr);
    inline double   sElevFactor(double e, double f, double g, double h, double nVolFrac, double Nlv, double Nfr, double angle);
    inline double   DarcyDP(double Vel, double Rho, double Len, double Diam, double Rough, double Viscosity);
  };

// --------------------------------------------------------------------------

class TPPSegBase
  {
  public:
    Integrator *iPos;
    dword       ValidFlags;
    State       dPosition;
    double      dVelocity;

    Integrator *iTemp;
    State       dTemp;
    double      dTempDeriv;

    Integrator *iMass;
    double      dMass;
    double      dMassDeriv;
    double      dMassTest;

    SpConduit  *pImg;

    int         iAge;

    TPPSegBase();
    TPPSegBase(double Position, double Velocity, double Mass, double Temperature, SpConduit * Img);
    ~TPPSegBase();

    double Position() { return dPosition; };
    double Velocity() { return dVelocity; };
    void SetVelocity(double V) { dVelocity=V; };
    void SetPosition(double P) { dPosition=P; };

    double Temp() { return dTemp;};
    void   SetTemp(double T) { dTemp=T;};
    void   SetTempDeriv(double T) { dTempDeriv=T;};

  };

// --------------------------------------------------------------------------

class TPPGSegment : public TPPSegBase, public TPPFlowStuff
  {
  public:
    TPPGasProps GProps;
    TPPProps    MxProps;

    double      dPress;
    double      dPressGrad;
    double      dLiqMass;

    TPPGSegment();
    TPPGSegment(double Position, double Velocity, double Mass, double Temperature, TPPGasProps &Properties, SpConduit * Img);
  };

inline void AFXAPI ConstructElements(TPPGSegment **pInfo, int nCount)
  { for ( int i = 0; i < nCount; i++, pInfo++ ) *pInfo= new TPPGSegment; };
inline void AFXAPI DestructElements(TPPGSegment **pInfo, int nCount) 
  { for ( int i = 0; i < nCount; i++, pInfo++ ) delete *pInfo; };
inline void AFXAPI DumpElements(CDumpContext& dc, TPPGSegment **pElements, int nCount) { };

// --------------------------------------------------------------------------

class TPPLSegment : public TPPSegBase, public TPPFlowStuff
  {
  public:
    TPPLiqProps LProps;
    TPPProps    MxProps;

    double      dGasMass;

    TPPLSegment();
    TPPLSegment(double Position, double Velocity, double Mass, double Temperature, TPPLiqProps &Properties, SpConduit * Img);
  };

inline void AFXAPI ConstructElements(TPPLSegment **pInfo, int nCount) 
  { for ( int i = 0; i < nCount; i++, pInfo++ ) *pInfo= new TPPLSegment; };
inline void AFXAPI DestructElements(TPPLSegment **pInfo, int nCount)
  { for ( int i = 0; i < nCount; i++, pInfo++ ) delete *pInfo; };
inline void AFXAPI DumpElements(CDumpContext& dc, TPPLSegment **pElements, int nCount)
  { };

// --------------------------------------------------------------------------

class TwoPhaseInitSlug
  {
  public:
    double dPosition;
    double dVolume;
  };

typedef CArray <TwoPhaseInitSlug, TwoPhaseInitSlug&> InitSlugArray;

// --------------------------------------------------------------------------

class TwoPhasePipe: public Pipe
  {
  public:
    flag            bSimple;
    flag            fFlash;

    double          dHTC;                      // Heat Transfer Coeff
    double          dSeaTemp;                  // Temperature of the external fluid
    double          dLength;                   // Pipe Length
    double          dDiam;                     // Pipe Diameter
    double          dODiam;                    // Pipe Outside Diameter
    double          dArea;                     // Calcd Pipe Area
    double          dVolume;                   // Calcd Pipe Volume
    double          dRough;                    // Pipe roughness - AKS
    double          dAngleS;                   // Pipe Angle of elevation, between -90° and 90°C - AKS
    double          dAngle;                    // Pipe Angle of elevation, between -90° and 90°C - AKS
    double          dAngleE;                   // Pipe Angle of elevation, between -90° and 90°C - AKS
    double          dLengthS;                  // Length of Start Segment (if valid)
    double          dLengthE;                  // Length of End Segment (if valid)
    double          dMaxSlugVolume;
    double          dTotLiqHoldup;
    long            nMaxSlugs;
    long            nSlugs;

    double          dSlugResInc;
    double          dSlugAppears;
    double          dSlugMinLen;
    double          dSlugReduction;

    double          dRqdLinePress;
    double          dRqdLineTemp;
    double          dRqdVapFrac;

    bool            fDoSetLinePress;
    bool            fDoSetLineSlugs;

    TPPSectArray    Sect;
    TPPJoinArray    Join;
    TPPGSegArray    GasSeg;
    TPPLSegArray    LiqSeg;
    TPPFlowStuff    IOFlw[2];

    double          dLineRes;

    TPPProps        FdProps;

    double          dQmGEntry;
    double          dQmLEntry;
    double          dCpGEntry;
    double          dCpLEntry;
    double          dTempEntry;
    double          dQmGExit;
    double          dQmLExit;
    double          dCpGExit;
    double          dCpLExit;
    double          dTempExit;

    SpConduit       FlowImg;

    InitSlugArray   InitSlug;

    TwoPhasePipe(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual         ~TwoPhasePipe();
    void            SetNSections(int NSects);
    void            SetNGSegments(int NSegs);
    void            SetNLSegments(int NSegs);
    int             NSections() { return Sect.GetSize(); };
    int             NGSegments() { return GasSeg.GetSize(); };
    int             NLSegments() { return LiqSeg.GetSize(); };
    int             NFullGSegs() { return GasSeg.GetSize()-1; };
    int             NFullLSegs() { return LiqSeg.GetSize()-1; };
    int             NJoins() { return Join.GetSize(); };
    int             LBJoin() { return 0; };
    int             UBJoin() { return Join.GetUpperBound(); };
    int             LBSect() { return 0; };
    int             UBSect() { return Sect.GetUpperBound(); };
    int             LBSeg() { return 0; };
    int             UBGSeg() { return GasSeg.GetUpperBound(); };
    int             UBLSeg() { return LiqSeg.GetUpperBound(); };

    void            SegsChanged() { TaggedObject::StructureChanged(this); };

    virtual void    BuildDataDefn(DataDefnBlk &DDB);
    virtual flag    DataXchg(DataChangeBlk & DCB);
    virtual flag    ValidateData(ValidateDataBlk & VDB);
    virtual flag    GetModelAction(Strng & Tag, MdlActionArray & Acts);
    virtual flag    SetModelAction(Strng & Tag, MdlAction & Act);
    virtual flag    GetOtherData(FilingControlBlock &FCB);
    virtual flag    PutOtherData(FilingControlBlock &FCB);
    virtual void    PostConnect(int IONo);
    virtual void    StartSolution();
    virtual void    StartStep();
    virtual void    ConfigureJoins();
    virtual void    EvalJoinPressures();
    virtual flag    EvalFlowEquations(CSpPropInfo *pProps, int IONo, int FE);
    virtual void    EvalProducts();
    virtual void    EvalDerivs();
    virtual void    ConvergeStates();
    virtual void    ApplyDerivs(double dTime, flag DoDbg);
    virtual void    EvalDiscrete();

    double          Area() { return dArea; };
    double          Volume() { return dVolume; };

    void            SetLinePress();
    void            AddStdSegment(double Pos, double Len, double Press, double Temp, double VapFrac, TPPProps &Properties, SpConduit * Img);
    void            SetLineSegs(flag DoSlug);
    void            AddSweptMass(flag AtStart, double dTime, double &GasMass, double &LiqMass, double &MixTemp);
    void            LiqMassInSegs(double SegS, double SegE, double &LiqMass, double &MixTemp, TPPLiqProps & LProps);
    void            CalcPressures(double dTime);
    void            CreateSlug(double SlugPos, double SlugVol);
    int             RemoveSlug(int iSlug);
    int             CombineLSegs(int iSeg0, int iSeg1);
    void            EvaluateSegment(double dTime);
    double          GVelocity(double Pos);
    double          LVelocity(double Pos);
    double          Pressure(double Pos);
    double          Temperature(double Pos);

    int             GSegIndex(double Pos);
    int             LSegIndex(double Pos);
    int             SectIndex(double Pos);
    int             JoinIndex(double Pos);
    int             LastFullGSeg() { return GasSeg.GetUpperBound()-1;};
    int             LastFullLSeg() { return LiqSeg.GetUpperBound()-1;};
    int             LastGSeg() { return GasSeg.GetUpperBound();};
    int             LastLSeg() { return LiqSeg.GetUpperBound();};
    TPPGSegment     &GSeg(int i) { return *GasSeg[i]; };
    TPPLSegment     &LSeg(int i) { return *LiqSeg[i]; };

    double          Length() { return dLength; };
    double          MeanSegLength() { return 0.5*dLength/NSections(); };

    double          SectionLength(int i) { return Sect[i].dEnd-Sect[i].dStart;};
    double          SectionVolume(int i) { return SectionLength(i)*dArea;};
    double          SectionStart(int i) { return Sect[i].dStart;};
    double          SectionEnd(int i) { return Sect[i].dEnd;};
    double          SectionMid(int i) { return 0.5*(Sect[i].dEnd+Sect[i].dStart);};

    double          JoinStart(int i) { return (i==0) ? Sect[i].dStart : SectionMid(i); };
    double          JoinEnd(int i) { return (i==Join.GetUpperBound()) ? Sect[i+1].dEnd : SectionMid(i+1); };
    double          JoinPos(int i) { return Sect[i].dEnd; };
    double          JoinLength(int i) { return JoinEnd(i)-JoinStart(i);};

    void            AddMass(double &GMass0, double &Temp0, TPPGasProps & Props0, double GMass1, double Temp1, TPPGasProps & Props1);
    void            AddMass(double &LMass0, double &Temp0, TPPLiqProps & Props0, double LMass1, double Temp1, TPPLiqProps & Props1);
    void            AddMass(double &GMass0, double &LMass0, double &Temp0, TPPProps & Props0, double GMass1, double LMass1, double Temp1, TPPProps & Props1);

    double          GSegStart(int i)   { return GasSeg[i]->Position();};
    double          GSegEnd(int i)     { return GasSeg[i+1]->Position(); };
    double          GSegMid(int i)     { return 0.5*(GasSeg[i]->Position()+GasSeg[i+1]->Position()); };
    double          GSegLength(int i)  { return (GasSeg[i+1]->Position()-GasSeg[i]->Position()); };
    double          GSegVolume(int i)  { return GSegLength(i)*dArea;};
    void            SetGSegStart(int i, double P) { GasSeg[i]->SetPosition(P);};
    void            SetGSegEnd(int i, double P)   { GasSeg[i+1]->SetPosition(P);};
    double          GSegStartVel(int i)   { return GasSeg[i]->Velocity();};
    double          GSegEndVel(int i)     { return GasSeg[i+1]->Velocity(); };

    double          LSegStart(int i)   { return LiqSeg[i]->Position();};
    double          LSegEnd(int i)     { return LiqSeg[i+1]->Position(); };
    double          LSegMid(int i)     { return 0.5*(LiqSeg[i]->Position()+LiqSeg[i+1]->Position()); };
    double          LSegLength(int i)  { return (LiqSeg[i+1]->Position()-LiqSeg[i]->Position()); };
    double          LSegVolume(int i)  { return LSegLength(i)*dArea;};
    void            SetLSegStart(int i, double P) { LiqSeg[i]->SetPosition(P);};
    void            SetLSegEnd(int i, double P)   { LiqSeg[i+1]->SetPosition(P);};
    double          LSegStartVel(int i)   { return LiqSeg[i]->Velocity();};
    double          LSegEndVel(int i)     { return LiqSeg[i+1]->Velocity(); };

    double          FillFraction(int i ) { return (LiqSeg[i]->dMass/GTZ(LiqSeg[i]->LProps.dLiqDens))/GTZ(LSegVolume(i)); };
    double          IsSlug(int i ) { return FillFraction(i)>0.90; };

    double          CalcPortion(double SrcS, double SrcE, double DstS, double DstE)
      {
      double PortionS=Max(DstS, SrcS);
      double PortionE=Min(DstE, SrcE);
      double Portion=GEZ(PortionE-PortionS)/GTZ(SrcE-SrcS);

      return Portion;

      }

    void           DbgDump(double dTime);

  private:

  };

#undef DllImportExport

#endif

