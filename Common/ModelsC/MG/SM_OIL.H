//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __SM_OIL_H
#define  __SM_OIL_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif  
#ifndef __SP_CONT_H
  #include "sp_cont.h"
#endif  

#ifdef _SM_OIL_CPP
  #define DllImportExport DllExport
#else
  #define DllImportExport 
#endif

#define SM_OIL_STATS           01
#define SM_OIL_WITHFLASHTEST   01

// ===========================================================================
//
//                            Model of Oil and Water
//
// ===========================================================================

const double RGas      = 8.3143;     //kcg was 8314.3
const double LnExpMax  = 690.0;

const double MinZValue = 1.0e-10;

const double Ca = 0.427480233;                      /* 1 / (9*(2**(1/3) - 1)) */
const double Cb = 0.08664034995;                    /* (2**(1/3) - 1) / 3 */

#if SM_OIL_STATS
class SM_OilCnt
  {
  public:
    int            CnvKCall, CnvKIter;   
    int            FugKCall, FugKIter;   
    int            RacRicIter, RacRicCall; 
    int            GundRootIter, GundRootCall; 
    void           Reset()
      {
      CnvKCall=CnvKIter=0;   
      FugKCall=FugKIter=0;   
      RacRicIter=RacRicCall=0; 
      GundRootIter=GundRootCall=0; 
      }
    void           Write(double T, double P)
      {
      dbgp("T/P:%8.2f %8.1f",T,P);
      dbgp(" CnvK:FugK:Rac:GRoot");
      dbgp("%4i%4i :",CnvKCall,CnvKIter);   
      dbgp("%6i%6i :",FugKCall,FugKIter);   
      dbgp("%6i%6i :",RacRicCall,RacRicIter); 
      dbgp("%6i%6i :",GundRootCall,GundRootIter); 
      dbgpln("");
      }
  };
#endif

//----------------------------------------------------------------------------

const int MaxSMOilSets=40;
const double SMBigDistance=1.0e30;

class SM_OilState
  {
  public:
    CDVector       K;                  // Estimates of outlet K values     
    double         VFrac;              // The current Vapour fracion

    SM_OilState() { VFrac=0.5; };

    flag ValidState() { return K.GetLen()>0; };
    void LoadFrom(SM_OilState & O)
      {
      K=O.K;
      VFrac=O.VFrac;
      };
    
    flag SaveTo(SM_OilState & O)
      {
      O.K=K;
      O.VFrac=VFrac;
      return True;
      };
  };

//----------------------------------------------------------------------------

class SM_OilStateSet : public SM_OilState
  {
  public:
    int            Age;
    double         Ts;
    double         Ps;

    SM_OilStateSet() { Age=0; Ps=Ts=-10.0; } // Impossible Values

    void SetTP(double T, double P)
      {
      Ts=T;
      Ps=P;
      };
    flag Initialised() { return Ts>0.0; };

    double Distance(double T, double P)
      {
      if (fabs(Ts-T)>1.0)
        return SMBigDistance;
      if (fabs(Ps-P)>10.0)
        return SMBigDistance;
      return Sqr(T-Ts)+Sqr(0.2*(P-Ps));
      }
  };

//----------------------------------------------------------------------------

class QPMemory
  {
  public:
    double       Temp;
//    SM_OilState  Init, Final; // Memory
  };

//----------------------------------------------------------------------------

DEFINE_SPMODEL(SM_Oil)
class SM_Oil : public SpModel, public SM_OilState 
  {
  DEFINE_SPARES(SM_Oil)
  public:
  
    SM_Oil(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach);
    ~SM_Oil();

    virtual bool   IsBaseClassOf(pSpModel pOther) 
      { return (dynamic_cast<SM_Oil*>(pOther)) != NULL; };

    SM_OilStateSet Sets[MaxSMOilSets];

  protected:
    static flag    fIdealFlashOnly;

    double QTLast, QPLast;
    flag   Qflashdone; 

    int            ZLen;
    
    flag           LoadZused;          // Initialised False in contructor then True in LoadZused
    flag           InitRqd;            // Initialised True in contructor then flase after converge.LoadZ can make it True
    double         Zvap;               // Compressibility vapour - set by ConvergeKratios
    double         Zliq;               // Compressability liquid - set by ConvergeKratios

    CDVector       Z;                  // Feed mole fractions

    CDVector       X;                  // Outlet liquid mole fractions     
    CDVector       Y;                  // Outlet vapour mole fractions     
    CDVector       A;                  // 'a' coefficients     
    CDVector       B;                  // 'b' coefficients     
    CDVector       S;                  // 'S' coefficients     
    CDVector       Alpha;              // 'ALPHA' coefficients     
    //CDMatrix       ALA;                // Mixing Coefficients     
    double         Tsetting;           // The temperature at which constants above were calculeted
    
    QPMemory       QPm;                // Memory for speed of qpflash;
                   
    double         H2OLiq, H2OVap;

    void           BuildDataDefn_Vars(DataDefnBlk & DDB, SPM_View View);
    
    void           SetConstantTcalcs(double T);
    void           IdealGasKratio(double T, double P);
    int            Fugacity (flag Vapour, SpMArray & PHI,double T,double P, CDVector& XorY, double& Z);
    double         FugacityKratio(double T, double P, double Damp);
    void           SetXYfromK();
    //int            SRKNR ( double& Z, double A1, double A0 );
    double         GundersonRoot(double A1,double A0,double StartZ);
    void           Gunderson(double& Q,double& R,double& A,double& B,int IPH,double& Z );
    
    int            RachRice_Solve();
    double         RachRice_Value(double Vf, double &Deriv, double &LfNew, double &VfNew);

    // ACF Stuff
    double         RacRic();
    double         DRacRic();
    void           FindBounds(double &Left, double &Right);
    void           ConvergeVFrac(flag NoGuess);
    double         FindZValue(flag Vapour, double B, double C, double D, CubeRoots Roots, int NRoots, double &AMix, double &BMix);

  public:

    double         CpCv(PhMask PhaseM=som_ALL) { return 1.27; }; //M.CpCv(PhaseM__); };
    double         TPFlash(double T1, double P1, double &Duty);
    QPF_Modes      QPFlash(double P1, double Duty, flag SaturationLimit, double *pOverDuty);
    QPF_Modes      QPFlash(pSpModel pVap, double P1, double Duty, flag SaturationLimit, double *pOverDuty);
     
    void           LoadZ();
    SM_OilStateSet *FindBestSet(double Temp_, double Pres_);
    SM_OilStateSet *FindOldSet(double Temp_, double Pres_);
    flag           ConvergeKratios(pchar Tag, double Temp_, double Pres_);//, SM_OilStateSet * SS);
    void           SetMassAndTemp(double Temp_);
                  
    // debug stuff start
    void           DbgDump(pchar dbgTag);

    #if SM_OIL_WITHFLASHTEST
    static flag    DoFlashTest;
    void           FlashTestGundersonValues();
    void           FlashTest();
                   // Routine to compare kenwalt and gunderson flashing
                   // Needs at least 8 species to be run. Does not checking
    #endif

    #if SM_OIL_STATS
    SM_OilCnt      Cnt;
    #endif

  };

// ===========================================================================

#undef DllImportExport 

#endif
