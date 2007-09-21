//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __MULTIFB_H
#define  __MULTIFB_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __FLWNODE_H
  #include "flwnode.h"
#endif
#ifndef __VLEBASE_H
  #include "vlebase.h"
#endif

#include <typeinfo.h>

#if defined(__MULTIFB_CPP)
  #define DllImportExport DllExport
#elif !defined(MdlBase)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

#define WITHMFBDEDT   01

XID xidMFBOn            = FlwBlkXID(1);
XID xidMFBBuildIt       = FlwBlkXID(2);
XID xidMFBDiam          = FlwBlkXID(3);
XID xidPFMethod         = FlwBlkXID(4);

// ===========================================================================
//
//
//
// ===========================================================================

// Void Fraction Methods
const int VM_Butterworth          = 0;
const int VM_Premoli              = 1;

// Void Fraction Butterworth Factors
const int VMB_Fauske              = 0;
const int VMB_Zivi                = 1;
const int VMB_Baroczy             = 2;
const int VMB_Wallis              = 3;
const int VMB_LockhartMartinelli  = 4;
const int VMB_Thom                = 5;
const int VMB_Moody               = 6;
const int VMB_Homogeneous         = 7;

// Density Multiplier Methods
const int DM_Friedel              = 0;
const int DM_Beattie              = 1;

extern DllImportExport CFlwEqnGrp MFBPipeGroup;
extern DllImportExport CFlwEqnGrp MFBJoinGroup;
extern DllImportExport CFlwEqnGrp MFBEnExGroup;

class MFCommonBlk
  {
  friend class CMultiFlwBlk;
  friend class CMultiFlwBlkEdt;

  protected:
    double          dMagn_Fac;
    double          dPowr_Fac;
    double          dAge_Fac;
    double          dRoughness;
    //double          dLViscosity;
    //double          dGViscosity;
    double          dIntDiamMult;

    byte            iVoidMeth;
    byte            iVMButterFact;
    byte            iDensMultMeth;

    //double          dRoughness;
//    DualDbl         LVisc;
//    DualDbl         GVisc;
//    DualDbl         GDens;
//    DualDbl         LDens;
//    //DualDbl         TPDens;
//    DualDbl         STens;

    //double          ;

    double          LVisc, LViscI, LViscO;
    double          GVisc, GViscI, GViscO;
    double          GDens, GDensI, GDensO;
    double          LDens;
    //DualDbl         TPDens;
    double          STens, STensI, STensO;


    flag            fDoXfer;
    flag            fAppRhoH;
    flag            fFindRhoH;
    flag            fPressOK;

    CVLEBase        *pVLE;

    // SetPhysInfo
    double          DiamI, AreaI;
    double          DiamE, AreaE;
    double          DiamO, AreaO;
    double          DiamMean, AreaMean;
    double          DZ;
    double          Length;
    double          KFact;
    // SetFlowInfo  
    double          Tk; 
    double          MassFlow; 
    double          VelMix;
    double          G; 
    double          X;
    flag            VFracOK;
    double          VFrac, LFrac; // SFrac=1-VFrac-LFrac
    //double          GDens, LDens;
    double          TPDens;
    //double          SurfaceTension;
    //double          GVisc, LVisc;
    double          ReLiq, ReGas, WeLiq;
    // SetFricFact  
    double          LFricFact, GFricFact, FLog;
    //SetDensityMul t
    double          VoidFrac; // Correlation
    double          Slip;  // 
    double          Phi;   // Multiplier
    //double          FLC, FGC, FL, FG, FLog;
    //double          LFricFact, GFricFact, FLog;
    double          DPFricLiq1;
    double          DPAccl1, DPStat1, DPFric1;
    double          DPAccl2, DPStat2, DPFric2;
    double          DPAccl, DPStat, DPFric;
    double          DPScale;
    double          dOnePhPart; // Correlation
    //double          dPrevDiam;
                                                                       
    double          TIn;
    double          HIn;
    double          PIn, POut;
    double          VfIn, VfOut;
    double          dOnePhDPQ, dOnePhDPZ;

  public:
    MFCommonBlk(CVLEBase *VLE);

    double  Reynolds(double Diam, double Vel, double Dens) { return Diam*Vel*Dens/LVisc; };
    double  RelRough(double Diam) { return dRoughness/Diam; };
    double  FrictionFact(double Diam, double Vel, double Dens);
    double  FrictionFactTurbulent(double Diam, double Dens) { return FrictionFact(Diam, 1e3, Dens); };
    double  PressDropLength(double Diam, double Vel, double Dens, double Length);
    double  PressDropKFact(double Vel, double Dens, double KFact);
            
            
    flag    TestPressureOK(double &P0, double &P1);
    flag    DoSatPLiqAtStart(flag AtEntry, CFlwBlkBase & FE, 
                              double Regulation, SpConduit &Cd, 
                              double K, double DI, double DE, double DO);//, double D2);
    flag    DoSatPVapAtStart(flag AtEntry, CFlwBlkBase & FE, 
                              double Regulation, SpConduit &Cd, 
                              double K, double DI, double DE, double DO);//, double D2);
    flag    DoFlash(flag AtEntry, CFlwBlkBase & FE, 
                    double Regulation, SpConduit &Cd, 
                    double K, double DI, double DE, double DO,//double D2,
                    double OnePhDPQ, double OnePhDPZ);
            
    //void    SetPrevDiam(double D1) { dPrevDiam=D1; };
    void    SetPhysInfo(double DI, double DE, double DO, double KMax, double Len, double Rise, double Scale);
    flag    SetFlowInfo(SpConduit & Cd, double RqdPress);
    void    SetFricFact(); 
    void    SetVoidFrac();
    void    SetDensityMult();
    void    SetProperties(SpConduit & Cd, double RqdP, double MaxT, flag DoDbgHdr);
            
    void    SetTwoPhDPs1(SpConduit & Cd, double P1);
    void    SetTwoPhDPs2(SpConduit & Cd, double P2, double MaxTemp);
    void    SetTwoPhDPs();
            
    double  DPa()          { return DPAccl; };
    double  DPz()          { return DPStat; };
    double  DPq()          { return DPFric; };
            
    double  VapFrac()      { return VFrac; };
    double  LiqFrac()      { return LFrac; };
    double  SolFrac()      { return 1-VFrac-LFrac; };
            
    double  MixVel()       { return VelMix; };
    double  SlipRatio()    { return Slip; };
    double  DensMult()     { return Phi; };
    double  VoidFraction() { return VoidFrac; };
    double  OnePhPart()    { return dOnePhPart; };
  };

