//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __SQSSA_H
#define  __SQSSA_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif 
#ifndef __SP_MODEL_H
  #include "sp_model.h"
#endif 
#include "..\..\..\SMDK\Include\md_PSD.h"

#ifdef __SQSSA_CPP
  #define DllImportExport DllExport
#elif !defined(SizeDst1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

DEFINE_TAGOBJ(CSzSSA);//, SQTagObjClass);

const byte SSA_Independent = 0;
const byte SSA_FromPSD     = 1;

class DllImportExport CSzSSA : public SpQuality, public MISSA
  {
  //friend class SQSzDist1Edt;
  //friend class CSD_Distribution;

  DEFINE_SPARES(CSzSSA)

  public:
    //CArray <CSD_Distribution*, CSD_Distribution* > Distributions;
    //flag           fAllowSizeEdits;
    //int            iRqdDistOff,iRqdDistOn;
    //int            iDistUsed; //index of distribution used

    CSzSSA(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CSzSSA();

    DEFINE_QUALITYPTR(CSzSSA, "SzSSA");

    //void           AllocDist(int d);
    //void           FreeDist(int d);
    //void           ChangeToDist(int d);

    //Does Nothing: virtual void   BuildDataDefn_Lcl(DataDefnBlk & DDB);
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    //flag           CalculateResults(int d, int DataId, int SpId);

    void           OnDataChange()
      {
      //for (int d=0; d<NDistributions(); d++)
      //  if (Distributions[d])
      //    {
      //    Distributions[d]->iCurrentDataId=-1;
      //    Distributions[d]->iCurrentSpId=-1;
      //    }
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

    virtual double PartDiamFromSAM();
    virtual double PartNumPerSec();
    virtual double PartNumPerLiter();
    virtual double SpecificSurfaceAreaVol();
    virtual double SpecificSurfaceAreaMass();
    virtual void   SetSAM(double SAM, bool Calculated);
    virtual void   SetSAMFromFlow(double THAFlow, double NoPerSec);
    virtual void   SetSAMFromPartDiam(double D);
    virtual void   SetSAMfromPSD(long npts, double x[], double psd[]);

    //virtual double BoundSodaFrac();
    //virtual double BoundSodaOrgFrac();

    virtual double SolidsMassFlow();
    virtual double SolidsDens();
    virtual double LiquorVolume();

  protected:

    CSD_Distribution* GetDist();
    //long              GetSpInx();

  public:
    // Extras for Tear Management;
    virtual void   TearGetInputs(TearVarArray & TV, int n);
    virtual void   TearGetOutputs(TearVarArray & TV, int n);
    virtual void   TearSetOutputs(TearVarArray & TV, int n);

    // Debug Stuff
    virtual void   Dump();

    flag    DataSettable() { return SQCreator() && m_iMethod!=SSA_FromPSD; }
/*#ifndef _RELEASE
  private:
    CSzSSA &     operator=(const CSzSSA & S);
#endif*/
  protected:
    long            m_iSolids;
    long            m_iInclusion;
    //flag    m_bSetSAM;
    //flag            m_bSAMCalcd;
    double          m_dSAM; // Surface Area 
    byte            m_iMethod;
    bool            m_bHasPSD;
    //bool            m_bAllowSet;

    int ID; //temp for debug

    DEFINE_CI(CSzSSA, SpQuality, 4);
  };


// ===========================================================================
//
// Virtual Base class for Bayer type Streams
//
// ===========================================================================
//

//class DllImportExport SMBayerRqdComp
//  {
//  public:
//    double  m_Temp;     // Temperature
//    double  m_rh;       // mass hydrate/mass slurry ratio
//    double  m_rl;       // mass liquor /mass slurry ratio
//    double  m_ra;       // mass Alumina/mass slurry ratio
//    double  m_rc;       // mass Na2O/mass slurry ratio
//    double  m_rCl;      // mass NaCl/mass slurry ratio
//    double  m_rF;       // mass NaF/mass slurry ratio
//    double  m_rCO3;     // mass Na2CO3/mass slurry ratio
//    double  m_rSO4;     // mass Na2SO4/mass slurry ratio
//    double  m_rSi;      // mass SiO2(impurity)/mass slurry ratio
//    double  m_rFe;      // mass Fe2O3(impurity)/mass slurry ratio
//    double  m_rOX;      // mass Na2C2O4/mass slurry ratio
//    double  m_rOC;      // mass other carbon/mass slurry ratio
//    double  m_rSoda;    // mass soda/mass slurry ratio
//
//    SMBayerRqdComp();
//  };
//
//#define TestBayerForWater 1
//
//const word xidPartSAL         = 35001;
//const word xidPartDiam        = 35002;
//const word xidPartNoperSec    = 35003;
//const word xidPartNoperVol    = 35004;
//const word xidPartTHAQm       = 35005;
//const word xidPartBndSoda     = 35006;
//const word xidPartBndSodaOrg  = 35007;
//
//class DllImportExport SMBayerBase : public SpModelEx, public MIBayer
//  {
//  protected:
//    flag    m_bSetSAM;
//    flag    m_bSAMCalcd;
//    double  m_dSAM; // Surface Area Measured
//  public:
//    SMBayerBase(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach);
//    virtual ~SMBayerBase();
//
//    // Local Quality
//
//    virtual void   BuildDataDefn_PartSize(DataDefnBlk & DDB, eScdSVView View);
//    virtual flag   DataXchg(DataChangeBlk & DCB);
////    virtual flag   ValidateData(ValidateDataBlk & VDB)=0;
//
//    //virtual double PartDiamFromSAM();
//    //virtual double PartNumPerSec();
//    //virtual double PartNumPerLiter();
//    //virtual double SpecificSurfaceAreaVol();
//    //virtual double SpecificSurfaceAreaMass();
//    //virtual void   SetSAM(double SAM, bool Calculated);
//    //virtual void   SetSAMFromFlow(double THAFlow, double NoPerSec);
//    //virtual void   SetSAMFromPartDiam(double D);
//    //virtual void   SetSAMfromPSD(long npts, double x[], double psd[]);
//
//    //virtual double CausticConc(double T_)=0;
//    //virtual double AluminaConc(double T_)=0;
//    //virtual double SodaConc(double T_)=0;
//    //virtual double SodiumCarbonateConc(double T_)=0;
//    //virtual double SodiumSulphateConc(double T_)=0;
//    //virtual double SodiumOxalateConc(double T_)=0;
//    //virtual double SolidsConc(double T_)=0;
//    //virtual double TOOC(double T_)=0;
//    //virtual double TOC(double T_)=0;
//    //virtual double FreeCaustic(double T_)=0;
//    //virtual double AtoC()=0;
//    //virtual double CtoS()=0;
//    //virtual double CltoC()=0;
//    //virtual double TOOCtoC()=0;
//    //virtual double Na2SO4toC()=0;
//
//    //Jan 2002 : new virtuals for PSD...
//    // for PSD Precip
//    //virtual double OX(double T_)=0;
//    //virtual double OC(double T_)=0;
//    //virtual void   DumpState()=0;
//
//    //virtual double SodiumFlourideConc(double T_)=0;
//    //virtual double QuartzConc(double T_)=0;
//    //virtual double HeamatiteConc(double T_)=0;
//
//    virtual double BoundSodaFrac()=0;
//    virtual double BoundSodaOrgFrac()=0;
//    //... end for new virtuals for PSD
//
//    //virtual double GrowthRate()                         { return 0; };
//    //virtual double AgglomerationRate()                  { return 0; };
//    //virtual double NucleationRate(double SpecSurface)   { return 0; };
//
//    ////virtual methods for general precip / SAM model...
//    //virtual double THAMassFlow()=0;
//    //virtual double THADens(double T_)=0;
//    //virtual double AluminaConcSat(double T_)=0;
//    //virtual double OxalateEquilibrium(double T_)=0;
////    virtual double PerformAluminaSolubility(double TRqd, double ARqd, double THARqd, double NoPerSec, double Na2OFac, double SSat, flag & ConvergeErr)=0;
//
//  protected:
//    // Local Qualities Overides
//    virtual void LQExchangeSpecies(SpModel *Other);
//    virtual void LQZeroMass();
//    virtual void LQZeroDeriv();
//    virtual void LQScaleMass(PhMask PhaseM, double Mult);
//    virtual void LQSetMassF(CSysVector &M1, SpModel *Other);
//    virtual void LQAddMassF(CSysVector &M1, SpModel *Other, CSysVector &M2);
//    virtual void LQSubMassF(CSysVector &M1, SpModel *Other, CSysVector &M2);
//    virtual void LQCopy(pSpModel pMdl2);
//    virtual void LQAddDeriv(SpModel *Other, double Sgn_);
//    virtual void LQAddDiscrete(SpModel *Other, double Sgn_);
//    virtual void LQODEOperate(CODEDataBlock & ODB);
//    virtual void LQEquilibrate();
//
//  public:
//    // ConditionBlk Override
//    // DEFINE_CI(SMBayer, SMSteam, 4);
//  };
//
// ==========================================================================
//
//                                  End
//
// ==========================================================================

#undef DllImportExport

#endif
