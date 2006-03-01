//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __SCREEN_H
#define  __SCREEN_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#define SIZEDST1
#include "models.h"
#ifndef __2D_FN_H
  #include "2d_fn.h"
#endif

#ifdef __SCREEN_CPP
  #define DllImportExport DllExport
#elif !defined(Separ1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//===========================================================================

DEFINE_TAGOBJ(ScreenBlk);
class ScreenBlk : public TaggedObject
  {
  protected:
    flag           bTrackStatus;
    flag           wet;
    byte           fMode;      // Screen model to use
    flag           bKneeCorr;
    byte           iMethod, iFineMeth, iKM, FE;
    double         MinFine;    // Minimum percentage fines to coarse fraction
    double         m_CutSize, ICut;
    double         Cap, TotCap;
    double         QmSIn;      // Total solids flow in the feed stream
    double         QmLIn;      // Liquids flow in the feed stream
    double         OSDens;     // Achieved Underflow Density
    double         USDens;     // Achieved Overflow Density(Clarity)
    double         OSSplit;    // Achieved Solids Split
    double         ht,hmm;     // Cut point
    double         ht_chk;     // Screen aperture at which constants have been evaluated
    double         FdSol;      // total feed solids flow
    double         OSSol;      // total solids flow to the over size
    double         OSLiq;      // total liquid flow to the over size
    double         USSol;      // total solids flow to the under size
    double         USLiq;      // total liquid flow to the under size
    double         m_OSMoist;    // % moisture in over size
    double         m_OSMoistOp;  // % moisture in over size
    double         m_OSMoistKnee;
    double         m_OSMoistAtCap;
    double         Area, SArea;// Total and Individual Screening area
    double         MaxSize;    // Maximum particle size in incoming feed
    double         m_d50Op;        // Size fraction with 50% chance of passing to undersize

    double         m_CutSizeAtCap; // d50 When total tons reaches TotCap
    double         m_CutSizeKnee;// Cap at which d50 starts changing

    double         m_Alpha;    // Alpha parameter for Whiten method
    double         m_AlphaOp; 
    double         m_AlphaKnee; 
    double         m_AlphaAtCap; 
    double         Beta;       // Beta parameter for Whiten BetaStar method
    double         BetaStar;   // Calculated Beta* parameter for Whiten BetaStar method
    double         m_Rf;       //             for Whiten method
    double         m_RfOp; 
    double         m_RfKnee; 
    double         m_RfAtCap; 
    double         EWash;      // Washing efficiency
    double         FineTop, ReqdPerc, FineTons, FineOS, FinetoCoarse;  // Constants for fine entrainment
    double         DeckFeed;

    double         A;          // Constants to calculate screening
    double         B;
    double         C;
    double         D;
    double         E;
    double         F;
    double         G;

    double         m_EfficiencyAtSize; // Result % Size in Underflow / % Size in Feed
    double         m_EfficiencySize;   // Parameter Size to calculate efficiency

    CDArray        OTemp, Feed, UTemp, YTemp, Fine;
    SpConduit      QFine, QTemp;

    SzPartCrv1     PartCrv, PartCrv2;

  public:
    ScreenBlk(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~ScreenBlk (){};

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    void           Setup_Constants();
    double         Distribution(double topsize, double Lowersize, SpConduit &QFd);
    double         HalfSize(SpConduit &QFd);
    double         OverSize(SpConduit &QFd);
    double         NearSize(SpConduit &QFd);
    double         Intersection(SpConduit &QUs, SpConduit &QOs);
    double         CalcBetaStar();
    void           Separate(SpConduit &QFd, SpConduit &QOs, SpConduit &QUs, int s, double NumS);

  private:
    DEFINE_CI(ScreenBlk, TaggedObject, 4);
  };


const byte  Eff_Calc  = 0;
const byte  Eff_Set   = 1;


DEFINE_TAGOBJ(Screen);
class Screen : public MdlNode
  {
  public:
    byte           iType, nDecks;
    double         QmWIn;          // Liquids flow in the wash water stream
    double         NumS;

    ScreenBlk      TopDeck,
                   BottomDeck;

    SpConduit      QFd, QMs;
    C2DFn          WashEffCurve;   // Wash efficiency curve for screen

    Screen(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~Screen (){};
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    virtual void   ConnectionsChanged();

    virtual void   ConfigureJoins();
    virtual void   EvalJoinPressures(long JoinMask);
    //virtual flag   EvalFlowEquations(CSpPropInfo *pProps, int IONo, int FE);
    virtual void   EvalProducts(long JoinMask);

    virtual dword  ModelStatus();
  protected:
  private:
  };

//===========================================================================

#undef DllImportExport

#endif