// ---------------------------------------------------------------------------

class MFB_Eqn : public CFlwEqn
  {
  public:
    MFCommonBlk      *pMFCB;
    Strng            m_sIdStr;

    MFB_Eqn(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
      CFlwEqn(pClass_, pTag, pAttach, eAttach)
        {
        pMFCB=NULL;
        };

    virtual ~MFB_Eqn() {};
    virtual void    BuildDataDefn(DataDefnBlk & DDB);
    virtual flag    DataXchg(DataChangeBlk & DCB);

    virtual double  GetFricFact() { return dNAN; };
    virtual double  GetSlipRatio() { return pMFCB ? pMFCB->SlipRatio() : dNAN; };
    virtual double  GetVoidFraction() { return pMFCB ? pMFCB->VoidFraction() : dNAN; };
    virtual double  GetOnePhPart() { return pMFCB ? pMFCB->OnePhPart() : 1.0; };

  };

// ===========================================================================
//
//
//
// ===========================================================================

union CFBCount
  {
    dword         Msk;
    int           Cnt[4];
    long Total() { return Cnt[0]+Cnt[1]+Cnt[2]+Cnt[3]; };
  };

struct CFBPtr
  {
  FlwBlk * pFB;
  int      iBlk;
  int      iFE;
  int      iSgn;
  
  CFBPtr()
    {
    pFB=NULL;
    iBlk=iFE=iSgn=0;
    };
  };

#if WITHMFBDEDT
DEFINE_TAGOBJEDT(CMultiFlwBlk);
#else
DEFINE_TAGOBJ(CMultiFlwBlk);
#endif

class DllImportExport CMultiFlwBlk : public TaggedObject
  {
  friend class CMultiFlwBlkEdt;
  protected:
    flag            fOn, fOnRqd; // Buffered
    int             iIoIn, iIoOut;
    CFBCount        FBCnt;
    int             FBEdtBlkId[4];
    CArray <CFBPtr, CFBPtr&> FBA;
    MFCommonBlk     Common;

  public:
    CMultiFlwBlk(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);
    CMultiFlwBlk(pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach, int IoIn, int IoOut, CVLEBase * Vle);

    virtual ~CMultiFlwBlk();

    flag            On() { return fOn; };
    void            SetOn(flag On);
    void            SetOnRqd(flag On);
                    
    virtual void    AssignFlowGroups();
    void            GetFBInfo();
    void            SetFBs();
    void            ChgFBs(int Blk, int At, int Inc);
    virtual void    BuildDataDefn(DataDefnBlk & DDB);

    virtual flag    DataXchg(DataChangeBlk & DCB);// {return TaggedObject::DataXchg(DCB);};
    virtual flag    ValidateData(ValidateDataBlk & VDB);

    virtual void    SetPhysicalData();
    virtual void    SetDatums(int Pass, CFlwNodeIndexList & List, int IOIn, double &RiseRqd, double &DatumChg);
    virtual void    EvalJoinPressures(flag DoXfer, flag AppRhoH);
  };


// ===========================================================================
//
//
//
// ===========================================================================

#if WITHMFBDEDT

struct MFBColDesc
  {
  char * sTag;
  int    iId;
  flag   bOn;
  int    iWide;
  UCHAR  iJust;

  CnvAttribute   Cnv;
  FmtAttribute   Fmt;

  MFBColDesc()
    {
    sTag=NULL;
    iId=-1;
    bOn=false;
    iWide=8;
    }
  MFBColDesc(char * D, int Id, flag On, int Wide, UCHAR Just, CCnvIndex DC, char * CnvTxt, int NDec) :
    Cnv(DC,     CnvTxt),
    Fmt("", 0, (byte)NDec, 'f')
    {
    sTag=D;
    iId=Id;
    bOn=On;
    iWide=Wide;
    iJust=Just;
    }
  };

_FWDDEF(CMultiFlwBlkEdt);
class DllImportExport CMultiFlwBlkEdt : public FxdEdtBookRef
  {
  public :
    CMultiFlwBlk   &rMFB;
    int            iPg1;

    CnvAttribute   ViscCnv, DensCnv, STensCnv, RoughCnv;
    FmtAttribute   ViscFmt, DensFmt, STensFmt, RoughFmt;

    CArray <MFBColDesc, MFBColDesc&> CDs;

    int DistFromPgNo(int Pg);

    CMultiFlwBlkEdt(pFxdEdtView pView_, pCMultiFlwBlk pChgO_);//, rStrng Tag, rStrng Name);
    virtual ~CMultiFlwBlkEdt();

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
    virtual flag SelectValues(char* DataFile, char* Table, char** Columns, CStringArray &RetValues);
    virtual flag DumpPDS(char * Fn);

  };
#endif

// ===========================================================================
//
//
//
//===========================================================================

#undef DllImportExport

#endif

